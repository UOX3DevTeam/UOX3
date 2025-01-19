
#ifndef __CSERVERDATA__
#define __CSERVERDATA__
#include <map>
#include <string>
#include <cstdint>
#include <array>

#include "StringUtility.hpp"
#include "IP4Address.hpp"
//o------------------------------------------------------------------------------------------------o
enum ClientFeatures
{
	CF_BIT_CHAT = 0,		// 0x01
	CF_BIT_UOR,				// 0x02
	CF_BIT_TD,				// 0x04
	CF_BIT_LBR,				// 0x08 - Enables LBR features: mp3s instead of midi, show new LBR monsters
	CF_BIT_AOS,				// 0x10 - Enable AoS monsters/map, AoS skills, Necro/Pala/Fight book stuff - works for 4.0+
	CF_BIT_SIXCHARS,		// 0x20 - Enable sixth character slot
	CF_BIT_SE,				// 0x40
	CF_BIT_ML,				// 0x80 - Elven race, new spells, skills + housing tiles
	CF_BIT_EIGHTAGE,		// 0x100 - Splash screen for 8th age
	CF_BIT_NINTHAGE,		// 0x200 - Splash screen for 9th age
	CF_BIT_TENTHAGE,		// 0x400 - Splash screen for 10th age - crystal/shadow house tiles
	CF_BIT_UNKNOWN1,		// 0x800 - Increased housing/bank storage (6.0.3.0 or earlier)
	CF_BIT_SEVENCHARS,		// 0x1000 - Enable seventh character slot
	//CF_BIT_KRFACES,		// 0x2000 - KR release (6.0.0.0)
	//CF_BIT_TRIAL,			// 0x4000 - Trial account
	CF_BIT_EXPANSION = 15,	// 0x8000 - Live account
	CF_BIT_SA,				// 0x10000 - Enable SA features: gargoyle race, spells, skills, housing tiles - clients 6.0.14.2+
	CF_BIT_HS,				// 0x20000 - Enable HS features: boats, new movementtype? ++
	CF_BIT_GOTHHOUSE,		// 0x40000
	CF_BIT_RUSTHOUSE,		// 0x80000
	CF_BIT_JUNGLEHOUSE,		// 0x100000 - Enable Jungle housing tiles
	CF_BIT_SHADOWHOUSE,		// 0x200000 - Enable Shadowguard housing tiles
	CF_BIT_TOLHOUSE,		// 0x400000 - Enable Time of Legends features
	CF_BIT_ENDLESSHOUSE,	// 0x800000 - Enable Endless Journey account
	CF_BIT_COUNT
};


enum ServerFeatures
{
	SF_BIT_UNKNOWN1 = 0,	// 0x01
	SF_BIT_IGR,				// 0x02
	SF_BIT_ONECHAR,			// 0x04 - One char only, Siege-server style
	SF_BIT_CONTEXTMENUS,	// 0x08
	SF_BIT_LIMITCHAR,		// 0x10 - Limit amount of chars, combine with OneChar
	SF_BIT_AOS,				// 0x20 - Enable Tooltips, fight system book - but not monsters/map/skills/necro/pala classes
	SF_BIT_SIXCHARS,		// 0x40 - Use 6 character slots instead of 5 (4.0.3a)
	SF_BIT_SE,				// 0x80 - Samurai and Ninja classes, bushido, ninjitsu (4.0.5a)
	SF_BIT_ML,				// 0x100 - Elven race, spellweaving (4.0.11d)
	SF_BIT_UNKNOWN2,		// 0x200 - added with UO:KR launch (6.0.0.0)
	SF_BIT_SEND3DTYPE,		// 0x400 - Send UO3D client type? KR and SA clients will send 0xE1)
	SF_BIT_UNKNOWN4,		// 0x800 - added sometime between UO:KR and UO:SA
	SF_BIT_SEVENCHARS,		// 0x1000 - Use 7 character slots instead of 5?6?, only 2D client?
	SF_BIT_UNKNOWN5,		// 0x2000 - added with UO:SA launch, imbuing, mysticism, throwing? (7.0.0.0)
	SF_BIT_NEWMOVE,			// 0x4000 - new movement system (packets 0xF0, 0xF1, 0xF2))
	SF_BIT_FACTIONAREAS = 15,	// 0x8000 - Unlock new Felucca faction-areas (map0x.mul?)
	SF_BIT_COUNT
};

enum AssistantFeatures : UI64
{
	AF_NONE = 0,

	// Razor/RE/AssistUO
	AF_FILTERWEATHER = 1 << 0,			// Weather Filter
	AF_FILTERLIGHT = 1 << 1,			// Light Filter
	AF_SMARTTARGET = 1 << 2,			// Smart Last Target
	AF_RANGEDTARGET = 1 << 3,			// Range Check Last Target
	AF_AUTOOPENDOORS = 1 << 4,			// Automatically Open Doors
	AF_DEQUIPONCAST = 1 << 5,			// Unequip Weapon on spell cast
	AF_AUTOPOTIONEQUIP = 1 << 6,		// Un/Re-equip weapon on potion use
	AF_POISONEDCHECKS = 1 << 7,			// Block heal If poisoned/Macro IIf Poisoned condition/Heal or Cure self
	AF_LOOPEDMACROS = 1 << 8,			// Disallow Looping macros, For loops, and macros that call other macros
	AF_USEONCEAGENT = 1 << 9,			// The use once agent
	AF_RESTOCKAGENT = 1 << 10,			// The restock agent
	AF_SELLAGENT = 1 << 11,				// The sell agent
	AF_BUYAGENT = 1 << 12,				// The buy agent
	AF_POTIONHOTKEYS = 1 << 13,			// All potion hotkeys
	AF_RANDOMTARGETS = 1 << 14,			// All random target hotkeys (Not target next, last target, target self)
	AF_CLOSESTTARGETS = 1 << 15,		// All closest target hotkeys
	AF_OVERHEADHEALTH = 1 << 16,		// Health and Mana/Stam messages shown over player's heads
	
	// AssistUO only?
	AF_AUTOLOOTAGENT = 1 << 17,			// The autoloot agent
	AF_BONECUTTERAGENT = 1 << 18,		// The bone cutter agent
	AF_JSCRIPTMACROS = 1 << 19,			// Javascript macro engine
	AF_AUTOREMOUNT = 1 << 20,			// Auto remount after dismount

	// UOSteam
	AF_AUTOBANDAGE = 1 << 21,			// Automatically apply bandages when health is low
	AF_ENEMYTARGETSHARE = 1 << 22,		// Share target with party/guild/alliance
	AF_FILTERSEASON = 1 << 23,			// Season filter that forces a specific season
	AF_SPELLTARGETSHARE = 1 << 24,		// Share spell target with party/guild/alliance
	AF_HUMANOIDHEALTHCHECKS = 1 << 25,	// 
	AF_SPEECHJOURNALCHECKS = 1 << 26,	// ???
	AF_ALL = 0xFFFFFFFFFFFFFFFF			// Every feature possible
};

