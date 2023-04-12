//

#include "uomap.hpp"

#include <iostream>
#include <stdexcept>
#include <algorithm>
#include "uoxtile.hpp"

using namespace std::string_literals ;

//=========================================================================================
// terrainblock
//=========================================================================================

//=========================================================================================
terrainblock::terrainblock(const void *dataptr,const TileInfo *infoptr):mapptr(dataptr),info(infoptr){
    
}
//=========================================================================================
auto terrainblock::tile(int xoffset,int yoffset,TileInfo *info) -> Tile_st {
    auto offset = 4 + (yoffset*24) + (xoffset*3) ;
    
    auto tile = Tile_st(terrain) ;
    if (mapptr){
        std::copy(static_cast<const std::uint8_t*>(mapptr)+offset,static_cast<const std::uint8_t*>(mapptr)+offset+2,reinterpret_cast<std::uint8_t*>(&tile.tileId));
        std::copy(static_cast<const std::uint8_t*>(mapptr)+offset+2,static_cast<const std::uint8_t*>(mapptr)+offset+3,reinterpret_cast<std::uint8_t*>(&tile.altitude));
        if (info != nullptr){
            tile.terrainInfo = &info->TerrainInfo(tile.tileId);
        }
    }
    return tile ;
}


