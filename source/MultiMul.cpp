// 
// Created on:  4/30/21

#include "MultiMul.hpp"

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <unordered_map>
#include "mapstuff.h"

using namespace std::string_literals;

//o------------------------------------------------------------------------------------------------o
// MultiItem_st
//o------------------------------------------------------------------------------------------------o
MultiItem_st::MultiItem_st() : tileId( 0xFFFF ), altitude( 0 ), offsetX( 0 ), offsetY( 0 ), flag( 0 )
{
	info = nullptr;
}

//o------------------------------------------------------------------------------------------------o
// CollectionItem_st
//o------------------------------------------------------------------------------------------------o

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CollectionItem_st::ItemsAt()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns collection of items at given coordinates
//o------------------------------------------------------------------------------------------------o
auto CollectionItem_st::ItemsAt( int offsetX, int offsetY ) -> std::vector<MultiItem_st>
{
	std::vector<MultiItem_st> rValue;
	for( auto &item : items )
	{
		if(( item.offsetX == static_cast<std::int16_t>( offsetX )) && ( item.offsetY == static_cast<std::int16_t>( offsetY )))
		{
			rValue.push_back( item );
		}
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
CollectionItem_st::CollectionItem_st() :minX( 0 ),  minY( 0 ), maxX( 0 ), maxY( 0 ), minAlt( 0 ), maxAlt( 0 )
{
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CollectionItem_st::Name()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Return name of item in collection
//o------------------------------------------------------------------------------------------------o
auto CollectionItem_st::Name( int multi_id ) -> std::string
{
	try
	{
		auto name = collectionNames.at( multi_id );
		return name;
	}
	catch(...)
	{
		return "unknown - "s + std::to_string( multi_id );
	}
}
//=========================================================
const std::unordered_map<int, std::string> CollectionItem_st::collectionNames = {
	{ 0, "Small Boat [north]"s }, { 1, "Small Boat [east]"s },
	{ 2, "Small Boat [south]"s }, { 3, "Small Boat [west]"s },
	{ 4, "Small Dragon Boat [north]"s }, { 5, "Small Dragon Boat [east]"s },
	{ 6, "Small Dragon Boat [south]"s }, { 7, "Small Dragon Boat [west]"s },
	{ 8, "Medium Boat [north]"s }, { 9, "Medium Boat [east]"s },
	{ 10, "Medium Boat [south]"s }, { 11, "Medium Boat [west]"s },
	{ 12, "Medium Dragon Boat [north]"s }, { 13, "Medium Dragon Boat [east]"s },
	{ 14, "Medium Dragon Boat [south]"s }, { 15, "Medium Dragon Boat [west]"s },
	{ 16, "Large Boat [north]"s }, { 17, "Large Boat [east]"s },
	{ 18, "Large Boat [soutch]"s }, { 19, "Large Boat [west]"s },
	{ 20, "Large Dragon Boat [north]"s }, { 21, "Large Dragon Boat [east]"s },
	{ 22, "Large Dragon Boat [south]"s }, { 23, "Large Dragon Boat [west]"s },
	{ 24, "Orcish Galleon [north]"s }, { 25, "Orcish Galleon [east]"s },
	{ 26, "Orcish Galleon [south]"s }, { 27, "Orcish Galleon [west]"s },
	{ 28, "Orcish Galleon [damaged][north]"s }, { 29, "Orcish Galleon [damaged][east]"s },
	{ 30, "Orcish Galleon [damaged][south]"s }, { 31, "Orcish Galleon [damaged][west]"s },
	{ 32, "Orcish Galleon [destroyed][north]"s }, { 33, "Orcish Galleon [destroyed][east]"s },
	{ 34, "Orcish Galleon [destroyed][south]"s }, { 35, "Orcish Galleon [destroyed][west]"s },

	{ 36, "Gargish Galleon [north]"s }, { 37, "Gargish Galleon [east]"s },
	{ 38, "Gargish Galleon [south]"s }, { 39, "Gargish Galleon [west]"s },
	{ 40, "Gargish Galleon [damaged][north]"s }, { 41, "Gargish Galleon [damaged][east]"s },
	{ 42, "Gargish Galleon [damaged][south]"s }, { 43, "Gargish Galleon [damaged][west]"s },
	{ 44, "Gargish Galleon [destroyed][north]"s }, { 45, "Gargish Galleon [destroyed][east]"s },
	{ 46, "Gargish Galleon [destroyed][south]"s }, { 47, "Gargish Galleon [destroyed][west]"s },

	{ 48, "Tokuno Galleon [north]"s }, { 49, "Tokuno Galleon [east]"s },
	{ 50, "Tokuno Galleon [south]"s }, { 51, "Tokuno Galleon [west]"s },
	{ 52, "Tokuno Galleon [damaged][north]"s }, { 53, "Tokuno Galleon [damaged][east]"s },
	{ 54, "Tokuno Galleon [damaged][south]"s }, { 55, "Tokuno Galleon [damaged][west]"s },
	{ 56, "Tokuno Galleon [destroyed][north]"s }, { 57, "Tokuno Galleon [destroyed][east]"s },
	{ 58, "Tokuno Galleon [destroyed][south]"s }, { 59, "Tokuno Galleon [destroyed][west]"s },

	{ 60, "Rowboat [north]"s }, { 61, "Rowboat [east]"s },
	{ 62, "Rowboat [south]"s }, { 63, "Rowboat [west]"s },

	{ 64, "British Galleon [north]"s }, { 65, "British Galleon [east]"s },
	{ 66, "British Galleon [south]"s }, { 67, "British Galleon [west]"s },
	{ 68, "British Galleon [damaged][north]"s }, { 69, "British Galleon [damaged][east]"s },
	{ 70, "British Galleon [damaged][south]"s }, { 71, "British Galleon [damaged][west]"s },

	{ 100, "Small Stone and Plaster House"s }, { 101, "Small Stone and Plaster House"s },
	{ 102, "Small Fieldstone House"s }, { 103, "Small Fieldstone House"s },
	{ 104, "Small Brick House"s }, { 105, "Small Brick House"s },
	{ 106, "Small Wood House"s }, { 107, "Small Wood House"s },
	{ 108, "Small Wood and Plaster House"s }, { 109, "Small Wood and Plaster House"s },
	{ 110, "Small Thatched Roof Cottage"s }, { 111, "Small Thatched Roof Cottage"s },
	{ 112, "Tent [blue]"s }, { 113, "Tent [blue]"s },
	{ 114, "Tent [green]"s }, { 115, "Tent [green]"s },
	{ 116, "Large Brick House"s }, { 117, "Large Brick House"s },
	{ 118, "Two Story Wood and Plaster House"s }, { 119, "Two Story Wood and Plaster House"s },
	{ 120, "Two Story Stone and Plaster House"s }, { 121, "Two Story Stone and Plaster House"s },
	{ 122, "Large Tower"s }, { 123, "Large Tower"s },
	{ 124, "Stone Keep"s }, { 125, "Stone Keep"s },
	{ 126, "Castle"s }, { 127, "Castle"s },
	{ 128, "Large Patio House"s }, { 129, "Large Patio House"s },
	{ 141, "Large Patio House"s }, { 150, "Large Marble Patio House"s },
	{ 152, "Small Tower"s }, { 154, "Log Cabin"s },
	{ 156, "Sandstone Patio House"s }, { 158, "Two-Story Villa"s },
	{ 160, "Small Stone Workshop"s }, { 162, "Small Marble Workshop"s },
	{ 201, "Mine's Elevator"s }, { 202, "Lifting Bridge"s },
	{ 500, "Wandering Healer Camp"s }, { 501, "Wandering Mage Camp"s },
	{ 502, "Wandering Bank Camp"s },

	{ 1000, "Treasure Pile"s }, { 1001, "Treasure Pile"s },
	{ 1002, "Treasure Pile"s }, { 1003, "Treasure Pile"s },

	{ 2000, "Wooden Supports"s }, { 2001, "Wooden Supports"s },
	{ 2002, "Wooden Supports"s }, { 2003, "Wooden Supports"s },
	{ 2004, "Wooden Supports"s }, { 2005, "Wooden Supports"s },
	{ 2006, "Wooden Supports"s }, { 2007, "Wooden Supports"s },

	{ 3000, "Farmer's Cabin"s },

	{ 4000, "Exodus Generator"s }, { 4001, "Exodus Generator"s },
	{ 4002, "Exodus Control Pylon"s },

	{ 4003, "Nystul's Tower (stage 1)"s }, { 4004, "Nystul's Tower (stage 2)"s },
	{ 4005, "Nystul's Tower (stage 3)"s }, { 4006, "Nystul's Tower (stage 4)"s },
	{ 4007, "Nystul's Tower (stage 5)"s }, { 4008, "Nystul's Tower (stage 6)"s },

	{ 4009, "Scaffold and Trailer"s }, { 4010, "Exodus Energy Tower"s },
	{ 4011, "Exodus Barrier"s }, { 4012, "Exodus Control Device"s },
	{ 5000, "Minax Stronghold"s },

	{ 5100, "Foundation [7x7]"s }, { 5101, "Foundation [7x8]"s },
	{ 5102, "Foundation [7x9]"s }, { 5103, "Foundation [7x10]"s },
	{ 5104, "Foundation [7x11]"s }, { 5105, "Foundation [7x12]"s },

	{ 5112, "Foundation [8x7]"s }, { 5113, "Foundation [8x8]"s },
	{ 5114, "Foundation [8x9]"s }, { 5115, "Foundation [8x10]"s },
	{ 5116, "Foundation [8x11]"s }, { 5117, "Foundation [8x12]"s },
	{ 5118, "Foundation [8x13]"s },

	{ 5124, "Foundation [9x7]"s }, { 5125, "Foundation [9x8]"s },
	{ 5126, "Foundation [9x9]"s }, { 5127, "Foundation [9x10]"s },
	{ 5128, "Foundation [9x11]"s }, { 5129, "Foundation [9x12]"s },
	{ 5130, "Foundation [9x13]"s }, { 5131, "Foundation [9x14]"s },

	{ 5136, "Foundation [10x7]"s }, { 5137, "Foundation [10x8]"s },
	{ 5138, "Foundation [10x9]"s }, { 5139, "Foundation [10x10]"s },
	{ 5140, "Foundation [10x11]"s }, { 5141, "Foundation [10x12]"s },
	{ 5142, "Foundation [10x13]"s }, { 5143, "Foundation [10x14]"s },
	{ 5144, "Foundation [10x15]"s },

	{ 5148, "Foundation [11x7]"s }, { 5149, "Foundation [11x8]"s },
	{ 5150, "Foundation [11x9]"s }, { 5151, "Foundation [11x10]"s },
	{ 5152, "Foundation [11x11]"s }, { 5153, "Foundation [11x12]"s },
	{ 5154, "Foundation [11x13]"s }, { 5155, "Foundation [11x14]"s },
	{ 5156, "Foundation [11x15]"s }, { 5157, "Foundation [11x16]"s },


	{ 5160, "Foundation [12x7]"s }, { 5161, "Foundation [12x8]"s },
	{ 5162, "Foundation [12x9]"s }, { 5163, "Foundation [12x10]"s },
	{ 5164, "Foundation [12x11]"s }, { 5165, "Foundation [12x12]"s },
	{ 5166, "Foundation [12x13]"s }, { 5167, "Foundation [12x14]"s },
	{ 5168, "Foundation [12x15]"s }, { 5169, "Foundation [12x16]"s },
	{ 5170, "Foundation [12x17]"s },


	{ 5173, "Foundation [13x8]"s }, { 5174, "Foundation [13x9]"s },
	{ 5175, "Foundation [13x10]"s }, { 5176, "Foundation [13x11]"s },
	{ 5177, "Foundation [13x12]"s }, { 5178, "Foundation [13x13]"s },
	{ 5179, "Foundation [13x14]"s }, { 5180, "Foundation [13x15]"s },
	{ 5181, "Foundation [13x16]"s }, { 5182, "Foundation [13x17]"s },
	{ 5183, "Foundation [13x18]"s },

	{ 5186, "Foundation [14x9]"s }, { 5187, "Foundation [14x10]"s },
	{ 5188, "Foundation [14x11]"s }, { 5189, "Foundation [14x12]"s },
	{ 5190, "Foundation [14x13]"s }, { 5191, "Foundation [14x14]"s },
	{ 5192, "Foundation [14x15]"s }, { 5193, "Foundation [14x16]"s },
	{ 5194, "Foundation [14x17]"s }, { 5195, "Foundation [14x18]"s },

	{ 5199, "Foundation [15x10]"s }, { 5200, "Foundation [15x11]"s },
	{ 5201, "Foundation [15x12]"s }, { 5202, "Foundation [15x13]"s },
	{ 5203, "Foundation [15x14]"s }, { 5204, "Foundation [15x15]"s },
	{ 5205, "Foundation [15x16]"s }, { 5206, "Foundation [15x17]"s },
	{ 5207, "Foundation [15x18]"s },

	{ 5212, "Foundation [16x11]"s }, { 5213, "Foundation [16x12]"s },
	{ 5214, "Foundation [16x13]"s }, { 5215, "Foundation [16x14]"s },
	{ 5216, "Foundation [16x15]"s }, { 5217, "Foundation [16x16]"s },
	{ 5218, "Foundation [16x17]"s }, { 5219, "Foundation [16x18]"s },

	{ 5225, "Foundation [17x12]"s }, { 5226, "Foundation [17x13]"s },
	{ 5227, "Foundation [17x14]"s }, { 5228, "Foundation [17x15]"s },
	{ 5229, "Foundation [17x16]"s }, { 5230, "Foundation [17x17]"s },
	{ 5231, "Foundation [17x18]"s },

	{ 5238, "Foundation [18x13]"s }, { 5239, "Foundation [18x14]"s },
	{ 5240, "Foundation [18x15]"s }, { 5241, "Foundation [18x16]"s },
	{ 5242, "Foundation [18x17]"s }, { 5243, "Foundation [18x18]"s },

	{ 7500, "New Player Quest Camp"s }
};

//o------------------------------------------------------------------------------------------------o
//|	Function	-	MultiCollection::MultiCollection()
//o------------------------------------------------------------------------------------------------o
MultiCollection::MultiCollection( const std::filesystem::path &uodir, TileInfo *info )
{
	this->info = info;
	if( !uodir.empty() )
	{
		LoadMultiCollection( uodir, info );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	MultiCollection::LoadMultiCollection()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Load Multi MUL data from disk
//o------------------------------------------------------------------------------------------------o
auto MultiCollection::LoadMultiCollection( const std::filesystem::path &uodir, const std::string &housingbin, TileInfo *info ) -> bool
{
	_multis.clear();
	auto multifile = uodir / std::filesystem::path( "MultiCollection.uop" );
	auto hash = "build/multicollection/%.6u.bin"s;
	_housingBin = housingbin;
	this->info = info;
	return LoadUop( multifile.string(), 0x10000, hash );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	MultiCollection::LoadMul()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Load Multi MUL data from disk
//o------------------------------------------------------------------------------------------------o
auto MultiCollection::LoadMul( const std::filesystem::path &uodir, TileInfo *info ) -> bool
{
	_multis.clear();
	this->info = info;
	auto rValue = false;
	auto idxfile = uodir / std::filesystem::path( "multi.idx" );
	auto mulfile = uodir / std::filesystem::path( "multi.mul" );
	auto isHS = true;
	if( std::filesystem::exists( mulfile ))
	{
		isHS = ( std::filesystem::file_size( mulfile) >= hsSize ? true : false );
	}
	auto idx = std::ifstream( idxfile, std::ios::binary );
	auto mul = std::ifstream( mulfile, std::ios::binary );
	if( mul.is_open() && idx.is_open() )
	{
		rValue = true;
		auto offset = std::uint32_t( 0 );
		auto length = std::uint32_t( 0 );
		auto mulid = 0;
		while( idx.good() && !idx.eof() )
		{
			idx.read( reinterpret_cast<char*>( &offset ), 4 );
			idx.read( reinterpret_cast<char*>( &length ), 4 );
			if( idx.gcount() == 4 )
			{
				idx.seekg( 4, std::ios::cur );
				if(( offset <= 0xFFFFFFFE ) && ( length > 0 ) && ( length < 0xFFFFFFFF ))
				{
					// This is a valid entry;
					mul.seekg( offset, std::ios::beg );
					auto data = std::vector<std::uint8_t>( length, 0 );
					mul.read( reinterpret_cast<char*>( data.data() ), length );
					ProcessData( isHS, mulid, data );
				}
			}
			mulid += 1;
		}
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	MultiCollection::LoadMultiCollection()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Load Multi UOP data from disk
//o------------------------------------------------------------------------------------------------o
auto MultiCollection::LoadMultiCollection( const std::filesystem::path &uodir, TileInfo *info ) -> bool
{
	auto rValue = false;
	// First, see if the uop exists?
	auto multifile = uodir / std::filesystem::path( "MultiCollection.uop" );
	if( std::filesystem::exists( multifile ))
	{
		// It does, so lets load the uop
		rValue = LoadMultiCollection( uodir, "", info );
	}
	else
	{
		rValue = LoadMul( uodir, info );
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	MultiCollection::ProcessHash()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	???
//o------------------------------------------------------------------------------------------------o
auto MultiCollection::ProcessHash( std::uint64_t hash, [[maybe_unused]] std::size_t entry, std::vector<std::uint8_t> &data ) -> bool
{
	if( hash == 0x126D1E99DDEDEE0ALL )
	{
		if( !_housingBin.empty() )
		{
			auto output = std::ofstream( _housingBin, std::ios::binary );
			if( output.is_open() )
			{
				output.write( reinterpret_cast<char*>( data.data() ), data.size() );
			}
		}
		return false;  // This is the hash for the housing.bin file
	}
	return true;
}
//o------------------------------------------------------------------------------------------------o
//|	Function	-	MultiCollection::ProcessEntry()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	???
//o------------------------------------------------------------------------------------------------o
auto MultiCollection::ProcessEntry( [[maybe_unused]] std::size_t entry, std::size_t index, std::vector<std::uint8_t> &data ) -> bool
{
	if( data.size() > 8 )
	{
		auto offset = std::uint32_t( 4 );
		// We skip the first 32 bit word (why offset is 4).
		// The next 32 bit word is the number of entries in this data;

		auto count = std::int32_t( 0 );
		std::copy( data.data() + offset, data.data() + offset + 4, reinterpret_cast<std::uint8_t*>( &count ));
		offset += 4;;
		// we increment offfset now that we have read the count
		// at this point,we are now looking at the data for each entry;
		// before we start looping, make some dummy read space we can get data
		// out of the vector
		auto flag = std::int16_t( 0 );
		auto clilocs = std::uint32_t( 0 );
		auto multi = CollectionItem_st();
		multi.items.resize( count );
		auto first = true;
		for( auto entry = 0; entry < count; ++entry )
		{
			auto item = MultiItem_st();
			// Get the tileId
			std::copy( data.begin() + offset, data.begin() + offset + 2, reinterpret_cast<unsigned char*>( &item.tileId ));
			offset += 2;
			if( info )
			{
				item.info = &info->ArtInfo( item.tileId );
			}
			// Now get the xoffset  --
			std::copy( data.begin()+offset, data.begin() + offset + 2, reinterpret_cast<unsigned char*>( &flag ));
			item.offsetX = static_cast<int>(flag);
			offset += 2;
			// Now get the yoffset
			std::copy( data.begin()+offset, data.begin() + offset + 2, reinterpret_cast<unsigned char*>( &flag ));
			item.offsetY = static_cast<int>(flag);
			offset += 2;
			// Now get the z
			std::copy( data.begin()+offset, data.begin() + offset + 2, reinterpret_cast<unsigned char*>( &flag ));
			item.altitude = static_cast<int>(flag);
			offset += 2;
			// now we need to get the flag
			std::copy( data.begin()+offset, data.begin() + offset + 2, reinterpret_cast<unsigned char*>( &flag ));
			offset += 2;
			switch( flag )
			{
				default:
				case 0:
					item.flag = 1;
					break;
				case 1:
					break;
				case 257:
					item.flag = 0x800;
					break;
			}
			if( flag == 0 )
			{
				if( first )
				{
					multi.maxY = item.offsetY;
					multi.minY = item.offsetY;
					multi.maxX = item.offsetX;
					multi.minX = item.offsetX;
					multi.maxAlt = item.altitude;
					multi.minAlt = item.altitude;
					first = false;
				}
				else
				{
					multi.maxY = std::max( item.offsetY, multi.maxY );
					multi.minY = std::max( item.offsetY, multi.minY );
					multi.maxX = std::max( item.offsetX, multi.maxX );
					multi.minX = std::max( item.offsetX, multi.minX );
					multi.maxAlt = std::max( item.altitude, multi.maxAlt );
					multi.minAlt = std::max( item.altitude, multi.minAlt );
				}
			}
			// we now have some clilocs to read past, have to find out how many
			std::copy( data.begin() + offset, data.begin() + offset + 4, reinterpret_cast<unsigned char*>( &clilocs ));
			offset += 4;
			// Now skip past them;
			offset += ( clilocs * 4 );
			multi.items[entry] = std::move( item );
		}
		_multis.insert_or_assign( static_cast<int>( index ), std::move( multi ));
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	MultiCollection::ProcessData()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	???
//o------------------------------------------------------------------------------------------------o
auto MultiCollection::ProcessData( bool isHS, int index, std::vector<std::uint8_t> &data ) -> void
{
	auto size = ( isHS ? 16 : 12 );
	auto count = static_cast<int>( data.size() ) / size;
	auto multi = CollectionItem_st();
	auto first = true;
	for( auto entry = 0; entry < count; ++entry )
	{
		auto item = MultiItem_st();

		std::copy( data.data() + ( entry * size ), data.data() + ( entry * size ) + 2, reinterpret_cast<std::uint8_t*>( &item.tileId ));
		if( info )
		{
			item.info = &( info->ArtInfo( item.tileId ));
		}
		auto value = std::int16_t( 0 );
		std::copy( data.data() + ( entry * size ) + 2, data.data() + ( entry * size ) + 2 + 2, reinterpret_cast<std::uint8_t*>( &value ));
		item.offsetX = value;
		std::copy( data.data() + ( entry * size ) + 4, data.data() + ( entry * size ) + 4 + 2, reinterpret_cast<std::uint8_t*>( &value ));
		item.offsetY = value;
		std::copy( data.data() + ( entry * size ) + 6, data.data() + ( entry * size ) + 6 + 2, reinterpret_cast<std::uint8_t*>( &value ));
		item.altitude = value;

		if( isHS )
		{
			auto flag = std::uint64_t( 0 );
			std::copy( data.data() + ( entry * size ) + 8, data.data() + ( entry * size ) + 8 + 8, reinterpret_cast<std::uint8_t*>( &flag ));
			item.flag = static_cast<std::uint16_t>( flag );
		}
		else
		{
			auto flag = std::uint32_t( 0 );
			std::copy( data.data() + ( entry * size ) + 4, data.data() + ( entry * size ) + 8 + 4, reinterpret_cast<std::uint8_t*>( &flag ));
			item.flag = static_cast<std::uint16_t>( flag );
		}
		if( first )
		{
			multi.maxY = item.offsetY;
			multi.minY = item.offsetY;
			multi.maxX = item.offsetX;
			multi.minX = item.offsetX;
			multi.maxAlt = item.altitude;
			multi.minAlt = item.altitude;
			first = false;
		}
		else
		{
			multi.maxY = std::max( item.offsetY, multi.maxY );
			multi.minY = std::max( item.offsetY, multi.minY );
			multi.maxX = std::max( item.offsetX, multi.maxX );
			multi.minX = std::max( item.offsetX, multi.minX );
			multi.maxAlt = std::max( item.altitude, multi.maxAlt );
			multi.minAlt = std::max( item.altitude, multi.minAlt );
		}

		multi.items.push_back( item );
	}
	_multis[index] = multi;
}

auto MultiCollection::Size() const -> size_t
{
	return _multis.size();
}

auto MultiCollection::Multis() const -> const std::unordered_map<int, CollectionItem_st>&
{
	return _multis;
}

auto MultiCollection::Multis() -> std::unordered_map<int, CollectionItem_st>&
{
	return _multis;
}

auto MultiCollection::Multi( std::uint16_t multiId ) -> CollectionItem_st&
{
	return _multis.at( static_cast<int>( multiId ));
}

auto MultiCollection::Multi( std::uint16_t multiId ) const -> const CollectionItem_st&
{
	return _multis.at( static_cast<int>( multiId ));

}

auto MultiCollection::Exists( std::uint16_t multiId ) const -> bool
{
	auto rValue = false;
	auto iter = _multis.find( multiId );
	if( iter != _multis.end() )
	{
		rValue = true;;
	}
	return rValue;
}
