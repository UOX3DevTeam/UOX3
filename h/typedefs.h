//------------------------------------------------------------------------
//  typedefs.h
//
//------------------------------------------------------------------------
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 1997 - 2001 by Marcus Rating (Cironian)
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//	
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//   
//------------------------------------------------------------------------
#ifndef __UOXTYPES_H
#define __UOXTYPES_H

#define MaxZstep 9


#define MAX_LASTON			30
#define MAX_NAME			50
#define MAX_TITLE			50
#define MAX_TRIGWORD		31
#define MAX_GUILDTITLE		21

#ifndef __linux__			// changed from __64BIT__, Tseramed

typedef double				R64;
typedef float				R32;
typedef unsigned long int	UI32;
typedef signed long int		SI32;
typedef unsigned int		UI16;
typedef signed int			SI16;
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
typedef int					GUILDID;
typedef unsigned int		TIMERVAL;
typedef unsigned int		UOXSOCKET;
typedef int					PLAYER;
typedef int					CHARACTER;
typedef unsigned int		RACEID;
typedef unsigned int		GENDER;
typedef unsigned int		COLOR;
typedef unsigned char		LIGHTLEVEL;
typedef unsigned char		SECONDS;
typedef unsigned char		ARMORCLASS;
typedef int					ITEM;
typedef int					SERIAL;
typedef unsigned int		RACEREL;
typedef int					SKILLVAL;
typedef signed char			RANGE;
typedef unsigned char		weathID;
typedef vector< string *>	stringList;

#define MAXVAL				0
#define MINVAL				1
#define CURRVAL				2
#define TEMP				0
#define WIND				1
#define BEARD				1
#define HAIR				2
#define SKIN				3
#define MALE				1
#define FEMALE				2
#define LIGHT				0
#define RAIN				1
#define COLD				2
#define HEAT				3
#define LIGHTNING			4
#define SNOW				5
#define WEATHNUM			6

#define END					-1			// void bldstr(...);

// EXIT CODES
#define UNKNOWN_ERROR								666

#define FATAL_UOX3_ALLOC_WORLDSTATE					100
#define FATAL_UOX3_ALLOC_MAPREGIONS					101
#define FATAL_UOX3_ALLOC_ADMIN						102
#define FATAL_UOX3_ALLOC_BOATS						103
#define FATAL_UOX3_ALLOC_COMBAT						104
#define FATAL_UOX3_ALLOC_COMMANDS					105
#define FATAL_UOX3_ALLOC_GUILDS						106
#define FATAL_UOX3_ALLOC_GUMPS						107
#define FATAL_UOX3_ALLOC_ITEMS						108
#define FATAL_UOX3_ALLOC_MAP						109
#define FATAL_UOX3_ALLOC_NPCS						110
#define FATAL_UOX3_ALLOC_SKILLS						111
#define FATAL_UOX3_ALLOC_TOWNS						112
#define FATAL_UOX3_ALLOC_WEIGHT						113
#define FATAL_UOX3_ALLOC_TARG						114
#define FATAL_UOX3_ALLOC_NETWORK					115
#define FATAL_UOX3_ALLOC_MAGIC						116
#define FATAL_UOX3_ALLOC_RACES						117
#define FATAL_UOX3_ALLOC_SHOP						118
#define FATAL_UOX3_ALLOC_WEATHER					119
#define FATAL_UOX3_ALLOC_MOVE						120
#define FATAL_UOX3_ALLOC_EFFECTS					121
#define FATAL_UOX3_ALLOC_WHOLIST					122

#define FATAL_UOX3_CHARS							150
#define FATAL_UOX3_TALKINGTO						151
#define FATAL_UOX3_CLICKX							152
#define FATAL_UOX3_CLICKY							153
#define FATAL_UOX3_SPATTACKVALUE					154
#define FATAL_UOX3_CURRENTSPELLTYPE					155
#define FATAL_UOX3_TARGETOK							156
#define FATAL_UOX3_TEFFECTS							157

#define FATAL_UOX3_ITEMS							200
#define FATAL_UOX3_OLDSERIALS						201
#define FATAL_UOX3_CONTCACHE						202
#define FATAL_UOX3_LOSCACHE							203
#define FATAL_UOX3_ITEMIDS							204

#define FATAL_UOX3_CREATE_UOXMENUS					250

#define FATAL_UOX3_ALLOC_ITEMSP						300
#define FATAL_UOX3_ALLOC_CHARSP						301
#define FATAL_UOX3_ALLOC_COWNSP						302
#define FATAL_UOX3_ALLOC_SPAWNSP					303
#define FATAL_UOX3_ALLOC_CONTSP						304
#define FATAL_UOX3_ALLOC_CSPAWNSP					305
#define FATAL_UOX3_ALLOC_OWNSP						306
#define FATAL_UOX3_ALLOC_IMULTISP					307
#define FATAL_UOX3_ALLOC_CMULTISP					308
#define FATAL_UOX3_ALLOC_GLOWSP						309


