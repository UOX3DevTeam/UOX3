#ifndef __UOXTYPES_H
#define __UOXTYPES_H

#include "enums.h"

#ifdef __NEED_VALIST__
	typedef void *  va_list;
#endif

#ifdef __LINUX__

	#define MAKEWORD(a, b)      ((UI16)(((UI08)(a)) | ((UI16)((UI08)(b))) << 8))
	#define MAKELONG(a, b)      ((UI32)(((UI16)(a)) | ((UI32)((UI16)(b))) << 16))
	#define LOWORD(l)           ((UI16)(l))
	#define HIWORD(l)           ((UI16)(((UI32)(l) >> 16) & 0xFFFF))
	#define LOBYTE(w)           ((UI08)(w))
	#define HIBYTE(w)           ((UI08)(((UI16)(w) >> 8) & 0xFF))

#endif

#define MaxZstep 9

#define MAX_LASTON			30
#define MAX_NAME			30
#define MAX_PASSWORD		30
#ifdef __LINUX__
	#define MAX_PATH		512
	#define INVALID_SOCKET  (~0)
	#define SOCKET_ERROR	(-1)
#endif
#define MAX_COMMENT			128
#define MAX_TITLE			50
#define MAX_TRIGWORD		31
#define MAX_GUILDTITLE		21

#ifndef __LINUX__			// changed from __64BIT__, Tseramed

typedef double				R64;
typedef float				R32;
typedef unsigned long int	UI32;
typedef signed long int		SI32;
typedef unsigned short int	UI16;
typedef signed short int	SI16;
typedef unsigned char		UI08;
typedef signed char			SI08;

#else

typedef double				R64;
typedef float				R32;
typedef unsigned int		UI32;
typedef signed int			SI32;
typedef unsigned short int	UI16;
typedef signed short int	SI16;
typedef unsigned char		UI08;
typedef signed char			SI08;

#endif

typedef UI32		TIMERVAL;
typedef SI32		UOXSOCKET;
typedef SI32		PLAYER;
typedef SI32		CHARACTER;
typedef UI32		RACEID;
typedef UI32		GENDER;
typedef UI16		COLOR;
typedef UI08		LIGHTLEVEL;
typedef UI08		SECONDS;
typedef UI08		ARMORCLASS;
typedef SI32		ITEM;
typedef UI32		SERIAL;
typedef UI32		RACEREL;
typedef SI32		SKILLVAL;
typedef SI08		RANGE;
typedef UI08		weathID;

#ifdef __LINUX__
	typedef UI32		ULONG;
	typedef SI32		LONG;
	typedef UI32		UINT;
	typedef SI32		INT;
	typedef UI08		UCHAR;
	typedef SI08		CHAR;
	typedef CHAR		TCHAR;
	typedef UI08		BYTE;
#endif

typedef vector< string >	stringList;

const UI32 INVALIDSERIAL = 0xFFFFFFFF;
class CChar;
typedef SI16 GuildID;

typedef vector< CChar *> VCCHAR;
const UI08 MAXVAL = 0;
const UI08 MINVAL = 1;
const UI08 CURRVAL = 2;
const UI08 TEMP = 0;
const UI08 WIND = 1;
const UI08 BEARD = 1;
const UI08 HAIR = 2;
const UI08 SKIN = 3;
const UI08 MALE = 1;
const UI08 FEMALE = 2;

#define END					-1			// void bldstr(...);


#define SPELL_MAX 68 //:Terrin: use define for now; can make autocount later
#define MAXSCRIPTLINE 1024
#define MAXFILESPEC 2048
#define NONWHITESPACE "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789~!@#$%^&*()_+{}|?/><'.,"
#define RANGEINDICATOR "#"
#define WHITESPACE " "
#define VALID "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890!@#$%^&*+_,(){}[]<>?|"
#define GOLD_WEIGHT 0.005 //what a goldpiece weighs this is in hundreths of a stone!

#define MAXIMUM 128 // Maximum open connections to server
#define MAXCLIENT MAXIMUM+1
#define MAXACCT 2000 // Maximum accounts
#define MAXSERV 32 // Maximum servers in login listing
#define MAXSTART 64 // Maximum starting locations
#define MAXBUFFER 2560 // Buffer Size (For socket operations)
#define MAXPAGES 20 // Maximum number of pages in the GM queue
//#define VISRANGE 18 // Visibility for normal items
#define BUILDRANGE 31 // Visibility for castles and keeps

#define XYMAX 256 // Maximum items UOX can handle on one X/Y square
#define MAXLAYERS 40 // Maximum number of Layers in paperdolls (still not sure how many)
#define ITEMMENUOFFSET 256
#define CMAX 40 // Maximum parameters in one line (Only for memory reasons)

#define TILEDATA_TILES 0x68800
#define REGENRATE1 8 // Seconds to heal ONE hp
#define REGENRATE2 3 // Seconds to heal ONE stm
#define REGENRATE3 5 // Seconds to heal ONE mn
#define SPIRITSPEAKTIMER 30 // spiritspeak lasts at least 30 seconds, additional time is calculated by Skill & INT
#define SHOWLOGGEDOUTPCS 1 // Melen 9/7/99

#define RELEASED
#define NODEBUG
#define NOSPECIAL
#define CRYPT
#define NOLOGALL

// For Combat ONLY

#define SKILL 0
#define WEAP 1

#define ARROW 1
#define BOLT 2

