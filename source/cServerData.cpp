#include "uox3.h"
#include "scriptc.h"
#include "ssection.h"
#include "Dictionary.h"
#include <filesystem>
#include <cstdint>
#include <limits>
#include <optional>
#include "IP4Address.hpp"
#if PLATFORM != WINDOWS
#include <netdb.h>

#include <arpa/inet.h>
#endif

#include "StringUtility.hpp"
#include "osunique.hpp"

using namespace std::string_literals;
//===========================================================================================
// constant data
//===========================================================================================
//===========================================================================================
const std::map<std::string,std::int32_t> CServerData::uox3inicasevalue{
	// these numbers dont have to be in order, and can skip aound
	// they just CANT be repeated (now we could add some protection
	// but they dont change that much, and there are a lot, so if you delete one
	// dont worry about reusing it. Just keep adding up from the last one would
	// be easiest.
	// They key is that number HAS to match the case statement number
	// in
	//bool CServerData::HandleLine( const std::string& tag, const std::string& value )
	// in cServerData.cpp (this file).
	{"SERVERNAME"s, 1},
	{"CONSOLELOG"s, 2},
	{"COMMANDPREFIX"s, 3},
	{"ANNOUNCEWORLDSAVES"s, 4},
	{"BACKUPSENABLED"s, 5},
	{"SAVESTIMER"s, 6},
	{"SKILLCAP"s, 7},
	{"SKILLDELAY"s, 8},
	{"STATCAP"s, 9},
	{"MAXSTEALTHMOVEMENTS"s, 10},
	{"MAXSTAMINAMOVEMENTS"s, 11},
	{"ARMORAFFECTMANAREGEN"s, 12},
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
	{"BASEFISHINGTIMER"s,24},
	{"SCRIPTSDIRECTORY"s, 25},
	{"JOINPARTMSGS"s, 26},
	{"MAXPETOWNERS"s, 34},
	{"MAXFOLLOWERS"s, 35},
	{"MAXCONTROLSLOTS"s, 36},
	{"MANAREGENTIMER"s, 37},
	{"RANDOMFISHINGTIMER"s, 38},
	{"SPIRITSPEAKTIMER"s, 39},
	{"DIRECTORY"s, 40},
	{"DATADIRECTORY"s, 41},
	{"DEFSDIRECTORY"s, 42},
	{"ACTSDIRECTORY"s, 43},
	{"BACKUPDIRECTORY"s, 44},
	{"MSGBOARDDIRECTORY"s, 45},
	{"SHAREDDIRECTORY"s, 46},
	{"LOOTDECAYSWITHCORPSE"s, 47},
	{"GUARDSACTIVE"s, 49},
	{"DEATHANIMATION"s, 27},
	{"AMBIENTSOUNDS"s, 50},
	{"AMBIENTFOOTSTEPS"s, 51},
	{"INTERNALACCOUNTCREATION"s, 52},
	{"SHOWOFFLINEPCS"s, 53},
	{"ROGUESENABLED"s, 54},
	{"PLAYERPERSECUTION"s, 55},
	{"ACCOUNTFLUSH"s, 56},
	{"HTMLSTATUSENABLED"s, 57},
	{"SELLBYNAME"s, 58},
	{"SELLMAXITEMS"s, 59},
	{"TRADESYSTEM"s, 60},
	{"RANKSYSTEM"s, 61},
	{"CUTSCROLLREQUIREMENTS"s, 62},
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
	{"STATSAFFECTSKILLCHECKS"s, 90 },
	{"HUNGERRATE"s, 91},
	{"HUNGERDMGVAL"s, 92},
	{"MAXRANGE"s, 93},
	{"SPELLMAXRANGE"s,94},
	{"DISPLAYHITMSG"s, 95},
	{"MONSTERSVSANIMALS"s, 96},
	{"ANIMALATTACKCHANCE"s, 97},
	{"ANIMALSGUARDED"s, 98},
	{"NPCDAMAGERATE"s, 99},
	{"NPCBASEFLEEAT"s, 100},
	{"NPCBASEREATTACKAT"s, 101},
	{"ATTACKSTAMINA"s, 102},
	{"LOCATION"s, 103},
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
	{"SNOOPISCRIME"s, 125},
	{"BOOKSDIRECTORY"s, 126},
	//{"SERVERLIST"s, 127},
	{"PORT"s, 128},
	{"ACCESSDIRECTORY"s, 129},
	{"LOGSDIRECTORY"s, 130},
	{"ACCOUNTISOLATION"s, 131},
	{"HTMLDIRECTORY"s, 132},
	{"SHOOTONANIMALBACK"s, 133},
	{"NPCTRAININGENABLED"s, 134},
	{"DICTIONARYDIRECTORY"s, 135},
	{"BACKUPSAVERATIO"s, 136},
	{"HIDEWHILEMOUNTED"s, 137},
	{"SECONDSPERUOMINUTE"s, 138},
	{"WEIGHTPERSTR"s, 139},
	{"POLYDURATION"s, 140},
	{"UOGENABLED"s, 141},
	{"NETRCVTIMEOUT"s, 142},
	{"NETSNDTIMEOUT"s, 143},
	{"NETRETRYCOUNT"s, 144},
	{"CLIENTFEATURES"s, 145},
	{"OVERLOADPACKETS"s, 146},
	{"NPCMOVEMENTSPEED"s, 147},
	{"PETHUNGEROFFLINE"s, 148},
	{"PETOFFLINETIMEOUT"s, 149},
	{"PETOFFLINECHECKTIMER"s, 150},
	{"ARCHERRANGE"s, 151},
	{"ADVANCEDPATHFINDING"s, 152},
	{"SERVERFEATURES"s, 153},
	{"LOOTINGISCRIME"s, 154},
	{"NPCRUNNINGSPEED"s, 155},
	{"NPCFLEEINGSPEED"s, 156},
	{"BASICTOOLTIPSONLY"s, 157},
	{"GLOBALITEMDECAY"s, 158},
	{"SCRIPTITEMSDECAYABLE"s, 159},
	{"BASEITEMSDECAYABLE"s, 160},
	{"ITEMDECAYINHOUSES"s, 161},
	{"SPAWNREGIONSFACETS"s, 162},
	{"PAPERDOLLGUILDBUTTON"s, 163},
	{"ATTACKSPEEDFROMSTAMINA"s, 164},
	{"DISPLAYDAMAGENUMBERS"s, 169},
	{"CLIENTSUPPORT4000"s, 170},
	{"CLIENTSUPPORT5000"s, 171},
	{"CLIENTSUPPORT6000"s, 172},
	{"CLIENTSUPPORT6050"s, 173},
	{"CLIENTSUPPORT7000"s, 174},
	{"CLIENTSUPPORT7090"s, 175},
	{"CLIENTSUPPORT70160"s, 176},
	{"CLIENTSUPPORT70240"s, 177},
	{"CLIENTSUPPORT70300"s, 178},
	{"CLIENTSUPPORT70331"s, 179},
	{"CLIENTSUPPORT704565"s, 180},
	{"CLIENTSUPPORT70610"s, 181},
	{"EXTENDEDSTARTINGSTATS"s, 182},
	{"EXTENDEDSTARTINGSKILLS"s, 183},
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
	{"ASSISTANTNEGOTIATION"s, 194},
	{"KICKONASSISTANTSILENCE"s, 195},
	{"AF_FILTERWEATHER"s, 196},
	{"AF_FILTERLIGHT"s, 197},
	{"AF_SMARTTARGET"s, 198},
	{"AF_RANGEDTARGET"s, 199},
	{"AF_AUTOOPENDOORS"s, 200},
	{"AF_DEQUIPONCAST"s, 201},
	{"AF_AUTOPOTIONEQUIP"s, 202},
	{"AF_POISONEDCHECKS"s, 203},
	{"AF_LOOPEDMACROS"s, 204},
	{"AF_USEONCEAGENT"s, 205},
	{"AF_RESTOCKAGENT"s, 206},
	{"AF_SELLAGENT"s, 207},
	{"AF_BUYAGENT"s, 208},
	{"AF_POTIONHOTKEYS"s, 209},
	{"AF_RANDOMTARGETS"s, 210},
	{"AF_CLOSESTTARGETS"s, 211},
	{"AF_OVERHEADHEALTH"s, 212},
	{"AF_AUTOLOOTAGENT"s, 213},
	{"AF_BONECUTTERAGENT"s, 214},
	{"AF_JSCRIPTMACROS"s, 215},
	{"AF_AUTOREMOUNT"s, 216},
	// 217 free
	{"CLASSICUOMAPTRACKER"s, 218},
	{"DECAYTIMERINHOUSE"s, 219},
	{"PROTECTPRIVATEHOUSES"s, 220},
	{"TRACKHOUSESPERACCOUNT"s, 221},
	{"MAXHOUSESOWNABLE"s, 222},
	{"MAXHOUSESCOOWNABLE"s, 223},
	{"CANOWNANDCOOWNHOUSES"s, 224},
	{"COOWNHOUSESONSAMEACCOUNT"s, 225},
	{"ITEMSDETECTSPEECH"s, 226},
	{"MAXPLAYERPACKITEMS"s, 227},
	{"MAXPLAYERBANKITEMS"s, 228},
	{"FORCENEWANIMATIONPACKET"s, 229},
	{"MAPDIFFSENABLED"s, 230},
	{"PARRYDAMAGECHANCE"s, 240},
	{"PARRYDAMAGEMIN"s, 241},
	{"PARRYDAMAGEMAX"s, 242},
	{"ARMORCLASSDAMAGEBONUS"s, 243},
	{"CUOENABLED"s, 244},
	{"ALCHEMYBONUSENABLED"s, 245},
	{"ALCHEMYBONUSMODIFIER"s, 246},
	{"NPCFLAGUPDATETIMER"s, 247},
	{"JSENGINESIZE"s, 248},
	{"USEUNICODEMESSAGES"s, 249},
	{"SCRIPTDATADIRECTORY"s, 250},
	{"THIRSTRATE"s, 251},
	{"THIRSTDRAINVAL"s, 252},
	{"PETTHIRSTOFFLINE"s, 253},
	{"EXTERNALIP"s,254},
	{"BLOODDECAYTIMER"s, 255},
	{"BLOODDECAYCORPSETIMER"s, 256},
	{"BLOODEFFECTCHANCE"s, 257},
	{"NPCCORPSEDECAYTIMER"s, 258},
	{"HUNGERENABLED"s, 259},
	{"THIRSTENABLED"s, 260},
	{"TRAVELSPELLSFROMBOATKEYS"s, 261},
	{"TRAVELSPELLSWHILEOVERWEIGHT"s, 262},
	{"MARKRUNESINMULTIS"s, 263},
	{"TRAVELSPELLSBETWEENWORLDS"s, 264},
	{"TRAVELSPELLSWHILEAGGRESSOR"s, 265},
	{"BANKBUYTHRESHOLD"s, 266},
	{"NETWORKLOG"s, 267},
	{"SPEECHLOG"s, 268},
	{"NPCMOUNTEDWALKINGSPEED"s, 269},
	{"NPCMOUNTEDRUNNINGSPEED"s, 270},
	{"NPCMOUNTEDFLEEINGSPEED"s, 271},
	{"CONTEXTMENUS"s, 272},
	{"SERVERLANGUAGE"s, 273},
	{"CHECKPETCONTROLDIFFICULTY"s, 274},
	{"PETLOYALTYGAINONSUCCESS"s, 275},
	{"PETLOYALTYLOSSONFAILURE"s, 276},
	{"PETLOYALTYRATE"s, 277},
	{"SHOWNPCTITLESINTOOLTIPS"s, 278},
	{"FISHPERAREA"s, 279},
	{"FISHRESPAWNTIMER"s, 280},
	{"ARCHERYHITBONUS"s, 281},
	{"ITEMSINTERRUPTCASTING"s, 282},
	{"SYSMESSAGECOLOUR"s, 283},
	{"AF_AUTOBANDAGE"s, 284},
	{"AF_ENEMYTARGETSHARE"s, 285},
	{"AF_FILTERSEASON"s, 286},
	{"AF_SPELLTARGETSHARE"s, 287},
	{"AF_HUMANOIDHEALTHCHECKS"s, 288},
	{"AF_SPEECHJOURNALCHECKS"s, 289},
	{"ARCHERYSHOOTDELAY"s, 290},
	{"MAXCLIENTBYTESIN"s, 291},
	{"MAXCLIENTBYTESOUT"s, 292},
	{"NETTRAFFICTIMEBAN"s, 293},
	{"TOOLUSELIMIT"s, 294},
	{"TOOLUSEBREAK"s, 295},
	{"ITEMREPAIRDURABILITYLOSS"s, 296},
	{"HIDESTATSFORUNKNOWNMAGICITEMS"s, 297},
	{"CRAFTCOLOUREDWEAPONS"s, 298},
	{"MAXSAFETELEPORTSPERDAY"s, 299},
	{"TELEPORTTONEARESTSAFELOCATION"s, 300},
	{"ALLOWAWAKENPCS"s, 301}
};
constexpr auto MAX_TRACKINGTARGETS = 128 ;
constexpr auto SKILLTOTALCAP = 7000 ;
constexpr auto SKILLCAP = 1000 ;
constexpr auto STATCAP = 225 ;

constexpr auto BIT_ANNOUNCESAVES		= std::uint32_t(0);
constexpr auto BIT_ANNOUNCEJOINPART		= std::uint32_t(1);
constexpr auto BIT_SERVERBACKUP		= std::uint32_t(2);
constexpr auto BIT_SHOOTONANIMALBACK	= std::uint32_t(3);
constexpr auto BIT_NPCTRAINING		= std::uint32_t(4);
constexpr auto BIT_LOOTDECAYSONCORPSE	= std::uint32_t(5);
constexpr auto BIT_GUARDSENABLED		= std::uint32_t(6);
constexpr auto BIT_PLAYDEATHANIMATION	= std::uint32_t(7);
constexpr auto BIT_AMBIENTFOOTSTEPS		= std::uint32_t(8);
constexpr auto BIT_INTERNALACCOUNTS		= std::uint32_t(9);
constexpr auto BIT_SHOWOFFLINEPCS		= std::uint32_t(10);
constexpr auto BIT_ROGUESTATUS		= std::uint32_t(11);
constexpr auto BIT_SNOOPISCRIME		= std::uint32_t(12);
constexpr auto BIT_PERSECUTIONSTATUS	= std::uint32_t(13);
constexpr auto BIT_SELLBYNAME			= std::uint32_t(14);
constexpr auto BIT_TRADESYSSTATUS		= std::uint32_t(15);
constexpr auto BIT_RANKSYSSTATUS		= std::uint32_t(16);
constexpr auto BIT_CUTSCROLLREQ		= std::uint32_t(17);
constexpr auto BIT_SHOWHITMESSAGE		= std::uint32_t(18);
constexpr auto BIT_ESCORTSTATUS		= std::uint32_t(19);
constexpr auto BIT_MONSTERSVSANIMALS	= std::uint32_t(20);
constexpr auto BIT_PETHUNGEROFFLINE		= std::uint32_t(21);
constexpr auto BIT_HIDEWHILEMOUNTED		= std::uint32_t(22);
constexpr auto BIT_OVERLOADPACKETS		= std::uint32_t(23);
constexpr auto BIT_ARMORAFFECTMANAREGEN 	= std::uint32_t(24);
constexpr auto BIT_ANIMALSGUARDED		= std::uint32_t(25);
constexpr auto BIT_ADVANCEDPATHFIND		= std::uint32_t(26);
constexpr auto BIT_LOOTINGISCRIME		= std::uint32_t(27);
constexpr auto BIT_BASICTOOLTIPSONLY	= std::uint32_t(28);
constexpr auto BIT_GLOBALITEMDECAY		= std::uint32_t(29);
constexpr auto BIT_SCRIPTITEMSDECAYABLE	= std::uint32_t(30);
constexpr auto BIT_BASEITEMSDECAYABLE	= std::uint32_t(31);
constexpr auto BIT_ITEMDECAYINHOUSES	= std::uint32_t(32);
constexpr auto BIT_PAPERDOLLGUILDBUTTON 	= std::uint32_t(33);
constexpr auto BIT_ATTSPEEDFROMSTAMINA	= std::uint32_t(34);
constexpr auto BIT_SHOWDAMAGENUMBERS	= std::uint32_t(35);
// 37 free!
constexpr auto BIT_EXTENDEDSTARTINGSTATS	= std::uint32_t(38);
constexpr auto BIT_EXTENDEDSTARTINGSKILLS	= std::uint32_t(39);
constexpr auto BIT_ASSISTANTNEGOTIATION	= std::uint32_t(40);
constexpr auto BIT_KICKONASSISTANTSILENCE	= std::uint32_t(41);
constexpr auto BIT_CLASSICUOMAPTRACKER	= std::uint32_t(42);
constexpr auto BIT_PROTECTPRIVATEHOUSES	= std::uint32_t(43);
constexpr auto BIT_TRACKHOUSESPERACCOUNT	= std::uint32_t(44);
constexpr auto BIT_CANOWNANDCOOWNHOUSES	= std::uint32_t(45);
constexpr auto BIT_COOWNHOUSESONSAMEACCOUNT	= std::uint32_t(46);
constexpr auto BIT_ITEMSDETECTSPEECH		= std::uint32_t(47);
constexpr auto BIT_FORCENEWANIMATIONPACKET	= std::uint32_t(48);
constexpr auto BIT_MAPDIFFSENABLED			= std::uint32_t(49);
constexpr auto BIT_ARMORCLASSDAMAGEBONUS		= std::uint32_t(50);
constexpr auto BIT_CONNECTUOSERVERPOLL		= std::uint32_t(51);
constexpr auto BIT_ALCHEMYDAMAGEBONUSENABLED 	= std::uint32_t(52);
constexpr auto BIT_PETTHIRSTOFFLINE         	= std::uint32_t(53);
constexpr auto BIT_USEUNICODEMESSAGES 		= std::uint32_t(54);
constexpr auto BIT_HUNGERSYSTEMENABLED		= std::uint32_t(55);
constexpr auto BIT_THIRSTSYSTEMENABLED		= std::uint32_t(56);
constexpr auto BIT_TRAVELSPELLSFROMBOATKEYS	= std::uint32_t(57);
constexpr auto BIT_TRAVELSPELLSWHILEOVERWEIGHT	= std::uint32_t(58);
constexpr auto BIT_MARKRUNESINMULTIS		= std::uint32_t(59);
constexpr auto BIT_TRAVELSPELLSBETWEENWORLDS	= std::uint32_t(60);
constexpr auto BIT_TRAVELSPELLSWHILEAGGRESSOR	= std::uint32_t(61);
constexpr auto BIT_CONSOLELOG				= std::uint32_t(62);
constexpr auto BIT_NETWORKLOG				= std::uint32_t(63);
constexpr auto BIT_SPEECHLOG				= std::uint32_t(64);
constexpr auto BIT_CONTEXTMENUS			= std::uint32_t(65);
constexpr auto BIT_CHECKPETCONTROLDIFFICULTY		= std::uint32_t(66);
constexpr auto BIT_SHOWNPCTITLESINTOOLTIPS		= std::uint32_t(67);
constexpr auto BIT_ITEMSINTERRUPTCASTING			= std::uint32_t(68);
constexpr auto BIT_STATSAFFECTSKILLCHECKS			= std::uint32_t(69);
constexpr auto BIT_TOOLUSELIMIT				= std::uint32_t(70);
constexpr auto BIT_TOOLUSEBREAK				= std::uint32_t(71);
constexpr auto BIT_ITEMREPAIRDURABILITYLOSS		= std::uint32_t(72);
constexpr auto BIT_HIDESTATSFORUNKNOWNMAGICITEMS	= std::uint32_t(73);
constexpr auto BIT_CRAFTCOLOUREDWEAPONS			= std::uint32_t(74);
constexpr auto BIT_TELEPORTTONEARESTSAFELOC		= std::uint32_t(75);
constexpr auto BIT_ALLOWAWAKENPCS				= std::uint32_t(76);

// New uox3.ini format lookup
// January 13, 2001	- 	Modified: January 30, 2001 Converted to uppercase
// February 26 2002 	- 	Modified: to support the AccountIsolation, and left out dir3ectory tags
// September 22 2002 	- 	Added the  "HIDEWHILEMOUNTED" tag to support hide fix
// September 06 2003 	- 	Removed unused tags (heartbeat, wilderness, uoxbot, lagfix)
// October 16, 2003 	- 	Removed unused tag (SAVEMODE) and added "WEIGHTPERSTR".
// April 3, 2004 		- 	Added new tags, for UOG support, as well as new facet tags etc.
// June 15, 2004 		- 	Added the new tags for the xFTPd, and xFTPc support.
// April 2, 2005 		- 	Removed superfluous tags (BEGGINGTIMER, HUNGERDAMAGERATE, HUNGERTHRESHOLD, BEGGINGRANGE)
// September 13, 2005 	- 	Added MAPCOUNT to the uox.ini to specify the number of maps UOX3 uses.
// November 20, 2005 	- 	Removed USEFACETSAVES, MAP0, MAP1, MAP2, MAP3, USERMAP, FTPDENABLED,
//								FTPDUSER, FTPDUSERLIMIT, FTPDBIND, FTPDROOT, FTPCENABLED, FTPCHOST,
//								FTPCPORT, FTPCUSER, FTPCULFLAGS, and MAPCOUNT as they were unused.
//								Added OVERLOADPACKETS option to uox.ini to toggle JS packet handling.
// December 13, 2005 	- 	Changed uox.ini entry SHOWHIDDENNPCS to SHOWOFFLINEPCS to better reflect its function.
//								Removed CACHEMULS from the uox.ini as it is no longer used.
// February 12, 2006	- 	Added NPCMOVEMENTSPEED to the UOX.ini to allow customization of NPC movement speeds.
// July 7, 2006 		- 	PLAYERPERSECUTION in uox.ini (allows ghosts to drain mana from other players) set to 0 by default
// July 18, 2006 		- 	Added uox.ini flag to disable/enable the A* Pathfinding routine: ADVANCEDPATHFINDING=0/1
// July 24, 2006 		- 	Simplified many entries in uox.ini, making them easier to read.
// January 28, 2007 	- 	Moved dynamic parts of [worldlight] section from uox.ini to their own output file in the /shared/ directory
// March 3, 2007 		- 	Removed CLIENTSUPPORT from the uox.ini
//						  	Allowed customization of supported client features via uox.ini
//									CLIENTFEATURES
//									SERVERFEATURES
// April 9, 2007 		- 	Added a new setting to the uox.ini, LOOTINGISCRIME. If you set it to 0 looting corpses of innocent chars is not taken as a crime.
// June 10, 2007 		- 	Added two new settings to the uox.ini, NPCRUNNINGSPEED and NPCFLEEINGSPEED, they work the same way as NPCMOVEMENTSPEED.
// July 28, 2007 		- 	Added a new setting to the uox.ini: BASICTOOLTIPSONLY. If this is set to 1 the tooltips will only contain basic information, like the name and the weight of an item.
// February 7, 2010 	- 	Added new UOX.INI settings:
//		GLOBALITEMDECAY - Toggles on / off item decay on a global scale.Note that it does not remove the decay - status from items, it just resets the decay - timer when it reaches 0
//		SCRIPTITEMSDECAYABLE - Toggles whether DFN - items will decay by default or not.Can be overriden by DECAY tag in item - DFNs
//		BASEITEMSDECAYABLE - Toggles whether base - items will decay by default or not.Can be overriden by DECAY tag in harditems.dfn
//		ITEMDECAYINHOUSES - Toggles default decay of non - locked down items inside multis( houses and boats )
// December 14, 20011 	- 	Exposed CombatExplodeDelay to UOX.INI, and changed the default delay between targeting and damage for the Explosion spell from 0 to 2 seconds
// March 19, 2012 	- 	Added support for packet 0xD7, SubCommand 0x28 - Guild button on paperdoll, which gives access to guild-functions if character belongs to a guild.
//		Can be enabled / disabled through UOX.INI setting PAPERDOLLGUILDBUTTON = 0 / 1. Defaults to 0.
// Added new UOX.INI option to determine whether attack - speed bonuses are gained from Stamina( default ) or Dexterity: ATTACKSPEEDFROMSTAMINA = 1 / 0
// Added new UOX.INI option to control the displaying of damage - numbers in combat( previously used DISPLAYHITMSG for numbers too ) : DISPLAYDAMAGENUMBERS = 1 / 0
// Added a new section to UOX.INI - [clientsupport] - along with the following settings to determine approved client - versions for the server :
//		CLIENTSUPPORT4000 = 0 / 1
//		CLIENTSUPPORT5000 = 0 / 1
//		CLIENTSUPPORT6000 = 0 / 1
//		CLIENTSUPPORT6050 = 0 / 1
//		CLIENTSUPPORT7000 = 0 / 1
//		CLIENTSUPPORT7090 = 0 / 1
//		CLIENTSUPPORT70160 = 0 / 1
//		CLIENTSUPPORT70240 = 0 / 1
//		NOTE: Each of these settings represent a range of clients, not just the individual versions mentioned.This means that CLIENTSUPPORT4000, for instance, will allow or disallow connections from clients 4.0.0 to 4.0.11f.Also note that while it is possible to enable support for all clients at the same time, it highly recommended to restrict support for client versions that match up to what the server is running.
// Added new UOX.INI option : EXTENDEDSTARTINGSTATS = 0 / 1
//		If enabled, makes new characters start with 90 statpoints( selectable in clients 7.0.16.0 + only, lower versions only get 90 if using templates ) instead of 80.
// Added new UOX.INI option : EXTENDEDSTARTINGSKILLS = 0 / 1
//		If enabled, allows for four starting skills( selectable in clients 7.0.16.0 + only, lower versions only get 4th skill if using templates ) instead of three
// November 14, 2012	-	Fixed issue where DISPLAYDAMAGENUMBERS and ATTACKSPEEDFROMSTAMINA settings in UOX.INI were overwriting the DISPLAYHITMSG setting instead of their own respective settings
// November 10, 2013	-	Added new UOX.INI settings for adjusting combat-speed on a global scale:
//		GLOBALATTACKSPEED = 1 	// for adjusting speed of melee and ranged attacks globally for all chars
//		NPCSPELLCASTSPEED = 1 	// for adjusting the overall speed of spellcasts for NPCs (base spellcast speed determined by SPADELAY tag in NPC dfns)
// Added new UOX.INI settings to make hardcoded damage to armor and weapons in combat more customizable :
//		WEAPONDAMAGECHANCE = 17 	// Chance of weapons taking damage when attacking
//		WEAPONDAMAGEMIN = 0		// Minimum amount of damage a weapon takes if damaged in combat
//		WEAPONDAMAGEMAX = 1		// Maximum amount of damage a weapon takes if damaged in combat
//		ARMORDAMAGECHANCE = 25	// Chance of armor taking damage when defending
//		ARMORDAMAGEMIN = 0		// Minimum amount of damage armor can take if damaged in combat
//		ARMORDAMAGEMAX = 1		// Maximum amount of damage armor can take if damaged in combat
// September 12, 2015 	- 	Added new UOX.INI setting for defining the amount of stamina lost when using the fishing skill
//		FISHINGSTAMINALOSS = 2	// The amount of stamina lost when using the fishing skill
// November 24, 2019 	- 	Added new uox.ini flag for choosing a random starting location for new players (from list in uox.ini)
//		RANDOMSTARTINGLOCATION = 0	// 0 to disable (default), 1 to enable

