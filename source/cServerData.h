
#ifndef __CSERVERDATA__
#define __CSERVERDATA__

#define REGENRATE1 8 // Seconds to heal ONE hp
#define REGENRATE2 3 // Seconds to heal ONE stm
#define REGENRATE3 5 // Seconds to heal ONE mn
#define PLAYERCORPSEDECAYMULTIPLIER 3
#define SPIRITSPEAKTIMER 30 // spiritspeak lasts at least 30 seconds, additional time is calculated by Skill & INT
#define BEGGINGRANGE 3                  // begging range
#define FISHINGTIMEBASE 10              // it takes at least FISHINGTIMEBASE sec to fish
#define FISHINGTIMER 5
#define DECAYTIMER 300  // 5 minute decay rate
#define HUNGERRATE 6000 // 15 minutes
#define HUNGERDAMAGERATE 10 //every 10 seconds
#define HUNGERDAMAGE 2 //-2 hp
#define POISONTIMER 180
#define POTIONDELAY 10
#define SPECIALBANKTRIGGER "WARE" // AntiChrist - default word trigger for special bank
#define STATCAP 999
#define SKILLCAP 999999
#define INVISTIMER 60 // invisibility spell lasts 1 minute
#define SKILLDELAY 5   // Skill usage delay     (5 seconds)
#define OBJECTDELAY 1
#define GATETIMER 30
#define ATTACKSTAMINA -2 // AntiChrist - attacker loses stamina when hits
#define MONSTERS_VS_ANIMALS 0 // By default monsters won't attack animals
#define ANIMALS_ATTACK_CHANCE 15 // By default 15% of chance to attack an animal
#define ANIMALS_GUARDED 0 // By default players can attack animals without summoning guards
#define MAX_ABSORBTION 20 // 20 Arm (single armour piece) -- Magius(CHE)
#define MAX_NON_HUMAN_ABSORBTION 100 // 100 Arm (total armour) -- Magius(CHE)
#define NPC_DAMAGE_RATE 2 // DAMAGE / NPC_DAMAGE_RATE for NPCs -- Magius(CHE)
#define NPC_BASE_FLEEAT 20
#define NPC_BASE_REATTACKAT 40
#define SELLBYNAME 1 // Values = 0(Disabled) or 1(Enabled) - The NPC Vendors buy from you if your item has the same name of his item! --- Magius(CHE)
#define SKILLLEVEL 5 // Range from 1 to 10 - This value if the difficult to create an item using a make command: 1 is easy, 5 is normal (default), 10 is difficult! - Magius(CHE)
#define SELLMAXITEM 5 // Set maximum amount of items that one player can sell at a time ( 5 is standard OSI ) --- Magius(CHE)
#define RANKSYSTEM 1 // Rank sstem to make various type of a single item based on the creator's skill! - Magius(CHE)
#define NPCSPEED 1	// 2 steps every second, ab said to change this to 0.5 - fur - NPC's slowed down a bit - Zane
#define TRACKINGRANGE 10        // tracking range is at least TRACKINGRANGE, additional distance is calculated by Skill,INT
#define MAXTRACKINGTARGETS 20   // maximum number of trackable targets
#define TRACKINGTIMER 30        // tracking last at least TRACKINGTIMER seconds, additional time is calculated by Skill,INT, & DEX
#define TRACKINGDISPLAYTIME 30  // tracking will display above the character every TRACKINGDISPLAYTIME seconds


class physicalServer
{
public:
  void setName(const std::string newName);
  void setDomain(const std::string newDomain);
  void setIP(const std::string newIP);
  void setPort(SI16 newPort);
  std::string getName(void);
  std::string getDomain(void);
  std::string getIP(void);
  SI16 getPort(void);

private:
	std::string name;
	std::string domain;
	std::string ip;
	SI16 port;
};

class cServerData 
{
protected:

	UI16		port;
	// ServerSystems
	UI08		saveMode;
	bool		isvalid;
	std::vector< physicalServer > serverList;
	UI32		consolelogenabled;
	UI32		crashprotectionenabled;
	char		commandprefix;					//	March 20, 2000 - EviLDeD
	bool		announcesaves;					//	December 27, 1998 - EviLDeD
	bool		joinpartmsgsenabled;
	bool		mulcachingenabled;
	bool		backupsenabled;
	UI08		accountisolationlevel;	//	EviLDeD: 022602: Added this to support account isolation
	SI16		antilagsavesperloop;
	SI16		backupRatio;
	UI32		serversavestimer;
	UI32		mainthreadsleep;

