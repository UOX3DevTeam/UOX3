//Copyright © 2022 Charles Kerr. All rights reserved.

#ifndef strutil_hpp
#define strutil_hpp
#include <string>
#include <utility>
#include <algorithm>

#include <vector>
#include <array>
#include <chrono>
#include <memory>
#include <charconv>
#include <type_traits>
#include <system_error>
#include <ostream>

//=========================================================
namespace strutil {
	//=========================================================
	// Trim utilities
	//=========================================================
	
	//=========================================================
	// Trim all whitespace from the left of the string
	auto ltrim(const std::string &value) ->std::string ;
	
	//=========================================================
	// Trim all whitespace from the right of the string
	auto rtrim(const std::string &value) ->std::string ;
	
	//=========================================================
	// Trim all whitespace from both sides of the string
	auto trim(const std::string &value) ->std::string ;
	
	//=========================================================
	// Trim all whitespace from both sides of the string.
	// In addition, replace all runs of whitespace inside the string
	// with a single space character
	auto simplify(const std::string &value) ->std::string ;
	
	//=========================================================
	// Case utilities
	//=========================================================
	
	//=========================================================
	// Upper case the string
	auto upper(const std::string &value) ->std::string ;
	
	//========================================================================
	// Lower case the string
	auto lower(const std::string &value) ->std::string ;
	
	//=========================================================
	// String manipulation (remove remaining based on separator,
	// split, parse)
	//=========================================================
	
	//=========================================================
	// Remove everthing from the string following the separator provided
	// If pack is true, it will also remove rtrim the string after removal
	auto strip(const std::string &value , const std::string &sep="//",bool pack = true) ->std::string ;
	
	//=========================================================
	// Split a string into two strings based on a separator (separtor is not included)
	// The remaining two values are trimmed of whitespace
	auto split(const std::string &value , const std::string &sep=",") ->std::pair<std::string,std::string> ;
	
	//=========================================================
	// Break a string into components based on a separtor
	// Each component is trimmed of whitespace
	auto parse(const std::string& value, const std::string& sep = ",") ->std::vector<std::string> ;
	
	//=========================================================
	// Time/String conversions
	//=========================================================
	
	//=========================================================
	auto sysTimeToString(const std::chrono::system_clock::time_point &t) ->std::string;
	
	//=========================================================
	// Format expected by the default format is: Thu Dec 30 14:13:28 2021
	auto stringToSysTime(const std::string &str, const std::string &format = "%a %b %d %H:%M:%S %Y") ->std::chrono::system_clock::time_point;
	
	//==========================================================
	// String formatting (like printf)
	//==========================================================
	
	//==========================================================
	// The source for this was found on StackOverflow at:
	// https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
	//
	template<typename ... Args>
	std::string format( const std::string& format_str, Args ... args ) 
	{
		int size_s = std::snprintf( nullptr, 0, format_str.c_str(), args ... ) + 1; // Extra space for '\0'
		if (size_s > 0){
			auto size = static_cast<size_t>( size_s );
			auto buf = std::make_unique<char[]>( size );
			std::snprintf( buf.get(), size, format_str.c_str(), args ... );
			return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
			
		}
		return format_str ; // We take the same approach as std library, and just fail silently with
		// best answer
	}
	
	//==========================================================
	// Number/string conversions
	//==========================================================
	
	//==========================================================
	// Radix that are supported
	enum class radix_t {dec=10,oct=8,hex=16,bin=2};
	
	//==========================================================
	// The maximum characters in a string number for conversion sake
	inline constexpr auto max_characters_in_number = 12 ;
	
	//==========================================================
	// Convert a bool to a string
	// the true_value/false_value are returned based on the bool
	auto ntos(bool value,const std::string &true_value = "true",const std::string &false_value = "false")->std::string;
	
