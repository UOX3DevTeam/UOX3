//

#include "networkmgr.hpp"

#include <algorithm>
#include <stdexcept>


using namespace std::string_literals ;

//======================================================================
NetworkMgr::NetworkMgr(){
    util::net::startup();
}
//======================================================================
NetworkMgr::~NetworkMgr(){
    serverSocket.close();
    util::net::shutdown();
}

//======================================================================
auto NetworkMgr::loadAccessFiles(const std::filesystem::path &allowList, const std::filesystem::path &denyList) -> void {
    firewall.reload(denyList.string(), allowList.string());
}
//======================================================================
auto NetworkMgr::listen(std::uint16_t port, const std::string &publicIP) ->void {
    serverSocket.listen("", std::to_string(port), false);
}
//======================================================================
auto NetworkMgr::close() const ->void {
    serverSocket.close();
}
//======================================================================
auto NetworkMgr::accept() const -> std::optional<std::tuple<util::net::sockfd_t,std::string,std::string,util::net::ip4_t>> {
    auto connection = serverSocket.accept() ;
    if (connection.has_value()){
        auto [ipaddress,port] = util::net::peerInformation(connection.value()) ;
        auto binaryaddress = util::net::IP4Entry::describeIP(ipaddress) ;
        if (firewall.allowIP(binaryaddress)){
            auto relay = ipRelay.relayFor(binaryaddress) ;
            return std::make_tuple(connection.value(),ipaddress,port,relay);
        }
    }
    return {};
}
