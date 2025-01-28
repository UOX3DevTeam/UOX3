#ifndef __UOX3_ENUMS__
#define __UOX3_ENUMS__

enum DistinctLanguage
{
    DL_DEFAULT = 0,
    DL_ENGLISH,
    DL_GERMAN,
    DL_SPANISH,
    DL_FRENCH,
	DL_PORTUGUESE,
	DL_ITALIAN,
	DL_CZECH,
	DL_POLISH,
    DL_COUNT
};

enum UnicodeTypes
{
    ZERO = 0,
    UT_ARA,
    UT_ARI,
    UT_ARE,
    UT_ARL,
    UT_ARG,
    UT_ARM,
    UT_ART,
    UT_ARO,
    UT_ARY,
    UT_ARS,
    UT_ARJ,
    UT_ARB,
    UT_ARK,
    UT_ARU,
    UT_ARH,
    UT_ARQ,
    UT_BGR,
    UT_CAT,
    UT_CHT,
    UT_CHS,
    UT_ZHH,
    UT_ZHI,
    UT_ZHM,
    UT_CSY,
    UT_DAN,
    UT_DEU,
    UT_DES,
    UT_DEA,
    UT_DEL,
    UT_DEC,
    UT_ELL,
    UT_ENU,
    UT_ENG,
    UT_ENA,
    UT_ENC,
    UT_ENZ,
    UT_ENI,
    UT_ENS,
    UT_ENJ,
    UT_ENB,
    UT_ENL,
    UT_ENT,
    UT_ENW,
    UT_ENP,
    UT_ESP,
    UT_ESM,
    UT_ESN,
    UT_ESG,
    UT_ESC,
    UT_ESA,
    UT_ESD,
    UT_ESV,
    UT_ESO,
    UT_ESR,
    UT_ESS,
    UT_ESF,
    UT_ESL,
    UT_ESY,
    UT_ESZ,
    UT_ESB,
    UT_ESE,
    UT_ESH,
    UT_ESI,
    UT_ESU,
    UT_FIN,
    UT_FRA,
    UT_FRB,
    UT_FRC,
    UT_FRS,
    UT_FRL,
    UT_FRM,
    UT_HEB,
    UT_HUN,
    UT_ISL,
    UT_ITA,
    UT_ITS,
    UT_JPN,
    UT_KOR,
    UT_NLD,
    UT_NLB,
    UT_NOR,
    UT_NON,
    UT_PLK,
    UT_PTB,
    UT_PTG,
    UT_ROM,
    UT_RUS,
    UT_HRV,
    UT_SRL,
    UT_SRB,
    UT_SKY,
    UT_SQI,
    UT_SVE,
    UT_SVF,
    UT_THA,
    UT_TRK,
    UT_URP,
    UT_IND,
    UT_UKR,
    UT_BEL,
    UT_SLV,
    UT_ETI,
    UT_LVI,
    UT_LTH,
    UT_LTC,
    UT_FAR,
    UT_VIT,
    UT_HYE,
    UT_AZE,
    UT_EUQ,
    UT_MKI,
    UT_AFK,
    UT_KAT,
    UT_FOS,
    UT_HIN,
    UT_MSL,
    UT_MSB,
    UT_KAZ,
    UT_SWK,
    UT_UZB,
    UT_TAT,
    UT_BEN,
    UT_PAN,
    UT_GUJ,
    UT_ORI,
    UT_TAM,
    UT_TEL,
    UT_KAN,
    UT_MAL,
    UT_ASM,
    UT_MAR,
    UT_SAN,
    UT_KOK,
    TOTAL_LANGUAGES
};

enum SpeechType
{
    UNKNOWN			= -1,
    TALK			= 0,	// normal system message
    PROMPT			= 1,	// Display as system prompt
    EMOTE			= 2,	// : text
    SAY				= 3,	// character speaking
    OBJ				= 4,	// at object
    NOTHING			= 5,	// does not display
    SYSTEM			= 6,	// text labelling an item
    NOSCROLL		= 7,	// status msg, does not scroll
    WHISPER			= 8,	// only those close can here
    YELL			= 9,	// can be heard 2 screens away
    ASCIITALK		= 0xC0,	// ASCII version of TALK, all ASCII stuff is | 0xC0'd
    ASCIIPROMPT		= 0xC1,
    ASCIIEMOTE		= 0xC2,
    ASCIISAY		= 0xC3,
    ASCIIOBJ		= 0xC4,
    ASCIINOTHING	= 0xC5,
    ASCIISYSTEM		= 0xC6,
    ASCIINOSCROLL	= 0xC7,
    ASCIIWHISPER	= 0xC8,
    ASCIIYELL		= 0xC9,
    BROADCAST		= 0xFF
};

