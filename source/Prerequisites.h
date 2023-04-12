//o------------------------------------------------------------------------------------------------o
// This file is an adaptation of the file OgrePrerequisites.h from OGRE
// and customised to our own needs
//o------------------------------------------------------------------------------------------------o
#ifndef __Prerequisites_H__
#define __Prerequisites_H__

// Platform-specific stuff
#include "ConfigOS.h"

//o------------------------------------------------------------------------------------------------o
// The Timestamp one I understand, but the NOACTCOPY, if we have/dont test
// with the setting to 1, we should delete it (and corresponding code)
//o------------------------------------------------------------------------------------------------o
//o------------------------------------------------------------------------------------------------o
// Define this if you don't want the accounts block to have a copy constructor
//	or assignment operator
#define _NOACTCOPY_ 0
// Define this if we want to have time stamps associated with our packet logs
#define P_TIMESTAMP 1

//o------------------------------------------------------------------------------------------------o
#if defined(_WIN32)
// disable: "conversion from 'double' to 'float', possible loss of data
#pragma warning (disable : 4244)
// disable: "truncation from 'double' to 'float'
#pragma warning (disable : 4305)
#endif
//o------------------------------------------------------------------------------------------------o

#if defined(DEBUG) || defined(_DEBUG)
#define UOX_DEBUG_MODE 1
#endif   //DEBUG

// define the Char type as either char or wchar_t
#if UOX_WCHAR_T_STRINGS == 1
#   define UOXChar wchar_t
#	define _TO_CHAR( x ) L##x
#else
#   define UOXChar char
#	define _TO_CHAR( x ) x
#endif

/// Useful macros
#define UOX_DELETE( p )       { if( p ) { delete ( p );     ( p ) = nullptr; }}
#define UOX_DELETE_ARRAY( p ) { if( p ) { delete[] ( p );   ( p ) = nullptr; }}
// For generating compiler warnings - should work on any compiler
// As a side note, if you start your message with 'Warning: ', the MSVC
// IDE actually does catch a warning :)
#define _QUOTE( x ) # x
#define QUOTE( x ) _QUOTE( x )
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

#endif // __Prerequisites_H__


