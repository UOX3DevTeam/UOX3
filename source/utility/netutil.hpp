//Copyright © 2023 Charles Kerr. All rights reserved.

#ifndef netutil_hpp
#define netutil_hpp

#include <array>
#include <cstdint>
#include <cstring>
#include <optional>
#include <string>
#include <vector>

#if defined(_WIN32)
// We just dont fight it, go ahead and get windows
#if !defined(NOMINMAX)
#define NOMINMAX 
#endif
#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#else

#endif


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
        
        // Get the text for error numbers
        auto errormsg(int error) -> std::string;
        using ip4_t = std::uint32_t ;
        // Get the available IP4's that are on this machine
        auto availIP4() -> std::vector<ip4_t> ;


    }
}

#endif /* netutil_hpp */
