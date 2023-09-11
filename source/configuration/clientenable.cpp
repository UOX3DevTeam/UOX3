//

#include "clientenable.hpp"

#include <algorithm>
#include <stdexcept>


using namespace std::string_literals ;

//======================================================================
const std::map<std::string,ClientVersion::version_t> ClientEnable::NAMECLIENTMAP{
    {"CLIENTSUPPORT4000"s,ClientVersion::V400}, {"CLIENTSUPPORT5000"s,ClientVersion::V500A},
    {"CLIENTSUPPORT6000"s,ClientVersion::V6000}, {"CLIENTSUPPORT6050"s,ClientVersion::V6050},
    {"CLIENTSUPPORT7000"s,ClientVersion::V7000}, {"CLIENTSUPPORT7090"s,ClientVersion::V7090},
    {"CLIENTSUPPORT70160"s,ClientVersion::V70160}, {"CLIENTSUPPORT70240"s,ClientVersion::V70240},
    {"CLIENTSUPPORT70300"s,ClientVersion::V70300}, {"CLIENTSUPPORT70331"s,ClientVersion::V70331},
    {"CLIENTSUPPORT704565"s,ClientVersion::V704565}, {"CLIENTSUPPORT70610"s,ClientVersion::V70610}
};
//======================================================================
ClientEnable::ClientEnable() {
    reset();
}
//======================================================================
auto ClientEnable::reset()->void {
    clients.clear() ;
    for (const auto &entry:NAMECLIENTMAP){
        clients.insert_or_assign(entry.second,false);
    }
    clients.at(ClientVersion::V70610) = true ;
}
//======================================================================
auto ClientEnable::set(ClientVersion::version_t version, bool state) -> void {
    auto iter = clients.find(version) ;
    if (iter != clients.end()){
        iter->second = state ;
    }
    throw std::out_of_range("Client version not valid for enabling: "s + std::to_string(version));
}
//======================================================================
auto ClientEnable::test(ClientVersion::version_t version) const ->bool {
    auto iter = clients.find(version) ;
    if (iter != clients.end()){
        return iter->second  ;
    }
    throw std::out_of_range("Client version not valid for enabling: "s + std::to_string(version));
}
//======================================================================
auto ClientEnable::setKeyValue(const std::string &key, const std::string &value) ->bool {
    auto rvalue = false ;
    auto iter = NAMECLIENTMAP.find(key) ;
    if (iter != NAMECLIENTMAP.end()){
        rvalue = true ;
        clients.at(iter->second) = static_cast<bool>(std::stoi(value,nullptr,0));
    }
    return rvalue ;
}

//======================================================================
auto ClientEnable::enableClient4000() const ->bool {
    return test(ClientVersion::V400);
}
//======================================================================
auto ClientEnable::setEnableClient4000(bool state) ->void {
    set(ClientVersion::V400,state);
}
//======================================================================
auto ClientEnable::enableClient5000() const ->bool {
    return test(ClientVersion::V500A);
}
//======================================================================
auto ClientEnable::setEnableClient5000(bool state) ->void {
    set(ClientVersion::V500A,state);
}
//======================================================================
auto ClientEnable::enableClient6000() const ->bool {
    return test(ClientVersion::V6000);
}
//======================================================================
auto ClientEnable::setEnableClient6000(bool state) ->void {
    set(ClientVersion::V6000,state);
}
//======================================================================
auto ClientEnable::enableClient6050() const ->bool {
    return test(ClientVersion::V6050);
}
//======================================================================
auto ClientEnable::setEnableClient6050(bool state) ->void {
    set(ClientVersion::V6050,state);
};
//======================================================================
auto ClientEnable::enableClient7000() const ->bool {
    return test(ClientVersion::V7000);
}
//======================================================================
auto ClientEnable::setEnableClient7000(bool state) ->void {
    set(ClientVersion::V7000,state);
}
//======================================================================
auto ClientEnable::enableClient7090() const ->bool {
    return test(ClientVersion::V7090);
}
//======================================================================
auto ClientEnable::setEnableClient7090(bool state) ->void {
    set(ClientVersion::V7090,state);
}
//======================================================================
auto ClientEnable::enableClient70160() const ->bool {
    return test(ClientVersion::V70160);
}
//======================================================================
auto ClientEnable::setEnableClient70160(bool state) ->void {
    set(ClientVersion::V70160,state);
}
//======================================================================
auto ClientEnable::enableClient70240() const ->bool {
    return test(ClientVersion::V70240);
}
//======================================================================
auto ClientEnable::setEnableClient70240(bool state) ->void {
    set(ClientVersion::V70240,state);
}
//======================================================================
auto ClientEnable::enableClient70300() const ->bool {
    return test(ClientVersion::V70300);
}
//======================================================================
auto ClientEnable::setEnableClient70300(bool state) ->void {
    set(ClientVersion::V70300,state);
}
//======================================================================
auto ClientEnable::enableClient70331() const ->bool {
    return test(ClientVersion::V70331);
}
//======================================================================
auto ClientEnable::setEnableClient70331(bool state) ->void {
    set(ClientVersion::V70331,state);
}
//======================================================================
auto ClientEnable::enableClient704565() const ->bool {
    return test(ClientVersion::V704565);
}
//======================================================================
auto ClientEnable::setEnableClient704565(bool state) ->void {
    set(ClientVersion::V704565,state);
}
//======================================================================
auto ClientEnable::enableClient70610() const ->bool {
    return test(ClientVersion::V70610);
}
//======================================================================
auto ClientEnable::setEnableClient70610(bool state) ->void {
    set(ClientVersion::V70610,state);
}
