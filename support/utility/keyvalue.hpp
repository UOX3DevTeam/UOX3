//Copyright © 2022 Charles Kerr. All rights reserved.

#ifndef keyvalue_hpp
#define keyvalue_hpp
#include "framework.h"
#include <cstdint>
#include <string>
//=========================================================
class keyvalue_t {
private:
	int _line_number ;
	std::string _filename ;
	
	std::string _line_value ;
	std::string _key ;
	std::string _value ;
public:
	keyvalue_t(const std::string &string_line="",int line_number=-1, const std::string &filename="");
	auto line() const ->int ;
	auto line(int line_number) ->void ;
	auto filename() const ->std::string_view ;
	auto filename(const std::string &file_name)->void;
	
	auto source() const ->std::string ;
	auto description() const ->std::string ;
	
	auto key() const ->const std::string& ;
	auto value() const ->const std::string& ;
	auto key(const std::string &key);
	auto value(const std::string &value);
};
#endif /* keyvalue_hpp */
