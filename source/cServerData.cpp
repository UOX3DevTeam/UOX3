#include "cServerData.h"

#ifdef __LINUX__
	#include <sys/types.h>
	#include <dirent.h>
#else
	#include <direct.h>
#endif

using namespace std;

// Legacy lookup for the server.scp file	(January 30, 2001 - EviLDeD)
// NOTE:	Very important the first lookups required duplication or the search fails on them
const string SERVERCFG_LOOKUP("|CRASH_PROTECTION|CRASH_PROTECTION|SKILLCAP|STATCAP|USESPECIALBANK|SPECIALBANKTRIGGER|"
	"DECAYTIMER|PLAYERCORPSEDECAYMULTIPLIER|LOOTDECAYSWITHCORPSE|INVISTIMER|SKILLDELAY|SKILLLEVEL|OBJECTDELAY|GATETIMER|SHOWDEATHANIM|GUARDSACTIVE|BG_SOUNDS|ANNOUNCE_WORLDSAVES|WWWACCOUNTS|ARCHIVEPATH|SAVE_PER_LOOP|"
	"BACKUP_SAVE_RATIO|UOXBOT|POISONTIMER|JOINMSG|PARTMSG|LOG|ROGUE|WEATHERTIME|SHOPTIME|SHOWLOGGEDOUTPCS|CHAR_TIME_OUT|MAXSTEALTHSTEPS|RUNNINGSTAMINASTEPS|BOAT_SPEED|HTML|CUT_SCROLL_REQUIREMENTS|PERSECUTION|"
	"AUTO_CREATE_ACCTS|MSGBOARDPATH|MSGPOSTACCESS|MSGPOSTREMOVE|MSGRETENTION|ESCORTACTIVE|ESCORTINITEXPIRE|ESCORTACTIVEEXPIRE|ESCORTDONEEXPIRE|FOOTSTEPS|COMMANDPREFIX|PORT|NICE|LORDB_LAG_FIX|CHECK_ITEMS|"
	"CHECK_NPCS|NPC_TILE_CHECK|CHECK_NPCAI|CHECK_SPAWNREGIONS|CHECKMEM|CACHE_MULS|ACCOUNTFLUSH|MAXRANGE|WRESTLESPEED|MAX_RANGE_SPELL|MAXDMG|DEATH_ON_THROAT|DIST_TO_POISON|COMBAT_HIT_MESSAGE|MAX_ABSORBTION|"
	"MAX_NON_HUMAN_ABSORBTION|MONSTERS_VS_ANIMALS|ANIMALS_ATTACK_CHANCE|ANIMALS_GUARDED|NPC_DAMAGE_RATE|NPC_BASE_FLEEAT|NPC_BASE_REATTACKAT|ATTACKSTAMINA|SELLBYNAME|SELLMAXITEM|TRADESYSTEM|RANKSYSTEM|"
	"HITPOINTS_REGENRATE|STAMINA_REGENRATE|MANA_REGENRATE|ARMOR_AFFECT_MANA_REGEN|HUNGERRATE|HUNGER_DAMAGE|HUNGER_DAMAGE_RATE|MINECHECK|ORE_PER_AREA|ORE_RESPAWN_TIME|ORE_RESPAWN_AREA|LOGS_PER_AREA|LOG_RESPAWN_TIME|"
	"LOG_RESPAWN_AREA|MURDER_DECAY|MAXKILLS|CRIMINAL_TIME|BASE_TRACKING_RANGE|MAX_TRACKING_TARGETS|BASE_TRACKING_TIME|TRACKING_MESSAGE_REDISPLAY_TIME|BEGGING_RANGE|BASE_FISHING_TIME|"
	"RANDOM_FISHING_TIME|SECTION SPIRITSPEAK|SPIRITSPEAKTIMER|DAY|HOUR|MINUTE|AMPM|MOON1|MOON2|MOON1UPDATE|MOON2UPDATE|DUNGEONLIGHTLEVEL|WORLDFIXEDLEVEL|WORLDCURLEVEL|WORLDBRIGHTLEVEL|WORLDDARKLEVEL|SECONDSPERUOMINUTE|"
	"TITLECOLOUR|LEFTTEXTCOLOUR|RIGHTTEXTCOLOUR|BUTTONCANCEL|BUTTONLEFT|BUTTONRIGHT|BACKGROUNDPIC|POLLTIME|MAYORTIME|TAXPERIOD|GUARDSPAID|SNOOPISCRIME|ENGRAVEENABLED"
);

// New uox3.ini format lookup	
// (January 13, 2001 - EviLDeD) Modified: January 30, 2001 Converted to uppercase
// (February 26 2002 - EviLDeD) Modified: to support the AccountIsolation, and left out dir3ectory tags
// NOTE:	Very important the first lookups required duplication or the search fails on them
const string UOX3INI_LOOKUP("|SERVERNAME|SERVERNAME|CONSOLELOG|CRASHPROTECTION|COMMANDPREFIX|ANNOUNCEWORLDSAVES|JOINPARTMSGS|MULCACHING|BACKUPSENABLED|SAVESPERLOOP|SAVESTIMER|MAINTHREADSLEEP|" 
	"SKILLCAP|SKILLDELAY|STATCAP|STATDELAY|MAXSTEALTHMOVEMENTS|MAXSTAMINAMOVEMENTS|CORPSEDECAYTIMER|LOOTDECAYTIMER|WEATHERTIMER|SHOPSPAWNTIMER|DECAYTIMER|INVISIBILITYTIMER|"
	"OBJECTUSETIMER||GATETIMER|POISONTIMER|LOGINTIMEOUT|HITPOINTREGENTIMER|STAMINAREGENTIMER|MANAREGENTIMER|ARMORAFFECTREGENTIMER|BASEFISHINGTIMER|RANDOMFISHINGTIMER|SPIRITSPEAKTIMER|"
	"BEGGINGTIMER|DIRECTORY|DATADIRECTORY|DEFSDIRECTORY|ACTSDIRECTORY|SCRIPTSDIRECTORY|BACKUPDIRECTORY|MSGBOARDDIRECTORY|SHAREDDIRECTORY|LOOTDECAYSWITHCORPSE|GUARDSACTIVE|DEATHANIMATION|"
	"AMBIENTSOUNDS|AMBIENTFOOTSTEPS|EXTERNALACCOUNTCREATION|INTERNALACCOUNTCREATION|SHOWHIDDENNPCS|ROGUESENABLED|UOXBOTENABLED|PLAYERPERSECUTION|LAGFIX|ACCOUNTFLUSH|HTMLSTATUSENABLED|"
	"SELLBYNAME|SELLMAXITEMS|TRADESYSTEM|RANKSYSTEM|NPCTILECHECK|CUTSCROLLREQUIREMENTS|WILDERNESSBANKENABLED|WILDERNESSBANKKEYWORD|HEARTBEAT|SPEEDCHECKITEMS|SPEEDCHECKNPCS|SPEEDCHECKBOATS|SPEEDCHECKNPCAI|"
	"SPEEDCHECKSPAWNREGIONS|SPEEDCHECKMEMORY|MSGBOARDKEEPMESSAGES|MSGBOARDADMINLEVEL|MSGBOARDPOSTINGLEVEL|MSGBOARDREMOVALLEVEL|MSGBOARDMAXMESSAGESALLOWED|MSGBOARDMAXMSGSRESPONSE|ESCORTENABLED|ESCORTINITEXPIRE|ESCORTACTIVEEXPIRE|LIGHTMOON1|LIGHTMOON1UPDATE|LIGHTMOON2|"
	"LIGHTMOON2UPDATE|LIGHTDUNGEONLEVEL|LIGHTFIXEDLEVEL|LIGHTCURRENTLEVEL|LIGHTBRIGHTLEVEL|BEGGINGRANGE|TRACKINGBASERANGE|TRACKINGBASETIMER|TRACKINGMAXTARGETS|TRACKINGMSGREDISPLAYTIME|"
	"REPSYSMURDERDECAYTIMER|REPSYSMAXKILLS|REPSYSCRIMINALTIMER|RESOURCEMINECHECK|RESOURCEOREPERAREA|RESOURCEORERESPAWNTIMER|RESOURCEORERESPAWNAREA|RESOURCELOGSPERAREA|RESPURCELOGSRESPAWNTIMER|RESOURCELOGSRESPAWNAREA|HUNGERRATE|HUNGERDAMAGERATETIMER|HUNGERDMGVAL|HUNGERTHRESHOLD|"
	"COMBATMAXRANGE|COMBATWRESTLESPEED|COMBATSPELLMAXRANGE|COMBATMAXMELEEDAMAGE|COMBATMAXSPELLDAMAGE|COMBATALLOWCRITICALS|COMBATMAXPOISONINGDISTANCE|COMBATDISPLAYHITMSG|COMBATMAXHUMANABSORBTION|COMBATMAXNONHUMANABSORBTION|COMBATMONSTERSVSANIMALS|"
	"COMBATANIMALATTACKCHANCE|COMBATANIMALSGUARDED|COMBATNPCDAMAGERATE|COMBATNPCBASEFLEEAT|COMBATNPCBASEREATTACKAT|COMBATATTACKSTAMINA|LOCATION|STARTGOLD|STARTPRIVS1|STARTPRIVS2|ESCORTDONEEXPIRE|LIGHTDARKLEVEL|"
	"TITLECOLOUR|LEFTTEXTCOLOUR|RIGHTTEXTCOLOUR|BUTTONCANCEL|BUTTONLEFT|BUTTONRIGHT|BACKGROUNDPIC|POLLTIME|MAYORTIME|TAXPERIOD|GUARDSPAID|DAY|HOURS|MINUTES|SECONDS|AMPM|SKILLLEVEL|SNOOPISCRIME|ENGRAVEENABLED|BOOKSDIRECTORY|SERVERLIST|SCRIPTSECTIONHEADER|PORT|SAVEMODE|"
	"ACCESSDIRECTORY|LOGSDIRECTORY|ACCOUNTISOLATION|HTMLDIRECTORY|SHOOTONANIMALBACK|NPCTRAININGENABLED|GUMPSDIRECTORY|DICTIONARYDIRECTORY"
);

void cServerData::SetServerName( const char *setname )
{
	if( !setname )
	{
		serverList[0].name = "Default UOX Server";
		return;
	}
	serverList[0].name.erase();
	serverList[0].name = setname;
}

const char *cServerData::GetServerName( void )
{
	return serverList[0].name.c_str();
}

void cServerData::SetServerDomain( const char *setdomain )
{
	if( !setdomain )
	{
		serverList[0].domain = "";
		return;
	}
	serverList[0].domain.erase();
	serverList[0].domain = setdomain;
}

const char *cServerData::GetServerDomain( void )
{
	return serverList[0].domain.c_str();
}

void cServerData::SetServerIP( const char *setip )
{
	if(!setip)
	{
		serverList[0].ip="127.0.0.1";
		return;
	}
	serverList[0].ip.erase();
	serverList[0].ip=setip;
}

const char *cServerData::GetServerIP( void )
{
	return serverList[0].ip.c_str();
}

void cServerData::SetServerPort( SI16 setport )
{
	if( (unsigned) setport < 1 )
	{
		port = 2593;
		return;
	}
	port = setport;
}

SI16 cServerData::GetServerPort( void )
{
	return port;
}

void cServerData::SetServerConsoleLog( UI32 setting )
{
	consolelogenabled = setting;
}

UI32 cServerData::GetServerConsoleLogStatus( void )
{
	return consolelogenabled;
}

void cServerData::SetServerCrashProtection( UI32 setting )
{
	crashprotectionenabled = (setting > 5 && !(setting<1) ) ? 1 : setting;
}

UI32 cServerData::GetServerCrashProtectionStatus( void )
{
	return crashprotectionenabled;
}

void cServerData::SetServerCommandPrefix( char cmdvalue )
{
	commandprefix = cmdvalue;
}

char cServerData::GetServerCommandPrefix( void )
{
	return commandprefix;
}

void cServerData::SetServerAnnounceSaves( bool setting )
{
	announcesaves = setting;
}

bool cServerData::GetServerAnnounceSavesStatus( void )
{
	return announcesaves;
}

void cServerData::SetServerJoinPartAnnouncements( bool setting )
{
	joinpartmsgsenabled = setting;
}

bool cServerData::GetServerJoinPartAnnouncementsStatus( void )
{
	return joinpartmsgsenabled;
}

void cServerData::SetServerMulCaching( bool setting )
{
	mulcachingenabled = setting;
}

bool cServerData::GetServerMulCachingStatus( void )
{
	return mulcachingenabled;
}

void cServerData::SetServerBackups( bool setting )
{
	backupsenabled = setting;
}

bool cServerData::GetServerBackupStatus( void )
{
	return backupsenabled;
}

void cServerData::SetServerAntiLagSavesPerLoop( SI16 value )
{
	if( value < 0 )
	{
		antilagsavesperloop = 0;
		return;
	}
	antilagsavesperloop = value;
}

SI16 cServerData::GetServerAntiLagSavesPerLoop( void )
{
	return antilagsavesperloop;
}

void cServerData::SetServerSavesTimer( UI32 timer )
{
	if( timer < 180 )						// 3 minutes is the lowest value you can set saves for
	{
		serversavestimer = 300;	// 10 minutes default
		return;
	}
	serversavestimer = timer;
}

UI32 cServerData::GetServerSavesTimerStatus( void )
{
	return serversavestimer;
}

void cServerData::SetServerMainThreadTimer( UI32 threadtimer )
{
	if( threadtimer < 1000 )		// Default is on second loop sleeping
	{
		mainthreadsleep = 500;
		return;
	}
	mainthreadsleep = threadtimer;
}

UI32 cServerData::GetServerMainThreadTimerStatus( void )
{
	return mainthreadsleep;
}

void cServerData::SetServerSkillCap( UI32 cap )
{
	if( cap < 1 )		// Default is on second loop sleeping
	{
		skillcap = 7000;
		return;
	}
	skillcap = cap;
}

UI32 cServerData::GetServerSkillCapStatus( void )
{
	return skillcap;
}

void cServerData::SetServerSkillDelay( UI32 skdelay )
{
	if( skdelay < 1 )		// Default is on second loop sleeping
	{
		skilldelay = 5;
		return;
	}
	skilldelay = skdelay;
}

UI32 cServerData::GetServerSkillDelayStatus( void )
{
	return skilldelay;
}

void cServerData::SetServerStatCap( UI32 cap )
{
	if( cap < 1 )		// Default is on second loop sleeping
	{
		statcap = 300;
		return;
	}
	statcap = cap;
}

UI32 cServerData::GetServerStatCapStatus( void )
{
	return statcap;
}

void cServerData::SetServerStatDelay( UI32 stdelay )
{
	if( stdelay < 1 )		// Default is on second loop sleeping
	{
		statdelay = 5;
		return;
	}
	statdelay = stdelay;
}

UI32 cServerData::GetServerStatDelayStatus( void )
{
	return statdelay;
}

void cServerData::SetMaxStealthMovement( SI16 value )
{
	maxstealthmovement = value;
}

SI16 cServerData::GetMaxStealthMovement( void )
{
	return maxstealthmovement;
}

void cServerData::SetMaxStaminaMovement( SI16 value )
{
	maxstaminamovement = value;
}

SI16 cServerData::GetMaxStaminaMovement( void )
{
	return maxstaminamovement;
}

void cServerData::SetSkillAdvanceModifier( UI32 value )
{
	skilladvancemodifier = value;
}

UI32 cServerData::GetSkillAdvanceModifier( void )
{
	return skilladvancemodifier;
}

void cServerData::SetStatAdvanceModifier( UI32 value )
{
	statadvancemodifier = value;
}

UI32 cServerData::GetStatAdvanceModifier( void )
{
	return statadvancemodifier;
}

void cServerData::SetSystemTimer( TID timerid, UI32 value )
{
	switch( timerid )
	{
		case CORPSE_DECAY:			corpsedecaytimer = value;				break;
		case LOOT_DECAY:			lootdecaytimer = value;					break;
		case WEATHER:				weathertimer = value;					break;
		case SHOP_SPAWN:			shopspawntimer = value;					break;
		case DECAY:					decaytimer = value;						break;
		case INVISIBILITY:			invisibilitytimer = value;				break;
		case OBJECT_USAGE:			objectusagetimer = value;				break;
		case GATE:					gatetimer = value;						break;
		case POISON:				poisontimer = value;					break;
		case LOGIN_TIMEOUT:			logintimeout = value;					break;
		case HITPOINT_REGEN:		hitpointregentimer = value;				break;
		case STAMINA_REGEN:			staminaregentimer = value;				break;
		case MANA_REGEN:			manaregentimer = value;					break;
		case ARMORAFFECTMANA_REGEN:	armoraffectregentimer = (SI16)value;	break;
		case BASE_FISHING:			fishingbasetimer = value;				break;
		case RANDOM_FISHING:		fishingrandomtimer = value;				break;
		case SPIRIT_SPEAK:			spiritspeaktimer = value;				break;
		case BEGGING_T:				beggingtimer = value;					break;
		default:															break;
	}
}

SI32 cServerData::GetSystemTimerStatus( TID timerid )
{
	switch( timerid )
	{
		case CORPSE_DECAY:			return corpsedecaytimer;
		case LOOT_DECAY:			return lootdecaytimer;
		case WEATHER:				return weathertimer;
		case SHOP_SPAWN:			return shopspawntimer;
		case DECAY:					return decaytimer;
		case INVISIBILITY:			return invisibilitytimer;
		case OBJECT_USAGE:			return objectusagetimer;
		case GATE:					return gatetimer;
		case POISON:				return poisontimer;
		case LOGIN_TIMEOUT:			return logintimeout;
		case HITPOINT_REGEN:		return hitpointregentimer;
		case STAMINA_REGEN:			return staminaregentimer;
		case MANA_REGEN:			return manaregentimer;
		case ARMORAFFECTMANA_REGEN:	return armoraffectregentimer;
		case BASE_FISHING:			return fishingbasetimer;
		case RANDOM_FISHING:		return fishingrandomtimer;
		case SPIRIT_SPEAK:			return spiritspeaktimer;
		case BEGGING_T:				return beggingtimer;
		default:					break;
	}
	return TIMER_ERROR;
}

