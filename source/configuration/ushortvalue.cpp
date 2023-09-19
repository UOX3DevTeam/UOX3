//

#include "ushortvalue.hpp"

#include <algorithm>
#include <stdexcept>


using namespace std::string_literals ;

//======================================================================
const std::map<std::string, UShortValue::item_t> UShortValue::NAMEITEMMAP{
    {"PORT"s,PORT},{"SERVERLANGUAGE"s,LANGUAGE},{"JSENGINESIZE"s,JSENGINE},{"SYSMESSAGECOLOUR"s,SYSMESSAGECOLOR},
    {"APSPERFTHRESHOLD"s,APSPERFTHRESHOLD},{"APSINTERVAL"s,APSINTERVAL},{"APSDELAYSTEP"s,APSDELAYSTEP},{"APSDELAYMAXCAP"s,APSDELAYMAX},
    {"SKILLLEVEL"s,SKILLLEVEL},{"SKILLCAP"s,SKILLCAP},{"SKILLDELAY"s,SKILLDELAY},{"STATCAP"s,STATCAP},
    {"MAXPLAYERPACKITEMS"s,MAXPLAYERPACKITEM},{"MAXPLAYERBANKITEMS"s,MAXPLAYERBANKITEM},{"MAXSAFETELEPORTSPERDAY"s,MAXSAFETELEPORT},
    {"MAXCONTROLSLOTS"s,MAXCONTROLSLOT},{"MAXFOLLOWERS"s,MAXFOLLOWER},{"MAXPETOWNERS"s,MAXPETOWNER},
    {"PETLOYALTYGAINONSUCCESS"s,PETLOYALITYGAINSUCCESS},{"PETLOYALTYLOSSONFAILURE"s,PETLOYALITYLOSSFAILURE},
    {"POSTINGLEVEL"s,POSTINGLEVEL},{"REMOVALLEVEL"s,REMOVALLEVEL},{"DUNGEONLEVEL"s,DUNGEONLIGHT},
    {"BRIGHTLEVEL"s,BRIGHTLEVEL},{"DARKLEVEL"s,DARKLEVEL},{"SECONDSPERUOMINUTE"s,SECONDSPERUOMINUTE},{"BASERANGE"s,BASERANGE},
    {"MAXTARGETS"s,MAXTARGET},{"MSGREDISPLAYTIME"s,MSGREDISPLAYTIME},{"MAXKILLS"s,MAXKILL},{"RESOURCEAREASIZE"s,RESOURCEAREASIZE},
    {"MINECHECK"s,MINECHECK},{"LOGSPERAREA"s,LOGPERAREA},{"PETOFFLINETIMEOUT"s,PETOFFLINETIMEOUT},{"ANIMALATTACKCHANCE"s,ANIMALATTACKCHANCE},{"WEAPONDAMAGEMIN"s,MINWEAPONDAMAGE},
    {"WEAPONDAMAGEMAX"s,MAXWEAPONDAMAGE},{"ARMORDAMAGECHANCE"s,ARMORDAMAGECHANCE},{"ARMORDAMAGEMIN"s,MINARMORDAMAGE},{"ARMORDAMAGEMAX"s,MAXARMORDAMAGE},
    {"PARRYDAMAGECHANCE"s,PARRYDAMAGECHANCE},{"PARRYDAMAGEMIN"s,MINPARRYDAMAGE},{"PARRYDAMAGEMAX"s,MAXPARRYDAMAGE},{"ALCHEMYBONUSMODIFIER"s,ALCHEMENYBONUSMODIFIER},
    {"BLOODEFFECTCHANCE"s,BLOODEFFECTCHANCE},{"WEAPONDAMAGEBONUSTYPE"s,WEAPONDAMAGEBONUSTYPE},{"STARTPRIVS"s,STARTPRIV},
    {"TITLECOLOUR"s,TITLECOLOR},{"LEFTTEXTCOLOUR"s,LEFTTEXTCOLOR},{"RIGHTTEXTCOLOUR"s,RIGHTTEXTCOLOR},
    {"BUTTONCANCEL"s,BUTTONCANCEL},{"BUTTONLEFT"s,BUTTONLEFT},{"BUTTONRIGHT"s,BUTTONRIGHT},{"BACKGROUNDPIC"s,BACKGROUNDPIC},
    {"MAXHOUSESOWNABLE"s,MAXHOUSEOWNABLE},{"MAXHOUSESCOOWNABLE"s,MAXHOUSECOOWNABLE},
    {"OREPERAREA"s,OREPERAREA},{"FISHPERAREA"s,FISHPERAREA},{"WEAPONDAMAGECHANCE"s,WEAPONDAMAGECHANCE},
};
//======================================================================
auto UShortValue::nameFor(item_t setting)-> const std::string &{
    auto iter = std::find_if(NAMEITEMMAP.begin(),NAMEITEMMAP.end(),[setting](const std::pair<std::string,item_t> &entry){
        return setting == entry.second;
    });
    if (iter == NAMEITEMMAP.end()){
        throw std::runtime_error("No name was found for item_t setting: "s+std::to_string(static_cast<int>(setting)));
    }
    return iter->first ;
}

