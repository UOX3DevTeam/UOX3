//Copyright © 2023 Charles Kerr. All rights reserved.

#include "netsocket.hpp"

#include <algorithm>
#include <stdexcept>
#include <chrono>
#include <cstring>

#if !defined(_WIN32)
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#endif

#include "netutil.hpp"
#include "neterror.hpp"

using namespace std::string_literals ;
//======================================================================
namespace util {
    namespace net {
        //======================================================================
        auto NetSocket::setOption(sockfd_t descriptor) -> void {
#if defined(__APPLE__)
            auto optionvalue = 1 ;
            socklen_t size = sizeof(optionvalue);
            if (setsockopt(descriptor, SOL_SOCKET, SO_NOSIGPIPE, static_cast<void*>(&optionvalue), size) == -1) {
                // This failed!
                auto error = errno ;
                throw std::runtime_error("Error setting socket to not sigpipe: "s + errormsg(error));
            }
#endif
        }
        
        //======================================================================
        NetSocket::NetSocket() :descriptor(BADSOCKET),block(false) {
#if !defined(_WIN32) && !defined(__APPLE__)
            // We dont just check for linux, because these flags are valid on BSD as well
            rcvFlag = 0 ;
            sendFlag =  MSG_NOSIGNAL ;
#else
            rcvFlag = 0 ;
            sendFlag = 0 ;
#endif
        }
        
        //======================================================================
        NetSocket::~NetSocket(){
            if (this->valid()){
                this->close();
            }
        }
        //======================================================================
        NetSocket::NetSocket(sockfd_t descriptor,bool blocking) : NetSocket() {
            this->descriptor = descriptor;
            setOption(this->descriptor);
            setBlocking(blocking) ;
        }
        
        //======================================================================
        NetSocket::NetSocket(NetSocket&& value) {
            this->descriptor = value.descriptor;
            value.descriptor = BADSOCKET;
            this->rcvFlag = value.rcvFlag ;
            this->sendFlag = value.sendFlag ;
            this->block = value.block ;
#if !defined(_WIN32) && !defined(__APPLE__)
            // We dont just check for linux, because these flags are valid on BSD as well
            value.rcvFlag = 0 ;
            value.sendFlag =  MSG_NOSIGNAL ;
#else
            value.rcvFlag = 0 ;
            value.sendFlag = 0 ;
#endif
            value.block = true ;
        }
        
        //======================================================================
        auto NetSocket::operator=(NetSocket&& value)->NetSocket& {
            this->descriptor = value.descriptor;
            value.descriptor = BADSOCKET;
            this->rcvFlag = value.rcvFlag ;
            this->sendFlag = value.sendFlag ;
            this->block = value.block ;
#if !defined(_WIN32) && !defined(__APPLE__)
            // We dont just check for linux, because these flags are valid on BSD as well
            value.rcvFlag = 0 ;
            value.sendFlag =  MSG_NOSIGNAL ;
#else
            value.rcvFlag = 0 ;
            value.sendFlag = 0 ;
#endif
            value.block = true ;
            return *this;
        }
        
        //======================================================================
        auto NetSocket::create(bool blocking) ->void {
            if (this->valid()){
                this->close();
            }
            this->descriptor = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
            if (this->descriptor == BADSOCKET){
                auto error = 0 ;
#if defined(_WIN32)
                error = WSAGetLastError() ;
#else
                error = errno ;
#endif
                throw SocketError(errormsg(error));
            }
            setOption(this->descriptor);
            setBlocking(blocking);
        }
        
        //======================================================================
        auto NetSocket::valid() const ->bool {
            return this->descriptor != BADSOCKET;
        }
        
        //======================================================================
        auto NetSocket::close() -> void {
            if (this->descriptor != BADSOCKET){
                closeSocket(this->descriptor);
                this->descriptor = BADSOCKET;
            }
        }
        