	// Skills & Stats
	UI32		skillcap;								// Unknown - LB skill cap
	UI32		skilldelay;
	UI32		statcap;								// Unknown - AntiChrist stat cap
	UI32		statdelay;							// December 26, 2000 - EviLDeD moved here from server.scp to unify uox3.ini
	SI16		maxstealthmovement;				// AntiChrist - max number of steps allowed with stealth skill at 100.0
	SI16		maxstaminamovement;				// AntiChrist - max number of steps allowed with stealth skill at 100.0
	UI32		skilladvancemodifier;
	UI32		statadvancemodifier;
	bool		snoopiscrime;

	// ServerTimers
	UI32		corpsedecaytimer;
	UI32		lootdecaytimer;					// December 26, 2000 - EviLDeD - Support item decay for items that are in the open(not in containers)
	UI32		weathertimer;
	UI32		shopspawntimer;
	UI32		decaytimer;							// December 26, 2000 - EviLDeD - Can use this as a generic item decay timer?
	UI32		invisibilitytimer;
	UI32		objectusagetimer;
	UI32		gatetimer;
	UI32		poisontimer;
	UI32		logintimeout;
	UI32		hitpointregentimer;
	UI32		staminaregentimer;
	UI32		manaregentimer;
	SI16		armoraffectregentimer;	// Should armor slow mana regen ?
	UI32		fishingbasetimer;
	UI32		fishingrandomtimer;
	UI32		spiritspeaktimer;
	UI32		beggingtimer;

	// Directories
	std::string		rootdirectory;			// Don't want arrays of strings
	std::string		datadirectory;			// This is the mulfiledirectory
	std::string		defsdirectory;
	std::string		accessdirectory;
	std::string		accountsdirectory;
	std::string		scriptsdirectory;
	std::string		backupdirectory;
	std::string		msgboarddirectory;
	std::string		shareddirectory;		// This is where all files that will be shared with clients andother servers willbe kept
	std::string		htmldirectory;			// Self explanitory.
	std::string		logsdirectory;
	std::string		dictionarydirectory;
	std::string		booksdirectory;

	// Settings
	bool		lootdecayswithcorpse;
	bool		guardsenabled;
	bool		deathanimationenabled;
	SI16		ambientsounds;
	bool		ambientfootsteps;
	bool		externalaccountsenabled;// February 10, 2000 - EviLDeD - Use the www account creation system. Causes autoload on worldsaves
	bool		internalaccountsenabled;// Create accounts when people log in with unknown name/password
	bool		showhiddennpcsenabled;
	bool		roguesenabled;
	bool		engraveenabled;
	bool		uoxbotenabled;
	bool		playerpersecution;				// AntiChrist - persecute thing
	bool		lagfixenabled;						// LB Lag Fix
	bool		accountflushenabled;
	SI16		htmlstatusenabled;				// If >0 then # of seconds between updates
	bool		sellbynameenabled;				// Magius(CHE) 
	SI16		sellmaxitems;							// Magius(CHE) 
	bool		tradesystemenabled;				// Magius(CHE) 
	bool		ranksystemenabled;				// Magius(CHE) 
	SI16		npctilecheck;
	SI16		cutscrollrequirement;			// AntiChrist - cut skill requirements for scrolls
	bool		wildernessbankenabled;		// AntiChrist - special bank
	std::string		wildernessbanktrigger;	// AntiChrist - special bank trigger EviLDeD Dec 30, 2000 changed this to a string
	bool		heartbeat;
	bool		npctraining;
	SI16		charhidewhilemounted;			// EviLDeD - Support for Xuri's Hide while Mounted fix

	// SpeedUp
	R64			checkitems;
	R64			checknpcs;
	R64			checkboats;
	R64			checknpcai;
	R64			checkspawnregions;
	R64			checkmemory;
	R64			npcSpeed;
	R64			flushTime;

	// MessageBoards
	UI32		keepmessages;
	SI16		msgadminlevel;
	SI16		msgpostinglevel;
	SI16		msgremovallevel;
	SI16		maxmessagesallowed;
	SI16		maxmessagesreaction;

	// Escorts
	bool		escortsenabled;
	UI32		escortinitexpire;
	UI32		escortactiveexpire;
	UI32		escortdoneexpire;

