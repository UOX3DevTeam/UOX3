#ifndef __MAPSTUFF_H__
#define __MAPSTUFF_H__

#include "mapclasses.h"

namespace UOX
{

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
	std::string		staticsFile;
	std::string		staidxFile;
	std::string		mapDiffFile;
	std::string		mapDiffListFile;
	std::string		staticsDiffFile;
	std::string		staticsDiffListFile;
	std::string		staticsDiffIndexFile;
	UI16			xBlock;
	UI16			yBlock;
	SI32			fileSize;
	UOXFile	*		mapObj;
	UOXFile *		staticsObj;
	UOXFile *		staidxObj;
	UOXFile *		mapDiffObj;
	UOXFile *		staticsDiffObj;

	std::map< UI32, UI32 > mapDiffList;
	std::map< UI32, StaticsIndex_st > staticsDiffIndex;

	MapData_st() : mapFile( "" ), staticsFile( "" ), staidxFile( "" ), mapDiffFile( "" ), mapDiffListFile( "" ), staticsDiffFile( "" ), 
		staticsDiffListFile( "" ), staticsDiffIndexFile( "" ), xBlock( 0 ), yBlock( 0 ), fileSize( -1 ), mapObj( NULL ), staticsObj( NULL ), 
		staidxObj( NULL ), mapDiffObj( NULL ), staticsDiffObj( NULL )
	{
		mapDiffList.clear();
		staticsDiffIndex.clear();
	}
	~MapData_st();
};

// full comments on this class are available in mapstuff.cpp
class MapStaticIterator
{
public:
	MapStaticIterator( UI32 x, UI32 y, UI08 world, bool exact = true );
	~MapStaticIterator()
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
private:
	staticrecord staticArray;
	SI32 baseX, baseY, pos;
	UI08 remainX, remainY;
	UI32 index, length;
	UI16 tileid;
	bool exactCoords;
	UI08 worldNumber;
	bool useDiffs;
};



class cMapStuff
{
public:
					cMapStuff();
					~cMapStuff();

	void			Load( void );

	// height functions
	SI08			StaticTop( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber, SI08 maxZ );
	SI08			DynamicElevation( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber, SI08 maxZ );
	SI08			MapElevation( SI16 x, SI16 y, UI08 worldNumber );
	SI08			AverageMapElevation( SI16 x, SI16 y, UI16 &id, UI08 worldNumber );
	SI08			TileHeight( UI16 tilenum );
	SI08			Height( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber );
	bool			IsTileWet( UI16 tilenum );

	// look at tile functions
	void			MultiArea( CMultiObj *i, SI16 &x1, SI16 &y1, SI16 &x2, SI16 &y2 );
	void			SeekMulti( UI16 multinum, SI32 *length );
	st_multi *		SeekIntoMulti( UI16 multinum, SI32 number );
	bool			SeekTile( UI16 tilenum, CTile *tile );
	void			SeekLand( UI16 landnum, CLand *land );
	map_st			SeekMap( SI16 x, SI16 y, UI08 worldNumber );

	// misc functions
	bool			CanMonsterMoveHere( SI16 x, SI16 y, SI08 z, UI08 worldNumber );
	bool			MapExists( UI08 worldNumber );

	MapData_st&		GetMapData( UI08 mapNum );
	UI08			MapCount( void ) const;
public:
	size_t			TileMem, MultisMem;

// Functions
private:
	SI08			MultiHeight( CItem *i, SI16 x, SI16 y, SI08 oldz, SI08 maxZ );
	UI16			MultiTile( CItem *i, SI16 x, SI16 y, SI08 oldz );

	UI16			DynTile( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber );
	bool			DoesTileBlock( UI16 tilenum );
	bool			DoesStaticBlock( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber );

	bool			InsideValidWorld( SI16 x, SI16 y, UI08 worldNumber = 0xFF );

	// caching functions
	void			SeekMultiSizes( UI16 multiNum, SI16& x1, SI16& x2, SI16& y1, SI16& y2 );
	void			LoadMultis( const UString &basePath );
	void			LoadDFNOverrides( void );

//Variables
private:
	typedef std::vector< MapData_st >					MAPLIST;
	typedef std::vector< MapData_st >::iterator			MAPLIST_ITERATOR;

	struct MultiItemsIndex
	{
		st_multi *	items;		// point into where the items begin.
		SI32		size;				// # of items.
		SI16		lx, ly, lz;
		SI16		hx, hy, hz;
					MultiItemsIndex() : items( NULL ), size( -1 ), lx( SHRT_MAX ), ly( SHRT_MAX ), lz( SHRT_MAX ), 
						hx( SHRT_MIN ), hy( SHRT_MIN ), hz( SHRT_MIN )
					{}
		void		Include(SI16 x, SI16 y, SI16 z);	
	};
	friend class MapStaticIterator;
	// all the world's map and static Items.
	// multiItem, tileSet, and verdata(patches really)
	CLand    *			landTile;			// the 512*32 pieces of land tile
	CTile    *			staticTile;			// the 512*32 pieces of static tile set
	st_multi *			multiItems;			// the multis cache(shadow) from files
	MultiItemsIndex *	multiIndex;	// here's our index to multiItems
	size_t				multiIndexSize;			// the length of index

	MAPLIST				MapList;
};

extern cMapStuff *Map;

}

#endif

