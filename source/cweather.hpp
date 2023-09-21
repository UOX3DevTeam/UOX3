#ifndef __WeatherAb__
#define __WeatherAb__

#include <cstdint>
#include <string>
#include <vector>

#include "typedefs.h"
#include "type/weather.hpp"

class CBaseObject;
class CChar;
class CItem;

class CWeather {
private:
    struct WeathPart_st {
        std::int8_t Chance;
        std::int8_t Intensity;
        std::int8_t IntensityLow;
        std::int8_t IntensityHigh;
        bool Active;
    };
    
    [[maybe_unused]] weathid_t ID;
    WeathPart_st weather[Weather::numberweather];
    float assortVals[3][3];
    float snowThreshold;
    float light[3];
    float rainTempDrop;
    float stormTempDrop;
    bool stormDelay;
    
public:
    CWeather();
    virtual ~CWeather();
    
    std::int8_t IntensityHigh(std::uint8_t weathType) const;
    void IntensityHigh(std::uint8_t weathType, std::int8_t value);
    std::int8_t IntensityLow(std::uint8_t weathType) const;
    void IntensityLow(std::uint8_t weathType, std::int8_t value);
    std::int8_t Intensity(std::uint8_t weathType) const;
    void Intensity(std::uint8_t weathType, std::int8_t value);
    
    std::int8_t Chance(std::uint8_t weathType) const;
    void Chance(std::uint8_t weathType, std::int8_t value);
    
    float Value(std::uint8_t valType, std::uint8_t valOff) const;
    void Value(std::uint8_t valType, std::uint8_t valOff, float value);
    
    std::int8_t SnowIntensityHigh() const;
    std::int8_t SnowIntensityLow() const;
    std::int8_t SnowIntensity() const;
    std::int8_t RainIntensityHigh() const;
    std::int8_t RainIntensityLow() const;
    std::int8_t RainIntensity() const;
    std::int8_t HeatIntensityHigh() const;
    std::int8_t ColdIntensityHigh() const;
    std::int8_t StormIntensityHigh() const;
    std::int8_t StormIntensityLow() const;
    std::int8_t StormIntensity() const;
    
    float MaxTemp() const;
    float MinTemp() const;
    float Temp() const;
    float MaxWindSpeed() const;
    float MinWindSpeed() const;
    float WindSpeed() const;
    float RainTempDrop() const;
    float StormTempDrop() const;
    
    std::int8_t RainChance() const;
    std::int8_t SnowChance() const;
    std::int8_t StormChance() const;
    std::int8_t HeatChance() const;
    std::int8_t ColdChance() const;
    
    float LightMin() const;
    float LightMax() const;
    float CurrentLight() const;
    
    bool RainActive() const;
    bool SnowActive() const;
    bool StormBrewing() const;
    bool StormActive() const;
    bool HeatActive() const;
    bool ColdActive() const;
    bool StormDelay() const;
    
    float SnowThreshold() const;
    
    void WindSpeed(float value);
    void MaxWindSpeed(float value);
    void MinWindSpeed(float value);
    void MaxTemp(float value);
    void MinTemp(float value);
    void Temp(float value);
    void RainTempDrop(float value);
    void StormTempDrop(float value);
    void SnowIntensityHigh(std::int8_t value);
    void SnowIntensityLow(std::int8_t value);
    void SnowIntensity(std::int8_t value);
    void HeatIntensityHigh(std::int8_t value);
    void RainIntensityHigh(std::int8_t value);
    void RainIntensityLow(std::int8_t value);
    void RainIntensity(std::int8_t value);
    void ColdIntensityHigh(std::int8_t value);
    void StormIntensityHigh(std::int8_t value);
    void StormIntensityLow(std::int8_t value);
    void StormIntensity(std::int8_t value);
    void RainChance(std::int8_t value);
    void SnowChance(std::int8_t value);
    void StormChance(std::int8_t value);
    void HeatChance(std::int8_t value);
    void ColdChance(std::int8_t value);
    void SnowThreshold(float value);
    void LightMin(float value);
    void LightMax(float value);
    void CurrentLight(float value);
    
    float EffectiveMaxTemp() const;
    float EffectiveMinTemp() const;
    void EffectiveMaxTemp(float value);
    void EffectiveMinTemp(float value);
    
    void RainActive(bool value);
    void SnowActive(bool value);
    void StormBrewing(bool value);
    void StormActive(bool value);
    void HeatActive(bool value);
    void ColdActive(bool value);
    void StormDelay(bool value);
    
    void NewDay();
    void NewHour();
    bool PeriodicUpdate();
};

class cWeatherAb {
private:
    std::vector<CWeather> weather;
    std::int8_t IntensityHigh(weathid_t toCheck, std::uint8_t weathType);
    void IntensityHigh(weathid_t toCheck, std::uint8_t weathType, std::int8_t value);
    std::int8_t IntensityLow(weathid_t toCheck, std::uint8_t weathType);
    void IntensityLow(weathid_t toCheck, std::uint8_t weathType, std::int8_t value);
    std::int8_t Intensity(weathid_t toCheck, std::uint8_t weathType);
    void Intensity(weathid_t toCheck, std::uint8_t weathType, std::int8_t value);
    std::int8_t Chance(weathid_t toCheck, std::uint8_t weathType);
    void Chance(weathid_t toCheck, std::uint8_t weathType, std::int8_t value);
    float Value(weathid_t toCheck, std::uint8_t valType, std::uint8_t valOff);
    void Value(weathid_t toCheck, std::uint8_t valType, std::uint8_t valOff, float value);
    
public:
    cWeatherAb();
    ~cWeatherAb();
    bool load();
    bool NewDay();
    bool NewHour();
    size_t Count() const;
    