enum FontType
{
	FNT_NULL = -1,
    FNT_BOLD = 0,
    FNT_TEXT_WITH_SHADOW,
    FNT_BOLD_PLUS_SHADOW,
    FNT_NORMAL,
    FNT_GOTHIC,
    FNT_ITALIC,
    FNT_SMALL_DARK,
    FNT_COLOURFUL,
    FNT_RUNIC,		// Only use CAPS!
    FNT_SMALL_LIGHT,
	FNT_TEN,		// Unicode only
	FNT_ELEVEN,		// Unicode only
	FNT_TWELVE,		// Unicode only
    FNT_UNKNOWN
};

enum NameRequestSources
{
	NRS_SPEECH = 0,
	NRS_GUILD,
	NRS_STATWINDOW_SELF,
	NRS_STATWINDOW_OTHER,
	NRS_TOOLTIP,
	NRS_JOURNAL,
	NRS_PAPERDOLL,
	NRS_SINGLECLICK,
	NRS_SYSTEM,
	NRS_SECURETRADE,
	NRS_SCRIPT
};

enum CharacterDeletionResult
{
	CDR_BADPASSWORD = 0x00,
	CDR_CHARNOTFOUND,
	CDR_CHARISBUSY,
	CDR_CHARISTOOYOUNG,
	CDR_CHARISQUEUED,
	CDR_BADREQUEST,
	CDR_SUCCESS
};

enum PickupLocations
{
    PL_NOWHERE = 0,
    PL_GROUND,
    PL_OWNPACK,
    PL_OTHERPACK,
    PL_PAPERDOLL,
    PL_BANK
};

enum WeatherType
{
    NONE = 0,
    PHYSICAL,
    LIGHT,
    RAIN,
    COLD,
    HEAT,
    LIGHTNING,
    POISON,
    SNOW,
    STORM,
    STORMBREW,
    WEATHNUM
};

enum WorldType
{
    WRLD_SPRING = 0,
    WRLD_SUMMER,
    WRLD_AUTUMN,
    WRLD_WINTER,
    WRLD_DESOLATION,
    WRLD_UNKNOWN,
    WRLD_COUNT
};

// EXIT CODES
enum ErrorCodes
{
    FATAL_UOX3_SUCCESS=0,
    UNKNOWN_ERROR = 666,
    FATAL_UOX3_JAVASCRIPT = 50,
    FATAL_UOX3_ALLOC_WORLDSTATE = 100,
    FATAL_UOX3_ALLOC_MAPREGIONS,
    FATAL_UOX3_ALLOC_COMBAT,
    FATAL_UOX3_ALLOC_COMMANDS,
    FATAL_UOX3_ALLOC_GUILDS,
    FATAL_UOX3_ALLOC_ITEMS,
    FATAL_UOX3_ALLOC_MAP,
    FATAL_UOX3_ALLOC_NPCS,
    FATAL_UOX3_ALLOC_SKILLS,
    FATAL_UOX3_ALLOC_TOWNS,
    FATAL_UOX3_ALLOC_WEIGHT,
    FATAL_UOX3_ALLOC_NETWORK,
    FATAL_UOX3_ALLOC_MAGIC,
    FATAL_UOX3_ALLOC_RACES,
    FATAL_UOX3_ALLOC_WEATHER,
    FATAL_UOX3_ALLOC_MOVE,
    FATAL_UOX3_ALLOC_WHOLIST,
    FATAL_UOX3_ALLOC_DICTIONARY,
    FATAL_UOX3_ALLOC_BOOKS,
    FATAL_UOX3_ALLOC_PAGEVECTOR,
    FATAL_UOX3_ALLOC_ACCOUNTS,
    FATAL_UOX3_ALLOC_MAPREGION,
    FATAL_UOX3_ALLOC_TRIGGERS,
    FATAL_UOX3_ALLOC_SPEECHSYS,
    FATAL_UOX3_ALLOC_SCRIPTS,
    FATAL_UOX3_ALLOC_EFFECTS,
    FATAL_UOX3_ALLOC_HTMLTEMPLATES,
    FATAL_UOX3_ALLOC_JAILSYS,

