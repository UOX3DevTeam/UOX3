//Copyright © 2023 Charles Kerr. All rights reserved.

#ifndef tileinfo_hpp
#define tileinfo_hpp

#include <array>
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
        
        auto checkFlag(flag_t flagbit) const ->bool ;
        
    };
    
    //======================================================================
    // TerrainInfo
    //======================================================================
    struct TerrainInfo : public BaseTileInfo {
        // UOX3 extension
        static constexpr std::array<std::uint16_t,18> ROADIDS{
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
        
        std::uint16_t textureID ;
        TerrainInfo();
        TerrainInfo(std::istream &input,bool largFlag);
        ~TerrainInfo() = default;
        auto load(std::istream &input,bool largeFlag) ->void final ;
        auto describe() const -> std::string final;
        // UOX3 extensions
        auto isRoad() const ->bool ;
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
        // This really should be unsigned, but everyone wants it to be signed
        std::int8_t height ;
        
        ArtInfo();
        ArtInfo(std::istream &input,bool largeFlag);
        
        ~ArtInfo() = default;
        auto load(std::istream &input,bool largeFlag) ->void final ;
        
        auto describe() const -> std::string final ;
        auto climbHeight(bool trueHeight = false) const -> std::int8_t ;
        
    };
    
    //======================================================================
    // TileInfo
    //======================================================================
    struct TileInfo {
        std::vector<TerrainInfo> terrainInfo ;
        std::vector<ArtInfo> artInfo ;
        TileInfo(const std::filesystem::path &path = std::filesystem::path());
        auto load(const std::filesystem::path &path) ->bool ;
        auto loadOverride(const std::filesystem::path &path) ->void ;
        auto loadOverride(std::istream &input) ->void ;
        // Compatability with UOX3,
        auto loadUOX3Override(const std::filesystem::path &path) ->void ;
        auto loadUOX3Override(std::istream &input) ->void ;
        [[maybe_unused]] auto checkUOX3Attribute(std::uint32_t tilenumber,const std::string &key, const std::string &value) ->bool ;
        [[maybe_unused]] auto checkUOX3Flag(std::uint32_t tilenumber,const std::string &key, const std::string &value) ->bool ;
        
        auto sizeArt() const ->size_t { return artInfo.size();}
    };
}
#endif /* tileinfo_hpp */
