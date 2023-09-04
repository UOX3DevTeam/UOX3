
#ifndef __CSERVERDATA__
#define __CSERVERDATA__
#include <array>
#include <bitset>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "typedefs.h"

#include "ip4address.hpp"
#include "stringutility.hpp"
// o------------------------------------------------------------------------------------------------o
enum ClientFeatures {
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
    // CF_BIT_KRFACES,		// 0x2000 - KR release (6.0.0.0)
    // CF_BIT_TRIAL,			// 0x4000 - Trial account
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
    CF_BIT_COUNT
};

enum ServerFeatures {
    SF_BIT_UNKNOWN1 = 0, // 0x01
    SF_BIT_IGR,          // 0x02
    SF_BIT_ONECHAR,      // 0x04 - One char only, Siege-server style
    SF_BIT_CONTEXTMENUS, // 0x08
    SF_BIT_LIMITCHAR,    // 0x10 - Limit amount of chars, combine with OneChar
    SF_BIT_AOS,          // 0x20 - Enable Tooltips, fight system book - but not
                         // monsters/map/skills/necro/pala classes
    SF_BIT_SIXCHARS,     // 0x40 - Use 6 character slots instead of 5 (4.0.3a)
    SF_BIT_SE,           // 0x80 - Samurai and Ninja classes, bushido, ninjitsu (4.0.5a)
    SF_BIT_ML,           // 0x100 - Elven race, spellweaving (4.0.11d)
    SF_BIT_UNKNOWN2,     // 0x200 - added with UO:KR launch (6.0.0.0)
    SF_BIT_SEND3DTYPE,   // 0x400 - Send UO3D client type? KR and SA clients will send 0xE1)
    SF_BIT_UNKNOWN4,     // 0x800 - added sometime between UO:KR and UO:SA
    SF_BIT_SEVENCHARS,   // 0x1000 - Use 7 character slots instead of 5?6?, only 2D client?
    SF_BIT_UNKNOWN5, // 0x2000 - added with UO:SA launch, imbuing, mysticism, throwing? (7.0.0.0)
    SF_BIT_NEWMOVE,  // 0x4000 - new movement system (packets 0xF0, 0xF1, 0xF2))
    SF_BIT_FACTIONAREAS = 15, // 0x8000 - Unlock new Felucca faction-areas (map0x.mul?)
    SF_BIT_COUNT
};

enum AssistantFeatures : std::uint64_t {
    AF_NONE = 0,

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

enum cSD_TID {
    tSERVER_ERROR = -1,
    tSERVER_CORPSEDECAY = 0, // Amount of time for a corpse to decay.
    tSERVER_WEATHER,         // Amount of time between changing light levels (day cycles).
    tSERVER_SHOPSPAWN,       // Amount of time between shopkeeper restocks.
    tSERVER_DECAY, // Amount of time a decayable item will remain on the ground before dissapearing.
    tSERVER_DECAYINHOUSE,  // Amount of time a decayable item will remain on the floor of a house
                           // before dissapearing, if not locked down.
    tSERVER_INVISIBILITY,  // Duration of the invisibility spell.
    tSERVER_OBJECTUSAGE,   // Amount of time a player must wait between using objects.
    tSERVER_GATE,          // Duration of a summoned moongate.
    tSERVER_POISON,        // Duration of the poison effect on a character.
    tSERVER_LOGINTIMEOUT,  // Amount of time for an idle connection to time out.
    tSERVER_HITPOINTREGEN, // Amount of time required to regenerate a single point of health.
    tSERVER_STAMINAREGEN,  // Amount of time required to regenerate a single point of stamina.
    tSERVER_MANAREGEN,     // Amount of time required to regenerate a single point of mana.
    tSERVER_FISHINGBASE,   // Base time for fishing skill to complete.
    tSERVER_FISHINGRANDOM, // Max random time for fishing skill to complete.
    tSERVER_SPIRITSPEAK,   // Duration of the spirit speak skill.
    tSERVER_HUNGERRATE,    // Amount of time a player has before his hunger level decreases.
    tSERVER_THIRSTRATE,    // Amount of time a player has before his thirst level decreases.
    tSERVER_POLYMORPH,     // Duration of the polymorph spell.
    tSERVER_ESCORTWAIT,    // Amount of time until an escort NPC will dissapear while waiting for a
                           // player to start his quest.
    tSERVER_ESCORTACTIVE,  // Amount of time until an escort NPC will dissapear while a player is
                           // escorting him.
    tSERVER_ESCORTDONE,    // Amount of time until an escort NPC will dissapear when his quest is
                           // finished.
    tSERVER_MURDERDECAY,   // Amount of time before a permanent murder count will decay.
    tSERVER_CRIMINAL,     // Amount of time a character remains criminal after committing a criminal
                          // act.
    tSERVER_STEALINGFLAG, // Amount of time a character's stealing flag remains active
    tSERVER_AGGRESSORFLAG, // Amount of time a character remains aggressor after committing an
                           // aggressive act
    tSERVER_PERMAGREYFLAG, // Amount of time a permagrey flag remains active after player has stolen
                           // from someone
    tSERVER_COMBATIGNORE,  // Amount of time an NPC will ignore an unreachable target in combat
    tSERVER_POTION,        // Delay between using potions
    tSERVER_PETOFFLINECHECK,    // Delay between checks for the PetOfflineTimeout
    tSERVER_NPCFLAGUPDATETIMER, // Delay in seconds between each time NPC flags are updated
    tSERVER_BLOODDECAY,         // Delay in seconds before blood spatter spawned from combat decays
    tSERVER_BLOODDECAYCORPSE,   // Delay in seconds before blood spawned along with corpses decays
    tSERVER_NPCCORPSEDECAY,     // Delay in seconds before NPC corpses decay
    tSERVER_LOYALTYRATE,        // Amount of time between each time loyalty decreases for a pet
    tSERVER_COUNT
};

enum CSDDirectoryPaths {
    CSDDP_ROOT = 0,
    CSDDP_DATA,
    CSDDP_DEFS,
    CSDDP_ACCESS,
    CSDDP_ACCOUNTS,
    CSDDP_SCRIPTS,
    CSDDP_BACKUP,
    CSDDP_MSGBOARD,
    CSDDP_SHARED,
    CSDDP_HTML,
    CSDDP_LOGS,
    CSDDP_DICTIONARIES,
    CSDDP_BOOKS,
    CSDDP_SCRIPTDATA,
    CSDDP_COUNT
};
struct __STARTLOCATIONDATA__ {
    __STARTLOCATIONDATA__() {
        x = 0;
        y = 0;
        z = 0;
        worldNum = 0;
        instanceId = 0;
        clilocDesc = 0;
    }
    std::string oldTown;
    std::string oldDescription;
    std::string newTown;
    std::string newDescription;
    std::int16_t x;
    std::int16_t y;
    std::int16_t z;
    std::int16_t worldNum;
    std::uint16_t instanceId;
    std::uint32_t clilocDesc;
};
// o------------------------------------------------------------------------------------------------o
//  PhysicalServer
// o------------------------------------------------------------------------------------------------o
// o------------------------------------------------------------------------------------------------o
class PhysicalServer {
  public:
    auto SetName(const std::string &newName) -> void;
    auto SetDomain(const std::string &newDomain) -> void;
    auto SetIP(const std::string &newIP) -> void;
    auto SetPort(std::uint16_t newPort) -> void;
    auto GetName() const -> const std::string &;
    auto GetDomain() const -> const std::string &;
    auto GetIP() const -> const std::string &;
    auto GetPort() const -> std::uint16_t;

