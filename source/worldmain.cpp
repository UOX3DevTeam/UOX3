// o------------------------------------------------------------------------------------------------o
//|	File		-	worldmain.cpp
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Version History
//|
//|						1.0
//|						Initial implementation.
//|
//|						1.1		March 12, 2003
//|						Most global variables moved from uox3.h to the
//CWorldMain class
//|
//|						1.2		October 16, 2003
//|						Added quite a few more vectors to remove them from
//global scope |						Removed many members that were no
// longer necesarry and moved |							others to more
// appropriate
// locations. |						Added a proper constructor rather than
// ResetDefaults()
//|						Grouped timers together in an array using an enum.
// o------------------------------------------------------------------------------------------------o
#include "worldmain.h"

#include <fstream>

#include "subsystem/account.hpp"
#include "ceffects.h"
#include "cguild.h"
#include "subsystem/console.hpp"
#include "cspawnregion.h"
#include "dictionary.h"
#include "funcdecl.h"
#include "jail.h"
#include "network.h"
#include "objectfactory.h"
#include "osunique.hpp"
#include "regions.h"
#include "configuration/serverconfig.hpp"
#include "skills.h"
#include "speech.h"
#include "townregion.h"

CWorldMain *cwmWorldState = nullptr;

// o------------------------------------------------------------------------------------------------o
//| CWorldMain Constructor & Destructor
// o------------------------------------------------------------------------------------------------o
const bool DEFWORLD_KEEPRUN = true;
const bool DEFWORLD_ERROR = false;
const bool DEFWORLD_SECURE = true;

const bool DEFWORLD_LOADED = false;
const std::uint32_t DEFWORLD_UICURRENTTIME = 0;
const std::uint32_t DEFWORLD_UOTICKCOUNT = 1;
const bool DEFWORLD_OVERFLOW = false;
const std::uint32_t DEFWORLD_STARTTIME = 0;
const std::uint32_t DEFWORLD_ENDTIME = 0;
const std::uint32_t DEFWORLD_LCLOCK = 0;
const size_t DEFWORLD_PLAYERSONLINE = 0;
const std::uint32_t DEFWORLD_NEWTIME = 0;
const std::uint32_t DEFWORLD_OLDTIME = 0;
const bool DEFWORLD_AUTOSAVED = false;
const savestatus_t DEFWORLD_SAVEPROGRESS = SS_NOTSAVING;
const bool DEFWORLD_RELOADINGSCRIPTS = false;
const bool DEFWORLD_CLASSESINITIALIZED = false;

CWorldMain::CWorldMain() : error(DEFWORLD_ERROR), keepRun(DEFWORLD_KEEPRUN), secure(DEFWORLD_SECURE), hasLoaded(DEFWORLD_LOADED), uoTickCount(DEFWORLD_UOTICKCOUNT), startTime(DEFWORLD_STARTTIME), endTime(DEFWORLD_ENDTIME), lClock(DEFWORLD_LCLOCK), overflow(DEFWORLD_OVERFLOW), uiCurrentTime(DEFWORLD_UICURRENTTIME), oldTime(DEFWORLD_OLDTIME), newTime(DEFWORLD_NEWTIME), autoSaved(DEFWORLD_AUTOSAVED), worldSaveProgress(DEFWORLD_SAVEPROGRESS), playersOnline(DEFWORLD_PLAYERSONLINE), reloadingScripts(DEFWORLD_RELOADINGSCRIPTS), classesInitialized(DEFWORLD_CLASSESINITIALIZED) {
    
    for (std::int32_t mTID = static_cast<std::int32_t>(tWORLD_NEXTFIELDEFFECT); mTID < static_cast<std::int32_t>(tWORLD_COUNT); ++mTID) {
        
        worldTimers[mTID] = 0;
    }
    creatures.clear();
    prowessTitles.resize(0);
    murdererTags.resize(0);
    teleLocs.resize(0);
    escortRegions.resize(0);
    logoutLocs.resize(0);
    sosLocs.resize(0);
    goPlaces.clear();
    refreshQueue.clear();
    deletionQueue.clear();
    spawnRegions.clear();
    uoxTimeout.tv_sec = 0;
    uoxTimeout.tv_usec = 0;
    availableIPs = ip4list_t::available();
    
}
//==================================================================================================
auto CWorldMain::startup() -> void {}


// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetTimer()
//|					CWorldMain::SetTimer()
//|	Date		-	10/17/2003
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles all the world timers (next time we check NPC AI, etc)
// o------------------------------------------------------------------------------------------------o
timerval_t CWorldMain::GetTimer(CWM_TID timerId) const {
    timerval_t rValue = 0;
    if (timerId != tWORLD_COUNT) {
        rValue = worldTimers[timerId];
    }
    return rValue;
}
void CWorldMain::SetTimer(CWM_TID timerId, timerval_t newVal) {
    if (timerId != tWORLD_COUNT) {
        worldTimers[timerId] = newVal;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetError()
//|					CWorldMain::SetError()
//|	Date		-	3/12/2003
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets if we have generated an error
// o------------------------------------------------------------------------------------------------o
bool CWorldMain::GetError() const { return error; }
void CWorldMain::SetError(bool newVal) { error = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetKeepRun()
//|					CWorldMain::SetKeepRun()
//|	Date		-	3/12/2003
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets if server should be kept running
// o------------------------------------------------------------------------------------------------o
bool CWorldMain::GetKeepRun() const { return keepRun; }
void CWorldMain::SetKeepRun(bool newVal) { keepRun = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetSecure()
//|					CWorldMain::SetSecure()
//|	Date		-	3/12/2003
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether console "secure" mode is enabled/disabled
// o------------------------------------------------------------------------------------------------o
bool CWorldMain::GetSecure() const { return secure; }
void CWorldMain::SetSecure(bool newVal) { secure = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetLoaded() const
//|					CWorldMain::SetLoaded()
//|	Date		-	3/12/2003
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether UOX has been loaded
// o------------------------------------------------------------------------------------------------o
bool CWorldMain::GetLoaded() const { return hasLoaded; }
void CWorldMain::SetLoaded(bool newVal) { hasLoaded = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetUICurrentTime()
//|					CWorldMain::SetUICurrentTime()
//|	Date		-	3/12/2003
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets current time
// o------------------------------------------------------------------------------------------------o
std::uint32_t CWorldMain::GetUICurrentTime() const { return uiCurrentTime; }
void CWorldMain::SetUICurrentTime(std::uint32_t newVal) { uiCurrentTime = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetUOTickCount()
//|					CWorldMain::SetUOTickCount()
//|	Date		-	3/12/2003
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets UO Minutes
// o------------------------------------------------------------------------------------------------o
std::uint32_t CWorldMain::GetUOTickCount() const { return uoTickCount; }
void CWorldMain::SetUOTickCount(std::uint32_t newVal) { uoTickCount = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetOverflow()
//|					CWorldMain::SetOverflow()
//|	Date		-	3/12/2003
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether we overflowed the time
// o------------------------------------------------------------------------------------------------o
bool CWorldMain::GetOverflow() const { return overflow; }
void CWorldMain::SetOverflow(bool newVal) { overflow = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetStartTime()
//|					CWorldMain::SetStartTime()
//|	Date		-	3/12/2003
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets time when server started up
// o------------------------------------------------------------------------------------------------o
std::uint32_t CWorldMain::GetStartTime() const { return startTime; }
void CWorldMain::SetStartTime(std::uint32_t newVal) { startTime = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetEndTime()
//|					CWorldMain::SetEndTime()
//|	Date		-	3/12/2003
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets time when server will shutdown
// o------------------------------------------------------------------------------------------------o
std::uint32_t CWorldMain::GetEndTime() const { return endTime; }
void CWorldMain::SetEndTime(std::uint32_t newVal) { endTime = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetLClock()
//|					CWorldMain::SetLClock()
//|	Date		-	3/12/2003
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets end time
// o------------------------------------------------------------------------------------------------o
std::uint32_t CWorldMain::GetLClock() const { return lClock; }
void CWorldMain::SetLClock(std::uint32_t newVal) { lClock = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetNewTime()
//|					CWorldMain::SetNewTime()
//|	Date		-	3/12/2003
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets time for next auto worldsave
// o------------------------------------------------------------------------------------------------o
std::uint32_t CWorldMain::GetNewTime() const { return newTime; }
void CWorldMain::SetNewTime(std::uint32_t newVal) { newTime = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetOldTime()
//|					CWorldMain::SetOldTime()
//|	Date		-	3/12/2003
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets time of last auto worldsave
// o------------------------------------------------------------------------------------------------o
std::uint32_t CWorldMain::GetOldTime() const { return oldTime; }
void CWorldMain::SetOldTime(std::uint32_t newVal) { oldTime = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetAutoSaved()
//|					CWorldMain::SetAutoSaved()
//|	Date		-	3/12/2003
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether world was autoSaved
// o------------------------------------------------------------------------------------------------o
bool CWorldMain::GetAutoSaved() const { return autoSaved; }
void CWorldMain::SetAutoSaved(bool newVal) { autoSaved = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetWorldSaveProgress()
//|					CWorldMain::SetWorldSaveProgress()
//|	Date		-	3/12/2003
//|	Changes		-	10/10/2003 - Now uses enum SaveStatus for ease of use
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets status of World saves (Not Saving, Saving, Just Saved)
// o------------------------------------------------------------------------------------------------o
savestatus_t CWorldMain::GetWorldSaveProgress() const { return worldSaveProgress; }
void CWorldMain::SetWorldSaveProgress(savestatus_t newVal) { worldSaveProgress = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetPlayersOnline()
//|					CWorldMain::SetPlayersOnline()
//|	Date		-	3/12/2003
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets total Players Online
// o------------------------------------------------------------------------------------------------o
size_t CWorldMain::GetPlayersOnline() const { return playersOnline; }
void CWorldMain::SetPlayersOnline(size_t newVal) { playersOnline = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::DecPlayersOnline()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Decrements the counter for players online
// o------------------------------------------------------------------------------------------------o
void CWorldMain::DecPlayersOnline() {
    if (playersOnline == 0) {
        throw std::runtime_error("Decrementing a 0 count which will rollover");
    }
    --playersOnline;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetReloadingScripts()
//|					CWorldMain::SetReloadingScripts()
//|	Date		-	6/22/2004
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether server is reloading its scripts
// o------------------------------------------------------------------------------------------------o
bool CWorldMain::GetReloadingScripts() const { return reloadingScripts; }
void CWorldMain::SetReloadingScripts(bool newVal) { reloadingScripts = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::CheckTimers()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check shutdown timers
// o------------------------------------------------------------------------------------------------o
void CWorldMain::CheckTimers() {
    SetOverflow((GetLClock() > GetUICurrentTime()));
    if (GetEndTime()) {
        if (GetEndTime() <= GetUICurrentTime()) {
            SetKeepRun(false);
        }
    }
    SetLClock(GetUICurrentTime());
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetNewIPTime()
//|					CWorldMain::SetNewIPTime()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets time for next auto IP update
// o------------------------------------------------------------------------------------------------o
std::uint32_t CWorldMain::GetNewIPTime() const { return newIPtime; }
void CWorldMain::SetNewIPTime(std::uint32_t newVal) { newIPtime = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetOldIPTime()
//|					CWorldMain::SetOldIPTime()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets time of last auto IP update
// o------------------------------------------------------------------------------------------------o
std::uint32_t CWorldMain::GetOldIPTime() const { return oldIPtime; }
void CWorldMain::SetOldIPTime(std::uint32_t newVal) { oldIPtime = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::GetIPUpdated()
//|					CWorldMain::SetIPUpdated()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether IPs have been updated
// o------------------------------------------------------------------------------------------------o
bool CWorldMain::GetIPUpdated() const { return ipUpdated; }
void CWorldMain::SetIPUpdated(bool newVal) { ipUpdated = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::ClassesInitialized()
//|					CWorldMain::ClassesInitialized()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether base classes been initialized (in case of early
// shut down)
// o------------------------------------------------------------------------------------------------o
bool CWorldMain::ClassesInitialized() const { return classesInitialized; }
void CWorldMain::ClassesInitialized(bool newVal) { classesInitialized = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::DoWorldLight()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Set world light level based on time of day and script settings
// o------------------------------------------------------------------------------------------------o
void CWorldMain::DoWorldLight() {
    
    std::uint8_t worlddarklevel = static_cast<std::uint8_t>(ServerConfig::shared().ushortValues[UShortValue::DARKLEVEL]);
    std::uint8_t worldbrightlevel = static_cast<std::uint8_t>(ServerConfig::shared().ushortValues[UShortValue::BRIGHTLEVEL]);
    bool ampm = uoTime.ampm ;
    std::uint8_t currentHour = uoTime.hours ;
    std::uint8_t currentMinute = uoTime.minutes;
    
    float currentTime = float(currentHour + (currentMinute / 60.0f));
    float hourIncrement = float(fabs((worlddarklevel - worldbrightlevel) / 12.0f)); // we want the amount to subtract from LightMax in the
    // morning / add to LightMin in evening
    
    if (ampm) {
        uoTime.worldLightLevel = static_cast<std::uint16_t>(RoundNumber(worldbrightlevel + (hourIncrement * currentTime)));
    }
    else {
        uoTime.worldLightLevel = static_cast<std::uint16_t>(RoundNumber(worlddarklevel - (hourIncrement * currentTime)));
    }
}

void FileArchive();
void CollectGarbage();
void sysBroadcast(const std::string &txt);
// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::SaveNewWorld()
//|	Date		-	1997
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Saves the UOX world
//|
//|	Changes		-	10/21/2002	-	fix for archiving. Now it wont always
// archive :)
// o------------------------------------------------------------------------------------------------o
void CWorldMain::SaveNewWorld(bool x) {
    static std::uint32_t save_counter = 0;
    
    std::for_each(cwmWorldState->spawnRegions.begin(), cwmWorldState->spawnRegions.end(), [](std::pair<std::uint16_t, CSpawnRegion *> entry) {
        if (entry.second) {
            entry.second->CheckSpawned();
        }
    });
    
    if (GetWorldSaveProgress() != SS_SAVING) {
        SetWorldSaveProgress(SS_SAVING);
        Console::shared().printSectionBegin();
        if (ServerConfig::shared().enabled(ServerSwitch::ANNOUNCESAVE)) {
            sysBroadcast(Dictionary->GetEntry(1615)); // World data saving, you may experience some
            // lag for the next several minutes.
            SpeechSys->poll();
        }
        Network->ClearBuffers();
        
        if (x) {
            Console::shared() << "Starting manual world data save...." << myendl;
        }
        else {
            Console::shared() << "Starting automatic world data save...." << myendl;
        }
        
        if (ServerConfig::shared().enabled(ServerSwitch::BACKUP) && !ServerConfig::shared().directoryFor(dirlocation_t::BACKUP).empty()) {
            ++save_counter;
            if ((save_counter % ServerConfig::shared().shortValues[ShortValue::SAVERATIO])  == 0) {
                Console::shared() << "Archiving world files." << myendl;
                FileArchive();
            }
        }
        Console::shared() << "Saving Misc. data... ";
        ServerConfig::shared().writeConfig(std::filesystem::path()) ;
        Console::shared().log("Server data save", "server.log");
        RegionSave();
        Console::shared().printDone();
        MapRegion->Save();
        GuildSys->Save();
        JailSys->WriteData();
        Effects->SaveEffects();
        uoTime.save(ServerConfig::shared().directoryFor(dirlocation_t::SAVE));
        SaveStatistics();
        
        if (ServerConfig::shared().enabled(ServerSwitch::ANNOUNCESAVE)) {
            sysBroadcast("World Save Complete.");
        }
        
        //	If accounts are to be loaded then they should be loaded
        //	all the time if using the web interface
        Account::shared().save();
        // Make sure to import the new accounts so they have access too.
        Console::shared() << "New accounts processed: " << Account::shared().importAccounts() << myendl;
        SetWorldSaveProgress(SS_JUSTSAVED);
        
        char saveTimestamp[100];
        time_t tempTimestamp =
        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        struct tm curtime;
        lcltime(tempTimestamp, curtime);
        strftime(saveTimestamp, 50, "%F at %T", &curtime);
        
        Console::shared() << "World save complete on " << saveTimestamp << myendl;
        Console::shared().printSectionBegin();
    }
    CollectGarbage();
    uiCurrentTime = GetClock();
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::RegionSave()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loops through all town regions and saves them to disk
// o------------------------------------------------------------------------------------------------o
void CWorldMain::RegionSave() {
    auto regionsFile = ServerConfig::shared().directoryFor(dirlocation_t::SAVE)/ std::filesystem::path("regions.wsc");
    std::ofstream regionsDestination(regionsFile.string());
    if (!regionsDestination) {
        Console::shared().error(util::format("Failed to open %s for writing", regionsFile.string().c_str()));
        return;
    }
    std::for_each(cwmWorldState->townRegions.begin(), cwmWorldState->townRegions.end(),[&regionsDestination](const std::pair<std::uint16_t, CTownRegion *> &town) {
        if (town.second) {
            town.second->Save(regionsDestination);
        }
    });
    regionsDestination.close();
}


//==================================================================================================
auto CWorldMain::ServerProfile() -> CServerProfile * { return &sProfile; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CWorldMain::SaveStatistics()
//|	Date		-	February 5th, 2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Saves out some useful statistics so that some tools
//|						such as WorldBuilder can do some memory reserve
// shortcuts
// o------------------------------------------------------------------------------------------------o
void CWorldMain::SaveStatistics() {
    auto statsFile = ServerConfig::shared().directoryFor(dirlocation_t::SAVE) / std::filesystem::path("statistics.wsc");
    std::ofstream statsDestination(statsFile.string());
    if (!statsDestination) {
        Console::shared().error(util::format("Failed to open %s for writing", statsFile.string().c_str()));
        return;
    }
    statsDestination << "[STATISTICS]" << '\n' << "{" << '\n';
    statsDestination << "PLAYERCOUNT=" << ObjectFactory::shared().CountOfObjects(CBaseObject::OT_CHAR)
    << '\n';
    statsDestination << "ITEMCOUNT=" << ObjectFactory::shared().CountOfObjects(CBaseObject::OT_ITEM)
    << '\n';
    statsDestination << "MULTICOUNT=" << ObjectFactory::shared().CountOfObjects(CBaseObject::OT_MULTI)
    << '\n';
    statsDestination << "}" << '\n' << '\n';
    
    statsDestination.close();
}

auto CWorldMain::matchIP(const IP4Addr &ip) const -> IP4Addr {
    auto [candidate, match] = availableIPs.bestmatch(ip);
    if (match == 0) {
        if (!ServerConfig::shared().serverString[ServerString::PUBLICIP].empty()) {
            candidate = IP4Addr(ServerConfig::shared().serverString[ServerString::PUBLICIP]);
        }
    }
    else {
        // We got some kind of match, see if on same network type?
        if (candidate.type() != ip.type()) {
            if (ip.type() == IP4Addr::ip4type_t::wan) {
                if (!ServerConfig::shared().serverString[ServerString::PUBLICIP].empty()) {
                    candidate = IP4Addr(ServerConfig::shared().serverString[ServerString::PUBLICIP]);
                }
            }
        }
    }
    return candidate;
}



// o------------------------------------------------------------------------------------------------o
//|	Class		-	CWorldMain::CServerProfile()
//|	Date		-	2004
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Version History
//|
//|						1.0		 		2004
//|						Original implementation
//|						Moving all profiling variables into their own class
// managed
//|						by CWorldMain in order to take them out of global
// scope
// o------------------------------------------------------------------------------------------------o
CServerProfile::CServerProfile()
: networkTime(0), timerTime(0), autoTime(0), loopTime(0), networkTimeCount(1000),
timerTimeCount(1000), autoTimeCount(1000), loopTimeCount(1000), globalRecv(0), globalSent(0) {
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerProfile::NetworkTime()
//|					CServerProfile::IncNetworkTime()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets/Increments networkTime
// o------------------------------------------------------------------------------------------------o
std::uint32_t CServerProfile::NetworkTime() const { return networkTime; }
void CServerProfile::NetworkTime(std::uint32_t newVal) { networkTime = newVal; }
void CServerProfile::IncNetworkTime(std::uint32_t toInc) { networkTime += toInc; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerProfile::TimerTime()
//|					CServerProfile::IncTimerTime()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets/Increments timerTime
// o------------------------------------------------------------------------------------------------o
std::uint32_t CServerProfile::TimerTime() const { return timerTime; }
void CServerProfile::TimerTime(std::uint32_t newVal) { timerTime = newVal; }
void CServerProfile::IncTimerTime(std::uint32_t toInc) { timerTime += toInc; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerProfile::AutoTime()
//|					CServerProfile::IncAutoTime()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets/Increments autoTime
// o------------------------------------------------------------------------------------------------o
std::uint32_t CServerProfile::AutoTime() const { return autoTime; }
void CServerProfile::AutoTime(std::uint32_t newVal) { autoTime = newVal; }
void CServerProfile::IncAutoTime(std::uint32_t toInc) { autoTime += toInc; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerProfile::LoopTime()
//|					CServerProfile::IncLoopTime()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets/Increments loopTime
// o------------------------------------------------------------------------------------------------o
std::uint32_t CServerProfile::LoopTime() const { return loopTime; }
void CServerProfile::LoopTime(std::uint32_t newVal) { loopTime = newVal; }
void CServerProfile::IncLoopTime(std::uint32_t toInc) { loopTime += toInc; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerProfile::NetworkTimeCount()
//|					CServerProfile::IncNetworkTimeCount()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets/Increments networkTimeCount
// o------------------------------------------------------------------------------------------------o
std::uint32_t CServerProfile::NetworkTimeCount() const { return networkTimeCount; }
void CServerProfile::NetworkTimeCount(std::uint32_t newVal) { networkTimeCount = newVal; }
void CServerProfile::IncNetworkTimeCount() { ++networkTimeCount; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerProfile::TimerTimeCount()
//|					CServerProfile::IncTimerTimeCount()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets/Increments timerTimeCount
// o------------------------------------------------------------------------------------------------o
std::uint32_t CServerProfile::TimerTimeCount() const { return timerTimeCount; }
void CServerProfile::TimerTimeCount(std::uint32_t newVal) { timerTimeCount = newVal; }
void CServerProfile::IncTimerTimeCount() { ++timerTimeCount; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerProfile::AutoTimeCount()
//|					CServerProfile::IncAutoTimeCount()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets/Increments autoTimeCount
// o------------------------------------------------------------------------------------------------o
std::uint32_t CServerProfile::AutoTimeCount() const { return autoTimeCount; }
void CServerProfile::AutoTimeCount(std::uint32_t newVal) { autoTimeCount = newVal; }
void CServerProfile::IncAutoTimeCount() { ++autoTimeCount; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerProfile::LoopTimeCount()
//|					CServerProfile::IncLoopTimeCount()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets/Increments amount of simulation cycles
// o------------------------------------------------------------------------------------------------o
std::uint32_t CServerProfile::LoopTimeCount() const { return loopTimeCount; }
void CServerProfile::LoopTimeCount(std::uint32_t newVal) { loopTimeCount = newVal; }
void CServerProfile::IncLoopTimeCount() { ++loopTimeCount; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerProfile::GlobalReceived()
//|					CServerProfile::GlobalReceived()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets total amount of bytes received by server
// o------------------------------------------------------------------------------------------------o
std::int32_t CServerProfile::GlobalReceived() const { return globalRecv; }
void CServerProfile::GlobalReceived(std::int32_t newVal) { globalRecv = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerProfile::GlobalSent()
//|					CServerProfile::GlobalSent()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets total amount of bytes sent by server
// o------------------------------------------------------------------------------------------------o
std::int32_t CServerProfile::GlobalSent() const { return globalSent; }
void CServerProfile::GlobalSent(std::int32_t newVal) { globalSent = newVal; }

