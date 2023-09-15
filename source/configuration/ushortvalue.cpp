//

#include "ushortvalue.hpp"
#include <stdexcept>


using namespace std::string_literals ;

//======================================================================
const std::map<std::string, UShortValue::item_t> UShortValue::NAMEITEMMAP{
    {"PORT"s,PORT},{"SERVERLANGUAGE"s,LANGUAGE},{"JSENGINESIZE"s,JSENGINE},{"SYSMESSAGECOLOUR"s,SYSMESSAGECOLOR},
    {"APSPERFTHRESHOLD"s,APSPERFTHRESHOLD},{"APSINTERVAL"s,APSINTERNAL},{"APSDELAYSTEP"s,APSDELAYSTEP},{"APSDELAYMAXCAP"s,APSDELAYMAX},
    {"SKILLLEVEL"s,SKILLLEVEL},{"SKILLCAP"s,SKILLCAP},{"SKILLDELAY"s,SKILLDELAY},{"STATCAP"s,STATCAP},
    {"MAXPLAYERPACKITEMS"s,MAXPLAYERPACKITEM},{"MAXPLAYERBANKITEMS"s,MAXPLAYERBANKITEM},{"MAXSAFETELEPORTSPERDAY"s,MAXSAFETELEPORT},
    {"MAXCONTROLSLOTS"s,MAXCONTROLSLOT},{"MAXFOLLOWERS"s,MAXFOLLOWER},{"MAXPETOWNERS"s,MAXPETOWNER},
    {"PETLOYALTYGAINONSUCCESS"s,PETLOYALITYGAINSUCCESS},{"PETLOYALTYLOSSONFAILURE"s,PETLOYALITYGAINFAILURE},
    {"POSTINGLEVEL"s,POSTINGLEVEL},{"REMOVALLEVEL"s,REMOVALLEVEL},{"DUNGEONLEVEL"s,DUNGEONLIGHT},
    {"BRIGHTLEVEL"s,BRIGHTLEVEL},{"DARKLEVEL"s,DARKLEVEL},{"SECONDSPERUOMINUTE"s,SECONDSPERUOMINUTE},{"BASERANGE"s,BASERANGE},
    {"MAXTARGETS"s,MAXTARGET},{"MSGREDISPLAYTIME"s,MSGREDISPLAYTIME},{"MAXKILLS"s,MAXKILL},{"RESOURCEAREASIZE"s,RESOURCEAREASIZE},
    {"MINECHECK"s,MINECHECK},{"LOGSPERAREA"s,LOGSPERAREA},{"PETOFFLINETIMEOUT"s,PETOFFLINETIMEOUT},{"ANIMALATTACKCHANCE"s,ANIMALATTACKCHANCE},{"WEAPONDAMAGEMIN"s,MINWEAPONDAMAGE},
    {"WEAPONDAMAGEMAX"s,MAXWEAPONDAMAGE},{"ARMORDAMAGECHANCE"s,ARMORDAMAGECHANCE},{"ARMORDAMAGEMIN"s,MINARMORDAMAGE},{"ARMORDAMAGEMAX"s,MAXARMORDAMAGE},
    {"PARRYDAMAGECHANCE"s,PARRYDAMAGECHANCE},{"PARRYDAMAGEMIN"s,MINPARRYDAMAGE},{"PARRYDAMAGEMAX"s,MAXPARRYDAMAGE},{"ALCHEMYBONUSMODIFIER"s,ALCHEMENYBONUSMODIFIER},
    {"BLOODEFFECTCHANCE"s,BLOODEFFECTCHANCE},{"WEAPONDAMAGEBONUSTYPE"s,WEAPONDAMAGEBONUSTYPE},{"STARTPRIVS"s,STARTPRIV},
    {"TITLECOLOUR"s,TITLECOLOR},{"LEFTTEXTCOLOUR"s,LEFTTEXTCOLOR},{"RIGHTTEXTCOLOUR"s,RIGHTTEXTCOLOR},
    {"BUTTONCANCEL"s,BUTTONCANCEL},{"BUTTONLEFT"s,BUTTONLEFT},{"BUTTONRIGHT"s,BUTTONRIGHT},{"BACKGROUNDPIC"s,BACKGROUNDPIC},
    {"MAXHOUSESOWNABLE"s,MAXHOUSEOWNABLE},{"MAXHOUSESCOOWNABLE"s,MAXHOUSECOOWNABLE}
};
//======================================================================
UShortValue::UShortValue() {
    this->reset();
}
//======================================================================
auto UShortValue::reset() ->void {
    values = std::vector<std::uint16_t>(NAMEITEMMAP.size(),0) ;
    
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
