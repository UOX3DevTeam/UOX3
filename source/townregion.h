#ifndef __TownRegion__
#define __TownRegion__

struct townPers
{
	SERIAL		townMember;
	SERIAL		targVote;
	CItem *		PlayerBank;
};

struct orePref
{
	int oreIndex;
	UI08 percentChance;
};

const std::string WorldTypeNames[WRLD_COUNT] = { "Spring", "Summer", "Autumn", "Winter", "Desolation", "Unknown" };

class cTownRegion
{
private:
	UI08				regionNum;
	char				name[50];
	SI32				midilist;
	UI08				worldNumber;
	UI08				priv;	// 0x01 guarded, 0x02, mark allowed, 0x04 gate allowed, 0x08 recall
								// 0x10 raining, 0x20, snowing,		 0x40 magic damage reduced to 0
								// 0x80 Dungeon region
	char				guardowner[50];
	std::vector< int >		guards;
	std::vector< orePref >	orePreferences;
	std::vector< townPers >	townMember;
	std::vector< UI08 >		alliedTowns;		// allied towns are ones taken over, or ones allied to.  Share resources
	SERIAL				mayorSerial;			// serial of the mayor, calculated on startup always
	RACEID				race;					// town's race property, guards will be racially based
	weathID				weather;				// weather system the region belongs to
	SI32				goodsell[256];			// Magius(CHE)
	SI32				goodbuy[256];			// Magius(CHE)
	SI32				goodrnd1[256];			// Magius(CHE) (2)
	SI32				goodrnd2[256];			// Magius(CHE) (2)
	SI32				goldReserved;			// amount of gold belonging to the town's treasury

	long				timeSinceGuardsPaid;	// time since the guards were last paid
	long				timeSinceTaxedMembers;	// time since we last taxed our own members
	long				timeToElectionClose;	// time since the last election was completed
	long				timeToNextPoll;			// time since the polling booth was opened
	SI16				guardsPurchased;
	long				resourceCollected;		// how much we have gotten from taxes
	UI16				taxedResource;			// item # of the taxed resource
	UI16				taxedAmount;			// how much to tax
	WorldType			visualAppearance;		// seasonal choice, basically.  Each of the 4 seasons, or "dead"

	SI16				health;					// health of the town
	SI16				minColourSkill;			// minimum skill required to check for coloured ore
	UI08				chanceFindBigOre;		// chance of finding big ore
	UI08				chanceColourOre;		// chance of finding coloured ore (lowest skilled ore would be the default chance)

	void				SendEnemyGump( cSocket *sock );
	void				SendBasicInfo( cSocket *sock );
	void				SendPotentialMember( cSocket *sock );
	void				SendMayorGump( cSocket *sock );
	void				SendDefaultGump( cSocket *sock );
	SERIAL				FindPositionOf( CChar& toAdd );

	bool				RemoveCharacter( int position );

public:

						cTownRegion( UI08 region );
	virtual				~cTownRegion();

	bool				DisplayTownMenu( CItem *used, cSocket *sock, SI08 flag = -1 );
	bool				VoteForMayor( cSocket *sock );
	bool				DonateResource( cSocket *s, SI32 amount );
	bool				PurchaseGuard( cSocket *sock, UI08 number );
	bool				ViewBudget( cSocket *sock );
	bool				PeriodicCheck( void );
	bool				Load( Script *ss );		// entry is the region #, fp is the file to load from
	bool				Save( FILE *fp );		// entry is the region #, fp is the file to save in
	bool				InitFromScript( int& l );
	bool				AddAsTownMember( CChar& toAdd );	// toAdd is the character to add
	bool				RemoveTownMember( CChar& toAdd );	// toAdd is the character to remove
	bool				IsGuarded( void ) const;
	bool				CanMark( void ) const;
	bool				CanGate( void ) const;
	bool				CanRecall( void ) const;
	bool				CanCastAggressive( void ) const;
	bool				IsDungeon( void ) const;
	bool				IsMemberOfTown( CChar *player ) const;
	bool				IsAlliedTown( UI08 townToCheck ) const;
	bool				MakeAlliedTown( UI08 townToMake );

	void				IsGuarded( bool value );
	void				CanMark( bool value );
	void				CanGate( bool value );
	void				CanRecall( bool value );
	void				CanCastAggressive( bool value );
	void				IsDungeon( bool value );
	void				SetName( const char *toSet );
	void				SetRace( RACEID newRace );
	void				TellMembers( SI32 dictEntry, ... );
	void				SendAlliedTowns( cSocket *sock );
	void				SendEnemyTowns( cSocket *sock );
	void				ForceEarlyElection( void );
	void				Possess( UI08 possessorTown );
	void				SetTaxesLeft( long newValue );
	void				SetReserves( long newValue );
	void				CalcNewMayor( void );				// calculates the new mayor
	void				DisplayTownMembers( cSocket *sock );
	void				ViewTaxes( cSocket *s );
	void				DoDamage( SI16 reduction );
	void				TaxedAmount( UI16 amount );
	void				SetResourceID( UI16 resID );
	void				SetHealth( SI16 newValue );
	void				SetChanceBigOre( UI08 newValue );
	void				SetChanceColourOre( UI08 newValue );

	WorldType			GetAppearance( void ) const;
	UI08				GetChanceBigOre( void ) const;
	UI08				GetChanceColourOre( void ) const;
	SI32				GetGoodBuy( UI08 index ) const;
	SI32				GetGoodRnd1( UI08 index ) const;
	SI32				GetGoodRnd2( UI08 index ) const;
	SI32				GetGoodSell( UI08 index ) const;
	SI16				GetHealth( void ) const;
	CChar *				GetMayor( void );					// returns the index into chars[] of the mayor
	SERIAL				GetMayorSerial( void ) const;			// returns the mayor's serial
	SI32				GetMidiList( void ) const;
	SI16				GetMinColourSkill( void ) const;
	const char *		GetName( void ) const;
	SI32				GetNumOrePreferences( void ) const;
	const orePref *		GetOrePreference( SI32 targValue ) const;
	SI32				GetOreChance( void ) const;
	const char *		GetOwner( void ) const;
	SI32				GetPopulation( void ) const;
	RACEID				GetRace( void ) const;
	SI32				GetRandomGuard( void );			// returns a random guard from guard list
	long				GetReserves( void ) const;
	UI16				GetResourceID( void ) const;
	long				GetTaxes( void ) const;
	weathID				GetWeather( void ) const;
	UI32				NumGuards( void ) const;
	UI16				TaxedAmount( void ) const;
	UI08				WorldNumber( void ) const;
};

#endif

