//------------------------------------------------------------------------
//  books.cpp
//
//------------------------------------------------------------------------
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 2000 - 2001 by Nikolaus Gerstmayr (Lord Binary)
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
// Books class by Lord Binary 7'th December 1999
// Implements writable books

#include "uox3.h"
#include "books.h"

cBooks::cBooks()//Constructor
{
}

cBooks::~cBooks()//Destructor
{
}

// opens old (readonly) books == old, bugfixed readbook function
void cBooks::openbook_old(UOXSOCKET s, ITEM i)
{
	char bookopen[10]="\x93\x40\x01\x02\x03\x00\x00\x00\x02"; //LB 7'th dec 1999, making it client 1.26 complaint
	char booktitle[61] = { "\x00", };
	char bookauthor[31] = { "\x00", };
	
	openscript("misc.scp");
	sprintf(temp, "BOOK %i", calcserial( items[i].more1, items[i].more2, items[i].more3, items[i].more4 ) );
	if( !i_scripts[misc_script]->find( temp ) )
	{
		closescript();
		return;
	}
	bookopen[1] = items[i].ser1;
	bookopen[2] = items[i].ser2;
	bookopen[3] = items[i].ser3;
	bookopen[4] = items[i].ser4;
	do
	{
		read2();
	}
	while( strcmp( script1, "PAGES" ) );
	bookopen[8] = str2num( script2 ); // LB, bugfixing old code
	do
	{
		read2();
	}
	while( strcmp( script1, "TITLE" ) );
	strcpy( booktitle, script2 );
	do
	{
		read2();
	}
	while( strcmp( script1, "AUTHOR" ) );
	strcpy( bookauthor, script2 );
	Network->xSend( s, bookopen, 9, 0 ); // LB, bugfixing of old code
	Network->xSend( s, booktitle, 60, 0 );
	Network->xSend( s, bookauthor, 30, 0 );
	closescript();
}

