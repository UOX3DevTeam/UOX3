//o---------------------------------------------------------------------------o
//| UOX3.h
//o---------------------------------------------------------------------------o
//| This is the main header file, where alot of stuff are set
//o---------------------------------------------------------------------------o
//| Version History
//| 1.5		Added comments and changed int variables into typefefs
//|         (Mr. Fixit 5. November 2001)
//| 1.6		Removed the warning promotions from level 3 to 4 off.
//|					(EviLDeD 022602)
//o---------------------------------------------------------------------------o


//o---------------------------------------------------------------------------o
// Only run once
//o---------------------------------------------------------------------------o
#define __REENTRANT
#ifndef __UOX3_H
#define __UOX3_H

#define ___UOX3_DTL__

//o---------------------------------------------------------------------------o
// FIXMEs / TODOs / NOTE macros
//o---------------------------------------------------------------------------o
#define _QUOTE(x) # x
#define QUOTE(x) _QUOTE(x)
#define __FILE__LINE__ __FILE__ "(" QUOTE(__LINE__) ") : "
#define NOTE( x )  message( x )
#define FILE_LINE  message( __FILE__LINE__ )
#define TODO( x )  message( __FILE__LINE__"\n"           \
        "+------------------------------------------------\n" \
        "|  TODO :   " #x "\n" \
        "+-------------------------------------------------\n" )
#define FIXME( x )  message(  __FILE__LINE__"\n"           \
        "+------------------------------------------------\n" \
        "|  FIXME :  " #x "\n" \
        "+-------------------------------------------------\n" )
#define todo( x )  message( __FILE__LINE__" TODO :   " #x "\n" )
#define fixme( x )  message( __FILE__LINE__" FIXME:   " #x "\n" )
#define note( x )  message( __FILE__LINE__" NOTE :   " #x "\n" )


//o---------------------------------------------------------------------------o
// product info
//o---------------------------------------------------------------------------o
//#define VER "0.97.01"
// EviLDeD: 012402: Please while we are on a build run for this version make sure
//									you increment this build number? Im am gonna up the build Num
//									here cause Oi was told that MrFixit, and some others were in
//									code and made some changes.
//#define BUILD "14"
//#define REALBUILD "0"
//#define SVER "2.0"
//#define CVER "2.0"
//#define IVER "2.0"
//#define PRODUCT "Ultima Offline eXperiment 3"
//#define NAME "The Official DevTeam"
//#define EMAIL "http://uox3.sourceforge.net/"
//#define PROGRAMMERS "UOX3 DevTeam"


//o---------------------------------------------------------------------------o
// Set what OS UOX is compiled under
//o---------------------------------------------------------------------------o
#ifdef __linux__
	#define __LINUX__
#endif

#ifdef _WIN32
	#define __NT__
	#ifndef __MINGW32__
		#define _MSVC
	#endif
#endif
#ifdef __NT__
	#ifndef _WIN32
		#define _WIN32
	#endif
#endif

#ifdef __LINUX__
	#define OS_STR "Linux"
#else
	#define OS_STR "Win32"
#endif

//o---------------------------------------------------------------------------o
// remove PACKED for unix/linux because it going to cause bus errors - fur
//o---------------------------------------------------------------------------o
#if defined _WIN32 && (!defined(__MINGW32__))
	#define PACK_NEEDED
#else
	#define PACK_NEEDED
#endif


//o---------------------------------------------------------------------------o
// Skip some errors
//o---------------------------------------------------------------------------o
#pragma warning( disable : 4097 )
#pragma warning( disable : 4100 )	// unreferenced formal parameter
#pragma warning( disable : 4201 )	// nameless struct/union
#pragma warning( disable : 4503 )
#pragma warning( disable : 4511 )	// copy constructor could not be generated
#pragma warning( disable : 4512 )	// assignment operator could not be generated
#pragma warning( disable : 4663 )	// C++ language change: to explicitly specialize class template '' use the following syntax...
#pragma warning( disable : 4786 )	// Gets rid of BAD stl warnings


//o---------------------------------------------------------------------------o
// More linux stuff
//o---------------------------------------------------------------------------o
#ifdef __LINUX__
	#define XP_UNIX
	#define closesocket( s )	close( s )
	#define ioctlsocket( s,b,c )	ioctl( s,b,c )
