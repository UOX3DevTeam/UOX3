//""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
//  uox3.h
//
//""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
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
//""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
#ifndef __UOX3_H
#define __UOX3_H
// product info
#define VER " 0.70.03"
#define BUILD "21"
#define SVER "1.0"
#define CVER "1.0"
#define IVER "1.0"
#define PRODUCT "Ultima Offline eXperiment 3"
#define TIMEZONE "GMT+10"
#define NAME "The Official DevTeam"
#define EMAIL "http://www.uox3.net/"
#define PROGRAMMERS "UOX3 DevTeam[Abaddon/EviLDeD/Freelancers]"
#ifdef __linux__
#define __LINUX__
#endif
#ifdef _WIN32
#define __NT__
#define _MSVC
#endif
#ifdef __NT__
#ifndef _WIN32
#define _WIN32
#endif
#endif
// remove PACKED for unix/linux because it going to cause bus errors - fur
#if defined _WIN32 && (!defined(__MINGW32__))
#define PACK_NEEDED
#else
#define PACK_NEEDED
#endif

/*#ifdef _MSVC
#pragma pack(1)        //fixes tile problem in MSVC++ (maybe others)
#pragma warning(disable: 4786) //Gets rid of BAD stl warnings
#pragma warning(disable: 4503)
#endif
*/

#ifndef _DEBUG//Never define crash protection in debug mode
/*Crash protection stuff:
Note, this slows things down, it's nice and shuts down (with crash 
protection of 0)  Try/catch takes a 5-10% performance hit in C++, 
because of extra stuff it needs to track.  So for max preformace 
consider commenting out this line*/
//#define _CRASH_PROTECT_
#endif

struct lookuptr_st //Tauriel  used to create pointers to the items dynamically allocated
{               //         so don't mess with it unless you know what you are doing!
  int max;
  int *pointer;
};
#ifdef __LINUX__
#define XP_UNIX
typedef unsigned char BYTE;
#else
#define XP_PC
#endif
// new includes for VC 6
// Include files
#include <vector>
#include <map>
#include <fstream>
#include <strstream>
#include <iostream>

#include <cstdio>
#include <cstdlib>
#include <string>
#include <ctime>
#include <cmath>
#include <sys/types.h>
#ifdef __NT__
	#ifdef _BORLAND_
		#include <condefs.h>
	#endif
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <winsock2.h>
	#include <process.h>
	#include <cstddef>
	#include <winbase.h>
#ifdef  __MINGW32__
	#include <limits.h>
#else
	#include <limits>
#endif
	#include <conio.h>
	#include <sys/timeb.h>
	typedef long int32;
#else
	#include <ctype.h>
	#include <netinet/in.h>
	#include <sys/socket.h>
	#include <sys/time.h>
	#include <netdb.h>
	#include <sys/signal.h>
	#ifdef __LINUX__				// Tseramed's stuff
		#include <unistd.h>
		#include <arpa/inet.h>
	#endif
#endif

using namespace std;

#include "typedefs.h"
#include "uoxstruct.h"
#include "im.h"
#include "uoxlist.h"
#include "scriptc.h"

#ifndef __FILIO_H
	#include "fileio.h"
#endif
#include "msgboard.h"
#include "classes.h"
#include "books.h"
#include "boats.h"//Boats
#include "worldmain.h"
#include "regions.h"
#include "xgm.h"

#include "craces.h"
#include "cshop.h"
#include "cweather.h"

#ifdef _BORLAND_
int iTempBuff;
typedef double				R64;
typedef float				R32;
typedef unsigned long int	UI32;
typedef signed long int		SI32;
typedef unsigned int		UI16;
typedef signed int			SI16;
typedef unsigned char		UI08;
typedef signed char			SI08;

typedef int					GUILDID;
typedef unsigned int		TIMERVAL;
typedef int					UOXSOCKET;
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

#endif
// LINUX Definitions
#ifndef __NT__
inline int min(int a, int b) { if (a < b) return a; return b; }
inline int max(int a, int b) { if (a > b) return a; return b; }
extern "C" {
char *strlwr(char *);
char *strupr(char *);
};
#endif
// clientversion 0 -> compiles new crypt stuff (1.26.0), doesnt work yet
// clientversion 32..35, 37 -> compiles 2 key old encryption ( client version 1.32..1.35, +1.37)
// clietnversion 36         -> compiles 6 keys old encryption ( client version 1.25.36)


#define CLIENTVERSION 2   // clientversion 0..30  -> 1.26.x
                          // clientversion 31..37 -> 1.25.x
