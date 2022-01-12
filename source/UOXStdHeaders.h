#ifndef __StdHeaders_H__
#define __StdHeaders_H__


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




#   include <sys/types.h>
#   include <sys/stat.h>


#if PLATFORM == WINDOWS

#undef min
#undef max

#define WIN32_LEAN_AND_MEAN
#include <sys/timeb.h>
#endif

#if PLATFORM != WINDOWS


#   include <unistd.h>
#	include <sys/time.h>
#	include <sys/signal.h>
#	include <errno.h>		// for errno


#endif


#include "jsapi.h"

#endif
