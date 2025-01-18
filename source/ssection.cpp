//o------------------------------------------------------------------------------------------------o
//| ssection.cpp
//o------------------------------------------------------------------------------------------------o
//| Script Section class implementation
//o------------------------------------------------------------------------------------------------o

#include "uox3.h"
#include "ssection.h"

constexpr auto DFN_STRING 		= UI08( 0 );
constexpr auto DFN_NUMERIC 		= UI08( 1 );
constexpr auto DFN_UPPERSTRING 	= UI08( 2 );
constexpr auto DFN_NODATA 		= UI08( 3 );
constexpr auto DFN_UNKNOWN 		= UI08( 4 );
constexpr auto DFN_DOUBLENUMERIC= UI08( 5 );

using namespace std::string_literals;
// 04302004 - Added DFN_UPPERSTRING to support the new DFNTAG_ADDMENUITEM tag
const UI08 dfnDataTypes[DFNTAG_COUNTOFTAGS] =
{
	DFN_NUMERIC,		//	DFNTAG_AC = 0,
	DFN_UPPERSTRING,	//	DFNTAG_ADDMENUITEM,
	DFN_NUMERIC,		//	DFNTAG_ADVOBJ,
	DFN_DOUBLENUMERIC,	//	DFNTAG_ALCHEMY,
	DFN_NUMERIC,		//	DFNTAG_ARTIFACTRARITY
	DFN_STRING,			//	DFNTAG_AMMO,
	DFN_STRING,			//	DFNTAG_AMMOFX,
	DFN_NUMERIC,		//	DFNTAG_AMOUNT,
	DFN_DOUBLENUMERIC,	//	DFNTAG_ANATOMY,
	DFN_DOUBLENUMERIC,	//	DFNTAG_ANIMALLORE,
	DFN_DOUBLENUMERIC,	//	DFNTAG_ARCHERY,
	DFN_DOUBLENUMERIC,	//	DFNTAG_ARMSLORE,
	DFN_DOUBLENUMERIC,	//	DFNTAG_ATT,
	DFN_NUMERIC,		//	DFNTAG_AWAKE,
	DFN_NODATA,			//	DFNTAG_BACKPACK,
	DFN_NUMERIC,		//	DFNTAG_BASERANGE,
	DFN_DOUBLENUMERIC,	//	DFNTAG_BEGGING,
	DFN_DOUBLENUMERIC,	//	DFNTAG_BLACKSMITHING,
	DFN_DOUBLENUMERIC,	//	DFNTAG_BOWCRAFT,
	DFN_DOUBLENUMERIC,	//	DFNTAG_BUSHIDO
	DFN_DOUBLENUMERIC,	//	DFNTAG_CAMPING,
	DFN_DOUBLENUMERIC,	//	DFNTAG_CARPENTRY,
	DFN_DOUBLENUMERIC,	//	DFNTAG_CARTOGRAPHY,
	DFN_NUMERIC,		//	DFNTAG_CARVE,
	DFN_DOUBLENUMERIC,	//	DFNTAG_CHIVALRY,
	DFN_NUMERIC,		//	DFNTAG_COLD,
	DFN_NUMERIC,		//	DFNTAG_COLOUR,
	DFN_STRING,			//	DFNTAG_COLOURLIST,
	DFN_NODATA,			//	DFNTAG_COLOURMATCHHAIR,
	DFN_NUMERIC,		//	DFNTAG_CONTROLSLOTS,
	DFN_DOUBLENUMERIC,	//	DFNTAG_COOKING,
	DFN_NUMERIC,		//	DFNTAG_CORPSE,
	DFN_NUMERIC,		//	DFNTAG_CREATOR,
	DFN_STRING,			//	DFNTAG_CUSTOMSTRINGTAG,
	DFN_STRING,			//	DFNTAG_CUSTOMINTTAG,
	DFN_DOUBLENUMERIC,	//	DFNTAG_DAMAGE,
	DFN_NUMERIC,		//	DFNTAG_DAMAGEABLE,
	DFN_NUMERIC,		//	DFNTAG_DECAY,
	DFN_DOUBLENUMERIC,	//	DFNTAG_DEF,
	DFN_DOUBLENUMERIC,	//	DFNTAG_DEFBONUS,
	DFN_DOUBLENUMERIC,	//	DFNTAG_DETECTINGHIDDEN,
	DFN_DOUBLENUMERIC,	//	DFNTAG_DEX,
	DFN_NUMERIC,		//	DFNTAG_DEXADD,
	DFN_STRING,			//	DFNTAG_DIR,
	DFN_NODATA,			//	DFNTAG_DISPELLABLE,
	DFN_NUMERIC,		//	DFNTAG_DISABLED,
	DFN_NUMERIC,		//	DFNTAG_DOORFLAG,
	DFN_NUMERIC,		//	DFNTAG_DYE,
	DFN_NUMERIC,		//	DFNTAG_DYEBEARD,
	DFN_NUMERIC,		//	DFNTAG_DYEHAIR,
	DFN_STRING,			//	DFNTAG_ELEMENTRESIST,
	DFN_STRING,			//	DFNTAG_ERBONUS,
	DFN_NUMERIC,		//	DFNTAG_EMOTECOLOUR,
	DFN_DOUBLENUMERIC,	//	DFNTAG_ENTICEMENT,
	DFN_UPPERSTRING,	//	DFNTAG_EQUIPITEM,
	DFN_DOUBLENUMERIC,	//	DFNTAG_EVALUATINGINTEL,
	DFN_STRING,			//	DFNTAG_EVENT,
	DFN_NUMERIC,		//	DFNTAG_FAME,
	DFN_DOUBLENUMERIC,	//	DFNTAG_FENCING,
	DFN_DOUBLENUMERIC,	//	DFNTAG_FISHING,
	DFN_UPPERSTRING,	//	DFNTAG_FLAG,
	DFN_NUMERIC,		//	DFNTAG_FLEEAT,
	DFN_STRING,			//  DFNTAG_FLEEINGSPEED,
	DFN_STRING,			//  DFNTAG_FLEEINGSPEEDMOUNTED,
	DFN_DOUBLENUMERIC,	//	DFNTAG_FOCUS,
	DFN_DOUBLENUMERIC,	//	DFNTAG_FORENSICS,
	DFN_NUMERIC,		//	DFNTAG_FX1,
	DFN_NUMERIC,		//	DFNTAG_FX2,
	DFN_NUMERIC,		//	DFNTAG_FY1,
	DFN_NUMERIC,		//	DFNTAG_FY2,
	DFN_NUMERIC,		//	DFNTAG_FZ1,
	DFN_UPPERSTRING,	//	DFNTAG_FOOD,
	DFN_UPPERSTRING,	//	DFNTAG_GET,
	DFN_UPPERSTRING,	//	DFNTAG_GETUO,
	DFN_UPPERSTRING,	//	DFNTAG_GETT2A,
	DFN_UPPERSTRING,	//	DFNTAG_GETUOR,
	DFN_UPPERSTRING,	//	DFNTAG_GETTD,
	DFN_UPPERSTRING,	//	DFNTAG_GETLBR,
	DFN_UPPERSTRING,	//	DFNTAG_GETAOS,
	DFN_UPPERSTRING,	//	DFNTAG_GETSE,
	DFN_UPPERSTRING,	//	DFNTAG_GETML,
	DFN_UPPERSTRING,	//	DFNTAG_GETSA,
	DFN_UPPERSTRING,	//	DFNTAG_GETHS,
	DFN_UPPERSTRING,	//	DFNTAG_GETTOL,
	DFN_NUMERIC,		//	DFNTAG_GLOW,
	DFN_NUMERIC,		//	DFNTAG_GLOWBC,
	DFN_NUMERIC,		//	DFNTAG_GLOWTYPE,
	DFN_DOUBLENUMERIC,	//	DFNTAG_GOLD,
	DFN_NUMERIC,		//	DFNTAG_GOOD,
	DFN_STRING,			//	DFNTAG_HAIRCOLOUR,
	DFN_DOUBLENUMERIC,	//	DFNTAG_HEALING,
	DFN_NUMERIC,		//	DFNTAG_HEAT,
	DFN_DOUBLENUMERIC,	//	DFNTAG_HERDING,
	DFN_NUMERIC,		//	DFNTAG_HIDAMAGE,
	DFN_DOUBLENUMERIC,	//	DFNTAG_HIDING,
	DFN_NODATA,			//	DFNTAG_HIRELING,
	DFN_DOUBLENUMERIC,	//	DFNTAG_HP,
	DFN_DOUBLENUMERIC,	//	DFNTAG_HPMAX,
	DFN_UPPERSTRING,	//	DFNTAG_ID,
	DFN_DOUBLENUMERIC,	//	DFNTAG_IMBUING,
	DFN_NUMERIC,		//	DFNTAG_INTADD,
	DFN_DOUBLENUMERIC,	//	DFNTAG_INTELLIGENCE,
	DFN_DOUBLENUMERIC,	//	DFNTAG_INTERVAL,
	DFN_DOUBLENUMERIC,	//	DFNTAG_INSCRIPTION,
	DFN_NUMERIC,		//	DFNTAG_ITEMID,
	DFN_UPPERSTRING,	//	DFNTAG_ITEMLIST,
	DFN_NUMERIC,		//	DFNTAG_KARMA,
	DFN_NODATA,			//	DFNTAG_KILLHAIR,
	DFN_NODATA,			//	DFNTAG_KILLBEARD,
	DFN_NODATA,			//	DFNTAG_KILLPACK,
	DFN_NUMERIC,		//	DFNTAG_LAYER,
	DFN_NUMERIC,		//	DFNTAG_LIGHT,
	DFN_NUMERIC,		//	DFNTAG_LIGHTNING,
	DFN_NUMERIC,		//	DFNTAG_LOCKPICKING,
	DFN_NUMERIC,		//	DFNTAG_LODAMAGE,
	DFN_UPPERSTRING,	//	DFNTAG_LOOT,
	DFN_NUMERIC,		//	DFNTAG_LOYALTY,
	DFN_DOUBLENUMERIC,	//	DFNTAG_LUMBERJACKING,
	DFN_DOUBLENUMERIC,	//	DFNTAG_MACEFIGHTING,
	DFN_DOUBLENUMERIC,	//	DFNTAG_MAGERY,
	DFN_DOUBLENUMERIC,	//	DFNTAG_MAGICRESISTANCE,
	DFN_DOUBLENUMERIC,	//	DFNTAG_MANA,
	DFN_DOUBLENUMERIC,	//	DFNTAG_MANAMAX,
	DFN_NUMERIC,		//	DFNTAG_MAXHP,
	DFN_NUMERIC,		//	DFNTAG_MAXITEMS,
	DFN_NUMERIC,		//	DFNTAG_MAXLOYALTY,
	DFN_NUMERIC,		//	DFNTAG_MAXRANGE,
	DFN_NUMERIC,		//	DFNTAG_MAXUSES,
	DFN_DOUBLENUMERIC,	//	DFNTAG_MEDITATION,
	DFN_DOUBLENUMERIC,	//	DFNTAG_MINING,
	DFN_NUMERIC,		//	DFNTAG_MOVABLE,
	DFN_STRING,			//	DFNTAG_MORE,
	DFN_STRING,			//	DFNTAG_MORE0,
	DFN_STRING,			//	DFNTAG_MORE1,
	DFN_STRING,			//	DFNTAG_MORE2,
	DFN_STRING,			//	DFNTAG_MOREX,
	DFN_STRING,			//	DFNTAG_MOREY,
	DFN_STRING,			//	DFNTAG_MOREZ,
	DFN_DOUBLENUMERIC,	//	DFNTAG_MUSICIANSHIP,
	DFN_DOUBLENUMERIC,	//	DFNTAG_MYSTICISM,
	DFN_STRING,			//	DFNTAG_NAME,
	DFN_STRING,			//	DFNTAG_NAME2,
	DFN_STRING,			//	DFNTAG_NAMELIST,
	DFN_DOUBLENUMERIC,	//	DFNTAG_NECROMANCY,
	DFN_NODATA,			//	DFNTAG_NEWBIE,
	DFN_DOUBLENUMERIC,	//	DFNTAG_NINJITSU,
	DFN_NODATA,			//	DFNTAG_NOHIRELING,
	DFN_NODATA,			//	DFNTAG_NOTRAIN,
	DFN_NUMERIC,		//	DFNTAG_NPCAI,
	DFN_NUMERIC,		//	DFNTAG_NPCGUILD,
	DFN_UPPERSTRING,	//	DFNTAG_NPCLIST,
	DFN_NUMERIC,		//	DFNTAG_NPCWANDER,
	DFN_NUMERIC,		//	DFNTAG_OFFSPELL,
	DFN_STRING,			//	DFNTAG_ORIGIN,
	DFN_STRING,			//	DFNTAG_PACKITEM,
	DFN_DOUBLENUMERIC,	//	DFNTAG_PARRYING,
	DFN_DOUBLENUMERIC,	//	DFNTAG_PEACEMAKING,
	DFN_NUMERIC,		//	DFNTAG_PILEABLE,
	DFN_NUMERIC,		//	DFNTAG_POISONDAMAGE,
	DFN_NUMERIC,		//	DFNTAG_POISONSTRENGTH
	DFN_NUMERIC,		//	DFNTAG_POISONED,
	DFN_DOUBLENUMERIC,	//	DFNTAG_POISONING,
	DFN_NUMERIC,		//	DFNTAG_POLY,
	DFN_NUMERIC,		//	DFNTAG_PRIV,
	DFN_DOUBLENUMERIC,	//	DFNTAG_PROVOCATION,
	DFN_NUMERIC,		//	DFNTAG_RACE,
	DFN_NUMERIC,		//	DFNTAG_RAIN,
	DFN_NUMERIC,		//	DFNTAG_RANK,
	DFN_NUMERIC,		//	DFNTAG_REATTACKAT,
	DFN_DOUBLENUMERIC,	//	DFNTAG_REMOVETRAP,
	DFN_NUMERIC,		//	DFNTAG_RESTOCK,
	DFN_DOUBLENUMERIC,	//	DFNTAG_RESISTFIRE,
	DFN_DOUBLENUMERIC,	//	DFNTAG_RESISTCOLD,
	DFN_DOUBLENUMERIC,	//	DFNTAG_RESISTLIGHTNING,
	DFN_DOUBLENUMERIC,	//	DFNTAG_RESISTPOISON,
	DFN_STRING,			//	DFNTAG_RSHOPITEM,
	DFN_STRING,			//  DFNTAG_RUNNINGSPEED,
	DFN_STRING,			//  DFNTAG_RUNNINGSPEEDMOUNTED,
	DFN_NODATA,			//	DFNTAG_RUNS,
	DFN_NUMERIC,		//	DFNTAG_SAYCOLOUR,
	DFN_NUMERIC,		//	DFNTAG_SCRIPT,
	DFN_STRING,			//	DFNTAG_SECTIONID,
	DFN_STRING,			//	DFNTAG_SELLITEM,
	DFN_STRING,			//	DFNTAG_SHOPITEM,
	DFN_NODATA,			//	DFNTAG_SHOPKEEPER,
	DFN_STRING,			//	DFNTAG_SHOPLIST,
	DFN_NUMERIC,		//	DFNTAG_SK_MADE,
	DFN_DOUBLENUMERIC,	//	DFNTAG_SKILL,
	DFN_UNKNOWN,		//	DFNTAG_SKILLVAR,
	DFN_NUMERIC,		//	DFNTAG_SKIN,
	DFN_STRING,			//	DFNTAG_SKINLIST,
	DFN_DOUBLENUMERIC,	//	DFNTAG_SNOOPING,
	DFN_NUMERIC,		//	DFNTAG_SNOW,
	DFN_NUMERIC,		//	DFNTAG_SPADELAY,
	DFN_NUMERIC,		//	DFNTAG_SPATTACK,
	DFN_STRING,			//	DFNTAG_SPAWNOBJ,
	DFN_STRING,			//	DFNTAG_SPAWNOBJLIST,
	DFN_NUMERIC,		//	DFNTAG_SPD,
	DFN_NUMERIC,		//  DFNTAG_HEALTHBONUS,
	DFN_NUMERIC,		//  DFNTAG_STAMINABONUS,
	DFN_NUMERIC,		//  DFNTAG_MANABONUS,
	DFN_STRING,			//	DFNTAG_SPELLS,
	DFN_DOUBLENUMERIC,	//	DFNTAG_SPELLWEAVING,
	DFN_DOUBLENUMERIC,	//	DFNTAG_SPIRITSPEAK,
	DFN_NUMERIC,		//	DFNTAG_SPLIT,
	DFN_NUMERIC,		//	DFNTAG_SPLITCHANCE,
	DFN_DOUBLENUMERIC,	//	DFNTAG_STAMINA,
	DFN_DOUBLENUMERIC,	//	DFNTAG_STAMINAMAX,
	DFN_DOUBLENUMERIC,	//	DFNTAG_STRENGTH,
	DFN_NUMERIC,		//	DFNTAG_STRADD,
	DFN_NUMERIC,		//	DFNTAG_STEALABLE,
	DFN_DOUBLENUMERIC,	//	DFNTAG_STEALING,
	DFN_DOUBLENUMERIC,	//	DFNTAG_STEALTH,
	DFN_DOUBLENUMERIC,	//	DFNTAG_SWORDSMANSHIP,
	DFN_DOUBLENUMERIC,	//	DFNTAG_TACTICS,
	DFN_DOUBLENUMERIC,	//	DFNTAG_TAILORING,
	DFN_DOUBLENUMERIC,	//	DFNTAG_TAMING,
	DFN_DOUBLENUMERIC,	//	DFNTAG_TAMEDHUNGER,
	DFN_DOUBLENUMERIC,	//	DFNTAG_TAMEDTHIRST,
	DFN_DOUBLENUMERIC,	//	DFNTAG_TASTEID,
	DFN_DOUBLENUMERIC,	//	DFNTAG_THROWING,
	DFN_DOUBLENUMERIC,	//	DFNTAG_TINKERING,
	DFN_STRING,			//	DFNTAG_TITLE,
	DFN_NUMERIC,		//	DFNTAG_TOTAME,
	DFN_NUMERIC,		//	DFNTAG_TOPROV,
	DFN_DOUBLENUMERIC,	//	DFNTAG_TOPEACE,
	DFN_DOUBLENUMERIC,	//	DFNTAG_TRACKING,
	DFN_NUMERIC,		//	DFNTAG_TYPE,
	DFN_NUMERIC,		//	DFNTAG_USESLEFT,
	DFN_DOUBLENUMERIC,	//	DFNTAG_VALUE,
	DFN_DOUBLENUMERIC,	//	DFNTAG_VETERINARY,
	DFN_NUMERIC,		//	DFNTAG_VISIBLE,
	DFN_STRING,			//  DFNTAG_WALKINGSPEED,
	DFN_STRING,			//  DFNTAG_WALKINGSPEEDMOUNTED,
	DFN_NUMERIC,		//	DFNTAG_WEIGHT,
	DFN_NUMERIC,		//	DFNTAG_WEIGHTMAX,
	DFN_NUMERIC,		//	DFNTAG_WILLHUNGER,
	DFN_NUMERIC,		//	DFNTAG_WILLTHIRST,
	DFN_NUMERIC,		//	DFNTAG_WIPE,
	DFN_DOUBLENUMERIC	//	DFNTAG_WRESTLING
};

