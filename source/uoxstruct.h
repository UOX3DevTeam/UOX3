//------------------------------------------------------------------------
//  uoxstruct.h
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
#ifndef __UOXSTRUCT_H
#define __UOXSTRUCT_H
#include "uox3.h"


// these are the fixed record lengths as determined by the .mul files from OSI
// i made them longs because they are used to calculate offsets into the files
const UI32 VersionRecordSize = 20L;
const UI32 MultiRecordSize = 12L;
const UI32 LandRecordSize = 26L;
const UI32 TileRecordSize = 37L;
const UI32 MapRecordSize = 3L;
const UI32 MultiIndexRecordSize = 12L;
const UI32 StaticRecordSize = 7L;

struct combat_st
{
	int wrestleSpeed;	// The speed for wrestling.
	int tacticFormula;	// The formula for getting extra damage from tactics.
	int maxRangeSpell;	// Max range for casting spells
	int maxRange;		// The max range until the combat stops
	int maxDmg;			// Max Damage
	int dToPoison;		// Distance To Poison
	int deathOnThroat;	// Player dies if his throat was slit with dmg higher then 15
	float explodeDelay;	// explosion delay
};
struct path_st {
	unsigned short x;
	unsigned short y;
};

struct move_st
{
	short int effect[5];
};

struct stat_st
{
	short int effect[4];
};

struct sound_st
{
	short int effect[2];
};

struct char_st
{
	unsigned char	usepotion;
	int locked;
	unsigned char	ser1; // Character serial number		(Abaddon converted to unsigned char)
	unsigned char	ser2;
	unsigned char	ser3;
	unsigned char	ser4;
	long			serial;

    unsigned char	multi1;//Multi serial1		(Abaddon converted to unsigned char)
	unsigned char	multi2;//Multi serial2
	unsigned char	multi3;//Multi serial3
	unsigned char	multi4;//Multi serial4
	long			multis;//Multi serial

	char			free;
	char			name[MAX_NAME];
	char			orgname[MAX_NAME]; // original name - for Incognito
	char			title[MAX_TITLE];
	char			unicode; // This is set to 1 if the player uses unicode speech, 0 if not
//	unsigned int	account;
	int				account;
	short int		x;
	short int		y;
	signed char		z;

	signed char	dispz; // Z that the char is SHOWN at. Server needs other coordinates for real movement calculations.
	           // changed from unsigned to signed, LB
	          
	unsigned int	oldx; // fix for jail bug
	unsigned int	oldy; // fix for jail bug

	signed char 	oldz;

