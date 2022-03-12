//Copyright © 2022 Charles Kerr. All rights reserved.

#ifndef section_hpp
#define section_hpp
#include "framework.h"

#include <cstdint>
#include <vector>
#include <istream>
#include <ostream>
#include <string>
#include <optional>

#include "keyvalue.hpp"
#include "header.hpp"
//=========================================================
class section_t {
private:
	header_t _header;
	// We use an vector, as order can be imporant
	std::vector<keyvalue_t> _entries ;
public:
	static auto load(std::istream &input, int &line_number, const std::string &sourcefile)->std::optional<section_t> ;
	// Constructors
	section_t(const header_t &header);
	section_t() ;
	
	// Header retrival
	auto header() const ->const header_t&;
	auto header() ->header_t&;
	
	// Number of entries
	auto size() const ->size_t ;
	
	// Entry reterival
	auto entries() const ->const std::vector<keyvalue_t>& ;
	auto entries()  -> std::vector<keyvalue_t>& ;

	// key based retrevial
	auto contain(const std::string &key) const ->bool ;
	auto find(const std::string &key) const ->std::vector<const keyvalue_t*> ;
	auto first(const std::string &key) const ->const keyvalue_t*;
	auto first(const std::string &key)  -> keyvalue_t*;
	auto last(const std::string &key) const ->const keyvalue_t*;
	auto last(const std::string &key) -> keyvalue_t*;
	
	// Direct access retrevial
	auto operator[](size_t index) const ->const keyvalue_t& ;
	auto operator[](size_t index) -> keyvalue_t& ;
	
	// Add entry , the original is invalid after this call
	auto push_back(keyvalue_t& keyvalue)->void ;
	
	auto contentsDescription(std::ostream &output)const ->void ;

};
#endif /* section_hpp */
