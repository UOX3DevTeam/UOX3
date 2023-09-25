//Copyright © 2023 Charles Kerr. All rights reserved.

#ifndef netsocket_hpp
#define netsocket_hpp

#include <cstdint>
#include <iostream>
#include <string>
#include <optional>
// NOTE, if windows, ensure you are compiling with NOMINMAX and WIN32_LEAN_AND_MEAN defined

#include "nettype.hpp"

//======================================================================
namespace util {
    namespace net {
        class NetSocket {
        protected:
            mutable sockfd_t descriptor;
            int rcvFlag;
            int sendFlag;
            bool block ;
            
            auto setOption(sockfd_t descriptor) -> void ;
            auto create(bool blocking)->void ;

        public:
            NetSocket();
            ~NetSocket() ;
            NetSocket(sockfd_t descriptor,bool blocking);
            // There is no reason to have two of the same sockets, so we dont let you copy
            NetSocket(const NetSocket&) = delete;
            auto operator=(const NetSocket&)->NetSocket & = delete;
            // We do allow moves however
            NetSocket(NetSocket &&value) noexcept;
            auto operator=(NetSocket&& value)->NetSocket&;
            
            // If you want to close, and status
            auto valid() const ->bool;
            auto close() const -> void;
            auto clientDescriptor() const -> util::net::sockfd_t     ;

            // We allow you to send and read data
            auto send(const char* data, iosize_t size) const -> status_t;
            auto read(char* data, iosize_t size) const->status_t;
            auto readRetry(char* data, iosize_t size,int microseconds) const -> status_t ;
            auto sendRetry(const char* data,iosize_t size,int microseconds) const ->status_t ;
            auto peek() const -> std::optional<std::uint8_t> ;
            
            // Socket items
            auto isBlocking() const -> bool ;
            auto setBlocking(bool state) ->void ;

            auto bind(const std::string &ipaddress, const std::string &port,bool blocking) ->void;
            auto listen(const std::string &ipaddress, const std::string &port, bool blocking) ->void ;
            auto connect(const std::string &ipaddress, const std::string &port) ->bool ;
            
            auto accept() const -> std::optional<sockfd_t> ;
            
            auto peerInformation() const -> std::pair<std::string,std::string>;
        };

    }
}

#endif /* netsocket_hpp */
