//

#ifndef serverdata_hpp
#define serverdata_hpp

#include <bitset>
#include <cstdint>
#include <iostream>
#include <map>
#include <string>
#include <variant>

#include "typedefs.h"

//======================================================================
// StartLocation
//======================================================================

struct StartLocation {
    std::string oldTown ;
    std::string oldDescription;
    std::string newTown ;
    std::string newDescription ;
    std::int16_t x ;
    std::int16_t y ;
    std::int16_t z ;
    std::int16_t world ;
    std::uint16_t instanceID ;
    std::uint32_t clilocDesc ;
    StartLocation() :x(0),y(0),z(0),world(0),instanceID(0),clilocDesc(0){}
    StartLocation(const std::string &line) ;
};
//======================================================================
// ClientFeature
//======================================================================
//======================================================================
struct ClientFeature {
    enum features_t {
        CHAT = 0, // 0x01
        UOR,      // 0x02
        TD,       // 0x04
        LBR,      // 0x08 - Enables LBR features: mp3s instead of midi, show new LBR monsters
        AOS, // 0x10 - Enable AoS monsters/map, AoS skills, Necro/Pala/Fight book stuff - works
        // for 4.0+
        SIXCHARS,   // 0x20 - Enable sixth character slot
        SE,         // 0x40
        ML,         // 0x80 - Elven race, new spells, skills + housing tiles
        EIGHTAGE,   // 0x100 - Splash screen for 8th age
        NINTHAGE,   // 0x200 - Splash screen for 9th age
        TENTHAGE,   // 0x400 - Splash screen for 10th age - crystal/shadow house tiles
        UNKNOWN1,   // 0x800 - Increased housing/bank storage (6.0.3.0 or earlier)
        SEVENCHARS, // 0x1000 - Enable seventh character slot
        KRFACES,        // 0x2000 - KR release (6.0.0.0)
        TRIAL,            // 0x4000 - Trial account
        EXPANSION , // 0x8000 - Live account
        SA, // 0x10000 - Enable SA features: gargoyle race, spells, skills, housing tiles -
        // clients 6.0.14.2+
        HS, // 0x20000 - Enable HS features: boats, new movementtype? ++
        GOTHHOUSE,    // 0x40000
        RUSTHOUSE,    // 0x80000
        JUNGLEHOUSE,  // 0x100000 - Enable Jungle housing tiles
        SHADOWHOUSE,  // 0x200000 - Enable Shadowguard housing tiles
        OLHOUSE,     // 0x400000 - Enable Time of Legends features
        ENDLESSHOUSE // 0x800000 - Enable Endless Journey account
    };
    static const std::map<std::string,features_t> FEATURENAMEMAP ;
    static auto size() ->size_t ;
    std::bitset<32> databits ;
    auto value() const -> std::uint32_t ;
    auto set(features_t feature,bool value) ->void ;
    auto set(const std::string &feature,bool value) ->bool ;
    auto test(features_t feature) const ->bool ;
    auto test(const std::string & eature) const ->bool ;

};
//======================================================================
// ServerFeature
//======================================================================
//======================================================================
struct ServerFeature {
    enum features_t {
        UNKNOWN1 = 0, // 0x01
        IGR,          // 0x02
        ONECHAR,      // 0x04 - One char only, Siege-server style
        CONTEXTMENUS, // 0x08
        LIMITCHAR,    // 0x10 - Limit amount of chars, combine with OneChar
        AOS,          // 0x20 - Enable Tooltips, fight system book - but not
        // monsters/map/skills/necro/pala classes
        SIXCHARS,     // 0x40 - Use 6 character slots instead of 5 (4.0.3a)
        SE,           // 0x80 - Samurai and Ninja classes, bushido, ninjitsu (4.0.5a)
        ML,           // 0x100 - Elven race, spellweaving (4.0.11d)
        UNKNOWN2,     // 0x200 - added with UO:KR launch (6.0.0.0)
        SEND3DTYPE,   // 0x400 - Send UO3D client type? KR and SA clients will send 0xE1)
        UNKNOWN4,     // 0x800 - added sometime between UO:KR and UO:SA
        SEVENCHARS,   // 0x1000 - Use 7 character slots instead of 5?6?, only 2D client?
        UNKNOWN5, // 0x2000 - added with UO:SA launch, imbuing, mysticism, throwing? (7.0.0.0)
        NEWMOVE,  // 0x4000 - new movement system (packets 0xF0, 0xF1, 0xF2))
        FACTIONAREAS // 0x8000 - Unlock new Felucca faction-areas (map0x.mul?)
    };
    static const std::map<std::string,features_t> FEATURENAMEMAP ;
    static auto size() ->size_t ;
    std::bitset<16> databits ;
    auto value() const -> std::uint16_t ;
    auto set(features_t feature,bool value) ->void ;
    auto set(const std::string &feature,bool value) ->bool ;
    auto test(features_t feature) const ->bool ;
    auto test(const std::string & eature) const ->bool ;
};
//======================================================================
// ServerData
//======================================================================
//======================================================================
class ServerData {
public:
    enum timer_t {
        TIMERERROR=-1,CORPSEDECAY,WEATHER,SHOPSPAWN,DECAY,DECAYINHOUSE,
        INVISIBLITY,OBJECTUSAGE,GATE,POISON,LOGINTIMEOUT,HITPOINTREGEN,
        STAMINAREGEN,MANAREGEN,FISHINGBASE,FISHINGRANDOM,SPIRITSPEAK,HUNGERRATE,
        THIRSTRATE,POLYMORPH,ESCORTWAIT,ESCORTACTIVE,ESCORTDONE,MURDERDECAY,
        CRIMINAL,STEALINGFLAG,AGGRESSORFLAG,PERMAGREYFLAG,COMBATIGNORE,POTION,
        PETOFFLINECHECK,NPCFLAGUPDATETIMER,BLOODDECAY,BLOODDECAYCORPSE,NPCCORPSEDECAY,LOYALITYRATE
    };
    auto timerCount() const -> size_t ;
    enum clientfeatures_t {
        CF_BIT_CHAT = 0, // 0x01
        CF_BIT_UOR,      // 0x02
        CF_BIT_TD,       // 0x04
        CF_BIT_LBR,      // 0x08 - Enables LBR features: mp3s instead of midi, show new LBR monsters
        CF_BIT_AOS, // 0x10 - Enable AoS monsters/map, AoS skills, Necro/Pala/Fight book stuff - works
        // for 4.0+
        CF_BIT_SIXCHARS,   // 0x20 - Enable sixth character slot
        CF_BIT_SE,         // 0x40
        CF_BIT_ML,         // 0x80 - Elven race, new spells, skills + housing tiles
        CF_BIT_EIGHTAGE,   // 0x100 - Splash screen for 8th age
        CF_BIT_NINTHAGE,   // 0x200 - Splash screen for 9th age
        CF_BIT_TENTHAGE,   // 0x400 - Splash screen for 10th age - crystal/shadow house tiles
        CF_BIT_UNKNOWN1,   // 0x800 - Increased housing/bank storage (6.0.3.0 or earlier)
        CF_BIT_SEVENCHARS, // 0x1000 - Enable seventh character slot
        // CF_BIT_KRFACES,        // 0x2000 - KR release (6.0.0.0)
        // CF_BIT_TRIAL,            // 0x4000 - Trial account
        CF_BIT_EXPANSION = 15, // 0x8000 - Live account
        CF_BIT_SA, // 0x10000 - Enable SA features: gargoyle race, spells, skills, housing tiles -
        // clients 6.0.14.2+
        CF_BIT_HS, // 0x20000 - Enable HS features: boats, new movementtype? ++
        CF_BIT_GOTHHOUSE,    // 0x40000
        CF_BIT_RUSTHOUSE,    // 0x80000
        CF_BIT_JUNGLEHOUSE,  // 0x100000 - Enable Jungle housing tiles
        CF_BIT_SHADOWHOUSE,  // 0x200000 - Enable Shadowguard housing tiles
        CF_BIT_TOLHOUSE,     // 0x400000 - Enable Time of Legends features
        CF_BIT_ENDLESSHOUSE, // 0x800000 - Enable Endless Journey account
    };