  private:
    std::string name;
    std::string domain;
    std::string ip;
    std::uint16_t port;
};

class CServerData {
  private:
    std::bitset<CF_BIT_COUNT> clientFeatures;
    std::bitset<SF_BIT_COUNT> serverFeatures;

    // Once over 62, bitsets are costly.  std::vector<bool> has a special exception in the c++
    // specificaiton, to minimize wasted space for bools These should be updated
    std::bitset<104> boolVals;          // Many values stored this way, rather than using bools.
    std::bitset<64> spawnRegionsFacets; // Used to determine which facets to enable spawn regions
                                        // for, set in UOX>INI

    // ServerSystems
    std::string sServerName; // 04/03/2004 - Need a place to store the name of the server (Added to
                             // support the UOG Info Request)
    std::string commandprefix; //	Character that acts as the command prefix
    std::string externalIP;
    ip4list_t availableIPs;

    std::vector<PhysicalServer> serverList; //	Series of server entries for shard list
    std::uint8_t consoleLogEnabled; //	Various levels of legging 0 == none, 1 == normal, 2 == normal + all
                            // speech
    std::uint16_t serverLanguage;    //	Default language used on server
    std::uint16_t port;              //	Port number that the server listens on, for connections
    std::uint16_t jsEngineSize;      // gcMaxBytes limit in MB per JS runtime
    std::uint16_t apsPerfThreshold; // Performance threshold (simulation cycles) below which APS system kicks
                           // in - 0 disables system
    std::uint16_t apsInterval;      // Interval at which APS checks and optionally makes adjustments based on
                           // shard performance
    std::uint16_t apsDelayStep; // Step value in milliseconds that APS uses to gradually adjust delay for NPC
                       // AI/movement checks
    std::uint16_t apsDelayMaxCap; // Max impact in milliseconds APS can have on NPC AI/movement checks
    std::uint16_t sysMsgColour; // Default text colour for system messages displayed in bottom left corner of
                       // screen
    std::int16_t backupRatio;  //	Number of saves before a backup occurs
    std::uint32_t serverSavesTimer; //	Number of seconds between world saves
    std::uint32_t netRcvTimeout; // 04/03/2004 - Used to be hardcoded as 2 seconds (2 * 1000ms) for some raly
                        // laggy nets this would drop alot of packets, and disconnect people.
    std::uint32_t netSndTimeout; // 04/03/2004 - Not used at this time.
    std::uint32_t netRetryCount; // 04/03/2004 - Used to set the number of times a network recieve will be
                        // attempted before it throws an error
    std::uint32_t maxBytesIn;    // Max bytes that can be received from a client in a 10-second window before
                        // client is warned/kicked for excessive data use
    std::uint32_t maxBytesOut; // Max bytes that can be sent to a client in a 10-second window before client
                      // is warned/kicked for excessive data use
    std::uint32_t trafficTimeban; // Duration in minutes that player will be banned for if they exceed their
                         // network traffic budget

    bool uogEnabled;             // 04/03/2004 - Added to support the UOG Info Request Service
    bool randomStartingLocation; // Enable or disable randomizing starting location for new players
                                 // based on starting location entries
    bool useUnicodeMessages; // Enable or disable sending messages originating on server in Unicode
                             // format

    // Client Support
    bool Clients4000Enabled;  // Allow client connections from 4.0.0 to 4.0.11f
    bool Clients5000Enabled;  // Allow client connections from 5.0.0.0 to 5.0.8.2 (Mondain's Legacy)
    bool Clients6000Enabled;  // Allow client connections from 6.0.0.0 to 6.0.4.0
    bool Clients6050Enabled;  // Allow client connections from 6.0.5.0 to 6.0.14.2
    bool Clients7000Enabled;  // Allow client connections from 7.0.0.0 to 7.0.8.2
    bool Clients7090Enabled;  // Allow client connections from 7.0.9.0 to 7.0.15.1 (High Seas)
    bool Clients70160Enabled; // Allow client connections from 7.0.16.0 to 7.0.23.1
    bool Clients70240Enabled; // Allow client connections from 7.0.24.0+
    bool Clients70300Enabled; // Allow client connections from 7.0.30.0+
    bool Clients70331Enabled; // Allow client connections from 7.0.33.1+
    bool Clients704565Enabled; // Allow client connections from 7.0.45.65+ (Time of Legends)
    bool Clients70610Enabled;  // Allow client connections from 7.0.61.0+ (Endless Journey)

    // facet block
    bool useFacetSaves;
    std::vector<std::string> facetNameList;
    std::vector<std::uint32_t> facetAccessFlags;

    // Skills & Stats
    std::uint8_t skillDelay;    //	Number of seconds after a skill is used before another skill can be used
    std::uint16_t skillTotalCap; //	A cap on the total of all a PC's skills
    std::uint16_t skillCap;      //	A cap on each individual PC skill
    std::uint16_t statCap;       //	A cap on the total of a PC's stats
    std::int16_t maxStealthMovement; //	Max number of steps allowed with stealth skill at 100.0
    std::int16_t maxStaminaMovement; //	Max number of steps allowed while running before stamina is reduced

    // ServerTimers
    // array
    std::uint16_t serverTimers[tSERVER_COUNT];
    // Directories
    // array
    std::string serverDirectories[CSDDP_COUNT];

    std::string actualINI; // 	The actual uox.ini file loaded, used for saveing

    std::string secretShardKey; // Secret shard key used to only allow connection from specific
                                // custom clients with matching key

    // Expansion
    // 0 = core, 1 = UO, 2 = T2A, 3 = UOR, 4 = TD, 5 = LBR (Pub15), 6 = AoS, 7 = SE, 8 = ML, 9 = SA,
    // 10 = HS, 11 = ToL
    std::uint8_t coreShardEra; // Determines core era ruleset for shard (determines which items/npcs are
                       // loaded, and which rules are applied in combat)
    std::uint8_t expansionArmorCalculation; // Determines which era ruleset to use for calculating armor and
                                    // defense
    std::uint8_t expansionStrengthDamageBonus; // Determines which era ruleset to use for calculating
                                       // strength damage bonus
    std::uint8_t expansionTacticsDamageBonus; // Determines which era ruleset to use for calculating tactics
                                      // damage bonus
    std::uint8_t expansionAnatomyDamageBonus; // Determines which era ruleset to use for calculating anatomy
                                      // damage bonus
    std::uint8_t expansionLumberjackDamageBonus; // Determines which era ruleset to use for calculating
                                         // lumberjack damage bonus
    std::uint8_t expansionRacialDamageBonus; // Determines which era ruleset to use for calculating racial
                                     // damage bonus
    std::uint8_t expansionDamageBonusCap;    // Determines which era ruleset to use for calculating damage
                                     // bonus cap
    std::uint8_t expansionShieldParry; // Determines which era ruleset to use for shield parry calculations
    std::uint8_t expansionWeaponParry; // Determines which era ruleset to use for weapon parry calculations
    std::uint8_t expansionWrestlingParry;  // Determines which era ruleset to use for wrestling parry
                                   // calculations
    std::uint8_t expansionCombatHitChance; // Determines which era ruleset to use for calculating melee hit
                                   // chance

