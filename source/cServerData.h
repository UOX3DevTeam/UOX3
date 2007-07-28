
#ifndef __CSERVERDATA__
#define __CSERVERDATA__

namespace UOX
{

	enum ClientFeatures
	{
		CF_BIT_CHAT = 0,
		CF_BIT_LBR,
		CF_BIT_UNKNOWN,
		CF_BIT_UNKNOWN2,
		CF_BIT_AOS,
		CF_BIT_SIXCHARS,
		CF_BIT_SE,
		CF_BIT_ML,
		CF_BIT_EXPANSION = 15,
		CF_BIT_COUNT
	};

	enum ServerFeatures
	{
		SF_BIT_UNKNOWN1 = 0,
		SF_BIT_IGR,
		SF_BIT_CHARLIMIT,
		SF_BIT_CONTEXTMENUS,
		SF_BIT_ONECHAR,
		SF_BIT_AOS,
		SF_BIT_SIXCHARS,
		SF_BIT_SE,
		SF_BIT_ML,
		SF_BIT_COUNT
	};

enum cSD_TID
{
	tSERVER_ERROR = -1,
	tSERVER_CORPSEDECAY = 0,	// Amount of time for a corpse to decay.
	tSERVER_WEATHER,			// Amount of time between changing light levels (day cycles).
	tSERVER_SHOPSPAWN,			// Amount of time between shopkeeper restocks.
	tSERVER_DECAY,				// Amount of time a decayable item will remain on the ground before dissapearing.
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
	tSERVER_POLYMORPH,			// Duration of the polymorph spell.
	tSERVER_ESCORTWAIT,			// Amount of time until an escort NPC will dissapear while waiting for a player to start his quest.
	tSERVER_ESCORTACTIVE,		// Amount of time until an escort NPC will dissapear while a player is escorting him.
	tSERVER_ESCORTDONE,			// Amount of time until an escort NPC will dissapear when his quest is finished.
	tSERVER_MURDERDECAY,		// Amount of time before a permanent murder count will decay.
	tSERVER_CRIMINAL,			// Amount of time a character remains criminal after committing a criminal act.
	tSERVER_POTION,				// Delay between using potions
	tSERVER_PETOFFLINECHECK,	// Delay between checks for the PetOfflineTimeout
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
	CSDDP_COUNT
};

class physicalServer
{
public:
  void setName(const std::string newName);
  void setDomain(const std::string newDomain);
  void setIP(const std::string newIP);
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
	std::bitset< 32 >	boolVals;						// Many values stored this way, rather than using bools.

	// ServerSystems
	std::string sServerName;					// 04/03/2004 - Need a place to store the name of the server (Added to support the UOG Info Request)
	UI16		port;							//	Port number that the server listens on, for connections
	std::vector< physicalServer > serverList;	//	Series of server entries for shard list
	UI08		consolelogenabled;				//	Various levels of legging 0 == none, 1 == normal, 2 == normal + all speech
	UI08		crashprotectionenabled;			//	Level of crash protection - number of crash restart attempts before stopping - NEVER ACTIVELY USED
	char		commandprefix;					//	Character that acts as the command prefix
	SI16		backupRatio;					//	Number of saves before a backup occurs
	UI32		serversavestimer;				//	Number of seconds between world saves
	UI32		netRcvTimeout;					// 04/03/2004 - Used to be hardcoded as 2 seconds (2 * 1000ms) for some raly laggy nets this would drop alot of packets, and disconnect people.
	UI32		netSndTimeout;					// 04/03/2004 - Not used at this time.
	UI32		netRetryCount;					// 04/03/2004 - Used to set the number of times a network recieve will be attempted before it throws an error
	bool		uogEnabled;						// 04/03/2004 - Added to support the UOG Info Request Service

	// facet block
	bool		useFacetSaves;
	std::vector< std::string > facetNameList;
	std::vector< UI32 > facetAccessFlags;

	// Skills & Stats
	UI16		skillcap;						//	A cap on the total of all a PC's skills
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
	UI08		weightPerSTR;					//	How much weight per point of STR a character can hold.
	UI16		petOfflineTimeout;				//	Offline time after a player looses all pets

	// SpeedUp
	R64			checkitems;						//	How often (in seconds) items are checked for decay and other things
	R64			checkboats;						//	How often (in seconds) boats are checked for motion and so forth
	R64			checknpcai;						//	How often (in seconds) NPCs can execute an AI routine
	R64			checkspawnregions;				//	How often (in seconds) spawn regions are checked for new spawns
	R32			npcWalkingSpeed;				//	Speed at which walking NPCs move
	R32			npcRunningSpeed;				//	Speed at which running NPCs move
	R32			npcFleeingSpeed;				//	Speed at which fleeing NPCs move
	R64			flushTime;						//	How often (in minutes) online accounts are checked to see if they really ARE online

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

	// Combat
	SI16		combatmaxrange;					//	RANGE?  Range at which combat can actually occur
	SI16		combatarcherrange;				//	RANGE?  Range at which archers stop charging the enemy
	SI16		combatmaxspellrange;			//	RANGE?  Range at which spells can be cast
	UI08		combatanimalattackchance;		//	Chance of animals being attacked (0-100)
	SI16		combatnpcdamagerate;			//	NPC Damage divisor - PCs sustain less than NPCs.  If a PC, damage is 1/value
	SI16		combatnpcbasefleeat;			//	% of HP where an NPC will flee, if it's not defined for them
	SI16		combatnpcbasereattackat;		//	% of HP where an NPC will resume attacking
	SI16		combatattackstamina;			//	Amount of stamina lost when hitting an opponent
	UI16		combatExplodeDelay;				//	Time from casting to actual explosion

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

