// 
// Created on:  6/8/21

#include "IP4Address.hpp"


#include <algorithm>
#include <stdexcept>
#include <fstream>
#if defined(_WIN32)
#define NOMINMAX
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stringapiset.h>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "IPHLPAPI.lib")
constexpr auto WORKING_BUFFER_SIZE = 15000;
constexpr auto MAX_TRIES = 3;

#define MALLOC( x ) HeapAlloc( GetProcessHeap(), 0, ( x ))
#define FREE( x ) HeapFree( GetProcessHeap(), 0, ( x ))
#else

#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif
using namespace std::string_literals;

//o------------------------------------------------------------------------------------------------o
//|	string manipulation, in case strutil is not available. Enables this to be standalone
//o------------------------------------------------------------------------------------------------o
auto trim( const std::string &value ) -> std::string
{
	auto rValue = std::string();
	auto startpos = value.find_first_not_of( " \t\r\n\f" );
	if( startpos!= std::string::npos )
	{
		rValue = value.substr( startpos );
		auto endpos = rValue.find_last_not_of( " \t\r\n\f" );
		if( endpos != std::string::npos )
		{
			rValue = rValue.substr( 0, endpos + 1 );
		}
	}
	return rValue;
}
//==================================================================================================
auto parse( const std::string &value, const std::string &separator = "." ) -> std::vector<std::string>
{
	auto rValue = std::vector<std::string>();
	auto subject = trim( value );
	auto position = subject.find( separator );
	if( position == std::string::npos )
	{
		// It coulnd't find the separator
		rValue.push_back( subject );
	}
	else
	{
		while( position != std::string::npos )
		{
			auto parsed = trim( subject.substr( 0, position ));
			rValue.push_back( parsed );
			subject = subject.substr( position + separator.size() );
			position = subject.find( separator );
		}
		subject = trim( subject );
		rValue.push_back( subject );
	}
	return rValue;
}
//==================================================================================================
auto strip( const std::string &value, const std::string &identifier = "//" ) -> std::string
{
	auto position = value.find( identifier );
	return value.substr( 0, position );
}
//==================================================================================================
auto split( const std::string &value, const std::string &identifier = "=" ) -> std::pair<std::string, std::string>
{
	auto position = value.find( identifier );
	auto pos1 = trim( value.substr( 0, position ));
	auto pos2 = trim( value.substr( position + identifier.size() ));
	return std::make_pair( pos1, pos2 );
}

//o------------------------------------------------------------------------------------------------o
//|	Ip4Addr_st
//o------------------------------------------------------------------------------------------------o
const std::vector<Ip4Addr_st> Ip4Addr_st::lanips
{
	Ip4Addr_st( "10.*.*.*"s ), Ip4Addr_st( "192.168.*.*"s ), Ip4Addr_st( "172.16.*.*"s ), Ip4Addr_st( "172.17.*.*"s ),
	Ip4Addr_st( "172.18.*.*"s ), Ip4Addr_st( "172.19.*.*"s ), Ip4Addr_st( "172.20.*.*"s ), Ip4Addr_st( "172.21.*.*"s ),
	Ip4Addr_st( "172.22.*.*"s ), Ip4Addr_st( "172.23.*.*"s ), Ip4Addr_st( "172.241.*.*"s ), Ip4Addr_st( "172.25.*.*"s ),
	Ip4Addr_st( "172.26.*.*"s ), Ip4Addr_st( "172.27.*.*"s ), Ip4Addr_st( "172.28.*.*"s ), Ip4Addr_st( "172.29.*.*"s ),
	Ip4Addr_st( "172.30.*.*"s ), Ip4Addr_st( "172.31.*.*"s )
};
//==================================================================================================
const std::vector<Ip4Addr_st> Ip4Addr_st::localips
{
	Ip4Addr_st( "127.*.*.*" )
};
//==================================================================================================
const std::vector<Ip4Addr_st> Ip4Addr_st::apipaips
{
	Ip4Addr_st( "169.254.*.*" )
};

auto Ip4Addr_st::exact( const Ip4Addr_st& value ) const -> bool
{
	auto rValue = true;
	for( auto i = 0; i < 4; ++i )
	{
		if( components[i] != value.components[i] )
		{
			rValue = false;
			break;
		}
	}
	return rValue;
}

