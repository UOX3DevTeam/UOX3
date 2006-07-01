#ifndef __CCHAR_H__
#define __CCHAR_H__

namespace UOX
{

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
	tCHAR_POISONTIME,
	tCHAR_POISONTEXT,
	tCHAR_POISONWEAROFF,
	tCHAR_SPELLTIME,
	tCHAR_ANTISPAM,
	tCHAR_CRIMFLAG,
	tCHAR_MURDERRATE,
	tCHAR_PEACETIMER,
	// NPC Timers
	tNPC_MOVETIME,
	tNPC_SPATIMER,
	tNPC_SUMMONTIME,
	// PC Timers
	tPC_LOGOUT,
	tCHAR_COUNT
};

struct DamageTrackEntry
{
	DamageTrackEntry() : damager( INVALIDSERIAL ), damageDone( 0 ), lastDamageDone( INVALIDSERIAL ) { }
	DamageTrackEntry( SERIAL dmgr, SI32 dmgDn, TIMERVAL lstDmgDn ) : damager( dmgr ), damageDone( dmgDn ), lastDamageDone( lstDmgDn ) { }
	SERIAL		damager;			// who did the damage?
	SI32		damageDone;			// how much damage has been accumulated?
	TIMERVAL	lastDamageDone;		// when was the last time that damage was done?
};

bool DTEgreater( DamageTrackEntry &elem1, DamageTrackEntry &elem2 );

class CChar : public CBaseObject
{
private:
	typedef std::map< ItemLayers, CItem * >				LAYERLIST;
	typedef std::map< ItemLayers, CItem * >::iterator	LAYERLIST_ITERATOR;
	typedef CDataList< DamageTrackEntry * >				DAMAGETRACK;

	struct NPCValues_st
	{
							NPCValues_st();
		void				DumpBody( std::ofstream& outStream );

		SI16				aiType;
		CBaseObject *		petGuarding;
		SI16				taming;
		SI16				peaceing;
		SI16				provoing;
		UI08				trainingPlayerIn;
		UI32				goldOnHand;

		UI08				splitNum;
		UI08				splitChance;

		SI16				fx[2]; //NPC Wander Point x
		SI16				fy[2]; //NPC Wander Point y
		SI08				fz; //NPC Wander Point z

		SI08				wanderMode; // NPC Wander Mode
		SI08				oldWanderMode; // Used for fleeing npcs
		std::queue< UI08 >	pathToFollow;	// let's use a queue of directions to follow

		SI16				spellAttack;
		SI08				spellDelay;	// won't time out for more than 255 seconds!

		UI08				questType;
		UI08				questDestRegion;
		UI08				questOrigRegion;
			
		SI16				fleeAt;		// HP Level to flee at
		SI16				reAttackAt;	// HP Level to re-Attack at

		CHARLIST			petFriends;
		UI16				tamedHungerRate;	// The rate at which hunger decreases when char is tamed
		UI08				hungerWildChance;	// The chance that the char get's wild when hungry
		UString				foodList;

		SERIAL				fTarg; // NPC Follow Target

		cNPC_FLAG			npcFlag;
		
		std::bitset< 8 >	boolFlags;
	};

	struct PlayerValues_st
	{
					PlayerValues_st();
		void		DumpBody( std::ofstream& outStream );

		CSocket *	socket;

		SERIAL		robe;

		SERIAL		trackingTarget; // Tracking target ID
		CHARLIST	trackingTargets;

		UI16		accountNum;

		UI16		origID; // Backup of body type for polymorph
		UI16		origSkin;
		std::string	origName; // original name - for Incognito

		UI16		hairStyle;
		UI16		beardStyle;
		COLOUR		hairColour;
		COLOUR		beardColour;

		std::string	lastOn; //Last time a character was on
		UI32		lastOnSecs; //Last time a character was on in seconds.

