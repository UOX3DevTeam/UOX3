// Books class by Lord Binary 7'th December 1999
// Implements writable books

// V0.9 5'th dec, initial version
// V1.0 7'th dec 1999, "wrapped" everything in a class, added customable number of max-pages
// V1.1 12-dec-1999 -> nasty bug fixed (item# used as "key" instead of serial#) and a few very small bugfixes

#include "uox3.h"
#include "books.h"
#include "ssection.h"

cBooks::cBooks()//Constructor
{
	return;
}

cBooks::~cBooks()//Destructor
{

}

// opens old (readonly) books == old, bugfixed readbook function
void cBooks::OpenPreDefBook( cSocket *mSock, CItem *i )
{
	if( mSock == NULL )
		return;

	char temp[1024];
	char bookopen[10] = "\x93\x40\x01\x02\x03\x00\x00\x00\x02";
	char booktitle[61] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
	char bookauthor[31] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
	
	sprintf( temp, "BOOK %i", i->GetMore() );
	ScriptSection *book = FileLookup->FindEntry( temp, misc_def );
	if( book == NULL )
		return;

	const char *data = NULL;
	bookopen[1] = i->GetSerial( 1 );
	bookopen[2] = i->GetSerial( 2 );
	bookopen[3] = i->GetSerial( 3 );
	bookopen[4] = i->GetSerial( 4 );
	bool part1 = false, part2 = false, part3 = false;
	for( const char *tag = book->First(); !book->AtEnd(); tag = book->Next() )
	{
		data = book->GrabData();
		if( !strcmp( tag, "PAGES" ) )
		{
			part1 = true;
			bookopen[8] = (UI08)(makeNum( data ));
		}
		else if( !strcmp( tag, "TITLE" ) )
		{
			part2 = true;
			strcpy( booktitle, data );
		}
		else if( !strcmp( tag, "AUTHOR" ) )
		{
			part3 = true;
			strcpy( bookauthor, data );
		}
		if( part1 && part2 && part3 )
			break;
	}

	mSock->Send( bookopen, 9 );
	mSock->Send( booktitle, 60 );
	mSock->Send( bookauthor, 30 );
}

// opens new books
void cBooks::OpenBook( cSocket *mSock, CItem *i, bool isWriteable )
{
	char bookopen[10]=   "\x93\x40\x01\x02\x03\x01\x01\x00\x02";
	char bookopen_ro[10]="\x93\x40\x01\x02\x03\x00\x01\x00\x02";

	int a, b, c, pages;
	char line[33];
	char buch[256][8][33];
	char fileName[MAX_PATH];

	// I dont know what that new client 1.26 byte does, but it needs to be set to 1 or 2 or writing doesnt work
    // wild guess: rune books ...

	UI08 booktitle[61] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
	UI08 bookauthor[31] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

	sprintf( fileName, "%s/%x.bok", cwmWorldState->ServerData()->GetBooksDirectory(), i->GetSerial() );
  FILE *file = fopen( fileName, "r+b" ); // open existing file for read/write
	bool bookexists = ( file != NULL );
	if( bookexists )
	{
		fclose( file );
		ReadAuthor( i, bookauthor ); // fetch author if existing
		ReadTitle ( i, booktitle  ); // fetch title if existing
		pages = getNumberOfPages( i ); 
	}
	else 
	{ 
		pages = i->GetMoreY();              // if new book get number of maxpages ...
		if( pages < 1 || pages > 255 ) 
			pages = 16;
	}
	mSock->ClearAuthor();
	mSock->ClearTitle();
	mSock->ClearPage();
	
	bookopen[1] = bookopen_ro[1] = i->GetSerial( 1 );
	bookopen[2] = bookopen_ro[2] = i->GetSerial( 2 );
	bookopen[3] = bookopen_ro[3] = i->GetSerial( 3 );
	bookopen[4] = bookopen_ro[4] = i->GetSerial( 4 );

	bookopen[7] = bookopen_ro[7] = (UI08)(pages>>8); 
	bookopen[8] = bookopen_ro[8] = (UI08)(pages%256);

	if( mSock != NULL )
	{
		if( isWriteable ) 
			mSock->Send( bookopen, 9 ); 
		else 
			mSock->Send( bookopen_ro, 9 );

		mSock->Send( booktitle, 60 );
		mSock->Send( bookauthor, 30 );
	}
	
	if( !bookexists ) 
		return; // dont send book contents if the file doesnt exist (yet)!

	if( !isWriteable ) 
		return; // if readonly book return !!

    //////////////////////////////////////////////////////////////
	// Now we HAVE to send the ENTIRE Book                     / /
	// Cauz in writeable mode the client only sends out packets  /
	// if something  gets changed                                /
	// this also means -> for each bookopening in writeable mode /
	// lots of data has to be send.                              /
	//////////////////////////////////////////////////////////////

	UI08 bookpage_pre[10] = "\x66\x01\x02\x40\x01\x02\x03\x00\x01";
	UI08 bookpage[5] = "\x00\x00\x00\x08";

    // we have to know the total size "in advance"
	// thats why i save the book data in a temporaray array to 
	// avoid reading it twice from (slow) hd

	int bytes = 9;
	for( a = 1; a <= pages; a++ )
	{
		bytes += 4; // 4 bytes for each page
		for( b = 1; b <= 8; b++ )
		{
			ReadLine( i, a, b, line );
			c = strlen( line ) + 1;
			strcpy( buch[a-1][b-1], line );
			bytes += c; // plus the string length + null terminator per(!) row
		}
	}

	bookpage_pre[1] = (UI08)(bytes>>8);
	bookpage_pre[2] = (UI08)(bytes%256);
	bookpage_pre[3] = i->GetSerial( 1 );
	bookpage_pre[4] = i->GetSerial( 2 );
	bookpage_pre[5] = i->GetSerial( 3 );
	bookpage_pre[6] = i->GetSerial( 4 );
	bookpage_pre[7] = (UI08)(pages>>8);
	bookpage_pre[8] = (UI08)(pages%256);

	if( mSock != NULL )
	{
		mSock->Send( bookpage_pre, 9 );

		for( a = 1; a <= pages; a++ )
		{
			bookpage[0] = (UI08)(a>>8);
			bookpage[1] = (UI08)(a%256);
			mSock->Send( bookpage, 4 );
			for( UI08 j = 0; j < 8; j++ )
				mSock->Send( buch[a-1][j], strlen( buch[a-1][j] ) + 1 );
		}
	}
}

