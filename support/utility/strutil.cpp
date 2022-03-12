//Copyright © 2022 Charles Kerr. All rights reserved.

#include "strutil.hpp"

#include <iostream>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <ctime>

using namespace std::string_literals;
//=========================================================
namespace strutil {
	//=========================================================
	// Define the constant "_whitespace"
	// Whitespace is space,tab,vertical tab,feed,newline,carriage return
	//=========================================================
	static const auto _whitespace = " \t\v\f\n\r"s;

	//=========================================================
	// Trim utilities
	//=========================================================
	// Trim all whitespace from the left of the string
	auto ltrim(const std::string &value) ->std::string {
		if (!value.empty()){
			auto loc = value.find_first_not_of(_whitespace) ;
			if (loc == std::string::npos){
				return std::string() ;
			}
			return value.substr(loc);
		}
		return value ;
	}
	//=========================================================
	// Trim all whitespace from the right of the string
	auto rtrim(const std::string &value) ->std::string {
		if (!value.empty()){
			auto loc = value.find_last_not_of(_whitespace);
			if (loc == std::string::npos){
				return value ;
			}
			return value.substr(0,loc+1);
		}
		return value ;
	}
	//=========================================================
	// Trim all whitespace from both sides of the string
	auto trim(const std::string &value) ->std::string {
		return rtrim(ltrim(value));
	}
	//=========================================================
	// Trim all whitespace from both sides of the string.
	// In addition, replace all runs of whitespace inside the string
	// with a single space character
	auto simplify(const std::string &value) ->std::string {
		// first get the leading/trailing whitespace off
		auto working = trim(value) ;
		if (!working.empty()){
			auto startloc = working.find_first_of(_whitespace);
			while ((startloc != std::string::npos) && (startloc < working.size())){
				auto endloc = working.find_first_not_of(_whitespace,startloc);
				auto amount = endloc - startloc ;
				working.replace(startloc,amount," "s);
				startloc = working.find_first_of(_whitespace,startloc+1) ;
			}
		}
		return working ;
	}

	//=========================================================
	// Case utilities
	//=========================================================
	//=========================================================
	auto upper(const std::string& value) ->std::string {
		std::string rvalue ;
		std::transform(value.begin(), value.end(), std::back_inserter(rvalue),
				   [](unsigned char c){ return std::toupper(c); } // correct
				   );
		return rvalue;
		
	}
	//========================================================================
	auto lower(const std::string& value) ->std::string{
		std::string rvalue ;
		std::transform(value.begin(), value.end(), std::back_inserter(rvalue),
				   [](unsigned char c){ return std::tolower(c); } // correct
				   );
		return rvalue;
		
	}

	//=========================================================
	// String manipulation (remove remaining based on separator,
	// split, parse)
	//=========================================================

	//=========================================================
	auto strip(const std::string &value , const std::string &sep,bool pack ) ->std::string{
		auto rvalue = value ;
		auto loc = rvalue.find(sep) ;
		if (loc != std::string::npos){
			rvalue = rvalue.substr(0,loc) ;
		}
		if (pack) {
			rvalue = rtrim(rvalue) ;
		}
		return rvalue ;
	}

	//=========================================================
	auto split(const std::string &value , const std::string &sep) ->std::pair<std::string,std::string> {
		auto first = value ;
		auto second = ""s ;
		auto loc = value.find(sep) ;
		if (loc != std::string::npos){
			first = trim(value.substr(0,loc));
			loc = loc + sep.size() ;
			if (loc < value.size()){
				second = trim(value.substr(loc));
			}
		}
		return std::make_pair(first, second);
	}

	//=========================================================
	auto parse(const std::string& value, const std::string& sep ) ->std::vector<std::string> {
		std::vector<std::string> rvalue ;
		auto current = std::string::size_type(0) ;
		auto loc = value.find(sep,current) ;
		while (loc != std::string::npos){
			rvalue.push_back(trim(value.substr(current,loc-current)));
			current = loc+sep.size() ;
			loc = value.find(sep,current);
		}
		if (current < value.size()){
			rvalue.push_back(trim(value.substr(current)));
		}
		return rvalue ;
	}

