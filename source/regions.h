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

	struct MapResource_st
	{
		SI16	oreAmt;
		UI32	oreTime;
		SI16	logAmt;
		UI32	logTime;

		MapResource_st() : oreAmt( 0 ), oreTime( 0 ), logAmt( 0 ), logTime( 0 )
		{
		}
	};

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
		SI16								upperArrayX;
		SI16								upperArrayY;
		UI16								resourceX;
		UI16								resourceY;
		CMapRegion							mapRegions[UpperX][UpperY];
		std::map< UI32, MapResource_st >	mapResources;
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
		void			SetResourceX( UI16 newVal );
		void			SetResourceY( UI16 newVal );

		SI16			GetUpperX( void ) const;
		SI16			GetUpperY( void ) const;
		UI16			GetResourceX( void ) const;
		UI16			GetResourceY( void ) const;

		MapResource_st&	GetResource( SI16 x, SI16 y );

		void			LoadResources( UI08 worldNum );
		void			SaveResources( UI08 worldNUm );
	};

	class CMapHandler
	{
	private:
		typedef std::vector< CMapWorld * >				WORLDLIST;
		typedef std::vector< CMapWorld * >::iterator	WORLDLIST_ITERATOR;

		WORLDLIST		mapWorlds;
		CMapRegion		overFlow;

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

		MapResource_st * GetResource( SI16 x, SI16 y, UI08 worldNum );
	};

	extern CMapHandler *MapRegion;

}

#endif

