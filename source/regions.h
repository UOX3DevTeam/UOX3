// -- Region class  Added by Tauriel 3/6/1999 To lookup items by region
//									This should help for now. BTW- my first attempt at C++
//									So forgive any newbie mistakes :)
//									-- Side note, I wanted regions to be more generic, but
//									now that I have to do this, time doesn't allow for it yet.

#include "typedefs.h"

#ifndef __Region_h
#define __Region_h


typedef UI32 vIterator;

class SubRegion
{
protected:
	vector< CChar *> charData;
	vector< CItem *> itemData;
	vector< vIterator > charIteratorBackup;
	vector< vIterator > itemIteratorBackup;

	vIterator charCounter;
	vIterator itemCounter;

public:
	SubRegion(); //constructor
	virtual			~SubRegion(); //destructor
	virtual CItem *	GetCurrentItem( void );
	virtual CChar *	GetCurrentChar( void );

	virtual CItem *	FirstItem( void );
	virtual CChar *	FirstChar( void );

	virtual CItem *	GetNextItem( void );
	virtual CChar *	GetNextChar( void );

	virtual bool	FinishedItems( void );
	virtual bool	FinishedChars( void );

	virtual bool	AddItem( CItem *toAdd );
	virtual bool	AddChar( CChar *toAdd );

	virtual bool	RemoveItem( CItem *toRemove );
	virtual bool	RemoveChar( CChar *toRemove );

	virtual void	SaveToDisk( ofstream& writeDestination, SI32 mode, ofstream &houseDestination );
	virtual void	LoadFromDisk( ifstream& readDestination, SI32 mode );

	virtual void	PopItem( void );
	virtual void	PopChar( void );
	virtual void	PushItem( void );
	virtual void	PushChar( void );

};

const SI16 MapColSize = 32;
const SI16 MapRowSize = 128;

const SI16 UpperX = (SI16)(6144 / MapColSize);
const SI16 UpperY = (SI16)(4096 / MapRowSize);

class cMapRegion
{
private:
	SubRegion		internalRegions[UpperX][UpperY][NumberOfWorlds];
	SubRegion		overFlow;
	virtual void	LoadHouseMulti( ifstream &houseDestination, SI32 mode );

public:
						cMapRegion(); //constructor
	virtual				~cMapRegion(); //destructor

	virtual SubRegion *	GetCell( SI16 x, SI16 y, UI08 worldNumber );

	virtual bool		AddItem( CItem *nItem );
	virtual bool		RemoveItem( CItem *nItem );

	virtual bool		AddChar( CChar *toAdd );
	virtual bool		RemoveChar( CChar *toRemove );

	virtual void		Save( void );
	virtual void		Load( void );

	virtual int			GetGridIndex( SI16 x, SI16 y );
//	virtual SubRegion *	GetGrid( int gridIndex, UI08 worldNumber );
	virtual SubRegion *	GetGrid( int xOffset, int yOffset, UI08 worldNumber );

	virtual int			GetGridX( SI16 x );
	virtual int			GetGridY( SI16 y );

	virtual bool		Add( cBaseObject *toAdd );
	virtual bool		Remove( cBaseObject *toRemove );
};

#endif

