#include "uox3.h"
#include "classes.h"
#include "regions.h"

#include "ObjectFactory.h"

namespace UOX
{

CMapHandler *MapRegion;

void LoadChar( std::ifstream& readDestination )
{
	CChar *x = static_cast< CChar * >(ObjectFactory::getSingleton().CreateBlankObject( OT_CHAR ));
	if( x == NULL ) 
		return;
	if( !x->Load( readDestination ) )
	{
		x->Cleanup();
		ObjectFactory::getSingleton().DestroyObject( x );
	}
}
void LoadItem( std::ifstream& readDestination )
{
	CItem *x = static_cast< CItem * >(ObjectFactory::getSingleton().CreateBlankObject( OT_ITEM ));
	if( x == NULL ) 
		return;
	if( !x->Load( readDestination ) )
	{
		x->Cleanup();
		ObjectFactory::getSingleton().DestroyObject( x );
	}
}
void LoadMulti( std::ifstream& readDestination )
{
	CMultiObj *ourHouse = static_cast< CMultiObj * >(ObjectFactory::getSingleton().CreateBlankObject( OT_MULTI ));
	if( !ourHouse->Load( readDestination ) )	// if no load, DELETE
	{
		ourHouse->Cleanup();
		ObjectFactory::getSingleton().DestroyObject( ourHouse );
	}
}

void LoadBoat( std::ifstream& readDestination )
{
	CBoatObj *ourBoat = static_cast< CBoatObj * >(ObjectFactory::getSingleton().CreateBlankObject( OT_BOAT ));
	if( !ourBoat->Load( readDestination ) ) // if no load, DELETE
	{
		ourBoat->Cleanup();
		ObjectFactory::getSingleton().DestroyObject( ourBoat );
	}
}

void LoadSpawnItem( std::ifstream& readDestination )
{
	CSpawnItem *ourSpawner = static_cast< CSpawnItem * >(ObjectFactory::getSingleton().CreateBlankObject( OT_SPAWNER ));
	if( !ourSpawner->Load( readDestination ) ) // if no load, DELETE
	{
		ourSpawner->Cleanup();
		ObjectFactory::getSingleton().DestroyObject( ourSpawner );
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	void SaveToDisk( std::ofstream& writeDestination, std::ofstream &houseDestination )
//|	Date			-	Unknown
//|	Developers		-	Abaddon
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Save all items and characters inside a subregion
//|								reworked SaveChar from WorldMain to deal with pointer based stuff in region rather than index based stuff in array
//|								Also saves out all data regardless (in preparation for a simple binary save)
//o--------------------------------------------------------------------------o
void CMapRegion::SaveToDisk( std::ofstream& writeDestination, std::ofstream &houseDestination )
{
	charData.Push();
	for( CChar* charToWrite = charData.First(); !charData.Finished(); charToWrite = charData.Next() )
	{
		if( !ValidateObject( charToWrite ) )
		{
			charData.Remove( charToWrite );
			continue;
		}
		//if( !charToWrite->IsNpc() && charToWrite->GetAccount() != -1 ) {
#if defined( _MSC_VER )
		#pragma todo( "PlayerHTML Dumping needs to be reimplemented" )
#endif
		//DumpPlayerHTML( charToWrite );
		if( charToWrite->ShouldSave() ) 
			charToWrite->Save( writeDestination );
	}
	charData.Pop();
	itemData.Push();
	for( CItem *itemToWrite = itemData.First(); !itemData.Finished(); itemToWrite = itemData.Next() )
	{
		if( !ValidateObject( itemToWrite ) )
		{
			itemData.Remove( itemToWrite );
			continue;
		}
		if( itemToWrite->ShouldSave() )
		{
			if( itemToWrite->GetObjType() == OT_MULTI )
			{
				CMultiObj *iMulti = static_cast<CMultiObj *>(itemToWrite);
				iMulti->Save( houseDestination );
			}
			else if( itemToWrite->GetObjType() == OT_BOAT )
			{
				CBoatObj *iBoat = static_cast< CBoatObj * >(itemToWrite);
				iBoat->Save( houseDestination );
			}
			else
				itemToWrite->Save( writeDestination );
		}
	}
	itemData.Pop();
}

//o--------------------------------------------------------------------------o
//|	Function		-	CDataList< CItem * > * GetItemList( void )
//|	Date			-	Unknown
//|	Programmer		-	giwo
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Returns the Item DataList for iteration
//o--------------------------------------------------------------------------o
CDataList< CItem * > * CMapRegion::GetItemList( void )
{
	return &itemData;
}

//o--------------------------------------------------------------------------o
//|	Function		-	CDataList< CChar * > * GetCharList( void )
//|	Date			-	Unknown
//|	Programmer		-	giwo
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Returns the Character DataList for iteration
//o--------------------------------------------------------------------------o
CDataList< CChar * > * CMapRegion::GetCharList( void )
{
	return &charData;
}

//o--------------------------------------------------------------------------o
//|	Function		-	CMapRegion * GetMapRegion( SI16 xOffset, SI16 yOffset )
//|	Date			-	9/13/2005
//|	Programmer		-	giwo
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Returns the MapRegion based on its x and y offsets
//o--------------------------------------------------------------------------o
CMapRegion * CMapWorld::GetMapRegion( SI16 xOffset, SI16 yOffset )
{
	CMapRegion *mRegion = NULL;
	if( xOffset >= 0 && xOffset < upperArrayX && yOffset >= 0 && yOffset < upperArrayY )
		mRegion = &mapRegions[xOffset][yOffset];
	return mRegion;
}

//o--------------------------------------------------------------------------o
//|	Function		-	SI16 CMapWorld::UpperX()
//|	Date			-	9/13/2005
//|	Programmer		-	giwo
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Gets/Sets the upper X boundry of the array
//o--------------------------------------------------------------------------o
SI16 CMapWorld::GetUpperX( void ) const
{
	return upperArrayX;
}
void CMapWorld::SetUpperX( SI16 newVal )
{
	upperArrayX = newVal;
}

//o--------------------------------------------------------------------------o
//|	Function		-	SI16 CMapWorld::UpperY()
//|	Date			-	9/13/2005
//|	Programmer		-	giwo
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Gets/Sets the upper Y boundry of the array
//o--------------------------------------------------------------------------o
SI16 CMapWorld::GetUpperY( void ) const
{
	return upperArrayY;
}
void CMapWorld::SetUpperY( SI16 newVal )
{
	upperArrayY = newVal;
}

//o--------------------------------------------------------------------------o
//|	Function		-	CMapHandler Constructor/Destructor
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Initializes / Clears the MapWorld data
//o--------------------------------------------------------------------------o
CMapHandler::CMapHandler()
{
	mapWorlds.resize( 0 );

	for( UI08 i = 0; i < cwmWorldState->ServerData()->ServerMapCount(); ++i )
	{
		CMapWorld *mWorld = new CMapWorld;

		MapData_st& mMap = Map->GetMapData( i );
		mWorld->SetUpperX( mMap.xBlock / MapColSize );
		mWorld->SetUpperY( mMap.yBlock / MapRowSize );
		mapWorlds.push_back( mWorld );
	}
}
CMapHandler::~CMapHandler()
{
	for( std::vector< CMapWorld * >::iterator mIter = mapWorlds.begin(); mIter != mapWorlds.end(); ++mIter )
	{
		delete (*mIter);
		(*mIter) = NULL;
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool Add( CBaseObject *toAdd )
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Adds specified object to the MapRegion
//o--------------------------------------------------------------------------o
bool CMapHandler::Add( CBaseObject *toAdd )
{
	if( !ValidateObject( toAdd ) )
		return false;
	if( toAdd->CanBeObjType( OT_CHAR ) )
		return AddChar( static_cast< CChar *>(toAdd) );
	else
		return AddItem( (CItem *)(toAdd) );
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool Remove( CBaseObject *toRemove )
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Removes specified object to the MapRegion
//o--------------------------------------------------------------------------o
bool CMapHandler::Remove( CBaseObject *toRemove )
{
	if( !ValidateObject( toRemove ) )
		return false;
	if( toRemove->CanBeObjType( OT_CHAR ) )
		return RemoveChar( static_cast< CChar *>(toRemove) );
	else
		return RemoveItem( (CItem *)(toRemove) );
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool AddItem( CItem *nItem )
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Adds nItem to the proper SubRegion
//o--------------------------------------------------------------------------o
bool CMapHandler::AddItem( CItem *nItem )
{
	if( !ValidateObject( nItem ) )
		return false;
	CMapRegion *cell = GetMapRegion( nItem );
	if( cell == &overFlow )
	{
		overFlow.GetItemList()->Add( nItem );
		return false;
	}
	return cell->GetItemList()->Add( nItem );
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool RemoveItem( CItem *nItem )
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Removes nItem from it's CURRENT SubRegion
//|					-	Do this before adjusting the location
//o--------------------------------------------------------------------------o
bool CMapHandler::RemoveItem( CItem *nItem )
{
	if( !ValidateObject( nItem ) )
		return false;
	CMapRegion *cell = GetMapRegion( nItem );
	if( cell == &overFlow )
	{
		overFlow.GetItemList()->Remove( nItem );
		return false;
	}
	return cell->GetItemList()->Remove( nItem );
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool AddChar( CChar *toAdd )
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Adds toAdd to the proper SubRegion
//o--------------------------------------------------------------------------o
bool CMapHandler::AddChar( CChar *toAdd )
{
	if( !ValidateObject( toAdd ) )
		return false;
	CMapRegion *cell = GetMapRegion( toAdd );
	if( cell == &overFlow )
	{
		overFlow.GetCharList()->Add( toAdd );
		return false;
	}
	return cell->GetCharList()->Add( toAdd );
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool RemoveChar( CChar *toRemove )
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Removes toRemove from it's CURRENT SubRegion
//|					-	Do this before adjusting the location
//o--------------------------------------------------------------------------o
bool CMapHandler::RemoveChar( CChar *toRemove )
{
	if( !ValidateObject( toRemove ) )
		return false;
	CMapRegion *cell = GetMapRegion( toRemove );
	if( cell == &overFlow )
	{
		overFlow.GetCharList()->Remove( toRemove );
		return false;
	}
	return cell->GetCharList()->Remove( toRemove );
}

//o--------------------------------------------------------------------------o
//|	Function		-	SI16 GetGridX( SI16 x )
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Find the Column of SubRegion we want based on location
//o--------------------------------------------------------------------------o
SI16 CMapHandler::GetGridX( SI16 x )
{
	return static_cast<SI16>(x / MapColSize);
}

//o--------------------------------------------------------------------------o
//|	Function		-	SI16 GetGridY( SI16 y )
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Finds the Row of SubRegion we want based on location
//o--------------------------------------------------------------------------o
SI16 CMapHandler::GetGridY( SI16 y )
{
	return static_cast<SI16>(y / MapRowSize);
}

//o--------------------------------------------------------------------------o
//|	Function		-	CMapRegion *GetMapRegion( SI16 xOffset, SI16 yOffset, UI08 worldNumber )
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Returns the MapRegion based on the offsets
//o--------------------------------------------------------------------------o
CMapRegion *CMapHandler::GetMapRegion( SI16 xOffset, SI16 yOffset, UI08 worldNumber )
{
	CMapRegion * mRegion = mapWorlds[worldNumber]->GetMapRegion( xOffset, yOffset );
	if( mRegion == NULL )
		mRegion = &overFlow;

	return mRegion;
}

//o--------------------------------------------------------------------------o
//|	Function		-	CMapRegion *GetMapRegion( SI16 x, SI16 y )
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Returns the subregion that x,y is in
//o--------------------------------------------------------------------------o
CMapRegion *CMapHandler::GetMapRegion( CBaseObject *mObj )
{
	return GetMapRegion( GetGridX( mObj->GetX() ), GetGridY( mObj->GetY() ), mObj->WorldNumber() );
}

//o--------------------------------------------------------------------------o
//|	Function		-	CMapWorld *	GetMapWorld( UI08 worldNum )
//|	Date			-	9/13/2005
//|	Programmer		-	giwo
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Returns the MapWorld object associated with the worldNumber
//o--------------------------------------------------------------------------o
CMapWorld *	CMapHandler::GetMapWorld( UI08 worldNum )
{
	CMapWorld *mWorld = NULL;
	if( worldNum > mapWorlds.size() )
		mWorld = mapWorlds[worldNum];
	return mWorld;
}

//o--------------------------------------------------------------------------o
//|	Function		-	REGIONLIST PopulateList( CBaseObject *mObj )
//|	Date			-	Unknown
//|	Programmer		-	giwo
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Creates a list of nearby MapRegions based on the object provided
//o--------------------------------------------------------------------------o
REGIONLIST CMapHandler::PopulateList( CBaseObject *mObj )
{
	return PopulateList( mObj->GetX(), mObj->GetY(), mObj->WorldNumber() );
}

//o--------------------------------------------------------------------------o
//|	Function		-	REGIONLIST PopulateList( SI16 x, SI16 y, UI08 worldNumber )
//|	Date			-	Unknown
//|	Programmer		-	giwo
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Creates a list of nearby MapRegions based on the coordinates provided
//o--------------------------------------------------------------------------o
REGIONLIST CMapHandler::PopulateList( SI16 x, SI16 y, UI08 worldNumber )
{
	REGIONLIST nearbyRegions;
	bool isOnList	= false;
	const SI16 xOffset	= MapRegion->GetGridX( x );
	const SI16 yOffset	= MapRegion->GetGridY( y );
	for( SI08 counter1 = -1; counter1 <= 1; ++counter1 )
	{
		for( SI08 counter2 = -1; counter2 <= 1; ++counter2 )
		{
			isOnList			= false;
			CMapRegion *MapArea	= GetMapRegion( xOffset + counter1, yOffset + counter2, worldNumber );
			if( MapArea == NULL )
				continue;
			nearbyRegions.push_back( MapArea );
		}
	}
	return nearbyRegions;
}

//o--------------------------------------------------------------------------o
//|	Function		-	Save()
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Saves out the MapRegions
//o--------------------------------------------------------------------------o
void CMapHandler::Save( void )
{
	const SI16 AreaX				= UpperX / 8;	// we're storing 8x8 grid arrays together
	const SI16 AreaY				= UpperY / 8;
	std::ofstream writeDestination, houseDestination;
	const int onePercent			= (int)((float)(AreaX*AreaY*8*8*cwmWorldState->ServerData()->ServerMapCount())/100.0f);
	const char blockDiscriminator[] = "\n\n---REGION---\n\n";
	UI32 count						= 0;
	const UI32 s_t						= getclock();

	Console << "Saving Character and Item Map Region data...   ";
	Console.TurnYellow();
	Console << "0%";

	std::string basePath = cwmWorldState->ServerData()->Directory( CSDDP_SHARED );
	std::string filename = basePath + "house.wsc";
	
	houseDestination.open( filename.c_str() );

	for( SI16 counter1 = 0; counter1 < AreaX; ++counter1 )	// move left->right
	{
		SI32 baseX = counter1 * 8;
		for( SI16 counter2 = 0; counter2 < AreaY; ++counter2 )	// move up->down
		{
			SI32 baseY	= counter2 * 8;								// calculate x grid offset
			filename	= basePath + UString::number( counter1 ) + "." + UString::number( counter2 ) + ".wsc";	// let's name our file
			writeDestination.open( filename.c_str() );

			if( !writeDestination ) 
			{
				Console.Error( 1, "Failed to open %s for writing", filename.c_str() );
				continue;
			}

			for( UI08 xCnt = 0; xCnt < 8; ++xCnt )					// walk through each part of the 8x8 grid, left->right
			{
				for( UI08 yCnt = 0; yCnt < 8; ++yCnt )				// walk the row
				{
					for( std::vector< CMapWorld * >::iterator mIter = mapWorlds.begin(); mIter != mapWorlds.end(); ++mIter )
					{
						++count;
						if( count%onePercent == 0 )
						{
							if( count/onePercent <= 10 )
								Console << "\b\b" << (UI32)(count/onePercent) << "%";
							else if( count/onePercent <= 100 )
								Console << "\b\b\b" << (UI32)(count/onePercent) << "%";
						}
						CMapRegion * mRegion = (*mIter)->GetMapRegion( (baseX+xCnt), (baseY+yCnt) );
						if( mRegion != NULL )
							mRegion->SaveToDisk( writeDestination, houseDestination );

						writeDestination << blockDiscriminator;
					}
				}
			}
			writeDestination.close();
		}
	}
	houseDestination.close();

	filename = basePath + "overflow.wsc";
	writeDestination.open( filename.c_str() );

	if( writeDestination.is_open() )
	{
		overFlow.SaveToDisk( writeDestination, writeDestination );
		writeDestination.close();
	}
	else
	{
		Console.Error( 1, "Failed to open %s for writing", filename.c_str() );
		return;
	}
	
	Console << "\b\b\b\b";
	Console.PrintDone();

	const UI32 e_t = getclock();
	Console.Print( "World saved in %.02fsec\n", ((float)(e_t-s_t))/1000.0f );
}

bool PostLoadFunctor( CBaseObject *a, UI32 &b, void *extraData )
{
	if( ValidateObject( a ) )
	{
		if( !a->isFree() )
			a->PostLoadProcessing();
	}
	return true;
}

//o--------------------------------------------------------------------------o
//|	Function		-	Load()
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Loads in the MapRegions
//o--------------------------------------------------------------------------o
void CMapHandler::Load( void )
{
	const SI16 AreaX		= UpperX / 8;	// we're storing 8x8 grid arrays together
	const SI16 AreaY		= UpperY / 8;
	const int onePercent	= (int)((float)(AreaX*AreaY)/100.0f);
	UI32 count				= 0;
	std::ifstream readDestination;

	Console.TurnYellow();
	Console << "0%";
	UI32 s_t					= getclock();
	std::string basePath	= cwmWorldState->ServerData()->Directory( CSDDP_SHARED );
	std::string filename;
	for( SI16 counter1 = 0; counter1 < AreaX; ++counter1 )	// move left->right
	{
		for( SI16 counter2 = 0; counter2 < AreaY; ++counter2 )	// move up->down
		{
			filename	= basePath + UString::number( counter1 ) + "." + UString::number( counter2 ) + ".wsc";	// let's name our file
			readDestination.open( filename.c_str() );					// let's open it 

			readDestination.seekg( 0, std::ios::beg );

			if( readDestination.eof() || readDestination.fail() )
			{
				readDestination.close();
				readDestination.clear();
				continue;
			}

			++count;
			if( count%onePercent == 0 )
			{
				if( count/onePercent <= 10 )
					Console << "\b\b" << (UI32)(count/onePercent) << "%";
				else if( count/onePercent <= 100 )
					Console << "\b\b\b" << (UI32)(count/onePercent) << "%";
			}
			LoadFromDisk( readDestination );

			readDestination.close();
			readDestination.clear();
		}
	}

	Console.TurnNormal();
	Console << "\b\b\b";
	Console.PrintDone();

	filename = basePath + "overflow.wsc";
	std::ifstream flowDestination( filename.c_str() );
	LoadFromDisk( flowDestination );
	flowDestination.close();

	filename = basePath + "house.wsc";
	std::ifstream houseDestination( filename.c_str() );
	LoadFromDisk( houseDestination );

	UI32 b		= 0;
	ObjectFactory::getSingleton().IterateOver( OT_MULTI, b, NULL, &PostLoadFunctor );
	ObjectFactory::getSingleton().IterateOver( OT_CHAR, b, NULL, &PostLoadFunctor );
	ObjectFactory::getSingleton().IterateOver( OT_ITEM, b, NULL, &PostLoadFunctor );
	houseDestination.close();

	UI32 e_t = getclock();
	Console.Print( "ASCII world loaded in %.02fsec\n", ((float)(e_t-s_t))/1000.0f );
}

//o--------------------------------------------------------------------------o
//|	Function		-	LoadFromDisk( std::ifstream& readDestination )
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Loads in objects from specified file
//o--------------------------------------------------------------------------o
void CMapHandler::LoadFromDisk( std::ifstream& readDestination )
{
	char line[1024];
	while( !readDestination.eof() && !readDestination.fail() )
	{
		readDestination.getline( line, 1024 );
		UString sLine( line );
		sLine = sLine.removeComment().stripWhiteSpace();
		if( sLine.substr( 0, 1 ) == "[" )	// in a section
		{
			sLine = sLine.substr( 1, sLine.size() - 2 );
			sLine = sLine.upper().stripWhiteSpace();
			if( sLine == "CHARACTER" )
				LoadChar( readDestination );
			else if( sLine == "ITEM" )
				LoadItem( readDestination );
			else if( sLine == "HOUSE" )
				LoadMulti( readDestination );
			else if( sLine == "BOAT" )
				LoadBoat( readDestination );
			else if( sLine == "SPAWNITEM" )
				LoadSpawnItem( readDestination );
		}
		else if( sLine == "---REGION---" )	// end of region
			continue;
	}
}

}