	//=========================================================
	// Time/String conversions
	//=========================================================

	//=========================================================
	// The format returned is:  Thu Dec 30 14:13:28 2021
	auto sysTimeToString(const std::chrono::system_clock::time_point &t) ->std::string
	{
		auto time = std::chrono::system_clock::to_time_t(t);
#if defined( _MSC_VER )
#pragma warning(push)
#pragma warning(disable : 4996)
#endif
		auto time_str = std::string(std::ctime(&time));
#if defined( _MSC_VER )
#pragma warning(pop)
#endif
		time_str.resize(time_str.size() - 1);
		return time_str;
	}

	//=========================================================
	// Format expected by the default format is: Thu Dec 30 14:13:28 2021
	auto stringToSysTime(const std::string &str,const std::string &format) ->std::chrono::system_clock::time_point {
		std::stringstream timbuf(str);
		tm converted;
		
		timbuf >> std::get_time(&converted, format.c_str());
		converted.tm_isdst = -1 ;
		auto ntime = mktime(&converted);
		return std::chrono::system_clock::from_time_t(ntime) ;
	}

	//==========================================================
	// Number/string conversions
	//==========================================================
	//==========================================================
	// Convert a bool to a string
	// the true_value/false_value are returned based on the bool
	auto ntos(bool value,const std::string &true_value ,const std::string &false_value )->std::string{
		return (value?true_value:false_value) ;
	}

	//===========================================================
	// Formatted dump of a byte buffer
	//===========================================================
	
	//===========================================================
	auto dump(std::ostream &output,const std::uint8_t *buffer, std::size_t length,radix_t radix,int entries_line)->void{
		// number of characters for entry
		auto entry_size = 3 ;  // decimal and octal
		switch (static_cast<int>(radix)) {
			case static_cast<int>(radix_t::hex):
				entry_size=2 ;
				break;
			case static_cast<int>(radix_t::bin):
				entry_size=8 ;
				break;
			default:
				break;
		}
		auto num_rows = (length/entries_line) + (((length%entries_line)==0)?0:1) ;
		// what is the largest number for the address ?
		auto max_address_chars = static_cast<int>((ntos(num_rows * entries_line)).size()) ;
		
		// first write out the header
		output << std::setw(max_address_chars+2)<<""<<std::setw(1);
		for (auto i=0; i < entries_line;++i){
			output<<ntos(i,radix_t::dec,false,entry_size,' ')<<" ";
		}
		output <<"\n";
		
		// now we write out the values for each line
		std::string text(entries_line,' ');
		
		for (std::size_t i=0; i< length ; ++i) {
			auto row = i/entries_line ;
			if (((i%static_cast<size_t>(entries_line) == 0) && (i>= static_cast<size_t>(entries_line))) || (i==0)){
				// This is a new line!
				output <<ntos(row*entries_line,radix_t::dec,false,max_address_chars,' ')<<": ";
				text = std::string(entries_line,' ');
			}
			output <<ntos(buffer[i],radix,false,entry_size)<<" " ;
			// If it is an alpha, we want to write it
			if (std::isalpha(static_cast<int>(buffer[i]))!=0){
				// we want to write this to the string
				text[(i%entries_line)]=buffer[i];
			}
			else {
				text[(i%entries_line)]='.';

			}
			if (i%entries_line == entries_line-1){
				output <<" "<<text<<"\n" ;
			}
		}
		// what if we had a partial last line, we need to figure that out
		auto last_line_entry = length%entries_line ;
		if (last_line_entry != 0){
			// we need to put the number of leading spaces
			output <<std::setw(static_cast<int>((entries_line -last_line_entry)*(entry_size+1)))<<""<<std::setw(1)<<" "<<text<<"\n";
		}
	}

}