	//==========================================================
	// Convert a number to a string, with options on radix,prefix,size,pad
	// Radix indicates the radix the string will represent
	// prefix indicates if the "radix" indicator will be present at the front of the string
	// size is the minimum size the string must be (if the number is not large enough
	// it will use the "pad" character prepended to the number
	template <typename T>
	auto ntos(T value,radix_t radix=radix_t::dec,bool prefix=false,int size=0,char pad='0')->std::string {
		if constexpr( std::is_integral_v<T> && !std::is_same_v<T, bool>){
			// first, thing we need to convert the value to a string
			std::array<char,max_characters_in_number> str ;
			
			if (auto [pc,ec] = std::to_chars(str.data(), str.data()+str.size(), value,static_cast<int>(radix)); ec==std::errc()) {
				// How many characters did this number take
				auto numchars = static_cast<int>(std::distance(str.data(),pc)) ;
				// what is larger, that is the size of our string
				auto sizeofstring = std::max(numchars,size) ;
				// where do we start adding the number into our string ?
				auto index = sizeofstring - numchars ;
				if (prefix) {
					// We have a prefix, so we add two characters to the beginning
					sizeofstring += 2;
					index +=2 ;
				}
				auto rvalue = std::string(sizeofstring,pad);
				// copy the value into the string
				std::copy(str.data(),pc,rvalue.begin()+index);
				// do we need our prefix?
				if (prefix){
					switch (static_cast<int>(radix)) {
						case static_cast<int>(radix_t::dec):
							// We dont add anything for decimal!
							break;
						case static_cast<int>(radix_t::hex):
							rvalue[0]='0';
							rvalue[1]='x';
							break;
						case static_cast<int>(radix_t::oct):
							rvalue[0]='0';
							rvalue[1]='o';
							break;
						case static_cast<int>(radix_t::bin):
							rvalue[0]='0';
							rvalue[1]='b';
							break;
							
						default:
							break;
					}
				}
				return rvalue ;
				
			}
			else {
				// The conversion was not successful, so we return an empty string
				return std::string();
				
			}
			
		}
	}
	
	//==========================================================
	// Convert a string to a number
	template<typename T>
	typename std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T,bool>,T>
	ston(const std::string &str_value,radix_t radix=radix_t::dec) {
			auto value = T{0};
			if (!str_value.empty()){
				if (str_value.size() < 2){
					std::from_chars(str_value.data(), str_value.data()+str_value.size(), value,static_cast<int>(radix));
					
				}
				else if (std::isalpha(static_cast<int>(static_cast<int>(str_value[1])))){
					// This has a "radix indicator"
					switch(str_value[1]){
						case 'b':
						case 'B':
							std::from_chars(str_value.data()+2, str_value.data()+str_value.size(), value,static_cast<int>(radix_t::bin));
							break;
						case 'x':
						case 'X':
							std::from_chars(str_value.data()+2, str_value.data()+str_value.size(), value,static_cast<int>(radix_t::hex));
							break;
						case 'o':
						case 'O':
							std::from_chars(str_value.data()+2, str_value.data()+str_value.size(), value,static_cast<int>(radix_t::oct));
							break;
						default:
							// we dont do anything, we dont undertand so let value be 0
							break;
					}
					
				}
				else {
					std::from_chars(str_value.data(), str_value.data()+str_value.size(), value,static_cast<int>(radix));
					return value ;
					
				}
			}
			return value ;
			
	}
	
	//==========================================================
	// Convert a string to a bool
	template<typename T>
	typename std::enable_if_t<std::is_integral_v<T> && std::is_same_v<T,bool>,T>
	ston(const std::string &str_value,const std::string &true_value="true") {
		//If string empty, we return false
		// we take advantege, that if in ston() we set value to 0 false, and if
		// the from_chars fails, it doesn't touch value
		auto numvalue = ston<int>(str_value) ;
		if ((str_value == true_value) || (numvalue != 0)){
			return true ;
		}
		return false ;
	}
	
	//===========================================================
	// Formatted dump of a byte buffer
	//===========================================================
	
	//===========================================================
	// Dumps a byte buffer, formatted to a provided stream.
	// The entries_line indicate how many bytes to display per line.
	auto dump(std::ostream &output,const std::uint8_t *buffer, std::size_t length,radix_t radix=radix_t::hex,int entries_line = 8)->void;
}
#endif /* strutil_hpp */
