#include "uox3.h"

#define IM_MAX_SIZE 40	 // Max number of menu options

typedef struct _item_menu item_menu;
typedef void (*im_callback)( item_menu *menu, cSocket *player, long item_param );


struct _item_menu 
{
	char *title;           // Menu title
	im_callback callback;  // Callback function

	struct im_item 
	{
		int id;              // id (used for picture)
		long param;          // parameter (for callback)
		char *title;         // option name
	} items[IM_MAX_SIZE];  // List of menu options

	int nitems;

	UI08 *gump_message;	 // byte block for message
	int total_bytes;		 // total bytes = prefix + len + text + # items + middle + len + text (for each item)

};

item_menu *im_create( const char * title, im_callback callback );
bool im_additem( item_menu *menu, int id, long param, char *title );
bool im_close( item_menu *menu );
void im_send( item_menu *menu, cSocket *player );
bool im_choice( cSocket *player, int menu_code, int option );
void im_kill( item_menu *menu );
typedef void (*im_menucallback)( const char *menu_name, cSocket *player, long item_param );
void im_loadmenus( const char *file_name, im_menucallback callback );
void im_clearmenus();
void im_sendmenu( const char *menu_name, cSocket *player );

