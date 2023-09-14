//

#ifndef assistantfeature_hpp
#define assistantfeature_hpp

#include <cstdint>
#include <iostream>
#include <map>
#include <optional>
#include <string>

//======================================================================
struct AssistantFeature {
    // We would normaly do a vector<bool> or biset, but given we dont want to break existing javascript with is
    // expecting these values, we will use | and & against a std::uint64_t, as these enums are effectively a mask
    enum feature_t : std::uint64_t {
        NONE = 0,
        
        // Razor/RE/AssistUO
        FILTERWEATHER = 1 << 0,   // Weather Filter
        FILTERLIGHT = 1 << 1,     // Light Filter
        SMARTTARGET = 1 << 2,     // Smart Last Target
        RANGEDTARGET = 1 << 3,    // Range Check Last Target
        AUTOOPENDOORS = 1 << 4,   // Automatically Open Doors
        DEQUIPONCAST = 1 << 5,    // Unequip Weapon on spell cast
        AUTOPOTIONEQUIP = 1 << 6, // Un/Re-equip weapon on potion use
        POISONEDCHECKS = 1 << 7, // Block heal If poisoned/Macro IIf Poisoned condition/Heal or Cure self
        LOOPEDMACROS = 1 << 8, // Disallow Looping macros, For loops, and macros that call other macros
        USEONCEAGENT = 1 << 9,   // The use once agent
        RESTOCKAGENT = 1 << 10,  // The restock agent
        SELLAGENT = 1 << 11,     // The sell agent
        BUYAGENT = 1 << 12,      // The buy agent
        POTIONHOTKEYS = 1 << 13, // All potion hotkeys
        RANDOMTARGETS = 1 << 14, // All random target hotkeys (Not target next, last target, target self)
        CLOSESTTARGETS = 1 << 15, // All closest target hotkeys
        OVERHEADHEALTH = 1 << 16, // Health and Mana/Stam messages shown over player's heads
        
        // AssistUO only?
        AUTOLOOTAGENT = 1 << 17,   // The autoloot agent
        BONECUTTERAGENT = 1 << 18, // The bone cutter agent
        JSCRIPTMACROS = 1 << 19,   // Javascript macro engine
        AUTOREMOUNT = 1 << 20,     // Auto remount after dismount
        
        // UOSteam
        AUTOBANDAGE = 1 << 21,          // Automatically apply bandages when health is low
        ENEMYTARGETSHARE = 1 << 22,     // Share target with party/guild/alliance
        FILTERSEASON = 1 << 23,         // Season filter that forces a specific season
        SPELLTARGETSHARE = 1 << 24,     // Share spell target with party/guild/alliance
        HUMANOIDHEALTHCHECKS = 1 << 25, //
        SPEECHJOURNALCHECKS = 1 << 26,  // ???
        ALL = 0xFFFFFFFFFFFFFFFF        // Every feature possible
    };
    std::uint64_t value ;
    
    static const std::map<std::string,feature_t> ININAMEFEATUREMAP ;
    
    AssistantFeature() ;
    AssistantFeature(std::uint64_t value);
    auto operator=(std::uint64_t value) -> AssistantFeature& ;
    
    auto set(feature_t feature , bool state) ->void ;
    auto test(feature_t feature) const ->bool ;
    auto setKeyValue(const std::string &key,const std::string &value) ->bool ;
    auto describe() const -> std::string ;
    auto valueFor(const std::string &keyword) const ->std::optional<bool> ;
};

#endif /* assistantfeature_hpp */
