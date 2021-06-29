
#ifndef __CSERVERDATA__
#define __CSERVERDATA__
#include <map>
#include <string>
#include <cstdint>
#include "StringUtility.hpp"

enum ClientFeatures
{
	CF_BIT_CHAT = 0,		// 0x01
	CF_BIT_UOR,				// 0x02
	CF_BIT_TD,				// 0x04
	CF_BIT_LBR,				// 0x08 - Enables LBR features: mp3s instead of midi, show new LBR monsters
	CF_BIT_AOS,				// 0x10 - Enable AoS monsters/map, AoS skills, Necro/Pala/Fight book stuff - works for 4.0+
	CF_BIT_SIXCHARS,		// 0x20
	CF_BIT_SE,				// 0x40
	CF_BIT_ML,				// 0x80
	CF_BIT_EIGHTAGE,		// 0x100
	CF_BIT_NINTHAGE,		// 0x200
	CF_BIT_TENTHAGE,		// 0x400
	CF_BIT_UNKNOWN1,		// 0x800 - Increased housing/bank storage?
	CF_BIT_SEVENCHARS,		// 0x1000
	//CF_BIT_KRFACES,		// 0x2000
	//CF_BIT_TRIAL,			// 0x4000
	CF_BIT_EXPANSION = 15,	// 0x8000
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
	SF_BIT_SIXCHARS,		// 0x40 - Use 6 character slots instead of 5
	SF_BIT_SE,				// 0x80 - Samurai and Ninja classes
	SF_BIT_ML,				// 0x100 - Elven race
	SF_BIT_UNKNOWN2,		// 0x200 - added with UO:KR launch
	SF_BIT_SEND3DTYPE,		// 0x400 - Send UO3D client type? KR and SA clients will send 0xE1)
	SF_BIT_UNKNOWN4,		// 0x800 - added sometime between UO:KR and UO:SA
	SF_BIT_SEVENCHARS,		// 0x1000 - Use 7 character slots instead of 5?6?, only 2D client?
	SF_BIT_UNKNOWN5,		// 0x2000 - added with UO:SA launch
	SF_BIT_NEWMOVE,			// 0x4000 - new movement system
	SF_BIT_FACTIONAREAS = 15,	// 0x8000 - Unlock new Felucca faction-areas
	SF_BIT_COUNT
};

enum AssistantFeatures : UI64
{
	AF_NONE = 0,

	// Razor
	AF_FILTERWEATHER = 1 << 0, // Weather Filter
	AF_FILTERLIGHT = 1 << 1, // Light Filter
	AF_SMARTTARGET = 1 << 2, // Smart Last Target
	AF_RANGEDTARGET = 1 << 3, // Range Check Last Target
	AF_AUTOOPENDOORS = 1 << 4, // Automatically Open Doors
	AF_DEQUIPONCAST = 1 << 5, // Unequip Weapon on spell cast
	AF_AUTOPOTIONEQUIP = 1 << 6, // Un/Re-equip weapon on potion use
	AF_POISONEDCHECKS = 1 << 7, // Block heal If poisoned/Macro IIf Poisoned condition/Heal or Cure self
	AF_LOOPEDMACROS = 1 << 8, // Disallow Looping macros, For loops, and macros that call other macros
	AF_USEONCEAGENT = 1 << 9, // The use once agent
	AF_RESTOCKAGENT = 1 << 10, // The restock agent
	AF_SELLAGENT = 1 << 11, // The sell agent
	AF_BUYAGENT = 1 << 12, // The buy agent
	AF_POTIONHOTKEYS = 1 << 13, // All potion hotkeys
	AF_RANDOMTARGETS = 1 << 14, // All random target hotkeys (Not target next, last target, target self)
	AF_CLOSESTTARGETS = 1 << 15, // All closest target hotkeys
	AF_OVERHEADHEALTH = 1 << 16, // Health and Mana/Stam messages shown over player's heads

	// AssistUO Only
	AF_AUTOLOOTAGENT = 1 << 17, // The autoloot agent
	AF_BONECUTTERAGENT = 1 << 18, // The bone cutter agent
	AF_JSCRIPTMACROS = 1 << 19, // Javascript macro engine
	AF_AUTOREMOUNT = 1 << 20, // Auto remount after dismount
	AF_ALL = 0xFFFFFFFFFFFFFFFF // Every feature possible
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
	tSERVER_POTION,				// Delay between using potions
	tSERVER_PETOFFLINECHECK,	// Delay between checks for the PetOfflineTimeout
	tSERVER_NPCFLAGUPDATETIMER, // Delay in seconds between each time NPC flags are updated
	tSERVER_BLOODDECAY,			// Delay in seconds before blood spatter spawned from combat decays
	tSERVER_BLOODDECAYCORPSE,	// Delay in seconds before blood spawned along with corpses decays
	tSERVER_NPCCORPSEDECAY,		// Delay in seconds before NPC corpses decay
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

class physicalServer
{
public:
	void setName(const std::string& newName);
	void setDomain(const std::string& newDomain);
	void setIP(const std::string& newIP);
	void setPort(UI16 newPort);
	std::string getName( void ) const;
	std::string getDomain( void ) const;
	std::string getIP( void ) const;
	UI16 getPort( void ) const;

private:
	std::string name;
	std::string domain;
	std::string ip;
	UI16 port;
};

class CServerData
{
private:

