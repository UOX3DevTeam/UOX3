//o--------------------------------------------------------------------------o
//|	File			-	worldmain.cpp
//|	Date			-	Unknown
//|	Developers		-	Zane/EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Version History
//|									
//|							1.0		UOX3 DevTeam
//|							Initial implementation.
//|									
//|							1.1		Zane		March 12, 2003
//|							Most global variables moved from uox3.h to the CWorldMain class
//|									
//|							1.2		giwo		October 16, 2003
//|							Added quite a few more vectors to remove them from global scope
//|							Removed many members that were no longer necesarry and moved
//|								others to more appropriate locations.
//|							Added a proper constructor rather than ResetDefaults()
//|							Grouped timers together in an array using an enum.
//o--------------------------------------------------------------------------o
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

namespace UOX
{

CWorldMain						*cwmWorldState = NULL;

//o--------------------------------------------------------------------------o
//| CWorldMain Constructor & Destructor
//o--------------------------------------------------------------------------o
const SERIAL		DEFWORLD_NEXTITEMSERIAL		= BASEITEMSERIAL;
const SERIAL		DEFWORLD_NEXTCHARSERIAL		= 1;
const UI32			DEFWORLD_IMEM				= 0;
const UI32			DEFWORLD_CMEM				= 0;
const bool			DEFWORLD_KEEPRUN			= true;
const bool			DEFWORLD_ERROR				= false;
const bool			DEFWORLD_SECURE				= true;
const UI32			DEFWORLD_ERRORCOUNT			= 0;
const bool			DEFWORLD_LOADED				= false;
const UI16			DEFWORLD_SPAWNREGIONS		= 0;
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
const bool			DEFWORLD_XGMENABLED			= false;
const SaveStatus	DEFWORLD_SAVEPROGRESS		= SS_NOTSAVING;
const bool			DEFWORLD_RELOADINGSCRIPTS	= false;

CWorldMain::CWorldMain() : nextItemSerial( DEFWORLD_NEXTITEMSERIAL ), 
nextCharSerial( DEFWORLD_NEXTCHARSERIAL ), imem( DEFWORLD_IMEM ), cmem( DEFWORLD_CMEM ), error( DEFWORLD_ERROR ), 
keeprun( DEFWORLD_KEEPRUN ), secure( DEFWORLD_SECURE ), ErrorCount( DEFWORLD_ERRORCOUNT ), Loaded( DEFWORLD_LOADED ), 
uotickcount( DEFWORLD_UOTICKCOUNT ), starttime( DEFWORLD_STARTTIME ), endtime( DEFWORLD_ENDTIME ), lclock( DEFWORLD_LCLOCK ), 
overflow( DEFWORLD_OVERFLOW ), uiCurrentTime( DEFWORLD_UICURRENTTIME ), oldtime( DEFWORLD_OLDTIME ), newtime( DEFWORLD_NEWTIME ), 
autosaved( DEFWORLD_AUTOSAVED ), worldSaveProgress( DEFWORLD_SAVEPROGRESS ), playersOnline( DEFWORLD_PLAYERSONLINE ), 
xgm( DEFWORLD_XGMENABLED ), reloadingScripts( DEFWORLD_RELOADINGSCRIPTS )
{
	for( int mTID = (int)tWORLD_NEXTFIELDEFFECT; mTID < (int)tWORLD_COUNT; ++mTID )
		worldTimers[mTID] = 0;
	creatures.clear();
	prowessTitles.resize( 0 );
	murdererTags.resize( 0 );
	teleLocs.resize( 0 );
	escortRegions.resize( 0 );
	logoutLocs.resize( 0 );
	goPlaces.clear();
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
	delete sData;
	delete sProfile;
}

//o--------------------------------------------------------------------------o
//|	Function		-	TIMERVAL Timer()
//|	Date			-	10/17/2003
//|	Programmer		-	giwo
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Handles all the world timers (next time we check NPC AI, etc)
//o--------------------------------------------------------------------------o
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

//o--------------------------------------------------------------------------o
//|	Function		-	SERIAL NextItemSerial()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-	Changed to NextItemSerial() from ItemCount2() (giwo 10/16/03)
//o--------------------------------------------------------------------------o
//|	Purpose			-	Item Serials
//o--------------------------------------------------------------------------o
SERIAL CWorldMain::GetNextItemSerial( void ) const
{
	return nextItemSerial;
}
void CWorldMain::SetNextItemSerial( SERIAL newVal )
{
	nextItemSerial = newVal;
}
void CWorldMain::IncNextItemSerial( void )
{
	++nextItemSerial;
}

//o--------------------------------------------------------------------------o
//|	Function		-	SERIAL NextCharSerial()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-	Changed to NextCharSerial() from CharCount2() (giwo 10/16/03)
//o--------------------------------------------------------------------------o
//|	Purpose			-	Character Serials
//o--------------------------------------------------------------------------o
SERIAL CWorldMain::GetNextCharSerial( void ) const
{
	return nextCharSerial;
}
void CWorldMain::SetNextCharSerial( SERIAL newVal )
{
	nextCharSerial = newVal;
}
void CWorldMain::IncNextCharSerial( void )
{
	++nextCharSerial;
}

//o--------------------------------------------------------------------------o
//|	Function		-	UI32 IMem()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Item Memory
//o--------------------------------------------------------------------------o
UI32 CWorldMain::GetIMem( void ) const
{
	return imem;
}
void CWorldMain::SetIMem( UI32 newVal )
{
	imem = newVal;
}

//o--------------------------------------------------------------------------o
//|	Function		-	UI32 CMem()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Character Memory
//o--------------------------------------------------------------------------o
UI32 CWorldMain::GetCMem( void ) const
{
	return cmem;
}
void CWorldMain::SetCMem( UI32 newVal )
{
	cmem = newVal;
}
void CWorldMain::IncCMem( void )
{
	++cmem;
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool Error()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Check if we have generated an error
//o--------------------------------------------------------------------------o
bool CWorldMain::GetError( void ) const
{
	return error;
}
void CWorldMain::SetError( bool newVal )
{
	error = newVal;
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool KeepRun()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Check if server should be kept running
//o--------------------------------------------------------------------------o
bool CWorldMain::GetKeepRun( void ) const
{
	return keeprun;
}
void CWorldMain::SetKeepRun( bool newVal )
{
	keeprun = newVal;
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool Secure()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Console "secure" mode
//o--------------------------------------------------------------------------o
bool CWorldMain::GetSecure( void ) const
{
	return secure;
}
void CWorldMain::SetSecure( bool newVal )
{
	secure = newVal;
}

//o--------------------------------------------------------------------------o
//|	Function		-	UI32 ErrorCount()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Total number of errors on a server (for crash protection)
//o--------------------------------------------------------------------------o
UI32 CWorldMain::GetErrorCount( void ) const
{
	return ErrorCount;
}
void CWorldMain::SetErrorCount( UI32 newVal )
{
	ErrorCount = newVal;
}
void CWorldMain::IncErrorCount( void )
{
	++ErrorCount;
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool Loaded()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	UOX has been loaded
//o--------------------------------------------------------------------------o
bool CWorldMain::GetLoaded( void ) const
{
	return Loaded;
}
void CWorldMain::SetLoaded( bool newVal )
{
	Loaded = newVal;
}

//o--------------------------------------------------------------------------o
//|	Function		-	UI32 UICurrentTime()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Current time
//o--------------------------------------------------------------------------o
UI32 CWorldMain::GetUICurrentTime( void ) const
{
	return uiCurrentTime;
}
void CWorldMain::SetUICurrentTime( UI32 newVal )
{
	uiCurrentTime = newVal;
}

//o--------------------------------------------------------------------------o
//|	Function		-	UI32 UOTickCount()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	UO Minutes
//o--------------------------------------------------------------------------o
UI32 CWorldMain::GetUOTickCount( void ) const
{
	return uotickcount;
}
void CWorldMain::SetUOTickCount( UI32 newVal )
{
	uotickcount = newVal;
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool Overflow()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Overflowed the time
//o--------------------------------------------------------------------------o
bool CWorldMain::GetOverflow( void ) const
{
	return overflow;
}
void CWorldMain::SetOverflow( bool newVal )
{
	overflow = newVal;
}

//o--------------------------------------------------------------------------o
//|	Function		-	UI32 StartTime()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Time server started up
//o--------------------------------------------------------------------------o
UI32 CWorldMain::GetStartTime( void ) const
{
	return starttime;
}
void CWorldMain::SetStartTime( UI32 newVal )
{
	starttime = newVal;
}

//o--------------------------------------------------------------------------o
//|	Function		-	UI32 EndTime()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Time When Server Will Shutdown
//o--------------------------------------------------------------------------o
UI32 CWorldMain::GetEndTime( void ) const
{
	return endtime;
}
void CWorldMain::SetEndTime( UI32 newVal )
{
	endtime = newVal;
}

//o--------------------------------------------------------------------------o
//|	Function		-	UI32 LClock()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	End Time
//o--------------------------------------------------------------------------o
UI32 CWorldMain::GetLClock( void ) const
{
	return lclock;
}
void CWorldMain::SetLClock( UI32 newVal )
{
	lclock = newVal;
}

//o--------------------------------------------------------------------------o
//|	Function		-	UI32 NewTime()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Time for next auto worldsave
//o--------------------------------------------------------------------------o
UI32 CWorldMain::GetNewTime( void ) const
{
	return newtime;
}
void CWorldMain::SetNewTime( UI32 newVal )
{
	newtime = newVal;
}

//o--------------------------------------------------------------------------o
//|	Function		-	UI32 OldTime()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Time of last auto worldsave
//o--------------------------------------------------------------------------o
UI32 CWorldMain::GetOldTime( void ) const
{
	return oldtime;
}
void CWorldMain::SetOldTime( UI32 newVal )
{
	oldtime = newVal;
}

//o--------------------------------------------------------------------------o
//|	Function		-	UI32 AutoSaved()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	World autosaved
//o--------------------------------------------------------------------------o
bool CWorldMain::GetAutoSaved( void ) const
{
	return autosaved;
}
void CWorldMain::SetAutoSaved( bool newVal )
{
	autosaved = newVal;
}

//o--------------------------------------------------------------------------o
//|	Function		-	SaveStatus WorldSaveProgress()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-	giwo - 10/10/2003 - Now uses enum SaveStatus for ease of use
//o--------------------------------------------------------------------------o
//|	Purpose			-	Status of World saves (Not Saving, Saving, Just Saved)
//o--------------------------------------------------------------------------o
SaveStatus CWorldMain::GetWorldSaveProgress( void ) const
{
	return worldSaveProgress;
}
void CWorldMain::SetWorldSaveProgress( SaveStatus newVal )
{
	worldSaveProgress = newVal;
}

//o--------------------------------------------------------------------------o
//|	Function		-	SI32 PlayersOnline()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Total Players Online
//o--------------------------------------------------------------------------o
size_t CWorldMain::GetPlayersOnline( void ) const
{
	return playersOnline;
}
void CWorldMain::SetPlayersOnline( size_t newVal )
{
	playersOnline = newVal;
}
void CWorldMain::DecPlayersOnline( void )
{
	if( playersOnline == 0 )
		throw std::runtime_error( "Decrementing a 0 count which will rollover" );
	--playersOnline;
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool XGMEnabled()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	XGM Enabled or Disabled
//o--------------------------------------------------------------------------o
bool CWorldMain::GetXGMEnabled( void ) const
{
	return xgm;
}
void CWorldMain::SetXGMEnabled( bool newVal )
{
	xgm = newVal;
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool ReloadingScripts( void )
//|	Date			-	6/22/2004
//|	Programmer	-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose		-	Server is reloading its scripts
//o--------------------------------------------------------------------------o
bool CWorldMain::GetReloadingScripts( void ) const
{
	return reloadingScripts;
}
void CWorldMain::SetReloadingScripts( bool newVal )
{
	reloadingScripts = newVal;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void CWorldMain::CheckTimers( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check shutdown timers
//o---------------------------------------------------------------------------o
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

//o---------------------------------------------------------------------------o
//|	Function	-	void CWorldMain::doWorldLight( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Set world light level based on time of day and script settings
//o---------------------------------------------------------------------------o
void CWorldMain::doWorldLight( void )
{
	UI08 worlddarklevel		= ServerData()->WorldLightDarkLevel();
	UI08 worldbrightlevel	= ServerData()->WorldLightBrightLevel();
	bool ampm				= ServerData()->ServerTimeAMPM();
	R32 hourIncrement		= R32( fabs( ( worlddarklevel - worldbrightlevel ) / 12.0f ) );	// we want the amount to subtract from LightMax in the morning / add to LightMin in evening
	if( ampm )
		ServerData()->WorldLightCurrentLevel( static_cast<UI08>( worldbrightlevel + hourIncrement ) );
	else
		ServerData()->WorldLightCurrentLevel( static_cast<UI08>( worlddarklevel - hourIncrement ) );
}

void fileArchive( void );
void CollectGarbage( void );
void sysBroadcast( std::string txt );
//o--------------------------------------------------------------------------o
//|	Function/Class	-	SaveNewWorld( bool x )
//|	Date			-	1997
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	Saves the UOX world
//|	
//|	Modification	-	10/21/2002	-	Xuri fix for archiving. Now it wont always
//|									archive :)
//o--------------------------------------------------------------------------o
void CWorldMain::SaveNewWorld( bool x )
{
	static unsigned int save_counter = 0;

	std::vector< cSpawnRegion * >::iterator spawnCounter;
	for( spawnCounter = spawnregions.begin(); spawnCounter != spawnregions.end(); ++spawnCounter )
	{
		cSpawnRegion *spawnReg = (*spawnCounter);
		if( spawnReg != NULL )
			spawnReg->checkSpawned();
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
		//Accounts->SaveAccounts();
		ServerData()->save();
		Console.Log( "Server data save", "server.log" );
		RegionSave();
		Console.PrintDone();
		MapRegion->Save(); 
		GuildSys->Save();
		JailSys->WriteData();
		Skills->SaveResources();
		Effects->SaveEffects();

		if( ServerData()->ServerAnnounceSavesStatus() )
			sysBroadcast( "World Save Complete." );

		//	If accounts are to be loaded then they should be loaded
		//	all the time if using the web interface
		Accounts->Save();
		// Make sure to import the new accounts so they have access too.
		Console << "New accounts processed: " << Accounts->ImportAccounts() << myendl;
		SetWorldSaveProgress( SS_JUSTSAVED );
		Console << "World save complete." << myendl;
		Console.PrintSectionBegin();
	}
	CollectGarbage();
	uiCurrentTime = getclock();
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	void CWorldMain::RegionSave( void )
//|	Date			-	Unknown
//|	Developer(s)	-	Abaddon
//|	Company/Team	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Loops through all town regions and saves them to disk
//o--------------------------------------------------------------------------o	
void CWorldMain::RegionSave( void )
{
	std::string regionsFile	= cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "regions.wsc";
	std::ofstream regionsDestination( regionsFile.c_str() );
	if( !regionsDestination ) 
	{
		Console.Error( 1, "Failed to open %s for writing", regionsFile.c_str() );
		return;
	}
	std::vector< cTownRegion * >::iterator regionCounter;
	for( regionCounter = regions.begin(); regionCounter != regions.end(); ++regionCounter )
	{
		cTownRegion *myReg = (*regionCounter);
		if( myReg != NULL )
			myReg->Save( regionsDestination );
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

//o--------------------------------------------------------------------------o
//|	Class				-	CServerProfile
//|	Date				-	2004
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Version History
//|									
//|						1.0		giwo		2004
//|						Original implementation
//|						Moving all profiling variables into their own class managed
//|						by CWorldMain in order to take them out of global scope
//o--------------------------------------------------------------------------o
CServerProfile::CServerProfile() : networkTime( 0 ), timerTime( 0 ), autoTime( 0 ), loopTime( 0 ),
networkTimeCount( 1000 ), timerTimeCount( 1000 ), autoTimeCount( 1000 ), loopTimeCount( 1000 ),
globalRecv( 0 ), globalSent( 0 )
{
}
CServerProfile::~CServerProfile()
{
}

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

SI32 CServerProfile::GlobalReceived( void ) const
{
	return globalRecv;
}
void CServerProfile::GlobalReceived( SI32 newVal )
{
	globalRecv = newVal;
}

SI32 CServerProfile::GlobalSent( void ) const
{
	return globalSent;
}
void CServerProfile::GlobalSent( SI32 newVal )
{
	globalSent = newVal;
}

}
