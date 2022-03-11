#ifndef __UOXJSPropertyEnums__
#define __UOXJSPropertyEnums__

const uint8 JSPROP_ENUMANDPERM	= JSPROP_ENUMERATE | JSPROP_PERMANENT;
const uint8 JSPROP_ENUMPERMRO	= JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_READONLY;
const uint8 JSPROP_ENUMPERMIDX	= JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_INDEX;

enum CSpell_Properties
{
	CSP_ID = 0,
	CSP_ACTION,
	CSP_DELAY,
	CSP_DAMAGEDELAY,
	CSP_RECOVERYDELAY,
	CSP_HEALTH,
	CSP_STAMINA,
	CSP_MANA,
	CSP_MANTRA,
	CSP_NAME,
	CSP_STRTOSAY,
	CSP_SCROLLLOW,
	CSP_SCROLLHIGH,
	CSP_CIRCLE,
	CSP_LOWSKILL,
	CSP_HIGHSKILL,
	CSP_GINSENG,
	CSP_MOSS,
	CSP_DRAKE,
	CSP_PEARL,
	CSP_SILK,
	CSP_ASH,
	CSP_SHADE,
	CSP_GARLIC,
	CSP_REQUIRETARGET,
	CSP_REQUIREITEM,
	CSP_REQUIRELOCATION,
	CSP_REQUIRECHAR,
	CSP_TRAVELSPELL,
	CSP_FIELDSPELL,
	CSP_REFLECTABLE,
	CSP_AGGRESSIVESPELL,
	CSP_RESISTABLE,
	CSP_SOUNDEFFECT,
	CSP_ENABLED,
	CSP_BASEDMG,
	CSP_COUNT
};

enum CGlobalSkill_Properties
{
	CGSKILL_NAME,
	CGSKILL_MADEWORD,
	CGSKILL_STRENGTH,
	CGSKILL_DEXTERITY,
	CGSKILL_INTELLIGENCE,
	CGSKILL_SKILLDELAY,
	CGSKILL_SCRIPTID,
	CGSKILL_COUNT
};

enum CCreateEntry_Properties
{
	CMAKE_ID = 0,
	CMAKE_NAME,
	CMAKE_ADDITEM,
	CMAKE_COLOUR,
	CMAKE_SOUND,
	CMAKE_MINRANK,
	CMAKE_MAXRANK,
	CMAKE_DELAY,
	CMAKE_SPELL,
	CMAKE_RESOURCES,
	CMAKE_SKILLS,
	CMAKE_AVGMINSKILL,
	CMAKE_AVGMAXSKILL,
	CMAKE_COUNT
};

enum CTimer_Properties
{
	// Character timers
	TIMER_TIMEOUT = 0,
	TIMER_INVIS,
	TIMER_HUNGER,
	TIMER_THIRST,
	TIMER_POISONTIME,
	TIMER_POISONTEXT,
	TIMER_POISONWEAROFF,
	TIMER_SPELLTIME,
	TIMER_SPELLRECOVERYTIME,
	TIMER_CRIMFLAG,
	TIMER_ANTISPAM,
	TIMER_MURDERRATE,
	TIMER_PEACETIMER,
	TIMER_FLYINGTOGGLE,
	TIMER_FIREFIELDTICK,
	TIMER_POISONFIELDTICK,
	TIMER_PARAFIELDTICK,
	TIMER_MOVETIME,
	TIMER_SPATIMER,
	TIMER_SUMMONTIME,
	TIMER_EVADETIME,
	TIMER_LOYALTYTIME,
	TIMER_LOGOUT,

	// Socket timers
	TIMER_SOCK_SKILLDELAY,
	TIMER_SOCK_OBJDELAY,
	TIMER_SOCK_SPIRITSPEAK,
	TIMER_SOCK_TRACKING,
	TIMER_SOCK_FISHING,
	TIMER_SOCK_MUTETIME,
	TIMER_SOCK_TRACKINGDISPLAY,
	TIMER_SOCK_TRAFFICWARDEN,
	TIMER_COUNT
};

enum CRace_Properties
{
	CRP_ID = 0,
	CRP_NAME,
	CRP_WEAKTOWEATHER,
	CRP_REQUIRESBEARD,
	CRP_REQUIRESNOBEARD,
	CRP_ISPLAYERRACE,
	CRP_GENDERRESTRICT,
	CRP_ARMOURCLASS,
	CRP_LANGUAGESKILLMIN,
	CRP_SKILLADJUSTMENT,
	CRP_POISONRESISTANCE,
	CRP_MAGICRESISTANCE,
	CRP_VISIBLEDISTANCE,
	CRP_NIGHTVISION,
	CRP_COUNT
};