	std::bitset< CF_BIT_COUNT > clientFeatures;
	std::bitset< SF_BIT_COUNT > serverFeatures;
	std::bitset< 65 >	boolVals;						// Many values stored this way, rather than using bools.

	// ServerSystems
	std::string sServerName;					// 04/03/2004 - Need a place to store the name of the server (Added to support the UOG Info Request)
	UI16		port;							//	Port number that the server listens on, for connections
	std::string externalIP;
	std::vector< physicalServer > serverList;	//	Series of server entries for shard list
	UI08		consolelogenabled;				//	Various levels of legging 0 == none, 1 == normal, 2 == normal + all speech
	char		commandprefix;					//	Character that acts as the command prefix
	SI16		backupRatio;					//	Number of saves before a backup occurs
	UI32		serversavestimer;				//	Number of seconds between world saves
	UI32		netRcvTimeout;					// 04/03/2004 - Used to be hardcoded as 2 seconds (2 * 1000ms) for some raly laggy nets this would drop alot of packets, and disconnect people.
	UI32		netSndTimeout;					// 04/03/2004 - Not used at this time.
	UI32		netRetryCount;					// 04/03/2004 - Used to set the number of times a network recieve will be attempted before it throws an error
	bool		uogEnabled;						// 04/03/2004 - Added to support the UOG Info Request Service
	bool		randomStartingLocation;			// Enable or disable randomizing starting location for new players based on starting location entries
	UI16		jsEngineSize;					// gcMaxBytes limit in MB per JS runtime

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
	std::vector< std::string > facetNameList;
	std::vector< UI32 > facetAccessFlags;

	// Skills & Stats
	UI16		skilltotalcap;					//	A cap on the total of all a PC's skills
	UI16		skillcap;						//	A cap on each individual PC skill
	UI08		skilldelay;						//	Number of seconds after a skill is used before another skill can be used
	UI16		statcap;						//	A cap on the total of a PC's stats
	SI16		maxstealthmovement;				//	Max number of steps allowed with stealth skill at 100.0
	SI16		maxstaminamovement;				//	Max number of steps allowed while running before stamina is reduced

	// ServerTimers
	UI16		serverTimers[tSERVER_COUNT];
	// Directories
	std::string serverDirectories[CSDDP_COUNT];

	// Settings
	SI16		ambientsounds;					//	Ambient sounds - values from 1->10 - higher values indicate sounds occur less often
	SI16		htmlstatusenabled;				//	If > 0 then it's enabled - only used at PC char creation - use elsewhere? (was # of seconds between updates)
	SI16		sellmaxitems;					//	Maximum number of items that can be sold to a vendor
	R32			weightPerSTR;					//	How much weight per point of STR a character can hold.
	UI16		petOfflineTimeout;				//	Offline time after a player looses all pets
	bool		paperdollGuildButton;			//	Enable Guild-button on paperdoll to access guild-menus without going through guildstone
	SI16		fishingstaminaloss;				//	The amount of stamina lost with each use of fishing skill
	UI16		maxPlayerPackItems;				//	The max amount of items a player's backpack can contain
	UI16		maxPlayerBankItems;				//	The max amount of items a player's bankbox can contain

	// SpeedUp
	R64			checkitems;						//	How often (in seconds) items are checked for decay and other things
	R64			checkboats;						//	How often (in seconds) boats are checked for motion and so forth
	R64			checknpcai;						//	How often (in seconds) NPCs can execute an AI routine
	R64			checkspawnregions;				//	How often (in seconds) spawn regions are checked for new spawns
	R32			npcWalkingSpeed;				//	Speed at which walking NPCs move
	R32			npcRunningSpeed;				//	Speed at which running NPCs move
	R32			npcFleeingSpeed;				//	Speed at which fleeing NPCs move
	R32			npcMountedWalkingSpeed;			//	Speed at which (mounted) walking NPCs move
	R32			npcMountedRunningSpeed;			//	Speed at which (mounted) running NPCs move
	R32			npcMountedFleeingSpeed;			//	Speed at which (mounted) fleeing NPCs move
	R64			flushTime;						//	How often (in minutes) online accounts are checked to see if they really ARE online
	R32			globalattackspeed;				//  Global attack speed that can be tweaked to quickly increase or decrease overall combat speed. Defaults to 1.0
	R32			npcspellcastspeed;				//  For adjusting the overall speed of (or delay between) NPC spell casts. Defaults to 1.0

