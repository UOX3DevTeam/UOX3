#ifndef __CCHAR_H__
#define __CCHAR_H__

class CChar : public cBaseObject
{
	protected:

		ACTREC *	ourAccount;
		SI16		npcaitype;
		SERIAL		petguarding; // Get what a pet is guarding
		SERIAL		making; // skill number of skill using to make item, 0 if not making anything, used for house building, reduce at later date, for changeover
		SI16		taming; //Skill level required for taming	// candidate for SI16
		SI16		weight; //Total weight
		SI08		blocked;
		SI08		hunger;  // Level of hungerness, 6 = full, 0 = "empty"
		UI08		fixedlight; // Fixed lighting level (For chars in dungeons, where they dont see the night)
		UI08		town;       //Matches Region number in regions.scp
		UI08		med; // 0=not meditating, 1=meditating //Morrolan - Meditation 
		UI08		trainingplayerin; // Index in skillname of the skill the NPC is training the player in
		SI32		account;
		SI32		carve; // for new carve system
		UI32		holdg; // Gold a player vendor is holding for Owner
		SI32		addMenuLoc; // add menu location (ie menu)

		UI08		split;	// won't split into more than 255
		UI08		splitchnc;	// candidate for reduction!! char??

		UI32		bools;	// lots of flags
		bool		remove;

		SERIAL		robe;
		SERIAL		townvote;
		SERIAL		trainer;		// Serial of the NPC training the char, -1 if none.
		SERIAL		guildfealty;	// Serial of player you are loyal to (default=yourself)	(DasRaetsel)
		SERIAL		poisonserial;	// poisoning skill

		char		orgname[MAX_NAME]; // original name - for Incognito
		char		laston[MAX_LASTON]; //Last time a character was on
		char		guildtitle[MAX_GUILDTITLE];	// Title Guildmaster granted player						(DasRaetsel)

		SI32		timeout; // Combat timeout (For hitting)
		SI32		logout;//Time till logout for this char -1 means in the world or already logged out //Instalog // -1 on UI32, now signed long
		UI32		regen[3];
		UI32		npcmovetime; // Next time npc will walk
		UI32		invistimeout;
		UI32		hungertime; // Timer used for hunger, one point is dropped every 20 min
		UI32		spiritspeaktimer; // Timer used for duration of spirit speak
		UI32		spatimer;
		UI32		summontimer; //Timer for summoned creatures.
		UI32		trackingtimer; // Timer used for the duration of tracking
		UI32		fishingtimer; // Timer used to delay the catching of fish
		UI32		poisontime; // poison damage timer
		UI32		poisontxt; // poision text timer
		UI32		poisonwearofftime; // makes poision wear off ...
		SI32		spelltime; //Time when they are done casting....
		UI32		nextact; //time to next spell action..
		UI32		smoketimer;
		UI32		smokedisplaytimer;
		UI32		antispamtimer; // anti spam
		UI32		skilldelay;
		SI32		crimflag; //Time when No longer criminal -1=Not Criminal
		SI32		mutetime; //Time till they are UN-Squelched.
		SI32		objectdelay;
		TIMERVAL	weathDamage[WEATHNUM];			// Light Damage timer
		SI32		murderrate; //#of ticks until one murder decays //REPSYS 
		UI32		trackingdisplaytimer;

		UI16		xid; // Backup of body type for ghosts
		UI16		orgid; // Backup of body type for polymorph
		UI16		hairstyle;
		UI16		beardstyle;
		UI16		envokeid;

		UI16		orgSkin;
		UI16		xskin;
		UI16		haircolor;
		UI16		beardcolour;
		UI16		saycolor;
		UI16		emotecolor;

		bool		may_levitate;

		SI16		fx1; //NPC Wander Point 1 x	// this, plus fx2, are being used to store item indices.  WRONG WRONG WRONG!!!
		SI16		fx2; //NPC Wander Point 2 x	// DON'T USE IT PEOPLE!!!
		SI16		oldx; 
		SI16		oldy; 
		SI16		fy1; //NPC Wander Point 1 y
		SI16		fy2; //NPC Wander Point 2 y
		SI08		fz1; //NPC Wander Point 1 z
		SI08		dispz; 
		SI08 		oldz;
		SI08		stealth; // stealth ( steps already done, -1=not using )
		SI08		dir2;
		SI08		cell; // Reserved for jailing players
		SI08		npcWander; // NPC Wander Mode
		SI08		oldnpcWander; // Used for fleeing npcs
		SI08		fly_steps; // number of step the creatures flies if it can fly
		UI08		running; // Stamina Loose while running
		UI08		step;						// 1 if step 1 0 if step 2 3 if step 1 skip 2 if step 2 skip
		std::queue< UI08 >	pathToFollow;	// let's use a queue of directions to follow
		UI08		region;
		
