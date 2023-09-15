//

#ifndef timersetting_hpp
#define timersetting_hpp

#include <cstdint>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <vector>

//======================================================================
struct TimerSetting {
    enum timer_t {
        CORPSEDECAY,        // Amount of time for a corpse to decay.
        WEATHER,            // Amount of time between changing light levels (day cycles).
        SHOPSPAWN,          // Amount of time between shopkeeper restocks.
        DECAY,              // Amount of time a decayable item will remain on the ground before dissapearing.
        DECAYINHOUSE,       // Amount of time a decayable item will remain on the floor of a house
                            // before dissapearing, if not locked down.
        INVISIBILITY,       // Duration of the invisibility spell.
        OBJECTUSAGE,        // Amount of time a player must wait between using objects.
        GATE,               // Duration of a summoned moongate.
        POISON,             // Duration of the poison effect on a character.
        LOGINTIMEOUT,       // Amount of time for an idle connection to time out.
        HITPOINTREGEN,      // Amount of time required to regenerate a single point of health.
        STAMINAREGEN,       // Amount of time required to regenerate a single point of stamina.
        MANAREGEN,          // Amount of time required to regenerate a single point of mana.
        FISHINGBASE,        // Base time for fishing skill to complete.
        FISHINGRANDOM,      // Max random time for fishing skill to complete.
        SPIRITSPEAK,        // Duration of the spirit speak skill.
        HUNGERRATE,         // Amount of time a player has before his hunger level decreases.
        THIRSTRATE,         // Amount of time a player has before his thirst level decreases.
        POLYMORPH,          // Duration of the polymorph spell.
        ESCORTWAIT,         // Amount of time until an escort NPC will dissapear while waiting for a
                            // player to start his quest.
        ESCORTACTIVE,       // Amount of time until an escort NPC will dissapear while a player is
                            // escorting him.
        ESCORTDONE,         // Amount of time until an escort NPC will dissapear when his quest is
                            // finished.
        MURDERDECAY,        // Amount of time before a permanent murder count will decay.
        CRIMINAL,           // Amount of time a character remains criminal after committing a criminal
                            // act.
        STEALINGFLAG,       // Amount of time a character's stealing flag remains active
        AGGRESSORFLAG,      // Amount of time a character remains aggressor after committing an
                            // aggressive act
        PERMAGREYFLAG,      // Amount of time a permagrey flag remains active after player has stolen
                            // from someone
        COMBATIGNORE,       // Amount of time an NPC will ignore an unreachable target in combat
        PETOFFLINECHECK,    // Delay between checks for the PetOfflineTimeout
        NPCFLAGUPDATETIMER, // Delay in seconds between each time NPC flags are updated
        BLOODDECAY,         // Delay in seconds before blood spatter spawned from combat decays
        BLOODDECAYCORPSE,   // Delay in seconds before blood spawned along with corpses decays
        NPCCORPSEDECAY,     // Delay in seconds before NPC corpses decay
        LOYALTYRATE,        // Amount of time between each time loyalty decreases for a pet
        TRACKING,           // Tracking base timer
        ORE,                // Ore respawn
        LOG,                // Log respawn
        FISH,               // Fish respawn
        POTION             // Delay between using potions

    };
    static const std::map<std::string,timer_t> NAMETIMERMAP ;
    std::vector<std::uint16_t> timerValue;
    
    TimerSetting() ;
    auto reset() ->void ;
    
    auto operator[](timer_t timer) const -> const std::uint16_t & ;
    auto operator[](timer_t timer) -> std::uint16_t& ;
    auto size() const ->size_t ;
    
    auto setKeyValue(const std::string &key, const std::string &value) ->bool ;
    auto valueFor(const std::string &key) -> std::optional<std::uint16_t> ;
};

#endif /* timersetting_hpp */