	// MessageBoards
	UI08		msgpostinglevel;				//	If not 0, then players can post
	UI08		msgremovallevel;				//	If not 0, then players can remove posts

	// WorldLight
	LIGHTLEVEL	dungeonlightlevel;				//	Default light level for a dungeon, if not subject to a weather system
	LIGHTLEVEL	currentlightlevel;				//	Default current light level if not subject to a weather system
	LIGHTLEVEL	brightnesslightlevel;			//	Default brightest light level if not subject to a weather system
	LIGHTLEVEL	darknesslightlevel;				//	Default darkest light level if not subject to a weather system

	// WorldTimer								//	days/hours/minutes/seconds to it's own file?
	UI16		secondsperuominute;				//	Number of seconds for a UOX minute.
	SI16		moon[2];						//	Moon current state
	SI16		days;							//	Number of days the world has been running (persistent)
	UI08		hours;							//	Number of hours the world has been running (persistent)
	UI08		minutes;						//	Number of minutes the world has been running (persistent)
	UI08		seconds;						//	Number of seconds the world has been running (persistent)
	bool		ampm;							//	Whether our current time is in the morning or afternoon

	// Tracking
	UI16		trackingbaserange;				//	Minimum range even a novice can track from
	UI16		trackingbasetimer;				//	Tracking timer - maximum time for a GM to track at
	UI08		trackingmaxtargets;				//	Maximum number of targets that can be tracked
	UI16		trackingmsgredisplaytimer;		//	How often (in seconds) the tracking message is redisplayed

	// Reputation
	UI16		maxmurdersallowed;				//	Maximum number of kills before turning red

	// Resources
	UI08		minecheck;						//	Type of mining check performed - 0 anywhere 1 only mountains/floors 2 region based (not working)
	SI16		oreperarea;						//	TIMERVAL? Maximum number of ores in a resource area
	UI16		orerespawntimer;				//	Time at which ore is respawned (only 1 ore, not all)
	UI16		orerespawnarea;					//	Number of ore areas (ie Y x Y where Y == value)
	SI16		logsperarea;					//	Maximum number of logs in a resource area
	UI16		logsrespawntimer;				//	TIMERVAL? Time at which logs are respawned (only 1 log, not all)
	UI16		logsrespawnarea;				//	Number of log areas (ie Y x Y where Y == value)

	// Hunger & Food
	SI16		hungerdamage;					//	Amount of damage applied if hungry and below threshold
	// Thirst
	SI16		thirstdrain;					//  Amount of stamina drained if thirsty and below threshold

	// Combat
	SI16		combatmaxrange;					//	RANGE?  Range at which combat can actually occur
	SI16		combatmaxspellrange;			//	RANGE?  Range at which spells can be cast
	UI08		combatanimalattackchance;		//	Chance of animals being attacked (0-100)
	SI16		combatnpcdamagerate;			//	NPC Damage divisor - PCs sustain less than NPCs.  If a PC, damage is 1/value
	SI16		combatnpcbasefleeat;			//	% of HP where an NPC will flee, if it's not defined for them
	SI16		combatnpcbasereattackat;		//	% of HP where an NPC will resume attacking
	SI16		combatattackstamina;			//	Amount of stamina lost when hitting an opponent
	UI16		combatExplodeDelay;				//	Time from casting to actual explosion
	UI08		combatweapondamagechance;		//  Chance of weapons being damaged when attacking in combat (0-100)
	UI08		combatweapondamagemin;			//  Minimum amount of hitpoints a weapon can lose when being damaged in combat
	UI08		combatweapondamagemax;			//  Maximum amount of hitpoints a weapon can lose when being damaged in combat
	UI08		combatarmordamagechance;		//  Chance of armor being damaged when defending in combat (0-100)
	UI08		combatarmordamagemin;			//  Minimum amount of hitpoints an armor can lose when being damaged in combat
	UI08		combatarmordamagemax;			//  Maximum amount of hitpoints an armor can lose when being damaged in combat
	UI08		combatbloodeffectchance;		//  Chance of blood splatter effects spawning during combat
	UI08		alchemyDamageBonusModifier;		//  Modifier used to calculate bonus damage for explosion potions based on alchemy skill