//==================================================================================================
auto Ip4Addr_st::match( std::uint32_t value, bool bigendian ) const -> int
{
	auto ptr = reinterpret_cast<std::uint8_t*>( &value );
	if( !bigendian )
	{
		std::reverse( ptr, ptr + 4 );
	}
	auto match = 0;
	for( const auto &comp : components )
	{
		match += 1;
		if(( comp != "*" ) && !comp.empty() )
		{
			try
			{
				if( std::stoi( comp ) != *ptr )
				{
					match -= 1;
					break;
				}
			}
			catch(...)
			{
				// the value couldn't be converted
				match -= 1;
				break;
			}
		}
	}
	return match;
}
//==================================================================================================
auto Ip4Addr_st::match( const Ip4Addr_st &value ) const -> int
{
	auto rValue = 0;
	for( auto i = 0; i < 4; ++i )
	{
		rValue = i+1;
		if( !((( value.components[i] == "*" ) || value.components[i].empty() ) ||
			(( components[i] == "*" ) || components[i].empty() )))
		{
			// Neither are wild, we have to match them
			if( value.components[i] != components[i] )
			{
				rValue = rValue -1;
				break;
			}
		}
	}
	return rValue;
}
//==================================================================================================
Ip4Addr_st::Ip4Addr_st( const std::string &value )
{
	components.fill( "*"s );
	auto values = parse( value );
	switch( value.size() )
	{
		default:
		case 4:
			components[3] = values[3];
			[[fallthrough]];
		case 3:
			components[2] = values[2];
			[[fallthrough]];
		case 2:
			components[1] = values[1];
			[[fallthrough]];
		case 1:
			components[0] = values[0];
			[[fallthrough]];
		case 0:
			break;
	}
}
//==================================================================================================
Ip4Addr_st::Ip4Addr_st( std::uint32_t addr, bool bigendian )
{
	components.fill( "*"s );
	auto ptr = reinterpret_cast<std::uint8_t*>( &addr );
	if( !bigendian )
	{
		std::reverse( ptr, ptr + 4 );
	}
	for( auto i = 0; i < 4; ++i )
	{
		components[i] = std::to_string( ptr[i] );
	}
}
//==================================================================================================
auto Ip4Addr_st::ipaddr( bool bigendian ) const -> std::uint32_t
{
	auto rValue = std::uint32_t( 0 );
	auto ptr = reinterpret_cast<std::uint8_t*>( &rValue );
	for( auto i = 0; i < 4; ++i )
	{
		auto value = std::uint8_t( 0 );
		try
		{
			value = static_cast<std::uint8_t>( std::stoi( components[i] ));
		}
		catch(...)
		{
			throw std::runtime_error( "Error converting ip to a number"s );
		}
		ptr[i] = value;
	}
	if( !bigendian )
	{
		std::reverse( ptr, ptr + 4 );
	}
	return rValue;
}
//==================================================================================================
// This assumes the value is in big endian
auto Ip4Addr_st::operator == ( std::uint32_t value ) const -> bool
{
	return match( value ) == 4;
}
//==================================================================================================
auto Ip4Addr_st::operator == ( const Ip4Addr_st &value ) const -> bool
{
	return match( value ) == 4;
}
//==================================================================================================
auto Ip4Addr_st::type() const -> ip4type_t
{
	auto rValue = ip4type_t::wan;
	try
	{
		auto iter = std::find_if( lanips.begin(), lanips.end(), [this]( const Ip4Addr_st& ip )
		{
			return *this == ip;
		});
		if( iter != lanips.end() )
		{
			rValue = ip4type_t::lan;
		}
		else
		{
			auto iter = std::find_if( localips.begin(), localips.end(), [this]( const Ip4Addr_st& ip )
			{
				return *this == ip;
			});
			if( iter != localips.end() )
			{
				rValue = ip4type_t::local;
			}
			else
			{
				auto iter = std::find_if( apipaips.begin(), apipaips.end(), [this]( const Ip4Addr_st& ip )
				{
					return *this == ip;
				});
				if( iter != apipaips.end() )
				{
					rValue = ip4type_t::apipa;
				}
			}
		}
	}
	catch(...)
	{
		rValue = ip4type_t::invalid;
	}
	return rValue;
}
//==================================================================================================
auto Ip4Addr_st::description() const -> std::string
{
	auto ip = std::string();
	for( auto &value : components )
	{
		ip += value + "."s;
	}
	return ip.substr( 0, ip.size() - 1 );
}

