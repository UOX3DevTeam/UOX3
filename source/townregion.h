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
        SERIAL townMember;
        SERIAL targVote;
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
    SERIAL mayorSerial; // serial of the mayor, calculated on startup always
    RACEID race;        // town's race property, guards will be racially based
    WEATHID weather;    // weather system the region belongs to
    std::int32_t goldReserved;  // amount of gold belonging to the town's treasury

    WorldType visualAppearance; // seasonal choice, basically.  Each of the 4 seasons, or "dead"
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
    SERIAL FindPositionOf(CChar &toAdd);

    bool RemoveCharacter(size_t position);

  public:
    CTownRegion(std::uint16_t region);
    ~CTownRegion();

    bool DisplayTownMenu(CItem *used, CSocket *sock, std::int8_t flag = -1);
    bool VoteForMayor(CSocket *sock);
    bool DonateResource(CSocket *s, std::int32_t amount);
    bool PurchaseGuard(CSocket *sock, std::uint8_t number);
    bool ViewBudget(CSocket *sock);
    bool PeriodicCheck(void);
    bool Load(Script *ss);              // entry is the region #, fp is the file to load from
    bool Save(std::ostream &outStream); // entry is the region #, fp is the file to save in
    bool InitFromScript(CScriptSection *toScan);
    bool AddAsTownMember(CChar &toAdd);  // toAdd is the character to add
    bool RemoveTownMember(CChar &toAdd); // toAdd is the character to remove
    bool IsGuarded(void) const;
    bool CanPlaceHouse(void) const;
    bool CanMark(void) const;
    bool CanGate(void) const;
    bool CanRecall(void) const;
    bool CanTeleport(void) const;
    bool CanCastAggressive(void) const;
    bool IsSafeZone(void) const;
    bool IsDungeon(void) const;
    bool IsSubRegion(void) const;
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
    void SetRace(RACEID newRace);
    void TellMembers(std::int32_t dictEntry, ...);
    void SendAlliedTowns(CSocket *sock);
    void SendEnemyTowns(CSocket *sock);
    void ForceEarlyElection(void);
    void Possess(CTownRegion *possessorTown);
    void SetTaxesLeft(std::uint32_t newValue);
    void SetReserves(std::uint32_t newValue);
    void CalcNewMayor(void); // calculates the new mayor
    void DisplayTownMembers(CSocket *sock);
    void ViewTaxes(CSocket *s);
    void DoDamage(std::int16_t reduction);
    void TaxedAmount(std::uint16_t amount);
    void SetResourceId(std::uint16_t resId);
    void SetHealth(std::int16_t newValue);
    void SetChanceBigOre(std::uint8_t newValue);
    void SetAppearance(WorldType worldType);

    WorldType GetAppearance(void) const;
    std::uint8_t GetChanceBigOre(void) const;
    std::int32_t GetGoodBuy(std::uint8_t index) const;
    std::int32_t GetGoodRnd1(std::uint8_t index) const;
    std::int32_t GetGoodRnd2(std::uint8_t index) const;
    std::int32_t GetGoodSell(std::uint8_t index) const;
    std::int16_t GetHealth(void) const;
    CChar *GetMayor(void);                 // returns the mayor character
    SERIAL GetMayorSerial(void) const;     // returns the mayor's serial
    void SetMayorSerial(SERIAL newvValue); // sets the mayor's serial
    std::uint16_t GetMusicList(void) const;
    void SetMusicList(std::uint16_t newValue);
    std::string GetName(void) const;
    size_t GetNumOrePreferences(void) const;
    const OrePref_st *GetOrePreference(size_t targValue) const;
    std::int32_t GetOreChance(void) const;
    std::string GetOwner(void) const;
    size_t GetPopulation(void) const;
    RACEID GetRace(void) const;
    CChar *GetRandomGuard(void); // returns a random guard from guard list
    std::uint32_t GetReserves(void) const;
    std::uint16_t GetResourceId(void) const;
    std::uint32_t GetTaxes(void) const;
    WEATHID GetWeather(void) const;
    void SetWeather(WEATHID newValue);
    std::uint16_t NumGuards(void) const;
    void SetNumGuards(std::uint16_t newValue);
    std::uint16_t TaxedAmount(void) const;
    std::uint8_t WorldNumber(void) const;
    void WorldNumber(std::uint8_t newValue);
    std::uint16_t GetInstanceId(void) const;
    void SetInstanceId(std::uint16_t newValue);

    std::vector<std::uint16_t> GetScriptTriggers(void);
    void AddScriptTrigger(std::uint16_t newValue);
    void RemoveScriptTrigger(std::uint16_t newValue);
    void ClearScriptTriggers(void);

    std::uint16_t GetRegionNum(void) const;
    void SetRegionNum(std::uint16_t newVal);

    size_t GetNumLocations(void) const;
    const RegLocs_st *GetLocation(size_t locNum) const;

    std::string GetTownMemberSerials(void) const;
    std::vector<TownPers_st> GetTownMembers(void) const;
};

#endif
