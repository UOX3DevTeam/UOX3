// Created on:  3/28/21

#ifndef TimeUtility_hpp
#define TimeUtility_hpp

#include <cstdint>
#include <string>
#include <chrono>

namespace timeutil {
	std::string timenow();
	
	struct Interval {
	private:
		std::chrono::time_point<std::chrono::steady_clock> start_time ;
		
	public:
		Interval();
		void start() ;
		long long elapsed() ;
		long long stop() ;
	};
}

#endif /* TimeUtility_hpp */