    FATAL_UOX3_CHARS = 150,

    FATAL_UOX3_ITEMS = 200,

    FATAL_UOX3_ORELIST = 250,

    FATAL_UOX3_MAP_NOT_FOUND = 350,
    FATAL_UOX3_STATICS_INDEX_NOT_FOUND,
    FATAL_UOX3_STATICS_NOT_FOUND,
    FATAL_UOX3_VERSION_NOT_FOUND,
    FATAL_UOX3_TILEDATA_NOT_FOUND,
    FATAL_UOX3_MULTI_DATA_NOT_FOUND,
    FATAL_UOX3_MULTI_INDEX_NOT_FOUND,

    FATAL_UOX3_DIR_NOT_FOUND = 400,
    FATAL_UOX3_BAD_DEF_DICT
};

// List of skill numbers (For later implementation)
enum Skills
{
    ALCHEMY = 0,
    ANATOMY,
    ANIMALLORE,
    ITEMID,
    ARMSLORE,
    PARRYING,
    BEGGING,
    BLACKSMITHING,
    BOWCRAFT,
    PEACEMAKING,
    CAMPING,
    CARPENTRY,
    CARTOGRAPHY,
    COOKING,
    DETECTINGHIDDEN,
    ENTICEMENT,
    EVALUATINGINTEL,
    HEALING,
    FISHING,
    FORENSICS,
    HERDING,
    HIDING,
    PROVOCATION,
    INSCRIPTION,
    LOCKPICKING,
    MAGERY,
    MAGICRESISTANCE,
    TACTICS,
    SNOOPING,
    MUSICIANSHIP,
    POISONING,
    ARCHERY,
    SPIRITSPEAK,
    STEALING,
    TAILORING,
    TAMING,
    TASTEID,
    TINKERING,
    TRACKING,
    VETERINARY,
    SWORDSMANSHIP,
    MACEFIGHTING,
    FENCING,
    WRESTLING,
    LUMBERJACKING,
    MINING,
    MEDITATION,
    STEALTH,
    REMOVETRAP,
    NECROMANCY,
    FOCUS,
    CHIVALRY,
    BUSHIDO,
    NINJITSU,
    SPELLWEAVING,
    MYSTICISM,
    IMBUING,
    THROWING,

    ALLSKILLS, // #skills+1

    STRENGTH, // #skills+2
    DEXTERITY, // #skills+3
    INTELLECT, // #skills+4
    FAME, // #skills+5
    KARMA,  // #skills+6
    SKILLS     // #skills+7
};

// Values for the skill locks
enum SkillLock
{
    SKILL_INCREASE = 0,
    SKILL_DECREASE,
    SKILL_LOCKED
};

enum ObjectType
{
    OT_CBO = 0,
    OT_CHAR,
    OT_ITEM,
    OT_MULTI,
    OT_BOAT,
    OT_SPAWNER,
};

enum DEFINITIONCATEGORIES
{
    items_def		=	0,
    npc_def,
    create_def,
    regions_def,
    misc_def,
    skills_def,
    location_def,
    menus_def,
    spells_def,
    newbie_def,
    titles_def,
    advance_def,
    house_def,
    colors_def,
    spawn_def,
    html_def,
    race_def,
    weathab_def,
    hard_items_def,
    command_def,
    msgboard_def,
    carve_def,
    creatures_def,
    maps_def,
    NUM_DEFS
};

enum CommandLevels
{
    CL_PLAYER = 0,
    CL_CNS,
    CL_GM,
    CL_ADMIN
};

