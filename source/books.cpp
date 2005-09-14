// Books class by Lord Binary 7'th December 1999
// Implements writable books

// V0.9 5'th dec, initial version
// V1.0 7'th dec 1999, "wrapped" everything in a class, added customable number of max-pages
// V1.1 12-dec-1999 -> nasty bug fixed (item# used as "key" instead of serial#) and a few very small bugfixes

#include "uox3.h"
#include "books.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "CPacketSend.h"

namespace UOX
{

cBooks *Books;

// opens old (readonly) books == old, bugfixed readbook function
void cBooks::OpenPreDefBook( CSocket *mSock, CItem *i )
{
	if( mSock != NULL )
	{
		UString temp		= "BOOK " + UString::number( i->GetTempVar( CITV_MORE ) );
		ScriptSection *book = FileLookup->FindEntry( temp, misc_def );
		if( book != NULL )
		{

			UString data;

			CPBookTitlePage toSend;
			toSend.Serial( i->GetSerial() );
			toSend.WriteFlag( 0 );
			toSend.NewFlag( 0 );

			bool part1 = false, part2 = false, part3 = false;
			for( UString tag = book->First(); !book->AtEnd(); tag = book->Next() )
			{
				data = book->GrabData();
				if( tag == "PAGES" )
				{
					part1 = true;
					toSend.Pages( data.toShort() );
				}
				else if( tag == "TITLE" )
				{
					part2 = true;
					toSend.Title( data );
				}
				else if( tag == "AUTHOR" )
				{
					part3 = true;
					toSend.Author( data );
				}
				if( part1 && part2 && part3 )
					break;
			}

			mSock->Send( &toSend );
		}
	}
}

// opens new books
void cBooks::OpenBook( CSocket *mSock, CItem *i, bool isWriteable )
{
	if( mSock != NULL )
	{
		SI16 pages;
		std::string line;
		// I dont know what that new client 1.26 byte does, but it needs to be set to 1 or 2 or writing doesnt work
		// wild guess: rune books ...

		std::string booktitle, bookauthor;
		std::string fileName	= cwmWorldState->ServerData()->Directory( CSDDP_BOOKS ) + UString::number( i->GetSerial(), 16 ) + ".bok";
		std::fstream file;
		file.open( fileName.c_str(), std::ios_base::in | std::ios_base::binary );	// Really binary?
		bool bookexists = ( file.is_open() );
		if( bookexists )
		{
			file.close();
			bookauthor	= ReadAuthor( i ); // fetch author if existing
			booktitle	= ReadTitle ( i ); // fetch title if existing
			pages		= getNumberOfPages( i ); 
		}
		else
		{
			pages = (SI16)i->GetTempVar( CITV_MOREY );				// if new book get number of maxpages ...
			if( pages < 1 || pages > 255 )
				pages = 16;
		}
		mSock->ClearAuthor();
		mSock->ClearTitle();
		mSock->ClearPage();

		CPBookTitlePage bInfo;
		bInfo.Serial( i->GetSerial() );
		bInfo.Pages( pages );
		bInfo.NewFlag( 1 );

		if( isWriteable )
			bInfo.WriteFlag( 1 );
		else
			bInfo.WriteFlag( 0 );
		mSock->Send( &bInfo );

		if( bookexists && isWriteable ) // dont send book contents if the file doesnt exist (yet)!
		{
			//////////////////////////////////////////////////////////////
			// Now we HAVE to send the ENTIRE Book                     / /
			// Cauz in writeable mode the client only sends out packets  /
			// if something  gets changed                                /
			// this also means -> for each bookopening in writeable mode /
			// lots of data has to be send.                              /
			//////////////////////////////////////////////////////////////

			CPBookPage cpbpToSend( (*i) );

			for( UI08 a = 1; a <= static_cast<UI08>(pages); ++a )
			{
				cpbpToSend.NewPage();
				for( UI08 b = 1; b <= 8; ++b )
				{
					line = ReadLine( i, a, b );
					cpbpToSend.AddLine( line );
				}
			}
			cpbpToSend.Finalize();
			mSock->Send( &cpbpToSend );
		}
	}
}

// sends a page of new readonly book to the client
void cBooks::ReadNonWritableBook( CSocket *s, CItem *i, UI16 p )
{
	if( s != NULL )
	{
		std::string line;
		std::string fileName	= cwmWorldState->ServerData()->Directory( CSDDP_BOOKS ) + UString::number( i->GetSerial(), 16 ) + ".bok";
		std::fstream file;
		file.open( fileName.c_str(), std::ios_base::in | std::ios_base::binary );	// IS this really binary?

		if( file.is_open() )
		{
			file.close();

			CPBookPage cpbpSend( (*i) );
			cpbpSend.NewPage( p );		// set explicitly to this page

			for( UI08 a = 1; a <= 8; ++a )
			{
				line = ReadLine( i, (UI08)p, a );
				cpbpSend.AddLine( line );
			}
			s->Send( &cpbpSend );
		}
		else
		{
			Console.Error( 1, "Failed to open %s", fileName.c_str() );
		}
	}
}

// old readbook function
void cBooks::ReadPreDefBook( CSocket *mSock, CItem *i, UI16 p )
{
	if( mSock != NULL )
	{
		UString temp		= "BOOK " + UString::number( i->GetTempVar( CITV_MORE ) );
		ScriptSection *book	= FileLookup->FindEntry( temp, misc_def );
		if( book != NULL )
		{
			UI16 curPage = p;
			for( UString tag = book->First(); !book->AtEnd(); tag = book->Next() )
			{
				if( tag == "PAGE" )
				{
					--curPage;
					if( curPage == 0 ) // we found our page
					{
						temp = "PAGE " + book->GrabData();
						ScriptSection *page = FileLookup->FindEntry( temp, misc_def );
						if( page != NULL )
						{
							CPBookPage cpbpSend;
							cpbpSend.Serial( i->GetTempVar( CITV_MORE ) );
							cpbpSend.NewPage( p );

							for( tag = page->First(); !page->AtEnd(); tag = page->Next() )
							{
								temp = tag + " " + page->GrabData();
								cpbpSend.AddLine( temp );
							}
							cpbpSend.Finalize();
							mSock->Send( &cpbpSend );
							break;
						}
						else
							break;
					}
				}
			}
		}
	}
}

// writes changes to a writable book to the bok file.		
void cBooks::ReadWritableBook( CSocket *s, CItem *i, UI16 p, UI16 l )
{
	char line[34], ch;

	if( changeAT )
	{
		WriteTitle( i, s );	// if title was changed by writer write the changes "down"
		WriteAuthor( i, s ); // if author was changed by writer write the changes "down" to the bok-file
	}

	if( s != NULL )
	{
		const char *pagebuffer = s->PageBuffer();
		UI16 lines_processed = 0, ii = 0, lin = 0;
		while( lines_processed < l )
		{
			if( ii > 511 )
				lines_processed = l; // avoid crash if client sends out inconsitent data
			ch = pagebuffer[ii];
			if( lin < 33 )
				line[lin] = ch;
			else
				line[33] = ch;
			++ii;
			++lin;
			if( ch == 0 )
			{
				++lines_processed;
				lin = 0;
				WriteLine( i, p, lines_processed, line );
			}
		}
		changeAT = false; // dont re-write author and title if not necassairy
	}
}

// private methods here

// writes the author into the corresponding-bok file
// PRE: packets 0x93 needs to be send by client BEFORE its called. 
// (and its data copied to the authorbuffer)

void cBooks::WriteAuthor( CItem *id, CSocket *s )
{
	if( s != NULL )
	{
		std::string fileName = cwmWorldState->ServerData()->Directory( CSDDP_BOOKS ) + UString::number( id->GetSerial(), 16 ) + ".bok";
		std::fstream file;
		file.open( fileName.c_str(), std::ios_base::out | std::ios_base::binary );	// Really binary?

		if( !file.is_open() ) // If the .bok file does not exist create it
		{
			if( CreateBook( fileName, id ) )
			{
				file.open( fileName.c_str(), std::ios_base::out | std::ios_base::binary );	// Really binary?
			}
		}
		if( file.is_open() )	// If it isn't open now, our create and open failed
		{
			const char *authorbuffer = s->AuthorBuffer();

			file.seekp( 62, std::ios_base::beg );
			if( !file.fail() )
			{
				file.write( authorbuffer, 32 );
				if( file.fail() )
					Console.Error( 1, "Couldn't write to book file %s", fileName.c_str() );
			}
			else
				Console.Error( 1, "Failed to seek to book file %s", fileName.c_str() );
			file.close();
		}
		else
			Console.Error( 1, "Couldn't write to book file %s for book 0x%X", fileName.c_str(), id->GetSerial() );
	}
}


void cBooks::WriteTitle( CItem *id, CSocket *s )
{
	if( s != NULL )
	{
		std::string fileName = cwmWorldState->ServerData()->Directory( CSDDP_BOOKS ) + UString::number( id->GetSerial(), 16 ) + ".bok";
		std::fstream file;
		file.open( fileName.c_str(), std::ios_base::out | std::ios_base::binary );

		if( !file.is_open() )	// If the BOK file does not exist -> that book must be new
		{
			// or the file got deleted -> lets try to create it	 
			if( CreateBook( fileName, id ) )
				file.open( fileName.c_str(), std::ios_base::out | std::ios_base::binary );
		}
		if( file.is_open() )
		{
			file.seekp( 0, std::ios_base::beg );
			if( !file.fail() )
			{
				const char *titlebuffer = s->TitleBuffer();
				file.write( titlebuffer, 62 );
				if( file.fail() )
					Console.Error( 1, "Couldn't write to book file %s for book 0x%X", fileName.c_str(), id->GetSerial() );
			}
			else
				Console.Error( 1, "Failed to seek to book file %s", fileName.c_str() );
			file.close();
		}
		else
			Console.Error( 1, "Couldn't write to book file %s", fileName.c_str() );
	}
}

void cBooks::WriteLine( CItem *id, UI16 page, UI16 line, char linestr[34] )
{
	std::string fileName = cwmWorldState->ServerData()->Directory( CSDDP_BOOKS ) + UString::number( id->GetSerial(), 16 ) + ".bok";
	std::fstream file;
	file.open( fileName.c_str(), std::ios_base::out | std::ios_base::binary );

	if( !file.is_open() )	// If the BOK file does not exist -> that book must be new
	{
		// or the file got deleted -> lets try to create it
		if( CreateBook( fileName, id ) )
			file.open( fileName.c_str(), std::ios_base::out | std::ios_base::binary );
	}
	if( file.is_open() )
	{
		int Offset = static_cast<int>(273 * page + 34 * line - 207);
		file.seekp( Offset, std::ios_base::beg );
		if( !file.fail() )
		{
			linestr[33] = '\n';
			file.write( linestr, 34 );
			if( file.fail() )
				Console.Error( 1, "Couldn't write to book file %s", fileName.c_str() );
		}
		else
			Console.Error( 1, "Failed to seek to book file %s", fileName.c_str() );
		file.close();
	}
	else
		Console.Error( 1, "Couldn't write to book file %s", fileName.c_str() );
}

std::string cBooks::ReadAuthor( CItem *id )
{
	char auth[31];
	std::string fileName = cwmWorldState->ServerData()->Directory( CSDDP_BOOKS ) + UString::number( id->GetSerial(), 16 ) + ".bok";
	std::fstream file;
	std::string rvalue;
	file.open( fileName.c_str(), std::ios_base::in | std::ios_base::binary );

	if( file.is_open() )
	{
		file.seekg( 62, std::ios_base::beg );
		if( !file.fail() )
		{
			file.read( (char *)auth, 31 );
			rvalue = auth;
		}
		else
			Console.Error( 1, "Failed to seek to book file %s", fileName.c_str() );
		file.close();
	}
	else
		Console.Error( 1, "Couldn't read book file %s", fileName.c_str() );
	return rvalue;
}


std::string cBooks::ReadTitle( CItem *id )
{
	std::string fileName = cwmWorldState->ServerData()->Directory( CSDDP_BOOKS ) + UString::number( id->GetSerial(), 16 ) + ".bok";
	std::fstream file;
	std::string rvalue;
	char title[61];
	file.open( fileName.c_str(), std::ios_base::in | std::ios_base::binary );

	if( file.is_open() )
	{
		file.seekg( 0, std::ios_base::beg );
		if( !file.fail() )
		{
			file.read( (char *)title, 61 );
			rvalue = title;
		}
		else
			Console.Error( 1, "Failed to seek to book file %s", fileName.c_str() );
		file.close();
	}
	else
		Console.Error( 1, "Couldn't read book file %s", fileName.c_str() );
	return rvalue;
}

UI08 cBooks::getNumberOfPages( CItem *id )
{
	char num[5];
	UI08 rvalue = 1;	// Why 1 if failure, no idea.  Seems 0 would be a better indicator
	std::fstream file;
	std::string fileName = cwmWorldState->ServerData()->Directory( CSDDP_BOOKS ) + UString::number( id->GetSerial(), 16 ) + ".bok";
	file.open( fileName.c_str(), std::ios_base::in | std::ios_base::binary );
	if( file.is_open() )
	{
		file.seekg( 94, std::ios_base::beg );
		file.read( num, 5 );
		UString tnum( num );
		rvalue = tnum.toUByte();
		file.close();
	}
	else
		Console.Error( 1, "Couldn't read book file %s for book 0x%X", fileName.c_str(), id->GetSerial() );
	return rvalue;
}

// page+linumber=1 indexed ! (as returned from client)
std::string cBooks::ReadLine( CItem *id, UI08 page, UI08 linenumber )
{
	std::string fileName = cwmWorldState->ServerData()->Directory( CSDDP_BOOKS ) + UString::number( id->GetSerial(), 16 ) + ".bok";
	std::fstream file;
	file.open( fileName.c_str(), std::ios_base::in | std::ios_base::binary );	// open existing file for read/write
	std::string rvalue;
	char line[33];

	if( file.is_open() )
	{
		int Offset = static_cast< int >(273 * page + 34 * linenumber - 207);
		file.seekg( Offset, std::ios_base::beg );
		file.read( line, 33 );
		rvalue = line;
		file.close();
	}
	else
		Console.Error( 1, "Couldn't read book file %s for book 0x%X", fileName.c_str(), id->GetSerial() );
	return rvalue;
}

void cBooks::DeleteBook( CItem *id )
{
	std::string fileName = cwmWorldState->ServerData()->Directory( CSDDP_BOOKS ) + UString::number( id->GetSerial(), 16 ) + ".bok";
	remove( fileName.c_str() );
}

// "Formats" a newly created book-file
// This NEEDS to be done with ANY new book file.
// 
bool cBooks::CreateBook( std::string fileName, CItem *id )
{
	std::fstream file;
	file.open( fileName.c_str(), std::ios_base::out | std::ios_base::binary );
	char ch[1];
	char author[33];
	char title[63];
	char line[35];
	char num[5];
	bool rvalue = false;

	if( file.is_open() )
	{
		author[0] = '.';	author[1] = 0;	author[31] = '\n';
		title[0]  = '.';	title[1]  = 0;	title[61]  = '\n';
		line[0]   = '.';	line[1]   = 0;	line[33]   = '\n';

		UI08 i;
		for( i = 2; i <= 60; ++i )
			title[i] = 32;
		for( i = 2; i <= 30; ++i )
			author[i] = 32;
		for( i = 2; i <= 32; ++i )
			line[i] = 32;

		file.write( title, 62 );
		file.write( author, 32 );

		UI32 maxpages = id->GetTempVar( CITV_MOREY );
		if( maxpages < 1 || maxpages > 255 )
			maxpages = 16; // default = 16 pages

		strcpy( num, UString::number( maxpages ).c_str() );
		num[4] = '\n';
		file.write( num, 5 );

		for( UI08 j = 0; j < static_cast<UI08>(maxpages); ++j )	// page loop
		{
			ch[0] = '\n'; // each page gets a cr
			file.write( ch, 1 );

			for( UI08 l = 0; l < 8; ++l ) // each page has 8 lines
			{
				line[0] = 0;
				file.write( line, 34 );
			}
		}
		file.close();
		rvalue = true;
	}
	else
	{
		Console.Error( 1, "Can't create new book file %s for book 0x%X", fileName.c_str(), id->GetSerial() );
	}
	return rvalue;

}

}
