//Copyright © 2023 Charles Kerr. All rights reserved.
// NOTE, if windows, ensure you are compiling with NOMINMAX and WIN32_LEAN_AND_MEAN defined

#ifndef nettype_hpp
#define nettype_hpp

#include <cstdint>
#include <iostream>
#include <string>

#if defined(_WIN32)
#include <Windows.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#endif
//======================================================================
namespace util {
    namespace net {
#if defined(_WIN32)
        using sockfd_t = SOCKET;
        constexpr auto BADSOCKET = INVALID_SOCKET;
        constexpr auto SOCKETERROR = SOCKET_ERROR;
        using iosize_t = int;
        using status_t = int;
#else
        using sockfd_t = int;
        constexpr auto BADSOCKET = -1;
        using iosize_t = size_t;
        using status_t = ssize_t;
        constexpr auto SOCKETERROR = -1;
#endif
        //=================================================================================
        using ip4_t = std::uint32_t ;
        
    }
}

#endif /* nettype_hpp */