	char			dir; //&0F=Direction
	unsigned char	id1; // Character body type
	unsigned char	id2; // Character body type
	unsigned char	xid1; // Backup of body type for ghosts
	unsigned char	xid2; // Backup of body type for ghosts
	unsigned char	orgid1; // Backup of body type for polymorph
	unsigned char	orgid2; // backup of .....
	unsigned char	haircolor1;		// backup of hair/beard for incognito spell
	unsigned char	haircolor2;
	unsigned char	hairstyle1;
	unsigned char	hairstyle2;
	unsigned char	beardcolor1;
	unsigned char	beardcolor2;
	unsigned char	beardstyle1;
	unsigned char	beardstyle2;
	unsigned char	skin1; // Skin color
	unsigned char	skin2; // Skin color
	unsigned char	orgskin1;	// skin color backup for incognito spell
	unsigned char	orgskin2;
	int             keynumb;  // for renaming keys 
	unsigned char	xskin1; // Backup of skin color
	unsigned char	xskin2; // Backup of skin color
	unsigned char	priv;	// 1:GM clearance, 2:Broadcast, 4:Invulnerable, 8: single click serial numbers
							// 10: Don't show skill titles, 20: GM Pagable, 40: Can snoop others packs, 80: Counselor clearance
	unsigned char	priv2;	// 1:Allmove, 2: Frozen, 4: View houses as icons, 8: permanently hidden
							// 10: no need mana, 20: dispellable, 40: permanent magic reflect, 80: no need reagents
	char			fonttype; // Speech font to use
	unsigned char	saycolor1; // Color for say messages
	unsigned char	saycolor2; // Color for say messages
	unsigned char	emotecolor1; // Color for emote messages
	unsigned char	emotecolor2; // Color for emote messages
	int				st; // Strength
	int				st2; // Reserved for calculation
	int dx; // Dexterity
	int dx2; // Reserved for calculation
	int in; // Intelligence
	int in2; // Reserved for calculation
	int hp; // Hitpoints
	int stm; // Stamina
	int mn; // Mana
	int mn2; // Reserved for calculation
	int hidamage; //NPC Damage
	int lodamage; //NPC Damage
	unsigned short int baseskill[ALLSKILLS+1]; // Base skills without stat modifiers
	unsigned short int skill[ALLSKILLS+1]; // List of skills (with stat modifiers)
	unsigned short atrophy[ALLSKILLS];
	unsigned char lockState[ALLSKILLS+1];	// state of the skill locks
	char npc; // 1=Character is an NPC
	char shop; //1=npc shopkeeper
	unsigned char cell; // Reserved for jailing players
	unsigned char own1; // If Char is an NPC, this sets its owner	(Abaddon converted to unsigned char)
	unsigned char own2; // If Char is an NPC, this sets its owner
	unsigned char own3; // If Char is an NPC, this sets its owner
	unsigned char own4; // If Char is an NPC, this sets its owner
	long ownserial; // If Char is an NPC, this sets its owner
	unsigned char robe1; // Serial number of generated death robe (If char is a ghost)
	unsigned char robe2; // Serial number of generated death robe (If char is a ghost)
	unsigned char robe3; // Serial number of generated death robe (If char is a ghost)
	unsigned char robe4; // Serial number of generated death robe (If char is a ghost)
	int karma;
	signed int fame;
	unsigned int kills; //PvP Kills
	unsigned int deaths;
	char dead; // Is character dead
	int packitem; // Only used during character creation
	unsigned char fixedlight; // Fixed lighting level (For chars in dungeons, where they dont see the night)
	char speech; // For NPCs: Number of the assigned speech block
	int weight; //Total weight
	unsigned int att; // Intrinsic attack (For monsters that cant carry weapons)
	unsigned int def; // Intrinsic defense
	char war; // War Mode
	int targ; // Current combat target
	SI32 timeout; // Combat timeout (For hitting)
	unsigned int regen, regen2, regen3;//Regeneration times for mana, stamin, and str
	int runenumb; // Used for naming runes
	int namedeed; /// for name deeds
	int attacker; // Character who attacked this character
	unsigned int npcmovetime; // Next time npc will walk
 char npcWander; // NPC Wander Mode
 char oldnpcWander; // Used for fleeing npcs
 int ftarg; // NPC Follow Target
 int fx1; //NPC Wander Point 1 x
 int fx2; //NPC Wander Point 2 x
 int fy1; //NPC Wander Point 1 y
 int fy2; //NPC Wander Point 2 y
 signed char fz1; //NPC Wander Point 1 z
 unsigned char spawn1; // Spawned by
 unsigned char spawn2; // Spawned by
 unsigned char spawn3; // Spawned by
 unsigned char spawn4; // Spawned by
 long spawnserial; // Spawned by
 int shopSpawn;		// Revana
 char hidden; // 0 = not hidden, 1 = hidden, 2 = invisible spell
 unsigned int invistimeout;
 char attackfirst; // 0 = defending, 1 = attacked first
 char onhorse; // On a horse?
 int hunger;  // Level of hungerness, 6 = full, 0 = "empty"
 unsigned int hungertime; // Timer used for hunger, one point is dropped every 20 min
 int smeltitem;
 int tailitem;
 short npcaitype;
 int callnum; //GM Paging
 int playercallnum; //GM Paging
 int pagegm; //GM Paging
 unsigned char region;
 unsigned int skilldelay;
 unsigned int objectdelay;
 int combathitmessage;
 int making; // skill number of skill using to make item, 0 if not making anything.
 char blocked;
 char dir2;
 unsigned int spiritspeaktimer; // Timer used for duration of spirit speak
 int spattack;
 int spadelay;
 unsigned int spatimer;
 int taming; //Skill level required for taming
 unsigned int summontimer; //Timer for summoned creatures.
 unsigned int trackingtimer; // Timer used for the duration of tracking
 unsigned int trackingtarget; // Tracking target ID
 unsigned int trackingtargets[MAXTRACKINGTARGETS];
 unsigned int fishingtimer; // Timer used to delay the catching of fish
 int town;       //Matches Region number in regions.scp
 unsigned char townvote1; //Serial Number of who they want to be mayor.
 unsigned char townvote2; //Serial Number of who they want to be mayor.
 unsigned char townvote3; //Serial Number of who they want to be mayor.
 unsigned char townvote4; //Serial Number of who they want to be mayor.
 int towntitle;  //0=off (default), 1=on. (i.e. - The Honorable Joe of Moonglow, Expert Swordsman)
 char townpriv;  //0=non resident (Other privledges added as more functionality added)
 int advobj; //Has used advance gate?

