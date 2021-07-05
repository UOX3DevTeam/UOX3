//o-----------------------------------------------------------------------------------------------o
//| ssection.cpp
//o-----------------------------------------------------------------------------------------------o
//| Script Section class implementation
//o-----------------------------------------------------------------------------------------------o

#include "uox3.h"
#include "ssection.h"

const UI08 DFN_STRING		= 0;
const UI08 DFN_NUMERIC		= 1;
const UI08 DFN_UPPERSTRING	= 2;
const UI08 DFN_NODATA		= 3;
const UI08 DFN_UNKNOWN		= 4;
const UI08 DFN_DOUBLENUMERIC= 5;

// 04302004 - Added DFN_UPPERSTRING to support the new DFNTAG_ADDMENUITEM tag
const UI08 dfnDataTypes[DFNTAG_COUNTOFTAGS] =
{
	DFN_NUMERIC,		//	DFNTAG_AC = 0,
	DFN_UPPERSTRING,	//	DFNTAG_ADDMENUITEM,
	DFN_NUMERIC,		//	DFNTAG_ADVOBJ,
	DFN_DOUBLENUMERIC,	//	DFNTAG_ALCHEMY,
	DFN_STRING,			//	DFNTAG_AMMO,
	DFN_STRING,			//	DFNTAG_AMMOFX,
	DFN_NUMERIC,		//	DFNTAG_AMOUNT,
	DFN_DOUBLENUMERIC,	//	DFNTAG_ANATOMY,
	DFN_DOUBLENUMERIC,	//	DFNTAG_ANIMALLORE,
	DFN_DOUBLENUMERIC,	//	DFNTAG_ARCHERY,
	DFN_DOUBLENUMERIC,	//	DFNTAG_ARMSLORE,
	DFN_DOUBLENUMERIC,	//	DFNTAG_ATT,
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
	DFN_DOUBLENUMERIC,	//	DFNTAG_COOKING,
	DFN_NUMERIC,		//	DFNTAG_CORPSE,
	DFN_NUMERIC,		//	DFNTAG_CREATOR,
	DFN_STRING,			//	DFNTAG_CUSTOMSTRINGTAG,
	DFN_STRING,			//	DFNTAG_CUSTOMINTTAG,
	DFN_DOUBLENUMERIC,	//	DFNTAG_DAMAGE,
	DFN_NUMERIC,		//	DFNTAG_DAMAGEABLE,
	DFN_NUMERIC,		//	DFNTAG_DECAY,
	DFN_DOUBLENUMERIC,	//	DFNTAG_DEF,
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
	DFN_NUMERIC,		//	DFNTAG_EMOTECOLOUR,
	DFN_DOUBLENUMERIC,	//	DFNTAG_ENTICEMENT,
	DFN_UPPERSTRING,	//	DFNTAG_EQUIPITEM,
	DFN_DOUBLENUMERIC,	//	DFNTAG_EVALUATINGINTEL,
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
	DFN_DOUBLENUMERIC,	//	DFNTAG_HP,
	DFN_DOUBLENUMERIC,	//	DFNTAG_HPMAX,
	DFN_NUMERIC,		//	DFNTAG_ID,
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
	DFN_DOUBLENUMERIC,	//	DFNTAG_LUMBERJACKING,
	DFN_DOUBLENUMERIC,	//	DFNTAG_MACEFIGHTING,
	DFN_DOUBLENUMERIC,	//	DFNTAG_MAGERY,
	DFN_DOUBLENUMERIC,	//	DFNTAG_MAGICRESISTANCE,
	DFN_DOUBLENUMERIC,	//	DFNTAG_MANA,
	DFN_DOUBLENUMERIC,	//	DFNTAG_MANAMAX,
	DFN_NUMERIC,		//	DFNTAG_MAXHP,
	DFN_NUMERIC,		//	DFNTAG_MAXITEMS,
	DFN_NUMERIC,		//	DFNTAG_MAXRANGE,
	DFN_DOUBLENUMERIC,	//	DFNTAG_MEDITATION,
	DFN_DOUBLENUMERIC,	//	DFNTAG_MINING,
	DFN_NUMERIC,		//	DFNTAG_MOVABLE,
	DFN_STRING,			//	DFNTAG_MORE,
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
	DFN_NODATA,			//	DFNTAG_NOTRAIN,
	DFN_NUMERIC,		//	DFNTAG_NPCAI,
	DFN_UPPERSTRING,	//	DFNTAG_NPCLIST,
	DFN_NUMERIC,		//	DFNTAG_NPCWANDER,
	DFN_NUMERIC,		//	DFNTAG_OFFSPELL,
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
	DFN_DOUBLENUMERIC,	//	DFNTAG_REMOVETRAPS,
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
	DFN_DOUBLENUMERIC,	//	DFNTAG_SPELLWEAVING,
	DFN_DOUBLENUMERIC,	//	DFNTAG_SPIRITSPEAK,
	DFN_NUMERIC,		//	DFNTAG_SPLIT,
	DFN_NUMERIC,		//	DFNTAG_SPLITCHANCE,
	DFN_DOUBLENUMERIC,	//	DFNTAG_STAMINA,
	DFN_DOUBLENUMERIC,	//	DFNTAG_STAMINAMAX,
	DFN_DOUBLENUMERIC,	//	DFNTAG_STRENGTH,
	DFN_NUMERIC,		//	DFNTAG_STRADD,
	DFN_DOUBLENUMERIC,	//	DFNTAG_STEALING,
	DFN_DOUBLENUMERIC,	//	DFNTAG_STEALTH,
	DFN_DOUBLENUMERIC,	//	DFNTAG_SWORDSMANSHIP,
	DFN_DOUBLENUMERIC,	//	DFNTAG_TACTICS,
	DFN_DOUBLENUMERIC,	//	DFNTAG_TAILORING,
	DFN_DOUBLENUMERIC,	//	DFNTAG_TAMING,
	DFN_DOUBLENUMERIC,	//	DFNTAG_TAMEDHUNGER,
	DFN_DOUBLENUMERIC,	//	DFNTAG_TASTEID,
	DFN_DOUBLENUMERIC,	//	DFNTAG_THROWING,
	DFN_DOUBLENUMERIC,	//	DFNTAG_TINKERING,
	DFN_STRING,			//	DFNTAG_TITLE,
	DFN_NUMERIC,		//	DFNTAG_TOTAME,
	DFN_NUMERIC,		//	DFNTAG_TOPROV,
	DFN_DOUBLENUMERIC,	//	DFNTAG_TOPEACE,
	DFN_DOUBLENUMERIC,	//	DFNTAG_TRACKING,
	DFN_NUMERIC,		//	DFNTAG_TYPE,
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

struct strToDFNLookup
{
	std::string strToAdd;
	DFNTAGS	dfnToAdd;
};

std::map< std::string, DFNTAGS > strToDFNTag;

void InitStrToDFN( void )
{
	// VC6 doesn't like assigning "text" to std::string in a table like fashion
	strToDFNTag["AC"]				=	DFNTAG_AC;
	strToDFNTag["ADDMENUITEM"]		=	DFNTAG_ADDMENUITEM;
	strToDFNTag["ADVOBJ"]			=	DFNTAG_ADVOBJ;
	strToDFNTag["ALCHEMY"]			=	DFNTAG_ALCHEMY;
	strToDFNTag["AMMO"]				=	DFNTAG_AMMO;
	strToDFNTag["AMMOFX"]			=	DFNTAG_AMMOFX;
	strToDFNTag["AMOUNT"]			=	DFNTAG_AMOUNT;
	strToDFNTag["ANATOMY"]			=	DFNTAG_ANATOMY;
	strToDFNTag["ANIMALLORE"]		=	DFNTAG_ANIMALLORE;
	strToDFNTag["ARCHERY"]			=	DFNTAG_ARCHERY;
	strToDFNTag["ARMSLORE"]			=	DFNTAG_ARMSLORE;
	strToDFNTag["ATT"]				=	DFNTAG_ATT;
	strToDFNTag["BACKPACK"]			=	DFNTAG_BACKPACK;
	strToDFNTag["BASERANGE"]		=	DFNTAG_BASERANGE;
	strToDFNTag["BEGGING"]			=	DFNTAG_BEGGING;
	strToDFNTag["BLACKSMITHING"]	=	DFNTAG_BLACKSMITHING;
	strToDFNTag["BOWCRAFT"]			=	DFNTAG_BOWCRAFT;
	strToDFNTag["BUSHIDO"]			=	DFNTAG_BUSHIDO;
	strToDFNTag["CAMPING"]			=	DFNTAG_CAMPING;
	strToDFNTag["CARPENTRY"]		=	DFNTAG_CARPENTRY;
	strToDFNTag["CARTOGRAPHY"]		=	DFNTAG_CARTOGRAPHY;
	strToDFNTag["CARVE"]			=	DFNTAG_CARVE;
	strToDFNTag["CHIVALRY"]			=	DFNTAG_CHIVALRY;
	strToDFNTag["COLD"]				=	DFNTAG_COLD;
	strToDFNTag["COLOR"]			=	DFNTAG_COLOUR;
	strToDFNTag["COLOUR"]			=	DFNTAG_COLOUR;
	strToDFNTag["COLORLIST"]		=	DFNTAG_COLOURLIST;
	strToDFNTag["COLOURLIST"]		=	DFNTAG_COLOURLIST;
	strToDFNTag["COLORMATCHHAIR"]	=	DFNTAG_COLOURMATCHHAIR;
	strToDFNTag["COLOURMATCHHAIR"]	=	DFNTAG_COLOURMATCHHAIR;
	strToDFNTag["COOKING"]			=	DFNTAG_COOKING;
	strToDFNTag["CORPSE"]			=	DFNTAG_CORPSE;
	strToDFNTag["CREATOR"]			=	DFNTAG_CREATOR;
	strToDFNTag["CUSTOMSTRINGTAG"]	=	DFNTAG_CUSTOMSTRINGTAG;
	strToDFNTag["CUSTOMINTTAG"]		=	DFNTAG_CUSTOMINTTAG;
	strToDFNTag["DAMAGE"]			=	DFNTAG_DAMAGE;
	strToDFNTag["DAMAGEABLE"]		=	DFNTAG_DAMAGEABLE;
	strToDFNTag["DECAY"]			=	DFNTAG_DECAY;
	strToDFNTag["DEF"]				=	DFNTAG_DEF;
	strToDFNTag["DETECTINGHIDDEN"]	=	DFNTAG_DETECTINGHIDDEN;
	strToDFNTag["DEX"]				=	DFNTAG_DEX;
	strToDFNTag["DEXTERITY"]		=	DFNTAG_DEX;
	strToDFNTag["DX"]				=	DFNTAG_DEX;
	strToDFNTag["DEXADD"]			=	DFNTAG_DEXADD;
	strToDFNTag["DX2"]				=	DFNTAG_DEXADD;
	strToDFNTag["DIR"]				=	DFNTAG_DIR;
	strToDFNTag["DIRECTION"]		=	DFNTAG_DIR;
	strToDFNTag["DISPELLABLE"]		=	DFNTAG_DISPELLABLE;
	strToDFNTag["DISABLED"]			=	DFNTAG_DISABLED;
	strToDFNTag["DOORFLAG"]			=	DFNTAG_DOORFLAG;
	strToDFNTag["DYE"]				=	DFNTAG_DYE;
	strToDFNTag["DYEABLE"]			=	DFNTAG_DYE;
	strToDFNTag["DYEHAIR"]			=	DFNTAG_DYEHAIR;
	strToDFNTag["DYEBEARD"]			=	DFNTAG_DYEBEARD;
	strToDFNTag["ELEMENTRESIST"]	=	DFNTAG_ELEMENTRESIST;
	strToDFNTag["EMOTECOLOR"]		=	DFNTAG_EMOTECOLOUR;
	strToDFNTag["EMOTECOLOUR"]		=	DFNTAG_EMOTECOLOUR;
	strToDFNTag["ENTICEMENT"]		=	DFNTAG_ENTICEMENT;
	strToDFNTag["EVALUATINGINTEL"]	=	DFNTAG_EVALUATINGINTEL;
	strToDFNTag["FAME"]				=	DFNTAG_FAME;
	strToDFNTag["FENCING"]			=	DFNTAG_FENCING;
	strToDFNTag["FISHING"]			=	DFNTAG_FISHING;
	strToDFNTag["FLAG"]				=	DFNTAG_FLAG;
	strToDFNTag["FLEEAT"]			=	DFNTAG_FLEEAT;
	strToDFNTag["FLEEINGSPEED"]		=	DFNTAG_FLEEINGSPEED;
	strToDFNTag["FLEEINGSPEEDMOUNTED"]		=	DFNTAG_FLEEINGSPEEDMOUNTED;
	strToDFNTag["FOCUS"]			=	DFNTAG_FOCUS;
	strToDFNTag["FORENSICS"]		=	DFNTAG_FORENSICS;
	strToDFNTag["FX1"]				=	DFNTAG_FX1;
	strToDFNTag["FX2"]				=	DFNTAG_FX2;
	strToDFNTag["FY1"]				=	DFNTAG_FY1;
	strToDFNTag["FY2"]				=	DFNTAG_FY2;
	strToDFNTag["FZ1"]				=	DFNTAG_FZ1;
	strToDFNTag["FOOD"]				=	DFNTAG_FOOD;
	strToDFNTag["GET"]				=	DFNTAG_GET;
	strToDFNTag["GLOW"]				=	DFNTAG_GLOW;
	strToDFNTag["GLOWBC"]			=	DFNTAG_GLOWBC;
	strToDFNTag["GLOWTYPE"]			=	DFNTAG_GLOWTYPE;
	strToDFNTag["GOLD"]				=	DFNTAG_GOLD;
	strToDFNTag["GOOD"]				=	DFNTAG_GOOD;
	strToDFNTag["HAIRCOLOR"]		=	DFNTAG_HAIRCOLOUR;
	strToDFNTag["HAIRCOLOUR"]		=	DFNTAG_HAIRCOLOUR;
	strToDFNTag["HEALING"]			=	DFNTAG_HEALING;
	strToDFNTag["HEAT"]				=	DFNTAG_HEAT;
	strToDFNTag["HERDING"]			=	DFNTAG_HERDING;
	strToDFNTag["HIDAMAGE"]			=	DFNTAG_HIDAMAGE;
	strToDFNTag["HIDING"]			=	DFNTAG_HIDING;
	strToDFNTag["HP"]				=	DFNTAG_HP;
	strToDFNTag["HPMAX"]			=	DFNTAG_HPMAX;
	strToDFNTag["ID"]				=	DFNTAG_ID;
	strToDFNTag["IMBUING"]			=	DFNTAG_IMBUING;
	strToDFNTag["IN"]				=	DFNTAG_INTELLIGENCE;
	strToDFNTag["INTELLIGENCE"]		=	DFNTAG_INTELLIGENCE;
	strToDFNTag["INT"]				=	DFNTAG_INTELLIGENCE;
	strToDFNTag["IN2"]				=	DFNTAG_INTADD;
	strToDFNTag["INTADD"]			=	DFNTAG_INTADD;
	strToDFNTag["INTERVAL"]			=	DFNTAG_INTERVAL;
	strToDFNTag["INSCRIPTION"]		=	DFNTAG_INSCRIPTION;
	strToDFNTag["EQUIPITEM"]		=	DFNTAG_EQUIPITEM;
	strToDFNTag["ITEMLIST"]			=	DFNTAG_ITEMLIST;
	strToDFNTag["ITEMID"]			=	DFNTAG_ITEMID;
	strToDFNTag["KARMA"]			=	DFNTAG_KARMA;
	strToDFNTag["KILLHAIR"]			=	DFNTAG_KILLHAIR;
	strToDFNTag["KILLBEARD"]		=	DFNTAG_KILLBEARD;
	strToDFNTag["KILLPACK"]			=	DFNTAG_KILLPACK;
	strToDFNTag["LAYER"]			=	DFNTAG_LAYER;
	strToDFNTag["LIGHT"]			=	DFNTAG_LIGHT;
	strToDFNTag["LIGHTNING"]		=	DFNTAG_LIGHTNING;
	strToDFNTag["LOCKPICKING"]		=	DFNTAG_LOCKPICKING;
	strToDFNTag["LODAMAGE"]			=	DFNTAG_LODAMAGE;
	strToDFNTag["LOOT"]				=	DFNTAG_LOOT;
	strToDFNTag["LUMBERJACKING"]	=	DFNTAG_LUMBERJACKING;
	strToDFNTag["MACEFIGHTING"]		=	DFNTAG_MACEFIGHTING;
	strToDFNTag["MAGERY"]			=	DFNTAG_MAGERY;
	strToDFNTag["MAGICRESISTANCE"]	=	DFNTAG_MAGICRESISTANCE;
	strToDFNTag["MANA"]				=	DFNTAG_MANA;
	strToDFNTag["MANAMAX"]			=	DFNTAG_MANAMAX;
	strToDFNTag["MAXRANGE"]			=	DFNTAG_MAXRANGE;
	strToDFNTag["MAXHP"]			=	DFNTAG_MAXHP;
	strToDFNTag["MAXITEMS"]			=	DFNTAG_MAXITEMS;
	strToDFNTag["MEDITATION"]		=	DFNTAG_MEDITATION;
	strToDFNTag["MINING"]			=	DFNTAG_MINING;
	strToDFNTag["MOVABLE"]			=	DFNTAG_MOVABLE;
	strToDFNTag["MORE"]				=	DFNTAG_MORE;
	strToDFNTag["MORE2"]			=	DFNTAG_MORE2;
	strToDFNTag["MOREX"]			=	DFNTAG_MOREX;
	strToDFNTag["MOREY"]			=	DFNTAG_MOREY;
	strToDFNTag["MOREZ"]			=	DFNTAG_MOREZ;
	strToDFNTag["MUSICIANSHIP"]		=	DFNTAG_MUSICIANSHIP;
	strToDFNTag["MYSTICISM"]		=	DFNTAG_MYSTICISM;
	strToDFNTag["NAME"]				=	DFNTAG_NAME;
	strToDFNTag["NAME2"]			=	DFNTAG_NAME2;
	strToDFNTag["NAMELIST"]			=	DFNTAG_NAMELIST;
	strToDFNTag["NECROMANCY"]		=	DFNTAG_NECROMANCY;
	strToDFNTag["NEWBIE"]			=	DFNTAG_NEWBIE;
	strToDFNTag["NINJITSU"]			=	DFNTAG_NINJITSU;
	strToDFNTag["NOTRAIN"]			=	DFNTAG_NOTRAIN;
	strToDFNTag["NPCAI"]			=	DFNTAG_NPCAI;
	strToDFNTag["NPCLIST"]			=	DFNTAG_NPCLIST;
	strToDFNTag["NPCWANDER"]		=	DFNTAG_NPCWANDER;
	strToDFNTag["OFFSPELL"]			=	DFNTAG_OFFSPELL;
	strToDFNTag["PACKITEM"]			=	DFNTAG_PACKITEM;
	strToDFNTag["PARRYING"]			=	DFNTAG_PARRYING;
	strToDFNTag["PEACEMAKING"]		=	DFNTAG_PEACEMAKING;
	strToDFNTag["PILEABLE"]			=	DFNTAG_PILEABLE;
	strToDFNTag["POLY"]				=	DFNTAG_POLY;
	strToDFNTag["POISONDAMAGE"]		=	DFNTAG_POISONDAMAGE;
	strToDFNTag["POISONSTRENGTH"]	=	DFNTAG_POISONSTRENGTH;
	strToDFNTag["POISONED"]			=	DFNTAG_POISONED;
	strToDFNTag["POISONING"]		=	DFNTAG_POISONING;
	strToDFNTag["PRIV"]				=	DFNTAG_PRIV;
	strToDFNTag["PROVOCATION"]		=	DFNTAG_PROVOCATION;
	strToDFNTag["RACE"]				=	DFNTAG_RACE;
	strToDFNTag["RAIN"]				=	DFNTAG_RAIN;
	strToDFNTag["RANK"]				=	DFNTAG_RANK;
	strToDFNTag["REATTACKAT"]		=	DFNTAG_REATTACKAT;
	strToDFNTag["RESISTFIRE"]		=	DFNTAG_RESISTFIRE;
	strToDFNTag["RESISTCOLD"]		=	DFNTAG_RESISTCOLD;
	strToDFNTag["RESISTLIGHTNING"]	=	DFNTAG_RESISTLIGHTNING;
	strToDFNTag["RESISTPOISON"]		=	DFNTAG_RESISTPOISON;
	strToDFNTag["REMOVETRAPS"]		=	DFNTAG_REMOVETRAPS;
	strToDFNTag["RESTOCK"]			=	DFNTAG_RESTOCK;
	strToDFNTag["RSHOPITEM"]		=	DFNTAG_RSHOPITEM;
	strToDFNTag["RUNS"]				=	DFNTAG_RUNS;
	strToDFNTag["RUNNINGSPEED"]		=	DFNTAG_RUNNINGSPEED;
	strToDFNTag["RUNNINGSPEEDMOUNTED"]		=	DFNTAG_RUNNINGSPEEDMOUNTED;
	strToDFNTag["SAYCOLOR"]			=	DFNTAG_SAYCOLOUR;
	strToDFNTag["SAYCOLOUR"]		=	DFNTAG_SAYCOLOUR;
	strToDFNTag["SCRIPT"]			=	DFNTAG_SCRIPT;
	strToDFNTag["SELLITEM"]			=	DFNTAG_SELLITEM;
	strToDFNTag["SHOPITEM"]			=	DFNTAG_SHOPITEM;
	strToDFNTag["SHOPKEEPER"]		=	DFNTAG_SHOPKEEPER;
	strToDFNTag["SHOPLIST"]			=	DFNTAG_SHOPLIST;
	strToDFNTag["SK_MADE"]			=	DFNTAG_SK_MADE;
	strToDFNTag["SKILL"]			=	DFNTAG_SKILL;
	strToDFNTag["SKIN"]				=	DFNTAG_SKIN;
	strToDFNTag["SKINLIST"]			=	DFNTAG_SKINLIST;
	strToDFNTag["SNOOPING"]			=	DFNTAG_SNOOPING;
	strToDFNTag["SNOW"]				=	DFNTAG_SNOW;
	strToDFNTag["SPADELAY"]			=	DFNTAG_SPADELAY;
	strToDFNTag["SPATTACK"]			=	DFNTAG_SPATTACK;
	strToDFNTag["SPAWNOBJ"]			=	DFNTAG_SPAWNOBJ;
	strToDFNTag["SPAWNOBJLIST"]		=	DFNTAG_SPAWNOBJLIST;
	strToDFNTag["SPD"]				=	DFNTAG_SPD;
	strToDFNTag["SPEED"]			=	DFNTAG_SPD;
	strToDFNTag["SPELLWEAVING"]		=	DFNTAG_SPELLWEAVING;
	strToDFNTag["SPIRITSPEAK"]		=	DFNTAG_SPIRITSPEAK;
	strToDFNTag["SPLIT"]			=	DFNTAG_SPLIT;
	strToDFNTag["SPLITCHANCE"]		=	DFNTAG_SPLITCHANCE;
	strToDFNTag["ST"]				=	DFNTAG_STRENGTH;
	strToDFNTag["STAMINA"]			=	DFNTAG_STAMINA;
	strToDFNTag["STAMINAMAX"]		=	DFNTAG_STAMINAMAX;
	strToDFNTag["STR"]				=	DFNTAG_STRENGTH;
	strToDFNTag["STRENGTH"]			=	DFNTAG_STRENGTH;
	strToDFNTag["ST2"]				=	DFNTAG_STRADD;
	strToDFNTag["STRADD"]			=	DFNTAG_STRADD;
	strToDFNTag["STEALING"]			=	DFNTAG_STEALING;
	strToDFNTag["STEALTH"]			=	DFNTAG_STEALTH;
	strToDFNTag["SWORDSMANSHIP"]	=	DFNTAG_SWORDSMANSHIP;
	strToDFNTag["TACTICS"]			=	DFNTAG_TACTICS;
	strToDFNTag["TAILORING"]		=	DFNTAG_TAILORING;
	strToDFNTag["TAMING"]			=	DFNTAG_TAMING;
	strToDFNTag["TAMEDHUNGER"]		=	DFNTAG_TAMEDHUNGER;
	strToDFNTag["TASTEID"]			=	DFNTAG_TASTEID;
	strToDFNTag["THROWING"]			=	DFNTAG_THROWING;
	strToDFNTag["TINKERING"]		=	DFNTAG_TINKERING;
	strToDFNTag["TITLE"]			=	DFNTAG_TITLE;
	strToDFNTag["TOTAME"]			=	DFNTAG_TOTAME;
	strToDFNTag["TOPROV"]			=	DFNTAG_TOPROV;
	strToDFNTag["TOPEACE"]			=	DFNTAG_TOPEACE;
	strToDFNTag["TRACKING"]			=	DFNTAG_TRACKING;
	strToDFNTag["TYPE"]				=	DFNTAG_TYPE;
	strToDFNTag["VALUE"]			=	DFNTAG_VALUE;
	strToDFNTag["VETERINARY"]		=	DFNTAG_VETERINARY;
	strToDFNTag["VISIBLE"]			=	DFNTAG_VISIBLE;
	strToDFNTag["WALKINGSPEED"]		=	DFNTAG_WALKINGSPEED;
	strToDFNTag["WALKINGSPEEDMOUNTED"]		=	DFNTAG_WALKINGSPEEDMOUNTED;
	strToDFNTag["WEIGHT"]			=	DFNTAG_WEIGHT;
	strToDFNTag["WEIGHTMAX"]		=	DFNTAG_WEIGHTMAX;
	strToDFNTag["WILLHUNGER"]		=	DFNTAG_WILLHUNGER;
	strToDFNTag["WIPE"]				=	DFNTAG_WIPE;
	strToDFNTag["WRESTLING"]		=	DFNTAG_WRESTLING;
}
void CleanupStrToDFN( void )
{
	strToDFNTag.clear();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	DFNTAGS FindDFNTagFromStr( std::string strToFind )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-
//o-----------------------------------------------------------------------------------------------o
DFNTAGS FindDFNTagFromStr( std::string strToFind )
{
	if( strToDFNTag.empty() ) // if we haven't built our array yet
	{
		InitStrToDFN();
	}
	std::map< std::string, DFNTAGS >::const_iterator toFind = strToDFNTag.find( strutil::toupper( strToFind ) );
	if( toFind != strToDFNTag.end() )
	{
		return toFind->second;
	}
	return DFNTAG_COUNTOFTAGS;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	ScriptSection( void ) : dfnCat( NUM_DEFS ),
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Default constructor, initializing all variables
//o-----------------------------------------------------------------------------------------------o
ScriptSection::ScriptSection( void ) : dfnCat( NUM_DEFS ),
npcListData( "" ), itemListData( "" ), npcList( false ), itemList( false )
{
	data.resize( 0 );
	dataV2.resize( 0 );
	currentPos	= data.end();
	currentPos2	= dataV2.end();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	ScriptSection( std::fstream& input, DEFINITIONCATEGORIES d )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Default constructor, initializing all variables
//|						and grabbing a section from the file passed in
//o-----------------------------------------------------------------------------------------------o
ScriptSection::ScriptSection( std::fstream& input, DEFINITIONCATEGORIES d ) :
dfnCat( d ), npcList( false ), itemList( false ), npcListData( "" ), itemListData( "" )
{
	data.resize( 0 );
	dataV2.resize( 0 );
	createSection( input );
	currentPos	= data.end();
	currentPos2	= dataV2.end();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool AtEnd( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if at end of the section
//o-----------------------------------------------------------------------------------------------o
bool ScriptSection::AtEnd( void )
// PRE:		vector loaded and init'd
// POST:	returns true if at end of array now
{
	return ( currentPos == data.end() );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	const std::string First( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the first tag at the start of the section
//o-----------------------------------------------------------------------------------------------o
const std::string ScriptSection::First( void )
// PRE:		vector loaded and init'd
// POST:	returns string (tag) of first entry
{
	std::string rvalue;
	currentPos = data.begin();
	if( !AtEnd() )
	{
		rvalue = (*currentPos)->tag;
	}
	return rvalue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	const std::string Next( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the next tag in the section, or nullptr if no more
//o-----------------------------------------------------------------------------------------------o
const std::string ScriptSection::Next( void )
{
	std::string rvalue;
	if( !AtEnd() )
	{
		++currentPos;
		if( !AtEnd() )
			rvalue = (*currentPos)->tag;
	}
	return rvalue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	const std::string MoveTo( size_t position )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Moves to position in the section and returns the tag there
//o-----------------------------------------------------------------------------------------------o
const std::string ScriptSection::MoveTo( size_t position )
// PRE:		vector loaded and init'd
// POST:	returns string (tag) of next entry
{
	std::string rvalue;
	std::vector< sectData * >::iterator curPos	= currentPos;
	currentPos									= (data.begin() + position);
	if( !AtEnd() )
	{
		rvalue		= (*currentPos)->tag;
	}
	else
	{
		currentPos	= curPos;
	}
	return rvalue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	const std::string GrabData( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the data for the current tag
//o-----------------------------------------------------------------------------------------------o
const std::string ScriptSection::GrabData( void )
// PRE:		At a valid location, init'd data
// POST:	returns string of data of current entry
{
	std::string rvalue;
	if( !AtEnd() )
	{
		rvalue = (*currentPos)->data;
	}
	return rvalue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	~ScriptSection()
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Default deconstructor, removing any allocated memory
//|						and closing any files that may be open
//o-----------------------------------------------------------------------------------------------o
ScriptSection::~ScriptSection()
{
	if( !FlushData() )
		Console.error( "Section unable to flush data!" );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool FlushData( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Deletes all tag/data pairs and resizes array to 0
//o-----------------------------------------------------------------------------------------------o
bool ScriptSection::FlushData( void )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	size_t NumEntries( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the number of entries in the section
//o-----------------------------------------------------------------------------------------------o
size_t ScriptSection::NumEntries( void ) const
{
	return data.size();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	const std::string Prev( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the previous tag, or nullptr if at start
//o-----------------------------------------------------------------------------------------------o
const std::string ScriptSection::Prev( void )
{
	std::string rvalue;
	if( currentPos != data.begin() )
	{
		--currentPos;
		if( !AtEnd() )
		{
			rvalue = (*currentPos)->tag;
		}
	}
	return rvalue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void Remove( size_t position )
//|	Changes		-   (11-15-2001)
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes the tag/data pair at position in the array
//o-----------------------------------------------------------------------------------------------o
void ScriptSection::Remove( size_t position )
// PRE:		vector loaded and init'd
// POST:	removes thing at position
{
	if( AtEnd() )
		return;
	delete data[position];
	data.erase( data.begin() + position );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void Append( std::string tagToAdd, std::string dataToAdd )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a new tag/data pair at the end of the section
//o-----------------------------------------------------------------------------------------------o
void ScriptSection::Append( std::string tagToAdd, std::string dataToAdd )
{
	sectData *toAdd	= new sectData;
	toAdd->tag		= tagToAdd;
	toAdd->data		= dataToAdd;
	data.push_back( toAdd );
}

const std::string ScriptSection::GrabData( SI32& ndata, SI32& odata )
{
	std::string rvalue;
	if( AtEndTags() )
	{
		ndata = INVALIDSERIAL;
		odata = INVALIDSERIAL;
	}
	else
	{
		ndata	= (*currentPos2)->ndata;
		odata	= (*currentPos2)->odata;
		rvalue	= (*currentPos2)->cdata;
	}
	return rvalue;
}
bool ScriptSection::AtEndTags( void )
{
	if( currentPos2 == dataV2.end() )
		return true;
	return false;
}
DFNTAGS ScriptSection::PrevTag( void )
{
	DFNTAGS rValue = DFNTAG_COUNTOFTAGS;
	if( currentPos2 != dataV2.begin() )
	{
		--currentPos2;
		if( !AtEndTags() )
			rValue = (*currentPos2)->tag;
	}
	return rValue;
}
DFNTAGS ScriptSection::NextTag( void )
{
	DFNTAGS rValue = DFNTAG_COUNTOFTAGS;
	if( !AtEndTags() )
	{
		++currentPos2;
		if( !AtEndTags() )
			rValue = (*currentPos2)->tag;
	}
	return rValue;
}
DFNTAGS ScriptSection::FirstTag( void )
{
	currentPos2 = dataV2.begin();
	if( AtEndTags() )
		return DFNTAG_COUNTOFTAGS;
	return (*currentPos2)->tag;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ItemListExist( void ) const
//|	Date		-	12 January, 2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if an item list tag exists in section
//o-----------------------------------------------------------------------------------------------o
bool ScriptSection::ItemListExist( void ) const
{
	return itemList;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool NpcListExist( void ) const
//|	Date		-	12 January, 2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if an npc list tag exists in section
//o-----------------------------------------------------------------------------------------------o
bool ScriptSection::NpcListExist( void ) const
{
	return npcList;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	const std::string ItemListData( void ) const
//|	Date		-	12 January, 2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the itemlist data
//o-----------------------------------------------------------------------------------------------o
const std::string ScriptSection::ItemListData( void ) const
{
	return itemListData;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	const std::string NpcListData( void ) const
//|	Date		-	12 January, 2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the npclist data
//o-----------------------------------------------------------------------------------------------o
const std::string ScriptSection::NpcListData( void ) const
{
	return npcListData;
}

UI32 groupHolder = 0;
UI32 itemIndexHolder = 0;
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void createSection( std::fstream& input )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates section data from the input stream passed in
//o-----------------------------------------------------------------------------------------------o
void ScriptSection::createSection( std::fstream& input )
{
	char line[2048];
	std::string sLine;
	sectData *toAdd		= nullptr;
	sectDataV2 *toAdd2	= nullptr;
	DFNTAGS mTag;
	std::string tag, value, localName;
	// Now the reverse comes into play!
	while( !input.eof() && sLine.substr( 0, 1 ) != "}" && !input.fail() )
	{
		input.getline(line, 2047);
		line[input.gcount()] = 0;

		sLine = line;
		sLine = strutil::stripTrim( sLine );
		if( sLine != "}" && !sLine.empty() )
		{
			// do something here
			if( sLine.substr( 0, 1 ) != "}" )
			{
				auto secs = strutil::sections( sLine, "=" );
				tag = "";
				if( secs.size() >= 1 )
				{
					try {
						tag = strutil::stripTrim( secs[0] );
					}
					catch (...)	{
						tag = "";
					}
				}
				auto utag = strutil::toupper( tag );
				value = "";
				if( secs.size() >= 2 )
				{
					try {
						value = strutil::stripTrim( secs[1] );
					}
					catch (...) {
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
						if( mTag != DFNTAG_COUNTOFTAGS	&& mTag != DFNTAG_ITEMLIST && mTag != DFNTAG_NPCLIST )	// we have a validly recognized tag
						{
							if( dfnDataTypes[mTag] != DFN_NODATA && value.empty() )	// it's a valid tag, needs data though!
								break;
							toAdd2 = new sectDataV2;
							toAdd2->tag = mTag;

							switch( dfnDataTypes[mTag] )
							{
								case DFN_UPPERSTRING:
								{
									value = strutil::toupper( value );
									if( utag == "ADDMENUITEM" )
									{
										// Handler for the new AUTO-Addmenu stuff. Each item that contains this tag is added to the list, and assigned to the correct menuitem group
										// Format: ADDMENUITEM=GroupID,TileID,WeightPosition,ObjectFlags,ObjectID
										ADDMENUITEM amiLocalCopy;
										memset(&amiLocalCopy,0x00,sizeof(ADDMENUITEM));
										amiLocalCopy.itemName = std::string( localName );
										auto csecs = strutil::sections( value, "," );
										amiLocalCopy.groupID = static_cast<UI32>(std::stoul(strutil::stripTrim( csecs[0] ), nullptr, 0));
										if( amiLocalCopy.groupID != groupHolder )
										{
											groupHolder = amiLocalCopy.groupID;
											itemIndexHolder = 0;
										}
										else
										{
											itemIndexHolder += 1;
										}
										amiLocalCopy.itemIndex = itemIndexHolder;
										amiLocalCopy.tileID = static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[1] ), nullptr, 0));
										amiLocalCopy.weightPosition = static_cast<UI32>(std::stoul(strutil::stripTrim( csecs[2] ), nullptr, 0));
										amiLocalCopy.objectFlags = static_cast<UI32>(std::stoul(strutil::stripTrim( csecs[3] ), nullptr, 0));
										amiLocalCopy.weightPosition = static_cast<UI32>(std::stoul(strutil::stripTrim( csecs[4] ), nullptr, 0));

										//if( amiLocalCopy.tileID == INVALIDSERIAL )
											//amiLocalCopy.tileID = amiLocalCopy.objectID;
										// Need to shove it into the multimap
										g_mmapAddMenuMap.insert(std::make_pair(amiLocalCopy.groupID,amiLocalCopy));
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
									try {
										toAdd2->ndata = std::stoi( value, nullptr, 0 );
									}
									catch (...) {
										toAdd2->ndata = 0;
										Console.warning( strutil::format( "Invalid data (%s) found for %s tag in advance/harditems/item or character DFNs", value.c_str(), utag.c_str() ));
									}
									break;
								case DFN_DOUBLENUMERIC:
								{
									// Best I can tell the seperator here is a space
									value = strutil::simplify( value );
									auto ssecs = strutil::sections( value, " " );
									if( ssecs.size() >= 2 )
									{
										try {
											toAdd2->ndata = std::stoi( ssecs[0], nullptr, 0 );
										}
										catch (...) {
											toAdd2->ndata = 0;
										}
										try {
											toAdd2->odata = std::stoi( ssecs[1], nullptr, 0 );
										}
										catch (...) {
											toAdd2->odata = 0;
										}
									}
									else
									{
										try {
											toAdd2->ndata = std::stoi( value, nullptr, 0 );
										}
										catch (...) {
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
							toAdd		= new sectData;
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
						toAdd = new sectData;
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
		currentPos = data.end();
}
