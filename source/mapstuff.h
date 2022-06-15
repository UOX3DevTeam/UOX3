#ifndef __MAPSTUFF_H__
#define __MAPSTUFF_H__

#include "mapclasses.h"
#include <climits>
#include "uox3.h"
#include "MultiMul.hpp"

constexpr auto MAX_Z_STEP = std::uint8_t(9);
constexpr auto MAX_Z_FALL = std::uint8_t(20);
//===============================================================================================
// tileinfo 
//===============================================================================================
//===============================================================================================
class tileinfo {
	constexpr static auto hssize = 3188736;
	std::vector<CLand > terrain_data ;
	std::vector<CTile> art_data;
	auto processTerrain(std::ifstream &input) ->void ;
	auto processArt(std::ifstream &input) ->void ;
	bool isHS_format ;
public:
	tileinfo(const std::string &filename = "");
	auto load(const std::string &filename) ->bool ;
	auto terrain(std::uint16_t tileid) const ->const CLand &;
	auto terrain(std::uint16_t tileid) ->CLand & ;
	auto art(std::uint16_t tileid) ->CTile &;
	auto art(std::uint16_t tileid) const ->const CTile & ;
	
	auto sizeTerrain() const -> size_t ;
	auto sizeArt() const -> size_t ;
	
	auto collectionTerrain() const -> const std::vector<CLand>&;
	auto collectionTerrain() ->std::vector<CLand> &;
	auto collectionArt() const -> const std::vector<CTile>& ;
	auto collectionArt() ->std::vector<CTile>& ;
	auto totalMemory() const ->size_t ;
	
};

//================================================================================================================
// Blocks and maps would normally be in separate files, but since we dont want want to impact windows project files, we
// will use the existing files to include them

class terrainblock {
	std::array<std::array<tile_t,8>,8> _tiles ;
	
public:
	terrainblock(std::uint8_t *data = nullptr,const tileinfo *info = nullptr);
	auto load(std::uint8_t *data,const tileinfo *info = nullptr) ->void ;
	auto tile(int x, int y) ->tile_t& ;
	auto tile(int x, int y) const ->const tile_t& ;
};

//=========================================================
class artblock {
	std::array<std::array<std::vector<tile_t>,8>,8> _tiles ;
	
public:
	artblock(int length=0,std::uint8_t *data = nullptr,const tileinfo *info = nullptr);
	auto load(int length,std::uint8_t *data,const tileinfo *info = nullptr) ->void ;
	auto load(int length,std::istream &input,const tileinfo *info = nullptr) ->void ;
	auto tile(int x, int y) ->std::vector<tile_t>& ;
	auto tile(int x, int y) const ->const std::vector<tile_t>& ;
	auto clear() ->void ;
};
//=========================================================
class ultimamap : public uopfile {
	std::vector<terrainblock> _terrain ;
	std::vector<artblock> _art;
	
	const tileinfo *tile_info ;
	static constexpr int _totalmaps = 6 ;
	static constexpr std::array<std::pair<int,int>,_totalmaps> _mapsizes{{
		{7168,4096},{7168,4096},{2304,1600},
		{2560,2048},{1448,1448},{1280,4096}
	}};
	int _mapnum ;
	int _width ;
	int _height ;
	bool isUOP ;
	int _diffcount ;
	int _diffterrain ;
	auto processEntry(std::size_t entry, std::size_t index, std::vector<std::uint8_t> &data) ->bool final ;
	auto calcBlock(int x,int y) const ->int ;
	auto calcXYOffset(int block) const ->std::pair<int,int> ;
	auto loadTerrainBlock(int blocknum,std::uint8_t *data)->void ;
public:
	ultimamap() ;
	ultimamap(int mapnum,int width=0, int height = 0,const tileinfo *info = nullptr);
	auto width() const ->int ;
	auto height() const ->int ;
	auto diffArt() const ->int ;
	auto diffTerrain() const ->int ;
	auto setSize(int width, int height) ->void ;
	auto size() const ->std::pair<int,int> ;
	auto loadUOPTerrainFile(const std::string &filename) ->bool ;
	auto loadMulTerrainFile(const std::string &filename) ->bool ;
	auto loadArt(const std::string &mulfile, const std::string &idxfile)->bool ;
	auto applyDiff(const std::string &difflpath,const std::string &diffipath, const std::string &diffpath) ->int ;
	auto applyTerrainDiff(const std::string &difflpath,const std::string &diffpath) ->int ;
	
	auto blockAndIndexFor(int x, int y) const ->std::tuple<int, int, int>;
	auto uop()const ->bool { return isUOP;}
	
	auto terrain(int x, int y) const ->const tile_t& ;
	auto terrain(int x, int y)  ->tile_t& ;
	auto art(int x, int y) const ->const std::vector<tile_t>& ;
	auto art(int x, int y)  ->std::vector<tile_t>& ;
};