#define CLIENT_SUB ""

#if CLIENTVERSION <=30
#define CLIENTVERSION_M 26
#endif

extern unsigned int showloggedoutpcs;
//////////////////
/*
RELEASED/UNRELEASED <-- Is program a release version? (To remove some debug features)
DEBUG/NODEBUG <-- Debug: Shows client->server messages in the server window
SPECIAL/NOSPECIAL <-- Special: Give characters a random mask in their starting equipment
SINGLEONLY/NOSINGLEONLY <-- Singleonly: For pre-release testers (Version without multiplayer)
LSERVCHECK/NOLSERVCHECK <-- Nolservcheck: Disable account verification for local testing
LOGALL/NOLOGALL <-- LOGALL: Bugs hunting routines are turned on. Slower execution when compiled.
                                                        Turn it off when releasing.
*/
extern int totalShop;

extern resour_st resources[9];

extern int shopSpawnTime;
extern int totalClassNPC;

extern creat_st creatures[2048]; //LB, stores the base-sound+sound flags of monsters, animals

extern short UOX_PORT;

extern acct_st acctx[MAXACCT];
extern wiped_st wiped[MAXACCT];
extern splInfo_st *spells; //:Terrin: adding variable for spell system "cache" had to make global for skills.cpp as a quick fix

// Global Variables
extern long int whomenudata[(MAXCLIENT+50)*7]; // We store a serial in here, but as an int, it can't store enough!

extern unsigned int uiCurrentTime, ErrorCount;
extern char Loaded;

extern unsigned short int doorbase[DOORTYPES];
extern char skillname[SKILLS+1][20];
extern char spellname[71][25];
extern char login04a[6];
extern char login04b[61];
extern char login04c[18];
extern char login04d[64];
extern char login03[12];
extern char noaccount[3];
extern char nopass[3];
extern char acctblock[3];
extern char pausex[3];			// Tseramed, pause reserved in lunux

extern unsigned char w_anim[3];

extern char noweather[MAXCLIENT+1];  //LB
extern char restart[3];
extern unsigned char goxyz[20];
extern char wearitem[16];
extern unsigned char talk[15];
extern char sysname[31];
extern char removeitem[6];
extern char gmprefix[10];
extern char gmmiddle[5];
extern unsigned char sfx[13];
extern char doact[15];
extern char bpitem[20];
extern char gump1[22];
extern char gump2[4];
extern char gump3[3];
extern char dyevat[10];
extern char updscroll[11];
extern char spc[2];
extern char bounce[3];
extern char extmove[18];
extern unsigned char attackok[6]; // AntiChrist! 26/10/99
extern unsigned char LSD[MAXCLIENT];
extern bool firstpacket[MAXCLIENT];
extern long idleTimeout[MAXCLIENT];
extern unsigned int raindroptime;
extern unsigned int polyduration;

#ifdef __NT__
extern WSADATA wsaData;
extern WORD wVersionRequested;
#endif

extern int gDecayItem; //Last item that was checked for decay Tauriel 3/7/99
extern int gRespawnItem; //Last item that was checked for respawn Tauriel 3/7/99

extern int save_counter; // LB, world backup rate

extern unsigned int fly_p; // flying probability = 1/fly_p each step ( if it doesn't fly )
extern unsigned char fly_steps_max;

extern int gatex[MAXGATES][2],gatey[MAXGATES][2];
extern signed char gatez[MAXGATES][2];
extern int gatecount;
extern int totalspawnregions; //Zippy
//Time variables
extern int day, hour, minute, ampm; //Initial time is noon.
extern int secondsperuominute; //Number of seconds for a UOX minute.
extern int uotickcount;
extern int moon1update;
extern int moon2update;
extern int hbu;       // heartbeat update var
extern char moon1;
extern char moon2;
extern char dungeonlightlevel;
extern unsigned char worldfixedlevel;
extern char worldcurlevel;
extern char worldbrightlevel;
extern char worlddarklevel;

extern int goldamount;
extern char goldamountstr[10];

extern char defaultpriv1str[2];
extern char defaultpriv2str[2];

extern int defaultpriv1;
extern int defaultpriv2;

extern int GetCombatResult[2];
extern combat_st combat;
void loadCombat(); // Load Combat

extern int currline;

extern unsigned int nextfieldeffecttime;
extern unsigned int nextnpcaitime;
extern unsigned int nextdecaytime;

extern short int max_tele_locations;
extern short int tele_locations[MAX_TELE_LOCATIONS][6];

