//Copyright © 2023 Charles Kerr. All rights reserved.

#ifndef servertime_hpp
#define servertime_hpp

#include <array>
#include <chrono>
#include <iostream>
#include <string>
//======================================================================
// ServerMoonPhase
//======================================================================
class ServerMoonPhase {
public:
    static constexpr auto MOONCOUNT = 2 ;
private:
    std::array<int,MOONCOUNT> moon ;
public:
    ServerMoonPhase() ;
    ServerMoonPhase(const std::string &moonPhaseValue);
    
    auto describe() const -> std::string ;
    
    auto operator[](std::size_t index) const -> const int& ;
    auto operator[](std::size_t index) -> int& ;
    
    [[maybe_unused]] auto incPhase(int moonNumber,int amount = 1) -> int ;

};

//======================================================================
// ServerTimeData
//======================================================================
class ServerTimeData {
public:
    static constexpr auto MOONCOUNT = 2 ;
private:
    int day ;
    int hour ;
    int minute ;
    int second ;
    bool isPM ;
    ServerMoonPhase moon ;
    
    auto incDay() ->bool ;
    auto incHour() ->bool ;
    auto incMinute() -> bool ;

public:
    static constexpr auto DAYINYEAR = 356 ;
    static constexpr auto HOURINDAY = 24 ;
    static constexpr auto MINUTEINHOUR = 60 ;
    static constexpr auto SECONDINMINUTE = 60 ;
    
    
    ServerTimeData() ;
    ServerTimeData(const std::string &timeValue) ;
    
    auto describe() const -> std::string ;
    
    auto operator<(const ServerTimeData &value) const -> bool ;
    auto operator==(const ServerTimeData &value) const -> bool ;
    auto operator<=(const ServerTimeData &value) const -> bool ;
    auto operator!=(const ServerTimeData &value) const -> bool ;
    /**
     Increments the server time by the number of seconds
     - Parameters:
        - seconds: The number of seconds to increment
     - Returns: true if the day has changed
     */
    [[maybe_unused]] auto incrementTime(int seconds=1) -> bool ;
    
    auto operator+(int seconds) const -> ServerTimeData ;
};

//======================================================================
struct ServerTime {
    static constexpr auto MILLISECONDPERTICK =  1000 ;
    std::chrono::steady_clock::time_point lastTick ;
    ServerTimeData timeData ;
    bool firstTick ;
    ServerTime() ;
    ServerTime(const std::string &timeValue) ;
    auto resetTickClock() -> void ;
    [[maybe_unused]] auto tick() -> bool ;
    
    auto describe() const -> std::string ;
    auto now() const -> const ServerTimeData& ;
    
    auto operator<(const ServerTimeData &time) const ->bool ;
    
};

#endif /* servertime_hpp */
