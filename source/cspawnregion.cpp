#include "cspawnregion.h"

#include <iostream>
#include <regex>
#include <sstream>

#include "cchar.h"
#include "citem.h"
#include "classes.h"
#include "cserverdefinitions.h"

#include "dictionary.h"
#include "funcdecl.h"
#include "utility/random.hpp"
#include "configuration/serverconfig.hpp"
#include "ssection.h"
#include "stringutility.hpp"
#include "utility/strutil.hpp"
#include "uodata/uomgr.hpp"
#include "uodata/uoxuoadapter.hpp"

using Random = effolkronium::random_static;

extern CDictionaryContainer worldDictionary ;
extern WorldItem worldItem ;
extern CCharStuff worldNPC ;
extern CServerDefinitions worldFileLookup ;
extern uo::UOMgr uoManager ;

using namespace std::string_literals;

const timerval_t DEFSPAWN_NEXTTIME = 0;
const std::uint16_t DEFSPAWN_CALL = 1;
const std::int16_t DEFSPAWN_X1 = 0;
const std::int16_t DEFSPAWN_X2 = 0;
const std::int16_t DEFSPAWN_Y1 = 0;
const std::int16_t DEFSPAWN_Y2 = 0;
const size_t DEFSPAWN_MAXCSPAWN = 0;
const size_t DEFSPAWN_MAXISPAWN = 0;
const std::uint8_t DEFSPAWN_MAXTIME = 0;
const std::uint8_t DEFSPAWN_MINTIME = 0;
const std::int32_t DEFSPAWN_CURCSPAWN = 0;
const std::int32_t DEFSPAWN_CURISPAWN = 0;
const std::uint8_t DEFSPAWN_WORLDNUM = 0;
const std::int8_t DEFSPAWN_PREFZ = 18;
const std::int8_t DEFSPAWN_DEFZ = ILLEGAL_Z;
const bool DEFSPAWN_ONLYOUTSIDE = false;
const bool DEFSPAWN_ISSPAWNER = false;