// MSVC fails to compile UOX if this is unsigned, change it then
#ifdef _MSVC
extern long int oldtime, newtime;
#else
extern unsigned long int oldtime, newtime;      //for autosaving
#endif
extern int autosaved, saveinterval, heartbeat;

extern unsigned int len_connection_addr;
extern struct sockaddr_in connection;
extern struct sockaddr_in client_addr;
extern struct hostent *he;
extern char hname[40];
extern int err, error;
extern int keeprun;
extern int a_socket;
extern int client[MAXCLIENT];
extern int server[MAXCLIENT];
extern fd_set conn;
extern fd_set all;
extern fd_set errsock;
extern int nfds;
extern fd_set UDPconn;
extern fd_set UDPall;
extern fd_set UDPerrsock;
extern int UDPnfds;

extern timeval uoxtimeout;
extern unsigned int now;
extern int newclient[MAXCLIENT];
extern unsigned char buffer[MAXCLIENT][MAXBUFFER];
extern unsigned char outbuffer[MAXCLIENT][MAXBUFFER];
extern unsigned char tbuffer[MAXBUFFER];
extern short int walksequence[MAXCLIENT];
extern int recvcount;
extern char temp[1024];
extern char temp2[1024];
extern FILE *infile, *scpfile, *lstfile, *wscfile;
extern unsigned int wipecount;
extern unsigned int acctcount;
extern unsigned int servcount;
extern unsigned int startcount;
extern char acctinuse[MAXACCT];
extern char serv[MAXSERV][2][30]; // Servers list
extern char start[MAXSTART][5][30]; // Startpoints list
extern int acctno[MAXCLIENT];
extern unsigned int currchar[MAXCLIENT];
extern int freecharmem[301]; //stores pointers to deleted char struct memory
extern int cmemcheck;        //stores point to freecharmem no. that stores last delete char memory
extern char cmemover;        // 0 means freecharmem buffer has not been overflowed, 1 means overflowed
extern int freeitemmem[501]; //stores pointers to deleted item struct memory
extern int imemcheck;        //stores point to freeitemmem no. that stores last delete item memory
extern char imemover;        // 0 means freeitemmem buffer has not been overflowed, 1 means overflowed
extern char xcounter;
extern char ycounter;        //x&y counter used for placing deleted items and chars
extern char clientip[MAXCLIENT][4];
extern int secure; // Secure mode
extern char fametitle[128];
extern char skilltitle[50];
extern char prowesstitle[50];

extern signed long int inworld[MAXACCT]; // Instalog

//extern char_st *chars;
// - the below structure is for looking up items based on serial #
// - item's serial, owner's serial, char's serial, and container's serial
extern lookuptr_st itemsp[HASHMAX], ownsp[HASHMAX], spawnsp[HASHMAX], contsp[HASHMAX];
extern lookuptr_st charsp[HASHMAX], cownsp[HASHMAX], cspawnsp[HASHMAX];
extern lookuptr_st imultisp[HASHMAX], cmultisp[HASHMAX], glowsp[HASHMAX];

extern location_st location[4000];
extern logout_st logout[1024];//Instalog
extern region_st region[256];
extern spawnregion_st spawnregion[512];//Zippy
extern skill_st skill[SKILLS+1];
extern advance_st statadvance[1000];
extern make_st itemmake[MAXCLIENT];
extern int locationcount;
extern unsigned int logoutcount;//Instalog
extern gmpage_st gmpages[MAXPAGES];
extern gmpage_st counspages[MAXPAGES];

extern jail_st jails[11];

extern int tempint[MAXCLIENT];

extern unsigned long int updatepctime;
extern signed char addid5[MAXCLIENT];

extern unsigned int charcount;
extern unsigned int charcount2;
extern unsigned int itemcount;
extern unsigned int itemcount2;
extern unsigned int imem;
extern unsigned int cmem;