	// Start & Location Settings
	std::vector< STARTLOCATION >	startlocations;
	SI16		startgold;						//	Amount of gold created when a PC is made
	UI16		startprivs;						//	Starting privileges of characters

	// Anything under this comment is left here for symantics
	UI08		skilllevel;						//	Some skill value associated with the rank system
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
	UI32		numSecsAsMayor;					//	Time (in seconds) that a PC would be a mayor
	UI32		taxPeriod;						//	Time (in seconds) between periods of taxes for PCs
	UI32		guardPayment;					//	Time (in seconds) between payments for guards

	void	PostLoadDefaults( void );

	std::map<std::string,std::int32_t>   	uox3inicasevalue;
	void		regAllINIValues() ;
	void		regINIValue(const std::string& tag, std::int32_t value);
public:
	std::int32_t lookupINIValue(const std::string& tag);
	UI64		DisabledAssistantFeatures;

	void		SetServerFeature( ServerFeatures, bool );
	void		SetServerFeatures( size_t );
	bool		GetServerFeature( ServerFeatures ) const;
	size_t		GetServerFeatures( void ) const;

	void		SetClientFeature( ClientFeatures, bool );
	void		SetClientFeatures( UI32 );
	bool		GetClientFeature( ClientFeatures ) const;
	UI32		GetClientFeatures( void ) const;

	void		SetDisabledAssistantFeature( AssistantFeatures, bool );
	void		SetDisabledAssistantFeatures( UI64 );
	bool		GetDisabledAssistantFeature( AssistantFeatures ) const;
	UI64		GetDisabledAssistantFeatures( void ) const;

	void		SetAssistantNegotiation( bool value );
	bool		GetAssistantNegotiation( void ) const;

	void		SetClassicUOMapTracker( bool value );
	bool		GetClassicUOMapTracker( void ) const;

	SI16		ServerMoon( SI16 slot ) const;
	LIGHTLEVEL	WorldLightDarkLevel( void ) const;
	LIGHTLEVEL	WorldLightBrightLevel( void ) const;
	LIGHTLEVEL	WorldLightCurrentLevel( void ) const;
	LIGHTLEVEL	DungeonLightLevel( void ) const;
	UI16		ServerStartPrivs( void ) const;
	SI16		ServerStartGold( void ) const;

	void		ServerMoon( SI16 slot, SI16 value );
	void		WorldLightDarkLevel( LIGHTLEVEL value );
	void		WorldLightBrightLevel( LIGHTLEVEL value );
	void		WorldLightCurrentLevel( LIGHTLEVEL value );
	void		DungeonLightLevel( LIGHTLEVEL value );
	void		ServerStartPrivs( UI16 value );
	void		ServerStartGold( SI16 value );
	bool		ParseINI( const std::string& filename );
	bool		HandleLine( const std::string& tag, const std::string& value );

	void		Load( void );
	bool		save( void );
	bool		save( std::string filename );

	void		ResetDefaults( void );

	//void		RefreshIPs( void );

	CServerData( void );
	~CServerData();
	void		ServerName( std::string setname );
	void		ServerDomain( std::string setdomain );
	void		ServerIP( std::string setip );
	std::string ServerName( void ) const;
	std::string ServerDomain( void ) const;
	std::string ServerIP( void ) const;
	std::string ExternalIP() const;
	void 		ExternalIP( const std::string &ip );
	void		ServerPort( UI16 setport );
	UI16		ServerPort( void ) const;
	void		ServerConsoleLog( bool setting );
	bool		ServerConsoleLog( void ) const;
	void		ServerNetworkLog( bool setting );
	bool		ServerNetworkLog( void ) const;
	void		ServerSpeechLog( bool setting );
	bool		ServerSpeechLog( void ) const;
	void		ServerCommandPrefix( char cmdvalue );
	char		ServerCommandPrefix( void ) const;
	void		ServerAnnounceSaves( bool setting );
	bool		ServerAnnounceSavesStatus( void ) const;
	void		ServerJoinPartAnnouncements( bool setting );
	bool		ServerJoinPartAnnouncementsStatus( void ) const;
	void		ServerMulCaching( bool setting );
	bool		ServerMulCachingStatus( void ) const;
	void		ServerBackups( bool setting );
	bool		ServerBackupStatus( void ) const;
	void		ServerSavesTimer( UI32 timer );
	UI32		ServerSavesTimerStatus( void ) const;
	void		ServerMainThreadTimer( UI32 threadtimer );
	UI32		ServerMainThreadTimerStatus( void ) const;
	void		ServerSkillTotalCap( UI16 cap );
	UI16		ServerSkillTotalCapStatus( void ) const;
	void		ServerSkillCap( UI16 cap );
	UI16		ServerSkillCapStatus( void ) const;
	void		ServerSkillDelay( UI08 skdelay );
	UI08		ServerSkillDelayStatus( void ) const;
	void		ServerStatCap( UI16 cap );
	UI16		ServerStatCapStatus( void ) const;
	void		MaxStealthMovement( SI16 value );
	SI16		MaxStealthMovement( void ) const;
	void		MaxStaminaMovement( SI16 value );
	SI16		MaxStaminaMovement( void ) const;
	void		SystemTimer( cSD_TID timerid, UI16 value );
	UI16		SystemTimer( cSD_TID timerid ) const;
	TIMERVAL	BuildSystemTimeValue( cSD_TID timerID ) const;

