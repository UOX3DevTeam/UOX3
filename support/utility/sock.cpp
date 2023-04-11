//  Copyright © 2023 Charles Kerr. All rights reserved.
//

#include "sock.hpp"
#include <stdexcept>
#include <cstring>
#include <array>
#include <algorithm>

#if !defined(_WIN32)
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/select.h>

#else
#include <WinBase.h>
#include <ws2tcpip.h>
#endif
using namespace std::string_literals;
namespace util {
    namespace net {
        // Declare errormsg in netutil.cpp
        auto errormsg(int error) -> std::string;
        
        
        
        
        //========================================================================
        auto socket_t::create() ->void {
            this->fd = ::socket(static_cast<int>(type),SOCK_STREAM,0);
            if (fd == socket_error){
#if defined(_WIN32)
                auto error = WSAGetLastError();
#else
                auto error = errno;
#endif
                throw std::runtime_error("Unable to create socket: "s+errormsg(error));
            }
            initialize();
            
        }
        
        //========================================================================
        auto  socket_t::initialize() ->void {
#if defined(__linux__)
            sflag = MSG_NOSIGNAL;
#elif !defined(_WIN32)
            auto value = int(1);
            auto status = setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, &value, sizeof(value));
            if (status != 0) {
                throw std::runtime_error("Unable to set socket to nosignal : "s+errormsg(errno));
            }
#endif
            
        }
        //========================================================================
        socket_t::socket_t():sflag(0),rflag(0),fd(invalid_socket),type(ipclass::ip4),blocking(true){
        }
        
        //========================================================================
        socket_t::socket_t(ipclass type):socket_t(){
            this->type = type ;
            create();
        }
        
        //========================================================================
        socket_t::socket_t(ipclass type, descrip_t descriptor):socket_t(){
            this->type = type ;
            fd = descriptor;
            initialize();
        }
        
        //========================================================================
        auto socket_t::descriptor() const ->descrip_t {
            return fd;
        }
        //========================================================================
        // General methods
        //========================================================================
        
        //========================================================================
        auto socket_t::setBlocking(bool value)->void {
            if (!this->valid()){
                throw std::runtime_error("Socket unitialized");
            }
#if defined(_WIN32)
            unsigned long mode = 0;
            if (!value) {
                mode = 1 ;
            }
            auto status = ioctlsocket(fd, FIONBIO, &mode);
            if (status != 0) {
                throw std::runtime_error("Error setting socket block state: " + errormsg(WSAGetLastError()));
            }
#else
            auto current = fcntl(fd,F_GETFL,0) & ~O_NONBLOCK ;  // Clear the nonblock flag
            if (!value) {
                current |= O_NONBLOCK;
            }
            int status = fcntl(fd, F_SETFL,current) ;
            if (status == -1) {
                throw std::runtime_error("Error setting socket block state: " + errormsg(errno));
            }
#endif
            blocking = value;
        }
        
        //========================================================================
        auto  socket_t::close() ->void {
            auto msg = std::string();
            if (valid()){
                
#if !defined(_WIN32)
                auto error = ::close(fd);
                if (error == -1) {
                    msg =errormsg(errno);
                }
#else
                auto error = closesocket(fd);
                if (error !=0) {
                    msg = errormsg( WSAGetLastError());
                }
#endif
                if (!msg.empty()){
                    throw std::runtime_error("Error closing socket: "s + msg);
                }
            }
        }
        
        //========================================================================
        auto socket_t::valid() const ->bool {
            return fd != invalid_socket;
        }
        
