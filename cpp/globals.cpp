//------------------------------------------------------------------------
//  globals.cpp
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
// Global Variables

#include <uox3.h>

creat_st creatures[2048];

short UOX_PORT = 2593;

unsigned int uiCurrentTime, ErrorCount;
char Loaded;
unsigned short int doorbase[DOORTYPES]={
0x0675, 0x0685, 0x0695, 0x06A5, 0x06B5, 0x06C5, 0x06D5, 0x06E5, 0x0839, 0x084C, 
0x0866, 0x00E8, 0x0314, 0x0324, 0x0334, 0x0344, 0x0354, 0x0824, 0x190E};

char skillname[SKILLS+1][20]={
"ALCHEMY", "ANATOMY", "ANIMALLORE", "ITEMID", "ARMSLORE", "PARRYING", "BEGGING", "BLACKSMITHING", "BOWCRAFT",
"PEACEMAKING", "CAMPING", "CARPENTRY", "CARTOGRAPHY", "COOKING", "DETECTINGHIDDEN", "ENTICEMENT", "EVALUATINGINTEL",
"HEALING", "FISHING", "FORENSICS", "HERDING", "HIDING", "PROVOCATION", "INSCRIPTION", "LOCKPICKING", "MAGERY",
"MAGICRESISTANCE", "TACTICS", "SNOOPING", "MUSICIANSHIP", "POISONING", "ARCHERY", "SPIRITSPEAK", "STEALING",
"TAILORING", "TAMING", "TASTEID", "TINKERING", "TRACKING", "VETERINARY", "SWORDSMANSHIP", "MACEFIGHTING", "FENCING",
"WRESTLING", "LUMBERJACKING", "MINING", "MEDITATION", "STEALTH", "REMOVETRAPS", "ALLSKILLS", "STR", "DEX", "INT", "FAME", "KARMA"
};

char spellname[71][25]={
        "Clumsy","Create Food","Feeblemind","Heal","Magic Arrow","Night Sight","Reactive Armor","Weaken",
        "Agility","Cunning","Cure","Harm","Magic Trap","Magic Untrap","Protection","Strength",
        "Bless","Fireball","Magic Lock","Poison","Telekinesis","Teleport","Unlock","Wall Of Stone",
        "Arch Cure","Arch Protection","Curse","Fire Field","Greater Heal","Lightning","Mana Drain","Recall",
        "Blade Spirits","Dispel Field","Incognito","Magic Reflection","Mind Blast","Paralyze","Poison Field","Summon Creature",
        "Dispel","Energy Bolt","Explosion","Invisibility","Mark","Mass Curse","Paralyze Field","Reveal",
        "Chain Lightning","Energy Field","Flame Strike","Gate Travel","Mana Vampire","Mass Dispel","Meteor Swarm","Polymorph",
        "Earthquake","Energy Vortex","Resurrection","Summon Air Elemental",
        "Summon Daemon","Summon Earth Elemental","Summon Fire Elemental","Summon Water Elemental",
        "Random", "Necro1", "Necro2", "Necro3", "Necro4", "Necro5"
};

