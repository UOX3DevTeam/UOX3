//------------------------------------------------------------------------
//  im.cpp
//
//------------------------------------------------------------------------
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 1997 - 2001 by Pedro Rabinovitch (Kathrrak)
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//	
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//   
//------------------------------------------------------------------------
/*
 * im.cpp
 *   Pedro "Kathrrak" Rabinovitch <miller@tecgraf.puc-rio.br>
 * Item menu implementation.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "uox3.h"
#include "debug.h"

#define DBGFILE "im.cpp"

#define Free(_) { if( _ ) free( _ ); }

/* Static internal structure. List of menus waiting for an answer.
   waiting_status is 0 if free, 1 if occupied. firstfree points to
   the first free slot. */
#define WAIT_MAX 256 /* must be a power of two */
static item_menu *waiting_menus[WAIT_MAX];
static char waiting_status[WAIT_MAX];
static int waiting_firstfree = -1;

/*
 * item_menu *im_create( char * title )
 *
 *   Initializes and returns a pointer to a new item_menu
 *   with the string title as title. callback will be called
 *   when an item is selected.
 */
item_menu *im_create( char * title, im_callback callback)
{

	//item_menu *new_menu = (item_menu *)malloc(sizeof(item_menu));		// new item_menu ???? Abaddon
	item_menu *new_menu = new item_menu;

	if( waiting_firstfree == -1 )
	{
		/* First time a menu is created. Initialize internals. */
		waiting_firstfree = 0;
		memset( waiting_status, 0, WAIT_MAX );
	}

	if( new_menu == NULL ) /* shouldn't happen. :) */
		return NULL;

	if( title == NULL )    
		new_menu->title = strdup( "" );
	else
		new_menu->title = strdup( title );

	new_menu->callback = callback;
	new_menu->nitems = 0;
	/* Reserve space for prefix + len + text + # items */
	new_menu->total_bytes = 9 + 1 + strlen( new_menu->title ) + 1;  
	new_menu->gump_message = NULL;

	return new_menu;
}

/*
 * int im_additem( item_menu *menu, int id, int param, char *title )
 *
 *   Adds a new item to the menu with the specified id, param
 *   and title. param will be passed via callback when an item 
 *   is selected.
 *   Returns 0 if succesful, error code otherwise.
 */
int im_additem( item_menu *menu, int id, long param, char *title ) 
{
	if( menu == NULL )
		return -1;

	if( menu->nitems >= IM_MAX_SIZE )
		return -2;

	menu->items[menu->nitems].id = id;
	menu->items[menu->nitems].param = param;
	if( title )
		menu->items[menu->nitems].title = strdup( title );
	else
		menu->items[menu->nitems].title = strdup( "" );

	/* Reserve space for gmmiddle + lenght + title */
	menu->total_bytes += 5 + strlen( menu->items[menu->nitems].title );

	menu->nitems++;

	return 0;
}

/*
 * int im_close( item_menu *menu )
 *
 *   Closes menu and generates gump message for later sending.
 *   Returns 0 if succesful, error code otherwise.
 */
int im_close( item_menu *menu )
{
	int i, curr;

	if( menu == NULL )
		return -1;

	if( menu->nitems == 0 )
		return -3;

	//menu->gump_message = (unsigned char *)malloc( menu->total_bytes );		// new (unsigned char)[menu->total_bytes] ??? Abaddon
	menu->gump_message = new unsigned char[menu->total_bytes];

	/* Gump prefix */
	menu->gump_message[0] = 0x7C;
	/* Total bytes */
	menu->gump_message[1] = (unsigned char)(menu->total_bytes >> 8);
	menu->gump_message[2] = (unsigned char)(menu->total_bytes & 0xFF);
	/* message [3 .. 6] -> character serial (set by im_send) */  
	/* message [7 .. 8] -> special code (set by im_send) */  
	/* Title length */
	menu->gump_message[9] = strlen( menu->title );
	/* Title */
	memcpy( menu->gump_message + 10, menu->title, menu->gump_message[9] );

	curr = 10 + menu->gump_message[9];

	/* Number of items */
	menu->gump_message[curr++] = menu->nitems;

	for( i=0; i < menu->nitems; i++ )
	{
		int ctl;
		/* item id */
		menu->gump_message[curr++] = (unsigned char)(menu->items[i].id >> 8);
		menu->gump_message[curr++] = (unsigned char)(menu->items[i].id & 0xFF);
		menu->gump_message[curr++] = 0;
		menu->gump_message[curr++] = 0;
		/* item title length */
		ctl = strlen( menu->items[i].title );
		menu->gump_message[curr++] = ctl;
		/* item title */
		memcpy( menu->gump_message + curr, menu->items[i].title, ctl );
		curr += ctl;
	}

	return 0;
}

