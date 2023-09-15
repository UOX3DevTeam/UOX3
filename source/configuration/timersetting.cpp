//

#include "timersetting.hpp"
#include <stdexcept>


using namespace std::string_literals ;

//======================================================================
const std::map<std::string,TimerSetting::timer_t> TimerSetting::NAMETIMERMAP{
    {"CORPSEDECAYTIMER"s,CORPSEDECAY},{"WEATHERTIMER"s,WEATHER},
    {"SHOPSPAWNTIMER"s,SHOPSPAWN},{"DECAYTIMER"s,DECAY},
    {"DECAYTIMERINHOUSE"s,DECAYINHOUSE},{"INVISIBILITYTIMER"s,INVISIBILITY},
    {"OBJECTUSETIMER"s,OBJECTUSAGE},{"GATETIMER"s,GATE},
    {"POISONTIMER"s,POISON},{"LOGINTIMEOUT"s,LOGINTIMEOUT},
    {"HITPOINTREGENTIMER"s,HITPOINTREGEN},{"STAMINAREGENTIMER"s,STAMINAREGEN},
    {"MANAREGENTIMER"s,MANAREGEN},{"BASEFISHINGTIMER"s,FISHINGBASE},
    {"RANDOMFISHINGTIMER"s,FISHINGRANDOM},{"SPIRITSPEAKTIMER"s,SPIRITSPEAK},
    {"HUNGERRATE"s,HUNGERRATE},{"THIRSTRATE"s,THIRSTRATE},
    {"POLYDURATION"s,POLYMORPH},{"ESCORTINITEXPIRE"s,ESCORTWAIT},
    {"ESCORTACTIVEEXPIRE"s,ESCORTACTIVE},{"ESCORTDONEEXPIRE"s,ESCORTDONE},
    {"MURDERDECAYTIMER"s,MURDERDECAY},{"CRIMINALTIMER"s,CRIMINAL},
    {"STEALINGFLAGTIMER"s,STEALINGFLAG},{"AGGRESSORFLAGTIMER"s,AGGRESSORFLAG},
    {"PERMAGREYFLAGTIMER"s,PERMAGREYFLAG},{"IGNORECOMBAT"s,COMBATIGNORE},
    {"PETOFFLINECHECKTIMER"s,PETOFFLINECHECK},{"NPCFLAGUPDATETIMER"s,NPCFLAGUPDATETIMER},
    {"BLOODDECAYTIMER"s,BLOODDECAY},{"BLOODDECAYCORPSETIMER"s,BLOODDECAYCORPSE},
    {"NPCCORPSEDECAYTIMER"s,NPCCORPSEDECAY},{"PETLOYALTYRATE"s,LOYALTYRATE},
    {"BASETIMER"s,TRACKING},{"ORERESPAWNTIMER"s,ORE},
    {"LOGSRESPAWNTIMER"s,LOG},{"FISHRESPAWNTIMER"s,FISH},
    {"POTIONDELAY",POTION}
};
//======================================================================
TimerSetting::TimerSetting() {
    this->reset();
}
//======================================================================
auto TimerSetting::reset() ->void {
    timerValue = std::vector<std::uint16_t>(NAMETIMERMAP.size(),0);
    this->operator[](CORPSEDECAY) = 420 ;
    this->operator[](WEATHER) = 60 ;
    this->operator[](SHOPSPAWN) = 300 ;
    this->operator[](DECAY) = 300 ;
    this->operator[](DECAYINHOUSE) = 3600 ;
    this->operator[](INVISIBILITY) = 60 ;
    this->operator[](OBJECTUSAGE) = 1 ;
    this->operator[](GATE) = 30 ;
    this->operator[](POISON) = 180 ;
    this->operator[](LOGINTIMEOUT) = 300 ;
    this->operator[](HITPOINTREGEN) = 8 ;
    this->operator[](STAMINAREGEN) = 3 ;
    this->operator[](MANAREGEN) = 5 ;
    this->operator[](FISHINGBASE) = 10 ;
    this->operator[](FISHINGRANDOM) = 5;
    this->operator[](SPIRITSPEAK) = 30 ;
    this->operator[](HUNGERRATE) = 6000 ;
    this->operator[](THIRSTRATE) = 6000 ;
    this->operator[](POLYMORPH) = 90 ;
    this->operator[](ESCORTWAIT) = 900 ;
    this->operator[](ESCORTACTIVE) = 600 ;
    this->operator[](ESCORTDONE) = 600 ;
    this->operator[](MURDERDECAY) = 28800 ;
    this->operator[](CRIMINAL) = 120 ;
    this->operator[](STEALINGFLAG) = 120 ;
    this->operator[](AGGRESSORFLAG) = 120 ;
    this->operator[](PERMAGREYFLAG) = 0 ; // We dont need, reset to 0
    this->operator[](COMBATIGNORE) = 20 ;
    this->operator[](PETOFFLINECHECK) = 600 ;
    this->operator[](NPCFLAGUPDATETIMER) = 10 ;
    this->operator[](BLOODDECAY) = 3 ;
    this->operator[](BLOODDECAYCORPSE) = 420 ;
    this->operator[](NPCCORPSEDECAY) = 420 ;
    this->operator[](LOYALTYRATE) = 900 ;
    this->operator[](TRACKING) = 30 ;
    this->operator[](ORE) = 600 ;
    this->operator[](FISH) = 600 ;
    this->operator[](LOG) = 600 ;
    this->operator[](POTION) = 10 ;
}

//======================================================================
auto TimerSetting::operator[](timer_t timer) const -> const std::uint16_t & {
    return timerValue.at(static_cast<int>(timer));
}
//======================================================================
auto TimerSetting::operator[](timer_t timer) -> std::uint16_t& {
    return timerValue.at(static_cast<int>(timer));
}
//======================================================================
auto TimerSetting::size() const ->size_t {
    return timerValue.size();
}

//======================================================================
auto TimerSetting::setKeyValue(const std::string &key, const std::string &value) ->bool {
    auto rvalue = false ;
    auto iter = NAMETIMERMAP.find(key) ;
    if (iter != NAMETIMERMAP.end()){
        rvalue = true ;
        timerValue.at(static_cast<int>(iter->second)) = static_cast<std::uint16_t>(std::stoul(value,nullptr,0));
    }
    return rvalue ;
}
//======================================================================
auto TimerSetting::valueFor(const std::string &key) -> std::optional<std::uint16_t> {
    auto iter = NAMETIMERMAP.find(key) ;
    if (iter == NAMETIMERMAP.end()){
        return {};
    }
    return timerValue.at(static_cast<int>(iter->second)) ;
}