    // Settings
    std::int16_t ambientSounds;      //	Ambient sounds - values from 1->10 - higher values indicate sounds
                             // occur less often
    std::int16_t htmlStatusEnabled;  //	If > 0 then it's enabled - only used at PC char creation - use
                             // elsewhere? (was # of seconds between updates)
    std::int16_t sellMaxItems;       //	Maximum number of items that can be sold to a vendor
    std::int16_t fishingstaminaloss; //	The amount of stamina lost with each use of fishing skill
    std::uint8_t maxControlSlots;    //	The default max amount of pet/follower control slots for each player
    std::uint8_t maxSafeTeleports;   //	The default max amount of free teleports to safety players get via
                             // the help menu per day
    std::uint8_t maxPetOwners;       //	The default max amount of different owners a pet may have in its
                             // lifetime
    std::uint8_t maxFollowers;       //	The default max amount of followers a player can have active
    std::uint16_t petOfflineTimeout;  //	Offline time after a player looses all pets
    std::uint16_t maxPlayerPackItems; //	The max amount of items a player's backpack can contain
    std::uint16_t maxPlayerBankItems; //	The max amount of items a player's bankbox can contain
    std::int32_t maxPlayerPackWeight;     //	The max weight capacity of a player's backpack (including
                                  // subcontainers)
    std::int32_t maxPlayerBankWeight;     //	The max weight capacity of a player's bankbox (including
                                  // subcontainers)
    R32 weightPerSTR;             //	How much weight per point of STR a character can hold.
    bool paperdollGuildButton;    //	Enable Guild-button on paperdoll to access guild-menus
                                  // without going through guildstone
    std::uint16_t petLoyaltyGainOnSuccess; //	The default amount of pet loyalty gained on successful use
                                  // of a pet command
    std::uint16_t petLoyaltyLossOnFailure; //	The default amount of pet loyalty lost on a failed attempt
                                  // to use a pet command

    // SpeedUp
    R32 npcWalkingSpeed;        //	Speed at which walking NPCs move
    R32 npcRunningSpeed;        //	Speed at which running NPCs move
    R32 npcFleeingSpeed;        //	Speed at which fleeing NPCs move
    R32 npcMountedWalkingSpeed; //	Speed at which (mounted) walking NPCs move
    R32 npcMountedRunningSpeed; //	Speed at which (mounted) running NPCs move
    R32 npcMountedFleeingSpeed; //	Speed at which (mounted) fleeing NPCs move
    R32 archeryShootDelay; //  Attack delay for archers; after coming to a full stop, they need to
                           //  wait this amount of time before they can fire an arrow. Defaults
                           //  to 1.0 seconds
    R32 globalAttackSpeed; //  Global attack speed that can be tweaked to quickly increase or
                           //  decrease overall combat speed. Defaults to 1.0
    R32 npcSpellcastSpeed; //  For adjusting the overall speed of (or delay between) NPC spell
                           //  casts. Defaults to 1.0
    R32 globalRestockMultiplier; //	Global multiplier applied to restock properties of items
                                 // when loaded from DFNs
    R32 bodGoldRewardMultiplier; //	Multiplier that adjusts the amount of Gold rewarded by
                                 // completing Bulk Order Deeds
    R32 bodFameRewardMultiplier; //	Multiplier that adjusts the amount of Fame rewarded by
                                 // completing Bulk Order Deeds
    R64 checkItems;        //	How often (in seconds) items are checked for decay and other things
    R64 checkBoats;        //	How often (in seconds) boats are checked for motion and so forth
    R64 checkNpcAi;        //	How often (in seconds) NPCs can execute an AI routine
    R64 checkSpawnRegions; //	How often (in seconds) spawn regions are checked for new spawns
    R64 flushTime; //	How often (in minutes) online accounts are checked to see if they really ARE
                   // online

    // MessageBoards
    std::uint8_t msgPostingLevel; //	If not 0, then players can post
    std::uint8_t msgRemovalLevel; //	If not 0, then players can remove posts

    // WorldLight
    lightlevel_t dungeonLightLevel; //	Default light level for a dungeon, if not subject to a
                                  // weather system
    lightlevel_t currentLightLevel; //	Default current light level if not subject to a weather system
    lightlevel_t brightnessLightLevel; //	Default brightest light level if not subject to a weather
                                     // system
    lightlevel_t darknessLightLevel; //	Default darkest light level if not subject to a weather system

    // WorldTimer								//	days/hours/minutes/seconds to it's
    // own file?
    std::uint16_t secondsPerUoMinute; //	Number of seconds for a UOX minute.
    std::uint8_t seconds;            //	Number of seconds the world has been running (persistent)
    std::uint8_t minutes;            //	Number of minutes the world has been running (persistent)
    std::uint8_t hours;              //	Number of hours the world has been running (persistent)
    std::int16_t days;               //	Number of days the world has been running (persistent)
    std::int16_t moon[2];            //	Moon current state
    bool ampm;               //	Whether our current time is in the morning or afternoon

    // Tracking
    std::uint8_t trackingMaxTargets;        //	Maximum number of targets that can be tracked
    std::uint16_t trackingBaseRange;         //	Minimum range even a novice can track from
    std::uint16_t trackingBaseTimer;         //	Tracking timer - maximum time for a GM to track at
    std::uint16_t trackingMsgRedisplayTimer; //	How often (in seconds) the tracking message is redisplayed

    // Reputation
    std::uint16_t maxMurdersAllowed; //	Maximum number of kills before turning red

    // Resources
    std::uint8_t mineCheck; //	Type of mining check performed - 0 anywhere 1 only mountains/floors 2 region
                    // based (not working)
    std::uint16_t resourceAreaSize; //	Size of each resource area to split each world into
    std::uint16_t oreRespawnTimer;  //	Time at which ore is respawned (only 1 ore, not all)
    std::uint16_t logsRespawnTimer; //	TIMERVAL? Time at which logs are respawned (only 1 log, not all)
    std::uint16_t fishRespawnTimer; //	TIMERVAL? Time at which fish are respawned (only 1 fish, not all)
    std::int16_t logsPerArea;      //	Maximum number of logs in a resource area
    std::int16_t orePerArea;       //	TIMERVAL? Maximum number of ores in a resource area
    std::int16_t fishPerArea;      //	Maximum number of fish in a resource area

    // Hunger & Food
    std::int16_t hungerDamage; //	Amount of damage applied if hungry and below threshold
    // Thirst
    std::int16_t thirstDrain; //  Amount of stamina drained if thirsty and below threshold

    // Combat
    std::uint8_t combatAnimalAttackChance; //	Chance of animals being attacked (0-100)
    std::uint8_t combatWeaponDamageChance; //  Chance of weapons being damaged when attacking in combat
                                   //  (0-100)
    std::uint8_t combatWeaponDamageMin; //  Minimum amount of hitpoints a weapon can lose when being damaged
                                //  in combat
    std::uint8_t combatWeaponDamageMax; //  Maximum amount of hitpoints a weapon can lose when being damaged
                                //  in combat
    std::uint8_t combatArmorDamageChance; //  Chance of armor being damaged when defending in combat (0-100)
    std::uint8_t combatArmorDamageMin; //  Minimum amount of hitpoints an armor can lose when being damaged
                               //  in combat
    std::uint8_t combatArmorDamageMax; //  Maximum amount of hitpoints an armor can lose when being damaged
                               //  in combat
    std::uint8_t combatParryDamageChance; //  Chance of shield being damaged when parrying in combat (0-100)
    std::uint8_t combatParryDamageMin;    //  Minimum amount of hitpoints a shield/weapon can lose when
                                  //  successfully parrying in combat
    std::uint8_t combatParryDamageMax;    //  Maximum amount of hitpoints a shield/weapon can lose when
                                  //  successfully parrying in combat
    std::uint8_t combatBloodEffectChance; //  Chance of blood splatter effects spawning during combat
    std::uint8_t alchemyDamageBonusModifier;  //  Modifier used to calculate bonus damage for explosion
                                      //  potions based on alchemy skill
    std::uint8_t combatWeaponDamageBonusType; //  Weapon damage bonus type (0 - apply to hidamage, 1 - split
                                      //  between lo and hi, 2 - apply equally to lo and hi
    std::int8_t combatArcheryHitBonus;   //  Bonus to hit chance for Archery skill in combat, applied after
                                  //  regular hit chance calculation
    std::int16_t combatMaxRange;          //	RANGE?  Range at which combat can actually occur
    std::int16_t combatMaxSpellRange;     //	RANGE?  Range at which spells can be cast
    std::int16_t combatNpcDamageRate;     //	NPC Damage divisor - PCs sustain less than NPCs.  If a PC,
                                  // damage is 1/value
    std::int16_t combatNpcBaseFleeAt;     //	% of HP where an NPC will flee, if it's not defined for them
    std::int16_t combatNpcBaseReattackAt; //	% of HP where an NPC will resume attacking
    std::int16_t combatAttackStamina;     //	Amount of stamina lost when hitting an opponent

