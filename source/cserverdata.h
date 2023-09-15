
#ifndef __CSERVERDATA__
#define __CSERVERDATA__
#include <array>
#include <bitset>
#include <cstdint>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include "typedefs.h"

#include "ip4address.hpp"
#include "configuration/serverconfig.hpp"
#include "stringutility.hpp"



//======================================================================================================
enum csd_tid_t {
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

//======================================================================================================
// CServerData
//======================================================================================================
class CServerData {
private:
    
    // Once over 62, bitsets are costly.  std::vector<bool> has a special exception in the c++
    // specificaiton, to minimize wasted space for bools These should be updated
    std::bitset<64> spawnRegionsFacets; // Used to determine which facets to enable spawn regions
    // for, set in UOX>INI
    
    // ServerSystems
    ip4list_t availableIPs;
    
//    std::vector<PhysicalServer> serverList; //	Series of server entries for shard list
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
    //std::string serverDirectories[CSDDP_COUNT];
    
    std::string actualINI; // 	The actual uox.ini file loaded, used for saveing
    
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
    lightlevel_t worldDungeonLightLevel; //	Default light level for a dungeon, if not subject to a
    // weather system
    lightlevel_t woldCurrentLightLevel; //	Default current light level if not subject to a weather system
    lightlevel_t worldBrightnessLightLevel; //	Default brightest light level if not subject to a weather
    // system
    lightlevel_t worldDarknessLightLevel; //	Default darkest light level if not subject to a weather system
    
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
    
    static const std::map<std::string, std::int32_t> uox3IniCaseValue;
    void postLoadDefaults();

public:
    std::uint64_t disabledAssistantFeatures;
    
    auto lookupINIValue(const std::string &tag) -> std::int32_t;
    
    
    
    
    
    auto getSpawnRegionsFacetStatus(std::uint32_t value) const -> bool;
    auto setSpawnRegionsFacetStatus(std::uint32_t value, bool status) -> void;
    auto getSpawnRegionsFacetStatus() const ->std::uint32_t ;
    auto setSpawnRegionsFacetStatus(std::uint32_t value) -> void;
        
    std::int16_t serverMoon(std::int16_t slot) const;
    lightlevel_t worldLightDarkLevel() const;
    lightlevel_t worldLightBrightLevel() const;
    lightlevel_t worldLightCurrentLevel() const;
    lightlevel_t dungeonLightLevel() const;
    auto serverStartPrivs() const -> std::uint16_t;
    std::int16_t ServerStartGold() const;
    
    auto serverMoon(std::int16_t slot, std::int16_t value) -> void;
    auto worldLightDarkLevel(lightlevel_t value) -> void;
    auto worldLightBrightLevel(lightlevel_t value) -> void;
    auto worldLightCurrentLevel(lightlevel_t value) -> void;
    auto dungeonLightLevel(lightlevel_t value) -> void;
    auto serverStartPrivs(std::uint16_t value) -> void;
    auto ServerStartGold(std::int16_t value) -> void;
    auto ParseIni(const std::string &filename) -> bool;
    auto HandleLine(const std::string &tag, const std::string &value) -> bool;
    
    auto load(const std::string &filename = "") -> bool;
    auto SaveIni() -> bool;
    auto SaveIni(const std::string &filename) -> bool;
    
//    auto EraEnumToString(expansionruleset_t eraNum, bool coreEnum = false) -> std::string;
//    auto EraStringToEnum(const std::string &eraString, bool useDefault = true, bool inheritCore = true) -> expansionruleset_t;
    
    auto ResetDefaults() -> void;
    auto startup() -> void;
    // void		RefreshIPs( void );
    
    CServerData();
    auto ServerDomain(const std::string &setdomain) -> void;
    auto ServerDomain() const -> const std::string &;
    auto ServerPort(std::uint16_t setport) -> void;
    auto ServerPort() const -> std::uint16_t;
    auto ServerMulCaching(bool setting) -> void;
    auto ServerMulCachingStatus() const -> bool;

