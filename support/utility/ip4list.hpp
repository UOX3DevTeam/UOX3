//Copyright © 2022 Charles Kerr. All rights reserved.

#ifndef ip4list_hpp
#define ip4list_hpp
#include "framework.h"
#include "ip4addr.hpp"

#include <cstdint>
#include <vector>
#include <array>
#include <string>
#include <ostream>

//=========================================================
// This is just a list of ip addresses
//
class ip4list_t {
public:
	std::vector<ip4addr_t> _entries ;
public:
	ip4list_t(const std::string &filepath = "");
	auto load(const std::string &filepath)->bool ;
	auto write( std::ostream &output)->void ;
	auto write(const std::string &filepath)->bool ;
	auto contain(const ip4addr_t& address)const ->bool ;
	auto operator+(const ip4addr_t& address)->ip4list_t& ;
	auto operator-(const ip4addr_t& address)->ip4list_t& ;
	auto empty() const ->bool ;
	auto size() const -> size_t ;
	auto retreive(ip4type type) const->std::vector<ip4addr_t>;
};
#endif /* ip4list_hpp */
