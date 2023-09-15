//

#ifndef realnumberconfig_hpp
#define realnumberconfig_hpp

#include <cstdint>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <vector>

/*
 R32 npcWalkingSpeed;        //    Speed at which walking NPCs move
 R32 npcRunningSpeed;        //    Speed at which running NPCs move
 R32 npcFleeingSpeed;        //    Speed at which fleeing NPCs move
 R32 npcMountedWalkingSpeed; //    Speed at which (mounted) walking NPCs move
 R32 npcMountedRunningSpeed; //    Speed at which (mounted) running NPCs move
 R32 npcMountedFleeingSpeed; //    Speed at which (mounted) fleeing NPCs move
 R32 archeryShootDelay; //  Attack delay for archers; after coming to a full stop, they need to
 //  wait this amount of time before they can fire an arrow. Defaults
 //  to 1.0 seconds
 R32 globalAttackSpeed; //  Global attack speed that can be tweaked to quickly increase or
 //  decrease overall combat speed. Defaults to 1.0
 R32 npcSpellcastSpeed; //  For adjusting the overall speed of (or delay between) NPC spell
 //  casts. Defaults to 1.0
 R32 globalRestockMultiplier; //    Global multiplier applied to restock properties of items
 // when loaded from DFNs
 R32 bodGoldRewardMultiplier; //    Multiplier that adjusts the amount of Gold rewarded by
 // completing Bulk Order Deeds
 R32 bodFameRewardMultiplier; //    Multiplier that adjusts the amount of Fame rewarded by
 // completing Bulk Order Deeds
 R64 checkItems;        //    How often (in seconds) items are checked for decay and other things
 R64 checkBoats;        //    How often (in seconds) boats are checked for motion and so forth
 R64 checkNpcAi;        //    How often (in seconds) NPCs can execute an AI routine
 R64 checkSpawnRegions; //    How often (in seconds) spawn regions are checked for new spawns
 R64 flushTime; //    How often (in minutes) online accounts are checked to see if they really ARE

 */
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