// opens new books
// writeable 1 -> open new books in writable mode
//           0 -> open new books in readonly mode
void cBooks::openbook_new(UOXSOCKET s, ITEM i, char writeable)
{
	char bookopen[10]=   "\x93\x40\x01\x02\x03\x01\x01\x00\x02"; //LB 7'th dec 1999, making it client 1.26 complaint
	
	char bookopen_ro[10]="\x93\x40\x01\x02\x03\x00\x01\x00\x02";

	int a,b,c,pages,bytes;
	char line[33];
	char buch[256][8][33];
	char fileName[13];

	bool bookexists = false;

	FILE *file;

	// I dont know what that new client 1.26 byte does, but it needs to be set to 1 or 2 or writing doesnt work
    // wild guess: rune books ...

	char booktitle[61] = { "\x00", }; 
	char bookauthor[31] = { "\x00", };

	sprintf( fileName, "%02x%02x%02x%02x.bok", i>>24, i>>16, i>>8, i%256);
    file = fopen( fileName, "r+b"); // open existing file for read/write

	bookexists = ( file != NULL );
	
	if( bookexists )
	{
		fclose( file );
		read_author ( i, bookauthor ); // fetch author if existing
		read_title  ( i, booktitle  ); // fetch title if existing
		pages = read_number_of_pages(i); 
	}
	else 
	{ 
		pages = items[i].morey;              // if new book get number of maxpages ...
		if( pages < 1 || pages > 255 ) 
			pages = 16;
	}

	int q = 0;
	// clear all buffers from previous book openings
	memset( &authorbuffer[s], '~', 32 );
	memset( &titlebuffer[s], '~', 62 );
	memset( &pagebuffer[s], '~', 511 );

	bookopen[1] = bookopen_ro[1] = items[i].ser1;
	bookopen[2] = bookopen_ro[2] = items[i].ser2;
	bookopen[3] = bookopen_ro[3] = items[i].ser3;
	bookopen[4] = bookopen_ro[4] = items[i].ser4;

	bookopen[7] = bookopen_ro[7] = pages>>8; 
	bookopen[8] = bookopen_ro[8] = pages%256;  

	if (writeable) Network->xSend(s, bookopen, 9, 0); else 
	{ 
		Network->xSend(s, bookopen_ro, 9, 0);
	}

	Network->xSend(s, booktitle, 60, 0);
	Network->xSend(s, bookauthor, 30, 0);
	
	if (!bookexists) return; // dont send book contents if the file doesnt exist (yet)!

	if (!writeable) return; // if readonly book return !!

    //////////////////////////////////////////////////////////////
	// Now we HAVE to send the ENTIRE Book                     / /
	// Cauz in writeable mode the client only sends out packets  /
	// if something  gets changed                                /
	// this also means -> for each bookopening in writeable mode /
	// lots of data has to be send.                              /
	//////////////////////////////////////////////////////////////

	char bookpage_pre[10]="\x66\x01\x02\x40\x01\x02\x03\x00\x01";
	char bookpage[5]="\x00\x00\x00\x08";

    // we have to know the total size "in advance"
	// thats why i save the book data in a temporaray array to 
	// avoid reading it twice from (slow) hd

	bytes=9;
	for (a=1;a<=pages;a++)
	{
	 bytes+=4; // 4 bytes for each page
     for (b=1;b<=8;b++)
	 {
        read_line(i, a,b, line);
		c=strlen(line)+1;
        //printf("%s l: %i\n",line,c);
		strcpy(buch[a-1][b-1],line);
		bytes+=c; // plus the stringlength+null terminator per(!) row
	 }
	}

	bookpage_pre[1]=bytes>>8;
	bookpage_pre[2]=bytes%256;
	bookpage_pre[3]=items[i].ser1;
	bookpage_pre[4]=items[i].ser2;
	bookpage_pre[5]=items[i].ser3;
	bookpage_pre[6]=items[i].ser4;
	bookpage_pre[7]=pages>>8;
	bookpage_pre[8]=pages%256;

	Network->xSend(s, bookpage_pre, 9, 0);

	for (a=1;a<=pages;a++)
	{

	  bookpage[0]=a>>8;
	  bookpage[1]=a%256;

	  Network->xSend(s, bookpage, 4, 0);

	  for (int j=0;j<8;j++)
	  {
		Network->xSend(s, buch[a-1][j], strlen(buch[a-1][j])+1, 0);
	  }
	}
}

// sends a page of new readonly book to the client
void cBooks::readbook_readonly(UOXSOCKET s, ITEM i, int p) 
{

    char bookpage[14]="\x66\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x08";
	int bytes=0,a,c;
	char seite[8][33];
	char fileName[13];
	char line[33];
	FILE *file;

	sprintf( fileName, "%02x%02x%02x%02x.bok", i>>24, i>>16, i>>8, i%256);
    file = fopen( fileName, "r+b"); // open existing file for read/write

    if (file==NULL) return;
	fclose(file);

	bytes=13;
	for (a=1;a<=8;a++)
	{
	   read_line(i, p,a,line);
	   c=strlen(line)+1;        
	   strcpy(seite[a-1],line);
	   bytes+=c;
	}
    
	bookpage[1]=bytes>>8;
	bookpage[2]=bytes%256;

	bookpage[3]=items[i].ser1;
	bookpage[4]=items[i].ser2;
	bookpage[5]=items[i].ser3;
	bookpage[6]=items[i].ser4;

	bookpage[9]=p>>8;
	bookpage[10]=p%256;

	Network->xSend(s, bookpage, 13, 0);

	for (int j=0;j<8;j++)
	{
		Network->xSend(s, seite[j], strlen(seite[j])+1, 0);
	}
	
}