enum cSD_TID
{
	tSERVER_ERROR = -1,
	tSERVER_CORPSEDECAY = 0,	// Amount of time for a corpse to decay.
	tSERVER_WEATHER,			// Amount of time between changing light levels (day cycles).
	tSERVER_SHOPSPAWN,			// Amount of time between shopkeeper restocks.
	tSERVER_DECAY,				// Amount of time a decayable item will remain on the ground before dissapearing.
	tSERVER_DECAYINHOUSE,		// Amount of time a decayable item will remain on the floor of a house before dissapearing, if not locked down.
	tSERVER_INVISIBILITY,		// Duration of the invisibility spell.
	tSERVER_OBJECTUSAGE,		// Amount of time a player must wait between using objects.
	tSERVER_GATE,				// Duration of a summoned moongate.
	tSERVER_POISON,				// Duration of the poison effect on a character.
	tSERVER_LOGINTIMEOUT,		// Amount of time for an idle connection to time out.
	tSERVER_HITPOINTREGEN,		// Amount of time required to regenerate a single point of health.
	tSERVER_STAMINAREGEN,		// Amount of time required to regenerate a single point of stamina.
	tSERVER_MANAREGEN,			// Amount of time required to regenerate a single point of mana.
	tSERVER_FISHINGBASE,		// Base time for fishing skill to complete.
	tSERVER_FISHINGRANDOM,		// Max random time for fishing skill to complete.
	tSERVER_SPIRITSPEAK,		// Duration of the spirit speak skill.
	tSERVER_HUNGERRATE,			// Amount of time a player has before his hunger level decreases.
	tSERVER_THIRSTRATE,			// Amount of time a player has before his thirst level decreases.
	tSERVER_POLYMORPH,			// Duration of the polymorph spell.
	tSERVER_ESCORTWAIT,			// Amount of time until an escort NPC will dissapear while waiting for a player to start his quest.
	tSERVER_ESCORTACTIVE,		// Amount of time until an escort NPC will dissapear while a player is escorting him.
	tSERVER_ESCORTDONE,			// Amount of time until an escort NPC will dissapear when his quest is finished.
	tSERVER_MURDERDECAY,		// Amount of time before a permanent murder count will decay.
	tSERVER_CRIMINAL,			// Amount of time a character remains criminal after committing a criminal act.
	tSERVER_STEALINGFLAG,		// Amount of time a character's stealing flag remains active
	tSERVER_AGGRESSORFLAG,		// Amount of time a character remains aggressor after committing an aggressive act
	tSERVER_PERMAGREYFLAG,		// Amount of time a permagrey flag remains active after player has stolen from someone
	tSERVER_COMBATIGNORE,		// Amount of time an NPC will ignore an unreachable target in combat
	tSERVER_POTION,				// Delay between using potions
	tSERVER_PETOFFLINECHECK,	// Delay between checks for the PetOfflineTimeout
	tSERVER_NPCFLAGUPDATETIMER, // Delay in seconds between each time NPC flags are updated
	tSERVER_BLOODDECAY,			// Delay in seconds before blood spatter spawned from combat decays
	tSERVER_BLOODDECAYCORPSE,	// Delay in seconds before blood spawned along with corpses decays
	tSERVER_NPCCORPSEDECAY,		// Delay in seconds before NPC corpses decay
	tSERVER_LOYALTYRATE,		// Amount of time between each time loyalty decreases for a pet
	tSERVER_COUNT
};

enum CSDDirectoryPaths
{
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
struct __STARTLOCATIONDATA__
{
	__STARTLOCATIONDATA__()
	{
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
	SI16	x;
	SI16	y;
	SI16	z;
	SI16	worldNum;
	UI16	instanceId;
	UI32	clilocDesc;
};
//o------------------------------------------------------------------------------------------------o
// PhysicalServer
//o------------------------------------------------------------------------------------------------o
//o------------------------------------------------------------------------------------------------o
class PhysicalServer
{
public:
	auto SetName( const std::string& newName ) -> void;
	auto SetDomain( const std::string& newDomain ) -> void;
	auto SetIP( const std::string& newIP ) -> void;
	auto SetPort( UI16 newPort ) -> void;
	auto GetName() const -> const std::string&;
	auto GetDomain() const -> const std::string&;
	auto GetIP() const -> const std::string&;
	auto GetPort() const -> UI16;

private:
	std::string name;
	std::string domain;
	std::string ip;
	UI16 port;
};

class CServerData
{
private:

	std::bitset<CF_BIT_COUNT> clientFeatures;
	std::bitset<SF_BIT_COUNT> serverFeatures;

	// Once over 62, bitsets are costly.  std::vector<bool> has a special exception in the c++ specificaiton, to minimize wasted space for bools
	// These should be updated
	std::bitset<106>	boolVals;			// Many values stored this way, rather than using bools.
	std::bitset<64>		spawnRegionsFacets;	// Used to determine which facets to enable spawn regions for, set in UOX>INI
	std::bitset<64>		moongateFacets;		// Used to determine which facets to enable moongates for, set in UOX>INI

	// ServerSystems
	std::string sServerName;				// 04/03/2004 - Need a place to store the name of the server (Added to support the UOG Info Request)
	std::string	commandprefix;				//	Character that acts as the command prefix
	std::string externalIP;
	ip4list_t   availableIPs;
	
	std::vector<PhysicalServer> serverList;	//	Series of server entries for shard list
	UI08		consoleLogEnabled;			//	Various levels of legging 0 == none, 1 == normal, 2 == normal + all speech
	UI16		serverLanguage;				//	Default language used on server
	UI16		port;						//	Port number that the server listens on, for connections
	UI16		jsEngineSize;				// gcMaxBytes limit in MB per JS runtime
	UI16		apsPerfThreshold;			// Performance threshold (simulation cycles) below which APS system kicks in - 0 disables system
	UI16		apsInterval;				// Interval at which APS checks and optionally makes adjustments based on shard performance
	UI16		apsDelayStep;				// Step value in milliseconds that APS uses to gradually adjust delay for NPC AI/movement checks
	UI16		apsDelayMaxCap;				// Max impact in milliseconds APS can have on NPC AI/movement checks
	UI16		sysMsgColour;				// Default text colour for system messages displayed in bottom left corner of screen
	SI16		backupRatio;				//	Number of saves before a backup occurs
	UI32		serverSavesTimer;				//	Number of seconds between world saves
	UI32		netRcvTimeout;				// 04/03/2004 - Used to be hardcoded as 2 seconds (2 * 1000ms) for some raly laggy nets this would drop alot of packets, and disconnect people.
	UI32		netSndTimeout;				// 04/03/2004 - Not used at this time.
	UI32		netRetryCount;				// 04/03/2004 - Used to set the number of times a network recieve will be attempted before it throws an error
	UI32		maxBytesIn;					// Max bytes that can be received from a client in a 10-second window before client is warned/kicked for excessive data use
	UI32		maxBytesOut;				// Max bytes that can be sent to a client in a 10-second window before client is warned/kicked for excessive data use
	UI32		trafficTimeban;				// Duration in minutes that player will be banned for if they exceed their network traffic budget

	bool		uogEnabled;					// 04/03/2004 - Added to support the UOG Info Request Service
	bool		randomStartingLocation;			// Enable or disable randomizing starting location for new players based on starting location entries
	bool		useUnicodeMessages;			// Enable or disable sending messages originating on server in Unicode format

	// Client Support
	bool		Clients4000Enabled;				// Allow client connections from 4.0.0 to 4.0.11f
	bool		Clients5000Enabled;				// Allow client connections from 5.0.0.0 to 5.0.8.2 (Mondain's Legacy)
	bool		Clients6000Enabled;				// Allow client connections from 6.0.0.0 to 6.0.4.0
	bool		Clients6050Enabled;				// Allow client connections from 6.0.5.0 to 6.0.14.2
	bool		Clients7000Enabled;				// Allow client connections from 7.0.0.0 to 7.0.8.2
	bool		Clients7090Enabled;				// Allow client connections from 7.0.9.0 to 7.0.15.1 (High Seas)
	bool		Clients70160Enabled;			// Allow client connections from 7.0.16.0 to 7.0.23.1
	bool		Clients70240Enabled;			// Allow client connections from 7.0.24.0+
	bool		Clients70300Enabled;			// Allow client connections from 7.0.30.0+
	bool		Clients70331Enabled;			// Allow client connections from 7.0.33.1+
	bool		Clients704565Enabled;			// Allow client connections from 7.0.45.65+ (Time of Legends)
	bool		Clients70610Enabled;			// Allow client connections from 7.0.61.0+ (Endless Journey)

	// facet block
	bool		useFacetSaves;
	std::vector<std::string> facetNameList;
	std::vector<UI32> facetAccessFlags;

	// Skills & Stats
	UI08		skillDelay;						//	Number of seconds after a skill is used before another skill can be used
	UI16		skillTotalCap;					//	A cap on the total of all a PC's skills
	UI16		skillCap;						//	A cap on each individual PC skill
	UI16		statCap;						//	A cap on the total of a PC's stats
	SI16		maxStealthMovement;				//	Max number of steps allowed with stealth skill at 100.0
	SI16		maxStaminaMovement;				//	Max number of steps allowed while running before stamina is reduced

	// ServerTimers
	// array
	UI16		serverTimers[tSERVER_COUNT];
	// Directories
	// array
	std::string serverDirectories[CSDDP_COUNT];
	
