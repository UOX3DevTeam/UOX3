// Created on:  3/28/21

#ifndef TimeUtility_hpp
#define TimeUtility_hpp

#include <cstdint>
#include <string>
#include <chrono>

namespace timeutil
{
	std::string timeNow();
	
	struct Interval_st
	{
	private:
		std::chrono::time_point<std::chrono::steady_clock> startTime;
		
	public:
		Interval_st();
		void Start();
		long long Elapsed();
		long long Stop();
	};
}

#endif /* TimeUtility_hpp */
