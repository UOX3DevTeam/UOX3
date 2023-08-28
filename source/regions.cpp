#include "regions.h"

#include <algorithm>
#include <filesystem>
#include <utility>
#include <vector>

#include "uox3.h"
#include "classes.h"
#include "StringUtility.hpp"
#include "ObjectFactory.h"

using namespace std::string_literals ;

#define DEBUG_REGIONS		0
#define SAVE_CONSOLE

CMapHandler *MapRegion;

//o------------------------------------------------------------------------------------------------o
//|	Function	-	LoadChar()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a new character object based on data loaded from worldfiles
//o------------------------------------------------------------------------------------------------o
auto LoadChar( std::istream& readDestination ) ->void {
	auto x = static_cast<CChar *>( ObjectFactory::GetSingleton().CreateBlankObject( OT_CHAR ));
	if( x == nullptr )
		return;

	if( !x->Load( readDestination )){
		x->Cleanup();
		ObjectFactory::GetSingleton().DestroyObject( x );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	LoadItem()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a new item object based on data loaded from worldfiles
//o------------------------------------------------------------------------------------------------o
auto LoadItem( std::istream& readDestination ) -> void {
	auto x = static_cast<CItem *>( ObjectFactory::GetSingleton().CreateBlankObject( OT_ITEM ));
	if( x == nullptr )
		return;

	if( !x->Load( readDestination )){
		x->Cleanup();
		ObjectFactory::GetSingleton().DestroyObject( x );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	LoadMulti()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a new multi object, like a house, based on data loaded from worldfiles
//o------------------------------------------------------------------------------------------------o
auto LoadMulti( std::istream& readDestination ) ->void  {
	auto ourHouse = static_cast<CMultiObj *>( ObjectFactory::GetSingleton().CreateBlankObject( OT_MULTI ));
	if( !ourHouse->Load( readDestination )) {
		ourHouse->Cleanup();
		ObjectFactory::GetSingleton().DestroyObject( ourHouse );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	LoadBoat()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a new boat object based on data loaded from worldfiles
//o------------------------------------------------------------------------------------------------o
auto LoadBoat( std::istream& readDestination ) ->void {
	auto ourBoat = static_cast<CBoatObj *>( ObjectFactory::GetSingleton().CreateBlankObject( OT_BOAT ));
	if( !ourBoat->Load( readDestination )) {
		ourBoat->Cleanup();
		ObjectFactory::GetSingleton().DestroyObject( ourBoat );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	LoadSpawnItem()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a new spawn object based on data loaded from worldfiles
//o------------------------------------------------------------------------------------------------o
auto LoadSpawnItem( std::istream& readDestination ) ->void {
	auto ourSpawner = static_cast<CSpawnItem *>( ObjectFactory::GetSingleton().CreateBlankObject( OT_SPAWNER ));
	if( !ourSpawner->Load( readDestination )) {
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
auto CMapRegion::SaveToDisk( std::ostream& writeDestination ) ->void {
	auto removeChar = std::vector<CChar *>() ;
	for( const auto &charToWrite : charData.collection() )
	{
		if( !ValidateObject( charToWrite )){
			removeChar.push_back( charToWrite );
		}
		else {
#if defined( _MSC_VER )
#pragma todo( "PlayerHTML Dumping needs to be reimplemented" )
#endif
			if( charToWrite->ShouldSave() ) {
				charToWrite->Save( writeDestination );
			}
		}
	}
	std::for_each( removeChar.begin(), removeChar.end(), [this]( CChar *character ) {
		charData.Remove( character );
	});

    auto removeItem = std::vector<CItem *>();
	for( const auto &itemToWrite : itemData.collection() ) {
		if( !ValidateObject( itemToWrite )) {
			removeItem.push_back( itemToWrite );
		}
		else {
			if( itemToWrite->ShouldSave() ) {
				if( itemToWrite->GetObjType() == OT_MULTI ) {
					CMultiObj *iMulti = static_cast<CMultiObj *>( itemToWrite );
					iMulti->Save( writeDestination );
				}
				else if( itemToWrite->GetObjType() == OT_BOAT ) {
					CBoatObj *iBoat = static_cast<CBoatObj *>( itemToWrite );
					iBoat->Save( writeDestination );
				}
				else {
					itemToWrite->Save( writeDestination );
				}
			}
		}
	}
	std::for_each( removeItem.begin(), removeItem.end(), [this]( CItem *item ) {
		itemData.Remove( item );
	});
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapRegion::GetItemList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the Item DataList for iteration
//o------------------------------------------------------------------------------------------------o
auto CMapRegion::GetItemList() ->GenericList<CItem *> * {
	return &itemData;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapRegion::GetCharList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the Character DataList for iteration
//o------------------------------------------------------------------------------------------------o
auto CMapRegion::GetCharList()->GenericList<CChar *> * {
	return &charData;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapRegion::GetRegionSerialList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the list of baseobject serials for iteration
//o------------------------------------------------------------------------------------------------o
auto CMapRegion::GetRegionSerialList()->RegionSerialList* {
	return &regionSerialData;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapRegion::HasRegionChanged()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a flag that says whether the region has seen any updates since last save
//o------------------------------------------------------------------------------------------------o
auto CMapRegion::HasRegionChanged() const -> bool {
	return hasRegionChanged;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapRegion::HasRegionChanged()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets a flag that says whether the region has seen any updates since last save
//o------------------------------------------------------------------------------------------------o
auto CMapRegion::HasRegionChanged( bool newVal ) ->void{
	hasRegionChanged = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapWorld Constructor/Destructor
//|	Date		-	17 October, 2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Initializes & Clears the MapWorld data
//o------------------------------------------------------------------------------------------------o
CMapWorld::CMapWorld() : upperArrayX( 0 ), upperArrayY( 0 ), resourceX( 0 ), resourceY( 0 ) {
    
    mapResources = std::vector<MapResource_st>(1) ;  // Always at least one map resource
    mapRegions = std::vector<CMapRegion>() ;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapWorld::CMapWorld()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Initializes resource regions for the given world
//o------------------------------------------------------------------------------------------------o
CMapWorld::CMapWorld( std::uint8_t worldNum ) {
	auto [mapWidth, mapHeight] = Map->SizeOfMap( worldNum );
	upperArrayX			= static_cast<std::int16_t>( mapWidth / MapColSize );
	upperArrayY			= static_cast<std::int16_t>( mapHeight/ MapRowSize );
	resourceX			= static_cast<std::uint16_t>( mapWidth / cwmWorldState->ServerData()->ResourceAreaSize() );
	resourceY			= static_cast<std::uint16_t>( mapHeight / cwmWorldState->ServerData()->ResourceAreaSize() );

	auto resourceSize = ( static_cast<size_t>( resourceX ) * static_cast<size_t>( resourceY ));
	if( resourceSize < 1 ) {
		resourceSize = 1;  // ALWAYS initialize at least one resource region.
	}
	mapResources.resize( resourceSize );

	mapRegions.resize( static_cast<size_t>( upperArrayX ) * static_cast<size_t>( upperArrayY ));
}


//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapWorld::GetMapRegion()
//|	Date		-	9/13/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the MapRegion based on its x and y offsets
//o------------------------------------------------------------------------------------------------o
auto CMapWorld::GetMapRegion( std::int16_t xOffset, std::int16_t yOffset ) ->CMapRegion *  {
	CMapRegion *mRegion			= nullptr;
	auto regionIndex	= ( static_cast<size_t>( xOffset ) * static_cast<size_t>( upperArrayY ) + static_cast<size_t>( yOffset ));

	if( xOffset >= 0 && xOffset < upperArrayX && yOffset >= 0 && yOffset < upperArrayY ) {
		if( regionIndex < mapRegions.size() ) {
			mRegion = &mapRegions[regionIndex];
		}
	}

	return mRegion;
}
//=================================================================================================
auto CMapWorld::GetMapRegions()->std::vector<CMapRegion> * {
	return &mapRegions;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapWorld::GetResource()
//|	Date		-	9/17/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the Resource region x and y is.
//o------------------------------------------------------------------------------------------------o
auto CMapWorld::GetResource( std::int16_t x, std::int16_t y ) ->MapResource_st&  {
	auto gridX = static_cast<std::uint16_t>( x / cwmWorldState->ServerData()->ResourceAreaSize() );
	auto gridY = static_cast<std::uint16_t>( y / cwmWorldState->ServerData()->ResourceAreaSize() );

	auto resIndex = (( static_cast<size_t>( gridX ) * static_cast<size_t>( resourceY )) + static_cast<size_t>( gridY ));

	if( gridX >= resourceX || gridY >= resourceY || resIndex > mapResources.size() ) {
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
auto CMapWorld::SaveResources( std::uint8_t worldNum ) ->void {
    auto resourceFile = std::filesystem::path(cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "resource["s + std::to_string( worldNum ) + "].bin"s);
    auto output = std::ofstream(resourceFile.string(), std::ios::binary);
    auto buffer = std::vector<std::int16_t>(3,0) ;
    
    if (output.is_open()) {
        for (auto iter = mapResources.begin(); iter != mapResources.end();iter++) {
            buffer[0] = iter->oreAmt ;
            std::reverse(reinterpret_cast<char*>(buffer.data()),reinterpret_cast<char*>(buffer.data())+2);  // Make it big endian
            buffer[1] = iter->logAmt ;
            std::reverse(reinterpret_cast<char*>(buffer.data())+2,reinterpret_cast<char*>(buffer.data())+4);
            buffer[2] = iter->fishAmt ;
            std::reverse(reinterpret_cast<char*>(buffer.data())+4,reinterpret_cast<char*>(buffer.data())+6);
            // Now write it
            output.write(reinterpret_cast<char*>(buffer.data()),buffer.size()*2);
        }
    }
	else {
        // Can't save resources
		Console.Error( "Failed to open resource.bin for writing" );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapWorld::LoadResources()
//|	Date		-	9/17/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads the Resource data from the disk
//o------------------------------------------------------------------------------------------------o
auto CMapWorld::LoadResources( std::uint8_t worldNum ) ->void{
    mapResources = std::vector<MapResource_st>(mapResources.size(),MapResource_st(cwmWorldState->ServerData()->ResOre(),cwmWorldState->ServerData()->ResLogs(),cwmWorldState->ServerData()->ResFish(),BuildTimeValue( static_cast<R32>( cwmWorldState->ServerData()->ResOreTime() )), BuildTimeValue( static_cast<R32>( cwmWorldState->ServerData()->ResLogTime() )),BuildTimeValue( static_cast<R32>( cwmWorldState->ServerData()->ResFishTime() ))))  ;
    
    auto resourceFile = std::filesystem::path(cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "resource["s + oldstrutil::number( worldNum ) + "].bin"s);

    // The data is grouped as three shorts (for each resource), so we read in that format
    auto buffer = std::vector<std::int16_t>(3,0) ;
    auto input = std::ifstream(resourceFile.string(),std::ios::binary) ;

    // We want to get the iteratro for the first mapResources ;
    auto iter = mapResources.begin() ;
    if (input.is_open()){
        while(input.good() && !input.eof() && iter != mapResources.end()){
            input.read(reinterpret_cast<char*>(buffer.data()),buffer.size()*2) ;
            if (input.gcount() != buffer.size()*2) {
                // We had issues reading the full amount, break out of this
                break;
            }
            // For whatever reason the resources are stored in big endidan, which on int/arm machines , we need little endian, so reverse them
            std::for_each(buffer.begin(), buffer.end(), [](std::int16_t &value){
                std::reverse(reinterpret_cast<char*>(&value),reinterpret_cast<char*>(&value)+2);
            });
            // Now set the values
            (*iter).oreAmt = buffer[0] ;
            (*iter).logAmt = buffer[1] ;
            (*iter).fishAmt = buffer[2] ;
             iter++ ;
        }
    }


}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::Startup()
//|	Date		-	23 July, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Fills and clears the mapWorlds vector.
//o------------------------------------------------------------------------------------------------o
auto CMapHandler::Startup() -> void {
	auto numWorlds = Map->MapCount();

	mapWorlds.reserve( numWorlds );
	for( std::uint8_t i = 0; i < numWorlds; ++i ) {
		mapWorlds.push_back( std::make_unique< CMapWorld>( i ));
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::ChangeRegion()
//|	Date		-	February 1, 2006
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Changes an items region ONLY if he has changed regions.
//o------------------------------------------------------------------------------------------------o
bool CMapHandler::ChangeRegion( CItem *nItem, std::int16_t x, std::int16_t y, std::uint8_t worldNum ) {
	if( !ValidateObject( nItem ))
		return false;

	auto curCell = GetMapRegion( nItem );
	auto newCell = GetMapRegion( GetGridX( x ), GetGridY( y ), worldNum );

	if( curCell != newCell ) {
		if( !curCell->GetRegionSerialList()->Remove( nItem->GetSerial() ) || !curCell->GetItemList()->Remove( nItem )) {
#if DEBUG_REGIONS
			Console.Warning( oldstrutil::format( "Item 0x%X does not exist in MapRegion, remove failed", nItem->GetSerial() ));
#endif
		}
		else {
			if( nItem->ShouldSave() ) {
				curCell->HasRegionChanged( true );
			}
		}

		if( newCell->GetRegionSerialList()->Add( nItem->GetSerial() )) {
			newCell->GetItemList()->Add( nItem, false );
			if( nItem->ShouldSave() ) {
				newCell->HasRegionChanged( true );
			}
			return true;
		}
		else {
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
auto CMapHandler::ChangeRegion( CChar *nChar, std::int16_t x, std::int16_t y, std::uint8_t worldNum )->bool {
	if( !ValidateObject( nChar ))
		return false;

	auto curCell = GetMapRegion( nChar );
	auto newCell = GetMapRegion( GetGridX( x ), GetGridY( y ), worldNum );

	if( curCell != newCell ) {
		if( !curCell->GetRegionSerialList()->Remove( nChar->GetSerial() ) || !curCell->GetCharList()->Remove( nChar )) {
#if DEBUG_REGIONS
			Console.Warning( oldstrutil::format( "Character 0x%X does not exist in MapRegion, remove failed", nChar->GetSerial() ));
#endif
		}
		else {
			if( nChar->ShouldSave() ) {
				curCell->HasRegionChanged( true );
			}
		}

		if( newCell->GetRegionSerialList()->Add( nChar->GetSerial() )) {
			newCell->GetCharList()->Add( nChar, false );
			if( nChar->ShouldSave() ) {
				newCell->HasRegionChanged( true );
			}
			return true;
		}
		else {
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
auto  CMapHandler::AddItem( CItem *nItem )->bool {
	if( !ValidateObject( nItem ))
		return false;

	auto cell = GetMapRegion( nItem );
	if( cell->GetRegionSerialList()->Add( nItem->GetSerial() )) {
		cell->GetItemList()->Add( nItem, false );
		if( nItem->ShouldSave() ) {
			cell->HasRegionChanged( true );
		}
		return true;
	}
	else {
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
auto CMapHandler::RemoveItem( CItem *nItem ) ->bool {
	if( !ValidateObject( nItem ))
		return false;

	auto cell = GetMapRegion( nItem );
	if( cell->GetRegionSerialList()->Remove( nItem->GetSerial() )) {
		cell->GetItemList()->Remove( nItem );
		if( nItem->ShouldSave() ) {
			cell->HasRegionChanged( true );
		}
	}
	else {
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
auto CMapHandler::AddChar( CChar *toAdd )->bool {
	if( !ValidateObject( toAdd ))
		return false;

	auto cell = GetMapRegion( toAdd );
	if( cell->GetRegionSerialList()->Add( toAdd->GetSerial() )) {
		cell->GetCharList()->Add( toAdd, false );
		if( toAdd->ShouldSave() ) {
			cell->HasRegionChanged( true );
		}
		return true;
	}
	else {
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
auto CMapHandler::RemoveChar( CChar *toRemove )->bool {
	if( !ValidateObject( toRemove ))
		return false;

	auto cell = GetMapRegion( toRemove );
	if( cell->GetRegionSerialList()->Remove( toRemove->GetSerial() )) {
		cell->GetCharList()->Remove( toRemove );
		if( toRemove->ShouldSave() ) {
			cell->HasRegionChanged( true );
		}
	}
	else {
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
auto CMapHandler::GetGridX( std::int16_t x )->std::int16_t  {
	return static_cast<std::int16_t>( x / MapColSize );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::GetGridY()
//|	Date		-	23 July, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Finds the Row of SubRegion we want based on location
//o------------------------------------------------------------------------------------------------o
auto CMapHandler::GetGridY( std::int16_t y ) -> std::int16_t {
	return static_cast<std::int16_t>( y / MapRowSize );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::GetMapRegion()
//|	Date		-	23 July, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the MapRegion based on the offsets
//o------------------------------------------------------------------------------------------------o
auto CMapHandler::GetMapRegion( std::int16_t xOffset, std::int16_t yOffset, std::uint8_t worldNumber )->CMapRegion * {
	auto mRegion = mapWorlds[worldNumber]->GetMapRegion( xOffset, yOffset );
	if( mRegion == nullptr ) {
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
auto CMapHandler::GetMapRegion( CBaseObject *mObj )->CMapRegion * {
	return GetMapRegion( GetGridX( mObj->GetX() ), GetGridY( mObj->GetY() ), mObj->WorldNumber() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::GetMapWorld()
//|	Date		-	9/13/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the MapWorld object associated with the worldNumber
//o------------------------------------------------------------------------------------------------o
auto CMapHandler::GetMapWorld( std::uint8_t worldNum ) ->CMapWorld * {
	CMapWorld *mWorld = nullptr;
	if( worldNum > mapWorlds.size() ) {
		mWorld = mapWorlds[worldNum].get();
	}
	return mWorld;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::GetMapWorld()
//|	Date		-	9/13/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the MapWorld object associated with the worldNumber
//o------------------------------------------------------------------------------------------------o
auto CMapHandler::GetResource( std::int16_t x, std::int16_t y, std::uint8_t worldNum )-> MapResource_st *  {
	MapResource_st *resData = nullptr;
	if( worldNum < mapWorlds.size() ) {
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
auto CMapHandler::PopulateList( std::int16_t x, std::int16_t y, std::uint8_t worldNumber ) -> std::vector<CMapRegion *>
{
	auto nearbyRegions =std::vector<CMapRegion *>();
	auto xOffset	= MapRegion->GetGridX( x );
	auto yOffset	= MapRegion->GetGridY( y );
	for( std::int8_t counter1 = -1; counter1 <= 1; ++counter1 ) {
		for( std::int8_t counter2 = -1; counter2 <= 1; ++counter2 ) {
			auto MapArea = GetMapRegion( xOffset + counter1, yOffset + counter2, worldNumber );
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
constexpr auto BUFFERSIZE = static_cast<size_t>(1024 * 1024) ;
static auto streamBuffer = std::vector<char>(BUFFERSIZE,0) ;
//===============================================================================================
auto CMapHandler::Save()->void {
	const auto AreaX = static_cast<int16_t>(UpperX / 8);	// we're storing 8x8 grid arrays together
	const auto AreaY = static_cast<int16_t>(UpperY / 8);
	auto onePercent = 0;
	auto i = static_cast<std::uint8_t>(0);
	for( i = 0; i < Map->MapCount(); ++i ) {
		auto [mapWidth, mapHeight] = Map->SizeOfMap( i );
		onePercent += static_cast<std::int32_t>(( mapWidth / MapColSize ) * ( mapHeight / MapRowSize ));
	}
	onePercent /= 100.0f;
    const auto blockDiscriminator = "\n\n---REGION---\n\n"s;
	
	auto count= static_cast<std::uint32_t>(0);
    auto s_t = static_cast<std::uint32_t>(GetClock());

	Console << "Saving Character and Item Map Region data...   ";
	Console.TurnYellow();
#if defined(SAVE_CONSOLE)
	Console << "0%";
#endif
	auto basePath = cwmWorldState->ServerData()->Directory( CSDDP_SHARED );

	// Legacy - deletes house.wsc on next world save, since house data is now saved in the regional
	// wsc files along with other objects, so we don't want this file loaded again on next startup
    auto houseFilePath = std::filesystem::path( basePath + "house.wsc"s);
    if (std::filesystem::exists(houseFilePath)){
        std::filesystem::remove( houseFilePath );
    }

    for( std::int16_t counter1 = 0; counter1 < AreaX; ++counter1 ){	// move left->right
		const std::int32_t baseX = counter1 * 8;
        for( std::int16_t counter2 = 0; counter2 < AreaY; ++counter2 ){	// move up->down
			const std::int32_t baseY = counter2 * 8;								// calculate x grid offset
			auto filename = basePath + oldstrutil::number( counter1 ) + "."s + oldstrutil::number( counter2 ) + ".wsc"s;	// let's name our file

			bool changesDetected = false;
			for( std::uint8_t xCnt = 0; !changesDetected && xCnt < 8; ++xCnt ){ // walk through each part of the 8x8 grid, left->right
                for( std::uint8_t yCnt = 0; !changesDetected && yCnt < 8; ++yCnt )	{			// walk the row
					for( auto mIter = mapWorlds.begin(); mIter != mapWorlds.end(); ++mIter ) {
						auto mRegion = ( *mIter )->GetMapRegion(( baseX + xCnt ), ( baseY + yCnt ));
						if( mRegion != nullptr ) {
							// Only save objects mapRegions marked as "changed" by objects updated in said regions
							if( mRegion->HasRegionChanged() ) {
								changesDetected = true;
								break;
							}
						}
					}
				}
			}
			
			if( !changesDetected )
				continue;

			auto writeDestination = std::ofstream(filename.c_str(), std::ios::binary ); // We need to investigage the std::ios::binary, for text file

			if( !writeDestination.is_open() ) {
				Console.Error( oldstrutil::format( "Failed to open %s for writing", filename.c_str() ));
				continue;
			}

			writeDestination.rdbuf()->pubsetbuf( streamBuffer.data(), BUFFERSIZE );

            for( std::uint8_t xCnt = 0; xCnt < 8; ++xCnt )	{				// walk through each part of the 8x8 grid, left->right
                for( std::uint8_t yCnt = 0; yCnt < 8; ++yCnt )	{			// walk the row
					for( auto mIter = mapWorlds.begin(); mIter != mapWorlds.end(); ++mIter ) {
#if defined(SAVE_CONSOLE)

						++count;
						if( count%onePercent == 0 ) {
							if( count/onePercent <= 10 ) {
								Console << "\b\b" << std::to_string( count / onePercent ) << "%";
							}
							else if( count/onePercent <= 100 ) {
								Console << "\b\b\b" << std::to_string( count / onePercent ) << "%";
							}
						}
#endif
						auto mRegion = ( *mIter )->GetMapRegion(( baseX + xCnt ), ( baseY + yCnt ));
						if( mRegion != nullptr ) {
							mRegion->SaveToDisk( writeDestination );

							// Remove "changed" flag from region, to avoid it saving again needlessly on next save
							mRegion->HasRegionChanged( false );
						}

						//writeDestination << blockDiscriminator;  // Lets not do this, just to see performance difference
					}
				}
			}
			writeDestination.close();
		}
	}
#if defined(SAVE_CONSOLE)

	Console << "\b\b\b\b" << static_cast<std::uint32_t>( 100 ) << "%";
#endif
	auto filename = basePath + "overflow.wsc"s;
	auto writeDestination = std::ofstream( filename.c_str() );

	if( writeDestination.is_open() ) {
		overFlow.SaveToDisk( writeDestination );
		writeDestination.close();
	}
	else {
		Console.Error( oldstrutil::format( "Failed to open %s for writing", filename.c_str() ));
		return;
	}

	Console << "\b\b\b\b";
	Console.PrintDone();

	const auto e_t = static_cast<std::uint32_t>(GetClock());
	Console.Print( oldstrutil::format( "World saved in %.02fsec\n", ( static_cast<R32>( e_t - s_t )) / 1000.0f ));

	i = 0;
	for( auto wIter = mapWorlds.begin(); wIter != mapWorlds.end(); ++wIter )
	{
		( *wIter )->SaveResources( i );
		++i;
	}
}

//===============================================================================================
auto PostLoadFunctor( CBaseObject *a, [[maybe_unused]] std::uint32_t &b, [[maybe_unused]] void *extraData )->bool {
	if( ValidateObject( a )) {
		if( !a->IsFree() ) {
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
auto CMapHandler::Load() ->void{
	auto AreaX		= UpperX / 8;	// we're storing 8x8 grid arrays together
	auto AreaY		= UpperY / 8;
    auto count = static_cast<std::uint32_t>(0) ;
	Console.TurnYellow();
	Console << "0%";
	auto s_t = GetClock();
	auto basePath	= cwmWorldState->ServerData()->Directory( CSDDP_SHARED );

	auto runningCount = static_cast<std::uint32_t>(0);
    auto fileSizes = std::vector<std::vector<std::int32_t>>(AreaX,std::vector<std::int32_t>(AreaY,0)) ;
	
	for( std::int16_t cx = 0; cx < AreaX; ++cx ) {
		for( std::int16_t cy = 0; cy < AreaY; ++cy ) {
            auto path = std::filesystem::path( basePath + std::to_string(cx) + "."s + std::to_string(cy) + ".wsc"s );
            if (std::filesystem::exists(path)){
                fileSizes[cx][cy] = std::filesystem::file_size(path) ;
            }
			runningCount += fileSizes[cx][cy];
		}
	}

    runningCount = std::max(static_cast<std::uint32_t>(1), runningCount) ; // Ensure at least 1

	auto runningDone = 0;
    for( std::int16_t counter1 = 0; counter1 < AreaX; ++counter1 ){	// move left->right
        for( std::int16_t counter2 = 0; counter2 < AreaY; ++counter2 ){	// move up->down
            auto path =  std::filesystem::path(basePath + std::to_string(counter1) + "."s + std::to_string(counter2) + ".wsc"s );
            auto input = std::ifstream(path.string()) ;
            if (!input.is_open() || input.eof()){
                continue;
            }
			++count;
			LoadFromDisk( input, runningDone, fileSizes[counter1][counter2], runningCount );

			runningDone		+= fileSizes[counter1][counter2];
			auto tempVal	= static_cast<R32>( runningDone ) / static_cast<R32>( runningCount ) * 100.0f;
			if( tempVal <= 10 ) {
				Console << "\b\b" << static_cast<std::uint32_t>( tempVal ) << "%";
			}
			else if( tempVal <= 100 ) {
				Console << "\b\b\b" << static_cast<std::uint32_t>( tempVal ) << "%";
			}
		}
	}

	// If runningDone is still 0, there was nothing to load! 100% it
	if( runningDone == 0 ) {
		Console << "\b\b" << static_cast<std::uint32_t>( 100 ) << "%";
	}

	Console.TurnNormal();
	Console << "\b\b\b";
	Console.PrintDone();

	
	auto flowDestination = std::ifstream( basePath + "overflow.wsc"s );
	LoadFromDisk( flowDestination, -1, -1, -1 );
	flowDestination.close();

    auto houseDestination = std::ifstream(basePath + "house.wsc"s);
	LoadFromDisk( houseDestination, -1, -1, -1 );

	auto b = static_cast<std::uint32_t>(0);
	ObjectFactory::GetSingleton().IterateOver( OT_MULTI, b, nullptr, &PostLoadFunctor );
	ObjectFactory::GetSingleton().IterateOver( OT_ITEM, b, nullptr, &PostLoadFunctor );
	ObjectFactory::GetSingleton().IterateOver( OT_CHAR, b, nullptr, &PostLoadFunctor );
	houseDestination.close();

	std::uint32_t e_t	= GetClock();
	Console.Print( oldstrutil::format( "ASCII world loaded in %.02fsec\n", ( static_cast<R32>( e_t - s_t )) / 1000.0f ));

	std::uint8_t i		= 0;
	for( auto wIter = mapWorlds.begin(); wIter != mapWorlds.end(); ++wIter )
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
auto CMapHandler::LoadFromDisk( std::istream& readDestination, std::int32_t baseValue, std::int32_t fileSize, std::uint32_t maxSize ) ->void {
    auto buffer = std::vector<char>(1024,0) ;
	auto basePercent	= static_cast<R32>( baseValue ) / static_cast<R32>( maxSize ) * 100.0f;
	auto targPercent	= static_cast<R32>( baseValue + fileSize ) / static_cast<R32>( maxSize ) * 100.0f;
	auto diffValue	= targPercent - basePercent;

	auto updateCount = 0;
	while( readDestination.good() && !readDestination.eof())
	{
		readDestination.getline( buffer.data(), buffer.size()-1 );
        if (readDestination.gcount() >0){
            buffer[readDestination.gcount()] = 0;
            std::string line = buffer.data() ;
            line = oldstrutil::trim(line) ;
            if (!line.empty()){
                if (line[0] == '['){
                    // It is a section, maybe
                    line = oldstrutil::upper(oldstrutil::trim(line.substr(1,line.size()-2)) );
                    // we have a few choices
                    if (line == "CHARACTER"){
                        LoadChar(readDestination);
                    }
                    else if (line == "ITEM") {
                        LoadItem( readDestination );
                    }
                    else if (line == "HOUSE"){
                        LoadMulti( readDestination );
                    }
                    else if (line == "BOAT"){
                        LoadBoat( readDestination );
                    }
                    else if (line == "SPAWNITEM"){
                        LoadSpawnItem( readDestination );
                    }
                    
                    if( fileSize != -1 && (( ++updateCount ) % 200) == 0 ){
                        auto curPos = static_cast<float>(readDestination.tellg()) ;
                        auto tempVal = static_cast<float>(basePercent + ( curPos / fileSize * diffValue )) ;
                        if( tempVal <= 10.0 ) {
                            Console << "\b\b" << static_cast<std::uint32_t>( tempVal ) << "%";
                        }
                        else {
                            Console << "\b\b\b" << static_cast<std::uint32_t>( tempVal ) << "%";
                        }
                    }
                }
            }
        }
	}
}
