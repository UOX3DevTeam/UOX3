//Copyright © 2022 Charles Kerr. All rights reserved.

#include "section.hpp"

#include <iostream>
#include <algorithm>
#include <utility>
#include <stdexcept>
#include <memory>
#include "strutil.hpp"

using namespace std::string_literals;

//==============================================================================
// static/factory method
//==============================================================================

//==============================================================================
auto section_t::load(std::istream &input, int &line_number, const std::string &sourcefile)->std::optional<section_t> {
	constexpr auto bufsize = 4096;
	auto buffer = std::make_unique<char[]>(bufsize);
	enum class search_t { header,startsection,endsection};
	auto comment_on = false ;
	// we now have a buffer of bufsize
	auto search_for = search_t::header ;
	section_t section ;
	auto end_loop = false ;
	do {
		input.getline(buffer.get(),bufsize);
		if (input.gcount()>0 ){
			buffer[input.gcount()] = 0 ;
			auto line = strutil::trim(strutil::strip(std::string(buffer.get()),"//"));
			
			++line_number;
			if (!line.empty()){
				if (!comment_on){
					// Are we turning on comments?
					if (line.find("/*")== 0){
						comment_on = true ;
					}
					else {
						switch(static_cast<int>(search_for)){
							case static_cast<int>(search_t::header):
							{
								// Is this a valid header?
								if (header_t::validHeader(line)) {
									// It is!
									section._header = header_t(line,sourcefile,line_number);
									search_for = search_t::startsection ;
									// Just in case!
									// Is the last entry "{"?
									if (line[line.size()-1]=='{'){
										// The start section was at the end of the header
										search_for = search_t::endsection ;
									}
								}
								break;
							}
							case static_cast<int>(search_t::startsection):
							{
								// just for santity check, lets see if if we have the start of a header or end section
								
								if (line[0]=='[' || line[0]=='}'){
									// Our data set is most likely corrupted, so we will
									// bail, no good way to recover
									throw std::runtime_error(strutil::format("Found header/end section indicator while searching for section start. Line: %i File: %s",line_number,sourcefile.c_str()));
									
									
								}
								if (line[0]=='{'){
									search_for = search_t::endsection;
								}
								break;
							}
							case static_cast<int>(search_t::endsection):
							{
								if (line[0]=='[' || line[0] == '{'){
									// Our data set is most likely corrupted, so we will
									// bail, no good way to recover
									// This section is actually ok, but the next section will be lost if it was a header , as we cant undo that
									throw std::runtime_error(strutil::format("Found header/start section indicator while searching for section end, section lost. Line: %i File: %s",line_number,sourcefile.c_str()));
								}
								else if (line[0]!='}') {
									auto keyvalue = keyvalue_t(line,line_number,sourcefile);
									section._entries.push_back(keyvalue);
								}
								else {
									// We found it!
									end_loop = true ;
									
								}
								break;
							}
						}
					}
				}
				else {
					// Is this turning off comments?
					if (line.find("*/")!= std::string::npos){
						// we did, turn off comment
						comment_on = false ;
					}
				}
			}
		}
	}while (input.good() && !input.eof() && !end_loop);
	if (!end_loop){
		return std::nullopt;
	}
	return std::optional<section_t>(section) ;
}

//==============================================================================
// Constructors
//==============================================================================

//==============================================================================
section_t::section_t(const header_t &header):section_t(){
	_header=header ;
}
//==============================================================================
section_t::section_t() {
	
}

//==============================================================================
// Header retrival
//==============================================================================

//==============================================================================
auto section_t::header() const ->const header_t&{
	return _header;
}
//==============================================================================
auto section_t::header() ->header_t&{
	return _header;
}

//==============================================================================
// Size of the section
//==============================================================================

//==============================================================================
auto section_t::size() const ->size_t {
	return _entries.size();
}

//==============================================================================
// Entry reterival
//==============================================================================

//==============================================================================
auto section_t::entries() const ->const std::vector<keyvalue_t>& {
	return _entries;
}

//==============================================================================
auto section_t::entries()  -> std::vector<keyvalue_t>& {
	return _entries;
}

//==============================================================================
// key based retrevial
//==============================================================================

//==============================================================================
auto section_t::contain(const std::string &key) const ->bool {
	auto iter = std::find_if(_entries.begin(),_entries.end(),[&key =std::as_const(key)](const keyvalue_t &entry){
		return entry.key()==key;
	});
	return iter != _entries.end();
}

//==============================================================================
auto section_t::find(const std::string &key) const ->std::vector<const keyvalue_t*> {
	auto rvalue = std::vector<const keyvalue_t*>() ;
	for (const auto &entry : _entries){
		if (entry.key()==key){
			rvalue.push_back(&entry);
		}
	}
	return rvalue ;
}

//==============================================================================
auto section_t::first(const std::string &key) const ->const keyvalue_t* {
	auto iter = std::find_if(_entries.cbegin(),_entries.cend(),[&key =std::as_const(key)](const keyvalue_t &entry){
		return entry.key()==key;
	});
	if (iter == _entries.cend()){
		return nullptr;
	}
	return &(*iter) ;
}

//==============================================================================
auto section_t::first(const std::string &key)  -> keyvalue_t*{
	auto iter = std::find_if(_entries.rbegin(),_entries.rend(),[&key =std::as_const(key)](const keyvalue_t &entry){
		return entry.key()==key;
	});
	if (iter == _entries.rend()){
		return nullptr;
	}
	return &(*iter) ;
}

//==============================================================================
auto section_t::last(const std::string &key) const ->const keyvalue_t*{
	auto iter = std::find_if(_entries.crbegin(),_entries.crend(),[&key =std::as_const(key)](const keyvalue_t &entry){
		return entry.key()==key;
	});
	if (iter == _entries.crend()){
		return nullptr ;
	}
	return &(*iter) ;
}

//==============================================================================
auto section_t::last(const std::string &key) -> keyvalue_t*{
	auto iter = std::find_if(_entries.rbegin(),_entries.rend(),[&key =std::as_const(key)](const keyvalue_t &entry){
		return entry.key()==key;
	});
	if (iter == _entries.rend()){
		return nullptr ;
	}
	return &(*iter) ;
}

//==============================================================================
// Direct access to all keyvalues
//==============================================================================
//==============================================================================
auto section_t::operator[](size_t index) const ->const keyvalue_t& {
	return _entries[index];
}
//==============================================================================
auto section_t::operator[](size_t index) -> keyvalue_t& {
	return _entries[index];
}

//==============================================================================
// Add entry , the original is invalid after this call
//==============================================================================

//==============================================================================
auto section_t::push_back(keyvalue_t& keyvalue)->void{
	_entries.push_back(std::move(keyvalue));
}

//==============================================================================
// print out our contents in a key = value format
//==============================================================================

//==============================================================================
auto section_t::contentsDescription(std::ostream &output) const ->void{
	for (const auto &keyvalue : _entries){
		output <<"\t"s<<keyvalue.description()<<"\n"s;
	}
}