    // Start & Location Settings
    std::vector<__STARTLOCATIONDATA__> startlocations;
    std::vector<__STARTLOCATIONDATA__> youngStartlocations;
    std::uint16_t startPrivs; //	Starting privileges of characters
    std::int16_t startGold;  //	Amount of gold created when a PC is made

    // Anything under this comment is left here for symantics
    std::uint8_t skillLevel;   //	Some skill value associated with the rank system
    std::int16_t buyThreshold; //	Value above which money will be sourced from the bank rather than
                       // the player

    // Gump stuff
    std::uint16_t titleColour;     //	Default text colour for titles in gumps
    std::uint16_t leftTextColour;  //	Default text colour for left text in gumps (2 column ones)
    std::uint16_t rightTextColour; //	Default text colour for right text in gumps
    std::uint16_t buttonCancel;    //	Default Button ID for cancel button in gumps
    std::uint16_t buttonLeft;      //	Default Button ID for left button in gumps
    std::uint16_t buttonRight;     //	Default Button ID for right button in gumps
    std::uint16_t backgroundPic;   //	Default Gump ID for background gump

    // Houses
    std::uint16_t maxHousesOwnable;   //	Max amount of houses that a player can own
    std::uint16_t maxHousesCoOwnable; //	Max amount of houses that a player can co-own

    // Townstone stuff
    std::uint32_t numSecsPollOpen; //	Time (in seconds) for which a town voting poll is open
    std::uint32_t numSecsAsMayor;  //	Time (in seconds) that a PC would be a mayor
    std::uint32_t taxPeriod;       //	Time (in seconds) between periods of taxes for PCs
    std::uint32_t guardPayment;    //	Time (in seconds) between payments for guards

    void PostLoadDefaults();
    static const std::map<std::string, std::int32_t> uox3IniCaseValue;

  public:
    std::uint64_t DisabledAssistantFeatures;

    auto LookupINIValue(const std::string &tag) -> std::int32_t;

    auto SetServerFeature(ServerFeatures, bool) -> void;
    auto SetServerFeatures(size_t) -> void;
    auto GetServerFeature(ServerFeatures) const -> bool;
    auto GetServerFeatures() const -> size_t;

    auto SetClientFeature(ClientFeatures, bool) -> void;
    auto SetClientFeatures(std::uint32_t) -> void;
    auto GetClientFeature(ClientFeatures) const -> bool;
    std::uint32_t GetClientFeatures() const;

    auto SetDisabledAssistantFeature(AssistantFeatures, bool) -> void;
    auto SetDisabledAssistantFeatures(std::uint64_t) -> void;
    auto GetDisabledAssistantFeature(AssistantFeatures) const -> bool;
    std::uint64_t GetDisabledAssistantFeatures() const;

    auto SetAssistantNegotiation(bool value) -> void;
    auto GetAssistantNegotiation() const -> bool;

    auto GetSpawnRegionsFacetStatus(std::uint32_t value) const -> bool;
    auto SetSpawnRegionsFacetStatus(std::uint32_t value, bool status) -> void;
    std::uint32_t GetSpawnRegionsFacetStatus() const;
    auto SetSpawnRegionsFacetStatus(std::uint32_t value) -> void;

    auto SetClassicUOMapTracker(bool value) -> void;
    auto GetClassicUOMapTracker() const -> bool;

    auto UseUnicodeMessages(bool value) -> void;
    auto UseUnicodeMessages() const -> bool;

    std::int16_t ServerMoon(std::int16_t slot) const;
    lightlevel_t WorldLightDarkLevel() const;
    lightlevel_t WorldLightBrightLevel() const;
    lightlevel_t WorldLightCurrentLevel() const;
    lightlevel_t DungeonLightLevel() const;
    auto ServerStartPrivs() const -> std::uint16_t;
    std::int16_t ServerStartGold() const;

    auto ServerMoon(std::int16_t slot, std::int16_t value) -> void;
    auto WorldLightDarkLevel(lightlevel_t value) -> void;
    auto WorldLightBrightLevel(lightlevel_t value) -> void;
    auto WorldLightCurrentLevel(lightlevel_t value) -> void;
    auto DungeonLightLevel(lightlevel_t value) -> void;
    auto ServerStartPrivs(std::uint16_t value) -> void;
    auto ServerStartGold(std::int16_t value) -> void;
    auto ParseIni(const std::string &filename) -> bool;
    auto HandleLine(const std::string &tag, const std::string &value) -> bool;

    auto Load(const std::string &filename = "") -> bool;
    auto SaveIni() -> bool;
    auto SaveIni(const std::string &filename) -> bool;

    auto EraEnumToString(ExpansionRuleset eraNum, bool coreEnum = false) -> std::string;
    auto EraStringToEnum(const std::string &eraString, bool useDefault = true,
                         bool inheritCore = true) -> ExpansionRuleset;

    auto ResetDefaults() -> void;
    auto Startup() -> void;
    // void		RefreshIPs( void );

    CServerData();
    auto ServerName(const std::string &setname) -> void;
    auto SecretShardKey(const std::string &newName) -> void;
    auto ServerDomain(const std::string &setdomain) -> void;
    auto ServerIP(const std::string &setip) -> void;
    auto ServerName() const -> const std::string &;
    auto SecretShardKey() const -> const std::string &;
    auto ServerDomain() const -> const std::string &;
    auto ServerIP() const -> const std::string &;
    auto ExternalIP() const -> const std::string &;
    auto ExternalIP(const std::string &ip) -> void;
    auto ServerPort(std::uint16_t setport) -> void;
    auto ServerPort() const -> std::uint16_t;
    auto ServerConsoleLog(bool setting) -> void;
    auto ServerConsoleLog() const -> bool;
    auto ServerNetworkLog(bool setting) -> void;
    auto ServerNetworkLog() const -> bool;
    auto ServerSpeechLog(bool setting) -> void;
    auto ServerSpeechLog() const -> bool;
    auto ServerCommandPrefix(char cmdvalue) -> void;
    char ServerCommandPrefix() const;
    auto ServerAnnounceSaves(bool setting) -> void;
    auto ServerAnnounceSavesStatus() const -> bool;
    auto ServerJoinPartAnnouncements(bool setting) -> void;
    auto ServerJoinPartAnnouncementsStatus() const -> bool;
    auto ServerMulCaching(bool setting) -> void;
    auto ServerMulCachingStatus() const -> bool;
    auto ServerBackups(bool setting) -> void;
    auto ServerBackupStatus() const -> bool;
    auto ServerContextMenus(bool setting) -> void;
    auto ServerContextMenus() const -> bool;
    auto ServerSavesTimer(std::uint32_t timer) -> void;
    std::uint32_t ServerSavesTimerStatus() const;
    auto ServerMainThreadTimer(std::uint32_t threadtimer) -> void;
    std::uint32_t ServerMainThreadTimerStatus() const;
    auto ServerSkillTotalCap(std::uint16_t cap) -> void;
    auto ServerSkillTotalCapStatus() const -> std::uint16_t;
    auto ServerSkillCap(std::uint16_t cap) -> void;
    auto ServerSkillCapStatus() const -> std::uint16_t;
    auto ServerSkillDelay(std::uint8_t skdelay) -> void;
    std::uint8_t ServerSkillDelayStatus() const;
    auto ServerStatCap(std::uint16_t cap) -> void;
    auto ServerStatCapStatus() const -> std::uint16_t;
    auto MaxStealthMovement(std::int16_t value) -> void;
    std::int16_t MaxStealthMovement() const;
    auto MaxStaminaMovement(std::int16_t value) -> void;
    std::int16_t MaxStaminaMovement() const;
    auto SystemTimer(cSD_TID timerId, std::uint16_t value) -> void;
    auto SystemTimer(cSD_TID timerId) const -> std::uint16_t;
    timerval_t BuildSystemTimeValue(cSD_TID timerId) const;
    auto SysMsgColour(std::uint16_t value) -> void;
    auto SysMsgColour() const -> std::uint16_t;