	std::string actualINI;						// 	The actual uox.ini file loaded, used for saveing

	std::string secretShardKey;					// Secret shard key used to only allow connection from specific custom clients with matching key

	// Expansion
	// 0 = core, 1 = UO, 2 = T2A, 3 = UOR, 4 = TD, 5 = LBR (Pub15), 6 = AoS, 7 = SE, 8 = ML, 9 = SA, 10 = HS, 11 = ToL
	UI08		coreShardEra;					// Determines core era ruleset for shard (determines which items/npcs are loaded, and which rules are applied in combat)
	UI08		expansionArmorCalculation;		// Determines which era ruleset to use for calculating armor and defense
	UI08		expansionStrengthDamageBonus;	// Determines which era ruleset to use for calculating strength damage bonus
	UI08		expansionTacticsDamageBonus;	// Determines which era ruleset to use for calculating tactics damage bonus
	UI08		expansionAnatomyDamageBonus;	// Determines which era ruleset to use for calculating anatomy damage bonus
	UI08		expansionLumberjackDamageBonus;	// Determines which era ruleset to use for calculating lumberjack damage bonus
	UI08		expansionRacialDamageBonus;		// Determines which era ruleset to use for calculating racial damage bonus
	UI08		expansionDamageBonusCap;		// Determines which era ruleset to use for calculating damage bonus cap
	UI08		expansionShieldParry;			// Determines which era ruleset to use for shield parry calculations
	UI08		expansionWeaponParry;			// Determines which era ruleset to use for weapon parry calculations
	UI08		expansionWrestlingParry;		// Determines which era ruleset to use for wrestling parry calculations
	UI08		expansionCombatHitChance;		// Determines which era ruleset to use for calculating melee hit chance

	// Settings
	SI16		ambientSounds;					//	Ambient sounds - values from 1->10 - higher values indicate sounds occur less often
	SI16		htmlStatusEnabled;				//	If > 0 then it's enabled - only used at PC char creation - use elsewhere? (was # of seconds between updates)
	SI16		sellMaxItems;					//	Maximum number of items that can be sold to a vendor
	SI16		fishingstaminaloss;				//	The amount of stamina lost with each use of fishing skill
	SI16		physicalResistCap;				//	The Cap for physical resist property
	SI16		fireResistCap;					//	The Cap for fire resist property
	SI16		coldResistCap;					//	The Cap for cold resist property
	SI16		poisonResistCap;				//	The Cap for poison resist property
	SI16		energyResistCap;				//	The Cap for energy resist property
	SI16		defenseChanceIncreaseCap;		//	The Cap for energy resist property
	UI08		maxControlSlots;				//	The default max amount of pet/follower control slots for each player
	UI08		maxSafeTeleports;				//	The default max amount of free teleports to safety players get via the help menu per day
	UI08		maxPetOwners;					//	The default max amount of different owners a pet may have in its lifetime
	UI08		maxFollowers;					//	The default max amount of followers a player can have active
	UI16		petOfflineTimeout;				//	Offline time after a player looses all pets
	UI16		maxPlayerPackItems;				//	The max amount of items a player's backpack can contain
	UI16		maxPlayerBankItems;				//	The max amount of items a player's bankbox can contain
	SI32		maxPlayerPackWeight;			//	The max weight capacity of a player's backpack (including subcontainers)
	SI32		maxPlayerBankWeight;			//	The max weight capacity of a player's bankbox (including subcontainers)
	R32			weightPerSTR;					//	How much weight per point of STR a character can hold.
	bool		paperdollGuildButton;			//	Enable Guild-button on paperdoll to access guild-menus without going through guildstone
	UI16		petLoyaltyGainOnSuccess;		//	The default amount of pet loyalty gained on successful use of a pet command
	UI16		petLoyaltyLossOnFailure;		//	The default amount of pet loyalty lost on a failed attempt to use a pet command

	// SpeedUp
	R32			npcWalkingSpeed;				//	Speed at which walking NPCs move
	R32			npcRunningSpeed;				//	Speed at which running NPCs move
	R32			npcFleeingSpeed;				//	Speed at which fleeing NPCs move
	R32			npcMountedWalkingSpeed;			//	Speed at which (mounted) walking NPCs move
	R32			npcMountedRunningSpeed;			//	Speed at which (mounted) running NPCs move
	R32			npcMountedFleeingSpeed;			//	Speed at which (mounted) fleeing NPCs move
	R32			archeryShootDelay;				//  Attack delay for archers; after coming to a full stop, they need to wait this amount of time before they can fire an arrow. Defaults to 1.0 seconds
	R32			globalAttackSpeed;				//  Global attack speed that can be tweaked to quickly increase or decrease overall combat speed. Defaults to 1.0
	R32			npcSpellcastSpeed;				//  For adjusting the overall speed of (or delay between) NPC spell casts. Defaults to 1.0
	R32			globalRestockMultiplier;		//	Global multiplier applied to restock properties of items when loaded from DFNs
	R32			bodGoldRewardMultiplier;		//	Multiplier that adjusts the amount of Gold rewarded by completing Bulk Order Deeds
	R32			bodFameRewardMultiplier;		//	Multiplier that adjusts the amount of Fame rewarded by completing Bulk Order Deeds
	R64			checkItems;						//	How often (in seconds) items are checked for decay and other things
	R64			checkBoats;						//	How often (in seconds) boats are checked for motion and so forth
	R64			checkNpcAi;						//	How often (in seconds) NPCs can execute an AI routine
	R64			checkSpawnRegions;				//	How often (in seconds) spawn regions are checked for new spawns
	R64			flushTime;						//	How often (in minutes) online accounts are checked to see if they really ARE online

	// MessageBoards
	UI08		msgPostingLevel;				//	If not 0, then players can post
	UI08		msgRemovalLevel;				//	If not 0, then players can remove posts

	// WorldLight
	LIGHTLEVEL	dungeonLightLevel;				//	Default light level for a dungeon, if not subject to a weather system
	LIGHTLEVEL	currentLightLevel;				//	Default current light level if not subject to a weather system
	LIGHTLEVEL	brightnessLightLevel;			//	Default brightest light level if not subject to a weather system
	LIGHTLEVEL	darknessLightLevel;				//	Default darkest light level if not subject to a weather system

	// WorldTimer								//	days/hours/minutes/seconds to it's own file?
	UI16		secondsPerUoMinute;				//	Number of seconds for a UOX minute.
	UI08		seconds;						//	Number of seconds the world has been running (persistent)
	UI08		minutes;						//	Number of minutes the world has been running (persistent)
	UI08		hours;							//	Number of hours the world has been running (persistent)
	SI16		days;							//	Number of days the world has been running (persistent)
	SI16		moon[2];						//	Moon current state
	bool		ampm;							//	Whether our current time is in the morning or afternoon

	// Tracking
	UI08		trackingMaxTargets;				//	Maximum number of targets that can be tracked
	UI16		trackingBaseRange;				//	Minimum range even a novice can track from
	UI16		trackingBaseTimer;				//	Tracking timer - maximum time for a GM to track at
	UI16		trackingMsgRedisplayTimer;		//	How often (in seconds) the tracking message is redisplayed

	// Reputation
	UI16		maxMurdersAllowed;				//	Maximum number of kills before turning red

	// Resources
	UI08		mineCheck;						//	Type of mining check performed - 0 anywhere 1 only mountains/floors 2 region based (not working)
	UI16		resourceAreaSize;				//	Size of each resource area to split each world into
	UI16		oreRespawnTimer;				//	Time at which ore is respawned (only 1 ore, not all)
	UI16		logsRespawnTimer;				//	TIMERVAL? Time at which logs are respawned (only 1 log, not all)
	UI16		fishRespawnTimer;				//	TIMERVAL? Time at which fish are respawned (only 1 fish, not all)
	SI16		logsPerArea;					//	Maximum number of logs in a resource area
	SI16		orePerArea;						//	TIMERVAL? Maximum number of ores in a resource area
	SI16		fishPerArea;					//	Maximum number of fish in a resource area