		CItem *		packitem; // Only used during character creation
		CHARACTER	targ; // Current combat target
		CHARACTER	attacker; // Character who attacked this character
		CHARACTER	ftarg; // NPC Follow Target
		ITEM		smeltitem;
		ITEM		tailitem;
		ITEM		advobj; //Has used advance gate?
		ITEM		envokeitem;
		CHARACTER	swingtarg; //Tagret they are going to hit after they swing
		CHARACTER	trackingtarget; // Tracking target ID
		CHARACTER	trackingtargets[MAXTRACKINGTARGETS];
		RACEID		raceGate;						// Race gate that has been used

		SI08		casting; // 0/1 is the cast casting a spell?
		SI08		spellCast;
		SI16		spellaction; //Action of the current spell....
		SI16		spattack;
		SI08		spadelay;	// won't time out for more than 255 seconds!

		UI08		questType;
		UI08		questDestRegion;
		UI08		questOrigRegion;
		
		SI16		fleeat;		// hp level I believe, so we could really reduce this!
		SI16		reattackat;	// I believe it's hp again, so candidate for SI16?

		UI16		priv;
		UI08		commandLevel;		// 0 = player, 1 = counselor, 2 = GM
		UI08		postType;
		SI08		townpriv;  //0=non resident (Other privledges added as more functionality added)

		SI16		stm2;	// Stamina2
		SI16		mn2; // Reserved for calculation

		UI16		baseskill[ALLSKILLS+1]; // Base skills without stat modifiers
		UI16		skill[ALLSKILLS+1]; // List of skills (with stat modifiers)
		UI16		atrophy[ALLSKILLS+1];
		UI08		lockState[ALLSKILLS+1];	// state of the skill locks

		SI16		karma2;			// should be a SI16!!!!
		SI16		fame2;
		SI16		guildnumber;		// Number of guild player is in (0=no guild)			(DasRaetsel)
		UI16		deaths;
		SI08		flag; //1=red 2=grey 4=Blue 8=green 10=Orange	// should it not be 0x10??? sounds like we're trying to do

		SI16		callnum; //GM Paging
		SI16		playercallnum; //GM Paging	// shrinkage!
		SI16		pagegm; //GM Paging	// shrinkage!
		
		SI08		fonttype; // Speech font to use
		UI08		squelched; // zippy  - squelching

		SI08		poison; // used for poison skill 
		SI08		poisoned; // type of poison

		SERIAL		speechItem;
		UI08		speechMode;
		UI08		speechID;
		UI16		speechCallback;
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

		bool		InUse;
		bool		Saved;
		long		SavedAt;

		CItem *		itemLayers[MAXLAYERS];
		CHARLIST	petsControlled;
		CHARLIST	petFriends;
		ITEMLIST	ownedItems;
		UI32		skillUsed[2];	// no more than 64 skills

		UI16		maxHP;
		UI16		maxHP_oldstr;
		RACEID		maxHP_oldrace;
		SI16		maxMana;
		UI16		maxMana_oldint;
		RACEID		maxMana_oldrace;
		SI16		maxStam;
		UI16		maxStam_olddex;
		RACEID		maxStam_oldrace;

		void		RemoveSelfFromOwner( void );
		void		AddSelfToOwner( void );

		UI16 ID2;
		UI16 Skin2;
		UI16 Stamina;
		UI16 Mana;
		UI16 Nomove;
		UI16 PoisonChance;
		UI16 PoisonStrength;

	public:

		void SetPoisonStrength( UI16 value );
		void SetPoisonChance( UI16 value );
		void SetNoMove( UI16 value );
		//void SetMana( UI16 value);
		void SetStamina( UI16 value );
		void SetSkin2( UI16 value );
		void SetID2( UI16 value );
		UI16 GetPoisonStrength( void ) const;
		UI16 GetPoisonChance( void ) const;
		UI16 GetNoMove( void ) const;
		//UI16 GetMana( void );
		UI16 GetStamina( void ) const;
		UI16 GetSkin2( void ) const;
		UI16 GetID2( void ) const;


