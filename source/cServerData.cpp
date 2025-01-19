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
//==================================================================================================
// constant data
//==================================================================================================
//==================================================================================================
const std::map<std::string, SI32> CServerData::uox3IniCaseValue
{
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
	{"BASEFISHINGTIMER"s, 24},
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
	{"LOOTDECAYSWITHPLAYERCORPSE"s, 47},
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
	{"SPELLMAXRANGE"s, 94},
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
	{"CORESHARDERA"s, 231},
	{"ARMORCALCULATION"s, 232},
	{"STRENGTHDAMAGEBONUS"s, 233},
	{"TACTICSDAMAGEBONUS"s, 234},
	{"ANATOMYDAMAGEBONUS"s, 235},
	{"LUMBERJACKDAMAGEBONUS"s, 236},
	{"RACIALDAMAGEBONUS"s, 237},
	{"DAMAGEBONUSCAP"s, 238},
	{"SHIELDPARRY"s, 239},
	{"PARRYDAMAGECHANCE"s, 240},
	{"PARRYDAMAGEMIN"s, 241},
	{"PARRYDAMAGEMAX"s, 242},
	{"ARMORCLASSDAMAGEBONUS"s, 243},
	{"FREESHARDSERVERPOLL"s, 244},
	{"ALCHEMYBONUSENABLED"s, 245},
	{"ALCHEMYBONUSMODIFIER"s, 246},
	{"NPCFLAGUPDATETIMER"s, 247},
	{"JSENGINESIZE"s, 248},
	{"USEUNICODEMESSAGES"s, 249},
	{"SCRIPTDATADIRECTORY"s, 250},
	{"THIRSTRATE"s, 251},
	{"THIRSTDRAINVAL"s, 252},
	{"PETTHIRSTOFFLINE"s, 253},
	{"EXTERNALIP"s, 254},
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
	{"ALLOWAWAKENPCS"s, 301},
	{"DISPLAYMAKERSMARK"s, 302},
	{"SHOWNPCTITLESOVERHEAD"s, 303},
	{"SHOWINVULNERABLETAGOVERHEAD"s, 304},
	{"PETCOMBATTRAINING"s, 305},
	{"HIRELINGCOMBATTRAINING"s, 306},
	{"NPCCOMBATTRAINING"s, 307},
	{"GLOBALRESTOCKMULTIPLIER"s, 308},
	{"SHOWITEMRESISTSTATS"s, 309},
	{"SHOWWEAPONDAMAGETYPES"s, 310},
	{"SHOWRACEWITHNAME"s, 311},
	{"SHOWRACEINPAPERDOLL"s, 312},
	{"WEAPONPARRY"s, 313},
	{"WRESTLINGPARRY"s, 314},
	{"COMBATHITCHANCE"s, 315},
	{"CASTSPELLSWHILEMOVING"s, 316},
	{"SHOWREPUTATIONTITLEINTOOLTIP"s, 317},
	{"SHOWGUILDINFOINTOOLTIP"s, 318},
	{"MAXPLAYERPACKWEIGHT"s, 319},
	{"MAXPLAYERBANKWEIGHT"s, 320},
	{"SAFECOOWNERLOGOUT"s, 321},
	{"SAFEFRIENDLOGOUT"s, 322},
	{"SAFEGUESTLOGOUT"s, 323},
	{"KEYLESSOWNERACCESS"s, 324},
	{"KEYLESSCOOWNERACCESS"s, 325},
	{"KEYLESSFRIENDACCESS"s, 326},
	{"KEYLESSGUESTACCESS"s, 327},
	{"WEAPONDAMAGEBONUSTYPE"s, 328},
	{"OFFERBODSFROMITEMSALES"s, 329},
	{"OFFERBODSFROMCONTEXTMENU"s, 330},
	{"BODSFROMCRAFTEDITEMSONLY"s, 331},
	{"BODGOLDREWARDMULTIPLIER"s, 332},
	{"BODFAMEREWARDMULTIPLIER"s, 333},
	{"ENABLENPCGUILDDISCOUNTS"s, 334},
	{"ENABLENPCGUILDPREMIUMS"s, 335},
	{"AGGRESSORFLAGTIMER"s, 336},
	{"PERMAGREYFLAGTIMER"s, 337},
	{"STEALINGFLAGTIMER"s, 338},
	{"SNOOPAWARENESS"s, 339},
	{"APSPERFTHRESHOLD"s, 340},
	{"APSINTERVAL"s, 341},
	{"APSDELAYSTEP"s, 342},
	{"APSDELAYMAXCAP"s, 343},
	{"YOUNGPLAYERSYSTEM"s, 344},
	{"YOUNGLOCATION"s, 345},
	{"SECRETSHARDKEY"s, 346},
	{"MOONGATEFACETS"s, 347},
	{"AUTOUNEQUIPPEDCASTING"s, 348},
	{"LOOTDECAYSWITHNPCCORPSE"s, 349},
	{"PHYSICALRESISTCAP"s, 355},
	{"FIRERESISTCAP"s, 356},
	{"COLDRESISTCAP"s, 357},
	{"POISONRESISTCAP"s, 358},
	{"ENERGYRESISTCAP"s, 359},
	{"DEFENSECHANCEINCREASECAP"s, 360},
};
constexpr auto MAX_TRACKINGTARGETS = 128;
constexpr auto SKILLTOTALCAP = 7000;
constexpr auto SKILLCAP = 1000;
constexpr auto STATCAP = 225;

constexpr auto BIT_ANNOUNCESAVES		= UI32( 0);
constexpr auto BIT_ANNOUNCEJOINPART		= UI32( 1 );
constexpr auto BIT_SERVERBACKUP			= UI32( 2 );
constexpr auto BIT_SHOOTONANIMALBACK	= UI32( 3 );
constexpr auto BIT_NPCTRAINING			= UI32( 4 );
constexpr auto BIT_LOOTDECAYSONPLAYERCORPSE	= UI32( 5 );
constexpr auto BIT_GUARDSENABLED		= UI32( 6 );
constexpr auto BIT_PLAYDEATHANIMATION	= UI32( 7 );
constexpr auto BIT_AMBIENTFOOTSTEPS		= UI32( 8 );
constexpr auto BIT_INTERNALACCOUNTS		= UI32( 9 );
constexpr auto BIT_SHOWOFFLINEPCS		= UI32( 10 );
constexpr auto BIT_ROGUESTATUS			= UI32( 11 );
constexpr auto BIT_SNOOPISCRIME			= UI32( 12 );
constexpr auto BIT_PERSECUTIONSTATUS	= UI32( 13 );
constexpr auto BIT_SELLBYNAME			= UI32( 14 );
constexpr auto BIT_TRADESYSSTATUS		= UI32( 15 );
constexpr auto BIT_RANKSYSSTATUS		= UI32( 16 );
constexpr auto BIT_CUTSCROLLREQ			= UI32( 17 );
constexpr auto BIT_SHOWHITMESSAGE		= UI32( 18 );
constexpr auto BIT_ESCORTSTATUS			= UI32( 19 );
constexpr auto BIT_MONSTERSVSANIMALS	= UI32( 20 );
constexpr auto BIT_PETHUNGEROFFLINE		= UI32( 21 );
constexpr auto BIT_HIDEWHILEMOUNTED		= UI32( 22 );
constexpr auto BIT_OVERLOADPACKETS		= UI32( 23 );
constexpr auto BIT_ARMORAFFECTMANAREGEN = UI32( 24 );
constexpr auto BIT_ANIMALSGUARDED		= UI32( 25 );
constexpr auto BIT_ADVANCEDPATHFIND		= UI32( 26 );
constexpr auto BIT_LOOTINGISCRIME		= UI32( 27 );
constexpr auto BIT_BASICTOOLTIPSONLY	= UI32( 28 );
constexpr auto BIT_GLOBALITEMDECAY		= UI32( 29 );
constexpr auto BIT_SCRIPTITEMSDECAYABLE	= UI32( 30 );
constexpr auto BIT_BASEITEMSDECAYABLE	= UI32( 31 );
constexpr auto BIT_ITEMDECAYINHOUSES	= UI32( 32 );
constexpr auto BIT_PAPERDOLLGUILDBUTTON = UI32( 33 );
constexpr auto BIT_ATTSPEEDFROMSTAMINA	= UI32( 34 );
constexpr auto BIT_SHOWDAMAGENUMBERS	= UI32( 35 );
// 37 free!
constexpr auto BIT_EXTENDEDSTARTINGSTATS	= UI32( 38 );
constexpr auto BIT_EXTENDEDSTARTINGSKILLS	= UI32( 39 );
constexpr auto BIT_ASSISTANTNEGOTIATION		= UI32( 40 );
constexpr auto BIT_KICKONASSISTANTSILENCE	= UI32( 41 );
constexpr auto BIT_CLASSICUOMAPTRACKER		= UI32( 42 );
constexpr auto BIT_PROTECTPRIVATEHOUSES		= UI32( 43 );
constexpr auto BIT_TRACKHOUSESPERACCOUNT	= UI32( 44 );
constexpr auto BIT_CANOWNANDCOOWNHOUSES		= UI32( 45 );
constexpr auto BIT_COOWNHOUSESONSAMEACCOUNT	= UI32( 46 );
constexpr auto BIT_ITEMSDETECTSPEECH		= UI32( 47 );
constexpr auto BIT_FORCENEWANIMATIONPACKET	= UI32( 48 );
constexpr auto BIT_MAPDIFFSENABLED			= UI32( 49 );
constexpr auto BIT_ARMORCLASSDAMAGEBONUS	= UI32( 50 );
constexpr auto BIT_FREESHARDSERVERPOLL		= UI32( 51 );
constexpr auto BIT_ALCHEMYDAMAGEBONUSENABLED 	= UI32( 52 );
constexpr auto BIT_PETTHIRSTOFFLINE         	= UI32( 53 );
constexpr auto BIT_USEUNICODEMESSAGES 			= UI32( 54 );
constexpr auto BIT_HUNGERSYSTEMENABLED			= UI32( 55 );
constexpr auto BIT_THIRSTSYSTEMENABLED			= UI32( 56 );
constexpr auto BIT_TRAVELSPELLSFROMBOATKEYS		= UI32( 57 );
constexpr auto BIT_TRAVELSPELLSWHILEOVERWEIGHT	= UI32( 58 );
constexpr auto BIT_MARKRUNESINMULTIS			= UI32( 59 );
constexpr auto BIT_TRAVELSPELLSBETWEENWORLDS	= UI32( 60 );
constexpr auto BIT_TRAVELSPELLSWHILEAGGRESSOR	= UI32( 61 );
[[maybe_unused]] constexpr auto BIT_CONSOLELOG	= UI32( 62 );
constexpr auto BIT_NETWORKLOG					= UI32( 63 );
constexpr auto BIT_SPEECHLOG					= UI32( 64 );
constexpr auto BIT_CONTEXTMENUS					= UI32( 65 );
constexpr auto BIT_CHECKPETCONTROLDIFFICULTY	= UI32( 66 );
constexpr auto BIT_SHOWNPCTITLESINTOOLTIPS		= UI32( 67 );
constexpr auto BIT_ITEMSINTERRUPTCASTING		= UI32( 68 );
constexpr auto BIT_STATSAFFECTSKILLCHECKS		= UI32( 69 );
constexpr auto BIT_TOOLUSELIMIT					= UI32( 70 );
constexpr auto BIT_TOOLUSEBREAK					= UI32( 71 );
constexpr auto BIT_ITEMREPAIRDURABILITYLOSS		= UI32( 72 );
constexpr auto BIT_HIDESTATSFORUNKNOWNMAGICITEMS	= UI32( 73 );
constexpr auto BIT_CRAFTCOLOUREDWEAPONS				= UI32( 74 );
constexpr auto BIT_TELEPORTTONEARESTSAFELOC			= UI32( 75 );
constexpr auto BIT_ALLOWAWAKENPCS					= UI32( 76 );
constexpr auto BIT_DISPLAYMAKERSMARK				= UI32( 77 );
constexpr auto BIT_SHOWNPCTITLESOVERHEAD			= UI32( 78 );
constexpr auto BIT_SHOWINVULNERABLETAGOVERHEAD		= UI32( 79 );
constexpr auto BIT_PETCOMBATTRAINING				= UI32( 80 );
constexpr auto BIT_HIRELINGCOMBATTRAINING			= UI32( 81 );
constexpr auto BIT_NPCCOMBATTRAINING				= UI32( 82 );
constexpr auto BIT_SHOWITEMRESISTSTATS				= UI32( 83 );
constexpr auto BIT_SHOWWEAPONDAMAGETYPES			= UI32( 84 );
constexpr auto BIT_SHOWRACEWITHNAME					= UI32( 85 );
constexpr auto BIT_SHOWRACEINPAPERDOLL				= UI32( 86 );
constexpr auto BIT_CASTSPELLSWHILEMOVING			= UI32( 87 );
constexpr auto BIT_SHOWREPTITLEINTOOLTIP			= UI32( 88 );
constexpr auto BIT_SHOWGUILDINFOINTOOLTIP			= UI32( 89 );
constexpr auto BIT_SAFECOOWNERLOGOUT				= UI32( 90 );
constexpr auto BIT_SAFEFRIENDLOGOUT					= UI32( 91 );
constexpr auto BIT_SAFEGUESTLOGOUT					= UI32( 92 );
constexpr auto BIT_KEYLESSOWNERACCESS				= UI32( 93 );
constexpr auto BIT_KEYLESSCOOWNERACCESS				= UI32( 94 );
constexpr auto BIT_KEYLESSFRIENDACCESS				= UI32( 95 );
constexpr auto BIT_KEYLESSGUESTACCESS				= UI32( 96 );
constexpr auto BIT_OFFERBODSFROMITEMSALES			= UI32( 97 );
constexpr auto BIT_OFFERBODSFROMCONTEXTMENU			= UI32( 98 );
constexpr auto BIT_BODSFROMCRAFTEDITEMSONLY			= UI32( 99 );
constexpr auto BIT_ENABLENPCGUILDDISCOUNTS			= UI32( 100 );
constexpr auto BIT_ENABLENPCGUILDPREMIUMS			= UI32( 101 );
constexpr auto BIT_SNOOPAWARENESS					= UI32( 102 );
constexpr auto BIT_YOUNGPLAYERSYSTEM				= UI32( 103 );
constexpr auto BIT_AUTOUNEQUIPPEDCASTING			= UI32( 104 );
constexpr auto BIT_LOOTDECAYSONNPCCORPSE			= UI32( 105 );


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
//									GLOBALITEMDECAY - Toggles on / off item decay on a global scale.Note that it does not remove the decay - status from items, it just resets the decay - timer when it reaches 0
//									SCRIPTITEMSDECAYABLE - Toggles whether DFN - items will decay by default or not.Can be overriden by DECAY tag in item - DFNs
//									BASEITEMSDECAYABLE - Toggles whether base - items will decay by default or not.Can be overriden by DECAY tag in harditems.dfn
//									ITEMDECAYINHOUSES - Toggles default decay of non - locked down items inside multis( houses and boats )
// December 14, 20011 	- 	Exposed CombatExplodeDelay to UOX.INI, and changed the default delay between targeting and damage for the Explosion spell from 0 to 2 seconds
// March 19, 2012 		- 	Added support for packet 0xD7, SubCommand 0x28 - Guild button on paperdoll, which gives access to guild-functions if character belongs to a guild.
//									Can be enabled / disabled through UOX.INI setting PAPERDOLLGUILDBUTTON = 0 / 1. Defaults to 0.
//							Added new UOX.INI option to determine whether attack - speed bonuses are gained from Stamina( default ) or Dexterity: ATTACKSPEEDFROMSTAMINA = 1 / 0
//							Added new UOX.INI option to control the displaying of damage - numbers in combat( previously used DISPLAYHITMSG for numbers too ) : DISPLAYDAMAGENUMBERS = 1 / 0
//							Added a new section to UOX.INI - [clientsupport] - along with the following settings to determine approved client - versions for the server :
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
//							Added new UOX.INI option : EXTENDEDSTARTINGSTATS = 0 / 1
//									If enabled, makes new characters start with 90 statpoints( selectable in clients 7.0.16.0 + only, lower versions only get 90 if using templates ) instead of 80.
//							Added new UOX.INI option : EXTENDEDSTARTINGSKILLS = 0 / 1
//									If enabled, allows for four starting skills( selectable in clients 7.0.16.0 + only, lower versions only get 4th skill if using templates ) instead of three
// November 14, 2012	-	Fixed issue where DISPLAYDAMAGENUMBERS and ATTACKSPEEDFROMSTAMINA settings in UOX.INI were overwriting the DISPLAYHITMSG setting instead of their own respective settings
// November 10, 2013	-	Added new UOX.INI settings for adjusting combat-speed on a global scale:
//									GLOBALATTACKSPEED = 1 	// for adjusting speed of melee and ranged attacks globally for all chars
//									NPCSPELLCASTSPEED = 1 	// for adjusting the overall speed of spellcasts for NPCs (base spellcast speed determined by SPADELAY tag in NPC dfns)
//							Added new UOX.INI settings to make hardcoded damage to armor and weapons in combat more customizable :
//									WEAPONDAMAGECHANCE = 17 	// Chance of weapons taking damage when attacking
//									WEAPONDAMAGEMIN = 0		// Minimum amount of damage a weapon takes if damaged in combat
//									WEAPONDAMAGEMAX = 1		// Maximum amount of damage a weapon takes if damaged in combat
//									ARMORDAMAGECHANCE = 25	// Chance of armor taking damage when defending
//									ARMORDAMAGEMIN = 0		// Minimum amount of damage armor can take if damaged in combat
//									ARMORDAMAGEMAX = 1		// Maximum amount of damage armor can take if damaged in combat
// September 12, 2015 	- 	Added new UOX.INI setting for defining the amount of stamina lost when using the fishing skill
//									FISHINGSTAMINALOSS = 2	// The amount of stamina lost when using the fishing skill
// November 24, 2019 	- 	Added new uox.ini flag for choosing a random starting location for new players (from list in uox.ini)
//									RANDOMSTARTINGLOCATION = 0	// 0 to disable (default), 1 to enable

// NOTE:	Very important the first lookups required duplication or the search fails on them

//+++++++++++++++++++++++++++++++++++++++++++++++
auto CServerData::LookupINIValue( const std::string& tag ) -> SI32
{
	auto iter = uox3IniCaseValue.find( tag );
	if( iter != uox3IniCaseValue.end() )
	{
		return iter->second;
	}
	return std::numeric_limits<SI32>::max();
}