/*
 * int im_send( item_menu *menu, int player )
 *
 *   Sends menu to the client. Response will come via callback.
 *   Returns 0 if succesful, error code otherwise.
 */
int im_send( item_menu *menu, int player )
{
	if( menu == NULL )
		return -1;

	if( menu->gump_message == NULL )
		return -4;

	/* Player's character's serial number */
	menu->gump_message[3] = chars[currchar[player]].ser1;
	menu->gump_message[4] = chars[currchar[player]].ser2;
	menu->gump_message[5] = chars[currchar[player]].ser3;
	menu->gump_message[6] = chars[currchar[player]].ser4;
	/* Special code for IM_ menu, read by choice() */
	menu->gump_message[7] = (unsigned char) ~ ( (WAIT_MAX-1) >> 8 ); 
	/* Index for waiting table (internal) */
	menu->gump_message[8] = (unsigned char)( waiting_firstfree & WAIT_MAX );
	if( waiting_firstfree == WAIT_MAX )
	{
		printf( "[IM]im_send: Uh-oh. All waiting slots are busy.\n" );
		return -5;
	}	
	/* Add this menu to the wait list. */

	waiting_status[waiting_firstfree] = 1;
	waiting_menus[waiting_firstfree] = menu;
	/* Find next free menu. */
	while( waiting_status[waiting_firstfree] == 1 && waiting_firstfree < WAIT_MAX )
		waiting_firstfree++;

	/* Fire away! */
	Network->xSend( player, menu->gump_message, menu->total_bytes, 0 );

	/* ... and we're done. Now sit and wait for im_choice() to be called. */
	return 0;
}  

/*
 * int im_choice( int player, int menu_code, int option )
 *
 *   Function called by choice to tell which menu option was selected.
 *   Returns 0 if succesful, error code otherwise.
 */
int im_choice( int player, int menu_code, int option )
{
	/* discard leftmost part of code (which are 1's) */
	menu_code &= menu_code & (WAIT_MAX-1);

	if( option == 0 )
	{
		/* User right-clicked, cancelling the menu. Right? */
	
		 chars[currchar[player]].making=0;
	     return 0; /** Lord binary **/
	}

	if( waiting_status[menu_code] == 0 )
		/* No menus here with that code, no sir! */
		return -1;

	/* Update status and firstfree */
	waiting_status[menu_code] = 0;
	if( waiting_firstfree > menu_code )
		waiting_firstfree = menu_code;

	/* Call callback */
	waiting_menus[menu_code]->callback( 
    waiting_menus[menu_code],                       /* Menu */
    player,                                         /* Player */
    waiting_menus[menu_code]->items[option-1].param /* parameter */
	);

	/* Ok, now it's his problem. */
	return 0;
}

/*
 * int im_kill( item_menu *menu )
 *
 *   Deletes the memory used by menu.
 *   Returns 0 if succesful, error code otherwise.
 */
int im_kill( item_menu *menu )
{
	if( menu == NULL )
		return -1;

	//Free( menu->title );
	delete menu->title;
	//Free( menu->gump_message );
  delete menu->gump_message;
	//Free( menu );
  //delete menu;

	return 0;
}

/*   ************************   */
/*  Start of .GMP menu section  */
/*   ************************   */

/* menulist_node: node in linked list of loaded menus. */
typedef struct _menulist_node {
  item_menu *menu;
  char *name;
  im_menucallback callback;
  struct _menulist_node *next;
} menulist_node;