	// WorldLight
	SI16		moon[2];
	SI16		moonupdate[2];
	SI16		dungeonlightlevel;
	SI16		fixedlightlevel;
	SI16		currentlightlevel;
	SI16		brightnesslightlevel;

	// WorldTimer		: This is now loaded in from the uoxclock.def file
	SI16		days;
	UI08		hours;
	UI08		minutes;
	UI08		seconds;
	SI16		uosecondsperminute;
	bool		ampm;

	// Begging
	SI16		beggingrange;

	// Tracking
	UI32		trackingbaserange;
	UI32		trackingbasetimer;
	UI32		trackingmaxtargets;
	UI32		trackingmsgredisplaytimer;


	// Reputation
	UI32		murderdecaytimer;
	UI32		criminaldecaytimer;
	UI32		maxmurdersallowed;

	// Resources
	SI16		minecheck;
	UI32		oreperarea;
	UI32		orerespawntimer;
	UI32		orerespawnarea;
	UI32		logcheck;
	UI32		logsperarea;
	UI32		logsrespawntimer;
	UI32		logsrespawnarea;

	// Hunger & Food
	UI32		hungerrate;
	UI32		hungerdamageratetimer;
	SI16		hungerdamage;
	UI32		hungerthreshold;						// Not sure what this does at this point Dec 27, 2000 - EviLDeD

	// Combat
	SI16		combatmaxrange;
	SI16		combatwrestlingspeed;
	SI16		combatmaxspellrange;
	SI16		combatmaxmeleedamage;
	SI16		combatmaxspelldamage;				// EviLDeD Dec 30, 2000
	bool		combatcriticalsenabled;
	SI16		combatmaxpoisoningdistance;
	bool		combatdisplayhitmessage;
	SI16		combatmaxhumanabsorbtion;		// Magius(CHE) --- Maximum Armour of the single pieces of armour (for player)!
	SI16		combatmaxnonhumanabsorbtion;// Magius(CHE) --- Maximum Armour of the total armour (for NPC)!
	bool		combatmonstersvsanimals;
	SI16		combatanimalattackchance;
	bool		combatanimalsguarded;
	SI16		combatnpcdamagerate;				// Magius(CHE) --- DAmage divided by this number if the attacked is an NPC!
	SI16		combatnpcbasefleeat;
	SI16		combatnpcbasereattackat;
	SI16		combatattackstamina;				// AntiChrist (6) - for ATTACKSTAMINA
	R32			combatExplodeDelay;
	bool		shootonanimalback;		// sereg - lets make that optional

	// Start & Location Settings
	std::vector< STARTLOCATION >	startlocations;
	SI16		startgold;
	SI16		startprivs[2];
	SI16		skillmodifiers[64];
	SI16		statmodifiers[6];

	// Anything under this comment is left here for symantics
	UI32 skilllevel; // Magius(CHE) 
	SI16 buyThreshold;
	SI16 niceness;		//	Abaddon wtf is this ?  And define Skilllevel for me and this buyThreshold.

	// Gump stuff
	UI16	titleColour;
	UI16	leftTextColour;
	UI16	rightTextColour;
	UI16	buttonCancel;
	UI16	buttonLeft;
	UI16	buttonRight;
	UI16	backgroundPic;

	// Townstone stuff
	long numSecsPollOpen;
	long numSecsAsMayor;
	long taxPeriod;			// # of seconds
	long guardPayment;	// # of seconds between guard payments

	void	PostLoadDefaults( void );

	SI16	potiondelay;
	bool	srvScpSectHeader;

public:
	UI08	SaveMode( void ) const;
	UI08	SaveMode( UI08 );
	long	VersionCheck( const char *filename );
	SI16	darknesslightlevel;
	SI16	GetWorldLightDarkLevel( void );
	SI16	GetWorldLightBrightLevel( void );
	SI16	GetWorldLightCurrentLevel( void );
	SI16	GetWorldFixedLightLevel( void );
	SI16	GetDungeonLightLevel( void );
	SI16	GetServerMoonUpdate( SI16 slot );
	SI16	GetServerMoon( SI16 slot );
	UI32	GetServerManaRegenTimer( void );
	UI32	GetServerStaminaRegenTimer( void );
	UI32	GetServerHitpointRegenTimer( void );
	UI32	GetMsgBoardKeepMsgsStatus( void );
	UI32	GetWeatherTimer( void );
	UI32	GetPoisonTimer( void );
	UI32	GetServerDecayTimer( void );
	UI08	GetServerStartPrivs( SI16 slot ) const;
	SI16	GetServerStartGold( void ) const;

