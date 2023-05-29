//
//  StringUtility.hpp
//  Infinity
//
//

#ifndef StringUtility_hpp
#define StringUtility_hpp

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <tuple>
#include <cstdarg>
#include <type_traits>

namespace oldstrutil
{

	// Return the values between two delimitors.
	std::string contents( const std::string &value, const std::string& startdelim, const std::string &enddelim, std::string::size_type location = 0 );

	
	std::string format( std::size_t maxsize, const std::string fmtstring, ... );
	std::string format( std::size_t maxsize, const std::string fmtstring, va_list &list );
	
	std::string format( const std::string fmtstring, va_list &list );
	std::string formatMessage( const std::string& uformat, const std::string& data );

	
	
	
	//+++++++++++++++++++++++++++++++++++++++++++++++
	//
	// This mainly is on gumps, where the string needs to be broken up
	// based on the max size
	
	std::vector<std::string> breakSize( std::size_t maxsize, const std::string& input );
	
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Ensures path ends with a / and converts \ to /
	std::string fixDirectory( const std::string& base );
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Replace the \ with a /
	std::string replaceSlash( std::string& value );
	
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Extract a section (area between N "separators" from a string
	std::string extractSection( const std::string& value, const std::string& sep, std::string::size_type start, std::string::size_type stop = std::string::npos );
	
	// Convert a string to wstring (for UTF8 characters)
	std::wstring stringToWstring( const std::string& t_str );

	// Convert a wstring to string (retaining UTF8 characters)
	std::string wStringToString( const std::wstring& t_str );

	// Convert a string to wstring and back to string (retaining UTF8 characters)
	std::string stringToWstringToString( const std::string& t_str );
	
	// Tackles converting text loaded from dictionary into japanese
	std::wstring FromUTF8( const char* str );
	
	

}
#endif /* StringUtility_hpp */