int GetCombatResult[2];
combat_st combat;
char login04a[6]="\xA9\x09\x24\x02";
char login04b[61]="\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
char login04c[18]="\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
char login04d[64]="\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
char login03[12]="\x8C\x00\x00\x00\x00\x13\x88\x7B\x7B\x7B\x01";
char noaccount[3]="\x82\x00";
char acctblock[3]="\x82\x02";
char pausex[3]="\x33\x01";
char restart[3]="\x33\x00";
unsigned char goxyz[20]="\x20\x00\x05\xA8\x90\x01\x90\x00\x83\xFF\x00\x06\x08\x06\x49\x00\x00\x02\x00";
char wearitem[16]="\x2E\x40\x0A\x00\x01\x00\x00\x00\x01\x00\x05\xA8\x90\x00\x00";
unsigned char talk[15]="\x1C\x00\x00\x01\x02\x03\x04\x01\x90\x00\x00\x38\x00\x03";
char sysname[31]="System\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
char removeitem[6]="\x1D\x00\x00\x00\x00";
char gmprefix[10]="\x7C\x00\x00\x01\x02\x03\x04\x00\x64";
char gmmiddle[5]="\x00\x00\x00\x00";
unsigned char sfx[13]="\x54\x01\x12\x34\x00\x00\x06\x40\x05\x9A\x00\x00";
char doact[15]="\x6E\x01\x02\x03\x04\x01\x02\x00\x05\x00\x01\x00\x00\x01";
char bpitem[20]="\x40\x0D\x98\xF7\x0F\x4F\x00\x00\x09\x00\x30\x00\x52\x40\x0B\x00\x1A\x00\x00";
char gump1[22]="\xB0\x04\x0A\x40\x91\x51\xE7\x00\x00\x00\x03\x00\x00\x00\x6E\x00\x00\x00\x46\x02\x3B";
char gump2[4]="\x00\x00\x00";
char gump3[3]="\x00\x00";
char dyevat[10]="\x95\x40\x01\x02\x03\x00\x00\x0F\xAB";
char updscroll[11]="\xA6\x01\x02\x02\x00\x00\x00\x00\x01\x02";
char spc[2]="\x20";
char bounce[3]="\x27\x00";
char extmove[18]="\x77\x01\x02\x03\x04\x01\x90\x01\x02\x01\x02\x0A\x00\xED\x00\x00\x00";
char xgm;
unsigned char attackok[6] = "\xAA\x00\x00\x00\x00"; // AntiChrist! 26/10/99 (make it unsigned in future, stops warnings
#ifdef __NT__
WSADATA wsaData;
WORD wVersionRequested;
#endif

unsigned long int updatepctime=0;

unsigned char LSD[MAXCLIENT];
bool firstpacket[MAXCLIENT];
long idleTimeout[MAXCLIENT];

short int gatex[MAXGATES][2],gatey[MAXGATES][2];
signed char gatez[MAXGATES][2];
int gatecount=0;
int totalspawnregions=0;
//Time variables
int day=0, hour = 12, minute = 1, ampm = 0; //Initial time is noon.
int secondsperuominute=5; //Number of seconds for a UOX minute.
int uotickcount=1;
int moon1update=0;
int moon2update=0;
int hbu=0;       // heartbeat update var
int openings=0;
char moon1=0;
char moon2=0;
char dungeonlightlevel=0x15;
unsigned char worldfixedlevel=255;
char worldcurlevel=0;
char worldbrightlevel=0x01; //used to be 0x05
char worlddarklevel=0x010; //used to be 0x1a - too dark
int goldamount;
char goldamountstr [10];
char defaultpriv1str [2];
char defaultpriv2str [2];
int defaultpriv1;
int defaultpriv2;
//unsigned int teffectcount=0; // No temp effects to start with
unsigned int nextfieldeffecttime=0;
unsigned int nextnpcaitime=0;
unsigned int nextdecaytime=0;
short int max_tele_locations;
short int tele_locations[MAX_TELE_LOCATIONS][6];
char noweather[MAXCLIENT+1];
// MSVC fails to compile UOX if this is unsigned, change it then
#ifdef _MSVC
long int oldtime, newtime;
#else
unsigned long int oldtime, newtime;      //for autosaving
#endif
int autosaved, saveinterval, heartbeat;
//char saveintervalstr[4];
long int whomenudata[(MAXCLIENT+50)*7]; // LB, for improved whomenu, ( is important !!! (Abaddon actually it's not here, because of operator precedence... and if you were going to include () put it like this (MAXCLIENT*7) not (MAXCLIENT)*7
unsigned int len_connection_addr;
struct sockaddr_in connection;
struct sockaddr_in client_addr;
struct hostent *he;
char hname[40];
int err, error;
int keeprun;
int a_socket;
int client[MAXCLIENT];
int server[MAXCLIENT];
fd_set conn;
fd_set all;
fd_set errsock;
int nfds;
timeval uoxtimeout;
unsigned int now;
int newclient[MAXCLIENT];
unsigned char buffer[MAXCLIENT][MAXBUFFER];
unsigned char outbuffer[MAXCLIENT][MAXBUFFER];
unsigned char tbuffer[MAXBUFFER];
short int walksequence[MAXCLIENT];
int recvcount;
char temp[1024];
char temp2[1024];
FILE *infile = NULL, *scpfile = NULL, *lstfile = NULL, *wscfile = NULL;
unsigned int acctcount;
unsigned int servcount;
unsigned int startcount;
acct_st acctx[MAXACCT];

//nameMenu_st nameMenus; //Coming Soon - Name Menus

