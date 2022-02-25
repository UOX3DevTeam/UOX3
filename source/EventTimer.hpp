//

#ifndef EventTimer_hpp
#define EventTimer_hpp
#include <chrono>
#include <cstdint>
#include <string>

#define STRINGIFY(variable) #variable

#define EVENT_TIMER(varname,state) \
auto varname = EventTimer() ; \
constexpr auto TIMER_##varname = state

#define EVENT_TIMER_NOW(varname,msg,reset) \
if constexpr (TIMER_##varname) varname.output(STRINGIFY(msg),reset)

#define EVENT_TIMER_RESET(varname) if constexpr (TIMER_##varname) varname.elapsed(true)

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
	[[maybe_unused]] auto elapsed(bool reset=true) -> long long ;
	auto output(const std::string &label,bool reset=true)->void ;
};
#endif /* EventTimer_hpp */
