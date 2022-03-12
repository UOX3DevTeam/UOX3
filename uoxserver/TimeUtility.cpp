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

namespace timeutil {
	//+++++++++++++++++++++++++++++++++++++++++++++++++++
	//
	//		Methods for TimeUtilities
	//
	//+++++++++++++++++++++++++++++++++++++++++++++++++++
	
	//===============================================
	std::string timenow() {
		std::stringstream msg ;
		
		auto now_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		struct tm buffer ;
#if PLATFORM == WINDOWS
		localtime_s(&buffer,&now_time);
		auto now_tm = &buffer ;
#else
		auto now_tm = localtime_r(&now_time,&buffer);
#endif
		msg << std::put_time(now_tm,"%c") ;
		return msg.str();
	}
	
	
	//====================================================
	Interval::Interval() {
		start_time = std::chrono::steady_clock::now();
	}
	//====================================================
	void Interval::start() {
		start_time = std::chrono::steady_clock::now() ;
	}
	//====================================================
	long long Interval::elapsed() {
		auto end = std::chrono::steady_clock::now();
		auto diff = end - start_time ;
		return std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
		
	}
	//====================================================
	long long Interval::stop() {
		auto end = std::chrono::steady_clock::now();
		auto diff = end - start_time ;
		start_time = std::chrono::steady_clock::now() ;
		return std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
		
	}
}