	// Hunger & Food
	SI16		hungerDamage;					//	Amount of damage applied if hungry and below threshold
	// Thirst
	SI16		thirstDrain;					//  Amount of stamina drained if thirsty and below threshold

	// Combat
	UI08		combatAnimalAttackChance;		//	Chance of animals being attacked (0-100)
	UI08		combatWeaponDamageChance;		//  Chance of weapons being damaged when attacking in combat (0-100)
	UI08		combatWeaponDamageMin;			//  Minimum amount of hitpoints a weapon can lose when being damaged in combat
	UI08		combatWeaponDamageMax;			//  Maximum amount of hitpoints a weapon can lose when being damaged in combat
	UI08		combatArmorDamageChance;		//  Chance of armor being damaged when defending in combat (0-100)
	UI08		combatArmorDamageMin;			//  Minimum amount of hitpoints an armor can lose when being damaged in combat
	UI08		combatArmorDamageMax;			//  Maximum amount of hitpoints an armor can lose when being damaged in combat
	UI08		combatParryDamageChance;		//  Chance of shield being damaged when parrying in combat (0-100)
	UI08		combatParryDamageMin;			//  Minimum amount of hitpoints a shield/weapon can lose when successfully parrying in combat
	UI08		combatParryDamageMax;			//  Maximum amount of hitpoints a shield/weapon can lose when successfully parrying in combat
	UI08		combatBloodEffectChance;		//  Chance of blood splatter effects spawning during combat
	UI08		alchemyDamageBonusModifier;		//  Modifier used to calculate bonus damage for explosion potions based on alchemy skill
	UI08		combatWeaponDamageBonusType;	//  Weapon damage bonus type (0 - apply to hidamage, 1 - split between lo and hi, 2 - apply equally to lo and hi
	SI08		combatArcheryHitBonus;			//  Bonus to hit chance for Archery skill in combat, applied after regular hit chance calculation
	SI16		combatMaxRange;					//	RANGE?  Range at which combat can actually occur
	SI16		combatMaxSpellRange;			//	RANGE?  Range at which spells can be cast
	SI16		combatNpcDamageRate;			//	NPC Damage divisor - PCs sustain less than NPCs.  If a PC, damage is 1/value
	SI16		combatNpcBaseFleeAt;			//	% of HP where an NPC will flee, if it's not defined for them
	SI16		combatNpcBaseReattackAt;		//	% of HP where an NPC will resume attacking
	SI16		combatAttackStamina;			//	Amount of stamina lost when hitting an opponent

	// Start & Location Settings
	std::vector<__STARTLOCATIONDATA__>	startlocations;
	std::vector<__STARTLOCATIONDATA__>	youngStartlocations;
	UI16		startPrivs;						//	Starting privileges of characters
	SI16		startGold;						//	Amount of gold created when a PC is made

	// Anything under this comment is left here for symantics
	UI08		skillLevel;						//	Some skill value associated with the rank system
	SI16		buyThreshold;					//	Value above which money will be sourced from the bank rather than the player

	// Gump stuff
	UI16		titleColour;					//	Default text colour for titles in gumps
	UI16		leftTextColour;					//	Default text colour for left text in gumps (2 column ones)
	UI16		rightTextColour;				//	Default text colour for right text in gumps
	UI16		buttonCancel;					//	Default Button ID for cancel button in gumps
	UI16		buttonLeft;						//	Default Button ID for left button in gumps
	UI16		buttonRight;					//	Default Button ID for right button in gumps
	UI16		backgroundPic;					//	Default Gump ID for background gump

	// Houses
	UI16		maxHousesOwnable;				//	Max amount of houses that a player can own
	UI16		maxHousesCoOwnable;				//	Max amount of houses that a player can co-own

	// Townstone stuff
	UI32		numSecsPollOpen;				//	Time (in seconds) for which a town voting poll is open
	UI32		numSecsAsMayor;				//	Time (in seconds) that a PC would be a mayor
	UI32		taxPeriod;					//	Time (in seconds) between periods of taxes for PCs
	UI32		guardPayment;				//	Time (in seconds) between payments for guards

	void	PostLoadDefaults();
	static const std::map<std::string, SI32> uox3IniCaseValue;
public:
	UI64		DisabledAssistantFeatures;
	
	auto 		LookupINIValue( const std::string& tag ) -> SI32;

	auto		SetServerFeature( ServerFeatures, bool ) -> void;
	auto		SetServerFeatures( size_t ) -> void;
	auto		GetServerFeature( ServerFeatures ) const -> bool;
	auto		GetServerFeatures() const -> size_t;

	auto		SetClientFeature( ClientFeatures, bool ) -> void;
	auto		SetClientFeatures( UI32 ) -> void;
	auto		GetClientFeature( ClientFeatures ) const -> bool;
	UI32		GetClientFeatures() const;

	auto		SetDisabledAssistantFeature( AssistantFeatures, bool ) -> void;
	auto		SetDisabledAssistantFeatures( UI64 ) -> void;
	auto		GetDisabledAssistantFeature( AssistantFeatures ) const -> bool;
	UI64		GetDisabledAssistantFeatures() const;

	auto		SetAssistantNegotiation( bool value ) -> void;
	auto		GetAssistantNegotiation() const -> bool;

	auto		GetSpawnRegionsFacetStatus( UI32 value ) const -> bool;
	auto		SetSpawnRegionsFacetStatus( UI32 value, bool status ) -> void;
	UI32		GetSpawnRegionsFacetStatus() const;
	auto		SetSpawnRegionsFacetStatus( UI32 value ) -> void;

	auto		GetMoongateFacetStatus( UI32 value ) const -> bool;
	auto		SetMoongateFacetStatus( UI32 value, bool status ) -> void;
	UI32		GetMoongateFacetStatus() const;
	auto		SetMoongateFacetStatus( UI32 value ) -> void;

	auto		SetClassicUOMapTracker( bool value ) -> void;
	auto		GetClassicUOMapTracker() const -> bool;

	auto		UseUnicodeMessages( bool value ) -> void;
	auto		UseUnicodeMessages() const -> bool;

	SI16		ServerMoon( SI16 slot ) const;
	LIGHTLEVEL	WorldLightDarkLevel() const;
	LIGHTLEVEL	WorldLightBrightLevel() const;
	LIGHTLEVEL	WorldLightCurrentLevel() const;
	LIGHTLEVEL	DungeonLightLevel() const;
	auto		ServerStartPrivs() const -> UI16;
	SI16		ServerStartGold() const;

	auto		ServerMoon( SI16 slot, SI16 value ) -> void;
	auto		WorldLightDarkLevel( LIGHTLEVEL value ) -> void;
	auto		WorldLightBrightLevel( LIGHTLEVEL value ) -> void;
	auto		WorldLightCurrentLevel( LIGHTLEVEL value ) -> void;
	auto		DungeonLightLevel( LIGHTLEVEL value ) -> void;
	auto		ServerStartPrivs( UI16 value ) -> void;
	auto		ServerStartGold( SI16 value ) -> void;
	auto		ParseIni( const std::string& filename ) -> bool;
	auto		HandleLine( const std::string& tag, const std::string& value ) -> bool;

	auto		Load( const std::string &filename = "" ) -> bool;
	auto		SaveIni() -> bool;
	auto		SaveIni( const std::string &filename ) -> bool;

	auto		EraEnumToString( ExpansionRuleset eraNum, bool coreEnum = false ) -> std::string;
	auto		EraStringToEnum( const std::string &eraString, bool useDefault = true, bool inheritCore = true ) -> ExpansionRuleset;

	auto ResetDefaults() -> void;
	auto Startup() -> void;
	//void		RefreshIPs( void );