// sends a page of new readonly book to the client
void cBooks::ReadNonWritableBook( cSocket *s, CItem *i, int p )
{
    UI08 bookpage[14] = "\x66\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x08";
	int c;
	char seite[8][33];
	char fileName[MAX_PATH];
	char line[33];
	
	sprintf( fileName, "%s/%x.bok", cwmWorldState->ServerData()->GetBooksDirectory(), i->GetSerial() );
  FILE *file = fopen( fileName, "r+b"); // open existing file for read/write

    if( file == NULL ) 
	{
		Console.Error( 1, "Failed to open %s", fileName );
		return;
	}
	fclose( file );

	int bytes = 13;
	for( int a = 1; a <= 8; a++ )
	{
		ReadLine( i, p, a, line );
		c = strlen( line ) + 1;        
		strcpy( seite[a-1], line );
		bytes += c;
	}
    
	bookpage[1] = (UI08)(bytes>>8);
	bookpage[2] = (UI08)(bytes%256);

	bookpage[3] = i->GetSerial( 1 );
	bookpage[4] = i->GetSerial( 2 );
	bookpage[5] = i->GetSerial( 3 );
	bookpage[6] = i->GetSerial( 4 );

	bookpage[9] = (UI08)(p>>8);
	bookpage[10] = (UI08)(p%256);

	if( s != NULL )
	{
		s->Send( bookpage, 13 );
		for( UI08 j = 0; j < 8; j++ )
			s->Send( seite[j], strlen( seite[j] ) + 1 );
	}	
}