    enum assistantfeatures_t : std::uint64_t {
        AF_NONE = 0 ,
        // Razor/RE/AssistUO
        AF_FILTERWEATHER = 1 << 0,   // Weather Filter
        AF_FILTERLIGHT = 1 << 1,     // Light Filter
        AF_SMARTTARGET = 1 << 2,     // Smart Last Target
        AF_RANGEDTARGET = 1 << 3,    // Range Check Last Target
        AF_AUTOOPENDOORS = 1 << 4,   // Automatically Open Doors
        AF_DEQUIPONCAST = 1 << 5,    // Unequip Weapon on spell cast
        AF_AUTOPOTIONEQUIP = 1 << 6, // Un/Re-equip weapon on potion use
        AF_POISONEDCHECKS =
        1 << 7, // Block heal If poisoned/Macro IIf Poisoned condition/Heal or Cure self
        AF_LOOPEDMACROS =
        1 << 8, // Disallow Looping macros, For loops, and macros that call other macros
        AF_USEONCEAGENT = 1 << 9,   // The use once agent
        AF_RESTOCKAGENT = 1 << 10,  // The restock agent
        AF_SELLAGENT = 1 << 11,     // The sell agent
        AF_BUYAGENT = 1 << 12,      // The buy agent
        AF_POTIONHOTKEYS = 1 << 13, // All potion hotkeys
        AF_RANDOMTARGETS =
        1 << 14, // All random target hotkeys (Not target next, last target, target self)
        AF_CLOSESTTARGETS = 1 << 15, // All closest target hotkeys
        AF_OVERHEADHEALTH = 1 << 16, // Health and Mana/Stam messages shown over player's heads
        
