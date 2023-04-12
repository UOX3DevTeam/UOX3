//

#ifndef uoxtile_hpp
#define uoxtile_hpp

#include <cstdint>
#include <string>
#include <vector>
#include <istream>

#include "tile.hpp"
//=========================================================================================
// These are compatability wrappers for UOX3
enum TileFlags
{
    // Flag:                Also known as:
    TF_FLOORLEVEL    = 0,    // "Background"
    TF_HOLDABLE,            // "Weapon"
    TF_TRANSPARENT,            // "SignGuildBanner"
    TF_TRANSLUCENT,            // "WebDirtBlood"
    TF_WALL,                // "WallVertTile"
    TF_DAMAGING,
    TF_BLOCKING,            // "Impassable"
    TF_WET,                    // "LiquidWet"
    TF_UNKNOWN1,            // "Ignored"
    TF_SURFACE,                // "Standable"
    TF_CLIMBABLE,            // "Bridge"
    TF_STACKABLE,            // "Generic"
    TF_WINDOW,                // "WindowArchDoor"
    TF_NOSHOOT,                // "CannotShootThru"
    TF_DISPLAYA,            // "Prefix A"
    TF_DISPLAYAN,            // "Prefix An"
    TF_DESCRIPTION,            // "Internal"
    TF_FOLIAGE,                // "FadeWithTrans"
    TF_PARTIALHUE,
    TF_UNKNOWN2,
    TF_MAP,
    TF_CONTAINER,
    TF_WEARABLE,            // "Equipable"
    TF_LIGHT,                // "LightSource"
    TF_ANIMATED,
    TF_NODIAGONAL,            // "HoverOver" in SA clients and later, to determine if tiles can be moved on by flying gargoyles
    TF_UNKNOWN3,            // "NoDiagonal" in SA clients and later?
    TF_ARMOR,                // "WholeBodyItem"
    TF_ROOF,                // "WallRoofWeap"
    TF_DOOR,
    TF_STAIRBACK,            // "ClimbableBit1"
    TF_STAIRRIGHT,            // "ClimbableBit2"
    TF_ALPHABLEND,
    TF_USENEWART,
    TF_ARTUSED,
    TF_BIT36,
    TF_NOSHADOW,
    TF_PIXELBLEED,
    TF_PLAYANIMONCE,
    TF_BIT40,
    TF_MULTIMOVABLE,
    TF_COUNT
};

//=========================================================================================
// CTile
//=========================================================================================
class CTile :public uo::arttile_t {
    
public:
    CTile() ;
    CTile(std::istream &input,bool is64);
    auto Unknown1() const -> std::uint16_t ;
    auto Unknown2() const -> std::uint8_t ;
    auto Unknown3() const -> std::uint8_t;
    auto Unknown4() const -> std::uint8_t ;
    auto Unknown5() const -> std::uint8_t ;
    auto Hue() const -> std::uint8_t ;
    auto Quantity() const -> std::uint8_t ;
    auto Animation() const -> std::uint16_t ;
    auto Weight() const -> std::uint8_t;
    auto Layer() const -> std::int8_t ;
    auto Height() const -> std::int8_t ;
    auto ClimbHeight( bool trueHeight = false ) const -> std::int8_t;
    auto Unknown1( std::uint16_t newVal ) ->void ;
    auto Unknown2( std::uint8_t newVal ) ->void ;
    auto Unknown3( std::uint8_t newVal ) ->void ;
    auto Unknown4( std::uint8_t newVal ) ->void ;
    auto Unknown5( std::uint8_t newVal ) ->void ;
    auto Animation( std::uint16_t newVal ) ->void ;
    auto Weight( std::uint8_t newVal ) ->void ;
    auto Layer( std::int8_t newVal ) ->void ;
    auto Height( std::int8_t newVal ) ->void ;
    auto Hue( std::uint8_t newVal ) ->void ;
    auto Quantity( std::uint8_t newVal ) ->void ;
    auto Name() const -> const std::string& { return name; }
    auto Name( const std::string &value ) -> void { name = value; }
    auto CheckFlag(TileFlags toCheck) const -> bool {
        if (toCheck >= TF_COUNT){
            return false ;
        }
        return flag.test(static_cast<std::size_t>(toCheck));
    }
    auto SetFlag( TileFlags toSet, bool newVal ) ->void {
        if( toSet < TF_COUNT ){
            flag.set( static_cast<std::size_t>(toSet), newVal );
        }
    }

};

