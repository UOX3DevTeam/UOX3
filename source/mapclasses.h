#ifndef __UOXMAP_CLASSES_H__
#define __UOXMAP_CLASSES_H__

#include <cstdint>
#include <sstream>
#include <array>
#include <bitset>
#include <algorithm>

#include "power.h"
#include "MultiMul.hpp"
#include "osunique.hpp"


//o------------------------------------------------------------------------------------------------o

enum TileFlags
{
	// Flag:				Also known as:
	TF_FLOORLEVEL	= 0,	// "Background"
	TF_HOLDABLE,			// "Weapon"
	TF_TRANSPARENT,			// "SignGuildBanner"
	TF_TRANSLUCENT,			// "WebDirtBlood"
	TF_WALL,				// "WallVertTile"
	TF_DAMAGING,
	TF_BLOCKING,			// "Impassable"
	TF_WET,					// "LiquidWet"
	TF_UNKNOWN1,			// "Ignored"
	TF_SURFACE,				// "Standable"
	TF_CLIMBABLE,			// "Bridge"
	TF_STACKABLE,			// "Generic"
	TF_WINDOW,				// "WindowArchDoor"
	TF_NOSHOOT,				// "CannotShootThru"
	TF_DISPLAYA,			// "Prefix A"
	TF_DISPLAYAN,			// "Prefix An"
	TF_DESCRIPTION,			// "Internal"
	TF_FOLIAGE,				// "FadeWithTrans"
	TF_PARTIALHUE,
	TF_UNKNOWN2,
	TF_MAP,
	TF_CONTAINER,
	TF_WEARABLE,			// "Equipable"
	TF_LIGHT,				// "LightSource"
	TF_ANIMATED,
	TF_NODIAGONAL,			// "HoverOver" in SA clients and later, to determine if tiles can be moved on by flying gargoyles
	TF_UNKNOWN3,			// "NoDiagonal" in SA clients and later?
	TF_ARMOR,				// "WholeBodyItem"
	TF_ROOF,				// "WallRoofWeap"
	TF_DOOR,
	TF_STAIRBACK,			// "ClimbableBit1"
	TF_STAIRRIGHT,			// "ClimbableBit2"

	// Following flags were added in HS expansion? Purpose unknown
	TF_ALPHABLEND,
	TF_USENEWART,
	TF_ARTUSED,
	TF_NOSHADOW,
	TF_PIXELBLEED,
	TF_PLAYANIMONCE,
	TF_MULTIMOVABLE,
	TF_COUNT
};

//o------------------------------------------------------------------------------------------------o
// Pre declare tileinfo so we can make it a friend of CBaseTile, CTile, and CLand
class TileInfo;


//o------------------------------------------------------------------------------------------------o
// Frankly, these chould/should have been structures, There really isn't a reason to make the
// data private, and then expose everything with accessors.  Just adds a lot of verbage,
// that can clutter when maintaining.  Oh well, we stay compatable for now.
class CBaseTile
{
	friend TileInfo;
protected:
	std::bitset<64> flags; // We are going to use a 64 bit value here, so specify 64 bit.
					// This one day may be changed to a vector of bools, but for now
	std::string name;
	
public:
	CBaseTile()
	{
		flags.reset();
	}
	virtual ~CBaseTile() = default;
	// This doesnt seem to be used, but I wonder about DFN overloads?
	//
	/*
	 auto Flag( UI08 part ) const  -> std::uint8_t
	 {
	 	auto mFlags = static_cast<std::uint32_t>( flags.to_ulong() );
	 	auto retVal = std::uint8_t( 0 );
	 	switch( part )
	 	{
	 		case 0: retVal = static_cast<std::uint8_t>( mFlags >> 24 );	break;
	 		case 1: retVal = static_cast<std::uint8_t>( mFlags >> 16 );	break;
	 		case 2: retVal = static_cast<std::uint8_t>( mFlags >> 8 );	break;
	 		case 3: retVal = static_cast<std::uint8_t>( mFlags % 256 );	break;
	 	}
	 	return retVal;
	 }
	 */
	auto Name() const -> const std::string& { return name; }
	auto Name( const std::string &value ) -> void { name = value; }
	
	auto FlagsNum() const -> std::uint32_t {	return static_cast<std::uint32_t>( flags.to_ulong() ); }
	auto Flags() const -> std::bitset<64> { return flags; }
	void Flags( std::bitset<64> newVal ) { flags = newVal; }
	
