//Copyright © 2022 Charles Kerr. All rights reserved.

#ifndef socket_hpp
#define socket_hpp
#include "framework.h"
#include <cstdint>
#include <string>
#include <memory>
#include <utility>
#include <chrono>

#if defined (_WIN32)
// Windows specific

#include <WS2tcpip.h>
#include <winsock2.h>    // We have to pollute the header, to get the SOCKET , INVALID_SOCKET, SOCKET_ERROR defines
using descriptor_t = SOCKET ;
constexpr descriptor_t invalid_socket = INVALID_SOCKET;
constexpr int socket_error = SOCKET_ERROR ;
#else
// Unix specific
using descriptor_t = int ;
constexpr descriptor_t invalid_socket = -1 ;
constexpr int socket_error = -1 ;
#endif
// So we have ip4addr available to us
#include "ip4addr.hpp"



//=========================================================
// socket_t
//=========================================================
//===================================================================
class socket_t {
private:
	descriptor_t _descriptor ;
	bool _blocking ;
	int _read_flag ;		// Used on read
	int _write_flag ;
	std::chrono::time_point<std::chrono::system_clock> _create_time ;
	auto setOptions()->void ;
	auto setBlocking(bool value)->void ;
	
public:
	socket_t(descriptor_t descriptor = invalid_socket,bool blocking=true);
	virtual ~socket_t() ;
	// Because we close the socket when the instance is destroyed, we need to
	// disable copying or assigning (or the orignial instance will close the socket
	// when it gets destroyed
	socket_t(const socket_t&) = delete ;
	auto operator=(const socket_t&)->socket_t&  = delete ;
	
	auto created() const ->std::chrono::time_point<std::chrono::system_clock>;
	auto peer() const ->std::pair<ip4addr_t,int> ;
	auto blocking() const ->bool ;
	auto blocking(bool value)->void ;
	auto write(const unsigned char *data,int amount)->int ;
	auto read(unsigned char *data,int amount)->int ;
	auto accept(bool peer_blocking = true) ->std::unique_ptr<socket_t> ;
	auto connected() const ->const ip4addr_t& ;
	auto connected() ->ip4addr_t& ;
	static auto listen(const std::string &ip,const std::string &port,bool blocking = true )->std::unique_ptr<socket_t> ;
	static auto connect(const std::string &ip,const std::string &port,bool blocking = true )->std::unique_ptr<socket_t> ;
};
#endif /* socket_hpp */
