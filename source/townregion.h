#ifndef __TownRegion__
#define __TownRegion__

struct	miningData;

struct orePref
{
	miningData *oreIndex;
	UI16		percentChance;
};

struct regLocs
{
	SI16 x1 = 0;
	SI16 y1 = 0;
	SI16 x2 = 0;
	SI16 y2 = 0;
};


const std::string WorldTypeNames[WRLD_COUNT] = { "Spring", "Summer", "Autumn", "Winter", "Desolation", "Unknown" };

class CTownRegion
{
private:
	struct townPers
	{
		SERIAL		townMember;
		SERIAL		targVote;
		CItem *		PlayerBank;
	};

	struct GoodData_st
	{
		SI32 sellVal;
		SI32 buyVal;
		SI32 rand1;
		SI32 rand2;

		GoodData_st() : sellVal( 0 ), buyVal( 0 ), rand1( 0 ), rand2( 0 )
		{
		}
	};

	UI16				regionNum;
	std::string			name;
	UI16				musicList;
	UI08				worldNumber;
	UI16				instanceID;
	std::bitset< 11 >	priv;	// 0x01 guarded, 0x02, mark allowed, 0x04 gate allowed, 0x08 recall
	// 0x10 raining, 0x20, snowing,		 0x40 magic damage reduced to 0
	// 0x80 Dungeon region
	std::string					guardowner;
	std::vector< std::string >	guards;
	std::vector< orePref >		orePreferences;
	std::vector< townPers >		townMember;
	std::vector< UI16 >			alliedTowns;		// allied towns are ones taken over, or ones allied to.  Share resources
	std::vector< regLocs >		locations;
	std::map< SI32, GoodData_st > goodList;
	std::string			guardList;
	UI16				numGuards;
	SERIAL				mayorSerial;			// serial of the mayor, calculated on startup always
	RACEID				race;					// town's race property, guards will be racially based
	weathID				weather;				// weather system the region belongs to
	SI32				goldReserved;			// amount of gold belonging to the town's treasury

	SI32				timeSinceGuardsPaid;	// time since the guards were last paid
	SI32				timeSinceTaxedMembers;	// time since we last taxed our own members
	SI32				timeToElectionClose;	// time since the last election was completed
	SI32				timeToNextPoll;			// time since the polling booth was opened
	SI16				guardsPurchased;
	SI32				resourceCollected;		// how much we have gotten from taxes
	UI16				taxedResource;			// item # of the taxed resource
	UI16				taxedAmount;			// how much to tax
	WorldType			visualAppearance;		// seasonal choice, basically.  Each of the 4 seasons, or "dead"

	SI16				health;					// health of the town
	UI08				chanceFindBigOre;		// chance of finding big ore

	UI16				jsScript;
	std::vector<UI16>	scriptTriggers;

	void				SendEnemyGump( CSocket *sock );
	void				SendBasicInfo( CSocket *sock );
	void				SendPotentialMember( CSocket *sock );
	void				SendMayorGump( CSocket *sock );
	void				SendDefaultGump( CSocket *sock );
	SERIAL				FindPositionOf( CChar& toAdd );

	bool				RemoveCharacter( size_t position );

public:

	CTownRegion( UI16 region );
	~CTownRegion();

	bool				DisplayTownMenu( CItem *used, CSocket *sock, SI08 flag = -1 );
	bool				VoteForMayor( CSocket *sock );
	bool				DonateResource( CSocket *s, SI32 amount );
	bool				PurchaseGuard( CSocket *sock, UI08 number );
	bool				ViewBudget( CSocket *sock );
	bool				PeriodicCheck( void );
	bool				Load( Script *ss );		// entry is the region #, fp is the file to load from
	bool				Save( std::ofstream &outStream );		// entry is the region #, fp is the file to save in
	bool				InitFromScript( ScriptSection *toScan );
	bool				AddAsTownMember( CChar& toAdd );	// toAdd is the character to add
	bool				RemoveTownMember( CChar& toAdd );	// toAdd is the character to remove
	bool				IsGuarded( void ) const;
	bool				CanPlaceHouse( void ) const;
	bool				CanMark( void ) const;
	bool				CanGate( void ) const;
	bool				CanRecall( void ) const;
	bool				CanTeleport( void ) const;
	bool				CanCastAggressive( void ) const;
	bool				IsSafeZone( void ) const;
	bool				IsDungeon( void ) const;
	bool				IsMemberOfTown( CChar *player ) const;
	bool				IsAlliedTown( UI16 townToCheck ) const;
	bool				MakeAlliedTown( UI16 townToMake );