		UI08		commandLevel;		// 0 = player, 1 = counselor, 2 = GM
		UI08		postType;
		SI16		callNum;		// Callnum GM or Counsellor is on
		SI16		playerCallNum;	// Players call number in GM or Counsellor Queue

		UI08		squelched; // zippy  - squelching
		UI08		fixedLight; // Fixed lighting level (For chars in dungeons, where they dont see the night)
		UI16		deaths;

		SERIAL		townvote;
		SI08		townpriv;  //0=non resident (Other privledges added as more functionality added)

		UI08		atrophy[INTELLECT+1];
		UI08		lockState[INTELLECT+1];	// state of the skill locks

		CItem *		speechItem;
		UI08		speechMode;
		UI08		speechID;
		cScript *	speechCallback;
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

	SI08		hunger;		// Level of hungerness, 6 = full, 0 = "empty"
	UI08		town;       // Matches Region number in regions.scp
	UI08		regionNum;

	UI08		brkPeaceChanceGain;
	UI08		brkPeaceChance;

	std::bitset< 32 >		bools;	// lots of flags
	std::bitset< 16 >		priv;

	SI16		guildnumber;		// Number of guild player is in (0=no guild)			(DasRaetsel)
	SERIAL		guildfealty;	// Serial of player you are loyal to (default=yourself)	(DasRaetsel)
	std::string	guildtitle;	// Title Guildmaster granted player						(DasRaetsel)

	TIMERVAL	charTimers[tCHAR_COUNT];

	TIMERVAL	regen[3];
	TIMERVAL	weathDamage[WEATHNUM];			// Light Damage timer
	UI08		nextact;						//time to next spell action..

	SI08		fonttype; // Speech font to use
	COLOUR		saycolor;
	COLOUR		emotecolor;

	SI08		stealth; // stealth ( steps already done, -1=not using )
	SI08		cell; // Reserved for jailing players
	UI08		running; // Stamina Loose while running
	UI08		step;						// 1 if step 1 0 if step 2 3 if step 1 skip 2 if step 2 skip
		
	CItem *		packitem; // Characters backpack
	SERIAL		targ; // Current combat target
	SERIAL		attacker; // Character who attacked this character
	UI16		advobj; //Has used advance gate?
	RACEID		raceGate;						// Race gate that has been used

	SI08		spellCast;

	SKILLVAL	baseskill[ALLSKILLS]; // Base skills without stat modifiers
	SKILLVAL	skill[INTELLECT+1]; // List of skills (with stat modifiers)

	UI08		flag; //1=red 2=grey 4=Blue 8=green 10=Orange	// should it not be 0x10??? sounds like we're trying to do

	LAYERLIST				itemLayers;
	LAYERLIST_ITERATOR		layerCtr;
	CDataList< CChar * >	petsControlled;
	ITEMLIST				ownedItems;
	std::bitset< 32 >		skillUsed[2];	// no more than 64 skills
	std::bitset< UT_COUNT >	updateTypes;

	UI16		maxHP;
	UI16		maxHP_oldstr;
	SI16		maxMana;
	UI16		maxMana_oldint;
	SI16		maxStam;
	UI16		maxStam_olddex;
	RACEID		oldRace;

	UI08		PoisonStrength;

	DAMAGETRACK		damageDealt;
	DAMAGETRACK		damageHealed;

	virtual bool	DumpHeader( std::ofstream &outStream ) const;
	virtual bool	DumpBody( std::ofstream &outStream ) const;
	virtual bool	HandleLine( UString &UTag, UString &data );
	virtual bool	LoadRemnants( void );

	void		CopyData( CChar *target );

	void		CreateNPC( void );
	void		CreatePlayer( void );

	bool		IsValidNPC( void ) const;
	bool		IsValidPlayer( void ) const;


public:

	virtual void	SetWeight( SI32 newVal, bool doWeightUpdate = true );

	bool		GetUpdate( UpdateTypes updateType ) const;
	void		ClearUpdate( void );
	virtual void	Dirty( UpdateTypes updateType );