// o------------------------------------------------------------------------------------------------o
//|	Class		-	CSpawnRegion()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Constructor for CSpawnRegion class
// o------------------------------------------------------------------------------------------------o
CSpawnRegion::CSpawnRegion(std::uint16_t spawnregion)
: regionNum(spawnregion), maxCharSpawn(DEFSPAWN_MAXCSPAWN), maxItemSpawn(DEFSPAWN_MAXISPAWN),
curCharSpawn(DEFSPAWN_CURCSPAWN), curItemSpawn(DEFSPAWN_CURISPAWN), minTime(DEFSPAWN_MINTIME),
maxTime(DEFSPAWN_MAXTIME), nextTime(DEFSPAWN_NEXTTIME), x1(DEFSPAWN_X1), x2(DEFSPAWN_X2),
y1(DEFSPAWN_Y1), y2(DEFSPAWN_Y2), prefZ(DEFSPAWN_PREFZ), defZ(DEFSPAWN_DEFZ),
onlyOutside(DEFSPAWN_ONLYOUTSIDE), isSpawner(DEFSPAWN_ISSPAWNER), call(DEFSPAWN_CALL),
worldNumber(DEFSPAWN_WORLDNUM) {
    sItems.resize(0);
    sNpcs.resize(0);
    name = worldDictionary.GetEntry(1117);
    // note: doesn't go here, but i'll see it here.  when an item is spawned, as soon as it's moved
    // it needs to lose it's spawn setting.  If not, then when people pick up spawned items, they
    // will disappear (on region spawns)
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::~CSpawnRegion()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Deconstructor for CSpawnRegion class
// o------------------------------------------------------------------------------------------------o
CSpawnRegion::~CSpawnRegion() {
    sItems.resize(0);
    sNpcs.resize(0);
    // Wipe out all items and npcs
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetName()
//|					CSpawnRegion::SetName()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets spawn region's name
// o------------------------------------------------------------------------------------------------o
const std::string CSpawnRegion::GetName() const { return name; }
void CSpawnRegion::SetName(const std::string &newName) { name = newName; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::MaxSpawn()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Maximum amount of objects a region can spawn
// o------------------------------------------------------------------------------------------------o
size_t CSpawnRegion::GetMaxSpawn() const { return (maxCharSpawn + maxItemSpawn); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetMaxCharSpawn()
//|					CSpawnRegion::SetMaxCharSpawn()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets maximum amount of characters to spawn
// o------------------------------------------------------------------------------------------------o
size_t CSpawnRegion::GetMaxCharSpawn() const { return maxCharSpawn; }
void CSpawnRegion::SetMaxCharSpawn(size_t newVal) { maxCharSpawn = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetMaxItemSpawn()
//|					CSpawnRegion::SetMaxItemSpawn()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets maximum amount of items to spawn
// o------------------------------------------------------------------------------------------------o
size_t CSpawnRegion::GetMaxItemSpawn() const { return maxItemSpawn; }
void CSpawnRegion::SetMaxItemSpawn(size_t newVal) { maxItemSpawn = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::Current()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Current amount of objects spawned
// o------------------------------------------------------------------------------------------------o
std::int32_t CSpawnRegion::GetCurrent() const { return (curCharSpawn + curItemSpawn); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetCurrentCharAmt()
//|					CSpawnRegion::IncCurrentCharAmt()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets current amount of characters spawned
// o------------------------------------------------------------------------------------------------o
std::int32_t CSpawnRegion::GetCurrentCharAmt() const { return curCharSpawn; }
void CSpawnRegion::IncCurrentCharAmt(std::int16_t incAmt) { curCharSpawn += incAmt; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetCurrentItemAmt()
//|					CSpawnRegion::IncCurrentItemAmt()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets current amount of items spawned
// o------------------------------------------------------------------------------------------------o
std::int32_t CSpawnRegion::GetCurrentItemAmt() const { return curItemSpawn; }
void CSpawnRegion::IncCurrentItemAmt(std::int16_t incAmt) { curItemSpawn += incAmt; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetRegionNum()
//|					CSpawnRegion::SetRegionNum()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets spawn region number
// o------------------------------------------------------------------------------------------------o
std::uint16_t CSpawnRegion::GetRegionNum() const { return regionNum; }
void CSpawnRegion::SetRegionNum(std::uint16_t newVal) { regionNum = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetMinTime()
//|					CSpawnRegion::SetMinTime()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets minimum amount of time to pass before a spawnregion
//|					spawns a new object
// o------------------------------------------------------------------------------------------------o
std::uint8_t CSpawnRegion::GetMinTime() const { return minTime; }
void CSpawnRegion::SetMinTime(std::uint8_t newVal) { minTime = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetMaxTime()
//|					CSpawnRegion::SetMaxTime()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets maximum amount of time to pass before a spawnregion
//|					spawns a new object
// o------------------------------------------------------------------------------------------------o
std::uint8_t CSpawnRegion::GetMaxTime() const { return maxTime; }
void CSpawnRegion::SetMaxTime(std::uint8_t newVal) { maxTime = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetNextTime()
//|					CSpawnRegion::SetNextTime()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets time for when a spawnregion will next spawn a new object
// o------------------------------------------------------------------------------------------------o
timerval_t CSpawnRegion::GetNextTime() const { return nextTime; }
void CSpawnRegion::SetNextTime(timerval_t newVal) { nextTime = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::SetPrefZ()
//|					CSpawnRegion::GetPrefZ()
//|	Date		-	04/22/2002
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets Z Level of the Spawn Region
// o------------------------------------------------------------------------------------------------o
void CSpawnRegion::SetPrefZ(std::int8_t newVal) { prefZ = newVal; }
std::int8_t CSpawnRegion::GetPrefZ() const { return prefZ; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::SetDefZ()
//|					CSpawnRegion::GetDefZ()
//|	Date		-	04/22/2002
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets definitive Z Level of the Spawn Region at which to attempt
// spawning objects
// o------------------------------------------------------------------------------------------------o
void CSpawnRegion::SetDefZ(std::int8_t newVal) { defZ = newVal; }
std::int8_t CSpawnRegion::GetDefZ() const { return defZ; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetY1()
//|					CSpawnRegion::SetY1()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the y pos of the top corner of the spawnregion
// o------------------------------------------------------------------------------------------------o
std::int16_t CSpawnRegion::GetY1() const { return y1; }
void CSpawnRegion::SetY1(std::int16_t newVal) { y1 = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetX1()
//|					CSpawnRegion::SetX1()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the x pos of the top corner of the spawnregion
// o------------------------------------------------------------------------------------------------o
std::int16_t CSpawnRegion::GetX1() const { return x1; }
void CSpawnRegion::SetX1(std::int16_t newVal) { x1 = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetY2()
//|					CSpawnRegion::SetY2()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the y pos of the bottom corner of the spawn region
// o------------------------------------------------------------------------------------------------o
std::int16_t CSpawnRegion::GetY2() const { return y2; }
void CSpawnRegion::SetY2(std::int16_t newVal) { y2 = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetX2()
//|					CSpawnRegion::SetX2()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the x pos of the bottom corner of the spawn region
// o------------------------------------------------------------------------------------------------o
std::int16_t CSpawnRegion::GetX2() const { return x2; }
void CSpawnRegion::SetX2(std::int16_t newVal) { x2 = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::WorldNumber()
//|					CSpawnRegion::WorldNumber()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the world number of the spawnregion
// o------------------------------------------------------------------------------------------------o
std::uint8_t CSpawnRegion::WorldNumber() const { return worldNumber; }
void CSpawnRegion::WorldNumber(std::uint8_t newVal) { worldNumber = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetInstanceId()
//|					CSpawnRegion::SetInstanceId()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the instanceId of the spawnregion
// o------------------------------------------------------------------------------------------------o
std::uint16_t CSpawnRegion::GetInstanceId() const { return instanceId; }
void CSpawnRegion::SetInstanceId(std::uint16_t newVal) { instanceId = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetOnlyOutside()
//|					CSpawnRegion::SetOnlyOutside()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether NPCs should only spawn outside buildings
// o------------------------------------------------------------------------------------------------o
bool CSpawnRegion::GetOnlyOutside() const { return onlyOutside; }
void CSpawnRegion::SetOnlyOutside(bool newVal) { onlyOutside = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::IsSpawner()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether items spawned from spawnregion is a spawner or not
// o------------------------------------------------------------------------------------------------o
bool CSpawnRegion::IsSpawner() const { return isSpawner; }
void CSpawnRegion::IsSpawner(bool newVal) { isSpawner = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetCall()
//|					CSpawnRegion::SetCall()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets number of objects that should be spawned in each call for
// a spawnregion
// o------------------------------------------------------------------------------------------------o
std::uint16_t CSpawnRegion::GetCall() const { return call; }
void CSpawnRegion::SetCall(std::uint16_t newVal) { call = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetNPC()
//|					CSpawnRegion::SetNPC()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets stringlist of individual NPCs to spawn in a spawnregion
// o------------------------------------------------------------------------------------------------o
//  WHy isn't this returning a const reference?
auto CSpawnRegion::GetNPC() const -> std::vector<std::string> { return sNpcs; }
// Believe this was a bug, it was taking a string?
// if it is, then it gets cleared each time an entry is set,
// So it can only have one entry, if you sue SetNPC
// DANGER DANGER WILL ROBINSON
// [REPLY TO ABOVE COMMENT]: Working as intended, so the spawn section of a spawn region gets
// overwritten when spawn regions are modified and then reloaded
void CSpawnRegion::SetNPC(const std::string &newVal) {
    // Clear old entries to make room for new ones
    sNpcs.clear();
    sNpcs.push_back(newVal);
}
void CSpawnRegion::SetNPCList(std::string newVal) {
    // Clear old entries to make room for new ones
    sNpcs.clear();
    
    // Check if a comma delimited string was provided
    if (newVal.find(",") != std::string::npos) {
        // Strip whitespace from string
        std::regex r("\\s+");
        newVal = std::regex_replace(newVal, r, "");
        
        // Add section of the string to the sNpcs list with the help of a stringstream
        std::stringstream s_stream(newVal);
        while (s_stream.good()) {
            std::string substr;
            getline(s_stream, substr, ',');
            sNpcs.push_back(substr);
        }
    }
    else {
        // Assume an actual NPClist was provided
        LoadNPCList(newVal);
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetItem()
//|					CSpawnRegion::SetItem()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets stringlist of individual Items to spawn in a spawnregion
// o------------------------------------------------------------------------------------------------o
//  THIS SHOULD BE A reference return! If you need to handle  const/non const, include both types
//  Just changing all this wojuld prevent copys being made each time it is called!
//  Need to change the header as well of course
//  example
/*
 auto CSpawnRegion::GetItem() const -> const std::vector<std::string> &
 {
 return sItems;
 }
 auto CSpawnRegion::GetItem() -> std::vector<std::string> &
 {
 return sItems;
 }
 */
auto CSpawnRegion::GetItem() const -> std::vector<std::string> { return sItems; }
void CSpawnRegion::SetItem(const std::string &newVal) {
    // Clear old entries to make room for new ones
    sItems.clear();
    sItems.push_back(newVal);
}
void CSpawnRegion::SetItemList(std::string newVal) {
    // Clear old entries to make room for new ones
    sItems.clear();
    
    // Check if a comma delimited string was provided
    if (newVal.find(",") != std::string::npos) {
        // Strip whitespace from string
        std::regex r("\\s+");
        newVal = std::regex_replace(newVal, r, "");
        
        // Add section of the string to the sItems list with the help of a stringstream
        std::stringstream s_stream(newVal);
        while (s_stream.good()) {
            std::string substr;
            getline(s_stream, substr, ',');
            sItems.push_back(substr);
        }
    }
    else {
        // Assume an actual ItemList was provided
        LoadItemList(newVal);
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::LoadNPCList()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads NPCLIST from DFNs with list of NPCs to spawn for a spawnregion
// o------------------------------------------------------------------------------------------------o
void CSpawnRegion::LoadNPCList(const std::string &npcList) {
    std::string sect = "NPCLIST " + npcList;
    CScriptSection *CharList = worldFileLookup.FindEntry(sect, npc_def);
    if (CharList != nullptr) {
        for (std::string npc = CharList->First(); !CharList->AtEnd(); npc = CharList->Next()) {
            if (util::upper(npc) == "NPCLIST") {
                LoadNPCList(CharList->GrabData());
            }
            else {
                sNpcs.push_back(npc);
            }
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::LoadItemList()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads ITEMLIST from DFNs with list of Items to spawn for a
// spawnregion
// o------------------------------------------------------------------------------------------------o
void CSpawnRegion::LoadItemList(const std::string &itemList) {
    std::string sect = "ITEMLIST " + itemList;
    CScriptSection *ItemList = worldFileLookup.FindEntry(sect, items_def);
    if (ItemList != nullptr) {
        for (std::string itm = ItemList->First(); !ItemList->AtEnd(); itm = ItemList->Next()) {
            if (util::upper(itm) == "ITEMLIST") {
                LoadItemList(ItemList->GrabData());
            }
            else {
                sItems.push_back(itm);
            }
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::load()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads the spawnregion from spawn.dfn script entry
// o------------------------------------------------------------------------------------------------o
void CSpawnRegion::load(CScriptSection *toScan) {
    std::string sect;
    std::string data;
    std::string UTag;
    
    for (std::string tag = toScan->First(); !toScan->AtEnd(); tag = toScan->Next()) {
        if (!tag.empty()) {
            UTag = util::upper(tag);
            data = toScan->GrabData();
            
            // Default to instanceId 0, in case nothing else is specified in DFN
            SetInstanceId(0);
            
            if (UTag == "NPCLIST") {
                LoadNPCList(data);
            }
            else if (UTag == "NPC") {
                sNpcs.push_back(data);
            }
            else if (UTag == "ITEMLIST") {
                LoadItemList(data);
            }
            else if (UTag == "ITEM") {
                sItems.push_back(data);
            }
            else if (UTag == "ISSPAWNER") {
                isSpawner = (static_cast<std::int8_t>(std::stoi(data, nullptr, 0)) == 1);
            }
            else if (UTag == "MAXITEMS") {
                maxItemSpawn = static_cast<std::uint32_t>(std::stoul(data, nullptr, 0));
            }
            else if (UTag == "MAXNPCS") {
                maxCharSpawn = static_cast<std::uint32_t>(std::stoul(data, nullptr, 0));
            }
            else if (UTag == "X1") {
                x1 = static_cast<std::int16_t>(std::stoi(data, nullptr, 0));
            }
            else if (UTag == "X2") {
                x2 = static_cast<std::int16_t>(std::stoi(data, nullptr, 0));
            }
            else if (UTag == "Y1") {
                y1 = static_cast<std::int16_t>(std::stoi(data, nullptr, 0));
            }
            else if (UTag == "Y2") {
                y2 = static_cast<std::int16_t>(std::stoi(data, nullptr, 0));
            }
            else if (UTag == "MINTIME") {
                minTime = static_cast<std::uint8_t>(std::stoul(data, nullptr, 0));
            }
            else if (UTag == "MAXTIME") {
                maxTime = static_cast<std::uint8_t>(std::stoul(data, nullptr, 0));
            }
            else if (UTag == "NAME") {
                name = data;
            }
            else if (UTag == "CALL") {
                call = static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
            }
            else if (UTag == "WORLD") {
                worldNumber = static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
            }
            else if (UTag == "INSTANCEID") {
                instanceId = static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
            }
            else if (UTag == "DEFZ") {
                defZ = static_cast<std::int8_t>(std::stoi(data, nullptr, 0));
            }
            else if (UTag == "PREFZ") {
                prefZ = static_cast<std::int8_t>(std::stoi(data, nullptr, 0));
            }
            else if (UTag == "ONLYOUTSIDE") {
                onlyOutside = (static_cast<std::int8_t>(std::stoi(data, nullptr, 0)) == 1);
            }
            else if (UTag == "VALIDLANDPOS") {
                data = util::simplify(data);
                auto csecs = oldstrutil::sections(data, ",");
                if (csecs.size() == 3) {
                    validLandPos.push_back(Point3(static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(csecs[0], "//")), nullptr, 0)), static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(csecs[1], "//")), nullptr, 0)), static_cast<std::uint8_t>(std::stoul(util::trim(util::strip(csecs[0], "//")), nullptr, 0))));
                    validLandPosCheck[static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(csecs[1], "//")), nullptr, 0)) + (static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(csecs[0], "//")), nullptr, 0)) << 16)] = static_cast<std::uint8_t>(std::stoul(util::trim(util::strip(csecs[2], "//")), nullptr, 0));
                }
            }
            else if (UTag == "VALIDWATERPOS") {
                data = util::simplify(data);
                auto csecs = oldstrutil::sections(data, ",");
                if (csecs.size() == 3) {
                    validWaterPos.push_back(Point3(static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(csecs[0], "//")), nullptr, 0)), static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(csecs[1], "//")), nullptr, 0)), static_cast<std::uint8_t>(std::stoul(util::trim(util::strip(csecs[0], "//")), nullptr, 0))));
                    validWaterPosCheck[static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(csecs[1], "//")), nullptr, 0)) + (static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(csecs[0], "//")), nullptr, 0)) << 16)] = static_cast<std::uint8_t>(std::stoul(util::trim(util::strip(csecs[2], "//")), nullptr, 0));
                }
            }
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::DoRegionSpawn()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Do spawn stuff for spawnregion
// o------------------------------------------------------------------------------------------------o
void CSpawnRegion::DoRegionSpawn(std::uint32_t &itemsSpawned, std::uint32_t &npcsSpawned) {
    // Only perform the region spawn if the spawn region in question is active
    
    if (!ServerConfig::shared().spawnFacet.test(static_cast<std::uint32_t>(WorldNumber()))){
        return;
    }
    
    if (sNpcs.empty()) {
        maxCharSpawn = 0;
    }
    if (sItems.empty()) {
        maxItemSpawn = 0;
    }
    
    bool shouldSpawnChars = (!sNpcs.empty() && maxCharSpawn > spawnedChars.Num());
    bool shouldSpawnItems = (!sItems.empty() && maxItemSpawn > spawnedItems.Num());
    if (shouldSpawnChars || shouldSpawnItems) {
        CChar *spawnChar = nullptr;
        CItem *spawnItem = nullptr;
        const std::uint8_t spawnChars = (shouldSpawnChars ? 0 : 50);
        const std::uint8_t spawnItems = (shouldSpawnItems ? 100 : 49);
        for (std::uint16_t i = 0; i < call && (shouldSpawnItems || shouldSpawnChars); ++i) {
            if (Random::get(static_cast<std::uint16_t>(spawnChars), static_cast<std::uint16_t>(spawnItems)) > 49) {
                if (shouldSpawnItems) {
                    spawnItem = RegionSpawnItem();
                    if (ValidateObject(spawnItem)) {
                        spawnedItems.Add(spawnItem);
                        ++itemsSpawned;
                        shouldSpawnItems = (spawnedItems.Num() < maxItemSpawn);
                    }
                }
            }
            else {
                if (shouldSpawnChars) {
                    spawnChar = RegionSpawnChar();
                    if (ValidateObject(spawnChar)) {
                        spawnedChars.Add(spawnChar);
                        ++npcsSpawned;
                        shouldSpawnChars = (spawnedChars.Num() < maxCharSpawn);
                    }
                }
            }
        }
    }
    
    SetNextTime(BuildTimeValue(static_cast<float>(Random::get(static_cast<std::uint16_t>(GetMinTime() * 60), static_cast<std::uint16_t>(GetMaxTime() * 60)))));
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::RegionSpawnChar()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Do a char spawn
// o------------------------------------------------------------------------------------------------o
auto CSpawnRegion::RegionSpawnChar() -> CChar * {
    // Stuff each NPC entry into a vector
    std::vector<std::pair<std::string, std::uint16_t>> npcListVector;
    for (size_t i = 0; i < sNpcs.size(); i++) {
        // Split string for entry into a stringlist based on | as separator
        auto csecs = oldstrutil::sections(util::trim(util::strip(sNpcs[i], "//")), "|");
        
        std::uint16_t sectionWeight = 1;
        if (csecs.size() > 1) {
            sectionWeight =
            static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(csecs[0], "//")), nullptr, 0));
        }
        
        auto npcSection = (csecs.size() > 1 ? csecs[1] : csecs[0]);
        npcListVector.emplace_back(npcSection, sectionWeight);
    }
    
    auto ourNPC = worldNPC.ChooseNpcToCreate(npcListVector);
    if (ourNPC.empty())
        return nullptr;
    
    auto csecs = oldstrutil::sections(util::trim(util::strip(ourNPC, "//")), "=");
    if (util::upper(csecs[0]) == "NPCLIST") {
        // Chosen entry contained another NPCLIST! Let's dive back into it...
        ourNPC = worldNPC.NpcListLookup(ourNPC);
    }
    
    CScriptSection *npcCreate = worldFileLookup.FindEntry(ourNPC, npc_def);
    if (npcCreate == nullptr)
        return nullptr;
    
    std::string cdata;
    std::int32_t ndata = -1, odata = -1;
    std::uint16_t npcId = 0;
    
    for (auto tag = npcCreate->FirstTag(); !npcCreate->AtEndTags(); tag = npcCreate->NextTag()) {
        cdata = npcCreate->GrabData(ndata, odata);
        switch (tag) {
            case DFNTAG_ID:
                npcId = util::ston<std::uint16_t>(cdata); // static_cast<std::uint16_t>( ndata );
                goto foundNpcId;
            default:
                break;
        }
    }
foundNpcId:
    
    bool waterCreature = false;
    bool amphiCreature = false;
    if (npcId > 0) {
        waterCreature = worldMain.creatures[npcId].IsWater();
        amphiCreature = worldMain.creatures[npcId].IsAmphibian();
    }
    
    std::int16_t x, y;
    std::int8_t z;
    if (FindCharSpotToSpawn(x, y, z, waterCreature, amphiCreature)) {
        CChar *CSpawn = nullptr;
        CSpawn = worldNPC.CreateBaseNPC(ourNPC, false);
        
        if (CSpawn != nullptr) {
            // NPCs should always wander the whole spawnregion
            CSpawn->SetNpcWander(WT_BOX);
            CSpawn->SetFx(x1, 0);
            CSpawn->SetFx(x2, 1);
            CSpawn->SetFy(y1, 0);
            CSpawn->SetFy(y2, 1);
            CSpawn->SetLocation(x, y, z, worldNumber, instanceId);
            CSpawn->SetSpawned(true);
            CSpawn->ShouldSave(false);
            CSpawn->SetSpawn(static_cast<std::uint32_t>(regionNum));
            worldNPC.PostSpawnUpdate(CSpawn);
            IncCurrentCharAmt();
            return CSpawn;
        }
    }
    else {
        Console::shared().warning(util::format("Unable to find valid location to spawn NPC in region %i at %i,%i,%i,%i,%i", this->GetRegionNum(),x,y,z,waterCreature,amphiCreature));
    }
    return nullptr;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::RegionSpawnItem()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Do an item spawn
// o------------------------------------------------------------------------------------------------o
auto CSpawnRegion::RegionSpawnItem() -> CItem * {
    CItem *ISpawn = nullptr;
    std::int16_t x, y;
    std::int8_t z;
    if (FindItemSpotToSpawn(x, y, z)) {
        auto objType = CBaseObject::OT_ITEM;
        if (isSpawner) {
            objType = CBaseObject::OT_SPAWNER;
        }
        
        ISpawn = worldItem.CreateBaseScriptItem(nullptr, sItems[Random::get(static_cast<size_t>(0), sItems.size() - 1)], worldNumber, 1, instanceId, objType, 0xFFFF, false);
        if (ISpawn != nullptr) {
            ISpawn->SetLocation(x, y, z);
            ISpawn->SetSpawn(static_cast<std::uint32_t>(regionNum));
            ISpawn->SetSpawned(true);
            ISpawn->ShouldSave(false);
            IncCurrentItemAmt();
        }
    }
    return ISpawn;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::FindCharSpotToSpawn()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Find a random spot within a region valid for dropping an item
// o------------------------------------------------------------------------------------------------o
bool CSpawnRegion::FindCharSpotToSpawn(std::int16_t &x, std::int16_t &y, std::int8_t &z, bool &waterCreature, bool &amphiCreature) {
    bool rValue = false;
    Point3 currLoc;
    std::map<std::uint32_t, std::int8_t>::const_iterator checkValid;
    std::int8_t z2 = ILLEGAL_Z;
    const size_t landPosSize = validLandPos.size();
    const size_t waterPosSize = validWaterPos.size();
    
    // By default, let's attempt - at most - this many times to find a valid spawn point for the NPC
    std::uint8_t maxSpawnAttempts = 100;
    
    // However, if we have found some valid spawn points already, reduce the amount of attempts to
    // find MORE valid spawn points, and increase the chance of using those valid spots instead!
    // Always make some attempts to find new spots, though
    if (!waterCreature && landPosSize > 0) { // land creature
        maxSpawnAttempts = std::max(static_cast<std::uint8_t>(25), static_cast<std::uint8_t>(maxSpawnAttempts - landPosSize));
    }
    else if ((waterCreature || amphiCreature) && waterPosSize > 0) { // water or amphibian creature
        maxSpawnAttempts = std::max(static_cast<std::int16_t>(25), static_cast<std::int16_t>(maxSpawnAttempts - waterPosSize));
    }
    
    for (std::uint8_t a = 0; a < maxSpawnAttempts; ++a) {
        x = Random::get(x1, x2);
        y = Random::get(y1, y2);
        z =  uo::mapElevation(x, y, worldNumber);
        
        if (defZ != ILLEGAL_Z) {
            // Use the definite Z level defined in the spawn region
            // Trust that the spawnregion scripter knows what they are doing...
            z = defZ;
        }
        else {
            // No definite Z has been defined, look for valid dynamic Z based on prefZ
            const std::int8_t dynZ = uo::dynamicElevation(x, y, z, worldNumber, instanceId, prefZ);
            if (ILLEGAL_Z != dynZ) {
                z = dynZ;
            }
            
            // Even if we got a valid dynamic Z, there might be a better match with statics, based
            // on prefZ
            const std::int8_t staticZ = uo::staticTop(x, y, z, worldNumber, prefZ);
            if (ILLEGAL_Z != staticZ && staticZ > z) {
                z = staticZ;
            }
        }
        
        // Continue with the next spawn attempt if z is illegal
        if (z == ILLEGAL_Z)
            continue;
        
        // First go through the lists of already valid spawn locations on land,
        // and see if the chosen location has already been validated
        if (!waterCreature) {
            if (!validLandPosCheck.empty() && landPosSize > 0) {
                checkValid = validLandPosCheck.find(y + (x << 16));
                if (checkValid != validLandPosCheck.end()) {
                    z2 = (*checkValid).second;
                }
                
                if (z2 == z && z != ILLEGAL_Z) {
                    rValue = true;
                    break;
                }
            }
        }
        
        // No luck, so let's try the list of already valid water tiles instead (if NPC can move on
        // water)
        if (waterCreature || amphiCreature) {
            if (!validWaterPosCheck.empty() && waterPosSize > 0) {
                checkValid = validWaterPosCheck.find(y + (x << 16));
                if (checkValid != validWaterPosCheck.end()) {
                    z2 = (*checkValid).second;
                }
                
                if (z2 == z && z != ILLEGAL_Z) {
                    rValue = true;
                    break;
                }
            }
        }
        
        // Since our chosen location has not already been validated, lets validate it with a
        // land-based creature in mind
        if (!waterCreature && uo::validSpawnLocation(x, y, z, worldNumber, instanceId)) {
            if (onlyOutside == false || !uo::inBuilding(x, y, z, worldNumber, instanceId)) {
                if (z != ILLEGAL_Z) {
                    rValue = true;
                    validLandPos.push_back(Point3(x, y, z));
                    validLandPosCheck[y + (x << 16)] = z;
                    break;
                }
            }
        }
        
        // Otherwise, validate it with a water-based creature in mind instead
        if ((waterCreature || amphiCreature) && uo::validSpawnLocation(x, y, z, worldNumber, instanceId, false)) {
            if (onlyOutside == false || !uo::inBuilding(x, y, z, worldNumber, instanceId)) {
                rValue = true;
                validWaterPos.push_back(Point3(x, y, z));
                validWaterPosCheck[y + (x << 16)] = z;
                break;
            }
        }
    }
    
    // If we haven't found a valid location pick a random location from the stored ones
    if (!rValue && !waterCreature && !validLandPos.empty() && landPosSize > 0) {
        currLoc = validLandPos[Random::get(static_cast<size_t>(0), (landPosSize - 1))];
        x = static_cast<std::int16_t>(currLoc.x);
        y = static_cast<std::int16_t>(currLoc.y);
        z = static_cast<std::int8_t>(currLoc.z);
        // Recalculate the z coordinate to see whether something has changed
        z2 = uo::mapElevation(x, y, worldNumber);
        const std::int8_t dynz = uo::dynamicElevation(x, y, z, worldNumber, instanceId, prefZ);
        if (ILLEGAL_Z != dynz) {
            z2 = dynz;
        }
        else {
            const std::int8_t staticz = uo::staticTop(x, y, z, worldNumber, prefZ);
            if (ILLEGAL_Z != staticz) {
                z2 = staticz;
            }
        }
        
        if (z2 == z) {
            rValue = true;
        }
    }
    else if (!rValue && (waterCreature || amphiCreature) && !validWaterPos.empty() && waterPosSize > 0) {
        currLoc = validWaterPos[Random::get(static_cast<size_t>(0), (waterPosSize - 1))];
        x = static_cast<std::int16_t>(currLoc.x);
        y = static_cast<std::int16_t>(currLoc.y);
        z = static_cast<std::int8_t>(currLoc.z);
        // Recalculate the z coordinate to see whether something has changed
        z2 = uo::mapElevation(x, y, worldNumber);
        const std::int8_t dynz = uo::dynamicElevation(x, y, z, worldNumber, instanceId, prefZ);
        if (ILLEGAL_Z != dynz) {
            z2 = dynz;
        }
        else {
            const std::int8_t staticz = uo::staticTop(x, y, z, worldNumber, prefZ);
            if (ILLEGAL_Z != staticz) {
                z2 = staticz;
            }
        }
        
        if (z2 == z) {
            rValue = true;
        }
    }
    return rValue;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::FindItemSpotToSpawn()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Find a random spot within a region valid for dropping an item
// o------------------------------------------------------------------------------------------------o
bool CSpawnRegion::FindItemSpotToSpawn(std::int16_t &x, std::int16_t &y, std::int8_t &z) {
    bool rValue = false;
    Point3 currLoc;
    std::int8_t z2 = ILLEGAL_Z;
    std::map<std::uint32_t, std::int8_t>::const_iterator checkValid;
    const size_t landPosSize = validLandPos.size();
    
    // By default, let's try - at most - this many times to find a valid spawn point for the item
    std::uint8_t maxSpawnAttempts = 100;
    
    // However, if we have found some valid spawn points already, reduce the amount of attempts to
    // find MORE valid spawn points, and increase the chance of using those valid spots instead!
    if (landPosSize > 0) {
        maxSpawnAttempts = std::max(static_cast<std::uint8_t>(25), static_cast<std::uint8_t>(maxSpawnAttempts - landPosSize));
    }
    
    for (std::uint8_t a = 0; a < maxSpawnAttempts; ++a) {
        x = Random::get(x1, x2);
        y = Random::get(y1, y2);
        z = uo::mapElevation(x, y, worldNumber);
        
        if (defZ != ILLEGAL_Z) {
            // Use the definite Z level defined in the spawn region
            // Trust that the spawnregion scripter knows what they are doing...
            z = defZ;
        }
        else {
            // No definite Z has been defined, look for valid dynamic Z based on prefZ
            const std::int8_t dynZ =uo::dynamicElevation(x, y, z, worldNumber, instanceId, prefZ);
            if (ILLEGAL_Z != dynZ) {
                z = dynZ;
            }
            
            // Even if we got a valid dynamic Z, there might be a better match with statics, based
            // on prefZ
            const std::int8_t staticZ = uo::staticTop(x, y, z, worldNumber, prefZ);
            if (ILLEGAL_Z != staticZ && staticZ > z) {
                z = staticZ;
            }
        }
        
        // First go through the lists of already stored good locations
        if (!validLandPosCheck.empty() && landPosSize > 0) {
            checkValid = validLandPosCheck.find(y + (x << 16));
            if (checkValid != validLandPosCheck.end()) {
                z2 = (*checkValid).second;
            }
            
            if (z2 == z && z != ILLEGAL_Z) {
                rValue = true;
                break;
            }
        }
        
        if (uo::validSpawnLocation(x, y, z, worldNumber, instanceId)) {
            if (onlyOutside == false || !uo::inBuilding(x, y, z, worldNumber, instanceId)) {
                rValue = true;
                validLandPos.push_back(Point3(x, y, z));
                validLandPosCheck[y + (x << 16)] = z;
                break;
            }
        }
    }
    
    // If we haven't found a valid location pick a random location from the stored ones
    if (!rValue && !validLandPos.empty() && landPosSize > 0) {
        currLoc = validLandPos[Random::get(static_cast<size_t>(0), (landPosSize - 1))];
        x = static_cast<std::int16_t>(currLoc.x);
        y = static_cast<std::int16_t>(currLoc.y);
        z = static_cast<std::int8_t>(currLoc.z);
        // Recalculate the z coordinate to see whether something has changed
        z2 = uo::mapElevation(x, y, worldNumber);
        const std::int8_t dynz = uo::dynamicElevation(x, y, z, worldNumber, instanceId, prefZ);
        if (ILLEGAL_Z != dynz) {
            z2 = dynz;
        }
        else {
            const std::int8_t staticz = uo::staticTop(x, y, z, worldNumber, prefZ);
            if (ILLEGAL_Z != staticz) {
                z2 = staticz;
            }
        }
        
        if (z2 == z) {
            rValue = true;
        }
    }
    
    return rValue;
}
// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::CheckSpawned()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if an item/npc should be removed from spawnlist and saved
// o------------------------------------------------------------------------------------------------o
void CSpawnRegion::CheckSpawned() {
    for (CChar *cCheck = spawnedChars.First(); !spawnedChars.Finished();
         cCheck = spawnedChars.Next()) {
        if (ValidateObject(cCheck)) {
            if (ValidateObject(cCheck->GetOwnerObj())) {
                cCheck->ShouldSave(true);
                spawnedChars.Remove(cCheck);
            }
        }
        else {
            Console::shared().warning("Invalid Object found in CSpawnRegion character list, AutoCorrecting.");
            spawnedChars.Remove(cCheck);
        }
    }
    
    for (CItem *iCheck = spawnedItems.First(); !spawnedItems.Finished();
         iCheck = spawnedItems.Next()) {
        if (ValidateObject(iCheck)) {
            if (ValidateObject(iCheck->GetCont())) {
                iCheck->ShouldSave(true);
                spawnedItems.Remove(iCheck);
            }
        }
        else {
            Console::shared().warning("Invalid Object found in CSpawnRegion item list, AutoCorrecting.");
            spawnedItems.Remove(iCheck);
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::DeleteSpawnedChar()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Delete an npc from spawnlist
// o------------------------------------------------------------------------------------------------o
void CSpawnRegion::DeleteSpawnedChar(CChar *toDelete) {
    if (spawnedChars.Remove(toDelete)) {
        IncCurrentCharAmt(-1);
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::DeleteSpawnedItem()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Delete an item from spawnlist
// o------------------------------------------------------------------------------------------------o
void CSpawnRegion::DeleteSpawnedItem(CItem *toDelete) {
    if (spawnedItems.Remove(toDelete)) {
        IncCurrentItemAmt(-1);
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetSpawnedItemsList()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets list of spawned Items for spawnregion
// o------------------------------------------------------------------------------------------------o
GenericList<CItem *> *CSpawnRegion::GetSpawnedItemsList() { return &spawnedItems; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetSpawnedCharsList()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets list of spawned NPCs for spawnregion
// o------------------------------------------------------------------------------------------------o
GenericList<CChar *> *CSpawnRegion::GetSpawnedCharsList() { return &spawnedChars; }