extern char *cline;
extern char *comm[CMAX];
extern int tnum;
extern unsigned char addid1[MAXCLIENT];
extern unsigned char addid2[MAXCLIENT];
extern unsigned char addid3[MAXCLIENT];
extern unsigned char addid4[MAXCLIENT];
extern int npcshape[5]; //Stores the coords of the bouding shape for the NPC
extern char dyeall[MAXCLIENT];
extern int addx[MAXCLIENT];
extern int addy[MAXCLIENT];
extern int addx2[MAXCLIENT];
extern int addy2[MAXCLIENT];
extern signed char addz[MAXCLIENT];
extern int addmitem[MAXCLIENT];
extern char xtext[MAXCLIENT][31];
extern char perm[MAXCLIENT];
extern char uoxmonitor[100]; // monitor is already a function under unix
extern unsigned int starttime, endtime, lclock;
extern char overflow;
extern char idname[256];
extern char globallight;
extern char wtype;
extern char script1[1024];
extern char script2[1024];
extern char script3[1024];
extern char script4[1024]; // added to use for newbie items
extern char pass1[256];
extern char pass2[256];
extern int executebatch;
extern int showlayer;
extern int ph1, ph2, ph3, ph4;
extern int openings;
extern int binlength[MAXIMUM+1];
extern int boutlength[MAXIMUM+1];
extern char tempflag;
extern char xoutbuffer[MAXBUFFER*2];
extern char cryptclient[MAXCLIENT];
extern char usedfree[MAXCLIENT];
extern int freelogins;
extern int resendweathertime;
extern int weathertime;
extern int shopTime;
extern int shoprestocktime;
extern int shoprestockrate;
extern unsigned int respawntime;
extern unsigned int gatedesttime;
extern int stablockcachex[STABLOCKCACHESIZE];
extern int stablockcachey[STABLOCKCACHESIZE];
extern signed char stablockcachez[STABLOCKCACHESIZE];
extern int stablockcacher[STABLOCKCACHESIZE];
extern int stablockcachei;
extern int stablockcachehit;
extern int stablockcachemiss;
extern int layers[MAXLAYERS];
extern char gettokenstr[256];
extern int donpcupdate;
extern tracking_st tracking_data;
extern begging_st begging_data;
extern fishing_st fishing_data;
extern spiritspeak_st spiritspeak_data;
extern speed_st speed;//Lag Fix -- Zippy
extern server_st server_data;
extern title_st title[ALLSKILLS+1];
extern int triggerx;
extern int triggery;
extern signed char triggerz;
extern unsigned int hungerdamagetimer; // Used for hunger damage
extern char xgm;

// Profiling
extern int networkTime;
extern int timerTime;
extern int autoTime;
extern int loopTime;
extern int networkTimeCount;
extern int timerTimeCount;
extern int autoTimeCount;
extern int loopTimeCount;


extern char sScriptPath[256];
extern Script *i_scripts[NUM_SCRIPTS]; // array of script references
extern char n_scripts[NUM_SCRIPTS][32]; // array of script filenames
extern vector< int > menuList;

//
// Function Declarations
//

void updatehtml();
void offlinehtml();
void start_glow( void );
void setLastOn( UOXSOCKET s );
void safeCopy( char *dest, const char *src, unsigned int maxLen );

int GetPackOwner(int p);


//Boats --Fucntions in Boats.cpp
void sendinrange(int);
int dist(int,int,int);
int findmulti(int x, int y, signed char z);
int inmulti(int x, int y, signed char z, int m);
void itemtalk(int,int,char *);//Boats
//End Boat functions

extern void init_creatures(void);

void startchar(int s);
void TellScroll( char *menu_name, int player, long item_param );
void sendbpitem(int s, int i);

void doubleclick(int s);
void singleclick(int s);
char *RealTime(char *time_str);

void deny(int k, int s, int sequence);
void teleporters(int s);
void bolteffect2(int player,char a1, char a2);
void all_items(int s);
void savelog(const char *msg, char *logfile);
void staticeffect(int player, unsigned char eff1, unsigned char eff2, char speed, char loop); //for chars
void staticeffect2(int nItem, unsigned char eff1, unsigned char eff2, char speed, char loop, char explode); //for items
void staticeffect3(UI16 x, UI16 y, SI08 z, unsigned char eff1, unsigned char eff2, char speed, char loop, char explode);

void movingeffect(int source, int dest, unsigned char eff1, unsigned char eff2, char speed, char loop, char explode);
void movingeffect2(int source, int dest, char eff1, char eff2, char speed, char loop, char explode);
void movingeffect3(CHARACTER source, unsigned short x, unsigned short y, signed char z, char eff1, char eff2, char speed, char loop, char explode);

void explodeitem(int s, unsigned int nItem);
void bolteffect(int player);
void monstergate(int s, int x);
//void npcMovement2(unsigned int, int);//Lag fix -- Zippy
void npcMovement(unsigned int);
void Karma(int nCharID,int nKilledID, int nKarma);
void Fame(int nCharID, int nFame);
void charstartup(int s);
void checkdumpdata(unsigned int currenttime); // This dumps data for Ridcully's UOXBot 0.02 (jluebbe@hannover.aball.de)
//void weather(int s);
void weather(int s, char bolt);
void killall(int s, int percent, char* sysmsg);

