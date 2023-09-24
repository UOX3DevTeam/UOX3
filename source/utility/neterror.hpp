//Copyright © 2023 Charles Kerr. All rights reserved.

#ifndef neterror_hpp
#define neterror_hpp

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
//======================================================================

namespace util {
    namespace net {
        class SocketClose : public std::runtime_error {
        public:
            SocketClose(const std::string &error):std::runtime_error(error){}
        };
        class SocketPeerClose : public std::runtime_error {
        public:
            SocketPeerClose(const std::string &error):std::runtime_error(error){}
        };
        class SocketError : public std::runtime_error {
        public:
            SocketError(const std::string &error):std::runtime_error(error){}
        };
    }
}
#endif /* neterror_hpp */