	// Townstone stuff
	UI32		numSecsPollOpen;				//	Time (in seconds) for which a town voting poll is open
	UI32		numSecsAsMayor;					//	Time (in seconds) that a PC would be a mayor
	UI32		taxPeriod;						//	Time (in seconds) between periods of taxes for PCs 
	UI32		guardPayment;					//	Time (in seconds) between payments for guards

	void	PostLoadDefaults( void );

public:
	void		SetServerFeature( ServerFeatures, bool );
	void		SetServerFeatures( size_t );
	bool		GetServerFeature( ServerFeatures ) const;
	size_t		GetServerFeatures( void ) const;

	void		SetClientFeature( ClientFeatures, bool );
	void		SetClientFeatures( UI16 );
	bool		GetClientFeature( ClientFeatures ) const;
	UI16		GetClientFeatures( void ) const;

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
	bool		ParseINI( const std::string filename );
	void		HandleLine( const UString tag, const UString value );

	void		Load( void );
	bool		save( void );
	bool		save( std::string filename );

	void		ResetDefaults( void );

	void		RefreshIPs( void );

				CServerData( void );
				~CServerData();
	void		ServerName( std::string setname );
	void		ServerDomain( std::string setdomain );
	void		ServerIP( std::string setip );
	std::string ServerName( void ) const;
	std::string ServerDomain( void ) const;
	std::string ServerIP( void ) const;
	
	void		ServerPort( UI16 setport );
	UI16		ServerPort( void ) const;
	void		ServerConsoleLog( UI08 setting );
	UI08		ServerConsoleLogStatus( void ) const;
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
	UI32		ServerNetRetryCount(void) const { return netRetryCount; }
	void		ServerNetRetryCount(UI32 retryValue) { netRetryCount = retryValue; }
	UI32		ServerNetRcvTimeout(void) const { return netRcvTimeout; }
	void		ServerNetRcvTimeout(UI32 timeoutValue) { netRcvTimeout = timeoutValue; }
	UI32		ServerNetSndTimeout(void) const { return netSndTimeout; }
	void		ServerNetSndTimeout(UI32 timeoutValue) { netSndTimeout = timeoutValue; }

	// Define all Path Get/Set's here please
	void		Directory( CSDDirectoryPaths dp, std::string value );
	std::string Directory( CSDDirectoryPaths dp );

	void		ServerCrashProtection( UI08 setting );
	UI08		ServerCrashProtectionStatus( void ) const;

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

	void		MsgBoardPostingLevel( UI08 value );
	UI08		MsgBoardPostingLevel( void ) const;

	void		MsgBoardPostRemovalLevel( UI08 value );
	UI08		MsgBoardPostRemovalLevel( void ) const;

	void		MineCheck( UI08 value );
	UI08		MineCheck( void ) const;

	void		CombatDisplayHitMessage( bool value );
	bool		CombatDisplayHitMessage( void ) const;

	void		CombatAttackStamina( SI16 value );
	SI16		CombatAttackStamina( void ) const;

	void		CombatNPCDamageRate( SI16 value );
	SI16		CombatNPCDamageRate( void ) const;

	UI08		SkillLevel( void ) const;
	void		SkillLevel( UI08 value );

	void		EscortsEnabled( bool value );
	bool		EscortsEnabled( void ) const;

	void		BasicTooltipsOnly( bool value );
	bool		BasicTooltipsOnly( void ) const;

	void		CombatMonstersVsAnimals( bool value );
	bool		CombatMonstersVsAnimals( void ) const;

	void		CombatAnimalsAttackChance( UI08 value );
	UI08		CombatAnimalsAttackChance( void ) const;

	void		HungerDamage( SI16 value );
	SI16		HungerDamage( void ) const;

	void		PetHungerOffline( bool value );
	bool		PetHungerOffline( void ) const;

	void		PetOfflineTimeout( UI16 value );
	UI16		PetOfflineTimeout( void ) const;

	void		BuyThreshold( SI16 value );
	SI16		BuyThreshold( void ) const;

	void		BackupRatio( SI16 value );
	SI16		BackupRatio( void ) const;

	void		CombatMaxRange( SI16 value );
	SI16		CombatMaxRange( void ) const;

	void		CombatArcherRange( SI16 value );
	SI16		CombatArcherRange( void ) const;

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

	// Sept 22, 2002 - EviLDeD - Support for Xuri's HideWhileMounted fix.
	void		CharHideWhileMounted( bool value );
	bool		CharHideWhileMounted( void ) const;

	void		WeightPerStr( UI08 newVal );
	UI08		WeightPerStr( void ) const;

	void		ServerOverloadPackets( bool newVal );
	bool		ServerOverloadPackets( void ) const;

	void		ArmorAffectManaRegen( bool newVal );
	bool		ArmorAffectManaRegen( void ) const;

	void		AdvancedPathfinding( bool value );
	bool		AdvancedPathfinding( void ) const;

	void		LootingIsCrime( bool value );
	bool		LootingIsCrime( void ) const;

	void		dumpLookup( int lookupid );
	void		dumpPaths( void );

	void			ServerLocation( std::string toSet );
	LPSTARTLOCATION ServerLocation( size_t locNum );

	size_t			NumServerLocations( void ) const;

	UI16			ServerSecondsPerUOMinute( void ) const;
	void			ServerSecondsPerUOMinute( UI16 newVal );

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

	void			incMoon( int mNumber );

	physicalServer *ServerEntry( UI16 entryNum );
	UI16			ServerCount( void ) const;

private: 
	bool			resettingDefaults; 

};

}

#endif

