
#ifndef __ConfigOS_H_
#define __ConfigOS_H_

// Define this if you don't want the accounts block to have a copy constructor
//	or assignment operator

#define _NOACTCOPY_ 0

// Define this if we want to have time stamps associated with our packet logs

#define P_TIMESTAMP 1




// For generating compiler warnings - should work on any compiler
// As a side note, if you start your message with 'Warning: ', the MSVC
// IDE actually does catch a warning :)
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

// Initial platform/compiler-related stuff to set.

#define WINDOWS		1
#define LINUX 		2
#define MACOS		3


// Finds the compiler type and version.
#if defined( _MSC_VER )

#define XP_WIN            // JS API Requires we define OS we compile with
#define XP_PC

#ifdef _WIN32 // Includes both 32 bit and 64 bit

#ifdef _WIN64
#define OS_STR "Win64"
#else 
#define OS_STR "Win32"
#endif   //_WIN64

#endif   //_WIN32

#define PLATFORM WINDOWS

// A quick define to overcome different names for the same function
#define STDC99
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#ifdef _DEBUG
#define UOX_DEBUG_MODE 1
#define _ITERATOR_DEBUG_LEVEL 1
#define _HAS_ITERATOR_DEBUGGING 0                    // Iterator debugging should only be enabled in debug, and WILL cause crashes if iterators are handled improperly.
#define _SECURE_SCL 1

#else

#define _HAS_ITERATOR_DEBUGGING 0                    // Iterator debugging should only be enabled in debug, and WILL cause crashes if iterators are handled improperly.
#define _SECURE_SCL 0

#endif		// endif to _DEBUG

#else       // A unix type system

#define XP_UNIX            // JS API Requires we define OS we compile with

#ifdef __linux__

#define PLATFORM LINUX
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

#if INTPTR_MAX == INT64_MAX
#define OS_STR "Linux64"
#elif INTPTR_MAX == INT32_MAX
#define OS_STR "Linux32"
#endif  // INTPTR_MAX

#elif defined(__APPLE__)

#define PLATFORM MACOS
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


#define ioctlsocket( s, b, c ) ioctl( s, b, c )
#define closesocket( s ) close( s )

#define FALSE 0L
#define TRUE  1L

// Unlike the Win32 compilers, Linux/macOS compilers use DEBUG for when
// specifying a debug build.
#ifdef DEBUG
#define UOX_DEBUG_MODE 1
#endif   //DEBUG


#endif    // End to the compiler type/version




#endif //__ConfigOS_H_