// old readbook function
void cBooks::ReadPreDefBook( cSocket *mSock, CItem *i, int p )
{
	if( mSock == NULL )
		return;

	char temp[512];

	sprintf( temp, "BOOK %i", i->GetMore() );
	ScriptSection *book = FileLookup->FindEntry( temp, misc_def );
	if( book == NULL )
		return;

	for( const char *tag = book->First(); !book->AtEnd(); tag = book->Next() )
	{
		if( !strcmp( tag, "PAGE" ) )
		{
			sprintf( temp, "PAGE %s", book->GrabData() );
			ScriptSection *page = FileLookup->FindEntry( temp, misc_def );
			if( page == NULL )
				return;

			int x = -1;
			int y = -2;
			for( tag = page->First(); !page->AtEnd(); tag = page->Next() )
			{
				sprintf( temp, "%s %s", tag, page->GrabData() );
				x++;
				y += strlen( temp ) + 1;
			}
			y += 13;

			UI08 bookpage[14] = "\x66\x01\x02\x40\x01\x02\x03\x00\x01\x00\x01\x00\x01";
			bookpage[1] = (UI08)(y>>8);
			bookpage[2] = (UI08)(y%256);
			bookpage[3] = i->GetSerial( 1 );
			bookpage[4] = i->GetSerial( 2 );
			bookpage[5] = i->GetSerial( 3 );
			bookpage[6] = i->GetSerial( 4 );
			bookpage[9] = (UI08)(p>>8);
			bookpage[10] = (UI08)(p%256);
			bookpage[11] = (UI08)(x>>8);
			bookpage[12] = (UI08)(x%256);

			mSock->Send( bookpage, 13 );

			tag = page->First();
			for( int j = 0; j < x; j++ )
			{
				if( page->AtEnd() )
					return;

				sprintf( temp, "%s %s", tag, page->GrabData() );
				mSock->Send( temp, strlen( temp ) + 1 );
				tag = page->Next();
			}
			return;
		}
	}
}

// writes changes to a writable book to the bok file.		
void cBooks::ReadWritableBook( cSocket *s, CItem *i, int p, int l )
{
	int ii = 0, lines_processed = 0, lin = 0;
	char line[34], ch;

	if( changeAT ) 
	{
		WriteTitle( i, s ); // if title was changed by writer write the changes "down"
		WriteAuthor( i, s ); // if author was changed by writer write the changes "down" to the bok-file
	}
 
	if( s == NULL )
		return;
	char *pagebuffer = s->PageBuffer();
	while( lines_processed < l )
	{
		if( ii > 511 ) 
			lines_processed = l; // avoid crash if client sends out inconsitent data
		ch = pagebuffer[ii];
		if( lin < 33 ) 
			line[lin] = ch; 
		else 
			line[33] = ch;
		ii++;
		lin++;
		if( ch == 0 ) 
		{
			lines_processed++;
			lin = 0;
			WriteLine( i, p, lines_processed, line, s );
		}
	}
 	changeAT = false; // dont re-write author and title if not necassairy
}

// private methods here

// writes the author into the corresponding-bok file
// PRE: packets 0x93 needs to be send by client BEFORE its called. 
// (and its data copied to the authorbuffer)

void cBooks::WriteAuthor( CItem *id, cSocket *s )
{
	char fileName[MAX_PATH];  // Standard 8.3 file name

	sprintf( fileName, "%s/%x.bok", cwmWorldState->ServerData()->GetBooksDirectory(), id->GetSerial() );
	FILE *file = fopen( fileName, "r+b" ); // open existing file for read/write

	if( file == NULL ) // If the .bok file does not exist create it
	{	 
		if( !CreateBook( fileName, id ) ) 
			return;

		file = fopen( fileName, "r+b" ); // open existing file for read/write (now it should exist)
		if( file == NULL )                 
		{
			Console.Error( 1, "Couldn't write to book file %s for book %x", fileName, id->GetSerial() );
			return;
		}
	}

	if( s == NULL )
		return;
	char *authorbuffer = s->AuthorBuffer();
 
	if( fseek( file, 62, SEEK_SET ) ) 
		Console.Error( 1, "Failed to seek to book file %s", fileName );

	authorbuffer[31] = '\n';

	if( fwrite( authorbuffer, sizeof( char ), 32, file ) != 32 ) 
	{
		Console.Error( 1, "Couldn't write to book file %s", fileName );
		fclose(file);
		return;
	}			
	fclose( file );
}


void cBooks::WriteTitle( CItem *id, cSocket *s )
{
	char fileName[MAX_PATH];  // Standard 8.3 file name

	sprintf( fileName, "%s/%x.bok", cwmWorldState->ServerData()->GetBooksDirectory(), id->GetSerial() );
	FILE *file = fopen( fileName, "r+b" ); // open existing file for read/write
  
	if( file == NULL )	// If the BOK file does not exist -> that book must be new
	{					// or the file got deleted -> lets try to create it	 
		if( !CreateBook( fileName, id ) ) 
			return;

		file = fopen( fileName, "r+b" ); // open existing file for read/write (now it should exist)
		if( file == NULL )                 
		{
			Console.Error( 1, "Couldn't write to book file %s", fileName );
			return;
		}
	}
 
	if( fseek( file, 0, SEEK_SET ) ) 
		Console.Error( 1, "Failed to seek to book file %s", fileName );

	if( s == NULL )
	{
		fclose(file);
		return;
	}
	char *titlebuffer = s->TitleBuffer();
	titlebuffer[61] = '\n';

	if( fwrite( titlebuffer, sizeof( char ), 62, file ) != 62 )
	{
		Console.Error( 1, "Couldn't write to book file %s for book %x", fileName, id->GetSerial() );
		fclose(file);
		return;
	}			
	fclose( file );
}