struct StrToDFNLookup_st
{
	std::string strToAdd;
	DFNTAGS	dfnToAdd;
};

const std::map<std::string, DFNTAGS> strToDFNTag
{
	{"AC"s,					DFNTAG_AC},
	{"ADDMENUITEM"s,		DFNTAG_ADDMENUITEM},
	{"ADVOBJ"s,				DFNTAG_ADVOBJ},
	{"ALCHEMY"s,			DFNTAG_ALCHEMY},
	{"ARTIFACTRARITY"s,		DFNTAG_ARTIFACTRARITY},
	{"AMMO"s,				DFNTAG_AMMO},
	{"AMMOFX"s,				DFNTAG_AMMOFX},
	{"AMOUNT"s,				DFNTAG_AMOUNT},
	{"ANATOMY"s,			DFNTAG_ANATOMY},
	{"ANIMALLORE"s,			DFNTAG_ANIMALLORE},
	{"ARCHERY"s,			DFNTAG_ARCHERY},
	{"ARMSLORE"s,			DFNTAG_ARMSLORE},
	{"ATT"s,				DFNTAG_ATT},
	{"AWAKE"s,				DFNTAG_AWAKE},
	{"BACKPACK"s,			DFNTAG_BACKPACK},
	{"BASERANGE"s,			DFNTAG_BASERANGE},
	{"BEGGING"s,			DFNTAG_BEGGING},
	{"BLACKSMITHING"s,		DFNTAG_BLACKSMITHING},
	{"BOWCRAFT"s,			DFNTAG_BOWCRAFT},
	{"BUSHIDO"s,			DFNTAG_BUSHIDO},
	{"HEALTHBONUS"s,		DFNTAG_HEALTHBONUS},
	{"STAMINABONUS"s,		DFNTAG_STAMINABONUS},
	{"MANABONUS"s,			DFNTAG_MANABONUS},
	{"CAMPING"s,			DFNTAG_CAMPING},
	{"CARPENTRY"s,			DFNTAG_CARPENTRY},
	{"CARTOGRAPHY"s,		DFNTAG_CARTOGRAPHY},
	{"CARVE"s,				DFNTAG_CARVE},
	{"CHIVALRY"s,			DFNTAG_CHIVALRY},
	{"COLD"s,				DFNTAG_COLD},
	{"COLOR"s,				DFNTAG_COLOUR},
	{"COLOUR"s,				DFNTAG_COLOUR},
	{"COLORLIST"s,			DFNTAG_COLOURLIST},
	{"COLOURLIST"s,			DFNTAG_COLOURLIST},
	{"COLORMATCHHAIR"s,		DFNTAG_COLOURMATCHHAIR},
	{"COLOURMATCHHAIR"s,	DFNTAG_COLOURMATCHHAIR},
	{"CONTROLSLOTS"s,		DFNTAG_CONTROLSLOTS},
	{"COOKING"s,			DFNTAG_COOKING},
	{"CORPSE"s,				DFNTAG_CORPSE},
	{"CREATOR"s,			DFNTAG_CREATOR},
	{"CUSTOMSTRINGTAG"s,	DFNTAG_CUSTOMSTRINGTAG},
	{"CUSTOMINTTAG"s,		DFNTAG_CUSTOMINTTAG},
	{"DAMAGE"s,				DFNTAG_DAMAGE},
	{"DAMAGEABLE"s,			DFNTAG_DAMAGEABLE},
	{"DECAY"s,				DFNTAG_DECAY},
	{"DEF"s,				DFNTAG_DEF},
	{"DEFBONUS"s,			DFNTAG_DEFBONUS},
	{"DETECTINGHIDDEN"s,	DFNTAG_DETECTINGHIDDEN},
	{"DEX"s,				DFNTAG_DEX},
	{"DEXTERITY"s,			DFNTAG_DEX},
	{"DX"s,					DFNTAG_DEX},
	{"DEXADD"s,				DFNTAG_DEXADD},
	{"DX2"s,				DFNTAG_DEXADD},
	{"DIR"s,				DFNTAG_DIR},
	{"DIRECTION"s,			DFNTAG_DIR},
	{"DISPELLABLE"s,		DFNTAG_DISPELLABLE},
	{"DISABLED"s,			DFNTAG_DISABLED},
	{"DOORFLAG"s,			DFNTAG_DOORFLAG},
	{"DYE"s,				DFNTAG_DYE},
	{"DYEABLE"s,			DFNTAG_DYE},
	{"DYEHAIR"s,			DFNTAG_DYEHAIR},
	{"DYEBEARD"s,			DFNTAG_DYEBEARD},
	{"ELEMENTRESIST"s,		DFNTAG_ELEMENTRESIST},
	{"ERBONUS"s,			DFNTAG_ERBONUS},	
	{"EMOTECOLOR"s,			DFNTAG_EMOTECOLOUR},
	{"EMOTECOLOUR"s,		DFNTAG_EMOTECOLOUR},
	{"ENTICEMENT"s,			DFNTAG_ENTICEMENT},
	{"EVALUATINGINTEL"s,	DFNTAG_EVALUATINGINTEL},
	{"EVENT"s,				DFNTAG_EVENT},
	{"FAME"s,				DFNTAG_FAME},
	{"FENCING"s,			DFNTAG_FENCING},
	{"FISHING"s,			DFNTAG_FISHING},
	{"FLAG"s,				DFNTAG_FLAG},
	{"FLEEAT"s,				DFNTAG_FLEEAT},
	{"FLEEINGSPEED"s,		DFNTAG_FLEEINGSPEED},
	{"FLEEINGSPEEDMOUNTED"s,DFNTAG_FLEEINGSPEEDMOUNTED},
	{"FOCUS"s,				DFNTAG_FOCUS},
	{"FORENSICS"s,			DFNTAG_FORENSICS},
	{"FX1"s,				DFNTAG_FX1},
	{"FX2"s,				DFNTAG_FX2},
	{"FY1"s,				DFNTAG_FY1},
	{"FY2"s,				DFNTAG_FY2},
	{"FZ1"s,				DFNTAG_FZ1},
	{"FOOD"s,				DFNTAG_FOOD},
	{"GET"s,				DFNTAG_GET},
	{"GET"s,				DFNTAG_GETUO},
	{"GETT2A"s,				DFNTAG_GETT2A},
	{"GETUOR"s,				DFNTAG_GETUOR},
	{"GETTD"s,				DFNTAG_GETTD},
	{"GETLBR"s,				DFNTAG_GETLBR},
	{"GETAOS"s,				DFNTAG_GETAOS},
	{"GETSE"s,				DFNTAG_GETSE},
	{"GETML"s,				DFNTAG_GETML},
	{"GETSA"s,				DFNTAG_GETSA},
	{"GETHS"s,				DFNTAG_GETHS},
	{"GETTOL"s,				DFNTAG_GETTOL},
	{"GLOW"s,				DFNTAG_GLOW},
	{"GLOWBC"s,				DFNTAG_GLOWBC},
	{"GLOWTYPE"s,			DFNTAG_GLOWTYPE},
	{"GOLD"s,				DFNTAG_GOLD},
	{"GOOD"s,				DFNTAG_GOOD},
	{"HAIRCOLOR"s,			DFNTAG_HAIRCOLOUR},
	{"HAIRCOLOUR"s,			DFNTAG_HAIRCOLOUR},
	{"HEALING"s,			DFNTAG_HEALING},
	{"HEAT"s,				DFNTAG_HEAT},
	{"HERDING"s,			DFNTAG_HERDING},
	{"HIDAMAGE"s,			DFNTAG_HIDAMAGE},
	{"HIDING"s,				DFNTAG_HIDING},
	{"HIRELING"s,			DFNTAG_HIRELING},
	{"HP"s,					DFNTAG_HP},
	{"HPMAX"s,				DFNTAG_HPMAX},
	{"ID"s,					DFNTAG_ID},
	{"IMBUING"s,			DFNTAG_IMBUING},
	{"IN"s,					DFNTAG_INTELLIGENCE},
	{"INTELLIGENCE"s,		DFNTAG_INTELLIGENCE},
	{"INT"s,				DFNTAG_INTELLIGENCE},
	{"IN2"s,				DFNTAG_INTADD},
	{"INTADD"s,				DFNTAG_INTADD},
	{"INTERVAL"s,			DFNTAG_INTERVAL},
	{"INSCRIPTION"s,		DFNTAG_INSCRIPTION},
	{"EQUIPITEM"s,			DFNTAG_EQUIPITEM},
	{"ITEMLIST"s,			DFNTAG_ITEMLIST},
	{"ITEMID"s,				DFNTAG_ITEMID},
	{"KARMA"s,				DFNTAG_KARMA},
	{"KILLHAIR"s,			DFNTAG_KILLHAIR},
	{"KILLBEARD"s,			DFNTAG_KILLBEARD},
	{"KILLPACK"s,			DFNTAG_KILLPACK},
	{"LAYER"s,				DFNTAG_LAYER},
	{"LIGHT"s,				DFNTAG_LIGHT},
	{"LIGHTNING"s,			DFNTAG_LIGHTNING},
	{"LOCKPICKING"s,		DFNTAG_LOCKPICKING},
	{"LODAMAGE"s,			DFNTAG_LODAMAGE},
	{"LOOT"s,				DFNTAG_LOOT},
	{"LOYALTY"s,			DFNTAG_MAXLOYALTY},
	{"LUMBERJACKING"s,		DFNTAG_LUMBERJACKING},
	{"MACEFIGHTING"s,		DFNTAG_MACEFIGHTING},
	{"MAGERY"s,				DFNTAG_MAGERY},
	{"MAGICRESISTANCE"s,	DFNTAG_MAGICRESISTANCE},
	{"MANA"s,				DFNTAG_MANA},
	{"MANAMAX"s,			DFNTAG_MANAMAX},
	{"MAXHP"s,				DFNTAG_MAXHP},
	{"MAXITEMS"s,			DFNTAG_MAXITEMS},
	{"MAXLOYALTY"s,			DFNTAG_MAXLOYALTY},
	{"MAXRANGE"s,			DFNTAG_MAXRANGE},
	{"MAXUSES"s,			DFNTAG_MAXUSES},
	{"MEDITATION"s,			DFNTAG_MEDITATION},
	{"MINING"s,				DFNTAG_MINING},
	{"MOVABLE"s,			DFNTAG_MOVABLE},
	{"MORE"s,				DFNTAG_MORE},
	{"MORE0"s,				DFNTAG_MORE0},
	{"MORE1"s,				DFNTAG_MORE1},
	{"MORE2"s,				DFNTAG_MORE2},
	{"MOREX"s,				DFNTAG_MOREX},
	{"MOREY"s,				DFNTAG_MOREY},
	{"MOREZ"s,				DFNTAG_MOREZ},
	{"MUSICIANSHIP"s,		DFNTAG_MUSICIANSHIP},
	{"MYSTICISM"s,			DFNTAG_MYSTICISM},
	{"NAME"s,				DFNTAG_NAME},
	{"NAME2"s,				DFNTAG_NAME2},
	{"NAMELIST"s,			DFNTAG_NAMELIST},
	{"NECROMANCY"s,			DFNTAG_NECROMANCY},
	{"NEWBIE"s,				DFNTAG_NEWBIE},
	{"NINJITSU"s,			DFNTAG_NINJITSU},
	{"NOHIRELING"s,			DFNTAG_NOHIRELING},
	{"NOTRAIN"s,			DFNTAG_NOTRAIN},
	{"NPCAI"s,				DFNTAG_NPCAI},
	{"NPCGUILD"s,			DFNTAG_NPCGUILD},
	{"NPCLIST"s,			DFNTAG_NPCLIST},
	{"NPCWANDER"s,			DFNTAG_NPCWANDER},
	{"OFFSPELL"s,			DFNTAG_OFFSPELL},
	{"ORIGIN"s,				DFNTAG_ORIGIN},
	{"PACKITEM"s,			DFNTAG_PACKITEM},
	{"PARRYING"s,			DFNTAG_PARRYING},
	{"PEACEMAKING"s,		DFNTAG_PEACEMAKING},
	{"PILEABLE"s,			DFNTAG_PILEABLE},
	{"POLY"s,				DFNTAG_POLY},
	{"POISONDAMAGE"s,		DFNTAG_POISONDAMAGE},
	{"POISONSTRENGTH"s,		DFNTAG_POISONSTRENGTH},
	{"POISONED"s,			DFNTAG_POISONED},
	{"POISONING"s,			DFNTAG_POISONING},
	{"PRIV"s,				DFNTAG_PRIV},
	{"PROVOCATION"s,		DFNTAG_PROVOCATION},
	{"RACE"s,				DFNTAG_RACE},
	{"RAIN"s,				DFNTAG_RAIN},
	{"RANK"s,				DFNTAG_RANK},
	{"REATTACKAT"s,			DFNTAG_REATTACKAT},
	{"RESISTFIRE"s,			DFNTAG_RESISTFIRE},
	{"RESISTCOLD"s,			DFNTAG_RESISTCOLD},
	{"RESISTLIGHTNING"s,	DFNTAG_RESISTLIGHTNING},
	{"RESISTPOISON"s,		DFNTAG_RESISTPOISON},
	{"REMOVETRAP"s,			DFNTAG_REMOVETRAP},
	{"RESTOCK"s,			DFNTAG_RESTOCK},
	{"RSHOPITEM"s,			DFNTAG_RSHOPITEM},
	{"RUNS"s,				DFNTAG_RUNS},
	{"RUNNINGSPEED"s,		DFNTAG_RUNNINGSPEED},
	{"RUNNINGSPEEDMOUNTED"s, DFNTAG_RUNNINGSPEEDMOUNTED},
	{"SAYCOLOR"s,			DFNTAG_SAYCOLOUR},
	{"SAYCOLOUR"s,			DFNTAG_SAYCOLOUR},
	{"SCRIPT"s,				DFNTAG_SCRIPT},
	{"SECTIONID"s,			DFNTAG_SECTIONID},
	{"SELLITEM"s,			DFNTAG_SELLITEM},
	{"SHOPITEM"s,			DFNTAG_SHOPITEM},
	{"SHOPKEEPER"s,			DFNTAG_SHOPKEEPER},
	{"SHOPLIST"s,			DFNTAG_SHOPLIST},
	{"SK_MADE"s,			DFNTAG_SK_MADE},
	{"SKILL"s,				DFNTAG_SKILL},
	{"SKIN"s,				DFNTAG_SKIN},
	{"SKINLIST"s,			DFNTAG_SKINLIST},
	{"SNOOPING"s,			DFNTAG_SNOOPING},
	{"SNOW"s,				DFNTAG_SNOW},
	{"SPADELAY"s,			DFNTAG_SPADELAY},
	{"SPATTACK"s,			DFNTAG_SPATTACK},
	{"SPAWNOBJ"s,			DFNTAG_SPAWNOBJ},
	{"SPAWNOBJLIST"s,		DFNTAG_SPAWNOBJLIST},
	{"SPD"s,				DFNTAG_SPD},
	{"SPEED"s,				DFNTAG_SPD},
	{"SPELLS"s,				DFNTAG_SPELLS},
	{"SPELLWEAVING"s,		DFNTAG_SPELLWEAVING},
	{"SPIRITSPEAK"s,		DFNTAG_SPIRITSPEAK},
	{"SPLIT"s,				DFNTAG_SPLIT},
	{"SPLITCHANCE"s,		DFNTAG_SPLITCHANCE},
	{"ST"s,					DFNTAG_STRENGTH},
	{"STAMINA"s,			DFNTAG_STAMINA},
	{"STAMINAMAX"s,			DFNTAG_STAMINAMAX},
	{"STR"s,				DFNTAG_STRENGTH},
	{"STRENGTH"s,			DFNTAG_STRENGTH},
	{"ST2"s,				DFNTAG_STRADD},
	{"STRADD"s,				DFNTAG_STRADD},
	{"STEALABLE"s,			DFNTAG_STEALABLE},
	{"STEALING"s,			DFNTAG_STEALING},
	{"STEALTH"s,			DFNTAG_STEALTH},
	{"SWORDSMANSHIP"s,		DFNTAG_SWORDSMANSHIP},
	{"TACTICS"s,			DFNTAG_TACTICS},
	{"TAILORING"s,			DFNTAG_TAILORING},
	{"TAMING"s,				DFNTAG_TAMING},
	{"TAMEDHUNGER"s,		DFNTAG_TAMEDHUNGER},
	{"TAMEDTHIRST"s,		DFNTAG_TAMEDTHIRST},
	{"TASTEID"s,			DFNTAG_TASTEID},
	{"THROWING"s,			DFNTAG_THROWING},
	{"TINKERING"s,			DFNTAG_TINKERING},
	{"TITLE"s,				DFNTAG_TITLE},
	{"TOTAME"s,				DFNTAG_TOTAME},
	{"TOPROV"s,				DFNTAG_TOPROV},
	{"TOPEACE"s,			DFNTAG_TOPEACE},
	{"TRACKING"s,			DFNTAG_TRACKING},
	{"TYPE"s,				DFNTAG_TYPE},
	{"USESLEFT"s,			DFNTAG_USESLEFT},
	{"VALUE"s,				DFNTAG_VALUE},
	{"VETERINARY"s,			DFNTAG_VETERINARY},
	{"VISIBLE"s,			DFNTAG_VISIBLE},
	{"WALKINGSPEED"s,		DFNTAG_WALKINGSPEED},
	{"WALKINGSPEEDMOUNTED"s, DFNTAG_WALKINGSPEEDMOUNTED},
	{"WEIGHT"s,				DFNTAG_WEIGHT},
	{"WEIGHTMAX"s,			DFNTAG_WEIGHTMAX},
	{"WILLHUNGER"s,			DFNTAG_WILLHUNGER},
	{"WIPE"s,				DFNTAG_WIPE},
	{"WRESTLING"s,			DFNTAG_WRESTLING}
};

