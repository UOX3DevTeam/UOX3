#ifndef __TownRegion__
#define __TownRegion__

namespace UOX
{

struct	miningData;

struct orePref
{
	miningData *oreIndex;
	UI08		percentChance;
};

struct regLocs
{
	SI16 x1;
	SI16 y1;
	SI16 x2;
	SI16 y2;
};


const std::string WorldTypeNames[WRLD_COUNT] = { "Spring", "Summer", "Autumn", "Winter", "Desolation", "Unknown" };

class cTownRegion
{
private:
	struct townPers
	{
		SERIAL		townMember;
		SERIAL		targVote;
		CItem *		PlayerBank;
	};

	UI08				regionNum;
	std::string			name;
	SI32				midilist;
	UI08				worldNumber;
	UI08				priv;	// 0x01 guarded, 0x02, mark allowed, 0x04 gate allowed, 0x08 recall
								// 0x10 raining, 0x20, snowing,		 0x40 magic damage reduced to 0
								// 0x80 Dungeon region
	std::string				guardowner;
	std::vector< std::string >		guards;
	std::vector< orePref >	orePreferences;
	std::vector< townPers >	townMember;
	std::vector< UI08 >		alliedTowns;		// allied towns are ones taken over, or ones allied to.  Share resources
	std::vector< regLocs >	locations;
	std::string			guardList;
	UI16				numGuards;
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

	UI16				jsScript;

	void				SendEnemyGump( cSocket *sock );
	void				SendBasicInfo( cSocket *sock );
	void				SendPotentialMember( cSocket *sock );
	void				SendMayorGump( cSocket *sock );
	void				SendDefaultGump( cSocket *sock );
	SERIAL				FindPositionOf( CChar& toAdd );

	bool				RemoveCharacter( size_t position );

public:

						cTownRegion( UI08 region );
						~cTownRegion();

	bool				DisplayTownMenu( CItem *used, cSocket *sock, SI08 flag = -1 );
	bool				VoteForMayor( cSocket *sock );
	bool				DonateResource( cSocket *s, SI32 amount );
	bool				PurchaseGuard( cSocket *sock, UI08 number );
	bool				ViewBudget( cSocket *sock );
	bool				PeriodicCheck( void );
	bool				Load( Script *ss );		// entry is the region #, fp is the file to load from
	bool				Save( std::ofstream &outStream );		// entry is the region #, fp is the file to save in
	bool				InitFromScript( ScriptSection *toScan );
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
	void				SetName( std::string toSet );
	void				SetRace( RACEID newRace );
	void				TellMembers( SI32 dictEntry, ... );
	void				SendAlliedTowns( cSocket *sock );
	void				SendEnemyTowns( cSocket *sock );
	void				ForceEarlyElection( void );
	void				Possess( cTownRegion *possessorTown );
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
	CChar *				GetMayor( void );						// returns the mayor character
	SERIAL				GetMayorSerial( void ) const;			// returns the mayor's serial
	SI32				GetMidiList( void ) const;
	SI16				GetMinColourSkill( void ) const;
	std::string			GetName( void ) const;
	size_t				GetNumOrePreferences( void ) const;
	const orePref *		GetOrePreference( size_t targValue ) const;
	SI32				GetOreChance( void ) const;
	std::string			GetOwner( void ) const;
	size_t				GetPopulation( void ) const;
	RACEID				GetRace( void ) const;
	CChar *				GetRandomGuard( void );			// returns a random guard from guard list
	long				GetReserves( void ) const;
	UI16				GetResourceID( void ) const;
	long				GetTaxes( void ) const;
	weathID				GetWeather( void ) const;
	UI16				NumGuards( void ) const;
	UI16				TaxedAmount( void ) const;
	UI08				WorldNumber( void ) const;

	UI16				GetScriptTrigger( void ) const;
	void				SetScriptTrigger( UI16 newValue );

	UI08				GetRegionNum( void ) const;
	void				SetRegionNum( UI08 newVal );

	size_t				GetNumLocations( void ) const;
	const regLocs *		GetLocation( size_t locNum ) const;
};

extern std::vector< cTownRegion * >		regions;

}

#endif

