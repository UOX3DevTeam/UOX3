/*
-----------------------------------------------------------------------------
This file is an adaptation of the OgrePlatform.h file but customised to
suit our own purposes
-----------------------------------------------------------------------------
*/
#ifndef __Platform_H_
#define __Platform_H_

#include "Config.h"

#define __REENTRANT

/* Initial platform/compiler-related stuff to set.
*/
#define PLATFORM_WIN32 1
#define PLATFORM_LINUX 2
#define PLATFORM_APPLE 3

#define COMPILER_MSVC 1
#define COMPILER_GNUC 2
#define COMPILER_BORL 3

#define ENDIAN_LITTLE 1
#define ENDIAN_BIG 2

/* Finds the compiler type and version.
*/
#if defined( _MSC_VER )
#   define UOX_COMPILER COMPILER_MSVC
#   define UOX_COMP_VER _MSC_VER

#elif defined( __GNUC__ )
#   define UOX_COMPILER COMPILER_GNUC
#   define UOX_COMP_VER __VERSION__
#	define FALSE 0L
#	define TRUE  1L 

#elif defined( __BORLANDC__ )
#   define UOX_COMPILER COMPILER_BORL
#   define UOX_COMP_VER __BCPLUSPLUS__

#else
#   pragma error "No known compiler. Abort! Abort!"

#endif

/* See if we can use __forceinline or if we need to use __inline instead */
#if UOX_COMPILER == COMPILER_MSVC 
#   if UOX_COMP_VER >= 1200
#       define FORCEINLINE __forceinline
#   endif
#else
#   define FORCEINLINE __inline
#endif

/* Finds the current platform */

#if defined( __WIN32__ ) || defined( _WIN32 )
#   define UOX_PLATFORM PLATFORM_WIN32

#elif defined( __APPLE_CC__)
#   define UOX_PLATFORM PLATFORM_APPLE

#else
#   define UOX_PLATFORM PLATFORM_LINUX
#endif

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

//----------------------------------------------------------------------------
// Windows Settings
#if UOX_PLATFORM == PLATFORM_WIN32

// Win32 compilers use _DEBUG for specifying debug builds.
#   ifdef _DEBUG
#       define UOX_DEBUG_MODE 1
#   endif

	#define XP_WIN			// JS API Requires we define OS we compile with
	#define XP_PC
	#define OS_STR "Win32"

// A quick define to overcome different names for the same function
#	if UOX_COMPILER != COMPILER_GNUC
#		define snprintf _snprintf
#		define vsnprintf _vsnprintf
#	endif

#endif
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Linux/Apple Settings
#if UOX_PLATFORM == PLATFORM_LINUX || UOX_PLATFORM == PLATFORM_APPLE

// A quick define to overcome different names for the same function
#   define stricmp strcasecmp

// Unlike the Win32 compilers, Linux compilers seem to use DEBUG for when
// specifying a debug build.
#   ifdef DEBUG
#       define UOX_DEBUG_MODE 1
#   endif

	#define XP_UNIX			// JS API Requires we define OS we compile with
	#define OS_STR "Linux"

	#define closesocket( s ) close( s )
	#define ioctlsocket( s, b, c ) ioctl( s, b, c )

	#include "uoxlinux.h"	// linux wrappers for windows-specific api calls

#endif

//For apple, we always have a custom config.h file
#if UOX_PLATFORM == PLATFORM_APPLE
#    include "config.h"

#endif

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Endian Settings
// check for BIG_ENDIAN config flag, set UOX_ENDIAN correctly
#ifdef CONFIG_BIG_ENDIAN
#    define UOX_ENDIAN ENDIAN_BIG
#else
#    define UOX_ENDIAN ENDIAN_LITTLE
#endif

#endif