 int poison; // used for poison skill 
 int poisoned; // type of poison
 unsigned int poisontime; // poison damage timer
 unsigned int poisontxt; // poision text timer
 unsigned int poisonwearofftime; // LB, makes poision wear off ...

 int fleeat;
 int reattackat;
 int trigger; //Trigger number that character activates
 char trigword[MAX_TRIGWORD]; //Word that character triggers on.
 unsigned int disabled; //Character is disabled, cant trigger.
 char envokeid1; //ID1 of item user envoked
 char envokeid2; //ID2 of item user envoked
 int envokeitem;
 int split;
 int splitchnc;
 int targtrig; //Stores the number of the trigger the character for targeting
 char ra;  // Reactive Armor spell
 int trainer; // Serial of the NPC training the char, -1 if none.
#ifdef __LINUX__
 short trainingplayerin;
#else
 unsigned char trainingplayerin; // Index in skillname of the skill the NPC is training the player in
#endif
 char cantrain;
 char laston[MAX_LASTON]; //Last time a character was on
// Begin of Guild Related Character information (DasRaetsel)
	int guildtoggle;		// Toggle for Guildtitle								(DasRaetsel)
	char guildtitle[MAX_GUILDTITLE];	// Title Guildmaster granted player						(DasRaetsel)
	int	guildfealty;		// Serial of player you are loyal to (default=yourself)	(DasRaetsel)
	int	guildnumber;		// Number of guild player is in (0=no guild)			(DasRaetsel)
 char flag; //1=red 2=grey 4=Blue 8=green 10=Orange
 //char tempflag; //Zippy -- Not Used
 //unsigned int tempflagtime;
 // End of Guild Related Character information
	long int murderrate; //#of ticks until one murder decays //REPSYS 
	int crimflag; //Time when No longer criminal -1=Not Criminal
	int casting; // 0/1 is the cast casting a spell?
	unsigned int spelltime; //Time when they are done casting....
	int spellCast; //current spell they are casting....
	int spellaction; //Action of the current spell....
	int nextact; //time to next spell action....
	int poisonserial; //AntiChrist -- poisoning skill
	
	int squelched; // zippy  - squelching
	int mutetime; //Time till they are UN-Squelched.
	int med; // 0=not meditating, 1=meditating //Morrolan - Meditation 
	int statuse[3]; //Morrolan - stat/skill cap STR/INT/DEX in that order
	int skilluse[TRUESKILLS][1]; //Morrolan - stat/skill cap
	int stealth; //AntiChrist - stealth ( steps already done, -1=not using )
	unsigned int running; //AntiChrist - Stamina Loose while running
	SI32 logout;//Time till logout for this char -1 means in the world or already logged out //Instalog // -1 on unsigned int, now signed long
	int swingtarg; //Tagret they are going to hit after they swing

