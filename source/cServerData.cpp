#include "uox3.h"

#if UOX_PLATFORM != PLATFORM_WIN32
	#include <dirent.h>
#else
	#include <direct.h>
#endif

namespace UOX
{
#define	MAX_TRACKINGTARGETS	128
#define SKILLCAP			7000
#define STATCAP				325

// New uox3.ini format lookup	
// (January 13, 2001 - EviLDeD) Modified: January 30, 2001 Converted to uppercase
// (February 26 2002 - EviLDeD) Modified: to support the AccountIsolation, and left out dir3ectory tags
// (September 22 2002 - EviLDeD) Added the  "HIDEWILEMOUNTED" tag to support Xuri hide fix
// (September 06 2003 - Maarc) Removed unused tags (heartbeat, wilderness, uoxbot, lagfix)
// (October 16, 2003 - giwo) Removed unused tag (SAVEMODE) and added "WEIGHTPERSTR".
// (April 3, 2004 = EviLDeD) Added new tags, for UOG support, as well as new facet tags etc.
// (June 15, 2004 - EviLDeD) Added the new tags for the xFTPd, and xFTPc support.
// NOTE:	Very important the first lookups required duplication or the search fails on them
const std::string UOX3INI_LOOKUP("|SERVERNAME|SERVERNAME|CONSOLELOG|CRASHPROTECTION|COMMANDPREFIX|ANNOUNCEWORLDSAVES|JOINPARTMSGS|MULCACHING|BACKUPSENABLED|SAVESTIMER|" 
	"SKILLCAP|SKILLDELAY|STATCAP|MAXSTEALTHMOVEMENTS|MAXSTAMINAMOVEMENTS|ARMORAFFECTMANAREGEN|CORPSEDECAYTIMER|WEATHERTIMER|SHOPSPAWNTIMER|DECAYTIMER|INVISIBILITYTIMER|"
	"OBJECTUSETIMER|GATETIMER|POISONTIMER|LOGINTIMEOUT|HITPOINTREGENTIMER|STAMINAREGENTIMER|MANAREGENTIMER|BASEFISHINGTIMER|RANDOMFISHINGTIMER|SPIRITSPEAKTIMER|"
	"DIRECTORY|DATADIRECTORY|DEFSDIRECTORY|ACTSDIRECTORY|SCRIPTSDIRECTORY|BACKUPDIRECTORY|MSGBOARDDIRECTORY|SHAREDDIRECTORY|LOOTDECAYSWITHCORPSE|GUARDSACTIVE|DEATHANIMATION|"
	"AMBIENTSOUNDS|AMBIENTFOOTSTEPS|INTERNALACCOUNTCREATION|SHOWHIDDENNPCS|ROGUESENABLED|PLAYERPERSECUTION|ACCOUNTFLUSH|HTMLSTATUSENABLED|"
	"SELLBYNAME|SELLMAXITEMS|TRADESYSTEM|RANKSYSTEM|CUTSCROLLREQUIREMENTS|SPEEDCHECKITEMS|SPEEDCHECKBOATS|SPEEDCHECKNPCAI|"
	"SPEEDCHECKSPAWNREGIONS|MSGBOARDPOSTINGLEVEL|MSGBOARDREMOVALLEVEL|ESCORTENABLED|ESCORTINITEXPIRE|ESCORTACTIVEEXPIRE|LIGHTMOON1|LIGHTMOON2|"
	"LIGHTDUNGEONLEVEL|LIGHTCURRENTLEVEL|LIGHTBRIGHTLEVEL|TRACKINGBASERANGE|TRACKINGBASETIMER|TRACKINGMAXTARGETS|TRACKINGMSGREDISPLAYTIME|"
	"REPSYSMURDERDECAYTIMER|REPSYSMAXKILLS|REPSYSCRIMINALTIMER|RESOURCEMINECHECK|RESOURCEOREPERAREA|RESOURCEORERESPAWNTIMER|RESOURCEORERESPAWNAREA|RESOURCELOGSPERAREA|RESPURCELOGSRESPAWNTIMER|RESOURCELOGSRESPAWNAREA|HUNGERRATE|HUNGERDMGVAL|"
	"COMBATMAXRANGE|COMBATSPELLMAXRANGE|COMBATDISPLAYHITMSG|COMBATMONSTERSVSANIMALS|"
	"COMBATANIMALATTACKCHANCE|COMBATANIMALSGUARDED|COMBATNPCDAMAGERATE|COMBATNPCBASEFLEEAT|COMBATNPCBASEREATTACKAT|COMBATATTACKSTAMINA|LOCATION|STARTGOLD|STARTPRIVS|ESCORTDONEEXPIRE|LIGHTDARKLEVEL|"
	"TITLECOLOUR|LEFTTEXTCOLOUR|RIGHTTEXTCOLOUR|BUTTONCANCEL|BUTTONLEFT|BUTTONRIGHT|BACKGROUNDPIC|POLLTIME|MAYORTIME|TAXPERIOD|GUARDSPAID|DAY|HOURS|MINUTES|SECONDS|AMPM|SKILLLEVEL|SNOOPISCRIME|BOOKSDIRECTORY|SERVERLIST|PORT|"
	"ACCESSDIRECTORY|LOGSDIRECTORY|ACCOUNTISOLATION|HTMLDIRECTORY|SHOOTONANIMALBACK|NPCTRAININGENABLED|DICTIONARYDIRECTORY|BACKUPSAVERATIO|HIDEWILEMOUNTED|SECONDSPERUOMINUTE|WEIGHTPERSTR|POLYDURATION|"
	"UOGENABLED|NETRCVTIMEOUT|NETSNDTIMEOUT|NETRETRYCOUNT|USEFACETSAVES|MAP0|MAP1|MAP2|MAP3|USERMAP|CLIENTSUPPORT|"
	"FTPDENABLED|FTPDUSER|FTPDUSERLIMIT|FTPDBIND|FTPDROOT|FTPCENABLED|FTPCHOST|FTPCPORT|FTPCUSER|FTPCULFLAGS"
);

void CServerData::ResetDefaults( void )
{
	resettingDefaults = true;
	serverList.resize( 1 );		// Set the initial count to hold one server. 

	ServerIP( "127.0.0.1" );
	ServerPort( 2593 );
	serverList[0].setPort( 2593 );
	ServerName( "Default UOX3 Server" );

	SystemTimer( tSERVER_POTION, 10 );
	ServerMoon( 0, 0 );
	ServerMoon( 1, 0 );
	DungeonLightLevel( 3 );
	WorldLightCurrentLevel( 0 );
	WorldLightBrightLevel( 0 );
	WorldLightDarkLevel( 5 );

	ServerSecondsPerUOMinute( 5 );
	ServerTimeDay( 0 );
	ServerTimeHours( 0 );
	ServerTimeMinutes( 0 );
	ServerTimeSeconds( 0 );
	ServerTimeAMPM( 0 );

	ServerMulCaching( false );
	ServerCrashProtection( 1 );
	InternalAccountStatus( false );
	CombatMaxRange( 10 );
	CombatMaxSpellRange( 10 );
	CombatExplodeDelay( 0 );
	
	// load defaults values
	SystemTimer( tSERVER_SHOPSPAWN, 300 );
	SystemTimer( tSERVER_POISON, 180 );
	SystemTimer( tSERVER_DECAY, 300 );
	ServerSkillCap( 7000 );
	ServerStatCap( 325 );
	PlayerCorpseDecayMultiplier( 3 );
	CorpseLootDecay( true );
	ServerSavesTimer( 300 );
	
	SystemTimer( tSERVER_INVISIBILITY, 60 );
	SystemTimer( tSERVER_HUNGERRATE, 6000 );
	HungerDamage( 2 );
	
	ServerSkillDelay( 5 );
	SystemTimer( tSERVER_OBJECTUSAGE, 1 );
	SystemTimer( tSERVER_HITPOINTREGEN, 8 );
	SystemTimer( tSERVER_STAMINAREGEN, 3 );
	SystemTimer( tSERVER_MANAREGEN, 5 );
	ArmorAffectManaRegen( true );
	SystemTimer( tSERVER_GATE, 30 );
	MineCheck( 2 );
	DeathAnimationStatus( true );
	ShowHiddenNpcStatus( true );
	CombatDisplayHitMessage( true );
	CombatAttackStamina( -2 );
	NPCTrainingStatus( true );
	CharHideWhileMounted( true );
	WeightPerStr( 5 );
	SystemTimer( tSERVER_POLYMORPH, 90 );

	CombatMonstersVsAnimals( true );
	CombatAnimalsAttackChance( 15 );
	CombatAnimalsGuarded( false );
	CombatNPCBaseFleeAt( 20 );
	CombatNPCBaseReattackAt( 40 );
	ShootOnAnimalBack( false );
	SellByNameStatus( false );
	SkillLevel( 5 );
	SellMaxItemsStatus( 5 );
	CombatNPCDamageRate( 2 );
	RankSystemStatus( true );

	char curWorkingDir[1024];
	GetCurrentDirectory( 1024, curWorkingDir );
	UString wDir( curWorkingDir );
	wDir = wDir.fixDirectory();
	UString tDir;
	Directory( CSDDP_ROOT, wDir );
	tDir = wDir + "muldata/";
	Directory( CSDDP_DATA, tDir );
	tDir = wDir + "dfndata/";
	Directory( CSDDP_DEFS, tDir );
	tDir = wDir + "accounts/";
	Directory( CSDDP_ACCOUNTS, tDir );
	Directory( CSDDP_ACCESS, tDir );
	tDir = wDir + "js/";
	Directory( CSDDP_SCRIPTS, tDir );
	tDir = wDir + "archives/";
	Directory( CSDDP_BACKUP, tDir );
	tDir = wDir + "msgboards/";
	Directory( CSDDP_MSGBOARD, tDir );
	tDir = wDir + "shared/";
	Directory( CSDDP_SHARED, tDir );
	tDir = wDir + "html/";
	Directory( CSDDP_HTML, tDir );
	tDir = wDir + "books/";
	Directory( CSDDP_BOOKS, tDir );
	tDir = wDir + "dictionaries/";
	Directory( CSDDP_DICTIONARIES, tDir );
	tDir = wDir + "logs/";
	Directory( CSDDP_LOGS, tDir );

	BuyThreshold( 2000 );
	GuardStatus( true );
	ServerAnnounceSaves( true );
	WorldAmbientSounds( 5 );
	ServerJoinPartAnnouncements( true );
	ServerConsoleLog( 1 );
	RogueStatus( true );
	SystemTimer( tSERVER_WEATHER, 60 );
	SystemTimer( tSERVER_LOGINTIMEOUT, 300 );
	BackupRatio( 5 );
	MaxStealthMovement( 10 );
	MaxStaminaMovement( 15 );
	SnoopIsCrime( false );
	
	CheckBoatSpeed( 0.75 );
	CheckNpcAISpeed( 0.5 );
	CutScrollRequirementStatus( true );
	PlayerPersecutionStatus( true );
	HtmlStatsStatus( -1 );
	
	MsgBoardPostingLevel( 0 );
	MsgBoardPostRemovalLevel( 0 );
	// No replacement I can see
	EscortsEnabled( true );
	SystemTimer( tSERVER_ESCORTWAIT, 10 );
	SystemTimer( tSERVER_ESCORTACTIVE, 20 );
	SystemTimer( tSERVER_ESCORTDONE, 1800 );
	AmbientFootsteps( false );
	ServerCommandPrefix( '\'' );
	
	CheckSpawnRegionSpeed( 30 );
	CheckItemsSpeed( 1.5 );
	NPCSpeed( 1.0 );
	AccountFlushTimer( 0.0 );
	
	ResLogs( 3 );
	ResLogTime( 600 );
	ResLogArea( 10 );
	ResOre( 10 );
	ResOreTime( 600 );
	ResOreArea( 10 );
	//REPSYS
	SystemTimer( tSERVER_CRIMINAL, 120 );
	RepMaxKills( 4 );
	SystemTimer( tSERVER_MURDERDECAY, 60 );
	//RepSys ---^
	TrackingBaseRange( 10 );
	TrackingMaxTargets( 20 );
	TrackingBaseTimer( 30 );
	TrackingRedisplayTime( 30 );

	SystemTimer( tSERVER_FISHINGBASE, 10 );
	SystemTimer( tSERVER_FISHINGRANDOM, 5 );
	SystemTimer( tSERVER_SPIRITSPEAK, 30 );

	// Abaddon, March 21, 2000

	TitleColour( 0 );
	LeftTextColour( 0 );
	RightTextColour( 0 );
	ButtonCancel( 4017 );
	ButtonLeft( 4014 );
	ButtonRight( 4005 );
	BackgroundPic( 2600 );

	TownNumSecsPollOpen( 3600 );	// one hour
	TownNumSecsAsMayor( 36000 );	// 10 hours as mayor
	TownTaxPeriod( 1800 );			// taxed every 30 minutes
	TownGuardPayment( 3600 );		// guards paid every hour

	ServerStartGold( 1000 );
	ServerStartPrivs( 0 );
	SystemTimer( tSERVER_CORPSEDECAY, 3 );
	resettingDefaults = false;
	PostLoadDefaults();	
}
CServerData::CServerData( void )
{
	ResetDefaults();
}
CServerData::~CServerData()
{
}

void CServerData::ServerName( std::string setname )
{
	if( serverList.empty() )
		serverList.resize( 1 );
	serverList[0].setName( setname );
	if( setname.empty() )
	{
		serverList[0].setName( "Default UOX Server" );
//		sServerName = "www.uox3dev.net";
	}
	else
	{
//		sServerName = setname;
	}
}

std::string CServerData::ServerName( void ) const
{
	return serverList[0].getName();
}

void CServerData::ServerDomain( std::string setdomain )
{
	if( serverList.empty() )
		serverList.resize( 1 );
	if( setdomain.empty() )
		serverList[0].setDomain( "" );
	else
		serverList[0].setDomain( setdomain );
}

std::string CServerData::ServerDomain( void ) const
{
	return serverList[0].getDomain();
}

void CServerData::ServerIP( std::string setip )
{
	if( serverList.empty() )
		serverList.resize( 1 );
	if( setip.empty() )
		serverList[0].setIP("127.0.0.1");
	else
		serverList[0].setIP(setip);
}

std::string CServerData::ServerIP( void ) const
{
	return serverList[0].getIP();
}

void CServerData::ServerPort( UI16 setport )
{
	if( setport == 0 )
		port = 2593;
	else
		port = setport;
}

UI16 CServerData::ServerPort( void ) const
{
	return port;
}

void CServerData::ServerConsoleLog( UI08 setting )
{
	consolelogenabled = setting;
}

UI08 CServerData::ServerConsoleLogStatus( void ) const
{
	return consolelogenabled;
}

void CServerData::ServerCrashProtection( UI08 setting )
{
	crashprotectionenabled = (setting > 5 && !(setting < 1)) ? 1 : setting;
}

UI08 CServerData::ServerCrashProtectionStatus( void ) const
{
	return crashprotectionenabled;
}

void CServerData::ServerCommandPrefix( char cmdvalue )
{
	commandprefix = cmdvalue;
}

char CServerData::ServerCommandPrefix( void ) const
{
	return commandprefix;
}

void CServerData::ServerAnnounceSaves( bool setting )
{
	announcesaves = setting;
}

bool CServerData::ServerAnnounceSavesStatus( void ) const
{
	return announcesaves;
}

void CServerData::ServerJoinPartAnnouncements( bool setting )
{
	joinpartmsgsenabled = setting;
}

bool CServerData::ServerJoinPartAnnouncementsStatus( void ) const
{
	return joinpartmsgsenabled;
}

void CServerData::ServerMulCaching( bool setting )
{
	mulcachingenabled = setting;
}

bool CServerData::ServerMulCachingStatus( void ) const
{
	return mulcachingenabled;
}

void CServerData::ServerBackups( bool setting )
{
	backupsenabled = setting;
}

bool CServerData::ServerBackupStatus( void ) const
{
	return backupsenabled;
}

void CServerData::ServerSavesTimer( UI32 timer )
{
	serversavestimer = timer;
	if( timer < 180 )						// 3 minutes is the lowest value you can set saves for
		serversavestimer = 300;	// 10 minutes default
}

UI32 CServerData::ServerSavesTimerStatus( void ) const
{
	return serversavestimer;
}

void CServerData::ServerSkillCap( UI16 cap )
{
	skillcap = cap;
	if( cap < 1 )		// Default is on second loop sleeping
		skillcap = SKILLCAP;
}

UI16 CServerData::ServerSkillCapStatus( void ) const
{
	return skillcap;
}

void CServerData::ServerSkillDelay( UI08 skdelay )
{
	skilldelay = skdelay;
}

UI08 CServerData::ServerSkillDelayStatus( void ) const
{
	return skilldelay;
}

void CServerData::ServerStatCap( UI16 cap )
{
	statcap = cap;
	if( cap < 1 )		// Default is on second loop sleeping
		statcap = STATCAP;
}

UI16 CServerData::ServerStatCapStatus( void ) const
{
	return statcap;
}

void CServerData::MaxStealthMovement( SI16 value )
{
	maxstealthmovement = value;
}

SI16 CServerData::MaxStealthMovement( void ) const
{
	return maxstealthmovement;
}

void CServerData::MaxStaminaMovement( SI16 value )
{
	maxstaminamovement = value;
}

SI16 CServerData::MaxStaminaMovement( void ) const
{
	return maxstaminamovement;
}

R32 CServerData::BuildSystemTimeValue( cSD_TID timerID ) const
{
	return BuildTimeValue( static_cast<R32>(SystemTimer( timerID )) );
}

void CServerData::SystemTimer( cSD_TID timerid, UI16 value )
{
	serverTimers[timerid] = value;
}

UI16 CServerData::SystemTimer( cSD_TID timerid ) const
{
	return serverTimers[timerid];
}

std::string CServerData::Directory( CSDDirectoryPaths dp )
{
	std::string rvalue;
	if( dp != CSDDP_COUNT )
		rvalue = serverDirectories[dp];
	return rvalue;
}

void CServerData::Directory( CSDDirectoryPaths dp, std::string value )
{
	if( dp != CSDDP_COUNT )
	{
		std::string verboseDirectory;
		switch( dp )
		{
			case CSDDP_ROOT:			verboseDirectory = "Root directory";			break;
			case CSDDP_DATA:			verboseDirectory = "Data directory";			break;
			case CSDDP_DEFS:			verboseDirectory = "DFNs directory";			break;
			case CSDDP_ACCESS:			verboseDirectory = "Access directory";			break;
			case CSDDP_ACCOUNTS:		verboseDirectory = "Accounts directory";		break;
			case CSDDP_SCRIPTS:			verboseDirectory = "Scripts directory";			break;
			case CSDDP_BACKUP:			verboseDirectory = "Backup directory";			break;
			case CSDDP_MSGBOARD:		verboseDirectory = "Messageboard directory";	break;
			case CSDDP_SHARED:			verboseDirectory = "Shared directory";			break;
			case CSDDP_HTML:			verboseDirectory = "HTML directory";			break;
			case CSDDP_LOGS:			verboseDirectory = "Logs directory";			break;
			case CSDDP_DICTIONARIES:	verboseDirectory = "Dictionary directory";		break;
			case CSDDP_BOOKS:			verboseDirectory = "Books directory";			break;
			case CSDDP_COUNT:
			default:					verboseDirectory = "Unknown directory";			break;
		};
		// First, let's normalize the path name and fix common errors
		UString sText( value );
		// remove all trailing and leading spaces...
		sText = sText.stripWhiteSpace();
		if( sText.empty() )
		{
			Console.Error( 1, " %s directory is blank, set in uox.ini", verboseDirectory.c_str() );
			Shutdown( FATAL_UOX3_DIR_NOT_FOUND );
			return;
		}

		// Make sure we're terminated with a directory separator
		// Just incase it's not set in the .ini
		// and convert the windows backward slashes to forward slashes

		sText = sText.fixDirectory();

		bool error = false;
		if( !resettingDefaults )
		{
	#if UOX_PLATFORM != PLATFORM_WIN32
			DIR *dirPtr = opendir( sText.c_str() );
			if( !dirPtr )
				error = true;
			else
				closedir( dirPtr );
	#else
			char curWorkingDir[1024];
			GetCurrentDirectory( 1024, curWorkingDir );
			int iResult = _chdir( sText.c_str() );
			if( iResult != 0 )
				error = true;
			else
				_chdir( curWorkingDir );	// move back to where we were
	#endif
		}

		if( error )
		{
				Console.Error( 1, " %s directory %s does not exist", verboseDirectory.c_str(), sText.c_str() );
				Shutdown( FATAL_UOX3_DIR_NOT_FOUND );
		}
		else
		{
			// There was a check to see if text was empty, to set to "./".  However, if text was blank, we bailed out in the
			// beginning of the routine
			serverDirectories[dp] = value;
		}
	}
}

bool CServerData::ShootOnAnimalBack( void ) const
{
	return shootonanimalback;
}

void CServerData::ShootOnAnimalBack( bool setting )
{
	shootonanimalback = setting;
}

bool CServerData::NPCTrainingStatus( void ) const
{
	return npctraining;
}

void CServerData::NPCTrainingStatus( bool setting )
{
	npctraining = setting;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	void CServerData::dumpPaths( void )
//|	Date			-	02/26/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	Needed a function that would dump out the paths. If you
//|							add any paths to the server please make sure that you
//|							place exports to the console here to please so it can
//|							belooked up if needed. 
//o--------------------------------------------------------------------------o
//|	Returns				-	N/A
//o--------------------------------------------------------------------------o
void CServerData::dumpPaths( void )
{
	Console.PrintSectionBegin();
	Console << "PathDump: \n";
	Console << "    Root      : " << Directory( CSDDP_ROOT ) << "\n";
	Console << "    Accounts  : " << Directory( CSDDP_ACCOUNTS ) << "\n";
	Console << "    Access    : " << Directory( CSDDP_ACCESS ) << "\n";
	Console << "    Mul(Data) : " << Directory( CSDDP_DATA ) << "\n";
	Console << "    DFN(Defs) : " << Directory( CSDDP_DEFS ) << "\n";
	Console << "    JScript   : " << Directory( CSDDP_SCRIPTS ) << "\n";
	Console << "    HTML      : " << Directory( CSDDP_HTML ) << "\n";
	Console << "    MSGBoards : " << Directory( CSDDP_MSGBOARD ) << "\n";
	Console << "    Books     : " << Directory( CSDDP_BOOKS ) << "\n";
	Console << "    Shared    : " << Directory( CSDDP_SHARED ) << "\n";
	Console << "    Backups   : " << Directory( CSDDP_BACKUP ) << "\n";
	Console << "    Logs      : " << Directory( CSDDP_LOGS ) << "\n";
	Console.PrintSectionBegin();
}

void CServerData::CorpseLootDecay( bool value )
{
	lootdecayswithcorpse = value;
}

bool CServerData::CorpseLootDecay( void ) const
{
	return lootdecayswithcorpse;
}

void CServerData::GuardStatus( bool value )
{
	guardsenabled = value;
}

bool CServerData::GuardsStatus( void ) const
{
	return guardsenabled;
}

void CServerData::DeathAnimationStatus( bool value )
{
	deathanimationenabled = value;
}

bool CServerData::DeathAnimationStatus( void ) const
{
	return deathanimationenabled;
}

void CServerData::WorldAmbientSounds( SI16 value )
{
	if( value < 0 )
		ambientsounds = 0;
	else
		ambientsounds = value;
}

SI16 CServerData::WorldAmbientSounds( void ) const
{
	return ambientsounds;
}

void CServerData::AmbientFootsteps( bool value )
{
	ambientfootsteps = value;
}

bool CServerData::AmbientFootsteps( void ) const
{
	return ambientfootsteps;
}

void CServerData::InternalAccountStatus( bool value )
{
	internalaccountsenabled = value;
}

bool CServerData::InternalAccountStatus( void ) const
{
	return internalaccountsenabled;
}

void CServerData::ShowHiddenNpcStatus( bool value )
{
	showhiddennpcsenabled = value;
}

bool CServerData::ShowHiddenNpcStatus( void ) const
{
	return showhiddennpcsenabled;
}

void CServerData::RogueStatus( bool value )
{
	roguesenabled = value;
}

bool CServerData::RogueStatus( void ) const
{
	return roguesenabled;
}

void CServerData::SnoopIsCrime( bool value )
{
	snoopiscrime = value;
}
bool CServerData::SnoopIsCrime( void ) const
{
	return snoopiscrime;
}

void CServerData::PlayerPersecutionStatus( bool value )
{
	playerpersecution = value;
}

bool CServerData::PlayerPersecutionStatus( void ) const
{
	return playerpersecution;
}

void CServerData::HtmlStatsStatus( SI16 value )
{
	htmlstatusenabled = value;
}

SI16 CServerData::HtmlStatsStatus( void ) const
{
	return htmlstatusenabled;
}

void CServerData::SellByNameStatus( bool value )
{
	sellbynameenabled = value;
}

bool CServerData::SellByNameStatus( void ) const
{
	return sellbynameenabled;
}

void CServerData::SellMaxItemsStatus( SI16 value )
{
	sellmaxitems = value;
}

SI16 CServerData::SellMaxItemsStatus( void ) const
{
	return sellmaxitems;
}

void CServerData::TradeSystemStatus( bool value )
{
	tradesystemenabled = value;
}

bool CServerData::TradeSystemStatus( void ) const
{
	return tradesystemenabled;
}

void CServerData::RankSystemStatus( bool value )
{
	ranksystemenabled = value;
}

bool CServerData::RankSystemStatus( void ) const
{
	return ranksystemenabled;
}

void CServerData::CutScrollRequirementStatus( SI16 value )
{
	cutscrollrequirement = value;
}

SI16 CServerData::CutScrollRequirementStatus( void ) const
{
	return cutscrollrequirement;
}

void CServerData::CheckItemsSpeed( R64 value )
{
	if( value < 0.0 )
		checkitems = 0.0;
	else
		checkitems = value;
}

R64 CServerData::CheckItemsSpeed( void ) const
{
	return checkitems;
}

void CServerData::CheckBoatSpeed( R64 value )
{
	if( value < 0.0 )
		checkboats = 0.0;
	else
		checkboats = value;
}

R64 CServerData::CheckBoatSpeed( void ) const
{
	return checkboats;
}

void CServerData::CheckNpcAISpeed( R64 value )
{
	if( value < 0.0 )
		checknpcai = 0.0;
	else
		checknpcai = value;
}

R64 CServerData::CheckNpcAISpeed( void ) const
{
	return checknpcai;
}

void CServerData::CheckSpawnRegionSpeed( R64 value )
{
	if( value < 0.0 )
		checkspawnregions = 0.0;
	else
		checkspawnregions = value;
}

R64 CServerData::CheckSpawnRegionSpeed( void ) const
{
	return checkspawnregions;
}

void CServerData::MsgBoardPostingLevel( UI08 value )
{
	msgpostinglevel = value;
}

UI08 CServerData::MsgBoardPostingLevel( void ) const
{
	return msgpostinglevel;
}

void CServerData::MsgBoardPostRemovalLevel( UI08 value )
{
	msgremovallevel = value;
}

UI08 CServerData::MsgBoardPostRemovalLevel( void ) const
{
	return msgremovallevel;
}

void CServerData::MineCheck( UI08 value )
{
	minecheck = value;
}

UI08 CServerData::MineCheck( void ) const
{
	return minecheck;
}

void CServerData::CombatDisplayHitMessage( bool value )
{
	combatdisplayhitmessage = value;
}

bool CServerData::CombatDisplayHitMessage( void ) const
{
	return combatdisplayhitmessage;
}

void CServerData::CombatNPCDamageRate( SI16 value )
{
	combatnpcdamagerate = value;
}

SI16 CServerData::CombatNPCDamageRate( void ) const
{
	return combatnpcdamagerate;
}

void CServerData::CombatAttackStamina( SI16 value )
{
	combatattackstamina = value;
}

SI16 CServerData::CombatAttackStamina( void ) const
{
	return combatattackstamina;
}

UI08 CServerData::SkillLevel( void ) const
{
	return skilllevel;
}
void CServerData::SkillLevel( UI08 value )
{
	skilllevel = value;
}

void CServerData::EscortsEnabled( bool value )
{
	escortsenabled = value;
}
bool CServerData::EscortsEnabled( void ) const
{
	return escortsenabled;
}

void CServerData::CombatMonstersVsAnimals( bool value )
{
	combatmonstersvsanimals = value;
}
bool CServerData::CombatMonstersVsAnimals( void ) const
{
	return combatmonstersvsanimals;
}

void CServerData::CombatAnimalsAttackChance( SI16 value )
{
	combatanimalattackchance = value;
}
SI16 CServerData::CombatAnimalsAttackChance( void ) const
{
	return combatanimalattackchance;
}

void CServerData::HungerDamage( SI16 value )
{
	hungerdamage = value;
}
SI16 CServerData::HungerDamage( void ) const
{
	return hungerdamage;
}

void CServerData::BuyThreshold( SI16 value )
{
	buyThreshold = value;
}
SI16 CServerData::BuyThreshold( void ) const
{
	return buyThreshold;
}


//o--------------------------------------------------------------------------o
//|	Function/Class	-	bool CharHideWhileMounted()
//|	Date			-	09/22/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Toggle characters ability to hide whilst mounted
//o--------------------------------------------------------------------------o
void CServerData::CharHideWhileMounted( bool value )
{
	charhidewhilemounted = value;
}
bool CServerData::CharHideWhileMounted( void ) const
{
	return charhidewhilemounted;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	UI08 WeightPerStr()
//|	Date			-	3/12/2003
//|	Developer(s)	-	Zane
//|	Company/Team	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Purpose			-	Amount of Weight one can hold per point of STR
//o--------------------------------------------------------------------------o
UI08 CServerData::WeightPerStr( void ) const
{
	return weightPerSTR;
}
void CServerData::WeightPerStr( UI08 newVal )
{
	weightPerSTR = newVal;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	bool ArmorAffectManaRegen()
//|	Date			-	3/20/2005
//|	Developer(s)	-	giwo
//|	Company/Team	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Purpose			-	Toggles whether or not armor affects mana regeneration
//o--------------------------------------------------------------------------o
bool CServerData::ArmorAffectManaRegen( void ) const
{
	return armorAffectManaRegen;
}
void CServerData::ArmorAffectManaRegen( bool newVal )
{
	armorAffectManaRegen = newVal;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	UI08 PlayerCorpseDecayMultiplier()
//|	Date			-	3/20/2005
//|	Developer(s)	-	giwo
//|	Company/Team	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Purpose			-	Multiplier for player corpse decay
//o--------------------------------------------------------------------------o
UI08 CServerData::PlayerCorpseDecayMultiplier( void ) const
{
	return playerCorpseDecayMultiplier;
}
void CServerData::PlayerCorpseDecayMultiplier( UI08 newVal )
{
	playerCorpseDecayMultiplier = newVal;
}

void CServerData::BackupRatio( SI16 value )
{
	backupRatio = value;
}
SI16 CServerData::BackupRatio( void ) const
{
	return backupRatio;
}

void CServerData::CombatMaxRange( SI16 value )
{
	combatmaxrange = value;
}
SI16 CServerData::CombatMaxRange( void ) const
{
	return combatmaxrange;
}

void CServerData::CombatMaxSpellRange( SI16 value )
{
	combatmaxspellrange = value;
}
SI16 CServerData::CombatMaxSpellRange( void ) const
{
	return combatmaxspellrange;
}

void CServerData::CombatExplodeDelay( UI16 value )
{
	combatExplodeDelay = value;
}
UI16 CServerData::CombatExplodeDelay( void ) const
{
	return combatExplodeDelay;
}

void CServerData::CombatAnimalsGuarded( bool value )
{
	combatanimalsguarded = value;
}
bool CServerData::CombatAnimalsGuarded( void ) const
{
	return combatanimalsguarded;
}

void CServerData::CombatNPCBaseFleeAt( SI16 value )
{
	combatnpcbasefleeat = value;
}
SI16 CServerData::CombatNPCBaseFleeAt( void ) const
{
	return combatnpcbasefleeat;
}

void CServerData::CombatNPCBaseReattackAt( SI16 value )
{
	combatnpcbasereattackat = value;
}
SI16 CServerData::CombatNPCBaseReattackAt( void ) const
{
	return combatnpcbasereattackat;
}

void CServerData::NPCSpeed( R64 value )
{
	npcSpeed = value;
}
R64 CServerData::NPCSpeed( void ) const
{
	return npcSpeed;
}

void CServerData::TitleColour( UI16 value )
{
	titleColour = value;
}
UI16 CServerData::TitleColour( void ) const
{
	return titleColour;
}

void CServerData::LeftTextColour( UI16 value )
{
	leftTextColour = value;
}
UI16 CServerData::LeftTextColour( void ) const
{
	return leftTextColour;
}

void CServerData::RightTextColour( UI16 value )
{
	rightTextColour = value;
}
UI16 CServerData::RightTextColour( void ) const
{
	return rightTextColour;
}

void CServerData::ButtonCancel( UI16 value )
{
	buttonCancel = value;
}
UI16 CServerData::ButtonCancel( void ) const
{
	return buttonCancel;
}

void CServerData::ButtonLeft( UI16 value )
{
	buttonLeft = value;
}
UI16 CServerData::ButtonLeft( void ) const
{
	return buttonLeft;
}

void CServerData::ButtonRight( UI16 value )
{
	buttonRight = value;
}
UI16 CServerData::ButtonRight( void ) const
{
	return buttonRight;
}

void CServerData::BackgroundPic( UI16 value )
{
	backgroundPic = value;
}
UI16 CServerData::BackgroundPic( void ) const
{
	return backgroundPic;
}

void CServerData::TownNumSecsPollOpen( UI32 value )
{
	numSecsPollOpen = value;
}
UI32 CServerData::TownNumSecsPollOpen( void ) const
{
	return numSecsPollOpen;
}

void CServerData::TownNumSecsAsMayor( UI32 value )
{
	numSecsAsMayor = value;
}
UI32 CServerData::TownNumSecsAsMayor( void ) const
{
	return numSecsAsMayor;
}

void CServerData::TownTaxPeriod( UI32 value )
{
	taxPeriod = value;
}
UI32 CServerData::TownTaxPeriod( void ) const
{
	return taxPeriod;
}

void CServerData::TownGuardPayment( UI32 value )
{
	guardPayment = value;
}
UI32 CServerData::TownGuardPayment( void ) const
{
	return guardPayment;
}

void CServerData::RepMaxKills( UI16 value )
{
	maxmurdersallowed = value;
}
UI16 CServerData::RepMaxKills( void ) const
{
	return maxmurdersallowed;
}

void CServerData::ResLogs( SI16 value )
{
	logsperarea = value;
}
SI16 CServerData::ResLogs( void ) const
{
	return logsperarea;
}

void CServerData::ResLogTime( UI16 value )
{
	logsrespawntimer = value;
}
UI16 CServerData::ResLogTime( void ) const
{
	return logsrespawntimer;
}

void CServerData::ResLogArea( UI16 value )
{
	logsrespawnarea = value;
}
UI16 CServerData::ResLogArea( void ) const
{
	return logsrespawnarea;
}

void CServerData::ResOre( SI16 value )
{
	oreperarea = value;
}
SI16 CServerData::ResOre( void ) const
{
	return oreperarea;
}

void CServerData::ResOreTime( UI16 value )
{
	orerespawntimer = value;
}
UI16 CServerData::ResOreTime( void ) const
{
	return orerespawntimer;
}

void CServerData::ResOreArea( UI16 value )
{
	orerespawnarea = value;
}
UI16 CServerData::ResOreArea( void ) const
{
	return orerespawnarea;
}

void CServerData::AccountFlushTimer( R64 value )
{
	flushTime = value;
}
R64 CServerData::AccountFlushTimer( void ) const
{
	return flushTime;
}


//o--------------------------------------------------------------------------o
//|	Function/Class	-	bool CServerData::save( void )
//|	Date			-	02/21/2002
//|	Developer(s)	-	Unknown
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	Save the uox.ini out. This is the default save
//o--------------------------------------------------------------------------o
//|	Returns			- [TRUE] If successfull
//o--------------------------------------------------------------------------o
bool CServerData::save( void )
{
	std::string s = Directory( CSDDP_ROOT );
	s += "uox.ini";
	return save( s );
}


//o--------------------------------------------------------------------------o
//|	Function/Class	-	bool CServerData::save( std::string filename )
//|	Date			-	Unknown
//|	Developer(s)	-	EviLDeD/Unknown
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	This is the full uox.ini save routing. 
//|									
//|	Modification	-	02/21/2002	-	Fixed paths not being saved back out 
//|									
//|	Modification	-	02/21/2002	-	Added ini support for the isloation system
//|									At this point it just defaults to Isolation level 1.
//|									
//|	Modification	-	02/27/2002	-	Made sure that ALL directory paths are
//|									written out to the ini file.
//|									
//|	Modification	-	04/03/2004	-	Added new tags to the system group
//|									 
//|										SERVERNAME		: Name of the server.
//|										NETRCVTIMEOUT	: Timeout in seconds for recieving data
//|										NETSNDTIMEOUT	: Timeout in seconds for sending data
//|										UOGENABLED		: Does this server respond to UOGInfo Requests?
//|																		0-No 1-Yes
//|									
//|	Modification	-	04/03/2004	-	Added new group [facet] with these tags 
//|									
//|										USEFACETSAVES	: Does the server seperate facet data?
//|																		0-No : All data will be saved into the shared folder
//|																		1-Yes: Each Facet will save its data into its own shared folder
//|										MAP0-MAP3			:	Format= MAPx=Mapname,MapAccess
//|									
//|																		Where Mapname is the name of the map and
//|																		MapAccess the set of bit flags that allow/deny features and map access
//|									
//o--------------------------------------------------------------------------o
//|	Returns				-
//o--------------------------------------------------------------------------o
bool CServerData::save( std::string filename )
{
	bool rvalue = false;
	std::ofstream ofsOutput;
	ofsOutput.open( filename.c_str(), std::ios::out );
	if( ofsOutput.is_open() )
	{
		ofsOutput << "// UOX Initialization File. V" << MAKEWORD( 2, 1 ) << std::endl << "//================================" << std::endl << std::endl;
		ofsOutput << "[system]" << std::endl << "{" << std::endl;
		ofsOutput << "SERVERNAME=" << ServerName() << std::endl;
		ofsOutput << "PORT=" << ServerPort() << std::endl;
		ofsOutput << "NETRCVTIMEOUT=" << ServerNetRcvTimeout() << std::endl;
		ofsOutput << "NETSNDTIMEOUT=" << "3" /*ServerNetSndTimeout()*/ << std::endl;
		ofsOutput << "NETRETRYCOUNT=" << ServerNetRetryCount() << std::endl;
		ofsOutput << "CONSOLELOG=" << static_cast<UI16>(ServerConsoleLogStatus()) << std::endl;
		ofsOutput << "CRASHPROTECTION=" << static_cast<UI16>(ServerCrashProtectionStatus()) << std::endl;
		ofsOutput << "COMMANDPREFIX=" << ServerCommandPrefix() << std::endl;
		ofsOutput << "ANNOUNCEWORLDSAVES=" << (ServerAnnounceSavesStatus()?1:0) << std::endl;
		ofsOutput << "JOINPARTMSGS=" << (ServerJoinPartAnnouncementsStatus()?1:0) << std::endl;
		ofsOutput << "MULCACHING=" << (ServerMulCachingStatus()?1:0) << std::endl;
		ofsOutput << "BACKUPSENABLED=" << (ServerBackupStatus()?1:0) << std::endl;
		ofsOutput << "BACKUPSAVERATIO=" << BackupRatio() << std::endl;
		ofsOutput << "SAVESTIMER=" << ServerSavesTimerStatus() << std::endl;
		ofsOutput << "ACCOUNTISOLATION=" << "1" /*AccountsIsolationLevel()*/ << std::endl;
		ofsOutput << "UOGENABLED=" << (ServerUOGEnabled()?1:0) << std::endl;
		ofsOutput << "}" << std::endl << std::endl;

		ofsOutput << "[facet]" << std::endl << "{" << std::endl;
		ofsOutput << "USEFACETSAVES=" << (ServerUseFacetSaves()?1:0) << std::endl;
		//ofsOutput << "MAP0=" << "MapName"/*ServerMap0Name()*/ << "," << "MapAccess"/*ServerMap0Access()*/ << std::endl;
		//ofsOutput << "MAP1=" << "MapName"/*ServerMap1Name()*/ << "," << "MapAccess"/*ServerMap1Access()*/ << std::endl;
		//ofsOutput << "MAP2=" << "MapName"/*ServerMap2Name()*/ << "," << "MapAccess"/*ServerMap2Access()*/ << std::endl;
		//ofsOutput << "MAP3=" << "MapName"/*ServerMap3Name()*/ << "," << "MapAccess"/*ServerMap3Access()*/ << std::endl;
		ofsOutput << "}" << std::endl;

		ofsOutput << std::endl << "[play server list]" << std::endl << "{" << std::endl;

		for( size_t cnt = 0; cnt < serverList.size(); ++cnt )
		{
			ofsOutput << "SERVERLIST=" << serverList[cnt].getName() << ",";
			if( !serverList[cnt].getDomain().empty() )
				ofsOutput << serverList[cnt].getDomain() << ",";
			else
				ofsOutput << serverList[cnt].getIP() << ",";
			ofsOutput << serverList[cnt].getPort() << std::endl;
		}
		ofsOutput << "}" << std::endl;

		ofsOutput << std::endl << "[network server list]" << std::endl << "{" << std::endl;
		ofsOutput << "}" << std::endl;

		// 06152004 - EviLDeD - added the tags for the soon to be added ftpd(light) and ftp(light) threads.
		ofsOutput << std::endl << "[xftpd]" << std::endl << "{" << std::endl;
		ofsOutput << "FTPDENABLED=" << std::endl;
		ofsOutput << "FTPDUSERLIMIT=" << std::endl;
		ofsOutput << "FTPDBIND=" << std::endl;
		ofsOutput << "FTPDPORT=" << std::endl;
		ofsOutput << "FTPDROOT=" << std::endl;
		ofsOutput << "FTPDUSER=" << std::endl;
		ofsOutput << "}" << std::endl;

		ofsOutput << std::endl << "[xftpc]" << std::endl << "{" << std::endl;
		ofsOutput << "FTPCENABLED=" << std::endl;
		ofsOutput << "FTPCEVENT=" << std::endl;
		ofsOutput << "FTPCHOST=" << std::endl;
		ofsOutput << "FTPCPORT=" << std::endl;
		ofsOutput << "FTPCUSER=" << std::endl;
		ofsOutput << "FTPCFLAGS=" << std::endl;
		ofsOutput << "}" << std::endl;

		ofsOutput << std::endl << "[skill & stats]" << std::endl << "{" << std::endl;
		ofsOutput << "SKILLLEVEL=" << static_cast<UI16>(SkillLevel()) << std::endl;
		ofsOutput << "SKILLCAP=" << ServerSkillCapStatus() << std::endl;
		ofsOutput << "SKILLDELAY=" << static_cast<UI16>(ServerSkillDelayStatus()) << std::endl;
		ofsOutput << "STATCAP=" << ServerStatCapStatus() << std::endl;
		ofsOutput << "MAXSTEALTHMOVEMENTS=" << MaxStealthMovement() << std::endl;
		ofsOutput << "MAXSTAMINAMOVEMENTS=" << MaxStaminaMovement() << std::endl;
		ofsOutput << "SNOOPISCRIME=" << (SnoopIsCrime()?1:0) << std::endl;
		ofsOutput << "ARMORAFFECTMANAREGEN=" << (ArmorAffectManaRegen() ? 1 : 0) << std::endl;
		ofsOutput << "}" << std::endl;
		
		ofsOutput << std::endl << "[timers]" << std::endl << "{" << std::endl;
		ofsOutput << "CORPSEDECAYTIMER=" << SystemTimer( tSERVER_CORPSEDECAY ) << std::endl;
		ofsOutput << "WEATHERTIMER=" << SystemTimer( tSERVER_WEATHER ) << std::endl;
		ofsOutput << "SHOPSPAWNTIMER=" << SystemTimer( tSERVER_SHOPSPAWN ) << std::endl;
		ofsOutput << "DECAYTIMER=" << SystemTimer( tSERVER_DECAY ) << std::endl;
		ofsOutput << "INVISIBILITYTIMER=" << SystemTimer( tSERVER_INVISIBILITY ) << std::endl;
		ofsOutput << "OBJECTUSETIMER=" << SystemTimer( tSERVER_OBJECTUSAGE ) << std::endl;
		ofsOutput << "GATETIMER=" << SystemTimer( tSERVER_GATE ) << std::endl;
		ofsOutput << "POISONTIMER=" << SystemTimer( tSERVER_POISON ) << std::endl;
		ofsOutput << "LOGINTIMEOUT=" << SystemTimer( tSERVER_LOGINTIMEOUT ) << std::endl;
		ofsOutput << "HITPOINTREGENTIMER=" << SystemTimer( tSERVER_HITPOINTREGEN ) << std::endl;
		ofsOutput << "STAMINAREGENTIMER=" << SystemTimer( tSERVER_STAMINAREGEN ) << std::endl;
		ofsOutput << "MANAREGENTIMER=" << SystemTimer( tSERVER_MANAREGEN ) << std::endl;
		ofsOutput << "BASEFISHINGTIMER=" << SystemTimer( tSERVER_FISHINGBASE ) << std::endl;
		ofsOutput << "RANDOMFISHINGTIMER=" << SystemTimer( tSERVER_FISHINGRANDOM ) << std::endl;
		ofsOutput << "SPIRITSPEAKTIMER=" << SystemTimer( tSERVER_SPIRITSPEAK ) << std::endl;
		ofsOutput << "}" << std::endl;
		
		ofsOutput << std::endl << "[directories]" << std::endl << "{" << std::endl;
		ofsOutput << "DIRECTORY=" << Directory( CSDDP_ROOT ) << std::endl;
		ofsOutput << "DATADIRECTORY=" << Directory( CSDDP_DATA ) << std::endl;
		ofsOutput << "DEFSDIRECTORY=" << Directory( CSDDP_DEFS ) << std::endl;
		ofsOutput << "BOOKSDIRECTORY=" << Directory( CSDDP_BOOKS ) << std::endl;
		ofsOutput << "ACTSDIRECTORY=" << Directory( CSDDP_ACCOUNTS ) << std::endl;
		ofsOutput << "SCRIPTSDIRECTORY=" << Directory( CSDDP_SCRIPTS ) << std::endl;
		ofsOutput << "BACKUPDIRECTORY=" << Directory( CSDDP_BACKUP ) << std::endl;
		ofsOutput << "MSGBOARDDIRECTORY=" << Directory( CSDDP_MSGBOARD ) << std::endl;
		ofsOutput << "SHAREDDIRECTORY=" << Directory( CSDDP_SHARED ) << std::endl;
		ofsOutput << "ACCESSDIRECTORY=" << Directory( CSDDP_ACCESS ) << std::endl;
		ofsOutput << "HTMLDIRECTORY=" << Directory( CSDDP_HTML ) << std::endl;
		ofsOutput << "LOGSDIRECTORY=" << Directory( CSDDP_LOGS ) << std::endl;
		ofsOutput << "DICTIONARYDIRECTORY=" << Directory( CSDDP_DICTIONARIES ) << std::endl;
		ofsOutput << "}" << std::endl;
		
		ofsOutput << std::endl << "[settings]" << std::endl << "{" << std::endl;
		ofsOutput << "LOOTDECAYSWITHCORPSE=" << (CorpseLootDecay()?1:0) << std::endl;
		ofsOutput << "GUARDSACTIVE=" << (GuardsStatus()?1:0) << std::endl;
		ofsOutput << "DEATHANIMATION=" << (DeathAnimationStatus()?1:0) << std::endl;
		ofsOutput << "AMBIENTSOUNDS=" << WorldAmbientSounds() << std::endl;
		ofsOutput << "AMBIENTFOOTSTEPS=" << (AmbientFootsteps()?1:0) << std::endl;
		ofsOutput << "INTERNALACCOUNTCREATION=" << (InternalAccountStatus()?1:0) << std::endl;
		ofsOutput << "SHOWHIDDENNPCS=" << (ShowHiddenNpcStatus()?1:0) << std::endl;
		ofsOutput << "ROGUESENABLED=" << (RogueStatus()?1:0) << std::endl;
		ofsOutput << "PLAYERPERSECUTION=" << (PlayerPersecutionStatus()?1:0) << std::endl;
		ofsOutput << "ACCOUNTFLUSH=" << AccountFlushTimer() << std::endl;
		ofsOutput << "HTMLSTATUSENABLED=" << HtmlStatsStatus() << std::endl;
		ofsOutput << "SELLBYNAME=" << (SellByNameStatus()?1:0) << std::endl;
		ofsOutput << "SELLMAXITEMS=" << SellMaxItemsStatus() << std::endl;
		ofsOutput << "TRADESYSTEM=" << (TradeSystemStatus()?1:0) << std::endl;
		ofsOutput << "RANKSYSTEM=" << (RankSystemStatus()?1:0) << std::endl;
		ofsOutput << "CUTSCROLLREQUIREMENTS=" << (CutScrollRequirementStatus()?1:0) << std::endl;
		ofsOutput << "NPCTRAININGENABLED=" << (NPCTrainingStatus()?1:0) << std::endl;
		ofsOutput << "HIDEWILEMOUNTED=" << (CharHideWhileMounted()?1:0) << std::endl;
		ofsOutput << "WEIGHTPERSTR=" << static_cast<UI16>(WeightPerStr()) << std::endl;
		ofsOutput << "POLYDURATION=" << SystemTimer( tSERVER_POLYMORPH ) << std::endl;
		ofsOutput << "CLIENTSUPPORT=" << ServerClientSupport() << std::endl;
		ofsOutput << "}" << std::endl;

		ofsOutput << std::endl << "[speedup]" << std::endl << "{" << std::endl;
		ofsOutput << "SPEEDCHECKITEMS=" << CheckItemsSpeed() << std::endl;
		ofsOutput << "SPEEDCHECKBOATS=" << CheckBoatSpeed() << std::endl;
		ofsOutput << "SPEEDCHECKNPCAI=" << CheckNpcAISpeed() << std::endl;
		ofsOutput << "SPEEDCHECKSPAWNREGIONS=" << CheckSpawnRegionSpeed() << std::endl;
		ofsOutput << "}" << std::endl;
		
		ofsOutput << std::endl << "[message boards]" << std::endl << "{" << std::endl;
		ofsOutput << "MSGBOARDPOSTINGLEVEL=" << static_cast<UI16>(MsgBoardPostingLevel()) << std::endl;
		ofsOutput << "MSGBOARDREMOVALLEVEL=" << static_cast<UI16>(MsgBoardPostRemovalLevel()) << std::endl;
		ofsOutput << "}" << std::endl;

		ofsOutput << std::endl << "[escorts]" << std::endl << "{" << std::endl;
		ofsOutput << "ESCORTENABLED=" << ( EscortsEnabled() ? 1 : 0 ) << std::endl;
		ofsOutput << "ESCORTINITEXPIRE=" << SystemTimer( tSERVER_ESCORTWAIT ) << std::endl;
		ofsOutput << "ESCORTACTIVEEXPIRE=" << SystemTimer( tSERVER_ESCORTACTIVE ) << std::endl;
		ofsOutput << "ESCORTDONEEXPIRE=" << SystemTimer( tSERVER_ESCORTDONE ) << std::endl;
		ofsOutput << "}" << std::endl;
		
		ofsOutput << std::endl << "[worldlight]" << std::endl << "{" << std::endl;
		ofsOutput << "LIGHTMOON1=" << ServerMoon( 0 ) << std::endl;
		ofsOutput << "LIGHTMOON2=" << ServerMoon( 1 ) << std::endl;
		ofsOutput << "LIGHTDUNGEONLEVEL=" << static_cast<UI16>(DungeonLightLevel()) << std::endl;
		ofsOutput << "LIGHTCURRENTLEVEL=" << static_cast<UI16>(WorldLightCurrentLevel()) << std::endl;
		ofsOutput << "LIGHTBRIGHTLEVEL=" << static_cast<UI16>(WorldLightBrightLevel()) << std::endl;
		ofsOutput << "LIGHTDARKLEVEL=" << static_cast<UI16>(WorldLightDarkLevel()) << std::endl;
		ofsOutput << "DAY=" << ServerTimeDay() << std::endl;
		ofsOutput << "HOUR=" << static_cast<UI16>(ServerTimeHours()) << std::endl;
		ofsOutput << "MINUTE=" << static_cast<UI16>(ServerTimeMinutes()) << std::endl;
		ofsOutput << "AMPM=" << ( ServerTimeAMPM() ? 1 : 0 ) << std::endl;
		ofsOutput << "SECONDSPERUOMINUTE=" << ServerSecondsPerUOMinute() << std::endl;
		ofsOutput << "}" << std::endl;
		
		ofsOutput << std::endl << "[tracking]" << std::endl << "{" << std::endl;
		ofsOutput << "TRACKINGBASERANGE=" << TrackingBaseRange() << std::endl;
		ofsOutput << "TRACKINGBASETIMER=" << TrackingBaseTimer() << std::endl;
		ofsOutput << "TRACKINGMAXTARGETS=" << static_cast<UI16>(TrackingMaxTargets()) << std::endl;
		ofsOutput << "TRACKINGMSGREDISPLAYTIME=" << TrackingRedisplayTime() << std::endl;
		ofsOutput << "}" << std::endl;
		
		ofsOutput << std::endl << "[reputation]" << std::endl << "{" << std::endl;
		ofsOutput << "REPSYSMURDERDECAYTIMER=" << SystemTimer( tSERVER_MURDERDECAY ) << std::endl;
		ofsOutput << "REPSYSMAXKILLS=" << RepMaxKills() << std::endl;
		ofsOutput << "REPSYSCRIMINALTIMER=" << SystemTimer( tSERVER_CRIMINAL ) << std::endl;
		ofsOutput << "}" << std::endl;
		
		ofsOutput << std::endl << "[resources]" << std::endl << "{" << std::endl;
		ofsOutput << "RESOURCEMINECHECK=" << static_cast<UI16>(MineCheck()) << std::endl;
		ofsOutput << "RESOURCEOREPERAREA=" << ResOre() << std::endl;
		ofsOutput << "RESOURCEORERESPAWNTIMER=" << ResOreTime() << std::endl;
		ofsOutput << "RESOURCEORERESPAWNAREA=" << ResOreArea() << std::endl;
		ofsOutput << "RESOURCELOGSPERAREA=" << ResLogs() << std::endl;
		ofsOutput << "RESOURCELOGSRESPAWNTIMER=" << ResLogTime() << std::endl;
		ofsOutput << "RESPAWNLOGSRESPAWNAREA=" << ResLogArea() << std::endl;
		ofsOutput << "}" << std::endl;
		
		ofsOutput << std::endl << "[hunger]" << std::endl << "{" << std::endl;
		ofsOutput << "HUNGERRATE=" << SystemTimer( tSERVER_HUNGERRATE ) << std::endl;
		ofsOutput << "HUNGERDMGVAL=" << HungerDamage() << std::endl;
		ofsOutput << "}" << std::endl;
		
		ofsOutput << std::endl << "[combat]" << std::endl << "{" << std::endl;
		ofsOutput << "COMBATMAXRANGE=" << CombatMaxRange() << std::endl;
		ofsOutput << "COMBATSPELLMAXRANGE=" << CombatMaxSpellRange() << std::endl;
		ofsOutput << "COMBATDISPLAYHITMSG=" << (CombatDisplayHitMessage()?1:0) << std::endl;
		ofsOutput << "COMBATMONSTERSVSANIMALS=" << (CombatMonstersVsAnimals()?1:0) << std::endl;
		ofsOutput << "COMBATANIMALATTACKCHANCE=" << CombatAnimalsAttackChance() << std::endl;
		ofsOutput << "COMBATANIMALSGUARDED=" << (CombatAnimalsGuarded()?1:0) << std::endl;
		ofsOutput << "COMBATNPCDAMAGERATE=" << CombatNPCDamageRate() << std::endl;
		ofsOutput << "COMBATNPCBASEFLEEAT=" << CombatNPCBaseFleeAt() << std::endl;
		ofsOutput << "COMBATNPCBASEREATTACKAT=" << CombatNPCBaseReattackAt() << std::endl;
		ofsOutput << "COMBATATTACKSTAMINA=" << CombatAttackStamina() << std::endl;
		ofsOutput << "SHOOTONANIMALBACK=" << (ShootOnAnimalBack()?1:0) << std::endl;
		ofsOutput << "}" << std::endl;
		
		ofsOutput << std::endl << "[start locations]" << std::endl << "{" << std::endl;
		for( size_t lCtr = 0; lCtr < startlocations.size(); ++lCtr )
			ofsOutput << "LOCATION=" << startlocations[lCtr].town << "," << startlocations[lCtr].description << "," << startlocations[lCtr].x << "," << startlocations[lCtr].y << "," << startlocations[lCtr].z << std::endl;
		ofsOutput << "}" << std::endl;
		
		ofsOutput << std::endl << "[startup]" << std::endl << "{" << std::endl;
		ofsOutput << "STARTGOLD=" << ServerStartGold() << std::endl;
		ofsOutput << "STARTPRIVS=" << ServerStartPrivs() << std::endl;
		ofsOutput << "}" << std::endl;
		
		ofsOutput << std::endl << "[gumps]" << std::endl << "{" << std::endl;
		ofsOutput << "TITLECOLOUR=" << TitleColour() << std::endl;
		ofsOutput << "LEFTTEXTCOLOUR=" << LeftTextColour() << std::endl;
		ofsOutput << "RIGHTTEXTCOLOUR=" << RightTextColour() << std::endl;
		ofsOutput << "BUTTONCANCEL=" << ButtonCancel() << std::endl;
		ofsOutput << "BUTTONLEFT=" << ButtonLeft() << std::endl;
		ofsOutput << "BUTTONRIGHT=" << ButtonRight() << std::endl;
		ofsOutput << "BACKGROUNDPIC=" << BackgroundPic() << std::endl;
		ofsOutput << "}" << std::endl;
		
		ofsOutput << std::endl << "[towns]" << std::endl << "{" << std::endl;
		ofsOutput << "POLLTIME=" << TownNumSecsPollOpen() << std::endl;
		ofsOutput << "MAYORTIME=" << TownNumSecsAsMayor() << std::endl;
		ofsOutput << "TAXPERIOD=" << TownTaxPeriod() << std::endl;
		ofsOutput << "GUARDSPAID=" << TownGuardPayment() << std::endl;
		ofsOutput << "}" << std::endl;
		
		ofsOutput.close();
		rvalue = true;
	}
	else
		Console.Error( 1, "Unable to open file %s for writing", filename.c_str() );
	return rvalue;
}

//o------------------------------------------------------------------------
//|	Function		-	CServerData *load( void )
//|	Programmer		-	EviLDeD
//|	Date			-	January 13, 2001
//|	Modified		-
//o------------------------------------------------------------------------
//|	Purpose			-	Load up the uox.ini file and parse it into the internals
//|	Returns			-	pointer to the valid inmemory serverdata storage(this)
//|						NULL is there is an error, or invalid file type
//o------------------------------------------------------------------------
CServerData * CServerData::load( void )
{
	std::string s = Directory( CSDDP_ROOT );
	s += "uox.ini";
	return load( s );
}

//o------------------------------------------------------------------------
//|	Function		-	CServerData *load()
//|	Programmer		-	EviLDeD
//|	Date			-	January 13, 2001
//|	Modified		-
//o------------------------------------------------------------------------
//|	Purpose			-	Load up the specified file and parse it into the internals
//|						providing that it is in the correct format.
//|	Returns			-	pointer to the valid inmemory serverdata storage(this) or
//|						NULL is there is an error, or invalid file type
//o------------------------------------------------------------------------
CServerData * CServerData::load( std::string filename )
{
	ParseUox3Ini( filename );
	return this;
}

void CServerData::TrackingBaseRange( UI16 value )
{
	trackingbaserange = value;
}
UI16 CServerData::TrackingBaseRange( void ) const
{
	return trackingbaserange;
}

void CServerData::TrackingMaxTargets( UI08 value )
{
	if( value >= MAX_TRACKINGTARGETS )
		trackingmaxtargets = MAX_TRACKINGTARGETS;
	else
		trackingmaxtargets = value;
}
UI08 CServerData::TrackingMaxTargets( void ) const
{
	return trackingmaxtargets;
}

void CServerData::TrackingBaseTimer( UI16 value )
{
	trackingbasetimer = value;
}
UI16 CServerData::TrackingBaseTimer( void ) const
{
	return trackingbasetimer;
}

void CServerData::TrackingRedisplayTime( UI16 value )
{
	trackingmsgredisplaytimer = value;
}
UI16 CServerData::TrackingRedisplayTime( void ) const
{
	return trackingmsgredisplaytimer;
}

//o-----------------------------------------------------------------------o
//|	Function	-	void DumpLookup( int lookupid );
//|	Date		-	January 31, 2001
//|	Programmer	-	EviLDeD
//o-----------------------------------------------------------------------o
//|	Purpose		-	This simply outputs a text file that contains the index
//|					and lookup name found in the lookuptables. Saves having
//|					to do it by hand if you make changes. Must be called from
//|					within the class if used.
//o-----------------------------------------------------------------------o
void CServerData::dumpLookup( int lookupid )
{
	// lookup is no longer needed, as we don't support server.scp any more

	std::ofstream ofsOutput;
	char buffer[81];
	int count = 0;

	ofsOutput.open( "uox.ini.lookup.txt", std::ios::out );
	if( !ofsOutput.is_open() )
	{
		Console << "-o [DLOOKUP]: Unable to open file \"uox3.ini.lookup.txt\" for writing.." << myendl;
		return;
	}

	UString tokens( UOX3INI_LOOKUP );
	int numTokens = tokens.sectionCount( "|" );
	for( int i = 0; i <= numTokens; i++ )
	{
		UString tokenStr = tokens.section( "|", i, i );
		sprintf( buffer, "case 0x%04X:\t // %s[%04i]\n\tCServerData:\n\tbreak;\n", UOX3INI_LOOKUP.find( tokenStr.c_str(), tokenStr.length() ), tokenStr.c_str(), count++ );
		ofsOutput << buffer;
	}
	ofsOutput.close();
}


//o--------------------------------------------------------------------------o
//|	Function/Class	-	CServerData * CServerData::ParseUox3Ini( std::string filename )
//|	Date			-	02/26/2001
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	Parse teh uox.ini file into its required information.
//|									
//|	Modification	-	02/26/2002	-	Make sure that we parse out the logs, access
//|									and other directories that we were not parsing/
//o--------------------------------------------------------------------------o
//|	Returns			-
//o--------------------------------------------------------------------------o
CServerData * CServerData::ParseUox3Ini( std::string filename )
{
	CServerData *rvalue = NULL;
	if( !filename.empty() )
	{
		std::string sFilename = filename;
		std::ifstream inFile;

		Console << "Processing INI Settings  ";

		// Lock this file tight, No access at anytime when open(should only be open and closed anyhow. For Thread blocking)
		inFile.open( sFilename.c_str(), std::ios::in );	
		if( inFile.is_open() )
		{
			char szLine[129];	// Trimmed this down to 80 to save some ram realestate
			inFile.getline( szLine, 128 );	// Snatch the first line.
			UString tag, value;
			UString sname, sip, sport;
			serverList.clear();
			startlocations.clear();
			struct hostent *lpHostEntry=NULL;
			while( !inFile.eof() && !inFile.fail() )
			{
				UString LineBuffer( szLine );
				LineBuffer = LineBuffer.removeComment().stripWhiteSpace();
				if( LineBuffer.sectionCount( "=" ) == 1 )
				{
					tag		= LineBuffer.section( "=", 0, 0 ).stripWhiteSpace().upper();
					value	= LineBuffer.section( "=", 1 ).stripWhiteSpace();

					switch( UOX3INI_LOOKUP.find( tag.c_str() ) )
					{
						case 0x0001:	// 04/03/2004 - EviLDeD - Seems that the new code can see the first case now. not completly tested, and its not going to kill us to allow the fall through
						case 0x000C:	 // SERVERNAME[0002]
							//ServerName( value );
							break;
						case 0x0017:	 // CONSOLELOG[0003]
							ServerConsoleLog( value.toUByte() );
							break;
						case 0x0022:	 // CRASHPROTECTION[0004]
							ServerCrashProtection( value.toUByte() );
							break;
						case 0x0032:	 // COMMANDPREFIX[0005]
							ServerCommandPrefix( (value.data()[0]) );	// return the first character of the return string only
							break;
						case 0x0040:	 // ANNOUNCEWORLDSAVES[0006]
							ServerAnnounceSaves( (value.toUShort()==1?true:false) ); 
							break;
						case 0x0053:	 // JOINPARTMSGS[0007]
							ServerJoinPartAnnouncements( (value.toUShort()==1?true:false) );
							break;
						case 0x0060:	 // MULCACHING[0008]
							ServerMulCaching( (value.toUShort()==1?true:false) );
							break;
						case 0x006B:	 // BACKUPSENABLED[0009]
							ServerBackups( (value.toUShort()>0?true:false) );
							break;
						case 0x007A:	 // SAVESTIMER[0010]
							ServerSavesTimer( value.toULong() );
							break;
						case 0x0085:	 // SKILLCAP[0011]
							ServerSkillCap( value.toUShort() );
							break;
						case 0x008E:	 // SKILLDELAY[0012]
							ServerSkillDelay( value.toUByte() );
							break;
						case 0x0099:	 // STATCAP[0013]
							ServerStatCap( value.toUShort() );
							break;
						case 0x00A1:	 // MAXSTEALTHMOVEMENTS[0014]
							MaxStealthMovement( value.toShort() );
							break;
						case 0x00B5:	 // MAXSTAMINAMOVEMENTS[0015]
							MaxStaminaMovement( value.toShort() );
							break;
						case 0x00C9:	 // ARMORAFFECTMANAREGEN[0016]
							ArmorAffectManaRegen( (value.toUByte() > 0 ? true : false) );
							break;
						case 0x00DE:	 // CORPSEDECAYTIMER[0017]
							SystemTimer( tSERVER_CORPSEDECAY, value.toUShort() );
							break;
						case 0x00EF:	 // WEATHERTIMER[0018]
							SystemTimer( tSERVER_WEATHER, value.toUShort() );
							break;
						case 0x00FC:	 // SHOPSPAWNTIMER[0019]
							SystemTimer( tSERVER_SHOPSPAWN, value.toUShort() );
							break;
						case 0x00E4:	 // DECAYTIMER[0020]
							SystemTimer( tSERVER_DECAY, value.toUShort() );
							break;
						case 0x0116:	 // INVISIBILITYTIMER[0021]
							SystemTimer( tSERVER_INVISIBILITY, value.toUShort() );
							break;
						case 0x0128:	 // OBJECTUSETIMER[0022]
							SystemTimer( tSERVER_OBJECTUSAGE, value.toUShort() );
							break;
						case 0x0137:	 // GATETIMER[0023]
							SystemTimer( tSERVER_GATE, value.toUShort() );
							break;
						case 0x0141:	 // POISONTIMER[0024]
							SystemTimer( tSERVER_POISON, value.toUShort() );
							break;
						case 0x014D:	 // LOGINTIMEOUT[0025]
							SystemTimer( tSERVER_LOGINTIMEOUT, value.toUShort() );
							break;
						case 0x015A:	 // HITPOINTREGENTIMER[0026]
							SystemTimer( tSERVER_HITPOINTREGEN, value.toUShort() );
							break;
						case 0x016D:	 // STAMINAREGENTIMER[0027]
							SystemTimer( tSERVER_STAMINAREGEN, value.toUShort() );
							break;
						case 0x017F:	 // MANAREGENTIMER[0028]
							SystemTimer( tSERVER_MANAREGEN, value.toUShort() );
							break;
						case 0x018E:	 // BASEFISHINGTIMER[0029]
							SystemTimer( tSERVER_FISHINGBASE, value.toUShort() );
							break;
						case 0x019F:	 // RANDOMFISHINGTIMER[0030]
							SystemTimer( tSERVER_FISHINGRANDOM, value.toUShort() );
							break;
						case 0x01B2:	 // SPIRITSPEAKTIMER[0031]
							SystemTimer( tSERVER_SPIRITSPEAK, value.toUShort() );
							break;
						case 0x01C3:	 // DIRECTORY[0032]
							Directory( CSDDP_ROOT, value );
							break;
						case 0x01CD:	 // DATADIRECTORY[0033]
							Directory( CSDDP_DATA, value );
							break;
						case 0x01DB:	 // DEFSDIRECTORY[0034]
							Directory( CSDDP_DEFS, value );
							break;
						case 0x01E9:	 // ACTSDIRECTORY[0035]
							Directory( CSDDP_ACCOUNTS, value );
							break;
						case 0x01F7:	 // SCRIPTSDIRECTORY[0036]
							Directory( CSDDP_SCRIPTS, value );
							break;
						case 0x0208:	 // BACKUPDIRECTORY[0037]
							Directory( CSDDP_BACKUP, value );
							break;
						case 0x0218:	 // MSGBOARDDIRECTORY[0038]
							Directory( CSDDP_MSGBOARD, value );
							break;
						case 0x022A:	 // SHAREDDIRECTORY[0039]
							Directory( CSDDP_SHARED, value );
							break;
						case 0x023A:	 // LOOTDECAYSWITHCORPSE[0040]
							CorpseLootDecay( value.toUShort() != 0 );
							break;
						case 0x024F:	 // GUARDSACTIVE[0041]
							GuardStatus( value.toUShort() != 0 );
							break;
						case 0x025C:	 // DEATHANIMATION[0042]
							DeathAnimationStatus( value.toUShort() != 0 );
							break;
						case 0x026B:	 // AMBIENTSOUNDS[0043]
							WorldAmbientSounds( value.toShort() );
							break;
						case 0x0279:	 // AMBIENTFOOTSTEPS[0044]
							AmbientFootsteps( value.toUShort() != 0 );
							break;
						case 0x028A:	 // INTERNALACCOUNTCREATION[0045]
							InternalAccountStatus( value.toUShort() != 0 );
							break;
						case 0x02A2:	 // SHOWHIDDENNPCS[0046]
							ShowHiddenNpcStatus( value.toUShort() != 0 );
							break;
						case 0x02B1:	 // ROGUESENABLED[0047]
							RogueStatus( value.toUShort() != 0 );
							break;
						case 0x02BF:	 // PLAYERPERSECUTION[0048]
							PlayerPersecutionStatus( value.toUShort() != 0 );
							break;
						case 0x02D1:	 // ACCOUNTFLUSH[0049]
							AccountFlushTimer( value.toDouble() );
							break;
						case 0x02DE:	 // HTMLSTATUSENABLED[0050]
							HtmlStatsStatus( value.toShort() );
							break;
						case 0x02F0:	 // SELLBYNAME[0051]
							SellByNameStatus( value.toUShort() == 1 );
							break;
						case 0x02FB:	 // SELLMAXITEMS[0052]
							SellMaxItemsStatus( value.toShort() );
							break;
						case 0x0308:	 // TRADESYSTEM[0053]
							TradeSystemStatus( value.toUShort() != 0 );
							break;
						case 0x0314:	 // RANKSYSTEM[0054]
							RankSystemStatus( value.toUShort() != 0 );
							break;
						case 0x031F:	 // CUTSCROLLREQUIREMENTS[0055]
							CutScrollRequirementStatus( value.toShort() );
							break;
						case 0x0335:	 // SPEEDCHECKITEMS[0056]
							CheckItemsSpeed( value.toDouble() );
							break;
						case 0x0345:	 // SPEEDCHECKBOATS[0057]
							CheckBoatSpeed( value.toDouble() );
							break;
						case 0x0355:	 // SPEEDCHECKNPCAI[0058]
							CheckNpcAISpeed( value.toDouble() );
							break;
						case 0x0365:	 // SPEEDCHECKSPAWNREGIONS[0059]
							CheckSpawnRegionSpeed( value.toDouble() );
							break;
						case 0x037C:	 // MSGBOARDPOSTINGLEVEL[0060]
							MsgBoardPostingLevel( value.toUByte() );
							break;
						case 0x0391:	 // MSGBOARDREMOVALLEVEL[0061]
							MsgBoardPostRemovalLevel( value.toUByte() );
							break;
						case 0x03A6:	 // ESCORTENABLED[0062]
							EscortsEnabled( value.toUShort() == 1 );
							break;
						case 0x03B4:	 // ESCORTINITEXPIRE[0063]
							SystemTimer( tSERVER_ESCORTWAIT, value.toUShort() );
							break;
						case 0x03C5:	 // ESCORTACTIVEEXPIRE[0064]
							SystemTimer( tSERVER_ESCORTACTIVE, value.toUShort() );
							break;
						case 0x03D8:	 // LIGHTMOON1[0065]
							ServerMoon( 0, value.toShort() );
							break;
						case 0x03E3:	 // LIGHTMOON2[0066]
							ServerMoon( 1, value.toShort() );
							break;
						case 0x03EE:	 // LIGHTDUNGEONLEVEL[0067]
							DungeonLightLevel( (LIGHTLEVEL)value.toUShort() );
							break;
						case 0x0400:	 // LIGHTCURRENTLEVEL[0068]
							WorldLightCurrentLevel( (LIGHTLEVEL)value.toUShort() );
							break;
						case 0x0412:	 // LIGHTBRIGHTLEVEL[0069]
							WorldLightBrightLevel( (LIGHTLEVEL)value.toUShort() );
							break;
						case 0x0423:	 // TRACKINGBASERANGE[0070]
							TrackingBaseRange( value.toUShort() );
							break;
						case 0x0435:	 // TRACKINGBASETIMER[0071]
							TrackingBaseTimer( value.toUShort() );
							break;
						case 0x0447:	 // TRACKINGMAXTARGETS[0072]
							TrackingMaxTargets( value.toUByte() );
							break;
						case 0x045A:	 // TRACKINGMSGREDISPLAYTIME[0073]
							TrackingRedisplayTime( value.toUShort() );
							break;
						case 0x0473:	 // REPSYSMURDERDECAYTIMER[0074]
							SystemTimer( tSERVER_MURDERDECAY, value.toUShort() );
							break;
						case 0x048A:	 // REPSYSMAXKILLS[0075]
							RepMaxKills( value.toUShort() );
							break;
						case 0x0499:	 // REPSYSCRIMINALTIMER[0076]
							SystemTimer( tSERVER_CRIMINAL, value.toUShort() );
							break;
						case 0x04AD:	 // RESOURCEMINECHECK[0077]
							MineCheck( value.toUByte() );
							break;
						case 0x04BF:	 // RESOURCEOREPERAREA[0078]
							ResOre( value.toShort() );
							break;
						case 0x04D2:	 // RESOURCEORERESPAWNTIMER[0079]
							ResOreTime( value.toUShort() );
							break;
						case 0x04EA:	 // RESOURCEORERESPAWNAREA[0080]
							ResOreArea( value.toUShort() );
							break;
						case 0x0501:	 // RESOURCELOGSPERAREA[0081]
							ResLogs( value.toShort() );
							break;
						case 0x0515:	 // RESPURCELOGSRESPAWNTIMER[0082]
							ResLogTime( value.toUShort() );
							break;
						case 0x052E:	 // RESOURCELOGSRESPAWNAREA[0083]
							ResLogArea( value.toUShort() );
							break;
						case 0x0546:	 // HUNGERRATE[0084]
							SystemTimer( tSERVER_HUNGERRATE, value.toUShort() );
							break;
						case 0x0551:	 // HUNGERDMGVAL[0085]
							HungerDamage( value.toShort() );
							break;
						case 0x055E:	 // COMBATMAXRANGE[0086]
							CombatMaxRange( value.toShort() );
							break;
						case 0x056D:	 // COMBATSPELLMAXRANGE[0087]
							CombatMaxSpellRange( value.toShort() );
							break;
						case 0x0581:	 // COMBATDISPLAYHITMSG[0088]
							CombatDisplayHitMessage( value.toUShort() == 1 );
							break;
						case 0x0595:	 // COMBATMONSTERSVSANIMALS[0089]
							CombatMonstersVsAnimals( value.toUShort() == 1 );
							break;
						case 0x05AD:	 // COMBATANIMALATTACKCHANCE[0090]
							CombatAnimalsAttackChance( value.toShort() );
							break;
						case 0x05C6:	 // COMBATANIMALSGUARDED[0091]
							CombatAnimalsGuarded( value.toUShort() == 1 );
							break;
						case 0x05DB:	 // COMBATNPCDAMAGERATE[0092]
							CombatNPCDamageRate( value.toShort() );
							break;
						case 0x05EF:	 // COMBATNPCBASEFLEEAT[0093]
							CombatNPCBaseFleeAt( value.toShort() );
							break;
						case 0x0603:	 // COMBATNPCBASEREATTACKAT[0094]
							CombatNPCBaseReattackAt( value.toShort() );
							break;
						case 0x061B:	 // COMBATATTACKSTAMINA[0095]
							CombatAttackStamina( value.toShort() );
							break;
						case 0x062F:	 // LOCATION[0096]
							ServerLocation( value );
							break;
						case 0x0638:	 // STARTGOLD[0097]
							ServerStartGold( value.toShort() );
							break;
						case 0x0642:	 // STARTPRIVS[0098]
							ServerStartPrivs( value.toUShort() );
							break;
						case 0x064D:	 // ESCORTDONEEXPIRE[0099]
							SystemTimer( tSERVER_ESCORTDONE, value.toUShort() );
							break;
						case 0x065E:	 // LIGHTDARKLEVEL[0100]
							WorldLightDarkLevel( (LIGHTLEVEL)value.toUShort() );
							break;
						case 0x066D:	 // TITLECOLOUR[0101]
							TitleColour( value.toUShort() );
							break;
						case 0x0679:	 // LEFTTEXTCOLOUR[0102]
							LeftTextColour( value.toUShort() );
							break;
						case 0x0688:	 // RIGHTTEXTCOLOUR[0103]
							RightTextColour( value.toUShort() );
							break;
						case 0x0698:	 // BUTTONCANCEL[0104]
							ButtonCancel( value.toUShort() );
							break;
						case 0x06A5:	 // BUTTONLEFT[0105]
							ButtonLeft( value.toUShort() );
							break;
						case 0x06B0:	 // BUTTONRIGHT[0106]
							ButtonRight( value.toUShort() );
							break;
						case 0x06BC:	 // BACKGROUNDPIC[0107]
							BackgroundPic( value.toUShort() );
							break;
						case 0x06CA:	 // POLLTIME[0108]
							TownNumSecsPollOpen( value.toULong() );
							break;
						case 0x06D3:	 // MAYORTIME[0109]
							TownNumSecsAsMayor( value.toULong() );
							break;
						case 0x06DD:	 // TAXPERIOD[0110]
							TownTaxPeriod( value.toULong() );
							break;
						case 0x06E7:	 // GUARDSPAID[0111]
							TownGuardPayment( value.toULong() );
							break;
						case 0x06F2:	 // DAY[0112]
							ServerTimeDay( value.toShort() );
							break;
						case 0x06F6:	 // HOURS[0113]
							ServerTimeHours( value.toUByte() );
							break;
						case 0x06FC:	 // MINUTES[0114]
							ServerTimeMinutes( value.toUByte() );
							break;
						case 0x0704:	 // SECONDS[0115]
							ServerTimeSeconds( value.toUByte() );
							break;
						case 0x070C:	 // AMPM[0116]
							ServerTimeAMPM( value.toUShort() != 0 );
							break;
						case 0x0711:	 // SKILLLEVEL[0117]
							SkillLevel( value.toUByte() );
							break;
						case 0x071C:	 // SNOOPISCRIME[0118]
							SnoopIsCrime( value.toUShort() != 0 );
							break;
						case 0x0729:	 // BOOKSDIRECTORY[0119]
							Directory( CSDDP_BOOKS, value );
							break;
						case 0x0738:	 // SERVERLIST[0120]
						{
							physicalServer toAdd;
							sname	= value.section( ",", 0, 0 ).stripWhiteSpace();
							sip		= value.section( ",", 1, 1 ).stripWhiteSpace();
							sport	= value.section( ",", 2, 2 ).stripWhiteSpace();

							toAdd.setName( sname );
							if( value.sectionCount( "," ) == 2 )
							{
								// Ok look up the data here see if its a number
								bool bDomain = true;
								if( ( lpHostEntry = gethostbyname( sip.c_str() ) ) == NULL )
								{
									// this was not a domain name so check for IP address
									if( ( lpHostEntry = gethostbyaddr( sip.c_str(), sip.size(), AF_INET ) ) == NULL )
									{
										// We get here it wasn't a valid IP either.
										Console.Warning( 1, "Failed to translate %s", sip.c_str() );
										Console.Warning( 1, "This shard will not show up on the shard listing" );
										break;
									}
									bDomain=false;
								}
								// Going to store a copy of the domain name as well to save to the ini if there is a domain name insteead of an ip.
								if( bDomain )
								{
									// Store the domain name for later then seeing as its a valid one
									toAdd.setDomain( sip );
								}
								else
								{
									// this was a valid ip address so we will use an ip instead so clear the domain string.
									toAdd.setDomain( "" );
								}
							}
							// Ok now the server itself uses the ip so we need to store that :) Means we only need to look thisip once 
							struct in_addr *pinaddr;
							pinaddr = ((struct in_addr*)lpHostEntry->h_addr);
							toAdd.setIP( inet_ntoa(*pinaddr) );
							toAdd.setPort( sport.toUShort() );
							serverList.push_back( toAdd );
							break;
						}
						case 0x07A0:	 // PORT[0154] // whatever that stands for..
							ServerPort( value.toUShort() );
							break;
						case 0x0748:	 // ACCESSDIRECTORY[0122]
							Directory( CSDDP_ACCESS, value );
							break;
						case 0x0758:	 // LOGSDIRECTORY[0123]
							Directory( CSDDP_LOGS, value );
							break;
						case 0x0766:	 // ACCOUNTISOLATION[0124]
							break;
						case 0x0777:	 // HTMLDIRECTORY[0125]
							Directory( CSDDP_HTML, value );
							break;
						case 0x0785:	 // SHOOTONANIMALBACK[0126]
							ShootOnAnimalBack( value.toUShort() == 1 );
							break;
						case 0x0797:	 // NPCTRAININGENABLED[0127]
							NPCTrainingStatus( value.toUShort() == 1 );
							break;
						case 0x07AA:	 // DICTIONARYDIRECTORY[0128]
							Directory( CSDDP_DICTIONARIES, value );
							break;
						case 0x07BE:	 // BACKUPSAVERATIO[0129]
							BackupRatio( value.toShort() );
							break;
						case 0x07CE:	 // HIDEWILEMOUNTED[0130]
							CharHideWhileMounted( value.toShort() == 1 );
							break;
						case 0x07DE:	 // SECONDSPERUOMINUTE[0131]
							ServerSecondsPerUOMinute( value.toUShort() );
							break;
						case 0x07F1:	 // WEIGHTPERSTR[0132]
							WeightPerStr( value.toUByte() );
							break;
						case 0x07FE:	 // POLYDURATION[0133]
							SystemTimer( tSERVER_POLYMORPH, value.toUShort() );
							break;
						case 0x080B:	 // UOGENABLED[0134]
							ServerUOGEnabled( value.toShort()==1 );
							break;
						case 0x0816:	 // NETRCVTIMEOUT[0135]
							ServerNetRcvTimeout( value.toULong() );
							break;
						case 0x0824:	 // NETSNDTIMEOUT[0136]
							ServerNetSndTimeout( value.toULong() );
							break;
						case 0x0832:	 // NETRETRYCOUNT[0137]
							ServerNetRetryCount( value.toULong() );
							break;
						case 0x0840:	 // USEFACETSAVES[0138]
							ServerUseFacetSaves( value.toUShort() == 1);
							break;
						case 0x084E:	 // MAP0[0139]
							// Place holder
							break;
						case 0x0853:	 // MAP1[0140]
							// Place holder
							break;
						case 0x0858:	 // MAP2[0141]
							// Place holder
							break;
						case 0x085D:	 // MAP3[0142]
							// Place holder
							break;
						case 0x0862:	 // USERMAP[0143]
							// Place holder
							break;
						case 0x086A:	 // CLIENTSUPPORT[0144]
							ServerClientSupport( value.toULong() );
							break;
						case 0x0878:	 // FTPDENABLED[0145]
							break;
						case 0x0884:	 // FTPDUSER[0146]
							break;
						case 0x088D:	 // FTPDUSERLIMIT[0147]
							break;
						case 0x089B:	 // FTPDBIND[0148]
							break;
						case 0x08A4:	 // FTPDROOT[0149]
							break;
						case 0x08AD:	 // FTPCENABLED[0150]
							break;
						case 0x08B9:	 // FTPCHOST[0151]
							break;
						case 0x08C2:	 // FTPCPORT[0152]
							break;
						case 0x08CB:	 // FTPCUSER[0153]
							break;
						case 0x08D4:	 // FTPCULFLAGS[0154]
							break;
						default:
							//Console << "Unknown tag \"" << l << "\" in " << filename << myendl;					break;
							break;
					}
					inFile.getline( szLine, 127 );
				}
				else
				{
					inFile.getline( szLine, 127 );
				}
			}
			Console.PrintDone();
			rvalue = this;
		}
		else
			Console << "FAILED!" << myendl << "Error: Unable to open file." << sFilename << myendl;
	}
	return rvalue;
}

void CServerData::ServerStartGold( SI16 value )
{
	if( value >= 0 )
		startgold = value;
}


SI16 CServerData::ServerStartGold( void ) const
{
	return startgold;
}

void CServerData::ServerStartPrivs( UI16 value )
{
	startprivs = value;
}

UI16 CServerData::ServerStartPrivs( void ) const
{
	return startprivs;
}

void CServerData::ServerMoon( SI16 slot, SI16 value )
{
	if( slot >= 0 && slot <= 1 && value >= 0 )
		moon[slot] = value;
}

SI16 CServerData::ServerMoon( SI16 slot ) const
{
	SI16 rvalue = -1;
	if( slot >= 0 && slot <= 1 )
		rvalue = moon[slot];
	return rvalue;
}

void CServerData::DungeonLightLevel( LIGHTLEVEL value )
{
	dungeonlightlevel = value;
}

LIGHTLEVEL CServerData::DungeonLightLevel( void ) const
{
	return dungeonlightlevel;
}

void CServerData::WorldLightCurrentLevel( LIGHTLEVEL value )
{
	currentlightlevel = value;
}

LIGHTLEVEL CServerData::WorldLightCurrentLevel( void ) const
{
	return currentlightlevel;
}

void CServerData::WorldLightBrightLevel( LIGHTLEVEL value )
{
	brightnesslightlevel = value;
}

LIGHTLEVEL CServerData::WorldLightBrightLevel( void ) const
{
	return brightnesslightlevel;
}

void CServerData::WorldLightDarkLevel( LIGHTLEVEL value )
{
	darknesslightlevel=value;
}

LIGHTLEVEL CServerData::WorldLightDarkLevel( void ) const
{
	return darknesslightlevel;
}

void CServerData::PostLoadDefaults( void )
{
	if( startlocations.empty() )
	{
		ServerLocation( "Yew,Center,545,982,0" );
		ServerLocation( "Minoc,Tavern,2477,411,15" );
		ServerLocation( "Britain,Sweet Dreams Inn,1495,1629,10" );
		ServerLocation( "Moonglow,Docks,4406,1045,0" );
		ServerLocation( "Trinsic,West Gate,1832,2779,0" );
		ServerLocation( "Magincia,Docks,3675,2259,20" );
		ServerLocation( "Jhelom,Docks,1492,3696,0" );
		ServerLocation( "Skara Brae,Docks,639,2236,0" );
		ServerLocation( "Vesper,Ironwood Inn,2771,977,0" );
	}
}

void CServerData::ServerLocation( std::string toSet )
{
	UString temp( toSet );
	if( temp.sectionCount( "," ) == 4 )	// Wellformed server location
	{
		STARTLOCATION toAdd;
		toAdd.x = temp.section( ",", 2, 2 ).toShort();
		toAdd.y = temp.section( ",", 3, 3 ).toShort();
		toAdd.z = temp.section( ",", 4, 4 ).toShort();
		strcpy( toAdd.town, temp.section( ",", 0, 0 ).c_str() );
		strcpy( toAdd.description, temp.section( ",", 1, 1 ).c_str() );
		startlocations.push_back( toAdd );
	}
	else
	{
		Console.Error( 2, "Malformed location entry in ini file" );
	}
}
LPSTARTLOCATION CServerData::ServerLocation( UI32 locNum )
{
	LPSTARTLOCATION rvalue = NULL;
	if( locNum < startlocations.size() )
		rvalue = &startlocations[locNum];
	return rvalue;
}

size_t CServerData::NumServerLocations( void ) const
{
	return startlocations.size();
}

UI16 CServerData::ServerSecondsPerUOMinute( void ) const
{
	return secondsperuominute;
}
void CServerData::ServerSecondsPerUOMinute( UI16 newVal )
{
	secondsperuominute = newVal;
}

SI16 CServerData::ServerTimeDay( void ) const
{
	return days;
}
void CServerData::ServerTimeDay( SI16 nValue )
{
	days = nValue;
}
UI08 CServerData::ServerTimeHours( void ) const
{
	return hours;
}
void CServerData::ServerTimeHours( UI08 nValue )
{
	hours = nValue;
}
UI08 CServerData::ServerTimeMinutes( void ) const
{
	return minutes;
}

void CServerData::ServerTimeMinutes( UI08 nValue )
{
	minutes = nValue;
}
UI08 CServerData::ServerTimeSeconds( void ) const
{
	return seconds;
}
void CServerData::ServerTimeSeconds( UI08 nValue )
{
	seconds = nValue;
}
bool CServerData::ServerTimeAMPM( void ) const
{
	return ampm;
}
void CServerData::ServerTimeAMPM( bool nValue )
{
	ampm = nValue;
}

bool CServerData::incSecond( void )
{
	bool rvalue = false;
	++seconds;
	if( seconds == 60 )
	{
		seconds = 0;
		rvalue	= incMinute();
	}
	return rvalue;
}

void CServerData::incMoon( int mNumber )
{
	moon[mNumber] = (SI16)((moon[mNumber] + 1)%8);
}

bool CServerData::incMinute( void )
{
	bool rvalue = false;
	++minutes;
	if( minutes%8 == 0 )
		incMoon( 0 );
	if( minutes%3 == 0 )
		incMoon( 1 );

	if( minutes == 60 )
	{
		minutes = 0;
		rvalue	= incHour();
	}
	return rvalue;
}

bool CServerData::incHour( void )
{
	++hours;
	bool retVal = false;
	if( hours == 12 )
	{
		if( ampm )
			retVal = incDay();
		hours	= 0;
		ampm	= !ampm;
	}
	return retVal;
}

bool CServerData::incDay( void )
{
	++days;
	return true;
}

physicalServer *CServerData::ServerEntry( UI16 entryNum )
{
	physicalServer *rvalue = NULL;
	if( entryNum < serverList.size() )
		rvalue = &serverList[entryNum];
	return rvalue;
}
UI16 CServerData::ServerCount( void ) const
{
	return static_cast<UI16>(serverList.size());
}

void physicalServer::setName(const std::string newName)
{
  name = newName;
}
void physicalServer::setDomain(const std::string newDomain)
{
  domain = newDomain;
}
void physicalServer::setIP(const std::string newIP)
{
  ip = newIP;
}
void physicalServer::setPort(UI16 newPort)
{
  port = newPort;
}
std::string physicalServer::getName( void ) const
{
  return name;
}
std::string physicalServer::getDomain( void ) const
{
  return domain;
}
std::string physicalServer::getIP( void ) const
{
  return ip; 
}
UI16 physicalServer::getPort( void ) const
{
  return port;
}

}