// old readbook function
void cBooks::readbook_readonly_old(UOXSOCKET s, ITEM i, int p)
{

	int x, y, pos, j;
	char bookpage[14]="\x66\x01\x02\x40\x01\x02\x03\x00\x01\x00\x01\x00\x01";
	
	openscript("misc.scp");
	sprintf(temp, "BOOK %i",
		(items[i].more1<<24)+(items[i].more2<<16)+(items[i].more3<<8)+items[i].more4);
	if (!i_scripts[misc_script]->find(temp))
	{
		closescript();
		return;
	}
	x=p;
	do
	{
		do
		{
			read2();
		}
		while (strcmp(script1, "PAGE"));
		x--;
	}
	while (x>0);
	closescript();
	openscript("misc.scp");
	sprintf(temp, "PAGE %s", script2);
	if (!i_scripts[misc_script]->find(temp))
	{
		closescript();
		return;
	}
	pos=ftell(scpfile);
	x=-1;
	y=-2;
	do
	{
		read1();
		x++;
		y+=strlen(script1)+1;
	}
	while (strcmp(script1, "}"));
	y+=13;
	fseek(scpfile, pos, SEEK_SET);
	bookpage[1]=y>>8;
	bookpage[2]=y%256;
	bookpage[3]=items[i].ser1;
	bookpage[4]=items[i].ser2;
	bookpage[5]=items[i].ser3;
	bookpage[6]=items[i].ser4;
	bookpage[9]=p>>8;
	bookpage[10]=p%256;
	bookpage[11]=x>>8;
	bookpage[12]=x%256;
	Network->xSend(s, bookpage, 13, 0);
	for (j=0;j<x;j++)
	{
		read1();
		Network->xSend(s, script1, strlen(script1)+1, 0);
	}
	closescript();
}

// writes changes to a writable book to the bok file.		
void cBooks::readbook_writeable(UOXSOCKET s, ITEM i, int p, int l)
{
 int ii=0,lines_processed=0,lin=0;
 char line[34],ch;

 if (a_t) write_title(i,s); // if title was changed by writer write the changes "down"
 if (a_t) write_author(i,s); // if author was changed by writer write the changes "down" to the bok-file
 
 while (lines_processed<l)
 {
   if (ii>511) lines_processed=l; // avoid crash if client sends out inconsitent data
   ch=pagebuffer[s][ii];
   if (lin<33) line[lin]=ch; else line[33]=ch;
   ii++;
   lin++;
   if (ch==0) 
   {
	   lines_processed++;
	   lin=0;

	   //printf("page: %i\n",p);

       write_line(i, p, lines_processed, line,s);       

	   //printf("author: %s title: %s line: %i :%s\n",authorbuffer[s],titlebuffer[s],lines_processed,line);
   }
   
 }
 
 a_t=0; // dont re-write author and title if not necassairy

}

// private methods here

// writes the author into the corresponding-bok file
// PRE: packets 0x93 needs to be send by client BEFORE its called. 
// (and its data copied to the authorbuffer)

void cBooks::write_author(int id,UOXSOCKET s)
{
  FILE *file;
  char fileName[13];  // Standard 8.3 file name
  int newbook=0,Offset;

  //printf("write-auth called\n");

  sprintf( fileName, "%02x%02x%02x%02x.bok", id>>24, id>>16, id>>8, id%256);
  file = fopen( fileName, "r+b"); // open existing file for read/write

  
  if (file == NULL) // If the BOK file does not exist -> that book must be new
	                // or the file got deleted -> lets try to create it
  {
	  newbook=1;	 
	  if (make_new_book_file(fileName,id)==-1) 
	  {
		  return;
	  }

      file = fopen( fileName, "r+b"); // open existing file for read/write (now it should exist)
	  if (file==NULL)                 
	  {
		  printf("couldnt write to bok file\n");
		  return;
	  }
  }
 
  Offset=62; // position filepointer to the author-place
  if ( fseek(file, Offset, SEEK_SET) ) printf("failed to seek to bok file\n");
     
  authorbuffer[s][31]='\n';

  if ( fwrite(authorbuffer[s], sizeof(char), 32, file) != 32 ) 
  {
	 printf("coudnt write to book file\n");
	 return;
  }			

  fclose(file);

}


