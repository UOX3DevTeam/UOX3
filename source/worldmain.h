// o------------------------------------------------------------------------------------------------o
//|	File		-	worldmain.h
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Interface for the CWorldMain class.
// o------------------------------------------------------------------------------------------------o

#if !defined(__WORLDMAIN_H__)
#define __WORLDMAIN_H__

#include <cstdint>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#if defined(_WIN32)
#include <winsock2.h>
#endif

#include "genericlist.h"
#include "typedefs.h"
#include "type/uotime.hpp"
#include "uoxstruct.h"
#include "ip4address.hpp"

class CSpawnRegion;
class CTownRegion;
class CTEffect;
class CBaseObject;
enum CWM_TID {
    tWORLD_NEXTFIELDEFFECT = 0,
    tWORLD_NEXTNPCAI,
    tWORLD_SHOPRESTOCK,
    tWORLD_HUNGERDAMAGE,
    tWORLD_THIRSTDRAIN,
    tWORLD_LIGHTTIME,
    tWORLD_PETOFFLINECHECK,
    tWORLD_COUNT
};

class CServerProfile {
private:
    std::uint32_t networkTime;
    std::uint32_t timerTime;
    std::uint32_t autoTime;
    std::uint32_t loopTime;
    std::uint32_t networkTimeCount;
    std::uint32_t timerTimeCount;
    std::uint32_t autoTimeCount;
    std::uint32_t loopTimeCount;
    
    std::int32_t globalRecv;
    std::int32_t globalSent;
    
public:
    CServerProfile();
    
    std::uint32_t NetworkTime() const;
    std::uint32_t TimerTime() const;
    std::uint32_t AutoTime() const;
    std::uint32_t LoopTime() const;
    
    void NetworkTime(std::uint32_t newVal);
    void TimerTime(std::uint32_t newVal);
    void AutoTime(std::uint32_t newVal);
    void LoopTime(std::uint32_t newVal);
    
    void IncNetworkTime(std::uint32_t toInc);
    void IncTimerTime(std::uint32_t toInc);
    void IncAutoTime(std::uint32_t toInc);
    void IncLoopTime(std::uint32_t toInc);
    
    std::uint32_t NetworkTimeCount() const;
    std::uint32_t TimerTimeCount() const;
    std::uint32_t AutoTimeCount() const;
    std::uint32_t LoopTimeCount() const;
    
    void NetworkTimeCount(std::uint32_t newVal);
    void TimerTimeCount(std::uint32_t newVal);
    void AutoTimeCount(std::uint32_t newVal);
    void LoopTimeCount(std::uint32_t newVal);
    
    void IncNetworkTimeCount();
    void IncTimerTimeCount();
    void IncAutoTimeCount();
    void IncLoopTimeCount();
    
    std::int32_t GlobalReceived() const;
    std::int32_t GlobalSent() const;
    
    void GlobalReceived(std::int32_t newVal);
    void GlobalSent(std::int32_t newVal);
};

class CWorldMain {
private:
    // Custom Titles
    struct Title_st {
        std::string fame;
        std::string skill;
        Title_st() : fame(""), skill("") {}
    };
    
    // Timers
    timerval_t worldTimers[tWORLD_COUNT];
    ip4list_t availableIPs;
    
    // Console & Program Level Vars
    bool error;
    bool keepRun;
    bool secure; // Secure mode
    bool hasLoaded;
    
    // Time Functions
    std::uint32_t uoTickCount;
    std::uint32_t startTime, endTime, lClock;
    bool overflow;
    std::uint32_t uiCurrentTime;
    
    // Worldsave
    std::uint32_t oldTime, newTime;
    bool autoSaved;
    savestatus_t worldSaveProgress;
    
    // IP Update
    std::uint32_t oldIPtime, newIPtime;
    bool ipUpdated;
    
