#ifndef __WeatherAb__
#define __WeatherAb__

#include <cstdint>
#include <string>
#include <vector>

#include "typedefs.h"

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
    WeathPart_st weather[WEATHNUM];
    R32 assortVals[3][3];
    R32 snowThreshold;
    R32 light[3];
    R32 rainTempDrop;
    R32 stormTempDrop;
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
    
    R32 Value(std::uint8_t valType, std::uint8_t valOff) const;
    void Value(std::uint8_t valType, std::uint8_t valOff, R32 value);
    
    std::int8_t SnowIntensityHigh(void) const;
    std::int8_t SnowIntensityLow(void) const;
    std::int8_t SnowIntensity(void) const;
    std::int8_t RainIntensityHigh(void) const;
    std::int8_t RainIntensityLow(void) const;
    std::int8_t RainIntensity(void) const;
    std::int8_t HeatIntensityHigh(void) const;
    std::int8_t ColdIntensityHigh(void) const;
    std::int8_t StormIntensityHigh(void) const;
    std::int8_t StormIntensityLow(void) const;
    std::int8_t StormIntensity(void) const;
    
    R32 MaxTemp(void) const;
    R32 MinTemp(void) const;
    R32 Temp(void) const;
    R32 MaxWindSpeed(void) const;
    R32 MinWindSpeed(void) const;
    R32 WindSpeed(void) const;
    R32 RainTempDrop(void) const;
    R32 StormTempDrop(void) const;
    
    std::int8_t RainChance(void) const;
    std::int8_t SnowChance(void) const;
    std::int8_t StormChance(void) const;
    std::int8_t HeatChance(void) const;
    std::int8_t ColdChance(void) const;
    
    R32 LightMin(void) const;
    R32 LightMax(void) const;
    R32 CurrentLight(void) const;
    
    bool RainActive(void) const;
    bool SnowActive(void) const;
    bool StormBrewing(void) const;
    bool StormActive(void) const;
    bool HeatActive(void) const;
    bool ColdActive(void) const;
    bool StormDelay(void) const;
    
    R32 SnowThreshold(void) const;
    
    void WindSpeed(R32 value);
    void MaxWindSpeed(R32 value);
    void MinWindSpeed(R32 value);
    void MaxTemp(R32 value);
    void MinTemp(R32 value);
    void Temp(R32 value);
    void RainTempDrop(R32 value);
    void StormTempDrop(R32 value);
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
    void SnowThreshold(R32 value);
    void LightMin(R32 value);
    void LightMax(R32 value);
    void CurrentLight(R32 value);
    
    R32 EffectiveMaxTemp(void) const;
    R32 EffectiveMinTemp(void) const;
    void EffectiveMaxTemp(R32 value);
    void EffectiveMinTemp(R32 value);
    
    void RainActive(bool value);
    void SnowActive(bool value);
    void StormBrewing(bool value);
    void StormActive(bool value);
    void HeatActive(bool value);
    void ColdActive(bool value);
    void StormDelay(bool value);
    
    void NewDay(void);
    void NewHour(void);
    bool PeriodicUpdate(void);
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
    R32 Value(weathid_t toCheck, std::uint8_t valType, std::uint8_t valOff);
    void Value(weathid_t toCheck, std::uint8_t valType, std::uint8_t valOff, R32 value);
    
public:
    cWeatherAb();
    ~cWeatherAb();
    bool Load(void);
    bool NewDay(void);
    bool NewHour(void);
    size_t Count(void) const;
    
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
    R32 MaxTemp(weathid_t toCheck);
    R32 MinTemp(weathid_t toCheck);
    R32 Temp(weathid_t toCheck);
    R32 RainTempDrop(weathid_t toCheck);
    R32 StormTempDrop(weathid_t toCheck);
    R32 MaxWindSpeed(weathid_t toCheck);
    R32 MinWindSpeed(weathid_t toCheck);
    R32 WindSpeed(weathid_t toCheck);
    std::int8_t RainChance(weathid_t toCheck);
    std::int8_t SnowChance(weathid_t toCheck);
    std::int8_t HeatChance(weathid_t toCheck);
    std::int8_t ColdChance(weathid_t toCheck);
    std::int8_t StormChance(weathid_t toCheck);
    R32 LightMin(weathid_t toCheck);
    R32 LightMax(weathid_t toCheck);
    R32 CurrentLight(weathid_t toCheck);
    bool RainActive(weathid_t toCheck);
    bool SnowActive(weathid_t toCheck);
    bool StormBrewing(weathid_t toCheck);
    bool StormActive(weathid_t toCheck);
    bool HeatActive(weathid_t toCheck);
    bool ColdActive(weathid_t toCheck);
    
    R32 SnowThreshold(weathid_t toCheck);
    
    void WindSpeed(weathid_t toCheck, R32 value);
    void MaxWindSpeed(weathid_t toCheck, R32 value);
    void MinWindSpeed(weathid_t toCheck, R32 value);
    void MaxTemp(weathid_t toCheck, R32 value);
    void MinTemp(weathid_t toCheck, R32 value);
    void Temp(weathid_t toCheck, R32 value);
    void RainTempDrop(weathid_t toCheck, R32 value);
    void StormTempDrop(weathid_t toCheck, R32 value);
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
    void SnowThreshold(weathid_t toCheck, R32 value);
    void LightMin(weathid_t toCheck, R32 newValue);
    void LightMax(weathid_t toCheck, R32 newValue);
    void CurrentLight(weathid_t toCheck, R32 newValue);
    
    R32 EffectiveMaxTemp(weathid_t toCheck);
    R32 EffectiveMinTemp(weathid_t toCheck);
    void EffectiveMaxTemp(weathid_t toCheck, R32 value);
    void EffectiveMinTemp(weathid_t toCheck, R32 value);
    void RainActive(weathid_t toCheck, bool value);
    void SnowActive(weathid_t toCheck, bool value);
    void StormBrewing(weathid_t toCheck, bool value);
    void StormActive(weathid_t toCheck, bool value);
    void HeatActive(weathid_t toCheck, bool value);
    void ColdActive(weathid_t toCheck, bool value);
    bool DoStuff(void);
    bool DoPlayerStuff(CSocket *mSock, CChar *p);
    void DoPlayerWeather(CSocket *s, std::uint8_t weathType, std::int8_t currentTemp, weathid_t currval);
    bool doWeatherEffect(CSocket *mSock, CChar &mChar, weathertype_t element);
    bool DoLightEffect(CSocket *mSock, CChar &mChar);
    bool DoNPCStuff(CChar *p);
    bool DoItemStuff(CItem *p);
    void SendJSWeather(CBaseObject *mObj, weathertype_t weathType, std::int8_t currentTemp);
    
    CWeather *Weather(weathid_t toCheck);
};

extern cWeatherAb *Weather;

#endif