//char acctx[MAXACCT][3][31]; // Account list
char acctinuse[MAXACCT];
char serv[MAXSERV][2][30]; // Servers list
char start[MAXSTART][5][30]; // Startpoints list
int acctno[MAXCLIENT];
unsigned int currchar[MAXCLIENT];
int freecharmem[301];
int cmemcheck;
char cmemover;
int freeitemmem[501];
int imemcheck;
char imemover;
char xcounter;
char ycounter;
char clientip[MAXCLIENT][4];
int secure; // Secure mode
char fametitle[128];
char skilltitle[50];
char prowesstitle[50];
repsys_st repsys;
//statcap_st statcap; //Morrolan - stat/skill cap
resource_st resource;
//unitile_st xyblock[XYMAX];
//int xycount;
//unsigned int inworld[MAXACCT];//Instalog
signed long int inworld[MAXACCT];	// Instalog
resour_st resources[9];
//acct_st acct[MAXACCT];

wiped_st wiped[MAXACCT];
unsigned int wipecount = 0;
splInfo_st *spells; //:Terrin: adding variable for spell system "cache" had to make global for skills.cpp as a quick fix

int totalShop=0;
int totalClassNPC=0;
int shopSpawnTime=0;

int tempint[MAXCLIENT];

guild_st guilds[MAXGUILDS];  // Guild related structure (DasRaetsel)
// - the below structure is for looking up items based on serial #
// - item's serial, owner's serial, char's serial, and container's serial

lookuptr_st itemsp[HASHMAX], ownsp[HASHMAX], spawnsp[HASHMAX], contsp[HASHMAX];
lookuptr_st charsp[HASHMAX], cownsp[HASHMAX], cspawnsp[HASHMAX];
lookuptr_st imultisp[HASHMAX], cmultisp[HASHMAX], glowsp[HASHMAX];

//int *contcache;
int *loscache;
int *itemids;

unsigned int lighttime=0;

signed char addid5[MAXCLIENT];

unsigned char w_anim[3]={0x1d,0x95,0}; // flying blood instead of lightnings, LB in a real bad mood	// tseramed changed this from extern to nonextern

unsigned int raindroptime;

int save_counter; // LB, world backup rate

int gDecayItem=0; //Last item that was checked for decay Tauriel 3/7/99
int gRespawnItem=0; //Last item that was checked for respawn Tauriel 3/7/99

cItemHandle items;
cCharacterHandle chars;

location_st location[4000];
region_st region[256];
logout_st logout[1024];//Instalog
spawnregion_st spawnregion[512];//Regionspawns
skill_st skill[SKILLS+1];
advance_st statadvance[1000];
make_st itemmake[MAXCLIENT];
int locationcount;
unsigned int logoutcount;//Instalog
gmpage_st gmpages[MAXPAGES];
gmpage_st counspages[MAXPAGES];
title_st title[ALLSKILLS+1]; // For custom titles reads titles.scp
jail_st jails[11];
unsigned int charcount;
unsigned int charcount2;
unsigned int itemcount;
unsigned int itemcount2;
unsigned int imem;
unsigned int cmem;
char *cline;
char *comm[CMAX];
int tnum;
unsigned char addid1[MAXCLIENT];
unsigned char addid2[MAXCLIENT];
unsigned char addid3[MAXCLIENT];
unsigned char addid4[MAXCLIENT];

int npcshape[5]; //Stores the coords of the bouding shape for the NPC
char dyeall[MAXCLIENT];
int addx[MAXCLIENT];
int addy[MAXCLIENT];
int addx2[MAXCLIENT];
int addy2[MAXCLIENT];
signed char addz[MAXCLIENT];
int addmitem[MAXCLIENT];
char xtext[MAXCLIENT][31];
char perm[MAXCLIENT];
char uoxmonitor[100];
unsigned int starttime, endtime, lclock;
char overflow;
char idname[256];
char globallight;
char wtype;
char script1[1024];
char script2[1024];
char script3[1024];
char script4[1024]; //added for newbie items
char pass1[256];
char pass2[256];
int executebatch;
int showlayer;
int ph1, ph2, ph3, ph4;
int binlength[MAXIMUM+1];
int boutlength[MAXIMUM+1];
char xoutbuffer[MAXBUFFER*2];
char cryptclient[MAXCLIENT];
unsigned int fly_p = 18; // flying probability = 1/fly_p each step (if it doesnt fly)
unsigned char fly_steps_max = 27;

