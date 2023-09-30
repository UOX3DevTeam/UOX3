//

#include "mapsize.hpp"

#include <algorithm>
#include <stdexcept>


using namespace std::string_literals ;

//======================================================================
//======================================================================
namespace uo {
    //======================================================================
    const std::array<MapSize, MapSize::MAPCOUNT> MapSize::MAPSIZE{MapSize(7168,4096),MapSize(7168,4096),MapSize(2304,1600),MapSize(2560,2048),MapSize(1448,1448),MapSize(1280,4096)};
    const std::array<std::string,MapSize::MAPCOUNT> MAPNAME{"Felucca"s,"Trammel"s,"Ilshenar"s,"Malas"s,"Tokuno"s,"TerMur"s} ;
    
    //======================================================================
    auto MapSize::blockFor(int x, int y, int mapHeight) -> std::tuple<int,int,int> {
        auto xblock = x/8 ;
        auto yblock = y/8 ;
        auto block = (xblock * (mapHeight/8) + yblock);
        auto [xloc,yloc] = coordForBlock(block, mapHeight);
        return std::make_tuple(block,x-xloc,y-yloc) ;
    }
    //======================================================================
    auto MapSize::coordForBlock(int block, int mapHeight) -> std::pair<int,int> {
        auto x = (block / (mapHeight/8)) * 8 ;
        auto y = block % (mapHeight/8) * 8 ;
        return std::make_pair(x,y) ;
    }
    //======================================================================
    auto MapSize::uopBlockFor(int block)->std::pair<int,int> {
        constexpr auto byteOffsetForUOP = 0xC4000 ;
        constexpr auto blockSize = 196 ;
        constexpr auto blocksInUOP = byteOffsetForUOP/blockSize ;
        
        auto uopBlock = block / blocksInUOP ;
        auto offset = (block % blocksInUOP) * blockSize ;
        return std::make_pair(uopBlock, offset) ;
    }
    
    
}