    auto ServerSavesTimer(std::uint32_t timer) -> void;
    auto ServerSavesTimerStatus() const ->std::uint32_t ;
    auto ServerMainThreadTimer(std::uint32_t threadtimer) -> void;
    auto ServerMainThreadTimerStatus() const ->std::uint32_t ;
    auto ServerSkillTotalCap(std::uint16_t cap) -> void;
    auto ServerSkillTotalCapStatus() const -> std::uint16_t;
    auto ServerSkillCap(std::uint16_t cap) -> void;
    auto ServerSkillCapStatus() const -> std::uint16_t;
    auto ServerSkillDelay(std::uint8_t skdelay) -> void;
    auto ServerSkillDelayStatus() const ->std::uint8_t ;
    auto ServerStatCap(std::uint16_t cap) -> void;
    auto ServerStatCapStatus() const -> std::uint16_t;
    auto MaxStealthMovement(std::int16_t value) -> void;
    auto MaxStealthMovement() const ->std::int16_t ;
    auto MaxStaminaMovement(std::int16_t value) -> void;
    auto MaxStaminaMovement() const -> std::int16_t ;
    auto SystemTimer(csd_tid_t timerId, std::uint16_t value) -> void;
    auto SystemTimer(csd_tid_t timerId) const -> std::uint16_t;
    auto BuildSystemTimeValue(csd_tid_t timerId) const ->timerval_t ;
    auto SysMsgColour(std::uint16_t value) -> void;
    auto SysMsgColour() const -> std::uint16_t;
    
    auto ServerNetRetryCount() const -> std::uint32_t { return netRetryCount; }
    auto ServerNetRetryCount(std::uint32_t retryValue) -> void { netRetryCount = retryValue; }
    auto ServerNetRcvTimeout() const -> std::uint32_t { return netRcvTimeout; }
    auto ServerNetRcvTimeout(std::uint32_t timeoutValue) -> void { netRcvTimeout = timeoutValue; }
    auto ServerNetSndTimeout() const -> std::uint32_t { return netSndTimeout; }
    auto ServerNetSndTimeout(std::uint32_t timeoutValue) -> void { netSndTimeout = timeoutValue; }
    
    auto WorldAmbientSounds(std::int16_t value) -> void;
    auto WorldAmbientSounds() const -> std::int16_t ;
    
    
    auto SellMaxItemsStatus(std::int16_t value) -> void;
    auto SellMaxItemsStatus() const -> std::int16_t ;
    
    auto CheckItemsSpeed(R64 value) -> void;
    auto CheckItemsSpeed() const -> R64 ;
    
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
    
    auto AlchemyDamageBonusModifier(std::uint8_t value) -> void;
    std::uint8_t AlchemyDamageBonusModifier() const;
    
    auto WeaponDamageBonusType(std::uint8_t value) -> void;
    std::uint8_t WeaponDamageBonusType() const;
    
    auto FishingStaminaLoss(std::int16_t value) -> void;
    std::int16_t FishingStaminaLoss() const;
    
    auto CombatAttackStamina(std::int16_t value) -> void;
    std::int16_t CombatAttackStamina() const;
    
    auto CombatNpcDamageRate(std::int16_t value) -> void;
    std::int16_t CombatNpcDamageRate() const;
    
    std::uint8_t SkillLevel() const;
    auto SkillLevel(std::uint8_t value) -> void;
    
    auto MaxPlayerPackItems(std::uint16_t value) -> void;
    auto MaxPlayerPackItems() const -> std::uint16_t;
    
    auto MaxPlayerPackWeight(std::int32_t newVal) -> void;
    auto MaxPlayerPackWeight() const -> std::int32_t;
    
    auto MaxPlayerBankItems(std::uint16_t value) -> void;
    auto MaxPlayerBankItems() const -> std::uint16_t;
    
    auto MaxPlayerBankWeight(std::int32_t newVal) -> void;
    auto MaxPlayerBankWeight() const -> std::int32_t;
        
    auto MaxHousesOwnable(std::uint16_t value) -> void;
    auto MaxHousesOwnable() const -> std::uint16_t;
    
    auto MaxHousesCoOwnable(std::uint16_t value) -> void;
    auto MaxHousesCoOwnable() const -> std::uint16_t;
    
    auto CombatAnimalsAttackChance(std::uint16_t value) -> void;
    auto CombatAnimalsAttackChance() const -> std::uint16_t;
    
    auto CombatArcheryShootDelay(R32 value) -> void;
    R32 CombatArcheryShootDelay() const;
    
    auto CombatArcheryHitBonus(std::int8_t value) -> void;
    std::int8_t CombatArcheryHitBonus() const;
    
    auto CombatWeaponDamageChance(std::uint8_t value) -> void;
    std::uint8_t CombatWeaponDamageChance() const;
    
    auto CombatWeaponDamageMin(std::uint8_t value) -> void;
    std::uint8_t CombatWeaponDamageMin() const;
    
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
    
    auto HungerDamage(std::int16_t value) -> void;
    std::int16_t HungerDamage() const;
    
