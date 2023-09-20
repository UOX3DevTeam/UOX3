//

#ifndef weather_hpp
#define weather_hpp


#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <map>
#include <string>

struct UOTime ;

//======================================================================
struct Weather {
    enum type_t {
        NONE = 0, PHYSICAL, LIGHT, RAIN,
        COLD, HEAT, LIGHTNING, POISON,
        SNOW, STORM, STORMBREW
    };
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
            return this->value[state] ;
        }
        auto operator[](state_t state)  -> float&{
            return this->value[state] ;
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
            return this->impact[impact];
        }
        auto operator[](impact_t impact)  -> State& {
            return this->impact[impact];
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
            return this->intensity[state];
        }
        auto operator[](state_t state) -> std::int16_t& {
            return this->intensity[state];
        }
    };

    std::vector<Characteristic> weather ;
    Impact impact ;
    float snowThreshold ;
    float rainTempDrop ;
    float stormTempDrop ;
    bool stormDelay ;
    
    Weather() ;
    auto operator[](type_t weathertype) const -> const Characteristic& ;
    auto operator[](type_t weathertype) ->Characteristic& ;
    
    auto update(const UOTime &uotime) ->void ;
    auto newDay() ->void ;
    auto newHour() ->void ;
};

#endif /* weather_hpp */
