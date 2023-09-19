// Created on:  3/28/21

#ifndef TimeUtility_hpp
#define TimeUtility_hpp

#include <chrono>
#include <cstdint>
#include <string>

namespace timeutil {
    std::string timeNow();
    
    struct Interval_st {
    private:
        std::chrono::time_point<std::chrono::steady_clock> startTime;
        
    public:
        Interval_st();
        void start();
        long long Elapsed();
        long long Stop();
    };
} // namespace timeutil

#endif /* TimeUtility_hpp */
