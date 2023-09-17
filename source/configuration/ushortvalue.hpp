//

#ifndef ushortvalue_hpp
#define ushortvalue_hpp

#include <cstdint>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <vector>

// PORT The port for the network to listen on
// LANGUAGE langauge the server will default to
// JSENGINE gcMaxBytes limit in MB per JS runtime
// SYSMESSAGECOLOR Default text colour for system messages displayed in bottom left corner of screen
// APSPERFTHRESHOLD Performance threshold (simulation cycles) below which APS system kicks in - 0 disables system
// APSINTERVAL Interval at which APS checks and optionally makes adjustments based on shard performance
// APSDELAYSTEP Step value in milliseconds that APS uses to gradually adjust delay for NPC AI/movement checks
// APSDELAYMAX Max impact in milliseconds APS can have on NPC AI/movement checks
// SKILLLEVEL Some skill value associated with the rank system
// SKILLCAP A cap on each individual PC skill
// SKILLDELAY Number of seconds after a skill is used before another skill can be used
// STATCAP A cap on the total of a PC's stats
// MAXPLAYERPACKITEM The max amount of items a player's backpack can contain
// MAXPLAYERBANKITEM The max amount of items a player's bankbox can contain
// MAXSAFETELEPORT The default max amount of free teleports to safety players get via the help menu per day
// MAXCONTROLSLOT The default max amount of pet/follower control slots for each player
// MAXFOLLOWER The default max amount of followers a player can have active
// MAXPETOWNER The default max amount of different owners a pet may have in its lifetime
// PETLOYALITYGAINSUCCESS The default amount of pet loyalty gained on successful use of a pet command
// PETLOYALITYLOSSFAILURE The default amount of pet loyalty lost on a failed attempt to use a pet command
// POSTINGLEVEL If not 0, then players can post (should be a switch?)
// REMOVALLEVEL If not 0, then players can remove posts (should be a switch?)
// DUNGEONLIGHT Default light level for a dungeon, if not subject to a  weather system
// BRIGHTLEVEL Default brightest light level if not subject to a weather system
// DARKLEVEL Default darkest light level if not subject to a weather system
// SECONDSPERUOMINUTE Number of seconds for a UOX minute.
// BASERANGE Minimum range even a novice can track from
// MAXTARGET Maximum number of targets that can be tracked
// MSGREDISPLAYTIME How often (in seconds) the tracking message is redisplayed
// MAXKILL  Maximum number of kills before turning red
// RESOURCEAREASIZE Size of each resource area to split each world into
// MINECHECK Type of mining check performed - 0 anywhere 1 only mountains/floors 2 region based (not working)
// LOGPERAREA Maximum number of logs in a resource area
// PETOFFLINETIMEOUT Offline time after a player looses all pets
// ANIMALATTACKCHANCE Chance of animals being attacked (0-100)
// MINWEAPONDAMAGE Minimum amount of hitpoints a weapon can lose when being damaged in combat
// MAXWEAPONDAMAGE Maximum amount of hitpoints a weapon can lose when being damaged in combat
// ARMORDAMAGECHANCE Chance of armor being damaged when defending in combat (0-100)
// MINARMORDAMAGE Minimum amount of hitpoints an armor can lose when being damaged in combat
// MAXARMORDAMAGE  Maximum amount of hitpoints an armor can lose when being damaged in combat
// PARRYDAMAGECHANCE Chance of shield being damaged when parrying in combat (0-100)
// MINPARRYDAMAGE Minimum amount of hitpoints a shield/weapon can lose when successfully parrying in combat
// MAXPARRYDAMAGE Maximum amount of hitpoints a shield/weapon can lose when successfully parrying in combat
// ALCHEMENYBONUSMODIFIER Modifier used to calculate bonus damage for explosion potions based on alchemy skill
// BLOODEFFECTCHANCE Chance of blood splatter effects spawning during combat
// STARTPRIV Starting privileges of characters
// TITLECOLOR Default text colour for titles in gumps
// LEFTTEXTCOLOR Default text colour for left text in gumps (2 column ones)
// RIGHTTEXTCOLOR Default text colour for right text in gumps
// BUTTONCANCEL Default Button ID for cancel button in gumps
// BUTTONLEFT Default Button ID for left button in gumps
// BUTTONRIGHT Default Button ID for right button in gumps
// BACKGROUNDPIC Default Gump ID for background gump
// MAXHOUSEOWNABLE Max amount of houses that a player can own
// MAXHOUSECOOWNABLE Max amount of houses that a player can co-own
// OREPERAREA Maximum number of ores in a resource area
// FISHPERAREA Maximum number of fish in a resource area
// WEAPONDAMAGECHANCE Chance of weapons being damaged when attacking in combat
//======================================================================
struct UShortValue {
    enum item_t {
        PORT,LANGUAGE,JSENGINE,SYSMESSAGECOLOR,
        APSPERFTHRESHOLD,APSINTERVAL,APSDELAYSTEP,APSDELAYMAX,
        SKILLLEVEL,SKILLCAP,SKILLDELAY,STATCAP,
        MAXPLAYERPACKITEM,MAXPLAYERBANKITEM,MAXSAFETELEPORT,
        MAXCONTROLSLOT,MAXFOLLOWER,MAXPETOWNER,
        PETLOYALITYGAINSUCCESS,PETLOYALITYLOSSFAILURE,
        POSTINGLEVEL,REMOVALLEVEL,DUNGEONLIGHT,
        BRIGHTLEVEL,DARKLEVEL,SECONDSPERUOMINUTE,BASERANGE,
        MAXTARGET,MSGREDISPLAYTIME,MAXKILL,RESOURCEAREASIZE,
        MINECHECK,LOGPERAREA,PETOFFLINETIMEOUT,ANIMALATTACKCHANCE,MINWEAPONDAMAGE,
        MAXWEAPONDAMAGE,ARMORDAMAGECHANCE,MINARMORDAMAGE,MAXARMORDAMAGE,
        PARRYDAMAGECHANCE,MINPARRYDAMAGE,MAXPARRYDAMAGE,ALCHEMENYBONUSMODIFIER,
        BLOODEFFECTCHANCE,WEAPONDAMAGEBONUSTYPE,STARTPRIV,
        TITLECOLOR,LEFTTEXTCOLOR,RIGHTTEXTCOLOR,
        BUTTONCANCEL,BUTTONLEFT,BUTTONRIGHT,BACKGROUNDPIC,
        MAXHOUSEOWNABLE,MAXHOUSECOOWNABLE,OREPERAREA,
        FISHPERAREA,WEAPONDAMAGECHANCE
    };
    static const std::map<std::string, item_t> NAMEITEMMAP ;
    static auto nameFor(item_t setting)-> const std::string & ;

    std::vector<std::uint16_t> values ;
    
    UShortValue() ;
    auto reset() ->void ;
    
    auto operator[](item_t item) const -> const std::uint16_t &;
    auto operator[](item_t item) -> std::uint16_t &;
    
    auto setKeyValue(const std::string &key, const std::string &value) ->bool ;
    auto valueFor(const std::string &value) const -> std::optional<std::uint16_t> ;
};



#endif /* ushortvalue_hpp */
