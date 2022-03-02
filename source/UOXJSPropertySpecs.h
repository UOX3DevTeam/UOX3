//o-----------------------------------------------------------------------------------------------o
//|	File		-	UOXJSPropertyFuncts.cpp
//|	Date		-	12/14/2001
//o-----------------------------------------------------------------------------------------------o
//|	Notes		-	1.0		14th December, 2001 Initial implementation
//|							Defines the property specifications for race, region, guild,
//|							item and chars
//o-----------------------------------------------------------------------------------------------o
#ifndef __UOXJSPropertySpecs__
#define __UOXJSPropertySpecs__

#include "UOXJSPropertyEnums.h"
#include "enums.h"


static JSPropertySpec CSpellProperties[] =
{
	{ "id",					CSP_ID,					JSPROP_ENUMANDPERM },
	{ "action",				CSP_ACTION,				JSPROP_ENUMANDPERM },
	{ "delay",				CSP_DELAY,				JSPROP_ENUMANDPERM },
	{ "damageDelay",		CSP_DAMAGEDELAY,		JSPROP_ENUMANDPERM },
	{ "recoveryDelay",		CSP_RECOVERYDELAY,		JSPROP_ENUMANDPERM },
	{ "health",				CSP_HEALTH,				JSPROP_ENUMANDPERM },
	{ "stamina",			CSP_STAMINA,			JSPROP_ENUMANDPERM },
	{ "mana",				CSP_MANA,				JSPROP_ENUMANDPERM },
	{ "mantra",				CSP_MANTRA,				JSPROP_ENUMANDPERM },
	{ "name",				CSP_NAME,				JSPROP_ENUMANDPERM },
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
	{ "aggressiveSpell",	CSP_AGGRESSIVESPELL,	JSPROP_ENUMANDPERM },
	{ "resistable",			CSP_RESISTABLE,			JSPROP_ENUMANDPERM },
	{ "soundEffect",		CSP_SOUNDEFFECT,		JSPROP_ENUMANDPERM },
	{ "enabled",			CSP_ENABLED,			JSPROP_ENUMANDPERM },
	{ "baseDmg",			CSP_BASEDMG,			JSPROP_ENUMANDPERM },
	{ nullptr,				(SI08)0,				(UI08)0			   }
};

static JSPropertySpec CGlobalSkillProperties[] =
{
	{ "name",				CGSKILL_NAME,			JSPROP_ENUMANDPERM },
	{ "madeWord",			CGSKILL_MADEWORD,		JSPROP_ENUMANDPERM },
	{ "strength",			CGSKILL_STRENGTH,		JSPROP_ENUMANDPERM },
	{ "dexterity",			CGSKILL_DEXTERITY,		JSPROP_ENUMANDPERM },
	{ "intelligence",		CGSKILL_INTELLIGENCE,	JSPROP_ENUMANDPERM },
	{ "skillDelay",			CGSKILL_SKILLDELAY,		JSPROP_ENUMANDPERM },
	{ "scriptID",			CGSKILL_SCRIPTID,		JSPROP_ENUMANDPERM },

	{ nullptr,				(SI08)0,				(UI08)0			   }
};

static JSPropertySpec CCreateEntryProperties[] =
{
	{ "id",					CMAKE_ID,				JSPROP_ENUMANDPERM },
	{ "name",				CMAKE_NAME,				JSPROP_ENUMANDPERM },
	{ "addItem",			CMAKE_ADDITEM,			JSPROP_ENUMANDPERM },
	{ "colour",				CMAKE_COLOUR,			JSPROP_ENUMANDPERM },
	{ "sound",				CMAKE_SOUND,			JSPROP_ENUMANDPERM },
	{ "minRank",			CMAKE_MINRANK,			JSPROP_ENUMANDPERM },
	{ "maxRank",			CMAKE_MAXRANK,			JSPROP_ENUMANDPERM },
	{ "delay",				CMAKE_DELAY,			JSPROP_ENUMANDPERM },
	{ "spell",				CMAKE_SPELL,			JSPROP_ENUMANDPERM },
	{ "resources",			CMAKE_RESOURCES,		JSPROP_ENUMANDPERM },
	{ "skills",				CMAKE_SKILLS,			JSPROP_ENUMANDPERM },
	{ "avgMinSkill",		CMAKE_AVGMINSKILL,		JSPROP_ENUMANDPERM },
	{ "avgMaxSkill",		CMAKE_AVGMAXSKILL,		JSPROP_ENUMANDPERM },
	{ nullptr,				(SI08)0,				(UI08)0			   }
};