enum AITypes
{
    AI_NONE			= 0,
    AI_HEALER_G		= 1,
    AI_EVIL			= 2,
    AI_GUARD		= 4,
    AI_FIGHTER		= 5,
    AI_ANIMAL		= 6,
    AI_DUMMY		= 7,
    AI_BANKER		= 8, // handled in JS, but needed to identify NPC for context menus
	AI_STABLEMASTER = 9, // handled in JS, but needed to identify NPC for context menus
	AI_CASTER		= 10,
	AI_EVIL_CASTER  = 11,
	AI_ANIMAL_SCARED = 12,
    AI_PLAYERVENDOR	= 17,
    AI_PET_GUARD	= 32,
    AI_CHAOTIC		= 88,
    AI_HEALER_E		= 666
};


enum distLocs
{
    DIST_SAMETILE	= 1,
    DIST_NEXTTILE,
    DIST_NEARBY,
    DIST_INRANGE	= 7,
	DIST_CMDRANGE	= 12,
    DIST_SAMESCREEN = 24, // 24 is max in clients (enhanced/classic) v7.0.55.27 and beyond, and in ClassicUO
	DIST_BUILDRANGE = 31,
	DIST_COMBATRESETRANGE = 35,
    DIST_OUTOFRANGE	= 0xFFFF
};

enum SaveStatus
{
    SS_NOTSAVING = 0,
    SS_SAVING,
    SS_JUSTSAVED
};

enum ItemTypes
{
    IT_NOTYPE 				= 0,
    IT_CONTAINER			= 1,
    IT_CASTLEGATEOPENER		= 2,
    IT_CASTLEGATE			= 3,
    IT_TELEPORTITEM			= 6,
    IT_KEY 					= 7,
    IT_LOCKEDCONTAINER 		= 8,
    IT_SPELLBOOK 			= 9,
    IT_MAP 					= 10,
    IT_BOOK					= 11,
    IT_DOOR					= 12,
    IT_LOCKEDDOOR			= 13,
    IT_FOOD					= 14,
    IT_MAGICWAND			= 15,
    IT_RESURRECTOBJECT		= 16,
    IT_CRYSTALBALL			= 18,
    IT_POTION				= 19,
    IT_TRADEWINDOW			= 20,
    IT_TOWNSTONE			= 35,
	IT_RUNEBOOK				= 49,
	IT_RECALLRUNE			= 50,
	IT_GATE					= 51,
    IT_OBJTELEPORTER		= 60,
    IT_ITEMSPAWNER			= 61,
    IT_NPCSPAWNER			= 62,
    IT_SPAWNCONT 			= 63,
    IT_LOCKEDSPAWNCONT 		= 64,
    IT_UNLOCKABLESPAWNCONT 	= 65,
    IT_AREASPAWNER			= 69,
    IT_ADVANCEGATE			= 80,
    IT_MULTIADVANCEGATE		= 81,
    IT_MONSTERGATE			= 82,
    IT_RACEGATE				= 83,
    IT_DAMAGEOBJECT			= 85,
    IT_TRASHCONT 			= 87,
    IT_SOUNDOBJECT			= 88,
    IT_MAPCHANGEOBJECT		= 89,
    IT_WORLDCHANGEGATE		= 90,
    IT_MORPHOBJECT			= 101,
    IT_UNMORPHOBJECT		= 102,
    IT_DRINK				= 105,
    IT_STANDINGHARP			= 106,
	IT_SHIELD				= 107,
    IT_ZEROKILLSGATE		= 111,
    IT_PLANK				= 117,
    IT_FIREWORKSWAND		= 118,
	IT_SPELLCHANNELING		= 119,
    IT_ESCORTNPCSPAWNER		= 125,
    IT_RENAMEDEED			= 186,
    IT_LEATHERREPAIRTOOL	= 190,
    IT_BOWREPAIRTOOL		= 191,
    IT_TILLER				= 200,
	IT_HOUSEADDON			= 201,
    IT_GUILDSTONE			= 202,
    IT_HOUSESIGN			= 203,
    IT_TINKERTOOL			= 204,
    IT_METALREPAIRTOOL		= 205,
    IT_FORGE				= 207,
    IT_DYE					= 208,
    IT_DYEVAT				= 209,
    IT_MODELMULTI			= 210,
    IT_ARCHERYBUTTE			= 211,
    IT_DRUM					= 212,
    IT_TAMBOURINE			= 213,
    IT_HARP					= 214,
    IT_LUTE					= 215,
    IT_AXE					= 216,
    IT_PLAYERVENDORDEED		= 217,
    IT_SMITHYTOOL			= 218,
    IT_CARPENTRYTOOL		= 219,
    IT_MININGTOOL			= 220,
    IT_EMPTYVIAL			= 221,
    IT_UNSPUNFABRIC			= 222,
    IT_UNCOOKEDFISH			= 223,
    IT_UNCOOKEDMEAT			= 224,
    IT_SPUNFABRIC			= 225,
    IT_FLETCHINGTOOL		= 226,
    IT_CANNONBALL			= 227,
    IT_WATERPITCHER			= 228,
    IT_UNCOOKEDDOUGH		= 229,
    IT_SEWINGKIT			= 230,
    IT_ORE					= 231,
    IT_MESSAGEBOARD			= 232,
    IT_SWORD				= 233,
    IT_CAMPING				= 234,
    IT_MAGICSTATUE			= 235,
    IT_GUILLOTINE			= 236,
    IT_FLOURSACK			= 238,
    IT_OPENFLOURSACK		= 239,
    IT_FISHINGPOLE			= 240,
    IT_CLOCK				= 241,
    IT_MORTAR				= 242,
    IT_SCISSORS				= 243,
    IT_BANDAGE				= 244,
    IT_SEXTANT				= 245,
    IT_HAIRDYE				= 246,
    IT_LOCKPICK				= 247,
    IT_COTTONPLANT			= 248,
    IT_TINKERAXLE			= 249,
    IT_TINKERAWG			= 250,
    IT_TINKERCLOCK			= 251,
    IT_TINKERSEXTANT		= 252,
    IT_TRAININGDUMMY		= 253,
	IT_PETTRANSFERDEED		= 254,
    IT_COUNT				= 255
};

