#ifndef __CCHAR_H__
#define __CCHAR_H__
#include "cBaseObject.h"
#include "cAccountClass.h"
enum cNPC_FLAG
{
	fNPC_NEUTRAL = 0,
	fNPC_INNOCENT,
	fNPC_EVIL
};

enum cC_TID
{
	// Global Character Timers
	tCHAR_TIMEOUT = 0,
	tCHAR_INVIS,
	tCHAR_HUNGER,
	tCHAR_THIRST,
	tCHAR_POISONTIME,
	tCHAR_POISONTEXT,
	tCHAR_POISONWEAROFF,
	tCHAR_SPELLTIME,
	tCHAR_SPELLRECOVERYTIME,
	tCHAR_ANTISPAM,
	tCHAR_CRIMFLAG,
	tCHAR_STEALFLAG,
	tCHAR_MURDERRATE,
	tCHAR_PEACETIMER,
	tCHAR_FLYINGTOGGLE,
	tCHAR_FIREFIELDTICK,
	tCHAR_POISONFIELDTICK,
	tCHAR_PARAFIELDTICK,
	tCHAR_YOUNGHEAL,
	tCHAR_YOUNGMESSAGE,
	// NPC Timers
	tNPC_MOVETIME,
	tNPC_SPATIMER,
	tNPC_SUMMONTIME,
	tNPC_EVADETIME,
	tNPC_LOYALTYTIME,
	tNPC_IDLEANIMTIME,
	tNPC_PATHFINDDELAY,
	tNPC_FLEECOOLDOWN,
	// PC Timers
	tPC_LOGOUT,
	tCHAR_COUNT
};

struct TargetInfo
{
	UI32 timestamp;
	bool isNpc;
};

struct DamageTrackEntry_st
{
	DamageTrackEntry_st() : damager( INVALIDSERIAL ), damageDone( 0 ), lastDamageType( PHYSICAL ), lastDamageDone( INVALIDSERIAL ) { }
	DamageTrackEntry_st( SERIAL dmgr, SI32 dmgDn, WeatherType dmgType, TIMERVAL lstDmgDn ) : damager( dmgr ), damageDone( dmgDn ), lastDamageType( dmgType ), lastDamageDone( lstDmgDn ) { }
	SERIAL		damager;			// who did the damage?
	SI32		damageDone;			// how much damage has been accumulated?
	WeatherType lastDamageType;			// what type of damage was dealt most recently?
	TIMERVAL	lastDamageDone;		// when was the last time that damage was done?
};

bool DTEgreater( DamageTrackEntry_st &elem1, DamageTrackEntry_st &elem2 );

class CChar : public CBaseObject
{
private:
	typedef std::map<ItemLayers, CItem *>				LAYERLIST;
	typedef std::map<ItemLayers, CItem *>::iterator		LAYERLIST_ITERATOR;
	typedef GenericList<DamageTrackEntry_st *>			DAMAGETRACK;

	struct NPCValues_st
	{
		NPCValues_st();
		void				DumpBody( std::ostream& outStream );

		SI08				wanderMode; // NPC Wander Mode
		SI08				oldWanderMode; // Used for fleeing npcs
		SERIAL				fTarg; // NPC Follow Target
		SI16				fx[2]; //NPC Wander Point x
		SI16				fy[2]; //NPC Wander Point y
		SI08				fz;    //NPC Wander Point z
		SI16				aiType;
		SI16				spellAttack;
		SI08				spellDelay; // won't time out for more than 255 seconds!
		SI16				taming;
		SI16				fleeAt;     // HP Level to flee at
		SI16				reAttackAt; // HP Level to re-Attack at
		UI08				fleeDistance;	// Maximum distance in tiles the NPC can flee in one go
		UI08				splitNum;
		UI08				splitChance;
		UI08				trainingPlayerIn;
		UI32				goldOnHand;
		UI08				questType;
		UI08				questDestRegion;
		UI08				questOrigRegion;
		CBaseObject *		petGuarding;
		cNPC_FLAG			npcFlag;
		std::bitset<8>		boolFlags;
		SI16				peaceing;
		SI16				provoing;

		UI16				tamedHungerRate;  // The rate at which hunger decreases when char is tamed
		UI16				tamedThirstRate;  // The rate at which thirst decreases when char is tamed
		UI08				hungerWildChance; // The chance that the char goes wild when hungry
		UI08				thirstWildChance; // The chance that the char goes wild when thirsty
		R32					walkingSpeed;
		R32					runningSpeed;
		R32					fleeingSpeed;
		SI08				pathFail;
		SI08				pathResult;
		UI16				pathTargX;
		UI16				pathTargY;

		std::deque<UI08>	pathToFollow;	// let's use a queue of directions to follow

