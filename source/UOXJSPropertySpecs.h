//o--------------------------------------------------------------------------o
//|	File			-	UOXJSPropertyFuncts.cpp
//|	Date			-	12/14/2001
//|	Developers		-	Abaddon / EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	1.0		Abaddon		14th December, 2001 Initial implementation
//|									Defines the property specifications for race, region, guild,
//|									item and chars
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
#ifndef __UOXJSPropertySpecs__
#define __UOXJSPropertySpecs__

#include "UOXJSPropertyEnums.h"
#include "enums.h"

namespace UOX
{

static JSPropertySpec CSpellProperties[] = 
{
	{ "id",					CSP_ID,					JSPROP_ENUMANDPERM },
	{ "action",				CSP_ACTION,				JSPROP_ENUMANDPERM },
	{ "delay",				CSP_DELAY,				JSPROP_ENUMANDPERM },
	{ "health",				CSP_HEALTH,				JSPROP_ENUMANDPERM },
	{ "stamina",			CSP_STAMINA,			JSPROP_ENUMANDPERM },
	{ "mana",				CSP_MANA,				JSPROP_ENUMANDPERM },
	{ "mantra",				CSP_MANTRA,				JSPROP_ENUMANDPERM },
	{ "strToSay",			CSP_STRTOSAY,			JSPROP_ENUMANDPERM },
	{ "scrollLow",			CSP_SCROLLLOW,			JSPROP_ENUMANDPERM },
	{ "scrollHigh",			CSP_SCROLLHIGH,			JSPROP_ENUMANDPERM },
	{ "circle",				CSP_CIRCLE,				JSPROP_ENUMANDPERM },
	{ "lowSkill",			CSP_LOWSKILL,			JSPROP_ENUMANDPERM },
	{ "highSkill",			CSP_HIGHSKILL,			JSPROP_ENUMANDPERM },
	{ "ginseng",			CSP_GINSENG,			JSPROP_ENUMANDPERM },
	{ "moss",				CSP_MOSS,				JSPROP_ENUMANDPERM },
	{ "drake",				CSP_DRAKE,				JSPROP_ENUMANDPERM },
	{ "pearl",				CSP_PEARL,				JSPROP_ENUMANDPERM },
	{ "silk",				CSP_SILK,				JSPROP_ENUMANDPERM },
	{ "ash",				CSP_ASH,				JSPROP_ENUMANDPERM },
	{ "shade",				CSP_SHADE,				JSPROP_ENUMANDPERM },
	{ "garlic",				CSP_GARLIC,				JSPROP_ENUMANDPERM },
	{ "requireTarget",		CSP_REQUIRETARGET,		JSPROP_ENUMANDPERM },
	{ "requireItem",		CSP_REQUIREITEM,		JSPROP_ENUMANDPERM },
	{ "requireLocation",    CSP_REQUIRELOCATION,    JSPROP_ENUMANDPERM },
	{ "requireChar",		CSP_REQUIRECHAR,		JSPROP_ENUMANDPERM },
	{ "travelSpell",		CSP_TRAVELSPELL,		JSPROP_ENUMANDPERM },
	{ "fieldSpell",			CSP_FIELDSPELL,			JSPROP_ENUMANDPERM },
	{ "reflectable",		CSP_REFLECTABLE,		JSPROP_ENUMANDPERM },
	{ "agressiveSpell",		CSP_AGRESSIVESPELL,		JSPROP_ENUMANDPERM },
	{ "resistable",			CSP_RESISTABLE,			JSPROP_ENUMANDPERM },
	{ "soundEffect",		CSP_SOUNDEFFECT,		JSPROP_ENUMANDPERM },
	{ NULL,			-1,			(UI08)0	   }
};

static JSPropertySpec CRaceProperties[] = 
{
	{ "id",					CRP_ID,					JSPROP_ENUMANDPERM },
	{ "name",				CRP_NAME,				JSPROP_ENUMANDPERM },
	{ "weakToWeather",		CRP_WEAKTOWEATHER,		JSPROP_ENUMPERMIDX },
	{ "requiresBeard",		CRP_REQUIRESBEARD,		JSPROP_ENUMANDPERM },
	{ "requiresNoBeard",	CRP_REQUIRESNOBEARD,	JSPROP_ENUMANDPERM },
	{ "isPlayerRace",		CRP_ISPLAYERRACE,		JSPROP_ENUMANDPERM },
	{ "genderRestrict",		CRP_GENDERRESTRICT,		JSPROP_ENUMANDPERM },
	{ "armourClass",		CRP_ARMOURCLASS,		JSPROP_ENUMANDPERM },
	{ "languageSkillMin",	CRP_LANGUAGESKILLMIN,	JSPROP_ENUMANDPERM },
	{ "skillAdjustment",	CRP_SKILLADJUSTMENT,	JSPROP_ENUMPERMIDX },
	{ "poisonResistance",	CRP_POISONRESISTANCE,	JSPROP_ENUMANDPERM },
	{ "magicResistance",	CRP_MAGICRESISTANCE,	JSPROP_ENUMANDPERM },
	{ "visibleDistance",	CRP_VISIBLEDISTANCE,	JSPROP_ENUMANDPERM },
	{ "nightVision",		CRP_NIGHTVISION,		JSPROP_ENUMANDPERM },
	{ NULL,					(SI08)NULL,				(UI08)0			   }
};


static JSPropertySpec CRegionProperties[] = 
{
	{ "name",				CREGP_NAME,					JSPROP_ENUMANDPERM },
	{ "mayor",				CREGP_MAYOR,				JSPROP_ENUMANDPERM },
	{ "race",				CREGP_RACE,					JSPROP_ENUMANDPERM },
	{ "tax",				CREGP_TAX,					JSPROP_ENUMANDPERM },
	{ "taxResource",		CREGP_TAXRESOURCE,			JSPROP_ENUMANDPERM },
	{ "canMark",			CREGP_CANMARK,				JSPROP_ENUMANDPERM },
	{ "canRecall",			CREGP_CANRECALL,			JSPROP_ENUMANDPERM },
	{ "canGate",			CREGP_CANGATE,				JSPROP_ENUMANDPERM },
	{ "isGuarded",			CREGP_ISGUARDED,			JSPROP_ENUMANDPERM },
	{ "canCastAggressive",	CREGP_CANCASTAGGRESSIVE,	JSPROP_ENUMANDPERM },
	{ "health",				CREGP_HEALTH,				JSPROP_ENUMANDPERM },
	{ "isDungeon",			CREGP_ISDUNGEON,			JSPROP_ENUMANDPERM },
	{ "chanceBigOre",		CREGP_CHANCEBIGORE,			JSPROP_ENUMANDPERM },
	{ "chanceColourOre",	CREGP_CHANCECOLOURORE,		JSPROP_ENUMANDPERM },
	{ "numOrePrefs",		CREGP_NUMOREPREFERENCES,	JSPROP_ENUMPERMRO  },
	{ "orePrefs",			CREGP_OREPREFERENCES,		JSPROP_ENUMPERMIDX },
	{ "population",			CREGP_POPULATION,			JSPROP_ENUMPERMRO  },
	{ "members",			CREGP_MEMBERS,				JSPROP_ENUMPERMIDX },
	{ NULL,					(SI08)NULL,					(UI08)0			   }
};

static JSPropertySpec CGuildProperties[] = 
{
	{ "name",			CGP_NAME,			JSPROP_ENUMANDPERM },
	{ "type",			CGP_TYPE,			JSPROP_ENUMANDPERM },
	{ "master",			CGP_MASTER,			JSPROP_ENUMANDPERM },
	{ "stone",			CGP_STONE,			JSPROP_ENUMANDPERM },
	{ "numMembers",		CGP_NUMMEMBERS,		JSPROP_ENUMPERMRO  },
	{ "numRecruits",	CGP_NUMRECRUITS,	JSPROP_ENUMPERMRO  },
	{ "members",		CGP_MEMBERS,		JSPROP_ENUMPERMIDX },
	{ "recruits",		CGP_RECRUITS,		JSPROP_ENUMPERMIDX },
	{ "charter",		CGP_CHARTER,		JSPROP_ENUMANDPERM },
	{ "abbreviation",	CGP_ABBREVIATION,	JSPROP_ENUMANDPERM },
	{ "webPage",		CGP_WEBPAGE,		JSPROP_ENUMANDPERM },
	{ NULL,				(SI08)NULL,			(UI08)0}
};

static JSPropertySpec CCharacterProps[] = 
{
	{ "name",			CCP_NAME,			JSPROP_ENUMANDPERM },
	{ "title",			CCP_TITLE,			JSPROP_ENUMANDPERM },
	{ "x",				CCP_X,				JSPROP_ENUMANDPERM },
	{ "y",				CCP_Y,				JSPROP_ENUMANDPERM },
	{ "z",				CCP_Z,				JSPROP_ENUMANDPERM },
	{ "id",				CCP_ID,				JSPROP_ENUMANDPERM },
	{ "colour",			CCP_COLOUR,			JSPROP_ENUMANDPERM },
	{ "owner",			CCP_OWNER,			JSPROP_ENUMANDPERM },
	{ "visible",		CCP_VISIBLE,		JSPROP_ENUMANDPERM },
	{ "serial",			CCP_SERIAL,			JSPROP_ENUMANDPERM },
	{ "health",			CCP_HEALTH,			JSPROP_ENUMANDPERM },
	{ "scripttrigger",	CCP_SCRIPTTRIGGER,	JSPROP_ENUMANDPERM },
	{ "privateword",	CCP_PRIVATEWORD,	JSPROP_ENUMPERMIDX },
	{ "worldnumber",	CCP_WORLDNUMBER,	JSPROP_ENUMANDPERM },
	{ "target",			CCP_TARGET,			JSPROP_ENUMANDPERM },
	{ "dexterity",		CCP_DEXTERITY,		JSPROP_ENUMANDPERM },
	{ "intelligence",	CCP_INTELLIGENCE,	JSPROP_ENUMANDPERM },
	{ "strength",		CCP_STRENGTH,		JSPROP_ENUMANDPERM },
	{ "skills",			CCP_SKILLS,			JSPROP_ENUMANDPERM },
	{ "mana",			CCP_MANA,			JSPROP_ENUMANDPERM },
	{ "stamina",		CCP_STAMINA,		JSPROP_ENUMANDPERM },
	{ "pack",			CCP_CHARPACK,		JSPROP_ENUMANDPERM },
	{ "fame",			CCP_FAME,			JSPROP_ENUMANDPERM },
	{ "karma",			CCP_KARMA,			JSPROP_ENUMANDPERM },
	{ "defense",		CCP_DEFENSE,		JSPROP_ENUMPERMRO  },
	{ "attack",			CCP_ATTACK,			JSPROP_ENUMPERMRO  },
	{ "hunger",			CCP_HUNGER,			JSPROP_ENUMANDPERM },
	{ "frozen",			CCP_FROZEN,			JSPROP_ENUMANDPERM },
	{ "commandlevel",	CCP_COMMANDLEVEL,	JSPROP_ENUMANDPERM },
	{ "race",			CCP_RACE,			JSPROP_ENUMANDPERM },
	{ "criminal",		CCP_CRIMINAL,		JSPROP_ENUMANDPERM },
	{ "murderer",		CCP_MURDERER,		JSPROP_ENUMPERMRO  },
	{ "innocent",		CCP_INNOCENT,		JSPROP_ENUMANDPERM },
	{ "murdercount",	CCP_MURDERCOUNT,	JSPROP_ENUMANDPERM },
	{ "gender",			CCP_GENDER,			JSPROP_ENUMANDPERM },
	{ "dead",			CCP_DEAD,			JSPROP_ENUMPERMRO  },
	{ "npc",			CCP_NPC,			JSPROP_ENUMANDPERM },
	{ "online",			CCP_ONLINE,			JSPROP_ENUMPERMRO  },
	{ "direction",		CCP_DIRECTION,		JSPROP_ENUMANDPERM },
	{ "region",			CCP_REGION,			JSPROP_ENUMANDPERM },
	{ "town",			CCP_TOWN,			JSPROP_ENUMANDPERM },
	{ "guild",			CCP_GUILD,			JSPROP_ENUMANDPERM },
	{ "baseskills",		CCP_BASESKILLS,		JSPROP_ENUMANDPERM },
	{ "skillsused",		CCP_SKILLUSE,		JSPROP_ENUMANDPERM },
	{ "socket",			CCP_SOCKET,			JSPROP_ENUMANDPERM },
	{ "isChar",			CCP_ISCHAR,			JSPROP_ENUMPERMRO  },
	{ "isItem",			CCP_ISITEM,			JSPROP_ENUMPERMRO  },
	{ "isSpawner",		CCP_ISSPAWNER,		JSPROP_ENUMPERMRO  },
	{ "raceID",			CCP_RACEID,			JSPROP_ENUMANDPERM },
	{ "maxhp",			CCP_MAXHP,			JSPROP_ENUMANDPERM },
	{ "maxstamina",		CCP_MAXSTAMINA,		JSPROP_ENUMANDPERM },
	{ "maxmana",		CCP_MAXMANA,		JSPROP_ENUMANDPERM },
	{ "wandertype",		CCP_WANDERTYPE,		JSPROP_ENUMANDPERM },
	{ "isonhorse",		CCP_ISONHORSE,		JSPROP_ENUMANDPERM },
	{ "tempdex",		CCP_TDEXTERITY,		JSPROP_ENUMANDPERM },
	{ "tempint",		CCP_TINTELLIGENCE,	JSPROP_ENUMANDPERM },
	{ "tempstr",		CCP_TSTRENGTH,		JSPROP_ENUMANDPERM },
	{ "poison",			CCP_POISON,			JSPROP_ENUMANDPERM },
	{ "lightlevel",		CCP_LIGHTLEVEL,		JSPROP_ENUMANDPERM },
	{ "armour",			CCP_ARMOUR,			JSPROP_ENUMANDPERM },
	{ "vulnerable",		CCP_VULNERABLE,		JSPROP_ENUMANDPERM },
	{ "willhunger",		CCP_HUNGERSTATUS,	JSPROP_ENUMANDPERM },
	{ "lodamage",		CCP_LODAMAGE,		JSPROP_ENUMANDPERM },
	{ "hidamage",		CCP_HIDAMAGE,		JSPROP_ENUMANDPERM },
	{ "flag",			CCP_FLAG,			JSPROP_ENUMPERMRO  },
	{ "atWar",			CCP_ATWAR,			JSPROP_ENUMANDPERM },
	{ "spellCast",		CCP_SPELLCAST,		JSPROP_ENUMANDPERM },
	{ "isCasting",		CCP_ISCASTING,		JSPROP_ENUMANDPERM },

	{ "townPriv",		CCP_TOWNPRIV,		JSPROP_ENUMANDPERM },
	{ "guildTitle",		CCP_GUILDTITLE,		JSPROP_ENUMANDPERM },
	{ "fontType",		CCP_FONTTYPE,		JSPROP_ENUMANDPERM },
	{ "sayColour",		CCP_SAYCOLOUR,		JSPROP_ENUMANDPERM },
	{ "emoteColour",	CCP_EMOTECOLOUR,	JSPROP_ENUMANDPERM },
	{ "attacker",		CCP_ATTACKER,		JSPROP_ENUMANDPERM },
	{ "raceGate",		CCP_RACEGATE,		JSPROP_ENUMANDPERM },
	{ "skillLock",		CCP_SKILLLOCK,		JSPROP_ENUMANDPERM },
	{ "deaths",			CCP_DEATHS,			JSPROP_ENUMANDPERM },
	{ "nextAct",		CCP_NEXTACT,		JSPROP_ENUMANDPERM },
	{ "petCount",		CCP_PETCOUNT,		JSPROP_ENUMPERMRO  },
	{ "ownedItemsCount",CCP_OWNEDITEMSCOUNT,JSPROP_ENUMPERMRO  },
	{ "cell",			CCP_CELL,			JSPROP_ENUMANDPERM },
	{ "allmove",		CCP_ALLMOVE,		JSPROP_ENUMANDPERM },
	{ "houseicons",		CCP_HOUSEICONS,		JSPROP_ENUMANDPERM },
	{ "spattack",		CCP_SPATTACK,		JSPROP_ENUMANDPERM },
	{ "spdelay",		CCP_SPDELAY,		JSPROP_ENUMANDPERM },
	{ "aitype",			CCP_AITYPE,			JSPROP_ENUMANDPERM },
	{ "split",			CCP_SPLIT,			JSPROP_ENUMANDPERM },
	{ "splitchance",	CCP_SPLITCHANCE,	JSPROP_ENUMANDPERM },
	{ "trainer",		CCP_TRAINER,		JSPROP_ENUMANDPERM },
	{ "weight",			CCP_WEIGHT,			JSPROP_ENUMANDPERM },
	{ "squelch",		CCP_SQUELCH,		JSPROP_ENUMANDPERM },
	{ "isJailed",		CCP_ISJAILED,		JSPROP_ENUMPERMRO  },

	{ NULL,			(SI08)NULL,			(UI08)0}
};

static JSPropertySpec CItemProps[] = 
{
	{ "name",			CIP_NAME,			JSPROP_ENUMANDPERM },
	{ "title",			CIP_TITLE,			JSPROP_ENUMANDPERM },
	{ "x",				CIP_X,				JSPROP_ENUMANDPERM },
	{ "y",				CIP_Y,				JSPROP_ENUMANDPERM },
	{ "z",				CIP_Z,				JSPROP_ENUMANDPERM },
	{ "id",				CIP_ID,				JSPROP_ENUMANDPERM },
	{ "colour",			CIP_COLOUR,			JSPROP_ENUMANDPERM },
	{ "owner",			CIP_OWNER,			JSPROP_ENUMANDPERM },
	{ "visible",		CIP_VISIBLE,		JSPROP_ENUMANDPERM },
	{ "serial",			CIP_SERIAL,			JSPROP_ENUMANDPERM },
	{ "health",			CIP_HEALTH,			JSPROP_ENUMANDPERM },
	{ "scripttrigger",	CIP_SCRIPTTRIGGER,	JSPROP_ENUMANDPERM },
	{ "privateword",	CIP_PRIVATEWORD,	JSPROP_ENUMPERMIDX },
	{ "worldnumber",	CIP_WORLDNUMBER,	JSPROP_ENUMANDPERM },
	{ "amount",			CIP_AMOUNT,			JSPROP_ENUMANDPERM },
	{ "container",		CIP_CONTAINER,		JSPROP_ENUMANDPERM },
	{ "type",			CIP_TYPE,			JSPROP_ENUMANDPERM },
	{ "more",			CIP_MORE,			JSPROP_ENUMANDPERM },
	{ "morex",			CIP_MOREX,			JSPROP_ENUMANDPERM },
	{ "morey",			CIP_MOREY,			JSPROP_ENUMANDPERM },
	{ "morez",			CIP_MOREZ,			JSPROP_ENUMANDPERM },
	{ "movable",		CIP_MOVABLE,		JSPROP_ENUMANDPERM },
	{ "att",			CIP_ATT,			JSPROP_ENUMANDPERM },
	{ "def",			CIP_DEF,			JSPROP_ENUMANDPERM },
	{ "layer",			CIP_LAYER,			JSPROP_ENUMANDPERM },
	{ "itemsinside",	CIP_ITEMSINSIDE,	JSPROP_ENUMANDPERM },
	{ "decayable",		CIP_DECAYABLE,		JSPROP_ENUMANDPERM },
	{ "decaytime",		CIP_DECAYTIME,		JSPROP_ENUMANDPERM },
	{ "lodamage",		CIP_LODAMAGE,		JSPROP_ENUMANDPERM },
	{ "hidamage",		CIP_HIDAMAGE,		JSPROP_ENUMANDPERM },
	{ "name2",			CIP_NAME2,			JSPROP_ENUMANDPERM },
	{ "isChar",			CIP_ISCHAR,			JSPROP_ENUMPERMRO  },
	{ "isItem",			CIP_ISITEM,			JSPROP_ENUMPERMRO  },
	{ "isSpawner",		CIP_ISSPAWNER,		JSPROP_ENUMPERMRO  },
	{ "raceID",			CIP_RACEID,			JSPROP_ENUMANDPERM },
	{ "race",			CIP_RACE,			JSPROP_ENUMANDPERM },
	{ "maxhp",			CIP_MAXHP,			JSPROP_ENUMANDPERM },
	{ "rank",			CIP_RANK,			JSPROP_ENUMANDPERM },
	{ "poison",			CIP_POISON,			JSPROP_ENUMANDPERM },
	{ "dir",			CIP_DIR,			JSPROP_ENUMANDPERM },
	{ "wipable",		CIP_WIPABLE,		JSPROP_ENUMANDPERM },
	{ "buyvalue",		CIP_BUYVALUE,		JSPROP_ENUMANDPERM },
	{ "sellvalue",		CIP_SELLVALUE,		JSPROP_ENUMANDPERM },
	{ "restock",		CIP_RESTOCK,		JSPROP_ENUMANDPERM },
	{ "devinelock",		CIP_DEVINELOCK,		JSPROP_ENUMANDPERM },
	{ "weight",			CIP_WEIGHT,			JSPROP_ENUMANDPERM },
	{ "strength",		CIP_STRENGTH,		JSPROP_ENUMANDPERM },
	{ "corpse",			CIP_CORPSE,			JSPROP_ENUMANDPERM },
	// The Following vars are specific to CSpawnItem objects
	{ "spawnsection",	CIP_SPAWNSECTION,	JSPROP_ENUMANDPERM },
	{ "sectionalist",	CIP_SECTIONALIST,	JSPROP_ENUMANDPERM },
	{ "mininterval",	CIP_MININTERVAL,	JSPROP_ENUMANDPERM },
	{ "maxinterval",	CIP_MAXINTERVAL,	JSPROP_ENUMANDPERM },
	{ NULL,			(SI08)NULL,			(UI08)0}
};

static JSPropertySpec CSocketProps[] = 
{
	{ "account",			CSOCKP_ACCOUNT,				JSPROP_ENUMANDPERM },
	{ "currentChar",		CSOCKP_CURRENTCHAR,			JSPROP_ENUMANDPERM },
	{ "idleTimeout",		CSOCKP_IDLETIMEOUT,			JSPROP_ENUMANDPERM },
	{ "wasIdleWarned",		CSOCKP_WASIDLEWARNED,		JSPROP_ENUMANDPERM },
	{ "tempInt",			CSOCKP_TEMPINT,				JSPROP_ENUMANDPERM },
	{ "buffer",				CSOCKP_BUFFER,				JSPROP_ENUMPERMIDX },
	{ "xText",				CSOCKP_XTEXT,				JSPROP_ENUMPERMIDX },
	{ "clickZ",				CSOCKP_CLICKZ,				JSPROP_ENUMANDPERM },
	{ "addID",				CSOCKP_ADDID,				JSPROP_ENUMPERMIDX },
	{ "newClient",			CSOCKP_NEWCLIENT,			JSPROP_ENUMANDPERM },
	{ "firstPacket",		CSOCKP_FIRSTPACKET,			JSPROP_ENUMANDPERM },
	{ "cryptClient",		CSOCKP_CRYPTCLIENT,			JSPROP_ENUMANDPERM },
	{ "clientIP",			CSOCKP_CLIENTIP,			JSPROP_ENUMPERMIDX },
	{ "walkSequence",		CSOCKP_WALKSEQUENCE,		JSPROP_ENUMANDPERM },
	{ "currentSpellType",	CSOCKP_CURRENTSPELLTYPE,	JSPROP_ENUMANDPERM },
	{ "logging",			CSOCKP_LOGGING,				JSPROP_ENUMANDPERM },
	{ "bytesSent",			CSOCKP_BYTESSENT,			JSPROP_ENUMANDPERM },
	{ "bytesReceived",		CSOCKP_BYTESRECEIVED,		JSPROP_ENUMANDPERM },
	{ "targetOK",			CSOCKP_TARGETOK,			JSPROP_ENUMANDPERM },
	{ "clickX",				CSOCKP_CLICKX,				JSPROP_ENUMANDPERM },
	{ "clickY",				CSOCKP_CLICKY,				JSPROP_ENUMANDPERM },
	{ "pickupX",			CSOCKP_PICKUPX,				JSPROP_ENUMANDPERM },
	{ "pickupY",			CSOCKP_PICKUPY,				JSPROP_ENUMANDPERM },
	{ "pickupZ",			CSOCKP_PICKUPZ,				JSPROP_ENUMANDPERM },
	{ "pickupSpot",			CSOCKP_PICKUPSPOT,			JSPROP_ENUMANDPERM },
	{ "pickupSerial",		CSOCKP_PICKUPSERIAL,		JSPROP_ENUMANDPERM },
	{ "language",			CSOCKP_LANGUAGE,			JSPROP_ENUMANDPERM },
	{ "clientMajorVer",		CSOCKP_CLIENTMAJORVER,		JSPROP_ENUMANDPERM },
	{ "clientMinorVer",		CSOCKP_CLIENTMINORVER,		JSPROP_ENUMANDPERM },
	{ "clientSubVer",		CSOCKP_CLIENTSUBVER,		JSPROP_ENUMANDPERM },
	{ "clientLetterVer",	CSOCKP_CLIENTLETTERVER,		JSPROP_ENUMANDPERM },
	{ "clientType",			CSOCKP_CLIENTTYPE,			JSPROP_ENUMANDPERM },
	{ "target",				CSOCKP_TARGET,				JSPROP_ENUMANDPERM },
	{ "tempObj",			CSOCKP_TEMPOBJ,				JSPROP_ENUMANDPERM },
	{ NULL,					(SI08)NULL,				(UI08)0}
};

static JSPropertySpec CSkillsProps[] = 
{
	{ "alchemy",			ALCHEMY,			JSPROP_ENUMANDPERM },
	{ "anatomy",			ANATOMY,			JSPROP_ENUMANDPERM },
	{ "animallore",			ANIMALLORE,			JSPROP_ENUMANDPERM },
	{ "itemid",				ITEMID,				JSPROP_ENUMANDPERM },
	{ "armslore",			ARMSLORE,			JSPROP_ENUMANDPERM },
	{ "parrying",			PARRYING,			JSPROP_ENUMANDPERM },
	{ "begging",			BEGGING,			JSPROP_ENUMANDPERM },
	{ "blacksmithing",		BLACKSMITHING,		JSPROP_ENUMANDPERM },
	{ "bowcraft",			BOWCRAFT,			JSPROP_ENUMANDPERM },
	{ "peacemaking",		PEACEMAKING,		JSPROP_ENUMANDPERM },
	{ "camping",			CAMPING,			JSPROP_ENUMANDPERM },
	{ "carpentry",			CARPENTRY,			JSPROP_ENUMANDPERM },
	{ "cartography",		CARTOGRAPHY,		JSPROP_ENUMANDPERM },
	{ "cooking",			COOKING,			JSPROP_ENUMANDPERM },
	{ "detectinghidden",	DETECTINGHIDDEN,	JSPROP_ENUMANDPERM },
	{ "enticement",			ENTICEMENT,			JSPROP_ENUMANDPERM },
	{ "evaluatingintel",	EVALUATINGINTEL,	JSPROP_ENUMANDPERM },
	{ "healing",			HEALING,			JSPROP_ENUMANDPERM },
	{ "fishing",			FISHING,			JSPROP_ENUMANDPERM },
	{ "forensics",			FORENSICS,			JSPROP_ENUMANDPERM },
	{ "herding",			HERDING,			JSPROP_ENUMANDPERM },
	{ "hiding",				HIDING,				JSPROP_ENUMANDPERM },
	{ "provocation",		PROVOCATION,		JSPROP_ENUMANDPERM },
	{ "inscription",		INSCRIPTION,		JSPROP_ENUMANDPERM },
	{ "lockpicking",		LOCKPICKING,		JSPROP_ENUMANDPERM },
	{ "magery",				MAGERY,				JSPROP_ENUMANDPERM },
	{ "magicresistance",	MAGICRESISTANCE,	JSPROP_ENUMANDPERM },
	{ "tactics",			TACTICS,			JSPROP_ENUMANDPERM },
	{ "snooping",			SNOOPING,			JSPROP_ENUMANDPERM },
	{ "musicianship",		MUSICIANSHIP,		JSPROP_ENUMANDPERM },
	{ "poisoning",			POISONING,			JSPROP_ENUMANDPERM },
	{ "archery",			ARCHERY,			JSPROP_ENUMANDPERM },
	{ "spiritspeak",		SPIRITSPEAK,		JSPROP_ENUMANDPERM },
	{ "stealing",			STEALING,			JSPROP_ENUMANDPERM },
	{ "tailoring",			TAILORING,			JSPROP_ENUMANDPERM },
	{ "taming",				TAMING,				JSPROP_ENUMANDPERM },
	{ "tasteid",			TASTEID,			JSPROP_ENUMANDPERM },
	{ "tinkering",			TINKERING,			JSPROP_ENUMANDPERM },
	{ "tracking",			TRACKING,			JSPROP_ENUMANDPERM },
	{ "veterinary",			VETERINARY,			JSPROP_ENUMANDPERM },
	{ "swordsmanship",		SWORDSMANSHIP,		JSPROP_ENUMANDPERM },
	{ "macefighting",		MACEFIGHTING,		JSPROP_ENUMANDPERM },
	{ "fencing",			FENCING,			JSPROP_ENUMANDPERM },
	{ "wrestling",			WRESTLING,			JSPROP_ENUMANDPERM },
	{ "lumberjacking",		LUMBERJACKING,		JSPROP_ENUMANDPERM },
	{ "mining",				MINING,				JSPROP_ENUMANDPERM },
	{ "meditation",			MEDITATION,			JSPROP_ENUMANDPERM },
	{ "stealth",			STEALTH,			JSPROP_ENUMANDPERM },
	{ "removetraps",		REMOVETRAPS,		JSPROP_ENUMANDPERM },
	{ "allskills",			ALLSKILLS,			JSPROP_ENUMANDPERM },
	{ NULL,					(SI08)NULL,			(UI08)0}
};

static JSPropertySpec CGumpDataProperties[] =
{
	{	"buttons",	CGumpData_Button,	JSPROP_ENUMANDPERM	},
	{	"IDs",		CGumpData_ID ,		JSPROP_ENUMANDPERM	},
	{	NULL,		(SI08)NULL,			(UI08)0				}

};

static JSPropertySpec CFileProperties[] = 
{
	{	"eof",		CFILE_EOF,			JSPROP_ENUMPERMRO	},
	{	"pos",		CFILE_POS,			JSPROP_ENUMANDPERM	},
	{	"length",	CFILE_LENGTH,		JSPROP_ENUMPERMRO	},
	{	NULL,		(SI08)NULL,			(UI08)0				}
};

static JSPropertySpec CAccountProperties[] =
{
	{ "username",	CACCOUNT_USERNAME,		JSPROP_ENUMANDPERM },
	{ "password",	CACCOUNT_PASSWORD,		JSPROP_ENUMANDPERM },
	{ "flags",		CACCOUNT_FLAGS,			JSPROP_ENUMANDPERM },
	{ "path",		CACCOUNT_PATH,			JSPROP_ENUMANDPERM },
	{ "comment",	CACCOUNT_COMMENT,		JSPROP_ENUMANDPERM },
	{ "character1",	CACCOUNT_CHARACTER1,	JSPROP_ENUMANDPERM },
	{ "character2",	CACCOUNT_CHARACTER2,	JSPROP_ENUMANDPERM },
	{ "character3",	CACCOUNT_CHARACTER3,	JSPROP_ENUMANDPERM },
	{ "character4",	CACCOUNT_CHARACTER4,	JSPROP_ENUMANDPERM },
	{ "character5",	CACCOUNT_CHARACTER5,	JSPROP_ENUMANDPERM },
	{ "character6",	CACCOUNT_CHARACTER6,	JSPROP_ENUMANDPERM },
	{	NULL,		(SI08)NULL,			(UI08)0				}
};

}

#endif
