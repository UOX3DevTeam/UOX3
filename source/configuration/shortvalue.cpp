//

#include "shortvalue.hpp"

#include <algorithm>
#include <stdexcept>


using namespace std::string_literals ;

//======================================================================
const std::map<std::string, ShortValue::item_t> ShortValue::NAMEITEMMAP{
    {"BACKUPSAVERATIO"s,SAVERATIO},{"MAXSTEALTHMOVEMENTS"s,MAXSTEALTHMOVEMENT},
    {"MAXSTAMINAMOVEMENTS"s,MAXSTAMINAMOVEMENT},{"AMBIENTSOUNDS"s,AMBIENTSOUND},
    {"SELLMAXITEMS"s,MAXSELLITEM},{"BANKBUYTHRESHOLD"s,BANKBUYTHRESHOLD},
    {"FISHINGSTAMINALOSS"s,FISHINGSTAMINALOSS},{"HUNGERDMGVAL"s,HUNGERDAMAGE},
    {"THIRSTDRAINVAL"s,THIRSTDRAIN},{"MAXRANGE"s,MAXRANGE},
    {"SPELLMAXRANGE"s,MAXSPELLRANGE},{"NPCDAMAGERATE"s,NPCDAMAGERATE},
    {"NPCBASEFLEEAT"s,NPCBASEFLEEAT},{"NPCBASEREATTACKAT"s,NPCBASEREATTACKAT},
    {"ATTACKSTAMINA"s,ATTACKSTAMINA},{"ARCHERYHITBONUS"s,ARCHERYTHITBONUS},
    {"STARTGOLD"s,STARTGOLD}
};
//======================================================================
auto ShortValue::nameFor(item_t setting)-> const std::string &{
    auto iter = std::find_if(NAMEITEMMAP.begin(),NAMEITEMMAP.end(),[setting](const std::pair<std::string,item_t> &entry){
        return setting == entry.second;
    });
    if (iter == NAMEITEMMAP.end()){
        throw std::runtime_error("No name was found for item_t setting: "s+std::to_string(static_cast<int>(setting)));
    }
    return iter->first ;
}

//======================================================================
ShortValue::ShortValue() {
    this->reset();
}
//======================================================================
auto ShortValue::reset() ->void {
    values = std::vector<std::int16_t>(NAMEITEMMAP.size(),0) ;
    this->operator[](SAVERATIO) = 5 ;
    this->operator[](MAXSTEALTHMOVEMENT) = 10 ;
    this->operator[](MAXSTAMINAMOVEMENT) = 15 ;
    this->operator[](AMBIENTSOUND) = 5 ;
    this->operator[](MAXSELLITEM) = 5 ;
    this->operator[](BANKBUYTHRESHOLD) = 2000 ;
    this->operator[](FISHINGSTAMINALOSS) = 2 ;
    this->operator[](HUNGERDAMAGE) = 2 ;
    this->operator[](THIRSTDRAIN) = 2 ;
    this->operator[](MAXRANGE) = 10 ;
    this->operator[](MAXSPELLRANGE) = 10 ;
    this->operator[](NPCDAMAGERATE) = 1 ;
    this->operator[](NPCBASEFLEEAT) = 20 ;
    this->operator[](NPCBASEREATTACKAT) = 40 ;
    this->operator[](ATTACKSTAMINA) = -2 ;
    this->operator[](ARCHERYTHITBONUS) = 10 ;
    this->operator[](STARTGOLD) = 1000 ;
    
    
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

