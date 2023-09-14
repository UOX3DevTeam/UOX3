//

#ifndef serverfeature_hpp
#define serverfeature_hpp

#include <bitset>
#include <cstdint>
#include <iostream>
#include <map>
#include <optional>
#include <string>

//======================================================================
struct ServerFeature {
    enum feature_t {
        UNKNOWN1 = 0, // 0x01
        IGR,          // 0x02
        ONECHAR,      // 0x04 - One char only, Siege-server style
        CONTEXTMENUS, // 0x08
        LIMITCHAR,    // 0x10 - Limit amount of chars, combine with OneChar
        AOS,          // 0x20 - Enable Tooltips, fight system book - but not
        // monsters/map/skills/necro/pala classes
        SIXCHARS,     // 0x40 - Use 6 character slots instead of 5 (4.0.3a)
        SE,           // 0x80 - Samurai and Ninja classes, bushido, ninjitsu (4.0.5a)
        ML,           // 0x100 - Elven race, spellweaving (4.0.11d)
        UNKNOWN2,     // 0x200 - added with UO:KR launch (6.0.0.0)
        SEND3DTYPE,   // 0x400 - Send UO3D client type? KR and SA clients will send 0xE1)
        UNKNOWN4,     // 0x800 - added sometime between UO:KR and UO:SA
        SEVENCHARS,   // 0x1000 - Use 7 character slots instead of 5?6?, only 2D client?
        UNKNOWN5, // 0x2000 - added with UO:SA launch, imbuing, mysticism, throwing? (7.0.0.0)
        NEWMOVE,  // 0x4000 - new movement system (packets 0xF0, 0xF1, 0xF2))
        FACTIONAREAS  // 0x8000 - Unlock new Felucca faction-areas (map0x.mul?)
    };
    static const std::map<std::string,feature_t> FEATURENAMEMAP ;
    std::bitset<16> featureSet ;
    
    ServerFeature() ;
    ServerFeature(std::uint16_t value);
    auto operator=(std::uint16_t value) -> ServerFeature& ;
    auto test(feature_t feature) const ->bool ;
    auto set(feature_t feature, bool state) -> void ;
    auto setKeyValue(const std::string &key, const std::string &value) ->bool ;
    auto value() const -> std::uint16_t ;
};

#endif /* serverfeature_hpp */