	unsigned int holdg; // Gold a player vendor is holding for Owner
	RACEID race;							// Characters race
	RACEID raceGate;						// Race gate that has been used
	TIMERVAL weathDamage[WEATHNUM];			// Light Damage timer
	char fly_steps; // number of step the creatures flies if it can fly
	unsigned long int trackingdisplaytimer;
	bool tamed;
	bool guarded;							// (Abaddon) if guarded
	bool runs;								// (Abaddon) can he run?
	unsigned char step;						// (Abaddon) 1 if step 1 0 if step 2 3 if step 1 skip 2 if step 2 skip

	unsigned char pathnum;
	path_st path[PATHNUM];
	unsigned int smoketimer; // LB
	unsigned int smokedisplaytimer;
	unsigned int antispamtimer; // LB - anti spam
	int carve; // AntiChrist - for new carve system
	unsigned char commandLevel;		// 0 = player, 1 = counselor, 2 = GM
	int postType;
	int questType;
	int questDestRegion;
	int questOrigRegion;
	bool may_levitate;	// has this character climbed on ladder last move? if so he can levitate on next
};
//REPSYS
struct repsys_st
{
	long int		murderdecay;
	unsigned int	maxkills;
	int				crimtime;
};
struct resource_st
{
	unsigned int		logs;
	unsigned long int	logtime;
	unsigned int		logarea;
	unsigned int		ore;
	unsigned long int	oretime;
	unsigned int		orearea;
};
struct item_st
{
 unsigned char	ser1; // Item serial number
 unsigned char	ser2;
 unsigned char	ser3;
 unsigned char	ser4;
 long serial;

 unsigned char	multi1;//Multi serial1
 unsigned char	multi2;//Multi serial2
 unsigned char	multi3;//Multi serial3
 unsigned char	multi4;//Multi serial4
 long			multis;//Multi serial