	bool CheckFlag( TileFlags toCheck ) const
	{
		if( toCheck >= TF_COUNT )
		{
			return false;
		}
		return flags.test( toCheck );
	}
	void SetFlag( TileFlags toSet, bool newVal )
	{
		if( toSet >= TF_COUNT )
		{
			return;
		}
		flags.set( toSet, newVal );
	}
};
//o------------------------------------------------------------------------------------------------o
class CTile : public CBaseTile
{
private:
	friend TileInfo;
	std::uint8_t weight;
	std::int8_t layer;
	std::uint16_t unknown1;
	std::uint8_t unknown2;
	std::uint8_t quantity;
	std::uint16_t animation;
	std::uint8_t unknown3;
	std::uint8_t hue;
	std::uint8_t unknown4;
	std::uint8_t unknown5;
	std::int8_t height;
public:
	CTile() :CBaseTile(), weight( 0 ), layer( 0 ), unknown1( 0 ), unknown2( 0 ), quantity( 0 ), animation( 0 ), unknown3( 0 ), hue( 0 ), unknown4( 0 ), unknown5( 0 ), height( 0 )
	{
	}
	~CTile() = default;
	auto Unknown1() const -> std::uint16_t { return unknown1; }
	auto Unknown2() const -> std::uint8_t { return unknown2; }
	auto Unknown3() const -> std::uint8_t { return unknown3; }
	auto Unknown4() const -> std::uint8_t { return unknown4; }
	auto Unknown5() const -> std::uint8_t { return unknown5; }
	auto Hue() const -> std::uint8_t { return hue; }
	auto Quantity() const -> std::uint8_t { return quantity; }
	auto Animation() const -> std::uint16_t { return animation; }
	auto Weight() const -> std::uint8_t { return weight; }
	auto Layer() const -> std::int8_t { return layer; }
	auto Height() const -> std::int8_t { return height; }
	auto ClimbHeight() const -> std::int8_t
	{
		if( CheckFlag( TF_CLIMBABLE ))
		{
			return height / 2;
		}
		return height;
	}
	void Unknown1( std::uint16_t newVal ) { unknown1 = newVal; }
	void Unknown2( std::uint8_t newVal ) { unknown2 = newVal; }
	void Unknown3( std::uint8_t newVal ) { unknown3 = newVal; }
	void Unknown4( std::uint8_t newVal ) { unknown4 = newVal; }
	void Unknown5( std::uint8_t newVal ) { unknown5 = newVal; }
	void Animation( std::uint16_t newVal ) { animation = newVal; }
	void Weight( std::uint8_t newVal ) { weight = newVal; }
	void Layer( std::int8_t newVal ) { layer = newVal; }
	void Height( std::int8_t newVal ) { height = newVal; }
	void Hue( std::uint8_t newVal ) { hue = newVal; }
	void Quantity( std::uint8_t newVal ) { quantity = newVal; }
};
//o------------------------------------------------------------------------------------------------o
class CLand : public CBaseTile
{
	friend TileInfo;
private:
	std::uint16_t textureId;
	// List of road tiles, or road-related tiles, on which houses cannot be placed
	constexpr static
	std::array<std::uint16_t, 18> roadIds
	{
		0x0009, 0x0015, // furrows
		0x0071, 0x0078, // dirt
		0x00E8, 0x00EB, // dirt
		0x0150, 0x015C, // furrows
		0x0442, 0x0479, // sand stone
		0x0501, 0x0510, // sand stone
		0x07AE, 0x07B1, // dirt
		0x3FF4, 0x3FF4, // dirt
		0x3FF8, 0x3FFB  // dirt
	};
	
public:
	CLand() :CBaseTile(), textureId( 0 )
	{
	}
	~CLand() = default;
	auto TextureId() const -> std::uint16_t { return textureId; }
	auto TextureId( std::uint16_t newVal ) -> void { textureId = newVal; }
	auto IsRoadId() const -> bool
	{
		auto rValue = false;
		for( size_t j = 0; j < roadIds.size(); j += 2 )
		{
			if( textureId >= roadIds[j] && textureId <= roadIds[j + 1] )
			{
				rValue = true;
				break;
			}
		}
		return rValue;
	}
};


enum TileType_t { terrain, art, dyn };

// A structure that holds tile information. Type, id, information, altitude (if in a map), hue (if a static tile)

struct Tile_st
{
	TileType_t type;
	std::uint16_t tileId;	// for instance, this should be a tileid_t , that would make sense!
	std::int8_t altitude;
	std::uint16_t staticHue;
	union
	{
		const CTile *artInfo;
		const CLand *terrainInfo;
	};
	constexpr static std::array<std::uint16_t, 11> terrainVoids
	{
		430, 431, 432,
		433, 434, 475,
		580, 610, 611,
		612, 613
	};
	auto CheckFlag( TileFlags toCheck ) const -> bool
	{
		if( type != TileType_t::terrain )
		{
			return artInfo->CheckFlag( toCheck );
		}
		return terrainInfo->CheckFlag( toCheck );
	}
	Tile_st( TileType_t type = TileType_t::terrain ) :type( type ), tileId( 0 ), altitude( 0 ), staticHue( 0 ), artInfo( nullptr ) {}
	auto isVoid() const -> bool
	{
		auto rValue = false;
		if( type == TileType_t::terrain )
		{
			auto iter = std::find_if( terrainVoids.begin(), terrainVoids.end(), [this]( const std::uint16_t &value )
			{
				return tileId == value;
			});
			rValue = iter != terrainVoids.end();
		}
		return rValue;
	}
	auto IsMountain() const -> bool
	{
		auto rValue = false;
		if( type != TileType_t::terrain )
		{
			rValue = (( tileId >= 431 ) && ( tileId <= 432 )) || (( tileId >= 467 ) && ( tileId <= 474 )) ||
			(( tileId >= 543 ) && ( tileId <= 560 )) || (( tileId >= 1754 ) && ( tileId <= 1757 )) ||
			(( tileId >= 1787 ) && ( tileId <= 1789 )) || (( tileId >= 1821 ) && ( tileId <= 1824 )) ||
			(( tileId >= 1851 ) && ( tileId <= 1854 )) || (( tileId >= 1881 ) && ( tileId <= 1884 ));
		}
		return rValue;
	}
	auto top() -> std::int16_t
	{
		auto value = static_cast<int16_t>( altitude );
		if( type != TileType_t::terrain )
		{
			auto temp = artInfo->Height();
			temp = ( temp & 0x8 ? ( temp & 0xF ) >> 1 : temp & 0xF );
			
			value += temp;
		}
		return value;
	}
	auto height() -> std::uint8_t
	{
		if( type != TileType_t::terrain )
		{
			return artInfo->Height();
		}
		return 0;
	}
	auto name() const -> std::string
	{
		if( type != TileType_t::terrain )
		{
			return artInfo->Name();
		}
		return terrainInfo->Name();
	}
};
#endif

