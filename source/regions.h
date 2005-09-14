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

const SI16 MapColSize = 32;
const SI16 MapRowSize = 128;

const SI16 UpperX = static_cast<SI16>(7168 / MapColSize);
const SI16 UpperY = static_cast<SI16>(4096 / MapRowSize);

	class CMapRegion
	{
	private:
		CDataList< CItem * >	itemData;
		CDataList< CChar * >	charData;
	public:
		CDataList< CItem * > *	GetItemList( void );
		CDataList< CChar * > *	GetCharList( void );

								CMapRegion()
								{
								};
								~CMapRegion()
								{
								};

		void					SaveToDisk( std::ofstream& writeDestination, std::ofstream &houseDestination );
	};

	class CMapWorld
	{
	private:
		SI16			upperArrayX;
		SI16			upperArrayY;
		CMapRegion		mapRegions[UpperX][UpperY];
	public:
						CMapWorld( void )
						{
						};
						~CMapWorld( void )
						{
						};

		CMapRegion *	GetMapRegion( SI16 xOffset, SI16 yOffset );

		void			SetUpperX( SI16 newVal );
		void			SetUpperY( SI16 newVal );

		SI16			GetUpperX( void ) const;
		SI16			GetUpperY( void ) const;
	};

	class CMapHandler
	{
	private:
		std::vector< CMapWorld * >	mapWorlds;
		CMapRegion					overFlow;

		void		LoadFromDisk( std::ifstream& readDestination );
	public:
					CMapHandler();
					~CMapHandler();

		void		Save( void );
		void		Load( void );

		bool		AddItem( CItem *nItem );
		bool		RemoveItem( CItem *nItem );

		bool		AddChar( CChar *toAdd );
		bool		RemoveChar( CChar *toRemove );

		CMapRegion *GetMapRegion( CBaseObject *mObj );
		CMapRegion *GetMapRegion( SI16 xOffset, SI16 yOffset, UI08 worldNumber );

		SI16		GetGridX( SI16 x );
		SI16		GetGridY( SI16 y );

		bool		Add( CBaseObject *toAdd );
		bool		Remove( CBaseObject *toRemove );

		REGIONLIST	PopulateList( SI16 x, SI16 y, UI08 worldNumber );
		REGIONLIST	PopulateList( CBaseObject *mObj );

		CMapWorld *	GetMapWorld( UI08 worldNum );
	};

	extern CMapHandler *MapRegion;

}

#endif

