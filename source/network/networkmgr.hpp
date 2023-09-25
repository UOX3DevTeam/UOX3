//

#ifndef networkmgr_hpp
#define networkmgr_hpp

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "utility/ip4util.hpp"
#include "utility/netutil.hpp"
#include "utility/netsocket.hpp"

class CSocket ;
//======================================================================
class NetworkMgr {
    util::net::NetSocket serverSocket ;
    util::net::IP4Relay ipRelay ;
    util::net::AllowDeny firewall ;

public:
    NetworkMgr() ;
    ~NetworkMgr() ;
    
    auto loadAccessFiles(const std::filesystem::path &allowList, const std::filesystem::path &denyList) -> void ;
    auto listen(std::uint16_t port, const std::string &publicIP) ->void ;
    auto close() const -> void ;
    auto accept() const -> std::optional<std::tuple<util::net::sockfd_t,std::string,std::string,util::net::ip4_t>> ;
};

#endif /* networkmgr_hpp */
