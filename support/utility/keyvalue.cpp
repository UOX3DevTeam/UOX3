//

#include "keyvalue.hpp"

#include <iostream>
#include "strutil.hpp"
using namespace std::string_literals ;
//=========================================================
keyvalue_t::keyvalue_t::keyvalue_t(const std::string &string_line,int line_number, const std::string &filename):_line_number(line_number),_filename(filename){

	// a few things we will do
	_line_value = strutil::trim(string_line) ;
	if (!_line_value.empty()){
		auto [key,value] = strutil::split(_line_value,"=") ;
		_key = strutil::lower(key) ;
		_value=value ;
	}
}
//=========================================================
auto keyvalue_t::line() const ->int {
	return _line_number ;
}
//=========================================================
auto keyvalue_t::line(int line_number) ->void {
	_line_number = line_number;
}
//=========================================================
auto keyvalue_t::filename() const ->std::string_view {
	return _filename ;
}
//=========================================================
auto keyvalue_t::filename(const std::string &file_name)->void{
	_filename = file_name;
}

//=========================================================
auto keyvalue_t::source() const ->std::string {
	return "Line: "s+std::to_string(_line_number)+" : "s+_filename ;
}
//=========================================================
auto keyvalue_t::description() const ->std::string {
	return "\t"s+_key+" = "s + _value ;
}

//=========================================================
auto keyvalue_t::key() const ->const std::string&{
	return _key ;
}
//=========================================================
auto keyvalue_t::value() const ->const std::string& {
	return _value ;
}
//=========================================================
auto keyvalue_t::key(const std::string &key){
	_key = key ;
}
//=========================================================
auto keyvalue_t::value(const std::string &value){
	_value = value ;
}

