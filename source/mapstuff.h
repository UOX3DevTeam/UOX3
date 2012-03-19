#ifndef __MAPSTUFF_H__
#define __MAPSTUFF_H__

#include "mapclasses.h"
#include <climits>

namespace UOX
{

const UI08 MAX_Z_STEP	= 9;
const UI08 MAX_Z_FALL	= 20;

/*
enum UOMapType
{
	UOMT_BRITANNIA	= 0,
	UOMT_UNKNOWN1	= 1,
	UOMT_ILSHENAR	= 2,
	UOMT_MALAS		= 3,
	UOMT_COUNT		= 4,
	UOMT_UNKNOWN	= 0xFF
};
*/

struct StaticsIndex_st
{
	UI32 offset;
	UI32 length;
	UI32 unknown;
	StaticsIndex_st() : offset( 0 ), length( 0 ), unknown( 0 )
	{
	}
};

struct MapData_st
{
	std::string		mapFile;
	std::string		mapFileUOPWrap;
	std::string		staticsFile;
	std::string		staidxFile;
	std::string		mapDiffFile;
	std::string		mapDiffListFile;
	std::string		staticsDiffFile;
	std::string		staticsDiffListFile;
	std::string		staticsDiffIndexFile;
	UI16			xBlock;
	UI16			yBlock;
	UOXFile	*		mapObj;
	UOXFile *		staticsObj;
	UOXFile *		staidxObj;
	UOXFile *		mapDiffObj;
	UOXFile *		staticsDiffObj;

	std::map< UI32, UI32 > mapDiffList;
	std::map< UI32, StaticsIndex_st > staticsDiffIndex;

	MapData_st() : mapFile( "" ), mapFileUOPWrap( "" ), staticsFile( "" ), staidxFile( "" ), mapDiffFile( "" ), mapDiffListFile( "" ), staticsDiffFile( "" ), 
		staticsDiffListFile( "" ), staticsDiffIndexFile( "" ), xBlock( 0 ), yBlock( 0 ), mapObj( NULL ), staticsObj( NULL ), 
		staidxObj( NULL ), mapDiffObj( NULL ), staticsDiffObj( NULL )
	{
		mapDiffList.clear();
		staticsDiffIndex.clear();
	}
	~MapData_st();
};

// full comments on this class are available in mapstuff.cpp
class CStaticIterator
{
private:
	Static_st	staticArray;
	SI16		baseX, baseY;
	UI32		pos;
	UI08		remainX, remainY;
	UI32		index, length;
	bool		exactCoords;
	UI08		worldNumber;
	bool		useDiffs;
public:
				CStaticIterator( SI16 x, SI16 y, UI08 world, bool exact = true );
				~CStaticIterator()
				{
				};

	Static_st *	First( void );
	Static_st *	Next( void );
	SI32		GetPos() const		{ return pos; }
	UI32		GetLength() const	{ return length; }
};

class CMulHandler
{
private:
	struct MultiItemsIndex_st
	{
		Multi_st *	items;		// point into where the items begin.
		SI32		size;				// # of items.
		SI16		lx, ly, hx, hy;
		SI08		lz, hz;
					MultiItemsIndex_st() : items( NULL ), size( -1 ), 
						lx( SHRT_MAX ), ly( SHRT_MAX ), hx( SHRT_MIN ), hy( SHRT_MIN ), 
						lz( SCHAR_MAX ), hz( SCHAR_MIN )
					{}
		void		Include( SI16 x, SI16 y, SI08 z );	
	};

	struct MultiItemsIndexHS_st
	{
		MultiHS_st *	items;		// point into where the items begin.
		SI32		size;				// # of items.
		SI16		lx, ly, hx, hy;
		SI08		lz, hz;
					MultiItemsIndexHS_st() : items( NULL ), size( -1 ), 
						lx( SHRT_MAX ), ly( SHRT_MAX ), hx( SHRT_MIN ), hy( SHRT_MIN ), 
						lz( SCHAR_MAX ), hz( SCHAR_MIN )
					{}
		void		Include( SI16 x, SI16 y, SI08 z );	
	};

