// Created on:  3/28/21

#include "TimeUtility.hpp"

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <chrono>
//#include <ctime>
#include <time.h>
#include <sstream>
#include <iomanip>
#include "ConfigOS.h"

namespace timeutil
{
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//
	//		Methods for TimeUtilities
	//
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	
	std::string timeNow()
	{
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
	
	//o-----------------------------------------------------------------------------------------------o
	//|	Function	-	Interval_st::Interval_st()
	//o-----------------------------------------------------------------------------------------------o
	//|	Purpose		-	Constructor
	//o-----------------------------------------------------------------------------------------------o
	Interval_st::Interval_st()
	{
		startTime = std::chrono::steady_clock::now();
	}
	
	//o-----------------------------------------------------------------------------------------------o
	//|	Function	-	Interval_st::Start()
	//o-----------------------------------------------------------------------------------------------o
	//|	Purpose		-	Set a start time point representing current time
	//o-----------------------------------------------------------------------------------------------o
	void Interval_st::Start()
	{
		startTime = std::chrono::steady_clock::now();
	}

	//o-----------------------------------------------------------------------------------------------o
	//|	Function	-	Interval_st::Elapsed()
	//o-----------------------------------------------------------------------------------------------o
	//|	Purpose		-	Return elapsed time since start time was set
	//o-----------------------------------------------------------------------------------------------o
	long long Interval_st::Elapsed()
	{
		auto end = std::chrono::steady_clock::now();
		auto diff = end - startTime;
		return std::chrono::duration_cast<std::chrono::milliseconds>( diff ).count();
	}

	//o-----------------------------------------------------------------------------------------------o
	//|	Function	-	Interval_st::Stop()
	//o-----------------------------------------------------------------------------------------------o
	//|	Purpose		-	Reset start time and return time elapsed since start time was previously set
	//o-----------------------------------------------------------------------------------------------o
	long long Interval_st::Stop()
	{
		auto end = std::chrono::steady_clock::now();
		auto diff = end - startTime;
		startTime = std::chrono::steady_clock::now();
		return std::chrono::duration_cast<std::chrono::milliseconds>( diff ).count();
	}
}