#elif defined _WIN32
	#define XP_PC
#else
# error "Failed to define either __LINUX__ or _WIN32"
#endif


//o---------------------------------------------------------------------------o
// Include files
//o---------------------------------------------------------------------------o
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <ctime>
#include <cmath>
#include <sys/types.h>
#include <assert.h>
#include <sstream>
#include "cVersionClass.h"

//o---------------------------------------------------------------------------o
// OS specific includes
//o---------------------------------------------------------------------------o
#include "socket_interface.h"
#ifdef __NT__
	#define WIN32_LEAN_AND_MEAN
	//#include <windows.h>
	//#include <winsock2.h>
	#include <process.h>
	#include <cstddef>
	#include <winbase.h>
	#include <limits.h>
	#include <conio.h>
	#include <sstream>
	#include <sys/timeb.h>
	typedef long int32;
#else
	#include <ctype.h>
	#include <netinet/in.h>
	#include <sys/socket.h>
	#include <sys/time.h>
	#include <netdb.h>
	#include <sys/signal.h>
	#include <sstream>
	#ifdef __LINUX__				// Tseramed's stuff
		#include <unistd.h>
		#include <arpa/inet.h>
	#endif
#endif


//o---------------------------------------------------------------------------o
// Enable warning
//o---------------------------------------------------------------------------o
//#pragma warning( 3 : 4018 4127 4189 4244 4245 )


//o---------------------------------------------------------------------------o
// Javascript engine includes
//o---------------------------------------------------------------------------o
#include "jsapi.h"


//o---------------------------------------------------------------------------o
// Using std
//o---------------------------------------------------------------------------o
//using namespace std;


//o---------------------------------------------------------------------------o
// The UOX project includes
//o---------------------------------------------------------------------------o
#include "typedefs.h"
#include "uoxstruct.h"
class UOXFile;
#include "mapclasses.h"
#include "fileio.h"
#include "scriptc.h"
#include "classes.h"
#include "im.h"
#include "cConsole.h"
#include "msgboard.h"
#include "books.h"
#include "boats.h"	// Boats
#include "worldmain.h"
#include "regions.h"
#include "cRaces.h"
#include "cWeather.hpp"
#include "townregion.h"
#include "speech.h"
#include "Dictionary.h"
#include "cAccountClass.h"
#include "funcdecl.h"
#include "hash.h"
#include "packets.h"
#include "cGuild.h"
#include "cServerDefinitions.h"


//o---------------------------------------------------------------------------o
// LINUX Definitions
//o---------------------------------------------------------------------------o
#ifndef __NT__
inline int min( int a, int b ) { if( a < b ) return a; return b; }
inline int max( int a, int b ) { if( a > b ) return a; return b; }
extern "C" {
char *strlwr(char *);
char *strupr(char *);
};
#endif


//o---------------------------------------------------------------------------o
// DEBUG/NODEBUG <-- Debug: Shows client->server messages in the server window.
// Turn it off when releasing.
//o---------------------------------------------------------------------------o

//o---------------------------------------------------------------------------o
// Public variables, structures, classes and objects
//o---------------------------------------------------------------------------o
extern UI08 GMCMDLEVEL;
extern UI08 CNSCMDLEVEL;
extern creat_st creatures[2048]; // stores the base-sound+sound flags of monsters, animals
extern std::vector< SpellInfo > spells; //:Terrin: adding variable for spell system "cache" had to make global for skills.cpp as a quick fix
extern UI32 uiCurrentTime, ErrorCount;
extern char Loaded;
extern UI16 doorbase[DOORTYPES];
extern char skillname[SKILLS+1][20];
extern UI08 talk[15];
extern char gmprefix[10];
extern char gmmiddle[5];
extern char bpitem[20];
extern char gump1[22];
extern char gump2[4];
extern char gump3[3];
extern char updscroll[11];
extern char spc[2];
extern UI32 polyduration;
#ifdef __NT__
  extern WSADATA wsaData;
  extern WORD wVersionRequested;
#endif
extern UI32 totalspawnregions;
const UI08 MAXVISRANGE = 20;

//o---------------------------------------------------------------------------o
//Time variables
//o---------------------------------------------------------------------------o
extern UI16 secondsperuominute;		// Number of seconds for a UOX minute.
extern UI32 uotickcount;			// Changed from int to UI16 (Mr. Fixit)