		UI08				controlSlots;		// Amount of control slots taken up by a particular NPC
		std::vector<CChar *>	petFriends;		// Temporary list of friends a pet has
		GenericList<CChar *>	petOwnerList;	// Persistent list of owners a pet has previously had
		std::unordered_map<SERIAL, TargetInfo>	combatIgnore; // Chars this char ignores as targets in combat, with timestamps
		UI16				maxLoyalty;			// Pet's maximum loyalty to its master
		UI16				loyalty;			// Pet's current loyalty to its master
		UI16				orneriness;			// Difficulty to control pet

		std::string			foodList;

		R32					mountedWalkingSpeed;
		R32					mountedRunningSpeed;
		R32					mountedFleeingSpeed;
	};

	struct PlayerValues_st
	{
		PlayerValues_st();
		void		DumpBody( std::ostream& outStream );

		SERIAL		callNum;        // Callnum GM or Counsellor is on
		SERIAL		playerCallNum;  // Players call number in GM or Counsellor requestQueue
		SERIAL		trackingTarget; // Tracking target ID
		UI08		squelched;      // Squelching
		UI08		commandLevel;   // 0 = player, 1 = counselor, 2 = GM
		UI08		postType;
		UI16		hairStyle;
		UI16		beardStyle;
		COLOUR		hairColour;
		COLOUR		beardColour;
		CItem *		speechItem;
		UI08		speechMode;
		UI08		speechId;
		cScript *	speechCallback;
		SERIAL		robe;
		UI16		accountNum;
		UI16		origSkin;
		UI16		origId;     // Backup of body type for polymorph
		UI08		fixedLight; // Fixed lighting level (For chars in dungeons, where they dont see the night)
		UI16		deaths;
		CSocket *	socket;

		std::vector<CChar*>	trackingTargets;

		std::string	origName; // original name - for Incognito

		std::string	lastOn; //Last time a character was on
		UI32		lastOnSecs; //Last time a character was on in seconds.

		SERIAL		townVote;
		SI08		townPriv;  //0=non resident (Other privledges added as more functionality added)
		UI08		controlSlotsUsed; // The total number of control slots currently taken up by followers/pets
		UI32		createdOn;	// Timestamp for when player character was created
		UI32		npcGuildJoined;	// Timestamp for when player character joined NPC guild (0=never joined)
		UI32		playTime;	// Character's full playtime

		UI08		atrophy[INTELLECT+1];
		SkillLock	lockState[INTELLECT+1];	// state of the skill locks

		// speechMode valid values
		// 0 normal speech
		// 1 GM page
		// 2 Counselor page
		// 3 Player Vendor item pricing
		// 4 Player Vendor item describing
		// 5 Key renaming
		// 6 Name deed
		// 7 Rune renaming
		// 8 Sign renaming
		// 9 JavaScript speech
	};
	// Base Characters
protected:
	NPCValues_st	*	mNPC;
	PlayerValues_st	*	mPlayer;

	std::bitset<64>	bools;	// lots of flags
	SI08		fontType;	// Speech font to use
	UI16		maxHP;
	UI16		maxHP_oldstr;
	RACEID		oldRace;
	SI16		maxMana;
	UI16		maxMana_oldint;
	SI16		maxStam;
	UI16		maxStam_olddex;
	COLOUR		sayColor;
	COLOUR		emoteColor;
	SI08		cell;		// Reserved for jailing players
	CItem *		packItem;	// Characters backpack
	SERIAL		targ;		// Current combat target
	SERIAL		attacker;	// Character who attacked this character
	SI08		hunger;		// Level of hungerness, 6 = full, 0 = "empty"
	SI08		thirst;		// Level of thirstiness, 6 = full, 0 = "empty"
	UI16		regionNum;
	UI16		town;		// Matches Region number in regions.dfn

	UI08		brkPeaceChanceGain;
	UI08		brkPeaceChance;
  	UI16    	advObj;			// Has used advance gate?
  	SERIAL  	guildFealty;	// Serial of player you are loyal to (default=yourself) (DasRaetsel)
  	SI16    	guildNumber;	// Number of guild player is in (0=no guild)     (DasRaetsel)

  	UI08    	flag;			// 1=red 2=grey 4=Blue 8=green 10=Orange // should it not be 0x10??? sounds like we're trying to do
  	SI08    	spellCast;
  	UI08    	nextAct;		// time to next spell action..
  	SI08    	stealth;		// stealth ( steps already done, -1=not using )
  	UI08    	running;		// Stamina Loose while running
  	RACEID  	raceGate;		// Race gate that has been used
  	UI08    	step;			// 1 if step 1 0 if step 2 3 if step 1 skip 2 if step 2 skip

