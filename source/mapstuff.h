#ifndef __MAPSTUFF_H__
#define __MAPSTUFF_H__

#include "mapclasses.h"
#include <climits>
#include "uox3.h"
#include "MultiMul.hpp"

constexpr auto MAX_Z_STEP = std::uint8_t( 9 );
constexpr auto MAX_Z_FALL = std::uint8_t( 20 );
//===============================================================================================
// TileInfo 
//===============================================================================================
class TileInfo
{
	constexpr static auto hsSize = 3188736;
	std::vector<CLand> terrainData;
	std::vector<CTile> artData;
	auto ProcessTerrain( std::ifstream &input ) -> void;
	auto ProcessArt( std::ifstream &input ) -> void;
	bool isHsFormat;
public:
	TileInfo( const std::string &filename = "" );
	auto LoadTiles( const std::string &filename ) -> bool;
	auto TerrainInfo( std::uint16_t tileId ) const -> const CLand&;
	auto TerrainInfo( std::uint16_t tileId ) -> CLand&;
	auto ArtInfo( std::uint16_t tileId ) -> CTile&;
	auto ArtInfo( std::uint16_t tileId ) const -> const CTile&;

	auto SizeTerrain() const -> size_t;
	auto SizeArt() const -> size_t;

	auto CollectionTerrain() const -> const std::vector<CLand>&;
	auto CollectionTerrain() -> std::vector<CLand>&;
	auto CollectionArt() const -> const std::vector<CTile>&;
	auto CollectionArt() -> std::vector<CTile>&;
	auto TotalMemory() const -> size_t;
};

//==================================================================================================
// Blocks and maps would normally be in separate files, but since we dont want to impact windows
// project files, we will use the existing files to include them
class TerrainBlock
{
	std::array<std::array<Tile_st, 8>, 8> _tiles;
	
public:
	TerrainBlock( std::uint8_t *data = nullptr, const TileInfo *info = nullptr );
	auto LoadBlock( std::uint8_t *data, const TileInfo *info = nullptr ) -> void;
	auto TerrainTileAt( int x, int y ) -> Tile_st&;
	auto TerrainTileAt( int x, int y ) const -> const Tile_st&;
};

//=========================================================
class ArtBlock
{
	std::array<std::array<std::vector<Tile_st>, 8>, 8> _tiles;
	
public:
	ArtBlock( int length = 0, std::uint8_t *data = nullptr, const TileInfo *info = nullptr );
	auto LoadArtBlock( int length, std::uint8_t *data, const TileInfo *info = nullptr ) -> void;
	auto LoadArtBlock( int length, std::istream &input, const TileInfo *info = nullptr ) -> void;
	auto ArtTileAt( int x, int y ) -> std::vector<Tile_st>&;
	auto ArtTileAt( int x, int y ) const -> const std::vector<Tile_st>&;
	auto Clear() -> void;
};
//=========================================================
class UltimaMap : public UopFile
{
	std::vector<TerrainBlock> _terrain;
	std::vector<ArtBlock> _art;
	
	const TileInfo *tileInfo;
	static constexpr int _totalMaps = 6;
	static constexpr std::array<std::pair<int, int>, _totalMaps> _mapSizes {{
		{7168, 4096}, {7168, 4096}, {2304, 1600},
		{2560, 2048}, {1448, 1448}, {1280, 4096}
	}};
	int _mapNum;
	int _width;
	int _height;
	bool isUop;
	int _diffCount;
	int _diffTerrain;
	auto virtual ProcessEntry( std::size_t entry, std::size_t index, std::vector<std::uint8_t> &data ) -> bool final;
	auto CalcBlock (int x, int y ) const -> int;
	auto CalcXYOffset( int block ) const -> std::pair<int, int>;
	auto LoadTerrainBlock( int blockNum, std::uint8_t *data ) -> void;
public:
	UltimaMap();
	UltimaMap( int mapNum, int width = 0, int height = 0, const TileInfo *info = nullptr );
	auto Width() const -> int;
	auto Height() const -> int;
	auto DiffArt() const -> int;
	auto DiffTerrain() const -> int;
	auto SetSize( int width, int height ) -> void;
	auto Size() const -> std::pair<int, int>;
	auto LoadUOPTerrainFile( const std::string &fileName) -> bool;
	auto LoadMulTerrainFile( const std::string &fileName) -> bool;
	auto LoadArt( const std::string &mulFile, const std::string &idxFile ) -> bool;
	auto ApplyDiff( const std::string &difflPath, const std::string &diffiPath, const std::string &diffPath ) -> int;
	auto ApplyTerrainDiff( const std::string &difflPath, const std::string &diffPath ) -> int;
	
	auto BlockAndIndexFor( int x, int y ) const -> std::tuple<int, int, int>;
	auto Uop() const -> bool { return isUop; }
	
	auto TerrainAt( int x, int y ) const -> const Tile_st&;
	auto TerrainAt( int x, int y ) -> Tile_st&;
	auto ArtAt( int x, int y ) const -> const std::vector<Tile_st>&;
	auto ArtAt( int x, int y ) -> std::vector<Tile_st>&;
};

