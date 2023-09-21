//

#ifndef weather_hpp
#define weather_hpp


#include <algorithm>
#include <array>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <istream>
#include <map>
#include <string>
#include <vector>

#include "typedefs.h"

struct UOTime ;
class CChar ;
class CBaseObject ;
class CSocket ;
class CItem ;
//============================================================================================================
// For whatever reason, damage and weather seem to have been intermixed
// we need to separate these two, into their proper entities.
// For future, but not too far future
//===========================================================================================================
//======================================================================
// Weather (the weather attributes )
//======================================================================
struct Weather {
    enum type_t {
        NONE = 0, PHYSICAL, LIGHT, RAIN,
        COLD, HEAT, LIGHTNING, POISON,
        SNOW, STORM, STORMBREW
    };
    static constexpr auto numberweather = 11 ;  // This is for backward compatiblity, we should eliminate the need
    enum state_t {
        MAX=0,MIN,CURRENT
    };
    enum impact_t {
        TEMP=0,WIND,EFFECTIVETEMP,BRIGHTNESS
    };
    static const std::map<std::string, type_t> NAMETYPEMAP ;
    static auto nameForType(type_t type) ->const std::string& ;
    static auto typeForName(const std::string &name) -> type_t ;
    static auto size() -> size_t ;
    //============================================================
    // MIN, MAX, CURRENT
    struct State {
        std::array<float,3> value ;
        auto operator[](state_t state) const -> const float&{
            return this->value.at(state) ;
        }
        auto operator[](state_t state)  -> float&{
            return this->value.at(state) ;
        }
        State() {
            std::fill(value.begin(),value.end(),0.0) ;
        }
    };
    //============================================================
    // TEMP, WIND, EFFECTIVETEMP,BRIGHTNESS
    struct Impact {
        std::array<State,4> impact ;
        Impact() {std::fill(impact.begin(),impact.end(),State());};
        auto operator[](impact_t impact) const -> const State& {
            return this->impact.at(impact);
        }
        auto operator[](impact_t impact)  -> State& {
            return this->impact.at(impact);
        }
    };
    //============================================================
    //  we have STORMBREW but what we probably want is to add a "pending" to the
    //  structure.  I dont think we need a separate catectory, and then we
    //  we can the other things pending if we want as well.  For later
    //
    //============================================================
    struct Characteristic {
        std::int16_t chance ;
        std::array<std::int16_t,4> intensity ;
        bool active ;
        Characteristic():chance(0),active(false){
            std::fill(intensity.begin(),intensity.end(),0);
        }
        auto operator[](state_t state) const ->const std::int16_t&{
            return this->intensity.at(state);
        }
        auto operator[](state_t state) -> std::int16_t& {
            return this->intensity.at(state);
        }
    };

    std::vector<Characteristic> weather ;
    Impact impact ;
    float snowThreshold ;
    float rainTempDrop ;
    float stormTempDrop ;
    bool stormDelay ;
    
    Weather() ;
    auto operator[](type_t weathertype) const -> const Characteristic& {
        return weather.at(static_cast<int>(weathertype));
    }
    auto operator[](type_t weathertype) ->Characteristic& {
        return weather.at(static_cast<int>(weathertype));
    }
    
    auto update(const UOTime &uotime) ->void ;
    auto newDay() ->void ;
    auto newHour() ->void ;
    auto setValue(const std::string &line) ->void ;
    
private:
    auto setWeatherRange(type_t weathertype,  const std::string &value) ->void ;
    // This should not be needed, if we did the data organization correct
    auto setWeatherRange(type_t weathertype,state_t state,  const std::string &value) ->void ;
    auto setChance(type_t weathertype, const std::string &value) ->void ;
    // This should be a low,high as well, then no state_t is required. for future
    auto setImpact(impact_t impact,state_t state, const std::string &value) ;
};

//=====================================================================
// WorldWeather (the weather for all the regions of the world
//======================================================================
class WorldWeather {
    std::vector<Weather> region ;
    
public:
    WorldWeather() ;
    auto size() const -> size_t {
        return region.size();
    }
    auto operator[](weathid_t id) const ->const Weather& {
        return region.at(id) ;
    }
    auto operator[](weathid_t id) -> Weather& {
        return region.at(id) ;
    }
    
    auto newDay() ->void ;
    auto newHour() ->void ;
    auto update(const UOTime &uotime) ->void ;

    auto load(const std::filesystem::path &path) ->bool ;

    auto doPlayerStuff(CSocket *mSock, CChar *p) ->bool;
    auto doPlayerWeather(CSocket *s, std::uint8_t weathType, std::int8_t currentTemp, weathid_t currval) ->void;
    auto doWeatherEffect(CSocket *mSock, CChar &mChar, Weather::type_t element) ->bool;
    auto doLightEffect(CSocket *mSock, CChar &mChar)->bool;
    auto doNPCStuff(CChar *p)->bool;
    auto doItemStuff(CItem *mItem)->bool;
    auto sendJSWeather(CBaseObject *mObj, Weather::type_t weathType, std::int8_t currentTemp)->void;
    auto pointerForRegion(weathid_t id) -> Weather * {
        if (id < region.size()){
            return &region.at(id) ;
        }
        return nullptr ;
    }

};
#endif /* weather_hpp */