//o------------------------------------------------------------------------------------------------o
//|	Function	-	FindDFNTagFromStr()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Find a valid DFN tag from a provided string
//o------------------------------------------------------------------------------------------------o
auto FindDFNTagFromStr( const std::string &strToFind ) ->DFNTAGS
{
	auto iter = strToDFNTag.find( oldstrutil::upper( strToFind ));
	if( iter != strToDFNTag.end() )
	{
		return iter->second;
	}
	return DFNTAG_COUNTOFTAGS;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CScriptSection::CScriptSection() : dfnCat( NUM_DEFS ),
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Default constructor, initializing all variables
//o------------------------------------------------------------------------------------------------o
CScriptSection::CScriptSection() : dfnCat( NUM_DEFS ),
npcList( false ), itemList( false ), npcListData( "" ), itemListData( "" )
{
	data.resize( 0 );
	dataV2.resize( 0 );
	currentPos	= data.end();
	currentPos2	= dataV2.end();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CScriptSection::CScriptSection( std::fstream& input, DEFINITIONCATEGORIES d )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Default constructor, initializing all variables
//|						and grabbing a section from the file passed in
//o------------------------------------------------------------------------------------------------o
CScriptSection::CScriptSection( std::istream& input, DEFINITIONCATEGORIES d ) :
dfnCat( d ), npcList( false ), itemList( false ), npcListData( "" ), itemListData( "" )
{
	data.resize( 0 );
	dataV2.resize( 0 );
	CreateSection( input );
	currentPos	= data.end();
	currentPos2	= dataV2.end();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CScriptSection::AtEnd()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if at end of the section
//o------------------------------------------------------------------------------------------------o
bool CScriptSection::AtEnd()
{
	return ( currentPos == data.end() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CScriptSection::First()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the first tag at the start of the section
//o------------------------------------------------------------------------------------------------o
auto CScriptSection::First() -> std::string
{
	std::string rValue;
	currentPos = data.begin();
	if( !AtEnd() )
	{
		rValue = ( *currentPos )->tag;
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CScriptSection::Next()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the next tag in the section, or nullptr if no more
//o------------------------------------------------------------------------------------------------o
auto CScriptSection::Next() -> std::string
{
	std::string rValue;
	if( !AtEnd() )
	{
		++currentPos;
		if( !AtEnd() )
		{
			rValue = ( *currentPos )->tag;
		}
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CScriptSection::MoveTo()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Moves to position in the section and returns the tag there
//o------------------------------------------------------------------------------------------------o
auto CScriptSection::MoveTo( size_t position ) -> std::string
{
	std::string rValue;
	std::vector<SectData_st *>::iterator curPos = currentPos;
	currentPos = ( data.begin() + position );
	if( !AtEnd() )
	{
		rValue = ( *currentPos )->tag;
	}
	else
	{
		currentPos = curPos;
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CScriptSection::GrabData()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the data for the current tag
//o------------------------------------------------------------------------------------------------o
auto CScriptSection::GrabData() -> std::string
{
	std::string rValue;
	if( !AtEnd() )
	{
		rValue = ( *currentPos )->data;
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CScriptSection::~CScriptSection()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Default deconstructor, removing any allocated memory
//|						and closing any files that may be open
//o------------------------------------------------------------------------------------------------o
CScriptSection::~CScriptSection()
{
	if( !FlushData() )
	{
		Console.Error( "Section unable to flush data!" );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CScriptSection::FlushData()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Deletes all tag/data pairs and resizes array to 0
//o------------------------------------------------------------------------------------------------o
auto CScriptSection::FlushData() -> bool
{
	for( size_t i = 0; i < data.size(); ++i )
	{
		delete data[i];
	}
	for( size_t j = 0; j < dataV2.size(); ++j )
	{
		delete dataV2[j];
	}
	data.resize( 0 );
	dataV2.resize( 0 );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CScriptSection::NumEntries()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the number of entries in the section
//o------------------------------------------------------------------------------------------------o
auto CScriptSection::NumEntries() const -> size_t
{
	return data.size();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CScriptSection::Prev()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the previous tag, or nullptr if at start
//o------------------------------------------------------------------------------------------------o
auto CScriptSection::Prev() -> std::string
{
	std::string rValue;
	if( currentPos != data.begin() )
	{
		--currentPos;
		if( !AtEnd() )
		{
			rValue = ( *currentPos )->tag;
		}
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CScriptSection::Remove()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes the tag/data pair at position in the array
//o------------------------------------------------------------------------------------------------o
auto CScriptSection::Remove( size_t position ) -> void
{
	if( AtEnd() )
	{
		return;
	}
	delete data[position];
	data.erase( data.begin() + position );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CScriptSection::Append()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a new tag/data pair at the end of the section
//o------------------------------------------------------------------------------------------------o
auto CScriptSection::Append( std::string tagToAdd, std::string dataToAdd ) -> void
{
	SectData_st *toAdd	= new SectData_st;
	toAdd->tag		= tagToAdd;
	toAdd->data		= dataToAdd;
	data.push_back( toAdd );
}

auto CScriptSection::GrabData( SI32& ndata, SI32& odata ) -> std::string
{
	std::string rValue;
	if( AtEndTags() )
	{
		ndata = INVALIDSERIAL;
		odata = INVALIDSERIAL;
	}
	else
	{
		ndata	= ( *currentPos2 )->ndata;
		odata	= ( *currentPos2 )->odata;
		rValue	= ( *currentPos2 )->cdata;
	}
	return rValue;
}
auto CScriptSection::AtEndTags() -> bool
{
	if( currentPos2 == dataV2.end() )
	{
		return true;
	}
	return false;
}
auto CScriptSection::PrevTag() ->DFNTAGS
{
	DFNTAGS rValue = DFNTAG_COUNTOFTAGS;
	if( currentPos2 != dataV2.begin() )
	{
		--currentPos2;
		if( !AtEndTags() )
		{
			rValue = ( *currentPos2 )->tag;
		}
	}
	return rValue;
}
auto CScriptSection::NextTag() ->DFNTAGS
{
	DFNTAGS rValue = DFNTAG_COUNTOFTAGS;
	if( !AtEndTags() )
	{
		++currentPos2;
		if( !AtEndTags() )
		{
			rValue = ( *currentPos2 )->tag;
		}
	}
	return rValue;
}
auto CScriptSection::FirstTag() ->DFNTAGS
{
	currentPos2 = dataV2.begin();
	if( AtEndTags() )
	{
		return DFNTAG_COUNTOFTAGS;
	}
	return ( *currentPos2 )->tag;
}

//==================================================================================================
auto CScriptSection::collection() const -> const std::vector<SectData_st *>&
{
	return data;
}
//==================================================================================================
auto CScriptSection::collection()  -> std::vector<SectData_st *>&
{
	return data;

}
//==================================================================================================
auto CScriptSection::collection2() const -> const std::vector<SectDataV2_st *>&
{
	return dataV2;
}
//==================================================================================================
auto CScriptSection::collection2()  -> std::vector<SectDataV2_st *>&
{
	return dataV2;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CScriptSection::ItemListExist()
//|	Date		-	12 January, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if an item list tag exists in section
//o------------------------------------------------------------------------------------------------o
auto CScriptSection::ItemListExist() const -> bool
{
	return itemList;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CScriptSection::NpcListExist()
//|	Date		-	12 January, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if an npc list tag exists in section
//o------------------------------------------------------------------------------------------------o
auto CScriptSection::NpcListExist() const -> bool
{
	return npcList;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CScriptSection::ItemListData()
//|	Date		-	12 January, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the itemlist data
//o------------------------------------------------------------------------------------------------o
auto CScriptSection::ItemListData() const -> std::string
{
	return itemListData;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CScriptSection::NpcListData()
//|	Date		-	12 January, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the npclist data
//o------------------------------------------------------------------------------------------------o
auto CScriptSection::NpcListData() const -> std::string
{
	return npcListData;
}

UI32 groupHolder = 0;
UI32 itemIndexHolder = 0;
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CScriptSection::CreateSection()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates section data from the input stream passed in
//o------------------------------------------------------------------------------------------------o
auto CScriptSection::CreateSection( std::istream& input ) -> void
{
	char line[2048];
	std::string sLine;
	SectData_st *toAdd		= nullptr;
	SectDataV2_st *toAdd2	= nullptr;
	DFNTAGS mTag;
	std::string tag, value, localName;
	// Now the reverse comes into play!
	while( !input.eof() && sLine.substr( 0, 1 ) != "}" && !input.fail() )
	{
		input.getline( line, 2047 );
		line[input.gcount()] = 0;

		sLine = line;
		sLine = oldstrutil::trim( oldstrutil::removeTrailing( sLine, "//" ));
		if( sLine != "}" && !sLine.empty() )
		{
			// do something here
			if( sLine.substr( 0, 1 ) != "}" )
			{
				auto secs = oldstrutil::sections( sLine, "=" );
				tag = "";
				if( secs.size() >= 1 )
				{
					try
					{
						tag = oldstrutil::trim( oldstrutil::removeTrailing( secs[0], "//" ));
					}
					catch (...)
					{
						tag = "";
					}
				}
				auto utag = oldstrutil::upper( tag );
				value = "";
				if( secs.size() >= 2 )
				{
					try
					{
						value = oldstrutil::trim( oldstrutil::removeTrailing( secs[1], "//" ));
					}
					catch (...)
					{
						value = "";
					}
				}
				switch( dfnCat )
				{
					case advance_def:
					case hard_items_def:	// as it's the same format as items_def, in essence
					case npc_def:
					case items_def:
					{
						mTag = FindDFNTagFromStr( tag );
						if( mTag != DFNTAG_COUNTOFTAGS	&& mTag != DFNTAG_ITEMLIST && mTag != DFNTAG_NPCLIST ) // we have a validly recognized tag
						{
							if( dfnDataTypes[mTag] != DFN_NODATA && value.empty() )	// it's a valid tag, needs data though!
							{
								break;
							}
							toAdd2 = new SectDataV2_st;
							toAdd2->tag = mTag;

							switch( dfnDataTypes[mTag] )
							{
								case DFN_UPPERSTRING:
								{
									value = oldstrutil::upper( value );
									if( utag == "ADDMENUITEM" )
									{
										// Handler for the new AUTO-Addmenu stuff. Each item that contains this tag is added to the list, and assigned to the correct menuitem group
										// Format: ADDMENUITEM=GroupID,TileID,WeightPosition,ObjectFlags,ObjectID
										ADDMENUITEM amiLocalCopy = {};
										amiLocalCopy.itemName = std::string( localName );
										auto csecs = oldstrutil::sections( value, "," );
										amiLocalCopy.groupId = static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 ));
										if( amiLocalCopy.groupId != groupHolder )
										{
											groupHolder = amiLocalCopy.groupId;
											itemIndexHolder = 0;
										}
										else
										{
											itemIndexHolder += 1;
										}
										amiLocalCopy.itemIndex = itemIndexHolder;
										amiLocalCopy.tileId = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 ));
										amiLocalCopy.weightPosition = static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[2], "//" )), nullptr, 0 ));
										amiLocalCopy.objectFlags = static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[3], "//" )), nullptr, 0 ));
										amiLocalCopy.weightPosition = static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[4], "//" )), nullptr, 0 ));

										//if( amiLocalCopy.tileId == INVALIDSERIAL )
											//amiLocalCopy.tileId = amiLocalCopy.objectId;
										// Need to shove it into the multimap
										g_mmapAddMenuMap.insert( std::make_pair( amiLocalCopy.groupId, amiLocalCopy ));
									}
									toAdd2->cdata = value;
									break;
								}
								case DFN_STRING:
									if( utag == "NAME" )
									{
										localName = value;
									}
									toAdd2->cdata = value;
									break;
								case DFN_NUMERIC:
									try
									{
										toAdd2->ndata = std::stoi( value, nullptr, 0 );
									}
									catch (...)
									{
										toAdd2->ndata = 0;
										Console.Warning( oldstrutil::format( "Invalid data (%s) found for %s tag in advance/harditems/item or character DFNs", value.c_str(), utag.c_str() ));
									}
									break;
								case DFN_DOUBLENUMERIC:
								{
									// Best I can tell the seperator here is a space
									value = oldstrutil::simplify( value );
									auto ssecs = oldstrutil::sections( value, " " );
									if( ssecs.size() >= 2 )
									{
										try
										{
											toAdd2->ndata = std::stoi( ssecs[0], nullptr, 0 );
										}
										catch (...)
										{
											toAdd2->ndata = 0;
										}
										try
										{
											toAdd2->odata = std::stoi( ssecs[1], nullptr, 0 );
										}
										catch (...)
										{
											toAdd2->odata = 0;
										}
									}
									else
									{
										try
										{
											toAdd2->ndata = std::stoi( value, nullptr, 0 );
										}
										catch (...)
										{
											toAdd2->ndata = 0;
										}
										toAdd2->odata = toAdd2->ndata;
									}
									break;
								}
								case DFN_NODATA:
								case DFN_UNKNOWN:
									toAdd2->cdata = "";
									break;
							}
							dataV2.push_back( toAdd2 );
						}
						else
						{
							toAdd		= new SectData_st;
							toAdd->tag	= tag;
							toAdd->data = value;
							data.push_back( toAdd );
							if( dfnCat == items_def )
							{
								if( mTag == DFNTAG_ITEMLIST )
								{
									itemList = true;
									itemListData = value;
								}
							}
							else if( dfnCat == npc_def )
							{
								if( mTag == DFNTAG_NPCLIST )
								{
									npcList = true;
									npcListData = value;
								}
							}
						}
						break;
					}
					case spawn_def:
					case create_def:
					case command_def:
						tag = utag;
						[[fallthrough]]; // Indicate to compiler that fallthrough is intentional
					default:
						toAdd = new SectData_st;
						toAdd->tag	= tag;
						toAdd->data	= value;
						data.push_back( toAdd );
						break;
				}
			}
		}
	}
	// Now some cleanup
	if( data.empty() && dataV2.empty() )
	{
		currentPos = data.end();
	}
}
