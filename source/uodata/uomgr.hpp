//

#ifndef uomgr_hpp
#define uomgr_hpp

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <map>
#include <string>

#include "tileinfo.hpp"
#include "uomap.hpp"
#include "uotile.hpp"
#include "multicollection.hpp"
namespace uo {
    //======================================================================
    class UOMgr {
        struct DFNDef {
            std::filesystem::path mulpath ;
            std::filesystem::path uoppath ;
            std::filesystem::path staidxpath ;
            std::filesystem::path stamulpath ;
            std::filesystem::path mapdifflpath ;
            std::filesystem::path mapdiffpath ;
            std::filesystem::path stadifflpath ;
            std::filesystem::path stadiffipath ;
            std::filesystem::path stadiffpath ;
            int width ;
            int height ;
            int number ;
            DFNDef():width(0),height(0),number(0){}
            DFNDef(int number):DFNDef(){this->number = number;}
        };
        TileInfo tileInfo ;
        std::map<int,UOMap> world ;
        MultiCollection multiCollection ;
        
        auto loadDefinition(const std::filesystem::path &uodir,const DFNDef &def,bool applyDiff,bool log)->void ;
        auto loadDefinition(const std::filesystem::path &uodir, const std::filesystem::path &mapdfn,bool applyDiff,bool log) ->void ;
    public:
        UOMgr() = default;
        UOMgr(const std::filesystem::path &uodir, const std::filesystem::path &mapdfn) ;
        auto load(const std::filesystem::path &uodir, const std::filesystem::path &mapdef,bool log,bool applyDiff)->void ;
        
        // Tile information
        auto terrainSize() const -> size_t ;
        auto terrain(size_t tileid) const -> const TerrainInfo& ;
        auto terrain(size_t tileid)  ->  TerrainInfo& ;
        auto artSize() const -> size_t ;
        auto art(size_t tileid) const -> const ArtInfo& ;
        auto art(size_t tileid)  ->  ArtInfo& ;
        
        // World information
        auto artTileAt(int world, int x, int y) const ->const std::vector<UOTile>& ;
        auto terrainTileAt(int world, int x, int y) const -> UOTile ;
        
        // Multi Information
        auto sizeMulti() const-> size_t ;
        auto multiFor(int multiid) const -> const MultiEntry& ;
    };
}
#endif /* uomgr_hpp */
