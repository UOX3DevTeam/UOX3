#include <string>
#include <vector>
//using namespace std;

#ifndef __UOXTYPES_H
#define __UOXTYPES_H

#include "enums.h"

#ifdef __NEED_VALIST__
	typedef void *  va_list;
#endif

#if defined(__unix__)
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
#define MAX_COMMENT			128
#define MAX_TITLE			50
#define MAX_TRIGWORD		31
#define MAX_GUILDTITLE		21
#define MAX_STACK			0xFFFF
#define MAX_ARCHID			14
#define MAX_REGIONNAME	128

typedef double				R64;
typedef float				R32;
typedef unsigned long int	UI32;
typedef signed long int		SI32;
typedef unsigned short int	UI16;
typedef signed short int	SI16;
typedef unsigned char		UI08;
typedef signed char			SI08;

#if defined(__unix__)
	#define MAX_PATH		268
	#define INVALID_SOCKET  (~0)
	#define SOCKET_ERROR	(-1)
	#include "uoxlinux.h" //linux wrappers for windows-specific api calls

#endif

typedef UI32		TIMERVAL;
typedef SI32		UOXSOCKET;
typedef UI32		PLAYER;
typedef UI32		CHARACTER;
typedef UI16		RACEID;
typedef UI32		GENDER;
typedef UI16		COLOUR;
typedef UI08		LIGHTLEVEL;
typedef UI08		SECONDS;
typedef UI08		ARMORCLASS;
typedef UI32		ITEM;
typedef UI32		SERIAL;
typedef SI08		RACEREL;
typedef UI16		SKILLVAL;
typedef SI08		RANGE;
typedef UI08		weathID;

const UI08 WEIGHT_PER_STR = 4;
const SI32 MAX_WEIGHT = (SI32)(0xFFFF*100);

const UI32 INVALIDSERIAL	= 0xFFFFFFFF;
const UI16 INVALIDID		= 0xFFFF;
const UI16 INVALIDCOLOUR	= 0xFFFF;
class CChar;
class CItem;

typedef std::vector< std::string >	STRINGLIST;
typedef std::vector< SERIAL > SERLIST;
typedef std::vector< CChar* > CHARLIST;
typedef std::vector< CItem* > ITEMLIST;
typedef SI16 GuildID;

//
struct FirewallEntry
{
	SI16 b[4];
	FirewallEntry( SI16 p1, SI16 p2, SI16 p3, SI16 p4 ) { b[0] = p1; b[1] = p2; b[2] = p3; b[3] = p4; }
};

typedef FirewallEntry *LPFirewallEntry;

// December 27, 2000
typedef struct __STARTLOCATIONDATA__
{
	__STARTLOCATIONDATA__() { memset( this, 0x00, sizeof( __STARTLOCATIONDATA__ ) ); }
	char	town[32];
	char	description[32];
	SI16	x;
	SI16	y;
	SI16	z;
} STARTLOCATION, *LPSTARTLOCATION;
//	EviLDeD	-	End

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

#define SPELL_MAX 68 //:Terrin: use define for now; can make autocount later

#define MAXIMUM 128 // Maximum open connections to server
#define MAXCLIENT MAXIMUM+1
#define MAXBUFFER 2560 // Buffer Size (For socket operations)
#define MAXPAGES 20 // Maximum number of pages in the GM queue
const SI08 BUILDRANGE = 31; // Visibility for castles and keeps

#define XYMAX 256 // Maximum items UOX can handle on one X/Y square
const UI08 MAXLAYERS = 40; // Maximum number of Layers in paperdolls (still not sure how many)
#define CMAX 40 // Maximum parameters in one line (Only for memory reasons)

/*
const SI32 VERFILE_MAP = 0x00;
const SI32 VERFILE_STAIDX = 0x01;
const SI32 VERFILE_STATICS = 0x02;
const SI32 VERFILE_ARTIDX = 0x03;
const SI32 VERFILE_ART = 0x04;
const SI32 VERFILE_ANIMIDX = 0x05;
const SI32 VERFILE_ANIM = 0x06;
const SI32 VERFILE_SOUNDIDX = 0x07;
const SI32 VERFILE_SOUND = 0x08;
const SI32 VERFILE_TEXIDX = 0x09;
const SI32 VERFILE_TEXMAPS = 0x0A;
const SI32 VERFILE_GUMPIDX = 0x0B;
const SI32 VERFILE_GUMPART = 0x0C;
const SI32 VERFILE_MULTIIDX = 0x0D;
const SI32 VERFILE_MULTI = 0x0E;
const SI32 VERFILE_SKILLSIDX = 0x0F;
const SI32 VERFILE_SKILLS = 0x10;
const SI32 VERFILE_TILEDATA = 0x1E;
const SI32 VERFILE_ANIMDATA = 0x1F;

const SI32 TILEDATA_TILES = 0x68800;
*/

