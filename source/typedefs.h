#ifndef __UOXTYPES_H
#define __UOXTYPES_H

#ifdef __NEED_VALIST__
typedef void *  va_list;
#endif

#if PLATFORM != WINDOWS
#define MAX_PATH			268
#endif

#if defined( _DEBUG )
#define VALIDATESOCKET( s ) if( s == nullptr ) \
{ \
Console.print( strutil::format("Socket failure at %s", __FILE__LINE__) );	\
return;	\
}
#else
#define VALIDATESOCKET( s ) if( s == nullptr ) \
return;
#endif

typedef float				R32;
typedef double				R64;
typedef unsigned char		UI08; // 0 to 255
typedef signed char			SI08; // -128 to 127
typedef unsigned short int	UI16; // 0 to 65535
typedef signed short int	SI16; // -32768 to 32767
typedef unsigned int		UI32; // 0 to 4294967295
typedef signed int			SI32; // -2147483648 to 2147483647
typedef unsigned long long	UI64; // 0 to 18446744073709551615
typedef signed long long	SI64; // -9223372036854775808 to 9223372036854775807

typedef UI08		GENDER;
typedef UI08		LIGHTLEVEL;
typedef UI08		COLDLEVEL;
typedef UI08		HEATLEVEL;
typedef UI08		SECONDS;
typedef UI08		ARMORCLASS;
typedef SI08		RACEREL;
typedef SI08		RANGE;
typedef UI16		RACEID;
typedef UI16		COLOUR;
typedef UI16		SKILLVAL;
typedef UI16		weathID;
typedef SI16		GUILDID;
typedef UI32		TIMERVAL;
#if defined(_WIN32)
typedef UI32		UOXSOCKET;
#else
typedef SI32		UOXSOCKET;
#endif
typedef UI32		SERIAL;

const SERIAL		INVALIDSERIAL		= 0xFFFFFFFF;
const UI16			INVALIDID			= 0xFFFF;
const UI16			INVALIDCOLOUR		= 0xFFFF;
const SERIAL		BASEITEMSERIAL		= 0x40000000;

typedef std::vector< CMapRegion * >						REGIONLIST;
typedef std::vector< CMapRegion * >::iterator			REGIONLIST_ITERATOR;
typedef std::vector< CMapRegion * >::const_iterator		REGIONLIST_CITERATOR;

typedef std::vector< CSocket * >						SOCKLIST;
typedef std::vector< CSocket * >::iterator				SOCKLIST_ITERATOR;
typedef std::vector< CSocket * >::const_iterator		SOCKLIST_CITERATOR;
//
typedef std::vector< std::string >						STRINGLIST;
typedef std::vector< std::string >::iterator			STRINGLIST_ITERATOR;
typedef std::vector< std::string >::const_iterator		STRINGLIST_CITERATOR;
//
typedef std::vector< SERIAL >							SERLIST;
typedef std::vector< SERIAL >::iterator					SERLIST_ITERATOR;
typedef std::vector< SERIAL >::const_iterator			SERLIST_CITERATOR;
//
typedef std::vector< CChar* >							CHARLIST;
typedef std::vector< CChar* >::iterator					CHARLIST_ITERATOR;
typedef std::vector< CChar* >::const_iterator			CHARLIST_CITERATOR;
//
typedef std::vector< CItem* >							ITEMLIST;
typedef std::vector< CItem* >::iterator					ITEMLIST_ITERATOR;
typedef std::vector< CItem* >::const_iterator			ITEMLIST_CITERATOR;
//
typedef std::vector< CBaseObject* >						BASEOBJECTLIST;
typedef std::vector< CBaseObject* >::iterator			BASEOBJECTLIST_ITERATOR;
typedef std::vector< CBaseObject* >::const_iterator		BASEOBJECTLIST_CITERATOR;
//
typedef std::map< CBaseObject *, UI32 >					QUEUEMAP;
typedef std::map< CBaseObject *, UI32 >::iterator		QUEUEMAP_ITERATOR;
typedef std::map< CBaseObject *, UI32 >::const_iterator QUEUEMAP_CITERATOR;
//
typedef std::map< UI16, CSpawnRegion * >					SPAWNMAP;
typedef std::map< UI16, CSpawnRegion * >::const_iterator	SPAWNMAP_CITERATOR;
//
typedef std::map< UI16, CTownRegion * >						TOWNMAP;
typedef std::map< UI16, CTownRegion * >::const_iterator		TOWNMAP_CITERATOR;

typedef void (TargetFunc)( CSocket *s );

// December 27, 2000
typedef struct __STARTLOCATIONDATA__
{
	__STARTLOCATIONDATA__()
	{
		memset( this, 0x00, sizeof( __STARTLOCATIONDATA__ ) );
	}
	char	oldTown[31];
	char	oldDescription[31];
	char	newTown[32];
	char	newDescription[32];
	SI16	x;
	SI16	y;
	SI16	z;
	SI16	worldNum;
	UI16	instanceID;
	UI32	clilocDesc;

} STARTLOCATION, *LPSTARTLOCATION;
//	 	-	End

// Max values
const UI08 MAX_NAME		= 128;	// Several areas where we pass a character name will be restricted by packet size to 30 characters.
								// Higher MAX_NAME values do, however, work for items - and are in some cases required (magic item names, for instance). Seems to still work for regular-length names if I increase it, but we might consider splitting this into character/item-specific somehow?
const UI08 MAX_TITLE	= 60;
const UI16 MAX_STACK	= 0xFFFF;
const UI08 MAX_VISRANGE	= 18;
const UI16 MAXBUFFER	= 2560;	// Buffer Size (For socket operations)
const UI08 MAXPOSTS		= 128;	// Maximum number of posts on a messageboard

const SI08 ILLEGAL_Z	= -128;

// Offsets for Gump menu's (Relates to menus.dfn)
#define ITEMMENUOFFSET		256
#define TRACKINGMENUOFFSET	4096
#define POLYMORPHMENUOFFSET 8192

const UI08 NORTH		= 0x00;
const UI08 NORTHEAST	= 0x01;
const UI08 EAST			= 0x02;
const UI08 SOUTHEAST	= 0x03;
const UI08 SOUTH		= 0x04;
const UI08 SOUTHWEST	= 0x05;
const UI08 WEST			= 0x06;
const UI08 NORTHWEST	= 0x07;
const UI08 UNKNOWNDIR	= 0xFF;

// Line Of Sight
#define ITEM_TYPE_CHOICES		6
const UI08 TREES_BUSHES			= 1;	// Trees and other large vegetaion in the way
const UI08 WALLS_CHIMNEYS		= 2;	// Walls, chimineys, ovens, etc... in the way
const UI08 DOORS				= 4;	// Doors in the way
const UI08 ROOFING_SLANTED		= 8;	// So can't tele onto slanted roofs, basically
const UI08 FLOORS_FLAT_ROOFING	= 16;	// For attacking between floors
const UI08 LAVA_WATER			= 32;	// Don't know what all to use this for yet

#endif

