//
//  StringUtility.hpp
//  Infinity
//
//

#ifndef StringUtility_hpp
#define StringUtility_hpp

#include <string>
#include <tuple>
#include <cstdarg>

#include <string>
#include <tuple>
#include <vector>
#include <sstream>
#include <type_traits>

std::string ltrim(const std::string& s) ;
std::string rtrim(const std::string& s) ;

std::string trim(const std::string& s) ;

std::string stripComment(const std::string& input,const std::string &commentdelim = "#");

std::string str_toupper(const std::string &s);
std::string str_tolower(const std::string &s);



std::tuple<std::string,std::string> separate(const std::string& input,
											 const std::string& separator);


std::string format(std::size_t maxsize, const std::string fmtstring,...) ;
std::string format(std::size_t maxsize, const std::string fmtstring,va_list &list);

std::string format(const std::string fmtstring,...) ;
std::string format(const std::string fmtstring,va_list &list);

std::string simplify(const std::string& input);

std::vector<std::string> sections(const std::string& value, const std::string& sep=",", std::string::size_type start = 0, std::string::size_type end = std::string::npos) ;

//+++++++++++++++++++++++++++++++++++++++++++++++
// The next few are convience methods on the main method "sections".
//
std::uintmax_t sectionCount(const std::string& value, const std::string& sep=",", std::string::size_type start = 0, std::string::size_type end = std::string::npos);

std::string indexSection(const std::string& value,std::uintmax_t sectionindex=0, const std::string& sep=",", std::string::size_type start = 0, std::string::size_type end = std::string::npos);

//+++++++++++++++++++++++++++++++++++++++++++++++
//
// This mainly is on gumps, where the string needs to be broken up
// based on the max size

std::vector<std::string> breakSize(std::size_t maxsize, const std::string& input);

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Ensures path ends with a / and converts \ to /
std::string fixDirectory(const std::string& base);
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Replace the \ with a /
std::string replaceSlash(std::string& value);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Extract a section (area between N "separators" from a string
std::string extractSection( const std::string& value, const std::string& sep, std::string::size_type start, std::string::size_type stop = std::string::npos ) ;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Convert a number to a string
std::string str_number(char n, int base = 10);
std::string str_number(unsigned char n, int base = 10 );

template<typename T>
std::string str_number(T n, int base=10){
	std::stringstream conversion ;
	if (std::is_integral<T>::value) {

		switch (base) {
			case 10:
				conversion << std::dec << n ;
				break;
			case 16:
				conversion <<std::hex << n <<std::dec;
				break;
			case 8:
				conversion << std::oct << n <<std::dec;
				break;
			default:
				conversion << std::dec << n ;
				break;
		}
	}
	else if (std::is_floating_point<T>::value) {
		conversion << n ;
	}

	return conversion.str()  ;
}

//++++++++++++++++++++++++++++++++++++++++++++++
// Convert a string to a value (integer,number)
//
template<typename T>
T str_value(const std::string& input, int base = 0) {
	if (std::is_floating_point<T>::value){
		double value = 0.0 ;
		try {
			value = std::stod(input) ;
		}
		catch(...) {
			// ok, it couldn't convert it, leave it a 0.0
		}
		return static_cast<T>(value);
	}
	else {
		long long value = 0.0;
		try {
			value = std::stoll(input,0,base) ;
		}
		catch(...){
			// ok, it couldn't convert, keave it a 0
		}
		return static_cast<T>(value);
	}
}
#endif /* StringUtility_hpp */
