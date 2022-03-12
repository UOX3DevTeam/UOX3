//Copyright © 2022 Charles Kerr. All rights reserved.

#include "mapip.hpp"

#include <iostream>
#include <algorithm>

#include "ip4addr.hpp"

//=========================================================
static auto _devices = ethdevice_t::devices() ;
//=========================================================
auto mapip(const ip4addr_t &addr, const std::string &wanip)->std::optional<ip4addr_t> {
	auto devices = _devices ;
	auto rvalue = std::optional<ip4addr_t>(std::nullopt);
	if (!devices.empty() ){
		std::sort(devices.begin(),devices.end(),[&addr](const ethdevice_t &rhs,const ethdevice_t &lhs) ->bool {
			return rhs._address.component_match(addr) < lhs._address.component_match(addr) ;
		}) ;
		
		// The best is the  bottom one
		auto best = *devices.rbegin();
		// how good of a match is this?
		auto quality = best._address.component_match(addr);
		// what type of ip is this match
		auto type = iptype(best._address.value()) ;
		// what type is the client?
		auto requesting_type = iptype(addr.number());
		if (quality >0) {
			// we had some sort of match,
			if (std::get<0>(type) == std::get<0>(requesting_type)){
				rvalue = std::make_optional(best._address);
			}
		}
		// Now, if we havent found anything, we can check wan
		if (!rvalue){
			// Still nothing, check wans
			if (!wanip.empty()){
				rvalue = std::make_optional(ip4addr_t(wanip));
			}
		}
	}
	return rvalue ;
}
