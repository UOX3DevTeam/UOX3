//Copyright © 2022 Charles Kerr. All rights reserved.

#include "ip4addr.hpp"

#include <iostream>
#include <algorithm>
#include <vector>

#if defined(_WIN32)
// Windows specific
#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif
#if !defined(NOMINMAX)
#define NOMINMAX
#endif

#include <Windows.h>
#include <WinBase.h>
#include <WinSock2.h>
#include <stringapiset.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "Ws2_32.lib")
#else
// Unix specific
#include <ifaddrs.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include "strutil.hpp"

//=========================================================
using namespace std::string_literals ;
//=========================================================
// names for the ip4 types
//=========================================================
static auto _ip4names = std::array<std::string,6>{"invalid"s,"lan"s,"wan"s,"local"s,"apipa"s,"mcast"s};

//=========================================================
// auto iptype(std::uint32_t address)  ->ip4type ;
//=========================================================
//=========================================================
auto internal_iptype(std::uint32_t address)  ->ip4type {
	// First look at the type byte of the address
	auto msb = static_cast<std::uint8_t>((address>>24)&0xFF);
	auto msb2 = static_cast<std::uint8_t>((address>>16)&0xFF);

	switch (msb){
		case 127:
			return ip4type::local;
		case 192:
		{
			if (msb2==168){
				return ip4type::lan;
			}
			return ip4type::wan;
		}
		case 172:{
			
			if ((msb2>=16)&& (msb2<=31)){
				return ip4type::lan;
			}
			return ip4type::wan;
		}
		case 10:
			return ip4type::lan;
		case 169:
		{
			if (msb2==254){
				return ip4type::apipa;
			}
			return ip4type::wan;
		}
		case 224:
		{
			switch(msb2){
				case 0:
				case 1:
				case 3:
					return ip4type::mcast;
				default:
					return ip4type::wan ;
			}
		}
		case 225:
		case 226:
		case 227:
		case 228:
		case 229:
		case 230:
		case 231:
		case 232:
		case 235:
		case 236:
		case 237:
		case 238:
		case 239:
			return ip4type::mcast ;
		default:
			return ip4type::wan;
	}
}
//=========================================================
auto iptype(std::uint32_t address) ->std::pair<ip4type, std::string> {
	auto type = internal_iptype(address) ;
	return std::make_pair(type,_ip4names[static_cast<int>(type)]);
}
//=========================================================
auto iptype(const std::string &address) ->std::pair<ip4type,std::string>{
	return (iptype(ip4addr_t(address).value()));
}
//=========================================================
// ip4addr_t
//=========================================================

//=========================================================
ip4addr_t::addr_t::addr_t() :value(0) {

}
//=========================================================
ip4addr_t::ip4addr_t(std::uint32_t addr ,bool bigendian  ){
	if (bigendian) {
		addr = swapEndian(addr);
	}
	set(addr) ;
}
//=========================================================
ip4addr_t::ip4addr_t(const std::string ip){
	set(ip) ;
}
//=========================================================
auto ip4addr_t::set(int value) ->void {
	// now break up the pieces
	for (auto i=0 ; i<4 ; ++i){
		_address[i].value = static_cast<std::uint8_t>(((value >> (8*i))&0xFF));
		_address[i].number = std::to_string(_address[i].value);
	}
}

//=========================================================
auto ip4addr_t::set(const std::string &value) ->void {
	auto pieces = strutil::parse(value,".");
	if (pieces.size() != 4){
		throw std::runtime_error("IP address is malformed");
	}
	for (auto i=0 ; i<4 ; ++i){
		_address[3-i].value = strutil::ston<std::uint8_t>(pieces[i]) ;
		_address[3-i].number = pieces[i] ;
	}
}
//=========================================================
auto ip4addr_t::operator=(const std::uint32_t &addr) ->ip4addr_t& {
	set(addr) ;
	return *this;
}
//=========================================================
auto ip4addr_t::operator=(const std::string &addr) ->ip4addr_t&{
	set(addr);
	return *this;
}

//=========================================================
auto ip4addr_t::component_match(const ip4addr_t &addr) const ->int {
	auto match = 0 ;
	// We start from the top (entry 3) and work down (entry 0)
	for (auto i=3 ; i>=0 ;--i){
		if (!_address[i].wildcard() && !addr._address[i].wildcard()){
			if (_address[i].value != addr._address[i].value) {
				break;
			}
		}
		match +=1 ;
	}
	return match ;
}


//=========================================================
auto ip4addr_t::number() const ->std::string {
	return _address[3].number+"."s+_address[2].number+"."+_address[1].number+"."s+_address[0].number ;
}
//=========================================================
auto ip4addr_t::value(bool bigendian) const ->std::uint32_t {
	std::uint32_t ip{0} ;
	for (auto i=0; i<4;++i){
		if (!_address[i].wildcard()){
			ip |= (static_cast<std::uint32_t>(_address[i].value)<<(i*8));
		}
	}
	if (bigendian) {
		ip = swapEndian(ip);
	}
	return ip ;
}
//=========================================================
auto ip4addr_t::swapEndian(std::uint32_t addr) const ->std::uint32_t {
	
	std::reverse(reinterpret_cast<std::uint8_t*>(&addr), reinterpret_cast<std::uint8_t*>(&addr)+4);
	return addr ;
}
//=========================================================
auto ip4addr_t::type() const ->std::pair<ip4type,const std::string> {
	return iptype(this->value());
}