		CHARLIST *	GetPetList( void );
		CHARLIST *	GetFriendList( void );
		ITEMLIST *	GetOwnedItems( void );

		bool		IsMounted( void ) const;
		bool		MayLevitate( void ) const;
		void		MayLevitate( bool newValue );
		void		AddPet( CChar *toAdd );
		void		RemovePet( CChar *toRemove );
		void		AddFriend( CChar *toAdd );
		void		RemoveFriend( CChar *toRemove );

		void		AddOwnedItem( CItem *toAdd );
		void		RemoveOwnedItem( CItem *toRemove );

		ACTREC *	GetAccountObj( void );

		bool		IsInUse( void ) const;
		bool		IsSaved( void ) const;
		SI32		GetSavedAt( void ) const;

		void		SetInUse( bool newValue );
		void		SetSaved( bool newValue );
		void		SetSavedAt( SI32 newValue );

		SI16		GetNPCAiType( void ) const;
		SI16		GetWeight( void ) const;
		SERIAL		GetMaking( void ) const;
		SI16		GetTaming( void ) const;
		SI08		GetHidden( void ) const;
		SI08		GetHunger( void ) const;
		SI08		IsBlocked( void ) const;
		UI08		GetFixedLight( void ) const;
		UI08		GetTown( void ) const;
		UI08		GetMed( void ) const;
		UI08		GetTrainingPlayerIn( void ) const;
		SI32		GetAccount( void ) const;
		SI32		GetCarve( void ) const;
		UI32		GetHoldG( void ) const;

		void		SetAccount( SI32 newVal );
		void		SetAccountObj( ACTREC *tObj );
		void		SetWeight( SI16 newVal );		// Character c is a reference to our place in the chars[]
		void		SetFixedLight( UI08 newVal );
		void		SetHidden( SI08 newValue );		// same as weight
		void		SetHunger( SI08 newValue );
		void		SetNPCAiType( SI16 newValue );
		void		SetMaking( ITEM newValue );
		void		SetBlocked( SI08 newValue );
		void		SetTaming( SI16 newValue );
		void		SetTown( UI08 newValue );
		void		SetMed( UI08 newValue );
		void		SetHoldG( UI32 newValue );
		void		SetCarve( SI32 newValue );
		void		SetTrainingPlayerIn( UI08 newValue );

		void		DecHunger( const SI08 amt = 1 );

		UI08		GetSplit( void ) const;
		UI08		GetSplitChance( void ) const;

		void		SetSplit( UI08 newValue );
		void		SetSplitChance( UI08 newValue );

		bool		isFree( void ) const;
		bool		isUnicode( void ) const;
		bool		IsNpc( void ) const;
		bool		IsShop( void ) const;
		bool		IsDead( void ) const;
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

		void		setFree( bool newVal );
		void		setUnicode( bool newVal );
		void		SetNpc( bool newVal );
		void		SetShop( bool newVal );
		void		SetDead( bool newValue );
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

		void		SetSerial( SERIAL newSerial, CHARACTER c );
		void		SetRobe( UI32 newValue );
		void		SetOwner( UI32 newValue );
		void		SetSpawn( UI08 newVal, UI08 part, CHARACTER c );
		void		SetSpawn( UI32 newValue, CHARACTER c );
		void		SetTownVote( UI08 newValue, UI08 part );
		void		SetTownVote( UI32 newValue );
		void		SetTrainer( UI32 newValue );
		void		SetGuildFealty( UI32 newValue );
		void		SetPoisonSerial( UI32 newValue );

		UI32		GetRobe( void ) const;
		UI32		GetTownVote( void ) const;
		UI32		GetTrainer( void ) const;
		UI32		GetGuildFealty( void ) const;
		UI32		GetPoisonSerial( void ) const;
		UI08		GetRobe( UI08 part ) const;
		UI08		GetTownVote( UI08 part ) const;

		const char *GetOrgName( void ) const;
		const char *GetLastOn( void ) const;
		const char *GetGuildTitle( void ) const;

		void		SetOrgName(		const char *newName );
		void		SetLastOn(		const char *newValue );
		void		SetGuildTitle(	const char *newValue );

