// o------------------------------------------------------------------------------------------------o
//|	File		-	regions.h
// o------------------------------------------------------------------------------------------------o
//|	Notes		-	Region class  Added 3/6/1999 To lookup items by region
//|						This should help for now. BTW- my first attempt at
// C++ |						So forgive any newbie mistakes :) |
// Side note, I wanted regions to be more generic, but
//|						now that I have to do this, time doesn't allow for
//it yet.
// o------------------------------------------------------------------------------------------------o

#ifndef __Region_h
#define __Region_h
#include <cstdint>
#include <string>
#include <vector>

#include "genericlist.h"
#include "typedefs.h"

class CItem;
class CChar;
class CBaseObject;
class RegionSerialList;

const std::int16_t MapColSize = 32;
const std::int16_t MapRowSize = 128;

const std::int16_t UpperX = static_cast<std::int16_t>(7168 / MapColSize);
const std::int16_t UpperY = static_cast<std::int16_t>(4096 / MapRowSize);

struct MapResource_st {
    std::int16_t oreAmt;
    std::uint32_t oreTime;
    std::int16_t logAmt;
    std::uint32_t logTime;
    std::int16_t fishAmt;
    std::uint32_t fishTime;
    
    MapResource_st(std::int16_t defOre = 0, std::int16_t defLog = 0, std::int16_t defFish = 0, std::uint32_t defOreTime = 0, std::uint32_t defLogTIme = 0, std::uint32_t defFishTIme = 0) : oreAmt(defOre), oreTime(defOreTime), logAmt(defLog), logTime(defLogTIme), fishAmt(defFish), fishTime(defFishTIme) {}
};

class CMapRegion {
private:
    GenericList<CItem *> itemData;
    GenericList<CChar *> charData;
    RegionSerialList regionSerialData;
    bool hasRegionChanged = false;
    
public:
    GenericList<CItem *> *GetItemList();
    GenericList<CChar *> *GetCharList();
    RegionSerialList *GetRegionSerialList();
    bool HasRegionChanged();
    void HasRegionChanged(bool newVal);
    
    CMapRegion(){};
    ~CMapRegion(){};
    
    void SaveToDisk(std::ostream &writeDestination);
};

class CMapWorld {
private:
    std::int16_t upperArrayX;
    std::int16_t upperArrayY;
    std::uint16_t resourceX;
    std::uint16_t resourceY;
    std::vector<CMapRegion> mapRegions;
    std::vector<MapResource_st> mapResources;
    
public:
    CMapWorld();
    CMapWorld(std::uint8_t worldNum);
    ~CMapWorld();
    
    CMapRegion *GetMapRegion(std::int16_t xOffset, std::int16_t yOffset);
    std::vector<CMapRegion> *GetMapRegions();
    
    MapResource_st &GetResource(std::int16_t x, std::int16_t y);
    
    void LoadResources(std::uint8_t worldNum);
    void SaveResources(std::uint8_t worldNUm);
};

class CMapHandler {
private:
    typedef std::vector<CMapWorld *> WORLDLIST;
    typedef std::vector<CMapWorld *>::iterator WORLDLIST_ITERATOR;
    
    WORLDLIST mapWorlds;
    CMapRegion overFlow;
    
    void LoadFromDisk(std::istream &readDestination, std::int32_t baseValue, std::int32_t fileSize, std::uint32_t maxSize);
    
public:
    CMapHandler() = default;
    ~CMapHandler();
    auto startup() -> void;
    
    void Save();
    void load();
    
    bool AddItem(CItem *nItem);
    bool RemoveItem(CItem *nItem);
    
    bool AddChar(CChar *toAdd);
    bool RemoveChar(CChar *toRemove);
    
    bool ChangeRegion(CItem *nItem, std::int16_t x, std::int16_t y, std::uint8_t worldNum);
    bool ChangeRegion(CChar *nChar, std::int16_t x, std::int16_t y, std::uint8_t worldNum);
    
    CMapRegion *GetMapRegion(CBaseObject *mObj);
    CMapRegion *GetMapRegion(std::int16_t xOffset, std::int16_t yOffset, std::uint8_t worldNumber);
    
    std::int16_t GetGridX(std::int16_t x);
    std::int16_t GetGridY(std::int16_t y);
    
    auto PopulateList(std::int16_t x, std::int16_t y, std::uint8_t worldNumber) -> std::vector<CMapRegion *>;
    auto PopulateList(CBaseObject *mObj) -> std::vector<CMapRegion *>;
    
    CMapWorld *GetMapWorld(std::uint8_t worldNum);
    
    MapResource_st *GetResource(std::int16_t x, std::int16_t y, std::uint8_t worldNum);
};

extern CMapHandler *MapRegion;

#endif