        //======================================================================
        auto NetSocket::send(const char* data, iosize_t size) const->status_t {
            if (descriptor == BADSOCKET){
                throw SocketClose("Attempt to write a closed socket");
            }
            auto status = ::send(descriptor,data,size,sendFlag) ;
            if (status == SOCKETERROR){
                auto error = 0 ;
#if defined(_WIN32)
                error = WSAGetLastError();
                if (error == WSAEWOULDBLOCK || (block && error==WSAETIMEDOUT)){
                    return static_cast<status_t>(0);
                }
                if (error = WSAECONNRESET || error == WSAENOTCONN){
                    throw std::SocketPeerClose(errormsg(error));
                }
#else
                error = errno ;
                if (error == EPIPE || error == ECONNRESET) {
                    throw SocketPeerClose(errormsg(error));
                }
                if (error == EAGAIN || error == EWOULDBLOCK) {
                    return static_cast<status_t>(0);
                }
                
#endif
                throw SocketError("Error writing socket: "s + errormsg(error));
            }

            return status;
        }
        
        //======================================================================
        auto NetSocket::read(char* data, iosize_t size) const->status_t {
            if (descriptor == BADSOCKET){
                throw SocketClose("Attempt to read a closed socket");
            }
            
            auto status = recv(descriptor, data, size,rcvFlag );
            if (status == SOCKETERROR) {
                auto error = 0;
#if defined(_WIN32)
                error = WSAGetLastError();
                // Should we worry about WSAETIMEDOUT as well, yes if blocking
                if (error == WSAEWOULDBLOCK || (this->block && error == WSAETIMEDOUT)) {
                    return static_cast<status_t>(0);
                }
                if (error == WSAECONNRESET || error == WSAENOTCONN) {
                    throw SocketPeerClose(errormsg(error));
                }
#else
                error = errno;
                if (error == EAGAIN || error == EWOULDBLOCK ) {
                    return static_cast<status_t>(0);
                }
                if (error == EPIPE || error == ENOTCONN){
                    throw SocketPeerClose(errormsg(error));
                }
#endif
                throw SocketError("Error reading socket: "s + errormsg(error));
            }
            return status;
        }
        
        //======================================================================
        auto NetSocket::readRetry(char* data, iosize_t size,int microseconds) const -> status_t {
            auto totalProcessed = static_cast<iosize_t>(0);
            auto now = std::chrono::high_resolution_clock::now() ;
            do {
                auto status = read(data+totalProcessed, size-totalProcessed) ;
                totalProcessed += status ;
            }while (totalProcessed != size && (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()-now).count()<microseconds));
            return totalProcessed;
        }
        //======================================================================
        auto NetSocket::sendRetry(const char* data, iosize_t size,int microseconds) const -> status_t {
            auto totalProcessed = static_cast<iosize_t>(0);
            auto now = std::chrono::high_resolution_clock::now() ;
            do {
                auto status = send(data+totalProcessed, size-totalProcessed) ;
                totalProcessed += status ;
            }while (totalProcessed != size && (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()-now).count()<microseconds));
            return totalProcessed;
        }
        //======================================================================
        auto NetSocket::setBlocking(bool state) ->void {
            this->block = state ;
#if !defined(__APPLE__) && !defined(_WIN32)
            // We are doing BSD/LINUX
            if (state) {
                rcvFlag &= ~MSG_DONTWAIT ;
                sendFlag &= ~MSG_DONTWAIT ;
            }
            else {
                rcvFlag |= MSG_DONTWAIT ;
                sendFlag |= MSG_DONTWAIT ;
            }
#elif defined(__APPLE__)
            auto flags = fcntl(descriptor,F_GETFL,0);
            flags = state ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK) ;
            fcntl(descriptor, F_SETFL, flags);
#else
            // doing windows
            unsigned long mode = state ? 0 : 1;
            ioctlsocket(descriptor, FIONBIO, &mode);
