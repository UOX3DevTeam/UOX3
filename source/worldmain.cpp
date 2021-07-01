//o-----------------------------------------------------------------------------------------------o
//|	File		-	worldmain.cpp
//o-----------------------------------------------------------------------------------------------o
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
//o-----------------------------------------------------------------------------------------------o
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


CWorldMain						*cwmWorldState = nullptr;

//o-----------------------------------------------------------------------------------------------o
//| CWorldMain Constructor & Destructor
//o-----------------------------------------------------------------------------------------------o
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
keeprun( DEFWORLD_KEEPRUN ), secure( DEFWORLD_SECURE ),  Loaded( DEFWORLD_LOADED ),
uotickcount( DEFWORLD_UOTICKCOUNT ), starttime( DEFWORLD_STARTTIME ), endtime( DEFWORLD_ENDTIME ), lclock( DEFWORLD_LCLOCK ),
overflow( DEFWORLD_OVERFLOW ), uiCurrentTime( DEFWORLD_UICURRENTTIME ), oldtime( DEFWORLD_OLDTIME ), newtime( DEFWORLD_NEWTIME ),
autosaved( DEFWORLD_AUTOSAVED ), worldSaveProgress( DEFWORLD_SAVEPROGRESS ), playersOnline( DEFWORLD_PLAYERSONLINE ),
reloadingScripts( DEFWORLD_RELOADINGSCRIPTS ), classesInitialized( DEFWORLD_CLASSESINITIALIZED )
{
	for( SI32 mTID = (SI32)tWORLD_NEXTFIELDEFFECT; mTID < (SI32)tWORLD_COUNT; ++mTID )
		worldTimers[mTID] = 0;
	creatures.clear();
	prowessTitles.resize( 0 );
	murdererTags.resize( 0 );
	teleLocs.resize( 0 );
	escortRegions.resize( 0 );
	logoutLocs.resize( 0 );
	goPlaces.clear();
	refreshQueue.clear();
	deletionQueue.clear();
	spawnRegions.clear();
	uoxtimeout.tv_sec	= 0;
	uoxtimeout.tv_usec	= 0;
	sData				= new CServerData();
	sProfile			= new CServerProfile();
}

