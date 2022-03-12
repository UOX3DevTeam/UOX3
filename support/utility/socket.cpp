//Copyright © 2022 Charles Kerr. All rights reserved.

#include "socket.hpp"

#include <iostream>
#include <algorithm>
#include <cstdio>
#include <stdexcept>
#include <cstring>

#if !defined(_WIN32)
// Unix specific
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <errno.h>
#else
// Windows specific
#include <stringapiset.h>
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")
#endif

#include "strutil.hpp"
#include "netexcept.hpp"
using namespace std::string_literals;


//=========================================================
// socket_t
//=========================================================
//===================================================================
socket_t::socket_t(descriptor_t descriptor ,bool blocking):_descriptor(descriptor),_blocking(blocking),_read_flag(0),_write_flag(0){
	_create_time = std::chrono::system_clock::now();
	if (_descriptor != invalid_socket){
		setOptions() ;
		setBlocking(_blocking) ;
	}
}
//===================================================================
socket_t::~socket_t() {
	// we close our descriptor on desctruction
	if (_descriptor != invalid_socket){
#if defined(_WIN32)
		closesocket(_descriptor);
#else
		shutdown(_descriptor,SHUT_RDWR);
#endif
	}
}
//===================================================================
// There is really nothing to set up for windows.  For linux we
// we set the write flag to not SIGPIPE if writing to
// a closed socket. For APPLE,
// set it not sigpipe if sending to a closed socket
auto socket_t::setOptions()->void{
	unsigned long one = 1 ;
#if defined(__liniux__)
	
	_write_flag = MSG_NOSIGNAL
#elif defined(__APPLE__)
	setsockopt(_descriptor, SOL_SOCKET, SO_NOSIGPIPE, &one, sizeof(one));
	
#endif
}
//===================================================================
auto socket_t::setBlocking(bool value)->void{
	unsigned long one = 1 ;
	if (value){
		one = 0 ;
	}
#if defined(_WIN32)
	ioctlsocket( _descriptor, FIONBIO, &one );
#elif defined(__linux__)
	if (!value) {
		_write_flag |= MSG_DONTWAIT ;
	}
	else {
		_write_flag &= (~MSG_DONTWAIT);
	}
#else
	ioctl( _descriptor, FIONBIO, &one );
	
#endif
}
//===================================================================
auto socket_t::blocking() const ->bool {
	return _blocking ;
}
//===================================================================
auto socket_t::created() const ->std::chrono::time_point<std::chrono::system_clock> {
	return _create_time ;
}

//===================================================================
auto socket_t::peer() const ->std::pair<ip4addr_t,int> {
	struct sockaddr_in peer ;
	socklen_t peersize = sizeof(sockaddr_in);
	auto peerstatus = getpeername(_descriptor, reinterpret_cast<struct sockaddr*>(&peer), &peersize);
	if (peerstatus != 0) {
#if defined(_WIN32)
		// Windows way of doing it
		auto errornum = WSAGetLastError();
		WCHAR buffer[300];
		char sbuff[300];
		char DefChar = ' ';
		auto size = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, errornum, 0, buffer, 300, nullptr);
		WideCharToMultiByte(CP_ACP, 0, buffer, -1, sbuff, 300, &DefChar, NULL);
		auto message = std::string(sbuff);
		
		throw socket_exp(errornum, message);
		
#else
		auto errornum = errno ;
		auto msg = std::string(strerror(errornum));
		throw socket_exp(errornum, msg);
#endif
	}
	// Swap the bytes , it is in network byte order (big endian)
	auto port = static_cast<int>(((peer.sin_port&0xFF00)>>8) | ((peer.sin_port&0xFF)<<8));
	auto ip = ip4addr_t(peer.sin_addr.s_addr,true) ;
	return std::make_pair(ip, port);
	
}


//===================================================================
auto  socket_t::write(const unsigned char *data,int amount)->int {
	auto status = send(_descriptor, reinterpret_cast<const char*>(data),amount,_write_flag) ;
	if (status == socket_error){
#if defined(_WIN32)
		
		auto errornum = WSAGetLastError() ;
		if (errornum == WSAEWOULDBLOCK){
			// The call would block
			// so we assume we sent nothing
			status = 0 ;
			
		}
		else {
			WCHAR buffer[300];
			char sbuff[300];
			char DefChar = ' ';
			auto size = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, errornum, 0, buffer, 300,nullptr);
			WideCharToMultiByte(CP_ACP, 0, buffer, -1, sbuff, 300, &DefChar, NULL);
			
			auto message = std::string(sbuff);
			
			throw socket_exp(errornum,message) ;
			
		}
#else
		// We had some error
		if (errno == EAGAIN) {
			// The call would block
			// so we assume we sent nothing
			status = 0 ;
		}
		else {
			auto msg = std::vector<char>(200,0);
			strerror_r(errno, msg.data(), msg.size());
			auto loc = std::find(msg.begin(),msg.end(),0);
			auto message = std::string(msg.begin(),loc);
			throw socket_exp(errno,message) ;
		}
#endif
	}
	return static_cast<int>(status) ;
}