static JSPropertySpec CTimerProperties[] =
{
	// Character Timers
	{ "TIMEOUT",			TIMER_TIMEOUT,			JSPROP_ENUMPERMRO },
	{ "INVIS",				TIMER_INVIS,			JSPROP_ENUMPERMRO },
	{ "HUNGER",				TIMER_HUNGER,			JSPROP_ENUMPERMRO },
	{ "THIRST",				TIMER_THIRST,			JSPROP_ENUMPERMRO },
	{ "POISONTIME",			TIMER_POISONTIME,		JSPROP_ENUMPERMRO },
	{ "POISONTEXT",			TIMER_POISONTEXT,		JSPROP_ENUMPERMRO },
	{ "POISONWEAROFF",		TIMER_POISONWEAROFF,	JSPROP_ENUMPERMRO },
	{ "SPELLTIME",			TIMER_SPELLTIME,		JSPROP_ENUMPERMRO },
	{ "SPELLRECOVERYTIME",	TIMER_SPELLRECOVERYTIME,JSPROP_ENUMPERMRO },
	{ "CRIMFLAG",			TIMER_CRIMFLAG,			JSPROP_ENUMPERMRO },
	{ "ANTISPAM",			TIMER_ANTISPAM,			JSPROP_ENUMPERMRO },
	{ "MURDERRATE",			TIMER_MURDERRATE,		JSPROP_ENUMPERMRO },
	{ "PEACETIMER",			TIMER_PEACETIMER,		JSPROP_ENUMPERMRO },
	{ "FLYINGTOGGLE",		TIMER_FLYINGTOGGLE,		JSPROP_ENUMPERMRO },
	{ "FIREFIELDTICK",		TIMER_FIREFIELDTICK,	JSPROP_ENUMPERMRO },
	{ "POISONFIELDTICK",	TIMER_POISONFIELDTICK,	JSPROP_ENUMPERMRO },
	{ "PARAFIELDTICK",		TIMER_PARAFIELDTICK,	JSPROP_ENUMPERMRO },
	{ "MOVETIME",			TIMER_MOVETIME,			JSPROP_ENUMPERMRO },
	{ "SPATIMER",			TIMER_SPATIMER,			JSPROP_ENUMPERMRO },
	{ "SUMMONTIME",			TIMER_SUMMONTIME,		JSPROP_ENUMPERMRO },
	{ "EVADETIME",			TIMER_EVADETIME,		JSPROP_ENUMPERMRO },
	{ "LOYALTYTIME",		TIMER_LOYALTYTIME,		JSPROP_ENUMPERMRO },
	{ "LOGOUT",				TIMER_LOGOUT,			JSPROP_ENUMPERMRO },

	// Socket Timers
	{ "SOCK_SKILLDELAY",		TIMER_SOCK_SKILLDELAY,		JSPROP_ENUMPERMRO },
	{ "SOCK_OBJDELAY",			TIMER_SOCK_OBJDELAY,		JSPROP_ENUMPERMRO },
	{ "SOCK_SPIRITSPEAK",		TIMER_SOCK_SPIRITSPEAK,		JSPROP_ENUMPERMRO },
	{ "SOCK_TRACKING",			TIMER_SOCK_TRACKING,		JSPROP_ENUMPERMRO },
	{ "SOCK_FISHING",			TIMER_SOCK_FISHING,			JSPROP_ENUMPERMRO },
	{ "SOCK_MUTETIME",			TIMER_SOCK_MUTETIME,		JSPROP_ENUMPERMRO },
	{ "SOCK_TRACKINGDISPLAY",	TIMER_SOCK_TRACKINGDISPLAY,	JSPROP_ENUMPERMRO },
	{ "SOCK_TRAFFICWARDEN",		TIMER_SOCK_TRAFFICWARDEN,	JSPROP_ENUMPERMRO },
	{ nullptr,					(SI08)0,					(UI08)0			   }
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
	{ nullptr,				(SI08)0,				(UI08)0			   }
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
	{ "canTeleport",		CREGP_CANTELEPORT,			JSPROP_ENUMANDPERM },
	{ "canPlaceHouse",		CREGP_CANPLACEHOUSE,		JSPROP_ENUMANDPERM },
	{ "isGuarded",			CREGP_ISGUARDED,			JSPROP_ENUMANDPERM },
	{ "canCastAggressive",	CREGP_CANCASTAGGRESSIVE,	JSPROP_ENUMANDPERM },
	{ "isSafeZone",			CREGP_ISSAFEZONE,			JSPROP_ENUMANDPERM },
	{ "health",				CREGP_HEALTH,				JSPROP_ENUMANDPERM },
	{ "isDungeon",			CREGP_ISDUNGEON,			JSPROP_ENUMANDPERM },
	{ "worldNumber",		CREGP_WORLDNUMBER,			JSPROP_ENUMANDPERM },
	{ "instanceID",			CREGP_INSTANCEID,			JSPROP_ENUMANDPERM },
	{ "chanceBigOre",		CREGP_CHANCEBIGORE,			JSPROP_ENUMANDPERM },
	{ "numOrePrefs",		CREGP_NUMOREPREFERENCES,	JSPROP_ENUMPERMRO  },
	{ "population",			CREGP_POPULATION,			JSPROP_ENUMPERMRO  },
	{ "members",			CREGP_MEMBERS,				JSPROP_ENUMPERMIDX },
	{ "id",					CREGP_ID,					JSPROP_ENUMANDPERM },
	{ "scriptTrigger",		CREGP_SCRIPTTRIGGER,		JSPROP_ENUMANDPERM },
	{ "scriptTriggers",		CREGP_SCRIPTTRIGGERS,		JSPROP_ENUMANDPERM },
	{ "numGuards",			CREGP_NUMGUARDS,			JSPROP_ENUMANDPERM },
	{ "taxes",				CREGP_TAXES,				JSPROP_ENUMANDPERM },
	{ "reserves",			CREGP_RESERVES,				JSPROP_ENUMANDPERM },
	{ "appearance",			CREGP_APPEARANCE,			JSPROP_ENUMANDPERM },
	{ "music",				CREGP_MUSIC,				JSPROP_ENUMANDPERM },
	{ "weather",			CREGP_WEATHER,				JSPROP_ENUMANDPERM },
	{ "owner",				CREGP_OWNER,				JSPROP_ENUMANDPERM },
	{ nullptr,				(SI08)0,					(UI08)0			   }
};

static JSPropertySpec CSpawnRegionProperties[] =
{
	{ "name",				CSPAWNREGP_NAME,					JSPROP_ENUMANDPERM },
	{ "regionNum",			CSPAWNREGP_REGIONNUM,				JSPROP_ENUMANDPERM },
	{ "itemList",			CSPAWNREGP_ITEMLIST,				JSPROP_ENUMANDPERM },
	{ "npcList",			CSPAWNREGP_NPCLIST,					JSPROP_ENUMANDPERM },
	{ "item",				CSPAWNREGP_ITEM,					JSPROP_ENUMANDPERM },
	{ "npc",				CSPAWNREGP_NPC,						JSPROP_ENUMANDPERM },
	{ "maxItems",			CSPAWNREGP_MAXITEMS,				JSPROP_ENUMANDPERM },
	{ "maxNpcs",			CSPAWNREGP_MAXNPCS,					JSPROP_ENUMANDPERM },
	{ "itemCount",			CSPAWNREGP_ITEMCOUNT,				JSPROP_ENUMANDPERM },
	{ "npcCount",			CSPAWNREGP_NPCCOUNT,				JSPROP_ENUMANDPERM },
	{ "onlyOutside",		CSPAWNREGP_ONLYOUTSIDE,				JSPROP_ENUMANDPERM },
	{ "isSpawner",			CSPAWNREGP_ISSPAWNER,				JSPROP_ENUMANDPERM },
	{ "prefZ",				CSPAWNREGP_PREFZ,					JSPROP_ENUMANDPERM },
	{ "x1",					CSPAWNREGP_X1,						JSPROP_ENUMANDPERM },
	{ "y1",					CSPAWNREGP_X2,						JSPROP_ENUMANDPERM },
	{ "x2",					CSPAWNREGP_X2,						JSPROP_ENUMANDPERM },
	{ "y2",					CSPAWNREGP_Y2,						JSPROP_ENUMANDPERM },
	{ "world",				CSPAWNREGP_WORLD,					JSPROP_ENUMANDPERM },
	{ "instanceID",			CSPAWNREGP_INSTANCEID,				JSPROP_ENUMANDPERM },
	{ "minTime",			CSPAWNREGP_MINTIME,					JSPROP_ENUMANDPERM },
	{ "maxTime",			CSPAWNREGP_MAXTIME,					JSPROP_ENUMANDPERM },
	{ "call",				CSPAWNREGP_CALL,					JSPROP_ENUMANDPERM },
	{ nullptr,				(SI08)0,							(UI08)0			   }
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
	{ nullptr,			(SI08)0,			(UI08)0}
};

