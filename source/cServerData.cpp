#include "uox3.h"

#if UOX_PLATFORM != PLATFORM_WIN32
	#include <dirent.h>
#else
	#include <direct.h>
#endif

namespace UOX
{

#define REGENRATE1 8 // Seconds to heal ONE hp
#define REGENRATE2 3 // Seconds to heal ONE stm
#define REGENRATE3 5 // Seconds to heal ONE mn
#define PLAYERCORPSEDECAYMULTIPLIER 3
#define SPIRITSPEAKTIMER 30 // spiritspeak lasts at least 30 seconds, additional time is calculated by Skill & INT
#define BEGGINGRANGE 3                  // begging range
#define FISHINGTIMEBASE 10              // it takes at least FISHINGTIMEBASE sec to fish
#define FISHINGTIMER 5
#define DECAYTIMER 300  // 5 minute decay rate
#define HUNGERRATE 6000 // 15 minutes
#define HUNGERDAMAGERATE 10 //every 10 seconds
#define HUNGERDAMAGE 2 //-2 hp
#define POISONTIMER 180
#define POTIONDELAY 10
#define STATCAP 325
#define SKILLCAP 7000
#define INVISTIMER 60 // invisibility spell lasts 1 minute
#define SKILLDELAY 5   // Skill usage delay     (5 seconds)
#define OBJECTDELAY 1
#define GATETIMER 30
#define ATTACKSTAMINA -2 // AntiChrist - attacker loses stamina when hits
#define ANIMALS_ATTACK_CHANCE 15 // By default 15% of chance to attack an animal
#define MAX_ABSORBTION 20 // 20 Arm (single armour piece) -- Magius(CHE)
#define MAX_NON_HUMAN_ABSORBTION 100 // 100 Arm (total armour) -- Magius(CHE)
#define NPC_DAMAGE_RATE 2 // DAMAGE / NPC_DAMAGE_RATE for NPCs -- Magius(CHE)
#define NPC_BASE_FLEEAT 20
#define NPC_BASE_REATTACKAT 40
#define SKILLLEVEL 5 // Range from 1 to 10 - This value if the difficult to create an item using a make command: 1 is easy, 5 is normal (default), 10 is difficult! - Magius(CHE)
#define SELLMAXITEM 5 // Set maximum amount of items that one player can sell at a time ( 5 is standard OSI ) --- Magius(CHE)
#define RANKSYSTEM 1 // Rank sstem to make various type of a single item based on the creator's skill! - Magius(CHE)
#define NPCSPEED 1	// 2 steps every second, ab said to change this to 0.5 - fur - NPC's slowed down a bit - Zane
#define TRACKINGRANGE 10        // tracking range is at least TRACKINGRANGE, additional distance is calculated by Skill,INT
#define MAXTRACKINGTARGETS 20   // maximum number of trackable targets
#define TRACKINGTIMER 30        // tracking last at least TRACKINGTIMER seconds, additional time is calculated by Skill,INT, & DEX
#define TRACKINGDISPLAYTIME 30  // tracking will display above the character every TRACKINGDISPLAYTIME seconds
#define MAX_TRACKINGTARGETS	128

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
	"SKILLCAP|SKILLDELAY|STATCAP|MAXSTEALTHMOVEMENTS|MAXSTAMINAMOVEMENTS|CORPSEDECAYTIMER|LOOTDECAYTIMER|WEATHERTIMER|SHOPSPAWNTIMER|DECAYTIMER|INVISIBILITYTIMER|"
	"OBJECTUSETIMER|GATETIMER|POISONTIMER|LOGINTIMEOUT|HITPOINTREGENTIMER|STAMINAREGENTIMER|MANAREGENTIMER|ARMORAFFECTREGENTIMER|BASEFISHINGTIMER|RANDOMFISHINGTIMER|SPIRITSPEAKTIMER|"
	"BEGGINGTIMER|DIRECTORY|DATADIRECTORY|DEFSDIRECTORY|ACTSDIRECTORY|SCRIPTSDIRECTORY|BACKUPDIRECTORY|MSGBOARDDIRECTORY|SHAREDDIRECTORY|LOOTDECAYSWITHCORPSE|GUARDSACTIVE|DEATHANIMATION|"
	"AMBIENTSOUNDS|AMBIENTFOOTSTEPS|INTERNALACCOUNTCREATION|SHOWHIDDENNPCS|ROGUESENABLED|PLAYERPERSECUTION|ACCOUNTFLUSH|HTMLSTATUSENABLED|"
	"SELLBYNAME|SELLMAXITEMS|TRADESYSTEM|RANKSYSTEM|CUTSCROLLREQUIREMENTS|SPEEDCHECKITEMS|SPEEDCHECKBOATS|SPEEDCHECKNPCAI|"
	"SPEEDCHECKSPAWNREGIONS|MSGBOARDPOSTINGLEVEL|MSGBOARDREMOVALLEVEL|ESCORTENABLED|ESCORTINITEXPIRE|ESCORTACTIVEEXPIRE|LIGHTMOON1|LIGHTMOON2|"
	"LIGHTDUNGEONLEVEL|LIGHTCURRENTLEVEL|LIGHTBRIGHTLEVEL|BEGGINGRANGE|TRACKINGBASERANGE|TRACKINGBASETIMER|TRACKINGMAXTARGETS|TRACKINGMSGREDISPLAYTIME|"
	"REPSYSMURDERDECAYTIMER|REPSYSMAXKILLS|REPSYSCRIMINALTIMER|RESOURCEMINECHECK|RESOURCEOREPERAREA|RESOURCEORERESPAWNTIMER|RESOURCEORERESPAWNAREA|RESOURCELOGSPERAREA|RESPURCELOGSRESPAWNTIMER|RESOURCELOGSRESPAWNAREA|HUNGERRATE|HUNGERDAMAGERATETIMER|HUNGERDMGVAL|HUNGERTHRESHOLD|"
	"COMBATMAXRANGE|COMBATSPELLMAXRANGE|COMBATDISPLAYHITMSG|COMBATMONSTERSVSANIMALS|"
	"COMBATANIMALATTACKCHANCE|COMBATANIMALSGUARDED|COMBATNPCDAMAGERATE|COMBATNPCBASEFLEEAT|COMBATNPCBASEREATTACKAT|COMBATATTACKSTAMINA|LOCATION|STARTGOLD|STARTPRIVS1|STARTPRIVS2|ESCORTDONEEXPIRE|LIGHTDARKLEVEL|"
	"TITLECOLOUR|LEFTTEXTCOLOUR|RIGHTTEXTCOLOUR|BUTTONCANCEL|BUTTONLEFT|BUTTONRIGHT|BACKGROUNDPIC|POLLTIME|MAYORTIME|TAXPERIOD|GUARDSPAID|DAY|HOURS|MINUTES|SECONDS|AMPM|SKILLLEVEL|SNOOPISCRIME|BOOKSDIRECTORY|SERVERLIST|PORT|"
	"ACCESSDIRECTORY|LOGSDIRECTORY|ACCOUNTISOLATION|HTMLDIRECTORY|SHOOTONANIMALBACK|NPCTRAININGENABLED|DICTIONARYDIRECTORY|BACKUPSAVERATIO|HIDEWILEMOUNTED|SECONDSPERUOMINUTE|WEIGHTPERSTR|POLYDURATION|"
	"UOGENABLED|NETRCVTIMEOUT|NETSNDTIMEOUT|NETRETRYCOUNT|USEFACETSAVES|MAP0|MAP1|MAP2|MAP3|USERMAP|CLIENTSUPPORT|"
	"FTPDENABLED|FTPDUSERLIMIT|FTPDBIND|FTPDROOT|FTPDUSER|FTPCENABLED|FTPCHOST|FTPCPORT|FTPCUSER|FTPCULFLAGS"
);

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
	if( skdelay < 1 )		// Default is on second loop sleeping
		skilldelay = SKILLDELAY;
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
		const char dirSep = '/';

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