//===================================================================
auto socket_t::read(unsigned char *data,int amount)->int {
	auto status = recv(_descriptor,reinterpret_cast<char*>(data),static_cast<size_t>(amount),_read_flag);
	if (status == socket_error){
#if defined(_WIN32)
		auto errornum = WSAGetLastError() ;
		if (errornum == WSAEWOULDBLOCK){
			// The call would block
			// so we assume we sent nothing
			status = 0 ;
			
		}
		else {
			WCHAR buffer[300];
			char sbuff[300];
			char DefChar = ' ';
			auto size = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, errornum, 0, buffer, 300, nullptr);
			WideCharToMultiByte(CP_ACP, 0, buffer, -1, sbuff, 300, &DefChar, NULL);
			
			auto message = std::string(sbuff);
			
			throw socket_exp(errornum, message);
			
		}
		
		
#else
		if (errno == EAGAIN){
			status = 0 ;
		}
		else {
			auto msg = std::vector<char>(200,0);
			strerror_r(errno, msg.data(), msg.size());
			auto loc = std::find(msg.begin(),msg.end(),0);
			auto message = std::string(msg.begin(),loc);
			throw socket_exp(errno,message) ;
			
		}
#endif
	}
	else if (status == 0){
#if defined(_WIN32)
		auto errornum = WSAENOTCONN;
		WCHAR buffer[300];
		char sbuff[300];
		char DefChar = ' ';
		auto size = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, errornum, 0, buffer, 300, nullptr);
		WideCharToMultiByte(CP_ACP, 0, buffer, -1, sbuff, 300, &DefChar, NULL);
		
		auto message = std::string(sbuff);
		
		throw socket_exp(errornum, message);
		
		
#else
		// So the peer has closed its half side, report as if a connection reset
		auto msg = std::vector<char>(200,0);
		strerror_r(ECONNRESET, msg.data(), msg.size());
		auto loc = std::find(msg.begin(),msg.end(),0);
		auto message = std::string(msg.begin(),loc);
		throw socket_exp(ECONNRESET,message) ;
#endif
		
	}
	return static_cast<int>(status) ;
}

//==============================================================================
auto socket_t::connect(const std::string &ip,const std::string &port,bool blocking  )->std::unique_ptr<socket_t> {
	struct addrinfo hints ;
	struct addrinfo *servinfo =nullptr ;
	struct addrinfo *p  = nullptr ;
	std::unique_ptr<socket_t> rvalue = nullptr;
	// clear out hints structure
	std::fill(reinterpret_cast<char*>(&hints),reinterpret_cast<char*>(&hints)+sizeof(hints),0);
	hints.ai_family = AF_INET ;
	hints.ai_socktype = SOCK_STREAM ;
	hints.ai_protocol = IPPROTO_TCP ;
	auto status = getaddrinfo(ip.c_str(), port.c_str(), &hints, &servinfo);
	if (status != 0){
#if !defined(_WIN32)
		// We have an error
		auto message = gai_strerror(status);
#else
		auto msg = gai_strerror(status);
		char buffer[500];
		char DefChar = ' ';
		WideCharToMultiByte(CP_ACP, 0, msg, -1, buffer, 500, &DefChar,nullptr);
		auto message = std::string(buffer);
#endif
		// What should we do with it?
		throw socket_exp(status,message) ;
	}
	// So now we want to connect
	for (p = servinfo; p != nullptr ;p=p->ai_next) {
		auto sockfd = socket(p->ai_family,p->ai_socktype,p->ai_protocol);
		if (sockfd != invalid_socket) {
			// We got a good one!
			auto constatus = ::connect(sockfd, p->ai_addr, static_cast<int>(p->ai_addrlen));
			if (constatus == -1){
				// It was bad!
#if defined(_WIN32)
				closesocket(sockfd);
#else
				shutdown(sockfd,SHUT_RDWR);
#endif
				continue ;  // We couldn't connect, see if we can connect on another entry
				
			}
			rvalue = std::make_unique<socket_t>(sockfd,blocking);
			break;
		}
	}
	freeaddrinfo(servinfo);
	// Instead of exception  we return nullptr
	/*
	 if (p==nullptr){
	 // We didn't connect
	 throw socket_close_create(0,"Unable to connect socket"s);
	 }
	 */
	// Now, we need to set the connected
	return rvalue;
}

