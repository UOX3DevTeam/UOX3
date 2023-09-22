//

#ifndef netsocket_hpp
#define netsocket_hpp

#include <cstdint>
#include <iostream>
#include <string>

#if defined(_WIN32)
#include <winsock2.h>
using socketfd_t = SOCKET ;
constexpr auto SOCKETERROR = INVALID_SOCKET ;
#else
using socketfd_t = int ;
constexpr auto SOCKETERROR = -1 ;
#endif 


//======================================================================
class NetSocket {
    
};

#endif /* netsocket_hpp */