	void	SetWorldLightDarkLevel( SI16 value );
	void	SetWorldLightBrightLevel( SI16 value );
	void	SetWorldLightCurrentLevel( SI16 value );
	void	SetWorldFixedLightLevel( SI16 value );
	void	SetDungeonLightLevel( SI16 value );
	void	SetServerMoonUpdate( SI16 slot, SI16 value );
	void	SetServerMoon( SI16 slot, SI16 value );
	void	SetServerManaRegenTimer( UI32 value );
	void	SetServerStaminaRegenTimer( UI32 value );
	void	SetServerHitpointRegenTimer( UI32 value);
	void	SetMsgBoardKeepMsgs( UI32 value);
	void	SetWeatherTimer( UI32 value );
	void	SetPoisonTimer( UI32 value );
	void	SetServerDecayTimer( UI32 value );
	void	SetServerStartPrivs( SI16 slot, UI08 value );
	void	SetServerStartGold( SI16 value );
	cServerData * ParseUox3ServerScp( const char *filename );
	cServerData * ParseUox3Ini( const char * filename, long ver );

	virtual cServerData *load( void );
	virtual cServerData *load( const char *filename );
	virtual bool save( void );
	virtual bool save( const char *filename );

	void		ResetDefaults( void );

				cServerData( void );
	virtual		~cServerData();
	void		SetServerName( const char *setname );
	void		SetServerDomain( const char *setdomain );
	void		SetServerIP( const char *setip );
	const char *GetServerName( void );
	const char *GetServerDomain( void );
	const char *GetServerIP( void );
	void		SetServerPort( SI16 setport );
	SI16		GetServerPort( void );
	void		SetServerConsoleLog( UI32 setting );
	UI32		GetServerConsoleLogStatus( void );
	virtual void SetServerCommandPrefix( char cmdvalue );
	char		GetServerCommandPrefix( void );
	void		SetServerAnnounceSaves( bool setting );
	bool		GetServerAnnounceSavesStatus( void );
	void		SetServerJoinPartAnnouncements( bool setting );
	bool		GetServerJoinPartAnnouncementsStatus( void );
	void		SetServerMulCaching( bool setting );
	bool		GetServerMulCachingStatus( void );
	void		SetServerBackups( bool setting );
	bool		GetServerBackupStatus( void );
	void		SetServerAntiLagSavesPerLoop(SI16 value);		// Unused
	SI16		GetServerAntiLagSavesPerLoop( void );			// Unused
	void		SetServerSavesTimer( UI32 timer );
	UI32		GetServerSavesTimerStatus( void );
	void		SetServerMainThreadTimer( UI32 threadtimer );
	UI32		GetServerMainThreadTimerStatus( void );
	void		SetServerSkillCap( UI32 cap );
	UI32		GetServerSkillCapStatus( void );
	void		SetServerSkillDelay( UI32 skdelay );
	UI32		GetServerSkillDelayStatus( void );
	void		SetServerStatDelay( UI32 stdelay );
	UI32		GetServerStatCapStatus( void );
	void		SetServerStatCap( UI32 cap );
	UI32		GetServerStatDelayStatus( void );
	void		SetMaxStealthMovement( SI16 value );
	SI16		GetMaxStealthMovement( void );
	void		SetMaxStaminaMovement( SI16 value );
	SI16		GetMaxStaminaMovement( void );
	void		SetSkillAdvanceModifier( UI32 value );
	UI32		GetSkillAdvanceModifier( void );
	void		SetStatAdvanceModifier( UI32 value );
	UI32		GetStatAdvanceModifier( void );
	void		SetSystemTimer( TID timerid, UI32 value );
	SI32		GetSystemTimerStatus( TID timerid );

