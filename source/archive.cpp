#include <chrono>
#include <ctime>
#include <filesystem>
#include <regex>

#include "subsystem/console.hpp"
#include "osunique.hpp"
#include "regions.h"
#include "configuration/serverconfig.hpp"
#include "utility/strutil.hpp"
#include "worldmain.h"


// o------------------------------------------------------------------------------------------------o
//|	Function	-	BackupFile()
//|	Date		-	11th April, 2002
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Makes a backup copy of a file in the shared directory.
//|                  puts the copy in the backup directory
// o------------------------------------------------------------------------------------------------o
static void BackupFile(const std::filesystem::path &filename, const std::filesystem::path &backupDir) {
    auto to = backupDir / filename;
    
    auto from = ServerConfig::shared().directoryFor(dirlocation_t::SAVE) / filename;
    std::filesystem::copy_file(from, to);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	FileArchive()
//|	Date		-	24th September, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Makes a backup copy of the current world state
// o------------------------------------------------------------------------------------------------o
void FileArchive() {
    Console::shared() << "Beginning backup... ";
    auto mytime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm ttemp;
    char tbuffer[100];
    auto timenow = util::simplify(asciitime(tbuffer, 100, *lcltime(mytime, ttemp)));
    timenow = std::regex_replace(timenow, std::regex("[\\s:]"), std::string("-"));
    
    auto backupRoot = ServerConfig::shared().directoryFor(dirlocation_t::BACKUP).string()/std::filesystem::path(timenow);
    
    auto makeResult = std::filesystem::create_directory(std::filesystem::path(backupRoot));
    
    if (makeResult) {
        Console::shared()<< "NOTICE: Accounts not backed up. Archiving will change. Sorry for the trouble."<< myendl;
        
        BackupFile(std::filesystem::path("house.wsc"), backupRoot);
        
        // effect backups
        BackupFile(std::filesystem::path("effects.wsc"), backupRoot);
        
        const std::int16_t AreaX = UpperX / 8; // we're storing 8x8 grid arrays together
        const std::int16_t AreaY = UpperY / 8;
        
        
        for (std::int16_t counter1 = 0; counter1 < AreaX; ++counter1){ // move left->right
            
            for (std::int16_t counter2 = 0; counter2 < AreaY; ++counter2) {// move up->down
                auto filename1 = std::filesystem::path(util::format("%i.%i.wsc", counter1, counter2));
                BackupFile(filename1, backupRoot);
            }
        }
        BackupFile(std::filesystem::path("overflow.wsc"), backupRoot);
        BackupFile(std::filesystem::path("jails.wsc"), backupRoot);
        BackupFile(std::filesystem::path("guilds.wsc"), backupRoot);
        BackupFile(std::filesystem::path("regions.wsc"), backupRoot);
        BackupFile(std::filesystem::path("extra.wsc"), backupRoot);
   }
    else {
        Console::shared() << "Cannot create backup directory, please check available disk space" << myendl;
    }
    Console::shared() << "Finished backup" << myendl;
}
