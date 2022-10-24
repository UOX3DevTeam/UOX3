#ifndef __UOXTYPES_H
#define __UOXTYPES_H

//o------------------------------------------------------------------------------------------------o
// General note on constexpr and how it varies with const
//	constexpr is compile time evaluation(const is evaluated at run time).
//	So it can replace defines for static constants.  constexpr can only
//	be applied to intrinsics (so can not be applied to say, std::string values).
//
//o------------------------------------------------------------------------------------------------o
//o------------------------------------------------------------------------------------------------o
//	A note on "using"
//	Using-declarations can be used to introduce namespace members into other namespaces and block
//	scopes, or to introduce base class members into derived class definitions, or to introduce
//	enumerators into namespaces, block, and class scopes (since C++20).
//
//o------------------------------------------------------------------------------------------------o

#include <unordered_map>
#include <cstdint>
#if defined( _DEBUG )
#define VALIDATESOCKET( s ) if( s == nullptr ) \
{ \
Console.Print( oldstrutil::format( "Socket failure at %s", __FILE__LINE__) );	\
return;	\
}
#else
#define VALIDATESOCKET( s ) if( s == nullptr ) \
return;
#endif

#ifdef __NEED_VALIST__
typedef void *  va_list;
#endif

//o------------------------------------------------------------------------------------------------o
// Setting up types used
//o------------------------------------------------------------------------------------------------o

using R32 = float;
using R64 = double;
using UI08 = std::uint8_t;	// 0 to 255
using SI08 = std::int8_t;	// -128 to 127
using UI16 = std::uint16_t;	// 0 to 65535
using SI16 = std::int16_t;	// -32768 to 32767
using UI32 = std::uint32_t;	// 0 to 4294967295
using SI32 = std::int32_t;	// -2147483648 to 2147483647
using UI64 = std::uint64_t;	// 0 to 18446744073709551615
using SI64 = std::int64_t;	// -9223372036854775808 to 9223372036854775807

using SERIAL 		= std::uint32_t;
using GENDER 		= std::uint8_t;
using LIGHTLEVEL 	= std::uint8_t;
using COLDLEVEL		= std::uint8_t;
using HEATLEVEL		= std::uint8_t;
using SECONDS		= std::uint8_t;
using ARMORCLASS	= std::uint8_t;
using RACEREL		= std::int8_t;
using RANGE			= std::int8_t;
using RACEID		= std::uint16_t;
using COLOUR		= std::uint16_t;
using SKILLVAL		= std::uint16_t;
using WEATHID		= std::uint16_t;
using GUILDID		= std::int16_t;
using TIMERVAL		= std::uint32_t;

#if defined(_WIN32)
using UOXSOCKET = 	std::uint32_t;
#else
using UOXSOCKET = 	std::int32_t;
#endif

//==================================================================================================
// Setting up constants
//==================================================================================================
// Offsets for Gump menu's (Relates to menus.dfn)
constexpr auto ITEMMENUOFFSET		= 256;
constexpr auto TRACKINGMENUOFFSET	= 4096;
constexpr auto POLYMORPHMENUOFFSET	= 8192;
constexpr auto JSGUMPMENUOFFSET		= 16384;

constexpr auto INVALIDSERIAL	= SERIAL( 0xFFFFFFFF );
constexpr auto INVALIDID		= std::uint16_t( 0xFFFF );
constexpr auto INVALIDCOLOUR	= std::uint16_t( 0xFFFF );
constexpr auto BASEITEMSERIAL	= SERIAL( 0x40000000 );
#if PLATFORM != WINDOWS
constexpr auto  MAX_PATH = 268;
#endif

constexpr auto MAX_NAME 	= std::uint8_t( 128 );	// Several areas where we pass a character name will be restricted by packet size to 30 characters.
													// Higher MAX_NAME values do, however, work for items - and are in some cases required (magic item names, for instance). 
													// Seems to still work for regular-length names if I increase it, but we might consider splitting this into character/item-specific somehow?
constexpr auto MAX_ORIGIN 	= std::uint8_t( 128 );  // To store expansion origin of item
constexpr auto MAX_TITLE	= std::uint8_t( 60 );
constexpr auto MAX_STACK	= std::uint16_t( 0xFFFF );
constexpr auto MAX_VISRANGE	= std::uint8_t( 18 );
constexpr auto MAXBUFFER	= std::uint16_t( 4096 );	// Buffer Size (For socket operations)
constexpr auto MAXPOSTS		= std::uint8_t( 128 );		// Maximum number of posts on a messageboard

constexpr auto ILLEGAL_Z	= std::int8_t( -128 );



constexpr auto NORTH		= std::uint8_t( 0x00 );
constexpr auto NORTHEAST	= std::uint8_t( 0x01 );
constexpr auto EAST			= std::uint8_t( 0x02 );
constexpr auto SOUTHEAST	= std::uint8_t( 0x03 );
constexpr auto SOUTH		= std::uint8_t( 0x04 );
constexpr auto SOUTHWEST	= std::uint8_t( 0x05 );
constexpr auto WEST			= std::uint8_t( 0x06 );
constexpr auto NORTHWEST	= std::uint8_t( 0x07 );
constexpr auto UNKNOWNDIR	= std::uint8_t( 0xFF );

// Line Of Sight
constexpr auto ITEM_TYPE_CHOICES 	= 6;
constexpr auto TREES_BUSHES			= std::uint8_t( 1 );	// Trees and other large vegetaion in the way
constexpr auto WALLS_CHIMNEYS		= std::uint8_t( 2 );	// Walls, chimineys, ovens, etc... in the way
constexpr auto DOORS				= std::uint8_t( 4 );	// Doors in the way
constexpr auto ROOFING_SLANTED		= std::uint8_t( 8 );	// So can't tele onto slanted roofs, basically
constexpr auto FLOORS_FLAT_ROOFING	= std::uint8_t( 16 );	// For attacking between floors
constexpr auto LAVA_WATER			= std::uint8_t( 32 );	// Don't know what all to use this for yet

//==================================================================================================
// types we will get rid of - keeping for now
//==================================================================================================

typedef void ( TargetFunc )( CSocket *s );

#endif