//o------------------------------------------------------------------------------------------------o
//|	ip4list_t
//o------------------------------------------------------------------------------------------------o

//==================================================================================================
ip4list_t::ip4list_t( const std::string &filename )
{
	if( !filename.empty() )
	{
		load( filename );
	}
}

//==================================================================================================
auto ip4list_t::bestmatch( const Ip4Addr_st &value ) const -> std::pair<Ip4Addr_st, int>
{
	auto matches = std::vector<std::pair<Ip4Addr_st, int>>();
	for( const auto &addr: ipaddresses )
	{
		auto comp = addr.match( value );
		matches.push_back( std::make_pair( addr, comp ));
	}
	std::sort( matches.begin(), matches.end(), []( const std::pair<Ip4Addr_st, int> &lhs, const std::pair<Ip4Addr_st, int> &rhs )
	{
		return lhs.second < rhs.second;
	});
	return *matches.rbegin();
}
//==================================================================================================
auto ip4list_t::bestmatch( std::uint32_t value, bool bigendian ) const -> std::pair<Ip4Addr_st, int>
{
	auto ptr = reinterpret_cast<std::uint8_t*>( &value );
	if( !bigendian )
	{
		std::reverse( ptr, ptr + 4 );
	}
	auto matches = std::vector<std::pair<Ip4Addr_st, int>>();
	for( const auto &addr: ipaddresses )
	{
		auto comp = addr.match( value );
		matches.push_back( std::make_pair( addr, comp ));
	}
	std::sort( matches.begin(), matches.end(), []( const std::pair<Ip4Addr_st, int> &lhs, const std::pair<Ip4Addr_st, int> &rhs )
	{
		return lhs.second < rhs.second;
	});
	return *matches.rbegin();

}
//==================================================================================================
auto ip4list_t::add( const Ip4Addr_st &value ) -> void
{
	ipaddresses.push_back( value );
}
//==================================================================================================
auto ip4list_t::remove( const Ip4Addr_st &value ) -> void
{
	auto iter = std::find_if( ipaddresses.begin(), ipaddresses.end(), [value]( const Ip4Addr_st &entry )
	{
		return value.exact( entry );
	});
	if( iter != ipaddresses.end() )
	{
		ipaddresses.erase(iter);
	}
}
//==================================================================================================
auto ip4list_t::size() const -> size_t
{
	return ipaddresses.size();
}
//==================================================================================================
auto ip4list_t::load( const std::string &filename ) -> bool
{
	auto rValue = false;
	enum state_t { section, startsection, data };
	ipaddresses.clear();
	auto input = std::ifstream( filename );
	if( input.is_open() )
	{
		rValue = true;
		char inputline[4096];
		auto state = state_t::section;
		while( input.good() && !input.eof() )
		{
			input.getline( inputline, 4095 );
			if( input.gcount() > 0 )
			{
				inputline[input.gcount()] = 0;
				auto line = trim( strip(std::string( inputline )));
				if( !line.empty() )
				{
					// look for a section
					switch( static_cast<int>( state ))
					{
						case static_cast<int>( state_t::section ):
						{
							if( line[0] == '[' )
							{
								if( line[line.size() - 1] == ']' )
								{
									// it is a section!
									line = trim( line.substr( 1, line.find( "]" ) - 1 ));
									// apparently we dont look at this?
									state = state_t::startsection;
								}
							}
							break;
						}

						case static_cast<int>( state_t::startsection ):
						{
							if( line[0] == '{' )
							{
								state = state_t::data;
							}
							break;
						}
						case static_cast<int>( state_t::data ):
						{
							if( line[0] != '}' )
							{
								auto [key, value] = split( line, "=" );
								if(( key == "ip" ) || ( key == "IP" ) || ( key == "Ip" ) || ( key == "iP" ))
								{
									ipaddresses.push_back( Ip4Addr_st( value ));
								}
							}
							else
							{
								state = state_t::section;
							}
						}
					}
				}
			}
		}
	}
	return rValue;
}

