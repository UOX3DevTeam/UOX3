//------------------------------------------------------------------------
//  books.h
//
//------------------------------------------------------------------------
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 2000 - 2001 by Nikolaus Gertsmayr (Lord Binary)
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
// Book class by Lord Binary, 7'th december 1999

// new books readonly -> morex  999
// new books writeable -> morex 666
// old books from misc.scp, readonly: neither type 666 nor 999, section# = more-value
// of course ALL books need a type of 11

// note to everyone who wants to learn C++ -> this is NOT stricly OOP and has no good OOD.
// hence no real good example for OO-P :)
// for example inheritence could be used, public attributes arnt nice etc,etc.


#ifndef __books_h
#define __books_h

class cBooks
{
	private:
		
		// that private methods read and write from the *.bok files
		void write_author(int id,UOXSOCKET s);
		void write_title(int id, UOXSOCKET s);
		void write_line(int id, int page, int line, char linestr[34], UOXSOCKET s);
		
		void read_author(int id,char auth[31]);
		void read_title(int id,char title[61]);
		int  read_number_of_pages(int id);
		void read_line(int id, int page,int linenumber, char line[33]);

		char make_new_book_file(char *fileName, int id); // "formats and creates a new bok file"
					
	public:
		char a_t; // flag -> set if author and title changed		
		char authorbuffer[MAXCLIENT][32]; 
		char titlebuffer[MAXCLIENT][62];
		char pagebuffer[MAXCLIENT][512]; //i think 256 is enough (8 lines *32 chars per line = 256, but i took 512 to be on the safe side and avoid crashes 
	
	    cBooks();

        void openbook_old(UOXSOCKET s, ITEM i); // opens old-readonly books, takes data from misc.scp
		
		void openbook_new(UOXSOCKET s, ITEM i,char writeable); // opens new books

		void readbook_readonly_old(UOXSOCKET s, ITEM i, int p); // reads books from misc.scp, readonly = old books
		                                                        
		void readbook_readonly(UOXSOCKET s, ITEM i, int p);     // reads new books readonly ( from *.bok file )
		                                                     		                                                      
		void readbook_writeable(UOXSOCKET s, ITEM i, int p, int l); // writes changes to a new book opened in writable mode 

		void delete_bokfile(int id); // deletes bok-file.

		virtual ~cBooks();
        	
};
#endif
