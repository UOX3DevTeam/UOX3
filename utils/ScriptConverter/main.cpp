// Program to convert all SCP files in the current directory to DFN equivalents
// Version:		1.0	Initial Release
//				1.1	Added tag/data non-pairing to known sections
//				1.2	Added tab support, fixing some scripts
// Programmer:	Abaddon
// Date:		6th November 2001

#define VER_MAJOR "1"
#define VER_MINOR "2"

#pragma warning( disable : 4786 )
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <direct.h>
#else
	#include <dirent.h>
#endif

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

// file mask for old files and new files
const string oldExt = "*.scp";
const string defExt = "*.dfn";

// Old token seperator and new token seperator
const char oldToken[5] = " ";
const char oldToken2[5] = "\t";
const char newToken[5] = "=";

// these particular sections don't use tag/data pairings
const string sectionExceptions[7] = 
{ 
	"MOTD", 
	"TIP ", 
	"GUMPMENU ", 
	"GUMPTEXT ", 
	"PAGE ", 
	"ESCORT ", 
	"RANDOMNAME" 
};

// list of files that fit our description
vector< string > filenameListings;

//o-----------------------------------------------------------o
//	Prototype:	void BuildFileList( void )
//o-----------------------------------------------------------o
//	Programmer:	Abaddon
//	Date:		7th November, 2001
//	Purpose:	To browse the execution directory and grab all
//				files that match the file spec
//	Revision:	V1.0 Initial version
//				V1.1 Function documentation (Abaddon, 9th November 2001)
//o-----------------------------------------------------------o
void BuildFileList( void )
{
#ifdef _WIN32
	WIN32_FIND_DATA toFind;
	// setup file browser handle based upon old extension file spec
	HANDLE findHandle = FindFirstFile( oldExt.c_str(), &toFind );		// grab first file that meets spec
	BOOL retVal = 0;
	// if the handle is valid, push it on
	if( findHandle != INVALID_HANDLE_VALUE )	// there is a file
	{
		filenameListings.push_back( toFind.cFileName );
		retVal = 1;	// let's continue
	}
	while( retVal != 0 )	// while there are still files
	{
		retVal = FindNextFile( findHandle, &toFind );	// grab the next file
		if( retVal != 0 )
			filenameListings.push_back( toFind.cFileName );
	}
	// if the handle is open, and we're done, close it
	if( findHandle != INVALID_HANDLE_VALUE )
	{
		FindClose( findHandle );
		findHandle = INVALID_HANDLE_VALUE;
	}
#else
	// open directory for browsing
	DIR *dir = opendir( "." );
	struct dirent *dirp = NULL;

	// while there are still entries in the list
	while( ( dirp = readdir( dir ) ) )	
	{
		// if it's a directory type, skip over it
		if( dirp->d_type == DT_DIR )
			continue;
		// grab length of filename
		long nLen = strlen( dirp->d_name );
		if( nLen > 3 )	// if filename length > 3
		{
			// grab the old extension 
			const char *xtnsn = oldExt.c_str();
			// ensure that the last 3 characters of the filespec match the old extension, and if so, add to list
			if( dirp->d_name[nLen - 3] == xtnsn[2] && dirp->d_name[nLen - 2] == xtnsn[3] && dirp->d_name[nLen - 1] == xtnsn[4] )
				filenameListings.push_back( dirp->d_name );
		}
	}
#endif
}

//o-----------------------------------------------------------o
//	Prototype:	void CleanFileList( void )
//o-----------------------------------------------------------o
//	Programmer:	Abaddon
//	Date:		7th November, 2001
//	Purpose:	To empty out the file listings vector
//	Revision:	V1.0 Initial version
//				V1.1 Function documentation (Abaddon, 9th November 2001)
//o-----------------------------------------------------------o
void CleanFileList( void )
{
	filenameListings.resize( 0 );
}

