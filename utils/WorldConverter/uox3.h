#ifndef __UOX3_H
#define __UOX3_H

#define VER " 0.00.01"

#define PRODUCT "Ultima Offline eXperiment 3 World Converter"
#define TIMEZONE "GMT+10"
#define NAME "The Official DevTeam"
#define EMAIL "http://www.uox3.net/"
#define PROGRAMMERS "UOX3 DevTeam"

#pragma warning(disable: 4786) //Gets rid of BAD stl warnings
#pragma warning(disable: 4503)
#pragma warning(disable: 4786)
#pragma warning(disable: 4097)
#pragma warning(disable: 4100)
#pragma warning(disable: 4663)
#pragma warning(disable: 4201)	// nameless struct/union

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

#include "socket_interface.h"

#ifdef __NT__
	#ifdef _BORLAND_
		#include <condefs.h>
	#endif
	#define WIN32_LEAN_AND_MEAN
	//#include <windows.h>
	//#include <winsock2.h>
	#include <process.h>
	#include <cstddef>
	#include <winbase.h>
	#include <limits>
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
#include "classes.h"
#include "funcdecl.h"
#include "hash.h"
#include "regions.h"

// LINUX Definitions
#ifndef __NT__
inline int min(int a, int b) { if (a < b) return a; return b; }
inline int max(int a, int b) { if (a > b) return a; return b; }
extern "C" {
char *strlwr(char *);
char *strupr(char *);
};
#endif

extern SI32 charcount;
extern SERIAL charcount2;
extern SI32 itemcount;
extern SERIAL itemcount2;
extern SI32 imem;
extern SI32 cmem;

extern cMapRegion		*MapRegion;
#define MIN(arga, argb) ( (arga) < (argb) ? (arga) : ( argb ) )

extern cItemHandle items;
extern cCharacterHandle chars;

extern long erroredLayers[MAXLAYERS];

extern HashTableItem nitemsp;
extern HashTableChar ncharsp;

extern char *cline;
extern char *comm[CMAX];
extern SI32 charsWritten, itemsWritten;

extern FILE *errorLog;

#endif // __UOX3_H
