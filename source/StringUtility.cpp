//
//  StringUtility.cpp
//  Infinity
//
//

#include "StringUtility.hpp"
#include <cctype>
#include <locale>
#include <algorithm>
#include <cstdio>
#include <cstring>

#include <cstdio>
#include <stdexcept>
#include <filesystem>

using namespace std::string_literals;

namespace strutil {
	
	//=====================================================================
	// Lowercase the string
	std::string lower(const std::string &value){
		auto rvalue  = value ;
		std::transform(rvalue.begin(), rvalue.end(),rvalue.begin(),
				   [](unsigned char c){ return std::tolower(c);});
		return rvalue;
	}
	
	//=====================================================================
	// Uppercase the string
	std::string upper(const std::string &value){
		auto rvalue  = value ;
		std::transform(rvalue.begin(), rvalue.end(),rvalue.begin(),
				   [](unsigned char c){ return std::toupper(c);});
		return rvalue;
		
	}
	

	
	//=====================================================================
	// Remove leading whitespace
	std::string ltrim(const std::string &value){
		auto count = 0 ;
		for (auto it = value.begin(); it != value.end();it++) {
			if (!isspace(*it)){
				return value.substr(count) ;
			}
			else {
				count++;
			}
		}
		
		return std::string();
	}
	//=====================================================================
	// Remove trailing whitespace
	std::string rtrim(const std::string &value){
		auto count = 0 ;
		for (auto it = value.rbegin(); it != value.rend();it++) {
			if (!isspace(*it)){
				return value.substr(0,value.size()-count);
				
			}
			else {
				count++;
			}
		}
		
		return std::string();
		
	}
	//=====================================================================
	// Remove leading/trailing whitespace
	std::string trim(const std::string &value) {
		return rtrim(ltrim(value));
	}
	//=====================================================================
	// Remove leading/trailing whitepace, and reduce all other whitespace to one " "
	std::string simplify(const std::string &value){
		auto tvalue = trim(value) ;
		auto rvalue = ""s ;
		auto toggle = false ;
		for (auto &c :tvalue){
			if (isspace(c)) {
				if (!toggle){
					rvalue = rvalue + c ;
					toggle = true ;
				}
				
				
			}
			else {
				toggle = false ;
				rvalue = rvalue + c ;
				
			}
		}
		return rvalue ;
	}

	//=====================================================================
	// Removing everything after a delimiter. Normally "//" for comments
	std::string removeTrailing(const std::string& value,const std::string& delim){
		auto loc = value.find(delim) ;
		if (loc == std::string::npos){
			return value ;
		}
		return value.substr(0,loc) ;
	}


	//====================================================================
	std::string& stripTrim(std::string& s)  {
		auto loc = s.find("//") ;
		if (loc != std::string::npos) {
			s = s.substr(0,loc) ;
		}
		loc = s.find_first_not_of(" \t\v") ;
		if (loc != std::string::npos) {
			auto eloc = s.find_last_not_of(" \t\v") ;
			s = s.substr(loc,(eloc-loc)+1);
		}
		return s ;
	}
	
	//++++++++++++++++++++++++++++++++++++++
	std::tuple<std::string,std::string> separate(const std::string& input,
								   const std::string& separator){
		auto loc = input.find(separator);
		auto first = input.substr(0,loc) ;
		std::string second ="";
		if ((loc != std::string::npos) && ((loc+1)< input.size()) ) {
			second = input.substr(loc+1) ;
		}
		return std::make_tuple(trim(first),trim(second));
		
	}
	//+++++++++++++++++++++++++++++++++++++
	std::string format(std::size_t maxsize, const std::string fmtstring,...)  {
		
		std::va_list argptr ;
		va_start(argptr, fmtstring);
		auto test = strutil::format(fmtstring,argptr) ;
		if (maxsize>0){
			if (test.size() > maxsize) {
				test = test.substr(0,maxsize);
			}
		}
		return test ;
	}
	//++++++++++++++++++++++++++++++++++++++
	std::string format(std::size_t maxsize,const std::string fmtstring,va_list& list){
		auto temp = strutil::format(fmtstring,list);
		if (maxsize>0){
			if (temp.size() > maxsize) {
				temp = temp.substr(0,maxsize);
			}
		}
		return temp ;
	}
	//+++++++++++++++++++++++++++++++++++++
	std::string format(const std::string fmtstring,...)  {
		
		std::va_list argptr ;
		va_start(argptr, fmtstring);
		return strutil::format(fmtstring,argptr) ;
	}
	//++++++++++++++++++++++++++++++++++++++
	std::string format(const std::string fmtstring,va_list& list){
		constexpr std::uintmax_t buffersize = 4096 ;
		char msg[buffersize];
		std::memset(msg, 0,buffersize);
		std::vsnprintf(msg, buffersize, fmtstring.c_str(), list);
		va_end(list);
		return std::string(msg) ;
	}
	//++++++++++++++++++++++++++++++++++++++++
	std::string formatMessage(const std::string& uformat,const std::string& data){
		
		auto values = strutil::sections(data,",");
		auto format = uformat ;
		auto pos = format.find_first_of("%") ;
		if (pos == std::string::npos) {
			return format ;
		}
		auto index = 0 ;
		while ( pos != std::string::npos) {
			if (index >= values.size() ){
				break;
			}
			auto temp = values[index];
			index++;
			// now, we assume the formatting was done before the string, so basically we
			// replace the format specifier, with the string
			format.replace(pos, 2, temp);
			pos = format.find_first_of("%") ;
		}
		return format ;
	}
	