static menulist_node *menu_list;

static char imbuffer[256];
static long buf_number;
static enum {
  TK_NUMBER,
  TK_IDENT,
  TK_STRING,
  TK_SYMB,
  TK_EOF,
  TK_ERROR
} curr_token;

typedef struct _menu_option_info {
  menulist_node *parent;
  char is_final; /* 1 if param is valid, 0 if name. */
  union { 
    char *option_name;
    long param;
  } data;
} menu_option_info;

/** static int gettoken( FILE *fmenu )
 *  Places the next command string in buffer.
 *  Returns EOF if file is at the end, 1 if lexical error or 0 if ok.
 */
static int gettoken( FILE *fmenu )
{
  int c;

  buf_number = 0;
  c = getc( fmenu );
  
  while( 1 ) {
    switch( c ) {
      case '/':
        c = getc( fmenu );

        if( c != '/' ) {
          curr_token = TK_ERROR;
          return 1;  /* Lexical error */
        }
                          
        while( c != '\n' ) /* skip line */
          c = getc( fmenu );

        c = getc( fmenu );
        break;

      case '\n': case '\t':
      case '\r': case ' ':
        c = getc( fmenu );
        break;

      case EOF:
        curr_token = TK_EOF;
        return EOF;

      case ',': case ';': case '=':
      case '{': case '}':
        imbuffer[0] = c;
        imbuffer[1] = '\0';
        curr_token = TK_SYMB;
        return 0;

      case '"':
        {
          int i = 0;
          
          c = getc( fmenu );
          while( c != '"' ) {
            if( c == EOF ) {
              curr_token = TK_ERROR;
              return 1;
            }

            imbuffer[i++] = c;
            c = getc( fmenu );
          }
          imbuffer[i] = '\0';
          curr_token = TK_STRING;
          return 0;
        }
        
      default:
        if( isdigit( c )) { /* Number */
          int inhex = 0;
          int i = 0;

          if( c == '0' ) {
            c = getc( fmenu );
            if( c == 'x' || c == 'X' ) {
              inhex = 1;
              c = getc( fmenu );
            }
          }

          /* if it is a digit or hex is allowed and it is valid... */
          while( isdigit( c ) || 
                 ( inhex && toupper(c) >= 'A' && toupper(c) <= 'F' )) {
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

        } else if( isalpha( c ) || c == '_' ) { /* Ident */
          int i = 0;
          while( isalnum( c ) || c == '_' ) {
            imbuffer[i++] = c;
            c = getc( fmenu );
          }
          ungetc( c, fmenu );
          imbuffer[i] = '\0';
          curr_token = TK_IDENT;

        } else
          return 1;

        return 0;
    }
  } 
}

/** static void menus_callback( item_menu *menu, int player, long item_param )
 *  Callback used by the menu system.
 */
static void menus_callback( item_menu *menu, int player, long item_param )
// PARAM WARNING: menu is never referenced
{
  menu_option_info *moi = (menu_option_info *)item_param;

  if( moi->is_final )
    moi->parent->callback( moi->parent->name, player, moi->data.param );
  else
    im_sendmenu( moi->data.option_name, player );
}

/** static int read_menu( FILE *fmenu, im_menucallback callback )
 *  Reads the next menu, setting it with callback.
 *  Returns 0 if succesful, error code otherwise.
 */
static int read_menu( FILE *fmenu, im_menucallback callback )
{
  menulist_node *node;
  int icon;

  if( curr_token != TK_IDENT )
    return -1;

  //node = (menulist_node *)malloc( sizeof( menulist_node ));		// new menulist_node ???? Abaddon
  node = new menulist_node;

  if( node == NULL ) /* God, I'm boring. :)) */
    return -2;

//  node->name = strdup( imbuffer );
  node->name = new char[strlen( imbuffer ) + 1];
  strcpy( node->name, imbuffer );
  node->callback = callback;

  gettoken( fmenu ); /* read '=' */
  if( curr_token != TK_SYMB && imbuffer[0] != '=' )
  {
    delete node;
    return -3;
  }

  gettoken( fmenu ); /* read '{' */
  if( curr_token != TK_SYMB && imbuffer[0] != '{' )
  {
    delete node;
    return -4;
  }

  gettoken( fmenu ); /* read title */
  if( curr_token != TK_STRING )
  {
    delete node;
    return -5;
  }
  
  node->menu = im_create( imbuffer, menus_callback );
  if( node->menu == NULL )
  {
    delete node;
    return -6;
  }

  gettoken( fmenu ); /* read ';' */
  if( curr_token != TK_SYMB && imbuffer[0] != ';' )
  {
    delete node;
    return -7;
  }

  while( 1 ) {
    menu_option_info *moi;

    gettoken( fmenu ); /* get icon or '}' */
    if( curr_token == TK_SYMB && imbuffer[0] == '}' )
      break;

    if( curr_token != TK_NUMBER )
    {
      delete node;
      return -8;
    }
    icon = buf_number;

    gettoken( fmenu ); /* get ',' */
    if( curr_token != TK_SYMB && imbuffer[0] != ',' )
    {
      delete node;
      return -9;
    }

    gettoken( fmenu ); /* read title */
    if( curr_token != TK_STRING )
    {
      delete node;
      return -10;
    }
    
    //moi = (menu_option_info *)malloc( sizeof( menu_option_info ));		// new menu_option_info ??? Abaddon
	  moi = new menu_option_info;

    if( moi == NULL ) /* This REALLY shouldn't happen. */
    {
      delete node;
      return -11; 
    }    
    moi->parent = node;

    if( im_additem( node->menu, icon, (long)moi, imbuffer ))
    {
      delete node;
      delete moi;
      return -12;
    }
    gettoken( fmenu ); /* get ',' */
    if( curr_token != TK_SYMB && imbuffer[0] != ',' )
    {
      delete node;
      delete moi;
      return -13;
    }
    gettoken( fmenu ); /* read ident or number */
    if( curr_token == TK_IDENT ) {
      moi->data.option_name = strdup( imbuffer );
      moi->is_final = 0;
    } else if( curr_token == TK_NUMBER ) {
      moi->data.param = buf_number;
      moi->is_final = 1;
    } else 
    {
      delete node;
      delete moi;
      return -14;
    }
    gettoken( fmenu ); /* read ';' */
    if( curr_token != TK_SYMB && imbuffer[0] != ';' )
    {
      delete node;
      delete moi;
      return -15;
    }
  }

  if( im_close( node->menu ))
  {
    delete node;
    return -16;
  }
  node->next = menu_list;
  menu_list = node;

//  delete node;	// I THINK this is a premature deletion
  return 0;
}
        

/*
 * int im_loadmenus( char *file_name, im_menucallback callback )
 *
 *   Loads and prepares all the menus found in the gump file file_name.
 *   Returns 0 if succesful, error code otherwise.
 */
int im_loadmenus( char *file_name, im_menucallback callback )
{
  FILE *fmenu;
  int result;

  if( file_name == NULL )
    return -1;

  fmenu = fopen( file_name, "rt" );
  if( fmenu == NULL )
    return -2;

  imbuffer[0] = '\0';
  buf_number = 0;

  result = gettoken( fmenu );

  while( result == 0 ) {
    result = read_menu( fmenu, callback );
    if( result != 0 )
      return result;
    result = gettoken( fmenu );
  }

  if( result == EOF )
    return 0;
  else
    return result;
}

/*
 * void im_clearmenus()
 *
 *   Frees all internal memory used in menus.
 */
void im_clearmenus()
{
  menulist_node *p = menu_list, *next;

  while( p ) {
    next = p->next;
    im_kill( p->menu );
    //Free( p );
    delete p;
    p = next;
  }
}

/*
 * int im_sendmenu( char *menu_name, int player )
 *
 *   Sends the menu referenced by menu_name, previously loaded via loadmenus.
 *   Returns 0 if succesful, error code otherwise.
 */
int im_sendmenu( char *menu_name, int player )
{
  menulist_node *p = menu_list;

  while( p ) {
    if( !strcmp( p->name, menu_name )) /* Found it. */
      return im_send( p->menu, player );

    p = p->next;
  }

  return -66;
}