 char free;
 unsigned char id1; // Item visuals as stored in the client
 unsigned char id2;
 char name[MAX_NAME];
 char name2[MAX_NAME];
 short int x;
 short int y;
 signed char z;
 unsigned char color1; // Hue
 unsigned char color2;
 unsigned char cont1; // Container that this item is found in
 unsigned char cont2;
 unsigned char cont3;
 unsigned char cont4;
 long contserial;
 char layer; // Layer if equipped on paperdoll
 int itmhand; // ITEMHAND system - AntiChrist
 unsigned int type; // For things that do special things on doubleclicking
 unsigned int type2;
 char offspell;
 int weight;
 unsigned char more1; // For various stuff
 unsigned char more2;
 unsigned char more3;
 unsigned char more4;
 unsigned char moreb1;
 unsigned char moreb2;
 unsigned char moreb3;
 unsigned char moreb4;
 unsigned int morex;
 unsigned int morey;
 unsigned int morez;
 unsigned int amount; // Amount of items in pile
 unsigned int amount2; //Used to track things like number of yards left in a roll of cloth
 char doordir; // Reserved for doors
 char dooropen;
 char pileable; // Can item be piled
 char dye; // Reserved: Can item be dyed by dye kit
 char corpse; // Is item a corpse
// unsigned int att; // Item attack
 unsigned char att; // Item attack	// never above 50 in items.scp
 unsigned int def; // Item defense
 int lodamage; //Minimum Damage weapon inflicts
 int hidamage; //Maximum damage weapon inflicts
 int wpsk; //The skill needed to use the item
 int hp; //Number of hit points an item has.
 int maxhp; // Max number of hit points an item can have.
 int st; // The strength needed to equip the item
 int st2; // The strength the item gives
 int dx; // The dexterity needed to equip the item
 int dx2; // The dexterity the item gives
 int in; // The intelligence needed to equip the item
 int in2; // The intelligence the item gives
 int spd; //The speed of the weapon
 int wipe; //Should this item be wiped with the /wipe command
 char magic; // 0=Default as stored in client, 1=Always movable, 2=Never movable, 3=Owner movable.
 unsigned int gatetime;
 int gatenumber;
 unsigned int decaytime;
 unsigned char owner1;
 unsigned char owner2;
 unsigned char owner3;
 unsigned char owner4;
 long ownserial;
 char visible; // 0=Normally Visible, 1=Owner & GM Visible, 2=GM Visible
 unsigned char spawn1;  // Spawned by...
 unsigned char spawn2;
 unsigned char spawn3;
 unsigned char spawn4;
 long spawnserial;
#ifndef __LINUX__
 char dir; // Direction, or light source type.
#else
 short dir;
#endif
 char priv; // Bit 0, decay off/on.  Bit 1, newbie item off/on.  Bit 2 Dispellable
 int value; // Price shopkeeper sells item at.
 int restock; // Number up to which shopkeeper should restock this item
 int trigger; //Trigger number that item activates
 int trigtype; //Type of trigger
 unsigned int disabled; //Item is disabled, cant trigger.
 int tuses;    //Number of uses for trigger
 unsigned int poisoned; //AntiChrist -- for poisoning skill
 char murderer[50]; //AntiChrist -- for corpse -- char's name who kille the char (forensic ev.)
 unsigned int murdertime; //AntiChrist -- for corpse -- when the people has been killed	// used to be long, don't need THAT long
 RACEID racialEffect;
 ARMORCLASS armorClass;
 int rank;	// Magius(CHE) --- for rank system, this value is the LEVEL of the item from 1 to 10.  Simply multiply t he rank*10 and calculate the MALUS this item has from the original.
			// for example: RANK 5 --> 5*10 = 50% of malus
			// this item has same values decreased by 50%..
			// RANK 1 --> 1*10=10% this item has 90% of malus!
			// RANK 10 --> 10*10 = 100% this item has no malus! RANK 10 is automatically setted if you select RANKSYSTEM 0.
			// Vars: LODAMAGE, HIDAMAGE, ATT, DEF, HP, MAXHP
 char creator[50]; // Store the name of the player made this item -- Magius(CHE)
 int good; // Store type of GOODs to trade system! (Plz not set as UNSIGNED) --- Magius(CHE)
 int rndvaluerate; // Store the value calculated base on RANDOMVALUE in region.scp. ---- MAgius(CHE) (2)
 int madewith; // Store the skills used to make this item -- Magius(CHE)
 // Note by Magius: Value range to -ALLSKILLS-1 to ALLSKILLS+1
 // To calculate skill used to made this item:
 // if is a positive value, substract 1 it.
 //    Ex) madewith = 34, 34 - 1 = 33, 33 = STEALING
 // if is a negative value, add 1 from it and invert value.
 //    Ex) madewith = -34, -34 + 1 = -33, Abs(-33) = 33 = STEALING.
 // 0 = NULL
 // So... a positive value is used when the item is made by a 
 // player with 95.0+ at that skill. Infact in this way when
 // you click on the item appear its name and the name of the
 // creator. A negative value if the play is not skilled
 // enough!

 int glow;	// LB identifies glowing objects
 char glow_c1; // for backup of old color
 char glow_c2;
 char glow_effect;

 char desc[100];
 int carve; // AntiChrist - for new carve system
};

struct resour_st
{
	int value;
	int key;
};

struct creat_st
{
	int basesound;
	char soundflag;
	char who_am_i; 
	int icon;
};

struct versionrecord
{
 SI32 file;
 SI32 block;
 SI32 filepos;
 SI32 length;
 SI32 unknown;
} PACK_NEEDED;

/*
** if we aren't really using this, im taking it out - fur
struct OldStaRec
{//Old staticrecord (with .extra (needed for reading ))
	short int itemid;
	short int extra;
	unsigned char xoff;
	unsigned char yoff;
	signed char zoff;
} PACK_NEEDED;
*/

struct staticrecord
{
 short int itemid;
// short int extra; // Unknown yet --Zippy unknown thus not used thus taking up mem.
 unsigned char xoff;
 unsigned char yoff;
 signed char zoff;
 char align;	// force word alignment by hand to avoid bus errors - fur
} PACK_NEEDED;