	void				IsGuarded( bool value );
	void				CanPlaceHouse( bool value );
	void				CanMark( bool value );
	void				CanGate( bool value );
	void				CanRecall( bool value );
	void				CanTeleport( bool value );
	void				CanCastAggressive( bool value );
	void				IsSafeZone( bool value );
	void				IsDungeon( bool value );
	void				SetName( std::string toSet );
	void				SetRace( RACEID newRace );
	void				TellMembers( SI32 dictEntry, ... );
	void				SendAlliedTowns( CSocket *sock );
	void				SendEnemyTowns( CSocket *sock );
	void				ForceEarlyElection( void );
	void				Possess( CTownRegion *possessorTown );
	void				SetTaxesLeft( UI32 newValue );
	void				SetReserves( UI32 newValue );
	void				CalcNewMayor( void );				// calculates the new mayor
	void				DisplayTownMembers( CSocket *sock );
	void				ViewTaxes( CSocket *s );
	void				DoDamage( SI16 reduction );
	void				TaxedAmount( UI16 amount );
	void				SetResourceID( UI16 resID );
	void				SetHealth( SI16 newValue );
	void				SetChanceBigOre( UI08 newValue );
	void				SetAppearance( WorldType worldType );

	WorldType			GetAppearance( void ) const;
	UI08				GetChanceBigOre( void ) const;
	SI32				GetGoodBuy( UI08 index ) const;
	SI32				GetGoodRnd1( UI08 index ) const;
	SI32				GetGoodRnd2( UI08 index ) const;
	SI32				GetGoodSell( UI08 index ) const;
	SI16				GetHealth( void ) const;
	CChar *				GetMayor( void );						// returns the mayor character
	SERIAL				GetMayorSerial( void ) const;			// returns the mayor's serial
	void				SetMayorSerial( SERIAL newvValue );			// sets the mayor's serial
	UI16				GetMusicList( void ) const;
	void				SetMusicList( UI16 newValue );
	std::string			GetName( void ) const;
	size_t				GetNumOrePreferences( void ) const;
	const orePref *		GetOrePreference( size_t targValue ) const;
	SI32				GetOreChance( void ) const;
	std::string			GetOwner( void ) const;
	size_t				GetPopulation( void ) const;
	RACEID				GetRace( void ) const;
	CChar *				GetRandomGuard( void );			// returns a random guard from guard list
	UI32				GetReserves( void ) const;
	UI16				GetResourceID( void ) const;
	UI32				GetTaxes( void ) const;
	weathID				GetWeather( void ) const;
	void				SetWeather( weathID newValue );
	UI16				NumGuards( void ) const;
	void				SetNumGuards( UI16 newValue );
	UI16				TaxedAmount( void ) const;
	UI08				WorldNumber( void ) const;
	void				WorldNumber( UI08 newValue );
	UI16				GetInstanceID( void ) const;
	void				SetInstanceID( UI16 newValue );

	std::vector<UI16>	GetScriptTriggers( void );
	void				AddScriptTrigger( UI16 newValue );
	void				RemoveScriptTrigger( UI16 newValue );
	void				ClearScriptTriggers( void );

	UI16				GetRegionNum( void ) const;
	void				SetRegionNum( UI16 newVal );

	size_t				GetNumLocations( void ) const;
	const regLocs *		GetLocation( size_t locNum ) const;

	std::string			GetTownMemberSerials( void ) const;
	std::vector< townPers >		GetTownMembers( void ) const;
};

#endif