	std::bitset<16>		priv;

	std::string	guildTitle;		// Title Guildmaster granted player						(DasRaetsel)

	TIMERVAL	charTimers[tCHAR_COUNT];

	TIMERVAL	regen[3];
	TIMERVAL	weathDamage[WEATHNUM];	// Light Damage timer

	UI08		PoisonStrength;
	BodyType	bodyType;
	UI32		lastMoveTime;		// Timestamp for when character moved last
	UI16		npcGuild;		// ID of NPC guild character is in (0=no NPC guild)

	SKILLVAL	baseskill[ALLSKILLS]; 	// Base skills without stat modifiers
	SKILLVAL	skill[INTELLECT+1]; 	// List of skills (with stat modifiers)

	LAYERLIST				itemLayers;
	LAYERLIST_ITERATOR		layerCtr;
	std::unordered_map<SERIAL, TargetInfo>	aggressorFlags; // Chars this char is marked as aggressor to, with timestamps
	std::unordered_map<SERIAL, TargetInfo>	permaGreyFlags; // Chars this char is marked as permanent grey to, with timestamps
	GenericList<CChar *>	petsOwned;
	GenericList<CChar *>	activeFollowers;
	GenericList<CItem *>	ownedCorpses;
	std::vector<CItem *>	ownedItems;
	std::bitset<32>			skillUsed[2];	// no more than 64 skills
	std::bitset<UT_COUNT>	updateTypes;

	DAMAGETRACK		damageDealt;
	DAMAGETRACK		damageHealed;

	virtual bool	DumpHeader( std::ostream &outStream ) const override;
	virtual bool	DumpBody( std::ostream &outStream ) const override;
	virtual bool	HandleLine( std::string &UTag, std::string &data ) override;
	virtual bool	LoadRemnants( void ) override;

	void		CopyData( CChar *target );

	void		CreateNPC( void );
	void		CreatePlayer( void );

	bool		IsValidNPC( void ) const;
	bool		IsValidPlayer( void ) const;


public:

	BodyType	GetBodyType( void );

	virtual void	SetWeight( SI32 newVal, bool doWeightUpdate = true ) override;

	bool		GetUpdate( UpdateTypes updateType ) const;
	void		ClearUpdate( void );
	virtual void	Dirty( UpdateTypes updateType ) override;
	void		UpdateRegion( void );

	void		UpdateDamageTrack( void );
	auto		CheckDamageTrack( SERIAL serialToCheck, TIMERVAL lastXSeconds ) -> bool;

	void		SetPoisonStrength( UI08 value );
	UI08		GetPoisonStrength( void ) const;

	GenericList<CChar *> *	GetPetList( void );
	GenericList<CChar *> *	GetFollowerList( void );
	GenericList<CChar *> *	GetPetOwnerList( void );

	auto		GetOwnedCorpses() -> GenericList<CItem *>*;
	auto		GetOwnedItems() -> std::vector<CItem *>*;

	auto		AddCorpse( CItem *corpseToAdd ) -> bool;
	auto		RemoveCorpse( CItem *corpseToRemove ) -> bool;

	auto		AddFollower( CChar *npcToAdd ) -> bool;
	auto		RemoveFollower( CChar *followerToRemove ) -> bool;

	auto		GetAggressorFlags() const -> const std::unordered_map<SERIAL, TargetInfo>;
	auto		AddAggressorFlag( SERIAL toAdd ) -> void;
	auto		RemoveAggressorFlag( SERIAL toRemove ) -> void;
	auto		CheckAggressorFlag( SERIAL toCheck ) -> bool;
	auto		UpdateAggressorFlagTimestamp( SERIAL toUpdate ) -> void;
	auto		ClearAggressorFlags() -> void;
	auto		IsAggressor( bool checkForPlayersOnly ) -> bool;
	auto		AggressorFlagMaintenance() -> void;

	auto		GetPermaGreyFlags() const -> const std::unordered_map<SERIAL, TargetInfo>;
	auto		AddPermaGreyFlag( SERIAL toAdd ) -> void;
	auto		RemovePermaGreyFlag( SERIAL toRemove ) -> void;
	auto		CheckPermaGreyFlag( SERIAL toCheck ) -> bool;
	auto		UpdatePermaGreyFlagTimestamp( SERIAL toUpdate ) -> void;
	auto		ClearPermaGreyFlags() -> void;
	auto		IsPermaGrey( bool checkForPlayersOnly ) -> bool;
	auto		PermaGreyFlagMaintenance() -> void;

	void		AddOwnedItem( CItem *toAdd );
	void		RemoveOwnedItem( CItem *toRemove );