struct map_st {
	short int id;
	signed char z;
};

struct unitile_st
{
	signed char basez;
	char type; // 0=Terrain, 1=Item
	unsigned short int id;
	unsigned char flag1;
	unsigned char flag2;
	unsigned char flag3;
	unsigned char flag4;
	signed char height;
	unsigned char weight;
} PACK_NEEDED;
struct st_multiidx
{
	UI32 start;
	UI32 length;
	UI32 unknown;
} PACK_NEEDED;
struct st_multi
{
 SI32 visible;  // this needs to be first so it is word aligned to avoid bus errors - fur
 short int tile;
 signed short int x;
 signed short int y;
 signed char z;
 signed char empty;
} PACK_NEEDED;
struct teffect_st
{
 unsigned char sour1;
 unsigned char sour2;
 unsigned char sour3;
 unsigned char sour4;
 unsigned char dest1;
 unsigned char dest2;
 unsigned char dest3;
 unsigned char dest4;
 unsigned int expiretime;
 char num;
 unsigned char more1;
 unsigned char more2;
 unsigned char more3;
 char dispellable;
 int itemptr;
} PACK_NEEDED;
struct location_st
{
 int x1;
 int y1;
 int x2;
 int y2;
 char region;
};

struct Shops			// Revana
{
	char *name;
	
	int ignot;
	int wood;
	int water;
	int wheat;
	int meat;
	int cloth;
	int magic;
	int gold;

	int x1;
	int x2;
	int y1;
	int y2;

	int currnpc;

	int maxnpc;

	int *restrict;
};

struct classnpc			// Revana
{
	char *name;
	int *npc;
};

struct logout_st//Instalog
{
	unsigned int x1;
	unsigned int y1;
	unsigned int x2;
	unsigned int y2;
};
struct region_st
{
 char name[50];
 int midilist;
 char priv; // 0x01 guarded, 0x02, mark allowed, 0x04, gate allowed, 0x08, recall
            // 0x10 raining, 0x20, snowing, 0x40 magic damage reduced to 0
 char guardowner[50];
 char snowchance;
 char rainchance;
 int guardnum[10];
 char wtype;				// Revana
 int shop[1000];			// Revana
 weathID weather;
 int goodsell[256]; // Magius(CHE)
 int goodbuy[256]; // Magius(CHE)
 int goodrnd1[256]; // Magius(CHE) (2)
 int goodrnd2[256]; // Magius(CHE) (2)
};
struct spawnregion_st//Regionspawns
{
	char name[512];//Any Name to show up when this region is spawned [512]
	int npclists[512];//NPC lists [512]
	int npcs[512];//Individual npcs [512]
	int itemlists[512];//item Lists [512]
	int totalitemlists;//Total number of item lists
	int totalnpcs;//Total Number of indiviual npcs
	int totalnpclists;//Number of NPC lists
	int max;//Max amount of spawned characters
	int current;//Current amount of spawned chars
	int mintime;//Minimum spawn time
	int maxtime;//Maximum spawn time
	int nexttime;//Nextspawn time for this region
	int x1;//Top left X
	int x2;//Bottom right x
	int y1;//Top left y
	int y2;//Bottom right y
	int call;	// # of times that an NPC or ITEM is spawned from a list
};

#define MAX_ACCT_LOCK 5
struct acct_st
{
	char name[20];      // Client doesn't allow accounts with more than 16 chars.
	char pass[20];      // Client doesn't allow password with more than 16 chars.
	int banTime;
	int ban;
	int banSerial;
	int warning;
	int wipe;
	int wipeSerial;
	int saveChar;
	int ChgPass;
	int lock[MAX_ACCT_LOCK];
	char contact[512];
	char tempIP[512];
	unsigned long int ip1;
	unsigned long int ip2;
	unsigned long int ip3;
	unsigned long int ip4;
	bool xGM;
	bool listpublic;
};