    auto ServerUOGEnabled() const -> bool { return uogEnabled; }
    auto ServerUOGEnabled(bool uogValue) -> void { uogEnabled = uogValue; }
    auto FreeshardServerPoll(bool value) -> void;
    auto FreeshardServerPoll() const -> bool;
    auto ServerRandomStartingLocation() const -> bool { return randomStartingLocation; }
    auto ServerRandomStartingLocation(bool rndStartLocValue) -> void {
        randomStartingLocation = rndStartLocValue;
    }
    std::uint32_t ServerNetRetryCount() const { return netRetryCount; }
    auto ServerNetRetryCount(std::uint32_t retryValue) -> void { netRetryCount = retryValue; }
    std::uint32_t ServerNetRcvTimeout() const { return netRcvTimeout; }
    auto ServerNetRcvTimeout(std::uint32_t timeoutValue) -> void { netRcvTimeout = timeoutValue; }
    std::uint32_t ServerNetSndTimeout() const { return netSndTimeout; }
    auto ServerNetSndTimeout(std::uint32_t timeoutValue) -> void { netSndTimeout = timeoutValue; }

    // ClientSupport used to determine login-restrictions
    auto ClientSupport4000() const -> bool { return Clients4000Enabled; }
    auto ClientSupport4000(bool cliSuppValue) -> void { Clients4000Enabled = cliSuppValue; }
    auto ClientSupport5000() const -> bool { return Clients5000Enabled; }
    auto ClientSupport5000(bool cliSuppValue) -> void { Clients5000Enabled = cliSuppValue; }
    auto ClientSupport6000() const -> bool { return Clients6000Enabled; }
    auto ClientSupport6000(bool cliSuppValue) -> void { Clients6000Enabled = cliSuppValue; }
    auto ClientSupport6050() const -> bool { return Clients6050Enabled; }
    auto ClientSupport6050(bool cliSuppValue) -> void { Clients6050Enabled = cliSuppValue; }
    auto ClientSupport7000() const -> bool { return Clients7000Enabled; }
    auto ClientSupport7000(bool cliSuppValue) -> void { Clients7000Enabled = cliSuppValue; }
    auto ClientSupport7090() const -> bool { return Clients7090Enabled; }
    auto ClientSupport7090(bool cliSuppValue) -> void { Clients7090Enabled = cliSuppValue; }
    auto ClientSupport70160() const -> bool { return Clients70160Enabled; }
    auto ClientSupport70160(bool cliSuppValue) -> void { Clients70160Enabled = cliSuppValue; }
    auto ClientSupport70240() const -> bool { return Clients70240Enabled; }
    auto ClientSupport70240(bool cliSuppValue) -> void { Clients70240Enabled = cliSuppValue; }
    auto ClientSupport70300() const -> bool { return Clients70300Enabled; }
    auto ClientSupport70300(bool cliSuppValue) -> void { Clients70300Enabled = cliSuppValue; }
    auto ClientSupport70331() const -> bool { return Clients70331Enabled; }
    auto ClientSupport70331(bool cliSuppValue) -> void { Clients70331Enabled = cliSuppValue; }
    auto ClientSupport704565() const -> bool { return Clients704565Enabled; }
    auto ClientSupport704565(bool cliSuppValue) -> void { Clients704565Enabled = cliSuppValue; }
    auto ClientSupport70610() const -> bool { return Clients70610Enabled; }
    auto ClientSupport70610(bool cliSuppValue) -> void { Clients70610Enabled = cliSuppValue; }

    auto StatsAffectSkillChecks(bool setting) -> void;
    auto StatsAffectSkillChecks() const -> bool;

    // Enable/disable higher total for starting stats, and/or fourth starting skill in
    // clients 7.0.16+
    auto ExtendedStartingStats(bool setting) -> void;
    auto ExtendedStartingStats() const -> bool;
    auto ExtendedStartingSkills(bool setting) -> void;
    auto ExtendedStartingSkills() const -> bool;

    // Define all Path Get/Set's here please
    auto Directory(CSDDirectoryPaths dp, std::string value) -> void;
    std::string Directory(CSDDirectoryPaths dp);

    auto CorpseLootDecay(bool value) -> void;
    auto CorpseLootDecay() const -> bool;

    auto GuardStatus(bool value) -> void;
    auto GuardsStatus() const -> bool;

    auto DeathAnimationStatus(bool value) -> void;
    auto DeathAnimationStatus() const -> bool;

    auto WorldAmbientSounds(std::int16_t value) -> void;
    std::int16_t WorldAmbientSounds() const;

    auto AmbientFootsteps(bool value) -> void;
    auto AmbientFootsteps() const -> bool;

    auto InternalAccountStatus(bool value) -> void;
    auto InternalAccountStatus() const -> bool;

    auto YoungPlayerSystem(bool value) -> void;
    auto YoungPlayerSystem() const -> bool;

    auto ShowOfflinePCs(bool value) -> void;
    auto ShowOfflinePCs() const -> bool;

    auto RogueStatus(bool value) -> void;
    auto RogueStatus() const -> bool;

    auto SnoopIsCrime(bool value) -> void;
    auto SnoopIsCrime() const -> bool;

    auto SnoopAwareness(bool value) -> void;
    auto SnoopAwareness() const -> bool;

    auto PlayerPersecutionStatus(bool value) -> void;
    auto PlayerPersecutionStatus() const -> bool;

    auto HtmlStatsStatus(std::int16_t value) -> void;
    std::int16_t HtmlStatsStatus() const;

    auto SellByNameStatus(bool value) -> void;
    auto SellByNameStatus() const -> bool;

    auto SellMaxItemsStatus(std::int16_t value) -> void;
    std::int16_t SellMaxItemsStatus() const;

    auto TradeSystemStatus(bool value) -> void;
    auto TradeSystemStatus() const -> bool;

    auto RankSystemStatus(bool value) -> void;
    auto RankSystemStatus() const -> bool;

    void DisplayMakersMark(bool value);
    bool DisplayMakersMark(void) const;

    auto CutScrollRequirementStatus(bool value) -> void;
    auto CutScrollRequirementStatus() const -> bool;

    auto CheckPetControlDifficulty(bool value) -> void;
    auto CheckPetControlDifficulty() const -> bool;

    auto NPCTrainingStatus(bool setting) -> void;
    auto NPCTrainingStatus() const -> bool;

    auto CheckItemsSpeed(R64 value) -> void;
    R64 CheckItemsSpeed() const;

