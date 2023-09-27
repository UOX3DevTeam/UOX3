// o------------------------------------------------------------------------------------------------o
/*
 
 Ultima Offline eXperiment III (UOX3)
 UO Server Emulation Program
 
 Copyright 1998 - 2021 by UOX3 contributors
 Copyright 1997, 98 by Marcus Rating (Cironian)
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
 * In addition to that license, if you are running this program or modified  *
 * versions of it on a public system you HAVE TO make the complete source of *
 * the version used by you available or provide people with a location to    *
 * download it.                                                              *
 
 You can contact the author by sending email to <cironian@stratics.com>.
 
 */
// o------------------------------------------------------------------------------------------------o

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <numeric>
#include <optional>
#include <random>
#include <set>
#include <stdexcept>
#include <string>
#include <thread>
#if defined(_WIN32)
#include <process.h>
#else
#include <sys/signal.h>
#endif

//=====================================================================================================

#include "books.h"
#include "cchar.h"
#include "ceffects.h"
#include "cguild.h"
#include "cgump.h"
#include "chtmlsystem.h"
#include "citem.h"
#include "cjsengine.h"
#include "cjsmapping.h"
#include "classes.h"
#include "cmagic.h"
#include "cmultiobj.h"
#include "combat.h"
#include "commands.h"
#include "cpacketsend.h"
#include "craces.h"
#include "cscript.h"
#include "cserverdefinitions.h"
#include "csocket.h"
#include "cspawnregion.h"
#include "cthreadqueue.h"
#include "dictionary.h"
#include "eventtimer.hpp"
#include "funcdecl.h"
#include "jail.h"
#include "configuration/serverconfig.hpp"
#include "subsystem/account.hpp"
#include "subsystem/console.hpp"

#include "magic.h"
#include "mapstuff.h"
#include "movement.h"
#include "msgboard.h"
#include "network/networkmgr.hpp"
#include "utility/netutil.hpp"
#include "objectfactory.h"
#include "ostype.h"
#include "pagevector.h"
#include "partysystem.h"
#include "regions.h"
#include "skills.h"
#include "speech.h"
#include "ssection.h"
#include "stringutility.hpp"
#include "teffect.h"
#include "townregion.h"
#include "typedefs.h"
#include "utility/strutil.hpp"
#include "uodata/uomgr.hpp"
#include "other/uoxversion.hpp"
#include "type/weather.hpp"
#include "weight.h"
#include "wholist.h"

extern WorldWeather worldWeather;
extern CDictionaryContainer worldDictionary ;
extern CHandleCombat worldCombat ;
extern WorldItem worldItem ;
extern CCharStuff worldNPC ;
extern CSkills worldSkill ;
extern CMagic worldMagic ;
extern cRaces worldRace ;
extern CMovement worldMovement ;
extern CBooks worldBook ;
extern PageVector worldGMQueue ;
extern PageVector worldCounselorQueue ;
extern CJSMapping worldJSMapping ;
extern cEffects worldEffect ;
extern cHTMLTemplates worldHTMLTemplate;
extern CGuildCollection worldGuildSystem ;
extern CJailSystem worldJailSystem ;
extern CSpeechQueue worldSpeechSystem ;
extern CJSEngine worldJSEngine ;
extern CServerDefinitions worldFileLookup ;
extern CCommands serverCommands ;
extern CMulHandler worldMULHandler ;
extern CNetworkStuff worldNetwork ;
extern CMapHandler worldMapHandler ;
//=====================================
//extern uo::UOMgr uoManager ;
uo::UOMap uoManager;
using namespace std::string_literals;
// o------------------------------------------------------------------------------------------------o
//  Global variables
// o------------------------------------------------------------------------------------------------o
std::thread cons;
std::thread netw;
std::chrono::time_point<std::chrono::system_clock> current;
std::mt19937 generator;
std::random_device rd; // Will be used to obtain a seed for the random number engine

// o------------------------------------------------------------------------------------------------o
//  These should be atomic, for another day
// o------------------------------------------------------------------------------------------------o
auto isWorldSaving = false;
auto conThreadCloseOk = false;
auto netpollthreadclose = false;
auto saveOnShutdown = false;

// o------------------------------------------------------------------------------------------------o
//  Classes we will use
// o------------------------------------------------------------------------------------------------o
//  Non depdendent class

// Dependent or have startup()

// o------------------------------------------------------------------------------------------------o
//  FileIO Pre-Declarations
// o------------------------------------------------------------------------------------------------o
void loadCustomTitle();
void loadSkills();
void loadSpawnRegions();
void loadRegions();
void loadTeleportLocations();
void loadCreatures();
void loadPlaces();

// o------------------------------------------------------------------------------------------------o
//  Misc Pre-Declarations
// o------------------------------------------------------------------------------------------------o
auto restockNPC(CChar &i, bool stockAll)->void;
auto clearTrades()->void;
auto sysBroadcast(const std::string &txt)->void;
auto moveBoat(std::uint8_t dir, CBoatObj *boat)->void;
auto decayItem(CItem &toDecay, const std::uint32_t nextDecayItems, const std::uint32_t nextDecayItemsInHouses) ->bool;
auto checkArtificialIntelligence(CChar &mChar)->void;
// o------------------------------------------------------------------------------------------------o
//  Internal Pre-Declares
// o------------------------------------------------------------------------------------------------o
#if defined(_WIN32)
BOOL WINAPI exit_handler(DWORD dwCtrlType);
#else
auto app_stopped(int sig) ->void;
#endif
auto endMessage(std::int32_t x) -> void;
auto initClasses() -> void;
auto initMultis() -> void;
auto displayBanner() -> void;
auto checkConsoleKeyThread() -> void;
auto doMessageLoop() -> void;
//auto startInitialize(CServerData &server_data) -> void;
auto startInitialize() -> void;
auto initOperatingSystem() -> void;
auto adjustInterval(std::chrono::milliseconds interval, std::chrono::milliseconds maxTimer)-> std::chrono::milliseconds;

// o------------------------------------------------------------------------------------------------o
//|	Function	-	main()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Main UOX startup
// o------------------------------------------------------------------------------------------------o
auto main(std::int32_t argc, char *argv[]) -> int {
    std::uint32_t tempSecs, tempMilli, tempTime;
    std::uint32_t loopSecs, loopMilli;
    timerval_t uiNextCheckConn = 0;
    // We are going to do some fundmental checks, that if fail, we will bail out before
    // setting up
    auto configFile = std::filesystem::path("uox.ini");
    if (argc > 1) {
        configFile = std::filesystem::path(argv[1]);
    }
    try{
        // If we cant read the config file, should we even do anything else ?
        ServerConfig::shared().loadConfig(configFile);
        // Lets get the uodata now, otherwise, we can stop
        //uoManager.load(ServerConfig::shared().directoryFor(dirlocation_t::UODIR),ServerConfig::shared().directoryFor(dirlocation_t::DEFINITION)/std::filesystem::path("maps"),true,ServerConfig::shared().serverSwitch[ServerSwitch::MAPDIFF]) ;
        //std::cout <<"Tile information - Terrain: "<<uoManager.terrainSize()<<" Art: " << uoManager.artSize()<<std::endl;
        //std::cout <<"Multi Information: "<<uoManager.sizeMulti()<<std::endl;
        initOperatingSystem() ;
        return 0;
    }
    catch( const std::exception &e){
        std::cerr <<e.what() << std::endl;
        return EXIT_FAILURE ;
    }
    
    // Ok, we probably want the Console now
    Console::shared().Initialize();
    
    Console::shared().start(util::format("%s v%s.%s (%s)", UOXVersion::productName.c_str(), UOXVersion::version.c_str(), UOXVersion::build.c_str(), OS_STR));
    Console::shared().printSectionBegin();
    Console::shared() << "UOX Server start up!" << myendl << "Welcome to " << UOXVersion::productName << " v" << UOXVersion::version << "." << UOXVersion::build << " (" << OS_STR << ")" << myendl;
    Console::shared().printSectionBegin();
    startInitialize();
    // Main Loop
    Console::shared().printSectionBegin();
    EVENT_TIMER(stopwatch, EVENT_TIMER_OFF);
    
    // Initiate APS - Adaptive Performance System
    // Tracks simulation cycles over time and adjusts how often NPC AI/Pathfinding is updated as
    // necessary to keep shard performance and responsiveness to player input at acceptable levels
    auto apsPerfThreshold = ServerConfig::shared().ushortValues[UShortValue::APSPERFTHRESHOLD] ; // Performance threshold from ini, 0 disables APS feature
    
    [[maybe_unused]] int avgSimCycles = 0;
    std::uint16_t apsMovingAvgOld = 0;
    const int maxSimCycleSamples = 10; // Max number of samples for moving average
    std::vector<int> simCycleSamples;  // Store simulation cycle measurements for moving average
    
    // Fetch step value used by APS to gradually adjust delay in NPC AI/movement checking
    const std::chrono::milliseconds apsDelayStep = std::chrono::milliseconds(ServerConfig::shared().ushortValues[UShortValue::APSDELAYSTEP]);
    
    // Fetch max cap for delay introduced into loop by APS for checking NPC AI/movement stuff
    const std::chrono::milliseconds apsDelayMaxCap = std::chrono::milliseconds(ServerConfig::shared().ushortValues[UShortValue::APSDELAYMAX]);
    
    // Setup initial values for apsDelay
    std::chrono::milliseconds apsDelay = std::chrono::milliseconds(0);
    std::chrono::time_point<std::chrono::system_clock> adaptivePerfTimer = std::chrono::system_clock::now() + apsDelay;
    
    // Set the interval for APS evaluation and adjustment, and set initial timestamp for first
    // evaluation
    const std::chrono::milliseconds evaluationInterval =
    std::chrono::milliseconds(ServerConfig::shared().ushortValues[UShortValue::APSINTERVAL]);
    std::chrono::time_point<std::chrono::system_clock> nextEvaluationTime = std::chrono::system_clock::now() + evaluationInterval;
    
    // Core server loop
    while (worldMain.GetKeepRun()) {
        std::this_thread::sleep_for( std::chrono::milliseconds(worldMain.GetPlayersOnline() ? 5 : 90));
        if (worldMain.ServerProfile()->LoopTimeCount() >= 1000) {
            worldMain.ServerProfile()->LoopTimeCount(0);
            worldMain.ServerProfile()->LoopTime(0);
        }
        worldMain.ServerProfile()->IncLoopTimeCount();
        
        StartMilliTimer(loopSecs, loopMilli);
        
        if (worldMain.ServerProfile()->NetworkTimeCount() >= 1000) {
            worldMain.ServerProfile()->NetworkTimeCount(0);
            worldMain.ServerProfile()->NetworkTime(0);
        }
        
        StartMilliTimer(tempSecs, tempMilli);
        EVENT_TIMER_RESET(stopwatch);
        
        if (uiNextCheckConn <= worldMain.GetUICurrentTime() || worldMain.GetOverflow()) {
            // Cut lag on CheckConn by not doing it EVERY loop.
            worldNetwork.CheckConnections();
            uiNextCheckConn = BuildTimeValue(1.0f);
        }
        worldNetwork.CheckMessages();
        EVENT_TIMER_NOW(stopwatch, Complete net checkmessages, EVENT_TIMER_KEEP);
        tempTime = CheckMilliTimer(tempSecs, tempMilli);
        worldMain.ServerProfile()->IncNetworkTime(tempTime);
        worldMain.ServerProfile()->IncNetworkTimeCount();
        
        if (worldMain.ServerProfile()->TimerTimeCount() >= 1000) {
            worldMain.ServerProfile()->TimerTimeCount(0);
            worldMain.ServerProfile()->TimerTime(0);
        }
        
        StartMilliTimer(tempSecs, tempMilli);
        
        worldMain.CheckTimers();
        // stopwatch.output( "Delta for CheckTimers" );
        worldMain.SetUICurrentTime(GetClock());
        tempTime = CheckMilliTimer(tempSecs, tempMilli);
        worldMain.ServerProfile()->IncTimerTime(tempTime);
        worldMain.ServerProfile()->IncTimerTimeCount();
        
        if (worldMain.ServerProfile()->AutoTimeCount() >= 1000) {
            worldMain.ServerProfile()->AutoTimeCount(0);
            worldMain.ServerProfile()->AutoTime(0);
        }
        StartMilliTimer(tempSecs, tempMilli);
        
        if (!worldMain.GetReloadingScripts()) {
            // auto stopauto = EventTimer();
            EVENT_TIMER(stopauto, EVENT_TIMER_OFF);
            
            std::chrono::time_point<std::chrono::system_clock> currentTime = std::chrono::system_clock::now();
            if (apsPerfThreshold == static_cast<std::uint16_t>(0) || apsDelay == std::chrono::milliseconds(0) || currentTime >= adaptivePerfTimer) {
                // Check autotimers if the APS feature is disabled, if there's no delay, or if timer
                // has expired
                worldMain.CheckAutoTimers();
                
                // Set timer for next update, if apsDelay is higher than 0
                if (apsDelay > std::chrono::milliseconds(0)) {
                    adaptivePerfTimer = std::chrono::system_clock::now() + apsDelay;
                }
            }
            
            EVENT_TIMER_NOW(stopauto, CheckAutoTimers only, EVENT_TIMER_CLEAR);
        }
        
        tempTime = CheckMilliTimer(tempSecs, tempMilli);
        worldMain.ServerProfile()->IncAutoTime(tempTime);
        worldMain.ServerProfile()->IncAutoTimeCount();
        StartMilliTimer(tempSecs, tempMilli);
        EVENT_TIMER_RESET(stopwatch);
        worldNetwork.ClearBuffers();
        EVENT_TIMER_NOW( stopwatch, Delta for ClearBuffers, EVENT_TIMER_CLEAR );
        tempTime = CheckMilliTimer(tempSecs, tempMilli);
        worldMain.ServerProfile()->IncNetworkTime(tempTime);
        tempTime = CheckMilliTimer(loopSecs, loopMilli);
        worldMain.ServerProfile()->IncLoopTime(tempTime);
        EVENT_TIMER_RESET(stopwatch);
        doMessageLoop();
        EVENT_TIMER_NOW( stopwatch, Delta for doMessageLoop, EVENT_TIMER_CLEAR );
        
        // Check if it's time for evaluation and adjustment
        std::chrono::time_point<std::chrono::system_clock> currentTime =
        std::chrono::system_clock::now();
        if (apsPerfThreshold > static_cast<std::uint16_t>(0) && currentTime >= nextEvaluationTime) {
            // Get simulation cycles count
            auto simCycles = (1000.0 * (1.0 / static_cast<float>( static_cast<float>(worldMain.ServerProfile()->LoopTime()) / static_cast<float>(worldMain.ServerProfile()->LoopTimeCount()))));
            
            // Store last X simCycle samples
            simCycleSamples.push_back(simCycles);
            
            // Limit the number of samples kept to X
            if (simCycleSamples.size() > maxSimCycleSamples) {
                simCycleSamples.erase(simCycleSamples.begin());
            }
            
            int sum = std::accumulate(simCycleSamples.begin(), simCycleSamples.end(), 0);
            std::uint16_t apsMovingAvg = static_cast<std::uint16_t>(sum / simCycleSamples.size());
            if (apsMovingAvg < apsPerfThreshold) {
                // Performance is below threshold...
                if (apsMovingAvg <= apsMovingAvgOld && apsDelay < apsDelayMaxCap) {
                    // ... and dropping, or stable at low performance! DO SOMETHING...
                    apsDelay = apsDelay + apsDelayStep;
#if defined(UOX_DEBUG_MODE)
                    Console::shared() << "Performance below threshold! Increasing adaptive performance timer: " << apsDelay.count() << "ms" << "\n";
#endif
                }
                // If performance is below, but increasing, wait and see before reacting
            }
            else {
                // Performance exceeds threshold...
                if (apsDelay >= apsDelayStep) {
                    // ... reduce timer for snappier NPC AI/movement/etc.
                    apsDelay = apsDelay - apsDelayStep;
#if defined(UOX_DEBUG_MODE)
                    Console::shared() << "Performance exceeds threshold. Decreasing adaptive performance timer: "  << apsDelay.count() << "ms" << "\n";
#endif
                }
            }
            
            // Update previous moving average
            apsMovingAvgOld = apsMovingAvg;
            
            // Adjust the interval based on the timer value
            std::chrono::milliseconds adjustedInterval =
            adjustInterval(evaluationInterval, apsDelay);
            
            // Update the next evaluation time
            nextEvaluationTime = currentTime + adjustedInterval;
        }
    }
    
    // Shutdown/Cleanup
    sysBroadcast("The server is shutting down.");
    Console::shared() << "Closing sockets...";
    netpollthreadclose = true;
    /// HERE
    worldNetwork.SockClose();
    Console::shared().printDone();
    util::net::shutdown() ;
    
#if defined(_WIN32)
    SetConsoleCtrlHandler(exit_handler, true);
#endif
    if (worldMain.GetWorldSaveProgress() != SS_SAVING) {
        isWorldSaving = true;
        do {
            worldMain.SaveNewWorld(true);
        }
        while (worldMain.GetWorldSaveProgress() == SS_SAVING);
        isWorldSaving = false;
    }
    //worldMain.ServerData()->SaveIni();
    ServerConfig::shared().writeConfig(std::filesystem::path());
#if defined(_WIN32)
    SetConsoleCtrlHandler(exit_handler, false);
#endif
    
    Console::shared().log("Server Shutdown!\n=======================================================================\n","server.log");
    
    conThreadCloseOk = true; //	This will signal the console thread to close
    Shutdown(0);
    
    // Will never reach this, as Shutdown "exits"
    return EXIT_SUCCESS;
}

// Scaling function to adjust the interval based on the timer value
auto adjustInterval(std::chrono::milliseconds interval, std::chrono::milliseconds maxTimer) -> std::chrono::milliseconds {
    double scaleFactor = static_cast<double>(maxTimer.count()) / interval.count();
    double adjustmentFactor = 0.25; // Adjust this factor to control the rate of adjustment
    long long adjustedCount =
    static_cast<long long>(interval.count() * (1.0 + scaleFactor * adjustmentFactor));
    return std::chrono::milliseconds(adjustedCount);
}

// o------------------------------------------------------------------------------------------------o
//  Initialize the network
// o------------------------------------------------------------------------------------------------o
auto initOperatingSystem() -> void {
#if !defined(_WIN32)
    // Protection from server-shutdown during mid-worldsave
    signal(SIGINT, app_stopped);
#endif
}