//==================================================================================================
auto ip4list_t::ips() const -> const std::vector<Ip4Addr_st>&
{
	return ipaddresses;
}
//==================================================================================================
auto ip4list_t::ips() -> std::vector<Ip4Addr_st>&
{
	return ipaddresses;
}

// Unfortunately, the approach here for the unix/windows is almost totally
// different, so effectively, two completely different routines
#if defined(_WIN32)
//==================================================================================================
auto ip4list_t::available() -> ip4list_t
{
	/* Note: could also use malloc() and free() */

	ip4list_t rValue;
	std::string device;
	Ip4Addr_st device_address;

	/* Declare and initialize variables */
	DWORD dwSize = 0;
	DWORD dwRetVal = 0;

	unsigned int i = 0;

	// Set the flags to pass to GetAdaptersAddresses
	ULONG flags = GAA_FLAG_INCLUDE_PREFIX;

	// default to unspecified address family (both)
	ULONG family = AF_INET;

	LPVOID lpMsgBuf = NULL;

	PIP_ADAPTER_ADDRESSES pAddresses = NULL;
	ULONG outBufLen = 0;
	ULONG Iterations = 0;

	PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
	PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;
	PIP_ADAPTER_ANYCAST_ADDRESS pAnycast = NULL;
	PIP_ADAPTER_MULTICAST_ADDRESS pMulticast = NULL;
	IP_ADAPTER_DNS_SERVER_ADDRESS* pDnServer = NULL;
	IP_ADAPTER_PREFIX* pPrefix = NULL;

	// Allocate a 15 KB buffer to start with.
	outBufLen = WORKING_BUFFER_SIZE;

	do
	{
		pAddresses = ( IP_ADAPTER_ADDRESSES* )MALLOC( outBufLen );
		if( pAddresses == nullptr )
		{
			throw std::runtime_error( "Memory allocation files for IP_ADAPTER_ADDRESSES" );
		}

		dwRetVal = GetAdaptersAddresses( family, flags, NULL, pAddresses, &outBufLen );

		if( dwRetVal == ERROR_BUFFER_OVERFLOW )
		{
			FREE( pAddresses );
			pAddresses = NULL;
		}
		else
		{
			break;
		}

		Iterations++;

	} while(( dwRetVal == ERROR_BUFFER_OVERFLOW ) && ( Iterations < MAX_TRIES ));

	if( dwRetVal == NO_ERROR )
	{
		// If successful, output some information from the data we received
		pCurrAddresses = pAddresses;
		while( pCurrAddresses )
		{
			pUnicast = pCurrAddresses->FirstUnicastAddress;
			if( pUnicast != nullptr )
			{
				if( pUnicast->Address.lpSockaddr->sa_family == AF_INET )
				{
					for( i = 0; pUnicast != nullptr; i++ )
					{
						const int friendlen = 200;

						char friendly[friendlen];
						std::memset( friendly, 0, friendlen );
						sockaddr_in* sa_in = ( sockaddr_in* )pUnicast->Address.lpSockaddr;
						device_address = Ip4Addr_st( sa_in->sin_addr.S_un.S_addr );

						if( device_address.type() != Ip4Addr_st::ip4type_t::apipa )
						{
							//ourdevice.address = inet_ntop( AF_INET, &( sa_in->sin_addr ), buff, bufflen );
							BOOL conv = false;
							device = "";
							if( WideCharToMultiByte( CP_UTF8, 0, pCurrAddresses->FriendlyName, -1, friendly, friendlen, 0, &conv ) > 0 )
							{
								device = friendly;
							}
							if( !device.empty() )
							{
								// The device has a name, might be intersted
								if( device.find( "(WSL)" ) == std::string::npos )
								{
									// we dont want a psuedo WSL device on windows
									rValue.add( device_address );
								}
							}
						}
						pUnicast = pUnicast->Next;
					}
				}
			}

			pCurrAddresses = pCurrAddresses->Next;
		}
	}
	else
	{
		if( dwRetVal != ERROR_NO_DATA )
		{
			if( pAddresses )
			{
				FREE( pAddresses );
			}

			throw std::runtime_error( "Unable to get address info" );
		}
	}

	if( pAddresses )
	{
		FREE( pAddresses );
	}
	return rValue;
}
#else
auto ip4list_t::available() -> ip4list_t
{
	ip4list_t rValue;
	struct ifaddrs * ifAddrStruct = NULL;
	struct ifaddrs * ifa = NULL;
	Ip4Addr_st device_address;

	getifaddrs( &ifAddrStruct );

	for( ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next )
	{
		if( !ifa->ifa_addr )
		{
			continue;
		}
		if( ifa->ifa_addr->sa_family == AF_INET ) // check it is IP4
		{
			// is a valid IP4 Address
			auto holder = *reinterpret_cast<sockaddr_in *>( ifa->ifa_addr );
			auto addr = Ip4Addr_st( holder.sin_addr.s_addr );
			if( addr.type() != Ip4Addr_st::ip4type_t::apipa )
			{
				rValue.add( addr );
			}
		}
	}
	if( ifAddrStruct != NULL )
	{
		freeifaddrs( ifAddrStruct );
	}
	return rValue;
}

