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

#include "uodata/uoxtile.hpp"


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