enum CRegion_Properties
{
	CREGP_NAME = 0,
	CREGP_MAYOR,
	CREGP_RACE,
	CREGP_TAX,
	CREGP_TAXRESOURCE,
	CREGP_CANMARK,
	CREGP_CANRECALL,
	CREGP_CANGATE,
	CREGP_CANTELEPORT,
	CREGP_CANPLACEHOUSE,
	CREGP_ISGUARDED,
	CREGP_CANCASTAGGRESSIVE,
	CREGP_ISSAFEZONE,
	CREGP_HEALTH,
	CREGP_ISDUNGEON,
	CREGP_WORLDNUMBER,
	CREGP_INSTANCEID,
	CREGP_CHANCEBIGORE,
	CREGP_NUMOREPREFERENCES,
	CREGP_POPULATION,
	CREGP_MEMBERS,
	CREGP_ID,
	CREGP_SCRIPTTRIGGER,
	CREGP_SCRIPTTRIGGERS,
	CREGP_NUMGUARDS,
	CREGP_TAXES,
	CREGP_RESERVES,
	CREGP_APPEARANCE,
	CREGP_MUSIC,
	CREGP_WEATHER,
	CREGP_OWNER,
	CREGP_COUNT
};

enum CSpawnRegion_Properties
{
	CSPAWNREGP_NAME = 0,
	CSPAWNREGP_REGIONNUM,
	CSPAWNREGP_ITEMLIST,
	CSPAWNREGP_NPCLIST,
	CSPAWNREGP_ITEM,
	CSPAWNREGP_NPC,
	CSPAWNREGP_MAXITEMS,
	CSPAWNREGP_MAXNPCS,
	CSPAWNREGP_ITEMCOUNT,
	CSPAWNREGP_NPCCOUNT,
	CSPAWNREGP_ONLYOUTSIDE,
	CSPAWNREGP_ISSPAWNER,
	CSPAWNREGP_PREFZ,
	CSPAWNREGP_X1,
	CSPAWNREGP_Y1,
	CSPAWNREGP_X2,
	CSPAWNREGP_Y2,
	CSPAWNREGP_WORLD,
	CSPAWNREGP_INSTANCEID,
	CSPAWNREGP_MINTIME,
	CSPAWNREGP_MAXTIME,
	CSPAWNREGP_CALL,
	CSPAWNREGP_COUNT
};

enum CGuild_Properties
{
	CGP_NAME = 0,
	CGP_TYPE,
	CGP_MASTER,
	CGP_STONE,
	CGP_NUMMEMBERS,
	CGP_NUMRECRUITS,
	CGP_MEMBERS,
	CGP_RECRUITS,
	CGP_CHARTER,
	CGP_ABBREVIATION,
	CGP_WEBPAGE,
	CGP_COUNT
};