	bool		ServerUOGEnabled(void) const { return uogEnabled; }
	void		ServerUOGEnabled(bool uogValue) {	uogEnabled = uogValue; }
	void		ConnectUOServerPoll( bool value );
	bool		ConnectUOServerPoll( void ) const;
	bool		ServerRandomStartingLocation( void ) const { return randomStartingLocation; }
	void		ServerRandomStartingLocation( bool rndStartLocValue ) { randomStartingLocation = rndStartLocValue; }
	UI32		ServerNetRetryCount(void) const { return netRetryCount; }
	void		ServerNetRetryCount(UI32 retryValue) { netRetryCount = retryValue; }
	UI32		ServerNetRcvTimeout(void) const { return netRcvTimeout; }
	void		ServerNetRcvTimeout(UI32 timeoutValue) { netRcvTimeout = timeoutValue; }
	UI32		ServerNetSndTimeout(void) const { return netSndTimeout; }
	void		ServerNetSndTimeout(UI32 timeoutValue) { netSndTimeout = timeoutValue; }

	// ClientSupport used to determine login-restrictions
	bool		ClientSupport4000(void) const { return Clients4000Enabled; }
	void		ClientSupport4000(bool cliSuppValue) { Clients4000Enabled = cliSuppValue; }
	bool		ClientSupport5000(void) const { return Clients5000Enabled; }
	void		ClientSupport5000(bool cliSuppValue) { Clients5000Enabled = cliSuppValue; }
	bool		ClientSupport6000(void) const { return Clients6000Enabled; }
	void		ClientSupport6000(bool cliSuppValue) { Clients6000Enabled = cliSuppValue; }
	bool		ClientSupport6050(void) const { return Clients6050Enabled; }
	void		ClientSupport6050(bool cliSuppValue) { Clients6050Enabled = cliSuppValue; }
	bool		ClientSupport7000(void) const { return Clients7000Enabled; }
	void		ClientSupport7000(bool cliSuppValue) { Clients7000Enabled = cliSuppValue; }
	bool		ClientSupport7090(void) const { return Clients7090Enabled; }
	void		ClientSupport7090(bool cliSuppValue) { Clients7090Enabled = cliSuppValue; }
	bool		ClientSupport70160(void) const { return Clients70160Enabled; }
	void		ClientSupport70160(bool cliSuppValue) { Clients70160Enabled = cliSuppValue; }
	bool		ClientSupport70240(void) const { return Clients70240Enabled; }
	void		ClientSupport70240(bool cliSuppValue) { Clients70240Enabled = cliSuppValue; }
	bool		ClientSupport70300( void ) const { return Clients70300Enabled; }
	void		ClientSupport70300( bool cliSuppValue ) { Clients70300Enabled = cliSuppValue; }
	bool		ClientSupport70331( void ) const { return Clients70331Enabled; }
	void		ClientSupport70331( bool cliSuppValue ) { Clients70331Enabled = cliSuppValue; }
	bool		ClientSupport704565( void ) const { return Clients704565Enabled; }
	void		ClientSupport704565( bool cliSuppValue ) { Clients704565Enabled = cliSuppValue; }
	bool		ClientSupport70610( void ) const { return Clients70610Enabled; }
	void		ClientSupport70610( bool cliSuppValue ) { Clients70610Enabled = cliSuppValue; }

	// Enable/disable higher total for starting stats, and/or fourth starting skill in clients 7.0.16+
	void		ExtendedStartingStats( bool setting );
	bool		ExtendedStartingStats( void ) const;
	void		ExtendedStartingSkills( bool setting );
	bool		ExtendedStartingSkills( void ) const;

	// Define all Path Get/Set's here please
	void		Directory( CSDDirectoryPaths dp, std::string value );
	std::string Directory( CSDDirectoryPaths dp );


	void		CorpseLootDecay( bool value );
	bool		CorpseLootDecay( void ) const;