#define BARE_HANDS_SPEED 8
//////////////////
// List of monster sounds
#define SND_STARTATTACK 0
#define SND_IDLE 1
#define SND_ATTACK 2
#define SND_DEFEND 3
#define SND_DIE 4

//////////////////
// Skill & Stat cap - AntiChrist
#define STATCAP 999
#define SKILLCAP 999999

typedef __TIMERID__ TID;

#define TRUESKILLS ALLSKILLS //#skills+1


#define DOORTYPES 19
#define WEIGHT_PER_STR 4
#define MAXGATES 100
#define GATETIMER 30
#define MINECHECK 2 
#define DECAYTIMER 300  // 5 minute decay rate
#define PLAYERCORPSEDECAYMULTIPLIER 3 // JM - Player's corpse decays 48x the decay rate
#define LOOTDECAYSWITHCORPSE 1 // JM - 0 Loot disappears with corpse, 1 loot gets left when corpse decays
#define INVISTIMER 60 // invisibility spell lasts 1 minute
#define HUNGERRATE 6000 // 15 minutes
#define HUNGERDAMAGERATE 10 //every 10 seconds
#define HUNGERDAMAGE 2 //-2 hp
#define POISONTIMER 180 // Lord Binary
#define POTIONDELAY 10
#define NPCSPEED 0.5	// 2 steps every second, ab said to change this to 0.5 - fur
#define SKILLDELAY 5   // Skill usage delay     (5 seconds)
#define OBJECTDELAY 1
#define SHOWDEATHANIM 1
#define COMBATHITMESSAGE 1
#define USESPECIALBANK 0 // AntiChrist - don't use special bank as default
#define SPECIALBANKTRIGGER "WARE" // AntiChrist - default word trigger for special bank

#define ATTACKSTAMINA -2 // AntiChrist - attacker loses stamina when hits
#define MONSTERS_VS_ANIMALS 0 // By default monsters won't attack animals
#define ANIMALS_ATTACK_CHANCE 15 // By default 15% of chance to attack an animal
#define ANIMALS_GUARDED 0 // By default players can attack animals without summoning guards
#define MAX_ABSORBTION 20 // 20 Arm (single armour piece) -- Magius(CHE)
#define MAX_NON_HUMAN_ABSORBTION 100 // 100 Arm (total armour) -- Magius(CHE)
#define NPC_DAMAGE_RATE 2 // DAMAGE / NPC_DAMAGE_RATE for NPCs -- Magius(CHE)
#define NPC_BASE_FLEEAT 20
#define NPC_BASE_REATTACKAT 40
#define SELLBYNAME 1 // Values = 0(Disabled) or 1(Enabled) - The NPC Vendors buy from you if your item has the same name of his item! --- Magius(CHE)
#define SKILLLEVEL 5 // Range from 1 to 10 - This value if the difficult to create an item using a make command: 1 is easy, 5 is normal (default), 10 is difficult! - Magius(CHE)
#define SELLMAXITEM 5 // Set maximum amount of items that one player can sell at a time ( 5 is standard OSI ) --- Magius(CHE)
#define RANKSYSTEM 1 // Rank sstem to make various type of a single item based on the creator's skill! - Magius(CHE)

#define MAP0CACHE 300

#define MAKEMENUOFFSET 2048
#define TRACKINGMENUOFFSET 4096 // menu offset
#define TRACKINGRANGE 10        // tracking range is at least TRACKINGRANGE, additional distance is calculated by Skill,INT
#define MAXTRACKINGTARGETS 20   // maximum number of trackable targets
#define TRACKINGTIMER 30        // tracking last at least TRACKINGTIMER seconds, additional time is calculated by Skill,INT, & DEX
#define TRACKINGDISPLAYTIME 30  // tracking will display above the character every TRACKINGDISPLAYTIME seconds
#define POLYMORPHMENUOFFSET 8192 // polymorph spell menu offset - AntiChrist (9/99)

#define BEGGINGRANGE 3                  // begging range
#define FISHINGTIMEBASE 10              // it takes at least FISHINGTIMEBASE sec to fish
#define FISHINGTIMER 5
// for spinning wheel -vagrant
#define YARN 0
#define THREAD 1
// Structure Definitions
// Dupois - for fileArchive()
// Added Oct 20, 1998
#define MAXARCHID 14

// Line Of Sight
#define ITEM_TYPE_CHOICES 6
#define TREES_BUSHES 1 // Trees and other large vegetaion in the way
#define WALLS_CHIMNEYS 2  // Walls, chimineys, ovens, etc... in the way
#define DOORS 4 // Doors in the way
#define ROOFING_SLANTED 8  // So can't tele onto slanted roofs, basically
#define FLOORS_FLAT_ROOFING 16  //  For attacking between floors
#define LAVA_WATER 32  // Don't know what all to use this for yet

// House List types
#define H_FRIEND	1	// friend of the house
#define H_BAN		2	// banned from house

///////////////////////  End Defines


#define HASHMAX 1031 // lord binary, hashmax must be a prime for maximum performce (see Knuth, Sedgewick et al).

#define DEFAULTCHARTER "UOX3 Guildstone"
#define DEFAULTWEBPAGE "http://www.uox3.net/"
#define PATHNUM 2

// Item types when collided with

#define MONSTERGATE		82
#define RACEGATE		83
#define SOUNDOBJECT		86
#define DAMAGEOBJECT	85
#define ADVANCEMENTOBJ	80

// Item types when used

typedef vector< UI16 > itemIDList;

#endif