		SI32		GetTimeout( void ) const;	// candidate for SI16?
		SI32		GetLogout( void ) const;
		UI32		GetRegen( UI08 part ) const;
		UI32		GetNpcMoveTime( void ) const;
		UI32		GetInvisTimeout( void ) const;
		UI32		GetHungerTime( void ) const;
		UI32		GetSpiritSpeakTimer( void ) const;
		UI32		GetSpaTimer( void ) const;
		UI32		GetSummonTimer( void ) const;
		UI32		GetTrackingTimer( void ) const;
		UI32		GetFishingTimer( void ) const;
		UI32		GetPoisonTime( void ) const;
		UI32		GetPoisonTextTime( void ) const;
		UI32		GetPoisonWearOffTime( void ) const;
		SI32		GetSpellTime( void ) const;
		UI32		GetNextAct( void ) const;
		UI32		GetSmokeTimer( void ) const;
		UI32		GetSmokeDisplayTimer( void ) const;
		UI32		GetAntiSpamTimer( void ) const;
		UI32		GetSkillDelay( void ) const;
		SI32		GetCrimFlag( void ) const;
		SI32		GetMuteTime( void ) const;
		SI32		GetObjectDelay( void ) const;
		TIMERVAL	GetWeathDamage( UI08 part ) const;
		SI32		GetMurderRate( void ) const;
		UI32		GetTrackingDisplayTimer( void ) const;
		SERIAL		GetGuarding( void ) const;

		void		SetTimeout( SI32 newValue );
		void		SetRegen( UI32 newValue, UI08 part );
		void		SetNpcMoveTime( UI32 newValue );
		void		SetInvisTimeout( SI32 newValue );
		void		SetHungerTime( SI32 newValue );
		void		SetSpiritSpeakTimer( UI32 newValue );
		void		SetSpaTimer( UI32 newValue );
		void		SetSummonTimer( UI32 newValue );
		void		SetTrackingTimer( UI32 newValue );
		void		SetFishingTimer( UI32 newValue );
		void		SetPoisonTime( UI32 newValue );
		void		SetPoisonTextTime( UI32 newValue );
		void		SetPoisonWearOffTime( UI32 newValue );
		void		SetCrimFlag( SI32 newValue );
		void		SetSpellTime( UI32 newValue );
		void		SetNextAct( UI32 newValue );
		void		SetMuteTime( SI32 newValue );
		void		SetLogout( SI32 newValue );
		void		SetWeathDamage( TIMERVAL newValue, UI08 part );
		void		SetSmokeTimer( UI32 newValue );
		void		SetSmokeDisplayTimer( UI32 newValue );
		void		SetAntiSpamTimer( UI32 newValue );
		void		SetSkillDelay( UI32 newValue );
		void		SetObjectDelay( SI32 newValue );
		void		SetMurderRate( SI32 newValue );
		void		SetTrackingDisplayTimer( UI32 newValue );
		void		SetGuarding( SERIAL newValue );

		UI08		GetID( UI08 part ) const;	// 1 for id1, 2 for id2
		UI08		GetxID( UI08 part ) const;	// 1 for xid1, 2 for xid2
		UI08		GetOrgID( UI08 part ) const;	// 1 for orgid1, 2 for orgid2
		UI08		GetHairStyle( UI08 part ) const;	// in reality, we only need a SI16 to hold it all
		UI08		GetBeardStyle( UI08 part ) const;	// in reality, we only need a SI16 to hold it all
		UI08		GetEnvokeID( UI08 part ) const;
		UI16		GetEnvokeID( void ) const;
		UI16		GetID( void ) const;	// returns id1<<8 + id2
		UI16		GetxID( void ) const;	// returns xid1<<8 + xid2
		UI16		GetOrgID( void ) const;	// returns orgid1<<8 + orgid2
		UI16		GetHairStyle( void ) const;
		UI16		GetBeardStyle( void ) const;

		void		SetxID( UI08 value, UI08 part );
		void		SetxID( UI16 value );	// sets both parts!
		void		SetOrgID( UI08 value, UI08 part );
		void		SetOrgID( UI16 value );	// sets both parts!
		void		SetHairStyle( UI08 value, UI08 part );
		void		SetHairStyle( UI16 value );
		void		SetBeardStyle( UI08 value, UI08 part );
		void		SetBeardStyle( UI16 value );
		void		SetEnvokeID( UI08 value, UI08 part );
		void		SetEnvokeID( UI16 newValue );

