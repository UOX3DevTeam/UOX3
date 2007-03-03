#include "uox3.h"
#include "scriptc.h"
#include "ssection.h"

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

const UI32 BIT_ANNOUNCESAVES		= 0;
const UI32 BIT_ANNOUNCEJOINPART		= 1;
const UI32 BIT_SERVERBACKUP			= 2;
const UI32 BIT_SHOOTONANIMALBACK	= 3;
const UI32 BIT_NPCTRAINING			= 4;
const UI32 BIT_LOOTDECAYSONCORPSE	= 5;
const UI32 BIT_GUARDSENABLED		= 6;
const UI32 BIT_PLAYDEATHANIMATION	= 7;
const UI32 BIT_AMBIENTFOOTSTEPS		= 8;
const UI32 BIT_INTERNALACCOUNTS		= 9;
const UI32 BIT_SHOWOFFLINEPCS		= 10;
const UI32 BIT_ROGUESTATUS			= 11;
const UI32 BIT_SNOOPISCRIME			= 12;
const UI32 BIT_PERSECUTIONSTATUS	= 13;
const UI32 BIT_SELLBYNAME			= 14;
const UI32 BIT_TRADESYSSTATUS		= 15;
const UI32 BIT_RANKSYSSTATUS		= 16;
const UI32 BIT_CUTSCROLLREQ			= 17;
const UI32 BIT_SHOWHITMESSAGE		= 18;
const UI32 BIT_ESCORTSTATUS			= 19;
const UI32 BIT_MONSTERSVSANIMALS	= 20;
const UI32 BIT_PETHUNGEROFFLINE		= 21;
const UI32 BIT_HIDEWHILEMOUNTED		= 22;
const UI32 BIT_OVERLOADPACKETS		= 23;
const UI32 BIT_ARMORAFFECTMANAREGEN = 24;
const UI32 BIT_ANIMALSGUARDED		= 25;
const UI32 BIT_ADVANCEDPATHFIND		= 26;

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
	"AMBIENTSOUNDS|AMBIENTFOOTSTEPS|INTERNALACCOUNTCREATION|SHOWOFFLINEPCS|ROGUESENABLED|PLAYERPERSECUTION|ACCOUNTFLUSH|HTMLSTATUSENABLED|"
	"SELLBYNAME|SELLMAXITEMS|TRADESYSTEM|RANKSYSTEM|CUTSCROLLREQUIREMENTS|CHECKITEMS|CHECKBOATS|CHECKNPCAI|"
	"CHECKSPAWNREGIONS|POSTINGLEVEL|REMOVALLEVEL|ESCORTENABLED|ESCORTINITEXPIRE|ESCORTACTIVEEXPIRE|MOON1|MOON2|"
	"DUNGEONLEVEL|CURRENTLEVEL|BRIGHTLEVEL|BASERANGE|BASETIMER|MAXTARGETS|MSGREDISPLAYTIME|"
	"MURDERDECAYTIMER|MAXKILLS|CRIMINALTIMER|MINECHECK|OREPERAREA|ORERESPAWNTIMER|ORERESPAWNAREA|LOGSPERAREA|LOGSRESPAWNTIMER|LOGSRESPAWNAREA|HUNGERRATE|HUNGERDMGVAL|"
	"MAXRANGE|SPELLMAXRANGE|DISPLAYHITMSG|MONSTERSVSANIMALS|"
	"ANIMALATTACKCHANCE|ANIMALSGUARDED|NPCDAMAGERATE|NPCBASEFLEEAT|NPCBASEREATTACKAT|ATTACKSTAMINA|LOCATION|STARTGOLD|STARTPRIVS|ESCORTDONEEXPIRE|LIGHTDARKLEVEL|"
	"TITLECOLOUR|LEFTTEXTCOLOUR|RIGHTTEXTCOLOUR|BUTTONCANCEL|BUTTONLEFT|BUTTONRIGHT|BACKGROUNDPIC|POLLTIME|MAYORTIME|TAXPERIOD|GUARDSPAID|DAY|HOURS|MINUTES|SECONDS|AMPM|SKILLLEVEL|SNOOPISCRIME|BOOKSDIRECTORY|SERVERLIST|PORT|"
	"ACCESSDIRECTORY|LOGSDIRECTORY|ACCOUNTISOLATION|HTMLDIRECTORY|SHOOTONANIMALBACK|NPCTRAININGENABLED|DICTIONARYDIRECTORY|BACKUPSAVERATIO|HIDEWILEMOUNTED|SECONDSPERUOMINUTE|WEIGHTPERSTR|POLYDURATION|"
	"UOGENABLED|NETRCVTIMEOUT|NETSNDTIMEOUT|NETRETRYCOUNT|CLIENTFEATURES|PACKETOVERLOADS|NPCMOVEMENTSPEED|PETHUNGEROFFLINE|PETOFFLINETIMEOUT|PETOFFLINECHECKTIMER|ARCHERRANGE|ADVANCEDPATHFINDING|SERVERFEATURES"
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

	ServerCrashProtection( 1 );
	InternalAccountStatus( false );
	CombatMaxRange( 10 );
	CombatArcherRange( 7 );
	CombatMaxSpellRange( 10 );
	CombatExplodeDelay( 0 );
	
	// load defaults values
	SystemTimer( tSERVER_SHOPSPAWN, 300 );
	SystemTimer( tSERVER_POISON, 180 );
	SystemTimer( tSERVER_DECAY, 300 );
	ServerSkillCap( 7000 );
	ServerStatCap( 325 );
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
	ShowOfflinePCs( true );
	CombatDisplayHitMessage( true );
	CombatAttackStamina( -2 );
	NPCTrainingStatus( true );
	CharHideWhileMounted( true );
	WeightPerStr( 5 );
	SystemTimer( tSERVER_POLYMORPH, 90 );
	ServerOverloadPackets( true );
	AdvancedPathfinding( true );

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
	PetOfflineTimeout( 5 );
	PetHungerOffline( true );
	SystemTimer( tSERVER_PETOFFLINECHECK, 600 );
	
	CheckBoatSpeed( 0.75 );
	CheckNpcAISpeed( 1 );
	CutScrollRequirementStatus( true );
	PlayerPersecutionStatus( false );
	HtmlStatsStatus( -1 );
	
	MsgBoardPostingLevel( 0 );
	MsgBoardPostRemovalLevel( 0 );
	// No replacement I can see
	EscortsEnabled( true );
	SystemTimer( tSERVER_ESCORTWAIT, 900 );
	SystemTimer( tSERVER_ESCORTACTIVE, 600 );
	SystemTimer( tSERVER_ESCORTDONE, 600 );
	AmbientFootsteps( false );
	ServerCommandPrefix( '\'' );
	
	CheckSpawnRegionSpeed( 30 );
	CheckItemsSpeed( 1.5 );
	NPCSpeed( 0.7 );
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

	SetClientFeature( CF_BIT_CHAT, true );
	SetClientFeature( CF_BIT_LBR, true );
	SetClientFeature( CF_BIT_UNKNOWN2, true );
	SetClientFeature( CF_BIT_AOS, true );
	SetClientFeature( CF_BIT_SIXCHARS, true );
	SetClientFeature( CF_BIT_SE, true );
	SetClientFeature( CF_BIT_ML, true );
	SetClientFeature( CF_BIT_EXPANSION, true );

	SetServerFeature( SF_BIT_CONTEXTMENUS, true );
	SetServerFeature( SF_BIT_AOS, true );
	SetServerFeature( SF_BIT_SIXCHARS, true );
	SetServerFeature( SF_BIT_SE, true );
	SetServerFeature( SF_BIT_ML, true );

	ServerStartGold( 1000 );
	ServerStartPrivs( 0 );
	SystemTimer( tSERVER_CORPSEDECAY, 900 );
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