static JSPropertySpec CCharacterProps[] =
{
	{ "name",			CCP_NAME,			JSPROP_ENUMANDPERM },
	{ "title",			CCP_TITLE,			JSPROP_ENUMANDPERM },
	{ "x",				CCP_X,				JSPROP_ENUMANDPERM },
	{ "y",				CCP_Y,				JSPROP_ENUMANDPERM },
	{ "z",				CCP_Z,				JSPROP_ENUMANDPERM },
	{ "oldX",			CCP_OLDX,			JSPROP_ENUMANDPERM },
	{ "oldY",			CCP_OLDY,			JSPROP_ENUMANDPERM },
	{ "oldZ",			CCP_OLDZ,			JSPROP_ENUMANDPERM },
	{ "id",				CCP_ID,				JSPROP_ENUMANDPERM },
	{ "colour",			CCP_COLOUR,			JSPROP_ENUMANDPERM },
	{ "controlSlots",	CCP_CONTROLSLOTS,	JSPROP_ENUMANDPERM },
	{ "controlSlotsUsed",	CCP_CONTROLSLOTSUSED,	JSPROP_ENUMANDPERM },
	{ "orneriness",		CCP_ORNERINESS,		JSPROP_ENUMANDPERM },
	{ "owner",			CCP_OWNER,			JSPROP_ENUMANDPERM },
	{ "visible",		CCP_VISIBLE,		JSPROP_ENUMANDPERM },
	{ "serial",			CCP_SERIAL,			JSPROP_ENUMANDPERM },
	{ "health",			CCP_HEALTH,			JSPROP_ENUMANDPERM },
	{ "scripttrigger",	CCP_SCRIPTTRIGGER,	JSPROP_ENUMANDPERM },
	{ "scriptTriggers",	CCP_SCRIPTTRIGGERS,	JSPROP_ENUMANDPERM },
	{ "worldnumber",	CCP_WORLDNUMBER,	JSPROP_ENUMANDPERM },
	{ "instanceID",		CCP_INSTANCEID,		JSPROP_ENUMANDPERM },
	{ "target",			CCP_TARGET,			JSPROP_ENUMANDPERM },
	{ "dexterity",		CCP_DEXTERITY,		JSPROP_ENUMANDPERM },
	{ "intelligence",	CCP_INTELLIGENCE,	JSPROP_ENUMANDPERM },
	{ "strength",		CCP_STRENGTH,		JSPROP_ENUMANDPERM },
	{ "actualDexterity",	CCP_ACTUALDEXTERITY,	JSPROP_ENUMANDPERM },
	{ "actualIntelligence",	CCP_ACTUALINTELLIGENCE,	JSPROP_ENUMANDPERM },
	{ "actualStrength",		CCP_ACTUALSTRENGTH,		JSPROP_ENUMANDPERM },
	{ "skills",			CCP_SKILLS,			JSPROP_ENUMANDPERM },
	{ "mana",			CCP_MANA,			JSPROP_ENUMANDPERM },
	{ "stamina",		CCP_STAMINA,		JSPROP_ENUMANDPERM },
	{ "pack",			CCP_CHARPACK,		JSPROP_ENUMANDPERM },
	{ "fame",			CCP_FAME,			JSPROP_ENUMANDPERM },
	{ "karma",			CCP_KARMA,			JSPROP_ENUMANDPERM },
	{ "attack",			CCP_ATTACK,			JSPROP_ENUMPERMRO  },
	{ "canAttack",		CCP_CANATTACK,		JSPROP_ENUMANDPERM },
	{ "brkPeaceChance",	CCP_BRKPEACE,		JSPROP_ENUMANDPERM },
	{ "setPeace",		CCP_SETPEACE,		JSPROP_ENUMANDPERM },
	{ "hunger",			CCP_HUNGER,			JSPROP_ENUMANDPERM },
	{ "hungerRate",		CCP_HUNGERRATE,		JSPROP_ENUMPERMRO },
	{ "thirst",			CCP_THIRST,			JSPROP_ENUMANDPERM },
	{ "thirstRate",		CCP_THIRSTRATE,		JSPROP_ENUMPERMRO },
	{ "frozen",			CCP_FROZEN,			JSPROP_ENUMANDPERM },
	{ "commandlevel",	CCP_COMMANDLEVEL,	JSPROP_ENUMANDPERM },
	{ "race",			CCP_RACE,			JSPROP_ENUMANDPERM },
	{ "criminal",		CCP_CRIMINAL,		JSPROP_ENUMANDPERM },
	{ "murderer",		CCP_MURDERER,		JSPROP_ENUMPERMRO  },
	{ "innocent",		CCP_INNOCENT,		JSPROP_ENUMANDPERM },
	{ "murdercount",	CCP_MURDERCOUNT,	JSPROP_ENUMANDPERM },
	{ "neutral",		CCP_NEUTRAL,		JSPROP_ENUMANDPERM },
	{ "npcFlag",		CCP_NPCFLAG,		JSPROP_ENUMANDPERM },
	{ "gender",			CCP_GENDER,			JSPROP_ENUMANDPERM },
	{ "dead",			CCP_DEAD,			JSPROP_ENUMPERMRO  },
	{ "npc",			CCP_NPC,			JSPROP_ENUMANDPERM },
	{ "isAwake",		CCP_AWAKE,			JSPROP_ENUMANDPERM },
	{ "online",			CCP_ONLINE,			JSPROP_ENUMPERMRO  },
	{ "direction",		CCP_DIRECTION,		JSPROP_ENUMANDPERM },
	{ "isRunning",		CCP_ISRUNNING,		JSPROP_ENUMANDPERM },
	{ "region",			CCP_REGION,			JSPROP_ENUMANDPERM },
	{ "town",			CCP_TOWN,			JSPROP_ENUMANDPERM },
	{ "guild",			CCP_GUILD,			JSPROP_ENUMANDPERM },
	{ "baseskills",		CCP_BASESKILLS,		JSPROP_ENUMANDPERM },
	{ "skillsused",		CCP_SKILLUSE,		JSPROP_ENUMANDPERM },
	{ "socket",			CCP_SOCKET,			JSPROP_ENUMANDPERM },
	{ "isChar",			CCP_ISCHAR,			JSPROP_ENUMPERMRO  },
	{ "isItem",			CCP_ISITEM,			JSPROP_ENUMPERMRO  },
	{ "isSpawner",		CCP_ISSPAWNER,		JSPROP_ENUMPERMRO  },
	{ "spawnSerial",	CCP_SPAWNSERIAL,	JSPROP_ENUMPERMRO  },
	{ "maxhp",			CCP_MAXHP,			JSPROP_ENUMANDPERM },
	{ "maxstamina",		CCP_MAXSTAMINA,		JSPROP_ENUMANDPERM },
	{ "maxmana",		CCP_MAXMANA,		JSPROP_ENUMANDPERM },
	{ "oldWandertype",	CCP_OLDWANDERTYPE,	JSPROP_ENUMANDPERM },
	{ "wandertype",		CCP_WANDERTYPE,		JSPROP_ENUMANDPERM },
	{ "isonhorse",		CCP_ISONHORSE,		JSPROP_ENUMANDPERM },
	{ "isFlying",		CCP_ISFLYING,		JSPROP_ENUMANDPERM },
	{ "isGuarded",		CCP_ISGUARDED,		JSPROP_ENUMANDPERM },
	{ "guarding",		CCP_GUARDING,		JSPROP_ENUMANDPERM },
	{ "tempdex",		CCP_TDEXTERITY,		JSPROP_ENUMANDPERM },
	{ "tempint",		CCP_TINTELLIGENCE,	JSPROP_ENUMANDPERM },
	{ "tempstr",		CCP_TSTRENGTH,		JSPROP_ENUMANDPERM },
	{ "poison",			CCP_POISON,			JSPROP_ENUMANDPERM },
	{ "lightlevel",		CCP_LIGHTLEVEL,		JSPROP_ENUMANDPERM },
	{ "vulnerable",		CCP_VULNERABLE,		JSPROP_ENUMANDPERM },
	{ "willhunger",		CCP_HUNGERSTATUS,	JSPROP_ENUMANDPERM },
	{ "willthirst",		CCP_THIRSTSTATUS,	JSPROP_ENUMANDPERM },
	{ "lodamage",		CCP_LODAMAGE,		JSPROP_ENUMANDPERM },
	{ "hidamage",		CCP_HIDAMAGE,		JSPROP_ENUMANDPERM },
	{ "flag",			CCP_FLAG,			JSPROP_ENUMPERMRO  },
	{ "atWar",			CCP_ATWAR,			JSPROP_ENUMANDPERM },
	{ "spellCast",		CCP_SPELLCAST,		JSPROP_ENUMANDPERM },
	{ "isCasting",		CCP_ISCASTING,		JSPROP_ENUMANDPERM },
	{ "priv",			CCP_PRIV,			JSPROP_ENUMANDPERM },

	{ "townPriv",		CCP_TOWNPRIV,		JSPROP_ENUMANDPERM },
	{ "guildTitle",		CCP_GUILDTITLE,		JSPROP_ENUMANDPERM },
	{ "fontType",		CCP_FONTTYPE,		JSPROP_ENUMANDPERM },
	{ "sayColour",		CCP_SAYCOLOUR,		JSPROP_ENUMANDPERM },
	{ "emoteColour",	CCP_EMOTECOLOUR,	JSPROP_ENUMANDPERM },
	{ "attacker",		CCP_ATTACKER,		JSPROP_ENUMANDPERM },
	{ "raceGate",		CCP_RACEGATE,		JSPROP_ENUMANDPERM },
	{ "skillLock",		CCP_SKILLLOCK,		JSPROP_ENUMANDPERM },
	{ "deaths",			CCP_DEATHS,			JSPROP_ENUMANDPERM },
	{ "ownerCount",		CCP_OWNERCOUNT,		JSPROP_ENUMANDPERM },
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
	{ "hireling",		CCP_HIRELING,		JSPROP_ENUMANDPERM },
	{ "trainer",		CCP_TRAINER,		JSPROP_ENUMANDPERM },
	{ "weight",			CCP_WEIGHT,			JSPROP_ENUMANDPERM },
	{ "squelch",		CCP_SQUELCH,		JSPROP_ENUMANDPERM },
	{ "isJailed",		CCP_ISJAILED,		JSPROP_ENUMPERMRO  },
	{ "magicReflect",	CCP_MAGICREFLECT,	JSPROP_ENUMANDPERM },
	{ "permanentMagicReflect",	CCP_PERMMAGICREFLECT,	JSPROP_ENUMANDPERM },
	{ "tamed",			CCP_TAMED,			JSPROP_ENUMANDPERM },
	{ "tamedHungerRate",CCP_TAMEDHUNGERRATE,JSPROP_ENUMANDPERM },
	{ "tamedThirstRate",CCP_TAMEDTHIRSTRATE,JSPROP_ENUMANDPERM },
	{ "hungerWildChance",CCP_HUNGERWILDCHANCE,JSPROP_ENUMANDPERM },
	{ "thirstWildChance",CCP_THIRSTWILDCHANCE,JSPROP_ENUMANDPERM },
	{ "foodList",		CCP_FOODLIST,		JSPROP_ENUMANDPERM },
	{ "mounted",		CCP_MOUNTED,		JSPROP_ENUMANDPERM },
	{ "stabled",		CCP_STABLED,		JSPROP_ENUMANDPERM },
	{ "isUsingPotion",	CCP_USINGPOTION,	JSPROP_ENUMANDPERM },
	{ "stealth",		CCP_STEALTH,		JSPROP_ENUMANDPERM },
	{ "skillToTame",	CCP_SKILLTOTAME,	JSPROP_ENUMANDPERM },
	{ "skillToProv",	CCP_SKILLTOPROV,	JSPROP_ENUMANDPERM },
	{ "skillToPeace",	CCP_SKILLTOPEACE,	JSPROP_ENUMANDPERM },
	{ "poisonStrength",	CCP_POISONSTRENGTH,	JSPROP_ENUMANDPERM },
	{ "isPolymorphed",	CCP_ISPOLYMORPHED,	JSPROP_ENUMANDPERM },
	{ "isIncognito",	CCP_ISINCOGNITO,	JSPROP_ENUMANDPERM },
	{ "canRun",			CCP_CANRUN,			JSPROP_ENUMANDPERM },
	{ "isMeditating",	CCP_ISMEDITATING,	JSPROP_ENUMANDPERM },
	{ "isGM",			CCP_ISGM,			JSPROP_ENUMANDPERM },
	{ "canBroadcast",	CCP_CANBROADCAST,	JSPROP_ENUMANDPERM },
	{ "singClickSer",	CCP_SINGCLICKSER,	JSPROP_ENUMANDPERM },
	{ "noSkillTitles",	CCP_NOSKILLTITLES,	JSPROP_ENUMANDPERM },
	{ "isGMPageable",	CCP_ISGMPAGEABLE,	JSPROP_ENUMANDPERM },
	{ "canSnoop",		CCP_CANSNOOP,		JSPROP_ENUMANDPERM },
	{ "isCounselor",	CCP_ISCOUNSELOR,	JSPROP_ENUMANDPERM },
	{ "noNeedMana",		CCP_NONEEDMANA,		JSPROP_ENUMANDPERM },
	{ "isDispellable",	CCP_ISDISPELLABLE,	JSPROP_ENUMANDPERM },
	{ "noNeedReags",	CCP_NONEEDREAGS,	JSPROP_ENUMANDPERM },
	{ "orgID",			CCP_ORGID,			JSPROP_ENUMANDPERM },
	{ "orgSkin",		CCP_ORGSKIN,		JSPROP_ENUMANDPERM },
	{ "isAnimal",		CCP_ISANIMAL,		JSPROP_ENUMPERMRO  },
	{ "isHuman",		CCP_ISHUMAN,		JSPROP_ENUMPERMRO  },
	{ "isShop",			CCP_ISSHOP,			JSPROP_ENUMANDPERM },
	{ "attackFirst",	CCP_ATTACKFIRST,	JSPROP_ENUMANDPERM },
	{ "maxLoyalty",		CCP_MAXLOYALTY,		JSPROP_ENUMANDPERM },
	{ "loyalty",		CCP_LOYALTY,		JSPROP_ENUMANDPERM },
	{ "loyaltyRate",	CCP_LOYALTYRATE,	JSPROP_ENUMANDPERM },
	{ "shouldSave",		CCP_SHOULDSAVE,		JSPROP_ENUMANDPERM },

	{ "partyLootable",	CCP_PARTYLOOTABLE,	JSPROP_ENUMANDPERM },
	{ "party",			CCP_PARTY,			JSPROP_ENUMPERMRO  },
	{ "multi",			CCP_MULTI,			JSPROP_ENUMANDPERM },
	{ "accountNum",		CCP_ACCOUNTNUM,		JSPROP_ENUMANDPERM },
	{ "account",		CCP_ACCOUNT,		JSPROP_ENUMANDPERM },
	{ "createdOn",		CCP_CREATEDON,		JSPROP_ENUMANDPERM },
	{ "housesOwned",	CCP_HOUSESOWNED,	JSPROP_ENUMANDPERM },
	{ "housesCoOwned",	CCP_HOUSESCOOWNED,	JSPROP_ENUMANDPERM },

	{ nullptr,			(SI08)0,			(UI08)0}
};