//o---------------------------------------------------------------------------o
// More declerations
//o---------------------------------------------------------------------------o
extern UI32 nextfieldeffecttime;
extern UI32 nextnpcaitime;
extern std::vector< TeleLocationEntry > teleLocs;


//o---------------------------------------------------------------------------o
// MSVC fails to compile UOX if this is unsigned, change it then
//o---------------------------------------------------------------------------o
#ifdef _MSVC
extern long int oldtime, newtime;
#else
extern UI32 oldtime, newtime;		// for autosaving
#endif
extern bool autosaved;
extern int heartbeat;


//o---------------------------------------------------------------------------o
// More variables
//o---------------------------------------------------------------------------o
//extern struct hostent *he;
extern SI32 err;						// Changed from int to SI32 (Mr. Fixit)
extern bool error;
extern bool keeprun;
extern fd_set conn;
extern fd_set all;
extern fd_set errsock;
extern SI32 nfds;						// Changed from int to SI32 (Mr. Fixit)
extern timeval uoxtimeout;
extern SI32 now;
extern bool secure; // Secure mode
extern char fametitle[128];
extern char skilltitle[50];
extern char prowesstitle[50];


//o---------------------------------------------------------------------------o
// The below structure is for looking up items based on serial #
// item's serial, owner's serial, char's serial, and container's serial
//o---------------------------------------------------------------------------o
extern HashTable< ITEM >			nitemsp;
extern HashTableMulti< ITEM >		nspawnsp;
extern HashTable< CHARACTER >		ncharsp;
extern HashTableMulti< CHARACTER >	ncspawnsp;

//tables for data lengths in binary save files (located in worldmain.cpp)
extern const UI08 CharSaveTable[256];
extern const UI08 ItemSaveTable[256];
extern cBaseObject *DefBase;
extern CChar *DefChar;
extern CItem *DefItem;

//o---------------------------------------------------------------------------o
// More
//o---------------------------------------------------------------------------o
extern location_st location[4000];
extern logout_st logout[1024];			// Instalog
extern cTownRegion *region[256];
extern cSpawnRegion *spawnregion[4098];	// Zippy
extern skill_st skill[SKILLS+1];
extern UI16 locationcount;				// Changed from int to UI08 (Mr. Fixit)
extern UI32 logoutcount;				// Instalog
extern std::vector< JailCell > jails;
extern UI32 charcount, itemcount;
extern SERIAL charcount2, itemcount2;
extern UI32 imem, cmem;
extern char *comm[CMAX];
extern SI32 tnum;						// Changed from int to SI32 (Mr. Fixit)
extern UI32 npcshape[5];				// Stores the coords of the bouding shape for the NPC. DOES NOT NEED TO BE AN ARRAY. Changed from int to UI32 (Mr. Fixit)
extern UI32 starttime, endtime, lclock;
extern bool overflow;
extern char idname[256];
extern char pass1[256];
extern char pass2[256];
extern SI32 executebatch;				// Changed from int to SI32 (Mr. Fixit)
extern bool showlayer;
extern SI32 ph1, ph2, ph3, ph4;			// Not used for anything (Mr. Fixit)
extern UI32 shoprestocktime;			// Changed from int to UI32 (Mr. Fixit)
extern SI32 shoprestockrate;			// Changed from int to SI32 (Mr. Fixit)
extern UI32 respawntime;
extern title_st title[ALLSKILLS+1];
extern UI32 hungerdamagetimer; // Used for hunger damage
extern char xgm;


//o---------------------------------------------------------------------------o
// Profiling
//o---------------------------------------------------------------------------o
extern UI32 networkTime;				// Changed from int to UI32 (Mr. Fixit)
extern UI32 timerTime;					// Changed from int to UI32 (Mr. Fixit)
extern UI32 autoTime;					// Changed from int to UI32 (Mr. Fixit)
extern UI32 loopTime;					// Changed from int to UI32 (Mr. Fixit)
extern UI32 networkTimeCount;			// Changed from int to UI32 (Mr. Fixit)
extern UI32 timerTimeCount;				// Changed from int to UI32 (Mr. Fixit)
extern UI32 autoTimeCount;				// Changed from int to UI32 (Mr. Fixit)
extern UI32 loopTimeCount;				// Changed from int to UI32 (Mr. Fixit)


