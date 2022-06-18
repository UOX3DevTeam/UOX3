// 
// Created on:  6/8/21

#ifndef IP4Address_hpp
#define IP4Address_hpp
#include <array>
#include <cstdint>
#include <string>
#include <vector>
#include <utility>

//=================================================================================
// ip4addr_t
//=================================================================================
struct ip4addr_t {
private:
	static const std::vector<ip4addr_t> lanips ;
	static const std::vector<ip4addr_t> localips ;
	static const std::vector<ip4addr_t> apipaips ;
public:
	enum ip4type_t {invalid,local,lan,apipa,wan};
	std::array<std::string,4> components ;
	
	ip4addr_t(std::uint32_t addr,bool bigendian = true);
	ip4addr_t(const std::string &value = "");
	
	auto ipaddr(bool bigendian = true) const ->std::uint32_t ;
	
	auto exact(const ip4addr_t& value) const ->bool ;
	
	auto match(std::uint32_t value, bool bigendian = true ) const -> int ;
	auto match(const ip4addr_t &value) const ->int;
	
	auto operator==(const ip4addr_t &value) const ->bool ;
	auto operator==(std::uint32_t value) const ->bool ;
	auto description() const ->std::string ;
	auto type() const ->ip4type_t ;
	
};

//=================================================================================
// ip4list_t
//=================================================================================
struct ip4list_t {
	std::vector<ip4addr_t> ipaddresses ;
	
	static auto available() ->ip4list_t;
	
	ip4list_t(const std::string &filename="") ;
	auto load(const std::string &filename) ->bool ;
	
	auto add(const ip4addr_t &value) ->void ;
	auto remove(const ip4addr_t &value) ->void ;
	
	auto size() const ->size_t ;
	auto bestmatch(std::uint32_t value, bool bigendian = true ) const ->std::pair<ip4addr_t,int> ;
	auto bestmatch(const ip4addr_t &value) const ->std::pair<ip4addr_t,int> ;
	
	auto ips() const ->const std::vector<ip4addr_t>& ;
	auto ips() -> std::vector<ip4addr_t>&;
};


//===================================================================================
// To be removed
#if 0
class IP4Address {
private:
	std::vector<std::string> _components ;

	static std::vector<IP4Address> _myIPs ;

	static std::vector<IP4Address> available() ;
	static IP4Address _externalIP ;
	static const std::vector<IP4Address> _privateIPs ;
	static const IP4Address _localIP;
	static const IP4Address _APIPA ;
	
	// Break an ip into sections
	std::vector<std::string> parseIP(const std::string &ip);

	// Get an ip text into an unsigned int
	unsigned int createIP(const std::string &ip);
	unsigned int convert(int start, int increment) const;
	static const IP4Address& bestMatch(const IP4Address &address);
public:
	static void loadIPs() ;
	enum typeIP { wan,lan,local,apipa,mine};
	typeIP type(bool notmine = true ) const ;
	IP4Address() ;
	IP4Address(unsigned int address);
	IP4Address(const std::string &address);
	IP4Address& operator=(const std::string &address);
	IP4Address& operator=(const unsigned int &address);
	bool operator==(const IP4Address &address) const ;
	bool operator!=(const IP4Address &address) const ;
	
	bool match(const IP4Address &address, int level) const ;
	unsigned int littleEndian() const ;
	unsigned int bigEndian() const;
	std::string string() const ;
	bool valid() const ;
	static void setExternal(const std::string &address) ;
	static IP4Address lookup(const std::string& address);
	static IP4Address respond(IP4Address &address);
	static std::vector<IP4Address> deviceIPs() ;
	static std::string externalIP()  ;
	
	
};
#endif

#endif /* IP4Address_hpp */