enum CC_Properties
{
	CCP_NAME = 0,
	CCP_TITLE,
	CCP_X,
	CCP_Y,
	CCP_Z,
	CCP_OLDX,
	CCP_OLDY,
	CCP_OLDZ,
	CCP_ID,
	CCP_COLOUR,
	CCP_CONTROLSLOTS,
	CCP_CONTROLSLOTSUSED,
	CCP_ORNERINESS,
	CCP_OWNER,
	CCP_VISIBLE,
	CCP_SERIAL,
	CCP_HEALTH,
	CCP_SCRIPTTRIGGER,
	CCP_SCRIPTTRIGGERS,
	CCP_WORLDNUMBER,
	CCP_INSTANCEID,
	CCP_TARGET,
	CCP_DEXTERITY,
	CCP_INTELLIGENCE,
	CCP_STRENGTH,
	CCP_ACTUALDEXTERITY,
	CCP_ACTUALINTELLIGENCE,
	CCP_ACTUALSTRENGTH,
	CCP_SKILLS,
	CCP_MANA,
	CCP_STAMINA,
	CCP_CHARPACK,
	CCP_FAME,
	CCP_KARMA,
	CCP_ATTACK,
	CCP_CANATTACK,
	CCP_BRKPEACE,
	CCP_SETPEACE,
	CCP_HUNGER,
	CCP_HUNGERRATE,
	CCP_THIRST,
	CCP_THIRSTRATE,
	CCP_FROZEN,
	CCP_COMMANDLEVEL,
	CCP_RACE,
	CCP_CRIMINAL,
	CCP_MURDERER,
	CCP_INNOCENT,
	CCP_MURDERCOUNT,
	CCP_GENDER,
	CCP_DEAD,
	CCP_NPC,
	CCP_AWAKE,
	CCP_ONLINE,
	CCP_DIRECTION,
	CCP_ISRUNNING,
	CCP_REGION,
	CCP_TOWN,
	CCP_GUILD,
	CCP_SKILLUSE,
	CCP_BASESKILLS,
	CCP_SOCKET,
	CCP_ISITEM,
	CCP_ISCHAR,
	CCP_MAXHP,
	CCP_MAXMANA,
	CCP_MAXSTAMINA,
	CCP_OLDWANDERTYPE,
	CCP_WANDERTYPE,
	CCP_ISONHORSE,
	CCP_ISFLYING,
	CCP_TDEXTERITY,
	CCP_TINTELLIGENCE,
	CCP_TSTRENGTH,
	CCP_POISON,
	CCP_LIGHTLEVEL,
	CCP_VULNERABLE,
	CCP_HUNGERSTATUS,
	CCP_THIRSTSTATUS,
	CCP_LODAMAGE,
	CCP_HIDAMAGE,
	CCP_FLAG,
	CCP_ATWAR,
	CCP_SPELLCAST,
	CCP_ISCASTING,
	CCP_PRIV,
	CCP_TOWNPRIV,
	CCP_GUILDTITLE,
	CCP_FONTTYPE,
	CCP_SAYCOLOUR,
	CCP_EMOTECOLOUR,
	CCP_ATTACKER,
	CCP_RACEGATE,
	CCP_SKILLLOCK,
	CCP_DEATHS,
	CCP_OWNERCOUNT,
	CCP_NEXTACT,
	CCP_PETCOUNT,
	CCP_OWNEDITEMSCOUNT,
	CCP_CELL,
	CCP_ALLMOVE,
	CCP_HOUSEICONS,
	CCP_ISSPAWNER,
	CCP_SPAWNSERIAL,
	CCP_SPATTACK,
	CCP_SPDELAY,
	CCP_AITYPE,
	CCP_SPLIT,
	CCP_SPLITCHANCE,
	CCP_TRAINER,
	CCP_HIRELING,
	CCP_WEIGHT,
	CCP_SQUELCH,
	CCP_ISJAILED,
	CCP_MAGICREFLECT,
	CCP_PERMMAGICREFLECT,
	CCP_TAMED,
	CCP_TAMEDHUNGERRATE,
	CCP_TAMEDTHIRSTRATE,
	CCP_HUNGERWILDCHANCE,
	CCP_THIRSTWILDCHANCE,
	CCP_FOODLIST,
	CCP_MOUNTED,
	CCP_STABLED,
	CCP_USINGPOTION,
	CCP_STEALTH,
	CCP_SKILLTOTAME,
	CCP_SKILLTOPROV,
	CCP_SKILLTOPEACE,
	CCP_POISONSTRENGTH,
	CCP_ISPOLYMORPHED,
	CCP_ISINCOGNITO,
	CCP_CANRUN,
	CCP_ISGUARDED,
	CCP_GUARDING,
	CCP_ISMEDITATING,
	CCP_ISGM,
	CCP_CANBROADCAST,
	CCP_SINGCLICKSER,
	CCP_NOSKILLTITLES,
	CCP_ISGMPAGEABLE,
	CCP_CANSNOOP,
	CCP_ISCOUNSELOR,
	CCP_NONEEDMANA,
	CCP_ISDISPELLABLE,
	CCP_NONEEDREAGS,
	CCP_ORGID,
	CCP_ORGSKIN,
	CCP_NPCFLAG,
	CCP_ISANIMAL,
	CCP_ISHUMAN,
	CCP_NEUTRAL,
	CCP_ISSHOP,
	CCP_ATTACKFIRST,
	CCP_MAXLOYALTY,
	CCP_LOYALTY,
	CCP_LOYALTYRATE,
	CCP_SHOULDSAVE,
	CCP_PARTYLOOTABLE,
	CCP_PARTY,
	CCP_MULTI,
	CCP_ACCOUNTNUM,
	CCP_ACCOUNT,
	CCP_CREATEDON,
	CCP_HOUSESOWNED,
	CCP_HOUSESCOOWNED,
	CCP_COUNT
};