//======================================================================
UShortValue::UShortValue() {
    this->reset();
}
//======================================================================
auto UShortValue::reset() ->void {
    values = std::vector<std::uint16_t>(NAMEITEMMAP.size(),0) ;
    this->operator[](PORT) = 2593 ;
    this->operator[](LANGUAGE) = 0 ;
    this->operator[](JSENGINE) = 256 ;
    this->operator[](SYSMESSAGECOLOR) = 72 ;
    this->operator[](APSPERFTHRESHOLD) = 50 ;
    this->operator[](APSINTERVAL) = 100 ;
    this->operator[](APSDELAYSTEP) = 50 ;
    this->operator[](APSDELAYMAX) = 2000 ;
    this->operator[](SKILLLEVEL) = 7 ;
    this->operator[](SKILLCAP) = 7000 ;
    this->operator[](SKILLDELAY) = 1 ;
    this->operator[](STATCAP) = 225 ;
    this->operator[](MAXPLAYERPACKITEM) = 125 ;
    this->operator[](MAXPLAYERBANKITEM) = 125 ;
    this->operator[](MAXSAFETELEPORT) = 1 ;
    this->operator[](MAXCONTROLSLOT) = 5 ;
    this->operator[](MAXFOLLOWER) = 5 ;
    this->operator[](MAXPETOWNER) = 10 ;
    this->operator[](PETLOYALITYGAINSUCCESS) = 1 ;
    this->operator[](PETLOYALITYLOSSFAILURE) = 1 ;
    this->operator[](POSTINGLEVEL) = 0 ;
    this->operator[](REMOVALLEVEL) = 0 ;
    this->operator[](DUNGEONLIGHT) = 15 ;
    this->operator[](BRIGHTLEVEL) = 0 ;
    this->operator[](DARKLEVEL) = 12 ;
    this->operator[](SECONDSPERUOMINUTE) = 5 ;
    this->operator[](BASERANGE) = 10 ;
    this->operator[](MAXTARGET) = 20 ;
    this->operator[](MSGREDISPLAYTIME) = 3 ;
    this->operator[](MAXKILL) = 4 ;
    this->operator[](RESOURCEAREASIZE) = 8 ;
    this->operator[](MINECHECK) = 1 ;
    this->operator[](LOGPERAREA) = 10 ;
    this->operator[](PETOFFLINETIMEOUT) = 5 ;
    this->operator[](ANIMALATTACKCHANCE) = 2 ;
    this->operator[](WEAPONDAMAGECHANCE) = 17 ;
    this->operator[](MINWEAPONDAMAGE) = 0 ;
    this->operator[](MAXWEAPONDAMAGE) = 1 ;
    this->operator[](ARMORDAMAGECHANCE) = 33 ;
    this->operator[](MINARMORDAMAGE) = 0 ;
    this->operator[](MAXARMORDAMAGE) = 1 ;
    this->operator[](BLOODEFFECTCHANCE) = 25 ;
    this->operator[](WEAPONDAMAGEBONUSTYPE) = 2 ;
    this->operator[](STARTPRIV) = 0 ;
    this->operator[](TITLECOLOR) = 0 ;
    this->operator[](LEFTTEXTCOLOR) = 0 ;
    this->operator[](RIGHTTEXTCOLOR) = 0 ;
    this->operator[](BUTTONCANCEL) = 4017 ;
    this->operator[](BUTTONLEFT) = 4014 ;
    this->operator[](BUTTONRIGHT) = 4005 ;
    this->operator[](BACKGROUNDPIC) = 5054 ;
    this->operator[](MAXHOUSEOWNABLE) = 1 ;
    this->operator[](MAXHOUSECOOWNABLE) = 10 ;
    this->operator[](OREPERAREA) = 10 ;
    this->operator[](FISHPERAREA) = 10 ;
    
}

//======================================================================
auto UShortValue::operator[](item_t item) const -> const std::uint16_t &{
    return values.at(static_cast<size_t>(item)) ;
}
//======================================================================
auto UShortValue::operator[](item_t item) -> std::uint16_t &{
    return values.at(static_cast<size_t>(item)) ;
}

//======================================================================
auto UShortValue::setKeyValue(const std::string &key, const std::string &value) ->bool {
    auto rvalue = false ;
    auto iter = NAMEITEMMAP.find(key) ;
    if (iter != NAMEITEMMAP.end()){
        rvalue = true ;
        values.at(static_cast<size_t>(iter->second)) = static_cast<std::uint16_t>(std::stoul(value,nullptr,0));
    }
    return rvalue ;
}
//======================================================================
auto UShortValue::valueFor(const std::string &value) const -> std::optional<std::uint16_t> {
    auto iter = NAMEITEMMAP.find(value) ;
    if (iter == NAMEITEMMAP.end()){
        return {};
    }
    return  values.at(static_cast<size_t>(iter->second));
}