	void		DoLoyaltyUpdate( void );
	void		DoHunger( CSocket *mSock );
	void		DoThirst( CSocket* mSock );
	void		CheckPetOfflineTimeout( void );
	SI08		GetHunger( void ) const;
	SI08        GetThirst( void ) const;
	UI16		GetTamedHungerRate( void ) const;
	UI16		GetTamedThirstRate( void ) const;
	UI08		GetTamedHungerWildChance( void ) const;
	UI08		GetTamedThirstWildChance( void ) const;
	UI16		GetTown( void ) const;
	std::string GetFood( void ) const;

	bool		SetHunger( SI08 newValue );
	bool		SetThirst( SI08 newValue );
	void		SetTamedHungerRate( UI16 newValue );
	void		SetTamedThirstRate( UI16 newValue );
	void		SetTamedHungerWildChance( UI08 newValue );
	void		SetTamedThirstWildChance( UI08 newValue );
	void		SetTown( UI16 newValue );
	void		SetFood( std::string food );

	UI08		GetBrkPeaceChanceGain( void ) const;
	void		SetBrkPeaceChanceGain( UI08 newValue );

	UI08		GetBrkPeaceChance( void ) const;
	void		SetBrkPeaceChance( UI08 newValue );

	void		SetMounted( bool newValue );
	bool		GetMounted( void ) const;

	void		SetStabled( bool newValue );
	bool		GetStabled( void ) const;
	void		SetFlying( bool newValue );
	bool		IsFlying( void ) const;

	bool		ToggleFlying( void );

	void		SetMaxHPFixed( bool newValue );
	bool		GetMaxHPFixed( void ) const;
	void		SetMaxManaFixed( bool newValue );
	bool		GetMaxManaFixed( void ) const;
	void		SetMaxStamFixed( bool newValue );
	bool		GetMaxStamFixed( void ) const;

	bool		DecHunger( const SI08 amt = 1 );
	bool		DecThirst( const SI08 amt = 1 );

	bool		IsUnicode( void ) const;
	bool		IsNpc( void ) const;
	bool		IsAwake( void ) const;
	bool		IsEvading( void ) const;
	bool		IsShop( void ) const;
	bool		IsDead( void ) const;
	bool		GetCanAttack( void ) const;
	bool		IsAtWar( void ) const;
	bool		IsPassive( void ) const;
	auto		HasStolen() -> bool;
	auto		HasStolen( bool newValue ) -> void;
	bool		IsOnHorse( void ) const;
	bool		GetTownTitle( void ) const;
	bool		GetReactiveArmour( void ) const;
	bool		CanTrain( void ) const;
	bool		CanBeHired( void ) const;
	bool		GetGuildToggle( void ) const;
	bool		IsTamed( void ) const;
	bool		IsGuarded( void ) const;
	bool		CanRun( void ) const;
	bool		IsUsingPotion( void ) const;
	bool		MayLevitate( void ) const;
	bool		WillHunger( void ) const;
	bool        WillThirst( void ) const;
	bool		IsMeditating( void ) const;
	bool		IsCasting( void ) const;
	bool		IsJSCasting( void ) const;

	void		SetUnicode( bool newVal );
	void		SetNpc( bool newVal );
	void		SetAwake( bool newVal );
	void		SetEvadeState( bool newVal );
	void		SetShop( bool newVal );
	void		SetDead( bool newValue );
	void		SetCanAttack( bool newValue );
	void		SetPeace( UI32 newValue );
	void		SetWar( bool newValue );
	void		SetPassive( bool newValue );
	void		SetOnHorse( bool newValue );
	void		SetTownTitle( bool newValue );
	void		SetReactiveArmour( bool newValue );
	void		SetCanHire( bool newValue );
	void		SetCanTrain( bool newValue );
	void		SetGuildToggle( bool newValue );
	void		SetTamed( bool newValue );
	void		SetGuarded( bool newValue );
	void		SetRun( bool newValue );
	void		SetUsingPotion( bool newVal );
	void		SetLevitate( bool newValue );
	void		SetHungerStatus( bool newValue );
	void		SetThirstStatus( bool newValue );
	void		SetMeditating( bool newValue );
	void		SetCasting( bool newValue );
	void		SetJSCasting( bool newValue );
	void		SetInBuilding( bool newValue );

	void		SetTownVote( UI32 newValue );
	void		SetGuildFealty( UI32 newValue );

	UI32		GetTownVote( void ) const;
	UI32		GetGuildFealty( void ) const;

	std::string	GetGuildTitle( void ) const;
	void		SetGuildTitle( const std::string &newValue );

	TIMERVAL	GetTimer( cC_TID timerId ) const;
	TIMERVAL	GetRegen( UI08 part ) const;
	TIMERVAL	GetWeathDamage( UI08 part ) const;
	UI08		GetNextAct( void ) const;

