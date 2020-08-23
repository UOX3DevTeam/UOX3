#include "uox3.h"
#include "scriptc.h"
#include "ssection.h"

#if UOX_PLATFORM != PLATFORM_WIN32
	#include <dirent.h>
#else
	#include <direct.h>
#endif

#if P_ODBC == 1
	#include "ODBCManager.h"
#endif

namespace UOX
{
#define	MAX_TRACKINGTARGETS	128
#define SKILLTOTALCAP		7000
#define SKILLCAP			1000
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
const UI32 BIT_LOOTINGISCRIME		= 27;
const UI32 BIT_BASICTOOLTIPSONLY	= 28;
const UI32 BIT_GLOBALITEMDECAY		= 29;
const UI32 BIT_SCRIPTITEMSDECAYABLE	= 30;
const UI32 BIT_BASEITEMSDECAYABLE	= 31;
const UI32 BIT_ITEMDECAYINHOUSES	= 32;
const UI32 BIT_PAPERDOLLGUILDBUTTON = 33;
const UI32 BIT_ATTSPEEDFROMSTAMINA	= 34;
const UI32 BIT_SHOWDAMAGENUMBERS	= 35;
const UI32 BIT_SERVERUSINGHSMULTIS	= 36;
const UI32 BIT_SERVERUSINGHSTILES	= 37;
const UI32 BIT_EXTENDEDSTARTINGSTATS	= 38;
const UI32 BIT_EXTENDEDSTARTINGSKILLS	= 39;
const UI32 BIT_ASSISTANTNEGOTIATION = 40;
const UI32 BIT_KICKONASSISTANTSILENCE = 41;
const UI32 BIT_CLASSICUOMAPTRACKER = 42;


// New uox3.ini format lookup
// (January 13, 2001 - EviLDeD) Modified: January 30, 2001 Converted to uppercase
// (February 26 2002 - EviLDeD) Modified: to support the AccountIsolation, and left out dir3ectory tags
// (September 22 2002 - EviLDeD) Added the  "HIDEWILEMOUNTED" tag to support Xuri hide fix
// (September 06 2003 - Maarc)	Removed unused tags (heartbeat, wilderness, uoxbot, lagfix)
// (October 16, 2003 - giwo)	Removed unused tag (SAVEMODE) and added "WEIGHTPERSTR".
// (April 3, 2004 = EviLDeD)	Added new tags, for UOG support, as well as new facet tags etc.
// (June 15, 2004 - EviLDeD)	Added the new tags for the xFTPd, and xFTPc support.
// (April 2, 2005 - giwo)		Removed superfluous tags (BEGGINGTIMER, HUNGERDAMAGERATE, HUNGERTHRESHOLD, BEGGINGRANGE)
// (September 13, 2005 - giwo)	Added MAPCOUNT to the uox.ini to specify the number of maps UOX3 uses.
// (November 20, 2005 - giwo) 	Removed USEFACETSAVES, MAP0, MAP1, MAP2, MAP3, USERMAP, FTPDENABLED,
//								FTPDUSER, FTPDUSERLIMIT, FTPDBIND, FTPDROOT, FTPCENABLED, FTPCHOST,
//								FTPCPORT, FTPCUSER, FTPCULFLAGS, and MAPCOUNT as they were unused.
//								Added OVERLOADPACKETS option to uox.ini to toggle JS packet handling.
// (December 13, 2005 - giwo)	Changed uox.ini entry SHOWHIDDENNPCS to SHOWOFFLINEPCS to better reflect its function.
//								Removed CACHEMULS from the uox.ini as it is no longer used.
// (February 12, 2006 - giwo)	Added NPCMOVEMENTSPEED to the UOX.ini to allow customization of NPC movement speeds.
// (July 7, 2006 - Xuri)		PLAYERPERSECUTION in uox.ini (allows ghosts to drain mana from other players) set to 0 by default
// (July 18, 2006 - giwo)		Added uox.ini flag to disable/enable the A* Pathfinding routine: ADVANCEDPATHFINDING=0/1
// (July 24, 2006 - giwo)		Simplified many entries in uox.ini, making them easier to read.
// (January 28, 2007 - giwo)	Moved dynamic parts of [worldlight] section from uox.ini to their own output file in the /shared/ directory
// (March 3, 2007 - giwo)		Removed CLIENTSUPPORT from the uox.ini
//								Allowed customization of supported client features via uox.ini
//									CLIENTFEATURES
//									SERVERFEATURES
// (April 9, 2007 - grimson)	Added a new setting to the uox.ini, LOOTINGISCRIME. If you set it to 0 looting corpses of innocent chars is not taken as a crime.
// (June 10, 2007 - grimson)	Added two new settings to the uox.ini, NPCRUNNINGSPEED and NPCFLEEINGSPEED, they work the same way as NPCMOVEMENTSPEED.
// (July 28, 2007 - grimson)	Added a new setting to the uox.ini: BASICTOOLTIPSONLY. If this is set to 1 the tooltips will only contain basic information, like the name and the weight of an item.
// (February 7, 2010 - Xuri)	Added new UOX.INI settings:
//									GLOBALITEMDECAY - Toggles on / off item decay on a global scale.Note that it does not remove the decay - status from items, it just resets the decay - timer when it reaches 0
//									SCRIPTITEMSDECAYABLE - Toggles whether DFN - items will decay by default or not.Can be overriden by DECAY tag in item - DFNs
//									BASEITEMSDECAYABLE - Toggles whether base - items will decay by default or not.Can be overriden by DECAY tag in harditems.dfn
//									ITEMDECAYINHOUSES - Toggles default decay of non - locked down items inside multis( houses and boats )
// (December 14, 20011 - Xuri)	Exposed CombatExplodeDelay to UOX.INI, and changed the default delay between targeting and damage for the Explosion spell from 0 to 2 seconds
// (March 19, 2012 - Xuri)		Added support for packet 0xD7, SubCommand 0x28 - Guild button on paperdoll, which gives access to guild-functions if character belongs to a guild.
//									Can be enabled / disabled through UOX.INI setting PAPERDOLLGUILDBUTTON = 0 / 1. Defaults to 0.
//								Added new UOX.INI option to determine whether attack - speed bonuses are gained from Stamina( default ) or Dexterity: ATTACKSPEEDFROMSTAMINA = 1 / 0
//								Added new UOX.INI option to control the displaying of damage - numbers in combat( previously used DISPLAYHITMSG for numbers too ) : DISPLAYDAMAGENUMBERS = 1 / 0
//								Added a new section to UOX.INI - [clientsupport] - along with the following settings to determine approved client - versions for the server :
//									CLIENTSUPPORT4000 = 0 / 1
//									CLIENTSUPPORT5000 = 0 / 1
//									CLIENTSUPPORT6000 = 0 / 1
//									CLIENTSUPPORT6050 = 0 / 1
//									CLIENTSUPPORT7000 = 0 / 1
//									CLIENTSUPPORT7090 = 0 / 1
//									CLIENTSUPPORT70160 = 0 / 1
//									CLIENTSUPPORT70240 = 0 / 1
//									NOTE: Each of these settings represent a range of clients, not just the individual versions mentioned.This means that CLIENTSUPPORT4000, for instance,
//									will allow or disallow connections from clients 4.0.0 to 4.0.11f.Also note that while it is possible to enable support for all clients at the same time,
//									it highly recommended to restrict support for client versions that match up to what the server is running.
//								Added new UOX.INI option : EXTENDEDSTARTINGSTATS = 0 / 1
//									If enabled, makes new characters start with 90 statpoints( selectable in clients 7.0.16.0 + only, lower versions only get 90 if using templates ) instead of 80.
//								Added new UOX.INI option : EXTENDEDSTARTINGSKILLS = 0 / 1
//									If enabled, allows for four starting skills( selectable in clients 7.0.16.0 + only, lower versions only get 4th skill if using templates ) instead of three
// (November 14, 2012 - Xuri)	Fixed issue where DISPLAYDAMAGENUMBERS and ATTACKSPEEDFROMSTAMINA settings in UOX.INI were overwriting the DISPLAYHITMSG setting instead of their own respective settings
// (November 10, 2013 - Xuri)	Added new UOX.INI settings for adjusting combat-speed on a global scale:
//									GLOBALATTACKSPEED = 1 	// for adjusting speed of melee and ranged attacks globally for all chars
//									NPCSPELLCASTSPEED = 1 	// for adjusting the overall speed of spellcasts for NPCs (base spellcast speed determined by SPADELAY tag in NPC dfns)
//								Added new UOX.INI settings to make hardcoded damage to armor and weapons in combat more customizable :
//									WEAPONDAMAGECHANCE = 17 	// Chance of weapons taking damage when attacking
//									WEAPONDAMAGEMIN = 0		// Minimum amount of damage a weapon takes if damaged in combat
//									WEAPONDAMAGEMAX = 1		// Maximum amount of damage a weapon takes if damaged in combat
//									ARMORDAMAGECHANCE = 25	// Chance of armor taking damage when defending
//									ARMORDAMAGEMIN = 0		// Minimum amount of damage armor can take if damaged in combat
//									ARMORDAMAGEMAX = 1		// Maximum amount of damage armor can take if damaged in combat
// (September 12, 2015 - Xuri)	Added new UOX.INI setting for defining the amount of stamina lost when using the fishing skill
//									FISHINGSTAMINALOSS = 2	// The amount of stamina lost when using the fishing skill
// (November 24, 2019 - Xuri)	Added new uox.ini flag for choosing a random starting location for new players (from list in uox.ini)
//									RANDOMSTARTINGLOCATION = 0	// 0 to disable (default), 1 to enable

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
	"ANIMALATTACKCHANCE|ANIMALSGUARDED|NPCDAMAGERATE|NPCBASEFLEEAT|NPCBASEREATTACKAT|ATTACKSTAMINA|LOCATION|STARTGOLD|STARTPRIVS|ESCORTDONEEXPIRE|DARKLEVEL|"
	"TITLECOLOUR|LEFTTEXTCOLOUR|RIGHTTEXTCOLOUR|BUTTONCANCEL|BUTTONLEFT|BUTTONRIGHT|BACKGROUNDPIC|POLLTIME|MAYORTIME|TAXPERIOD|GUARDSPAID|DAY|HOURS|MINUTES|SECONDS|AMPM|SKILLLEVEL|SNOOPISCRIME|BOOKSDIRECTORY|SERVERLIST|PORT|"
	"ACCESSDIRECTORY|LOGSDIRECTORY|ACCOUNTISOLATION|HTMLDIRECTORY|SHOOTONANIMALBACK|NPCTRAININGENABLED|DICTIONARYDIRECTORY|BACKUPSAVERATIO|HIDEWILEMOUNTED|SECONDSPERUOMINUTE|WEIGHTPERSTR|POLYDURATION|"
	"UOGENABLED|NETRCVTIMEOUT|NETSNDTIMEOUT|NETRETRYCOUNT|CLIENTFEATURES|OVERLOADPACKETS|NPCMOVEMENTSPEED|PETHUNGEROFFLINE|PETOFFLINETIMEOUT|PETOFFLINECHECKTIMER|ARCHERRANGE|ADVANCEDPATHFINDING|SERVERFEATURES|LOOTINGISCRIME|"
	"NPCRUNNINGSPEED|NPCFLEEINGSPEED|BASICTOOLTIPSONLY|GLOBALITEMDECAY|SCRIPTITEMSDECAYABLE|BASEITEMSDECAYABLE|ITEMDECAYINHOUSES|COMBATEXPLODEDELAY|PAPERDOLLGUILDBUTTON|ATTACKSPEEDFROMSTAMINA|DISPLAYDAMAGENUMBERS|"
	"CLIENTSUPPORT4000|CLIENTSUPPORT5000|CLIENTSUPPORT6000|CLIENTSUPPORT6050|CLIENTSUPPORT7000|CLIENTSUPPORT7090|CLIENTSUPPORT70160|CLIENTSUPPORT70240|CLIENTSUPPORT70300|CLIENTSUPPORT70331|CLIENTSUPPORT704565|CLIENTSUPPORT70610|EXTENDEDSTARTINGSTATS|EXTENDEDSTARTINGSKILLS|WEAPONDAMAGECHANCE|"
	"ARMORDAMAGECHANCE|WEAPONDAMAGEMIN|WEAPONDAMAGEMAX|ARMORDAMAGEMIN|ARMORDAMAGEMAX|GLOBALATTACKSPEED|NPCSPELLCASTSPEED|FISHINGSTAMINALOSS|RANDOMSTARTINGLOCATION|ASSISTANTNEGOTIATION|KICKONASSISTANTSILENCE|"
	"AF_FILTERWEATHER|AF_FILTERLIGHT|AF_SMARTTARGET|AF_RANGEDTARGET|AF_AUTOOPENDOORS|AF_DEQUIPONCAST|AF_AUTOPOTIONEQUIP|AF_POISONEDCHECKS|AF_LOOPEDMACROS|AF_USEONCEAGENT|AF_RESTOCKAGENT|"
	"AF_SELLAGENT|AF_BUYAGENT|AF_POTIONHOTKEYS|AF_RANDOMTARGETS|AF_CLOSESTTARGETS|AF_OVERHEADHEALTH|AF_AUTOLOOTAGENT|AF_BONECUTTERAGENT|AF_JSCRIPTMACROS|AF_AUTOREMOUNT|AF_ALL|CLASSICUOMAPTRACKER|"
	"ODBCDSN|ODBCUSER|ODBCPASS|"
);

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void ResetDefaults( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Reset server settings to default
//o-----------------------------------------------------------------------------------------------o
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
	CombatExplodeDelay( 2 );

	// load defaults values
	SystemTimer( tSERVER_SHOPSPAWN, 300 );
	SystemTimer( tSERVER_POISON, 180 );
	SystemTimer( tSERVER_DECAY, 300 );
	ServerSkillTotalCap( 7000 );
	ServerSkillCap( 1000 );
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
	MineCheck( 1 );
	DeathAnimationStatus( true );
	ShowOfflinePCs( true );
	CombatDisplayHitMessage( true );
	CombatDisplayDamageNumbers( true );
	CombatAttackSpeedFromStamina( true );
	CombatAttackStamina( -2 );
	NPCTrainingStatus( true );
	CharHideWhileMounted( true );
	WeightPerStr( 3.5 );
	SystemTimer( tSERVER_POLYMORPH, 90 );
	ServerOverloadPackets( true );
	AdvancedPathfinding( true );
	LootingIsCrime( true );

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
	CombatWeaponDamageChance( 17 );
	CombatWeaponDamageMin( 0 );
	CombatWeaponDamageMax( 1 );
	CombatArmorDamageChance( 33 );
	CombatArmorDamageMin( 0 );
	CombatArmorDamageMax( 1 );
	GlobalAttackSpeed( 1.0 );
	NPCSpellCastSpeed( 1.0 );
	FishingStaminaLoss( 2.0 );

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

	CheckBoatSpeed( 0.65 );
	CheckNpcAISpeed( 1 );
	CutScrollRequirementStatus( true );
	PlayerPersecutionStatus( false );
	HtmlStatsStatus( -1 );

	MsgBoardPostingLevel( 0 );
	MsgBoardPostRemovalLevel( 0 );
	// No replacement I can see
	EscortsEnabled( true );
	BasicTooltipsOnly( false );
	GlobalItemDecay( true );
	ScriptItemsDecayable( true );
	BaseItemsDecayable( false );
	ItemDecayInHouses( false );
	SystemTimer( tSERVER_ESCORTWAIT, 900 );
	SystemTimer( tSERVER_ESCORTACTIVE, 600 );
	SystemTimer( tSERVER_ESCORTDONE, 600 );
	AmbientFootsteps( false );
	ServerCommandPrefix( '\'' );

	CheckSpawnRegionSpeed( 30 );
	CheckItemsSpeed( 1.5 );
	NPCWalkingSpeed( 0.5 );
	NPCRunningSpeed( 0.2 );
	NPCFleeingSpeed( 0.4 );
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
	SetClientFeature( CF_BIT_UOR, true );
	SetClientFeature( CF_BIT_TD, true );
	SetClientFeature( CF_BIT_LBR, true );
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

	SetDisabledAssistantFeature( AF_NONE, true );
	SetDisabledAssistantFeature( AF_FILTERWEATHER, false );
	SetDisabledAssistantFeature( AF_FILTERLIGHT, false );
	SetDisabledAssistantFeature( AF_SMARTTARGET, false );
	SetDisabledAssistantFeature( AF_RANGEDTARGET, false );
	SetDisabledAssistantFeature( AF_AUTOOPENDOORS, false );
	SetDisabledAssistantFeature( AF_DEQUIPONCAST, false );
	SetDisabledAssistantFeature( AF_AUTOPOTIONEQUIP, false );
	SetDisabledAssistantFeature( AF_POISONEDCHECKS, false );
	SetDisabledAssistantFeature( AF_LOOPEDMACROS, false );
	SetDisabledAssistantFeature( AF_USEONCEAGENT, false );
	SetDisabledAssistantFeature( AF_RESTOCKAGENT, false );
	SetDisabledAssistantFeature( AF_SELLAGENT, false );
	SetDisabledAssistantFeature( AF_BUYAGENT, false );
	SetDisabledAssistantFeature( AF_POTIONHOTKEYS, false );
	SetDisabledAssistantFeature( AF_RANDOMTARGETS, false );
	SetDisabledAssistantFeature( AF_CLOSESTTARGETS, false );
	SetDisabledAssistantFeature( AF_OVERHEADHEALTH, false );
	SetDisabledAssistantFeature( AF_AUTOLOOTAGENT, false );
	SetDisabledAssistantFeature( AF_BONECUTTERAGENT, false );
	SetDisabledAssistantFeature( AF_JSCRIPTMACROS, false );
	SetDisabledAssistantFeature( AF_AUTOREMOUNT, false );
	SetDisabledAssistantFeature( AF_ALL, false );

	// Enable login-support for any supported client version by default.
	ClientSupport4000( false );
	ClientSupport5000( false );
	ClientSupport6000( false );
	ClientSupport6050( false );
	ClientSupport7000( false );
	ClientSupport7090( false );
	ClientSupport70160( false );
	ClientSupport70240( false );
	ClientSupport70300( false );
	ClientSupport70331( true );
	ClientSupport704565( true );
	ClientSupport70610( true );

	ExtendedStartingStats( true );
	ExtendedStartingSkills( true );

	ServerRandomStartingLocation( false );
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void RefreshIPs( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Refresh IPs for servers in serverlist
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	std::string ServerName( void ) const
//|					void ServerName( std::string setname )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets server name. Sets to specified value, or to default name if no value specified
//o-----------------------------------------------------------------------------------------------o
std::string CServerData::ServerName( void ) const
{
	return serverList[0].getName();
}
void CServerData::ServerName( std::string setname )
{
	if( serverList.empty() )
		serverList.resize( 1 );
	serverList[0].setName( setname );
	if( setname.empty() )
	{
		serverList[0].setName( "Default UOX3 Server" );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	std::string ServerDomain( void ) const
//|					void ServerDomain( std::string setdomain )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets server domain. Sets to specified value, or to no domain if no value specified
//o-----------------------------------------------------------------------------------------------o
std::string CServerData::ServerDomain( void ) const
{
	return serverList[0].getDomain();
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	std::string CServerData::ServerIP( void ) const
//|					void ServerIP( std::string setip )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets server IP. Sets to specified value, or to loopback IP if no value specified
//o-----------------------------------------------------------------------------------------------o
std::string CServerData::ServerIP( void ) const
{
	return serverList[0].getIP();
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ServerPort( void ) const
//|					void ServerPort( UI16 setport )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Set server port to specified value, or to default port if no value specified
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::ServerPort( void ) const
{
	return port;
}
void CServerData::ServerPort( UI16 setport )
{
	if( setport == 0 )
		port = 2593;
	else
		port = setport;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 ServerConsoleLogStatus( void ) const
//|					void ServerConsoleLog( UI08 setting )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Set console log enabled/disabled state
//o-----------------------------------------------------------------------------------------------o
UI08 CServerData::ServerConsoleLogStatus( void ) const
{
	return consolelogenabled;
}
void CServerData::ServerConsoleLog( UI08 setting )
{
	consolelogenabled = setting;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 ServerCrashProtectionStatus( void ) const
//|					void ServerCrashProtection( UI08 setting )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Set crash protection enabled/disabled state
//o-----------------------------------------------------------------------------------------------o
UI08 CServerData::ServerCrashProtectionStatus( void ) const
{
	return crashprotectionenabled;
}
void CServerData::ServerCrashProtection( UI08 setting )
{
	crashprotectionenabled = (setting > 5 && !(setting < 1)) ? 1 : setting;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	char ServerCommandPrefix( void ) const
//|					void ServerCommandPrefix( char cmdvalue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Set command prefix
//o-----------------------------------------------------------------------------------------------o
char CServerData::ServerCommandPrefix( void ) const
{
	return commandprefix;
}
void CServerData::ServerCommandPrefix( char cmdvalue )
{
	commandprefix = cmdvalue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ServerAnnounceSavesStatus( void ) const
//|					void ServerAnnounceSaves( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Set status of server accouncements for worldsaves
//o-----------------------------------------------------------------------------------------------o
bool CServerData::ServerAnnounceSavesStatus( void ) const
{
	return boolVals.test( BIT_ANNOUNCESAVES );
}
void CServerData::ServerAnnounceSaves( bool newVal )
{
	boolVals.set( BIT_ANNOUNCESAVES, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ServerJoinPartAnnouncementsStatus( void ) const
//|					void ServerJoinPartAnnouncements( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets status of server accouncements for players connecting/disconnecting
//o-----------------------------------------------------------------------------------------------o
bool CServerData::ServerJoinPartAnnouncementsStatus( void ) const
{
	return boolVals.test( BIT_ANNOUNCEJOINPART );
}
void CServerData::ServerJoinPartAnnouncements( bool newVal )
{
	boolVals.set( BIT_ANNOUNCEJOINPART, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ServerBackupStatus( void ) const
//|					void ServerBackups( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets enabled status of server worldsave backups 
//o-----------------------------------------------------------------------------------------------o
bool CServerData::ServerBackupStatus( void ) const
{
	return boolVals.test( BIT_SERVERBACKUP );
}
void CServerData::ServerBackups( bool newVal )
{
	boolVals.set( BIT_SERVERBACKUP, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 ServerSavesTimerStatus( void ) const
//|					void ServerSavesTimer( UI32 timer )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets server world save timer
//o-----------------------------------------------------------------------------------------------o
UI32 CServerData::ServerSavesTimerStatus( void ) const
{
	return serversavestimer;
}
void CServerData::ServerSavesTimer( UI32 timer )
{
	serversavestimer = timer;
	if( timer < 180 )			// 3 minutes is the lowest value you can set saves for
		serversavestimer = 300;	// 10 minutes default
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ServerSkillCapStatus( void ) const
//|					void ServerSkillCap( UI16 cap )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets cap for individual skills
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::ServerSkillCapStatus( void ) const
{
	return skillcap;
}
void CServerData::ServerSkillCap( UI16 cap )
{
	skillcap = cap;
	if( cap < 1 )		// Default is on second loop sleeping
		skillcap = SKILLCAP;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ServerSkillTotalCapStatus( void ) const
//|					void ServerSkillTotalCap( UI16 cap )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets total cap for all skills
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::ServerSkillTotalCapStatus( void ) const
{
	return skilltotalcap;
}
void CServerData::ServerSkillTotalCap( UI16 cap )
{
	skilltotalcap = cap;
	if( cap < 1 )		// Default is on second loop sleeping
		skilltotalcap = SKILLTOTALCAP;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 ServerSkillDelayStatus( void ) const
//|					void ServerSkillDelay( UI08 skdelay )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global delay for skill usage
//o-----------------------------------------------------------------------------------------------o
UI08 CServerData::ServerSkillDelayStatus( void ) const
{
	return skilldelay;
}
void CServerData::ServerSkillDelay( UI08 skdelay )
{
	skilldelay = skdelay;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ServerStatCapStatus( void ) const
//|					void ServerStatCap( UI16 cap )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the total stat cap
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::ServerStatCapStatus( void ) const
{
	return statcap;
}
void CServerData::ServerStatCap( UI16 cap )
{
	statcap = cap;
	if( cap < 1 )		// Default is on second loop sleeping
		statcap = STATCAP;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 MaxStealthMovement( void ) const
//|					void MaxStealthMovement( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max number of steps allowed to take while stealthed
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::MaxStealthMovement( void ) const
{
	return maxstealthmovement;
}
void CServerData::MaxStealthMovement( SI16 value )
{
	maxstealthmovement = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 MaxStaminaMovement( void ) const
//|					void MaxStaminaMovement( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max number of steps allowed while running before stamina is reduced
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::MaxStaminaMovement( void ) const
{
	return maxstaminamovement;
}
void CServerData::MaxStaminaMovement( SI16 value )
{
	maxstaminamovement = value;
}

TIMERVAL CServerData::BuildSystemTimeValue( cSD_TID timerID ) const
{
	return BuildTimeValue( static_cast<R32>(SystemTimer( timerID )) );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 SystemTimer( cSD_TID timerid ) const
//|					void SystemTimer( cSD_TID timerid, UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets a specific server timer 
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::SystemTimer( cSD_TID timerid ) const
{
	return serverTimers[timerid];
}
void CServerData::SystemTimer( cSD_TID timerid, UI16 value )
{
	serverTimers[timerid] = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	std::string Directory( CSDDirectoryPaths dp )
//|					void Directory( CSDDirectoryPaths dp, std::string value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets directory paths
//o-----------------------------------------------------------------------------------------------o
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
			SI32 iResult = _chdir( sText.c_str() );
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ShootOnAnimalBack( void ) const
//|					void ShootOnAnimalBack( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether players can shoot arrows while riding mounts
//o-----------------------------------------------------------------------------------------------o
bool CServerData::ShootOnAnimalBack( void ) const
{
	return boolVals.test( BIT_SHOOTONANIMALBACK );
}
void CServerData::ShootOnAnimalBack( bool newVal )
{
	boolVals.set( BIT_SHOOTONANIMALBACK, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool NPCTrainingStatus( void ) const
//|					void NPCTrainingStatus( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether NPCs can train players in skills
//o-----------------------------------------------------------------------------------------------o
bool CServerData::NPCTrainingStatus( void ) const
{
	return boolVals.test( BIT_NPCTRAINING );
}
void CServerData::NPCTrainingStatus( bool newVal )
{
	boolVals.set( BIT_NPCTRAINING, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void dumpPaths( void )
//|	Date		-	02/26/2002
//|	Programmer	-	EviLDeD
//|	Org/Team	-	UOX3 DevTeam
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Needed a function that would dump out the paths. If you add any paths to the 
//|					server please make sure that you place exports to the console here to please
//|					 so it can be looked up if needed.
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CorpseLootDecay( void ) const
//|					void CorpseLootDecay( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether loot decays along with corpses or is left on ground
//o-----------------------------------------------------------------------------------------------o
bool CServerData::CorpseLootDecay( void ) const
{
	return boolVals.test( BIT_LOOTDECAYSONCORPSE );
}
void CServerData::CorpseLootDecay( bool newVal )
{
	boolVals.set( BIT_LOOTDECAYSONCORPSE, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GuardsStatus( void ) const
//|					void GuardStatus( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether guards are enabled globally or not
//o-----------------------------------------------------------------------------------------------o
bool CServerData::GuardsStatus( void ) const
{
	return boolVals.test( BIT_GUARDSENABLED );
}
void CServerData::GuardStatus( bool newVal )
{
	boolVals.set( BIT_GUARDSENABLED, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool DeathAnimationStatus( void ) const
//|					void DeathAnimationStatus( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether death animation plays when a player dies
//o-----------------------------------------------------------------------------------------------o
bool CServerData::DeathAnimationStatus( void ) const
{
	return boolVals.test( BIT_PLAYDEATHANIMATION );
}
void CServerData::DeathAnimationStatus( bool newVal )
{
	boolVals.set( BIT_PLAYDEATHANIMATION, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 WorldAmbientSounds( void ) const
//|					void WorldAmbientSounds( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets intensity of ambient world sounds, bird chirps, animal sounds, etc
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::WorldAmbientSounds( void ) const
{
	return ambientsounds;
}
void CServerData::WorldAmbientSounds( SI16 value )
{
	if( value < 0 )
		ambientsounds = 0;
	else
		ambientsounds = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool AmbientFootsteps( void ) const
//|					void AmbientFootsteps( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether context-specific footstep sounds are enabled or not
//o-----------------------------------------------------------------------------------------------o
bool CServerData::AmbientFootsteps( void ) const
{
	return boolVals.test( BIT_AMBIENTFOOTSTEPS );
}
void CServerData::AmbientFootsteps( bool newVal )
{
	boolVals.set( BIT_AMBIENTFOOTSTEPS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool InternalAccountStatus( void ) const
//|					void InternalAccountStatus( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether automatic account creation is enabled
//o-----------------------------------------------------------------------------------------------o
bool CServerData::InternalAccountStatus( void ) const
{
	return boolVals.test( BIT_INTERNALACCOUNTS );
}
void CServerData::InternalAccountStatus( bool newVal )
{
	boolVals.set( BIT_INTERNALACCOUNTS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ShowOfflinePCs( void ) const
//|					void ShowOfflinePCs( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether GMs can see offline players
//o-----------------------------------------------------------------------------------------------o
bool CServerData::ShowOfflinePCs( void ) const
{
	return boolVals.test( BIT_SHOWOFFLINEPCS );
}
void CServerData::ShowOfflinePCs( bool newVal )
{
	boolVals.set( BIT_SHOWOFFLINEPCS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool RogueStatus( void ) const
//|					void RogueStatus( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether stealing skill is enabled
//o-----------------------------------------------------------------------------------------------o
bool CServerData::RogueStatus( void ) const
{
	return boolVals.test( BIT_ROGUESTATUS );
}
void CServerData::RogueStatus( bool newVal )
{
	boolVals.set( BIT_ROGUESTATUS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool SnoopIsCrime( void ) const
//|					void SnoopIsCrime( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether snooping is considered a crime
//o-----------------------------------------------------------------------------------------------o
bool CServerData::SnoopIsCrime( void ) const
{
	return boolVals.test( BIT_SNOOPISCRIME );
}
void CServerData::SnoopIsCrime( bool newVal )
{
	boolVals.set( BIT_SNOOPISCRIME, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ExtendedStartingStats( void ) const
//|					void ExtendedStartingStats( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether extended starting stats are enabled
//|					If enabled players start with 90 total statpoints instead of 80
//o-----------------------------------------------------------------------------------------------o
bool CServerData::ExtendedStartingStats( void ) const
{
	return boolVals.test( BIT_EXTENDEDSTARTINGSTATS );
}
void CServerData::ExtendedStartingStats( bool newVal )
{
	boolVals.set( BIT_EXTENDEDSTARTINGSTATS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ExtendedStartingSkills( void ) const
//|					void ExtendedStartingSkills( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether extended starting skills are enabled
//|					If enabled, players start with 4 skills instead of 3, and have a total of
//|					120 skillpoints at the start instead of 100
//o-----------------------------------------------------------------------------------------------o
bool CServerData::ExtendedStartingSkills( void ) const
{
	return boolVals.test( BIT_EXTENDEDSTARTINGSKILLS );
}
void CServerData::ExtendedStartingSkills( bool newVal )
{
	boolVals.set( BIT_EXTENDEDSTARTINGSKILLS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool PlayerPersecutionStatus( void ) const
//|					void PlayerPersecutionStatus( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether player ghosts can drain mana from other players by attacking them
//o-----------------------------------------------------------------------------------------------o
bool CServerData::PlayerPersecutionStatus( void ) const
{
	return boolVals.test( BIT_PERSECUTIONSTATUS );
}
void CServerData::PlayerPersecutionStatus( bool newVal )
{
	boolVals.set( BIT_PERSECUTIONSTATUS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 HtmlStatsStatus( void ) const
//|					void HtmlStatsStatus( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether html stats are enabled or disabled
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::HtmlStatsStatus( void ) const
{
	return htmlstatusenabled;
}
void CServerData::HtmlStatsStatus( SI16 value )
{
	htmlstatusenabled = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool SellByNameStatus( void ) const
//|					void SellByNameStatus( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether items are sold by their name, not just ID/Colour
//o-----------------------------------------------------------------------------------------------o
bool CServerData::SellByNameStatus( void ) const
{
	return boolVals.test( BIT_SELLBYNAME );
}
void CServerData::SellByNameStatus( bool newVal )
{
	boolVals.set( BIT_SELLBYNAME, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 SellMaxItemsStatus( void ) const
//|					void SellMaxItemsStatus( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum amount of items that can be sold in one go to a vendor
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::SellMaxItemsStatus( void ) const
{
	return sellmaxitems;
}
void CServerData::SellMaxItemsStatus( SI16 value )
{
	sellmaxitems = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool TradeSystemStatus( void ) const
//|					void TradeSystemStatus( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether the advanced trade system is enabled or not.
//|					If enabled, prices of goods with NPC vendors will fluctuate with demand
//o-----------------------------------------------------------------------------------------------o
bool CServerData::TradeSystemStatus( void ) const
{
	return boolVals.test( BIT_TRADESYSSTATUS );
}
void CServerData::TradeSystemStatus( bool newVal )
{
	boolVals.set( BIT_TRADESYSSTATUS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool RankSystemStatus( void ) const
//|					void RankSystemStatus( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether rank system is enabled or not. If enabled, it allows for
//|					variable quality of crafted items
//o-----------------------------------------------------------------------------------------------o
bool CServerData::RankSystemStatus( void ) const
{
	return boolVals.test( BIT_RANKSYSSTATUS );
}
void CServerData::RankSystemStatus( bool newVal )
{
	boolVals.set( BIT_RANKSYSSTATUS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CutScrollRequirementStatus( void ) const
//|					void CutScrollRequirementStatus( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether skill requirements are cut when casting spells from scrolls
//o-----------------------------------------------------------------------------------------------o
bool CServerData::CutScrollRequirementStatus( void ) const
{
	return boolVals.test( BIT_CUTSCROLLREQ );
}
void CServerData::CutScrollRequirementStatus( bool newVal )
{
	boolVals.set( BIT_CUTSCROLLREQ, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R64 CheckItemsSpeed( void ) const
//|					void CheckItemsSpeed( R64 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets how often in seconds items are checked for decay and other things
//o-----------------------------------------------------------------------------------------------o
R64 CServerData::CheckItemsSpeed( void ) const
{
	return checkitems;
}
void CServerData::CheckItemsSpeed( R64 value )
{
	if( value < 0.0 )
		checkitems = 0.0;
	else
		checkitems = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R64 CheckBoatSpeed( void ) const
//|					void CheckBoatSpeed( R64 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets how often in seconds boats are checked for decay and other things
//o-----------------------------------------------------------------------------------------------o
R64 CServerData::CheckBoatSpeed( void ) const
{
	return checkboats;
}
void CServerData::CheckBoatSpeed( R64 value )
{
	if( value < 0.0 )
		checkboats = 0.0;
	else
		checkboats = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R64 CheckNpcAISpeed( void ) const
//|					void CheckNpcAISpeed( R64 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets how often in seconds NPCs will execute their AI routines
//o-----------------------------------------------------------------------------------------------o
R64 CServerData::CheckNpcAISpeed( void ) const
{
	return checknpcai;
}
void CServerData::CheckNpcAISpeed( R64 value )
{
	if( value < 0.0 )
		checknpcai = 0.0;
	else
		checknpcai = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R64 CheckSpawnRegionSpeed( void ) const
//|					void CheckSpawnRegionSpeed( R64 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets how often (in seconds) spawn regions are checked for new spawns
//o-----------------------------------------------------------------------------------------------o
R64 CServerData::CheckSpawnRegionSpeed( void ) const
{
	return checkspawnregions;
}
void CServerData::CheckSpawnRegionSpeed( R64 value )
{
	if( value < 0.0 )
		checkspawnregions = 0.0;
	else
		checkspawnregions = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 MsgBoardPostingLevel( void ) const
//|					void MsgBoardPostingLevel( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the posting level for bulletin boards
//|					UNUSED?
//o-----------------------------------------------------------------------------------------------o
UI08 CServerData::MsgBoardPostingLevel( void ) const
{
	return msgpostinglevel;
}
void CServerData::MsgBoardPostingLevel( UI08 value )
{
	msgpostinglevel = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 MsgBoardPostRemovalLevel( void ) const
//|					void MsgBoardPostRemovalLevel( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the post removal level for bulleting boards
//|					UNUSED?
//o-----------------------------------------------------------------------------------------------o
UI08 CServerData::MsgBoardPostRemovalLevel( void ) const
{
	return msgremovallevel;
}
void CServerData::MsgBoardPostRemovalLevel( UI08 value )
{
	msgremovallevel = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 CServerData::MineCheck( void ) const
//|					void CServerData::MineCheck( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the type of check used when players attempt to use the mining skill
//|						0 = mine anywhere
//|						1 = mine mountainsides/cavefloors only
//|						2 = mine in mining regions only (not working?)
//o-----------------------------------------------------------------------------------------------o
UI08 CServerData::MineCheck( void ) const
{
	return minecheck;
}
void CServerData::MineCheck( UI08 value )
{
	minecheck = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CombatDisplayHitMessage( void ) const
//|					void CombatDisplayHitMessage( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether combat hit messages are displayed in combat
//o-----------------------------------------------------------------------------------------------o
bool CServerData::CombatDisplayHitMessage( void ) const
{
	return boolVals.test( BIT_SHOWHITMESSAGE );
}
void CServerData::CombatDisplayHitMessage( bool newVal )
{
	boolVals.set( BIT_SHOWHITMESSAGE, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CombatDisplayDamageNumbers( void ) const
//|					void CombatDisplayDamageNumbers( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether combat damage numbers are displayed in combat
//o-----------------------------------------------------------------------------------------------o
bool CServerData::CombatDisplayDamageNumbers( void ) const
{
	return boolVals.test( BIT_SHOWDAMAGENUMBERS );
}
void CServerData::CombatDisplayDamageNumbers( bool newVal )
{
	boolVals.set( BIT_SHOWDAMAGENUMBERS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CombatAttackSpeedFromStamina( void ) const
//|					void CombatAttackSpeedFromStamina( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether combat attack speed is derived from stamina instead of dexterity
//o-----------------------------------------------------------------------------------------------o
bool CServerData::CombatAttackSpeedFromStamina( void ) const
{
	return boolVals.test( BIT_ATTSPEEDFROMSTAMINA );
}
void CServerData::CombatAttackSpeedFromStamina( bool newVal )
{
	boolVals.set( BIT_ATTSPEEDFROMSTAMINA, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 GlobalAttackSpeed( void ) const
//|					void GlobalAttackSpeed( R32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global attack speed in combat
//o-----------------------------------------------------------------------------------------------o
R32 CServerData::GlobalAttackSpeed( void ) const
{
	return globalattackspeed;
}
void CServerData::GlobalAttackSpeed( R32 value )
{
	if( value < 0.0 )
		globalattackspeed = 0.0;
	else
		globalattackspeed = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 NPCSpellCastSpeed( void ) const
//|					void NPCSpellCastSpeed( R32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global NPC spell casting speed
//o-----------------------------------------------------------------------------------------------o
R32 CServerData::NPCSpellCastSpeed( void ) const
{
	return npcspellcastspeed;
}
void CServerData::NPCSpellCastSpeed( R32 value )
{
	if( value < 0.0 )
		npcspellcastspeed = 0.0;
	else
		npcspellcastspeed = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 FishingStaminaLoss( void ) const
//|					void FishingStaminaLoss( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the stamina loss for using the fishing skill
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::FishingStaminaLoss( void ) const
{
	return fishingstaminaloss;
}
void CServerData::FishingStaminaLoss( SI16 value )
{
	fishingstaminaloss = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 CombatNPCDamageRate( void ) const
//|					void CombatNPCDamageRate( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the NPC damage divisor. If character is a player, damage from NPCs is
//|					divided by this value.
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::CombatNPCDamageRate( void ) const
{
	return combatnpcdamagerate;
}
void CServerData::CombatNPCDamageRate( SI16 value )
{
	combatnpcdamagerate = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ServerUsingHSMultis( void ) const
//|					void ServerUsingHSMultis( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether server uses multi data from High Seas expansion
//o-----------------------------------------------------------------------------------------------o
bool CServerData::ServerUsingHSMultis( void ) const
{
	return boolVals.test( BIT_SERVERUSINGHSMULTIS );
}
void CServerData::ServerUsingHSMultis( bool newVal )
{
	boolVals.set( BIT_SERVERUSINGHSMULTIS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ServerUsingHSTiles( void ) const
//|					void ServerUsingHSTiles( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether server uses tiledata from High Seas expansion
//o-----------------------------------------------------------------------------------------------o
bool CServerData::ServerUsingHSTiles( void ) const
{
	return boolVals.test( BIT_SERVERUSINGHSTILES );
}
void CServerData::ServerUsingHSTiles( bool newVal )
{
	boolVals.set( BIT_SERVERUSINGHSTILES, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 CombatAttackStamina( void ) const
//|					void CombatAttackStamina( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the amount of stamina lost by swinging weapon in combat
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::CombatAttackStamina( void ) const
{
	return combatattackstamina;
}
void CServerData::CombatAttackStamina( SI16 value )
{
	combatattackstamina = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 SkillLevel( void ) const
//|					void SkillLevel( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global difficulty level for crafting items
//|					1 = easy, 5 = default, 10 = difficult
//o-----------------------------------------------------------------------------------------------o
UI08 CServerData::SkillLevel( void ) const
{
	return skilllevel;
}
void CServerData::SkillLevel( UI08 value )
{
	skilllevel = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool EscortsEnabled( void ) const
//|					void EscortsEnabled( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether escorts are enabled or not
//o-----------------------------------------------------------------------------------------------o
bool CServerData::EscortsEnabled( void ) const
{
	return boolVals.test( BIT_ESCORTSTATUS );
}
void CServerData::EscortsEnabled( bool newVal )
{
	boolVals.set( BIT_ESCORTSTATUS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool BasicTooltipsOnly( void ) const
//|					void BasicTooltipsOnly( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether basic tooltips are enabled or not (instead of advanced tooltips)
//o-----------------------------------------------------------------------------------------------o
bool CServerData::BasicTooltipsOnly( void ) const
{
	return boolVals.test( BIT_BASICTOOLTIPSONLY );
}
void CServerData::BasicTooltipsOnly( bool newVal )
{
	boolVals.set( BIT_BASICTOOLTIPSONLY, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GlobalItemDecay( void ) const
//|					void GlobalItemDecay( bool newVal )
//|	Date		-	2/07/2010
//|	Programmer	-	Xuri
//|	Org/Team	-	UOX3 DevTeam
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether decay is enabled or disabled, on a global scale
//o-----------------------------------------------------------------------------------------------o
bool CServerData::GlobalItemDecay( void ) const
{
	return boolVals.test( BIT_GLOBALITEMDECAY );
}
void CServerData::GlobalItemDecay( bool newVal )
{
	boolVals.set( BIT_GLOBALITEMDECAY, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ScriptItemsDecayable( void ) const
//|					void ScriptItemsDecayable( bool newVal )
//|	Date		-	2/07/2010
//|	Programmer	-	Xuri
//|	Org/Team	-	UOX3 DevTeam
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether items added through scripts decay or not
//o-----------------------------------------------------------------------------------------------o
bool CServerData::ScriptItemsDecayable( void ) const
{
	return boolVals.test( BIT_SCRIPTITEMSDECAYABLE );
}
void CServerData::ScriptItemsDecayable( bool newVal )
{
	boolVals.set( BIT_SCRIPTITEMSDECAYABLE, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool BaseItemsDecayable( void ) const
//|					void BaseItemsDecayable( bool newVal )
//|	Date		-	2/07/2010
//|	Programmer	-	Xuri
//|	Org/Team	-	UOX3 DevTeam
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether base items added will decay or not
//o-----------------------------------------------------------------------------------------------o
bool CServerData::BaseItemsDecayable( void ) const
{
	return boolVals.test( BIT_BASEITEMSDECAYABLE );
}
void CServerData::BaseItemsDecayable( bool newVal )
{
	boolVals.set( BIT_BASEITEMSDECAYABLE, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ItemDecayInHouses( void ) const
//|					void ItemDecayInHouses( bool newVal )
//|	Date		-	2/07/2010
//|	Programmer	-	Xuri
//|	Org/Team	-	UOX3 DevTeam
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether items inside houses will decay or not
//o-----------------------------------------------------------------------------------------------o
bool CServerData::ItemDecayInHouses( void ) const
{
	return boolVals.test( BIT_ITEMDECAYINHOUSES );
}
void CServerData::ItemDecayInHouses( bool newVal )
{
	boolVals.set( BIT_ITEMDECAYINHOUSES, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool PaperdollGuildButton( void ) const
//|					void PaperdollGuildButton( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether guild menu can be accessed from paperdoll button or not
//o-----------------------------------------------------------------------------------------------o
bool CServerData::PaperdollGuildButton( void ) const
{
	return boolVals.test( BIT_PAPERDOLLGUILDBUTTON );
}
void CServerData::PaperdollGuildButton( bool newVal )
{
	boolVals.set( BIT_PAPERDOLLGUILDBUTTON, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CombatMonstersVsAnimals( void ) const
//|					void CombatMonstersVsAnimals( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether monsters will attack animals or not
//o-----------------------------------------------------------------------------------------------o
bool CServerData::CombatMonstersVsAnimals( void ) const
{
	return boolVals.test( BIT_MONSTERSVSANIMALS );
}
void CServerData::CombatMonstersVsAnimals( bool newVal )
{
	boolVals.set( BIT_MONSTERSVSANIMALS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 CombatAnimalsAttackChance( void ) const
//|					void CombatAnimalsAttackChance( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the chance of monsters attacking animals
//o-----------------------------------------------------------------------------------------------o
UI08 CServerData::CombatAnimalsAttackChance( void ) const
{
	return combatanimalattackchance;
}
void CServerData::CombatAnimalsAttackChance( UI08 value )
{
	if( value > 100 )
		value = 100;
	combatanimalattackchance = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 CombatWeaponDamageChance( void ) const
//|					void CombatWeaponDamageChance( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether combat will damage weapons or not
//o-----------------------------------------------------------------------------------------------o
UI08 CServerData::CombatWeaponDamageChance( void ) const
{
	return combatweapondamagechance;
}
void CServerData::CombatWeaponDamageChance( UI08 value )
{
	if( value > 100 )
		value = 100;
	combatweapondamagechance = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 CombatWeaponDamageMin( void ) const
//|					void CombatWeaponDamageMin( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the min amount of damage (in hitpoints) weapons will take from combat
//o-----------------------------------------------------------------------------------------------o
UI08 CServerData::CombatWeaponDamageMin( void ) const
{
	return combatweapondamagemin;
}
void CServerData::CombatWeaponDamageMin( UI08 value )
{
	combatweapondamagemin = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 CombatWeaponDamageMax( void ) const
//|					void CombatWeaponDamageMax( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max amount of damage (in hitpoints) weapons will take from combat
//o-----------------------------------------------------------------------------------------------o
UI08 CServerData::CombatWeaponDamageMax( void ) const
{
	return combatweapondamagemax;
}
void CServerData::CombatWeaponDamageMax( UI08 value )
{
	combatweapondamagemax = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 CombatArmorDamageChance( void ) const
//|					void CombatArmorDamageChance( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the chance to damage armor in combat
//o-----------------------------------------------------------------------------------------------o
UI08 CServerData::CombatArmorDamageChance( void ) const
{
	return combatarmordamagechance;
}
void CServerData::CombatArmorDamageChance( UI08 value )
{
	if( value > 100 )
		value = 100;
	combatarmordamagechance = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 CombatArmorDamageMin( void ) const
//|					void CombatArmorDamageMin( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the min damage dealt to armor in combat
//o-----------------------------------------------------------------------------------------------o
UI08 CServerData::CombatArmorDamageMin( void ) const
{
	return combatarmordamagemin;
}
void CServerData::CombatArmorDamageMin( UI08 value )
{
	combatarmordamagemin = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 CombatArmorDamageMax( void ) const
//|					void CombatArmorDamageMax( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max damage dealt to armor in combat
//o-----------------------------------------------------------------------------------------------o
UI08 CServerData::CombatArmorDamageMax( void ) const
{
	return combatarmordamagemax;
}
void CServerData::CombatArmorDamageMax( UI08 value )
{
	combatarmordamagemax = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 HungerDamage( void ) const
//|					void HungerDamage( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the damage taken from players being hungry
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::HungerDamage( void ) const
{
	return hungerdamage;
}
void CServerData::HungerDamage( SI16 value )
{
	hungerdamage = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 PetOfflineTimeout( void ) const
//|					void PetOfflineTimeout( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the interval in seconds between checks for the player offline time
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::PetOfflineTimeout( void ) const
{
	return petOfflineTimeout;
}
void CServerData::PetOfflineTimeout( UI16 value )
{
	petOfflineTimeout = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool PetHungerOffline( void ) const
//|					void PetHungerOffline( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether pets should hunger while the player (owner) is offline or not
//o-----------------------------------------------------------------------------------------------o
bool CServerData::PetHungerOffline( void ) const
{
	return boolVals.test( BIT_PETHUNGEROFFLINE );
}
void CServerData::PetHungerOffline( bool newVal )
{
	boolVals.set( BIT_PETHUNGEROFFLINE, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 BuyThreshold( void ) const
//|					void BuyThreshold( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the threshold for when money is taken from a player's bank account
//|					when buying something from a vendor instead of from their backpack
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::BuyThreshold( void ) const
{
	return buyThreshold;
}
void CServerData::BuyThreshold( SI16 value )
{
	buyThreshold = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CharHideWhileMounted( void ) const
//|					void CharHideWhileMounted( bool newVal )
//|	Date		-	09/22/2002
//|	Programmer	-	EviLDeD
//|	Org/Team	-	UOX3 DevTeam
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether players can hide while mounted or not
//o-----------------------------------------------------------------------------------------------o
bool CServerData::CharHideWhileMounted( void ) const
{
	return boolVals.test( BIT_HIDEWHILEMOUNTED );
}
void CServerData::CharHideWhileMounted( bool newVal )
{
	boolVals.set( BIT_HIDEWHILEMOUNTED, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 WeightPerStr( void ) const
//|					void WeightPerStr( R32 newVal )
//|	Date		-	3/12/2003
//|	Programmer	-	Zane
//|	Org/Team	-	UOX3 DevTeam
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets amount of weight one can hold per point of STR
//o-----------------------------------------------------------------------------------------------o
R32 CServerData::WeightPerStr( void ) const
{
	return weightPerSTR;
}
void CServerData::WeightPerStr( R32 newVal )
{
	weightPerSTR = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ServerOverloadPackets( void ) const
//|					void ServerOverloadPackets( bool newVal )
//|	Date		-	11/20/2005
//|	Programmer	-	giwo
//|	Org/Team	-	UOX3 DevTeam
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether packet handling in JS is enabled or disabled
//o-----------------------------------------------------------------------------------------------o
bool CServerData::ServerOverloadPackets( void ) const
{
	return boolVals.test( BIT_OVERLOADPACKETS );
}
void CServerData::ServerOverloadPackets( bool newVal )
{
	boolVals.set( BIT_OVERLOADPACKETS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ArmorAffectManaRegen( void ) const
//|					void ArmorAffectManaRegen( bool newVal )
//|	Date		-	3/20/2005
//|	Programmer	-	giwo
//|	Org/Team	-	UOX3 DevTeam
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether armor affects mana regeneration or not
//o-----------------------------------------------------------------------------------------------o
bool CServerData::ArmorAffectManaRegen( void ) const
{
	return boolVals.test( BIT_ARMORAFFECTMANAREGEN );
}
void CServerData::ArmorAffectManaRegen( bool newVal )
{
	boolVals.set( BIT_ARMORAFFECTMANAREGEN, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool AdvancedPathfinding( void ) const
//|					void AdvancedPathfinding( bool newVal )
//|	Date		-	7/16/2005
//|	Programmer	-	giwo
//|	Org/Team	-	UOX3 DevTeam
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether NPCs use the A* Pathfinding routine or not
//o-----------------------------------------------------------------------------------------------o
bool CServerData::AdvancedPathfinding( void ) const
{
	return boolVals.test( BIT_ADVANCEDPATHFIND );
}
void CServerData::AdvancedPathfinding( bool newVal )
{
	boolVals.set( BIT_ADVANCEDPATHFIND, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool LootingIsCrime( void ) const
//|					void LootingIsCrime( bool newVal )
//|	Date		-	4/09/2007
//|	Programmer	-	grimson
//|	Org/Team	-	UOX3 DevTeam
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether looting of corpses can be a crime or not
//o-----------------------------------------------------------------------------------------------o
bool CServerData::LootingIsCrime( void ) const
{
	return boolVals.test( BIT_LOOTINGISCRIME );
}
void CServerData::LootingIsCrime( bool newVal )
{
	boolVals.set( BIT_LOOTINGISCRIME, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 BackupRatio( void ) const
//|					void BackupRatio( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the ratio of worldsaves that get backed up
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::BackupRatio( void ) const
{
	return backupRatio;
}
void CServerData::BackupRatio( SI16 value )
{
	backupRatio = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 CombatMaxRange( void ) const
//|					void CombatMaxRange( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum range at which combat can be engaged
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::CombatMaxRange( void ) const
{
	return combatmaxrange;
}
void CServerData::CombatMaxRange( SI16 value )
{
	combatmaxrange = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 CombatArcherRange( void ) const
//|					void CombatArcherRange( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum range at which archery can be used in combat
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::CombatArcherRange( void ) const
{
	return combatarcherrange;
}
void CServerData::CombatArcherRange( SI16 value )
{
	combatarcherrange = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 CombatMaxSpellRange( void ) const
//|					void CombatMaxSpellRange( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum range at which spells can be cast in combat
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::CombatMaxSpellRange( void ) const
{
	return combatmaxspellrange;
}
void CServerData::CombatMaxSpellRange( SI16 value )
{
	combatmaxspellrange = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 CombatExplodeDelay( void ) const
//|					void CombatExplodeDelay( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the delay in seconds for the explosion spell to go into effect
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::CombatExplodeDelay( void ) const
{
	return combatExplodeDelay;
}
void CServerData::CombatExplodeDelay( UI16 value )
{
	combatExplodeDelay = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CombatAnimalsGuarded( void ) const
//|					void CombatAnimalsGuarded( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether animals are under the protection of town guards or not
//o-----------------------------------------------------------------------------------------------o
bool CServerData::CombatAnimalsGuarded( void ) const
{
	return boolVals.test( BIT_ANIMALSGUARDED );
}
void CServerData::CombatAnimalsGuarded( bool newVal )
{
	boolVals.set( BIT_ANIMALSGUARDED, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 CombatNPCBaseFleeAt( void ) const
//|					void CombatNPCBaseFleeAt( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global health threshold where NPCs start fleeing in combat
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::CombatNPCBaseFleeAt( void ) const
{
	return combatnpcbasefleeat;
}
void CServerData::CombatNPCBaseFleeAt( SI16 value )
{
	combatnpcbasefleeat = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 CombatNPCBaseReattackAt( void ) const
//|					void CombatNPCBaseReattackAt( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global health threshold where NPCs reattack after fleeing in combat
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::CombatNPCBaseReattackAt( void ) const
{
	return combatnpcbasereattackat;
}
void CServerData::CombatNPCBaseReattackAt( SI16 value )
{
	combatnpcbasereattackat = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 NPCWalkingSpeed( void ) const
//|					void NPCWalkingSpeed( R32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global, default walking speed for NPCs
//o-----------------------------------------------------------------------------------------------o
R32 CServerData::NPCWalkingSpeed( void ) const
{
	return npcWalkingSpeed;
}
void CServerData::NPCWalkingSpeed( R32 value )
{
	npcWalkingSpeed = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 NPCRunningSpeed( void ) const
//|					void NPCRunningSpeed( R32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global, default running speed for NPCs
//o-----------------------------------------------------------------------------------------------o
R32 CServerData::NPCRunningSpeed( void ) const
{
	return npcRunningSpeed;
}
void CServerData::NPCRunningSpeed( R32 value )
{
	npcRunningSpeed = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 CServerData::NPCFleeingSpeed( void ) const
//|					void CServerData::NPCFleeingSpeed( R32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global, default speed at which NPCs flee in combat
//o-----------------------------------------------------------------------------------------------o
R32 CServerData::NPCFleeingSpeed( void ) const
{
	return npcFleeingSpeed;
}
void CServerData::NPCFleeingSpeed( R32 value )
{
	npcFleeingSpeed = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 TitleColour( void ) const
//|					void TitleColour( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default text colour for titles in gumps
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::TitleColour( void ) const
{
	return titleColour;
}
void CServerData::TitleColour( UI16 value )
{
	titleColour = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 LeftTextColour( void ) const
//|					void LeftTextColour( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default text colour for left text in gumps (2 column ones)
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::LeftTextColour( void ) const
{
	return leftTextColour;
}
void CServerData::LeftTextColour( UI16 value )
{
	leftTextColour = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 RightTextColour( void ) const
//|					void RightTextColour( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default text colour for right text in gumps (2 column ones)
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::RightTextColour( void ) const
{
	return rightTextColour;
}
void CServerData::RightTextColour( UI16 value )
{
	rightTextColour = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ButtonCancel( void ) const
//|					void ButtonCancel( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default Button ID for cancel button in gumps
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::ButtonCancel( void ) const
{
	return buttonCancel;
}
void CServerData::ButtonCancel( UI16 value )
{
	buttonCancel = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ButtonLeft( void ) const
//|					void ButtonLeft( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default Button ID for left button (navigation) in gumps
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::ButtonLeft( void ) const
{
	return buttonLeft;
}
void CServerData::ButtonLeft( UI16 value )
{
	buttonLeft = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ButtonRight( void ) const
//|					void ButtonRight( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default Button ID for right button (navigation) in gumps
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::ButtonRight( void ) const
{
	return buttonRight;
}
void CServerData::ButtonRight( UI16 value )
{
	buttonRight = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 CServerData::BackgroundPic( void ) const
//|					void CServerData::BackgroundPic( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default Gump ID for background gump
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::BackgroundPic( void ) const
{
	return backgroundPic;
}
void CServerData::BackgroundPic( UI16 value )
{
	backgroundPic = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 TownNumSecsPollOpen( void ) const
//|					void TownNumSecsPollOpen( UI32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the time (in seconds) for which a town voting poll is open
//o-----------------------------------------------------------------------------------------------o
UI32 CServerData::TownNumSecsPollOpen( void ) const
{
	return numSecsPollOpen;
}
void CServerData::TownNumSecsPollOpen( UI32 value )
{
	numSecsPollOpen = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 TownNumSecsAsMayor( void ) const
//|					void TownNumSecsAsMayor( UI32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the time (in seconds) that a PC would be a mayor
//o-----------------------------------------------------------------------------------------------o
UI32 CServerData::TownNumSecsAsMayor( void ) const
{
	return numSecsAsMayor;
}
void CServerData::TownNumSecsAsMayor( UI32 value )
{
	numSecsAsMayor = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 TownTaxPeriod( void ) const
//|					void TownTaxPeriod( UI32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the time (in seconds) between periods of taxes for PCs
//o-----------------------------------------------------------------------------------------------o
UI32 CServerData::TownTaxPeriod( void ) const
{
	return taxPeriod;
}
void CServerData::TownTaxPeriod( UI32 value )
{
	taxPeriod = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 TownGuardPayment( void ) const
//|					void TownGuardPayment( UI32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the time (in seconds) between payments for guards
//o-----------------------------------------------------------------------------------------------o
UI32 CServerData::TownGuardPayment( void ) const
{
	return guardPayment;
}
void CServerData::TownGuardPayment( UI32 value )
{
	guardPayment = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 RepMaxKills( void ) const
//|					void RepMaxKills( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the threshold in player kills before a player turns red (murderer)
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::RepMaxKills( void ) const
{
	return maxmurdersallowed;
}
void CServerData::RepMaxKills( UI16 value )
{
	maxmurdersallowed = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 ResLogs( void ) const
//|					void ResLogs( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum number of logs in a given resource area
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::ResLogs( void ) const
{
	return logsperarea;
}
void CServerData::ResLogs( SI16 value )
{
	logsperarea = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ResLogTime( void ) const
//|					void ResLogTime( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the time it takes for 1 single log to respawn in a resource area
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::ResLogTime( void ) const
{
	return logsrespawntimer;
}
void CServerData::ResLogTime( UI16 value )
{
	logsrespawntimer = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 CServerData::ResLogArea( void ) const
//|					void CServerData::ResLogArea( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the number of log-areas to split the world into (min 10)
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::ResLogArea( void ) const
{
	return logsrespawnarea;
}
void CServerData::ResLogArea( UI16 value )
{
	if( value < 10 )
		value = 10;
	logsrespawnarea = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 ResOre( void ) const
//|					void ResOre( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum number of ore in a given resource area
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::ResOre( void ) const
{
	return oreperarea;
}
void CServerData::ResOre( SI16 value )
{
	oreperarea = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ResOreTime( void ) const
//|					void ResOreTime( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the time it takes for 1 single ore to respawn in a resource area
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::ResOreTime( void ) const
{
	return orerespawntimer;
}
void CServerData::ResOreTime( UI16 value )
{
	orerespawntimer = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ResOreArea( void ) const
//|					void ResOreArea( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the number of ore-areas to split the world into (min 10)
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::ResOreArea( void ) const
{
	return orerespawnarea;
}
void CServerData::ResOreArea( UI16 value )
{
	if( value < 10 )
		value = 10;
	orerespawnarea = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R64 AccountFlushTimer( void ) const
//|					void AccountFlushTimer( R64 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets how often (in mins) online accounts are checked to see if they really ARE online
//o-----------------------------------------------------------------------------------------------o
R64 CServerData::AccountFlushTimer( void ) const
{
	return flushTime;
}
void CServerData::AccountFlushTimer( R64 value )
{
	flushTime = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GetClientFeature( ClientFeatures bitNum ) const
//|					void SetClientFeature( ClientFeatures bitNum, bool nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets which client side features to enable for connecting clients
//|	Notes		-	See ClientFeatures enum in cServerData.h for full list
//o-----------------------------------------------------------------------------------------------o
bool CServerData::GetClientFeature( ClientFeatures bitNum ) const
{
	return clientFeatures.test( bitNum );
}
void CServerData::SetClientFeature( ClientFeatures bitNum, bool nVal )
{
	clientFeatures.set( bitNum, nVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 GetClientFeatures( void ) const
//|					void SetClientFeatures( UI32 nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets which client side features to enable for connecting clients
//|	Notes		-	See ClientFeatures enum in cServerData.h for full list
//o-----------------------------------------------------------------------------------------------o
UI32 CServerData::GetClientFeatures( void ) const
{
	return static_cast<UI32>(clientFeatures.to_ulong());
}
void CServerData::SetClientFeatures( UI32 nVal )
{
	clientFeatures = nVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GetServerFeature( ServerFeatures bitNum ) const
//|					void SetServerFeature( ServerFeatures bitNum, bool nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets which server side features to enable
//|	Notes		-	See ServerFeatures enum in cServerData.h for full list
//o-----------------------------------------------------------------------------------------------o
bool CServerData::GetServerFeature( ServerFeatures bitNum ) const
{
	return serverFeatures.test( bitNum );
}
void CServerData::SetServerFeature( ServerFeatures bitNum, bool nVal )
{
	serverFeatures.set( bitNum, nVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	size_t GetServerFeatures( void ) const
//|					void SetServerFeatures( size_t nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets which server side features to enable
//|	Notes		-	See ServerFeatures enum in cServerData.h for full list
//o-----------------------------------------------------------------------------------------------o
size_t CServerData::GetServerFeatures( void ) const
{
	return serverFeatures.to_ulong();
}
void CServerData::SetServerFeatures( size_t nVal )
{
	serverFeatures = nVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GetAssistantNegotiation( void ) const
//|					void SetAssistantNegotiation( bool nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets status of feature negotiation with assist tools like Razor and AssistUO
//o-----------------------------------------------------------------------------------------------o
bool CServerData::GetAssistantNegotiation( void ) const
{
	return boolVals.test( BIT_ASSISTANTNEGOTIATION );
}
void CServerData::SetAssistantNegotiation( bool nVal )
{
	boolVals.set( BIT_ASSISTANTNEGOTIATION, nVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GetClassicUOMapTracker( void ) const
//|					void SetClassicUOMapTracker( bool nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether server should respond to ClassicUO's WorldMap Tracker packets 
//o-----------------------------------------------------------------------------------------------o
bool CServerData::GetClassicUOMapTracker( void ) const
{
	return boolVals.test( BIT_CLASSICUOMAPTRACKER );
}
void CServerData::SetClassicUOMapTracker( bool nVal )
{
	boolVals.set( BIT_CLASSICUOMAPTRACKER, nVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GetDisabledAssistantFeature( ClientFeatures bitNum ) const
//|					void SetDisabledAssistantFeature( ClientFeatures bitNum, bool nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets which assistant features to enable for connecting clients
//|	Notes		-	Example of assistant: Razor, AssistUO
//|					See ClientFeatures enum in cServerData.h for full list
//o-----------------------------------------------------------------------------------------------o
bool CServerData::GetDisabledAssistantFeature( AssistantFeatures bitNum ) const
{
	return 0 != (CServerData::DisabledAssistantFeatures & bitNum);
}
void CServerData::SetDisabledAssistantFeature( AssistantFeatures bitNum, bool nVal )
{
	if( nVal )
	{
		CServerData::DisabledAssistantFeatures |= bitNum;
	}
	else
		CServerData::DisabledAssistantFeatures &= ~bitNum;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI64 GetDisabledAssistantFeatures( void ) const
//|					void SetDisabledAssistantFeatures( UI64 nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets which assistant features to enable for connecting clients
//|	Notes		-	Example of assistant: Razor, AssistUO
//|					See ClientFeatures enum in cServerData.h for full list
//o-----------------------------------------------------------------------------------------------o
UI64 CServerData::GetDisabledAssistantFeatures( void ) const
{
	return CServerData::DisabledAssistantFeatures;
}
void CServerData::SetDisabledAssistantFeatures( UI64 nVal )
{
	CServerData::DisabledAssistantFeatures = nVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool KickOnAssistantSilence( void ) const
//|					void KickOnAssistantSilence( bool nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether player is kicked if there's no response from assistant tool
//|					to verify it complies with the allowed assistant features
//o-----------------------------------------------------------------------------------------------o
bool CServerData::KickOnAssistantSilence( void ) const
{
	return boolVals.test( BIT_KICKONASSISTANTSILENCE );
}
void CServerData::KickOnAssistantSilence( bool nVal )
{
	boolVals.set( BIT_KICKONASSISTANTSILENCE, nVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CServerData::save( void )
//|	Date		-	02/21/2002
//|	Org/Team	-	UOX3 DevTeam
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Save the uox.ini out. This is the default save
//o-----------------------------------------------------------------------------------------------o
//|	Returns		- [TRUE] If successfull
//o-----------------------------------------------------------------------------------------------o
bool CServerData::save( void )
{
	std::string s = Directory( CSDDP_ROOT );
	s += "uox.ini";
	return save( s );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CServerData::save( std::string filename )
//|	Programmer	-	EviLDeD/Unknown
//|	Org/Team	-	UOX3 DevTeam
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	This is the full uox.ini save routing.
//|
//|	Changes		-	02/21/2002	-	Fixed paths not being saved back out
//|
//|	Changes		-	02/21/2002	-	Added ini support for the isloation system
//|									At this point it just defaults to Isolation level 1.
//|
//|	Changes		-	02/27/2002	-	Made sure that ALL directory paths are
//|									written out to the ini file.
//|
//|	Changes		-	04/03/2004	-	Added new tags to the system group
//|
//|										SERVERNAME		: Name of the server.
//|										NETRCVTIMEOUT	: Timeout in seconds for recieving data
//|										NETSNDTIMEOUT	: Timeout in seconds for sending data
//|										UOGENABLED		: Does this server respond to UOGInfo Requests?
//|																		0-No 1-Yes
//|
//|	Changes		-	04/03/2004	-	Added new group [facet] with these tags
//|
//|										USEFACETSAVES	: Does the server seperate facet data?
//|																		0-No : All data will be saved into the shared folder
//|																		1-Yes: Each Facet will save its data into its own shared folder
//|										MAP0-MAP3			:	Format= MAPx=Mapname,MapAccess
//|
//|																		Where Mapname is the name of the map and
//|																		MapAccess the set of bit flags that allow/deny features and map access
//|
//o-----------------------------------------------------------------------------------------------o
bool CServerData::save( std::string filename )
{
	bool rvalue = false;
	std::ofstream ofsOutput;
	ofsOutput.open( filename.c_str(), std::ios::out );
	if( ofsOutput.is_open() )
	{
		ofsOutput << "// UOX Initialization File. V" << MAKEWORD( 2, 1 ) << '\n' << "//================================" << '\n' << '\n';
		ofsOutput << "[system]" << '\n' << "{" << '\n';
		ofsOutput << "SERVERNAME=" << ServerName() << '\n';
		ofsOutput << "PORT=" << ServerPort() << '\n';
		ofsOutput << "NETRCVTIMEOUT=" << ServerNetRcvTimeout() << '\n';
		ofsOutput << "NETSNDTIMEOUT=" << "3" << '\n';
		ofsOutput << "NETRETRYCOUNT=" << ServerNetRetryCount() << '\n';
		ofsOutput << "CONSOLELOG=" << static_cast<UI16>(ServerConsoleLogStatus()) << '\n';
		ofsOutput << "CRASHPROTECTION=" << static_cast<UI16>(ServerCrashProtectionStatus()) << '\n';
		ofsOutput << "COMMANDPREFIX=" << ServerCommandPrefix() << '\n';
		ofsOutput << "ANNOUNCEWORLDSAVES=" << (ServerAnnounceSavesStatus()?1:0) << '\n';
		ofsOutput << "JOINPARTMSGS=" << (ServerJoinPartAnnouncementsStatus()?1:0) << '\n';
		ofsOutput << "BACKUPSENABLED=" << (ServerBackupStatus()?1:0) << '\n';
		ofsOutput << "BACKUPSAVERATIO=" << BackupRatio() << '\n';
		ofsOutput << "SAVESTIMER=" << ServerSavesTimerStatus() << '\n';
		ofsOutput << "ACCOUNTISOLATION=" << "1" << '\n';
		ofsOutput << "UOGENABLED=" << (ServerUOGEnabled()?1:0) << '\n';
		ofsOutput << "RANDOMSTARTINGLOCATION=" << ( ServerRandomStartingLocation() ? 1 : 0 ) << '\n';
		ofsOutput << "ASSISTANTNEGOTIATION=" << (GetAssistantNegotiation()?1:0) << '\n';
		ofsOutput << "KICKONASSISTANTSILENCE=" << (KickOnAssistantSilence()?1:0) << '\n';
		ofsOutput << "CLASSICUOMAPTRACKER=" << (GetClassicUOMapTracker()?1:0) << '\n';
		ofsOutput << "}" << '\n' << '\n';

		ofsOutput << "[clientsupport]" << '\n' << "{" << '\n';
		ofsOutput << "CLIENTSUPPORT4000=" << (ClientSupport4000()?1:0) << '\n';
		ofsOutput << "CLIENTSUPPORT5000=" << (ClientSupport5000()?1:0) << '\n';
		ofsOutput << "CLIENTSUPPORT6000=" << (ClientSupport6000()?1:0) << '\n';
		ofsOutput << "CLIENTSUPPORT6050=" << (ClientSupport6050()?1:0) << '\n';
		ofsOutput << "CLIENTSUPPORT7000=" << (ClientSupport7000()?1:0) << '\n';
		ofsOutput << "CLIENTSUPPORT7090=" << (ClientSupport7090()?1:0) << '\n';
		ofsOutput << "CLIENTSUPPORT70160=" << (ClientSupport70160()?1:0) << '\n';
		ofsOutput << "CLIENTSUPPORT70240=" << (ClientSupport70240()?1:0) << '\n';
		ofsOutput << "CLIENTSUPPORT70300=" << (ClientSupport70300()?1:0) << '\n';
		ofsOutput << "CLIENTSUPPORT70331=" << (ClientSupport70331()?1:0) << '\n';
		ofsOutput << "CLIENTSUPPORT704565=" << (ClientSupport704565()?1:0) << '\n';
		ofsOutput << "CLIENTSUPPORT70610=" << (ClientSupport70610()?1:0) << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[play server list]" << '\n' << "{" << '\n';

		std::vector< physicalServer >::iterator slIter;
		for( slIter = serverList.begin(); slIter != serverList.end(); ++slIter )
		{
			ofsOutput << "SERVERLIST=" << slIter->getName() << ",";
			if( !slIter->getDomain().empty() )
				ofsOutput << slIter->getDomain() << ",";
			else
				ofsOutput << slIter->getIP() << ",";
			ofsOutput << slIter->getPort() << '\n';
		}
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[skill & stats]" << '\n' << "{" << '\n';
		ofsOutput << "SKILLLEVEL=" << static_cast<UI16>(SkillLevel()) << '\n';
		ofsOutput << "SKILLCAP=" << ServerSkillTotalCapStatus() << '\n';
		ofsOutput << "SKILLDELAY=" << static_cast<UI16>(ServerSkillDelayStatus()) << '\n';
		ofsOutput << "STATCAP=" << ServerStatCapStatus() << '\n';
		ofsOutput << "EXTENDEDSTARTINGSTATS=" << (ExtendedStartingStats()?1:0) << '\n';
		ofsOutput << "EXTENDEDSTARTINGSKILLS=" << (ExtendedStartingSkills()?1:0) << '\n';
		ofsOutput << "MAXSTEALTHMOVEMENTS=" << MaxStealthMovement() << '\n';
		ofsOutput << "MAXSTAMINAMOVEMENTS=" << MaxStaminaMovement() << '\n';
		ofsOutput << "SNOOPISCRIME=" << (SnoopIsCrime()?1:0) << '\n';
		ofsOutput << "ARMORAFFECTMANAREGEN=" << (ArmorAffectManaRegen() ? 1 : 0) << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[timers]" << '\n' << "{" << '\n';
		ofsOutput << "CORPSEDECAYTIMER=" << SystemTimer( tSERVER_CORPSEDECAY ) << '\n';
		ofsOutput << "WEATHERTIMER=" << SystemTimer( tSERVER_WEATHER ) << '\n';
		ofsOutput << "SHOPSPAWNTIMER=" << SystemTimer( tSERVER_SHOPSPAWN ) << '\n';
		ofsOutput << "DECAYTIMER=" << SystemTimer( tSERVER_DECAY ) << '\n';
		ofsOutput << "INVISIBILITYTIMER=" << SystemTimer( tSERVER_INVISIBILITY ) << '\n';
		ofsOutput << "OBJECTUSETIMER=" << SystemTimer( tSERVER_OBJECTUSAGE ) << '\n';
		ofsOutput << "GATETIMER=" << SystemTimer( tSERVER_GATE ) << '\n';
		ofsOutput << "POISONTIMER=" << SystemTimer( tSERVER_POISON ) << '\n';
		ofsOutput << "LOGINTIMEOUT=" << SystemTimer( tSERVER_LOGINTIMEOUT ) << '\n';
		ofsOutput << "HITPOINTREGENTIMER=" << SystemTimer( tSERVER_HITPOINTREGEN ) << '\n';
		ofsOutput << "STAMINAREGENTIMER=" << SystemTimer( tSERVER_STAMINAREGEN ) << '\n';
		ofsOutput << "MANAREGENTIMER=" << SystemTimer( tSERVER_MANAREGEN ) << '\n';
		ofsOutput << "BASEFISHINGTIMER=" << SystemTimer( tSERVER_FISHINGBASE ) << '\n';
		ofsOutput << "RANDOMFISHINGTIMER=" << SystemTimer( tSERVER_FISHINGRANDOM ) << '\n';
		ofsOutput << "SPIRITSPEAKTIMER=" << SystemTimer( tSERVER_SPIRITSPEAK ) << '\n';
		ofsOutput << "PETOFFLINECHECKTIMER=" << SystemTimer( tSERVER_PETOFFLINECHECK ) << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[directories]" << '\n' << "{" << '\n';
		ofsOutput << "DIRECTORY=" << Directory( CSDDP_ROOT ) << '\n';
		ofsOutput << "DATADIRECTORY=" << Directory( CSDDP_DATA ) << '\n';
		ofsOutput << "DEFSDIRECTORY=" << Directory( CSDDP_DEFS ) << '\n';
		ofsOutput << "BOOKSDIRECTORY=" << Directory( CSDDP_BOOKS ) << '\n';
		ofsOutput << "ACTSDIRECTORY=" << Directory( CSDDP_ACCOUNTS ) << '\n';
		ofsOutput << "SCRIPTSDIRECTORY=" << Directory( CSDDP_SCRIPTS ) << '\n';
		ofsOutput << "BACKUPDIRECTORY=" << Directory( CSDDP_BACKUP ) << '\n';
		ofsOutput << "MSGBOARDDIRECTORY=" << Directory( CSDDP_MSGBOARD ) << '\n';
		ofsOutput << "SHAREDDIRECTORY=" << Directory( CSDDP_SHARED ) << '\n';
		ofsOutput << "ACCESSDIRECTORY=" << Directory( CSDDP_ACCESS ) << '\n';
		ofsOutput << "HTMLDIRECTORY=" << Directory( CSDDP_HTML ) << '\n';
		ofsOutput << "LOGSDIRECTORY=" << Directory( CSDDP_LOGS ) << '\n';
		ofsOutput << "DICTIONARYDIRECTORY=" << Directory( CSDDP_DICTIONARIES ) << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[settings]" << '\n' << "{" << '\n';
		ofsOutput << "LOOTDECAYSWITHCORPSE=" << (CorpseLootDecay()?1:0) << '\n';
		ofsOutput << "GUARDSACTIVE=" << (GuardsStatus()?1:0) << '\n';
		ofsOutput << "DEATHANIMATION=" << (DeathAnimationStatus()?1:0) << '\n';
		ofsOutput << "AMBIENTSOUNDS=" << WorldAmbientSounds() << '\n';
		ofsOutput << "AMBIENTFOOTSTEPS=" << (AmbientFootsteps()?1:0) << '\n';
		ofsOutput << "INTERNALACCOUNTCREATION=" << (InternalAccountStatus()?1:0) << '\n';
		ofsOutput << "SHOWOFFLINEPCS=" << (ShowOfflinePCs()?1:0) << '\n';
		ofsOutput << "ROGUESENABLED=" << (RogueStatus()?1:0) << '\n';
		ofsOutput << "PLAYERPERSECUTION=" << (PlayerPersecutionStatus()?1:0) << '\n';
		ofsOutput << "ACCOUNTFLUSH=" << AccountFlushTimer() << '\n';
		ofsOutput << "HTMLSTATUSENABLED=" << HtmlStatsStatus() << '\n';
		ofsOutput << "SELLBYNAME=" << (SellByNameStatus()?1:0) << '\n';
		ofsOutput << "SELLMAXITEMS=" << SellMaxItemsStatus() << '\n';
		ofsOutput << "TRADESYSTEM=" << (TradeSystemStatus()?1:0) << '\n';
		ofsOutput << "RANKSYSTEM=" << (RankSystemStatus()?1:0) << '\n';
		ofsOutput << "CUTSCROLLREQUIREMENTS=" << (CutScrollRequirementStatus()?1:0) << '\n';
		ofsOutput << "NPCTRAININGENABLED=" << (NPCTrainingStatus()?1:0) << '\n';
		ofsOutput << "HIDEWILEMOUNTED=" << (CharHideWhileMounted()?1:0) << '\n';
		//ofsOutput << "WEIGHTPERSTR=" << static_cast<UI16>(WeightPerStr()) << '\n';
		ofsOutput << "WEIGHTPERSTR=" << static_cast<R32>(WeightPerStr()) << '\n';
		ofsOutput << "POLYDURATION=" << SystemTimer( tSERVER_POLYMORPH ) << '\n';
		ofsOutput << "CLIENTFEATURES=" << GetClientFeatures() << '\n';
		ofsOutput << "SERVERFEATURES=" << GetServerFeatures() << '\n';
		ofsOutput << "OVERLOADPACKETS=" << (ServerOverloadPackets()?1:0) << '\n';
		ofsOutput << "ADVANCEDPATHFINDING=" << (AdvancedPathfinding()?1:0) << '\n';
		ofsOutput << "LOOTINGISCRIME=" << (LootingIsCrime()?1:0) << '\n';
		ofsOutput << "BASICTOOLTIPSONLY=" << (BasicTooltipsOnly()?1:0) << '\n';
		ofsOutput << "GLOBALITEMDECAY=" << (GlobalItemDecay()?1:0) << '\n';
		ofsOutput << "SCRIPTITEMSDECAYABLE=" << (ScriptItemsDecayable()?1:0) << '\n';
		ofsOutput << "BASEITEMSDECAYABLE=" << (BaseItemsDecayable()?1:0) << '\n';
		ofsOutput << "ITEMDECAYINHOUSES=" << (ItemDecayInHouses()?1:0) << '\n';
		ofsOutput << "PAPERDOLLGUILDBUTTON=" << (PaperdollGuildButton()?1:0) << '\n';
		ofsOutput << "FISHINGSTAMINALOSS=" << FishingStaminaLoss() << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[speedup]" << '\n' << "{" << '\n';
		ofsOutput << "CHECKITEMS=" << CheckItemsSpeed() << '\n';
		ofsOutput << "CHECKBOATS=" << CheckBoatSpeed() << '\n';
		ofsOutput << "CHECKNPCAI=" << CheckNpcAISpeed() << '\n';
		ofsOutput << "CHECKSPAWNREGIONS=" << CheckSpawnRegionSpeed() << '\n';
		ofsOutput << "NPCMOVEMENTSPEED=" << NPCWalkingSpeed() << '\n';
		ofsOutput << "NPCRUNNINGSPEED=" << NPCRunningSpeed() << '\n';
		ofsOutput << "NPCFLEEINGSPEED=" << NPCFleeingSpeed() << '\n';
		ofsOutput << "NPCSPELLCASTSPEED=" << NPCSpellCastSpeed() << '\n';
		ofsOutput << "GLOBALATTACKSPEED=" << GlobalAttackSpeed() << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[message boards]" << '\n' << "{" << '\n';
		ofsOutput << "POSTINGLEVEL=" << static_cast<UI16>(MsgBoardPostingLevel()) << '\n';
		ofsOutput << "REMOVALLEVEL=" << static_cast<UI16>(MsgBoardPostRemovalLevel()) << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[escorts]" << '\n' << "{" << '\n';
		ofsOutput << "ESCORTENABLED=" << ( EscortsEnabled() ? 1 : 0 ) << '\n';
		ofsOutput << "ESCORTINITEXPIRE=" << SystemTimer( tSERVER_ESCORTWAIT ) << '\n';
		ofsOutput << "ESCORTACTIVEEXPIRE=" << SystemTimer( tSERVER_ESCORTACTIVE ) << '\n';
		ofsOutput << "ESCORTDONEEXPIRE=" << SystemTimer( tSERVER_ESCORTDONE ) << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[worldlight]" << '\n' << "{" << '\n';
		ofsOutput << "DUNGEONLEVEL=" << static_cast<UI16>(DungeonLightLevel()) << '\n';
		ofsOutput << "BRIGHTLEVEL=" << static_cast<UI16>(WorldLightBrightLevel()) << '\n';
		ofsOutput << "DARKLEVEL=" << static_cast<UI16>(WorldLightDarkLevel()) << '\n';
		ofsOutput << "SECONDSPERUOMINUTE=" << ServerSecondsPerUOMinute() << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[tracking]" << '\n' << "{" << '\n';
		ofsOutput << "BASERANGE=" << TrackingBaseRange() << '\n';
		ofsOutput << "BASETIMER=" << TrackingBaseTimer() << '\n';
		ofsOutput << "MAXTARGETS=" << static_cast<UI16>(TrackingMaxTargets()) << '\n';
		ofsOutput << "MSGREDISPLAYTIME=" << TrackingRedisplayTime() << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[reputation]" << '\n' << "{" << '\n';
		ofsOutput << "MURDERDECAYTIMER=" << SystemTimer( tSERVER_MURDERDECAY ) << '\n';
		ofsOutput << "MAXKILLS=" << RepMaxKills() << '\n';
		ofsOutput << "CRIMINALTIMER=" << SystemTimer( tSERVER_CRIMINAL ) << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[resources]" << '\n' << "{" << '\n';
		ofsOutput << "MINECHECK=" << static_cast<UI16>(MineCheck()) << '\n';
		ofsOutput << "OREPERAREA=" << ResOre() << '\n';
		ofsOutput << "ORERESPAWNTIMER=" << ResOreTime() << '\n';
		ofsOutput << "ORERESPAWNAREA=" << ResOreArea() << '\n';
		ofsOutput << "LOGSPERAREA=" << ResLogs() << '\n';
		ofsOutput << "LOGSRESPAWNTIMER=" << ResLogTime() << '\n';
		ofsOutput << "LOGSRESPAWNAREA=" << ResLogArea() << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[hunger]" << '\n' << "{" << '\n';
		ofsOutput << "HUNGERRATE=" << SystemTimer( tSERVER_HUNGERRATE ) << '\n';
		ofsOutput << "HUNGERDMGVAL=" << HungerDamage() << '\n';
		ofsOutput << "PETHUNGEROFFLINE=" << (PetHungerOffline()?1:0) << '\n';
		ofsOutput << "PETOFFLINETIMEOUT=" << PetOfflineTimeout() << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[combat]" << '\n' << "{" << '\n';
		ofsOutput << "MAXRANGE=" << CombatMaxRange() << '\n';
		ofsOutput << "ARCHERRANGE=" << CombatArcherRange() << '\n';
		ofsOutput << "SPELLMAXRANGE=" << CombatMaxSpellRange() << '\n';
		ofsOutput << "DISPLAYHITMSG=" << (CombatDisplayHitMessage()?1:0) << '\n';
		ofsOutput << "DISPLAYDAMAGENUMBERS=" << (CombatDisplayDamageNumbers()?1:0) << '\n';
		ofsOutput << "MONSTERSVSANIMALS=" << (CombatMonstersVsAnimals()?1:0) << '\n';
		ofsOutput << "ANIMALATTACKCHANCE=" << static_cast<UI16>(CombatAnimalsAttackChance()) << '\n';
		ofsOutput << "ANIMALSGUARDED=" << (CombatAnimalsGuarded()?1:0) << '\n';
		ofsOutput << "NPCDAMAGERATE=" << CombatNPCDamageRate() << '\n';
		ofsOutput << "NPCBASEFLEEAT=" << CombatNPCBaseFleeAt() << '\n';
		ofsOutput << "NPCBASEREATTACKAT=" << CombatNPCBaseReattackAt() << '\n';
		ofsOutput << "ATTACKSTAMINA=" << CombatAttackStamina() << '\n';
		ofsOutput << "ATTACKSPEEDFROMSTAMINA=" << (CombatAttackSpeedFromStamina()?1:0) << '\n';
		ofsOutput << "SHOOTONANIMALBACK=" << (ShootOnAnimalBack()?1:0) << '\n';
		ofsOutput << "COMBATEXPLODEDELAY=" << CombatExplodeDelay() << '\n';
		ofsOutput << "WEAPONDAMAGECHANCE=" << static_cast<UI16>(CombatWeaponDamageChance()) << '\n';
		ofsOutput << "WEAPONDAMAGEMIN=" << static_cast<UI16>(CombatWeaponDamageMin()) << '\n';
		ofsOutput << "WEAPONDAMAGEMAX=" << static_cast<UI16>(CombatWeaponDamageMax()) << '\n';
		ofsOutput << "ARMORDAMAGECHANCE=" << static_cast<UI16>(CombatArmorDamageChance()) << '\n';
		ofsOutput << "ARMORDAMAGEMIN=" << static_cast<UI16>(CombatArmorDamageMin()) << '\n';
		ofsOutput << "ARMORDAMAGEMAX=" << static_cast<UI16>(CombatArmorDamageMax()) << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[start locations]" << '\n' << "{" << '\n';
		for( size_t lCtr = 0; lCtr < startlocations.size(); ++lCtr )
			ofsOutput << "LOCATION=" << startlocations[lCtr].newTown << "," << startlocations[lCtr].newDescription << "," << startlocations[lCtr].x << "," << startlocations[lCtr].y << "," << startlocations[lCtr].z << "," << startlocations[lCtr].worldNum << "," << startlocations[lCtr].instanceID << "," << startlocations[lCtr].clilocDesc << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[startup]" << '\n' << "{" << '\n';
		ofsOutput << "STARTGOLD=" << ServerStartGold() << '\n';
		ofsOutput << "STARTPRIVS=" << ServerStartPrivs() << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[gumps]" << '\n' << "{" << '\n';
		ofsOutput << "TITLECOLOUR=" << TitleColour() << '\n';
		ofsOutput << "LEFTTEXTCOLOUR=" << LeftTextColour() << '\n';
		ofsOutput << "RIGHTTEXTCOLOUR=" << RightTextColour() << '\n';
		ofsOutput << "BUTTONCANCEL=" << ButtonCancel() << '\n';
		ofsOutput << "BUTTONLEFT=" << ButtonLeft() << '\n';
		ofsOutput << "BUTTONRIGHT=" << ButtonRight() << '\n';
		ofsOutput << "BACKGROUNDPIC=" << BackgroundPic() << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[towns]" << '\n' << "{" << '\n';
		ofsOutput << "POLLTIME=" << TownNumSecsPollOpen() << '\n';
		ofsOutput << "MAYORTIME=" << TownNumSecsAsMayor() << '\n';
		ofsOutput << "TAXPERIOD=" << TownTaxPeriod() << '\n';
		ofsOutput << "GUARDSPAID=" << TownGuardPayment() << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[disabled assistant features]" << '\n' << "{" << '\n';
		ofsOutput << "AF_FILTERWEATHER=" << GetDisabledAssistantFeature( AF_FILTERWEATHER ) << '\n';
		ofsOutput << "AF_FILTERLIGHT=" << GetDisabledAssistantFeature( AF_FILTERLIGHT ) << '\n';
		ofsOutput << "AF_SMARTTARGET=" << GetDisabledAssistantFeature( AF_SMARTTARGET ) << '\n';
		ofsOutput << "AF_RANGEDTARGET=" << GetDisabledAssistantFeature( AF_RANGEDTARGET ) << '\n';
		ofsOutput << "AF_AUTOOPENDOORS=" << GetDisabledAssistantFeature( AF_AUTOOPENDOORS ) << '\n';
		ofsOutput << "AF_DEQUIPONCAST=" << GetDisabledAssistantFeature( AF_DEQUIPONCAST ) << '\n';
		ofsOutput << "AF_AUTOPOTIONEQUIP=" << GetDisabledAssistantFeature( AF_AUTOPOTIONEQUIP ) << '\n';
		ofsOutput << "AF_POISONEDCHECKS=" << GetDisabledAssistantFeature( AF_POISONEDCHECKS ) << '\n';
		ofsOutput << "AF_LOOPEDMACROS=" << GetDisabledAssistantFeature( AF_LOOPEDMACROS ) << '\n';
		ofsOutput << "AF_USEONCEAGENT=" << GetDisabledAssistantFeature( AF_USEONCEAGENT ) << '\n';
		ofsOutput << "AF_RESTOCKAGENT=" << GetDisabledAssistantFeature( AF_RESTOCKAGENT ) << '\n';
		ofsOutput << "AF_SELLAGENT=" << GetDisabledAssistantFeature( AF_SELLAGENT ) << '\n';
		ofsOutput << "AF_BUYAGENT=" << GetDisabledAssistantFeature( AF_BUYAGENT ) << '\n';
		ofsOutput << "AF_POTIONHOTKEYS=" << GetDisabledAssistantFeature( AF_POTIONHOTKEYS ) << '\n';
		ofsOutput << "AF_RANDOMTARGETS=" << GetDisabledAssistantFeature( AF_RANDOMTARGETS ) << '\n';
		ofsOutput << "AF_CLOSESTTARGETS=" << GetDisabledAssistantFeature( AF_CLOSESTTARGETS ) << '\n';
		ofsOutput << "AF_OVERHEADHEALTH=" << GetDisabledAssistantFeature( AF_OVERHEADHEALTH ) << '\n';
		ofsOutput << "AF_AUTOLOOTAGENT=" << GetDisabledAssistantFeature( AF_AUTOLOOTAGENT ) << '\n';
		ofsOutput << "AF_BONECUTTERAGENT=" << GetDisabledAssistantFeature( AF_BONECUTTERAGENT ) << '\n';
		ofsOutput << "AF_JSCRIPTMACROS=" << GetDisabledAssistantFeature( AF_JSCRIPTMACROS ) << '\n';
		ofsOutput << "AF_AUTOREMOUNT=" << GetDisabledAssistantFeature( AF_AUTOREMOUNT ) << '\n';
		ofsOutput << "AF_ALL=" << GetDisabledAssistantFeature( AF_ALL ) << '\n';
		ofsOutput << "}" << '\n';

#if P_ODBC == 1
		ODBCManager::getSingleton().SaveSettings( ofsOutput );
#endif

		ofsOutput.close();
		rvalue = true;
	}
	else
		Console.Error( "Unable to open file %s for writing", filename.c_str() );
	return rvalue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	Load()
//|	Programmer	-	EviLDeD
//|	Date		-	January 13, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Load up the uox.ini file and parse it into the internals
//|	Returns		-	pointer to the valid inmemory serverdata storage(this)
//|						NULL is there is an error, or invalid file type
//o-----------------------------------------------------------------------------------------------o
void CServerData::Load( void )
{
	const UString fileName = Directory( CSDDP_ROOT ) + "uox.ini";
	ParseINI( fileName );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 TrackingBaseRange( void ) const
//|					void TrackingBaseRange( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the base (minimum) range even beginner trackers can track at 
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::TrackingBaseRange( void ) const
{
	return trackingbaserange;
}
void CServerData::TrackingBaseRange( UI16 value )
{
	trackingbaserange = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 TrackingMaxTargets( void ) const
//|					void TrackingMaxTargets( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max amount of targets someone can see in the tracking window
//o-----------------------------------------------------------------------------------------------o
UI08 CServerData::TrackingMaxTargets( void ) const
{
	return trackingmaxtargets;
}
void CServerData::TrackingMaxTargets( UI08 value )
{
	if( value >= MAX_TRACKINGTARGETS )
		trackingmaxtargets = MAX_TRACKINGTARGETS;
	else
		trackingmaxtargets = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 TrackingBaseTimer( void ) const
//|					void TrackingBaseTimer( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max amount of time a grandmaster tracker can track someone
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::TrackingBaseTimer( void ) const
{
	return trackingbasetimer;
}
void CServerData::TrackingBaseTimer( UI16 value )
{
	trackingbasetimer = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 TrackingRedisplayTime( void ) const
//|					void TrackingRedisplayTime( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets how often in seconds the tracking message is updated and displayed
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::TrackingRedisplayTime( void ) const
{
	return trackingmsgredisplaytimer;
}
void CServerData::TrackingRedisplayTime( UI16 value )
{
	trackingmsgredisplaytimer = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void dumpLookup( SI32 lookupid )
//|	Date		-	January 31, 2001
//|	Programmer	-	EviLDeD
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	This simply outputs a text file that contains the index
//|					and lookup name found in the lookuptables. Saves having
//|					to do it by hand if you make changes. Must be called from
//|					within the class if used.
//o-----------------------------------------------------------------------------------------------o
void CServerData::dumpLookup( SI32 lookupid )
{
	// lookup is no longer needed, as we don't support server.scp any more

	std::ofstream ofsOutput;
	char buffer[81];
	SI32 count = 0;

	ofsOutput.open( "uox.ini.lookup.txt", std::ios::out );
	if( !ofsOutput.is_open() )
	{
		Console << "-o [DLOOKUP]: Unable to open file \"uox3.ini.lookup.txt\" for writing.." << myendl;
		return;
	}

	UString tokens( UOX3INI_LOOKUP );
	SI32 numTokens = tokens.sectionCount( "|" );
	for( SI32 i = 0; i <= numTokens; i++ )
	{
		UString tokenStr = tokens.section( "|", i, i );
		sprintf( buffer, "case 0x%04X:\t // %s[%04i]\n\tCServerData:\n\tbreak;\n", UOX3INI_LOOKUP.find( tokenStr.c_str(), tokenStr.length() ), tokenStr.c_str(), count++ );
		ofsOutput << buffer;
	}
	ofsOutput.close();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ParseINI( const std::string filename )
//|	Date		-	02/26/2001
//|	Programmer	-	EviLDeD
//|	Org/Team	-	UOX3 DevTeam
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Parse the uox.ini file into its required information.
//|
//|	Changes		-	02/26/2002	-	Make sure that we parse out the logs, access
//|									and other directories that we were not parsing/
//o-----------------------------------------------------------------------------------------------o
bool CServerData::ParseINI( const std::string& filename )
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
					if( !HandleLine( tag, data ) )
					{
						Console.Warning( "Unhandled tag '%s'", tag.c_str() );
					}
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

bool CServerData::HandleLine( const UString& tag, const UString& value )
{
	bool rvalue = true;
	std::string::size_type lFind = UOX3INI_LOOKUP.find( tag.c_str() );
	switch( lFind )
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
		ServerSavesTimer( value.toUInt() );
		break;
	case 0x0085:	 // SKILLCAP[0011]
		ServerSkillTotalCap( value.toUShort() );
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
	case 0x057B:	 // DARKLEVEL[0100]
		WorldLightDarkLevel( (LIGHTLEVEL)value.toUShort() );
		break;
	case 0x0585:	 // TITLECOLOUR[0101]
		TitleColour( value.toUShort() );
		break;
	case 0x0591:	 // LEFTTEXTCOLOUR[0102]
		LeftTextColour( value.toUShort() );
		break;
	case 0x05A0:	 // RIGHTTEXTCOLOUR[0103]
		RightTextColour( value.toUShort() );
		break;
	case 0x05B0:	 // BUTTONCANCEL[0104]
		ButtonCancel( value.toUShort() );
		break;
	case 0x05BD:	 // BUTTONLEFT[0105]
		ButtonLeft( value.toUShort() );
		break;
	case 0x05C8:	 // BUTTONRIGHT[0106]
		ButtonRight( value.toUShort() );
		break;
	case 0x05D4:	 // BACKGROUNDPIC[0107]
		BackgroundPic( value.toUShort() );
		break;
	case 0x05E2:	 // POLLTIME[0108]
		TownNumSecsPollOpen( value.toUInt() );
		break;
	case 0x05EB:	 // MAYORTIME[0109]
		TownNumSecsAsMayor( value.toUInt() );
		break;
	case 0x05F5:	 // TAXPERIOD[0110]
		TownTaxPeriod( value.toUInt() );
		break;
	case 0x05FF:	 // GUARDSPAID[0111]
		TownGuardPayment( value.toUInt() );
		break;
	case 0x060A:	 // DAY[0112]
		ServerTimeDay( value.toShort() );
		break;
	case 0x060E:	 // HOURS[0113]
		ServerTimeHours( value.toUByte() );
		break;
	case 0x0614:	 // MINUTES[0114]
		ServerTimeMinutes( value.toUByte() );
		break;
	case 0x061C:	 // SECONDS[0115]
		ServerTimeSeconds( value.toUByte() );
		break;
	case 0x0624:	 // AMPM[0116]
		ServerTimeAMPM( value.toUShort() != 0 );
		break;
	case 0x0629:	 // SKILLLEVEL[0117]
		SkillLevel( value.toUByte() );
		break;
	case 0x0634:	 // SNOOPISCRIME[0118]
		SnoopIsCrime( value.toUShort() != 0 );
		break;
	case 0x0641:	 // BOOKSDIRECTORY[0119]
		Directory( CSDDP_BOOKS, value );
		break;
	case 0x0650:	 // SERVERLIST[0120]
	{
		UString sname, sip, sport;
		physicalServer toAdd;
		if( value.sectionCount( "," ) == 2 )
		{
			struct hostent *lpHostEntry = NULL;
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
	case 0x065B:	 // PORT[0121]
		ServerPort( value.toUShort() );
		break;
	case 0x0660:	 // ACCESSDIRECTORY[0122]
		Directory( CSDDP_ACCESS, value );
		break;
	case 0x0670:	 // LOGSDIRECTORY[0123]
		Directory( CSDDP_LOGS, value );
		break;
	case 0x067E:	 // ACCOUNTISOLATION[0124]
		break;
	case 0x068F:	 // HTMLDIRECTORY[0125]
		Directory( CSDDP_HTML, value );
		break;
	case 0x069D:	 // SHOOTONANIMALBACK[0126]
		ShootOnAnimalBack( value.toUShort() == 1 );
		break;
	case 0x06AF:	 // NPCTRAININGENABLED[0127]
		NPCTrainingStatus( value.toUShort() == 1 );
		break;
	case 0x06C2:	 // DICTIONARYDIRECTORY[0128]
		Directory( CSDDP_DICTIONARIES, value );
		break;
	case 0x06D6:	 // BACKUPSAVERATIO[0129]
		BackupRatio( value.toShort() );
		break;
	case 0x06E6:	 // HIDEWILEMOUNTED[0130]
		CharHideWhileMounted( value.toShort() == 1 );
		break;
	case 0x06F6:	 // SECONDSPERUOMINUTE[0131]
		ServerSecondsPerUOMinute( value.toUShort() );
		break;
	case 0x0709:	 // WEIGHTPERSTR[0132]
		//WeightPerStr( value.toUByte() );
		WeightPerStr( value.toFloat() );
		break;
	case 0x0716:	 // POLYDURATION[0133]
		SystemTimer( tSERVER_POLYMORPH, value.toUShort() );
		break;
	case 0x0723:	 // UOGENABLED[0134]
		ServerUOGEnabled( value.toShort()==1 );
		break;
	case 0x072E:	 // NETRCVTIMEOUT[0135]
		ServerNetRcvTimeout( value.toUInt() );
		break;
	case 0x073C:	 // NETSNDTIMEOUT[0136]
		ServerNetSndTimeout( value.toUInt() );
		break;
	case 0x074A:	 // NETRETRYCOUNT[0137]
		ServerNetRetryCount( value.toUInt() );
		break;
	case 0x0758:	 // CLIENTFEATURES[0138]
		SetClientFeatures( value.toUInt() );
		break;
	case 0x0767:	 // PACKETOVERLOADS[0139]
		ServerOverloadPackets( (value.toByte() == 1) );
		break;
	case 0x0777:	 // NPCMOVEMENTSPEED[0140]
		NPCWalkingSpeed( value.toFloat() );
		break;
	case 0x0788:	 // PETHUNGEROFFLINE[0141]
		PetHungerOffline( (value.toByte() == 1) );
		break;
	case 0x0799:	 // PETOFFLINETIMEOUT[0142]
		PetOfflineTimeout( value.toUShort() );
		break;
	case 0x07AB:	 // PETOFFLINECHECKTIMER[0143]
		SystemTimer( tSERVER_PETOFFLINECHECK, value.toUShort() );
		break;
	case 0x07C0:	 // ARCHERRANGE[0144]
		CombatArcherRange( value.toShort() );
		break;
	case 0x07CC:	 // ADVANCEDPATHFINDING[0145]
		AdvancedPathfinding( (value.toByte() == 1) );
		break;
	case 0x07E0:	 // SERVERFEATURES[0146]
		SetServerFeatures( value.toUInt() );
		break;
	case 0x07EF:	 // LOOTINGISCRIME[0147]
		LootingIsCrime( (value.toByte() == 1) );
		break;
	case 0x07FE:	 // NPCRUNNINGSPEED[0148]
		NPCRunningSpeed( value.toFloat() );
		break;
	case 0x080E:	 // NPCFLEEINGSPEED[0149]
		NPCFleeingSpeed( value.toFloat() );
		break;
	case 0x081E:	 // BASICTOOLTIPSONLY[0150]
		BasicTooltipsOnly( (value.toByte() == 1) );
		break;
	case 0x0830:	 // GLOBALITEMDECAY[0151]
		GlobalItemDecay( (value.toByte() == 1) );
		break;
	case 0x0840:	 // SCRIPTITEMSDECAYABLE[0152]
		ScriptItemsDecayable( (value.toByte() == 1) );
		break;
	case 0x0855:	 // BASEITEMSDECAYABLE[0152]
		BaseItemsDecayable( (value.toByte() == 1) );
		break;
	case 0x0868:	 // ITEMDECAYINHOUSES[0153]
		ItemDecayInHouses( (value.toByte() == 1) );
		break;
	case 0x087a:	// COMBATEXPLODEDELAY[0154]
		CombatExplodeDelay( value.toUShort() );
		break;
	case 0x088d:	// PAPERDOLLGUILDBUTTON[0155]
		PaperdollGuildButton( value.toByte() == 1 );
		break;
	case 0x08a2:	// ATTACKSPEEDFROMSTAMINA[0156]
		CombatAttackSpeedFromStamina( value.toUShort() == 1 );
		break;
	case 0x08b9:	 // DISPLAYDAMAGENUMBERS[0157]
		CombatDisplayDamageNumbers( value.toUShort() == 1 );
		break;
	case 0x08ce:	 // CLIENTSUPPORT4000[0158]
		ClientSupport4000( value.toUShort() == 1 );
		break;
	case 0x08e0:	 // CLIENTSUPPORT5000[0159]
		ClientSupport5000( value.toUShort() == 1 );
		break;
	case 0x08f2:	 // CLIENTSUPPORT6000[0160]
		ClientSupport6000( value.toUShort() == 1 );
		break;
	case 0x0904:	 // CLIENTSUPPORT6050[0161]
		ClientSupport6050( value.toUShort() == 1 );
		break;
	case 0x0916:	 // CLIENTSUPPORT7000[0162]
		ClientSupport7000( value.toUShort() == 1 );
		break;
	case 0x0928:	 // CLIENTSUPPORT7090[0163]
		ClientSupport7090( value.toUShort() == 1 );
		break;
	case 0x093a:	 // CLIENTSUPPORT70160[0164]
		ClientSupport70160( value.toUShort() == 1 );
		break;
	case 0x094d:	// CLIENTSUPPORT70240[0165]
		ClientSupport70240( value.toUShort() == 1 );
		break;
	case 0x0960:	// CLIENTSUPPORT70300[0166]
		ClientSupport70300( value.toUShort() == 1 );
		break;
	case 0x0973:	// CLIENTSUPPORT70331[0167]
		ClientSupport70331( value.toUShort() == 1 );
		break;
	case 0x0986:	// CLIENTSUPPORT704565[0168]
		ClientSupport704565( value.toUShort() == 1 );
		break;
	case 0x099a:	// CLIENTSUPPORT70610[0169]
		ClientSupport70610( value.toUShort() == 1 );
		break;
	case 0x09ad:	 // EXTENDEDSTARTINGSTATS[0170]
		ExtendedStartingStats( value.toUShort() == 1 );
		break;
	case 0x09c3:	 // EXTENDEDSTARTINGSKILLS[0171]
		ExtendedStartingSkills( value.toUShort() == 1 );
		break;
	case 0x09da:	// WEAPONDAMAGECHANCE[0172]
		CombatWeaponDamageChance( value.toUByte() );
		break;
	case 0x09ed:	// ARMORDAMAGECHANCE[0173]
		CombatArmorDamageChance( value.toUByte() );
		break;
	case 0x09ff:	// WEAPONDAMAGEMIN[0174]
		CombatWeaponDamageMin( value.toUByte() );
		break;
	case 0x0a0f:	// WEAPONDAMAGEMAX[0175]
		CombatWeaponDamageMax( value.toUByte() );
		break;
	case 0x0a1f:	// ARMORDAMAGEMIN[0176]
		CombatArmorDamageMin( value.toUByte() );
		break;
	case 0x0a2e:	// ARMORDAMAGEMAX[0177]
		CombatArmorDamageMax( value.toUByte() );
		break;
	case 0x0a3d:	// GLOBALATTACKSPEED[0178]
		GlobalAttackSpeed( value.toFloat() );
		break;
	case 0x0a4f:	// NPCSPELLCASTSPEED[0179]
		NPCSpellCastSpeed( value.toFloat() );
		break;
	case 0x0a61:	// FISHINGSTAMINALOSS[0180]
		FishingStaminaLoss( value.toFloat() );
		break;
	case 0x0a74:	// RANDOMSTARTINGLOCATION[0181]
		ServerRandomStartingLocation( value.toUShort() == 1 );
		break;
	case 0x0a8b:	// ASSISTANTNEGOTIATION[0183]
		SetAssistantNegotiation( (value.toByte() == 1) );
		break;
	case 0x0aa0:	// KICKONASSISTANTSILENCE[0184]
		KickOnAssistantSilence( (value.toByte() == 1) );
		break;
	case 0x0ab7:	// AF_FILTERWEATHER[0185]
		SetDisabledAssistantFeature( AF_FILTERWEATHER, value.toByte() == 1 );
		break;
	case 0x0ac8:	// AF_FILTERLIGHT[0186]
		SetDisabledAssistantFeature( AF_FILTERLIGHT, value.toByte() == 1 );
		break;
	case 0x0ad7:	// AF_SMARTTARGET[0187]
		SetDisabledAssistantFeature( AF_SMARTTARGET, value.toByte() == 1 );
		break;
	case 0x0ae6:	// AF_RANGEDTARGET[0188]
		SetDisabledAssistantFeature( AF_RANGEDTARGET, value.toByte() == 1 );
		break;
	case 0x0af6:	// AF_AUTOOPENDOORS[0189]
		SetDisabledAssistantFeature( AF_AUTOOPENDOORS, value.toByte() == 1 );
		break;
	case 0x0b07:	// AF_DEQUIPONCAST[0190]
		SetDisabledAssistantFeature( AF_DEQUIPONCAST, value.toByte() == 1 );
		break;
	case 0x0b17:	// AF_AUTOPOTIONEQUIP[0191]
		SetDisabledAssistantFeature( AF_AUTOPOTIONEQUIP, value.toByte() == 1 );
		break;
	case 0x0b2a:	// AF_POISONEDCHECKS[0192]
		SetDisabledAssistantFeature( AF_POISONEDCHECKS, value.toByte() == 1 );
		break;
	case 0x0b3c:	// AF_LOOPEDMACROS[0193]
		SetDisabledAssistantFeature( AF_LOOPEDMACROS, value.toByte() == 1 );
		break;
	case 0x0b4c:	// AF_USEONCEAGENT[0194]
		SetDisabledAssistantFeature( AF_USEONCEAGENT, value.toByte() == 1 );
		break;
	case 0x0b5c:	// AF_RESTOCKAGENT[0195]
		SetDisabledAssistantFeature( AF_RESTOCKAGENT, value.toByte() == 1 );
		break;
	case 0x0b6c:	// AF_SELLAGENT[0196]
		SetDisabledAssistantFeature( AF_SELLAGENT, value.toByte() == 1 );
		break;
	case 0x0b79:	// AF_BUYAGENT[0197]
		SetDisabledAssistantFeature( AF_BUYAGENT, value.toByte() == 1 );
		break;
	case 0x0b85:	// AF_POTIONHOTKEYS[0198]
		SetDisabledAssistantFeature( AF_POTIONHOTKEYS, value.toByte() == 1 );
		break;
	case 0x0b96:	// AF_RANDOMTARGETS[0199]
		SetDisabledAssistantFeature( AF_RANDOMTARGETS, value.toByte() == 1 );
		break;
	case 0x0ba7:	// AF_CLOSESTTARGETS[0200]
		SetDisabledAssistantFeature( AF_CLOSESTTARGETS, value.toByte() == 1 );
		break;
	case 0x0bb9:	// AF_OVERHEADHEALTH[0201]
		SetDisabledAssistantFeature( AF_OVERHEADHEALTH, value.toByte() == 1 );
		break;
	case 0x0bcb:	// AF_AUTOLOOTAGENT[0202]
		SetDisabledAssistantFeature( AF_AUTOLOOTAGENT, value.toByte() == 1 );
		break;
	case 0x0bdc:	// AF_BONECUTTERAGENT[0203]
		SetDisabledAssistantFeature( AF_BONECUTTERAGENT, value.toByte() == 1 );
		break;
	case 0x0bef:	// AF_JSCRIPTMACROS[0204]
		SetDisabledAssistantFeature( AF_JSCRIPTMACROS, value.toByte() == 1 );
		break;
	case 0x0c00:	// AF_AUTOREMOUNT[0205]
		SetDisabledAssistantFeature( AF_AUTOREMOUNT, value.toByte() == 1 );
		break;
	case 0x0c0f:	// AF_ALL[0206]
		SetDisabledAssistantFeature( AF_ALL, value.toByte() == 1 );
		break;
	case 0x0c16:	// CLASSICUOMAPTRACKER[0207]
		SetClassicUOMapTracker( (value.toByte() == 1) );
		break;
	// How to add new entries here: Take previous case number, then add the length of the ini-setting (not function name) + 1 to find the next case number
#if P_ODBC == 1
	case 0x0c2a:	 // ODBCDSN[0208]
		ODBCManager::getSingleton(0168.SetDatabase( value );
		break;
	case 0x0c32:	 // ODBCUSER[0209]
		ODBCManager::getSingleton().SetUsername( value );
		break;
	case 0x0c3b:	// ODBCPASS[0210]
		ODBCManager::getSingleton().SetPassword( value );
		break;
#endif
	default:
		rvalue = false;
		break;
	}
	return rvalue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 ServerStartGold( void ) const
//|					void ServerStartGold( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default amount of starting gold for new characters
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::ServerStartGold( void ) const
{
	return startgold;
}
void CServerData::ServerStartGold( SI16 value )
{
	if( value >= 0 )
		startgold = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ServerStartPrivs( void ) const
//|					void ServerStartPrivs( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default starting privs for new characters
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::ServerStartPrivs( void ) const
{
	return startprivs;
}
void CServerData::ServerStartPrivs( UI16 value )
{
	startprivs = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 ServerMoon( SI16 slot ) const
//|					void ServerMoon( SI16 slot, SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the phase of one of the game's two moons
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::ServerMoon( SI16 slot ) const
{
	SI16 rvalue = -1;
	if( slot >= 0 && slot <= 1 )
		rvalue = moon[slot];
	return rvalue;
}
void CServerData::ServerMoon( SI16 slot, SI16 value )
{
	if( slot >= 0 && slot <= 1 && value >= 0 )
		moon[slot] = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	LIGHTLEVEL DungeonLightLevel( void ) const
//|					void DungeonLightLevel( LIGHTLEVEL value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default, global light level for dungeons
//o-----------------------------------------------------------------------------------------------o
LIGHTLEVEL CServerData::DungeonLightLevel( void ) const
{
	return dungeonlightlevel;
}
void CServerData::DungeonLightLevel( LIGHTLEVEL value )
{
	dungeonlightlevel = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	LIGHTLEVEL WorldLightCurrentLevel( void ) const
//|					void WorldLightCurrentLevel( LIGHTLEVEL value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default, global current light level outside of dungeons
//o-----------------------------------------------------------------------------------------------o
LIGHTLEVEL CServerData::WorldLightCurrentLevel( void ) const
{
	return currentlightlevel;
}
void CServerData::WorldLightCurrentLevel( LIGHTLEVEL value )
{
	currentlightlevel = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	LIGHTLEVEL WorldLightBrightLevel( void ) const
//|					void WorldLightBrightLevel( LIGHTLEVEL value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default, global light level for the brightest time of day
//o-----------------------------------------------------------------------------------------------o
LIGHTLEVEL CServerData::WorldLightBrightLevel( void ) const
{
	return brightnesslightlevel;
}
void CServerData::WorldLightBrightLevel( LIGHTLEVEL value )
{
	brightnesslightlevel = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	LIGHTLEVEL WorldLightDarkLevel( void ) const
//|					void WorldLightDarkLevel( LIGHTLEVEL value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default, global light level for the darkest time of day
//o-----------------------------------------------------------------------------------------------o
LIGHTLEVEL CServerData::WorldLightDarkLevel( void ) const
{
	return darknesslightlevel;
}
void CServerData::WorldLightDarkLevel( LIGHTLEVEL value )
{
	darknesslightlevel=value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void PostLoadDefaults( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	If no start locations have been provided in ini, use hardcoded defaults
//o-----------------------------------------------------------------------------------------------o
void CServerData::PostLoadDefaults( void )
{
	if( startlocations.empty() )
	{
		ServerLocation( "Yew,Center,545,982,0,0,0,1075072" );
		ServerLocation( "Minoc,Tavern,2477,411,15,0,0,1075073" );
		ServerLocation( "Britain,Sweet Dreams Inn,1495,1629,10,0,0,1075074" );
		ServerLocation( "Moonglow,Docks,4406,1045,0,0,0,1075075" );
		ServerLocation( "Trinsic,West Gate,1832,2779,0,0,0,1075076" );
		ServerLocation( "Magincia,Docks,3675,2259,26,0,0,1075077" );
		ServerLocation( "Jhelom,Docks,1492,3696,0,0,0,1075078" );
		ServerLocation( "Skara Brae,Docks,639,2236,0,0,0,1075079" );
		ServerLocation( "Vesper,Ironwood Inn,2771,977,0,0,0,1075080" );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	LPSTARTLOCATION ServerLocation( size_t locNum )
//|					void ServerLocation( std::string toSet )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets server start locations
//o-----------------------------------------------------------------------------------------------o
LPSTARTLOCATION CServerData::ServerLocation( size_t locNum )
{
	LPSTARTLOCATION rvalue = NULL;
	if( locNum < startlocations.size() )
		rvalue = &startlocations[locNum];
	return rvalue;
}
void CServerData::ServerLocation( std::string toSet )
{
	UString temp( toSet );
	if( temp.sectionCount( "," ) == 6 )	// Wellformed server location
	{
		STARTLOCATION toAdd;
		toAdd.x				= temp.section( ",", 2, 2 ).toShort();
		toAdd.y				= temp.section( ",", 3, 3 ).toShort();
		toAdd.z				= temp.section( ",", 4, 4 ).toShort();
		toAdd.worldNum		= temp.section( ",", 5, 5 ).toShort();
		toAdd.instanceID	= 0;
		toAdd.clilocDesc	= temp.section( ",", 6, 6 ).toUInt();
		strcpy( toAdd.oldTown, temp.section( ",", 0, 0 ).c_str() );
		strcpy( toAdd.oldDescription, temp.section( ",", 1, 1 ).c_str() );
		strcpy( toAdd.newTown, temp.section( ",", 0, 0 ).c_str() );
		strcpy( toAdd.newDescription, temp.section( ",", 1, 1 ).c_str() );
		startlocations.push_back( toAdd );
	}
	else if( temp.sectionCount( "," ) == 7 )	// instanceID included
	{
		STARTLOCATION toAdd;
		toAdd.x				= temp.section( ",", 2, 2 ).toShort();
		toAdd.y				= temp.section( ",", 3, 3 ).toShort();
		toAdd.z				= temp.section( ",", 4, 4 ).toShort();
		toAdd.worldNum		= temp.section( ",", 5, 5 ).toShort();
		toAdd.instanceID	= temp.section( ",", 6, 6 ).toUShort();
		toAdd.clilocDesc	= temp.section( ",", 7, 7 ).toUInt();
		strcpy( toAdd.oldTown, temp.section( ",", 0, 0 ).c_str() );
		strcpy( toAdd.oldDescription, temp.section( ",", 1, 1 ).c_str() );
		strcpy( toAdd.newTown, temp.section( ",", 0, 0 ).c_str() );
		strcpy( toAdd.newDescription, temp.section( ",", 1, 1 ).c_str() );
		startlocations.push_back( toAdd );
	}
	else
	{
		Console.Error( "Malformed location entry in ini file" );
	}
}

size_t CServerData::NumServerLocations( void ) const
{
	return startlocations.size();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ServerSecondsPerUOMinute( void ) const
//|					void ServerSecondsPerUOMinute( UI16 newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the number of real life seconds per UO minute
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::ServerSecondsPerUOMinute( void ) const
{
	return secondsperuominute;
}
void CServerData::ServerSecondsPerUOMinute( UI16 newVal )
{
	secondsperuominute = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SaveTime( void )
//|	Date		-	January 28th, 2007
//|	Programmer	-	giwo
//|	Org/Team	-	UOX3 DevTeam
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Outputs server time information to time.wsc in the /shared/ directory
//o-----------------------------------------------------------------------------------------------o
void CServerData::SaveTime( void )
{
	std::string		timeFile = cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "time.wsc";
	std::ofstream	timeDestination( timeFile.c_str() );
	if( !timeDestination )
	{
		Console.Error( "Failed to open %s for writing", timeFile.c_str() );
		return;
	}

	timeDestination << "[TIME]" << '\n' << "{" << '\n';
	timeDestination << "CURRENTLIGHT=" << static_cast<UI16>(WorldLightCurrentLevel()) << '\n';
	timeDestination << "DAY=" << ServerTimeDay() << '\n';
	timeDestination << "HOUR=" << static_cast<UI16>(ServerTimeHours()) << '\n';
	timeDestination << "MINUTE=" << static_cast<UI16>(ServerTimeMinutes()) << '\n';
	timeDestination << "AMPM=" << ( ServerTimeAMPM() ? 1 : 0 ) << '\n';
	timeDestination << "MOON=" << ServerMoon( 0 ) << "," << ServerMoon( 1 ) << '\n';
	timeDestination << "}" << '\n' << '\n';

	timeDestination.close();
}

void ReadWorldTagData( std::ifstream &inStream, UString &tag, UString &data );
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CServerData::LoadTime( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads server time information from time.wsc in the /shared/ directory
//o-----------------------------------------------------------------------------------------------o
void CServerData::LoadTime( void )
{
	std::ifstream input;
	std::string filename = cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "time.wsc";

	input.open( filename.c_str(), std::ios_base::in );
	input.seekg( 0, std::ios::beg );


	if( input.is_open() )
	{
		char line[1024];
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

void CServerData::incMoon( SI32 mNumber )
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

void physicalServer::setName(const std::string& newName)
{
  name = newName;
}
void physicalServer::setDomain(const std::string& newDomain)
{
  domain = newDomain;
}
void physicalServer::setIP(const std::string& newIP)
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
