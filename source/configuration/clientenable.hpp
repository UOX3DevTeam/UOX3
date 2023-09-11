//

#ifndef clientenable_hpp
#define clientenable_hpp

#include <cstdint>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "type/clientversion.hpp"

struct ClientEnable {
    std::map<ClientVersion::version_t,bool> clients ;
    static const std::map<std::string,ClientVersion::version_t> NAMECLIENTMAP ;
    ClientEnable() ;
    auto reset() ->void ;
    auto set(ClientVersion::version_t version, bool state) -> void ;
    auto test(ClientVersion::version_t version) const ->bool ;
    auto setKeyValue(const std::string &key, const std::string &value) ->bool ;
    // Convience accessors
    // ClientSupport used to determine login-restrictions
    auto enableClient4000() const ->bool ;
    auto setEnableClient4000(bool state) ->void ;
    auto enableClient5000() const ->bool ;
    auto setEnableClient5000(bool state) ->void ;
    auto enableClient6000() const ->bool ;
    auto setEnableClient6000(bool state) ->void ;
    auto enableClient6050() const ->bool ;
    auto setEnableClient6050(bool state) ->void ;
    auto enableClient7000() const ->bool ;
    auto setEnableClient7000(bool state) ->void ;
    auto enableClient7090() const ->bool ;
    auto setEnableClient7090(bool state) ->void ;
    auto enableClient70160() const ->bool ;
    auto setEnableClient70160(bool state) ->void ;
    auto enableClient70240() const ->bool ;
    auto setEnableClient70240(bool state) ->void ;
    auto enableClient70300() const ->bool ;
    auto setEnableClient70300(bool state) ->void ;
    auto enableClient70331() const ->bool ;
    auto setEnableClient70331(bool state) ->void ;
    auto enableClient704565() const ->bool ;
    auto setEnableClient704565(bool state) ->void ;
    auto enableClient70610() const ->bool ;
    auto setEnableClient70610(bool state) ->void ;

    
};

#endif /* clientenable_hpp */