	void		SetTimer( cC_TID timerId, TIMERVAL value );
	void		SetRegen( TIMERVAL newValue, UI08 part );
	void		SetWeathDamage( TIMERVAL newValue, UI08 part );
	void		SetNextAct( UI08 newVal );

	COLOUR		GetEmoteColour( void ) const;
	COLOUR		GetSayColour( void ) const;
	UI16		GetSkin( void ) const;

	void		SetSkin( UI16 value );
	void		SetEmoteColour( COLOUR newValue );
	void		SetSayColour( COLOUR newValue );

	SI08		GetStealth( void ) const;
	SI08		GetCell( void ) const;
	UI08		GetRunning( void ) const;
	UI08		GetStep( void ) const;
	CTownRegion *GetRegion( void ) const;
	UI16		GetRegionNum( void ) const;

	void			SetCell( SI08 newVal );
	void			SetStealth( SI08 newValue );
	void			SetRunning( UI08 newValue );
	void			SetStep( UI08 newValue );
	void			SetRegion( UI16 newValue );
	virtual void	SetOldLocation( SI16 newX, SI16 newY, SI08 newZ ) override;
	virtual void	SetLocation( SI16 newX, SI16 newY, SI08 newZ, UI08 world, UI16 instanceId ) override;
	virtual void	SetLocation( SI16 newX, SI16 newY, SI08 newZ ) override;
	virtual void	SetLocation( const CBaseObject *toSet ) override;
	void			WalkZ( SI08 newZ );
	void			WalkDir( SI08 newDir );

	CItem *		GetPackItem( void );
	CChar *		GetTarg( void ) const;
	CChar *		GetAttacker( void ) const;
	UI16		GetAdvObj( void ) const;
	RACEID		GetRaceGate( void ) const;

	void		SetPackItem( CItem *newVal );
	void		SetTarg( CChar *newTarg );
	void		SetAttacker( CChar *newValue );
	void		SetAdvObj( UI16 newValue );
	void		SetRaceGate( RACEID newValue );

	SI08		GetSpellCast( void ) const;
	void		SetSpellCast( SI08 newValue );

	UI16		GetPriv( void ) const;
	SI08		GetTownPriv( void ) const;
	bool		IsGM( void ) const;
	bool		CanBroadcast( void ) const;
	bool		IsInvulnerable( void ) const;
	bool		GetSingClickSer( void ) const;
	bool		NoSkillTitles( void ) const;
	bool		IsGMPageable( void ) const;
	bool		CanSnoop( void ) const;
	bool		IsCounselor( void ) const;
	bool		AllMove( void ) const;
	bool		IsFrozen( void ) const;
	bool		ViewHouseAsIcon( void ) const;
	bool		NoNeedMana( void ) const;
	bool		IsDispellable( void ) const;
	bool		IsTempReflected( void ) const;
	bool		IsPermReflected( void ) const;
	bool		NoNeedReags( void ) const;

	void		SetGM( bool newValue );
	void		SetBroadcast( bool newValue );
	void		SetInvulnerable( bool newValue );
	void		SetSingClickSer( bool newValue );
	void		SetSkillTitles( bool newValue );
	void		SetGMPageable( bool newValue );
	void		SetSnoop( bool newValue );
	void		SetCounselor( bool newValue );
	void		SetAllMove( bool newValue );
	void		SetFrozen( bool newValue );
	void		SetViewHouseAsIcon( bool newValue );
	void		SetNoNeedMana( bool newValue );
	void		SetDispellable( bool newValue );
	void		SetTempReflected( bool newValue );
	void		SetPermReflected( bool newValue );
	void		SetNoNeedReags( bool newValue );

	void		SetPriv( UI16 newValue );
	void		SetTownpriv( SI08 newValue );

	UI16		GetBaseSkill( UI08 skillToGet ) const;
	UI16		GetSkill( UI08 skillToGet ) const;

	void		SetBaseSkill( SKILLVAL newSkillValue, UI08 skillToSet );
	void		SetSkill( SKILLVAL newSkillValue, UI08 skillToSet );

	UI16		GetDeaths( void ) const;		// can we die 4 billion times?!
	SI16		GetGuildNumber( void ) const;
	UI08		GetFlag( void ) const;
	UI08		GetControlSlotsUsed( void ) const;

	void		SetDeaths( UI16 newVal );
	void		SetFlag( UI08 newValue );
	void		SetGuildNumber( SI16 newValue );
	void		SetControlSlotsUsed( UI08 newValue );

	SI08		GetFontType( void ) const;
	void		SetFontType( SI08 newType );