CWorldMain::~CWorldMain()
{
	prowessTitles.clear();
	murdererTags.clear();
	teleLocs.clear();
	logoutLocs.clear();
	escortRegions.clear();
	creatures.clear();
	goPlaces.clear();
	refreshQueue.clear();
	deletionQueue.clear();
	spawnRegions.clear();
	delete sData;
	delete sProfile;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	TIMERVAL GetTimer( CWM_TID timerID ) const
//|					void SetTimer( CWM_TID timerID, TIMERVAL newVal )
//|	Date		-	10/17/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles all the world timers (next time we check NPC AI, etc)
//o-----------------------------------------------------------------------------------------------o
TIMERVAL CWorldMain::GetTimer( CWM_TID timerID ) const
{
	TIMERVAL rvalue = 0;
	if( timerID != tWORLD_COUNT )
		rvalue = worldTimers[timerID];
	return rvalue;
}
void CWorldMain::SetTimer( CWM_TID timerID, TIMERVAL newVal )
{
	if( timerID != tWORLD_COUNT )
		worldTimers[timerID] = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GetError( void ) const
//|					void SetError( bool newVal )
//|	Date		-	3/12/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets if we have generated an error
//o-----------------------------------------------------------------------------------------------o
bool CWorldMain::GetError( void ) const
{
	return error;
}
void CWorldMain::SetError( bool newVal )
{
	error = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GetKeepRun( void ) const
//|					void SetKeepRun( bool newVal )
//|	Date		-	3/12/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets if server should be kept running
//o-----------------------------------------------------------------------------------------------o
bool CWorldMain::GetKeepRun( void ) const
{
	return keeprun;
}
void CWorldMain::SetKeepRun( bool newVal )
{
	keeprun = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GetSecure( void ) const
//|					void SetSecure( bool newVal )
//|	Date		-	3/12/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether console "secure" mode is enabled/disabled
//o-----------------------------------------------------------------------------------------------o
bool CWorldMain::GetSecure( void ) const
{
	return secure;
}
void CWorldMain::SetSecure( bool newVal )
{
	secure = newVal;
}



//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GetLoaded( void ) const
//|					void SetLoaded( bool newVal )
//|	Date		-	3/12/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether UOX has been loaded
//o-----------------------------------------------------------------------------------------------o
bool CWorldMain::GetLoaded( void ) const
{
	return Loaded;
}
void CWorldMain::SetLoaded( bool newVal )
{
	Loaded = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 GetUICurrentTime( void ) const
//|					void SetUICurrentTime( UI32 newVal )
//|	Date		-	3/12/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets current time
//o-----------------------------------------------------------------------------------------------o
UI32 CWorldMain::GetUICurrentTime( void ) const
{
	return uiCurrentTime;
}
void CWorldMain::SetUICurrentTime( UI32 newVal )
{
	uiCurrentTime = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 GetUOTickCount( void ) const
//|					void SetUOTickCount( UI32 newVal )
//|	Date		-	3/12/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets UO Minutes
//o-----------------------------------------------------------------------------------------------o
UI32 CWorldMain::GetUOTickCount( void ) const
{
	return uotickcount;
}
void CWorldMain::SetUOTickCount( UI32 newVal )
{
	uotickcount = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GetOverflow( void ) const
//|					void SetOverflow( bool newVal )
//|	Date		-	3/12/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether we overflowed the time
//o-----------------------------------------------------------------------------------------------o
bool CWorldMain::GetOverflow( void ) const
{
	return overflow;
}
void CWorldMain::SetOverflow( bool newVal )
{
	overflow = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 CWorldMain::GetStartTime( void ) const
//|					void CWorldMain::SetStartTime( UI32 newVal )
//|	Date		-	3/12/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets time when server started up
//o-----------------------------------------------------------------------------------------------o
UI32 CWorldMain::GetStartTime( void ) const
{
	return starttime;
}
void CWorldMain::SetStartTime( UI32 newVal )
{
	starttime = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 GetEndTime( void ) const
//|					void SetEndTime( UI32 newVal )
//|	Date		-	3/12/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets time when server will shutdown
//o-----------------------------------------------------------------------------------------------o
UI32 CWorldMain::GetEndTime( void ) const
{
	return endtime;
}
void CWorldMain::SetEndTime( UI32 newVal )
{
	endtime = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 GetLClock( void ) const
//|					void SetLClock( UI32 newVal )
//|	Date		-	3/12/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets end time
//o-----------------------------------------------------------------------------------------------o
UI32 CWorldMain::GetLClock( void ) const
{
	return lclock;
}
void CWorldMain::SetLClock( UI32 newVal )
{
	lclock = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 GetNewTime( void ) const
//|					void SetNewTime( UI32 newVal )
//|	Date		-	3/12/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets time for next auto worldsave
//o-----------------------------------------------------------------------------------------------o
UI32 CWorldMain::GetNewTime( void ) const
{
	return newtime;
}
void CWorldMain::SetNewTime( UI32 newVal )
{
	newtime = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 GetOldTime( void ) const
//|					void SetOldTime( UI32 newVal )
//|	Date		-	3/12/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets time of last auto worldsave
//o-----------------------------------------------------------------------------------------------o
UI32 CWorldMain::GetOldTime( void ) const
{
	return oldtime;
}
void CWorldMain::SetOldTime( UI32 newVal )
{
	oldtime = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GetAutoSaved( void ) const
//|					void SetAutoSaved( bool newVal )
//|	Date		-	3/12/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether world was autosaved
//o-----------------------------------------------------------------------------------------------o
bool CWorldMain::GetAutoSaved( void ) const
{
	return autosaved;
}
void CWorldMain::SetAutoSaved( bool newVal )
{
	autosaved = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SaveStatus GetWorldSaveProgress( void ) const
//|					void SetWorldSaveProgress( SaveStatus newVal )
//|	Date		-	3/12/2003
//|	Changes		-	10/10/2003 - Now uses enum SaveStatus for ease of use
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets status of World saves (Not Saving, Saving, Just Saved)
//o-----------------------------------------------------------------------------------------------o
SaveStatus CWorldMain::GetWorldSaveProgress( void ) const
{
	return worldSaveProgress;
}
void CWorldMain::SetWorldSaveProgress( SaveStatus newVal )
{
	worldSaveProgress = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	size_t GetPlayersOnline( void ) const
//|					void SetPlayersOnline( size_t newVal )
//|	Date		-	3/12/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets total Players Online
//o-----------------------------------------------------------------------------------------------o
size_t CWorldMain::GetPlayersOnline( void ) const
{
	return playersOnline;
}
void CWorldMain::SetPlayersOnline( size_t newVal )
{
	playersOnline = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CWorldMain::DecPlayersOnline( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Decrements the counter for players online
//o-----------------------------------------------------------------------------------------------o
void CWorldMain::DecPlayersOnline( void )
{
	if( playersOnline == 0 )
		throw std::runtime_error( "Decrementing a 0 count which will rollover" );
	--playersOnline;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GetReloadingScripts( void ) const
//|					void SetReloadingScripts( bool newVal )
//|	Date		-	6/22/2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether server is reloading its scripts
//o-----------------------------------------------------------------------------------------------o
bool CWorldMain::GetReloadingScripts( void ) const
{
	return reloadingScripts;
}
void CWorldMain::SetReloadingScripts( bool newVal )
{
	reloadingScripts = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CheckTimers( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Check shutdown timers
//o-----------------------------------------------------------------------------------------------o
void CWorldMain::CheckTimers( void )
{
	SetOverflow( ( GetLClock() > GetUICurrentTime() ) );
	if( GetEndTime() )
	{
		if( GetEndTime() <= GetUICurrentTime() )
			SetKeepRun( false );
	}
	SetLClock( GetUICurrentTime() );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 GetNewIPTime( void ) const
//|					void SetNewIPTime( UI32 newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets time for next auto IP update
//o-----------------------------------------------------------------------------------------------o
UI32 CWorldMain::GetNewIPTime( void ) const
{
	return newIPtime;
}
void CWorldMain::SetNewIPTime( UI32 newVal )
{
	newIPtime = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 GetOldIPTime( void ) const
//|					void SetOldIPTime( UI32 newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets time of last auto IP update
//o-----------------------------------------------------------------------------------------------o
UI32 CWorldMain::GetOldIPTime( void ) const
{
	return oldIPtime;
}
void CWorldMain::SetOldIPTime( UI32 newVal )
{
	oldIPtime = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GetIPUpdated( void ) const
//|					void SetIPUpdated( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether IPs have been updated
//o-----------------------------------------------------------------------------------------------o
bool CWorldMain::GetIPUpdated( void ) const
{
	return ipupdated;
}
void CWorldMain::SetIPUpdated( bool newVal )
{
	ipupdated = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ClassesInitialized( void ) const
//|					void ClassesInitialized( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether base classes been initialized (in case of early shut down)
//o-----------------------------------------------------------------------------------------------o
bool CWorldMain::ClassesInitialized( void ) const
{
	return classesInitialized;
}
void CWorldMain::ClassesInitialized( bool newVal )
{
	classesInitialized = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void doWorldLight( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Set world light level based on time of day and script settings
//o-----------------------------------------------------------------------------------------------o
void CWorldMain::doWorldLight( void )
{
	UI08 worlddarklevel		= ServerData()->WorldLightDarkLevel();
	UI08 worldbrightlevel	= ServerData()->WorldLightBrightLevel();
	bool ampm				= ServerData()->ServerTimeAMPM();
	UI08 currentHour		= ServerData()->ServerTimeHours();
	UI08 currentMinute		= ServerData()->ServerTimeMinutes();

	R32 currentTime			= R32( currentHour + ( currentMinute / 60.0f) );
	R32 hourIncrement		= R32( fabs( ( worlddarklevel - worldbrightlevel ) / 12.0f ) );	// we want the amount to subtract from LightMax in the morning / add to LightMin in evening

	if( ampm )
		ServerData()->WorldLightCurrentLevel( static_cast<UI08>( roundNumber( worldbrightlevel + ( hourIncrement * currentTime) ) ) );
	else
		ServerData()->WorldLightCurrentLevel( static_cast<UI08>( roundNumber( worlddarklevel - ( hourIncrement * currentTime) ) ) );
}

void fileArchive( void );
void CollectGarbage( void );
void sysBroadcast( const std::string& txt );
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SaveNewWorld( bool x )
//|	Date		-	1997
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Saves the UOX world
//|
//|	Changes		-	10/21/2002	-	fix for archiving. Now it wont always archive :)
//o-----------------------------------------------------------------------------------------------o
void CWorldMain::SaveNewWorld( bool x )
{
	static UI32 save_counter = 0;

	SPAWNMAP_CITERATOR spIter	= cwmWorldState->spawnRegions.begin();
	SPAWNMAP_CITERATOR spEnd	= cwmWorldState->spawnRegions.end();
	while( spIter != spEnd )
	{
		CSpawnRegion *spawnReg = spIter->second;
		if( spawnReg != nullptr )
			spawnReg->checkSpawned();
		++spIter;
	}

	if( GetWorldSaveProgress() != SS_SAVING )
	{
		SetWorldSaveProgress( SS_SAVING );
		Console.PrintSectionBegin();
		if( ServerData()->ServerAnnounceSavesStatus() )
		{
			sysBroadcast( Dictionary->GetEntry( 1615 ) );
			SpeechSys->Poll();
		}
		Network->ClearBuffers();

		if( x )
			Console << "Starting manual world data save...." << myendl;
		else
			Console << "Starting automatic world data save...." << myendl;

		if( ServerData()->ServerBackupStatus() && ServerData()->Directory( CSDDP_BACKUP ).length() > 1 )
		{
			++save_counter;
			if(( save_counter % ServerData()->BackupRatio() ) == 0 )
			{
				Console << "Archiving world files." << myendl;
				fileArchive();
			}
		}
		Console << "Saving Misc. data... ";
		ServerData()->save();
		Console.log( "Server data save", "server.log" );
		RegionSave();
		Console.PrintDone();
		MapRegion->Save();
		GuildSys->Save();
		JailSys->WriteData();
		Effects->SaveEffects();
		ServerData()->SaveTime();
		SaveStatistics();

		if( ServerData()->ServerAnnounceSavesStatus() )
			sysBroadcast( "World Save Complete." );

		//	If accounts are to be loaded then they should be loaded
		//	all the time if using the web interface
		Accounts->Save();
		// Make sure to import the new accounts so they have access too.
		Console << "New accounts processed: " << Accounts->ImportAccounts() << myendl;
		SetWorldSaveProgress( SS_JUSTSAVED );

		char saveTimestamp[100];
		time_t tempTimestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		struct tm *curtime = std::localtime( &tempTimestamp );
		strftime( saveTimestamp, 50, "%F at %T", curtime );

		Console << "World save complete on " << saveTimestamp << myendl;
		Console.PrintSectionBegin();
	}
	CollectGarbage();
	uiCurrentTime = getclock();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void RegionSave( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Loops through all town regions and saves them to disk
//o-----------------------------------------------------------------------------------------------o
void CWorldMain::RegionSave( void )
{
	std::string regionsFile	= cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "regions.wsc";
	std::ofstream regionsDestination( regionsFile.c_str() );
	if( !regionsDestination )
	{
		Console.error( strutil::format("Failed to open %s for writing", regionsFile.c_str()) );
		return;
	}
	TOWNMAP_CITERATOR tIter	= cwmWorldState->townRegions.begin();
	TOWNMAP_CITERATOR tEnd	= cwmWorldState->townRegions.end();
	while( tIter != tEnd )
	{
		CTownRegion *myReg = tIter->second;
		if( myReg != nullptr )
			myReg->Save( regionsDestination );
		++tIter;
	}
	regionsDestination.close();
}

CServerData *CWorldMain::ServerData( void )
{
	return sData;
}

CServerProfile *CWorldMain::ServerProfile( void )
{
	return sProfile;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SaveStatistics( void )
//|	Date		-	February 5th, 2005
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Saves out some useful statistics so that some tools
//|						such as WorldBuilder can do some memory reserve shortcuts
//o-----------------------------------------------------------------------------------------------o
void CWorldMain::SaveStatistics( void )
{
	std::string		statsFile = cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "statistics.wsc";
	std::ofstream	statsDestination( statsFile.c_str() );
	if( !statsDestination )
	{
		Console.error( strutil::format("Failed to open %s for writing", statsFile.c_str()) );
		return;
	}
	statsDestination << "[STATISTICS]" << '\n' << "{" << '\n';
	statsDestination << "PLAYERCOUNT=" << ObjectFactory::getSingleton().CountOfObjects( OT_CHAR ) << '\n';
	statsDestination << "ITEMCOUNT=" << ObjectFactory::getSingleton().CountOfObjects( OT_ITEM ) << '\n';
	statsDestination << "MULTICOUNT=" << ObjectFactory::getSingleton().CountOfObjects( OT_MULTI ) << '\n';
	statsDestination << "}" << '\n' << '\n';

	statsDestination.close();
}

//o-----------------------------------------------------------------------------------------------o
//|	Class		-	CServerProfile
//|	Date		-	2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Version History
//|
//|						1.0		 		2004
//|						Original implementation
//|						Moving all profiling variables into their own class managed
//|						by CWorldMain in order to take them out of global scope
//o-----------------------------------------------------------------------------------------------o
CServerProfile::CServerProfile() : networkTime( 0 ), timerTime( 0 ), autoTime( 0 ), loopTime( 0 ),
networkTimeCount( 1000 ), timerTimeCount( 1000 ), autoTimeCount( 1000 ), loopTimeCount( 1000 ),
globalRecv( 0 ), globalSent( 0 )
{
}
CServerProfile::~CServerProfile()
{
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 NetworkTime( void ) const
//|					void NetworkTime( UI32 newVal )
//|					void IncNetworkTime( UI32 toInc )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets/Increments networkTime
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 TimerTime( void ) const
//|					void TimerTime( UI32 newVal )
//|					void IncTimerTime( UI32 toInc )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets/Increments timerTime
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 AutoTime( void ) const
//|					void AutoTime( UI32 newVal )
//|					void IncAutoTime( UI32 toInc )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets/Increments autoTime
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 LoopTime( void ) const
//|					void LoopTime( UI32 newVal )
//|					void IncLoopTime( UI32 toInc )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets/Increments loopTime
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 NetworkTimeCount( void ) const
//|					void NetworkTimeCount( UI32 newVal )
//|					void IncNetworkTimeCount( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets/Increments networkTimeCount
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 TimerTimeCount( void ) const
//|					void TimerTimeCount( UI32 newVal )
//|					void IncTimerTimeCount( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets/Increments timerTimeCount
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 AutoTimeCount( void ) const
//|					void AutoTimeCount( UI32 newVal )
//|					void IncAutoTimeCount( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets/Increments autoTimeCount
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 LoopTimeCount( void ) const
//|					void LoopTimeCount( UI32 newVal )
//|					void IncLoopTimeCount( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets/Increments amount of simulation cycles
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI32 GlobalReceived( void ) const
//|					void GlobalReceived( SI32 newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets total amount of bytes received by server
//o-----------------------------------------------------------------------------------------------o
SI32 CServerProfile::GlobalReceived( void ) const
{
	return globalRecv;
}
void CServerProfile::GlobalReceived( SI32 newVal )
{
	globalRecv = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI32 GlobalSent( void ) const
//|					void GlobalSent( SI32 newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets total amount of bytes sent by server
//o-----------------------------------------------------------------------------------------------o
SI32 CServerProfile::GlobalSent( void ) const
{
	return globalSent;
}
void CServerProfile::GlobalSent( SI32 newVal )
{
	globalSent = newVal;
}
