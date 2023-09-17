#include "cserverdata.h"

#include <cstdint>
#include <filesystem>
#include <limits>
#include <optional>
#if !defined(_WIN32)
#include <netdb.h>

#include <arpa/inet.h>
#endif

#include "dictionary.h"
#include "funcdecl.h"
#include "ip4address.hpp"
#include "osunique.hpp"
#include "subsystem/console.hpp"

#include "scriptc.h"
#include "ssection.h"

#include "stringutility.hpp"
#include "utility/strutil.hpp"

using namespace std::string_literals;
//==================================================================================================
// constant data
//==================================================================================================
//==================================================================================================
const std::map<std::string, std::int32_t> CServerData::uox3IniCaseValue{
    // these numbers dont have to be in order, and can skip aound
    // they just CANT be repeated (now we could add some protection
    // but they dont change that much, and there are a lot, so if you delete one
    // dont worry about reusing it. Just keep adding up from the last one would
    // be easiest.
    // They key is that number HAS to match the case statement number
    // in
    // bool CServerData::HandleLine( const std::string& tag, const std::string& value )
    // in cserverdata.cpp (this file).
    {"SAVESTIMER"s, 6},
    {"SKILLCAP"s, 7},
    {"SKILLDELAY"s, 8},
    {"STATCAP"s, 9},
    {"MAXSTEALTHMOVEMENTS"s, 10},
    {"MAXSTAMINAMOVEMENTS"s, 11},
    {"CORPSEDECAYTIMER"s, 13},
    {"WEATHERTIMER"s, 14},
    {"SHOPSPAWNTIMER"s, 15},
    {"DECAYTIMER"s, 16},
    {"INVISIBILITYTIMER"s, 17},
    {"OBJECTUSETIMER"s, 18},
    {"GATETIMER"s, 19},
    {"POISONTIMER"s, 20},
    {"LOGINTIMEOUT"s, 21},
    {"HITPOINTREGENTIMER"s, 22},
    {"STAMINAREGENTIMER"s, 23},
    {"BASEFISHINGTIMER"s, 24},
    {"MAXPETOWNERS"s, 34},
    {"MAXFOLLOWERS"s, 35},
    {"MAXCONTROLSLOTS"s, 36},
    {"MANAREGENTIMER"s, 37},
    {"RANDOMFISHINGTIMER"s, 38},
    {"SPIRITSPEAKTIMER"s, 39},
    {"DEATHANIMATION"s, 27},
    {"AMBIENTSOUNDS"s, 50},
    {"ACCOUNTFLUSH"s, 56},
    {"SELLMAXITEMS"s, 59},
    {"CHECKITEMS"s, 63},
    {"CHECKBOATS"s, 64},
    {"CHECKNPCAI"s, 65},
    {"CHECKSPAWNREGIONS"s, 66},
    {"POSTINGLEVEL"s, 67},
    {"REMOVALLEVEL"s, 68},
    {"ESCORTENABLED"s, 69},
    {"ESCORTINITEXPIRE"s, 70},
    {"ESCORTACTIVEEXPIRE"s, 71},
    {"MOON1"s, 72},
    {"MOON2"s, 73},
    {"DUNGEONLEVEL"s, 74},
    {"CURRENTLEVEL"s, 75},
    {"BRIGHTLEVEL"s, 76},
    {"BASERANGE"s, 77},
    {"BASETIMER"s, 78},
    {"MAXTARGETS"s, 79},
    {"MSGREDISPLAYTIME"s, 80},
    {"MURDERDECAYTIMER"s, 81},
    {"MAXKILLS"s, 82},
    {"CRIMINALTIMER"s, 83},
    {"MINECHECK"s, 84},
    {"OREPERAREA"s, 85},
    {"ORERESPAWNTIMER"s, 86},
    {"RESOURCEAREASIZE"s, 87},
    {"LOGSPERAREA"s, 88},
    {"LOGSRESPAWNTIMER"s, 89},
    {"HUNGERRATE"s, 91},
    {"HUNGERDMGVAL"s, 92},
    {"MAXRANGE"s, 93},
    {"SPELLMAXRANGE"s, 94},
    {"ANIMALATTACKCHANCE"s, 97},
    {"NPCDAMAGERATE"s, 99},
    {"NPCBASEFLEEAT"s, 100},
    {"NPCBASEREATTACKAT"s, 101},
    {"ATTACKSTAMINA"s, 102},
     {"STARTGOLD"s, 104},
    {"STARTPRIVS"s, 105},
    {"ESCORTDONEEXPIRE"s, 106},
    {"DARKLEVEL"s, 107},
    {"TITLECOLOUR"s, 108},
    {"LEFTTEXTCOLOUR"s, 109},
    {"RIGHTTEXTCOLOUR"s, 110},
    {"BUTTONCANCEL"s, 111},
    {"BUTTONLEFT"s, 112},
    {"BUTTONRIGHT"s, 113},
    {"BACKGROUNDPIC"s, 114},
    {"POLLTIME"s, 115},
    {"MAYORTIME"s, 116},
    {"TAXPERIOD"s, 117},
    {"GUARDSPAID"s, 118},
    {"DAY"s, 119},
    {"HOURS"s, 120},
    {"MINUTES"s, 121},
    {"SECONDS"s, 122},
    {"AMPM"s, 123},
    {"SKILLLEVEL"s, 124},
    //{"SERVERLIST"s, 127},
    {"PORT"s, 128},
    {"ACCOUNTISOLATION"s, 131},
    {"SHOOTONANIMALBACK"s, 133},
    {"BACKUPSAVERATIO"s, 136},
    {"SECONDSPERUOMINUTE"s, 138},
    {"WEIGHTPERSTR"s, 139},
    {"POLYDURATION"s, 140},
    {"NETRCVTIMEOUT"s, 142},
    {"NETSNDTIMEOUT"s, 143},
    {"NETRETRYCOUNT"s, 144},
    {"CLIENTFEATURES"s, 145},
    {"NPCMOVEMENTSPEED"s, 147},
    {"PETOFFLINETIMEOUT"s, 149},
    {"PETOFFLINECHECKTIMER"s, 150},
    {"ARCHERRANGE"s, 151},
    {"SERVERFEATURES"s, 153},
    {"NPCRUNNINGSPEED"s, 155},
    {"NPCFLEEINGSPEED"s, 156},
    {"SPAWNREGIONSFACETS"s, 162},
    {"WEAPONDAMAGECHANCE"s, 184},
    {"ARMORDAMAGECHANCE"s, 185},
    {"WEAPONDAMAGEMIN"s, 186},
    {"WEAPONDAMAGEMAX"s, 187},
    {"ARMORDAMAGEMIN"s, 188},
    {"ARMORDAMAGEMAX"s, 189},
    {"GLOBALATTACKSPEED"s, 190},
    {"NPCSPELLCASTSPEED"s, 191},
    {"FISHINGSTAMINALOSS"s, 192},
    {"RANDOMSTARTINGLOCATION"s, 193},
    // 217 free
    {"DECAYTIMERINHOUSE"s, 219},
    {"MAXHOUSESOWNABLE"s, 222},
    {"MAXHOUSESCOOWNABLE"s, 223},
    {"COOWNHOUSESONSAMEACCOUNT"s, 225},
    {"MAXPLAYERPACKITEMS"s, 227},
    {"MAXPLAYERBANKITEMS"s, 228},
    {"PARRYDAMAGECHANCE"s, 240},
    {"PARRYDAMAGEMIN"s, 241},
    {"PARRYDAMAGEMAX"s, 242},
    {"ALCHEMYBONUSENABLED"s, 245},
    {"ALCHEMYBONUSMODIFIER"s, 246},
    {"NPCFLAGUPDATETIMER"s, 247},
    {"JSENGINESIZE"s, 248},
    {"THIRSTRATE"s, 251},
    {"THIRSTDRAINVAL"s, 252},
    {"BLOODDECAYTIMER"s, 255},
    {"BLOODDECAYCORPSETIMER"s, 256},
    {"BLOODEFFECTCHANCE"s, 257},
    {"NPCCORPSEDECAYTIMER"s, 258},
    {"BANKBUYTHRESHOLD"s, 266},
    {"NPCMOUNTEDWALKINGSPEED"s, 269},
    {"NPCMOUNTEDRUNNINGSPEED"s, 270},
    {"NPCMOUNTEDFLEEINGSPEED"s, 271},
    {"SERVERLANGUAGE"s, 273},
    {"PETLOYALTYGAINONSUCCESS"s, 275},
    {"PETLOYALTYLOSSONFAILURE"s, 276},
    {"PETLOYALTYRATE"s, 277},
    {"FISHPERAREA"s, 279},
    {"FISHRESPAWNTIMER"s, 280},
    {"ARCHERYHITBONUS"s, 281},
    {"SYSMESSAGECOLOUR"s, 283},
    {"ARCHERYSHOOTDELAY"s, 290},
    {"MAXCLIENTBYTESIN"s, 291},
    {"MAXCLIENTBYTESOUT"s, 292},
    {"NETTRAFFICTIMEBAN"s, 293},
    {"HIDESTATSFORUNKNOWNMAGICITEMS"s, 297},
    {"MAXSAFETELEPORTSPERDAY"s, 299},
    {"GLOBALRESTOCKMULTIPLIER"s, 308},
    {"CASTSPELLSWHILEMOVING"s, 316},
    {"MAXPLAYERPACKWEIGHT"s, 319},
    {"MAXPLAYERBANKWEIGHT"s, 320},
    {"WEAPONDAMAGEBONUSTYPE"s, 328},
    {"OFFERBODSFROMCONTEXTMENU"s, 330},
    {"BODSFROMCRAFTEDITEMSONLY"s, 331},
    {"BODGOLDREWARDMULTIPLIER"s, 332},
    {"BODFAMEREWARDMULTIPLIER"s, 333},
    {"ENABLENPCGUILDDISCOUNTS"s, 334},
    {"ENABLENPCGUILDPREMIUMS"s, 335},
    {"AGGRESSORFLAGTIMER"s, 336},
    {"PERMAGREYFLAGTIMER"s, 337},
    {"STEALINGFLAGTIMER"s, 338},
    {"APSPERFTHRESHOLD"s, 340},
    {"APSINTERVAL"s, 341},
    {"APSDELAYSTEP"s, 342},
    {"APSDELAYMAXCAP"s, 343},
    
};
constexpr auto MAX_TRACKINGTARGETS = 128;
constexpr auto SKILLTOTALCAP = 7000;
constexpr auto SKILLCAP = 1000;
constexpr auto STATCAP = 225;


// New uox3.ini format lookup
// January 13, 2001	- 	Modified: January 30, 2001 Converted to uppercase
// February 26 2002 	- 	Modified: to support the AccountIsolation, and left out dir3ectory
// tags September 22 2002 	- 	Added the  "HIDEWHILEMOUNTED" tag to support hide fix
// September 06 2003 	- 	Removed unused tags (heartbeat, wilderness, uoxbot, lagfix)
// October 16, 2003 	- 	Removed unused tag (SAVEMODE) and added "WEIGHTPERSTR".
// April 3, 2004 		- 	Added new tags, for UOG support, as well as new facet tags
// etc. June 15, 2004 		- 	Added the new tags for the xFTPd, and xFTPc support. April
// 2, 2005 		- 	Removed superfluous tags (BEGGINGTIMER, HUNGERDAMAGERATE,
// HUNGERTHRESHOLD, BEGGINGRANGE) September 13, 2005 	- 	Added MAPCOUNT to the uox.ini to
// specify the number of maps UOX3 uses. November 20, 2005 	- 	Removed USEFACETSAVES, MAP0,
// MAP1, MAP2, MAP3, USERMAP, FTPDENABLED,
//								FTPDUSER, FTPDUSERLIMIT, FTPDBIND,
//FTPDROOT,
// FTPCENABLED, FTPCHOST, 								FTPCPORT, FTPCUSER, FTPCULFLAGS,
// and MAPCOUNT as they were unused. Added OVERLOADPACKETS option to uox.ini to toggle JS packet
// handling.
// December 13, 2005 	- 	Changed uox.ini entry SHOWHIDDENNPCS to SHOWOFFLINEPCS to better
// reflect its function.
//								Removed CACHEMULS from the uox.ini as it is
//no longer used.
// February 12, 2006	- 	Added NPCMOVEMENTSPEED to the UOX.ini to allow customization of NPC
// movement speeds.
// July 7, 2006 		- 	PLAYERPERSECUTION in uox.ini (allows ghosts to drain mana
// from other players) set to 0 by default
// July 18, 2006 		- 	Added uox.ini flag to disable/enable the A* Pathfinding
// routine: ADVANCEDPATHFINDING=0/1
// July 24, 2006 		- 	Simplified many entries in uox.ini, making them easier to
// read. January 28, 2007 	- 	Moved dynamic parts of [worldlight] section from uox.ini to
// their own output file in the /shared/ directory March 3, 2007 		- 	Removed
// CLIENTSUPPORT from the uox.ini
//						  	Allowed customization of supported client features
//via uox.ini 									CLIENTFEATURES
// SERVERFEATURES
// April 9, 2007 		- 	Added a new setting to the uox.ini, LOOTINGISCRIME. If you set
// it to 0 looting corpses of innocent chars is not taken as a crime. June 10, 2007 		-
// Added two new settings to the uox.ini, NPCRUNNINGSPEED and NPCFLEEINGSPEED, they work the same
// way as NPCMOVEMENTSPEED. July 28, 2007 		- 	Added a new setting to the uox.ini:
// BASICTOOLTIPSONLY. If this is set to 1 the tooltips will only contain basic information, like the
// name and the weight of an item. February 7, 2010 	- 	Added new UOX.INI settings:
//									GLOBALITEMDECAY - Toggles on / off
//item decay on a global scale.Note that it does not remove the decay - status from items, it just
// resets the decay - timer when it reaches 0
// SCRIPTITEMSDECAYABLE - Toggles whether DFN - items will decay by default or not.Can be overriden
// by DECAY tag in item - DFNs 									BASEITEMSDECAYABLE -
// Toggles whether base - items will decay by default or not.Can be overriden by DECAY tag in
// harditems.dfn 									ITEMDECAYINHOUSES - Toggles default decay of non - locked down items inside multis(
// houses and boats )
// December 14, 20011 	- 	Exposed CombatExplodeDelay to UOX.INI, and changed the default delay
// between targeting and damage for the Explosion spell from 0 to 2 seconds
// March 19, 2012 		- 	Added support for packet 0xD7, SubCommand 0x28 - Guild button
// on paperdoll, which gives access to guild-functions if character belongs to a guild.
//									Can be enabled / disabled through
//UOX.INI setting PAPERDOLLGUILDBUTTON = 0 / 1. Defaults to 0.
// Added new UOX.INI option to determine whether attack - speed bonuses are gained from Stamina(
// default ) or Dexterity: ATTACKSPEEDFROMSTAMINA = 1
/// 0 							Added new UOX.INI option to control the displaying of damage -
/// numbers in combat( previously
// used DISPLAYHITMSG for numbers too ) : DISPLAYDAMAGENUMBERS = 1 / 0
// Added a new section to UOX.INI
//- [clientsupport] - along with the following settings to determine approved client - versions for
// the server : 									CLIENTSUPPORT4000 = 0 / 1
// CLIENTSUPPORT5000 = 0 / 1 									CLIENTSUPPORT6000 = 0 /
// 1 									CLIENTSUPPORT6050 = 0 / 1 									CLIENTSUPPORT7000 = 0 / 1 									CLIENTSUPPORT7090 = 0 / 1
//									CLIENTSUPPORT70160 = 0 / 1
//									CLIENTSUPPORT70240 = 0 / 1
//									NOTE: Each of these settings represent
//a range of clients, not just the individual versions mentioned.This means that CLIENTSUPPORT4000,
// for instance, 									will allow or disallow connections
// from clients 4.0.0 to 4.0.11f.Also note that while it is possible to enable support for all
// clients at the same time, 									it highly recommended
// to restrict support for client versions that match up to what the server is running.
// Added new
// UOX.INI option : EXTENDEDSTARTINGSTATS = 0 / 1 									If
// enabled, makes new characters start with 90 statpoints( selectable in clients 7.0.16.0 + only,
// lower versions only get 90 if using templates ) instead of 80.
// Added new UOX.INI option : EXTENDEDSTARTINGSKILLS = 0 / 1
// If enabled, allows for four starting skills( selectable in clients 7.0.16.0 + only, lower
// versions only get 4th skill if using templates ) instead of three
//  November 14, 2012	-	Fixed issue where DISPLAYDAMAGENUMBERS and ATTACKSPEEDFROMSTAMINA
//  settings in UOX.INI were overwriting the DISPLAYHITMSG setting instead of their own respective
//  settings November 10, 2013	-	Added new UOX.INI settings for adjusting combat-speed on a
//  global scale:
//									GLOBALATTACKSPEED = 1 	// for
//adjusting speed of melee and ranged attacks globally for all chars
// NPCSPELLCASTSPEED = 1 	// for adjusting the overall speed of spellcasts for NPCs (base
// spellcast speed determined by SPADELAY tag in NPC dfns)
// Added new UOX.INI settings to make hardcoded damage to armor and weapons in combat more
// customizable : 									WEAPONDAMAGECHANCE = 17 	//
// Chance of weapons taking damage when attacking
// WEAPONDAMAGEMIN = 0		// Minimum amount of damage a weapon takes if damaged in combat
// WEAPONDAMAGEMAX = 1		// Maximum amount of damage a weapon takes if damaged in combat
// ARMORDAMAGECHANCE = 25	// Chance of armor taking damage when defending
// ARMORDAMAGEMIN = 0
//// Minimum amount of damage armor can take if damaged in combat
///ARMORDAMAGEMAX = 1		//
// Maximum amount of damage armor can take if damaged in combat
//  September 12, 2015 	- 	Added new UOX.INI setting for defining the amount of stamina lost
//  when using the fishing skill
//									FISHINGSTAMINALOSS = 2	// The amount
//of stamina lost when using the fishing skill
//  November 24, 2019 	- 	Added new uox.ini flag for choosing a random starting location for
//  new players (from list in uox.ini)
//									RANDOMSTARTINGLOCATION = 0	// 0
//to disable (default), 1 to enable

// NOTE:	Very important the first lookups required duplication or the search fails on them