    auto ThirstDrain(std::int16_t value) -> void;
    std::int16_t ThirstDrain() const;
    
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
    
    auto CombatNPCBaseFleeAt(std::int16_t value) -> void;
    std::int16_t CombatNPCBaseFleeAt() const;
    
    auto CombatNPCBaseReattackAt(std::int16_t value) -> void;
    std::int16_t CombatNPCBaseReattackAt() const;
    
    
    auto NPCWalkingSpeed(R32 value) -> void;
    R32 NPCWalkingSpeed() const;
    
    auto NPCRunningSpeed(R32 value) -> void;
    R32 NPCRunningSpeed() const;
    
    auto NPCFleeingSpeed(R32 value) -> void;
    R32 NPCFleeingSpeed() const;
    
    auto NPCMountedWalkingSpeed(R32 value) -> void;
    R32 NPCMountedWalkingSpeed() const;
    
    auto NPCMountedRunningSpeed(R32 value) -> void;
    auto NPCMountedRunningSpeed() const ->R32 ;
    
    auto NPCMountedFleeingSpeed(R32 value) -> void;
    auto NPCMountedFleeingSpeed() const ->R32 ;
    
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
    auto TownNumSecsPollOpen() const ->std::uint32_t ;
    
    auto TownNumSecsAsMayor(std::uint32_t value) -> void;
    auto TownNumSecsAsMayor() const ->std::uint32_t ;
    
    auto TownTaxPeriod(std::uint32_t value) -> void;
    auto TownTaxPeriod() const ->std::uint32_t ;
    
    auto TownGuardPayment(std::uint32_t value) -> void;
    auto TownGuardPayment() const ->std::uint32_t ;
    
    auto RepMaxKills(std::uint16_t value) -> void;
    auto RepMaxKills() const -> std::uint16_t;
    
    auto ResLogs(std::int16_t value) -> void;
    auto ResLogs() const -> std::int16_t ;
    
    auto ResLogTime(std::uint16_t value) -> void;
    auto ResLogTime() const -> std::uint16_t;
    
    auto ResOre(std::int16_t value) -> void;
    auto ResOre() const ->std::int16_t ;
    
    auto ResOreTime(std::uint16_t value) -> void;
    auto ResOreTime() const -> std::uint16_t;
    
    auto ResourceAreaSize(std::uint16_t value) -> void;
    auto ResourceAreaSize() const -> std::uint16_t;
    
    auto ResFish(std::int16_t value) -> void;
    auto ResFish() const ->std::int16_t ;
    
    auto ResFishTime(std::uint16_t value) -> void;
    auto ResFishTime() const -> std::uint16_t;
    
    auto AccountFlushTimer(R64 value) -> void;
    auto AccountFlushTimer() const ->R64 ;
    
    auto TrackingBaseRange(std::uint16_t value) -> void;
    auto TrackingBaseRange() const -> std::uint16_t;
    
    auto TrackingMaxTargets(std::uint8_t value) -> void;
    auto TrackingMaxTargets() const ->std::uint8_t ;
    
    auto TrackingBaseTimer(std::uint16_t value) -> void;
    auto TrackingBaseTimer() const -> std::uint16_t;
    
    auto TrackingRedisplayTime(std::uint16_t value) -> void;
    auto TrackingRedisplayTime() const -> std::uint16_t;
    
    auto WeightPerStr(R32 newVal) -> void;
    auto WeightPerStr() const ->R32 ;
    
    auto DumpPaths() -> void;
    
    auto ServerSecondsPerUOMinute() const -> std::uint16_t;
    auto ServerSecondsPerUOMinute(std::uint16_t newVal) -> void;
    
    auto ServerLanguage() const -> std::uint16_t;
    auto ServerLanguage(std::uint16_t newVal) -> void;
    
    auto MaxClientBytesIn() const ->std::uint32_t ;
    auto MaxClientBytesIn(std::uint32_t newVal) -> void;
    
    auto MaxClientBytesOut() const ->std::uint32_t ;
    auto MaxClientBytesOut(std::uint32_t newVal) -> void;
    
    auto NetTrafficTimeban() const ->std::uint32_t ;
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
    
    auto ServerTimeDay() const -> std::int16_t ;
    auto ServerTimeHours() const ->std::uint8_t ;
    auto ServerTimeMinutes() const ->std::uint8_t ;
    auto ServerTimeSeconds() const ->std::uint8_t ;
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
    
    auto matchIP(const IP4Addr &ip) const -> IP4Addr;
    
    
private:
    bool resettingDefaults;
};

#endif