#endif



#if 0


#include "ConfigOS.h"

#include <iostream>
#include <stdexcept>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <sstream>

#if PLATFORM == WINDOWS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stringapiset.h>

#pragma comment( lib, "Ws2_32.lib" )
#pragma comment( lib, "IPHLPAPI.lib" )
#define WORKING_BUFFER_SIZE 15000
#define MAX_TRIES 3

#define MALLOC( x ) HeapAlloc(GetProcessHeap(), 0, ( x ))
#define FREE( x ) HeapFree(GetProcessHeap(), 0, ( x ))

#else

#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>

#endif
//+++++++++++++++++++++++++++++++++++++++++++++++++++
//
//		Methods for IP4Address
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++
//
const std::vector<IP4Address> IP4Address::_privateIPs  = {IP4Address( "10.*.*.*" ), IP4Address( "192.168.*.*" ),
	IP4Address( "172.16.*.*" ), IP4Address( "172.17.*.*"), IP4Address( "172.18.*.*" ), IP4Address( "172.19.*.*" ),
	IP4Address( "172.20.*.*" ), IP4Address( "172.21.*.*" ), IP4Address( "172.22.*.*" ), IP4Address( "172.23.*.*" ),
	IP4Address( "172.241.*.*" ), IP4Address( "172.25.*.*" ), IP4Address( "172.26.*.*" ), IP4Address( "172.27.*.*" ),
	IP4Address( "172.28.*.*" ), IP4Address( "172.29.*.*" ), IP4Address( "172.30.*.*" ), IP4Address( "172.31.*.*" )
};

const IP4Address IP4Address::_localIP = IP4Address( "127.*.*.*" );

const IP4Address IP4Address::_APIPA = IP4Address( "169.254.*.*" );

std::vector<IP4Address> IP4Address::_myIPs = std::vector<IP4Address>();

IP4Address IP4Address::_externalIP = IP4Address( "*.*.*.*" );
//==================================================================================================
void IP4Address::loadIPs()
{
	_myIPs = available();
}


//==================================================================================================
void IP4Address::setExternal( const std::string &address )
{
	IP4Address::_externalIP = lookup( address );
}