struct wiped_st
{
	char name[20];
	char pass[20];
	int wipeSerial;
	char contact[512];
};

struct skill_st
{
 int st;
 int dx;
 int in;
 int advance_index;
 char madeword[50]; // Added by Magius(CHE)
};
struct advance_st
{
	char skill;
	int base;
	int success;
	int failure;
};
struct make_st
{
	int has;
	int has2;
	int needs;
	int minskill;
	int maxskill;
	unsigned char materialid1; // id1 of material used to make item       
	unsigned char materialid2; // id2 of material used to make item       (vagrant)
	unsigned char materialid1b; // id1 of second material used to make item       (vagrant)
	unsigned char materialid2b; // id2 of second material used to make item       (vagrant)
	int minrank; // value of minum rank level of the item to create! - Magius(CHE)
	int maxrank; // value of maximum rank level of the item to create! - Magius(CHE)
	int number; // Store Script Number used to Rank System by Magius(CHE)
};
struct gmpage_st
{
	char reason[80];
	unsigned char ser1;
	unsigned char ser2;
	unsigned char ser3;
	unsigned char ser4;
	char timeofcall[9];
	char name[20];
	int handled;
} PACK_NEEDED;
struct jail_st
{
	unsigned int x;
	unsigned int y;
	signed char z;
	unsigned int occupied;
} PACK_NEEDED;
struct tracking_st
{
	unsigned int baserange;
	unsigned int maxtargets;
	unsigned int basetimer;
	unsigned int redisplaytime;
};
struct begging_st
{
	unsigned int range;
	char text[3][256];
};
struct fishing_st
{
	unsigned int basetime;
	unsigned int randomtime;
};
struct spiritspeak_st
{
	unsigned int spiritspeaktimer;
};
struct speed_st //Lag Fix
{
//	unsigned int nice;
	int nice;
	double itemtime;
	unsigned int srtime;
	double npctime;
	double npcaitime;
	unsigned int checkmem;
	unsigned char cache;
	unsigned char tilecheck;
	unsigned char accountFlush;
};
struct server_st
{
	unsigned int potiondelay;
	unsigned int stat_advance; // Gunther stat gain fix... make it adjustable
	unsigned short crashprotect;
	unsigned int poisontimer;
	unsigned int joinmsg;
	unsigned int partmsg;
	unsigned int decaytimer;
	unsigned int playercorpsedecaymultiplier;
	unsigned int lootdecayswithcorpse;
	unsigned int invisibiliytimer;
	unsigned int hungerrate;
	unsigned char hungerThreshold;
	unsigned int skilldelay;
	unsigned int objectdelay;
	unsigned int hitpointrate;
	unsigned int staminarate;
	unsigned int manarate;
	unsigned int gatetimer;
	unsigned int minecheck;
	unsigned int showdeathanim;
	unsigned int combathitmessage;
	unsigned int monsters_vs_animals;
	unsigned int animals_attack_chance;
	unsigned int animals_guarded;
	unsigned int npc_base_fleeat;
	unsigned int npc_base_reattackat;
	unsigned char guardsactive;
	unsigned char bg_sounds;
	char archivepath[256];
	int backup_save_ratio; // LB, each X'th save-intervall a backup is done if a path is given ...
	unsigned char UOXBot;
	unsigned int maxabsorbtion; // Magius(CHE) --- Maximum Armour of the single pieces of armour (for player)!
	unsigned int maxnohabsorbtion; // Magius(CHE) --- Maximum Armour of the total armour (for NPC)!
	unsigned int npcdamage; // Magius(CHE) --- DAmage divided by this number if the attacked is an NPC!
	unsigned int sellbyname; // Magius(CHE) 
	unsigned int sellmaxitem; // Magius(CHE) 
	unsigned int skilllevel; // Magius(CHE) 
	unsigned int trade_system; // Magius(CHE) 
	unsigned int rank_system; // Magius(CHE) 
	unsigned char hungerdamage;
	unsigned int hungerdamagerate;
	char armoraffectmana; // Should armor slow mana regen ?
	//	EviLDeD	-	Added server save support for this
	//	December 27, 1998
	char announceworldsaves;
	//	February 10, 2000
	bool wwwaccounts;	//	Use the www account creation system. Causes autoload on worldsaves
	//	EviLDeD	-	End
	unsigned int log;
	unsigned int rogue;