	// Define all Path Get/Set's here please
	void		SetRootDirectory( char *text );
	const char *GetRootDirectory( void );
	void		SetDefsDirectory( char *text );
	const char *GetDefsDirectory( void );
	void		SetBooksDirectory( char *text );
	const char *GetBooksDirectory( void );
	void		SetAccessDirectory( char *text );
	const char *GetAccessDirectory( void );
	void		SetAccountsDirectory( char *text );
	const char *GetAccountsDirectory( void );
	void		SetScriptsDirectory( char *text );
	const char *GetScriptsDirectory( void );
	void		SetBackupDirectory( char *text );
	const char *GetBackupDirectory( void );
	void		SetMsgBoardDirectory( char *text );
	const char *GetMsgBoardDirectory( void );
	void		SetSharedDirectory( char *text );
	const char *GetSharedDirectory( void );
	void		SetHTMLDirectory( char *text );
	const char *GetHTMLDirectory( void );
	void		SetDataDirectory( char *text );
	const char *GetDataDirectory( void );
	void		SetLogsDirectory( char *text );
	const char *GetLogsDirectory( void );
	void		SetDictionaryDirectory( char *text );
	const char *GetDictionaryDirectory( void );

	void		SetServerCrashProtection( UI32 setting );
	UI32		GetServerCrashProtectionStatus( void );

	void		SetCorpseLootDecay( bool value );
	bool		GetCorpseLootDecay( void );

	void		SetGuardStatus( bool value );
	bool		GetGuardsStatus( void );

	void		SetDeathAnimationStatus( bool value );
	bool		GetDeathAnimationStatus( void );

	void		SetWorldAmbientSounds( SI16 value );
	SI16		GetWorldAmbientSounds( void );

	void		SetAmbientFootsteps( bool value );
	bool		GetAmbientFootsteps( void );

	void		SetExternalAccountStatus( bool value );
	bool		GetExternalAccountStatus( void );

	void		SetInternalAccountStatus( bool value );
	bool		GetInternalAccountStatus( void );

	void		SetShowHiddenNpcStatus( bool value );
	bool		GetShowHiddenNpcStatus( void );

	void		SetRogueStatus( bool value );
	bool		GetRogueStatus( void );

	void		SetSnoopIsCrime( bool value );
	bool		GetSnoopIsCrime( void );

	void		SetEngraveStatus( bool value );
	bool		GetEngraveStatus( void );

	void		SetUoxBotStatus( bool value );		// Unused
	bool		GetUoxBotStatus( void );			// Unused

	void		SetPlayerPersecutionStatus( bool value );
	bool		GetPlayerPersecutionStatus( void );

	void		SetLagFixStatus( bool value );		// Unused
	bool		GetLagFixStatus( void );			// Unused

	void		SetHtmlStatsStatus( SI16 value );
	SI16		GetHtmlStatsStatus( void );

	void		SetSellByNameStatus( bool value );
	bool		GetSellByNameStatus( void );

	void		SetSellMaxItemsStatus( SI16 value );
	SI16		GetSellMaxItemsStatus( void );

	void		SetTradeSystemStatus( bool value );
	bool		GetTradeSystemStatus( void );

	void		SetRankSystemStatus( bool value );
	bool		GetRankSystemStatus( void );

	void		SetCutScrollRequirementStatus( SI16 value );
	SI16		GetCutScrollRequirementStatus( void );

	SI16		GetNpcTileCheckStatus( void );		// Unused
	void		SetNpcTileCheckStatus( SI16 value );// Unused

	void		SetNPCTrainingStatus( bool setting );
	bool		GetNPCTrainingStatus( void );

	void		SetWildernessBankStatus( bool value );
	bool		GetWildernessBankStatus( void );

	void		SetWildernessBankTriggerString( const char *value );
	const char *GetWildernessBankTriggerString( void );

	void		SetSystemHeartBeatStatus( bool value );
	bool		GetSystemHeartBeatStatus( void );

	void		SetCheckItemsSpeed( R64 value );
	R64			GetCheckItemsSpeed( void );

	void		SetCheckNpcSpeed( R64 value );
	R64			GetCheckNpcSpeed( void );

	void		SetCheckBoatSpeed( R64 value );
	R64			GetCheckBoatSpeed( void );

	void		SetCheckNpcAISpeed( R64 value );
	R64			GetCheckNpcAISpeed( void );

	void		SetCheckSpawnRegionSpeed( R64 value );
	R64			GetCheckSpawnRegionSpeed( void );

	void		SetCheckMemorySpeed( R64 value );
	R64			GetCheckMemorySpeed( void );

	void		SetKeepMessagesStatus( bool value );
	bool		GetKeepMessagesStatus( void );

	void		SetMsgBoardAdminLevel( SI16 value );
	SI16		GetMsgBoardAdminLevel( void );

