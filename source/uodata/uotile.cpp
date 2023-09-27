//

#include "uotile.hpp"

#include <algorithm>
#include <stdexcept>


using namespace std::string_literals ;

//======================================================================
namespace uo {
    UOTile::UOTile(tile_t type){
        this->type = type;
        this->tileid = 0 ;
        this->altitude = 0 ;
        this->hue = 0 ;
        info = static_cast<ArtInfo*>(nullptr);
        info = static_cast<TerrainInfo*>(nullptr);
    }
    UOTile::UOTile(tile_t type, std::uint16_t tileid):UOTile(type) {
        this->tileid = tileid ;
    }
}
