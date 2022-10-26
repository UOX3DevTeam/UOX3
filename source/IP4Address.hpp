// 
// Created on:  6/8/21

#ifndef IP4Address_hpp
#define IP4Address_hpp
#include <array>
#include <cstdint>
#include <string>
#include <vector>
#include <utility>

//o------------------------------------------------------------------------------------------------o
// ip4addrt
//o------------------------------------------------------------------------------------------------o
struct Ip4Addr_st
{
private:
	static const std::vector<Ip4Addr_st> lanips;
	static const std::vector<Ip4Addr_st> localips;
	static const std::vector<Ip4Addr_st> apipaips;
public:
	enum ip4type_t { invalid, local, lan, apipa, wan };
	std::array<std::string, 4> components;
	
	Ip4Addr_st( std::uint32_t addr, bool bigendian = true );
	Ip4Addr_st( const std::string &value = "" );
	
	auto ipaddr( bool bigendian = true ) const -> std::uint32_t;
	
	auto exact( const Ip4Addr_st& value ) const -> bool;
	
	auto match( std::uint32_t value, bool bigendian = true ) const -> int;
	auto match( const Ip4Addr_st &value ) const -> int;
	
	auto operator == ( const Ip4Addr_st &value ) const -> bool;
	auto operator == ( std::uint32_t value ) const -> bool;
	auto description() const -> std::string;
	auto type() const -> ip4type_t;
};

//o------------------------------------------------------------------------------------------------o
// ip4list_t
//o------------------------------------------------------------------------------------------------o
struct ip4list_t
{
	std::vector<Ip4Addr_st> ipaddresses;
	
	static auto available() ->ip4list_t;
	
	ip4list_t( const std::string &filename = "" );
	auto load( const std::string &filename ) -> bool;
	
	auto add( const Ip4Addr_st &value ) -> void;
	auto remove( const Ip4Addr_st &value ) -> void;
	
	auto size() const -> size_t;
	auto bestmatch( std::uint32_t value, bool bigendian = true ) const -> std::pair<Ip4Addr_st, int>;
	auto bestmatch( const Ip4Addr_st &value ) const -> std::pair<Ip4Addr_st, int>;
	
	auto ips() const -> const std::vector<Ip4Addr_st>&;
	auto ips() -> std::vector<Ip4Addr_st>&;
};

//o------------------------------------------------------------------------------------------------o
// To be removed
#if 0
class IP4Address
{
private:
	std::vector<std::string> _components;

	static std::vector<IP4Address> _myIPs;

	static std::vector<IP4Address> available();
	static IP4Address _externalIP;
	static const std::vector<IP4Address> _privateIPs;
	static const IP4Address _localIP;
	static const IP4Address _APIPA;
	
	// Break an ip into sections
	std::vector<std::string> parseIP( const std::string &ip );

	// Get an ip text into an unsigned int
	unsigned int createIP( const std::string &ip );
	unsigned int convert( int start, int increment ) const;
	static const IP4Address& bestMatch( const IP4Address &address );
public:
	static void loadIPs();
	enum typeIP { wan, lan, local, apipa, mine};
	typeIP type( bool notmine = true ) const;
	IP4Address();
	IP4Address( unsigned int address );
	IP4Address( const std::string &address );
	IP4Address& operator = ( const std::string &address );
	IP4Address& operator = ( const unsigned int &address );
	bool operator == ( const IP4Address &address ) const;
	bool operator != ( const IP4Address &address ) const;
	
	bool match( const IP4Address &address, int level ) const;
	unsigned int littleEndian() const;
	unsigned int bigEndian() const;
	std::string string() const;
	bool valid() const;
	static void setExternal( const std::string &address );
	static IP4Address lookup( const std::string& address );
	static IP4Address respond( IP4Address &address );
	static std::vector<IP4Address> deviceIPs();
	static std::string externalIP();
	
	
};
#endif

#endif /* IP4Address_hpp */
