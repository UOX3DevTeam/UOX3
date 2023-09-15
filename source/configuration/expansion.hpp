//

#ifndef expansion_hpp
#define expansion_hpp

#include <cstdint>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "type/era.hpp"
//======================================================================
struct Expansion {
    enum type_t {
        SHARD = 0, ARMOR, STRENGTH, TATIC,
        ANATOMY, LUMBERJACK, RACIAL, DAMAGE,
        SHIELD, WEAPON,WRESTLING, COMBAT
    };
    static const std::map<std::string, type_t> NAMETYPEMAP ;
    static const Era defaultEra ; 
    std::vector<Era> values ;
    Expansion() ;
    auto reset() ->void ;
    auto operator[](type_t type) const -> const Era& ;
    auto operator[](type_t type)  ->  Era& ;
    auto setKeyValue(const std::string &key, const std::string &value) ->bool ;
    auto valueFor(const std::string &key) const -> std::optional<Era> ;
    auto normalizedEraString(const std::string &eraString, bool useDefault=true, bool inheritCore=true) const -> Era ;
    auto normalizedEra(const Era &era, bool coreEnum=false) const -> const std::string & ;
};

#endif /* expansionconfig_hpp */
