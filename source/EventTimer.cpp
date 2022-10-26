#include "EventTimer.hpp"

#include <iostream>
using namespace std::string_literals;

//o------------------------------------------------------------------------------------------------o
//|	Function	-	EventTimer::EventTimer()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Return current time point in frame of the high resolution clock
//o------------------------------------------------------------------------------------------------o
EventTimer::EventTimer()
{
	_now = std::chrono::high_resolution_clock::now();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	EventTimer::Elapsed()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Return elapsed time since EventTimer was started
//o------------------------------------------------------------------------------------------------o
auto EventTimer::Elapsed( bool reset ) -> long long
{
	auto delta = std::chrono::high_resolution_clock::now() - _now;
	if( reset )
	{
		_now = std::chrono::high_resolution_clock::now();
	}
	return std::chrono::duration_cast<std::chrono::milliseconds>( delta ).count();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	EventTimer::output()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Output the elapsed time to console
//o------------------------------------------------------------------------------------------------o
auto EventTimer::Output( const std::string &label = "Delta from last call", bool reset ) -> void
{
	std::cout << label << " - "s << Elapsed( reset ) << "(ms)\n"s;
}