		UI08		GetSkin( UI08 part ) const;	// in reality, we only need a SI16 to hold it all
		UI08		GetOrgSkin( UI08 part ) const;	// in reality, we only need a SI16 to hold it all
		UI08		GetxSkin( UI08 part ) const;	// in reality, we only need a SI16 to hold it all
		UI08		GetHairColour( UI08 part ) const;	// in reality, we only need a SI16 to hold it all
		UI08		GetBeardColour( UI08 part ) const;	// in reality, we only need a SI16 to hold it all
		UI08		GetSayColour( UI08 part ) const;
		UI08		GetEmoteColour( UI08 part ) const;
		UI16		GetEmoteColour( void ) const;
		UI16		GetSayColour( void ) const;
		UI16		GetHairColour( void ) const;
		UI16		GetBeardColour( void ) const;
		UI16		GetSkin( void ) const;
		UI16		GetOrgSkin( void ) const;
		UI16		GetxSkin( void ) const;

		void		SetSkin( UI08 value, UI08 part );
		void		SetSkin( UI16 value );
		void		SetOrgSkin( UI08 value, UI08 part );
		void		SetOrgSkin( UI16 value );
		void		SetxSkin( UI08 value, UI08 part );
		void		SetxSkin( UI16 value );
		void		SetHairColour( UI08 value, UI08 part );
		void		SetHairColour( UI16 value );
		void		SetBeardColour( UI08 value, UI08 part );
		void		SetBeardColour( UI16 value );
		void		SetEmoteColour( UI08 value, UI08 part );
		void		SetEmoteColour( UI16 newValue );
		void		SetSayColour( UI08 value, UI08 part );
		void		SetSayColour( UI16 newValue );

		SI16		GetFx( UI08 part ) const;
		SI16		GetFy( UI08 part ) const;
		SI08		GetDispZ( void ) const;
		SI08		GetFz( void ) const;
		SI08		GetStealth( void ) const;
		SI08		GetDir2( void ) const;
		SI08		GetCell( void ) const;
		SI08		GetNpcWander( void ) const;
		SI08		GetOldNpcWander( void ) const;
		SI08		GetFlySteps( void ) const;
		UI08		GetRunning( void ) const;
		UI08		GetStep( void ) const;
		UI08		GetRegion( void ) const;

		void		SetDispZ( SI08 newZ );
		void		SetFx( SI16 newVal, UI08 part );
		void		SetFy( SI16 newVal, UI08 part );
		void		SetFz( SI08 newVal );
		void		SetDir2( SI08 newValue );
		void		SetCell( SI08 newVal );
		void		SetStealth( SI08 newValue );
		void		SetRunning( UI08 newValue );
		void		SetNpcWander( SI08 newValue );
		void		SetOldNpcWander( SI08 newValue );
		void		SetFlySteps( SI08 newValue );
		void		SetStep( UI08 newValue );
		void		SetRegion( UI08 newValue );
		void		SetLocation( SI16 newX, SI16 newY, SI08 newZ, UI08 world );
		void		SetLocation( SI16 newX, SI16 newY, SI08 newZ );
		void		SetLocation( const cBaseObject *toSet );
		void		SetLocation( struct point3 &targ );

		CItem *		GetPackItem( void ) const;
		CHARACTER	GetTarg( void ) const;
		CHARACTER	GetAttacker( void ) const;
		CHARACTER	GetFTarg( void ) const;
		ITEM		GetSmeltItem( void ) const;
		ITEM		GetTailItem( void ) const;
		ITEM		GetAdvObj( void ) const;
		ITEM		GetEnvokeItem( void ) const;
		CHARACTER	GetSwingTarg( void ) const;
		CHARACTER	GetTrackingTarget( void ) const;
		CHARACTER	GetTrackingTargets( UI08 targetNum ) const;
		RACEID		GetRaceGate( void ) const;

		void		SetPackItem( CItem *newVal );
		void		SetTarg( CHARACTER newTarg );
		void		SetAttacker( CHARACTER newValue );
		void		SetFTarg( CHARACTER newTarg );
		void		SetTailItem( ITEM newValue );
		void		SetSmeltItem( ITEM newValue );
		void		SetAdvObj( ITEM newValue );
		void		SetEnvokeItem( ITEM newValue );
		void		SetSwingTarg( CHARACTER newValue );
		void		SetTrackingTarget( CHARACTER newValue );
		void		SetRaceGate( RACEID newValue );
		void		SetTrackingTargets( CHARACTER newValue, UI08 targetNum );

