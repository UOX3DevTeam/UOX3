//Copyright © 2022 Charles Kerr. All rights reserved.

#ifndef mapip_hpp
#define mapip_hpp

#include <cstdint>
#include <string>
#include <optional>

struct ip4addr_t ;
//=========================================================
auto mapip(const ip4addr_t &addr, const std::string &wanip)->std::optional<ip4addr_t> ;
#endif /* mapip_hpp */