#define SHOWLOGGEDOUTPCS 1 // Melen 9/7/99

#define NODEBUG

//////////////////
// List of monster sounds
const UI08 SND_STARTATTACK = 0;
const UI08 SND_IDLE = 1;
const UI08 SND_ATTACK = 2;
const UI08 SND_DEFEND = 3;
const UI08 SND_DIE = 4;

//////////////////
// Skill & Stat cap
#define STATCAP 999
#define SKILLCAP 999999

typedef __TIMERID__ TID;

#define TRUESKILLS ALLSKILLS //#skills+1

#define DOORTYPES 19

#define NPC_BASE_FLEEAT 20
#define NPC_BASE_REATTACKAT 40
#define SELLBYNAME 1 // Values = 0(Disabled) or 1(Enabled) - The NPC Vendors buy from you if your item has the same name of his item!
#define SKILLLEVEL 5 // Range from 1 to 10 - This value if the difficult to create an item using a make command: 1 is easy, 5 is normal (default), 10 is difficult!
#define SELLMAXITEM 5 // Set maximum amount of items that one player can sell at a time ( 5 is standard OSI )

#define MAP0CACHE 300

#define ITEMMENUOFFSET 256
#define TRACKINGMENUOFFSET 4096 // menu offset
#define POLYMORPHMENUOFFSET 8192
#define TRACKINGRANGE 10        // tracking range is at least TRACKINGRANGE, additional distance is calculated by Skill,INT
#define MAXTRACKINGTARGETS 20   // maximum number of trackable targets
#define TRACKINGTIMER 30        // tracking last at least TRACKINGTIMER seconds, additional time is calculated by Skill,INT, & DEX
#define TRACKINGDISPLAYTIME 30  // tracking will display above the character every TRACKINGDISPLAYTIME seconds

const UI08 NORTH		= 0;
const UI08 NORTHEAST	= 1;
const UI08 EAST			= 2;
const UI08 SOUTHEAST	= 3;
const UI08 SOUTH		= 4;
const UI08 SOUTHWEST	= 5;
const UI08 WEST			= 6;
const UI08 NORTHWEST	= 7;
const UI08 UNKNOWNDIR	= 0xFF;

const UI08 RUNNORTH		= NORTH|0x80;
const UI08 RUNNORTHEAST = NORTHEAST|0x80;
const UI08 RUNEAST		= EAST|0x80;
const UI08 RUNSOUTHEAST	= SOUTHEAST|0x80;
const UI08 RUNSOUTH		= SOUTH|0x80;
const UI08 RUNSOUTHWEST = SOUTHWEST|0x80;
const UI08 RUNWEST		= WEST|0x80;
const UI08 RUNNORTHWEST	= NORTHWEST|0x80;

#define BEGGINGRANGE 3                  // begging range
#define FISHINGTIMEBASE 10              // it takes at least FISHINGTIMEBASE sec to fish
#define FISHINGTIMER 5

// Line Of Sight
#define ITEM_TYPE_CHOICES 6
#define TREES_BUSHES 1 // Trees and other large vegetaion in the way
#define WALLS_CHIMNEYS 2  // Walls, chimineys, ovens, etc... in the way
#define DOORS 4 // Doors in the way
#define ROOFING_SLANTED 8  // So can't tele onto slanted roofs, basically
#define FLOORS_FLAT_ROOFING 16  //  For attacking between floors
#define LAVA_WATER 32  // Don't know what all to use this for yet

#define HASHMAX 1031 // hashmax must be a prime for maximum performce.

#define DEFAULTCHARTER "UOX3 DevTeam Guildstone"
#define DEFAULTWEBPAGE "http://www.uox3dev.net/"

// Item types when collided with

const SI16 NumberOfWorlds				= UOMT_COUNT;
const SI16 MapWidths[UOMT_COUNT]		= { 6144, 0, 2304, 2560 };
const SI16 MapHeights[UOMT_COUNT]		= { 4096, 0, 1600, 2048 };
const SI16 MapTileWidths[UOMT_COUNT]	= { 768,  0, 288,  320 };
const SI16 MapTileHeights[UOMT_COUNT]	= { 512,  0, 200,  256 };
const SI32 MapFileLengths[UOMT_COUNT]	= { 77070336, 0, 11289600, 16056320 };

const SERIAL BASEITEMSERIAL = 0x40000000;

#endif