	void		GuardStatus( bool value );
	bool		GuardsStatus( void ) const;

	void		DeathAnimationStatus( bool value );
	bool		DeathAnimationStatus( void ) const;

	void		WorldAmbientSounds( SI16 value );
	SI16		WorldAmbientSounds( void ) const;

	void		AmbientFootsteps( bool value );
	bool		AmbientFootsteps( void ) const;

	void		InternalAccountStatus( bool value );
	bool		InternalAccountStatus( void ) const;

	void		ShowOfflinePCs( bool value );
	bool		ShowOfflinePCs( void ) const;

	void		RogueStatus( bool value );
	bool		RogueStatus( void ) const;

	void		SnoopIsCrime( bool value );
	bool		SnoopIsCrime( void ) const;

	void		PlayerPersecutionStatus( bool value );
	bool		PlayerPersecutionStatus( void ) const;

	void		HtmlStatsStatus( SI16 value );
	SI16		HtmlStatsStatus( void ) const;

	void		SellByNameStatus( bool value );
	bool		SellByNameStatus( void ) const;

	void		SellMaxItemsStatus( SI16 value );
	SI16		SellMaxItemsStatus( void ) const;

	void		TradeSystemStatus( bool value );
	bool		TradeSystemStatus( void ) const;

	void		RankSystemStatus( bool value );
	bool		RankSystemStatus( void ) const;

	void		CutScrollRequirementStatus( bool value );
	bool		CutScrollRequirementStatus( void ) const;

	void		NPCTrainingStatus( bool setting );
	bool		NPCTrainingStatus( void ) const;

	void		CheckItemsSpeed( R64 value );
	R64			CheckItemsSpeed( void ) const;

	void		CheckBoatSpeed( R64 value );
	R64			CheckBoatSpeed( void ) const;

	void		CheckNpcAISpeed( R64 value );
	R64			CheckNpcAISpeed( void ) const;

	void		CheckSpawnRegionSpeed( R64 value );
	R64			CheckSpawnRegionSpeed( void ) const;

	void		GlobalAttackSpeed( R32 value );
	R32			GlobalAttackSpeed( void ) const;

	void		NPCSpellCastSpeed( R32 value );
	R32			NPCSpellCastSpeed( void ) const;

	void		MsgBoardPostingLevel( UI08 value );
	UI08		MsgBoardPostingLevel( void ) const;

	void		MsgBoardPostRemovalLevel( UI08 value );
	UI08		MsgBoardPostRemovalLevel( void ) const;

	void		MineCheck( UI08 value );
	UI08		MineCheck( void ) const;

	void		AlchemyDamageBonusEnabled( bool value );
	bool		AlchemyDamageBonusEnabled( void ) const;

	void		AlchemyDamageBonusModifier( UI08 value );
	UI08		AlchemyDamageBonusModifier( void ) const;

	void		CombatDisplayHitMessage( bool value );
	bool		CombatDisplayHitMessage( void ) const;

	void		CombatDisplayDamageNumbers( bool value );
	bool		CombatDisplayDamageNumbers( void ) const;

	void		CombatAttackSpeedFromStamina( bool value );
	bool		CombatAttackSpeedFromStamina( void ) const;

	void		FishingStaminaLoss( SI16 value );
	SI16		FishingStaminaLoss( void ) const;

	void		CombatAttackStamina( SI16 value );
	SI16		CombatAttackStamina( void ) const;

	void		CombatNPCDamageRate( SI16 value );
	SI16		CombatNPCDamageRate( void ) const;

	UI08		SkillLevel( void ) const;
	void		SkillLevel( UI08 value );

	void		EscortsEnabled( bool value );
	bool		EscortsEnabled( void ) const;

	void		ItemsDetectSpeech( bool value );
	bool		ItemsDetectSpeech( void ) const;

	void		ForceNewAnimationPacket( bool value );
	bool		ForceNewAnimationPacket( void ) const;

	void		MapDiffsEnabled( bool value );
	bool		MapDiffsEnabled( void ) const;

	void		MaxPlayerPackItems( UI16 value );
	UI16		MaxPlayerPackItems( void ) const;

	void		MaxPlayerBankItems( UI16 value );
	UI16		MaxPlayerBankItems( void ) const;

	void		BasicTooltipsOnly( bool value );
	bool		BasicTooltipsOnly( void ) const;

	void		GlobalItemDecay( bool value );
	bool		GlobalItemDecay( void ) const;

	void		ScriptItemsDecayable( bool value );
	bool		ScriptItemsDecayable( void ) const;

	void		BaseItemsDecayable( bool value );
	bool		BaseItemsDecayable( void ) const;

