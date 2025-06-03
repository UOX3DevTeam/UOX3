//o------------------------------------------------------------------------------------------------o
//|	File		-	regions.h
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Region class  Added 3/6/1999 To lookup items by region
//|						This should help for now. BTW- my first attempt at C++
//|						So forgive any newbie mistakes :)
//|					Side note, I wanted regions to be more generic, but
//|						now that I have to do this, time doesn't allow for it yet.
//o------------------------------------------------------------------------------------------------o

#ifndef __Region_h
#define __Region_h

#include "mapstuff.h"

const SI16 MapColSize = 32;
const SI16 MapRowSize = 128;

const SI16 UpperX = static_cast<SI16>( 7168 / MapColSize );
const SI16 UpperY = static_cast<SI16>( 4096 / MapRowSize );

struct MapResource_st
{
	SI16	oreAmt;
	TIMERVAL	oreTime;
	SI16	logAmt;
	TIMERVAL	logTime;
	SI16	fishAmt;
	TIMERVAL	fishTime;

	MapResource_st( SI16 defOre = 0, SI16 defLog = 0, SI16 defFish = 0, TIMERVAL defOreTime = 0, TIMERVAL defLogTIme = 0, TIMERVAL defFishTIme = 0 ) :
		oreAmt( defOre ), oreTime( defOreTime ), logAmt( defLog ), logTime( defLogTIme ), fishAmt( defFish ), fishTime( defFishTIme )
	{
	}
};

class CMapRegion
{
private:
	GenericList<CItem *>	itemData;
	GenericList<CChar *>	charData;
	RegionSerialList 		regionSerialData;
	bool hasRegionChanged = false;

public:
	GenericList<CItem *> *	GetItemList( void );
	GenericList<CChar *> *	GetCharList( void );
	RegionSerialList*		GetRegionSerialList();
	bool					HasRegionChanged( void );
	void					HasRegionChanged( bool newVal );

	CMapRegion()
	{
	};
	~CMapRegion()
	{
	};

	void					SaveToDisk( std::ostream& writeDestination );
};

class CMapWorld
{
private:
	SI16							upperArrayX;
	SI16							upperArrayY;
	UI16							resourceX;
	UI16							resourceY;
	std::vector<CMapRegion>			mapRegions;
	std::vector<MapResource_st>		mapResources;
public:
	CMapWorld( void );
	CMapWorld( UI08 worldNum );
	~CMapWorld( void );

	CMapRegion *	GetMapRegion( SI16 xOffset, SI16 yOffset );
	std::vector<CMapRegion> *		GetMapRegions();

	MapResource_st&	GetResource( SI16 x, SI16 y );

	void			LoadResources( UI08 worldNum );
	void			SaveResources( UI08 worldNUm );
};

class CMapHandler
{
private:
	typedef std::vector<CMapWorld *>			WORLDLIST;
	typedef std::vector<CMapWorld *>::iterator	WORLDLIST_ITERATOR;

	WORLDLIST		mapWorlds;
	CMapRegion		overFlow;

	void		LoadFromDisk( std::istream& readDestination, SI32 baseValue, SI32 fileSize, UI32 maxSize );
public:
	CMapHandler() = default;
	~CMapHandler();
	auto Startup() -> void;

	void		Save( void );
	void		Load( void );

	bool		AddItem( CItem *nItem );
	bool		RemoveItem( CItem *nItem );

	bool		AddChar( CChar *toAdd );
	bool		RemoveChar( CChar *toRemove );

	bool		ChangeRegion( CItem *nItem, SI16 x, SI16 y, UI08 worldNum );
	bool		ChangeRegion( CChar *nChar, SI16 x, SI16 y, UI08 worldNum );

	CMapRegion *GetMapRegion( CBaseObject *mObj );
	CMapRegion *GetMapRegion( SI16 xOffset, SI16 yOffset, UI08 worldNumber );

	SI16		GetGridX( SI16 x );
	SI16		GetGridY( SI16 y );

	auto	PopulateList( SI16 x, SI16 y, UI08 worldNumber ) -> std::vector<CMapRegion *>;
	auto	PopulateList( CBaseObject *mObj ) -> std::vector<CMapRegion *>;

	CMapWorld *	GetMapWorld( UI08 worldNum );

	MapResource_st * GetResource( SI16 x, SI16 y, UI08 worldNum );
};

extern CMapHandler *MapRegion;

#endif

