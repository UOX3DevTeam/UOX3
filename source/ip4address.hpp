//
// Created on:  6/8/21

#ifndef IP4Address_hpp
#define IP4Address_hpp
#include <array>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

// o------------------------------------------------------------------------------------------------o
//  ip4addrt
// o------------------------------------------------------------------------------------------------o
struct IP4Addr {
private:
    static const std::vector<IP4Addr> lanips;
    static const std::vector<IP4Addr> localips;
    static const std::vector<IP4Addr> apipaips;
    
public:
    enum ip4type_t { invalid, local, lan, apipa, wan };
    std::array<std::string, 4> components;
    
    IP4Addr(std::uint32_t addr, bool bigendian = true);
    IP4Addr(const std::string &value = "");
    
    auto ipaddr(bool bigendian = true) const -> std::uint32_t;
    
    auto exact(const IP4Addr &value) const -> bool;
    
    auto match(std::uint32_t value, bool bigendian = true) const -> int;
    auto match(const IP4Addr &value) const -> int;
    
    auto operator==(const IP4Addr &value) const -> bool;
    auto operator==(std::uint32_t value) const -> bool;
    auto description() const -> std::string;
    auto type() const -> ip4type_t;
};

// o------------------------------------------------------------------------------------------------o
//  ip4list_t
// o------------------------------------------------------------------------------------------------o
struct ip4list_t {
    std::vector<IP4Addr> ipaddresses;
    
    static auto available() -> ip4list_t;
    
    ip4list_t(const std::string &filename = "");
    auto load(const std::string &filename) -> bool;
    
    auto add(const IP4Addr &value) -> void;
    auto remove(const IP4Addr &value) -> void;
    
    auto size() const -> size_t;
    auto bestmatch(std::uint32_t value, bool bigendian = true) const -> std::pair<IP4Addr, int>;
    auto bestmatch(const IP4Addr &value) const -> std::pair<IP4Addr, int>;
    
    auto ips() const -> const std::vector<IP4Addr> &;
    auto ips() -> std::vector<IP4Addr> &;
};

#endif /* IP4Address_hpp */