enum ItemLayers
{
    IL_NONE					= 0x00,
    IL_RIGHTHAND			= 0x01,
    IL_LEFTHAND				= 0x02,
    IL_FOOTWEAR				= 0x03,
    IL_PANTS				= 0x04,
    IL_INNERSHIRT			= 0x05,
    IL_HELM					= 0x06,
    IL_GLOVES				= 0x07,
    IL_RING					= 0x08,
    IL_TALISMAN				= 0x09,
    IL_NECK					= 0x0A,
    IL_HAIR					= 0x0B,
    IL_WAIST				= 0x0C,
    IL_TUNIC				= 0x0D,
    IL_BRACELET				= 0x0E,
    IL_FACE					= 0x0F,
    IL_FACIALHAIR			= 0x10,
    IL_OUTERSHIRT			= 0x11,
    IL_EARRINGS				= 0x12,
    IL_ARMS					= 0x13,
    IL_CLOAK				= 0x14,
    IL_PACKITEM				= 0x15,
    IL_ROBE					= 0x16,
    IL_OUTERLEGGINGS		= 0x17,
    IL_INNERLEGGINGS		= 0x18,
    IL_MOUNT				= 0x19,
    IL_SELLCONTAINER		= 0x1A,
    IL_BOUGHTCONTAINER		= 0x1B,
    IL_BUYCONTAINER			= 0x1C,
    IL_BANKBOX				= 0x1D,
    IL_UNUSED				= 0x1E, // turns player invisible when used?
    IL_SECURETRADE			= 0x1F
};

enum TargetIds
{
    TARGET_ADDSCRIPTNPC = 0,
    TARGET_TELE,
    TARGET_DYE,
    TARGET_KEY,
    TARGET_REPAIRMETAL,
    TARGET_DYEALL,
    TARGET_DVAT,
    TARGET_FREEZE,
    TARGET_UNFREEZE,
    TARGET_ITEMID,
    TARGET_FISH,
    TARGET_INFO,
    TARGET_TOWNALLY,
    TARGET_SMITH,
    TARGET_MINE,
    TARGET_SMELTORE,
    TARGET_WSTATS,
    TARGET_NPCRESURRECT,
    TARGET_TWEAK,
    TARGET_MAKESTATUS,
    TARGET_SETSCPTRIG,
    TARGET_CASTSPELL,
    TARGET_FOLLOW,
    TARGET_ATTACK,
    TARGET_TRANSFER,
    TARGET_GUARD,
    TARGET_RESTOCK,
    TARGET_FRIEND,
    TARGET_VOTEFORMAYOR,
    TARGET_VIAL,
    TARGET_TILING,
    TARGET_STEALING,
    TARGET_PLVBUY,
    TARGET_HOUSEOWNER,
    TARGET_HOUSEEJECT,
    TARGET_HOUSEBAN,
    TARGET_HOUSEFRIEND,
    TARGET_HOUSEUNLIST,
    TARGET_HOUSELOCKDOWN,
    TARGET_HOUSERELEASE,
	TARGET_HOUSESECURE,
    TARGET_SMELT,
    TARGET_SHOWSKILLS,
    TARGET_BUILDHOUSE,
    TARGET_PARTYADD,
    TARGET_PARTYREMOVE,
	TARGET_HOUSETRASHBARREL,
	TARGET_HOUSESTRONGBOX,
	TARGET_REMOVEFRIEND,
    TARGET_NOFUNC
};

