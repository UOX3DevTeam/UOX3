
//o---------------------------------------------------------------------------o
//| Globals.cpp
//o---------------------------------------------------------------------------o
//| Most of the global variables are set here
//o---------------------------------------------------------------------------o
//| Version History
//| 1.3		Added comments and changed int variables into typefefs
//|         (Mr. Fixit 5. November 2001)
//o---------------------------------------------------------------------------o


// Global Variables

#include "uox3.h"

creat_st creatures[2048];

UI08 GMCMDLEVEL = 2;
UI08 CNSCMDLEVEL = 1;

UI32 uiCurrentTime, ErrorCount;
char Loaded;
UI16 doorbase[DOORTYPES] = {
0x0675, 0x0685, 0x0695, 0x06A5, 0x06B5, 0x06C5, 0x06D5, 0x06E5, 0x0839, 0x084C, 
0x0866, 0x00E8, 0x0314, 0x0324, 0x0334, 0x0344, 0x0354, 0x0824, 0x190E
};

char skillname[SKILLS+1][20]={
"ALCHEMY", "ANATOMY", "ANIMALLORE", "ITEMID", "ARMSLORE", "PARRYING", "BEGGING", "BLACKSMITHING", "BOWCRAFT",
"PEACEMAKING", "CAMPING", "CARPENTRY", "CARTOGRAPHY", "COOKING", "DETECTINGHIDDEN", "ENTICEMENT", "EVALUATINGINTEL",
"HEALING", "FISHING", "FORENSICS", "HERDING", "HIDING", "PROVOCATION", "INSCRIPTION", "LOCKPICKING", "MAGERY",
"MAGICRESISTANCE", "TACTICS", "SNOOPING", "MUSICIANSHIP", "POISONING", "ARCHERY", "SPIRITSPEAK", "STEALING",
"TAILORING", "TAMING", "TASTEID", "TINKERING", "TRACKING", "VETERINARY", "SWORDSMANSHIP", "MACEFIGHTING", "FENCING",
"WRESTLING", "LUMBERJACKING", "MINING", "MEDITATION", "STEALTH", "REMOVETRAPS", "ALLSKILLS", "STR", "DEX", "INT", "FAME", "KARMA"
};

//combat_st combat;
UI08 talk[15]="\x1C\x00\x00\x01\x02\x03\x04\x01\x90\x00\x00\x38\x00\x03";
char gmprefix[10]="\x7C\x00\x00\x01\x02\x03\x04\x00\x64";
char gmmiddle[5]="\x00\x00\x00\x00";
char bpitem[20]="\x40\x0D\x98\xF7\x0F\x4F\x00\x00\x09\x00\x30\x00\x52\x40\x0B\x00\x1A\x00\x00";
char gump1[22]="\xB0\x04\x0A\x40\x91\x51\xE7\x00\x00\x00\x03\x00\x00\x00\x6E\x00\x00\x00\x46\x02\x3B";
char gump2[4]="\x00\x00\x00";
char gump3[3]="\x00\x00";
char updscroll[11]="\xA6\x01\x02\x02\x00\x00\x00\x00\x01\x02";
char spc[2]="\x20";
char xgm;
#ifdef __NT__
WSADATA wsaData;
WORD wVersionRequested;
#endif

UI32 polyduration = 90;
UI32 totalspawnregions = 0;
//Time variables
UI16 secondsperuominute = 5;					// Number of seconds for a UOX minute. Changed from int to UI16 (Mr. Fixit)
UI32 uotickcount = 1;							// Changed from int to UI16 (Mr. Fixit)
UI32 nextfieldeffecttime = 0;
UI32 nextnpcaitime = 0;
vector< TeleLocationEntry > teleLocs;


// MSVC fails to compile UOX if this is unsigned, change it then
#ifdef _MSVC
long int oldtime, newtime;
#else
UI32 oldtime, newtime;						//for autosaving
#endif
bool autosaved;
int heartbeat;
//struct hostent *he;
SI32 err;									// Changed from int to SI32 (Mr. Fixit)
bool error;
bool keeprun;
fd_set conn;
fd_set all;
fd_set errsock;
SI32 nfds;									// Changed from int to SI32 (Mr. Fixit)
timeval uoxtimeout;
SI32 now;

bool secure;								// Secure mode
char fametitle[128];
char skilltitle[50];
char prowesstitle[50];

vector< MurderPair > murdererTags;

vector< SpellInfo > spells;					//:Terrin: adding variable for spell system "cache" had to make global for skills.cpp as a quick fix

HashTable< ITEM >			nitemsp;
HashTableMulti< ITEM >		nspawnsp;
HashTable< CHARACTER >		ncharsp;
HashTableMulti< CHARACTER >	ncspawnsp;

int *loscache;								
int *itemids;								

UI32 lighttime = 0;

cItemHandle items;
cCharacterHandle chars;