//==============================================================================
auto socket_t::listen(const std::string &ip,const std::string &port,bool blocking  )->std::unique_ptr<socket_t> {
	if (ip.empty()){
		// Then it is quick and easy, we want to bind to everything!
		struct sockaddr_in temp ;
		std::fill(reinterpret_cast<char*>(&temp),reinterpret_cast<char*>(&temp)+sizeof(sockaddr_in),0);
		temp.sin_addr.s_addr = INADDR_ANY ;
		temp.sin_family = AF_INET ;
		
		auto portnum = static_cast<unsigned short>(std::stoul(port,nullptr,0));
		// Now we get to swap it to big endian
		portnum = ((portnum&0xFF00)>>8) | ((portnum&0xff)<<8);
		temp.sin_port = portnum ;
		auto sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		if (sockfd != invalid_socket) {
			// We got a good one!
			// Do we want to reuse the addr
			auto one = 1 ;
			setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>( & one), static_cast<int>(sizeof(one)));
			auto bindstatus = ::bind(sockfd, reinterpret_cast<sockaddr*>(&temp), static_cast<int>(sizeof(sockaddr_in)));
			if (bindstatus == -1){
				// It was bad!
#if defined(_WIN32)
				closesocket(sockfd);
#else
				shutdown(sockfd,SHUT_RDWR);
#endif
				return nullptr ;
				
			}
			::listen(sockfd,42);
			return std::make_unique<socket_t>(sockfd,blocking);
		}
		return nullptr ;
		
	}
	// If we get to here, we are binding on a specific device!
	struct addrinfo hints ;
	struct addrinfo *servinfo =nullptr ;
	struct addrinfo *p  = nullptr ;
	std::unique_ptr<socket_t> rvalue = nullptr;
	// clear out hints structure
	std::fill(reinterpret_cast<char*>(&hints),reinterpret_cast<char*>(&hints)+sizeof(hints),0);
	hints.ai_family = AF_INET ;
	hints.ai_socktype = SOCK_STREAM ;
	hints.ai_protocol = IPPROTO_TCP ;
	auto status = getaddrinfo(ip.c_str(), port.c_str(), &hints, &servinfo);
	if (status != 0){
#if !defined(_WIN32)
		// We have an error
		auto message = gai_strerror(status);
#else
		auto msg = gai_strerror(status);
		char buffer[500];
		char DefChar = ' ';
		WideCharToMultiByte(CP_ACP, 0, msg, -1, buffer, 500, &DefChar, nullptr);
		auto message = std::string(buffer);
#endif		// What should we do with it?
		throw socket_exp(status,message) ;
	}
	// So now we want to connect
	for (p = servinfo; p != nullptr ;p=p->ai_next) {
		auto sockfd = socket(p->ai_family,p->ai_socktype,p->ai_protocol);
		if (sockfd != invalid_socket) {
			// We got a good one!
			// Do we want to reuse the addr
			auto one = 1 ;
			setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>( & one), static_cast<int>(sizeof(one)));
			auto bindstatus = ::bind(sockfd, p->ai_addr, static_cast<int>(p->ai_addrlen));
			if (bindstatus == -1){
				// It was bad!
#if defined(_WIN32)
				closesocket(sockfd);
#else
				shutdown(sockfd,SHUT_RDWR);
#endif
				continue ;  // We couldn't connect, see if we can connect on another entry
				
			}
			::listen(sockfd,42);
			rvalue = std::make_unique<socket_t>(sockfd,blocking);
			break;
		}
	}
	freeaddrinfo(servinfo);
	// We couldn't bind
	// Instead of exception, we return a nullptr
	/*
	 if (p==nullptr){
	 throw socket_close_create(0,"Unable to bind socket"s);
	 }
	 */
	return rvalue ;
	
}
//==============================================================================
auto socket_t::accept(bool peer_blocking ) ->std::unique_ptr<socket_t> {
	struct sockaddr_in sockstruct ;
	socklen_t socksize = sizeof(sockaddr_in);
	auto sockfd = ::accept(_descriptor,reinterpret_cast<struct sockaddr *>(&sockstruct),&socksize) ;
	if (sockfd != socket_error){
		// We have a good socket!
		return std::make_unique<socket_t>(sockfd,peer_blocking) ;
	}
	// It did not, so, lets see why?
#if !defined(_WIN32)
	auto errornum = errno ;
#else
	auto errornum = WSAGetLastError();
#endif
	// Basically, if it is because we would block, not a real error, and return nullptr
	if (errornum == EWOULDBLOCK){
		return nullptr ;
	}
	// ANything else is an error!
#if !defined(_WIN32)
	// We have an error
	auto message = strerror(errornum);
#else
	WCHAR buffer[300];
	char sbuff[300];
	char DefChar = ' ';
	auto size = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, errornum, 0, buffer, 300, nullptr);
	WideCharToMultiByte(CP_ACP, 0, buffer, -1, sbuff, 300, &DefChar, NULL);
	
	auto message = std::string(sbuff);
#endif
	
	throw socket_exp(errornum, message);
	
}

