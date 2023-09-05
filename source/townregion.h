#ifndef __TownRegion__
#define __TownRegion__

#include <bitset>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "typedefs.h"

class CScriptSection;
class Script;
class CChar;
class CSocket;
class CItem;

struct MiningData_st;

struct OrePref_st {
    MiningData_st *oreIndex;
    std::uint16_t percentChance;
};

struct RegLocs_st {
    std::int16_t x1 = 0;
    std::int16_t y1 = 0;
    std::int16_t x2 = 0;
    std::int16_t y2 = 0;
};

const std::string WorldTypeNames[WRLD_COUNT] = {"Spring", "Summer",     "Autumn",
    "Winter", "Desolation", "Unknown"};

class CTownRegion {
private:
    struct TownPers_st {
        serial_t townMember;
        serial_t targVote;
        CItem *PlayerBank;
    };
    
    struct GoodData_st {
        std::int32_t sellVal;
        std::int32_t buyVal;
        std::int32_t rand1;
        std::int32_t rand2;
        
        GoodData_st() : sellVal(0), buyVal(0), rand1(0), rand2(0) {}
    };
    
    std::uint16_t regionNum;
    std::uint16_t numGuards;
    std::uint16_t parentRegion; // reference to parent region
    std::string name;
    std::uint16_t musicList;
    std::uint8_t worldNumber;
    std::uint16_t instanceId;
    std::bitset<12> priv; // 0x01 guarded, 0x02, mark allowed, 0x04 gate allowed, 0x08 recall
    // 0x10 raining, 0x20, snowing,		 0x40 magic damage reduced to 0
    // 0x80 Dungeon region
    std::string guardowner;
    std::vector<std::string> guards;
    std::vector<OrePref_st> orePreferences;
    std::vector<TownPers_st> townMember;
    std::vector<std::uint16_t>
    alliedTowns; // allied towns are ones taken over, or ones allied to.  Share resources
    std::vector<RegLocs_st> locations;
    std::map<std::int32_t, GoodData_st> goodList;
    std::string guardList;
    serial_t mayorSerial; // serial of the mayor, calculated on startup always
    raceid_t race;        // town's race property, guards will be racially based
    weathid_t weather;    // weather system the region belongs to
    std::int32_t goldReserved;  // amount of gold belonging to the town's treasury
    
    worldtype_t visualAppearance; // seasonal choice, basically.  Each of the 4 seasons, or "dead"
    std::int32_t timeSinceGuardsPaid;   // time since the guards were last paid
    std::int32_t timeSinceTaxedMembers; // time since we last taxed our own members
    std::int32_t timeToElectionClose;   // time since the last election was completed
    std::int32_t timeToNextPoll;        // time since the polling booth was opened
    std::int16_t guardsPurchased;
    std::int32_t resourceCollected; // how much we have gotten from taxes
    std::uint16_t taxedResource;     // item # of the taxed resource
    std::uint16_t taxedAmount;       // how much to tax
    
    std::int16_t health;           // health of the town
    std::uint8_t chanceFindBigOre; // chance of finding big ore
    
    [[maybe_unused]] std::uint16_t jsScript;
    std::vector<std::uint16_t> scriptTriggers;
    
    void SendEnemyGump(CSocket *sock);
    void SendBasicInfo(CSocket *sock);
    void SendPotentialMember(CSocket *sock);
    void SendMayorGump(CSocket *sock);
    void SendDefaultGump(CSocket *sock);
    serial_t FindPositionOf(CChar &toAdd);
    
    bool RemoveCharacter(size_t position);
    
public:
    CTownRegion(std::uint16_t region);
    ~CTownRegion();
    