int resendweathertime=0;
int weathertime=0;
int shopTime=0;
int shoprestocktime=0;
int shoprestockrate=5;
unsigned int respawntime=0;
unsigned int gatedesttime=0;
int stablockcachex[STABLOCKCACHESIZE];
int stablockcachey[STABLOCKCACHESIZE];
signed char stablockcachez[STABLOCKCACHESIZE];
int stablockcacher[STABLOCKCACHESIZE];
int stablockcachei;
int stablockcachehit;
int stablockcachemiss;
int layers[MAXLAYERS];
// Profiling
int networkTime = 0;
int timerTime = 0;
int autoTime = 0;
int loopTime = 0;
int networkTimeCount = 1000;
int timerTimeCount = 1000;
int autoTimeCount = 1000;
int loopTimeCount = 1000;
int *clickx;
int *clicky;
//int *spattackValue;
int *currentSpellType; // 0=spellcast, 1=scrollcast, 2=wand cast
char gettokenstr[256];
int donpcupdate;
char *targetok;
tracking_st tracking_data = {TRACKINGRANGE,MAXTRACKINGTARGETS,TRACKINGTIMER,TRACKINGDISPLAYTIME};
begging_st begging_data; // was = {BEGGINGRANGE,"Could thou spare a few coins?","Hey buddy can you spare some gold?","I have a family to feed, think of the children."};
fishing_st fishing_data = {FISHINGTIMEBASE,FISHINGTIMER};
spiritspeak_st spiritspeak_data = {SPIRITSPEAKTIMER};
speed_st speed;
server_st server_data = {DECAYTIMER,INVISTIMER,HUNGERRATE,SKILLDELAY,REGENRATE1,REGENRATE2,REGENRATE3,GATETIMER};
int triggerx;
int triggery;
signed char triggerz;
// Script files that need to be cached
// Crackerjack Jul 31/99
char sScriptPath[256];// Zippy stuff
char n_scripts[NUM_SCRIPTS][32] =	
{   "items.scp",	"npc.scp",		"create.scp",	"regions.scp",
	"misc.scp",		"skills.scp",	"location.scp",	"menus.scp",
	"spells.scp",	"speech.scp",	"tracking.scp", "newbie.scp",
	"titles.scp",	"advance.scp",	"triggers.scp", "ntrigrs.scp",
	"wtrigrs.scp",	"necro.scp",	"house.scp",	"colors.scp",
	"spawn.scp",	"htmlstrm.scp", "",	"", "races.scp", "weather.scp",	
	"shop.scp",		"class.scp", "polymorph.scp","weatherab.scp","harditems.scp","commands.scp",
	"msgboard.scp" };

Script *i_scripts[NUM_SCRIPTS];

int escortRegions = 0;
int validEscortRegion[256];

unsigned int hungerdamagetimer=0; // For hunger damage
#ifndef __NT__
char *strlwr(char *str) {
  for (unsigned int i=0;i<strlen(str);i++)
    str[i]=tolower(str[i]);
  return str;
}
char *strupr(char *str) {
  for (unsigned int i=0;i<strlen(str);i++)
    str[i]=toupper(str[i]);
  return str;
}
#endif
char da;
int lenConnAddr;
unsigned int showloggedoutpcs;

//-=-=-=-=-=-=-Classes Definitions=-=-=-=-=-=//
CWorldMain		*cwmWorldState;
cRegion			*mapRegions;
cAdmin			*Admin;
cBoat			*Boats;//Boats ! --Zippy
cCombat			*Combat;
cCommands		*Commands;
cGuilds			*Guilds;
cGump			*Gumps;
cItem			*Items;
cMapStuff		*Map = NULL;
cCharStuff		*Npcs;
cSkills			*Skills;
cTownStones		*Towns;
cWeight			*Weight;
cTargets		*Targ;
cNetworkStuff	*Network;
cMagic			*Magic;
cRaces			*Races;
cWeatherAb		*Weather;
cRemote			*xGM[MAXCLIENT];
cMovement       *Movement;
cTEffect		*Effects;
cBooks			*Books;
cWhoList		*WhoList;
cWhoList		*OffList;
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-//
vector< int > menuList;