void CServerData::MsgBoardPostingLevel( SI16 value )
{
	msgpostinglevel = value;
}

SI16 CServerData::MsgBoardPostingLevel( void ) const
{
	return msgpostinglevel;
}

void CServerData::MsgBoardPostRemovalLevel( SI16 value )
{
	msgremovallevel = value;
}

SI16 CServerData::MsgBoardPostRemovalLevel( void ) const
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

void CServerData::EscortInitExpire( UI16 value )
{
	escortinitexpire = value;
}
UI16 CServerData::EscortInitExpire( void ) const
{
	return escortinitexpire;
}

void CServerData::EscortActiveExpire( UI16 value )
{
	escortactiveexpire = value;
}
UI16 CServerData::EscortActiveExpire( void ) const
{
	return escortactiveexpire;
}

void CServerData::EscortDoneExpire( UI16 value )
{
	escortdoneexpire = value;
}
UI16 CServerData::EscortDoneExpire( void ) const
{
	return escortdoneexpire;
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

void CServerData::HungerRate( UI16 value )
{
	hungerrate = value;
}
UI16 CServerData::HungerRate( void ) const
{
	return hungerrate;
}

void CServerData::HungerDamageRateTimer( UI16 value )
{
	hungerdamageratetimer = value;
}
UI16 CServerData::HungerDamageRateTimer( void ) const
{
	return hungerdamageratetimer;
}

void CServerData::HungerDamage( SI16 value )
{
	hungerdamage = value;
}
SI16 CServerData::HungerDamage( void ) const
{
	return hungerdamage;
}

void CServerData::HungerThreshold( SI16 value )
{
	hungerthreshold = value;
}
SI16 CServerData::HungerThreshold( void ) const
{
	return hungerthreshold;
}

void CServerData::PotionDelay( SI16 value )
{
	potiondelay = value;
}
SI16 CServerData::PotionDelay( void ) const
{
	return potiondelay;
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
//|	Function		-	UI16 PolyDuration()
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Duration (in seconds) of polymorph spell
//o--------------------------------------------------------------------------o
UI16 CServerData::PolyDuration( void ) const
{
	return polyDuration;
}
void CServerData::PolyDuration( UI16 newVal )
{
	polyDuration = newVal;
}

void CServerData::BackupRatio( SI16 value )
{
	backupRatio = value;
}
SI16 CServerData::BackupRatio( void ) const
{
	return backupRatio;
}

void CServerData::ResetDefaults( void )
{
	resettingDefaults = true;
	serverList.resize( 1 );		// Set the initial count to hold one server. 

	ServerIP( "127.0.0.1" );
	ServerPort( 2593 );
	serverList[0].setPort( 2593 );
	ServerName( "Default UOX3 Server" );

	PotionDelay( POTIONDELAY );
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
	SystemTimer( SHOP_SPAWN, 300 );
	SystemTimer( POISON, POISONTIMER );
	PotionDelay( POTIONDELAY );
	SystemTimer( DECAY, DECAYTIMER );
	ServerSkillCap( SKILLCAP );
	ServerStatCap( STATCAP );
	SystemTimer( PLAYER_CORPSE, 3 );
	CorpseLootDecay( true );
	ServerSavesTimer( 300 );
	
	SystemTimer( INVISIBILITY, INVISTIMER );
	HungerRate( HUNGERRATE );
	HungerDamageRateTimer( HUNGERDAMAGERATE );
	HungerDamage( HUNGERDAMAGE );
	HungerThreshold( 3 );
	
	ServerSkillDelay( SKILLDELAY );
	SystemTimer( OBJECT_USAGE, OBJECTDELAY );
	SystemTimer( HITPOINT_REGEN, REGENRATE1 );
	SystemTimer( STAMINA_REGEN, REGENRATE2 );
	SystemTimer( MANA_REGEN, REGENRATE3 );
	SystemTimer( ARMORAFFECTMANA_REGEN, 0 );
	SystemTimer( GATE, GATETIMER );
	MineCheck( 2 );
	DeathAnimationStatus( true );
	ShowHiddenNpcStatus( true );
	CombatDisplayHitMessage( true );
	CombatAttackStamina( ATTACKSTAMINA );
	NPCTrainingStatus( true );
	CharHideWhileMounted( true );
	WeightPerStr( 5 );
	PolyDuration( 90 );

	CombatMonstersVsAnimals( true );
	CombatAnimalsAttackChance( ANIMALS_ATTACK_CHANCE );
	CombatAnimalsGuarded( false );
	CombatNPCBaseFleeAt( NPC_BASE_FLEEAT );
	CombatNPCBaseReattackAt( NPC_BASE_REATTACKAT );
	ShootOnAnimalBack( false );
	SellByNameStatus( false );
	SkillLevel( SKILLLEVEL );
	SellMaxItemsStatus( SELLMAXITEM );
	CombatNPCDamageRate( NPC_DAMAGE_RATE );
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
	SystemTimer( WEATHER, 60 );
	SystemTimer( LOGIN_TIMEOUT, 300 );
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
	EscortInitExpire( 10 );
	EscortActiveExpire( 20 );
	EscortDoneExpire( 1800 );
	AmbientFootsteps( false );
	ServerCommandPrefix( '\'' );
	
	CheckSpawnRegionSpeed( 30 );
	CheckItemsSpeed( 1.5 );
	NPCSpeed( 1.0 );
	AccountFlushTimer( 0.0 );
	NPCSpeed( NPCSPEED );
	
	ResLogs( 3 );
	ResLogTime( 600 );
	ResLogArea( 10 );
	ResOre( 10 );
	ResOreTime( 600 );
	ResOreArea( 10 );
	//REPSYS
	RepCrimTime( 120 );
	RepMaxKills( 4 );
	RepMurderDecay( 60 );
	//RepSys ---^
	TrackingBaseRange( TRACKINGRANGE );
	TrackingMaxTargets( MAXTRACKINGTARGETS );
	TrackingBaseTimer( TRACKINGTIMER );
	TrackingRedisplayTime( TRACKINGDISPLAYTIME );
	BeggingRange( BEGGINGRANGE );

	SystemTimer( BASE_FISHING, FISHINGTIMEBASE );
	SystemTimer( RANDOM_FISHING, FISHINGTIMER );
	SystemTimer( SPIRIT_SPEAK, SPIRITSPEAKTIMER );

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
	SystemTimer( CORPSE_DECAY, PLAYERCORPSEDECAYMULTIPLIER );
	SystemTimer( LOOT_DECAY, 0 );
	SystemTimer( BEGGING_T, 3 );
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

void CServerData::CombatExplodeDelay( R32 value )
{
	combatExplodeDelay = value;
}
R32 CServerData::CombatExplodeDelay( void ) const
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

void CServerData::RepMurderDecay( UI16 value )
{
	murderdecaytimer = value;
}
UI16 CServerData::RepMurderDecay( void ) const
{
	return murderdecaytimer;
}

void CServerData::RepMaxKills( UI16 value )
{
	maxmurdersallowed = value;
}
UI16 CServerData::RepMaxKills( void ) const
{
	return maxmurdersallowed;
}

void CServerData::RepCrimTime( UI16 value )
{
	criminaldecaytimer = value;
}
UI16 CServerData::RepCrimTime( void ) const
{
	return criminaldecaytimer;
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
		ofsOutput << "}" << std::endl;
		
		ofsOutput << std::endl << "[timers]" << std::endl << "{" << std::endl;
		ofsOutput << "CORPSEDECAYTIMER=" << SystemTimer( CORPSE_DECAY ) << std::endl;
		ofsOutput << "LOOTDECAYTIMER=" << SystemTimer( LOOT_DECAY ) << std::endl;
		ofsOutput << "WEATHERTIMER=" << SystemTimer( WEATHER ) << std::endl;
		ofsOutput << "SHOPSPAWNTIMER=" << SystemTimer( SHOP_SPAWN ) << std::endl;
		ofsOutput << "DECAYTIMER=" << SystemTimer( DECAY ) << std::endl;
		ofsOutput << "INVISIBILITYTIMER=" << SystemTimer( INVISIBILITY ) << std::endl;
		ofsOutput << "OBJECTUSETIMER=" << SystemTimer( OBJECT_USAGE ) << std::endl;
		ofsOutput << "GATETIMER=" << SystemTimer( GATE ) << std::endl;
		ofsOutput << "POISONTIMER=" << SystemTimer( POISON ) << std::endl;
		ofsOutput << "LOGINTIMEOUT=" << SystemTimer( LOGIN_TIMEOUT ) << std::endl;
		ofsOutput << "HITPOINTREGENTIMER=" << SystemTimer( HITPOINT_REGEN ) << std::endl;
		ofsOutput << "STAMINAREGENTIMER=" << SystemTimer( STAMINA_REGEN ) << std::endl;
		ofsOutput << "MANAREGENTIMER=" << SystemTimer( MANA_REGEN ) << std::endl;
		ofsOutput << "ARMORAFFECTREGENTIMER=" << SystemTimer( ARMORAFFECTMANA_REGEN ) << std::endl;
		ofsOutput << "BASEFISHINGTIMER=" << SystemTimer( BASE_FISHING ) << std::endl;
		ofsOutput << "RANDOMFISHINGTIMER=" << SystemTimer( RANDOM_FISHING ) << std::endl;
		ofsOutput << "SPIRITSPEAKTIMER=" << SystemTimer( SPIRIT_SPEAK ) << std::endl;
		ofsOutput << "BEGGINGTIMER=" << SystemTimer( BEGGING_T ) << std::endl;
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
		ofsOutput << "POLYDURATION=" << PolyDuration() << std::endl;
		ofsOutput << "CLIENTSUPPORT=" << ServerClientSupport() << std::endl;
		ofsOutput << "}" << std::endl;

		ofsOutput << std::endl << "[speedup]" << std::endl << "{" << std::endl;
		ofsOutput << "SPEEDCHECKITEMS=" << CheckItemsSpeed() << std::endl;
		ofsOutput << "SPEEDCHECKBOATS=" << CheckBoatSpeed() << std::endl;
		ofsOutput << "SPEEDCHECKNPCAI=" << CheckNpcAISpeed() << std::endl;
		ofsOutput << "SPEEDCHECKSPAWNREGIONS=" << CheckSpawnRegionSpeed() << std::endl;
		ofsOutput << "}" << std::endl;
		
		ofsOutput << std::endl << "[message boards]" << std::endl << "{" << std::endl;
		ofsOutput << "MSGBOARDPOSTINGLEVEL=" << MsgBoardPostingLevel() << std::endl;
		ofsOutput << "MSGBOARDREMOVALLEVEL=" << MsgBoardPostRemovalLevel() << std::endl;
		ofsOutput << "}" << std::endl;

		ofsOutput << std::endl << "[escorts]" << std::endl << "{" << std::endl;
		ofsOutput << "ESCORTENABLED=" << ( EscortsEnabled() ? 1 : 0 ) << std::endl;
		ofsOutput << "ESCORTINITEXPIRE=" << EscortInitExpire() << std::endl;
		ofsOutput << "ESCORTACTIVEEXPIRE=" << EscortActiveExpire() << std::endl;
		ofsOutput << "ESCORTDONEEXPIRE=" << EscortDoneExpire() << std::endl;
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
		
		ofsOutput << std::endl << "[begging]" << std::endl << "{" << std::endl;
		ofsOutput << "BEGGINGRANGE=" << BeggingRange() << std::endl;
		ofsOutput << "}" << std::endl;
		
		ofsOutput << std::endl << "[tracking]" << std::endl << "{" << std::endl;
		ofsOutput << "TRACKINGBASERANGE=" << TrackingBaseRange() << std::endl;
		ofsOutput << "TRACKINGBASETIMER=" << TrackingBaseTimer() << std::endl;
		ofsOutput << "TRACKINGMAXTARGETS=" << static_cast<UI16>(TrackingMaxTargets()) << std::endl;
		ofsOutput << "TRACKINGMSGREDISPLAYTIME=" << TrackingRedisplayTime() << std::endl;
		ofsOutput << "}" << std::endl;
		
		ofsOutput << std::endl << "[reputation]" << std::endl << "{" << std::endl;
		ofsOutput << "REPSYSMURDERDECAYTIMER=" << RepMurderDecay() << std::endl;
		ofsOutput << "REPSYSMAXKILLS=" << RepMaxKills() << std::endl;
		ofsOutput << "REPSYSCRIMINALTIMER=" << RepCrimTime() << std::endl;
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
		ofsOutput << "HUNGERRATE=" << HungerRate() << std::endl;
		ofsOutput << "HUNGERDAMAGERATETIMER=" << HungerDamageRateTimer() << std::endl;
		ofsOutput << "HUNGERDMGVAL=" << HungerDamage() << std::endl;
		ofsOutput << "HUNGERTHRESHOLD=" << HungerThreshold() << std::endl;
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
		ofsOutput << "STARTPRIVS1=" << ServerStartPrivs() << std::endl;
		ofsOutput << "STARTPRIVS2=" << 0 << std::endl;
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

void CServerData::BeggingRange( SI16 value )
{
	beggingrange = value;
}
SI16 CServerData::BeggingRange( void ) const
{
	return beggingrange;
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
						case 0x000C:	// SERVERNAME
							//ServerName( value );
							break;
						case 0x0017:	// CONSOLELOG
							ServerConsoleLog( value.toUByte() );
							break;
						case 0x0022:	// CRASHPROTECTION
							ServerCrashProtection( value.toUByte() );
							break;
						case 0x0032:	// COMMANDPREFIX
							ServerCommandPrefix( (value.data()[0]) );	// return the first character of the return string only
							break;
						case 0x0040:	// ANNOUNCEWORLDSAVES
							ServerAnnounceSaves( (value.toUShort()==1?true:false) ); 
							break;
						case 0x0053:	// JOINPARTMSGS
							ServerJoinPartAnnouncements( (value.toUShort()==1?true:false) );
							break;
						case 0x0060:	// MULCACHING
							ServerMulCaching( (value.toUShort()==1?true:false) );
							break;
						case 0x006B:	// BACKUPSENABLED
							ServerBackups( (value.toUShort()>0?true:false) );
							break;
						case 0x007A:	// SAVESTIMER
							ServerSavesTimer( value.toULong() );
							break;
						case 0x0085:	// SKILLCAP
							ServerSkillCap( value.toUShort() );
							break;
						case 0x008E:	// SKILLDELAY
							ServerSkillDelay( value.toUByte() );
							break;
						case 0x0099:	// STATCAP
							ServerStatCap( value.toUShort() );
							break;
						case 0x00A1:	// MAXSTEALTHMOVEMENTS
							MaxStealthMovement( value.toShort() );
							break;
						case 0x00B5:	// MAXSTAMINAMOVEMENTS
							MaxStaminaMovement( value.toShort() );
							break;
						case 0x00C9:	// CORPSEDECAYTIMER
							SystemTimer( CORPSE_DECAY, value.toUShort() );
							break;
						case 0x00DA:	// LOOTDECAYTIMER
							SystemTimer( LOOT_DECAY, value.toUShort() );
							break;
						case 0x00E9:	// WEATHERTIMER
							SystemTimer( WEATHER, value.toUShort() );
							break;
						case 0x00F6:	// SHOWSPAWNTIMER
							SystemTimer( SHOP_SPAWN, value.toUShort() );
							break;
						case 0x00CF:	// DECAYTIMER
							SystemTimer( DECAY, value.toUShort() );
							break;
						case 0x0110:	// INVISIBILITYTIMER
							SystemTimer( INVISIBILITY, value.toUShort() );
							break;
						case 0x0122:	// OBJECTUSETIMER
							SystemTimer( OBJECT_USAGE, value.toUShort() );
							break;
						case 0x0131:	// GATETIMER
							SystemTimer( GATE, value.toUShort() );
							break;
						case 0x013B:	// POISONTIMER
							SystemTimer( POISON, value.toUShort() );
							break;
						case 0x0147:	// LOGINTIMEOUT
							SystemTimer( LOGIN_TIMEOUT, value.toUShort() );
							break;
						case 0x0154:	// HITPOINTREGENTIMER
							SystemTimer( HITPOINT_REGEN, value.toUShort() );
							break;
						case 0x0167:	// STAMINAREGENTIMER
							SystemTimer( STAMINA_REGEN, value.toUShort() );
							break;
						case 0x0179:	// MANAREGENTIMER
							SystemTimer( MANA_REGEN, value.toUShort() );
							break;
						case 0x0188:	// ARMORAFFECTREGENTIMER
							SystemTimer( ARMORAFFECTMANA_REGEN, value.toUShort() );
							break;
						case 0x019E:	// BASEFISHINGTIMER
							SystemTimer( BASE_FISHING, value.toUShort() );
							break;
						case 0x01AF:	// RANDOMFISHINGTIMER
							SystemTimer( RANDOM_FISHING, value.toUShort() );
							break;
						case 0x01C2:	// SPIRITSPEAKTIMER
							SystemTimer( SPIRIT_SPEAK, value.toUShort() );
							break;
						case 0x01D3:	// BEGGINGTIMER
							SystemTimer( BEGGING_T, value.toUShort() );
							break;
						case 0x01E0:	// DIRECTORY
							Directory( CSDDP_ROOT, value );
							break;
						case 0x01EA:	// DATADIRECTORY
							Directory( CSDDP_DATA, value );
							break;
						case 0x01F8:	// DEFSDIRECTORY
							Directory( CSDDP_DEFS, value );
							break;
						case 0x0206:	// ACTSDIRECTORY
							Directory( CSDDP_ACCOUNTS, value );
							break;
						case 0x0214:	// SCRIPTSDIRECTORY
							Directory( CSDDP_SCRIPTS, value );
							break;
						case 0x0225:	// BACKUPDIRECTORY
							Directory( CSDDP_BACKUP, value );
							break;
						case 0x0235:	// MSGBOARDDIRECTORY
							Directory( CSDDP_MSGBOARD, value );
							break;
						case 0x0247:	// SHAREDDIRECTORY
							Directory( CSDDP_SHARED, value );
							break;
						case 0x0257:	// LOOTDECAYSWITHCORPSE
							CorpseLootDecay( value.toUShort() != 0 );
							break;
						case 0x026C:	// GUARDSACTIVE
							GuardStatus( value.toUShort() != 0 );
							break;
						case 0x0279:	// DEATHANIMATION
							DeathAnimationStatus( value.toUShort() != 0 );
							break;
						case 0x0288:	// AMBIENTSOUNDS
							WorldAmbientSounds( value.toShort() );
							break;
						case 0x0296:	// AMBIENTFOOTSTEPS
							AmbientFootsteps( value.toUShort() != 0 );
							break;
						case 0x02A7:	// INTERNALACCOUNTCREATION
							InternalAccountStatus( value.toUShort() != 0 );
							break;
						case 0x02BF:	// SHOWHIDDENNPCS
							ShowHiddenNpcStatus( value.toUShort() != 0 );
							break;
						case 0x02CE:	// ROGUESENABLED
							RogueStatus( value.toUShort() != 0 );
							break;
						case 0x02DC:	// PLAYERPERSECUTION
							PlayerPersecutionStatus( value.toUShort() != 0 );
							break;
						case 0x02EE:	// ACCOUNTFLUSH
							AccountFlushTimer( value.toDouble() );
							break;
						case 0x02FB:	// HTMLSTATUSENABLED
							HtmlStatsStatus( value.toShort() );
							break;
						case 0x030D:	// SELLBYNAME
							SellByNameStatus( value.toUShort() == 1 );
							break;
						case 0x0318:	// SELLMAXITEMS
							SellMaxItemsStatus( value.toShort() );
							break;
						case 0x0325:	// TRADESYSTEM
							TradeSystemStatus( value.toUShort() != 0 );
							break;
						case 0x0331:	// RANKSYSTEM
							RankSystemStatus( value.toUShort() != 0 );
							break;
						case 0x033C:	// CUTSCROLLREQUIREMENTS
							CutScrollRequirementStatus( value.toShort() );
							break;
						case 0x0352:	// SPEEDCHECKITEMS
							CheckItemsSpeed( value.toDouble() );
							break;
						case 0x0362:	// SPEEDCHECKBOATS
							CheckBoatSpeed( value.toDouble() );
							break;
						case 0x0372:	// SPEEDCHECKNPCAI
							CheckNpcAISpeed( value.toDouble() );
							break;
						case 0x0382:	// SPEEDCHECKSPAWNREGIONS
							CheckSpawnRegionSpeed( value.toDouble() );
							break;
						case 0x0399:	// MSGBOARDPOSTINGLEVEL
							MsgBoardPostingLevel( value.toShort() );
							break;
						case 0x03AE:	// MSGBOARDREMOVALLEVEL
							MsgBoardPostRemovalLevel( value.toShort() );
							break;
						case 0x03C3:	// ESCOURTENABLED
							EscortsEnabled( value.toUShort() == 1 );
							break;
						case 0x03D1:	// ESCOURTINITEXPIRE
							EscortInitExpire( value.toUShort() );
							break;
						case 0x03E2:	// ESCOURTACTIVEEXPIRE
							EscortActiveExpire( value.toUShort() );
							break;
						case 0x03F5:	// LIGHTMOON1
							ServerMoon( 0, value.toShort() );
							break;
						case 0x0400:	// LIGHTMOON2
							ServerMoon( 1, value.toShort() );
							break;
						case 0x040B:	// LIGHTDUNGEONLEVEL
							DungeonLightLevel( (LIGHTLEVEL)value.toUShort() );
							break;
						case 0x041D:	// LIGHTCURRENTLEVEL
							WorldLightCurrentLevel( (LIGHTLEVEL)value.toUShort() );
							break;
						case 0x042F:	// LIGHTBRIGHTLEVEL
							WorldLightBrightLevel( (LIGHTLEVEL)value.toUShort() );
							break;
						case 0x0440:	// BEGGINGRANGE
							BeggingRange( value.toShort() );
							break;
						case 0x044D:	 // TRACKINGBASERANGE[0091]
							TrackingBaseRange( value.toUShort() );
							break;
						case 0x045F:	 // TRACKINGBASETIMER[0092]
							TrackingBaseTimer( value.toUShort() );
							break;
						case 0x0471:	 // TRACKINGMAXTARGETS[0093]
							TrackingMaxTargets( value.toUByte() );
							break;
						case 0x0484:	 // TRACKINGMSGREDISPLAYTIME[0094]
							TrackingRedisplayTime( value.toUShort() );
							break;
						case 0x049D:	 // REPSYSMURDERDECAYTIMER[0095]
							RepMurderDecay( value.toUShort() );
							break;
						case 0x04B4:	 // REPSYSMAXKILLS[0096]
							RepMaxKills( value.toUShort() );
							break;
						case 0x04C3:	 // REPSYSCRIMINALTIMER[0097]
							RepCrimTime( value.toUShort() );
							break;
						case 0x04D7:	 // RESOURCEMINECHECK[0098]
							MineCheck( value.toUByte() );
							break;
						case 0x04E9:	 // RESOURCEOREPERAREA[0099]
							ResOre( value.toShort() );
							break;
						case 0x04FC:	 // RESOURCEORERESPAWNTIMER[0100]
							ResOreTime( value.toUShort() );
							break;
						case 0x0514:	 // RESOURCEORERESPAWNAREA[0101]
							ResOreArea( value.toUShort() );
							break;
						case 0x052B:	 // RESOURCELOGSPERAREA[0102]
							ResLogs( value.toShort() );
							break;
						case 0x053F:	 // RESPURCELOGSRESPAWNTIMER[0103]
							ResLogTime( value.toUShort() );
							break;
						case 0x0558:	 // RESOURCELOGSRESPAWNAREA[0104]
							ResLogArea( value.toUShort() );
							break;
						case 0x0570:	 // HUNGERRATE[0105]
							HungerRate( value.toUShort() );
							break;
						case 0x057B:	 // HUNGERDAMAGERATETIMER[0106]
							HungerDamageRateTimer( value.toUShort() );
							break;
						case 0x0591:	 // HUNGERDMGVAL[0107]
							HungerDamage( value.toShort() );
							break;
						case 0x059E:	 // HUNGERTHRESHOLD[0108]
							HungerThreshold( value.toShort() );
							break;
						case 0x05AE:	 // COMBATMAXRANGE[0109]
							CombatMaxRange( value.toShort() );
							break;
						case 0x05BD:	 // COMBATSPELLMAXRANGE[0111]
							CombatMaxSpellRange( value.toShort() );
							break;
						case 0x05D1:	 // COMBATDISPLAYHITMSG[0116]
							CombatDisplayHitMessage( value.toUShort() == 1 );
							break;
						case 0x05E5:	 // COMBATMONSTERSVSANIMALS[0119]
							CombatMonstersVsAnimals( value.toUShort() == 1 );
							break;
						case 0x05FD:	 // COMBATANIMALATTACKCHANCE[0120]
							CombatAnimalsAttackChance( value.toShort() );
							break;
						case 0x0616:	 // COMBATANIMALSGUARDED[0121]
							CombatAnimalsGuarded( value.toUShort() == 1 );
							break;
						case 0x062B:	 // COMBATNPCDAMAGERATE[0122]
							CombatNPCDamageRate( value.toShort() );
							break;
						case 0x063F:	 // COMBATNPCBASEFLEEAT[0123]
							CombatNPCBaseFleeAt( value.toShort() );
							break;
						case 0x0653:	 // COMBATNPCBASEREATTACKAT[0124]
							CombatNPCBaseReattackAt( value.toShort() );
							break;
						case 0x066B:	 // COMBATATTACKSTAMINA[0125]
							CombatAttackStamina( value.toShort() );
							break;
						case 0x067F:	 // LOCATION[0126]
							ServerLocation( value );
							break;
						case 0x0688:	 // STARTGOLD[0127]
							ServerStartGold( value.toShort() );
							break;
						case 0x0692:	 // STARTPRIVS1[0128]
							ServerStartPrivs( value.toUShort() );
							break;
						case 0x069E:	 // STARTPRIVS2[0129]
							break;
						case 0x06AA:	 // ESCORTDONEEXPIRE[0130]
							EscortDoneExpire( value.toUShort() );
							break;
						case 0x06BB:	 // LIGHTDARKLEVEL[0131]
							WorldLightDarkLevel( (LIGHTLEVEL)value.toUShort() );
							break;
						case 0x06CA:	 // TITLECOLOUR[0132]
							TitleColour( value.toUShort() );
							break;
						case 0x06D6:	 // LEFTTEXTCOLOUR[0133]
							LeftTextColour( value.toUShort() );
							break;
						case 0x06E5:	 // RIGHTTEXTCOLOUR[0134]
							RightTextColour( value.toUShort() );
							break;
						case 0x06F5:	 // BUTTONCANCEL[0135]
							ButtonCancel( value.toUShort() );
							break;
						case 0x0702:	 // BUTTONLEFT[0136]
							ButtonLeft( value.toUShort() );
							break;
						case 0x070D:	 // BUTTONRIGHT[0137]
							ButtonRight( value.toUShort() );
							break;
						case 0x0719:	 // BACKGROUNDPIC[0138]
							BackgroundPic( value.toUShort() );
							break;
						case 0x0727:	 // POLLTIME[0139]
							TownNumSecsPollOpen( value.toULong() );
							break;
						case 0x0730:	 // MAYORTIME[0140]
							TownNumSecsAsMayor( value.toULong() );
							break;
						case 0x073A:	 // TAXPERIOD[0141]
							TownTaxPeriod( value.toULong() );
							break;
						case 0x0744:	 // GUARDSPAID[0142]
							TownGuardPayment( value.toULong() );
							break;
						case 0x074F:	 // DAY[0143]
							ServerTimeDay( value.toShort() );
							break;
						case 0x0753:	 // HOURS[0144]
							ServerTimeHours( value.toUByte() );
							break;
						case 0x0759:	 // MINUTES[0145]
							ServerTimeMinutes( value.toUByte() );
							break;
						case 0x0761:	 // SECONDS[0146]
							ServerTimeSeconds( value.toUByte() );
							break;
						case 0x0769:	 // AMPM[0147]
							ServerTimeAMPM( value.toUShort() != 0 );
							break;
						case 0x076E:	 // SKILLLEVEL[0148]
							SkillLevel( value.toUByte() );
							break;
						case 0x0779:	 // SNOOPISCRIME[0149]
							SnoopIsCrime( value.toUShort() != 0 );
							break;
						case 0x0786:	 // BOOKSDIRECTORY[0151]
							Directory( CSDDP_BOOKS, value );
							break;
						case 0x0795:	 // SERVERLIST[0152]
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
						case 0x07A5:	 // ACCESSDIRECTORY[0128]
							Directory( CSDDP_ACCESS, value );
							break;
						case 0x07B5:	 // LOGSDIRECTORY[0129]
							Directory( CSDDP_LOGS, value );
							break;
						case 0x07C3:	 // ACCOUNTISOLATION[0130]
							break;
						case 0x07D4:	 // HTMLDIRECTORY[0131]
							Directory( CSDDP_HTML, value );
							break;
						case 0x07E2:	 // SHOOTONANIMALBACK[0132]
							ShootOnAnimalBack( value.toUShort() == 1 );
							break;
						case 0x07F4:	 // NPCTRAININGENABLED[0133]
							NPCTrainingStatus( value.toUShort() == 1 );
							break;
						case 0x0807:	 // DICTIONARYDIRECTORY[0134]
							Directory( CSDDP_DICTIONARIES, value );
							break;
						case 0x081B:	 // BACKUPSAVERATIO[0135]
							BackupRatio( value.toShort() );
							break;
						case 0x082B:	 // HIDEWILEMOUNTED[0136]
							CharHideWhileMounted( value.toShort() == 1 );
							break;
						case 0x083B:	 // SECONDSPERUOMINUTE[0137]
							ServerSecondsPerUOMinute( value.toUShort() );
							break;
						case 0x084E:	// WEIGHTPERSTR[0138]
							WeightPerStr( value.toUByte() );
							break;
						case 0x085B:	// POLYDURATION[0139]
							PolyDuration( value.toUShort() );
							break;
						case 0x0868:	 // UOGENABLED[0140] 
							ServerUOGEnabled( value.toShort()==1 );
							break;
						case 0x0873:	 // NETRCVTIMEOUT[0141]
							ServerNetRcvTimeout( value.toULong() );
							break;
						case 0x0881:	 // NETSNDTIMEOUT[0142]
							ServerNetSndTimeout( value.toULong() );
							break;
						case 0x088F:	 // NETRETRYCOUNT[0143]
							ServerNetRetryCount( value.toULong() );
							break;
						case 0x089D:	 // USEFACETSAVES[0144]
							ServerUseFacetSaves( value.toUShort() == 1);
							break;
						case 0x08AB:	 // MAP0[0145] Will set MAP0 specific flags, and data eventually 
							// Place holder
							break;
						case 0x08B0:	 // MAP1[0146]
							// Place holder
							break;
						case 0x08B5:	 // MAP2[0147]
							// Place holder
							break;
						case 0x08BA:	 // MAP3[0148]
							// Place holder
							break;
						case 0x08BF:	 // USERMAP[0149]
							// Place holder
							break;
						case 0x08C7:	 // CLIENTSUPPORT[0150]
							ServerClientSupport( value.toULong() );
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