    bool DisplayTownMenu(CItem *used, CSocket *sock, std::int8_t flag = -1);
    bool VoteForMayor(CSocket *sock);
    bool DonateResource(CSocket *s, std::int32_t amount);
    bool PurchaseGuard(CSocket *sock, std::uint8_t number);
    bool ViewBudget(CSocket *sock);
    bool PeriodicCheck();
    bool load(Script *ss);              // entry is the region #, fp is the file to load from
    bool Save(std::ostream &outStream); // entry is the region #, fp is the file to save in
    bool InitFromScript(CScriptSection *toScan);
    bool AddAsTownMember(CChar &toAdd);  // toAdd is the character to add
    bool RemoveTownMember(CChar &toAdd); // toAdd is the character to remove
    bool IsGuarded() const;
    bool CanPlaceHouse() const;
    bool CanMark() const;
    bool CanGate() const;
    bool CanRecall() const;
    bool CanTeleport() const;
    bool CanCastAggressive() const;
    bool IsSafeZone() const;
    bool IsDungeon() const;
    bool IsSubRegion() const;
    bool IsMemberOfTown(CChar *player) const;
    bool IsAlliedTown(std::uint16_t townToCheck) const;
    bool MakeAlliedTown(std::uint16_t townToMake);
    
    void IsGuarded(bool value);
    void CanPlaceHouse(bool value);
    void CanMark(bool value);
    void CanGate(bool value);
    void CanRecall(bool value);
    void CanTeleport(bool value);
    void CanCastAggressive(bool value);
    void IsSafeZone(bool value);
    void IsDungeon(bool value);
    void IsSubRegion(bool value);
    void SetName(std::string toSet);
    void SetRace(raceid_t newRace);
    void TellMembers(std::int32_t dictEntry, ...);
    void SendAlliedTowns(CSocket *sock);
    void SendEnemyTowns(CSocket *sock);
    void ForceEarlyElection();
    void Possess(CTownRegion *possessorTown);
    void SetTaxesLeft(std::uint32_t newValue);
    void SetReserves(std::uint32_t newValue);
    void CalcNewMayor(); // calculates the new mayor
    void DisplayTownMembers(CSocket *sock);
    void ViewTaxes(CSocket *s);
    void DoDamage(std::int16_t reduction);
    void TaxedAmount(std::uint16_t amount);
    void SetResourceId(std::uint16_t resId);
    void SetHealth(std::int16_t newValue);
    void SetChanceBigOre(std::uint8_t newValue);
    void SetAppearance(worldtype_t worldType);
    
    worldtype_t GetAppearance() const;
    std::uint8_t GetChanceBigOre() const;
    std::int32_t GetGoodBuy(std::uint8_t index) const;
    std::int32_t GetGoodRnd1(std::uint8_t index) const;
    std::int32_t GetGoodRnd2(std::uint8_t index) const;
    std::int32_t GetGoodSell(std::uint8_t index) const;
    std::int16_t GetHealth() const;
    CChar *GetMayor();                 // returns the mayor character
    serial_t GetMayorSerial() const;     // returns the mayor's serial
    void SetMayorSerial(serial_t newvValue); // sets the mayor's serial
    std::uint16_t GetMusicList() const;
    void SetMusicList(std::uint16_t newValue);
    std::string GetName() const;
    size_t GetNumOrePreferences() const;
    const OrePref_st *GetOrePreference(size_t targValue) const;
    std::int32_t GetOreChance() const;
    std::string GetOwner() const;
    size_t GetPopulation() const;
    raceid_t GetRace() const;
    CChar *GetRandomGuard(); // returns a random guard from guard list
    std::uint32_t GetReserves() const;
    std::uint16_t GetResourceId() const;
    std::uint32_t GetTaxes() const;
    weathid_t GetWeather() const;
    void SetWeather(weathid_t newValue);
    std::uint16_t NumGuards() const;
    void SetNumGuards(std::uint16_t newValue);
    std::uint16_t TaxedAmount() const;
    std::uint8_t WorldNumber() const;
    void WorldNumber(std::uint8_t newValue);
    std::uint16_t GetInstanceId() const;
    void SetInstanceId(std::uint16_t newValue);
    
    std::vector<std::uint16_t> GetScriptTriggers();
    void AddScriptTrigger(std::uint16_t newValue);
    void RemoveScriptTrigger(std::uint16_t newValue);
    void ClearScriptTriggers();
    
    std::uint16_t GetRegionNum() const;
    void SetRegionNum(std::uint16_t newVal);
    
    size_t GetNumLocations() const;
    const RegLocs_st *GetLocation(size_t locNum) const;
    
    std::string GetTownMemberSerials() const;
    std::vector<TownPers_st> GetTownMembers() const;
};

#endif