//==================================================================================================
std::vector<std::string> IP4Address::parseIP( const std::string &ip )
{
	// Examples:  	192.168.1.0
	//			192..1.0
	//			192.*.1.0

	std::vector<std::string> rValue;
	std::string::size_type startloc = 0;
	std::string::size_type endloc = ip.size();
	std::size_t increment = 0;
	for( increment = 0; increment < 3; increment++ )
	{
		if( startloc >= ip.size() )
		{
			endloc = std::string::npos;
		}
		else
		{
			endloc = ip.find( ".", startloc );
		}
		std::string entry;
		entry = "";
		// Coudn't find it
		if( endloc == std::string::npos )
		{
			if( startloc < ip.size() )
			{
				entry = ip.substr( startloc );
			}
		}
		else
		{
			entry = ip.substr( startloc, endloc - startloc );
		}
		if( entry.empty() )
		{
			entry = "*";
		}
		rValue.push_back( entry );
		if( endloc == std::string::npos )
		{
			break;
		}
		startloc = endloc + 1;
	}
	if( increment < 3 )
	{
		for( auto i = increment + 1; i < 4; i++ )
		{
			rValue.push_back( "*" );
		}
	}
	else
	{
		// just need to find the last
		std::string entry;
		if( startloc < ip.size() )
		{
			entry = ip.substr( startloc );
		}
		if( entry.empty() )
		{
			entry = "*";
		}
		rValue.push_back( entry );
	}
	return rValue;
}
//==================================================================================================
unsigned int IP4Address::createIP( const std::string &ip )
{
	auto values = parseIP( ip );
	std::vector<char> numbers;
	numbers.resize( 4, 0 );
	for( auto i = 0; i < 4; i++ )
	{
		if( values[i] != "*" )
		{
			numbers[i] = static_cast<char>( std::stoi( values[i] ));
		}
	}
	return *reinterpret_cast<unsigned int*>( numbers.data() );
}
//==================================================================================================
std::string IP4Address::string() const
{
	return _components[0] + "." + _components[1]+ "." + _components[2] + "." + _components[3];
}
//==================================================================================================
IP4Address::IP4Address()
{
	_components.resize( 4, "*" );
}
//==================================================================================================
IP4Address::IP4Address( unsigned int address ) : IP4Address()
{
	_components.clear();
	_components.push_back( std::to_string((( address >> 24 ) & 0xFF )));
	_components.push_back( std::to_string((( address >> 16 ) & 0xFF )));
	_components.push_back( std::to_string((( address >> 8 ) & 0xFF )));
	_components.push_back( std::to_string((( address ) & 0xFF )));
}
//==================================================================================================
IP4Address::IP4Address( const std::string &address ) : IP4Address()
{
	_components = parseIP( address );
}
//==================================================================================================
IP4Address& IP4Address::operator = ( const std::string &address )
{
	_components.clear();
	_components = parseIP( address );
	return *this;
}
//==================================================================================================
IP4Address& IP4Address::operator = ( const unsigned int &address )
{
	_components.clear();
	_components.push_back( std::to_string((( address >> 24 ) & 0xFF )));
	_components.push_back( std::to_string((( address >> 16 ) & 0xFF )));
	_components.push_back( std::to_string((( address >> 8 ) & 0xFF )));
	_components.push_back( std::to_string((( address ) & 0xFF )));
	return *this;
}

//==================================================================================================
std::vector<IP4Address> IP4Address::deviceIPs()
{
	return _myIPs;
}
//==================================================================================================
std::string IP4Address::externalIP()
{
	return _externalIP.string();
}

//==================================================================================================
bool IP4Address::operator == ( const IP4Address &address ) const
{
	return match( address, 4 );
}
//==================================================================================================
bool IP4Address::operator != ( const IP4Address &address ) const
{
	return !match( address, 4 );
}

//==================================================================================================
bool IP4Address::match( const IP4Address &address, int level ) const
{
	for( auto i = 0; i < level; i++ )
	{
		if( !(( _components[i] == "*" ) || ( address._components[i] == "*" )))
		{
			if( _components[i] != address._components[i] )
			{
				return false;
			}
		}
	}
	return true;
}
//==================================================================================================
IP4Address::typeIP IP4Address::type( bool notmine ) const
{
	// Check for a match!
	if( !notmine )
	{
		for( auto &entry: _myIPs )
		{
			if( entry == *this )
			{
				return mine;
			}
		}
	}
	if( _externalIP == *this )
	{
		return mine;
	}
	if( *this == _localIP )
	{
		return local;
	}

	if( *this == _APIPA )
	{
		return apipa;
	}
	for( auto &entry: _privateIPs )
	{
		if( entry == *this )
		{
			return lan;
		}
	}
	return wan;
}
//==================================================================================================
IP4Address IP4Address::respond( IP4Address &address )
{
	// get the type
	auto ttype = address.type( false );
	if( ttype == mine )
	{
		// respond with local host
		return IP4Address( "127.0.0.1" );
	}
	else if( ttype == lan )
	{
		return bestMatch( address );
	}
	return IP4Address::_externalIP;
}

