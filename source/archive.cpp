#include "uox3.h"
#include "regions.h"
#include <filesystem>
#include <chrono>
#include <ctime>
#include <regex>


//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void fileArchive( void )
//|	Date		-	11th April, 2002
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Makes a backup copy of a file in the shared directory.
//|                  puts the copy in the backup directory
//o-----------------------------------------------------------------------------------------------o
static void backupFile( const std::string &filename, std::string backupDir )
{
	auto to = std::filesystem::path(backupDir);
	to /= std::filesystem::path(filename);
	auto from = std::filesystem::path(cwmWorldState->ServerData()->Directory(CSDDP_SHARED));
	std::filesystem::copy_file(from, to);

}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void fileArchive( void )
//|	Date		-	24th September, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Makes a backup copy of the current world state
//o-----------------------------------------------------------------------------------------------o
void fileArchive( void )
{
	Console << "Beginning backup... ";
	auto mytime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	auto timenow = strutil::simplify(std::asctime(std::localtime(&mytime)));
	timenow = std::regex_replace(timenow, std::regex("[\\s:]"), std::string("-"));

	std::string backupRoot	= cwmWorldState->ServerData()->Directory( CSDDP_BACKUP );
	backupRoot				+= timenow;
	auto makeResult = std::filesystem::create_directory(std::filesystem::path(backupRoot));

	if( makeResult  )
	{
		Console << "NOTICE: Accounts not backed up. Archiving will change. Sorry for the trouble." << myendl;

		backupFile( "house.wsc", backupRoot );

		// effect backups
		backupFile( "effects.wsc", backupRoot );

		const SI16 AreaX = UpperX / 8;	// we're storing 8x8 grid arrays together
		const SI16 AreaY = UpperY / 8;


		auto backupPath = strutil::format ("%s%s/", cwmWorldState->ServerData()->Directory( CSDDP_SHARED ).c_str(), timenow.c_str() );

		for( SI16 counter1 = 0; counter1 < AreaX; ++counter1 )	// move left->right
		{
			for( SI16 counter2 = 0; counter2 < AreaY; ++counter2 )	// move up->down
			{
				auto filename1 = strutil::format("%i.%i.wsc", counter1, counter2 );
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