enum CI_Properties
{
	CIP_NAME = 0,
	CIP_TITLE,
	CIP_X,
	CIP_Y,
	CIP_Z,
	CIP_OLDX,
	CIP_OLDY,
	CIP_OLDZ,
	CIP_ID,
	CIP_COLOUR,
	CIP_OWNER,
	CIP_VISIBLE,
	CIP_SERIAL,
	CIP_HEALTH,
	CIP_SCRIPTTRIGGER,
	CIP_SCRIPTTRIGGERS,
	CIP_WORLDNUMBER,
	CIP_INSTANCEID,
	CIP_AMOUNT,
	CIP_CONTAINER,
	CIP_TYPE,
	CIP_MORE,
	CIP_MOREX,
	CIP_MOREY,
	CIP_MOREZ,
	CIP_MOVABLE,
	CIP_ATT,
	CIP_LAYER,
	CIP_ITEMSINSIDE,
	CIP_TOTALITEMCOUNT,
	CIP_DECAYABLE,
	CIP_DECAYTIME,
	CIP_LODAMAGE,
	CIP_HIDAMAGE,
	CIP_AC,
	CIP_DEF,
	CIP_RESISTCOLD,
	CIP_RESISTHEAT,
	CIP_RESISTLIGHT,
	CIP_RESISTLIGHTNING,
	CIP_RESISTPOISON,
	CIP_RESISTRAIN,
	CIP_RESISTSNOW,
	CIP_DAMAGECOLD,
	CIP_DAMAGEHEAT,
	CIP_DAMAGELIGHT,
	CIP_DAMAGELIGHTNING,
	CIP_DAMAGEPOISON,
	CIP_DAMAGERAIN,
	CIP_DAMAGESNOW,
	CIP_NAME2,
	CIP_ISITEM,
	CIP_ISCHAR,
	CIP_RACE,
	CIP_MAXHP,
	CIP_MAXUSES,
	CIP_USESLEFT,
	CIP_RANK,
	CIP_CREATOR,
	CIP_POISON,
	CIP_DIR,
	CIP_ISSPAWNER,
	CIP_WIPABLE,
	CIP_BUYVALUE,
	CIP_SELLVALUE,
	CIP_RESTOCK,
	CIP_DIVINELOCK,
	CIP_WEIGHT,
	CIP_STRENGTH,
	CIP_DEXTERITY,
	CIP_INTELLIGENCE,
	CIP_CORPSE,
	CIP_DESC,
	CIP_EVENT,
	CIP_TEMPTIMER,
	CIP_SHOULDSAVE,
	// The following entries are specifically for CSpawnItem objects
	CIP_SPAWNSECTION,
	CIP_SECTIONALIST,
	CIP_MININTERVAL,
	CIP_MAXINTERVAL,

	CIP_ISNEWBIE,
	CIP_ISDISPELLABLE,
	CIP_MADEWITH,
	CIP_ENTRYMADEFROM,
	CIP_ISPILEABLE,
	CIP_ISDYEABLE,
	CIP_ISDAMAGEABLE,
	CIP_ISWIPEABLE,
	CIP_ISGUARDED,
	CIP_ISDOOROPEN,
	CIP_ISFIELDSPELL,
	CIP_ISLOCKEDDOWN,
	CIP_ISSHIELDTYPE,
	CIP_ISMETALTYPE,
	CIP_ISLEATHERTYPE,
	CIP_CANBELOCKEDDOWN,
	CIP_ISCONTTYPE,
	CIP_CARVESECTION,
	CIP_SPEED,
	CIP_MULTI,
	CIP_AMMOID,
	CIP_AMMOHUE,
	CIP_AMMOFX,
	CIP_AMMOFXHUE,
	CIP_AMMOFXRENDER,
	CIP_WEIGHTMAX,
	CIP_BASEWEIGHT,
	CIP_MAXITEMS,
	CIP_MAXRANGE,
	CIP_BASERANGE,
	CIP_REGION,
	CIP_SPAWNSERIAL,
	CIP_ISITEMHELD,

	CIP_LOCKDDOWNS,
	CIP_MAXLOCKDOWNS,
	CIP_TRASHCONTAINERS,
	CIP_MAXTRASHCONTAINERS,
	CIP_SECURECONTAINERS,
	CIP_MAXSECURECONTAINERS,
	CIP_FRIENDS,
	CIP_MAXFRIENDS,
	CIP_GUESTS,
	CIP_MAXGUESTS,
	CIP_OWNERS,
	CIP_MAXOWNERS,
	CIP_BANS,
	CIP_MAXBANS,
	CIP_VENDORS,
	CIP_MAXVENDORS,
	CIP_DEED,
	CIP_ISPUBLIC,
	CIP_BUILDTIMESTAMP,
	CIP_TRADETIMESTAMP,
	CIP_BANX,
	CIP_BANY,
	CIP_COUNT
};

