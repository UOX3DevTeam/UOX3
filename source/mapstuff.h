#ifndef __MAPSTUFF_H__
#define __MAPSTUFF_H__

#include "mapclasses.h"

namespace UOX
{

#define MAP0CACHE 300

enum UOMapType
{
	UOMT_BRITANNIA	= 0,
	UOMT_UNKNOWN1	= 1,
	UOMT_ILSHENAR	= 2,
	UOMT_MALAS		= 3,
	UOMT_COUNT		= 4,
	UOMT_UNKNOWN	= 0xFF
};

const UI08 NumberOfWorlds				= UOMT_COUNT;
const SI16 MapWidths[UOMT_COUNT]		= { 6144, 0, 2304, 2560 };
const SI16 MapHeights[UOMT_COUNT]		= { 4096, 0, 1600, 2048 };
const SI16 MapTileWidths[UOMT_COUNT]	= { 768,  0, 288,  320 };
const SI16 MapTileHeights[UOMT_COUNT]	= { 512,  0, 200,  256 };
const SI32 MapFileLengths[UOMT_COUNT]	= { 77070336, 0, 11289600, 16056320 };

// full comments on this class are available in mapstuff.cpp
class MapStaticIterator
{
private:
	staticrecord staticArray;
	SI32 baseX, baseY, pos;
	UI08 remainX, remainY;
	UI32 index, length, tileid;
	bool exactCoords;
	UI08 worldNumber;

public:
	MapStaticIterator( UI32 x, UI32 y, UI08 world, bool exact = true );
	virtual ~MapStaticIterator()
	{
	};

	staticrecord *First( void );
	staticrecord *Next( void );
	void GetTile( CTile *tile) const;
	UI32 GetPos() const
	{
		return pos;
	}
	UI32 GetLength() const
	{
		return length;
	}
};

struct StaCache_st
{
	staticrecord *Cache;
	UI16 CacheLen;
	StaCache_st() : Cache( NULL ), CacheLen( 0 )
	{
	}
	virtual ~StaCache_st() 
	{ 
		if( Cache != NULL ) 
			delete [] Cache; 
	}
};

typedef std::vector< StaCache_st > StaCacheLine;

class StaWorldCache
{
protected:
	std::vector< StaCacheLine >	CacheList;

public:
	StaWorldCache( SI16 maxX, SI16 maxY );
	StaWorldCache( void );
	virtual void	Resize( SI16 maxX, SI16 maxY );
	virtual			~StaWorldCache();
	StaCacheLine *	GrabCacheLine( SI16 maxX );
};

class cMapStuff
{
//Variables
private:
struct MultiCache
{
	SI32	length;	// already done the / by multi length here, this is the size of our array!
	st_multi *cache;
	SI16	lx;
	SI16	ly;
	SI16	hx;
	SI16	hy;
	MultiCache() : length( -1 ), cache( NULL ), lx( 0 ), ly( 0 ), hx( 0 ), hy( 0 )
	{
	}
};

	friend class MapStaticIterator;

	// moved from global vars into here - fur 11/3/1999
	UOXFile *verfile, *tilefile, *multifile, *midxfile;

	UOXFile	*	mapArrays[NumberOfWorlds];
	UOXFile	*	statArrays[NumberOfWorlds];
	UOXFile	*	sidxArrays[NumberOfWorlds];
	UOMapType	uomapTypes[NumberOfWorlds];

	// tile caching items
	CTile		tilecache[0x4000];

	// static caching items
	UI32		StaticBlocks;

	// map caching items
	struct MapCache
	{
		UI16 xb;
		UI16 yb;
		UI08 xo;
		UI08 yo;
		map_st Cache;
	};
	MapCache Map0Cache[NumberOfWorlds][MAP0CACHE];
	std::vector< MultiCache *> multiCache;	// here's our cache, this allows us to resize it!

	// version caching items
	versionrecord *versionCache;
	UI32	versionRecordCount;
	UI16	multiCount;

	// caching functions
	void	CacheTiles( void );
	void	CacheStatics( void );
	void	CacheMultis( void );
	void	CalculateMultiSizes( void );
	void	SeekMultiSizes( UI16 multiNum, SI16& x1, SI16& x2, SI16& y1, SI16& y2 );
public:
	char mapname[80], sidxname[80], statname[80], vername[80], tilename[80], multiname[80], midxname[80];
	UI32 StaMem, TileMem, versionMemory;
	UI32 Map0CacheHit, Map0CacheMiss;

	StaWorldCache StaticCache[NumberOfWorlds];
	bool	Cache;
	
// Functions
private:
	bool	VerLand( int landnum, CLand *land );
	SI08	MultiHeight( CItem *i, SI16 x, SI16 y, SI08 oldz );
	int		MultiTile( CItem *i, SI16 x, SI16 y, SI08 oldz );
	SI32	VerSeek( SI32 file, SI32 block );
	bool	VerTile( int tilenum, CTile *tile );
	bool	TileWalk( int tilenum );
	void	CacheVersion( void );

	int		DynTile( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber );
	bool	DoesTileBlock( int tilenum );
	bool	DoesStaticBlock( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber );

	bool	InsideValidWorld( SI16 x, SI16 y, UI08 worldNumber = 0xFF );

	UOMapType	CalcFromFileLength( UOXFile *toCalcFrom );

public:
			cMapStuff();
	virtual ~cMapStuff();

	void	Load( void );

	UOMapType	GetMapType( UI08 worldNumber );

	// height functions
	SI08	StaticTop( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber );
	SI08	DynamicElevation( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber );
	SI08	MapElevation( SI16 x, SI16 y, UI08 worldNumber );
	SI08	AverageMapElevation( SI16 x, SI16 y, UI16 &id, UI08 worldNumber );
	SI08	TileHeight( UI16 tilenum );
	SI08	Height( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber );
	bool	IsTileWet( int tilenum );

	// look at tile functions
	void	MultiArea( CMultiObj *i, SI16 &x1, SI16 &y1, SI16 &x2, SI16 &y2 );
	void	SeekTile( int tilenum, CTile *tile );
	void	SeekMulti( UI32 multinum, SI32 *length );
	st_multi *SeekIntoMulti( int multinum, int number );
	void	SeekLand( int landnum, CLand *land );
	map_st	SeekMap0( SI16 x, SI16 y, UI08 worldNumber );
	bool	IsRoofOrFloorTile( CTile *tile );
	bool	IsRoofOrFloorTile( CTileUni *tile );

	// misc functions
	bool	CanMonsterMoveHere( SI16 x, SI16 y, SI08 z, UI08 worldNumber );
	StaCache_st *GrabCacheEntry( SI16 x, SI16 y, UI08 worldNumber );

	bool	MapExists( UI08 worldNumber );
};

extern cMapStuff *Map;

}

#endif