//=========================================================
auto ip4addr_t::operator[](std::size_t index)->addr_t&{
	return _address[index] ;
}
//=========================================================
auto ip4addr_t::operator[](std::size_t index) const ->const addr_t&{
	return _address[index];
}
//=========================================================
auto ip4addr_t::operator==(std::uint32_t value) const ->bool {
	return operator==(ip4addr_t(value));
}
//=========================================================
auto ip4addr_t::operator!=(std::uint32_t value) const ->bool {
	return !operator==(value);
}
//=========================================================
auto ip4addr_t::operator==(std::string &value ) const ->bool {
	return operator==(ip4addr_t(value) );
}
//=========================================================
auto ip4addr_t::operator!=(std::string &value) const ->bool {
	return !operator==(value);
}
//=========================================================
auto ip4addr_t::operator==(const ip4addr_t &value ) const ->bool {
	for (auto i=0 ; i<4 ; ++i){
		if (!value._address[i].wildcard() && !_address[i].wildcard()){
			if (value._address[i].value != _address[i].value){
				return false ;
			}
		}
	}
	return true ;
}
//=========================================================
auto ip4addr_t::operator!=(const ip4addr_t &value) const ->bool {
	return !operator==(value);
}

//=========================================================
// ethdevice_t
//=========================================================
//=========================================================
ethdevice_t::ethdevice_t(const std::string &name,std::uint32_t address):_name(name),_address(address){
	
}
//=========================================================
ethdevice_t::ethdevice_t(const std::string& name, const ip4addr_t& addr):_name(name),_address(addr) {
	
}

//=========================================================
// Unfortunely, windows and "unix's" are totally different on this, so we just
// provide two routines

#if defined(_WIN32)
//=========================================================
auto ethdevice_t::devices() ->std::vector<ethdevice_t>{
	auto rvalue = std::vector<ethdevice_t>() ;
	// We only really want IP4
	std::uint32_t family = AF_INET;
	// Return the addresses (now, this may cause IP6 to also be returned)
	std::uint32_t flags = GAA_FLAG_INCLUDE_PREFIX;
	
	int count = 0;
	constexpr int max_tries = 3;
	void* reserved = nullptr;
	PIP_ADAPTER_ADDRESSES adapter_addresses = nullptr ;
	std::uint32_t* size_pointer = nullptr;
	
	// We dont know how much memory we need, since we dont know the number of adapters.
	// So we are going to allocate some, see if we dont get a buffer overflow. if we do get an overflow
	// we will release what we got,  and try again. The GetAdapterAddresses returns the size required in our output_length
	// variable, assuming no adapter gets added between calls.  We do this up to max_tries times, and then
	// just bail out
	// Now we get this from the HEAP, we could use normal new/free, but this is a tad faster (in theory, as it doesn't relocate)
	constexpr unsigned long default_working_size = 15000;  // This is the recommended starting size provided by MS documents
	unsigned long output_length = default_working_size;
	do {
		// Get some memory
		adapter_addresses = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(HeapAlloc(GetProcessHeap(), 0, output_length));
		if (adapter_addresses == nullptr) {
			// We now have a huge problem, we couldn't get the memory!
			throw std::runtime_error("Unable to allocate memory for networkDevices()");
		}
		// We have our memory, see if we can get the devices into it
		auto status = GetAdaptersAddresses(family, flags, NULL, adapter_addresses, &output_length);
		if (status == ERROR_BUFFER_OVERFLOW) {
			// Drat, not enough memory, first, free what we did get
			HeapFree(GetProcessHeap(), 0, adapter_addresses);
			adapter_addresses = nullptr;
			count++;
		}
		else if (status != NO_ERROR) {
			// We had some error , not memory related
			HeapFree(GetProcessHeap(), 0, adapter_addresses);
			adapter_addresses = nullptr;
			break;
		}
		
	} while ((adapter_addresses == nullptr) && (count < max_tries));
	if (adapter_addresses != nullptr) {
		// We got some data, process it!
		auto current = adapter_addresses;
		while (current != nullptr) {
			auto unicast = current->FirstUnicastAddress;
			if (unicast != nullptr) {
				if (unicast->Address.lpSockaddr->sa_family == AF_INET) {
					// We want to get all the addresses for this adapter
					while (unicast != nullptr) {
						constexpr int namelength = 200;
						auto name_buffer = std::vector<char>(namelength, 0);
						auto addr = ip4addr_t(reinterpret_cast<sockaddr_in*>(unicast->Address.lpSockaddr)->sin_addr.S_un.S_addr, true);
						// We dont want any device that is looking for auto determination
						if (!((addr[3].value == 169) && (addr[2].value == 254))) {
							// Get the device name
							auto conv = 0;  // This is a BOOL, but windows makes BOOL an int
							if (WideCharToMultiByte(CP_UTF8, 0, current->FriendlyName, -1, name_buffer.data(), static_cast<int>(name_buffer.size()), 0, &conv) > 0) {
								rvalue.push_back(std::move(ethdevice_t(name_buffer.data(), addr)));
							}
						}
						unicast = unicast->Next;
					}
					
				}
			}
			current = current->Next;
		}
		HeapFree(GetProcessHeap(), 0, adapter_addresses);
	}
	return rvalue;
}
#else
//=========================================================
auto ethdevice_t::devices() ->std::vector<ethdevice_t>{
	auto rvalue = std::vector<ethdevice_t>() ;
	struct ifaddrs *ifap = nullptr ;
	auto status = getifaddrs(&ifap) ;
	if (status == 0){
		for (auto current = ifap ; current!=nullptr; current = current->ifa_next){
			if (current->ifa_addr != nullptr){
				if (current->ifa_addr->sa_family == AF_INET){
					auto device = ethdevice_t() ;
					device._name = current->ifa_name;
					device._address = ip4addr_t(reinterpret_cast<sockaddr_in*>(current->ifa_addr)->sin_addr.s_addr,true);
					rvalue.push_back(device);
				}
			}
		}
		freeifaddrs(ifap);
	}
	return rvalue ;
}
#endif
