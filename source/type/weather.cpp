//

#include "weather.hpp"

#include <cmath>
#include <fstream>
#include <stdexcept>

#include "cbaseobject.h"
#include "cchar.h"
#include "citem.h"
#include "combat.h"
#include "cpacketsend.h"
#include "craces.h"
#include "cscript.h"
#include "csocket.h"
#include "ceffects.h"
#include "funcdecl.h"
#include "cjsmapping.h"
#include "utility/random.hpp"
#include "configuration/serverconfig.hpp"
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

//======================================================================
//Updates weather effects for players when light levels change,or depending on whether player is inside or outside of
auto WorldWeather::doPlayerStuff(CSocket *mSock, CChar *p)->bool {
    auto rvalue = true ;
    if (!ValidateObject(p) || p->IsNpc()){
        return rvalue ;
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
            sendJSWeather(p, Weather::LIGHT, defaultTemp);
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
            doPlayerWeather(mSock, 5, static_cast<std::int8_t>(temperature), curregion);
            if (p->GetWeathDamage(Weather::STORM) == 0) {
                p->SetWeathDamage(static_cast<std::uint32_t>(BuildTimeValue(static_cast<float>(Races->Secs(p->GetRace(), Weather::STORM)))),Weather::STORM);
            }
            if (p->GetWeathDamage(Weather::SNOW) != 0) {
                p->SetWeathDamage(0, Weather::SNOW);
            }
            if (p->GetWeathDamage(Weather::RAIN) != 0) {
                p->SetWeathDamage(0, Weather::RAIN);
            }
            sendJSWeather(p, Weather::STORM, temperature);
        }
        else if (brewStorm) {
            doPlayerWeather(mSock, 4, temperature, curregion);
        }
        else if (isSnowing &&  region.at(curregion).snowThreshold > region.at(curregion).impact[Weather::TEMP][Weather::CURRENT]) {
            doPlayerWeather(mSock, 2, temperature, curregion);
            if (p->GetWeathDamage(Weather::SNOW) == 0) {
                p->SetWeathDamage(static_cast<std::uint32_t>(BuildTimeValue(static_cast<float>(Races->Secs(p->GetRace(), Weather::SNOW)))),Weather::SNOW);
            }
            if (p->GetWeathDamage(Weather::STORM) != 0) {
                p->SetWeathDamage(0, Weather::STORM);
            }
            if (p->GetWeathDamage(Weather::RAIN) != 0) {
                p->SetWeathDamage(0, Weather::RAIN);
            }
            sendJSWeather(p, Weather::STORM, temperature);
        }
        else if (isRaining) {
            doPlayerWeather(mSock, 1, temperature, curregion);
            if (p->GetWeathDamage(Weather::RAIN) == 0) {
                p->SetWeathDamage(static_cast<std::uint32_t>(BuildTimeValue(static_cast<float>(Races->Secs(p->GetRace(), Weather::RAIN)))),Weather::RAIN);
            }
            if (p->GetWeathDamage(Weather::SNOW) != 0) {
                p->SetWeathDamage(0, Weather::SNOW);
            }
            if (p->GetWeathDamage(Weather::STORM) != 0) {
                p->SetWeathDamage(0, Weather::STORM);
            }
            sendJSWeather(p, Weather::STORM, temperature);
        }
        else {
            doPlayerWeather(mSock, 0, temperature, curregion);
            if (p->GetWeathDamage(Weather::SNOW) != 0) {
                p->SetWeathDamage(0, Weather::SNOW);
            }
            if (p->GetWeathDamage(Weather::STORM) != 0) {
                p->SetWeathDamage(0, Weather::STORM);
            }
            if (p->GetWeathDamage(Weather::RAIN) != 0) {
                p->SetWeathDamage(0, Weather::RAIN);
            }
            sendJSWeather(p, Weather::STORM, temperature);
        }
        if ((Races->Affect(p->GetRace(), Weather::HEAT)) && p->GetWeathDamage(Weather::HEAT) == 0) {
            p->SetWeathDamage(static_cast<std::uint32_t>(BuildTimeValue(static_cast<float>(Races->Secs(p->GetRace(), Weather::HEAT)))),Weather::HEAT);
        }
        
        if ((Races->Affect(p->GetRace(), Weather::COLD)) && p->GetWeathDamage(Weather::COLD) == 0) {
            p->SetWeathDamage(static_cast<std::uint32_t>(BuildTimeValue(static_cast<float>(Races->Secs(p->GetRace(), Weather::COLD)))),Weather::COLD);
        }
    }
    return rvalue ;
}

