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
struct Ip4Addr_st {
  private:
    static const std::vector<Ip4Addr_st> lanips;
    static const std::vector<Ip4Addr_st> localips;
    static const std::vector<Ip4Addr_st> apipaips;

  public:
    enum ip4type_t { invalid, local, lan, apipa, wan };
    std::array<std::string, 4> components;

    Ip4Addr_st(std::uint32_t addr, bool bigendian = true);
    Ip4Addr_st(const std::string &value = "");

    auto ipaddr(bool bigendian = true) const -> std::uint32_t;

    auto exact(const Ip4Addr_st &value) const -> bool;

    auto match(std::uint32_t value, bool bigendian = true) const -> int;
    auto match(const Ip4Addr_st &value) const -> int;

    auto operator==(const Ip4Addr_st &value) const -> bool;
    auto operator==(std::uint32_t value) const -> bool;
    auto description() const -> std::string;
    auto type() const -> ip4type_t;
};

// o------------------------------------------------------------------------------------------------o
//  ip4list_t
// o------------------------------------------------------------------------------------------------o
struct ip4list_t {
    std::vector<Ip4Addr_st> ipaddresses;

    static auto available() -> ip4list_t;

    ip4list_t(const std::string &filename = "");
    auto load(const std::string &filename) -> bool;

    auto add(const Ip4Addr_st &value) -> void;
    auto remove(const Ip4Addr_st &value) -> void;

    auto size() const -> size_t;
    auto bestmatch(std::uint32_t value, bool bigendian = true) const -> std::pair<Ip4Addr_st, int>;
    auto bestmatch(const Ip4Addr_st &value) const -> std::pair<Ip4Addr_st, int>;

    auto ips() const -> const std::vector<Ip4Addr_st> &;
    auto ips() -> std::vector<Ip4Addr_st> &;
};

#endif /* IP4Address_hpp */
