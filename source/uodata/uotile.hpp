//

#ifndef uotile_hpp
#define uotile_hpp

#include <array>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

#include "uoflag.hpp"

//=====================================================================
namespace uo {
    
    struct ArtInfo;
    struct TerrainInfo ;
    
    class UODataError:public std::runtime_error{
    public:
        UODataError(const std::string &reason):std::runtime_error(reason){}
    };
    
    enum tile_t{
        TERRAIN,ART,DYNAMIC
    };
    struct UOTile {
        tile_t type ;
        std::uint16_t tileid ;
        int altitude ;
        std::uint16_t hue ;  // This is for static art tiles only ;
        std::variant<const ArtInfo*,const TerrainInfo*> info ;
        static constexpr std::array<std::uint16_t,18> ROADIDS {
            0x0009, 0x0015, // furrows
            0x0071, 0x0078, // dirt
            0x00E8, 0x00EB, // dirt
            0x0150, 0x015C, // furrows
            0x0442, 0x0479, // sand stone
            0x0501, 0x0510, // sand stone
            0x07AE, 0x07B1, // dirt
            0x3FF4, 0x3FF4, // dirt
            0x3FF8, 0x3FFB  // dirt
        };
        UOTile(tile_t type);
        UOTile(tile_t type,std::uint16_t tileid);
        
        // UOX3 extensions
        static constexpr std::array<std::uint16_t, 11> TERRAINVOIDS{430, 431, 432, 433, 434, 475, 580, 610, 611, 612, 613};
        
        auto isVoid() const ->bool ;
        auto isMountain() const ->bool ;
        auto isIgnored() const ->bool ;
        auto isRoad() const ->bool ;
        auto checkFlag(flag_t flagbit) const ->bool ;
        auto climbHeight() const -> int ;
        auto top() const ->int ;
        auto name() const -> const std::string& ;
        auto height() const -> int ;
    };
}

#endif /* uotile_hpp */
