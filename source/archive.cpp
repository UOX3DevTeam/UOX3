#include "uox3.h"
#include "regions.h"
#include <filesystem>
#include <chrono>
#include <ctime>
#include <regex>
#include "osunique.hpp"

//o------------------------------------------------------------------------------------------------o
//|	Function	-	BackupFile()
//|	Date		-	11th April, 2002
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Makes a backup copy of a file in the shared directory.
//|                  puts the copy in the backup directory
//o------------------------------------------------------------------------------------------------o
static void BackupFile( const std::string &filename, std::string backupDir )
{
	auto to = std::filesystem::path( backupDir );
	to /= std::filesystem::path( filename );
	auto from = std::filesystem::path( cwmWorldState->ServerData()->Directory( CSDDP_SHARED ));
	std::filesystem::copy_file( from, to );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	FileArchive()
//|	Date		-	24th September, 2001
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Makes a backup copy of the current world state
//o------------------------------------------------------------------------------------------------o
void FileArchive( void )
{
	Console << "Beginning backup... ";
	auto mytime = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
	struct tm ttemp;
	char tbuffer[100];
	auto timenow = oldstrutil::simplify( asciitime( tbuffer, 100, *lcltime( mytime, ttemp )));
	timenow = std::regex_replace( timenow, std::regex( "[\\s:]" ), std::string( "-" ));

	std::string backupRoot	= cwmWorldState->ServerData()->Directory( CSDDP_BACKUP );
	backupRoot				+= timenow;
	auto makeResult = std::filesystem::create_directory( std::filesystem::path( backupRoot ));

	if( makeResult  )
	{
		Console << "NOTICE: Accounts not backed up. Archiving will change. Sorry for the trouble." << myendl;

		BackupFile( "house.wsc", backupRoot );

		// effect backups
		BackupFile( "effects.wsc", backupRoot );

		const SI16 AreaX = UpperX / 8;	// we're storing 8x8 grid arrays together
		const SI16 AreaY = UpperY / 8;

		auto backupPath = oldstrutil::format( "%s%s/", cwmWorldState->ServerData()->Directory( CSDDP_SHARED ).c_str(), timenow.c_str() );

		for( SI16 counter1 = 0; counter1 < AreaX; ++counter1 )	// move left->right
		{
			for( SI16 counter2 = 0; counter2 < AreaY; ++counter2 )	// move up->down
			{
				auto filename1 = oldstrutil::format( "%i.%i.wsc", counter1, counter2 );
				BackupFile( filename1, backupRoot );
			}
		}
		BackupFile( "overflow.wsc", backupRoot );
		BackupFile( "jails.wsc", backupRoot );
		BackupFile( "guilds.wsc", backupRoot );
		BackupFile( "regions.wsc", backupRoot );
	}
	else
	{
		Console << "Cannot create backup directory, please check available disk space" << myendl;
	}
	Console << "Finished backup" << myendl;
}