	void		UpdateDamageTrack( void );

	void		SetPoisonStrength( UI08 value );
	UI08		GetPoisonStrength( void ) const;

	CDataList< CChar * > *	GetPetList( void );
	ITEMLIST *	GetOwnedItems( void );

	void		AddOwnedItem( CItem *toAdd );
	void		RemoveOwnedItem( CItem *toRemove );

	void		DoHunger( CSocket *mSock );
	void		checkPetOfflineTimeout( void );
	SI08		GetHunger( void ) const;
	UI16		GetTamedHungerRate( void ) const;
	UI08		GetTamedHungerWildChance( void ) const;
	UI08		GetTown( void ) const;
	std::string GetFood( void ) const;

	bool		SetHunger( SI08 newValue );
	void		SetTamedHungerRate( UI16 newValue );
	void		SetTamedHungerWildChance( UI08 newValue );
	void		SetTown( UI08 newValue );
	void		SetFood( std::string food );
	
	UI08		GetBrkPeaceChanceGain( void ) const;
	void		SetBrkPeaceChanceGain( UI08 newValue );

	UI08		GetBrkPeaceChance( void ) const;
	void		SetBrkPeaceChance( UI08 newValue );

	void		SetMounted( bool newValue );
	bool		GetMounted( void ) const;

	void		SetStabled( bool newValue );
	bool		GetStabled( void ) const;

	void		SetMaxHPFixed( bool newValue );
	bool		GetMaxHPFixed( void ) const;
	void		SetMaxManaFixed( bool newValue );
	bool		GetMaxManaFixed( void ) const;
	void		SetMaxStamFixed( bool newValue );
	bool		GetMaxStamFixed( void ) const;

	bool		DecHunger( const SI08 amt = 1 );

	bool		isUnicode( void ) const;
	bool		IsNpc( void ) const;
	bool		IsShop( void ) const;
	bool		IsDead( void ) const;
	bool		GetCanAttack( void ) const;
	bool		IsAtWar( void ) const;
	bool		DidAttackFirst( void ) const;
	bool		IsOnHorse( void ) const;
	bool		GetTownTitle( void ) const;
	bool		GetReactiveArmour( void ) const;
	bool		CanTrain( void ) const;
	bool		GetGuildToggle( void ) const;
	bool		IsTamed( void ) const;
	bool		IsGuarded( void ) const;
	bool		CanRun( void ) const;
	bool		IsUsingPotion( void ) const;
	bool		MayLevitate( void ) const;
	bool		WillHunger( void ) const;
	bool		IsMeditating( void ) const;
	bool		IsCasting( void ) const;
	bool		IsJSCasting( void ) const;

	void		setUnicode( bool newVal );
	void		SetNpc( bool newVal );
	void		SetShop( bool newVal );
	void		SetDead( bool newValue );
	void		SetCanAttack( bool newValue );
	void		SetPeace( UI32 newValue );
	void		SetWar( bool newValue );
	void		SetAttackFirst( bool newValue );
	void		SetOnHorse( bool newValue );
	void		SetTownTitle( bool newValue );
	void		SetReactiveArmour( bool newValue );
	void		SetCanTrain( bool newValue );
	void		SetGuildToggle( bool newValue );
	void		SetTamed( bool newValue );
	void		SetGuarded( bool newValue );
	void		SetRun( bool newValue );
	void		SetUsingPotion( bool newVal );
	void		SetLevitate( bool newValue );
	void		SetHungerStatus( bool newValue );
	void		SetMeditating( bool newValue );
	void		SetCasting( bool newValue );
	void		SetJSCasting( bool newValue );
	void		SetInBuilding( bool newValue );

	void		SetTownVote( UI32 newValue );
	void		SetGuildFealty( UI32 newValue );

	UI32		GetTownVote( void ) const;
	UI32		GetGuildFealty( void ) const;

	std::string	GetGuildTitle( void ) const;
	void		SetGuildTitle( std::string newValue );

