//Copyright © 2023 Charles Kerr. All rights reserved.

#ifndef netutil_hpp
#define netutil_hpp

#include <array>
#include <cstdint>
#include <cstring>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "nettype.hpp"
//======================================================================
namespace util {
    namespace net {
        //=================================================================================
        // Intialize the network
        // For windows, this is initialize winsock2.
        // For everythig else, nothing
        auto startup() ->void ;
        //=================================================================================
        // shutdown the network
        // For windows, this is initialize winsock2.
        // For everythig else, nothing
        auto shutdown(bool complain = false) ->void ;
        
        //=================================================================================
        // close a socket
        auto closeSocket(sockfd_t descriptor) ->void ;
        
        //=================================================================================
        // Network error message
        auto errormsg(int error) -> std::string;
        
        //=================================================================================
        // Peer information
        auto peerInformation(sockfd_t descriptor) -> std::pair<std::string,std::string> ;
        
        // Get the available IP4's that are on this machine
        auto availIP4() -> std::vector<ip4_t> ;
        
        
        
    }
}

#endif /* netutil_hpp */
