// WorldMain.cpp: implementation of the CWorldMain class.
//
//////////////////////////////////////////////////////////////////////

#include "uox3.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWorldMain::CWorldMain()
{
	//	December 24, 2000 - EviLDeD
	//	Modify the constructor to load in the uox3.ini file automatically
	ResetDefaults();
	announce( false );

	isSaving = false;
	sData = new cServerData();
}

CWorldMain::~CWorldMain()
{
	delete sData;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI32 NextFieldEffectTime()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Global field effect timer
//o--------------------------------------------------------------------------
UI32 CWorldMain::GetNextFieldEffectTime( void )
{
	return nextfieldeffecttime;
}
void CWorldMain::SetNextFieldEffectTime( UI32 newVal )
{
	nextfieldeffecttime = newVal;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI32 NextNPCAITime()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Global NPC AI timer
//o--------------------------------------------------------------------------
UI32 CWorldMain::GetNextNPCAITime( void )
{
	return nextnpcaitime;
}
void CWorldMain::SetNextNPCAITime( UI32 newVal )
{
	nextnpcaitime = newVal;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI32 ShopRestockTime()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Global shop restock timer
//o--------------------------------------------------------------------------
UI32 CWorldMain::GetShopRestockTime( void )
{
	return shoprestocktime;
}
void CWorldMain::SetShopRestockTime( UI32 newVal )
{
	shoprestocktime = newVal;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI32 HungerDamageTimer()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Global hunger damage timer
//o--------------------------------------------------------------------------
UI32 CWorldMain::GetHungerDamageTimer( void )
{
	return hungerdamagetimer;
}
void CWorldMain::SetHungerDamageTimer( UI32 newVal )
{
	hungerdamagetimer = newVal;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI32 PolyDuration()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Global polymorph timer
//o--------------------------------------------------------------------------
UI32 CWorldMain::GetPolyDuration( void )
{
	return polyduration;
}
void CWorldMain::SetPolyDuration( UI32 newVal )
{
	polyduration = newVal;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI32 ItemCount()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Total itemcount
//o--------------------------------------------------------------------------
UI32 CWorldMain::GetItemCount( void )
{
	return itemcount;
}
void CWorldMain::SetItemCount( UI32 newVal )
{
	itemcount = newVal;
}
void CWorldMain::IncItemCount( void )
{
	itemcount++;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI32 CharCount()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Total charcount
//o--------------------------------------------------------------------------
UI32 CWorldMain::GetCharCount( void )
{
	return charcount;
}
void CWorldMain::SetCharCount( UI32 newVal )
{
	charcount = newVal;
}
void CWorldMain::IncCharCount( void )
{
	charcount++;
}

//o--------------------------------------------------------------------------
//|	Function		-	SERIAL ItemCount2()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Item Serials
//o--------------------------------------------------------------------------
SERIAL CWorldMain::GetItemCount2( void )
{
	return itemcount2;
}
void CWorldMain::SetItemCount2( SERIAL newVal )
{
	itemcount2 = newVal;
}
void CWorldMain::IncItemCount2( void )
{
	itemcount2++;
}

//o--------------------------------------------------------------------------
//|	Function		-	SERIAL CharCount2()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Character Serials
//o--------------------------------------------------------------------------
SERIAL CWorldMain::GetCharCount2( void )
{
	return charcount2;
}
void CWorldMain::SetCharCount2( SERIAL newVal )
{
	charcount2 = newVal;
}
void CWorldMain::IncCharCount2( void )
{
	charcount2++;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI32 IMem()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Item Memory
//o--------------------------------------------------------------------------
UI32 CWorldMain::GetIMem( void )
{
	return imem;
}
void CWorldMain::SetIMem( UI32 newVal )
{
	imem = newVal;
}
void CWorldMain::IncIMem( void )
{
	imem++;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI32 CMem()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Character Memory
//o--------------------------------------------------------------------------
UI32 CWorldMain::GetCMem( void )
{
	return cmem;
}
void CWorldMain::SetCMem( UI32 newVal )
{
	cmem = newVal;
}
void CWorldMain::IncCMem( void )
{
	cmem++;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI08 WeightPerStr()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Amount of Weight one can hold per point of STR
//o--------------------------------------------------------------------------
UI08 CWorldMain::GetWeightPerStr( void )
{
	return weight_per_str;
}
void CWorldMain::SetWeightPerStr( UI08 newVal )
{
	weight_per_str = newVal;
}

//o--------------------------------------------------------------------------
//|	Function		-	bool Error()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Check if we have generated an error
//o--------------------------------------------------------------------------
bool CWorldMain::GetError( void )
{
	return error;
}
void CWorldMain::SetError( bool newVal )
{
	error = newVal;
}

//o--------------------------------------------------------------------------
//|	Function		-	bool KeepRun()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Check if server should be kept running
//o--------------------------------------------------------------------------
bool CWorldMain::GetKeepRun( void )
{
	return keeprun;
}
void CWorldMain::SetKeepRun( bool newVal )
{
	keeprun = newVal;
}

//o--------------------------------------------------------------------------
//|	Function		-	bool Secure()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Console "secure" mode
//o--------------------------------------------------------------------------
bool CWorldMain::GetSecure( void )
{
	return secure;
}
void CWorldMain::SetSecure( bool newVal )
{
	secure = newVal;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI32 ErrorCount()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Total spawnregions on a server
//o--------------------------------------------------------------------------
UI32 CWorldMain::GetErrorCount( void )
{
	return ErrorCount;
}
void CWorldMain::SetErrorCount( UI32 newVal )
{
	ErrorCount = newVal;
}
void CWorldMain::IncErrorCount( void )
{
	ErrorCount++;
}

//o--------------------------------------------------------------------------
//|	Function		-	bool Loaded()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	UOX has been loaded
//o--------------------------------------------------------------------------
bool CWorldMain::GetLoaded( void )
{
	return Loaded;
}
void CWorldMain::SetLoaded( bool newVal )
{
	Loaded = newVal;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI32 UICurrentTime()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Current time
//o--------------------------------------------------------------------------
UI32 CWorldMain::GetUICurrentTime( void )
{
	return uiCurrentTime;
}
void CWorldMain::SetUICurrentTime( UI32 newVal )
{
	uiCurrentTime = newVal;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI16 SecondsPerUOMinute()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Number of seconds per one minute ingame
//o--------------------------------------------------------------------------
UI16 CWorldMain::GetSecondsPerUOMinute( void )
{
	return secondsperuominute;
}
void CWorldMain::SetSecondsPerUOMinute( UI16 newVal )
{
	secondsperuominute = newVal;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI32 UOTickCount()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	UO Minutes
//o--------------------------------------------------------------------------
UI32 CWorldMain::GetUOTickCount( void )
{
	return uotickcount;
}
void CWorldMain::SetUOTickCount( UI32 newVal )
{
	uotickcount = newVal;
}

//o--------------------------------------------------------------------------
//|	Function		-	bool Overflow()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Overflowed the time
//o--------------------------------------------------------------------------
bool CWorldMain::GetOverflow( void )
{
	return overflow;
}
void CWorldMain::SetOverflow( bool newVal )
{
	overflow = newVal;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI32 StartTime()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Time server started up
//o--------------------------------------------------------------------------
UI32 CWorldMain::GetStartTime( void )
{
	return starttime;
}
void CWorldMain::SetStartTime( UI32 newVal )
{
	starttime = newVal;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI32 EndTime()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Time When Server Will Shutdown
//o--------------------------------------------------------------------------
UI32 CWorldMain::GetEndTime( void )
{
	return endtime;
}
void CWorldMain::SetEndTime( UI32 newVal )
{
	endtime = newVal;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI32 LClock()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	End Time
//o--------------------------------------------------------------------------
UI32 CWorldMain::GetLClock( void )
{
	return lclock;
}
void CWorldMain::SetLClock( UI32 newVal )
{
	lclock = newVal;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI32 LightTime()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Time to change lighting
//o--------------------------------------------------------------------------
UI32 CWorldMain::GetLightTime( void )
{
	return lighttime;
}
void CWorldMain::SetLightTime( UI32 newVal )
{
	lighttime = newVal;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI32 NewTime()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Time for next auto worldsave
//o--------------------------------------------------------------------------
UI32 CWorldMain::GetNewTime( void )
{
	return newtime;
}
void CWorldMain::SetNewTime( UI32 newVal )
{
	newtime = newVal;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI32 OldTime()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Time of last auto worldsave
//o--------------------------------------------------------------------------
UI32 CWorldMain::GetOldTime( void )
{
	return oldtime;
}
void CWorldMain::SetOldTime( UI32 newVal )
{
	oldtime = newVal;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI32 AutoSaved()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	World autosaved
//o--------------------------------------------------------------------------
bool CWorldMain::GetAutoSaved( void )
{
	return autosaved;
}
void CWorldMain::SetAutoSaved( bool newVal )
{
	autosaved = newVal;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI08 WorldSaveProgress()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Status of World saves (0=not saving, 1=saving, 2=just saved)
//o--------------------------------------------------------------------------
UI08 CWorldMain::GetWorldSaveProgress( void )
{
	return worldSaveProgress;
}
void CWorldMain::SetWorldSaveProgress( UI08 newVal )
{
	worldSaveProgress = newVal;
}

//o--------------------------------------------------------------------------
//|	Function		-	SI32 PlayersOnline()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Total Players Online
//o--------------------------------------------------------------------------
SI32 CWorldMain::GetPlayersOnline( void )
{
	return now;
}
void CWorldMain::SetPlayersOnline( SI32 newVal )
{
	now = newVal;
}
void CWorldMain::DecPlayersOnline( void )
{
	now--;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI08 DisplayLayers()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Toggle displaying an items layer
//o--------------------------------------------------------------------------
bool CWorldMain::GetDisplayLayers( void )
{
	return showlayer;
}
void CWorldMain::SetDisplayLayers( bool newVal )
{
	showlayer = newVal;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI16 TotalSpawnRegions()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Total spawnregions on a server
//o--------------------------------------------------------------------------
UI16 CWorldMain::GetTotalSpawnRegions( void )
{
	return totalspawnregions;
}
void CWorldMain::SetTotalSpawnRegions( UI16 newVal )
{
	totalspawnregions = newVal;
}
void CWorldMain::IncTotalSpawnRegions( void )
{
	totalspawnregions++;
}

//o--------------------------------------------------------------------------
//|	Function		-	bool XGMEnabled()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	XGM Enabled or Disabled
//o--------------------------------------------------------------------------
bool CWorldMain::GetXGMEnabled( void )
{
	return xgm;
}
void CWorldMain::SetXGMEnabled( bool newVal )
{
	xgm = newVal;
}

//o--------------------------------------------------------------------------
//|	Function		-	SI32 ExecuteBatch()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Executes a batch file
//o--------------------------------------------------------------------------
SI32 CWorldMain::GetExecuteBatch( void )
{
	return executebatch;
}
void CWorldMain::SetExecuteBatch( SI32 newVal )
{
	executebatch = newVal;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI16 LocationCount()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Total locations
//o--------------------------------------------------------------------------
UI16 CWorldMain::GetLocationCount( void )
{
	return locationcount;
}
void CWorldMain::SetLocationCount( UI16 newVal )
{
	executebatch = newVal;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI32 LogoutCount()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Used for instalog
//o--------------------------------------------------------------------------
UI32 CWorldMain::GetLogoutCount( void )
{
	return logoutcount;
}
void CWorldMain::SetLogoutCount( UI32 newVal )
{
	logoutcount = newVal;
}
void CWorldMain::IncLogoutCount( void )
{
	logoutcount++;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI08 EscortRegions()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Total escort regions
//o--------------------------------------------------------------------------
UI08 CWorldMain::GetEscortRegions( void )
{
	return escortRegions;
}
void CWorldMain::SetEscortRegions( UI08 newVal )
{
	escortRegions = newVal;
}
void CWorldMain::IncEscortRegions( void )
{
	escortRegions++;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI08 ValidEscortRegion()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Valid escort region
//o--------------------------------------------------------------------------
UI08 CWorldMain::GetValidEscortRegion( UI08 part )
{
	return validEscortRegion[part];
}
void CWorldMain::SetValidEscortRegion( UI08 part, UI08 newVal )
{
	validEscortRegion[part] = newVal;
}

//o--------------------------------------------------------------------------
//|	Function		-	SI32 ErroredLayers()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Errored Layers
//o--------------------------------------------------------------------------
SI32 CWorldMain::GetErroredLayer( UI08 part )
{
	return erroredLayers[part];
}
void CWorldMain::SetErroredLayer( UI08 part, SI32 newVal )
{
	erroredLayers[part] = newVal;
}
void CWorldMain::IncErroredLayer( UI08 part )
{
	erroredLayers[part]++;
}

//o--------------------------------------------------------------------------
//|	Function		-	CWorldMain::ResetDefaults( void )
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Reset all global vars to default
//o--------------------------------------------------------------------------
void CWorldMain::ResetDefaults( void )
{
	SetNextFieldEffectTime( 0 );
	SetNextNPCAITime( 0 );
	SetShopRestockTime( 0 );
	SetHungerDamageTimer( 0 );
	SetPolyDuration( 90 );
	SetItemCount( 0 );
	SetCharCount( 0 );
	SetItemCount2( BASEITEMSERIAL );
	SetCharCount2( 1 );
	SetIMem( 0 );
	SetCMem( 0 );
	SetWeightPerStr( 4 );
	SetKeepRun( true );
	SetError( false );
	SetSecure( true );
	SetErrorCount( 0 );
	SetLoaded( false );
	SetDisplayLayers( false );
	SetTotalSpawnRegions( 0 );
	SetUICurrentTime( 0 );
	SetSecondsPerUOMinute( 5 );
	SetUOTickCount( 1 );
	SetOverflow( false );
	SetStartTime( 0 );
	SetEndTime( 0 );
	SetLClock( 0 );
	SetLightTime( 0 );
	SetPlayersOnline( 0 );
	SetNewTime( 0 );
	SetOldTime( 0 );
	SetAutoSaved( false );
	SetXGMEnabled( false );
	SetExecuteBatch( 0 );
	SetLocationCount( 0 );
	SetLogoutCount( 0 );
	SetWorldSaveProgress( 0 );
	SetEscortRegions( 0 );
	memset( validEscortRegion, 0, sizeof( validEscortRegion[0] ) * 256 );
	memset( erroredLayers, 0, sizeof( erroredLayers[0] ) * MAXLAYERS );
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
	SI16 worlddarklevel = ServerData()->GetWorldLightDarkLevel();
	SI16 worldbrightlevel = ServerData()->GetWorldLightBrightLevel();

	bool ampm = ServerData()->GetServerTimeAMPM();

	R32 hourIncrement = R32( fabs( ( worlddarklevel - worldbrightlevel ) / 12.0f ) );	// we want the amount to subtract from LightMax in the morning / add to LightMin in evening
	if( ampm )
		ServerData()->SetWorldLightCurrentLevel( (UI16)( worldbrightlevel + hourIncrement ) );
	else
		ServerData()->SetWorldLightCurrentLevel( (UI16)( worlddarklevel - hourIncrement ) );
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	
//|	Date			-	?/?/1997
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	
//|									
//|	Modification	-	10/21/2002	-	Xuri fix for archiving. Now it wont always
//|									archive :)
//o--------------------------------------------------------------------------o
//|	Returns				-	N/A
//o--------------------------------------------------------------------------o	
void CWorldMain::savenewworld( bool x )
{
	static unsigned int save_counter = 0;
//	tempeffectsoff();

	cSpawnRegion *spawnReg = NULL;

	for( UI16 i = 1; i < totalspawnregions; i++ )
	{
		spawnReg = spawnregion[i];

		if( spawnReg != NULL )
			spawnReg->checkSpawned();
	}
	
	ncspawnsp.GarbageCollect();
	nspawnsp.GarbageCollect();
	ncharsp.GarbageCollect();
	nitemsp.GarbageCollect();
	SetWorldSaveProgress( 1 );
	if( !Saving() )
	{
		Console.PrintSectionBegin();
		if( announce() )
		{
			sysbroadcast( Dictionary->GetEntry( 1615 ) );
			SpeechSys->Poll();
		}
		Network->ClearBuffers();

		if( x )
			Console << "Starting manual world data save...." << myendl;
		else 
			Console << "Starting automatic world data save...." << myendl;
		
		if( ServerData()->GetServerBackupStatus() && strlen( ServerData()->GetBackupDirectory() ) > 1 )
		{
			save_counter++;
			if(( save_counter % ServerData()->GetBackupRatio() ) == 0 )
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
		SaveEffects();

		if( announce() )
			sysbroadcast("World Save Complete.");

		//	If accounts are to be loaded then they should be loaded
		//	all the time if using the web interface
		if( ServerData()->GetExternalAccountStatus()  )
		{
			Accounts->Load();
		}
		Accounts->Save();
		SetWorldSaveProgress( 2 );
		Console << "World save complete." << myendl;
		Console.PrintSectionBegin();
	}

//	tempeffectson();
	uiCurrentTime = getclock();
}

bool CWorldMain::announce( void )
{
	return DisplayWorldSaves;
}

void CWorldMain::announce( bool choice )
{
	DisplayWorldSaves = choice;
}

bool CWorldMain::Saving( void )
{
	return isSaving;
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	void CWorldMain::RegionSave( void )
//|	Date					-	Unknown
//|	Developer(s)	-	Abaddon
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//|	Returns				-
//o--------------------------------------------------------------------------o
//|	Notes					-	
//o--------------------------------------------------------------------------o	
void CWorldMain::RegionSave( void )
{
	char regionsFile[MAX_PATH];
	sprintf(regionsFile, "%s%s", cwmWorldState->ServerData()->GetSharedDirectory(), "regions.wsc");
	FILE *regions = fopen( regionsFile, "w" );
	if( regions != NULL )
	{
		for( UI16 regionCounter = 0; regionCounter < 256; regionCounter++ )
		{
			if( region[regionCounter] != NULL )
				region[regionCounter]->Save( regions );
		}
		fclose( regions );
	}
}

cServerData *CWorldMain::ServerData( void )
{
	return sData;
}

/*
Below are the tables to be writen to saveidx.wsc
these are lengths of all the data fields that are used in binary 
saves.  These tables should be updated when any tag is changed.
These tables provide useful info for 3rd party tools... so they can
easily skip unknown data in world save files.

IMPORTANT:
The first 64 tags for BOTH items and characters are RESERVED for 
cBaseObject values and should not be used for item/char specific stuff.
The first 64 values in char and item tables should always match!

Length of 0xFF(255) implies NULL terminated string.
-The exception is 21 and 22, which are used for tags, 21 is a null 
 string for the tag name, followed by a null string, and 22 is a null
 string for tag name, followed by a 4 byte (long) JSVal

~Bryan "Zippy" Pass (1/9/02)
*/

const UI08 CharSaveTable[256] = 
{// 0	1	2	3	4	5	6	7  
	255,255,4,	7,	4,	2,	2,	2,	//0 (0)
	4,	12,	6,	4,	4,	4,	2,	1,	//8 (8)
	1,	4,	2,	2,	16,	255,255,0,	//16 (10)
	0,	0,	0,	0,	0,	0,	0,	0,	//24 (18)
	0,	0,	0,	0,	0,	0,	0,	0,	//32 (20)
	0,	0,	0,	0,	0,	0,	0,	0,	//40 (28)
	0,	0,	0,	0,	0,	0,	0,	0,	//48 (30)
	0,	0,	0,	0,	0,	0,	0,	0,	//56 (38)
//indexes above this line are reserved for cBaseObject
	4,	4,255,255,255,	2,	4,	2,	//64 (40)
	2,	1,	1,	8,	4,	4,	2,	4,	//72 (48)
	8,	4,	4,	4,	8,	4,	1,	9,	//80 (50)
	1,	1,	1,	1,	5,	1,	4,	6,	//88 (58)
	3,	3,	4,	2,	4,	4,	1+2*TRUESKILLS,1+2*ALLSKILLS,//96 (60)
	1+TRUESKILLS,	4,	2,	2,	2,	0,	0,	0,	//104 (68)
	0,	0,	0,	0,	0,	0,	0,	0,	//112 (70)
	0,	0,	0,	0,	0,	0,	0,	0,	//120 (78)
	0,	0,	0,	0,	0,	0,	0,	0,	//128 (80)
	0,	0,	0,	0,	0,	0,	0,	0,	//136 (88)
	0,	0,	0,	0,	0,	0,	0,	0,	//144 (90)
	0,	0,	0,	0,	0,	0,	0,	0,	//152 (98)
	0,	0,	0,	0,	0,	0,	0,	0,	//160 (a0)
	0,	0,	0,	0,	0,	0,	0,	0,	//168 (a8)
	0,	0,	0,	0,	0,	0,	0,	0,	//176 (b0)
	0,	0,	0,	0,	0,	0,	0,	0,	//184 (b8)
	0,	0,	0,	0,	0,	0,	0,	0,	//192 (c0)
	0,	0,	0,	0,	0,	0,	0,	0,	//200 (c8)
	0,	0,	0,	0,	0,	0,	0,	0,	//208 (d0)
	0,	0,	0,	0,	0,	0,	0,	0,	//216 (d8)
	0,	0,	0,	0,	0,	0,	0,	0,	//224 (e0)
	0,	0,	0,	0,	0,	0,	0,	0,	//232 (e8)
	0,	0,	0,	0,	0,	0,	0,	0,	//240 (f0)
	0,	0,	0,	0,	0,	0,	0,	0,	//248 (f8)
	//ff
};

const UI08 ItemSaveTable[256] = 
{// 0	1	2	3	4	5	6	7  
	255,255,4,	7,	4,	2,	2,	2,	//0 (0)
	4,	12,	6,	4,	4,	4,	2,	1,	//8 (8)
	1,	4,	2,	2,	16,	255,255,0,	//16 (10)
	0,	0,	0,	0,	0,	0,	0,	0,	//24 (18)
	0,	0,	0,	0,	0,	0,	0,	0,	//32 (20)
	0,	0,	0,	0,	0,	0,	0,	0,	//40 (28)
	0,	0,	0,	0,	0,	0,	0,	0,	//48 (30)
	0,	0,	0,	0,	0,	0,	0,	0,	//56 (38)
//indexes above this line are reserved for cBaseObject
	5,	8,255,	4,	4,255,	2,	1,	//64 (40)
	4, 12,	4,	1,	2,	3,	8,	1,	//72 (48)
	2,	5,	4,	7,	4,	1,	4,	1,	//80 (50)
//88 to 95 used for multis
	4,	4,	4,	2,	0,	0,	0,	0,	//88 (58)
//-------------------------------------------------
	0,	0,	0,	0,	0,	0,	0,	0,	//96 (60)
	0,	0,	0,	0,	0,	0,	0,	0,	//104 (68)
	0,	0,	0,	0,	0,	0,	0,	0,	//112 (70)
	0,	0,	0,	0,	0,	0,	0,	0,	//120 (78)
	0,	0,	0,	0,	0,	0,	0,	0,	//128 (80)
	0,	0,	0,	0,	0,	0,	0,	0,	//136 (88)
	0,	0,	0,	0,	0,	0,	0,	0,	//144 (90)
	0,	0,	0,	0,	0,	0,	0,	0,	//152 (98)
	0,	0,	0,	0,	0,	0,	0,	0,	//160 (a0)
	0,	0,	0,	0,	0,	0,	0,	0,	//168 (a8)
	0,	0,	0,	0,	0,	0,	0,	0,	//176 (b0)
	0,	0,	0,	0,	0,	0,	0,	0,	//184 (b8)
	0,	0,	0,	0,	0,	0,	0,	0,	//192 (c0)
	0,	0,	0,	0,	0,	0,	0,	0,	//200 (c8)
	0,	0,	0,	0,	0,	0,	0,	0,	//208 (d0)
	0,	0,	0,	0,	0,	0,	0,	0,	//216 (d8)
	0,	0,	0,	0,	0,	0,	0,	0,	//224 (e0)
	0,	0,	0,	0,	0,	0,	0,	0,	//232 (e8)
	0,	0,	0,	0,	0,	0,	0,	0,	//240 (f0)
	0,	0,	0,	0,	0,	0,	0,	0,	//248 (f8)
	//ff
};