	unsigned char auto_a_create; //Create accounts when people log in with unknown name/password

	unsigned int lordblagfix;//LB Lag Fix
	unsigned int maxstealthsteps;//AntiChrist - max number of steps allowed with stealth skill at 100.0
	unsigned int runningstaminasteps;//AntiChrist - max number of steps allowed with stealth skill at 100.0
	unsigned short int quittime;//Instalog
	int html;//HTML
	int weathertime;
	int shopSpawnTime;
	int auto_acct;
	double boatspeed;
	int showloggedoutpcs;
	int attackstamina;		// AntiChrist (6) - for ATTACKSTAMINA

	unsigned int skillcap; // LB skill cap
	unsigned int statcap; // AntiChrist stat cap
	char specialbanktrigger[50]; // special bank trigger - AntiChrist
	int usespecialbank; // special bank - AntiChrist

	int cutscrollreq; // AntiChrist - cut skill requirements for scrolls
	int persecute; // AntiChrist - persecute thing
	char msgboardpath[256];
	int msgpostaccess;
	int msgpostremove;
	int msgretention;
	int escortactive;
	int escortinitexpire;
	int escortactiveexpire;
	int escortdoneexpire;
	bool footSteps;
	unsigned char commandPrefix;  
	short buyThreshold;
	bool snoopiscrime;
};
struct title_st // For custom titles
{
 char fame[50];
 char skill[50];
 char prowess[50];
};

// Guildstone related functions

// New structure for basic guild related infos (DasRaetsel)
struct guild_st
{
	char	free;							// Guild slot used?
	char	name[41];						// Name of the guild
	char	abbreviation[4];				// Abbreviation of the guild
	int		type;							// Type of guild (0=standard/1=order/2=chaos)
	char	charter[51];					// Charter of guild
	char	webpage[51];					// Web url of guild
	int		stone;							// The serial of the guildstone
	int		master;							// The serial of the guildmaster
	int		recruits;						// Amount of recruits
	int		recruit[MAXGUILDRECRUITS+1];	// Serials of candidates
	int		members;						// Amount of members
	int		member[MAXGUILDMEMBERS+1];		// Serials of all the members
	int		wars;							// Amount of wars
	int		war[MAXGUILDWARS+1];			// Numbers of Guilds we have declared war to
	int		priv;							// Some dummy to remember some values
};
// extern	guild_st guilds[MAXGUILDS];
// End of new structure for guilds

struct statcap_st
{
	int statmax; //250 stat points (100 str, 100 int, 50 dex?)
	int skillmax; //900 BASE skill points
	long int skilldecay; //12000 seconds
	int numsktrk; //track the last numsktrk (10) skills SUCCESSFULLY used
}; // Morrolan - stat/skill cap

struct reag_st
{
	char ginseng;
	char moss;
	char drake;
	char pearl;
	char silk;
	char ash;
	char shade;
	char garlic;
};

struct splInfo_st
{
	bool enabled;
	char circle;
	int mana;
	int health;
	int stamina;
	int loskill;
	int hiskill;
	int sclo;
	int schi;
	char mantra[25];
	int action;
	int delay;
	reag_st reags;
	char strToSay[100]; // string visualized with targ. system
	bool reflect;		// 1 = spell reflectable, 0 = spell not reflectable
	bool resistable;	// can this spell be resisted?
	bool aggressive;	// is this a criminal spell?
	short int soundEffect[2];	// scriptable sound effects
	short int moveEffect[5];	// scriptable move effects
	short int staticEffect[4];	// scriptable static effects
};

struct sectData
{
	char *tag;
	char *data;
};

#endif
