//

#ifndef uotime_hpp
#define uotime_hpp

#include <array>
#include <cstdint>
#include <iostream>
#include <filesystem>
#include <istream>
#include <ostream>
#include <string>

//======================================================================
struct UOTime {
    std::array<std::int16_t,2> moon ; //    Moon current state
    bool ampm ; //    Whether our current time is in the morning or afternoon
    std::uint16_t worldLightLevel ;
    std::uint8_t seconds;            //    Number of seconds the world has been running (persistent)
    std::uint8_t minutes;            //    Number of minutes the world has been running (persistent)
    std::uint8_t hours;              //    Number of hours the world has been running (persistent)
    std::int16_t days;               //    Number of days the world has been running (persistent)
    UOTime() ;
    [[maybe_unused]] auto load(const std::filesystem::path &savepath) -> bool ;
    auto load(std::istream &input) -> void ;
    [[maybe_unused]] auto save(const std::filesystem::path &writepath) -> bool ;
    auto save(std::ostream &output) ->void ;
    
    auto incrementDay() ->bool ;
    auto incrementHour() ->bool ;
    auto incrementMinute() ->bool ;
    auto incrementSecond() ->bool ;
    auto incrementMoon(int moon)->void;
};

#endif /* uotime_hpp */
