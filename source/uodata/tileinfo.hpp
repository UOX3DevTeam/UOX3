//Copyright © 2023 Charles Kerr. All rights reserved.

#ifndef tileinfo_hpp
#define tileinfo_hpp


#include <filesystem>
#include <iostream>
#include <istream>
#include <map>
#include <string>
#include <vector>

#include "uoflag.hpp"

/* *******************************************************************
 UO is mainly 16 bit, so it does a lot to save size in their binary data
 structures.  We should be making them all ints in the structure, it would be faster (native processor size),
 and simplify (the core server code should not be dealing with the size differences, it is error prone).
 Size of the data should come into play when reading/writeing to a file/packet.
 Oh well, it is what it is.
 ******************************************************************* */

namespace uo{
    constexpr auto ENTRIESPERBLOCK = 32 ;
    constexpr auto INFOSTRINGSIZE = 20 ;
    constexpr auto INVALID = 0xFFFF ;
    constexpr auto TILEDATASIZE = std::size_t(3188736);
    constexpr auto NUMTERRAINBLOCK = 512 ;

    
    //=====================================================================
    enum class TileType {
        BASE,NONE,TERRAIN,ART
    };
    
    //======================================================================
    // BaseTileInfo
    //======================================================================
    struct BaseTileInfo {
        TileType type ;
        Flag flag;
        std::string name;
        BaseTileInfo(){type = TileType::BASE;} ;
        virtual ~BaseTileInfo() = default ;
        virtual auto load(std::istream &input, bool largeFlag) ->void = 0 ;
        auto loadName(std::istream &input) ->void ;
        virtual auto describe() const -> std::string ;
    };
    
    //======================================================================
    // TerrainInfo
    //======================================================================
    struct TerrainInfo : public BaseTileInfo {
        int textureID ;
        TerrainInfo();
        TerrainInfo(std::istream &input,bool largFlag);
        ~TerrainInfo() = default;
        auto load(std::istream &input,bool largeFlag) ->void final ;
        auto describe() const -> std::string final;
    };
    
    //======================================================================
    // ArtInfo
    //======================================================================
    struct ArtInfo : public BaseTileInfo {
        std::uint8_t weight ;
        std::uint8_t quality ;
        std::uint16_t miscData ;
        std::uint8_t unknown2 ;
        std::uint8_t quantity ;
        std::uint16_t animid ;
        std::uint8_t unknown3 ;
        std::uint8_t hue ;
        std::uint16_t stackOffset ;
        std::uint8_t height ;
        
        ArtInfo();
        ArtInfo(std::istream &input,bool largeFlag);
        
        ~ArtInfo() = default;
        auto load(std::istream &input,bool largeFlag) ->void final ;
        
        auto describe() const -> std::string final ;
    };
    
    //======================================================================
    // TileInfo
    //======================================================================
    struct TileInfo {
        std::vector<TerrainInfo> terrainInfo ;
        std::vector<ArtInfo> artInfo ;
//        std::map<int,ArtInfo> terrainInfo ;
//        std::map<int,ArtInfo> artInfo ;
        TileInfo(const std::filesystem::path &path = std::filesystem::path());
        auto load(const std::filesystem::path &path) ->bool ;
        auto loadOverride(const std::filesystem::path &path) ->void ;
        auto loadOverride(std::istream &input) ->void ;
        // Compatability with UOX3,
        auto loadUOX3Override(const std::filesystem::path &path) ->void ;
        auto loadUOX3Override(std::istream &input) ->void ;
        [[maybe_unused]] auto checkUOX3Attribute(std::uint32_t tilenumber,const std::string &key, const std::string &value) ->bool ;
        [[maybe_unused]] auto checkUOX3Flag(std::uint32_t tilenumber,const std::string &key, const std::string &value) ->bool ;
         
    };
}
#endif /* tileinfo_hpp */