void cBooks::write_title(int id,UOXSOCKET s)
{
  FILE *file;
  char fileName[13];  // Standard 8.3 file name
  int newbook=0,Offset;

  sprintf( fileName, "%02x%02x%02x%02x.bok", id>>24, id>>16, id>>8, id%256);
  file = fopen( fileName, "r+b"); // open existing file for read/write

  
  if (file == NULL) // If the BOK file does not exist -> that book must be new
	                // or the file got deleted -> lets try to create it
  {
	  newbook=1;	 
	  if (make_new_book_file(fileName,id)==-1) 
	  {
		  return;
	  }

      file = fopen( fileName, "r+b"); // open existing file for read/write (now it should exist)
	  if (file==NULL)                 
	  {
		  printf("couldnt write to bok file\n");
		  return;
	  }
  }
 
  Offset=0; // position filepointer to the title-place
  if ( fseek(file, Offset, SEEK_SET) ) printf("failed to seek to bok file\n");
     
  titlebuffer[s][61]='\n';

  if ( fwrite(titlebuffer[s], sizeof(char), 62, file) != 62 ) 
  {
	 printf("coudnt write to book file\n");
	 return;
  }			

  fclose(file);

}

void cBooks::write_line(int id, int page, int line, char linestr[34], UOXSOCKET s)
{

  //printf("id: %i page: %i line: %i linestr: %s sock: %i\n",id,page,line,linestr,s);

  FILE *file;
  char fileName[13];  // Standard 8.3 file name
  int newbook=0,Offset;

  sprintf( fileName, "%02x%02x%02x%02x.bok", id>>24, id>>16, id>>8, id%256);
  file = fopen( fileName, "r+b"); // open existing file for read/write
  
  if (file == NULL) // If the BOK file does not exist -> that book must be new
	                // or the file got deleted -> lets try to create it
  {
	  newbook=1;	 
	  if (make_new_book_file(fileName,id)==-1) 
	  {
		  return;
	  }

      file = fopen( fileName, "r+b"); // open existing file for read/write (now it should exist)
	  if (file==NULL)                 
	  {
		  printf("couldnt write to bok file\n");
		  return;
	  }
  }
 
  Offset=273*page+34*line-207; // wohoooo, what a neat geeky formula :)

  if ( fseek(file, Offset, SEEK_SET) ) printf("failed to seek to bok file\n");
     
  linestr[33]='\n';

  if ( fwrite(linestr, sizeof(char), 34, file) != 34 ) 
  {
	 printf("coudnt write to book file\n");
	 return;
  }			

  fclose(file);

}

void cBooks::read_author(int id,char auth[31])
{

  FILE *file;
  char fileName[13];  
  int Offset;

  sprintf( fileName, "%02x%02x%02x%02x.bok", id>>24, id>>16, id>>8, id%256);
  file = fopen( fileName, "r+b"); // open existing file for read/write
  
  if (file == NULL) 
  {
	  printf("couldnt read bok file\n");
	  return;
  }
	        
  Offset=62; // position filepointer to the author-place
  if ( fseek(file, Offset, SEEK_SET) ) printf("failed to seek to bok file\n");
       
  if ( fread(auth, sizeof(char), 31, file) != 31 )  // read it
  {
	 printf("coudnt write to book file\n");
	 return;
  }
	
  // clear garbage after strign termination
  int end=0; 
  for (int a=0;a<31;a++)
  {
    if (auth==0) end=1;
	if (end) auth[a]=0;
  }
  
  fclose(file);

}

	
void cBooks::read_title(int id,char title[61])
{

  FILE *file;
  char fileName[13];  
  int Offset;

  sprintf( fileName, "%02x%02x%02x%02x.bok", id>>24, id>>16, id>>8, id%256);
  file = fopen( fileName, "r+b"); // open existing file for read/write
  
  if (file == NULL) 
  {
	  printf("couldnt read bok file\n");
	  return;
  }
	        
  Offset=0; // position filepointer to the title place
  if ( fseek(file, Offset, SEEK_SET) ) printf("failed to seek to bok file\n");
       
  if ( fread(title, sizeof(char), 61, file) != 61 )  // read it
  {
	 printf("coudnt write to book file\n");
	 return;
  }
	
  // clear garbage after strign termination
  int end=0; 
  for (int a=0;a<61;a++)
  {
    if (title==0) end=1;
	if (end) title[a]=0;
  }
  
  fclose(file);

}

