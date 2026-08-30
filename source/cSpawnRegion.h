#ifndef __SpawnRegion__
#define __SpawnRegion__

class CSpawnRegion	//Regionspawns
{
private:
	std::string name;			// Any Name to show up when this region is spawned [512]

	std::vector<std::string>	sNpcs;				// Individual Npcs
	std::vector<std::string> 	sItems;				// Individual Items

	UI16		regionNum;			// Region Number

	// These two will replace maxspawn
	size_t		maxCharSpawn;			// Maximum amount of characters to spawn
	size_t		maxItemSpawn;			// Maximum amount of items to spawn

	// These two will replace current
	SI32		curCharSpawn;			// Current amount of spawned characters
	SI32		curItemSpawn;			// Current amount of spawned items

	UI08		minTime;			// Minimum spawn time
	UI08		maxTime;			// Maximum spawn time
	TIMERVAL	nextTime;			// Nextspawn time for this region

	// Box values
	SI16		x1;					// Top left X
	SI16		x2;					// Bottom right x
	SI16		y1;					// Top left y
	SI16		y2;					// Bottom right y

	SI08		prefZ;				// Maximum Z influence static and dynamic items can have on spawning.
	SI08		defZ;				// Definite Z to attempt to spawn object at

	bool		forceSpawn;			// Force spawning character, don't validate location
	bool		onlyOutside;		// Should Chars, Items only spawn outside of buildings
	bool		isSpawner;			// Whether the items spawned are spawner objects or not
	UI16		call;				// # of times that an NPC or Item is spawned from a list
	UI08		worldNumber;		// which world are we spawning in?
	UI16		instanceId;			// Which instance are we spawning in?

	struct CachedSpawnPoint
	{
		SI16 x;
		SI16 y;
		SI08 baseZ;   // Cached Base Elevation
		SI08 staticZ; // Cached Static Elevation

		CachedSpawnPoint( SI16 X, SI16 Y, SI08 BaseZ, SI08 StaticZ ) : x( X ), y( Y ), baseZ( BaseZ ), staticZ( StaticZ ) {}
	};

	GenericList<CChar *>	spawnedChars;
	GenericList<CItem *>	spawnedItems;
	std::unordered_map<UI32, CachedSpawnPoint> validLandPosCheck;
	std::unordered_map<UI32, CachedSpawnPoint> validWaterPosCheck;
	std::vector<CachedSpawnPoint>	validLandPos;
	std::vector<CachedSpawnPoint>	validWaterPos;

	// Pre-parsed weighted entity lists for fast zero-allocation spawning
	std::vector<std::pair<std::string, UI16>> parsedNpcList;
	std::vector<std::pair<std::string, UI16>> parsedItemList;

	// Exclusion areas for Spawn Regions
	struct SpawnRegionExclusionAreas_st
	{
		SI16 x1 = 0;
		SI16 y1 = 0;
		SI16 x2 = 0;
		SI16 y2 = 0;
	};
	std::vector<SpawnRegionExclusionAreas_st>    exclusionAreas;
	std::vector<UI16> spawnRegionScripts;   // Extra scripts for anything spawned from this spawn region


public:
	CSpawnRegion( UI16 spawnregion );
	~CSpawnRegion();

	bool		Load( CScriptSection *toScan, bool isParentSpawnRegion = false );
	void		DoRegionSpawn( UI32& itemsSpawned, UI32& npcsSpawned );

	const std::string	GetName( void ) const;
	UI16		GetRegionNum( void ) const;
	size_t		GetMaxSpawn( void ) const;
	size_t		GetMaxCharSpawn( void ) const;
	size_t		GetMaxItemSpawn( void ) const;
	SI32		GetCurrent( void ) const;
	SI32		GetCurrentCharAmt( void ) const;
	SI32		GetCurrentItemAmt( void ) const;
	UI08		GetMinTime( void ) const;
	UI08		GetMaxTime( void ) const;
	TIMERVAL	GetNextTime( void ) const;
	SI16		GetX1( void ) const;
	SI16		GetY1( void ) const;
	SI16		GetX2( void ) const;
	SI16		GetY2( void ) const;
	SI08		GetDefZ( void ) const;
	SI08		GetPrefZ( void ) const;
	UI16		GetCall( void ) const;
	bool		GetOnlyOutside( void ) const;
	bool		IsSpawner( void ) const;
	bool		GetForceSpawn( void ) const;
	auto		GetNPC( void ) const -> std::vector<std::string>;
	auto		GetItem( void ) const -> std::vector<std::string>;

	void		SetName( const std::string& newName );
	void		SetRegionNum( UI16 newVal );
	void		SetMaxCharSpawn( size_t newVal );
	void		SetMaxItemSpawn( size_t newVal );
	void		IncCurrentCharAmt( SI16 incAmt = 1 );
	void		IncCurrentItemAmt( SI16 incAmt = 1 );
	void		SetMinTime( UI08 newVal );
	void		SetMaxTime( UI08 newVal );
	void		SetNextTime( TIMERVAL newVal );
	void		SetX1( SI16 newVal );
	void		SetY1( SI16 newVal );
	void		SetX2( SI16 newVal );
	void		SetY2( SI16 newVal );
	void		SetDefZ( SI08 newVal );
	void		SetPrefZ( SI08 newVal );
	void		SetCall( UI16 newVal );
	void		SetOnlyOutside( bool newVal );
	void		IsSpawner( bool newVal );
	void		SetForceSpawn( bool newVal );
	void		SetNPC( const std::string &newVal );
	void		SetNPCList( std::string newVal );
	void		SetItem( const std::string &newVal );
	void		SetItemList( std::string newVal );

	UI16		GetInstanceId( void ) const;
	void		SetInstanceId( UI16 newVal );

	UI08		WorldNumber( void ) const;
	void		WorldNumber( UI08 newVal );

	void		CheckSpawned();
	void		DeleteSpawnedChar( CChar *toDelete );
	void		DeleteSpawnedItem( CItem *toDelete );

	GenericList<CItem *> *	GetSpawnedItemsList( void );
	GenericList<CChar *> *	GetSpawnedCharsList( void );
    const		std::vector<UI16> &SpawnRegionScripts( void ) const;
    void		ClearSpawnRegionScripts( void );
    void		AddSpawnRegionScripts( UI16 scriptId );
private:
	auto		RegionSpawnChar() -> CChar *;
	auto		RegionSpawnItem() -> CItem *;

	void		BuildParsedLists();

	bool		IsInExclusionArea( SI16 x, SI16 y );
	bool		TryRandomStoredLocation( const std::vector<CachedSpawnPoint>& positions, SI16& x, SI16& y, SI08& z, SI08& z2 );

	bool		FindItemSpotToSpawn( SI16 &x, SI16 &y, SI08 &z, std::string idToSpawn );
	bool		FindCharSpotToSpawn( SI16 &x, SI16 &y, SI08 &z, bool &waterCreature, bool &amphiCreature );

	void		LoadNPCList( const std::string &npcList );
	void		LoadItemList( const std::string &itemList );
};
#endif