//====================================================================================================
struct mapdfndata_st{
	int width;
	int height;
	std::filesystem::path mapfile ;
	std::filesystem::path mapuop ;
	std::filesystem::path stamul ;
	std::filesystem::path staidx ;
	std::filesystem::path mapdiff ;
	std::filesystem::path mapdiffl ;
	std::filesystem::path stadiff ;
	std::filesystem::path stadiffi;
	std::filesystem::path stadiffl ;
	mapdfndata_st():width(0),height(0){}
};

//==========================================================================================
// until we can replace
//==========================================================================================
class CMulHandler{
private:
	// uo eq5q
	tileinfo tile_info ;
	multicollection multi_data ;
	std::unordered_map<int, ultimamap> uoworlds ;
	
	
	auto loadMapsDFN(const std::string &uodir) ->std::map<int,mapdfndata_st> ;
	auto loadDFNOverrides() ->void ;
	auto loadTileData(const std::string &uodir) ->void;
	auto loadMultis(const std::string &uodir) ->void ;
	auto loadMapAndStatics(const std::map<int,mapdfndata_st> &info) ->void;

public:
	CMulHandler() = default ;
	auto load() ->void ;
	auto artAt(std::int16_t x, std::int16_t y, std::uint8_t world) ->std::vector<tile_t>& ;
	auto artAt(std::int16_t x, std::int16_t y, std::uint8_t world) const -> const std::vector<tile_t>& ;
	auto sizeOfMap(std::uint8_t worldNumber) const ->std::pair<int,int> ;
	auto diffCountForMap(std::uint8_t worldNumber) const ->std::pair<int,int> ;

	auto MultiHeight(CItem *i, std::int16_t x, std::int16_t y, std::int8_t oldZ, std::int8_t maxZ, bool checkHeight = false) ->std::int8_t;
	auto MultiTile(CItem *i, std::int16_t x, std::int16_t y, std::int8_t oldz, bool checkVisible = true) ->std::uint16_t;
	
	auto DoesStaticBlock(std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, bool checkWater = false) ->bool;
	auto DoesDynamicBlock(std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::uint16_t realm, bool checkWater, bool waterWalk, bool checkOnlyMultis, bool checkOnlyNonMultis) ->bool;
	auto DynTile( std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::uint16_t realm, bool checkOnlyMultis, bool checkOnlyNonMultis ) ->CItem *;
	auto DoesMapBlock(std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, bool checkWater, bool waterWalk, bool checkMultiPlacement, bool checkForRoad) ->bool;
	auto CheckStaticFlag(std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, TileFlags toCheck, bool checkSpawnSurface = false) ->bool;
	auto CheckDynamicFlag(std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::uint16_t realm, TileFlags toCheck) ->bool;
	auto CheckTileFlag(std::uint16_t itemID, TileFlags flagToCheck) ->bool;
	
	// height functions
	auto StaticTop(std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::int8_t maxZ) ->std::int8_t;
	auto DynamicElevation(std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::int8_t maxZ, std::uint16_t realm) ->std::int8_t;
	auto MapElevation(std::int16_t x, std::int16_t y, std::uint8_t worldNumber) ->std::int8_t;
	auto TileHeight(std::uint16_t tilenum) ->std::int8_t;
	auto Height(std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber,  std::uint16_t realm) ->std::int8_t;
	auto inBuilding( std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::uint16_t realm) ->bool;
	auto IsIgnored(std::uint16_t landnum) ->bool {
		if (landnum == 2 || landnum == 0x1DB || ( landnum >= 0x1AE && landnum <= 0x1B5 )){
			return true;
		}
		else{
			return false;
		}
	}
	// look at tile functions
	auto MultiArea(CMultiObj *i, std::int16_t &x1, std::int16_t &y1, std::int16_t &x2, std::int16_t &y2) ->void ;
	auto multiExists(std::uint16_t multinum) const ->bool;
	
	auto SeekMulti( std::uint16_t multinum ) const  -> const collection_item &;
	auto IsValidTile(std::uint16_t tileNum) const ->bool;
	auto SeekTile(std::uint16_t tileNum) ->CTile&;
	auto SeekTile(std::uint16_t tileNum) const ->const CTile&;
	auto SeekLand(std::uint16_t landNum) ->CLand&;
	auto SeekLand(std::uint16_t landNum) const -> const CLand&;
	auto SeekMap(std::int16_t x, std::int16_t y, std::uint8_t worldNumber) ->tile_t&;
	auto SeekMap(std::int16_t x, std::int16_t y, std::uint8_t worldNumber) const ->const tile_t&;

	// misc functions
	auto ValidSpawnLocation(std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::uint16_t realm, bool checkWater = true) ->bool;
	auto ValidMultiLocation(std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::uint16_t realm, bool checkWater,
							 bool checkOnlyOtherMultis, bool checkOnlyNonMultis, bool checkForRoads) ->std::uint8_t;
	auto MapExists(std::uint8_t worldNumber ) const ->bool;
	auto InsideValidWorld(std::int16_t x, std::int16_t y, std::uint8_t worldNumber = 0xFF) const ->bool;
	auto MapCount() const ->std::uint8_t;
};

extern CMulHandler *Map;
#endif

