//o------------------------------------------------------------------------------------------------o
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
 //o------------------------------------------------------------------------------------------------o

#include <chrono>
#include <random>
#include <thread>
#include <cstdlib>
#include <filesystem>
#include <optional>
#include <numeric>

#include "uox3.h"
#include "weight.h"
#include "books.h"
#include "cGuild.h"
#include "combat.h"
#include "msgboard.h"
#include "townregion.h"
#include "cWeather.hpp"
#include "movement.h"
#include "cRaces.h"
#include "cServerDefinitions.h"
#include "skills.h"
#include "commands.h"
#include "cSpawnRegion.h"
#include "wholist.h"
#include "cMagic.h"
#include "PageVector.h"
#include "speech.h"
#include "cVersionClass.h"
#include "ssection.h"
#include "cHTMLSystem.h"
#include "CGump.h"
#include "CJSMapping.h"
#include "cScript.h"
#include "cEffects.h"
#include "teffect.h"
#include "CPacketSend.h"
#include "classes.h"
#include "cThreadQueue.h"
#include "regions.h"
#include "magic.h"
#include "jail.h"
#include "Dictionary.h"
#include "ObjectFactory.h"
#include "PartySystem.h"
#include "CJSEngine.h"
#include "StringUtility.hpp"
#include "EventTimer.hpp"
#include <atomic>

#if PLATFORM == WINDOWS
#include <process.h>
#include <conio.h>
#endif


//o------------------------------------------------------------------------------------------------o
// Global variables
//o------------------------------------------------------------------------------------------------o
std::thread cons;
std::thread netw;
std::chrono::time_point<std::chrono::system_clock> current;
std::mt19937 generator;
std::random_device rd;  // Will be used to obtain a seed for the random number engine

using namespace std::string_literals;
//o------------------------------------------------------------------------------------------------o
// These should be atomic, for another day
//o------------------------------------------------------------------------------------------------o
bool isWorldSaving = false;
bool conThreadCloseOk	= false;
bool netpollthreadclose	= false;
auto saveOnShutdown = false;

//o------------------------------------------------------------------------------------------------o
// Classes we will use
//o------------------------------------------------------------------------------------------------o
CConsole Console;      // non pointer  class, has initialize
// Non depdendent class
auto aWorld			= CWorldMain();
auto aDictionary	= CDictionaryContainer(); // no startup
auto aCombat		= CHandleCombat();  // No dependency, startup
auto aItems			= cItem();  // No startup, no dependency
auto aNpcs			= CCharStuff();  // nodependency, no startup
auto aSkills		= CSkills(); // no ddependency, no startup
auto aWeight		= CWeight(); // no dependency, no startup
auto aMagic			= CMagic();  // No dependent, no startup
auto aRaces			= cRaces();  // no dependent, no startup

auto aWeather		= cWeatherAb(); // no dependent, no startup
auto aMovement		= CMovement();  // No dependent, no startup
auto aWhoList		= CWhoList(); // no dependent, no startup
auto aOffList		= CWhoList(false); // no dependent, no startup
auto aBooks			= CBooks(); // no dependent, no startup
auto aGMQueue		= PageVector("GM Queue"); // no dependent, no startup
auto aCounselorQueue	= PageVector("Counselor Queue"); // no dependent, no startup
auto aJSMapping		= CJSMapping(); // nodepend, no startup
auto aEffects		= cEffects(); // No dependnt, no startup
auto aHTMLTemplates	= cHTMLTemplates();  // no depend, no startup
auto aGuildSys		= CGuildCollection(); // no depend, no startup
auto aJailSys		= CJailSystem(); // no depend, no startup
// Dependent or have startup()
auto aSpeechSys		= CSpeechQueue(); // has startup
auto aJSEngine 		= CJSEngine(); // has startup
auto aFileLookup	= CServerDefinitions();  // has startup
auto aCommands		= CCommands(); // Restart resets commands, maybe no dependency
auto aMap			= CMulHandler(); // replaced
auto aNetwork		= CNetworkStuff();  // Maybe dependent, has startup
auto aMapRegion		= CMapHandler(); // Dependent (Map->) , has startup
auto aAccounts		= cAccountClass();  // no dpend, use SetPath
 

//o------------------------------------------------------------------------------------------------o
// FileIO Pre-Declarations
//o------------------------------------------------------------------------------------------------o
void		LoadCustomTitle( void );
void		LoadSkills( void );
void		LoadSpawnRegions( void );
void		LoadRegions( void );
void		LoadTeleportLocations( void );
void		LoadCreatures( void );
void		LoadPlaces( void );

//o------------------------------------------------------------------------------------------------o
// Misc Pre-Declarations
//o------------------------------------------------------------------------------------------------o
void		RestockNPC( CChar& i, bool stockAll );
void		ClearTrades( void );
void		SysBroadcast( const std::string& txt );
void		MoveBoat( UI08 dir, CBoatObj *boat );
bool		DecayItem( CItem& toDecay, const UI32 nextDecayItems, const UI32 nextDecayItemsInHouses );
void		CheckAI( CChar& mChar );
//o------------------------------------------------------------------------------------------------o
// Internal Pre-Declares
//o------------------------------------------------------------------------------------------------o
#if PLATFORM == WINDOWS
BOOL WINAPI exit_handler( DWORD dwCtrlType );
#else
void app_stopped(int sig);
#endif
auto EndMessage( SI32 x ) -> void;
auto InitClasses() -> void;
auto InitMultis() -> void;
auto DisplayBanner() -> void;
auto CheckConsoleKeyThread() -> void;
auto DoMessageLoop() -> void;
auto StartInitialize( CServerData &server_data ) -> void;
auto InitOperatingSystem() -> std::optional<std::string>;
auto AdjustInterval( std::chrono::milliseconds interval, std::chrono::milliseconds maxTimer ) -> std::chrono::milliseconds;

//o------------------------------------------------------------------------------------------------o
//|	Function	-	main()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Main UOX startup
//o------------------------------------------------------------------------------------------------o
auto main( SI32 argc, char *argv[] ) ->int
{
	UI32 tempSecs, tempMilli, tempTime;
	UI32 loopSecs, loopMilli;
	TIMERVAL uiNextCheckConn = 0;

	// We are going to do some fundmental checks, that if fail, we will bail out before
	// setting up
	auto configFile = std::string( "uox.ini" );
	if( argc > 1 )
	{
		configFile = argv[1];
	}
	
	auto status = InitOperatingSystem();
	if( status.has_value() )
	{
		std::cerr << status.value() << std::endl;
		return EXIT_FAILURE;
	}

	// Ok, we probably want the Console now
	Console.Initialize();

	Console.Start( oldstrutil::format( "%s v%s.%s (%s)", CVersionClass::GetProductName().c_str(), CVersionClass::GetVersion().c_str(), CVersionClass::GetBuild().c_str(), OS_STR ));
	Console.PrintSectionBegin();
	Console << "UOX Server start up!" << myendl << "Welcome to " << CVersionClass::GetProductName() << " v" << CVersionClass::GetVersion() << "." << CVersionClass::GetBuild() << " (" << OS_STR << ")" << myendl;
	Console.PrintSectionBegin();

	// We are going to load some of our basic data, if that goes ok, we then initialize classes, data, network
	// and the classes
	Console << "Processing INI Settings  ";
	if( !std::filesystem::exists( std::filesystem::path( configFile )))
	{
		Console.Error( configFile.empty() ? "Cannot find UOX3 ini file." : oldstrutil::format( "Cannot find UOX3 ini file: %s", configFile.c_str() ));
		return EXIT_FAILURE;
	}
	auto serverdata = CServerData();
	if( !serverdata.Load( configFile ))
	{
		Console.Error( configFile.empty() ? "Error loading UOX3 ini file." : oldstrutil::format( "Error loading UOX3 ini file: %s", configFile.c_str() ));
		return EXIT_FAILURE;
	}
	Console.PrintDone();

	// Start/Initalize classes, data, network
	StartInitialize( serverdata );

	// Main Loop
	Console.PrintSectionBegin();
	EVENT_TIMER( stopwatch, EVENT_TIMER_OFF );

	// Initiate APS - Adaptive Performance System
	// Tracks simulation cycles over time and adjusts how often NPC AI/Pathfinding is updated as
	// necessary to keep shard performance and responsiveness to player input at acceptable levels
	auto apsPerfThreshold = cwmWorldState->ServerData()->APSPerfThreshold(); // Performance threshold from ini, 0 disables APS feature

	[[maybe_unused]] int avgSimCycles = 0;
	UI16 apsMovingAvgOld = 0;
	const int maxSimCycleSamples = 10;  // Max number of samples for moving average
	std::vector<int> simCycleSamples;  // Store simulation cycle measurements for moving average

	// Fetch step value used by APS to gradually adjust delay in NPC AI/movement checking
	const std::chrono::milliseconds apsDelayStep = std::chrono::milliseconds( cwmWorldState->ServerData()->APSDelayStep() );

	// Fetch max cap for delay introduced into loop by APS for checking NPC AI/movement stuff
	const std::chrono::milliseconds apsDelayMaxCap = std::chrono::milliseconds( cwmWorldState->ServerData()->APSDelayMaxCap() );

	// Setup initial values for apsDelay
	std::chrono::milliseconds apsDelay = std::chrono::milliseconds( 0 );
	std::chrono::time_point<std::chrono::system_clock> adaptivePerfTimer = std::chrono::system_clock::now() + apsDelay;

	// Set the interval for APS evaluation and adjustment, and set initial timestamp for first evaluation
	const std::chrono::milliseconds evaluationInterval = std::chrono::milliseconds( cwmWorldState->ServerData()->APSInterval() );
	std::chrono::time_point<std::chrono::system_clock> nextEvaluationTime = std::chrono::system_clock::now() + evaluationInterval;

	// Core server loop
	while( cwmWorldState->GetKeepRun() )
	{
		std::this_thread::sleep_for( std::chrono::milliseconds( cwmWorldState->GetPlayersOnline() ? 5 : 90 ));
		if( cwmWorldState->ServerProfile()->LoopTimeCount() >= 1000 )
		{
			cwmWorldState->ServerProfile()->LoopTimeCount( 0 );
			cwmWorldState->ServerProfile()->LoopTime( 0 );
		}
		cwmWorldState->ServerProfile()->IncLoopTimeCount();
		
		StartMilliTimer( loopSecs, loopMilli );
		
		if( cwmWorldState->ServerProfile()->NetworkTimeCount() >= 1000 )
		{
			cwmWorldState->ServerProfile()->NetworkTimeCount( 0 );
			cwmWorldState->ServerProfile()->NetworkTime( 0 );
		}
		
		StartMilliTimer( tempSecs, tempMilli );
		EVENT_TIMER_RESET( stopwatch );
		
		if( uiNextCheckConn <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
		{
			// Cut lag on CheckConn by not doing it EVERY loop.
			Network->CheckConnections();
			uiNextCheckConn = BuildTimeValue( 1.0f );
		}
		Network->CheckMessages();
		EVENT_TIMER_NOW( stopwatch, Complete net checkmessages, EVENT_TIMER_KEEP );
		tempTime = CheckMilliTimer( tempSecs, tempMilli );
		cwmWorldState->ServerProfile()->IncNetworkTime( tempTime );
		cwmWorldState->ServerProfile()->IncNetworkTimeCount();
		
		if( cwmWorldState->ServerProfile()->TimerTimeCount() >= 1000 )
		{
			cwmWorldState->ServerProfile()->TimerTimeCount( 0 );
			cwmWorldState->ServerProfile()->TimerTime( 0 );
		}
		
		StartMilliTimer( tempSecs, tempMilli );
		
		cwmWorldState->CheckTimers();
		//stopwatch.output( "Delta for CheckTimers" );
		cwmWorldState->SetUICurrentTime( GetClock() );
		tempTime = CheckMilliTimer( tempSecs, tempMilli );
		cwmWorldState->ServerProfile()->IncTimerTime( tempTime );
		cwmWorldState->ServerProfile()->IncTimerTimeCount();
		
		if( cwmWorldState->ServerProfile()->AutoTimeCount() >= 1000 )
		{
			cwmWorldState->ServerProfile()->AutoTimeCount( 0 );
			cwmWorldState->ServerProfile()->AutoTime( 0 );
		}
		StartMilliTimer( tempSecs, tempMilli );
		
		if( !cwmWorldState->GetReloadingScripts() )
		{
			//auto stopauto = EventTimer();
			EVENT_TIMER( stopauto, EVENT_TIMER_OFF );

			std::chrono::time_point<std::chrono::system_clock> currentTime = std::chrono::system_clock::now();
			if( apsPerfThreshold == static_cast<UI16>( 0 ) || apsDelay == std::chrono::milliseconds(0) || currentTime >= adaptivePerfTimer )
			{
				// Check autotimers if the APS feature is disabled, if there's no delay, or if timer has expired
				cwmWorldState->CheckAutoTimers();

				// Set timer for next update, if apsDelay is higher than 0
				if( apsDelay > std::chrono::milliseconds( 0 ))
				{
					adaptivePerfTimer = std::chrono::system_clock::now() + apsDelay;
				}
			}

			EVENT_TIMER_NOW( stopauto, CheckAutoTimers only, EVENT_TIMER_CLEAR );
		}
		
		tempTime = CheckMilliTimer( tempSecs, tempMilli );
		cwmWorldState->ServerProfile()->IncAutoTime( tempTime );
		cwmWorldState->ServerProfile()->IncAutoTimeCount();
		StartMilliTimer( tempSecs, tempMilli );
		EVENT_TIMER_RESET( stopwatch );
		Network->ClearBuffers();
		EVENT_TIMER_NOW( stopwatch, Delta for ClearBuffers, EVENT_TIMER_CLEAR );
		tempTime = CheckMilliTimer( tempSecs, tempMilli );
		cwmWorldState->ServerProfile()->IncNetworkTime( tempTime );
		tempTime = CheckMilliTimer( loopSecs, loopMilli );
		cwmWorldState->ServerProfile()->IncLoopTime( tempTime );
		EVENT_TIMER_RESET( stopwatch );
		DoMessageLoop();
		EVENT_TIMER_NOW( stopwatch, Delta for DoMessageLoop, EVENT_TIMER_CLEAR );

		// Check if it's time for evaluation and adjustment
		std::chrono::time_point<std::chrono::system_clock> currentTime = std::chrono::system_clock::now();
		if( apsPerfThreshold > static_cast<UI16>( 0 ) && currentTime >= nextEvaluationTime )
		{
			// Get simulation cycles count
			auto simCycles = ( 1000.0 * ( 1.0 / static_cast<R32>( static_cast<R32>( cwmWorldState->ServerProfile()->LoopTime() ) / static_cast<R32>( cwmWorldState->ServerProfile()->LoopTimeCount() ))));

			// Store last X simCycle samples
			simCycleSamples.push_back( simCycles );

			// Limit the number of samples kept to X
			if( simCycleSamples.size() > maxSimCycleSamples )
			{
				simCycleSamples.erase( simCycleSamples.begin() );
			}

			int sum = std::accumulate( simCycleSamples.begin(), simCycleSamples.end(), 0 );
			UI16 apsMovingAvg = static_cast<UI16>( sum / simCycleSamples.size() );
			if( apsMovingAvg < apsPerfThreshold )
			{
				// Performance is below threshold...
				if( apsMovingAvg <= apsMovingAvgOld && apsDelay < apsDelayMaxCap )
				{
					// ... and dropping, or stable at low performance! DO SOMETHING...
					apsDelay = apsDelay + apsDelayStep;
#if defined( UOX_DEBUG_MODE )
					Console << "Performance below threshold! Increasing adaptive performance timer: " << apsDelay.count() << "ms" << "\n";
#endif
				}
				// If performance is below, but increasing, wait and see before reacting
			}
			else
			{
				// Performance exceeds threshold...
				if( apsDelay >= apsDelayStep )
				{
					// ... reduce timer for snappier NPC AI/movement/etc.
					apsDelay = apsDelay - apsDelayStep;
#if defined( UOX_DEBUG_MODE )
					Console << "Performance exceeds threshold. Decreasing adaptive performance timer: " << apsDelay.count() << "ms" << "\n";
#endif
				}
			}

			// Update previous moving average
			apsMovingAvgOld = apsMovingAvg;

			// Adjust the interval based on the timer value
			std::chrono::milliseconds adjustedInterval = AdjustInterval( evaluationInterval, apsDelay );

			// Update the next evaluation time
			nextEvaluationTime = currentTime + adjustedInterval;
		}
	}

	// Shutdown/Cleanup
	SysBroadcast( "The server is shutting down." );
	Console << "Closing sockets...";
	netpollthreadclose = true;
	///HERE
	Network->SockClose();
	Console.PrintDone();
	
#if PLATFORM == WINDOWS
	SetConsoleCtrlHandler( exit_handler, true );
#endif
	if( cwmWorldState->GetWorldSaveProgress() != SS_SAVING )
	{
		isWorldSaving = true;
		do
		{
			cwmWorldState->SaveNewWorld( true );
		} while( cwmWorldState->GetWorldSaveProgress() == SS_SAVING );
		isWorldSaving = false;
	}
	cwmWorldState->ServerData()->SaveIni();
#if PLATFORM == WINDOWS
	SetConsoleCtrlHandler( exit_handler, false );
#endif
	
	Console.Log( "Server Shutdown!\n=======================================================================\n" , "server.log" );
	
	conThreadCloseOk = true;	//	This will signal the console thread to close
	Shutdown( 0 );
	
	// Will never reach this, as Shutdown "exits"
	return EXIT_SUCCESS;
}

// Scaling function to adjust the interval based on the timer value
auto AdjustInterval( std::chrono::milliseconds interval, std::chrono::milliseconds maxTimer ) -> std::chrono::milliseconds
{
	double scaleFactor = static_cast<double>(maxTimer.count()) / interval.count();
	double adjustmentFactor = 0.25; // Adjust this factor to control the rate of adjustment
	long long adjustedCount = static_cast<long long>(interval.count() * (1.0 + scaleFactor * adjustmentFactor));
	return std::chrono::milliseconds(adjustedCount);
}

//o------------------------------------------------------------------------------------------------o
// Initialize the network
//o------------------------------------------------------------------------------------------------o
auto InitOperatingSystem() -> std::optional<std::string>
{
	// Startup Winsock2(windows) or signal handers (unix)
#if PLATFORM == WINDOWS
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD( 2, 2 );
	SI32 err = WSAStartup( wVersionRequested, &wsaData );
	if( err )
	{
		return "Winsock 2.2 not found on your system!"s;
	}
#else
	// Protection from server-shutdown during mid-worldsave
	signal( SIGINT, app_stopped );
	signal( SIGPIPE, SIG_IGN ); // This appears when we try to write to a broken network connection
	
#endif
	return {};
}

//o------------------------------------------------------------------------------------------------o
// Startup and Initialization
//o------------------------------------------------------------------------------------------------o
auto StartInitialize( CServerData &serverdata ) -> void
{
	saveOnShutdown = false;
	// Let's measure startup time
	auto startupStartTime = std::chrono::high_resolution_clock::now();

	cwmWorldState = &aWorld;
	cwmWorldState->SetServerData( serverdata );

	Console << "Initializing and creating class pointers... " << myendl;
	InitClasses();
	cwmWorldState->SetUICurrentTime( GetClock() );

	Console.PrintSectionBegin();

	cwmWorldState->ServerData()->LoadTime();

	Console << "Loading skill advancement      ";
	LoadSkills();
	Console.PrintDone();

	// Moved BulkStartup here, dunno why that function was there...
	Console << "Loading dictionaries...        " << myendl;
	Console.PrintBasedOnVal( Dictionary->LoadDictionaries( cwmWorldState->ServerData()->Directory( CSDDP_DICTIONARIES )) >= 0 );

	Console << "Loading teleport               ";
	LoadTeleportLocations();
	Console.PrintDone();

	Console << "Loading GoPlaces               ";
	LoadPlaces();
	Console.PrintDone();
	generator = std::mt19937( rd() ); // Standard mersenne_twister_engine seeded with rd()

	auto packetSection = JSMapping->GetSection( SCPT_PACKET );
	for( const auto &[id, ourScript] : packetSection->collection() )
	{
		if( ourScript )
		{
			ourScript->ScriptRegistration( "Packet" );
		}
	}

	Skills->Load();

	Console << "Loading Spawn Regions          ";
	LoadSpawnRegions();
	Console.PrintDone();

	Console << "Loading Regions                ";
	LoadRegions();
	Console.PrintDone();

	Magic->LoadScript();

	Console << "Loading Races                  ";
	Races->Load();
	Console.PrintDone();

	Console << "Loading Weather                ";
	Weather->Load();
	Weather->NewDay();
	Weather->NewHour();
	Console.PrintDone();

	Console << "Loading Commands               " << myendl;
	Commands->Load();
	Console.PrintDone();

	// Rework that...
	Console << "Loading World now              ";
	MapRegion->Load();

	Console << "Loading Guilds                 ";
	GuildSys->Load();
	Console.PrintDone();

	Console.PrintSectionBegin();
	Console << "Clearing all trades            ";
	ClearTrades();
	Console.PrintDone();
	InitMultis();

	cwmWorldState->SetStartTime( cwmWorldState->GetUICurrentTime() );

	cwmWorldState->SetEndTime( 0 );
	cwmWorldState->SetLClock( 0 );

	// no longer Que, because that's taken care of by PageVector
	Console << "Initializing Jail system       ";
	JailSys->ReadSetup();
	JailSys->ReadData();
	Console.PrintDone();

	Console << "Initializing Status system     ";
	HTMLTemplates->Load();
	Console.PrintDone();

	Console << "Loading custom titles          ";
	LoadCustomTitle();
	Console.PrintDone();

	Console << "Loading temporary Effects      ";
	Effects->LoadEffects();
	Console.PrintDone();

	Console << "Loading creatures              ";
	LoadCreatures();
	Console.PrintDone();

	Console << "Starting World Timers          ";
	cwmWorldState->SetTimer( tWORLD_LIGHTTIME, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_WEATHER ));
	cwmWorldState->SetTimer( tWORLD_NEXTNPCAI, BuildTimeValue( static_cast<R32>( cwmWorldState->ServerData()->CheckNpcAISpeed() )));
	cwmWorldState->SetTimer( tWORLD_NEXTFIELDEFFECT, BuildTimeValue( 0.5f ));
	cwmWorldState->SetTimer( tWORLD_SHOPRESTOCK, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_SHOPSPAWN ));
	cwmWorldState->SetTimer( tWORLD_PETOFFLINECHECK, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_PETOFFLINECHECK ));

	Console.PrintDone();

	DisplayBanner();

	Console << "Loading Accounts               ";
	Accounts->Load();
	Console.PrintDone();

	Console.Log( "-=Server Startup=-\n=======================================================================", "server.log" );

	Console << "Creating and Initializing Console Thread      ";

	cons = std::thread( &CheckConsoleKeyThread );

	Console.PrintDone();

	// Shows information about IPs and ports being listened on
	Console.TurnYellow();

	auto externalIP = cwmWorldState->ServerData()->ExternalIP();
	if( externalIP != "" && externalIP != "localhost" && externalIP != "127.0.0.1" )
	{
		Console << "UOX: listening for incoming connections on External/WAN IP: " << externalIP.c_str() << myendl;
	}

	auto deviceIPs = ip4list_t::available();
	for( auto &entry : deviceIPs.ips() )
	{
		switch( entry.type() )
		{
			case Ip4Addr_st::ip4type_t::lan:
				Console << "UOX: listening for incoming connections on LAN IP: " << entry.description() << myendl;
				break;
			case Ip4Addr_st::ip4type_t::local:
				Console << "UOX: listening for incoming connections on Local IP: " << entry.description() << myendl;
				break;
			case Ip4Addr_st::ip4type_t::wan:
				Console << "UOX: listening for incoming connections on WAN IP: " << entry.description() << myendl;
				break;
			default:
				Console << "UOX: listening for incoming connections on IP: " << entry.description() << myendl;
				break;
		}
	}
	Console.TurnNormal();

	// we've really finished loading here
	cwmWorldState->SetLoaded( true );

	// Get a second timestamp for startup time
	auto startupEndTime = std::chrono::high_resolution_clock::now();

	// Calculate startup time in milliseconds
	auto startupDuration = std::chrono::duration_cast<std::chrono::milliseconds>( startupEndTime - startupStartTime ).count();
	Console.TurnGreen();
	Console << "UOX: Startup Completed in " << static_cast<R32>( startupDuration ) / 1000 << " seconds." << myendl;
	Console.TurnNormal();
}