    auto CheckBoatSpeed(R64 value) -> void;
    R64 CheckBoatSpeed() const;

    auto CheckNpcAISpeed(R64 value) -> void;
    R64 CheckNpcAISpeed() const;

    auto CheckSpawnRegionSpeed(R64 value) -> void;
    R64 CheckSpawnRegionSpeed() const;

    auto GlobalAttackSpeed(R32 value) -> void;
    R32 GlobalAttackSpeed() const;

    auto NPCSpellCastSpeed(R32 value) -> void;
    R32 NPCSpellCastSpeed() const;

    auto GlobalRestockMultiplier(R32 value) -> void;
    R32 GlobalRestockMultiplier() const;

    auto BODGoldRewardMultiplier(R32 value) -> void;
    R32 BODGoldRewardMultiplier() const;

    auto BODFameRewardMultiplier(R32 value) -> void;
    R32 BODFameRewardMultiplier() const;

    auto MsgBoardPostingLevel(std::uint8_t value) -> void;
    std::uint8_t MsgBoardPostingLevel() const;

    auto MsgBoardPostRemovalLevel(std::uint8_t value) -> void;
    std::uint8_t MsgBoardPostRemovalLevel() const;

    auto MineCheck(std::uint8_t value) -> void;
    std::uint8_t MineCheck() const;

    auto AlchemyDamageBonusEnabled(bool value) -> void;
    auto AlchemyDamageBonusEnabled() const -> bool;

    auto AlchemyDamageBonusModifier(std::uint8_t value) -> void;
    std::uint8_t AlchemyDamageBonusModifier() const;

    auto WeaponDamageBonusType(std::uint8_t value) -> void;
    std::uint8_t WeaponDamageBonusType() const;

    auto ItemsInterruptCasting(bool value) -> void;
    auto ItemsInterruptCasting() const -> bool;

    auto CombatArmorClassDamageBonus(bool value) -> void;
    auto CombatArmorClassDamageBonus() const -> bool;

    auto CombatDisplayHitMessage(bool value) -> void;
    auto CombatDisplayHitMessage() const -> bool;

    auto CombatDisplayDamageNumbers(bool value) -> void;
    auto CombatDisplayDamageNumbers() const -> bool;

    auto CombatAttackSpeedFromStamina(bool value) -> void;
    auto CombatAttackSpeedFromStamina() const -> bool;

    auto FishingStaminaLoss(std::int16_t value) -> void;
    std::int16_t FishingStaminaLoss() const;

    auto CombatAttackStamina(std::int16_t value) -> void;
    std::int16_t CombatAttackStamina() const;

    auto CombatNpcDamageRate(std::int16_t value) -> void;
    std::int16_t CombatNpcDamageRate() const;

    std::uint8_t SkillLevel() const;
    auto SkillLevel(std::uint8_t value) -> void;

    auto EscortsEnabled(bool value) -> void;
    auto EscortsEnabled() const -> bool;

    auto ItemsDetectSpeech(bool value) -> void;
    auto ItemsDetectSpeech() const -> bool;

    auto ForceNewAnimationPacket(bool value) -> void;
    auto ForceNewAnimationPacket() const -> bool;

    auto MapDiffsEnabled(bool value) -> void;
    auto MapDiffsEnabled() const -> bool;

    auto MaxPlayerPackItems(std::uint16_t value) -> void;
    auto MaxPlayerPackItems() const -> std::uint16_t;

    auto MaxPlayerPackWeight(std::int32_t newVal) -> void;
    auto MaxPlayerPackWeight() const -> std::int32_t;

    auto MaxPlayerBankItems(std::uint16_t value) -> void;
    auto MaxPlayerBankItems() const -> std::uint16_t;

    auto MaxPlayerBankWeight(std::int32_t newVal) -> void;
    auto MaxPlayerBankWeight() const -> std::int32_t;

    auto BasicTooltipsOnly(bool value) -> void;
    auto BasicTooltipsOnly() const -> bool;

    auto ShowNpcTitlesInTooltips(bool value) -> void;
    auto ShowNpcTitlesInTooltips() const -> bool;

    auto ShowNpcTitlesOverhead(bool value) -> void;
    auto ShowNpcTitlesOverhead() const -> bool;

    auto ShowInvulnerableTagOverhead(bool value) -> void;
    auto ShowInvulnerableTagOverhead() const -> bool;

    auto ShowRaceWithName(bool value) -> void;
    auto ShowRaceWithName() const -> bool;

    auto ShowRaceInPaperdoll(bool value) -> void;
    auto ShowRaceInPaperdoll() const -> bool;

    auto ShowGuildInfoInTooltip(bool value) -> void;
    auto ShowGuildInfoInTooltip() const -> bool;

    auto ShowReputationTitleInTooltip(bool value) -> void;
    auto ShowReputationTitleInTooltip() const -> bool;

    auto EnableNPCGuildDiscounts(bool value) -> void;
    auto EnableNPCGuildDiscounts() const -> bool;

    auto EnableNPCGuildPremiums(bool value) -> void;
    auto EnableNPCGuildPremiums() const -> bool;

    auto CastSpellsWhileMoving(bool value) -> void;
    auto CastSpellsWhileMoving() const -> bool;

    auto PetCombatTraining(bool value) -> void;
    auto PetCombatTraining() const -> bool;

    auto HirelingCombatTraining(bool value) -> void;
    auto HirelingCombatTraining() const -> bool;

    auto NpcCombatTraining(bool value) -> void;
    auto NpcCombatTraining() const -> bool;

    auto ShowItemResistStats(bool value) -> void;
    auto ShowItemResistStats() const -> bool;

    auto ShowWeaponDamageTypes(bool value) -> void;
    auto ShowWeaponDamageTypes() const -> bool;

    auto GlobalItemDecay(bool value) -> void;
    auto GlobalItemDecay() const -> bool;

    auto ScriptItemsDecayable(bool value) -> void;
    auto ScriptItemsDecayable() const -> bool;

    auto BaseItemsDecayable(bool value) -> void;
    auto BaseItemsDecayable() const -> bool;

    auto ItemDecayInHouses(bool value) -> void;
    auto ItemDecayInHouses() const -> bool;

    auto ProtectPrivateHouses(bool value) -> void;
    auto ProtectPrivateHouses() const -> bool;

    auto TrackHousesPerAccount(bool value) -> void;
    auto TrackHousesPerAccount() const -> bool;

    auto CanOwnAndCoOwnHouses(bool value) -> void;
    auto CanOwnAndCoOwnHouses() const -> bool;

    auto CoOwnHousesOnSameAccount(bool value) -> void;
    auto CoOwnHousesOnSameAccount() const -> bool;

    auto SafeCoOwnerLogout(bool value) -> void;
    auto SafeCoOwnerLogout() const -> bool;

    auto SafeFriendLogout(bool value) -> void;
    auto SafeFriendLogout() const -> bool;

    auto SafeGuestLogout(bool value) -> void;
    auto SafeGuestLogout() const -> bool;

    auto KeylessOwnerAccess(bool value) -> void;
    auto KeylessOwnerAccess() const -> bool;

    auto KeylessCoOwnerAccess(bool value) -> void;
    auto KeylessCoOwnerAccess() const -> bool;

    auto KeylessFriendAccess(bool value) -> void;
    auto KeylessFriendAccess() const -> bool;

    auto KeylessGuestAccess(bool value) -> void;
    auto KeylessGuestAccess() const -> bool;

    auto MaxHousesOwnable(std::uint16_t value) -> void;
    auto MaxHousesOwnable() const -> std::uint16_t;

