//Copyright © 2022 Charles Kerr. All rights reserved.

#include "ip4list.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <memory>
#include <chrono>

#include "strutil.hpp"
using namespace std::string_literals;
//=============================================================================
ip4list_t::ip4list_t(const std::string &filepath ){
	if (!filepath.empty()){
		load(filepath) ;
	}
}
//=============================================================================
auto ip4list_t::load(const std::string &filepath)->bool {
	_entries.clear();
	auto rvalue = false ;
	auto constexpr buflen = 4096 ;
	auto buffer = std::make_unique<char[]>(buflen);
	auto input = std::ifstream(filepath) ;
	if (input.is_open()){
		while (input.good() && !input.eof()){
			input.getline(buffer.get(), buflen-1);
			if (input.gcount()>0){
				buffer[static_cast<size_t>(input.gcount())]=0 ;
				auto value = strutil::trim(strutil::strip(std::string(buffer.get()),"//"));
				if (!value.empty()){
					try {
						auto addr = ip4addr_t(value) ;
						_entries.push_back(addr);
					}
					catch(...){
						// Do nothing, continue ;
					}
				}
			}
		}
		rvalue = true ;
		input.close();
	}
	return rvalue ;
}
//=============================================================================
auto ip4list_t::write( std::ostream &output)->void {
	for (const auto &entry : _entries){
		output << entry.number()<<"\n";
	}
}
//=============================================================================
auto ip4list_t::write(const std::string &filepath)->bool {
	auto output = std::ofstream(filepath) ;
	auto rvalue = false ;
	if (output.is_open()){
		rvalue = true ;
		output << "//Written - " << strutil::sysTimeToString(std::chrono::system_clock::now()) <<"\n\n"s ;
		this->write(output) ;
	}
	return rvalue ;
}

//=============================================================================
auto ip4list_t::contain(const ip4addr_t& address) const ->bool {
	return  std::find(_entries.begin(),_entries.end(),address) != _entries.end() ;
	
}
//=============================================================================
auto ip4list_t::operator+(const ip4addr_t& address)->ip4list_t& {
	_entries.push_back(address);
	return *this;
}
//=============================================================================
auto ip4list_t::operator-(const ip4addr_t& address)->ip4list_t& {
	// We need to find the exact match, not the normal operator= (that would match wildcards as well)
	auto iter = std::find_if(_entries.begin(),_entries.end(),[&address](const ip4addr_t value){
		for (auto i=0;i<4;++i){
			if (address[i].number == value[i].number){
				return false ;
			}
		}
		return true ;
	});
	if (iter != _entries.end()){
		_entries.erase(iter);
	}
	return *this;
}
//=============================================================================
auto ip4list_t::empty() const ->bool {
	return _entries.empty();
}
//=============================================================================
auto ip4list_t::size() const -> size_t {
	return _entries.size() ;
}
//=============================================================================
auto ip4list_t::retreive(ip4type type) const->std::vector<ip4addr_t> {
	auto rvalue = std::vector<ip4addr_t>();
	for (const auto& entry : _entries) {
		if (std::get<0>(entry.type()) == type) {
			rvalue.push_back(entry);
		}
	}
	return rvalue;
}