//o------------------------------------------------------------------------------------------------o
// Most things after this point, should be in different files, not in uox3.cpp.


//o------------------------------------------------------------------------------------------------o
// Signal and exit handers
//o------------------------------------------------------------------------------------------------o
#if PLATFORM == WINDOWS
//o------------------------------------------------------------------------------------------------o
//|	Function	-	exit_handler()
//|					app_stopped()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Prevent closing of console via CTRL+C/or CTRL+BREAK keys during worldsaves
//o------------------------------------------------------------------------------------------------o
BOOL WINAPI exit_handler( DWORD dwCtrlType )
{
	switch( dwCtrlType )
	{
		case CTRL_C_EVENT:
		case CTRL_BREAK_EVENT:
		case CTRL_CLOSE_EVENT:
			std::cout << std::endl << "World save in progress - closing UOX3 before it completes may result in corrupted save data!" << std::endl;
			// Shutdown of the application will only be halted for as long as the exit_handler is doing something,
			// so do some non-work while isWorldSaving is true to prevent shutdown during save
			while( isWorldSaving == true )
			{
				Sleep( 0 );
			}
			return true;
		default:
			return false;
	}
}
#else
//o------------------------------------------------------------------------------------------------o
// These first two, should be removed.  We should fix the error
//o------------------------------------------------------------------------------------------------o
auto illinst( SI32 x = 0 ) -> void
{
	SysBroadcast( "Fatal Server Error! Bailing out - Have a nice day!" );
	Console.Error( "Illegal Instruction Signal caught - attempting shutdown" );
	EndMessage( x );
}
auto aus( [[maybe_unused]] SI32 signal ) -> void
{
	Console.Error( "Server crash averted! Floating point exception caught." );
}
void app_stopped( [[maybe_unused]] int sig )
{
	// function called when signal is received.
	if( isWorldSaving == false )
	{
		cwmWorldState->SetKeepRun( false );
	}
}
#endif

//o------------------------------------------------------------------------------------------------o
// Spawn related
//o------------------------------------------------------------------------------------------------o

