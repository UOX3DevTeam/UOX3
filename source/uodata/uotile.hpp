//

#ifndef uotile_hpp
#define uotile_hpp

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

//======================================================================
namespace uo {
    struct ArtInfo;
    struct TerrainInfo ;
    
    class UODataError:public std::runtime_error{
    public:
        UODataError(const std::string &reason):std::runtime_error(reason){}
    };
    
    enum tile_t{
        TERRAIN,ART
    };
    struct UOTile {
        tile_t type ;
        std::uint16_t tileid ;
        int altitude ;
        std::uint16_t hue ;  // This is for static art tiles only ;
        std::variant<const ArtInfo*,const TerrainInfo*> info ;
        UOTile(tile_t type);
        UOTile(tile_t type,std::uint16_t tileid);
    };
}

#endif /* uotile_hpp */
