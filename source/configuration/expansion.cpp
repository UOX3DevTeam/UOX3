//

#include "expansion.hpp"

#include <algorithm>
#include <stdexcept>


using namespace std::string_literals ;


//======================================================================
const std::map< std::string,Expansion::type_t> Expansion::NAMETYPEMAP{
    {"CORESHARDERA"s,SHARD},{"ARMORCALCULATION"s,ARMOR},{"STRENGTHDAMAGEBONUS"s,STRENGTH},{"TACTICSDAMAGEBONUS"s,TATIC},
    {"ANATOMYDAMAGEBONUS"s,ANATOMY},{"LUMBERJACKDAMAGEBONUS"s,LUMBERJACK,},{"RACIALDAMAGEBONUS"s,RACIAL},{"DAMAGEBONUSCAP"s,DAMAGE},
    {"SHIELDPARRY"s,SHIELD},{"WEAPONPARRY"s,WEAPON},{"WRESTLINGPARRY"s,WRESTLING},{"COMBATHITCHANCE"s,COMBAT}
};
//======================================================================
auto Expansion::nameFor(type_t type) -> const std::string& {
    auto iter = std::find_if(NAMETYPEMAP.begin(),NAMETYPEMAP.end(),[type](const std::pair<std::string,type_t> &entry){
        return type == entry.second ;
    });
    if (iter == NAMETYPEMAP.end()){
        throw std::runtime_error("No name was found for type_t setting: "s+ std::to_string(static_cast<int>(type)));
    }
    return iter->first;
}
//======================================================================
const Era Expansion::defaultEra = Era("lbr");
//======================================================================
Expansion::Expansion(){
    reset();
}
//======================================================================
auto Expansion::reset() ->void {
    values = std::vector<Era>(NAMETYPEMAP.size(),Era("core"));
    values.at(SHARD) = Era("lbr");
}
//======================================================================
auto Expansion::operator[](type_t type) const -> const Era& {
    return values.at(static_cast<int>(type));
}
//======================================================================
auto Expansion::operator[](type_t type)  ->  Era& {
    return values.at(static_cast<int>(type));
}
//======================================================================
auto Expansion::setKeyValue(const std::string &key, const std::string &value) ->bool {
    auto rvalue = false ;
    auto iter = NAMETYPEMAP.find(key) ;
    if (iter != NAMETYPEMAP.end()){
        rvalue = true ;
        values.at(iter->second) = Era(value) ;
    }
    return rvalue ;
}
//======================================================================
auto Expansion::valueFor(const std::string &key) const -> std::optional<std::string> {
    auto iter = NAMETYPEMAP.find(key) ;
    if (iter == NAMETYPEMAP.end()){
        return {};
    }
    return values.at(iter->second).describe();
}
//EraStringToEnum
//======================================================================
auto Expansion::normalizedEraString(const std::string &eraString, bool useDefault, bool inheritCore) const -> Era {
    auto era = Era(eraString) ;
    const auto COREERA = Era(Era::CORE) ;
    if (useDefault){
        era = defaultEra ;
    }
    if (era == COREERA && inheritCore){
        era = values.at(SHARD);
    }
    return era ;
}
//EraEnumToString
//======================================================================
auto Expansion::normalizedEra(const Era &era, bool coreEnum) const -> const std::string & {
    const auto COREERA = Era(Era::CORE) ;
    if (values.at(SHARD) == era && !coreEnum) {
        return COREERA.describe() ;
    }
    else {
        return era.describe();
    }
}