//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ResetDefaults()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Reset server settings to default
//o------------------------------------------------------------------------------------------------o
auto CServerData::ResetDefaults() -> void
{
	resettingDefaults = true;
	serverList.resize( 1 );		// Set the initial count to hold one server.

	ServerIP( "127.0.0.1" );
	ServerPort( 2593 );
	ExternalIP( "127.0.0.1" );
	serverList[0].SetPort( 2593 );
	ServerName( "My UOX3 Shard" );
	SecretShardKey( "None" );

	// Set default gcMaxBytes limit in MB per JS runtime
	// If set too low, UOX3 might crash when reloading (full) JS engine
	// JS API references describe it as "Maximum nominal heap before last ditch GC"
	SetJSEngineSize( 256 );

	// Send server-originating messages in Unicode format, if possible
	UseUnicodeMessages( true );

	ServerLanguage( DL_DEFAULT );
	SystemTimer( tSERVER_POTION, 10 );
	ServerMoon( 0, 0 );
	ServerMoon( 1, 0 );
	DungeonLightLevel( 15 );
	WorldLightCurrentLevel( 0 );
	WorldLightBrightLevel( 0 );
	WorldLightDarkLevel( 12 );

	ServerNetRcvTimeout( 3 );
	ServerNetSndTimeout( 3 );
	ServerNetRetryCount( 3 );
	MaxClientBytesIn( 50000 );
	MaxClientBytesOut( 200000 );
	NetTrafficTimeban( 30 );

	// Adaptive Performance System
	APSPerfThreshold( 50 ); // Default to 50 simulation cycles as performance threshold
	APSInterval( 100 ); // Default to APS checking performance at 250 ms intervals
	APSDelayStep( 50 ); // Default to 25 ms as the value at which APS gradually increments the delay to NPC AI/movement checking
	APSDelayMaxCap( 2000 ); // Default to 1500 ms as max cap on delay APS can introduce to NPC AI/movement checking

	ServerSecondsPerUOMinute( 5 );
	ServerTimeDay( 0 );
	ServerTimeHours( 0 );
	ServerTimeMinutes( 0 );
	ServerTimeSeconds( 0 );
	ServerTimeAMPM( 0 );

	InternalAccountStatus( true );
	YoungPlayerSystem( true );
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
	PlayerCorpseLootDecay( true );
	NpcCorpseLootDecay( true );
	ServerSavesTimer( 600 );

	// Enable login-support only for latest available client by default
	ClientSupport4000( false );
	ClientSupport5000( false );
	ClientSupport6000( false );
	ClientSupport6050( false );
	ClientSupport7000( false );
	ClientSupport7090( false );
	ClientSupport70160( false );
	ClientSupport70240( false );
	ClientSupport70300( false );
	ClientSupport70331( false );
	ClientSupport704565( false );
	ClientSupport70610( true );

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
	SnoopIsCrime( false );
	SnoopAwareness( false );
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
	FreeshardServerPoll( true );
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
	CombatNpcDamageRate( 1 );
	RankSystemStatus( true );
	DisplayMakersMark( true );
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
	FishingStaminaLoss( 2 );
	PhysicalResistCap( 70 );
	FireResistCap( 70 );
	ColdResistCap( 70 );
	PoisonResistCap( 70 );
	EnergyResistCap( 70 );
	DefenseChanceIncreaseCap( 45 );
	CombatArmorClassDamageBonus( false );
	AlchemyDamageBonusEnabled( false );
	AlchemyDamageBonusModifier( 5 );
	PetCombatTraining( true );
	HirelingCombatTraining( true );
	NpcCombatTraining( false );
	WeaponDamageBonusType( 2 );

	CheckPetControlDifficulty( true );
	SetPetLoyaltyGainOnSuccess( 1 );
	SetPetLoyaltyLossOnFailure( 3 );
	SystemTimer( tSERVER_LOYALTYRATE, 900 );
	ItemsInterruptCasting( true );

	auto curWorkingDir = std::filesystem::current_path().string();

	auto wDir = oldstrutil::fixDirectory( curWorkingDir );
	std::string tDir;
	Directory( CSDDP_ROOT, wDir );
	tDir = wDir + std::string( "muldata/" );
	Directory( CSDDP_DATA, tDir );
	tDir = wDir + std::string( "dfndata/" );
	Directory( CSDDP_DEFS, tDir );
	tDir = wDir + std::string( "accounts/" );
	Directory( CSDDP_ACCOUNTS, tDir );
	Directory( CSDDP_ACCESS, tDir );
	tDir = wDir + std::string( "js/" );
	Directory( CSDDP_SCRIPTS, tDir );
	tDir = wDir + std::string( "js/jsdata" );
	Directory( CSDDP_SCRIPTDATA, tDir );
	tDir = wDir + std::string( "archives/" );
	Directory( CSDDP_BACKUP, tDir );
	tDir = wDir + std::string( "msgboards/" );
	Directory( CSDDP_MSGBOARD, tDir );
	tDir = wDir + std::string( "shared/" );
	Directory( CSDDP_SHARED, tDir );
	tDir = wDir + std::string( "html/" );
	Directory( CSDDP_HTML, tDir );
	tDir = wDir + std::string( "books/" );
	Directory( CSDDP_BOOKS, tDir );
	tDir = wDir + std::string( "dictionaries/" );
	Directory( CSDDP_DICTIONARIES, tDir );
	tDir = wDir + std::string( "logs/" );
	Directory( CSDDP_LOGS, tDir );

	// Expansion settings
	ExpansionCoreShardEra( ER_LBR ); // Default to LBR expansion
	ExpansionArmorCalculation( ER_CORE );
	ExpansionStrengthDamageBonus( ER_CORE );
	ExpansionTacticsDamageBonus( ER_CORE );
	ExpansionAnatomyDamageBonus( ER_CORE );
	ExpansionLumberjackDamageBonus( ER_CORE );
	ExpansionRacialDamageBonus( ER_CORE );
	ExpansionDamageBonusCap( ER_CORE );
	ExpansionShieldParry( ER_CORE );
	ExpansionWeaponParry( ER_CORE );
	ExpansionWrestlingParry( ER_CORE );
	ExpansionCombatHitChance( ER_CORE );

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
	SystemTimer( tSERVER_NPCFLAGUPDATETIMER, 10 );
	PetOfflineTimeout( 5 );
	PetHungerOffline( true );
	SystemTimer( tSERVER_PETOFFLINECHECK, 600 );
	ItemsDetectSpeech( false );
	MaxPlayerPackItems( 125 );
	MaxPlayerBankItems( 125 );
	MaxPlayerPackWeight( 40000 ); // 400.00 stones
	MaxPlayerBankWeight( 160000 ); // 1600.00 stones
	ForceNewAnimationPacket( true );
	MapDiffsEnabled( false );
	TravelSpellsFromBoatKeys( true );
	TravelSpellsWhileOverweight( false );
	MarkRunesInMultis( true );
	TravelSpellsBetweenWorlds( false );
	TravelSpellsWhileAggressor( false );
	CastSpellsWhileMoving( false );
	AutoUnequippedCasting( false );
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
	AllowAwakeNPCs( true );
	GlobalRestockMultiplier( 1.0 );
	ShowItemResistStats( false );
	ShowWeaponDamageTypes( true );
	ShowReputationTitleInTooltip( true );
	ShowGuildInfoInTooltip( true );
	EnableNPCGuildDiscounts( true );
	EnableNPCGuildPremiums( true );

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
	ShowNpcTitlesOverhead( true );
	ShowInvulnerableTagOverhead( false );
	ShowRaceWithName( true );
	ShowRaceInPaperdoll( true );
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
	SystemTimer( tSERVER_MURDERDECAY, 28800 );
	SystemTimer( tSERVER_AGGRESSORFLAG, 120 );
	SystemTimer( tSERVER_PERMAGREYFLAG, 0 ); // No duration by default
	SystemTimer( tSERVER_STEALINGFLAG, 120 );
	SystemTimer( tSERVER_COMBATIGNORE, 20 ); // Not in ini

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
	SafeCoOwnerLogout( true );
	SafeFriendLogout( true );
	SafeGuestLogout( true );
	KeylessOwnerAccess( true );
	KeylessCoOwnerAccess( true );
	KeylessFriendAccess( true );
	KeylessGuestAccess( false );

	// Bulk Order Deeds
	OfferBODsFromItemSales( true );
	OfferBODsFromContextMenu( false );
	BODsFromCraftedItemsOnly( false );
	BODGoldRewardMultiplier( 1.0 );
	BODFameRewardMultiplier( 1.0 );

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

    // Enable Felucca by default
    SetMoongateFacetStatus( 0, true );

    // Enable Trammel by default
    SetMoongateFacetStatus( 1, true );

    // Enable Ilshenar by default
    SetMoongateFacetStatus( 2, true );

	// Set no assistant features as disabled by default
	SetDisabledAssistantFeature( AF_ALL, false );

	ExtendedStartingStats( true );
	ExtendedStartingSkills( true );

	ServerRandomStartingLocation( false );
	ServerStartGold( 1000 );
	ServerStartPrivs( 0 );
	SystemTimer( tSERVER_CORPSEDECAY, 420 );
	SystemTimer( tSERVER_NPCCORPSEDECAY, 420 );
	SystemTimer( tSERVER_BLOODDECAYCORPSE, 210 ); // Default to half the decay timer of a npc corpse
	SystemTimer( tSERVER_BLOODDECAY, 3 ); // Keep it short and sweet
	startlocations.clear();
	youngStartlocations.clear();
	resettingDefaults = false;
}
//==================================================================================================
CServerData::CServerData()
{
	Startup();
	availableIPs = ip4list_t::available();
}
//==================================================================================================
auto CServerData::Startup() -> void
{
	ResetDefaults();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ServerName()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets server name. Sets to specified value, or to default name if no value specified
//o------------------------------------------------------------------------------------------------o
auto CServerData::ServerName() const -> const std::string&
{
	return serverList[0].GetName();
}
auto CServerData::ServerName( const std::string &setname ) -> void
{
	if( serverList.empty() )
	{
		serverList.resize( 1 );
	}
	serverList[0].SetName( setname );
	if( setname.empty() )
	{
		serverList[0].SetName( "My UOX3 Shard" );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::SecretShardKey()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets secret shard key used to only allows connections from specific
//|					custom clients that provide a matching secret key
//o------------------------------------------------------------------------------------------------o
auto CServerData::SecretShardKey() const -> const std::string&
{
	return secretShardKey;
}
auto CServerData::SecretShardKey( const std::string &newName ) -> void
{
	if( !newName.empty() )
	{
		secretShardKey = newName;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ServerDomain()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets server domain. Sets to specified value, or to no domain if no value specified
//o------------------------------------------------------------------------------------------------o
auto CServerData::ServerDomain() const -> const std::string&
{
	return serverList[0].GetDomain();
}
auto CServerData::ServerDomain( const std::string &setdomain ) -> void
{
	if( serverList.empty() )
	{
		serverList.resize( 1 );
	}
	if( setdomain.empty() )
	{
		serverList[0].SetDomain( "" );
	}
	else
	{
		serverList[0].SetDomain( setdomain );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ServerIP()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets server IP. Sets to specified value, or to loopback IP if no value specified
//o------------------------------------------------------------------------------------------------o
auto CServerData::ServerIP() const -> const std::string &
{
	return serverList[0].GetIP();
}
auto CServerData::ServerIP( const std::string &setip ) -> void
{
	if( serverList.empty() )
	{
		serverList.resize( 1 );
	}
	if( setip.empty() )
	{
		serverList[0].SetIP("127.0.0.1" );
	}
	else
	{
		serverList[0].SetIP( setip );
	}
}
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ExternalIP()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets external (WAN) IP for server
//o------------------------------------------------------------------------------------------------o
auto CServerData::ExternalIP() const -> const std::string&
{
	return externalIP;
}
auto CServerData::ExternalIP( const std::string &ip ) -> void
{
	externalIP = ip;
	//IP4Address::setExternal( externalIP );
}
auto CServerData::matchIP( const Ip4Addr_st &ip ) const -> Ip4Addr_st
{
	auto [candidate, match] = availableIPs.bestmatch( ip );
	if( match == 0 )
	{
		if( !externalIP.empty() )
		{
			candidate = Ip4Addr_st( externalIP );
		}
	}
	else
	{
		// We got some kind of match, see if on same network type?
		if( candidate.type() != ip.type() ) 
		{
			if( ip.type() == Ip4Addr_st::ip4type_t::wan )
			{
				if( !externalIP.empty() )
				{
					candidate = Ip4Addr_st( externalIP );
				}
			}
		}
	}
	return candidate;	
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ServerPort()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Set server port to specified value, or to default port if no value specified
//o------------------------------------------------------------------------------------------------o
auto CServerData::ServerPort() const -> UI16
{
	return port;
}
auto CServerData::ServerPort( UI16 setport ) -> void
{
	if( setport == 0 )
	{
		port = 2593;
	}
	else
	{
		port = setport;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ServerConsoleLog()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Set logging of console messages, warnings and errors
//o------------------------------------------------------------------------------------------------o
auto CServerData::ServerConsoleLog() const -> bool
{
	return consoleLogEnabled;
}
auto CServerData::ServerConsoleLog( bool setting ) -> void
{
	consoleLogEnabled = setting;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ServerNetworkLog()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets logging of network traffic to logs folder
//o------------------------------------------------------------------------------------------------o
auto CServerData::ServerNetworkLog() const -> bool
{
	return boolVals.test( BIT_NETWORKLOG );
}
auto CServerData::ServerNetworkLog( bool newVal ) -> void
{
	boolVals.set( BIT_NETWORKLOG, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ServerSpeechLog()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets logging of player/staff speech to logs folder
//o------------------------------------------------------------------------------------------------o
auto CServerData::ServerSpeechLog() const -> bool
{
	return boolVals.test( BIT_SPEECHLOG );
}
auto CServerData::ServerSpeechLog( bool newVal ) -> void
{
	boolVals.set( BIT_SPEECHLOG, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ServerCommandPrefix()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Set command prefix
//o------------------------------------------------------------------------------------------------o
auto CServerData::ServerCommandPrefix() const -> char
{
	return commandprefix.empty() ? 0 : commandprefix[0];
}
auto CServerData::ServerCommandPrefix( char cmdvalue ) -> void
{
	commandprefix = std::string( 1, cmdvalue );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ServerAnnounceSavesStatus()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Set status of server accouncements for worldsaves
//o------------------------------------------------------------------------------------------------o
auto CServerData::ServerAnnounceSavesStatus() const -> bool
{
	return boolVals.test( BIT_ANNOUNCESAVES );
}
auto CServerData::ServerAnnounceSaves( bool newVal ) -> void
{
	boolVals.set( BIT_ANNOUNCESAVES, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ServerJoinPartAnnouncementsStatus()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets status of server accouncements for players connecting/disconnecting
//o------------------------------------------------------------------------------------------------o
auto CServerData::ServerJoinPartAnnouncementsStatus() const -> bool
{
	return boolVals.test( BIT_ANNOUNCEJOINPART );
}
auto CServerData::ServerJoinPartAnnouncements( bool newVal ) -> void
{
	boolVals.set( BIT_ANNOUNCEJOINPART, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ServerBackupStatus()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets enabled status of server worldsave backups
//o------------------------------------------------------------------------------------------------o
auto CServerData::ServerBackupStatus() const -> bool
{
	return boolVals.test( BIT_SERVERBACKUP );
}
auto CServerData::ServerBackups( bool newVal ) -> void
{
	boolVals.set( BIT_SERVERBACKUP, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ServerContextMenus()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets enabled status of context menus
//o------------------------------------------------------------------------------------------------o
auto CServerData::ServerContextMenus() const -> bool
{
	return boolVals.test( BIT_CONTEXTMENUS );
}
auto CServerData::ServerContextMenus( bool newVal ) -> void
{
	boolVals.set( BIT_CONTEXTMENUS, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::SysMsgColour()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default text colour for system messages displayed in bottom left
//o------------------------------------------------------------------------------------------------o
auto CServerData::SysMsgColour() const -> UI16
{
	return sysMsgColour;
}
auto CServerData::SysMsgColour( UI16 value ) -> void
{
	sysMsgColour = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ServerSavesTimerStatus()
//|					CServerData::ServerSavesTimer()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets server world save timer
//o------------------------------------------------------------------------------------------------o
auto CServerData::ServerSavesTimerStatus() const -> UI32
{
	return serverSavesTimer;
}
auto CServerData::ServerSavesTimer( UI32 timer ) -> void
{
	serverSavesTimer = timer;
	if( timer < 180 )			// 3 minutes is the lowest value you can set saves for
	{
		serverSavesTimer = 300;	// 10 minutes default
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ServerSkillCapStatus() 
//|					CServerData::ServerSkillCap()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets cap for individual skills
//o------------------------------------------------------------------------------------------------o
auto CServerData::ServerSkillCapStatus() const -> UI16
{
	return skillCap;
}
auto CServerData::ServerSkillCap( UI16 cap ) -> void
{
	skillCap = cap;
	if( cap < 1 )		// Default is on second loop sleeping
	{
		skillCap = SKILLCAP;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ServerSkillTotalCapStatus()
//|					CServerData::ServerSkillTotalCap()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets total cap for all skills
//o------------------------------------------------------------------------------------------------o
auto CServerData::ServerSkillTotalCapStatus() const -> UI16
{
	return skillTotalCap;
}
auto CServerData::ServerSkillTotalCap( UI16 cap ) -> void
{
	skillTotalCap = cap;
	if( cap < 1 )		// Default is on second loop sleeping
	{
		skillTotalCap = SKILLTOTALCAP;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ServerSkillDelayStatus()
//|					CServerData::ServerSkillDelay()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global delay for skill usage
//o------------------------------------------------------------------------------------------------o
auto CServerData::ServerSkillDelayStatus() const -> UI08
{
	return skillDelay;
}
auto CServerData::ServerSkillDelay( UI08 skdelay ) -> void
{
	skillDelay = skdelay;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ServerStatCapStatus()
//|					CServerData::ServerStatCap()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the total stat cap
//o------------------------------------------------------------------------------------------------o
auto CServerData::ServerStatCapStatus() const -> UI16
{
	return statCap;
}
auto CServerData::ServerStatCap( UI16 cap ) -> void
{
	statCap = cap;
	if( cap < 1 )		// Default is on second loop sleeping
	{
		statCap = STATCAP;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::MaxStealthMovement()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max number of steps allowed to take while stealthed
//o------------------------------------------------------------------------------------------------o
auto CServerData::MaxStealthMovement() const -> SI16
{
	return maxStealthMovement;
}
auto CServerData::MaxStealthMovement( SI16 value ) -> void
{
	maxStealthMovement = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::MaxStaminaMovement()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max number of steps allowed while running before stamina is reduced
//o------------------------------------------------------------------------------------------------o
auto CServerData::MaxStaminaMovement() const -> SI16
{
	return maxStaminaMovement;
}
auto CServerData::MaxStaminaMovement( SI16 value ) -> void
{
	maxStaminaMovement = value;
}

auto CServerData::BuildSystemTimeValue( cSD_TID timerId ) const ->TIMERVAL
{
	return BuildTimeValue( static_cast<R32>( SystemTimer( timerId )));
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::SystemTimer()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets a specific server timer
//o------------------------------------------------------------------------------------------------o
auto CServerData::SystemTimer( cSD_TID timerid ) const -> UI16
{
	return serverTimers[timerid];
}
auto CServerData::SystemTimer( cSD_TID timerid, UI16 value ) -> void
{
	serverTimers[timerid] = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Directory()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets directory paths
//o------------------------------------------------------------------------------------------------o
auto CServerData::Directory( CSDDirectoryPaths dp ) -> std::string
{
	std::string rValue;
	if( dp != CSDDP_COUNT )
	{
		rValue = serverDirectories[dp];
	}
	return rValue;
}
auto CServerData::Directory( CSDDirectoryPaths dp, std::string value ) -> void
{
	bool create_dir = false;

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
			case CSDDP_SCRIPTDATA:		verboseDirectory = "ScriptData directory";
				create_dir = true;
				break;
			case CSDDP_BACKUP:			verboseDirectory = "Backup directory";			break;
			case CSDDP_MSGBOARD:		verboseDirectory = "Messageboard directory";	break;
			case CSDDP_SHARED:			verboseDirectory = "Shared directory";
				create_dir = true;
				break;
			case CSDDP_HTML:			verboseDirectory = "HTML directory";			break;
			case CSDDP_LOGS:			verboseDirectory = "Logs directory";			break;
			case CSDDP_DICTIONARIES:	verboseDirectory = "Dictionary directory";		break;
			case CSDDP_BOOKS:			verboseDirectory = "Books directory";			break;
			case CSDDP_COUNT:
			default:					verboseDirectory = "Unknown directory";			break;
		};
		// First, let's normalize the path name and fix common errors
		// remove all trailing and leading spaces...
		auto sText = oldstrutil::trim( value );

		if( sText.empty() )
		{
			Console.Error( oldstrutil::format( " %s directory is blank, set in uox.ini", verboseDirectory.c_str() ));
			Shutdown( FATAL_UOX3_DIR_NOT_FOUND );
		}
		else
		{
			// Make sure we're terminated with a directory separator
			// Just incase it's not set in the .ini
			// and convert the windows backward slashes to forward slashes
			
			sText = oldstrutil::fixDirectory( sText );
			
			bool error = false;
			if( !resettingDefaults )
			{
				error = true;
				//auto curWorkingDir = fixDirectory( std::filesystem::current_path().string() );
				
				auto npath = std::filesystem::path( sText );
				
				if( std::filesystem::exists( npath ))
				{
					error = false;
				}
				else if( create_dir )
				{
					// Create missing directory
					std::filesystem::create_directory( npath );
					error = false;
				}
			}
			
			if( error )
			{
				Console.Error( oldstrutil::format( "%s %s does not exist", verboseDirectory.c_str(), sText.c_str() ));
				Shutdown( FATAL_UOX3_DIR_NOT_FOUND );
			}
			else
			{
				// There was a check to see if text was empty, to set to "./".  However, if text was blank, we bailed out in the
				// beginning of the routine
				serverDirectories[dp] = sText;
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ExpansionCoreShardEra()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Set core era ruleset for shard
//|	Notes		-		0 = Core (used to inherit CoreShardEra setting)
//|						1 = UO - Original UO release
//|						2 = T2A - The Second Age
//|						3 = UOR - Rennaissance
//|						4 = TD - Third Dawn
//|						5 = LBR - Lord Blackthorn's Revenge
//|						6 = AoS - Age of Shadows
//|						7 = SE - Samurai Empire
//|						8 = ML - Mondain's Legacy
//|						9 = SA - Stygian Abyss
//|						10 = HS - High Seas
//|						11 = ToL - Time of Legends
//o------------------------------------------------------------------------------------------------o
auto CServerData::ExpansionCoreShardEra( void ) const -> UI08
{
	return coreShardEra;
}
auto CServerData::ExpansionCoreShardEra( UI08 setting ) -> void
{
	if( setting >= ER_COUNT )
	{
		setting = ER_COUNT - 1;
	}
	coreShardEra = setting;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ExpansionArmorCalculation()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Set era ruleset for armor/defense calculation
//|	Notes		-		0 = Core (used to inherit CoreShardEra setting)
//|						1 = UO - Original UO release
//|						2 = T2A - The Second Age
//|						3 = UOR - Rennaissance
//|						4 = TD - Third Dawn
//|						5 = LBR - Lord Blackthorn's Revenge
//|						6 = AoS - Age of Shadows
//|						7 = SE - Samurai Empire
//|						8 = ML - Mondain's Legacy
//|						9 = SA - Stygian Abyss
//|						10 = HS - High Seas
//|						11 = ToL - Time of Legends
//o------------------------------------------------------------------------------------------------o
auto CServerData::ExpansionArmorCalculation() const -> UI08
{
	if( static_cast<ExpansionRuleset>( expansionArmorCalculation ) == ER_CORE )
	{
		// Inherit CoreShardEra setting
		return ExpansionCoreShardEra();
	}
	return expansionArmorCalculation;
}
auto CServerData::ExpansionArmorCalculation( UI08 setting ) -> void
{
	if( setting >= ER_COUNT )
	{
		setting = ER_COUNT - 1;
	}
	else if( static_cast<ExpansionRuleset>( setting ) == ER_CORE )
	{
		setting = ExpansionCoreShardEra();
	}
	expansionArmorCalculation = setting;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ExpansionStrengthDamageBonus()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Set era ruleset for strength damage bonus
//|	Notes		-		0 = Core (used to inherit CoreShardEra setting)
//|						1 = UO - Original UO release
//|						2 = T2A - The Second Age
//|						3 = UOR - Rennaissance
//|						4 = TD - Third Dawn
//|						5 = LBR - Lord Blackthorn's Revenge
//|						6 = AoS - Age of Shadows
//|						7 = SE - Samurai Empire
//|						8 = ML - Mondain's Legacy
//|						9 = SA - Stygian Abyss
//|						10 = HS - High Seas
//|						11 = ToL - Time of Legends
//o------------------------------------------------------------------------------------------------o
auto CServerData::ExpansionStrengthDamageBonus() const -> UI08
{
	if( static_cast<ExpansionRuleset>( expansionStrengthDamageBonus ) == ER_CORE )
	{
		// Inherit CoreShardEra setting
		return ExpansionCoreShardEra();
	}
	return expansionStrengthDamageBonus;
}
auto CServerData::ExpansionStrengthDamageBonus( UI08 setting ) -> void
{
	if( setting >= ER_COUNT )
	{
		setting = ER_COUNT - 1;
	}
	expansionStrengthDamageBonus = setting;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ExpansionTacticsDamageBonus()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Set era ruleset for tactics damage bonus
//|	Notes		-		0 = Core (used to inherit CoreShardEra setting)
//|						1 = UO - Original UO release
//|						2 = T2A - The Second Age
//|						3 = UOR - Rennaissance
//|						4 = TD - Third Dawn
//|						5 = LBR - Lord Blackthorn's Revenge
//|						6 = AoS - Age of Shadows
//|						7 = SE - Samurai Empire
//|						8 = ML - Mondain's Legacy
//|						9 = SA - Stygian Abyss
//|						10 = HS - High Seas
//|						11 = ToL - Time of Legends
//o------------------------------------------------------------------------------------------------o
auto CServerData::ExpansionTacticsDamageBonus() const -> UI08
{
	if( static_cast<ExpansionRuleset>( expansionTacticsDamageBonus ) == ER_CORE )
	{
		// Inherit CoreShardEra setting
		return ExpansionCoreShardEra();
	}
	return expansionTacticsDamageBonus;
}
auto CServerData::ExpansionTacticsDamageBonus( UI08 setting ) -> void
{
	if( setting >= ER_COUNT )
	{
		setting = ER_COUNT - 1;
	}
	expansionTacticsDamageBonus = setting;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ExpansionAnatomyDamageBonus()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Set era ruleset for anatomy damage bonus
//|	Notes		-		0 = Core (used to inherit CoreShardEra setting)
//|						1 = UO - Original UO release
//|						2 = T2A - The Second Age
//|						3 = UOR - Rennaissance
//|						4 = TD - Third Dawn
//|						5 = LBR - Lord Blackthorn's Revenge
//|						6 = AoS - Age of Shadows
//|						7 = SE - Samurai Empire
//|						8 = ML - Mondain's Legacy
//|						9 = SA - Stygian Abyss
//|						10 = HS - High Seas
//|						11 = ToL - Time of Legends
//o------------------------------------------------------------------------------------------------o
auto CServerData::ExpansionAnatomyDamageBonus() const -> UI08
{
	if( static_cast<ExpansionRuleset>( expansionAnatomyDamageBonus ) == ER_CORE )
	{
		// Inherit CoreShardEra setting
		return ExpansionCoreShardEra();
	}
	return expansionAnatomyDamageBonus;
}
auto CServerData::ExpansionAnatomyDamageBonus( UI08 setting ) -> void
{
	if( setting >= ER_COUNT )
	{
		setting = ER_COUNT - 1;
	}
	expansionAnatomyDamageBonus = setting;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ExpansionLumberjackDamageBonus()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Set era ruleset for lumberjack damage bonus
//|	Notes		-		0 = Core (used to inherit CoreShardEra setting)
//|						1 = UO - Original UO release
//|						2 = T2A - The Second Age
//|						3 = UOR - Rennaissance
//|						4 = TD - Third Dawn
//|						5 = LBR - Lord Blackthorn's Revenge
//|						6 = AoS - Age of Shadows
//|						7 = SE - Samurai Empire
//|						8 = ML - Mondain's Legacy
//|						9 = SA - Stygian Abyss
//|						10 = HS - High Seas
//|						11 = ToL - Time of Legends
//o------------------------------------------------------------------------------------------------o
auto CServerData::ExpansionLumberjackDamageBonus() const -> UI08
{
	if( static_cast<ExpansionRuleset>( expansionLumberjackDamageBonus ) == ER_CORE )
	{
		// Inherit CoreShardEra setting
		return ExpansionCoreShardEra();
	}
	return expansionLumberjackDamageBonus;
}
auto CServerData::ExpansionLumberjackDamageBonus( UI08 setting ) -> void
{
	if( setting >= ER_COUNT )
	{
		setting = ER_COUNT - 1;
	}
	expansionLumberjackDamageBonus = setting;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ExpansionRacialDamageBonus()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Set era ruleset for racial damage bonus
//|	Notes		-		0 = Core (used to inherit CoreShardEra setting)
//|						1 = UO - Original UO release
//|						2 = T2A - The Second Age
//|						3 = UOR - Rennaissance
//|						4 = TD - Third Dawn
//|						5 = LBR - Lord Blackthorn's Revenge
//|						6 = AoS - Age of Shadows
//|						7 = SE - Samurai Empire
//|						8 = ML - Mondain's Legacy
//|						9 = SA - Stygian Abyss
//|						10 = HS - High Seas
//|						11 = ToL - Time of Legends
//o------------------------------------------------------------------------------------------------o
auto CServerData::ExpansionRacialDamageBonus() const -> UI08
{
	if( static_cast<ExpansionRuleset>( expansionRacialDamageBonus ) == ER_CORE )
	{
		// Inherit CoreShardEra setting
		return ExpansionCoreShardEra();
	}
	return expansionRacialDamageBonus;
}
auto CServerData::ExpansionRacialDamageBonus( UI08 setting ) -> void
{
	if( setting >= ER_COUNT )
	{
		setting = ER_COUNT - 1;
	}
	expansionRacialDamageBonus = setting;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ExpansionDamageBonusCap()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Set era ruleset for cap applied to damage bonus
//|	Notes		-		0 = Core (used to inherit CoreShardEra setting)
//|						1 = UO - Original UO release
//|						2 = T2A - The Second Age
//|						3 = UOR - Rennaissance
//|						4 = TD - Third Dawn
//|						5 = LBR - Lord Blackthorn's Revenge
//|						6 = AoS - Age of Shadows
//|						7 = SE - Samurai Empire
//|						8 = ML - Mondain's Legacy
//|						9 = SA - Stygian Abyss
//|						10 = HS - High Seas
//|						11 = ToL - Time of Legends
//o------------------------------------------------------------------------------------------------o
auto CServerData::ExpansionDamageBonusCap() const -> UI08
{
	if( static_cast<ExpansionRuleset>( expansionDamageBonusCap ) == ER_CORE )
	{
		// Inherit CoreShardEra setting
		return ExpansionCoreShardEra();
	}
	return expansionDamageBonusCap;
}
auto CServerData::ExpansionDamageBonusCap( UI08 setting ) -> void
{
	if( setting >= ER_COUNT )
	{
		setting = ER_COUNT - 1;
	}
	expansionDamageBonusCap = setting;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ExpansionShieldParry()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Set era ruleset for shield parry
//|	Notes		-		0 = Core (used to inherit CoreShardEra setting)
//|						1 = UO - Original UO release
//|						2 = T2A - The Second Age
//|						3 = UOR - Rennaissance
//|						4 = TD - Third Dawn
//|						5 = LBR - Lord Blackthorn's Revenge
//|						6 = AoS - Age of Shadows
//|						7 = SE - Samurai Empire
//|						8 = ML - Mondain's Legacy
//|						9 = SA - Stygian Abyss
//|						10 = HS - High Seas
//|						11 = ToL - Time of Legends
//o------------------------------------------------------------------------------------------------o
auto CServerData::ExpansionShieldParry() const -> UI08
{
	if( static_cast<ExpansionRuleset>( expansionShieldParry ) == ER_CORE )
	{
		// Inherit CoreShardEra setting
		return ExpansionCoreShardEra();
	}
	return expansionShieldParry;
}
auto CServerData::ExpansionShieldParry( UI08 setting ) -> void
{
	if( setting >= ER_COUNT )
	{
		setting = ER_COUNT - 1;
	}
	expansionShieldParry = setting;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ExpansionWeaponParry()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Set era ruleset for weapon parry
//|	Notes		-		0 = Core (used to inherit CoreShardEra setting)
//|						1 = UO - Original UO release
//|						2 = T2A - The Second Age
//|						3 = UOR - Rennaissance
//|						4 = TD - Third Dawn
//|						5 = LBR - Lord Blackthorn's Revenge
//|						6 = AoS - Age of Shadows
//|						7 = SE - Samurai Empire
//|						8 = ML - Mondain's Legacy
//|						9 = SA - Stygian Abyss
//|						10 = HS - High Seas
//|						11 = ToL - Time of Legends
//o------------------------------------------------------------------------------------------------o
auto CServerData::ExpansionWeaponParry() const -> UI08
{
	if( static_cast<ExpansionRuleset>( expansionWeaponParry ) == ER_CORE )
	{
		// Inherit CoreShardEra setting
		return ExpansionCoreShardEra();
	}
	return expansionWeaponParry;
}
auto CServerData::ExpansionWeaponParry( UI08 setting ) -> void
{
	if( setting >= ER_COUNT )
	{
		setting = ER_COUNT - 1;
	}
	expansionWeaponParry = setting;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ExpansionWrestlingParry()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Set era ruleset for wrestling parry
//|	Notes		-		0 = Core (used to inherit CoreShardEra setting)
//|						1 = UO - Original UO release
//|						2 = T2A - The Second Age
//|						3 = UOR - Rennaissance
//|						4 = TD - Third Dawn
//|						5 = LBR - Lord Blackthorn's Revenge
//|						6 = AoS - Age of Shadows
//|						7 = SE - Samurai Empire
//|						8 = ML - Mondain's Legacy
//|						9 = SA - Stygian Abyss
//|						10 = HS - High Seas
//|						11 = ToL - Time of Legends
//o------------------------------------------------------------------------------------------------o
auto CServerData::ExpansionWrestlingParry() const -> UI08
{
	if( static_cast<ExpansionRuleset>( expansionWrestlingParry ) == ER_CORE )
	{
		// Inherit CoreShardEra setting
		return ExpansionCoreShardEra();
	}
	return expansionWrestlingParry;
}
auto CServerData::ExpansionWrestlingParry( UI08 setting ) -> void
{
	if( setting >= ER_COUNT )
	{
		setting = ER_COUNT - 1;
	}
	expansionWrestlingParry = setting;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ExpansionCombatHitChance()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Set era ruleset for combat hit chance calculations
//|	Notes		-		0 = Core (used to inherit CoreShardEra setting)
//|						1 = UO - Original UO release
//|						2 = T2A - The Second Age
//|						3 = UOR - Rennaissance
//|						4 = TD - Third Dawn
//|						5 = LBR - Lord Blackthorn's Revenge
//|						6 = AoS - Age of Shadows
//|						7 = SE - Samurai Empire
//|						8 = ML - Mondain's Legacy
//|						9 = SA - Stygian Abyss
//|						10 = HS - High Seas
//|						11 = ToL - Time of Legends
//o------------------------------------------------------------------------------------------------o
auto CServerData::ExpansionCombatHitChance() const -> UI08
{
	if( static_cast<ExpansionRuleset>( expansionCombatHitChance ) == ER_CORE )
	{
		// Inherit CoreShardEra setting
		return ExpansionCoreShardEra();
	}
	return expansionCombatHitChance;
}
auto CServerData::ExpansionCombatHitChance( UI08 setting ) -> void
{
	if( setting >= ER_COUNT )
	{
		setting = ER_COUNT - 1;
	}
	expansionCombatHitChance = setting;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ShootOnAnimalBack()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether players can shoot arrows while riding mounts
//o------------------------------------------------------------------------------------------------o
auto CServerData::ShootOnAnimalBack() const -> bool
{
	return boolVals.test( BIT_SHOOTONANIMALBACK );
}
auto CServerData::ShootOnAnimalBack( bool newVal ) -> void
{
	boolVals.set( BIT_SHOOTONANIMALBACK, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::NPCTrainingStatus()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether NPCs can train players in skills
//o------------------------------------------------------------------------------------------------o
auto CServerData::NPCTrainingStatus() const -> bool
{
	return boolVals.test( BIT_NPCTRAINING );
}
auto CServerData::NPCTrainingStatus( bool newVal ) -> void
{
	boolVals.set( BIT_NPCTRAINING, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::DumpPaths()
//|	Date		-	02/26/2002
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Needed a function that would dump out the paths. If you add any paths to the
//|					server please make sure that you place exports to the console here to please
//|					 so it can be looked up if needed.
//o------------------------------------------------------------------------------------------------o
auto CServerData::DumpPaths() -> void
{
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

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::PlayerCorpseLootDecay()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether loot decays along with player corpses or is left on ground
//o------------------------------------------------------------------------------------------------o
auto CServerData::PlayerCorpseLootDecay() const -> bool
{
	return boolVals.test( BIT_LOOTDECAYSONPLAYERCORPSE );
}
auto CServerData::PlayerCorpseLootDecay( bool newVal ) -> void
{
	boolVals.set( BIT_LOOTDECAYSONPLAYERCORPSE, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::NpcCorpseLootDecay()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether loot decays along with  npc corpses or is left on ground
//o------------------------------------------------------------------------------------------------o
auto CServerData::NpcCorpseLootDecay() const -> bool
{
	return boolVals.test( BIT_LOOTDECAYSONNPCCORPSE );
}
auto CServerData::NpcCorpseLootDecay( bool newVal ) -> void
{
	boolVals.set( BIT_LOOTDECAYSONNPCCORPSE, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::GuardsStatus()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether guards are enabled globally or not
//o------------------------------------------------------------------------------------------------o
auto CServerData::GuardsStatus() const -> bool
{
	return boolVals.test( BIT_GUARDSENABLED );
}
auto CServerData::GuardStatus( bool newVal ) -> void
{
	boolVals.set( BIT_GUARDSENABLED, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::DeathAnimationStatus()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether death animation plays when a player dies
//o------------------------------------------------------------------------------------------------o
auto CServerData::DeathAnimationStatus() const -> bool
{
	return boolVals.test( BIT_PLAYDEATHANIMATION );
}
auto CServerData::DeathAnimationStatus( bool newVal ) -> void
{
	boolVals.set( BIT_PLAYDEATHANIMATION, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::WorldAmbientSounds()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets intensity of ambient world sounds, bird chirps, animal sounds, etc
//o------------------------------------------------------------------------------------------------o
auto CServerData::WorldAmbientSounds() const -> SI16
{
	return ambientSounds;
}
auto CServerData::WorldAmbientSounds( SI16 value ) -> void
{
	if( value < 0 )
	{
		ambientSounds = 0;
	}
	else
	{
		ambientSounds = value;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::AmbientFootsteps()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether context-specific footstep sounds are enabled or not
//o------------------------------------------------------------------------------------------------o
auto CServerData::AmbientFootsteps() const -> bool
{
	return boolVals.test( BIT_AMBIENTFOOTSTEPS );
}
auto CServerData::AmbientFootsteps( bool newVal ) -> void
{
	boolVals.set( BIT_AMBIENTFOOTSTEPS, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::InternalAccountStatus()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether automatic account creation is enabled
//o------------------------------------------------------------------------------------------------o
auto CServerData::InternalAccountStatus() const -> bool
{
	return boolVals.test( BIT_INTERNALACCOUNTS );
}
auto CServerData::InternalAccountStatus( bool newVal ) -> void
{
	boolVals.set( BIT_INTERNALACCOUNTS, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::YoungPlayerSystem()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether the Young Player system is enabled
//o------------------------------------------------------------------------------------------------o
auto CServerData::YoungPlayerSystem() const -> bool
{
	return boolVals.test( BIT_YOUNGPLAYERSYSTEM );
}
auto CServerData::YoungPlayerSystem( bool newVal ) -> void
{
	boolVals.set( BIT_YOUNGPLAYERSYSTEM, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ShowOfflinePCs() 
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether GMs can see offline players
//o------------------------------------------------------------------------------------------------o
auto CServerData::ShowOfflinePCs() const -> bool
{
	return boolVals.test( BIT_SHOWOFFLINEPCS );
}
auto CServerData::ShowOfflinePCs( bool newVal ) -> void
{
	boolVals.set( BIT_SHOWOFFLINEPCS, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::RogueStatus()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether stealing skill is enabled
//o------------------------------------------------------------------------------------------------o
auto CServerData::RogueStatus() const -> bool
{
	return boolVals.test( BIT_ROGUESTATUS );
}
auto CServerData::RogueStatus( bool newVal ) -> void
{
	boolVals.set( BIT_ROGUESTATUS, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::SnoopIsCrime()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether snooping is considered a crime
//o------------------------------------------------------------------------------------------------o
auto CServerData::SnoopIsCrime() const -> bool
{
	return boolVals.test( BIT_SNOOPISCRIME );
}
auto CServerData::SnoopIsCrime( bool newVal ) -> void
{
	boolVals.set( BIT_SNOOPISCRIME, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::SnoopAwareness()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether targets of snooping become more aware of it over time
//o------------------------------------------------------------------------------------------------o
auto CServerData::SnoopAwareness() const -> bool
{
	return boolVals.test( BIT_SNOOPAWARENESS );
}
auto CServerData::SnoopAwareness( bool newVal ) -> void
{
	boolVals.set( BIT_SNOOPAWARENESS, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::StatsAffectSkillChecks()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether stats like strength, dexterity and intelligence provide
//|					bonuses to skillchecks
//o------------------------------------------------------------------------------------------------o
auto CServerData::StatsAffectSkillChecks() const -> bool
{
	return boolVals.test( BIT_STATSAFFECTSKILLCHECKS );
}
auto CServerData::StatsAffectSkillChecks( bool newVal ) -> void
{
	boolVals.set( BIT_STATSAFFECTSKILLCHECKS, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ExtendedStartingStats()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether extended starting stats are enabled
//|					If enabled players start with 90 total statpoints instead of 80
//o------------------------------------------------------------------------------------------------o
auto CServerData::ExtendedStartingStats() const -> bool
{
	return boolVals.test( BIT_EXTENDEDSTARTINGSTATS );
}
auto CServerData::ExtendedStartingStats( bool newVal ) -> void
{
	boolVals.set( BIT_EXTENDEDSTARTINGSTATS, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ExtendedStartingSkills()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether extended starting skills are enabled
//|					If enabled, players start with 4 skills instead of 3, and have a total of
//|					120 skillpoints at the start instead of 100
//o------------------------------------------------------------------------------------------------o
auto CServerData::ExtendedStartingSkills() const -> bool
{
	return boolVals.test( BIT_EXTENDEDSTARTINGSKILLS );
}
auto CServerData::ExtendedStartingSkills( bool newVal ) -> void
{
	boolVals.set( BIT_EXTENDEDSTARTINGSKILLS, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::PlayerPersecutionStatus()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether player ghosts can drain mana from other players by attacking them
//o------------------------------------------------------------------------------------------------o
auto CServerData::PlayerPersecutionStatus() const -> bool
{
	return boolVals.test( BIT_PERSECUTIONSTATUS );
}
auto CServerData::PlayerPersecutionStatus( bool newVal ) -> void
{
	boolVals.set( BIT_PERSECUTIONSTATUS, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::HtmlStatsStatus()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether html stats are enabled or disabled
//o------------------------------------------------------------------------------------------------o
auto CServerData::HtmlStatsStatus() const -> SI16
{
	return htmlStatusEnabled;
}
auto CServerData::HtmlStatsStatus( SI16 value ) -> void
{
	htmlStatusEnabled = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::GlobalRestockMultiplier()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global restock multiplier, which is applied to restock property
//|					of items as they are loaded in from the DFNs
//o------------------------------------------------------------------------------------------------o
auto CServerData::GlobalRestockMultiplier() const -> float
{
	return globalRestockMultiplier;
}
auto CServerData::GlobalRestockMultiplier( R32 value ) -> void
{
	globalRestockMultiplier = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::BODGoldRewardMultiplier()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets a multiplier that adjusts amount of Gold rewarded to players for
//|					handing in completed Bulk Order Deeds
//o------------------------------------------------------------------------------------------------o
auto CServerData::BODGoldRewardMultiplier() const -> float
{
	return bodGoldRewardMultiplier;
}
auto CServerData::BODGoldRewardMultiplier( R32 value ) -> void
{
	bodGoldRewardMultiplier = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::BODFameRewardMultiplier()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets a multiplier that adjusts amount of Fame rewarded to players for
//|					handing in completed Bulk Order Deeds
//o------------------------------------------------------------------------------------------------o
auto CServerData::BODFameRewardMultiplier() const -> float
{
	return bodFameRewardMultiplier;
}
auto CServerData::BODFameRewardMultiplier( R32 value ) -> void
{
	bodFameRewardMultiplier = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::SellByNameStatus()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether items are sold by their name, not just ID/Colour
//o------------------------------------------------------------------------------------------------o
auto CServerData::SellByNameStatus() const -> bool
{
	return boolVals.test( BIT_SELLBYNAME );
}
auto CServerData::SellByNameStatus( bool newVal ) -> void
{
	boolVals.set( BIT_SELLBYNAME, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::SellMaxItemsStatus() 
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum amount of items that can be sold in one go to a vendor
//o------------------------------------------------------------------------------------------------o
auto CServerData::SellMaxItemsStatus() const -> SI16
{
	return sellMaxItems;
}
auto CServerData::SellMaxItemsStatus( SI16 value ) -> void
{
	sellMaxItems = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::TradeSystemStatus()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether the advanced trade system is enabled or not.
//|					If enabled, prices of goods with NPC vendors will fluctuate with demand
//o------------------------------------------------------------------------------------------------o
auto CServerData::TradeSystemStatus() const -> bool
{
	return boolVals.test( BIT_TRADESYSSTATUS );
}
auto CServerData::TradeSystemStatus( bool newVal ) -> void
{
	boolVals.set( BIT_TRADESYSSTATUS, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::RankSystemStatus()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether rank system is enabled or not. If enabled, it allows for
//|					variable quality of crafted items
//o------------------------------------------------------------------------------------------------o
auto CServerData::RankSystemStatus() const -> bool
{
	return boolVals.test( BIT_RANKSYSSTATUS );
}
auto CServerData::RankSystemStatus( bool newVal ) -> void
{
	boolVals.set( BIT_RANKSYSSTATUS, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::DisplayMakersMark() 
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether maker's mark is displayed for crafted items that have this
//o------------------------------------------------------------------------------------------------o
auto CServerData::DisplayMakersMark( void ) const -> bool
{
	return boolVals.test( BIT_DISPLAYMAKERSMARK );
}
auto CServerData::DisplayMakersMark( bool newVal ) -> void
{
	boolVals.set( BIT_DISPLAYMAKERSMARK, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CutScrollRequirementStatus()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether skill requirements are cut when casting spells from scrolls
//o------------------------------------------------------------------------------------------------o
auto CServerData::CutScrollRequirementStatus() const -> bool
{
	return boolVals.test( BIT_CUTSCROLLREQ );
}
auto CServerData::CutScrollRequirementStatus( bool newVal ) -> void
{
	boolVals.set( BIT_CUTSCROLLREQ, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CheckPetControlDifficulty()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether pet control difficulty is enabled or not
//o------------------------------------------------------------------------------------------------o
auto CServerData::CheckPetControlDifficulty() const -> bool
{
	
	return boolVals.test( BIT_CHECKPETCONTROLDIFFICULTY );
}
auto CServerData::CheckPetControlDifficulty( bool newVal ) -> void
{
	boolVals.set( BIT_CHECKPETCONTROLDIFFICULTY, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CheckItemsSpeed()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets how often in seconds items are checked for decay and other things
//o------------------------------------------------------------------------------------------------o
auto CServerData::CheckItemsSpeed() const -> double
{
	return checkItems;
}
auto CServerData::CheckItemsSpeed( R64 value ) -> void
{
	if( value < 0.0 )
	{
		checkItems = 0.0;
	}
	else
	{
		checkItems = value;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CheckBoatSpeed()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets how often in seconds boats are checked for decay and other things
//o------------------------------------------------------------------------------------------------o
auto CServerData::CheckBoatSpeed() const -> double
{
	return checkBoats;
}
auto CServerData::CheckBoatSpeed( R64 value ) -> void
{
	if( value < 0.0 )
	{
		checkBoats = 0.0;
	}
	else
	{
		checkBoats = value;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CheckNpcAISpeed()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets how often in seconds NPCs will execute their AI routines
//o------------------------------------------------------------------------------------------------o
auto CServerData::CheckNpcAISpeed() const -> double
{
	return checkNpcAi;
}
auto CServerData::CheckNpcAISpeed( R64 value ) -> void
{
	if( value < 0.0 )
	{
		checkNpcAi = 0.0;
	}
	else
	{
		checkNpcAi = value;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CheckSpawnRegionSpeed()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets how often (in seconds) spawn regions are checked for new spawns
//o------------------------------------------------------------------------------------------------o
auto CServerData::CheckSpawnRegionSpeed() const -> double
{
	return checkSpawnRegions;
}
auto CServerData::CheckSpawnRegionSpeed( R64 value ) -> void
{
	if( value < 0.0 )
	{
		checkSpawnRegions = 0.0;
	}
	else
	{
		checkSpawnRegions = value;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::MsgBoardPostingLevel()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the posting level for bulletin boards
//|					UNUSED?
//o------------------------------------------------------------------------------------------------o
auto CServerData::MsgBoardPostingLevel() const -> UI08
{
	return msgPostingLevel;
}
auto CServerData::MsgBoardPostingLevel( UI08 value ) -> void
{
	msgPostingLevel = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::MsgBoardPostRemovalLevel()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the post removal level for bulleting boards
//|					UNUSED?
//o------------------------------------------------------------------------------------------------o
auto CServerData::MsgBoardPostRemovalLevel() const -> UI08
{
	return msgRemovalLevel;
}
auto CServerData::MsgBoardPostRemovalLevel( UI08 value ) -> void
{
	msgRemovalLevel = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::MineCheck()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the type of check used when players attempt to use the mining skill
//|						0 = mine anywhere
//|						1 = mine mountainsides/cavefloors only
//|						2 = mine in mining regions only (not working?)
//o------------------------------------------------------------------------------------------------o
auto CServerData::MineCheck() const -> UI08
{
	return mineCheck;
}
auto CServerData::MineCheck( UI08 value ) -> void
{
	mineCheck = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::FreeshardServerPoll()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether UOX3 will respond to freeshard server poll requests
//o------------------------------------------------------------------------------------------------o
auto CServerData::FreeshardServerPoll() const -> bool
{
	return boolVals.test( BIT_FREESHARDSERVERPOLL );
}
auto CServerData::FreeshardServerPoll( bool newVal ) -> void
{
	boolVals.set( BIT_FREESHARDSERVERPOLL, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatArmorClassDamageBonus()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether weapons get a double damage bonus versus armors of matching AC
//o------------------------------------------------------------------------------------------------o
auto CServerData::CombatArmorClassDamageBonus() const -> bool
{
	return boolVals.test( BIT_ARMORCLASSDAMAGEBONUS );
}
auto CServerData::CombatArmorClassDamageBonus( bool newVal ) -> void
{
	boolVals.set( BIT_ARMORCLASSDAMAGEBONUS, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatDisplayHitMessage()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether combat hit messages are displayed in combat
//o------------------------------------------------------------------------------------------------o
auto CServerData::CombatDisplayHitMessage() const -> bool
{
	return boolVals.test( BIT_SHOWHITMESSAGE );
}
auto CServerData::CombatDisplayHitMessage( bool newVal ) -> void
{
	boolVals.set( BIT_SHOWHITMESSAGE, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatDisplayDamageNumbers()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether combat damage numbers are displayed in combat
//o------------------------------------------------------------------------------------------------o
auto CServerData::CombatDisplayDamageNumbers() const -> bool
{
	return boolVals.test( BIT_SHOWDAMAGENUMBERS );
}
auto CServerData::CombatDisplayDamageNumbers( bool newVal ) -> void
{
	boolVals.set( BIT_SHOWDAMAGENUMBERS, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatAttackSpeedFromStamina()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether combat attack speed is derived from stamina instead of dexterity
//o------------------------------------------------------------------------------------------------o
auto CServerData::CombatAttackSpeedFromStamina() const -> bool
{
	return boolVals.test( BIT_ATTSPEEDFROMSTAMINA );
}
auto CServerData::CombatAttackSpeedFromStamina( bool newVal ) -> void
{
	boolVals.set( BIT_ATTSPEEDFROMSTAMINA, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::GlobalAttackSpeed()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global attack speed in combat
//o------------------------------------------------------------------------------------------------o
auto CServerData::GlobalAttackSpeed() const -> float
{
	return globalAttackSpeed;
}
auto CServerData::GlobalAttackSpeed( R32 value ) -> void
{
	if( value < 0.0 )
	{
		globalAttackSpeed = 0.0;
	}
	else
	{
		globalAttackSpeed = value;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::NPCSpellCastSpeed()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global NPC spell casting speed
//o------------------------------------------------------------------------------------------------o
auto CServerData::NPCSpellCastSpeed() const -> float
{
	return npcSpellcastSpeed;
}
auto CServerData::NPCSpellCastSpeed( R32 value ) -> void
{
	if( value < 0.0 )
	{
		npcSpellcastSpeed = 0.0;
	}
	else
	{
		npcSpellcastSpeed = value;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::FishingStaminaLoss()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the stamina loss for using the fishing skill
//o------------------------------------------------------------------------------------------------o
auto CServerData::FishingStaminaLoss() const -> SI16
{
	return fishingstaminaloss;
}
auto CServerData::FishingStaminaLoss( SI16 value ) -> void
{
	fishingstaminaloss = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatNpcDamageRate()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the NPC damage divisor. If character is a player, damage from NPCs is
//|					divided by this value.
//o------------------------------------------------------------------------------------------------o
auto CServerData::CombatNpcDamageRate() const -> SI16
{
	return combatNpcDamageRate;
}
auto CServerData::CombatNpcDamageRate( SI16 value ) -> void
{
	combatNpcDamageRate = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::AlchemyDamageBonusEnabled()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether Alchemy Damage Bonus Modifier is enabled
//o------------------------------------------------------------------------------------------------o
auto CServerData::AlchemyDamageBonusEnabled() const -> bool
{
	return boolVals.test( BIT_ALCHEMYDAMAGEBONUSENABLED );
}
auto CServerData::AlchemyDamageBonusEnabled( bool newVal ) -> void
{
	boolVals.set( BIT_ALCHEMYDAMAGEBONUSENABLED, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::AlchemyDamageBonusModifier()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the Alchemy Damage Bonus Modifier, which gives bonus damage to 
//|					explosion potions based on this formula: 
//|						bonusDamage = attackerAlchemySkill / alchemyDamageBonusModifier
//o------------------------------------------------------------------------------------------------o
auto CServerData::AlchemyDamageBonusModifier() const -> UI08
{
	return alchemyDamageBonusModifier;
}
auto CServerData::AlchemyDamageBonusModifier( UI08 value ) -> void
{
	alchemyDamageBonusModifier = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::WeaponDamageBonusType()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the Weapon Damage Bonus Type, which determines how bonuses to weapon
//|					damage is applied to weapons:
//|						0 - Apply flat weapon damage bonus to .hidamage
//|						1 - Split weapon damage bonus between .lodamage and .hidamage
//|						2 - Apply flat weapon damage bonus to BOTH .lodamage and .hidamage (Default)
//o------------------------------------------------------------------------------------------------o
auto CServerData::WeaponDamageBonusType() const -> UI08
{
	return combatWeaponDamageBonusType;
}
auto CServerData::WeaponDamageBonusType( UI08 value ) -> void
{
	combatWeaponDamageBonusType = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ItemsInterruptCasting()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether manipulation of items interrupts casting
//o------------------------------------------------------------------------------------------------o
auto CServerData::ItemsInterruptCasting() const -> bool
{
	return boolVals.test( BIT_ITEMSINTERRUPTCASTING );
}
auto CServerData::ItemsInterruptCasting( bool newVal ) -> void
{
	boolVals.set( BIT_ITEMSINTERRUPTCASTING, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatAttackStamina()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the amount of stamina lost by swinging weapon in combat
//o------------------------------------------------------------------------------------------------o
auto CServerData::CombatAttackStamina() const -> SI16
{
	return combatAttackStamina;
}
auto CServerData::CombatAttackStamina( SI16 value ) -> void
{
	combatAttackStamina = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::SkillLevel()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global difficulty level for crafting items
//|					1 = easy, 5 = default, 10 = difficult
//o------------------------------------------------------------------------------------------------o
auto CServerData::SkillLevel() const -> UI08
{
	return skillLevel;
}
auto CServerData::SkillLevel( UI08 value ) -> void
{
	skillLevel = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::EscortsEnabled()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether escorts are enabled or not
//o------------------------------------------------------------------------------------------------o
auto CServerData::EscortsEnabled() const -> bool
{
	return boolVals.test( BIT_ESCORTSTATUS );
}
auto CServerData::EscortsEnabled( bool newVal ) -> void
{
	boolVals.set( BIT_ESCORTSTATUS, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ItemsDetectSpeech()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether items can trigger onSpeech JS event
//o------------------------------------------------------------------------------------------------o
auto CServerData::ItemsDetectSpeech() const -> bool
{
	return boolVals.test( BIT_ITEMSDETECTSPEECH );
}
auto CServerData::ItemsDetectSpeech( bool newVal ) -> void
{
	boolVals.set( BIT_ITEMSDETECTSPEECH, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ForceNewAnimationPacket()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether server should force the use of new animation packet for NPCs
//o------------------------------------------------------------------------------------------------o
auto CServerData::ForceNewAnimationPacket() const -> bool
{
	return boolVals.test( BIT_FORCENEWANIMATIONPACKET );
}
auto CServerData::ForceNewAnimationPacket( bool newVal ) -> void
{
	if( ClientSupport4000()
		|| ClientSupport5000()
		|| ClientSupport6000()
		|| ClientSupport6050() )
	{
		boolVals.set( BIT_FORCENEWANIMATIONPACKET, false );
		Console.Warning( "FORCENEWANIMATIONPACKET setting not compatible with support for client versions below 7.0.0.0. Setting disabled!" );
	}
	else
	{
		boolVals.set( BIT_FORCENEWANIMATIONPACKET, newVal );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::MapDiffsEnabled()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether server should load diff files for maps and statics
//|	Notes		-	Diff files are not used by client versions newer than 7.0.8.2
//o------------------------------------------------------------------------------------------------o
auto CServerData::MapDiffsEnabled() const -> bool
{
	return boolVals.test( BIT_MAPDIFFSENABLED );
}
auto CServerData::MapDiffsEnabled( bool newVal ) -> void
{
	boolVals.set( BIT_MAPDIFFSENABLED, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::MaxPlayerPackItems()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max item capacity for player (and NPC) backpacks
//o------------------------------------------------------------------------------------------------o
auto CServerData::MaxPlayerPackItems() const -> UI16
{
	return maxPlayerPackItems;
}
auto CServerData::MaxPlayerPackItems( UI16 newVal ) -> void
{
	maxPlayerPackItems = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::MaxPlayerPackWeight()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max weight capacity for player (and NPC) backpacks
//o------------------------------------------------------------------------------------------------o
auto CServerData::MaxPlayerPackWeight() const -> SI32
{
	return maxPlayerPackWeight;
}
auto CServerData::MaxPlayerPackWeight( SI32 newVal ) -> void
{
	maxPlayerPackWeight = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::MaxPlayerBankItems()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max item capacity for player bankboxes
//o------------------------------------------------------------------------------------------------o
auto CServerData::MaxPlayerBankItems() const -> UI16
{
	return maxPlayerBankItems;
}
auto CServerData::MaxPlayerBankItems( UI16 newVal ) -> void
{
	maxPlayerBankItems = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::MaxPlayerBankWeight()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max weight capacity for player (and NPC) bankboxes
//o------------------------------------------------------------------------------------------------o
auto CServerData::MaxPlayerBankWeight() const -> SI32
{
	return maxPlayerBankWeight;
}
auto CServerData::MaxPlayerBankWeight( SI32 newVal ) -> void
{
	maxPlayerBankWeight = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::BasicTooltipsOnly()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether basic tooltips are enabled or not (instead of advanced tooltips)
//o------------------------------------------------------------------------------------------------o
auto CServerData::BasicTooltipsOnly() const -> bool
{
	return boolVals.test( BIT_BASICTOOLTIPSONLY );
}
auto CServerData::BasicTooltipsOnly( bool newVal ) -> void
{
	boolVals.set( BIT_BASICTOOLTIPSONLY, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ShowNpcTitlesInTooltips()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether NPC titles show up in tooltips
//o------------------------------------------------------------------------------------------------o
auto CServerData::ShowNpcTitlesInTooltips() const -> bool
{
	return boolVals.test( BIT_SHOWNPCTITLESINTOOLTIPS );
}
auto CServerData::ShowNpcTitlesInTooltips( bool newVal ) -> void
{
	boolVals.set( BIT_SHOWNPCTITLESINTOOLTIPS, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ShowNpcTitlesOverhead()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether NPC titles show up along with name over their heads
//o------------------------------------------------------------------------------------------------o
auto CServerData::ShowNpcTitlesOverhead() const -> bool
{
	return boolVals.test( BIT_SHOWNPCTITLESOVERHEAD );
}
auto CServerData::ShowNpcTitlesOverhead( bool newVal ) -> void
{
	boolVals.set( BIT_SHOWNPCTITLESOVERHEAD, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ShowInvulnerableTagOverhead()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether (invulnerable) tags show over character heads when invulnerable
//o------------------------------------------------------------------------------------------------o
auto CServerData::ShowInvulnerableTagOverhead() const -> bool
{
	return boolVals.test( BIT_SHOWINVULNERABLETAGOVERHEAD );
}
auto CServerData::ShowInvulnerableTagOverhead( bool newVal ) -> void
{
	boolVals.set( BIT_SHOWINVULNERABLETAGOVERHEAD, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ShowRaceWithName()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether character races show up in tooltips
//o------------------------------------------------------------------------------------------------o
auto CServerData::ShowRaceWithName() const -> bool
{
	return boolVals.test( BIT_SHOWRACEWITHNAME );
}
auto CServerData::ShowRaceWithName( bool newVal ) -> void
{
	boolVals.set( BIT_SHOWRACEWITHNAME, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ShowRaceInPaperdoll()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether character races show up in tooltips
//o------------------------------------------------------------------------------------------------o
auto CServerData::ShowRaceInPaperdoll() const -> bool
{
	return boolVals.test( BIT_SHOWRACEINPAPERDOLL );
}
auto CServerData::ShowRaceInPaperdoll( bool newVal ) -> void
{
	boolVals.set( BIT_SHOWRACEINPAPERDOLL, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ShowGuildInfoInTooltip()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether guild info shows in character tooltips
//o------------------------------------------------------------------------------------------------o
auto CServerData::ShowGuildInfoInTooltip() const -> bool
{
	return boolVals.test( BIT_SHOWGUILDINFOINTOOLTIP );
}
auto CServerData::ShowGuildInfoInTooltip( bool newVal ) -> void
{
	boolVals.set( BIT_SHOWGUILDINFOINTOOLTIP, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ShowReputationTitleInTooltip()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether reputation title (fame/karma) shows up in character tooltip
//o------------------------------------------------------------------------------------------------o
auto CServerData::ShowReputationTitleInTooltip() const -> bool
{
	return boolVals.test( BIT_SHOWREPTITLEINTOOLTIP );
}
auto CServerData::ShowReputationTitleInTooltip( bool newVal ) -> void
{
	boolVals.set( BIT_SHOWREPTITLEINTOOLTIP, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::EnableNPCGuildDiscounts()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether a discounted price is given when buying items from shopkeepers
//|					that belong to the same NPC Guild as the player
//o------------------------------------------------------------------------------------------------o
auto CServerData::EnableNPCGuildDiscounts() const -> bool
{
	return boolVals.test( BIT_ENABLENPCGUILDDISCOUNTS );
}
auto CServerData::EnableNPCGuildDiscounts( bool newVal ) -> void
{
	boolVals.set( BIT_ENABLENPCGUILDDISCOUNTS, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::EnableNPCGuildPremiums()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether a premium price is offered when selling items to shopkeepers
//|					that belong to the same NPC Guild as the player
//o------------------------------------------------------------------------------------------------o
auto CServerData::EnableNPCGuildPremiums() const -> bool
{
	return boolVals.test( BIT_ENABLENPCGUILDPREMIUMS );
}
auto CServerData::EnableNPCGuildPremiums( bool newVal ) -> void
{
	boolVals.set( BIT_ENABLENPCGUILDPREMIUMS, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CastSpellsWhileMoving()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether character races show up in tooltips
//o------------------------------------------------------------------------------------------------o
auto CServerData::CastSpellsWhileMoving() const -> bool
{
	return boolVals.test( BIT_CASTSPELLSWHILEMOVING );
}
auto CServerData::CastSpellsWhileMoving( bool newVal ) -> void
{
	boolVals.set( BIT_CASTSPELLSWHILEMOVING, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::PetCombatTraining()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether pets can gain skills/stats from fighting
//o------------------------------------------------------------------------------------------------o
auto CServerData::PetCombatTraining() const -> bool
{
	return boolVals.test( BIT_PETCOMBATTRAINING );
}
auto CServerData::PetCombatTraining( bool newVal ) -> void
{
	boolVals.set( BIT_PETCOMBATTRAINING, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::NpcCombatTraining()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether (non-tamed, non-hireling) NPCs can gain skills/stats from fighting
//o------------------------------------------------------------------------------------------------o
auto CServerData::NpcCombatTraining() const -> bool
{
	return boolVals.test( BIT_NPCCOMBATTRAINING );
}
auto CServerData::NpcCombatTraining( bool newVal ) -> void
{
	boolVals.set( BIT_NPCCOMBATTRAINING, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::HirelingCombatTraining()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether hireling NPCs can gain skills/stats from fighting
//o------------------------------------------------------------------------------------------------o
auto CServerData::HirelingCombatTraining() const -> bool
{
	return boolVals.test( BIT_HIRELINGCOMBATTRAINING );
}
auto CServerData::HirelingCombatTraining( bool newVal ) -> void
{
	boolVals.set( BIT_HIRELINGCOMBATTRAINING, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ShowItemResistStats()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether elemental resist shows up in item tooltips
//o------------------------------------------------------------------------------------------------o
auto CServerData::ShowItemResistStats() const -> bool
{
	return boolVals.test( BIT_SHOWITEMRESISTSTATS );
}
auto CServerData::ShowItemResistStats( bool newVal ) -> void
{
	boolVals.set( BIT_SHOWITEMRESISTSTATS, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ShowWeaponDamageTypes()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether damage types show up in weapon tooltips
//o------------------------------------------------------------------------------------------------o
auto CServerData::ShowWeaponDamageTypes() const -> bool
{
	return boolVals.test( BIT_SHOWWEAPONDAMAGETYPES );
}
auto CServerData::ShowWeaponDamageTypes( bool newVal ) -> void
{
	boolVals.set( BIT_SHOWWEAPONDAMAGETYPES, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::GlobalItemDecay()
//|	Date		-	2/07/2010
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether decay is enabled or disabled, on a global scale
//o------------------------------------------------------------------------------------------------o
auto CServerData::GlobalItemDecay() const -> bool
{
	return boolVals.test( BIT_GLOBALITEMDECAY );
}
auto CServerData::GlobalItemDecay( bool newVal ) -> void
{
	boolVals.set( BIT_GLOBALITEMDECAY, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ScriptItemsDecayable()
//|	Date		-	2/07/2010
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether items added through scripts decay or not
//o------------------------------------------------------------------------------------------------o
auto CServerData::ScriptItemsDecayable() const -> bool
{
	return boolVals.test( BIT_SCRIPTITEMSDECAYABLE );
}
auto CServerData::ScriptItemsDecayable( bool newVal ) -> void
{
	boolVals.set( BIT_SCRIPTITEMSDECAYABLE, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::BaseItemsDecayable()
//|	Date		-	2/07/2010
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether base items added will decay or not
//o------------------------------------------------------------------------------------------------o
auto CServerData::BaseItemsDecayable() const -> bool
{
	return boolVals.test( BIT_BASEITEMSDECAYABLE );
}
auto CServerData::BaseItemsDecayable( bool newVal ) -> void
{
	boolVals.set( BIT_BASEITEMSDECAYABLE, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ItemDecayInHouses()
//|	Date		-	2/07/2010
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether items inside houses will decay or not
//o------------------------------------------------------------------------------------------------o
auto CServerData::ItemDecayInHouses() const -> bool
{
	return boolVals.test( BIT_ITEMDECAYINHOUSES );
}
auto CServerData::ItemDecayInHouses( bool newVal ) -> void
{
	boolVals.set( BIT_ITEMDECAYINHOUSES, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ProtectPrivateHouses()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether players without explicit access are prevented from entering private houses
//o------------------------------------------------------------------------------------------------o
auto CServerData::ProtectPrivateHouses() const -> bool
{
	return boolVals.test( BIT_PROTECTPRIVATEHOUSES);
}
auto CServerData::ProtectPrivateHouses( bool newVal ) -> void
{
	boolVals.set( BIT_PROTECTPRIVATEHOUSES, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::TrackHousesPerAccount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether house ownership is tracked per account (true) or character (false )
//o------------------------------------------------------------------------------------------------o
auto CServerData::TrackHousesPerAccount() const -> bool
{
	return boolVals.test( BIT_TRACKHOUSESPERACCOUNT);
}
auto CServerData::TrackHousesPerAccount( bool newVal ) -> void
{
	boolVals.set( BIT_TRACKHOUSESPERACCOUNT, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CanOwnAndCoOwnHouses()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether players can both own and co-own houses at the same time
//o------------------------------------------------------------------------------------------------o
auto CServerData::CanOwnAndCoOwnHouses() const -> bool
{
	return boolVals.test( BIT_CANOWNANDCOOWNHOUSES);
}
auto CServerData::CanOwnAndCoOwnHouses( bool newVal ) -> void
{
	boolVals.set( BIT_CANOWNANDCOOWNHOUSES, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CoOwnHousesOnSameAccount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether characters on same account as house owner will be treated as
//|					if they are co-owners of the house
//o------------------------------------------------------------------------------------------------o
auto CServerData::CoOwnHousesOnSameAccount() const -> bool
{
	return boolVals.test( BIT_COOWNHOUSESONSAMEACCOUNT);
}
auto CServerData::CoOwnHousesOnSameAccount( bool newVal ) -> void
{
	boolVals.set( BIT_COOWNHOUSESONSAMEACCOUNT, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData:SafeCoOwnerLogout()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether co-owners of a house can safely log out in the house without
//|					being booted out of the house
//o------------------------------------------------------------------------------------------------o
auto CServerData::SafeCoOwnerLogout() const -> bool
{
	return boolVals.test( BIT_SAFECOOWNERLOGOUT );
}
auto CServerData::SafeCoOwnerLogout( bool newVal ) -> void
{
	boolVals.set( BIT_SAFECOOWNERLOGOUT, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::SafeFriendLogout()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether friends of a house can safely log out in the house without
//|					being booted out of the house
//o------------------------------------------------------------------------------------------------o
auto CServerData::SafeFriendLogout() const -> bool
{
	return boolVals.test( BIT_SAFEFRIENDLOGOUT );
}
auto CServerData::SafeFriendLogout( bool newVal ) -> void
{
	boolVals.set( BIT_SAFEFRIENDLOGOUT, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::SafeGuestLogout()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether guests of a house can safely log out in the house without
//|					being booted out of the house
//o------------------------------------------------------------------------------------------------o
auto CServerData::SafeGuestLogout() const -> bool
{
	return boolVals.test( BIT_SAFEGUESTLOGOUT );
}
auto CServerData::SafeGuestLogout( bool newVal ) -> void
{
	boolVals.set( BIT_SAFEGUESTLOGOUT, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::KeylessOwnerAccess()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether owner of a house can access locked doors without a key
//o------------------------------------------------------------------------------------------------o
auto CServerData::KeylessOwnerAccess() const -> bool
{
	return boolVals.test( BIT_KEYLESSOWNERACCESS );
}
auto CServerData::KeylessOwnerAccess( bool newVal ) -> void
{
	boolVals.set( BIT_KEYLESSOWNERACCESS, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::KeylessCoOwnerAccess()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether co-owners of a house can access locked doors without a key
//o------------------------------------------------------------------------------------------------o
auto CServerData::KeylessCoOwnerAccess() const -> bool
{
	return boolVals.test( BIT_KEYLESSCOOWNERACCESS );
}
auto CServerData::KeylessCoOwnerAccess( bool newVal ) -> void
{
	boolVals.set( BIT_KEYLESSCOOWNERACCESS, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::KeylessFriendAccess()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether friends of a house can access locked doors without a key
//o------------------------------------------------------------------------------------------------o
auto CServerData::KeylessFriendAccess() const -> bool
{
	return boolVals.test( BIT_KEYLESSFRIENDACCESS );
}
auto CServerData::KeylessFriendAccess( bool newVal ) -> void
{
	boolVals.set( BIT_KEYLESSFRIENDACCESS, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::KeylessGuestAccess()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether guests of a house can access locked doors without a key
//o------------------------------------------------------------------------------------------------o
auto CServerData::KeylessGuestAccess() const -> bool
{
	return boolVals.test( BIT_KEYLESSGUESTACCESS );
}
auto CServerData::KeylessGuestAccess( bool newVal ) -> void
{
	boolVals.set( BIT_KEYLESSGUESTACCESS, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::PaperdollGuildButton()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether guild menu can be accessed from paperdoll button or not
//o------------------------------------------------------------------------------------------------o
auto CServerData::PaperdollGuildButton() const -> bool
{
	return boolVals.test( BIT_PAPERDOLLGUILDBUTTON );
}
auto CServerData::PaperdollGuildButton( bool newVal ) -> void
{
	boolVals.set( BIT_PAPERDOLLGUILDBUTTON, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatMonstersVsAnimals()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether monsters will attack animals or not
//o------------------------------------------------------------------------------------------------o
auto CServerData::CombatMonstersVsAnimals() const -> bool
{
	return boolVals.test( BIT_MONSTERSVSANIMALS );
}
auto CServerData::CombatMonstersVsAnimals( bool newVal ) -> void
{
	boolVals.set( BIT_MONSTERSVSANIMALS, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::OfferBODsFromItemSales()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether Bulk Order Deeds should be offered to players upon selling
//|					items to a vendor that supports that feature (like a blacksmith)
//o------------------------------------------------------------------------------------------------o
auto CServerData::OfferBODsFromItemSales() const -> bool
{
	return boolVals.test( BIT_OFFERBODSFROMITEMSALES );
}
auto CServerData::OfferBODsFromItemSales( bool newVal ) -> void
{
	boolVals.set( BIT_OFFERBODSFROMITEMSALES, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::OfferBODsFromContextMenu()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether Bulk Order Deeds should be offered to players via
//|					context menu option on vendors that supports that feature (like a blacksmith)
//o------------------------------------------------------------------------------------------------o
auto CServerData::OfferBODsFromContextMenu() const -> bool
{
	return boolVals.test( BIT_OFFERBODSFROMCONTEXTMENU );
}
auto CServerData::OfferBODsFromContextMenu( bool newVal ) -> void
{
	boolVals.set( BIT_OFFERBODSFROMCONTEXTMENU, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::BODsFromCraftedItemsOnly()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether Bulk Order Deed offers will be triggered by selling any
//|					items to a vendor that supports the feature, or just from selling crafted items.
//|					Depends on OFFERBODSFROMITEMSALES feature being enabled.
//o------------------------------------------------------------------------------------------------o
auto CServerData::BODsFromCraftedItemsOnly() const -> bool
{
	return boolVals.test( BIT_BODSFROMCRAFTEDITEMSONLY );
}
auto CServerData::BODsFromCraftedItemsOnly( bool newVal ) -> void
{
	boolVals.set( BIT_BODSFROMCRAFTEDITEMSONLY, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::MaxHousesOwnable()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max amount of houses that a player can own
//o------------------------------------------------------------------------------------------------o
auto CServerData::MaxHousesOwnable() const -> UI16
{
	return maxHousesOwnable;
}
auto CServerData::MaxHousesOwnable( UI16 value ) -> void
{
	maxHousesOwnable = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::MaxHousesCoOwnable()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max amount of houses that a player can co-own
//o------------------------------------------------------------------------------------------------o
auto CServerData::MaxHousesCoOwnable() const -> UI16
{
	return maxHousesCoOwnable;
}
auto CServerData::MaxHousesCoOwnable( UI16 value ) -> void
{
	maxHousesCoOwnable = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatAnimalsAttackChance()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the chance of monsters attacking animals
//o------------------------------------------------------------------------------------------------o
auto CServerData::CombatAnimalsAttackChance() const -> UI16
{
	return combatAnimalAttackChance;
}
auto CServerData::CombatAnimalsAttackChance( UI16 value ) -> void
{
	if( value > 1000 )
	{
		value = 1000;
	}
	combatAnimalAttackChance = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatArcheryHitBonus()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the delay hit bonus (in percentage) in combat for Archery. Can be negative!
//|					This bonus was mentioned in official patch notes for Publish 5 (UOR patch).
//o------------------------------------------------------------------------------------------------o
auto CServerData::CombatArcheryHitBonus() const -> SI08
{
	return combatArcheryHitBonus;
}
auto CServerData::CombatArcheryHitBonus( SI08 value ) -> void
{
	if( value > 100 )
	{
		value = 100;
	}
	combatArcheryHitBonus = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatArcheryShootDelay()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the delay (in seconds, with decimals) after archers stop moving until
//|					they can fire a shot in combat
//o------------------------------------------------------------------------------------------------o
auto CServerData::CombatArcheryShootDelay() const -> float
{
	return archeryShootDelay;
}
auto CServerData::CombatArcheryShootDelay( R32 value ) -> void
{
	if( value < 0.0 )
	{
		archeryShootDelay = 0.0;
	}
	else
	{
		archeryShootDelay = value;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatWeaponDamageChance()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets chance for weapons to take damage in combat (on hit)
//o------------------------------------------------------------------------------------------------o
auto CServerData::CombatWeaponDamageChance() const -> UI08
{
	return combatWeaponDamageChance;
}
auto CServerData::CombatWeaponDamageChance( UI08 value ) -> void
{
	if( value > 100 )
	{
		value = 100;
	}
	combatWeaponDamageChance = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatWeaponDamageMin()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the min amount of damage (in hitpoints) weapons will take from combat
//o------------------------------------------------------------------------------------------------o
auto CServerData::CombatWeaponDamageMin() const -> UI08
{
	return combatWeaponDamageMin;
}
auto CServerData::CombatWeaponDamageMin( UI08 value ) -> void
{
	combatWeaponDamageMin = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatWeaponDamageMax()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max amount of damage (in hitpoints) weapons will take from combat
//o------------------------------------------------------------------------------------------------o
auto CServerData::CombatWeaponDamageMax() const -> UI08
{
	return combatWeaponDamageMax;
}
auto CServerData::CombatWeaponDamageMax( UI08 value ) -> void
{
	combatWeaponDamageMax = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatArmorDamageChance()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the chance to damage armor in combat
//o------------------------------------------------------------------------------------------------o
auto CServerData::CombatArmorDamageChance() const -> UI08
{
	return combatArmorDamageChance;
}
auto CServerData::CombatArmorDamageChance( UI08 value ) -> void
{
	if( value > 100 )
	{
		value = 100;
	}
	combatArmorDamageChance = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatArmorDamageMin()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the min damage dealt to armor in combat
//o------------------------------------------------------------------------------------------------o
auto CServerData::CombatArmorDamageMin() const -> UI08
{
	return combatArmorDamageMin;
}
auto CServerData::CombatArmorDamageMin( UI08 value ) -> void
{
	combatArmorDamageMin = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatArmorDamageMax()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max damage dealt to armor in combat
//o------------------------------------------------------------------------------------------------o
auto CServerData::CombatArmorDamageMax() const -> UI08
{
	return combatArmorDamageMax;
}
auto CServerData::CombatArmorDamageMax( UI08 value ) -> void
{
	combatArmorDamageMax = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatParryDamageChance()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the chance to damage items used to parry in combat
//o------------------------------------------------------------------------------------------------o
auto CServerData::CombatParryDamageChance() const -> UI08
{
	return combatParryDamageChance;
}
auto CServerData::CombatParryDamageChance( UI08 value ) -> void
{
	if( value > 100 )
	{
		value = 100;
	}
	combatParryDamageChance = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatParryDamageMin()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the min damage dealt to items used to parry in combat
//o------------------------------------------------------------------------------------------------o
auto CServerData::CombatParryDamageMin() const -> UI08
{
	return combatParryDamageMin;
}
auto CServerData::CombatParryDamageMin( UI08 value ) -> void
{
	combatParryDamageMin = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatParryDamageMax()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max damage dealt to items used to parry in combat
//o------------------------------------------------------------------------------------------------o
auto CServerData::CombatParryDamageMax() const -> UI08
{
	return combatParryDamageMax;
}
auto CServerData::CombatParryDamageMax( UI08 value ) -> void
{
	combatParryDamageMax = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatBloodEffectChance()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the chance to spawn blood splatter effects during combat
//o------------------------------------------------------------------------------------------------o
auto CServerData::CombatBloodEffectChance() const -> UI08
{
	return combatBloodEffectChance;
}
auto CServerData::CombatBloodEffectChance( UI08 value ) -> void
{
	if( value > 100 )
	{
		value = 100;
	}
	combatBloodEffectChance = value;
}

//o------------------------------------------------------------------------------------------------o
//| Function    -   CServerData::HungerSystemEnabled()
//o------------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets whether hunger system is enabled or disabled
//o------------------------------------------------------------------------------------------------o
auto CServerData::HungerSystemEnabled() const -> bool
{
	return boolVals.test( BIT_HUNGERSYSTEMENABLED );
}
auto CServerData::HungerSystemEnabled( bool newVal ) -> void
{
	boolVals.set( BIT_HUNGERSYSTEMENABLED, newVal );
}

//o------------------------------------------------------------------------------------------------o
//| Function    -   CServerData::ThirstSystemEnabled()
//o------------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets whether hunger system is enabled or disabled
//o------------------------------------------------------------------------------------------------o
auto CServerData::ThirstSystemEnabled() const -> bool
{
	return boolVals.test( BIT_THIRSTSYSTEMENABLED );
}
auto CServerData::ThirstSystemEnabled( bool newVal ) -> void
{
	boolVals.set( BIT_THIRSTSYSTEMENABLED, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::HungerDamage()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the damage taken from players being hungry
//o------------------------------------------------------------------------------------------------o
auto CServerData::HungerDamage() const -> SI16
{
	return hungerDamage;
}
auto CServerData::HungerDamage( SI16 value ) -> void
{
	hungerDamage = value;
}

//o------------------------------------------------------------------------------------------------o
//| Function    -   CServerData::ThirstDrain()
//o------------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets the stamina drain from players being thirsty
//o------------------------------------------------------------------------------------------------o
auto CServerData::ThirstDrain() const -> SI16
{
	return thirstDrain;
}
auto CServerData::ThirstDrain( SI16 value ) -> void
{
	thirstDrain = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::PetOfflineTimeout()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the interval in seconds between checks for the player offline time
//o------------------------------------------------------------------------------------------------o
auto CServerData::PetOfflineTimeout() const -> UI16
{
	return petOfflineTimeout;
}
auto CServerData::PetOfflineTimeout( UI16 value ) -> void
{
	petOfflineTimeout = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::PetHungerOffline()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether pets should hunger while the player (owner) is offline or not
//o------------------------------------------------------------------------------------------------o
auto CServerData::PetHungerOffline() const -> bool
{
	return boolVals.test( BIT_PETHUNGEROFFLINE );
}
auto CServerData::PetHungerOffline( bool newVal ) -> void
{
	boolVals.set( BIT_PETHUNGEROFFLINE, newVal );
}

//o------------------------------------------------------------------------------------------------o
//| Function    -   CServerData::PetThirstOffline()
//o------------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets whether pets should thirst while the player (owner) is offline or not
//o------------------------------------------------------------------------------------------------o
auto CServerData::PetThirstOffline() const -> bool
{
	return boolVals.test( BIT_PETTHIRSTOFFLINE );
}
auto CServerData::PetThirstOffline( bool newVal ) -> void
{
	boolVals.set( BIT_PETTHIRSTOFFLINE, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::BuyThreshold()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the threshold for when money is taken from a player's bank account
//|					when buying something from a vendor instead of from their backpack
//o------------------------------------------------------------------------------------------------o
auto CServerData::BuyThreshold() const -> SI16
{
	return buyThreshold;
}
auto CServerData::BuyThreshold( SI16 value ) -> void
{
	buyThreshold = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CharHideWhileMounted()
//|	Date		-	09/22/2002
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether players can hide while mounted or not
//o------------------------------------------------------------------------------------------------o
auto CServerData::CharHideWhileMounted() const -> bool
{
	return boolVals.test( BIT_HIDEWHILEMOUNTED );
}
auto CServerData::CharHideWhileMounted( bool newVal ) -> void
{
	boolVals.set( BIT_HIDEWHILEMOUNTED, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::WeightPerStr()
//|	Date		-	3/12/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets amount of weight one can hold per point of STR
//o------------------------------------------------------------------------------------------------o
auto CServerData::WeightPerStr() const -> float
{
	return weightPerSTR;
}
auto CServerData::WeightPerStr( R32 newVal ) -> void
{
	weightPerSTR = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ServerOverloadPackets()
//|	Date		-	11/20/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether packet handling in JS is enabled or disabled
//o------------------------------------------------------------------------------------------------o
auto CServerData::ServerOverloadPackets() const -> bool
{
	return boolVals.test( BIT_OVERLOADPACKETS );
}
auto CServerData::ServerOverloadPackets( bool newVal ) -> void
{
	boolVals.set( BIT_OVERLOADPACKETS, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ArmorAffectManaRegen()
//|	Date		-	3/20/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether armor affects mana regeneration or not
//o------------------------------------------------------------------------------------------------o
auto CServerData::ArmorAffectManaRegen() const -> bool
{
	return boolVals.test( BIT_ARMORAFFECTMANAREGEN );
}
auto CServerData::ArmorAffectManaRegen( bool newVal ) -> void
{
	boolVals.set( BIT_ARMORAFFECTMANAREGEN, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::AdvancedPathfinding()
//|	Date		-	7/16/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether NPCs use the A* Pathfinding routine or not
//o------------------------------------------------------------------------------------------------o
auto CServerData::AdvancedPathfinding() const -> bool
{
	return boolVals.test( BIT_ADVANCEDPATHFIND );
}
auto CServerData::AdvancedPathfinding( bool newVal ) -> void
{
	boolVals.set( BIT_ADVANCEDPATHFIND, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::LootingIsCrime()
//|	Date		-	4/09/2007
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether looting of corpses can be a crime or not
//o------------------------------------------------------------------------------------------------o
auto CServerData::LootingIsCrime() const -> bool
{
	return boolVals.test( BIT_LOOTINGISCRIME );
}
auto CServerData::LootingIsCrime( bool newVal ) -> void
{
	boolVals.set( BIT_LOOTINGISCRIME, newVal );
}

//o------------------------------------------------------------------------------------------------o
//| Function    -   CServerData::TravelSpellsFromBoatKeys()
//o------------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets whether travel spells (recall, gate) are usable with boat keys to teleport
//|					directly to boat's location
//o------------------------------------------------------------------------------------------------o
auto CServerData::TravelSpellsFromBoatKeys() const -> bool
{	
	return boolVals.test( BIT_TRAVELSPELLSFROMBOATKEYS );
}
auto CServerData::TravelSpellsFromBoatKeys( bool newVal ) -> void
{
	boolVals.set( BIT_TRAVELSPELLSFROMBOATKEYS, newVal );
}

//o------------------------------------------------------------------------------------------------o
//| Function    -   CServerData::TravelSpellsWhileOverweight()
//o------------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets whether travel spells (recall, gate) are allowed while overweight
//o------------------------------------------------------------------------------------------------o
auto CServerData::TravelSpellsWhileOverweight() const -> bool
{
	return boolVals.test( BIT_TRAVELSPELLSWHILEOVERWEIGHT );
}
auto CServerData::TravelSpellsWhileOverweight( bool newVal ) -> void
{
	boolVals.set( BIT_TRAVELSPELLSWHILEOVERWEIGHT, newVal );
}

//o------------------------------------------------------------------------------------------------o
//| Function    -   CServerData::MarkRunesInMultis()
//o------------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets whether server allows marking recall runes in multis (houses, boats)
//o------------------------------------------------------------------------------------------------o
auto CServerData::MarkRunesInMultis() const -> bool
{
	return boolVals.test( BIT_MARKRUNESINMULTIS );
}
auto CServerData::MarkRunesInMultis( bool newVal ) -> void
{
	boolVals.set( BIT_MARKRUNESINMULTIS, newVal );
}

//o------------------------------------------------------------------------------------------------o
//| Function    -   CServerData::TravelSpellsBetweenWorlds()
//o------------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets whether travel spells (recall, gate) are allowed between worlds
//o------------------------------------------------------------------------------------------------o
auto CServerData::TravelSpellsBetweenWorlds() const -> bool
{
	return boolVals.test( BIT_TRAVELSPELLSBETWEENWORLDS );
}
auto CServerData::TravelSpellsBetweenWorlds( bool newVal ) -> void
{
	boolVals.set( BIT_TRAVELSPELLSBETWEENWORLDS, newVal );
}

//o------------------------------------------------------------------------------------------------o
//| Function    -   CServerData::TravelSpellsWhileAggressor()
//o------------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets whether travel spells (recall, gate) are allowed while marked as an aggressor
//o------------------------------------------------------------------------------------------------o
auto CServerData::TravelSpellsWhileAggressor() const -> bool
{
	return boolVals.test( BIT_TRAVELSPELLSWHILEAGGRESSOR );
}
auto CServerData::TravelSpellsWhileAggressor( bool newVal ) -> void
{
	boolVals.set( BIT_TRAVELSPELLSWHILEAGGRESSOR, newVal );
}

//o------------------------------------------------------------------------------------------------o
//| Function    -   CServerData::AutoUnequippedCasting()
//o------------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets whether spells will auto unequipe the hands that is not a spellbook or spellchanneling type.
//o------------------------------------------------------------------------------------------------o
auto CServerData::AutoUnequippedCasting() const -> bool
{
	return boolVals.test( BIT_AUTOUNEQUIPPEDCASTING );
}
auto CServerData::AutoUnequippedCasting( bool newVal ) -> void
{
	boolVals.set( BIT_AUTOUNEQUIPPEDCASTING, newVal );
}

//o------------------------------------------------------------------------------------------------o
//| Function    -   CServerData::MaxControlSlots()
//o------------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets the max amount of control slots a player has available (0 for disable)
//o------------------------------------------------------------------------------------------------o
auto CServerData::MaxControlSlots() const -> UI08
{
	return maxControlSlots;
}
auto CServerData::MaxControlSlots( UI08 newVal ) -> void
{
	maxControlSlots = newVal;
}

//o------------------------------------------------------------------------------------------------o
//| Function    -   CServerData::MaxFollowers()
//o------------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets the max amount of active followers/pets a player can have (0 for disable)
//o------------------------------------------------------------------------------------------------o
auto CServerData::MaxFollowers() const -> UI08
{
	return maxFollowers;
}
auto CServerData::MaxFollowers( UI08 newVal ) -> void
{
	maxFollowers = newVal;
}

//o------------------------------------------------------------------------------------------------o
//| Function    -  CServerData::MaxPetOwners()
//o------------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets the max amount of different owners a pet can have in its lifetime
//o------------------------------------------------------------------------------------------------o
auto CServerData::MaxPetOwners() const -> UI08
{
	return maxPetOwners;
}
auto CServerData::MaxPetOwners( UI08 newVal ) -> void
{
	maxPetOwners = newVal;
}

//o------------------------------------------------------------------------------------------------o
//| Function    -   CServerData::GetPetLoyaltyGainOnSuccess()
//o------------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets the pet loyalty gained on successful use of pet command
//o------------------------------------------------------------------------------------------------o
auto CServerData::GetPetLoyaltyGainOnSuccess() const -> UI16
{
	return petLoyaltyGainOnSuccess;
}
auto CServerData::SetPetLoyaltyGainOnSuccess( UI16 newVal ) -> void
{
	petLoyaltyGainOnSuccess = newVal;
}

//o------------------------------------------------------------------------------------------------o
//| Function    -   CServerData::GetPetLoyaltyLossOnFailure()
//o------------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets the pet loyalty loss for failed use of pet command
//o------------------------------------------------------------------------------------------------o
auto CServerData::GetPetLoyaltyLossOnFailure() const -> UI16
{
	return petLoyaltyLossOnFailure;
}
auto CServerData::SetPetLoyaltyLossOnFailure( UI16 newVal ) -> void
{
	petLoyaltyLossOnFailure = newVal;
}

//o------------------------------------------------------------------------------------------------o
//| Function    -   CServerData::MaxSafeTeleportsPerDay()
//o------------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets the max amount of teleports to safety players get per day via help menu when stuck
//o------------------------------------------------------------------------------------------------o
auto CServerData::MaxSafeTeleportsPerDay() const -> UI08
{
	return maxSafeTeleports;
}
auto CServerData::MaxSafeTeleportsPerDay( UI08 newVal ) -> void
{
	maxSafeTeleports = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::TeleportToNearestSafeLocation()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether players who use the safe teleport via help menu will teleport
//|					to the nearest safe location, or to a random safe location anywhere on the map
//o------------------------------------------------------------------------------------------------o
auto CServerData::TeleportToNearestSafeLocation() const -> bool
{
	return boolVals.test( BIT_TELEPORTTONEARESTSAFELOC );
}
auto CServerData::TeleportToNearestSafeLocation( bool newVal ) -> void
{
	boolVals.set( BIT_TELEPORTTONEARESTSAFELOC, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::AllowAwakeNPCs()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether the feature that allows permanently awake NPCs (ones marked
//|					as such) is enabled or not
//o------------------------------------------------------------------------------------------------o
auto CServerData::AllowAwakeNPCs() const -> bool
{
	return boolVals.test( BIT_ALLOWAWAKENPCS );
}
auto CServerData::AllowAwakeNPCs( bool newVal ) -> void
{
	boolVals.set( BIT_ALLOWAWAKENPCS, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ToolUseLimit()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether tools have usage limits (based on item health)
//o------------------------------------------------------------------------------------------------o
auto CServerData::ToolUseLimit() const -> bool
{
	return boolVals.test( BIT_TOOLUSELIMIT );
}
auto CServerData::ToolUseLimit( bool newVal ) -> void
{
	boolVals.set( BIT_TOOLUSELIMIT, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ToolUseBreak()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether tools will break if they reach 0 health
//o------------------------------------------------------------------------------------------------o
auto CServerData::ToolUseBreak() const -> bool
{
	return boolVals.test( BIT_TOOLUSEBREAK );
}
auto CServerData::ToolUseBreak( bool newVal ) -> void
{
	boolVals.set( BIT_TOOLUSEBREAK, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ItemRepairDurabilityLoss()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether items will have their durability reduced when repaired
//o------------------------------------------------------------------------------------------------o
auto CServerData::ItemRepairDurabilityLoss() const -> bool
{
	return boolVals.test( BIT_ITEMREPAIRDURABILITYLOSS );
}
auto CServerData::ItemRepairDurabilityLoss( bool newVal ) -> void
{
	boolVals.set( BIT_ITEMREPAIRDURABILITYLOSS, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::HideStatsForUnknownMagicItems()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether stats are displayed for unknown/unidentified magic items
//o------------------------------------------------------------------------------------------------o
auto CServerData::HideStatsForUnknownMagicItems() const -> bool
{
	return boolVals.test( BIT_HIDESTATSFORUNKNOWNMAGICITEMS );
}
auto CServerData::HideStatsForUnknownMagicItems( bool newVal ) -> void
{
	boolVals.set( BIT_HIDESTATSFORUNKNOWNMAGICITEMS, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CraftColouredWeapons()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether players can craft weapons from coloured ore
//o------------------------------------------------------------------------------------------------o
auto CServerData::CraftColouredWeapons() const -> bool
{
	return boolVals.test( BIT_CRAFTCOLOUREDWEAPONS );
}
auto CServerData::CraftColouredWeapons( bool newVal ) -> void
{
	boolVals.set( BIT_CRAFTCOLOUREDWEAPONS, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::BackupRatio()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the ratio of worldsaves that get backed up
//o------------------------------------------------------------------------------------------------o
auto CServerData::BackupRatio() const -> SI16
{
	return backupRatio;
}
auto CServerData::BackupRatio( SI16 value ) -> void
{
	backupRatio = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatMaxRange()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum range at which combat can be engaged
//o------------------------------------------------------------------------------------------------o
auto CServerData::CombatMaxRange() const -> SI16
{
	return combatMaxRange;
}
auto CServerData::CombatMaxRange( SI16 value ) -> void
{
	combatMaxRange = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatMaxSpellRange()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum range at which spells can be cast in combat
//o------------------------------------------------------------------------------------------------o
auto CServerData::CombatMaxSpellRange() const -> SI16
{
	return combatMaxSpellRange;
}
auto CServerData::CombatMaxSpellRange( SI16 value ) -> void
{
	combatMaxSpellRange = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatAnimalsGuarded()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether animals are under the protection of town guards or not
//o------------------------------------------------------------------------------------------------o
auto CServerData::CombatAnimalsGuarded() const -> bool
{
	return boolVals.test( BIT_ANIMALSGUARDED );
}
auto CServerData::CombatAnimalsGuarded( bool newVal ) -> void
{
	boolVals.set( BIT_ANIMALSGUARDED, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatNPCBaseFleeAt()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global health threshold where NPCs start fleeing in combat
//o------------------------------------------------------------------------------------------------o
auto CServerData::CombatNPCBaseFleeAt() const -> SI16
{
	return combatNpcBaseFleeAt;
}
auto CServerData::CombatNPCBaseFleeAt( SI16 value ) -> void
{
	combatNpcBaseFleeAt = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::CombatNPCBaseReattackAt()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global health threshold where NPCs reattack after fleeing in combat
//o------------------------------------------------------------------------------------------------o
auto CServerData::CombatNPCBaseReattackAt() const -> SI16
{
	return combatNpcBaseReattackAt;
}
auto CServerData::CombatNPCBaseReattackAt( SI16 value ) -> void
{
	combatNpcBaseReattackAt = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::NPCWalkingSpeed()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global, default walking speed for NPCs
//o------------------------------------------------------------------------------------------------o
auto CServerData::NPCWalkingSpeed() const -> float
{
	return npcWalkingSpeed;
}
auto CServerData::NPCWalkingSpeed( R32 value ) -> void
{
	npcWalkingSpeed = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::NPCRunningSpeed()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global, default running speed for NPCs
//o------------------------------------------------------------------------------------------------o
auto CServerData::NPCRunningSpeed() const -> float
{
	return npcRunningSpeed;
}
auto CServerData::NPCRunningSpeed( R32 value ) -> void
{
	npcRunningSpeed = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::NPCFleeingSpeed()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global, default speed at which NPCs flee in combat
//o------------------------------------------------------------------------------------------------o
auto CServerData::NPCFleeingSpeed() const -> float
{
	return npcFleeingSpeed;
}
auto CServerData::NPCFleeingSpeed( R32 value ) -> void
{
	npcFleeingSpeed = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::NPCMountedWalkingSpeed()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global, default walking speed for mounted NPCs
//o------------------------------------------------------------------------------------------------o
auto CServerData::NPCMountedWalkingSpeed() const -> float
{
	return npcMountedWalkingSpeed;
}
auto CServerData::NPCMountedWalkingSpeed( R32 value ) -> void
{
	npcMountedWalkingSpeed = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::NPCMountedRunningSpeed()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global, default running speed for mounted NPCs
//o------------------------------------------------------------------------------------------------o
auto CServerData::NPCMountedRunningSpeed() const -> float
{
	return npcMountedRunningSpeed;
}
auto CServerData::NPCMountedRunningSpeed( R32 value ) -> void
{
	npcMountedRunningSpeed = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::NPCMountedFleeingSpeed()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global, default speed at which mounted NPCs flee in combat
//o------------------------------------------------------------------------------------------------o
auto CServerData::NPCMountedFleeingSpeed() const -> float
{
	return npcMountedFleeingSpeed;
}
auto CServerData::NPCMountedFleeingSpeed( R32 value ) -> void
{
	npcMountedFleeingSpeed = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::TitleColour()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default text colour for titles in gumps
//o------------------------------------------------------------------------------------------------o
auto CServerData::TitleColour() const -> UI16
{
	return titleColour;
}
auto CServerData::TitleColour( UI16 value ) -> void
{
	titleColour = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::LeftTextColour()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default text colour for left text in gumps (2 column ones)
//o------------------------------------------------------------------------------------------------o
auto CServerData::LeftTextColour() const -> UI16
{
	return leftTextColour;
}
auto CServerData::LeftTextColour( UI16 value ) -> void
{
	leftTextColour = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::RightTextColour()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default text colour for right text in gumps (2 column ones)
//o------------------------------------------------------------------------------------------------o
auto CServerData::RightTextColour() const -> UI16
{
	return rightTextColour;
}
auto CServerData::RightTextColour( UI16 value ) -> void
{
	rightTextColour = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ButtonCancel()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default Button ID for cancel button in gumps
//o------------------------------------------------------------------------------------------------o
auto CServerData::ButtonCancel() const -> UI16
{
	return buttonCancel;
}
auto CServerData::ButtonCancel( UI16 value ) -> void
{
	buttonCancel = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ButtonLeft()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default Button ID for left button (navigation) in gumps
//o------------------------------------------------------------------------------------------------o
auto CServerData::ButtonLeft() const -> UI16
{
	return buttonLeft;
}
auto CServerData::ButtonLeft( UI16 value ) -> void
{
	buttonLeft = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ButtonRight()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default Button ID for right button (navigation) in gumps
//o------------------------------------------------------------------------------------------------o
auto CServerData::ButtonRight() const -> UI16
{
	return buttonRight;
}
auto CServerData::ButtonRight( UI16 value ) -> void
{
	buttonRight = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::BackgroundPic()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default Gump ID for background gump
//o------------------------------------------------------------------------------------------------o
auto CServerData::BackgroundPic() const -> UI16
{
	return backgroundPic;
}
auto CServerData::BackgroundPic( UI16 value ) -> void
{
	backgroundPic = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::TownNumSecsPollOpen()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the time (in seconds) for which a town voting poll is open
//o------------------------------------------------------------------------------------------------o
auto CServerData::TownNumSecsPollOpen() const -> UI32
{
	return numSecsPollOpen;
}
auto CServerData::TownNumSecsPollOpen( UI32 value ) -> void
{
	numSecsPollOpen = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::TownNumSecsAsMayor()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the time (in seconds) that a PC would be a mayor
//o------------------------------------------------------------------------------------------------o
auto CServerData::TownNumSecsAsMayor() const -> UI32
{
	return numSecsAsMayor;
}
auto CServerData::TownNumSecsAsMayor( UI32 value ) -> void
{
	numSecsAsMayor = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::TownTaxPeriod()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the time (in seconds) between periods of taxes for PCs
//o------------------------------------------------------------------------------------------------o
auto CServerData::TownTaxPeriod() const -> UI32
{
	return taxPeriod;
}
auto CServerData::TownTaxPeriod( UI32 value ) -> void
{
	taxPeriod = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::TownGuardPayment()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the time (in seconds) between payments for guards
//o------------------------------------------------------------------------------------------------o
auto CServerData::TownGuardPayment() const -> UI32
{
	return guardPayment;
}
auto CServerData::TownGuardPayment( UI32 value ) -> void
{
	guardPayment = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::RepMaxKills()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the threshold in player kills before a player turns red (murderer)
//o------------------------------------------------------------------------------------------------o
auto CServerData::RepMaxKills() const -> UI16
{
	return maxMurdersAllowed;
}
auto CServerData::RepMaxKills( UI16 value ) -> void
{
	maxMurdersAllowed = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ResLogs()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum number of logs in a given resource area
//o------------------------------------------------------------------------------------------------o
auto CServerData::ResLogs() const -> SI16
{
	return logsPerArea;
}
auto CServerData::ResLogs( SI16 value ) -> void
{
	logsPerArea = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ResLogTime()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the time it takes for 1 single log to respawn in a resource area
//o------------------------------------------------------------------------------------------------o
auto CServerData::ResLogTime() const -> UI16
{
	return logsRespawnTimer;
}
auto CServerData::ResLogTime( UI16 value ) -> void
{
	logsRespawnTimer = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ResOre()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum number of ore in a given resource area
//o------------------------------------------------------------------------------------------------o
auto CServerData::ResOre() const -> SI16
{
	return orePerArea;
}
auto CServerData::ResOre( SI16 value ) -> void
{
	orePerArea = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ResOreTime()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the time it takes for 1 single ore to respawn in a resource area
//o------------------------------------------------------------------------------------------------o
auto CServerData::ResOreTime() const -> UI16
{
	return oreRespawnTimer;
}
auto CServerData::ResOreTime( UI16 value ) -> void
{
	oreRespawnTimer = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ResourceAreaSize()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the size of each resource area to split the world into (min 8x8)
//o------------------------------------------------------------------------------------------------o
auto CServerData::ResourceAreaSize() const -> UI16
{
	return resourceAreaSize;
}
auto CServerData::ResourceAreaSize( UI16 value ) -> void
{
	if( value < 8 )
	{
		value = 8;
	}
	resourceAreaSize = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ResFish()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum number of fish in a given resource area
//o------------------------------------------------------------------------------------------------o
auto CServerData::ResFish() const -> SI16
{
	return fishPerArea;
}
auto CServerData::ResFish( SI16 value ) -> void
{
	fishPerArea = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ResFishTime()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the time it takes for 1 single fish to respawn in a resource area
//o------------------------------------------------------------------------------------------------o
auto CServerData::ResFishTime() const -> UI16
{
	return fishRespawnTimer;
}
auto CServerData::ResFishTime( UI16 value ) -> void
{
	fishRespawnTimer = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::AccountFlushTimer()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets how often (in mins) online accounts are checked to see if they really ARE online
//o------------------------------------------------------------------------------------------------o
auto CServerData::AccountFlushTimer() const -> double
{
	return flushTime;
}
auto CServerData::AccountFlushTimer( R64 value ) -> void
{
	flushTime = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::GetClientFeature()
//|					CServerData::SetClientFeature()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets a specific client-side feature
//|	Notes		-	See ClientFeatures enum in cServerData.h for full list
//o------------------------------------------------------------------------------------------------o
auto CServerData::GetClientFeature( ClientFeatures bitNum ) const -> bool
{
	return clientFeatures.test( bitNum );
}
auto CServerData::SetClientFeature( ClientFeatures bitNum, bool nVal ) -> void
{
	clientFeatures.set( bitNum, nVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::GetClientFeatures()
//|					CServerData::SetClientFeatures()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets which client side features to enable for connecting clients
//|	Notes		-	See ClientFeatures enum in cServerData.h for full list
//o------------------------------------------------------------------------------------------------o
auto CServerData::GetClientFeatures() const -> UI32
{
	return static_cast<UI32>( clientFeatures.to_ulong() );
}
auto CServerData::SetClientFeatures( UI32 nVal ) -> void
{
	clientFeatures = nVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::GetServerFeature()
//|					CServerData::SetServerFeature()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets a specific server-side feature
//|	Notes		-	See ServerFeatures enum in cServerData.h for full list
//o------------------------------------------------------------------------------------------------o
auto CServerData::GetServerFeature( ServerFeatures bitNum ) const -> bool
{
	return serverFeatures.test( bitNum );
}
auto CServerData::SetServerFeature( ServerFeatures bitNum, bool nVal ) -> void
{
	serverFeatures.set( bitNum, nVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::GetServerFeatures()
//|					CServerData::SetServerFeatures()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets which server side features to enable
//|	Notes		-	See ServerFeatures enum in cServerData.h for full list
//o------------------------------------------------------------------------------------------------o
auto CServerData::GetServerFeatures() const -> size_t
{
	return serverFeatures.to_ulong();
}
auto CServerData::SetServerFeatures( size_t nVal ) -> void
{
	serverFeatures = nVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::GetSpawnRegionsFacetStatus()
//|					CServerData::SetSpawnRegionsFacetStatus()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets active status of spawn regions per facet
//o------------------------------------------------------------------------------------------------o
auto CServerData::GetSpawnRegionsFacetStatus( UI32 value ) const -> bool
{
	return spawnRegionsFacets.test( value );
}
auto CServerData::SetSpawnRegionsFacetStatus( UI32 nVal, bool status ) -> void
{
	spawnRegionsFacets.set( nVal, status );
}
auto CServerData::GetSpawnRegionsFacetStatus() const -> UI32
{
	return static_cast<UI32>( spawnRegionsFacets.to_ulong() );
}
auto CServerData::SetSpawnRegionsFacetStatus( UI32 nVal ) -> void
{
	spawnRegionsFacets = nVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::GetMoongateFacetStatus()
//|					CServerData::SetMoongateFacetStatus()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets active status of moongates per facet
//o------------------------------------------------------------------------------------------------o
auto CServerData::GetMoongateFacetStatus( UI32 value ) const -> bool
{
	return moongateFacets.test( value );
}
auto CServerData::SetMoongateFacetStatus( UI32 nVal, bool status ) -> void
{
	moongateFacets.set( nVal, status );
}
auto CServerData::GetMoongateFacetStatus() const -> UI32
{
	return static_cast<UI32>( moongateFacets.to_ulong() );
}
auto CServerData::SetMoongateFacetStatus( UI32 nVal ) -> void
{
	moongateFacets = nVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::GetAssistantNegotiation() 
//|					CServerData::SetAssistantNegotiation()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets status of feature negotiation with assist tools like Razor and AssistUO
//o------------------------------------------------------------------------------------------------o
auto CServerData::GetAssistantNegotiation() const -> bool
{
	return boolVals.test( BIT_ASSISTANTNEGOTIATION );
}
auto CServerData::SetAssistantNegotiation( bool nVal ) -> void
{
	boolVals.set( BIT_ASSISTANTNEGOTIATION, nVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::GetClassicUOMapTracker()
//|					CServerData::SetClassicUOMapTracker()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether server should respond to ClassicUO's WorldMap Tracker packets
//o------------------------------------------------------------------------------------------------o
auto CServerData::GetClassicUOMapTracker() const -> bool
{
	return boolVals.test( BIT_CLASSICUOMAPTRACKER );
}
auto CServerData::SetClassicUOMapTracker( bool nVal ) -> void
{
	boolVals.set( BIT_CLASSICUOMAPTRACKER, nVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::UseUnicodeMessages()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether server should send messages originating on server as unicode
//o------------------------------------------------------------------------------------------------o
auto CServerData::UseUnicodeMessages() const -> bool
{
	return boolVals.test( BIT_USEUNICODEMESSAGES );
}
auto CServerData::UseUnicodeMessages( bool nVal ) -> void
{
	boolVals.set( BIT_USEUNICODEMESSAGES, nVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::GetDisabledAssistantFeature()
//|					CServerData::SetDisabledAssistantFeature()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets which assistant features to enable for connecting clients
//|	Notes		-	Example of assistant: Razor, AssistUO
//|					See ClientFeatures enum in cServerData.h for full list
//o------------------------------------------------------------------------------------------------o
auto CServerData::GetDisabledAssistantFeature( AssistantFeatures bitNum ) const -> bool
{
	return 0 != ( CServerData::DisabledAssistantFeatures & bitNum );
}
auto CServerData::SetDisabledAssistantFeature( AssistantFeatures bitNum, bool nVal ) -> void
{
	if( nVal )
	{
		CServerData::DisabledAssistantFeatures |= bitNum;
	}
	else
	{
		CServerData::DisabledAssistantFeatures &= ~bitNum;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::GetDisabledAssistantFeatures()
//|					CServerData::SetDisabledAssistantFeatures()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets which assistant features to enable for connecting clients
//|	Notes		-	Example of assistant: Razor, AssistUO
//|					See ClientFeatures enum in cServerData.h for full list
//o------------------------------------------------------------------------------------------------o
auto CServerData::GetDisabledAssistantFeatures() const -> UI64
{
	return CServerData::DisabledAssistantFeatures;
}
auto CServerData::SetDisabledAssistantFeatures( UI64 nVal ) -> void
{
	CServerData::DisabledAssistantFeatures = nVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::KickOnAssistantSilence()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether player is kicked if there's no response from assistant tool
//|					to verify it complies with the allowed assistant features
//o------------------------------------------------------------------------------------------------o
auto CServerData::KickOnAssistantSilence() const -> bool
{
	return boolVals.test( BIT_KICKONASSISTANTSILENCE );
}
auto CServerData::KickOnAssistantSilence( bool nVal ) -> void
{
	boolVals.set( BIT_KICKONASSISTANTSILENCE, nVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::SaveIni()
//|	Date		-	02/21/2002
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Save the uox.ini out. This is the default save
//o------------------------------------------------------------------------------------------------o
//|	Returns		- [true] If successfull
//o------------------------------------------------------------------------------------------------o
auto CServerData::SaveIni() -> bool
{
	auto s = actualINI;
	if( s.empty() )
	{
		s = Directory( CSDDP_ROOT );
		s += "uox.ini"s;
	}
	return SaveIni( s );
}

// Map of era enums to era strings, used for conversion between the two types
static const std::unordered_map<ExpansionRuleset, std::string> eraNames
{
	{ ER_UO,	"uo"s },
	{ ER_T2A,	"t2a"s },
	{ ER_UOR,	"uor"s },
	{ ER_TD,	"td"s },
	{ ER_LBR,	"lbr"s },
	{ ER_AOS,	"aos"s },
	{ ER_SE,	"se"s },
	{ ER_ML,	"ml"s },
	{ ER_SA,	"sa"s },
	{ ER_HS,	"hs"s },
	{ ER_TOL,	"tol"s }
};

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::EraEnumToString()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Pass an era enum in, get an era string back
//o------------------------------------------------------------------------------------------------o
auto CServerData::EraEnumToString( ExpansionRuleset eraEnum, bool coreEnum ) -> std::string
{
	std::string eraName = "lbr"; // default
	if( !coreEnum && eraEnum == static_cast<ExpansionRuleset>( ExpansionCoreShardEra() ))
	{
		// Enum matches the core shard era setting; return it as "core"
		eraName = "core";
	}
	else
	{
		try
		{
			eraName = eraNames.at( eraEnum );
		}
		catch( const std::out_of_range &e )
		{
			Console.Error( oldstrutil::format( "Unknown era enum detected, exception thrown: %s", e.what() ));
		}
	}

	return eraName;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::EraStringToEnum()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Pass an era string in, get an era enum back
//o------------------------------------------------------------------------------------------------o
auto CServerData::EraStringToEnum( const std::string &eraString, bool useDefault, bool inheritCore ) -> ExpansionRuleset
{
	auto rValue = ER_CORE;
	if( useDefault )
	{
		rValue = ER_LBR; // Default era if specified era is not found
	}

	auto searchEra = oldstrutil::lower( eraString );
	if( inheritCore && searchEra == "core" )
	{
		// Inherit setting from CORESHARDERA setting
		return static_cast<ExpansionRuleset>( ExpansionCoreShardEra() );
	}
	
	// Look for string with era name in eras map
	auto iter = std::find_if( eraNames.begin(), eraNames.end(), [&searchEra]( const std::pair<ExpansionRuleset, std::string> &value ) {
		return searchEra == std::get<1>( value );
	});

	if( iter != eraNames.end() )
	{
		rValue = iter->first;
	}

	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::SaveIni()
//o------------------------------------------------------------------------------------------------o
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
//o------------------------------------------------------------------------------------------------o
auto CServerData::SaveIni( const std::string &filename ) -> bool
{
	auto rValue = false;
	auto ofsOutput = std::ofstream( filename );
	if( ofsOutput.is_open() )
	{
		ofsOutput << "// UOX Initialization File. V";
		ofsOutput << ( static_cast<UI16>( 1 ) << 8 | static_cast<UI16>( 2 )) << '\n' << "//================================" << '\n' << '\n';
		ofsOutput << "[system]" << '\n' << "{" << '\n';
		ofsOutput << "SERVERNAME=" << ServerName() << '\n';
		ofsOutput << "EXTERNALIP=" << ExternalIP() << '\n';
		ofsOutput << "PORT=" << ServerPort() << '\n';
		ofsOutput << "SECRETSHARDKEY=" << SecretShardKey() << '\n';
		ofsOutput << "SERVERLANGUAGE=" << ServerLanguage() << '\n';
		ofsOutput << "NETRCVTIMEOUT=" << ServerNetRcvTimeout() << '\n';
		ofsOutput << "NETSNDTIMEOUT=" << ServerNetSndTimeout() << '\n';
		ofsOutput << "NETRETRYCOUNT=" << ServerNetRetryCount() << '\n';
		ofsOutput << "CONSOLELOG=" << ( ServerConsoleLog() ? 1 : 0 ) << '\n';
		ofsOutput << "NETWORKLOG=" << ( ServerNetworkLog() ? 1 : 0 ) << '\n';
		ofsOutput << "SPEECHLOG=" << ( ServerSpeechLog() ? 1 : 0 ) << '\n';
		ofsOutput << "COMMANDPREFIX=" << ServerCommandPrefix() << '\n';
		ofsOutput << "ANNOUNCEWORLDSAVES=" << ( ServerAnnounceSavesStatus() ? 1 : 0 ) << '\n';
		ofsOutput << "JOINPARTMSGS=" << ( ServerJoinPartAnnouncementsStatus() ? 1 : 0 ) << '\n';
		ofsOutput << "BACKUPSENABLED=" << ( ServerBackupStatus() ? 1 : 0 ) << '\n';
		ofsOutput << "BACKUPSAVERATIO=" << BackupRatio() << '\n';
		ofsOutput << "SAVESTIMER=" << ServerSavesTimerStatus() << '\n';
		ofsOutput << "ACCOUNTISOLATION=" << "1" << '\n';
		ofsOutput << "UOGENABLED=" << ( ServerUOGEnabled() ? 1 : 0 ) << '\n';
		ofsOutput << "FREESHARDSERVERPOLL=" << ( FreeshardServerPoll() ? 1 : 0 ) << '\n';
		ofsOutput << "RANDOMSTARTINGLOCATION=" << ( ServerRandomStartingLocation() ? 1 : 0 ) << '\n';
		ofsOutput << "ASSISTANTNEGOTIATION=" << ( GetAssistantNegotiation() ? 1 : 0 ) << '\n';
		ofsOutput << "KICKONASSISTANTSILENCE=" << ( KickOnAssistantSilence() ? 1 : 0 ) << '\n';
		ofsOutput << "CLASSICUOMAPTRACKER=" << ( GetClassicUOMapTracker() ? 1 : 0 ) << '\n';
		ofsOutput << "JSENGINESIZE=" << static_cast<UI16>( GetJSEngineSize() ) << '\n';
		ofsOutput << "USEUNICODEMESSAGES=" << ( UseUnicodeMessages() ? 1 : 0 ) << '\n';
		ofsOutput << "CONTEXTMENUS=" << ( ServerContextMenus() ? 1 : 0 ) << '\n';
		ofsOutput << "SYSMESSAGECOLOUR=" << SysMsgColour() << '\n';
		ofsOutput << "MAXCLIENTBYTESIN=" << static_cast<UI32>( MaxClientBytesIn() ) << '\n';
		ofsOutput << "MAXCLIENTBYTESOUT=" << static_cast<UI32>( MaxClientBytesOut() ) << '\n';
		ofsOutput << "NETTRAFFICTIMEBAN=" << static_cast<UI32>( NetTrafficTimeban() ) << '\n';
		ofsOutput << "APSPERFTHRESHOLD=" << static_cast<UI16>( APSPerfThreshold() ) << '\n';
		ofsOutput << "APSINTERVAL=" << static_cast<UI16>( APSInterval() ) << '\n';
		ofsOutput << "APSDELAYSTEP=" << static_cast<UI16>( APSDelayStep() ) << '\n';
		ofsOutput << "APSDELAYMAXCAP=" << static_cast<UI16>( APSDelayMaxCap() ) << '\n';
		ofsOutput << "}" << '\n' << '\n';

		ofsOutput << "[clientsupport]" << '\n' << "{" << '\n';
		ofsOutput << "CLIENTSUPPORT4000=" << ( ClientSupport4000() ? 1 : 0 ) << '\n';
		ofsOutput << "CLIENTSUPPORT5000=" << ( ClientSupport5000() ? 1 : 0 ) << '\n';
		ofsOutput << "CLIENTSUPPORT6000=" << ( ClientSupport6000() ? 1 : 0 ) << '\n';
		ofsOutput << "CLIENTSUPPORT6050=" << ( ClientSupport6050() ? 1 : 0 ) << '\n';
		ofsOutput << "CLIENTSUPPORT7000=" << ( ClientSupport7000() ? 1 : 0 ) << '\n';
		ofsOutput << "CLIENTSUPPORT7090=" << ( ClientSupport7090() ? 1 : 0 ) << '\n';
		ofsOutput << "CLIENTSUPPORT70160=" << ( ClientSupport70160() ? 1 : 0 ) << '\n';
		ofsOutput << "CLIENTSUPPORT70240=" << ( ClientSupport70240() ? 1 : 0 ) << '\n';
		ofsOutput << "CLIENTSUPPORT70300=" << ( ClientSupport70300() ? 1 : 0 ) << '\n';
		ofsOutput << "CLIENTSUPPORT70331=" << ( ClientSupport70331() ? 1 : 0 ) << '\n';
		ofsOutput << "CLIENTSUPPORT704565=" << ( ClientSupport704565() ? 1 : 0 ) << '\n';
		ofsOutput << "CLIENTSUPPORT70610=" << ( ClientSupport70610() ? 1 : 0 ) << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[directories]" << '\n' << "{" << '\n';
		ofsOutput << "DIRECTORY=" << Directory( CSDDP_ROOT ) << '\n';
		ofsOutput << "DATADIRECTORY=" << Directory( CSDDP_DATA ) << '\n';
		ofsOutput << "DEFSDIRECTORY=" << Directory( CSDDP_DEFS ) << '\n';
		ofsOutput << "BOOKSDIRECTORY=" << Directory( CSDDP_BOOKS ) << '\n';
		ofsOutput << "ACTSDIRECTORY=" << Directory( CSDDP_ACCOUNTS ) << '\n';
		ofsOutput << "SCRIPTSDIRECTORY=" << Directory( CSDDP_SCRIPTS ) << '\n';
		ofsOutput << "SCRIPTDATADIRECTORY=" << Directory( CSDDP_SCRIPTDATA ) << '\n';
		ofsOutput << "BACKUPDIRECTORY=" << Directory( CSDDP_BACKUP ) << '\n';
		ofsOutput << "MSGBOARDDIRECTORY=" << Directory( CSDDP_MSGBOARD ) << '\n';
		ofsOutput << "SHAREDDIRECTORY=" << Directory( CSDDP_SHARED ) << '\n';
		ofsOutput << "ACCESSDIRECTORY=" << Directory( CSDDP_ACCESS ) << '\n';
		ofsOutput << "HTMLDIRECTORY=" << Directory( CSDDP_HTML ) << '\n';
		ofsOutput << "LOGSDIRECTORY=" << Directory( CSDDP_LOGS ) << '\n';
		ofsOutput << "DICTIONARYDIRECTORY=" << Directory( CSDDP_DICTIONARIES ) << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[skill & stats]" << '\n' << "{" << '\n';
		ofsOutput << "SKILLLEVEL=" << static_cast<UI16>( SkillLevel() ) << '\n';
		ofsOutput << "SKILLCAP=" << ServerSkillTotalCapStatus() << '\n';
		ofsOutput << "SKILLDELAY=" << static_cast<UI16>( ServerSkillDelayStatus() ) << '\n';
		ofsOutput << "STATCAP=" << ServerStatCapStatus() << '\n';
		ofsOutput << "STATSAFFECTSKILLCHECKS=" << ( StatsAffectSkillChecks() ? 1 : 0 ) << '\n';
		ofsOutput << "EXTENDEDSTARTINGSTATS=" << ( ExtendedStartingStats() ? 1 : 0 ) << '\n';
		ofsOutput << "EXTENDEDSTARTINGSKILLS=" << ( ExtendedStartingSkills() ? 1 : 0 ) << '\n';
		ofsOutput << "MAXSTEALTHMOVEMENTS=" << MaxStealthMovement() << '\n';
		ofsOutput << "MAXSTAMINAMOVEMENTS=" << MaxStaminaMovement() << '\n';
		ofsOutput << "SNOOPISCRIME=" << ( SnoopIsCrime() ? 1 : 0 ) << '\n';
		ofsOutput << "SNOOPAWARENESS=" << ( SnoopAwareness() ? 1 : 0 ) << '\n';
		ofsOutput << "ARMORAFFECTMANAREGEN=" << ( ArmorAffectManaRegen() ? 1 : 0) << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[timers]" << '\n' << "{" << '\n';
		ofsOutput << "CORPSEDECAYTIMER=" << SystemTimer( tSERVER_CORPSEDECAY ) << '\n';
		ofsOutput << "NPCCORPSEDECAYTIMER=" << SystemTimer( tSERVER_NPCCORPSEDECAY ) << '\n';
		ofsOutput << "WEATHERTIMER=" << SystemTimer( tSERVER_WEATHER ) << '\n';
		ofsOutput << "SHOPSPAWNTIMER=" << SystemTimer( tSERVER_SHOPSPAWN ) << '\n';
		ofsOutput << "DECAYTIMER=" << SystemTimer( tSERVER_DECAY ) << '\n';
		ofsOutput << "DECAYTIMERINHOUSE=" << SystemTimer( tSERVER_DECAYINHOUSE ) << '\n';
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
		ofsOutput << "NPCFLAGUPDATETIMER=" << SystemTimer( tSERVER_NPCFLAGUPDATETIMER ) << '\n';
		ofsOutput << "BLOODDECAYTIMER=" << SystemTimer( tSERVER_BLOODDECAY ) << '\n';
		ofsOutput << "BLOODDECAYCORPSETIMER=" << SystemTimer( tSERVER_BLOODDECAYCORPSE ) << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "// Supported era values: core, uo, t2a, uor, td, lbr, aos, se, ml, sa, hs, tol" << '\n';
		ofsOutput << '\n' << "// Note: A value of 'core' inherits whatever is set in CORESHARDERA" << '\n';
		ofsOutput << "[expansion settings]" << '\n' << "{" << '\n';
		ofsOutput << "CORESHARDERA=" << EraEnumToString( static_cast<ExpansionRuleset>( ExpansionCoreShardEra() ), true ) << '\n';
		ofsOutput << "ARMORCALCULATION=" << EraEnumToString( static_cast<ExpansionRuleset>( expansionArmorCalculation )) << '\n';
		ofsOutput << "STRENGTHDAMAGEBONUS=" << EraEnumToString( static_cast<ExpansionRuleset>( expansionStrengthDamageBonus )) << '\n';
		ofsOutput << "TACTICSDAMAGEBONUS=" << EraEnumToString( static_cast<ExpansionRuleset>( expansionTacticsDamageBonus )) << '\n';
		ofsOutput << "ANATOMYDAMAGEBONUS=" << EraEnumToString( static_cast<ExpansionRuleset>( expansionAnatomyDamageBonus )) << '\n';
		ofsOutput << "LUMBERJACKDAMAGEBONUS=" << EraEnumToString( static_cast<ExpansionRuleset>( expansionLumberjackDamageBonus )) << '\n';
		ofsOutput << "RACIALDAMAGEBONUS=" << EraEnumToString( static_cast<ExpansionRuleset>( expansionRacialDamageBonus )) << '\n';
		ofsOutput << "DAMAGEBONUSCAP=" << EraEnumToString( static_cast<ExpansionRuleset>( expansionDamageBonusCap )) << '\n';
		ofsOutput << "SHIELDPARRY=" << EraEnumToString( static_cast<ExpansionRuleset>( expansionShieldParry )) << '\n';
		ofsOutput << "WEAPONPARRY=" << EraEnumToString( static_cast<ExpansionRuleset>( expansionWeaponParry )) << '\n';
		ofsOutput << "WRESTLINGPARRY=" << EraEnumToString( static_cast<ExpansionRuleset>( expansionWrestlingParry )) << '\n';
		ofsOutput << "COMBATHITCHANCE=" << EraEnumToString( static_cast<ExpansionRuleset>( expansionCombatHitChance )) << '\n';

		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[settings]" << '\n' << "{" << '\n';
		ofsOutput << "LOOTDECAYSWITHPLAYERCORPSE=" << ( PlayerCorpseLootDecay() ? 1 : 0 ) << '\n';
		ofsOutput << "LOOTDECAYSWITHNPCCORPSE=" << ( NpcCorpseLootDecay() ? 1 : 0 ) << '\n';
		ofsOutput << "GUARDSACTIVE=" << ( GuardsStatus() ? 1 : 0 ) << '\n';
		ofsOutput << "DEATHANIMATION=" << ( DeathAnimationStatus() ? 1 : 0 ) << '\n';
		ofsOutput << "AMBIENTSOUNDS=" << WorldAmbientSounds() << '\n';
		ofsOutput << "AMBIENTFOOTSTEPS=" << ( AmbientFootsteps() ? 1 : 0 ) << '\n';
		ofsOutput << "INTERNALACCOUNTCREATION=" << ( InternalAccountStatus() ? 1 : 0 ) << '\n';
		ofsOutput << "SHOWOFFLINEPCS=" << ( ShowOfflinePCs() ? 1 : 0 ) << '\n';
		ofsOutput << "ROGUESENABLED=" << ( RogueStatus() ? 1 : 0 ) << '\n';
		ofsOutput << "PLAYERPERSECUTION=" << ( PlayerPersecutionStatus() ? 1 : 0 ) << '\n';
		ofsOutput << "ACCOUNTFLUSH=" << AccountFlushTimer() << '\n';
		ofsOutput << "HTMLSTATUSENABLED=" << HtmlStatsStatus() << '\n';
		ofsOutput << "SELLBYNAME=" << ( SellByNameStatus() ? 1 : 0 ) << '\n';
		ofsOutput << "SELLMAXITEMS=" << SellMaxItemsStatus() << '\n';
		ofsOutput << "GLOBALRESTOCKMULTIPLIER=" << GlobalRestockMultiplier() << '\n';
		ofsOutput << "BANKBUYTHRESHOLD=" << BuyThreshold() << '\n';
		ofsOutput << "TRADESYSTEM=" << ( TradeSystemStatus() ? 1 : 0 ) << '\n';
		ofsOutput << "RANKSYSTEM=" << ( RankSystemStatus() ? 1 : 0 ) << '\n';
		ofsOutput << "DISPLAYMAKERSMARK=" << ( DisplayMakersMark() ? 1 : 0 ) << '\n';
		ofsOutput << "CUTSCROLLREQUIREMENTS=" << ( CutScrollRequirementStatus() ? 1 : 0 ) << '\n';
		ofsOutput << "NPCTRAININGENABLED=" << ( NPCTrainingStatus() ? 1 : 0 ) << '\n';
		ofsOutput << "HIDEWHILEMOUNTED=" << ( CharHideWhileMounted() ? 1 : 0 ) << '\n';
		ofsOutput << "WEIGHTPERSTR=" << static_cast<R32>( WeightPerStr() ) << '\n';
		ofsOutput << "POLYDURATION=" << SystemTimer( tSERVER_POLYMORPH ) << '\n';
		ofsOutput << "CLIENTFEATURES=" << GetClientFeatures() << '\n';
		ofsOutput << "SERVERFEATURES=" << GetServerFeatures() << '\n';
		ofsOutput << "SPAWNREGIONSFACETS=" << GetSpawnRegionsFacetStatus() << '\n';
		ofsOutput << "MOONGATEFACETS=" << GetMoongateFacetStatus() << '\n';
		ofsOutput << "OVERLOADPACKETS=" << ( ServerOverloadPackets() ? 1 : 0 ) << '\n';
		ofsOutput << "ADVANCEDPATHFINDING=" << ( AdvancedPathfinding() ? 1 : 0 ) << '\n';
		ofsOutput << "LOOTINGISCRIME=" << ( LootingIsCrime() ? 1 : 0 ) << '\n';
		ofsOutput << "BASICTOOLTIPSONLY=" << ( BasicTooltipsOnly() ? 1 : 0 ) << '\n';
		ofsOutput << "SHOWREPUTATIONTITLEINTOOLTIP=" << ( ShowReputationTitleInTooltip() ? 1 : 0 ) << '\n';
		ofsOutput << "SHOWGUILDINFOINTOOLTIP=" << ( ShowGuildInfoInTooltip() ? 1 : 0 ) << '\n';
		ofsOutput << "SHOWNPCTITLESINTOOLTIPS=" << ( ShowNpcTitlesInTooltips() ? 1 : 0 ) << '\n';
		ofsOutput << "SHOWNPCTITLESOVERHEAD=" << ( ShowNpcTitlesOverhead() ? 1 : 0 ) << '\n';
		ofsOutput << "SHOWINVULNERABLETAGOVERHEAD=" << ( ShowInvulnerableTagOverhead() ? 1 : 0 ) << '\n';
		ofsOutput << "SHOWRACEWITHNAME=" << ( ShowRaceWithName() ? 1 : 0 ) << '\n';
		ofsOutput << "SHOWRACEINPAPERDOLL=" << ( ShowRaceInPaperdoll() ? 1 : 0 ) << '\n';
		ofsOutput << "GLOBALITEMDECAY=" << ( GlobalItemDecay() ? 1 : 0 ) << '\n';
		ofsOutput << "SCRIPTITEMSDECAYABLE=" << ( ScriptItemsDecayable() ? 1 : 0 ) << '\n';
		ofsOutput << "BASEITEMSDECAYABLE=" << ( BaseItemsDecayable() ? 1 : 0 ) << '\n';
		ofsOutput << "PAPERDOLLGUILDBUTTON=" << ( PaperdollGuildButton() ? 1 : 0 ) << '\n';
		ofsOutput << "FISHINGSTAMINALOSS=" << FishingStaminaLoss() << '\n';
		ofsOutput << "ITEMSDETECTSPEECH=" << ItemsDetectSpeech() << '\n';
		ofsOutput << "MAXPLAYERPACKITEMS=" << MaxPlayerPackItems() << '\n';
		ofsOutput << "MAXPLAYERBANKITEMS=" << MaxPlayerBankItems() << '\n';
		ofsOutput << "MAXPLAYERPACKWEIGHT=" << MaxPlayerPackWeight() << '\n';
		ofsOutput << "MAXPLAYERBANKWEIGHT=" << MaxPlayerBankWeight() << '\n';
		ofsOutput << "FORCENEWANIMATIONPACKET=" << ForceNewAnimationPacket() << '\n';
		ofsOutput << "MAPDIFFSENABLED=" << MapDiffsEnabled() << '\n';
		ofsOutput << "TOOLUSELIMIT=" << ToolUseLimit() << '\n';
		ofsOutput << "TOOLUSEBREAK=" << ToolUseBreak() << '\n';
		ofsOutput << "ITEMREPAIRDURABILITYLOSS=" << ItemRepairDurabilityLoss() << '\n';
		ofsOutput << "CRAFTCOLOUREDWEAPONS=" << CraftColouredWeapons() << '\n';
		ofsOutput << "MAXSAFETELEPORTSPERDAY=" << static_cast<UI16>( MaxSafeTeleportsPerDay() ) << '\n';
		ofsOutput << "TELEPORTTONEARESTSAFELOCATION=" << ( TeleportToNearestSafeLocation() ? 1 : 0 ) << '\n';
		ofsOutput << "ALLOWAWAKENPCS=" << ( AllowAwakeNPCs() ? 1 : 0 ) << '\n';
		ofsOutput << "ENABLENPCGUILDDISCOUNTS=" << ( EnableNPCGuildDiscounts() ? 1 : 0 ) << '\n';
		ofsOutput << "ENABLENPCGUILDPREMIUMS=" << ( EnableNPCGuildPremiums() ? 1 : 0 ) << '\n';
		ofsOutput << "YOUNGPLAYERSYSTEM=" << ( YoungPlayerSystem() ? 1 : 0 ) << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[pets and followers]" << '\n' << "{" << '\n';
		ofsOutput << "MAXCONTROLSLOTS=" << static_cast<UI16>( MaxControlSlots() ) << '\n';
		ofsOutput << "MAXFOLLOWERS=" << static_cast<UI16>( MaxFollowers() ) << '\n';
		ofsOutput << "MAXPETOWNERS=" << static_cast<UI16>( MaxPetOwners() ) << '\n';
		ofsOutput << "CHECKPETCONTROLDIFFICULTY=" << ( CheckPetControlDifficulty() ? 1 : 0 ) << '\n';
		ofsOutput << "PETLOYALTYGAINONSUCCESS=" << static_cast<UI16>( GetPetLoyaltyGainOnSuccess() ) << '\n';
		ofsOutput << "PETLOYALTYLOSSONFAILURE=" << static_cast<UI16>( GetPetLoyaltyLossOnFailure() ) << '\n';
		ofsOutput << "PETLOYALTYRATE=" << SystemTimer( tSERVER_LOYALTYRATE ) << '\n';
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
		ofsOutput << "POSTINGLEVEL=" << static_cast<UI16>( MsgBoardPostingLevel() ) << '\n';
		ofsOutput << "REMOVALLEVEL=" << static_cast<UI16>( MsgBoardPostRemovalLevel() ) << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[escorts]" << '\n' << "{" << '\n';
		ofsOutput << "ESCORTENABLED=" << ( EscortsEnabled() ? 1 : 0 ) << '\n';
		ofsOutput << "ESCORTINITEXPIRE=" << SystemTimer( tSERVER_ESCORTWAIT ) << '\n';
		ofsOutput << "ESCORTACTIVEEXPIRE=" << SystemTimer( tSERVER_ESCORTACTIVE ) << '\n';
		ofsOutput << "ESCORTDONEEXPIRE=" << SystemTimer( tSERVER_ESCORTDONE ) << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[worldlight]" << '\n' << "{" << '\n';
		ofsOutput << "DUNGEONLEVEL=" << static_cast<UI16>( DungeonLightLevel() ) << '\n';
		ofsOutput << "BRIGHTLEVEL=" << static_cast<UI16>( WorldLightBrightLevel() ) << '\n';
		ofsOutput << "DARKLEVEL=" << static_cast<UI16>( WorldLightDarkLevel() ) << '\n';
		ofsOutput << "SECONDSPERUOMINUTE=" << ServerSecondsPerUOMinute() << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[tracking]" << '\n' << "{" << '\n';
		ofsOutput << "BASERANGE=" << TrackingBaseRange() << '\n';
		ofsOutput << "BASETIMER=" << TrackingBaseTimer() << '\n';
		ofsOutput << "MAXTARGETS=" << static_cast<UI16>( TrackingMaxTargets() ) << '\n';
		ofsOutput << "MSGREDISPLAYTIME=" << TrackingRedisplayTime() << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[reputation]" << '\n' << "{" << '\n';
		ofsOutput << "MURDERDECAYTIMER=" << SystemTimer( tSERVER_MURDERDECAY ) << '\n';
		ofsOutput << "MAXKILLS=" << RepMaxKills() << '\n';
		ofsOutput << "CRIMINALTIMER=" << SystemTimer( tSERVER_CRIMINAL ) << '\n';
		ofsOutput << "AGGRESSORFLAGTIMER=" << SystemTimer( tSERVER_AGGRESSORFLAG ) << '\n';
		ofsOutput << "PERMAGREYFLAGTIMER=" << SystemTimer( tSERVER_PERMAGREYFLAG ) << '\n';
		ofsOutput << "STEALINGFLAGTIMER=" << SystemTimer( tSERVER_STEALINGFLAG ) << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[resources]" << '\n' << "{" << '\n';
		ofsOutput << "RESOURCEAREASIZE=" << ResourceAreaSize() << '\n';
		ofsOutput << "MINECHECK=" << static_cast<UI16>( MineCheck() ) << '\n';
		ofsOutput << "OREPERAREA=" << ResOre() << '\n';
		ofsOutput << "ORERESPAWNTIMER=" << ResOreTime() << '\n';
		ofsOutput << "LOGSPERAREA=" << ResLogs() << '\n';
		ofsOutput << "LOGSRESPAWNTIMER=" << ResLogTime() << '\n';
		ofsOutput << "FISHPERAREA=" << ResFish() << '\n';
		ofsOutput << "FISHRESPAWNTIMER=" << ResFishTime() << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[hunger]" << '\n' << "{" << '\n';
		ofsOutput << "HUNGERENABLED=" << ( HungerSystemEnabled() ? 1 : 0 ) << '\n';
		ofsOutput << "HUNGERRATE=" << SystemTimer( tSERVER_HUNGERRATE ) << '\n';
		ofsOutput << "HUNGERDMGVAL=" << HungerDamage() << '\n';
		ofsOutput << "PETHUNGEROFFLINE=" << ( PetHungerOffline() ? 1 : 0 ) << '\n';
		ofsOutput << "PETOFFLINETIMEOUT=" << PetOfflineTimeout() << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[thirst]" << '\n' << "{" << '\n';
		ofsOutput << "THIRSTENABLED=" << ( ThirstSystemEnabled() ? 1 : 0 ) << '\n';
		ofsOutput << "THIRSTRATE=" << SystemTimer( tSERVER_THIRSTRATE ) << '\n';
		ofsOutput << "THIRSTDRAINVAL=" << ThirstDrain() << '\n';
		ofsOutput << "PETTHIRSTOFFLINE=" << ( PetThirstOffline() ? 1 : 0 ) << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[combat]" << '\n' << "{" << '\n';
		ofsOutput << "MAXRANGE=" << CombatMaxRange() << '\n';
		ofsOutput << "SPELLMAXRANGE=" << CombatMaxSpellRange() << '\n';
		ofsOutput << "DISPLAYHITMSG=" << ( CombatDisplayHitMessage() ? 1 : 0 ) << '\n';
		ofsOutput << "DISPLAYDAMAGENUMBERS=" << ( CombatDisplayDamageNumbers() ? 1 : 0 ) << '\n';
		ofsOutput << "MONSTERSVSANIMALS=" << ( CombatMonstersVsAnimals() ? 1 : 0 ) << '\n';
		ofsOutput << "ANIMALATTACKCHANCE=" << static_cast<UI16>( CombatAnimalsAttackChance() ) << '\n';
		ofsOutput << "ANIMALSGUARDED=" << ( CombatAnimalsGuarded() ? 1 : 0 ) << '\n';
		ofsOutput << "NPCDAMAGERATE=" << CombatNpcDamageRate() << '\n';
		ofsOutput << "NPCBASEFLEEAT=" << CombatNPCBaseFleeAt() << '\n';
		ofsOutput << "NPCBASEREATTACKAT=" << CombatNPCBaseReattackAt() << '\n';
		ofsOutput << "ATTACKSTAMINA=" << CombatAttackStamina() << '\n';
		ofsOutput << "ATTACKSPEEDFROMSTAMINA=" << ( CombatAttackSpeedFromStamina() ? 1 : 0 ) << '\n';
		ofsOutput << "ARCHERYHITBONUS=" << static_cast<SI16>( CombatArcheryHitBonus() ) << '\n';
		ofsOutput << "ARCHERYSHOOTDELAY=" << CombatArcheryShootDelay() << '\n';
		ofsOutput << "SHOOTONANIMALBACK=" << ( ShootOnAnimalBack() ? 1 : 0 ) << '\n';
		ofsOutput << "WEAPONDAMAGECHANCE=" << static_cast<UI16>( CombatWeaponDamageChance() ) << '\n';
		ofsOutput << "WEAPONDAMAGEMIN=" << static_cast<UI16>( CombatWeaponDamageMin() ) << '\n';
		ofsOutput << "WEAPONDAMAGEMAX=" << static_cast<UI16>( CombatWeaponDamageMax() ) << '\n';
		ofsOutput << "ARMORDAMAGECHANCE=" << static_cast<UI16>( CombatArmorDamageChance() ) << '\n';
		ofsOutput << "ARMORDAMAGEMIN=" << static_cast<UI16>( CombatArmorDamageMin() ) << '\n';
		ofsOutput << "ARMORDAMAGEMAX=" << static_cast<UI16>( CombatArmorDamageMax() ) << '\n';
		ofsOutput << "PARRYDAMAGECHANCE=" << static_cast<UI16>( CombatParryDamageChance() ) << '\n';
		ofsOutput << "PARRYDAMAGEMIN=" << static_cast<UI16>( CombatParryDamageMin() ) << '\n';
		ofsOutput << "PARRYDAMAGEMAX=" << static_cast<UI16>( CombatParryDamageMax() ) << '\n';
		ofsOutput << "ARMORCLASSDAMAGEBONUS=" << ( CombatArmorClassDamageBonus() ? 1 : 0 ) << '\n';
		ofsOutput << "ALCHEMYBONUSENABLED=" << ( AlchemyDamageBonusEnabled() ? 1 : 0 ) << '\n';
		ofsOutput << "ALCHEMYBONUSMODIFIER=" << static_cast<UI16>( AlchemyDamageBonusModifier() ) << '\n';
		ofsOutput << "BLOODEFFECTCHANCE=" << static_cast<UI16>( CombatBloodEffectChance() ) << '\n';
		ofsOutput << "ITEMSINTERRUPTCASTING=" << ( ItemsInterruptCasting() ? 1 : 0 ) << '\n';
		ofsOutput << "PETCOMBATTRAINING=" << ( PetCombatTraining() ? 1 : 0 ) << '\n';
		ofsOutput << "HIRELINGCOMBATTRAINING=" << ( HirelingCombatTraining() ? 1 : 0 ) << '\n';
		ofsOutput << "NPCCOMBATTRAINING=" << ( NpcCombatTraining() ? 1 : 0 ) << '\n';
		ofsOutput << "SHOWITEMRESISTSTATS=" << ( ShowItemResistStats() ? 1 : 0 ) << '\n';
		ofsOutput << "SHOWWEAPONDAMAGETYPES=" << ( ShowWeaponDamageTypes() ? 1 : 0 ) << '\n';
		ofsOutput << "WEAPONDAMAGEBONUSTYPE=" << static_cast<UI16>( WeaponDamageBonusType() ) << '\n';
		ofsOutput << "PHYSICALRESISTCAP=" << PhysicalResistCap() << '\n';
		ofsOutput << "FIRERESISTCAP=" << FireResistCap() << '\n';
		ofsOutput << "COLDRESISTCAP=" << ColdResistCap() << '\n';
		ofsOutput << "POISONRESISTCAP=" << PoisonResistCap() << '\n';
		ofsOutput << "ENERGYRESISTCAP=" << EnergyResistCap() << '\n';
		ofsOutput << "DEFENSECHANCEINCREASECAP=" << DefenseChanceIncreaseCap() << '\n';

		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[magic]" << '\n' << "{" << '\n';
		ofsOutput << "TRAVELSPELLSFROMBOATKEYS=" << ( TravelSpellsFromBoatKeys() ? 1 : 0 ) << '\n';
		ofsOutput << "TRAVELSPELLSWHILEOVERWEIGHT=" << ( TravelSpellsWhileOverweight() ? 1 : 0 ) << '\n';
		ofsOutput << "MARKRUNESINMULTIS=" << ( MarkRunesInMultis() ? 1 : 0 ) << '\n';
		ofsOutput << "TRAVELSPELLSBETWEENWORLDS=" << ( TravelSpellsBetweenWorlds() ? 1 : 0 ) << '\n';
		ofsOutput << "TRAVELSPELLSWHILEAGGRESSOR=" << ( TravelSpellsWhileAggressor() ? 1 : 0 ) << '\n';
		ofsOutput << "HIDESTATSFORUNKNOWNMAGICITEMS=" << HideStatsForUnknownMagicItems() << '\n';
		ofsOutput << "CASTSPELLSWHILEMOVING=" << ( CastSpellsWhileMoving() ? 1 : 0 ) << '\n';
		ofsOutput << "AUTOUNEQUIPPEDCASTING=" << ( AutoUnequippedCasting() ? 1 : 0 ) << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[start locations]" << '\n' << "{" << '\n';
		for( size_t lCtr = 0; lCtr < startlocations.size(); ++lCtr )
		{
			ofsOutput << "LOCATION=" << startlocations[lCtr].newTown << "," << startlocations[lCtr].newDescription << "," << startlocations[lCtr].x << "," << startlocations[lCtr].y << "," << startlocations[lCtr].z << "," << startlocations[lCtr].worldNum << "," << startlocations[lCtr].instanceId << "," << startlocations[lCtr].clilocDesc << '\n';
		}
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[young start locations]" << '\n' << "{" << '\n';
		for( size_t lCtr = 0; lCtr < youngStartlocations.size(); ++lCtr )
		{
			ofsOutput << "YOUNGLOCATION=" << youngStartlocations[lCtr].newTown << "," << youngStartlocations[lCtr].newDescription << "," << youngStartlocations[lCtr].x << "," << youngStartlocations[lCtr].y << "," << youngStartlocations[lCtr].z << "," << youngStartlocations[lCtr].worldNum << "," << youngStartlocations[lCtr].instanceId << "," << youngStartlocations[lCtr].clilocDesc << '\n';
		}
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

		ofsOutput << '\n' << "[houses]" << '\n' << "{" << '\n';
		ofsOutput << "TRACKHOUSESPERACCOUNT=" << ( TrackHousesPerAccount() ? 1 : 0 ) << '\n';
		ofsOutput << "CANOWNANDCOOWNHOUSES=" << ( CanOwnAndCoOwnHouses() ? 1 : 0 ) << '\n';
		ofsOutput << "COOWNHOUSESONSAMEACCOUNT=" << ( CoOwnHousesOnSameAccount() ? 1 : 0 ) << '\n';
		ofsOutput << "ITEMDECAYINHOUSES=" << ( ItemDecayInHouses() ? 1 : 0 ) << '\n';
		ofsOutput << "PROTECTPRIVATEHOUSES=" << ( ProtectPrivateHouses() ? 1 : 0 ) << '\n';
		ofsOutput << "MAXHOUSESOWNABLE=" << MaxHousesOwnable() << '\n';
		ofsOutput << "MAXHOUSESCOOWNABLE=" << MaxHousesCoOwnable() << '\n';
		ofsOutput << "SAFECOOWNERLOGOUT=" << ( SafeCoOwnerLogout() ? 1 : 0 ) << '\n';
		ofsOutput << "SAFEFRIENDLOGOUT=" << ( SafeFriendLogout() ? 1 : 0 ) << '\n';
		ofsOutput << "SAFEGUESTLOGOUT=" << ( SafeGuestLogout() ? 1 : 0 ) << '\n';
		ofsOutput << "KEYLESSOWNERACCESS=" << ( KeylessOwnerAccess() ? 1 : 0 ) << '\n';
		ofsOutput << "KEYLESSCOOWNERACCESS=" << ( KeylessCoOwnerAccess() ? 1 : 0 ) << '\n';
		ofsOutput << "KEYLESSFRIENDACCESS=" << ( KeylessFriendAccess() ? 1 : 0 ) << '\n';
		ofsOutput << "KEYLESSGUESTACCESS=" << ( KeylessGuestAccess() ? 1 : 0 ) << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[bulk order deeds]" << '\n' << "{" << '\n';
		ofsOutput << "OFFERBODSFROMITEMSALES=" << ( OfferBODsFromItemSales() ? 1 : 0 ) << '\n';
		ofsOutput << "OFFERBODSFROMCONTEXTMENU=" << ( OfferBODsFromContextMenu() ? 1 : 0 ) << '\n';
		ofsOutput << "BODSFROMCRAFTEDITEMSONLY=" << ( BODsFromCraftedItemsOnly() ? 1 : 0 ) << '\n';
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
		ofsOutput << "AF_FILTERWEATHER=" << ( GetDisabledAssistantFeature( AF_FILTERWEATHER ) ? 1 : 0 ) << '\n';
		ofsOutput << "AF_FILTERLIGHT=" << ( GetDisabledAssistantFeature( AF_FILTERLIGHT ) ? 1 : 0 ) << '\n';
		ofsOutput << "AF_SMARTTARGET=" << ( GetDisabledAssistantFeature( AF_SMARTTARGET ) ? 1 : 0 ) << '\n';
		ofsOutput << "AF_RANGEDTARGET=" << ( GetDisabledAssistantFeature( AF_RANGEDTARGET ) ? 1 : 0 ) << '\n';
		ofsOutput << "AF_AUTOOPENDOORS=" << ( GetDisabledAssistantFeature( AF_AUTOOPENDOORS ) ? 1 : 0 ) << '\n';
		ofsOutput << "AF_DEQUIPONCAST=" << ( GetDisabledAssistantFeature( AF_DEQUIPONCAST ) ? 1 : 0 ) << '\n';
		ofsOutput << "AF_AUTOPOTIONEQUIP=" << ( GetDisabledAssistantFeature( AF_AUTOPOTIONEQUIP ) ? 1 : 0 ) << '\n';
		ofsOutput << "AF_POISONEDCHECKS=" << ( GetDisabledAssistantFeature( AF_POISONEDCHECKS ) ? 1 : 0 ) << '\n';
		ofsOutput << "AF_LOOPEDMACROS=" << ( GetDisabledAssistantFeature( AF_LOOPEDMACROS ) ? 1 : 0 ) << '\n';
		ofsOutput << "AF_USEONCEAGENT=" << ( GetDisabledAssistantFeature( AF_USEONCEAGENT ) ? 1 : 0 ) << '\n';
		ofsOutput << "AF_RESTOCKAGENT=" << ( GetDisabledAssistantFeature( AF_RESTOCKAGENT ) ? 1 : 0 ) << '\n';
		ofsOutput << "AF_SELLAGENT=" << ( GetDisabledAssistantFeature( AF_SELLAGENT ) ? 1 : 0 ) << '\n';
		ofsOutput << "AF_BUYAGENT=" << ( GetDisabledAssistantFeature( AF_BUYAGENT ) ? 1 : 0 ) << '\n';
		ofsOutput << "AF_POTIONHOTKEYS=" << ( GetDisabledAssistantFeature( AF_POTIONHOTKEYS ) ? 1 : 0 ) << '\n';
		ofsOutput << "AF_RANDOMTARGETS=" << ( GetDisabledAssistantFeature( AF_RANDOMTARGETS ) ? 1 : 0 ) << '\n';
		ofsOutput << "AF_CLOSESTTARGETS=" << ( GetDisabledAssistantFeature( AF_CLOSESTTARGETS ) ? 1 : 0 ) << '\n';
		ofsOutput << "AF_OVERHEADHEALTH=" << ( GetDisabledAssistantFeature( AF_OVERHEADHEALTH ) ? 1 : 0 ) << '\n';
		ofsOutput << "AF_AUTOLOOTAGENT=" << ( GetDisabledAssistantFeature( AF_AUTOLOOTAGENT ) ? 1 : 0 ) << '\n';
		ofsOutput << "AF_BONECUTTERAGENT=" << ( GetDisabledAssistantFeature( AF_BONECUTTERAGENT ) ? 1 : 0 ) << '\n';
		ofsOutput << "AF_JSCRIPTMACROS=" << ( GetDisabledAssistantFeature( AF_JSCRIPTMACROS ) ? 1 : 0 ) << '\n';
		ofsOutput << "AF_AUTOREMOUNT=" << ( GetDisabledAssistantFeature( AF_AUTOREMOUNT ) ? 1 : 0 ) << '\n';
		ofsOutput << "AF_AUTOBANDAGE=" << ( GetDisabledAssistantFeature( AF_AUTOBANDAGE ) ? 1 : 0 ) << '\n';
		ofsOutput << "AF_ENEMYTARGETSHARE=" << ( GetDisabledAssistantFeature( AF_ENEMYTARGETSHARE ) ? 1 : 0 ) << '\n';
		ofsOutput << "AF_FILTERSEASON=" << ( GetDisabledAssistantFeature( AF_FILTERSEASON ) ? 1 : 0 ) << '\n';
		ofsOutput << "AF_SPELLTARGETSHARE=" << ( GetDisabledAssistantFeature( AF_SPELLTARGETSHARE ) ? 1 : 0 ) << '\n';
		ofsOutput << "AF_HUMANOIDHEALTHCHECKS=" << ( GetDisabledAssistantFeature( AF_HUMANOIDHEALTHCHECKS ) ? 1 : 0 ) << '\n';
		ofsOutput << "AF_SPEECHJOURNALCHECKS=" << ( GetDisabledAssistantFeature( AF_SPEECHJOURNALCHECKS ) ? 1 : 0 ) << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput.close();
		rValue = true;
	}
	else
	{
		Console.Error( oldstrutil::format( "Unable to open file %s for writing", filename.c_str() ));
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::Load()
//|	Date		-	January 13, 2001
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Load up the uox.ini file and parse it into the internals
//|	Returns		-	pointer to the valid inmemory serverdata storage(this)
//|						nullptr is there is an error, or invalid file type
//o------------------------------------------------------------------------------------------------o
auto CServerData::Load( const std::string &filename ) -> bool
{
	auto iniFile = filename;
	if( iniFile.empty() )
	{
		iniFile = Directory( CSDDP_ROOT ) + "uox.ini"s;
	}
	actualINI = iniFile;
	auto rValue = ParseIni( iniFile );
	PostLoadDefaults();
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::TrackingBaseRange()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the base (minimum) range even beginner trackers can track at
//o------------------------------------------------------------------------------------------------o
auto CServerData::TrackingBaseRange() const -> UI16
{
	return trackingBaseRange;
}
auto CServerData::TrackingBaseRange( UI16 value ) -> void
{
	trackingBaseRange = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::TrackingMaxTargets()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max amount of targets someone can see in the tracking window
//o------------------------------------------------------------------------------------------------o
auto CServerData::TrackingMaxTargets() const -> UI08
{
	return trackingMaxTargets;
}
auto CServerData::TrackingMaxTargets( UI08 value ) -> void
{
	if( value >= MAX_TRACKINGTARGETS )
	{
		trackingMaxTargets = MAX_TRACKINGTARGETS;
	}
	else
	{
		trackingMaxTargets = value;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::TrackingBaseTimer()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max amount of time a grandmaster tracker can track someone
//o------------------------------------------------------------------------------------------------o
auto CServerData::TrackingBaseTimer() const -> UI16
{
	return trackingBaseTimer;
}
auto CServerData::TrackingBaseTimer( UI16 value ) -> void
{
	trackingBaseTimer = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::TrackingRedisplayTime()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets how often in seconds the tracking message is updated and displayed
//o------------------------------------------------------------------------------------------------o
auto CServerData::TrackingRedisplayTime() const -> UI16
{
	return trackingMsgRedisplayTimer;
}
auto CServerData::TrackingRedisplayTime( UI16 value ) -> void
{
	trackingMsgRedisplayTimer = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::PhysicalResistCap()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the for Physical cap propertie
//o------------------------------------------------------------------------------------------------o
auto CServerData::PhysicalResistCap() const -> SI16
{
	return physicalResistCap;
}
auto CServerData::PhysicalResistCap( SI16 value ) -> void
{
	physicalResistCap = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::FireResistCap()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the for Fire cap propertie
//o------------------------------------------------------------------------------------------------o
auto CServerData::FireResistCap() const -> SI16
{
	return fireResistCap;
}
auto CServerData::FireResistCap( SI16 value ) -> void
{
	fireResistCap = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ColdResistCap()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the for Cold cap propertie
//o------------------------------------------------------------------------------------------------o
auto CServerData::ColdResistCap() const -> SI16
{
	return coldResistCap;
}
auto CServerData::ColdResistCap( SI16 value ) -> void
{
	coldResistCap = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::PoisonResistCap()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the for Poison cap propertie
//o------------------------------------------------------------------------------------------------o
auto CServerData::PoisonResistCap() const -> SI16
{
	return poisonResistCap;
}
auto CServerData::PoisonResistCap( SI16 value ) -> void
{
	poisonResistCap = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::EnergyResistCap()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the for Energy cap propertie
//o------------------------------------------------------------------------------------------------o
auto CServerData::EnergyResistCap() const -> SI16
{
	return energyResistCap;
}
auto CServerData::EnergyResistCap( SI16 value ) -> void
{
	energyResistCap = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::DefenseChanceIncreaseCap()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the for Defense Chance Increase cap propertie
//o------------------------------------------------------------------------------------------------o
auto CServerData::DefenseChanceIncreaseCap() const -> SI16
{
	return defenseChanceIncreaseCap;
}
auto CServerData::DefenseChanceIncreaseCap( SI16 value ) -> void
{
	defenseChanceIncreaseCap = value;
}


//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ParseIni()
//|	Date		-	02/26/2001
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Parse the uox.ini file into its required information.
//|
//|	Changes		-	02/26/2002	-	Make sure that we parse out the logs, access
//|									and other directories that we were not parsing/
//o------------------------------------------------------------------------------------------------o
auto CServerData::ParseIni( const std::string& filename ) -> bool
{
	auto rValue = false;
	auto input = std::ifstream( filename );
	enum search_t { header, startsection, endsection };
	auto retrieveContents = []( const std::string& input ) -> std::optional<std::string>
	{
		auto pos1 = input.find("[" );
		if( pos1 != std::string::npos )
		{
			auto pos2 = input.find( "]", pos1 );
			if( pos2 != std::string::npos )
			{
				// We have the bounds
				auto contents = oldstrutil::trim( input.substr( pos1 + 1, ( pos2 - pos1 ) - 1 ));
				return contents;
			}
		}
		return {};
	};
	auto state = search_t( header );
	if( input.is_open() )
	{
		startlocations.clear();
		youngStartlocations.clear();
		char input_buffer[4096];
		while( input.good() && !input.eof() )
		{
			input.getline( input_buffer, 4095 );
			if( input.gcount() > 0 )
			{
				input_buffer[input.gcount()] = 0;
				auto line = oldstrutil::trim( oldstrutil::removeTrailing( std::string( input_buffer ), "//" ));
				if( !line.empty() )
				{
					switch( static_cast<int>( state ))
					{
						case static_cast<int>( search_t::header ):
						{
							if( line[0] == '[' )
							{
								auto contents = retrieveContents( line );
								if( contents.has_value() )
								{
									state = search_t::startsection;
								}
							}
						}
						[[fallthrough]];
						case static_cast<int>( search_t::startsection ):
						{
							if( line[0] == '{' )
							{
								state = search_t::endsection;
							}
						}
						[[fallthrough]];
						case static_cast<int>( search_t::endsection ):
						{
							if( line[0] != '}' )
							{
								auto [key, value] = oldstrutil::split( line, "=" );
								try
								{
									if( HandleLine( key, value ))
									{
										rValue = true;
									}
								}
								catch( const std::exception &e )
								{
									Console.Error( "Error parsing ini file" );
									Console.Error( oldstrutil::format( "Entry was: %s = %s", key.c_str(), value.c_str() ));
									Console.Error( oldstrutil::format( "Exception was: %s", e.what() ));
									exit( 1 );
								}
							}
							else
							{
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
	 
	 Console << "Processing INI Settings  ";
	 
	 Script toParse( filename, NUM_DEFS, false );
	 // Lock this file tight, No access at anytime when open(should only be open and closed anyhow. For Thread blocking)
	 if( !toParse.IsErrored() ) {
	 //serverList.clear();
	 startlocations.clear();
	 for (auto &[id,section] : toParse.collection() ){
	 if( section){
	 for (auto &sec : section->collection() ){
	 auto tag = sec->tag;
	 auto data = oldstrutil::simplify( sec->data );
	 if( !HandleLine( tag, data )) {
	 Console.Warning( oldstrutil::format( "Unhandled tag '%s'", tag.c_str() ));
	 }
	 }
	 }
	 }
	 Console.PrintDone();
	 rValue = true;
	 }
	 else
	 {
	 Console.Warning( oldstrutil::format( "%s File not found, Using default settings.", filename.c_str() ));
	 cwmWorldState->ServerData()->save();
	 }
	 }
	 return rValue;
	 */
}

//==================================================================================================
auto CServerData::HandleLine( const std::string& tag, const std::string& value ) -> bool
{
	bool rValue = true;
	auto titer = uox3IniCaseValue.find( tag );
	if( titer == uox3IniCaseValue.end() )
	{
		return false;
	}

	switch( titer->second )
	{
		case 1:	 // SERVERNAME
			ServerName( value );
			break;
		case 2:	 // CONSOLELOG
			ServerConsoleLog(( static_cast<UI16>( std::stoul( value, nullptr, 0 )) >= 1 ? true : false ));
			break;
		case 3:	 // COMMANDPREFIX
			ServerCommandPrefix(( value.data()[0] ));	// return the first character of the return string only
			break;
		case 4:	 // ANNOUNCEWORLDSAVES
			ServerAnnounceSaves(( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 ? true : false ));
			break;
		case 26:	 // JOINPARTMSGS
			ServerJoinPartAnnouncements(( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 ? true : false ));
			break;
		case 5:	 // BACKUPSENABLED
			ServerBackups(( static_cast<UI16>( std::stoul( value, nullptr, 0 )) > 0 ? true : false ));
			break;
		case 6:	 // SAVESTIMER
			ServerSavesTimer( static_cast<UI32>( std::stoul( value, nullptr, 0 )));
			break;
		case 7:	 // SKILLCAP
			ServerSkillTotalCap( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 8:	 // SKILLDELAY
			ServerSkillDelay( static_cast<UI08>( std::stoul( value, nullptr, 0 )));
			break;
		case 9:	 // STATCAP
			ServerStatCap( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 10:	 // MAXSTEALTHMOVEMENTS
			MaxStealthMovement( static_cast<SI16>( std::stoi( value, nullptr, 0 )));
			break;
		case 11:	 // MAXSTAMINAMOVEMENTS
			MaxStaminaMovement( static_cast<SI16>( std::stoi( value, nullptr, 0 )));
			break;
		case 12:	 // ARMORAFFECTMANAREGEN
			ArmorAffectManaRegen(( static_cast<UI08>( std::stoul( value, nullptr, 0 )) > 0 ? true : false ));
			break;
		case 13:	 // CORPSEDECAYTIMER
			SystemTimer( tSERVER_CORPSEDECAY, static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 14:	 // WEATHERTIMER
			SystemTimer( tSERVER_WEATHER, static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 15:	 // SHOPSPAWNTIMER
			SystemTimer( tSERVER_SHOPSPAWN, static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 16:	 // DECAYTIMER
			SystemTimer( tSERVER_DECAY, static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 17:	 // INVISIBILITYTIMER
			SystemTimer( tSERVER_INVISIBILITY, static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 18:	 // OBJECTUSETIMER
			SystemTimer( tSERVER_OBJECTUSAGE, static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 19:	 // GATETIMER
			SystemTimer( tSERVER_GATE, static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 20:	 // POISONTIMER
			SystemTimer( tSERVER_POISON, static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 21:	 // LOGINTIMEOUT
			SystemTimer( tSERVER_LOGINTIMEOUT, static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 22:	 // HITPOINTREGENTIMER
			SystemTimer( tSERVER_HITPOINTREGEN, static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 23:	 // STAMINAREGENTIMER
			SystemTimer( tSERVER_STAMINAREGEN, static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 24:	 // BASEFISHINGTIMER
			SystemTimer( tSERVER_FISHINGBASE, static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 34:	// MAXPETOWNERS
			MaxPetOwners( static_cast<UI08>( std::stoul( value, nullptr, 0 )));
			break;
		case 35:	// MAXFOLLOWERS
			MaxFollowers( static_cast<UI08>( std::stoul( value, nullptr, 0 )));
			break;
		case 36:	// MAXCONTROLSLOTS
			MaxControlSlots( static_cast<UI08>( std::stoul( value, nullptr, 0 )));
			break;
		case 37:	 // MANAREGENTIMER
			SystemTimer( tSERVER_MANAREGEN, static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 38:	 // RANDOMFISHINGTIMER
			SystemTimer( tSERVER_FISHINGRANDOM, static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 39:	 // SPIRITSPEAKTIMER
			SystemTimer( tSERVER_SPIRITSPEAK, static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 40:	 // DIRECTORY
		{
			Directory( CSDDP_ROOT, value );
			break;
		}
		case 41:	 // DATADIRECTORY
		{
			Directory( CSDDP_DATA, value );
			break;
		}
		case 42:	 // DEFSDIRECTORY
		{
			Directory( CSDDP_DEFS, value );
			break;
		}
		case 43:	 // ACTSDIRECTORY
		{
			Directory( CSDDP_ACCOUNTS, value );
			break;
		}
		case 25:	 // SCRIPTSDIRECTORY
		{
			Directory( CSDDP_SCRIPTS, value );
			break;
		}
		case 44:	 // BACKUPDIRECTORY
		{
			Directory( CSDDP_BACKUP, value );
			break;
		}
		case 45:	 // MSGBOARDDIRECTORY
		{
			Directory( CSDDP_MSGBOARD, value );
			break;
		}
		case 46:	 // SHAREDDIRECTORY
		{
			Directory( CSDDP_SHARED, value );
			break;
		}
		case 47:	 // LOOTDECAYSWITHPLAYERCORPSE
			PlayerCorpseLootDecay( static_cast<UI16>( std::stoul( value, nullptr, 0 )) != 0 );
			break;
		case 49:	 // GUARDSACTIVE
			GuardStatus( static_cast<UI16>( std::stoul( value, nullptr, 0 )) != 0 );
			break;
		case 27:	 // DEATHANIMATION
			DeathAnimationStatus( static_cast<UI16>( std::stoul( value, nullptr, 0 )) != 0 );
			break;
		case 50:	 // AMBIENTSOUNDS
			WorldAmbientSounds( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 51:	 // AMBIENTFOOTSTEPS
			AmbientFootsteps( static_cast<UI16>( std::stoul( value, nullptr, 0 )) != 0 );
			break;
		case 52:	 // INTERNALACCOUNTCREATION
			InternalAccountStatus( static_cast<UI16>( std::stoul( value, nullptr, 0 ))!= 0 );
			break;
		case 53:	 // SHOWOFFLINEPCS
			ShowOfflinePCs( static_cast<UI16>( std::stoul( value, nullptr, 0 )) != 0 );
			break;
		case 54:	 // ROGUESENABLED
			RogueStatus( static_cast<UI16>( std::stoul( value, nullptr, 0 )) != 0 );
			break;
		case 55:	 // PLAYERPERSECUTION
			PlayerPersecutionStatus( static_cast<UI16>( std::stoul( value, nullptr, 0 )) != 0 );
			break;
		case 56:	 // ACCOUNTFLUSH
			AccountFlushTimer( std::stod( value ));
			break;
		case 57:	 // HTMLSTATUSENABLED
			HtmlStatsStatus( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 58:	 // SELLBYNAME
			SellByNameStatus( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 );
			break;
		case 59:	 // SELLMAXITEMS
			SellMaxItemsStatus( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 60:	 // TRADESYSTEM
			TradeSystemStatus( static_cast<UI16>( std::stoul( value, nullptr, 0 )) != 0 );
			break;
		case 61:	 // RANKSYSTEM
			RankSystemStatus( static_cast<UI16>( std::stoul( value, nullptr, 0 )) != 0 );
			break;
		case 62:	 // CUTSCROLLREQUIREMENTS
			CutScrollRequirementStatus( static_cast<UI16>( std::stoul( value, nullptr, 0 )) != 0);
			break;
		case 63:	 // CHECKITEMS
			CheckItemsSpeed( std::stod( value ));
			break;
		case 64:	 // CHECKBOATS
			CheckBoatSpeed( std::stod( value ));
			break;
		case 65:	 // CHECKNPCAI
			CheckNpcAISpeed( std::stod( value ));
			break;
		case 66:	 // CHECKSPAWNREGIONS
			CheckSpawnRegionSpeed( std::stod( value ));
			break;
		case 67:	 // POSTINGLEVEL
			MsgBoardPostingLevel( static_cast<UI08>( std::stoul( value, nullptr, 0 )));
			break;
		case 68:	 // REMOVALLEVEL
			MsgBoardPostRemovalLevel( static_cast<UI08>( std::stoul( value, nullptr, 0 )));
			break;
		case 69:	 // ESCORTENABLED
			EscortsEnabled( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 );
			break;
		case 70:	 // ESCORTINITEXPIRE
			SystemTimer( tSERVER_ESCORTWAIT, static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 71:	 // ESCORTACTIVEEXPIRE
			SystemTimer( tSERVER_ESCORTACTIVE, static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 72:	 // MOON1
			ServerMoon( 0, static_cast<SI16>( std::stoi( value, nullptr, 0 )));
			break;
		case 73:	 // MOON2
			ServerMoon( 1, static_cast<SI16>( std::stoi( value, nullptr, 0 )));
			break;
		case 74:	 // DUNGEONLEVEL
			DungeonLightLevel( static_cast<LIGHTLEVEL>( std::stoul( value, nullptr, 0 )));
			break;
		case 75:	 // CURRENTLEVEL
			WorldLightCurrentLevel( static_cast<LIGHTLEVEL>( std::stoul( value, nullptr, 0 )));
			break;
		case 76:	 // BRIGHTLEVEL
			WorldLightBrightLevel( static_cast<LIGHTLEVEL>( std::stoul( value, nullptr, 0 )));
			break;
		case 77:	 // BASERANGE
			TrackingBaseRange( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 78:	 // BASETIMER
			TrackingBaseTimer( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 79:	 // MAXTARGETS
			TrackingMaxTargets( static_cast<UI08>( std::stoul( value, nullptr, 0 )));
			break;
		case 80:	 // MSGREDISPLAYTIME
			TrackingRedisplayTime( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 81:	 // MURDERDECAYTIMER
			SystemTimer( tSERVER_MURDERDECAY, static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 82:	 // MAXKILLS
			RepMaxKills( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 83:	 // CRIMINALTIMER
			SystemTimer( tSERVER_CRIMINAL, static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 84:	 // MINECHECK
			MineCheck( static_cast<UI08>( std::stoul( value, nullptr, 0 )));
			break;
		case 85:	 // OREPERAREA
			ResOre( static_cast<SI16>( std::stoi( value, nullptr, 0 )));
			break;
		case 86:	 // ORERESPAWNTIMER
			ResOreTime( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 87:	 // RESOURCEAREASIZE
			ResourceAreaSize( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 88:	 // LOGSPERAREA
			ResLogs( static_cast<SI16>( std::stoi( value, nullptr, 0 )));
			break;
		case 89:	 // LOGSRESPAWNTIMER
			ResLogTime( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 90:	 // STATSAFFECTSKILLCHECKS
			StatsAffectSkillChecks( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 );
			break;
		case 91:	 // HUNGERRATE
			SystemTimer( tSERVER_HUNGERRATE, static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 92:	 // HUNGERDMGVAL
			HungerDamage( static_cast<SI16>( std::stoi( value, nullptr, 0 )));
			break;
		case 93:	 // MAXRANGE
			CombatMaxRange( static_cast<SI16>( std::stoi( value, nullptr, 0 )));
			break;
		case 94:	 // SPELLMAXRANGE
			CombatMaxSpellRange( static_cast<SI16>( std::stoi( value, nullptr, 0 )));
			break;
		case 95:	 // DISPLAYHITMSG
			CombatDisplayHitMessage( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 );
			break;
		case 96:	 // MONSTERSVSANIMALS
			CombatMonstersVsAnimals( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 );
			break;
		case 97:	 // ANIMALATTACKCHANCE
			CombatAnimalsAttackChance( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 98:	 // ANIMALSGUARDED
			CombatAnimalsGuarded( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 );
			break;
		case 99:	 // NPCDAMAGERATE
			CombatNpcDamageRate( static_cast<SI16>( std::stoi( value, nullptr, 0 )));
			break;
		case 100:	 // NPCBASEFLEEAT
			CombatNPCBaseFleeAt( static_cast<SI16>( std::stoi( value, nullptr, 0 )));
			break;
		case 101:	 // NPCBASEREATTACKAT
			CombatNPCBaseReattackAt( static_cast<SI16>( std::stoi( value, nullptr, 0 )));
			break;
		case 102:	 // ATTACKSTAMINA
			CombatAttackStamina( static_cast<SI16>( std::stoi( value, nullptr, 0 )));
			break;
		case 103:	 // LOCATION
			ServerLocation( value );
			break;
		case 104:	 // STARTGOLD
			ServerStartGold( static_cast<SI16>( std::stoi( value, nullptr, 0 )));
			break;
		case 105:	 // STARTPRIVS
			ServerStartPrivs( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 106:	 // ESCORTDONEEXPIRE
			SystemTimer( tSERVER_ESCORTDONE, static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 107:	 // DARKLEVEL
			WorldLightDarkLevel( static_cast<LIGHTLEVEL>( std::stoul( value, nullptr, 0 )));
			break;
		case 108:	 // TITLECOLOUR
			TitleColour( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 109:	 // LEFTTEXTCOLOUR
			LeftTextColour( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 110:	 // RIGHTTEXTCOLOUR
			RightTextColour( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 111:	 // BUTTONCANCEL
			ButtonCancel( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 112:	 // BUTTONLEFT
			ButtonLeft( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 113:	 // BUTTONRIGHT
			ButtonRight( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 114:	 // BACKGROUNDPIC
			BackgroundPic( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 115:	 // POLLTIME
			TownNumSecsPollOpen( static_cast<UI32>( std::stoul( value, nullptr, 0 )));
			break;
		case 116:	 // MAYORTIME
			TownNumSecsAsMayor( static_cast<UI32>( std::stoul( value, nullptr, 0 )));
			break;
		case 117:	 // TAXPERIOD
			TownTaxPeriod( static_cast<UI32>( std::stoul( value, nullptr, 0 )));
			break;
		case 118:	 // GUARDSPAID
			TownGuardPayment( static_cast<UI32>( std::stoul( value, nullptr, 0 )));
			break;
		case 119:	 // DAY
			ServerTimeDay( static_cast<SI16>( std::stoi( value, nullptr, 0 )));
			break;
		case 120:	 // HOURS
			ServerTimeHours( static_cast<UI08>( std::stoul( value, nullptr, 0 )));
			break;
		case 121:	 // MINUTES
			ServerTimeMinutes( static_cast<UI08>( std::stoul( value, nullptr, 0 )));
			break;
		case 122:	 // SECONDS
			ServerTimeSeconds( static_cast<UI08>( std::stoul( value, nullptr, 0 )));
			break;
		case 123:	 // AMPM
			ServerTimeAMPM( static_cast<UI16>( std::stoul( value, nullptr, 0 )) != 0 );
			break;
		case 124:	 // SKILLLEVEL
			SkillLevel( static_cast<UI08>( std::stoul( value, nullptr, 0 )));
			break;
		case 125:	 // SNOOPISCRIME
			SnoopIsCrime( static_cast<UI16>( std::stoul( value, nullptr, 0 )) != 0 );
			break;
		case 126:	 // BOOKSDIRECTORY
			Directory( CSDDP_BOOKS, value );
			break;
		case 127:	 // SERVERLIST
			break;
		case 128:	 // PORT
			ServerPort( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 129:	 // ACCESSDIRECTORY
			Directory( CSDDP_ACCESS, value );
			break;
		case 130:	 // LOGSDIRECTORY
			Directory( CSDDP_LOGS, value );
			break;
		case 131:	 // ACCOUNTISOLATION
			break;
		case 132:	 // HTMLDIRECTORY
			Directory( CSDDP_HTML, value );
			break;
		case 133:	 // SHOOTONANIMALBACK
			ShootOnAnimalBack( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 );
			break;
		case 134:	 // NPCTRAININGENABLED
			NPCTrainingStatus( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 );
			break;
		case 135:	 // DICTIONARYDIRECTORY
			Directory( CSDDP_DICTIONARIES, value );
			break;
		case 136:	 // BACKUPSAVERATIO
			BackupRatio( static_cast<SI16>( std::stoi( value, nullptr, 0 )));
			break;
		case 137:	 // HIDEWHILEMOUNTED
			CharHideWhileMounted( static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 );
			break;
		case 138:	 // SECONDSPERUOMINUTE
			ServerSecondsPerUOMinute( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 139:	 // WEIGHTPERSTR
			//WeightPerStr( value.toUByte() );
			WeightPerStr( std::stof( value ));
			break;
		case 140:	 // POLYDURATION
			SystemTimer( tSERVER_POLYMORPH, static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 141:	 // UOGENABLED
			ServerUOGEnabled( static_cast<UI16>( std::stoul( value, nullptr, 0 ))==1 );
			break;
		case 142:	 // NETRCVTIMEOUT
			ServerNetRcvTimeout( static_cast<UI32>( std::stoul( value, nullptr, 0 )));
			break;
		case 143:	 // NETSNDTIMEOUT
			ServerNetSndTimeout( static_cast<UI32>( std::stoul( value, nullptr, 0 )));
			break;
		case 144:	 // NETRETRYCOUNT
			ServerNetRetryCount( static_cast<UI32>( std::stoul( value, nullptr, 0 )));
			break;
		case 145:	 // CLIENTFEATURES
			SetClientFeatures( static_cast<UI32>( std::stoul( value, nullptr, 0 )));
			break;
		case 146:	 // PACKETOVERLOADS
			ServerOverloadPackets(( static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 ));
			break;
		case 147:	 // NPCMOVEMENTSPEED
			NPCWalkingSpeed( std::stof( value ));
			break;
		case 148:	 // PETHUNGEROFFLINE
			PetHungerOffline(( static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 ));
			break;
		case 149:	 // PETOFFLINETIMEOUT
			PetOfflineTimeout( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 150:	 // PETOFFLINECHECKTIMER
			SystemTimer( tSERVER_PETOFFLINECHECK, static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 152:	 // ADVANCEDPATHFINDING
			AdvancedPathfinding(( static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 ));
			break;
		case 153:	 // SERVERFEATURES
			SetServerFeatures( static_cast<UI32>( std::stoul( value, nullptr, 0 )));
			break;
		case 154:	 // LOOTINGISCRIME
			LootingIsCrime(( static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 ));
			break;
		case 155:	 // NPCRUNNINGSPEED
			NPCRunningSpeed( std::stof( value ));
			break;
		case 156:	 // NPCFLEEINGSPEED
			NPCFleeingSpeed( std::stof( value ));
			break;
		case 157:	 // BASICTOOLTIPSONLY
			BasicTooltipsOnly(( static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 ));
			break;
		case 158:	 // GLOBALITEMDECAY
			GlobalItemDecay(( static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 ));
			break;
		case 159:	 // SCRIPTITEMSDECAYABLE
			ScriptItemsDecayable(( static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 ));
			break;
		case 160:	 // BASEITEMSDECAYABLE
			BaseItemsDecayable(( static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 ));
			break;
		case 161:	 // ITEMDECAYINHOUSES
			ItemDecayInHouses(( static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 ));
			break;
		case 162:	 // SPAWNREGIONSFACETS
			SetSpawnRegionsFacetStatus( static_cast<UI32>( std::stoul( value, nullptr, 0 )));
			break;
		case 163:	// PAPERDOLLGUILDBUTTON
			PaperdollGuildButton( static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 );
			break;
		case 164:	// ATTACKSPEEDFROMSTAMINA
			CombatAttackSpeedFromStamina( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 );
			break;
		case 169:	 // DISPLAYDAMAGENUMBERS
			CombatDisplayDamageNumbers( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 );
			break;
		case 170:	 // CLIENTSUPPORT4000
			ClientSupport4000( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 );
			break;
		case 171:	 // CLIENTSUPPORT5000
			ClientSupport5000( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 );
			break;
		case 172:	 // CLIENTSUPPORT6000
			ClientSupport6000( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 );
			break;
		case 173:	 // CLIENTSUPPORT6050
			ClientSupport6050( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 );
			break;
		case 174:	 // CLIENTSUPPORT7000
			ClientSupport7000( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 );
			break;
		case 175:	 // CLIENTSUPPORT7090
			ClientSupport7090( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 );
			break;
		case 176:	 // CLIENTSUPPORT70160
			ClientSupport70160( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 );
			break;
		case 177:	// CLIENTSUPPORT70240
			ClientSupport70240( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 );
			break;
		case 178:	// CLIENTSUPPORT70300
			ClientSupport70300( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 );
			break;
		case 179:	// CLIENTSUPPORT70331
			ClientSupport70331( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 );
			break;
		case 180:	// CLIENTSUPPORT704565
			ClientSupport704565( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 );
			break;
		case 181:	// CLIENTSUPPORT70610
			ClientSupport70610( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 );
			break;
		case 182:	 // EXTENDEDSTARTINGSTATS
			ExtendedStartingStats( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 );
			break;
		case 183:	 // EXTENDEDSTARTINGSKILLS
			ExtendedStartingSkills( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 );
			break;
		case 184:	// WEAPONDAMAGECHANCE
			CombatWeaponDamageChance( static_cast<UI08>( std::stoul( value, nullptr, 0 )));
			break;
		case 185:	// ARMORDAMAGECHANCE
			CombatArmorDamageChance( static_cast<UI08>( std::stoul( value, nullptr, 0 )));
			break;
		case 186:	// WEAPONDAMAGEMIN
			CombatWeaponDamageMin( static_cast<UI08>( std::stoul( value, nullptr, 0 )));
			break;
		case 187:	// WEAPONDAMAGEMAX
			CombatWeaponDamageMax( static_cast<UI08>( std::stoul( value, nullptr, 0 )));
			break;
		case 188:	// ARMORDAMAGEMIN
			CombatArmorDamageMin( static_cast<UI08>( std::stoul( value, nullptr, 0 )));
			break;
		case 189:	// ARMORDAMAGEMAX
			CombatArmorDamageMax( static_cast<UI08>( std::stoul( value, nullptr, 0 )));
			break;
		case 190:	// GLOBALATTACKSPEED
			GlobalAttackSpeed( std::stof( value ));
			break;
		case 191:	// NPCSPELLCASTSPEED
			NPCSpellCastSpeed( std::stof( value ));
			break;
		case 192:	// FISHINGSTAMINALOSS
			FishingStaminaLoss( std::stof( value ));
			break;
		case 193:	// RANDOMSTARTINGLOCATION
			ServerRandomStartingLocation( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 );
			break;
		case 194:	// ASSISTANTNEGOTIATION
			SetAssistantNegotiation(( static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 ));
			break;
		case 195:	// KICKONASSISTANTSILENCE
			KickOnAssistantSilence(( static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 ));
			break;
		case 196:	// AF_FILTERWEATHER
			SetDisabledAssistantFeature( AF_FILTERWEATHER, static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 );
			break;
		case 197:	// AF_FILTERLIGHT
			SetDisabledAssistantFeature( AF_FILTERLIGHT, static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 );
			break;
		case 198:	// AF_SMARTTARGET
			SetDisabledAssistantFeature( AF_SMARTTARGET, static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 );
			break;
		case 199:	// AF_RANGEDTARGET
			SetDisabledAssistantFeature( AF_RANGEDTARGET, static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 );
			break;
		case 200:	// AF_AUTOOPENDOORS
			SetDisabledAssistantFeature( AF_AUTOOPENDOORS, static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 );
			break;
		case 201:	// AF_DEQUIPONCAST
			SetDisabledAssistantFeature( AF_DEQUIPONCAST, static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 );
			break;
		case 202:	// AF_AUTOPOTIONEQUIP
			SetDisabledAssistantFeature( AF_AUTOPOTIONEQUIP, static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 );
			break;
		case 203:	// AF_POISONEDCHECKS
			SetDisabledAssistantFeature( AF_POISONEDCHECKS, static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 );
			break;
		case 204:	// AF_LOOPEDMACROS
			SetDisabledAssistantFeature( AF_LOOPEDMACROS, static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 );
			break;
		case 205:	// AF_USEONCEAGENT
			SetDisabledAssistantFeature( AF_USEONCEAGENT, static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 );
			break;
		case 206:	// AF_RESTOCKAGENT
			SetDisabledAssistantFeature( AF_RESTOCKAGENT, static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 );
			break;
		case 207:	// AF_SELLAGENT
			SetDisabledAssistantFeature( AF_SELLAGENT, static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 );
			break;
		case 208:	// AF_BUYAGENT
			SetDisabledAssistantFeature( AF_BUYAGENT, static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 );
			break;
		case 209:	// AF_POTIONHOTKEYS
			SetDisabledAssistantFeature( AF_POTIONHOTKEYS, static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 );
			break;
		case 210:	// AF_RANDOMTARGETS
			SetDisabledAssistantFeature( AF_RANDOMTARGETS, static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 );
			break;
		case 211:	// AF_CLOSESTTARGETS
			SetDisabledAssistantFeature( AF_CLOSESTTARGETS, static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 );
			break;
		case 212:	// AF_OVERHEADHEALTH
			SetDisabledAssistantFeature( AF_OVERHEADHEALTH, static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 );
			break;
		case 213:	// AF_AUTOLOOTAGENT
			SetDisabledAssistantFeature( AF_AUTOLOOTAGENT, static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 );
			break;
		case 214:	// AF_BONECUTTERAGENT
			SetDisabledAssistantFeature( AF_BONECUTTERAGENT, static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 );
			break;
		case 215:	// AF_JSCRIPTMACROS
			SetDisabledAssistantFeature( AF_JSCRIPTMACROS, static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 );
			break;
		case 216:	// AF_AUTOREMOUNT
			SetDisabledAssistantFeature( AF_AUTOREMOUNT, static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 );
			break;
		case 218:	// CLASSICUOMAPTRACKER
			SetClassicUOMapTracker(( static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 ));
			break;
		case 219:	// DECAYTIMERINHOUSE
			SystemTimer( tSERVER_DECAYINHOUSE, static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 220:	// PROTECTPRIVATEHOUSES
			ProtectPrivateHouses( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 );
			break;
		case 221:	// TRACKHOUSESPERACCOUNT
			TrackHousesPerAccount( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 );
			break;
		case 222:	// MAXHOUSESOWNABLE
			MaxHousesOwnable( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 223:	// MAXHOUSESCOOWNABLE
			MaxHousesCoOwnable( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 224:	// CANOWNANDCOOWNHOUSES
			CanOwnAndCoOwnHouses( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 );
			break;
		case 225:	// COOWNHOUSESONSAMEACCOUNT
			CoOwnHousesOnSameAccount( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 );
			break;
		case 226:	// ITEMSDETECTSPEECH
			ItemsDetectSpeech( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 );
			break;
		case 227:	// MAXPLAYERPACKITEMS
			MaxPlayerPackItems( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 228:	// MAXPLAYERBANKITEMS
			MaxPlayerBankItems( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 229:	// FORCENEWANIMATIONPACKET
			ForceNewAnimationPacket( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 );
			break;
		case 230:	// MAPDIFFSENABLED
			MapDiffsEnabled( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 );
			break;
		case 231:	// CORESHARDERA
			ExpansionCoreShardEra( EraStringToEnum( oldstrutil::trim( value )));
			break;
		case 232:	// ARMORCALCULATION
			ExpansionArmorCalculation( EraStringToEnum( oldstrutil::trim( value )));
			break;
		case 233:	// STRENGTHDAMAGEBONUS
			ExpansionStrengthDamageBonus( EraStringToEnum( oldstrutil::trim( value )));
			break;
		case 234:	// TACTICSDAMAGEBONUS
			ExpansionTacticsDamageBonus( EraStringToEnum( oldstrutil::trim( value )));
			break;
		case 235:	// ANATOMYDAMAGEBONUS
			ExpansionAnatomyDamageBonus( EraStringToEnum( oldstrutil::trim( value )));
			break;
		case 236:	// LUMBERJACKDAMAGEBONUS
			ExpansionLumberjackDamageBonus( EraStringToEnum( oldstrutil::trim( value )));
			break;
		case 237:	// RACIALDAMAGEBONUS
			ExpansionRacialDamageBonus( EraStringToEnum( oldstrutil::trim( value )));
			break;
		case 238:	// DAMAGEBONUSCAP
			ExpansionDamageBonusCap( EraStringToEnum( oldstrutil::trim( value )));
			break;
		case 239:	// SHIELDPARRY
			ExpansionShieldParry( EraStringToEnum( oldstrutil::trim( value )));
			break;
		case 240:	// PARRYDAMAGECHANCE
			CombatParryDamageChance( static_cast<UI08>( std::stoul( value, nullptr, 0 )));
			break;
		case 241:	// PARRYDAMAGEMIN
			CombatParryDamageMin( static_cast<UI08>( std::stoul( value, nullptr, 0 )));
			break;
		case 242:	// PARRYDAMAGEMAX
			CombatParryDamageMax( static_cast<UI08>( std::stoul( value, nullptr, 0 )));
			break;
		case 243:	// ARMORCLASSDAMAGEBONUS
			CombatArmorClassDamageBonus(  static_cast<SI08>( std::stoi( value, nullptr, 0 )) == 1 );
			break;
		case 244:	// FREESHARDSERVERPOLL
			FreeshardServerPoll(( static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 ));
			break;
		case 245:	// ALCHEMYBONUSENABLED
			AlchemyDamageBonusEnabled( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 );
			break;
		case 246:	// ALCHEMYBONUSMODIFIER
			AlchemyDamageBonusModifier( static_cast<UI08>( std::stoul( value, nullptr, 0 )));
			break;
		case 247:	 // NPCFLAGUPDATETIMER
			SystemTimer( tSERVER_NPCFLAGUPDATETIMER, static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 248:	 // JSENGINESIZE
			SetJSEngineSize( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 249:	 // USEUNICODEMESSAGES
			UseUnicodeMessages( static_cast<UI16>( std::stoul( value, nullptr, 0 )) == 1 );
			break;
		case 250:	 // SCRIPTDATADIRECTORY
		{
			Directory( CSDDP_SCRIPTDATA, value );
			break;
		}
		case 251:    // THIRSTRATE
			SystemTimer( tSERVER_THIRSTRATE, static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 252:    // THIRSTDRAINVAL
			ThirstDrain( static_cast<SI16>( std::stoi( value, nullptr, 0 )));
			break;
		case 253:    // PETTHIRSTOFFLINE
			PetThirstOffline(( static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 ));
			break;
		case 254:	// ExternalIP
			ExternalIP( value );
			break;
		case 255:	 // BLOODDECAYTIMER
			SystemTimer( tSERVER_BLOODDECAY, static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 256:	 // BLOODDECAYCORPSETIMER
			SystemTimer( tSERVER_BLOODDECAYCORPSE, static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 257:	// BLOODEFFECTCHANCE
			CombatBloodEffectChance( static_cast<UI08>( std::stoul( value, nullptr, 0 )));
			break;
		case 258:	 // NPCCORPSEDECAYTIMER
			SystemTimer( tSERVER_NPCCORPSEDECAY, static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 259:    // HUNGERENABLED
			HungerSystemEnabled(( static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 ));
			break;
		case 260:    // THIRSTENABLED
			ThirstSystemEnabled(( static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 ));
			break;
		case 261:    // TRAVELSPELLSFROMBOATKEYS
			TravelSpellsFromBoatKeys(( static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 ));
			break;
		case 262:    // TRAVELSPELLSWHILEOVERWEIGHT
			TravelSpellsWhileOverweight(( static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 ));
			break;
		case 263:    // MARKRUNESINMULTIS
			MarkRunesInMultis(( static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 ));
			break;
		case 264:    // TRAVELSPELLSBETWEENWORLD
			TravelSpellsBetweenWorlds(( static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 ));
			break;
		case 265:    // TRAVELSPELLSWHILEAGGRESSOR
			TravelSpellsWhileAggressor(( static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 ));
			break;
		case 266:	// BUYBANKTHRESHOLD
			BuyThreshold( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 267:	// NETWORKLOG
			ServerNetworkLog(( static_cast<UI16>( std::stoul( value, nullptr, 0 )) >= 1 ? true : false ));
			break;
		case 268:	// SPEECHLOG
			ServerSpeechLog(( static_cast<UI16>( std::stoul( value, nullptr, 0 )) >= 1 ? true : false ));
			break;
		case 269:	 // NPCMOUNTEDWALKINGSPEED
			NPCMountedWalkingSpeed( std::stof( value ));
			break;
		case 270:	 // NPCMOUNTEDRUNNINGSPEED
			NPCMountedRunningSpeed( std::stof( value ));
			break;
		case 271:	 // NPCMOUNTEDFLEEINGSPEED
			NPCMountedFleeingSpeed( std::stof( value ));
			break;
		case 272:	// CONTEXTMENUS
			ServerContextMenus(( static_cast<UI16>( std::stoul( value, nullptr, 0 )) >= 1 ? true : false ));
			break;
		case 273:	// SERVERLANGUAGE
			ServerLanguage( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 274:	// CHECKPETCONTROLDIFFICULTY
			CheckPetControlDifficulty(( static_cast<UI16>( std::stoul( value, nullptr, 0 )) >= 1 ? true : false ));
			break;
		case 275:	// PETLOYALTYGAINONSUCCESS
			SetPetLoyaltyGainOnSuccess( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 276:	// PETLOYALTYLOSSONFAILURE
			SetPetLoyaltyLossOnFailure( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 277:    // LOYALTYRATE
			SystemTimer( tSERVER_LOYALTYRATE, static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 278:	// SHOWNPCTITLESINTOOLTIPS
			ShowNpcTitlesInTooltips(( static_cast<UI16>( std::stoul( value, nullptr, 0 )) >= 1 ? true : false ));
			break;
		case 279:	 // FISHPERAREA
			ResFish( static_cast<SI16>( std::stoi( value, nullptr, 0 )));
			break;
		case 280:	 // FISHRESPAWNTIMER
			ResFishTime( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 281:	 // ARCHERYHITBONUS
			CombatArcheryHitBonus( static_cast<SI16>( std::stoi( value, nullptr, 0 )));
			break;
		case 282:    // ITEMSINTERRUPTCASTING
			ItemsInterruptCasting(( static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 ));
			break;
		case 283:	 // SYSMESSAGECOLOUR
			SysMsgColour( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 284:	// AF_AUTOBANDAGE
			SetDisabledAssistantFeature( AF_AUTOBANDAGE, static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 );
			break;
		case 285:	// AF_ENEMYTARGETSHARE
			SetDisabledAssistantFeature( AF_ENEMYTARGETSHARE, static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 );
			break;
		case 286:	// AF_FILTERSEASON
			SetDisabledAssistantFeature( AF_FILTERSEASON, static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 );
			break;
		case 287:	// AF_SPELLTARGETSHARE
			SetDisabledAssistantFeature( AF_SPELLTARGETSHARE, static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 );
			break;
		case 288:	// AF_HUMANOIDHEALTHCHECKS
			SetDisabledAssistantFeature( AF_HUMANOIDHEALTHCHECKS, static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 );
			break;
		case 289:	// AF_SPEECHJOURNALCHECKS
			SetDisabledAssistantFeature( AF_SPEECHJOURNALCHECKS, static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 );
			break;
		case 290:	// ARCHERYSHOOTDELAY
			CombatArcheryShootDelay( std::stof( value ));
			break;
		case 291:	 // MAXCLIENTBYTESIN
			MaxClientBytesIn( static_cast<UI32>( std::stoul( value, nullptr, 0 )));
			break;
		case 292:	 // MAXCLIENTBYTESOUT
			MaxClientBytesOut( static_cast<UI32>( std::stoul( value, nullptr, 0 )));
			break;
		case 293:	 // NETTRAFFICTIMEBAN
			NetTrafficTimeban( static_cast<UI32>( std::stoul( value, nullptr, 0 )));
			break;
		case 294:    // TOOLUSELIMIT
			ToolUseLimit(( static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 ));
			break;
		case 295:    // TOOLUSEBREAK
			ToolUseBreak(( static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 ));
			break;
		case 296:    // ITEMREPAIRDURABILITYLOSS
			ItemRepairDurabilityLoss(( static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 ));
			break;
		case 297:    // HIDESTATSFORUNKNOWNMAGICITEMS
			HideStatsForUnknownMagicItems(( static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 ));
			break;
		case 298:    // CRAFTCOLOUREDWEAPONS
			CraftColouredWeapons(( static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 ));
			break;
		case 299:	// MAXSAFETELEPORTSPERDAY
			MaxSafeTeleportsPerDay( static_cast<UI08>( std::stoul( value, nullptr, 0 )));
			break;
		case 300:    // TELEPORTONEARESTSAFELOCATION
			TeleportToNearestSafeLocation(( static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 ));
			break;
		case 301:    // ALLOWAWAKENPCS
			AllowAwakeNPCs(( static_cast<SI16>( std::stoi( value, nullptr, 0 )) == 1 ));
			break;
		case 302:	 // DISPLAYMAKERSMARK
			DisplayMakersMark( static_cast<UI16>( std::stoul( value, nullptr, 0 )) != 0 );
			break;
		case 303:	// SHOWNPCTITLESOVERHEAD
			ShowNpcTitlesOverhead(( static_cast<UI16>( std::stoul( value, nullptr, 0 )) >= 1 ? true : false ));
			break;
		case 304:	// SHOWINVULNERABLETAGOVERHEAD
			ShowInvulnerableTagOverhead(( static_cast<UI16>( std::stoul( value, nullptr, 0 )) >= 1 ? true : false ));
			break;
		case 305:	// PETCOMBATTRAINING
			PetCombatTraining(( static_cast<UI16>( std::stoul( value, nullptr, 0 )) >= 1 ? true : false ));
			break;
		case 306:	// HIRELINGCOMBATTRAINING
			HirelingCombatTraining(( static_cast<UI16>( std::stoul( value, nullptr, 0 )) >= 1 ? true : false ));
			break;
		case 307:	// NPCCOMBATTRAINING
			NpcCombatTraining(( static_cast<UI16>( std::stoul( value, nullptr, 0 )) >= 1 ? true : false ));
			break;
		case 308:	 // GLOBALRESTOCKMULTIPLIER
			GlobalRestockMultiplier( std::stof( value ));
			break;
		case 309:	// SHOWITEMRESISTSTATS
			ShowItemResistStats(( static_cast<UI16>( std::stoul( value, nullptr, 0 )) >= 1 ? true : false ));
			break;
		case 310:	// SHOWWEAPONDAMAGETYPES
			ShowWeaponDamageTypes(( static_cast<UI16>( std::stoul( value, nullptr, 0 )) >= 1 ? true : false ));
			break;
		case 311:	// SHOWRACEWITHNAME
			ShowRaceWithName(( static_cast<UI16>( std::stoul( value, nullptr, 0 )) >= 1 ? true : false ));
			break;
		case 312:	// SHOWRACEINPAPERDOLL
			ShowRaceInPaperdoll(( static_cast<UI16>( std::stoul( value, nullptr, 0 )) >= 1 ? true : false ));
			break;
		case 313:	// WEAPONPARRY
			ExpansionWeaponParry( EraStringToEnum( oldstrutil::trim( value )));
			break;
		case 314:	// WRESTLINGPARRY
			ExpansionWrestlingParry( EraStringToEnum( oldstrutil::trim( value )));
			break;
		case 315:	// COMBATHITCHANCE
			ExpansionCombatHitChance( EraStringToEnum( oldstrutil::trim( value )));
			break;
		case 316:	// CASTSPELLSWHILEMOVING
			CastSpellsWhileMoving(( static_cast<UI16>( std::stoul( value, nullptr, 0 )) >= 1 ? true : false ));
			break;
		case 317:	// SHOWREPUTATIONTITLEINTOOLTIP
			ShowReputationTitleInTooltip(( static_cast<UI16>( std::stoul( value, nullptr, 0 )) >= 1 ? true : false ));
			break;
		case 318:	// SHOWGUILDINFOINTOOLTIP
			ShowGuildInfoInTooltip(( static_cast<UI16>( std::stoul( value, nullptr, 0 )) >= 1 ? true : false ));
			break;
		case 319:	// MAXPLAYERPACKWEIGHT
			MaxPlayerPackWeight( static_cast<SI32>( std::stoi( value, nullptr, 0 )));
			break;
		case 320:	// MAXPLAYERBANKWEIGHT
			MaxPlayerBankWeight( static_cast<SI32>( std::stoi( value, nullptr, 0 )));
			break;
		case 321:	// SAFECOOWNERLOGOUT
			SafeCoOwnerLogout(( static_cast<UI16>( std::stoul( value, nullptr, 0 )) >= 1 ? true : false ));
			break;
		case 322:	// SAFEFRIENDLOGOUT
			SafeFriendLogout(( static_cast<UI16>( std::stoul( value, nullptr, 0 )) >= 1 ? true : false ));
			break;
		case 323:	// SAFEGUESTLOGOUT
			SafeGuestLogout(( static_cast<UI16>( std::stoul( value, nullptr, 0 )) >= 1 ? true : false ));
			break;
		case 324:	// KEYLESSOWNERACCESS
			KeylessOwnerAccess(( static_cast<UI16>( std::stoul( value, nullptr, 0 )) >= 1 ? true : false ));
			break;
		case 325:	// KEYLESSCOOWNERACCESS
			KeylessCoOwnerAccess(( static_cast<UI16>( std::stoul( value, nullptr, 0 )) >= 1 ? true : false ));
			break;
		case 326:	// KEYLESSFRIENDACCESS
			KeylessFriendAccess(( static_cast<UI16>( std::stoul( value, nullptr, 0 )) >= 1 ? true : false ));
			break;
		case 327:	// KEYLESSGUESTACCESS
			KeylessGuestAccess( ( static_cast<UI16>( std::stoul( value, nullptr, 0 ) ) >= 1 ? true : false ) );
			break;
		case 328:	// WEAPONDAMAGEBONUSTYPE
			WeaponDamageBonusType( static_cast<UI08>( std::stoul( value, nullptr, 0 )));
			break;
		case 329:	// OFFERBODSFROMITEMSALES
			OfferBODsFromItemSales(( static_cast<UI16>( std::stoul( value, nullptr, 0 )) >= 1 ? true : false ));
			break;
		case 330:	// OFFERBODSFROMCONTEXTMENU
			OfferBODsFromContextMenu(( static_cast<UI16>( std::stoul( value, nullptr, 0 )) >= 1 ? true : false ));
			break;
		case 331:	// BODSFROMCRAFTEDITEMSONLY
			BODsFromCraftedItemsOnly(( static_cast<UI16>( std::stoul( value, nullptr, 0 )) >= 1 ? true : false ));
			break;
		case 332:	 // BODGOLDREWARDMULTIPLIER
			BODGoldRewardMultiplier( std::stof( value ));
			break;
		case 333:	 // BODFAMEREWARDMULTIPLIER
			BODFameRewardMultiplier( std::stof( value ));
			break;
		case 334:	// ENABLENPCGUILDDISCOUNTS
			EnableNPCGuildDiscounts(( static_cast<UI16>( std::stoul( value, nullptr, 0 )) >= 1 ? true : false ));
			break;
		case 335:	// ENABLENPCGUILDPREMIUMS
			EnableNPCGuildPremiums(( static_cast<UI16>( std::stoul( value, nullptr, 0 )) >= 1 ? true : false ));
			break;
		case 336:	 // AGGRESSORFLAGTIMER
			SystemTimer( tSERVER_AGGRESSORFLAG, static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 337:	 // PERMAGREYFLAGTIMER
			SystemTimer( tSERVER_PERMAGREYFLAG, static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 338:	 // STEALINGFLAGTIMER
			SystemTimer( tSERVER_STEALINGFLAG, static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 339:	 // SNOOPAWARENESS
			SnoopAwareness( static_cast<UI16>( std::stoul( value, nullptr, 0 )) != 0 );
			break;
		case 340:	 // APSPERFTHRESHOLD
			APSPerfThreshold( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 341:	 // APSINTERVAL
			APSInterval( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 342:	 // APSDELAYSTEP
			APSDelayStep( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 343:	 // APSDELAYMAXCAP
			APSDelayMaxCap( static_cast<UI16>( std::stoul( value, nullptr, 0 )));
			break;
		case 344:	 // YOUNGPLAYERSYSTEM
			YoungPlayerSystem( static_cast<UI16>( std::stoul( value, nullptr, 0 )) != 0 );
			break;
		case 345:	 // YOUNGLOCATION
			YoungServerLocation( value );
			break;
		case 346:	 // SECRETSHARDKEY
			SecretShardKey( value );
			break;
		case 347:	 // MOONGATEFACETS
			SetMoongateFacetStatus( static_cast<UI32>( std::stoul( value, nullptr, 0 )));
			break;
		case 348:    // AUTOUNEQUIPPEDCASTING
			AutoUnequippedCasting(( static_cast<UI16>( std::stoul( value, nullptr, 0 )) >= 1 ? true : false ));
			break;
		case 349:	 // LOOTDECAYSWITHNPCCORPSE
			NpcCorpseLootDecay( static_cast<UI16>( std::stoul( value, nullptr, 0 )) != 0 );
			break;
		case 355:	// PHYSICALRESISTCAP
			PhysicalResistCap( std::stof( value ));
			break;
		case 356:	// FIRERESISTCAP
			FireResistCap( std::stof( value ));
			break;
		case 357:	// COLDRESISTCAP
			ColdResistCap( std::stof( value ));
			break;
		case 358:	// POISONRESISTCAP
			PoisonResistCap( std::stof( value ));
			break;
		case 359:	// ENERGYRESISTCAP
			EnergyResistCap( std::stof( value ));
			break;
		case 360:	// DEFENSECHANCEINCREASECAP
			DefenseChanceIncreaseCap( std::stof( value ));
			break;
		default:
			rValue = false;
			break;
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ServerStartGold()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default amount of starting gold for new characters
//o------------------------------------------------------------------------------------------------o
auto CServerData::ServerStartGold() const -> SI16
{
	return startGold;
}
auto CServerData::ServerStartGold( SI16 value ) -> void
{
	if( value >= 0 )
	{
		startGold = value;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ServerStartPrivs()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default starting privs for new characters
//o------------------------------------------------------------------------------------------------o
auto CServerData::ServerStartPrivs() const -> UI16
{
	return startPrivs;
}
auto CServerData::ServerStartPrivs( UI16 value ) -> void
{
	startPrivs = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ServerMoon()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the phase of one of the game's two moons
//o------------------------------------------------------------------------------------------------o
auto CServerData::ServerMoon( SI16 slot ) const -> SI16
{
	SI16 rValue = -1;
	if( slot >= 0 && slot <= 1 )
	{
		rValue = moon[slot];
	}
	return rValue;
}
auto CServerData::ServerMoon( SI16 slot, SI16 value ) -> void
{
	if( slot >= 0 && slot <= 1 && value >= 0 )
	{
		moon[slot] = value;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::DungeonLightLevel()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default, global light level for dungeons
//o------------------------------------------------------------------------------------------------o
auto CServerData::DungeonLightLevel() const -> LIGHTLEVEL
{
	return dungeonLightLevel;
}
auto CServerData::DungeonLightLevel( LIGHTLEVEL value ) -> void
{
	dungeonLightLevel = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::WorldLightCurrentLevel()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default, global current light level outside of dungeons
//o------------------------------------------------------------------------------------------------o
auto CServerData::WorldLightCurrentLevel() const -> LIGHTLEVEL
{
	return currentLightLevel;
}
auto CServerData::WorldLightCurrentLevel( LIGHTLEVEL value ) -> void
{
	currentLightLevel = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::WorldLightBrightLevel()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default, global light level for the brightest time of day
//o------------------------------------------------------------------------------------------------o
auto CServerData::WorldLightBrightLevel() const -> LIGHTLEVEL
{
	return brightnessLightLevel;
}
auto CServerData::WorldLightBrightLevel( LIGHTLEVEL value ) -> void
{
	brightnessLightLevel = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::WorldLightDarkLevel()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default, global light level for the darkest time of day
//o------------------------------------------------------------------------------------------------o
auto CServerData::WorldLightDarkLevel() const -> LIGHTLEVEL
{
	return darknessLightLevel;
}
auto CServerData::WorldLightDarkLevel( LIGHTLEVEL value ) -> void
{
	darknessLightLevel=value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::PostLoadDefaults()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	If no start locations have been provided in ini, use hardcoded defaults
//o------------------------------------------------------------------------------------------------o
auto CServerData::PostLoadDefaults() -> void
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
	
	// Also load young player start locations, which default to same as normal players
	if( YoungPlayerSystem() && youngStartlocations.empty() )
	{
		YoungServerLocation( "Yew,Center,545,982,0,0,0,1075072" );
		YoungServerLocation( "Minoc,Tavern,2477,411,15,0,0,1075073" );
		YoungServerLocation( "Britain,Sweet Dreams Inn,1495,1629,10,0,0,1075074" );
		YoungServerLocation( "Moonglow,Docks,4406,1045,0,0,0,1075075" );
		YoungServerLocation( "Trinsic,West Gate,1832,2779,0,0,0,1075076" );
		YoungServerLocation( "Magincia,Docks,3675,2259,26,0,0,1075077" );
		YoungServerLocation( "Jhelom,Docks,1492,3696,0,0,0,1075078" );
		YoungServerLocation( "Skara Brae,Docks,639,2236,0,0,0,1075079" );
		YoungServerLocation( "Vesper,Ironwood Inn,2771,977,0,0,0,1075080" );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ServerLocation()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets server start locations
//o------------------------------------------------------------------------------------------------o
auto CServerData::ServerLocation( size_t locNum ) -> __STARTLOCATIONDATA__*
{
	__STARTLOCATIONDATA__ *rValue = nullptr;
	if( locNum < startlocations.size() )
	{
		rValue = &startlocations[locNum];
	}
	return rValue;
}

auto CServerData::ServerLocation( std::string toSet ) -> void
{
	auto temp = toSet;
	temp = oldstrutil::trim( oldstrutil::removeTrailing( temp, "//" ));
	auto csecs = oldstrutil::sections( temp, "," );
	
	__STARTLOCATIONDATA__ toAdd;
	if( csecs.size() >= 7 )
	{
		toAdd.oldTown 			= oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" ));
		toAdd.oldDescription 	= oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" ));
		toAdd.newTown 			= toAdd.oldTown;
		toAdd.newDescription 	= toAdd.oldDescription;
		
		toAdd.x 		= static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( csecs[2], "//" )), nullptr, 0 ));
		toAdd.y 		= static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( csecs[3], "//" )), nullptr, 0 ));
		toAdd.z 		= static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( csecs[4], "//" )), nullptr, 0 ));
		toAdd.worldNum 	= static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( csecs[5], "//" )), nullptr, 0 ));
		if( csecs.size() == 7 )
		{
			toAdd.clilocDesc	= static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[6], "//" )), nullptr, 0 ));
		}
		else
		{
			toAdd.instanceId	= static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( csecs[6], "//" )), nullptr, 0 ));
			toAdd.clilocDesc	= static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[7], "//" )), nullptr, 0 ));
		}
		startlocations.push_back( toAdd );
	}
	else
	{
		Console.Error( "Malformed location entry in ini file" );
	}
}

auto CServerData::NumServerLocations() const -> size_t
{
	return startlocations.size();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::YoungServerLocation()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets server start location(s) for Young players
//o------------------------------------------------------------------------------------------------o
auto CServerData::YoungServerLocation( size_t locNum ) -> __STARTLOCATIONDATA__*
{
	__STARTLOCATIONDATA__ *rValue = nullptr;
	if( locNum < youngStartlocations.size() )
	{
		rValue = &youngStartlocations[locNum];
	}
	return rValue;
}

auto CServerData::YoungServerLocation( std::string toSet ) -> void
{
	auto temp = toSet;
	temp = oldstrutil::trim( oldstrutil::removeTrailing( temp, "//" ));
	auto csecs = oldstrutil::sections( temp, "," );

	__STARTLOCATIONDATA__ toAdd;
	if( csecs.size() >= 7 )
	{
		toAdd.oldTown 			= oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" ));
		toAdd.oldDescription 	= oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" ));
		toAdd.newTown 			= toAdd.oldTown;
		toAdd.newDescription 	= toAdd.oldDescription;

		toAdd.x 		= static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( csecs[2], "//" )), nullptr, 0 ));
		toAdd.y 		= static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( csecs[3], "//" )), nullptr, 0 ));
		toAdd.z 		= static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( csecs[4], "//" )), nullptr, 0 ));
		toAdd.worldNum 	= static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( csecs[5], "//" )), nullptr, 0 ));
		if( csecs.size() == 7 )
		{
			toAdd.clilocDesc	= static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[6], "//" )), nullptr, 0 ));
		}
		else
		{
			toAdd.instanceId	= static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( csecs[6], "//" )), nullptr, 0 ));
			toAdd.clilocDesc	= static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[7], "//" )), nullptr, 0 ));
		}
		youngStartlocations.push_back( toAdd );
	}
	else
	{
		Console.Error( "Malformed young start location entry in ini file" );
	}
}

auto CServerData::NumYoungServerLocations() const -> size_t
{
	return youngStartlocations.size();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ServerSecondsPerUOMinute()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the number of real life seconds per UO minute
//o------------------------------------------------------------------------------------------------o
auto CServerData::ServerSecondsPerUOMinute() const -> UI16
{
	return secondsPerUoMinute;
}
auto CServerData::ServerSecondsPerUOMinute( UI16 newVal ) -> void
{
	secondsPerUoMinute = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::ServerLanguage()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default (if any) language used for the server
//o------------------------------------------------------------------------------------------------o
auto CServerData::ServerLanguage() const -> UI16
{
	return serverLanguage;
}
auto CServerData::ServerLanguage( UI16 newVal ) -> void
{
	if( newVal < DL_COUNT )
	{
		serverLanguage = newVal;
	}
	else
	{
		serverLanguage = DL_DEFAULT;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::MaxClientBytesIn()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets max incoming bytes received from a socket in each 10 second period
//|					If amount exceeds this cap, client might receive a warning/get kicked
//o------------------------------------------------------------------------------------------------o
auto CServerData::MaxClientBytesIn() const -> UI32
{
	return maxBytesIn;
}
auto CServerData::MaxClientBytesIn( UI32 newVal ) -> void
{
	maxBytesIn = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::MaxClientBytesOut()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets max outgoing bytes sent to a socket in each 10 second period
//|					If amount exceeds this cap, client might receive a warning/get kicked
//o------------------------------------------------------------------------------------------------o
auto CServerData::MaxClientBytesOut() const -> UI32
{
	return maxBytesOut;
}
auto CServerData::MaxClientBytesOut( UI32 newVal ) -> void
{
	maxBytesOut = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::NetTrafficTimeban()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets max amount of time a player will be temporarily banned for if they
//|					exceed their alotted network traffic budget
//o------------------------------------------------------------------------------------------------o
auto CServerData::NetTrafficTimeban() const -> UI32
{
	return trafficTimeban;
}
auto CServerData::NetTrafficTimeban( UI32 newVal ) -> void
{
	trafficTimeban = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::GetJSEngineSize()
//|					CServerData::SetJSEngineSize()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets jsEngineSize (in MB), used to define max bytes per JSRuntime
//|					before a last ditch GC effort is made
//o------------------------------------------------------------------------------------------------o
auto CServerData::GetJSEngineSize() const -> UI16
{
	return jsEngineSize;
}
auto CServerData::SetJSEngineSize( UI16 newVal ) -> void
{
	jsEngineSize = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::APSPerfThreshold()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the performance threshold (simulation cycles) below which the
//|					APS - Adaptive Performance System - kicks in. A value of 0 disables the system
//o------------------------------------------------------------------------------------------------o
auto CServerData::APSPerfThreshold() const -> UI16
{
	return apsPerfThreshold;
}
auto CServerData::APSPerfThreshold( UI16 newVal ) -> void
{
	apsPerfThreshold = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::APSInterval()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the interval at which APS will check shard performance and make
//|					adjustments if necessary
//o------------------------------------------------------------------------------------------------o
auto CServerData::APSInterval() const -> UI16
{
	return apsInterval;
}
auto CServerData::APSInterval( UI16 newVal ) -> void
{
	apsInterval = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::APSDelayStep()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the step value - in milliseconds - that is used by APS to
//|					gradually increase/decrease the delay effect for checking NPC AI/movement stuff
//o------------------------------------------------------------------------------------------------o
auto CServerData::APSDelayStep() const -> UI16
{
	return apsDelayStep;
}
auto CServerData::APSDelayStep( UI16 newVal ) -> void
{
	apsDelayStep = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::APSDelayMaxCap()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max cap - in milliseconds - for how much NPC AI/movement stuff can
//|					be delayed by APS in an effort to restore shard performance to above threshold levels
//o------------------------------------------------------------------------------------------------o
auto CServerData::APSDelayMaxCap() const -> UI16
{
	return apsDelayMaxCap;
}
auto CServerData::APSDelayMaxCap( UI16 newVal ) -> void
{
	apsDelayMaxCap = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::SaveTime()
//|	Date		-	January 28th, 2007
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Outputs server time information to time.wsc in the /shared/ directory
//o------------------------------------------------------------------------------------------------o
auto CServerData::SaveTime() -> void
{
	std::string		timeFile = cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "time.wsc";
	std::ofstream	timeDestination( timeFile.c_str() );
	if( !timeDestination )
	{
		Console.Error( oldstrutil::format( "Failed to open %s for writing", timeFile.c_str() ));
		return;
	}

	timeDestination << "[TIME]" << '\n' << "{" << '\n';
	timeDestination << "CURRENTLIGHT=" << static_cast<UI16>( WorldLightCurrentLevel() ) << '\n';
	timeDestination << "DAY=" << ServerTimeDay() << '\n';
	timeDestination << "HOUR=" << static_cast<UI16>( ServerTimeHours() ) << '\n';
	timeDestination << "MINUTE=" << static_cast<UI16>( ServerTimeMinutes() ) << '\n';
	timeDestination << "AMPM=" << ( ServerTimeAMPM() ? 1 : 0 ) << '\n';
	timeDestination << "MOON=" << ServerMoon( 0 ) << "," << ServerMoon( 1 ) << '\n';
	timeDestination << "}" << '\n' << '\n';

	timeDestination.close();
}

auto ReadWorldTagData( std::istream &inStream, std::string &tag, std::string &data ) -> void;
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerData::LoadTime()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads server time information from time.wsc in the /shared/ directory
//o------------------------------------------------------------------------------------------------o
auto CServerData::LoadTime() -> void
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
			input.getline( line, 1023 );
			line[input.gcount()] = 0;
			std::string sLine( line );
			sLine = oldstrutil::trim( oldstrutil::removeTrailing( sLine, "//" ));
			if( !sLine.empty() )
			{
				if( oldstrutil::upper( sLine ) == "[TIME]" )
				{
					LoadTimeTags( input );
				}
			}
		}
		input.close();
	}
}

//==============================================================================================
auto CServerData::LoadTimeTags( std::istream &input ) -> void
{
	std::string UTag, tag, data;
	while( tag != "o---o" )
	{
		ReadWorldTagData( input, tag, data );
		if( tag != "o---o" )
		{
			UTag = oldstrutil::upper( tag );
			
			if( UTag == "AMPM" )
			{
				ServerTimeAMPM(( std::stoi( data, nullptr, 0 ) == 1 ));
			}
			else if( UTag == "CURRENTLIGHT" )
			{
				WorldLightCurrentLevel( static_cast<UI16>( std::stoul( data, nullptr, 0 )));
			}
			else if( UTag == "DAY" )
			{
				ServerTimeDay( static_cast<SI16>( std::stoi( data, nullptr, 0 )));
			}
			else if( UTag == "HOUR" )
			{
				ServerTimeHours( static_cast<UI16>( std::stoul( data, nullptr, 0 )));
			}
			else if( UTag == "MINUTE" )
			{
				ServerTimeMinutes( static_cast<UI16>( std::stoul( data, nullptr, 0 )));
			}
			else if( UTag == "MOON" )
			{
				auto csecs = oldstrutil::sections( data, "," );
				if( csecs.size() > 1 )
				{
					ServerMoon( 0, static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 )));
					ServerMoon( 1, static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 )));
				}
			}
		}
	}
	tag = "";
}

//==============================================================================================
auto CServerData::ServerTimeDay() const -> SI16
{
	return days;
}
//==============================================================================================
auto CServerData::ServerTimeDay( SI16 nValue ) -> void
{
	days = nValue;
}
//==============================================================================================
auto CServerData::ServerTimeHours() const -> UI08
{
	return hours;
}
//==============================================================================================
auto CServerData::ServerTimeHours( UI08 nValue ) -> void
{
	hours = nValue;
}
//==============================================================================================
auto CServerData::ServerTimeMinutes() const -> UI08
{
	return minutes;
}

//==============================================================================================
auto CServerData::ServerTimeMinutes( UI08 nValue ) -> void
{
	minutes = nValue;
}
//==============================================================================================
auto CServerData::ServerTimeSeconds() const -> UI08
{
	return seconds;
}
//==============================================================================================
auto CServerData::ServerTimeSeconds( UI08 nValue) -> void
{
	seconds = nValue;
}
//==============================================================================================
auto CServerData::ServerTimeAMPM() const -> bool
{
	return ampm;
}
//==============================================================================================
auto CServerData::ServerTimeAMPM( bool nValue ) -> void
{
	ampm = nValue;
}

//==============================================================================================
auto CServerData::IncSecond() -> bool
{
	bool rValue = false;
	++seconds;
	if( seconds == 60 )
	{
		seconds = 0;
		rValue	= IncMinute();
	}
	return rValue;
}

//==============================================================================================
auto CServerData::IncMoon( SI32 mNumber ) -> void
{
	moon[mNumber] = static_cast<SI16>(( moon[mNumber] + 1 ) % 8 );
}

//==============================================================================================
auto CServerData::IncMinute() -> bool
{
	bool rValue = false;
	++minutes;
	if( minutes%8 == 0 )
	{
		IncMoon( 0 );
	}
	if( minutes%3 == 0 )
	{
		IncMoon( 1 );
	}

	if( minutes == 60 )
	{
		minutes = 0;
		rValue	= IncHour();
	}
	return rValue;
}

//==============================================================================================
auto CServerData::IncHour() -> bool
{
	++hours;
	bool retVal = false;
	if( hours == 12 )
	{
		if( ampm )
		{
			retVal = IncDay();
		}
		hours	= 0;
		ampm	= !ampm;
	}
	return retVal;
}

//==============================================================================================
auto CServerData::IncDay() -> bool
{
	++days;
	return true;
}


//==============================================================================================
auto CServerData::ServerEntry( UI16 entryNum ) -> PhysicalServer *
{
	PhysicalServer *rValue = nullptr;
	if( entryNum < serverList.size() )
	{
		rValue = &serverList[entryNum];
	}
	return rValue;
}
//==============================================================================================
auto CServerData::ServerCount() const -> UI16
{
	return static_cast<UI16>( serverList.size() );
}
//==============================================================================================
// PhysicalServer
//==============================================================================================

//==============================================================================================
auto PhysicalServer::SetName( const std::string& newName ) -> void
{
	name = newName;
}
//==============================================================================================
auto PhysicalServer::SetDomain( const std::string& newDomain ) -> void
{
	domain = newDomain;
}
//==============================================================================================
auto PhysicalServer::SetIP( const std::string& newIP ) -> void
{
	ip = newIP;
}
//==============================================================================================
auto PhysicalServer::SetPort( UI16 newPort ) -> void
{
	port = newPort;
}
//==============================================================================================
auto PhysicalServer::GetName() const -> const std::string&
{
	return name;
}
//==============================================================================================
auto PhysicalServer::GetDomain() const -> const std::string &
{
	return domain;
}
//==============================================================================================
auto PhysicalServer::GetIP() const -> const std::string &
{
	return ip;
}
//==============================================================================================
auto PhysicalServer::GetPort() const -> UI16
{
	return port;
}