// o------------------------------------------------------------------------------------------------o
//  Startup and Initialization
// o------------------------------------------------------------------------------------------------o
auto startInitialize() -> void {
    saveOnShutdown = false;
    // Let's measure startup time
    auto startupStartTime = std::chrono::high_resolution_clock::now();
    
//    cwmWorldState = &aWorld;
    
    Console::shared() << "Initializing and creating class pointers... " << myendl;
    initClasses();
    worldMain.SetUICurrentTime(GetClock());
    
    Console::shared().printSectionBegin();
    
    worldMain.uoTime.load(ServerConfig::shared().directoryFor(dirlocation_t::SAVE));
    
    Console::shared() << "Loading skill advancement      ";
    loadSkills();
    Console::shared().printDone();
    // Moved BulkStartup here, dunno why that function was there...
    Console::shared() << "Loading dictionaries...        " << myendl;
    Console::shared().printBasedOnVal( worldDictionary.LoadDictionaries(ServerConfig::shared().directoryFor(dirlocation_t::LANGUAGE)) >= 0);
    
    Console::shared() << "Loading teleport               ";
    loadTeleportLocations();
    Console::shared().printDone();
    
    Console::shared() << "Loading GoPlaces               ";
    loadPlaces();
    Console::shared().printDone();
    generator = std::mt19937(rd()); // Standard mersenne_twister_engine seeded with rd()
    
    auto packetSection = worldJSMapping.GetSection(CJSMappingSection::SCPT_PACKET);
    for (const auto &[id, ourScript] : packetSection->collection()) {
        if (ourScript) {
            ourScript->ScriptRegistration("Packet");
        }
    }
    
    worldSkill.load();
    
    Console::shared() << "Loading Spawn Regions          ";
    loadSpawnRegions();
    Console::shared().printDone();
    
    Console::shared() << "Loading Regions                ";
    loadRegions();
    Console::shared().printDone();
    
    worldMagic.LoadScript();
    
    Console::shared() << "Loading Races                  ";
    worldRace.load();
    Console::shared().printDone();
    
    Console::shared() << "Loading Weather                ";
    worldWeather.load(ServerConfig::shared().directoryFor(dirlocation_t::DEFINITION)/std::filesystem::path("weather")/std::filesystem::path("weatherab.dfn"));
    worldWeather.newDay();
    worldWeather.newHour();
    Console::shared().printDone();
    
    Console::shared() << "Loading serverCommands               " << myendl;
    serverCommands.load();
    Console::shared().printDone();
    
    // Rework that...
    Console::shared() << "Loading World now              ";
    worldMapHandler.load();
    
    Console::shared() << "Loading Guilds                 ";
    worldGuildSystem.load();
    Console::shared().printDone();
    
    Console::shared().printSectionBegin();
    Console::shared() << "Clearing all trades            ";
    clearTrades();
    Console::shared().printDone();
    initMultis();
    
    worldMain.SetStartTime(worldMain.GetUICurrentTime());
    
    worldMain.SetEndTime(0);
    worldMain.SetLClock(0);
    
    // no longer Que, because that's taken care of by PageVector
    Console::shared() << "Initializing Jail system       ";
    worldJailSystem.ReadSetup();
    worldJailSystem.ReadData();
    Console::shared().printDone();
    
    Console::shared() << "Initializing Status system     ";
    worldHTMLTemplate.load();
    Console::shared().printDone();
    
    Console::shared() << "Loading custom titles          ";
    loadCustomTitle();
    Console::shared().printDone();
    
    Console::shared() << "Loading temporary Effects      ";
    worldEffect.LoadEffects();
    Console::shared().printDone();
    
    Console::shared() << "Loading creatures              ";
    loadCreatures();
    Console::shared().printDone();
    
    Console::shared() << "Starting World Timers          ";
    worldMain.SetTimer(tWORLD_LIGHTTIME, BuildTimeValue(static_cast<float>(ServerConfig::shared().timerSetting[TimerSetting::WEATHER])) );
    
    worldMain.SetTimer(tWORLD_NEXTNPCAI, BuildTimeValue(static_cast<float>( ServerConfig::shared().realNumbers[RealNumberConfig::CHECKAI])));
    worldMain.SetTimer(tWORLD_NEXTFIELDEFFECT, BuildTimeValue(0.5f));
    worldMain.SetTimer(tWORLD_SHOPRESTOCK, BuildTimeValue(static_cast<float>(ServerConfig::shared().timerSetting[TimerSetting::SHOPSPAWN])));
    worldMain.SetTimer( tWORLD_PETOFFLINECHECK, BuildTimeValue(static_cast<float>(ServerConfig::shared().timerSetting[TimerSetting::PETOFFLINECHECK])));
    
    Console::shared().printDone();
    
    displayBanner();
    
    Console::shared() << "Loading Accounts               ";
    Account::shared().load();
    Console::shared().printDone();
    
    Console::shared().log("-=Server Startup=-\n=======================================================================","server.log");
    
    Console::shared() << "Creating and Initializing Console Thread      ";
    
    cons = std::thread(&checkConsoleKeyThread);
    
    Console::shared().printDone();
    
    // Shows information about IPs and ports being listened on
    Console::shared().turnYellow();
    
    auto externalIP = ServerConfig::shared().serverString[ServerString::PUBLICIP];
    if (externalIP != "" && externalIP != "localhost" && externalIP != "127.0.0.1") {
        Console::shared() << "UOX: listening for incoming connections on External/WAN IP: " << externalIP.c_str() << myendl;
    }
    /*
    auto deviceIPs = ip4list_t::available();
    for (auto &entry : deviceIPs.ips()) {
        switch (entry.type()) {
            case IP4Addr::ip4type_t::lan:
                Console::shared() << "UOX: listening for incoming connections on LAN IP: " << entry.description() << myendl;
                break;
            case IP4Addr::ip4type_t::local:
                Console::shared() << "UOX: listening for incoming connections on Local IP: " << entry.description() << myendl;
                break;
            case IP4Addr::ip4type_t::wan:
                Console::shared() << "UOX: listening for incoming connections on WAN IP: " << entry.description() << myendl;
                break;
            default:
                Console::shared() << "UOX: listening for incoming connections on IP: " << entry.description() << myendl;
                break;
        }
    }
     */
    Console::shared().turnNormal();
    
    // we've really finished loading here
    worldMain.SetLoaded(true);
    
    // Get a second timestamp for startup time
    auto startupEndTime = std::chrono::high_resolution_clock::now();
    
    // Calculate startup time in milliseconds
    auto startupDuration = std::chrono::duration_cast<std::chrono::milliseconds>(startupEndTime - startupStartTime) .count();
    Console::shared().turnGreen();
    Console::shared() << "UOX: Startup Completed in " << static_cast<float>(startupDuration) / 1000 << " seconds." << myendl;
    Console::shared().turnNormal();
}

// o------------------------------------------------------------------------------------------------o
//  Most things after this point, should be in different files, not in uox3.cpp.

// o------------------------------------------------------------------------------------------------o
//  Signal and exit handers
// o------------------------------------------------------------------------------------------------o
#if defined(_WIN32)
// o------------------------------------------------------------------------------------------------o
//|	Function	-	exit_handler()
//|					app_stopped()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Prevent closing of console via CTRL+C/or CTRL+BREAK keys during
// worldsaves
// o------------------------------------------------------------------------------------------------o
BOOL WINAPI exit_handler(DWORD dwCtrlType) {
    switch (dwCtrlType) {
        case CTRL_C_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_CLOSE_EVENT:
            std::cout << std::endl << "World save in progress - closing UOX3 before it completes may result in corrupted save data!" << std::endl;
            // Shutdown of the application will only be halted for as long as the exit_handler
            // is doing something, so do some non-work while isWorldSaving is true to prevent
            // shutdown during save
            while (isWorldSaving == true) {
                Sleep(0);
            }
            return true;
        default:
            return false;
    }
}
#else
void app_stopped([[maybe_unused]] int sig) {
    // function called when signal is received.
    if (isWorldSaving == false) {
        worldMain.SetKeepRun(false);
    }
}
#endif

// o------------------------------------------------------------------------------------------------o
//  Spawn related
// o------------------------------------------------------------------------------------------------o