int cBooks::read_number_of_pages(int id)
{
  FILE *file;
  char fileName[13];  
  int Offset;
  char num[5];

  sprintf( fileName, "%02x%02x%02x%02x.bok", id>>24, id>>16, id>>8, id%256);
  file = fopen( fileName, "r+b"); // open existing file for read/write
  
  if (file == NULL) 
  {
	  printf("couldnt read bok file\n");
	  return 1;
  }
	        
  Offset=94; // position filepointer to the number of pages place
  if ( fseek(file, Offset, SEEK_SET) ) printf("failed to seek to bok file\n");
       
  if ( fread(num, sizeof(char), 5, file) != 5 )  // read it
  {
	 printf("coudnt write to book file\n");
	 return 1;
  }
	
  // clear garbage after string termination
  int end=0, a; 
  for(a = 0; a < 5; a++ )
  {
    if (num==0) end=1;
	if (end) num[a]=0;
  }
  
  fclose(file);

  a=str2num(num);
  if (a<1 || a>255) return 255;
  return a;

}
	

// page+linumber=1 indexed ! (as returned from client)
void cBooks::read_line(int id, int page,int linenumber, char line[33])
{
  FILE *file;
  char fileName[13];  
  int Offset;

  sprintf( fileName, "%02x%02x%02x%02x.bok", id>>24, id>>16, id>>8, id%256);
  file = fopen( fileName, "r+b"); // open existing file for read/write
  
  if (file == NULL) 
  {
	  printf("couldnt read bok file\n");
	  return;
  }
	        

  Offset=273*page+34*linenumber-207; // wohoooo, what a neat geeky formula :)
  if ( fseek(file, Offset, SEEK_SET) ) printf("failed to seek to bok file\n");
       
  if ( fread(line, sizeof(char), 33, file) != 33 )  // read it
  {
	 printf("coudnt write to book file\n");
	 return;
  }
	
  // clear garbage after strign termination
  int end=0; 
  for (int a=0;a<33;a++)
  {
    if (line==0) end=1;
	if (end) line[a]=0;
  }
  
  fclose(file);
}

void cBooks::delete_bokfile(int id)
{
  char fileName[13];    
  sprintf( fileName, "%02x%02x%02x%02x.bok", id>>24, id>>16, id>>8, id%256);
  remove(fileName);

}

// "Formats" a newly created book-file
// This NEEDS to be done with ANY new book file.
// 
char cBooks::make_new_book_file(char *fileName, int id)
{
  FILE *file;

  file = fopen( fileName, "w+b"); // create new file
  int i,maxpages;
  char ch;
  char author[33];
  char title[63];
  char line[35];
  char num[5];

  if (file == NULL) 
  {
	printf("cant create new book file\n");
	return -1;
  }

  author[0]='.';author[1]=0;author[31]='\n';
  title[0]='.';title[1]=0;title[61]='\n';
  line[0]='.';line[1]=0;line[33]='\n';

  for (i=2;i<=60;i++) title[i]=32;
  for (i=2;i<=30;i++) author[i]=32;
  for (i=2;i<=32;i++) line[i]=32;
 
  if ( fwrite(&title, sizeof(char), 62, file) != 62 ) 
  {
	printf("coudnt write to book file\n");
	return -1;
  }

  if ( fwrite(&author, sizeof(char), 32, file) != 32 ) 
  {
	printf("coudnt write to book file\n");
	return -1;
  }
  
  
  maxpages=items[id].morey; // morey=maxpages
  if (maxpages<1 || maxpages>255) maxpages=16; // default = 16 pages

  numtostr(maxpages,num); // danger,danger, didnt this cause probs under LINUX ???
                          // sorry, i cant test ???

  num[4]='\n';

  if ( fwrite(num, sizeof(char), 5, file) != 5 )  // writens number
  {
	printf("coudnt write to book file\n");
	return -1;
  }

  for (int j=0;j<maxpages;j++) // page loop
  {
	ch='\n'; // each page gets a cr
    if ( fwrite(&ch, sizeof(char), 1, file) != 1 ) 
	{
	  printf("coudnt write to book file\n");
	  return -1;
	}
	
    for (int l=0;l<8;l++) // each page has 8 lines
	{	         
		line[0]=0;
        if ( fwrite(&line, sizeof(char), 34, file) != 34 ) 
		{
	       printf("coudnt write to book file\n");
	       return -1;
		}			
	}

  }

  fclose(file);
  return 0;
}
 
