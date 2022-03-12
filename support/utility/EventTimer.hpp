//

#ifndef EventTimer_hpp
#define EventTimer_hpp
#include <chrono>
#include <cstdint>
#include <string>

// A few instrumentation macros
// This estabishs a timer, and state determins if on/off
#define EVENT_TIMER(varname,state) \
constexpr auto TIMER_##varname = state ; \
auto varname = EventTimer();

#if !defined(EVENT_TIMER_ON)
#define EVENT_TIMER_ON 1
#endif
#if !defined(EVENT_TIMER_OFF)
#define EVENT_TIMER_OFF 0
#endif
#if !defined(EVENT_TIMER_CLEAR)
#define EVENT_TIMER_CLEAR 1
#endif
#if !defined(EVENT_TIMER_KEEP)
#define EVENT_TIMER_KEEP 0
#endif

// This prints out the delta time in millisconds for the timer since the last reset.
// The msg variable is the message you want with the time (no quotes around it, just text (so you cant use , in the message)
// reset tells the timer if it should reset the time count, or continue without resetting
#define EVENT_TIMER_NOW(varname,msg,reset) \
if constexpr (TIMER_##varname) {varname.output(#msg,reset);}

// This resets the timer
#define EVENT_TIMER_RESET(varname) if constexpr (TIMER_##varname) {varname.elapsed(true);}


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