	TIMERVAL	GetTimer( cC_TID timerID ) const;
	TIMERVAL	GetRegen( UI08 part ) const;
	TIMERVAL	GetWeathDamage( UI08 part ) const;
	UI08		GetNextAct( void ) const;

	void		SetTimer( cC_TID timerID, TIMERVAL value );
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
	UI08		GetRegionNum( void ) const;

	void			SetCell( SI08 newVal );
	void			SetStealth( SI08 newValue );
	void			SetRunning( UI08 newValue );
	void			SetStep( UI08 newValue );
	void			SetRegion( UI08 newValue );
	virtual void	SetLocation( SI16 newX, SI16 newY, SI08 newZ, UI08 world );
	virtual void	SetLocation( SI16 newX, SI16 newY, SI08 newZ );
	virtual void	SetLocation( const CBaseObject *toSet );
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

	void		SetDeaths( UI16 newVal );
	void		SetFlag( UI08 newValue );
	void		SetGuildNumber( SI16 newValue );

	SI08		GetFontType( void ) const;
	void		SetFontType( SI08 newType );

	CSocket *	GetSocket( void ) const;
	void		SetSocket( CSocket *newVal );

				CChar();
	virtual		~CChar();

	CChar *			Dupe( void );
	virtual void	RemoveFromSight( CSocket *mSock = NULL );
	void			SendWornItems( CSocket *s );
	void			Teleport( void );
	void			ExposeToView( void );
	virtual void	Update( CSocket *mSock = NULL );
	virtual void	SendToSocket( CSocket *s );

	CItem *			GetItemAtLayer( ItemLayers Layer );
	bool			WearItem( CItem *toWear );
	bool			TakeOffItem( ItemLayers Layer );

	CItem *			FirstItem( void );
	CItem *			NextItem( void );
	bool			FinishedItems( void );

	void			BreakConcentration( CSocket *sock = NULL );

	virtual bool	Save( std::ofstream &outStream );
	virtual void	PostLoadProcessing( void );

	SI16			ActualStrength( void ) const;
	virtual SI16	GetStrength( void ) const;

	SI16			ActualDexterity( void ) const;
	virtual SI16	GetDexterity( void ) const;

	SI16			ActualIntelligence( void ) const;
	virtual SI16	GetIntelligence( void ) const;

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
	void			IsPolymorphed( bool newValue );
	void			IsIncognito( bool newValue );
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
	virtual void	SetMana( SI16 newValue );
	virtual void	SetHP( SI16 newValue );
	virtual void	SetStamina( SI16 newValue );
	virtual void	SetStrength( SI16 newValue );
	virtual void	SetDexterity( SI16 newValue );
	virtual void	SetIntelligence( SI16 newValue );
	virtual void	SetStrength2( SI16 newValue );
	virtual void	SetDexterity2( SI16 newValue );
	virtual void	SetIntelligence2( SI16 newValue );
	void			IncStamina( SI16 toInc );
	void			IncMana( SI16 toInc );

	void			ToggleCombat( void );

	virtual void	SetPoisoned( UI08 newValue );
		
	bool			isHuman( void );
	bool			inDungeon( void );
	bool			inBuilding( void );

	void			talk( CSocket *s, SI32 dictEntry, bool antispam, ... );
	void			talk( CSocket *s, std::string txt, bool antispam );
	void			talkAll( std::string txt, bool antispam );
	void			talkAll( SI32 dictEntry, bool antispam, ... );

	void			emote( CSocket *s, std::string txt, bool antispam );
	void			emote( CSocket *s, SI32 dictEntry, bool antispam, ... );
	void			emoteAll( SI32 dictEntry, bool antispam, ... );

	virtual void	Cleanup( void );
	virtual void	Delete( void );
	virtual bool	CanBeObjType( ObjectType toCompare ) const;
	