	//++++++++++++++++++++++++++++++++++++++++++
	std::vector<std::string> sections(const std::string& value, const std::string& sep, std::string::size_type start, std::string::size_type end ) {
		std::vector<std::string> rvalue ;
		
		auto pos = value.find(sep,start) ;
		while (pos < end) {
			if ((pos - start) > 0) {
				auto potential = value.substr(start,pos - start) ;
				rvalue.push_back(potential);
				start = pos+sep.size() ;
			}
			else if (pos == start) {
				rvalue.push_back("");
				start += sep.size() ;
			}
			pos = value.find(sep,start) ;
		}
		rvalue.push_back(value.substr(start,pos - start)) ;
		return rvalue ;
	}
	
	//++++++++++++++++++++++++++++++++++++++++++
	std::uintmax_t sectionCount(const std::string& value, const std::string& sep, std::string::size_type start , std::string::size_type end ){
		
		return strutil::sections(value,sep,start,end).size() ;
	}
	
	//++++++++++++++++++++++++++++++++++++++++++
	std::string indexSection(const std::string& value, std::uintmax_t sectionindex,const std::string& sep, std::string::size_type start , std::string::size_type end ){
		auto sec = strutil::sections(value,sep,start,end) ;
		if (sec.size() <= sectionindex) {
			return sec[sectionindex];
		}
		throw std::runtime_error(strutil::format("Section index %ul exceeded size %ul from sections %s",sectionindex,sec.size(),value.c_str()));
	}
	
	//+++++++++++++++++++++++++++++++++++++++++
	std::string extractSection( const std::string& value, const std::string& sep, std::string::size_type start, std::string::size_type stop )
	{
		std::string data ;
		int count = -1 ;
		size_t startoffset = 0 ;
		size_t stopoffset = 0 ;
		bool match = false ;
		stopoffset = value.find(sep,startoffset);
		while ( startoffset != std::string::npos )
		{
			count++ ;
			if( count == start )
			{
				match = true ;
				// We dont return the seperator, so jump over it
				size_t tempoffset = startoffset ;
				while ( stopoffset != std::string::npos )
				{
					if( count == stop )
					{
						break ;
					}
					else
					{
						tempoffset = stopoffset + sep.length() ;
						stopoffset = value.find(sep,tempoffset) ;
						count++ ;
					}
				}
				break ;
				
			}
			else
			{
				if( stopoffset != std::string::npos )
					startoffset = stopoffset+ sep.length();
				else
					startoffset	= static_cast<size_t>(std::string::npos) ;
				stopoffset = value.find(sep,startoffset) ;
				
			}
		}
		if( match )
		{
			size_t length = stopoffset - startoffset ;
			if( stopoffset == std::string::npos ){
				length = value.length() - startoffset ;
			}
			data = value.substr(startoffset,length) ;
		}
		return data ;
	}
	
	
	
	//++++++++++++++++++++++++++++++++++++++++++
	std::vector<std::string> breakSize(std::size_t maxsize, const std::string& input){
		auto working = input ;
		std::vector<std::string> total ;
		while (working.size() > maxsize) {
			auto portion = working.substr(0,maxsize);
			working.replace(0, maxsize, "");
			total.push_back(portion);
			
		}
		if (!working.empty()){
			total.push_back(working);
		}
		return total ;
	}
	
	//++++++++++++++++++++++++++++++++++++++++++
	std::string fixDirectory(const std::string& base){
		std::string value = "\\" ;
		std::string::size_type index = 0 ;
		auto data = trim(base) ;
		while ( (index = data.find( value, index ) ) != std::string::npos )
		{
			data = data.replace(index,1,"/") ;
		}
		if( data.find_last_of("/") != data.size()-1 )
		{
			data = data+"/" ;
		}
		
		return data ;
	}
	
	//+++++++++++++++++++++++++++++++++++++++++++
	std::string replaceSlash( std::string& data )
	{
		std::string value = "\\" ;
		std::string::size_type index = 0 ;
		
		while ( (index = data.find( value, index ) ) != std::string::npos )
		{
			data = data.replace(index,1,"/") ;
		}
		return data ;
	}
	
	std::string number(char n, int base){
		std::stringstream conversion ;
		auto value = static_cast<std::int16_t>(n);
		
		switch (base) {
			case 10:
				conversion << std::dec << value ;
				break;
			case 16:
				conversion <<std::hex << value <<std::dec;
				break;
			case 8:
				conversion << std::oct << value <<std::dec;
				break;
			default:
				conversion << std::dec << value ;
				break;
		}
		
		return conversion.str()  ;
	}
	std::string number(unsigned char n, int base){
		std::stringstream conversion ;
		auto value = static_cast<std::uint16_t>(n);
		
		switch (base) {
			case 10:
				conversion << std::dec << value ;
				break;
			case 16:
				conversion <<std::hex << value <<std::dec;
				break;
			case 8:
				conversion << std::oct << value <<std::dec;
				break;
			default:
				conversion << std::dec << value ;
				break;
		}
		
		return conversion.str()  ;
	}
	
	//====================================================================
	std::tuple<std::string,std::string> split(const std::string &value, const std::string &sep){
		std::string first ;
		std::string second ;
		auto loc = value.find(sep);
		if (loc == std::string::npos){
			first = value ;
		}
		else {
			first = strutil::rtrim(value.substr(0,loc));
			if ((loc+1) < value.size()) {
				second = strutil::ltrim(value.substr(loc+1));
			}
		}
		return std::make_tuple(first,second);
	}
	
}
