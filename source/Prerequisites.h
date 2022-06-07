/*-------------------------------------------------------------------------
 This file is an adaptation of the file OgrePrerequisites.h from OGRE
 and customised to our own needs
 -------------------------------------------------------------------------*/
#ifndef __Prerequisites_H__
#define __Prerequisites_H__

// Platform-specific stuff
#include "ConfigOS.h"

#if PLATFORM == WINDOWS

// disable: "conversion from 'double' to 'float', possible loss of data
#pragma warning (disable : 4244)

// disable: "truncation from 'double' to 'float'
#pragma warning (disable : 4305)


#endif


#if defined(DEBUG) || defined(_DEBUG)
#define UOX_DEBUG_MODE 1
#endif   //DEBUG


/* Include all the standard header *after* all the configuration
 settings have been made.
 */
#include "UOXStdHeaders.h"

// define the real number values to be used
// default to use 'float' unless precompiler option set

// Define UOX version
#define UOX_VERSION_MAJOR 0
#define UOX_VERSION_MINOR 14
#define UOX_VERSION_PATCH 0

// define the Char type as either char or wchar_t
#if UOX_WCHAR_T_STRINGS == 1
#   define UOXChar wchar_t
#	define _TO_CHAR( x ) L##x
#else
#   define UOXChar char
#	define _TO_CHAR( x ) x
#endif

/// Useful macros
#define UOX_DELETE(p)       { if(p) { delete (p);     (p)=nullptr; } }
#define UOX_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=nullptr; } }
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

// Define this if you don't want the accounts block to have a copy constructor
//	or assignment operator

#define _NOACTCOPY_ 0

// Define this if we want to have time stamps associated with our packet logs

#define P_TIMESTAMP 1




// Pre-declare classes
// Allows use of pointers in header files without including individual .h
// so decreases dependencies between files
class CBaseTile;
class cAccountClass;
class CBaseObject;
class cBooks;
class CBoatObj;
class CChar;
class cCharStuff;
class cCommands;
class CConsole;
class cDice;
class CDictionary;
class CDictionaryContainer;
class cDirectoryListing;
class CEndL;
class cEffects;
class CEnvoke;
class CGuild;
class CGuildCollection;
class CGump;
class cHTMLTemplate;
class cHTMLTemplates;
class cItem;
class CItem;
class CJSMapping;
class CJSMappingSection;
class CLand;
class cMagic;
class CMagicMove;
class CMagicStat;
class CMapHandler;
class CMapRegion;
class CMulHandler;
class cMovement;
class CMultiObj;
class cNetworkStuff;
class CPUOXBuffer;
class CPInputBuffer;
class cRaces;
class CRace;
class cScript;
class CServerData;
class CServerDefinitions;
class CServerProfile;
class cSkillClass;
class cSkills;
class CSocket;
class CSpawnItem;
class CSpawnRegion;
class CSpeechEntry;
class CSpeechQueue;
class CTEffect;
class CThreadQueue;
class CTile;
class CTileUni;
class CTownRegion;
class CVersionClass;
class CWeather;
class cWeatherAb;
class CWeight;
class cWhoList;
class CWorldMain;
class GumpDisplay;
class HelpRequest;
class JailCell;
class JailSystem;
class ObjectFactory;
class Script;
class ScriptSection;
class SpellInfo;
class UOXFile;

/** In order to avoid finger-aches :)
 */
#include "enums.h"
#include "typedefs.h"

#endif // __Prerequisites_H__