	void		ItemDecayInHouses( bool value );
	bool		ItemDecayInHouses( void ) const;

	void		ProtectPrivateHouses( bool value );
	bool		ProtectPrivateHouses( void ) const;

	void		TrackHousesPerAccount( bool value );
	bool		TrackHousesPerAccount( void ) const;

	void		CanOwnAndCoOwnHouses( bool value );
	bool		CanOwnAndCoOwnHouses( void ) const;

	void		CoOwnHousesOnSameAccount( bool value );
	bool		CoOwnHousesOnSameAccount( void ) const;

	void		MaxHousesOwnable( UI16 value );
	UI16		MaxHousesOwnable( void ) const;

	void		MaxHousesCoOwnable( UI16 value );
	UI16		MaxHousesCoOwnable( void ) const;

	void		PaperdollGuildButton( bool value );
	bool		PaperdollGuildButton( void) const;

	void		CombatMonstersVsAnimals( bool value );
	bool		CombatMonstersVsAnimals( void ) const;

	void		CombatAnimalsAttackChance( UI16 value );
	UI16		CombatAnimalsAttackChance( void ) const;

	void		CombatWeaponDamageChance( UI08 value );
	UI08		CombatWeaponDamageChance( void ) const;

	void		CombatWeaponDamageMin( UI08 value );
	UI08		CombatWeaponDamageMin( void ) const;

	void		CombatWeaponDamageMax( UI08 value );
	UI08		CombatWeaponDamageMax( void ) const;

	void		CombatArmorDamageChance( UI08 value );
	UI08		CombatArmorDamageChance( void ) const;

	void		CombatArmorDamageMin( UI08 value );
	UI08		CombatArmorDamageMin( void ) const;

	void		CombatArmorDamageMax( UI08 value );
	UI08		CombatArmorDamageMax( void ) const;

	void		CombatBloodEffectChance( UI08 value );
	UI08		CombatBloodEffectChance( void ) const;

	void		TravelSpellsFromBoatKeys( bool value );
	bool		TravelSpellsFromBoatKeys( void ) const;

	void		TravelSpellsWhileOverweight( bool value );
	bool		TravelSpellsWhileOverweight( void ) const;

	void		MarkRunesInMultis( bool value );
	bool		MarkRunesInMultis( void ) const;

	void		TravelSpellsBetweenWorlds( bool value );
	bool		TravelSpellsBetweenWorlds( void ) const;

	void		TravelSpellsWhileAggressor( bool value );
	bool		TravelSpellsWhileAggressor( void ) const;

	void		HungerSystemEnabled( bool value );
	bool		HungerSystemEnabled( void ) const;

	void		ThirstSystemEnabled( bool value );
	bool		ThirstSystemEnabled( void ) const;

	void		HungerDamage( SI16 value );
	SI16		HungerDamage( void ) const;

	void		ThirstDrain( SI16 value );
	SI16		ThirstDrain( void ) const;

	void		PetHungerOffline( bool value );
	bool		PetHungerOffline( void ) const;

	void		PetThirstOffline( bool value );
	bool		PetThirstOffline( void ) const;

	void		PetOfflineTimeout( UI16 value );
	UI16		PetOfflineTimeout( void ) const;

	void		BuyThreshold( SI16 value );
	SI16		BuyThreshold( void ) const;

	void		BackupRatio( SI16 value );
	SI16		BackupRatio( void ) const;

	void		CombatMaxRange( SI16 value );
	SI16		CombatMaxRange( void ) const;

	void		CombatMaxSpellRange( SI16 value );
	SI16		CombatMaxSpellRange( void ) const;

	void		CombatExplodeDelay( UI16 value );
	UI16		CombatExplodeDelay( void ) const;

	void		CombatAnimalsGuarded( bool value );
	bool		CombatAnimalsGuarded( void ) const;

	void		CombatNPCBaseFleeAt( SI16 value );
	SI16		CombatNPCBaseFleeAt( void ) const;

	void		CombatNPCBaseReattackAt( SI16 value );
	SI16		CombatNPCBaseReattackAt( void ) const;

	void		ShootOnAnimalBack( bool setting );
	bool		ShootOnAnimalBack( void ) const;

	void		NPCWalkingSpeed( R32 value );
	R32			NPCWalkingSpeed( void ) const;

	void		NPCRunningSpeed( R32 value );
	R32			NPCRunningSpeed( void ) const;

	void		NPCFleeingSpeed( R32 value );
	R32			NPCFleeingSpeed( void ) const;

	void		NPCMountedWalkingSpeed( R32 value );
	R32			NPCMountedWalkingSpeed( void ) const;

