#include "cweather.hpp"

#include "cchar.h"
#include "ceffects.h"
#include "citem.h"
#include "cjsmapping.h"
#include "combat.h"
#include "cpacketsend.h"
#include "craces.h"
#include "cscript.h"
#include "cserverdefinitions.h"
#include "csocket.h"

#include "funcdecl.h"

#include "mapstuff.h"

#include "scriptc.h"
#include "configuration/serverconfig.hpp"
#include "ssection.h"
#include "utility/strutil.hpp"
#include "townregion.h"

#define EFFECTIVE 2
const std::uint8_t MAXVAL = 0;
const std::uint8_t MINVAL = 1;
const std::uint8_t CURRVAL = 2;
const std::uint8_t TEMP = 0;
const std::uint8_t WIND = 1;

cWeatherAb *Weather = nullptr;
// Version History
// 1.0		 		Unknown
//			Initial implementation
//			Based on a weatherSys structure system
// 2.0		 		11th April, 2001
//			Rewritten system
//			Each weather sys is a CWeather class
//			PeriodicUpdate for CWeather also updates light level
//			cWeatherAb simplified to access CWeather containers
//			Replaced a new'd array with a vector of CWeather

CWeather::CWeather() {
    memset(weather, 0, sizeof(WeathPart_st) * WEATHNUM);
    ColdChance(5);
    RainChance(10);
    SnowChance(1);
    HeatChance(10);
    StormChance(1);
    LightMin(300);
    LightMax(300);
    CurrentLight(300);
    
    MaxTemp(30);
    MinTemp(5);
    RainTempDrop(5);
    StormTempDrop(10);
    ColdIntensityHigh(0);
    HeatIntensityHigh(35);
    StormIntensityHigh(100);
    SnowIntensityHigh(100);
    RainIntensityHigh(100);
    StormIntensityLow(0);
    SnowIntensityLow(0);
    RainIntensityLow(0);
    StormIntensity(0);
    SnowIntensity(0);
    RainIntensity(0);
    Temp(15);
    MaxWindSpeed(0);
    MinWindSpeed(0);
    WindSpeed(0);
    SnowThreshold(15.0f);
}

