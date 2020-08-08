#include "uox3.h"
#include "regions.h"
#if UOX_PLATFORM != PLATFORM_WIN32
	#include <dirent.h>
#else
	#include <direct.h>
#endif

namespace UOX
{

const SI32 BUFFERSIZE = 16384;

bool fileCopy( std::string sourceFile, std::string targetFile )
{
	bool rvalue = false;
	if( !sourceFile.empty() && !targetFile.empty() )
	{
		std::fstream source;
		std::fstream target;
		source.open( sourceFile.c_str(), std::ios_base::in | std::ios_base::binary );
		target.open( targetFile.c_str(), std::ios_base::out | std::ios_base::binary );
		if( target.is_open() && source.is_open() )
		{
			rvalue = true;
			char buffer[BUFFERSIZE];
			while( !source.eof() )
			{
				source.read( buffer, BUFFERSIZE );
				std::streamsize amtRead = source.gcount();
				if( amtRead > 0 )
				{
					target.write( buffer, amtRead );
				}
			}
		}
		// We put these out of the if scope, since one may be open.  It doesn't hurt closing a stream not open
		source.close();
		target.close();
	}
	return rvalue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void fileArchive( void )
//|	Date		-	11th April, 2002
//|	Programmer	-	duckhead
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Makes a backup copy of a file in the shared directory.
//|                  puts the copy in the backup directory
//o-----------------------------------------------------------------------------------------------o
static void backupFile( const std::string &filename, std::string backupDir )
{
	UString			to( backupDir );
	to				= to.fixDirectory();
	to				+= filename;
	UString from	= cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + filename;

	fileCopy( from, to );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void fileArchive( void )
//|	Date		-	24th September, 2001
//|	Programmer	-	Abaddon (rewritten for 0.95)
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Makes a backup copy of the current world state
//o-----------------------------------------------------------------------------------------------o
void fileArchive( void )
{
	Console << "Beginning backup... ";
	time_t mytime;
	time( &mytime );
	tm *ptime = localtime( &mytime );			// get local time
	const char *timenow = asctime( ptime );		// convert it to a string

	char timebuffer[256];
	strcpy( timebuffer, timenow );

	// overwriting the characters that aren't allowed in paths
	for( UI32 a = 0; a < strlen( timebuffer ); ++a )
	{
		if( isspace( timebuffer[a] ) || timebuffer[a] == ':' )
			timebuffer[a]='-';
	}

	std::string backupRoot	= cwmWorldState->ServerData()->Directory( CSDDP_BACKUP );
	backupRoot				+= timebuffer;
	int makeResult = _mkdir( backupRoot.c_str(), 0777 );
	if( makeResult == 0 )
	{
		Console << "NOTICE: Accounts not backed up. Archiving will change. Sorry for the trouble." << myendl;

		backupFile( "house.wsc", backupRoot );

		// effect backups
		backupFile( "effects.wsc", backupRoot );

		const SI16 AreaX = UpperX / 8;	// we're storing 8x8 grid arrays together
		const SI16 AreaY = UpperY / 8;
		char backupPath[MAX_PATH + 1];
		char filename1[MAX_PATH];

		sprintf( backupPath, "%s%s/", cwmWorldState->ServerData()->Directory( CSDDP_SHARED ).c_str(), timebuffer );

		for( SI16 counter1 = 0; counter1 < AreaX; ++counter1 )	// move left->right
		{
			for( SI16 counter2 = 0; counter2 < AreaY; ++counter2 )	// move up->down
			{
				sprintf( filename1, "%i.%i.wsc", counter1, counter2 );
				backupFile( filename1, backupRoot );
			}
		}
		backupFile( "overflow.wsc", backupRoot );
		backupFile( "jails.wsc", backupRoot );
		backupFile( "guilds.wsc", backupRoot );
		backupFile( "regions.wsc", backupRoot );
	}
	else
	{
		Console << "Cannot create backup directory, please check available disk space" << myendl;
	}
	Console << "Finished backup" << myendl;
}

}
