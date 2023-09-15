//

#include "shortvalue.hpp"
#include <stdexcept>


using namespace std::string_literals ;

//======================================================================
const std::map<std::string, ShortValue::item_t> ShortValue::NAMEITEMMAP{
    {"BACKUPSAVERATIO"s,SAVERATIO},{"MAXSTEALTHMOVEMENTS"s,MAXSTEALTHMOVEMENT},
    {"MAXSTAMINAMOVEMENTS"s,MAXSTAMINIAMOVEMENT},{"AMBIENTSOUNDS"s,AMBIENTSOUND},
    {"SELLMAXITEMS"s,MAXSELLITEM},{"BANKBUYTHRESHOLD"s,BANKBUYTHRESHOLD},
    {"FISHINGSTAMINALOSS"s,FISHINGSTAMINALOSS},{"OREPERAREA"s,OREPERAREA},
    {"FISHPERAREA"s,FISHPERAREA},{"HUNGERDMGVAL"s,HUNGERDAMAGE},
    {"THIRSTDRAINVAL"s,THIRSTDRAIN},{"MAXRANGE"s,MAXRANGE},
    {"SPELLMAXRANGE"s,MAXSPELLRANGE},{"NPCDAMAGERATE"s,NPCDAMAGERATE},
    {"NPCBASEFLEEAT"s,NPCBASEFLEEAT},{"NPCBASEREATTACKAT"s,NPCBASEREATTACKAT},
    {"ATTACKSTAMINA"s,ATTACKSTAMINA},{"ARCHERYHITBONUS"s,ARCHERYTHITBONUS},
    {"WEAPONDAMAGECHANCE"s,WEAPONDAMAGECHANCE},{"STARTGOLD"s,STARTGOLD}
};
//======================================================================
ShortValue::ShortValue() {
    this->reset();
}
//======================================================================
auto ShortValue::reset() ->void {
    values = std::vector<std::int16_t>(NAMEITEMMAP.size(),0) ;
    
}

//======================================================================
auto ShortValue::operator[](item_t item) const -> const std::int16_t &{
    return values.at(static_cast<size_t>(item)) ;
}
//======================================================================
auto ShortValue::operator[](item_t item) -> std::int16_t &{
    return values.at(static_cast<size_t>(item)) ;
}

//======================================================================
auto ShortValue::setKeyValue(const std::string &key, const std::string &value) ->bool {
    auto rvalue = false ;
    auto iter = NAMEITEMMAP.find(key) ;
    if (iter != NAMEITEMMAP.end()){
        rvalue = true ;
        values.at(static_cast<size_t>(iter->second)) = static_cast<std::int16_t>(std::stoi(value,nullptr,0));
    }
    return rvalue ;
}
//======================================================================
auto ShortValue::valueFor(const std::string &value) const -> std::optional<std::int16_t> {
    auto iter = NAMEITEMMAP.find(value) ;
    if (iter == NAMEITEMMAP.end()){
        return {};
    }
    return  values.at(static_cast<size_t>(iter->second));
}

