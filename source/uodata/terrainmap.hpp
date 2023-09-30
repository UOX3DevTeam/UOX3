//

#ifndef terrainmap_hpp
#define terrainmap_hpp

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <map>
#include <string>

#include "utility/filemap.hpp"
#include "uotile.hpp"

namespace uo{
    
    struct TileInfo;

    //======================================================================
    //  TerrainDiff
    //======================================================================
    
    //======================================================================
    class TerrainDiff {
        std::map<int, std::uint64_t> diffEntry ;
        util::filemap_t diffmap ;
        std::uint8_t *data ;
    public:
        TerrainDiff() ;
        auto load(const std::filesystem::path &diffList, const std::filesystem::path &diff) ->bool ;
        auto size() const -> size_t ;
        auto dataForBlock(int block) const -> const std::uint8_t* ;
    };
    
    //======================================================================
    //  TerrainMap
    //======================================================================
    class TerrainMap {
        util::filemap_t datamap ;
        std::uint8_t *data ;
        TerrainDiff diff ;
        const TileInfo *info ;
        bool valid ;
        static const std::string hashformat ;
        std::map<int,std::uint64_t> uopOffsets ;
        int mapHeight  ;
        int mapWidth ;
        auto dataForBlock(int block) const -> const std::uint8_t * ;

    public:
        TerrainMap() ;
        auto load(const std::filesystem::path &mappath,int mapNumber,int mapHeight=0,int mapWidth=0 ) ->bool ;
        auto loadDiff(const std::filesystem::path &diffl, const std::filesystem::path diff) -> bool ;
        auto sizeDiff() const ->size_t ;
        auto setInfo(const TileInfo *info) -> void ;
        auto tileFor(int x, int y) const -> UOTile ;
        auto size() const ->std::pair<int,int> ;
    };

    
}
#endif /* terrainmap_hpp */