	void		SetMsgBoardPostingLevel( SI16 value );
	SI16		GetMsgBoardPostingLevel( void );

	void		SetMsgBoardPostRemovalLevel( SI16 value );
	SI16		GetMsgBoardPostRemovalLevel( void );

	void		SetMsgBoardMaxMessageCount( SI16 value );
	SI16		GetMsgBoardMaxMessageCount( void );

	void		SetMsgBoardMaxMessageseaction( SI16 value );
	SI16		GetMsgBoardMaxMessagesReaction( void );

	void		SetMineCheck( SI16 value );
	SI16		GetMineCheck( void );

	void		SetCombatDisplayHitMessage( bool value );
	bool		GetCombatDisplayHitMessage( void );

	void		SetCombatMaxHumanAbsorbtion( SI16 value );
	SI16		GetCombatMaxHumanAbsorbtion( void );

	void		SetCombatMaxNonHumanAbsorbtion( SI16 value );
	SI16		GetCombatMaxNonHumanAbsorbtion( void );

	void		SetCombatAttackStamina( SI16 value );
	SI16		GetCombatAttackStamina( void );

	void		SetCombatNPCDamageRate( SI16 value );
	SI16		GetCombatNPCDamageRate( void );

	UI32		GetSkillLevel( void );
	void		SetSkillLevel( UI32 value );

	void		SetEscortsEnabled( bool value );
	bool		GetEscortsEnabled( void );

	void		SetEscortInitExpire( UI32 value );
	UI32		GetEscortInitExpire( void );

	void		SetEscortActiveExpire( UI32 value );
	UI32		GetEscortActiveExpire( void );

	void		SetEscortDoneExpire( UI32 value );
	UI32		GetEscortDoneExpire( void );

	void		SetCombatMonstersVsAnimals( bool value );
	bool		GetCombatMonstersVsAnimals( void );

	void		SetCombatAnimalsAttackChance( SI16 value );
	SI16		GetCombatAnimalsAttackChance( void );

	void		SetServerArmorAffectManaRegen( SI16 value );
	SI16		GetServerArmorAffectManaRegen( void );

	void		SetHungerRate( UI32 value );
	UI32		GetHungerRate( void );

	void		SetHungerDamageRateTimer( UI32 value );
	UI32		GetHungerDamageRateTimer( void );

	void		SetHungerDamage( SI16 value );
	SI16		GetHungerDamage( void );

	void		SetHungerThreshold( UI32 value );
	UI32		GetHungerThreshold( void );

	void		SetPotionDelay( SI16 value );
	SI16		GetPotionDelay( void );

	void		SetBuyThreshold( SI16 value );
	SI16		GetBuyThreshold( void );

	void		SetBackupRatio( SI16 value );
	SI16		GetBackupRatio( void );

	void		SetCombatWrestlingSpeed( SI16 value );
	SI16		GetCombatWrestlingSpeed( void );

	void		SetCombatMaxMeleeDamage( SI16 value );
	SI16		GetCombatMaxMeleeDamage( void );

	void		SetCombatMaxPoisoningDistance( SI16 value );
	SI16		GetCombatMaxPoisoningDistance( void );

	void		SetCombatMaxRange( SI16 value );
	SI16		GetCombatMaxRange( void );

	void		SetCombatMaxSpellRange( SI16 value );
	SI16		GetCombatMaxSpellRange( void );

	void		SetCombatMaxSpellDamage( SI16 value );
	SI16		GetCombatMaxSpellDamage( void ) const;

	void		SetCombatCriticalsEnabled( bool value );	// Unused
	bool		GetCombatCriticalsEnabled( void );			// Unused

	void		SetCombatExplodeDelay( R32 value );
	R32			GetCombatExplodeDelay( void );

	void		SetCombatAnimalsGuarded( bool value );
	bool		GetCombatAnimalsGuarded( void );

	void		SetCombatNPCBaseFleeAt( SI16 value );
	SI16		GetCombatNPCBaseFleeAt( void );

	void		SetCombatNPCBaseReattackAt( SI16 value );
	SI16		GetCombatNPCBaseReattackAt( void );

	void		SetShootOnAnimalBack( bool setting );
	bool		GetShootOnAnimalBack( void );

	void		SetNPCSpeed( R64 value );
	R64			GetNPCSpeed( void );

	void		SetNiceness( SI16 value );
	SI16		GetNiceness( void );