	CServerData();
	auto		ServerName( const std::string &setname ) -> void;
	auto		SecretShardKey( const std::string &newName ) -> void;
	auto		ServerDomain( const std::string &setdomain ) -> void;
	auto		ServerIP( const std::string &setip ) -> void;
	auto		ServerName() const -> const std::string &;
	auto		SecretShardKey() const -> const std::string &;
	auto		ServerDomain() const -> const std::string &;
	auto		ServerIP() const -> const std::string &;
	auto		ExternalIP() const -> const std::string &;
	auto 		ExternalIP( const std::string &ip ) -> void;
	auto		ServerPort( UI16 setport ) -> void;
	auto		ServerPort() const -> UI16;
	auto		ServerConsoleLog( bool setting ) -> void;
	auto		ServerConsoleLog() const -> bool;
	auto		ServerNetworkLog( bool setting ) -> void;
	auto		ServerNetworkLog() const -> bool;
	auto		ServerSpeechLog( bool setting ) -> void;
	auto		ServerSpeechLog() const -> bool;
	auto		ServerCommandPrefix( char cmdvalue ) -> void;
	char		ServerCommandPrefix() const;
	auto		ServerAnnounceSaves( bool setting ) -> void;
	auto		ServerAnnounceSavesStatus() const -> bool;
	auto		ServerJoinPartAnnouncements( bool setting ) -> void;
	auto		ServerJoinPartAnnouncementsStatus() const -> bool;
	auto		ServerMulCaching( bool setting ) -> void;
	auto		ServerMulCachingStatus() const -> bool;
	auto		ServerBackups( bool setting ) -> void;
	auto		ServerBackupStatus() const -> bool;
	auto		ServerContextMenus( bool setting ) -> void;
	auto		ServerContextMenus() const -> bool;
	auto		ServerSavesTimer( UI32 timer ) -> void;
	UI32		ServerSavesTimerStatus() const;
	auto		ServerMainThreadTimer( UI32 threadtimer ) -> void;
	UI32		ServerMainThreadTimerStatus() const;
	auto		ServerSkillTotalCap( UI16 cap ) -> void;
	auto		ServerSkillTotalCapStatus() const -> UI16;
	auto		ServerSkillCap( UI16 cap ) -> void;
	auto		ServerSkillCapStatus() const -> UI16;
	auto		ServerSkillDelay( UI08 skdelay ) -> void;
	UI08		ServerSkillDelayStatus() const;
	auto		ServerStatCap( UI16 cap ) -> void;
	auto		ServerStatCapStatus() const -> UI16;
	auto		MaxStealthMovement( SI16 value ) -> void;
	SI16		MaxStealthMovement() const;
	auto		MaxStaminaMovement( SI16 value ) -> void;
	SI16		MaxStaminaMovement() const;
	auto		SystemTimer( cSD_TID timerId, UI16 value ) -> void;
	auto		SystemTimer( cSD_TID timerId ) const -> UI16;
	TIMERVAL	BuildSystemTimeValue( cSD_TID timerId ) const;
	auto		SysMsgColour( UI16 value ) -> void;
	auto		SysMsgColour() const -> UI16;

	auto		ServerUOGEnabled() const -> bool { return uogEnabled; }
	auto		ServerUOGEnabled( bool uogValue ) -> void {	uogEnabled = uogValue; }
	auto		FreeshardServerPoll( bool value ) -> void;
	auto		FreeshardServerPoll() const -> bool;
	auto		ServerRandomStartingLocation() const -> bool { return randomStartingLocation; }
	auto		ServerRandomStartingLocation( bool rndStartLocValue ) -> void { randomStartingLocation = rndStartLocValue; }
	UI32		ServerNetRetryCount() const { return netRetryCount; }
	auto		ServerNetRetryCount( UI32 retryValue ) -> void{ netRetryCount = retryValue; }
	UI32		ServerNetRcvTimeout() const { return netRcvTimeout; }
	auto		ServerNetRcvTimeout( UI32 timeoutValue ) -> void { netRcvTimeout = timeoutValue; }
	UI32		ServerNetSndTimeout() const { return netSndTimeout; }
	auto		ServerNetSndTimeout( UI32 timeoutValue ) -> void { netSndTimeout = timeoutValue; }

	// ClientSupport used to determine login-restrictions
	auto		ClientSupport4000() const -> bool { return Clients4000Enabled; }
	auto		ClientSupport4000( bool cliSuppValue ) -> void { Clients4000Enabled = cliSuppValue; }
	auto		ClientSupport5000() const -> bool { return Clients5000Enabled; }
	auto		ClientSupport5000( bool cliSuppValue ) -> void { Clients5000Enabled = cliSuppValue; }
	auto		ClientSupport6000() const  -> bool{ return Clients6000Enabled; }
	auto		ClientSupport6000( bool cliSuppValue ) -> void { Clients6000Enabled = cliSuppValue; }
	auto		ClientSupport6050() const  -> bool{ return Clients6050Enabled; }
	auto		ClientSupport6050( bool cliSuppValue ) -> void { Clients6050Enabled = cliSuppValue; }
	auto		ClientSupport7000() const  -> bool{ return Clients7000Enabled; }
	auto		ClientSupport7000( bool cliSuppValue ) -> void { Clients7000Enabled = cliSuppValue; }
	auto		ClientSupport7090() const -> bool { return Clients7090Enabled; }
	auto		ClientSupport7090( bool cliSuppValue ) -> void { Clients7090Enabled = cliSuppValue; }
	auto		ClientSupport70160() const -> bool { return Clients70160Enabled; }
	auto		ClientSupport70160( bool cliSuppValue ) -> void { Clients70160Enabled = cliSuppValue; }
	auto		ClientSupport70240() const -> bool { return Clients70240Enabled; }
	auto		ClientSupport70240( bool cliSuppValue ) -> void { Clients70240Enabled = cliSuppValue; }
	auto		ClientSupport70300() const  -> bool{ return Clients70300Enabled; }
	auto		ClientSupport70300( bool cliSuppValue ) -> void { Clients70300Enabled = cliSuppValue; }
	auto		ClientSupport70331() const -> bool { return Clients70331Enabled; }
	auto		ClientSupport70331( bool cliSuppValue ) -> void { Clients70331Enabled = cliSuppValue; }
	auto		ClientSupport704565() const -> bool { return Clients704565Enabled; }
	auto		ClientSupport704565( bool cliSuppValue ) -> void { Clients704565Enabled = cliSuppValue; }
	auto		ClientSupport70610() const  -> bool{ return Clients70610Enabled; }
	auto		ClientSupport70610( bool cliSuppValue ) -> void { Clients70610Enabled = cliSuppValue; }

	auto		StatsAffectSkillChecks( bool setting ) -> void;
	auto		StatsAffectSkillChecks() const -> bool;

	// Enable/disable higher total for starting stats, and/or fourth starting skill in clients 7.0.16+
	auto		ExtendedStartingStats( bool setting ) -> void;
	auto		ExtendedStartingStats() const -> bool;
	auto		ExtendedStartingSkills( bool setting ) -> void;
	auto		ExtendedStartingSkills() const -> bool;

	// Define all Path Get/Set's here please
	auto		Directory( CSDDirectoryPaths dp, std::string value ) -> void;
	std::string Directory( CSDDirectoryPaths dp );


	auto		PlayerCorpseLootDecay( bool value ) -> void;
	auto		PlayerCorpseLootDecay() const -> bool;
	auto		NpcCorpseLootDecay( bool value ) -> void;
	auto		NpcCorpseLootDecay() const -> bool;

	auto		GuardStatus( bool value ) -> void;
	auto		GuardsStatus() const -> bool;

	auto		DeathAnimationStatus( bool value ) -> void;
	auto		DeathAnimationStatus() const -> bool;

	auto		WorldAmbientSounds( SI16 value ) -> void;
	SI16		WorldAmbientSounds() const;

	auto		AmbientFootsteps( bool value ) -> void;
	auto		AmbientFootsteps() const -> bool;

	auto		InternalAccountStatus( bool value ) -> void;
	auto		InternalAccountStatus() const -> bool;

	auto		YoungPlayerSystem( bool value ) -> void;
	auto		YoungPlayerSystem() const -> bool;

	auto		ShowOfflinePCs( bool value ) -> void;
	auto		ShowOfflinePCs() const -> bool;

	auto		RogueStatus( bool value ) -> void;
	auto		RogueStatus() const -> bool;

	auto		SnoopIsCrime( bool value ) -> void;
	auto		SnoopIsCrime() const -> bool;

	auto		SnoopAwareness( bool value ) -> void;
	auto		SnoopAwareness() const -> bool;

	auto		PlayerPersecutionStatus( bool value ) -> void;
	auto		PlayerPersecutionStatus() const -> bool;