// functions in necro.cpp
int SpawnRandomMonster(UOXSOCKET nCharID, char* cScript, char* cList, char* cNpcID);
int SpawnRandomItem(UOXSOCKET nCharID,int nInPack, char* cScript, char* cList, char* cItemID);
void vialtarget(int nSocket);
void MakeNecroReg( UOXSOCKET nSocket, ITEM nItem, unsigned char cItemID1, unsigned char cItemID2 );

#ifndef __NT__
unsigned long int getclock();
#endif
#ifdef __NT__
#define getclock() clock()
#else
#undef CLOCKS_PER_SEC
#define CLOCKS_PER_SEC 100
#endif

void cleanup(int s);
void npcaction(int npc, int x);
int calcgold(int p);
int packitem(int p);
void titletarget(int s);
int ishuman( int p );
void npcact(int s);
void objTeleporters(int s);
void npcToggleCombat(int s);
int chardir(int a, int b);
int calcSocketFromChar(int i);
int calcItemFromSer(unsigned char ser1, unsigned char ser2, unsigned char ser3, unsigned char ser4);
int calcItemFromSer( int ser ); // Added by Magius(CHE) (2)
int calcCharFromSer(unsigned char ser1, unsigned char ser2, unsigned char ser3, unsigned char ser4);
int calcCharFromSer(int ser);
int calcSerFromChar(int ser);
int calcLastContainerFromSer( int ser ); // Magius(che) (2)
int checkBoundingBox(int xPos, int yPos, int fx1, int fy1, int fz1, int fx2, int fy2);
int checkBoundingCircle(int xPos, int yPos, int fx1, int fy1, int fz1, int radius);
void updatechar(int c);
int unmounthorse(int s);
void swordtarget(int s);
void telltime(int s);
void visibletarget(int s);
void impaction(int s, int act);
int chardirxyz(int a, int x, int y, int z);
int fielddir(int s, int x, int y, int z);
int checkforchar(int x, int y, int z);
char tempeffect(int source, int dest, int num, char more1, char more2, char more3, int targItemPtr = -1 );
char tempeffect2(int source, int dest, int num, char more1, char more2, char more3, int targItemPtr = -1 );
void tempeffectsoff();
void tempeffectson();
void checktempeffects();
void npcattacktarget( CHARACTER target2, CHARACTER target );
void npcsimpleattacktarget(int target2, int target);
void npcaitarget(int s);
int RandomNum(int nLowNum, int nHighNum);
void enlist(int s, int listnum); // For enlisting in army

// Day and Night related prototypes
void doworldlight(void);
void dolight(int s, char level);
char indungeon(int s);
void setabovelight(char);

void tweakmenu(int s, int j, int type);
int validtelepos(int s);
void showcname (int s, int i, char b);
void addhere(int s, char z);
void whomenu(int s, int type);
void gmmenu(int s, int m);
void scriptcommand (int s, char *script1, char *script2);
void endmessage(int x);

//For setting the various npc wandering vars
void npcwandertarget(int s);
void npctarget(int s);
void npctarget2(int s);
void npcrecttarget(int s);
void npccircletarget(int s);
void goldsfx(int s, int goldtotal, bool bAllHear = false);

//	EviLDeD	-	I noticed that this was prototyped. Just thought
//				I would take the liberty to do so
//	December 23, 1998
void gcollect();
//	February 27, 2000
void consolebroadcast(char *txt);
void CheckConsoleKeyThread(void *params);
void checkkey();
//	EviLDeD	-	End

int compare_charst (const char_st *a, const char_st *b);
int compare_itemst (const item_st *a, const item_st *b);