//======================================================================
auto WorldWeather::doPlayerWeather(CSocket *s, std::uint8_t weathType, std::int8_t currentTemp, weathid_t currval) ->void{
    // Weather Types
    // 0 - dry
    // 1 - rain
    // 2 - snow
    // 3 - rain and snow
    // 4 - storm is brewing
    // 5 - storm
    //    Byte 1 - 0x65 (Weather)
    //    Byte 2 - Weather Type(  0, rain, 1, fierce storm, 2, snow, 3 a storm is brewing, and 0xFF
    // none )     Byte 3 - Particle count (upper limit of 70)     Byte 4 - Temperature
    
    if (s == nullptr)
        return;
    
    CPWeather dry(0xFF, 0x00, currentTemp);
    CPWeather rain(0x00, static_cast<std::uint8_t>(this->region.at(currval)[Weather::RAIN][Weather::CURRENT]), currentTemp);
    CPWeather storm(0x01, static_cast<std::uint8_t>(this->region.at(currval)[Weather::STORM][Weather::CURRENT]), currentTemp);
    CPWeather snow(0x02, static_cast<std::uint8_t>(this->region.at(currval)[Weather::SNOW][Weather::CURRENT]), currentTemp);
    CPWeather strmbrw(0x03, static_cast<std::uint8_t>((this->region.at(currval)[Weather::STORMBREW][Weather::CURRENT] / 2)), currentTemp);
    
    CChar *mChar = s->CurrcharObj();
    s->Send(&dry);
    
    switch (weathType) {
        case 0:
            break;
        case 1:
            s->Send(&rain);
            break;
        case 2:
            Effects->PlaySound(mChar, 0x14 + RandomNum(0, 1));
            s->Send(&snow);
            break;
        case 3:
            s->Send(&rain);
            Effects->PlaySound(mChar, 0x14 + RandomNum(0, 1));
            s->Send(&snow);
            break;
        case 4:
            Effects->PlaySound(mChar, 0x14 + RandomNum(0, 1));
            s->Send(&strmbrw);
            break;
        case 5:
            Effects->PlaySound(mChar, 0x14 + RandomNum(0, 1));
            s->Send(&storm);
            break;
        default:
            break;
    }
    
}
//======================================================================
// Handles effects of weather on characters affected by weather
auto WorldWeather::doWeatherEffect(CSocket *mSock, CChar &mChar, Weather::type_t element) ->bool{
    auto resistElement = element;
    
    if (element == Weather::NONE || element == Weather::LIGHT || static_cast<int>(element) >= Weather::numberweather)
        return false;
    
    if (mChar.IsInvulnerable() || mChar.IsDead() || !Races->Affect(mChar.GetRace(), element) || mChar.InBuilding())
        return false;
    
    bool didDamage = false;
    auto weatherSys = mChar.GetRegion()->GetWeather();
    if (!(weatherSys > region.size() || region.empty()) && mChar.GetWeathDamage(element) != 0 && mChar.GetWeathDamage(element) <= cwmWorldState->GetUICurrentTime()) {
        auto tempCurrent = (this->region.at(weatherSys)).impact[Weather::TEMP][Weather::CURRENT] ;
        // const float tempMax        = MaxTemp( weatherSys );
        // const float tempMin        = MinTemp( weatherSys );
        // const float tempSnowMax    = SnowThreshold( weatherSys );
        const float tempEffMax = (this->region.at(weatherSys)).impact[Weather::EFFECTIVETEMP][Weather::Weather::MAX];
        const float tempEffMin = (this->region.at(weatherSys)).impact[Weather::EFFECTIVETEMP][Weather::Weather::MIN];
        
        float damageModifier = 0;
        std::int32_t damage = 0;
        float baseDamage = static_cast<float>(Races->Damage(mChar.GetRace(), element));
        float heatLevel = static_cast<float>(Races->HeatLevel(mChar.GetRace()));
        float coldLevel = static_cast<float>(Races->ColdLevel(mChar.GetRace()));
        
        std::int32_t damageMessage = 0;
        std::uint16_t damageAnim = 0x373A;
        
        if (element == Weather::RAIN) {
            damageModifier = static_cast<float>((this->region.at(weatherSys))[Weather::RAIN][Weather::CURRENT]);
            damage = static_cast<std::int32_t>(std::round((baseDamage / 100) * damageModifier));
            damageMessage = 1219;
            resistElement = Weather::NONE;
        }
        
        if (element == Weather::SNOW) {
            damageModifier = static_cast<float>((this->region.at(weatherSys))[Weather::SNOW][Weather::CURRENT]);
            damage = static_cast<std::int32_t>(std::round((baseDamage / 100) * damageModifier));
            damageMessage = 1220;
            // Snow is also cold damage when it comes to resistance values
            resistElement = Weather::COLD;
        }
        
        if (element == Weather::STORM) {
            damageModifier = static_cast<float>((this->region.at(weatherSys))[Weather::STORM][Weather::CURRENT]);
            damage = static_cast<std::int32_t>(std::round((baseDamage / 100) * damageModifier));
            damageMessage = 1775;
            resistElement = Weather::NONE;
            
            if (Races->Affect(mChar.GetRace(), Weather::LIGHTNING)) {
                if (static_cast<std::uint8_t>(RandomNum(1, 100)) <=  Races->Secs(mChar.GetRace(), Weather::LIGHTNING)) {
                    damage = static_cast<std::int32_t>(Races->Damage(mChar.GetRace(), Weather::LIGHTNING));
                    Effects->Bolteffect(&mChar);
                    damageMessage = 1777;
                    damageAnim = 0x0;
                    resistElement = Weather::LIGHTNING;
                }
            }
        }
        
        if (element == Weather::COLD && tempCurrent <= coldLevel) {
            if ((coldLevel - tempEffMin) != 0) {
                damageModifier = ((tempCurrent - tempEffMin) / (coldLevel - tempEffMin));
            }
            else {
                damageModifier = 0;
            }
            
            damage = static_cast<std::int32_t>(std::round(baseDamage - (baseDamage * damageModifier)));
            damageMessage = 1776; // The freezing cold hurts you!
        }
        
        if (element == Weather::HEAT && tempCurrent >= heatLevel) {
            if ((tempEffMax - heatLevel) != 0) {
                damageModifier = ((tempCurrent - heatLevel) / (tempEffMax - heatLevel));
            }
            else {
                damageModifier = 0;
            }
            
            damage = static_cast<std::int32_t>(std::round(baseDamage * damageModifier));
            damageMessage = 1221; // The sheer heat sucks at your body, draining it of moisture!
            damageAnim = 0x3709;
        }
        
        damage = Combat->ApplyDefenseModifiers(resistElement, nullptr, &mChar, 0, 0, damage, true);
        
        if (damage > 0) {
            if (mChar.Damage(damage, element)) {
                mChar.SetStamina(mChar.GetStamina() - 2);
                if (mSock != nullptr) {
                    mSock->SysMessage(damageMessage);
                }
                if (damageAnim != 0x0) {
                    Effects->PlayStaticAnimation((&mChar), damageAnim, 0x09, 0x19);
                }
                Effects->PlaySound((&mChar), 0x0208);
                didDamage = true;
            }
        }
        mChar.SetWeathDamage(static_cast<std::uint32_t>(BuildTimeValue(static_cast<float>(Races->Secs(mChar.GetRace(), element)))), element);
    }
    return didDamage;
    
}
//======================================================================
// Handles effects of light levels on characters affected by light
auto WorldWeather::doLightEffect(CSocket *mSock, CChar &mChar)->bool{
    auto didDamage = false;
    
    if (mChar.IsInvulnerable() || mChar.IsDead() || !Races->Affect(mChar.GetRace(), Weather::LIGHT) || mChar.InBuilding()) {
        return false;
    }
    
    if (mChar.GetWeathDamage(LIGHT) != 0 && mChar.GetWeathDamage(LIGHT) <= cwmWorldState->GetUICurrentTime()) {
        float damageModifier = 0;
        std::int32_t damage = 0;
        float baseDamage = static_cast<float>(Races->Damage(mChar.GetRace(), Weather::LIGHT));
        float lightLevel = static_cast<float>(Races->LightLevel(mChar.GetRace()));
        float currentLight = 255;
        float lightMin = 255;
        float lightMax = 255;
        std::int32_t message = 0;
        bool ampm = cwmWorldState->uoTime.ampm;
        
        weathid_t weatherSys = mChar.GetRegion()->GetWeather();
        if (!region.empty() && weatherSys < region.size()) {
            lightMin = region.at(weatherSys).impact[Weather::BRIGHTNESS][Weather::MIN];
            lightMax = region.at(weatherSys).impact[Weather::BRIGHTNESS][Weather::MAX];
            
            if (lightMin < 300 && lightMax < 300) {
                currentLight = region.at(weatherSys).impact[Weather::BRIGHTNESS][Weather::CURRENT];
            }
            else {
                currentLight = cwmWorldState->uoTime.worldLightLevel ;
                lightMin = ServerConfig::shared().ushortValues[UShortValue::DARKLEVEL] ;
                lightMax = ServerConfig::shared().ushortValues[UShortValue::BRIGHTLEVEL] ;
            }
        }
        else {
            currentLight = cwmWorldState->uoTime.worldLightLevel;
            lightMin = ServerConfig::shared().ushortValues[UShortValue::DARKLEVEL] ;
            lightMax = ServerConfig::shared().ushortValues[UShortValue::BRIGHTLEVEL] ;
        }
        
        if (mChar.InDungeon()) {
            float dungeonLight = 255;
            dungeonLight = ServerConfig::shared().ushortValues[UShortValue::DUNGEONLIGHT] ;
            
            if (lightLevel > dungeonLight) {
                if (lightLevel > 0) {
                    damageModifier = (dungeonLight / lightLevel);
                }
                else {
                    damageModifier = 0;
                }
                
                damage = static_cast<std::int32_t>(std::round(baseDamage - (baseDamage * damageModifier)));
                
                if (std::round(damageModifier) > 0) {
                    message = 1216; // You are scalded by the fury of the light!
                }
                else {
                    message = 1217; // You are burnt by the light's fury!
                }
            }
            else if (lightLevel == currentLight) {
                if (ampm) {
                    message = 1218; // The sun will set soon!
                }
                else {
                    message = 1215; // The sun will rise soon!
                }
            }
        }
        else {
            if (lightLevel > currentLight) {
                if (lightLevel > 0) {
                    damageModifier = (currentLight / lightLevel);
                }
                else {
                    damageModifier = 0;
                }
                
                damage = static_cast<std::int32_t>(std::round(baseDamage - (baseDamage * damageModifier)));
                
                if (std::round(damageModifier) > 0) {
                    message = 1216; // You are scalded by the fury of the light!
                }
                else {
                    message = 1217; // You are burnt by the light's fury!
                }
            }
            else if (lightLevel == currentLight) {
                if (ampm) {
                    message = 1218; // The sun will set soon!
                }
                else {
                    message = 1215; // The sun will rise soon!
                }
            }
        }
        
        damage = Combat->ApplyDefenseModifiers(Weather::LIGHT, nullptr, &mChar, 0, 0, damage, true);
        
        if (damage > 0) {
            if (mChar.Damage(damage, Weather::LIGHT)) {
                Effects->PlayStaticAnimation((&mChar), 0x3709, 0x09, 0x19);
                Effects->PlaySound((&mChar), 0x0208);
                didDamage = true;
                
                if (message != 0 && mSock != nullptr) {
                    mSock->SysMessage(message);
                }
            }
        }
        mChar.SetWeathDamage(static_cast<std::uint32_t>(BuildTimeValue( static_cast<float>(Races->Secs(mChar.GetRace(), Weather::LIGHT)))), Weather::LIGHT);
    }
    return didDamage;
    
}
//======================================================================
// Updates weather effects for NPCs when light levels change, or depending on whether NPC is inside or outside of buildings
auto WorldWeather::doNPCStuff(CChar *p)->bool{
    if (!ValidateObject(p))
        return true;
    
    auto currval = p->GetRegion()->GetWeather();
    if (currval > region.size() || region.empty() || p->InBuilding()) {
        sendJSWeather(p, Weather::LIGHT, 0);
        return false;
    }
    auto isStorm = this->region.at(currval)[Weather::STORM].active ;
    auto isSnowing = this->region.at(currval)[Weather::SNOW].active ;
    auto isRaining = this->region.at(currval)[Weather::RAIN].active ;
    auto temp = static_cast<std::int8_t>(this->region.at(currval).impact[Weather::TEMP][Weather::CURRENT]) ;
    
    if (isStorm) {
        sendJSWeather(p, Weather::STORM, temp);
        if (p->GetWeathDamage(Weather::STORM) == 0) {
            p->SetWeathDamage(static_cast<std::uint32_t>(BuildTimeValue(static_cast<float>(Races->Secs(p->GetRace(), Weather::STORM)))),Weather::STORM);
        }
        if (p->GetWeathDamage(Weather::SNOW) != 0) {
            p->SetWeathDamage(0, Weather::SNOW);
        }
        if (p->GetWeathDamage(Weather::RAIN) != 0) {
            p->SetWeathDamage(0, Weather::RAIN);
        }
    }
    else if (isSnowing && region.at(currval).snowThreshold > this->region.at(currval).impact[Weather::TEMP][Weather::CURRENT]) {
        sendJSWeather(p, Weather::SNOW, temp);
        if (p->GetWeathDamage(Weather::SNOW) == 0) {
            p->SetWeathDamage(static_cast<std::uint32_t>(BuildTimeValue(static_cast<float>(Races->Secs(p->GetRace(), Weather::SNOW)))),Weather::SNOW);
        }
        if (p->GetWeathDamage(Weather::STORM) != 0) {
            p->SetWeathDamage(0, Weather::STORM);
        }
        if (p->GetWeathDamage(Weather::RAIN) != 0) {
            p->SetWeathDamage(0, Weather::RAIN);
        }
    }
    else if (isRaining) {
        sendJSWeather(p, Weather::RAIN, temp);
        if (p->GetWeathDamage(Weather::RAIN) == 0) {
            p->SetWeathDamage(static_cast<std::uint32_t>(BuildTimeValue(static_cast<float>(Races->Secs(p->GetRace(), Weather::RAIN)))),Weather::RAIN);
        }
        if (p->GetWeathDamage(Weather::SNOW) != 0) {
            p->SetWeathDamage(0, Weather::SNOW);
        }
        if (p->GetWeathDamage(Weather::STORM) != 0) {
            p->SetWeathDamage(0, Weather::STORM);
        }
    }
    else {
        sendJSWeather(p, Weather::LIGHT, temp);
        if (p->GetWeathDamage(Weather::SNOW) != 0) {
            p->SetWeathDamage(0, Weather::SNOW);
        }
        if (p->GetWeathDamage(Weather::STORM) != 0) {
            p->SetWeathDamage(0, Weather::STORM);
        }
        if (p->GetWeathDamage(Weather::RAIN) != 0) {
            p->SetWeathDamage(0, Weather::RAIN);
        }
    }
    
    if ((Races->Affect(p->GetRace(), Weather::HEAT)) && p->GetWeathDamage(Weather::HEAT) == 0) {
        p->SetWeathDamage(static_cast<std::uint32_t>(BuildTimeValue(static_cast<float>(Races->Secs(p->GetRace(), Weather::HEAT)))),Weather::HEAT);
    }
    
    if ((Races->Affect(p->GetRace(), Weather::COLD)) && p->GetWeathDamage(Weather::COLD) == 0) {
        p->SetWeathDamage(static_cast<std::uint32_t>(BuildTimeValue(static_cast<float>(Races->Secs(p->GetRace(), Weather::COLD)))),Weather::COLD);
    }
    
    return true;
    
}
//======================================================================
// Updates weather effects for items when light levels change
auto WorldWeather::doItemStuff(CItem *mItem)->bool{
    if (!ValidateObject(mItem))
        return true;
    
    auto currval = mItem->GetRegion()->GetWeather();
    if (currval > region.size() || region.empty()) {
        sendJSWeather(mItem, Weather::LIGHT, 0);
        return true;
    }
    
    auto isStorm = this->region.at(currval)[Weather::STORM].active ;
    auto isSnowing = this->region.at(currval)[Weather::SNOW].active ;
    auto isRaining = this->region.at(currval)[Weather::RAIN].active ;
    auto temp = static_cast<std::int8_t>(this->region.at(currval).impact[Weather::TEMP][Weather::CURRENT]) ;
    
    if (isStorm) {
        sendJSWeather(mItem, Weather::STORM, temp);
    }
    else if (isSnowing && region.at(currval).snowThreshold > this->region.at(currval).impact[Weather::TEMP][Weather::CURRENT]) {
        sendJSWeather(mItem, Weather::SNOW, temp);
    }
    else if (isRaining) {
        sendJSWeather(mItem, Weather::RAIN, temp);
    }
    else {
        sendJSWeather(mItem, Weather::LIGHT, temp);
    }
    
    return true;
    
}
//======================================================================
auto WorldWeather::sendJSWeather(CBaseObject *mObj, Weather::type_t weathType, std::int8_t currentTemp)->void{
    // Check for events in specific scripts attached to object
    auto scriptTriggers = mObj->GetScriptTriggers();
    for (auto scriptTrig : scriptTriggers) {
        cScript *toExecute = JSMapping->GetScript(scriptTrig);
        if (toExecute != nullptr) {
            toExecute->OnWeatherChange(mObj, weathType);
            toExecute->OnTempChange(mObj, currentTemp);
        }
    }
    
    // Check global script as well
    auto toExecuteGlobal = JSMapping->GetScript(static_cast<std::uint16_t>(0));
    if (toExecuteGlobal != nullptr) {
        toExecuteGlobal->OnWeatherChange(mObj, weathType);
        toExecuteGlobal->OnTempChange(mObj, currentTemp);
    }
    
}
