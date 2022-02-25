//

#ifndef EventTimer_hpp
#define EventTimer_hpp
#include <chrono>
#include <cstdint>
#include <string>
//=========================================================
class EventTimer {
private:
	std::chrono::high_resolution_clock::time_point _now ;
public:
	EventTimer() ;
	auto elapsed() -> long long ;
	auto output(const std::string &label)->void ;
};
#endif /* EventTimer_hpp */