char iteminrange (int s, int i, int distance);
void updateskill(int s, int skillnum);
char npcinrange (int s, int i, int distance);  //check for horse distance...
void openbank(int s, int i);
void openspecialbank( int s, int i ); // AntiChrist
char inbankrange(int i);
void deathaction(int s, int x);
void deathmenu(int s);
int  getamount(int s, unsigned char id1, unsigned char id2);
int  getsubamount(int p, char id1, char id2);
int  delequan(int s, int id1, int id2, int amount);
int  delesubquan(int p, int id1, int id2, int amount);
void movetobagtarget(int s);
void dupetarget(int s);
void impowncreate(int s, int i, int z); //socket, player to send, send z (0) or dispz (1)
void gettokennum(char * s, int num);
void setrandomname(int s, char * namelist);
void addrandomitem(int s, char * itemlist, int spawnpoint);
void donewithcall(int s, int type);
void initque();
void gmopentarget(int s);
void manatarget(int s);
void staminatarget(int s);
void makeshoptarget(int s);
void buyshoptarget(int s);
void choice(int s);
void mounthorse(int s, int x);
void openbook(int s, int i);
char *title1(CHARACTER p);
char *title2(CHARACTER p);
char *title3(CHARACTER p);
void doregionspawn(int r);
void sendshopinfo(int s, int c, int i);
int getname(int i, char* itemname);
void buyaction(int s);
void restockitem(int, unsigned int);//Lag Fix -- Zippy
void restock(int s);
void setrestocktarget(int s);
void setvaluetarget(int s);
void dooruse(int s, int item);

int response(int s);
void responsevendor(int s);
void attacktarget(int s);
void followtarget(int s);
void transfertarget(int s);
void fetchtarget(int s);
void who(int s);
void gms(int s);
void soundeffect3(int p, unsigned char a, unsigned char b);
void playmonstersound(int monster, int id1, int id2, int sfx);
void playTileSound( UOXSOCKET s );
void getsellsubitem( int npc, int p, int q, unsigned char *m1, int &m1t );
void sellstufftarget(int s);
int sellstuff(int s, int i);
void sellaction(int s);
void addgold(int s, int totgold);
void playmidi(int s, char num1, char num2);
void tradetesttarget(int s);
void statwindow(int s, int i);
void usepotion(int p, int i);
int calcValue(int i, int value);
int calcGoodValue( int npcnum, int i, int value, int goodtype ); // by Magius(CHE) for trade system
void StoreItemRandomValue( int i, int tmpreg ); // by Magius(CHE) (2) for trade system

int tradestart(int s, int i);
void clearalltrades();
void trademsg(int s);
void endtrade(int b1, int b2, int b3, int b4);
void sendtradestatus(int cont1, int cont2);
void dotrade(int cont1, int cont2);
void loadspawnregions();//Zippy
void loadregions();
void checkregion(int i);
#ifndef __LINUX__
char calcRegionFromXY(int x, int y);
#else
short calcRegionFromXY( int x, int y );
#endif
void dosocketmidi(int s);
void wipe(int s);
void respawnnow();

void tellmessage(int i, int s, unsigned char *txt);

void setwipetarget(int s);
void setspeechtarget(int s);
void xteleport(int s,int x);
void sysmessage(int, char *, ...);
void senditem(UOXSOCKET s, ITEM i);
void wornitems( UOXSOCKET s, CHARACTER j );
void RefreshItem( ITEM i ); // AntiChrist
void soundeffects(int s, unsigned char a, unsigned char b, bool bAllHear = false);
void soundeffect(int s, unsigned char a, unsigned char b);
void soundeffect2(int p, unsigned char a, unsigned char b);
void updatestats(int c, char x);
void action(int s, int x);
int str2num (char *s);
int hstr2num (char *s);
void numtostr( int i, char *string );
void hextostr( int i, char *string );
void closescript ();
unsigned char openscript (char *name);
FILE *openscript( char *name, FILE *toOpen );

void itemmenu(int s, int m);

int FindNameMenu( char * );
void LoadNameMenus( void );

void npcemote(int s, int npc, char *txt, char antispam);
void npcemoteall(int npc, char *txt, char antispam);
void target(UOXSOCKET s, unsigned char a1, unsigned char a2, unsigned char a3, unsigned char a4, char *txt);
int findsection (char *s);
void read1( void );
void read2( void );
void read2( FILE *toRead ); 
void itemmessage(UOXSOCKET s, char *txt, unsigned char a1, unsigned char a2, unsigned char a3, unsigned char a4, unsigned char loColour = 0x03, unsigned char hiColour = 0xB2 );
int inrange1( UOXSOCKET a, UOXSOCKET b );
int inrange1p (CHARACTER a, CHARACTER b);
int inrange2 (UOXSOCKET s, ITEM i);
void deathstuff(int i);
int online(CHARACTER c);
void teleport(int s);
void npctalkall(int npc, char *txt, char antispam);
void npctalk(int s, int npc, char *txt, char antispam); // NPC speech
void cantraintarget(int s); //NPC Training
void backpack(UOXSOCKET s, unsigned char a1, unsigned char a2, unsigned char a3, unsigned char a4);
void loadserverscript(void);
void saveserverscript(char);
void loadserverdefaults(void);
void loadserver(void);
void loadtracking(void);
void loadspiritspeak(void);
void loadbegging(void);
void loadfishing(void);
void loadtime_light();
void loadskills();
void loadhunger( void ); // By Magius(CHE)
void loadvendor( void ); // By Magius(CHE)
void loadregenerate( void ); // By Magius(CHE)
int numbitsset( int number );
int whichbit( int number, int bit );
unsigned int chardist( CHARACTER a, CHARACTER b );
unsigned int itemdist( CHARACTER a, int i );
void sysbroadcast( char *txt);
void target(UOXSOCKET s, unsigned char a1, unsigned char a2, unsigned char a3, unsigned char a4, char *txt);

