//------------------------------------------------------------------------
//  im.h
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
 * im.h
 *   Pedro "Kathrrak" Rabinovitch <miller@tecgraf.puc-rio.br>
 * Item menu header file.
 */

#define IM_MAX_SIZE 40 /* Max number of menu options */

typedef struct _item_menu item_menu;

/* void im_callback( item_menu *menu, int player, long item_param )
 *
 * Callback called by im when an item is selected from the
 * menu. States menu, player, and parameter.
 */
typedef void (*im_callback)( item_menu *menu, int player, long item_param );

/* item_menu structure */
struct _item_menu {
  char *title;           /* Menu title */
  im_callback callback;  /* Callback function */

  struct im_item {
    int id;              /* id (used for picture) */
    long param;          /* parameter (for callback) */
    char *title;         /* option name */
  } items[IM_MAX_SIZE];  /* List of menu options */
  
  int nitems;

  unsigned char *gump_message; /* byte block for message */
  int total_bytes; /* total bytes = prefix + len + text + # items + ... */
                           /* ... + middle + len + text (for each item) */

};

/*
 * item_menu *im_create( char * title )
 *
 *   Initializes and returns a pointer to a new item_menu
 *   with the string title as title. callback will be called
 *   when an item is selected.
 *   Returns NULL if there's any problem, menu otherwise.
 */
item_menu *im_create( char * title, im_callback callback);

/*
 * int im_additem( item_menu *menu, int id, int param, char *title )
 *
 *   Adds a new item to the menu with the specified id, param
 *   and title. param will be passed via callback when an item 
 *   is selected.
 *   Returns 0 if succesful, error code otherwise.
 */
int im_additem( item_menu *menu, int id, long param, char *title );

/*
 * int im_close( item_menu *menu )
 *
 *   Closes menu and generates gump message for later sending.
 *   Returns 0 if succesful, error code otherwise.
 */
int im_close( item_menu *menu );

/*
 * int im_send( item_menu *menu, int player )
 *
 *   Sends menu to the client. Response will come via callback.
 *   Returns 0 if succesful, error code otherwise.
 */
int im_send( item_menu *menu, int player );

/*
 * int im_choice( int player, int menu_code, int option )
 *
 *   Function called by choice to tell which menu option was selected.
 *   Returns 0 if succesful, error code otherwise.
 */
int im_choice( int player, int menu_code, int option );

/*
 * int im_kill( item_menu *menu )
 *
 *   Deletes the memory used by menu.
 *   Returns 0 if succesful, error code otherwise.
 */
int im_kill( item_menu *menu );

/* void im_menucallback( char *menu_name, int player, long item_param )
 *
 * Callback called by im when an item is selected from the
 * menu. States menu, player, and parameter.
 */
typedef void (*im_menucallback)( char *menu_name, int player, long item_param );

/*
 * int im_loadmenus( char *file_name )
 *
 *   Loads and prepares all the menus found in the gump file file_name.
 *   Returns 0 if succesful, error code otherwise.
 */
int im_loadmenus( char *file_name, im_menucallback callback );

/*
 * void im_clearmenus()
 *
 *   Frees all internal memory used in menus.
 */
void im_clearmenus();

/*
 * int im_sendmenu( char *menu_name, int player )
 *
 *   Sends the menu referenced by menu_name, previously loaded via loadmenus.
 *   Returns 0 if succesful, error code otherwise.
 */
int im_sendmenu( char *menu_name, int player );