	auto		HtmlStatsStatus( SI16 value ) -> void;
	SI16		HtmlStatsStatus() const;

	auto		SellByNameStatus( bool value ) -> void;
	auto		SellByNameStatus() const -> bool;

	auto		SellMaxItemsStatus( SI16 value ) -> void;
	SI16		SellMaxItemsStatus() const;

	auto		TradeSystemStatus( bool value ) -> void;
	auto		TradeSystemStatus() const -> bool;

	auto		RankSystemStatus( bool value ) -> void;
	auto		RankSystemStatus() const -> bool;

	void		DisplayMakersMark( bool value );
	bool		DisplayMakersMark( void ) const;

	auto		CutScrollRequirementStatus( bool value ) -> void;
	auto		CutScrollRequirementStatus() const -> bool;

	auto		CheckPetControlDifficulty( bool value ) -> void;
	auto		CheckPetControlDifficulty() const -> bool;

	auto		NPCTrainingStatus( bool setting ) -> void;
	auto		NPCTrainingStatus() const -> bool;

	auto		CheckItemsSpeed( R64 value ) -> void;
	R64			CheckItemsSpeed() const;

	auto		CheckBoatSpeed( R64 value ) -> void;
	R64			CheckBoatSpeed() const;

	auto		CheckNpcAISpeed( R64 value ) -> void;
	R64			CheckNpcAISpeed() const;

	auto		CheckSpawnRegionSpeed( R64 value ) -> void;
	R64			CheckSpawnRegionSpeed() const;

	auto		GlobalAttackSpeed( R32 value ) -> void;
	R32			GlobalAttackSpeed() const;

	auto		NPCSpellCastSpeed( R32 value ) -> void;
	R32			NPCSpellCastSpeed() const;

	auto		GlobalRestockMultiplier( R32 value ) -> void;
	R32			GlobalRestockMultiplier() const;

	auto		BODGoldRewardMultiplier( R32 value ) -> void;
	R32			BODGoldRewardMultiplier() const;

	auto		BODFameRewardMultiplier( R32 value ) -> void;
	R32			BODFameRewardMultiplier() const;

	auto		MsgBoardPostingLevel( UI08 value ) -> void;
	UI08		MsgBoardPostingLevel() const;

	auto		MsgBoardPostRemovalLevel( UI08 value ) -> void;
	UI08		MsgBoardPostRemovalLevel() const;

	auto		MineCheck( UI08 value ) -> void;
	UI08		MineCheck() const;

	auto		AlchemyDamageBonusEnabled( bool value ) -> void;
	auto		AlchemyDamageBonusEnabled() const -> bool;

	auto		AlchemyDamageBonusModifier( UI08 value ) -> void;
	UI08		AlchemyDamageBonusModifier() const;

	auto		WeaponDamageBonusType( UI08 value ) -> void;
	UI08		WeaponDamageBonusType() const;

	auto		ItemsInterruptCasting( bool value ) -> void;
	auto		ItemsInterruptCasting() const -> bool;

	auto		CombatArmorClassDamageBonus( bool value ) -> void;
	auto		CombatArmorClassDamageBonus() const -> bool;

	auto		CombatDisplayHitMessage( bool value ) -> void;
	auto		CombatDisplayHitMessage() const -> bool;

	auto		CombatDisplayDamageNumbers( bool value ) -> void;
	auto		CombatDisplayDamageNumbers() const -> bool;

	auto		CombatAttackSpeedFromStamina( bool value ) -> void;
	auto		CombatAttackSpeedFromStamina() const -> bool;

	auto		FishingStaminaLoss( SI16 value ) -> void;
	SI16		FishingStaminaLoss() const;

	auto		CombatAttackStamina( SI16 value ) -> void;
	SI16		CombatAttackStamina() const;

	auto		CombatNpcDamageRate( SI16 value ) -> void;
	SI16		CombatNpcDamageRate() const;

	UI08		SkillLevel() const;
	auto		SkillLevel( UI08 value ) -> void;

	auto		EscortsEnabled( bool value ) -> void;
	auto		EscortsEnabled() const -> bool;

	auto		ItemsDetectSpeech( bool value ) -> void;
	auto		ItemsDetectSpeech() const -> bool;

	auto		ForceNewAnimationPacket( bool value ) -> void;
	auto		ForceNewAnimationPacket() const -> bool;

	auto		MapDiffsEnabled( bool value ) -> void;
	auto		MapDiffsEnabled() const -> bool;

	auto		MaxPlayerPackItems( UI16 value ) -> void;
	auto		MaxPlayerPackItems() const -> UI16;

	auto		MaxPlayerPackWeight( SI32 newVal ) -> void;
	auto		MaxPlayerPackWeight() const -> SI32;

	auto		MaxPlayerBankItems( UI16 value ) -> void;
	auto		MaxPlayerBankItems() const -> UI16;

	auto		MaxPlayerBankWeight( SI32 newVal ) -> void;
	auto		MaxPlayerBankWeight() const -> SI32;

	auto		BasicTooltipsOnly( bool value ) -> void;
	auto		BasicTooltipsOnly() const -> bool;

	auto		ShowNpcTitlesInTooltips( bool value ) -> void;
	auto		ShowNpcTitlesInTooltips() const -> bool;

	auto		ShowNpcTitlesOverhead( bool value ) -> void;
	auto		ShowNpcTitlesOverhead() const -> bool;

	auto		ShowInvulnerableTagOverhead( bool value ) -> void;
	auto		ShowInvulnerableTagOverhead() const -> bool;

	auto		ShowRaceWithName( bool value ) -> void;
	auto		ShowRaceWithName() const -> bool;

	auto		ShowRaceInPaperdoll( bool value ) -> void;
	auto		ShowRaceInPaperdoll() const -> bool;

	auto		ShowGuildInfoInTooltip( bool value ) -> void;
	auto		ShowGuildInfoInTooltip() const -> bool;

	auto		ShowReputationTitleInTooltip( bool value ) -> void;
	auto		ShowReputationTitleInTooltip() const -> bool;

	auto		EnableNPCGuildDiscounts( bool value ) -> void;
	auto		EnableNPCGuildDiscounts() const -> bool;

	auto		EnableNPCGuildPremiums( bool value ) -> void;
	auto		EnableNPCGuildPremiums() const -> bool;

	auto		CastSpellsWhileMoving( bool value ) -> void;
	auto		CastSpellsWhileMoving() const -> bool;

	auto		PetCombatTraining( bool value ) -> void;
	auto		PetCombatTraining() const -> bool;

	auto		HirelingCombatTraining( bool value ) -> void;
	auto		HirelingCombatTraining() const -> bool;

	auto		NpcCombatTraining( bool value ) -> void;
	auto		NpcCombatTraining() const -> bool;

	auto		ShowItemResistStats( bool value ) -> void;
	auto		ShowItemResistStats() const -> bool;

	auto		ShowWeaponDamageTypes( bool value ) -> void;
	auto		ShowWeaponDamageTypes() const -> bool;

	auto		GlobalItemDecay( bool value ) -> void;
	auto		GlobalItemDecay() const -> bool;

	auto		ScriptItemsDecayable( bool value ) -> void;
	auto		ScriptItemsDecayable() const -> bool;

	auto		BaseItemsDecayable( bool value ) -> void;
	auto		BaseItemsDecayable() const -> bool;

	auto		ItemDecayInHouses( bool value ) -> void;
	auto		ItemDecayInHouses() const -> bool;

	auto		ProtectPrivateHouses( bool value ) -> void;
	auto		ProtectPrivateHouses() const -> bool;

	auto		TrackHousesPerAccount( bool value ) -> void;
	auto		TrackHousesPerAccount() const -> bool;

	auto		CanOwnAndCoOwnHouses( bool value ) -> void;
	auto		CanOwnAndCoOwnHouses() const -> bool;

	auto		CoOwnHousesOnSameAccount( bool value ) -> void;
	auto		CoOwnHousesOnSameAccount() const -> bool;

	auto		SafeCoOwnerLogout( bool value ) -> void;
	auto		SafeCoOwnerLogout() const -> bool;

	auto		SafeFriendLogout( bool value ) -> void;
	auto		SafeFriendLogout() const -> bool;

