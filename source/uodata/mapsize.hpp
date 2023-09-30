//

#ifndef mapsize_hpp
#define mapsize_hpp

#include <array>
#include <cstdint>
#include <iostream>
#include <string>
#include <tuple>
#include <utility>
//======================================================================
namespace uo {
    //======================================================================
    /**
     Holds the size of the map in terms of cells
     */
    struct MapSize{
        static constexpr auto MAPCOUNT = 6 ;
        static const std::array<MapSize, MAPCOUNT> MAPSIZE;
        static const std::array<std::string,MAPCOUNT> MAPNAME ;
        static auto blockFor(int x, int y, int mapHeight) -> std::tuple<int,int,int> ;
        static auto coordForBlock(int block, int mapHeight) -> std::pair<int,int> ;
        static auto uopBlockFor(int block)->std::pair<int,int> ;
        
        int width;
        int height;
        MapSize(int width = 0 , int height = 0): width(width),height(height){
        }
        auto describe() const -> std::string {
            return std::to_string(width)+std::string(",")+std::to_string(height);
        }
    };
    
}

#endif /* mapsize_hpp */