#define FATAL_UOX3_MAP_NOT_FOUND					350
#define FATAL_UOX3_STATICS_INDEX_NOT_FOUND			351
#define FATAL_UOX3_STATICS_NOT_FOUND				352
#define FATAL_UOX3_VERSION_NOT_FOUND				353
#define FATAL_UOX3_TILEDATA_NOT_FOUND				354
#define FATAL_UOX3_MULTI_DATA_NOT_FOUND				355
#define FATAL_UOX3_MULTI_INDEX_NOT_FOUND			356

#define SPELL_MAX 68 //:Terrin: use define for now; can make autocount later
#define MAXSCRIPTLINE 1024
#define MAXFILESPEC 2048
#define NONWHITESPACE "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789~!@#$%^&*()_+{}|?/><'.,"
#define RANGEINDICATOR "#"
#define VALID "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890!@#$%^&*+_,(){}[]<>?|"
#define GOLD_WEIGHT 0.005 //what a goldpiece weighs this is in hundreths of a stone!

#define MAXIMUM 128 // Maximum open connections to server
#define MAXCLIENT MAXIMUM+1
#define MAXACCT 2000 // Maximum accounts
#define MAXSERV 32 // Maximum servers in login listing
#define MAXSTART 64 // Maximum starting locations
#define MAXBUFFER 2560 // Buffer Size (For socket operations)
#define MAXPAGES 100 // Maximum number of pages in the GM queue
//#define VISRANGE 18 // Visibility for normal items
#define BUILDRANGE 31 // Visibility for castles and keeps

#define XYMAX 256 // Maximum items UOX can handle on one X/Y square
//#define MAXEFFECTS MAXCHARS/10
#define MAXLAYERS 50 // Maximum number of Layers in paperdolls (still not sure how many)
#define ITEMMENUOFFSET 256
#define CMAX 40 // Maximum parameters in one line (Only for memory reasons)
#define VERFILE_MAP 0x00
#define VERFILE_STAIDX 0x01
#define VERFILE_STATICS 0x02
#define VERFILE_ARTIDX 0x03
#define VERFILE_ART 0x04
#define VERFILE_ANIMIDX 0x05
#define VERFILE_ANIM 0x06
#define VERFILE_SOUNDIDX 0x07
#define VERFILE_SOUND 0x08
#define VERFILE_TEXIDX 0x09
#define VERFILE_TEXMAPS 0x0A
#define VERFILE_GUMPIDX 0x0B
#define VERFILE_GUMPART 0x0C
#define VERFILE_MULTIIDX 0x0D
#define VERFILE_MULTI 0x0E
#define VERFILE_SKILLSIDX 0x0F
#define VERFILE_SKILLS 0x10
#define VERFILE_TILEDATA 0x1E
#define VERFILE_ANIMDATA 0x1F
#define TILEDATA_TILES 0x68800

#define REGENRATE1 8 // Seconds to heal ONE hp
#define REGENRATE2 3 // Seconds to heal ONE stm
#define REGENRATE3 5 // Seconds to heal ONE mn
#define SPIRITSPEAKTIMER 30 // spiritspeak lasts at least 30 seconds, additional time is calculated by Skill & INT
#define SHOWLOGGEDOUTPCS 1 // Melen 9/7/99

#define RELEASED
#define NODEBUG
#define NOSPECIAL
#define NOSINGLEONLY
#define NOLSERVCHECK
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
enum MonsterSoundType
{
	SND_STARTATTACK = 0,
	SND_IDLE,
	SND_ATTACK,
	SND_DEFEND,
	SND_DIE
};
/*const int SND_STARTATTACK = 0;
const int SND_IDLE = 1;
const int SND_ATTACK = 2;
const int SND_DEFEND = 3;
const int SND_DIE = 4;*/

//////////////////
// Skill & Stat cap - AntiChrist
#define STATCAP 999
#define SKILLCAP 999999