	void		NPCMountedRunningSpeed( R32 value );
	R32			NPCMountedRunningSpeed( void ) const;

	void		NPCMountedFleeingSpeed( R32 value );
	R32			NPCMountedFleeingSpeed( void ) const;

	void		TitleColour( UI16 value );
	UI16		TitleColour( void ) const;

	void		LeftTextColour( UI16 value );
	UI16		LeftTextColour( void ) const;

	void		RightTextColour( UI16 value );
	UI16		RightTextColour( void ) const;

	void		ButtonCancel( UI16 value );
	UI16		ButtonCancel( void ) const;

	void		ButtonLeft( UI16 value );
	UI16		ButtonLeft( void ) const;

	void		ButtonRight( UI16 value );
	UI16		ButtonRight( void ) const;

	void		BackgroundPic( UI16 value );
	UI16		BackgroundPic( void ) const;

	void		TownNumSecsPollOpen( UI32 value );
	UI32		TownNumSecsPollOpen( void ) const;

	void		TownNumSecsAsMayor( UI32 value );
	UI32		TownNumSecsAsMayor( void ) const;

	void		TownTaxPeriod( UI32 value );
	UI32		TownTaxPeriod( void ) const;

	void		TownGuardPayment( UI32 value );
	UI32		TownGuardPayment( void ) const;

	void		RepMaxKills( UI16 value );
	UI16		RepMaxKills( void ) const;

	void		ResLogs( SI16 value );
	SI16		ResLogs( void ) const;

	void		ResLogTime( UI16 value );
	UI16		ResLogTime( void ) const;

	void		ResLogArea( UI16 value );
	UI16		ResLogArea( void ) const;

	void		ResOre( SI16 value );
	SI16		ResOre( void ) const;

	void		ResOreTime( UI16 value );
	UI16		ResOreTime( void ) const;

	void		ResOreArea( UI16 value );
	UI16		ResOreArea( void ) const;

	void		AccountFlushTimer( R64 value );
	R64			AccountFlushTimer( void ) const;

	void		TrackingBaseRange( UI16 value );
	UI16		TrackingBaseRange( void ) const;

	void		TrackingMaxTargets( UI08 value );
	UI08		TrackingMaxTargets( void ) const;

	void		TrackingBaseTimer( UI16 value );
	UI16		TrackingBaseTimer( void ) const;

	void		TrackingRedisplayTime( UI16 value );
	UI16		TrackingRedisplayTime( void ) const;

	// Sept 22, 2002 - Support for HideWhileMounted fix.
	void		CharHideWhileMounted( bool value );
	bool		CharHideWhileMounted( void ) const;

	void		WeightPerStr( R32 newVal );
	R32			WeightPerStr( void ) const;

	void		ServerOverloadPackets( bool newVal );
	bool		ServerOverloadPackets( void ) const;

	void		ArmorAffectManaRegen( bool newVal );
	bool		ArmorAffectManaRegen( void ) const;

	void		AdvancedPathfinding( bool value );
	bool		AdvancedPathfinding( void ) const;

	void		LootingIsCrime( bool value );
	bool		LootingIsCrime( void ) const;

	void		KickOnAssistantSilence( bool value );
	bool		KickOnAssistantSilence( void ) const;

	void		dumpPaths( void );

	void			ServerLocation( std::string toSet );
	LPSTARTLOCATION ServerLocation( size_t locNum );

	size_t			NumServerLocations( void ) const;

	UI16			ServerSecondsPerUOMinute( void ) const;
	void			ServerSecondsPerUOMinute( UI16 newVal );

	UI16			GetJSEngineSize( void ) const;
	void			SetJSEngineSize( UI16 newVal );

	SI16			ServerTimeDay( void ) const;
	UI08			ServerTimeHours( void ) const;
	UI08			ServerTimeMinutes( void ) const;
	UI08			ServerTimeSeconds( void ) const;
	bool			ServerTimeAMPM( void ) const;

	void			ServerTimeDay( SI16 nValue );
	void			ServerTimeHours( UI08 nValue );
	void			ServerTimeMinutes( UI08 nValue );
	void			ServerTimeSeconds( UI08 nValue );
	void			ServerTimeAMPM( bool nValue );

	void			SaveTime( void );
	void			LoadTime( void );
	void			LoadTimeTags( std::ifstream &input );

	// These functions return TRUE if it's a new day
	bool			incSecond( void );
	bool			incMinute( void );
	bool			incHour( void );
	bool			incDay( void );

	void			incMoon( SI32 mNumber );

	physicalServer *ServerEntry( UI16 entryNum );
	UI16			ServerCount( void ) const;

private:
	bool			resettingDefaults;

};

#endif