void usehairdye(int s, int x);
void UseHairDye( UOXSOCKET s, short int colour, int x );
void buildhouse(int s, int i);
void deedhouse(int s, int i); //crackerjack 8/9/99
void killkeys(unsigned char s1, unsigned char s2, unsigned char s3, unsigned char s4); // crackerjack 8/11/99
// house list functions - cj 8/12/99
int on_hlist(int h, unsigned char s1, unsigned char s2, unsigned char s3, unsigned char s4, int *li);
int add_hlist(int c, int h, int t);
int del_hlist(int c, int h);
//
void house_speech(int s, unsigned char *talk);
void addthere(int s, int xx, int yy, int zz, int t);
void mtarget(int s, unsigned char a1, unsigned char a2, unsigned char a3, unsigned char a4, unsigned char b1, unsigned char b2, char *txt);


void lockpick(int s);

#ifdef  __NT__
void Writeslot(LPSTR lpszMessage);
#else
void Writeslot(char *lpszMessage);
#endif

int getstatskillvalue(char *stringguy);
// for newbie stuff
int bestskill( CHARACTER p );
int secndbstskll(int m, int bstskll);
int thrdbstskll(int n, int scnbst);
//void addfromitemlist(int itemlist,int c);
void newbieitems(unsigned int c);
void read3 (); 
void read4 ();
void readscript( void );
void readscript( FILE *toReadFrom );

//For custom titles
void loadcustomtitle();

// Profiling
//void StartMilliTimer(unsigned long &Seconds, unsigned long &Milliseconds);
//unsigned long CheckMilliTimer(unsigned long &Seconds, unsigned long &Milliseconds);
#ifdef __NT__
	inline void StartMilliTimer( UI32 &Seconds, UI32 &Milliseconds ) { struct timeb t; ftime( &t ); Seconds = t.time; Milliseconds = t.millitm; };
	inline UI32 CheckMilliTimer( UI32 &Seconds, UI32 &Milliseconds ) { struct timeb t; ftime( &t ); return( 1000 * ( t.time - Seconds ) + ( t.millitm - Milliseconds ) ); };
#else
	inline void StartMilliTimer( UI32 &Seconds, UI32 &Milliseconds ) 
	{ 
		struct timeval t; 
		gettimeofday( &t, NULL ); 
		Seconds = t.tv_sec; 
		Milliseconds = t.tv_usec; 
	};
	inline UI32 CheckMilliTimer( UI32 &Seconds, UI32 &Milliseconds ) { struct timeval t; gettimeofday( &t, NULL ); return( 1000 * ( t.tv_sec - Seconds ) + ( t.tv_usec - Milliseconds ) ); };
#endif

void updates( int s );
void advancementobjects(int s, int x, int always);
void itemsfx(int s, int ID1, int ID2, bool bAllHear = false);
void bgsound(int s);
void splitline();
int hexnumber(int countx);
int makenumber(int countx);

inline long bitCheck( long toCheck, long bit ) { return toCheck&bit; };
inline int bitCheck( int toCheck, int bit ) { return toCheck&bit; };
inline char bitCheck( char toCheck, char bit ) { return toCheck&bit; };
int makenum2( char *s );
inline int RandomNum( int nLowNum, int nHighNum )
{
	if( nHighNum - nLowNum + 1 )
		return ((rand() % ( nHighNum - nLowNum + 1 )) + nLowNum );
	else
		return nLowNum;
};
// Dupois - fileArchive() prototypes
// Added Oct 20, 1998
void fileArchive(char *pFile2Archive_chars, char *pFile2Archive_items, char *pArchiveDir);
void ArchiveID(char archiveid[MAXARCHID]);
// End - Dupois

void enlist(int s); // For enlisting in army