//==================================================================================================
const IP4Address& IP4Address::bestMatch( const IP4Address &address )
{
	if( _myIPs.size() == 0 )
	{
		return _externalIP;
	}
	auto matchcount = 0;
	auto index = -1;
	for( auto i = 0; i < _myIPs.size(); i++ )
	{
		auto count = 0;
		for( auto j = 0; j < 4; j++ )
		{
			if( _myIPs[i]._components[j] != address._components[j] )
			{
				break;
			}
			count = count + 1;
		}
		if( count >= matchcount )
		{
			index = i;
			matchcount = count;
		}
	}
	if( matchcount == 0 )
	{
		return _externalIP;
	}
	else
	{
		return _myIPs[index];
	}
}
//==================================================================================================
unsigned int IP4Address::littleEndian() const
{
	return convert( 3, -1 );
}
//==================================================================================================
unsigned int IP4Address::bigEndian() const
{
	return convert( 0, 1 );
}
//==================================================================================================
unsigned int IP4Address::convert( int start, int increment ) const
{
	unsigned int address = 0;
	auto count = start;
	for( auto &entry: _components )
	{
		unsigned int temp = 0;
		try
		{
			temp = static_cast<unsigned int>( std::stoi( entry ));
		}
		catch(...)
		{
			temp = 0;
		}
		address = ( temp << ( count * 8 )) | address;
		count = count + increment;
	}
	return address;
}

//==================================================================================================
bool IP4Address::valid() const
{
	for( auto &entry : _components )
	{
		if( entry != "*" )
		{
			try
			{
				auto value = std::stoi( entry );
				if(( value < 0 ) || ( value > 255 ))
				{
					return false;
				}
			}
			catch(...)
			{
				return false;
			}
		}
	}
	return true;
}

//==================================================================================================
IP4Address IP4Address::lookup( const std::string& address )
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	std::memset( &hints, 0, sizeof( hints ));
	hints.ai_family = AF_INET;    /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
	hints.ai_flags = 0;
	hints.ai_protocol = 0;          /* Any protocol */
#if PLATFORM == WINDOWS
	WSAData wsdata;
	int startresult = WSAStartup( MAKEWORD( 2, 2 ), &wsdata );
	if( startresult != 0 )
	{
		throw std::runtime_error( std::string( "Error start Winsock: " ) + std::to_string( startresult ));
	}
#endif
	int status = getaddrinfo( address.c_str(), nullptr, &hints, &result );
	if( status != 0 )
	{
#if PLATFORM == WINDOWS
		WSACleanup();
#endif
		return IP4Address( address );
		/*
#if PLATFORM == WINDOWS
		WSACleanup();
		throw std::runtime_error( std::string( "Error on DNS lookup for ") + address+std::string( " : " ) + std::to_string( WSAGetLastError() ));
#else
		throw std::runtime_error( std::string( "Error on DNS lookup for ") + address+std::string( " : " )+std::string( gai_strerror( status )));
#endif
		 */
	}
	else
	{
		for( rp = result; rp != nullptr; rp = rp->ai_next )
		{
			if( rp->ai_family == AF_INET )
			{
				sockaddr_in adr = *reinterpret_cast<sockaddr_in*>( rp->ai_addr );
				auto number =  ntohl( adr.sin_addr.s_addr );
				freeaddrinfo( result );
#if PLATFORM == WINDOWS
				WSACleanup();
#endif
				return IP4Address( number );
			}
		}
		freeaddrinfo( result );
		return IP4Address();
	}
}


