
#ifndef __ConfigOS_H_
#define __ConfigOS_H_


// Initial platform/compiler-related stuff to set.

#define WINDOWS		1
#define LINUX 		2
#define MACOS		3


// Finds the compiler type and version.
#if defined( _MSC_VER )

#define XP_WIN            // JS API Requires we define OS we compile with
#define XP_PC

#ifdef _WIN32 // Includes both 32 bit and 64 bit

#undef min
#undef max

#define WIN32_LEAN_AND_MEAN

#ifdef _WIN64
#define OS_STR "Win64"
#else 
#define OS_STR "Win32"
#endif   //_WIN64

#endif   //_WIN32

#define PLATFORM WINDOWS

// A quick define to overcome different names for the same function
//#define STDC99
//#define _CRT_SECURE_NO_DEPRECATE
//#define _CRT_SECURE_NO_WARNINGS

#else       // A unix type system

#define XP_UNIX            // JS API Requires we define OS we compile with

#ifdef __linux__

#define PLATFORM LINUX

#if INTPTR_MAX == INT64_MAX
#define OS_STR "Linux64"
#elif INTPTR_MAX == INT32_MAX
#define OS_STR "Linux32"
#endif  // INTPTR_MAX

#elif defined(__APPLE__)

#define PLATFORM MACOS

#if INTPTR_MAX == INT64_MAX
#define OS_STR "MacOS64"
#elif INTPTR_MAX == INT32_MAX
#define OS_STR "MacOS32"
#endif   //INPTR_MAX

#else

#define OS_STR "Unknown"
#endif  //__linux__




#endif    // End to the compiler type/version




#endif //__ConfigOS_H_
