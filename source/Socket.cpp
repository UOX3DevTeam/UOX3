// Copyright © 2021 Charles Kerr. All rights reserved.
// Created on:  5/25/21

#include "Socket.hpp"

#include <iostream>
#include <stdexcept>
#include <cstring>
#include <sstream>

#if (PLATFORM == MACOS) || (PLATFORM == LINUX)

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#endif

#include "TimeUtility.hpp"
//+++++++++++++++++++++++++++++++++++++++++++++++++++
//
//		Methods for Socket
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++


//============================================================================
Socket::Socket(){
	_fd = INVALID_SOCKET ;
	
	_time_created = timeutil::timenow() ;
	
}
//============================================================================
Socket::Socket(int port) : Socket() {
	setServer(port);
}
//===========================================================================
Socket::~Socket(){
	close();
}
//==============================================================================
void Socket::setServer(int port){
	struct addrinfo hints, *res;
	
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;  // use IPv4 or IPv6, whichever
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
	
	auto status = getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &res);
	if (status != 0) {
#if PLATFORM == WINDOWS
		auto error = WSAGetLastError() ;
		throw std::runtime_error(std::string("Unable to get socket address info: ") + std::to_string(error));

#else
		throw std::runtime_error(std::string("Unable to get socket address info: ") + std::string(gai_strerror(status)));

#endif
	}
	_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (_fd == INVALID_SOCKET) {
		freeaddrinfo(res);
#if PLATFORM == WINDOWS
		auto error = WSAGetLastError() ;
		throw std::runtime_error(std::string("Unable to create socket on port ")+std::to_string(port)+std::string(" - ")+std::to_string(error));
		
#else
		auto error = errno ;
		throw std::runtime_error(std::string("Unable to create socket on port ")+std::to_string(port)+std::string(" - ")+std::string(std::strerror(error)));
#endif
	}
	SOCKET fd = _fd ;

	setOptions(fd);
	auto bstatus = bind(static_cast<int>(fd), res->ai_addr, res->ai_addrlen);
	freeaddrinfo(res);
	if (bstatus == SOCKET_ERROR){
		close();
		
#if PLATFORM == WINDOWS
		auto error = WSAGetLastError() ;
		throw std::runtime_error(std::string("Unable to bind socket on port ")+std::to_string(port)+std::string(" - ")+std::to_string(error));
#else
		auto error = errno ;
		throw std::runtime_error(std::string("Unable to bind socket on port ")+std::to_string(port)+std::string(" - ")+std::string(std::strerror(error)));
		
#endif
	}
	

	listen(fd, 42);
}
//============================================================================
Socket::Socket(SOCKET fd, IP4Address &ipaddress, int port) : Socket() {
	_fd = fd ;
	_ipaddress = ipaddress ;
	_port = port ;
	

	setOptions(fd);

	
}
//============================================================================
void Socket::initialize() {
#if PLATFORM == WINDOWS
	int iResult;
	WSADATA wsaData ;
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		throw std::runtime_error(std::string("Unable to start winsock: ")+std::to_string(iResult));
	}
#elif PLATFORM == LINUX
	signal(SIGPIPE, SIG_IGN);		// We dont do this on macos,as we can set a socket option
#endif
}
//============================================================================
void Socket::setOptions(SOCKET fd) {
	unsigned long one = 1 ;
	
#if PLATFORM == MACOS
	[[maybe_unused]] int retVal = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
	[[maybe_unused]] int retVal = setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, &one, sizeof(one));
	ioctl( fd, FIONBIO, &one );

#elif PLATFORM == LINUX
	[[maybe_unused]] int retVal = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
	ioctl( fd, FIONBIO, &one );

#elif PLATFORM == WINDOWS
	[[maybe_unused]] int retVal = ioctlsocket( fd, FIONBIO, &one );
