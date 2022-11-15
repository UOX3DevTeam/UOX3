#include "uox3.h"
#include "classes.h"
#include "regions.h"
#include "StringUtility.hpp"
#include "ObjectFactory.h"
#include <filesystem>

#define DEBUG_REGIONS		0

CMapHandler *MapRegion;
//o------------------------------------------------------------------------------------------------o
//|	Function	-	FileSize()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the filesize of a given file
//o------------------------------------------------------------------------------------------------o
SI32 FileSize( std::string filename )
{
	SI32 retVal = 0;

	try
	{
		retVal = static_cast<SI32>( std::filesystem::file_size( filename ));
	}
	catch( ... )
	{
		retVal = 0;
	}

	return retVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	LoadChar()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a new character object based on data loaded from worldfiles
//o------------------------------------------------------------------------------------------------o
void LoadChar( std::ifstream& readDestination )
{
	CChar *x = static_cast<CChar *>( ObjectFactory::GetSingleton().CreateBlankObject( OT_CHAR ));
	if( x == nullptr )
		return;

	if( !x->Load( readDestination ))
	{
		x->Cleanup();
		ObjectFactory::GetSingleton().DestroyObject( x );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	LoadItem()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a new item object based on data loaded from worldfiles
//o------------------------------------------------------------------------------------------------o
void LoadItem( std::ifstream& readDestination )
{
	CItem *x = static_cast<CItem *>( ObjectFactory::GetSingleton().CreateBlankObject( OT_ITEM ));
	if( x == nullptr )
		return;

	if( !x->Load( readDestination ))
	{
		x->Cleanup();
		ObjectFactory::GetSingleton().DestroyObject( x );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	LoadMulti()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a new multi object, like a house, based on data loaded from worldfiles
//o------------------------------------------------------------------------------------------------o
void LoadMulti( std::ifstream& readDestination )
{
	CMultiObj *ourHouse = static_cast<CMultiObj *>( ObjectFactory::GetSingleton().CreateBlankObject( OT_MULTI ));
	if( !ourHouse->Load( readDestination ))	// if no load, DELETE
	{
		ourHouse->Cleanup();
		ObjectFactory::GetSingleton().DestroyObject( ourHouse );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	LoadBoat()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a new boat object based on data loaded from worldfiles
//o------------------------------------------------------------------------------------------------o
void LoadBoat( std::ifstream& readDestination )
{
	CBoatObj *ourBoat = static_cast<CBoatObj *>( ObjectFactory::GetSingleton().CreateBlankObject( OT_BOAT ));
	if( !ourBoat->Load( readDestination )) // if no load, DELETE
	{
		ourBoat->Cleanup();
		ObjectFactory::GetSingleton().DestroyObject( ourBoat );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	LoadSpawnItem()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a new spawn object based on data loaded from worldfiles
//o------------------------------------------------------------------------------------------------o
void LoadSpawnItem( std::ifstream& readDestination )
{
	CSpawnItem *ourSpawner = static_cast<CSpawnItem *>( ObjectFactory::GetSingleton().CreateBlankObject( OT_SPAWNER ));
	if( !ourSpawner->Load( readDestination )) // if no load, DELETE
	{
		ourSpawner->Cleanup();
		ObjectFactory::GetSingleton().DestroyObject( ourSpawner );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Functio		-	CMapRegion::SaveToDisk()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Save all items and characters inside a subregion reworked SaveChar from WorldMain
//|					to deal with pointer based stuff in region rather than index based stuff in array
//|					Also saves out all data regardless (in preparation for a simple binary save)
//o------------------------------------------------------------------------------------------------o
void CMapRegion::SaveToDisk( std::ofstream& writeDestination )
{
	std::vector<CChar *> removeChar;
	for( const auto &charToWrite : charData.collection() )
	{
		if( !ValidateObject( charToWrite ))
		{
			removeChar.push_back( charToWrite );
		}
		else
		{
#if defined( _MSC_VER )
#pragma todo( "PlayerHTML Dumping needs to be reimplemented" )
#endif
			if( charToWrite->ShouldSave() )
			{
				charToWrite->Save( writeDestination );
			}
		}
	}
	std::for_each( removeChar.begin(), removeChar.end(), [this]( CChar *character )
	{
		charData.Remove( character );
	});

	std::vector<CItem *> removeItem;
	for( const auto &itemToWrite : itemData.collection() )
	{
		if( !ValidateObject( itemToWrite ))
		{
			removeItem.push_back( itemToWrite );
		}
		else
		{
			if( itemToWrite->ShouldSave() )
			{
				if( itemToWrite->GetObjType() == OT_MULTI )
				{
					CMultiObj *iMulti = static_cast<CMultiObj *>( itemToWrite );
					iMulti->Save( writeDestination );
				}
				else if( itemToWrite->GetObjType() == OT_BOAT )
				{
					CBoatObj *iBoat = static_cast<CBoatObj *>( itemToWrite );
					iBoat->Save( writeDestination );
				}
				else
				{
					itemToWrite->Save( writeDestination );
				}
			}
		}
	}
	std::for_each( removeItem.begin(), removeItem.end(), [this]( CItem *item )
	{
		itemData.Remove( item );
	});
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapRegion::GetItemList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the Item DataList for iteration
//o------------------------------------------------------------------------------------------------o
GenericList<CItem *> * CMapRegion::GetItemList( void )
{
	return &itemData;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapRegion::GetCharList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the Character DataList for iteration
//o------------------------------------------------------------------------------------------------o
GenericList<CChar *> * CMapRegion::GetCharList( void )
{
	return &charData;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapRegion::GetRegionSerialList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the list of baseobject serials for iteration
//o------------------------------------------------------------------------------------------------o
RegionSerialList* CMapRegion::GetRegionSerialList()
{
	return &regionSerialData;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapRegion::HasRegionChanged()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a flag that says whether the region has seen any updates since last save
//o------------------------------------------------------------------------------------------------o
bool CMapRegion::HasRegionChanged( void )
{
	return hasRegionChanged;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapRegion::HasRegionChanged()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets a flag that says whether the region has seen any updates since last save
//o------------------------------------------------------------------------------------------------o
void CMapRegion::HasRegionChanged( bool newVal )
{
	hasRegionChanged = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapWorld Constructor/Destructor
//|	Date		-	17 October, 2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Initializes & Clears the MapWorld data
//o------------------------------------------------------------------------------------------------o
CMapWorld::CMapWorld() : upperArrayX( 0 ), upperArrayY( 0 ), resourceX( 0 ), resourceY( 0 )
{
	mapResources.resize( 1 );	// ALWAYS initialize at least one resource region.
	mapRegions.resize( 0 );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapWorld::CMapWorld()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Initializes resource regions for the given world
//o------------------------------------------------------------------------------------------------o
CMapWorld::CMapWorld( UI08 worldNum )
{
	auto [mapWidth, mapHeight] = Map->SizeOfMap( worldNum );
	upperArrayX			= static_cast<SI16>( mapWidth / MapColSize );
	upperArrayY			= static_cast<SI16>( mapHeight/ MapRowSize );
	resourceX			= static_cast<UI16>( mapWidth / cwmWorldState->ServerData()->ResourceAreaSize() );
	resourceY			= static_cast<UI16>( mapHeight / cwmWorldState->ServerData()->ResourceAreaSize() );

	size_t resourceSize = ( static_cast<size_t>( resourceX ) * static_cast<size_t>( resourceY ));
	if( resourceSize < 1 )	// ALWAYS initialize at least one resource region.
	{
		resourceSize = 1;
	}
	mapResources.resize( resourceSize );

	mapRegions.resize( static_cast<size_t>( upperArrayX ) * static_cast<size_t>( upperArrayY ));
}

CMapWorld::~CMapWorld()
{
	mapResources.resize( 0 );
	mapRegions.resize( 0 );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapWorld::GetMapRegion()
//|	Date		-	9/13/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the MapRegion based on its x and y offsets
//o------------------------------------------------------------------------------------------------o
CMapRegion * CMapWorld::GetMapRegion( SI16 xOffset, SI16 yOffset )
{
	CMapRegion *mRegion			= nullptr;
	const size_t regionIndex	= ( static_cast<size_t>( xOffset ) * static_cast<size_t>( upperArrayY ) + static_cast<size_t>( yOffset ));

	if( xOffset >= 0 && xOffset < upperArrayX && yOffset >= 0 && yOffset < upperArrayY )
	{
		if( regionIndex < mapRegions.size() )
		{
			mRegion = &mapRegions[regionIndex];
		}
	}

	return mRegion;
}

std::vector<CMapRegion> * CMapWorld::GetMapRegions()
{
	return &mapRegions;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapWorld::GetResource()
//|	Date		-	9/17/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the Resource region x and y is.
//o------------------------------------------------------------------------------------------------o
MapResource_st& CMapWorld::GetResource( SI16 x, SI16 y )
{
	const UI16 gridX = ( x / cwmWorldState->ServerData()->ResourceAreaSize() );
	const UI16 gridY = ( y / cwmWorldState->ServerData()->ResourceAreaSize() );

	size_t resIndex = (( static_cast<size_t>( gridX ) * static_cast<size_t>( resourceY )) + static_cast<size_t>( gridY ));

	if( gridX >= resourceX || gridY >= resourceY || resIndex > mapResources.size() )
	{
		resIndex = 0;
	}

	return mapResources[resIndex];
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapWorld::SaveResources()
//|	Date		-	9/17/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Saves the Resource data to disk
//o------------------------------------------------------------------------------------------------o
void CMapWorld::SaveResources( UI08 worldNum )
{
	char wBuffer[2];
	const std::string resourceFile	= cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "resource[" + std::to_string( worldNum ) + "].bin";
	std::ofstream toWrite( resourceFile.c_str(), std::ios::out | std::ios::trunc | std::ios::binary );

	if( toWrite )
	{
		for( std::vector<MapResource_st>::const_iterator mIter = mapResources.begin(); mIter != mapResources.end(); ++mIter )
		{
			wBuffer[0] = static_cast<SI08>(( *mIter ).oreAmt >> 8 );
			wBuffer[1] = static_cast<SI08>(( *mIter ).oreAmt % 256 );
			toWrite.write(( const char * )&wBuffer, 2 );

			wBuffer[0] = static_cast<SI08>(( *mIter ).logAmt >> 8 );
			wBuffer[1] = static_cast<SI08>(( *mIter ).logAmt % 256 );
			toWrite.write(( const char * )&wBuffer, 2 );

			wBuffer[0] = static_cast<SI08>(( *mIter ).fishAmt >> 8 );
			wBuffer[1] = static_cast<SI08>(( *mIter ).fishAmt % 256 );
			toWrite.write(( const char * )&wBuffer, 2 );
		}
		toWrite.close();
	}
	else // Can't save resources
	{
		Console.Error( "Failed to open resource.bin for writing" );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapWorld::LoadResources()
//|	Date		-	9/17/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads the Resource data from the disk
//o------------------------------------------------------------------------------------------------o
void CMapWorld::LoadResources( UI08 worldNum )
{
	const SI16 resOre				= cwmWorldState->ServerData()->ResOre();
	const SI16 resLog				= cwmWorldState->ServerData()->ResLogs();
	const SI16 resFish				= cwmWorldState->ServerData()->ResFish();
	const UI32 oreTime				= BuildTimeValue( static_cast<R32>( cwmWorldState->ServerData()->ResOreTime() ));
	const UI32 logTime				= BuildTimeValue( static_cast<R32>( cwmWorldState->ServerData()->ResLogTime() ));
	const UI32 fishTime				= BuildTimeValue( static_cast<R32>( cwmWorldState->ServerData()->ResFishTime() ));
	const std::string resourceFile	= cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + std::string("resource[") + oldstrutil::number( worldNum ) + std::string("].bin");

	char rBuffer[2];
	std::ifstream toRead ( resourceFile.c_str(), std::ios::in | std::ios::binary );

	bool fileExists	= ( toRead.is_open() );

	if( fileExists )
	{
		toRead.seekg( 0, std::ios::beg );
	}

	for( std::vector<MapResource_st>::iterator mIter = mapResources.begin(); mIter != mapResources.end(); ++mIter )
	{
		if( fileExists )
		{
			toRead.read( rBuffer, 2 );
			( *mIter ).oreAmt = (( rBuffer[0] << 8 ) + rBuffer[1] );

			toRead.read( rBuffer, 2 );
			( *mIter ).logAmt = (( rBuffer[0] << 8 ) + rBuffer[1] );

			toRead.read( rBuffer, 2 );
			( *mIter ).fishAmt = (( rBuffer[0] << 8 ) + rBuffer[1] );

			fileExists = toRead.eof();
		}
		else
		{
			( *mIter ).oreAmt  = resOre;
			( *mIter ).logAmt  = resLog;
			( *mIter ).fishAmt  = resFish;
		}
		// No need to preserve time.  Do a refresh automatically
		( *mIter ).oreTime = oreTime;
		( *mIter ).logTime = logTime;
		( *mIter ).fishTime = fishTime;
	}
	if( fileExists )
	{
		toRead.close();
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::Startup()
//|	Date		-	23 July, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Fills and clears the mapWorlds vector.
//o------------------------------------------------------------------------------------------------o
auto CMapHandler::Startup() -> void
{
	UI08 numWorlds = Map->MapCount();

	mapWorlds.reserve( numWorlds );
	for( UI08 i = 0; i < numWorlds; ++i )
	{
		mapWorlds.push_back( new CMapWorld( i ));
	}
}
CMapHandler::~CMapHandler()
{
	for( WORLDLIST_ITERATOR mIter = mapWorlds.begin(); mIter != mapWorlds.end(); ++mIter )
	{
		if(( *mIter ) != nullptr )
		{
			delete ( *mIter );
			( *mIter ) = nullptr;
		}
	}
	mapWorlds.resize( 0 );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::ChangeRegion()
//|	Date		-	February 1, 2006
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Changes an items region ONLY if he has changed regions.
//o------------------------------------------------------------------------------------------------o
bool CMapHandler::ChangeRegion( CItem *nItem, SI16 x, SI16 y, UI08 worldNum )
{
	if( !ValidateObject( nItem ))
		return false;

	CMapRegion *curCell = GetMapRegion( nItem );
	CMapRegion *newCell = GetMapRegion( GetGridX( x ), GetGridY( y ), worldNum );

	if( curCell != newCell )
	{
		if( !curCell->GetRegionSerialList()->Remove( nItem->GetSerial() ) || !curCell->GetItemList()->Remove( nItem ))
		{
#if DEBUG_REGIONS
			Console.Warning( oldstrutil::format( "Item 0x%X does not exist in MapRegion, remove failed", nItem->GetSerial() ));
#endif
		}
		else
		{
			if( nItem->ShouldSave() )
			{
				curCell->HasRegionChanged( true );
			}
		}

		if( newCell->GetRegionSerialList()->Add( nItem->GetSerial() ))
		{
			newCell->GetItemList()->Add( nItem, false );
			if( nItem->ShouldSave() )
			{
				newCell->HasRegionChanged( true );
			}
			return true;
		}
		else
		{
#if DEBUG_REGIONS
			Console.Warning( oldstrutil::format( "Item 0x%X already exists in MapRegion, add failed", nItem->GetSerial() ));
#endif
		}
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::ChangeRegion()
//|	Date		-	February 1, 2006
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Changes a characters region ONLY if he has changed regions.
//o------------------------------------------------------------------------------------------------o
bool CMapHandler::ChangeRegion( CChar *nChar, SI16 x, SI16 y, UI08 worldNum )
{
	if( !ValidateObject( nChar ))
		return false;

	CMapRegion *curCell = GetMapRegion( nChar );
	CMapRegion *newCell = GetMapRegion( GetGridX( x ), GetGridY( y ), worldNum );

	if( curCell != newCell )
	{
		if( !curCell->GetRegionSerialList()->Remove( nChar->GetSerial() ) || !curCell->GetCharList()->Remove( nChar ))
		{
#if DEBUG_REGIONS
			Console.Warning( oldstrutil::format( "Character 0x%X does not exist in MapRegion, remove failed", nChar->GetSerial() ));
#endif
		}
		else
		{
			if( nChar->ShouldSave() )
			{
				curCell->HasRegionChanged( true );
			}
		}

		if( newCell->GetRegionSerialList()->Add( nChar->GetSerial() ))
		{
			newCell->GetCharList()->Add( nChar, false );
			if( nChar->ShouldSave() )
			{
				newCell->HasRegionChanged( true );
			}
			return true;
		}
		else
		{
#if DEBUG_REGIONS
			Console.Warning( oldstrutil::format( "Character 0x%X already exists in MapRegion, add failed", nChar->GetSerial() ));
#endif
		}
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::AddItem()
//|	Date		-	23 July, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds nItem to the proper SubRegion
//o------------------------------------------------------------------------------------------------o
bool CMapHandler::AddItem( CItem *nItem )
{
	if( !ValidateObject( nItem ))
		return false;

	CMapRegion *cell = GetMapRegion( nItem );
	if( cell->GetRegionSerialList()->Add( nItem->GetSerial() ))
	{
		cell->GetItemList()->Add( nItem, false );
		if( nItem->ShouldSave() )
		{
			cell->HasRegionChanged( true );
		}
		return true;
	}
	else
	{
#if DEBUG_REGIONS
		Console.Warning( oldstrutil::format( "Item 0x%X already exists in MapRegion, add failed", nItem->GetSerial() ));
#endif
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::RemoveItem()
//|	Date		-	23 July, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes nItem from it's CURRENT SubRegion. Do this before adjusting the location
//o------------------------------------------------------------------------------------------------o
bool CMapHandler::RemoveItem( CItem *nItem )
{
	if( !ValidateObject( nItem ))
		return false;

	CMapRegion *cell = GetMapRegion( nItem );
	if( cell->GetRegionSerialList()->Remove( nItem->GetSerial() ))
	{
		cell->GetItemList()->Remove( nItem );
		if( nItem->ShouldSave() )
		{
			cell->HasRegionChanged( true );
		}
	}
	else
	{
#if DEBUG_REGIONS
		Console.Warning( oldstrutil::format( "Item 0x%X does not exist in MapRegion, remove failed", nItem->GetSerial() ));
#endif
		return false;
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::AddChar()
//|	Date		-	23 July, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds toAdd to the proper SubRegion
//o------------------------------------------------------------------------------------------------o
bool CMapHandler::AddChar( CChar *toAdd )
{
	if( !ValidateObject( toAdd ))
		return false;

	CMapRegion *cell = GetMapRegion( toAdd );
	if( cell->GetRegionSerialList()->Add( toAdd->GetSerial() ))
	{
		cell->GetCharList()->Add( toAdd, false );
		if( toAdd->ShouldSave() )
		{
			cell->HasRegionChanged( true );
		}
		return true;
	}
	else
	{
#if DEBUG_REGIONS
		Console.Warning( oldstrutil::format( "Character 0x%X already exists in MapRegion, add failed", toAdd->GetSerial() ));
#endif
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::RemoveChar()
//|	Date		-	23 July, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes toRemove from it's CURRENT SubRegion. Do this before adjusting the location
//o------------------------------------------------------------------------------------------------o
bool CMapHandler::RemoveChar( CChar *toRemove )
{
	if( !ValidateObject( toRemove ))
		return false;

	CMapRegion *cell = GetMapRegion( toRemove );
	if( cell->GetRegionSerialList()->Remove( toRemove->GetSerial() ))
	{
		cell->GetCharList()->Remove( toRemove );
		if( toRemove->ShouldSave() )
		{
			cell->HasRegionChanged( true );
		}
	}
	else
	{
#if DEBUG_REGIONS
		Console.Warning( oldstrutil::format( "Character 0x%X does not exist in MapRegion, remove failed", toRemove->GetSerial() ));
#endif
		return false;
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::GetGridX()
//|	Date		-	23 July, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Find the Column of SubRegion we want based on location
//o------------------------------------------------------------------------------------------------o
SI16 CMapHandler::GetGridX( SI16 x )
{
	return static_cast<SI16>( x / MapColSize );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::GetGridY()
//|	Date		-	23 July, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Finds the Row of SubRegion we want based on location
//o------------------------------------------------------------------------------------------------o
SI16 CMapHandler::GetGridY( SI16 y )
{
	return static_cast<SI16>( y / MapRowSize );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::GetMapRegion()
//|	Date		-	23 July, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the MapRegion based on the offsets
//o------------------------------------------------------------------------------------------------o
CMapRegion *CMapHandler::GetMapRegion( SI16 xOffset, SI16 yOffset, UI08 worldNumber )
{
	CMapRegion * mRegion = mapWorlds[worldNumber]->GetMapRegion( xOffset, yOffset );
	if( mRegion == nullptr )
	{
		mRegion = &overFlow;
	}

	return mRegion;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::GetMapRegion()
//|	Date		-	23 July, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the subregion that x,y is in
//o------------------------------------------------------------------------------------------------o
CMapRegion *CMapHandler::GetMapRegion( CBaseObject *mObj )
{
	return GetMapRegion( GetGridX( mObj->GetX() ), GetGridY( mObj->GetY() ), mObj->WorldNumber() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::GetMapWorld()
//|	Date		-	9/13/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the MapWorld object associated with the worldNumber
//o------------------------------------------------------------------------------------------------o
CMapWorld *	CMapHandler::GetMapWorld( UI08 worldNum )
{
	CMapWorld *mWorld = nullptr;
	if( worldNum > mapWorlds.size() )
	{
		mWorld = mapWorlds[worldNum];
	}
	return mWorld;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::GetMapWorld()
//|	Date		-	9/13/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the MapWorld object associated with the worldNumber
//o------------------------------------------------------------------------------------------------o
MapResource_st * CMapHandler::GetResource( SI16 x, SI16 y, UI08 worldNum )
{
	MapResource_st *resData = nullptr;
	if( worldNum < mapWorlds.size() )
	{
		resData = &mapWorlds[worldNum]->GetResource( x, y );
	}
	return resData;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::PopulateList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a list of nearby MapRegions based on the object provided
//o------------------------------------------------------------------------------------------------o
auto CMapHandler::PopulateList( CBaseObject *mObj ) -> std::vector<CMapRegion *>
{
	return PopulateList( mObj->GetX(), mObj->GetY(), mObj->WorldNumber() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::PopulateList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a list of nearby MapRegions based on the coordinates provided
//o------------------------------------------------------------------------------------------------o
auto CMapHandler::PopulateList( SI16 x, SI16 y, UI08 worldNumber ) -> std::vector<CMapRegion *>
{
	std::vector<CMapRegion *> nearbyRegions;
	const SI16 xOffset	= MapRegion->GetGridX( x );
	const SI16 yOffset	= MapRegion->GetGridY( y );
	for( SI08 counter1 = -1; counter1 <= 1; ++counter1 )
	{
		for( SI08 counter2 = -1; counter2 <= 1; ++counter2 )
		{
			CMapRegion *MapArea	= GetMapRegion( xOffset + counter1, yOffset + counter2, worldNumber );
			if( MapArea == nullptr )
				continue;

			nearbyRegions.push_back( MapArea );
		}
	}
	return nearbyRegions;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::Save()
//|	Date		-	23 July, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Saves out data from MapRegions to worldfiles
//o------------------------------------------------------------------------------------------------o
void CMapHandler::Save( void )
{
	const SI16 AreaX				= UpperX / 8;	// we're storing 8x8 grid arrays together
	const SI16 AreaY				= UpperY / 8;
	std::ofstream writeDestination, houseDestination;
	SI32 onePercent = 0;
	UI08 i = 0;
	for( i = 0; i < Map->MapCount(); ++i )
	{
		auto [mapWidth, mapHeight] = Map->SizeOfMap( i );
		onePercent += static_cast<SI32>(( mapWidth / MapColSize ) * ( mapHeight / MapRowSize ));
	}
	onePercent /= 100.0f;
	const size_t bufferSize = 1024 * 1024;
	char* buffer = ( char* )malloc( bufferSize );

	const char blockDiscriminator[] = "\n\n---REGION---\n\n";
	UI32 count						= 0;
	const UI32 s_t						= GetClock();

	Console << "Saving Character and Item Map Region data...   ";
	Console.TurnYellow();
	Console << "0%";

	std::string basePath = cwmWorldState->ServerData()->Directory( CSDDP_SHARED );

	// Legacy - deletes house.wsc on next world save, since house data is now saved in the regional
	// wsc files along with other objects, so we don't want this file loaded again on next startup
	std::filesystem::path houseFilePath = basePath + "house.wsc";
	std::filesystem::remove( houseFilePath );

	std::string filename = "";
	for( SI16 counter1 = 0; counter1 < AreaX; ++counter1 )	// move left->right
	{
		const SI32 baseX = counter1 * 8;
		for( SI16 counter2 = 0; counter2 < AreaY; ++counter2 )	// move up->down
		{
			const SI32 baseY = counter2 * 8;								// calculate x grid offset
			filename = basePath + oldstrutil::number( counter1 ) + std::string( "." ) + oldstrutil::number( counter2 ) + std::string( ".wsc" );	// let's name our file

			bool changesDetected = false;
			for( UI08 xCnt = 0; !changesDetected && xCnt < 8; ++xCnt )					// walk through each part of the 8x8 grid, left->right
			{
				for( UI08 yCnt = 0; !changesDetected && yCnt < 8; ++yCnt )				// walk the row
				{
					for( WORLDLIST_ITERATOR mIter = mapWorlds.begin(); mIter != mapWorlds.end(); ++mIter )
					{
						CMapRegion * mRegion = ( *mIter )->GetMapRegion(( baseX + xCnt ), ( baseY + yCnt ));
						if( mRegion != nullptr )
						{
							// Only save objects mapRegions marked as "changed" by objects updated in said regions
							if( mRegion->HasRegionChanged() )
							{
								changesDetected = true;
								break;
							}
						}
					}
				}
			}
			
			if( !changesDetected )
				continue;

			writeDestination.open( filename.c_str(), std::ios::binary );

			if( !writeDestination )
			{
				Console.Error( oldstrutil::format( "Failed to open %s for writing", filename.c_str() ));
				continue;
			}

			writeDestination.rdbuf()->pubsetbuf( buffer, bufferSize );

			for( UI08 xCnt = 0; xCnt < 8; ++xCnt )					// walk through each part of the 8x8 grid, left->right
			{
				for( UI08 yCnt = 0; yCnt < 8; ++yCnt )				// walk the row
				{
					for( WORLDLIST_ITERATOR mIter = mapWorlds.begin(); mIter != mapWorlds.end(); ++mIter )
					{
						++count;
						if( count%onePercent == 0 )
						{
							if( count/onePercent <= 10 )
							{
								Console << "\b\b" << std::to_string( count / onePercent ) << "%";
							}
							else if( count/onePercent <= 100 )
							{
								Console << "\b\b\b" << std::to_string( count / onePercent ) << "%";
							}
						}

						CMapRegion * mRegion = ( *mIter )->GetMapRegion(( baseX + xCnt ), ( baseY + yCnt ));
						if( mRegion != nullptr )
						{
							mRegion->SaveToDisk( writeDestination );

							// Remove "changed" flag from region, to avoid it saving again needlessly on next save
							mRegion->HasRegionChanged( false );
						}

						writeDestination << blockDiscriminator;
					}
				}
			}
			writeDestination.close();
		}
	}
	Console << "\b\b\b\b" << static_cast<UI32>( 100 ) << "%";

	filename = basePath + "overflow.wsc";
	writeDestination.open( filename.c_str() );

	if( writeDestination.is_open() )
	{
		overFlow.SaveToDisk( writeDestination );
		writeDestination.close();
	}
	else
	{
		Console.Error( oldstrutil::format( "Failed to open %s for writing", filename.c_str() ));
		return;
	}

	Console << "\b\b\b\b";
	Console.PrintDone();

	const UI32 e_t = GetClock();
	Console.Print( oldstrutil::format( "World saved in %.02fsec\n", ( static_cast<R32>( e_t - s_t )) / 1000.0f ));

	i = 0;
	for( WORLDLIST_ITERATOR wIter = mapWorlds.begin(); wIter != mapWorlds.end(); ++wIter )
	{
		( *wIter )->SaveResources( i );
		++i;
	}
}

bool PostLoadFunctor( CBaseObject *a, [[maybe_unused]] UI32 &b, [[maybe_unused]] void *extraData )
{
	if( ValidateObject( a ))
	{
		if( !a->IsFree() )
		{
			a->PostLoadProcessing();
		}
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::Load()
//|	Date		-	23 July, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads in worldfile data to MapRegions
//o------------------------------------------------------------------------------------------------o
void CMapHandler::Load( void )
{
	const SI16 AreaX		= UpperX / 8;	// we're storing 8x8 grid arrays together
	const SI16 AreaY		= UpperY / 8;
	UI32 count				= 0;
	std::ifstream readDestination;
	Console.TurnYellow();
	Console << "0%";
	UI32 s_t				= GetClock();
	std::string basePath	= cwmWorldState->ServerData()->Directory( CSDDP_SHARED );
	std::string filename;

	UI32 runningCount = 0;
	SI32 fileSizes[AreaX][AreaY];
	for( SI16 cx = 0; cx < AreaX; ++cx )
	{
		for( SI16 cy = 0; cy < AreaY; ++cy )
		{
			filename = basePath + oldstrutil::number( cx ) + std::string( "." ) + oldstrutil::number( cy ) + std::string( ".wsc" );	// let's name our file
			fileSizes[cx][cy] = FileSize( filename );
			runningCount += fileSizes[cx][cy];
		}
	}

	if( runningCount == 0 )
	{
		runningCount = 1;
	}

	SI32 runningDone = 0;
	for( SI16 counter1 = 0; counter1 < AreaX; ++counter1 )	// move left->right
	{
		for( SI16 counter2 = 0; counter2 < AreaY; ++counter2 )	// move up->down
		{
			filename = basePath + oldstrutil::number( counter1 ) + std::string( "." ) + oldstrutil::number( counter2 ) + std::string( ".wsc" );	// let's name our file
			readDestination.open( filename.c_str() );					// let's open it
			readDestination.seekg( 0, std::ios::beg );

			if( readDestination.eof() || readDestination.fail() )
			{
				readDestination.close();
				readDestination.clear();
				continue;
			}

			++count;
			LoadFromDisk( readDestination, runningDone, fileSizes[counter1][counter2], runningCount );

			runningDone		+= fileSizes[counter1][counter2];
			float tempVal	= static_cast<R32>( runningDone ) / static_cast<R32>( runningCount ) * 100.0f;
			if( tempVal <= 10 )
			{
				Console << "\b\b" << static_cast<UI32>( tempVal ) << "%";
			}
			else if( tempVal <= 100 )
			{
				Console << "\b\b\b" << static_cast<UI32>( tempVal ) << "%";
			}

			readDestination.close();
			readDestination.clear();
		}
	}

	// If runningDone is still 0, there was nothing to load! 100% it
	if( runningDone == 0 )
	{
		Console << "\b\b" << static_cast<UI32>( 100 ) << "%";
	}

	Console.TurnNormal();
	Console << "\b\b\b";
	Console.PrintDone();

	filename	= basePath + "overflow.wsc";
	std::ifstream flowDestination( filename.c_str() );
	LoadFromDisk( flowDestination, -1, -1, -1 );
	flowDestination.close();

	filename	= basePath + "house.wsc";
	std::ifstream houseDestination( filename.c_str() );
	LoadFromDisk( houseDestination, -1, -1, -1 );

	UI32 b		= 0;
	ObjectFactory::GetSingleton().IterateOver( OT_MULTI, b, nullptr, &PostLoadFunctor );
	ObjectFactory::GetSingleton().IterateOver( OT_ITEM, b, nullptr, &PostLoadFunctor );
	ObjectFactory::GetSingleton().IterateOver( OT_CHAR, b, nullptr, &PostLoadFunctor );
	houseDestination.close();

	UI32 e_t	= GetClock();
	Console.Print( oldstrutil::format( "ASCII world loaded in %.02fsec\n", ( static_cast<R32>( e_t - s_t )) / 1000.0f ));

	UI08 i		= 0;
	for( WORLDLIST_ITERATOR wIter = mapWorlds.begin(); wIter != mapWorlds.end(); ++wIter )
	{
		( *wIter )->LoadResources( i );
		++i;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::LoadFromDisk()
//|	Date		-	23 July, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads in objects from specified file
//o------------------------------------------------------------------------------------------------o
void CMapHandler::LoadFromDisk( std::ifstream& readDestination, SI32 baseValue, SI32 fileSize, UI32 maxSize )
{
	char line[1024];
	R32 basePercent	= static_cast<R32>( baseValue ) / static_cast<R32>( maxSize ) * 100.0f;
	R32 targPercent	= static_cast<R32>( baseValue + fileSize ) / static_cast<R32>( maxSize ) * 100.0f;
	R32 diffValue	= targPercent - basePercent;

	SI32 updateCount = 0;
	while( !readDestination.eof() && !readDestination.fail() )
	{
		readDestination.getline( line, 1023 );
		line[readDestination.gcount()] = 0;
		std::string sLine( line );
		sLine = oldstrutil::trim( sLine );

		if( sLine.substr( 0, 1 ) == "[" )	// in a section
		{
			sLine = sLine.substr( 1, sLine.size() - 2 );
			sLine = oldstrutil::upper( oldstrutil::trim( sLine ));
			if( sLine == "CHARACTER" )
			{
				LoadChar( readDestination );
			}
			else if( sLine == "ITEM" )
			{
				LoadItem( readDestination );
			}
			else if( sLine == "HOUSE" )
			{
				LoadMulti( readDestination );
			}
			else if( sLine == "BOAT" )
			{
				LoadBoat( readDestination );
			}
			else if( sLine == "SPAWNITEM" )
			{
				LoadSpawnItem( readDestination );
			}

			if( fileSize != -1 && ( ++updateCount ) % 200 == 0 )
			{
				R32 curPos	= readDestination.tellg();
				R32 tempVal	= basePercent + ( curPos / fileSize * diffValue );
				if( tempVal <= 10 )
				{
					Console << "\b\b" << static_cast<UI32>( tempVal ) << "%";
				}
				else
				{
					Console << "\b\b\b" << static_cast<UI32>( tempVal ) << "%";
				}
			}
		}
		else if( sLine == "---REGION---" )	// end of region
			continue;
	}
}
