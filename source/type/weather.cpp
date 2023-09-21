//

#include "weather.hpp"

#include <cmath>
#include <fstream>
#include <stdexcept>

#include "cbaseobject.h"
#include "cchar.h"
#include "citem.h"
#include "cpacketsend.h"
#include "csocket.h"
#include "funcdecl.h"
#include "utility/random.hpp"
#include "utility/strutil.hpp"
#include "townregion.h"

#include "uotime.hpp"

using namespace std::string_literals ;
using Random = effolkronium::random_static ;

//======================================================================
// Weather (the weather attributes )
//======================================================================

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

//======================================================================
auto Weather::setWeatherRange(type_t weathertype, const std::string &value) ->void {
    auto [low,high] = util::split(value,",") ;
    if (high.empty()){
        weather.at(weathertype)[MAX] = std::stoi(low,nullptr,0);
    }
    else {
        weather.at(weathertype)[MAX] = std::stoi(high,nullptr,0);
        weather.at(weathertype)[MIN] = std::stoi(low,nullptr,0);
    }
}
//======================================================================
auto Weather::setWeatherRange(type_t weathertype, state_t state, const std::string &value) ->void {
        weather.at(weathertype)[state] = std::stoi(value,nullptr,0);
}

//======================================================================
auto Weather::setChance(type_t weathertype, const std::string &value) ->void {
    weather.at(weathertype).chance = std::stoi(value,nullptr,0);
}
//======================================================================
auto Weather::setImpact(impact_t impact,state_t state, const std::string &value) {
    this->impact[impact][state] = std::stof(value) ;
}

//======================================================================
auto Weather::setValue(const std::string &line) ->void {
    auto [key,value] = util::split(line,"=");
    auto ukey = util::upper(key);
    switch(ukey[0]){
        case 'C': {
            if (ukey == "COLDCHANCE"){
                setChance(COLD, value) ;
            }
            else if (ukey == "COLDINTENSITY") {
                // Why isn't cold the same as all the other(low,high), who knows
                // Just to be sure, we will parse it and check
                setWeatherRange(COLD, value);
            }
            break;
        }
        case 'H': {
            if (ukey == "HEATCHANCE") {
                setChance(HEAT, value) ;
            }
            else if (ukey == "HEATINTENSITY") {
                // Why isn't cold the same as all the other(low,high), who knows
                // Just to be sure, we will parse it and check
                setWeatherRange(HEAT, value);
            }
            break;
        }
        case 'L':{
            if (ukey == "LIGHTMIN"){
                setImpact(BRIGHTNESS, MIN, value);
            }
            else if (ukey == "LIGHTMAX"){
                setImpact(BRIGHTNESS, MAX, value);
            }
            break;
        }
        case 'M': {
            if (ukey == "MAXTEMP") {
                setImpact(TEMP, MAX, value);
            }
            else if (ukey == "MINTEMP") {
                setImpact(TEMP, MIN, value);

            }
            else if (ukey == "MAXWIND") {
                setImpact(WIND, MAX, value);
            }
            else if (ukey == "MINWIND") {
                setImpact(WIND, MIN, value);
            }
            break;
        }
        case 'R': {
            if (ukey == "RAINCHANCE") {
                setChance(RAIN, value);
            }
            else if (ukey == "RAININTENSITY") {
                setWeatherRange(RAIN, value);

            }
            else if (ukey == "RAINTEMPDROP") {
                rainTempDrop = std::stof(value);
            }
            break;
        }
        case 'S': {
            if (ukey == "SNOWCHANCE") {
                setChance(SNOW, value);
            }
            else if (ukey == "SNOWINTENSITY") {
                setWeatherRange(SNOW, value);

            }
            else if (ukey == "SNOWTHRESHOLD") {
                snowThreshold = std::stof(value);
            }
            else if (ukey == "STORMCHANCE") {
                setChance(STORM, value);
            }
            else if (ukey == "STORMINTENSITY") {
                setWeatherRange(STORM, value);
            }
            else if (ukey == "STORMTEMPDROP") {
                stormTempDrop = std::stof(value);
            }
            break;
        }
   }
    
}
//=====================================================================
// WorldWeather (the weather for all the regions of the world
//======================================================================
WorldWeather::WorldWeather() {
    region = std::vector<Weather>() ;
}
//======================================================================
auto WorldWeather::newDay() ->void {
    std::for_each(region.begin(),region.end(),[](Weather &entry){
        entry.newDay();
    });
}
//======================================================================
auto WorldWeather::newHour() ->void {
    std::for_each(region.begin(),region.end(),[](Weather &entry){
        entry.newHour();
    });
}
//======================================================================
auto WorldWeather::update(const UOTime &uotime) ->void {
    std::for_each(region.begin(),region.end(),[&uotime](Weather &entry){
        entry.update(uotime);
    });

}

