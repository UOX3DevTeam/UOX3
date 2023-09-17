//Copyright © 2023 Charles Kerr. All rights reserved.

#include "servertime.hpp"
#include <algorithm>

#include "utility/strutil.hpp"

using namespace std::string_literals ;


//======================================================================
// ServerMoonPhase
//======================================================================
//======================================================================
ServerMoonPhase::ServerMoonPhase() {
    std::fill(moon.begin(),moon.end(),0) ;
}
//======================================================================
ServerMoonPhase::ServerMoonPhase(const std::string &moonPhaseValue):ServerMoonPhase(){
    auto values = util::parse(moonPhaseValue, ",");
    for (std::size_t i = 0 ; i < values.size();i++){
        if (!values[i].empty()){
            moon[i] = std::stoi(values[i]) ;
        }
    }
}

//======================================================================
auto ServerMoonPhase::describe() const -> std::string {
    auto rvalue = std::string() ;
    for (const auto &phase:moon){
        if (!rvalue.empty()){
            rvalue += ","s ;
        }
        rvalue += std::to_string(phase) ;
    }
    return rvalue ;
}
//======================================================================
auto ServerMoonPhase::operator[](std::size_t index) const -> const int& {
    return moon[index];
}
//======================================================================
auto ServerMoonPhase::operator[](std::size_t index) -> int& {
    return moon[index];
}
//======================================================================
auto ServerMoonPhase::incPhase(int moonNumber,int amount) -> int {
    for (auto i = 0 ; i< amount ; i++){
        moon[moonNumber] =  (moon[moonNumber]+1) % 8 ;
    }
    return  moon[moonNumber];
}


//======================================================================
// ServerTimeData
//======================================================================
//=====================================================================
auto ServerTimeData::incDay() ->bool  {
    day += 1 ;
    
    if (day == DAYINYEAR){
        day = 0 ;
    }
    return true ;
}
//=====================================================================
auto ServerTimeData::incHour() ->bool {
    auto rvalue = false ;
    hour += 1 ;
    if (hour >= HOURINDAY/2){
        hour = 0 ;
        auto toset = true ;
        if ( (hour + (isPM?HOURINDAY/2:0)) >= HOURINDAY){
            rvalue = incDay() ;
            toset = false ;
        }
        isPM = toset ;
    }
    return rvalue ;
}
//=====================================================================
auto ServerTimeData::incMinute() -> bool {
    minute++ ;
    auto rvalue = false ;
    if (minute%8 == 0) {
        moon.incPhase(0 );
    }
    if (minute%3 == 0) {
        moon.incPhase(1 );
    }
    if (minute == MINUTEINHOUR){
        minute = 0 ;
        rvalue = incHour();
    }
    return rvalue ;
}

//======================================================================
ServerTimeData::ServerTimeData() {
    day = 0 ;
    hour = 0 ;
    minute = 0 ;
    second = 0 ;
    isPM = false ;
}
//======================================================================
ServerTimeData::ServerTimeData(const std::string &timeValue):ServerTimeData(){
    auto [timeData,phaseData] = util::split(timeValue,":") ;
    moon = ServerMoonPhase(phaseData);
    auto values = util::parse(timeData, ",") ;
    switch (values.size()){
        default:
        case 5:
            isPM = static_cast<bool>(std::stoi(values[4])) ;
            [[fallthrough]];
        case 4:
            second = std::stoi(values[3]) ;
            [[fallthrough]];
        case 3:
            minute = std::stoi(values[2]) ;
            [[fallthrough]];
        case 2:
            hour = std::stoi(values[1]) ;
            [[fallthrough]];
        case 1:
            day = std::stoi(values[0]) ;
            [[fallthrough]];
        case 0:
            break;
    }
}

//=====================================================================
auto ServerTimeData::describe() const -> std::string {
    return std::to_string(day)+","s+std::to_string(hour)+","s+std::to_string(minute)+","s+std::to_string(second)+","s+std::to_string(isPM) + ":"s + moon.describe();
}
//=====================================================================
auto ServerTimeData::operator<(const ServerTimeData &value) const -> bool {
    auto rvalue = false ;
        if (day < value.day){
            if (hour < value.hour){
                if (minute < value.minute){
                    if (second < value.second){
                        rvalue = true ;
                    }
                }
            }
        }
    return rvalue ;
}
//=====================================================================
auto ServerTimeData::operator==(const ServerTimeData &value) const -> bool {
    auto rvalue = false ;
        if (day == value.day){
            if (hour == value.hour){
                if (minute == value.minute){
                    if (second == value.second){
                        rvalue = true ;
                    }
                }
            }
        }
    return rvalue ;
}
//=====================================================================
auto ServerTimeData::operator<=(const ServerTimeData &value) const -> bool {
    auto rvalue = false ;
        if (day <= value.day){
            if (hour <= value.hour){
                if (minute <= value.minute){
                    if (second <= value.second){
                        rvalue = true ;
                    }
                }
            }
        }
    return rvalue ;
}
//=====================================================================
auto ServerTimeData::operator!=(const ServerTimeData &value) const -> bool {
    return !operator==(value) ;
}

//=====================================================================
auto ServerTimeData::incrementTime(int seconds) -> bool {
    auto temp = false ;
    for (auto i = 0 ; i < seconds; ++i){
        if (incMinute()){
            temp = true ;
        }
    }
    return temp ;
}
//=====================================================================
auto ServerTimeData::operator+(int seconds) const -> ServerTimeData {
    auto temp = ServerTimeData(*this) ;
    temp.incrementTime(seconds) ;
    return temp ;
}

//======================================================================
// ServerTime
//======================================================================
//=====================================================================
ServerTime::ServerTime(){
    firstTick = true ;
    lastTick = std::chrono::steady_clock::now() ;
}
//=====================================================================
ServerTime::ServerTime(const std::string &timeValue):ServerTime(){
    timeData = ServerTimeData(timeValue) ;
}
//=====================================================================
auto ServerTime::resetTickClock() ->void {
    firstTick= true ;
}
//=====================================================================
auto ServerTime::tick() ->bool {
    
    auto count = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-lastTick).count()/MILLISECONDPERTICK);
    if (firstTick){
        count = 1 ;
    }
    lastTick = std::chrono::steady_clock::now() ;
    
    return timeData.incrementTime(count) ;
    
}

//=====================================================================
auto ServerTime::describe() const -> std::string {
    return timeData.describe();
}
//=====================================================================
auto ServerTime::operator<(const ServerTimeData &time) const ->bool {
    return timeData < time ;
}
