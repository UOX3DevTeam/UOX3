//Copyright © 2023 Charles Kerr. All rights reserved.

#include "netutil.hpp"

#include <iostream>
#include <stdexcept>

#if defined (_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#pragma comment(lib,"Ws2_32.lib")
#else
#include <errno.h>

#endif


using namespace std::string_literals;


namespace util {
    namespace net {
        //=================================================================================
        // Intialize the network
        auto initialize() ->void{
#if defined(_WIN32)
            WSADATA wsa_data ;
            auto results = WSAStartup(MAKEWORD(2,2),&wsa_data);
            if (results != 0){
                results = WSAGetLastError();
                auto msg = errormsg(results);
                throw std::runtime_error("Error initializing network: "s + msg);
            }
            if ( LOBYTE(wsa_data.wVersion ) != 2 ||
                HIBYTE(wsa_data.wVersion ) != 2 ) {
                WSACleanup( );
                throw std::runtime_error("Unable to obtain minimum WinSock version of 2.2 "s);
            }
            
#endif
        }
        
        //========================================================================
        auto shutdown() ->void {
#if defined(_WIN32)
            WSACleanup();
#endif
        }
        //========================================================================
        auto errormsg(int error) ->std::string {
            auto value = std::string();
#if !defined(_WIN32)
            value = std::strerror(error);
#else
            LPTSTR  buffer = NULL;
            error = WSAGetLastError();
            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPTSTR>(&buffer), 0, NULL);
            if (buffer != NULL) {
                value = *buffer;
                LocalFree(buffer);
            }
#endif
            return value;
        }
        
    }
}