//====================================================================================================
struct MapDfnData_st
{
	int width;
	int height;
	std::filesystem::path mapFile;
	std::filesystem::path mapUop;
	std::filesystem::path staMul;
	std::filesystem::path staIdx;
	std::filesystem::path mapDiff;
	std::filesystem::path mapDiffl;
	std::filesystem::path staDiff;
	std::filesystem::path staDiffi;
	std::filesystem::path staDiffl;
	MapDfnData_st() : width( 0 ), height( 0 ) {}
};

//==========================================================================================
// until we can replace
//==========================================================================================
class CMulHandler
{
private:
	// uo eq5q
	TileInfo tileInfo;
	MultiCollection multiData;
	std::unordered_map<int, UltimaMap> uoWorlds;
	
	auto LoadMapsDFN( const std::string &uoDir ) -> std::map<int, MapDfnData_st>;
	auto LoadDFNOverrides() -> void;
	auto LoadTileData( const std::string &uoDir ) -> void;
	auto LoadMultis( const std::string &uoDir ) -> void;
	auto LoadMapAndStatics( const std::map<int, MapDfnData_st> &info ) -> void;

public:
	CMulHandler() = default;
	auto Load() -> void;
	auto ArtAt( std::int16_t x, std::int16_t y, std::uint8_t world ) -> std::vector<Tile_st>&;
	auto ArtAt( std::int16_t x, std::int16_t y, std::uint8_t world ) const -> const std::vector<Tile_st>&;
	auto SizeOfMap( std::uint8_t worldNumber ) const -> std::pair<int, int>;
	auto DiffCountForMap( std::uint8_t worldNumber ) const -> std::pair<int, int>;

	auto MultiHeight( CItem *i, std::int16_t x, std::int16_t y, std::int8_t oldZ, std::int8_t maxZ, bool checkHeight = false ) -> std::int8_t;
	auto MultiTile( CItem *i, std::int16_t x, std::int16_t y, std::int8_t oldZ, bool checkVisible = true ) -> std::uint16_t;
	
	auto DoesStaticBlock( std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, bool checkWater = false ) -> bool;
	auto DoesDynamicBlock( std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::uint16_t instanceId, bool checkWater, bool waterWalk, bool checkOnlyMultis, bool checkOnlyNonMultis ) -> bool;
	auto DynTile( std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::uint16_t instanceId, bool checkOnlyMultis, bool checkOnlyNonMultis ) -> CItem *;
	auto DoesMapBlock( std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, bool checkWater, bool waterWalk, bool checkMultiPlacement, bool checkForRoad ) -> bool;
	auto CheckStaticFlag( std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, TileFlags toCheck, bool checkSpawnSurface = false ) -> bool;
	auto CheckDynamicFlag( std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::uint16_t instanceId, TileFlags toCheck ) -> bool;
	auto CheckTileFlag( std::uint16_t itemId, TileFlags flagToCheck) -> bool;
	
	// height functions
	auto StaticTop( std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::int8_t maxZ ) -> std::int8_t;
	auto DynamicElevation( std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::uint16_t instanceId, std::int8_t maxZ ) -> std::int8_t;
	auto MapElevation( std::int16_t x, std::int16_t y, std::uint8_t worldNumber ) -> std::int8_t;
	auto TileHeight( std::uint16_t tileNum ) -> std::int8_t;
	auto Height( std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::uint16_t instanceId ) -> std::int8_t;
	auto InBuilding( std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::uint16_t instanceId ) -> bool;
	auto IsIgnored( std::uint16_t landNum ) -> bool
	{
		if( landNum == 2 || landNum == 0x1DB || ( landNum >= 0x1AE && landNum <= 0x1B5 ))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	// look at tile functions
	auto MultiArea( CMultiObj *i, std::int16_t &x1, std::int16_t &y1, std::int16_t &x2, std::int16_t &y2 ) -> void;
	auto MultiExists( std::uint16_t multiNum ) const -> bool;
	
	auto SeekMulti( std::uint16_t multiNum ) const  -> const CollectionItem_st &;
	auto IsValidTile( std::uint16_t tileNum ) const -> bool;
	auto SeekTile( std::uint16_t tileNum ) ->CTile&;
	auto SeekTile( std::uint16_t tileNum ) const -> const CTile&;
	auto SeekLand( std::uint16_t landNum ) -> CLand&;
	auto SeekLand( std::uint16_t landNum ) const -> const CLand&;
	auto SeekMap( std::int16_t x, std::int16_t y, std::uint8_t worldNumber ) -> Tile_st&;
	auto SeekMap( std::int16_t x, std::int16_t y, std::uint8_t worldNumber ) const -> const Tile_st&;

	// misc functions
	auto ValidSpawnLocation( std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::uint16_t instanceId, bool checkWater = true ) -> bool;
	auto ValidMultiLocation( std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::uint16_t instanceId, bool checkWater,
							 bool checkOnlyOtherMultis, bool checkOnlyNonMultis, bool checkForRoads ) -> std::uint8_t;
	auto MapExists( std::uint8_t worldNumber ) const -> bool;
	auto InsideValidWorld( std::int16_t x, std::int16_t y, std::uint8_t worldNumber = 0xFF ) const -> bool;
	auto MapCount() const -> std::uint8_t;
};

extern CMulHandler *Map;
#endif