	typedef std::vector< MapData_st >					MAPLIST;
	typedef std::vector< MapData_st >::iterator			MAPLIST_ITERATOR;

//Variables
	// all the world's map and static Items.
	// multiItem, tileSet, and verdata(patches really)
	CLand    *			landTile;			// the 512*32 pieces of land tile
	CLandHS *			landTileHS;			// the 512*32 pieces of land tile
	CTile    *			staticTile;			// the 512*32 pieces of static tile set
	CTileHS *			staticTileHS;		// the 512*32 pieces of static tile set
	Multi_st *			multiItems;			// the multis cache(shadow) from files
	MultiHS_st *		multiItemsHS;		// the multis cache(shadow) from files
	MultiItemsIndex_st *multiIndex;			// here's our index to multiItems
	MultiItemsIndexHS_st *multiIndexHS;		// here's our index to multiItems
	size_t				multiIndexSize;		// the length of index
	size_t				multiSize;
	size_t				tileDataSize;

	MAPLIST				MapList;

// Functions
	SI08			MultiHeight( CItem *i, SI16 x, SI16 y, SI08 oldz, SI08 maxZ );
	UI16			MultiTile( CItem *i, SI16 x, SI16 y, SI08 oldz );

	UI16			DynTile( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber );
	bool			DoesStaticBlock( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber, bool checkWater = false );
	bool			DoesDynamicBlock( SI16 x, SI16 y, SI08 z, UI08 worldNumber, bool checkWater, bool waterWalk );
	bool			DoesMapBlock( SI16 x, SI16 y, SI08 z, UI08 worldNumber, bool checkWater, bool waterWalk );
	bool			CheckStaticFlag( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber, TileFlags toCheck );

	// caching functions
	void			LoadMapsDFN( void );
	void			LoadMultis( const std::string& basePath );
	void			LoadDFNOverrides( void );
	void			LoadMapAndStatics( MapData_st& mMap, const std::string& basePath, UI08 &totalMaps );
	void			LoadTileData( const std::string& basePath );

public:
					CMulHandler();
					~CMulHandler();

	void			Load( void );

	// height functions
	SI08			StaticTop( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber, SI08 maxZ );
	SI08			DynamicElevation( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber, SI08 maxZ );
	SI08			MapElevation( SI16 x, SI16 y, UI08 worldNumber );
	SI08			TileHeight( UI16 tilenum );
	SI08			Height( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber );
	bool			inBuilding( SI16 x, SI16 y, SI08 z, UI08 worldNumber );
	bool IsIgnored(UI16 landnum) { 
		if (landnum == 2 || landnum == 0x1DB || ( landnum >= 0x1AE && landnum <= 0x1B5 ))
			return true;
		else
			return false;
	}

	// look at tile functions
	void			MultiArea( CMultiObj *i, SI16 &x1, SI16 &y1, SI16 &x2, SI16 &y2 );
	SI32			SeekMulti( UI16 multinum );
	SI32			SeekMultiHS( UI16 multinum );
	Multi_st &		SeekIntoMulti( UI16 multinum, SI32 number );
	MultiHS_st &	SeekIntoMultiHS( UI16 multinum, SI32 number );
	bool			IsValidTile( UI16 tileNum );
	CTile &			SeekTile( UI16 tileNum );
	CTileHS &		SeekTileHS( UI16 tileNum );
	CLand &			SeekLand( UI16 landNum );
	CLandHS &		SeekLandHS( UI16 landNum );
	map_st			SeekMap( SI16 x, SI16 y, UI08 worldNumber );

	// misc functions
	bool			ValidSpawnLocation( SI16 x, SI16 y, SI08 z, UI08 worldNumber, bool checkWater = true );
	bool			ValidMultiLocation( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber, bool checkWater );
	bool			MapExists( UI08 worldNumber );
	bool			InsideValidWorld( SI16 x, SI16 y, UI08 worldNumber = 0xFF );

	MapData_st&		GetMapData( UI08 mapNum );
	UI08			MapCount( void ) const;

	size_t			GetTileMem( void ) const;
	size_t			GetMultisMem( void ) const;
};

extern CMulHandler *Map;

}

#endif

