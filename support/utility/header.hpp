//Copyright © 2022 Charles Kerr. All rights reserved.

#ifndef header_hpp
#define header_hpp
#include "framework.h"
#include <cstdint>
#include <string>
#include <tuple>
//=========================================================
class header_t {
private:
	std::string _original ;
	std::string _filename ;
	int		_line_number ;
	
	std::string _type ;
	std::string _identifer ;
	std::string _parent ;
	
	auto parse(const std::string& value) const ->std::tuple<std::string,std::string,std::string>;
public:
	// An easy way to check if a valid header
	static bool validHeader(const std::string &header) ;
	
	// Pieces of a header
	auto type() const ->const std::string& ;
	auto identifier() const ->const std::string& ;
	auto parent() const ->const std::string& ;
	
	// Debug information
	auto original() const ->std::string_view ;
	auto filename() const ->std::string_view ;
	auto filename(const std::string filename)->void ;
	auto lineNumber() const ->int ;
	auto lineNumber(int linenum) ->void ;
	// Summary info
	auto description() const ->std::string;
	auto source() const  ->std::string ;
	
	header_t(const std::string &line_value="",const std::string filename ="",int line_number=-1);
};
#endif /* header_hpp */
