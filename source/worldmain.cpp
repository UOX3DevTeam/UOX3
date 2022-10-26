//o------------------------------------------------------------------------------------------------o
//|	File		-	worldmain.cpp
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Version History
//|
//|						1.0
//|						Initial implementation.
//|
//|						1.1		March 12, 2003
//|						Most global variables moved from uox3.h to the CWorldMain class
//|
//|						1.2		October 16, 2003
//|						Added quite a few more vectors to remove them from global scope
//|						Removed many members that were no longer necesarry and moved
//|							others to more appropriate locations.
//|						Added a proper constructor rather than ResetDefaults()
//|						Grouped timers together in an array using an enum.
//o------------------------------------------------------------------------------------------------o
#include "uox3.h"
#include "cGuild.h"
#include "townregion.h"
#include "cSpawnRegion.h"
#include "skills.h"
#include "speech.h"
#include "cEffects.h"
#include "network.h"
#include "regions.h"
#include "jail.h"
#include "Dictionary.h"
#include "ObjectFactory.h"
#include "osunique.hpp"

CWorldMain *cwmWorldState = nullptr;

//o------------------------------------------------------------------------------------------------o
//| CWorldMain Constructor & Destructor
//o------------------------------------------------------------------------------------------------o
const bool			DEFWORLD_KEEPRUN			= true;
const bool			DEFWORLD_ERROR				= false;
const bool			DEFWORLD_SECURE				= true;

const bool			DEFWORLD_LOADED				= false;
const UI32			DEFWORLD_UICURRENTTIME		= 0;
const UI32			DEFWORLD_UOTICKCOUNT		= 1;
const bool			DEFWORLD_OVERFLOW			= false;
const UI32			DEFWORLD_STARTTIME			= 0;
const UI32			DEFWORLD_ENDTIME			= 0;
const UI32			DEFWORLD_LCLOCK				= 0;
const size_t		DEFWORLD_PLAYERSONLINE		= 0;
const UI32			DEFWORLD_NEWTIME			= 0;
const UI32			DEFWORLD_OLDTIME			= 0;
const bool			DEFWORLD_AUTOSAVED			= false;
const SaveStatus	DEFWORLD_SAVEPROGRESS		= SS_NOTSAVING;
const bool			DEFWORLD_RELOADINGSCRIPTS	= false;
const bool			DEFWORLD_CLASSESINITIALIZED	= false;

CWorldMain::CWorldMain() : error( DEFWORLD_ERROR ),
keepRun( DEFWORLD_KEEPRUN ), secure( DEFWORLD_SECURE ), hasLoaded( DEFWORLD_LOADED ),
uoTickCount( DEFWORLD_UOTICKCOUNT ), startTime( DEFWORLD_STARTTIME ), endTime( DEFWORLD_ENDTIME ), lClock( DEFWORLD_LCLOCK ),
overflow( DEFWORLD_OVERFLOW ), uiCurrentTime( DEFWORLD_UICURRENTTIME ), oldTime( DEFWORLD_OLDTIME ), newTime( DEFWORLD_NEWTIME ),
autoSaved( DEFWORLD_AUTOSAVED ), worldSaveProgress( DEFWORLD_SAVEPROGRESS ), playersOnline( DEFWORLD_PLAYERSONLINE ),
reloadingScripts( DEFWORLD_RELOADINGSCRIPTS ), classesInitialized( DEFWORLD_CLASSESINITIALIZED )
{
	sData = nullptr;
	
	for( SI32 mTID = static_cast<SI32>( tWORLD_NEXTFIELDEFFECT ); mTID < static_cast<SI32>( tWORLD_COUNT ); ++mTID )
	{
		worldTimers[mTID] = 0;
	}
	creatures.clear();
	prowessTitles.resize( 0 );
	murdererTags.resize( 0 );
	teleLocs.resize( 0 );
	escortRegions.resize( 0 );
	logoutLocs.resize( 0 );
	sosLocs.resize( 0 );
	goPlaces.clear();
	refreshQueue.clear();
	deletionQueue.clear();
	spawnRegions.clear();
	uoxTimeout.tv_sec	= 0;
	uoxTimeout.tv_usec	= 0;	
}
//==================================================================================================
auto CWorldMain::Startup() -> void
{
}