		SI08		GetCasting( void ) const;
		SI08		GetSpellCast( void ) const;
		SI16		GetSpellAction( void ) const;
		SI16		GetSpAttack( void ) const;
		SI08		GetSpDelay( void ) const;

		void		SetSpellCast( SI08 newValue );
		void		SetCasting( SI08 newValue );
		void		SetSpellAction( SI16 newValue );
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

		UI08		GetCommandLevel( void ) const;
		UI08		GetPostType( void ) const;
		UI08		GetPriv( void ) const;
		UI08		GetPriv2( void ) const;
		SI08		GetTownPriv( void ) const;
		bool		IsGM( void ) const;
		bool		CanBroadcast( void ) const;
		bool		IsInvulnerable( void ) const;
		bool		GetSingClickSer( void ) const;
		bool		NoSkillTitles( void ) const;
		bool		IsGMPageable( void ) const;
		bool		CanSnoop( void ) const;
		bool		IsCounselor( void ) const;

		void		SetGM( bool newValue );
		void		SetBroadcast( bool newValue );
		void		SetInvulnerable( bool newValue );
		void		SetSingClickSer( bool newValue );
		void		SetSkillTitles( bool newValue );
		void		SetGMPageable( bool newValue );
		void		SetSnoop( bool newValue );
		void		SetCounselor( bool newValue );
		bool		AllMove( void ) const;
		bool		IsFrozen( void ) const;
		bool		ViewHouseAsIcon( void ) const;
		bool		IsPermHidden( void ) const;
		bool		NoNeedMana( void ) const;
		bool		IsDispellable( void ) const;
		bool		IsPermReflected( void ) const;
		bool		NoNeedReags( void ) const;
		bool		GetRemove( void ) const;

		void		SetAllMove( bool newValue );
		void		SetFrozen( bool newValue );
		void		SetViewHouseAsIcon( bool newValue );
		void		SetPermHidden( bool newValue );
		void		SetNoNeedMana( bool newValue );
		void		SetDispellable( bool newValue );
		void		SetPermReflected( bool newValue );
		void		SetNoNeedReags( bool newValue );
		void		SetRemove( bool newValue );

		void		SetPriv( UI08 newValue );
		void		SetPriv2( UI08 newValue );
		void		SetPostType( UI08 newValue );
		void		SetCommandLevel( UI08 newValue );
		void		SetTownpriv( SI08 newValue );

		SI16		GetMana2( void ) const;
		SI16		GetStamina2( void ) const;

		void		SetStamina2( SI16 newVal );
		void		SetMana2( SI16 newVal );

		UI16		GetBaseSkill( UI08 skillToGet ) const;
		UI16		GetAtrophy( UI08 skillToGet ) const;
		UI08		GetSkillLock( UI08 skillToGet ) const;
		UI16		GetSkill( UI08 skillToGet ) const;

		void		SetBaseSkill( UI16 newSkillValue, UI08 skillToSet );
		void		SetSkill( UI16 newSkillValue, UI08 skillToSet );
		void		SetAtrophy( UI16 newValue, UI08 skillToSet );
		void		SetSkillLock( UI08 newSkillValue, UI08 skillToSet );

		SI16		GetKarma2( void ) const;
		SI16		GetFame2( void ) const;
		UI16		GetDeaths( void ) const;		// can we die 4 billion times?!
		SI16		GetGuildNumber( void ) const;
		SI08		GetFlag( void ) const;

		void		SetFame2( SI16 newVal );
		void		SetKarma2( SI16 newVal );
		void		SetDeaths( UI16 newVal );
		void		SetFlag( SI08 newValue );
		void		SetFlagRed( void );
		void		SetFlagBlue( void );
		void		SetFlagGray( void );
		void		SetGuildNumber( SI16 newValue );

		SI16		GetCallNum( void ) const;
		SI16		GetPageGM( void ) const;
		SI16		GetPlayerCallNum( void ) const;

		void		SetPlayerCallNum( SI16 newValue );
		void		SetCallNum( SI16 newValue );
		void		SetPageGM( SI16 newValue );

		SI08		GetSpeech( void ) const;
		SI08		GetFontType( void ) const;
		UI08		GetSquelched( void ) const;

