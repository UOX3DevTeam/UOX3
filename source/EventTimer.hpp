//

#ifndef EventTimer_hpp
#define EventTimer_hpp
#include <chrono>
#include <cstdint>
#include <string>
#if defined(TIMER_ON)
#define STRINGIFY(variable) #variable
#define EVENT_TIMER_ON(varname) auto varname = EventTimer()
#define EVENT_TIMER_NOW(varname,msg) varname.output(STRINGIFY(msg))
#else
#define EVENT_TIMER_ON(varname)
#define EVENT_TIMER_NOW(varname,msg)
#endif
/*
#define EVENT_TIMER_OFF ( #varnam, #msg) \
#if defined(TIMER_ON) \
varname.output("msg") ; \
#endif

*/


//=========================================================
class EventTimer {
private:
	std::chrono::high_resolution_clock::time_point _now ;
public:
	EventTimer() ;
	[[maybe_unused]] auto elapsed() -> long long ;
	auto output(const std::string &label)->void ;
};
#endif /* EventTimer_hpp */
