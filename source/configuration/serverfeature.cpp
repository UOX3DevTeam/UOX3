//

#include "serverfeature.hpp"
#include <stdexcept>


using namespace std::string_literals ;
/*
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

 */
//======================================================================
const std::map<std::string,ServerFeature::feature_t> ServerFeature::FEATURENAMEMAP{
    {"UNKNOWN1",UNKNOWN1},{"IGR",IGR},{"ONECHAR",ONECHAR},{"CONTEXTMENUS",CONTEXTMENUS},
    {"LIMITCHAR",LIMITCHAR},{"AOS",AOS},{"SIXCHARS",SIXCHARS},{"SE",SE},
    {"ML",ML},{"UNKNOWN2",UNKNOWN2},{"SEND3DTYPE",SEND3DTYPE},{"UNKNOWN4",UNKNOWN4},
    {"SEVENCHARS",SEVENCHARS},{"UNKNOWN5",UNKNOWN5},{"NEWMOVE",NEWMOVE},{"FACTIONAREAS",FACTIONAREAS}
};
//======================================================================
ServerFeature::ServerFeature() {
    featureSet = 0 ;
    this->set(CONTEXTMENUS,true) ;
    this->set(AOS,true) ;
    this->set(SIXCHARS,true) ;
    this->set(SE,true) ;
    this->set(ML,true) ;
}
//======================================================================
ServerFeature::ServerFeature(std::uint16_t value) {
    featureSet = static_cast<std::uint32_t>(value);
}
//======================================================================
auto ServerFeature::operator=(std::uint16_t value) -> ServerFeature& {
    featureSet = static_cast<std::uint32_t>(value);
    return *this ;
}
//======================================================================
auto ServerFeature::test(feature_t feature) const ->bool {
    return featureSet.test(feature) ;
}
//======================================================================
auto ServerFeature::set(feature_t feature, bool state) -> void {
    featureSet.set(feature,state);
}
//======================================================================
auto ServerFeature::setKeyValue(const std::string &key, const std::string &value) ->bool {
    auto rvalue = false ;
    if (key == "SERVERFEATURES"){
        rvalue = true ;
        featureSet = std::stoul(value,nullptr,0);
    }
    return rvalue ;
}
//======================================================================
auto ServerFeature::value() const -> std::uint16_t {
    return static_cast<std::uint16_t>(featureSet.to_ulong());
}

