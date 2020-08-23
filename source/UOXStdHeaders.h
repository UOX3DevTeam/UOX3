#ifndef __StdHeaders_H__
#define __StdHeaders_H__

#ifdef __BORLANDC__
    #define __STD_ALGORITHM
#endif

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cstdarg>
#include <cmath>

// STL containers
#include <vector>
#include <map>
#include <string>
#include <set>
#include <list>
#include <deque>
#include <queue>
#include <bitset>

// STL algorithms & functions
#include <algorithm>
#include <functional>
#include <limits>

// C++ Stream stuff
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

// We use exceptions in a lot of places
#include <stdexcept>

#ifdef __BORLANDC__
namespace UOX
{
    using namespace std;
}
#endif

extern "C" {

#   include <sys/types.h>
#   include <sys/stat.h>

}

#if UOX_PLATFORM == PLATFORM_WIN32

#undef min
#undef max

	#define WIN32_LEAN_AND_MEAN
	#include <sys/timeb.h>
	typedef long int32;

#endif

#if UOX_PLATFORM == PLATFORM_LINUX
extern "C" {

#   include <unistd.h>
#   include <dlfcn.h>
#	include <ctype.h>
#	include <sys/time.h>
#	include <sys/signal.h>
#	include <errno.h>		// for errno

}
#endif

#if UOX_PLATFORM == PLATFORM_APPLE
extern "C" {
#   include <unistd.h>
#   include <sys/param.h>
#   include <CoreFoundation/CoreFoundation.h>
#   include <sys/time.h>
}
#endif

#include "socket_interface.h"
#include "jsapi.h"

#endif
