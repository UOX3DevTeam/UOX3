#include "uox3.h"
#include "regions.h"
#include <filesystem>
#include <chrono>
#include <ctime>
#include <regex>
#include "osunique.hpp"

//o------------------------------------------------------------------------------------------------o
//|	Function	-	BackupEntireDirectory()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Makes a backup copy of an entire directory, storing it in the backup directory
//o------------------------------------------------------------------------------------------------o
static void BackupEntireDirectory( const std::string &sourceDirPath, const std::string &backupDirPath )
{
	std::error_code ec;

	auto sourcePath = std::filesystem::path( sourceDirPath );
	auto backupPath = std::filesystem::path( backupDirPath );

	if( std::filesystem::exists( sourcePath, ec ))
	{
		if( std::filesystem::is_directory( sourcePath, ec ))
		{
			auto finalDestPath = backupPath / sourcePath.parent_path().filename();

			// Create subdirectory
			if( !std::filesystem::create_directory( finalDestPath, ec ))
			{
				if( ec && ec != std::errc::file_exists )
				{
					Console.Error( oldstrutil::format( "Backup error creating destination subdirectory '%s'. Reason: %s (%d)", finalDestPath.string().c_str(), ec.message().c_str(), ec.value() ));
				}
			}

			// Perform recursive copy
			try
			{
				std::filesystem::copy( sourcePath, finalDestPath, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing );
			}
			catch( const std::filesystem::filesystem_error &e )
			{
				Console.Error( oldstrutil::format( "Backup error copying '%s' to '%s'. Error: %s (code: %d)", sourcePath.string().c_str(), finalDestPath.string().c_str(), e.code().message().c_str(), e.code().value() ));
			}
		}
		else
		{
			if( ec )
			{
				Console.Error( oldstrutil::format( "Backup error while performing type check for path ('%s'): %s.", sourcePath.string().c_str(), ec.message().c_str() ));
			}
			else
			{
				Console.Warning( oldstrutil::format( "Path ('%s') is not a directory, skipping.", sourcePath.string().c_str() ));
			}			
		}
	}
	else
	{
		if( ec )
		{
			Console.Error( oldstrutil::format( "Backup error while checking for existence of directory ('%s'): %s.", sourcePath.string().c_str(), ec.message().c_str() ));
		}
		else
		{
			Console.Warning( oldstrutil::format( "Directory ('%s') not found during backup, skipping.", sourcePath.string().c_str() ));
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	FileArchive()
//|	Date		-	24th September, 2001
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Makes a backup copy of the current world state
//o------------------------------------------------------------------------------------------------o
void FileArchive( void )
{
	auto mytime = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
	struct tm ttemp;
	char tbuffer[100];
	auto timenow = oldstrutil::simplify( asciitime( tbuffer, 100, *lcltime( mytime, ttemp )));
	timenow = std::regex_replace( timenow, std::regex( "[\\s:]" ), std::string( "-" ));

	std::string backupRoot	= cwmWorldState->ServerData()->Directory( CSDDP_BACKUP );
	backupRoot				+= timenow;

	std::error_code ec;
	auto makeResult = std::filesystem::create_directories( std::filesystem::path( backupRoot ), ec );

	if( makeResult )
	{
		Console << "Backing up world data, accounts, books and message board posts...";
		auto sharedPath = cwmWorldState->ServerData()->Directory( CSDDP_SHARED );
		BackupEntireDirectory( sharedPath, backupRoot );
		auto booksPath = cwmWorldState->ServerData()->Directory( CSDDP_BOOKS );
		BackupEntireDirectory( booksPath, backupRoot );
		auto msgBoardPath = cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD );
		BackupEntireDirectory( msgBoardPath, backupRoot );
		auto accountsPath = cwmWorldState->ServerData()->Directory( CSDDP_ACCOUNTS );
		BackupEntireDirectory( accountsPath, backupRoot );
		Console << "Finished backup" << myendl;
	}
	else
	{
		if( ec )
		{
			Console.Error( oldstrutil::format( "Unable to create backup directory at '%s'. Reason: %s", backupRoot.c_str(), ec.message().c_str() ));
		}
		else
		{
			Console << "Cannot create backup directory, please check available disk space" << myendl;
		}
	}
}
