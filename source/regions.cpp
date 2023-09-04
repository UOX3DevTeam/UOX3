#include "regions.h"

#include <algorithm>
#include <filesystem>
#include <vector>

#include "cbaseobject.h"
#include "cchar.h"
#include "citem.h"
#include "classes.h"
#include "cmultiobj.h"
#include "funcdecl.h"
#include "mapstuff.h"
#include "subsystem/console.hpp"

#include "objectfactory.h"
#include "stringutility.hpp"
#include "utility/strutil.hpp"

#include "useful.h"
using namespace std::string_literals;

#define DEBUG_REGIONS 0

CMapHandler *MapRegion;
// o------------------------------------------------------------------------------------------------o
//|	Function	-	FileSize()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the filesize of a given file
// o------------------------------------------------------------------------------------------------o
std::int32_t FileSize(std::string filename) {
    std::int32_t retVal = 0;
    
    try {
        retVal = static_cast<std::int32_t>(std::filesystem::file_size(filename));
    } catch (...) {
        retVal = 0;
    }
    
    return retVal;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	LoadChar()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a new character object based on data loaded from worldfiles
// o------------------------------------------------------------------------------------------------o
void LoadChar(std::istream &readDestination) {
    CChar *x = static_cast<CChar *>(ObjectFactory::shared().CreateBlankObject(CBaseObject::OT_CHAR));
    if (x == nullptr)
        return;
    
    if (!x->Load(readDestination)) {
        x->Cleanup();
        ObjectFactory::shared().DestroyObject(x);
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	LoadItem()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a new item object based on data loaded from worldfiles
// o------------------------------------------------------------------------------------------------o
void LoadItem(std::istream &readDestination) {
    CItem *x = static_cast<CItem *>(ObjectFactory::shared().CreateBlankObject(CBaseObject::OT_ITEM));
    if (x == nullptr)
        return;
    
    if (!x->Load(readDestination)) {
        x->Cleanup();
        ObjectFactory::shared().DestroyObject(x);
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	LoadMulti()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a new multi object, like a house, based on data loaded from
// worldfiles
// o------------------------------------------------------------------------------------------------o
void LoadMulti(std::istream &readDestination) {
    CMultiObj *ourHouse =
    static_cast<CMultiObj *>(ObjectFactory::shared().CreateBlankObject(CBaseObject::OT_MULTI));
    if (!ourHouse->Load(readDestination)) // if no load, DELETE
    {
        ourHouse->Cleanup();
        ObjectFactory::shared().DestroyObject(ourHouse);
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	LoadBoat()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a new boat object based on data loaded from worldfiles
// o------------------------------------------------------------------------------------------------o
void LoadBoat(std::istream &readDestination) {
    CBoatObj *ourBoat =
    static_cast<CBoatObj *>(ObjectFactory::shared().CreateBlankObject(CBaseObject::OT_BOAT));
    if (!ourBoat->Load(readDestination)) // if no load, DELETE
    {
        ourBoat->Cleanup();
        ObjectFactory::shared().DestroyObject(ourBoat);
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	LoadSpawnItem()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a new spawn object based on data loaded from worldfiles
// o------------------------------------------------------------------------------------------------o
void LoadSpawnItem(std::istream &readDestination) {
    CSpawnItem *ourSpawner =
    static_cast<CSpawnItem *>(ObjectFactory::shared().CreateBlankObject(CBaseObject::OT_SPAWNER));
    if (!ourSpawner->Load(readDestination)) // if no load, DELETE
    {
        ourSpawner->Cleanup();
        ObjectFactory::shared().DestroyObject(ourSpawner);
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Functio		-	CMapRegion::SaveToDisk()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Save all items and characters inside a subregion reworked SaveChar
// from WorldMain |					to deal with pointer based stuff in region
// rather than index based stuff in array
//|					Also saves out all data regardless (in preparation for a
//simple binary save)
// o------------------------------------------------------------------------------------------------o
void CMapRegion::SaveToDisk(std::ostream &writeDestination) {
    std::vector<CChar *> removeChar;
    for (const auto &charToWrite : charData.collection()) {
        if (!ValidateObject(charToWrite)) {
            removeChar.push_back(charToWrite);
        }
        else {
            // todo( "PlayerHTML Dumping needs to be reimplemented" );
            
            if (charToWrite->ShouldSave()) {
                charToWrite->Save(writeDestination);
            }
        }
    }
    std::for_each(removeChar.begin(), removeChar.end(),
                  [this](CChar *character) { charData.Remove(character); });
    
    std::vector<CItem *> removeItem;
    for (const auto &itemToWrite : itemData.collection()) {
        if (!ValidateObject(itemToWrite)) {
            removeItem.push_back(itemToWrite);
        }
        else {
            if (itemToWrite->ShouldSave()) {
                if (itemToWrite->GetObjType() == CBaseObject::OT_MULTI) {
                    CMultiObj *iMulti = static_cast<CMultiObj *>(itemToWrite);
                    iMulti->Save(writeDestination);
                }
                else if (itemToWrite->GetObjType() == CBaseObject::OT_BOAT) {
                    CBoatObj *iBoat = static_cast<CBoatObj *>(itemToWrite);
                    iBoat->Save(writeDestination);
                }
                else {
                    itemToWrite->Save(writeDestination);
                }
            }
        }
    }
    std::for_each(removeItem.begin(), removeItem.end(),
                  [this](CItem *item) { itemData.Remove(item); });
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapRegion::GetItemList()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the Item DataList for iteration
// o------------------------------------------------------------------------------------------------o
GenericList<CItem *> *CMapRegion::GetItemList(void) { return &itemData; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapRegion::GetCharList()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the Character DataList for iteration
// o------------------------------------------------------------------------------------------------o
GenericList<CChar *> *CMapRegion::GetCharList(void) { return &charData; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapRegion::GetRegionSerialList()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the list of baseobject serials for iteration
// o------------------------------------------------------------------------------------------------o
RegionSerialList *CMapRegion::GetRegionSerialList() { return &regionSerialData; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapRegion::HasRegionChanged()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a flag that says whether the region has seen any updates
// since last save
// o------------------------------------------------------------------------------------------------o
bool CMapRegion::HasRegionChanged(void) { return hasRegionChanged; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapRegion::HasRegionChanged()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets a flag that says whether the region has seen any updates since
// last save
// o------------------------------------------------------------------------------------------------o
void CMapRegion::HasRegionChanged(bool newVal) { hasRegionChanged = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapWorld Constructor/Destructor
//|	Date		-	17 October, 2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Initializes & Clears the MapWorld data
// o------------------------------------------------------------------------------------------------o
CMapWorld::CMapWorld() : upperArrayX(0), upperArrayY(0), resourceX(0), resourceY(0) {
    mapResources.resize(1); // ALWAYS initialize at least one resource region.
    mapRegions.resize(0);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapWorld::CMapWorld()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Initializes resource regions for the given world
// o------------------------------------------------------------------------------------------------o
CMapWorld::CMapWorld(std::uint8_t worldNum) {
    auto [mapWidth, mapHeight] = Map->SizeOfMap(worldNum);
    upperArrayX = static_cast<std::int16_t>(mapWidth / MapColSize);
    upperArrayY = static_cast<std::int16_t>(mapHeight / MapRowSize);
    resourceX = static_cast<std::uint16_t>(mapWidth / cwmWorldState->ServerData()->ResourceAreaSize());
    resourceY = static_cast<std::uint16_t>(mapHeight / cwmWorldState->ServerData()->ResourceAreaSize());
    
    size_t resourceSize = (static_cast<size_t>(resourceX) * static_cast<size_t>(resourceY));
    if (resourceSize < 1) // ALWAYS initialize at least one resource region.
    {
        resourceSize = 1;
    }
    mapResources.resize(resourceSize);
    
    mapRegions.resize(static_cast<size_t>(upperArrayX) * static_cast<size_t>(upperArrayY));
}

CMapWorld::~CMapWorld() {
    mapResources.resize(0);
    mapRegions.resize(0);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapWorld::GetMapRegion()
//|	Date		-	9/13/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the MapRegion based on its x and y offsets
// o------------------------------------------------------------------------------------------------o
CMapRegion *CMapWorld::GetMapRegion(std::int16_t xOffset, std::int16_t yOffset) {
    CMapRegion *mRegion = nullptr;
    const size_t regionIndex = (static_cast<size_t>(xOffset) * static_cast<size_t>(upperArrayY) +
                                static_cast<size_t>(yOffset));
    
    if (xOffset >= 0 && xOffset < upperArrayX && yOffset >= 0 && yOffset < upperArrayY) {
        if (regionIndex < mapRegions.size()) {
            mRegion = &mapRegions[regionIndex];
        }
    }
    
    return mRegion;
}

std::vector<CMapRegion> *CMapWorld::GetMapRegions() { return &mapRegions; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapWorld::GetResource()
//|	Date		-	9/17/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the Resource region x and y is.
// o------------------------------------------------------------------------------------------------o
MapResource_st &CMapWorld::GetResource(std::int16_t x, std::int16_t y) {
    const std::uint16_t gridX = (x / cwmWorldState->ServerData()->ResourceAreaSize());
    const std::uint16_t gridY = (y / cwmWorldState->ServerData()->ResourceAreaSize());
    
    size_t resIndex = ((static_cast<size_t>(gridX) * static_cast<size_t>(resourceY)) +
                       static_cast<size_t>(gridY));
    
    if (gridX >= resourceX || gridY >= resourceY || resIndex > mapResources.size()) {
        resIndex = 0;
    }
    
    return mapResources[resIndex];
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapWorld::SaveResources()
//|	Date		-	9/17/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Saves the Resource data to disk
// o------------------------------------------------------------------------------------------------o
void CMapWorld::SaveResources(std::uint8_t worldNum) {
    auto resourceFile = std::filesystem::path(cwmWorldState->ServerData()->Directory(CSDDP_SHARED) +
                                              "resource["s + std::to_string(worldNum) + "].bin"s);
    auto output = std::ofstream(resourceFile.string(), std::ios::binary);
    auto buffer = std::vector<std::int16_t>(3, 0);
    
    if (output.is_open()) {
        for (auto iter = mapResources.begin(); iter != mapResources.end(); iter++) {
            buffer[0] = iter->oreAmt;
            std::reverse(reinterpret_cast<char *>(buffer.data()),
                         reinterpret_cast<char *>(buffer.data()) + 2); // Make it big endian
            buffer[1] = iter->logAmt;
            std::reverse(reinterpret_cast<char *>(buffer.data()) + 2,
                         reinterpret_cast<char *>(buffer.data()) + 4);
            buffer[2] = iter->fishAmt;
            std::reverse(reinterpret_cast<char *>(buffer.data()) + 4,
                         reinterpret_cast<char *>(buffer.data()) + 6);
            // Now write it
            output.write(reinterpret_cast<char *>(buffer.data()), buffer.size() * 2);
        }
    }
    else {
        // Can't save resources
        Console::shared().Error("Failed to open resource.bin for writing");
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapWorld::LoadResources()
//|	Date		-	9/17/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads the Resource data from the disk
// o------------------------------------------------------------------------------------------------o
void CMapWorld::LoadResources(std::uint8_t worldNum) {
    mapResources = std::vector<MapResource_st>(
                                               mapResources.size(),
                                               MapResource_st(
                                                              cwmWorldState->ServerData()->ResOre(), cwmWorldState->ServerData()->ResLogs(),
                                                              cwmWorldState->ServerData()->ResFish(),
                                                              BuildTimeValue(static_cast<R32>(cwmWorldState->ServerData()->ResOreTime())),
                                                              BuildTimeValue(static_cast<R32>(cwmWorldState->ServerData()->ResLogTime())),
                                                              BuildTimeValue(static_cast<R32>(cwmWorldState->ServerData()->ResFishTime()))));
    
    auto resourceFile = std::filesystem::path(cwmWorldState->ServerData()->Directory(CSDDP_SHARED) +
                                              "resource["s + util::ntos(worldNum) + "].bin"s);
    
    // The data is grouped as three shorts (for each resource), so we read in that format
    auto buffer = std::vector<std::int16_t>(3, 0);
    auto input = std::ifstream(resourceFile.string(), std::ios::binary);
    
    // We want to get the iteratro for the first mapResources ;
    auto iter = mapResources.begin();
    if (input.is_open()) {
        while (input.good() && !input.eof() && iter != mapResources.end()) {
            input.read(reinterpret_cast<char *>(buffer.data()), buffer.size() * 2);
            if (input.gcount() != buffer.size() * 2) {
                // We had issues reading the full amount, break out of this
                break;
            }
            // For whatever reason the resources are stored in big endidan, which on int/arm
            // machines , we need little endian, so reverse them
            std::for_each(buffer.begin(), buffer.end(), [](std::int16_t &value) {
                std::reverse(reinterpret_cast<char *>(&value),
                             reinterpret_cast<char *>(&value) + 2);
            });
            // Now set the values
            (*iter).oreAmt = buffer[0];
            (*iter).logAmt = buffer[1];
            (*iter).fishAmt = buffer[2];
            iter++;
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::Startup()
//|	Date		-	23 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Fills and clears the mapWorlds vector.
// o------------------------------------------------------------------------------------------------o
auto CMapHandler::Startup() -> void {
    std::uint8_t numWorlds = Map->MapCount();
    
    mapWorlds.reserve(numWorlds);
    for (std::uint8_t i = 0; i < numWorlds; ++i) {
        mapWorlds.push_back(new CMapWorld(i));
    }
}
CMapHandler::~CMapHandler() {
    for (WORLDLIST_ITERATOR mIter = mapWorlds.begin(); mIter != mapWorlds.end(); ++mIter) {
        if ((*mIter) != nullptr) {
            delete (*mIter);
            (*mIter) = nullptr;
        }
    }
    mapWorlds.resize(0);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::ChangeRegion()
//|	Date		-	February 1, 2006
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Changes an items region ONLY if he has changed regions.
// o------------------------------------------------------------------------------------------------o
bool CMapHandler::ChangeRegion(CItem *nItem, std::int16_t x, std::int16_t y, std::uint8_t worldNum) {
    if (!ValidateObject(nItem))
        return false;
    
    CMapRegion *curCell = GetMapRegion(nItem);
    CMapRegion *newCell = GetMapRegion(GetGridX(x), GetGridY(y), worldNum);
    
    if (curCell != newCell) {
        if (!curCell->GetRegionSerialList()->Remove(nItem->GetSerial()) ||
            !curCell->GetItemList()->Remove(nItem)) {
#if DEBUG_REGIONS
            Console::shared().Warning(util::format(
                                                   "Item 0x%X does not exist in MapRegion, remove failed", nItem->GetSerial()));
#endif
        }
        else {
            if (nItem->ShouldSave()) {
                curCell->HasRegionChanged(true);
            }
        }
        
        if (newCell->GetRegionSerialList()->Add(nItem->GetSerial())) {
            newCell->GetItemList()->Add(nItem, false);
            if (nItem->ShouldSave()) {
                newCell->HasRegionChanged(true);
            }
            return true;
        }
        else {
#if DEBUG_REGIONS
            Console::shared().Warning(util::format(
                                                   "Item 0x%X already exists in MapRegion, add failed", nItem->GetSerial()));
#endif
        }
    }
    return false;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::ChangeRegion()
//|	Date		-	February 1, 2006
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Changes a characters region ONLY if he has changed regions.
// o------------------------------------------------------------------------------------------------o
bool CMapHandler::ChangeRegion(CChar *nChar, std::int16_t x, std::int16_t y, std::uint8_t worldNum) {
    if (!ValidateObject(nChar))
        return false;
    
    CMapRegion *curCell = GetMapRegion(nChar);
    CMapRegion *newCell = GetMapRegion(GetGridX(x), GetGridY(y), worldNum);
    
    if (curCell != newCell) {
        if (!curCell->GetRegionSerialList()->Remove(nChar->GetSerial()) ||
            !curCell->GetCharList()->Remove(nChar)) {
#if DEBUG_REGIONS
            Console::shared().Warning(util::format(
                                                   "Character 0x%X does not exist in MapRegion, remove failed", nChar->GetSerial()));
#endif
        }
        else {
            if (nChar->ShouldSave()) {
                curCell->HasRegionChanged(true);
            }
        }
        
        if (newCell->GetRegionSerialList()->Add(nChar->GetSerial())) {
            newCell->GetCharList()->Add(nChar, false);
            if (nChar->ShouldSave()) {
                newCell->HasRegionChanged(true);
            }
            return true;
        }
        else {
#if DEBUG_REGIONS
            Console::shared().Warning(util::format(
                                                   "Character 0x%X already exists in MapRegion, add failed", nChar->GetSerial()));
#endif
        }
    }
    return false;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::AddItem()
//|	Date		-	23 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds nItem to the proper SubRegion
// o------------------------------------------------------------------------------------------------o
bool CMapHandler::AddItem(CItem *nItem) {
    if (!ValidateObject(nItem))
        return false;
    
    CMapRegion *cell = GetMapRegion(nItem);
    if (cell->GetRegionSerialList()->Add(nItem->GetSerial())) {
        cell->GetItemList()->Add(nItem, false);
        if (nItem->ShouldSave()) {
            cell->HasRegionChanged(true);
        }
        return true;
    }
    else {
#if DEBUG_REGIONS
        CConsole::shared() onsole.Warning(
                                          util::format("Item 0x%X already exists in MapRegion, add failed", nItem->GetSerial()));
#endif
    }
    return false;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::RemoveItem()
//|	Date		-	23 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes nItem from it's CURRENT SubRegion. Do this before adjusting
// the location
// o------------------------------------------------------------------------------------------------o
bool CMapHandler::RemoveItem(CItem *nItem) {
    if (!ValidateObject(nItem))
        return false;
    
    CMapRegion *cell = GetMapRegion(nItem);
    if (cell->GetRegionSerialList()->Remove(nItem->GetSerial())) {
        cell->GetItemList()->Remove(nItem);
        if (nItem->ShouldSave()) {
            cell->HasRegionChanged(true);
        }
    }
    else {
#if DEBUG_REGIONS
        Console::shared().Warning(util::format(
                                               "Item 0x%X does not exist in MapRegion, remove failed", nItem->GetSerial()));
#endif
        return false;
    }
    return true;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::AddChar()
//|	Date		-	23 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds toAdd to the proper SubRegion
// o------------------------------------------------------------------------------------------------o
bool CMapHandler::AddChar(CChar *toAdd) {
    if (!ValidateObject(toAdd))
        return false;
    
    CMapRegion *cell = GetMapRegion(toAdd);
    if (cell->GetRegionSerialList()->Add(toAdd->GetSerial())) {
        cell->GetCharList()->Add(toAdd, false);
        if (toAdd->ShouldSave()) {
            cell->HasRegionChanged(true);
        }
        return true;
    }
    else {
#if DEBUG_REGIONS
        Console::shared().Warning(util::format(
                                               "Character 0x%X already exists in MapRegion, add failed", toAdd->GetSerial()));
#endif
    }
    return false;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::RemoveChar()
//|	Date		-	23 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes toRemove from it's CURRENT SubRegion. Do this before
// adjusting the location
// o------------------------------------------------------------------------------------------------o
bool CMapHandler::RemoveChar(CChar *toRemove) {
    if (!ValidateObject(toRemove))
        return false;
    
    CMapRegion *cell = GetMapRegion(toRemove);
    if (cell->GetRegionSerialList()->Remove(toRemove->GetSerial())) {
        cell->GetCharList()->Remove(toRemove);
        if (toRemove->ShouldSave()) {
            cell->HasRegionChanged(true);
        }
    }
    else {
#if DEBUG_REGIONS
        Console::shared().Warning(util::format(
                                               "Character 0x%X does not exist in MapRegion, remove failed", toRemove->GetSerial()));
#endif
        return false;
    }
    return true;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::GetGridX()
//|	Date		-	23 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Find the Column of SubRegion we want based on location
// o------------------------------------------------------------------------------------------------o
std::int16_t CMapHandler::GetGridX(std::int16_t x) { return static_cast<std::int16_t>(x / MapColSize); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::GetGridY()
//|	Date		-	23 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Finds the Row of SubRegion we want based on location
// o------------------------------------------------------------------------------------------------o
std::int16_t CMapHandler::GetGridY(std::int16_t y) { return static_cast<std::int16_t>(y / MapRowSize); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::GetMapRegion()
//|	Date		-	23 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the MapRegion based on the offsets
// o------------------------------------------------------------------------------------------------o
CMapRegion *CMapHandler::GetMapRegion(std::int16_t xOffset, std::int16_t yOffset, std::uint8_t worldNumber) {
    CMapRegion *mRegion = mapWorlds[worldNumber]->GetMapRegion(xOffset, yOffset);
    if (mRegion == nullptr) {
        mRegion = &overFlow;
    }
    
    return mRegion;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::GetMapRegion()
//|	Date		-	23 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the subregion that x,y is in
// o------------------------------------------------------------------------------------------------o
CMapRegion *CMapHandler::GetMapRegion(CBaseObject *mObj) {
    return GetMapRegion(GetGridX(mObj->GetX()), GetGridY(mObj->GetY()), mObj->WorldNumber());
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::GetMapWorld()
//|	Date		-	9/13/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the MapWorld object associated with the worldNumber
// o------------------------------------------------------------------------------------------------o
CMapWorld *CMapHandler::GetMapWorld(std::uint8_t worldNum) {
    CMapWorld *mWorld = nullptr;
    if (worldNum > mapWorlds.size()) {
        mWorld = mapWorlds[worldNum];
    }
    return mWorld;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::GetMapWorld()
//|	Date		-	9/13/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the MapWorld object associated with the worldNumber
// o------------------------------------------------------------------------------------------------o
MapResource_st *CMapHandler::GetResource(std::int16_t x, std::int16_t y, std::uint8_t worldNum) {
    MapResource_st *resData = nullptr;
    if (worldNum < mapWorlds.size()) {
        resData = &mapWorlds[worldNum]->GetResource(x, y);
    }
    return resData;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::PopulateList()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a list of nearby MapRegions based on the object provided
// o------------------------------------------------------------------------------------------------o
auto CMapHandler::PopulateList(CBaseObject *mObj) -> std::vector<CMapRegion *> {
    return PopulateList(mObj->GetX(), mObj->GetY(), mObj->WorldNumber());
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::PopulateList()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a list of nearby MapRegions based on the coordinates
// provided
// o------------------------------------------------------------------------------------------------o
auto CMapHandler::PopulateList(std::int16_t x, std::int16_t y, std::uint8_t worldNumber) -> std::vector<CMapRegion *> {
    std::vector<CMapRegion *> nearbyRegions;
    const std::int16_t xOffset = MapRegion->GetGridX(x);
    const std::int16_t yOffset = MapRegion->GetGridY(y);
    for (std::int8_t counter1 = -1; counter1 <= 1; ++counter1) {
        for (std::int8_t counter2 = -1; counter2 <= 1; ++counter2) {
            CMapRegion *MapArea = GetMapRegion(xOffset + counter1, yOffset + counter2, worldNumber);
            if (MapArea == nullptr)
                continue;
            
            nearbyRegions.push_back(MapArea);
        }
    }
    return nearbyRegions;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::Save()
//|	Date		-	23 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Saves out data from MapRegions to worldfiles
// o------------------------------------------------------------------------------------------------o
constexpr size_t BUFFERSIZE = 1024 * 1024;
static auto streamBuffer = std::vector<char>(BUFFERSIZE, 0);

void CMapHandler::Save(void) {
    const std::int16_t AreaX = UpperX / 8; // we're storing 8x8 grid arrays together
    const std::int16_t AreaY = UpperY / 8;
    std::ofstream writeDestination, houseDestination;
    std::int32_t onePercent = 0;
    std::uint8_t i = 0;
    for (i = 0; i < Map->MapCount(); ++i) {
        auto [mapWidth, mapHeight] = Map->SizeOfMap(i);
        onePercent += static_cast<std::int32_t>((mapWidth / MapColSize) * (mapHeight / MapRowSize));
    }
    onePercent = static_cast<std::int32_t>(onePercent / 100.0f);
    const char blockDiscriminator[] = "\n\n---REGION---\n\n";
    std::uint32_t count = 0;
    const std::uint32_t s_t = GetClock();
    
    Console::shared() << "Saving Character and Item Map Region data...   ";
    Console::shared().TurnYellow();
    Console::shared() << "0%";
    
    std::string basePath = cwmWorldState->ServerData()->Directory(CSDDP_SHARED);
    
    // Legacy - deletes house.wsc on next world save, since house data is now saved in the regional
    // wsc files along with other objects, so we don't want this file loaded again on next startup
    std::filesystem::path houseFilePath = basePath + "house.wsc";
    std::filesystem::remove(houseFilePath);
    
    std::string filename = "";
    for (std::int16_t counter1 = 0; counter1 < AreaX; ++counter1) // move left->right
    {
        const std::int32_t baseX = counter1 * 8;
        for (std::int16_t counter2 = 0; counter2 < AreaY; ++counter2) // move up->down
        {
            const std::int32_t baseY = counter2 * 8; // calculate x grid offset
            filename = basePath + util::ntos(counter1) + std::string(".") + util::ntos(counter2) +
            std::string(".wsc"); // let's name our file
            
            bool changesDetected = false;
            for (std::uint8_t xCnt = 0; !changesDetected && xCnt < 8;
                 ++xCnt) // walk through each part of the 8x8 grid, left->right
            {
                for (std::uint8_t yCnt = 0; !changesDetected && yCnt < 8; ++yCnt) // walk the row
                {
                    for (WORLDLIST_ITERATOR mIter = mapWorlds.begin(); mIter != mapWorlds.end();
                         ++mIter) {
                        CMapRegion *mRegion =
                        (*mIter)->GetMapRegion((baseX + xCnt), (baseY + yCnt));
                        if (mRegion != nullptr) {
                            // Only save objects mapRegions marked as "changed" by objects updated
                            // in said regions
                            if (mRegion->HasRegionChanged()) {
                                changesDetected = true;
                                break;
                            }
                        }
                    }
                }
            }
            
            if (!changesDetected)
                continue;
            
            writeDestination.open(filename.c_str(), std::ios::binary);
            
            if (!writeDestination) {
                Console::shared().Error(
                                        util::format("Failed to open %s for writing", filename.c_str()));
                continue;
            }
            
            writeDestination.rdbuf()->pubsetbuf(streamBuffer.data(), BUFFERSIZE);
            
            for (std::uint8_t xCnt = 0; xCnt < 8;
                 ++xCnt) // walk through each part of the 8x8 grid, left->right
            {
                for (std::uint8_t yCnt = 0; yCnt < 8; ++yCnt) // walk the row
                {
                    for (WORLDLIST_ITERATOR mIter = mapWorlds.begin(); mIter != mapWorlds.end();
                         ++mIter) {
                        ++count;
                        if (count % onePercent == 0) {
                            if (count / onePercent <= 10) {
                                Console::shared()
                                << "\b\b" << std::to_string(count / onePercent) << "%";
                            }
                            else if (count / onePercent <= 100) {
                                Console::shared()
                                << "\b\b\b" << std::to_string(count / onePercent) << "%";
                            }
                        }
                        
                        CMapRegion *mRegion =
                        (*mIter)->GetMapRegion((baseX + xCnt), (baseY + yCnt));
                        if (mRegion != nullptr) {
                            mRegion->SaveToDisk(writeDestination);
                            
                            // Remove "changed" flag from region, to avoid it saving again
                            // needlessly on next save
                            mRegion->HasRegionChanged(false);
                        }
                        
                        writeDestination << blockDiscriminator;
                    }
                }
            }
            writeDestination.close();
        }
    }
    Console::shared() << "\b\b\b\b" << static_cast<std::uint32_t>(100) << "%";
    
    filename = basePath + "overflow.wsc";
    writeDestination.open(filename.c_str());
    
    if (writeDestination.is_open()) {
        overFlow.SaveToDisk(writeDestination);
        writeDestination.close();
    }
    else {
        Console::shared().Error(util::format("Failed to open %s for writing", filename.c_str()));
        return;
    }
    
    Console::shared() << "\b\b\b\b";
    Console::shared().PrintDone();
    
    const std::uint32_t e_t = GetClock();
    Console::shared().Print(
                            util::format("World saved in %.02fsec\n", (static_cast<R32>(e_t - s_t)) / 1000.0f));
    
    i = 0;
    for (WORLDLIST_ITERATOR wIter = mapWorlds.begin(); wIter != mapWorlds.end(); ++wIter) {
        (*wIter)->SaveResources(i);
        ++i;
    }
}

bool PostLoadFunctor(CBaseObject *a, [[maybe_unused]] std::uint32_t &b, [[maybe_unused]] void *extraData) {
    if (ValidateObject(a)) {
        if (!a->IsFree()) {
            a->PostLoadProcessing();
        }
    }
    return true;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::Load()
//|	Date		-	23 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads in worldfile data to MapRegions
// o------------------------------------------------------------------------------------------------o
void CMapHandler::Load(void) {
    const std::int16_t AreaX = UpperX / 8; // we're storing 8x8 grid arrays together
    const std::int16_t AreaY = UpperY / 8;
    std::uint32_t count = 0;
    std::ifstream readDestination;
    Console::shared().TurnYellow();
    Console::shared() << "0%";
    std::uint32_t s_t = GetClock();
    std::string basePath = cwmWorldState->ServerData()->Directory(CSDDP_SHARED);
    std::string filename;
    
    std::uint32_t runningCount = 0;
    std::int32_t fileSizes[AreaX][AreaY];
    for (std::int16_t cx = 0; cx < AreaX; ++cx) {
        for (std::int16_t cy = 0; cy < AreaY; ++cy) {
            filename = basePath + util::ntos(cx) + std::string(".") + util::ntos(cy) +
            std::string(".wsc"); // let's name our file
            fileSizes[cx][cy] = FileSize(filename);
            runningCount += fileSizes[cx][cy];
        }
    }
    
    if (runningCount == 0) {
        runningCount = 1;
    }
    
    std::int32_t runningDone = 0;
    for (std::int16_t counter1 = 0; counter1 < AreaX; ++counter1) // move left->right
    {
        for (std::int16_t counter2 = 0; counter2 < AreaY; ++counter2) // move up->down
        {
            filename = basePath + util::ntos(counter1) + std::string(".") + util::ntos(counter2) +
            std::string(".wsc");         // let's name our file
            readDestination.open(filename.c_str()); // let's open it
            readDestination.seekg(0, std::ios::beg);
            
            if (readDestination.eof() || readDestination.fail()) {
                readDestination.close();
                readDestination.clear();
                continue;
            }
            
            ++count;
            LoadFromDisk(readDestination, runningDone, fileSizes[counter1][counter2], runningCount);
            
            runningDone += fileSizes[counter1][counter2];
            float tempVal = static_cast<R32>(runningDone) / static_cast<R32>(runningCount) * 100.0f;
            if (tempVal <= 10) {
                Console::shared() << "\b\b" << static_cast<std::uint32_t>(tempVal) << "%";
            }
            else if (tempVal <= 100) {
                Console::shared() << "\b\b\b" << static_cast<std::uint32_t>(tempVal) << "%";
            }
            
            readDestination.close();
            readDestination.clear();
        }
    }
    
    // If runningDone is still 0, there was nothing to load! 100% it
    if (runningDone == 0) {
        Console::shared() << "\b\b" << static_cast<std::uint32_t>(100) << "%";
    }
    
    Console::shared().TurnNormal();
    Console::shared() << "\b\b\b";
    Console::shared().PrintDone();
    
    filename = basePath + "overflow.wsc";
    std::ifstream flowDestination(filename.c_str());
    LoadFromDisk(flowDestination, -1, -1, -1);
    flowDestination.close();
    
    filename = basePath + "house.wsc";
    std::ifstream houseDestination(filename.c_str());
    LoadFromDisk(houseDestination, -1, -1, -1);
    
    std::uint32_t b = 0;
    ObjectFactory::shared().IterateOver(CBaseObject::OT_MULTI, b, nullptr, &PostLoadFunctor);
    ObjectFactory::shared().IterateOver(CBaseObject::OT_ITEM, b, nullptr, &PostLoadFunctor);
    ObjectFactory::shared().IterateOver(CBaseObject::OT_CHAR, b, nullptr, &PostLoadFunctor);
    houseDestination.close();
    
    std::uint32_t e_t = GetClock();
    Console::shared().Print(
                            util::format("ASCII world loaded in %.02fsec\n", (static_cast<R32>(e_t - s_t)) / 1000.0f));
    
    std::uint8_t i = 0;
    for (WORLDLIST_ITERATOR wIter = mapWorlds.begin(); wIter != mapWorlds.end(); ++wIter) {
        (*wIter)->LoadResources(i);
        ++i;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CMapHandler::LoadFromDisk()
//|	Date		-	23 July, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads in objects from specified file
// o------------------------------------------------------------------------------------------------o
void CMapHandler::LoadFromDisk(std::istream &readDestination, std::int32_t baseValue, std::int32_t fileSize,
                               std::uint32_t maxSize) {
    char line[1024];
    R32 basePercent = static_cast<R32>(baseValue) / static_cast<R32>(maxSize) * 100.0f;
    R32 targPercent = static_cast<R32>(baseValue + fileSize) / static_cast<R32>(maxSize) * 100.0f;
    R32 diffValue = targPercent - basePercent;
    
    std::int32_t updateCount = 0;
    while (!readDestination.eof() && !readDestination.fail()) {
        readDestination.getline(line, 1023);
        line[readDestination.gcount()] = 0;
        std::string sLine(line);
        sLine = util::trim(sLine);
        
        if (sLine.substr(0, 1) == "[") // in a section
        {
            sLine = sLine.substr(1, sLine.size() - 2);
            sLine = util::upper(util::trim(sLine));
            if (sLine == "CHARACTER") {
                LoadChar(readDestination);
            }
            else if (sLine == "ITEM") {
                LoadItem(readDestination);
            }
            else if (sLine == "HOUSE") {
                LoadMulti(readDestination);
            }
            else if (sLine == "BOAT") {
                LoadBoat(readDestination);
            }
            else if (sLine == "SPAWNITEM") {
                LoadSpawnItem(readDestination);
            }
            
            if (fileSize != -1 && (++updateCount) % 200 == 0) {
                R32 curPos = readDestination.tellg();
                R32 tempVal = basePercent + (curPos / fileSize * diffValue);
                if (tempVal <= 10) {
                    Console::shared() << "\b\b" << static_cast<std::uint32_t>(tempVal) << "%";
                }
                else {
                    Console::shared() << "\b\b\b" << static_cast<std::uint32_t>(tempVal) << "%";
                }
            }
        }
        else if (sLine == "---REGION---") // end of region
            continue;
    }
}
