#ifndef __UOXTYPES_H
#define __UOXTYPES_H

// o------------------------------------------------------------------------------------------------o
//  General note on constexpr and how it varies with const
//	constexpr is compile time evaluation(const is evaluated at run time).
//	So it can replace defines for static constants.  constexpr can only
//	be applied to intrinsics (so can not be applied to say, std::string values).
//
// o------------------------------------------------------------------------------------------------o
// o------------------------------------------------------------------------------------------------o
//	A note on "using"
//	Using-declarations can be used to introduce namespace members into other namespaces and
// block 	scopes, or to introduce base class members into derived class definitions, or to
// introduce 	enumerators into namespaces, block, and class scopes (since C++20).
//
// o------------------------------------------------------------------------------------------------o

#include "enums.h"
#include "useful.h"
#include <cstdint>
class CSocket;

#if defined(_DEBUG)
#define VALIDATESOCKET(s)                                                                          \
    if (s == nullptr) {                                                                            \
        Console::shared().Print(util::format("Socket failure at %s", __FILE__LINE__));             \
        return;                                                                                    \
    }
#define VALIDATESOCKET_WITH_RETURN(s)                                                              \
    if (s == nullptr) {                                                                            \
        Console::shared().Print(util::format("Socket failure at %s", __FILE__LINE__));             \
        return false;                                                                              \
    }
#else
#define VALIDATESOCKET(s)                                                                          \
    if (s == nullptr)                                                                              \
        return;
#define VALIDATESOCKET_WITH_RETURN(s)                                                              \
    if (s == nullptr)                                                                              \
        return false;
#endif

#ifdef __NEED_VALIST__
typedef void *va_list;
#endif

#if !defined(_WIN32)
#define ioctlsocket(s, b, c) ioctl(s, b, c)
#define closesocket(s) close(s)
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif
// o------------------------------------------------------------------------------------------------o
//  Setting up types used
// o------------------------------------------------------------------------------------------------o

using R32 = float;
using R64 = double;

using SERIAL = std::uint32_t;
using gender_t = std::uint8_t;
using LIGHTLEVEL = std::uint8_t;
using COLDLEVEL = std::uint8_t;
using HEATLEVEL = std::uint8_t;
using SECONDS = std::uint8_t;
using ARMORCLASS = std::uint8_t;
using RACEREL = std::int8_t;
using RANGE = std::int8_t;
using RACEID = std::uint16_t;
using colour_t = std::uint16_t;
using SKILLVAL = std::uint16_t;
using WEATHID = std::uint16_t;
using guildid_t = std::int16_t;
using TIMERVAL = std::uint32_t;

#if defined(_WIN32)
using UOXSOCKET = std::uint32_t;
#else
using UOXSOCKET = std::int32_t;
#endif

//==================================================================================================
// Setting up constants
//==================================================================================================
// Offsets for Gump menu's (Relates to menus.dfn)
constexpr auto ITEMMENUOFFSET = 256;
constexpr auto TRACKINGMENUOFFSET = 4096;
constexpr auto POLYMORPHMENUOFFSET = 8192;
constexpr auto JSGUMPMENUOFFSET = 16384;

constexpr auto INVALIDSERIAL = SERIAL(0xFFFFFFFF);
constexpr auto INVALIDID = std::uint16_t(0xFFFF);
constexpr auto INVALIDCOLOUR = std::uint16_t(0xFFFF);
constexpr auto BASEITEMSERIAL = SERIAL(0x40000000);
#if !defined(_WIN32)
constexpr auto MAX_PATH = 268;
#endif

constexpr auto MAX_NAME =
    std::uint8_t(128); // Several areas where we pass a character name will be restricted by packet
                       // size to 30 characters. Higher MAX_NAME values do, however, work for items
                       // - and are in some cases required (magic item names, for instance). Seems
                       // to still work for regular-length names if I increase it, but we might
                       // consider splitting this into character/item-specific somehow?
constexpr auto MAX_ORIGIN = std::uint8_t(128); // To store expansion origin of item
constexpr auto MAX_TITLE = std::uint8_t(60);
constexpr auto MAX_STACK = std::uint16_t(0xFFFF);
constexpr auto MAX_VISRANGE = std::uint8_t(18);
constexpr auto MAXBUFFER = std::uint16_t(4096); // Buffer Size (For socket operations)
constexpr auto MAXPOSTS = std::uint8_t(128);    // Maximum number of posts on a messageboard

constexpr auto ILLEGAL_Z = std::int8_t(-128);

constexpr auto NORTH = std::uint8_t(0x00);
constexpr auto NORTHEAST = std::uint8_t(0x01);
constexpr auto EAST = std::uint8_t(0x02);
constexpr auto SOUTHEAST = std::uint8_t(0x03);
constexpr auto SOUTH = std::uint8_t(0x04);
constexpr auto SOUTHWEST = std::uint8_t(0x05);
constexpr auto WEST = std::uint8_t(0x06);
constexpr auto NORTHWEST = std::uint8_t(0x07);
constexpr auto UNKNOWNDIR = std::uint8_t(0xFF);

// Line Of Sight
constexpr auto ITEM_TYPE_CHOICES = 6;
constexpr auto TREES_BUSHES = std::uint8_t(1);    // Trees and other large vegetaion in the way
constexpr auto WALLS_CHIMNEYS = std::uint8_t(2);  // Walls, chimineys, ovens, etc... in the way
constexpr auto DOORS = std::uint8_t(4);           // Doors in the way
constexpr auto ROOFING_SLANTED = std::uint8_t(8); // So can't tele onto slanted roofs, basically
constexpr auto FLOORS_FLAT_ROOFING = std::uint8_t(16); // For attacking between floors
constexpr auto LAVA_WATER = std::uint8_t(32);          // Don't know what all to use this for yet

//==================================================================================================
// types we will get rid of - keeping for now
//==================================================================================================

typedef void(TargetFunc)(CSocket *s);

#endif