    std::int8_t SnowIntensityHigh(weathid_t toCheck);
    std::int8_t SnowIntensityLow(weathid_t toCheck);
    std::int8_t SnowIntensity(weathid_t toCheck);
    std::int8_t RainIntensityHigh(weathid_t toCheck);
    std::int8_t RainIntensityLow(weathid_t toCheck);
    std::int8_t RainIntensity(weathid_t toCheck);
    std::int8_t HeatIntensityHigh(weathid_t toCheck);
    std::int8_t ColdIntensityHigh(weathid_t toCheck);
    std::int8_t StormIntensityHigh(weathid_t toCheck);
    std::int8_t StormIntensityLow(weathid_t toCheck);
    std::int8_t StormIntensity(weathid_t toCheck);
    float MaxTemp(weathid_t toCheck);
    float MinTemp(weathid_t toCheck);
    float Temp(weathid_t toCheck);
    float RainTempDrop(weathid_t toCheck);
    float StormTempDrop(weathid_t toCheck);
    float MaxWindSpeed(weathid_t toCheck);
    float MinWindSpeed(weathid_t toCheck);
    float WindSpeed(weathid_t toCheck);
    std::int8_t RainChance(weathid_t toCheck);
    std::int8_t SnowChance(weathid_t toCheck);
    std::int8_t HeatChance(weathid_t toCheck);
    std::int8_t ColdChance(weathid_t toCheck);
    std::int8_t StormChance(weathid_t toCheck);
    float LightMin(weathid_t toCheck);
    float LightMax(weathid_t toCheck);
    float CurrentLight(weathid_t toCheck);
    bool RainActive(weathid_t toCheck);
    bool SnowActive(weathid_t toCheck);
    bool StormBrewing(weathid_t toCheck);
    bool StormActive(weathid_t toCheck);
    bool HeatActive(weathid_t toCheck);
    bool ColdActive(weathid_t toCheck);
    
    float SnowThreshold(weathid_t toCheck);
    
    void WindSpeed(weathid_t toCheck, float value);
    void MaxWindSpeed(weathid_t toCheck, float value);
    void MinWindSpeed(weathid_t toCheck, float value);
    void MaxTemp(weathid_t toCheck, float value);
    void MinTemp(weathid_t toCheck, float value);
    void Temp(weathid_t toCheck, float value);
    void RainTempDrop(weathid_t toCheck, float value);
    void StormTempDrop(weathid_t toCheck, float value);
    void SnowIntensityHigh(weathid_t toCheck, std::int8_t value);
    void SnowIntensityLow(weathid_t toCheck, std::int8_t value);
    void SnowIntensity(weathid_t toCheck, std::int8_t value);
    void RainIntensityHigh(weathid_t toCheck, std::int8_t value);
    void RainIntensityLow(weathid_t toCheck, std::int8_t value);
    void RainIntensity(weathid_t toCheck, std::int8_t value);
    void HeatIntensityHigh(weathid_t toCheck, std::int8_t value);
    void ColdIntensityHigh(weathid_t toCheck, std::int8_t value);
    void StormIntensityHigh(weathid_t toCheck, std::int8_t value);
    void StormIntensityLow(weathid_t toCheck, std::int8_t value);
    void StormIntensity(weathid_t toCheck, std::int8_t value);
    void RainChance(weathid_t toCheck, std::int8_t value);
    void SnowChance(weathid_t toCheck, std::int8_t value);
    void HeatChance(weathid_t toCheck, std::int8_t value);
    void ColdChance(weathid_t toCheck, std::int8_t value);
    void StormChance(weathid_t toCheck, std::int8_t value);
    void SnowThreshold(weathid_t toCheck, float value);
    void LightMin(weathid_t toCheck, float newValue);
    void LightMax(weathid_t toCheck, float newValue);
    void CurrentLight(weathid_t toCheck, float newValue);
    
    float EffectiveMaxTemp(weathid_t toCheck);
    float EffectiveMinTemp(weathid_t toCheck);
    void EffectiveMaxTemp(weathid_t toCheck, float value);
    void EffectiveMinTemp(weathid_t toCheck, float value);
    void RainActive(weathid_t toCheck, bool value);
    void SnowActive(weathid_t toCheck, bool value);
    void StormBrewing(weathid_t toCheck, bool value);
    void StormActive(weathid_t toCheck, bool value);
    void HeatActive(weathid_t toCheck, bool value);
    void ColdActive(weathid_t toCheck, bool value);
    bool DoStuff();
    bool DoPlayerStuff(CSocket *mSock, CChar *p);
    void DoPlayerWeather(CSocket *s, std::uint8_t weathType, std::int8_t currentTemp, weathid_t currval);
    bool doWeatherEffect(CSocket *mSock, CChar &mChar, Weather::type_t element);
    bool DoLightEffect(CSocket *mSock, CChar &mChar);
    bool DoNPCStuff(CChar *p);
    bool DoItemStuff(CItem *p);
    void SendJSWeather(CBaseObject *mObj, Weather::type_t weathType, std::int8_t currentTemp);
    
    CWeather *Weather(weathid_t toCheck);
};

extern cWeatherAb *Weather;

#endif
