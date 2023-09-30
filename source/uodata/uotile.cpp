//

#include "uotile.hpp"

#include <algorithm>
#include <stdexcept>

#include "tileinfo.hpp"

using namespace std::string_literals ;

//======================================================================
namespace uo {
    //======================================================================
    UOTile::UOTile(tile_t type){
        this->type = type;
        this->tileid = 0 ;
        this->altitude = 0 ;
        this->hue = 0 ;
        info = static_cast<ArtInfo*>(nullptr);
        info = static_cast<TerrainInfo*>(nullptr);
    }
    //======================================================================
    UOTile::UOTile(tile_t type, std::uint16_t tileid):UOTile(type) {
        this->tileid = tileid ;
    }
    //======================================================================
    auto UOTile::isVoid() const ->bool {
        if (type == tile_t::TERRAIN){
            auto id = this->tileid ;
            auto iter = std::find_if(TERRAINVOIDS.begin(),TERRAINVOIDS.end(),[id](const std::uint16_t &entry){
                return id == entry;
            });
            return iter!=TERRAINVOIDS.end() ;
        }
        return false ;
    }
    //======================================================================
    auto UOTile::isMountain() const ->bool {
        if (type == tile_t::ART){
            return ((tileid >= 431) && (tileid <= 432)) || ((tileid >= 467) && (tileid <= 474)) || ((tileid >= 543) && (tileid <= 560)) || ((tileid >= 1754) && (tileid <= 1757)) || ((tileid >= 1787) && (tileid <= 1789)) || ((tileid >= 1821) && (tileid <= 1824)) || ((tileid >= 1851) && (tileid <= 1854)) || ((tileid >= 1881) && (tileid <= 1884));
        }
        return false ;
    }
    //======================================================================
    auto UOTile::isIgnored() const ->bool {
        if (type != tile_t::TERRAIN){
            return false ;
        }
        return ((tileid == 2) || (tileid == 0x1db) || ((tileid>=0x1ae) && (tileid<=0x1b5)));
    }
    //======================================================================
    auto UOTile::isRoad() const ->bool {
        if (type != tile_t::TERRAIN){
            return false ;
        }
        auto texture = std::get<const TerrainInfo*>(info)->textureID ;
        for (auto iter = ROADIDS.begin() ; iter != ROADIDS.end(); iter += 2){
            if (texture >= *iter && texture <= *(iter+1)){
                return true ;
            }
        }
        return false ;
    }
    
    //======================================================================
    auto UOTile::checkFlag(flag_t flagbit) const ->bool {
        if (this->type == tile_t::TERRAIN) {
            return std::get<const TerrainInfo*>(info)->checkFlag(flagbit);
        }
        return std::get<const ArtInfo*>(info)->checkFlag(flagbit);
        
    }
    //======================================================================
    auto UOTile::climbHeight() const -> int {
        if (this->type != tile_t::TERRAIN){
            std::get<const ArtInfo*>(info)->climbHeight();
        }
        return 0;
    }
    //======================================================================
    auto UOTile::top() const ->int {
        auto rvalue = this->altitude ;
        if (this->type != tile_t::TERRAIN){
            auto temp = std::get<const ArtInfo*>(info)->height;
            rvalue += static_cast<int>((temp & 0x8 ? (temp & 0xF) >> 1 : temp & 0xF));
        }
        return rvalue ;
    }
    //======================================================================
    auto UOTile::name() const ->const std::string&{
        if (this->type == tile_t::TERRAIN) {
            return std::get<const TerrainInfo*>(info)->name;
        }
        return std::get<const ArtInfo*>(info)->name;
        
    }
    //======================================================================
    auto UOTile::height() const -> int {
        if (this->type != tile_t::TERRAIN) {
            return static_cast<int>(std::get<const ArtInfo*>(info)->height);
        }
        return 0;
    }
    
}
