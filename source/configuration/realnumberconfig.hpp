//

#ifndef realnumberconfig_hpp
#define realnumberconfig_hpp

#include <cstdint>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <vector>

// NPCMOVEMENT Speed at which walking NPCs move (smaller if faster)
// NPCRUNNING Speed at which running NPCs move
// NPCFLEEING Speed at which fleeing NPCs move
// NPCMOUNTMOVEMENT Speed at which (mounted) walking NPCs move
// NPCMOUNTRUNNING Speed at which (mounted) running NPCs move
// NPCMOUNTFLEEING Speed at which (mounted) fleeing NPCs move
// NPCSPELLCAST For adjusting the overall speed of (or delay between) NPC spell casts
// GLOBALATTACK Global attack speed that can be tweaked to quickly increase or decrease overall combat speed.
// RESTOCKMULTIPLER Global multiplier applied to restock properties of items when loaded from DFNs
// FAMEREWARDMULTIPLER Multiplier that adjusts the amount of Fame rewarded by completing Bulk Order Deeds
// CHECKITEMS How often (in seconds) items are checked for decay and other things
// CHECKBOATS How often (in seconds) boats are checked for motion and so forth
// CHECKAI How often (in seconds) NPCs can execute an AI routine
// CHECKSPAWN How often (in seconds) spawn regions are checked for new spawns
// FLUSHTIME How often (in minutes) online accounts are checked to see if they really ARE online
// GOLDREWARDMULTIPLER     Multiplier that adjusts the amount of Gold rewarded by completing Bulk Order Deeds
// WEIGHTSTR How much weight per point of STR a character can hold without going through guildstone
// ARCHERYDELAY Attack delay for archers; after coming to a full stop, they need to wait this amount of time before they can fire an arrow.
//======================================================================
struct RealNumberConfig {
    enum type_t {
        NPCMOVEMENT=0,NPCRUNNING,NPCFLEEING,NPCMOUNTMOVEMENT,
        NPCMOUNTRUNNING,NPCMOUNTFLEEING,NPCSPELLCAST,GLOBALATTACK,
        RESTOCKMULTIPLER,FAMEREWARDMULTIPLER,CHECKITEMS,CHECKBOATS,
        CHECKAI,CHECKSPAWN,FLUSHTIME,GOLDREWARDMULTIPLER,
        WEIGHTSTR,ARCHERYDELAY
    };
    static const std::map<std::string, type_t> NAMETYPEMAP;
    static auto nameFor(type_t setting)-> const std::string & ;
    std::vector<double> values ;
    
    RealNumberConfig() ;
    auto reset() ->void ;
    auto size() const -> size_t;
    
    auto operator[](type_t type) const -> const double&;
    auto operator[](type_t type) -> double& ;
    auto setKeyValue(const std::string &key, const std::string &value) ->bool ;
    auto valueFor(const std::string &key) const -> std::optional<double> ;
};

#endif /* realnumberconfig_hpp */