    // Misc
    size_t playersOnline; // Players online
    bool reloadingScripts;
    bool classesInitialized;
    
public:
    UOTime uoTime ;
    struct Skill_st {
        std::uint16_t strength;
        std::uint16_t dexterity;
        std::uint16_t intelligence;
        std::int32_t skillDelay;
        std::string madeWord;
        std::vector<Advance> advancement;
        std::uint16_t jsScript;
        std::string name;
        Skill_st() { ResetDefaults(); }
        auto ResetDefaults() -> void {
            advancement.resize(0);
            strength = 0;
            dexterity = 0;
            intelligence = 0;
            skillDelay = -1;
            madeWord = "made";
            jsScript = 0xFFFF;
            name = "";
        }
    };
    
    auto matchIP(const IP4Addr &ip) const -> IP4Addr;
    // Timers
    void SetTimer(CWM_TID timerId, timerval_t newVal);
    timerval_t GetTimer(CWM_TID timerId) const;
    
    // Console & Program Level Vars
    void SetError(bool newVal);
    bool GetError() const;
    void SetKeepRun(bool newVal);
    bool GetKeepRun() const;
    void SetSecure(bool newVal);
    bool GetSecure() const;
    void SetLoaded(bool newVal);
    bool GetLoaded() const;
    
    // Time Functions
    void SetUICurrentTime(std::uint32_t newVal);
    std::uint32_t GetUICurrentTime() const;
    void SetUOTickCount(std::uint32_t newVal);
    std::uint32_t GetUOTickCount() const;
    void SetOverflow(bool newVal);
    bool GetOverflow() const;
    void SetStartTime(std::uint32_t newVal);
    std::uint32_t GetStartTime() const;
    void SetEndTime(std::uint32_t newVal);
    std::uint32_t GetEndTime() const;
    void SetLClock(std::uint32_t newVal);
    std::uint32_t GetLClock() const;
    
    // Worldsave
    void SetNewTime(std::uint32_t newVal);
    std::uint32_t GetNewTime() const;
    void SetOldTime(std::uint32_t newVal);
    std::uint32_t GetOldTime() const;
    void SetAutoSaved(bool newVal);
    bool GetAutoSaved() const;
    void SetWorldSaveProgress(savestatus_t newVal);
    savestatus_t GetWorldSaveProgress() const;
    
    // IP update
    std::uint32_t GetNewIPTime() const;
    void SetNewIPTime(std::uint32_t newVal);
    std::uint32_t GetOldIPTime() const;
    void SetOldIPTime(std::uint32_t newVal);
    void SetIPUpdated(bool newVal);
    bool GetIPUpdated() const;
    
    // Misc
    void SetPlayersOnline(size_t newVal);
    size_t GetPlayersOnline() const;
    void DecPlayersOnline();
    bool GetReloadingScripts() const;
    void SetReloadingScripts(bool newVal);
    void ClassesInitialized(bool newVal);
    bool ClassesInitialized() const;
    
    void CheckAutoTimers();
    
    // Structs
    std::map<std::uint16_t, CCreatures> creatures;
    timeval uoxTimeout;
    Skill_st skill[INTELLECT + 1]; // Skill data
    Title_st title[ALLSKILLS + 1]; // For custom titles reads titles.dfn
    std::vector<TitlePair> prowessTitles;
    std::vector<TitlePair> murdererTags;
    std::vector<CTeleLocationEntry> teleLocs;
    std::vector<LogoutLocationEntry_st> logoutLocs;
    std::vector<SOSLocationEntry> sosLocs;
    std::vector<std::uint16_t> escortRegions;
    std::map<std::uint16_t, GoPlaces> goPlaces;
    std::unordered_map<std::uint16_t, CSpawnRegion *> spawnRegions;
    std::map<std::uint16_t, CTownRegion *> townRegions;
    GenericList<CTEffect *> tempEffects;
    
    std::map<CBaseObject *, std::uint32_t> refreshQueue;
    std::map<CBaseObject *, std::uint32_t> deletionQueue;
    
    void CheckTimers();
    void DoWorldLight();
    void SaveNewWorld(bool x);
    auto startup() -> void;
    CWorldMain();
    //auto ServerData() -> CServerData *;
    //auto SetServerData(CServerData &server_data) -> void;
    auto ServerProfile() -> CServerProfile *;
    
private:
    void RegionSave();
    void SaveStatistics();
    
    //CServerData *sData;
    CServerProfile sProfile;
};

//extern CWorldMain *cwmWorldState;

#endif