location_st location[4000];
cTownRegion *region[256];
logout_st logout[1024];						// Instalog
cSpawnRegion *spawnregion[512];				//Regionspawns
skill_st skill[SKILLS+1];
UI16 locationcount;							// Changed from int to UI08 (Mr. Fixit)
UI32 logoutcount;							// Instalog
title_st title[ALLSKILLS+1];				// For custom titles reads titles.scp
vector< JailCell > jails;

UI32 charcount, itemcount;
SERIAL charcount2, itemcount2;
UI32 imem, cmem;

char *comm[CMAX];
SI32 tnum;									// Changed from int to SI32 (Mr. Fixit)

UI32 npcshape[5];							// Stores the coords of the bouding shape for the NPC. DOES NOT NEED TO BE AN ARRAY. Changed from int to UI32 (Mr. Fixit)
UI32 starttime, endtime, lclock;
bool overflow;
char idname[256];
char pass1[256];
char pass2[256];
SI32 executebatch;							// Changed from int to SI32 (Mr. Fixit)
bool showlayer;
SI32 ph1, ph2, ph3, ph4;					// Not used for anything (Mr. Fixit)

UI32 shoprestocktime = 0;					// Changed from int to UI32 (Mr. Fixit)
SI32 shoprestockrate = 5;					// Changed from int to SI32 (Mr. Fixit)
UI32 respawntime=0;
// Profiling
UI32 networkTime = 0;						// Changed from int to UI32 (Mr. Fixit)
UI32 timerTime = 0;							// Changed from int to UI32 (Mr. Fixit)
UI32 autoTime = 0;							// Changed from int to UI32 (Mr. Fixit)
UI32 loopTime = 0;							// Changed from int to UI32 (Mr. Fixit)
UI32 networkTimeCount = 1000;				// Changed from int to UI32 (Mr. Fixit)
UI32 timerTimeCount = 1000;					// Changed from int to UI32 (Mr. Fixit)
UI32 autoTimeCount = 1000;					// Changed from int to UI32 (Mr. Fixit)
UI32 loopTimeCount = 1000;					// Changed from int to UI32 (Mr. Fixit)

UI08 worldSaveProgress = 0;

UI08 escortRegions = 0;							// Changed from int to UI08 (Mr. Fixit)
UI08 validEscortRegion[256];					// Changed from int to UI08 (Mr. Fixit)

UI32 hungerdamagetimer = 0;						// For hunger damage
#ifndef __NT__
char *strlwr( char *str ) 
{
	for( UI32 i = 0; i < strlen( str ); i++ )
		str[i] = tolower( str[i] );
	return str;
}
char *strupr( char *str )
{
	for( UI32 i = 0; i < strlen(str ); i++ )
		str[i] = toupper( str[i] );
	return str;
}
#endif
//int lenConnAddr;							// Not used anywhere (Mr. Fixit)

//-=-=-=-=-=-=-Classes Definitions=-=-=-=-=-=//
CWorldMain			*cwmWorldState = NULL;
cBoat				*Boats = NULL;
cCombat				*Combat = NULL;
cCommands			*Commands = NULL;
cGump				*Gumps = NULL;
cItem				*Items = NULL;
cMapStuff			*Map = NULL;
cCharStuff			*Npcs = NULL;
cSkills				*Skills = NULL;
cWeight				*Weight = NULL;
cTargets			*Targ = NULL;
cNetworkStuff		*Network = NULL;
cMagic				*Magic = NULL;
cRaces				*Races = NULL;
cWeatherAb			*Weather = NULL;
cMovement			*Movement = NULL;
cTEffect			*Effects = NULL;
cWhoList			*WhoList = NULL;
cWhoList			*OffList = NULL;
cBooks				*Books = NULL;
PageVector			*GMQueue = NULL;
PageVector			*CounselorQueue = NULL;
CDictionaryContainer	*Dictionary = NULL;
cAccounts			*Accounts = NULL;
Triggers			*Trigger = NULL;
cMapRegion			*MapRegion = NULL;
CSpeechQueue		*SpeechSys = NULL;
cHTMLTemplates		*HTMLTemplates = NULL;
CGuildCollection	*GuildSys = NULL;
cServerDefinitions	*FileLookup = NULL;

CConsole			Console;	// no *, else we can't overload <<
CThreadQueue		messageLoop;
JailSystem			*JailSys = NULL;

cBaseObject *DefBase = NULL;
CChar *DefChar = NULL;
CItem *DefItem = NULL;


SI32 erroredLayers[MAXLAYERS];						// Changed from long to SI32 (Mr. Fixit)
SI32 globalRecv;									// Changed from long int to SI32 (Mr. Fixit)
SI32 globalSent;									// Changed from long int to SI32 (Mr. Fixit)

JSRuntime *jsRuntime; 
JSContext *jsContext; 
JSObject *jsGlobal; 
JSClass global_class = { 
        "global",0, 
        JS_PropertyStub,JS_PropertyStub,JS_PropertyStub,JS_PropertyStub, 
        JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub 
    }; 
JSClass uox_class = { 
        "uoxscript",0, 
        JS_PropertyStub,JS_PropertyStub,JS_PropertyStub,JS_PropertyStub, 
        JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub 
    }; 

CEndL myendl;