//o------------------------------------------------------------------------------------------------o
//|	Function	-	UnloadSpawnRegions()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Unload spawn regions on server shutdown or when reloading spawn regions
//o------------------------------------------------------------------------------------------------o
auto UnloadSpawnRegions() -> void
{
	for( auto &[regionnum, spawnregion] : cwmWorldState->spawnRegions )
	{
		if( spawnregion )
		{
			// Iterate over list of spawned characters and delete them if no player has tamed them/hired them
			std::vector<CChar *> toDelete;
			auto spawnedCharsList = spawnregion->GetSpawnedCharsList();
			for( const auto &cCheck : spawnedCharsList->collection() )
			{
				if( ValidateObject( cCheck ))
				{
					if( !ValidateObject( cCheck->GetOwnerObj() ))
					{
						toDelete.push_back( cCheck );
					}
				}
			}
			std::for_each( toDelete.begin(), toDelete.end(), []( CChar *entry )
			{
				entry->Delete();
			});
			// Iterate over list of spawned items and delete them if no player has picked them up
			std::vector<CItem *> toIDelete;
			auto spawnedItemsList = spawnregion->GetSpawnedItemsList();
			for( const auto &iCheck : spawnedItemsList->collection() )
			{
				if( ValidateObject( iCheck ))
				{
					if( iCheck->GetContSerial() != INVALIDSERIAL || !ValidateObject( iCheck->GetOwnerObj() ))
					{
						toIDelete.push_back( iCheck );
					}
				}
			}
			std::for_each( toIDelete.begin(), toIDelete.end(), []( CItem *entry )
			{
				entry->Delete();
			});

			delete spawnregion;
		}
	}
	cwmWorldState->spawnRegions.clear();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	UnloadRegions()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Unload town regions on server shutdown or when reloading town regions
//o------------------------------------------------------------------------------------------------o
auto UnloadRegions() -> void
{
	std::for_each( cwmWorldState->townRegions.begin(), cwmWorldState->townRegions.end(), []( const std::pair<UI16, CTownRegion *> &entry )
	{
		if( entry.second )
		{
			delete entry.second;
		}
	});
	cwmWorldState->townRegions.clear();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	DoMessageLoop()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Watch for messages thrown by UOX
//o------------------------------------------------------------------------------------------------o
auto DoMessageLoop() -> void
{
	// Grab all the data in the queue
	auto messages = messageLoop.BulkData();
	while( !messages.empty() )
	{
		auto tVal = messages.front();
		messages.pop();
		switch( tVal.actualMessage )
		{
			case MSG_SHUTDOWN: 	cwmWorldState->SetKeepRun( false ); break;
			case MSG_COUNT: 	break;
			case MSG_WORLDSAVE: cwmWorldState->SetOldTime( 0 ); break;
			case MSG_PRINT: 	Console << tVal.data << myendl; break;
			case MSG_RELOADJS:
				JSEngine->Reload();
				JSMapping->Reload();
				Console.PrintDone();
				Commands->Load();
				break;
			case MSG_CONSOLEBCAST:	SysBroadcast( tVal.data ); break;
			case MSG_PRINTDONE: 	Console.PrintDone(); break;
			case MSG_PRINTFAILED:	Console.PrintFailed(); break;
			case MSG_SECTIONBEGIN:	Console.PrintSectionBegin(); break;
			case MSG_RELOAD:
				if( !cwmWorldState->GetReloadingScripts() )
				{
					cwmWorldState->SetReloadingScripts( true );
					switch( tVal.data[0] )
					{
						case '0':	cwmWorldState->ServerData()->Load(); break;	// Reload INI file
						case '1':	Accounts->Load();	 break;	// Reload accounts
						case '2':	// Reload regions/TeleportLocations
							UnloadRegions(); 
							LoadRegions();
							LoadTeleportLocations(); 
							break;
						case '3':	UnloadSpawnRegions();	LoadSpawnRegions(); break;	// Reload spawn regions
						case '4':	Magic->LoadScript(); break;	// Reload spells
						case '5':	// Reload commands	
							JSMapping->Reload( SCPT_COMMAND );
							Commands->Load();	 
							break;
						case '6': // Reload definition files
							FileLookup->Reload();
							LoadCreatures();
							LoadCustomTitle();
							LoadSkills();
							LoadPlaces();
							Skills->Load(); break;
						case '7': // Reload JS
							JSEngine->Reload();
							JSMapping->Reload();
							Console.PrintDone();
							Commands->Load();
							Skills->Load(); break;
						case '8': // Reload HTML
							HTMLTemplates->Unload();
							HTMLTemplates->Load();	 break;
					}
					cwmWorldState->SetReloadingScripts( false );
				}
				break;
			case MSG_UNKNOWN:
			default: Console.Error( "Unknown message type" ); break;
		}
	}
}

//o------------------------------------------------------------------------------------------------o
// Threads
//o------------------------------------------------------------------------------------------------o

//o------------------------------------------------------------------------------------------------o
//|	Function	-	NetworkPollConnectionThread()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Watch for new connections
//o------------------------------------------------------------------------------------------------o
auto NetworkPollConnectionThread() -> void
{
	messageLoop << "Thread: NetworkPollConnection has started";
	netpollthreadclose = false;
	while( !netpollthreadclose )
	{
		Network->CheckConnections();
		Network->CheckLoginMessage();
		std::this_thread::sleep_for( std::chrono::milliseconds( 20 ));
	}
	messageLoop << "Thread: NetworkPollConnection has Closed";
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CheckConsoleKeyThread()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Listen for key inputs in server console
//o------------------------------------------------------------------------------------------------o
auto CheckConsoleKeyThread() -> void
{
	messageLoop << "Thread: CheckConsoleThread has started";
	Console.Registration();
	conThreadCloseOk = false;
	while( !conThreadCloseOk )
	{
		Console.Poll();
		std::this_thread::sleep_for( std::chrono::milliseconds( 500 ));
	}
	messageLoop << "Thread: CheckConsoleKeyThread Closed";
}
//o------------------------------------------------------------------------------------------------o
// Misc other functions
//o------------------------------------------------------------------------------------------------o

//o------------------------------------------------------------------------------------------------o
//|	Function	-	IsOnline()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if the socket owning character c is still connected
//o------------------------------------------------------------------------------------------------o
auto IsOnline( CChar& mChar ) -> bool
{
	auto rValue = false;
	if( !mChar.IsNpc() )
	{
		CAccountBlock_st& actbTemp = mChar.GetAccount();
		if( actbTemp.wAccountIndex != AB_INVALID_ID )
		{
			if( actbTemp.dwInGame == mChar.GetSerial() )
			{
				rValue = true;
			}
		}
		if( !rValue )
		{
			for( auto &tSock : Network->connClients )
			{
				if( tSock->CurrcharObj() == &mChar )
				{
					rValue = true;
					break;
				}
			}
		}
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	UpdateStats()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Updates object's stats
//o------------------------------------------------------------------------------------------------o
auto UpdateStats( CBaseObject *mObj, UI08 x ) -> void
{
	for( auto &tSock : FindNearbyPlayers( mObj ))
	{
		if( tSock->LoginComplete() )
		{
			// Normalize stats if we're updating our stats for other players
			auto normalizeStats = true;
			if( tSock->CurrcharObj()->GetSerial() == mObj->GetSerial() )
			{
				tSock->StatWindow( mObj );
				normalizeStats = false;
			}
			
			// Prepare the stat update packet
			CPUpdateStat toSend(( *mObj ), x, normalizeStats );
			
			// Send the stat update packet
			tSock->Send( &toSend );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CollectGarbage()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Deletes objects in the Deletion Queue
//o------------------------------------------------------------------------------------------------o
auto CollectGarbage() -> void
{
	Console << "Performing Garbage Collection...";
	auto objectsDeleted = UI32( 0 );
	std::for_each( cwmWorldState->deletionQueue.begin(), cwmWorldState->deletionQueue.end(), [&objectsDeleted]( std::pair<CBaseObject*, UI32> entry )
	{
		if( entry.first )
		{
			if( entry.first->IsFree() && entry.first->IsDeleted() )
			{
				Console.Warning( "Invalid object found in Deletion Queue" );
			}
			else
			{
				ObjectFactory::GetSingleton().DestroyObject( entry.first );
				++objectsDeleted;
			}
		}
	});

	cwmWorldState->deletionQueue.clear();

	Console << " Removed " << objectsDeleted << " objects";

	JSEngine->CollectGarbage();
	Console.PrintDone();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	MountCreature()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Mount a ridable creature
//|
//|	Changes		-	09/22/2002	-	Unhide players when mounting horses etc.
//o------------------------------------------------------------------------------------------------o
auto MountCreature( CSocket *sockPtr, CChar *s, CChar *x ) -> void
{
	if( s->IsOnHorse() )
		return;

	//No mounting horses for gargoyles!
	if( s->GetId() == 0x029A || s->GetId() == 0x029B )
	{
		sockPtr->SysMessage( 1798 ); // You cannot mount.
		return;
	}

	if( !ObjInRange( s, x, DIST_NEXTTILE ))
		return;

	if( x->GetOwnerObj() == s || Npcs->CheckPetFriend( s, x ) || s->IsGM() )
	{
		if( !cwmWorldState->ServerData()->CharHideWhileMounted() )
		{
			s->ExposeToView();
		}

		s->SetOnHorse( true );
		auto c = Items->CreateItem( nullptr, s, 0x0915, 1, x->GetSkin(), OT_ITEM );

		auto xName = GetNpcDictName( x, sockPtr, NRS_SYSTEM );
		c->SetName( xName );
		c->SetDecayable( false );
		c->SetLayer( IL_MOUNT );

		if( cwmWorldState->creatures[x->GetId()].MountId() != 0 )
		{
			c->SetId( cwmWorldState->creatures[x->GetId()].MountId() );
		}
		else
		{
			c->SetId( 0x3E00 );
		}
		
		if( !c->SetCont( s ))
		{
			s->SetOnHorse( false );	// let's get off our horse again
			c->Delete();
			return;
		}
		else
		{
			for( auto &tSock : FindNearbyPlayers( s ))
			{
				s->SendWornItems( tSock );
			}
			
			if( x->GetTarg() ) // zero out target, under all circumstances
			{
				x->SetTarg( nullptr );
				if( x->IsAtWar() )
				{
					x->ToggleCombat();
				}
			}
			if( ValidateObject( x->GetAttacker() ))
			{
				x->GetAttacker()->SetTarg( nullptr );
			}
			x->SetFrozen( true );
			x->SetMounted( true );
			x->SetInvulnerable( true );
			x->SetLocation( 7000, 7000, 0 );
			
			c->SetTempVar( CITV_MOREX, x->GetSerial() );
			if( x->GetTimer( tNPC_SUMMONTIME ) != 0 )
			{
				c->SetDecayTime( x->GetTimer( tNPC_SUMMONTIME ));
			}
		}
	}
	else
	{
		sockPtr->SysMessage( 1214 ); // You don't own that creature.
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	DismountCreature()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Dismount a ridable creature
//o------------------------------------------------------------------------------------------------o
auto DismountCreature( CChar *s ) -> void
{
	if( ValidateObject( s ))
	{
		auto ci = s->GetItemAtLayer( IL_MOUNT );
		if( ValidateObject( ci ) && !ci->IsFree() )
		{
			s->SetOnHorse( false );
			auto tMount = CalcCharObjFromSer( ci->GetTempVar( CITV_MOREX ));
			if( ValidateObject( tMount ))
			{
				tMount->SetLocation( s );
				tMount->SetFrozen( false );
				tMount->SetMounted( false );
				tMount->SetInvulnerable( false );
				if( ci->GetDecayTime() != 0 )
				{
					tMount->SetTimer( tNPC_SUMMONTIME, ci->GetDecayTime() );
				}
				tMount->SetDir( s->GetDir() );
				tMount->SetVisible( VT_VISIBLE );
			}
			ci->Delete();
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	EndMessage()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Global message players with shutdown message
//o------------------------------------------------------------------------------------------------o
auto EndMessage( [[maybe_unused]] SI32 x ) -> void
{
	x = 0; // Really, then why take a parameter?
	const UI32 iGetClock = cwmWorldState->GetUICurrentTime();
	if( cwmWorldState->GetEndTime() < iGetClock )
	{
		cwmWorldState->SetEndTime( iGetClock );
	}
	SysBroadcast( oldstrutil::format( Dictionary->GetEntry( 1209 ), (( cwmWorldState->GetEndTime() - iGetClock ) / 1000 ) / 60 )); // Server going down in %i minutes!
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CallGuards()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Used when a character calls "Guards" Will look for a criminal
//|					first checking for anyone attacking him. If no one is attacking
//|					him it will look for any people nearby who are criminal or
//|					murderers
//o------------------------------------------------------------------------------------------------o
auto CallGuards( CChar *mChar ) -> void
{
	if( ValidateObject( mChar ) && mChar->GetRegion()->IsGuarded() && cwmWorldState->ServerData()->GuardsStatus() )
	{
		auto attacker = mChar->GetAttacker();
		if( ValidateObject( attacker ))
		{
			if( !attacker->IsDead() && ( attacker->IsCriminal() || attacker->IsMurderer() ))
			{
				// Can only be called on criminals for the first 10 seconds of receiving the criminal flag
				if( !attacker->IsMurderer() && attacker->IsCriminal() && mChar->GetTimer( tCHAR_CRIMFLAG ) - cwmWorldState->GetUICurrentTime() <= 10 )
				{
					// Too late!
					return;
				}

				if( CharInRange( mChar, attacker ))
				{
					Combat->SpawnGuard( mChar, attacker, attacker->GetX(), attacker->GetY(), attacker->GetZ() );
					return;
				}
			}
		}
		
		auto toCheck = MapRegion->GetMapRegion( mChar );
		if( toCheck )
		{
			auto regChars = toCheck->GetCharList();
			for( const auto &tempChar : regChars->collection() )
			{
				if( ValidateObject( tempChar ))
				{
					if( !tempChar->IsDead() && ( tempChar->IsCriminal() || tempChar->IsMurderer() ))
					{
						// Can only be called on criminals for the first 10 seconds of receiving the criminal flag
						if( !tempChar->IsMurderer() && tempChar->IsCriminal() && mChar->GetTimer( tCHAR_CRIMFLAG ) - cwmWorldState->GetUICurrentTime() <= 10 )
						{
							// Too late!
							return;
						}

						if( CharInRange( tempChar, mChar ))
						{
							Combat->SpawnGuard( mChar, tempChar, tempChar->GetX(), tempChar->GetY(), tempChar->GetZ() );
							break;
						}
					}
				}
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CallGuards()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Used when a character calls guards on another character, will
//|					ensure that character is not dead and is either a criminal or
//|					murderer, and that he is in visual range of the victim, will
//|					then spawn a guard to take care of the criminal.
//o------------------------------------------------------------------------------------------------o
auto CallGuards( CChar *mChar, CChar *targChar ) -> void
{	
	if( ValidateObject( mChar ) && ValidateObject( targChar ))
	{
		if( mChar->GetRegion()->IsGuarded()  && cwmWorldState->ServerData()->GuardsStatus() )
		{
			if( !targChar->IsDead() && ( targChar->IsCriminal() || targChar->IsMurderer() ))
			{
				// Can only be called on criminals for the first 10 seconds of receiving the criminal flag
				if( !targChar->IsMurderer() && targChar->IsCriminal() && mChar->GetTimer( tCHAR_CRIMFLAG ) - cwmWorldState->GetUICurrentTime() <= 10 )
				{
					// Too late!
					return;
				}

				if( CharInRange( mChar, targChar ))
				{
					Combat->SpawnGuard( mChar, targChar, targChar->GetX(), targChar->GetY(), targChar->GetZ() );
				}
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	GenericCheck()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check characters status.  Returns true if character was killed
//o------------------------------------------------------------------------------------------------o
auto GenericCheck( CSocket *mSock, CChar& mChar, bool checkFieldEffects, bool doWeather ) -> bool
{
	UI16 c;
	if( !mChar.IsDead() )
	{
		const auto maxHP = mChar.GetMaxHP();
		const auto maxStam = mChar.GetMaxStam();
		const auto maxMana = mChar.GetMaxMana();

		if( mChar.GetHP() > maxHP )
		{
			mChar.SetHP( maxHP );
		}
		if( mChar.GetStamina() > maxStam )
		{
			mChar.SetStamina( maxStam );
		}
		if( mChar.GetMana() > maxMana )
		{
			mChar.SetMana( maxMana );
		}

		if( mChar.GetRegen( 0 ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
		{
			if( mChar.GetHP() < maxHP )
			{
				if( !cwmWorldState->ServerData()->HungerSystemEnabled() || ( mChar.GetHunger() > 0)
				   || ( !Races->DoesHunger( mChar.GetRace() ) && (( cwmWorldState->ServerData()->SystemTimer( tSERVER_HUNGERRATE ) == 0) || mChar.IsNpc() )))
				{
					for( auto c = 0; c <= maxHP; ++c )
					{
						if( mChar.GetHP() <= maxHP && ( mChar.GetRegen( 0 ) + ( c * cwmWorldState->ServerData()->SystemTimer( tSERVER_HITPOINTREGEN ) * 1000 )) <= cwmWorldState->GetUICurrentTime() )
						{
							auto healingSkill = mChar.GetSkill( HEALING );
							UI08 hpIncrement = 0;
							if( healingSkill < 500 )
							{
								hpIncrement = 1;
							}
							else if( healingSkill < 800 )
							{
								hpIncrement = 2;
							}
							else
							{
								hpIncrement = 3;
							}

							SI16 totalRegenBonus = std::min( mChar.GetHealthRegen(), cwmWorldState->ServerData()->HealthRegenCap() );
								
							mChar.IncHP( static_cast<SI16>( hpIncrement ) + totalRegenBonus ); // Increment character's HP with the total regeneration bonus
							if( mChar.GetHP() >= maxHP )
							{
								mChar.SetHP( maxHP );
								break;
							}
						}
						else // either we're all healed up, or all time periods have passed
						{
							break;
						}
					}
				}
			}
			mChar.SetRegen( cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_HITPOINTREGEN ), 0 );
		}
		if( mChar.GetRegen( 1 ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
		{
			auto mStamina = mChar.GetStamina();
			if( mStamina < maxStam )
			{
				// Continue with stamina regen if  character is not yet fully parched, or if character is parched but has less than 25% stamina, or if char belongs to race that does not thirst
				if( !cwmWorldState->ServerData()->ThirstSystemEnabled() || ( mChar.GetThirst() > 0 )|| (( mChar.GetThirst() == 0) && ( mStamina < static_cast<SI16>( maxStam * 0.25 )))
				   || ( !Races->DoesThirst( mChar.GetRace() ) && ( cwmWorldState->ServerData()->SystemTimer( tSERVER_THIRSTRATE ) == 0 || mChar.IsNpc() )))
				{
					for( auto c = 0; c <= maxStam; ++c )
					{
						if(( mChar.GetRegen( 1 ) + ( c * cwmWorldState->ServerData()->SystemTimer( tSERVER_STAMINAREGEN ) * 1000 )) <= cwmWorldState->GetUICurrentTime() && mChar.GetStamina() <= maxStam )
						{
							R64 focusBonus = 0;
							if( cwmWorldState->ServerData()->ExpansionCoreShardEra() >= ER_AOS )
							{
								Skills->CheckSkill(( &mChar ), FOCUS, 0, 1000 ); // Check FOCUS for skill gain AOS
								focusBonus = ( 0.1 * mChar.GetSkill( FOCUS ) / 10 );	// Bonus for focus
							}

							SI16 totalRegenBonus = std::min( mChar.GetStaminaRegen(), cwmWorldState->ServerData()->StaminaRegenCap() );

							mChar.IncStamina( 1 + focusBonus + totalRegenBonus );
							
							if( mChar.GetStamina() >= maxStam )
							{
								mChar.SetStamina( maxStam );
								break;
							}
						}
						else
						{
							break;
						}
					}
				}
			}
			mChar.SetRegen( cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_STAMINAREGEN ), 1 );
		}

		// MANA REGENERATION:Rewrite of passive and active meditation code
		if( mChar.GetRegen( 2 ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
		{
			if( mChar.GetMana() < maxMana )
			{
				for( c = 0; c < maxMana + 1; ++c )
				{
					if( mChar.GetRegen( 2 ) + ( c * cwmWorldState->ServerData()->SystemTimer( tSERVER_MANAREGEN ) * 1000 ) <= cwmWorldState->GetUICurrentTime() && mChar.GetMana() <= maxMana )
					{
						R64 focusBonus = 0;
						if( cwmWorldState->ServerData()->ExpansionCoreShardEra() >= ER_AOS )
						{
							Skills->CheckSkill(( &mChar ), FOCUS, 0, 1000 ); // Check FOCUS for skill gain AOS
							focusBonus = mChar.GetSkill( FOCUS ) / 200; // Bonus for focus

							// Bonus from Meditation
							R64 meditationBonus = ( mChar.GetSkill( MEDITATION ) * 0.3 + mChar.GetIntelligence() / 400 ) * ( cwmWorldState->ServerData()->ArmorAffectManaRegen() && ( mChar.IsMeditating() || !cwmWorldState->ServerData()->ArmorAffectManaRegen() ) ? ( mChar.IsMeditating() ? 1.1 : 1.0 ) : 0.0 );

							// Variables to accumulate mana over the timer interval
							R64 totalManaRegenerated = 0.0;

							// Calculation of base mana regeneration per second
							R64 manaRegPerSecond = 0.2 + focusBonus + meditationBonus;

							// If meditating, apply additional bonuses
							if( mChar.IsMeditating() )
							{
								R64 itemBonus = sqrt( mChar.GetManaRegen() ) - 1;
								itemBonus = std::min( itemBonus, 5.5 ); // Cap item bonus at 5.5
								R64 additionalBonus = (( mChar.GetSkill( MEDITATION ) / 2 + mChar.GetSkill( FOCUS ) / 4 ) * 1 / 90 * 0.65 + 2.35 );
							    manaRegPerSecond += ( itemBonus + 0.1 * sqrt( mChar.GetManaRegen() )) * additionalBonus;
							}

							// Calculate the total mana regeneration from equipment
							R64 totalManaRegenFromEquipment = std::min( sqrt( mChar.GetManaRegen()), 30.0 ); // Cap total mana regeneration from equipment at 30

							R64 timerIntervalInSeconds = static_cast<R64>( cwmWorldState->ServerData()->SystemTimer( tSERVER_MANAREGEN )) / 1000.0;

							//auto timerInterval = cwmWorldState->ServerData()->SystemTimer(tSERVER_MANAREGEN);
							totalManaRegenerated += manaRegPerSecond * timerIntervalInSeconds + totalManaRegenFromEquipment;

							// Increment mana by the total regenerated over the timer interval
							mChar.IncMana( 1 + totalManaRegenerated ); // No need to divide by timerIntervalInSeconds

						}
						else
						{
							SI16 totalRegenBonus = std::min( mChar.GetManaRegen(), cwmWorldState->ServerData()->ManaRegenCap() );
							mChar.IncMana( 1 + focusBonus + totalRegenBonus );	// Gain a mana point
						}

						Skills->CheckSkill(( &mChar ), MEDITATION, 0, 1000 ); // Check Meditation for skill gain ala OSI
						if( mChar.GetMana() == maxMana )
						{
							if( mChar.IsMeditating() )
							{
								if( mSock )
								{
									mSock->SysMessage( 969 ); // You are at peace.
								}
								mChar.SetMeditating( false );
							}
							break;
						}
					}
				}
			}
			const R32 MeditationBonus = ( .00075f * mChar.GetSkill( MEDITATION ));	// Bonus for Meditation
			SI32 NextManaRegen = static_cast<SI32>( cwmWorldState->ServerData()->SystemTimer( tSERVER_MANAREGEN ) * ( 1 - MeditationBonus ) * 1000 );
			if( cwmWorldState->ServerData()->ArmorAffectManaRegen() ) // If armor effects mana regeneration...
			{
				R32 ArmorPenalty = Combat->CalcDef(( &mChar ), 0, false );	// Penalty taken due to high def
				if( ArmorPenalty > 100 ) // For def higher then 100, penalty is the same...just in case
				{
					ArmorPenalty = 100;
				}
				ArmorPenalty = 1 + ( ArmorPenalty / 25 );
				NextManaRegen = static_cast<SI32>( NextManaRegen * ArmorPenalty );
			}
			if( mChar.IsMeditating() ) // If player is meditation...
			{
				mChar.SetRegen(( cwmWorldState->GetUICurrentTime() + ( NextManaRegen / 2 )), 2 );
			}
			else
			{
				mChar.SetRegen(( cwmWorldState->GetUICurrentTime() + NextManaRegen ), 2 );
			}
		}
	}

	if( mChar.GetVisible() == VT_INVISIBLE && ( mChar.GetTimer( tCHAR_INVIS ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() ))
	{
		mChar.ExposeToView();
	}

	// Take NPC out of EvadeState
	if( mChar.IsNpc() && mChar.IsEvading() && mChar.GetTimer( tNPC_EVADETIME ) <= cwmWorldState->GetUICurrentTime() )
	{
		mChar.SetEvadeState( false );
#if defined( UOX_DEBUG_MODE ) && defined( DEBUG_COMBAT )
		std::string mCharName = GetNpcDictName( &mChar, nullptr, NRS_SYSTEM );
		Console.Print( oldstrutil::format( "DEBUG: EvadeTimer ended for NPC (%s, 0x%X, at %i, %i, %i, %i).\n", mCharName.c_str(), mChar.GetSerial(), mChar.GetX(), mChar.GetY(), mChar.GetZ(), mChar.WorldNumber() ));
#endif
	}

	if( !mChar.IsDead() )
	{
		// Hunger/Thirst Code
		mChar.DoHunger( mSock );
		mChar.DoThirst( mSock );

		// Loyalty update for pets
		mChar.DoLoyaltyUpdate();

		if( !mChar.IsInvulnerable() && mChar.GetPoisoned() > 0 )
		{
			if( mChar.GetTimer( tCHAR_POISONTIME ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
			{
				if( mChar.GetTimer( tCHAR_POISONWEAROFF ) > cwmWorldState->GetUICurrentTime() )
				{
					std::string mCharName = GetNpcDictName( &mChar, nullptr, NRS_SPEECH );

					switch( mChar.GetPoisoned() )
					{
						case 1: // Lesser Poison
						{
							mChar.SetTimer( tCHAR_POISONTIME, BuildTimeValue( 2 ));
							if( mChar.GetTimer( tCHAR_POISONTEXT ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
							{
								mChar.SetTimer( tCHAR_POISONTEXT, BuildTimeValue( 6 ));
								mChar.TextMessage( nullptr, 1240, EMOTE, 1, mCharName.c_str() ); // * %s looks a bit nauseous *
							}
							SI16 poisonDmgPercent = RandomNum( 3, 6 ); // 3% to 6% of current health per tick
							SI16 poisonDmg = static_cast<SI16>(( mChar.GetHP() * poisonDmgPercent ) / 100 );
							[[maybe_unused]] bool retVal = mChar.Damage( std::max( static_cast<SI16>( 3 ), poisonDmg ), POISON ); // Minimum 3 damage per tick
							break;
						}
						case 2: // Normal Poison
						{
							mChar.SetTimer( tCHAR_POISONTIME, BuildTimeValue( 3 ));
							if( mChar.GetTimer( tCHAR_POISONTEXT ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
							{
								mChar.SetTimer( tCHAR_POISONTEXT, BuildTimeValue( 10 ));
								mChar.TextMessage( nullptr, 1241, EMOTE, 1, mCharName.c_str() ); // * %s looks disoriented and nauseous! *
							}
							SI16 poisonDmgPercent = RandomNum( 4, 8 ); // 4% to 8% of current health per tick
							SI16 poisonDmg = static_cast<SI16>(( mChar.GetHP() * poisonDmgPercent ) / 100 );
							[[maybe_unused]] bool retVal = mChar.Damage( std::max( static_cast<SI16>( 5 ), poisonDmg ), POISON ); // Minimum 5 damage per tick
							break;
						}
						case 3: // Greater Poison
						{
							mChar.SetTimer( tCHAR_POISONTIME, BuildTimeValue( 4 ));
							if( mChar.GetTimer( tCHAR_POISONTEXT ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
							{
								mChar.SetTimer( tCHAR_POISONTEXT, BuildTimeValue( 10 ));
								mChar.TextMessage( nullptr, 1242, EMOTE, 1, mCharName.c_str() ); // * %s is in severe pain! *
							}
							SI16 poisonDmgPercent = RandomNum( 8, 12 ); // 8% to 12% of current health per tick
							SI16 poisonDmg = static_cast<SI16>(( mChar.GetHP() * poisonDmgPercent ) / 100 );
							[[maybe_unused]] bool retVal = mChar.Damage( std::max( static_cast<SI16>( 8 ), poisonDmg ), POISON ); // Minimum 8 damage per tick
							break;
						}
						case 4: // Deadly Poison
						{
							mChar.SetTimer( tCHAR_POISONTIME, BuildTimeValue( 5 ));
							if( mChar.GetTimer( tCHAR_POISONTEXT ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
							{
								mChar.SetTimer( tCHAR_POISONTEXT, BuildTimeValue( 10 ));
								mChar.TextMessage( nullptr, 1243, EMOTE, 1, mCharName.c_str() ); // * %s looks extremely weak and is wrecked in pain! *
							}
							SI16 poisonDmgPercent = RandomNum( 12, 25 ); // 12% to 25% of current health per tick
							SI16 poisonDmg = static_cast<SI16>(( mChar.GetHP() * poisonDmgPercent ) / 100 );
							[[maybe_unused]] bool retVal = mChar.Damage( std::max( static_cast<SI16>( 14 ), poisonDmg ), POISON ); // Minimum 14 damage per tick
							break;
						}
						case 5: // Lethal Poison - Used by monsters only
						{
							mChar.SetTimer( tCHAR_POISONTIME, BuildTimeValue( 5 ));
							if( mChar.GetTimer( tCHAR_POISONTEXT ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
							{
								mChar.SetTimer( tCHAR_POISONTEXT, BuildTimeValue( 10 ));
								mChar.TextMessage( nullptr, 1243, EMOTE, 1, mCharName.c_str() ); // * %s looks extremely weak and is wrecked in pain! *
							}
							SI16 poisonDmgPercent = RandomNum( 25, 50 ); // 25% to 50% of current health per tick
							SI16 poisonDmg = static_cast<SI16>(( mChar.GetHP() * poisonDmgPercent ) / 100 );
							[[maybe_unused]] bool retVal = mChar.Damage( std::max( static_cast<SI16>( 17 ), poisonDmg ), POISON ); // Minimum 14 damage per tick
							break;
						}
						default:
							Console.Error( " Fallout of switch statement without default. uox3.cpp, GenericCheck(), mChar.GetPoisoned() not within valid range." );
							mChar.SetPoisoned( 0 );
							break;
					}
					if( mChar.GetHP() < 1 && !mChar.IsDead() )
					{
						std::vector<UI16> scriptTriggers = mChar.GetScriptTriggers();
						for( auto &i : scriptTriggers )
						{
							cScript *toExecute = JSMapping->GetScript( i );
							if( toExecute )
							{
								SI08 retStatus = toExecute->OnDeathBlow( &mChar, mChar.GetAttacker() );

								// -1 == script doesn't exist, or returned -1
								// 0 == script returned false, 0, or nothing - don't execute hard code
								// 1 == script returned true or 1
								if( retStatus == 0 )
									return false;
							}
						}

						HandleDeath(( &mChar ), nullptr );
						if( mSock )
						{
							mSock->SysMessage( 1244 ); // The poison has killed you!
						}
					}
				}
			}
		}

		if( mChar.GetTimer( tCHAR_POISONWEAROFF ) <= cwmWorldState->GetUICurrentTime() )
		{
			if( mChar.GetPoisoned() > 0 )
			{
				mChar.SetPoisoned( 0 );
				if( mSock != nullptr )
				{
					mSock->SysMessage( 1245 ); // The poison has worn off.
				}
			}
		}
	}

	if( !mChar.GetCanAttack() && mChar.GetTimer( tCHAR_PEACETIMER ) <= cwmWorldState->GetUICurrentTime() )
	{
		mChar.SetCanAttack( true );
		if( mSock != nullptr )
		{
			mSock->SysMessage( 1779 ); // You are no longer affected by peace!
		}
	}

	// Perform maintenance on NPC's list of ignored targets in combat
	if( mChar.IsNpc() )
	{
		mChar.CombatIgnoreMaintenance();
	}

	// Perform maintenance on character's aggressor flags to clear out expired entries from the list
	mChar.AggressorFlagMaintenance();

	// Periodically reset permagrey flags if global timer is above 0, otherwise... they're permanent :P
	if( cwmWorldState->ServerData()->SystemTimer( tSERVER_PERMAGREYFLAG ) > 0 )
	{
		mChar.PermaGreyFlagMaintenance();
	}

	if( mChar.IsCriminal() && mChar.GetTimer( tCHAR_CRIMFLAG ) && ( mChar.GetTimer( tCHAR_CRIMFLAG ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() ))
	{
		if( mSock != nullptr )
		{
			mSock->SysMessage( 1238 ); // You are no longer a criminal.
		}
		mChar.SetTimer( tCHAR_CRIMFLAG, 0 );
		UpdateFlag( &mChar );
	}
	if( mChar.HasStolen() && mChar.GetTimer( tCHAR_STEALFLAG ) && ( mChar.GetTimer( tCHAR_STEALFLAG ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() ) )
	{
		mChar.SetTimer( tCHAR_STEALFLAG, 0 );
		mChar.HasStolen( false );
		UpdateFlag( &mChar );
	}
	if( mChar.GetKills() && ( mChar.GetTimer( tCHAR_MURDERRATE ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() ))
	{
		mChar.SetKills( static_cast<SI16>( mChar.GetKills() - 1 ));

		if( mChar.GetKills() )
		{
			mChar.SetTimer( tCHAR_MURDERRATE, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_MURDERDECAY ));
		}
		else
		{
			mChar.SetTimer( tCHAR_MURDERRATE, 0 );
		}
		if( mSock != nullptr && mChar.GetKills() == cwmWorldState->ServerData()->RepMaxKills() )
		{
			mSock->SysMessage( 1239 ); // You are no longer a murderer.
		}
		UpdateFlag( &mChar );
	}

	if( !mChar.IsDead() )
	{
		if( doWeather )
		{
			const UI08 curLevel = cwmWorldState->ServerData()->WorldLightCurrentLevel();
			LIGHTLEVEL toShow;
			if( Races->VisLevel( mChar.GetRace() ) > curLevel )
			{
				toShow = 0;
			}
			else
			{
				toShow = static_cast<UI08>( curLevel - Races->VisLevel( mChar.GetRace() ));
			}
			if( mChar.IsNpc() )
			{
				DoLight( &mChar, toShow );
			}
			else
			{
				DoLight( mSock, toShow );
			}
		}

		Weather->DoLightEffect( mSock, mChar );
		Weather->doWeatherEffect( mSock, mChar, RAIN );
		Weather->doWeatherEffect( mSock, mChar, SNOW );
		Weather->doWeatherEffect( mSock, mChar, HEAT );
		Weather->doWeatherEffect( mSock, mChar, COLD );
		Weather->doWeatherEffect( mSock, mChar, STORM );

		if( checkFieldEffects )
		{
			Magic->CheckFieldEffects( mChar );
		}

		mChar.UpdateDamageTrack();
	}

	if( mChar.IsDead() )
	{
		return true;
	}
	else if( mChar.GetHP() <= 0 )
	{
		std::vector<UI16> scriptTriggers = mChar.GetScriptTriggers();
		for( auto i : scriptTriggers )
		{
			auto toExecute = JSMapping->GetScript( i );
			if( toExecute )
			{
				auto retStatus = toExecute->OnDeathBlow( &mChar, mChar.GetAttacker() );

				// -1 == script doesn't exist, or returned -1
				// 0 == script returned false, 0, or nothing - don't execute hard code
				// 1 == script returned true or 1
				if( retStatus == 0 )
				{
					return false;
				}
			}
		}

		HandleDeath(( &mChar ), nullptr );
		return true;
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CheckPC()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check a PC's status
//o------------------------------------------------------------------------------------------------o
auto CheckPC( CSocket *mSock, CChar& mChar ) -> void
{
	Combat->CombatLoop( mSock, mChar );

	if( mChar.GetSquelched() == 2 )
	{
		if( mSock->GetTimer( tPC_MUTETIME ) != 0 && ( mSock->GetTimer( tPC_MUTETIME ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() ))
		{
			mChar.SetSquelched( 0 );
			mSock->SetTimer( tPC_MUTETIME, 0 );
			mSock->SysMessage( 1237 ); // You are no longer squelched!
		}
	}

	if( mChar.IsCasting() && !mChar.IsJSCasting() && mChar.GetSpellCast() != -1 )
	{
		// Casting a spell
		auto spellNum = mChar.GetSpellCast();
		mChar.SetNextAct( mChar.GetNextAct() - 1 );
		if( mChar.GetTimer( tCHAR_SPELLTIME ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() ) // Spell is complete target it.
		{
			// Set the recovery time before another spell can be cast
			mChar.SetTimer( tCHAR_SPELLRECOVERYTIME, BuildTimeValue( static_cast<R32>( Magic->spells[spellNum].RecoveryDelay() )));

			if( Magic->spells[spellNum].RequireTarget() )
			{
				mChar.SetCasting( false );
				mChar.SetFrozen( false );
				mChar.Dirty( UT_UPDATE );
				UI08 cursorType = 0;
				if( Magic->spells[spellNum].AggressiveSpell() )
				{
					cursorType = 1;
				}
				else if(( spellNum == 4 ) || ( spellNum == 6 ) || ( spellNum == 7 ) || ( spellNum >= 9 && spellNum <= 11 ) || ( spellNum >= 15 && spellNum <= 17 ) || ( spellNum == 25 ) || ( spellNum == 26 ) || ( spellNum == 29 ) || ( spellNum == 44 ) || ( spellNum == 59 ))
				{
					cursorType = 2;
				}
				mSock->SendTargetCursor( 0, TARGET_CASTSPELL, Magic->spells[spellNum].StringToSay().c_str(), cursorType );
			}
			else
			{
				mChar.SetCasting( false );
				Magic->CastSpell( mSock, &mChar );
				mChar.SetTimer( tCHAR_SPELLTIME, 0 );
				mChar.SetFrozen( false );
				mChar.Dirty( UT_UPDATE );
			}
		}
		else if( mChar.GetNextAct() <= 0 )
		{
			//redo the spell action
			mChar.SetNextAct( 75 );
			if( !mChar.IsOnHorse() && !mChar.IsFlying() )
			{
				// Consider Gargoyle flying as mounted here
				Effects->PlaySpellCastingAnimation( &mChar, Magic->spells[mChar.GetSpellCast()].Action(), false, false );
			}
		}
	}

	if( cwmWorldState->ServerData()->WorldAmbientSounds() >= 1 )
	{
		if( cwmWorldState->ServerData()->WorldAmbientSounds() > 10 )
		{
			cwmWorldState->ServerData()->WorldAmbientSounds( 10 );
		}
		const SI16 soundTimer = static_cast<SI16>( cwmWorldState->ServerData()->WorldAmbientSounds() * 100 );
		if( !mChar.IsDead() && ( RandomNum( 0, soundTimer - 1 )) == ( soundTimer / 2 ))
		{
			Effects->PlayBGSound(( *mSock ), mChar );
		}
	}

	if( mSock->GetTimer( tPC_SPIRITSPEAK ) > 0 && mSock->GetTimer( tPC_SPIRITSPEAK) < cwmWorldState->GetUICurrentTime() )
	{
		mSock->SetTimer( tPC_SPIRITSPEAK, 0 );
	}

	if( mSock->GetTimer( tPC_TRACKING ) > cwmWorldState->GetUICurrentTime() )
	{
		if( mSock->GetTimer( tPC_TRACKINGDISPLAY ) <= cwmWorldState->GetUICurrentTime() )
		{
			mSock->SetTimer( tPC_TRACKINGDISPLAY, BuildTimeValue( static_cast<R32>( cwmWorldState->ServerData()->TrackingRedisplayTime() )));
			Skills->Track( &mChar );
		}
	}
	else
	{
		if( mSock->GetTimer( tPC_TRACKING ) > ( cwmWorldState->GetUICurrentTime() / 10 ))
		{
			// dont send arrow-away packet all the time
			mSock->SetTimer( tPC_TRACKING, 0 );
			if( ValidateObject( mChar.GetTrackingTarget() ))
			{
				CPTrackingArrow tSend = ( *mChar.GetTrackingTarget() );
				tSend.Active( 0 );
				if( mSock->ClientType() >= CV_HS2D )
				{
					tSend.AddSerial( mChar.GetTrackingTarget()->GetSerial() );
				}
				mSock->Send( &tSend );
			}
		}
	}

	if( mChar.IsOnHorse() )
	{
		CItem *horseItem = mChar.GetItemAtLayer( IL_MOUNT );
		if( !ValidateObject( horseItem ))
		{
			mChar.SetOnHorse( false );	// turn it off, we aren't on one because there's no item!
		}
		else if( horseItem->GetDecayTime() != 0 && ( horseItem->GetDecayTime() <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() ))
		{
			mChar.SetOnHorse( false );
			horseItem->Delete();
		}
	}

	if( mChar.GetTimer( tCHAR_FLYINGTOGGLE ) > 0 && mChar.GetTimer( tCHAR_FLYINGTOGGLE ) < cwmWorldState->GetUICurrentTime() )
	{
		mChar.SetTimer( tCHAR_FLYINGTOGGLE, 0 );
		mChar.SetFrozen( false );
		mChar.Teleport();
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CheckNPC()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check NPC's status
//o------------------------------------------------------------------------------------------------o
auto CheckNPC( CChar& mChar, bool checkAI, bool doRestock, bool doPetOfflineCheck ) -> void
{
	bool doAICheck = true;
	std::vector<UI16> scriptTriggers = mChar.GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute )
		{
			if( toExecute->OnAISliver( &mChar ) == 1 )
			{
				// Script returned true or 1, don't do hard-coded AI check
				doAICheck = false;
			}
		}
	}

	if( doAICheck && checkAI )
	{
		CheckAI( mChar );
	}
	Movement->NpcMovement( mChar );

	if( doRestock )
	{
		RestockNPC( mChar, false );
	}

	if( doPetOfflineCheck )
	{
		mChar.CheckPetOfflineTimeout();
	}

	if( mChar.GetTimer( tNPC_SUMMONTIME ))
	{
		if(( mChar.GetTimer( tNPC_SUMMONTIME ) <= cwmWorldState->GetUICurrentTime() ) || cwmWorldState->GetOverflow() )
		{
			// Added Dec 20, 1999
			// QUEST expire check - after an Escort quest is created a timer is set
			// so that the NPC will be deleted and removed from the game if it hangs around
			// too long without every having its quest accepted by a player so we have to remove
			// its posting from the messageboard before icing the NPC
			// Only need to remove the post if the NPC does not have a follow target set
			if( mChar.GetQuestType() == QT_ESCORTQUEST && !ValidateObject( mChar.GetFTarg() ))
			{
				MsgBoardQuestEscortRemovePost( &mChar );
				mChar.Delete();
				return;
			}

			if( mChar.GetNpcAiType() == AI_GUARD && mChar.IsAtWar() )
			{
				mChar.SetTimer( tNPC_SUMMONTIME, BuildTimeValue( 25 ));
				return;
			}
			Effects->PlaySound( &mChar, 0x01FE );
			mChar.SetDead( true );
			mChar.Delete();
			return;
		}
	}

	if( mChar.GetFleeAt() == 0 )
	{
		mChar.SetFleeAt( cwmWorldState->ServerData()->CombatNPCBaseFleeAt() );
	}
	if( mChar.GetReattackAt() == 0 )
	{
		mChar.SetReattackAt( cwmWorldState->ServerData()->CombatNPCBaseReattackAt() );
	}

	auto mNpcWander = mChar.GetNpcWander();
	if( mNpcWander == WT_SCARED )
	{
		if( mChar.GetTimer( tNPC_FLEECOOLDOWN ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
		{
			if( mChar.GetTimer( tNPC_MOVETIME ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
			{
				mChar.SetFleeDistance( static_cast<UI08>( 0 ));
				CChar *mTarget = mChar.GetTarg();
				if( ValidateObject( mTarget ) && !mTarget->IsDead() && ObjInRange( &mChar, mTarget, DIST_INRANGE ))
				{
					if( mChar.GetMounted() )
					{
						mChar.SetTimer( tNPC_MOVETIME, BuildTimeValue( mChar.GetMountedFleeingSpeed() ));
					}
					else
					{
						mChar.SetTimer( tNPC_MOVETIME, BuildTimeValue( mChar.GetFleeingSpeed() ));
					}
				}
				else
				{
					// target no longer exists, or is out of range, stop running
					mChar.SetTarg( nullptr );
					if( mChar.GetOldNpcWander() != WT_NONE )
					{
						mChar.SetNpcWander( mChar.GetOldNpcWander() );
					}
					if( mChar.GetMounted() )
					{
						mChar.SetTimer( tNPC_MOVETIME, BuildTimeValue( mChar.GetMountedWalkingSpeed() ));
					}
					else
					{
						mChar.SetTimer( tNPC_MOVETIME, BuildTimeValue( mChar.GetWalkingSpeed() ));
					}
					mChar.SetOldNpcWander( WT_NONE ); // so it won't save this at the wsc file
				}
			}
		}
	}
	else if( mNpcWander != WT_FLEE && mNpcWander != WT_FROZEN && ( mChar.GetHP() < mChar.GetMaxHP() * mChar.GetFleeAt() / 100 ))
	{
		if( mChar.GetTimer( tNPC_FLEECOOLDOWN ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
		{
			// Make NPC try to flee away from their opponent, if still within range
			CChar *mTarget = mChar.GetTarg();
			if( ValidateObject( mTarget ) && !mTarget->IsDead() && ObjInRange( &mChar, mTarget, DIST_SAMESCREEN ))
			{
				mChar.SetFleeDistance( static_cast<UI08>( 0 ));
				mChar.SetOldNpcWander( mNpcWander );
				mChar.SetNpcWander( WT_FLEE );
				if( mChar.GetMounted() )
				{
					mChar.SetTimer( tNPC_MOVETIME, BuildTimeValue( mChar.GetMountedFleeingSpeed() ));
				}
				else
				{
					mChar.SetTimer( tNPC_MOVETIME, BuildTimeValue( mChar.GetFleeingSpeed() ));
				}
			}
		}
	}
	else if(( mNpcWander == WT_FLEE ) && ( mChar.GetHP() > mChar.GetMaxHP() * mChar.GetReattackAt() / 100 ))
	{
		// Bring NPC out of flee-mode and back to their original wandermode
		mChar.SetTimer( tNPC_FLEECOOLDOWN, BuildTimeValue( 5 )); // Wait at least 5 seconds before reentring flee-mode!
		mChar.SetNpcWander( mChar.GetOldNpcWander() );
		if( mChar.GetMounted() )
		{
			mChar.SetTimer( tNPC_MOVETIME, BuildTimeValue( mChar.GetMountedWalkingSpeed() ));
		}
		else
		{
			mChar.SetTimer( tNPC_MOVETIME, BuildTimeValue( mChar.GetWalkingSpeed() ));
		}
		mChar.SetOldNpcWander( WT_NONE ); // so it won't save this at the wsc file
	}
	Combat->CombatLoop( nullptr, mChar );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CheckItem()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check item decay, spawn timers and boat movement in a given region
//o------------------------------------------------------------------------------------------------o
auto CheckItem( CMapRegion *toCheck, bool checkItems, UI32 nextDecayItems, UI32 nextDecayItemsInHouses, bool doWeather )
{
	auto regItems = toCheck->GetItemList();
	auto collection = regItems->collection();
	for( const auto &itemCheck : collection )
	{
		if( !ValidateObject( itemCheck ) || itemCheck->IsFree() )
			continue;
		
		if( checkItems )
		{
			if( itemCheck->IsDecayable() && ( itemCheck->GetCont() == nullptr ))
			{
				if( itemCheck->GetType() == IT_HOUSESIGN && itemCheck->GetTempVar( CITV_MORE ) > 0 )
				{
					// Don't decay signs that belong to houses
					itemCheck->SetDecayable( false );
				}
				if(( itemCheck->GetDecayTime() <= cwmWorldState->GetUICurrentTime() ) || cwmWorldState->GetOverflow() )
				{
					auto scriptTriggers = itemCheck->GetScriptTriggers();
					for( auto scriptTrig : scriptTriggers )
					{
						cScript *toExecute = JSMapping->GetScript( scriptTrig );
						if( toExecute )
						{
							if( toExecute->OnDecay( itemCheck ) == 0 )
							{
								// if it exists and we don't want hard code, return
								return;
							}
						}
					}

					// Check global script! Maybe there's another event there
					auto toExecute = JSMapping->GetScript( static_cast<UI16>( 0 ));
					if( toExecute )
					{
						if( toExecute->OnDecay( itemCheck ) == 0 )
						{
							// if it exists and we don't want hard code, return
							return;
						}
					}

					if( DecayItem(( *itemCheck ), nextDecayItems, nextDecayItemsInHouses ))
						continue;
				}
			}
			switch( itemCheck->GetType() )
			{
				case IT_ITEMSPAWNER:
				case IT_NPCSPAWNER:
				case IT_SPAWNCONT:
				case IT_LOCKEDSPAWNCONT:
				case IT_UNLOCKABLESPAWNCONT:
				case IT_AREASPAWNER:
				case IT_ESCORTNPCSPAWNER:
				case IT_PLANK:
				{
					if(( itemCheck->GetTempTimer() <= cwmWorldState->GetUICurrentTime() ) || cwmWorldState->GetOverflow() )
					{
						if( itemCheck->GetObjType() == OT_SPAWNER )
						{
							CSpawnItem *spawnItem = static_cast<CSpawnItem *>( itemCheck );
							if( spawnItem->DoRespawn() )
							{
								continue;
							}
							spawnItem->SetTempTimer( BuildTimeValue( static_cast<R32>( RandomNum( spawnItem->GetInterval( 0 ) * 60, spawnItem->GetInterval( 1 ) * 60 ))));
						}
						else if( itemCheck->GetObjType() == OT_ITEM && itemCheck->GetType() == IT_PLANK)
						{
							// Automatically close the plank if it's still open, and still locked
							auto plankStatus = itemCheck->GetTag( "plankLocked" );
							if( plankStatus.m_IntValue == 1 )
							{
								switch( itemCheck->GetId() )
								{
									case 0x3E84: itemCheck->SetId( 0x3EE9 ); itemCheck->SetTempTimer( 0 ); break;
									case 0x3ED5: itemCheck->SetId( 0x3EB1 ); itemCheck->SetTempTimer( 0 ); break;
									case 0x3ED4: itemCheck->SetId( 0x3EB2 ); itemCheck->SetTempTimer( 0 ); break;
									case 0x3E89: itemCheck->SetId( 0x3E8A ); itemCheck->SetTempTimer( 0 ); break;
								}
							}
						}
						else
						{
							itemCheck->SetType( IT_NOTYPE );
							Console.Warning( "Invalid spawner object detected; item type reverted to 0. All spawner objects have to be added using 'ADD SPAWNER # command." );
						}
					}
					break;
				}
				case IT_SOUNDOBJECT:
					if( itemCheck->GetTempVar( CITV_MOREY ) < 25 )
					{
						if( RandomNum( 1, 100 ) <= static_cast<SI32>( itemCheck->GetTempVar( CITV_MOREZ )))
						{
							for( auto &tSock : FindNearbyPlayers( itemCheck, static_cast<UI16>( itemCheck->GetTempVar( CITV_MOREY ))))
							{
								Effects->PlaySound( tSock, static_cast<UI16>( itemCheck->GetTempVar( CITV_MOREX )), false );
							}
						}
					}
					break;
				default:
					break;
			}
		}
		if( itemCheck->CanBeObjType( OT_BOAT ))
		{
			CBoatObj *mBoat = static_cast<CBoatObj *>( itemCheck );
			SI08 boatMoveType = mBoat->GetMoveType();
			if( ValidateObject( mBoat ) && boatMoveType && (( mBoat->GetMoveTime() <= cwmWorldState->GetUICurrentTime() ) || cwmWorldState->GetOverflow() ))
			{
				if( boatMoveType != BOAT_ANCHORED )
				{
					switch( boatMoveType )
					{
						//case BOAT_ANCHORED:
						//case BOAT_STOP:
						case BOAT_FORWARD:
						case BOAT_SLOWFORWARD:
						case BOAT_ONEFORWARD:
							MoveBoat( itemCheck->GetDir(), mBoat );
							break;
						case BOAT_BACKWARD:
						case BOAT_SLOWBACKWARD:
						case BOAT_ONEBACKWARD:
						{
							UI08 dir = static_cast<UI08>( itemCheck->GetDir() + 4 );
							if( dir > 7 )
							{
								dir %= 8;
							}
							MoveBoat( dir, mBoat );
							break;
						}
						case BOAT_LEFT:
						case BOAT_SLOWLEFT:
						case BOAT_ONELEFT:
						{
							UI08 dir = static_cast<UI08>( itemCheck->GetDir() - 2 );

							dir %= 8;
							MoveBoat( dir, mBoat );
							break;
						}
						case BOAT_RIGHT:
						case BOAT_SLOWRIGHT:
						case BOAT_ONERIGHT:
						{
							// Right / One Right
							UI08 dir = static_cast<UI08>( itemCheck->GetDir() + 2 );

							dir %= 8;
							MoveBoat( dir, mBoat );
							break;
						}
						case BOAT_FORWARDLEFT:
						case BOAT_SLOWFORWARDLEFT:
						case BOAT_ONEFORWARDLEFT:
						{
							UI08 dir = static_cast<UI08>( itemCheck->GetDir() - 1 );

							dir %= 8;
							MoveBoat( dir, mBoat );
							break;
						}
						case BOAT_FORWARDRIGHT:
						case BOAT_SLOWFORWARDRIGHT:
						case BOAT_ONEFORWARDRIGHT:
						{
							UI08 dir = static_cast<UI08>( itemCheck->GetDir() + 1 );

							dir %= 8;
							MoveBoat( dir, mBoat );
							break;
						}
						case BOAT_BACKWARDLEFT:
						case BOAT_SLOWBACKWARDLEFT:
						case BOAT_ONEBACKWARDLEFT:
						{
							UI08 dir = static_cast<UI08>( itemCheck->GetDir() + 5 );
							if( dir > 7 )
							{
								dir %= 8;
							}
							MoveBoat( dir, mBoat );
							break;
						}
						case BOAT_BACKWARDRIGHT:
						case BOAT_SLOWBACKWARDRIGHT:
						case BOAT_ONEBACKWARDRIGHT:
						{
							UI08 dir = static_cast<UI08>( itemCheck->GetDir() + 3 );
							if( dir > 7 )
							{
								dir %= 8;
							}
							MoveBoat( dir, mBoat );
							break;
						}
						default:
							break;
					}

					// One-step boat commands, so reset move type to 0 after the initial move
					if( boatMoveType == BOAT_LEFT || boatMoveType == BOAT_RIGHT )
					{
						// Move 50% slower left/right than forward/back
						mBoat->SetMoveTime( BuildTimeValue( static_cast<R32>( cwmWorldState->ServerData()->CheckBoatSpeed() ) * 1.5 ));
					}
					else if( boatMoveType >= BOAT_ONELEFT && boatMoveType <= BOAT_ONEBACKWARDRIGHT )
					{
						mBoat->SetMoveType( 0 );

						// Set timer to restrict movement to normal boat speed if player spams command
						mBoat->SetMoveTime( BuildTimeValue( static_cast<R32>( cwmWorldState->ServerData()->CheckBoatSpeed() ) * 1.5 ));
					}
					else if( boatMoveType >= BOAT_SLOWLEFT && boatMoveType <= BOAT_SLOWBACKWARDLEFT )
					{
						// Set timer to slowly move the boat forward
						mBoat->SetMoveTime( BuildTimeValue( static_cast<R32>( cwmWorldState->ServerData()->CheckBoatSpeed() ) * 2.0 ));
					}
					else
					{
						// Set timer to move the boat forward at normal speed
						mBoat->SetMoveTime( BuildTimeValue( static_cast<R32>( cwmWorldState->ServerData()->CheckBoatSpeed() )));
					}
				}
			}
		}

		// Do JS Weather for item
		if( doWeather )
		{
			DoLight( itemCheck, cwmWorldState->ServerData()->WorldLightCurrentLevel() );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::CheckAutoTimers()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check automatic and timer controlled functions
//o------------------------------------------------------------------------------------------------o
auto CWorldMain::CheckAutoTimers() -> void
{
	static UI32 nextCheckSpawnRegions	= 0;
	static UI32 nextCheckTownRegions	= 0;
	static UI32 nextCheckItems			= 0;
	static UI32 nextDecayItems			= 0;
	static UI32 nextDecayItemsInHouses	= 0;
	static UI32 nextSetNPCFlagTime		= 0;
	static UI32 accountFlush			= 0;
	bool doWeather						= false;
	bool doPetOfflineCheck				= false;
	CServerData *serverData				= ServerData();

	// modify this stuff to take into account more variables
	if(( accountFlush <= GetUICurrentTime() ) || GetOverflow() )
	{
		bool reallyOn = false;
		// time to flush our account status!
		MAPUSERNAMEID_ITERATOR I;
		for( I = Accounts->Begin(); I != Accounts->End(); ++I )
		{
			CAccountBlock_st& actbTemp = I->second;
			if( actbTemp.wAccountIndex == AB_INVALID_ID )
			{
				continue;
			}

			if( actbTemp.wFlags.test( AB_FLAGS_ONLINE ))
			{
				reallyOn = false;	// to start with, there's no one really on
				{
					for( auto &tSock : Network->connClients )
					{
						CChar *tChar = tSock->CurrcharObj();
						if( !ValidateObject( tChar ))
						{
							continue;
						}
						if( tChar->GetAccount().wAccountIndex == actbTemp.wAccountIndex )
						{
							reallyOn = true;
						}
					}
				}
				if( !reallyOn )
				{
					// no one's really on, let's set that
					actbTemp.wFlags.reset( AB_FLAGS_ONLINE );
				}
			}
		}
		accountFlush = BuildTimeValue( static_cast<R32>( serverData->AccountFlushTimer() ));
	}
	//Network->On();   //<<<<<< WHAT the HECK, this is why you dont bury mutex locking
	// PushConn and PopConn lock and unlock as well (yes, bad)
	// But now we are doing recursive lock here
	
	if( GetWorldSaveProgress() == SS_NOTSAVING )
	{
		{
			for( auto &tSock : Network->connClients )
			{
				if(( tSock->IdleTimeout() != -1 ) && static_cast<UI32>( tSock->IdleTimeout() ) <= GetUICurrentTime() )
				{
					CChar *tChar = tSock->CurrcharObj();
					if( !ValidateObject( tChar ))
					{
						continue;
					}
					if( !tChar->IsGM() )
					{
						tSock->IdleTimeout( -1 );
						tSock->SysMessage( 1246 ); // You're being disconnected because you were idle too long.
						Network->Disconnect( tSock );
					}
				}
				else if((( static_cast<UI32>( tSock->IdleTimeout() + 300 * 1000 ) <= GetUICurrentTime() 
					&& static_cast<UI32>( tSock->IdleTimeout() + 200 * 1000 ) >= GetUICurrentTime() ) || GetOverflow() ) && !tSock->WasIdleWarned() )
				{
					//is their idle time between 3 and 5 minutes, and they haven't been warned already?
					CPIdleWarning warn( 0x07 );
					tSock->Send( &warn );
					tSock->WasIdleWarned( true );
				}

				if( serverData->KickOnAssistantSilence() )
				{
					if( !tSock->NegotiatedWithAssistant() && tSock->NegotiateTimeout() != -1 && static_cast<UI32>( tSock->NegotiateTimeout() ) <= GetUICurrentTime() )
					{
						const CChar *tChar = tSock->CurrcharObj();
						if( !ValidateObject( tChar ))
						{
							continue;
						}
						if( !tChar->IsGM() )
						{
							tSock->IdleTimeout( -1 );
							tSock->SysMessage( 9047 ); // Failed to negotiate features with assistant tool. Disconnecting client...
							Network->Disconnect( tSock );
						}
					}
				}

				// Check player's network traffic usage versus caps set in ini
				if( tSock->LoginComplete() && ( tSock->AcctNo() != 0 ) && tSock->GetTimer( tPC_TRAFFICWARDEN ) <= GetUICurrentTime() )
				{
					if( !ValidateObject( tSock->CurrcharObj() ) || tSock->CurrcharObj()->IsGM() )
					{
						continue;
					}

					bool tempTimeBan = false;
					if( tSock->BytesReceived() > serverData->MaxClientBytesIn() )
					{
						// Player has exceeded the cap! Send one warning - next time kick player
						tSock->SysMessage( Dictionary->GetEntry( 9082, tSock->Language() )); // Excessive data usage detected! Sending too many requests to the server in a short amount of time will get you banned.
						tSock->BytesReceivedWarning( tSock->BytesReceivedWarning() + 1 );
						if( tSock->BytesReceivedWarning() > 2 )
						{
							// If it happens 3 times in the same session, give player a temporary ban
							tempTimeBan = true;
						}
					}

					if( tSock->BytesSent() > serverData->MaxClientBytesOut() )
					{
						// This is data sent from server, so should be more lenient before a kick (though could still be initiated by player somehow)
						tSock->SysMessage( Dictionary->GetEntry( 9082, tSock->Language() )); // Excessive data usage detected! Sending too many requests to the server in a short amount of time will get you banned.
						tSock->BytesSentWarning( tSock->BytesSentWarning() + 1 );
						if( tSock->BytesSentWarning() > 2 )
						{
							// If it happens 3 times or more in the same session, give player a temporary ban
							tempTimeBan = true;
						}
					}

					if( tempTimeBan )
					{
						// Give player a 30 minute temp ban
						CAccountBlock_st& myAccount = Accounts->GetAccountById( tSock->GetAccount().wAccountIndex );
						myAccount.wFlags.set( AB_FLAGS_BANNED, true );
						myAccount.wTimeBan = GetMinutesSinceEpoch() + serverData->NetTrafficTimeban();
						Network->Disconnect( tSock );
						continue;
					}

					// Reset amount of bytes received and sent, and restart timer
					tSock->BytesReceived( 0 );
					tSock->BytesSent( 0 );
					tSock->SetTimer( tPC_TRAFFICWARDEN, BuildTimeValue( static_cast<R32>( 10 )));
				}
			}
		}
	}
	else if( GetWorldSaveProgress() == SS_JUSTSAVED )
	{
		// if we've JUST saved, do NOT kick anyone off (due to a possibly really long save), but reset any offending players to 60 seconds to go before being kicked off		
		{
			for( auto &wsSocket : Network->connClients )
			{
				if( wsSocket )
				{
					if( static_cast<UI32>( wsSocket->IdleTimeout() ) < GetUICurrentTime() )
					{
						wsSocket->IdleTimeout( BuildTimeValue( 60.0F ));
						wsSocket->WasIdleWarned( true );//don't give them the message if they only have 60s
					}
					if( cwmWorldState->ServerData()->KickOnAssistantSilence() )
					{
						if( !wsSocket->NegotiatedWithAssistant() && static_cast<UI32>( wsSocket->NegotiateTimeout() ) < GetUICurrentTime() )
						{
							wsSocket->NegotiateTimeout( BuildTimeValue( 60.0F ));
						}
					}
				}
			}
		}
		SetWorldSaveProgress( SS_NOTSAVING );
	}
	if(( nextCheckTownRegions <= GetUICurrentTime() ) || GetOverflow() )
	{
		std::for_each( cwmWorldState->townRegions.begin(), cwmWorldState->townRegions.end(),[]( std::pair<const UI16, CTownRegion*> &town )
		{
			if( town.second != nullptr )
			{
				town.second->PeriodicCheck();
			}
		});
		nextCheckTownRegions = BuildTimeValue( 10 ); // do checks every 10 seconds or so, rather than every single time
		JailSys->PeriodicCheck();
	}
	
	if(( nextCheckSpawnRegions <= GetUICurrentTime() ) && ( serverData->CheckSpawnRegionSpeed() != -1 ))
	{
		//Regionspawns
		UI32 itemsSpawned		= 0;
		UI32 npcsSpawned		= 0;
		UI32 totalItemsSpawned	= 0;
		UI32 totalNpcsSpawned	= 0;
		UI32 maxItemsSpawned	= 0;
		UI32 maxNpcsSpawned		= 0;
		
		for( auto &[regnum, spawnReg] : cwmWorldState->spawnRegions )
		{
			if( spawnReg )
			{
				if( spawnReg->GetNextTime() <= GetUICurrentTime() )
				{
					spawnReg->DoRegionSpawn( itemsSpawned, npcsSpawned );
				}
				// Grab some info from the spawn region anyway, even if it's not time to spawn
				totalItemsSpawned += static_cast<UI32>( spawnReg->GetCurrentItemAmt() );
				totalNpcsSpawned += static_cast<UI32>( spawnReg->GetCurrentCharAmt() );
				maxItemsSpawned += static_cast<UI32>( spawnReg->GetMaxItemSpawn() );
				maxNpcsSpawned += static_cast<UI32>( spawnReg->GetMaxCharSpawn() );
			}
		}

		// Adaptive spawn region check timer. The closer spawn regions as a whole are to being at their defined max capacity,
		// the less frequently UOX3 will check spawn regions again. Similarly, the more room there is to spawn additional
		// stuff, the more frequently UOX3 will check spawn regions
		auto checkSpawnRegionSpeed = static_cast<R32>( serverData->CheckSpawnRegionSpeed() );
		UI16 itemSpawnCompletionRatio = ( maxItemsSpawned > 0 ? (( totalItemsSpawned * 100.0 ) / maxItemsSpawned ) : 100 );
		UI16 npcSpawnCompletionRatio = ( maxNpcsSpawned > 0 ? (( totalNpcsSpawned * 100.0 ) / maxNpcsSpawned ) : 100 );
		
		SI32 avgCompletionRatio = ( itemSpawnCompletionRatio + npcSpawnCompletionRatio ) / 2;
		if( avgCompletionRatio == 100 )
		{
			checkSpawnRegionSpeed *= 3;
		}
		else if( avgCompletionRatio >= 90 )
		{
			checkSpawnRegionSpeed *= 2;
		}
		else if( avgCompletionRatio >= 75 )
		{
			checkSpawnRegionSpeed *= 1.5;
		}
		else if( avgCompletionRatio >= 50 )
		{
			checkSpawnRegionSpeed *= 1.25;
		}

		nextCheckSpawnRegions = BuildTimeValue( checkSpawnRegionSpeed );//Don't check them TOO often (Keep down the lag)
	}

	HTMLTemplates->Poll( ETT_ALLTEMPLATES );

	const UI32 saveinterval = serverData->ServerSavesTimerStatus();
	if( saveinterval != 0 )
	{
		time_t oldTime = GetOldTime();
		if( !GetAutoSaved() )
		{
			SetAutoSaved( true );
			time( &oldTime );
			SetOldTime( static_cast<UI32>( oldTime ));
		}
		time_t newTime = GetNewTime();
		time( &newTime );
		SetNewTime( static_cast<UI32>( newTime ));

		if( difftime( GetNewTime(), GetOldTime() ) >= saveinterval )
		{
			// Added Dec 20, 1999
			// After an automatic world save occurs, lets check to see if
			// anyone is online (clients connected).  If nobody is connected
			// Lets do some maintenance on the bulletin boards.
			if( !GetPlayersOnline() && ( GetWorldSaveProgress() != SS_SAVING ))
			{
				Console << "No players currently online. Starting bulletin board maintenance" << myendl;
				Console.Log( "Bulletin Board Maintenance routine running (AUTO)", "server.log" );
				MsgBoardMaintenance();
			}

			SetAutoSaved( false );

#if PLATFORM == WINDOWS
			SetConsoleCtrlHandler( exit_handler, TRUE );
#endif
			isWorldSaving = true;
			SaveNewWorld( false );
			isWorldSaving = false;
#if PLATFORM == WINDOWS
			SetConsoleCtrlHandler( exit_handler, false );
#endif
		}
	}

	/*time_t oldIPTime = GetOldIPTime();
	if( !GetIPUpdated() )
	{
		SetIPUpdated( true );
		time(&oldIPTime);
		SetOldIPTime( static_cast<UI32>(oldIPTime) );
	}
	time_t newIPTime = GetNewIPTime();
	time(&newIPTime);
	SetNewIPTime( static_cast<UI32>(newIPTime) );

	if( difftime( GetNewIPTime(), GetOldIPTime() ) >= 120 )
	{
		ServerData()->RefreshIPs();
		SetIPUpdated( false );
	}*/

	//Time functions
	if(( GetUOTickCount() <= GetUICurrentTime() ) || ( GetOverflow() ))
	{
		UI08 oldHour = serverData->ServerTimeHours();
		if( serverData->IncMinute() )
		{
			Weather->NewDay();
		}
		if( oldHour != serverData->ServerTimeHours() )
		{
			Weather->NewHour();
		}

		SetUOTickCount( BuildTimeValue( serverData->ServerSecondsPerUOMinute() ));
	}

	if(( GetTimer( tWORLD_LIGHTTIME ) <= GetUICurrentTime() ) || GetOverflow() )
	{
		DoWorldLight();  //Changes lighting, if it is currently time to.
		Weather->DoStuff();	// updates the weather types
		SetTimer( tWORLD_LIGHTTIME, serverData->BuildSystemTimeValue( tSERVER_WEATHER ));
		doWeather = true;
	}

	if(( GetTimer( tWORLD_PETOFFLINECHECK ) <= GetUICurrentTime() ) || GetOverflow() )
	{
		SetTimer( tWORLD_PETOFFLINECHECK, serverData->BuildSystemTimeValue( tSERVER_PETOFFLINECHECK ));
		doPetOfflineCheck = true;
	}

	bool checkFieldEffects = false;
	if(( GetTimer( tWORLD_NEXTFIELDEFFECT ) <= GetUICurrentTime() ) || GetOverflow() )
	{
		checkFieldEffects = true;
		SetTimer( tWORLD_NEXTFIELDEFFECT, BuildTimeValue( 0.5f ));
	}
	std::set<CMapRegion *> regionList;
	{
		for( auto &iSock : Network->connClients )
		{
			if( iSock )
			{
				CChar *mChar = iSock->CurrcharObj();
				if( !ValidateObject( mChar ))
				{
					continue;
				}
				UI08 worldNumber = mChar->WorldNumber();
				if( mChar->GetAccount().wAccountIndex == iSock->AcctNo() && mChar->GetAccount().dwInGame == mChar->GetSerial() )
				{
					GenericCheck( iSock, (*mChar), checkFieldEffects, doWeather );
					CheckPC( iSock, ( *mChar ));
					
					SI16 xOffset = MapRegion->GetGridX( mChar->GetX() );
					SI16 yOffset = MapRegion->GetGridY( mChar->GetY() );
					
					// Restrict the amount of active regions based on how far player is from the border
					// to the next one. This reduces active regions around a player from always 9 to
					// varying between 3 to 6. Only regions on yOffset are considered, because the xOffset
					// ones are too narrow
					auto yOffsetUnrounded = static_cast<R32>( mChar->GetY() ) / static_cast<R32>( MapRowSize );
					SI08 counter2Start = 0, counter2End = 0;
					if( yOffsetUnrounded < yOffset + 0.25 )
					{
						counter2Start = -1;
						counter2End = 0;
					}
					else if( yOffsetUnrounded > yOffset + 0.75 )
					{
						counter2Start = 0;
						counter2End = 1;
					}
					
					for( SI08 counter = -1; counter <= 1; ++counter )
					{
						// Check 3 x colums
						for( SI08 ctr2 = counter2Start; ctr2 <= counter2End; ++ctr2 )
						{
							// Check variable y colums
							auto tC = MapRegion->GetMapRegion( xOffset + counter, yOffset + ctr2, worldNumber );
							if( tC )
							{
								regionList.insert( tC );
							}
						}
					}
				}
			}
		}
	}

	// Reduce some lag checking these timers constantly in the loop
	bool setNPCFlags = false, checkItems = false, checkAI = false, doRestock = false;
	if(( nextSetNPCFlagTime <= GetUICurrentTime() ) || GetOverflow() )
	{
		nextSetNPCFlagTime = serverData->BuildSystemTimeValue( tSERVER_NPCFLAGUPDATETIMER );	// Slow down lag "needed" for setting flags, they are set often enough;-)
		setNPCFlags = true;
	}
	if(( nextCheckItems <= GetUICurrentTime() ) || GetOverflow() )
	{
		nextCheckItems = BuildTimeValue( static_cast<R32>( serverData->CheckItemsSpeed() ));
		nextDecayItems = serverData->BuildSystemTimeValue( tSERVER_DECAY );
		nextDecayItemsInHouses = serverData->BuildSystemTimeValue( tSERVER_DECAYINHOUSE );
		checkItems = true;
	}
	if(( GetTimer( tWORLD_NEXTNPCAI ) <= GetUICurrentTime() ) || GetOverflow() )
	{
		SetTimer( tWORLD_NEXTNPCAI, BuildTimeValue( static_cast<R32>( serverData->CheckNpcAISpeed() )));
		checkAI = true;
	}
	if(( GetTimer( tWORLD_SHOPRESTOCK ) <= GetUICurrentTime() ) || GetOverflow() )
	{
		SetTimer( tWORLD_SHOPRESTOCK, serverData->BuildSystemTimeValue( tSERVER_SHOPSPAWN ));
		doRestock = true;
	}

	bool allowAwakeNPCs = cwmWorldState->ServerData()->AllowAwakeNPCs();
	for( auto &toCheck : regionList )
	{
		auto regChars = toCheck->GetCharList();
		auto collection = regChars->collection();
		for( const auto &charCheck : collection )
		{
			if( ValidateObject( charCheck ))
			{
				if( charCheck->IsNpc() )
				{
					if( !charCheck->IsAwake() || !allowAwakeNPCs )
					{
						// Only perform these checks on NPCs that are not permanently awake
						if( !GenericCheck( nullptr, ( *charCheck ), checkFieldEffects, doWeather ))
						{
							if( setNPCFlags )
							{
								UpdateFlag( charCheck );	 // only set flag on npcs every 60 seconds (save a little extra lag)
							}
							CheckNPC(( *charCheck ), checkAI, doRestock, doPetOfflineCheck );
						}
					}
				}
				else if( charCheck->GetTimer( tPC_LOGOUT ))
				{
					CAccountBlock_st& actbTemp = charCheck->GetAccount();
					if( actbTemp.wAccountIndex != AB_INVALID_ID )
					{
						SERIAL oaiw = actbTemp.dwInGame;
						if( oaiw == INVALIDSERIAL )
						{
							charCheck->SetTimer( tPC_LOGOUT, 0 );
							charCheck->RemoveFromSight();
							charCheck->Update();
						}
						else if(( oaiw == charCheck->GetSerial() ) && (( charCheck->GetTimer( tPC_LOGOUT ) <= GetUICurrentTime() ) || GetOverflow() ))
						{
							actbTemp.dwInGame = INVALIDSERIAL;
							charCheck->SetTimer( tPC_LOGOUT, 0 );

							// End combat, clear targets
							charCheck->SetAttacker( nullptr );
							charCheck->SetWar( false );
							charCheck->SetTarg( nullptr );

							charCheck->Update();
							charCheck->Teleport();

							// Announce that player has logged out (if enabled)
							if( cwmWorldState->ServerData()->ServerJoinPartAnnouncementsStatus() )
							{
								SysBroadcast( oldstrutil::format(1024, Dictionary->GetEntry( 752 ), charCheck->GetName().c_str() )); // %s has left the realm.
							}
						}
					}
				}
			}
		}
		CheckItem( toCheck, checkItems, nextDecayItems, nextDecayItemsInHouses, doWeather );
	}

	// Check NPCs marked as always active, regardless of whether their region is "awake"
	if( allowAwakeNPCs )
	{
		auto alwaysAwakeChars = Npcs->GetAlwaysAwakeNPCs();
		std::vector<CChar*> toRemove;
		for( const auto &charCheck : alwaysAwakeChars->collection() )
		{
			if( ValidateObject( charCheck ) && !charCheck->IsFree() && charCheck->IsNpc() )
			{
				if( !GenericCheck( nullptr, ( *charCheck ), checkFieldEffects, doWeather ))
				{
					if( setNPCFlags )
					{
						UpdateFlag( charCheck );	 // only set flag on npcs every 60 seconds (save a little extra lag)
					}
					CheckNPC(( *charCheck ), checkAI, doRestock, doPetOfflineCheck );
				}
			}
			else
			{
				toRemove.push_back( charCheck );
			}
		}
		std::for_each( toRemove.begin(), toRemove.end(), [&alwaysAwakeChars]( CChar *character )
		{
			alwaysAwakeChars->Remove( character );
		});
		toRemove.clear();
	}

	Effects->CheckTempeffects();
	SpeechSys->Poll();

	// Implement RefreshItem() / StatWindow() queue here
	std::for_each( cwmWorldState->refreshQueue.begin(), cwmWorldState->refreshQueue.end(), []( std::pair<CBaseObject*, UI32> entry )
	{
		if( ValidateObject( entry.first ))
		{
			if( entry.first->CanBeObjType( OT_CHAR ))
			{
				auto uChar = static_cast<CChar *>( entry.first );
				
				if( uChar->GetUpdate( UT_HITPOINTS ))
				{
					UpdateStats( entry.first, 0 );
				}
				if( uChar->GetUpdate( UT_STAMINA ))
				{
					UpdateStats( entry.first, 1 );
				}
				if( uChar->GetUpdate( UT_MANA ))
				{
					UpdateStats( entry.first, 2 );
				}
				
				if( uChar->GetUpdate( UT_LOCATION ))
				{
					uChar->Teleport();
				}
				else if( uChar->GetUpdate( UT_HIDE ))
				{
					uChar->ClearUpdate();
					if( uChar->GetVisible() != VT_VISIBLE )
					{
						uChar->RemoveFromSight();
					}
					uChar->Update( nullptr, false );
				}
				else if( uChar->GetUpdate( UT_UPDATE ))
				{
					uChar->Update();
				}
				else if( uChar->GetUpdate( UT_STATWINDOW ))
				{
					CSocket *uSock = uChar->GetSocket();
					if( uSock )
					{
						uSock->StatWindow( uChar );
					}
				}

				uChar->ClearUpdate();
			}
			else
			{
				entry.first->Update();
			}
		}
	});
	cwmWorldState->refreshQueue.clear();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	InitClasses()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Initialize UOX classes
//o------------------------------------------------------------------------------------------------o
auto InitClasses() -> void
{
	cwmWorldState->ClassesInitialized( true );
	JSEngine = &aJSEngine;
	JSMapping = &aJSMapping;
	Effects = &aEffects;
	Commands = &aCommands;
	Combat = &aCombat;
	Items = &aItems;
	Map = &aMap;
	Npcs = &aNpcs;
	Skills = &aSkills;
	Weight = &aWeight;
	JailSys = &aJailSys;
	Network = &aNetwork;
	Magic = &aMagic;
	Races = &aRaces;
	Weather = &aWeather;
	Movement = &aMovement;
	GuildSys = &aGuildSys;
	WhoList = &aWhoList;
	OffList = &aOffList;
	Books = &aBooks;
	GMQueue = &aGMQueue;
	Dictionary = &aDictionary;
	Accounts = &aAccounts;
	MapRegion = &aMapRegion;
	SpeechSys = &aSpeechSys;
	CounselorQueue = &aCounselorQueue;
	HTMLTemplates = &aHTMLTemplates;
	FileLookup = &aFileLookup;
	
	aJSEngine.Startup();
	aFileLookup.Startup();
	aCommands.Startup();
 	aSpeechSys.Startup();
	// Need to do map
	aNetwork.Startup();
	aMap.Load();
	JSMapping->ResetDefaults();
	JSMapping->GetEnvokeById()->Parse();
	JSMapping->GetEnvokeByType()->Parse();
	aMapRegion.Startup();
	aAccounts.SetPath( cwmWorldState->ServerData()->Directory( CSDDP_ACCOUNTS ));
}

auto FindNearbyObjects( SI16 x, SI16 y, UI08 worldNumber, UI16 instanceId, UI16 distance ) -> std::vector<CBaseObject *>;
auto InMulti( SI16 x, SI16 y, SI08 z, CMultiObj *m ) -> bool;
//o------------------------------------------------------------------------------------------------o
//|	Function	-	FindMultiFunctor()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Looks for a multi at object's location and assigns any multi found to object
//o------------------------------------------------------------------------------------------------o
auto FindMultiFunctor( CBaseObject *a, [[maybe_unused]] UI32 &b, [[maybe_unused]] void *extraData ) -> bool
{
	if( ValidateObject( a ))
	{
		if( a->CanBeObjType( OT_MULTI ))
		{
			auto aMulti = static_cast<CMultiObj *>( a );
			for( auto &objToCheck : FindNearbyObjects( aMulti->GetX(), aMulti->GetY(), aMulti->WorldNumber(), aMulti->GetInstanceId(), 20 ))
			{
				if( InMulti( objToCheck->GetX(), objToCheck->GetY(), objToCheck->GetZ(), aMulti ))
				{
					objToCheck->SetMulti( aMulti );
				}
				else if(( objToCheck->GetObjType() == OT_ITEM )
					  && ((( objToCheck->GetId() >= 0x0b95 ) && ( objToCheck->GetId() <= 0x0c0e )) || ( objToCheck->GetId() == 0x1f28 ) || ( objToCheck->GetId() == 0x1f29 )))
				{
					// Reunite house signs with their multis
					SERIAL houseSerial = static_cast<CItem *>( objToCheck )->GetTempVar( CITV_MORE );
					CMultiObj *multi = CalcMultiFromSer( houseSerial );
					if( ValidateObject( multi ))
					{
						objToCheck->SetMulti( multi );
					}
				}
				else
				{
					// No other multi found where item is, safe to set item's multi to INVALIDSERIAL
					if( FindMulti( objToCheck ) == nullptr )
					{
						objToCheck->SetMulti( INVALIDSERIAL );
					}
				}
			}
		}
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	InitMultis()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Initialize Multis
//o------------------------------------------------------------------------------------------------o
auto InitMultis() -> void
{
	Console << "Initializing multis            ";

	UI32 b = 0;
	ObjectFactory::GetSingleton().IterateOver( OT_MULTI, b, nullptr, &FindMultiFunctor );

	Console.PrintDone();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	DisplayBanner()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Display some information at the end of UOX startup
//o------------------------------------------------------------------------------------------------o
auto DisplayBanner() -> void
{
	Console.PrintSectionBegin();

	Console.TurnYellow();
	Console << "Compiled on ";
	Console.TurnNormal();
	Console << __DATE__ << " (" << __TIME__ << ")" << myendl;

	Console.TurnYellow();
	Console << "Compiled by ";
	Console.TurnNormal();
	Console << CVersionClass::GetName() << myendl;

	Console.TurnYellow();
	Console << "Contact: ";
	Console.TurnNormal();
	Console << CVersionClass::GetEmail() << myendl;

	Console.PrintSectionBegin();
	saveOnShutdown = true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Shutdown()
//|	Date		-	Oct. 09, 1999
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handled deleting / free() ing of pointers as neccessary
//|					as well as closing open file handles to avoid file file corruption.
//|					Exits with proper error code.
//o------------------------------------------------------------------------------------------------o
auto Shutdown( SI32 retCode ) -> void
{
	Console.PrintSectionBegin();
	Console << "Beginning UOX final shut down sequence..." << myendl;
	if( retCode && saveOnShutdown )
	{
		//they want us to save, there has been an error, we have loaded the world, and WorldState is a valid pointer.
#if PLATFORM == WINDOWS
		SetConsoleCtrlHandler( exit_handler, true );
#endif
		isWorldSaving = true;
		do
		{
			cwmWorldState->SaveNewWorld( true );
		} while( cwmWorldState->GetWorldSaveProgress() == SS_SAVING );
		isWorldSaving = false;
#if PLATFORM == WINDOWS
		SetConsoleCtrlHandler( exit_handler, false );
#endif
	}

	if( cwmWorldState && cwmWorldState->ClassesInitialized() )
	{
		if( HTMLTemplates )
		{
			Console << "HTMLTemplates object detected. Writing Offline HTML Now...";
			HTMLTemplates->Poll( ETT_OFFLINE );
			Console.PrintDone();
		}
		else
		{
			Console << "HTMLTemplates object not found." << myendl;
		}

		Console << "Destroying class objects and pointers... ";
		// delete any objects that were created (delete takes care of nullptr check =)
		UnloadSpawnRegions();

		UnloadRegions();
		Console.PrintDone();
	}

	//Lets wait for console thread to quit here
	if( !retCode )
	{
		cons.join();
	}

	// don't leave file pointers open, could lead to file corruption

	Console.PrintSectionBegin();

	Console.TurnGreen();
	Console << "Server shutdown complete!" << myendl;
	Console << "Thank you for supporting " << CVersionClass::GetName() << myendl;
	Console.TurnNormal();
	Console.PrintSectionBegin();

	// dispay what error code we had
	// don't report errorlevel for no errors, this is confusing ppl
	if( retCode )
	{
		Console.TurnRed();
		Console << "Exiting UOX with errorlevel " << retCode << myendl;
		Console.TurnNormal();
#if PLATFORM == WINDOWS
		Console << "Press Return to exit " << myendl;
		std::string throwAway;
		std::getline( std::cin, throwAway );
#endif
	}
	else
	{
		Console.TurnGreen();
		Console << "Exiting UOX with no errors..." << myendl;
		Console.TurnNormal();
	}

	Console.PrintSectionBegin();
	exit( retCode );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	AdvanceObj()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle advancement objects (stat / skill gates)
//o------------------------------------------------------------------------------------------------o
auto AdvanceObj( CChar *applyTo, UI16 advObj, bool multiUse ) -> void
{
	if(( applyTo->GetAdvObj() == 0 ) || multiUse )
	{
		Effects->PlayStaticAnimation( applyTo, 0x373A, 0, 15);
		Effects->PlaySound( applyTo, 0x01E9 );
		applyTo->SetAdvObj( advObj );
		auto sect = "ADVANCEMENT "s + oldstrutil::number( advObj );
		sect						= oldstrutil::trim( oldstrutil::removeTrailing( sect, "//" ));
		auto Advancement	= FileLookup->FindEntry( sect, advance_def );
		if( Advancement == nullptr )
		{
			Console << "ADVANCEMENT OBJECT: Script section not found, Aborting" << myendl;
			applyTo->SetAdvObj( 0 );
			return;
		}
		CItem *retItem		= nullptr;
		auto hairobject		= applyTo->GetItemAtLayer( IL_HAIR );
		auto beardobject	= applyTo->GetItemAtLayer( IL_FACIALHAIR );
		DFNTAGS tag			= DFNTAG_COUNTOFTAGS;
		std::string cdata;
		SI32 ndata			= -1, odata = -1;
		UI08 skillToSet		= 0;
		for( const auto &sec : Advancement->collection2() )
		{
			tag = sec->tag;
			cdata = sec->cdata;
			ndata = sec->ndata;
			odata = sec->odata;
			
			switch( tag )
			{
				case DFNTAG_ALCHEMY:			skillToSet = ALCHEMY;							break;
				case DFNTAG_ANATOMY:			skillToSet = ANATOMY;							break;
				case DFNTAG_ANIMALLORE:			skillToSet = ANIMALLORE;						break;
				case DFNTAG_ARMSLORE:			skillToSet = ARMSLORE;							break;
				case DFNTAG_ARCHERY:			skillToSet = ARCHERY;							break;
				case DFNTAG_ADVOBJ:				applyTo->SetAdvObj( static_cast<UI16>( ndata ));	break;
				case DFNTAG_BEGGING:			skillToSet = BEGGING;							break;
				case DFNTAG_BLACKSMITHING:		skillToSet = BLACKSMITHING;						break;
				case DFNTAG_BOWCRAFT:			skillToSet = BOWCRAFT;							break;
				case DFNTAG_BUSHIDO:			skillToSet = BUSHIDO;							break;
				case DFNTAG_CAMPING:			skillToSet = CAMPING;							break;
				case DFNTAG_CARPENTRY:			skillToSet = CARPENTRY;							break;
				case DFNTAG_CARTOGRAPHY:		skillToSet = CARTOGRAPHY;						break;
				case DFNTAG_CHIVALRY:			skillToSet = CHIVALRY;							break;
				case DFNTAG_COOKING:			skillToSet = COOKING;							break;
				case DFNTAG_DEX:				applyTo->SetDexterity( static_cast<SI16>( RandomNum( ndata, odata )));	break;
				case DFNTAG_DETECTINGHIDDEN:	skillToSet = DETECTINGHIDDEN;					break;
				case DFNTAG_DYEHAIR:
					if( ValidateObject( hairobject ))
					{
						hairobject->SetColour( static_cast<UI16>( ndata ));
					}
					break;
				case DFNTAG_DYEBEARD:
					if( ValidateObject( beardobject ))
					{
						beardobject->SetColour( static_cast<UI16>( ndata ));
					}
					break;
				case DFNTAG_ENTICEMENT:			skillToSet = ENTICEMENT;						break;
				case DFNTAG_EVALUATINGINTEL:	skillToSet = EVALUATINGINTEL;					break;
				case DFNTAG_EQUIPITEM:
					retItem = Items->CreateBaseScriptItem( nullptr, cdata, applyTo->WorldNumber(), 1 );
					if( retItem )
					{
						if( !retItem->SetCont( applyTo ))
						{
							retItem->SetCont( applyTo->GetPackItem() );
							retItem->PlaceInPack();
						}
					}
					break;
				case DFNTAG_FAME:				applyTo->SetFame( static_cast<SI16>( ndata ));	break;
				case DFNTAG_FENCING:			skillToSet = FENCING;							break;
				case DFNTAG_FISHING:			skillToSet = FISHING;							break;
				case DFNTAG_FOCUS:				skillToSet = FOCUS;								break;
				case DFNTAG_FORENSICS:			skillToSet = FORENSICS;							break;
				case DFNTAG_HEALING:			skillToSet = HEALING;							break;
				case DFNTAG_HERDING:			skillToSet = HERDING;							break;
				case DFNTAG_HIDING:				skillToSet = HIDING;							break;
				case DFNTAG_IMBUING:			skillToSet = IMBUING;							break;
				case DFNTAG_INTELLIGENCE:		applyTo->SetIntelligence( static_cast<SI16>( RandomNum( ndata, odata )));	break;
				case DFNTAG_ITEMID:				skillToSet = ITEMID;							break;
				case DFNTAG_INSCRIPTION:		skillToSet = INSCRIPTION;						break;
				case DFNTAG_KARMA:				applyTo->SetKarma( static_cast<SI16>( ndata ));	break;
				case DFNTAG_KILLHAIR:
					retItem = applyTo->GetItemAtLayer( IL_HAIR );
					if( ValidateObject( retItem ))
					{
						retItem->Delete();
					}
					break;
				case DFNTAG_KILLBEARD:
					retItem = applyTo->GetItemAtLayer( IL_FACIALHAIR );
					if( ValidateObject( retItem ))
					{
						retItem->Delete();
					}
					break;
				case DFNTAG_KILLPACK:
					retItem = applyTo->GetItemAtLayer( IL_PACKITEM );
					if( ValidateObject( retItem ))
					{
						retItem->Delete();
					}
					break;
				case DFNTAG_LOCKPICKING:		skillToSet = LOCKPICKING;					break;
				case DFNTAG_LUMBERJACKING:		skillToSet = LUMBERJACKING;					break;
				case DFNTAG_MAGERY:				skillToSet = MAGERY;						break;
				case DFNTAG_MAGICRESISTANCE:	skillToSet = MAGICRESISTANCE;				break;
				case DFNTAG_MACEFIGHTING:		skillToSet = MACEFIGHTING;					break;
				case DFNTAG_MEDITATION:			skillToSet = MEDITATION;					break;
				case DFNTAG_MINING:				skillToSet = MINING;						break;
				case DFNTAG_MUSICIANSHIP:		skillToSet = MUSICIANSHIP;					break;
				case DFNTAG_MYSTICISM:			skillToSet = MYSTICISM;						break;
				case DFNTAG_NECROMANCY:			skillToSet = NECROMANCY;					break;
				case DFNTAG_NINJITSU:			skillToSet = NINJITSU;						break;
				case DFNTAG_PARRYING:			skillToSet = PARRYING;						break;
				case DFNTAG_PEACEMAKING:		skillToSet = PEACEMAKING;					break;
				case DFNTAG_POISONING:			skillToSet = POISONING;						break;
				case DFNTAG_PROVOCATION:		skillToSet = PROVOCATION;					break;
				case DFNTAG_POLY:				applyTo->SetId( static_cast<UI16>( ndata ));	break;
				case DFNTAG_PACKITEM:
					if( ValidateObject( applyTo->GetPackItem() ))
					{
						auto csecs = oldstrutil::sections( cdata, "," );
						if( !cdata.empty() )
						{
							if( csecs.size() > 1 )
							{
								retItem = Items->CreateScriptItem( nullptr, applyTo, oldstrutil::trim( oldstrutil::removeTrailing( csecs[0],"//") ), oldstrutil::value<UI16>( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" ))), OT_ITEM, true );
							}
							else
							{
								retItem = Items->CreateScriptItem( nullptr, applyTo, cdata, 1, OT_ITEM, true );
							}
						}
					}
					else
					{
						Console << "Warning: Bad NPC Script with problem no backpack for packitem" << myendl;
					}
					break;
				case DFNTAG_REMOVETRAP:			skillToSet = REMOVETRAP;					break;
				case DFNTAG_STRENGTH:			applyTo->SetStrength( static_cast<SI16>( RandomNum( ndata, odata )));			break;
				case DFNTAG_SKILL:				applyTo->SetBaseSkill( static_cast<UI16>( odata ), static_cast<UI08>( ndata ));	break;
				case DFNTAG_SKIN:				applyTo->SetSkin( static_cast<UI16>( std::stoul( cdata, nullptr, 0 )));			break;
				case DFNTAG_SNOOPING:			skillToSet = SNOOPING;						break;
				case DFNTAG_SPELLWEAVING:		skillToSet = SPELLWEAVING;					break;
				case DFNTAG_SPIRITSPEAK:		skillToSet = SPIRITSPEAK;					break;
				case DFNTAG_STEALING:			skillToSet = STEALING;						break;
				case DFNTAG_STEALTH:			skillToSet = STEALTH;						break;
				case DFNTAG_SWORDSMANSHIP:		skillToSet = SWORDSMANSHIP;					break;
				case DFNTAG_TACTICS:			skillToSet = TACTICS;						break;
				case DFNTAG_TAILORING:			skillToSet = TAILORING;						break;
				case DFNTAG_TAMING:				skillToSet = TAMING;						break;
				case DFNTAG_TASTEID:			skillToSet = TASTEID;						break;
				case DFNTAG_THROWING:			skillToSet = THROWING;						break;
				case DFNTAG_TINKERING:			skillToSet = TINKERING;						break;
				case DFNTAG_TRACKING:			skillToSet = TRACKING;						break;
				case DFNTAG_VETERINARY:			skillToSet = VETERINARY;					break;
				case DFNTAG_WRESTLING:			skillToSet = WRESTLING;						break;
				default:						Console << "Unknown tag in AdvanceObj(): " << static_cast<SI32>( tag ) << myendl;		break;
			}
			if( skillToSet > 0 )
			{
				applyTo->SetBaseSkill( static_cast<UI16>( RandomNum( ndata, odata )), skillToSet );
				skillToSet = 0;	// reset for next time through
			}
		}
		applyTo->Teleport();
	}
	else
	{
		auto sock = applyTo->GetSocket();
		if( sock )
		{
			sock->SysMessage( 1366 ); // You have already used an advancement object with this character.
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	GetClock()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Return CPU time used, Emulates clock()
//o------------------------------------------------------------------------------------------------o
auto GetClock() -> UI32
{
	auto now = std::chrono::system_clock::now();
	return static_cast<UI32>( std::chrono::duration_cast<std::chrono::milliseconds>( now - current ).count() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	IsNumber()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if string is a number, false if not
//o------------------------------------------------------------------------------------------------o
auto IsNumber( const std::string& str ) -> bool
{
	return str.find_first_not_of( "0123456789" ) == std::string::npos;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	DoLight()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets light level for player and applies relevant effects
//o------------------------------------------------------------------------------------------------o
auto DoLight( CSocket *s, UI08 level ) -> void
{
	if( s == nullptr )
		return;

	auto mChar = s->CurrcharObj();
	CPLightLevel toSend( level );

	if(( Races->Affect( mChar->GetRace(), LIGHT )) && mChar->GetWeathDamage( LIGHT ) == 0 )
	{
		mChar->SetWeathDamage( static_cast<UI32>( BuildTimeValue( static_cast<R32>( Races->Secs( mChar->GetRace(), LIGHT )))), LIGHT );
	}

	if( mChar->GetFixedLight() != 255 )
	{
		toSend.Level( mChar->GetFixedLight() );
		s->Send( &toSend );
		Weather->DoPlayerStuff( s, mChar );
		return;
	}

	auto curRegion	= mChar->GetRegion();
	auto wSys = Weather->Weather( curRegion->GetWeather() );
	auto toShow = cwmWorldState->ServerData()->WorldLightCurrentLevel();

	auto dunLevel = cwmWorldState->ServerData()->DungeonLightLevel();
	// we have a valid weather system
	if( wSys )
	{
		const R32 lightMin = wSys->LightMin();
		const R32 lightMax = wSys->LightMax();
		if( lightMin < 300 && lightMax < 300 )
		{
			R32 i = wSys->CurrentLight();
			if( Races->VisLevel( mChar->GetRace() ) > i )
			{
				toShow = 0;
			}
			else
			{
				toShow = static_cast<LIGHTLEVEL>( std::round( i - Races->VisLevel( mChar->GetRace() )));
			}
			toSend.Level( toShow );
		}
		else
		{
			toSend.Level( level );
		}
	}
	else
	{
		if( mChar->InDungeon() )
		{
			if( Races->VisLevel( mChar->GetRace() ) > dunLevel )
			{
				toShow = 0;
			}
			else
			{
				toShow = static_cast<LIGHTLEVEL>( std::round( dunLevel - Races->VisLevel( mChar->GetRace() )));
			}
			toSend.Level( toShow );
		}
	}
	s->Send( &toSend );

	auto eventFound = false;
	auto scriptTriggers = mChar->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		auto toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute )
		{
			if( toExecute->OnLightChange( mChar, toShow ) == 1 )
			{
				// A script with the event returned true; prevent other scripts from running
				eventFound = true;
				break;
			}
		}
	}

	if( !eventFound )
	{
		// Check global script! Maybe there's another event there
		auto toExecute = JSMapping->GetScript( static_cast<UI16>( 0 ));
		if( toExecute )
		{
			toExecute->OnLightChange( mChar, toShow );
		}
	}

	Weather->DoPlayerStuff( s, mChar );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	DoLight()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets light level for character and applies relevant effects
//o------------------------------------------------------------------------------------------------o
auto DoLight( CChar *mChar, UI08 level ) -> void
{
	if(( Races->Affect( mChar->GetRace(), LIGHT )) && ( mChar->GetWeathDamage( LIGHT ) == 0 ))
	{
		mChar->SetWeathDamage( static_cast<UI32>( BuildTimeValue( static_cast<R32>( Races->Secs( mChar->GetRace(), LIGHT )))), LIGHT );
	}

	auto curRegion 	= mChar->GetRegion();
	auto wSys 		= Weather->Weather( curRegion->GetWeather() );

	LIGHTLEVEL toShow = level;
	LIGHTLEVEL dunLevel = cwmWorldState->ServerData()->DungeonLightLevel();

	// we have a valid weather system
	if( wSys )
	{
		const R32 lightMin = wSys->LightMin();
		const R32 lightMax = wSys->LightMax();
		if( lightMin < 300 && lightMax < 300 )
		{
			R32 i = wSys->CurrentLight();
			if( Races->VisLevel( mChar->GetRace() ) > i )
			{
				toShow = 0;
			}
			else
			{
				toShow = static_cast<LIGHTLEVEL>( std::round( i - Races->VisLevel( mChar->GetRace() )));
			}
		}
	}
	else
	{
		if( mChar->InDungeon() )
		{
			if( Races->VisLevel( mChar->GetRace() ) > dunLevel )
			{
				toShow = 0;
			}
			else
			{
				toShow = static_cast<LIGHTLEVEL>( std::round( dunLevel - Races->VisLevel( mChar->GetRace() )));
			}
		}
	}

	bool eventFound = false;
	auto scriptTriggers = mChar->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		auto toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute )
		{
			if( toExecute->OnLightChange( mChar, toShow ) == 1 )
			{
				// A script with the event returned true; prevent other scripts from running
				eventFound = true;
				break;
			}
		}
	}

	if( !eventFound )
	{
		// Check global script! Maybe there's another event there
		auto toExecute = JSMapping->GetScript( static_cast<UI16>( 0 ));
		if( toExecute  )
		{
			toExecute->OnLightChange( mChar, toShow );
		}
	}

	Weather->DoNPCStuff( mChar );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	DoLight()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets light level for items and applies relevant effects
//o------------------------------------------------------------------------------------------------o
auto DoLight( CItem *mItem, UI08 level ) -> void
{
	auto curRegion 	= mItem->GetRegion();
	auto wSys 		= Weather->Weather( curRegion->GetWeather() );

	LIGHTLEVEL toShow = level;
	LIGHTLEVEL dunLevel = cwmWorldState->ServerData()->DungeonLightLevel();

	// we have a valid weather system
	if( wSys )
	{
		const R32 lightMin = wSys->LightMin();
		const R32 lightMax = wSys->LightMax();
		if(( lightMin < 300 ) && ( lightMax < 300 ))
		{
			toShow = static_cast<LIGHTLEVEL>( wSys->CurrentLight() );
		}
	}
	else
	{
		if( mItem->InDungeon() )
		{
			toShow = dunLevel;
		}
	}

	auto eventFound = false;
	auto scriptTriggers = mItem->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		auto toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute )
		{
			if( toExecute->OnLightChange( mItem, toShow ) == 1 )
			{
				// A script with the event returned true; prevent other scripts from running
				eventFound = true;
				break;
			}
		}
	}

	if( !eventFound )
	{
		// Check global script! Maybe there's another event there
		auto toExecute = JSMapping->GetScript( static_cast<UI16>( 0 ));
		if( toExecute )
		{
			toExecute->OnLightChange( mItem, toShow );
		}
	}

	Weather->DoItemStuff( mItem );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	GetPoisonDuration()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculates the duration of poison based on its strength
//o------------------------------------------------------------------------------------------------o
auto GetPoisonDuration( UI08 poisonStrength ) ->TIMERVAL
{
	// Calculate duration of poison, based on the strength of the poison
	auto poisonDuration = TIMERVAL( 0 );
	switch( poisonStrength )
	{
		case 1: // Lesser poison - 9 to 13 pulses, 2 second frequency
			poisonDuration = RandomNum( 9, 13 ) * 2;
			break;
		case 2: // Normal poison - 10 to 14 pulses, 3 second frequency
			poisonDuration = RandomNum( 10, 14 ) * 3;
			break;
		case 3: // Greater poison - 11 to 15 pulses, 4 second frequency
			poisonDuration = RandomNum( 11, 15 ) * 4;
			break;
		case 4: // Deadly poison - 12 to 16 pulses, 5 second frequency
			poisonDuration = RandomNum( 12, 16 ) * 5;
			break;
		case 5: // Lethal poison - 13 to 17 pulses, 5 second frequency
			poisonDuration = RandomNum( 13, 17 ) * 5;
			break;
	}
	return poisonDuration;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	GetPoisonTickTime()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculates the time between each tick of a poison, based on its strength
//o------------------------------------------------------------------------------------------------o
auto GetPoisonTickTime( UI08 poisonStrength )->TIMERVAL
{
	// Calculate duration of poison, based on the strength of the poison
	auto poisonTickTime = TIMERVAL( 0 );
	switch( poisonStrength )
	{
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

//o------------------------------------------------------------------------------------------------o
//|	Function	-	GetTileName()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the length of an items name from tiledata.mul and
//|					sets itemname to the name.
//|					The format it accepts is same as UO style - %plural/single% or %plural%
//|						arrow%s%
//|						loa%ves/f% of bread
//o------------------------------------------------------------------------------------------------o
auto GetTileName( CItem& mItem, std::string& itemname ) -> size_t
{
	std::string temp = mItem.GetName();
	temp = oldstrutil::trim( oldstrutil::removeTrailing( temp, "//" ));
	const UI16 getAmount = mItem.GetAmount();
	CTile& tile = Map->SeekTile( mItem.GetId() );
	if( temp.substr( 0, 1 ) == "#" )
	{
		temp = tile.Name();
	}
	
	if( getAmount == 1 )
	{
		if( tile.CheckFlag( TF_DISPLAYAN ))
		{
			temp = "an " + temp;
		}
		else if( tile.CheckFlag( TF_DISPLAYA ))
		{
			temp = "a " + temp;
		}
	}

	auto psecs = oldstrutil::sections( temp, "%" );
	// Find out if the name has a % in it
	if( psecs.size() > 2 )
	{
		std::string single;
		const std::string first	= psecs[0];
		std::string plural		= psecs[1];
		const std::string rest	= psecs[2];
		auto fssecs = oldstrutil::sections( plural, "/" );
		if( fssecs.size() > 1 )
		{
			single = fssecs[1];
			plural = fssecs[0];
		}
		if( getAmount < 2 )
		{
			temp = first + single + rest;
		}
		else
		{
			temp = first + plural + rest;
		}
	}
	itemname = oldstrutil::simplify( temp );
	return itemname.size() + 1;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	GetNpcDictName()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the dictionary name for a given NPC, if their name equals # or a dictionary ID
//o------------------------------------------------------------------------------------------------o
auto GetNpcDictName( CChar *mChar, CSocket *tSock, UI08 requestSource ) -> std::string
{
	CChar *tChar = nullptr;
	if( tSock )
	{
		tChar = tSock->CurrcharObj();
	}

	std::string dictName = mChar->GetNameRequest( tChar, requestSource );
	SI32 dictEntryId = 0;

	if( dictName == "#" )
	{
		// If character name is #, get dictionary entry based on base dictionary entry for creature names (3000) plus character's ID
		dictEntryId = static_cast<SI32>( 3000 + mChar->GetId() );
		if( tSock )
		{
			dictName = Dictionary->GetEntry( dictEntryId, tSock->Language() );
		}
		else
		{
			dictName = Dictionary->GetEntry( dictEntryId );
		}
	}
	else if( IsNumber( dictName ))
	{
		// If name is a number, assume it's a direct dictionary entry reference, and use that
		dictEntryId = static_cast<SI32>( oldstrutil::value<SI32>( dictName ));
		if( tSock )
		{
			dictName = Dictionary->GetEntry( dictEntryId, tSock->Language() );
		}
		else
		{
			dictName = Dictionary->GetEntry( dictEntryId );
		}
	}

	return dictName;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	GetNpcDictTitle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the dictionary string for the title of a given NPC, if their title 
//|					equals a dictionary ID
//o------------------------------------------------------------------------------------------------o
auto GetNpcDictTitle( CChar *mChar, CSocket *tSock ) -> std::string
{
	std::string dictTitle = mChar->GetTitle();
	SI32 dictEntryId = 0;

	if( !dictTitle.empty() && IsNumber( dictTitle ))
	{
		// If title is a number, assume it's a direct dictionary entry reference, and use that
		dictEntryId = static_cast<SI32>( oldstrutil::value<SI32>( dictTitle ));
		if( tSock )
		{
			dictTitle = Dictionary->GetEntry( dictEntryId, tSock->Language() );
		}
		else
		{
			dictTitle = Dictionary->GetEntry( dictEntryId );
		}
	}

	return dictTitle;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CheckRegion()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check what region a character is in, updating it if necesarry.
//o------------------------------------------------------------------------------------------------o
auto CheckRegion( CSocket *mSock, CChar& mChar, bool forceUpdateLight) -> void
{
	// Get character's old/previous region
	auto iRegion = mChar.GetRegion();
	auto oldSubRegionNum = mChar.GetSubRegion();

	// Calculate character's current region
	auto calcReg = CalcRegionFromXY( mChar.GetX(), mChar.GetY(), mChar.WorldNumber(), mChar.GetInstanceId(), &mChar );

	if(( iRegion == nullptr ) && ( calcReg != nullptr ))
	{
		mChar.SetRegion( calcReg->GetRegionNum() );
	}
	else if( calcReg != iRegion )
	{
		if( mSock )
		{
			if( iRegion != nullptr && calcReg != nullptr )
			{
				// Don't display left/entered region messages if name of region is identical
				if( iRegion->GetName() != calcReg->GetName() )
				{
					if( !iRegion->GetName().empty() )
					{
						mSock->SysMessage( 1358, iRegion->GetName().c_str() ); // You have left %s.
					}
					
					if( !calcReg->GetName().empty() )
					{
						mSock->SysMessage( 1359, calcReg->GetName().c_str() ); // You have entered %s.
					}
				}
				if( calcReg->IsGuarded() || iRegion->IsGuarded() )
				{
					if( calcReg->IsGuarded() )
					{
						// Don't display change of guard message if guardowner is identical
						if( !iRegion->IsGuarded() || ( iRegion->IsGuarded() && calcReg->GetOwner() != iRegion->GetOwner() ))
						{
							if( calcReg->GetOwner().empty() )
							{
								mSock->SysMessage( 1360 ); // You are now under the protection of the guards.
							}
							else
							{
								mSock->SysMessage( 1361, calcReg->GetOwner().c_str() ); // You are now under the protection of %s guards.
							}
						}
					}
					else
					{
						if( iRegion->GetOwner().empty() )
						{
							mSock->SysMessage( 1362 ); // You are no longer under the protection of the guards.
						}
						else
						{
							mSock->SysMessage( 1363, iRegion->GetOwner().c_str() ); // You are no longer under the protection of %s guards.
						}
					}
					UpdateFlag( &mChar );
				}
				if( calcReg->GetAppearance() != iRegion->GetAppearance() ) // if the regions look different
				{
					CPWorldChange wrldChange( calcReg->GetAppearance(), 1 );
					mSock->Send( &wrldChange );
				}
				if( calcReg == cwmWorldState->townRegions[mChar.GetTown()] ) // enter our home town
				{
					mSock->SysMessage( 1364 ); // You feel loved and cherished under the protection of your home town.
					CItem *packItem = mChar.GetPackItem();
					if( ValidateObject( packItem ))
					{
						auto piCont = packItem->GetContainsList();
						for( const auto &toScan : piCont->collection() )
						{
							if( ValidateObject( toScan ))
							{
								if( toScan->GetType() == IT_TOWNSTONE )
								{
									CTownRegion *targRegion = cwmWorldState->townRegions[static_cast<UI16>( toScan->GetTempVar( CITV_MOREX ))];
									mSock->SysMessage( 1365, targRegion->GetName().c_str() ); // You have successfully returned the townstone of %s to your home town.
									targRegion->DoDamage( targRegion->GetHealth() );	// finish it off
									targRegion->Possess( calcReg );
									mChar.SetFame( static_cast<SI16>( mChar.GetFame() + mChar.GetFame() / 5 ));	// 20% fame boost
									break;
								}
							}
						}
					}
				}
			}
		}
		if( iRegion != nullptr && calcReg != nullptr )
		{
			// Run onLeaveRegion/onEnterRegion for character
			auto scriptTriggers = mChar.GetScriptTriggers();
			for( auto scriptTrig : scriptTriggers )
			{
				auto toExecute = JSMapping->GetScript( scriptTrig );
				if( toExecute)
				{
					toExecute->OnLeaveRegion( &mChar, iRegion->GetRegionNum() );
					toExecute->OnEnterRegion( &mChar, calcReg->GetRegionNum() );
				}
			}

			// Run onLeaveRegion event for region being left
			scriptTriggers.clear();
			scriptTriggers.shrink_to_fit();
			scriptTriggers = iRegion->GetScriptTriggers();
			for( auto scriptTrig : scriptTriggers )
			{
				auto toExecute = JSMapping->GetScript( scriptTrig );
				if( toExecute )
				{
					toExecute->OnLeaveRegion( &mChar, iRegion->GetRegionNum() );
				}
			}

			// Run onEnterRegion event for region being entered
			scriptTriggers.clear();
			scriptTriggers.shrink_to_fit();
			scriptTriggers = calcReg->GetScriptTriggers();
			for( auto scriptTrig : scriptTriggers )
			{
				auto toExecute = JSMapping->GetScript( scriptTrig );
				if( toExecute )
				{
					toExecute->OnEnterRegion( &mChar, calcReg->GetRegionNum() );
				}
			}
		}
		if( calcReg )
		{
			mChar.SetRegion( calcReg->GetRegionNum() );
		}
		if( mSock )
		{
			Effects->DoSocketMusic( mSock );
			DoLight( mSock, cwmWorldState->ServerData()->WorldLightCurrentLevel() );
		}
	}
	else
	{
		// Main region didn't change, but subregion did! Update music
		if( oldSubRegionNum != mChar.GetSubRegion() )
		{
			Effects->DoSocketMusic( mSock );
		}

		// Update lighting
		if( forceUpdateLight && mSock != nullptr )
		{
			DoLight( mSock, cwmWorldState->ServerData()->WorldLightCurrentLevel() );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CheckCharInsideBuilding()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if a character is inside a building before applying weather effects
//o------------------------------------------------------------------------------------------------o
auto CheckCharInsideBuilding( CChar *c, CSocket *mSock, bool doWeatherStuff ) -> void
{
	if( !c->GetMounted() && !c->GetStabled() )
	{
		auto wasInBuilding = c->InBuilding();
		bool isInBuilding = Map->InBuilding( c->GetX(), c->GetY(), c->GetZ(), c->WorldNumber(), c->GetInstanceId() );
		if( wasInBuilding != isInBuilding )
		{
			c->SetInBuilding( isInBuilding );
			if( doWeatherStuff )
			{
				if( c->IsNpc() )
				{
					Weather->DoNPCStuff( c );
				}
				else
				{
					Weather->DoPlayerStuff( mSock, c );
				}
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	WillResultInCriminal()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check flagging, race, and guild info to find if character
//|					should be flagged criminal (returns true if so)
//o------------------------------------------------------------------------------------------------o
auto WillResultInCriminal( CChar *mChar, CChar *targ ) -> bool
{
	auto tOwner = targ->GetOwnerObj();
	auto mOwner = mChar->GetOwnerObj();
	auto mCharParty = PartyFactory::GetSingleton().Get( mChar );
	auto rValue = false;
	if( ValidateObject( mChar ) && ValidateObject( targ ) && mChar != targ )
	{
		// Make sure they're not racial enemies, or guild members/guild enemies
		if(( Races->Compare( mChar, targ ) > RACE_ENEMY ) && GuildSys->ResultInCriminal( mChar, targ ))
		{
			// Make sure they're not in the same party
			if( !mCharParty || mCharParty->HasMember( targ ))
			{
				// Make sure the target is not the aggressor in the fight
				if( !targ->CheckAggressorFlag( mChar->GetSerial() ))
				{
					// Make sure target doesn't have an owner  
					if( !ValidateObject( tOwner ))
					{
						// Make sure attacker doesn't have an owner
						if( !ValidateObject( mOwner ))
						{
							// Make sure target is innocent
							if( targ->IsInnocent() )
							{
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

//o------------------------------------------------------------------------------------------------o
//|	Function	-	MakeCriminal()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Make character a criminal
//o------------------------------------------------------------------------------------------------o
auto MakeCriminal( CChar *c ) -> void
{
	c->SetTimer( tCHAR_CRIMFLAG, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_CRIMINAL ));
	if( !c->IsCriminal() && !c->IsMurderer() )
	{
		auto cSock = c->GetSocket();
		if( cSock )
		{
			cSock->SysMessage( 1379 ); // You are now a criminal!
		}
		UpdateFlag( c );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	FlagForStealing()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Flag character for stealing
//o------------------------------------------------------------------------------------------------o
auto FlagForStealing( CChar *c ) -> void
{
	c->SetTimer( tCHAR_STEALFLAG, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_STEALINGFLAG ));
	if( !c->IsCriminal() && !c->IsMurderer() && !c->HasStolen() )
	{
		c->HasStolen( true );
		UpdateFlag( c );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	UpdateFlag()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Updates character flags
//o------------------------------------------------------------------------------------------------o
auto UpdateFlag( CChar *mChar ) -> void
{
	if( !ValidateObject( mChar ))
		return;

	UI08 oldFlag = mChar->GetFlag();

	if( mChar->IsTamed() )
	{
		CChar *i = mChar->GetOwnerObj();
		if( ValidateObject( i ))
		{
			// Set character's flag to match owner's flag
			mChar->SetFlag( i->GetFlag() );
		}
		else
		{
			// Default to blue, invalid owner detected
			mChar->SetFlagBlue();
			Console.Warning( oldstrutil::format( "Tamed Creature has an invalid owner, Serial: 0x%X", mChar->GetSerial() ));
		}
	}
	else
	{
		if( mChar->GetKills() > cwmWorldState->ServerData()->RepMaxKills() )
		{
			// Character is flagged as a murderer
			mChar->SetFlagRed();
		}
		else if(( mChar->GetTimer( tCHAR_CRIMFLAG ) != 0 || mChar->GetTimer( tCHAR_STEALFLAG ) != 0 ) && ( mChar->GetNPCFlag() != fNPC_EVIL ))
		{
			// Character is flagged as criminal or for stealing
			mChar->SetFlagGray();
		}
		else
		{
			if( mChar->IsNpc() )
			{
				auto doSwitch = true;
				if( cwmWorldState->creatures[mChar->GetId()].IsAnimal() && ( mChar->GetNpcAiType() != AI_EVIL && mChar->GetNpcAiType() != AI_EVIL_CASTER ))
				{
					if( cwmWorldState->ServerData()->CombatAnimalsGuarded() && mChar->GetRegion()->IsGuarded() )
					{
						mChar->SetFlagBlue();
						doSwitch = false;
					}
				}

				if( doSwitch )
				{
					switch( mChar->GetNPCFlag() )
					{
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
			else
			{
				mChar->SetFlagBlue();
			}
		}
	}

	UI08 newFlag = mChar->GetFlag();
	if( oldFlag != newFlag )
	{
		auto scriptTriggers = mChar->GetScriptTriggers();
		for( auto scriptTrig : scriptTriggers )
		{
			auto toExecute = JSMapping->GetScript( scriptTrig );
			if( toExecute )
			{
				if( toExecute->OnFlagChange( mChar, newFlag, oldFlag ) == 1 )
				{
					break;
				}
			}
		}

		mChar->Dirty( UT_UPDATE );
	}

	if( !mChar->IsNpc() )
	{
		// Flag was updated, so loop through player's corpses so flagging can be updated for those!
		for( auto tempCorpse = mChar->GetOwnedCorpses()->First(); !mChar->GetOwnedCorpses()->Finished(); tempCorpse = mChar->GetOwnedCorpses()->Next() )
		{
			if( ValidateObject( tempCorpse ))
			{
				tempCorpse->Dirty( UT_UPDATE );
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SendMapChange()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Send mapchange packet to client to teleport player to new world/map
//o------------------------------------------------------------------------------------------------o
void SendMapChange( UI08 worldNumber, CSocket *sock, [[maybe_unused]] bool initialLogin )
{
	if( sock )
	{
		CPMapChange mapChange( worldNumber );
		sock->Send( &mapChange );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SocketMapChange()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if conditions are right to send a map change packet to the client
//o------------------------------------------------------------------------------------------------o
auto SocketMapChange( CSocket *sock, CChar *charMoving, CItem *gate ) -> void
{
	if( !sock )
		return;

	if( !ValidateObject( gate ) || !ValidateObject( charMoving ))
		return;

	UI08 tWorldNum = static_cast<UI08>( gate->GetTempVar( CITV_MORE ));
	UI16 tInstanceId = gate->GetInstanceId();
	if( !Map->MapExists( tWorldNum ))
		return;

	CChar *toMove = nullptr;
	if( ValidateObject( charMoving ))
	{
		toMove = charMoving;
	}
	else
	{
		toMove = sock->CurrcharObj();
	}
	if( !ValidateObject( toMove ))
		return;

	// Teleport followers to new location too!
	auto myFollowers = toMove->GetFollowerList();
	for( CChar *myFollower = myFollowers->First(); !myFollowers->Finished(); myFollower = myFollowers->Next() )
	{
		if( ValidateObject( myFollower ))
		{
			if( !myFollower->GetMounted() && myFollower->GetOwnerObj() == toMove )
			{
				if( myFollower->GetNpcWander() == WT_FOLLOW && ObjInOldRange( toMove, myFollower, DIST_CMDRANGE ))
				{
					myFollower->SetLocation( static_cast<SI16>( gate->GetTempVar( CITV_MOREX )), 
										static_cast<SI16>( gate->GetTempVar( CITV_MOREY )), 
										static_cast<SI08>( gate->GetTempVar( CITV_MOREZ )), tWorldNum, tInstanceId );
				}
			}
		}
	}

	switch( sock->ClientType() )
	{
		case CV_UO3D:
		case CV_KRRIOS:
			toMove->SetLocation( static_cast<SI16>( gate->GetTempVar( CITV_MOREX )), 
								static_cast<SI16>( gate->GetTempVar( CITV_MOREY )), 
								static_cast<SI08>( gate->GetTempVar( CITV_MOREZ )), tWorldNum, tInstanceId );
			break;
		default:
			toMove->SetLocation( static_cast<SI16>( gate->GetTempVar( CITV_MOREX )), 
								static_cast<SI16>( gate->GetTempVar( CITV_MOREY )), 
								static_cast<SI08>( gate->GetTempVar( CITV_MOREZ )), tWorldNum, tInstanceId );
			break;
	}
	SendMapChange( tWorldNum, sock );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	DoorMacro()
//|	Date		-	11th October, 1999
//|	Changes		-	(support CSocket *s and door blocking)
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Door use macro support.
//o------------------------------------------------------------------------------------------------o
void DoorMacro( CSocket *s )
{
	CChar *mChar = s->CurrcharObj();
	SI16 xc = mChar->GetX(), yc = mChar->GetY();
	switch( mChar->GetDir() )
	{
		case 0 : --yc;				break;
		case 1 : { ++xc; --yc; }	break;
		case 2 : ++xc;				break;
		case 3 : { ++xc; ++yc; }	break;
		case 4 : ++yc;				break;
		case 5 : { --xc; ++yc; }	break;
		case 6 : --xc;				break;
		case 7 : { --xc; --yc; }	break;
	}

	for( auto &toCheck : MapRegion->PopulateList( mChar ))
	{
		if( !toCheck )
			continue;

		auto regItems = toCheck->GetItemList();
		for( const auto &itemCheck : regItems->collection() )
		{
			if( !ValidateObject( itemCheck ) || itemCheck->GetInstanceId() != mChar->GetInstanceId() )
				continue;

			SI16 distZ = abs( itemCheck->GetZ() - mChar->GetZ() );
			if( itemCheck->GetX() == xc && itemCheck->GetY() == yc && distZ < 7 )
			{
				if( itemCheck->GetType() == IT_DOOR || itemCheck->GetType() == IT_LOCKEDDOOR )
				{
					// only open doors
					if( JSMapping->GetEnvokeByType()->Check( static_cast<UI16>( itemCheck->GetType() )))
					{
						UI16 envTrig = JSMapping->GetEnvokeByType()->GetScript( static_cast<UI16>( itemCheck->GetType() ));
						auto envExecute = JSMapping->GetScript( envTrig );
						if( envExecute )
						{
							[[maybe_unused]] SI08 retVal = envExecute->OnUseChecked( mChar, itemCheck );
						}

						return;
					}
				}
			}
		}
	}
}


