// Global Variables

#include "uox3.h"

UI32 uiCurrentTime, ErrorCount;
char Loaded;
UI16 doorbase[DOORTYPES] = {
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

//combat_st combat;

UI32 polyduration = 90;
SI16 gatex[MAXGATES][2], gatey[MAXGATES][2];
SI08 gatez[MAXGATES][2];
int gatecount=0;
int totalspawnregions=0;
//Time variables
int secondsperuominute=5; //Number of seconds for a UOX minute.
int uotickcount=1;
int openings=0;
int closings=0;
UI32 nextfieldeffecttime = 0;
UI32 nextnpcaitime = 0;
UI32 nextdecaytime = 0;

#ifdef _MSVC
long int oldtime, newtime;
#else
UI32 oldtime, newtime;      //for autosaving
#endif
bool autosaved;
int saveinterval, heartbeat;
struct hostent *he;
int err;
bool error;
bool keeprun;
fd_set conn;
fd_set all;
fd_set errsock;
int nfds;
timeval uoxtimeout;
SI32 now;
char temp[1024];
char temp2[1024];
FILE *scpfile = NULL, *lstfile = NULL, *wscfile = NULL;

char start[MAXSTART][5][30]; // Startpoints list
char xcounter;
char ycounter;
bool secure; // Secure mode
char fametitle[128];
char skilltitle[50];
char prowesstitle[50];

HashTableItem nitemsp;
HashTableChar ncharsp;

int *loscache;
int *itemids;

UI32 lighttime=0;

UI08 w_anim[3] = { 0x1D, 0x95, 0 }; // flying blood instead of lightnings, LB in a real bad mood	// tseramed changed this from extern to nonextern

int save_counter;

cItemHandle items;
cCharacterHandle chars;

SI32 charcount, itemcount;
SERIAL charcount2, itemcount2;
SI32 imem, cmem;

char *cline;
char *comm[CMAX];
int tnum;

int npcshape[5]; //Stores the coords of the bouding shape for the NPC
char uoxmonitor[100];
UI32 starttime, endtime, lclock;
bool overflow;
char idname[256];
char wtype;
char script1[1024];
char script2[1024];
char script3[1024];
char pass1[256];
char pass2[256];
int executebatch;
bool showlayer;
int ph1, ph2, ph3, ph4;
UI32 fly_p = 18; // flying probability = 1/fly_p each step (if it doesnt fly)
UI08 fly_steps_max = 27;

int shoprestocktime=0;
int shoprestockrate=5;
UI32 respawntime=0;
// Profiling
int networkTime = 0;
int timerTime = 0;
int autoTime = 0;
int loopTime = 0;
int networkTimeCount = 1000;
int timerTimeCount = 1000;
int autoTimeCount = 1000;
int loopTimeCount = 1000;
char gettokenstr[256];

short int triggerx;
short int triggery;
SI08 triggerz;

int escortRegions = 0;
int validEscortRegion[256];

UI32 hungerdamagetimer = 0; // For hunger damage
#ifndef __NT__
char *strlwr(char *str) {
  for (UI32 i=0;i<strlen(str);i++)
    str[i]=tolower(str[i]);
  return str;
}
char *strupr(char *str) {
  for (UI32 i=0;i<strlen(str);i++)
    str[i]=toupper(str[i]);
  return str;
}
#endif
char da;
int lenConnAddr;
UI32 showloggedoutpcs;

//-=-=-=-=-=-=-Classes Definitions=-=-=-=-=-=//
cMapRegion			*MapRegion = NULL;
vector< int > menuList;
long erroredLayers[MAXLAYERS];
long int globalRecv;
long int globalSent;

SI32 charsWritten = 0, itemsWritten = 0;

FILE *errorLog = NULL;