#endif
        }
        
        //======================================================================
        auto NetSocket::bind(const std::string &ipaddress, const std::string &port,bool blocking) ->void{
            if (this->valid()){
                this->close();
            }
            struct addrinfo hints;
            struct addrinfo *res = nullptr;
            std::memset(&hints,0,sizeof hints) ;
            hints.ai_family = AF_INET ;
            hints.ai_socktype = SOCK_STREAM ;
            hints.ai_flags = AI_PASSIVE ;
            if (ipaddress.empty()){
                getaddrinfo(NULL, port.c_str(), &hints, &res);
            }
            else {
                getaddrinfo(ipaddress.c_str(), port.c_str(), &hints, &res);
            }
            this->descriptor = socket(res->ai_family,res->ai_socktype,res->ai_protocol) ;
            if (this->descriptor == BADSOCKET) {
                auto error = 0 ;
#if defined(_WIN32)
                error = WSAGetLastError() ;
#else
                error = errno ;
#endif
                throw SocketError("Unable to create socket: "s + errormsg(error));
            }
            setOption(this->descriptor) ;
            // We are going to reuse the addr
#if defined(_WIN32)
            auto optionvalue = 1 ;
            auto size = sizeof optionvalue ;
            setsockopt(this->descriptor, SOL_SOCKET, SO_REUSEADDR, static_cast<const char*>(&optionvalue), size);
            if (setsockopt(descriptor, SOL_SOCKET, SO_REUSEADDR, static_cast<void*>(&optionvalue), size) == SOCKETERROR) {
                // This failed!
                auto error = WSAGetLastError() ;
                freeaddrinfo(res);
                throw std::runtime_error("Error setting socket to reuseaddr: "s + errormsg(error));
            }
#else
            auto optionvalue = 1 ;
            socklen_t size = sizeof(optionvalue);
            if (setsockopt(descriptor, SOL_SOCKET, SO_REUSEADDR, static_cast<void*>(&optionvalue), size) == SOCKETERROR) {
                // This failed!
                auto error = errno ;
                freeaddrinfo(res);
                throw std::runtime_error("Error setting socket to reuseaddr: "s + errormsg(error));
            }
#endif
            auto status = ::bind(this->descriptor,res->ai_addr,res->ai_addrlen);
            if (status == SOCKETERROR){
                
                auto error = 0 ;
#if defined(_WIN32)
                error = WSAGetLastError() ;
#else
                error = errno ;
#endif
                freeaddrinfo(res);
                throw SocketError("Unable to bind socket: "s + errormsg(error));

            }
            freeaddrinfo(res);
            setBlocking(blocking);
        }
        //======================================================================
        auto NetSocket::listen(const std::string &ipaddress, const std::string &port, bool blocking) ->void {
            this->bind(ipaddress, port, blocking);
            ::listen(this->descriptor, 20);
        }
        //======================================================================
        auto NetSocket::connect(const std::string &ipaddress, const std::string &port) ->bool {
            if (!this->valid()){
                this->create(true);
            }
            struct addrinfo hints;
            struct addrinfo *res = nullptr;
            std::memset(&hints,0,sizeof hints) ;
            hints.ai_family = AF_INET ;
            hints.ai_socktype = SOCK_STREAM ;
            hints.ai_flags = AI_PASSIVE ;
            auto rvalue = true ;
            getaddrinfo(ipaddress.c_str(), port.c_str(), &hints, &res);
            auto status = ::connect(this->descriptor, res->ai_addr, res->ai_addrlen);
            if (status == SOCKETERROR){
                rvalue = false ;
            }
            freeaddrinfo(res);
            return rvalue ;
        }
        //======================================================================
        auto NetSocket::accept() const -> std::optional<sockfd_t> {
            struct sockaddr_storage their_addr;
            socklen_t size = sizeof their_addr ;
            
            auto status = ::accept(this->descriptor, reinterpret_cast<struct sockaddr *>(&their_addr), &size);
            if (status == SOCKETERROR){
                auto error = 0 ;
#if defined(_WIN32)
                error = WSAGetLastError() ;
                if (error = WSAEWOULDBLOCK) {
                    return {} ;
                }
#else
                error = errno ;
                if (error == EAGAIN || error == EWOULDBLOCK){
                    return {};
                }
#endif
                throw SocketError("Unable to accept socket: "s + errormsg(error));
            }
            return status ;
        }
        
        //=============================================================================
        auto NetSocket::peerInformation() const -> std::pair<std::string,std::string> {
            return  util::net::peerInformation(this->descriptor);
        }

    }
}

