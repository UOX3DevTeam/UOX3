#ifndef __OSUNIQUE_HPP
#define __OSUNIQUE_HPP
#include <time.h>

#ifdef _WIN32

auto asciitime = [](char* buffer, size_t count, const struct tm &tmsource)->char* {
	asctime_s(buffer, count, &tmsource);
	return buffer;
};
// Beware, MSVC localtime_s is not the same as the standard localtime_s
auto lcltime = [](const time_t& source, struct tm& dest)->struct tm* {
	localtime_s(&dest, &source);
	return &dest;
};
#else 
auto asciitime = [](char* buffer, size_t count, const struct tm &tmsource)->char* {
	return asctime_r(  &tmsource,buffer);
	
};
auto lcltime = [](const time_t& source, struct tm& dest)->struct tm* {
	localtime_r(&source, &dest);
	return &dest;
};
#endif

#endif