void cServerData::setDirectoryHelper( string dirName, string &dir, char *text )
{
        // First, let's normalize the path name and fix common errors
#ifdef __LINUX__
        const char dirSep = '/';
#else
	const char dirSep = '\\';
#endif
        if (!text)
	{
		Console.Error( 1, " %s directory is blank, set in uox.ini", dirName.c_str() );
		Shutdown( FATAL_UOX3_DIR_NOT_FOUND );
		return;
        }

	// remove all trailing spaces...
	int length = strlen(text);
	while (text[length - 1] == ' ')
	        text[--length] = '\0';

	// Make sure we're terminated with a directory separator
	// Just incase it's not set in the .ini

	bool addSep = text[length - 1] != dirSep;

	bool error = false;
	if( !resettingDefaults )
	{
#ifdef __LINUX__
		DIR *dirPtr = opendir( text );
		if( !dirPtr )
			error = true;
		else
			closedir( dirPtr );
#else
		char curWorkingDir[1024];
		GetCurrentDirectory( 1024, curWorkingDir );
		int iResult = _chdir( text );
		if( iResult != 0 )
			error = true;
		else
			_chdir( curWorkingDir );	// move back to where we were
#endif
	}

	if (error)
	{
	        Console.Error( 1, " %s directory %s does not exist", dirName.c_str(), text );
			Shutdown( FATAL_UOX3_DIR_NOT_FOUND );
	}
	else
	{
	        if( !text )
		        dir = "./";
		else
		{
		        dir = text;
			if (addSep)
			  dir += dirSep;
		}
	}
}

void cServerData::SetRootDirectory( char *text )
{
	setDirectoryHelper( "Root directory", rootdirectory, text );
}

const char *cServerData::GetRootDirectory( void )
{
	return rootdirectory.c_str();
}

const char *cServerData::GetHTMLDirectory( void )
{
	return htmldirectory.c_str();
}

void cServerData::SetHTMLDirectory( char *text )
{
	setDirectoryHelper( "HTML directory", htmldirectory, text );
}


const char *cServerData::GetDataDirectory( void )
{
	return datadirectory.c_str();
}

void cServerData::SetDataDirectory( char *text )
{
	setDirectoryHelper( "Data directory", datadirectory, text );
}

const char *cServerData::GetDefsDirectory( void )
{
	return defsdirectory.c_str();
}

void cServerData::SetDefsDirectory( char *text )
{
	setDirectoryHelper( "DFNs directory", defsdirectory, text );
}

const char *cServerData::GetBooksDirectory( void )
{
	return booksdirectory.c_str();
}

void cServerData::SetBooksDirectory( char *text )
{
	setDirectoryHelper( "Books directory", booksdirectory, text );
}

const char *cServerData::GetScriptsDirectory( void )
{
	return scriptsdirectory.c_str();
}

void cServerData::SetScriptsDirectory( char *text )
{
	setDirectoryHelper( "Scripts directory", scriptsdirectory, text );
}

const char *cServerData::GetBackupDirectory( void )
{
	return backupdirectory.c_str();
}

void cServerData::SetBackupDirectory( char *text )
{
	setDirectoryHelper( "Backup directory", backupdirectory, text );
}

bool cServerData::GetShootOnAnimalBack( void )
{
	return shootonanimalback;
}

void cServerData::SetShootOnAnimalBack( bool setting )
{
	shootonanimalback = setting;
}

bool cServerData::GetNPCTrainingStatus( void )
{
	return npctraining;
}