enum CAccount_Properties
{

};

enum CSocket_Properties
{
	CSOCKP_ACCOUNT = 0,
	CSOCKP_CURRENTCHAR,
	CSOCKP_IDLETIMEOUT,
	CSOCKP_WASIDLEWARNED,
	CSOCKP_TEMPINT,
	CSOCKP_TEMPINT2,
	CSOCKP_BUFFER,
	CSOCKP_XTEXT,
	CSOCKP_CLICKZ,
	CSOCKP_ADDID,
	CSOCKP_NEWCLIENT,
	CSOCKP_FIRSTPACKET,
	CSOCKP_CRYPTCLIENT,
	CSOCKP_CLIENTIP,
	CSOCKP_WALKSEQUENCE,
	CSOCKP_CURRENTSPELLTYPE,
	CSOCKP_LOGGING,
	CSOCKP_BYTESSENT,
	CSOCKP_BYTESRECEIVED,
	CSOCKP_TARGETOK,
	CSOCKP_CLICKX,
	CSOCKP_CLICKY,
	CSOCKP_PICKUPX,
	CSOCKP_PICKUPY,
	CSOCKP_PICKUPZ,
	CSOCKP_PICKUPSPOT,
	CSOCKP_PICKUPSERIAL,
	CSOCKP_LANGUAGE,
	CSOCKP_CLIENTMAJORVER,
	CSOCKP_CLIENTMINORVER,
	CSOCKP_CLIENTSUBVER,
	CSOCKP_CLIENTLETTERVER,
	CSOCKP_CLIENTTYPE,
	CSOCKP_TARGET,
	CSOCKP_TEMPOBJ,
	CSOCKP_TEMPOBJ2,
	CSOCKP_COUNT,
};

enum CGumpData_Properties
{
	CGumpData_ID = 0 ,
	CGumpData_Button
};

enum CAccountProperties
{
	CACCOUNT_ID = 0,
	CACCOUNT_USERNAME,
	CACCOUNT_PASSWORD,
	CACCOUNT_FLAGS,
	CACCOUNT_PATH,
	CACCOUNT_COMMENT,
	CACCOUNT_TIMEBAN,
	CACCOUNT_FIRSTLOGIN,
	CACCOUNT_CHARACTER1,
	CACCOUNT_CHARACTER2,
	CACCOUNT_CHARACTER3,
	CACCOUNT_CHARACTER4,
	CACCOUNT_CHARACTER5,
	CACCOUNT_CHARACTER6,
	CACCOUNT_CHARACTER7,
	CACCOUNT_CURRENTCHAR,
	CACCOUNT_LASTIP,
	CACCOUNT_BANNED,
	CACCOUNT_SUSPENDED,
	CACCOUNT_PUBLIC,
	CACCOUNT_ONLINE,
	CACCOUNT_CHARSLOT1BLOCKED,
	CACCOUNT_CHARSLOT2BLOCKED,
	CACCOUNT_CHARSLOT3BLOCKED,
	CACCOUNT_CHARSLOT4BLOCKED,
	CACCOUNT_CHARSLOT5BLOCKED,
	CACCOUNT_CHARSLOT6BLOCKED,
	CACCOUNT_CHARSLOT7BLOCKED,
	CACCOUNT_UNUSED9,
	CACCOUNT_UNUSED10,
	CACCOUNT_SEER,
	CACCOUNT_COUNSELOR,
	CACCOUNT_GM
};

enum CConsoleProperties
{
	CCONSOLE_MODE = 0,
	CCONSOLE_LOGECHO,
	CCONSOLE_COUNT
};

enum CScriptSectionProperties
{
	CSS_NUMTAGS = 0,
	CSS_ATEND,
	CSS_ATENDTAGS,
	CSS_COUNT
};

enum CResourceProperties
{
	CRESP_LOGAMT = 0,
	CRESP_LOGTIME,
	CRESP_OREAMT,
	CRESP_ORETIME,
	CRESP_FISHAMT,
	CRESP_FISHTIME,
	CRESP_COUNT
};

enum CPartyProperties
{
	CPARTYP_LEADER,
	CPARTYP_MEMBERCOUNT,
	CPARTYP_ISNPC,
	CPARTYP_COUNT
};

#endif