        //========================================================================
        auto socket_t::peer() const ->std::pair<std::string,std::string> {
            if (!this->valid()){
                throw std::runtime_error("Socket unitialized");
            }
            auto name = std::string();
            auto port = std::string();
            struct sockaddr_in addr4 ;
            struct sockaddr_in6 addr6 ;
            auto size = 0 ;
            sockaddr * addr = nullptr ;
            auto ip = std::array<char,50>();
            auto service = std::array<char,50>();
            std::fill(ip.begin(),ip.end(),0);
            std::fill(service.begin(),service.end(),0);
            if (type == ipclass::ip4){
                size = sizeof(addr4);
                addr = reinterpret_cast<sockaddr*>(&addr4);
            }
            else if (type == ipclass::ip6){
                size = sizeof(addr6);
                addr = reinterpret_cast<sockaddr*>(&addr6);
            }
            
            auto status = getpeername(fd, reinterpret_cast<sockaddr*>(&addr), reinterpret_cast<socklen_t*>(&size));
            if (status != 0){
#if defined (_WIN32)
                throw std::runtime_error("Unable to retreive peer: "s + errormsg(WSAGetLastError()));
#else
                throw std::runtime_error("Unable to retreive peer: "s + errormsg(errno));
#endif
            }
            status = getnameinfo(reinterpret_cast<const sockaddr*>(&addr),static_cast<socklen_t>(size),ip.data(),static_cast<socklen_t>(ip.size()),service.data(),static_cast<socklen_t>(service.size()),NI_NUMERICSERV|NI_NUMERICHOST);
            if (status != 0 ){
#if defined (_WIN32)
                throw std::runtime_error("Unable to convert peer to string: "s + errormsg(WSAGetLastError()));
#else
                throw std::runtime_error("Unable to convert peer to string: "s + errormsg(errno));
#endif
            }
            name = ip.data();
            port = service.data();
            return std::make_pair(name, port);
        }
        
        
        
        //========================================================================
        auto socket_t::bind(const std::string &port, const std::string &ipaddress,bool reuse)->void {
            if (!this->valid()){
                create();
            }
            if (reuse){
#if defined(_WIN32)
                BOOL value = 1;
                auto status = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&value), sizeof(value));
                if (status != 0) {
                    throw std::runtime_error("Error setting socket reuse: "s + errormsg(WSAGetLastError()));
                }
#else
                auto value = int(1);
                auto status = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));
                if (status != 0) {
                    throw std::runtime_error("Error setting socket reuse: "s + errormsg(errno));
                }
#endif
            }
            
            auto status = int(0);
            struct addrinfo hints;
            struct addrinfo *res;  // will point to the results
            std::memset(&hints,0,sizeof(hints));
            hints.ai_family = AF_UNSPEC;
            if (type == ipclass::ip4){
                hints.ai_family = AF_INET ;
            }
            else if (type == ipclass::ip6){
                hints.ai_family = AF_INET6;
            }
            hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
            hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
            
            status = getaddrinfo(ipaddress.c_str(), port.c_str(), &hints, &res);
            if (status != 0){
#if defined(_WIN32)
                throw std::runtime_error("Error obtaining ip information: "s + errormsg(WSAGetLastError()));
#else
                throw std::runtime_error("Error obtaining ip information: "s + std::string(gai_strerror(status)));
#endif
            }
#if defined(_WIN32)
            status = ::bind(fd, res->ai_addr, static_cast<int>(res->ai_addrlen));
#else
            status = ::bind(fd, res->ai_addr, res->ai_addrlen);
