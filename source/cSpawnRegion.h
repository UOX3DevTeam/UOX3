#ifndef __SpawnRegion__
#define __SpawnRegion__

class cSpawnRegion	//Regionspawns
{
protected:
	char		name[128];			// Any Name to show up when this region is spawned [512]

	stringList	npcs;				// Individual Npcs
	stringList	items;				// Individual Items

	SI32		regionnum;			// Region Number

// These two will replace maxspawn

	SI32		maxcspawn;			// Maximum amount of characters to spawn
	SI32		maxispawn;			// Maximum amount of items to spawn

// These two will replace current

	SI32		curcspawn;			// Current amount of spawned characters
	SI32		curispawn;			// Current amount of spawned items

	SI32		mintime;			// Minimum spawn time
	SI32		maxtime;			// Maximum spawn time
	SI32		nexttime;			// Nextspawn time for this region

// These are to keep track of the items/characters spawned

	CHARLIST	scharlist;			// Vector of pointers to spawned characters
	ITEMLIST	sitemlist;			// Vector of pointers to spawned items

// Box values

	SI16		x1;					// Top left X
	SI16		x2;					// Bottom right x
	SI16		y1;					// Top left y
	SI16		y2;					// Bottom right y
	
	SI08		z;					// Z position of spawned items

	SI16		call;				// # of times that an NPC or ITEM is spawned from a list
	UI08		worldNumber;		// which world are we spawning in?

public:
				cSpawnRegion( SI32 spawnregion );
	virtual		~cSpawnRegion();
	void		Load( ScriptSection *toScan );
	void		doRegionSpawn( void );

	CChar *		RegionSpawnChar( void );
	CItem *		RegionSpawnItem( void );

	const char *	GetName( void ) const;
	SI32		GetRegionNum( void ) const;
	SI32		GetMaxSpawn( void ) const;
	SI32		GetMaxCharSpawn( void ) const;
	SI32		GetMaxItemSpawn( void ) const;
	SI32		GetCurrent( void ) const;
	SI32		GetCurrentCharAmt( void ) const;
	SI32		GetCurrentItemAmt( void ) const;
	SI32		GetMinTime( void ) const;
	SI32		GetMaxTime( void ) const;
	SI32		GetNextTime( void ) const;
	SI16		GetX1( void ) const;
	SI16		GetY1( void ) const;
	SI16		GetX2( void ) const;
	SI16		GetY2( void ) const;
	SI08		GetZ( void ) const;

	void		SetName( const char *newName );
	void		SetRegionNum( SI32 newVal );
	void		SetMaxCharSpawn( SI32 newVal );
	void		SetMaxItemSpawn( SI32 newVal );
	void		SetMinTime( SI32 newVal );
	void		SetMaxTime( SI32 newVal );
	void		SetNextTime( SI32 newVal );
	void		SetX1( SI16 newVal );
	void		SetY1( SI16 newVal );
	void		SetX2( SI16 newVal );
	void		SetY2( SI16 newVal );
	void		SetZ( SI08 newVal );

	void		checkSpawned();
	void		deleteSpawnedChar( CChar *toDelete );
	void		deleteSpawnedItem( CItem *toDelete );
private:
	bool		FindSpotForItem( SI16 &x, SI16 &y, SI08 &z );
};

#endif