enum VisibleTypes
{
    VT_VISIBLE			= 0,	// Visible to All
    VT_TEMPHIDDEN,				// Hidden (With Skill) / Visible To Item Owner
    VT_INVISIBLE,				// Magically Hidden
    VT_PERMHIDDEN,				// Permanent Hidden (GM Hidden)
    VT_GHOSTHIDDEN				// Hidden because the player is a ghost
};

enum monsterSound
{
    SND_STARTATTACK = 0,
    SND_IDLE,
    SND_ATTACK,
    SND_DEFEND,
    SND_DIE,
    SND_COUNT
};

enum BloodTypes
{
	BLOOD_DEATH = 0,
	BLOOD_BLEED,
	BLOOD_CRITICAL,
};

enum FlagColors
{
    FC_INNOCENT = 1,
    FC_FRIEND,
    FC_NEUTRAL,
    FC_CRIMINAL,
    FC_ENEMY,
    FC_MURDERER,
    FC_INVULNERABLE
};

enum WanderTypes
{
    WT_NONE = 0,
    WT_FOLLOW,
    WT_FREE,
    WT_BOX,
    WT_CIRCLE,
    WT_FROZEN,
    WT_FLEE,
    WT_PATHFIND,
	WT_SCARED,
    WT_COUNT
};

enum BoatMoveType
{
	BOAT_ANCHORED = -1,
	BOAT_STOP,
	BOAT_FORWARD,
	BOAT_BACKWARD,
	BOAT_LEFT,
	BOAT_RIGHT,
	BOAT_FORWARDLEFT,
	BOAT_FORWARDRIGHT,
	BOAT_BACKWARDLEFT,
	BOAT_BACKWARDRIGHT,
	BOAT_SLOWLEFT,
	BOAT_SLOWRIGHT,
	BOAT_SLOWFORWARD,
	BOAT_SLOWBACKWARD,
	BOAT_SLOWFORWARDLEFT,
	BOAT_SLOWFORWARDRIGHT,
	BOAT_SLOWBACKWARDRIGHT,
	BOAT_SLOWBACKWARDLEFT,
	BOAT_ONELEFT,
	BOAT_ONERIGHT,
	BOAT_ONEFORWARD,
	BOAT_ONEBACKWARD,
	BOAT_ONEFORWARDLEFT,
	BOAT_ONEFORWARDRIGHT,
	BOAT_ONEBACKWARDLEFT,
	BOAT_ONEBACKWARDRIGHT
};

enum RaceRelate
{
    RACE_ENEMY = -1,
    RACE_NEUTRAL = 0,
    RACE_ALLY = 1
};

enum BodyType
{
	BT_HUMAN = 0,
	BT_ELF,
	BT_GARGOYLE,
	BT_OTHER,
	BT_COUNT
};

enum ExpansionRuleset
{
	ER_CORE,
	ER_UO,
	ER_T2A,
	ER_UOR,
	ER_TD,
	ER_LBR,
	ER_AOS,
	ER_SE,
	ER_ML,
	ER_SA,
	ER_HS,
	ER_TOL,
	ER_COUNT
};

enum Actions // Pre-v7.0.0.0
{
	ACT_WALK_UNARMED		= 0x00,

