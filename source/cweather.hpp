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

    [[maybe_unused]] WEATHID ID;
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
    std::int8_t IntensityHigh(WEATHID toCheck, std::uint8_t weathType);
    void IntensityHigh(WEATHID toCheck, std::uint8_t weathType, std::int8_t value);
    std::int8_t IntensityLow(WEATHID toCheck, std::uint8_t weathType);
    void IntensityLow(WEATHID toCheck, std::uint8_t weathType, std::int8_t value);
    std::int8_t Intensity(WEATHID toCheck, std::uint8_t weathType);
    void Intensity(WEATHID toCheck, std::uint8_t weathType, std::int8_t value);
    std::int8_t Chance(WEATHID toCheck, std::uint8_t weathType);
    void Chance(WEATHID toCheck, std::uint8_t weathType, std::int8_t value);
    R32 Value(WEATHID toCheck, std::uint8_t valType, std::uint8_t valOff);
    void Value(WEATHID toCheck, std::uint8_t valType, std::uint8_t valOff, R32 value);

  public:
    cWeatherAb();
    ~cWeatherAb();
    bool Load(void);
    bool NewDay(void);
    bool NewHour(void);
    size_t Count(void) const;

    std::int8_t SnowIntensityHigh(WEATHID toCheck);
    std::int8_t SnowIntensityLow(WEATHID toCheck);
    std::int8_t SnowIntensity(WEATHID toCheck);
    std::int8_t RainIntensityHigh(WEATHID toCheck);
    std::int8_t RainIntensityLow(WEATHID toCheck);
    std::int8_t RainIntensity(WEATHID toCheck);
    std::int8_t HeatIntensityHigh(WEATHID toCheck);
    std::int8_t ColdIntensityHigh(WEATHID toCheck);
    std::int8_t StormIntensityHigh(WEATHID toCheck);
    std::int8_t StormIntensityLow(WEATHID toCheck);
    std::int8_t StormIntensity(WEATHID toCheck);
    R32 MaxTemp(WEATHID toCheck);
    R32 MinTemp(WEATHID toCheck);
    R32 Temp(WEATHID toCheck);
    R32 RainTempDrop(WEATHID toCheck);
    R32 StormTempDrop(WEATHID toCheck);
    R32 MaxWindSpeed(WEATHID toCheck);
    R32 MinWindSpeed(WEATHID toCheck);
    R32 WindSpeed(WEATHID toCheck);
    std::int8_t RainChance(WEATHID toCheck);
    std::int8_t SnowChance(WEATHID toCheck);
    std::int8_t HeatChance(WEATHID toCheck);
    std::int8_t ColdChance(WEATHID toCheck);
    std::int8_t StormChance(WEATHID toCheck);
    R32 LightMin(WEATHID toCheck);
    R32 LightMax(WEATHID toCheck);
    R32 CurrentLight(WEATHID toCheck);
    bool RainActive(WEATHID toCheck);
    bool SnowActive(WEATHID toCheck);
    bool StormBrewing(WEATHID toCheck);
    bool StormActive(WEATHID toCheck);
    bool HeatActive(WEATHID toCheck);
    bool ColdActive(WEATHID toCheck);

    R32 SnowThreshold(WEATHID toCheck);

    void WindSpeed(WEATHID toCheck, R32 value);
    void MaxWindSpeed(WEATHID toCheck, R32 value);
    void MinWindSpeed(WEATHID toCheck, R32 value);
    void MaxTemp(WEATHID toCheck, R32 value);
    void MinTemp(WEATHID toCheck, R32 value);
    void Temp(WEATHID toCheck, R32 value);
    void RainTempDrop(WEATHID toCheck, R32 value);
    void StormTempDrop(WEATHID toCheck, R32 value);
    void SnowIntensityHigh(WEATHID toCheck, std::int8_t value);
    void SnowIntensityLow(WEATHID toCheck, std::int8_t value);
    void SnowIntensity(WEATHID toCheck, std::int8_t value);
    void RainIntensityHigh(WEATHID toCheck, std::int8_t value);
    void RainIntensityLow(WEATHID toCheck, std::int8_t value);
    void RainIntensity(WEATHID toCheck, std::int8_t value);
    void HeatIntensityHigh(WEATHID toCheck, std::int8_t value);
    void ColdIntensityHigh(WEATHID toCheck, std::int8_t value);
    void StormIntensityHigh(WEATHID toCheck, std::int8_t value);
    void StormIntensityLow(WEATHID toCheck, std::int8_t value);
    void StormIntensity(WEATHID toCheck, std::int8_t value);
    void RainChance(WEATHID toCheck, std::int8_t value);
    void SnowChance(WEATHID toCheck, std::int8_t value);
    void HeatChance(WEATHID toCheck, std::int8_t value);
    void ColdChance(WEATHID toCheck, std::int8_t value);
    void StormChance(WEATHID toCheck, std::int8_t value);
    void SnowThreshold(WEATHID toCheck, R32 value);
    void LightMin(WEATHID toCheck, R32 newValue);
    void LightMax(WEATHID toCheck, R32 newValue);
    void CurrentLight(WEATHID toCheck, R32 newValue);

    R32 EffectiveMaxTemp(WEATHID toCheck);
    R32 EffectiveMinTemp(WEATHID toCheck);
    void EffectiveMaxTemp(WEATHID toCheck, R32 value);
    void EffectiveMinTemp(WEATHID toCheck, R32 value);
    void RainActive(WEATHID toCheck, bool value);
    void SnowActive(WEATHID toCheck, bool value);
    void StormBrewing(WEATHID toCheck, bool value);
    void StormActive(WEATHID toCheck, bool value);
    void HeatActive(WEATHID toCheck, bool value);
    void ColdActive(WEATHID toCheck, bool value);
    bool DoStuff(void);
    bool DoPlayerStuff(CSocket *mSock, CChar *p);
    void DoPlayerWeather(CSocket *s, std::uint8_t weathType, std::int8_t currentTemp, WEATHID currval);
    bool doWeatherEffect(CSocket *mSock, CChar &mChar, WeatherType element);
    bool DoLightEffect(CSocket *mSock, CChar &mChar);
    bool DoNPCStuff(CChar *p);
    bool DoItemStuff(CItem *p);
    void SendJSWeather(CBaseObject *mObj, WeatherType weathType, std::int8_t currentTemp);

    CWeather *Weather(WEATHID toCheck);
};

extern cWeatherAb *Weather;

#endif