//======================================================================
auto WorldWeather::load(const std::filesystem::path &path) ->bool {
    region.clear();
    auto input = std::ifstream(path.string());
    if (!input.is_open()){
        return false ;
    }
    auto buffer = std::vector<char>(4096,0) ;
    auto regNumber = -1  ;
    while(input.good() && !input.eof()){
        input.getline(buffer.data(), buffer.size()-1);
        if (input.gcount()>0){
            buffer[input.gcount()] = 0 ;
            std::string line = buffer.data();
            line = util::trim(util::strip(line,"//"));
            if (!line.empty()){
                if (line[0] == '}' && regNumber != -1){
                     regNumber = -1 ;
                 }
                else if(line[0] == '[') {
                    auto contents = util::simplify(util::contentsOf(line, "[", "]"));
                    auto [key,num] = util::split(contents," ") ;
                    if (util::upper(key) == "WEATHERAB"){
                        regNumber = std::stoi(num,nullptr,0);
                        if (regNumber >= region.size()){
                            region.resize(regNumber+1);
                        }
                    }
                }
                else if( regNumber!= -1) {
                    // We only care if we are in a region we care about.
                    region.at(regNumber).setValue(line);
                }
            }
        }
    }
    return true ;
}
/*
//======================================================================
//Updates weather effects for players when light levels change,or depending on whether player is inside or outside of
auto WorldWeather::doPlayerStuff(CSocket *mSock, CChar *p)->bool {
    auto rvalue = true ;
    if (!ValidateObject(p) || p->IsNpc()){
        return true ;
    }
    auto defaultTemp = 20 ;
    auto curregion = p->GetRegion()->GetWeather() ;
    if (curregion > this->region.size() || region.empty() || p->InBuilding()){
        // If in a building, invalid regino, or no regions loaded weather wise
        if (mSock != nullptr){
            CPWeather dry(0xFF,0x00,defaultTemp) ;
            mSock->Send(&dry) ;
            if (p->GetWeathDamage(Weather::SNOW) != 0) {
                p->SetWeathDamage(0, Weather::SNOW);
            }
            if (p->GetWeathDamage(Weather::RAIN) != 0) {
                p->SetWeathDamage(0, Weather::RAIN);
            }
            if (p->GetWeathDamage(Weather::COLD) != 0) {
                p->SetWeathDamage(0, Weather::COLD);
            }
            if (p->GetWeathDamage(Weather::HEAT) != 0) {
                p->SetWeathDamage(0, Weather::HEAT);
            }
            SendJSWeather(p, Weather::LIGHT, defaultTemp);
        }
        rvalue = false ;
    }
    else {
        auto isStorm = (this->region.at(curregion))[Weather::STORM].active ;
        auto brewStorm = (this->region.at(curregion))[Weather::STORMBREW].active;
        auto isSnowing = (this->region.at(curregion))[Weather::SNOW].active;
        auto isRaining = (this->region.at(curregion))[Weather::RAIN].active;
        auto temperature = (this->region.at(curregion)).impact[Weather::TEMP][Weather::CURRENT] ;
        if (isStorm) {
            DoPlayerWeather(mSock, 5, static_cast<std::int8_t>(temperature), curregion);
            if (p->GetWeathDamage(Weather::STORM) != 0) {
                //p->SetWeathDamage(<#timerval_t newValue#>, <#std::uint8_t part#>)
            }
        }
    }
    return rvalue ;
}
*/
