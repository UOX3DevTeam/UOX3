// -- Region class  Added by Tauriel 3/6/1999 To lookup items by region
//									This should help for now. BTW- my first attempt at C++
//									So forgive any newbie mistakes :)
//									-- Side note, I wanted regions to be more generic, but
//									now that I have to do this, time doesn't allow for it yet.

#ifndef __Region_h
#define __Region_h

#ifdef _BORLAND_
#include "typedefs.h"	// sth: the local one... ICK
#endif

const int CharacterOffset = 1000000;

typedef vector< int >::iterator vIterator;
// Here's the question.  Should we have an ItemSubRegion class and a CharSubRegion class and inherit from both?
// ORRRRRRR... do we leave it as is?
// And one final option... make it polymorphic?  Make a CObject base class, and store SERIAL, CObject * and static cast as needed?

class SubRegion
{
protected:
	vector< CChar *> charData;
	vector< CItem *> itemData;
	vector< int > charIteratorBackup;
	vector< int > itemIteratorBackup;

	int charCounter;
	int itemCounter;

public:
	SubRegion(); //constructor
	virtual ~SubRegion(); //destructor
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

const int MapWidth = 6144;
const int MapHeight = 4096;

const int MapTileWidth = MapWidth / 8;
const int MapTileHeight = MapHeight / 8;

const long MapColSize = 32;
const long MapRowSize = 128;
const long upperArrayX = MapWidth / MapColSize;
const long upperArrayY = MapHeight / MapRowSize;
class cMapRegion
{
private:
	SubRegion		internalRegions[upperArrayX][upperArrayY];
	SubRegion		overFlow;
	virtual void	LoadHouseMulti( ifstream &houseDestination, SI32 mode );

public:
	cMapRegion(); //constructor
	virtual ~cMapRegion(); //destructor

	virtual SubRegion *GetCell( short int x, short int y );

	virtual bool AddItem( CItem *nItem );
	virtual bool RemoveItem( CItem *nItem );

	virtual bool AddChar( CChar *toAdd );
	virtual bool RemoveChar( CChar *toRemove );

	virtual void Save( void );
	virtual void Load( void );

	virtual int  GetGridIndex( short x, short y );
	virtual SubRegion *GetGrid( int gridIndex );
	virtual SubRegion *GetGrid( int xOffset, int yOffset );

	virtual int GetGridX( short int x );
	virtual int GetGridY( short int y );
};

#endif