void cBooks::WriteLine( CItem *id, int page, int line, char linestr[34], cSocket *s )
{
	char fileName[MAX_PATH];  // Standard 8.3 file name

	sprintf( fileName, "%s/%x.bok", cwmWorldState->ServerData()->GetBooksDirectory(), id->GetSerial() );
	FILE *file = fopen( fileName, "r+b" ); // open existing file for read/write

	if( file == NULL )	// If the BOK file does not exist -> that book must be new
	{					// or the file got deleted -> lets try to create it
		if( !CreateBook( fileName, id ) ) 
			return;

		file = fopen( fileName, "r+b" ); // open existing file for read/write (now it should exist)
		if( file == NULL )                 
		{
			Console.Error( 1, "Couldn't write to book file %s", fileName );
			return;
		}
	}
 
	int Offset = 273 * page + 34 * line - 207;
	if( fseek( file, Offset, SEEK_SET ) ) 
		Console.Error( 1, "Failed to seek to book file %s", fileName );
 
	linestr[33] = '\n';

	if( fwrite( linestr, sizeof( char ), 34, file ) != 34 ) 
	{
		Console.Error( 1, "Couldn't write to book file %s", fileName );
		fclose(file);
		return;
	}			
	fclose( file );
}

void cBooks::ReadAuthor( CItem *id, UI08 auth[31] )
{
	char fileName[MAX_PATH];  

	sprintf( fileName, "%s/%x.bok", cwmWorldState->ServerData()->GetBooksDirectory(), id->GetSerial() );
	FILE *file = fopen( fileName, "r+b" ); // open existing file for read/write

	if( file == NULL ) 
	{
		Console.Error( 1, "Couldn't read book file %s", fileName );
		return;
	}

	if( fseek( file, 62, SEEK_SET ) ) 
		Console.Error( 1, "Failed to seek to book file %s", fileName );

	if( fread( auth, sizeof( char ), 31, file ) != 31 )  // read it
	{
		Console.Error( 1, "Coudn't write to book file %s", fileName );
		fclose(file);
		return;
	}

	// clear garbage after strign termination
	bool end = ( auth == 0 ); 
	for( UI08 a = 0; a < 31; a++ )
	{
		if( end ) 
			auth[a] = 0;
	}
	fclose( file );
}

	
void cBooks::ReadTitle( CItem *id, UI08 title[61] )
{
	char fileName[MAX_PATH];  

	sprintf( fileName, "%s/%x.bok", cwmWorldState->ServerData()->GetBooksDirectory(), id->GetSerial() );
	FILE *file = fopen( fileName, "r+b" ); // open existing file for read/write

	if( file == NULL )
	{
		Console.Error( 1, "Couldn't read book file %s", fileName );
		return;
	}

	if( fseek( file, 0, SEEK_SET ) ) 
		Console.Error( 1, "Failed to seek to book file %s", fileName );

	if( fread( title, sizeof( char ), 61, file ) != 61 )  // read it
	{
		Console.Error( 1, "Couldn't write to book file %s", fileName );
		fclose(file);
		return;
	}

	// clear garbage after strign termination
	bool end = ( title == 0 ); 
	for( UI08 a = 0; a < 61; a++ )
	{
		if( end ) 
			title[a] = 0;
	}
	fclose( file );
}

UI08 cBooks::getNumberOfPages( CItem *id )
{
	char fileName[MAX_PATH];  
	char num[5];

	sprintf( fileName, "%s/%x.bok", cwmWorldState->ServerData()->GetBooksDirectory(), id->GetSerial() );
	FILE *file = fopen( fileName, "r+b" ); // open existing file for read/write

	if( file == NULL ) 
	{
		Console.Error( 1, "Couldn't read book file %s for book %x", fileName, id->GetSerial() );
		return 1;
	}

	if( fseek( file, 94, SEEK_SET ) ) 
		Console.Error( 1, "Failed to seek to book file %s for book %x", fileName, id->GetSerial() );

	if( fread( num, sizeof(char), 5, file ) != 5 )  // read it
	{
		Console.Error( 1, "Couldn't write to book file %s for book %x", fileName, id->GetSerial() );
		fclose(file);
		return 1;
	}

	// clear garbage after string termination
	bool end = ( num == 0 ); 
	for( UI08 a = 0; a < 5; a++ )
	{
		if( end ) 
			num[a] = 0;
	}
	fclose( file );

	return (UI08)makeNum( num );
}
	