static JSPropertySpec CItemProps[] =
{
	{ "name",			CIP_NAME,			JSPROP_ENUMANDPERM },
	{ "title",			CIP_TITLE,			JSPROP_ENUMANDPERM },
	{ "x",				CIP_X,				JSPROP_ENUMANDPERM },
	{ "y",				CIP_Y,				JSPROP_ENUMANDPERM },
	{ "z",				CIP_Z,				JSPROP_ENUMANDPERM },
	{ "oldX",			CIP_OLDX,			JSPROP_ENUMANDPERM },
	{ "oldY",			CIP_OLDY,			JSPROP_ENUMANDPERM },
	{ "oldZ",			CIP_OLDZ,			JSPROP_ENUMANDPERM },
	{ "id",				CIP_ID,				JSPROP_ENUMANDPERM },
	{ "colour",			CIP_COLOUR,			JSPROP_ENUMANDPERM },
	{ "owner",			CIP_OWNER,			JSPROP_ENUMANDPERM },
	{ "visible",		CIP_VISIBLE,		JSPROP_ENUMANDPERM },
	{ "serial",			CIP_SERIAL,			JSPROP_ENUMANDPERM },
	{ "health",			CIP_HEALTH,			JSPROP_ENUMANDPERM },
	{ "scripttrigger",	CIP_SCRIPTTRIGGER,	JSPROP_ENUMANDPERM },
	{ "scriptTriggers",	CIP_SCRIPTTRIGGERS,	JSPROP_ENUMANDPERM },
	{ "worldnumber",	CIP_WORLDNUMBER,	JSPROP_ENUMANDPERM },
	{ "instanceID",		CIP_INSTANCEID,		JSPROP_ENUMANDPERM },
	{ "amount",			CIP_AMOUNT,			JSPROP_ENUMANDPERM },
	{ "container",		CIP_CONTAINER,		JSPROP_ENUMANDPERM },
	{ "type",			CIP_TYPE,			JSPROP_ENUMANDPERM },
	{ "more",			CIP_MORE,			JSPROP_ENUMANDPERM },
	{ "morex",			CIP_MOREX,			JSPROP_ENUMANDPERM },
	{ "morey",			CIP_MOREY,			JSPROP_ENUMANDPERM },
	{ "morez",			CIP_MOREZ,			JSPROP_ENUMANDPERM },
	{ "movable",		CIP_MOVABLE,		JSPROP_ENUMANDPERM },
	{ "att",			CIP_ATT,			JSPROP_ENUMANDPERM },
	{ "layer",			CIP_LAYER,			JSPROP_ENUMANDPERM },
	{ "itemsinside",	CIP_ITEMSINSIDE,	JSPROP_ENUMANDPERM },
	{ "totalItemCount",	CIP_TOTALITEMCOUNT,	JSPROP_ENUMANDPERM },	
	{ "decayable",		CIP_DECAYABLE,		JSPROP_ENUMANDPERM },
	{ "decaytime",		CIP_DECAYTIME,		JSPROP_ENUMANDPERM },
	{ "lodamage",		CIP_LODAMAGE,		JSPROP_ENUMANDPERM },
	{ "hidamage",		CIP_HIDAMAGE,		JSPROP_ENUMANDPERM },
	{ "ac",				CIP_AC,				JSPROP_ENUMANDPERM },
	{ "def",			CIP_DEF,			JSPROP_ENUMANDPERM },
	{ "resistCold",		CIP_RESISTCOLD,		JSPROP_ENUMANDPERM },
	{ "resistHeat",		CIP_RESISTHEAT,		JSPROP_ENUMANDPERM },
	{ "resistLight",	CIP_RESISTLIGHT,	JSPROP_ENUMANDPERM },
	{ "resistLightning",CIP_RESISTLIGHTNING,JSPROP_ENUMANDPERM },
	{ "resistPoison",	CIP_RESISTPOISON,	JSPROP_ENUMANDPERM },
	{ "resistRain",		CIP_RESISTRAIN,		JSPROP_ENUMANDPERM },
	{ "resistSnow",		CIP_RESISTSNOW,		JSPROP_ENUMANDPERM },
	{ "damageHeat",		CIP_DAMAGEHEAT,		JSPROP_ENUMANDPERM },
	{ "damageCold",		CIP_DAMAGECOLD,		JSPROP_ENUMANDPERM },
	{ "damageLight",	CIP_DAMAGELIGHT,	JSPROP_ENUMANDPERM },
	{ "damageLightning",CIP_DAMAGELIGHTNING,JSPROP_ENUMANDPERM },
	{ "damagePoison",	CIP_DAMAGEPOISON,	JSPROP_ENUMANDPERM },
	{ "damageRain",		CIP_DAMAGERAIN,		JSPROP_ENUMANDPERM },
	{ "damageSnow",		CIP_DAMAGESNOW,		JSPROP_ENUMANDPERM },
	{ "name2",			CIP_NAME2,			JSPROP_ENUMANDPERM },
	{ "isChar",			CIP_ISCHAR,			JSPROP_ENUMPERMRO  },
	{ "isItem",			CIP_ISITEM,			JSPROP_ENUMPERMRO  },
	{ "isSpawner",		CIP_ISSPAWNER,		JSPROP_ENUMPERMRO  },
	{ "race",			CIP_RACE,			JSPROP_ENUMANDPERM },
	{ "maxhp",			CIP_MAXHP,			JSPROP_ENUMANDPERM },
	{ "maxUses",		CIP_MAXUSES,		JSPROP_ENUMANDPERM },
	{ "usesLeft",		CIP_USESLEFT,		JSPROP_ENUMANDPERM },
	{ "rank",			CIP_RANK,			JSPROP_ENUMANDPERM },
	{ "creator",		CIP_CREATOR,		JSPROP_ENUMANDPERM },
	{ "poison",			CIP_POISON,			JSPROP_ENUMANDPERM },
	{ "dir",			CIP_DIR,			JSPROP_ENUMANDPERM },
	{ "wipable",		CIP_WIPABLE,		JSPROP_ENUMANDPERM },
	{ "buyvalue",		CIP_BUYVALUE,		JSPROP_ENUMANDPERM },
	{ "sellvalue",		CIP_SELLVALUE,		JSPROP_ENUMANDPERM },
	{ "restock",		CIP_RESTOCK,		JSPROP_ENUMANDPERM },
	{ "divinelock",		CIP_DIVINELOCK,		JSPROP_ENUMANDPERM },
	{ "baseWeight",		CIP_BASEWEIGHT,		JSPROP_ENUMANDPERM },
	{ "weight",			CIP_WEIGHT,			JSPROP_ENUMANDPERM },
	{ "weightMax",		CIP_WEIGHTMAX,		JSPROP_ENUMANDPERM },
	{ "maxItems",		CIP_MAXITEMS,		JSPROP_ENUMANDPERM },
	{ "strength",		CIP_STRENGTH,		JSPROP_ENUMANDPERM },
	{ "dexterity",		CIP_DEXTERITY,		JSPROP_ENUMANDPERM },
	{ "intelligence",	CIP_INTELLIGENCE,	JSPROP_ENUMANDPERM },
	{ "corpse",			CIP_CORPSE,			JSPROP_ENUMANDPERM },
	{ "desc",			CIP_DESC,			JSPROP_ENUMANDPERM },
	{ "event",			CIP_EVENT,			JSPROP_ENUMANDPERM },
	{ "tempTimer",		CIP_TEMPTIMER,		JSPROP_ENUMANDPERM },
	{ "shouldSave",		CIP_SHOULDSAVE,		JSPROP_ENUMANDPERM },

	{ "isNewbie",		CIP_ISNEWBIE,		JSPROP_ENUMANDPERM },
	{ "isDispellable",	CIP_ISDISPELLABLE,	JSPROP_ENUMANDPERM },
	{ "madeWith",		CIP_MADEWITH,		JSPROP_ENUMANDPERM },
	{ "entryMadeFrom",	CIP_ENTRYMADEFROM,	JSPROP_ENUMANDPERM },
	{ "isPileable",		CIP_ISPILEABLE,		JSPROP_ENUMANDPERM },
	{ "isDyeable",		CIP_ISDYEABLE,		JSPROP_ENUMANDPERM },
	{ "isDamageable",	CIP_ISDAMAGEABLE,	JSPROP_ENUMANDPERM },
	{ "isWipeable",		CIP_ISWIPEABLE,		JSPROP_ENUMANDPERM },
	{ "isGuarded",		CIP_ISGUARDED,		JSPROP_ENUMANDPERM },
	{ "isDoorOpen",		CIP_ISDOOROPEN,		JSPROP_ENUMANDPERM },
	{ "isFieldSpell",	CIP_ISFIELDSPELL,	JSPROP_ENUMPERMRO },
	{ "isLockedDown",	CIP_ISLOCKEDDOWN,	JSPROP_ENUMPERMRO },
	{ "isShieldType",	CIP_ISSHIELDTYPE,	JSPROP_ENUMPERMRO },
	{ "isMetalType",	CIP_ISMETALTYPE,	JSPROP_ENUMPERMRO },
	{ "isLeatherType",	CIP_ISLEATHERTYPE,	JSPROP_ENUMPERMRO },
	{ "canBeLockedDown",CIP_CANBELOCKEDDOWN,JSPROP_ENUMPERMRO },
	{ "isContType",		CIP_ISCONTTYPE,		JSPROP_ENUMPERMRO },
	{ "carveSection",	CIP_CARVESECTION,	JSPROP_ENUMANDPERM },
	{ "ammoID",			CIP_AMMOID,			JSPROP_ENUMANDPERM },
	{ "ammoHue",		CIP_AMMOHUE,		JSPROP_ENUMANDPERM },
	{ "ammoFX",			CIP_AMMOFX,			JSPROP_ENUMANDPERM },
	{ "ammoFXHue",		CIP_AMMOFXHUE,		JSPROP_ENUMANDPERM },
	{ "ammoFXRender",	CIP_AMMOFXRENDER,	JSPROP_ENUMANDPERM },
	{ "speed",			CIP_SPEED,			JSPROP_ENUMANDPERM },
	{ "multi",			CIP_MULTI,			JSPROP_ENUMANDPERM },
	{ "maxRange",		CIP_MAXRANGE,		JSPROP_ENUMANDPERM },
	{ "baseRange",		CIP_BASERANGE,		JSPROP_ENUMANDPERM },
	{ "region",			CIP_REGION,			JSPROP_ENUMPERMRO },
	{ "spawnSerial",	CIP_SPAWNSERIAL,	JSPROP_ENUMPERMRO },
	{ "isItemHeld",		CIP_ISITEMHELD,		JSPROP_ENUMANDPERM },

	// The Following vars are specific to CSpawnItem objects
	{ "spawnsection",	CIP_SPAWNSECTION,	JSPROP_ENUMANDPERM },
	{ "sectionalist",	CIP_SECTIONALIST,	JSPROP_ENUMANDPERM },
	{ "mininterval",	CIP_MININTERVAL,	JSPROP_ENUMANDPERM },
	{ "maxinterval",	CIP_MAXINTERVAL,	JSPROP_ENUMANDPERM },
	
	// The Following vars are specific to cMultiObj objects
	{ "lockdowns",		CIP_LOCKDDOWNS,		JSPROP_ENUMANDPERM },
	{ "maxLockdowns",	CIP_MAXLOCKDOWNS,	JSPROP_ENUMANDPERM },
	{ "trashContainers",	CIP_TRASHCONTAINERS,	JSPROP_ENUMANDPERM },
	{ "maxTrashContainers",	CIP_MAXTRASHCONTAINERS,	JSPROP_ENUMANDPERM },
	{ "secureContainers",	CIP_SECURECONTAINERS,	JSPROP_ENUMANDPERM },
	{ "maxSecureContainers",CIP_MAXSECURECONTAINERS,JSPROP_ENUMANDPERM },
	{ "friends",		CIP_FRIENDS,		JSPROP_ENUMANDPERM },
	{ "maxFriends",		CIP_MAXFRIENDS,		JSPROP_ENUMANDPERM },
	{ "guests",			CIP_GUESTS,			JSPROP_ENUMANDPERM },
	{ "maxGuests",		CIP_MAXGUESTS,		JSPROP_ENUMANDPERM },
	{ "owners",			CIP_OWNERS,			JSPROP_ENUMANDPERM },
	{ "maxOwners",		CIP_MAXOWNERS,		JSPROP_ENUMANDPERM },
	{ "bans",			CIP_BANS,			JSPROP_ENUMANDPERM },
	{ "maxBans",		CIP_MAXBANS,		JSPROP_ENUMANDPERM },
	{ "vendors",		CIP_VENDORS,		JSPROP_ENUMANDPERM },
	{ "maxVendors",		CIP_MAXVENDORS,		JSPROP_ENUMANDPERM },
	{ "deed",			CIP_DEED,			JSPROP_ENUMANDPERM },
	{ "isPublic",		CIP_ISPUBLIC,		JSPROP_ENUMANDPERM },
	{ "buildTimestamp",	CIP_BUILDTIMESTAMP,	JSPROP_ENUMANDPERM },
	{ "tradeTimestamp",	CIP_TRADETIMESTAMP,	JSPROP_ENUMANDPERM },
	{ "banX",			CIP_BANX,			JSPROP_ENUMANDPERM },
	{ "banY",			CIP_BANY,			JSPROP_ENUMANDPERM },


	{ nullptr,			(SI08)0,			(UI08)0}
};