void readbook(int s, int i, int p) // Book window
{
	int x, y, pos, j;
	char bookpage[14]="\x66\x01\x02\x40\x01\x02\x03\x00\x01\x00\x01\x00\x01";
	
	openscript("misc.scp");
	sprintf(temp, "BOOK %i",
		(items[i].more1<<24)+(items[i].more2<<16)+(items[i].more3<<8)+items[i].more4);
	if (!i_scripts[misc_script]->find(temp))
	{
		closescript();
		return;
	}
	x=p;
	do
	{
		do
		{
			read2();
		}
		while (strcmp(script1, "PAGE"));
		x--;
	}
	while (x>0);
	closescript();
	openscript("misc.scp");
	sprintf(temp, "PAGE %s", script2);
	if (!i_scripts[misc_script]->find(temp))
	{
		closescript();
		return;
	}
	pos=ftell(scpfile);
	x=-1;
	y=-2;
	do
	{
		read1();
		x++;
		y+=strlen(script1)+1;
	}
	while (strcmp(script1, "}"));
	y+=13;
	fseek(scpfile, pos, SEEK_SET);
	bookpage[1]=y>>8;
	bookpage[2]=y%256;
	bookpage[3]=items[i].ser1;
	bookpage[4]=items[i].ser2;
	bookpage[5]=items[i].ser3;
	bookpage[6]=items[i].ser4;
	bookpage[9]=p>>8;
	bookpage[10]=p%256;
	bookpage[11]=x>>8;
	bookpage[12]=x%256;
	// Network->xSend(s, pause, 2, 0);
	Network->xSend(s, bookpage, 13, 0);
	for (j=0;j<x;j++)
	{
		read1();
		Network->xSend(s, script1, strlen(script1)+1, 0);
	}
	// Network->xSend(s, restart, 2, 0);
	closescript();
}

void openbook(int s, int i)
{
	char bookopen[9]="\x93\x40\x01\x02\x03\x00\x00\x02";
	char booktitle[61] = { "\x00", };
	char bookauthor[31] = { "\x00", };
	
	openscript("misc.scp");
	sprintf(temp, "BOOK %i",
		(items[i].more1<<24)+(items[i].more2<<16)+(items[i].more3<<8)+items[i].more4);
	if (!i_scripts[misc_script]->find(temp))
	{
		closescript();
		return;
	}
	bookopen[1]=items[i].ser1;
	bookopen[2]=items[i].ser2;
	bookopen[3]=items[i].ser3;
	bookopen[4]=items[i].ser4;
	do
	{
		read2();
	}
	while (strcmp(script1, "PAGES"));
	bookopen[7]=str2num(script2);
	do
	{
		read2();
	}
	while (strcmp(script1, "TITLE"));
	strcpy(booktitle, script2);
	do
	{
		read2();
	}
	while (strcmp(script1, "AUTHOR"));
	strcpy(bookauthor, script2);
	// Network->xSend(s, pause, 2, 0);
	Network->xSend(s, bookopen, 8, 0);
	Network->xSend(s, booktitle, 60, 0);
	Network->xSend(s, bookauthor, 30, 0);
	// Network->xSend(s, restart, 2, 0);
	closescript();
}