// page+linumber=1 indexed ! (as returned from client)
void cBooks::ReadLine( CItem *id, int page, int linenumber, char line[33] )
{
	char fileName[MAX_PATH];  

	sprintf( fileName, "%s/%x.bok", cwmWorldState->ServerData()->GetBooksDirectory(), id->GetSerial() );
	FILE *file = fopen( fileName, "r+b" ); // open existing file for read/write

	if( file == NULL )
	{
		Console.Error( 1, "Couldn't read book file %s for book %x", fileName, id->GetSerial() );
		return;
	}

	int Offset = 273 * page + 34 * linenumber - 207;
	if( fseek( file, Offset, SEEK_SET ) ) 
		Console.Error( 1, "Failed to seek to book file %s for book %x", fileName, id->GetSerial() );

	if( fread( line, sizeof( char ), 33, file ) != 33 )  // read it
	{
		Console.Error( 1, "Couldn't write to book file %s for book %x", fileName, id->GetSerial() );
		fclose(file);
		return;
	}

	// clear garbage after strign termination
	bool end = ( line == 0 ); 
	for( UI08 a = 0; a < 33; a++ )
	{
		if( end )
			line[a] = 0;
	}

	fclose( file );
}

void cBooks::DeleteBook( CItem *id )
{
	char fileName[MAX_PATH];    
	sprintf( fileName, "%s/%x.bok", cwmWorldState->ServerData()->GetBooksDirectory(), id->GetSerial() );
	remove( fileName );
}

// "Formats" a newly created book-file
// This NEEDS to be done with ANY new book file.
// 
bool cBooks::CreateBook( char *fileName, CItem *id )
{
	FILE *file = fopen( fileName, "w+b" ); // create new file
	char ch;
	char author[33];
	char title[63];
	char line[35];
	char num[5];

	if( file == NULL )
	{
		Console.Error( 1, "Can't create new book file %s for book %x", fileName, id->GetSerial() );
		return false;
	}

	author[0] = '.';	author[1] = 0;	author[31] = '\n';
	title[0]  = '.';	title[1]  = 0;	title[61]  = '\n';
	line[0]   = '.';	line[1]   = 0;	line[33]   = '\n';

	UI08 i;
	for( i = 2; i <= 60; i++ ) 
		title[i] = 32;
	for( i = 2; i <= 30; i++ ) 
		author[i] = 32;
	for( i = 2; i <= 32; i++ ) 
		line[i] = 32;

	if( fwrite( title, sizeof( char ), 62, file ) != 62 ) 
	{
		Console.Error( 1, "Couldn't write to book file %s for book %x", fileName, id->GetSerial() );
		fclose(file);
		return false;
	}

	if( fwrite( author, sizeof(char), 32, file) != 32 ) 
	{
		Console.Error( 1, "Couldn't write to book file %s for book %x", fileName, id->GetSerial() );
		fclose(file);
		return false;
	}

	UI32 maxpages = id->GetMoreY();
	if( maxpages < 1 || maxpages > 255 ) 
		maxpages = 16; // default = 16 pages

	numtostr( maxpages, num ); // danger,danger, didnt this cause probs under LINUX ???

	num[4] = '\n';

	if( fwrite( num, sizeof( char ), 5, file ) != 5 )  // writens number
	{
		Console.Error( 1, "Couldn't write to book file %s for book %x", fileName, id->GetSerial() );
		fclose(file);
		return false;
	}

	for( UI08 j = 0; j < maxpages; j++ ) // page loop
	{
		ch = '\n'; // each page gets a cr
		if( fwrite( &ch, sizeof( char ), 1, file ) != 1 ) 
		{
			Console.Error( 1, "Couldn't write to book file %s for book %x", fileName, id->GetSerial() );
			fclose(file);
			return false;
		}

		for( UI08 l = 0; l < 8; l++ ) // each page has 8 lines
		{	         
			line[0] = 0;
			if( fwrite( line, sizeof( char ), 34, file ) != 34 ) 
			{
				Console.Error( 1, "Couldn't write to book file %s for book %x", fileName, id->GetSerial() );
				fclose(file);
				return false;
			}			
		}
	}
	fclose( file );
	return true;
}

