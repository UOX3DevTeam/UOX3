// Copyright © 2021 Charles Kerr. All rights reserved.
// Created on:  5/25/21

#ifndef Socket_hpp
#define Socket_hpp
#include "ConfigOS.h"
#if PLATFORM == WINDOWS			// We do this here, to get SOCKET defined
#include <winsock2.h>
#include <ws2tcpip.h>
//#pragma message XSTR(PLATFORM)
#pragma comment(lib, "Ws2_32.lib")
#endif

#include <cstdint>
#include <string>
#include <vector>
#include <tuple>
#include <mutex>
#include <atomic>
#include "IP4Address.hpp"

class Socket {

protected:
	
	std::atomic<SOCKET> _fd ;
	// If a connection, there address and prot and time connected
	IP4Address _ipaddress ;
	int _port ;
	std::string _time_created ;

	
	
	int create(int port) ;
	void setOptions(SOCKET fd) ;
	void setServer(int port) ;
	
public:
	Socket();
	Socket(int port) ;
	Socket(SOCKET fd, IP4Address& ipaddress, int port);
	~Socket();
	bool valid()  ;
	static void closeSocket(SOCKET fd);  // We do this so other can close without bringing in platform dep
	void close() ;
	SOCKET descriptor()  ;

	std::tuple<SOCKET,IP4Address, std::string>  accept() ;
	std::size_t send(unsigned char *data,std::size_t amount );
	std::size_t receive(unsigned char *data, std::size_t amount);
	
	std::string description() const ;
	
	static void initialize() ;
	static void shutdown() ;

	};

#endif /* Socket_hpp */
