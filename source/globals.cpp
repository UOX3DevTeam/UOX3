//o---------------------------------------------------------------------------o
//| Globals.cpp
//o---------------------------------------------------------------------------o
//| Most of the global variables are set here
//o---------------------------------------------------------------------------o
//| Version History
//| 1.3		Added comments and changed int variables into typefefs
//|         (Mr. Fixit 5. November 2001)
//o---------------------------------------------------------------------------o

#include "uox3.h"

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

#if !defined(__unix__)
WSADATA wsaData;
WORD wVersionRequested;
#endif

fd_set conn;
fd_set all;
fd_set errsock;

HashTable< ITEM >			nitemsp;
HashTableMulti< ITEM >		nspawnsp;
HashTable< CHARACTER >		ncharsp;
HashTableMulti< CHARACTER >	ncspawnsp;

cItemHandle					items;
cCharacterHandle			chars;

std::vector< SpellInfo > spells;					//:Terrin: adding variable for spell system "cache" had to make global for skills.cpp as a quick fix

creat_st creatures[2048];

int *loscache;
int *itemids;

cTownRegion *region[256];
cSpawnRegion *spawnregion[4098];				//Regionspawns

char *comm[CMAX];

// Profiling
UI32 networkTime = 0;						// Changed from int to UI32 (Mr. Fixit)
UI32 timerTime = 0;							// Changed from int to UI32 (Mr. Fixit)
UI32 autoTime = 0;							// Changed from int to UI32 (Mr. Fixit)
UI32 loopTime = 0;							// Changed from int to UI32 (Mr. Fixit)
UI32 networkTimeCount = 1000;				// Changed from int to UI32 (Mr. Fixit)
UI32 timerTimeCount = 1000;					// Changed from int to UI32 (Mr. Fixit)
UI32 autoTimeCount = 1000;					// Changed from int to UI32 (Mr. Fixit)
UI32 loopTimeCount = 1000;					// Changed from int to UI32 (Mr. Fixit)

SI32 globalRecv;
SI32 globalSent;

#if defined(__unix__)
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
cTEffect			*TEffects = NULL;
cWhoList			*WhoList = NULL;
cWhoList			*OffList = NULL;
cBooks				*Books = NULL;
PageVector			*GMQueue = NULL;
PageVector			*CounselorQueue = NULL;
CDictionaryContainer	*Dictionary = NULL;
cAccountClass			*Accounts = NULL;
Triggers			*Trigger = NULL;
cMapRegion			*MapRegion = NULL;
CSpeechQueue		*SpeechSys = NULL;
cHTMLTemplates		*HTMLTemplates = NULL;
CGuildCollection	*GuildSys = NULL;
cServerDefinitions	*FileLookup = NULL;
cEffects			*Effects = NULL;
cFileIO				*FileIO = NULL;

CConsole			Console;	// no *, else we can't overload <<
CThreadQueue		messageLoop;
JailSystem			*JailSys = NULL;

cBaseObject *DefBase = NULL;
CChar *DefChar = NULL;
CItem *DefItem = NULL;


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