//+++++++++++++++++++++++++++++++++++++++++++++++
auto CServerData::lookupINIValue(const std::string &tag) -> std::int32_t {
    auto iter = uox3IniCaseValue.find(tag);
    if (iter != uox3IniCaseValue.end()) {
        return iter->second;
    }
    return std::numeric_limits<std::int32_t>::max();
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ResetDefaults()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Reset server settings to default
// o------------------------------------------------------------------------------------------------o
auto CServerData::ResetDefaults() -> void {
    resettingDefaults = true;
//    serverList.resize(1); // Set the initial count to hold one server.
    
//    ServerIP("127.0.0.1");
    ServerPort(2593);
//    ExternalIP("127.0.0.1");
//    serverList[0].SetPort(2593);
//    ServerName("My UOX3 Shard");
//    SecretShardKey("None");
    
    // Set default gcMaxBytes limit in MB per JS runtime
    // If set too low, UOX3 might crash when reloading (full) JS engine
    // JS API references describe it as "Maximum nominal heap before last ditch GC"
    SetJSEngineSize(256);
    
    // Send server-originating messages in Unicode format, if possible
//    useUnicodeMessages(true);
    
    ServerLanguage(DL_DEFAULT);
    SystemTimer(tSERVER_POTION, 10);
    serverMoon(0, 0);
    serverMoon(1, 0);
    dungeonLightLevel(15);
    worldLightCurrentLevel(0);
    worldLightBrightLevel(0);
    worldLightDarkLevel(12);
    
    ServerNetRcvTimeout(3);
    ServerNetSndTimeout(3);
    ServerNetRetryCount(3);
    MaxClientBytesIn(25000);
    MaxClientBytesOut(100000);
    NetTrafficTimeban(30);
    
    // Adaptive Performance System
    APSPerfThreshold(50); // Default to 50 simulation cycles as performance threshold
    APSInterval(100);     // Default to APS checking performance at 250 ms intervals
    APSDelayStep(50); // Default to 25 ms as the value at which APS gradually increments the delay
    // to NPC AI/movement checking
    APSDelayMaxCap(2000); // Default to 1500 ms as max cap on delay APS can introduce to NPC
    // AI/movement checking
    
    ServerSecondsPerUOMinute(5);
    ServerTimeDay(0);
    ServerTimeHours(0);
    ServerTimeMinutes(0);
    ServerTimeSeconds(0);
    ServerTimeAMPM(0);
    
    //InternalAccountStatus(true);
//    YoungPlayerSystem(true);
    CombatMaxRange(10);
    CombatMaxSpellRange(10);
    
    // load defaults values
    SystemTimer(tSERVER_SHOPSPAWN, 300);
    SystemTimer(tSERVER_POISON, 180);
    SystemTimer(tSERVER_DECAY, 300);
    SystemTimer(tSERVER_DECAYINHOUSE, 3600);
    ServerSkillTotalCap(7000);
    ServerSkillCap(1000);
    ServerStatCap(225);
//    StatsAffectSkillChecks(false);
//    CorpseLootDecay(true);
    ServerSavesTimer(600);
    
    
    SystemTimer(tSERVER_INVISIBILITY, 60);
    SystemTimer(tSERVER_HUNGERRATE, 6000);
    HungerDamage(2);
//    HungerSystemEnabled(true);
    
    SystemTimer(tSERVER_THIRSTRATE, 6000);
    ThirstDrain(2);
//    ThirstSystemEnabled(false);
    
    ServerSkillDelay(5);
    SystemTimer(tSERVER_OBJECTUSAGE, 1);
    SystemTimer(tSERVER_HITPOINTREGEN, 8);
    SystemTimer(tSERVER_STAMINAREGEN, 3);
    SystemTimer(tSERVER_MANAREGEN, 5);
 //   ArmorAffectManaRegen(true);
//    SnoopIsCrime(false);
//    SnoopAwareness(false);
    SystemTimer(tSERVER_GATE, 30);
    MineCheck(1);
//    DeathAnimationStatus(true);
//    ShowOfflinePCs(true);
//    CombatDisplayHitMessage(true);
//    CombatDisplayDamageNumbers(true);
//    CombatAttackSpeedFromStamina(true);
    CombatAttackStamina(-2);
//    NPCTrainingStatus(true);
//    CharHideWhileMounted(true);
    WeightPerStr(3.5);
    SystemTimer(tSERVER_POLYMORPH, 90);
//    ServerOverloadPackets(true);
//    AdvancedPathfinding(true);
//    LootingIsCrime(true);
//    ServerUOGEnabled(true);
//    FreeshardServerPoll(true);
 //   ServerContextMenus(true);
    SysMsgColour(0x0048);
    
    //CombatMonstersVsAnimals(true);
    CombatAnimalsAttackChance(2);
//    CombatAnimalsGuarded(false);
    CombatNPCBaseFleeAt(20);
    CombatNPCBaseReattackAt(40);
//    ShootOnAnimalBack(false);
//    SellByNameStatus(false);
    SkillLevel(5);
    SellMaxItemsStatus(250);
    CombatNpcDamageRate(1);
 //   RankSystemStatus(true);
//    DisplayMakersMark(true);
    CombatArcheryHitBonus(10);
    CombatArcheryShootDelay(1.0);
    CombatWeaponDamageChance(17);
    CombatWeaponDamageMin(0);
    CombatWeaponDamageMax(1);
    CombatArmorDamageChance(33);
    CombatArmorDamageMin(0);
    CombatArmorDamageMax(1);
    CombatParryDamageChance(20);
    CombatParryDamageMin(0);
    CombatParryDamageMax(1);
    CombatBloodEffectChance(75);
    GlobalAttackSpeed(1.0);
    NPCSpellCastSpeed(1.0);
    FishingStaminaLoss(static_cast<std::uint16_t>(2.0));
//    CombatArmorClassDamageBonus(false);
//    AlchemyDamageBonusEnabled(false);
    AlchemyDamageBonusModifier(5);
//    PetCombatTraining(true);
//    HirelingCombatTraining(true);
 //   NpcCombatTraining(false);
    WeaponDamageBonusType(2);
    
//    CheckPetControlDifficulty(true);
    SetPetLoyaltyGainOnSuccess(1);
    SetPetLoyaltyLossOnFailure(3);
    SystemTimer(tSERVER_LOYALTYRATE, 900);
//    ItemsInterruptCasting(true);
    /*
    auto curWorkingDir = std::filesystem::current_path().string();
    
    auto wDir = oldstrutil::fixDirectory(curWorkingDir);
    std::string tDir;
    Directory(CSDDP_ROOT, wDir);
    tDir = wDir + std::string("muldata/");
    Directory(CSDDP_DATA, tDir);
    tDir = wDir + std::string("dfndata/");
    Directory(CSDDP_DEFS, tDir);
    tDir = wDir + std::string("accounts/");
    Directory(CSDDP_ACCOUNTS, tDir);
    Directory(CSDDP_ACCESS, tDir);
    tDir = wDir + std::string("js/");
    Directory(CSDDP_SCRIPTS, tDir);
    tDir = wDir + std::string("js/jsdata");
    Directory(CSDDP_SCRIPTDATA, tDir);
    tDir = wDir + std::string("archives/");
    Directory(CSDDP_BACKUP, tDir);
    tDir = wDir + std::string("msgboards/");
    Directory(CSDDP_MSGBOARD, tDir);
    tDir = wDir + std::string("shared/");
    Directory(CSDDP_SHARED, tDir);
    tDir = wDir + std::string("html/");
    Directory(CSDDP_HTML, tDir);
    tDir = wDir + std::string("books/");
    Directory(CSDDP_BOOKS, tDir);
    tDir = wDir + std::string("dictionaries/");
    Directory(CSDDP_DICTIONARIES, tDir);
    tDir = wDir + std::string("logs/");
    Directory(CSDDP_LOGS, tDir);
    */
    // Expansion settings
    /*
    ExpansionCoreShardEra(ER_LBR); // Default to LBR expansion
    ExpansionArmorCalculation(ER_CORE);
    ExpansionStrengthDamageBonus(ER_CORE);
    ExpansionTacticsDamageBonus(ER_CORE);
    ExpansionAnatomyDamageBonus(ER_CORE);
    ExpansionLumberjackDamageBonus(ER_CORE);
    ExpansionRacialDamageBonus(ER_CORE);
    ExpansionDamageBonusCap(ER_CORE);
    ExpansionShieldParry(ER_CORE);
    ExpansionWeaponParry(ER_CORE);
    ExpansionWrestlingParry(ER_CORE);
    ExpansionCombatHitChance(ER_CORE);
    */
    BuyThreshold(2000);
 //   GuardStatus(true);
//    ServerAnnounceSaves(true);
    WorldAmbientSounds(5);
//    ServerJoinPartAnnouncements(true);
//    ServerConsoleLog(true);
//    ServerNetworkLog(false);
//    ServerSpeechLog(false);
//    RogueStatus(true);
    SystemTimer(tSERVER_WEATHER, 60);
    SystemTimer(tSERVER_LOGINTIMEOUT, 300);
    BackupRatio(5);
    MaxStealthMovement(10);
    MaxStaminaMovement(15);
    SystemTimer(tSERVER_NPCFLAGUPDATETIMER, 10);
    PetOfflineTimeout(5);
//    PetHungerOffline(true);
    SystemTimer(tSERVER_PETOFFLINECHECK, 600);
//    ItemsDetectSpeech(false);
    MaxPlayerPackItems(125);
    MaxPlayerBankItems(125);
    MaxPlayerPackWeight(40000);  // 400.00 stones
    MaxPlayerBankWeight(160000); // 1600.00 stones
//    ForceNewAnimationPacket(true);
//    MapDiffsEnabled(false);
//    TravelSpellsFromBoatKeys(true);
//    TravelSpellsWhileOverweight(false);
//    MarkRunesInMultis(true);
//    TravelSpellsBetweenWorlds(false);
 //   TravelSpellsWhileAggressor(false);
 //   CastSpellsWhileMoving(false);
    MaxControlSlots(0); // Default to 0, which is equal to off
    MaxFollowers(5);
    MaxPetOwners(10);
//    ToolUseLimit(true);
//    ToolUseBreak(true);
//    ItemRepairDurabilityLoss(true);
//    HideStatsForUnknownMagicItems(true);
//    CraftColouredWeapons(false);
    MaxSafeTeleportsPerDay(1);
//    TeleportToNearestSafeLocation(false);
//    AllowAwakeNPCs(true);
    GlobalRestockMultiplier(1.0);
//    ShowItemResistStats(false);
//    ShowWeaponDamageTypes(true);
//    ShowReputationTitleInTooltip(true);
//    ShowGuildInfoInTooltip(true);
//    EnableNPCGuildDiscounts(true);
 //   EnableNPCGuildPremiums(true);
    
    CheckBoatSpeed(0.65);
    CheckNpcAISpeed(1);
//    CutScrollRequirementStatus(true);
    //PlayerPersecutionStatus(false);
//    HtmlStatsStatus(-1);
    
    MsgBoardPostingLevel(0);
    MsgBoardPostRemovalLevel(0);
    // No replacement I can see
//    EscortsEnabled(true);
//    BasicTooltipsOnly(false);
//    ShowNpcTitlesInTooltips(true);
//    ShowNpcTitlesOverhead(true);
//    ShowInvulnerableTagOverhead(false);
//    ShowRaceWithName(true);
//    ShowRaceInPaperdoll(true);
//    GlobalItemDecay(true);
//    ScriptItemsDecayable(true);
//    BaseItemsDecayable(false);
    SystemTimer(tSERVER_ESCORTWAIT, 900);
    SystemTimer(tSERVER_ESCORTACTIVE, 600);
    SystemTimer(tSERVER_ESCORTDONE, 600);
//    AmbientFootsteps(false);
//    ServerCommandPrefix('\'');
    
    CheckSpawnRegionSpeed(30);
    CheckItemsSpeed(1.5);
    NPCWalkingSpeed(0.6);
    NPCRunningSpeed(0.3);
    NPCFleeingSpeed(0.4);
    NPCMountedWalkingSpeed(0.3);
    NPCMountedRunningSpeed(0.12);
    NPCMountedFleeingSpeed(0.2);
    AccountFlushTimer(5.0);
    
    // RESOURCES
    ResourceAreaSize(8);
    ResLogs(3);
    ResLogTime(600);
    ResOre(10);
    ResOreTime(600);
    ResFish(10);
    ResFishTime(600);
    
    // REPSYS
    SystemTimer(tSERVER_CRIMINAL, 120);
    RepMaxKills(4);
    SystemTimer(tSERVER_MURDERDECAY, 28800);
    SystemTimer(tSERVER_AGGRESSORFLAG, 120);
    SystemTimer(tSERVER_PERMAGREYFLAG, 0); // No duration by default
    SystemTimer(tSERVER_STEALINGFLAG, 120);
    SystemTimer(tSERVER_COMBATIGNORE, 20); // Not in ini
    
    // RepSys ---^
    TrackingBaseRange(10);
    TrackingMaxTargets(20);
    TrackingBaseTimer(30);
    TrackingRedisplayTime(30);
    
    SystemTimer(tSERVER_FISHINGBASE, 10);
    SystemTimer(tSERVER_FISHINGRANDOM, 5);
    SystemTimer(tSERVER_SPIRITSPEAK, 30);
    
    TitleColour(0);
    LeftTextColour(0);
    RightTextColour(0);
    ButtonCancel(4017);
    ButtonLeft(4014);
    ButtonRight(4005);
    BackgroundPic(5054);
    
    // Houses
//    ItemDecayInHouses(true);
//    ProtectPrivateHouses(true);
//    TrackHousesPerAccount(true);
//    CanOwnAndCoOwnHouses(true);
//    CoOwnHousesOnSameAccount(true);
    MaxHousesOwnable(1);
    MaxHousesCoOwnable(10);
//    SafeCoOwnerLogout(true);
//    SafeFriendLogout(true);
//    SafeGuestLogout(true);
//    KeylessOwnerAccess(true);
//    KeylessCoOwnerAccess(true);
 //   KeylessFriendAccess(true);
 //   KeylessGuestAccess(false);
    
    // Bulk Order Deeds
//    OfferBODsFromItemSales(true);
//    OfferBODsFromContextMenu(false);
//    BODsFromCraftedItemsOnly(false);
    BODGoldRewardMultiplier(1.0);
    BODFameRewardMultiplier(1.0);
    
    // Towns
    TownNumSecsPollOpen(3600); // one hour
    TownNumSecsAsMayor(36000); // 10 hours as mayor
    TownTaxPeriod(1800);       // taxed every 30 minutes
    TownGuardPayment(3600);    // guards paid every hour
    
    
    
    // Disable spawn regions for all facets by default
    setSpawnRegionsFacetStatus(0);
    
    
//    ExtendedStartingStats(true);
//    ExtendedStartingSkills(true);
    
//    ServerRandomStartingLocation(false);
    ServerStartGold(1000);
    serverStartPrivs(0);
    SystemTimer(tSERVER_CORPSEDECAY, 420);
    SystemTimer(tSERVER_NPCCORPSEDECAY, 420);
    SystemTimer(tSERVER_BLOODDECAYCORPSE, 210); // Default to half the decay timer of a npc corpse
    SystemTimer(tSERVER_BLOODDECAY, 3);         // Keep it short and sweet
    resettingDefaults = false;
}
//==================================================================================================
CServerData::CServerData() {
    startup();
    availableIPs = ip4list_t::available();
}
//==================================================================================================
auto CServerData::startup() -> void { ResetDefaults(); }


/*
// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ServerDomain()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets server domain. Sets to specified value, or to no domain if
// no value specified
// o------------------------------------------------------------------------------------------------o
auto CServerData::ServerDomain() const -> const std::string & { return serverList[0].GetDomain(); }
auto CServerData::ServerDomain(const std::string &setdomain) -> void {
    if (serverList.empty()) {
        serverList.resize(1);
    }
    if (setdomain.empty()) {
        serverList[0].SetDomain("");
    }
    else {
        serverList[0].SetDomain(setdomain);
    }
}
*/
auto CServerData::matchIP(const IP4Addr &ip) const -> IP4Addr {
    auto [candidate, match] = availableIPs.bestmatch(ip);
    if (match == 0) {
        if (!ServerConfig::shared().serverString[ServerString::PUBLICIP].empty()) {
            candidate = IP4Addr(ServerConfig::shared().serverString[ServerString::PUBLICIP]);
        }
    }
    else {
        // We got some kind of match, see if on same network type?
        if (candidate.type() != ip.type()) {
            if (ip.type() == IP4Addr::ip4type_t::wan) {
                if (!ServerConfig::shared().serverString[ServerString::PUBLICIP].empty()) {
                    candidate = IP4Addr(ServerConfig::shared().serverString[ServerString::PUBLICIP]);
                }
            }
        }
    }
    return candidate;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ServerPort()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Set server port to specified value, or to default port if no
// value specified
// o------------------------------------------------------------------------------------------------o
auto CServerData::ServerPort() const -> std::uint16_t { return port; }
auto CServerData::ServerPort(std::uint16_t setport) -> void {
    if (setport == 0) {
        port = 2593;
    }
    else {
        port = setport;
    }
}




// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::SysMsgColour()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default text colour for system messages displayed in
// bottom left
// o------------------------------------------------------------------------------------------------o
auto CServerData::SysMsgColour() const -> std::uint16_t { return sysMsgColour; }
auto CServerData::SysMsgColour(std::uint16_t value) -> void { sysMsgColour = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ServerSavesTimerStatus()
//|					CServerData::ServerSavesTimer()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets server world save timer
// o------------------------------------------------------------------------------------------------o
auto CServerData::ServerSavesTimerStatus() const -> std::uint32_t { return serverSavesTimer; }
auto CServerData::ServerSavesTimer(std::uint32_t timer) -> void {
    serverSavesTimer = timer;
    if (timer < 180) // 3 minutes is the lowest value you can set saves for
    {
        serverSavesTimer = 300; // 10 minutes default
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ServerSkillCapStatus()
//|					CServerData::ServerSkillCap()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets cap for individual skills
// o------------------------------------------------------------------------------------------------o
auto CServerData::ServerSkillCapStatus() const -> std::uint16_t { return skillCap; }
auto CServerData::ServerSkillCap(std::uint16_t cap) -> void {
    skillCap = cap;
    if (cap < 1) // Default is on second loop sleeping
    {
        skillCap = SKILLCAP;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ServerSkillTotalCapStatus()
//|					CServerData::ServerSkillTotalCap()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets total cap for all skills
// o------------------------------------------------------------------------------------------------o
auto CServerData::ServerSkillTotalCapStatus() const -> std::uint16_t { return skillTotalCap; }
auto CServerData::ServerSkillTotalCap(std::uint16_t cap) -> void {
    skillTotalCap = cap;
    if (cap < 1) // Default is on second loop sleeping
    {
        skillTotalCap = SKILLTOTALCAP;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ServerSkillDelayStatus()
//|					CServerData::ServerSkillDelay()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global delay for skill usage
// o------------------------------------------------------------------------------------------------o
auto CServerData::ServerSkillDelayStatus() const -> std::uint8_t { return skillDelay; }
auto CServerData::ServerSkillDelay(std::uint8_t skdelay) -> void { skillDelay = skdelay; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ServerStatCapStatus()
//|					CServerData::ServerStatCap()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the total stat cap
// o------------------------------------------------------------------------------------------------o
auto CServerData::ServerStatCapStatus() const -> std::uint16_t { return statCap; }
auto CServerData::ServerStatCap(std::uint16_t cap) -> void {
    statCap = cap;
    if (cap < 1) // Default is on second loop sleeping
    {
        statCap = STATCAP;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::MaxStealthMovement()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max number of steps allowed to take while stealthed
// o------------------------------------------------------------------------------------------------o
auto CServerData::MaxStealthMovement() const -> std::int16_t { return maxStealthMovement; }
auto CServerData::MaxStealthMovement(std::int16_t value) -> void { maxStealthMovement = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::MaxStaminaMovement()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max number of steps allowed while running before
// stamina is reduced
// o------------------------------------------------------------------------------------------------o
auto CServerData::MaxStaminaMovement() const -> std::int16_t { return maxStaminaMovement; }
auto CServerData::MaxStaminaMovement(std::int16_t value) -> void { maxStaminaMovement = value; }

auto CServerData::BuildSystemTimeValue(csd_tid_t timerId) const -> timerval_t {
    return BuildTimeValue(static_cast<R32>(SystemTimer(timerId)));
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::SystemTimer()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets a specific server timer
// o------------------------------------------------------------------------------------------------o
auto CServerData::SystemTimer(csd_tid_t timerid) const -> std::uint16_t { return serverTimers[timerid]; }
auto CServerData::SystemTimer(csd_tid_t timerid, std::uint16_t value) -> void {
    serverTimers[timerid] = value;
}


// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::DumpPaths()
//|	Date		-	02/26/2002
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Needed a function that would dump out the paths. If you add any paths to the
//|					server please make sure that you place exports to the console here to please
//|                 so it can be looked up if needed.
// o------------------------------------------------------------------------------------------------o
auto CServerData::DumpPaths() -> void {
    Console::shared().printSectionBegin();
    Console::shared() << "    Root        : " << ServerConfig::shared().unwindDirectoryFor(dirlocation_t::BASE).string() << "\n";
    Console::shared() << "    Accounts    : " << ServerConfig::shared().unwindDirectoryFor(dirlocation_t::ACCOUNT).string() << "\n";
    Console::shared() << "    Access      : " << ServerConfig::shared().unwindDirectoryFor(dirlocation_t::ACCESS).string() << "\n";
    Console::shared() << "    Mul(Data)   : " << ServerConfig::shared().unwindDirectoryFor(dirlocation_t::UODIR).string() << "\n";
    Console::shared() << "    DFN(Defs)   : " << ServerConfig::shared().unwindDirectoryFor(dirlocation_t::DEFINITION).string() << "\n";
    Console::shared() << "    JScript     : " << ServerConfig::shared().unwindDirectoryFor(dirlocation_t::SCRIPT).string() << "\n";
    Console::shared() << "    JScriptData : " << ServerConfig::shared().unwindDirectoryFor(dirlocation_t::SCRIPTDATA).string() << "\n";
    Console::shared() << "    HTML        : " << ServerConfig::shared().unwindDirectoryFor(dirlocation_t::HTML).string() << "\n";
    Console::shared() << "    MSGBoards   : " << ServerConfig::shared().unwindDirectoryFor(dirlocation_t::MSGBOARD).string() << "\n";
    Console::shared() << "    Books       : " << ServerConfig::shared().unwindDirectoryFor(dirlocation_t::BOOK).string() << "\n";
    Console::shared() << "    Shared      : " << ServerConfig::shared().unwindDirectoryFor(dirlocation_t::SAVE).string() << "\n";
    Console::shared() << "    Backups     : " << ServerConfig::shared().unwindDirectoryFor(dirlocation_t::BACKUP).string() << "\n";
    Console::shared() << "    Logs        : " << ServerConfig::shared().unwindDirectoryFor(dirlocation_t::LOG).string() << "\n";
    Console::shared().printSectionBegin();
}




// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::WorldAmbientSounds()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets intensity of ambient world sounds, bird chirps, animal
// sounds, etc
// o------------------------------------------------------------------------------------------------o
auto CServerData::WorldAmbientSounds() const -> std::int16_t { return ambientSounds; }
auto CServerData::WorldAmbientSounds(std::int16_t value) -> void {
    if (value < 0) {
        ambientSounds = 0;
    }
    else {
        ambientSounds = value;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::GlobalRestockMultiplier()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global restock multiplier, which is applied to restock
// property |					of items as they are loaded in from the DFNs
// o------------------------------------------------------------------------------------------------o
auto CServerData::GlobalRestockMultiplier() const -> float { return globalRestockMultiplier; }
auto CServerData::GlobalRestockMultiplier(R32 value) -> void { globalRestockMultiplier = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::BODGoldRewardMultiplier()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets a multiplier that adjusts amount of Gold rewarded to
// players for |					handing in completed Bulk Order Deeds
// o------------------------------------------------------------------------------------------------o
auto CServerData::BODGoldRewardMultiplier() const -> float { return bodGoldRewardMultiplier; }
auto CServerData::BODGoldRewardMultiplier(R32 value) -> void { bodGoldRewardMultiplier = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::BODFameRewardMultiplier()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets a multiplier that adjusts amount of Fame rewarded to
// players for |					handing in completed Bulk Order Deeds
// o------------------------------------------------------------------------------------------------o
auto CServerData::BODFameRewardMultiplier() const -> float { return bodFameRewardMultiplier; }
auto CServerData::BODFameRewardMultiplier(R32 value) -> void { bodFameRewardMultiplier = value; }


// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::SellMaxItemsStatus()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum amount of items that can be sold in one go to
// a vendor
// o------------------------------------------------------------------------------------------------o
auto CServerData::SellMaxItemsStatus() const -> std::int16_t { return sellMaxItems; }
auto CServerData::SellMaxItemsStatus(std::int16_t value) -> void { sellMaxItems = value; }


// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CheckItemsSpeed()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets how often in seconds items are checked for decay and other
// things
// o------------------------------------------------------------------------------------------------o
auto CServerData::CheckItemsSpeed() const -> double { return checkItems; }
auto CServerData::CheckItemsSpeed(R64 value) -> void {
    if (value < 0.0) {
        checkItems = 0.0;
    }
    else {
        checkItems = value;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CheckBoatSpeed()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets how often in seconds boats are checked for decay and other
// things
// o------------------------------------------------------------------------------------------------o
auto CServerData::CheckBoatSpeed() const -> double { return checkBoats; }
auto CServerData::CheckBoatSpeed(R64 value) -> void {
    if (value < 0.0) {
        checkBoats = 0.0;
    }
    else {
        checkBoats = value;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CheckNpcAISpeed()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets how often in seconds NPCs will execute their AI routines
// o------------------------------------------------------------------------------------------------o
auto CServerData::CheckNpcAISpeed() const -> double { return checkNpcAi; }
auto CServerData::CheckNpcAISpeed(R64 value) -> void {
    if (value < 0.0) {
        checkNpcAi = 0.0;
    }
    else {
        checkNpcAi = value;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CheckSpawnRegionSpeed()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets how often (in seconds) spawn regions are checked for new
// spawns
// o------------------------------------------------------------------------------------------------o
auto CServerData::CheckSpawnRegionSpeed() const -> double { return checkSpawnRegions; }
auto CServerData::CheckSpawnRegionSpeed(R64 value) -> void {
    if (value < 0.0) {
        checkSpawnRegions = 0.0;
    }
    else {
        checkSpawnRegions = value;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::MsgBoardPostingLevel()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the posting level for bulletin boards
//|					UNUSED?
// o------------------------------------------------------------------------------------------------o
auto CServerData::MsgBoardPostingLevel() const -> std::uint8_t { return msgPostingLevel; }
auto CServerData::MsgBoardPostingLevel(std::uint8_t value) -> void { msgPostingLevel = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::MsgBoardPostRemovalLevel()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the post removal level for bulleting boards
//|					UNUSED?
// o------------------------------------------------------------------------------------------------o
auto CServerData::MsgBoardPostRemovalLevel() const -> std::uint8_t { return msgRemovalLevel; }
auto CServerData::MsgBoardPostRemovalLevel(std::uint8_t value) -> void { msgRemovalLevel = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::MineCheck()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the type of check used when players attempt to use the
// mining skill |						0 = mine anywhere |
// 1 = mine mountainsides/cavefloors only |						2 = mine in
// mining regions only (not working?)
// o------------------------------------------------------------------------------------------------o
auto CServerData::MineCheck() const -> std::uint8_t { return mineCheck; }
auto CServerData::MineCheck(std::uint8_t value) -> void { mineCheck = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::GlobalAttackSpeed()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global attack speed in combat
// o------------------------------------------------------------------------------------------------o
auto CServerData::GlobalAttackSpeed() const -> float { return globalAttackSpeed; }
auto CServerData::GlobalAttackSpeed(R32 value) -> void {
    if (value < 0.0) {
        globalAttackSpeed = 0.0;
    }
    else {
        globalAttackSpeed = value;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::NPCSpellCastSpeed()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global NPC spell casting speed
// o------------------------------------------------------------------------------------------------o
auto CServerData::NPCSpellCastSpeed() const -> float { return npcSpellcastSpeed; }
auto CServerData::NPCSpellCastSpeed(R32 value) -> void {
    if (value < 0.0) {
        npcSpellcastSpeed = 0.0;
    }
    else {
        npcSpellcastSpeed = value;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::FishingStaminaLoss()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the stamina loss for using the fishing skill
// o------------------------------------------------------------------------------------------------o
auto CServerData::FishingStaminaLoss() const -> std::int16_t { return fishingstaminaloss; }
auto CServerData::FishingStaminaLoss(std::int16_t value) -> void { fishingstaminaloss = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatNpcDamageRate()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the NPC damage divisor. If character is a player, damage
// from NPCs is |					divided by this value.
// o------------------------------------------------------------------------------------------------o
auto CServerData::CombatNpcDamageRate() const -> std::int16_t { return combatNpcDamageRate; }
auto CServerData::CombatNpcDamageRate(std::int16_t value) -> void { combatNpcDamageRate = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::AlchemyDamageBonusModifier()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the Alchemy Damage Bonus Modifier, which gives bonus
// damage to |					explosion potions based on this formula: |
// bonusDamage = attackerAlchemySkill / alchemyDamageBonusModifier
// o------------------------------------------------------------------------------------------------o
auto CServerData::AlchemyDamageBonusModifier() const -> std::uint8_t { return alchemyDamageBonusModifier; }
auto CServerData::AlchemyDamageBonusModifier(std::uint8_t value) -> void {
    alchemyDamageBonusModifier = value;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::WeaponDamageBonusType()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the Weapon Damage Bonus Type, which determines how bonuses
// to weapon |					damage is applied to weapons:
//|						0 - Apply flat weapon damage bonus to .hidamage
//|						1 - Split weapon damage bonus between .lodamage and
//.hidamage
//|						2 - Apply flat weapon damage bonus to BOTH .lodamage
//and .hidamage (Default)
// o------------------------------------------------------------------------------------------------o
auto CServerData::WeaponDamageBonusType() const -> std::uint8_t { return combatWeaponDamageBonusType; }
auto CServerData::WeaponDamageBonusType(std::uint8_t value) -> void { combatWeaponDamageBonusType = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatAttackStamina()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the amount of stamina lost by swinging weapon in combat
// o------------------------------------------------------------------------------------------------o
auto CServerData::CombatAttackStamina() const -> std::int16_t { return combatAttackStamina; }
auto CServerData::CombatAttackStamina(std::int16_t value) -> void { combatAttackStamina = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::SkillLevel()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global difficulty level for crafting items
//|					1 = easy, 5 = default, 10 = difficult
// o------------------------------------------------------------------------------------------------o
auto CServerData::SkillLevel() const -> std::uint8_t { return skillLevel; }
auto CServerData::SkillLevel(std::uint8_t value) -> void { skillLevel = value; }



// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::MaxPlayerPackItems()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max item capacity for player (and NPC) backpacks
// o------------------------------------------------------------------------------------------------o
auto CServerData::MaxPlayerPackItems() const -> std::uint16_t { return maxPlayerPackItems; }
auto CServerData::MaxPlayerPackItems(std::uint16_t newVal) -> void { maxPlayerPackItems = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::MaxPlayerPackWeight()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max weight capacity for player (and NPC) backpacks
// o------------------------------------------------------------------------------------------------o
auto CServerData::MaxPlayerPackWeight() const -> std::int32_t { return maxPlayerPackWeight; }
auto CServerData::MaxPlayerPackWeight(std::int32_t newVal) -> void { maxPlayerPackWeight = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::MaxPlayerBankItems()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max item capacity for player bankboxes
// o------------------------------------------------------------------------------------------------o
auto CServerData::MaxPlayerBankItems() const -> std::uint16_t { return maxPlayerBankItems; }
auto CServerData::MaxPlayerBankItems(std::uint16_t newVal) -> void { maxPlayerBankItems = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::MaxPlayerBankWeight()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max weight capacity for player (and NPC) bankboxes
// o------------------------------------------------------------------------------------------------o
auto CServerData::MaxPlayerBankWeight() const -> std::int32_t { return maxPlayerBankWeight; }
auto CServerData::MaxPlayerBankWeight(std::int32_t newVal) -> void { maxPlayerBankWeight = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::MaxHousesOwnable()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max amount of houses that a player can own
// o------------------------------------------------------------------------------------------------o
auto CServerData::MaxHousesOwnable() const -> std::uint16_t { return maxHousesOwnable; }
auto CServerData::MaxHousesOwnable(std::uint16_t value) -> void { maxHousesOwnable = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::MaxHousesCoOwnable()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max amount of houses that a player can co-own
// o------------------------------------------------------------------------------------------------o
auto CServerData::MaxHousesCoOwnable() const -> std::uint16_t { return maxHousesCoOwnable; }
auto CServerData::MaxHousesCoOwnable(std::uint16_t value) -> void { maxHousesCoOwnable = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatAnimalsAttackChance()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the chance of monsters attacking animals
// o------------------------------------------------------------------------------------------------o
auto CServerData::CombatAnimalsAttackChance() const -> std::uint16_t { return combatAnimalAttackChance; }
auto CServerData::CombatAnimalsAttackChance(std::uint16_t value) -> void {
    if (value > 1000) {
        value = 1000;
    }
    combatAnimalAttackChance = value;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatArcheryHitBonus()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the delay hit bonus (in percentage) in combat for Archery.
// Can be negative!
//|					This bonus was mentioned in official patch notes for Publish 5
//(UOR patch).
// o------------------------------------------------------------------------------------------------o
auto CServerData::CombatArcheryHitBonus() const -> std::int8_t { return combatArcheryHitBonus; }
auto CServerData::CombatArcheryHitBonus(std::int8_t value) -> void {
    if (value > 100) {
        value = 100;
    }
    combatArcheryHitBonus = value;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatArcheryShootDelay()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the delay (in seconds, with decimals) after archers stop
// moving until |					they can fire a shot in combat
// o------------------------------------------------------------------------------------------------o
auto CServerData::CombatArcheryShootDelay() const -> float { return archeryShootDelay; }
auto CServerData::CombatArcheryShootDelay(R32 value) -> void {
    if (value < 0.0) {
        archeryShootDelay = 0.0;
    }
    else {
        archeryShootDelay = value;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatWeaponDamageChance()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets chance for weapons to take damage in combat (on hit)
// o------------------------------------------------------------------------------------------------o
auto CServerData::CombatWeaponDamageChance() const -> std::uint8_t { return combatWeaponDamageChance; }
auto CServerData::CombatWeaponDamageChance(std::uint8_t value) -> void {
    if (value > 100) {
        value = 100;
    }
    combatWeaponDamageChance = value;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatWeaponDamageMin()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the min amount of damage (in hitpoints) weapons will take
// from combat
// o------------------------------------------------------------------------------------------------o
auto CServerData::CombatWeaponDamageMin() const -> std::uint8_t { return combatWeaponDamageMin; }
auto CServerData::CombatWeaponDamageMin(std::uint8_t value) -> void { combatWeaponDamageMin = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatWeaponDamageMax()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max amount of damage (in hitpoints) weapons will take
// from combat
// o------------------------------------------------------------------------------------------------o
auto CServerData::CombatWeaponDamageMax() const -> std::uint8_t { return combatWeaponDamageMax; }
auto CServerData::CombatWeaponDamageMax(std::uint8_t value) -> void { combatWeaponDamageMax = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatArmorDamageChance()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the chance to damage armor in combat
// o------------------------------------------------------------------------------------------------o
auto CServerData::CombatArmorDamageChance() const -> std::uint8_t { return combatArmorDamageChance; }
auto CServerData::CombatArmorDamageChance(std::uint8_t value) -> void {
    if (value > 100) {
        value = 100;
    }
    combatArmorDamageChance = value;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatArmorDamageMin()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the min damage dealt to armor in combat
// o------------------------------------------------------------------------------------------------o
auto CServerData::CombatArmorDamageMin() const -> std::uint8_t { return combatArmorDamageMin; }
auto CServerData::CombatArmorDamageMin(std::uint8_t value) -> void { combatArmorDamageMin = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatArmorDamageMax()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max damage dealt to armor in combat
// o------------------------------------------------------------------------------------------------o
auto CServerData::CombatArmorDamageMax() const -> std::uint8_t { return combatArmorDamageMax; }
auto CServerData::CombatArmorDamageMax(std::uint8_t value) -> void { combatArmorDamageMax = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatParryDamageChance()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the chance to damage items used to parry in combat
// o------------------------------------------------------------------------------------------------o
auto CServerData::CombatParryDamageChance() const -> std::uint8_t { return combatParryDamageChance; }
auto CServerData::CombatParryDamageChance(std::uint8_t value) -> void {
    if (value > 100) {
        value = 100;
    }
    combatParryDamageChance = value;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatParryDamageMin()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the min damage dealt to items used to parry in combat
// o------------------------------------------------------------------------------------------------o
auto CServerData::CombatParryDamageMin() const -> std::uint8_t { return combatParryDamageMin; }
auto CServerData::CombatParryDamageMin(std::uint8_t value) -> void { combatParryDamageMin = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatParryDamageMax()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max damage dealt to items used to parry in combat
// o------------------------------------------------------------------------------------------------o
auto CServerData::CombatParryDamageMax() const -> std::uint8_t { return combatParryDamageMax; }
auto CServerData::CombatParryDamageMax(std::uint8_t value) -> void { combatParryDamageMax = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatBloodEffectChance()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the chance to spawn blood splatter effects during combat
// o------------------------------------------------------------------------------------------------o
auto CServerData::CombatBloodEffectChance() const -> std::uint8_t { return combatBloodEffectChance; }
auto CServerData::CombatBloodEffectChance(std::uint8_t value) -> void {
    if (value > 100) {
        value = 100;
    }
    combatBloodEffectChance = value;
}



// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::HungerDamage()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the damage taken from players being hungry
// o------------------------------------------------------------------------------------------------o
auto CServerData::HungerDamage() const -> std::int16_t { return hungerDamage; }
auto CServerData::HungerDamage(std::int16_t value) -> void { hungerDamage = value; }

// o------------------------------------------------------------------------------------------------o
//| Function    -   CServerData::ThirstDrain()
// o------------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets the stamina drain from players being thirsty
// o------------------------------------------------------------------------------------------------o
auto CServerData::ThirstDrain() const -> std::int16_t { return thirstDrain; }
auto CServerData::ThirstDrain(std::int16_t value) -> void { thirstDrain = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::PetOfflineTimeout()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the interval in seconds between checks for the player
// offline time
// o------------------------------------------------------------------------------------------------o
auto CServerData::PetOfflineTimeout() const -> std::uint16_t { return petOfflineTimeout; }
auto CServerData::PetOfflineTimeout(std::uint16_t value) -> void { petOfflineTimeout = value; }


// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::BuyThreshold()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the threshold for when money is taken from a player's bank
// account
//|					when buying something from a vendor instead of from their
// backpack
// o------------------------------------------------------------------------------------------------o
auto CServerData::BuyThreshold() const -> std::int16_t { return buyThreshold; }
auto CServerData::BuyThreshold(std::int16_t value) -> void { buyThreshold = value; }
// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::WeightPerStr()
//|	Date		-	3/12/2003
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets amount of weight one can hold per point of STR
// o------------------------------------------------------------------------------------------------o
auto CServerData::WeightPerStr() const -> float { return weightPerSTR; }
auto CServerData::WeightPerStr(R32 newVal) -> void { weightPerSTR = newVal; }



// o------------------------------------------------------------------------------------------------o
//| Function    -   CServerData::MaxControlSlots()
// o------------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets the max amount of control slots a player has available (0 for disable)
// o------------------------------------------------------------------------------------------------o
auto CServerData::MaxControlSlots() const -> std::uint8_t { return maxControlSlots; }
auto CServerData::MaxControlSlots(std::uint8_t newVal) -> void { maxControlSlots = newVal; }

// o------------------------------------------------------------------------------------------------o
//| Function    -   CServerData::MaxFollowers()
// o------------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets the max amount of active followers/pets a player can have (0 for
// disable)
// o------------------------------------------------------------------------------------------------o
auto CServerData::MaxFollowers() const -> std::uint8_t { return maxFollowers; }
auto CServerData::MaxFollowers(std::uint8_t newVal) -> void { maxFollowers = newVal; }

// o------------------------------------------------------------------------------------------------o
//| Function    -  CServerData::MaxPetOwners()
// o------------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets the max amount of different owners a pet can have in its lifetime
// o------------------------------------------------------------------------------------------------o
auto CServerData::MaxPetOwners() const -> std::uint8_t { return maxPetOwners; }
auto CServerData::MaxPetOwners(std::uint8_t newVal) -> void { maxPetOwners = newVal; }

// o------------------------------------------------------------------------------------------------o
//| Function    -   CServerData::GetPetLoyaltyGainOnSuccess()
// o------------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets the pet loyalty gained on successful use of pet command
// o------------------------------------------------------------------------------------------------o
auto CServerData::GetPetLoyaltyGainOnSuccess() const -> std::uint16_t { return petLoyaltyGainOnSuccess; }
auto CServerData::SetPetLoyaltyGainOnSuccess(std::uint16_t newVal) -> void {
    petLoyaltyGainOnSuccess = newVal;
}

// o------------------------------------------------------------------------------------------------o
//| Function    -   CServerData::GetPetLoyaltyLossOnFailure()
// o------------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets the pet loyalty loss for failed use of pet command
// o------------------------------------------------------------------------------------------------o
auto CServerData::GetPetLoyaltyLossOnFailure() const -> std::uint16_t { return petLoyaltyLossOnFailure; }
auto CServerData::SetPetLoyaltyLossOnFailure(std::uint16_t newVal) -> void {
    petLoyaltyLossOnFailure = newVal;
}

// o------------------------------------------------------------------------------------------------o
//| Function    -   CServerData::MaxSafeTeleportsPerDay()
// o------------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets the max amount of teleports to safety players get per day via help
// menu when stuck
// o------------------------------------------------------------------------------------------------o
auto CServerData::MaxSafeTeleportsPerDay() const -> std::uint8_t { return maxSafeTeleports; }
auto CServerData::MaxSafeTeleportsPerDay(std::uint8_t newVal) -> void { maxSafeTeleports = newVal; }


// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::BackupRatio()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the ratio of worldsaves that get backed up
// o------------------------------------------------------------------------------------------------o
auto CServerData::BackupRatio() const -> std::int16_t { return backupRatio; }
auto CServerData::BackupRatio(std::int16_t value) -> void { backupRatio = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatMaxRange()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum range at which combat can be engaged
// o------------------------------------------------------------------------------------------------o
auto CServerData::CombatMaxRange() const -> std::int16_t { return combatMaxRange; }
auto CServerData::CombatMaxRange(std::int16_t value) -> void { combatMaxRange = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatMaxSpellRange()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum range at which spells can be cast in combat
// o------------------------------------------------------------------------------------------------o
auto CServerData::CombatMaxSpellRange() const -> std::int16_t { return combatMaxSpellRange; }
auto CServerData::CombatMaxSpellRange(std::int16_t value) -> void { combatMaxSpellRange = value; }


// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatNPCBaseFleeAt()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global health threshold where NPCs start fleeing in
// combat
// o------------------------------------------------------------------------------------------------o
auto CServerData::CombatNPCBaseFleeAt() const -> std::int16_t { return combatNpcBaseFleeAt; }
auto CServerData::CombatNPCBaseFleeAt(std::int16_t value) -> void { combatNpcBaseFleeAt = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatNPCBaseReattackAt()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global health threshold where NPCs reattack after
// fleeing in combat
// o------------------------------------------------------------------------------------------------o
auto CServerData::CombatNPCBaseReattackAt() const -> std::int16_t { return combatNpcBaseReattackAt; }
auto CServerData::CombatNPCBaseReattackAt(std::int16_t value) -> void { combatNpcBaseReattackAt = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::NPCWalkingSpeed()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global, default walking speed for NPCs
// o------------------------------------------------------------------------------------------------o
auto CServerData::NPCWalkingSpeed() const -> float { return npcWalkingSpeed; }
auto CServerData::NPCWalkingSpeed(R32 value) -> void { npcWalkingSpeed = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::NPCRunningSpeed()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global, default running speed for NPCs
// o------------------------------------------------------------------------------------------------o
auto CServerData::NPCRunningSpeed() const -> float { return npcRunningSpeed; }
auto CServerData::NPCRunningSpeed(R32 value) -> void { npcRunningSpeed = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::NPCFleeingSpeed()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global, default speed at which NPCs flee in combat
// o------------------------------------------------------------------------------------------------o
auto CServerData::NPCFleeingSpeed() const -> float { return npcFleeingSpeed; }
auto CServerData::NPCFleeingSpeed(R32 value) -> void { npcFleeingSpeed = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::NPCMountedWalkingSpeed()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global, default walking speed for mounted NPCs
// o------------------------------------------------------------------------------------------------o
auto CServerData::NPCMountedWalkingSpeed() const -> float { return npcMountedWalkingSpeed; }
auto CServerData::NPCMountedWalkingSpeed(R32 value) -> void { npcMountedWalkingSpeed = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::NPCMountedRunningSpeed()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global, default running speed for mounted NPCs
// o------------------------------------------------------------------------------------------------o
auto CServerData::NPCMountedRunningSpeed() const -> float { return npcMountedRunningSpeed; }
auto CServerData::NPCMountedRunningSpeed(R32 value) -> void { npcMountedRunningSpeed = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::NPCMountedFleeingSpeed()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global, default speed at which mounted NPCs flee in
// combat
// o------------------------------------------------------------------------------------------------o
auto CServerData::NPCMountedFleeingSpeed() const -> float { return npcMountedFleeingSpeed; }
auto CServerData::NPCMountedFleeingSpeed(R32 value) -> void { npcMountedFleeingSpeed = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::TitleColour()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default text colour for titles in gumps
// o------------------------------------------------------------------------------------------------o
auto CServerData::TitleColour() const -> std::uint16_t { return titleColour; }
auto CServerData::TitleColour(std::uint16_t value) -> void { titleColour = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::LeftTextColour()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default text colour for left text in gumps (2 column
// ones)
// o------------------------------------------------------------------------------------------------o
auto CServerData::LeftTextColour() const -> std::uint16_t { return leftTextColour; }
auto CServerData::LeftTextColour(std::uint16_t value) -> void { leftTextColour = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::RightTextColour()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default text colour for right text in gumps (2 column
// ones)
// o------------------------------------------------------------------------------------------------o
auto CServerData::RightTextColour() const -> std::uint16_t { return rightTextColour; }
auto CServerData::RightTextColour(std::uint16_t value) -> void { rightTextColour = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ButtonCancel()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default Button ID for cancel button in gumps
// o------------------------------------------------------------------------------------------------o
auto CServerData::ButtonCancel() const -> std::uint16_t { return buttonCancel; }
auto CServerData::ButtonCancel(std::uint16_t value) -> void { buttonCancel = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ButtonLeft()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default Button ID for left button (navigation) in
// gumps
// o------------------------------------------------------------------------------------------------o
auto CServerData::ButtonLeft() const -> std::uint16_t { return buttonLeft; }
auto CServerData::ButtonLeft(std::uint16_t value) -> void { buttonLeft = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ButtonRight()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default Button ID for right button (navigation) in
// gumps
// o------------------------------------------------------------------------------------------------o
auto CServerData::ButtonRight() const -> std::uint16_t { return buttonRight; }
auto CServerData::ButtonRight(std::uint16_t value) -> void { buttonRight = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::BackgroundPic()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default Gump ID for background gump
// o------------------------------------------------------------------------------------------------o
auto CServerData::BackgroundPic() const -> std::uint16_t { return backgroundPic; }
auto CServerData::BackgroundPic(std::uint16_t value) -> void { backgroundPic = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::TownNumSecsPollOpen()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the time (in seconds) for which a town voting poll is open
// o------------------------------------------------------------------------------------------------o
auto CServerData::TownNumSecsPollOpen() const -> std::uint32_t { return numSecsPollOpen; }
auto CServerData::TownNumSecsPollOpen(std::uint32_t value) -> void { numSecsPollOpen = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::TownNumSecsAsMayor()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the time (in seconds) that a PC would be a mayor
// o------------------------------------------------------------------------------------------------o
auto CServerData::TownNumSecsAsMayor() const -> std::uint32_t { return numSecsAsMayor; }
auto CServerData::TownNumSecsAsMayor(std::uint32_t value) -> void { numSecsAsMayor = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::TownTaxPeriod()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the time (in seconds) between periods of taxes for PCs
// o------------------------------------------------------------------------------------------------o
auto CServerData::TownTaxPeriod() const -> std::uint32_t { return taxPeriod; }
auto CServerData::TownTaxPeriod(std::uint32_t value) -> void { taxPeriod = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::TownGuardPayment()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the time (in seconds) between payments for guards
// o------------------------------------------------------------------------------------------------o
auto CServerData::TownGuardPayment() const -> std::uint32_t { return guardPayment; }
auto CServerData::TownGuardPayment(std::uint32_t value) -> void { guardPayment = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::RepMaxKills()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the threshold in player kills before a player turns red
//(murderer)
// o------------------------------------------------------------------------------------------------o
auto CServerData::RepMaxKills() const -> std::uint16_t { return maxMurdersAllowed; }
auto CServerData::RepMaxKills(std::uint16_t value) -> void { maxMurdersAllowed = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ResLogs()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum number of logs in a given resource area
// o------------------------------------------------------------------------------------------------o
auto CServerData::ResLogs() const -> std::int16_t { return logsPerArea; }
auto CServerData::ResLogs(std::int16_t value) -> void { logsPerArea = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ResLogTime()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the time it takes for 1 single log to respawn in a
// resource area
// o------------------------------------------------------------------------------------------------o
auto CServerData::ResLogTime() const -> std::uint16_t { return logsRespawnTimer; }
auto CServerData::ResLogTime(std::uint16_t value) -> void { logsRespawnTimer = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ResOre()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum number of ore in a given resource area
// o------------------------------------------------------------------------------------------------o
auto CServerData::ResOre() const -> std::int16_t { return orePerArea; }
auto CServerData::ResOre(std::int16_t value) -> void { orePerArea = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ResOreTime()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the time it takes for 1 single ore to respawn in a
// resource area
// o------------------------------------------------------------------------------------------------o
auto CServerData::ResOreTime() const -> std::uint16_t { return oreRespawnTimer; }
auto CServerData::ResOreTime(std::uint16_t value) -> void { oreRespawnTimer = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ResourceAreaSize()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the size of each resource area to split the world into
//(min 8x8)
// o------------------------------------------------------------------------------------------------o
auto CServerData::ResourceAreaSize() const -> std::uint16_t { return resourceAreaSize; }
auto CServerData::ResourceAreaSize(std::uint16_t value) -> void {
    if (value < 8) {
        value = 8;
    }
    resourceAreaSize = value;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ResFish()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum number of fish in a given resource area
// o------------------------------------------------------------------------------------------------o
auto CServerData::ResFish() const -> std::int16_t { return fishPerArea; }
auto CServerData::ResFish(std::int16_t value) -> void { fishPerArea = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ResFishTime()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the time it takes for 1 single fish to respawn in a
// resource area
// o------------------------------------------------------------------------------------------------o
auto CServerData::ResFishTime() const -> std::uint16_t { return fishRespawnTimer; }
auto CServerData::ResFishTime(std::uint16_t value) -> void { fishRespawnTimer = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::AccountFlushTimer()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets how often (in mins) online accounts are checked to see if
// they really ARE online
// o------------------------------------------------------------------------------------------------o
auto CServerData::AccountFlushTimer() const -> double { return flushTime; }
auto CServerData::AccountFlushTimer(R64 value) -> void { flushTime = value; }




// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::getSpawnRegionsFacetStatus()
//|					CServerData::setSpawnRegionsFacetStatus()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets active status of spawn regions per facet
// o------------------------------------------------------------------------------------------------o
auto CServerData::getSpawnRegionsFacetStatus(std::uint32_t value) const -> bool {
    return spawnRegionsFacets.test(value);
}
auto CServerData::setSpawnRegionsFacetStatus(std::uint32_t nVal, bool status) -> void {
    spawnRegionsFacets.set(nVal, status);
}
auto CServerData::getSpawnRegionsFacetStatus() const -> std::uint32_t {
    return static_cast<std::uint32_t>(spawnRegionsFacets.to_ulong());
}
auto CServerData::setSpawnRegionsFacetStatus(std::uint32_t nVal) -> void { spawnRegionsFacets = nVal; }



// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::SaveIni()
//|	Date		-	02/21/2002
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Save the uox.ini out. This is the default save
// o------------------------------------------------------------------------------------------------o
//|	Returns		- [true] If successfull
// o------------------------------------------------------------------------------------------------o
auto CServerData::SaveIni() -> bool {
    /*
    auto s = actualINI;
    if (s.empty()) {
        s = Directory(CSDDP_ROOT);
        s += "uox.ini"s;
    }
    return SaveIni(s);
     */
    return true ;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::SaveIni()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	This is the full uox.ini save routing.
//|
//|	Changes		-	02/21/2002	-	Fixed paths not being saved back out
//|
//|	Changes		-	02/21/2002	-	Added ini support for the isloation system
//|									At this point it just defaults to
//Isolation level 1.
//|
//|	Changes		-	02/27/2002	-	Made sure that ALL directory paths are
//|									written out to the ini file.
//|
//|	Changes		-	04/03/2004	-	Added new tags to the system group
//|
//|										SERVERNAME		: Name of
//the server.
//|										NETRCVTIMEOUT	: Timeout
//in seconds for recieving data |
//NETSNDTIMEOUT	: Timeout in seconds for sending data
//|										UOGENABLED		: Does
//this server respond to UOGInfo Requests?
//|
// 0-No 1-Yes
//|
//|	Changes		-	04/03/2004	-	Added new group [facet] with these tags
//|
//|										USEFACETSAVES	: Does the
//server seperate facet data?
//|
// 0-No : All data will be saved into the shared folder
//|
// 1-Yes: Each Facet will save its data into its own shared folder |
// MAP0-MAP3			:	Format= MAPx=Mapname,MapAccess
//|
//|
// Where Mapname is the name of the map and |
// MapAccess the set of bit flags that allow/deny features and map access
//|
// o------------------------------------------------------------------------------------------------o
auto CServerData::SaveIni(const std::string &filename) -> bool {
    auto rValue = false;
    auto ofsOutput = std::ofstream(filename);
    if (ofsOutput.is_open()) {
        ofsOutput << "// UOX Initialization File. V";
        ofsOutput << (static_cast<std::uint16_t>(1) << 8 | static_cast<std::uint16_t>(2)) << '\n'
        << "//================================" << '\n'
        << '\n';
        ofsOutput << "[system]" << '\n' << "{" << '\n';
        ofsOutput << "SERVERNAME=" << ServerConfig::shared().serverString[ServerString::SERVERNAME] << '\n';
        ofsOutput << "EXTERNALIP=" << ServerConfig::shared().serverString[ServerString::PUBLICIP]  << '\n';
        ofsOutput << "PORT=" << ServerPort() << '\n';
        ofsOutput << "SECRETSHARDKEY=" << ServerConfig::shared().serverString[ServerString::SHARDKEY] << '\n';
        ofsOutput << "SERVERLANGUAGE=" << ServerLanguage() << '\n';
        ofsOutput << "NETRCVTIMEOUT=" << ServerNetRcvTimeout() << '\n';
        ofsOutput << "NETSNDTIMEOUT=" << ServerNetSndTimeout() << '\n';
        ofsOutput << "NETRETRYCOUNT=" << ServerNetRetryCount() << '\n';
        ofsOutput << "CONSOLELOG=" << (ServerConfig::shared().enabled(ServerSwitch::CONSOLELOG)) << '\n';
        ofsOutput << "NETWORKLOG=" << (ServerConfig::shared().enabled(ServerSwitch::NETWORKLOG)) << '\n';
        ofsOutput << "SPEECHLOG=" << ServerConfig::shared().enabled(ServerSwitch::SPEECHLOG) << '\n';
        ofsOutput << "COMMANDPREFIX=" << ServerConfig::shared().serverString[ServerString::COMMANDPREFIX] << '\n';
        ofsOutput << "ANNOUNCEWORLDSAVES=" << ServerConfig::shared().enabled(ServerSwitch::ANNOUNCESAVE) << '\n';
        ofsOutput << "JOINPARTMSGS=" << ServerConfig::shared().enabled(ServerSwitch::ANNOUNCEJOINPART) << '\n';
        ofsOutput << "BACKUPSENABLED=" << ServerConfig::shared().enabled(ServerSwitch::BACKUP) << '\n';
        ofsOutput << "BACKUPSAVERATIO=" << BackupRatio() << '\n';
        ofsOutput << "SAVESTIMER=" << ServerSavesTimerStatus() << '\n';
        ofsOutput << "ACCOUNTISOLATION=" << "1" << '\n';
        ofsOutput << "UOGENABLED=" << static_cast<int>(ServerConfig::shared().enabled(ServerSwitch::UOG)) << '\n';
        ofsOutput << "FREESHARDSERVERPOLL=" << static_cast<int>(ServerConfig::shared().enabled(ServerSwitch::FREESHARD)) << '\n';
        ofsOutput << "RANDOMSTARTINGLOCATION=" << ServerConfig::shared().enabled(ServerSwitch::RANDOMSTART) << '\n';
        ofsOutput << "ASSISTANTNEGOTIATION=" << ServerConfig::shared().enabled(ServerSwitch::ASSISTANTNEGOTIATION) << '\n';
        ofsOutput << "KICKONASSISTANTSILENCE=" << ServerConfig::shared().enabled(ServerSwitch::KICKONASSISTANTSILENCE) << '\n';
        ofsOutput << "CLASSICUOMAPTRACKER=" << ServerConfig::shared().enabled(ServerSwitch::CUOMAPTRACKER) << '\n';
        ofsOutput << "JSENGINESIZE=" << static_cast<std::uint16_t>(GetJSEngineSize()) << '\n';
        ofsOutput << "USEUNICODEMESSAGES=" << ServerConfig::shared().enabled(ServerSwitch::UNICODEMESSAGE) << '\n';
        ofsOutput << "CONTEXTMENUS=" << ServerConfig::shared().enabled(ServerSwitch::CONTEXTMENU) << '\n';
        ofsOutput << "SYSMESSAGECOLOUR=" << SysMsgColour() << '\n';
        ofsOutput << "MAXCLIENTBYTESIN=" << static_cast<std::uint32_t>(MaxClientBytesIn()) << '\n';
        ofsOutput << "MAXCLIENTBYTESOUT=" << static_cast<std::uint32_t>(MaxClientBytesOut()) << '\n';
        ofsOutput << "NETTRAFFICTIMEBAN=" << static_cast<std::uint32_t>(NetTrafficTimeban()) << '\n';
        ofsOutput << "APSPERFTHRESHOLD=" << static_cast<std::uint16_t>(APSPerfThreshold()) << '\n';
        ofsOutput << "APSINTERVAL=" << static_cast<std::uint16_t>(APSInterval()) << '\n';
        ofsOutput << "APSDELAYSTEP=" << static_cast<std::uint16_t>(APSDelayStep()) << '\n';
        ofsOutput << "APSDELAYMAXCAP=" << static_cast<std::uint16_t>(APSDelayMaxCap()) << '\n';
        ofsOutput << "}" << '\n' << '\n';
        /*
        ofsOutput << "[clientsupport]" << '\n' << "{" << '\n';
        ofsOutput << "CLIENTSUPPORT4000=" << (clientSupport4000() ? 1 : 0) << '\n';
        ofsOutput << "CLIENTSUPPORT5000=" << (clientSupport5000() ? 1 : 0) << '\n';
        ofsOutput << "CLIENTSUPPORT6000=" << (clientSupport6000() ? 1 : 0) << '\n';
        ofsOutput << "CLIENTSUPPORT6050=" << (clientSupport6050() ? 1 : 0) << '\n';
        ofsOutput << "CLIENTSUPPORT7000=" << (clientSupport7000() ? 1 : 0) << '\n';
        ofsOutput << "CLIENTSUPPORT7090=" << (clientSupport7090() ? 1 : 0) << '\n';
        ofsOutput << "CLIENTSUPPORT70160=" << (clientSupport70160() ? 1 : 0) << '\n';
        ofsOutput << "CLIENTSUPPORT70240=" << (clientSupport70240() ? 1 : 0) << '\n';
        ofsOutput << "CLIENTSUPPORT70300=" << (clientSupport70300() ? 1 : 0) << '\n';
        ofsOutput << "CLIENTSUPPORT70331=" << (clientSupport70331() ? 1 : 0) << '\n';
        ofsOutput << "CLIENTSUPPORT704565=" << (clientSupport704565() ? 1 : 0) << '\n';
        ofsOutput << "CLIENTSUPPORT70610=" << (clientSupport70610() ? 1 : 0) << '\n';
        ofsOutput << "}" << '\n';
         */
        /*
        ofsOutput << '\n' << "[directories]" << '\n' << "{" << '\n';
        ofsOutput << "DIRECTORY=" << Directory(CSDDP_ROOT) << '\n';
        ofsOutput << "DATADIRECTORY=" << Directory(CSDDP_DATA) << '\n';
        ofsOutput << "DEFSDIRECTORY=" << Directory(CSDDP_DEFS) << '\n';
        ofsOutput << "BOOKSDIRECTORY=" << Directory(CSDDP_BOOKS) << '\n';
        ofsOutput << "ACTSDIRECTORY=" << Directory(CSDDP_ACCOUNTS) << '\n';
        ofsOutput << "SCRIPTSDIRECTORY=" << Directory(CSDDP_SCRIPTS) << '\n';
        ofsOutput << "SCRIPTDATADIRECTORY=" << Directory(CSDDP_SCRIPTDATA) << '\n';
        ofsOutput << "BACKUPDIRECTORY=" << Directory(CSDDP_BACKUP) << '\n';
        ofsOutput << "MSGBOARDDIRECTORY=" << Directory(CSDDP_MSGBOARD) << '\n';
        ofsOutput << "SHAREDDIRECTORY=" << Directory(CSDDP_SHARED) << '\n';
        ofsOutput << "ACCESSDIRECTORY=" << Directory(CSDDP_ACCESS) << '\n';
        ofsOutput << "HTMLDIRECTORY=" << Directory(CSDDP_HTML) << '\n';
        ofsOutput << "LOGSDIRECTORY=" << Directory(CSDDP_LOGS) << '\n';
        ofsOutput << "DICTIONARYDIRECTORY=" << Directory(CSDDP_DICTIONARIES) << '\n';
        ofsOutput << "}" << '\n';
         */
        
        ofsOutput << '\n' << "[skill & stats]" << '\n' << "{" << '\n';
        ofsOutput << "SKILLLEVEL=" << static_cast<std::uint16_t>(SkillLevel()) << '\n';
        ofsOutput << "SKILLCAP=" << ServerSkillTotalCapStatus() << '\n';
        ofsOutput << "SKILLDELAY=" << static_cast<std::uint16_t>(ServerSkillDelayStatus()) << '\n';
        ofsOutput << "STATCAP=" << ServerStatCapStatus() << '\n';
        ofsOutput << "STATSAFFECTSKILLCHECKS=" << ServerConfig::shared().enabled(ServerSwitch::STATIMPACTSKILL) << '\n';
        ofsOutput << "EXTENDEDSTARTINGSTATS=" << ServerConfig::shared().enabled(ServerSwitch::EXTENDEDSTATS) << '\n';
        ofsOutput << "EXTENDEDSTARTINGSKILLS=" << ServerConfig::shared().enabled(ServerSwitch::EXTENDEDSKILLS) << '\n';
        ofsOutput << "MAXSTEALTHMOVEMENTS=" << MaxStealthMovement() << '\n';
        ofsOutput << "MAXSTAMINAMOVEMENTS=" << MaxStaminaMovement() << '\n';
        ofsOutput << "SNOOPISCRIME=" << ServerConfig::shared().enabled(ServerSwitch::SNOOPISCRIME) << '\n';
        ofsOutput << "SNOOPAWARENESS=" << ServerConfig::shared().enabled(ServerSwitch::SNOOPAWARE) << '\n';
        ofsOutput << "ARMORAFFECTMANAREGEN=" << ServerConfig::shared().enabled(ServerSwitch::ARMORIMPACTSMANA) << '\n';
        ofsOutput << "}" << '\n';
        
        ofsOutput << '\n' << "[timers]" << '\n' << "{" << '\n';
        ofsOutput << "CORPSEDECAYTIMER=" << SystemTimer(tSERVER_CORPSEDECAY) << '\n';
        ofsOutput << "NPCCORPSEDECAYTIMER=" << SystemTimer(tSERVER_NPCCORPSEDECAY) << '\n';
        ofsOutput << "WEATHERTIMER=" << SystemTimer(tSERVER_WEATHER) << '\n';
        ofsOutput << "SHOPSPAWNTIMER=" << SystemTimer(tSERVER_SHOPSPAWN) << '\n';
        ofsOutput << "DECAYTIMER=" << SystemTimer(tSERVER_DECAY) << '\n';
        ofsOutput << "DECAYTIMERINHOUSE=" << SystemTimer(tSERVER_DECAYINHOUSE) << '\n';
        ofsOutput << "INVISIBILITYTIMER=" << SystemTimer(tSERVER_INVISIBILITY) << '\n';
        ofsOutput << "OBJECTUSETIMER=" << SystemTimer(tSERVER_OBJECTUSAGE) << '\n';
        ofsOutput << "GATETIMER=" << SystemTimer(tSERVER_GATE) << '\n';
        ofsOutput << "POISONTIMER=" << SystemTimer(tSERVER_POISON) << '\n';
        ofsOutput << "LOGINTIMEOUT=" << SystemTimer(tSERVER_LOGINTIMEOUT) << '\n';
        ofsOutput << "HITPOINTREGENTIMER=" << SystemTimer(tSERVER_HITPOINTREGEN) << '\n';
        ofsOutput << "STAMINAREGENTIMER=" << SystemTimer(tSERVER_STAMINAREGEN) << '\n';
        ofsOutput << "MANAREGENTIMER=" << SystemTimer(tSERVER_MANAREGEN) << '\n';
        ofsOutput << "BASEFISHINGTIMER=" << SystemTimer(tSERVER_FISHINGBASE) << '\n';
        ofsOutput << "RANDOMFISHINGTIMER=" << SystemTimer(tSERVER_FISHINGRANDOM) << '\n';
        ofsOutput << "SPIRITSPEAKTIMER=" << SystemTimer(tSERVER_SPIRITSPEAK) << '\n';
        ofsOutput << "PETOFFLINECHECKTIMER=" << SystemTimer(tSERVER_PETOFFLINECHECK) << '\n';
        ofsOutput << "NPCFLAGUPDATETIMER=" << SystemTimer(tSERVER_NPCFLAGUPDATETIMER) << '\n';
        ofsOutput << "BLOODDECAYTIMER=" << SystemTimer(tSERVER_BLOODDECAY) << '\n';
        ofsOutput << "BLOODDECAYCORPSETIMER=" << SystemTimer(tSERVER_BLOODDECAYCORPSE) << '\n';
        ofsOutput << "}" << '\n';
        
         
        ofsOutput << '\n' << "[settings]" << '\n' << "{" << '\n';
        ofsOutput << "LOOTDECAYSWITHCORPSE=" << ServerConfig::shared().enabled(ServerSwitch::CORPSELOOTDECAY) << '\n';
        ofsOutput << "GUARDSACTIVE=" << ServerConfig::shared().enabled(ServerSwitch::GUARDSACTIVE) << '\n';
        ofsOutput << "DEATHANIMATION=" << ServerConfig::shared().enabled(ServerSwitch::DEATHANIMATION) << '\n';
        ofsOutput << "AMBIENTSOUNDS=" << WorldAmbientSounds() << '\n';
        ofsOutput << "AMBIENTFOOTSTEPS=" << ServerConfig::shared().enabled(ServerSwitch::AMBIENTFOOTSTEPS) << '\n';
        ofsOutput << "INTERNALACCOUNTCREATION=" << ServerConfig::shared().enabled(ServerSwitch::AUTOACCOUNT) << '\n';
        ofsOutput << "SHOWOFFLINEPCS=" << ServerConfig::shared().enabled(ServerSwitch::SHOWOFFLINEPCS) << '\n';
        ofsOutput << "ROGUESENABLED=" << ServerConfig::shared().enabled(ServerSwitch::ROGUEENABLE) << '\n';
        ofsOutput << "PLAYERPERSECUTION=" << ServerConfig::shared().enabled(ServerSwitch::PLAYERPERSECUTION) << '\n';
        ofsOutput << "ACCOUNTFLUSH=" << AccountFlushTimer() << '\n';
        ofsOutput << "SELLBYNAME=" << ServerConfig::shared().enabled(ServerSwitch::SELLBYNAME) << '\n';
        ofsOutput << "SELLMAXITEMS=" << SellMaxItemsStatus() << '\n';
        ofsOutput << "GLOBALRESTOCKMULTIPLIER=" << GlobalRestockMultiplier() << '\n';
        ofsOutput << "BANKBUYTHRESHOLD=" << BuyThreshold() << '\n';
        ofsOutput << "TRADESYSTEM=" << ServerConfig::shared().enabled(ServerSwitch::TRADESYSTEM) << '\n';
        ofsOutput << "RANKSYSTEM=" << ServerConfig::shared().enabled(ServerSwitch::RANKSYSTEM) << '\n';
        ofsOutput << "DISPLAYMAKERSMARK=" << ServerConfig::shared().enabled(ServerSwitch::MAKERMARK) << '\n';
        ofsOutput << "CUTSCROLLREQUIREMENTS=" << ServerConfig::shared().enabled(ServerSwitch::CUTSCROLLREQ) << '\n';
        ofsOutput << "NPCTRAININGENABLED=" << ServerConfig::shared().enabled(ServerSwitch::NPCTRAINING) << '\n';
        ofsOutput << "HIDEWHILEMOUNTED=" << ServerConfig::shared().enabled(ServerSwitch::HIDEWHILEMOUNTED) << '\n';
        ofsOutput << "WEIGHTPERSTR=" << static_cast<R32>(WeightPerStr()) << '\n';
        ofsOutput << "POLYDURATION=" << SystemTimer(tSERVER_POLYMORPH) << '\n';
        ofsOutput << "CLIENTFEATURES=" << ServerConfig::shared().clientFeature.value() << '\n';
        ofsOutput << "SERVERFEATURES=" << ServerConfig::shared().serverFeature.value()  << '\n';
        ofsOutput << "SPAWNREGIONSFACETS=" << getSpawnRegionsFacetStatus() << '\n';
        ofsOutput << "OVERLOADPACKETS=" << ServerConfig::shared().enabled(ServerSwitch::OVERLOADPACKETS) << '\n';
        ofsOutput << "ADVANCEDPATHFINDING=" << ServerConfig::shared().enabled(ServerSwitch::ADVANCEDPATHFINDING) << '\n';
        ofsOutput << "LOOTINGISCRIME=" << ServerConfig::shared().enabled(ServerSwitch::LOOTINGISCRIME) << '\n';
        ofsOutput << "BASICTOOLTIPSONLY=" << ServerConfig::shared().enabled(ServerSwitch::BASICTOOLTIPSONLY) << '\n';
        ofsOutput << "SHOWREPUTATIONTITLEINTOOLTIP=" << ServerConfig::shared().enabled(ServerSwitch::DISPLAYREPUTATIONTOOLTIP) << '\n';
        ofsOutput << "SHOWGUILDINFOINTOOLTIP=" << ServerConfig::shared().enabled(ServerSwitch::DISPLAYGUILDTOOLTIP) << '\n';
        ofsOutput << "SHOWNPCTITLESINTOOLTIPS=" << ServerConfig::shared().enabled(ServerSwitch::NPCTOOLTIPS) << '\n';
        ofsOutput << "SHOWNPCTITLESOVERHEAD=" << ServerConfig::shared().enabled(ServerSwitch::OVERHEADTITLE) << '\n';
        ofsOutput << "SHOWINVULNERABLETAGOVERHEAD=" << ServerConfig::shared().enabled(ServerSwitch::DISPLAYINVUNERABLE) << '\n';
        ofsOutput << "SHOWRACEWITHNAME=" << ServerConfig::shared().enabled(ServerSwitch::DISPLAYRACE) << '\n';
        ofsOutput << "SHOWRACEINPAPERDOLL=" << ServerConfig::shared().enabled(ServerSwitch::DISPLAYRACEPAPERDOLL) << '\n';
        ofsOutput << "GLOBALITEMDECAY=" << ServerConfig::shared().enabled(ServerSwitch::ITEMDECAY) << '\n';
        ofsOutput << "SCRIPTITEMSDECAYABLE=" << ServerConfig::shared().enabled(ServerSwitch::SCRIPTITEMSDECAYABLE) << '\n';
        ofsOutput << "BASEITEMSDECAYABLE=" << ServerConfig::shared().enabled(ServerSwitch::BASEITEMSDECAYABLE) << '\n';
        ofsOutput << "PAPERDOLLGUILDBUTTON=" << ServerConfig::shared().enabled(ServerSwitch::DISPLAYGUILDBUTTON) << '\n';
        ofsOutput << "FISHINGSTAMINALOSS=" << FishingStaminaLoss() << '\n';
        ofsOutput << "ITEMSDETECTSPEECH=" << ServerConfig::shared().enabled(ServerSwitch::ITEMDETECTSPEECH) << '\n';
        ofsOutput << "MAXPLAYERPACKITEMS=" << MaxPlayerPackItems() << '\n';
        ofsOutput << "MAXPLAYERBANKITEMS=" << MaxPlayerBankItems() << '\n';
        ofsOutput << "MAXPLAYERPACKWEIGHT=" << MaxPlayerPackWeight() << '\n';
        ofsOutput << "MAXPLAYERBANKWEIGHT=" << MaxPlayerBankWeight() << '\n';
        ofsOutput << "FORCENEWANIMATIONPACKET=" << ServerConfig::shared().enabled(ServerSwitch::FORECENEWANIMATIONPACKET) << '\n';
        ofsOutput << "MAPDIFFSENABLED=" << ServerConfig::shared().enabled(ServerSwitch::MAPDIFF) << '\n';
        ofsOutput << "TOOLUSELIMIT=" << ServerConfig::shared().enabled(ServerSwitch::TOOLUSE) << '\n';
        ofsOutput << "TOOLUSEBREAK=" << ServerConfig::shared().enabled(ServerSwitch::TOOLBREAK) << '\n';
        ofsOutput << "ITEMREPAIRDURABILITYLOSS=" << ServerConfig::shared().enabled(ServerSwitch::REPAIRLOSS) << '\n';
        ofsOutput << "CRAFTCOLOUREDWEAPONS=" << ServerConfig::shared().enabled(ServerSwitch::COLORWEAPON) << '\n';
        ofsOutput << "MAXSAFETELEPORTSPERDAY=" << static_cast<std::uint16_t>(MaxSafeTeleportsPerDay())
        << '\n';
        ofsOutput << "TELEPORTTONEARESTSAFELOCATION=" << ServerConfig::shared().enabled(ServerSwitch::SAFETELEPORT)
        << '\n';
        ofsOutput << "ALLOWAWAKENPCS=" << ServerConfig::shared().enabled(ServerSwitch::AWAKENPC) << '\n';
        ofsOutput << "ENABLENPCGUILDDISCOUNTS=" << ServerConfig::shared().enabled(ServerSwitch::GUILDDISCOUNT) << '\n';
        ofsOutput << "ENABLENPCGUILDPREMIUMS=" << ServerConfig::shared().enabled(ServerSwitch::GUILDPREMIUM) << '\n';
        ofsOutput << "YOUNGPLAYERSYSTEM=" << ServerConfig::shared().enabled(ServerSwitch::YOUNGPLAYER) << '\n';
        ofsOutput << "}" << '\n';
        
        ofsOutput << '\n' << "[pets and followers]" << '\n' << "{" << '\n';
        ofsOutput << "MAXCONTROLSLOTS=" << static_cast<std::uint16_t>(MaxControlSlots()) << '\n';
        ofsOutput << "MAXFOLLOWERS=" << static_cast<std::uint16_t>(MaxFollowers()) << '\n';
        ofsOutput << "MAXPETOWNERS=" << static_cast<std::uint16_t>(MaxPetOwners()) << '\n';
        ofsOutput << "CHECKPETCONTROLDIFFICULTY=" << ServerConfig::shared().enabled(ServerSwitch::PETDIFFICULTY) << '\n';
        ofsOutput << "PETLOYALTYGAINONSUCCESS=" << static_cast<std::uint16_t>(GetPetLoyaltyGainOnSuccess())
        << '\n';
        ofsOutput << "PETLOYALTYLOSSONFAILURE=" << static_cast<std::uint16_t>(GetPetLoyaltyLossOnFailure())
        << '\n';
        ofsOutput << "PETLOYALTYRATE=" << SystemTimer(tSERVER_LOYALTYRATE) << '\n';
        ofsOutput << "}" << '\n';
        
        ofsOutput << '\n' << "[speedup]" << '\n' << "{" << '\n';
        ofsOutput << "CHECKITEMS=" << CheckItemsSpeed() << '\n';
        ofsOutput << "CHECKBOATS=" << CheckBoatSpeed() << '\n';
        ofsOutput << "CHECKNPCAI=" << CheckNpcAISpeed() << '\n';
        ofsOutput << "CHECKSPAWNREGIONS=" << CheckSpawnRegionSpeed() << '\n';
        ofsOutput << "NPCMOVEMENTSPEED=" << NPCWalkingSpeed() << '\n';
        ofsOutput << "NPCRUNNINGSPEED=" << NPCRunningSpeed() << '\n';
        ofsOutput << "NPCFLEEINGSPEED=" << NPCFleeingSpeed() << '\n';
        ofsOutput << "NPCMOUNTEDWALKINGSPEED=" << NPCMountedWalkingSpeed() << '\n';
        ofsOutput << "NPCMOUNTEDRUNNINGSPEED=" << NPCMountedRunningSpeed() << '\n';
        ofsOutput << "NPCMOUNTEDFLEEINGSPEED=" << NPCMountedFleeingSpeed() << '\n';
        ofsOutput << "NPCSPELLCASTSPEED=" << NPCSpellCastSpeed() << '\n';
        ofsOutput << "GLOBALATTACKSPEED=" << GlobalAttackSpeed() << '\n';
        ofsOutput << "}" << '\n';
        
        ofsOutput << '\n' << "[message boards]" << '\n' << "{" << '\n';
        ofsOutput << "POSTINGLEVEL=" << static_cast<std::uint16_t>(MsgBoardPostingLevel()) << '\n';
        ofsOutput << "REMOVALLEVEL=" << static_cast<std::uint16_t>(MsgBoardPostRemovalLevel()) << '\n';
        ofsOutput << "}" << '\n';
        
        ofsOutput << '\n' << "[escorts]" << '\n' << "{" << '\n';
        ofsOutput << "ESCORTENABLED=" << ServerConfig::shared().enabled(ServerSwitch::ESCORTS) << '\n';
        ofsOutput << "ESCORTINITEXPIRE=" << SystemTimer(tSERVER_ESCORTWAIT) << '\n';
        ofsOutput << "ESCORTACTIVEEXPIRE=" << SystemTimer(tSERVER_ESCORTACTIVE) << '\n';
        ofsOutput << "ESCORTDONEEXPIRE=" << SystemTimer(tSERVER_ESCORTDONE) << '\n';
        ofsOutput << "}" << '\n';
        
        ofsOutput << '\n' << "[worldlight]" << '\n' << "{" << '\n';
        ofsOutput << "DUNGEONLEVEL=" << static_cast<std::uint16_t>(dungeonLightLevel()) << '\n';
        ofsOutput << "BRIGHTLEVEL=" << static_cast<std::uint16_t>(worldLightBrightLevel()) << '\n';
        ofsOutput << "DARKLEVEL=" << static_cast<std::uint16_t>(worldLightDarkLevel()) << '\n';
        ofsOutput << "SECONDSPERUOMINUTE=" << ServerSecondsPerUOMinute() << '\n';
        ofsOutput << "}" << '\n';
        
        ofsOutput << '\n' << "[tracking]" << '\n' << "{" << '\n';
        ofsOutput << "BASERANGE=" << TrackingBaseRange() << '\n';
        ofsOutput << "BASETIMER=" << TrackingBaseTimer() << '\n';
        ofsOutput << "MAXTARGETS=" << static_cast<std::uint16_t>(TrackingMaxTargets()) << '\n';
        ofsOutput << "MSGREDISPLAYTIME=" << TrackingRedisplayTime() << '\n';
        ofsOutput << "}" << '\n';
        
        ofsOutput << '\n' << "[reputation]" << '\n' << "{" << '\n';
        ofsOutput << "MURDERDECAYTIMER=" << SystemTimer(tSERVER_MURDERDECAY) << '\n';
        ofsOutput << "MAXKILLS=" << RepMaxKills() << '\n';
        ofsOutput << "CRIMINALTIMER=" << SystemTimer(tSERVER_CRIMINAL) << '\n';
        ofsOutput << "AGGRESSORFLAGTIMER=" << SystemTimer(tSERVER_AGGRESSORFLAG) << '\n';
        ofsOutput << "PERMAGREYFLAGTIMER=" << SystemTimer(tSERVER_PERMAGREYFLAG) << '\n';
        ofsOutput << "STEALINGFLAGTIMER=" << SystemTimer(tSERVER_STEALINGFLAG) << '\n';
        ofsOutput << "}" << '\n';
        
        ofsOutput << '\n' << "[resources]" << '\n' << "{" << '\n';
        ofsOutput << "RESOURCEAREASIZE=" << ResourceAreaSize() << '\n';
        ofsOutput << "MINECHECK=" << static_cast<std::uint16_t>(MineCheck()) << '\n';
        ofsOutput << "OREPERAREA=" << ResOre() << '\n';
        ofsOutput << "ORERESPAWNTIMER=" << ResOreTime() << '\n';
        ofsOutput << "LOGSPERAREA=" << ResLogs() << '\n';
        ofsOutput << "LOGSRESPAWNTIMER=" << ResLogTime() << '\n';
        ofsOutput << "FISHPERAREA=" << ResFish() << '\n';
        ofsOutput << "FISHRESPAWNTIMER=" << ResFishTime() << '\n';
        ofsOutput << "}" << '\n';
        
        ofsOutput << '\n' << "[hunger]" << '\n' << "{" << '\n';
        ofsOutput << "HUNGERENABLED=" << ServerConfig::shared().enabled(ServerSwitch::HUNGER) << '\n';
        ofsOutput << "HUNGERRATE=" << SystemTimer(tSERVER_HUNGERRATE) << '\n';
        ofsOutput << "HUNGERDMGVAL=" << HungerDamage() << '\n';
        ofsOutput << "PETHUNGEROFFLINE=" << ServerConfig::shared().enabled(ServerSwitch::PETHUNGEROFFLINE) << '\n';
        ofsOutput << "PETOFFLINETIMEOUT=" << PetOfflineTimeout() << '\n';
        ofsOutput << "}" << '\n';
        
        ofsOutput << '\n' << "[thirst]" << '\n' << "{" << '\n';
        ofsOutput << "THIRSTENABLED=" << ServerConfig::shared().enabled(ServerSwitch::THIRST) << '\n';
        ofsOutput << "THIRSTRATE=" << SystemTimer(tSERVER_THIRSTRATE) << '\n';
        ofsOutput << "THIRSTDRAINVAL=" << ThirstDrain() << '\n';
        ofsOutput << "PETTHIRSTOFFLINE=" << ServerConfig::shared().enabled(ServerSwitch::PETTHIRSTOFFLINE) << '\n';
        ofsOutput << "}" << '\n';
        
        ofsOutput << '\n' << "[combat]" << '\n' << "{" << '\n';
        ofsOutput << "MAXRANGE=" << CombatMaxRange() << '\n';
        ofsOutput << "SPELLMAXRANGE=" << CombatMaxSpellRange() << '\n';
        ofsOutput << "DISPLAYHITMSG=" << ServerConfig::shared().enabled(ServerSwitch::DISPLAYHITMSG) << '\n';
        ofsOutput << "DISPLAYDAMAGENUMBERS=" << ServerConfig::shared().enabled(ServerSwitch::DISPLAYDAMAGENUMBERS) << '\n';
        ofsOutput << "MONSTERSVSANIMALS=" << ServerConfig::shared().enabled(ServerSwitch::MONSTERSVSANIMALS) << '\n';
        ofsOutput << "ANIMALATTACKCHANCE=" << static_cast<std::uint16_t>(CombatAnimalsAttackChance())
        << '\n';
        ofsOutput << "ANIMALSGUARDED=" << ServerConfig::shared().enabled(ServerSwitch::ANIMALSGUARDED) << '\n';
        ofsOutput << "NPCDAMAGERATE=" << CombatNpcDamageRate() << '\n';
        ofsOutput << "NPCBASEFLEEAT=" << CombatNPCBaseFleeAt() << '\n';
        ofsOutput << "NPCBASEREATTACKAT=" << CombatNPCBaseReattackAt() << '\n';
        ofsOutput << "ATTACKSTAMINA=" << CombatAttackStamina() << '\n';
        ofsOutput << "ATTACKSPEEDFROMSTAMINA=" << ServerConfig::shared().enabled(ServerSwitch::ATTACKSPEEDFROMSTAMINA) << '\n';
        ofsOutput << "ARCHERYHITBONUS=" << static_cast<std::int16_t>(CombatArcheryHitBonus()) << '\n';
        ofsOutput << "ARCHERYSHOOTDELAY=" << CombatArcheryShootDelay() << '\n';
        ofsOutput << "SHOOTONANIMALBACK=" << ServerConfig::shared().enabled(ServerSwitch::SHOOTONANIMALBACK) << '\n';
        ofsOutput << "WEAPONDAMAGECHANCE=" << static_cast<std::uint16_t>(CombatWeaponDamageChance()) << '\n';
        ofsOutput << "WEAPONDAMAGEMIN=" << static_cast<std::uint16_t>(CombatWeaponDamageMin()) << '\n';
        ofsOutput << "WEAPONDAMAGEMAX=" << static_cast<std::uint16_t>(CombatWeaponDamageMax()) << '\n';
        ofsOutput << "ARMORDAMAGECHANCE=" << static_cast<std::uint16_t>(CombatArmorDamageChance()) << '\n';
        ofsOutput << "ARMORDAMAGEMIN=" << static_cast<std::uint16_t>(CombatArmorDamageMin()) << '\n';
        ofsOutput << "ARMORDAMAGEMAX=" << static_cast<std::uint16_t>(CombatArmorDamageMax()) << '\n';
        ofsOutput << "PARRYDAMAGECHANCE=" << static_cast<std::uint16_t>(CombatParryDamageChance()) << '\n';
        ofsOutput << "PARRYDAMAGEMIN=" << static_cast<std::uint16_t>(CombatParryDamageMin()) << '\n';
        ofsOutput << "PARRYDAMAGEMAX=" << static_cast<std::uint16_t>(CombatParryDamageMax()) << '\n';
        ofsOutput << "ARMORCLASSDAMAGEBONUS=" << ServerConfig::shared().enabled(ServerSwitch::ARMORCLASSBONUS) << '\n';
        ofsOutput << "ALCHEMYBONUSENABLED=" << ServerConfig::shared().enabled(ServerSwitch::ALCHEMYBONUS) << '\n';
        ofsOutput << "ALCHEMYBONUSMODIFIER=" << static_cast<std::uint16_t>(AlchemyDamageBonusModifier())
        << '\n';
        ofsOutput << "BLOODEFFECTCHANCE=" << static_cast<std::uint16_t>(CombatBloodEffectChance()) << '\n';
        ofsOutput << "ITEMSINTERRUPTCASTING=" << ServerConfig::shared().enabled(ServerSwitch::INTERRUPTCASTING) << '\n';
        ofsOutput << "PETCOMBATTRAINING=" << ServerConfig::shared().enabled(ServerSwitch::PETCOMBATTRAINING) << '\n';
        ofsOutput << "HIRELINGCOMBATTRAINING=" << ServerConfig::shared().enabled(ServerSwitch::HIRELINGTRAINING) << '\n';
        ofsOutput << "NPCCOMBATTRAINING=" << ServerConfig::shared().enabled(ServerSwitch::NPCCOMBAT) << '\n';
        ofsOutput << "SHOWITEMRESISTSTATS=" << ServerConfig::shared().enabled(ServerSwitch::DISPLAYRESISTSTATS) << '\n';
        ofsOutput << "SHOWWEAPONDAMAGETYPES=" << ServerConfig::shared().enabled(ServerSwitch::DISPLAYDAMAGETYPE) << '\n';
        ofsOutput << "WEAPONDAMAGEBONUSTYPE=" << static_cast<std::uint16_t>(WeaponDamageBonusType()) << '\n';
        
        ofsOutput << "}" << '\n';
        
        ofsOutput << '\n' << "[magic]" << '\n' << "{" << '\n';
        ofsOutput << "TRAVELSPELLSFROMBOATKEYS=" << ServerConfig::shared().enabled(ServerSwitch::TRAVELBOATKEY) << '\n';
        ofsOutput << "TRAVELSPELLSWHILEOVERWEIGHT=" << ServerConfig::shared().enabled(ServerSwitch::TRAVELBURDEN)
        << '\n';
        ofsOutput << "MARKRUNESINMULTIS=" << ServerConfig::shared().enabled(ServerSwitch::RUNEMULTI) << '\n';
        ofsOutput << "TRAVELSPELLSBETWEENWORLDS=" << ServerConfig::shared().enabled(ServerSwitch::SPELLWORLDTRAVEL) << '\n';
        ofsOutput << "TRAVELSPELLSWHILEAGGRESSOR=" << ServerConfig::shared().enabled(ServerSwitch::TRAVELAGRESSOR)
        << '\n';
        ofsOutput << "HIDESTATSFORUNKNOWNMAGICITEMS=" << ServerConfig::shared().enabled(ServerSwitch::MAGICSTATS) << '\n';
        ofsOutput << "CASTSPELLSWHILEMOVING=" << ServerConfig::shared().enabled(ServerSwitch::SPELLMOVING) << '\n';
        ofsOutput << "}" << '\n';
        
        ofsOutput << '\n' << "[start locations]" << '\n' << "{" << '\n';
        ofsOutput<< ServerConfig::shared().startLocation.describe();
        ofsOutput << "}" << '\n';
        
        ofsOutput << '\n' << "[young start locations]" << '\n' << "{" << '\n';
        ofsOutput<< ServerConfig::shared().youngLocation.describe();

        ofsOutput << "}" << '\n';
        
        ofsOutput << '\n' << "[startup]" << '\n' << "{" << '\n';
        ofsOutput << "STARTGOLD=" << ServerStartGold() << '\n';
        ofsOutput << "STARTPRIVS=" << serverStartPrivs() << '\n';
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
        
        ofsOutput << '\n' << "[houses]" << '\n' << "{" << '\n';
        ofsOutput << "TRACKHOUSESPERACCOUNT=" << ServerConfig::shared().enabled(ServerSwitch::TRACKHOUSESPERACCOUNT) << '\n';
        ofsOutput << "CANOWNANDCOOWNHOUSES=" << ServerConfig::shared().enabled(ServerSwitch::OWNCOOWNHOUSE)<< '\n';
        ofsOutput << "COOWNHOUSESONSAMEACCOUNT=" << ServerConfig::shared().enabled(ServerSwitch::COWNHOUSEACCOUNT) << '\n';
        ofsOutput << "ITEMDECAYINHOUSES=" << ServerConfig::shared().enabled(ServerSwitch::INHOUSEDECAY) << '\n';
        ofsOutput << "PROTECTPRIVATEHOUSES=" << ServerConfig::shared().enabled(ServerSwitch::PROTECTPRIVATEHOUSES) << '\n';
        ofsOutput << "MAXHOUSESOWNABLE=" << MaxHousesOwnable() << '\n';
        ofsOutput << "MAXHOUSESCOOWNABLE=" << MaxHousesCoOwnable() << '\n';
        ofsOutput << "SAFECOOWNERLOGOUT=" << ServerConfig::shared().enabled(ServerSwitch::COOWNERLOGOUT) << '\n';
        ofsOutput << "SAFEFRIENDLOGOUT=" << ServerConfig::shared().enabled(ServerSwitch::FRIENDLOGOUT) << '\n';
        ofsOutput << "SAFEGUESTLOGOUT=" << ServerConfig::shared().enabled(ServerSwitch::GUESTLOGOUT) << '\n';
        ofsOutput << "KEYLESSOWNERACCESS=" << ServerConfig::shared().enabled(ServerSwitch::KEYLESSOWNER) << '\n';
        ofsOutput << "KEYLESSCOOWNERACCESS=" << ServerConfig::shared().enabled(ServerSwitch::KEYLESSCOOWNER) << '\n';
        ofsOutput << "KEYLESSFRIENDACCESS=" << ServerConfig::shared().enabled(ServerSwitch::KEYLESSFRIEND) << '\n';
        ofsOutput << "KEYLESSGUESTACCESS=" << ServerConfig::shared().enabled(ServerSwitch::KEYLESSGUEST) << '\n';
        ofsOutput << "}" << '\n';
        
        ofsOutput << '\n' << "[bulk order deeds]" << '\n' << "{" << '\n';
        ofsOutput << "OFFERBODSFROMITEMSALES=" << ServerConfig::shared().enabled(ServerSwitch::OFFERBODSFROMITEMSALES) << '\n';
        ofsOutput << "OFFERBODSFROMCONTEXTMENU=" << ServerConfig::shared().enabled(ServerSwitch::OFFERBODSFROMCONTEXTMENU) << '\n';
        ofsOutput << "BODSFROMCRAFTEDITEMSONLY=" << ServerConfig::shared().enabled(ServerSwitch::BODSFROMCRAFTEDITEMSONLY) << '\n';
        ofsOutput << "BODGOLDREWARDMULTIPLIER=" << BODGoldRewardMultiplier() << '\n';
        ofsOutput << "BODFAMEREWARDMULTIPLIER=" << BODFameRewardMultiplier() << '\n';
        ofsOutput << "}" << '\n';
        
        ofsOutput << '\n' << "[towns]" << '\n' << "{" << '\n';
        ofsOutput << "POLLTIME=" << TownNumSecsPollOpen() << '\n';
        ofsOutput << "MAYORTIME=" << TownNumSecsAsMayor() << '\n';
        ofsOutput << "TAXPERIOD=" << TownTaxPeriod() << '\n';
        ofsOutput << "GUARDSPAID=" << TownGuardPayment() << '\n';
        ofsOutput << "}" << '\n';
        
        ofsOutput << '\n' << "[disabled assistant features]" << '\n' << "{" << '\n';
        ofsOutput << ServerConfig::shared().assistantFeature.describe() ;
        ofsOutput << "}" << '\n';
        
        ofsOutput.close();
        rValue = true;
    }
    else {
        Console::shared().error(util::format("Unable to open file %s for writing", filename.c_str()));
    }
    return rValue;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::load()
//|	Date		-	January 13, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Load up the uox.ini file and parse it into the internals
//|	Returns		-	pointer to the valid inmemory serverdata storage(this)
//|						nullptr is there is an error, or invalid file type
// o------------------------------------------------------------------------------------------------o
auto CServerData::load(const std::string &filename) -> bool {
    /*
    auto iniFile = filename;
    if (iniFile.empty()) {
        iniFile = Directory(CSDDP_ROOT) + "uox.ini"s;
    }
    actualINI = iniFile;
    auto rValue = ParseIni(iniFile);
     */
    postLoadDefaults();
    return true;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::TrackingBaseRange()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the base (minimum) range even beginner trackers can track
// at
// o------------------------------------------------------------------------------------------------o
auto CServerData::TrackingBaseRange() const -> std::uint16_t { return trackingBaseRange; }
auto CServerData::TrackingBaseRange(std::uint16_t value) -> void { trackingBaseRange = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::TrackingMaxTargets()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max amount of targets someone can see in the tracking
// window
// o------------------------------------------------------------------------------------------------o
auto CServerData::TrackingMaxTargets() const -> std::uint8_t { return trackingMaxTargets; }
auto CServerData::TrackingMaxTargets(std::uint8_t value) -> void {
    if (value >= MAX_TRACKINGTARGETS) {
        trackingMaxTargets = MAX_TRACKINGTARGETS;
    }
    else {
        trackingMaxTargets = value;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::TrackingBaseTimer()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max amount of time a grandmaster tracker can track
// someone
// o------------------------------------------------------------------------------------------------o
auto CServerData::TrackingBaseTimer() const -> std::uint16_t { return trackingBaseTimer; }
auto CServerData::TrackingBaseTimer(std::uint16_t value) -> void { trackingBaseTimer = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::TrackingRedisplayTime()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets how often in seconds the tracking message is updated and
// displayed
// o------------------------------------------------------------------------------------------------o
auto CServerData::TrackingRedisplayTime() const -> std::uint16_t { return trackingMsgRedisplayTimer; }
auto CServerData::TrackingRedisplayTime(std::uint16_t value) -> void { trackingMsgRedisplayTimer = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ParseIni()
//|	Date		-	02/26/2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Parse the uox.ini file into its required information.
//|
//|	Changes		-	02/26/2002	-	Make sure that we parse out the logs, access
//|									and other directories that we were
//not parsing/
// o------------------------------------------------------------------------------------------------o
auto CServerData::ParseIni(const std::string &filename) -> bool {
    auto rValue = false;
    auto input = std::ifstream(filename);
    enum search_t { header, startsection, endsection };
    auto retrieveContents = [](const std::string &input) -> std::optional<std::string> {
        auto pos1 = input.find("[");
        if (pos1 != std::string::npos) {
            auto pos2 = input.find("]", pos1);
            if (pos2 != std::string::npos) {
                // We have the bounds
                auto contents = util::trim(input.substr(pos1 + 1, (pos2 - pos1) - 1));
                return contents;
            }
        }
        return {};
    };
    auto state = search_t(header);
    if (input.is_open()) {
        char input_buffer[4096];
        while (input.good() && !input.eof()) {
            input.getline(input_buffer, 4095);
            if (input.gcount() > 0) {
                input_buffer[input.gcount()] = 0;
                auto line = util::trim(util::strip(std::string(input_buffer), "//"));
                if (!line.empty()) {
                    switch (static_cast<int>(state)) {
                        case static_cast<int>(search_t::header): {
                            if (line[0] == '[') {
                                auto contents = retrieveContents(line);
                                if (contents.has_value()) {
                                    state = search_t::startsection;
                                }
                            }
                        }
                            [[fallthrough]];
                        case static_cast<int>(search_t::startsection): {
                            if (line[0] == '{') {
                                state = search_t::endsection;
                            }
                        }
                            [[fallthrough]];
                        case static_cast<int>(search_t::endsection): {
                            if (line[0] != '}') {
                                auto [key, value] = util::split(line, "=");
                                try {
                                    if (HandleLine(key, value)) {
                                        rValue = true;
                                    }
                                } catch (const std::exception &e) {
                                    Console::shared().error("Error parsing ini file");
                                    Console::shared().error(util::format("Entry was: %s = %s", key.c_str(), value.c_str()));
                                    Console::shared().error(util::format("Exception was: %s", e.what()));
                                    exit(1);
                                }
                            }
                            else {
                                state = search_t::header;
                            }
                        }
                    }
                }
            }
        }
    }
    return rValue;
    /*
     bool rValue = false;
     if( !filename.empty() ) {
     
     Console::shared() << "Processing INI Settings  ";
     
     Script toParse( filename, NUM_DEFS, false );
     // Lock this file tight, No access at anytime when open(should only be open and closed anyhow.
     For Thread blocking) if( !toParse.IsErrored() ) {
     //serverList.clear();
     startlocations.clear();
     for (auto &[id,section] : toParse.collection() ){
     if( section){
     for (auto &sec : section->collection() ){
     auto tag = sec->tag;
     auto data = util::simplify( sec->data );
     if( !HandleLine( tag, data )) {
     Console::shared().warning( util::format( "Unhandled tag '%s'", tag.c_str() ));
     }
     }
     }
     }
     Console::shared().printDone();
     rValue = true;
     }
     else
     {
     Console::shared().warning( util::format( "%s File not found, Using default settings.",
     filename.c_str() )); cwmWorldState->ServerData()->save();
     }
     }
     return rValue;
     */
}

//==================================================================================================
auto CServerData::HandleLine(const std::string &tag, const std::string &value) -> bool {
    bool rValue = true;
    auto titer = uox3IniCaseValue.find(tag);
    if (titer == uox3IniCaseValue.end()) {
        return false;
    }
    
    switch (titer->second) {
        case 6: // SAVESTIMER
            ServerSavesTimer(static_cast<std::uint32_t>(std::stoul(value, nullptr, 0)));
            break;
        case 7: // SKILLCAP
            ServerSkillTotalCap(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 8: // SKILLDELAY
            ServerSkillDelay(static_cast<std::uint8_t>(std::stoul(value, nullptr, 0)));
            break;
        case 9: // STATCAP
            ServerStatCap(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 10: // MAXSTEALTHMOVEMENTS
            MaxStealthMovement(static_cast<std::int16_t>(std::stoi(value, nullptr, 0)));
            break;
        case 11: // MAXSTAMINAMOVEMENTS
            MaxStaminaMovement(static_cast<std::int16_t>(std::stoi(value, nullptr, 0)));
            break;
        case 13: // CORPSEDECAYTIMER
            SystemTimer(tSERVER_CORPSEDECAY, static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 14: // WEATHERTIMER
            SystemTimer(tSERVER_WEATHER, static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 15: // SHOPSPAWNTIMER
            SystemTimer(tSERVER_SHOPSPAWN, static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 16: // DECAYTIMER
            SystemTimer(tSERVER_DECAY, static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 17: // INVISIBILITYTIMER
            SystemTimer(tSERVER_INVISIBILITY, static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 18: // OBJECTUSETIMER
            SystemTimer(tSERVER_OBJECTUSAGE, static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 19: // GATETIMER
            SystemTimer(tSERVER_GATE, static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 20: // POISONTIMER
            SystemTimer(tSERVER_POISON, static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 21: // LOGINTIMEOUT
            SystemTimer(tSERVER_LOGINTIMEOUT, static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 22: // HITPOINTREGENTIMER
            SystemTimer(tSERVER_HITPOINTREGEN, static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 23: // STAMINAREGENTIMER
            SystemTimer(tSERVER_STAMINAREGEN, static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 24: // BASEFISHINGTIMER
            SystemTimer(tSERVER_FISHINGBASE, static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 34: // MAXPETOWNERS
            MaxPetOwners(static_cast<std::uint8_t>(std::stoul(value, nullptr, 0)));
            break;
        case 35: // MAXFOLLOWERS
            MaxFollowers(static_cast<std::uint8_t>(std::stoul(value, nullptr, 0)));
            break;
        case 36: // MAXCONTROLSLOTS
            MaxControlSlots(static_cast<std::uint8_t>(std::stoul(value, nullptr, 0)));
            break;
        case 37: // MANAREGENTIMER
            SystemTimer(tSERVER_MANAREGEN, static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 38: // RANDOMFISHINGTIMER
            SystemTimer(tSERVER_FISHINGRANDOM, static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 39: // SPIRITSPEAKTIMER
            SystemTimer(tSERVER_SPIRITSPEAK, static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;            break;
        case 50: // AMBIENTSOUNDS
            WorldAmbientSounds(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 56: // ACCOUNTFLUSH
            AccountFlushTimer(std::stod(value));
            break;
        case 59: // SELLMAXITEMS
            SellMaxItemsStatus(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 63: // CHECKITEMS
            CheckItemsSpeed(std::stod(value));
            break;
        case 64: // CHECKBOATS
            CheckBoatSpeed(std::stod(value));
            break;
        case 65: // CHECKNPCAI
            CheckNpcAISpeed(std::stod(value));
            break;
        case 66: // CHECKSPAWNREGIONS
            CheckSpawnRegionSpeed(std::stod(value));
            break;
        case 67: // POSTINGLEVEL
            MsgBoardPostingLevel(static_cast<std::uint8_t>(std::stoul(value, nullptr, 0)));
            break;
        case 68: // REMOVALLEVEL
            MsgBoardPostRemovalLevel(static_cast<std::uint8_t>(std::stoul(value, nullptr, 0)));
            break;
        case 70: // ESCORTINITEXPIRE
            SystemTimer(tSERVER_ESCORTWAIT, static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 71: // ESCORTACTIVEEXPIRE
            SystemTimer(tSERVER_ESCORTACTIVE, static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 72: // MOON1
            serverMoon(0, static_cast<std::int16_t>(std::stoi(value, nullptr, 0)));
            break;
        case 73: // MOON2
            serverMoon(1, static_cast<std::int16_t>(std::stoi(value, nullptr, 0)));
            break;
        case 74: // DUNGEONLEVEL
            dungeonLightLevel(static_cast<lightlevel_t>(std::stoul(value, nullptr, 0)));
            break;
        case 75: // CURRENTLEVEL
            worldLightCurrentLevel(static_cast<lightlevel_t>(std::stoul(value, nullptr, 0)));
            break;
        case 76: // BRIGHTLEVEL
            worldLightBrightLevel(static_cast<lightlevel_t>(std::stoul(value, nullptr, 0)));
            break;
        case 77: // BASERANGE
            TrackingBaseRange(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 78: // BASETIMER
            TrackingBaseTimer(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 79: // MAXTARGETS
            TrackingMaxTargets(static_cast<std::uint8_t>(std::stoul(value, nullptr, 0)));
            break;
        case 80: // MSGREDISPLAYTIME
            TrackingRedisplayTime(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 81: // MURDERDECAYTIMER
            SystemTimer(tSERVER_MURDERDECAY, static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 82: // MAXKILLS
            RepMaxKills(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 83: // CRIMINALTIMER
            SystemTimer(tSERVER_CRIMINAL, static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 84: // MINECHECK
            MineCheck(static_cast<std::uint8_t>(std::stoul(value, nullptr, 0)));
            break;
        case 85: // OREPERAREA
            ResOre(static_cast<std::int16_t>(std::stoi(value, nullptr, 0)));
            break;
        case 86: // ORERESPAWNTIMER
            ResOreTime(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 87: // RESOURCEAREASIZE
            ResourceAreaSize(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 88: // LOGSPERAREA
            ResLogs(static_cast<std::int16_t>(std::stoi(value, nullptr, 0)));
            break;
        case 89: // LOGSRESPAWNTIMER
            ResLogTime(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 91: // HUNGERRATE
            SystemTimer(tSERVER_HUNGERRATE, static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 92: // HUNGERDMGVAL
            HungerDamage(static_cast<std::int16_t>(std::stoi(value, nullptr, 0)));
            break;
        case 93: // MAXRANGE
            CombatMaxRange(static_cast<std::int16_t>(std::stoi(value, nullptr, 0)));
            break;
        case 94: // SPELLMAXRANGE
            CombatMaxSpellRange(static_cast<std::int16_t>(std::stoi(value, nullptr, 0)));
            break;
        case 97: // ANIMALATTACKCHANCE
            CombatAnimalsAttackChance(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 99: // NPCDAMAGERATE
            CombatNpcDamageRate(static_cast<std::int16_t>(std::stoi(value, nullptr, 0)));
            break;
        case 100: // NPCBASEFLEEAT
            CombatNPCBaseFleeAt(static_cast<std::int16_t>(std::stoi(value, nullptr, 0)));
            break;
        case 101: // NPCBASEREATTACKAT
            CombatNPCBaseReattackAt(static_cast<std::int16_t>(std::stoi(value, nullptr, 0)));
            break;
        case 102: // ATTACKSTAMINA
            CombatAttackStamina(static_cast<std::int16_t>(std::stoi(value, nullptr, 0)));
            break;
        case 104: // STARTGOLD
            ServerStartGold(static_cast<std::int16_t>(std::stoi(value, nullptr, 0)));
            break;
        case 105: // STARTPRIVS
            serverStartPrivs(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 106: // ESCORTDONEEXPIRE
            SystemTimer(tSERVER_ESCORTDONE, static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 107: // DARKLEVEL
            worldLightDarkLevel(static_cast<lightlevel_t>(std::stoul(value, nullptr, 0)));
            break;
        case 108: // TITLECOLOUR
            TitleColour(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 109: // LEFTTEXTCOLOUR
            LeftTextColour(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 110: // RIGHTTEXTCOLOUR
            RightTextColour(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 111: // BUTTONCANCEL
            ButtonCancel(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 112: // BUTTONLEFT
            ButtonLeft(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 113: // BUTTONRIGHT
            ButtonRight(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 114: // BACKGROUNDPIC
            BackgroundPic(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 115: // POLLTIME
            TownNumSecsPollOpen(static_cast<std::uint32_t>(std::stoul(value, nullptr, 0)));
            break;
        case 116: // MAYORTIME
            TownNumSecsAsMayor(static_cast<std::uint32_t>(std::stoul(value, nullptr, 0)));
            break;
        case 117: // TAXPERIOD
            TownTaxPeriod(static_cast<std::uint32_t>(std::stoul(value, nullptr, 0)));
            break;
        case 118: // GUARDSPAID
            TownGuardPayment(static_cast<std::uint32_t>(std::stoul(value, nullptr, 0)));
            break;
        case 119: // DAY
            ServerTimeDay(static_cast<std::int16_t>(std::stoi(value, nullptr, 0)));
            break;
        case 120: // HOURS
            ServerTimeHours(static_cast<std::uint8_t>(std::stoul(value, nullptr, 0)));
            break;
        case 121: // MINUTES
            ServerTimeMinutes(static_cast<std::uint8_t>(std::stoul(value, nullptr, 0)));
            break;
        case 122: // SECONDS
            ServerTimeSeconds(static_cast<std::uint8_t>(std::stoul(value, nullptr, 0)));
            break;
        case 123: // AMPM
            ServerTimeAMPM(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)) != 0);
            break;
        case 124: // SKILLLEVEL
            SkillLevel(static_cast<std::uint8_t>(std::stoul(value, nullptr, 0)));
            break;
        case 127: // SERVERLIST
            break;
        case 128: // PORT
            ServerPort(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 131: // ACCOUNTISOLATION
            break;
        case 136: // BACKUPSAVERATIO
            BackupRatio(static_cast<std::int16_t>(std::stoi(value, nullptr, 0)));
            break;
        case 138: // SECONDSPERUOMINUTE
            ServerSecondsPerUOMinute(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 139: // WEIGHTPERSTR
            // WeightPerStr( value.toUByte() );
            WeightPerStr(std::stof(value));
            break;
        case 140: // POLYDURATION
            SystemTimer(tSERVER_POLYMORPH, static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 142: // NETRCVTIMEOUT
            ServerNetRcvTimeout(static_cast<std::uint32_t>(std::stoul(value, nullptr, 0)));
            break;
        case 143: // NETSNDTIMEOUT
            ServerNetSndTimeout(static_cast<std::uint32_t>(std::stoul(value, nullptr, 0)));
            break;
        case 144: // NETRETRYCOUNT
            ServerNetRetryCount(static_cast<std::uint32_t>(std::stoul(value, nullptr, 0)));
            break;
        case 145: // CLIENTFEATURES
            ServerConfig::shared().clientFeature = static_cast<std::uint32_t>(std::stoul(value, nullptr, 0));
            break;
        case 147: // NPCMOVEMENTSPEED
            NPCWalkingSpeed(std::stof(value));
            break;
        case 149: // PETOFFLINETIMEOUT
            PetOfflineTimeout(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 150: // PETOFFLINECHECKTIMER
            SystemTimer(tSERVER_PETOFFLINECHECK, static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 155: // NPCRUNNINGSPEED
            NPCRunningSpeed(std::stof(value));
            break;
        case 156: // NPCFLEEINGSPEED
            NPCFleeingSpeed(std::stof(value));
            break;
        case 162: // SPAWNREGIONSFACETS
            setSpawnRegionsFacetStatus(static_cast<std::uint32_t>(std::stoul(value, nullptr, 0)));
            break;
        case 184: // WEAPONDAMAGECHANCE
            CombatWeaponDamageChance(static_cast<std::uint8_t>(std::stoul(value, nullptr, 0)));
            break;
        case 185: // ARMORDAMAGECHANCE
            CombatArmorDamageChance(static_cast<std::uint8_t>(std::stoul(value, nullptr, 0)));
            break;
        case 186: // WEAPONDAMAGEMIN
            CombatWeaponDamageMin(static_cast<std::uint8_t>(std::stoul(value, nullptr, 0)));
            break;
        case 187: // WEAPONDAMAGEMAX
            CombatWeaponDamageMax(static_cast<std::uint8_t>(std::stoul(value, nullptr, 0)));
            break;
        case 188: // ARMORDAMAGEMIN
            CombatArmorDamageMin(static_cast<std::uint8_t>(std::stoul(value, nullptr, 0)));
            break;
        case 189: // ARMORDAMAGEMAX
            CombatArmorDamageMax(static_cast<std::uint8_t>(std::stoul(value, nullptr, 0)));
            break;
        case 190: // GLOBALATTACKSPEED
            GlobalAttackSpeed(std::stof(value));
            break;
        case 191: // NPCSPELLCASTSPEED
            NPCSpellCastSpeed(std::stof(value));
            break;
        case 192: // FISHINGSTAMINALOSS
            FishingStaminaLoss(std::stof(value));
            break;
        case 219: // DECAYTIMERINHOUSE
            SystemTimer(tSERVER_DECAYINHOUSE, static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 222: // MAXHOUSESOWNABLE
            MaxHousesOwnable(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 223: // MAXHOUSESCOOWNABLE
            MaxHousesCoOwnable(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 227: // MAXPLAYERPACKITEMS
            MaxPlayerPackItems(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 228: // MAXPLAYERBANKITEMS
            MaxPlayerBankItems(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 240: // PARRYDAMAGECHANCE
            CombatParryDamageChance(static_cast<std::uint8_t>(std::stoul(value, nullptr, 0)));
            break;
        case 241: // PARRYDAMAGEMIN
            CombatParryDamageMin(static_cast<std::uint8_t>(std::stoul(value, nullptr, 0)));
            break;
        case 242: // PARRYDAMAGEMAX
            CombatParryDamageMax(static_cast<std::uint8_t>(std::stoul(value, nullptr, 0)));
            break;
        case 246: // ALCHEMYBONUSMODIFIER
            AlchemyDamageBonusModifier(static_cast<std::uint8_t>(std::stoul(value, nullptr, 0)));
            break;
        case 247: // NPCFLAGUPDATETIMER
            SystemTimer(tSERVER_NPCFLAGUPDATETIMER, static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 248: // JSENGINESIZE
            SetJSEngineSize(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 251: // THIRSTRATE
            SystemTimer(tSERVER_THIRSTRATE, static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 252: // THIRSTDRAINVAL
            ThirstDrain(static_cast<std::int16_t>(std::stoi(value, nullptr, 0)));
            break;
        case 255: // BLOODDECAYTIMER
            SystemTimer(tSERVER_BLOODDECAY, static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 256: // BLOODDECAYCORPSETIMER
            SystemTimer(tSERVER_BLOODDECAYCORPSE, static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 257: // BLOODEFFECTCHANCE
            CombatBloodEffectChance(static_cast<std::uint8_t>(std::stoul(value, nullptr, 0)));
            break;
        case 258: // NPCCORPSEDECAYTIMER
            SystemTimer(tSERVER_NPCCORPSEDECAY, static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 266: // BUYBANKTHRESHOLD
            BuyThreshold(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 269: // NPCMOUNTEDWALKINGSPEED
            NPCMountedWalkingSpeed(std::stof(value));
            break;
        case 270: // NPCMOUNTEDRUNNINGSPEED
            NPCMountedRunningSpeed(std::stof(value));
            break;
        case 271: // NPCMOUNTEDFLEEINGSPEED
            NPCMountedFleeingSpeed(std::stof(value));
            break;
        case 273: // SERVERLANGUAGE
            ServerLanguage(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 275: // PETLOYALTYGAINONSUCCESS
            SetPetLoyaltyGainOnSuccess(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 276: // PETLOYALTYLOSSONFAILURE
            SetPetLoyaltyLossOnFailure(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 277: // LOYALTYRATE
            SystemTimer(tSERVER_LOYALTYRATE, static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 279: // FISHPERAREA
            ResFish(static_cast<std::int16_t>(std::stoi(value, nullptr, 0)));
            break;
        case 280: // FISHRESPAWNTIMER
            ResFishTime(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 281: // ARCHERYHITBONUS
            CombatArcheryHitBonus(static_cast<std::int16_t>(std::stoi(value, nullptr, 0)));
            break;
        case 283: // SYSMESSAGECOLOUR
            SysMsgColour(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
       case 290: // ARCHERYSHOOTDELAY
            CombatArcheryShootDelay(std::stof(value));
            break;
        case 291: // MAXCLIENTBYTESIN
            MaxClientBytesIn(static_cast<std::uint32_t>(std::stoul(value, nullptr, 0)));
            break;
        case 292: // MAXCLIENTBYTESOUT
            MaxClientBytesOut(static_cast<std::uint32_t>(std::stoul(value, nullptr, 0)));
            break;
        case 293: // NETTRAFFICTIMEBAN
            NetTrafficTimeban(static_cast<std::uint32_t>(std::stoul(value, nullptr, 0)));
            break;
        case 299: // MAXSAFETELEPORTSPERDAY
            MaxSafeTeleportsPerDay(static_cast<std::uint8_t>(std::stoul(value, nullptr, 0)));
            break;
        case 308: // GLOBALRESTOCKMULTIPLIER
            GlobalRestockMultiplier(std::stof(value));
            break;
        case 319: // MAXPLAYERPACKWEIGHT
            MaxPlayerPackWeight(static_cast<std::int32_t>(std::stoi(value, nullptr, 0)));
            break;
        case 320: // MAXPLAYERBANKWEIGHT
            MaxPlayerBankWeight(static_cast<std::int32_t>(std::stoi(value, nullptr, 0)));
            break;
        case 328: // WEAPONDAMAGEBONUSTYPE
            WeaponDamageBonusType(static_cast<std::uint8_t>(std::stoul(value, nullptr, 0)));
            break;
        case 332: // BODGOLDREWARDMULTIPLIER
            BODGoldRewardMultiplier(std::stof(value));
            break;
        case 333: // BODFAMEREWARDMULTIPLIER
            BODFameRewardMultiplier(std::stof(value));
            break;
        case 336: // AGGRESSORFLAGTIMER
            SystemTimer(tSERVER_AGGRESSORFLAG, static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 337: // PERMAGREYFLAGTIMER
            SystemTimer(tSERVER_PERMAGREYFLAG, static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 338: // STEALINGFLAGTIMER
            SystemTimer(tSERVER_STEALINGFLAG, static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 340: // APSPERFTHRESHOLD
            APSPerfThreshold(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 341: // APSINTERVAL
            APSInterval(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 342: // APSDELAYSTEP
            APSDelayStep(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        case 343: // APSDELAYMAXCAP
            APSDelayMaxCap(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
            break;
        default:
            rValue = false;
            break;
    }
    return rValue;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ServerStartGold()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default amount of starting gold for new characters
// o------------------------------------------------------------------------------------------------o
auto CServerData::ServerStartGold() const -> std::int16_t { return startGold; }
auto CServerData::ServerStartGold(std::int16_t value) -> void {
    if (value >= 0) {
        startGold = value;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::serverStartPrivs()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default starting privs for new characters
// o------------------------------------------------------------------------------------------------o
auto CServerData::serverStartPrivs() const -> std::uint16_t { return startPrivs; }
auto CServerData::serverStartPrivs(std::uint16_t value) -> void { startPrivs = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::serverMoon()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the phase of one of the game's two moons
// o------------------------------------------------------------------------------------------------o
auto CServerData::serverMoon(std::int16_t slot) const -> std::int16_t {
    std::int16_t rValue = -1;
    if (slot >= 0 && slot <= 1) {
        rValue = moon[slot];
    }
    return rValue;
}
auto CServerData::serverMoon(std::int16_t slot, std::int16_t value) -> void {
    if (slot >= 0 && slot <= 1 && value >= 0) {
        moon[slot] = value;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::dungeonLightLevel()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default, global light level for dungeons
// o------------------------------------------------------------------------------------------------o
auto CServerData::dungeonLightLevel() const -> lightlevel_t { return worldDungeonLightLevel; }
auto CServerData::dungeonLightLevel(lightlevel_t value) -> void { worldDungeonLightLevel = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::worldLightCurrentLevel()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default, global current light level outside of
// dungeons
// o------------------------------------------------------------------------------------------------o
auto CServerData::worldLightCurrentLevel() const -> lightlevel_t { return worldCurrentLightLevel; }
auto CServerData::worldLightCurrentLevel(lightlevel_t value) -> void { worldCurrentLightLevel = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::worldLightBrightLevel()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default, global light level for the brightest time of
// day
// o------------------------------------------------------------------------------------------------o
auto CServerData::worldLightBrightLevel() const -> lightlevel_t { return worldBrightnessLightLevel; }
auto CServerData::worldLightBrightLevel(lightlevel_t value) -> void { worldBrightnessLightLevel = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::worldLightDarkLevel()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default, global light level for the darkest time of
// day
// o------------------------------------------------------------------------------------------------o
auto CServerData::worldLightDarkLevel() const -> lightlevel_t { return worldDarknessLightLevel; }
auto CServerData::worldLightDarkLevel(lightlevel_t value) -> void { worldDarknessLightLevel = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::postLoadDefaults()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	If no start locations have been provided in ini, use hardcoded
// defaults
// o------------------------------------------------------------------------------------------------o
auto CServerData::postLoadDefaults() -> void {
    /*
    if (startlocations.empty()) {
        ServerLocation("Yew,Center,545,982,0,0,0,1075072");
        ServerLocation("Minoc,Tavern,2477,411,15,0,0,1075073");
        ServerLocation("Britain,Sweet Dreams Inn,1495,1629,10,0,0,1075074");
        ServerLocation("Moonglow,Docks,4406,1045,0,0,0,1075075");
        ServerLocation("Trinsic,West Gate,1832,2779,0,0,0,1075076");
        ServerLocation("Magincia,Docks,3675,2259,26,0,0,1075077");
        ServerLocation("Jhelom,Docks,1492,3696,0,0,0,1075078");
        ServerLocation("Skara Brae,Docks,639,2236,0,0,0,1075079");
        ServerLocation("Vesper,Ironwood Inn,2771,977,0,0,0,1075080");
    }
    
    // Also load young player start locations, which default to same as normal players
    if (YoungPlayerSystem() && youngStartlocations.empty()) {
        YoungServerLocation("Yew,Center,545,982,0,0,0,1075072");
        YoungServerLocation("Minoc,Tavern,2477,411,15,0,0,1075073");
        YoungServerLocation("Britain,Sweet Dreams Inn,1495,1629,10,0,0,1075074");
        YoungServerLocation("Moonglow,Docks,4406,1045,0,0,0,1075075");
        YoungServerLocation("Trinsic,West Gate,1832,2779,0,0,0,1075076");
        YoungServerLocation("Magincia,Docks,3675,2259,26,0,0,1075077");
        YoungServerLocation("Jhelom,Docks,1492,3696,0,0,0,1075078");
        YoungServerLocation("Skara Brae,Docks,639,2236,0,0,0,1075079");
        YoungServerLocation("Vesper,Ironwood Inn,2771,977,0,0,0,1075080");
    }
     */
}


// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ServerSecondsPerUOMinute()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the number of real life seconds per UO minute
// o------------------------------------------------------------------------------------------------o
auto CServerData::ServerSecondsPerUOMinute() const -> std::uint16_t { return secondsPerUoMinute; }
auto CServerData::ServerSecondsPerUOMinute(std::uint16_t newVal) -> void { secondsPerUoMinute = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ServerLanguage()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default (if any) language used for the server
// o------------------------------------------------------------------------------------------------o
auto CServerData::ServerLanguage() const -> std::uint16_t { return serverLanguage; }
auto CServerData::ServerLanguage(std::uint16_t newVal) -> void {
    if (newVal < DL_COUNT) {
        serverLanguage = newVal;
    }
    else {
        serverLanguage = DL_DEFAULT;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::MaxClientBytesIn()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets max incoming bytes received from a socket in each 10
// second period |					If amount exceeds this cap, client might
// receive a warning/get kicked
// o------------------------------------------------------------------------------------------------o
auto CServerData::MaxClientBytesIn() const -> std::uint32_t { return maxBytesIn; }
auto CServerData::MaxClientBytesIn(std::uint32_t newVal) -> void { maxBytesIn = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::MaxClientBytesOut()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets max outgoing bytes sent to a socket in each 10 second
// period |					If amount exceeds this cap, client might receive a
// warning/get kicked
// o------------------------------------------------------------------------------------------------o
auto CServerData::MaxClientBytesOut() const -> std::uint32_t { return maxBytesOut; }
auto CServerData::MaxClientBytesOut(std::uint32_t newVal) -> void { maxBytesOut = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::NetTrafficTimeban()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets max amount of time a player will be temporarily banned for
// if they |					exceed their alotted network traffic budget
// o------------------------------------------------------------------------------------------------o
auto CServerData::NetTrafficTimeban() const -> std::uint32_t { return trafficTimeban; }
auto CServerData::NetTrafficTimeban(std::uint32_t newVal) -> void { trafficTimeban = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::GetJSEngineSize()
//|					CServerData::SetJSEngineSize()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets jsEngineSize (in MB), used to define max bytes per
// JSRuntime |					before a last ditch GC effort is made
// o------------------------------------------------------------------------------------------------o
auto CServerData::GetJSEngineSize() const -> std::uint16_t { return jsEngineSize; }
auto CServerData::SetJSEngineSize(std::uint16_t newVal) -> void { jsEngineSize = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::APSPerfThreshold()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the performance threshold (simulation cycles) below which
// the
//|					APS - Adaptive Performance System - kicks in. A value of 0
//disables the system
// o------------------------------------------------------------------------------------------------o
auto CServerData::APSPerfThreshold() const -> std::uint16_t { return apsPerfThreshold; }
auto CServerData::APSPerfThreshold(std::uint16_t newVal) -> void { apsPerfThreshold = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::APSInterval()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the interval at which APS will check shard performance and
// make |					adjustments if necessary
// o------------------------------------------------------------------------------------------------o
auto CServerData::APSInterval() const -> std::uint16_t { return apsInterval; }
auto CServerData::APSInterval(std::uint16_t newVal) -> void { apsInterval = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::APSDelayStep()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the step value - in milliseconds - that is used by APS to
//|					gradually increase/decrease the delay effect for checking
//NPC AI/movement stuff
// o------------------------------------------------------------------------------------------------o
auto CServerData::APSDelayStep() const -> std::uint16_t { return apsDelayStep; }
auto CServerData::APSDelayStep(std::uint16_t newVal) -> void { apsDelayStep = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::APSDelayMaxCap()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max cap - in milliseconds - for how much NPC
// AI/movement stuff can |					be delayed by APS in an effort to
// restore shard performance to above threshold levels
// o------------------------------------------------------------------------------------------------o
auto CServerData::APSDelayMaxCap() const -> std::uint16_t { return apsDelayMaxCap; }
auto CServerData::APSDelayMaxCap(std::uint16_t newVal) -> void { apsDelayMaxCap = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::SaveTime()
//|	Date		-	January 28th, 2007
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Outputs server time information to time.wsc in the /shared/
// directory
// o------------------------------------------------------------------------------------------------o
auto CServerData::SaveTime() -> void {
    auto timeFile = ServerConfig::shared().directoryFor(dirlocation_t::SAVE) / std::filesystem::path("time.wsc");
    std::ofstream timeDestination(timeFile.string());
    if (!timeDestination) {
        Console::shared().error(util::format("Failed to open %s for writing", timeFile.string().c_str()));
        return;
    }
    
    timeDestination << "[TIME]" << '\n' << "{" << '\n';
    timeDestination << "CURRENTLIGHT=" << static_cast<std::uint16_t>(worldLightCurrentLevel()) << '\n';
    timeDestination << "DAY=" << ServerTimeDay() << '\n';
    timeDestination << "HOUR=" << static_cast<std::uint16_t>(ServerTimeHours()) << '\n';
    timeDestination << "MINUTE=" << static_cast<std::uint16_t>(ServerTimeMinutes()) << '\n';
    timeDestination << "AMPM=" << (ServerTimeAMPM() ? 1 : 0) << '\n';
    timeDestination << "MOON=" << serverMoon(0) << "," << serverMoon(1) << '\n';
    timeDestination << "}" << '\n' << '\n';
    
    timeDestination.close();
}

auto ReadWorldTagData(std::istream &inStream, std::string &tag, std::string &data) -> void;
// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::LoadTime()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads server time information from time.wsc in the /shared/
// directory
// o------------------------------------------------------------------------------------------------o
auto CServerData::LoadTime() -> void {
    std::ifstream input;
    auto filename = ServerConfig::shared().directoryFor(dirlocation_t::SAVE) / std::filesystem::path("time.wsc");
    
    input.open(filename.string(), std::ios_base::in);
    input.seekg(0, std::ios::beg);
    
    if (input.is_open()) {
        char line[1024];
        while (!input.eof() && !input.fail()) {
            input.getline(line, 1023);
            line[input.gcount()] = 0;
            std::string sLine(line);
            sLine = util::trim(util::strip(sLine, "//"));
            if (!sLine.empty()) {
                if (util::upper(sLine) == "[TIME]") {
                    LoadTimeTags(input);
                }
            }
        }
        input.close();
    }
}

//==============================================================================================
auto CServerData::LoadTimeTags(std::istream &input) -> void {
    std::string UTag, tag, data;
    while (tag != "o---o") {
        ReadWorldTagData(input, tag, data);
        if (tag != "o---o") {
            UTag = util::upper(tag);
            
            if (UTag == "AMPM") {
                ServerTimeAMPM((std::stoi(data, nullptr, 0) == 1));
            }
            else if (UTag == "CURRENTLIGHT") {
                worldLightCurrentLevel(static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)));
            }
            else if (UTag == "DAY") {
                ServerTimeDay(static_cast<std::int16_t>(std::stoi(data, nullptr, 0)));
            }
            else if (UTag == "HOUR") {
                ServerTimeHours(static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)));
            }
            else if (UTag == "MINUTE") {
                ServerTimeMinutes(static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)));
            }
            else if (UTag == "MOON") {
                auto csecs = oldstrutil::sections(data, ",");
                if (csecs.size() > 1) {
                    serverMoon(0, static_cast<std::int16_t>(std::stoi(
                                                                      util::trim(util::strip(csecs[0], "//")), nullptr, 0)));
                    serverMoon(1, static_cast<std::int16_t>(std::stoi(
                                                                      util::trim(util::strip(csecs[1], "//")), nullptr, 0)));
                }
            }
        }
    }
    tag = "";
}

//==============================================================================================
auto CServerData::ServerTimeDay() const -> std::int16_t { return days; }
//==============================================================================================
auto CServerData::ServerTimeDay(std::int16_t nValue) -> void { days = nValue; }
//==============================================================================================
auto CServerData::ServerTimeHours() const -> std::uint8_t { return hours; }
//==============================================================================================
auto CServerData::ServerTimeHours(std::uint8_t nValue) -> void { hours = nValue; }
//==============================================================================================
auto CServerData::ServerTimeMinutes() const -> std::uint8_t { return minutes; }

//==============================================================================================
auto CServerData::ServerTimeMinutes(std::uint8_t nValue) -> void { minutes = nValue; }
//==============================================================================================
auto CServerData::ServerTimeSeconds() const -> std::uint8_t { return seconds; }
//==============================================================================================
auto CServerData::ServerTimeSeconds(std::uint8_t nValue) -> void { seconds = nValue; }
//==============================================================================================
auto CServerData::ServerTimeAMPM() const -> bool { return ampm; }
//==============================================================================================
auto CServerData::ServerTimeAMPM(bool nValue) -> void { ampm = nValue; }

//==============================================================================================
auto CServerData::IncSecond() -> bool {
    bool rValue = false;
    ++seconds;
    if (seconds == 60) {
        seconds = 0;
        rValue = IncMinute();
    }
    return rValue;
}

//==============================================================================================
auto CServerData::IncMoon(std::int32_t mNumber) -> void {
    moon[mNumber] = static_cast<std::int16_t>((moon[mNumber] + 1) % 8);
}

//==============================================================================================
auto CServerData::IncMinute() -> bool {
    bool rValue = false;
    ++minutes;
    if (minutes % 8 == 0) {
        IncMoon(0);
    }
    if (minutes % 3 == 0) {
        IncMoon(1);
    }
    
    if (minutes == 60) {
        minutes = 0;
        rValue = IncHour();
    }
    return rValue;
}

//==============================================================================================
auto CServerData::IncHour() -> bool {
    ++hours;
    bool retVal = false;
    if (hours == 12) {
        if (ampm) {
            retVal = IncDay();
        }
        hours = 0;
        ampm = !ampm;
    }
    return retVal;
}

//==============================================================================================
auto CServerData::IncDay() -> bool {
    ++days;
    return true;
}
