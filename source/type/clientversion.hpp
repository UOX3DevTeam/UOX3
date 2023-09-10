//

#ifndef clientversion_hpp
#define clientversion_hpp

#include <cstdint>
#include <iostream>
#include <string>
#include <map>

//======================================================================
// ClientType
//======================================================================
struct ClientType {
    enum type_t {
        DEFAULT = 0,
        UNKNOWN,
        KRRIOS,
        T2A,  // From 4.0.0p to 4.0.11c
        UO3D, // Third Dawn 3D client
        ML,   // From 4.0.11f to 5.0.9.1
        KR2D, // From 6.0.0.0+ to 6.0.14.1, first packet sent is 0xEF.
        KR3D,
        SA2D, // From 6.0.14.2 to 7.0.8.2, Stygian Abyss expansion client. First patcket sent is
        // 0xEF, requires 0xB9 size-change from 3 to 5, new 0xF3 packet replacex 0x1A
        SA3D,
        HS2D, // From 7.0.9.0 to 7.0.45.89, High Seas expansion client
        HS3D,
        TOL2D, // From 7.0.46.0 to infinity, Time of Legends expansion client (Endless Journey
        // "started" at 7.0.61.0, but it's not an expansion)
        TOL3D,
    };
    static const std::map<std::string,type_t> TYPENAMEMAP ;
    static auto typeForName(const std::string &name)  ->type_t ;
    static auto nameForType(type_t type)->const std::string& ;
    type_t type ;
    
    auto operator=(type_t type) -> ClientType& ;

    auto operator==(type_t version) const -> bool ;
    auto operator!=(type_t type) const ->bool ;

    auto operator>(type_t type) const ->bool ;
    auto operator>=(type_t type) const -> bool ;
    auto operator<(type_t type) const ->bool ;
    auto operator<=(type_t type) const -> bool ;

    ClientType() ;
    ClientType(type_t type);
};
//======================================================================
// ClientVersion
//======================================================================
struct NClientVersion {
    ClientType type ;
    std::uint8_t major ;
    std::uint8_t minor ;
    std::uint8_t sub ;
    std::uint8_t letter ;
    NClientVersion() ;
    NClientVersion(std::uint32_t version) ;
    NClientVersion(std::uint8_t major, std::uint8_t minor, std::uint8_t sub, std::uint8_t letter) ;
    auto version() const -> std::uint32_t ;
};
#endif /* clientversion_hpp */
