//

#ifndef shortvalue_hpp
#define shortvalue_hpp

#include <cstdint>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <vector>

// SAVERATIO Number of saves before a backup occurs
// MAXSTEALTHMOVEMENT Max number of steps allowed with stealth skill at 100.0
// MAXSTAMINAMOVEMENT Max number of steps allowed while running before stamina is reduced
// AMBIENTSOUND Ambient sounds - values from 1->10 - higher values indicate sounds
// MAXSELLITEM Maximum number of items that can be sold to a vendor
// FISHINGSTAMINALOSS  The amount of stamina lost with each use of fishing skill
// HUNGERDAMAGE Amount of damage applied if hungry and below threshold
// THIRSTDRAIN Amount of stamina drained if thirsty and below threshold
// MAXRANGE Range at which combat can actually occur
// MAXSPELLRANGE Range at which spells can be cast
// NPCDAMAGERATE NPC Damage divisor - PCs sustain less than NPCs.  If a PC, damage is 1/value
// NPCBASEFLEEAT % of HP where an NPC will flee, if it's not defined for them
// NPCBASEREATTACKAT % of HP where an NPC will resume attacking
// ATTACKSTAMINA Amount of stamina lost when hitting an opponent
// ARCHERYTHITBONUS  Bonus to hit chance for Archery skill in combat, applied after regular hit chance calculation
// STARTGOLD Amount of gold created when a PC is made
//======================================================================
struct ShortValue {
    enum item_t {
        SAVERATIO,MAXSTEALTHMOVEMENT,
        MAXSTAMINAMOVEMENT,AMBIENTSOUND,
        MAXSELLITEM,BANKBUYTHRESHOLD,
        FISHINGSTAMINALOSS,HUNGERDAMAGE,
        THIRSTDRAIN,MAXRANGE,
        MAXSPELLRANGE,NPCDAMAGERATE,
        NPCBASEFLEEAT,NPCBASEREATTACKAT,
        ATTACKSTAMINA,ARCHERYTHITBONUS,
        STARTGOLD
    };
    static const std::map<std::string, item_t> NAMEITEMMAP ;
    static auto nameFor(item_t setting)-> const std::string & ;

    std::vector<std::int16_t> values ;
    
    ShortValue() ;
    auto reset() ->void ;
    
    auto operator[](item_t item) const -> const std::int16_t &;
    auto operator[](item_t item) -> std::int16_t &;
    
    auto setKeyValue(const std::string &key, const std::string &value) ->bool ;
    auto valueFor(const std::string &value) const -> std::optional<std::int16_t> ;
};



#endif /* shortvalue_hpp */
