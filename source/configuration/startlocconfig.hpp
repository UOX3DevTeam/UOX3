//

#ifndef startlocconfig_hpp
#define startlocconfig_hpp

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "type/startlocation.hpp"
//======================================================================
struct StartLocConfig {
    std::string keyword ;
    std::vector<StartLocation> startLocation ;
    auto describe() const -> std::string ;
    StartLocConfig(const std::string &keyword="");
    auto reset() -> void ;
    auto setKeyValue(const std::string &key, const std::string &value) ->bool ;
    auto operator[](size_t index) const -> const StartLocation&;
    auto operator[](size_t index)  ->  StartLocation&;
    auto size() const -> size_t ;

};

#endif /* startlocconfig_hpp */