static JSPropertySpec CSocketProps[] =
{
	{ "account",			CSOCKP_ACCOUNT,				JSPROP_ENUMANDPERM },
	{ "currentChar",		CSOCKP_CURRENTCHAR,			JSPROP_ENUMANDPERM },
	{ "idleTimeout",		CSOCKP_IDLETIMEOUT,			JSPROP_ENUMANDPERM },
	{ "wasIdleWarned",		CSOCKP_WASIDLEWARNED,		JSPROP_ENUMANDPERM },
	{ "tempInt",			CSOCKP_TEMPINT,				JSPROP_ENUMANDPERM },
	{ "tempInt2",			CSOCKP_TEMPINT2,			JSPROP_ENUMANDPERM },
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
	{ "tempObj2",			CSOCKP_TEMPOBJ2,			JSPROP_ENUMANDPERM },
	{ nullptr,				(SI08)0,					(UI08)0}
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
	{ "removetrap",			REMOVETRAP,			JSPROP_ENUMANDPERM },
	{ "necromancy",			NECROMANCY,			JSPROP_ENUMANDPERM },
	{ "focus",				FOCUS,				JSPROP_ENUMANDPERM },
	{ "chivalry",			CHIVALRY,			JSPROP_ENUMANDPERM },
	{ "bushido",			BUSHIDO,			JSPROP_ENUMANDPERM },
	{ "ninjitsu",			NINJITSU,			JSPROP_ENUMANDPERM },
	{ "spellweaving",		SPELLWEAVING,		JSPROP_ENUMANDPERM },
	{ "imbuing",			IMBUING,			JSPROP_ENUMANDPERM },
	{ "mysticism",			MYSTICISM,			JSPROP_ENUMANDPERM },
	{ "throwing",			THROWING,			JSPROP_ENUMANDPERM },
	{ "allskills",			ALLSKILLS,			JSPROP_ENUMANDPERM },
	{ nullptr,				(SI08)0,			(UI08)0}
};

