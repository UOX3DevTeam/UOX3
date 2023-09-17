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

// SHARD Determines core era ruleset for shard (determines which items/npcs are loaded, and which rules are applied in combat)
// ARMOR Determines which era ruleset to use for calculating armor and defense
// STRENGTH Determines which era ruleset to use for calculating strength damage bonus
// TATIC Determines which era ruleset to use for calculating tactics damage bonus
// ANATOMY  Determines which era ruleset to use for calculating anatomy damage bonus
// LUMBERJACK Determines which era ruleset to use for calculating lumberjack damage bonus
// RACIAL Determines which era ruleset to use for calculating racial damage bonus
// DAMAGE Determines which era ruleset to use for calculating damage bonus cap
// SHIELD Determines which era ruleset to use for shield parry calculations
// WEAPON Determines which era ruleset to use for weapon parry calculations
// WRESTLING Determines which era ruleset to use for wrestling parry calculations
// COMBAT Determines which era ruleset to use for calculating melee hit chance
//======================================================================
struct Expansion {
    enum type_t {
        SHARD = 0, ARMOR, STRENGTH, TATIC,
        ANATOMY, LUMBERJACK, RACIAL, DAMAGE,
        SHIELD, WEAPON, WRESTLING, COMBAT
    };
    static const std::map<std::string, type_t> NAMETYPEMAP ;
    static const Era defaultEra ;
    static auto nameFor(type_t type) -> const std::string&;
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
