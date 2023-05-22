//Copyright © 2023 Charles Kerr. All rights reserved.

#include "timer.hpp"

#include <iostream>
#include <stdexcept>
#include <thread>
#include <sstream>
#include <time.h>
#include <iomanip>

using namespace std::string_literals;

namespace util{
    
    //=================================================================================
    auto timer_t::now() ->std::string {
        std::stringstream msg;
        auto now_time = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
        struct tm buffer;
#if defined(_WIN32)
        localtime_s( &buffer, &now_time );
        auto now_tm = &buffer;
#else
        auto now_tm = localtime_r( &now_time, &buffer );
#endif
        msg << std::put_time( now_tm, "%c" );
        return msg.str();
     }
    
    //=================================================================================
    auto timer_t::current() ->std::chrono::system_clock::time_point {
        return std::chrono::system_clock::now() ;
    }
    
    //=================================================================================
    auto timer_t::delta(const std::chrono::steady_clock::time_point &time) ->std::int64_t {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - time).count();
    }
    
    //=================================================================================
    timer_t::timer_t(){
        start_time=std::chrono::steady_clock::now();
        time_duration = 0 ;
    }
    //=================================================================================
    timer_t::timer_t(std::int64_t milliseconds,bool block):timer_t(){
        time(milliseconds,block);
    }
    //=================================================================================
    auto timer_t::time(std::int64_t milliseconds,bool block)->void {
        start_time=std::chrono::steady_clock::now();
        time_duration = milliseconds;
        if (block){
            std::this_thread::sleep_for(std::chrono::duration<std::int64_t,std::milli>(milliseconds));
        }
    }
    
    //================================================================================
    auto timer_t::start() ->void {
        start_time=std::chrono::steady_clock::now();
        time_duration = 0 ;
    }
    //=================================================================================
    auto timer_t::elapsed() const ->std::int64_t {
        auto now = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();
       
    }
    
    //=================================================================================
    auto timer_t::expired() const ->bool{
        return elapsed()>= time_duration;
    }
    //=================================================================================
    auto timer_t::remaining() const -> std::int64_t {
        return time_duration - elapsed();
    }
}
