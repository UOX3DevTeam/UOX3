// 
// Created on:  4/30/21

#ifndef MultiMul_hpp
#define MultiMul_hpp

#include <cstdint>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include "UOPData.hpp"

//====================================================================
struct multi_component {
	unsigned short tileid ;
	short xoffset ;
	short yoffset ;
	short zoffset ;
	unsigned int visible ;
};

//====================================================================
class multi_structure {
	int _id ;
	std::map<int,std::vector<multi_component> > _items ;
	int _minX ;
	int _maxX ;
	int _minY ;
	int _maxY ;
	friend class MultiMul ;
public:
	multi_structure() { _id = 0xFFFFFFFF; _minX=0; _maxX = 0 ; _minY = 0; _maxY=0;}
	int id() const { return _id; }
	void setId(int value) { _id = value ;}
	int minx(){ return _minX;}
	int miny(){ return _minY;}
	int maxx(){ return _maxX;}
	int maxy(){ return _maxY;}
	std::size_t memoryComponent() const ;
	
	int formLookup(short xoffset, short yoffset) const{
		return ((xoffset << 16) || yoffset);
	}
	
	void addItem( const multi_component &item) {
		auto lookup = formLookup(item.xoffset, item.yoffset) ;
		auto loc = _items.find(lookup);
		if (loc != _items.end()) {
			loc->second.push_back(item);
			/*
			 sort(loc->second.begin(),loc->second.end(), [] (multi_component lhs, multi_component rhs) {
			 return lhs.zoffset < rhs.zoffset ;
			 });
			 */
		}
		else {
			std::vector<multi_component> temp ;
			temp.push_back(item);
			_items.insert_or_assign(lookup, temp);
		}
	}
	
	std::vector<multi_component> itemsAt(int xoffset, int yoffset) const {
		auto lookup = formLookup(static_cast<short>(xoffset), static_cast<short>(yoffset));
		auto loc = _items.find(lookup) ;
		if (loc != _items.end()) {
			return loc->second ;
		}
		else {
			return std::vector<multi_component>() ;
		}
	}
	
	std::vector<multi_component> itemsAt(int centerx, int centery, int curx, int cury) const {
		auto offsetx = curx - centerx ;
		auto offsety = cury - centery ;
		return itemsAt(offsetx,offsety);
	}
	std::vector<multi_component> allItems() const {
		std::vector<multi_component> result ;
		for (auto &iter : _items) {
			result.insert(result.end(),iter.second.begin(),iter.second.end());
		}
		return result;
	}
};

//====================================================================

class MultiMul : public UO::UOPData{
protected:
	static constexpr std::size_t hssize = 908592;
	static const std::string _uopformat ;
	std::map<int, multi_structure> _entries ;

	
public:
	MultiMul() ;
	MultiMul(const std::string &mulpath, const std::string &idxpath);
	MultiMul(const std::string &uoppath);
	void load(const std::string &mulpath, const std::string &idxpath);
	void load(const std::string &uoppath);
	void processData(std::vector<unsigned char> &data, std::uint32_t chunkid) override ;
	const multi_structure& entry(int id) const ;
	std::size_t amount() const ;
	bool exists(int id) ;
	std::size_t memoryMulti() const ;
	std::size_t size() const ;
};


//=========================================================================================================
// Everything above should be able to be deleted

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
	
	std::unordered_map<int, collection_item> _multis ;
	
	std::string _housingbin ;
	auto processEntry(std::size_t entry, std::size_t index, std::vector<std::uint8_t> &data) ->bool final ;
	auto processHash(std::uint64_t hash,std::size_t entry , std::vector<std::uint8_t> &data) ->bool final ;
	auto processData(bool isHS, int index, std::vector<std::uint8_t> &data) ->void ;
	
public:
	multicollection(const std::filesystem::path &uodir = std::filesystem::path());
	
	auto load(const std::filesystem::path &uodir) ->bool ;
	auto load(const std::filesystem::path &uodir,const std::string &housingbin) ->bool ;
	auto loadMul(const std::filesystem::path &uodir) ->bool;
	auto size() const ->size_t ;
	auto multis() const -> const std::unordered_map<int, collection_item>& ;
	auto multis()  ->  std::unordered_map<int, collection_item>& ;
	auto multi(std::uint16_t multid) ->collection_item& ;
	auto multi(std::uint16_t multid) const ->const collection_item&;
	auto exists(std::uint16_t multid) const ->bool ;
};

#endif /* MultiMul_hpp */