// Unfortunately, the approach here for the unix/windows is almost totally
// different, so effectively, to completely different routines
#if PLATFORM == WINDOWS
//==================================================================================================
std::vector<IP4Address> IP4Address::available()
{
	/* Note: could also use malloc() and free() */

	std::vector<IP4Address> rValue;
	std::string device;
	IP4Address device_address;

		/* Declare and initialize variables */
		DWORD dwSize = 0;
		DWORD dwRetVal = 0;

		unsigned int i = 0;

		// Set the flags to pass to GetAdaptersAddresses
		ULONG flags = GAA_FLAG_INCLUDE_PREFIX;

		// default to unspecified address family (both)
		ULONG family = AF_INET;

		LPVOID lpMsgBuf = NULL;

		PIP_ADAPTER_ADDRESSES pAddresses = NULL;
		ULONG outBufLen = 0;
		ULONG Iterations = 0;

		PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
		PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;
		PIP_ADAPTER_ANYCAST_ADDRESS pAnycast = NULL;
		PIP_ADAPTER_MULTICAST_ADDRESS pMulticast = NULL;
		IP_ADAPTER_DNS_SERVER_ADDRESS* pDnServer = NULL;
		IP_ADAPTER_PREFIX* pPrefix = NULL;

		// Allocate a 15 KB buffer to start with.
		outBufLen = WORKING_BUFFER_SIZE;

		do
		{
			pAddresses = ( IP_ADAPTER_ADDRESSES* )MALLOC( outBufLen );
			if( pAddresses == nullptr )
			{
				throw std::runtime_error( "Memory allocation files for IP_ADAPTER_ADDRESSES" );
			}

			dwRetVal = GetAdaptersAddresses( family, flags, NULL, pAddresses, &outBufLen );

			if( dwRetVal == ERROR_BUFFER_OVERFLOW )
			{
				FREE( pAddresses );
				pAddresses = NULL;
			}
			else
			{
				break;
			}

			Iterations++;

		} while(( dwRetVal == ERROR_BUFFER_OVERFLOW ) && ( Iterations < MAX_TRIES ));

		if( dwRetVal == NO_ERROR )
		{
			// If successful, output some information from the data we received
			pCurrAddresses = pAddresses;
			while( pCurrAddresses )
			{
				pUnicast = pCurrAddresses->FirstUnicastAddress;
				if( pUnicast != nullptr )
				{
					if( pUnicast->Address.lpSockaddr->sa_family == AF_INET )
					{
						for( i = 0; pUnicast != nullptr; i++ )
						{
							const int friendlen = 200;

							char friendly[friendlen];
							std::memset( friendly, 0, friendlen );
							sockaddr_in* sa_in = ( sockaddr_in* )pUnicast->Address.lpSockaddr;
							device_address = IP4Address( ntohl( sa_in->sin_addr.S_un.S_addr ));

							if( device_address != _APIPA )
							{
								//ourdevice.address = inet_ntop(AF_INET, &(sa_in->sin_addr), buff, bufflen);
								BOOL conv = false;
								device = "";
								if( WideCharToMultiByte( CP_UTF8, 0, pCurrAddresses->FriendlyName, -1, friendly, friendlen, 0, &conv )> 0 )
								{
									device = friendly;
								}
								if( !device.empty() )
								{
									// The device has a name, might be intersted
									if( device.find( "(WSL)" ) == std::string::npos )
									{
										// we dont want a psuedo WSL device on windows
										rValue.push_back( device_address );
									}
								}
							}
							pUnicast = pUnicast->Next;
						}
					}
				}
				pCurrAddresses = pCurrAddresses->Next;
			}
		}
		else
		{
			if( dwRetVal != ERROR_NO_DATA )
			{
				if( pAddresses )
				{
					FREE( pAddresses );
				}

				throw std::runtime_error( "Unable to get address info" );
			}
		}

		if( pAddresses )
		{
			FREE( pAddresses );
		}
		return rValue;
}
#else
std::vector<IP4Address> IP4Address::available()
{
	std::vector<IP4Address> rValue;
	struct ifaddrs * ifAddrStruct = NULL;
	struct ifaddrs * ifa = NULL;
	void * tmpAddrPtr = NULL;
	IP4Address device_address;

	getifaddrs( &ifAddrStruct );

	for( ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next )
	{
		if( !ifa->ifa_addr )
		{
			continue;
		}
		if( ifa->ifa_addr->sa_family == AF_INET ) // check it is IP4
		{
			// is a valid IP4 Address
			tmpAddrPtr = &(( struct sockaddr_in * )ifa->ifa_addr )->sin_addr;
			auto holder = *reinterpret_cast<sockaddr_in *>( ifa->ifa_addr );
			auto addr = IP4Address( ntohl( holder.sin_addr.s_addr ));
			if( addr != _APIPA )
			{
				rValue.push_back( addr );
			}
		}
	}
	if( ifAddrStruct != NULL )
	{
		freeifaddrs( ifAddrStruct );
	}
	return rValue;
}

#endif
#endif