void cServerData::SetNPCTrainingStatus( bool setting )
{
	npctraining = setting;
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	const char *cServerData::GetAccessDirectory( void )
//|	Date					-	02/07/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 Development Team
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	This path specified where to find the fast look up file
//|									for shard access. This is a fast patch for those that
//|									rely on external account cratioon web scripts. Some
//|									problems can be found accessing outside a webservers
//|									local hdd space which is invalid running web scripts.
//|									This path ensures that UOX3 will look elsewhere for that
//|									account lookup file. Meaning that you can keep it in
//|									context for your webtools over your network topography.
//|									
//|	Modification	-	02/21/2002	-	Need to put the lowercase stuff here instead
//o--------------------------------------------------------------------------o
//|	Returns				- [const char*] Pointing to a buffer that contains the
//|									path to the Access.adm file.
//o--------------------------------------------------------------------------o
const char *cServerData::GetAccessDirectory( void )
{
	return accessdirectory.c_str();
}
void cServerData::SetAccessDirectory( char *text )
{
	setDirectoryHelper( "Access directory", accessdirectory, text );
}
//
const char *cServerData::GetAccountsDirectory( void )
{
	return accountsdirectory.c_str();
}

void cServerData::SetAccountsDirectory( char *text )
{
	setDirectoryHelper( "Accounts directory", accountsdirectory, text );
}

const char *cServerData::GetMsgBoardDirectory( void )
{
	return msgboarddirectory.c_str();
}

void cServerData::SetMsgBoardDirectory( char *text)
{
	setDirectoryHelper( "Msgboard directory", msgboarddirectory, text );
}

const char *cServerData::GetSharedDirectory( void )
{
	return shareddirectory.c_str();
}

void cServerData::SetSharedDirectory( char *text )
{
	setDirectoryHelper( "Shared directory", shareddirectory, text );
}


//o--------------------------------------------------------------------------o
//|	Function/Class-	const char *cServerData::GetLogsDirectory( void )
//|	Date					-	02/26/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//|	Returns				-	[const char*] Pointing to the Log path data
//o--------------------------------------------------------------------------o
const char *cServerData::GetLogsDirectory( void )
{
	return logsdirectory.c_str();
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	void cServerData::SetLogsDirectory( char *text )
//|	Date					-	02/26/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//|	Returns				-	N/A
//o--------------------------------------------------------------------------o
void cServerData::SetLogsDirectory( char *text )
{
	setDirectoryHelper( "Logs directory", logsdirectory, text );
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	const char *cServerData::GetGumpsDirectory( void )
//|	Date					-	02/26/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//|	Returns				-	[const char*] Pointing to the Log path data
//o--------------------------------------------------------------------------o
const char *cServerData::GetGumpsDirectory( void )
{
	return gumpsdirectory.c_str();
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	const char *cServerData::GetDictionaryDirectory( void )
//|	Date					-	04/07/2002
//|	Developer(s)	-	duckhead
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//|	Returns				-	[const char*] Pointing to the Log path data
//o--------------------------------------------------------------------------o
const char *cServerData::GetDictionaryDirectory( void )
{
	return dictionarydirectory.c_str();
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	void cServerData::SetGumpsDirectory( char *text )
//|	Date					-	03/15/2002
//|	Developer(s)	-	duckhead
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	Holds the location of the gumps
//o--------------------------------------------------------------------------o
//|	Returns				-	N/A
//o--------------------------------------------------------------------------o
void cServerData::SetGumpsDirectory( char *text )
{
	setDirectoryHelper( "Gumps directory", gumpsdirectory, text );
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	void cServerData::SetDictionaryDirectory( char *text )
//|	Date					-	04/07/2002
//|	Developer(s)	-	duckhead
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	Holds the location of the gumps
//o--------------------------------------------------------------------------o
//|	Returns				-	N/A
//o--------------------------------------------------------------------------o
void cServerData::SetDictionaryDirectory( char *text )
{
	setDirectoryHelper( "Dictionary directory", dictionarydirectory, text );
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	void cServerData::DumpPaths( void )
//|	Date					-	02/26/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	Needed a function that would dump out the paths. If you
//|									add any paths to the server please make sure that you
//|									place exports to the console here to please so it can
//|									belooked up if needed. 
//o--------------------------------------------------------------------------o
//|	Returns				-	N/A
//o--------------------------------------------------------------------------o
void cServerData::DumpPaths( void )
{
	Console.PrintSectionBegin();
	Console << "| PathDump: \n";
	Console << "|     Root      : " << GetRootDirectory() << "\n";
	Console << "|     Accounts  : " << GetAccountsDirectory() << "\n";
	Console << "|     Access    : " << GetAccessDirectory() << "\n";
	Console << "|     Mul(Data) : " << GetDataDirectory() << "\n";
	Console << "|     DFN(Defs) : " << GetDefsDirectory() << "\n";
	Console << "|     JScript   : " << GetScriptsDirectory() << "\n";
	Console << "|     HTML      : " << GetHTMLDirectory() << "\n";
	Console << "|     MSGBoards : " << GetMsgBoardDirectory() << "\n";
	Console << "|     Books     : " << GetBooksDirectory() << "\n";
	Console << "|     Shared    : " << GetSharedDirectory() << "\n";
	Console << "|     Backups   : " << GetBackupDirectory() << "\n";
	Console << "|     Logs      : " << GetLogsDirectory() << "\n";
	Console.PrintSectionBegin();
}

void cServerData::SetCorpseLootDecay( bool value )
{
	lootdecayswithcorpse = value;
}

bool cServerData::GetCorpseLootDecay( void )
{
	return lootdecayswithcorpse;
}

void cServerData::SetGuardStatus( bool value )
{
	guardsenabled = value;
}

bool cServerData::GetGuardsStatus( void )
{
	return guardsenabled;
}

void cServerData::SetDeathAnimationStatus( bool value )
{
	deathanimationenabled = value;
}

bool cServerData::GetDeathAnimationStatus( void )
{
	return deathanimationenabled;
}

void cServerData::SetWorldAmbientSounds( SI16 value )
{
	if( value < 0 )
	{
		ambientsounds = 0;
		return;
	}
	ambientsounds = value;
}

SI16 cServerData::GetWorldAmbientSounds( void )
{
	return ambientsounds;
}

void cServerData::SetAmbientFootsteps( bool value )
{
	ambientfootsteps = value;
}

bool cServerData::GetAmbientFootsteps( void )
{
	return ambientfootsteps;
}

void cServerData::SetExternalAccountStatus( bool value )
{
	externalaccountsenabled = value;
}

bool cServerData::GetExternalAccountStatus( void )
{
	return externalaccountsenabled;
}

void cServerData::SetInternalAccountStatus( bool value )
{
	internalaccountsenabled = value;
}

bool cServerData::GetInternalAccountStatus( void )
{
	return internalaccountsenabled;
}

void cServerData::SetShowHiddenNpcStatus( bool value )
{
	showhiddennpcsenabled = value;
}

bool cServerData::GetShowHiddenNpcStatus( void )
{
	return showhiddennpcsenabled;
}

void cServerData::SetRogueStatus( bool value )
{
	roguesenabled = value;
}

bool cServerData::GetRogueStatus( void )
{
	return roguesenabled;
}

void cServerData::SetSnoopIsCrime( bool value )
{
	snoopiscrime = value;
}
bool cServerData::GetSnoopIsCrime( void )
{
	return snoopiscrime;
}

void cServerData::SetEngraveStatus( bool value )
{
	engraveenabled = value;
}

bool cServerData::GetEngraveStatus( void )
{
	return engraveenabled;
}

void cServerData::SetUoxBotStatus( bool value )
{
	uoxbotenabled = value;
}

bool cServerData::GetUoxBotStatus( void )
{
	return uoxbotenabled;
}

void cServerData::SetPlayerPersecutionStatus( bool value )
{
	playerpersecution = value;
}

bool cServerData::GetPlayerPersecutionStatus( void )
{
	return playerpersecution;
}

void cServerData::SetLagFixStatus( bool value )
{
	lagfixenabled = value;
}

bool cServerData::GetLagFixStatus( void )
{
	return lagfixenabled;
}

void cServerData::SetHtmlStatsStatus( SI16 value )
{
	htmlstatusenabled = value;
}

SI16 cServerData::GetHtmlStatsStatus( void )
{
	return htmlstatusenabled;
}

void cServerData::SetSellByNameStatus( bool value )
{
	sellbynameenabled = value;
}

bool cServerData::GetSellByNameStatus( void )
{
	return sellbynameenabled;
}

void cServerData::SetSellMaxItemsStatus( SI16 value )
{
	sellmaxitems = value;
}

SI16 cServerData::GetSellMaxItemsStatus( void )
{
	return sellmaxitems;
}

void cServerData::SetTradeSystemStatus( bool value )
{
	tradesystemenabled = value;
}

bool cServerData::GetTradeSystemStatus( void )
{
	return tradesystemenabled;
}

void cServerData::SetRankSystemStatus( bool value )
{
	ranksystemenabled = value;
}

bool cServerData::GetRankSystemStatus( void )
{
	return ranksystemenabled;
}

SI16 cServerData::GetNpcTileCheckStatus( void )
{
	return npctilecheck;
}

void cServerData::SetNpcTileCheckStatus( SI16 value )
{
	npctilecheck = value;
}

void cServerData::SetCutScrollRequirementStatus( SI16 value )
{
	cutscrollrequirement = value;
}

SI16 cServerData::GetCutScrollRequirementStatus( void )
{
	return cutscrollrequirement;
}

void cServerData::SetWildernessBankStatus( bool value )
{
	wildernessbankenabled = value;
}

bool cServerData::GetWildernessBankStatus( void )
{
	return wildernessbankenabled;
}

void cServerData::SetWildernessBankTriggerString( const char *value )
{
	wildernessbanktrigger = value;
}

const char *cServerData::GetWildernessBankTriggerString( void )
{
	return wildernessbanktrigger.c_str();
}

void cServerData::SetSystemHeartBeatStatus( bool value )
{
	heartbeat = value;
}

bool cServerData::GetSystemHeartBeatStatus( void )
{
	return heartbeat;
}

void cServerData::SetCheckItemsSpeed( R64 value )
{
	if( value < 0.0 )
	{
		checkitems = 0.0;
		return;
	}
	checkitems = value;
}

R64 cServerData::GetCheckItemsSpeed( void )
{
	return checkitems;
}

void cServerData::SetCheckNpcSpeed( R64 value )
{
	if( value < 0.0 )
	{
		checknpcs = 0.0;
		return;
	}
	checknpcs = value;
}

R64 cServerData::GetCheckNpcSpeed( void )
{
	return checknpcs;
}

void cServerData::SetCheckBoatSpeed( R64 value )
{
	if( value < 0.0 )
	{
		checkboats = 0.0;
		return;
	}
	checkboats = value;
}

R64 cServerData::GetCheckBoatSpeed( void )
{
	return checkboats;
}

void cServerData::SetCheckNpcAISpeed( R64 value )
{
	if( value < 0.0 )
	{
		checknpcai = 0.0;
		return;
	}
	checknpcai = value;
}

R64 cServerData::GetCheckNpcAISpeed( void )
{
	return checknpcai;
}

void cServerData::SetCheckSpawnRegionSpeed( R64 value )
{
	if( value < 0.0 )
	{
		checkspawnregions = 0.0;
		return;
	}
	checkspawnregions = value;
}

R64 cServerData::GetCheckSpawnRegionSpeed( void )
{
	return checkspawnregions;
}

void cServerData::SetCheckMemorySpeed( R64 value )
{
	if( value < 0.0 )
	{
		checkmemory = 0.0;
		return;
	}
	checkmemory=value;
}

R64 cServerData::GetCheckMemorySpeed( void )
{
	return checkmemory;
}

void cServerData::SetMsgBoardAdminLevel( SI16 value )
{
	msgadminlevel = value;
}

SI16 cServerData::GetMsgBoardAdminLevel( void )
{
	return msgadminlevel;
}

void cServerData::SetMsgBoardPostingLevel( SI16 value )
{
	msgpostinglevel = value;
}

SI16 cServerData::GetMsgBoardPostingLevel( void )
{
	return msgpostinglevel;
}

void cServerData::SetMsgBoardPostRemovalLevel( SI16 value )
{
	msgremovallevel = value;
}

SI16 cServerData::GetMsgBoardPostRemovalLevel( void )
{
	return msgremovallevel;
}

void cServerData::SetMsgBoardMaxMessageCount( SI16 value )
{
	maxmessagesallowed = value;
}

SI16 cServerData::GetMsgBoardMaxMessageCount( void )
{
	return maxmessagesallowed;
}

void cServerData::SetMsgBoardMaxMessageseaction( SI16 value )
{
	maxmessagesreaction = value;
}

SI16 cServerData::GetMsgBoardMaxMessagesReaction( void )
{
	return maxmessagesreaction;
}

void cServerData::SetMineCheck( SI16 value )
{
	minecheck = value;
}

SI16 cServerData::GetMineCheck( void )
{
	return minecheck;
}

void cServerData::SetCombatDisplayHitMessage( bool value )
{
	combatdisplayhitmessage = value;
}

bool cServerData::GetCombatDisplayHitMessage( void )
{
	return combatdisplayhitmessage;
}

void cServerData::SetCombatMaxHumanAbsorbtion( SI16 value )
{
	combatmaxhumanabsorbtion = value;
}

SI16 cServerData::GetCombatMaxHumanAbsorbtion( void )
{
	return combatmaxhumanabsorbtion;
}

void cServerData::SetCombatMaxNonHumanAbsorbtion( SI16 value )
{
	combatmaxnonhumanabsorbtion = value;
}

SI16 cServerData::GetCombatMaxNonHumanAbsorbtion( void )
{
	return combatmaxnonhumanabsorbtion;
}

void cServerData::SetCombatNPCDamageRate( SI16 value )
{
	combatnpcdamagerate = value;
}

SI16 cServerData::GetCombatNPCDamageRate( void )
{
	return combatnpcdamagerate;
}

void cServerData::SetCombatAttackStamina( SI16 value )
{
	combatattackstamina = value;
}

SI16 cServerData::GetCombatAttackStamina( void )
{
	return combatattackstamina;
}

UI32 cServerData::GetSkillLevel( void )
{
	return skilllevel;
}
void cServerData::SetSkillLevel( UI32 value )
{
	skilllevel = value;
}

void cServerData::SetEscortsEnabled( bool value )
{
	escortsenabled = value;
}
bool cServerData::GetEscortsEnabled( void )
{
	return escortsenabled;
}

void cServerData::SetEscortInitExpire( UI32 value )
{
	escortinitexpire = value;
}
UI32 cServerData::GetEscortInitExpire( void )
{
	return escortinitexpire;
}

void cServerData::SetEscortActiveExpire( UI32 value )
{
	escortactiveexpire = value;
}
UI32 cServerData::GetEscortActiveExpire( void )
{
	return escortactiveexpire;
}

void cServerData::SetEscortDoneExpire( UI32 value )
{
	escortdoneexpire = value;
}
UI32 cServerData::GetEscortDoneExpire( void )
{
	return escortdoneexpire;
}

void cServerData::SetCombatMonstersVsAnimals( bool value )
{
	combatmonstersvsanimals = value;
}
bool cServerData::GetCombatMonstersVsAnimals( void )
{
	return combatmonstersvsanimals;
}

void cServerData::SetCombatAnimalsAttackChance( SI16 value )
{
	combatanimalattackchance = value;
}
SI16 cServerData::GetCombatAnimalsAttackChance( void )
{
	return combatanimalattackchance;
}

void cServerData::SetServerArmorAffectManaRegen( SI16 value )
{
	armoraffectregentimer = value;
}
SI16 cServerData::GetServerArmorAffectManaRegen( void )
{
	return armoraffectregentimer;
}

void cServerData::SetHungerRate( UI32 value )
{
	hungerrate = value;
}
UI32 cServerData::GetHungerRate( void )
{
	return hungerrate;
}

void cServerData::SetHungerDamageRateTimer( UI32 value )
{
	hungerdamageratetimer = value;
}
UI32 cServerData::GetHungerDamageRateTimer( void )
{
	return hungerdamageratetimer;
}

void cServerData::SetHungerDamage( SI16 value )
{
	hungerdamage = value;
}
SI16 cServerData::GetHungerDamage( void )
{
	return hungerdamage;
}

void cServerData::SetHungerThreshold( UI32 value )
{
	hungerthreshold = value;
}
UI32 cServerData::GetHungerThreshold( void )
{
	return hungerthreshold;
}

void cServerData::SetPotionDelay( SI16 value )
{
	potiondelay = value;
}
SI16 cServerData::GetPotionDelay( void )
{
	return potiondelay;
}

void cServerData::SetBuyThreshold( SI16 value )
{
	buyThreshold = value;
}
SI16 cServerData::GetBuyThreshold( void )
{
	return buyThreshold;
}

void cServerData::SetBackupRatio( SI16 value )
{
	backupRatio = value;
}
SI16 cServerData::GetBackupRatio( void )
{
	return backupRatio;
}

void cServerData::ResetDefaults( void )
{
	resettingDefaults = true;
	port = 2593;
	saveMode = 0;
	serverList.resize( 1 );		// Set the initial count to hold one server. 

	SetPotionDelay( POTIONDELAY );
	SetServerMoon( 0, 0 );
	SetServerMoon( 1, 0 );
	SetServerMoonUpdate( 0, 0 );
	SetServerMoonUpdate( 1, 0 );
	SetDungeonLightLevel( 3 );
	SetWorldFixedLightLevel( 0 );
	SetWorldLightCurrentLevel( 0 );
	SetWorldLightBrightLevel( 0 );
	SetWorldLightDarkLevel( 5 );

	SetServerTimeDay( 0 );
	SetServerTimeHours( 0 );
	SetServerTimeMinutes( 0 );
	SetServerTimeSeconds( 0 );
	SetServerTimeAMPM( 0 );

	SetServerCrashProtection( 1 );
	SetInternalAccountStatus( false );
	SetCombatWrestlingSpeed( 8 );
	SetCombatMaxMeleeDamage( 120 );
	SetCombatMaxSpellDamage( 120 );
	SetCombatMaxPoisoningDistance( 2 );
	SetCombatMaxRange( 10 );
	SetCombatMaxSpellRange( 10 );
	SetCombatCriticalsEnabled( true );
	SetCombatExplodeDelay( 0 );
	SetWildernessBankTriggerString( SPECIALBANKTRIGGER );
	SetWildernessBankStatus( false );
	
	// load defaults values
	SetSystemTimer( SHOP_SPAWN, 60 );
	SetSystemTimer( POISON, POISONTIMER );
	SetPotionDelay( POTIONDELAY );
	SetSystemTimer( DECAY, DECAYTIMER );
	SetServerSkillCap( SKILLCAP );
	SetServerStatCap( STATCAP );
	SetSystemTimer( PLAYER_CORPSE, 3 );
	SetCorpseLootDecay( true );
	SetServerAntiLagSavesPerLoop( 0 );
	
	SetSystemTimer( INVISIBILITY, INVISTIMER );
	SetHungerRate( HUNGERRATE );
	SetHungerDamageRateTimer( HUNGERDAMAGERATE );
	SetHungerDamage( HUNGERDAMAGE );
	SetHungerThreshold( 3 );
	
	SetServerSkillDelay( SKILLDELAY );
	SetSystemTimer( OBJECT_USAGE, OBJECTDELAY );
	SetSystemTimer( HITPOINT_REGEN, REGENRATE1 );
	SetSystemTimer( STAMINA_REGEN, REGENRATE2 );
	SetSystemTimer( MANA_REGEN, REGENRATE3 );
	SetSystemTimer( ARMORAFFECTMANA_REGEN, 0 );
	SetSystemTimer( GATE, GATETIMER );
	SetMineCheck( 2 );
	SetDeathAnimationStatus( true );
	SetShowHiddenNpcStatus( true );
	SetCombatDisplayHitMessage( true );
	SetCombatAttackStamina( ATTACKSTAMINA );
	SetNPCTrainingStatus( true );

	SetCombatMonstersVsAnimals( true );
	SetCombatAnimalsAttackChance( ANIMALS_ATTACK_CHANCE );
	SetCombatAnimalsGuarded( false );
	SetCombatNPCBaseFleeAt( NPC_BASE_FLEEAT );
	SetCombatNPCBaseReattackAt( NPC_BASE_REATTACKAT );
	SetCombatMaxHumanAbsorbtion( MAX_ABSORBTION );
	SetCombatMaxNonHumanAbsorbtion( MAX_NON_HUMAN_ABSORBTION );
	SetShootOnAnimalBack( false );
	SetSellByNameStatus( false );
	SetSkillLevel( SKILLLEVEL );
	SetSellMaxItemsStatus( SELLMAXITEM );
	SetCombatNPCDamageRate( NPC_DAMAGE_RATE );
	SetRankSystemStatus( true );

#ifndef __LINUX
	char curWorkingDir[1024], tempStuff[1024];
	GetCurrentDirectory( 1024, curWorkingDir );
	SetRootDirectory( curWorkingDir );
	sprintf( tempStuff, "%s/muldata/", curWorkingDir );
	SetDataDirectory( tempStuff );
	sprintf( tempStuff, "%s/dfndata/", curWorkingDir );
	SetDefsDirectory( tempStuff );
	sprintf( tempStuff, "%s/accounts/", curWorkingDir );
	SetAccountsDirectory( tempStuff );
	SetAccessDirectory( tempStuff );
	sprintf( tempStuff, "%s/scripts/", curWorkingDir );
	SetScriptsDirectory( tempStuff );
	sprintf( tempStuff, "%s/archives/", curWorkingDir );
	SetBackupDirectory( tempStuff );
	sprintf( tempStuff, "%s/msgboards/", curWorkingDir );
	SetMsgBoardDirectory( tempStuff );
	sprintf( tempStuff, "%s/shared/", curWorkingDir );
	SetSharedDirectory( tempStuff );
	sprintf( tempStuff, "%s/html/", curWorkingDir );
	SetHTMLDirectory( tempStuff );
	sprintf( tempStuff, "%s/books/", curWorkingDir );
	SetBooksDirectory( tempStuff );
	sprintf( tempStuff, "%s/gumps/", curWorkingDir );
	SetGumpsDirectory( tempStuff );
#else
	SetRootDirectory( "./" );
	SetDataDirectory( "./muldata/" );
	SetDefsDirectory( "./dfndata/" );
	SetAccountsDirectory( "./accounts/" );
	SetAccessDirectory( "./accounts/" );
	SetScriptsDirectory( "./scripts/" );
	SetBackupDirectory( "./archives/" );
	SetMsgBoardDirectory( "./msgboards/" );
	SetSharedDirectory( "./shared/" );
	SetHTMLDirectory( "./html/" );
	SetBooksDirectory( "./books/" );
	SetGumpsDirectory( "./gumps/" );
#endif

	SetBuyThreshold( 2000 );
	SetGuardStatus( true );
	SetServerAnnounceSaves( true );
	SetExternalAccountStatus( false );
	SetWorldAmbientSounds( 5 );
	SetServerJoinPartAnnouncements( true );
	SetServerConsoleLog( 1 );
	SetRogueStatus( true );
	SetEngraveStatus( true );
	SetSystemTimer( WEATHER, 60 );
	SetSystemTimer( LOGIN_TIMEOUT, 300 );
	SetBackupRatio( 5 );
	SetUoxBotStatus( false );
	SetMaxStealthMovement( 10 );
	SetMaxStaminaMovement( 15 );
	SetSnoopIsCrime( false );
	
	SetCheckBoatSpeed( 0.75 );
	SetCheckNpcAISpeed( 0.5 );
	SetCutScrollRequirementStatus( true );
	SetPlayerPersecutionStatus( true );
	SetHtmlStatsStatus( -1 );
	
	SetMsgBoardPostingLevel( 0 );
	SetMsgBoardPostRemovalLevel( 0 );
	// No replacement I can see
	SetMsgBoardKeepMsgs( 30 );           // Dupois - Added Dec 20, 1999 for message boards (30 Days)
	SetEscortsEnabled( true );
	SetEscortInitExpire( 86400 );
	SetEscortActiveExpire( 1800 );
	SetEscortDoneExpire( 1800 );
	SetAmbientFootsteps( false );
	SetServerCommandPrefix( '\'' );
	SetStatAdvanceModifier( 20 );
	
	SetLagFixStatus( false );
	SetCheckSpawnRegionSpeed( 30 );
	SetCheckItemsSpeed( 1.5 );
	SetCheckMemorySpeed( 30 );
	SetCheckNpcSpeed( 2 );
	SetNPCSpeed( 1.0 );
	SetNiceness( 2 );
	SetNpcTileCheckStatus( 24 );
	SetAccountFlushTimer( 0.0 );
	SetNPCSpeed( NPCSPEED );
	
	SetResLogs( 3 );
	SetResLogTime( 600 );
	SetResLogArea( 10 );
	SetResOre( 10 );
	SetResOreTime( 600 );
	SetResOreArea( 10 );
	//REPSYS
	SetRepCrimTime( 120 );
	SetRepMaxKills( 4 );
	SetRepMurderDecay( 60 );
	//RepSys ---^
	SetTrackingBaseRange( TRACKINGRANGE );
	SetTrackingMaxTargets( MAXTRACKINGTARGETS );
	SetTrackingBaseTimer( TRACKINGTIMER );
	SetTrackingRedisplayTime( TRACKINGDISPLAYTIME );
	SetBeggingRange( BEGGINGRANGE );

	SetFishingBaseTime( FISHINGTIMEBASE );
	SetFishingRandomTime( FISHINGTIMER );
	SetSpiritSpeakTimer( SPIRITSPEAKTIMER );

	// Abaddon, March 21, 2000

	SetTitleColour( 0 );
	SetLeftTextColour( 0 );
	SetRightTextColour( 0 );
	SetButtonCancel( 4017 );
	SetButtonLeft( 4014 );
	SetButtonRight( 4005 );
	SetBackgroundPic( 2600 );

	SetTownNumSecsPollOpen( 3600 );	// one hour
	SetTownNumSecsAsMayor( 36000 );	// 10 hours as mayor
	SetTownTaxPeriod( 1800 );			// taxed every 30 minutes
	SetTownGuardPayment( 3600 );		// guards paid every hour

	SetServerStartGold( 1000 );
	SetServerStartPrivs( 0, 0 );
	SetServerStartPrivs( 1, 0 );
	SetServerStatDelay( 5 );
	SetSystemTimer( CORPSE_DECAY, PLAYERCORPSEDECAYMULTIPLIER );
	SetSystemTimer( LOOT_DECAY, 0 );
	SetSystemTimer( BEGGING_T, 3 );
	ServerScriptSectionHeader( false );	// set to false to do DFN style lookups
	resettingDefaults = false;
	PostLoadDefaults();	
}
cServerData::cServerData( void )
{
	ResetDefaults();
}
cServerData::~cServerData()
{
}

void cServerData::SetCombatWrestlingSpeed( SI16 value )
{
	combatwrestlingspeed = value;
}

SI16 cServerData::GetCombatWrestlingSpeed( void )
{
	return combatwrestlingspeed;
}

void cServerData::SetCombatMaxMeleeDamage( SI16 value )
{
	combatmaxmeleedamage = value;
}

SI16 cServerData::GetCombatMaxMeleeDamage( void )
{
	return combatmaxmeleedamage;
}

void cServerData::SetCombatMaxPoisoningDistance( SI16 value )
{
	combatmaxpoisoningdistance = value;
}
SI16 cServerData::GetCombatMaxPoisoningDistance( void )
{
	return combatmaxpoisoningdistance;
}

void cServerData::SetCombatMaxRange( SI16 value )
{
	combatmaxrange = value;
}
SI16 cServerData::GetCombatMaxRange( void )
{
	return combatmaxrange;
}

void cServerData::SetCombatMaxSpellRange( SI16 value )
{
	combatmaxspellrange = value;
}
SI16 cServerData::GetCombatMaxSpellRange( void )
{
	return combatmaxspellrange;
}

void cServerData::SetCombatCriticalsEnabled( bool value )
{
	combatcriticalsenabled = value;
}
bool cServerData::GetCombatCriticalsEnabled( void )
{
	return combatcriticalsenabled;
}

void cServerData::SetCombatExplodeDelay( R32 value )
{
	combatExplodeDelay = value;
}
R32 cServerData::GetCombatExplodeDelay( void )
{
	return combatExplodeDelay;
}

void cServerData::SetCombatAnimalsGuarded( bool value )
{
	combatanimalsguarded = value;
}
bool cServerData::GetCombatAnimalsGuarded( void )
{
	return combatanimalsguarded;
}

void cServerData::SetCombatNPCBaseFleeAt( SI16 value )
{
	combatnpcbasefleeat = value;
}
SI16 cServerData::GetCombatNPCBaseFleeAt( void )
{
	return combatnpcbasefleeat;
}

void cServerData::SetCombatNPCBaseReattackAt( SI16 value )
{
	combatnpcbasereattackat = value;
}
SI16 cServerData::GetCombatNPCBaseReattackAt( void )
{
	return combatnpcbasereattackat;
}

void cServerData::SetNPCSpeed( R64 value )
{
	npcSpeed = value;
}
R64 cServerData::GetNPCSpeed( void )
{
	return npcSpeed;
}

void cServerData::SetNiceness( SI16 value )
{
	niceness = value;
}
SI16 cServerData::GetNiceness( void )
{
	return niceness;
}

void cServerData::SetTitleColour( UI16 value )
{
	titleColour = value;
}
UI16 cServerData::GetTitleColour( void )
{
	return titleColour;
}

void cServerData::SetLeftTextColour( UI16 value )
{
	leftTextColour = value;
}
UI16 cServerData::GetLeftTextColour( void )
{
	return leftTextColour;
}

void cServerData::SetRightTextColour( UI16 value )
{
	rightTextColour = value;
}
UI16 cServerData::GetRightTextColour( void )
{
	return rightTextColour;
}

void cServerData::SetButtonCancel( UI16 value )
{
	buttonCancel = value;
}
UI16 cServerData::GetButtonCancel( void )
{
	return buttonCancel;
}

void cServerData::SetButtonLeft( UI16 value )
{
	buttonLeft = value;
}
UI16 cServerData::GetButtonLeft( void )
{
	return buttonLeft;
}

void cServerData::SetButtonRight( UI16 value )
{
	buttonRight = value;
}
UI16 cServerData::GetButtonRight( void )
{
	return buttonRight;
}

void cServerData::SetBackgroundPic( UI16 value )
{
	backgroundPic = value;
}
UI16 cServerData::GetBackgroundPic( void )
{
	return backgroundPic;
}

void cServerData::SetTownNumSecsPollOpen( long value )
{
	numSecsPollOpen = value;
}
long cServerData::GetTownNumSecsPollOpen( void )
{
	return numSecsPollOpen;
}

void cServerData::SetTownNumSecsAsMayor( long value )
{
	numSecsAsMayor = value;
}
long cServerData::GetTownNumSecsAsMayor( void )
{
	return numSecsAsMayor;
}

void cServerData::SetTownTaxPeriod( long value )
{
	taxPeriod = value;
}
long cServerData::GetTownTaxPeriod( void )
{
	return taxPeriod;
}

void cServerData::SetTownGuardPayment( long value )
{
	guardPayment = value;
}
long cServerData::GetTownGuardPayment( void )
{
	return guardPayment;
}

void cServerData::SetRepMurderDecay( long value )
{
	murderdecaytimer = value;
}
long cServerData::GetRepMurderDecay( void )
{
	return murderdecaytimer;
}

void cServerData::SetRepMaxKills( int value )
{
	maxmurdersallowed = value;
}
int cServerData::GetRepMaxKills( void )
{
	return maxmurdersallowed;
}

void cServerData::SetRepCrimTime( int value )
{
	criminaldecaytimer = value;
}
int cServerData::GetRepCrimTime( void )
{
	return criminaldecaytimer;
}

void cServerData::SetResLogs( UI32 value )
{
	logsperarea = value;
}
UI32 cServerData::GetResLogs( void )
{
	return logsperarea;
}

void cServerData::SetResLogTime( UI32 value )
{
	logsrespawntimer = value;
}
UI32 cServerData::GetResLogTime( void )
{
	return logsrespawntimer;
}

void cServerData::SetResLogArea( UI32 value )
{
	logsrespawnarea = value;
}
UI32 cServerData::GetResLogArea( void )
{
	return logsrespawnarea;
}

void cServerData::SetResOre( UI32 value )
{
	oreperarea = value;
}
UI32 cServerData::GetResOre( void )
{
	return oreperarea;
}

void cServerData::SetResOreTime( UI32 value )
{
	orerespawntimer = value;
}
UI32 cServerData::GetResOreTime( void )
{
	return orerespawntimer;
}

void cServerData::SetResOreArea( UI32 value )
{
	orerespawnarea = value;
}
UI32 cServerData::GetResOreArea( void )
{
	return orerespawnarea;
}

void cServerData::SetAccountFlushTimer( R64 value )
{
	flushTime = value;
}
R64 cServerData::GetAccountFlushTimer( void )
{
	return flushTime;
}


//o--------------------------------------------------------------------------o
//|	Function/Class-	bool cServerData::save( void )
//|	Date					-	02/21/2002
//|	Developer(s)	-	Unknown
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	Save the uox.ini out. This is the default save
//o--------------------------------------------------------------------------o
//|	Returns				- [TRUE] If successfull
//o--------------------------------------------------------------------------o
bool cServerData::save( void )
{
	string s = GetRootDirectory();
	s += "/uox.ini";
	return save( s.c_str() );
}


//o--------------------------------------------------------------------------o
//|	Function/Class-	bool cServerData::save( const char *filename )
//|	Date					-	Unknown
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
//o--------------------------------------------------------------------------o
//|	Returns				-
//o--------------------------------------------------------------------------o
bool cServerData::save( const char *filename )
{
	ofstream ofsOutput;
	ofsOutput.open( filename, ios::out );
	if( !ofsOutput.is_open() )
	{
		Console.Error( 1, "Unable to open file %s for writing", filename );
		return false;
	}
	ofsOutput << "// UOX Initialization File. V" << MAKEWORD( 2, 0 ) << endl << "//================================" << endl << endl;
	ofsOutput << "[system]" << endl;
	ofsOutput << "PORT=" << GetServerPort() << endl;
	ofsOutput << "SAVEMODE=" << ( SaveMode() == 1 ? "BINARY" : "ASCII" ) << endl;
	ofsOutput << "CONSOLELOG=" << GetServerConsoleLogStatus() << endl;
	ofsOutput << "CRASHPROTECTION=" << GetServerCrashProtectionStatus() << endl;
	ofsOutput << "COMMANDPREFIX=" << GetServerCommandPrefix() << endl;
	ofsOutput << "ANNOUNCEWORLDSAVES=" << (GetServerAnnounceSavesStatus()?1:0) << endl;
	ofsOutput << "JOINPARTMSGS=" << (GetServerJoinPartAnnouncementsStatus()?1:0) << endl;
	ofsOutput << "MULCACHING=" << (GetServerMulCachingStatus()?1:0) << endl;
	ofsOutput << "BACKUPSENABLED=" << GetBackupRatio() << endl;
	ofsOutput << "SAVESTIMER=" << GetServerSavesTimerStatus() << endl;
	ofsOutput << "MAINTHREADSLEEP=" << GetNiceness()  << endl;
	ofsOutput << "ACCOUNTISOLATION=" << "1" /*GetAccountsIsolationLevel()*/ << endl;
	ofsOutput << endl << "[play server list]" << endl;

	for( UI32 cnt = 0; cnt < serverList.size(); cnt++ )
		ofsOutput << "SERVERLIST=" << serverList[cnt].name << "," << serverList[cnt].ip << "," << serverList[cnt].port << endl;

	ofsOutput << endl << "[network server list]" << endl;

	ofsOutput << endl << "[skill & stats]" << endl;
	ofsOutput << "SKILLLEVEL=" << GetSkillLevel() << endl;
	ofsOutput << "SKILLCAP=" << GetServerSkillCapStatus() << endl;
	ofsOutput << "SKILLDELAY=" << GetServerSkillDelayStatus() << endl;
	ofsOutput << "STATCAP=" << GetServerStatCapStatus() << endl;
	ofsOutput << "STATDELAY=" << GetServerStatDelayStatus() << endl;
	ofsOutput << "MAXSTEALTHMOVEMENTS=" << GetMaxStealthMovement() << endl;
	ofsOutput << "MAXSTAMINAMOVEMENTS=" << GetMaxStaminaMovement() << endl;
	ofsOutput << "SNOOPISCRIME=" << (GetSnoopIsCrime()?1:0) << endl;
	
	ofsOutput << endl << "[timers]" << endl;
	ofsOutput << "CORPSEDECAYTIMER=" << GetSystemTimerStatus( CORPSE_DECAY ) << endl;
	ofsOutput << "LOOTDECAYTIMER=" << GetSystemTimerStatus( LOOT_DECAY ) << endl;
	ofsOutput << "WEATHERTIMER=" << GetSystemTimerStatus( WEATHER ) << endl;
	ofsOutput << "SHOPSPAWNTIMER=" << GetSystemTimerStatus( SHOP_SPAWN ) << endl;
	ofsOutput << "DECAYTIMER=" << GetSystemTimerStatus( DECAY ) << endl;
	ofsOutput << "INVISIBILITYTIMER=" << GetSystemTimerStatus( INVISIBILITY ) << endl;
	ofsOutput << "OBJECTUSETIMER=" << GetSystemTimerStatus( OBJECT_USAGE ) << endl;
	ofsOutput << "GATETIMER=" << GetSystemTimerStatus( GATE ) << endl;
	ofsOutput << "POISONTIMER=" << GetSystemTimerStatus( POISON ) << endl;
	ofsOutput << "LOGINTIMEOUT=" << GetSystemTimerStatus( LOGIN_TIMEOUT ) << endl;
	ofsOutput << "HITPOINTREGENTIMER=" << GetSystemTimerStatus( HITPOINT_REGEN ) << endl;
	ofsOutput << "STAMINAREGENTIMER=" << GetSystemTimerStatus( STAMINA_REGEN ) << endl;
	ofsOutput << "MANAREGENTIMER=" << GetSystemTimerStatus( MANA_REGEN ) << endl;
	ofsOutput << "ARMORAFFECTREGENTIMER=" << GetSystemTimerStatus( ARMORAFFECTMANA_REGEN ) << endl;
	ofsOutput << "BASEFISHINGTIMER=" << GetSystemTimerStatus( BASE_FISHING ) << endl;
	ofsOutput << "RANDOMFISHINGTIMER=" << GetSystemTimerStatus( RANDOM_FISHING ) << endl;
	ofsOutput << "SPIRITSPEAKTIMER=" << GetSystemTimerStatus( SPIRIT_SPEAK ) << endl;
	ofsOutput << "BEGGINGTIMER=" << GetSystemTimerStatus( BEGGING_T ) << endl;

	
	ofsOutput << endl << "[directories]" << endl;
	ofsOutput << "DIRECTORY=" << GetRootDirectory() << endl;
	ofsOutput << "DATADIRECTORY=" << GetDataDirectory() << endl;
	ofsOutput << "DEFSDIRECTORY=" << GetDefsDirectory() << endl;
	ofsOutput << "BOOKSDIRECTORY=" << GetBooksDirectory() << endl;
	ofsOutput << "ACTSDIRECTORY=" << GetAccountsDirectory() << endl;
	ofsOutput << "SCRIPTSDIRECTORY=" << GetScriptsDirectory() << endl;
	ofsOutput << "BACKUPDIRECTORY=" << GetBackupDirectory() << endl;
	ofsOutput << "MSGBOARDDIRECTORY=" << GetMsgBoardDirectory() << endl;
	ofsOutput << "SHAREDDIRECTORY=" << GetSharedDirectory() << endl;
	// EviLDeD: 022702: The might be some of the reason that paths are fucked.
	ofsOutput << "ACCESSDIRECTORY=" << GetAccessDirectory() << endl;
	ofsOutput << "HTMLDIRECTORY=" << GetHTMLDirectory() << endl;
	ofsOutput << "LOGSDIRECTORY=" << GetLogsDirectory() << endl;
	ofsOutput << "GUMPSDIRECTORY=" << GetGumpsDirectory() << endl;
	ofsOutput << "DICTIONARYDIRECTORY=" << GetDictionaryDirectory() << endl;
	
	ofsOutput << endl << "[settings]" << endl;
	ofsOutput << "SAVESPERLOOP=" << GetServerAntiLagSavesPerLoop() << endl;
	ofsOutput << "LOOTDECAYSWITHCORPSE=" << (GetCorpseLootDecay()?1:0) << endl;
	ofsOutput << "GUARDSACTIVE=" << (GetGuardsStatus()?1:0) << endl;
	ofsOutput << "DEATHANIMATION=" << (GetDeathAnimationStatus()?1:0) << endl;
	ofsOutput << "AMBIENTSOUNDS=" << GetWorldAmbientSounds() << endl;
	ofsOutput << "AMBIENTFOOTSTEPS=" << (GetAmbientFootsteps()?1:0) << endl;
	ofsOutput << "EXTERNALACCOUNTCREATION=" << (GetExternalAccountStatus()?1:0) << endl;
	ofsOutput << "INTERNALACCOUNTCREATION=" << (GetInternalAccountStatus()?1:0) << endl;
	ofsOutput << "SHOWHIDDENNPCS=" << (GetShowHiddenNpcStatus()?1:0) << endl;
	ofsOutput << "ROGUESENABLED=" << (GetRogueStatus()?1:0) << endl;
	ofsOutput << "ENGRAVEENABLED=" << (GetEngraveStatus()?1:0) << endl;
	ofsOutput << "UOXBOTENABLED=" << (GetUoxBotStatus()?1:0) << endl;
	ofsOutput << "PLAYERPERSECUTION=" << (GetPlayerPersecutionStatus()?1:0) << endl;
	ofsOutput << "LAGFIX=" << (GetLagFixStatus()?1:0) << endl;
	ofsOutput << "ACCOUNTFLUSH=" << GetAccountFlushTimer() << endl;
	ofsOutput << "HTMLSTATUSENABLED=" << GetHtmlStatsStatus() << endl;
	ofsOutput << "SELLBYNAME=" << (GetSellByNameStatus()?1:0) << endl;
	ofsOutput << "SELLMAXITEMS=" << GetSellMaxItemsStatus() << endl;
	ofsOutput << "TRADESYSTEM=" << (GetTradeSystemStatus()?1:0) << endl;
	ofsOutput << "RANKSYSTEM=" << (GetRankSystemStatus()?1:0) << endl;
	ofsOutput << "NPCTILECHECK=" << GetNpcTileCheckStatus() << endl;
	ofsOutput << "CUTSCROLLREQUIREMENTS=" << (GetCutScrollRequirementStatus()?1:0) << endl;
	ofsOutput << "WILDERNESSBANKENABLED=" << (GetWildernessBankStatus()?1:0) << endl;
	ofsOutput << "WILDERNESSBANKKEYWORD=" << GetWildernessBankTriggerString() << endl;
	ofsOutput << "HEARTBEAT=" << (GetSystemHeartBeatStatus()?1:0) << endl;
	ofsOutput << "SCRIPTSECTIONHEADER=" << (ServerScriptSectionHeader()?1:0) << endl;
	ofsOutput << "NPCTRAININGENABLED=" << (GetNPCTrainingStatus()?1:0) << endl;
	
	ofsOutput << endl << "[speedup]" << endl;
	ofsOutput << "SPEEDCHECKITEMS=" << GetCheckItemsSpeed() << endl;
	ofsOutput << "SPEEDCHECKNPCS=" << GetCheckNpcSpeed() << endl;
	ofsOutput << "SPEEDCHECKBOATS=" << GetCheckBoatSpeed() << endl;
	ofsOutput << "SPEEDCHECKNPCAI=" << GetCheckNpcAISpeed() << endl;
	ofsOutput << "SPEEDCHECKSPAWNREGIONS=" << GetCheckSpawnRegionSpeed() << endl;
	ofsOutput << "SPEEDCHECKMEMORY=" << GetCheckMemorySpeed() << endl;
	
	ofsOutput << endl << "[message boards]" << endl;
	ofsOutput << "MSGBOARDPOSTINGLEVEL=" << GetMsgBoardPostingLevel() << endl;
	ofsOutput << "MSGBOARDREMOVALLEVEL=" << GetMsgBoardPostRemovalLevel() << endl;
	ofsOutput << "MSGBOARDKEEPMESSAGES=" << GetMsgBoardKeepMsgsStatus() << endl;
	//ofsOutput << "MSGBOARDADMINLEVEL=" << GetMsgBoardAdminLevel() << endl;
	//ofsOutput << "MSGBOARDMAXMESSAGESALLOWED=" << GetMsgBoardMaxMsgsAllowed() << endl;
	//ofsOutput << "MSGBOARDMAXMSGSRESPONSE=" << GetMsgBoardMaxMsgResponse() << endl;

	ofsOutput << endl << "[escorts]" << endl;
	ofsOutput << "ESCORTENABLED=" << GetEscortsEnabled() << endl;
	ofsOutput << "ESCORTINITEXPIRE=" << GetEscortInitExpire() << endl;
	ofsOutput << "ESCORTACTIVEEXPIRE=" << GetEscortActiveExpire() << endl;
	ofsOutput << "ESCORTDONEEXPIRE=" << GetEscortDoneExpire() << endl;
	
	ofsOutput << endl << "[worldlight]" << endl;
	ofsOutput << "LIGHTMOON1=" << GetServerMoon( 0 ) << endl;
	ofsOutput << "LIGHTMOON1UPDATE=" << GetServerMoonUpdate( 0 ) << endl;
	ofsOutput << "LIGHTMOON2=" << GetServerMoon( 1 ) << endl;
	ofsOutput << "LIGHTMOON2UPDATE=" << GetServerMoonUpdate( 1 ) << endl;
	ofsOutput << "LIGHTDUNGEONLEVEL=" << GetDungeonLightLevel() << endl;
	ofsOutput << "LIGHTFIXEDLEVEL=" << GetWorldFixedLightLevel() << endl;
	ofsOutput << "LIGHTCURRENTLEVEL=" << GetWorldLightCurrentLevel() << endl;
	ofsOutput << "LIGHTBRIGHTLEVEL=" << GetWorldLightBrightLevel() << endl;
	ofsOutput << "LIGHTDARKLEVEL=" << GetWorldLightDarkLevel() << endl;
	ofsOutput << "DAY=" << GetServerTimeDay() << endl;
	ofsOutput << "HOUR=" << GetServerTimeHours() << endl;
	ofsOutput << "MINUTE=" << GetServerTimeMinutes() << endl;
	ofsOutput << "AMPM=" << ( GetServerTimeAMPM() ? 1 : 0 ) << endl;
	
	ofsOutput << endl << "[begging]" << endl;
	ofsOutput << "BEGGINGRANGE=" << GetBeggingRange() << endl;
	
	ofsOutput << endl << "[tracking]" << endl;
	ofsOutput << "TRACKINGBASERANGE=" << GetTrackingBaseRange() << endl;
	ofsOutput << "TRACKINGBASETIMER=" << GetTrackingBaseTimer() << endl;
	ofsOutput << "TRACKINGMAXTARGETS=" << GetTrackingMaxTargets() << endl;
	ofsOutput << "TRACKINGMSGREDISPLAYTIME=" << GetTrackingRedisplayTime() << endl;
	
	ofsOutput << endl << "[reputation]" << endl;
	ofsOutput << "REPSYSMURDERDECAYTIMER=" << GetRepMurderDecay() << endl;
	ofsOutput << "REPSYSMAXKILLS=" << GetRepMaxKills() << endl;
	ofsOutput << "REPSYSCRIMINALTIMER=" << GetRepCrimTime() << endl;
	
	ofsOutput << endl << "[resources]" << endl;
	ofsOutput << "RESOURCEMINECHECK=" << GetMineCheck() << endl;
	ofsOutput << "RESOURCEOREPERAREA=" << GetResOre() << endl;
	ofsOutput << "RESOURCEORERESPAWNTIMER=" << GetResOreTime() << endl;
	ofsOutput << "RESOURCEORERESPAWNAREA=" << GetResOreArea() << endl;
	ofsOutput << "RESOURCELOGSPERAREA=" << GetResLogs() << endl;
	ofsOutput << "RESOURCELOGSRESPAWNTIMER=" << GetResLogTime() << endl;
	ofsOutput << "RESPAWNLOGSRESPAWNAREA=" << GetResLogArea() << endl;
	
	ofsOutput << endl << "[hunger]" << endl;
	ofsOutput << "HUNGERRATE=" << GetHungerRate() << endl;
	ofsOutput << "HUNGERDAMAGERATETIMER=" << GetHungerDamageRateTimer() << endl;
	ofsOutput << "HUNGERDMGVAL=" << GetHungerDamage() << endl;
	ofsOutput << "HUNGERTHRESHOLD=" << GetHungerThreshold() << endl;

	
	ofsOutput << endl << "[combat]" << endl;
	ofsOutput << "COMBATMAXRANGE=" << GetCombatMaxRange() << endl;
	ofsOutput << "COMBATWRESTLESPEED=" << GetCombatWrestlingSpeed() << endl;
	ofsOutput << "COMBATSPELLMAXRANGE=" << GetCombatMaxSpellRange() << endl;
	ofsOutput << "COMBATMAXMELEEDAMAGE=" << GetCombatMaxMeleeDamage() << endl;
	ofsOutput << "COMBATMAXSPELLDAMAGE=" << GetCombatMaxSpellDamage() << endl;
	ofsOutput << "COMBATALLOWCRITICALS=" << GetCombatCriticalsEnabled() << endl;
	ofsOutput << "COMBATMAXPOISONINGDISTANCE=" << GetCombatMaxPoisoningDistance() << endl;
	ofsOutput << "COMBATDISPLAYHITMSG=" << (GetCombatDisplayHitMessage()?1:0) << endl;
	ofsOutput << "COMBATMAXHUMANABSORBTION=" << GetCombatMaxHumanAbsorbtion() << endl;
	ofsOutput << "COMBATMAXNONHUMANABSORBTION=" << GetCombatMaxNonHumanAbsorbtion() << endl;
	ofsOutput << "COMBATMONSTERSVSANIMALS=" << (GetCombatMonstersVsAnimals()?1:0) << endl;
	ofsOutput << "COMBATANIMALATTACKCHANCE=" << GetCombatAnimalsAttackChance() << endl;
	ofsOutput << "COMBATANIMALSGUARDED=" << (GetCombatAnimalsGuarded()?1:0) << endl;
	ofsOutput << "COMBATNPCDAMAGERATE=" << GetCombatNPCDamageRate() << endl;
	ofsOutput << "COMBATNPCBASEFLEEAT=" << GetCombatNPCBaseFleeAt() << endl;
	ofsOutput << "COMBATNPCBASEREATTACKAT=" << GetCombatNPCBaseReattackAt() << endl;
	ofsOutput << "COMBATATTACKSTAMINA=" << GetCombatAttackStamina() << endl;
	ofsOutput << "SHOOTONANIMALBACK=" << (GetShootOnAnimalBack()?1:0) << endl;
	
	ofsOutput << endl << "[start locations]" << endl;
	for( UI32 lCtr = 0; lCtr < startlocations.size(); lCtr++ )
		ofsOutput << "LOCATION=" << startlocations[lCtr].town << "," << startlocations[lCtr].description << "," << startlocations[lCtr].x << "," << startlocations[lCtr].y << "," << startlocations[lCtr].z << endl;
	
	ofsOutput << endl << "[startup]" << endl;
	ofsOutput << "STARTGOLD=" << GetServerStartGold() << endl;
	ofsOutput << "STARTPRIVS1=" << (UI16)GetServerStartPrivs( 0 ) << endl;
	ofsOutput << "STARTPRIVS2=" << (UI16)GetServerStartPrivs( 1 ) << endl;
	
	ofsOutput << endl << "[gumps]" << endl;
	ofsOutput << "TITLECOLOUR=" << GetTitleColour() << endl;
	ofsOutput << "LEFTTEXTCOLOUR=" << GetLeftTextColour() << endl;
	ofsOutput << "RIGHTTEXTCOLOUR=" << GetRightTextColour() << endl;
	ofsOutput << "BUTTONCANCEL=" << GetButtonCancel() << endl;
	ofsOutput << "BUTTONLEFT=" << GetButtonLeft() << endl;
	ofsOutput << "BUTTONRIGHT=" << GetButtonRight() << endl;
	ofsOutput << "BACKGROUNDPIC=" << GetBackgroundPic() << endl;
	
	ofsOutput << endl << "[towns]" << endl;
	ofsOutput << "POLLTIME=" << GetTownNumSecsPollOpen() << endl;
	ofsOutput << "MAYORTIME=" << GetTownNumSecsAsMayor() << endl;
	ofsOutput << "TAXPERIOD=" << GetTownTaxPeriod() << endl;
	ofsOutput << "GUARDSPAID=" << GetTownGuardPayment() << endl;
	
	ofsOutput.close();
	return true;
}

//o------------------------------------------------------------------------
//|	Function		-	cServerData *load( void )
//|	Programmer	-	EviLDeD
//|	Date				-	January 13, 2001
//|	Modified		-
//o------------------------------------------------------------------------
//|	Purpose			-	Load up the uox.ini file and parse it into the internals
//|	Returns			- pointer to the valid inmemory serverdata storage(this)
//|								NULL is there is an error, or invalid file type
//o------------------------------------------------------------------------
cServerData * cServerData::load( void )
{
	string s = GetRootDirectory();
	s += "/uox.ini";
	return load( s.c_str() );
}

//o------------------------------------------------------------------------
//|	Function		-	cServerData *load()
//|	Programmer	-	EviLDeD
//|	Date				-	January 13, 2001
//|	Modified		-
//o------------------------------------------------------------------------
//|	Purpose			-	Load up the specifiedu file and parse it into the internals
//|								providing that it is in the correct format.
//|	Returns			- pointer to the valid inmemory serverdata storage(this) or
//|								NULL is there is an error, or invalid file type
//o------------------------------------------------------------------------
cServerData * cServerData::load( const char *filename )
{
	long ver = VersionCheck( filename );
	if( ver == MAKEWORD( 1, 0 ) || ver == MAKEWORD( 1, 1 ) )
	{	// Version 1,0(default old format) so needs to parse the server.scp as well
		char tempStuff[1024];
		sprintf( tempStuff, "%s/server.scp", GetRootDirectory() );
		ParseUox3ServerScp( tempStuff );
		ParseUox3Ini( filename, ver );
	}
	else if( ver == MAKEWORD( 2, 0 ) )
	{	// Version greater than 1.1 - in this case defaults to 2.0(newformat)
		//printf("\n*** blah blah ***\n");
		//cServerData *DumpTest = ParseUox3Ini(filename, ver);
		ParseUox3Ini( filename, ver );
	}
	else if( ver == 0 )
		return NULL;
	return this;
}

void cServerData::SetTrackingBaseRange( UI32 value )
{
	trackingbaserange = value;
}
UI32 cServerData::GetTrackingBaseRange( void )
{
	return trackingbaserange;
}

void cServerData::SetTrackingMaxTargets( UI32 value )
{
	trackingmaxtargets = value;
}
UI32 cServerData::GetTrackingMaxTargets( void )
{
	return trackingmaxtargets;
}

void cServerData::SetTrackingBaseTimer( UI32 value )
{
	trackingbasetimer = value;
}
UI32 cServerData::GetTrackingBaseTimer( void )
{
	return trackingbasetimer;
}

void cServerData::SetTrackingRedisplayTime( UI32 value )
{
	trackingmsgredisplaytimer = value;
}
UI32 cServerData::GetTrackingRedisplayTime( void )
{
	return trackingmsgredisplaytimer;
}

void cServerData::SetBeggingRange( SI16 value )
{
	beggingrange = value;
}
SI16 cServerData::GetBeggingRange( void )
{
	return beggingrange;
}

void cServerData::SetFishingBaseTime( UI32 value )
{
	fishingbasetimer = value;
}
UI32 cServerData::GetFishingBaseTime( void )
{
	return fishingbasetimer;
}

void cServerData::SetFishingRandomTime( UI32 value )
{
	fishingrandomtimer = value;
}
UI32 cServerData::GetFishingRandomTime( void )
{
	return fishingrandomtimer;
}

void cServerData::SetSpiritSpeakTimer( UI32 value )
{
	spiritspeaktimer = value;
}
UI32 cServerData::GetSpiritSpeakTimer( void )
{
	return spiritspeaktimer;
}

//o-----------------------------------------------------------------------o
//|	Function		-	void DumpLookup(int lookupid);
//|	Date				-	January 31, 2001
//|	Programmer	-	EviLDeD
//o-----------------------------------------------------------------------o
//|	Purpose			-	This simply outputs a text file that contains the index
//|								and lookup name found in the lookuptables. Saves having
//|								to do it by hand if you make changes. Must be called from
//|								within the class if used.
//o-----------------------------------------------------------------------o
void cServerData::DumpLookup(int lookupid)
{
	ofstream ofsOutput;
	cServerData sd;
	char *source1 = NULL, *token = NULL;
	char buffer[81];
	int count = 0;

	switch( lookupid )
	{
		case 1:
			ofsOutput.open( "uox.ini.lookup.txt", ios::out );
			if( !ofsOutput.is_open() )
			{
				Console << "-o [DLOOKUP]: Unable to open file \"uox3.ini.lookup.txt\" for writing.." << myendl;
				return;
			}

			source1 = (char*)UOX3INI_LOOKUP.c_str();

			token = strtok( source1, "|" );
			while( token != NULL )
			{
				sprintf( buffer, "case 0x%04X:\t // %s[%04i]\n\tcServerData:\n\tbreak;\n", UOX3INI_LOOKUP.find( token, strlen( token ) ), token, count++ );
				ofsOutput << buffer;
				token = strtok( NULL, "|" );
			}
			ofsOutput.close();
			break;
		case 0:
		default:
			ofsOutput.open( "server.scp.lookup.txt", ios::out );
			if( !ofsOutput.is_open() )
			{
				Console << "|-o [DLOOKUP]: Unable to open file \"server.scp.lookup.txt\" for writing.." << myendl;
				return;
			}

			source1 = (char*)SERVERCFG_LOOKUP.c_str();

			token = strtok( source1, "|" );
			while( token != NULL )
			{
				sprintf( buffer, "case 0x%04X:\t // %s[%04i]\n\tcServerData:\n\tbreak;\n", SERVERCFG_LOOKUP.find( token, strlen( token ) ), token, count++ );
				ofsOutput << buffer;
				token = strtok( NULL, "|" );
			}
			ofsOutput.close();
			source1 = NULL;
			count = 0;
			break;
	}
}


//o--------------------------------------------------------------------------o
//|	Function/Class-	cServerData * cServerData::ParseUox3Ini( const char *filename, long ver )
//|	Date					-	02/26/2001
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	Parse teh uox.ini file into its required information.
//|									
//|	Modification	-	02/26/2002	-	Make sure that we parse out the logs, access
//|									and other directories that we were not parsing/
//o--------------------------------------------------------------------------o
//|	Returns				-
//o--------------------------------------------------------------------------o
cServerData * cServerData::ParseUox3Ini( const char *filename, long ver )
{
	if( !filename )
		return NULL;
	string sFilename = filename;
	ifstream inFile;

	Console << "Processing INI Settings (V" << (SI16)LOBYTE( ver ) << "." << (SI16)HIBYTE( ver ) << ") ";

	// Lock this file tight, No access at anytime when open(should only be open and closed anyhow. For Thread blocking)
	inFile.open( sFilename.c_str(), ios::in );	
	if( !inFile.is_open() )
	{
		Console << "FAILED!" << myendl << "Error: Unable to open file." << sFilename << myendl;
		return NULL;
	}
	char szLine[129];	// Trimmed this down to 80 to save some ram realestate
	inFile.getline( szLine, 128 );	// Snatch the first line.
	char *szpLineUpper = NULL;
	char *l, *r, *p;
	char  LineBuffer[81];
	SI16 zflag = 0;
	SI16 zi = 0,yi = 0;
	serverList.clear();
	startlocations.clear();
	while( !inFile.eof() )
	{
		strcpy( LineBuffer, szLine );
		szpLineUpper = strupr( LineBuffer );
		if( ( *(char*)szpLineUpper == '/' && *((char*)szpLineUpper + 1 ) == '/' ) || *(char*)szpLineUpper == '\n' )
		{
			inFile.getline( szLine, 128 );
			continue;
		}
		if( ver == MAKEWORD( 1, 0 ) || ver == MAKEWORD( 1, 1 ) )	// otherwise it'll never attempt a v2 load
		{
			if( strncmp( szLine, "###", strlen( "###" ) ) == 0 )
			{	// zflag == (0)-server info (1)-startlocations (2)-system settings
				zflag++;
				inFile.getline( szLine, 128 );
				continue;
			}
			if( zflag == 0 )
			{	//	Ok this is still server data for the server list parse it
				zi = serverList.size() + 1;
				serverList.resize( zi );
				serverList[zi-1].name = szLine;
				inFile.getline( szLine, 128 );
				if( inFile.eof() )
				{
					inFile.close();
					Console << "FAILED!" << myendl << "Error: Error parsing server data." << myendl;
					return NULL;
				}
				serverList[zi-1].ip = szLine;
				if( ver == MAKEWORD( 1, 1 ) )
				{
					inFile.getline( szLine, 128 );
					if( inFile.eof() )
					{
						inFile.close();
						Console << "FAILED!" << myendl << "Error: Error parsing server data." << myendl;
						return NULL;
					}
					serverList[zi-1].port = (SI16)makeNum( szLine ); 
				}
				else
					serverList[zi-1].port = GetServerPort();	// old 1.0 doesn't support port numbers, here ya go Abaddon :)
				serverList[zi-1].domain = "";		// Ditto here too, there is no domain and its really arbitrary at this stage. Will be used later.
				zi = 0;
				inFile.getline( szLine, 128 ); // Save some cycles having to parse down the rest.
				continue;
			}
			else if( zflag == 1 )
			{	// OK this ons ins the start position list there are 9, not sure why the ### is there anyhow
				zi = startlocations.size() + 1;
				startlocations.resize(zi);
				strncpy( startlocations[zi-1].town, szLine, 31 );
				for( yi = 0; yi <= 3; yi++ )
				{
					inFile.getline( szLine, 128 );
					if( inFile.eof() )
					{
						inFile.close();
						Console << "FAILED!" << myendl << "Error: Error parsing startlocation data." << myendl;
						return NULL;
					}
					switch( yi )
					{
						case 0:	
							strncpy(startlocations[zi-1].description, szLine, 31 );
							startlocations[zi-1].description[31] = 0;	// doesn't always NULL it
							break; 
						case 1:	startlocations[zi-1].x = (SI16)makeNum( szLine );	break;
						case 2:	startlocations[zi-1].y = (SI16)makeNum( szLine );	break;
						case 3:	startlocations[zi-1].z = (SI16)makeNum( szLine );	break;
						default:
							inFile.close();
							Console << "FAILED!" << myendl << "Error: Invalid XYZ location encountered." << myendl;
							return NULL;
					}
				}
				zi = 0;
				inFile.getline( szLine, 128 );		// Same cycle save here, its really small but its there
				continue;
			}
			else if( zflag == 2 )
			{	// server settings (lol if you could call it that)
				char szTemp[129];	// There are windows API that can do this but I wasn't sure abotu linux so I did it this way instead. Viva La Linux!
				strcpy( szTemp, szLine );
				for( int jj = strlen( szLine ); jj >= 0; jj-- )
				{
					if( szTemp[jj] != '\\' && szTemp[jj] != '/' )
						continue;
					else
					{
						szTemp[jj] = 0x00;
						break;
					}
				}
				strcpy( szLine, szTemp );
				SetDataDirectory( szLine );	//	Going to use the first mulfile listed as the directory for all mull files. Can always be changed later. :)
				for( UI08 kk = 0; kk <= 5; kk++ )	// Loop out the other dir listings. FileIO's your friend the strangled computer instructor frantically scratched onto his postit notes before the class killed him. Bawhahahahahahahahaha
				{
					inFile.getline( szLine, 128 );
					if( inFile.eof() )
					{
						inFile.close();
						Console << "FAILED!" << myendl << "Error: Invalid MUL file count detected." << myendl;
						return NULL;
					}
				}
				// OK now need to parse out the rest of this file.
				for( yi = 0; yi <= 4; yi++ )
				{
					inFile.getline( szLine, 128 );
					if( inFile.eof() )
					{
						inFile.close();
						Console << "FAILED!" << myendl << "Error: Error processing remainder of file" << myendl;
						return NULL;
					}
					switch( yi )
					{
						case 0:	SetServerSavesTimer( (UI32)makeNum( szLine ) );		break;
						case 1:	SetSystemHeartBeatStatus( makeNum( szLine ) == 1 );	break;
						case 2:	SetServerStartGold( (SI16)makeNum( szLine ) );		break;
						case 3:	SetServerStartPrivs( 0, (UI08)makeNum( szLine ) );	break;
						case 4:	SetServerStartPrivs( 1, (UI08)makeNum( szLine ) );	break;
						default:
							inFile.close();
							Console << "FAILED!" << myendl << "Error: Error parsing old version server settings." << myendl;
							return NULL;
					}
				}
				inFile.close();
				Console.PrintDone();
				return this;
			}
		}
		else if( ver == MAKEWORD( 2, 0 ) )
		{
			char seperator[2];	
			memset( seperator, 0x00, 2 );
			*seperator='=';
			l = strtok( szpLineUpper, seperator );
			r = strtok( NULL, "\0" );	// Get tag/value seperation (if there a way to do this using string and the stl ab)
			if( l == NULL )
			{
				inFile.getline( szLine, 128 );
				continue;
			}
			SI32 offset = 0;
			if( r != NULL )
			{
				offset = r - l;
				r = &szLine[offset];
			}

			switch( UOX3INI_LOOKUP.find( l ) )
			{
				case 0x000C:	// SERVERNAME
					SetServerName( (const char*)r );
					break;
				case 0x0017:	// CONSOLELOG
					SetServerConsoleLog( makeNum( r ) );
					break;
				case 0x0022:	// CRASHPROTECTION
					SetServerCrashProtection( makeNum( r ) );
					break;
				case 0x0032:	// COMMANDPREFIX
					SetServerCommandPrefix( *r );	// return the first character of the return string only
					break;
				case 0x0040:	// ANNOUNCEWORLDSAVES
					SetServerAnnounceSaves((makeNum( r )==1?true:false)); 
					break;
				case 0x0053:	// JOINPARTMSGS
					SetServerJoinPartAnnouncements((makeNum(r)==1?true:false));
					break;
				case 0x0060:	// MULCACHING
					SetServerMulCaching((makeNum( r )==1?true:false));
					break;
				case 0x006B:	// BACKUPSENABLED
					SetServerBackups((makeNum( r )==1?true:false));
					break;
				case 0x007A:	// SAVESPERLOOP
					//printf("\nr == %i(%s)\n",makeNum( r ),r);
					SetServerAntiLagSavesPerLoop( (SI16)makeNum( r ) );
					break;
				case 0x0087:	// SAVESTIMER
					SetServerSavesTimer( (UI32)makeNum( r ) );
					break;
				case 0x0092:	// MAINTHREADSLEEP
					SetServerMainThreadTimer( (UI32)makeNum( r ) );
					break;
				case 0x00A2:	// SKILLCAP
					SetServerSkillCap( (UI32)makeNum( r ) );
					break;
				case 0x00AB:	// SKILLDELAY
					SetServerSkillDelay( (UI32)makeNum( r ) );
					break;
				case 0x00B6:	// STATCAP
					SetServerStatCap( (UI32)makeNum( r ) );
					break;
				case 0x00BE:	// STATDELAY
					SetServerStatDelay( (UI32)makeNum( r ) );
					break;
				case 0x00C8:	// MAXSTEALTHMOVEMENTS
					SetMaxStealthMovement( (SI16)makeNum( r ) );
					break;
				case 0x00DC:	// MAXSTAMINAMOVEMENTS
					SetMaxStaminaMovement( (SI16)makeNum( r ) );
					break;
				case 0x00F0:	// CORPSEDECAYTIMER
					SetSystemTimer( CORPSE_DECAY, (UI32)makeNum( r ) );
					break;
				case 0x0101:	// LOOTDECAYTIMER
					SetSystemTimer( LOOT_DECAY, (UI32)makeNum( r ) );
					break;
				case 0x0110:	// WEATHERTIMER
					SetSystemTimer( WEATHER, (UI32)makeNum( r ) );
					break;
				case 0x011D:	// SHOWSPWANTIMER
					SetSystemTimer( SHOP_SPAWN, (UI32)makeNum( r ) );
					break;
				case 0x00F6:	// DECAYTIMER
					SetSystemTimer( DECAY, (UI32)makeNum( r ) );
					break;
				case 0x0137:	// INVISIBILITYTIMER
					SetSystemTimer( INVISIBILITY, (UI32)makeNum( r ) );
					break;
				case 0x0149:	// OBJECTUSETIMER
					SetSystemTimer( OBJECT_USAGE, (UI32)makeNum( r ) );
					break;
				case 0x0159:	// GATETIMER
					SetSystemTimer( GATE, (UI32)makeNum( r ) );
					break;
				case 0x0163:	// POISONTIMER
					SetSystemTimer( POISON, (UI32)makeNum( r ) );
					break;
				case 0x016F:	// LOGINTIMEOUT
					SetSystemTimer( LOGIN_TIMEOUT, (UI32)makeNum( r ) );
					break;
				case 0x017C:	// HITPOINTREGENTIMER
					SetSystemTimer( HITPOINT_REGEN, (UI32)makeNum( r ) );
					break;
				case 0x018F:	// STAMINAREGENTIMER
					SetSystemTimer( STAMINA_REGEN, (UI32)makeNum( r ) );
					break;
				case 0x01A1:	// MANAREGENTIMER
					SetSystemTimer( MANA_REGEN, (UI32)makeNum( r ) );
					break;
				case 0x01B0:	// ARMORAFFECTREGENTIMER
					SetSystemTimer( ARMORAFFECTMANA_REGEN, (UI32)makeNum( r ) );
					break;
				case 0x01C6:	// BASEFISHINGTIMER
					SetSystemTimer( BASE_FISHING, (UI32)makeNum( r ) );
					break;
				case 0x01D7:	// RANDOMFISHINGTIMER
					SetSystemTimer( RANDOM_FISHING, (UI32)makeNum( r ) );
					break;
				case 0x01EA:	// SPIRITSPEAKTIMER
					SetSystemTimer( SPIRIT_SPEAK, (UI32)makeNum( r ) );
					break;
				case 0x01FB:	// BEGGINGTIMER
					SetSystemTimer( BEGGING_T, (UI32)makeNum( r ) );
					break;
				case 0x0208:	// DIRECTORY
					SetRootDirectory( r );
					break;
				case 0x0212:	// DATADIRECTORY
					SetDataDirectory( r );
					break;
				case 0x0220:	// DEFSDIRECTORY
					SetDefsDirectory( r );
					break;
				case 0x022E:	// ACTSDIRECTORY
					SetAccountsDirectory( r );
					break;
				case 0x023C:	// SCRIPTSDIRECTORY
					SetScriptsDirectory( r );
					break;
				case 0x024D:	// BACKUPDIRECTORY
					SetBackupDirectory( r );
					break;
				case 0x025D:	// MSGBOARDDIRECTORY
					SetMsgBoardDirectory( r );
					break;
				case 0x026F:	// SHAREDDIRECTORY
					SetSharedDirectory( r );
					break;
				case 0x027F:	// LOOTDECAYSWITHCORPSE
					SetCorpseLootDecay( makeNum( r ) != 0 );
					break;
				case 0x0294:	// GUARDSACTIVE
					SetGuardStatus( makeNum( r ) != 0 );
					break;
				case 0x02A1:	// DEATHANIMATION
					SetDeathAnimationStatus( makeNum( r ) != 0 );
					break;
				case 0x02B0:	// AMBIENTSOUNDS
					this->SetWorldAmbientSounds( (SI16)makeNum( r ) );
					break;
				case 0x02BE:	// AMBIENTFOOTSTEPS
					SetAmbientFootsteps( makeNum( r ) != 0 );
					break;
				case 0x02CF:	// EXTERNALACCOUNTCREATION
					SetExternalAccountStatus( makeNum( r ) != 0 );
					break;
				case 0x02E7:	// INTERNALACCOUNTCREATION
					SetInternalAccountStatus( makeNum( r ) != 0 );
					break;
				case 0x02FF:	// SHOWHIDDENNPCS
					SetShowHiddenNpcStatus( makeNum( r ) != 0 );
					break;
				case 0x030E:	// ROGUESENABLED
					SetRogueStatus( makeNum( r ) != 0 );
					break;
				case 0x031C:	// UOXBOTENABLED
					SetUoxBotStatus( makeNum( r ) != 0 );
					break;
				case 0x032A:	// PLAYERPERSECUTION
					SetPlayerPersecutionStatus( makeNum( r ) != 0 );
					break;
				case 0x033C:	// LAGFIX
					SetLagFixStatus( makeNum( r ) != 0 );
					break;
				case 0x0343:	// ACCOUNTFLUSH
					SetAccountFlushTimer( atof( r ) );
					break;
				case 0x0350:	// HTMLSTATUSENABLED
					SetHtmlStatsStatus( (SI16)makeNum( r ) );
					break;
				case 0x0362:	// SELLBYNAME
					SetSellByNameStatus( makeNum( r ) == 1 );
					break;
				case 0x036D:	// SELLMAXITEMS
					SetSellMaxItemsStatus( (SI16)makeNum( r ) );
					break;
				case 0x037A:	// TRADESYSTEM
					SetTradeSystemStatus( makeNum( r ) != 0 );
					break;
				case 0x0386:	// RANKSYSTEM
					SetRankSystemStatus( makeNum( r ) != 0 );
					break;
				case 0x0391:	// NPCTILECHECK
					SetNpcTileCheckStatus( (SI16)makeNum( r ) );
					break;
				case 0x039E:	// CUTSCROLLREQUIREMENTS
					SetCutScrollRequirementStatus( (SI16)makeNum( r ) );
					break;
				case 0x03B4:	// WILDERNESSBANKENABLED
					SetWildernessBankStatus( makeNum( r ) == 1 );
					break;
				case 0x03CA:	// WILDERNESSBANKKEYWORD
					SetWildernessBankTriggerString(r);
					break;
				case 0x03E0:	// HEARTBEAT
					SetSystemHeartBeatStatus( makeNum( r ) != 0 );
					break;
				case 0x03EA:	// SPEEDCHECKITEMS
					SetCheckItemsSpeed((R64)atof(r));
					break;
				case 0x03FA:	// SPEEDCHECKNPCS
					SetCheckNpcSpeed((R64)atof(r));
					break;
				case 0x0409:	// SPEEDCHECKBOATS
					SetCheckBoatSpeed((R64)atof(r));
					break;
				case 0x0419:	// SPEEDCHECKNPCAI
					SetCheckNpcAISpeed((R64)atof(r));
					break;
				case 0x0429:	// SPEEDCHECKSPAWNREGIONS
					SetCheckSpawnRegionSpeed((R64)atof(r));
					break;
				case 0x0440:	// SPEEDCHECKMEMORY
					SetCheckMemorySpeed((R64)atof(r));
					break;
				case 0x0451:	// MSGBOARDKEEPMESSAGES
					SetMsgBoardKeepMsgs( makeNum( r ) );
					break;
				case 0x0466:	// MSGBOARDADMINLEVEL
					SetMsgBoardAdminLevel( (SI16)makeNum( r ) );
					break;
				case 0x0479:	// MSGBOARDPOSTINGLEVEL
					SetMsgBoardPostingLevel( (SI16)makeNum( r ) );
					break;
				case 0x048E:	// MSGBOARDREMOVALLEVEL
					SetMsgBoardPostRemovalLevel( (SI16)makeNum( r ) );
					break;
				case 0x04A3:	// MSGBOARDMAXMESSAGESALLOWED
					SetMsgBoardMaxMessageCount( (SI16)makeNum( r ) );
					break;
				case 0x04BE:	// MSGBOARDMAXMSGSRESPONSE
					//SetMsgBoardMaxMessageseAction((SI16)makeNum(r));
					break;
				case 0x03D6:	// ESCOURTENABLED
					SetEscortsEnabled( makeNum( r ) == 1 );
					break;
				case 0x04E4:	// ESCOURTINITEXPIRE
					SetEscortInitExpire( (UI32)makeNum( r ) );
					break;
				case 0x04F5:	// ESCOURTACTIVEEXPIRE
					SetEscortActiveExpire( (UI32)makeNum( r ) );
					break;
				case 0x0508:	// LIGHTMOON1
					SetServerMoon( 0, (SI16)makeNum( r ) );
					break;
				case 0x0513:	// LIGHTMOON1UPDATE
					SetServerMoonUpdate( 0, (SI16)makeNum( r ) );
					break;
				case 0x0524:	// LIGHTMOON2
					SetServerMoon( 1, (SI16)makeNum( r ) );
					break;
				case 0x052F:	// LIGHTMOON2UPDATE
					SetServerMoonUpdate( 1, (SI16)makeNum( r ) );
					break;
				case 0x0540:	// LIGHTDUNGEONLEVEL
					SetDungeonLightLevel( (SI16)makeNum( r ) );
					break;
				case 0x0552:	// LIGHTFIXEDLEVEL
					SetWorldFixedLightLevel( (SI16)makeNum( r ) );
					break;
				case 0x0562:	// LIGHTCURRENTLEVEL
					SetWorldLightCurrentLevel( (SI16)makeNum( r ) );
					break;
				case 0x0574:	// LIGHTBRIGHTLEVEL
					SetWorldLightBrightLevel( (SI16)makeNum( r ) );
					break;
				case 0x0585:	// BEGGINGRANGE
					SetBeggingRange( (SI16)makeNum( r ) );
					break;
				case 0x0592:	 // TRACKINGBASERANGE[0091]
					SetTrackingBaseRange( (UI32)makeNum( r ) );
					break;
				case 0x05A4:	 // TRACKINGBASETIMER[0092]
					SetTrackingBaseTimer( (UI32)makeNum( r ) );
					break;
				case 0x05B6:	 // TRACKINGMAXTARGETS[0093]
					SetTrackingMaxTargets( (UI32)makeNum( r ) );
					break;
				case 0x05C9:	 // TRACKINGMSGREDISPLAYTIME[0094]
					SetTrackingRedisplayTime( (UI32)makeNum( r ) );
					break;
				case 0x05E2:	 // REPSYSMURDERDECAYTIMER[0095]
					SetRepMurderDecay( (SI32)atol( r ) );
					break;
				case 0x05F9:	 // REPSYSMAXKILLS[0096]
					SetRepMaxKills( (SI32)makeNum( r ) );
					break;
				case 0x0608:	 // REPSYSCRIMINALTIMER[0097]
					SetRepCrimTime( (SI32)makeNum( r ) );
					break;
				case 0x061C:	 // RESOURCEMINECHECK[0098]
					SetMineCheck( (SI16)makeNum(r) );
					break;
				case 0x062E:	 // RESOURCEOREPERAREA[0099]
					SetResOre( (UI32)makeNum( r ) );
					break;
				case 0x0641:	 // RESOURCEORERESPAWNTIMER[0100]
					SetResOreTime((UI32)makeNum(r));
					break;
				case 0x0659:	 // RESOURCEORERESPAWNAREA[0101]
					SetResOreArea((UI32)makeNum(r));
					break;
				case 0x0670:	 // RESOURCELOGSPERAREA[0102]
					SetResLogs((UI32)makeNum(r));
					break;
				case 0x0684:	 // RESPURCELOGSRESPAWNTIMER[0103]
					SetResLogTime((UI32)makeNum(r));
					break;
				case 0x069D:	 // RESOURCELOGSRESPAWNAREA[0104]
					SetResLogArea((UI32)makeNum(r));
					break;
				case 0x06B5:	 // HUNGERRATE[0105]
					SetHungerRate((UI32)makeNum(r));
					break;
				case 0x06C0:	 // HUNGERDAMAGERATETIMER[0106]
					SetHungerDamageRateTimer((UI32)makeNum(r));
					break;
				case 0x06D6:	 // HUNGERDMGVAL[0107]
					SetHungerDamage( (SI16)makeNum( r ) );
					break;
				case 0x06E3:	 // HUNGERTHRESHOLD[0108]
					SetHungerThreshold((UI32)makeNum(r));
					break;
				case 0x06F3:	 // COMBATMAXRANGE[0109]
					SetCombatMaxRange((SI16)makeNum(r));
					break;
				case 0x0702:	 // COMBATWRESTLESPEED[0110]
					SetCombatWrestlingSpeed((SI16)makeNum(r));
					break;
				case 0x0715:	 // COMBATSPELLMAXRANGE[0111]
					SetCombatMaxSpellRange((SI16)makeNum(r));
					break;
				case 0x0729:	 // COMBATMAXMELEEDAMAGE[0112]
					SetCombatMaxMeleeDamage((SI16)makeNum(r));
					break;
				case 0x073E:	 // COMBATMAXSPELLDAMAGE[0113]
					//SetCombatMaxSpellDamage((SI16)makeNum(r));
					break;
				case 0x0753:	 // COMBATALLOWCRITICALS[0114]
					SetCombatCriticalsEnabled( makeNum( r ) == 1 );
					break;
				case 0x0768:	 // COMBATMAXPOISONINGDISTANCE[0115]
					SetCombatMaxPoisoningDistance((SI16)makeNum(r));
					break;
				case 0x0783:	 // COMBATDISPLAYHITMSG[0116]
					SetCombatDisplayHitMessage( makeNum( r ) == 1 );
					break;
				case 0x0797:	 // COMBATMAXHUMANABSORBTION[0117]
					SetCombatMaxHumanAbsorbtion((SI16)makeNum(r));
					break;
				case 0x07B0:	 // COMBATMAXNONHUMANABSORBTION[0118]
					SetCombatMaxNonHumanAbsorbtion((SI16)makeNum(r));
					break;
				case 0x07CC:	 // COMBATMONSTERSVSANIMALS[0119]
					SetCombatMonstersVsAnimals( makeNum( r ) == 1 );
					break;
				case 0x07E4:	 // COMBATANIMALATTACKCHANCE[0120]
					SetCombatAnimalsAttackChance((SI16)makeNum(r));
					break;
				case 0x07FD:	 // COMBATANIMALSGUARDED[0121]
					SetCombatAnimalsGuarded( makeNum( r ) == 1 );
					break;
				case 0x0812:	 // COMBATNPCDAMAGERATE[0122]
					SetCombatNPCDamageRate((SI16)makeNum(r));
					break;
				case 0x0826:	 // COMBATNPCBASEFLEEAT[0123]
					SetCombatNPCBaseFleeAt((SI16)makeNum(r));
					break;
				case 0x083A:	 // COMBATNPCBASEREATTACKAT[0124]
					SetCombatNPCBaseReattackAt((SI16)makeNum(r));
					break;
				case 0x0852:	 // COMBATATTACKSTAMINA[0125]
					SetCombatAttackStamina((SI16)makeNum(r));
					break;
				case 0x0866:	 // LOCATION[0126]
					SetServerLocation( r );
					break;
				case 0x086F:	 // STARTGOLD[0127]
					SetServerStartGold( (SI16)makeNum( r ) );
					break;
				case 0x0879:	 // STARTPRIVS1[0128]
					SetServerStartPrivs( 0, (UI08)makeNum( r ) );
					break;
				case 0x0885:	 // STARTPRIVS2[0129]
					SetServerStartPrivs( 1, (UI08)makeNum( r ) );
					break;
				case 0x0891:	 // ESCORTDONEEXPIRE[0130]
					SetEscortDoneExpire( (UI32)makeNum( r ) );
					break;
				case 0x08A2:	 // LIGHTDARKLEVEL[0131]
					SetWorldLightDarkLevel( (SI16)makeNum( r ) );
					break;
				case 0x08B1:	 // TITLECOLOUR[0132]
					SetTitleColour( (UI16)makeNum( r ) );
					break;
				case 0x08BD:	 // LEFTTEXTCOLOUR[0133]
					SetLeftTextColour( (UI16)makeNum( r ) );
					break;
				case 0x08CC:	 // RIGHTTEXTCOLOUR[0134]
					SetRightTextColour( (UI16)makeNum( r ) );
					break;
				case 0x08DC:	 // BUTTONCANCEL[0135]
					SetButtonCancel( (UI16)makeNum( r ) );
					break;
				case 0x08E9:	 // BUTTONLEFT[0136]
					SetButtonLeft( (UI16)makeNum( r ) );
					break;
				case 0x08F4:	 // BUTTONRIGHT[0137]
					SetButtonRight( (UI16)makeNum( r ) );
					break;
				case 0x0900:	 // BACKGROUNDPIC[0138]
					SetBackgroundPic( (UI16)makeNum( r ) );
					break;
				case 0x090E:	 // POLLTIME[0139]
					SetTownNumSecsPollOpen( (long)makeNum( r ) );
					break;
				case 0x0917:	 // MAYORTIME[0140]
					SetTownNumSecsAsMayor( (long)makeNum( r ) );
					break;
				case 0x0921:	 // TAXPERIOD[0141]
					SetTownTaxPeriod( (long)makeNum( r ) );
					break;
				case 0x092B:	 // GUARDSPAID[0142]
					SetTownGuardPayment( (long)makeNum( r ) );
					break;
				case 0x0936:	 // DAY[0143]
					SetServerTimeDay( (SI16)makeNum( r ) );
					break;
				case 0x093A:	 // HOURS[0144]
					SetServerTimeHours( (UI08)makeNum( r ) );
					break;
				case 0x0940:	 // MINUTES[0145]
					SetServerTimeMinutes( (UI08)makeNum( r ) );
					break;
				case 0x0948:	 // SECONDS[0146]
					SetServerTimeSeconds( (UI08)makeNum( r ) );
					break;
				case 0x0950:	 // AMPM[0147]
					SetServerTimeAMPM( makeNum( r ) != 0 );
					break;
				case 0x0955:	 // SKILLLEVEL[0148]
					SetSkillLevel( (UI32)makeNum( r ) );
					break;
				case 0x0960:	 // SNOOPISCRIME[0149]
					SetSnoopIsCrime( makeNum( r ) != 0 );
					break;
				case 0x096D:	 // ENGRAVEENABLED[0150]
					SetEngraveStatus( makeNum( r ) != 0 );
					break;
				case 0x097C:	 // BOOKSDIRECTORY[0151]
					SetBooksDirectory( r );
					break;
				case 0x098B:	 // SERVERLIST[0152]
					zi = serverList.size() + 1;
					serverList.resize( zi );
					p = strtok( szLine, "=\n\r" );
					
					p = strtok( NULL, ",\n\r" );
					serverList[zi-1].name = p;
					
					p = strtok( NULL, ",\n\r" );
					serverList[zi-1].ip = p;
					
					p = strtok( NULL, ",\n\r" );
					serverList[zi-1].port = (SI16)makeNum( p );	// old 1.0 doesn't support port numbers, here ya go Abaddon :)
					serverList[zi-1].domain = "";		// Ditto here too, there is no domain and its really arbitrary at this stage. Will be used later.
					zi = 0;
					break;

				case 0x0996:	 // SCRIPTSECTIONHEADER[0153]
					ServerScriptSectionHeader( makeNum( r ) != 0 );
					break;
				case 0x09AA:	 // PORT[0154] // whatever that stands for..
					SetServerPort( (SI16)makeNum( r ) );
					break;
				case 0x09AF:	 // SAVEMODE[0155]
					if( !strcmp( r, "BINARY" ) ) 
						SaveMode( 1 );
					else 
						SaveMode( 0 );
					break;
				case 0x09B8:	 // ACCESSDIRECTORY[0156]
					cServerData::SetAccessDirectory( r );
					break;
				case 0x09C8:	 // LOGSDIRECTORY[0157]
					cServerData::SetLogsDirectory( r );
					break;
				case 0x09D6:	 // ACCOUNTISOLATION[0158]
					//cServerData::SetAccountIsolation( makeNum( r ) );
					break;
				case 0x09E7:	 // HTMLDIRECTORY[0159]
					cServerData::SetHTMLDirectory( r );
					break;
				case 0x09F5:	 // SHOOTONANIMALBACK[0160]
					cServerData::SetShootOnAnimalBack( makeNum( r ) == 1 );
					break;
				case 0x0A07:	 // NPCTRAININGENABLED[0161]
					cServerData::SetNPCTrainingStatus( makeNum( r ) == 1 );
					break;
				case 0x0A1A:	 // GUMPSDIRECTORY[0162]
					cServerData::SetGumpsDirectory( r );
					break;
			        case 0x0A29:	 // DICTIONARYDIRECTORY[0163]
				        cServerData::SetDictionaryDirectory( r );
	                                break;
				default:
					//Console << "Unknown tag \"" << l << "\" in " << filename << myendl;					break;
					break;
			}
			inFile.getline( szLine, 127 );
			continue;
		}
	}
	
	Console.PrintDone();
	return this;
}

cServerData * cServerData::ParseUox3ServerScp(const char *filename)
{
	if( !filename )
		return NULL;
	string sFilename = filename;
	ifstream inFile;

	Console << "Processing server file \"" << filename << "\"..." << "Detected V1...";

	// Lock this file tight, No access at anytime when open(should only be open and closed anyhow. For Thread blocking)
	inFile.open( sFilename.c_str(), ios::in );	
	if( !inFile.is_open() )
	{
		Console << "FAILED!" << myendl << "Error: Unable to open source file." << filename << myendl;
		return NULL;
	}
	char szLine[129];	// Trimmed this down to 80 to save some ram realestate
	inFile.getline( szLine, 128 );	// Snatch the first line.
	char *szpLineUpper = NULL, LineBuffer[129];
	char *l = NULL, *r = NULL;

	while( !inFile.eof() )
	{
		strcpy(LineBuffer,szLine);
		//Console << szLine << myendl;
		szpLineUpper = strupr( LineBuffer );
		if( ( *(char*)szpLineUpper == '/' && *((char*)szpLineUpper + 1 ) == '/' ) || *(char*)szpLineUpper == '\n' )
		{
			inFile.getline( szLine, 128 );
			continue;
		}
		l = strtok( szpLineUpper, " ");
		r = strtok( NULL, "\0" );	// Get tag/value seperation (if there a way to do this using string and the stl ab)
		if(l==NULL || (l!=NULL&&r==NULL) || !strcmp("SECTION",l) || *(l)=='{' || *(l)=='}')
		{
			inFile.getline( szLine, 128 );
			continue;
		}
		int lookupidx=SERVERCFG_LOOKUP.find( l );
		switch( lookupidx )
		{
			case 0x0001:	// CRASH_PROTECTION[0001]
			case 0x0012:	// CRASH_PROTECTION[0001]
				SetServerCrashProtection((UI32)makeNum(r));
				break;
			case 0x0023:	// SKILLCAP[0002]
				SetServerSkillCap((UI32)makeNum(r));
				break;
			case 0x002C:	// STATCAP[0003]
				SetServerStatCap((UI32)makeNum(r));
				break;
			case 0x0034:	// USESPECIALBANK[0004]
				SetWildernessBankStatus( makeNum( r ) == 1 );
				break;
			case 0x0043:	// SPECIALBANKTRIGGER[0005]
				SetWildernessBankTriggerString(r);
				break;
			case 0x0056:	// DECAYTIMER[0006]
				SetServerDecayTimer((UI32)makeNum(r));
				break;
			case 0x0061:	// PLAYERCORPSEDECAYMULTIPLIER[0007]
				//SetCorpseDecayMultiplier((UI32)makeNum(r));
				break;
			case 0x007D:	// LOOTDECAYSWITHCORPSE[0008]
				SetCorpseLootDecay( makeNum( r ) == 1 );
				break;
			case 0x0092:	 // INVISTIMER[0009]
				SetSystemTimer( INVISIBILITY, ( (UI32)makeNum(r) ) );
				break;
			case 0x009D:	 // SKILLDELAY[0010]
				SetServerSkillDelay( (UI32)makeNum(r));
				break;
			case 0x00A8:	 // SKILLLEVEL[0011]
				SetSkillLevel((UI32)makeNum(r));
				break;
			case 0x00B3:	 // OBJECTDELAY[0012]
				SetSystemTimer( OBJECT_USAGE, ((UI32)makeNum(r)) );
				break;
			case 0x00BF:	 // GATETIMER[0013]
				SetSystemTimer( GATE, (UI32)makeNum(r) );
				break;
			case 0x00C9:	 // SHOWDEATHANIM[0014]
				SetDeathAnimationStatus( makeNum( r ) == 1 );
				break;
			case 0x00D7:	 // GUARDSACTIVE[0015]
				SetGuardStatus( makeNum( r ) == 1 );
				break;
			case 0x00E4:	 // BG_SOUNDS[0016]
				SetWorldAmbientSounds((SI16)makeNum(r));
				break;
			case 0x00EE:	 // ANNOUNCE_WORLDSAVES[0017]
				SetServerAnnounceSaves( makeNum( r ) == 1 );
				break;
			case 0x0102:	 // WWWACCOUNTS[0018]
				SetExternalAccountStatus( makeNum( r ) == 1 );
				break;
			case 0x010E:	 // ARCHIVEPATH[0019]
				SetBackupDirectory( r );
				break;
			case 0x011A:	 // SAVE_PER_LOOP[0020]
				SetServerAntiLagSavesPerLoop((SI16)makeNum(r));	
				break;
			case 0x0128:	 // BACKUP_SAVE_RATIO[0021]
				SetBackupRatio((SI16)makeNum(r));
				break;
			case 0x013A:	 // UOXBOT[0022]
				SetUoxBotStatus( makeNum( r ) == 1 );
				break;
			case 0x0141:	 // POISONTIMER[0023]
				SetPoisonTimer((UI32)makeNum(r));
				break;
			case 0x014D:	 // JOINMSG[0024]
				SetServerJoinPartAnnouncements( makeNum( r ) == 1 );
				break;
			case 0x0155:	 // PARTMSG[0025] Ignored now
				break;
			case 0x015D:	 // LOG[0026]
				SetServerConsoleLog((SI08)makeNum(r));
				break;
			case 0x0161:	 // ROGUE[0027]
				SetRogueStatus( makeNum( r ) == 1 );
				break;
			case 0x0167:	 // WEATHERTIME[0028]
				SetWeatherTimer((UI32)makeNum(r));
				break;
			case 0x0173:	 // SHOPTIME[0029]
				SetSystemTimer( SHOP_SPAWN, (UI32)makeNum(r) );
				break;
			case 0x017C:	 // SHOWLOGGEDOUTPCS[0030]
				SetShowHiddenNpcStatus( makeNum( r ) == 1 );
				break;
			case 0x018D:	 // CHAR_TIME_OUT[0031]
				SetSystemTimer( LOGIN_TIMEOUT, ((UI32)makeNum(r)) );
				break;
			case 0x019B:	 // MAXSTEALTHSTEPS[0032]
				SetMaxStealthMovement((SI16)makeNum(r));
				break;
			case 0x01AB:	 // RUNNINGSTAMINASTEPS[0033]
				SetMaxStaminaMovement((SI16)makeNum(r));
				break;
			case 0x01BF:	 // BOAT_SPEED[0034]
				SetCheckBoatSpeed((R64)atof(r));
				break;
			case 0x01CA:	 // HTML[0035]
				SetHtmlStatsStatus((SI16)makeNum(r));
				break;
			case 0x01CF:	 // CUT_SCROLL_REQUIREMENTS[0036]
				SetCutScrollRequirementStatus((SI16)(makeNum(r)));
				break;
			case 0x01E7:	 // PERSECUTION[0037]
				SetPlayerPersecutionStatus( makeNum( r ) == 1 );
				break;
			case 0x01F3:	 // AUTO_CREATE_ACCTS[0038]
				SetInternalAccountStatus( makeNum( r ) == 1 );
				break;
			case 0x0205:	 // MSGBOARDPATH[0039]
				SetMsgBoardDirectory(r);
				break;
			case 0x0212:	 // MSGPOSTACCESS[0040]
				SetMsgBoardPostingLevel((SI16)makeNum(r));
				break;
			case 0x0220:	 // MSGPOSTREMOVE[0041]
				SetMsgBoardPostRemovalLevel((SI16)makeNum(r));
				break;
			case 0x022E:	 // MSGRETENTION[0042]
				SetMsgBoardKeepMsgs( (UI32)makeNum( r ) );
				break;
			case 0x023B:	 // ESCORTACTIVE[0043]
				SetEscortsEnabled( makeNum( r ) == 1 );
				break;
			case 0x0248:	 // ESCORTINITEXPIRE[0044]
				SetEscortInitExpire((UI32)makeNum(r));
				break;
			case 0x0259:	 // ESCORTACTIVEEXPIRE[0045]
				SetEscortActiveExpire((UI32)makeNum(r));
				break;
			case 0x026C:	 // ESCORTDONEEXPIRE[0046]
				SetEscortDoneExpire((UI32)makeNum(r));
				break;
			case 0x027D:	 // FOOTSTEPS[0047]
				SetAmbientFootsteps( makeNum( r ) == 1 );
				break;
			case 0x0287:	 // COMMANDPREFIX[0048]
				SetServerCommandPrefix((char)*r);
				break;
			case 0x0295:	 // PORT[0049]
				SetServerPort((SI16)makeNum(r));
				break;
			case 0x029A:	 // NICE[0050] Not sure what this does
				SetNiceness((SI16)makeNum(r));
				break;
			case 0x029F:	 // LORDB_LAG_FIX[0051]
				SetLagFixStatus( makeNum( r ) == 1 );
				break;
			case 0x02AD:	 // CHECK_ITEMS[0052]
				SetCheckItemsSpeed((R64)atof(r));
				break;
			case 0x02B9:	 // CHECK_NPCS[0053]
				SetCheckNpcSpeed((R64)atof( r ));
				break;
			case 0x02C4:	 // NPC_TILE_CHECK[0054]
				SetNpcTileCheckStatus( (SI16)makeNum( r ) );
				break;
			case 0x02D3:	 // CHECK_NPCAI[0055]
				SetCheckNpcAISpeed((R64)atof(r));
				break;
			case 0x02DF:	 // CHECK_SPAWNREGIONS[0056]
				SetCheckSpawnRegionSpeed((R64)atof(r));
				break;
			case 0x02F2:	 // CHECKMEM[0057]
				SetCheckMemorySpeed((R64)atof(r));
				break;
			case 0x02FB:	 // CACHE_MULS[0058]
				SetServerMulCaching( makeNum( r ) == 1 );
				break;
			case 0x0306:	 // ACCOUNTFLUSH[0059]
				SetAccountFlushTimer( (R64)atof( r ) );
				break;
			case 0x0313:	 // MAXRANGE[0060] 
				SetCombatMaxRange( (SI16)makeNum( r ) );
				break;
			case 0x031C:	 // WRESTLESPEED[0061]
				SetCombatWrestlingSpeed((SI16)makeNum(r));
				break;
			case 0x0329:	 // MAX_RANGE_SPELL[0062]
				SetCombatMaxSpellRange((SI16)makeNum(r));
				break;
			case 0x0339:	 // MAXDMG[0063]
				SetCombatMaxMeleeDamage((SI16)makeNum(r));
				break;
			case 0x0340:	 // DEATH_ON_THROAT[0064]
				SetCombatCriticalsEnabled( makeNum( r ) == 1 );
				break;
			case 0x0350:	 // DIST_TO_POISON[0065]
				SetCombatMaxPoisoningDistance((SI16)makeNum(r));
				break;
			case 0x035F:	 // COMBAT_HIT_MESSAGE[0066]
				SetCombatDisplayHitMessage( makeNum( r ) == 1 );
				break;
			case 0x0372:	 // MAX_ABSORBTION[0067]
				SetCombatMaxHumanAbsorbtion((SI16)makeNum(r));
				break;
			case 0x0381:	 // MAX_NON_HUMAN_ABSORBTION[0068]
				SetCombatMaxNonHumanAbsorbtion((SI16)makeNum(r));
				break;
			case 0x039A:	 // MONSTERS_VS_ANIMALS[0069]
				SetCombatMonstersVsAnimals( makeNum( r ) == 1 );
				break;
			case 0x03AE:	 // ANIMALS_ATTACK_CHANCE[0070]
				SetCombatAnimalsAttackChance((SI16)makeNum(r));
				break;
			case 0x03C4:	 // ANIMALS_GUARDED[0071]
				SetCombatAnimalsGuarded( makeNum( r ) == 1 );
				break;
			case 0x03D4:	 // NPC_DAMAGE_RATE[0072]
				SetCombatNPCDamageRate((SI16)makeNum(r));
				break;
			case 0x03E4:	 // NPC_BASE_FLEEAT[0073]
				SetCombatNPCBaseFleeAt((SI16)makeNum(r));
				break;
			case 0x03F4:	 // NPC_BASE_REATTACKAT[0074]
				SetCombatNPCBaseReattackAt((SI16)makeNum(r));
				break;
			case 0x0408:	 // ATTACKSTAMINA[0075]
				SetCombatAttackStamina((SI16)makeNum(r));
				break;
			case 0x0416:	 // SELLBYNAME[0076]
				SetSellByNameStatus( makeNum( r ) == 1 );
				break;
			case 0x0421:	 // SELLMAXITEM[0077]
				SetSellMaxItemsStatus((SI16)makeNum(r)); // rename this in the class to this name not with status on the end -> What Ab?
				break;
			case 0x042D:	 // TRADESYSTEM[0078]
				SetTradeSystemStatus( makeNum( r ) == 1 );
				break;
			case 0x0439:	 // RANKSYSTEM[0079]
				SetRankSystemStatus( makeNum( r ) == 1 );
				break;
			case 0x0444:	 // HITPOINTS_REGENRATE[0080]
				SetSystemTimer( HITPOINT_REGEN, (UI32)makeNum( r ) );
				break;
			case 0x0458:	 // STAMINA_REGENRATE[0081]
				SetSystemTimer( STAMINA_REGEN, (UI32)makeNum( r ) );
				break;
			case 0x046A:	 // MANA_REGENRATE[0082]
				SetSystemTimer( MANA_REGEN, (UI32)makeNum( r ) );
				break;
			case 0x0479:	 // ARMOR_AFFECT_MANA_REGEN[0083]
				SetSystemTimer( ARMORAFFECTMANA_REGEN, (UI32)makeNum( r ) );
				break;
			case 0x0491:	 // HUNGERRATE[0084]
				SetHungerRate((UI32)makeNum(r));
				break;
			case 0x049C:	 // HUNGER_DAMAGE[0085]
				SetHungerDamage((SI16)makeNum(r));
				break;
			case 0x04AA:	 // HUNGER_DAMAGE_RATE[0086]
				SetHungerDamageRateTimer((UI32)makeNum(r));
				break;
			case 0x04BD:	 // MINECHECK[0087]
				SetMineCheck( (SI16)makeNum(r) );
				break;
			case 0x04C7:	 // ORE_PER_AREA[0088]
				SetResOre((UI32)makeNum(r));
				break;
			case 0x04D4:	 // ORE_RESPAWN_TIME[0089]
				SetResOreTime( (UI32)atol( r ) );
				break;
			case 0x04E5:	 // ORE_RESPAWN_AREA[0090]
				SetResOreArea((UI32)makeNum(r));
				break;
			case 0x04F6:	 // LOGS_PER_AREA[0091]
				SetResLogs((UI32)makeNum(r));
				break;
			case 0x0504:	 // LOG_RESPAWN_TIME[0092]
				SetResLogTime( (UI32)atol(r) );
				break;
			case 0x0515:	 // LOG_RESPAWN_AREA[0093]
				SetResLogArea((UI32)makeNum(r));
				break;
			case 0x0526:	 // MURDER_DECAY[0094]
				SetRepMurderDecay((long)atol(r));
				break;
			case 0x0533:	 // MAXKILLS[0095]
				SetRepMaxKills(makeNum(r));
				break;
			case 0x053C:	 // CRIMINAL_TIME[0096]
				SetRepCrimTime(makeNum(r));
				break;
			case 0x054A:	 // BASE_TRACKING_RANGE[0097]
				SetTrackingBaseRange((UI32)makeNum(r));
				break;
			case 0x055E:	 // MAX_TRACKING_TARGETS[0098]
				SetTrackingMaxTargets( (UI32)makeNum(r));
				break;
			case 0x0573:	 // BASE_TRACKING_TIME[0099]
				SetTrackingBaseTimer((UI32)makeNum(r));
				break;
			case 0x0586:	 // TRACKING_MESSAGE_REDISPLAY_TIME[0100]
				SetTrackingRedisplayTime((UI32)makeNum(r));
				break;
			case 0x05A6:	 // BEGGING_RANGE[0101]
				SetBeggingRange( (SI16)makeNum( r ) );
				break;
			case 0x05B4:	 // BASE_FISHING_TIME[0102]
				SetFishingBaseTime((UI32)makeNum(r));
				break;
			case 0x05C6:	 // RANDOM_FISHING_TIME[0103]
				SetFishingRandomTime((UI32)makeNum(r));
				break;
			case 0x05DA:	 // SECTION SPIRITSPEAK[0104]
			case 0x05EE:	 // SPIRITSPEAKTIMER[0105]
				SetSpiritSpeakTimer((UI32)makeNum(r));
				break;
			case 0x05FF:	 // DAY[0106]
				SetServerTimeDay( (SI16)makeNum(r) );
				break;
			case 0x0603:	 // HOUR[0107]
				SetServerTimeHours( (UI08)makeNum(r) );
				break;
			case 0x0608:	 // MINUTE[0108]
				SetServerTimeMinutes( (UI08)makeNum(r) );
				break;
			case 0x060F:	 // AMPM[0109]
				SetServerTimeAMPM( makeNum( r ) != 0 );
				break;
			case 0x0614:	 // MOON1[0110]
				SetServerMoon(0,(SI16)makeNum(r));
				break;
			case 0x061A:	 // MOON2[0111]
				SetServerMoon(1,(SI16)makeNum(r));
				break;
			case 0x0620:	 // MOON1UPDATE[0112]
				SetServerMoonUpdate(0,(SI16)makeNum(r));
				break;
			case 0x062C:	 // MOON2UPDATE[0113]
				SetServerMoonUpdate(1,(SI16)makeNum(r));
				break;
			case 0x0638:	 // DUNGEONLIGHTLEVEL[0114]
				SetDungeonLightLevel((SI16)makeNum(r));
				break;
			case 0x064A:	 // WORLDFIXEDLEVEL[0115]
				SetWorldFixedLightLevel((SI16)makeNum(r));
				break;
			case 0x065A:	 // WORLDCURLEVEL[0116]
				SetWorldLightCurrentLevel((SI16)makeNum(r));
				break;
			case 0x0668:	 // WORLDBRIGHTLEVEL[0117]
				SetWorldLightBrightLevel((SI16)makeNum(r));
				break;
			case 0x0679:	 // WORLDDARKLEVEL[0118]
				SetWorldLightDarkLevel((SI16)makeNum(r));
				break;
			case 0x0688:	 // SECONDSPERUOMINUTE[0119]
				break;
			case 0x069B:	 // TITLECOLOUR[0120]
				SetTitleColour( (UI16)makeNum( r ) );
				break;
			case 0x06A7:	 // LEFTTEXTCOLOUR[0121]
				SetLeftTextColour( (UI16)makeNum( r ) );
				break;
			case 0x06B6:	 // RIGHTTEXTCOLOUR[0122]
				SetRightTextColour( (UI16)makeNum( r ) );
				break;
			case 0x06C6:	 // BUTTONCANCEL[0123]
				SetButtonCancel( (UI16)makeNum( r ) );
				break;
			case 0x06D3:	 // BUTTONLEFT[0124]
				SetButtonLeft( (UI16)makeNum( r ) );
				break;
			case 0x06DE:	 // BUTTONRIGHT[0125]
				SetButtonRight( (UI16)makeNum( r ) );
				break;
			case 0x06EA:	 // BACKGROUNDPIC[0126]
				SetBackgroundPic( (UI16)makeNum( r ) );
				break;
			case 0x06F8:	 // POLLTIME[0127]
				SetTownNumSecsPollOpen( (SI32)makeNum( r ) );
				break;
			case 0x0701:	 // MAYORTIME[0128]
				SetTownNumSecsAsMayor( (SI32)makeNum( r ) );
				break;
			case 0x070B:	 // TAXPERIOD[0129]
				SetTownTaxPeriod( (SI32)makeNum( r ) );
				break;
			case 0x0715:	 // GUARDSPAID[0130]
				SetTownGuardPayment( (SI32)makeNum( r ) );
				break;
			case 0x0720:	 // SNOOPISCRIME[0131]
				SetSnoopIsCrime( makeNum( r ) != 0 );
				break;
			case 0x072D:	 // ENGRAVEENABLED[0132]
				SetEngraveStatus( makeNum( r ) != 0 );
				break;
			default:
				inFile.getline( szLine, 128 );
				continue;
		}
		inFile.getline( szLine, 128 );
	}
	Console.PrintDone();

	return this;
}

void cServerData::SetServerStartGold( SI16 value )
{
	if( value < 0 )
		return;
	startgold = value;
}


SI16 cServerData::GetServerStartGold( void ) const
{
	return startgold;
}

void cServerData::SetServerStartPrivs( SI16 slot, UI08 value )
{
	if( slot < 0 || slot > 1 && value < 0 )
		return;
	startprivs[slot] = value;
}

UI08 cServerData::GetServerStartPrivs( SI16 slot ) const
{
	if( slot < 0 || slot > 1 )
		return 0;
	return startprivs[slot];
}

void cServerData::SetServerDecayTimer( UI32 value )
{
	decaytimer=value;
}

UI32 cServerData::GetServerDecayTimer( void )
{
	return decaytimer;
}

void cServerData::SetPoisonTimer( UI32 value )
{
	poisontimer=value;
}

UI32 cServerData::GetPoisonTimer( void )
{
	return poisontimer;
}

void cServerData::SetWeatherTimer( UI32 value )
{
	weathertimer=value;
}

UI32 cServerData::GetWeatherTimer( void )
{
	return weathertimer;
}

void cServerData::SetMsgBoardKeepMsgs( UI32 value )
{
	keepmessages = value;
}

UI32 cServerData::GetMsgBoardKeepMsgsStatus( void )
{
	return keepmessages;
}

void cServerData::SetServerHitpointRegenTimer( UI32 value )
{
	hitpointregentimer = value;
}

UI32 cServerData::GetServerHitpointRegenTimer( void )
{
	return hitpointregentimer;
}

void cServerData::SetServerStaminaRegenTimer( UI32 value )
{
	staminaregentimer = value;
}

UI32 cServerData::GetServerStaminaRegenTimer( void )
{
	return staminaregentimer;
}

void cServerData::SetServerManaRegenTimer( UI32 value )
{
	manaregentimer = value;
}

UI32 cServerData::GetServerManaRegenTimer( void )
{
	return manaregentimer;
}

void cServerData::SetServerMoon( SI16 slot, SI16 value )
{
	if( slot < 0 || slot > 1 || value < 0 )
		return;
	moon[slot]=value;
}

void cServerData::SetServerMoonUpdate( SI16 slot, SI16 value )
{
	if( slot < 0 || slot > 1 || value < 0 )
		return;
	moonupdate[slot]=value;
}

SI16 cServerData::GetServerMoon(SI16 slot)
{
	if(slot<0||slot>1)
		return -1;
	return moon[slot];
}

SI16 cServerData::GetServerMoonUpdate(SI16 slot)
{
	if(slot<0||slot>1)
		return -1;
	return moonupdate[slot];
}

void cServerData::SetDungeonLightLevel(SI16 value)
{
	if(value<0)
		return;
	dungeonlightlevel=value;
}

SI16 cServerData::GetDungeonLightLevel( void )
{
	return dungeonlightlevel;
}

void cServerData::SetWorldFixedLightLevel(SI16 value)
{
	if(value<0)
		return;
	fixedlightlevel=value;
}

SI16 cServerData::GetWorldFixedLightLevel( void )
{
	return fixedlightlevel;
}

void cServerData::SetWorldLightCurrentLevel( SI16 value )
{
	if(value<0)
		return;
	currentlightlevel=value;
}

SI16 cServerData::GetWorldLightCurrentLevel( void )
{
	return currentlightlevel;
}

void cServerData::SetWorldLightBrightLevel(SI16 value)
{
	if( value < 0 )
		return;
	brightnesslightlevel = value;
}

SI16 cServerData::GetWorldLightBrightLevel( void )
{
	return brightnesslightlevel;
}

void cServerData::SetWorldLightDarkLevel(SI16 value)
{
	if( value < 0 )
		return;
	darknesslightlevel=value;
}

SI16 cServerData::GetWorldLightDarkLevel( void )
{
	return darknesslightlevel;
}

void cServerData::PostLoadDefaults( void )
{
	if( startlocations.size() == 0 )
	{
		SetServerLocation( "Yew,Center,545,982,0" );
		SetServerLocation( "Minoc,Tavern,2477,411,15" );
		SetServerLocation( "Britain,Sweet Dreams Inn,1495,1629,10" );
		SetServerLocation( "Moonglow,Docks,4406,1045,0" );
		SetServerLocation( "Trinsic,West Gate,1832,2779,0" );
		SetServerLocation( "Magincia,Docks,3675,2259,20" );
		SetServerLocation( "Jhelom,Docks,1492,3696,0" );
		SetServerLocation( "Skara Brae,Docks,639,2236,0" );
		SetServerLocation( "Vesper,Ironwood Inn,2771,977,0" );
	}
}

void cServerData::SetServerLocation( const char *toSet )
{
	char *toScan = NULL;
	toScan = new char[strlen( toSet ) + 1];
	strcpy( toScan, toSet );
	char *parts[5] = { NULL, NULL, NULL, NULL, NULL };
	char *token = NULL;
	token = strtok( toScan, "," );
	int pCounter = 0;
	parts[pCounter++] = token;
	while( token != NULL )
	{
		token = strtok( NULL, "," );
		parts[pCounter++] = token;
		if( pCounter == 5 )
			break;
	}
	if( pCounter == 5 )	// Wellformed server location
	{
		STARTLOCATION toAdd;
		toAdd.x = (SI16)makeNum( parts[2] );
		toAdd.y = (SI16)makeNum( parts[3] );
		toAdd.z = (SI16)makeNum( parts[4] );
		strcpy( toAdd.town, parts[0] );
		strcpy( toAdd.description, parts[1] );
		startlocations.push_back( toAdd );
	}
	else
	{
		Console.Error( 2, "Malformed location entry in ini file" );
	}
	delete [] toScan;		// Must come last, or we delete our manipulation string before we manipulate it
}
LPSTARTLOCATION cServerData::GetServerLocation( UI32 locNum )
{
	if( locNum < 0 || locNum >= startlocations.size() )
		return NULL;
	else
		return &startlocations[locNum];
}

UI32 cServerData::GetNumServerLocations( void ) const
{
	return startlocations.size();
}


void cServerData::SetCombatMaxSpellDamage( SI16 value )
{
	combatmaxspelldamage = value;
}
SI16 cServerData::GetCombatMaxSpellDamage( void ) const
{
	return combatmaxspelldamage;
}

SI16 cServerData::GetServerTimeDay( void ) const
{
	return days;
}
void cServerData::SetServerTimeDay( SI16 nValue )
{
	days = nValue;
}
UI08 cServerData::GetServerTimeHours( void ) const
{
	return hours;
}
void cServerData::SetServerTimeHours( UI08 nValue )
{
	hours = nValue;
}
UI08 cServerData::GetServerTimeMinutes( void ) const
{
	return minutes;
}
void cServerData::SetServerTimeMinutes( UI08 nValue )
{
	minutes = nValue;
}
UI08 cServerData::GetServerTimeSeconds( void ) const
{
	return seconds;
}
void cServerData::SetServerTimeSeconds( UI08 nValue )
{
	seconds = nValue;
}
bool cServerData::GetServerTimeAMPM( void ) const
{
	return ampm;
}
void cServerData::SetServerTimeAMPM( bool nValue )
{
	ampm = nValue;
}

bool cServerData::IncSecond( void )
{
	seconds++;
	if( seconds == 60 )
	{
		seconds = 0;
		return IncMinute();
	}
	return false;
}

void cServerData::IncMoon( int mNumber )
{
	moon[mNumber] = (SI16)((moon[mNumber] + 1)%8);
}

bool cServerData::IncMinute( void )
{
	minutes++;
	if( minutes%8 == 0 )
		IncMoon( 0 );
	if( minutes%3 == 0 )
		IncMoon( 1 );

	if( minutes == 60 )
	{
		minutes = 0;
		return IncHour();
	}
	return false;
}

bool cServerData::IncHour( void )
{
	hours++;
	bool retVal = false;
	if( hours == 12 )
	{
		if( ampm )
			retVal = IncDay();
		hours = 0;
		ampm = !ampm;
	}
	return retVal;
}

bool cServerData::IncDay( void )
{
	days++;
	return true;
}

long cServerData::VersionCheck( const char *filename )
{
	long version = MAKEWORD( 0, 0 );
	string sFilename = filename;
	ifstream inFile( sFilename.c_str(), ios::in );
	if( !inFile.is_open() )
		return version;
	char szLine[129];
	inFile.getline( szLine, 128 );
	char *left = NULL, *right = NULL;
	left = strtok( szLine, "V" );
	right = strtok( NULL, "\0" );

	if( right == NULL )
	{
		if( atoi( szLine ) == 0 )
		{
			// ok passed first line.
			inFile.getline( szLine,128 );
			if( atoi( szLine ) > 0 )
			{
				// ok passed ip check
				inFile.getline( szLine,128 );
				if( atoi(szLine) > 0 )
				{
					// this is a port all is kewl. return 1.1
					return MAKEWORD( 1, 1 );
				}
				return MAKEWORD( 1, 0 );
			}
			return MAKEWORD( 1, 0 );
		}
	}
	return makeNum( right );
}

physicalServer *cServerData::GetServerEntry( UI16 entryNum )
{
	if( entryNum >= serverList.size() )
		return NULL;
	return &serverList[entryNum];
}
UI32 cServerData::GetServerCount( void ) const
{
	return serverList.size();
}

bool cServerData::ServerScriptSectionHeader( void ) const
{
	return srvScpSectHeader;
}

void cServerData::ServerScriptSectionHeader( bool value )
{
	srvScpSectHeader = value;
}


UI08 cServerData::SaveMode( void ) const
{
	return saveMode;
}

UI08 cServerData::SaveMode( UI08 NewMode )
{
	if( NewMode <= 0 || NewMode > 1 )
		saveMode = 0;
	else
		saveMode = 1;

	return SaveMode();
}
