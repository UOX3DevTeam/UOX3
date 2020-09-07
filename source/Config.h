
#ifndef __Config_H_
#define __Config_H_

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

#define PLATFORM_WIN32 1
#define PLATFORM_LINUX 2

#define COMPILER_MSVC 1
#define COMPILER_GNUC 2

#define ENDIAN_LITTLE 1
#define ENDIAN_BIG 2

// Finds the compiler type and version.
#if defined( _MSC_VER )

#define XP_WIN            // JS API Requires we define OS we compile with
#define XP_PC
#define OS_STR "Win32"

#define UOX_COMPILER COMPILER_MSVC
#define UOX_COMP_VER _MSC_VER
#define UOX_PLATFORM PLATFORM_WIN32

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
#endif
#else       // A unix type system

#define XP_UNIX            // JS API Requires we define OS we compile with
#define OS_STR "Linux"

#define UOX_COMPILER COMPILER_GNUC
#define UOX_COMP_VER __VERSION__
#define UOX_PLATFORM PLATFORM_LINUX

#define ioctlsocket( s, b, c ) ioctl( s, b, c )
#define closesocket( s ) close( s )

#define FALSE 0L
#define TRUE  1L

// Unlike the Win32 compilers, Linux/macOS compilers use DEBUG for when
// specifying a debug build.
#ifdef DEBUG
#define UOX_DEBUG_MODE 1
#endif


#endif    // End to the compiler type/version




#endif