// List of skill numbers (For later implementation)
enum Skills
{
	ALCHEMY = 0,
	ANATOMY,					// 1
	ANIMALLORE,					// 2
	ITEMID,					// 3
	ARMSLORE,					// 4
	PARRYING,					// 5
	BEGGING,					// 6
	BLACKSMITHING,					// 7
	BOWCRAFT,					// 8
	PEACEMAKING,					// 9
	CAMPING,					// 10
	CARPENTRY,					// 11
	CARTOGRAPHY,					// 12
	COOKING ,					// 13
	DETECTINGHIDDEN ,					// 14
	ENTICEMENT ,					// 15
	EVALUATINGINTEL ,					// 16
	HEALING ,					// 17
	FISHING ,					// 18
	FORENSICS ,					// 19
	HERDING ,					// 20
	HIDING ,					// 21
	PROVOCATION ,					// 22
	INSCRIPTION ,					// 23
	LOCKPICKING ,					// 24
	MAGERY ,					// 25
	MAGICRESISTANCE ,					// 26
	TACTICS ,					// 27
	SNOOPING ,					// 28
	MUSICIANSHIP ,					// 29
	POISONING ,					// 30
	ARCHERY ,					// 31
	SPIRITSPEAK ,					// 32
	STEALING ,					// 33
	TAILORING ,					// 34
	TAMING ,					// 35
	TASTEID ,					// 36
	TINKERING ,					// 37
	TRACKING ,					// 38
	VETERINARY ,					// 39
	SWORDSMANSHIP ,					// 40
	MACEFIGHTING ,					// 41
	FENCING ,					// 42
	WRESTLING ,					// 43
	LUMBERJACKING ,					// 44
	MINING ,					// 45

	MEDITATION  ,					// 46
	STEALTH  ,					// 47
	REMOVETRAPS ,					// 48


	ALLSKILLS , // #skills+1

	STRENGTH , // #skills+2
	DEXTERITY , // #skills+3
	INTELLECT , // #skills+4
	FAME , // #skills+5
	KARMA,  // #skills+6 
	SKILLS     // #skills+7
};
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
#define NPCSPEED 0.125	// 2 steps every second, ab said to change this to 0.5 - fur
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
#define STABLOCKCACHESIZE 1000

#define MAP0CACHE 300

#define MAKEMENUOFFSET 2048
#define TRACKINGMENUOFFSET 4096 // menu offset
#define TRACKINGRANGE 10        // tracking range is at least TRACKINGRANGE, additional distance is calculated by Skill,INT
#define MAXTRACKINGTARGETS 20   // maximum number of trackable targets
#define TRACKINGTIMER 30        // tracking last at least TRACKINGTIMER seconds, additional time is calculated by Skill,INT, & DEX
#define TRACKINGDISPLAYTIME 30  // tracking will display above the character every TRACKINGDISPLAYTIME seconds
#define POLYMORPHMENUOFFSET 8192 // polymorph spell menu offset - AntiChrist (9/99)
#define NORTH 1
#define NORTHEAST 2
#define EAST 3
#define SOUTHEAST 4
#define SOUTH 5
#define SOUTHWEST 6
#define WEST 7
#define NORTHWEST 8
#define BEGGINGRANGE 3                  // begging range
#define FISHINGTIMEBASE 10              // it takes at least FISHINGTIMEBASE sec to fish
#define FISHINGTIMER 5
// for spinning wheel -vagrant
#define YARN 0
#define THREAD 1
// Structure Definitions
#define MAX_TELE_LOCATIONS 400
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

// Script files that need to be cached
// Crackerjack Jul 31/99 - Changing to references
// so main always gets to run first.
// List of scripts number->name conversion
const char items_script		=	0;
const char npc_script		=	1;
const char create_script	=	2;
const char regions_script	=	3;
const char misc_script		=	4;
const char skills_script	=	5;
const char location_script	=	6;
const char menus_script		=	7;
const char spells_script	=	8;
const char speech_script	=	9;
const char tracking_script	=	10;
const char newbie_script	=	11;
const char titles_script	=	12;
const char advance_script	=	13;
const char trigger_script	=	14;
const char ntrigger_script	=	15;
const char wtrigger_script	=	16;
const char necro_script		=	17;
const char house_script		=	18;
const char colors_script	=	19;
const char spawn_script		=	20;
const char html_script		=	21;
const char custom_npc_script=	22;
const char custom_item_script=	23;
const char race_script		=	24;
const char weather_script	=	25;
const char shop_script		=	26;
const char class_script		=	27;
const char polymorph_script	=	28;
const char weathab_script	=	29;
const char hard_items		=	30;	// by Magius(CHE)
const char command_script	=	31;
const char msgboard_script	=	32;
//#define addmenu_script		32 //Coming soon - Name Menus

const char NUM_SCRIPTS		=	33; // always # of last script + 1

// DasRaetsels' stuff down, don't touch ;)

#define	MAXGUILDS 100
#define	MAXGUILDMEMBERS 10
#define	MAXGUILDRECRUITS 10
#define	MAXGUILDWARS 10
#define DEFAULTCHARTER "UOX3 Guildstone"
#define DEFAULTWEBPAGE "http://www.uoxdev.com/"
#define PATHNUM 2

// Item types when collided with

#define MONSTERGATE		82
#define RACEGATE		83
#define SOUNDOBJECT		86
#define DAMAGEOBJECT	85
#define ADVANCEMENTOBJ	80

// Item types when used

#define SMOKING			185

#define GMBIT           0x01;
#endif
