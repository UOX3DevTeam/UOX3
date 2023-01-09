// 
// Created on:  4/30/21

#ifndef MultiMul_hpp
#define MultiMul_hpp

#include <cstdint>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <filesystem>

#include "UOPData.hpp"
#include "mapclasses.h"
class TileInfo;
//==================================================================================================
// MultiItem_st
//==================================================================================================
// A tile item in a multi.  Contains its offset from the center point of the multi structure, its
// altitude relative to the center point of the tile, and its multi flag (is it a placeholder item,
// should be replaced with a dynamic).
struct MultiItem_st
{
	std::uint16_t tileId;
	int altitude;
	int offsetX;
	int offsetY;
	std::uint16_t flag;
	CTile *info;
	MultiItem_st();
};

//==================================================================================================
// collection_item
//==================================================================================================
// So a multi is a colleciton (thus collection_item) of MultiItem_st.
struct CollectionItem_st
{
	int minX;
	int minY;
	int maxX;
	int maxY;
	int minAlt;
	int maxAlt;
	std::vector<MultiItem_st> items;
	CollectionItem_st();
	static const std::unordered_map<int, std::string> collectionNames;
	static auto Name( int multiId ) -> std::string;
	auto ItemsAt( int offsetX, int offsetY ) -> std::vector<MultiItem_st>;
};

//==================================================================================================
// multicollection
//==================================================================================================
class MultiCollection : public UopFile
{
	constexpr static auto hsSize = 908592;
	TileInfo *info;
	std::unordered_map<int, CollectionItem_st> _multis;

	std::string _housingBin;
	auto virtual ProcessEntry( std::size_t entry, std::size_t index, std::vector<std::uint8_t> &data ) -> bool final;
	auto virtual ProcessHash( std::uint64_t hash, std::size_t entry, std::vector<std::uint8_t> &data ) -> bool final;
	auto ProcessData( bool isHS, int index, std::vector<std::uint8_t> &data ) -> void;

public:
	MultiCollection( const std::filesystem::path &uodir = std::filesystem::path(), TileInfo *info = nullptr );

	auto LoadMultiCollection( const std::filesystem::path &uodir, TileInfo *info = nullptr ) -> bool;
	auto LoadMultiCollection( const std::filesystem::path &uodir, const std::string &housingbin, TileInfo *info = nullptr ) -> bool;
	auto LoadMul( const std::filesystem::path &uodir, TileInfo *info = nullptr ) -> bool;
	auto Size() const -> size_t;
	auto Multis() const -> const std::unordered_map<int, CollectionItem_st>&;
	auto Multis() -> std::unordered_map<int, CollectionItem_st>&;
	auto Multi( std::uint16_t multid ) -> CollectionItem_st&;
	auto Multi( std::uint16_t multid ) const -> const CollectionItem_st&;
	auto Exists( std::uint16_t multid ) const -> bool;
};

#endif /* MultiMul_hpp */
