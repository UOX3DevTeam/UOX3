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
        TOL3D
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
    
    auto describe() const -> const std::string & ;
    
    ClientType() ;
    ClientType(type_t type);
};
//======================================================================
// ClientVersion
//======================================================================
struct ClientVersion {
    // Only client-versions with major packet changes included. Clients between two versions "belong" to
    // the nearest previous version
    enum version_t {
        DEFAULT = 0,
        V400,
        V407A,
        V4011C,
        V500A, // map0.mul size increased from 6144x4096 to 7168x4096
        V502A,
        V5082,
        V6000,  // Felucca/Trammel no longer both use map0.mul, Trammel gets its own: map1.mul
        V6017,  // Packet updates to support container-grid in KR client, support implemented so it
        // (in theory) will have no effect on lower versions
        V6050,  // 21 extra bytes of data added prior to initial 0x80 packet, in the form of a new
        // clientversion packet: 0xEF
        V25302, // UOKR3D 2.53.0.2
        V60142,
        V7000, // animation packet 0xE2 replaces 0x6E, packet 0xF3 is sent instead of 0x1A (object
        // information packet)
        V7090,
        V70130, // Packet 0xA9 updated with extra information and longer City/Building names
        V70151,
        V70160, // Packet 0xF8 (New Character Creation) replaces 0x00 (Character Creation)
        V70240, // Map#.mul files are now wrapped in .uop headers. This means incompability with
        // tools, and updated emulators needed to read map correctly.
        V70300,
        V70331,
        V704565,
        V705527, // Max update range increase from 18 to 24
        V70610
    };
    std::uint8_t major ;
    std::uint8_t minor ;
    std::uint8_t sub ;
    std::uint8_t letter ;
    version_t shortVersion ;
    
    ClientVersion() ;
    ClientVersion(std::uint32_t version) ;
    ClientVersion(std::uint8_t major, std::uint8_t minor, std::uint8_t sub, std::uint8_t letter) ;
    auto version() const -> std::uint32_t ;
    auto setVersion(std::uint32_t version) ->void;
    auto describe() const -> std::string ;
    auto operator==(ClientVersion::version_t shortVersion) const ->bool ;
    auto operator!=(ClientVersion::version_t shortVersion) const ->bool ;
    auto operator>=(ClientVersion::version_t shortVersion) const ->bool;
    auto operator>(ClientVersion::version_t shortVersion) const ->bool;
    auto operator<=(ClientVersion::version_t shortVersion) const ->bool;
    auto operator<(ClientVersion::version_t shortVersion) const ->bool;
    
    
};
#endif /* clientversion_hpp */
