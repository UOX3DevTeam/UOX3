//  Copyright © 2023 Charles Kerr. All rights reserved.
//

#ifndef sock_hpp
#define sock_hpp

#include <cstdint>
#include <string>
#include <optional>
#include <utility>

#if !defined(_WIN32)
#include <sys/socket.h>
using descrip_t = int ;
using amount_t = ssize_t ;
constexpr auto invalid_socket = -1 ;
constexpr auto socket_error = -1 ;

#else
#include <winsock2.h>
using descrip_t =SOCKET;
using amount_t = int;

constexpr auto invalid_socket = INVALID_SOCKET;
constexpr auto socket_error = SOCKET_ERROR ;
#endif

namespace util{
    namespace net {
        enum class ipclass {ip4=PF_INET,ip6=PF_INET6};
        enum class socktime {send=SO_SNDTIMEO,recv=SO_RCVTIMEO};
        //===========================================================================================
        // Note: This does not close the socket on destruction!
        // It allows one to copy the socket, etc.  Therefore, the USER must ensure
        // proper closure of the socket!
        //===========================================================================================
        
        class socket_t {
        protected:
            descrip_t fd ;
            ipclass type ;
            int sflag ;
            int rflag ;
            bool blocking;
            
            
            auto create() ->void ;
            auto initialize() ->void ;
            socket_t(ipclass type, descrip_t descriptor);
        public:
            socket_t();
            socket_t(ipclass type);
            
            auto descriptor() const ->descrip_t ;
            
            // General methods
            auto setBlocking(bool value)->void;
            auto close() ->void ;
            auto valid() const ->bool ;
            auto peer() const ->std::pair<std::string,std::string> ;
            
            // Will create a socket if not initailized yet.
            auto bind(const std::string &port, const std::string &ipaddress = "",bool reuse=false)->void ;
            
            
            // Server oriented
            auto listen(int backlog = 64)->void;
            auto accept() ->std::optional<socket_t> ;
            
            // Client oriented
            // Connect will create a socket if invalid
            // Note, that if connect returns false, the socket is closed. If one has "bind" the socket
            // that will need to be repeated before connecting again.
            auto connect(const std::string &ipaddress, const std::string &port,int sec_timeout=2)->bool ;
            auto read(std::uint8_t *data,amount_t amount,bool peek=false) -> amount_t ;
            auto write(const std::uint8_t *data,amount_t amount) -> amount_t ;
            
        };
        
    }
}
#endif /* sock_hpp */