// NOTE:	Very important the first lookups required duplication or the search fails on them

//+++++++++++++++++++++++++++++++++++++++++++++++
auto CServerData::lookupINIValue(const std::string& tag) ->std::int32_t  {
	auto iter = uox3inicasevalue.find(tag) ;
	if (iter != uox3inicasevalue.end()){
		return iter->second ;
	}
	return std::numeric_limits<std::int32_t>::max();
}


//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void ResetDefaults()
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Reset server settings to default
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ResetDefaults()->void {
	resettingDefaults = true;
	serverList.resize( 1 );		// Set the initial count to hold one server.
	
	ServerIP( "127.0.0.1" );
	ServerPort( 2593 );
	ExternalIP("127.0.0.1");
	serverList[0].setPort( 2593 );
	ServerName( "My UOX3 Shard" );
	
	// Set default gcMaxBytes limit in MB per JS runtime
	// If set too low, UOX3 might crash when reloading (full) JS engine
	// JS API references describe it as "Maximum nominal heap before last ditch GC"
	SetJSEngineSize( 256 );
	
	// Send server-originating messages i`n Unicode format, if possible
	UseUnicodeMessages( true );
	
	ServerLanguage( DL_DEFAULT );
	SystemTimer( tSERVER_POTION, 10 );
	ServerMoon( 0, 0 );
	ServerMoon( 1, 0 );
	DungeonLightLevel( 3 );
	WorldLightCurrentLevel( 0 );
	WorldLightBrightLevel( 0 );
	WorldLightDarkLevel( 5 );
	
	ServerNetRcvTimeout( 3 );
	ServerNetSndTimeout( 3 );
	ServerNetRetryCount( 3 );
	MaxClientBytesIn( 25000 );
	MaxClientBytesOut( 100000 );
	NetTrafficTimeban( 30 );
	
	ServerSecondsPerUOMinute( 5 );
	ServerTimeDay( 0 );
	ServerTimeHours( 0 );
	ServerTimeMinutes( 0 );
	ServerTimeSeconds( 0 );
	ServerTimeAMPM( 0 );
	
	InternalAccountStatus( false );
	CombatMaxRange( 10 );
	CombatMaxSpellRange( 10 );
	
	// load defaults values
	SystemTimer( tSERVER_SHOPSPAWN, 300 );
	SystemTimer( tSERVER_POISON, 180 );
	SystemTimer( tSERVER_DECAY, 300 );
	SystemTimer( tSERVER_DECAYINHOUSE, 3600 );
	ServerSkillTotalCap( 7000 );
	ServerSkillCap( 1000 );
	ServerStatCap( 225 );
	StatsAffectSkillChecks( false );
	CorpseLootDecay( true );
	ServerSavesTimer( 600 );
	
	SystemTimer( tSERVER_INVISIBILITY, 60 );
	SystemTimer( tSERVER_HUNGERRATE, 6000 );
	HungerDamage( 2 );
	HungerSystemEnabled( true );
	
	SystemTimer( tSERVER_THIRSTRATE, 6000 );
	ThirstDrain( 2 );
	ThirstSystemEnabled( false );
	
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
	ServerUOGEnabled( true );
	ConnectUOServerPoll( true );
	ServerContextMenus( true );
	SysMsgColour( 0x0048 );
	
	CombatMonstersVsAnimals( true );
	CombatAnimalsAttackChance( 2 );
	CombatAnimalsGuarded( false );
	CombatNPCBaseFleeAt( 20 );
	CombatNPCBaseReattackAt( 40 );
	ShootOnAnimalBack( false );
	SellByNameStatus( false );
	SkillLevel( 5 );
	SellMaxItemsStatus( 250 );
	CombatNPCDamageRate( 1 );
	RankSystemStatus( true );
	CombatArcheryHitBonus( 10 );
	CombatArcheryShootDelay( 1.0 );
	CombatWeaponDamageChance( 17 );
	CombatWeaponDamageMin( 0 );
	CombatWeaponDamageMax( 1 );
	CombatArmorDamageChance( 33 );
	CombatArmorDamageMin( 0 );
	CombatArmorDamageMax( 1 );
	CombatParryDamageChance( 20 );
	CombatParryDamageMin( 0 );
	CombatParryDamageMax( 1 );
	CombatBloodEffectChance( 75 );
	GlobalAttackSpeed( 1.0 );
	NPCSpellCastSpeed( 1.0 );
	FishingStaminaLoss( 2.0 );
	CombatArmorClassDamageBonus( false );
	AlchemyDamageBonusEnabled( false );
	AlchemyDamageBonusModifier( 5 );
	CheckPetControlDifficulty( true );
	SetPetLoyaltyGainOnSuccess( 1 );
	SetPetLoyaltyLossOnFailure( 3 );
	SystemTimer( tSERVER_LOYALTYRATE, 900 );
	ItemsInterruptCasting( true );
	
	auto curWorkingDir = std::filesystem::current_path().string();
	
	auto wDir = oldstrutil::fixDirectory(curWorkingDir);
	std::string tDir;
	Directory( CSDDP_ROOT, wDir );
	tDir = wDir + std::string("muldata/");
	Directory( CSDDP_DATA, tDir );
	tDir = wDir + std::string("dfndata/");
	Directory( CSDDP_DEFS, tDir );
	tDir = wDir + std::string("accounts/");
	Directory( CSDDP_ACCOUNTS, tDir );
	Directory( CSDDP_ACCESS, tDir );
	tDir = wDir + std::string("js/");
	Directory( CSDDP_SCRIPTS, tDir );
	tDir = wDir + std::string( "js/jsdata" );
	Directory( CSDDP_SCRIPTDATA, tDir );
	tDir = wDir + std::string("archives/");
	Directory( CSDDP_BACKUP, tDir );
	tDir = wDir + std::string("msgboards/");
	Directory( CSDDP_MSGBOARD, tDir );
	tDir = wDir + std::string("shared/");
	Directory( CSDDP_SHARED, tDir );
	tDir = wDir + std::string("html/");
	Directory( CSDDP_HTML, tDir );
	tDir = wDir + std::string("books/");
	Directory( CSDDP_BOOKS, tDir );
	tDir = wDir + std::string("dictionaries/");
	Directory( CSDDP_DICTIONARIES, tDir );
	tDir = wDir + std::string("logs/");
	Directory( CSDDP_LOGS, tDir );
	
	BuyThreshold( 2000 );
	GuardStatus( true );
	ServerAnnounceSaves( true );
	WorldAmbientSounds( 5 );
	ServerJoinPartAnnouncements( true );
	ServerConsoleLog( true );
	ServerNetworkLog( false );
	ServerSpeechLog( false );
	RogueStatus( true );
	SystemTimer( tSERVER_WEATHER, 60 );
	SystemTimer( tSERVER_LOGINTIMEOUT, 300 );
	BackupRatio( 5 );
	MaxStealthMovement( 10 );
	MaxStaminaMovement( 15 );
	SnoopIsCrime( false );
	SystemTimer( tSERVER_NPCFLAGUPDATETIMER, 10 );
	PetOfflineTimeout( 5 );
	PetHungerOffline( true );
	SystemTimer( tSERVER_PETOFFLINECHECK, 600 );
	ItemsDetectSpeech( false );
	MaxPlayerPackItems( 125 );
	MaxPlayerBankItems( 125 );
	ForceNewAnimationPacket( true );
	MapDiffsEnabled( false );
	TravelSpellsFromBoatKeys( true );
	TravelSpellsWhileOverweight( false );
	MarkRunesInMultis( true );
	TravelSpellsBetweenWorlds( false );
	TravelSpellsWhileAggressor( false );
	MaxControlSlots( 0 ); // Default to 0, which is equal to off
	MaxFollowers( 5 );
	MaxPetOwners( 10 );
	ToolUseLimit( true );
	ToolUseBreak( true );
	ItemRepairDurabilityLoss( true );
	HideStatsForUnknownMagicItems( true );
	CraftColouredWeapons( false );
	MaxSafeTeleportsPerDay( 1 );
	TeleportToNearestSafeLocation( false );
	AllowAwakeNPCs( false );
	
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
	ShowNpcTitlesInTooltips( true );
	GlobalItemDecay( true );
	ScriptItemsDecayable( true );
	BaseItemsDecayable( false );
	SystemTimer( tSERVER_ESCORTWAIT, 900 );
	SystemTimer( tSERVER_ESCORTACTIVE, 600 );
	SystemTimer( tSERVER_ESCORTDONE, 600 );
	AmbientFootsteps( false );
	ServerCommandPrefix( '\'' );
	
	CheckSpawnRegionSpeed( 30 );
	CheckItemsSpeed( 1.5 );
	NPCWalkingSpeed( 0.6 );
	NPCRunningSpeed( 0.3 );
	NPCFleeingSpeed( 0.4 );
	NPCMountedWalkingSpeed( 0.3 );
	NPCMountedRunningSpeed( 0.12 );
	NPCMountedFleeingSpeed( 0.2 );
	AccountFlushTimer( 5.0 );
	
	// RESOURCES
	ResourceAreaSize( 8 );
	ResLogs( 3 );
	ResLogTime( 600 );
	ResOre( 10 );
	ResOreTime( 600 );
	ResFish( 10 );
	ResFishTime( 600 );
	
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
	
	TitleColour( 0 );
	LeftTextColour( 0 );
	RightTextColour( 0 );
	ButtonCancel( 4017 );
	ButtonLeft( 4014 );
	ButtonRight( 4005 );
	BackgroundPic( 5054 );
	
	// Houses
	ItemDecayInHouses( true );
	ProtectPrivateHouses( true );
	TrackHousesPerAccount( true );
	CanOwnAndCoOwnHouses( true );
	CoOwnHousesOnSameAccount( true );
	MaxHousesOwnable( 1 );
	MaxHousesCoOwnable( 10 );
	
	// Towns
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
	
	// Disable spawn regions for all facets by default
	SetSpawnRegionsFacetStatus( 0 );
	
	// Set no assistant features as disabled by default
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
	SystemTimer( tSERVER_CORPSEDECAY, 420 );
	SystemTimer( tSERVER_NPCCORPSEDECAY, 420 );
	SystemTimer( tSERVER_BLOODDECAYCORPSE, 210 ); // Default to half the decay timer of a npc corpse
	SystemTimer( tSERVER_BLOODDECAY, 3 ); // Keep it short and sweet
	startlocations.clear() ;
	resettingDefaults = false;
}
//===============================================================================================
CServerData::CServerData() {
	startup() ;
	availableIPs = ip4list_t::available() ;
}
//===============================================================================================
auto CServerData::startup() ->void{
	ResetDefaults();
}



