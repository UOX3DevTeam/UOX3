//

#include "uomap.hpp"

#include <algorithm>
#include <stdexcept>

#include "tileinfo.hpp"
using namespace std::string_literals ;

//======================================================================
namespace uo {
    //======================================================================
    UOMap::UOMap(TileInfo *info):valid(false) {
        this->setInfo(info) ;
    }
    //======================================================================
    auto UOMap::setInfo(const TileInfo *info) -> void {
        terrainData.setInfo(info);
        artData.setTileInfo(info);
        this->info = info ;
    }
    //======================================================================
    auto UOMap::size() const ->std::pair<int,int> {
        return terrainData.size() ;
    }
    //======================================================================
    auto UOMap::validLocation(int x, int y) const ->bool {
        if (isValid()){
            auto [width,height] = this->size() ;
            return (x>= 0 && x< width  && y>=0 && y<height);
        }
        return false ;
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
        if (isValid()){
            return terrainData.tileFor(x, y);
        }
        throw std::runtime_error("Accessing invalid map for terrain data");
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
