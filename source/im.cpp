#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "uox3.h"
#include "debug.h"

#undef DBGFILE
#define DBGFILE "im.cpp"

#define Free(_) { if( _ ) free( _ ); }

// Static internal structure. List of menus waiting for an answer.
// waiting_status is 0 if free, 1 if occupied. firstfree points to
// the first free slot.
#define WAIT_MAX 256 // must be a power of two
static item_menu *waiting_menus[WAIT_MAX];
static char waiting_status[WAIT_MAX];
static int waiting_firstfree = -1;

//o---------------------------------------------------------------------------o
//|	Function	-	item_menu *im_create( char * title, im_callback callback )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Initializes and returns a pointer to a new item_menu with
//|					the string title as title. callback will be called when
//|					an item is selected.
//o---------------------------------------------------------------------------o
item_menu *im_create( const char *title, im_callback callback )
{
	item_menu *new_menu = new item_menu;

	if( waiting_firstfree == -1 )
	{
		// First time a menu is created. Initialize internals.
		waiting_firstfree = 0;
		memset( waiting_status, 0, WAIT_MAX );
	}

	if( new_menu == NULL ) // shouldn't happen. :)
		return NULL;

	if( title == NULL )    
		new_menu->title = strdup( "" );
	else
		new_menu->title = strdup( title );

	new_menu->callback = callback;
	new_menu->nitems = 0;
	// Reserve space for prefix + len + text + # items
	new_menu->total_bytes = 9 + 1 + strlen( new_menu->title ) + 1;  
	new_menu->gump_message = NULL;

	return new_menu;
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool im_additem( item_menu *menu, int id, int param, char *title )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Adds a new item to the menu with the specified id, param
//|					and title. param will be passed via callback when an item
//|					an item is selected.
//o---------------------------------------------------------------------------o
bool im_additem( item_menu *menu, int id, long param, char *title ) 
{
	if( menu == NULL )
		return false;

	if( menu->nitems >= IM_MAX_SIZE )
		return false;

	menu->items[menu->nitems].id = id;
	menu->items[menu->nitems].param = param;
	if( title )
		menu->items[menu->nitems].title = strdup( title );
	else
		menu->items[menu->nitems].title = strdup( "" );

	// Reserve space for gmmiddle + lenght + title
	menu->total_bytes += 5 + strlen( menu->items[menu->nitems].title );

	menu->nitems++;

	return true;
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool im_close( item_menu *menu )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Closes menu and generates gump message for later sending.
//o---------------------------------------------------------------------------o
bool im_close( item_menu *menu )
{
	if( menu == NULL )
		return false;

	if( menu->nitems == 0 )
		return false;

	//menu->gump_message = (UI08 *)malloc( menu->total_bytes );		// new (UI08)[menu->total_bytes] ??? Abaddon
	menu->gump_message = new UI08 [menu->total_bytes];

	// Gump prefix
	menu->gump_message[0] = 0x7C;
	// Total bytes
	menu->gump_message[1] = (UI08)(menu->total_bytes >> 8);
	menu->gump_message[2] = (UI08)(menu->total_bytes & 0xFF);
	// message [3 .. 6] -> character serial (set by im_send)
	// message [7 .. 8] -> special code (set by im_send)
	// Title length
	menu->gump_message[9] = strlen( menu->title );
	// Title
	memcpy( menu->gump_message + 10, menu->title, menu->gump_message[9] );
	int curr;
	curr = 10 + menu->gump_message[9];

	// Number of items
	menu->gump_message[curr++] = menu->nitems;

	int ctl;
	for( int i = 0; i < menu->nitems; ++i )
	{
		// item id
		menu->gump_message[curr++] = (UI08)(menu->items[i].id >> 8);
		menu->gump_message[curr++] = (UI08)(menu->items[i].id & 0xFF);
		menu->gump_message[curr++] = 0;
		menu->gump_message[curr++] = 0;
		// item title length
		ctl = strlen( menu->items[i].title );
		menu->gump_message[curr++] = ctl;
		// item title
		memcpy( menu->gump_message + curr, menu->items[i].title, ctl );
		curr += ctl;
	}
	return true;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void im_send( item_menu *menu, cSocket *player )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Sends menu to the client. Response will come via callback.
//o---------------------------------------------------------------------------o
void im_send( item_menu *menu, cSocket *player )
{
	if( menu == NULL )
		return;

	if( menu->gump_message == NULL )
		return;

	// Player's character's serial number
	CChar *mChar = player->CurrcharObj();
	menu->gump_message[3] = mChar->GetSerial( 1 );
	menu->gump_message[4] = mChar->GetSerial( 2 );
	menu->gump_message[5] = mChar->GetSerial( 3 );
	menu->gump_message[6] = mChar->GetSerial( 4 );
	// Special code for IM_ menu, read by choice()
	menu->gump_message[7] = (UI08) ~ ( (WAIT_MAX-1) >> 8 ); 
	// Index for waiting table (internal) */
	menu->gump_message[8] = (UI08)( waiting_firstfree & WAIT_MAX );
	if( waiting_firstfree == WAIT_MAX )
	{
		Console << "[IM]im_send: Uh-oh. All waiting slots are busy" << myendl;
		return;
	}	
	// Add this menu to the wait list.

	waiting_status[waiting_firstfree] = 1;
	waiting_menus[waiting_firstfree] = menu;
	// Find next free menu.
	while( waiting_status[waiting_firstfree] == 1 && waiting_firstfree < WAIT_MAX )
		waiting_firstfree++;

	player->Send( menu->gump_message, menu->total_bytes );
}  

//o---------------------------------------------------------------------------o
//|	Function	-	bool im_choice( cSocket *player, int menu_code, int option )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Function called by choice to tell which menu option was selected.
//o---------------------------------------------------------------------------o
bool im_choice( cSocket *player, int menu_code, int option )
{
	CChar *mChar = player->CurrcharObj();
	if( mChar == NULL )
		return false;

	// discard leftmost part of code (which are 1's)
	menu_code &= menu_code & (WAIT_MAX-1);

	if( option == 0 )
	{
		// User right-clicked, cancelling the menu.
		mChar->SetMaking( 0 );
		return true;
	}

	if( waiting_status[menu_code] == 0 )
		return false;

	// Update status and firstfree
	waiting_status[menu_code] = 0;
	if( waiting_firstfree > menu_code )
		waiting_firstfree = menu_code;

	// Call callback
	waiting_menus[menu_code]->callback( waiting_menus[menu_code], player, waiting_menus[menu_code]->items[option-1].param );

	return true;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void im_kill( item_menu *menu )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Deletes the memory used by menu.
//o---------------------------------------------------------------------------o
void im_kill( item_menu *menu )
{
	if( menu == NULL )
		return;

	delete [] menu->title;
	delete [] menu->gump_message;
}

//   ************************   \\
//  Start of .GMP menu section  \\
//   ************************   \\

// menulist_node: node in linked list of loaded menus.
typedef struct _menulist_node 
{
	item_menu *menu;
	char *name;
	im_menucallback callback;
	struct _menulist_node *next;
} menulist_node;

static menulist_node *menu_list;

static char imbuffer[256];
static long buf_number;
static enum 
{
	TK_NUMBER,
	TK_IDENT,
	TK_STRING,
	TK_SYMB,
	TK_EOF,
	TK_ERROR
} curr_token;

typedef struct _menu_option_info 
{
	menulist_node *parent;
	char is_final; // 1 if param is valid, 0 if name.
	union 
	{ 
		char *option_name;
		long param;
	} data;
} menu_option_info;

//o---------------------------------------------------------------------------o
//|	Function	-	static UI08 gettoken( FILE *fmenu )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Places the next command string in buffer
//o---------------------------------------------------------------------------o
static UI08 gettoken( FILE *fmenu )
{
	buf_number = 0;
	int c, i;
	c = getc( fmenu );
  
	while( 1 )
	{
		switch( c )
		{
		case '/':
			c = getc( fmenu );

			if( c != '/' )
			{
				curr_token = TK_ERROR;
				return 0;  // Lexical error
			}
                          
			while( c != '\n' ) // skip line
				c = getc( fmenu );

			c = getc( fmenu );
			break;

		case '\n':
		case '\t':
		case '\r':
		case ' ':
			c = getc( fmenu );
			break;

		case EOF:
			curr_token = TK_EOF;
			return static_cast<UI08>(EOF);

		case ',':
		case ';':
		case '=':
		case '{':
		case '}':
			imbuffer[0] = c;
			imbuffer[1] = '\0';
			curr_token = TK_SYMB;
			return 1;

		case '"':
			i = 0;
          
			c = getc( fmenu );
			while( c != '"' )
			{
				if( c == EOF )
				{
					curr_token = TK_ERROR;
					return 0;
				}
				imbuffer[i++] = c;
				c = getc( fmenu );
			}
			imbuffer[i] = '\0';
			curr_token = TK_STRING;
			return 1;
        
		default:
			if( isdigit( c ))
			{ // Number
				int inhex = 0;
				i = 0;

				if( c == '0' )
				{
					c = getc( fmenu );
					if( c == 'x' || c == 'X' )
					{
						inhex = 1;
						c = getc( fmenu );
					}
				}

				// if it is a digit or hex is allowed and it is valid..
				while( isdigit( c ) || ( inhex && toupper(c) >= 'A' && toupper(c) <= 'F' ))
				{
					imbuffer[i++] = c;
					c = getc( fmenu );
				}
				imbuffer[i] = '\0';
          
				ungetc( c, fmenu );
				if( inhex )
					sscanf( imbuffer, "%lx", &buf_number );
				else
					sscanf( imbuffer, "%ld", &buf_number );

				imbuffer[0] = '\0';
				curr_token = TK_NUMBER;

			}
			else if( isalpha( c ) || c == '_' )
			{ // Ident
				i = 0;
				while( isalnum( c ) || c == '_' )
				{
					imbuffer[i++] = c;
					c = getc( fmenu );
				}
				ungetc( c, fmenu );
				imbuffer[i] = '\0';
				curr_token = TK_IDENT;
			}
			else
				return 0;
			return 1;
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	static void menus_callback( item_menu *menu, cSocket *player, long item_param )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Callback used by the menu system.
//o---------------------------------------------------------------------------o
static void menus_callback( item_menu *menu, cSocket *player, long item_param )
// PARAM WARNING: menu is never referenced
{
	menu_option_info *moi = (menu_option_info *)item_param;

	if( moi->is_final )
		moi->parent->callback( moi->parent->name, player, moi->data.param );
	else
		im_sendmenu( moi->data.option_name, player );
}

//o---------------------------------------------------------------------------o
//|	Function	-	static UI08 read_menu( FILE *fmenu, im_menucallback callback )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Reads the next menu, setting it with callback.
//o---------------------------------------------------------------------------o
static UI08 read_menu( FILE *fmenu, im_menucallback callback )
{
	int icon;

	if( curr_token != TK_IDENT )
		return 0;

	// node = (menulist_node *)malloc( sizeof( menulist_node ));		// new menulist_node ???? Abaddon
	menulist_node *node = new menulist_node;

	if( node == NULL )
		return 0;

//  node->name = strdup( imbuffer );
	node->name = new char[strlen( imbuffer ) + 1];
	strcpy( node->name, imbuffer );
	node->callback = callback;
	gettoken( fmenu ); // read '='
	if( curr_token != TK_SYMB && imbuffer[0] != '=' )
	{
		delete node;
		return 0;
	}
	gettoken( fmenu ); // read '{'
	if( curr_token != TK_SYMB && imbuffer[0] != '{' )
	{
		delete node;
		return 0;
	}

	gettoken( fmenu ); // read title
	if( curr_token != TK_STRING )
	{
		delete node;
		return 0;
	}
  
	node->menu = im_create( imbuffer, menus_callback );
	if( node->menu == NULL )
	{
		delete node;
		return 0;
	}

	gettoken( fmenu ); // read ';'
	if( curr_token != TK_SYMB && imbuffer[0] != ';' )
	{
		delete node;
		return 0;
	}
	while( 1 ) {
		menu_option_info *moi;

		gettoken( fmenu ); // get icon or '}'
		if( curr_token == TK_SYMB && imbuffer[0] == '}' )
			break;

		if( curr_token != TK_NUMBER )
		{
			delete node;
			return 0;
		}
		icon = buf_number;

		gettoken( fmenu ); // get ','
		if( curr_token != TK_SYMB && imbuffer[0] != ',' )
		{
			delete node;
			return 0;
		}

		gettoken( fmenu ); // read title
		if( curr_token != TK_STRING )
		{
			delete node;
			return 0;
		}
    
		//moi = (menu_option_info *)malloc( sizeof( menu_option_info ));		// new menu_option_info ??? Abaddon
		moi = new menu_option_info;

		if( moi == NULL ) // This REALLY shouldn't happen.
		{
			delete node;
			return 0; 
		}    
		moi->parent = node;

		if( !im_additem( node->menu, icon, (long)moi, imbuffer ) )
		{
			delete node;
			delete moi;
			return 0;
		}
		gettoken( fmenu ); // get ','
		if( curr_token != TK_SYMB && imbuffer[0] != ',' )
		{
			delete node;
			delete moi;
			return 0;
		}
		gettoken( fmenu ); // read ident or number
		if( curr_token == TK_IDENT )
		{
			moi->data.option_name = strdup( imbuffer );
			moi->is_final = 0;
		}
		else if( curr_token == TK_NUMBER )
		{
			moi->data.param = buf_number;
			moi->is_final = 1;
		}
		else 
		{
			delete node;
			delete moi;
			return 0;
		}
		gettoken( fmenu ); // read ';'
		if( curr_token != TK_SYMB && imbuffer[0] != ';' )
		{
			delete node;
			delete moi;
			return 0;
		}
	}
	if( !im_close( node->menu ) )
	{
		delete node;
		return 0;
	}
	node->next = menu_list;
	menu_list = node;
	return 1;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void im_loadmenus( char *file_name, im_menucallback callback )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Loads and prepares all the menus found in the gump file file_name.
//o---------------------------------------------------------------------------o
void im_loadmenus( const char *file_name, im_menucallback callback )
{
	if( file_name == NULL )
		return;

	char real_file_name[MAX_PATH];
	sprintf(real_file_name, "%s%s", cwmWorldState->ServerData()->GetGumpsDirectory(), file_name);
	FILE *fmenu = fopen( real_file_name, "rt" );
	if( fmenu == NULL )
	{
		Console.Error( 1, "Failed to open gump file %s for reading", file_name );
		return;
	}

	imbuffer[0] = '\0';
	buf_number = 0;

	int result = gettoken( fmenu );
	while( result == 1 )
	{
		result = read_menu( fmenu, callback );
		if( !result )
		{
			fclose(fmenu);
			return;
		}
		result = gettoken( fmenu );
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void im_clearmenus()
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Frees all internal memory used in menus.
//o---------------------------------------------------------------------------o
void im_clearmenus()
{
	menulist_node *p = menu_list, *next;

	while( p )
	{
		next = p->next;
		im_kill( p->menu );
		delete p;
		p = next;
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void im_sendmenu( char *menu_name, cSocket *player )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Sends the menu referenced by menu_name, previously loaded via loadmenus.
//o---------------------------------------------------------------------------o
void im_sendmenu( const char *menu_name, cSocket *player )
{
	menulist_node *p = menu_list;
	while( p )
	{
		if( !strcmp( p->name, menu_name )) // Found it.
		{
			im_send( p->menu, player );
			return;
		}

		p = p->next;
	}
}