//o-----------------------------------------------------------------------------------------------o
//|	Function	-	std::string ServerName() const
//|					void ServerName( std::string setname )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets server name. Sets to specified value, or to default name if no value specified
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ServerName() const ->const std::string& {
	return serverList[0].getName();
}
auto CServerData::ServerName( const std::string &setname )->void {
	if( serverList.empty() ){
		serverList.resize( 1 );
	}
	serverList[0].setName( setname );
	if( setname.empty() ) {
		serverList[0].setName( "My UOX3 Shard" );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	std::string ServerDomain() const
//|					void ServerDomain( std::string setdomain )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets server domain. Sets to specified value, or to no domain if no value specified
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ServerDomain() const ->const std::string& {
	return serverList[0].getDomain();
}
auto CServerData::ServerDomain( const std::string &setdomain )->void{
	if( serverList.empty() ){
		serverList.resize( 1 );
	}
	if( setdomain.empty() ){
		serverList[0].setDomain( "" );
	}
	else {
		serverList[0].setDomain( setdomain );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	std::string CServerData::ServerIP() const
//|					void ServerIP( std::string setip )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets server IP. Sets to specified value, or to loopback IP if no value specified
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ServerIP() const ->const std::string & {
	return serverList[0].getIP();
}
auto CServerData::ServerIP( const std::string &setip ) ->void {
	if( serverList.empty() ){
		serverList.resize( 1 );
	}
	if( setip.empty() ){
		serverList[0].setIP("127.0.0.1");
	}
	else{
		serverList[0].setIP(setip);
	}
}
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	std::string CServerData::ExternalIP() const
//|					void CServerData::ExternalIP( const std::string &ip )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets external (WAN) IP for server
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ExternalIP() const ->const std::string& {
	return externalIP;
}
auto CServerData::ExternalIP( const std::string &ip ) ->void{
	externalIP = ip;
	//IP4Address::setExternal( externalIP );
}
auto CServerData::matchIP(const ip4addr_t &ip) const -> ip4addr_t {
	auto [candidate,match] = availableIPs.bestmatch(ip) ;
	if (match == 0){
		if (!externalIP.empty()){
			candidate = ip4addr_t(externalIP);
		}
	}
	else {
		// We got some kind of match, see if on same network type?
		if (candidate.type() != ip.type()){
			if (ip.type() == ip4addr_t::ip4type_t::wan) {
				if (!externalIP.empty()) {
					candidate = ip4addr_t(externalIP) ;
				}
			}
		}
	}
	return candidate ;
	
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ServerPort() const
//|					void ServerPort( UI16 setport )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Set server port to specified value, or to default port if no value specified
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ServerPort() const ->std::uint16_t {
	return port;
}
auto CServerData::ServerPort( std::uint16_t setport )->void{
	if( setport == 0 ){
		port = 2593;
	}
	else {
		port = setport;
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ServerConsoleLog() const
//|					void ServerConsoleLog( bool setting )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Set logging of console messages, warnings and errors
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ServerConsoleLog() const ->bool {
	return consolelogenabled;
}
auto CServerData::ServerConsoleLog(bool setting) ->void {
	consolelogenabled = setting;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ServerNetworkLog() const
//|					void ServerNetworkLog( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets logging of network traffic to logs folder
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ServerNetworkLog() const ->bool{
	return boolVals.test( BIT_NETWORKLOG );
}
auto CServerData::ServerNetworkLog(bool newVal) ->void{
	boolVals.set( BIT_NETWORKLOG, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ServerSpeechLog() const
//|					void ServerSpeechLog( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets logging of player/staff speech to logs folder
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ServerSpeechLog() const ->bool{
	return boolVals.test( BIT_SPEECHLOG );
}
auto CServerData::ServerSpeechLog(bool newVal) ->void{
	boolVals.set( BIT_SPEECHLOG, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	char ServerCommandPrefix() const
//|					void ServerCommandPrefix( char cmdvalue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Set command prefix
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ServerCommandPrefix() const ->char{
	
	return commandprefix.empty()?0:commandprefix[0];
}
auto CServerData::ServerCommandPrefix( char cmdvalue )->void {
	commandprefix = std::string(1,cmdvalue);
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ServerAnnounceSavesStatus() const
//|					void ServerAnnounceSaves( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Set status of server accouncements for worldsaves
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ServerAnnounceSavesStatus() const ->bool{

	return boolVals.test( BIT_ANNOUNCESAVES );
}
auto CServerData::ServerAnnounceSaves(bool newVal) ->void {
	boolVals.set( BIT_ANNOUNCESAVES, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ServerJoinPartAnnouncementsStatus() const
//|					void ServerJoinPartAnnouncements( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets status of server accouncements for players connecting/disconnecting
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ServerJoinPartAnnouncementsStatus() const ->bool {

	return boolVals.test( BIT_ANNOUNCEJOINPART );
}
auto CServerData::ServerJoinPartAnnouncements(bool newVal) ->void {
	boolVals.set( BIT_ANNOUNCEJOINPART, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ServerBackupStatus() const
//|					void ServerBackups( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets enabled status of server worldsave backups
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ServerBackupStatus() const ->bool {

	return boolVals.test( BIT_SERVERBACKUP );
}
auto CServerData::ServerBackups(bool newVal) ->void {
	boolVals.set( BIT_SERVERBACKUP, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ServerContextMenus() const
//|					void ServerContextMenus( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets enabled status of context menus
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ServerContextMenus() const->bool {

	return boolVals.test( BIT_CONTEXTMENUS );
}
auto CServerData::ServerContextMenus(bool newVal) ->void {
	boolVals.set( BIT_CONTEXTMENUS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 SysMsgColour() const
//|					void SysMsgColour( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default text colour for system messages displayed in bottom left
//o-----------------------------------------------------------------------------------------------o
auto CServerData::SysMsgColour() const ->std::uint16_t {
	return sysMsgColour;
}
auto CServerData::SysMsgColour(UI16 value) ->void {
	sysMsgColour = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 ServerSavesTimerStatus() const
//|					void ServerSavesTimer( UI32 timer )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets server world save timer
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ServerSavesTimerStatus() const ->std::uint32_t {
	return serversavestimer;
}
auto CServerData::ServerSavesTimer(UI32 timer) ->void {
	serversavestimer = timer;
	if( timer < 180 )			// 3 minutes is the lowest value you can set saves for
		serversavestimer = 300;	// 10 minutes default
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ServerSkillCapStatus() const
//|					void ServerSkillCap( UI16 cap )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets cap for individual skills
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ServerSkillCapStatus() const ->std::uint16_t {
	return skillcap;
}
auto CServerData::ServerSkillCap(UI16 cap) ->void {
	skillcap = cap;
	if( cap < 1 )		// Default is on second loop sleeping
		skillcap = SKILLCAP;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ServerSkillTotalCapStatus() const
//|					void ServerSkillTotalCap( UI16 cap )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets total cap for all skills
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ServerSkillTotalCapStatus() const ->std::uint16_t {
	return skilltotalcap;
}
auto CServerData::ServerSkillTotalCap(UI16 cap) ->void {
	skilltotalcap = cap;
	if( cap < 1 )		// Default is on second loop sleeping
		skilltotalcap = SKILLTOTALCAP;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 ServerSkillDelayStatus() const
//|					void ServerSkillDelay( UI08 skdelay )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global delay for skill usage
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ServerSkillDelayStatus() const ->std::uint8_t {
	return skilldelay;
}
auto CServerData::ServerSkillDelay(UI08 skdelay) ->void {
	skilldelay = skdelay;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ServerStatCapStatus() const
//|					void ServerStatCap( UI16 cap )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the total stat cap
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ServerStatCapStatus() const ->std::uint16_t {
	return statcap;
}
auto CServerData::ServerStatCap(UI16 cap) ->void {
	statcap = cap;
	if( cap < 1 )		// Default is on second loop sleeping
		statcap = STATCAP;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 MaxStealthMovement() const
//|					void MaxStealthMovement( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max number of steps allowed to take while stealthed
//o-----------------------------------------------------------------------------------------------o
auto CServerData::MaxStealthMovement() const ->std::int16_t {
	return maxstealthmovement;
}
auto CServerData::MaxStealthMovement(SI16 value) ->void {
	maxstealthmovement = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 MaxStaminaMovement() const
//|					void MaxStaminaMovement( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max number of steps allowed while running before stamina is reduced
//o-----------------------------------------------------------------------------------------------o
auto CServerData::MaxStaminaMovement() const ->std::int16_t {
	return maxstaminamovement;
}
auto CServerData::MaxStaminaMovement(SI16 value) ->void {
	maxstaminamovement = value;
}

auto CServerData::BuildSystemTimeValue( cSD_TID timerID ) const ->TIMERVAL {
	return BuildTimeValue( static_cast<R32>(SystemTimer( timerID )) );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 SystemTimer( cSD_TID timerid ) const
//|					void SystemTimer( cSD_TID timerid, UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets a specific server timer
//o-----------------------------------------------------------------------------------------------o
auto CServerData::SystemTimer( cSD_TID timerid ) const ->std::uint16_t {
	return serverTimers[timerid];
}
auto CServerData::SystemTimer(cSD_TID timerid, UI16 value) ->void {
	serverTimers[timerid] = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	std::string Directory( CSDDirectoryPaths dp )
//|					void Directory( CSDDirectoryPaths dp, std::string value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets directory paths
//o-----------------------------------------------------------------------------------------------o
auto CServerData::Directory( CSDDirectoryPaths dp ) ->std::string {
	std::string rvalue;
	if( dp != CSDDP_COUNT ){
		rvalue = serverDirectories[dp];
	}
	return rvalue;
}
auto CServerData::Directory( CSDDirectoryPaths dp, std::string value ) ->void {
	bool create_dir = false;
	
	if( dp != CSDDP_COUNT ) {
		std::string verboseDirectory;
		switch( dp ) {
			case CSDDP_ROOT: verboseDirectory = "Root directory"; break;
			case CSDDP_DATA: verboseDirectory = "Data directory"; break;
			case CSDDP_DEFS: verboseDirectory = "DFNs directory"; break;
			case CSDDP_ACCESS: verboseDirectory = "Access directory"; break;
			case CSDDP_ACCOUNTS: verboseDirectory = "Accounts directory"; break;
			case CSDDP_SCRIPTS: verboseDirectory = "Scripts directory"; break;
			case CSDDP_SCRIPTDATA: verboseDirectory = "ScriptData directory";
				create_dir = true;
				break;
			case CSDDP_BACKUP: verboseDirectory = "Backup directory"; break;
			case CSDDP_MSGBOARD: verboseDirectory = "Messageboard directory"; break;
			case CSDDP_SHARED: verboseDirectory = "Shared directory";
				create_dir = true;
				break;
			case CSDDP_HTML: verboseDirectory = "HTML directory"; break;
			case CSDDP_LOGS: verboseDirectory = "Logs directory"; break;
			case CSDDP_DICTIONARIES: verboseDirectory = "Dictionary directory"; break;
			case CSDDP_BOOKS: verboseDirectory = "Books directory"; break;
			case CSDDP_COUNT:
			default: verboseDirectory = "Unknown directory"; break;
		};
		// First, let's normalize the path name and fix common errors
		// remove all trailing and leading spaces...
		auto sText = oldstrutil::trim(value) ;
		
		if(sText.empty()) {
			Console.error( oldstrutil::format(" %s directory is blank, set in uox.ini", verboseDirectory.c_str() ));
			Shutdown( FATAL_UOX3_DIR_NOT_FOUND );
		}
		else {
			// Make sure we're terminated with a directory separator
			// Just incase it's not set in the .ini
			// and convert the windows backward slashes to forward slashes
			
			sText = oldstrutil::fixDirectory(sText);
			
			bool error = false;
			if(!resettingDefaults) {
				error = true;
				//auto curWorkingDir = fixDirectory(std::filesystem::current_path().string());
				
				auto npath = std::filesystem::path(sText);
				
				if (std::filesystem::exists(npath)) {
					error = false;
				}
				else if (create_dir) {
					// Create missing directory
					std::filesystem::create_directory(npath);
					error = false;
				}
			}
			
			if(error){
				Console.error( oldstrutil::format("%s %s does not exist", verboseDirectory.c_str(), sText.c_str()) );
				Shutdown( FATAL_UOX3_DIR_NOT_FOUND );
			}
			else{
				// There was a check to see if text was empty, to set to "./".  However, if text was blank, we bailed out in the
				// beginning of the routine
				serverDirectories[dp] = sText;
			}
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ShootOnAnimalBack() const
//|					void ShootOnAnimalBack( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether players can shoot arrows while riding mounts
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ShootOnAnimalBack() const ->bool {
	return boolVals.test( BIT_SHOOTONANIMALBACK );
}
auto CServerData::ShootOnAnimalBack( bool newVal ) ->void {
	boolVals.set( BIT_SHOOTONANIMALBACK, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool NPCTrainingStatus() const
//|					void NPCTrainingStatus( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether NPCs can train players in skills
//o-----------------------------------------------------------------------------------------------o
auto CServerData::NPCTrainingStatus() const ->bool {
	return boolVals.test( BIT_NPCTRAINING );
}
auto CServerData::NPCTrainingStatus(bool newVal) ->void {
	boolVals.set( BIT_NPCTRAINING, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void dumpPaths()
//|	Date		-	02/26/2002
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Needed a function that would dump out the paths. If you add any paths to the
//|					server please make sure that you place exports to the console here to please
//|					 so it can be looked up if needed.
//o-----------------------------------------------------------------------------------------------o
auto CServerData::dumpPaths() ->void {
	Console.PrintSectionBegin();
	Console << "PathDump: \n";
	Console << "    Root        : " << Directory( CSDDP_ROOT ) << "\n";
	Console << "    Accounts    : " << Directory( CSDDP_ACCOUNTS ) << "\n";
	Console << "    Access      : " << Directory( CSDDP_ACCESS ) << "\n";
	Console << "    Mul(Data)   : " << Directory( CSDDP_DATA ) << "\n";
	Console << "    DFN(Defs)   : " << Directory( CSDDP_DEFS ) << "\n";
	Console << "    JScript     : " << Directory( CSDDP_SCRIPTS ) << "\n";
	Console << "    JScriptData : " << Directory( CSDDP_SCRIPTDATA ) << "\n";
	Console << "    HTML        : " << Directory( CSDDP_HTML ) << "\n";
	Console << "    MSGBoards   : " << Directory( CSDDP_MSGBOARD ) << "\n";
	Console << "    Books       : " << Directory( CSDDP_BOOKS ) << "\n";
	Console << "    Shared      : " << Directory( CSDDP_SHARED ) << "\n";
	Console << "    Backups     : " << Directory( CSDDP_BACKUP ) << "\n";
	Console << "    Logs        : " << Directory( CSDDP_LOGS ) << "\n";
	Console.PrintSectionBegin();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CorpseLootDecay() const
//|					void CorpseLootDecay( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether loot decays along with corpses or is left on ground
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CorpseLootDecay() const ->bool {
	return boolVals.test( BIT_LOOTDECAYSONCORPSE );
}
auto CServerData::CorpseLootDecay(bool newVal) ->void {
	boolVals.set( BIT_LOOTDECAYSONCORPSE, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GuardsStatus() const
//|					void GuardStatus( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether guards are enabled globally or not
//o-----------------------------------------------------------------------------------------------o
auto CServerData::GuardsStatus() const ->bool {
	return boolVals.test( BIT_GUARDSENABLED );
}
auto CServerData::GuardStatus(bool newVal) ->void {
	boolVals.set( BIT_GUARDSENABLED, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool DeathAnimationStatus() const
//|					void DeathAnimationStatus( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether death animation plays when a player dies
//o-----------------------------------------------------------------------------------------------o
auto CServerData::DeathAnimationStatus() const ->bool {
	return boolVals.test( BIT_PLAYDEATHANIMATION );
}
auto CServerData::DeathAnimationStatus(bool newVal) ->void {
	boolVals.set( BIT_PLAYDEATHANIMATION, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 WorldAmbientSounds() const
//|					void WorldAmbientSounds( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets intensity of ambient world sounds, bird chirps, animal sounds, etc
//o-----------------------------------------------------------------------------------------------o
auto CServerData::WorldAmbientSounds() const ->std::int16_t {
	return ambientsounds;
}
auto CServerData::WorldAmbientSounds(SI16 value) ->void {
	if( value < 0 ){
		ambientsounds = 0;
	}
	else{
		ambientsounds = value;
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool AmbientFootsteps() const
//|					void AmbientFootsteps( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether context-specific footstep sounds are enabled or not
//o-----------------------------------------------------------------------------------------------o
auto CServerData::AmbientFootsteps() const ->bool {
	return boolVals.test( BIT_AMBIENTFOOTSTEPS );
}
auto CServerData::AmbientFootsteps(bool newVal) ->void {
	boolVals.set( BIT_AMBIENTFOOTSTEPS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool InternalAccountStatus() const
//|					void InternalAccountStatus( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether automatic account creation is enabled
//o-----------------------------------------------------------------------------------------------o
auto CServerData::InternalAccountStatus() const ->bool {
	return boolVals.test( BIT_INTERNALACCOUNTS );
}
auto CServerData::InternalAccountStatus(bool newVal) ->void {
	boolVals.set( BIT_INTERNALACCOUNTS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ShowOfflinePCs() const
//|					void ShowOfflinePCs( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether GMs can see offline players
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ShowOfflinePCs() const ->bool{
	return boolVals.test( BIT_SHOWOFFLINEPCS );
}
auto CServerData::ShowOfflinePCs(bool newVal) ->void {
	boolVals.set( BIT_SHOWOFFLINEPCS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool RogueStatus() const
//|					void RogueStatus( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether stealing skill is enabled
//o-----------------------------------------------------------------------------------------------o
auto CServerData::RogueStatus() const ->bool{
	
	return boolVals.test( BIT_ROGUESTATUS );
}
auto CServerData::RogueStatus(bool newVal) ->void {
	boolVals.set( BIT_ROGUESTATUS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool SnoopIsCrime() const
//|					void SnoopIsCrime( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether snooping is considered a crime
//o-----------------------------------------------------------------------------------------------o
auto CServerData::SnoopIsCrime() const ->bool{
	return boolVals.test( BIT_SNOOPISCRIME );
}
auto CServerData::SnoopIsCrime(bool newVal) ->void {
	boolVals.set( BIT_SNOOPISCRIME, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool StatsAffectSkillChecks() const
//|					void StatsAffectSkillChecks( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether stats like strength, dexterity and intelligence provide
//|					bonuses to skillchecks
//o-----------------------------------------------------------------------------------------------o
auto CServerData::StatsAffectSkillChecks() const ->bool{
	
	return boolVals.test( BIT_STATSAFFECTSKILLCHECKS );
}
auto CServerData::StatsAffectSkillChecks(bool newVal) ->void {
	boolVals.set( BIT_STATSAFFECTSKILLCHECKS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ExtendedStartingStats() const
//|					void ExtendedStartingStats( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether extended starting stats are enabled
//|					If enabled players start with 90 total statpoints instead of 80
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ExtendedStartingStats() const ->bool{
	return boolVals.test( BIT_EXTENDEDSTARTINGSTATS );
}
auto CServerData::ExtendedStartingStats(bool newVal) ->void {
	boolVals.set( BIT_EXTENDEDSTARTINGSTATS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ExtendedStartingSkills() const
//|					void ExtendedStartingSkills( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether extended starting skills are enabled
//|					If enabled, players start with 4 skills instead of 3, and have a total of
//|					120 skillpoints at the start instead of 100
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ExtendedStartingSkills() const ->bool{
	return boolVals.test( BIT_EXTENDEDSTARTINGSKILLS );
}
auto CServerData::ExtendedStartingSkills(bool newVal) ->void {
	boolVals.set( BIT_EXTENDEDSTARTINGSKILLS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool PlayerPersecutionStatus() const
//|					void PlayerPersecutionStatus( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether player ghosts can drain mana from other players by attacking them
//o-----------------------------------------------------------------------------------------------o
auto CServerData::PlayerPersecutionStatus() const ->bool{
	return boolVals.test( BIT_PERSECUTIONSTATUS );
}
auto CServerData::PlayerPersecutionStatus(bool newVal) ->void {
	boolVals.set( BIT_PERSECUTIONSTATUS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 HtmlStatsStatus() const
//|					void HtmlStatsStatus( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether html stats are enabled or disabled
//o-----------------------------------------------------------------------------------------------o
auto CServerData::HtmlStatsStatus() const ->std::int16_t {
	return htmlstatusenabled;
}
auto CServerData::HtmlStatsStatus(SI16 value) ->void {
	htmlstatusenabled = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool SellByNameStatus() const
//|					void SellByNameStatus( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether items are sold by their name, not just ID/Colour
//o-----------------------------------------------------------------------------------------------o
auto CServerData::SellByNameStatus() const ->bool{
	
	return boolVals.test( BIT_SELLBYNAME );
}
auto CServerData::SellByNameStatus(bool newVal) ->void {
	boolVals.set( BIT_SELLBYNAME, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 SellMaxItemsStatus() const
//|					void SellMaxItemsStatus( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum amount of items that can be sold in one go to a vendor
//o-----------------------------------------------------------------------------------------------o
auto CServerData::SellMaxItemsStatus() const ->std::int16_t {
	return sellmaxitems;
}
auto CServerData::SellMaxItemsStatus(SI16 value) ->void {
	sellmaxitems = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool TradeSystemStatus() const
//|					void TradeSystemStatus( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether the advanced trade system is enabled or not.
//|					If enabled, prices of goods with NPC vendors will fluctuate with demand
//o-----------------------------------------------------------------------------------------------o
auto CServerData::TradeSystemStatus() const ->bool{
	return boolVals.test( BIT_TRADESYSSTATUS );
}
auto CServerData::TradeSystemStatus(bool newVal) ->void {
	boolVals.set( BIT_TRADESYSSTATUS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool RankSystemStatus() const
//|					void RankSystemStatus( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether rank system is enabled or not. If enabled, it allows for
//|					variable quality of crafted items
//o-----------------------------------------------------------------------------------------------o
auto CServerData::RankSystemStatus() const ->bool{
	return boolVals.test( BIT_RANKSYSSTATUS );
}
auto CServerData::RankSystemStatus(bool newVal) ->void {
	boolVals.set( BIT_RANKSYSSTATUS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CutScrollRequirementStatus() const
//|					void CutScrollRequirementStatus( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether skill requirements are cut when casting spells from scrolls
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CutScrollRequirementStatus() const ->bool{
	return boolVals.test( BIT_CUTSCROLLREQ );
}
auto CServerData::CutScrollRequirementStatus(bool newVal) ->void {
	boolVals.set( BIT_CUTSCROLLREQ, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CheckPetControlDifficulty() const
//|					void CheckPetControlDifficulty( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether pet control difficulty is enabled or not
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CheckPetControlDifficulty() const ->bool{
	
	return boolVals.test( BIT_CHECKPETCONTROLDIFFICULTY );
}
auto CServerData::CheckPetControlDifficulty(bool newVal) ->void {
	boolVals.set( BIT_CHECKPETCONTROLDIFFICULTY, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R64 CheckItemsSpeed() const
//|					void CheckItemsSpeed( R64 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets how often in seconds items are checked for decay and other things
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CheckItemsSpeed() const ->double {
	return checkitems;
}
auto CServerData::CheckItemsSpeed(R64 value) ->void {
	if( value < 0.0 )
		checkitems = 0.0;
	else
		checkitems = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R64 CheckBoatSpeed() const
//|					void CheckBoatSpeed( R64 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets how often in seconds boats are checked for decay and other things
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CheckBoatSpeed() const ->double {
	return checkboats;
}
auto CServerData::CheckBoatSpeed(R64 value) ->void {
	if( value < 0.0 )
		checkboats = 0.0;
	else
		checkboats = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R64 CheckNpcAISpeed() const
//|					void CheckNpcAISpeed( R64 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets how often in seconds NPCs will execute their AI routines
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CheckNpcAISpeed() const ->double {
	return checknpcai;
}
auto CServerData::CheckNpcAISpeed(R64 value) ->void {
	if( value < 0.0 )
		checknpcai = 0.0;
	else
		checknpcai = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R64 CheckSpawnRegionSpeed() const
//|					void CheckSpawnRegionSpeed( R64 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets how often (in seconds) spawn regions are checked for new spawns
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CheckSpawnRegionSpeed() const ->double {
	return checkspawnregions;
}
auto CServerData::CheckSpawnRegionSpeed(R64 value) ->void {
	if( value < 0.0 )
		checkspawnregions = 0.0;
	else
		checkspawnregions = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 MsgBoardPostingLevel() const
//|					void MsgBoardPostingLevel( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the posting level for bulletin boards
//|					UNUSED?
//o-----------------------------------------------------------------------------------------------o
auto CServerData::MsgBoardPostingLevel() const ->std::uint8_t {
	return msgpostinglevel;
}
auto CServerData::MsgBoardPostingLevel(UI08 value) ->void {
	msgpostinglevel = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 MsgBoardPostRemovalLevel() const
//|					void MsgBoardPostRemovalLevel( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the post removal level for bulleting boards
//|					UNUSED?
//o-----------------------------------------------------------------------------------------------o
auto CServerData::MsgBoardPostRemovalLevel() const ->std::uint8_t {
	return msgremovallevel;
}
auto CServerData::MsgBoardPostRemovalLevel(UI08 value) ->void {
	msgremovallevel = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 CServerData::MineCheck() const
//|					void CServerData::MineCheck( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the type of check used when players attempt to use the mining skill
//|						0 = mine anywhere
//|						1 = mine mountainsides/cavefloors only
//|						2 = mine in mining regions only (not working?)
//o-----------------------------------------------------------------------------------------------o
auto CServerData::MineCheck() const ->std::uint8_t {
	return minecheck;
}
auto CServerData::MineCheck(UI08 value) ->void {
	minecheck = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ConnectUOServerPoll() const
//|					void ConnectUOServerPoll( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether UOX3 will respond to server poll requests from ConnectUO
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ConnectUOServerPoll() const ->bool{
	return boolVals.test(BIT_CONNECTUOSERVERPOLL);
}
auto CServerData::ConnectUOServerPoll(bool newVal) ->void {
	boolVals.set( BIT_CONNECTUOSERVERPOLL, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CombatArmorClassDamageBonus() const
//|					void CombatArmorClassDamageBonus( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether weapons get a double damage bonus versus armors of matching AC
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CombatArmorClassDamageBonus() const ->bool{
	return boolVals.test( BIT_ARMORCLASSDAMAGEBONUS );
}
auto CServerData::CombatArmorClassDamageBonus(bool newVal) ->void {
	boolVals.set( BIT_ARMORCLASSDAMAGEBONUS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CombatDisplayHitMessage() const
//|					void CombatDisplayHitMessage( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether combat hit messages are displayed in combat
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CombatDisplayHitMessage() const ->bool{
	return boolVals.test( BIT_SHOWHITMESSAGE );
}
auto CServerData::CombatDisplayHitMessage(bool newVal) ->void {
	boolVals.set( BIT_SHOWHITMESSAGE, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CombatDisplayDamageNumbers() const
//|					void CombatDisplayDamageNumbers( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether combat damage numbers are displayed in combat
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CombatDisplayDamageNumbers() const ->bool{
	return boolVals.test( BIT_SHOWDAMAGENUMBERS );
}
auto CServerData::CombatDisplayDamageNumbers(bool newVal) ->void {
	boolVals.set( BIT_SHOWDAMAGENUMBERS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CombatAttackSpeedFromStamina() const
//|					void CombatAttackSpeedFromStamina( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether combat attack speed is derived from stamina instead of dexterity
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CombatAttackSpeedFromStamina() const ->bool{
	return boolVals.test( BIT_ATTSPEEDFROMSTAMINA );
}
auto CServerData::CombatAttackSpeedFromStamina(bool newVal) ->void {
	boolVals.set( BIT_ATTSPEEDFROMSTAMINA, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 GlobalAttackSpeed() const
//|					void GlobalAttackSpeed( R32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global attack speed in combat
//o-----------------------------------------------------------------------------------------------o
auto CServerData::GlobalAttackSpeed() const ->float {
	return globalattackspeed;
}
auto CServerData::GlobalAttackSpeed( R32 value ) ->void {
	if( value < 0.0 )
		globalattackspeed = 0.0;
	else
		globalattackspeed = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 NPCSpellCastSpeed() const
//|					void NPCSpellCastSpeed( R32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global NPC spell casting speed
//o-----------------------------------------------------------------------------------------------o
auto CServerData::NPCSpellCastSpeed() const ->float {
	return npcspellcastspeed;
}
auto CServerData::NPCSpellCastSpeed( R32 value ) ->void {
	if( value < 0.0 )
		npcspellcastspeed = 0.0;
	else
		npcspellcastspeed = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 FishingStaminaLoss() const
//|					void FishingStaminaLoss( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the stamina loss for using the fishing skill
//o-----------------------------------------------------------------------------------------------o
auto CServerData::FishingStaminaLoss() const ->std::int16_t {
	return fishingstaminaloss;
}
auto CServerData::FishingStaminaLoss( SI16 value ) ->void {
	fishingstaminaloss = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 CombatNPCDamageRate() const
//|					void CombatNPCDamageRate( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the NPC damage divisor. If character is a player, damage from NPCs is
//|					divided by this value.
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CombatNPCDamageRate() const ->std::int16_t {
	return combatnpcdamagerate;
}
auto CServerData::CombatNPCDamageRate( SI16 value ) ->void {
	combatnpcdamagerate = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool AlchemyDamageBonusEnabled() const
//|					void AlchemyDamageBonusEnabled( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether Alchemy Damage Bonus Modifier is enabled
//o-----------------------------------------------------------------------------------------------o
auto CServerData::AlchemyDamageBonusEnabled() const ->bool{
	return boolVals.test( BIT_ALCHEMYDAMAGEBONUSENABLED );
}
auto CServerData::AlchemyDamageBonusEnabled( bool newVal ) ->void {
	boolVals.set( BIT_ALCHEMYDAMAGEBONUSENABLED, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 AlchemyDamageBonusModifier() const
//|					void AlchemyDamageBonusModifier( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the Alchemy Damage Bonus Modifier, which gives bonus damage to 
//|					explosion potions based on this formula: 
//|						bonusDamage = attackerAlchemySkill / alchemyDamageBonusModifier
//o-----------------------------------------------------------------------------------------------o
auto CServerData::AlchemyDamageBonusModifier() const ->std::uint8_t {
	return alchemyDamageBonusModifier;
}
auto CServerData::AlchemyDamageBonusModifier( UI08 value ) ->void {
	alchemyDamageBonusModifier = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ItemsInterruptCasting() const
//|					void ItemsInterruptCasting( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether manipulation of items interrupts casting
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ItemsInterruptCasting() const ->bool{
	return boolVals.test( BIT_ITEMSINTERRUPTCASTING );
}
auto CServerData::ItemsInterruptCasting( bool newVal ) ->void {
	boolVals.set( BIT_ITEMSINTERRUPTCASTING, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 CombatAttackStamina() const
//|					void CombatAttackStamina( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the amount of stamina lost by swinging weapon in combat
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CombatAttackStamina() const ->std::int16_t {
	return combatattackstamina;
}
auto CServerData::CombatAttackStamina( SI16 value ) ->void {
	combatattackstamina = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 SkillLevel() const
//|					void SkillLevel( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global difficulty level for crafting items
//|					1 = easy, 5 = default, 10 = difficult
//o-----------------------------------------------------------------------------------------------o
auto CServerData::SkillLevel() const ->std::uint8_t {
	return skilllevel;
}
auto CServerData::SkillLevel( UI08 value ) ->void {
	skilllevel = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool EscortsEnabled() const
//|					void EscortsEnabled( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether escorts are enabled or not
//o-----------------------------------------------------------------------------------------------o
auto CServerData::EscortsEnabled() const ->bool{
	return boolVals.test( BIT_ESCORTSTATUS );
}
auto CServerData::EscortsEnabled( bool newVal ) ->void {
	boolVals.set( BIT_ESCORTSTATUS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ItemsDetectSpeech() const
//|					void ItemsDetectSpeech( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether items can trigger onSpeech JS event
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ItemsDetectSpeech() const ->bool{
	return boolVals.test( BIT_ITEMSDETECTSPEECH );
}
auto CServerData::ItemsDetectSpeech( bool newVal ) ->void {
	boolVals.set( BIT_ITEMSDETECTSPEECH, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ForceNewAnimationPacket() const
//|					void ForceNewAnimationPacket( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether server should force the use of new animation packet for NPCs
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ForceNewAnimationPacket() const ->bool{
	
	return boolVals.test( BIT_FORCENEWANIMATIONPACKET );
}
auto CServerData::ForceNewAnimationPacket( bool newVal ) ->void {
	boolVals.set( BIT_FORCENEWANIMATIONPACKET, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool MapDiffsEnabled() const
//|					void MapDiffsEnabled( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether server should load diff files for maps and statics
//|	Notes		-	Diff files are not used by client versions newer than 7.0.8.2
//o-----------------------------------------------------------------------------------------------o
auto CServerData::MapDiffsEnabled() const ->bool{
	return boolVals.test( BIT_MAPDIFFSENABLED );
}
auto CServerData::MapDiffsEnabled( bool newVal ) ->void {
	boolVals.set( BIT_MAPDIFFSENABLED, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 MaxPlayerPackItems() const
//|					void MaxPlayerPackItems( UI16 newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max item capacity for player (and NPC) backpacks
//o-----------------------------------------------------------------------------------------------o
auto CServerData::MaxPlayerPackItems() const ->std::uint16_t {
	return maxPlayerPackItems;
}
auto CServerData::MaxPlayerPackItems( UI16 newVal ) ->void {
	maxPlayerPackItems = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 MaxPlayerBankItems() const
//|					void MaxPlayerBankItems( UI16 newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max item capacity for player bankboxes
//o-----------------------------------------------------------------------------------------------o
auto CServerData::MaxPlayerBankItems() const ->std::uint16_t {
	return maxPlayerBankItems;
}
auto CServerData::MaxPlayerBankItems( UI16 newVal ) ->void {
	maxPlayerBankItems = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool BasicTooltipsOnly() const
//|					void BasicTooltipsOnly( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether basic tooltips are enabled or not (instead of advanced tooltips)
//o-----------------------------------------------------------------------------------------------o
auto CServerData::BasicTooltipsOnly() const ->bool{
	return boolVals.test( BIT_BASICTOOLTIPSONLY );
}
auto CServerData::BasicTooltipsOnly( bool newVal ) ->void {
	boolVals.set( BIT_BASICTOOLTIPSONLY, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ShowNpcTitlesInTooltips() const
//|					void ShowNpcTitlesInTooltips( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether NPC titles show up in tooltips
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ShowNpcTitlesInTooltips() const ->bool{
	return boolVals.test( BIT_SHOWNPCTITLESINTOOLTIPS );
}
auto CServerData::ShowNpcTitlesInTooltips( bool newVal ) ->void {
	boolVals.set( BIT_SHOWNPCTITLESINTOOLTIPS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GlobalItemDecay() const
//|					void GlobalItemDecay( bool newVal )
//|	Date		-	2/07/2010
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether decay is enabled or disabled, on a global scale
//o-----------------------------------------------------------------------------------------------o
auto CServerData::GlobalItemDecay() const ->bool{
	return boolVals.test( BIT_GLOBALITEMDECAY );
}
auto CServerData::GlobalItemDecay( bool newVal ) ->void {
	boolVals.set( BIT_GLOBALITEMDECAY, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ScriptItemsDecayable() const
//|					void ScriptItemsDecayable( bool newVal )
//|	Date		-	2/07/2010
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether items added through scripts decay or not
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ScriptItemsDecayable() const ->bool{
	return boolVals.test( BIT_SCRIPTITEMSDECAYABLE );
}
auto CServerData::ScriptItemsDecayable( bool newVal ) ->void {
	boolVals.set( BIT_SCRIPTITEMSDECAYABLE, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool BaseItemsDecayable() const
//|					void BaseItemsDecayable( bool newVal )
//|	Date		-	2/07/2010
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether base items added will decay or not
//o-----------------------------------------------------------------------------------------------o
auto CServerData::BaseItemsDecayable() const ->bool{
	return boolVals.test( BIT_BASEITEMSDECAYABLE );
}
auto CServerData::BaseItemsDecayable( bool newVal ) ->void {
	boolVals.set( BIT_BASEITEMSDECAYABLE, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ItemDecayInHouses() const
//|					void ItemDecayInHouses( bool newVal )
//|	Date		-	2/07/2010
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether items inside houses will decay or not
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ItemDecayInHouses() const ->bool{
	return boolVals.test( BIT_ITEMDECAYINHOUSES );
}
auto CServerData::ItemDecayInHouses( bool newVal ) ->void {
	boolVals.set( BIT_ITEMDECAYINHOUSES, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ProtectPrivateHouses() const
//|					void ProtectPrivateHouses( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether players without explicit access are prevented from entering private houses
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ProtectPrivateHouses() const ->bool{
	return boolVals.test( BIT_PROTECTPRIVATEHOUSES);
}
auto CServerData::ProtectPrivateHouses( bool newVal ) ->void {
	boolVals.set( BIT_PROTECTPRIVATEHOUSES, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool TrackHousesPerAccount() const
//|					void TrackHousesPerAccount( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether house ownership is tracked per account (true) or character (false)
//o-----------------------------------------------------------------------------------------------o
auto CServerData::TrackHousesPerAccount() const ->bool{
	return boolVals.test( BIT_TRACKHOUSESPERACCOUNT);
}
auto CServerData::TrackHousesPerAccount( bool newVal ) ->void {
	boolVals.set( BIT_TRACKHOUSESPERACCOUNT, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CanOwnAndCoOwnHouses() const
//|					void CanOwnAndCoOwnHouses( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether players can both own and co-own houses at the same time
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CanOwnAndCoOwnHouses() const ->bool{
	return boolVals.test( BIT_CANOWNANDCOOWNHOUSES);
}
auto CServerData::CanOwnAndCoOwnHouses( bool newVal ) ->void {
	boolVals.set( BIT_CANOWNANDCOOWNHOUSES, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CoOwnHousesOnSameAccount() const
//|					void CoOwnHousesOnSameAccount( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether characters on same account as house owner will be treated as
//|					if they are co-owners of the house
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CoOwnHousesOnSameAccount() const ->bool{
	return boolVals.test( BIT_COOWNHOUSESONSAMEACCOUNT);
}
auto CServerData::CoOwnHousesOnSameAccount( bool newVal ) ->void {
	boolVals.set( BIT_COOWNHOUSESONSAMEACCOUNT, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool PaperdollGuildButton() const
//|					void PaperdollGuildButton( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether guild menu can be accessed from paperdoll button or not
//o-----------------------------------------------------------------------------------------------o
auto CServerData::PaperdollGuildButton() const ->bool{
	return boolVals.test( BIT_PAPERDOLLGUILDBUTTON );
}
auto CServerData::PaperdollGuildButton( bool newVal ) ->void {
	boolVals.set( BIT_PAPERDOLLGUILDBUTTON, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CombatMonstersVsAnimals() const
//|					void CombatMonstersVsAnimals( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether monsters will attack animals or not
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CombatMonstersVsAnimals() const ->bool{
	return boolVals.test( BIT_MONSTERSVSANIMALS );
}
auto CServerData::CombatMonstersVsAnimals( bool newVal ) ->void {
	boolVals.set( BIT_MONSTERSVSANIMALS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 MaxHousesOwnable() const
//|					void MaxHousesOwnable( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max amount of houses that a player can own
//o-----------------------------------------------------------------------------------------------o
auto CServerData::MaxHousesOwnable() const ->std::uint16_t {
	return maxHousesOwnable;
}
auto CServerData::MaxHousesOwnable( UI16 value ) ->void {
	maxHousesOwnable = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 MaxHousesCoOwnable() const
//|					void MaxHousesCoOwnable( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max amount of houses that a player can co-own
//o-----------------------------------------------------------------------------------------------o
auto CServerData::MaxHousesCoOwnable() const ->std::uint16_t {
	return maxHousesCoOwnable;
}
auto CServerData::MaxHousesCoOwnable( UI16 value ) ->void {
	maxHousesCoOwnable = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 CombatAnimalsAttackChance() const
//|					void CombatAnimalsAttackChance( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the chance of monsters attacking animals
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CombatAnimalsAttackChance() const ->std::uint16_t {
	return combatanimalattackchance;
}
auto CServerData::CombatAnimalsAttackChance( UI16 value ) ->void {
	if( value > 1000 )
		value = 1000;
	combatanimalattackchance = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 CombatArcheryHitBonus() const
//|					void CombatArcheryHitBonus( SI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the delay hit bonus (in percentage) in combat for Archery. Can be negative!
//|					This bonus was mentioned in official patch notes for Publish 5 (UOR patch).
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CombatArcheryHitBonus() const ->std::int8_t {
	return combatArcheryHitBonus;
}
auto CServerData::CombatArcheryHitBonus( SI08 value ) ->void {
	if( value > 100 )
		value = 100;
	combatArcheryHitBonus = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 CombatArcheryShootDelay() const
//|					void CombatArcheryShootDelay( R32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the delay (in seconds, with decimals) after archers stop moving until
//|					they can fire a shot in combat
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CombatArcheryShootDelay() const ->float {
	return archeryShootDelay;
}
auto CServerData::CombatArcheryShootDelay( R32 value ) ->void {
	if( value < 0.0 )
		archeryShootDelay = 0.0;
	else
		archeryShootDelay = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 CombatWeaponDamageChance() const
//|					void CombatWeaponDamageChance( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets chance for weapons to take damage in combat (on hit)
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CombatWeaponDamageChance() const ->std::uint8_t {
	return combatweapondamagechance;
}
auto CServerData::CombatWeaponDamageChance( UI08 value ) ->void {
	if( value > 100 )
		value = 100;
	combatweapondamagechance = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 CombatWeaponDamageMin() const
//|					void CombatWeaponDamageMin( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the min amount of damage (in hitpoints) weapons will take from combat
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CombatWeaponDamageMin() const ->std::uint8_t {
	return combatweapondamagemin;
}
auto CServerData::CombatWeaponDamageMin( UI08 value ) ->void {
	combatweapondamagemin = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 CombatWeaponDamageMax() const
//|					void CombatWeaponDamageMax( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max amount of damage (in hitpoints) weapons will take from combat
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CombatWeaponDamageMax() const ->std::uint8_t {
	return combatweapondamagemax;
}
auto CServerData::CombatWeaponDamageMax( UI08 value ) ->void {
	combatweapondamagemax = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 CombatArmorDamageChance() const
//|					void CombatArmorDamageChance( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the chance to damage armor in combat
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CombatArmorDamageChance() const ->std::uint8_t {
	return combatarmordamagechance;
}
auto CServerData::CombatArmorDamageChance( UI08 value ) ->void {
	if( value > 100 )
		value = 100;
	combatarmordamagechance = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 CombatArmorDamageMin() const
//|					void CombatArmorDamageMin( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the min damage dealt to armor in combat
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CombatArmorDamageMin() const ->std::uint8_t {
	return combatarmordamagemin;
}
auto CServerData::CombatArmorDamageMin( UI08 value ) ->void {
	combatarmordamagemin = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 CombatArmorDamageMax() const
//|					void CombatArmorDamageMax( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max damage dealt to armor in combat
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CombatArmorDamageMax() const ->std::uint8_t {
	return combatarmordamagemax;
}
auto CServerData::CombatArmorDamageMax( UI08 value ) ->void {
	combatarmordamagemax = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 CombatParryDamageChance() const
//|					void CombatParryDamageChance( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the chance to damage items used to parry in combat
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CombatParryDamageChance() const ->std::uint8_t {
	return combatparrydamagechance;
}
auto CServerData::CombatParryDamageChance( UI08 value ) ->void {
	if( value > 100 )
		value = 100;
	combatparrydamagechance = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 CombatParryDamageMin() const
//|					void CombatParryDamageMin( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the min damage dealt to items used to parry in combat
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CombatParryDamageMin() const ->std::uint8_t {
	return combatparrydamagemin;
}
auto CServerData::CombatParryDamageMin( UI08 value ) ->void {
	combatparrydamagemin = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 CombatParryDamageMax() const
//|					void CombatParryDamageMax( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max damage dealt to items used to parry in combat
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CombatParryDamageMax() const ->std::uint8_t {
	return combatparrydamagemax;
}
auto CServerData::CombatParryDamageMax( UI08 value ) ->void {
	combatparrydamagemax = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 CombatBloodEffectChance() const
//|					void CombatBloodEffectChance( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the chance to spawn blood splatter effects during combat
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CombatBloodEffectChance() const ->std::uint8_t {
	return combatbloodeffectchance;
}
auto CServerData::CombatBloodEffectChance( UI08 value ) ->void {
	if( value > 100 )
		value = 100;
	combatbloodeffectchance = value;
}

//o-----------------------------------------------------------------------------------------------o
//| Function    -   bool HungerSystemEnabled() const
//|                 void HungerSystemEnabled( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets whether hunger system is enabled or disabled
//o-----------------------------------------------------------------------------------------------o
auto CServerData::HungerSystemEnabled() const ->bool{
	return boolVals.test( BIT_HUNGERSYSTEMENABLED );
}
auto CServerData::HungerSystemEnabled( bool newVal ) ->void {
	boolVals.set( BIT_HUNGERSYSTEMENABLED, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//| Function    -   bool ThirstSystemEnabled() const
//|                 void ThirstSystemEnabled( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets whether hunger system is enabled or disabled
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ThirstSystemEnabled() const ->bool{
	return boolVals.test( BIT_THIRSTSYSTEMENABLED );
}
auto CServerData::ThirstSystemEnabled( bool newVal ) ->void {
	boolVals.set( BIT_THIRSTSYSTEMENABLED, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 HungerDamage() const
//|					void HungerDamage( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the damage taken from players being hungry
//o-----------------------------------------------------------------------------------------------o
auto CServerData::HungerDamage() const ->std::int16_t {
	return hungerdamage;
}
auto CServerData::HungerDamage( SI16 value ) ->void {
	hungerdamage = value;
}

//o-----------------------------------------------------------------------------------------------o
//| Function    -   SI16 ThirstDrain() const
//|                 void ThirstDrain( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets the stamina drain from players being thirsty
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ThirstDrain() const ->std::int16_t {
	return thirstdrain;
}
auto CServerData::ThirstDrain( SI16 value ) ->void {
	thirstdrain = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 PetOfflineTimeout() const
//|					void PetOfflineTimeout( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the interval in seconds between checks for the player offline time
//o-----------------------------------------------------------------------------------------------o
auto CServerData::PetOfflineTimeout() const ->std::uint16_t {
	return petOfflineTimeout;
}
auto CServerData::PetOfflineTimeout( UI16 value ) ->void {
	petOfflineTimeout = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool PetHungerOffline() const
//|					void PetHungerOffline( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether pets should hunger while the player (owner) is offline or not
//o-----------------------------------------------------------------------------------------------o
auto CServerData::PetHungerOffline() const ->bool{
	return boolVals.test( BIT_PETHUNGEROFFLINE );
}
auto CServerData::PetHungerOffline( bool newVal ) ->void {
	boolVals.set( BIT_PETHUNGEROFFLINE, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//| Function    -   bool PetThirstOffline() const
//|                 void PetThirstOffline( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets whether pets should thirst while the player (owner) is offline or not
//o-----------------------------------------------------------------------------------------------o
auto CServerData::PetThirstOffline() const ->bool{
	return boolVals.test( BIT_PETTHIRSTOFFLINE );
}
auto CServerData::PetThirstOffline( bool newVal ) ->void {
	boolVals.set( BIT_PETTHIRSTOFFLINE, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 BuyThreshold() const
//|					void BuyThreshold( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the threshold for when money is taken from a player's bank account
//|					when buying something from a vendor instead of from their backpack
//o-----------------------------------------------------------------------------------------------o
auto CServerData::BuyThreshold() const ->std::int16_t {
	return buyThreshold;
}
auto CServerData::BuyThreshold( SI16 value ) ->void {
	buyThreshold = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CharHideWhileMounted() const
//|					void CharHideWhileMounted( bool newVal )
//|	Date		-	09/22/2002
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether players can hide while mounted or not
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CharHideWhileMounted() const ->bool{
	return boolVals.test( BIT_HIDEWHILEMOUNTED );
}
auto CServerData::CharHideWhileMounted( bool newVal ) ->void {
	boolVals.set( BIT_HIDEWHILEMOUNTED, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 WeightPerStr() const
//|					void WeightPerStr( R32 newVal )
//|	Date		-	3/12/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets amount of weight one can hold per point of STR
//o-----------------------------------------------------------------------------------------------o
auto CServerData::WeightPerStr() const ->float {
	return weightPerSTR;
}
auto CServerData::WeightPerStr( R32 newVal ) ->void {
	weightPerSTR = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ServerOverloadPackets() const
//|					void ServerOverloadPackets( bool newVal )
//|	Date		-	11/20/2005
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether packet handling in JS is enabled or disabled
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ServerOverloadPackets() const ->bool{
	
	return boolVals.test( BIT_OVERLOADPACKETS );
}
auto CServerData::ServerOverloadPackets( bool newVal ) ->void {
	boolVals.set( BIT_OVERLOADPACKETS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ArmorAffectManaRegen() const
//|					void ArmorAffectManaRegen( bool newVal )
//|	Date		-	3/20/2005
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether armor affects mana regeneration or not
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ArmorAffectManaRegen() const ->bool{
	return boolVals.test( BIT_ARMORAFFECTMANAREGEN );
}
auto CServerData::ArmorAffectManaRegen( bool newVal ) ->void {
	boolVals.set( BIT_ARMORAFFECTMANAREGEN, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool AdvancedPathfinding() const
//|					void AdvancedPathfinding( bool newVal )
//|	Date		-	7/16/2005
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether NPCs use the A* Pathfinding routine or not
//o-----------------------------------------------------------------------------------------------o
auto CServerData::AdvancedPathfinding() const ->bool{
	return boolVals.test( BIT_ADVANCEDPATHFIND );
}
auto CServerData::AdvancedPathfinding( bool newVal ) ->void {
	boolVals.set( BIT_ADVANCEDPATHFIND, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool LootingIsCrime() const
//|					void LootingIsCrime( bool newVal )
//|	Date		-	4/09/2007
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether looting of corpses can be a crime or not
//o-----------------------------------------------------------------------------------------------o
auto CServerData::LootingIsCrime() const ->bool{
	return boolVals.test( BIT_LOOTINGISCRIME );
}
auto CServerData::LootingIsCrime( bool newVal ) ->void {
	boolVals.set( BIT_LOOTINGISCRIME, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//| Function    -   bool TravelSpellsFromBoatKeys() const
//|                 void TravelSpellsFromBoatKeys( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets whether travel spells (recall, gate) are usable with boat keys to teleport
//|					directly to boat's location
//o-----------------------------------------------------------------------------------------------o
auto CServerData::TravelSpellsFromBoatKeys() const ->bool{
	
	return boolVals.test( BIT_TRAVELSPELLSFROMBOATKEYS );
}
auto CServerData::TravelSpellsFromBoatKeys( bool newVal ) ->void {
	boolVals.set( BIT_TRAVELSPELLSFROMBOATKEYS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//| Function    -   bool TravelSpellsWhileOverweight() const
//|                 void TravelSpellsWhileOverweight( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets whether travel spells (recall, gate) are allowed while overweight
//o-----------------------------------------------------------------------------------------------o
auto CServerData::TravelSpellsWhileOverweight() const ->bool{
	return boolVals.test( BIT_TRAVELSPELLSWHILEOVERWEIGHT );
}
auto CServerData::TravelSpellsWhileOverweight( bool newVal ) ->void {
	boolVals.set( BIT_TRAVELSPELLSWHILEOVERWEIGHT, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//| Function    -   bool MarkRunesInMultis() const
//|                 void MarkRunesInMultis( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets whether server allows marking recall runes in multis (houses, boats)
//o-----------------------------------------------------------------------------------------------o
auto CServerData::MarkRunesInMultis() const ->bool{
	return boolVals.test( BIT_MARKRUNESINMULTIS );
}
auto CServerData::MarkRunesInMultis( bool newVal ) ->void {
	boolVals.set( BIT_MARKRUNESINMULTIS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//| Function    -   bool TravelSpellsBetweenWorlds() const
//|                 void TravelSpellsBetweenWorlds( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets whether travel spells (recall, gate) are allowed between worlds
//o-----------------------------------------------------------------------------------------------o
auto CServerData::TravelSpellsBetweenWorlds() const ->bool{
	return boolVals.test( BIT_TRAVELSPELLSBETWEENWORLDS );
}
auto CServerData::TravelSpellsBetweenWorlds( bool newVal ) ->void {
	boolVals.set( BIT_TRAVELSPELLSBETWEENWORLDS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//| Function    -   bool TravelSpellsWhileAggressor() const
//|                 void TravelSpellsWhileAggressor( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets whether travel spells (recall, gate) are allowed while marked as an aggressor
//o-----------------------------------------------------------------------------------------------o
auto CServerData::TravelSpellsWhileAggressor() const ->bool{
	return boolVals.test( BIT_TRAVELSPELLSWHILEAGGRESSOR );
}
auto CServerData::TravelSpellsWhileAggressor( bool newVal ) ->void {
	boolVals.set( BIT_TRAVELSPELLSWHILEAGGRESSOR, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//| Function    -   UI08 MaxControlSlots() const
//|                 void MaxControlSlots( UI08 newVal )
//o-----------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets the max amount of control slots a player has available (0 for disable)
//o-----------------------------------------------------------------------------------------------o
auto CServerData::MaxControlSlots() const ->std::uint8_t {
	return maxControlSlots;
}
auto CServerData::MaxControlSlots( UI08 newVal ) ->void {
	maxControlSlots = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//| Function    -   UI08 MaxFollowers() const
//|                 void MaxFollowers( UI08 newVal )
//o-----------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets the max amount of active followers/pets a player can have (0 for disable)
//o-----------------------------------------------------------------------------------------------o
auto CServerData::MaxFollowers() const ->std::uint8_t {
	return maxFollowers;
}
auto CServerData::MaxFollowers( UI08 newVal ) ->void {
	maxFollowers = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//| Function    -   UI08 MaxPetOwners() const
//|                 void MaxPetOwners( UI08 newVal )
//o-----------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets the max amount of different owners a pet can have in its lifetime
//o-----------------------------------------------------------------------------------------------o
auto CServerData::MaxPetOwners() const ->std::uint8_t {
	return maxPetOwners;
}
auto CServerData::MaxPetOwners( UI08 newVal ) ->void {
	maxPetOwners = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//| Function    -   UI16 GetPetLoyaltyGainOnSuccess() const
//|                 void SetPetLoyaltyGainOnSuccess( UI16 newVal )
//o-----------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets the pet loyalty gained on successful use of pet command
//o-----------------------------------------------------------------------------------------------o
auto CServerData::GetPetLoyaltyGainOnSuccess() const ->std::uint16_t {
	return petLoyaltyGainOnSuccess;
}
auto CServerData::SetPetLoyaltyGainOnSuccess( UI16 newVal ) ->void {
	petLoyaltyGainOnSuccess = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//| Function    -   UI16 GetPetLoyaltyLossOnFailure() const
//|                 void SetPetLoyaltyLossOnFailure( UI16 newVal )
//o-----------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets the pet loyalty loss for failed use of pet command
//o-----------------------------------------------------------------------------------------------o
auto CServerData::GetPetLoyaltyLossOnFailure() const ->std::uint16_t {
	return petLoyaltyLossOnFailure;
}
auto CServerData::SetPetLoyaltyLossOnFailure( UI16 newVal ) ->void {
	petLoyaltyLossOnFailure = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//| Function    -   UI08 MaxSafeTeleportsPerDay() const
//|                 void MaxSafeTeleportsPerDay( UI08 newVal )
//o-----------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets the max amount of teleports to safety players get per day via help menu when stuck
//o-----------------------------------------------------------------------------------------------o
auto CServerData::MaxSafeTeleportsPerDay() const ->std::uint8_t {
	return maxSafeTeleports;
}
auto CServerData::MaxSafeTeleportsPerDay( UI08 newVal ) ->void {
	maxSafeTeleports = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool TeleportToNearestSafeLocation() const
//|					void TeleportToNearestSafeLocation( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether players who use the safe teleport via help menu will teleport
//|					to the nearest safe location, or to a random safe location anywhere on the map
//o-----------------------------------------------------------------------------------------------o
auto CServerData::TeleportToNearestSafeLocation() const ->bool{
	return boolVals.test( BIT_TELEPORTTONEARESTSAFELOC );
}
auto CServerData::TeleportToNearestSafeLocation( bool newVal ) ->void {
	boolVals.set( BIT_TELEPORTTONEARESTSAFELOC, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool AllowAwakeNPCs() const
//|					void AllowAwakeNPCs( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether the feature that allows permanently awake NPCs (ones marked
//|					as such) is enabled or not
//o-----------------------------------------------------------------------------------------------o
auto CServerData::AllowAwakeNPCs() const ->bool{
	return boolVals.test( BIT_ALLOWAWAKENPCS );
}
auto CServerData::AllowAwakeNPCs( bool newVal ) ->void {
	boolVals.set( BIT_ALLOWAWAKENPCS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ToolUseLimit() const
//|					void ToolUseLimit( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether tools have usage limits (based on item health)
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ToolUseLimit() const ->bool{
	return boolVals.test( BIT_TOOLUSELIMIT );
}
auto CServerData::ToolUseLimit( bool newVal ) ->void {
	boolVals.set( BIT_TOOLUSELIMIT, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ToolUseBreak() const
//|					void ToolUseBreak( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether tools will break if they reach 0 health
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ToolUseBreak() const ->bool{
	return boolVals.test( BIT_TOOLUSEBREAK );
}
auto CServerData::ToolUseBreak( bool newVal ) ->void {
	boolVals.set( BIT_TOOLUSEBREAK, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ItemRepairDurabilityLoss() const
//|					void ItemRepairDurabilityLoss( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether items will have their durability reduced when repaired
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ItemRepairDurabilityLoss() const ->bool{
	return boolVals.test( BIT_ITEMREPAIRDURABILITYLOSS );
}
auto CServerData::ItemRepairDurabilityLoss( bool newVal ) ->void {
	boolVals.set( BIT_ITEMREPAIRDURABILITYLOSS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool HideStatsForUnknownMagicItems() const
//|					void HideStatsForUnknownMagicItems( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether stats are displayed for unknown/unidentified magic items
//o-----------------------------------------------------------------------------------------------o
auto CServerData::HideStatsForUnknownMagicItems() const ->bool{
	return boolVals.test( BIT_HIDESTATSFORUNKNOWNMAGICITEMS );
}
auto CServerData::HideStatsForUnknownMagicItems( bool newVal ) ->void {
	boolVals.set( BIT_HIDESTATSFORUNKNOWNMAGICITEMS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CraftColouredWeapons() const
//|					void CraftColouredWeapons( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether players can craft weapons from coloured ore
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CraftColouredWeapons() const ->bool{
	return boolVals.test( BIT_CRAFTCOLOUREDWEAPONS );
}
auto CServerData::CraftColouredWeapons( bool newVal ) ->void {
	boolVals.set( BIT_CRAFTCOLOUREDWEAPONS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 BackupRatio() const
//|					void BackupRatio( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the ratio of worldsaves that get backed up
//o-----------------------------------------------------------------------------------------------o
auto CServerData::BackupRatio() const ->std::int16_t {
	return backupRatio;
}
auto CServerData::BackupRatio( SI16 value ) ->void {
	backupRatio = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 CombatMaxRange() const
//|					void CombatMaxRange( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum range at which combat can be engaged
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CombatMaxRange() const ->std::int16_t {
	return combatmaxrange;
}
auto CServerData::CombatMaxRange( SI16 value ) ->void {
	combatmaxrange = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 CombatMaxSpellRange() const
//|					void CombatMaxSpellRange( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum range at which spells can be cast in combat
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CombatMaxSpellRange() const ->std::int16_t {
	return combatmaxspellrange;
}
auto CServerData::CombatMaxSpellRange( SI16 value ) ->void {
	combatmaxspellrange = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CombatAnimalsGuarded() const
//|					void CombatAnimalsGuarded( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether animals are under the protection of town guards or not
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CombatAnimalsGuarded() const ->bool{
	return boolVals.test( BIT_ANIMALSGUARDED );
}
auto CServerData::CombatAnimalsGuarded( bool newVal ) ->void {
	boolVals.set( BIT_ANIMALSGUARDED, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 CombatNPCBaseFleeAt() const
//|					void CombatNPCBaseFleeAt( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global health threshold where NPCs start fleeing in combat
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CombatNPCBaseFleeAt() const ->std::int16_t {
	return combatnpcbasefleeat;
}
auto CServerData::CombatNPCBaseFleeAt( SI16 value ) ->void {
	combatnpcbasefleeat = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 CombatNPCBaseReattackAt() const
//|					void CombatNPCBaseReattackAt( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global health threshold where NPCs reattack after fleeing in combat
//o-----------------------------------------------------------------------------------------------o
auto CServerData::CombatNPCBaseReattackAt() const ->std::int16_t {
	return combatnpcbasereattackat;
}
auto CServerData::CombatNPCBaseReattackAt( SI16 value ) ->void {
	combatnpcbasereattackat = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 NPCWalkingSpeed() const
//|					void NPCWalkingSpeed( R32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global, default walking speed for NPCs
//o-----------------------------------------------------------------------------------------------o
auto CServerData::NPCWalkingSpeed() const ->float {
	return npcWalkingSpeed;
}
auto CServerData::NPCWalkingSpeed( R32 value ) ->void {
	npcWalkingSpeed = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 NPCRunningSpeed() const
//|					void NPCRunningSpeed( R32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global, default running speed for NPCs
//o-----------------------------------------------------------------------------------------------o
auto CServerData::NPCRunningSpeed() const ->float {
	return npcRunningSpeed;
}
auto CServerData::NPCRunningSpeed( R32 value ) ->void {
	npcRunningSpeed = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 CServerData::NPCFleeingSpeed() const
//|					void CServerData::NPCFleeingSpeed( R32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global, default speed at which NPCs flee in combat
//o-----------------------------------------------------------------------------------------------o
auto CServerData::NPCFleeingSpeed() const ->float {
	return npcFleeingSpeed;
}
auto CServerData::NPCFleeingSpeed( R32 value ) ->void {
	npcFleeingSpeed = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 NPCMountedWalkingSpeed() const
//|					void NPCMountedWalkingSpeed( R32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global, default walking speed for mounted NPCs
//o-----------------------------------------------------------------------------------------------o
auto CServerData::NPCMountedWalkingSpeed() const ->float {
	return npcMountedWalkingSpeed;
}
auto CServerData::NPCMountedWalkingSpeed( R32 value ) ->void {
	npcMountedWalkingSpeed = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 NPCMountedRunningSpeed() const
//|					void NPCMountedRunningSpeed( R32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global, default running speed for mounted NPCs
//o-----------------------------------------------------------------------------------------------o
auto CServerData::NPCMountedRunningSpeed() const ->float {
	return npcMountedRunningSpeed;
}
auto CServerData::NPCMountedRunningSpeed( R32 value ) ->void {
	npcMountedRunningSpeed = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 NPCMountedFleeingSpeed() const
//|					void NPCMountedFleeingSpeed( R32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global, default speed at which mounted NPCs flee in combat
//o-----------------------------------------------------------------------------------------------o
auto CServerData::NPCMountedFleeingSpeed() const ->float {

	return npcMountedFleeingSpeed;
}
auto CServerData::NPCMountedFleeingSpeed( R32 value ) ->void {
	npcMountedFleeingSpeed = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 TitleColour() const
//|					void TitleColour( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default text colour for titles in gumps
//o-----------------------------------------------------------------------------------------------o
auto CServerData::TitleColour() const ->std::uint16_t {
	return titleColour;
}
auto CServerData::TitleColour( UI16 value ) ->void {
	titleColour = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 LeftTextColour() const
//|					void LeftTextColour( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default text colour for left text in gumps (2 column ones)
//o-----------------------------------------------------------------------------------------------o
auto CServerData::LeftTextColour() const ->std::uint16_t {
	return leftTextColour;
}
auto CServerData::LeftTextColour( UI16 value ) ->void {
	leftTextColour = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 RightTextColour() const
//|					void RightTextColour( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default text colour for right text in gumps (2 column ones)
//o-----------------------------------------------------------------------------------------------o
auto CServerData::RightTextColour() const ->std::uint16_t {
	return rightTextColour;
}
auto CServerData::RightTextColour( UI16 value ) ->void {
	rightTextColour = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ButtonCancel() const
//|					void ButtonCancel( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default Button ID for cancel button in gumps
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ButtonCancel() const ->std::uint16_t {
	return buttonCancel;
}
auto CServerData::ButtonCancel( UI16 value ) ->void {
	buttonCancel = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ButtonLeft() const
//|					void ButtonLeft( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default Button ID for left button (navigation) in gumps
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ButtonLeft() const ->std::uint16_t {
	return buttonLeft;
}
auto CServerData::ButtonLeft( UI16 value ) ->void {
	buttonLeft = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ButtonRight() const
//|					void ButtonRight( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default Button ID for right button (navigation) in gumps
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ButtonRight() const ->std::uint16_t {
	return buttonRight;
}
auto CServerData::ButtonRight( UI16 value ) ->void {
	buttonRight = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 CServerData::BackgroundPic() const
//|					void CServerData::BackgroundPic( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default Gump ID for background gump
//o-----------------------------------------------------------------------------------------------o
auto CServerData::BackgroundPic() const ->std::uint16_t {
	return backgroundPic;
}
auto CServerData::BackgroundPic( UI16 value ) ->void {

	backgroundPic = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 TownNumSecsPollOpen() const
//|					void TownNumSecsPollOpen( UI32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the time (in seconds) for which a town voting poll is open
//o-----------------------------------------------------------------------------------------------o
auto CServerData::TownNumSecsPollOpen() const ->std::uint32_t {
	return numSecsPollOpen;
}
auto CServerData::TownNumSecsPollOpen( UI32 value ) ->void {
	numSecsPollOpen = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 TownNumSecsAsMayor() const
//|					void TownNumSecsAsMayor( UI32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the time (in seconds) that a PC would be a mayor
//o-----------------------------------------------------------------------------------------------o
auto CServerData::TownNumSecsAsMayor() const ->std::uint32_t {
	return numSecsAsMayor;
}
auto CServerData::TownNumSecsAsMayor( UI32 value ) ->void {
	numSecsAsMayor = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 TownTaxPeriod() const
//|					void TownTaxPeriod( UI32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the time (in seconds) between periods of taxes for PCs
//o-----------------------------------------------------------------------------------------------o
auto CServerData::TownTaxPeriod() const ->std::uint32_t {
	return taxPeriod;
}
auto CServerData::TownTaxPeriod( UI32 value ) ->void {
	taxPeriod = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 TownGuardPayment() const
//|					void TownGuardPayment( UI32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the time (in seconds) between payments for guards
//o-----------------------------------------------------------------------------------------------o
auto CServerData::TownGuardPayment() const ->std::uint32_t {
	return guardPayment;
}
auto CServerData::TownGuardPayment( UI32 value ) ->void {
	guardPayment = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 RepMaxKills() const
//|					void RepMaxKills( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the threshold in player kills before a player turns red (murderer)
//o-----------------------------------------------------------------------------------------------o
auto CServerData::RepMaxKills() const ->std::uint16_t {
	return maxmurdersallowed;
}
auto CServerData::RepMaxKills( UI16 value ) ->void {
	maxmurdersallowed = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 ResLogs() const
//|					void ResLogs( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum number of logs in a given resource area
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ResLogs() const ->std::int16_t {
	return logsperarea;
}
auto CServerData::ResLogs( SI16 value ) ->void {
	logsperarea = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ResLogTime() const
//|					void ResLogTime( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the time it takes for 1 single log to respawn in a resource area
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ResLogTime() const ->std::uint16_t {
	return logsrespawntimer;
}
auto CServerData::ResLogTime( UI16 value ) ->void {
	logsrespawntimer = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 ResOre() const
//|					void ResOre( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum number of ore in a given resource area
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ResOre() const ->std::int16_t {
	return oreperarea;
}
auto CServerData::ResOre( SI16 value ) ->void {
	oreperarea = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ResOreTime() const
//|					void ResOreTime( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the time it takes for 1 single ore to respawn in a resource area
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ResOreTime() const ->std::uint16_t {
	return orerespawntimer;
}
auto CServerData::ResOreTime( UI16 value ) ->void {
	orerespawntimer = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ResourceAreaSize() const
//|					void ResourceAreaSize( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the size of each resource area to split the world into (min 8x8)
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ResourceAreaSize() const ->std::uint16_t {
	return resourceAreaSize;
}
auto CServerData::ResourceAreaSize( UI16 value ) ->void {
	if( value < 8 )
		value = 8;
	resourceAreaSize = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 ResFish() const
//|					void ResFish( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum number of fish in a given resource area
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ResFish() const ->std::int16_t {
	return fishperarea;
}
auto CServerData::ResFish( SI16 value ) ->void {
	fishperarea = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ResFishTime() const
//|					void ResFishTime( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the time it takes for 1 single fish to respawn in a resource area
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ResFishTime() const ->std::uint16_t {
	return fishrespawntimer;
}
auto CServerData::ResFishTime( UI16 value ) ->void {
	fishrespawntimer = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R64 AccountFlushTimer() const
//|					void AccountFlushTimer( R64 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets how often (in mins) online accounts are checked to see if they really ARE online
//o-----------------------------------------------------------------------------------------------o
auto CServerData::AccountFlushTimer() const ->double {

	return flushTime;
}
auto CServerData::AccountFlushTimer( R64 value ) ->void {
	flushTime = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GetClientFeature( ClientFeatures bitNum ) const
//|					void SetClientFeature( ClientFeatures bitNum, bool nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets a specific client-side feature
//|	Notes		-	See ClientFeatures enum in cServerData.h for full list
//o-----------------------------------------------------------------------------------------------o
auto CServerData::GetClientFeature( ClientFeatures bitNum ) const ->bool{
	return clientFeatures.test( bitNum );
}
auto CServerData::SetClientFeature( ClientFeatures bitNum, bool nVal ) ->void {
	clientFeatures.set( bitNum, nVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 GetClientFeatures() const
//|					void SetClientFeatures( UI32 nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets which client side features to enable for connecting clients
//|	Notes		-	See ClientFeatures enum in cServerData.h for full list
//o-----------------------------------------------------------------------------------------------o
auto CServerData::GetClientFeatures() const ->std::uint32_t {
	return static_cast<UI32>(clientFeatures.to_ulong());
}
auto CServerData::SetClientFeatures( UI32 nVal ) ->void {
	clientFeatures = nVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GetServerFeature( ServerFeatures bitNum ) const
//|					void SetServerFeature( ServerFeatures bitNum, bool nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets a specific server-side feature
//|	Notes		-	See ServerFeatures enum in cServerData.h for full list
//o-----------------------------------------------------------------------------------------------o
auto CServerData::GetServerFeature( ServerFeatures bitNum ) const ->bool{
	return serverFeatures.test( bitNum );
}
auto CServerData::SetServerFeature( ServerFeatures bitNum, bool nVal ) ->void {

	serverFeatures.set( bitNum, nVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	size_t GetServerFeatures() const
//|					void SetServerFeatures( size_t nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets which server side features to enable
//|	Notes		-	See ServerFeatures enum in cServerData.h for full list
//o-----------------------------------------------------------------------------------------------o
auto CServerData::GetServerFeatures() const ->size_t {
	return serverFeatures.to_ulong();
}
//==============================================================================================
auto CServerData::SetServerFeatures( size_t nVal ) ->void {
	serverFeatures = nVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GetSpawnRegionsFacetStatus( UI32 value ) const
//|					UI32 GetSpawnRegionsFacetStatus() const
//|					void SetSpawnRegionsFacetStatus( UI32 nVal, bool status )
//|					void SetSpawnRegionsFacetStatus( UI32 nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets active status of spawn regions per facet
//o-----------------------------------------------------------------------------------------------o
auto CServerData::GetSpawnRegionsFacetStatus( UI32 value ) const ->bool{
	return spawnRegionsFacets.test( value );
}
//==============================================================================================
auto CServerData::SetSpawnRegionsFacetStatus( UI32 nVal, bool status ) ->void {
	spawnRegionsFacets.set( nVal, status );
}
//==============================================================================================
auto CServerData::GetSpawnRegionsFacetStatus() const ->std::uint32_t {
	return static_cast<UI32>(spawnRegionsFacets.to_ulong());
}
//==============================================================================================
auto CServerData::SetSpawnRegionsFacetStatus( UI32 nVal ) ->void {
	spawnRegionsFacets = nVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GetAssistantNegotiation() const
//|					void SetAssistantNegotiation( bool nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets status of feature negotiation with assist tools like Razor and AssistUO
//o-----------------------------------------------------------------------------------------------o
auto CServerData::GetAssistantNegotiation() const ->bool{
	return boolVals.test( BIT_ASSISTANTNEGOTIATION );
}
//==============================================================================================
auto CServerData::SetAssistantNegotiation( bool nVal ) ->void {
	boolVals.set( BIT_ASSISTANTNEGOTIATION, nVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GetClassicUOMapTracker() const
//|					void SetClassicUOMapTracker( bool nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether server should respond to ClassicUO's WorldMap Tracker packets
//o-----------------------------------------------------------------------------------------------o
auto CServerData::GetClassicUOMapTracker() const ->bool{
	return boolVals.test( BIT_CLASSICUOMAPTRACKER );
}
//==============================================================================================
auto CServerData::SetClassicUOMapTracker( bool nVal ) ->void {
	boolVals.set( BIT_CLASSICUOMAPTRACKER, nVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool UseUnicodeMessages() const
//|					void UseUnicodeMessages( bool nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether server should send messages originating on server as unicode
//o-----------------------------------------------------------------------------------------------o
auto CServerData::UseUnicodeMessages() const ->bool{
	return boolVals.test( BIT_USEUNICODEMESSAGES );
}
//==============================================================================================
auto CServerData::UseUnicodeMessages( bool nVal ) ->void {
	boolVals.set( BIT_USEUNICODEMESSAGES, nVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GetDisabledAssistantFeature( ClientFeatures bitNum ) const
//|					void SetDisabledAssistantFeature( ClientFeatures bitNum, bool nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets which assistant features to enable for connecting clients
//|	Notes		-	Example of assistant: Razor, AssistUO
//|					See ClientFeatures enum in cServerData.h for full list
//o-----------------------------------------------------------------------------------------------o
auto CServerData::GetDisabledAssistantFeature( AssistantFeatures bitNum ) const ->bool{
	
	return 0 != (CServerData::DisabledAssistantFeatures & bitNum);
}
//==============================================================================================
auto CServerData::SetDisabledAssistantFeature( AssistantFeatures bitNum, bool nVal ) ->void {
	if( nVal )
	{
		CServerData::DisabledAssistantFeatures |= bitNum;
	}
	else
		CServerData::DisabledAssistantFeatures &= ~bitNum;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI64 GetDisabledAssistantFeatures() const
//|					void SetDisabledAssistantFeatures( UI64 nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets which assistant features to enable for connecting clients
//|	Notes		-	Example of assistant: Razor, AssistUO
//|					See ClientFeatures enum in cServerData.h for full list
//o-----------------------------------------------------------------------------------------------o
auto CServerData::GetDisabledAssistantFeatures() const ->std::uint64_t{
	return CServerData::DisabledAssistantFeatures;
}
//==============================================================================================
auto CServerData::SetDisabledAssistantFeatures( UI64 nVal ) ->void {
	CServerData::DisabledAssistantFeatures = nVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool KickOnAssistantSilence() const
//|					void KickOnAssistantSilence( bool nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether player is kicked if there's no response from assistant tool
//|					to verify it complies with the allowed assistant features
//o-----------------------------------------------------------------------------------------------o
auto CServerData::KickOnAssistantSilence() const ->bool{
	return boolVals.test( BIT_KICKONASSISTANTSILENCE );
}
//==============================================================================================
auto CServerData::KickOnAssistantSilence( bool nVal ) ->void {
	boolVals.set( BIT_KICKONASSISTANTSILENCE, nVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CServerData::save()
//|	Date		-	02/21/2002
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Save the uox.ini out. This is the default save
//o-----------------------------------------------------------------------------------------------o
//|	Returns		- [true] If successfull
//o-----------------------------------------------------------------------------------------------o
auto CServerData::save() ->bool{
	auto s = actualINI ;
	if (s.empty()){
		s = Directory( CSDDP_ROOT );
		s += "uox.ini"s;
	}
	return save( s );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CServerData::save( std::string filename )
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
auto CServerData::save( const std::string &filename ) ->bool {
	auto rvalue = false;
	auto ofsOutput = std::ofstream(filename) ;
	if( ofsOutput.is_open() ) {
		
		ofsOutput << "// UOX Initialization File. V";
		ofsOutput << (static_cast<std::uint16_t>(1)<<8 | static_cast<std::uint16_t>(2)) << '\n' << "//================================" << '\n' << '\n';
		ofsOutput << "[system]" << '\n' << "{" << '\n';
		ofsOutput << "\tSERVERNAME=" << ServerName() << '\n';
		ofsOutput << "\tEXTERNALIP=" << ExternalIP() << '\n';
		ofsOutput << "\tPORT=" << ServerPort() << '\n';
		ofsOutput << "\tSERVERLANGUAGE=" << ServerLanguage() << '\n';
		ofsOutput << "\tNETRCVTIMEOUT=" << ServerNetRcvTimeout() << '\n';
		ofsOutput << "\tNETSNDTIMEOUT=" << ServerNetSndTimeout() << '\n';
		ofsOutput << "\tNETRETRYCOUNT=" << ServerNetRetryCount() << '\n';
		ofsOutput << "\tCONSOLELOG=" << ( ServerConsoleLog() ? 1 : 0 ) << '\n';
		ofsOutput << "\tNETWORKLOG=" << ( ServerNetworkLog() ? 1 : 0 ) << '\n';
		ofsOutput << "\tSPEECHLOG=" << ( ServerSpeechLog() ? 1 : 0 ) << '\n';
		ofsOutput << "\tCOMMANDPREFIX=" << ServerCommandPrefix() << '\n';
		ofsOutput << "\tANNOUNCEWORLDSAVES=" << (ServerAnnounceSavesStatus()?1:0) << '\n';
		ofsOutput << "\tJOINPARTMSGS=" << (ServerJoinPartAnnouncementsStatus()?1:0) << '\n';
		ofsOutput << "\tBACKUPSENABLED=" << (ServerBackupStatus()?1:0) << '\n';
		ofsOutput << "\tBACKUPSAVERATIO=" << BackupRatio() << '\n';
		ofsOutput << "\tSAVESTIMER=" << ServerSavesTimerStatus() << '\n';
		ofsOutput << "\tACCOUNTISOLATION=" << "1" << '\n';
		ofsOutput << "\tUOGENABLED=" << (ServerUOGEnabled()?1:0) << '\n';
		ofsOutput << "\tCUOENABLED=" << (ConnectUOServerPoll()?1:0) << '\n';
		ofsOutput << "\tRANDOMSTARTINGLOCATION=" << ( ServerRandomStartingLocation() ? 1 : 0 ) << '\n';
		ofsOutput << "\tASSISTANTNEGOTIATION=" << (GetAssistantNegotiation()?1:0) << '\n';
		ofsOutput << "\tKICKONASSISTANTSILENCE=" << (KickOnAssistantSilence()?1:0) << '\n';
		ofsOutput << "\tCLASSICUOMAPTRACKER=" << (GetClassicUOMapTracker()?1:0) << '\n';
		ofsOutput << "\tJSENGINESIZE=" << static_cast<UI16>(GetJSEngineSize()) << '\n';
		ofsOutput << "\tUSEUNICODEMESSAGES=" << (UseUnicodeMessages()?1:0) << '\n';
		ofsOutput << "\tCONTEXTMENUS=" << ( ServerContextMenus() ? 1 : 0 ) << '\n';
		ofsOutput << "\tSYSMESSAGECOLOUR=" << SysMsgColour() << '\n';
		ofsOutput << "\tMAXCLIENTBYTESIN=" << static_cast<UI32>(MaxClientBytesIn()) << '\n';
		ofsOutput << "\tMAXCLIENTBYTESOUT=" << static_cast<UI32>(MaxClientBytesOut()) << '\n';
		ofsOutput << "\tNETTRAFFICTIMEBAN=" << static_cast<UI32>(NetTrafficTimeban()) << '\n';
		ofsOutput << "}" << '\n' << '\n';
		
		ofsOutput << "[clientsupport]" << '\n' << "{" << '\n';
		ofsOutput << "\tCLIENTSUPPORT4000=" << (ClientSupport4000()?1:0) << '\n';
		ofsOutput << "\tCLIENTSUPPORT5000=" << (ClientSupport5000()?1:0) << '\n';
		ofsOutput << "\tCLIENTSUPPORT6000=" << (ClientSupport6000()?1:0) << '\n';
		ofsOutput << "\tCLIENTSUPPORT6050=" << (ClientSupport6050()?1:0) << '\n';
		ofsOutput << "\tCLIENTSUPPORT7000=" << (ClientSupport7000()?1:0) << '\n';
		ofsOutput << "\tCLIENTSUPPORT7090=" << (ClientSupport7090()?1:0) << '\n';
		ofsOutput << "\tCLIENTSUPPORT70160=" << (ClientSupport70160()?1:0) << '\n';
		ofsOutput << "\tCLIENTSUPPORT70240=" << (ClientSupport70240()?1:0) << '\n';
		ofsOutput << "\tCLIENTSUPPORT70300=" << (ClientSupport70300()?1:0) << '\n';
		ofsOutput << "\tCLIENTSUPPORT70331=" << (ClientSupport70331()?1:0) << '\n';
		ofsOutput << "\tCLIENTSUPPORT704565=" << (ClientSupport704565()?1:0) << '\n';
		ofsOutput << "\tCLIENTSUPPORT70610=" << (ClientSupport70610()?1:0) << '\n';
		ofsOutput << "}" << '\n';
		
		/*ofsOutput << '\n' << "[play server list]" << '\n' << "{" << '\n';
		 
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
		 ofsOutput << "}" << '\n';*/
		
		ofsOutput << '\n' << "[directories]" << '\n' << "{" << '\n';
		ofsOutput << "\tDIRECTORY=" << Directory( CSDDP_ROOT ) << '\n';
		ofsOutput << "\tDATADIRECTORY=" << Directory( CSDDP_DATA ) << '\n';
		ofsOutput << "\tDEFSDIRECTORY=" << Directory( CSDDP_DEFS ) << '\n';
		ofsOutput << "\tBOOKSDIRECTORY=" << Directory( CSDDP_BOOKS ) << '\n';
		ofsOutput << "\tACTSDIRECTORY=" << Directory( CSDDP_ACCOUNTS ) << '\n';
		ofsOutput << "\tSCRIPTSDIRECTORY=" << Directory( CSDDP_SCRIPTS ) << '\n';
		ofsOutput << "\tSCRIPTDATADIRECTORY=" << Directory( CSDDP_SCRIPTDATA ) << '\n';
		ofsOutput << "\tBACKUPDIRECTORY=" << Directory( CSDDP_BACKUP ) << '\n';
		ofsOutput << "\tMSGBOARDDIRECTORY=" << Directory( CSDDP_MSGBOARD ) << '\n';
		ofsOutput << "\tSHAREDDIRECTORY=" << Directory( CSDDP_SHARED ) << '\n';
		ofsOutput << "\tACCESSDIRECTORY=" << Directory( CSDDP_ACCESS ) << '\n';
		ofsOutput << "\tHTMLDIRECTORY=" << Directory( CSDDP_HTML ) << '\n';
		ofsOutput << "\tLOGSDIRECTORY=" << Directory( CSDDP_LOGS ) << '\n';
		ofsOutput << "\tDICTIONARYDIRECTORY=" << Directory( CSDDP_DICTIONARIES ) << '\n';
		ofsOutput << "}" << '\n';
		
		ofsOutput << '\n' << "[skill & stats]" << '\n' << "{" << '\n';
		ofsOutput << "\tSKILLLEVEL=" << static_cast<UI16>(SkillLevel()) << '\n';
		ofsOutput << "\tSKILLCAP=" << ServerSkillTotalCapStatus() << '\n';
		ofsOutput << "\tSKILLDELAY=" << static_cast<UI16>(ServerSkillDelayStatus()) << '\n';
		ofsOutput << "\tSTATCAP=" << ServerStatCapStatus() << '\n';
		ofsOutput << "\tSTATSAFFECTSKILLCHECKS=" << (StatsAffectSkillChecks()?1:0) << '\n';
		ofsOutput << "\tEXTENDEDSTARTINGSTATS=" << (ExtendedStartingStats()?1:0) << '\n';
		ofsOutput << "\tEXTENDEDSTARTINGSKILLS=" << (ExtendedStartingSkills()?1:0) << '\n';
		ofsOutput << "\tMAXSTEALTHMOVEMENTS=" << MaxStealthMovement() << '\n';
		ofsOutput << "\tMAXSTAMINAMOVEMENTS=" << MaxStaminaMovement() << '\n';
		ofsOutput << "\tSNOOPISCRIME=" << (SnoopIsCrime()?1:0) << '\n';
		ofsOutput << "\tARMORAFFECTMANAREGEN=" << (ArmorAffectManaRegen() ? 1 : 0) << '\n';
		ofsOutput << "}" << '\n';
		
		ofsOutput << '\n' << "[timers]" << '\n' << "{" << '\n';
		ofsOutput << "\tCORPSEDECAYTIMER=" << SystemTimer( tSERVER_CORPSEDECAY ) << '\n';
		ofsOutput << "\tNPCCORPSEDECAYTIMER=" << SystemTimer( tSERVER_NPCCORPSEDECAY ) << '\n';
		ofsOutput << "\tWEATHERTIMER=" << SystemTimer( tSERVER_WEATHER ) << '\n';
		ofsOutput << "\tSHOPSPAWNTIMER=" << SystemTimer( tSERVER_SHOPSPAWN ) << '\n';
		ofsOutput << "\tDECAYTIMER=" << SystemTimer( tSERVER_DECAY ) << '\n';
		ofsOutput << "\tDECAYTIMERINHOUSE=" << SystemTimer( tSERVER_DECAYINHOUSE ) << '\n';
		ofsOutput << "\tINVISIBILITYTIMER=" << SystemTimer( tSERVER_INVISIBILITY ) << '\n';
		ofsOutput << "\tOBJECTUSETIMER=" << SystemTimer( tSERVER_OBJECTUSAGE ) << '\n';
		ofsOutput << "\tGATETIMER=" << SystemTimer( tSERVER_GATE ) << '\n';
		ofsOutput << "\tPOISONTIMER=" << SystemTimer( tSERVER_POISON ) << '\n';
		ofsOutput << "\tLOGINTIMEOUT=" << SystemTimer( tSERVER_LOGINTIMEOUT ) << '\n';
		ofsOutput << "\tHITPOINTREGENTIMER=" << SystemTimer( tSERVER_HITPOINTREGEN ) << '\n';
		ofsOutput << "\tSTAMINAREGENTIMER=" << SystemTimer( tSERVER_STAMINAREGEN ) << '\n';
		ofsOutput << "\tMANAREGENTIMER=" << SystemTimer( tSERVER_MANAREGEN ) << '\n';
		ofsOutput << "\tBASEFISHINGTIMER=" << SystemTimer( tSERVER_FISHINGBASE ) << '\n';
		ofsOutput << "\tRANDOMFISHINGTIMER=" << SystemTimer( tSERVER_FISHINGRANDOM ) << '\n';
		ofsOutput << "\tSPIRITSPEAKTIMER=" << SystemTimer( tSERVER_SPIRITSPEAK ) << '\n';
		ofsOutput << "\tPETOFFLINECHECKTIMER=" << SystemTimer( tSERVER_PETOFFLINECHECK ) << '\n';
		ofsOutput << "\tNPCFLAGUPDATETIMER=" << SystemTimer( tSERVER_NPCFLAGUPDATETIMER ) << '\n';
		ofsOutput << "\tBLOODDECAYTIMER=" << SystemTimer( tSERVER_BLOODDECAY ) << '\n';
		ofsOutput << "\tBLOODDECAYCORPSETIMER=" << SystemTimer( tSERVER_BLOODDECAYCORPSE ) << '\n';
		ofsOutput << "}" << '\n';
		
		ofsOutput << '\n' << "[settings]" << '\n' << "{" << '\n';
		ofsOutput << "\tLOOTDECAYSWITHCORPSE=" << (CorpseLootDecay()?1:0) << '\n';
		ofsOutput << "\tGUARDSACTIVE=" << (GuardsStatus()?1:0) << '\n';
		ofsOutput << "\tDEATHANIMATION=" << (DeathAnimationStatus()?1:0) << '\n';
		ofsOutput << "\tAMBIENTSOUNDS=" << WorldAmbientSounds() << '\n';
		ofsOutput << "\tAMBIENTFOOTSTEPS=" << (AmbientFootsteps()?1:0) << '\n';
		ofsOutput << "\tINTERNALACCOUNTCREATION=" << (InternalAccountStatus()?1:0) << '\n';
		ofsOutput << "\tSHOWOFFLINEPCS=" << (ShowOfflinePCs()?1:0) << '\n';
		ofsOutput << "\tROGUESENABLED=" << (RogueStatus()?1:0) << '\n';
		ofsOutput << "\tPLAYERPERSECUTION=" << (PlayerPersecutionStatus()?1:0) << '\n';
		ofsOutput << "\tACCOUNTFLUSH=" << AccountFlushTimer() << '\n';
		ofsOutput << "\tHTMLSTATUSENABLED=" << HtmlStatsStatus() << '\n';
		ofsOutput << "\tSELLBYNAME=" << (SellByNameStatus()?1:0) << '\n';
		ofsOutput << "\tSELLMAXITEMS=" << SellMaxItemsStatus() << '\n';
		ofsOutput << "\tBANKBUYTHRESHOLD=" << BuyThreshold() << '\n';
		ofsOutput << "\tTRADESYSTEM=" << (TradeSystemStatus()?1:0) << '\n';
		ofsOutput << "\tRANKSYSTEM=" << (RankSystemStatus()?1:0) << '\n';
		ofsOutput << "\tCUTSCROLLREQUIREMENTS=" << (CutScrollRequirementStatus()?1:0) << '\n';
		ofsOutput << "\tNPCTRAININGENABLED=" << (NPCTrainingStatus()?1:0) << '\n';
		ofsOutput << "\tHIDEWHILEMOUNTED=" << (CharHideWhileMounted()?1:0) << '\n';
		//ofsOutput << "WEIGHTPERSTR=" << static_cast<UI16>(WeightPerStr()) << '\n';
		ofsOutput << "\tWEIGHTPERSTR=" << static_cast<R32>(WeightPerStr()) << '\n';
		ofsOutput << "\tPOLYDURATION=" << SystemTimer( tSERVER_POLYMORPH ) << '\n';
		ofsOutput << "\tCLIENTFEATURES=" << GetClientFeatures() << '\n';
		ofsOutput << "\tSERVERFEATURES=" << GetServerFeatures() << '\n';
		ofsOutput << "\tSPAWNREGIONSFACETS=" << GetSpawnRegionsFacetStatus() << '\n';
		ofsOutput << "\tOVERLOADPACKETS=" << (ServerOverloadPackets()?1:0) << '\n';
		ofsOutput << "\tADVANCEDPATHFINDING=" << (AdvancedPathfinding()?1:0) << '\n';
		ofsOutput << "\tLOOTINGISCRIME=" << (LootingIsCrime()?1:0) << '\n';
		ofsOutput << "\tBASICTOOLTIPSONLY=" << (BasicTooltipsOnly()?1:0) << '\n';
		ofsOutput << "\tSHOWNPCTITLESINTOOLTIPS=" << (ShowNpcTitlesInTooltips()?1:0) << '\n';
		ofsOutput << "\tGLOBALITEMDECAY=" << (GlobalItemDecay()?1:0) << '\n';
		ofsOutput << "\tSCRIPTITEMSDECAYABLE=" << (ScriptItemsDecayable()?1:0) << '\n';
		ofsOutput << "\tBASEITEMSDECAYABLE=" << (BaseItemsDecayable()?1:0) << '\n';
		ofsOutput << "\tPAPERDOLLGUILDBUTTON=" << (PaperdollGuildButton()?1:0) << '\n';
		ofsOutput << "\tFISHINGSTAMINALOSS=" << FishingStaminaLoss() << '\n';
		ofsOutput << "\tITEMSDETECTSPEECH=" << ItemsDetectSpeech() << '\n';
		ofsOutput << "\tMAXPLAYERPACKITEMS=" << MaxPlayerPackItems() << '\n';
		ofsOutput << "\tMAXPLAYERBANKITEMS=" << MaxPlayerBankItems() << '\n';
		ofsOutput << "\tFORCENEWANIMATIONPACKET=" << ForceNewAnimationPacket() << '\n';
		ofsOutput << "\tMAPDIFFSENABLED=" << MapDiffsEnabled() << '\n';
		ofsOutput << "\tTOOLUSELIMIT=" << ToolUseLimit() << '\n';
		ofsOutput << "\tTOOLUSEBREAK=" << ToolUseBreak() << '\n';
		ofsOutput << "\tITEMREPAIRDURABILITYLOSS=" << ItemRepairDurabilityLoss() << '\n';
		ofsOutput << "\tCRAFTCOLOUREDWEAPONS=" << CraftColouredWeapons() << '\n';
		ofsOutput << "\tMAXSAFETELEPORTSPERDAY=" << static_cast<UI16>(MaxSafeTeleportsPerDay()) << '\n';
		ofsOutput << "\tTELEPORTTONEARESTSAFELOCATION=" << (TeleportToNearestSafeLocation()?1:0) << '\n';
		ofsOutput << "\tALLOWAWAKENPCS=" << (AllowAwakeNPCs()?1:0) << '\n';
		ofsOutput << "}" << '\n';
		
		ofsOutput << '\n' << "[pets and followers]" << '\n' << "{" << '\n';
		ofsOutput << "\tMAXCONTROLSLOTS=" << static_cast<UI16>(MaxControlSlots()) << '\n';
		ofsOutput << "\tMAXFOLLOWERS=" << static_cast<UI16>(MaxFollowers()) << '\n';
		ofsOutput << "\tMAXPETOWNERS=" << static_cast<UI16>(MaxPetOwners()) << '\n';
		ofsOutput << "\tCHECKPETCONTROLDIFFICULTY=" << ( CheckPetControlDifficulty() ? 1 : 0 ) << '\n';
		ofsOutput << "\tPETLOYALTYGAINONSUCCESS=" << static_cast<UI16>( GetPetLoyaltyGainOnSuccess() ) << '\n';
		ofsOutput << "\tPETLOYALTYLOSSONFAILURE=" << static_cast<UI16>( GetPetLoyaltyLossOnFailure() ) << '\n';
		ofsOutput << "\tPETLOYALTYRATE=" << SystemTimer( tSERVER_LOYALTYRATE ) << '\n';
		ofsOutput << "}" << '\n';
		
		ofsOutput << '\n' << "[speedup]" << '\n' << "{" << '\n';
		ofsOutput << "\tCHECKITEMS=" << CheckItemsSpeed() << '\n';
		ofsOutput << "\tCHECKBOATS=" << CheckBoatSpeed() << '\n';
		ofsOutput << "\tCHECKNPCAI=" << CheckNpcAISpeed() << '\n';
		ofsOutput << "\tCHECKSPAWNREGIONS=" << CheckSpawnRegionSpeed() << '\n';
		ofsOutput << "\tNPCMOVEMENTSPEED=" << NPCWalkingSpeed() << '\n';
		ofsOutput << "\tNPCRUNNINGSPEED=" << NPCRunningSpeed() << '\n';
		ofsOutput << "\tNPCFLEEINGSPEED=" << NPCFleeingSpeed() << '\n';
		ofsOutput << "\tNPCMOUNTEDWALKINGSPEED=" << NPCMountedWalkingSpeed() << '\n';
		ofsOutput << "\tNPCMOUNTEDRUNNINGSPEED=" << NPCMountedRunningSpeed() << '\n';
		ofsOutput << "\tNPCMOUNTEDFLEEINGSPEED=" << NPCMountedFleeingSpeed() << '\n';
		ofsOutput << "\tNPCSPELLCASTSPEED=" << NPCSpellCastSpeed() << '\n';
		ofsOutput << "\tGLOBALATTACKSPEED=" << GlobalAttackSpeed() << '\n';
		ofsOutput << "}" << '\n';
		
		ofsOutput << '\n' << "[message boards]" << '\n' << "{" << '\n';
		ofsOutput << "\tPOSTINGLEVEL=" << static_cast<UI16>(MsgBoardPostingLevel()) << '\n';
		ofsOutput << "\tREMOVALLEVEL=" << static_cast<UI16>(MsgBoardPostRemovalLevel()) << '\n';
		ofsOutput << "}" << '\n';
		
		ofsOutput << '\n' << "[escorts]" << '\n' << "{" << '\n';
		ofsOutput << "\tESCORTENABLED=" << ( EscortsEnabled() ? 1 : 0 ) << '\n';
		ofsOutput << "\tESCORTINITEXPIRE=" << SystemTimer( tSERVER_ESCORTWAIT ) << '\n';
		ofsOutput << "\tESCORTACTIVEEXPIRE=" << SystemTimer( tSERVER_ESCORTACTIVE ) << '\n';
		ofsOutput << "\tESCORTDONEEXPIRE=" << SystemTimer( tSERVER_ESCORTDONE ) << '\n';
		ofsOutput << "}" << '\n';
		
		ofsOutput << '\n' << "[worldlight]" << '\n' << "{" << '\n';
		ofsOutput << "\tDUNGEONLEVEL=" << static_cast<UI16>(DungeonLightLevel()) << '\n';
		ofsOutput << "\tBRIGHTLEVEL=" << static_cast<UI16>(WorldLightBrightLevel()) << '\n';
		ofsOutput << "\tDARKLEVEL=" << static_cast<UI16>(WorldLightDarkLevel()) << '\n';
		ofsOutput << "\tSECONDSPERUOMINUTE=" << ServerSecondsPerUOMinute() << '\n';
		ofsOutput << "}" << '\n';
		
		ofsOutput << '\n' << "[tracking]" << '\n' << "{" << '\n';
		ofsOutput << "\tBASERANGE=" << TrackingBaseRange() << '\n';
		ofsOutput << "\tBASETIMER=" << TrackingBaseTimer() << '\n';
		ofsOutput << "\tMAXTARGETS=" << static_cast<UI16>(TrackingMaxTargets()) << '\n';
		ofsOutput << "\tMSGREDISPLAYTIME=" << TrackingRedisplayTime() << '\n';
		ofsOutput << "}" << '\n';
		
		ofsOutput << '\n' << "[reputation]" << '\n' << "{" << '\n';
		ofsOutput << "\tMURDERDECAYTIMER=" << SystemTimer( tSERVER_MURDERDECAY ) << '\n';
		ofsOutput << "\tMAXKILLS=" << RepMaxKills() << '\n';
		ofsOutput << "\tCRIMINALTIMER=" << SystemTimer( tSERVER_CRIMINAL ) << '\n';
		ofsOutput << "}" << '\n';
		
		ofsOutput << '\n' << "[resources]" << '\n' << "{" << '\n';
		ofsOutput << "\tRESOURCEAREASIZE=" << ResourceAreaSize() << '\n';
		ofsOutput << "\tMINECHECK=" << static_cast<UI16>(MineCheck()) << '\n';
		ofsOutput << "\tOREPERAREA=" << ResOre() << '\n';
		ofsOutput << "\tORERESPAWNTIMER=" << ResOreTime() << '\n';
		ofsOutput << "\tLOGSPERAREA=" << ResLogs() << '\n';
		ofsOutput << "\tLOGSRESPAWNTIMER=" << ResLogTime() << '\n';
		ofsOutput << "\tFISHPERAREA=" << ResFish() << '\n';
		ofsOutput << "\tFISHRESPAWNTIMER=" << ResFishTime() << '\n';
		ofsOutput << "}" << '\n';
		
		ofsOutput << '\n' << "[hunger]" << '\n' << "{" << '\n';
		ofsOutput << "\tHUNGERENABLED=" << (HungerSystemEnabled()?1:0) << '\n';
		ofsOutput << "\tHUNGERRATE=" << SystemTimer( tSERVER_HUNGERRATE ) << '\n';
		ofsOutput << "\tHUNGERDMGVAL=" << HungerDamage() << '\n';
		ofsOutput << "\tPETHUNGEROFFLINE=" << (PetHungerOffline()?1:0) << '\n';
		ofsOutput << "\tPETOFFLINETIMEOUT=" << PetOfflineTimeout() << '\n';
		ofsOutput << "}" << '\n';
		
		ofsOutput << '\n' << "[thirst]" << '\n' << "{" << '\n';
		ofsOutput << "\tTHIRSTENABLED=" << ( ThirstSystemEnabled() ? 1 : 0 ) << '\n';
		ofsOutput << "\tTHIRSTRATE=" << SystemTimer( tSERVER_THIRSTRATE ) << '\n';
		ofsOutput << "\tTHIRSTDRAINVAL=" << ThirstDrain() << '\n';
		ofsOutput << "\tPETTHIRSTOFFLINE=" << (PetThirstOffline()?1:0) << '\n';
		ofsOutput << "}" << '\n';
		
		ofsOutput << '\n' << "[combat]" << '\n' << "{" << '\n';
		ofsOutput << "\tMAXRANGE=" << CombatMaxRange() << '\n';
		ofsOutput << "\tSPELLMAXRANGE=" << CombatMaxSpellRange() << '\n';
		ofsOutput << "\tDISPLAYHITMSG=" << (CombatDisplayHitMessage()?1:0) << '\n';
		ofsOutput << "\tDISPLAYDAMAGENUMBERS=" << (CombatDisplayDamageNumbers()?1:0) << '\n';
		ofsOutput << "\tMONSTERSVSANIMALS=" << (CombatMonstersVsAnimals()?1:0) << '\n';
		ofsOutput << "\tANIMALATTACKCHANCE=" << static_cast<UI16>(CombatAnimalsAttackChance()) << '\n';
		ofsOutput << "\tANIMALSGUARDED=" << (CombatAnimalsGuarded()?1:0) << '\n';
		ofsOutput << "\tNPCDAMAGERATE=" << CombatNPCDamageRate() << '\n';
		ofsOutput << "\tNPCBASEFLEEAT=" << CombatNPCBaseFleeAt() << '\n';
		ofsOutput << "\tNPCBASEREATTACKAT=" << CombatNPCBaseReattackAt() << '\n';
		ofsOutput << "\tATTACKSTAMINA=" << CombatAttackStamina() << '\n';
		ofsOutput << "\tATTACKSPEEDFROMSTAMINA=" << (CombatAttackSpeedFromStamina()?1:0) << '\n';
		ofsOutput << "\tARCHERYHITBONUS=" << static_cast<SI16>(CombatArcheryHitBonus()) << '\n';
		ofsOutput << "\tARCHERYSHOOTDELAY=" << CombatArcheryShootDelay() << '\n';
		ofsOutput << "\tSHOOTONANIMALBACK=" << (ShootOnAnimalBack()?1:0) << '\n';
		ofsOutput << "\tWEAPONDAMAGECHANCE=" << static_cast<UI16>(CombatWeaponDamageChance()) << '\n';
		ofsOutput << "\tWEAPONDAMAGEMIN=" << static_cast<UI16>(CombatWeaponDamageMin()) << '\n';
		ofsOutput << "\tWEAPONDAMAGEMAX=" << static_cast<UI16>(CombatWeaponDamageMax()) << '\n';
		ofsOutput << "\tARMORDAMAGECHANCE=" << static_cast<UI16>(CombatArmorDamageChance()) << '\n';
		ofsOutput << "\tARMORDAMAGEMIN=" << static_cast<UI16>(CombatArmorDamageMin()) << '\n';
		ofsOutput << "\tARMORDAMAGEMAX=" << static_cast<UI16>(CombatArmorDamageMax()) << '\n';
		ofsOutput << "\tPARRYDAMAGECHANCE=" << static_cast<UI16>(CombatParryDamageChance()) << '\n';
		ofsOutput << "\tPARRYDAMAGEMIN=" << static_cast<UI16>(CombatParryDamageMin()) << '\n';
		ofsOutput << "\tPARRYDAMAGEMAX=" << static_cast<UI16>(CombatParryDamageMax()) << '\n';
		ofsOutput << "\tARMORCLASSDAMAGEBONUS=" << (CombatArmorClassDamageBonus()?1:0) << '\n';
		ofsOutput << "\tALCHEMYBONUSENABLED=" << (AlchemyDamageBonusEnabled()?1:0) << '\n';
		ofsOutput << "\tALCHEMYBONUSMODIFIER=" << static_cast<UI16>(AlchemyDamageBonusModifier()) << '\n';
		ofsOutput << "\tBLOODEFFECTCHANCE=" << static_cast<UI16>( CombatBloodEffectChance() ) << '\n';
		ofsOutput << "\tITEMSINTERRUPTCASTING=" << (ItemsInterruptCasting()?1:0) << '\n';
		ofsOutput << "}" << '\n';
		
		ofsOutput << '\n' << "[magic]" << '\n' << "{" << '\n';
		ofsOutput << "\tTRAVELSPELLSFROMBOATKEYS=" << ( TravelSpellsFromBoatKeys() ? 1 : 0 ) << '\n';
		ofsOutput << "\tTRAVELSPELLSWHILEOVERWEIGHT=" << ( TravelSpellsWhileOverweight() ? 1 : 0 ) << '\n';
		ofsOutput << "\tMARKRUNESINMULTIS=" << ( MarkRunesInMultis() ? 1 : 0 ) << '\n';
		ofsOutput << "\tTRAVELSPELLSBETWEENWORLDS=" << ( TravelSpellsBetweenWorlds() ? 1 : 0 ) << '\n';
		ofsOutput << "\tTRAVELSPELLSWHILEAGGRESSOR=" << ( TravelSpellsWhileAggressor() ? 1 : 0 ) << '\n';
		ofsOutput << "\tHIDESTATSFORUNKNOWNMAGICITEMS=" << HideStatsForUnknownMagicItems() << '\n';
		ofsOutput << "}" << '\n';
		
		ofsOutput << '\n' << "[start locations]" << '\n' << "{" << '\n';
		for( size_t lCtr = 0; lCtr < startlocations.size(); ++lCtr )
			ofsOutput << "\tLOCATION=" << startlocations[lCtr].newTown << "," << startlocations[lCtr].newDescription << "," << startlocations[lCtr].x << "," << startlocations[lCtr].y << "," << startlocations[lCtr].z << "," << startlocations[lCtr].worldNum << "," << startlocations[lCtr].instanceID << "," << startlocations[lCtr].clilocDesc << '\n';
		ofsOutput << "}" << '\n';
		
		ofsOutput << '\n' << "[startup]" << '\n' << "{" << '\n';
		ofsOutput << "\tSTARTGOLD=" << ServerStartGold() << '\n';
		ofsOutput << "\tSTARTPRIVS=" << ServerStartPrivs() << '\n';
		ofsOutput << "}" << '\n';
		
		ofsOutput << '\n' << "[gumps]" << '\n' << "{" << '\n';
		ofsOutput << "\tTITLECOLOUR=" << TitleColour() << '\n';
		ofsOutput << "\tLEFTTEXTCOLOUR=" << LeftTextColour() << '\n';
		ofsOutput << "\tRIGHTTEXTCOLOUR=" << RightTextColour() << '\n';
		ofsOutput << "\tBUTTONCANCEL=" << ButtonCancel() << '\n';
		ofsOutput << "\tBUTTONLEFT=" << ButtonLeft() << '\n';
		ofsOutput << "\tBUTTONRIGHT=" << ButtonRight() << '\n';
		ofsOutput << "\tBACKGROUNDPIC=" << BackgroundPic() << '\n';
		ofsOutput << "}" << '\n';
		
		ofsOutput << '\n' << "[houses]" << '\n' << "{" << '\n';
		ofsOutput << "\tTRACKHOUSESPERACCOUNT=" << (TrackHousesPerAccount()?1:0) << '\n';
		ofsOutput << "\tCANOWNANDCOOWNHOUSES=" << (CanOwnAndCoOwnHouses()?1:0) << '\n';
		ofsOutput << "\tCOOWNHOUSESONSAMEACCOUNT=" << (CoOwnHousesOnSameAccount()?1:0) << '\n';
		ofsOutput << "\tITEMDECAYINHOUSES=" << (ItemDecayInHouses()?1:0) << '\n';
		ofsOutput << "\tPROTECTPRIVATEHOUSES=" << (ProtectPrivateHouses()?1:0) << '\n';
		ofsOutput << "\tMAXHOUSESOWNABLE=" << MaxHousesOwnable() << '\n';
		ofsOutput << "\tMAXHOUSESCOOWNABLE=" << MaxHousesCoOwnable() << '\n';
		ofsOutput << "}" << '\n';
		
		ofsOutput << '\n' << "[towns]" << '\n' << "{" << '\n';
		ofsOutput << "\tPOLLTIME=" << TownNumSecsPollOpen() << '\n';
		ofsOutput << "\tMAYORTIME=" << TownNumSecsAsMayor() << '\n';
		ofsOutput << "\tTAXPERIOD=" << TownTaxPeriod() << '\n';
		ofsOutput << "\tGUARDSPAID=" << TownGuardPayment() << '\n';
		ofsOutput << "}" << '\n';
		
		ofsOutput << '\n' << "[disabled assistant features]" << '\n' << "{" << '\n';
		ofsOutput << "\tAF_FILTERWEATHER=" << (GetDisabledAssistantFeature( AF_FILTERWEATHER )?1:0) << '\n';
		ofsOutput << "\tAF_FILTERLIGHT=" << (GetDisabledAssistantFeature( AF_FILTERLIGHT )?1:0) << '\n';
		ofsOutput << "\tAF_SMARTTARGET=" << (GetDisabledAssistantFeature( AF_SMARTTARGET )?1:0) << '\n';
		ofsOutput << "\tAF_RANGEDTARGET=" << (GetDisabledAssistantFeature( AF_RANGEDTARGET )?1:0) << '\n';
		ofsOutput << "\tAF_AUTOOPENDOORS=" << (GetDisabledAssistantFeature( AF_AUTOOPENDOORS )?1:0) << '\n';
		ofsOutput << "\tAF_DEQUIPONCAST=" << (GetDisabledAssistantFeature( AF_DEQUIPONCAST )?1:0) << '\n';
		ofsOutput << "\tAF_AUTOPOTIONEQUIP=" << (GetDisabledAssistantFeature( AF_AUTOPOTIONEQUIP )?1:0) << '\n';
		ofsOutput << "\tAF_POISONEDCHECKS=" << (GetDisabledAssistantFeature( AF_POISONEDCHECKS )?1:0) << '\n';
		ofsOutput << "\tAF_LOOPEDMACROS=" << (GetDisabledAssistantFeature( AF_LOOPEDMACROS )?1:0) << '\n';
		ofsOutput << "\tAF_USEONCEAGENT=" << (GetDisabledAssistantFeature( AF_USEONCEAGENT )?1:0) << '\n';
		ofsOutput << "\tAF_RESTOCKAGENT=" << (GetDisabledAssistantFeature( AF_RESTOCKAGENT )?1:0) << '\n';
		ofsOutput << "\tAF_SELLAGENT=" << (GetDisabledAssistantFeature( AF_SELLAGENT )?1:0) << '\n';
		ofsOutput << "\tAF_BUYAGENT=" << (GetDisabledAssistantFeature( AF_BUYAGENT )?1:0) << '\n';
		ofsOutput << "\tAF_POTIONHOTKEYS=" << (GetDisabledAssistantFeature( AF_POTIONHOTKEYS )?1:0) << '\n';
		ofsOutput << "\tAF_RANDOMTARGETS=" << (GetDisabledAssistantFeature( AF_RANDOMTARGETS )?1:0) << '\n';
		ofsOutput << "\tAF_CLOSESTTARGETS=" << (GetDisabledAssistantFeature( AF_CLOSESTTARGETS )?1:0) << '\n';
		ofsOutput << "\tAF_OVERHEADHEALTH=" << (GetDisabledAssistantFeature( AF_OVERHEADHEALTH )?1:0) << '\n';
		ofsOutput << "\tAF_AUTOLOOTAGENT=" << (GetDisabledAssistantFeature( AF_AUTOLOOTAGENT )?1:0) << '\n';
		ofsOutput << "\tAF_BONECUTTERAGENT=" << (GetDisabledAssistantFeature( AF_BONECUTTERAGENT )?1:0) << '\n';
		ofsOutput << "\tAF_JSCRIPTMACROS=" << (GetDisabledAssistantFeature( AF_JSCRIPTMACROS )?1:0) << '\n';
		ofsOutput << "\tAF_AUTOREMOUNT=" << (GetDisabledAssistantFeature( AF_AUTOREMOUNT )?1:0) << '\n';
		ofsOutput << "\tAF_AUTOBANDAGE=" << (GetDisabledAssistantFeature( AF_AUTOBANDAGE )?1:0) << '\n';
		ofsOutput << "\tAF_ENEMYTARGETSHARE=" << (GetDisabledAssistantFeature( AF_ENEMYTARGETSHARE )?1:0) << '\n';
		ofsOutput << "\tAF_FILTERSEASON=" << (GetDisabledAssistantFeature( AF_FILTERSEASON )?1:0) << '\n';
		ofsOutput << "\tAF_SPELLTARGETSHARE=" << (GetDisabledAssistantFeature( AF_SPELLTARGETSHARE )?1:0) << '\n';
		ofsOutput << "\tAF_HUMANOIDHEALTHCHECKS=" << (GetDisabledAssistantFeature( AF_HUMANOIDHEALTHCHECKS )?1:0) << '\n';
		ofsOutput << "\tAF_SPEECHJOURNALCHECKS=" << (GetDisabledAssistantFeature( AF_SPEECHJOURNALCHECKS )?1:0) << '\n';
		ofsOutput << "}" << '\n';
		
		
		ofsOutput.close();
		rvalue = true;
	}
	else
		Console.error( oldstrutil::format("Unable to open file %s for writing", filename.c_str()) );
	return rvalue;
	
	
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	Load()
//|	Date		-	January 13, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Load up the uox.ini file and parse it into the internals
//|	Returns		-	pointer to the valid inmemory serverdata storage(this)
//|						nullptr is there is an error, or invalid file type
//o-----------------------------------------------------------------------------------------------o
auto CServerData::Load(const std::string &filename ) ->bool {
	auto inifile = filename ;
	if (inifile.empty()){
		
		inifile= Directory( CSDDP_ROOT ) + "uox.ini"s;
	}
	actualINI = inifile ;
	auto rvalue = ParseINI( inifile );
	PostLoadDefaults();
	return rvalue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 TrackingBaseRange() const
//|					void TrackingBaseRange( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the base (minimum) range even beginner trackers can track at
//o-----------------------------------------------------------------------------------------------o
auto CServerData::TrackingBaseRange() const ->std::uint16_t {
	return trackingbaserange;
}
//==============================================================================================
auto CServerData::TrackingBaseRange( UI16 value ) ->void {
	trackingbaserange = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 TrackingMaxTargets() const
//|					void TrackingMaxTargets( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max amount of targets someone can see in the tracking window
//o-----------------------------------------------------------------------------------------------o
auto CServerData::TrackingMaxTargets() const ->std::uint8_t {
	return trackingmaxtargets;
}
//==============================================================================================
auto CServerData::TrackingMaxTargets( UI08 value ) ->void {
	if( value >= MAX_TRACKINGTARGETS )
		trackingmaxtargets = MAX_TRACKINGTARGETS;
	else
		trackingmaxtargets = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 TrackingBaseTimer() const
//|					void TrackingBaseTimer( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max amount of time a grandmaster tracker can track someone
//o-----------------------------------------------------------------------------------------------o
auto CServerData::TrackingBaseTimer() const ->std::uint16_t {
	return trackingbasetimer;
}
//==============================================================================================
auto CServerData::TrackingBaseTimer( UI16 value ) ->void {
	trackingbasetimer = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 TrackingRedisplayTime() const
//|					void TrackingRedisplayTime( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets how often in seconds the tracking message is updated and displayed
//o-----------------------------------------------------------------------------------------------o
auto CServerData::TrackingRedisplayTime() const ->std::uint16_t {
	return trackingmsgredisplaytimer;
}
//==============================================================================================
auto CServerData::TrackingRedisplayTime( UI16 value ) ->void {
	trackingmsgredisplaytimer = value;
}


//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ParseINI( const std::string filename )
//|	Date		-	02/26/2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Parse the uox.ini file into its required information.
//|
//|	Changes		-	02/26/2002	-	Make sure that we parse out the logs, access
//|									and other directories that we were not parsing/
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ParseINI( const std::string& filename ) ->bool {
	auto rvalue = false ;
	auto input = std::ifstream(filename) ;
	enum search_t {header,startsection,endsection};
	auto retrieveContents = [](const std::string& input) ->std::optional<std::string> {
		auto pos1 = input.find("[") ;
		if (pos1 != std::string::npos) {
			auto pos2 = input.find("]",pos1) ;
			if (pos2 != std::string::npos){
				// We have the bounds
				auto contents = oldstrutil::trim(input.substr(pos1+1,(pos2 - pos1)-1)) ;
				return contents ;
			}
			
		}
		return {};
	};
	auto state = search_t(header) ;
	if (input.is_open()){
		char input_buffer[4096] ;
		while (input.good() && !input.eof()) {
			input.getline(input_buffer, 4095) ;
			if (input.gcount()>0) {
				input_buffer[input.gcount()] = 0 ;
				auto line = oldstrutil::trim(oldstrutil::removeTrailing(std::string(input_buffer),"//"))   ;
				if (!line.empty()) {
					switch (static_cast<int>(state)){
						case static_cast<int>(search_t::header): {
							if (line[0] == '[') {
								auto contents = retrieveContents(line) ;
								if (contents.has_value()){
									state = search_t::startsection;
								}
							}
						}
						case static_cast<int>(search_t::startsection): {
							if (line[0] == '{'){
								state = search_t::endsection ;
							}
						}
						case static_cast<int>(search_t::endsection): {
							if (line[0] != '}') {
								auto [key,value] = oldstrutil::split(line,"=");
								if (HandleLine(key, value)) {
									rvalue = true ;
								}
							}
							else {
								state = search_t::header ;
							}
							
						}
							
					}
				}
			}
		}
	}
	return rvalue ;
	/*
	 bool rvalue = false;
	 if( !filename.empty() ) {
	 
	 Console << "Processing INI Settings  ";
	 
	 Script toParse( filename, NUM_DEFS, false );
	 // Lock this file tight, No access at anytime when open(should only be open and closed anyhow. For Thread blocking)
	 if( !toParse.IsErrored() ) {
	 //serverList.clear();
	 startlocations.clear();
	 for (auto &[id,section] : toParse.collection()){
	 if( section){
	 for (auto &sec : section->collection()){
	 auto tag = sec->tag ;
	 auto data = oldstrutil::simplify( sec->data );
	 if( !HandleLine( tag, data ) ) {
	 Console.warning( oldstrutil::format("Unhandled tag '%s'", tag.c_str()) );
	 }
	 }
	 }
	 }
	 Console.PrintDone();
	 rvalue = true;
	 }
	 else
	 {
	 Console.warning( oldstrutil::format("%s File not found, Using default settings.", filename.c_str() ));
	 cwmWorldState->ServerData()->save();
	 }
	 }
	 return rvalue;
	 */
	
}

//==============================================================================================
auto CServerData::HandleLine( const std::string& tag, const std::string& value ) ->bool{
	bool rvalue = true;
	auto titer = uox3inicasevalue.find(tag) ;
	if (titer == uox3inicasevalue.end()){
		return false ;
	}
	
	switch( titer->second )
	{
		case 1:	 // SERVERNAME[0002]
			ServerName( value );
			break;
		case 2:	 // CONSOLELOG[0003]
			ServerConsoleLog( ( static_cast<UI16>( std::stoul( value, nullptr, 0 ) ) >= 1 ? true : false ) );
			break;
		case 3:	 // COMMANDPREFIX[0005]
			ServerCommandPrefix( (value.data()[0]) );	// return the first character of the return string only
			break;
		case 4:	 // ANNOUNCEWORLDSAVES[0006]
			ServerAnnounceSaves( (static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 ? true : false) );
			break;
		case 26:	 // JOINPARTMSGS[0007]
			ServerJoinPartAnnouncements( (static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 ? true : false) );
			break;
		case 5:	 // BACKUPSENABLED[0009]
			ServerBackups( (static_cast<UI16>(std::stoul(value, nullptr, 0)) > 0 ? true : false) );
			break;
		case 6:	 // SAVESTIMER[0010]
			ServerSavesTimer( static_cast<UI32>(std::stoul(value, nullptr, 0)) );
			break;
		case 7:	 // SKILLCAP[0011]
			ServerSkillTotalCap( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 8:	 // SKILLDELAY[0012]
			ServerSkillDelay( static_cast<UI08>(std::stoul(value, nullptr, 0)) );
			break;
		case 9:	 // STATCAP[0013]
			ServerStatCap( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 10:	 // MAXSTEALTHMOVEMENTS[0014]
			MaxStealthMovement( static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 11:	 // MAXSTAMINAMOVEMENTS[0015]
			MaxStaminaMovement( static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 12:	 // ARMORAFFECTMANAREGEN[0016]
			ArmorAffectManaRegen( (static_cast<UI08>(std::stoul(value, nullptr, 0)) > 0 ? true : false) );
			break;
		case 13:	 // CORPSEDECAYTIMER[0017]
			SystemTimer( tSERVER_CORPSEDECAY, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 14:	 // WEATHERTIMER[0018]
			SystemTimer( tSERVER_WEATHER, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 15:	 // SHOPSPAWNTIMER[0019]
			SystemTimer( tSERVER_SHOPSPAWN, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 16:	 // DECAYTIMER[0020]
			SystemTimer( tSERVER_DECAY, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 17:	 // INVISIBILITYTIMER[0021]
			SystemTimer( tSERVER_INVISIBILITY, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 18:	 // OBJECTUSETIMER[0022]
			SystemTimer( tSERVER_OBJECTUSAGE, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 19:	 // GATETIMER[0023]
			SystemTimer( tSERVER_GATE, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 20:	 // POISONTIMER[0024]
			SystemTimer( tSERVER_POISON, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 21:	 // LOGINTIMEOUT[0025]
			SystemTimer( tSERVER_LOGINTIMEOUT, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 22:	 // HITPOINTREGENTIMER[0026]
			SystemTimer( tSERVER_HITPOINTREGEN, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 23:	 // STAMINAREGENTIMER[0027]
			SystemTimer( tSERVER_STAMINAREGEN, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 24:	 // BASEFISHINGTIMER[0029]
			SystemTimer( tSERVER_FISHINGBASE,static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 34:	// MAXPETOWNERS[0211]
			MaxPetOwners( static_cast<UI08>( std::stoul( value, nullptr, 0 ) ) );
			break;
		case 35:	// MAXFOLLOWERS[0211]
			MaxFollowers( static_cast<UI08>( std::stoul( value, nullptr, 0 ) ) );
			break;
		case 36:	// MAXCONTROLSLOTS[0211]
			MaxControlSlots( static_cast<UI08>( std::stoul( value, nullptr, 0 ) ) );
			break;
		case 37:	 // MANAREGENTIMER[0028]
			SystemTimer( tSERVER_MANAREGEN, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 38:	 // RANDOMFISHINGTIMER[0030]
			SystemTimer( tSERVER_FISHINGRANDOM, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 39:	 // SPIRITSPEAKTIMER[0031]
			SystemTimer( tSERVER_SPIRITSPEAK, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 40:	 // DIRECTORY[0032]
		{
			Directory( CSDDP_ROOT, value );
			break;
		}
		case 41:	 // DATADIRECTORY[0033]
		{
			Directory( CSDDP_DATA, value );
			break;
		}
		case 42:	 // DEFSDIRECTORY[0034]
		{
			Directory( CSDDP_DEFS, value );
			break;
		}
		case 43:	 // ACTSDIRECTORY[0035]
		{
			Directory( CSDDP_ACCOUNTS, value );
			break;
		}
		case 25:	 // SCRIPTSDIRECTORY[0036]
		{
			Directory( CSDDP_SCRIPTS, value );
			break;
		}
		case 44:	 // BACKUPDIRECTORY[0037]
		{
			Directory( CSDDP_BACKUP, value );
			break;
		}
		case 45:	 // MSGBOARDDIRECTORY[0038]
		{
			Directory( CSDDP_MSGBOARD, value );
			break;
		}
		case 46:	 // SHAREDDIRECTORY[0039]
		{
			Directory( CSDDP_SHARED, value );
			break;
		}
		case 47:	 // LOOTDECAYSWITHCORPSE[0040]
			CorpseLootDecay( static_cast<UI16>(std::stoul(value, nullptr, 0)) != 0 );
			break;
		case 49:	 // GUARDSACTIVE[0041]
			GuardStatus( static_cast<UI16>(std::stoul(value, nullptr, 0)) != 0 );
			break;
		case 27:	 // DEATHANIMATION[0042]
			DeathAnimationStatus( static_cast<UI16>(std::stoul(value, nullptr, 0)) != 0 );
			break;
		case 50:	 // AMBIENTSOUNDS[0043]
			WorldAmbientSounds( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 51:	 // AMBIENTFOOTSTEPS[0044]
			AmbientFootsteps( static_cast<UI16>(std::stoul(value, nullptr, 0)) != 0 );
			break;
		case 52:	 // INTERNALACCOUNTCREATION[0045]
			InternalAccountStatus( static_cast<UI16>(std::stoul(value, nullptr, 0))!= 0 );
			break;
		case 53:	 // SHOWOFFLINEPCS[0046]
			ShowOfflinePCs( static_cast<UI16>(std::stoul(value, nullptr, 0)) != 0 );
			break;
		case 54:	 // ROGUESENABLED[0047]
			RogueStatus( static_cast<UI16>(std::stoul(value, nullptr, 0)) != 0 );
			break;
		case 55:	 // PLAYERPERSECUTION[0048]
			PlayerPersecutionStatus( static_cast<UI16>(std::stoul(value, nullptr, 0)) != 0 );
			break;
		case 56:	 // ACCOUNTFLUSH[0049]
			AccountFlushTimer( std::stod(value) );
			break;
		case 57:	 // HTMLSTATUSENABLED[0050]
			HtmlStatsStatus( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 58:	 // SELLBYNAME[0051]
			SellByNameStatus( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 59:	 // SELLMAXITEMS[0052]
			SellMaxItemsStatus( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 60:	 // TRADESYSTEM[0053]
			TradeSystemStatus( static_cast<UI16>(std::stoul(value, nullptr, 0)) != 0 );
			break;
		case 61:	 // RANKSYSTEM[0054]
			RankSystemStatus( static_cast<UI16>(std::stoul(value, nullptr, 0)) != 0 );
			break;
		case 62:	 // CUTSCROLLREQUIREMENTS[0055]
			CutScrollRequirementStatus( static_cast<UI16>(std::stoul(value, nullptr, 0)) != 0) ;
			break;
		case 63:	 // CHECKITEMS[0056]
			CheckItemsSpeed( std::stod(value) );
			break;
		case 64:	 // CHECKBOATS[0057]
			CheckBoatSpeed( std::stod(value) );
			break;
		case 65:	 // CHECKNPCAI[0058]
			CheckNpcAISpeed( std::stod(value) );
			break;
		case 66:	 // CHECKSPAWNREGIONS[0059]
			CheckSpawnRegionSpeed( std::stod(value) );
			break;
		case 67:	 // POSTINGLEVEL[0060]
			MsgBoardPostingLevel( static_cast<UI08>(std::stoul(value, nullptr, 0)) );
			break;
		case 68:	 // REMOVALLEVEL[0061]
			MsgBoardPostRemovalLevel( static_cast<UI08>(std::stoul(value, nullptr, 0)) );
			break;
		case 69:	 // ESCORTENABLED[0062]
			EscortsEnabled( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 70:	 // ESCORTINITEXPIRE[0063]
			SystemTimer( tSERVER_ESCORTWAIT, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 71:	 // ESCORTACTIVEEXPIRE[0064]
			SystemTimer( tSERVER_ESCORTACTIVE, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 72:	 // MOON1[0065]
			ServerMoon( 0, static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 73:	 // MOON2[0066]
			ServerMoon( 1, static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 74:	 // DUNGEONLEVEL[0067]
			DungeonLightLevel( static_cast<LIGHTLEVEL>(std::stoul(value, nullptr, 0)) );
			break;
		case 75:	 // CURRENTLEVEL[0068]
			WorldLightCurrentLevel( static_cast<LIGHTLEVEL>(std::stoul(value, nullptr, 0)) );
			break;
		case 76:	 // BRIGHTLEVEL[0069]
			WorldLightBrightLevel( static_cast<LIGHTLEVEL>(std::stoul(value, nullptr, 0)) );
			break;
		case 77:	 // BASERANGE[0070]
			TrackingBaseRange( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 78:	 // BASETIMER[0071]
			TrackingBaseTimer( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 79:	 // MAXTARGETS[0072]
			TrackingMaxTargets( static_cast<UI08>(std::stoul(value, nullptr, 0)) );
			break;
		case 80:	 // MSGREDISPLAYTIME[0073]
			TrackingRedisplayTime( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 81:	 // MURDERDECAYTIMER[0074]
			SystemTimer( tSERVER_MURDERDECAY, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 82:	 // MAXKILLS[0075]
			RepMaxKills( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 83:	 // CRIMINALTIMER[0076]
			SystemTimer( tSERVER_CRIMINAL, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 84:	 // MINECHECK[0077]
			MineCheck( static_cast<UI08>(std::stoul(value, nullptr, 0)) );
			break;
		case 85:	 // OREPERAREA[0078]
			ResOre(static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 86:	 // ORERESPAWNTIMER[0079]
			ResOreTime(static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 87:	 // RESOURCEAREASIZE
			ResourceAreaSize( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 88:	 // LOGSPERAREA[0081]
			ResLogs( static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 89:	 // LOGSRESPAWNTIMER[0082]
			ResLogTime( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 90:	 // STATSAFFECTSKILLCHECKS
			StatsAffectSkillChecks( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 91:	 // HUNGERRATE[0084]
			SystemTimer( tSERVER_HUNGERRATE, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 92:	 // HUNGERDMGVAL[0085]
			HungerDamage( static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 93:	 // MAXRANGE[0086]
			CombatMaxRange( static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 94:	 // SPELLMAXRANGE[0087]
			CombatMaxSpellRange(static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 95:	 // DISPLAYHITMSG[0088]
			CombatDisplayHitMessage( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 96:	 // MONSTERSVSANIMALS[0089]
			CombatMonstersVsAnimals( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 97:	 // ANIMALATTACKCHANCE[0090]
			CombatAnimalsAttackChance( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 98:	 // ANIMALSGUARDED[0091]
			CombatAnimalsGuarded( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 99:	 // NPCDAMAGERATE[0092]
			CombatNPCDamageRate( static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 100:	 // NPCBASEFLEEAT[0093]
			CombatNPCBaseFleeAt( static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 101:	 // NPCBASEREATTACKAT[0094]
			CombatNPCBaseReattackAt( static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 102:	 // ATTACKSTAMINA[0095]
			CombatAttackStamina( static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 103:	 // LOCATION[0096]
			ServerLocation( value );
			break;
		case 104:	 // STARTGOLD[0097]
			ServerStartGold( static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 105:	 // STARTPRIVS[0098]
			ServerStartPrivs( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 106:	 // ESCORTDONEEXPIRE[0099]
			SystemTimer( tSERVER_ESCORTDONE, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 107:	 // DARKLEVEL[0100]
			WorldLightDarkLevel( static_cast<LIGHTLEVEL>(std::stoul(value, nullptr, 0)) );
			break;
		case 108:	 // TITLECOLOUR[0101]
			TitleColour( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 109:	 // LEFTTEXTCOLOUR[0102]
			LeftTextColour( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 110:	 // RIGHTTEXTCOLOUR[0103]
			RightTextColour( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 111:	 // BUTTONCANCEL[0104]
			ButtonCancel( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 112:	 // BUTTONLEFT[0105]
			ButtonLeft( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 113:	 // BUTTONRIGHT[0106]
			ButtonRight( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 114:	 // BACKGROUNDPIC[0107]
			BackgroundPic( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 115:	 // POLLTIME[0108]
			TownNumSecsPollOpen( static_cast<UI32>(std::stoul(value, nullptr, 0)) );
			break;
		case 116:	 // MAYORTIME[0109]
			TownNumSecsAsMayor( static_cast<UI32>(std::stoul(value, nullptr, 0)) );
			break;
		case 117:	 // TAXPERIOD[0110]
			TownTaxPeriod( static_cast<UI32>(std::stoul(value, nullptr, 0)) );
			break;
		case 118:	 // GUARDSPAID[0111]
			TownGuardPayment( static_cast<UI32>(std::stoul(value, nullptr, 0)) );
			break;
		case 119:	 // DAY[0112]
			ServerTimeDay( static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 120:	 // HOURS[0113]
			ServerTimeHours( static_cast<UI08>(std::stoul(value, nullptr, 0)) );
			break;
		case 121:	 // MINUTES[0114]
			ServerTimeMinutes( static_cast<UI08>(std::stoul(value, nullptr, 0)) );
			break;
		case 122:	 // SECONDS[0115]
			ServerTimeSeconds( static_cast<UI08>(std::stoul(value, nullptr, 0)) );
			break;
		case 123:	 // AMPM[0116]
			ServerTimeAMPM( static_cast<UI16>(std::stoul(value, nullptr, 0)) != 0 );
			break;
		case 124:	 // SKILLLEVEL[0117]
			SkillLevel( static_cast<UI08>(std::stoul(value, nullptr, 0)) );
			break;
		case 125:	 // SNOOPISCRIME[0118]
			SnoopIsCrime( static_cast<UI16>(std::stoul(value, nullptr, 0)) != 0 );
			break;
		case 126:	 // BOOKSDIRECTORY[0119]
			Directory( CSDDP_BOOKS, value );
			break;
		case 127:	 // SERVERLIST[0120]
		{
			break;
		}
		case 128:	 // PORT[0121]
			ServerPort( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 129:	 // ACCESSDIRECTORY[0122]
			Directory( CSDDP_ACCESS, value );
			break;
		case 130:	 // LOGSDIRECTORY[0123]
			Directory( CSDDP_LOGS, value );
			break;
		case 131:	 // ACCOUNTISOLATION[0124]
			break;
		case 132:	 // HTMLDIRECTORY[0125]
			Directory( CSDDP_HTML, value );
			break;
		case 133:	 // SHOOTONANIMALBACK[0126]
			ShootOnAnimalBack( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 134:	 // NPCTRAININGENABLED[0127]
			NPCTrainingStatus( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 135:	 // DICTIONARYDIRECTORY[0128]
			Directory( CSDDP_DICTIONARIES, value );
			break;
		case 136:	 // BACKUPSAVERATIO[0129]
			BackupRatio( static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 137:	 // HIDEWHILEMOUNTED[0130]
			CharHideWhileMounted( static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 138:	 // SECONDSPERUOMINUTE[0131]
			ServerSecondsPerUOMinute( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 139:	 // WEIGHTPERSTR[0132]
			//WeightPerStr( value.toUByte() );
			WeightPerStr( std::stof(value) );
			break;
		case 140:	 // POLYDURATION[0133]
			SystemTimer( tSERVER_POLYMORPH, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 141:	 // UOGENABLED[0134]
			ServerUOGEnabled( static_cast<UI16>(std::stoul(value, nullptr, 0))==1 );
			break;
		case 142:	 // NETRCVTIMEOUT[0135]
			ServerNetRcvTimeout( static_cast<UI32>(std::stoul(value, nullptr, 0)));
			break;
		case 143:	 // NETSNDTIMEOUT[0136]
			ServerNetSndTimeout( static_cast<UI32>(std::stoul(value, nullptr, 0)) );
			break;
		case 144:	 // NETRETRYCOUNT[0137]
			ServerNetRetryCount( static_cast<UI32>(std::stoul(value, nullptr, 0)) );
			break;
		case 145:	 // CLIENTFEATURES[0138]
			SetClientFeatures( static_cast<UI32>(std::stoul(value, nullptr, 0)) );
			break;
		case 146:	 // PACKETOVERLOADS[0139]
			ServerOverloadPackets( (static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1) );
			break;
		case 147:	 // NPCMOVEMENTSPEED[0140]
			NPCWalkingSpeed( std::stof(value) );
			break;
		case 148:	 // PETHUNGEROFFLINE[0141]
			PetHungerOffline( (static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1) );
			break;
		case 149:	 // PETOFFLINETIMEOUT[0142]
			PetOfflineTimeout( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 150:	 // PETOFFLINECHECKTIMER[0143]
			SystemTimer( tSERVER_PETOFFLINECHECK, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 152:	 // ADVANCEDPATHFINDING[0145]
			AdvancedPathfinding( (static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1) );
			break;
		case 153:	 // SERVERFEATURES[0146]
			SetServerFeatures( static_cast<UI32>(std::stoul(value, nullptr, 0)) );
			break;
		case 154:	 // LOOTINGISCRIME[0147]
			LootingIsCrime( (static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1) );
			break;
		case 155:	 // NPCRUNNINGSPEED[0148]
			NPCRunningSpeed( std::stof(value) );
			break;
		case 156:	 // NPCFLEEINGSPEED[0149]
			NPCFleeingSpeed( std::stof(value) );
			break;
		case 157:	 // BASICTOOLTIPSONLY[0150]
			BasicTooltipsOnly( (static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1) );
			break;
		case 158:	 // GLOBALITEMDECAY[0151]
			GlobalItemDecay( (static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1) );
			break;
		case 159:	 // SCRIPTITEMSDECAYABLE[0152]
			ScriptItemsDecayable( (static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1) );
			break;
		case 160:	 // BASEITEMSDECAYABLE[0152]
			BaseItemsDecayable( (static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1) );
			break;
		case 161:	 // ITEMDECAYINHOUSES[0153]
			ItemDecayInHouses( (static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1) );
			break;
		case 162:	 // SPAWNREGIONSFACETS
			SetSpawnRegionsFacetStatus( static_cast<UI32>(std::stoul(value, nullptr, 0)) );
			break;
		case 163:	// PAPERDOLLGUILDBUTTON[0155]
			PaperdollGuildButton( static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 164:	// ATTACKSPEEDFROMSTAMINA[0156]
			CombatAttackSpeedFromStamina( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 169:	 // DISPLAYDAMAGENUMBERS[0157]
			CombatDisplayDamageNumbers( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 170:	 // CLIENTSUPPORT4000[0158]
			ClientSupport4000( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 171:	 // CLIENTSUPPORT5000[0159]
			ClientSupport5000( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 172:	 // CLIENTSUPPORT6000[0160]
			ClientSupport6000( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 173:	 // CLIENTSUPPORT6050[0161]
			ClientSupport6050( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 174:	 // CLIENTSUPPORT7000[0162]
			ClientSupport7000( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 175:	 // CLIENTSUPPORT7090[0163]
			ClientSupport7090( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 176:	 // CLIENTSUPPORT70160[0164]
			ClientSupport70160( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 177:	// CLIENTSUPPORT70240[0165]
			ClientSupport70240( static_cast<UI16>(std::stoul(value, nullptr, 0))== 1 );
			break;
		case 178:	// CLIENTSUPPORT70300[0166]
			ClientSupport70300( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 179:	// CLIENTSUPPORT70331[0167]
			ClientSupport70331( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 180:	// CLIENTSUPPORT704565[0168]
			ClientSupport704565( static_cast<UI16>(std::stoul(value, nullptr, 0))== 1 );
			break;
		case 181:	// CLIENTSUPPORT70610[0169]
			ClientSupport70610( static_cast<UI16>(std::stoul(value, nullptr, 0))== 1 );
			break;
		case 182:	 // EXTENDEDSTARTINGSTATS[0170]
			ExtendedStartingStats(static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 183:	 // EXTENDEDSTARTINGSKILLS[0171]
			ExtendedStartingSkills( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 184:	// WEAPONDAMAGECHANCE[0172]
			CombatWeaponDamageChance( static_cast<UI08>(std::stoul(value, nullptr, 0)) );
			break;
		case 185:	// ARMORDAMAGECHANCE[0173]
			CombatArmorDamageChance( static_cast<UI08>(std::stoul(value, nullptr, 0)));
			break;
		case 186:	// WEAPONDAMAGEMIN[0174]
			CombatWeaponDamageMin( static_cast<UI08>(std::stoul(value, nullptr, 0)));
			break;
		case 187:	// WEAPONDAMAGEMAX[0175]
			CombatWeaponDamageMax( static_cast<UI08>(std::stoul(value, nullptr, 0)) );
			break;
		case 188:	// ARMORDAMAGEMIN[0176]
			CombatArmorDamageMin( static_cast<UI08>(std::stoul(value, nullptr, 0)) );
			break;
		case 189:	// ARMORDAMAGEMAX[0177]
			CombatArmorDamageMax( static_cast<UI08>(std::stoul(value, nullptr, 0)) );
			break;
		case 190:	// GLOBALATTACKSPEED[0178]
			GlobalAttackSpeed( std::stof(value) );
			break;
		case 191:	// NPCSPELLCASTSPEED[0179]
			NPCSpellCastSpeed( std::stof(value) );
			break;
		case 192:	// FISHINGSTAMINALOSS[0180]
			FishingStaminaLoss( std::stof(value) );
			break;
		case 193:	// RANDOMSTARTINGLOCATION[0181]
			ServerRandomStartingLocation( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 194:	// ASSISTANTNEGOTIATION[0183]
			SetAssistantNegotiation( (static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1) );
			break;
		case 195:	// KICKONASSISTANTSILENCE[0184]
			KickOnAssistantSilence( (static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1) );
			break;
		case 196:	// AF_FILTERWEATHER[0185]
			SetDisabledAssistantFeature( AF_FILTERWEATHER, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 197:	// AF_FILTERLIGHT[0186]
			SetDisabledAssistantFeature( AF_FILTERLIGHT, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 198:	// AF_SMARTTARGET[0187]
			SetDisabledAssistantFeature( AF_SMARTTARGET, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 199:	// AF_RANGEDTARGET[0188]
			SetDisabledAssistantFeature( AF_RANGEDTARGET, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 200:	// AF_AUTOOPENDOORS[0189]
			SetDisabledAssistantFeature( AF_AUTOOPENDOORS, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 201:	// AF_DEQUIPONCAST[0190]
			SetDisabledAssistantFeature( AF_DEQUIPONCAST, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 202:	// AF_AUTOPOTIONEQUIP[0191]
			SetDisabledAssistantFeature( AF_AUTOPOTIONEQUIP, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 203:	// AF_POISONEDCHECKS[0192]
			SetDisabledAssistantFeature( AF_POISONEDCHECKS, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 204:	// AF_LOOPEDMACROS[0193]
			SetDisabledAssistantFeature( AF_LOOPEDMACROS, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 205:	// AF_USEONCEAGENT[0194]
			SetDisabledAssistantFeature( AF_USEONCEAGENT, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 206:	// AF_RESTOCKAGENT[0195]
			SetDisabledAssistantFeature( AF_RESTOCKAGENT, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 207:	// AF_SELLAGENT[0196]
			SetDisabledAssistantFeature( AF_SELLAGENT, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 208:	// AF_BUYAGENT[0197]
			SetDisabledAssistantFeature( AF_BUYAGENT, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 209:	// AF_POTIONHOTKEYS[0198]
			SetDisabledAssistantFeature( AF_POTIONHOTKEYS, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 210:	// AF_RANDOMTARGETS[0199]
			SetDisabledAssistantFeature( AF_RANDOMTARGETS, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 211:	// AF_CLOSESTTARGETS[0200]
			SetDisabledAssistantFeature( AF_CLOSESTTARGETS, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 212:	// AF_OVERHEADHEALTH[0201]
			SetDisabledAssistantFeature( AF_OVERHEADHEALTH, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 213:	// AF_AUTOLOOTAGENT[0202]
			SetDisabledAssistantFeature( AF_AUTOLOOTAGENT, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 214:	// AF_BONECUTTERAGENT[0203]
			SetDisabledAssistantFeature( AF_BONECUTTERAGENT, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 215:	// AF_JSCRIPTMACROS[0204]
			SetDisabledAssistantFeature( AF_JSCRIPTMACROS, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 216:	// AF_AUTOREMOUNT[0205]
			SetDisabledAssistantFeature( AF_AUTOREMOUNT, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 218:	// CLASSICUOMAPTRACKER[0207]
			SetClassicUOMapTracker( (static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1) );
			break;
		case 219:	// DECAYTIMERINHOUSE[0208]
			SystemTimer( tSERVER_DECAYINHOUSE, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 220:	// PROTECTPRIVATEHOUSES[0209]
			ProtectPrivateHouses( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 221:	// TRACKHOUSESPERACCOUNT[0210]
			TrackHousesPerAccount( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 222:	// MAXHOUSESOWNABLE[0211]
			MaxHousesOwnable( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 223:	// MAXHOUSESCOOWNABLE[0212]
			MaxHousesCoOwnable( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 224:	// CANOWNANDCOOWNHOUSES[0213]
			CanOwnAndCoOwnHouses( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 225:	// COOWNHOUSESONSAMEACCOUNT[0214]
			CoOwnHousesOnSameAccount( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 226:	// ITEMSDETECTSPEECH[0215]
			ItemsDetectSpeech( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 227:	// MAXPLAYERPACKITEMS[0216]
			MaxPlayerPackItems( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 228:	// MAXPLAYERBANKITEMS[0217]
			MaxPlayerBankItems( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 229:	// FORCENEWANIMATIONPACKET[0218]
			ForceNewAnimationPacket( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 230:	// MAPDIFFSENABLED[0219]
			MapDiffsEnabled( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 240:	// PARRYDAMAGECHANCE[0229]
			CombatParryDamageChance( static_cast<UI08>(std::stoul(value, nullptr, 0)) );
			break;
		case 241:	// PARRYDAMAGEMIN[0230]
			CombatParryDamageMin( static_cast<UI08>(std::stoul(value, nullptr, 0)) );
			break;
		case 242:	// PARRYDAMAGEMAX[0231]
			CombatParryDamageMax( static_cast<UI08>(std::stoul(value, nullptr, 0)) );
			break;
		case 243:	// ARMORCLASSDAMAGEBONUS[0232]
			CombatArmorClassDamageBonus(  static_cast<SI08>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 244:	// CUOENABLED[0233]
			ConnectUOServerPoll( (static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1) );
			break;
		case 245:	// ALCHEMYBONUSENABLED[0234]
			AlchemyDamageBonusEnabled( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 246:	// ALCHEMYBONUSMODIFIER[0235]
			AlchemyDamageBonusModifier( static_cast<UI08>(std::stoul(value, nullptr, 0)) );
			break;
		case 247:	 // NPCFLAGUPDATETIMER[0236]
			SystemTimer( tSERVER_NPCFLAGUPDATETIMER, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 248:	 // JSENGINESIZE[0237]
			SetJSEngineSize( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 249:	 // USEUNICODEMESSAGES[0238]
			UseUnicodeMessages( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 250:	 // SCRIPTDATADIRECTORY[0239]
		{
			Directory( CSDDP_SCRIPTDATA, value );
			break;
		}
		case 251:    // THIRSTRATE[0240]
			SystemTimer( tSERVER_THIRSTRATE, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 252:    // THIRSTDRAINVAL[0241]
			ThirstDrain( static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 253:    // PETTHIRSTOFFLINE[0242]
			PetThirstOffline( (static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1) );
			break;
		case 254:	// ExternalIP
			ExternalIP(value);
			break;
		case 255:	 // BLOODDECAYTIMER[0243]
			SystemTimer( tSERVER_BLOODDECAY, static_cast<UI16>( std::stoul( value, nullptr, 0 ) ) );
			break;
		case 256:	 // BLOODDECAYCORPSETIMER[0244]
			SystemTimer( tSERVER_BLOODDECAYCORPSE, static_cast<UI16>( std::stoul( value, nullptr, 0 ) ) );
			break;
		case 257:	// BLOODEFFECTCHANCE[0245]
			CombatBloodEffectChance( static_cast<UI08>( std::stoul( value, nullptr, 0 ) ) );
			break;
		case 258:	 // NPCCORPSEDECAYTIMER[0246]
			SystemTimer( tSERVER_NPCCORPSEDECAY, static_cast<UI16>( std::stoul( value, nullptr, 0 ) ) );
			break;
		case 259:    // HUNGERENABLED[0247]
			HungerSystemEnabled( ( static_cast<SI16>( std::stoi( value, nullptr, 0 ) ) == 1 ) );
			break;
		case 260:    // THIRSTENABLED[0248]
			ThirstSystemEnabled( ( static_cast<SI16>( std::stoi( value, nullptr, 0 ) ) == 1 ) );
			break;
		case 261:    // TRAVELSPELLSFROMBOATKEYS[0249]
			TravelSpellsFromBoatKeys( ( static_cast<SI16>( std::stoi( value, nullptr, 0 ) ) == 1 ) );
			break;
		case 262:    // TRAVELSPELLSWHILEOVERWEIGHT[0250]
			TravelSpellsWhileOverweight( ( static_cast<SI16>( std::stoi( value, nullptr, 0 ) ) == 1 ) );
			break;
		case 263:    // MARKRUNESINMULTIS[0251]
			MarkRunesInMultis( ( static_cast<SI16>( std::stoi( value, nullptr, 0 ) ) == 1 ) );
			break;
		case 264:    // TRAVELSPELLSBETWEENWORLD[0252]
			TravelSpellsBetweenWorlds( ( static_cast<SI16>( std::stoi( value, nullptr, 0 ) ) == 1 ) );
			break;
		case 265:    // TRAVELSPELLSWHILEAGGRESSOR[0253]
			TravelSpellsWhileAggressor( ( static_cast<SI16>( std::stoi( value, nullptr, 0 ) ) == 1 ) );
			break;
		case 266:	// BUYBANKTHRESHOLD[0254]
			BuyThreshold( static_cast<UI16>( std::stoul( value, nullptr, 0 ) ) );
			break;
		case 267:	// NETWORKLOG[0255]
			ServerNetworkLog(( static_cast<UI16>( std::stoul( value, nullptr, 0 ) ) >= 1 ? true : false ));
			break;
		case 268:	// SPEECHLOG[0256]
			ServerSpeechLog(( static_cast<UI16>( std::stoul( value, nullptr, 0 ) ) >= 1 ? true : false ));
			break;
		case 269:	 // NPCMOUNTEDWALKINGSPEED[0257]
			NPCMountedWalkingSpeed( std::stof( value ) );
			break;
		case 270:	 // NPCMOUNTEDRUNNINGSPEED[0258]
			NPCMountedRunningSpeed( std::stof( value ) );
			break;
		case 271:	 // NPCMOUNTEDFLEEINGSPEED[0259]
			NPCMountedFleeingSpeed( std::stof( value ) );
			break;
		case 272:	// CONTEXTMENUS[0260]
			ServerContextMenus( ( static_cast<UI16>( std::stoul( value, nullptr, 0 ) ) >= 1 ? true : false ) );
			break;
		case 273:	// SERVERLANGUAGE[0261]
			ServerLanguage( static_cast<UI16>( std::stoul( value, nullptr, 0 ) ) );
			break;
		case 274:	// CHECKPETCONTROLDIFFICULTY[0262]
			CheckPetControlDifficulty( ( static_cast<UI16>( std::stoul( value, nullptr, 0 ) ) >= 1 ? true : false ) );
			break;
		case 275:	// PETLOYALTYGAINONSUCCESS[0263]
			SetPetLoyaltyGainOnSuccess( static_cast<UI16>( std::stoul( value, nullptr, 0 ) ) );
			break;
		case 276:	// PETLOYALTYLOSSONFAILURE[0264]
			SetPetLoyaltyLossOnFailure( static_cast<UI16>( std::stoul( value, nullptr, 0 ) ) );
			break;
		case 277:    // LOYALTYRATE[0265]
			SystemTimer( tSERVER_LOYALTYRATE, static_cast<UI16>( std::stoul( value, nullptr, 0 ) ) );
			break;
		case 278:	// SHOWNPCTITLESINTOOLTIPS[0262]
			ShowNpcTitlesInTooltips( ( static_cast<UI16>( std::stoul( value, nullptr, 0 ) ) >= 1 ? true : false ) );
			break;
		case 279:	 // FISHPERAREA
			ResFish(static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 280:	 // FISHRESPAWNTIMER
			ResFishTime(static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 281:	 // ARCHERYHITBONUS
			CombatArcheryHitBonus( static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 282:    // ITEMSINTERRUPTCASTING
			ItemsInterruptCasting( ( static_cast<SI16>( std::stoi( value, nullptr, 0 ) ) == 1 ) );
			break;
		case 283:	 // SYSMESSAGECOLOUR
			SysMsgColour( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 284:	// AF_AUTOBANDAGE
			SetDisabledAssistantFeature( AF_AUTOBANDAGE, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 285:	// AF_ENEMYTARGETSHARE
			SetDisabledAssistantFeature( AF_ENEMYTARGETSHARE, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 286:	// AF_FILTERSEASON
			SetDisabledAssistantFeature( AF_FILTERSEASON, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 287:	// AF_SPELLTARGETSHARE
			SetDisabledAssistantFeature( AF_SPELLTARGETSHARE, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 288:	// AF_HUMANOIDHEALTHCHECKS
			SetDisabledAssistantFeature( AF_HUMANOIDHEALTHCHECKS, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 289:	// AF_SPEECHJOURNALCHECKS
			SetDisabledAssistantFeature( AF_SPEECHJOURNALCHECKS, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 290:	// ARCHERYSHOOTDELAY
			CombatArcheryShootDelay( std::stof(value) );
			break;
		case 291:	 // MAXCLIENTBYTESIN
			MaxClientBytesIn( static_cast<UI32>(std::stoul(value, nullptr, 0)) );
			break;
		case 292:	 // MAXCLIENTBYTESOUT
			MaxClientBytesOut( static_cast<UI32>(std::stoul(value, nullptr, 0)) );
			break;
		case 293:	 // NETTRAFFICTIMEBAN
			NetTrafficTimeban( static_cast<UI32>(std::stoul(value, nullptr, 0)) );
			break;
		case 294:    // TOOLUSELIMIT
			ToolUseLimit( ( static_cast<SI16>( std::stoi( value, nullptr, 0 ) ) == 1 ) );
			break;
		case 295:    // TOOLUSEBREAK
			ToolUseBreak( ( static_cast<SI16>( std::stoi( value, nullptr, 0 ) ) == 1 ) );
			break;
		case 296:    // ITEMREPAIRDURABILITYLOSS
			ItemRepairDurabilityLoss( ( static_cast<SI16>( std::stoi( value, nullptr, 0 ) ) == 1 ) );
			break;
		case 297:    // HIDESTATSFORUNKNOWNMAGICITEMS
			HideStatsForUnknownMagicItems( ( static_cast<SI16>( std::stoi( value, nullptr, 0 ) ) == 1 ) );
			break;
		case 298:    // CRAFTCOLOUREDWEAPONS
			CraftColouredWeapons( ( static_cast<SI16>( std::stoi( value, nullptr, 0 ) ) == 1 ) );
			break;
		case 299:	// MAXSAFETELEPORTSPERDAY
			MaxSafeTeleportsPerDay( static_cast<UI08>( std::stoul( value, nullptr, 0 ) ) );
			break;
		case 300:    // TELEPORTONEARESTSAFELOCATION
			TeleportToNearestSafeLocation( ( static_cast<SI16>( std::stoi( value, nullptr, 0 ) ) == 1 ) );
			break;
		case 301:    // ALLOWAWAKENPCS
			AllowAwakeNPCs( ( static_cast<SI16>( std::stoi( value, nullptr, 0 ) ) == 1 ) );
			break;
			break;		default:
			rvalue = false;
			break;
	}
	return rvalue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 ServerStartGold() const
//|					void ServerStartGold( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default amount of starting gold for new characters
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ServerStartGold() const ->std::int16_t {
	return startgold;
}
//==============================================================================================
auto CServerData::ServerStartGold( SI16 value ) ->void {
	if( value >= 0 )
		startgold = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ServerStartPrivs() const
//|					void ServerStartPrivs( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default starting privs for new characters
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ServerStartPrivs() const ->std::uint16_t {
	return startprivs;
}
//==============================================================================================
auto CServerData::ServerStartPrivs( UI16 value ) ->void {
	startprivs = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 ServerMoon( SI16 slot ) const
//|					void ServerMoon( SI16 slot, SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the phase of one of the game's two moons
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ServerMoon( SI16 slot ) const ->std::int16_t {
	SI16 rvalue = -1;
	if( slot >= 0 && slot <= 1 )
		rvalue = moon[slot];
	return rvalue;
}
//==============================================================================================
auto CServerData::ServerMoon( SI16 slot, SI16 value ) ->void {
	if( slot >= 0 && slot <= 1 && value >= 0 )
		moon[slot] = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	LIGHTLEVEL DungeonLightLevel() const
//|					void DungeonLightLevel( LIGHTLEVEL value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default, global light level for dungeons
//o-----------------------------------------------------------------------------------------------o
auto CServerData::DungeonLightLevel() const->LIGHTLEVEL {
	return dungeonlightlevel;
}
//==============================================================================================
auto CServerData::DungeonLightLevel( LIGHTLEVEL value ) ->void {
	dungeonlightlevel = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	LIGHTLEVEL WorldLightCurrentLevel() const
//|					void WorldLightCurrentLevel( LIGHTLEVEL value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default, global current light level outside of dungeons
//o-----------------------------------------------------------------------------------------------o
auto CServerData::WorldLightCurrentLevel() const->LIGHTLEVEL {
	return currentlightlevel;
}
//==============================================================================================
auto CServerData::WorldLightCurrentLevel( LIGHTLEVEL value ) ->void {
	currentlightlevel = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	LIGHTLEVEL WorldLightBrightLevel() const
//|					void WorldLightBrightLevel( LIGHTLEVEL value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default, global light level for the brightest time of day
//o-----------------------------------------------------------------------------------------------o
auto CServerData::WorldLightBrightLevel() const->LIGHTLEVEL {
	return brightnesslightlevel;
}
//==============================================================================================
auto CServerData::WorldLightBrightLevel( LIGHTLEVEL value ) ->void {
	brightnesslightlevel = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	LIGHTLEVEL WorldLightDarkLevel() const
//|					void WorldLightDarkLevel( LIGHTLEVEL value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default, global light level for the darkest time of day
//o-----------------------------------------------------------------------------------------------o
auto CServerData::WorldLightDarkLevel() const ->LIGHTLEVEL {
	return darknesslightlevel;
}
//==============================================================================================
auto CServerData::WorldLightDarkLevel( LIGHTLEVEL value ) ->void {
	darknesslightlevel=value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void PostLoadDefaults()
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	If no start locations have been provided in ini, use hardcoded defaults
//o-----------------------------------------------------------------------------------------------o
auto CServerData::PostLoadDefaults() ->void {
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
auto CServerData::ServerLocation( size_t locNum ) ->__STARTLOCATIONDATA__* {
	__STARTLOCATIONDATA__ *rvalue = nullptr;
	if( locNum < startlocations.size() )
		rvalue = &startlocations[locNum];
	return rvalue;
}

//==============================================================================================
auto CServerData::ServerLocation( std::string toSet ) ->void {
	auto temp = toSet;
	temp = oldstrutil::trim( oldstrutil::removeTrailing( temp, "//" ));
	auto csecs = oldstrutil::sections( temp, "," );
	
	__STARTLOCATIONDATA__ toAdd;
	if (csecs.size() >=7){
		toAdd.oldTown = oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" ));
		toAdd.oldDescription = oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" ));
		toAdd.newTown = toAdd.oldTown ;
		toAdd.newDescription = toAdd.oldDescription;
		
		toAdd.x = static_cast<SI16>(std::stoi(oldstrutil::trim( oldstrutil::removeTrailing( csecs[2], "//" )), nullptr, 0));
		toAdd.y = static_cast<SI16>(std::stoi(oldstrutil::trim( oldstrutil::removeTrailing( csecs[3], "//" )), nullptr, 0));
		toAdd.z = static_cast<SI16>(std::stoi(oldstrutil::trim( oldstrutil::removeTrailing( csecs[4], "//" )), nullptr, 0));
		toAdd.worldNum = static_cast<SI16>(std::stoi(oldstrutil::trim( oldstrutil::removeTrailing( csecs[5], "//" )), nullptr, 0));
		if (csecs.size() ==7){
			toAdd.clilocDesc	= static_cast<UI32>(std::stoul(oldstrutil::trim( oldstrutil::removeTrailing( csecs[6], "//" )), nullptr, 0));
		}
		else {
			toAdd.instanceID	= static_cast<SI16>(std::stoi(oldstrutil::trim( oldstrutil::removeTrailing( csecs[6], "//" )), nullptr, 0));
			toAdd.clilocDesc	= static_cast<UI32>(std::stoul(oldstrutil::trim( oldstrutil::removeTrailing( csecs[7], "//" )), nullptr, 0));
		}
		startlocations.push_back( toAdd );
	}
	else {
		Console.error( "Malformed location entry in ini file" );
	}
}

auto CServerData::NumServerLocations() const ->size_t {
	return startlocations.size();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ServerSecondsPerUOMinute() const
//|					void ServerSecondsPerUOMinute( UI16 newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the number of real life seconds per UO minute
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ServerSecondsPerUOMinute() const ->std::uint16_t {
	return secondsperuominute;
}
//==============================================================================================
auto CServerData::ServerSecondsPerUOMinute( UI16 newVal ) ->void {
	secondsperuominute = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ServerLanguage() const
//|					void ServerLanguage( UI16 newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default (if any) language used for the server
//o-----------------------------------------------------------------------------------------------o
auto CServerData::ServerLanguage() const ->std::uint16_t {
	return serverLanguage;
}
//==============================================================================================
auto CServerData::ServerLanguage( UI16 newVal ) ->void {
	if( newVal < DL_COUNT )
		serverLanguage = newVal;
	else
		serverLanguage = DL_DEFAULT;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 MaxClientBytesIn() const
//|					void MaxClientBytesIn( UI32 newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets max incoming bytes received from a socket in each 10 second period
//|					If amount exceeds this cap, client might receive a warning/get kicked
//o-----------------------------------------------------------------------------------------------o
auto CServerData::MaxClientBytesIn() const ->std::uint32_t {
	return maxBytesIn;
}
//==============================================================================================
auto CServerData::MaxClientBytesIn( UI32 newVal ) ->void {
	maxBytesIn = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 MaxClientBytesOut() const
//|					void MaxClientBytesOut( UI32 newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets max outgoing bytes sent to a socket in each 10 second period
//|					If amount exceeds this cap, client might receive a warning/get kicked
//o-----------------------------------------------------------------------------------------------o
auto CServerData::MaxClientBytesOut() const ->std::uint32_t {
	return maxBytesOut;
}
//==============================================================================================
auto CServerData::MaxClientBytesOut( UI32 newVal ) ->void {
	maxBytesOut = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 NetTrafficTimeban() const
//|					void NetTrafficTimeban( UI32 newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets max amount of time a player will be temporarily banned for if they
//|					exceed their alotted network traffic budget
//o-----------------------------------------------------------------------------------------------o
//==============================================================================================
auto CServerData::NetTrafficTimeban() const ->std::uint32_t {
	return trafficTimeban;
}
//==============================================================================================
auto CServerData::NetTrafficTimeban( UI32 newVal ) ->void {
	trafficTimeban = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetJSEngineSize() const
//|					void SetJSEngineSize( UI16 newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets jsEngineSize (in MB), used to define max bytes per JSRuntime
//|					before a last ditch GC effort is made
//o-----------------------------------------------------------------------------------------------o
auto CServerData::GetJSEngineSize() const ->std::uint16_t {
	return jsEngineSize;
}
//==============================================================================================
auto CServerData::SetJSEngineSize( UI16 newVal ) ->void {
	jsEngineSize = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SaveTime()
//|	Date		-	January 28th, 2007
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Outputs server time information to time.wsc in the /shared/ directory
//o-----------------------------------------------------------------------------------------------o
auto CServerData::SaveTime() ->void {
	std::string		timeFile = cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "time.wsc";
	std::ofstream	timeDestination( timeFile.c_str() );
	if( !timeDestination )
	{
		Console.error( oldstrutil::format("Failed to open %s for writing", timeFile.c_str()) );
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

auto ReadWorldTagData( std::ifstream &inStream, std::string &tag, std::string &data ) ->void ;
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CServerData::LoadTime()
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads server time information from time.wsc in the /shared/ directory
//o-----------------------------------------------------------------------------------------------o
auto CServerData::LoadTime() ->void {
	std::ifstream input;
	std::string filename = cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "time.wsc";
	
	input.open( filename.c_str(), std::ios_base::in );
	input.seekg( 0, std::ios::beg );
	
	
	if( input.is_open() )
	{
		char line[1024];
		while( !input.eof() && !input.fail() )
		{
			input.getline(line, 1023);
			line[input.gcount()] = 0;
			std::string sLine(line);
			sLine = oldstrutil::trim( oldstrutil::removeTrailing( sLine, "//" ));
			if( !sLine.empty() )
			{
				if( oldstrutil::upper( sLine ) == "[TIME]" )
					LoadTimeTags( input );
			}
		}
		input.close();
	}
}

//==============================================================================================
auto CServerData::LoadTimeTags( std::ifstream &input ) ->void {
	std::string UTag, tag, data;
	while( tag != "o---o" )
	{
		ReadWorldTagData( input, tag, data );
		if( tag != "o---o" )
		{
			UTag = oldstrutil::upper(tag);
			
			if( UTag == "AMPM" )
			{
				ServerTimeAMPM( (std::stoi(data, nullptr, 0) == 1) );
			}
			else if( UTag == "CURRENTLIGHT" )
			{
				WorldLightCurrentLevel( static_cast<UI16>(std::stoul(data, nullptr, 0)) );
			}
			else if( UTag == "DAY" )
			{
				ServerTimeDay( static_cast<SI16>(std::stoi(data, nullptr, 0)) );
			}
			else if( UTag == "HOUR" )
			{
				ServerTimeHours( static_cast<UI16>(std::stoul(data, nullptr, 0)) );
			}
			else if( UTag == "MINUTE" )
			{
				ServerTimeMinutes( static_cast<UI16>(std::stoul(data, nullptr, 0)) );
			}
			else if( UTag == "MOON" )
			{
				auto csecs = oldstrutil::sections( data, "," );
				if( csecs.size() > 1 )
				{
					ServerMoon( 0, static_cast<SI16>(std::stoi(oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0)) );
					ServerMoon( 1, static_cast<SI16>(std::stoi(oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0)) );
				}
			}
		}
	}
	tag = "";
}

//==============================================================================================
auto CServerData::ServerTimeDay() const ->std::int16_t {
	return days;
}
//==============================================================================================
auto CServerData::ServerTimeDay( SI16 nValue ) ->void {
	days = nValue;
}
//==============================================================================================
auto CServerData::ServerTimeHours() const ->std::uint8_t {
	return hours;
}
//==============================================================================================
auto CServerData::ServerTimeHours( UI08 nValue ) ->void {
	hours = nValue;
}
//==============================================================================================
auto CServerData::ServerTimeMinutes() const ->std::uint8_t {
	return minutes;
}

//==============================================================================================
auto CServerData::ServerTimeMinutes( UI08 nValue ) ->void {
	minutes = nValue;
}
//==============================================================================================
auto CServerData::ServerTimeSeconds() const ->std::uint8_t {
	return seconds;
}
//==============================================================================================
auto CServerData::ServerTimeSeconds(UI08 nValue) ->void {
	seconds = nValue;
}
//==============================================================================================
auto CServerData::ServerTimeAMPM() const ->bool{
	return ampm;
}
//==============================================================================================
auto CServerData::ServerTimeAMPM(bool nValue) ->void {
	ampm = nValue;
}

//==============================================================================================
auto CServerData::incSecond() ->bool{
	bool rvalue = false;
	++seconds;
	if( seconds == 60 )
	{
		seconds = 0;
		rvalue	= incMinute();
	}
	return rvalue;
}

//==============================================================================================
auto CServerData::incMoon(SI32 mNumber) ->void {
	moon[mNumber] = (SI16)((moon[mNumber] + 1)%8);
}

//==============================================================================================
auto CServerData::incMinute() ->bool{
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

//==============================================================================================
auto CServerData::incHour() ->bool{
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

//==============================================================================================
auto CServerData::incDay() ->bool{
	++days;
	return true;
}


//==============================================================================================
auto CServerData::ServerEntry(std::uint16_t  entryNum ) ->physicalServer *{
	physicalServer *rvalue = nullptr;
	if( entryNum < serverList.size() ){
		rvalue = &serverList[entryNum];
	}
	return rvalue;
}
//==============================================================================================
auto CServerData::ServerCount() const ->std::uint16_t {
	
	return static_cast<UI16>(serverList.size());
}
//==============================================================================================
// physicalServer
//==============================================================================================

//==============================================================================================
auto physicalServer::setName(const std::string& newName) ->void {
	name = newName;
}
//==============================================================================================
auto physicalServer::setDomain(const std::string& newDomain) ->void {
	domain = newDomain;
}
//==============================================================================================
auto physicalServer::setIP(const std::string& newIP) ->void {
	ip = newIP;
}
//==============================================================================================
auto physicalServer::setPort(UI16 newPort) ->void {
	port = newPort;
}
//==============================================================================================
auto physicalServer::getName() const ->const std::string& {
	return name;
}
//==============================================================================================
auto physicalServer::getDomain() const ->const std::string &{
	return domain;
}
//==============================================================================================
auto physicalServer::getIP() const ->const std::string &{
	return ip;
}
//==============================================================================================
auto physicalServer::getPort() const -> std::uint16_t {
	return port;
}