        // AssistUO only?
        AF_AUTOLOOTAGENT = 1 << 17,   // The autoloot agent
        AF_BONECUTTERAGENT = 1 << 18, // The bone cutter agent
        AF_JSCRIPTMACROS = 1 << 19,   // Javascript macro engine
        AF_AUTOREMOUNT = 1 << 20,     // Auto remount after dismount
        
        // UOSteam
        AF_AUTOBANDAGE = 1 << 21,          // Automatically apply bandages when health is low
        AF_ENEMYTARGETSHARE = 1 << 22,     // Share target with party/guild/alliance
        AF_FILTERSEASON = 1 << 23,         // Season filter that forces a specific season
        AF_SPELLTARGETSHARE = 1 << 24,     // Share spell target with party/guild/alliance
        AF_HUMANOIDHEALTHCHECKS = 1 << 25, //
        AF_SPEECHJOURNALCHECKS = 1 << 26,  // ???
        AF_ALL = 0xFFFFFFFFFFFFFFFF        // Every feature possible
    };
    
private:
    ServerData()  ;
    
    static const std::map<timer_t,std::string> TIMERNAMEMAP ;
    auto timerForName(const std::string &name) const -> timer_t ;

    static std::map<std::string,std::variant<R32,R64,bool,std::string,std::uint8_t,std::int8_t,std::uint16_t,std::int16_t,std::uint32_t,std::int32_t,std::uint64_t,std::int64_t>> dataValue ;
    
    template<typename T>
    auto setValue(const std::string& key,T value) {
        dataValue.insert_or_assign(key,value) ;
    }
    template<typename T>
    auto getValue(const std::string& key) const  -> const T& {
        return std::get<T>(dataValue.at(key)) ;
    }
    
public:
    ServerData(const ServerData&) = delete ;
    ServerData(ServerData&&) = delete ;
    auto operator=(const ServerData&)->ServerData& = delete ;
    auto operator=( ServerData&&)->ServerData& = delete ;
    static auto shared() ->ServerData& ;
    
    auto reset() ->void ;
    
    // Accessors
    auto serverName() const -> const std::string & ;
    auto setServerName(const std::string &name) -> void ;
    
    auto secretShardKey() const -> const std::string & ;
    auto setSecretShardKey(const std::string &shardkey) ->void ;
    
    auto publicIP() const ->const std::string & ;
    auto setPublicIP(const std::string &ipvalue) ->void ;
    
    auto jsengineSize() const -> std::uint16_t ;
    auto setJSEngineSize(std::uint16_t value) ->void ;
    
    auto useUnicodeMessages() const -> bool;
    auto setUseUnicodeMessages(bool value) ->void;
    
    auto serverLanguage() const -> std::uint16_t;
    auto setServerLanguage(std::uint16_t value) ->void;
    
    auto lightDarkLevel() const -> lightlevel_t ;
    auto setLightDarkLevel(lightlevel_t level) ->void ;
    
    auto lightBrightLevel() const -> lightlevel_t;
    auto setLightBrightLevel(lightlevel_t level) ->void ;
    
    auto lightCurrentLevel() const -> lightlevel_t;
    auto setLightCurrentLevel(lightlevel_t level) ->void ;
    
    auto lightDungeonLevel() const -> lightlevel_t;
    auto setLightDungeonLevel(lightlevel_t level) ->void ;
    
    auto clientSupport4000() const -> bool ;
    auto setClientSupport4000(bool value) ->void ;
    
    auto clientSupport5000() const -> bool ;
    auto setClientSupport5000(bool value) ->void ;
    
    auto clientSupport6000() const -> bool ;
    auto setClientSupport6000(bool value) ->void ;
    
    auto clientSupport6050() const -> bool ;
    auto setClientSupport6050(bool value) ->void ;
    
    auto clientSupport7000() const -> bool ;
    auto setClientSupport7000(bool value) ->void ;
    
    auto clientSupport7090() const -> bool ;
    auto setClientSupport7090(bool value) ->void ;
    
    auto clientSupport70160() const -> bool ;
    auto setClientSupport70160(bool value) ->void ;
    
    auto clientSupport70240() const -> bool ;
    auto setClientSupport70240(bool value) ->void ;
    
    auto clientSupport70300() const -> bool ;
    auto setClientSupport70300(bool value) ->void ;
    
    auto clientSupport70331() const -> bool ;
    auto setClientSupport70331(bool value) ->void ;
    
    auto clientSupport704565() const -> bool ;
    auto setClientSupport704565(bool value) ->void ;
    
    auto clientSupport70610() const -> bool ;
    auto setClientSupport70610(bool value) ->void ;
    
};

#endif /* serverdata_hpp */