	FlagColors		FlagColour( CChar *toCompare );
	void			Heal( SI16 healValue, CChar *healer = NULL );
	void			Damage( SI16 damageValue, CChar *attacker = NULL, bool doRepsys = false );
	void			ReactOnDamage( WeatherType damageType, CChar *attacker = NULL );
	void			Die( CChar *attacker, bool doRepsys );

// NPC Characters
protected:
	virtual void	RemoveSelfFromOwner( void );
	virtual void	AddSelfToOwner( void );
public:
	CHARLIST *	GetFriendList( void );

	void		AddFriend( CChar *toAdd );
	void		RemoveFriend( CChar *toRemove );

	SI16		GetNPCAiType( void ) const;
	SI16		GetTaming( void ) const;
	SI16		GetPeaceing( void ) const;
	SI16		GetProvoing( void ) const;
	UI08		GetTrainingPlayerIn( void ) const;
	UI32		GetHoldG( void ) const;
	UI08		GetSplit( void ) const;
	UI08		GetSplitChance( void ) const;

	void		SetNPCAiType( SI16 newValue );
	void		SetTaming( SI16 newValue );
	void		SetPeaceing( SI16 newValue );
	void		SetProvoing( SI16 newValue );
	void		SetTrainingPlayerIn( UI08 newValue );
	void		SetHoldG( UI32 newValue );
	void		SetSplit( UI08 newValue );
	void		SetSplitChance( UI08 newValue );

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
	void		SetNpcWander( SI08 newValue );
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

	void		SetFleeAt( SI16 newValue );
	void		SetReattackAt( SI16 newValue );

	UI08		PopDirection( void );
	void		PushDirection( UI08 newDir );
	bool		StillGotDirs( void ) const;
	void		FlushPath( void );

	cNPC_FLAG	GetNPCFlag( void ) const;
	void		SetNPCFlag( cNPC_FLAG flagType );

// Player Characters
public:
	void					SetAccount( CAccountBlock& actbAccount );
	CAccountBlock &			GetAccount(void);
	UI16					GetAccountNum( void ) const;
	void					SetAccountNum( UI16 newVal );

	void		SetRobe( SERIAL newValue );
	SERIAL		GetRobe( void ) const;

	UI16		GetOrgID( void ) const;
	void		SetOrgSkin( UI16 value );
	void		SetOrgID( UI16 value );
	UI16		GetOrgSkin( void ) const;
	std::string	GetOrgName( void ) const;
	void		SetOrgName( std::string newName );

	UI08		GetCommandLevel( void ) const;
	void		SetCommandLevel( UI08 newValue );
	UI08		GetPostType( void ) const;
	void		SetPostType( UI08 newValue );
	void		SetPlayerCallNum( SI16 newValue );
	void		SetCallNum( SI16 newValue );

	SI16		GetCallNum( void ) const;
	SI16		GetPlayerCallNum( void ) const;

	void		SetLastOn( std::string newValue );
	std::string GetLastOn( void ) const;
	void		SetLastOnSecs( UI32 newValue );
	UI32		GetLastOnSecs( void ) const;


	CChar *		GetTrackingTarget( void ) const;
	CChar *		GetTrackingTargets( UI08 targetNum ) const;
	void		SetTrackingTarget( CChar *newValue );
	void		SetTrackingTargets( CChar *newValue, UI08 targetNum );

	UI08		GetSquelched( void ) const;
	void		SetSquelched( UI08 newValue );

	CItem *		GetSpeechItem( void ) const;
	UI08		GetSpeechMode( void ) const;
	UI08		GetSpeechID( void ) const;
	cScript *	GetSpeechCallback( void ) const;

	void		SetSpeechMode( UI08 newValue );
	void		SetSpeechID( UI08 newValue );
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
	UI08		GetSkillLock( UI08 skillToGet ) const;
	void		SetAtrophy( UI08 newValue, UI08 skillToSet );
	void		SetSkillLock( UI08 newValue, UI08 skillToSet );
};

}

#endif

