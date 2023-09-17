//

#include "realnumberconfig.hpp"

#include <algorithm>
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
auto RealNumberConfig::nameFor(type_t setting)-> const std::string &{
    auto iter = std::find_if(NAMETYPEMAP.begin(),NAMETYPEMAP.end(),[setting](const std::pair<std::string,type_t> &entry){
        return setting == entry.second;
    });
    if (iter == NAMETYPEMAP.end()){
        throw std::runtime_error("No name was found for type_t setting: "s+std::to_string(static_cast<int>(setting)));
    }
    return iter->first ;
}
//======================================================================
RealNumberConfig::RealNumberConfig(){
    reset();
}
//======================================================================
auto RealNumberConfig::reset()  -> void {
    values = std::vector<double>(NAMETYPEMAP.size(),0.0);
    this->operator[](NPCMOVEMENT) = 0.6 ;
    this->operator[](NPCRUNNING) = 0.3 ;
    this->operator[](NPCFLEEING) = 0.4 ;
    this->operator[](NPCMOUNTMOVEMENT) = 0.3 ;
    this->operator[](NPCMOUNTRUNNING) = 0.12 ;
    this->operator[](NPCMOUNTFLEEING) = 0.2 ;
    this->operator[](NPCSPELLCAST) = 1.0 ;
    this->operator[](GLOBALATTACK) = 1.0 ;
    this->operator[](RESTOCKMULTIPLER) = 1.0 ;
    this->operator[](RESTOCKMULTIPLER) = 1.0 ;
    this->operator[](FAMEREWARDMULTIPLER) = 1.0 ;
    this->operator[](CHECKITEMS) = 1.5 ;
    this->operator[](CHECKBOATS) = 0.65 ;
    this->operator[](WEIGHTSTR) = 3.5 ;
    this->operator[](ARCHERYDELAY) = 0.5 ;

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