	CSocket *	GetSocket( void ) const;
	void		SetSocket( CSocket *newVal );

	CChar();
	virtual		~CChar();

	CChar *			Dupe( void );
	virtual void	RemoveFromSight( CSocket *mSock = nullptr );
	virtual void	RemoveAllObjectsFromSight( CSocket *mSock = nullptr );
	void			SendWornItems( CSocket *s );
	void			Teleport( void );
	void			ExposeToView( void );
	virtual void	Update( CSocket *mSock = nullptr, bool drawGamePlayer = false, bool sendToSelf = true ) override;
	virtual void	SendToSocket( CSocket *s, bool drawGamePlayer = false ) override;

	CItem *			GetItemAtLayer( ItemLayers Layer );
	bool			WearItem( CItem *toWear );
	bool			TakeOffItem( ItemLayers Layer );

	CItem *			FirstItem( void );
	CItem *			NextItem( void );
	bool			FinishedItems( void );

	void			BreakConcentration( CSocket *sock = nullptr );

	virtual bool	Save( std::ostream &outStream ) override;
	virtual void	PostLoadProcessing( void ) override;

	SI16			ActualStrength( void ) const;
	virtual SI16	GetStrength( void ) const override;

	SI16			ActualDexterity( void ) const;
	virtual SI16	GetDexterity( void ) const override;

	SI16			ActualIntelligence( void ) const;
	virtual SI16	GetIntelligence( void ) const override;

	void			IncStrength2( SI16 toAdd = 1 );
	void			IncDexterity2( SI16 toAdd = 1 );
	void			IncIntelligence2( SI16 toAdd = 1 );

	bool			IsMurderer( void ) const;
	bool			IsCriminal( void ) const;
	bool			IsInnocent( void ) const;
	bool			IsNeutral( void ) const;

	void			SetFlagRed( void );
	void			SetFlagBlue( void );
	void			SetFlagGray( void );
	void			SetFlagNeutral( void );

	void			StopSpell( void );
	bool			SkillUsed( UI08 skillNum ) const;
	void			SkillUsed( bool value, UI08 skillNum );

	bool			IsPolymorphed( void ) const;
	bool			IsIncognito( void ) const;
	bool			IsDisguised( void ) const;
	void			IsPolymorphed( bool newValue );
	void			IsIncognito( bool newValue );
	void			IsDisguised( bool newValue );
	bool			IsJailed( void ) const;

	void			SetMaxHP( UI16 newmaxhp, UI16 newoldstr, RACEID newoldrace );
	void			SetFixedMaxHP( SI16 newmaxhp );
	void			SetMaxMana( SI16 newmaxmana, UI16 newoldint, RACEID newoldrace );
	void			SetFixedMaxMana( SI16 newmaxmana );
	void			SetMaxStam( SI16 newmaxstam, UI16 newolddex, RACEID newoldrace );
	void			SetFixedMaxStam( SI16 newmaxstam );
	virtual UI16	GetMaxHP( void );
	SI16			GetMaxMana( void );
	SI16			GetMaxStam( void );
	UI16			GetMaxLoyalty( void ) const;
	UI16			GetLoyalty( void ) const;
	virtual void	SetMana( SI16 newValue ) override;
	virtual void	SetHP( SI16 newValue ) override;
	virtual void	SetStamina( SI16 newValue ) override;
	virtual void	SetStrength( SI16 newValue ) override;
	virtual void	SetDexterity( SI16 newValue ) override;
	virtual void	SetIntelligence( SI16 newValue ) override;
	virtual void	SetStrength2( SI16 newValue ) override;
	virtual void	SetDexterity2( SI16 newValue ) override;
	virtual void	SetIntelligence2( SI16 newValue ) override;

	virtual void	SetHealthBonus( SI16 newValue ) override;
	virtual void	SetStaminaBonus( SI16 newValue ) override;
	virtual void	SetManaBonus( SI16 newValue ) override;

	void			IncHealthBonus( SI16 toAdd = 1 );
	void			IncStaminaBonus( SI16 toAdd = 1 );
	void			IncManaBonus( SI16 toAdd = 1 );

	void			IncStamina( SI16 toInc );
	void			IncMana( SI16 toInc );
	void			SetMaxLoyalty( UI16 newMaxLoyalty );
	void			SetLoyalty( UI16 newLoyalty );

	void			ToggleCombat( void );

	virtual void	SetPoisoned( UI08 newValue ) override;

	bool			InDungeon( void );
	bool			InBuilding( void );

	void			TextMessage( CSocket *s, std::string toSay, SpeechType msgType, bool spamTimer );
	void			TextMessage( CSocket *s, SI32 dictEntry, SpeechType msgType, int spamTimer, ... );

