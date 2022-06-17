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
class tileinfo ;
//=========================================================================================================
// multi_item
//=========================================================================================================
// A tile item in a multi.  Contains its offset from the center point of the multi structure, its altitude relative to the center point of the tile, and its multi flag (is it a placeholder item, should be replaced with a dynamic).
struct multi_item {
	std::uint16_t tileid ;
	int altitude ;
	int offsetx ;
	int offsety ;
	std::uint16_t flag ;
	CTile *info ;
	multi_item() ;
};

//=========================================================================================================
// collection_item
//=========================================================================================================
// So a multi is a colleciton (thus collection_item) of multi_item.  T
struct collection_item {
	int min_y;
	int max_y;
	int min_alt ;
	int max_alt;
	int min_x ;
	int max_x ;
	std::vector<multi_item> items ;
	collection_item();
	static const std::unordered_map<int,std::string> collection_names ;
	static auto name(int multi_id) ->std::string ;
	auto itemsAt(int offsetx,int offsety) ->std::vector<multi_item> ;
};
//=========================================================================================================
// multicollection
//=========================================================================================================

//=========================================================
class multicollection : public uopfile {
	constexpr static auto hssize = 908592;
	tileinfo *info ;
	std::unordered_map<int, collection_item> _multis ;
	
	std::string _housingbin ;
	auto processEntry(std::size_t entry, std::size_t index, std::vector<std::uint8_t> &data) ->bool final ;
	auto processHash(std::uint64_t hash,std::size_t entry , std::vector<std::uint8_t> &data) ->bool final ;
	auto processData(bool isHS, int index, std::vector<std::uint8_t> &data) ->void ;
	
public:
	multicollection(const std::filesystem::path &uodir = std::filesystem::path(),tileinfo *info = nullptr);
	
	auto load(const std::filesystem::path &uodir,tileinfo *info=nullptr) ->bool ;
	auto load(const std::filesystem::path &uodir,const std::string &housingbin,tileinfo *info = nullptr) ->bool ;
	auto loadMul(const std::filesystem::path &uodir,tileinfo *info = nullptr) ->bool;
	auto size() const ->size_t ;
	auto multis() const -> const std::unordered_map<int, collection_item>& ;
	auto multis()  ->  std::unordered_map<int, collection_item>& ;
	auto multi(std::uint16_t multid) ->collection_item& ;
	auto multi(std::uint16_t multid) const ->const collection_item&;
	auto exists(std::uint16_t multid) const ->bool ;
};

#endif /* MultiMul_hpp */