//o-----------------------------------------------------------o
//	Prototype:	void main( void )
//o-----------------------------------------------------------o
//	Programmer:	Abaddon
//	Date:		7th November, 2001
//	Purpose:	To convert all files of .SCP to of type .DFN
//	Revision:	V1.0	Initial version
//				V1.1	Function documentation 
//						Added exceptions for known non-data/tag pairings
//						(Abaddon, 9th November 2001)
//				V1.2	Added tab support to help with some scripts
//						(Abaddon, 16th November 2001)
//o-----------------------------------------------------------o
void main( void )
{
	char newFileName[1024];
	cout << "Running Script Converter V" << VER_MAJOR << "." << VER_MINOR << endl;
	cout << "=============================" << endl;
	cout << "Converting SCP files to DFN files" << endl;
	CleanFileList();
	BuildFileList();

	const char *xtnsn = defExt.c_str();

	for( unsigned long i = 0; i < filenameListings.size(); i++ )
	{
		const char *filename = filenameListings[i].c_str();
		long sLen = strlen( filename );
		strcpy( newFileName, filename );

		newFileName[sLen - 3] = xtnsn[2];
		newFileName[sLen - 2] = xtnsn[3];
		newFileName[sLen - 1] = xtnsn[4];

		cout << "Converting " << filename << " to " << newFileName << "... ";

		char line[2048];
		ifstream toRead;
		ofstream toWrite;
		toRead.open( filename );
		toWrite.open( newFileName );
		bool inSection = false;
		bool knownNonTagSection = false;

		char *spaceLocation = NULL;

		while( !toRead.eof() )
		{
			toRead.getline( line, 2047 );
			long lineLength = strlen( line );
			if( line[0] == '/' && line[1] == '/' )	// if a comment
			{
				toWrite.write( line, lineLength );
				toWrite.write( "\n", 1 );
			}
			else if( inSection )
			{
				if( !knownNonTagSection )
				{
					if( line[0] == '}' )
					{
						toWrite.write( line, lineLength );
						toWrite.write( "\n", 1 );
						inSection = false;
						knownNonTagSection = false;
					}
					else if( line[0] == '/' && line[1] == '/' )	// if a comment
					{
						toWrite.write( line, lineLength );
						toWrite.write( "\n", 1 );
					}
					else
					{
						spaceLocation = NULL;
						spaceLocation = strstr( line, oldToken );
						if( spaceLocation == NULL )
							spaceLocation = strstr( line, oldToken2 );
						if( spaceLocation != NULL )
						{
							char tagStuff[1024];
							strncpy( tagStuff, line, spaceLocation - line );
							tagStuff[spaceLocation - line] = 0;
							strcat( tagStuff, newToken );
							strcat( tagStuff, spaceLocation + 1 );
							strcat( tagStuff, "\n" );
							toWrite.write( tagStuff, strlen( tagStuff ) );
						}
						else
						{
							toWrite.write( line, lineLength );
							toWrite.write( "\n", 1 );
						}
					}
				}
				else
				{
					if( line[0] == '}' )	// is it end of section?
					{
						toWrite.write( line, lineLength );
						toWrite.write( "\n", 1 );
						inSection = false;
						knownNonTagSection = false;
					}
					else	// Guess not, dump unmodified
					{
						toWrite.write( line, lineLength );
						toWrite.write( "\n", 1 );
					}
				}
			}
			else
			{
				if( !strncmp( line, "SECTION ", 8 ) )
				{
					for( int k = 0; k < 7; k++ )
					{
						if( strstr( line, sectionExceptions[k].c_str() ) )
						{
							knownNonTagSection = true;
							break;
						}
					}
					char builtSection[1024];
					strcpy( builtSection, "[" );
					strcat( builtSection, &line[8] );
					strcat( builtSection, "]\n" );
					toWrite.write( builtSection, strlen( builtSection ) );
				}
				else if( line[0] == '{' )
				{
					toWrite.write( line, lineLength );
					toWrite.write( "\n", 1 );
					inSection = true;
				}
				else
				{
					toWrite.write( line, lineLength );
					toWrite.write( "\n", 1 );
				}
			}
		}
		cout << "Done!" << endl;

	}
	CleanFileList();
}