//Trigger routines
void triggerwitem(int ts, int ti, int ttype);  // trigger.cpp
void triggernpc(int ts,int ti) ;  // trigger.cpp
int checkenvoke(char eid1, char eid2);  //trigger.scp

inline int calcserial(unsigned char a1,unsigned char a2,unsigned char a3,unsigned char a4) {return ((a1*16777216)+(a2*65536)+(a3*256)+a4);}	

// Pointer.cpp functions
// - set item in pointer array
void setptr(lookuptr_st *ptr, int item);
// - remove item from pointer array
int removefromptr(lookuptr_st *ptr, int nItem);
// - find item in a pointer array
int findbyserial(lookuptr_st *ptr, int nSerial, int nType);
// - set serial#s and ptr arrays
void setserial(int nChild, int nParent, int nType);
void unsetserial( int nChild, int nType );

// the stuff McCleod added
void setsplittarget(int s);
void setsplitchancetarget(int s);
//void possess(int s); //Not done
char line_of_sight(int s, short int x1, short int y1, int z1, short int x2, short int y2, int z2, int checkfor);

// Dupois message board prototype
// Proto for handling the different message type for message 0x71
void    MsgBoardEvent(int nSerial);

void Shutdown( int retCode );

void batchcheck( int s );
void weblaunch(int s, char *txt);
void readw2( void );
void readw3( void );
void entrygump(int s, unsigned char tser1, unsigned char tser2, unsigned char tser3, unsigned char tser4, char type, char index, short int maxlength, char *text1);
SI32 grabNumber( int startPos, int length, UOXSOCKET s, char base = 10 );

// DasRaetsels' stuff up, don't touch :)

extern char da;
int addrandomcolor(int s, char *colorlist);
int addrandomhaircolor(int s, char *colorlist);

extern repsys_st repsys;
extern resource_st resource;
void criminal(int c);
void setcharflag(int c);
void loadrepsys();
void loadresources();
void reverse_effect(int i);//Morrolan bugfix
int recursestatcap(int chr); //Morrolan - stat/skill cap
void skillfreq(int chr, int skill); //Morrolan - stat/skill cap
void	SendVecsAsGump( UOXSOCKET sock, stringList& one, stringList& two, unsigned char type );
//-=-=-=-=-=-=-Classes Definitions=-=-=-=-=-=//
extern cBoat			*Boats;
extern cRegion			*mapRegions; //setup map regions Tauriel
extern CWorldMain		*cwmWorldState;
extern cAdmin			*Admin;
extern cCombat			*Combat;
extern cCommands		*Commands;
extern cGuilds			*Guilds;
extern cGump			*Gumps;
extern cItem			*Items;
extern cMapStuff		*Map;
extern cCharStuff		*Npcs;
extern cSkills			*Skills;
extern cTownStones		*Towns;
extern cWeight			*Weight;
extern cTargets			*Targ;
extern cNetworkStuff	*Network;
extern cMagic			*Magic;
extern cRaces			*Races;
extern cWeatherAb		*Weather;
extern cRemote			*xGM[MAXCLIENT];
extern cMovement        *Movement;
extern cTEffect			*Effects;
extern cWhoList			*WhoList;
extern cWhoList			*OffList;
extern cBooks			*Books;

extern unsigned int lighttime;
//:Terrin: MIN/MAX macros need the parens. to handle complex parameters 
#define MIN(arga, argb) ( (arga) < (argb) ? (arga) : ( argb ) )


extern cItemHandle items;
extern cCharacterHandle chars;

extern int *loscache; 
extern int *itemids;
extern int *clickx;
extern int *clicky;
extern int *currentSpellType; // 0=spellcast, 1=scrollcast, 2=wand cast
extern char *targetok;
extern int UDPsock;
extern int lenConnAddr;
extern sockaddr_in connUDP;

extern int escortRegions;
extern int validEscortRegion[256];

void readw2( void );
void readw3( void );

void MsgBoardEvent( int nSerial );
void AddToMenuList( int toAdd );
bool IsInMenuList( int toCheck );
int GenerateCorpse( CHARACTER i, int nType, char *murderername );
void PlayDeathSound( CHARACTER i );
void KillTrades( CHARACTER i );
void NeutralizeEnemies( CHARACTER i );
void breakConcentration( CHARACTER p , UOXSOCKET s = -1 );
int GetBankCount( CHARACTER p, unsigned short itemID, unsigned short colour = 0x0000 );
int DeleBankItem( CHARACTER p, unsigned short itemID, unsigned short colour, int amt );
void Kill( CHARACTER attack, CHARACTER defend );

#endif // __UOX3_H