	auto		SafeGuestLogout( bool value ) -> void;
	auto		SafeGuestLogout() const -> bool;

	auto		KeylessOwnerAccess( bool value ) -> void;
	auto		KeylessOwnerAccess() const -> bool;

	auto		KeylessCoOwnerAccess( bool value ) -> void;
	auto		KeylessCoOwnerAccess() const -> bool;

	auto		KeylessFriendAccess( bool value ) -> void;
	auto		KeylessFriendAccess() const -> bool;

	auto		KeylessGuestAccess( bool value ) -> void;
	auto		KeylessGuestAccess() const -> bool;

	auto		MaxHousesOwnable( UI16 value ) -> void;
	auto		MaxHousesOwnable() const -> UI16;

	auto		MaxHousesCoOwnable( UI16 value ) -> void;
	auto		MaxHousesCoOwnable() const -> UI16;

	auto		PaperdollGuildButton( bool value ) -> void;
	auto		PaperdollGuildButton() const -> bool;

	auto		CombatMonstersVsAnimals( bool value ) -> void;
	auto		CombatMonstersVsAnimals() const -> bool;

	auto		CombatAnimalsAttackChance( UI16 value ) -> void;
	auto		CombatAnimalsAttackChance() const -> UI16;

	auto		CombatArcheryShootDelay( R32 value ) -> void;
	R32			CombatArcheryShootDelay() const;

	auto		CombatArcheryHitBonus( SI08 value ) -> void;
	SI08		CombatArcheryHitBonus() const;

	auto		CombatWeaponDamageChance( UI08 value ) -> void;
	UI08		CombatWeaponDamageChance() const;

	auto		CombatWeaponDamageMin( UI08 value ) -> void;
	UI08		CombatWeaponDamageMin( void ) const;

	auto		CombatWeaponDamageMax( UI08 value ) -> void;
	UI08		CombatWeaponDamageMax() const;

	auto		CombatArmorDamageChance( UI08 value ) -> void;
	UI08		CombatArmorDamageChance() const;

	auto		CombatArmorDamageMin( UI08 value ) -> void;
	UI08		CombatArmorDamageMin() const;

	auto		CombatArmorDamageMax( UI08 value ) -> void;
	UI08		CombatArmorDamageMax() const;

	auto		CombatParryDamageChance( UI08 value ) -> void;
	UI08		CombatParryDamageChance() const;

	auto		CombatParryDamageMin( UI08 value ) -> void;
	UI08		CombatParryDamageMin() const;

	auto		CombatParryDamageMax( UI08 value ) -> void;
	UI08		CombatParryDamageMax() const;

	auto		CombatBloodEffectChance( UI08 value ) -> void;
	UI08		CombatBloodEffectChance() const;

	auto		TravelSpellsFromBoatKeys( bool value ) -> void;
	auto		TravelSpellsFromBoatKeys() const -> bool;

	auto		TravelSpellsWhileOverweight( bool value ) -> void;
	auto		TravelSpellsWhileOverweight() const -> bool;

	auto		MarkRunesInMultis( bool value ) -> void;
	auto		MarkRunesInMultis() const -> bool;

	auto		TravelSpellsBetweenWorlds( bool value ) -> void;
	auto		TravelSpellsBetweenWorlds() const -> bool;

	auto		TravelSpellsWhileAggressor( bool value ) -> void;
	auto		TravelSpellsWhileAggressor() const -> bool;

	auto		AutoUnequippedCasting( bool value ) -> void;
	auto		AutoUnequippedCasting() const -> bool;

	auto		ToolUseLimit( bool value ) -> void;
	auto		ToolUseLimit() const -> bool;

	auto		ToolUseBreak( bool value ) -> void;
	auto		ToolUseBreak() const -> bool;

	auto		ItemRepairDurabilityLoss( bool value ) -> void;
	auto		ItemRepairDurabilityLoss() const -> bool;

	auto		HideStatsForUnknownMagicItems( bool value ) -> void;
	auto		HideStatsForUnknownMagicItems() const -> bool;

	auto		CraftColouredWeapons( bool value ) -> void;
	auto		CraftColouredWeapons() const -> bool;

	auto		OfferBODsFromItemSales( bool value ) -> void;
	auto		OfferBODsFromItemSales() const -> bool;

	auto		OfferBODsFromContextMenu( bool value ) -> void;
	auto		OfferBODsFromContextMenu() const -> bool;

	auto		BODsFromCraftedItemsOnly( bool value ) -> void;
	auto		BODsFromCraftedItemsOnly() const -> bool;

	auto		PhysicalResistCap( SI16 value ) -> void;
	SI16		PhysicalResistCap() const;
	auto		FireResistCap( SI16 value ) -> void;
	SI16		FireResistCap() const;
	auto		ColdResistCap( SI16 value ) -> void;
	SI16		ColdResistCap() const;
	auto		PoisonResistCap( SI16 value ) -> void;
	SI16		PoisonResistCap() const;
	auto		EnergyResistCap( SI16 value ) -> void;
	SI16		EnergyResistCap() const;

	auto		DefenseChanceIncreaseCap( SI16 value ) -> void;
	SI16		DefenseChanceIncreaseCap() const;

	auto		MaxControlSlots( UI08 value ) -> void;
	UI08		MaxControlSlots() const;

	auto		MaxFollowers( UI08 value ) -> void;
	UI08		MaxFollowers() const;

	auto		MaxPetOwners( UI08 value ) -> void;
	UI08		MaxPetOwners() const;

	auto		SetPetLoyaltyGainOnSuccess( UI16 value ) -> void;
	auto		GetPetLoyaltyGainOnSuccess() const -> UI16;

	auto		SetPetLoyaltyLossOnFailure( UI16 value ) -> void;
	auto		GetPetLoyaltyLossOnFailure() const -> UI16;

	auto		MaxSafeTeleportsPerDay( UI08 value ) -> void;
	UI08		MaxSafeTeleportsPerDay() const;

	auto		TeleportToNearestSafeLocation( bool value ) -> void;
	auto		TeleportToNearestSafeLocation() const -> bool;

	auto		AllowAwakeNPCs( bool value ) -> void;
	auto		AllowAwakeNPCs() const -> bool;

	auto		HungerSystemEnabled( bool value ) -> void;
	auto		HungerSystemEnabled() const -> bool;

	auto		ThirstSystemEnabled( bool value ) -> void;
	auto		ThirstSystemEnabled() const -> bool;

	auto		HungerDamage( SI16 value ) -> void;
	SI16		HungerDamage() const;

	auto		ThirstDrain( SI16 value ) -> void;
	SI16		ThirstDrain() const;

	auto		PetHungerOffline( bool value ) -> void;
	auto		PetHungerOffline() const -> bool;

	auto		PetThirstOffline( bool value ) -> void;
	auto		PetThirstOffline() const -> bool;

	auto		PetOfflineTimeout( UI16 value ) -> void;
	auto		PetOfflineTimeout() const -> UI16;

	auto		BuyThreshold( SI16 value ) -> void;
	SI16		BuyThreshold() const;

	auto		BackupRatio( SI16 value ) -> void;
	SI16		BackupRatio() const;

	auto		CombatMaxRange( SI16 value ) -> void;
	SI16		CombatMaxRange() const;

	auto		CombatMaxSpellRange( SI16 value ) -> void;
	SI16		CombatMaxSpellRange() const;

	auto		CombatAnimalsGuarded( bool value ) -> void;
	auto		CombatAnimalsGuarded() const -> bool;

	auto		CombatNPCBaseFleeAt( SI16 value ) -> void;
	SI16		CombatNPCBaseFleeAt() const;

	void		ExpansionCoreShardEra( UI08 value );
	UI08		ExpansionCoreShardEra( void ) const;

	void		ExpansionArmorCalculation( UI08 value );
	UI08		ExpansionArmorCalculation() const;

	void		ExpansionStrengthDamageBonus( UI08 value );
	UI08		ExpansionStrengthDamageBonus() const;

	void		ExpansionTacticsDamageBonus( UI08 value );
	UI08		ExpansionTacticsDamageBonus() const;

	void		ExpansionAnatomyDamageBonus( UI08 value );
	UI08		ExpansionAnatomyDamageBonus() const;

