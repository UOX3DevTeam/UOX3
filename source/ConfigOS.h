
#ifndef __ConfigOS_H_
#define __ConfigOS_H_


// Initial platform/compiler-related stuff to set.

//#define WINDOWS		1
//#define LINUX 		2
//#define MACOS		3

// Finds the compiler type and version.
#if defined( _MSC_VER )
#define XP_WIN            // JS API Requires we define OS we compile with
#define XP_PC

//o------------------------------------------------------------------------------------------------o
// These should be defined on the compilier line, in case windows.h gets defined ahead (resources.h?)
#define NOMINMAX		// Dont want min/max macros, use <algorithm>
#define WIN32_LEAN_AND_MEAN  // We NEVER want Winsock1, and the extra windows stuff
//o------------------------------------------------------------------------------------------------o
//o------------------------------------------------------------------------------------------------o
// Ok, windows sets _ITERATOR_DEBUG_LEVEL based on debug/release.
// Check: https://docs.microsoft.com/en-us/cpp/standard-library/iterator-debug-level?view=msvc-170
// In theory, we shouldn't need these, if we have issues, we want to fix the iterator problem
// If we continue to need them, then ideally, they should be on the compile line in the project
// Got rid of _HAS_ITERATOR_DEBUGGING/_SECURE_SCL as _ITERATOR_DEBUG_LEVEL supersedes them.
#ifdef _DEBUG
#define _ITERATOR_DEBUG_LEVEL 2   // THis is the default, why do we need this?
#else
#define _HAS_ITERATOR_DEBUGGING 0  // This is the default for non debug, why do we need?
#endif		// endif to _DEBUG
//o------------------------------------------------------------------------------------------------o

#ifdef _WIN32 // Includes both 32 bit and 64 bit

#ifdef _WIN64
#define OS_STR "Win64"
#else 
#define OS_STR "Win32"
#endif   //_WIN64

#endif   //_WIN32

//#define PLATFORM WINDOWS

#else       // A unix type system

#define XP_UNIX            // JS API Requires we define OS we compile with

#define ioctlsocket( s, b, c ) ioctl( s, b, c )
#define closesocket( s ) close( s )

#ifdef __linux__

//#define PLATFORM LINUX
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

#if INTPTR_MAX == INT64_MAX
#define OS_STR "Linux64"
#elif INTPTR_MAX == INT32_MAX
#define OS_STR "Linux32"
#endif  // INTPTR_MAX

#elif defined(__APPLE__)

//#define PLATFORM MACOS
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

#if INTPTR_MAX == INT64_MAX
#define OS_STR "MacOS64"
#elif INTPTR_MAX == INT32_MAX
#define OS_STR "MacOS32"
#endif   //INPTR_MAX

#else

#define OS_STR "Unknown"
#endif  //__linux__

#define FALSE 0L
#define TRUE  1L
#endif    // End to the compiler type/version

#endif //__ConfigOS_H_