    auto MaxHousesCoOwnable(std::uint16_t value) -> void;
    auto MaxHousesCoOwnable() const -> std::uint16_t;

    auto PaperdollGuildButton(bool value) -> void;
    auto PaperdollGuildButton() const -> bool;

    auto CombatMonstersVsAnimals(bool value) -> void;
    auto CombatMonstersVsAnimals() const -> bool;

    auto CombatAnimalsAttackChance(std::uint16_t value) -> void;
    auto CombatAnimalsAttackChance() const -> std::uint16_t;

    auto CombatArcheryShootDelay(R32 value) -> void;
    R32 CombatArcheryShootDelay() const;

    auto CombatArcheryHitBonus(std::int8_t value) -> void;
    std::int8_t CombatArcheryHitBonus() const;

    auto CombatWeaponDamageChance(std::uint8_t value) -> void;
    std::uint8_t CombatWeaponDamageChance() const;

    auto CombatWeaponDamageMin(std::uint8_t value) -> void;
    std::uint8_t CombatWeaponDamageMin(void) const;

    auto CombatWeaponDamageMax(std::uint8_t value) -> void;
    std::uint8_t CombatWeaponDamageMax() const;

    auto CombatArmorDamageChance(std::uint8_t value) -> void;
    std::uint8_t CombatArmorDamageChance() const;

    auto CombatArmorDamageMin(std::uint8_t value) -> void;
    std::uint8_t CombatArmorDamageMin() const;

    auto CombatArmorDamageMax(std::uint8_t value) -> void;
    std::uint8_t CombatArmorDamageMax() const;

    auto CombatParryDamageChance(std::uint8_t value) -> void;
    std::uint8_t CombatParryDamageChance() const;

    auto CombatParryDamageMin(std::uint8_t value) -> void;
    std::uint8_t CombatParryDamageMin() const;

    auto CombatParryDamageMax(std::uint8_t value) -> void;
    std::uint8_t CombatParryDamageMax() const;

    auto CombatBloodEffectChance(std::uint8_t value) -> void;
    std::uint8_t CombatBloodEffectChance() const;

    auto TravelSpellsFromBoatKeys(bool value) -> void;
    auto TravelSpellsFromBoatKeys() const -> bool;

    auto TravelSpellsWhileOverweight(bool value) -> void;
    auto TravelSpellsWhileOverweight() const -> bool;

    auto MarkRunesInMultis(bool value) -> void;
    auto MarkRunesInMultis() const -> bool;

    auto TravelSpellsBetweenWorlds(bool value) -> void;
    auto TravelSpellsBetweenWorlds() const -> bool;

    auto TravelSpellsWhileAggressor(bool value) -> void;
    auto TravelSpellsWhileAggressor() const -> bool;

    auto ToolUseLimit(bool value) -> void;
    auto ToolUseLimit() const -> bool;

    auto ToolUseBreak(bool value) -> void;
    auto ToolUseBreak() const -> bool;

    auto ItemRepairDurabilityLoss(bool value) -> void;
    auto ItemRepairDurabilityLoss() const -> bool;

    auto HideStatsForUnknownMagicItems(bool value) -> void;
    auto HideStatsForUnknownMagicItems() const -> bool;

    auto CraftColouredWeapons(bool value) -> void;
    auto CraftColouredWeapons() const -> bool;

    auto OfferBODsFromItemSales(bool value) -> void;
    auto OfferBODsFromItemSales() const -> bool;

    auto OfferBODsFromContextMenu(bool value) -> void;
    auto OfferBODsFromContextMenu() const -> bool;

    auto BODsFromCraftedItemsOnly(bool value) -> void;
    auto BODsFromCraftedItemsOnly() const -> bool;

    auto MaxControlSlots(std::uint8_t value) -> void;
    std::uint8_t MaxControlSlots() const;

    auto MaxFollowers(std::uint8_t value) -> void;
    std::uint8_t MaxFollowers() const;

    auto MaxPetOwners(std::uint8_t value) -> void;
    std::uint8_t MaxPetOwners() const;

    auto SetPetLoyaltyGainOnSuccess(std::uint16_t value) -> void;
    auto GetPetLoyaltyGainOnSuccess() const -> std::uint16_t;

    auto SetPetLoyaltyLossOnFailure(std::uint16_t value) -> void;
    auto GetPetLoyaltyLossOnFailure() const -> std::uint16_t;

    auto MaxSafeTeleportsPerDay(std::uint8_t value) -> void;
    std::uint8_t MaxSafeTeleportsPerDay() const;

    auto TeleportToNearestSafeLocation(bool value) -> void;
    auto TeleportToNearestSafeLocation() const -> bool;

    auto AllowAwakeNPCs(bool value) -> void;
    auto AllowAwakeNPCs() const -> bool;

    auto HungerSystemEnabled(bool value) -> void;
    auto HungerSystemEnabled() const -> bool;

    auto ThirstSystemEnabled(bool value) -> void;
    auto ThirstSystemEnabled() const -> bool;

    auto HungerDamage(std::int16_t value) -> void;
    std::int16_t HungerDamage() const;

    auto ThirstDrain(std::int16_t value) -> void;
    std::int16_t ThirstDrain() const;

    auto PetHungerOffline(bool value) -> void;
    auto PetHungerOffline() const -> bool;

    auto PetThirstOffline(bool value) -> void;
    auto PetThirstOffline() const -> bool;

    auto PetOfflineTimeout(std::uint16_t value) -> void;
    auto PetOfflineTimeout() const -> std::uint16_t;

    auto BuyThreshold(std::int16_t value) -> void;
    std::int16_t BuyThreshold() const;

    auto BackupRatio(std::int16_t value) -> void;
    std::int16_t BackupRatio() const;

    auto CombatMaxRange(std::int16_t value) -> void;
    std::int16_t CombatMaxRange() const;

    auto CombatMaxSpellRange(std::int16_t value) -> void;
    std::int16_t CombatMaxSpellRange() const;

    auto CombatAnimalsGuarded(bool value) -> void;
    auto CombatAnimalsGuarded() const -> bool;

    auto CombatNPCBaseFleeAt(std::int16_t value) -> void;
    std::int16_t CombatNPCBaseFleeAt() const;

    void ExpansionCoreShardEra(std::uint8_t value);
    std::uint8_t ExpansionCoreShardEra(void) const;

    void ExpansionArmorCalculation(std::uint8_t value);
    std::uint8_t ExpansionArmorCalculation() const;

    void ExpansionStrengthDamageBonus(std::uint8_t value);
    std::uint8_t ExpansionStrengthDamageBonus() const;

    void ExpansionTacticsDamageBonus(std::uint8_t value);
    std::uint8_t ExpansionTacticsDamageBonus() const;

    void ExpansionAnatomyDamageBonus(std::uint8_t value);
    std::uint8_t ExpansionAnatomyDamageBonus() const;

    void ExpansionLumberjackDamageBonus(std::uint8_t value);
    std::uint8_t ExpansionLumberjackDamageBonus() const;

    void ExpansionRacialDamageBonus(std::uint8_t value);
    std::uint8_t ExpansionRacialDamageBonus() const;

    void ExpansionDamageBonusCap(std::uint8_t value);
    std::uint8_t ExpansionDamageBonusCap() const;

    void ExpansionShieldParry(std::uint8_t value);
    std::uint8_t ExpansionShieldParry() const;

    void ExpansionWeaponParry(std::uint8_t value);
    std::uint8_t ExpansionWeaponParry() const;

    void ExpansionWrestlingParry(std::uint8_t value);
    std::uint8_t ExpansionWrestlingParry() const;

    void ExpansionCombatHitChance(std::uint8_t value);
    std::uint8_t ExpansionCombatHitChance() const;