//==================================================================================================
auto CWorldMain::SetServerData( CServerData &server_data ) -> void
{	
	sData = &server_data;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetTimer()
//|					CWorldMain::SetTimer()
//|	Date		-	10/17/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles all the world timers (next time we check NPC AI, etc)
//o------------------------------------------------------------------------------------------------o
TIMERVAL CWorldMain::GetTimer( CWM_TID timerId ) const
{
	TIMERVAL rValue = 0;
	if( timerId != tWORLD_COUNT )
	{
		rValue = worldTimers[timerId];
	}
	return rValue;
}
void CWorldMain::SetTimer( CWM_TID timerId, TIMERVAL newVal )
{
	if( timerId != tWORLD_COUNT )
	{
		worldTimers[timerId] = newVal;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetError()
//|					CWorldMain::SetError()
//|	Date		-	3/12/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets if we have generated an error
//o------------------------------------------------------------------------------------------------o
bool CWorldMain::GetError( void ) const
{
	return error;
}
void CWorldMain::SetError( bool newVal )
{
	error = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetKeepRun()
//|					CWorldMain::SetKeepRun()
//|	Date		-	3/12/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets if server should be kept running
//o------------------------------------------------------------------------------------------------o
bool CWorldMain::GetKeepRun( void ) const
{
	return keepRun;
}
void CWorldMain::SetKeepRun( bool newVal )
{
	keepRun = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetSecure()
//|					CWorldMain::SetSecure()
//|	Date		-	3/12/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether console "secure" mode is enabled/disabled
//o------------------------------------------------------------------------------------------------o
bool CWorldMain::GetSecure( void ) const
{
	return secure;
}
void CWorldMain::SetSecure( bool newVal )
{
	secure = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetLoaded() const
//|					CWorldMain::SetLoaded()
//|	Date		-	3/12/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether UOX has been loaded
//o------------------------------------------------------------------------------------------------o
bool CWorldMain::GetLoaded( void ) const
{
	return hasLoaded;
}
void CWorldMain::SetLoaded( bool newVal )
{
	hasLoaded = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetUICurrentTime()
//|					CWorldMain::SetUICurrentTime()
//|	Date		-	3/12/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets current time
//o------------------------------------------------------------------------------------------------o
UI32 CWorldMain::GetUICurrentTime( void ) const
{
	return uiCurrentTime;
}
void CWorldMain::SetUICurrentTime( UI32 newVal )
{
	uiCurrentTime = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetUOTickCount()
//|					CWorldMain::SetUOTickCount()
//|	Date		-	3/12/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets UO Minutes
//o------------------------------------------------------------------------------------------------o
UI32 CWorldMain::GetUOTickCount( void ) const
{
	return uoTickCount;
}
void CWorldMain::SetUOTickCount( UI32 newVal )
{
	uoTickCount = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetOverflow()
//|					CWorldMain::SetOverflow()
//|	Date		-	3/12/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether we overflowed the time
//o------------------------------------------------------------------------------------------------o
bool CWorldMain::GetOverflow( void ) const
{
	return overflow;
}
void CWorldMain::SetOverflow( bool newVal )
{
	overflow = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetStartTime()
//|					CWorldMain::SetStartTime()
//|	Date		-	3/12/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets time when server started up
//o------------------------------------------------------------------------------------------------o
UI32 CWorldMain::GetStartTime( void ) const
{
	return startTime;
}
void CWorldMain::SetStartTime( UI32 newVal )
{
	startTime = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetEndTime()
//|					CWorldMain::SetEndTime()
//|	Date		-	3/12/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets time when server will shutdown
//o------------------------------------------------------------------------------------------------o
UI32 CWorldMain::GetEndTime( void ) const
{
	return endTime;
}
void CWorldMain::SetEndTime( UI32 newVal )
{
	endTime = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetLClock()
//|					CWorldMain::SetLClock()
//|	Date		-	3/12/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets end time
//o------------------------------------------------------------------------------------------------o
UI32 CWorldMain::GetLClock( void ) const
{
	return lClock;
}
void CWorldMain::SetLClock( UI32 newVal )
{
	lClock = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetNewTime()
//|					CWorldMain::SetNewTime()
//|	Date		-	3/12/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets time for next auto worldsave
//o------------------------------------------------------------------------------------------------o
UI32 CWorldMain::GetNewTime( void ) const
{
	return newTime;
}
void CWorldMain::SetNewTime( UI32 newVal )
{
	newTime = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetOldTime()
//|					CWorldMain::SetOldTime()
//|	Date		-	3/12/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets time of last auto worldsave
//o------------------------------------------------------------------------------------------------o
UI32 CWorldMain::GetOldTime( void ) const
{
	return oldTime;
}
void CWorldMain::SetOldTime( UI32 newVal )
{
	oldTime = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetAutoSaved()
//|					CWorldMain::SetAutoSaved()
//|	Date		-	3/12/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether world was autoSaved
//o------------------------------------------------------------------------------------------------o
bool CWorldMain::GetAutoSaved( void ) const
{
	return autoSaved;
}
void CWorldMain::SetAutoSaved( bool newVal )
{
	autoSaved = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetWorldSaveProgress()
//|					CWorldMain::SetWorldSaveProgress()
//|	Date		-	3/12/2003
//|	Changes		-	10/10/2003 - Now uses enum SaveStatus for ease of use
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets status of World saves (Not Saving, Saving, Just Saved)
//o------------------------------------------------------------------------------------------------o
SaveStatus CWorldMain::GetWorldSaveProgress( void ) const
{
	return worldSaveProgress;
}
void CWorldMain::SetWorldSaveProgress( SaveStatus newVal )
{
	worldSaveProgress = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetPlayersOnline()
//|					CWorldMain::SetPlayersOnline()
//|	Date		-	3/12/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets total Players Online
//o------------------------------------------------------------------------------------------------o
size_t CWorldMain::GetPlayersOnline( void ) const
{
	return playersOnline;
}
void CWorldMain::SetPlayersOnline( size_t newVal )
{
	playersOnline = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::DecPlayersOnline()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Decrements the counter for players online
//o------------------------------------------------------------------------------------------------o
void CWorldMain::DecPlayersOnline( void )
{
	if( playersOnline == 0 )
	{
		throw std::runtime_error( "Decrementing a 0 count which will rollover" );
	}
	--playersOnline;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetReloadingScripts()
//|					CWorldMain::SetReloadingScripts()
//|	Date		-	6/22/2004
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether server is reloading its scripts
//o------------------------------------------------------------------------------------------------o
bool CWorldMain::GetReloadingScripts( void ) const
{
	return reloadingScripts;
}
void CWorldMain::SetReloadingScripts( bool newVal )
{
	reloadingScripts = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::CheckTimers()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check shutdown timers
//o------------------------------------------------------------------------------------------------o
void CWorldMain::CheckTimers( void )
{
	SetOverflow(( GetLClock() > GetUICurrentTime() ));
	if( GetEndTime() )
	{
		if( GetEndTime() <= GetUICurrentTime() )
		{
			SetKeepRun( false );
		}
	}
	SetLClock( GetUICurrentTime() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetNewIPTime()
//|					CWorldMain::SetNewIPTime()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets time for next auto IP update
//o------------------------------------------------------------------------------------------------o
UI32 CWorldMain::GetNewIPTime( void ) const
{
	return newIPtime;
}
void CWorldMain::SetNewIPTime( UI32 newVal )
{
	newIPtime = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetOldIPTime()
//|					CWorldMain::SetOldIPTime()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets time of last auto IP update
//o------------------------------------------------------------------------------------------------o
UI32 CWorldMain::GetOldIPTime( void ) const
{
	return oldIPtime;
}
void CWorldMain::SetOldIPTime( UI32 newVal )
{
	oldIPtime = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetIPUpdated()
//|					CWorldMain::SetIPUpdated()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether IPs have been updated
//o------------------------------------------------------------------------------------------------o
bool CWorldMain::GetIPUpdated( void ) const
{
	return ipUpdated;
}
void CWorldMain::SetIPUpdated( bool newVal )
{
	ipUpdated = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::ClassesInitialized()
//|					CWorldMain::ClassesInitialized()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether base classes been initialized (in case of early shut down)
//o------------------------------------------------------------------------------------------------o
bool CWorldMain::ClassesInitialized( void ) const
{
	return classesInitialized;
}
void CWorldMain::ClassesInitialized( bool newVal )
{
	classesInitialized = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::DoWorldLight()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Set world light level based on time of day and script settings
//o------------------------------------------------------------------------------------------------o
void CWorldMain::DoWorldLight( void )
{
	UI08 worlddarklevel		= ServerData()->WorldLightDarkLevel();
	UI08 worldbrightlevel	= ServerData()->WorldLightBrightLevel();
	bool ampm				= ServerData()->ServerTimeAMPM();
	UI08 currentHour		= ServerData()->ServerTimeHours();
	UI08 currentMinute		= ServerData()->ServerTimeMinutes();

	R32 currentTime			= R32( currentHour + ( currentMinute / 60.0f ));
	R32 hourIncrement		= R32( fabs(( worlddarklevel - worldbrightlevel ) / 12.0f )); // we want the amount to subtract from LightMax in the morning / add to LightMin in evening

	if( ampm )
	{
		ServerData()->WorldLightCurrentLevel( static_cast<UI08>( RoundNumber( worldbrightlevel + ( hourIncrement * currentTime ))));
	}
	else
	{
		ServerData()->WorldLightCurrentLevel( static_cast<UI08>( RoundNumber( worlddarklevel - ( hourIncrement * currentTime ))));
	}
}

void FileArchive( void );
void CollectGarbage( void );
void SysBroadcast( const std::string& txt );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::SaveNewWorld()
//|	Date		-	1997
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Saves the UOX world
//|
//|	Changes		-	10/21/2002	-	fix for archiving. Now it wont always archive :)
//o------------------------------------------------------------------------------------------------o
void CWorldMain::SaveNewWorld( bool x )
{
	static UI32 save_counter = 0;

	std::for_each( cwmWorldState->spawnRegions.begin(), cwmWorldState->spawnRegions.end(), []( std::pair<UI16, CSpawnRegion*> entry )
	{
		if( entry.second )
		{
			entry.second->CheckSpawned();
		}
	});

	if( GetWorldSaveProgress() != SS_SAVING )
	{
		SetWorldSaveProgress( SS_SAVING );
		Console.PrintSectionBegin();
		if( ServerData()->ServerAnnounceSavesStatus() )
		{
			SysBroadcast( Dictionary->GetEntry( 1615 )); // World data saving, you may experience some lag for the next several minutes.
			SpeechSys->Poll();
		}
		Network->ClearBuffers();

		if( x )
		{
			Console << "Starting manual world data save...." << myendl;
		}
		else
		{
			Console << "Starting automatic world data save...." << myendl;
		}

		if( ServerData()->ServerBackupStatus() && ServerData()->Directory( CSDDP_BACKUP ).length() > 1 )
		{
			++save_counter;
			if(( save_counter % ServerData()->BackupRatio() ) == 0 )
			{
				Console << "Archiving world files." << myendl;
				FileArchive();
			}
		}
		Console << "Saving Misc. data... ";
		ServerData()->SaveIni();
		Console.Log( "Server data save", "server.log" );
		RegionSave();
		Console.PrintDone();
		MapRegion->Save();
		GuildSys->Save();
		JailSys->WriteData();
		Effects->SaveEffects();
		ServerData()->SaveTime();
		SaveStatistics();

		if( ServerData()->ServerAnnounceSavesStatus() )
		{
			SysBroadcast( "World Save Complete." );
		}

		//	If accounts are to be loaded then they should be loaded
		//	all the time if using the web interface
		Accounts->Save();
		// Make sure to import the new accounts so they have access too.
		Console << "New accounts processed: " << Accounts->ImportAccounts() << myendl;
		SetWorldSaveProgress( SS_JUSTSAVED );

		char saveTimestamp[100];
		time_t tempTimestamp = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
		struct tm curtime;
		lcltime( tempTimestamp, curtime );
		strftime( saveTimestamp, 50, "%F at %T", &curtime );

		Console << "World save complete on " << saveTimestamp << myendl;
		Console.PrintSectionBegin();
	}
	CollectGarbage();
	uiCurrentTime = GetClock();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::RegionSave()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loops through all town regions and saves them to disk
//o------------------------------------------------------------------------------------------------o
void CWorldMain::RegionSave()
{
	std::string regionsFile	= cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "regions.wsc";
	std::ofstream regionsDestination( regionsFile.c_str() );
	if( !regionsDestination )
	{
		Console.Error( oldstrutil::format( "Failed to open %s for writing", regionsFile.c_str() ));
		return;
	}
	std::for_each( cwmWorldState->townRegions.begin(), cwmWorldState->townRegions.end(), [&regionsDestination]( const std::pair<UI16, CTownRegion*> &town )
	{
		if( town.second )
		{
			town.second->Save( regionsDestination );
		}
	});
	regionsDestination.close();
}

//==================================================================================================
auto CWorldMain::ServerData()->CServerData *
{
	return sData;
}

//==================================================================================================
auto CWorldMain::ServerProfile() -> CServerProfile *
{
	return &sProfile;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::SaveStatistics()
//|	Date		-	February 5th, 2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Saves out some useful statistics so that some tools
//|						such as WorldBuilder can do some memory reserve shortcuts
//o------------------------------------------------------------------------------------------------o
void CWorldMain::SaveStatistics( void )
{
	std::string		statsFile = cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "statistics.wsc";
	std::ofstream	statsDestination( statsFile.c_str() );
	if( !statsDestination )
	{
		Console.Error( oldstrutil::format( "Failed to open %s for writing", statsFile.c_str() ));
		return;
	}
	statsDestination << "[STATISTICS]" << '\n' << "{" << '\n';
	statsDestination << "PLAYERCOUNT=" << ObjectFactory::GetSingleton().CountOfObjects( OT_CHAR ) << '\n';
	statsDestination << "ITEMCOUNT=" << ObjectFactory::GetSingleton().CountOfObjects( OT_ITEM ) << '\n';
	statsDestination << "MULTICOUNT=" << ObjectFactory::GetSingleton().CountOfObjects( OT_MULTI ) << '\n';
	statsDestination << "}" << '\n' << '\n';

	statsDestination.close();
}

//o------------------------------------------------------------------------------------------------o
//|	Class		-	CWorldMain::CServerProfile()
//|	Date		-	2004
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Version History
//|
//|						1.0		 		2004
//|						Original implementation
//|						Moving all profiling variables into their own class managed
//|						by CWorldMain in order to take them out of global scope
//o------------------------------------------------------------------------------------------------o
CServerProfile::CServerProfile() : networkTime( 0 ), timerTime( 0 ), autoTime( 0 ), loopTime( 0 ),
networkTimeCount( 1000 ), timerTimeCount( 1000 ), autoTimeCount( 1000 ), loopTimeCount( 1000 ),
globalRecv( 0 ), globalSent( 0 )
{
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerProfile::NetworkTime()
//|					CServerProfile::IncNetworkTime()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets/Increments networkTime
//o------------------------------------------------------------------------------------------------o
UI32 CServerProfile::NetworkTime( void ) const
{
	return networkTime;
}
void CServerProfile::NetworkTime( UI32 newVal )
{
	networkTime = newVal;
}
void CServerProfile::IncNetworkTime( UI32 toInc )
{
	networkTime += toInc;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerProfile::TimerTime()
//|					CServerProfile::IncTimerTime()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets/Increments timerTime
//o------------------------------------------------------------------------------------------------o
UI32 CServerProfile::TimerTime( void ) const
{
	return timerTime;
}
void CServerProfile::TimerTime( UI32 newVal )
{
	timerTime = newVal;
}
void CServerProfile::IncTimerTime( UI32 toInc )
{
	timerTime += toInc;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerProfile::AutoTime()
//|					CServerProfile::IncAutoTime()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets/Increments autoTime
//o------------------------------------------------------------------------------------------------o
UI32 CServerProfile::AutoTime( void ) const
{
	return autoTime;
}
void CServerProfile::AutoTime( UI32 newVal )
{
	autoTime = newVal;
}
void CServerProfile::IncAutoTime( UI32 toInc )
{
	autoTime += toInc;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerProfile::LoopTime()
//|					CServerProfile::IncLoopTime()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets/Increments loopTime
//o------------------------------------------------------------------------------------------------o
UI32 CServerProfile::LoopTime( void ) const
{
	return loopTime;
}
void CServerProfile::LoopTime( UI32 newVal )
{
	loopTime = newVal;
}
void CServerProfile::IncLoopTime( UI32 toInc )
{
	loopTime += toInc;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerProfile::NetworkTimeCount()
//|					CServerProfile::IncNetworkTimeCount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets/Increments networkTimeCount
//o------------------------------------------------------------------------------------------------o
UI32 CServerProfile::NetworkTimeCount( void ) const
{
	return networkTimeCount;
}
void CServerProfile::NetworkTimeCount( UI32 newVal )
{
	networkTimeCount = newVal;
}
void CServerProfile::IncNetworkTimeCount( void )
{
	++networkTimeCount;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerProfile::TimerTimeCount()
//|					CServerProfile::IncTimerTimeCount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets/Increments timerTimeCount
//o------------------------------------------------------------------------------------------------o
UI32 CServerProfile::TimerTimeCount( void ) const
{
	return timerTimeCount;
}
void CServerProfile::TimerTimeCount( UI32 newVal )
{
	timerTimeCount = newVal;
}
void CServerProfile::IncTimerTimeCount( void )
{
	++timerTimeCount;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerProfile::AutoTimeCount()
//|					CServerProfile::IncAutoTimeCount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets/Increments autoTimeCount
//o------------------------------------------------------------------------------------------------o
UI32 CServerProfile::AutoTimeCount( void ) const
{
	return autoTimeCount;
}
void CServerProfile::AutoTimeCount( UI32 newVal )
{
	autoTimeCount = newVal;
}
void CServerProfile::IncAutoTimeCount( void )
{
	++autoTimeCount;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerProfile::LoopTimeCount()
//|					CServerProfile::IncLoopTimeCount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets/Increments amount of simulation cycles
//o------------------------------------------------------------------------------------------------o
UI32 CServerProfile::LoopTimeCount( void ) const
{
	return loopTimeCount;
}
void CServerProfile::LoopTimeCount( UI32 newVal )
{
	loopTimeCount = newVal;
}
void CServerProfile::IncLoopTimeCount( void )
{
	++loopTimeCount;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerProfile::GlobalReceived()
//|					CServerProfile::GlobalReceived()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets total amount of bytes received by server
//o------------------------------------------------------------------------------------------------o
SI32 CServerProfile::GlobalReceived( void ) const
{
	return globalRecv;
}
void CServerProfile::GlobalReceived( SI32 newVal )
{
	globalRecv = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerProfile::GlobalSent()
//|					CServerProfile::GlobalSent()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets total amount of bytes sent by server
//o------------------------------------------------------------------------------------------------o
SI32 CServerProfile::GlobalSent( void ) const
{
	return globalSent;
}
void CServerProfile::GlobalSent( SI32 newVal )
{
	globalSent = newVal;
}
