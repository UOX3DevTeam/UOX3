//Copyright © 2022 Charles Kerr. All rights reserved.

#include "header.hpp"

#include <iostream>
#include "strutil.hpp"
using namespace std::string_literals;
//==============================================================================
// Static member to determine if the line is a valid header format
//==============================================================================
//==============================================================================
bool header_t::validHeader(const std::string &header) {
	auto valid_header = false ;
	// See if we have our header brackets
	auto sbracket = header.find("[");
	if (sbracket != std::string::npos){
		// Check to see if we have another "[", which would be invalid
		if (header.find("[",sbracket+1) == std::string::npos){
			auto ebracket = header.find("]");
			if (ebracket != std::string::npos){
				// Is the closing after the starting
				if (sbracket < ebracket){
					// Looking good, just need to ensure there isn't another "]"
					if (header.find("]",ebracket+1)==std::string::npos) {
						// all right, if we didn't allow empty sections
						// we would check here, but we do!
						valid_header = true ;
				
					}
				}
			}

		}
	}
	return valid_header;
}
//==============================================================================
// parse a line into a headers three pats, identifer, type, parent
// format is [identifier:type:parent]
//==============================================================================

//==============================================================================
auto header_t::parse(const std::string& value) const ->std::tuple<std::string,std::string,std::string>{
	auto identifier = std::string() ;
	auto type = std::string() ;
	auto parent = std::string() ;
	//we assume the value has been trimmed!
	auto start_bracket = value.find_last_of("[") ;
	if (start_bracket != std::string::npos){
		auto end_bracket = value.find_first_of("]",start_bracket);
		if (end_bracket!= std::string::npos){
			// we now have the bracket area
			auto inner = strutil::trim(value.substr(start_bracket+1,end_bracket-start_bracket-1)) ;
			if (!inner.empty()){
				// lets see if we can find the ":" separators
				auto [pident,rest_line] = strutil::split(inner,":");
				identifier = pident ;
				if (!rest_line.empty()){
					auto [ptype,pparent] = strutil::split(rest_line,":") ;
					type = strutil::lower(ptype) ;
					parent = pparent ;
				}
			}
		}
	}
	return std::make_tuple(identifier,type,parent);
}

//==============================================================================
header_t::header_t(const std::string &line_value,const std::string filename ,int line_number):_line_number(line_number),_filename(filename),_original(line_value){
	auto  [tidentifer,ttype,tparent] = this->parse(line_value);
	_identifer = tidentifer;
	_type = ttype;
	_parent = tparent;
}

//==============================================================================
// The pieces of a header
//==============================================================================

//==============================================================================
auto header_t::type() const ->const std::string& {
	return _type ;
}
//==============================================================================
auto header_t::identifier() const ->const std::string& {
	return _identifer;
}
//==============================================================================
auto header_t::parent() const ->const std::string& {
	return _parent;
}

//==============================================================================
// Debug information
//==============================================================================
//==============================================================================
auto header_t::original() const ->std::string_view {
	return _original;
}
//==============================================================================
auto header_t::filename() const ->std::string_view {
	return _filename;
}
//==============================================================================
auto header_t::filename(const std::string filename)->void {
	_filename=filename;
}

//==============================================================================
auto header_t::lineNumber() const ->int {
	return _line_number;
}

//==============================================================================
auto header_t::lineNumber(int linenum) ->void {
	_line_number = linenum;
}

//==============================================================================
// Summary info
//==============================================================================
//==============================================================================
auto header_t::description() const ->std::string{
	return strutil::format("[%s:%s:%s]",_identifer.c_str(),_type.c_str(),_parent.c_str());
}
//==============================================================================
auto header_t::source() const  ->std::string {
	return strutil::format("Line: %d File: %s",_line_number,_filename.c_str());
}