#endif
            if (status != 0){
#if defined(_WIN32)
                auto error = WSAGetLastError();
#else
                auto error = errno;
#endif
                freeaddrinfo(res);
                throw std::runtime_error("Error binding: "s + errormsg(error));
            }
            freeaddrinfo(res);
        }
        
        //========================================================================
        // Server methods
        //========================================================================
        
        //========================================================================
        auto socket_t::listen(int backlog )->void {
            if (!this->valid()){
                throw std::runtime_error("Socket unitialized");
            }
            auto status = ::listen(fd,backlog);
            if (status != 0){
#if defined(_WIN32)
                throw std::runtime_error("Error listening on socket: "s + errormsg(WSAGetLastError()));
#else
                throw std::runtime_error("Error listening on socket: "s + errormsg(errno));
#endif
            }
        }
        
        //========================================================================
        auto socket_t::accept() ->std::optional<socket_t>{
            if (!this->valid()){
                throw std::runtime_error("Socket unitialized");
            }
            struct sockaddr_storage their_addr;
            auto addr_size = sizeof(their_addr);
            auto status = ::accept(fd,reinterpret_cast<struct sockaddr *>(&their_addr),reinterpret_cast<socklen_t*>(&addr_size));
            if (status ==socket_error){
#if defined(_WIN32)
                auto error = WSAGetLastError();
                if (error != WSAEWOULDBLOCK){
                    throw std::runtime_error("Error listening on socket: "s + errormsg(error));
                }
#else
                auto error = errno;
                if (error != EWOULDBLOCK){
                    throw std::runtime_error("Error listening on socket: "s + errormsg(error));
                }
#endif
                
                return {};
            }
            
            return socket_t(type,status);
        }
        //========================================================================
        // Client methods
        //========================================================================
        
        
        //========================================================================
        auto socket_t::connect(const std::string &ipaddress, const std::string &port,int sec_timeout) ->bool {
            
            if (!this->valid()){
                this->create();
            }
#if defined(_WIN32)
            auto blockerror = WSAEWOULDBLOCK;
#else
            auto blockerror = EINPROGRESS;
#endif
            auto status = int(0);
            struct addrinfo hints;
            struct addrinfo *res;  // will point to the results
            std::memset(&hints,0,sizeof(hints));
            hints.ai_family = AF_INET;       // IP4
            hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
            hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
            if ((status = getaddrinfo(ipaddress.c_str(), port.c_str(), &hints, &res)) != 0) {
#if defined(_WIN32)
                std::string msg = errormsg(WSAGetLastError());
#else
                
                std::string msg = gai_strerror(status);
#endif
                throw std::runtime_error("Error obtaining ip information: "s + msg);
            }
            
            auto current_state = blocking;
            setBlocking(false);// We are going to use select with a timeout
            timeval delay;
            delay.tv_sec=sec_timeout;
            delay.tv_usec = 0 ;
            fd_set fdset;
            FD_ZERO(&fdset);
            FD_SET(fd,&fdset);
#if defined(_WIN32)
            status = ::connect(fd,res->ai_addr,static_cast<socklen_t>(res->ai_addrlen));
#else 
            status = ::connect(fd, res->ai_addr, res->ai_addrlen);
#endif
            if (status ==socket_error){
#if defined(_WIN32)
                auto failure = WSAGetLastError();
#else
                auto failure =errno;
#endif
                if (failure == blockerror){
                    // Ok, it might be ok still
                    status = ::select(static_cast<int>(fd+1), &fdset,nullptr, nullptr, &delay);
                    if ((status == 0) || (status == socket_error)){
                        try {
                            // Ok, the socket timed out.  We close the socket
                            this->close();
                        }
                        catch(...){
                            
                        }
                        freeaddrinfo(res);
                        return false ;
                    }
                }
                else {
                    // Real error
#if defined(_WIN32)
                    auto error = WSAGetLastError();
#else
                    auto error = errno;
#endif
                    freeaddrinfo(res);
                    throw std::runtime_error("Connection error: "s+errormsg(error));
                }
            }
            
            freeaddrinfo(res);
            setBlocking(current_state);// set the state back
            return true ;
        }
        
        //========================================================================
        auto socket_t::read(std::uint8_t *data,amount_t amount, bool peek) -> amount_t {
            if (!this->valid()){
                throw std::runtime_error("Socket unitialized");
            }
            auto status = amount_t(0);
            auto flag = rflag;
            if (peek) {
                flag |= MSG_PEEK;
            }
            if (amount > 0) {
                auto status = recv(fd, reinterpret_cast<char*>(data), amount, flag);
                if (status == socket_error) {
#if defined(_WIN32)
                    auto msg = errormsg(WSAGetLastError());
#else
                    auto msg = errormsg(errno);
#endif
                    throw std::runtime_error("Error on socket write: "s + msg);
                }
            }
            return status ;
        }
        //========================================================================
        auto socket_t::write(const std::uint8_t* data, amount_t amount) -> amount_t {
            if (!this->valid()){
                throw std::runtime_error("Socket unitialized");
            }
            
            auto status = amount_t(0);
            if (amount > 0) {
                auto status = send(fd, reinterpret_cast<const char*>(data), amount, sflag);
                if (status == socket_error) {
#if defined(_WIN32)
                    auto msg = errormsg(WSAGetLastError());
#else
                    auto msg = errormsg(errno);
#endif
                    throw std::runtime_error("Error on socket write: "s + msg);
                }
            }
            return status;
        }
    }
}
