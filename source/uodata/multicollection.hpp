//

#ifndef multicollection_hpp
#define multicollection_hpp

#include <cstdint>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "tileinfo.hpp"
#include "uoflag.hpp"
//======================================================================
namespace uo {
    struct ArtInfo ;
    //======================================================================
    // MultiTile ;
    //======================================================================
    struct MultiTile {
        std::uint16_t tileid ;
        int xoffset ;
        int yoffset ;
        int zoffset ;
        Flag flag ;
        const ArtInfo *info ;
        MultiTile() ;
        MultiTile(std::uint16_t tileid);
        auto checkFlag(flag_t flagbit) const ->bool ;
    };
    //======================================================================
    // MultiEntry
    //======================================================================
    struct MultiEntry {
        static const std::map<int,std::string> ENTRYNAME ;
        int minXOffset ;
        int maxXOffset ;
        int minYOffset ;
        int maxYOffset ;
        int minZOffset ;
        int maxZOffset ;
        std::vector<MultiTile> tiles ;
        MultiEntry() ;
        auto addTile(const MultiTile &tile) ->void ;
    };
    //======================================================================
    // MultiCollection
    //======================================================================
    class MultiCollection {
        std::unordered_map<int,MultiEntry> multiEntry ;
        static constexpr auto HSSIZE = 908592;
        static const std::string UOPHASH ;
        
        auto loadUOPData(const std::vector<std::uint8_t> &data, const TileInfo *info) -> MultiEntry ;
    public:
        MultiCollection() ;
        auto load(const std::filesystem::path &idxpath, const std::filesystem::path &mulpath,const TileInfo *info) ->bool ;
        auto load(const std::filesystem::path &uoppath,const TileInfo *info) ->bool ;
        
        auto size() const -> size_t ;
        auto entryFor(int multiid) const -> const MultiEntry& ;
        auto exists(int) const ->bool ;
    };
}

#endif /* multicollection_hpp */
