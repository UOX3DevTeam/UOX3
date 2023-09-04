#ifndef __SpawnRegion__
#define __SpawnRegion__

#include <string>
#include <vector>

#include "cbaseobject.h"
#include "genericlist.h"
#include "typedefs.h"

class CScriptSection;
class CChar;
class CItem;

class CSpawnRegion // Regionspawns
{
private:
    std::string name; // Any Name to show up when this region is spawned [512]
    
    std::vector<std::string> sNpcs;  // Individual Npcs
    std::vector<std::string> sItems; // Individual Items
    
    std::uint16_t regionNum; // Region Number
    
    // These two will replace maxspawn
    size_t maxCharSpawn; // Maximum amount of characters to spawn
    size_t maxItemSpawn; // Maximum amount of items to spawn
    
    // These two will replace current
    std::int32_t curCharSpawn; // Current amount of spawned characters
    std::int32_t curItemSpawn; // Current amount of spawned items
    
    std::uint8_t minTime;      // Minimum spawn time
    std::uint8_t maxTime;      // Maximum spawn time
    timerval_t nextTime; // Nextspawn time for this region
    
    // Box values
    std::int16_t x1; // Top left X
    std::int16_t x2; // Bottom right x
    std::int16_t y1; // Top left y
    std::int16_t y2; // Bottom right y
    
    std::int8_t prefZ; // Maximum Z influence static and dynamic items can have on spawning.
    std::int8_t defZ;  // Definite Z to attempt to spawn object at
    
    bool onlyOutside; // Should Chars, Items only spawn outside of buildings
    bool isSpawner;   // Whether the items spawned are spawner objects or not
    std::uint16_t call;        // # of times that an NPC or Item is spawned from a list
    std::uint8_t worldNumber; // which world are we spawning in?
    std::uint16_t instanceId;  // Which instance are we spawning in?
    
    GenericList<CChar *> spawnedChars;
    GenericList<CItem *> spawnedItems;
    std::map<std::uint32_t, std::int8_t> validLandPosCheck;
    std::map<std::uint32_t, std::int8_t> validWaterPosCheck;
    std::vector<Point3_st> validLandPos;
    std::vector<Point3_st> validWaterPos;
    
public:
    CSpawnRegion(std::uint16_t spawnregion);
    ~CSpawnRegion();
    
    void Load(CScriptSection *toScan);
    void DoRegionSpawn(std::uint32_t &itemsSpawned, std::uint32_t &npcsSpawned);
    
    const std::string GetName(void) const;
    std::uint16_t GetRegionNum(void) const;
    size_t GetMaxSpawn(void) const;
    size_t GetMaxCharSpawn(void) const;
    size_t GetMaxItemSpawn(void) const;
    std::int32_t GetCurrent(void) const;
    std::int32_t GetCurrentCharAmt(void) const;
    std::int32_t GetCurrentItemAmt(void) const;
    std::uint8_t GetMinTime(void) const;
    std::uint8_t GetMaxTime(void) const;
    timerval_t GetNextTime(void) const;
    std::int16_t GetX1(void) const;
    std::int16_t GetY1(void) const;
    std::int16_t GetX2(void) const;
    std::int16_t GetY2(void) const;
    std::int8_t GetDefZ(void) const;
    std::int8_t GetPrefZ(void) const;
    std::uint16_t GetCall(void) const;
    bool GetOnlyOutside(void) const;
    bool IsSpawner(void) const;
    auto GetNPC(void) const -> std::vector<std::string>;
    auto GetItem(void) const -> std::vector<std::string>;
    
    void SetName(const std::string &newName);
    void SetRegionNum(std::uint16_t newVal);
    void SetMaxCharSpawn(size_t newVal);
    void SetMaxItemSpawn(size_t newVal);
    void IncCurrentCharAmt(std::int16_t incAmt = 1);
    void IncCurrentItemAmt(std::int16_t incAmt = 1);
    void SetMinTime(std::uint8_t newVal);
    void SetMaxTime(std::uint8_t newVal);
    void SetNextTime(timerval_t newVal);
    void SetX1(std::int16_t newVal);
    void SetY1(std::int16_t newVal);
    void SetX2(std::int16_t newVal);
    void SetY2(std::int16_t newVal);
    void SetDefZ(std::int8_t newVal);
    void SetPrefZ(std::int8_t newVal);
    void SetCall(std::uint16_t newVal);
    void SetOnlyOutside(bool newVal);
    void IsSpawner(bool newVal);
    void SetNPC(const std::string &newVal);
    void SetNPCList(std::string newVal);
    void SetItem(const std::string &newVal);
    void SetItemList(std::string newVal);
    
    std::uint16_t GetInstanceId(void) const;
    void SetInstanceId(std::uint16_t newVal);
    
    std::uint8_t WorldNumber(void) const;
    void WorldNumber(std::uint8_t newVal);
    
    void CheckSpawned();
    void DeleteSpawnedChar(CChar *toDelete);
    void DeleteSpawnedItem(CItem *toDelete);
    
    GenericList<CItem *> *GetSpawnedItemsList(void);
    GenericList<CChar *> *GetSpawnedCharsList(void);
    
private:
    auto RegionSpawnChar() -> CChar *;
    auto RegionSpawnItem() -> CItem *;
    
    bool FindItemSpotToSpawn(std::int16_t &x, std::int16_t &y, std::int8_t &z);
    bool FindCharSpotToSpawn(std::int16_t &x, std::int16_t &y, std::int8_t &z, bool &waterCreature, bool &amphiCreature);
    
    void LoadNPCList(const std::string &npcList);
    void LoadItemList(const std::string &itemList);
};
#endif