// o------------------------------------------------------------------------------------------------o
//|	Function	-	UnloadSpawnRegions()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Unload spawn regions on server shutdown or when reloading spawn
// regions
// o------------------------------------------------------------------------------------------------o
auto UnloadSpawnRegions() -> void {
    for (auto &[regionnum, spawnregion] : worldMain.spawnRegions) {
        if (spawnregion) {
            // Iterate over list of spawned characters and delete them if no player has
            // tamed them/hired them
            std::vector<CChar *> toDelete;
            auto spawnedCharsList = spawnregion->GetSpawnedCharsList();
            for (const auto &cCheck : spawnedCharsList->collection()) {
                if (ValidateObject(cCheck)) {
                    if (!ValidateObject(cCheck->GetOwnerObj())) {
                        toDelete.push_back(cCheck);
                    }
                }
            }
            std::for_each(toDelete.begin(), toDelete.end(), [](CChar *entry) {
                entry->Delete();
            });
            // Iterate over list of spawned items and delete them if no player has picked
            // them up
            std::vector<CItem *> toIDelete;
            auto spawnedItemsList = spawnregion->GetSpawnedItemsList();
            for (const auto &iCheck : spawnedItemsList->collection()) {
                if (ValidateObject(iCheck)) {
                    if (iCheck->GetContSerial() != INVALIDSERIAL || !ValidateObject(iCheck->GetOwnerObj())) {
                        toIDelete.push_back(iCheck);
                    }
                }
            }
            std::for_each(toIDelete.begin(), toIDelete.end(),
                          [](CItem *entry) { entry->Delete(); });
            
            delete spawnregion;
        }
    }
    worldMain.spawnRegions.clear();
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	UnloadRegions()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Unload town regions on server shutdown or when reloading town
// regions
// o------------------------------------------------------------------------------------------------o
auto UnloadRegions() -> void {
    std::for_each(worldMain.townRegions.begin(), worldMain.townRegions.end(), [](const std::pair<std::uint16_t, CTownRegion *> &entry) {
        if (entry.second) {
            delete entry.second;
        }
    });
    worldMain.townRegions.clear();
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	doMessageLoop()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Watch for messages thrown by UOX
// o------------------------------------------------------------------------------------------------o
auto doMessageLoop() -> void {
    // Grab all the data in the queue
    auto messages = messageLoop.BulkData();
    while (!messages.empty()) {
        auto tVal = messages.front();
        messages.pop();
        switch (tVal.actualMessage) {
            case MSG_SHUTDOWN:
                worldMain.SetKeepRun(false);
                break;
            case MSG_COUNT:
                break;
            case MSG_WORLDSAVE:
                worldMain.SetOldTime(0);
                break;
            case MSG_PRINT:
                Console::shared() << tVal.data << myendl;
                break;
            case MSG_RELOADJS:
                worldJSEngine.Reload();
                worldJSMapping.Reload();
                Console::shared().printDone();
                serverCommands.load();
                break;
            case MSG_CONSOLEBCAST:
                sysBroadcast(tVal.data);
                break;
            case MSG_PRINTDONE:
                Console::shared().printDone();
                break;
            case MSG_PRINTFAILED:
                Console::shared().printFailed();
                break;
            case MSG_SECTIONBEGIN:
                Console::shared().printSectionBegin();
                break;
            case MSG_RELOAD:
                if (!worldMain.GetReloadingScripts()) {
                    worldMain.SetReloadingScripts(true);
                    switch (tVal.data[0]) {
                        case '0':
                            ServerConfig::shared().loadConfig(std::filesystem::path());
                            
                            break; // Reload INI file
                        case '1':
                            Account::shared().load();
                            break; // Reload accounts
                        case '2':  // Reload regions/TeleportLocations
                            UnloadRegions();
                            loadRegions();
                            loadTeleportLocations();
                            break;
                        case '3':
                            UnloadSpawnRegions();
                            loadSpawnRegions();
                            break; // Reload spawn regions
                        case '4':
                            worldMagic.LoadScript();
                            break; // Reload spells
                        case '5':  // Reload commands
                            worldJSMapping.Reload(CJSMappingSection::SCPT_COMMAND);
                            serverCommands.load();
                            break;
                        case '6': // Reload definition files
                            worldFileLookup.Reload();
                            loadCreatures();
                            loadCustomTitle();
                            loadSkills();
                            loadPlaces();
                            worldSkill.load();
                            break;
                        case '7': // Reload JS
                            worldJSEngine.Reload();
                            worldJSMapping.Reload();
                            Console::shared().printDone();
                            serverCommands.load();
                            worldSkill.load();
                            break;
                        case '8': // Reload HTML
                            worldHTMLTemplate.Unload();
                            worldHTMLTemplate.load();
                            break;
                    }
                    worldMain.SetReloadingScripts(false);
                }
                break;
            case MSG_UNKNOWN:
            default:
                Console::shared().error("Unknown message type");
                break;
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//  Threads
// o------------------------------------------------------------------------------------------------o

// o------------------------------------------------------------------------------------------------o
//|	Function	-	NetworkPollConnectionThread()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Watch for new connections
// o------------------------------------------------------------------------------------------------o
auto NetworkPollConnectionThread() -> void {
    messageLoop << "Thread: NetworkPollConnection has started";
    netpollthreadclose = false;
    while (!netpollthreadclose) {
        worldNetwork.CheckConnections();
        worldNetwork.CheckLoginMessage();
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    messageLoop << "Thread: NetworkPollConnection has Closed";
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	checkConsoleKeyThread()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Listen for key inputs in server console
// o------------------------------------------------------------------------------------------------o
auto checkConsoleKeyThread() -> void {
    messageLoop << "Thread: CheckConsoleThread has started";
    Console::shared().registration();
    conThreadCloseOk = false;
    while (!conThreadCloseOk) {
        Console::shared().poll();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    messageLoop << "Thread: checkConsoleKeyThread Closed";
}
// o------------------------------------------------------------------------------------------------o
//  Misc other functions
// o------------------------------------------------------------------------------------------------o

// o------------------------------------------------------------------------------------------------o
//|	Function	-	IsOnline()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if the socket owning character c is still connected
// o------------------------------------------------------------------------------------------------o
auto IsOnline(CChar &mChar) -> bool {
    auto rValue = false;
    if (!mChar.IsNpc()) {
        AccountEntry &actbTemp = mChar.GetAccount();
        if (actbTemp.accountNumber != AccountEntry::INVALID_ACCOUNT) {
            if (actbTemp.inGame == mChar.GetSerial()) {
                rValue = true;
            }
        }
        if (!rValue) {
            for (auto &tSock : worldNetwork.connClients) {
                if (tSock->CurrcharObj() == &mChar) {
                    rValue = true;
                    break;
                }
            }
        }
    }
    return rValue;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	UpdateStats()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Updates object's stats
// o------------------------------------------------------------------------------------------------o
auto UpdateStats(CBaseObject *mObj, std::uint8_t x) -> void {
    for (auto &tSock : FindNearbyPlayers(mObj)) {
        if (tSock->LoginComplete()) {
            // Normalize stats if we're updating our stats for other players
            auto normalizeStats = true;
            if (tSock->CurrcharObj()->GetSerial() == mObj->GetSerial()) {
                tSock->StatWindow(mObj);
                normalizeStats = false;
            }
            
            // Prepare the stat update packet
            CPUpdateStat toSend((*mObj), x, normalizeStats);
            
            // Send the stat update packet
            tSock->Send(&toSend);
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CollectGarbage()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Deletes objects in the Deletion Queue
// o------------------------------------------------------------------------------------------------o
auto CollectGarbage() -> void {
    Console::shared() << "Performing Garbage Collection...";
    auto objectsDeleted = std::uint32_t(0);
    std::for_each(worldMain.deletionQueue.begin(), worldMain.deletionQueue.end(),
                  [&objectsDeleted](std::pair<CBaseObject *, std::uint32_t> entry) {
        if (entry.first) {
            if (entry.first->IsFree() && entry.first->IsDeleted()) {
                Console::shared().warning("Invalid object found in Deletion Queue");
            }
            else {
                ObjectFactory::shared().DestroyObject(entry.first);
                ++objectsDeleted;
            }
        }
    });
    
    worldMain.deletionQueue.clear();
    
    Console::shared() << " Removed " << objectsDeleted << " objects";
    
    worldJSEngine.CollectGarbage();
    Console::shared().printDone();
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	MountCreature()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Mount a ridable creature
//|
//|	Changes		-	09/22/2002	-	Unhide players when mounting horses etc.
// o------------------------------------------------------------------------------------------------o
auto MountCreature(CSocket *sockPtr, CChar *s, CChar *x) -> void {
    if (s->IsOnHorse())
        return;
    
    // No mounting horses for gargoyles!
    if (s->GetId() == 0x029A || s->GetId() == 0x029B) {
        sockPtr->SysMessage(1798); // You cannot mount.
        return;
    }
    
    if (!ObjInRange(s, x, DIST_NEXTTILE))
        return;
    
    if (x->GetOwnerObj() == s || worldNPC.CheckPetFriend(s, x) || s->IsGM()) {
        if (!ServerConfig::shared().enabled(ServerSwitch::HIDEWHILEMOUNTED)) {
            s->ExposeToView();
        }
        
        s->SetOnHorse(true);
        auto c = worldItem.CreateItem(nullptr, s, 0x0915, 1, x->GetSkin(), CBaseObject::OT_ITEM);
        
        auto xName = GetNpcDictName(x, sockPtr, NRS_SYSTEM);
        c->SetName(xName);
        c->SetDecayable(false);
        c->SetLayer(IL_MOUNT);
        
        if (worldMain.creatures[x->GetId()].MountId() != 0) {
            c->SetId(worldMain.creatures[x->GetId()].MountId());
        }
        else {
            c->SetId(0x3E00);
        }
        
        if (!c->SetCont(s)) {
            s->SetOnHorse(false); // let's get off our horse again
            c->Delete();
            return;
        }
        else {
            for (auto &tSock : FindNearbyPlayers(s)) {
                s->SendWornItems(tSock);
            }
            
            if (x->GetTarg()) // zero out target, under all circumstances
            {
                x->SetTarg(nullptr);
                if (x->IsAtWar()) {
                    x->ToggleCombat();
                }
            }
            if (ValidateObject(x->GetAttacker())) {
                x->GetAttacker()->SetTarg(nullptr);
            }
            x->SetFrozen(true);
            x->SetMounted(true);
            x->SetInvulnerable(true);
            x->SetLocation(7000, 7000, 0);
            
            c->SetTempVar(CITV_MOREX, x->GetSerial());
            if (x->GetTimer(tNPC_SUMMONTIME) != 0) {
                c->SetDecayTime(x->GetTimer(tNPC_SUMMONTIME));
            }
        }
    }
    else {
        sockPtr->SysMessage(1214); // You don't own that creature.
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	DismountCreature()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Dismount a ridable creature
// o------------------------------------------------------------------------------------------------o
auto DismountCreature(CChar *s) -> void {
    if (ValidateObject(s)) {
        auto ci = s->GetItemAtLayer(IL_MOUNT);
        if (ValidateObject(ci) && !ci->IsFree()) {
            s->SetOnHorse(false);
            auto tMount = CalcCharObjFromSer(ci->GetTempVar(CITV_MOREX));
            if (ValidateObject(tMount)) {
                tMount->SetLocation(s);
                tMount->SetFrozen(false);
                tMount->SetMounted(false);
                tMount->SetInvulnerable(false);
                if (ci->GetDecayTime() != 0) {
                    tMount->SetTimer(tNPC_SUMMONTIME, ci->GetDecayTime());
                }
                tMount->SetDir(s->GetDir());
                tMount->SetVisible(VT_VISIBLE);
            }
            ci->Delete();
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	endMessage()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Global message players with shutdown message
// o------------------------------------------------------------------------------------------------o
auto endMessage([[maybe_unused]] std::int32_t x) -> void {
    x = 0; // Really, then why take a parameter?
    const std::uint32_t iGetClock = worldMain.GetUICurrentTime();
    if (worldMain.GetEndTime() < iGetClock) {
        worldMain.SetEndTime(iGetClock);
    }
    sysBroadcast(util::format(worldDictionary.GetEntry(1209), ((worldMain.GetEndTime() - iGetClock) / 1000) /  60)); // Server going down in %i minutes!
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CallGuards()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Used when a character calls "Guards" Will look for a criminal
//|					first checking for anyone attacking him. If no one is
// attacking
//|					him it will look for any people nearby who are criminal or
//|					murderers
// o------------------------------------------------------------------------------------------------o
auto CallGuards(CChar *mChar) -> void {
    if (ValidateObject(mChar) && mChar->GetRegion()->IsGuarded() && ServerConfig::shared().enabled(ServerSwitch::GUARDSACTIVE)) {
        auto attacker = mChar->GetAttacker();
        if (ValidateObject(attacker)) {
            if (!attacker->IsDead() && (attacker->IsCriminal() || attacker->IsMurderer())) {
                // Can only be called on criminals for the first 10 seconds of receiving the
                // criminal flag
                if (!attacker->IsMurderer() && attacker->IsCriminal() && mChar->GetTimer(tCHAR_CRIMFLAG) - worldMain.GetUICurrentTime() <= 10) {
                    // Too late!
                    return;
                }
                
                if (CharInRange(mChar, attacker)) {
                    worldCombat.SpawnGuard(mChar, attacker, attacker->GetX(), attacker->GetY(),  attacker->GetZ());
                    return;
                }
            }
        }
        
        auto toCheck = worldMapHandler.GetMapRegion(mChar);
        if (toCheck) {
            auto regChars = toCheck->GetCharList();
            for (const auto &tempChar : regChars->collection()) {
                if (ValidateObject(tempChar)) {
                    if (!tempChar->IsDead() && (tempChar->IsCriminal() || tempChar->IsMurderer())) {
                        // Can only be called on criminals for the first 10 seconds of
                        // receiving the criminal flag
                        if (!tempChar->IsMurderer() && tempChar->IsCriminal() && mChar->GetTimer(tCHAR_CRIMFLAG) - worldMain.GetUICurrentTime() <= 10) {
                            // Too late!
                            return;
                        }
                        
                        if (CharInRange(tempChar, mChar)) {
                            worldCombat.SpawnGuard(mChar, tempChar, tempChar->GetX(), tempChar->GetY(), tempChar->GetZ());
                            break;
                        }
                    }
                }
            }
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CallGuards()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Used when a character calls guards on another character, will
//|					ensure that character is not dead and is either a criminal
// or |					murderer, and that he is in visual range of the victim, will
//|					then spawn a guard to take care of the criminal.
// o------------------------------------------------------------------------------------------------o
auto CallGuards(CChar *mChar, CChar *targChar) -> void {
    if (ValidateObject(mChar) && ValidateObject(targChar)) {
        if (mChar->GetRegion()->IsGuarded() && ServerConfig::shared().enabled(ServerSwitch::GUARDSACTIVE)) {
            if (!targChar->IsDead() && (targChar->IsCriminal() || targChar->IsMurderer())) {
                // Can only be called on criminals for the first 10 seconds of receiving the
                // criminal flag
                if (!targChar->IsMurderer() && targChar->IsCriminal() && mChar->GetTimer(tCHAR_CRIMFLAG) - worldMain.GetUICurrentTime() <= 10) {
                    // Too late!
                    return;
                }
                
                if (CharInRange(mChar, targChar)) {
                    worldCombat.SpawnGuard(mChar, targChar, targChar->GetX(), targChar->GetY(), targChar->GetZ());
                }
            }
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	GenericCheck()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check characters status.  Returns true if character was killed
// o------------------------------------------------------------------------------------------------o
auto GenericCheck(CSocket *mSock, CChar &mChar, bool checkFieldEffects, bool doWeather) -> bool {
    std::uint16_t c;
    if (!mChar.IsDead()) {
        const auto maxHP = mChar.GetMaxHP();
        const auto maxStam = mChar.GetMaxStam();
        const auto maxMana = mChar.GetMaxMana();
        
        if (mChar.GetHP() > maxHP) {
            mChar.SetHP(maxHP);
        }
        if (mChar.GetStamina() > maxStam) {
            mChar.SetStamina(maxStam);
        }
        if (mChar.GetMana() > maxMana) {
            mChar.SetMana(maxMana);
        }
        
        if (mChar.GetRegen(0) <= worldMain.GetUICurrentTime() || worldMain.GetOverflow()) {
            if (mChar.GetHP() < maxHP) {
                if (!ServerConfig::shared().enabled(ServerSwitch::HUNGER) || (mChar.GetHunger() > 0) || (!worldRace.DoesHunger(mChar.GetRace()) && ((ServerConfig::shared().timerSetting[TimerSetting::HUNGERRATE] == 0) || mChar.IsNpc()))) {
                    for (auto c = 0; c <= maxHP; ++c) {
                        if (mChar.GetHP() <= maxHP && (mChar.GetRegen(0) + (c * ServerConfig::shared().timerSetting[TimerSetting::HITPOINTREGEN] * 1000)) <= worldMain.GetUICurrentTime()) {
                            if (mChar.GetSkill(HEALING) < 500) {
                                mChar.IncHP(1);
                            }
                            else if (mChar.GetSkill(HEALING) < 800) {
                                mChar.IncHP(2);
                            }
                            else {
                                mChar.IncHP(3);
                            }
                            if (mChar.GetHP() >= maxHP) {
                                mChar.SetHP(maxHP);
                                break;
                            }
                        }
                        else{ // either we're all healed up, or all time periods have passed
                            break;
                        }
                    }
                }
            }
            mChar.SetRegen( BuildTimeValue(static_cast<float>(ServerConfig::shared().timerSetting[TimerSetting::HITPOINTREGEN])), 0);
        }
        if (mChar.GetRegen(1) <= worldMain.GetUICurrentTime() || worldMain.GetOverflow()) {
            auto mStamina = mChar.GetStamina();
            if (mStamina < maxStam) {
                // Continue with stamina regen if  character is not yet fully parched, or if
                // character is parched but has less than 25% stamina, or if char belongs to
                // race that does not thirst
                if (!ServerConfig::shared().enabled(ServerSwitch::THIRST)|| (mChar.GetThirst() > 0) || ((mChar.GetThirst() == 0) && (mStamina < static_cast<std::int16_t>(maxStam * 0.25))) || (!worldRace.DoesThirst(mChar.GetRace()) && ( ServerConfig::shared().timerSetting[TimerSetting::THIRSTRATE] == 0 || mChar.IsNpc()))) {
                    for (auto c = 0; c <= maxStam; ++c) {
                        if ((mChar.GetRegen(1) + (c * ServerConfig::shared().timerSetting[TimerSetting::STAMINAREGEN] * 1000)) <= worldMain.GetUICurrentTime() && mChar.GetStamina() <= maxStam) {
                            mChar.IncStamina(1);
                            if (mChar.GetStamina() >= maxStam) {
                                mChar.SetStamina(maxStam);
                                break;
                            }
                        }
                        else {
                            break;
                        }
                    }
                }
            }
            mChar.SetRegen(BuildTimeValue(static_cast<float>(ServerConfig::shared().timerSetting[TimerSetting::STAMINAREGEN])), 1);
        }
        
        // MANA REGENERATION:Rewrite of passive and active meditation code
        if (mChar.GetRegen(2) <= worldMain.GetUICurrentTime() || worldMain.GetOverflow()) {
            if (mChar.GetMana() < maxMana) {
                for (c = 0; c < maxMana + 1; ++c) {
                    if (mChar.GetRegen(2) + (c * ServerConfig::shared().timerSetting[TimerSetting::MANAREGEN] * 1000) <= worldMain.GetUICurrentTime() && mChar.GetMana() <= maxMana) {
                        worldSkill.CheckSkill((&mChar), MEDITATION, 0, 1000); // Check Meditation for skill gain ala OSI
                        mChar.IncMana(1);         // Gain a mana point
                        if (mChar.GetMana() == maxMana) {
                            if (mChar.IsMeditating()) {
                                if (mSock) {
                                    mSock->SysMessage(969); // You are at peace.
                                }
                                mChar.SetMeditating(false);
                            }
                            break;
                        }
                    }
                }
            }
            const float MeditationBonus = (.00075f * mChar.GetSkill(MEDITATION)); // Bonus for Meditation
            std::int32_t NextManaRegen = static_cast<std::int32_t>(ServerConfig::shared().timerSetting[TimerSetting::MANAREGEN] * (1 - MeditationBonus) * 1000);
            if (ServerConfig::shared().enabled(ServerSwitch::ARMORIMPACTSMANA)){ // If armor effects mana regeneration...
                float ArmorPenalty = worldCombat.CalcDef((&mChar), 0, false); // Penalty taken due to high def
                if (ArmorPenalty > 100){// For def higher then 100, penalty is the same...just in case
                    ArmorPenalty = 100;
                }
                ArmorPenalty = 1 + (ArmorPenalty / 25);
                NextManaRegen = static_cast<std::int32_t>(NextManaRegen * ArmorPenalty);
            }
            if (mChar.IsMeditating()) {// If player is meditation...
                mChar.SetRegen((worldMain.GetUICurrentTime() + (NextManaRegen / 2)),2);
            }
            else {
                mChar.SetRegen((worldMain.GetUICurrentTime() + NextManaRegen), 2);
            }
        }
    }
    
    if (mChar.GetVisible() == VT_INVISIBLE && (mChar.GetTimer(tCHAR_INVIS) <= worldMain.GetUICurrentTime() || worldMain.GetOverflow())) {
        mChar.ExposeToView();
    }
    
    // Take NPC out of EvadeState
    if (mChar.IsNpc() && mChar.IsEvading() &&
        mChar.GetTimer(tNPC_EVADETIME) <= worldMain.GetUICurrentTime()) {
        mChar.SetEvadeState(false);
#if defined(UOX_DEBUG_MODE) && defined(DEBUG_COMBAT)
        std::string mCharName = GetNpcDictName(&mChar, nullptr, NRS_SYSTEM);
        Console::shared().print( util::format("DEBUG: EvadeTimer ended for NPC (%s, 0x%X, at %i, %i, %i, %i).\n", mCharName.c_str(), mChar.GetSerial(), mChar.GetX(), mChar.GetY(), mChar.GetZ(), mChar.WorldNumber()));
#endif
    }
    
    if (!mChar.IsDead()) {
        // Hunger/Thirst Code
        mChar.DoHunger(mSock);
        mChar.DoThirst(mSock);
        
        // Loyalty update for pets
        mChar.DoLoyaltyUpdate();
        
        if (!mChar.IsInvulnerable() && mChar.GetPoisoned() > 0) {
            if (mChar.GetTimer(tCHAR_POISONTIME) <= worldMain.GetUICurrentTime() || worldMain.GetOverflow()) {
                if (mChar.GetTimer(tCHAR_POISONWEAROFF) > worldMain.GetUICurrentTime()) {
                    std::string mCharName = GetNpcDictName(&mChar, nullptr, NRS_SPEECH);
                    
                    switch (mChar.GetPoisoned()) {
                        case 1:{ // Lesser Poison
                            
                            mChar.SetTimer(tCHAR_POISONTIME, BuildTimeValue(2));
                            if (mChar.GetTimer(tCHAR_POISONTEXT) <= worldMain.GetUICurrentTime() || worldMain.GetOverflow()) {
                                mChar.SetTimer(tCHAR_POISONTEXT, BuildTimeValue(6));
                                mChar.TextMessage(nullptr, 1240, EMOTE, 1, mCharName.c_str()); // * %s looks a bit nauseous *
                            }
                            std::int16_t poisonDmgPercent =RandomNum(3, 6); // 3% to 6% of current health per tick
                            std::int16_t poisonDmg =static_cast<std::int16_t>((mChar.GetHP() * poisonDmgPercent) / 100);
                            [[maybe_unused]] bool retVal =mChar.Damage(std::max(static_cast<std::int16_t>(3), poisonDmg),Weather::POISON); // Minimum 3 damage per tick
                            break;
                        }
                        case 2:  { // Normal Poison
                            
                            mChar.SetTimer(tCHAR_POISONTIME, BuildTimeValue(3));
                            if (mChar.GetTimer(tCHAR_POISONTEXT) <= worldMain.GetUICurrentTime() || worldMain.GetOverflow()) {
                                mChar.SetTimer(tCHAR_POISONTEXT, BuildTimeValue(10));
                                mChar.TextMessage(nullptr, 1241, EMOTE, 1,mCharName.c_str()); // * %s looks disoriented and nauseous! *
                            }
                            std::int16_t poisonDmgPercent = RandomNum(4, 8); // 4% to 8% of current health per tick
                            std::int16_t poisonDmg = static_cast<std::int16_t>((mChar.GetHP() * poisonDmgPercent) / 100);
                            [[maybe_unused]] bool retVal =  mChar.Damage(std::max(static_cast<std::int16_t>(5), poisonDmg), Weather::POISON); // Minimum 5 damage per tick
                            break;
                        }
                        case 3: { // Greater Poison
                            
                            mChar.SetTimer(tCHAR_POISONTIME, BuildTimeValue(4));
                            if (mChar.GetTimer(tCHAR_POISONTEXT) <= worldMain.GetUICurrentTime() || worldMain.GetOverflow()) {
                                mChar.SetTimer(tCHAR_POISONTEXT, BuildTimeValue(10));
                                mChar.TextMessage( nullptr, 1242, EMOTE, 1,  mCharName.c_str()); // * %s is in severe pain! *
                            }
                            std::int16_t poisonDmgPercent =  RandomNum(8, 12); // 8% to 12% of current health per tick
                            std::int16_t poisonDmg = static_cast<std::int16_t>((mChar.GetHP() * poisonDmgPercent) / 100);
                            [[maybe_unused]] bool retVal = mChar.Damage(std::max(static_cast<std::int16_t>(8), poisonDmg), Weather::POISON); // Minimum 8 damage per tick
                            break;
                        }
                        case 4: { // Deadly Poison
                            
                            mChar.SetTimer(tCHAR_POISONTIME, BuildTimeValue(5));
                            if (mChar.GetTimer(tCHAR_POISONTEXT) <= worldMain.GetUICurrentTime() || worldMain.GetOverflow()) {
                                mChar.SetTimer(tCHAR_POISONTEXT, BuildTimeValue(10));
                                mChar.TextMessage( nullptr, 1243, EMOTE, 1, mCharName.c_str()); // * %s looks extremely weak and is
                                // wrecked in pain! *
                            }
                            std::int16_t poisonDmgPercent = RandomNum(12, 25); // 12% to 25% of current health per tick
                            std::int16_t poisonDmg = static_cast<std::int16_t>((mChar.GetHP() * poisonDmgPercent) / 100);
                            [[maybe_unused]] bool retVal = mChar.Damage(std::max(static_cast<std::int16_t>(14), poisonDmg), Weather::POISON); // Minimum 14 damage per tick
                            break;
                        }
                        case 5: {// Lethal Poison - Used by monsters only
                            
                            mChar.SetTimer(tCHAR_POISONTIME, BuildTimeValue(5));
                            if (mChar.GetTimer(tCHAR_POISONTEXT) <= worldMain.GetUICurrentTime() || worldMain.GetOverflow()) {
                                mChar.SetTimer(tCHAR_POISONTEXT, BuildTimeValue(10));
                                mChar.TextMessage( nullptr, 1243, EMOTE, 1,  mCharName.c_str()); // * %s looks extremely weak and is
                                // wrecked in pain! *
                            }
                            std::int16_t poisonDmgPercent = RandomNum(25, 50); // 25% to 50% of current health per tick
                            std::int16_t poisonDmg = static_cast<std::int16_t>((mChar.GetHP() * poisonDmgPercent) / 100);
                            [[maybe_unused]] bool retVal = mChar.Damage(std::max(static_cast<std::int16_t>(17), poisonDmg),  Weather::POISON); // Minimum 14 damage per tick
                            break;
                        }
                        default:
                            Console::shared().error(" Fallout of switch statement without default. uox3.cpp, GenericCheck(), mChar.GetPoisoned() not within valid range.");
                            mChar.SetPoisoned(0);
                            break;
                    }
                    if (mChar.GetHP() < 1 && !mChar.IsDead()) {
                        std::vector<std::uint16_t> scriptTriggers = mChar.GetScriptTriggers();
                        for (auto &i : scriptTriggers) {
                            cScript *toExecute = worldJSMapping.GetScript(i);
                            if (toExecute) {
                                std::int8_t retStatus =
                                toExecute->OnDeathBlow(&mChar, mChar.GetAttacker());
                                
                                // -1 == script doesn't exist, or returned -1
                                // 0 == script returned false, 0, or nothing - don't execute
                                // hard code 1 == script returned true or 1
                                if (retStatus == 0)
                                    return false;
                            }
                        }
                        
                        HandleDeath((&mChar), nullptr);
                        if (mSock) {
                            mSock->SysMessage(1244); // The poison has killed you!
                        }
                    }
                }
            }
        }
        
        if (mChar.GetTimer(tCHAR_POISONWEAROFF) <= worldMain.GetUICurrentTime()) {
            if (mChar.GetPoisoned() > 0) {
                mChar.SetPoisoned(0);
                if (mSock != nullptr) {
                    mSock->SysMessage(1245); // The poison has worn off.
                }
            }
        }
    }
    
    if (!mChar.GetCanAttack() && mChar.GetTimer(tCHAR_PEACETIMER) <= worldMain.GetUICurrentTime()) {
        mChar.SetCanAttack(true);
        if (mSock != nullptr) {
            mSock->SysMessage(1779); // You are no longer affected by peace!
        }
    }
    
    // Perform maintenance on NPC's list of ignored targets in combat
    if (mChar.IsNpc()) {
        mChar.CombatIgnoreMaintenance();
    }
    
    // Perform maintenance on character's aggressor flags to clear out expired entries from the list
    mChar.AggressorFlagMaintenance();
    
    // Periodically reset permagrey flags if global timer is above 0, otherwise... they're permanent
    // :P
    if ( ServerConfig::shared().timerSetting[TimerSetting::PERMAGREYFLAG]  > 0) {
        mChar.PermaGreyFlagMaintenance();
    }
    
    if (mChar.IsCriminal() && mChar.GetTimer(tCHAR_CRIMFLAG) && (mChar.GetTimer(tCHAR_CRIMFLAG) <= worldMain.GetUICurrentTime() || worldMain.GetOverflow())) {
        if (mSock != nullptr) {
            mSock->SysMessage(1238); // You are no longer a criminal.
        }
        mChar.SetTimer(tCHAR_CRIMFLAG, 0);
        UpdateFlag(&mChar);
    }
    if (mChar.HasStolen() && mChar.GetTimer(tCHAR_STEALFLAG) && (mChar.GetTimer(tCHAR_STEALFLAG) <= worldMain.GetUICurrentTime() || worldMain.GetOverflow())) {
        mChar.SetTimer(tCHAR_STEALFLAG, 0);
        mChar.HasStolen(false);
        UpdateFlag(&mChar);
    }
    if (mChar.GetKills() && (mChar.GetTimer(tCHAR_MURDERRATE) <= worldMain.GetUICurrentTime() || worldMain.GetOverflow())) {
        mChar.SetKills(static_cast<std::int16_t>(mChar.GetKills() - 1));
        
        if (mChar.GetKills()) {
            mChar.SetTimer( tCHAR_MURDERRATE, BuildTimeValue(static_cast<float>(ServerConfig::shared().timerSetting[TimerSetting::MURDERDECAY])) );
        }
        else {
            mChar.SetTimer(tCHAR_MURDERRATE, 0);
        }
        if (mSock != nullptr &&  mChar.GetKills() == ServerConfig::shared().ushortValues[UShortValue::MAXKILL] ) {
            mSock->SysMessage(1239); // You are no longer a murderer.
        }
        UpdateFlag(&mChar);
    }
    
    if (!mChar.IsDead()) {
        if (doWeather) {
            const std::uint8_t curLevel = worldMain.uoTime.worldLightLevel ;
            lightlevel_t toShow;
            if (worldRace.VisLevel(mChar.GetRace()) > curLevel) {
                toShow = 0;
            }
            else {
                toShow = static_cast<std::uint8_t>(curLevel - worldRace.VisLevel(mChar.GetRace()));
            }
            if (mChar.IsNpc()) {
                DoLight(&mChar, toShow);
            }
            else {
                DoLight(mSock, toShow);
            }
        }
        worldWeather.doLightEffect(mSock, mChar);
        worldWeather.doWeatherEffect(mSock, mChar, Weather::RAIN);
        worldWeather.doWeatherEffect(mSock, mChar, Weather::SNOW);
        worldWeather.doWeatherEffect(mSock, mChar, Weather::HEAT);
        worldWeather.doWeatherEffect(mSock, mChar, Weather::COLD);
        worldWeather.doWeatherEffect(mSock, mChar, Weather::STORM);
        
        if (checkFieldEffects) {
            worldMagic.CheckFieldEffects(mChar);
        }
        
        mChar.UpdateDamageTrack();
    }
    
    if (mChar.IsDead()) {
        return true;
    }
    else if (mChar.GetHP() <= 0) {
        std::vector<std::uint16_t> scriptTriggers = mChar.GetScriptTriggers();
        for (auto i : scriptTriggers) {
            auto toExecute = worldJSMapping.GetScript(i);
            if (toExecute) {
                auto retStatus = toExecute->OnDeathBlow(&mChar, mChar.GetAttacker());
                
                // -1 == script doesn't exist, or returned -1
                // 0 == script returned false, 0, or nothing - don't execute hard code
                // 1 == script returned true or 1
                if (retStatus == 0) {
                    return false;
                }
            }
        }
        
        HandleDeath((&mChar), nullptr);
        return true;
    }
    return false;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CheckPC()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check a PC's status
// o------------------------------------------------------------------------------------------------o
auto CheckPC(CSocket *mSock, CChar &mChar) -> void {
    worldCombat.CombatLoop(mSock, mChar);
    
    if (mChar.GetSquelched() == 2) {
        if (mSock->GetTimer(tPC_MUTETIME) != 0 &&
            (mSock->GetTimer(tPC_MUTETIME) <= worldMain.GetUICurrentTime() || worldMain.GetOverflow())) {
            mChar.SetSquelched(0);
            mSock->SetTimer(tPC_MUTETIME, 0);
            mSock->SysMessage(1237); // You are no longer squelched!
        }
    }
    
    if (mChar.IsCasting() && !mChar.IsJSCasting() && mChar.GetSpellCast() != -1) {
        // Casting a spell
        auto spellNum = mChar.GetSpellCast();
        mChar.SetNextAct(mChar.GetNextAct() - 1);
        if (mChar.GetTimer(tCHAR_SPELLTIME) <= worldMain.GetUICurrentTime() || worldMain.GetOverflow()) // Spell is complete target it.
        {
            // Set the recovery time before another spell can be cast
            mChar.SetTimer( tCHAR_SPELLRECOVERYTIME, BuildTimeValue(static_cast<float>(worldMagic.spells[spellNum].RecoveryDelay())));
            
            if (worldMagic.spells[spellNum].RequireTarget()) {
                mChar.SetCasting(false);
                mChar.SetFrozen(false);
                mChar.Dirty(UT_UPDATE);
                std::uint8_t cursorType = 0;
                if (worldMagic.spells[spellNum].AggressiveSpell()) {
                    cursorType = 1;
                }
                else if ((spellNum == 4) || (spellNum == 6) || (spellNum == 7) || (spellNum >= 9 && spellNum <= 11) || (spellNum >= 15 && spellNum <= 17) || (spellNum == 25) || (spellNum == 26) || (spellNum == 29) || (spellNum == 44) || (spellNum == 59)) {
                    cursorType = 2;
                }
                mSock->SendTargetCursor(0, TARGET_CASTSPELL, worldMagic.spells[spellNum].StringToSay().c_str(), cursorType);
            }
            else {
                mChar.SetCasting(false);
                worldMagic.CastSpell(mSock, &mChar);
                mChar.SetTimer(tCHAR_SPELLTIME, 0);
                mChar.SetFrozen(false);
            }
        }
        else if (mChar.GetNextAct() <= 0) {
            // redo the spell action
            mChar.SetNextAct(75);
            if (!mChar.IsOnHorse() && !mChar.IsFlying()) {
                // Consider Gargoyle flying as mounted here
                worldEffect.PlaySpellCastingAnimation( &mChar, worldMagic.spells[mChar.GetSpellCast()].Action(), false, false);
            }
        }
    }
    
    if (ServerConfig::shared().shortValues[ShortValue::AMBIENTSOUND] >= 1) {
        if (ServerConfig::shared().shortValues[ShortValue::AMBIENTSOUND] > 10) {
            ServerConfig::shared().shortValues[ShortValue::AMBIENTSOUND] = 10 ; // This should be check in config post check?
        }
        const std::int16_t soundTimer = static_cast<std::int16_t>(ServerConfig::shared().shortValues[ShortValue::AMBIENTSOUND] * 100);
        if (!mChar.IsDead() && (RandomNum(0, soundTimer - 1)) == (soundTimer / 2)) {
            worldEffect.PlayBGSound((*mSock), mChar);
        }
    }
    
    if (mSock->GetTimer(tPC_SPIRITSPEAK) > 0 && mSock->GetTimer(tPC_SPIRITSPEAK) < worldMain.GetUICurrentTime()) {
        mSock->SetTimer(tPC_SPIRITSPEAK, 0);
    }
    
    if (mSock->GetTimer(tPC_TRACKING) > worldMain.GetUICurrentTime()) {
        if (mSock->GetTimer(tPC_TRACKINGDISPLAY) <= worldMain.GetUICurrentTime()) {
            mSock->SetTimer(tPC_TRACKINGDISPLAY,BuildTimeValue(static_cast<float>( ServerConfig::shared().ushortValues[UShortValue::MSGREDISPLAYTIME] )));
            worldSkill.Track(&mChar);
        }
    }
    else {
        if (mSock->GetTimer(tPC_TRACKING) > (worldMain.GetUICurrentTime() / 10)) {
            // dont send arrow-away packet all the time
            mSock->SetTimer(tPC_TRACKING, 0);
            if (ValidateObject(mChar.GetTrackingTarget())) {
                CPTrackingArrow tSend = (*mChar.GetTrackingTarget());
                tSend.Active(0);
                if (mSock->clientType() >= ClientType::HS2D) {
                    tSend.AddSerial(mChar.GetTrackingTarget()->GetSerial());
                }
                mSock->Send(&tSend);
            }
        }
    }
    
    if (mChar.IsOnHorse()) {
        CItem *horseItem = mChar.GetItemAtLayer(IL_MOUNT);
        if (!ValidateObject(horseItem)) {
            mChar.SetOnHorse(false); // turn it off, we aren't on one because there's no item!
        }
        else if (horseItem->GetDecayTime() != 0 && (horseItem->GetDecayTime() <= worldMain.GetUICurrentTime() || worldMain.GetOverflow())) {
            mChar.SetOnHorse(false);
            horseItem->Delete();
        }
    }
    
    if (mChar.GetTimer(tCHAR_FLYINGTOGGLE) > 0 && mChar.GetTimer(tCHAR_FLYINGTOGGLE) < worldMain.GetUICurrentTime()) {
        mChar.SetTimer(tCHAR_FLYINGTOGGLE, 0);
        mChar.SetFrozen(false);
        mChar.Teleport();
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CheckNPC()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check NPC's status
// o------------------------------------------------------------------------------------------------o
auto CheckNPC(CChar &mChar, bool checkAI, bool doRestock, bool doPetOfflineCheck) -> void {
    bool doAICheck = true;
    std::vector<std::uint16_t> scriptTriggers = mChar.GetScriptTriggers();
    for (auto scriptTrig : scriptTriggers) {
        cScript *toExecute = worldJSMapping.GetScript(scriptTrig);
        if (toExecute) {
            if (toExecute->OnAISliver(&mChar) == 1) {
                // Script returned true or 1, don't do hard-coded AI check
                doAICheck = false;
            }
        }
    }
    
    if (doAICheck && checkAI) {
        checkArtificialIntelligence(mChar);
    }
    worldMovement.NpcMovement(mChar);
    
    if (doRestock) {
        restockNPC(mChar, false);
    }
    
    if (doPetOfflineCheck) {
        mChar.CheckPetOfflineTimeout();
    }
    
    if (mChar.GetTimer(tNPC_SUMMONTIME)) {
        if ((mChar.GetTimer(tNPC_SUMMONTIME) <= worldMain.GetUICurrentTime()) || worldMain.GetOverflow()) {
            // Added Dec 20, 1999
            // QUEST expire check - after an Escort quest is created a timer is set
            // so that the NPC will be deleted and removed from the game if it hangs around
            // too long without every having its quest accepted by a player so we have to
            // remove its posting from the messageboard before icing the NPC Only need to
            // remove the post if the NPC does not have a follow target set
            if (mChar.GetQuestType() == QT_ESCORTQUEST && !ValidateObject(mChar.GetFTarg())) {
                MsgBoardQuestEscortRemovePost(&mChar);
                mChar.Delete();
                return;
            }
            
            if (mChar.GetNpcAiType() == AI_GUARD && mChar.IsAtWar()) {
                mChar.SetTimer(tNPC_SUMMONTIME, BuildTimeValue(25));
                return;
            }
            worldEffect.PlaySound(&mChar, 0x01FE);
            mChar.SetDead(true);
            mChar.Delete();
            return;
        }
    }
    
    if (mChar.GetFleeAt() == 0) {
        mChar.SetFleeAt( ServerConfig::shared().shortValues[ShortValue::NPCBASEFLEEAT] );
    }
    if (mChar.GetReattackAt() == 0) {
        mChar.SetReattackAt(ServerConfig::shared().shortValues[ShortValue::NPCBASEREATTACKAT] );
    }
    
    auto mNpcWander = mChar.GetNpcWander();
    if (mNpcWander == WT_SCARED) {
        if (mChar.GetTimer(tNPC_FLEECOOLDOWN) <= worldMain.GetUICurrentTime() ||  worldMain.GetOverflow()) {
            if (mChar.GetTimer(tNPC_MOVETIME) <= worldMain.GetUICurrentTime() || worldMain.GetOverflow()) {
                mChar.SetFleeDistance(static_cast<std::uint8_t>(0));
                CChar *mTarget = mChar.GetTarg();
                if (ValidateObject(mTarget) && !mTarget->IsDead() && ObjInRange(&mChar, mTarget, DIST_INRANGE)) {
                    if (mChar.GetMounted()) {
                        mChar.SetTimer(tNPC_MOVETIME, BuildTimeValue(mChar.GetMountedFleeingSpeed()));
                    }
                    else {
                        mChar.SetTimer(tNPC_MOVETIME, BuildTimeValue(mChar.GetFleeingSpeed()));
                    }
                }
                else {
                    // target no longer exists, or is out of range, stop running
                    mChar.SetTarg(nullptr);
                    if (mChar.GetOldNpcWander() != WT_NONE) {
                        mChar.SetNpcWander(mChar.GetOldNpcWander());
                    }
                    if (mChar.GetMounted()) {
                        mChar.SetTimer(tNPC_MOVETIME, BuildTimeValue(mChar.GetMountedWalkingSpeed()));
                    }
                    else {
                        mChar.SetTimer(tNPC_MOVETIME, BuildTimeValue(mChar.GetWalkingSpeed()));
                    }
                    mChar.SetOldNpcWander(WT_NONE); // so it won't save this at the wsc file
                }
            }
        }
    }
    else if (mNpcWander != WT_FLEE && mNpcWander != WT_FROZEN && (mChar.GetHP() < mChar.GetMaxHP() * mChar.GetFleeAt() / 100)) {
        if (mChar.GetTimer(tNPC_FLEECOOLDOWN) <= worldMain.GetUICurrentTime() || worldMain.GetOverflow()) {
            // Make NPC try to flee away from their opponent, if still within range
            CChar *mTarget = mChar.GetTarg();
            if (ValidateObject(mTarget) && !mTarget->IsDead() && ObjInRange(&mChar, mTarget, DIST_SAMESCREEN)) {
                mChar.SetFleeDistance(static_cast<std::uint8_t>(0));
                mChar.SetOldNpcWander(mNpcWander);
                mChar.SetNpcWander(WT_FLEE);
                if (mChar.GetMounted()) {
                    mChar.SetTimer(tNPC_MOVETIME, BuildTimeValue(mChar.GetMountedFleeingSpeed()));
                }
                else {
                    mChar.SetTimer(tNPC_MOVETIME, BuildTimeValue(mChar.GetFleeingSpeed()));
                }
            }
        }
    }
    else if ((mNpcWander == WT_FLEE) && (mChar.GetHP() > mChar.GetMaxHP() * mChar.GetReattackAt() / 100)) {
        // Bring NPC out of flee-mode and back to their original wandermode
        mChar.SetTimer( tNPC_FLEECOOLDOWN, BuildTimeValue(5)); // Wait at least 5 seconds before reentring flee-mode!
        mChar.SetNpcWander(mChar.GetOldNpcWander());
        if (mChar.GetMounted()) {
            mChar.SetTimer(tNPC_MOVETIME, BuildTimeValue(mChar.GetMountedWalkingSpeed()));
        }
        else {
            mChar.SetTimer(tNPC_MOVETIME, BuildTimeValue(mChar.GetWalkingSpeed()));
        }
        mChar.SetOldNpcWander(WT_NONE); // so it won't save this at the wsc file
    }
    worldCombat.CombatLoop(nullptr, mChar);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CheckItem()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check item decay, spawn timers and boat movement in a given region
// o------------------------------------------------------------------------------------------------o
auto CheckItem(CMapRegion *toCheck, bool checkItems, std::uint32_t nextDecayItems, std::uint32_t nextDecayItemsInHouses, bool doWeather) {
    auto regItems = toCheck->GetItemList();
    auto collection = regItems->collection();
    for (const auto &itemCheck : collection) {
        if (!ValidateObject(itemCheck) || itemCheck->IsFree())
            continue;
        
        if (checkItems) {
            if (itemCheck->IsDecayable() && (itemCheck->GetCont() == nullptr)) {
                if (itemCheck->GetType() == IT_HOUSESIGN &&
                    itemCheck->GetTempVar(CITV_MORE) > 0) {
                    // Don't decay signs that belong to houses
                    itemCheck->SetDecayable(false);
                }
                if ((itemCheck->GetDecayTime() <= worldMain.GetUICurrentTime()) || worldMain.GetOverflow()) {
                    auto scriptTriggers = itemCheck->GetScriptTriggers();
                    for (auto scriptTrig : scriptTriggers) {
                        cScript *toExecute = worldJSMapping.GetScript(scriptTrig);
                        if (toExecute) {
                            if (toExecute->OnDecay(itemCheck) == 0) {
                                // if it exists and we don't want hard code, return
                                return;
                            }
                        }
                    }
                    
                    // Check global script! Maybe there's another event there
                    auto toExecute = worldJSMapping.GetScript(static_cast<std::uint16_t>(0));
                    if (toExecute) {
                        if (toExecute->OnDecay(itemCheck) == 0) {
                            // if it exists and we don't want hard code, return
                            return;
                        }
                    }
                    
                    if (decayItem((*itemCheck), nextDecayItems, nextDecayItemsInHouses))
                        continue;
                }
            }
            switch (itemCheck->GetType()) {
                case IT_ITEMSPAWNER:
                case IT_NPCSPAWNER:
                case IT_SPAWNCONT:
                case IT_LOCKEDSPAWNCONT:
                case IT_UNLOCKABLESPAWNCONT:
                case IT_AREASPAWNER:
                case IT_ESCORTNPCSPAWNER:
                case IT_PLANK: {
                    if ((itemCheck->GetTempTimer() <= worldMain.GetUICurrentTime()) || worldMain.GetOverflow()) {
                        if (itemCheck->GetObjType() == CBaseObject::OT_SPAWNER) {
                            CSpawnItem *spawnItem = static_cast<CSpawnItem *>(itemCheck);
                            if (spawnItem->DoRespawn()) {
                                continue;
                            }
                            spawnItem->SetTempTimer(BuildTimeValue(static_cast<float>(RandomNum(spawnItem->GetInterval(0) * 60,spawnItem->GetInterval(1) * 60))));
                        }
                        else if (itemCheck->GetObjType() == CBaseObject::OT_ITEM &&
                                 itemCheck->GetType() == IT_PLANK) {
                            // Automatically close the plank if it's still open, and still
                            // locked
                            auto plankStatus = itemCheck->GetTag("plankLocked");
                            if (plankStatus.m_IntValue == 1) {
                                switch (itemCheck->GetId()) {
                                    case 0x3E84:
                                        itemCheck->SetId(0x3EE9);
                                        itemCheck->SetTempTimer(0);
                                        break;
                                    case 0x3ED5:
                                        itemCheck->SetId(0x3EB1);
                                        itemCheck->SetTempTimer(0);
                                        break;
                                    case 0x3ED4:
                                        itemCheck->SetId(0x3EB2);
                                        itemCheck->SetTempTimer(0);
                                        break;
                                    case 0x3E89:
                                        itemCheck->SetId(0x3E8A);
                                        itemCheck->SetTempTimer(0);
                                        break;
                                }
                            }
                        }
                        else {
                            itemCheck->SetType(IT_NOTYPE);
                            Console::shared().warning("Invalid spawner object detected; item type reverted to 0. All spawner objects have to be added using 'ADD SPAWNER # command.");
                        }
                    }
                    break;
                }
                case IT_SOUNDOBJECT:
                    if (itemCheck->GetTempVar(CITV_MOREY) < 25) {
                        if (RandomNum(1, 100) <=
                            static_cast<std::int32_t>(itemCheck->GetTempVar(CITV_MOREZ))) {
                            for (auto &tSock : FindNearbyPlayers(itemCheck,static_cast<std::uint16_t>(itemCheck->GetTempVar(CITV_MOREY)))) {
                                worldEffect.PlaySound(tSock, static_cast<std::uint16_t>(itemCheck->GetTempVar(CITV_MOREX)),false);
                            }
                        }
                    }
                    break;
                default:
                    break;
            }
        }
        if (itemCheck->CanBeObjType(CBaseObject::OT_BOAT)) {
            CBoatObj *mBoat = static_cast<CBoatObj *>(itemCheck);
            std::int8_t boatMoveType = mBoat->GetMoveType();
            if (ValidateObject(mBoat) && boatMoveType &&
                ((mBoat->GetMoveTime() <= worldMain.GetUICurrentTime()) ||
                 worldMain.GetOverflow())) {
                if (boatMoveType != BOAT_ANCHORED) {
                    switch (boatMoveType) {
                            // case BOAT_ANCHORED:
                            // case BOAT_STOP:
                        case BOAT_FORWARD:
                        case BOAT_SLOWFORWARD:
                        case BOAT_ONEFORWARD:
                            moveBoat(itemCheck->GetDir(), mBoat);
                            break;
                        case BOAT_BACKWARD:
                        case BOAT_SLOWBACKWARD:
                        case BOAT_ONEBACKWARD: {
                            std::uint8_t dir = static_cast<std::uint8_t>(itemCheck->GetDir() + 4);
                            if (dir > 7) {
                                dir %= 8;
                            }
                            moveBoat(dir, mBoat);
                            break;
                        }
                        case BOAT_LEFT:
                        case BOAT_SLOWLEFT:
                        case BOAT_ONELEFT: {
                            std::uint8_t dir = static_cast<std::uint8_t>(itemCheck->GetDir() - 2);
                            
                            dir %= 8;
                            moveBoat(dir, mBoat);
                            break;
                        }
                        case BOAT_RIGHT:
                        case BOAT_SLOWRIGHT:
                        case BOAT_ONERIGHT: {
                            // Right / One Right
                            std::uint8_t dir = static_cast<std::uint8_t>(itemCheck->GetDir() + 2);
                            
                            dir %= 8;
                            moveBoat(dir, mBoat);
                            break;
                        }
                        case BOAT_FORWARDLEFT:
                        case BOAT_SLOWFORWARDLEFT:
                        case BOAT_ONEFORWARDLEFT: {
                            std::uint8_t dir = static_cast<std::uint8_t>(itemCheck->GetDir() - 1);
                            
                            dir %= 8;
                            moveBoat(dir, mBoat);
                            break;
                        }
                        case BOAT_FORWARDRIGHT:
                        case BOAT_SLOWFORWARDRIGHT:
                        case BOAT_ONEFORWARDRIGHT: {
                            std::uint8_t dir = static_cast<std::uint8_t>(itemCheck->GetDir() + 1);
                            
                            dir %= 8;
                            moveBoat(dir, mBoat);
                            break;
                        }
                        case BOAT_BACKWARDLEFT:
                        case BOAT_SLOWBACKWARDLEFT:
                        case BOAT_ONEBACKWARDLEFT: {
                            std::uint8_t dir = static_cast<std::uint8_t>(itemCheck->GetDir() + 5);
                            if (dir > 7) {
                                dir %= 8;
                            }
                            moveBoat(dir, mBoat);
                            break;
                        }
                        case BOAT_BACKWARDRIGHT:
                        case BOAT_SLOWBACKWARDRIGHT:
                        case BOAT_ONEBACKWARDRIGHT: {
                            std::uint8_t dir = static_cast<std::uint8_t>(itemCheck->GetDir() + 3);
                            if (dir > 7) {
                                dir %= 8;
                            }
                            moveBoat(dir, mBoat);
                            break;
                        }
                        default:
                            break;
                    }
                    
                    // One-step boat commands, so reset move type to 0 after the initial
                    // move
                    if (boatMoveType == BOAT_LEFT || boatMoveType == BOAT_RIGHT) {
                        // Move 50% slower left/right than forward/back
                        mBoat->SetMoveTime(BuildTimeValue(static_cast<float>(ServerConfig::shared().realNumbers[RealNumberConfig::CHECKBOATS]) * 1.5));
                    }
                    else if (boatMoveType >= BOAT_ONELEFT &&  boatMoveType <= BOAT_ONEBACKWARDRIGHT) {
                        mBoat->SetMoveType(0);
                        
                        // Set timer to restrict movement to normal boat speed if player
                        // spams command
                        mBoat->SetMoveTime(BuildTimeValue(static_cast<float>(ServerConfig::shared().realNumbers[RealNumberConfig::CHECKBOATS]) * 1.5));
                    }
                    else if (boatMoveType >= BOAT_SLOWLEFT && boatMoveType <= BOAT_SLOWBACKWARDLEFT) {
                        // Set timer to slowly move the boat forward
                        mBoat->SetMoveTime(BuildTimeValue(static_cast<float>(ServerConfig::shared().realNumbers[RealNumberConfig::CHECKBOATS]) *2.0));
                    }
                    else {
                        // Set timer to move the boat forward at normal speed
                        mBoat->SetMoveTime(BuildTimeValue(static_cast<float>(ServerConfig::shared().realNumbers[RealNumberConfig::CHECKBOATS])));
                    }
                }
            }
        }
        
        // Do JS Weather for item
        if (doWeather) {
            DoLight(itemCheck, worldMain.uoTime.worldLightLevel);
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::CheckAutoTimers()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check automatic and timer controlled functions
// o------------------------------------------------------------------------------------------------o
auto CWorldMain::CheckAutoTimers() -> void {
    static std::uint32_t nextCheckSpawnRegions = 0;
    static std::uint32_t nextCheckTownRegions = 0;
    static std::uint32_t nextCheckItems = 0;
    static std::uint32_t nextDecayItems = 0;
    static std::uint32_t nextDecayItemsInHouses = 0;
    static std::uint32_t nextSetNPCFlagTime = 0;
    static std::uint32_t accountFlush = 0;
    bool doWeather = false;
    bool doPetOfflineCheck = false;
    
    // modify this stuff to take into account more variables
    if ((accountFlush <= GetUICurrentTime()) || GetOverflow()) {
        bool reallyOn = false;
        // time to flush our account status!
        for (auto &[num, entry] : Account::shared().allAccounts()) {
            if (entry.flag.test(AccountEntry::attributeflag_t::ONLINE)) {
                reallyOn = false; // to start with, there's no one really on
                {
                    for (auto &tSock : worldNetwork.connClients) {
                        CChar *tChar = tSock->CurrcharObj();
                        if (!ValidateObject(tChar)) {
                            continue;
                        }
                        if (tChar->GetAccount().accountNumber == num) {
                            reallyOn = true;
                        }
                    }
                }
                if (!reallyOn) {
                    // no one's really on, let's set that
                    entry.flag.reset(AccountEntry::attributeflag_t::ONLINE);
                }
            }
        }
        accountFlush = BuildTimeValue(static_cast<float>(ServerConfig::shared().realNumbers[RealNumberConfig::FLUSHTIME]));
    }
    // worldNetwork.On();   //<<<<<< WHAT the HECK, this is why you dont bury mutex locking
    //  PushConn and PopConn lock and unlock as well (yes, bad)
    //  But now we are doing recursive lock here
    
    if (GetWorldSaveProgress() == SS_NOTSAVING) {
        for (auto &tSock : worldNetwork.connClients) {
            if ((tSock->IdleTimeout() != -1) &&
                static_cast<std::uint32_t>(tSock->IdleTimeout()) <= GetUICurrentTime()) {
                CChar *tChar = tSock->CurrcharObj();
                if (!ValidateObject(tChar)) {
                    continue;
                }
                if (!tChar->IsGM()) {
                    tSock->IdleTimeout(-1);
                    tSock->SysMessage(1246); // You're being disconnected because you
                    // were idle too long.
                    worldNetwork.Disconnect(tSock);
                }
            }
            else if (((static_cast<std::uint32_t>(tSock->IdleTimeout() + 300 * 1000) <= GetUICurrentTime() && static_cast<std::uint32_t>(tSock->IdleTimeout() + 200 * 1000) >= GetUICurrentTime()) || GetOverflow()) &&  !tSock->WasIdleWarned()) {
                // is their idle time between 3 and 5 minutes, and they haven't been
                // warned already?
                CPIdleWarning warn(0x07);
                tSock->Send(&warn);
                tSock->WasIdleWarned(true);
            }
            
            if (ServerConfig::shared().enabled(ServerSwitch::KICKONASSISTANTSILENCE)) {
                if (!tSock->NegotiatedWithAssistant() && tSock->NegotiateTimeout() != -1 && static_cast<std::uint32_t>(tSock->NegotiateTimeout()) <= GetUICurrentTime()) {
                    const CChar *tChar = tSock->CurrcharObj();
                    if (!ValidateObject(tChar)) {
                        continue;
                    }
                    if (!tChar->IsGM()) {
                        tSock->IdleTimeout(-1);
                        tSock->SysMessage(9047); // Failed to negotiate features with assistant tool.
                        // Disconnecting client...
                        worldNetwork.Disconnect(tSock);
                    }
                }
            }
            
            // Check player's network traffic usage versus caps set in ini
            if (tSock->LoginComplete() && (tSock->AcctNo() != 0) && tSock->GetTimer(tPC_TRAFFICWARDEN) <= GetUICurrentTime()) {
                if (!ValidateObject(tSock->CurrcharObj()) || tSock->CurrcharObj()->IsGM()) {
                    continue;
                }
                
                bool tempTimeBan = false;
                if (tSock->BytesReceived() > ServerConfig::shared().uintValues[UIntValue::MAXCLIENTBYTESIN] ) {
                    // Player has exceeded the cap! Send one warning - next time kick
                    // player
                    tSock->SysMessage(worldDictionary.GetEntry(9082, tSock->Language())); // Excessive data usage detected!
                    // Sending too many requests to the
                    // server in a short amount of time
                    // will get you banned.
                    tSock->BytesReceivedWarning(tSock->BytesReceivedWarning() + 1);
                    if (tSock->BytesReceivedWarning() > 2) {
                        // If it happens 3 times in the same session, give player a
                        // temporary ban
                        tempTimeBan = true;
                    }
                }
                
                if (tSock->BytesSent() > ServerConfig::shared().uintValues[UIntValue::MAXCLIENTBYTESOUT]) {
                    // This is data sent from server, so should be more lenient before a
                    // kick (though could still be initiated by player somehow)
                    tSock->SysMessage(worldDictionary.GetEntry( 9082, tSock->Language())); // Excessive data usage detected!
                    // Sending too many requests to the
                    // server in a short amount of time
                    // will get you banned.
                    tSock->BytesSentWarning(tSock->BytesSentWarning() + 1);
                    if (tSock->BytesSentWarning() > 2) {
                        // If it happens 3 times or more in the same session, give
                        // player a temporary ban
                        tempTimeBan = true;
                    }
                }
                
                if (tempTimeBan) {
                    // Give player a 30 minute temp ban
                    AccountEntry &myAccount =
                    Account::shared()[tSock->GetAccount().accountNumber];
                    myAccount.flag.set(AccountEntry::attributeflag_t::BANNED, true);
                    myAccount.timeBan =
                    GetMinutesSinceEpoch() + ServerConfig::shared().uintValues[UIntValue::NETTRAFFICTIMEBAN] ;
                    worldNetwork.Disconnect(tSock);
                    continue;
                }
                
                // Reset amount of bytes received and sent, and restart timer
                tSock->BytesReceived(0);
                tSock->BytesSent(0);
                tSock->SetTimer(tPC_TRAFFICWARDEN, BuildTimeValue(static_cast<float>(10)));
            }
        }
    }
    else if (GetWorldSaveProgress() == SS_JUSTSAVED) {
        // if we've JUST saved, do NOT kick anyone off (due to a possibly really long save),
        // but reset any offending players to 60 seconds to go before being kicked off
        for (auto &wsSocket : worldNetwork.connClients) {
            if (wsSocket) {
                if (static_cast<std::uint32_t>(wsSocket->IdleTimeout()) < GetUICurrentTime()) {
                    wsSocket->IdleTimeout(BuildTimeValue(60.0F));
                    wsSocket->WasIdleWarned(
                                            true); // don't give them the message if they only have 60s
                }
                if (ServerConfig::shared().enabled(ServerSwitch::KICKONASSISTANTSILENCE)) {
                    if (!wsSocket->NegotiatedWithAssistant() &&
                        static_cast<std::uint32_t>(wsSocket->NegotiateTimeout()) <
                        GetUICurrentTime()) {
                        wsSocket->NegotiateTimeout(BuildTimeValue(60.0F));
                    }
                }
            }
        }
        SetWorldSaveProgress(SS_NOTSAVING);
    }
    if ((nextCheckTownRegions <= GetUICurrentTime()) || GetOverflow()) {
        std::for_each(worldMain.townRegions.begin(), worldMain.townRegions.end(), [](std::pair<const std::uint16_t, CTownRegion *> &town) {
            if (town.second != nullptr) {
                town.second->PeriodicCheck();
            }
        });
        nextCheckTownRegions = BuildTimeValue(10); // do checks every 10 seconds or so, rather than every single time
        worldJailSystem.PeriodicCheck();
    }
    
    if ((nextCheckSpawnRegions <= GetUICurrentTime()) &&  (ServerConfig::shared().realNumbers[RealNumberConfig::CHECKSPAWN] != -1)) {
        // Regionspawns
        std::uint32_t itemsSpawned = 0;
        std::uint32_t npcsSpawned = 0;
        std::uint32_t totalItemsSpawned = 0;
        std::uint32_t totalNpcsSpawned = 0;
        std::uint32_t maxItemsSpawned = 0;
        std::uint32_t maxNpcsSpawned = 0;
        
        for (auto &[regnum, spawnReg] : worldMain.spawnRegions) {
            if (spawnReg) {
                if (spawnReg->GetNextTime() <= GetUICurrentTime()) {
                    spawnReg->DoRegionSpawn(itemsSpawned, npcsSpawned);
                }
                // Grab some info from the spawn region anyway, even if it's not time to
                // spawn
                totalItemsSpawned += static_cast<std::uint32_t>(spawnReg->GetCurrentItemAmt());
                totalNpcsSpawned += static_cast<std::uint32_t>(spawnReg->GetCurrentCharAmt());
                maxItemsSpawned += static_cast<std::uint32_t>(spawnReg->GetMaxItemSpawn());
                maxNpcsSpawned += static_cast<std::uint32_t>(spawnReg->GetMaxCharSpawn());
            }
        }
        
        // Adaptive spawn region check timer. The closer spawn regions as a whole are to
        // being at their defined max capacity, the less frequently UOX3 will check spawn
        // regions again. Similarly, the more room there is to spawn additional stuff, the
        // more frequently UOX3 will check spawn regions
        auto checkSpawnRegionSpeed = static_cast<float>( ServerConfig::shared().realNumbers[RealNumberConfig::CHECKSPAWN] );
        std::uint16_t itemSpawnCompletionRatio = (maxItemsSpawned > 0 ? ((totalItemsSpawned * 100.0) / maxItemsSpawned) : 100);
        std::uint16_t npcSpawnCompletionRatio = (maxNpcsSpawned > 0 ? ((totalNpcsSpawned * 100.0) / maxNpcsSpawned) : 100);
        
        std::int32_t avgCompletionRatio = (itemSpawnCompletionRatio + npcSpawnCompletionRatio) / 2;
        if (avgCompletionRatio == 100) {
            checkSpawnRegionSpeed *= 3;
        }
        else if (avgCompletionRatio >= 90) {
            checkSpawnRegionSpeed *= 2;
        }
        else if (avgCompletionRatio >= 75) {
            checkSpawnRegionSpeed *= 1.5;
        }
        else if (avgCompletionRatio >= 50) {
            checkSpawnRegionSpeed *= 1.25;
        }
        
        nextCheckSpawnRegions = BuildTimeValue( checkSpawnRegionSpeed); // Don't check them TOO often (Keep down the lag)
    }
    
    worldHTMLTemplate.poll(ETT_ALLTEMPLATES);
    
    const std::uint32_t saveinterval = ServerConfig::shared().uintValues[UIntValue::SAVESTIMER] ;
    if (saveinterval != 0) {
        time_t oldTime = GetOldTime();
        if (!GetAutoSaved()) {
            SetAutoSaved(true);
            time(&oldTime);
            SetOldTime(static_cast<std::uint32_t>(oldTime));
        }
        time_t newTime = GetNewTime();
        time(&newTime);
        SetNewTime(static_cast<std::uint32_t>(newTime));
        
        if (difftime(GetNewTime(), GetOldTime()) >= saveinterval) {
            // Added Dec 20, 1999
            // After an automatic world save occurs, lets check to see if
            // anyone is online (clients connected).  If nobody is connected
            // Lets do some maintenance on the bulletin boards.
            if (!GetPlayersOnline() && (GetWorldSaveProgress() != SS_SAVING)) {
                Console::shared() << "No players currently online. Starting bulletin board maintenance" << myendl;
                Console::shared().log("Bulletin Board Maintenance routine running (AUTO)", "server.log");
                MsgBoardMaintenance();
            }
            
            SetAutoSaved(false);
            
#if defined(_WIN32)
            SetConsoleCtrlHandler(exit_handler, TRUE);
#endif
            isWorldSaving = true;
            SaveNewWorld(false);
            isWorldSaving = false;
#if defined(_WIN32)
            SetConsoleCtrlHandler(exit_handler, false);
#endif
        }
    }
    
    /*time_t oldIPTime = GetOldIPTime();
     if( !GetIPUpdated() )
     {
     SetIPUpdated( true );
     time(&oldIPTime);
     SetOldIPTime( static_cast<std::uint32_t>(oldIPTime) );
     }
     time_t newIPTime = GetNewIPTime();
     time(&newIPTime);
     SetNewIPTime( static_cast<std::uint32_t>(newIPTime) );
     
     if( difftime( GetNewIPTime(), GetOldIPTime() ) >= 120 )
     {
     ServerData()->RefreshIPs();
     SetIPUpdated( false );
     }*/
    
    // Time functions
    if ((GetUOTickCount() <= GetUICurrentTime()) || (GetOverflow())) {
        std::uint8_t oldHour = worldMain.uoTime.hours;
        if (worldMain.uoTime.incrementMinute()) {
            worldWeather.newDay();
        }
        if (oldHour != worldMain.uoTime.hours ) {
            worldWeather.newHour();
        }
        SetUOTickCount(BuildTimeValue(ServerConfig::shared().ushortValues[UShortValue::SECONDSPERUOMINUTE]));
    }
    
    if ((GetTimer(tWORLD_LIGHTTIME) <= GetUICurrentTime()) || GetOverflow()) {
        DoWorldLight();     // Changes lighting, if it is currently time to.
        worldWeather.update(worldMain.uoTime); // updates the weather types
        SetTimer(tWORLD_LIGHTTIME, BuildTimeValue(static_cast<float>( ServerConfig::shared().timerSetting[TimerSetting::WEATHER])));
        doWeather = true;
    }
    
    if ((GetTimer(tWORLD_PETOFFLINECHECK) <= GetUICurrentTime()) || GetOverflow()) {
        SetTimer(tWORLD_PETOFFLINECHECK, BuildTimeValue(static_cast<float>( ServerConfig::shared().timerSetting[TimerSetting::PETOFFLINECHECK])));
        doPetOfflineCheck = true;
    }
    
    bool checkFieldEffects = false;
    if ((GetTimer(tWORLD_NEXTFIELDEFFECT) <= GetUICurrentTime()) || GetOverflow()) {
        checkFieldEffects = true;
        SetTimer(tWORLD_NEXTFIELDEFFECT, BuildTimeValue(0.5f));
    }
    std::set<CMapRegion *> regionList;
    {
        for (auto &iSock : worldNetwork.connClients) {
            if (iSock) {
                CChar *mChar = iSock->CurrcharObj();
                if (!ValidateObject(mChar)) {
                    continue;
                }
                std::uint8_t worldNumber = mChar->WorldNumber();
                if (mChar->GetAccount().accountNumber == iSock->AcctNo() && mChar->GetAccount().inGame == mChar->GetSerial()) {
                    GenericCheck(iSock, (*mChar), checkFieldEffects, doWeather);
                    CheckPC(iSock, (*mChar));
                    
                    std::int16_t xOffset = worldMapHandler.GetGridX(mChar->GetX());
                    std::int16_t yOffset = worldMapHandler.GetGridY(mChar->GetY());
                    
                    // Restrict the amount of active regions based on how far player is from
                    // the border to the next one. This reduces active regions around a
                    // player from always 9 to varying between 3 to 6. Only regions on
                    // yOffset are considered, because the xOffset ones are too narrow
                    auto yOffsetUnrounded =
                    static_cast<float>(mChar->GetY()) / static_cast<float>(MapRowSize);
                    std::int8_t counter2Start = 0, counter2End = 0;
                    if (yOffsetUnrounded < yOffset + 0.25) {
                        counter2Start = -1;
                        counter2End = 0;
                    }
                    else if (yOffsetUnrounded > yOffset + 0.75) {
                        counter2Start = 0;
                        counter2End = 1;
                    }
                    
                    for (std::int8_t counter = -1; counter <= 1; ++counter) {
                        // Check 3 x colums
                        for (std::int8_t ctr2 = counter2Start; ctr2 <= counter2End; ++ctr2) {
                            // Check variable y colums
                            auto tC = worldMapHandler.GetMapRegion(xOffset + counter, yOffset + ctr2, worldNumber);
                            if (tC) {
                                regionList.insert(tC);
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Reduce some lag checking these timers constantly in the loop
    bool setNPCFlags = false, checkItems = false, checkAI = false, doRestock = false;
    if ((nextSetNPCFlagTime <= GetUICurrentTime()) || GetOverflow()) {
        nextSetNPCFlagTime = BuildTimeValue(static_cast<float>( ServerConfig::shared().timerSetting[TimerSetting::NPCFLAGUPDATETIMER])); // Slow down lag "needed" for setting flags, they
        // are set often enough;-)
        setNPCFlags = true;
    }
    if ((nextCheckItems <= GetUICurrentTime()) || GetOverflow()) {
        nextCheckItems = BuildTimeValue(static_cast<float>( ServerConfig::shared().realNumbers[RealNumberConfig::CHECKITEMS]));
        nextDecayItems = BuildTimeValue(static_cast<float>( ServerConfig::shared().timerSetting[TimerSetting::DECAY]));
        nextDecayItemsInHouses = BuildTimeValue(static_cast<float>( ServerConfig::shared().timerSetting[TimerSetting::DECAYINHOUSE]));
        checkItems = true;
    }
    if ((GetTimer(tWORLD_NEXTNPCAI) <= GetUICurrentTime()) || GetOverflow()) {
        
        SetTimer(tWORLD_NEXTNPCAI, BuildTimeValue(static_cast<float>(ServerConfig::shared().realNumbers[RealNumberConfig::CHECKAI])));
        checkAI = true;
    }
    if ((GetTimer(tWORLD_SHOPRESTOCK) <= GetUICurrentTime()) || GetOverflow()) {
        SetTimer(tWORLD_SHOPRESTOCK, BuildTimeValue(static_cast<float>( ServerConfig::shared().timerSetting[TimerSetting::SHOPSPAWN])));
        doRestock = true;
    }
    
    bool allowAwakeNPCs = ServerConfig::shared().enabled(ServerSwitch::AWAKENPC);
    for (auto &toCheck : regionList) {
        auto regChars = toCheck->GetCharList();
        auto collection = regChars->collection();
        for (const auto &charCheck : collection) {
            if (ValidateObject(charCheck)) {
                if (charCheck->IsNpc()) {
                    if (!charCheck->IsAwake() || !allowAwakeNPCs) {
                        // Only perform these checks on NPCs that are not permanently awake
                        if (!GenericCheck(nullptr, (*charCheck), checkFieldEffects, doWeather)) {
                            if (setNPCFlags) {
                                UpdateFlag(charCheck); // only set flag on npcs every 60
                                // seconds (save a little extra lag)
                            }
                            CheckNPC((*charCheck), checkAI, doRestock, doPetOfflineCheck);
                        }
                    }
                }
                else if (charCheck->GetTimer(tPC_LOGOUT)) {
                    AccountEntry &actbTemp = charCheck->GetAccount();
                    if (actbTemp.accountNumber != AccountEntry::INVALID_ACCOUNT) {
                        serial_t oaiw = actbTemp.inGame;
                        if (oaiw == INVALIDSERIAL) {
                            charCheck->SetTimer(tPC_LOGOUT, 0);
                            charCheck->Update();
                        }
                        else if ((oaiw == charCheck->GetSerial()) && ((charCheck->GetTimer(tPC_LOGOUT) <= GetUICurrentTime()) || GetOverflow())) {
                            actbTemp.inGame = INVALIDSERIAL;
                            charCheck->SetTimer(tPC_LOGOUT, 0);
                            
                            // End combat, clear targets
                            charCheck->SetAttacker(nullptr);
                            charCheck->SetWar(false);
                            charCheck->SetTarg(nullptr);
                            
                            charCheck->Update();
                            charCheck->Teleport();
                            
                            // Announce that player has logged out (if enabled)
                            if (ServerConfig::shared().enabled(ServerSwitch::ANNOUNCEJOINPART)) {
                                sysBroadcast(oldstrutil::format(1024, worldDictionary.GetEntry(752),charCheck->GetName().c_str())); // %s has left the realm.
                            }
                        }
                    }
                }
            }
        }
        CheckItem(toCheck, checkItems, nextDecayItems, nextDecayItemsInHouses, doWeather);
    }
    
    // Check NPCs marked as always active, regardless of whether their region is "awake"
    if (allowAwakeNPCs) {
        auto alwaysAwakeChars = worldNPC.GetAlwaysAwakeNPCs();
        std::vector<CChar *> toRemove;
        for (const auto &charCheck : alwaysAwakeChars->collection()) {
            if (ValidateObject(charCheck) && !charCheck->IsFree() && charCheck->IsNpc()) {
                if (!GenericCheck(nullptr, (*charCheck), checkFieldEffects, doWeather)) {
                    if (setNPCFlags) {
                        UpdateFlag(charCheck); // only set flag on npcs every 60 seconds
                        // (save a little extra lag)
                    }
                    CheckNPC((*charCheck), checkAI, doRestock, doPetOfflineCheck);
                }
            }
            else {
                toRemove.push_back(charCheck);
            }
        }
        std::for_each(toRemove.begin(), toRemove.end(),[&alwaysAwakeChars](CChar *character) {
            alwaysAwakeChars->Remove(character);
            
        });
        toRemove.clear();
    }
    
    worldEffect.CheckTempeffects();
    worldSpeechSystem.poll();
    
    // Implement RefreshItem() / StatWindow() queue here
    std::for_each(worldMain.refreshQueue.begin(), worldMain.refreshQueue.end(), [](std::pair<CBaseObject *, std::uint32_t> entry) {
        if (ValidateObject(entry.first)) {
            if (entry.first->CanBeObjType(CBaseObject::OT_CHAR)) {
                auto uChar = static_cast<CChar *>(entry.first);
                
                if (uChar->GetUpdate(UT_HITPOINTS)) {
                    UpdateStats(entry.first, 0);
                }
                if (uChar->GetUpdate(UT_STAMINA)) {
                    UpdateStats(entry.first, 1);
                }
                if (uChar->GetUpdate(UT_MANA)) {
                    UpdateStats(entry.first, 2);
                }
                
                if (uChar->GetUpdate(UT_LOCATION)) {
                    uChar->Teleport();
                }
                else if (uChar->GetUpdate(UT_HIDE)) {
                    uChar->ClearUpdate();
                    if (uChar->GetVisible() != VT_VISIBLE) {
                        uChar->RemoveFromSight();
                    }
                    uChar->Update(nullptr, false);
                }
                else if (uChar->GetUpdate(UT_UPDATE)) {
                    uChar->Update();
                }
                else if (uChar->GetUpdate(UT_STATWINDOW)) {
                    CSocket *uSock = uChar->GetSocket();
                    if (uSock) {
                        uSock->StatWindow(uChar);
                    }
                }
                
                uChar->ClearUpdate();
            }
            else {
                entry.first->Update();
            }
        }
    });
    worldMain.refreshQueue.clear();
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	initClasses()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Initialize UOX classes
// o------------------------------------------------------------------------------------------------o
auto initClasses() -> void {
    worldMain.ClassesInitialized(true);
    
    worldJSEngine.startup();
    worldFileLookup.startup();
    serverCommands.startup();
    worldSpeechSystem.startup();
    // Need to do map
    worldNetwork.startup();
    worldMULHandler.load();
    worldJSMapping.ResetDefaults();
    worldJSMapping.GetEnvokeById()->Parse();
    worldJSMapping.GetEnvokeByType()->Parse();
    worldMapHandler.startup();
    Account::accountDirectory = ServerConfig::shared().directoryFor(dirlocation_t::ACCOUNT);
}

auto FindNearbyObjects(std::int16_t x, std::int16_t y, std::uint8_t worldNumber, std::uint16_t instanceId, std::uint16_t distance)-> std::vector<CBaseObject *>;
auto InMulti(std::int16_t x, std::int16_t y, std::int8_t z, CMultiObj *m) -> bool;
// o------------------------------------------------------------------------------------------------o
//|	Function	-	FindMultiFunctor()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Looks for a multi at object's location and assigns any multi found
// to object
// o------------------------------------------------------------------------------------------------o
auto FindMultiFunctor(CBaseObject *a, [[maybe_unused]] std::uint32_t &b, [[maybe_unused]] void *extraData)-> bool {
    if (ValidateObject(a)) {
        if (a->CanBeObjType(CBaseObject::OT_MULTI)) {
            auto aMulti = static_cast<CMultiObj *>(a);
            for (auto &objToCheck :
                 FindNearbyObjects(aMulti->GetX(), aMulti->GetY(), aMulti->WorldNumber(),aMulti->GetInstanceId(), 20)) {
                if (InMulti(objToCheck->GetX(), objToCheck->GetY(), objToCheck->GetZ(),aMulti)) {
                    objToCheck->SetMulti(aMulti);
                }
                else if ((objToCheck->GetObjType() == CBaseObject::OT_ITEM) && (((objToCheck->GetId() >= 0x0b95) && (objToCheck->GetId() <= 0x0c0e)) || (objToCheck->GetId() == 0x1f28) || (objToCheck->GetId() == 0x1f29))) {
                    // Reunite house signs with their multis
                    serial_t houseSerial =
                    static_cast<CItem *>(objToCheck)->GetTempVar(CITV_MORE);
                    CMultiObj *multi = CalcMultiFromSer(houseSerial);
                    if (ValidateObject(multi)) {
                        objToCheck->SetMulti(multi);
                    }
                }
                else {
                    // No other multi found where item is, safe to set item's multi to
                    // INVALIDSERIAL
                    if (FindMulti(objToCheck) == nullptr) {
                        objToCheck->SetMulti(INVALIDSERIAL);
                    }
                }
            }
        }
    }
    return true;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	initMultis()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Initialize Multis
// o------------------------------------------------------------------------------------------------o
auto initMultis() -> void {
    Console::shared() << "Initializing multis            ";
    
    std::uint32_t b = 0;
    ObjectFactory::shared().IterateOver(CBaseObject::OT_MULTI, b, nullptr, &FindMultiFunctor);
    
    Console::shared().printDone();
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	displayBanner()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Display some information at the end of UOX startup
// o------------------------------------------------------------------------------------------------o
auto displayBanner() -> void {
    Console::shared().printSectionBegin();
    
    Console::shared().turnYellow();
    Console::shared() << "Compiled on ";
    Console::shared().turnNormal();
    Console::shared() << __DATE__ << " (" << __TIME__ << ")" << myendl;
    
    Console::shared().turnYellow();
    Console::shared() << "Compiled by ";
    Console::shared().turnNormal();
//    Console::shared() << UOXVersion::name << myendl;
    Console::shared() << "punt" << myendl;

    Console::shared().turnYellow();
    Console::shared() << "Contact: ";
    Console::shared().turnNormal();
    Console::shared() << UOXVersion::email << myendl;
    
    Console::shared().printSectionBegin();
    saveOnShutdown = true;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Shutdown()
//|	Date		-	Oct. 09, 1999
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handled deleting / free() ing of pointers as neccessary
//|					as well as closing open file handles to avoid file file
// corruption. |					Exits with proper error code.
// o------------------------------------------------------------------------------------------------o
auto Shutdown(std::int32_t retCode) -> void {
    Console::shared().printSectionBegin();
    Console::shared() << "Beginning UOX final shut down sequence..." << myendl;
    if (retCode && saveOnShutdown) {
        // they want us to save, there has been an error, we have loaded the world, and
        // WorldState is a valid pointer.
#if defined(_WIN32)
        SetConsoleCtrlHandler(exit_handler, true);
#endif
        isWorldSaving = true;
        do {
            worldMain.SaveNewWorld(true);
        } while (worldMain.GetWorldSaveProgress() == SS_SAVING);
        isWorldSaving = false;
#if defined(_WIN32)
        SetConsoleCtrlHandler(exit_handler, false);
#endif
    }
    
    if (worldMain.ClassesInitialized()) {
            Console::shared() << "HTMLTemplates object detected. Writing Offline HTML Now...";
            worldHTMLTemplate.poll(ETT_OFFLINE);
            Console::shared().printDone();
        
        Console::shared() << "Destroying class objects and pointers... ";
        // delete any objects that were created (delete takes care of nullptr check =)
        UnloadSpawnRegions();
        
        UnloadRegions();
        Console::shared().printDone();
    }
    
    // Lets wait for console thread to quit here
    if (!retCode) {
        cons.join();
    }
    
    // don't leave file pointers open, could lead to file corruption
    
    Console::shared().printSectionBegin();
    
    Console::shared().turnGreen();
    Console::shared() << "Server shutdown complete!" << myendl;
    Console::shared() << "Thank you for supporting " << UOXVersion::name << myendl;
    Console::shared().turnNormal();
    Console::shared().printSectionBegin();
    
    // dispay what error code we had
    // don't report errorlevel for no errors, this is confusing ppl
    if (retCode) {
        Console::shared().turnRed();
        Console::shared() << "Exiting UOX with errorlevel " << retCode << myendl;
        Console::shared().turnNormal();
#if defined(_WIN32)
        Console::shared() << "Press Return to exit " << myendl;
        std::string throwAway;
        std::getline(std::cin, throwAway);
#endif
    }
    else {
        Console::shared().turnGreen();
        Console::shared() << "Exiting UOX with no errors..." << myendl;
        Console::shared().turnNormal();
    }
    
    Console::shared().printSectionBegin();
    if (cons.joinable()) {
        cons.join();
    }
    exit(retCode);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	AdvanceObj()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle advancement objects (stat / skill gates)
// o------------------------------------------------------------------------------------------------o
auto AdvanceObj(CChar *applyTo, std::uint16_t advObj, bool multiUse) -> void {
    if ((applyTo->GetAdvObj() == 0) || multiUse) {
        worldEffect.PlayStaticAnimation(applyTo, 0x373A, 0, 15);
        worldEffect.PlaySound(applyTo, 0x01E9);
        applyTo->SetAdvObj(advObj);
        auto sect = "ADVANCEMENT "s + util::ntos(advObj);
        sect = util::trim(util::strip(sect, "//"));
        auto Advancement = worldFileLookup.FindEntry(sect, advance_def);
        if (Advancement == nullptr) {
            Console::shared() << "ADVANCEMENT OBJECT: Script section not found, Aborting" << myendl;
            applyTo->SetAdvObj(0);
            return;
        }
        CItem *retItem = nullptr;
        auto hairobject = applyTo->GetItemAtLayer(IL_HAIR);
        auto beardobject = applyTo->GetItemAtLayer(IL_FACIALHAIR);
        auto tag = DFNTAG_COUNTOFTAGS;
        std::string cdata;
        std::int32_t ndata = -1, odata = -1;
        std::uint8_t skillToSet = 0;
        for (const auto &sec : Advancement->collection2()) {
            tag = sec->tag;
            cdata = sec->cdata;
            ndata = sec->ndata;
            odata = sec->odata;
            
            switch (tag) {
                case DFNTAG_ALCHEMY:
                    skillToSet = ALCHEMY;
                    break;
                case DFNTAG_ANATOMY:
                    skillToSet = ANATOMY;
                    break;
                case DFNTAG_ANIMALLORE:
                    skillToSet = ANIMALLORE;
                    break;
                case DFNTAG_ARMSLORE:
                    skillToSet = ARMSLORE;
                    break;
                case DFNTAG_ARCHERY:
                    skillToSet = ARCHERY;
                    break;
                case DFNTAG_ADVOBJ:
                    applyTo->SetAdvObj(static_cast<std::uint16_t>(ndata));
                    break;
                case DFNTAG_BEGGING:
                    skillToSet = BEGGING;
                    break;
                case DFNTAG_BLACKSMITHING:
                    skillToSet = BLACKSMITHING;
                    break;
                case DFNTAG_BOWCRAFT:
                    skillToSet = BOWCRAFT;
                    break;
                case DFNTAG_BUSHIDO:
                    skillToSet = BUSHIDO;
                    break;
                case DFNTAG_CAMPING:
                    skillToSet = CAMPING;
                    break;
                case DFNTAG_CARPENTRY:
                    skillToSet = CARPENTRY;
                    break;
                case DFNTAG_CARTOGRAPHY:
                    skillToSet = CARTOGRAPHY;
                    break;
                case DFNTAG_CHIVALRY:
                    skillToSet = CHIVALRY;
                    break;
                case DFNTAG_COOKING:
                    skillToSet = COOKING;
                    break;
                case DFNTAG_DEX:
                    applyTo->SetDexterity(static_cast<std::int16_t>(RandomNum(ndata, odata)));
                    break;
                case DFNTAG_DETECTINGHIDDEN:
                    skillToSet = DETECTINGHIDDEN;
                    break;
                case DFNTAG_DYEHAIR:
                    if (ValidateObject(hairobject)) {
                        hairobject->SetColour(static_cast<std::uint16_t>(ndata));
                    }
                    break;
                case DFNTAG_DYEBEARD:
                    if (ValidateObject(beardobject)) {
                        beardobject->SetColour(static_cast<std::uint16_t>(ndata));
                    }
                    break;
                case DFNTAG_ENTICEMENT:
                    skillToSet = ENTICEMENT;
                    break;
                case DFNTAG_EVALUATINGINTEL:
                    skillToSet = EVALUATINGINTEL;
                    break;
                case DFNTAG_EQUIPITEM:
                    retItem =
                    worldItem.CreateBaseScriptItem(nullptr, cdata, applyTo->WorldNumber(), 1);
                    if (retItem) {
                        if (!retItem->SetCont(applyTo)) {
                            retItem->SetCont(applyTo->GetPackItem());
                            retItem->PlaceInPack();
                        }
                    }
                    break;
                case DFNTAG_FAME:
                    applyTo->SetFame(static_cast<std::int16_t>(ndata));
                    break;
                case DFNTAG_FENCING:
                    skillToSet = FENCING;
                    break;
                case DFNTAG_FISHING:
                    skillToSet = FISHING;
                    break;
                case DFNTAG_FOCUS:
                    skillToSet = FOCUS;
                    break;
                case DFNTAG_FORENSICS:
                    skillToSet = FORENSICS;
                    break;
                case DFNTAG_HEALING:
                    skillToSet = HEALING;
                    break;
                case DFNTAG_HERDING:
                    skillToSet = HERDING;
                    break;
                case DFNTAG_HIDING:
                    skillToSet = HIDING;
                    break;
                case DFNTAG_IMBUING:
                    skillToSet = IMBUING;
                    break;
                case DFNTAG_INTELLIGENCE:
                    applyTo->SetIntelligence(static_cast<std::int16_t>(RandomNum(ndata, odata)));
                    break;
                case DFNTAG_ITEMID:
                    skillToSet = ITEMID;
                    break;
                case DFNTAG_INSCRIPTION:
                    skillToSet = INSCRIPTION;
                    break;
                case DFNTAG_KARMA:
                    applyTo->SetKarma(static_cast<std::int16_t>(ndata));
                    break;
                case DFNTAG_KILLHAIR:
                    retItem = applyTo->GetItemAtLayer(IL_HAIR);
                    if (ValidateObject(retItem)) {
                        retItem->Delete();
                    }
                    break;
                case DFNTAG_KILLBEARD:
                    retItem = applyTo->GetItemAtLayer(IL_FACIALHAIR);
                    if (ValidateObject(retItem)) {
                        retItem->Delete();
                    }
                    break;
                case DFNTAG_KILLPACK:
                    retItem = applyTo->GetItemAtLayer(IL_PACKITEM);
                    if (ValidateObject(retItem)) {
                        retItem->Delete();
                    }
                    break;
                case DFNTAG_LOCKPICKING:
                    skillToSet = LOCKPICKING;
                    break;
                case DFNTAG_LUMBERJACKING:
                    skillToSet = LUMBERJACKING;
                    break;
                case DFNTAG_MAGERY:
                    skillToSet = MAGERY;
                    break;
                case DFNTAG_MAGICRESISTANCE:
                    skillToSet = MAGICRESISTANCE;
                    break;
                case DFNTAG_MACEFIGHTING:
                    skillToSet = MACEFIGHTING;
                    break;
                case DFNTAG_MEDITATION:
                    skillToSet = MEDITATION;
                    break;
                case DFNTAG_MINING:
                    skillToSet = MINING;
                    break;
                case DFNTAG_MUSICIANSHIP:
                    skillToSet = MUSICIANSHIP;
                    break;
                case DFNTAG_MYSTICISM:
                    skillToSet = MYSTICISM;
                    break;
                case DFNTAG_NECROMANCY:
                    skillToSet = NECROMANCY;
                    break;
                case DFNTAG_NINJITSU:
                    skillToSet = NINJITSU;
                    break;
                case DFNTAG_PARRYING:
                    skillToSet = PARRYING;
                    break;
                case DFNTAG_PEACEMAKING:
                    skillToSet = PEACEMAKING;
                    break;
                case DFNTAG_POISONING:
                    skillToSet = POISONING;
                    break;
                case DFNTAG_PROVOCATION:
                    skillToSet = PROVOCATION;
                    break;
                case DFNTAG_POLY:
                    applyTo->SetId(static_cast<std::uint16_t>(ndata));
                    break;
                case DFNTAG_PACKITEM:
                    if (ValidateObject(applyTo->GetPackItem())) {
                        auto csecs = oldstrutil::sections(cdata, ",");
                        if (!cdata.empty()) {
                            if (csecs.size() > 1) {
                                retItem = worldItem.CreateScriptItem(nullptr, applyTo, util::trim(util::strip(csecs[0], "//")),util::ston<std::uint16_t>(util::trim(util::strip(csecs[1], "//"))),CBaseObject::OT_ITEM, true);
                            }
                            else {
                                retItem = worldItem.CreateScriptItem(nullptr, applyTo, cdata, 1,CBaseObject::OT_ITEM, true);
                            }
                        }
                    }
                    else {
                        Console::shared() << "Warning: Bad NPC Script with problem no backpack for packitem"  << myendl;
                    }
                    break;
                case DFNTAG_REMOVETRAP:
                    skillToSet = REMOVETRAP;
                    break;
                case DFNTAG_STRENGTH:
                    applyTo->SetStrength(static_cast<std::int16_t>(RandomNum(ndata, odata)));
                    break;
                case DFNTAG_SKILL:
                    applyTo->SetBaseSkill(static_cast<std::uint16_t>(odata), static_cast<std::uint8_t>(ndata));
                    break;
                case DFNTAG_SKIN:
                    applyTo->SetSkin(static_cast<std::uint16_t>(std::stoul(cdata, nullptr, 0)));
                    break;
                case DFNTAG_SNOOPING:
                    skillToSet = SNOOPING;
                    break;
                case DFNTAG_SPELLWEAVING:
                    skillToSet = SPELLWEAVING;
                    break;
                case DFNTAG_SPIRITSPEAK:
                    skillToSet = SPIRITSPEAK;
                    break;
                case DFNTAG_STEALING:
                    skillToSet = STEALING;
                    break;
                case DFNTAG_STEALTH:
                    skillToSet = STEALTH;
                    break;
                case DFNTAG_SWORDSMANSHIP:
                    skillToSet = SWORDSMANSHIP;
                    break;
                case DFNTAG_TACTICS:
                    skillToSet = TACTICS;
                    break;
                case DFNTAG_TAILORING:
                    skillToSet = TAILORING;
                    break;
                case DFNTAG_TAMING:
                    skillToSet = TAMING;
                    break;
                case DFNTAG_TASTEID:
                    skillToSet = TASTEID;
                    break;
                case DFNTAG_THROWING:
                    skillToSet = THROWING;
                    break;
                case DFNTAG_TINKERING:
                    skillToSet = TINKERING;
                    break;
                case DFNTAG_TRACKING:
                    skillToSet = TRACKING;
                    break;
                case DFNTAG_VETERINARY:
                    skillToSet = VETERINARY;
                    break;
                case DFNTAG_WRESTLING:
                    skillToSet = WRESTLING;
                    break;
                default:
                    Console::shared() << "Unknown tag in AdvanceObj(): " << static_cast<std::int32_t>(tag) << myendl;
                    break;
            }
            if (skillToSet > 0) {
                applyTo->SetBaseSkill(static_cast<std::uint16_t>(RandomNum(ndata, odata)),skillToSet);
                skillToSet = 0; // reset for next time through
            }
        }
        applyTo->Teleport();
    }
    else {
        auto sock = applyTo->GetSocket();
        if (sock) {
            sock->SysMessage(1366); // You have already used an advancement object with this character.
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	GetClock()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Return CPU time used, Emulates clock()
// o------------------------------------------------------------------------------------------------o
auto GetClock() -> std::uint32_t {
    return static_cast<std::uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - current).count());
}


// o------------------------------------------------------------------------------------------------o
//|	Function	-	IsNumber()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if string is a number, false if not
// o------------------------------------------------------------------------------------------------o
auto IsNumber(const std::string &str) -> bool {
    return str.find_first_not_of("0123456789") == std::string::npos;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	DoLight()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets light level for player and applies relevant effects
// o------------------------------------------------------------------------------------------------o
auto DoLight(CSocket *s, std::uint8_t level) -> void {
    if (s == nullptr)
        return;
    
    auto mChar = s->CurrcharObj();
    CPLightLevel toSend(level);
    
    if ((worldRace.Affect(mChar->GetRace(), Weather::LIGHT)) && mChar->GetWeathDamage(LIGHT) == 0) {
        mChar->SetWeathDamage(static_cast<std::uint32_t>(BuildTimeValue(static_cast<float>(worldRace.Secs(mChar->GetRace(), Weather::LIGHT)))),Weather::LIGHT);
    }
    
    if (mChar->GetFixedLight() != 255) {
        toSend.Level(mChar->GetFixedLight());
        s->Send(&toSend);
        worldWeather.doPlayerStuff(s, mChar);
        return;
    }
    
    auto curRegion = mChar->GetRegion();
    auto toShow = worldMain.uoTime.worldLightLevel;
    
    auto dunLevel = ServerConfig::shared().ushortValues[UShortValue::DUNGEONLIGHT];
    // we have a valid weather system
    if (curRegion->GetWeather() < worldWeather.size()) {
        
        const float lightMin = worldWeather[curRegion->GetWeather()].impact[Weather::BRIGHTNESS][Weather::MIN] ;
        const float lightMax = worldWeather[curRegion->GetWeather()].impact[Weather::BRIGHTNESS][Weather::MAX] ;
        if (lightMin < 300 && lightMax < 300) {
            auto i = worldWeather[curRegion->GetWeather()].impact[Weather::BRIGHTNESS][Weather::CURRENT];
            if (worldRace.VisLevel(mChar->GetRace()) > i) {
                toShow = 0;
            }
            else {
                toShow = static_cast<lightlevel_t>(std::round(i - worldRace.VisLevel(mChar->GetRace())));
            }
            toSend.Level(toShow);
        }
        else {
            toSend.Level(level);
        }
    }
    else {
        if (mChar->InDungeon()) {
            if (worldRace.VisLevel(mChar->GetRace()) > dunLevel) {
                toShow = 0;
            }
            else {
                toShow = static_cast<lightlevel_t>(std::round(dunLevel - worldRace.VisLevel(mChar->GetRace())));
            }
            toSend.Level(toShow);
        }
    }
    s->Send(&toSend);
    
    auto eventFound = false;
    auto scriptTriggers = mChar->GetScriptTriggers();
    for (auto scriptTrig : scriptTriggers) {
        auto toExecute = worldJSMapping.GetScript(scriptTrig);
        if (toExecute) {
            if (toExecute->OnLightChange(mChar, toShow) == 1) {
                // A script with the event returned true; prevent other scripts from running
                eventFound = true;
                break;
            }
        }
    }
    
    if (!eventFound) {
        // Check global script! Maybe there's another event there
        auto toExecute = worldJSMapping.GetScript(static_cast<std::uint16_t>(0));
        if (toExecute) {
            toExecute->OnLightChange(mChar, toShow);
        }
    }
    
    worldWeather.doPlayerStuff(s, mChar);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	DoLight()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets light level for character and applies relevant effects
// o------------------------------------------------------------------------------------------------o
auto DoLight(CChar *mChar, std::uint8_t level) -> void {
    if ((worldRace.Affect(mChar->GetRace(), Weather::LIGHT)) && (mChar->GetWeathDamage(LIGHT) == 0)) {
        mChar->SetWeathDamage(static_cast<std::uint32_t>(BuildTimeValue( static_cast<float>(worldRace.Secs(mChar->GetRace(), Weather::LIGHT)))), Weather::LIGHT);
    }
    
    auto curRegion = mChar->GetRegion();
    auto wSys = worldWeather.pointerForRegion( curRegion->GetWeather());
    
    lightlevel_t toShow = level;
    lightlevel_t dunLevel = ServerConfig::shared().ushortValues[UShortValue::DUNGEONLIGHT] ;
    
    // we have a valid weather system
    if (wSys) {
        auto lightMin = (*wSys).impact[Weather::BRIGHTNESS][Weather::MIN];
        auto lightMax = (*wSys).impact[Weather::BRIGHTNESS][Weather::MAX];
        if (lightMin < 300 && lightMax < 300) {
            auto i = (*wSys).impact[Weather::BRIGHTNESS][Weather::CURRENT];
            if (worldRace.VisLevel(mChar->GetRace()) > i) {
                toShow = 0;
            }
            else {
                toShow = static_cast<lightlevel_t>( std::round(i - worldRace.VisLevel(mChar->GetRace())));
            }
        }
    }
    else {
        if (mChar->InDungeon()) {
            if (worldRace.VisLevel(mChar->GetRace()) > dunLevel) {
                toShow = 0;
            }
            else {
                toShow = static_cast<lightlevel_t>( std::round(dunLevel - worldRace.VisLevel(mChar->GetRace())));
            }
        }
    }
    
    bool eventFound = false;
    auto scriptTriggers = mChar->GetScriptTriggers();
    for (auto scriptTrig : scriptTriggers) {
        auto toExecute = worldJSMapping.GetScript(scriptTrig);
        if (toExecute) {
            if (toExecute->OnLightChange(mChar, toShow) == 1) {
                // A script with the event returned true; prevent other scripts from running
                eventFound = true;
                break;
            }
        }
    }
    
    if (!eventFound) {
        // Check global script! Maybe there's another event there
        auto toExecute = worldJSMapping.GetScript(static_cast<std::uint16_t>(0));
        if (toExecute) {
            toExecute->OnLightChange(mChar, toShow);
        }
    }
    
    worldWeather.doNPCStuff(mChar);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	DoLight()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets light level for items and applies relevant effects
// o------------------------------------------------------------------------------------------------o
auto DoLight(CItem *mItem, std::uint8_t level) -> void {
    auto curRegion = mItem->GetRegion();
    auto wSys = worldWeather.pointerForRegion( curRegion->GetWeather());
    
    lightlevel_t toShow = level;
    lightlevel_t dunLevel = ServerConfig::shared().ushortValues[UShortValue::DUNGEONLIGHT] ;
    
    // we have a valid weather system
    if (wSys) {
        auto lightMin = (*wSys).impact[Weather::BRIGHTNESS][Weather::MIN];
        auto lightMax = (*wSys).impact[Weather::BRIGHTNESS][Weather::MAX];
        if ((lightMin < 300) && (lightMax < 300)) {
            toShow = static_cast<lightlevel_t>((*wSys).impact[Weather::BRIGHTNESS][Weather::CURRENT]);
        }
    }
    else {
        if (mItem->InDungeon()) {
            toShow = dunLevel;
        }
    }
    
    auto eventFound = false;
    auto scriptTriggers = mItem->GetScriptTriggers();
    for (auto scriptTrig : scriptTriggers) {
        auto toExecute = worldJSMapping.GetScript(scriptTrig);
        if (toExecute) {
            if (toExecute->OnLightChange(mItem, toShow) == 1) {
                // A script with the event returned true; prevent other scripts from running
                eventFound = true;
                break;
            }
        }
    }
    
    if (!eventFound) {
        // Check global script! Maybe there's another event there
        auto toExecute = worldJSMapping.GetScript(static_cast<std::uint16_t>(0));
        if (toExecute) {
            toExecute->OnLightChange(mItem, toShow);
        }
    }
    
    worldWeather.doItemStuff(mItem);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	GetPoisonDuration()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculates the duration of poison based on its strength
// o------------------------------------------------------------------------------------------------o
auto GetPoisonDuration(std::uint8_t poisonStrength) -> timerval_t {
    // Calculate duration of poison, based on the strength of the poison
    auto poisonDuration = timerval_t(0);
    switch (poisonStrength) {
        case 1: // Lesser poison - 9 to 13 pulses, 2 second frequency
            poisonDuration = RandomNum(9, 13) * 2;
            break;
        case 2: // Normal poison - 10 to 14 pulses, 3 second frequency
            poisonDuration = RandomNum(10, 14) * 3;
            break;
        case 3: // Greater poison - 11 to 15 pulses, 4 second frequency
            poisonDuration = RandomNum(11, 15) * 4;
            break;
        case 4: // Deadly poison - 12 to 16 pulses, 5 second frequency
            poisonDuration = RandomNum(12, 16) * 5;
            break;
        case 5: // Lethal poison - 13 to 17 pulses, 5 second frequency
            poisonDuration = RandomNum(13, 17) * 5;
            break;
    }
    return poisonDuration;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	GetPoisonTickTime()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculates the time between each tick of a poison, based on its
// strength
// o------------------------------------------------------------------------------------------------o
auto GetPoisonTickTime(std::uint8_t poisonStrength) -> timerval_t {
    // Calculate duration of poison, based on the strength of the poison
    auto poisonTickTime = timerval_t(0);
    switch (poisonStrength) {
        case 1: // Lesser poison - 2 second frequency
            poisonTickTime = 2;
            break;
        case 2: // Normal poison - 3 second frequency
            poisonTickTime = 3;
            break;
        case 3: // Greater poison - 4 second frequency
            poisonTickTime = 4;
            break;
        case 4: // Deadly poison - 5 second frequency
            poisonTickTime = 5;
            break;
        case 5: // Lethal poison - 5 second frequency
            poisonTickTime = 5;
            break;
    }
    return poisonTickTime;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	GetTileName()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the length of an items name from tiledata.mul and
//|					sets itemname to the name.
//|					The format it accepts is same as UO style - %plural/single%
//or %plural% |						arrow%s% | loa%ves/f% of bread
// o------------------------------------------------------------------------------------------------o
auto GetTileName(CItem &mItem, std::string &itemname) -> size_t {
    std::string temp = mItem.GetName();
    temp = util::trim(util::strip(temp, "//"));
    const std::uint16_t getAmount = mItem.GetAmount();
    CTile &tile = worldMULHandler.SeekTile(mItem.GetId());
    if (temp.substr(0, 1) == "#") {
        temp = tile.Name();
    }
    
    if (getAmount == 1) {
        if (tile.CheckFlag(uo::flag_t::DISPLAYAN)) {
            temp = "an " + temp;
        }
        else if (tile.CheckFlag(uo::flag_t::DISPLAYA)) {
            temp = "a " + temp;
        }
    }
    
    auto psecs = oldstrutil::sections(temp, "%");
    // Find out if the name has a % in it
    if (psecs.size() > 2) {
        std::string single;
        const std::string first = psecs[0];
        std::string plural = psecs[1];
        const std::string rest = psecs[2];
        auto fssecs = oldstrutil::sections(plural, "/");
        if (fssecs.size() > 1) {
            single = fssecs[1];
            plural = fssecs[0];
        }
        if (getAmount < 2) {
            temp = first + single + rest;
        }
        else {
            temp = first + plural + rest;
        }
    }
    itemname = util::simplify(temp);
    return itemname.size() + 1;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	GetNpcDictName()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the dictionary name for a given NPC, if their name equals #
// or a dictionary ID
// o------------------------------------------------------------------------------------------------o
auto GetNpcDictName(CChar *mChar, CSocket *tSock, std::uint8_t requestSource) -> std::string {
    CChar *tChar = nullptr;
    if (tSock) {
        tChar = tSock->CurrcharObj();
    }
    
    std::string dictName = mChar->GetNameRequest(tChar, requestSource);
    std::int32_t dictEntryId = 0;
    
    if (dictName == "#") {
        // If character name is #, get dictionary entry based on base dictionary entry for
        // creature names (3000) plus character's ID
        dictEntryId = static_cast<std::int32_t>(3000 + mChar->GetId());
        if (tSock) {
            dictName = worldDictionary.GetEntry(dictEntryId, tSock->Language());
        }
        else {
            dictName = worldDictionary.GetEntry(dictEntryId);
        }
    }
    else if (IsNumber(dictName)) {
        // If name is a number, assume it's a direct dictionary entry reference, and use
        // that
        dictEntryId = static_cast<std::int32_t>(util::ston<std::int32_t>(dictName));
        if (tSock) {
            dictName = worldDictionary.GetEntry(dictEntryId, tSock->Language());
        }
        else {
            dictName = worldDictionary.GetEntry(dictEntryId);
        }
    }
    
    return dictName;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	GetNpcDictTitle()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the dictionary string for the title of a given NPC, if their
// title |					equals a dictionary ID
// o------------------------------------------------------------------------------------------------o
auto GetNpcDictTitle(CChar *mChar, CSocket *tSock) -> std::string {
    std::string dictTitle = mChar->GetTitle();
    std::int32_t dictEntryId = 0;
    
    if (!dictTitle.empty() && IsNumber(dictTitle)) {
        // If title is a number, assume it's a direct dictionary entry reference, and use
        // that
        dictEntryId = static_cast<std::int32_t>(util::ston<std::int32_t>(dictTitle));
        if (tSock) {
            dictTitle = worldDictionary.GetEntry(dictEntryId, tSock->Language());
        }
        else {
            dictTitle = worldDictionary.GetEntry(dictEntryId);
        }
    }
    
    return dictTitle;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CheckRegion()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check what region a character is in, updating it if necesarry.
// o------------------------------------------------------------------------------------------------o
auto CheckRegion(CSocket *mSock, CChar &mChar, bool forceUpdateLight) -> void {
    // Get character's old/previous region
    auto iRegion = mChar.GetRegion();
    auto oldSubRegionNum = mChar.GetSubRegion();
    
    // Calculate character's current region
    auto calcReg = CalcRegionFromXY(mChar.GetX(), mChar.GetY(), mChar.WorldNumber(), mChar.GetInstanceId(), &mChar);
    
    if ((iRegion == nullptr) && (calcReg != nullptr)) {
        mChar.SetRegion(calcReg->GetRegionNum());
    }
    else if (calcReg != iRegion) {
        if (mSock) {
            if (iRegion != nullptr && calcReg != nullptr) {
                // Don't display left/entered region messages if name of region is identical
                if (iRegion->GetName() != calcReg->GetName()) {
                    if (!iRegion->GetName().empty()) {
                        mSock->SysMessage(1358, iRegion->GetName().c_str()); // You have left %s.
                    }
                    
                    if (!calcReg->GetName().empty()) {
                        mSock->SysMessage(1359, calcReg->GetName().c_str()); // You have entered %s.
                    }
                }
                if (calcReg->IsGuarded() || iRegion->IsGuarded()) {
                    if (calcReg->IsGuarded()) {
                        // Don't display change of guard message if guardowner is identical
                        if (!iRegion->IsGuarded() || (iRegion->IsGuarded() && calcReg->GetOwner() != iRegion->GetOwner())) {
                            if (calcReg->GetOwner().empty()) {
                                mSock->SysMessage(1360); // You are now under the protection
                                // of the guards.
                            }
                            else {
                                mSock->SysMessage(1361, calcReg->GetOwner().c_str()); // You are now under the
                                // protection of %s guards.
                            }
                        }
                    }
                    else {
                        if (iRegion->GetOwner().empty()) {
                            mSock->SysMessage(1362); // You are no longer under the
                            // protection of the guards.
                        }
                        else {
                            mSock->SysMessage(1363, iRegion->GetOwner().c_str()); // You are no longer under the
                            // protection of %s guards.
                        }
                    }
                    UpdateFlag(&mChar);
                }
                if (calcReg->GetAppearance() != iRegion->GetAppearance()) { // if the regions look different
                    
                    CPWorldChange wrldChange(calcReg->GetAppearance(), 1);
                    mSock->Send(&wrldChange);
                }
                if (calcReg == worldMain.townRegions[mChar.GetTown()])  { // enter our home town
                    
                    mSock->SysMessage(1364); // You feel loved and cherished under the
                    // protection of your home town.
                    CItem *packItem = mChar.GetPackItem();
                    if (ValidateObject(packItem)) {
                        auto piCont = packItem->GetContainsList();
                        for (const auto &toScan : piCont->collection()) {
                            if (ValidateObject(toScan)) {
                                if (toScan->GetType() == IT_TOWNSTONE) {
                                    CTownRegion *targRegion = worldMain.townRegions[static_cast<std::uint16_t>(toScan->GetTempVar(CITV_MOREX))];
                                    mSock->SysMessage(1365,targRegion->GetName().c_str()); // You have successfully returned the
                                    // townstone of %s to your home town.
                                    targRegion->DoDamage(targRegion->GetHealth()); // finish it off
                                    targRegion->Possess(calcReg);
                                    mChar.SetFame(static_cast<std::int16_t>(mChar.GetFame() +mChar.GetFame() / 5)); // 20% fame boost
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        if (iRegion != nullptr && calcReg != nullptr) {
            // Run onLeaveRegion/onEnterRegion for character
            auto scriptTriggers = mChar.GetScriptTriggers();
            for (auto scriptTrig : scriptTriggers) {
                auto toExecute = worldJSMapping.GetScript(scriptTrig);
                if (toExecute) {
                    toExecute->OnLeaveRegion(&mChar, iRegion->GetRegionNum());
                    toExecute->OnEnterRegion(&mChar, calcReg->GetRegionNum());
                }
            }
            
            // Run onLeaveRegion event for region being left
            scriptTriggers.clear();
            scriptTriggers.shrink_to_fit();
            scriptTriggers = iRegion->GetScriptTriggers();
            for (auto scriptTrig : scriptTriggers) {
                auto toExecute = worldJSMapping.GetScript(scriptTrig);
                if (toExecute) {
                    toExecute->OnLeaveRegion(&mChar, iRegion->GetRegionNum());
                }
            }
            
            // Run onEnterRegion event for region being entered
            scriptTriggers.clear();
            scriptTriggers.shrink_to_fit();
            scriptTriggers = calcReg->GetScriptTriggers();
            for (auto scriptTrig : scriptTriggers) {
                auto toExecute = worldJSMapping.GetScript(scriptTrig);
                if (toExecute) {
                    toExecute->OnEnterRegion(&mChar, calcReg->GetRegionNum());
                }
            }
        }
        if (calcReg) {
            mChar.SetRegion(calcReg->GetRegionNum());
        }
        if (mSock) {
            worldEffect.DoSocketMusic(mSock);
            DoLight(mSock, worldMain.uoTime.worldLightLevel );
        }
    }
    else {
        // Main region didn't change, but subregion did! Update music
        if (oldSubRegionNum != mChar.GetSubRegion()) {
            worldEffect.DoSocketMusic(mSock);
        }
        
        // Update lighting
        if (forceUpdateLight && mSock != nullptr) {
            DoLight(mSock, worldMain.uoTime.worldLightLevel);
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CheckCharInsideBuilding()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if a character is inside a building before applying weather
// effects
// o------------------------------------------------------------------------------------------------o
auto CheckCharInsideBuilding(CChar *c, CSocket *mSock, bool doWeatherStuff) -> void {
    if (!c->GetMounted() && !c->GetStabled()) {
        auto wasInBuilding = c->InBuilding();
        bool isInBuilding = worldMULHandler.InBuilding(c->GetX(), c->GetY(), c->GetZ(), c->WorldNumber(), c->GetInstanceId());
        if (wasInBuilding != isInBuilding) {
            c->SetInBuilding(isInBuilding);
            if (doWeatherStuff) {
                if (c->IsNpc()) {
                    worldWeather.doNPCStuff(c);
                }
                else {
                    worldWeather.doPlayerStuff(mSock, c);
                }
            }
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	WillResultInCriminal()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check flagging, race, and guild info to find if character
//|					should be flagged criminal (returns true if so)
// o------------------------------------------------------------------------------------------------o
auto WillResultInCriminal(CChar *mChar, CChar *targ) -> bool {
    auto tOwner = targ->GetOwnerObj();
    auto mOwner = mChar->GetOwnerObj();
    auto mCharParty = PartyFactory::shared().Get(mChar);
    auto rValue = false;
    if (ValidateObject(mChar) && ValidateObject(targ) && mChar != targ) {
        // Make sure they're not racial enemies, or guild members/guild enemies
        if ((worldRace.Compare(mChar, targ) > RACE_ENEMY) &&
            worldGuildSystem.ResultInCriminal(mChar, targ)) {
            // Make sure they're not in the same party
            if (!mCharParty || mCharParty->HasMember(targ)) {
                // Make sure the target is not the aggressor in the fight
                if (!targ->CheckAggressorFlag(mChar->GetSerial())) {
                    // Make sure target doesn't have an owner
                    if (!ValidateObject(tOwner)) {
                        // Make sure attacker doesn't have an owner
                        if (!ValidateObject(mOwner)) {
                            // Make sure target is innocent
                            if (targ->IsInnocent()) {
                                // All the stars align - this is a criminal action!
                                rValue = true;
                            }
                        }
                    }
                }
            }
        }
    }
    return rValue;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	MakeCriminal()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Make character a criminal
// o------------------------------------------------------------------------------------------------o
auto MakeCriminal(CChar *c) -> void {
    
    c->SetTimer(tCHAR_CRIMFLAG, BuildTimeValue(static_cast<float>(ServerConfig::shared().timerSetting[TimerSetting::CRIMINAL])));
    if (!c->IsCriminal() && !c->IsMurderer()) {
        auto cSock = c->GetSocket();
        if (cSock) {
            cSock->SysMessage(1379); // You are now a criminal!
        }
        UpdateFlag(c);
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	FlagForStealing()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Flag character for stealing
// o------------------------------------------------------------------------------------------------o
auto FlagForStealing(CChar *c) -> void {
    c->SetTimer(tCHAR_STEALFLAG, BuildTimeValue(static_cast<float>(ServerConfig::shared().timerSetting[TimerSetting::STEALINGFLAG])));
    if (!c->IsCriminal() && !c->IsMurderer() && !c->HasStolen()) {
        c->HasStolen(true);
        UpdateFlag(c);
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	UpdateFlag()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Updates character flags
// o------------------------------------------------------------------------------------------------o
auto UpdateFlag(CChar *mChar) -> void {
    if (!ValidateObject(mChar))
        return;
    
    std::uint8_t oldFlag = mChar->GetFlag();
    
    if (mChar->IsTamed()) {
        CChar *i = mChar->GetOwnerObj();
        if (ValidateObject(i)) {
            // Set character's flag to match owner's flag
            mChar->SetFlag(i->GetFlag());
        }
        else {
            // Default to blue, invalid owner detected
            mChar->SetFlagBlue();
            Console::shared().warning(util::format("Tamed Creature has an invalid owner, Serial: 0x%X", mChar->GetSerial()));
        }
    }
    else {
        if (mChar->GetKills() > ServerConfig::shared().ushortValues[UShortValue::MAXKILL]) {
            // Character is flagged as a murderer
            mChar->SetFlagRed();
        }
        else if ((mChar->GetTimer(tCHAR_CRIMFLAG) != 0 || mChar->GetTimer(tCHAR_STEALFLAG) != 0) && (mChar->GetNPCFlag() != fNPC_EVIL)) {
            // Character is flagged as criminal or for stealing
            mChar->SetFlagGray();
        }
        else {
            if (mChar->IsNpc()) {
                auto doSwitch = true;
                if (worldMain.creatures[mChar->GetId()].IsAnimal() && (mChar->GetNpcAiType() != AI_EVIL && mChar->GetNpcAiType() != AI_EVIL_CASTER)) {
                    if (ServerConfig::shared().enabled(ServerSwitch::ANIMALSGUARDED) && mChar->GetRegion()->IsGuarded()) {
                        mChar->SetFlagBlue();
                        doSwitch = false;
                    }
                }
                
                if (doSwitch) {
                    switch (mChar->GetNPCFlag()) {
                        case fNPC_NEUTRAL:
                        default:
                            mChar->SetFlagNeutral();
                            break;
                        case fNPC_INNOCENT:
                            mChar->SetFlagBlue();
                            break;
                        case fNPC_EVIL:
                            mChar->SetFlagRed();
                            break;
                    }
                }
            }
            else {
                mChar->SetFlagBlue();
            }
        }
    }
    
    std::uint8_t newFlag = mChar->GetFlag();
    if (oldFlag != newFlag) {
        auto scriptTriggers = mChar->GetScriptTriggers();
        for (auto scriptTrig : scriptTriggers) {
            auto toExecute = worldJSMapping.GetScript(scriptTrig);
            if (toExecute) {
                if (toExecute->OnFlagChange(mChar, newFlag, oldFlag) == 1) {
                    break;
                }
            }
        }
        
        mChar->Dirty(UT_UPDATE);
    }
    
    if (!mChar->IsNpc()) {
        // Flag was updated, so loop through player's corpses so flagging can be updated for
        // those!
        for (auto tempCorpse = mChar->GetOwnedCorpses()->First();
             !mChar->GetOwnedCorpses()->Finished();
             tempCorpse = mChar->GetOwnedCorpses()->Next()) {
            if (ValidateObject(tempCorpse)) {
                tempCorpse->Dirty(UT_UPDATE);
            }
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SendMapChange()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Send mapchange packet to client to teleport player to new world/map
// o------------------------------------------------------------------------------------------------o
void SendMapChange(std::uint8_t worldNumber, CSocket *sock, [[maybe_unused]] bool initialLogin) {
    if (sock) {
        CPMapChange mapChange(worldNumber);
        sock->Send(&mapChange);
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SocketMapChange()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if conditions are right to send a map change packet to the
// client
// o------------------------------------------------------------------------------------------------o
auto SocketMapChange(CSocket *sock, CChar *charMoving, CItem *gate) -> void {
    if (!sock)
        return;
    
    if (!ValidateObject(gate) || !ValidateObject(charMoving))
        return;
    
    std::uint8_t tWorldNum = static_cast<std::uint8_t>(gate->GetTempVar(CITV_MORE));
    std::uint16_t tInstanceId = gate->GetInstanceId();
    if (!worldMULHandler.MapExists(tWorldNum))
        return;
    
    CChar *toMove = nullptr;
    if (ValidateObject(charMoving)) {
        toMove = charMoving;
    }
    else {
        toMove = sock->CurrcharObj();
    }
    if (!ValidateObject(toMove))
        return;
    
    // Teleport followers to new location too!
    auto myFollowers = toMove->GetFollowerList();
    for (CChar *myFollower = myFollowers->First(); !myFollowers->Finished();
         myFollower = myFollowers->Next()) {
        if (ValidateObject(myFollower)) {
            if (!myFollower->GetMounted() && myFollower->GetOwnerObj() == toMove) {
                if (myFollower->GetNpcWander() == WT_FOLLOW &&
                    ObjInOldRange(toMove, myFollower, DIST_CMDRANGE)) {
                    myFollower->SetLocation(static_cast<std::int16_t>(gate->GetTempVar(CITV_MOREX)), static_cast<std::int16_t>(gate->GetTempVar(CITV_MOREY)),  static_cast<std::int8_t>(gate->GetTempVar(CITV_MOREZ)),  tWorldNum, tInstanceId);
                }
            }
        }
    }
    
    switch (sock->clientType().type) {
        case ClientType::UO3D:
        case ClientType::KRRIOS:
            toMove->SetLocation(static_cast<std::int16_t>(gate->GetTempVar(CITV_MOREX)),static_cast<std::int16_t>(gate->GetTempVar(CITV_MOREY)), static_cast<std::int8_t>(gate->GetTempVar(CITV_MOREZ)), tWorldNum, tInstanceId);
            break;
        default:
            toMove->SetLocation(static_cast<std::int16_t>(gate->GetTempVar(CITV_MOREX)), static_cast<std::int16_t>(gate->GetTempVar(CITV_MOREY)), static_cast<std::int8_t>(gate->GetTempVar(CITV_MOREZ)), tWorldNum, tInstanceId);
            break;
    }
    SendMapChange(tWorldNum, sock);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	DoorMacro()
//|	Date		-	11th October, 1999
//|	Changes		-	(support CSocket *s and door blocking)
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Door use macro support.
// o------------------------------------------------------------------------------------------------o
void DoorMacro(CSocket *s) {
    CChar *mChar = s->CurrcharObj();
    std::int16_t xc = mChar->GetX(), yc = mChar->GetY();
    switch (mChar->GetDir()) {
        case 0:
            --yc;
            break;
        case 1: {
            ++xc;
            --yc;
        } break;
        case 2:
            ++xc;
            break;
        case 3: {
            ++xc;
            ++yc;
        } break;
        case 4:
            ++yc;
            break;
        case 5: {
            --xc;
            ++yc;
        } break;
        case 6:
            --xc;
            break;
        case 7: {
            --xc;
            --yc;
        } break;
    }
    
    for (auto &toCheck : worldMapHandler.PopulateList(mChar)) {
        if (!toCheck)
            continue;
        
        auto regItems = toCheck->GetItemList();
        for (const auto &itemCheck : regItems->collection()) {
            if (!ValidateObject(itemCheck) ||
                itemCheck->GetInstanceId() != mChar->GetInstanceId())
                continue;
            
            std::int16_t distZ = abs(itemCheck->GetZ() - mChar->GetZ());
            if (itemCheck->GetX() == xc && itemCheck->GetY() == yc && distZ < 7) {
                if (itemCheck->GetType() == IT_DOOR || itemCheck->GetType() == IT_LOCKEDDOOR) {
                    // only open doors
                    if (worldJSMapping.GetEnvokeByType()->Check( static_cast<std::uint16_t>(itemCheck->GetType()))) {
                        std::uint16_t envTrig = worldJSMapping.GetEnvokeByType()->GetScript( static_cast<std::uint16_t>(itemCheck->GetType()));
                        auto envExecute = worldJSMapping.GetScript(envTrig);
                        if (envExecute) {
                            [[maybe_unused]] std::int8_t retVal = envExecute->OnUseChecked(mChar, itemCheck);
                        }
                        
                        return;
                    }
                }
            }
        }
    }
}