	void		SetTitleColour( UI16 value );
	UI16		GetTitleColour( void );

	void		SetLeftTextColour( UI16 value );
	UI16		GetLeftTextColour( void );

	void		SetRightTextColour( UI16 value );
	UI16		GetRightTextColour( void );

	void		SetButtonCancel( UI16 value );
	UI16		GetButtonCancel( void );

	void		SetButtonLeft( UI16 value );
	UI16		GetButtonLeft( void );

	void		SetButtonRight( UI16 value );
	UI16		GetButtonRight( void );

	void		SetBackgroundPic( UI16 value );
	UI16		GetBackgroundPic( void );

	void		SetTownNumSecsPollOpen( long value );
	long		GetTownNumSecsPollOpen( void );

	void		SetTownNumSecsAsMayor( long value );
	long		GetTownNumSecsAsMayor( void );

	void		SetTownTaxPeriod( long value );
	long		GetTownTaxPeriod( void );

	void		SetTownGuardPayment( long value );
	long		GetTownGuardPayment( void );

	void		SetRepMurderDecay( long value );
	long		GetRepMurderDecay( void );

	void		SetRepMaxKills( int value );
	int			GetRepMaxKills( void );

	void		SetRepCrimTime( int value );
	int			GetRepCrimTime( void );

	void		SetResLogs( UI32 value );
	UI32		GetResLogs( void );

	void		SetResLogTime( UI32 value );
	UI32		GetResLogTime( void );

	void		SetResLogArea( UI32 value );
	UI32		GetResLogArea( void );

	void		SetResOre( UI32 value );
	UI32		GetResOre( void );

	void		SetResOreTime( UI32 value );
	UI32		GetResOreTime( void );

	void		SetResOreArea( UI32 value );
	UI32		GetResOreArea( void );

	void		SetAccountFlushTimer( R64 value );
	R64			GetAccountFlushTimer( void );

	void		SetTrackingBaseRange( UI32 value );
	UI32		GetTrackingBaseRange( void );

	void		SetTrackingMaxTargets( UI32 value );
	UI32		GetTrackingMaxTargets( void );

	void		SetTrackingBaseTimer( UI32 value );
	UI32		GetTrackingBaseTimer( void );

	void		SetTrackingRedisplayTime( UI32 value );
	UI32		GetTrackingRedisplayTime( void );

	void		SetBeggingRange( SI16 value );
	SI16		GetBeggingRange( void );

	void		SetFishingBaseTime( UI32 value );
	UI32		GetFishingBaseTime( void );

	void		SetFishingRandomTime( UI32 value );
	UI32		GetFishingRandomTime( void );

	void		SetSpiritSpeakTimer( UI32 value );
	UI32		GetSpiritSpeakTimer( void );

	// Sept 22, 2002 - EviLDeD - Support for Xuri's HideWhileMounted fix.
	void		SetCharHideWhileMounted( SI16 value );
	SI16		GetCharHideWhileMounted( void );

//protected:
	void DumpLookup(int lookupid);
	//	EviLDeD: 022602: Added this so I could check the paths, make sure they are correct
	void DumpPaths( void );
	// 

	void		SetServerLocation( const char *toSet );
	LPSTARTLOCATION GetServerLocation( UI32 locNum );

	UI32			GetNumServerLocations( void ) const;

	SI16			GetServerTimeDay( void ) const;
	UI08			GetServerTimeHours( void ) const;
	UI08			GetServerTimeMinutes( void ) const;
	UI08			GetServerTimeSeconds( void ) const;
	bool			GetServerTimeAMPM( void ) const;

	void			SetServerTimeDay( SI16 nValue );
	void			SetServerTimeHours( UI08 nValue );
	void			SetServerTimeMinutes( UI08 nValue );
	void			SetServerTimeSeconds( UI08 nValue );
	void			SetServerTimeAMPM( bool nValue );

	// These functions return TRUE if it's a new day
	bool			IncSecond( void );
	bool			IncMinute( void );
	bool			IncHour( void );
	bool			IncDay( void );

	void			IncMoon( int mNumber );

	physicalServer *GetServerEntry( UI16 entryNum );
	UI32			GetServerCount( void ) const;

	bool			ServerScriptSectionHeader( void ) const;
	void			ServerScriptSectionHeader( bool value );

private: 
  void setDirectoryHelper( std::string dirName, std::string &dir, char *text ); 
  bool resettingDefaults; 

};

#endif