	// Humans, elves
	ACT_WALK_ARMED			= 0x01,	// Walk with items equipped, while not in war mode
	ACT_RUN_UNARMED			= 0x02,	// Run without items equipped in hands
	ACT_RUN_ARMED			= 0x03,	// Run with items equipped in hands
	ACT_IDLE				= 0x04,	// Stand around idle
	ACT_IDLE_LOOK			= 0x05,	// Idle, looking around
	ACT_IDLE_YAWN			= 0x06,	// Idle, yawning
	ACT_IDLE_COMBAT_1H		= 0x07,	// Idle, in combat, with 1H weapon equipped
	ACT_IDLE_COMBAT_2H		= 0x08,	// Idle, in combat, with 2H weapon equipped
	ACT_ATT_1H_SLASH		= 0x09, // Swing using 1H blade weapon
	ACT_ATT_1H_PIERCE		= 0x0A, // Stab using 1H piercing weapon
	ACT_ATT_1H_BASH			= 0x0B, // Bash using 1H mace weapon
	ACT_ATT_2H_BASH			= 0x0C, // Bash using 2H mace weapon
	ACT_ATT_2H_SLASH		= 0x0D, // Slash using 2H blade weapon
	ACT_ATT_2H_PIERCE		= 0x0E, // Stab using 2H piercing weapon
	ACT_WALK_COMBAT			= 0x0F,	// Walk in combat mode
	ACT_SPELL_TARGET		= 0x10,	// Cast spell at target
	ACT_SPELL_AREA			= 0x11,	// Cast spell at area
	ACT_ATT_BOW				= 0x12,	// Fire bow
	ACT_ATT_XBOW			= 0x13,	// Fire crossbow
	ACT_IMPACT				= 0x14,	// Get hit
	ACT_DIE_BACKWARD		= 0x15,	// Die, falling backwards
	ACT_DIE_FORWARD			= 0x16,	// Die, falling forward
	ACT_BLOCK				= 0x1E,	// Block attack, or evade
	ACT_ATT_WRESTLE			= 0x1F,	// Unarmed attack using fists
	ACT_EMOTE_BOW			= 0x20, // Bow
	ACT_EMOTE_SALUTE		= 0x21, // Salute
	ACT_EMOTE_EAT			= 0x22, // Eat or drink

	// Mounted
	ACT_MOUNT_RIDE_WALK		= 0x17, // Walking while mounted
	ACT_MOUNT_RIDE_RUN		= 0x18, // Running while mounted
	ACT_MOUNT_IDLE			= 0x19, // Idle while mounted
	ACT_MOUNT_ATT_1H		= 0x1A, // Attacking with 1H weapon while mounted
	ACT_MOUNT_ATT_BOW		= 0x1B, // Attacking with Bow while mounted
	ACT_MOUNT_ATT_XBOW		= 0x1C, // Attacking with Crossbow while mounted
	ACT_MOUNT_ATT_2H		= 0x1D // Attacking with 2H weapon while mounted
};

enum ActionsMonsters
{
	ACT_MONSTER_WALK 		= 0x00, // Walking animation
	ACT_MONSTER_IDLE		= 0x01, // Idle animation
	ACT_MONSTER_DIE_1		= 0x02,	// Backward
	ACT_MONSTER_DIE_2		= 0x03, // Death, forward or to the side
	ACT_MONSTER_ATT_K1		= 0x04,	// Attack variation 1 - ALL creatures have at least this attack
	ACT_MONSTER_ATT_2		= 0x05,	// Attack variation 2 - swimming monsteers don't have this
	ACT_MONSTER_ATT_3		= 0x06, // Attack variation 3
	ACT_MONSTER_ATT_BOW		= 0x07, // Attack variation 4 - air/fire elem = flail arms
	ACT_MONSTER_ATT_XBOW	= 0x08,	// Attack variation 5 - Misc Roll over
	ACT_MONSTER_ATT_THROW	= 0x09, // Attack variation 6 - Throw
	ACT_MONSTER_IMPACT 		= 0x0A, // Get hit animation
	ACT_MONSTER_PILLAGE		= 0x0B,	// 11 = Misc, Stomp, slap ground, lich conjure
	ACT_MONSTER_CAST_1		= 0x0C,	// Spellcast variation 1 - Misc cast, breath fire, elem creation
	ACT_MONSTER_CAST_2		= 0x0D,	// Spellcast variation 2 - Trolls don't have this anim
	ACT_MONSTER_CAST_3		= 0x0E, // Spellcast variation 3
	ACT_MONSTER_BLOCKRIGHT	= 0x0f, // Block right
	ACT_MONSTER_BLOCKLEFT	= 0x10, // Block left
	ACT_MONSTER_IDLE_1		= 0x11,	// Idle Variation 1
	ACT_MONSTER_IDLE_2		= 0x12,	// Idle Variation 2
	ACT_MONSTER_FLY			= 0x13, // Take off
	ACT_MONSTER_LAND		= 0x14,	// Land
	ACT_MONSTER_DIE_FLIGHT	= 0x15	// Die in mid air
};

