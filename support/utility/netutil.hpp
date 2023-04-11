//Copyright © 2023 Charles Kerr. All rights reserved.

#ifndef netutil_hpp
#define netutil_hpp

#include <cstdint>
#include <string>
#include <cstring>
#include "sock.hpp"      // We include this, so users can just include one hpp file.
namespace util {
    namespace net {
        //=================================================================================
        // Intialize the network
        // For windows, this is initialize winsock2.
        // For everythig else, nothing
        auto initialize() ->void ;
        //=================================================================================
        // shutdown the network
        // For windows, this is initialize winsock2.
        // For everythig else, nothing
        auto shutdown() ->void ;
        
        // Get the text for error numbers
        auto errormsg(int error) -> std::string;
    }
}
#endif /* netutil_hpp */
