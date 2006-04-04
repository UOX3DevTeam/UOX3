//o--------------------------------------------------------------------------o
//|	File			-	books.cpp
//|	Date			-	12/05/1999
//|	Developers		-	LB / giwo
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Writable and Pre-Defined Book Handling
//o--------------------------------------------------------------------------o
//| Modifications	-	Version History
//|
//|							0.9			LB			12/05/1999
//|							Initial implimentation
//|
//|							1.0			LB			12/07/1999
//|							Wrapped everything in a class, added customizable number of max-pages
//|
//|							1.1			LB			12/12/1999
//|							Nasty bug fixed (item # used as "key" instead of serial #) and a few very small bugfixes
//|
//|							2.0			giwo		5th November, 2005
//|							Rewrote much of the original .bok handling to improve performance
//|							Fixed several issues with the fstream handling to ensure validity of data from .bok files.
//|							Slimmed down the cBook class making use of packet classes for handling data from the client.
//|							Added support for the new book header packet removing support for the old book header packet.
//|							
//o--------------------------------------------------------------------------o
#include "uox3.h"
#include "books.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "CPacketSend.h"

namespace UOX
{

cBooks *Books = NULL;

//o--------------------------------------------------------------------------o
//|	Function		-	bool CPINewBookHeader( void )
//|	Date			-	11/5/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Updates the .bok file with changes to the author or title
//o--------------------------------------------------------------------------o
bool CPINewBookHeader::Handle( void )
{
	if( tSock != NULL )
	{
		const SERIAL bookSer = tSock->GetDWord( 3 );
		CItem *mBook = calcItemObjFromSer( bookSer );
		if( !ValidateObject( mBook ) )
			return true;

		const std::string fileName = cwmWorldState->ServerData()->Directory( CSDDP_BOOKS ) + UString::number( bookSer, 16 ) + ".bok";

		if( !FileExists( fileName ) )
			Books->CreateBook( fileName, tSock->CurrcharObj(), mBook );

		std::fstream file( fileName.c_str(), std::ios::in | std::ios::out | std::ios::binary );

		if( file.is_open() )	// If it isn't open now, our create and open failed
		{
			const size_t titleLen = tSock->GetWord( 11 );
			const size_t authorLen = tSock->GetWord( 13+titleLen );
			
			char titleBuff[62];
			char authBuff[32];

			memset( titleBuff, 0x00, 62 );
			memset( authBuff, 0x00, 32 );

			memcpy( titleBuff, &tSock->Buffer()[13], (titleLen>61?61:titleLen) );
			memcpy( authBuff, &tSock->Buffer()[15+titleLen], (authorLen>31?31:authorLen) );

			file.seekp( 0, std::ios::beg );
			if( !file.fail() )
			{
				file.write( titleBuff, 62 );
				file.write( authBuff, 32 );

				if( file.fail() )
					Console.Error( "Couldn't write to book file %s", fileName.c_str() );
			}
			else
				Console.Error( "Failed to seek to book file %s", fileName.c_str() );
			file.close();
		}
		else
			Console.Error( "Couldn't write to book file %s for book 0x%X", fileName.c_str(), bookSer );
	}
	return true;
}

//o--------------------------------------------------------------------------o
//|	Function		-	void cBooks::OpenPreDefBook( CSocket *mSock, CItem *i )
//|	Date			-	11/5/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Opens "Read Only" Books defined in /misc/books.dfn
//o--------------------------------------------------------------------------o
void cBooks::OpenPreDefBook( CSocket *mSock, CItem *i )
{
	if( mSock != NULL )
	{
		UString temp		= "BOOK " + UString::number( i->GetTempVar( CITV_MORE ) );
		ScriptSection *book = FileLookup->FindEntry( temp, misc_def );
		if( book != NULL )
		{
			UString data, UTag;

			CPNewBookHeader toSend;
			toSend.Serial( i->GetSerial() );
			toSend.Flag1( 0 );
			toSend.Flag2( 0 );

			bool part1 = false, part2 = false, part3 = false;
			for( UString tag = book->First(); !book->AtEnd(); tag = book->Next() )
			{
				UTag = tag.upper();
				data = book->GrabData();
				if( UTag == "PAGES" )
				{
					part1 = true;
					toSend.Pages( data.toShort() );
				}
				else if( UTag == "TITLE" )
				{
					part2 = true;
					toSend.Title( data );
				}
				else if( UTag == "AUTHOR" )
				{
					part3 = true;
					toSend.Author( data );
				}
				if( part1 && part2 && part3 )
					break;
			}
			toSend.Finalize();
			mSock->Send( &toSend );
		}

		ReadPreDefBook( mSock, i, 1 );
	}
}


//o--------------------------------------------------------------------------o
//|	Function		-	void cBooks::OpenBook( CSocket *mSock, CItem *mBook, bool isWritable )
//|	Date			-	11/5/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Opens Writable books based on their .bok file
//o--------------------------------------------------------------------------o
void cBooks::OpenBook( CSocket *mSock, CItem *mBook, bool isWriteable )
{
	if( mSock != NULL )
	{
		CPBookPage cpbpToSend;
		CPNewBookHeader bInfo;
		bInfo.Serial( mBook->GetSerial() );

		UI16 numPages = 0;

		std::string bookTitle, bookAuthor;
		const std::string fileName = cwmWorldState->ServerData()->Directory( CSDDP_BOOKS ) + UString::number( mBook->GetSerial(), 16 ) + ".bok";

		std::ifstream file( fileName.c_str(), std::ios::in | std::ios::binary );

		bool bookExists = (file.is_open());
		if( bookExists )
		{
			file.seekg( 0, std::ios::beg );

			if( !file.fail() )
			{
				char rBuffer[2];
				char tempString[62];

				file.read( tempString, 62 );
				bookTitle = tempString;

				file.read( tempString, 32 );
				bookAuthor = tempString;

				file.read( rBuffer, 2 );
				numPages = static_cast<UI16>((rBuffer[0]<<8) + rBuffer[1]);

				cpbpToSend.Serial( mBook->GetSerial() );
				for( UI16 pageNum = 0; pageNum < numPages; ++pageNum )
				{
					UI08 blankLineCtr = 0;
					STRINGLIST tempLines;
					tempLines.resize( 0 );
					for( UI08 lineNum = 0; lineNum < 8; ++lineNum )
					{
						file.read( tempString, 34 );
						tempLines.push_back( tempString );
						if( tempString[0] == 0x00 )
							++blankLineCtr;
					}
					if( blankLineCtr == 8 )
						tempLines.clear();
					cpbpToSend.NewPage( -1, &tempLines );
				}
				cpbpToSend.Finalize();
			}
			else
				Console.Error( "Failed to seek to book file %s", fileName.c_str() );

			file.close();
		}
		else
		{
			numPages = static_cast<UI16>(mBook->GetTempVar( CITV_MOREY ));				// if new book get number of maxpages ...
			if( numPages < 1 || numPages > 255 )
				numPages = 16;
		}

		if( bookAuthor.empty() )
		{
			CChar *mChar = mSock->CurrcharObj();
			if( ValidateObject( mChar ) )
				bookAuthor = mChar->GetName();
		}
		if( bookTitle.empty() )
			bookTitle = "a book";

		bInfo.Pages( numPages );

		if( isWriteable )
		{
			bInfo.Flag1( 1 );
			bInfo.Flag2( 1 );
		}
		else
		{
			bInfo.Flag1( 0 );
			bInfo.Flag2( 0 );
		}

		bInfo.Author( bookAuthor );
		bInfo.Title( bookTitle );

		bInfo.Finalize();
		mSock->Send( &bInfo );

		if( bookExists ) // dont send book contents if the file doesnt exist (yet)!
			mSock->Send( &cpbpToSend );
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	void cBooks::ReadPreDefBook( CSocket *mSock, CItem *i, UI16 p )
//|	Date			-	11/5/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Sends pager number "p" of a Pre-defined Book to the socket
//o--------------------------------------------------------------------------o
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
				if( tag != "PAGE" )
					continue;

				--curPage;
				if( curPage == 0 ) // we found our page
				{
					temp = "PAGE " + book->GrabData();
					ScriptSection *page = FileLookup->FindEntry( temp, misc_def );
					if( page != NULL )
					{
						CPBookPage cpbpSend( (*i) );
						cpbpSend.NewPage( p );

						for( tag = page->First(); !page->AtEnd(); tag = page->Next() )
						{
							temp = tag + " " + page->GrabData();
							cpbpSend.AddLine( temp );
						}
						cpbpSend.Finalize();
						mSock->Send( &cpbpSend );
					}
					break;
				}
			}
		}
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	void CPIBookPage::Handle( void )
//|	Date			-	11/5/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Updates the .bok file with changes made on a specific page.
//o--------------------------------------------------------------------------o
bool CPIBookPage::Handle( void )
{
	if( tSock != NULL )
	{
		CItem *mBook = calcItemObjFromSer( tSock->GetDWord( 3 ) );
		if( !ValidateObject( mBook ) )
			return true;

		const UI16 pageNum = tSock->GetWord( 9 );

		if( mBook->GetTempVar( CITV_MOREX ) != 666 ) // Just incase, make sure it is a writable book
		{
			if( mBook->GetTempVar( CITV_MOREX ) != 999 )
				Books->ReadPreDefBook( tSock, mBook, pageNum ); 
			return true;
		}
		
		const std::string fileName	= cwmWorldState->ServerData()->Directory( CSDDP_BOOKS ) + UString::number( mBook->GetSerial(), 16 ) + ".bok";
		const UI16 totalLines		= tSock->GetWord( 11 );
		char tempLines[8][34];

		UI16 bufferCount = 0;
		for( UI08 lineNum = 0; lineNum < totalLines; ++lineNum )
		{
			memset( tempLines[lineNum], 0x00, 34 );

			UI08 i = 0;
			for( i = 0; i < 34; ++i )
			{
				tempLines[lineNum][i] = tSock->Buffer()[13+bufferCount];
				++bufferCount;
				if( tempLines[lineNum][i] == 0x00 )
					break;
			}
			if( tempLines[lineNum][i] != 0x00 )
				tempLines[lineNum][i] = 0x00;
		}

		if( !FileExists( fileName ) )
			Books->CreateBook( fileName, tSock->CurrcharObj(), mBook );

		std::fstream file( fileName.c_str(), std::ios::in | std::ios::out | std::ios::binary );
		if( file.is_open() )
		{
			file.seekp( (((pageNum-1)*272)+32+62+2), std::ios::beg );
			if( !file.fail() )
			{
				for( UI08 j = 0; j < totalLines; ++j )
				{
					file.write( (const char *)&tempLines[j], 34 );
				}
			}
			else
				Console.Error( "Failed to seek to book file %s", fileName.c_str() );
			file.close();
		}
		else
			Console.Error( "Couldn't write to book file %s", fileName.c_str() );
	}
	return true;
}

//o--------------------------------------------------------------------------o
//|	Function		-	void cBooks::DeleteBook( CItem *id )
//|	Date			-	11/5/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Deletes the .bok file associated with the book item
//o--------------------------------------------------------------------------o
void cBooks::DeleteBook( CItem *id )
{
	std::string fileName = cwmWorldState->ServerData()->Directory( CSDDP_BOOKS ) + UString::number( id->GetSerial(), 16 ) + ".bok";
	remove( fileName.c_str() );
}

//o--------------------------------------------------------------------------o
//|	Function		-	void cBooks::CreateBook( const std::string fileName, CChar *mChar, CItem *mBook )
//|	Date			-	11/5/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Formats a newly created .bok file, this must be done with any new book file
//o--------------------------------------------------------------------------o
void cBooks::CreateBook( const std::string fileName, CChar *mChar, CItem *mBook )
{
	char wBuffer[2];
	char line[34];
	char titleBuff[62];
	char authBuff[32];

	UI16 maxpages = static_cast<UI16>(mBook->GetTempVar( CITV_MOREY ));
	if( maxpages < 1 || maxpages > 255 )
		maxpages = 16;

	const std::string author	= mChar->GetName();
	const std::string title		= "a book";

	memset( line, 0x00, 34 );
	memset( titleBuff, 0x00, 62 );
	memset( authBuff, 0x00, 32 );

	strncpy( titleBuff, title.c_str(), 61 );
	strncpy( authBuff, author.c_str(), 31 );

	std::ofstream file( fileName.c_str(), std::ios::out | std::ios::binary | std::ios::trunc );

	file.seekp( 0, std::ios::beg );

	file.write( titleBuff, 62 );
	file.write( authBuff, 32 );

	wBuffer[0] = static_cast<char>(maxpages>>8);
	wBuffer[1] = static_cast<char>(maxpages%256);
	file.write( (const char *)&wBuffer, 2 );

	for( UI16 i = 0; i < maxpages; ++i )
	{
		for( UI08 lineNum = 0; lineNum < 8; ++lineNum )
		{
			file.write( (const char *)&line, 34 );
		}
	}

	file.close();
}

}
