// -- Region class  Added by Tauriel 3/6/1999 To lookup items by region
//									This should help for now. BTW- my first attempt at C++
//									So forgive any newbie mistakes :)
//									-- Side note, I wanted regions to be more generic, but
//									now that I have to do this, time doesn't allow for it yet.

#ifndef __Region_h
#define __Region_h

#include "mapstuff.h"

namespace UOX
{

class SubRegion
{
protected:
	DCHARLIST							charData;
	DITEMLIST							itemData;
	std::vector< size_t >	charIteratorBackup;
	std::vector< size_t >	itemIteratorBackup;

	DCHARLIST_ITERATOR					charCounter;
	DITEMLIST_ITERATOR					itemCounter;

public:
			SubRegion(); //constructor
			~SubRegion(); //destructor
	CItem *	GetCurrentItem( void );
	CChar *	GetCurrentChar( void );

	CItem *	FirstItem( void );
	CChar *	FirstChar( void );

	CItem *	GetNextItem( void );
	CChar *	GetNextChar( void );

	bool	FinishedItems( void );
	bool	FinishedChars( void );

	bool	AddItem( CItem *toAdd );
	bool	AddChar( CChar *toAdd );

	bool	RemoveItem( CItem *toRemove );
	bool	RemoveChar( CChar *toRemove );

	void	SaveToDisk( std::ofstream& writeDestination, std::ofstream &houseDestination );
	void	LoadFromDisk( std::ifstream& readDestination );

	void	PopItem( void );
	void	PopChar( void );
	void	PushItem( void );
	void	PushChar( void );
};

const SI16 MapColSize = 32;
const SI16 MapRowSize = 128;

const SI16 UpperX = (SI16)(6144 / MapColSize);
const SI16 UpperY = (SI16)(4096 / MapRowSize);

class cMapRegion
{
private:
	SI16			upperArrayX[UOMT_COUNT];
	SI16			upperArrayY[UOMT_COUNT];
	SubRegion		internalRegions[UpperX][UpperY][NumberOfWorlds];
	SubRegion		overFlow;
	void			LoadHouseMulti( std::ifstream &houseDestination );

public:
						cMapRegion(); //constructor
						~cMapRegion(); //destructor

	SubRegion *	GetCell( SI16 x, SI16 y, UI08 worldNumber );

	bool		AddItem( CItem *nItem );
	bool		RemoveItem( CItem *nItem );

	bool		AddChar( CChar *toAdd );
	bool		RemoveChar( CChar *toRemove );

	void		Save( void );
	void		Load( void );

	int			GetGridIndex( SI16 x, SI16 y );
//	SubRegion *	GetGrid( int gridIndex, UI08 worldNumber );
	SubRegion *	GetGrid( int xOffset, int yOffset, UI08 worldNumber );

	SI16		GetGridX( SI16 x );
	SI16		GetGridY( SI16 y );

	bool		Add( cBaseObject *toAdd );
	bool		Remove( cBaseObject *toRemove );

	REGIONLIST	PopulateList( SI16 x, SI16 y, UI08 worldNumber );
	REGIONLIST	PopulateList( cBaseObject *mObj );
};

extern cMapRegion *MapRegion;

}

#endif

