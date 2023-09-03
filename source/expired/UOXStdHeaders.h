#ifndef __StdHeaders_H__
#define __StdHeaders_H__

#include <cassert>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

// STL containers
#include <bitset>
#include <deque>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <vector>

// STL algorithms & functions
#include <algorithm>
#include <functional>
#include <limits>

// C++ Stream stuff
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

// We use exceptions in a lot of places
#include <stdexcept>

#include <sys/stat.h>
#include <sys/types.h>

#if defined(_WIN32)
#include <sys/timeb.h>

#else

#include <errno.h> // for errno
#include <sys/signal.h>
#include <sys/time.h>
#include <unistd.h>

#endif

#include "spidermonkey.h"

#endif
