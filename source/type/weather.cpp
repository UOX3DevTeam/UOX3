//

#include "weather.hpp"

#include <cmath>
#include <stdexcept>

#include "utility/random.hpp"
#include "uotime.hpp"

using namespace std::string_literals ;
using Random = effolkronium::random_static ;

//======================================================================
enum type_t {
    NONE = 0, PHYSICAL, LIGHT, RAIN,
    COLD, HEAT, LIGHTNING, POISON,
    SNOW, STORM, STORMBREW
};
//======================================================================
const std::map<std::string, Weather::type_t> Weather::NAMETYPEMAP {
    {"NONE"s,NONE},{"PHYSICAL"s,PHYSICAL},{"LIGHT"s,LIGHT},{"RAIN"s,RAIN},
    {"COLD"s,COLD},{"HEAT"s,HEAT},{"LIGHTNING"s,LIGHTNING},{"POISON"s,POISON},
    {"SNOW"s,SNOW},{"STORM"s,STORM},{"STORMBREW"s,STORMBREW}
};
//======================================================================
auto Weather::nameForType(type_t type) ->const std::string& {
    auto iter = std::find_if(NAMETYPEMAP.begin(),NAMETYPEMAP.end(),[type](const std::pair<std::string,type_t> &entry){
        return entry.second == type ;
    });
    if (iter != NAMETYPEMAP.end()){
        return iter->first ;
    }
    throw std::out_of_range("No weather type for: "s+std::to_string(static_cast<int>(type)));
}
//======================================================================
auto Weather::typeForName(const std::string &name) -> type_t {
    auto iter = NAMETYPEMAP.find(name) ;
    if (iter != NAMETYPEMAP.end()){
        return iter->second ;
    }
    throw std::out_of_range("No weather name for: "s+name);
}
//======================================================================
auto Weather::size() -> size_t {
    return NAMETYPEMAP.size();
}

//======================================================================
Weather::Weather():snowThreshold(0.0),rainTempDrop(0.0),stormTempDrop(0.0),stormDelay(false) {
 
    weather = std::vector<Characteristic>(Weather::size()) ;
    // Chances
    weather[COLD].chance = 5 ;
    weather[RAIN].chance = 10 ;
    weather[SNOW].chance = 1 ;
    weather[STORM].chance = 1 ;
    // States
    impact[BRIGHTNESS][MAX] = 300.0;
    impact[BRIGHTNESS][MIN] = 300.0 ;
    impact[BRIGHTNESS][CURRENT] = 300.0 ;
    impact[TEMP][MAX] = 30.0;
    impact[TEMP][MIN] = 5.0 ;
    
    rainTempDrop = 5.0 ;
    stormTempDrop = 10.0 ;
    
    weather[COLD][MAX] = 0 ;
    weather[HEAT][MAX] = 35 ;
    weather[STORM][MAX] = 100 ;
    weather[SNOW][MAX] = 100 ;
    weather[RAIN][MAX] = 100 ;
    
    impact[WIND][MAX] = 0 ;
    impact[WIND][MIN] = 0 ;
    impact[TEMP][CURRENT] =  15 ;
    snowThreshold = 15.0 ;
}
//======================================================================
auto Weather::update(const UOTime &uotime) ->void {
    auto hour = static_cast<float>(uotime.hours) ;
    auto minute = static_cast<float>(uotime.minutes) ;
    auto ampm = uotime.ampm ;
    
    if (stormDelay && weather[STORMBREW].active){
        weather[STORM].active = true ;
    }
    else if (weather[STORMBREW].active){
        stormDelay = true ;
    }
    
    if (impact[BRIGHTNESS][MAX] < 300.0 && impact[BRIGHTNESS][MIN] < 300.0){
        auto hourIncrement = std::fabs((impact[BRIGHTNESS][MAX] - impact[BRIGHTNESS][MIN])/12.0f);
        // We use the amount to subtract from lightMax in the morning and add to lightMin in the evening.
        auto minuteIncrement = hourIncrement / 60.0f ;
        auto tempLight = hour + minuteIncrement*minute ;
        if (ampm){
            impact[BRIGHTNESS][CURRENT] = impact[BRIGHTNESS][MIN] + tempLight;
        }
        else {
            impact[BRIGHTNESS][CURRENT] = impact[BRIGHTNESS][MAX] - tempLight;

        }
    }
    // Now figure out the effective temp
    auto tempHourIncrement = std::fabs((impact[TEMP][MAX] - impact[TEMP][MIN])/12.0f) ;
    auto tempMinuteIncrement = tempHourIncrement/60.0f ;
    auto tempLightChange = (tempHourIncrement * hour) + (minute * tempMinuteIncrement);
    if (ampm){
        impact[TEMP][CURRENT] =  impact[EFFECTIVETEMP][MAX] - tempLightChange ;
    }
    else {
        impact[TEMP][CURRENT] =  impact[EFFECTIVETEMP][MIN] + tempLightChange ;

    }
    
    if (weather[STORM].active){
        impact[TEMP][CURRENT]  -= stormTempDrop ;
        if (impact[TEMP][CURRENT] < 0.0){
            impact[TEMP][CURRENT] =  0.0;
        }
    }
    else if (weather[RAIN].active){
        impact[TEMP][CURRENT]  -= rainTempDrop ;
        if (impact[TEMP][CURRENT] < 0.0){
            impact[TEMP][CURRENT] =  0.0;
        }
    }
}
//======================================================================
auto Weather::newDay() ->void {
    
    auto isHeatWave = false ;
    auto isColdDay = false ;
    
    auto effMax = impact[TEMP][MAX];
    auto effMin = impact[TEMP][MIN];
    
    if ( Random::get(1,100) <= static_cast<int>(weather[HEAT].chance)) {
        isHeatWave = true ;
        auto currentTemp = Random::get(effMax,static_cast<float>(weather[HEAT][MAX])) ;
        effMax = currentTemp ;
        effMin = currentTemp ;
    }
    else if (Random::get(1,100) <= static_cast<int>(weather[COLD].chance) ) {
        isColdDay = true ;
        auto currentTemp = Random::get(effMin,static_cast<float>(weather[COLD][MAX])) ;
        effMax = currentTemp ;
        effMin = currentTemp ;
    }
    weather[HEAT].active = isHeatWave ;
    weather[COLD].active = isColdDay ;
    impact[EFFECTIVETEMP][MAX] = effMax ;
    impact[EFFECTIVETEMP][MIN] = effMax ;

}
//======================================================================
auto Weather::newHour() ->void {
    auto isSnowing = false ;
    auto isRaining = false ;
    auto isStorm = false ;
    if ( Random::get(1,100) <= static_cast<int>(weather[STORM].chance)) {
        isStorm = true ;
    }
    else if (Random::get(1,100) <= static_cast<int>(weather[SNOW].chance) ) {
        isSnowing = true ;
    }
    else if (Random::get(1,100) <= static_cast<int>(weather[RAIN].chance) ) {
        isRaining = true ;
    }
    if (!isStorm){
        stormDelay = false ;
        weather[STORM].chance = false ;

    }
    weather[RAIN].chance = isRaining ;
    weather[SNOW].chance = isSnowing ;
    weather[RAIN][CURRENT] = Random::get(weather[RAIN][MAX],weather[RAIN][MIN]);
    weather[SNOW][CURRENT] = Random::get(weather[SNOW][MAX],weather[SNOW][MIN]);
    weather[STORM][CURRENT] = Random::get(weather[STORM][MAX],weather[STORM][MIN]);
    weather[STORMBREW].active =  isStorm ;
}