//o---------------------------------------------------------------------------o
// Time inline functions
//o---------------------------------------------------------------------------o
#ifdef __LINUX__
	UI32 getclock( void );
	#undef CLOCKS_PER_SEC
	#define CLOCKS_PER_SEC 100
	inline void StartMilliTimer( UI32 &Seconds, UI32 &Milliseconds )
	{
		struct timeval t;
		gettimeofday( &t, NULL );
		Seconds = t.tv_sec;
		Milliseconds = t.tv_usec;
	};
	inline UI32 CheckMilliTimer( UI32 &Seconds, UI32 &Milliseconds ) { struct timeval t; gettimeofday( &t, NULL ); return( 1000 * ( t.tv_sec - Seconds ) + ( t.tv_usec - Milliseconds ) ); };
	inline void UOXSleep( int toSleep ) { usleep( toSleep * 1000 ); }
#else
	#define getclock() clock()
	inline void StartMilliTimer( UI32 &Seconds, UI32 &Milliseconds ) { struct timeb t; ftime( &t ); Seconds = t.time; Milliseconds = t.millitm; };
	inline UI32 CheckMilliTimer( UI32 &Seconds, UI32 &Milliseconds ) { struct timeb t; ftime( &t ); return( 1000 * ( t.time - Seconds ) + ( t.millitm - Milliseconds ) ); };
	inline void UOXSleep( int toSleep ) { Sleep( toSleep ); }
#endif
inline UI32 BuildTimeValue( R32 timeFromNow ) { return (UI32)( uiCurrentTime + ( (R32)(CLOCKS_PER_SEC) * timeFromNow ) );	}


//o---------------------------------------------------------------------------o
// Classes Definitions
//o---------------------------------------------------------------------------o
extern cBoat					*Boats;
extern CWorldMain				*cwmWorldState;
extern cCombat					*Combat;
extern cCommands				*Commands;
extern cGump					*Gumps;
extern cHTMLTemplates			*HTMLTemplates;
extern cItem					*Items;
extern cMapStuff				*Map;
extern cCharStuff				*Npcs;
extern cSkills					*Skills;
extern cWeight					*Weight;
extern cTargets					*Targ;
extern cNetworkStuff			*Network;
extern cMagic					*Magic;
extern cRaces					*Races;
extern cWeatherAb				*Weather;
extern cMovement				*Movement;
extern cWhoList					*WhoList;
extern cWhoList					*OffList;
extern cTEffect					*Effects;
extern cBooks					*Books;
extern PageVector				*GMQueue;
extern PageVector				*CounselorQueue;
extern CDictionaryContainer		*Dictionary;
extern cAccountClass		*Accounts;
extern Triggers					*Trigger;
extern CConsole					Console;
extern cMapRegion				*MapRegion;
extern CSpeechQueue				*SpeechSys;
extern CGuildCollection			*GuildSys;
extern cServerDefinitions		*FileLookup;
extern JailSystem				*JailSys;
#include "cThreadQueue.h"
extern CThreadQueue				messageLoop;
extern UI32 lighttime;
extern cItemHandle items;
extern cCharacterHandle chars;


//o---------------------------------------------------------------------------o
// More variable refrence
//o---------------------------------------------------------------------------o
extern int *loscache;
extern int *itemids;
//extern int lenConnAddr;					// Not used anywhere (Mr. Fixit)
extern SI32 globalRecv;						// Changed from long int to SI32 (Mr. Fixit)
extern SI32 globalSent;						// Changed from long int to SI32 (Mr. Fixit)
extern UI08 escortRegions;					// Changed from long int to UI08 (Mr. Fixit)
extern UI08 validEscortRegion[256];			// Changed from long int to UI08 (Mr. Fixit)
extern SI32 erroredLayers[MAXLAYERS];		// Changed from long int to SI32 (Mr. Fixit)
extern JSRuntime *jsRuntime;
extern JSContext *jsContext;
extern JSObject *jsGlobal;
extern JSClass global_class;
extern JSClass uox_class;
extern CEndL myendl;
extern std::vector< MurderPair > murdererTags;

#endif // __UOX3_H