enum ActionsAnimals
{
	ACT_ANIMAL_WALK			= 0x00,
	ACT_ANIMAL_RUN			= 0x01,
	ACT_ANIMAL_IDLE	    	= 0x02, // Default idle animation
	ACT_ANIMAL_EAT			= 0x03, // Eat/Graze on food
	ACT_ANIMAL_ALERT        = 0x04,	// Look alert and awake. Not all animals have this.
	ACT_ANIMAL_ATTACK_1		= 0x05,
	ACT_ANIMAL_ATTACK_2		= 0x06,
	ACT_ANIMAL_GETHIT 		= 0x07,
	ACT_ANIMAL_DIE_1 		= 0x08, // 
	ACT_ANIMAL_IDLE_1		= 0x09,	// Idle variation 1
	ACT_ANIMAL_IDLE_2		= 0x0a, // Idle variation 2
	ACT_ANIMAL_SLEEP		= 0x0b,	// Lie down and sleep (not all animals have this)
	ACT_ANIMAL_DIE_2		= 0x0c
};

enum NewActions // v7.0.0.0+
{
	N_ACT_ATT		= 0x00, // Variations listed under SubActionsAttack enum
	N_ACT_BLOCK		= 0x01,
	N_ACT_BLOCK2	= 0x02,
	N_ACT_DIE		= 0x03, // Variations listed under SubActionsDeath enum
	N_ACT_IMPACT	= 0x04,
	N_ACT_IDLE		= 0x05, // One variation available (shuffle feet) with subSubAction of 0x01?
	N_ACT_EAT		= 0x06,
	N_ACT_EMOTE		= 0x07, // Variations listed under SubActionsEmote enum
	N_ACT_ANGER		= 0x08,
	N_ACT_TAKEOFF	= 0x09,
	N_ACT_LAND		= 0x0A,
	N_ACT_SPELL		= 0x0B, // Variations listed under SubActionsSpells enum
	N_ACT_UNKNOWN1	= 0x0C, // Enhanced Client only?
	N_ACT_UNKNOWN2	= 0x0D, // Enhanced Client only?
	N_ACT_PILLAGE	= 0x0E, // Eat, pillage
	N_ACT_RISE		= 0x0F // Used for character creation in Enhanced Client
};

enum SubActionsAttack // Variations for attack animation, v7.0.0.0+
{
	S_ACT_WRESTLE	= 0x00,
	S_ACT_BOW 		= 0x01,
	S_ACT_XBOW		= 0x02,
	S_ACT_1H_BASH	= 0x03,
	S_ACT_1H_SLASH	= 0x04,
	S_ACT_1H_PIERCE	= 0x05,
	S_ACT_2H_BASH	= 0x06,
	S_ACT_2H_SLASH	= 0x07,
	S_ACT_2H_PIERCE	= 0x08,
	S_ACT_THROW		= 0x09
};

enum SubActionsDeath // Variations for death animation, v7.0.0.0+
{
	S_ACT_DEATH_FORWARD		= 0x00,
	S_ACT_DEATH_BACKWARD	= 0x01
};

enum SubActionsEmote // Variations for emote animation, v7.0.0.0+
{
	S_ACT_EMOTE_BOW		= 0x00,
	S_ACT_EMOTE_SALUTE	= 0x01
};

enum SubActionsSpells // Variations for spellcast animation, v7.0.0.0+
{
	S_ACT_SPELL_TARGET	= 0x00,
	S_ACT_SPELL_AREA	= 0x01
};
#endif