	virtual void	Cleanup( void ) override;
	virtual void	Delete( void ) override;
	virtual bool	CanBeObjType( ObjectType toCompare ) const override;

	FlagColors		FlagColour( CChar *toCompare );
	void			Heal( SI16 healValue, CChar *healer = nullptr );
	bool			Damage( SI16 damageValue, WeatherType damageType, CChar *attacker = nullptr, bool doRepsys = false );
	SI16			GetKarma( void ) const;
	void			ReactOnDamage( WeatherType damageType, CChar *attacker = nullptr );
	void			Die( CChar *attacker, bool doRepsys );

	// Values determining if the character is in a party or not, save us shortcutting in a few places
	// These values don't get saved or loaded, as only NPC parties get rebuilt, and that will be done
	// via the PartyFactory Load/Save routines, and not through here
	void			InParty( bool value );
	bool			InParty( void ) const;

	// NPC Characters
protected:
	virtual void	RemoveSelfFromOwner( void ) override;
	virtual void	AddSelfToOwner( void ) override;
public:
	void		ClearFriendList( void );
	auto 		GetFriendList() -> std::vector<CChar *>*;

	void		ClearPetOwnerList( void );
	bool		AddPetOwnerToList( CChar *toAdd );
	bool		RemovePetOwnerFromList( CChar *toRemove );
	bool		IsOnPetOwnerList( CChar *toCheck );

	bool		AddFriend( CChar *toAdd );
	bool		RemoveFriend( CChar *toRemove );

	auto		GetCombatIgnore() const -> const std::unordered_map<SERIAL, TargetInfo>;
	auto		AddToCombatIgnore( SERIAL toAdd, bool isNpc ) -> void;
	auto		RemoveFromCombatIgnore( SERIAL toRemove ) -> void;
	auto		CheckCombatIgnore( SERIAL toCheck ) -> bool;
	auto		ClearCombatIgnore() -> void;
	auto		CombatIgnoreMaintenance() -> void;

	SI16		GetNpcAiType( void ) const;
	UI16		GetNPCGuild( void ) const;
	SI16		GetTaming( void ) const;
	SI16		GetPeaceing( void ) const;
	SI16		GetProvoing( void ) const;
	UI08		GetTrainingPlayerIn( void ) const;
	UI32		GetHoldG( void ) const;
	UI08		GetSplit( void ) const;
	UI08		GetSplitChance( void ) const;
	UI08		GetOwnerCount( void );
	UI08		GetControlSlots( void ) const;
	UI16		GetOrneriness( void ) const;

	void		SetNPCAiType( SI16 newValue );
	void		SetNPCGuild( UI16 newValue );
	void		SetTaming( SI16 newValue );
	void		SetPeaceing( SI16 newValue );
	void		SetProvoing( SI16 newValue );
	void		SetTrainingPlayerIn( UI08 newValue );
	void		SetHoldG( UI32 newValue );
	void		SetSplit( UI08 newValue );
	void		SetSplitChance( UI08 newValue );
	void		SetControlSlots( UI08 newVal );
	void		SetOrneriness( UI16 newVal );

	SI08		GetPathFail( void ) const;
	void		SetPathFail( SI08 newValue );

	SI08		GetPathResult( void ) const;
	void		SetPathResult( SI08 newValue );

	UI16		GetPathTargX( void ) const;
	void		SetPathTargX( UI16 newValue );
	UI16		GetPathTargY( void ) const;
	void		SetPathTargY( UI16 newValue );

	void			SetGuarding( CBaseObject *newValue );

	CBaseObject *	GetGuarding( void ) const;

	SI16		GetFx( UI08 part ) const;
	SI16		GetFy( UI08 part ) const;
	SI08		GetFz( void ) const;
	SI08		GetNpcWander( void ) const;
	SI08		GetOldNpcWander( void ) const;

	void		SetFx( SI16 newVal, UI08 part );
	void		SetFy( SI16 newVal, UI08 part );
	void		SetFz( SI08 newVal );
	void		SetNpcWander( SI08 newValue, bool initArea = false );
	void		SetOldNpcWander( SI08 newValue );

	CChar *		GetFTarg( void ) const;
	void		SetFTarg( CChar *newTarg );

	SI16		GetSpAttack( void ) const;
	SI08		GetSpDelay( void ) const;

	void		SetSpAttack( SI16 newValue );
	void		SetSpDelay( SI08 newValue );

	UI08		GetQuestType( void ) const;
	UI08		GetQuestOrigRegion( void ) const;
	UI08		GetQuestDestRegion( void ) const;

	void		SetQuestDestRegion( UI08 newValue );
	void		SetQuestType( UI08 newValue );
	void		SetQuestOrigRegion( UI08 newValue );