CWeather::~CWeather() {}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::HeatActive()
//|	Date		-	11th April, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether the weather system is having a heat wave
// o------------------------------------------------------------------------------------------------o
bool CWeather::HeatActive() const { return weather[HEAT].Active; }
void CWeather::HeatActive(bool value) { weather[HEAT].Active = value; }
// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::ColdActive()
//|	Date		-	11th April, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/sets whether the weather system is having a cold snap
// o------------------------------------------------------------------------------------------------o
bool CWeather::ColdActive() const { return weather[COLD].Active; }
void CWeather::ColdActive(bool value) { weather[COLD].Active = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::CurrentLight()
//|	Date		-	11th April, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the current light level of the system, IF
//|					 LightMin and LightMax are less than 300
// o------------------------------------------------------------------------------------------------o
R32 CWeather::CurrentLight() const { return light[CURRVAL]; }
void CWeather::CurrentLight(R32 value) { light[CURRVAL] = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::PeriodicUpdate()
//|	Date		-	11th April, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Updates the current light level of the system and the current
// temperature of |					 the system. Wind is not currently updated
// o------------------------------------------------------------------------------------------------o
bool CWeather::PeriodicUpdate() {
    R32 currTemp;
    
    std::uint8_t hour = cwmWorldState->uoTime.hours   ;
    std::uint8_t minute = cwmWorldState->uoTime.minutes ;
    bool ampm = cwmWorldState->uoTime.ampm;
    
    if (StormDelay() && StormBrewing()) {
        StormActive(true);
    }
    else if (StormBrewing()) {
        StormDelay(true);
    }
    
    if (LightMin() < 300 && LightMax() < 300) {
        R32 hourIncrement = static_cast<R32>(fabs((LightMax() - LightMin()) / 12.0f)); // we want the amount to subtract from LightMax in
        // the morning / add to LightMin in evening
        R32 minuteIncrement = hourIncrement / 60.0f;
        R32 tempLight =
        hourIncrement * static_cast<R32>(hour) + minuteIncrement * static_cast<R32>(minute);
        if (ampm) {
            CurrentLight(LightMin() + tempLight);
        }
        else {
            CurrentLight(LightMax() - tempLight);
        }
    }
    
    R32 effTempMax = EffectiveMaxTemp();
    R32 effTempMin = EffectiveMinTemp();
    R32 tempHourIncrement = static_cast<R32>(fabs((effTempMax - effTempMin) / 12.0f));
    R32 tempMinuteIncrement = tempHourIncrement / 60.0f;
    
    R32 tempLightChange =
    tempHourIncrement * static_cast<R32>(hour) + tempMinuteIncrement * static_cast<R32>(minute);
    
    if (ampm) {
        currTemp = effTempMax - tempLightChange; // maximum temperature minus time
    }
    else {
        currTemp = effTempMin + tempLightChange; // minimum temperature plus time
    }
    
    if (StormActive()) {
        currTemp -= StormTempDrop();
        if (currTemp < 0) {
            currTemp = 0;
        }
    }
    else if (RainActive()) {
        currTemp -= RainTempDrop();
        if (currTemp < 0) {
            currTemp = 0;
        }
    }
    Temp(currTemp);
    return true;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::NewDay()
//|	Date		-	11th April, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the system up for a new day. Heat wave, cold snap are
// calculated
// o------------------------------------------------------------------------------------------------o
void CWeather::NewDay() {
    bool isHeatWave = false, isColdDay = false;
    
    R32 effTempMax = MaxTemp();
    R32 effTempMin = MinTemp();
    R32 currentTemp;
    
    if (static_cast<std::uint8_t>(RandomNum(1, 100)) <= HeatChance()) {
        isHeatWave = true;
        currentTemp =
        RandomNum(static_cast<std::int32_t>(effTempMax), static_cast<std::int32_t>(HeatIntensityHigh()));
        effTempMax = currentTemp;
        effTempMin = currentTemp;
    }
    else if (static_cast<std::uint8_t>(RandomNum(1, 100)) <= ColdChance()) {
        isColdDay = true;
        currentTemp =
        RandomNum(static_cast<std::int32_t>(ColdIntensityHigh()), static_cast<std::int32_t>(effTempMin));
        effTempMax = currentTemp;
        effTempMin = currentTemp;
    }
    HeatActive(isHeatWave);
    ColdActive(isColdDay);
    
    EffectiveMaxTemp(effTempMax);
    EffectiveMinTemp(effTempMin);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::NewHour()
//|	Date		-	16th Feb, 2006
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculates rain, snow and storm changes.
// o------------------------------------------------------------------------------------------------o
void CWeather::NewHour() {
    bool isSnowing = false, isRaining = false, isStorm = false;
    if (static_cast<std::uint8_t>(RandomNum(1, 100)) <= StormChance()) {
        isStorm = true;
    }
    if (static_cast<std::uint8_t>(RandomNum(1, 100)) <= SnowChance()) {
        isSnowing = true;
    }
    if (static_cast<std::uint8_t>(RandomNum(1, 100)) <= RainChance()) {
        isRaining = true;
    }
    if (!isStorm) {
        StormDelay(false);
        StormActive(false);
    }
    
    // Calculate intensity values for the weather types
    RainIntensity(static_cast<std::int8_t>(RandomNum(static_cast<std::int16_t>(RainIntensityLow()), static_cast<std::int16_t>(RainIntensityHigh()))));
    SnowIntensity(static_cast<std::int8_t>(RandomNum(static_cast<std::int16_t>(SnowIntensityLow()), static_cast<std::int16_t>(SnowIntensityHigh()))));
    StormIntensity(static_cast<std::int8_t>(RandomNum(static_cast<std::int16_t>(StormIntensityLow()), static_cast<std::int16_t>(StormIntensityHigh()))));
    
    SnowActive(isSnowing);
    RainActive(isRaining);
    StormBrewing(isStorm);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::IntensityHigh()
//|	Date		-	11th April, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set the high intensity of the weather weathType
// o------------------------------------------------------------------------------------------------o
std::int8_t CWeather::IntensityHigh(std::uint8_t weathType) const { return weather[weathType].IntensityHigh; }
void CWeather::IntensityHigh(std::uint8_t weathType, std::int8_t value) {
    weather[weathType].IntensityHigh = value;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::IntensityLow()
//|	Date		-	11th April, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set the low intensity of the weather weathType
// o------------------------------------------------------------------------------------------------o
std::int8_t CWeather::IntensityLow(std::uint8_t weathType) const { return weather[weathType].IntensityLow; }
void CWeather::IntensityLow(std::uint8_t weathType, std::int8_t value) { weather[weathType].IntensityLow = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::Intensity()
//|	Date		-	11th April, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set the intensity of the weather weathType
// o------------------------------------------------------------------------------------------------o
std::int8_t CWeather::Intensity(std::uint8_t weathType) const { return weather[weathType].Intensity; }
void CWeather::Intensity(std::uint8_t weathType, std::int8_t value) { weather[weathType].Intensity = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::Chance()
//|	Date		-	11th April, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the chance of weather weathType occurring
// o------------------------------------------------------------------------------------------------o
std::int8_t CWeather::Chance(std::uint8_t weathType) const { return weather[weathType].Chance; }
void CWeather::Chance(std::uint8_t weathType, std::int8_t value) { weather[weathType].Chance = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::Value()
//|	Date		-	11th April, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the value associated with valType and valOff
//|					 valType is either MAXVAL, MINVAL, or CURRVAL
//|					 valOff is either WIND, TEMP or EFFECTIVE
// o------------------------------------------------------------------------------------------------o
R32 CWeather::Value(std::uint8_t valType, std::uint8_t valOff) const { return assortVals[valType][valOff]; }
void CWeather::Value(std::uint8_t valType, std::uint8_t valOff, R32 value) { assortVals[valType][valOff] = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::SnowIntensityHigh()
//|	Date		-	11th April, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set the high intensity of the snow (if any)
// o------------------------------------------------------------------------------------------------o
std::int8_t CWeather::SnowIntensityHigh() const { return weather[SNOW].IntensityHigh; }
void CWeather::SnowIntensityHigh(std::int8_t value) { weather[SNOW].IntensityHigh = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::SnowIntensityLow()
//|	Date		-	11th April, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set the low intensity of the snow (if any)
// o------------------------------------------------------------------------------------------------o
std::int8_t CWeather::SnowIntensityLow() const { return weather[SNOW].IntensityLow; }
void CWeather::SnowIntensityLow(std::int8_t value) { weather[SNOW].IntensityLow = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::SnowIntensity()
//|	Date		-	11th April, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set the intensity of the snow (if any)
// o------------------------------------------------------------------------------------------------o
std::int8_t CWeather::SnowIntensity() const { return weather[SNOW].Intensity; }
void CWeather::SnowIntensity(std::int8_t value) { weather[SNOW].Intensity = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::RainIntensityHigh()
//|	Date		-	11th April, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set the high intensity of the rain, if any
// o------------------------------------------------------------------------------------------------o
std::int8_t CWeather::RainIntensityHigh() const { return weather[RAIN].IntensityHigh; }
void CWeather::RainIntensityHigh(std::int8_t value) { weather[RAIN].IntensityHigh = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::RainIntensityLow()
//|	Date		-	11th April, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set the low intensity of the rain, if any
// o------------------------------------------------------------------------------------------------o
std::int8_t CWeather::RainIntensityLow() const { return weather[RAIN].IntensityLow; }
void CWeather::RainIntensityLow(std::int8_t value) { weather[RAIN].IntensityLow = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::RainIntensity()
//|	Date		-	11th April, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set the intensity of the rain, if any
// o------------------------------------------------------------------------------------------------o
std::int8_t CWeather::RainIntensity() const { return weather[RAIN].Intensity; }
void CWeather::RainIntensity(std::int8_t value) { weather[RAIN].Intensity = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::StormIntensityHigh()
//|	Date		-	16th Feb, 2006
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set the high intensity of the storm, if any
// o------------------------------------------------------------------------------------------------o
std::int8_t CWeather::StormIntensityHigh() const { return weather[STORM].IntensityHigh; }
void CWeather::StormIntensityHigh(std::int8_t value) { weather[STORM].IntensityHigh = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::StormIntensityLow()
//|	Date		-	16th Feb, 2006
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set the low intensity of the storm, if any
// o------------------------------------------------------------------------------------------------o
std::int8_t CWeather::StormIntensityLow() const { return weather[STORM].IntensityLow; }
void CWeather::StormIntensityLow(std::int8_t value) { weather[STORM].IntensityLow = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::StormIntensity()
//|	Date		-	16th Feb, 2006
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set the intensity of the storm, if any
// o------------------------------------------------------------------------------------------------o
std::int8_t CWeather::StormIntensity() const { return weather[STORM].Intensity; }
void CWeather::StormIntensity(std::int8_t value) { weather[STORM].Intensity = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::HeatIntensityHigh()
//|	Date		-	11th April, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the heat intensity of the weather system
// o------------------------------------------------------------------------------------------------o
std::int8_t CWeather::HeatIntensityHigh() const { return weather[HEAT].IntensityHigh; }
void CWeather::HeatIntensityHigh(std::int8_t value) { weather[HEAT].IntensityHigh = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::ColdIntensityHigh()
//|	Date		-	11th April, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the cold intensity of the weather system
// o------------------------------------------------------------------------------------------------o
std::int8_t CWeather::ColdIntensityHigh() const { return weather[COLD].IntensityHigh; }
void CWeather::ColdIntensityHigh(std::int8_t value) { weather[COLD].IntensityHigh = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::MaxTemp()
//|	Date		-	11th April, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum temperature of the day (non effective)
// o------------------------------------------------------------------------------------------------o
R32 CWeather::MaxTemp() const { return Value(MAXVAL, TEMP); }
void CWeather::MaxTemp(R32 value) { Value(MAXVAL, TEMP, value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::MinTemp()
//|	Date		-	11th April, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the minimum temperature of the day (non effective)
// o------------------------------------------------------------------------------------------------o
R32 CWeather::MinTemp() const { return Value(MINVAL, TEMP); }
void CWeather::MinTemp(R32 value) { Value(MINVAL, TEMP, value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::Temp()
//|	Date		-	11th April, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the current temperature of the day
// o------------------------------------------------------------------------------------------------o
R32 CWeather::Temp() const { return Value(CURRVAL, TEMP); }
void CWeather::Temp(R32 value) { Value(CURRVAL, TEMP, value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::RainTempDrop()
//|	Date		-	19th Feb, 2006
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the amount the temperature drops when it rains
// o------------------------------------------------------------------------------------------------o
R32 CWeather::RainTempDrop() const { return rainTempDrop; }
void CWeather::RainTempDrop(R32 value) { rainTempDrop = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::StormTempDrop()
//|	Date		-	19th Feb, 2006
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the amount the temperature drops when it storms
// o------------------------------------------------------------------------------------------------o
R32 CWeather::StormTempDrop() const { return stormTempDrop; }
void CWeather::StormTempDrop(R32 value) { stormTempDrop = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::MaxWindSpeed()
//|	Date		-	11th April, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum wind speed for the day
// o------------------------------------------------------------------------------------------------o
R32 CWeather::MaxWindSpeed() const { return Value(MAXVAL, WIND); }
void CWeather::MaxWindSpeed(R32 value) { Value(MAXVAL, WIND, value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	MinWindSpeed()
//|	Date		-	11th April, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the minimum wind speed for the day
// o------------------------------------------------------------------------------------------------o
R32 CWeather::MinWindSpeed() const { return Value(MINVAL, WIND); }
void CWeather::MinWindSpeed(R32 value) { Value(MINVAL, WIND, value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::WindSpeed()
//|	Date		-	11th April, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the current wind speed
// o------------------------------------------------------------------------------------------------o
R32 CWeather::WindSpeed() const { return Value(CURRVAL, WIND); }
void CWeather::WindSpeed(R32 value) { Value(CURRVAL, WIND, value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::RainChance()
//|	Date		-	11th April, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the chance of rain for the day
// o------------------------------------------------------------------------------------------------o
std::int8_t CWeather::RainChance() const { return weather[RAIN].Chance; }
void CWeather::RainChance(std::int8_t value) { weather[RAIN].Chance = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::SnowChance()
//|	Date		-	11th April, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the chance of snow for the day
// o------------------------------------------------------------------------------------------------o
std::int8_t CWeather::SnowChance() const { return weather[SNOW].Chance; }
void CWeather::SnowChance(std::int8_t value) { weather[SNOW].Chance = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::StormChance()
//|	Date		-	16th Feb, 2006
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the chance of storm for the day
// o------------------------------------------------------------------------------------------------o
std::int8_t CWeather::StormChance() const { return weather[STORM].Chance; }
void CWeather::StormChance(std::int8_t value) { weather[STORM].Chance = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::HeatChance()
//|	Date		-	11th April, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the chance of a heat wave for the day
// o------------------------------------------------------------------------------------------------o
std::int8_t CWeather::HeatChance() const { return weather[HEAT].Chance; }
void CWeather::HeatChance(std::int8_t value) { weather[HEAT].Chance = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::ColdChance()
//|	Date		-	11th April, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the chance of a cold snap for the day
// o------------------------------------------------------------------------------------------------o
std::int8_t CWeather::ColdChance() const { return weather[COLD].Chance; }
void CWeather::ColdChance(std::int8_t value) { weather[COLD].Chance = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::LightMin()
//|	Date		-	11th April, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the minimum light level for the day (brightest)
// o------------------------------------------------------------------------------------------------o
R32 CWeather::LightMin() const { return light[MINVAL]; }
void CWeather::LightMin(R32 value) { light[MINVAL] = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::LightMax()
//|	Date		-	11th April, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum light level for the day (darkest)
// o------------------------------------------------------------------------------------------------o
R32 CWeather::LightMax() const { return light[MAXVAL]; }
void CWeather::LightMax(R32 value) { light[MAXVAL] = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::RainActive()
//|	Date		-	11th April, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether the weather system is raining
// o------------------------------------------------------------------------------------------------o
bool CWeather::RainActive() const { return weather[RAIN].Active; }
void CWeather::RainActive(bool value) { weather[RAIN].Active = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::SnowActive()
//|	Date		-	11th April, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether the weather system is snowing
// o------------------------------------------------------------------------------------------------o
bool CWeather::SnowActive() const { return weather[SNOW].Active; }
void CWeather::SnowActive(bool value) { weather[SNOW].Active = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::StormBrewing()
//|	Date		-	16th Feb, 2006
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether the weather system is brewing a storm
// o------------------------------------------------------------------------------------------------o
bool CWeather::StormBrewing() const { return weather[STORMBREW].Active; }
void CWeather::StormBrewing(bool value) { weather[STORMBREW].Active = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::StormActive()
//|	Date		-	16th Feb, 2006
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether a storm is active in the weather system
// o------------------------------------------------------------------------------------------------o
bool CWeather::StormActive() const { return weather[STORM].Active; }
void CWeather::StormActive(bool value) { weather[STORM].Active = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::StormDelay()
//|	Date		-	17th Feb, 2006
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether there is a storm delay in the weather system
// o------------------------------------------------------------------------------------------------o
bool CWeather::StormDelay() const { return stormDelay; }
void CWeather::StormDelay(bool value) { stormDelay = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::SnowThreshold()
//|	Date		-	11th April, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the temperature below which snow kicks in
// o------------------------------------------------------------------------------------------------o
R32 CWeather::SnowThreshold() const { return snowThreshold; }
void CWeather::SnowThreshold(R32 value) { snowThreshold = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::EffectiveMaxTemp()
//|	Date		-	11th April, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the effective maximum temperature for the day
// o------------------------------------------------------------------------------------------------o
R32 CWeather::EffectiveMaxTemp() const { return Value(MAXVAL, EFFECTIVE); }
void CWeather::EffectiveMaxTemp(R32 value) { Value(MAXVAL, EFFECTIVE, value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeather::EffectiveMinTemp()
//|	Date		-	11th April, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the effective minimum temperature for the day
// o------------------------------------------------------------------------------------------------o
R32 CWeather::EffectiveMinTemp() const { return Value(MINVAL, EFFECTIVE); }
void CWeather::EffectiveMinTemp(R32 value) { Value(MINVAL, EFFECTIVE, value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Default weather constructor
// o------------------------------------------------------------------------------------------------o
cWeatherAb::cWeatherAb() { weather.resize(0); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	~cWeatherAb()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Destroys any allocated memory
// o------------------------------------------------------------------------------------------------o
cWeatherAb::~cWeatherAb() { weather.clear(); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeatherAb::Count()
//|	Date		-	31st July, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns count of weather systems
// o------------------------------------------------------------------------------------------------o
size_t cWeatherAb::Count() const { return weather.size(); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWeatherAb::load()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if the weather system loaded okay
// o------------------------------------------------------------------------------------------------o
auto cWeatherAb::load() -> bool {
    weather.resize(FileLookup->CountOfEntries(weathab_def));
    std::string tag, data, UTag;
    size_t i = 0;
    for (auto &weathScp : FileLookup->ScriptListings[weathab_def]) {
        if (weathScp == nullptr)
            continue;
        
        for (auto &[entryName, WeatherStuff] : weathScp->collection()) {
            if (WeatherStuff == nullptr) {
                continue;
            }
            
            auto ssecs = oldstrutil::sections(entryName, " ");
            i = static_cast<std::uint32_t>(std::stoul(ssecs[1], nullptr, 0));
            if (i >= weather.size()) {
                weather.resize(i + 1);
            }
            for (const auto &sec : WeatherStuff->collection()) {
                tag = sec->tag;
                data = sec->data;
                UTag = util::upper(tag);
                switch (tag[0]) {
                    case 'c':
                    case 'C':
                        if (UTag == "COLDCHANCE") { // chance for a cold day
                            ColdChance(static_cast<weathid_t>(i), static_cast<std::int8_t>(std::stoi(data, nullptr, 0)));
                        }
                        else if (UTag == "COLDINTENSITY") { // cold intensity
                            ColdIntensityHigh(static_cast<weathid_t>(i), static_cast<std::int8_t>(std::stoi(data, nullptr, 0)));
                        }
                        break;
                    case 'h':
                    case 'H':
                        if (UTag == "HEATCHANCE") { // chance for a hot day
                            HeatChance(static_cast<weathid_t>(i),
                                       static_cast<std::int8_t>(std::stoi(data, nullptr, 0)));
                        }
                        else if (UTag == "HEATINTENSITY") { // heat intensity
                            HeatIntensityHigh(static_cast<weathid_t>(i),
                                              static_cast<std::int8_t>(std::stoi(data, nullptr, 0)));
                        }
                        break;
                    case 'l':
                    case 'L':
                        if (UTag == "LIGHTMIN") { // minimum light level
                            LightMin(static_cast<weathid_t>(i), std::stof(data));
                        }
                        else if (UTag == "LIGHTMAX") { // maximum light level
                            LightMax(static_cast<weathid_t>(i), std::stof(data));
                        }
                        break;
                    case 'm':
                    case 'M':
                        if (UTag == "MAXTEMP") { // maximum temperature
                            MaxTemp(static_cast<weathid_t>(i), std::stof(data));
                        }
                        else if (UTag == "MINTEMP") { // minimum temperature
                            MinTemp(static_cast<weathid_t>(i), std::stof(data));
                        }
                        else if (UTag == "MAXWIND") { // maximum wind speed
                            MaxWindSpeed(static_cast<weathid_t>(i), std::stof(data));
                        }
                        else if (UTag == "MINWIND") { // minimum wind speed
                            MinWindSpeed(static_cast<weathid_t>(i), std::stof(data));
                        }
                        break;
                    case 'r':
                    case 'R':
                        if (UTag == "RAINCHANCE") { // chance of rain
                            RainChance(static_cast<weathid_t>(i),
                                       static_cast<std::int8_t>(std::stoi(data, nullptr, 0)));
                        }
                        else if (UTag == "RAININTENSITY") { // intensity of rain
                            auto csecs = oldstrutil::sections(data, ",");
                            if (csecs.size() > 1) {
                                RainIntensityLow(static_cast<weathid_t>(i), static_cast<std::int8_t>(std::stoi(util::trim(util::strip(csecs[0], "//")), nullptr, 0)));
                                RainIntensityHigh(static_cast<weathid_t>(i), static_cast<std::int8_t>(std::stoi(util::trim(util::strip(csecs[1], "//")), nullptr, 0)));
                            }
                            else {
                                RainIntensityLow(static_cast<weathid_t>(i), 0);
                                RainIntensityHigh(static_cast<weathid_t>(i), static_cast<std::int8_t>(std::stoi(data, nullptr, 0)));
                            }
                        }
                        else if (UTag == "RAINTEMPDROP") { // temp drop of rain
                            RainTempDrop(static_cast<weathid_t>(i), static_cast<std::int8_t>(std::stoi(data, nullptr, 0)));
                        }
                        break;
                    case 's':
                    case 'S':
                        if (UTag == "SNOWCHANCE") { // chance of snow
                            SnowChance(static_cast<weathid_t>(i), static_cast<std::int8_t>(std::stoi(data, nullptr, 0)));
                        }
                        else if (UTag == "SNOWINTENSITY") { // intensity of snow
                            auto csecs = oldstrutil::sections(data, ",");
                            if (csecs.size() > 1) {
                                SnowIntensityLow( static_cast<weathid_t>(i), static_cast<std::int8_t>(std::stoi(util::trim(util::strip(csecs[0], "//")), nullptr, 0)));
                                SnowIntensityHigh( static_cast<weathid_t>(i),  static_cast<std::int8_t>(std::stoi(util::trim(util::strip(csecs[1], "//")), nullptr, 0)));
                            }
                            else {
                                SnowIntensityLow(static_cast<weathid_t>(i), 0);
                                SnowIntensityHigh(static_cast<weathid_t>(i), static_cast<std::int8_t>(std::stoi(data, nullptr, 0)));
                            }
                        }
                        else if (UTag == "SNOWTHRESHOLD") { // temperature at which snow kicks in
                            SnowThreshold(static_cast<weathid_t>(i), std::stof(data));
                        }
                        else if (UTag == "STORMCHANCE") { // chance of a storm
                            StormChance(static_cast<weathid_t>(i), static_cast<std::int8_t>(std::stoi(data, nullptr, 0)));
                        }
                        else if (UTag == "STORMINTENSITY") { // chance of a storm
                            auto csecs = oldstrutil::sections(data, ",");
                            if (csecs.size() > 1) {
                                SnowIntensityLow(static_cast<weathid_t>(i), static_cast<std::int8_t>(std::stoi(util::trim(util::strip(csecs[0], "//")), nullptr, 0)));
                                SnowIntensityHigh(static_cast<weathid_t>(i), static_cast<std::int8_t>(std::stoi(util::trim(util::strip(csecs[1], "//")), nullptr, 0)));
                            }
                            else {
                                SnowIntensityLow(static_cast<weathid_t>(i), 0);
                                SnowIntensityHigh(static_cast<weathid_t>(i), static_cast<std::int8_t>(std::stoi(data, nullptr, 0)));
                            }
                        }
                        else if (UTag == "STORMTEMPDROP") { // temp drop of storm
                            StormTempDrop(static_cast<weathid_t>(i), static_cast<std::int8_t>(std::stoi(data, nullptr, 0)));
                        }
                        break;
                }
            }
        }
    }
    return true;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::IntensityHigh( weathid_t toCheck, std::uint8_t weathType )
//|					cWeatherAb::IntensityLow( weathid_t toCheck, std::uint8_t weathType )
//|					cWeatherAb::Intensity( weathid_t toCheck, std::uint8_t weathType )
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Helper func to return intensity of a weather type
// o------------------------------------------------------------------------------------------------o
std::int8_t cWeatherAb::IntensityHigh(weathid_t toCheck, std::uint8_t weathType) {
    return weather[toCheck].IntensityHigh(weathType);
}
std::int8_t cWeatherAb::IntensityLow(weathid_t toCheck, std::uint8_t weathType) {
    return weather[toCheck].IntensityLow(weathType);
}
std::int8_t cWeatherAb::Intensity(weathid_t toCheck, std::uint8_t weathType) {
    return weather[toCheck].Intensity(weathType);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	IntensityHigh( weathid_t toCheck, std::uint8_t weathType, std::int8_t value )
//|					IntensityLow( weathid_t toCheck, std::uint8_t weathType, std::int8_t value )
//|					Intensity( weathid_t toCheck, std::uint8_t weathType, std::int8_t value )
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Helper func to set the intensity of a particular weather type
// o------------------------------------------------------------------------------------------------o
void cWeatherAb::IntensityHigh(weathid_t toCheck, std::uint8_t weathType, std::int8_t value) {
    weather[toCheck].IntensityHigh(weathType, value);
}
void cWeatherAb::IntensityLow(weathid_t toCheck, std::uint8_t weathType, std::int8_t value) {
    weather[toCheck].IntensityLow(weathType, value);
}
void cWeatherAb::Intensity(weathid_t toCheck, std::uint8_t weathType, std::int8_t value) {
    weather[toCheck].Intensity(weathType, value);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::Chance( weathid_t toCheck, std::uint8_t weathType )
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Helper func to return chance of a weather type
// o------------------------------------------------------------------------------------------------o
std::int8_t cWeatherAb::Chance(weathid_t toCheck, std::uint8_t weathType) {
    return weather[toCheck].Chance(weathType);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::Chance( weathid_t toCheck, std::uint8_t weathType, std::int8_t value )
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Helper func to set the chance of finding a weather type
// o------------------------------------------------------------------------------------------------o
void cWeatherAb::Chance(weathid_t toCheck, std::uint8_t weathType, std::int8_t value) {
    weather[toCheck].Chance(weathType, value);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::Value( weathid_t toCheck, std::uint8_t valType, std::uint8_t valOff )
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Helper func to return value of some weather types
// o------------------------------------------------------------------------------------------------o
R32 cWeatherAb::Value(weathid_t toCheck, std::uint8_t valType, std::uint8_t valOff)
// PRE:		toCheck is valid, valType is valid, valOff is valid
// POST:	returns value of valType and valOff in toCheck
{
    return weather[toCheck].Value(valType, valOff);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::Value( weathid_t toCheck, std::uint8_t valType, std::uint8_t valOff, R32
// value )
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Helper func to set the value of a particular weather type
// o------------------------------------------------------------------------------------------------o
void cWeatherAb::Value(weathid_t toCheck, std::uint8_t valType, std::uint8_t valOff, R32 value)
// PRE:		toCheck is valid, valType and valOff is valid, value is valid
// POST:	value of valType and valOff in toCheck is value
{
    weather[toCheck].Value(valType, valOff, value);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::SnowIntensityHigh()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set the high intensity of toCheck's snow
// o------------------------------------------------------------------------------------------------o
std::int8_t cWeatherAb::SnowIntensityHigh(weathid_t toCheck) { return IntensityHigh(toCheck, SNOW); }
void cWeatherAb::SnowIntensityHigh(weathid_t toCheck, std::int8_t value) {
    IntensityHigh(toCheck, SNOW, value);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::SnowIntensityLow()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set the low intensity of toCheck's snow
// o------------------------------------------------------------------------------------------------o
std::int8_t cWeatherAb::SnowIntensityLow(weathid_t toCheck) { return IntensityLow(toCheck, SNOW); }
void cWeatherAb::SnowIntensityLow(weathid_t toCheck, std::int8_t value) {
    IntensityLow(toCheck, SNOW, value);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::SnowIntensity()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set the intensity of toCheck's snow
// o------------------------------------------------------------------------------------------------o
std::int8_t cWeatherAb::SnowIntensity(weathid_t toCheck) { return Intensity(toCheck, SNOW); }
void cWeatherAb::SnowIntensity(weathid_t toCheck, std::int8_t value) { Intensity(toCheck, SNOW, value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::StormIntensityHigh()
//|	Date		-	16th Feb, 2006
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set the high intensity of toCheck's storm
// o------------------------------------------------------------------------------------------------o
std::int8_t cWeatherAb::StormIntensityHigh(weathid_t toCheck) { return IntensityHigh(toCheck, STORM); }
void cWeatherAb::StormIntensityHigh(weathid_t toCheck, std::int8_t value) {
    IntensityHigh(toCheck, STORM, value);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::StormIntensityLow()
//|	Date		-	16th Feb, 2006
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set the low intensity of toCheck's storm
// o------------------------------------------------------------------------------------------------o
std::int8_t cWeatherAb::StormIntensityLow(weathid_t toCheck) { return IntensityLow(toCheck, STORM); }
void cWeatherAb::StormIntensityLow(weathid_t toCheck, std::int8_t value) {
    IntensityLow(toCheck, STORM, value);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::StormIntensity()
//|	Date		-	16th Feb, 2006
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set the intensity of toCheck's storm
// o------------------------------------------------------------------------------------------------o
std::int8_t cWeatherAb::StormIntensity(weathid_t toCheck) { return Intensity(toCheck, STORM); }
void cWeatherAb::StormIntensity(weathid_t toCheck, std::int8_t value) { Intensity(toCheck, STORM, value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::RainIntensityHigh()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set the high intensity of toCheck's rain
// o------------------------------------------------------------------------------------------------o
std::int8_t cWeatherAb::RainIntensityHigh(weathid_t toCheck) { return IntensityHigh(toCheck, RAIN); }
void cWeatherAb::RainIntensityHigh(weathid_t toCheck, std::int8_t value) {
    IntensityHigh(toCheck, RAIN, value);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::RainIntensityLow()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set the low intensity of toCheck's rain
// o------------------------------------------------------------------------------------------------o
std::int8_t cWeatherAb::RainIntensityLow(weathid_t toCheck) { return IntensityLow(toCheck, RAIN); }
void cWeatherAb::RainIntensityLow(weathid_t toCheck, std::int8_t value) {
    IntensityLow(toCheck, RAIN, value);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::RainIntensity()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set the intensity of toCheck's rain
// o------------------------------------------------------------------------------------------------o
std::int8_t cWeatherAb::RainIntensity(weathid_t toCheck) { return Intensity(toCheck, RAIN); }
void cWeatherAb::RainIntensity(weathid_t toCheck, std::int8_t value) { Intensity(toCheck, RAIN, value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::HeatIntensityHigh()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the high intensity of toCheck's heat
// o------------------------------------------------------------------------------------------------o
std::int8_t cWeatherAb::HeatIntensityHigh(weathid_t toCheck) { return IntensityHigh(toCheck, HEAT); }
void cWeatherAb::HeatIntensityHigh(weathid_t toCheck, std::int8_t value) {
    IntensityHigh(toCheck, HEAT, value);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::ColdIntensityHigh()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the high intensity of toCheck's cold
// o------------------------------------------------------------------------------------------------o
std::int8_t cWeatherAb::ColdIntensityHigh(weathid_t toCheck) { return IntensityHigh(toCheck, COLD); }
void cWeatherAb::ColdIntensityHigh(weathid_t toCheck, std::int8_t value) {
    IntensityHigh(toCheck, COLD, value);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::MaxTemp()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum temperature of toCheck
// o------------------------------------------------------------------------------------------------o
R32 cWeatherAb::MaxTemp(weathid_t toCheck) { return Value(toCheck, MAXVAL, TEMP); }
void cWeatherAb::MaxTemp(weathid_t toCheck, R32 value) { Value(toCheck, MAXVAL, TEMP, value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::MinTemp()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the minimum temperature of toCheck
// o------------------------------------------------------------------------------------------------o
R32 cWeatherAb::MinTemp(weathid_t toCheck) { return Value(toCheck, MINVAL, TEMP); }
void cWeatherAb::MinTemp(weathid_t toCheck, R32 value) { Value(toCheck, MINVAL, TEMP, value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::Temp()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the temperature of toCheck
// o------------------------------------------------------------------------------------------------o
R32 cWeatherAb::Temp(weathid_t toCheck) { return Value(toCheck, CURRVAL, TEMP); }
void cWeatherAb::Temp(weathid_t toCheck, R32 value) { Value(toCheck, CURRVAL, TEMP, value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::RainTempDrop()
//|	Date		-	19th Feb, 2006
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the rain temperature drop of toCheck
// o------------------------------------------------------------------------------------------------o
R32 cWeatherAb::RainTempDrop(weathid_t toCheck) { return weather[toCheck].RainTempDrop(); }
void cWeatherAb::RainTempDrop(weathid_t toCheck, R32 value) { weather[toCheck].RainTempDrop(value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::StormTempDrop()
//|	Date		-	19th Feb, 2006
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the storm temperature drop of toCheck
// o------------------------------------------------------------------------------------------------o
R32 cWeatherAb::StormTempDrop(weathid_t toCheck) { return weather[toCheck].StormTempDrop(); }
void cWeatherAb::StormTempDrop(weathid_t toCheck, R32 value) {
    weather[toCheck].StormTempDrop(value);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::MaxWindSpeed()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum wind speed of toCheck
// o------------------------------------------------------------------------------------------------o
R32 cWeatherAb::MaxWindSpeed(weathid_t toCheck) { return Value(toCheck, MAXVAL, WIND); }
void cWeatherAb::MaxWindSpeed(weathid_t toCheck, R32 value) { Value(toCheck, MAXVAL, WIND, value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::MinWindSpeed()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the minimum wind speed of toCheck
// o------------------------------------------------------------------------------------------------o
R32 cWeatherAb::MinWindSpeed(weathid_t toCheck) { return Value(toCheck, MINVAL, WIND); }
void cWeatherAb::MinWindSpeed(weathid_t toCheck, R32 value) { Value(toCheck, MINVAL, WIND, value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::WindSpeed()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the wind speed of toCheck
// o------------------------------------------------------------------------------------------------o
R32 cWeatherAb::WindSpeed(weathid_t toCheck) { return Value(toCheck, CURRVAL, WIND); }
void cWeatherAb::WindSpeed(weathid_t toCheck, R32 value) { Value(toCheck, CURRVAL, WIND, value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::RainChance()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets toCheck's rain chance
// o------------------------------------------------------------------------------------------------o
std::int8_t cWeatherAb::RainChance(weathid_t toCheck) { return Chance(toCheck, RAIN); }
void cWeatherAb::RainChance(weathid_t toCheck, std::int8_t value) { Chance(toCheck, RAIN, value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::SnowChance()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets toCheck's snow chance
// o------------------------------------------------------------------------------------------------o
std::int8_t cWeatherAb::SnowChance(weathid_t toCheck) { return Chance(toCheck, SNOW); }
void cWeatherAb::SnowChance(weathid_t toCheck, std::int8_t value) { Chance(toCheck, SNOW, value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::StormChance()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets toCheck's storm chance
// o------------------------------------------------------------------------------------------------o
std::int8_t cWeatherAb::StormChance(weathid_t toCheck) { return Chance(toCheck, STORM); }
void cWeatherAb::StormChance(weathid_t toCheck, std::int8_t value) { Chance(toCheck, STORM, value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::HeatChance()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets toCheck's heat chance
// o------------------------------------------------------------------------------------------------o
std::int8_t cWeatherAb::HeatChance(weathid_t toCheck) { return Chance(toCheck, HEAT); }
void cWeatherAb::HeatChance(weathid_t toCheck, std::int8_t value) { Chance(toCheck, HEAT, value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::ColdChance()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets toCheck's cold chance
// o------------------------------------------------------------------------------------------------o
std::int8_t cWeatherAb::ColdChance(weathid_t toCheck) { return Chance(toCheck, COLD); }
void cWeatherAb::ColdChance(weathid_t toCheck, std::int8_t value) { Chance(toCheck, COLD, value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::SnowThreshold()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets toCheck's snow threshold
// o------------------------------------------------------------------------------------------------o
R32 cWeatherAb::SnowThreshold(weathid_t toCheck) { return weather[toCheck].SnowThreshold(); }
void cWeatherAb::SnowThreshold(weathid_t toCheck, R32 value) {
    weather[toCheck].SnowThreshold(value);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::NewDay()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Begins a new day in the weather system
// o------------------------------------------------------------------------------------------------o
bool cWeatherAb::NewDay() {
    std::vector<CWeather>::iterator wIter;
    for (wIter = weather.begin(); wIter != weather.end(); ++wIter) {
        (*wIter).NewDay();
    }
    return true;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::NewHour()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Begins a new hour in the weather system
// o------------------------------------------------------------------------------------------------o
bool cWeatherAb::NewHour() {
    std::vector<CWeather>::iterator wIter;
    for (wIter = weather.begin(); wIter != weather.end(); ++wIter) {
        (*wIter).NewHour();
    }
    return true;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::EffectiveMinTemp()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the effective min temperature of a weather system
// o------------------------------------------------------------------------------------------------o
R32 cWeatherAb::EffectiveMinTemp(weathid_t toCheck) { return Value(toCheck, MINVAL, EFFECTIVE); }
void cWeatherAb::EffectiveMinTemp(weathid_t toCheck, R32 value) {
    Value(toCheck, MINVAL, EFFECTIVE, value);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::EffectiveMaxTemp()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the effective max temperature of a weather system
// o------------------------------------------------------------------------------------------------o
R32 cWeatherAb::EffectiveMaxTemp(weathid_t toCheck) { return Value(toCheck, MAXVAL, EFFECTIVE); }
void cWeatherAb::EffectiveMaxTemp(weathid_t toCheck, R32 value) {
    Value(toCheck, MAXVAL, EFFECTIVE, value);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::DoStuff()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Periodically called from main loop to update weather types
// o------------------------------------------------------------------------------------------------o
bool cWeatherAb::DoStuff() {
    std::vector<CWeather>::iterator wIter;
    for (wIter = weather.begin(); wIter != weather.end(); ++wIter) {
        (*wIter).PeriodicUpdate();
    }
    return true;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::HeatActive()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets heat in specified weather system
// o------------------------------------------------------------------------------------------------o
bool cWeatherAb::HeatActive(weathid_t toCheck) { return weather[toCheck].HeatActive(); }
void cWeatherAb::HeatActive(weathid_t toCheck, bool value) { weather[toCheck].HeatActive(value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::ColdActive()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets cold in specified weather system
// o------------------------------------------------------------------------------------------------o
bool cWeatherAb::ColdActive(weathid_t toCheck) { return weather[toCheck].ColdActive(); }
void cWeatherAb::ColdActive(weathid_t toCheck, bool value) { weather[toCheck].ColdActive(value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::RainActive()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets rain in specified weather system
// o------------------------------------------------------------------------------------------------o
bool cWeatherAb::RainActive(weathid_t toCheck) { return weather[toCheck].RainActive(); }
void cWeatherAb::RainActive(weathid_t toCheck, bool value) { weather[toCheck].RainActive(value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::SnowActive()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets snow in specified weather system
// o------------------------------------------------------------------------------------------------o
bool cWeatherAb::SnowActive(weathid_t toCheck) { return weather[toCheck].SnowActive(); }
void cWeatherAb::SnowActive(weathid_t toCheck, bool value) { weather[toCheck].SnowActive(value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::StormBrewing()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether a storm is brewing in specified weather system
// o------------------------------------------------------------------------------------------------o
bool cWeatherAb::StormBrewing(weathid_t toCheck) { return weather[toCheck].StormBrewing(); }
void cWeatherAb::StormBrewing(weathid_t toCheck, bool value) { weather[toCheck].StormBrewing(value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::StormActive()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether a storm is active in specified weather system
// o------------------------------------------------------------------------------------------------o
bool cWeatherAb::StormActive(weathid_t toCheck) { return weather[toCheck].StormActive(); }
void cWeatherAb::StormActive(weathid_t toCheck, bool value) { weather[toCheck].StormActive(value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::DoPlayerStuff()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Updates weather effects for players when light levels change,
//|					or depending on whether player is inside or outside of
// buildings
// o------------------------------------------------------------------------------------------------o
bool cWeatherAb::DoPlayerStuff(CSocket *s, CChar *p) {
    if (!ValidateObject(p) || p->IsNpc())
        return true;
    
    std::int8_t defaultTemp = 20;
    weathid_t currval = p->GetRegion()->GetWeather();
    if (currval > weather.size() || weather.empty() || p->InBuilding()) {
        if (s != nullptr) {
            CPWeather dry(0xFF, 0x00, defaultTemp);
            s->Send(&dry);
            if (p->GetWeathDamage(SNOW) != 0) {
                p->SetWeathDamage(0, SNOW);
            }
            if (p->GetWeathDamage(STORM) != 0) {
                p->SetWeathDamage(0, STORM);
            }
            if (p->GetWeathDamage(RAIN) != 0) {
                p->SetWeathDamage(0, RAIN);
            }
            if (p->GetWeathDamage(COLD) != 0) {
                p->SetWeathDamage(0, COLD);
            }
            if (p->GetWeathDamage(HEAT) != 0) {
                p->SetWeathDamage(0, HEAT);
            }
            SendJSWeather(p, LIGHT, defaultTemp);
        }
        return false;
    }
    
    bool isStorm = StormActive(currval);
    bool brewStorm = StormBrewing(currval);
    bool isSnowing = SnowActive(currval);
    bool isRaining = RainActive(currval);
    std::int8_t temp = static_cast<std::int8_t>(Temp(currval));
    
    if (isStorm) {
        DoPlayerWeather(s, 5, temp, currval);
        if (p->GetWeathDamage(STORM) == 0) {
            p->SetWeathDamage(static_cast<std::uint32_t>(BuildTimeValue(static_cast<R32>(Races->Secs(p->GetRace(), STORM)))),STORM);
        }
        if (p->GetWeathDamage(SNOW) != 0) {
            p->SetWeathDamage(0, SNOW);
        }
        if (p->GetWeathDamage(RAIN) != 0) {
            p->SetWeathDamage(0, RAIN);
        }
        SendJSWeather(p, STORM, temp);
    }
    else if (brewStorm) {
        DoPlayerWeather(s, 4, temp, currval);
    }
    else if (isSnowing && SnowThreshold(currval) > Temp(currval)) {
        DoPlayerWeather(s, 2, temp, currval);
        if (p->GetWeathDamage(SNOW) == 0) {
            p->SetWeathDamage(static_cast<std::uint32_t>(BuildTimeValue(static_cast<R32>(Races->Secs(p->GetRace(), SNOW)))),SNOW);
        }
        if (p->GetWeathDamage(STORM) != 0) {
            p->SetWeathDamage(0, STORM);
        }
        if (p->GetWeathDamage(RAIN) != 0) {
            p->SetWeathDamage(0, RAIN);
        }
        SendJSWeather(p, STORM, temp);
    }
    else if (isRaining) {
        DoPlayerWeather(s, 1, temp, currval);
        if (p->GetWeathDamage(RAIN) == 0) {
            p->SetWeathDamage(static_cast<std::uint32_t>(BuildTimeValue(static_cast<R32>(Races->Secs(p->GetRace(), RAIN)))),RAIN);
        }
        if (p->GetWeathDamage(SNOW) != 0) {
            p->SetWeathDamage(0, SNOW);
        }
        if (p->GetWeathDamage(STORM) != 0) {
            p->SetWeathDamage(0, STORM);
        }
        SendJSWeather(p, STORM, temp);
    }
    else {
        DoPlayerWeather(s, 0, temp, currval);
        if (p->GetWeathDamage(SNOW) != 0) {
            p->SetWeathDamage(0, SNOW);
        }
        if (p->GetWeathDamage(STORM) != 0) {
            p->SetWeathDamage(0, STORM);
        }
        if (p->GetWeathDamage(RAIN) != 0) {
            p->SetWeathDamage(0, RAIN);
        }
        SendJSWeather(p, STORM, temp);
    }
    
    if ((Races->Affect(p->GetRace(), HEAT)) && p->GetWeathDamage(HEAT) == 0) {
        p->SetWeathDamage(static_cast<std::uint32_t>(BuildTimeValue(static_cast<R32>(Races->Secs(p->GetRace(), HEAT)))),HEAT);
    }
    
    if ((Races->Affect(p->GetRace(), COLD)) && p->GetWeathDamage(COLD) == 0) {
        p->SetWeathDamage(static_cast<std::uint32_t>(BuildTimeValue(static_cast<R32>(Races->Secs(p->GetRace(), COLD)))),COLD);
    }
    
    return true;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::DoNPCStuff()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Updates weather effects for NPCs when light levels change,
//|					or depending on whether NPC is inside or outside of
// buildings
// o------------------------------------------------------------------------------------------------o
bool cWeatherAb::DoNPCStuff(CChar *p) {
    if (!ValidateObject(p))
        return true;
    
    weathid_t currval = p->GetRegion()->GetWeather();
    if (currval > weather.size() || weather.empty() || p->InBuilding()) {
        SendJSWeather(p, LIGHT, 0);
        return false;
    }
    
    bool isStorm = StormActive(currval);
    bool isSnowing = SnowActive(currval);
    bool isRaining = RainActive(currval);
    std::int8_t temp = static_cast<std::int8_t>(Temp(currval));
    
    if (isStorm) {
        SendJSWeather(p, STORM, temp);
        if (p->GetWeathDamage(STORM) == 0) {
            p->SetWeathDamage(static_cast<std::uint32_t>(BuildTimeValue(static_cast<R32>(Races->Secs(p->GetRace(), STORM)))),STORM);
        }
        if (p->GetWeathDamage(SNOW) != 0) {
            p->SetWeathDamage(0, SNOW);
        }
        if (p->GetWeathDamage(RAIN) != 0) {
            p->SetWeathDamage(0, RAIN);
        }
    }
    else if (isSnowing && SnowThreshold(currval) > Temp(currval)) {
        SendJSWeather(p, SNOW, temp);
        if (p->GetWeathDamage(SNOW) == 0) {
            p->SetWeathDamage(static_cast<std::uint32_t>(BuildTimeValue(static_cast<R32>(Races->Secs(p->GetRace(), SNOW)))),SNOW);
        }
        if (p->GetWeathDamage(STORM) != 0) {
            p->SetWeathDamage(0, STORM);
        }
        if (p->GetWeathDamage(RAIN) != 0) {
            p->SetWeathDamage(0, RAIN);
        }
    }
    else if (isRaining) {
        SendJSWeather(p, RAIN, temp);
        if (p->GetWeathDamage(RAIN) == 0) {
            p->SetWeathDamage(static_cast<std::uint32_t>(BuildTimeValue(static_cast<R32>(Races->Secs(p->GetRace(), RAIN)))),RAIN);
        }
        if (p->GetWeathDamage(SNOW) != 0) {
            p->SetWeathDamage(0, SNOW);
        }
        if (p->GetWeathDamage(STORM) != 0) {
            p->SetWeathDamage(0, STORM);
        }
    }
    else {
        SendJSWeather(p, LIGHT, temp);
        if (p->GetWeathDamage(SNOW) != 0) {
            p->SetWeathDamage(0, SNOW);
        }
        if (p->GetWeathDamage(STORM) != 0) {
            p->SetWeathDamage(0, STORM);
        }
        if (p->GetWeathDamage(RAIN) != 0) {
            p->SetWeathDamage(0, RAIN);
        }
    }
    
    if ((Races->Affect(p->GetRace(), HEAT)) && p->GetWeathDamage(HEAT) == 0) {
        p->SetWeathDamage(static_cast<std::uint32_t>(BuildTimeValue(static_cast<R32>(Races->Secs(p->GetRace(), HEAT)))),HEAT);
    }
    
    if ((Races->Affect(p->GetRace(), COLD)) && p->GetWeathDamage(COLD) == 0) {
        p->SetWeathDamage(static_cast<std::uint32_t>(BuildTimeValue(static_cast<R32>(Races->Secs(p->GetRace(), COLD)))),COLD);
    }
    
    return true;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::DoItemStuff()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Updates weather effects for items when light levels change
// o------------------------------------------------------------------------------------------------o
bool cWeatherAb::DoItemStuff(CItem *mItem) {
    if (!ValidateObject(mItem))
        return true;
    
    weathid_t currval = mItem->GetRegion()->GetWeather();
    if (currval > weather.size() || weather.empty()) {
        SendJSWeather(mItem, LIGHT, 0);
        return true;
    }
    
    bool isStorm = StormActive(currval);
    // bool brewStorm = StormBrewing( currval );
    bool isSnowing = SnowActive(currval);
    bool isRaining = RainActive(currval);
    std::int8_t temp = static_cast<std::int8_t>(Temp(currval));
    
    if (isStorm) {
        SendJSWeather(mItem, STORM, temp);
    }
    else if (isSnowing && SnowThreshold(currval) > Temp(currval)) {
        SendJSWeather(mItem, SNOW, temp);
    }
    else if (isRaining) {
        SendJSWeather(mItem, RAIN, temp);
    }
    else {
        SendJSWeather(mItem, LIGHT, temp);
    }
    
    return true;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::SendJSWeather()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends weather changes to JS engine and triggers relevant JS events
// o------------------------------------------------------------------------------------------------o
void cWeatherAb::SendJSWeather(CBaseObject *mObj, weathertype_t weathType, std::int8_t currentTemp) {
    // Check for events in specific scripts attached to object
    std::vector<std::uint16_t> scriptTriggers = mObj->GetScriptTriggers();
    for (auto scriptTrig : scriptTriggers) {
        cScript *toExecute = JSMapping->GetScript(scriptTrig);
        if (toExecute != nullptr) {
            toExecute->OnWeatherChange(mObj, weathType);
            toExecute->OnTempChange(mObj, currentTemp);
        }
    }
    
    // Check global script as well
    cScript *toExecuteGlobal = JSMapping->GetScript(static_cast<std::uint16_t>(0));
    if (toExecuteGlobal != nullptr) {
        toExecuteGlobal->OnWeatherChange(mObj, weathType);
        toExecuteGlobal->OnTempChange(mObj, currentTemp);
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::DoPlayerWeather()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends weather effects to player's client
// o------------------------------------------------------------------------------------------------o
void cWeatherAb::DoPlayerWeather(CSocket *s, std::uint8_t weathType, std::int8_t currentTemp, weathid_t currval) {
    // Weather Types
    // 0 - dry
    // 1 - rain
    // 2 - snow
    // 3 - rain and snow
    // 4 - storm is brewing
    // 5 - storm
    //	Byte 1 - 0x65 (Weather)
    //	Byte 2 - Weather Type(  0, rain, 1, fierce storm, 2, snow, 3 a storm is brewing, and 0xFF
    // none ) 	Byte 3 - Particle count (upper limit of 70) 	Byte 4 - Temperature
    
    if (s == nullptr)
        return;
    
    CPWeather dry(0xFF, 0x00, currentTemp);
    CPWeather rain(0x00, static_cast<std::uint8_t>(RainIntensity(currval)), currentTemp);
    CPWeather storm(0x01, static_cast<std::uint8_t>(StormIntensity(currval)), currentTemp);
    CPWeather snow(0x02, static_cast<std::uint8_t>(SnowIntensity(currval)), currentTemp);
    CPWeather strmbrw(0x03, static_cast<std::uint8_t>((StormIntensity(currval) / 2)), currentTemp);
    
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

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::LightMin()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the light (min) value of specified weather system
// o------------------------------------------------------------------------------------------------o
R32 cWeatherAb::LightMin(weathid_t toCheck) { return weather[toCheck].LightMin(); }
void cWeatherAb::LightMin(weathid_t toCheck, R32 newValue) { weather[toCheck].LightMin(newValue); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::LightMax()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the light (max) value of specified weather system
// o------------------------------------------------------------------------------------------------o
R32 cWeatherAb::LightMax(weathid_t toCheck) { return weather[toCheck].LightMax(); }
void cWeatherAb::LightMax(weathid_t toCheck, R32 newValue) { weather[toCheck].LightMax(newValue); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::CurrentLight()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the light (current) value of specified weather system
// o------------------------------------------------------------------------------------------------o
R32 cWeatherAb::CurrentLight(weathid_t toCheck) { return weather[toCheck].CurrentLight(); }
void cWeatherAb::CurrentLight(weathid_t toCheck, R32 newValue) {
    weather[toCheck].CurrentLight(newValue);
}

CWeather *cWeatherAb::Weather(weathid_t toCheck) {
    if (toCheck >= weather.size()) {
        return nullptr;
    }
    else {
        return &weather[toCheck];
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::DoLightEffect()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles effects of light levels on characters affected by light
// o------------------------------------------------------------------------------------------------o
bool cWeatherAb::DoLightEffect(CSocket *mSock, CChar &mChar) {
    bool didDamage = false;
    
    if (mChar.IsInvulnerable() || mChar.IsDead() || !Races->Affect(mChar.GetRace(), LIGHT) ||
        mChar.InBuilding())
        return false;
    
    if (mChar.GetWeathDamage(LIGHT) != 0 && mChar.GetWeathDamage(LIGHT) <= cwmWorldState->GetUICurrentTime()) {
        R32 damageModifier = 0;
        std::int32_t damage = 0;
        R32 baseDamage = static_cast<R32>(Races->Damage(mChar.GetRace(), LIGHT));
        R32 lightLevel = static_cast<R32>(Races->LightLevel(mChar.GetRace()));
        R32 currentLight = 255;
        R32 lightMin = 255;
        R32 lightMax = 255;
        std::int32_t message = 0;
        bool ampm = cwmWorldState->uoTime.ampm;
        
        weathid_t weatherSys = mChar.GetRegion()->GetWeather();
        if (!weather.empty() && weatherSys < weather.size()) {
            lightMin = LightMin(weatherSys);
            lightMax = LightMax(weatherSys);
            
            if (lightMin < 300 && lightMax < 300) {
                currentLight = CurrentLight(weatherSys);
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
            R32 dungeonLight = 255;
            dungeonLight = ServerConfig::shared().ushortValues[UShortValue::DUNGEONLIGHT] ;
            
            if (lightLevel > dungeonLight) {
                if (lightLevel > 0) {
                    damageModifier = (dungeonLight / lightLevel);
                }
                else {
                    damageModifier = 0;
                }
                
                damage = static_cast<std::int32_t>(RoundNumber(baseDamage - (baseDamage * damageModifier)));
                
                if (RoundNumber(damageModifier) > 0) {
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
                
                damage = static_cast<std::int32_t>(RoundNumber(baseDamage - (baseDamage * damageModifier)));
                
                if (RoundNumber(damageModifier) > 0) {
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
        
        damage = Combat->ApplyDefenseModifiers(LIGHT, nullptr, &mChar, 0, 0, damage, true);
        
        if (damage > 0) {
            if (mChar.Damage(damage, LIGHT)) {
                Effects->PlayStaticAnimation((&mChar), 0x3709, 0x09, 0x19);
                Effects->PlaySound((&mChar), 0x0208);
                didDamage = true;
                
                if (message != 0 && mSock != nullptr) {
                    mSock->SysMessage(message);
                }
            }
        }
        mChar.SetWeathDamage(static_cast<std::uint32_t>(BuildTimeValue( static_cast<R32>(Races->Secs(mChar.GetRace(), LIGHT)))), LIGHT);
    }
    return didDamage;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb::doWeatherEffect()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles effects of weather on characters affected by weather
// o------------------------------------------------------------------------------------------------o
bool cWeatherAb::doWeatherEffect(CSocket *mSock, CChar &mChar, weathertype_t element) {
    weathertype_t resistElement = element;
    
    if (element == NONE || element == LIGHT || element == WEATHNUM)
        return false;
    
    if (mChar.IsInvulnerable() || mChar.IsDead() || !Races->Affect(mChar.GetRace(), element) || mChar.InBuilding())
        return false;
    
    bool didDamage = false;
    weathid_t weatherSys = mChar.GetRegion()->GetWeather();
    if (!(weatherSys > weather.size() || weather.empty()) && mChar.GetWeathDamage(element) != 0 && mChar.GetWeathDamage(element) <= cwmWorldState->GetUICurrentTime()) {
        const R32 tempCurrent = Temp(weatherSys);
        // const R32 tempMax		= MaxTemp( weatherSys );
        // const R32 tempMin		= MinTemp( weatherSys );
        // const R32 tempSnowMax	= SnowThreshold( weatherSys );
        const R32 tempEffMax = EffectiveMaxTemp(weatherSys);
        const R32 tempEffMin = EffectiveMinTemp(weatherSys);
        
        R32 damageModifier = 0;
        std::int32_t damage = 0;
        R32 baseDamage = static_cast<R32>(Races->Damage(mChar.GetRace(), element));
        R32 heatLevel = static_cast<R32>(Races->HeatLevel(mChar.GetRace()));
        R32 coldLevel = static_cast<R32>(Races->ColdLevel(mChar.GetRace()));
        
        std::int32_t damageMessage = 0;
        std::uint16_t damageAnim = 0x373A;
        
        if (element == RAIN) {
            damageModifier = static_cast<R32>(RainIntensity(weatherSys));
            damage = static_cast<std::int32_t>(RoundNumber((baseDamage / 100) * damageModifier));
            damageMessage = 1219;
            resistElement = NONE;
        }
        
        if (element == SNOW) {
            damageModifier = static_cast<R32>(SnowIntensity(weatherSys));
            damage = static_cast<std::int32_t>(RoundNumber((baseDamage / 100) * damageModifier));
            damageMessage = 1220;
            // Snow is also cold damage when it comes to resistance values
            resistElement = COLD;
        }
        
        if (element == STORM) {
            damageModifier = static_cast<R32>(StormIntensity(weatherSys));
            damage = static_cast<std::int32_t>(RoundNumber((baseDamage / 100) * damageModifier));
            damageMessage = 1775;
            resistElement = NONE;
            
            if (Races->Affect(mChar.GetRace(), LIGHTNING)) {
                if (static_cast<std::uint8_t>(RandomNum(1, 100)) <=  Races->Secs(mChar.GetRace(), LIGHTNING)) {
                    damage = static_cast<std::int32_t>(Races->Damage(mChar.GetRace(), LIGHTNING));
                    Effects->Bolteffect(&mChar);
                    damageMessage = 1777;
                    damageAnim = 0x0;
                    resistElement = LIGHTNING;
                }
            }
        }
        
        if (element == COLD && tempCurrent <= coldLevel) {
            if ((coldLevel - tempEffMin) != 0) {
                damageModifier = ((tempCurrent - tempEffMin) / (coldLevel - tempEffMin));
            }
            else {
                damageModifier = 0;
            }
            
            damage = static_cast<std::int32_t>(RoundNumber(baseDamage - (baseDamage * damageModifier)));
            damageMessage = 1776; // The freezing cold hurts you!
        }
        
        if (element == HEAT && tempCurrent >= heatLevel) {
            if ((tempEffMax - heatLevel) != 0) {
                damageModifier = ((tempCurrent - heatLevel) / (tempEffMax - heatLevel));
            }
            else {
                damageModifier = 0;
            }
            
            damage = static_cast<std::int32_t>(RoundNumber(baseDamage * damageModifier));
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
        mChar.SetWeathDamage(static_cast<std::uint32_t>(BuildTimeValue(static_cast<R32>(Races->Secs(mChar.GetRace(), element)))), element);
    }
    return didDamage;
}