#endif
}
//============================================================================
std::tuple<SOCKET,IP4Address, std::string>  Socket::accept() {
	struct sockaddr_storage their_addr;
	socklen_t addr_size = sizeof(their_addr) ;
	
	SOCKET fd = _fd ;

	auto new_fd = ::accept(fd, reinterpret_cast<sockaddr*>(&their_addr), &addr_size);
	if (new_fd == INVALID_SOCKET) {
#if PLATFORM == WINDOWS
		auto error = WSAGetLastError() ;
		if (error == WSAEWOULDBLOCK) {
			return std::make_tuple(INVALID_SOCKET,std::string(),std::string());
		}
		throw std::runtime_error(std::string("Error accepting sockets: ")+std::to_string(error));
#else
		auto error = errno ;
		if ((error == EAGAIN) || (error == EWOULDBLOCK)) {
			return std::make_tuple(INVALID_SOCKET,std::string(),std::string());
		}
		throw std::runtime_error(std::string("Error accepting sockets: ")+std::string(strerror(error)));
		
#endif
	}
	struct sockaddr_in *sin = reinterpret_cast<sockaddr_in *>(&their_addr) ;
	IP4Address ipaddress(ntohl(sin->sin_addr.s_addr));

	
	auto port = std::to_string(ntohs(sin->sin_port)) ;
	return std::make_tuple(new_fd,ipaddress,port) ;
}
//============================================================================
void Socket::closeSocket(SOCKET fd){
#if PLATFORM == WINDOWS
	closesocket(fd) ;
	
#else
	::close(fd) ;
	
#endif
}
//============================================================================
void Socket::close(){
	SOCKET fd = _fd ;

	if (valid()) {
#if PLATFORM == WINDOWS
		closesocket(fd) ;
		
#else
		::close(fd) ;
		
#endif
	}
	
	_fd = INVALID_SOCKET ;

}

//============================================================================
SOCKET Socket::descriptor()  {
	
	return _fd ;
}

//============================================================================
bool Socket::valid()  {
	
	return _fd != INVALID_SOCKET ;
}

//============================================================================
std::string Socket::description() const {
	std::stringstream output ;
	output << _time_created << " : " << _ipaddress.string() << " : " << std::to_string(_port) ;
	return output.str();
}

//============================================================================
std::size_t Socket::send(unsigned char *data,std::size_t amount ) {
	int flag = 0 ;
	
	
#if PLATFORM == LINUX
	ssize_t status = 0 ;
	flag = MSG_NOSIGNAL | MSG_DONTWAIT ;
#elif PLATFORM == MACOS
	ssize_t status = 0 ;
#else
	int status = 0 ;
#endif
#if PLATFORM != WINDOWS
	
	{
		
		status = ::send(_fd,reinterpret_cast<char*>(data),amount,flag);

	}
	if (status == SOCKET_ERROR) {
		auto error = errno ;
		if ((error == EAGAIN) || (error == EWOULDBLOCK)){
			return 0 ;
		}
		close() ;
		return 0 ;
	}
#else
	{
		status = ::send(_fd,reinterpret_cast<char*>(data),static_cast<int>(amount),flag);
	}

	if (status == SOCKET_ERROR) {
		auto error = WSAGetLastError() ;
		if (error == WSAEWOULDBLOCK ){
			return 0;
		}
		close() ;
		return 0 ;
	}
	
#endif
	return static_cast<std::size_t>(status) ;

}
//============================================================================
std::size_t Socket::receive(unsigned char *data, std::size_t amount){
	int flag = 0 ;
#if PLATFORM == LINUX
	ssize_t status = 0 ;
	flag =  MSG_DONTWAIT ;
#elif PLATFORM == MACOS
	ssize_t status = 0 ;
#else
	int status = 0 ;
#endif
#if PLATFORM != WINDOWS
	
	{
		SOCKET fd = _fd ;
		status = ::recv(fd,reinterpret_cast<char*>(data),amount,flag);
		
	}
	if (status == SOCKET_ERROR) {
		auto error = errno ;
		if ((error == EAGAIN) || (error == EWOULDBLOCK)){
			return 0 ;
		}
		close() ;
		return 0 ;
	}
#else
	{
		SOCKET fd = _fd ;
		status = ::recv(fd,reinterpret_cast<char*>(data),static_cast<int>(amount),flag);
	}
	
	if (status == SOCKET_ERROR) {
		auto error = WSAGetLastError() ;
		if (error == WSAEWOULDBLOCK ){
			return 0;
		}
		close() ;
		return 0 ;
	}
	
#endif
	return static_cast<std::size_t>(status) ;

}

//==============================================================================
void Socket::shutdown() {
	// Really just for windows, to shutdown WinSock2
#if PLATFORM == WINDOWS
	auto status =  WSACleanup();
	if (status != 0) {
		status = WSAGetLastError();
		std::string message ;
		switch (status) {
			case WSANOTINITIALISED:
				message = "A successful WSAStartup call must occur before using this function";
				break;
			case WSAENETDOWN:
				message = "The network subsystem has failed.";
				break;
			case WSAEINPROGRESS:
				message = "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.";
				break ;
			default:
				message = std::string("Unknown error: ") + std::to_string(status) ;
				break;
		}
		throw std::runtime_error(message);
	}
#endif
}
