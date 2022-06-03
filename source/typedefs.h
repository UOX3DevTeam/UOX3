#ifndef __UOXTYPES_H
#define __UOXTYPES_H
//================================================================
//
//		We want this to be as "independent" of other headers as possible.  So we dont
//		use any symbols defined by ConfOS, etc.
//
#include <unordered_map>
#include <cstdint>
#include <functional>

#ifdef __NEED_VALIST__
using va_list = void* ;
#endif

#if !defined(_WIN32)
constexpr auto MAX_PATH	= 268 ;
#endif

#if defined( _DEBUG ) || defined(DEBUG)
#define VALIDATESOCKET( s ) if( s == nullptr ) \
{ \
Console.print( oldstrutil::format("Socket failure at %s", __FILE__LINE__) );	\
return;	\
}
#else
#define VALIDATESOCKET( s ) if( s == nullptr ) \
return;
#endif

//  We define these for now. But we all ready have a "standard" way to define data with fixed sizes.
//  in <cstdint>. From a long term maintenance perspective, and training for others, it would make
//  sense to use the "standard" way provided by the language distribution.  But for now, these are here
using R32 = float ;
using R64 = double ;
using UI08 = std::uint8_t ; // 0 to 255
using SI08 = std::int8_t ; // -128 to 127
using SI16 = std::int16_t ; // -32768 to 32767
using UI16 = std::uint16_t; // 0 to 65535
using SI32 = std::int32_t ; // -2147483648 to 2147483647
using UI32 = std::uint32_t ; // 0 to 4294967295
using SI64 = std::int64_t ; // -9223372036854775808 to 9223372036854775807
using UI64 = std::uint64_t ; // 0 to 18446744073709551615

// Some of our unique data types, makes it consistent on data sizes (although a lot to be said to make everything
// int, and then just in the packet/data file i/o convert to the correct size.  It would be faster in terms of execution.
using GENDER	= std::uint8_t ;
using LIGHTLEVEL	= std::uint8_t ;
using COLDLEVEL	= std::uint8_t ;
using HEATLEVEL	= std::uint8_t ;
using SECONDS	= std::uint8_t ;
using ARMORCLASS	= std::uint8_t ;
using RACEREL	= std::uint8_t ;
using RANGE		= std::int8_t ;
using RACEID	= std::uint16_t ;
using COLOUR	= std::uint16_t ;
using SKILLVAL	= std::uint16_t ;
using weathID	= std::uint16_t ;
using GUILDID	= std::int16_t ;
using TIMERVAL	= std::uint32_t ;
using SERIAL	= std::uint32_t ;

#if defined(_WIN32)
using UOXSOCKET	= std::uint32_t ;
#else
using UOXSOCKET	= std::int32_t ;
#endif

constexpr auto INVALIDSERIAL	= std::uint32_t(0xFFFFFFFF) ;
constexpr auto BASEITEMSERIAL = std::uint32_t(0x40000000) ;
constexpr auto INVALIDID	= std::uint16_t(0xFFFF);
constexpr auto INVALIDCOLOUR	= std::uint16_t(0xFFFF);


// Two things about this: 	1. Creates a dependency on CSocket knowledge which we dont want in this header
//					2. Should use std::function, which we need to investigate on why it doesnt
typedef void (TargetFunc)( CSocket *s );
//using TargetFunc  = std::function<void>(CSocket*) ;

//	 	-	End

// Max values
constexpr auto MAX_NAME = std::uint8_t(128); // Several areas where we pass a character name will be restricted by packet size to 30 characters.
								// Higher MAX_NAME values do, however, work for items - and are in some cases required (magic item names, for instance). Seems to still work for regular-length names if I increase it, but we might consider splitting this into character/item-specific somehow?
constexpr auto MAX_TITLE	= std::uint8_t(60);
constexpr auto MAX_VISRANGE	= std::uint8_t(18);
constexpr auto MAXPOSTS		= std::uint8_t(128); // Maximum number of posts on a messageboard
constexpr auto MAX_STACK	= std::uint16_t(0xFFFF);
constexpr auto MAXBUFFER	= std::uint16_t(4096); // Buffer Size (For socket operations)
constexpr auto ILLEGAL_Z	= std::int8_t(-128) ;

// Offsets for Gump menu's (Relates to menus.dfn)
constexpr auto ITEMMENUOFFSET		= 256 ;
constexpr auto TRACKINGMENUOFFSET	= 4096 ;
constexpr auto POLYMORPHMENUOFFSET	= 8192 ;
constexpr auto JSGUMPMENUOFFSET	= 16384 ;

// Directions
constexpr auto NORTH		= std::uint8_t(0x00) ;
constexpr auto NORTHEAST	= std::uint8_t(0x01) ;
constexpr auto EAST		= std::uint8_t(0x02) ;
constexpr auto SOUTHEAST	= std::uint8_t(0x03) ;
constexpr auto SOUTH		= std::uint8_t(0x04) ;
constexpr auto SOUTHWEST	= std::uint8_t(0x05) ;
constexpr auto WEST		= std::uint8_t(0x06) ;
constexpr auto NORTHWEST	= std::uint8_t(0x07) ;
constexpr auto UNKNOWNDIR	= std::uint8_t(0xFF) ;


// Line Of Sight
constexpr auto ITEM_TYPE_CHOICES	= 6 ;
constexpr auto TREES_BUSHES		= 1 ; // Trees and other large vegetaion in the way
constexpr auto WALLS_CHIMNEYS		= 2 ; // Walls, chimineys, ovens, etc... in the way
constexpr auto DOORS			= 4 ; // Doors in the way
constexpr auto ROOFING_SLANTED	= 8 ; // So can't tele onto slanted roofs, basically
constexpr auto FLOORS_FLAT_ROOFING	= 16 ; // For attacking between floors
constexpr auto LAVA_WATER		= 32 ; // Don't know what all to use this for yet

#endif