static JSPropertySpec CGumpDataProperties[] =
{
	{	"buttons",	CGumpData_Button,	JSPROP_ENUMANDPERM	},
	{	"IDs",		CGumpData_ID ,		JSPROP_ENUMANDPERM	},
	{	nullptr,	(SI08)0,			(UI08)0				}

};

static JSPropertySpec CAccountProperties[] =
{
	{ "id",			CACCOUNT_ID,			JSPROP_ENUMANDPERM },
	{ "username",	CACCOUNT_USERNAME,		JSPROP_ENUMANDPERM },
	{ "password",	CACCOUNT_PASSWORD,		JSPROP_ENUMANDPERM },
	{ "flags",		CACCOUNT_FLAGS,			JSPROP_ENUMANDPERM },
	{ "path",		CACCOUNT_PATH,			JSPROP_ENUMANDPERM },
	{ "comment",	CACCOUNT_COMMENT,		JSPROP_ENUMANDPERM },
	{ "timeban",	CACCOUNT_TIMEBAN,		JSPROP_ENUMANDPERM },
	{ "firstLogin",	CACCOUNT_FIRSTLOGIN,	JSPROP_ENUMANDPERM },
	{ "character1",	CACCOUNT_CHARACTER1,	JSPROP_ENUMANDPERM },
	{ "character2",	CACCOUNT_CHARACTER2,	JSPROP_ENUMANDPERM },
	{ "character3",	CACCOUNT_CHARACTER3,	JSPROP_ENUMANDPERM },
	{ "character4",	CACCOUNT_CHARACTER4,	JSPROP_ENUMANDPERM },
	{ "character5",	CACCOUNT_CHARACTER5,	JSPROP_ENUMANDPERM },
	{ "character6",	CACCOUNT_CHARACTER6,	JSPROP_ENUMANDPERM },
	{ "character7",	CACCOUNT_CHARACTER7,	JSPROP_ENUMANDPERM },

	{ "currentChar",	CACCOUNT_CURRENTCHAR,	JSPROP_ENUMANDPERM },
	{ "lastIP",			CACCOUNT_LASTIP,		JSPROP_ENUMANDPERM },

	// Flags
	{ "isBanned",		CACCOUNT_BANNED,	JSPROP_ENUMANDPERM },
	{ "isSuspended",	CACCOUNT_SUSPENDED,	JSPROP_ENUMANDPERM },
	{ "isPublic",		CACCOUNT_PUBLIC,	JSPROP_ENUMANDPERM },
	{ "isOnline",		CACCOUNT_ONLINE,	JSPROP_ENUMANDPERM },
	{ "isSlot1Blocked",	CACCOUNT_CHARSLOT1BLOCKED,	JSPROP_ENUMANDPERM },
	{ "isSlot2Blocked",	CACCOUNT_CHARSLOT2BLOCKED,	JSPROP_ENUMANDPERM },
	{ "isSlot3Blocked",	CACCOUNT_CHARSLOT3BLOCKED,	JSPROP_ENUMANDPERM },
	{ "isSlot4Blocked",	CACCOUNT_CHARSLOT4BLOCKED,	JSPROP_ENUMANDPERM },
	{ "isSlot5Blocked",	CACCOUNT_CHARSLOT5BLOCKED,	JSPROP_ENUMANDPERM },
	{ "isSlot6Blocked",	CACCOUNT_CHARSLOT6BLOCKED,	JSPROP_ENUMANDPERM },
	{ "isSlot7Blocked",	CACCOUNT_CHARSLOT7BLOCKED,	JSPROP_ENUMANDPERM },
	{ "unused9",		CACCOUNT_UNUSED9,	JSPROP_ENUMANDPERM },
	{ "unused10",		CACCOUNT_UNUSED10,	JSPROP_ENUMANDPERM },
	{ "isSeer",			CACCOUNT_SEER,		JSPROP_ENUMANDPERM },
	{ "isCounselor",	CACCOUNT_COUNSELOR,	JSPROP_ENUMANDPERM },
	{ "isGM",			CACCOUNT_GM,		JSPROP_ENUMANDPERM },

	{	nullptr,		(SI08)0,			(UI08)0				}
};

