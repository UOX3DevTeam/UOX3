//

#include "uomap.hpp"

#include <algorithm>
#include <stdexcept>


using namespace std::string_literals ;

//======================================================================
namespace uo {
    //======================================================================
    UOMap::UOMap(TileInfo *info) {
        this->info = info ;
    }
    //======================================================================
    auto UOMap::setInfo(const TileInfo *info) -> void {
        terrainData.setInfo(info);
        artData.setTileInfo(info);
        this->info = info ;
    }

    //======================================================================
    auto UOMap::loadTerrain(int mapNumber,const std::filesystem::path &terrainpath,bool log , int mapHeight,int mapWidth) ->bool {
        return terrainData.load(terrainpath, mapNumber,mapHeight,mapWidth);
    }
    //======================================================================
    auto UOMap::loadTerrainDiff(const std::filesystem::path &diffl, const std::filesystem::path diff) -> bool {
        return terrainData.loadDiff(diffl, diff);
    }
    //======================================================================
    auto UOMap::sizeTerrainDiff() const ->size_t {
        return terrainData.sizeDiff();
    }
    //======================================================================
    auto UOMap::tileTerrainFor(int x, int y) const -> UOTile {
        return terrainData.tileFor(x, y);
    }
    
    // Art
    //======================================================================
    auto UOMap::loadArt(const std::filesystem::path &idxpath, const std::filesystem::path &mulpath,int mapHeight,int mapNumber) ->bool {
        return artData.load(idxpath, mulpath,mapHeight,mapNumber) ;
    }
    //======================================================================
    auto UOMap::loadArtDiff(const std::filesystem::path &difflpath, const std::filesystem::path &diffipath, const std::filesystem::path &diffpath)->bool{
        return artData.loadDiff(difflpath, diffipath, diffpath);
    }
    //======================================================================
    auto UOMap::tilesAt(int x, int y) const -> const std::vector<UOTile>& {
        return artData.tilesAt(x, y);
    }

}
