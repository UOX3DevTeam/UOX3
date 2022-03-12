//Copyright © 2022 Charles Kerr. All rights reserved.

#ifndef ip4addr_hpp
#define ip4addr_hpp
#include "framework.h"

#include <cstdint>
#include <string>
#include <array>
#include <vector>

//=========================================================
//=========================================================
// The type of ip4 addresses
//=========================================================
enum class ip4type {
	invalid, lan,wan,local,apipa,mcast
};
//===================================================================
// auto iptype(std::uint32_t address)  ->ip4type
//===================================================================
auto iptype(std::uint32_t address)  ->std::pair<ip4type, std::string> ;
auto iptype(const std::string &address)  ->std::pair<ip4type, std::string> ;

//=========================================================
// ip4addr_t
//=========================================================
//=========================================================
// Wildcards (*) or '' (empty) are represented numerically as 0
struct ip4addr_t {
public:
	struct addr_t {
		std::uint8_t value;
		std::string number ;
		auto wildcard()const ->bool {
			return (number == "*") || number.empty() ;
		}
		addr_t();
	};
private:
	std::array<addr_t,4> _address ;
	
	auto set(int value) ->void ;
	auto set(const std::string&value) ->void ;
	auto swapEndian(std::uint32_t addr) const  ->std::uint32_t ;
public:
	ip4addr_t(std::uint32_t addr = 0,bool bigendian = false );
	ip4addr_t(const std::string ip);
	auto operator=(const std::uint32_t &addr) ->ip4addr_t&;
	auto operator=(const std::string &addr) ->ip4addr_t&;
	
	auto component_match(const ip4addr_t &addr) const ->int ;
	
	auto number() const ->std::string ;
	auto value(bool bigendian = false) const ->std::uint32_t ;
	auto type() const ->std::pair<ip4type,const std::string> ;
	
	auto operator[](std::size_t index)->addr_t&;
	auto operator[](std::size_t index) const ->const addr_t&;
	auto operator==(std::uint32_t value) const ->bool ;
	auto operator!=(std::uint32_t value) const ->bool ;
	auto operator==(std::string &value ) const ->bool ;
	auto operator!=(std::string &value) const ->bool ;
	auto operator==(const ip4addr_t &value ) const ->bool ;
	auto operator!=(const ip4addr_t &value) const ->bool ;
};

//=========================================================
// ethdevice_t
//=========================================================
//===================================================================
struct ethdevice_t {
	std::string _name ;
	ip4addr_t _address ;
	ethdevice_t(const std::string &name="",std::uint32_t address=0);
	ethdevice_t(const std::string& name, const ip4addr_t& addr);
	static auto devices() ->std::vector<ethdevice_t> ;
};

#endif /* ip4addr_hpp */
