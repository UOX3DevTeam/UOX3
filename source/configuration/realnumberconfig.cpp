//

#include "realnumberconfig.hpp"
#include <stdexcept>


using namespace std::string_literals ;

//======================================================================
const std::map<std::string, RealNumberConfig::type_t> RealNumberConfig::NAMETYPEMAP{
    {"NPCMOVEMENTSPEED"s,NPCMOVEMENT},{"NPCRUNNINGSPEED"s,NPCRUNNING},{"NPCFLEEINGSPEED"s,NPCFLEEING},{"NPCMOUNTEDWALKINGSPEED"s,NPCMOUNTMOVEMENT},
    {"NPCMOUNTEDRUNNINGSPEED"s,NPCMOUNTRUNNING},{"NPCMOUNTEDFLEEINGSPEED"s,NPCMOUNTFLEEING},{"NPCSPELLCASTSPEED"s,NPCSPELLCAST},{"GLOBALATTACKSPEED"s,GLOBALATTACK},
    {"GLOBALRESTOCKMULTIPLIER"s,RESTOCKMULTIPLER},{"BODFAMEREWARDMULTIPLIER"s,FAMEREWARDMULTIPLER},{"CHECKITEMS"s,CHECKITEMS},{"CHECKBOATS"s,CHECKBOATS},
    {"CHECKNPCAI"s,CHECKAI},{"CHECKSPAWNREGIONS"s,CHECKSPAWN},{"ACCOUNTFLUSH"s,FLUSHTIME},{"BODGOLDREWARDMULTIPLIER"s,GOLDREWARDMULTIPLER},
    {"WEIGHTPERSTR"s,WEIGHTSTR},{"ARCHERYSHOOTDELAY"s,ARCHERYDELAY}
};
//======================================================================
RealNumberConfig::RealNumberConfig(){
    reset();
}
//======================================================================
auto RealNumberConfig::reset()  -> void {
    values = std::vector<double>(NAMETYPEMAP.size(),0.0);
}

//======================================================================
auto RealNumberConfig::size() const -> size_t{
    return values.size() ;
}

//======================================================================
auto RealNumberConfig::operator[](type_t type) const -> const double& {
    return values.at(static_cast<size_t>(type));
}
//======================================================================
auto RealNumberConfig::operator[](type_t type) -> double& {
    return values.at(static_cast<size_t>(type));
}
//======================================================================
auto RealNumberConfig::setKeyValue(const std::string &key, const std::string &value) ->bool {
    auto rvalue = false ;
    auto iter = NAMETYPEMAP.find(key) ;
    if (iter != NAMETYPEMAP.end()){
        rvalue = true ;
        values.at(static_cast<size_t>(iter->second)) = std::stod(value) ;
    }
    return rvalue ;
}
//======================================================================
auto RealNumberConfig::valueFor(const std::string &key) const -> std::optional<double> {
    auto iter = NAMETYPEMAP.find(key) ;
    if (iter == NAMETYPEMAP.end()){
        return {};
    }
    return values.at(static_cast<size_t>(iter->second));

}
