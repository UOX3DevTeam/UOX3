//o---------------------------------------------------------------------------o
//| Globals.cpp
//o---------------------------------------------------------------------------o
//| Many global variables are set here
//o---------------------------------------------------------------------------o
//| Version History
//| 1.3		Added comments and changed int variables into typefefs
//|         (Mr. Fixit 5. November 2001)
//|
//| 1.4		Reorganized a bit to match with uox3.h, many variables once stored
//|			here are now in CWorldMain or CServerData classes.
//|			(giwo, October 16, 2003)
//|
//|	1.5		Removed most class assignments, letting the classes header files and
//|			InitClasses() do their own work, this also removes them from global scope
//o---------------------------------------------------------------------------o

#include "uox3.h"
#include "cThreadQueue.h"

namespace UOX
{

//o---------------------------------------------------------------------------o
// Item / Character Variables
//o---------------------------------------------------------------------------o
const char skillname[SKILLS+1][20]={
"ALCHEMY", "ANATOMY", "ANIMALLORE", "ITEMID", "ARMSLORE", "PARRYING", "BEGGING", "BLACKSMITHING", "BOWCRAFT",
"PEACEMAKING", "CAMPING", "CARPENTRY", "CARTOGRAPHY", "COOKING", "DETECTINGHIDDEN", "ENTICEMENT", "EVALUATINGINTEL",
"HEALING", "FISHING", "FORENSICS", "HERDING", "HIDING", "PROVOCATION", "INSCRIPTION", "LOCKPICKING", "MAGERY",
"MAGICRESISTANCE", "TACTICS", "SNOOPING", "MUSICIANSHIP", "POISONING", "ARCHERY", "SPIRITSPEAK", "STEALING",
"TAILORING", "TAMING", "TASTEID", "TINKERING", "TRACKING", "VETERINARY", "SWORDSMANSHIP", "MACEFIGHTING", "FENCING",
#ifdef __NEW_SKILLS__
"WRESTLING", "LUMBERJACKING", "MINING", "MEDITATION", "STEALTH", "REMOVETRAPS", "NECROMANCY", "FOCUS", "CHIVALRY",
#else
"WRESTLING", "LUMBERJACKING", "MINING", "MEDITATION", "STEALTH", "REMOVETRAPS",
#endif
"ALLSKILLS", "STR", "DEX", "INT", "FAME", "KARMA"
};

//o---------------------------------------------------------------------------o
// Classes Definitions
//o---------------------------------------------------------------------------o
QUEUEMAP						refreshQueue;
QUEUEMAP						deletionQueue;
std::vector< CTownRegion * >	regions;
std::vector< CSpawnRegion *>	spawnregions;		// Regionspawns

//o---------------------------------------------------------------------------o
// JS Stuff
//o---------------------------------------------------------------------------o
JSRuntime *		jsRuntime; 
JSContext *		jsContext; 
JSObject *		jsGlobal; 
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

}