void CServerData::RefreshIPs( void )
{
	struct hostent *lpHostEntry = NULL;

	std::vector< physicalServer >::iterator slIter;
	for( slIter = serverList.begin(); slIter != serverList.end(); ++slIter )
	{
		if( !slIter->getDomain().empty() )
		{
			lpHostEntry = gethostbyname( slIter->getDomain().c_str() );
			if( lpHostEntry != NULL )
			{
				struct in_addr *pinaddr;
				pinaddr = ((struct in_addr*)lpHostEntry->h_addr);
				slIter->setIP( inet_ntoa(*pinaddr) );
			}
		}
	} 
}

void CServerData::ServerName( std::string setname )
{
	if( serverList.empty() )
		serverList.resize( 1 );
	serverList[0].setName( setname );
	if( setname.empty() )
	{
		serverList[0].setName( "Default UOX Server" );
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

void CServerData::ServerAnnounceSaves( bool newVal )
{
	boolVals.set( BIT_ANNOUNCESAVES, newVal );
}

bool CServerData::ServerAnnounceSavesStatus( void ) const
{
	return boolVals.test( BIT_ANNOUNCESAVES );
}

void CServerData::ServerJoinPartAnnouncements( bool newVal )
{
	boolVals.set( BIT_ANNOUNCEJOINPART, newVal );
}

bool CServerData::ServerJoinPartAnnouncementsStatus( void ) const
{
	return boolVals.test( BIT_ANNOUNCEJOINPART );
}

void CServerData::ServerBackups( bool newVal )
{
	boolVals.set( BIT_SERVERBACKUP, newVal );
}

bool CServerData::ServerBackupStatus( void ) const
{
	return boolVals.test( BIT_SERVERBACKUP );
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

TIMERVAL CServerData::BuildSystemTimeValue( cSD_TID timerID ) const
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
			Console.Error( " %s directory is blank, set in uox.ini", verboseDirectory.c_str() );
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
			char curWorkingDir[1024];
			GetCurrentDirectory( 1024, curWorkingDir );
			int iResult = _chdir( sText.c_str() );
			if( iResult != 0 )
				error = true;
			else
				_chdir( curWorkingDir );	// move back to where we were
		}

		if( error )
		{
				Console.Error( "%s %s does not exist", verboseDirectory.c_str(), sText.c_str() );
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
	return boolVals.test( BIT_SHOOTONANIMALBACK );
}

void CServerData::ShootOnAnimalBack( bool newVal )
{
	boolVals.set( BIT_SHOOTONANIMALBACK, newVal );
}

bool CServerData::NPCTrainingStatus( void ) const
{
	return boolVals.test( BIT_NPCTRAINING );
}

void CServerData::NPCTrainingStatus( bool newVal )
{
	boolVals.set( BIT_NPCTRAINING, newVal );
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

void CServerData::CorpseLootDecay( bool newVal )
{
	boolVals.set( BIT_LOOTDECAYSONCORPSE, newVal );
}

bool CServerData::CorpseLootDecay( void ) const
{
	return boolVals.test( BIT_LOOTDECAYSONCORPSE );
}

void CServerData::GuardStatus( bool newVal )
{
	boolVals.set( BIT_GUARDSENABLED, newVal );
}

bool CServerData::GuardsStatus( void ) const
{
	return boolVals.test( BIT_GUARDSENABLED );
}

void CServerData::DeathAnimationStatus( bool newVal )
{
	boolVals.set( BIT_PLAYDEATHANIMATION, newVal );
}

bool CServerData::DeathAnimationStatus( void ) const
{
	return boolVals.test( BIT_PLAYDEATHANIMATION );
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

void CServerData::AmbientFootsteps( bool newVal )
{
	boolVals.set( BIT_AMBIENTFOOTSTEPS, newVal );
}

bool CServerData::AmbientFootsteps( void ) const
{
	return boolVals.test( BIT_AMBIENTFOOTSTEPS );
}

void CServerData::InternalAccountStatus( bool newVal )
{
	boolVals.set( BIT_INTERNALACCOUNTS, newVal );
}

bool CServerData::InternalAccountStatus( void ) const
{
	return boolVals.test( BIT_INTERNALACCOUNTS );
}

void CServerData::ShowOfflinePCs( bool newVal )
{
	boolVals.set( BIT_SHOWOFFLINEPCS, newVal );
}

bool CServerData::ShowOfflinePCs( void ) const
{
	return boolVals.test( BIT_SHOWOFFLINEPCS );
}

void CServerData::RogueStatus( bool newVal )
{
	boolVals.set( BIT_ROGUESTATUS, newVal );
}

bool CServerData::RogueStatus( void ) const
{
	return boolVals.test( BIT_ROGUESTATUS );
}

void CServerData::SnoopIsCrime( bool newVal )
{
	boolVals.set( BIT_SNOOPISCRIME, newVal );
}
bool CServerData::SnoopIsCrime( void ) const
{
	return boolVals.test( BIT_SNOOPISCRIME );
}

void CServerData::PlayerPersecutionStatus( bool newVal )
{
	boolVals.set( BIT_PERSECUTIONSTATUS, newVal );
}

bool CServerData::PlayerPersecutionStatus( void ) const
{
	return boolVals.test( BIT_PERSECUTIONSTATUS );
}

void CServerData::HtmlStatsStatus( SI16 value )
{
	htmlstatusenabled = value;
}

SI16 CServerData::HtmlStatsStatus( void ) const
{
	return htmlstatusenabled;
}

void CServerData::SellByNameStatus( bool newVal )
{
	boolVals.set( BIT_SELLBYNAME, newVal );
}

bool CServerData::SellByNameStatus( void ) const
{
	return boolVals.test( BIT_SELLBYNAME );
}

void CServerData::SellMaxItemsStatus( SI16 value )
{
	sellmaxitems = value;
}

SI16 CServerData::SellMaxItemsStatus( void ) const
{
	return sellmaxitems;
}

void CServerData::TradeSystemStatus( bool newVal )
{
	boolVals.set( BIT_TRADESYSSTATUS, newVal );
}

bool CServerData::TradeSystemStatus( void ) const
{
	return boolVals.test( BIT_TRADESYSSTATUS );
}

void CServerData::RankSystemStatus( bool newVal )
{
	boolVals.set( BIT_RANKSYSSTATUS, newVal );
}

bool CServerData::RankSystemStatus( void ) const
{
	return boolVals.test( BIT_RANKSYSSTATUS );
}

void CServerData::CutScrollRequirementStatus( bool newVal )
{
	boolVals.set( BIT_CUTSCROLLREQ, newVal );
}

bool CServerData::CutScrollRequirementStatus( void ) const
{
	return boolVals.test( BIT_CUTSCROLLREQ );
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

void CServerData::CombatDisplayHitMessage( bool newVal )
{
	boolVals.set( BIT_SHOWHITMESSAGE, newVal );
}

bool CServerData::CombatDisplayHitMessage( void ) const
{
	return boolVals.test( BIT_SHOWHITMESSAGE );
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

void CServerData::EscortsEnabled( bool newVal )
{
	boolVals.set( BIT_ESCORTSTATUS, newVal );
}
bool CServerData::EscortsEnabled( void ) const
{
	return boolVals.test( BIT_ESCORTSTATUS );
}

void CServerData::CombatMonstersVsAnimals( bool newVal )
{
	boolVals.set( BIT_MONSTERSVSANIMALS, newVal );
}
bool CServerData::CombatMonstersVsAnimals( void ) const
{
	return boolVals.test( BIT_MONSTERSVSANIMALS );
}

void CServerData::CombatAnimalsAttackChance( UI08 value )
{
	if( value > 100 )
		value = 100;
	combatanimalattackchance = value;
}
UI08 CServerData::CombatAnimalsAttackChance( void ) const
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

void CServerData::PetOfflineTimeout( UI16 value )
{
	petOfflineTimeout = value;
}
UI16 CServerData::PetOfflineTimeout( void ) const
{
	return petOfflineTimeout;
}

void CServerData::PetHungerOffline( bool newVal )
{
	boolVals.set( BIT_PETHUNGEROFFLINE, newVal );
}
bool CServerData::PetHungerOffline( void ) const
{
	return boolVals.test( BIT_PETHUNGEROFFLINE );
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
void CServerData::CharHideWhileMounted( bool newVal )
{
	boolVals.set( BIT_HIDEWHILEMOUNTED, newVal );
}
bool CServerData::CharHideWhileMounted( void ) const
{
	return boolVals.test( BIT_HIDEWHILEMOUNTED );
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
//|	Function/Class	-	bool ServerOverloadPackets()
//|	Date			-	11/20/2005
//|	Developer(s)	-	giwo
//|	Company/Team	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Purpose			-	Enables / Disables Packet handling in JS
//o--------------------------------------------------------------------------o
bool CServerData::ServerOverloadPackets( void ) const
{
	return boolVals.test( BIT_OVERLOADPACKETS );
}
void CServerData::ServerOverloadPackets( bool newVal )
{
	boolVals.set( BIT_OVERLOADPACKETS, newVal );
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
	return boolVals.test( BIT_ARMORAFFECTMANAREGEN );
}
void CServerData::ArmorAffectManaRegen( bool newVal )
{
	boolVals.set( BIT_ARMORAFFECTMANAREGEN, newVal );
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	bool AdvancedPathfinding()
//|	Date			-	7/16/2005
//|	Developer(s)	-	giwo
//|	Company/Team	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Purpose			-	Toggles whether or not we use the A* Pathfinding routine
//o--------------------------------------------------------------------------o
void CServerData::AdvancedPathfinding( bool newVal )
{
	boolVals.set( BIT_ADVANCEDPATHFIND, newVal );
}
bool CServerData::AdvancedPathfinding( void ) const
{
	return boolVals.test( BIT_ADVANCEDPATHFIND );
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

void CServerData::CombatArcherRange( SI16 value )
{
	combatarcherrange = value;
}
SI16 CServerData::CombatArcherRange( void ) const
{
	return combatarcherrange;
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

void CServerData::CombatAnimalsGuarded( bool newVal )
{
	boolVals.set( BIT_ANIMALSGUARDED, newVal );
}
bool CServerData::CombatAnimalsGuarded( void ) const
{
	return boolVals.test( BIT_ANIMALSGUARDED );
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
	if( value < 10 )
		value = 10;
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
	if( value < 10 )
		value = 10;
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

void CServerData::SetClientFeature( ClientFeatures bitNum, bool nVal )
{
	clientFeatures.set( bitNum, nVal );
}

bool CServerData::GetClientFeature( ClientFeatures bitNum ) const
{
	return clientFeatures.test( bitNum );
}

UI16 CServerData::GetClientFeatures( void ) const
{
	return static_cast<UI16>(clientFeatures.to_ulong());
}

void CServerData::SetClientFeatures( UI16 nVal )
{
	clientFeatures = nVal;
}


void CServerData::SetServerFeature( ServerFeatures bitNum, bool nVal )
{
	serverFeatures.set( bitNum, nVal );
}

bool CServerData::GetServerFeature( ServerFeatures bitNum ) const
{
	return serverFeatures.test( bitNum );
}

size_t CServerData::GetServerFeatures( void ) const
{
	return serverFeatures.to_ulong();
}

void CServerData::SetServerFeatures( size_t nVal )
{
	serverFeatures = nVal;
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
		ofsOutput << "NETSNDTIMEOUT=" << "3" << std::endl;
		ofsOutput << "NETRETRYCOUNT=" << ServerNetRetryCount() << std::endl;
		ofsOutput << "CONSOLELOG=" << static_cast<UI16>(ServerConsoleLogStatus()) << std::endl;
		ofsOutput << "CRASHPROTECTION=" << static_cast<UI16>(ServerCrashProtectionStatus()) << std::endl;
		ofsOutput << "COMMANDPREFIX=" << ServerCommandPrefix() << std::endl;
		ofsOutput << "ANNOUNCEWORLDSAVES=" << (ServerAnnounceSavesStatus()?1:0) << std::endl;
		ofsOutput << "JOINPARTMSGS=" << (ServerJoinPartAnnouncementsStatus()?1:0) << std::endl;
		ofsOutput << "BACKUPSENABLED=" << (ServerBackupStatus()?1:0) << std::endl;
		ofsOutput << "BACKUPSAVERATIO=" << BackupRatio() << std::endl;
		ofsOutput << "SAVESTIMER=" << ServerSavesTimerStatus() << std::endl;
		ofsOutput << "ACCOUNTISOLATION=" << "1" << std::endl;
		ofsOutput << "UOGENABLED=" << (ServerUOGEnabled()?1:0) << std::endl;
		ofsOutput << "}" << std::endl << std::endl;

		ofsOutput << std::endl << "[play server list]" << std::endl << "{" << std::endl;

		std::vector< physicalServer >::iterator slIter;
		for( slIter = serverList.begin(); slIter != serverList.end(); ++slIter )
		{
			ofsOutput << "SERVERLIST=" << slIter->getName() << ",";
			if( !slIter->getDomain().empty() )
				ofsOutput << slIter->getDomain() << ",";
			else
				ofsOutput << slIter->getIP() << ",";
			ofsOutput << slIter->getPort() << std::endl;
		}
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
		ofsOutput << "PETOFFLINECHECKTIMER=" << SystemTimer( tSERVER_PETOFFLINECHECK ) << std::endl;
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
		ofsOutput << "SHOWOFFLINEPCS=" << (ShowOfflinePCs()?1:0) << std::endl;
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
		ofsOutput << "CLIENTFEATURES=" << GetClientFeatures() << std::endl;
		ofsOutput << "SERVERFEATURES=" << GetServerFeatures() << std::endl;
		ofsOutput << "OVERLOADPACKETS=" << (ServerOverloadPackets()?1:0) << std::endl;
		ofsOutput << "ADVANCEDPATHFINDING=" << (AdvancedPathfinding()?1:0) << std::endl;
		ofsOutput << "}" << std::endl;

		ofsOutput << std::endl << "[speedup]" << std::endl << "{" << std::endl;
		ofsOutput << "CHECKITEMS=" << CheckItemsSpeed() << std::endl;
		ofsOutput << "CHECKBOATS=" << CheckBoatSpeed() << std::endl;
		ofsOutput << "CHECKNPCAI=" << CheckNpcAISpeed() << std::endl;
		ofsOutput << "CHECKSPAWNREGIONS=" << CheckSpawnRegionSpeed() << std::endl;
		ofsOutput << "NPCMOVEMENTSPEED=" << NPCSpeed() << std::endl;
		ofsOutput << "}" << std::endl;
		
		ofsOutput << std::endl << "[message boards]" << std::endl << "{" << std::endl;
		ofsOutput << "POSTINGLEVEL=" << static_cast<UI16>(MsgBoardPostingLevel()) << std::endl;
		ofsOutput << "REMOVALLEVEL=" << static_cast<UI16>(MsgBoardPostRemovalLevel()) << std::endl;
		ofsOutput << "}" << std::endl;

		ofsOutput << std::endl << "[escorts]" << std::endl << "{" << std::endl;
		ofsOutput << "ESCORTENABLED=" << ( EscortsEnabled() ? 1 : 0 ) << std::endl;
		ofsOutput << "ESCORTINITEXPIRE=" << SystemTimer( tSERVER_ESCORTWAIT ) << std::endl;
		ofsOutput << "ESCORTACTIVEEXPIRE=" << SystemTimer( tSERVER_ESCORTACTIVE ) << std::endl;
		ofsOutput << "ESCORTDONEEXPIRE=" << SystemTimer( tSERVER_ESCORTDONE ) << std::endl;
		ofsOutput << "}" << std::endl;
		
		ofsOutput << std::endl << "[worldlight]" << std::endl << "{" << std::endl;
		ofsOutput << "DUNGEONLEVEL=" << static_cast<UI16>(DungeonLightLevel()) << std::endl;
		ofsOutput << "BRIGHTLEVEL=" << static_cast<UI16>(WorldLightBrightLevel()) << std::endl;
		ofsOutput << "DARKLEVEL=" << static_cast<UI16>(WorldLightDarkLevel()) << std::endl;
		ofsOutput << "SECONDSPERUOMINUTE=" << ServerSecondsPerUOMinute() << std::endl;
		ofsOutput << "}" << std::endl;
		
		ofsOutput << std::endl << "[tracking]" << std::endl << "{" << std::endl;
		ofsOutput << "BASERANGE=" << TrackingBaseRange() << std::endl;
		ofsOutput << "BASETIMER=" << TrackingBaseTimer() << std::endl;
		ofsOutput << "MAXTARGETS=" << static_cast<UI16>(TrackingMaxTargets()) << std::endl;
		ofsOutput << "MSGREDISPLAYTIME=" << TrackingRedisplayTime() << std::endl;
		ofsOutput << "}" << std::endl;
		
		ofsOutput << std::endl << "[reputation]" << std::endl << "{" << std::endl;
		ofsOutput << "MURDERDECAYTIMER=" << SystemTimer( tSERVER_MURDERDECAY ) << std::endl;
		ofsOutput << "MAXKILLS=" << RepMaxKills() << std::endl;
		ofsOutput << "CRIMINALTIMER=" << SystemTimer( tSERVER_CRIMINAL ) << std::endl;
		ofsOutput << "}" << std::endl;
		
		ofsOutput << std::endl << "[resources]" << std::endl << "{" << std::endl;
		ofsOutput << "MINECHECK=" << static_cast<UI16>(MineCheck()) << std::endl;
		ofsOutput << "OREPERAREA=" << ResOre() << std::endl;
		ofsOutput << "ORERESPAWNTIMER=" << ResOreTime() << std::endl;
		ofsOutput << "ORERESPAWNAREA=" << ResOreArea() << std::endl;
		ofsOutput << "LOGSPERAREA=" << ResLogs() << std::endl;
		ofsOutput << "LOGSRESPAWNTIMER=" << ResLogTime() << std::endl;
		ofsOutput << "LOGSRESPAWNAREA=" << ResLogArea() << std::endl;
		ofsOutput << "}" << std::endl;
		
		ofsOutput << std::endl << "[hunger]" << std::endl << "{" << std::endl;
		ofsOutput << "HUNGERRATE=" << SystemTimer( tSERVER_HUNGERRATE ) << std::endl;
		ofsOutput << "HUNGERDMGVAL=" << HungerDamage() << std::endl;
		ofsOutput << "PETHUNGEROFFLINE=" << (PetHungerOffline()?1:0) << std::endl;
		ofsOutput << "PETOFFLINETIMEOUT=" << PetOfflineTimeout() << std::endl;
		ofsOutput << "}" << std::endl;
		
		ofsOutput << std::endl << "[combat]" << std::endl << "{" << std::endl;
		ofsOutput << "MAXRANGE=" << CombatMaxRange() << std::endl;
		ofsOutput << "ARCHERRANGE=" << CombatArcherRange() << std::endl;
		ofsOutput << "SPELLMAXRANGE=" << CombatMaxSpellRange() << std::endl;
		ofsOutput << "DISPLAYHITMSG=" << (CombatDisplayHitMessage()?1:0) << std::endl;
		ofsOutput << "MONSTERSVSANIMALS=" << (CombatMonstersVsAnimals()?1:0) << std::endl;
		ofsOutput << "ANIMALATTACKCHANCE=" << static_cast<UI16>(CombatAnimalsAttackChance()) << std::endl;
		ofsOutput << "ANIMALSGUARDED=" << (CombatAnimalsGuarded()?1:0) << std::endl;
		ofsOutput << "NPCDAMAGERATE=" << CombatNPCDamageRate() << std::endl;
		ofsOutput << "NPCBASEFLEEAT=" << CombatNPCBaseFleeAt() << std::endl;
		ofsOutput << "NPCBASEREATTACKAT=" << CombatNPCBaseReattackAt() << std::endl;
		ofsOutput << "ATTACKSTAMINA=" << CombatAttackStamina() << std::endl;
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
		Console.Error( "Unable to open file %s for writing", filename.c_str() );
	return rvalue;
}

//o------------------------------------------------------------------------
//|	Function		-	Load()
//|	Programmer		-	EviLDeD
//|	Date			-	January 13, 2001
//|	Modified		-
//o------------------------------------------------------------------------
//|	Purpose			-	Load up the uox.ini file and parse it into the internals
//|	Returns			-	pointer to the valid inmemory serverdata storage(this)
//|						NULL is there is an error, or invalid file type
//o------------------------------------------------------------------------
void CServerData::Load( void )
{
	const UString fileName = Directory( CSDDP_ROOT ) + "uox.ini";
	ParseINI( fileName );
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
//|	Function/Class	-	bool CServerData::ParseINI( const std::string filename )
//|	Date			-	02/26/2001
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	Parse the uox.ini file into its required information.
//|									
//|	Modification	-	02/26/2002	-	Make sure that we parse out the logs, access
//|									and other directories that we were not parsing/
//o--------------------------------------------------------------------------o
//|	Returns			-
//o--------------------------------------------------------------------------o
bool CServerData::ParseINI( const std::string filename )
{
	bool rvalue = false;
	if( !filename.empty() )
	{
		Console << "Processing INI Settings  ";

		Script toParse( filename, NUM_DEFS, false );
		// Lock this file tight, No access at anytime when open(should only be open and closed anyhow. For Thread blocking)
		if( !toParse.IsErrored() )
		{
			serverList.clear();
			startlocations.clear();
			for( ScriptSection *sect = toParse.FirstEntry(); sect != NULL; sect = toParse.NextEntry() )
			{
				if( sect == NULL )
					continue;
				UString tag, data;
				for( tag = sect->First(); !sect->AtEnd(); tag = sect->Next() )
				{
					data = sect->GrabData().simplifyWhiteSpace();
					HandleLine( tag, data );
				}
			}
			Console.PrintDone();
			rvalue = true;
		}
		else
		{
			Console.Warning( "%s File not found, Using default settings.", filename.c_str() );
			cwmWorldState->ServerData()->save();
		}
	}
	return rvalue;
}

void CServerData::HandleLine( const UString tag, const UString value )
{
	switch( UOX3INI_LOOKUP.find( tag.c_str() ) )
	{
	case 0x0001:	// 04/03/2004 - EviLDeD - Seems that the new code can see the first case now. not completly tested, and its not going to kill us to allow the fall through
	case 0x000C:	 // SERVERNAME[0002]
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
	case 0x02A2:	 // SHOWOFFLINEPCS[0046]
		ShowOfflinePCs( value.toUShort() != 0 );
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
		CutScrollRequirementStatus( (value.toUShort() != 0) );
		break;
	case 0x0335:	 // CHECKITEMS[0056]
		CheckItemsSpeed( value.toDouble() );
		break;
	case 0x0340:	 // CHECKBOATS[0057]
		CheckBoatSpeed( value.toDouble() );
		break;
	case 0x034B:	 // CHECKNPCAI[0058]
		CheckNpcAISpeed( value.toDouble() );
		break;
	case 0x0356:	 // CHECKSPAWNREGIONS[0059]
		CheckSpawnRegionSpeed( value.toDouble() );
		break;
	case 0x0368:	 // POSTINGLEVEL[0060]
		MsgBoardPostingLevel( value.toUByte() );
		break;
	case 0x0375:	 // REMOVALLEVEL[0061]
		MsgBoardPostRemovalLevel( value.toUByte() );
		break;
	case 0x0382:	 // ESCORTENABLED[0062]
		EscortsEnabled( value.toUShort() == 1 );
		break;
	case 0x0390:	 // ESCORTINITEXPIRE[0063]
		SystemTimer( tSERVER_ESCORTWAIT, value.toUShort() );
		break;
	case 0x03A1:	 // ESCORTACTIVEEXPIRE[0064]
		SystemTimer( tSERVER_ESCORTACTIVE, value.toUShort() );
		break;
	case 0x03B4:	 // MOON1[0065]
		ServerMoon( 0, value.toShort() );
		break;
	case 0x03BA:	 // MOON2[0066]
		ServerMoon( 1, value.toShort() );
		break;
	case 0x03C0:	 // DUNGEONLEVEL[0067]
		DungeonLightLevel( (LIGHTLEVEL)value.toUShort() );
		break;
	case 0x03CD:	 // CURRENTLEVEL[0068]
		WorldLightCurrentLevel( (LIGHTLEVEL)value.toUShort() );
		break;
	case 0x03DA:	 // BRIGHTLEVEL[0069]
		WorldLightBrightLevel( (LIGHTLEVEL)value.toUShort() );
		break;
	case 0x03E6:	 // BASERANGE[0070]
		TrackingBaseRange( value.toUShort() );
		break;
	case 0x03F0:	 // BASETIMER[0071]
		TrackingBaseTimer( value.toUShort() );
		break;
	case 0x03FA:	 // MAXTARGETS[0072]
		TrackingMaxTargets( value.toUByte() );
		break;
	case 0x0405:	 // MSGREDISPLAYTIME[0073]
		TrackingRedisplayTime( value.toUShort() );
		break;
	case 0x0416:	 // MURDERDECAYTIMER[0074]
		SystemTimer( tSERVER_MURDERDECAY, value.toUShort() );
		break;
	case 0x0427:	 // MAXKILLS[0075]
		RepMaxKills( value.toUShort() );
		break;
	case 0x0430:	 // CRIMINALTIMER[0076]
		SystemTimer( tSERVER_CRIMINAL, value.toUShort() );
		break;
	case 0x043E:	 // MINECHECK[0077]
		MineCheck( value.toUByte() );
		break;
	case 0x0448:	 // OREPERAREA[0078]
		ResOre( value.toShort() );
		break;
	case 0x0453:	 // ORERESPAWNTIMER[0079]
		ResOreTime( value.toUShort() );
		break;
	case 0x0463:	 // ORERESPAWNAREA[0080]
		ResOreArea( value.toUShort() );
		break;
	case 0x0472:	 // LOGSPERAREA[0081]
		ResLogs( value.toShort() );
		break;
	case 0x047E:	 // LOGSRESPAWNTIMER[0082]
		ResLogTime( value.toUShort() );
		break;
	case 0x048F:	 // LOGSRESPAWNAREA[0083]
		ResLogArea( value.toUShort() );
		break;
	case 0x049F:	 // HUNGERRATE[0084]
		SystemTimer( tSERVER_HUNGERRATE, value.toUShort() );
		break;
	case 0x04AA:	 // HUNGERDMGVAL[0085]
		HungerDamage( value.toShort() );
		break;
	case 0x04B7:	 // MAXRANGE[0086]
		CombatMaxRange( value.toShort() );
		break;
	case 0x04C0:	 // SPELLMAXRANGE[0087]
		CombatMaxSpellRange( value.toShort() );
		break;
	case 0x04CE:	 // DISPLAYHITMSG[0088]
		CombatDisplayHitMessage( value.toUShort() == 1 );
		break;
	case 0x04DC:	 // MONSTERSVSANIMALS[0089]
		CombatMonstersVsAnimals( value.toUShort() == 1 );
		break;
	case 0x04EE:	 // ANIMALATTACKCHANCE[0090]
		CombatAnimalsAttackChance( value.toUByte() );
		break;
	case 0x0501:	 // ANIMALSGUARDED[0091]
		CombatAnimalsGuarded( value.toUShort() == 1 );
		break;
	case 0x0510:	 // NPCDAMAGERATE[0092]
		CombatNPCDamageRate( value.toShort() );
		break;
	case 0x051E:	 // NPCBASEFLEEAT[0093]
		CombatNPCBaseFleeAt( value.toShort() );
		break;
	case 0x052C:	 // NPCBASEREATTACKAT[0094]
		CombatNPCBaseReattackAt( value.toShort() );
		break;
	case 0x053E:	 // ATTACKSTAMINA[0095]
		CombatAttackStamina( value.toShort() );
		break;
	case 0x054C:	 // LOCATION[0096]
		ServerLocation( value );
		break;
	case 0x0555:	 // STARTGOLD[0097]
		ServerStartGold( value.toShort() );
		break;
	case 0x055F:	 // STARTPRIVS[0098]
		ServerStartPrivs( value.toUShort() );
		break;
	case 0x056A:	 // ESCORTDONEEXPIRE[0099]
		SystemTimer( tSERVER_ESCORTDONE, value.toUShort() );
		break;
	case 0x057B:	 // LIGHTDARKLEVEL[0100]
		WorldLightDarkLevel( (LIGHTLEVEL)value.toUShort() );
		break;
	case 0x058A:	 // TITLECOLOUR[0101]
		TitleColour( value.toUShort() );
		break;
	case 0x0596:	 // LEFTTEXTCOLOUR[0102]
		LeftTextColour( value.toUShort() );
		break;
	case 0x05A5:	 // RIGHTTEXTCOLOUR[0103]
		RightTextColour( value.toUShort() );
		break;
	case 0x05B5:	 // BUTTONCANCEL[0104]
		ButtonCancel( value.toUShort() );
		break;
	case 0x05C2:	 // BUTTONLEFT[0105]
		ButtonLeft( value.toUShort() );
		break;
	case 0x05CD:	 // BUTTONRIGHT[0106]
		ButtonRight( value.toUShort() );
		break;
	case 0x05D9:	 // BACKGROUNDPIC[0107]
		BackgroundPic( value.toUShort() );
		break;
	case 0x05E7:	 // POLLTIME[0108]
		TownNumSecsPollOpen( value.toULong() );
		break;
	case 0x05F0:	 // MAYORTIME[0109]
		TownNumSecsAsMayor( value.toULong() );
		break;
	case 0x05FA:	 // TAXPERIOD[0110]
		TownTaxPeriod( value.toULong() );
		break;
	case 0x0604:	 // GUARDSPAID[0111]
		TownGuardPayment( value.toULong() );
		break;
	case 0x060F:	 // DAY[0112]
		ServerTimeDay( value.toShort() );
		break;
	case 0x0613:	 // HOURS[0113]
		ServerTimeHours( value.toUByte() );
		break;
	case 0x0619:	 // MINUTES[0114]
		ServerTimeMinutes( value.toUByte() );
		break;
	case 0x0621:	 // SECONDS[0115]
		ServerTimeSeconds( value.toUByte() );
		break;
	case 0x0629:	 // AMPM[0116]
		ServerTimeAMPM( value.toUShort() != 0 );
		break;
	case 0x062E:	 // SKILLLEVEL[0117]
		SkillLevel( value.toUByte() );
		break;
	case 0x0639:	 // SNOOPISCRIME[0118]
		SnoopIsCrime( value.toUShort() != 0 );
		break;
	case 0x0646:	 // BOOKSDIRECTORY[0119]
		Directory( CSDDP_BOOKS, value );
		break;
	case 0x0655:	 // SERVERLIST[0120]
	{
		UString sname, sip, sport;
		struct hostent *lpHostEntry = NULL;
		physicalServer toAdd;
		if( value.sectionCount( "," ) == 2 )
		{
			sname	= value.section( ",", 0, 0 ).stripWhiteSpace();
			sip		= value.section( ",", 1, 1 ).stripWhiteSpace();
			sport	= value.section( ",", 2, 2 ).stripWhiteSpace();
	
			toAdd.setName( sname );
			// Ok look up the data here see if its a number
			bool bDomain = true;
			if( ( lpHostEntry = gethostbyname( sip.c_str() ) ) == NULL )
			{
				// this was not a domain name so check for IP address
				if( ( lpHostEntry = gethostbyaddr( sip.c_str(), sip.size(), AF_INET ) ) == NULL )
				{
					// We get here it wasn't a valid IP either.
					Console.Warning( "Failed to translate %s", sip.c_str() );
					Console.Warning( "This shard will not show up on the shard listing" );
					break;
				}
				bDomain = false;
			}
			// Going to store a copy of the domain name as well to save to the ini if there is a domain name insteead of an ip.
			if( bDomain )	// Store the domain name for later then seeing as its a valid one
				toAdd.setDomain( sip );
			else			// this was a valid ip address so we will use an ip instead so clear the domain string.
				toAdd.setDomain( "" );

			// Ok now the server itself uses the ip so we need to store that :) Means we only need to look thisip once 
			struct in_addr *pinaddr;
			pinaddr = ((struct in_addr*)lpHostEntry->h_addr);
			toAdd.setIP( inet_ntoa(*pinaddr) );
			toAdd.setPort( sport.toUShort() );
			serverList.push_back( toAdd );
		}
		else
		{
			Console.Warning( "Malformend Serverlist entry: %s", value.c_str() );
			Console.Warning( "This shard will not show up on the shard listing" );
		}
		break;
	}
	case 0x0660:	 // PORT[0121]
		ServerPort( value.toUShort() );
		break;
	case 0x0665:	 // ACCESSDIRECTORY[0122]
		Directory( CSDDP_ACCESS, value );
		break;
	case 0x0675:	 // LOGSDIRECTORY[0123]
		Directory( CSDDP_LOGS, value );
		break;
	case 0x0683:	 // ACCOUNTISOLATION[0124]
		break;
	case 0x0694:	 // HTMLDIRECTORY[0125]
		Directory( CSDDP_HTML, value );
		break;
	case 0x06A2:	 // SHOOTONANIMALBACK[0126]
		ShootOnAnimalBack( value.toUShort() == 1 );
		break;
	case 0x06B4:	 // NPCTRAININGENABLED[0127]
		NPCTrainingStatus( value.toUShort() == 1 );
		break;
	case 0x06C7:	 // DICTIONARYDIRECTORY[0128]
		Directory( CSDDP_DICTIONARIES, value );
		break;
	case 0x06DB:	 // BACKUPSAVERATIO[0129]
		BackupRatio( value.toShort() );
		break;
	case 0x06EB:	 // HIDEWILEMOUNTED[0130]
		CharHideWhileMounted( value.toShort() == 1 );
		break;
	case 0x06FB:	 // SECONDSPERUOMINUTE[0131]
		ServerSecondsPerUOMinute( value.toUShort() );
		break;
	case 0x070E:	 // WEIGHTPERSTR[0132]
		WeightPerStr( value.toUByte() );
		break;
	case 0x071B:	 // POLYDURATION[0133]
		SystemTimer( tSERVER_POLYMORPH, value.toUShort() );
		break;
	case 0x0728:	 // UOGENABLED[0134]
		ServerUOGEnabled( value.toShort()==1 );
		break;
	case 0x0733:	 // NETRCVTIMEOUT[0135]
		ServerNetRcvTimeout( value.toULong() );
		break;
	case 0x0741:	 // NETSNDTIMEOUT[0136]
		ServerNetSndTimeout( value.toULong() );
		break;
	case 0x074F:	 // NETRETRYCOUNT[0137]
		ServerNetRetryCount( value.toULong() );
		break;
	case 0x075D:	 // CLIENTFEATURES[0138]
		SetClientFeatures( value.toUShort() );
		break;
	case 0x076C:	 // PACKETOVERLOADS[0139]
		ServerOverloadPackets( (value.toByte() == 1) );
		break;
	case 0x077C:	 // NPCMOVEMENTSPEED[0140]
		NPCSpeed( value.toDouble() );
		break;
	case 0x078D:	 // PETHUNGEROFFLINE[0141]
		PetHungerOffline( (value.toByte() == 1) );
		break;
	case 0x079E:	 // PETOFFLINETIMEOUT[0142]
		PetOfflineTimeout( value.toUShort() );
		break;
	case 0x07B0:	 // PETOFFLINECHECKTIMER[0143]
		SystemTimer( tSERVER_PETOFFLINECHECK, value.toUShort() );
		break;
	case 0x07C5:	 // ARCHERRANGE[0144]
		CombatArcherRange( value.toShort() );
		break;
	case 0x07D1:	 // ADVANCEDPATHFINDING[0145]
		AdvancedPathfinding( (value.toByte() == 1) );
		break;
	case 0x07E5:	 // SERVERFEATURES[0146]
		SetServerFeatures( value.toULong() );
		break;
	default:
		break;
	}
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
		Console.Error( "Malformed location entry in ini file" );
	}
}
LPSTARTLOCATION CServerData::ServerLocation( size_t locNum )
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

//o--------------------------------------------------------------------------o
//|	Function/Class	-	void CServerData::SaveTime( void )
//|	Date			-	January 28th, 2007
//|	Developer(s)	-	giwo
//|	Company/Team	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Outputs server time information to time.wsc in the /shared/ directory
//o--------------------------------------------------------------------------o	
void CServerData::SaveTime( void )
{
	std::string		timeFile = cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "time.wsc";
	std::ofstream	timeDestination( timeFile.c_str() );
	if( !timeDestination ) 
	{
		Console.Error( "Failed to open %s for writing", timeFile.c_str() );
		return;
	}

	timeDestination << "[TIME]" << std::endl << "{" << std::endl;
	timeDestination << "CURRENTLIGHT=" << static_cast<UI16>(WorldLightCurrentLevel()) << std::endl;
	timeDestination << "DAY=" << ServerTimeDay() << std::endl;
	timeDestination << "HOUR=" << static_cast<UI16>(ServerTimeHours()) << std::endl;
	timeDestination << "MINUTE=" << static_cast<UI16>(ServerTimeMinutes()) << std::endl;
	timeDestination << "AMPM=" << ( ServerTimeAMPM() ? 1 : 0 ) << std::endl;
	timeDestination << "MOON=" << ServerMoon( 0 ) << "," << ServerMoon( 1 ) << std::endl;
	timeDestination << "}" << std::endl << std::endl;

	timeDestination.close();
}

void ReadWorldTagData( std::ifstream &inStream, UString &tag, UString &data );
void CServerData::LoadTime( void )
{
	std::ifstream input;
	std::string filename = cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "time.wsc";

	input.open( filename.c_str(), std::ios_base::in );
	input.seekg( 0, std::ios::beg );

	char line[1024];

	if( input.is_open() )
	{
		while( !input.eof() && !input.fail() )
		{
			input.getline( line, 1024 );
			UString sLine( line );
			sLine = sLine.removeComment().stripWhiteSpace();
			if( !sLine.empty() )
			{
				if( sLine.upper() == "[TIME]" )
					LoadTimeTags( input );
			}
		}
		input.close();
	}
}

void CServerData::LoadTimeTags( std::ifstream &input )
{
	UString UTag, tag, data;
	while( tag != "o---o" )
	{
		ReadWorldTagData( input, tag, data );
		if( tag != "o---o" )
		{
			UTag = tag.upper();
			if( UTag == "AMPM" )
				ServerTimeAMPM( (data.toByte() == 1) );
			else if( UTag == "CURRENTLIGHT" )
				WorldLightCurrentLevel( data.toUShort() );
			else if( UTag == "DAY" )
				ServerTimeDay( data.toShort() );
			else if( UTag == "HOUR" )
				ServerTimeHours( data.toUShort() );
			else if( UTag == "MINUTE" )
				ServerTimeMinutes( data.toUShort() );
			else if( UTag == "MOON" )
			{
				if( data.sectionCount( "," ) != 0 )
				{
					ServerMoon( 0, data.section( ",", 0, 0 ).stripWhiteSpace().toShort() );
					ServerMoon( 1, data.section( ",", 1, 1 ).stripWhiteSpace().toShort() );
				}
			}
		}
	}
	tag = "";
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