		void		SetFontType( SI08 newType );
		void		SetSquelched( UI08 newValue );

		SI08		GetPoison( void ) const;
		SI08		GetPoisoned( void ) const;

		void		SetPoison( SI08 newValue );
		void		SetPoisoned( SI08 newValue );

					CChar( CHARACTER c, bool zeroSer = false );
		virtual		~CChar();

		CChar *		Dupe( void );
		void		RemoveFromSight( void );
		void		SendToSocket( cSocket *s, bool sendDispZ, CChar *c );
		void		Teleport( void );
		void		ExposeToView( void );
		void		HideFromView( void );
		void		Update( void );

		SERIAL		GetSpeechItem( void ) const;
		UI08		GetSpeechMode( void ) const;
		UI08		GetSpeechID( void ) const;
		UI16		GetSpeechCallback( void ) const;

		void		SetSpeechItem( SERIAL newValue );
		void		SetSpeechMode( UI08 newValue );
		void		SetSpeechID( UI08 newValue );
		void		SetSpeechCallback( UI16 newValue );
		
		SI32		GetAddPart( void ) const;
		void		SetAddPart( SI32 newValue );
		CItem *		GetItemAtLayer( UI08 Layer ) const;
		bool		WearItem( CItem *toWear );
		bool		TakeOffItem( UI08 Layer );

		CItem *		FirstItem( void ) const;
		CItem *		NextItem( void ) const;
		bool		FinishedItems( void ) const;
		UI32		NumItems( void ) const;

		bool		IsValidMount( void ) const;

		virtual bool	Save( std::ofstream &outStream, SI32 mode ) const;
		virtual bool	DumpHeader( std::ofstream &outStream, SI32 mode ) const;
		virtual bool	DumpBody( std::ofstream &outStream, SI32 mode ) const;
		virtual bool	DumpFooter( std::ofstream &outStream, SI32 mode ) const;
		virtual bool	Load( std::ifstream &inStream, CHARACTER arrayOffset );
		virtual bool	Load( BinBuffer &buff, CHARACTER arrayOffset );
		virtual bool	HandleLine( char *tag, char *data );
		virtual bool	HandleBinTag( UI08 tag, BinBuffer &buff );
		virtual bool	LoadRemnants( int arrayOffset );
		void			BreakConcentration( cSocket *sock = NULL );
		virtual bool	IsUsingPotion( void ) const;
		void			SetUsingPotion( bool newVal );

		virtual SI16	ActualStrength( void ) const;
		virtual SI16	GetStrength( void ) const;

		virtual SI16	ActualDexterity( void ) const;
		virtual SI16	GetDexterity( void ) const;

		virtual SI16	ActualIntelligence( void ) const;
		virtual SI16	GetIntelligence( void ) const;

		virtual void	IncStrength2( SI16 toAdd = 1 );
		virtual void	IncDexterity2( SI16 toAdd = 1 );
		virtual void	IncIntelligence2( SI16 toAdd = 1 );

		virtual bool	IsMurderer( void ) const;
		virtual bool	IsCriminal( void ) const;
		virtual bool	IsInnocent( void ) const;

		void			StopSpell( void );
		bool			SkillUsed( UI08 skillNum ) const;
		void			SkillUsed( bool value, UI08 skillNum );

		virtual void	PostLoadProcessing( UI32 index );

		UI08			PopDirection( void );
		void			PushDirection( UI08 newDir );
		bool			StillGotDirs( void ) const;
		void			FlushPath( void );

		bool			IsPolymorphed( void ) const;
		bool			IsIncognito( void ) const;
		void			IsPolymorphed( bool newValue );
		void			IsIncognito( bool newValue );
		bool			IsJailed( void ) const;

		virtual void	SetMaxHP( UI16 newmaxhp, SI16 newoldstr, RACEID newoldrace );
		virtual void	SetMaxMana( SI16 newmaxmana, SI16 newoldint, RACEID newoldrace );
		virtual void	SetMaxStam( SI16 newmaxstam, SI16 newolddex, RACEID newoldrace );
		virtual UI16	GetMaxHP( void );
		virtual SI16	GetMaxMana( void );
		virtual SI16	GetMaxStam( void );
		void			SetMana( SI16 mn );
		
		bool			IsInBank( CItem* i );

};


#endif