    auto CombatNPCBaseReattackAt(std::int16_t value) -> void;
    std::int16_t CombatNPCBaseReattackAt() const;

    auto ShootOnAnimalBack(bool setting) -> void;
    auto ShootOnAnimalBack() const -> bool;

    auto NPCWalkingSpeed(R32 value) -> void;
    R32 NPCWalkingSpeed() const;

    auto NPCRunningSpeed(R32 value) -> void;
    R32 NPCRunningSpeed() const;

    auto NPCFleeingSpeed(R32 value) -> void;
    R32 NPCFleeingSpeed() const;

    auto NPCMountedWalkingSpeed(R32 value) -> void;
    R32 NPCMountedWalkingSpeed() const;

    auto NPCMountedRunningSpeed(R32 value) -> void;
    R32 NPCMountedRunningSpeed() const;

    auto NPCMountedFleeingSpeed(R32 value) -> void;
    R32 NPCMountedFleeingSpeed() const;

    auto TitleColour(std::uint16_t value) -> void;
    auto TitleColour() const -> std::uint16_t;

    auto LeftTextColour(std::uint16_t value) -> void;
    auto LeftTextColour() const -> std::uint16_t;

    auto RightTextColour(std::uint16_t value) -> void;
    auto RightTextColour() const -> std::uint16_t;

    auto ButtonCancel(std::uint16_t value) -> void;
    auto ButtonCancel() const -> std::uint16_t;

    auto ButtonLeft(std::uint16_t value) -> void;
    auto ButtonLeft() const -> std::uint16_t;

    auto ButtonRight(std::uint16_t value) -> void;
    auto ButtonRight() const -> std::uint16_t;

    auto BackgroundPic(std::uint16_t value) -> void;
    auto BackgroundPic() const -> std::uint16_t;

    auto TownNumSecsPollOpen(std::uint32_t value) -> void;
    std::uint32_t TownNumSecsPollOpen() const;

    auto TownNumSecsAsMayor(std::uint32_t value) -> void;
    std::uint32_t TownNumSecsAsMayor() const;

    auto TownTaxPeriod(std::uint32_t value) -> void;
    std::uint32_t TownTaxPeriod() const;

    auto TownGuardPayment(std::uint32_t value) -> void;
    std::uint32_t TownGuardPayment() const;

    auto RepMaxKills(std::uint16_t value) -> void;
    auto RepMaxKills() const -> std::uint16_t;

    auto ResLogs(std::int16_t value) -> void;
    std::int16_t ResLogs() const;

    auto ResLogTime(std::uint16_t value) -> void;
    auto ResLogTime() const -> std::uint16_t;

    auto ResOre(std::int16_t value) -> void;
    std::int16_t ResOre() const;

    auto ResOreTime(std::uint16_t value) -> void;
    auto ResOreTime() const -> std::uint16_t;

    auto ResourceAreaSize(std::uint16_t value) -> void;
    auto ResourceAreaSize() const -> std::uint16_t;

    auto ResFish(std::int16_t value) -> void;
    std::int16_t ResFish() const;

    auto ResFishTime(std::uint16_t value) -> void;
    auto ResFishTime() const -> std::uint16_t;

    auto AccountFlushTimer(R64 value) -> void;
    R64 AccountFlushTimer() const;

    auto TrackingBaseRange(std::uint16_t value) -> void;
    auto TrackingBaseRange() const -> std::uint16_t;

    auto TrackingMaxTargets(std::uint8_t value) -> void;
    std::uint8_t TrackingMaxTargets() const;

    auto TrackingBaseTimer(std::uint16_t value) -> void;
    auto TrackingBaseTimer() const -> std::uint16_t;

    auto TrackingRedisplayTime(std::uint16_t value) -> void;
    auto TrackingRedisplayTime() const -> std::uint16_t;

    // Sept 22, 2002 - Support for HideWhileMounted fix.
    auto CharHideWhileMounted(bool value) -> void;
    auto CharHideWhileMounted() const -> bool;

    auto WeightPerStr(R32 newVal) -> void;
    R32 WeightPerStr() const;

    auto ServerOverloadPackets(bool newVal) -> void;
    auto ServerOverloadPackets() const -> bool;

    auto ArmorAffectManaRegen(bool newVal) -> void;
    auto ArmorAffectManaRegen() const -> bool;

    auto AdvancedPathfinding(bool value) -> void;
    auto AdvancedPathfinding() const -> bool;

    auto LootingIsCrime(bool value) -> void;
    auto LootingIsCrime() const -> bool;

    auto KickOnAssistantSilence(bool value) -> void;
    auto KickOnAssistantSilence() const -> bool;

    auto DumpPaths() -> void;

    auto ServerLocation(std::string toSet) -> void;
    auto ServerLocation(size_t locNum) -> __STARTLOCATIONDATA__ *;
    auto NumServerLocations() const -> size_t;

    auto YoungServerLocation(std::string toSet) -> void;
    auto YoungServerLocation(size_t locNum) -> __STARTLOCATIONDATA__ *;
    auto NumYoungServerLocations() const -> size_t;

    auto ServerSecondsPerUOMinute() const -> std::uint16_t;
    auto ServerSecondsPerUOMinute(std::uint16_t newVal) -> void;

    auto ServerLanguage() const -> std::uint16_t;
    auto ServerLanguage(std::uint16_t newVal) -> void;

    std::uint32_t MaxClientBytesIn() const;
    auto MaxClientBytesIn(std::uint32_t newVal) -> void;

    std::uint32_t MaxClientBytesOut() const;
    auto MaxClientBytesOut(std::uint32_t newVal) -> void;

    std::uint32_t NetTrafficTimeban() const;
    auto NetTrafficTimeban(std::uint32_t newVal) -> void;

    auto GetJSEngineSize() const -> std::uint16_t;
    auto SetJSEngineSize(std::uint16_t newVal) -> void;

    auto APSPerfThreshold() const -> std::uint16_t;
    auto APSPerfThreshold(std::uint16_t newVal) -> void;
    auto APSInterval() const -> std::uint16_t;
    auto APSInterval(std::uint16_t newVal) -> void;
    auto APSDelayStep() const -> std::uint16_t;
    auto APSDelayStep(std::uint16_t newVal) -> void;
    auto APSDelayMaxCap() const -> std::uint16_t;
    auto APSDelayMaxCap(std::uint16_t newVal) -> void;

    std::int16_t ServerTimeDay() const;
    std::uint8_t ServerTimeHours() const;
    std::uint8_t ServerTimeMinutes() const;
    std::uint8_t ServerTimeSeconds() const;
    auto ServerTimeAMPM() const -> bool;

    auto ServerTimeDay(std::int16_t nValue) -> void;
    auto ServerTimeHours(std::uint8_t nValue) -> void;
    auto ServerTimeMinutes(std::uint8_t nValue) -> void;
    auto ServerTimeSeconds(std::uint8_t nValue) -> void;
    auto ServerTimeAMPM(bool nValue) -> void;

    auto SaveTime() -> void;
    auto LoadTime() -> void;
    auto LoadTimeTags(std::istream &input) -> void;

    // These functions return true if it's a new day
    auto IncSecond() -> bool;
    auto IncMinute() -> bool;
    auto IncHour() -> bool;
    auto IncDay() -> bool;

    auto IncMoon(std::int32_t mNumber) -> void;

    auto matchIP(const Ip4Addr_st &ip) const -> Ip4Addr_st;

    PhysicalServer *ServerEntry(std::uint16_t entryNum);
    auto ServerCount() const -> std::uint16_t;

  private:
    bool resettingDefaults;
};

#endif
