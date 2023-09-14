//

#ifndef clientfeature_hpp
#define clientfeature_hpp

#include <bitset>
#include <cstdint>
#include <iostream>
#include <map>
#include <optional>
#include <string>

//======================================================================
struct ClientFeature {
    enum feature_t {
        CHAT = 0, // 0x01
        UOR,      // 0x02
        TD,       // 0x04
        LBR,      // 0x08 - Enables LBR features: mp3s instead of midi, show new LBR monsters
        AOS, // 0x10 - Enable AoS monsters/map, AoS skills, Necro/Pala/Fight book stuff - works
        // for 4.0+
        SIXCHARS,   // 0x20 - Enable sixth character slot
        SE,         // 0x40
        ML,         // 0x80 - Elven race, new spells, skills + housing tiles
        EIGHTAGE,   // 0x100 - Splash screen for 8th age
        NINTHAGE,   // 0x200 - Splash screen for 9th age
        TENTHAGE,   // 0x400 - Splash screen for 10th age - crystal/shadow house tiles
        UNKNOWN1,   // 0x800 - Increased housing/bank storage (6.0.3.0 or earlier)
        SEVENCHARS, // 0x1000 - Enable seventh character slot
        // CF_BIT_KRFACES,        // 0x2000 - KR release (6.0.0.0)
        // CF_BIT_TRIAL,            // 0x4000 - Trial account
        EXPANSION = 15, // 0x8000 - Live account
        SA, // 0x10000 - Enable SA features: gargoyle race, spells, skills, housing tiles -
        // clients 6.0.14.2+
        HS, // 0x20000 - Enable HS features: boats, new movementtype? ++
        GOTHHOUSE,    // 0x40000
        RUSTHOUSE,    // 0x80000
        JUNGLEHOUSE,  // 0x100000 - Enable Jungle housing tiles
        SHADOWHOUSE,  // 0x200000 - Enable Shadowguard housing tiles
        TOLHOUSE,     // 0x400000 - Enable Time of Legends features
        ENDLESSHOUSE // 0x800000 - Enable Endless Journey account
    };
    static const std::map<std::string,feature_t> FEATURENAMEMAP;
    
    std::bitset<32> featureSet ;
    
    ClientFeature(std::uint32_t value) ;
    ClientFeature() ;
    auto operator=(std::uint32_t value) -> ClientFeature& ;
    auto test(feature_t feature) const ->bool ;
    auto set(feature_t feature, bool state) ->void ;
    auto setKeyValue(const std::string &key, const std::string &value) ->bool ;
    auto value() const -> std::uint32_t ;
    
};

#endif /* clientfeature_hpp */
