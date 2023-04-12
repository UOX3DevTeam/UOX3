//

#ifndef uomap_hpp
#define uomap_hpp

#include <cstdint>
#include <string>
#include <tuple>
#include <unordered_map>

#include "uodata/uoxtile.hpp"

//=========================================================================================
// terrainblock
//=========================================================================================

//=========================================================================================
class terrainblock {
    const void *mapptr ;
    const TileInfo *info;
public:
    static constexpr auto size = 196 ;
    
    terrainblock(const void *dataptr,const TileInfo *info);
    auto tile(int xoffset,int yoffset,TileInfo *infoptr) -> Tile_st ;
};


//=========================================================================================
// uomap
//=========================================================================================
class uomap {
    
    int mapheight ;
    int mapwidth ;
    int mapnumber ;
    
    const void *dataptr ;
    const void *mapdiffptr ;
    int mapdiffcount ;
    std::unordered_map<int, std::unordered_map<int, Tile_st>> tiles ;

public:
    uomap(int mapnumber,int mapheight,int mapwidth) ;
    /// Calculates the block id, and the offset into that block for any x,y location
    /// - Parameters:
    ///     - x: The x coordinate on the map
    ///     - y: the y coordinate on the map
    ///     - map_height: The height of the map in cells
    /// - Returns: a tuple of the block#, the xoffset, and the y offset
    static auto blockForLocation(int x, int y, int map_height) ->std::tuple<int,int,int> ;
    /// Calculates the uop block id, and the offset into that block for any map block#
    /// - Parameters:
    ///     - x: The x coordinate on the map
    ///     - y: the y coordinate on the map
    ///     - map_height: The height of the map in cells
    /// - Returns: a pair that contains the uop block#, and the offset into that block
    static auto uopblockForBlock(int block_number) ->std::pair<int,std::uint64_t> ;
    
    //auto tile(int x,int y) -> const Tile_st& ;

};

#endif /* uomap_hpp */