static JSPropertySpec CConsoleProperties[] =
{
	{ "mode",		CCONSOLE_MODE,		JSPROP_ENUMANDPERM },
	{ "logEcho",	CCONSOLE_LOGECHO,	JSPROP_ENUMANDPERM },
	{	nullptr,	(SI08)0,			(UI08)0				}
};

static JSPropertySpec CScriptSectionProperties[] =
{
	{ "numTags",	CSS_NUMTAGS,		JSPROP_ENUMANDPERM },
	{ "atEnd",		CSS_ATEND,			JSPROP_ENUMANDPERM },
	{ "atEndTags",	CSS_ATENDTAGS,		JSPROP_ENUMANDPERM },
	{	nullptr,	(SI08)0,			(UI08)0				}
};

static JSPropertySpec CResourceProperties[] =
{
	{ "logAmount",	CRESP_LOGAMT,		JSPROP_ENUMANDPERM },
	{ "logTime",	CRESP_LOGTIME,		JSPROP_ENUMANDPERM },
	{ "oreAmount",	CRESP_OREAMT,		JSPROP_ENUMANDPERM },
	{ "oreTime",	CRESP_ORETIME,		JSPROP_ENUMANDPERM },
	{ "fishAmount",	CRESP_FISHAMT,		JSPROP_ENUMANDPERM },
	{ "fishTime",	CRESP_FISHTIME,		JSPROP_ENUMANDPERM },
	{	nullptr,	(SI08)0,			(UI08)0				}
};

static JSPropertySpec CPartyProperties[] =
{
	{ "leader",			CPARTYP_LEADER,			JSPROP_ENUMANDPERM	},
	{ "memberCount",	CPARTYP_MEMBERCOUNT,	JSPROP_ENUMPERMRO	},
	{ "isNPC",			CPARTYP_ISNPC,			JSPROP_ENUMANDPERM	},
	{	nullptr,		(SI08)0,				(UI08)0				}
};

#endif