	SI16		GetFleeAt( void ) const;
	SI16		GetReattackAt( void ) const;
	UI08		GetFleeDistance( void ) const;

	void		SetFleeAt( SI16 newValue );
	void		SetReattackAt( SI16 newValue );
	void		SetFleeDistance( UI08 newValue );

	UI08		PopDirection( void );
	void		PushDirection( UI08 newDir, bool pushFront = false );
	bool		StillGotDirs( void ) const;
	void		FlushPath( void );

	cNPC_FLAG	GetNPCFlag( void ) const;
	void		SetNPCFlag( cNPC_FLAG flagType );

	R32			GetWalkingSpeed( void ) const;
	void		SetWalkingSpeed( R32 newValue );

	R32			GetRunningSpeed( void ) const;
	void		SetRunningSpeed( R32 newValue );

	R32			GetFleeingSpeed( void ) const;
	void		SetFleeingSpeed( R32 newValue );

	R32			GetMountedWalkingSpeed( void ) const;
	void		SetMountedWalkingSpeed( R32 newValue );

	R32			GetMountedRunningSpeed( void ) const;
	void		SetMountedRunningSpeed( R32 newValue );

	R32			GetMountedFleeingSpeed( void ) const;
	void		SetMountedFleeingSpeed( R32 newValue );

	// Player Characters
public:
	void					SetAccount( CAccountBlock_st& actbAccount );
	CAccountBlock_st &		GetAccount(void);
	UI16					GetAccountNum( void ) const;
	void					SetAccountNum( UI16 newVal );

	void		SetRobe( SERIAL newValue );
	SERIAL		GetRobe( void ) const;

	UI16		GetOrgId( void ) const;
	void		SetOrgSkin( UI16 value );
	void		SetOrgId( UI16 value );
	UI16		GetOrgSkin( void ) const;
	std::string	GetOrgName( void ) const;
	void		SetOrgName( std::string newName );

	UI08		GetCommandLevel( void ) const;
	void		SetCommandLevel( UI08 newValue );
	UI08		GetPostType( void ) const;
	void		SetPostType( UI08 newValue );
	void		SetPlayerCallNum( SERIAL newValue );
	void		SetCallNum( SERIAL newValue );

	SERIAL		GetCallNum( void ) const;
	SERIAL		GetPlayerCallNum( void ) const;

	void		SetLastOn( std::string newValue );
	std::string GetLastOn( void ) const;
	void		SetLastOnSecs( UI32 newValue );
	UI32		GetLastOnSecs( void ) const;

	auto		GetPlayTime() const -> UI32;
	auto		SetPlayTime( UI32 newValue ) -> void;

	void		SetCreatedOn( UI32 newValue );
	UI32		GetCreatedOn( void ) const;

	void		SetNPCGuildJoined( UI32 newValue );
	UI32		GetNPCGuildJoined( void ) const;

	UI32		LastMoveTime( void ) const;
	void		LastMoveTime( UI32 newValue );


	CChar *		GetTrackingTarget( void ) const;
	CChar *		GetTrackingTargets( UI08 targetNum ) const;
	SERIAL		GetTrackingTargetSerial( void ) const;
	void		SetTrackingTarget( CChar *newValue );
	void		SetTrackingTargets( CChar *newValue, UI08 targetNum );

	UI08		GetSquelched( void ) const;
	void		SetSquelched( UI08 newValue );

	CItem *		GetSpeechItem( void ) const;
	UI08		GetSpeechMode( void ) const;
	UI08		GetSpeechId( void ) const;
	cScript *	GetSpeechCallback( void ) const;

	void		SetSpeechMode( UI08 newValue );
	void		SetSpeechId( UI08 newValue );
	void		SetSpeechCallback( cScript *newValue );
	void		SetSpeechItem( CItem *newValue );

	UI16		GetHairStyle( void ) const;
	UI16		GetBeardStyle( void ) const;
	COLOUR		GetHairColour( void ) const;
	COLOUR		GetBeardColour( void ) const;

	void		SetHairColour( COLOUR value );
	void		SetBeardColour( COLOUR value );
	void		SetHairStyle( UI16 value );
	void		SetBeardStyle( UI16 value );

	UI08		GetFixedLight( void ) const;
	void		SetFixedLight( UI08 newVal );

	UI08		GetAtrophy( UI08 skillToGet ) const;
	SkillLock	GetSkillLock( UI08 skillToGet ) const;
	void		SetAtrophy( UI08 newValue, UI08 skillToSet );
	void		SetSkillLock( SkillLock newValue, UI08 skillToSet );

	UI32		CountHousesOwned( bool countCoOwnedHouses );
};


#endif