	void		ExpansionLumberjackDamageBonus( UI08 value );
	UI08		ExpansionLumberjackDamageBonus() const;

	void		ExpansionRacialDamageBonus( UI08 value );
	UI08		ExpansionRacialDamageBonus() const;

	void		ExpansionDamageBonusCap( UI08 value );
	UI08		ExpansionDamageBonusCap() const;

	void		ExpansionShieldParry( UI08 value );
	UI08		ExpansionShieldParry() const;

	void		ExpansionWeaponParry( UI08 value );
	UI08		ExpansionWeaponParry() const;

	void		ExpansionWrestlingParry( UI08 value );
	UI08		ExpansionWrestlingParry() const;

	void		ExpansionCombatHitChance( UI08 value );
	UI08		ExpansionCombatHitChance() const;

	auto		CombatNPCBaseReattackAt( SI16 value ) -> void;
	SI16		CombatNPCBaseReattackAt() const;

	auto		ShootOnAnimalBack( bool setting ) -> void;
	auto		ShootOnAnimalBack() const -> bool;

	auto		NPCWalkingSpeed( R32 value ) -> void;
	R32			NPCWalkingSpeed() const;

	auto		NPCRunningSpeed( R32 value ) -> void;
	R32			NPCRunningSpeed() const;

	auto		NPCFleeingSpeed( R32 value ) -> void;
	R32			NPCFleeingSpeed() const;

	auto		NPCMountedWalkingSpeed( R32 value ) -> void;
	R32			NPCMountedWalkingSpeed() const;

	auto		NPCMountedRunningSpeed( R32 value ) -> void;
	R32			NPCMountedRunningSpeed() const;

	auto		NPCMountedFleeingSpeed( R32 value ) -> void;
	R32			NPCMountedFleeingSpeed() const;

	auto		TitleColour( UI16 value ) -> void;
	auto		TitleColour() const -> UI16;

	auto		LeftTextColour( UI16 value ) -> void;
	auto		LeftTextColour() const -> UI16;

	auto		RightTextColour( UI16 value ) -> void;
	auto		RightTextColour() const -> UI16;

	auto		ButtonCancel( UI16 value ) -> void;
	auto		ButtonCancel() const -> UI16;

	auto		ButtonLeft( UI16 value ) -> void;
	auto		ButtonLeft() const -> UI16;

	auto		ButtonRight( UI16 value ) -> void;
	auto		ButtonRight() const -> UI16;

	auto		BackgroundPic( UI16 value ) -> void;
	auto		BackgroundPic() const -> UI16;

	auto		TownNumSecsPollOpen( UI32 value ) -> void;
	UI32		TownNumSecsPollOpen() const;

	auto		TownNumSecsAsMayor( UI32 value ) -> void;
	UI32		TownNumSecsAsMayor() const;

	auto		TownTaxPeriod( UI32 value ) -> void;
	UI32		TownTaxPeriod() const;

	auto		TownGuardPayment( UI32 value ) -> void;
	UI32		TownGuardPayment() const;

	auto		RepMaxKills( UI16 value ) -> void;
	auto		RepMaxKills() const -> UI16;

	auto		ResLogs( SI16 value ) -> void;
	SI16		ResLogs() const;

	auto		ResLogTime( UI16 value ) -> void;
	auto		ResLogTime() const -> UI16;

	auto		ResOre( SI16 value ) -> void;
	SI16		ResOre() const;

	auto		ResOreTime( UI16 value ) -> void;
	auto		ResOreTime() const -> UI16;

	auto		ResourceAreaSize( UI16 value ) -> void;
	auto		ResourceAreaSize() const -> UI16;

	auto		ResFish( SI16 value ) -> void;
	SI16		ResFish() const;

	auto		ResFishTime( UI16 value ) -> void;
	auto		ResFishTime() const -> UI16;

	auto		AccountFlushTimer( R64 value ) -> void;
	R64			AccountFlushTimer() const;

	auto		TrackingBaseRange( UI16 value ) -> void;
	auto		TrackingBaseRange() const -> UI16;

	auto		TrackingMaxTargets( UI08 value ) -> void;
	UI08		TrackingMaxTargets() const;

	auto		TrackingBaseTimer( UI16 value ) -> void;
	auto		TrackingBaseTimer() const -> UI16;

	auto		TrackingRedisplayTime( UI16 value ) -> void;
	auto		TrackingRedisplayTime() const -> UI16;

	// Sept 22, 2002 - Support for HideWhileMounted fix.
	auto		CharHideWhileMounted( bool value ) -> void;
	auto		CharHideWhileMounted() const -> bool;

	auto		WeightPerStr( R32 newVal ) -> void;
	R32			WeightPerStr() const;

	auto		ServerOverloadPackets( bool newVal ) -> void;
	auto		ServerOverloadPackets() const -> bool;

	auto		ArmorAffectManaRegen( bool newVal ) -> void;
	auto		ArmorAffectManaRegen() const -> bool;

	auto		AdvancedPathfinding( bool value ) -> void;
	auto		AdvancedPathfinding() const -> bool;

	auto		LootingIsCrime( bool value ) -> void;
	auto		LootingIsCrime() const -> bool;

	auto		KickOnAssistantSilence( bool value ) -> void;
	auto		KickOnAssistantSilence() const -> bool;

	auto		DumpPaths() -> void;

	auto		ServerLocation( std::string toSet ) -> void;
	auto 		ServerLocation( size_t locNum ) ->__STARTLOCATIONDATA__ *;
	auto		NumServerLocations() const -> size_t;

	auto		YoungServerLocation( std::string toSet ) -> void;
	auto 		YoungServerLocation( size_t locNum ) ->__STARTLOCATIONDATA__ *;
	auto		NumYoungServerLocations() const -> size_t;

	auto		ServerSecondsPerUOMinute() const -> UI16;
	auto		ServerSecondsPerUOMinute( UI16 newVal ) -> void;

	auto		ServerLanguage() const -> UI16;
	auto		ServerLanguage( UI16 newVal ) -> void;

	UI32		MaxClientBytesIn() const;
	auto		MaxClientBytesIn( UI32 newVal ) -> void;

	UI32		MaxClientBytesOut() const;
	auto		MaxClientBytesOut( UI32 newVal ) -> void;

	UI32		NetTrafficTimeban() const;
	auto		NetTrafficTimeban( UI32 newVal ) -> void;

	auto		GetJSEngineSize() const -> UI16;
	auto		SetJSEngineSize( UI16 newVal ) -> void;

	auto		APSPerfThreshold() const -> UI16;
	auto		APSPerfThreshold( UI16 newVal ) -> void;
	auto		APSInterval() const -> UI16;
	auto		APSInterval( UI16 newVal ) -> void;
	auto		APSDelayStep() const -> UI16;
	auto		APSDelayStep( UI16 newVal ) -> void;
	auto		APSDelayMaxCap() const -> UI16;
	auto		APSDelayMaxCap( UI16 newVal ) -> void;

	SI16		ServerTimeDay() const;
	UI08		ServerTimeHours() const;
	UI08		ServerTimeMinutes() const;
	UI08		ServerTimeSeconds() const;
	auto		ServerTimeAMPM() const -> bool;

	auto		ServerTimeDay( SI16 nValue ) -> void;
	auto		ServerTimeHours( UI08 nValue ) -> void;
	auto		ServerTimeMinutes( UI08 nValue ) -> void;
	auto		ServerTimeSeconds( UI08 nValue ) -> void;
	auto		ServerTimeAMPM( bool nValue ) -> void;

	auto		SaveTime() -> void;
	auto		LoadTime() -> void;
	auto		LoadTimeTags( std::istream &input ) -> void;

	// These functions return true if it's a new day
	auto		IncSecond() -> bool;
	auto		IncMinute() -> bool;
	auto		IncHour() -> bool;
	auto		IncDay() -> bool;

	auto		IncMoon( SI32 mNumber ) -> void;
	
	auto		matchIP( const Ip4Addr_st &ip ) const -> Ip4Addr_st;

	PhysicalServer *ServerEntry( UI16 entryNum );
	auto			ServerCount() const -> UI16;

private:
	bool			resettingDefaults;

};

#endif