//=========================================================================================
// CLand
//=========================================================================================
class CLand : public uo::landtile_t
{
private:
    // List of road tiles, or road-related tiles, on which houses cannot be placed
    constexpr static
    std::array<std::uint16_t, 18> roadIds
    {
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
    
public:
    CLand();
    CLand(std::istream &input,bool is64);
    ~CLand() = default;
    auto TextureId() const -> std::uint16_t;
    auto TextureId( std::uint16_t newVal ) -> void;
    auto IsRoadId() const -> bool;
    auto Name() const -> const std::string& { return name; }
    auto Name( const std::string &value ) -> void { name = value; }
    auto CheckFlag(TileFlags toCheck) const -> bool {
        if (toCheck >= TF_COUNT){
            return false ;
        }
        return flag.test(static_cast<std::size_t>(toCheck));
    }
    auto SetFlag( TileFlags toSet, bool newVal ) ->void {
        if( toSet < TF_COUNT ){
            flag.set( static_cast<std::size_t>(toSet), newVal );
        }
    }

};

//=========================================================================================
// TileInfo
//=========================================================================================
class TileInfo {
    std::vector<CLand> terrainData ;
    std::vector<CTile> artData ;
    auto processTerrain( std::istream &input,bool is64) -> void ;
    auto processArt(std::istream &input,bool is64) -> void ;
public:
    TileInfo();
    TileInfo( const std::string &filename );
    auto LoadTiles( const std::string &filename ) -> bool;
    auto TerrainInfo( std::uint16_t tileId ) const -> const CLand&;
    auto TerrainInfo( std::uint16_t tileId ) -> CLand&;
    auto ArtInfo( std::uint16_t tileId ) -> CTile&;
    auto ArtInfo( std::uint16_t tileId ) const -> const CTile&;

    auto SizeTerrain() const -> std::size_t;
    auto SizeArt() const -> std::size_t;

    auto CollectionTerrain() const -> const std::vector<CLand>&;
    auto CollectionTerrain() -> std::vector<CLand>&;
    auto CollectionArt() const -> const std::vector<CTile>&;
    auto CollectionArt() -> std::vector<CTile>&;
    auto TotalMemory() const -> std::size_t;

};



enum TileType_t { terrain, art, dyn };

// A structure that holds tile information. Type, id, information, altitude (if in a map), hue (if a static tile)

struct Tile_st{
    TileType_t type;
    std::uint16_t tileId;    // for instance, this should be a type of tileid_t , that would make sense!
    std::int8_t altitude;
    std::uint16_t staticHue;
    union {
        const CTile *artInfo;
        const CLand *terrainInfo;
    };
    constexpr static std::array<std::uint16_t, 11> terrainVoids {
        430, 431, 432,
        433, 434, 475,
        580, 610, 611,
        612, 613
    };
    auto CheckFlag( TileFlags toCheck ) const -> bool {
        if( type != TileType_t::terrain ) {
            return artInfo->CheckFlag( toCheck );
        }
        return terrainInfo->CheckFlag( toCheck );
    }
    Tile_st( TileType_t type = TileType_t::terrain ) :type( type ), tileId( 0 ), altitude( 0 ), staticHue( 0 ), artInfo( nullptr ) {}
    auto isVoid() const -> bool {
        auto rValue = false;
        if( type == TileType_t::terrain ) {
            auto iter = std::find_if( terrainVoids.begin(), terrainVoids.end(), [this]( const std::uint16_t &value ) {
                return tileId == value;
            });
            rValue = iter != terrainVoids.end();
        }
        return rValue;
    }
    auto IsMountain() const -> bool {
        auto rValue = false;
        if( type != TileType_t::terrain ) {
            rValue = (( tileId >= 431 ) && ( tileId <= 432 )) || (( tileId >= 467 ) && ( tileId <= 474 )) ||
            (( tileId >= 543 ) && ( tileId <= 560 )) || (( tileId >= 1754 ) && ( tileId <= 1757 )) ||
            (( tileId >= 1787 ) && ( tileId <= 1789 )) || (( tileId >= 1821 ) && ( tileId <= 1824 )) ||
            (( tileId >= 1851 ) && ( tileId <= 1854 )) || (( tileId >= 1881 ) && ( tileId <= 1884 ));
        }
        return rValue;
    }
    auto top() -> std::int16_t {
        auto value = static_cast<int16_t>( altitude );
        if( type != TileType_t::terrain ) {
            auto temp = artInfo->Height();
            temp = ( temp & 0x8 ? ( temp & 0xF ) >> 1 : temp & 0xF );
            
            value += temp;
        }
        return value;
    }
    auto height() -> std::uint8_t {
        if( type != TileType_t::terrain ) {
            return artInfo->Height();
        }
        return 0;
    }
    auto name() const -> std::string {
        if( type != TileType_t::terrain ) {
            return artInfo->Name();
        }
        return terrainInfo->Name();
    }
};
#endif /* uoxtile_hpp */
