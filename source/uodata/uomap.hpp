//

#ifndef uomap_hpp
#define uomap_hpp

#include <cstdint>
#include <iostream>
#include <string>

#include "artmap.hpp"
#include "terrainmap.hpp"


//======================================================================
namespace uo {
    struct TileInfo;
    
    class UOMap {
        ArtMap artData ;
        TerrainMap terrainData ;
        const TileInfo * info ;
        bool valid ;
    public:
        UOMap(TileInfo *info=nullptr);
        
        auto setInfo(const TileInfo *info) -> void ;
        auto size() const -> std::pair<int,int> ;
        auto validLocation(int x, int y) const ->bool ;
        auto isValid() const ->bool {return this->valid;}
        auto setValid(bool state) ->void {valid = state;}
        // Terrain
        auto loadTerrain(int mapNumber,const std::filesystem::path &terrainpath,bool log , int mapHeight,int mapWidth) ->bool ;
        auto loadTerrainDiff(const std::filesystem::path &diffl, const std::filesystem::path diff) -> bool ;
        auto sizeTerrainDiff() const ->size_t ;
        auto tileTerrainFor(int x, int y) const -> UOTile ;
        
        // Art
        auto loadArt(const std::filesystem::path &idxpath, const std::filesystem::path &mulpath,int mapHeight,int mapNumber) ->bool ;
        auto loadArtDiff(const std::filesystem::path &difflpath, const std::filesystem::path &diffipath, const std::filesystem::path &diffpath)->bool;
        auto sizeArtDiff() const ->size_t {return artData.sizeDiff();}
        auto tilesAt(int x, int y) const -> const std::vector<UOTile>& ;
        
    };
}

#endif /* uomap_hpp */
