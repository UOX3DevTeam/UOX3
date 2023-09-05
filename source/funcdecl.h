// o------------------------------------------------------------------------------------------------o
//| File		-	funcdecl.h
// o------------------------------------------------------------------------------------------------o
//| Purpose		-	This header file contains all of our globally declared functions
// o------------------------------------------------------------------------------------------------o
//| Notes		-	Version History
//|						2.0		10/16/2003
//|						Many functions removed, overall reorganization and updated
//to match |						the rest of UOX3.
// o------------------------------------------------------------------------------------------------o
#ifndef __FUNCDECLS_H__
#define __FUNCDECLS_H__
#include "stringutility.hpp"
#include "subsystem/console.hpp"
#include "uoxstruct.h"
#include "utility/strutil.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <sstream>
#include <thread>
#include <type_traits>

#include "cbaseobject.h"
#include "osunique.hpp"
#include "worldmain.h"
class CItem;
class CTownRegion;

extern CWorldMain *cwmWorldState;
extern std::mt19937 generator;

// o------------------------------------------------------------------------------------------------o
//  Range check functions
// o------------------------------------------------------------------------------------------------o
bool ObjInRange(CSocket *mSock, CBaseObject *obj, std::uint16_t distance);
bool ObjInRange(CBaseObject *a, CBaseObject *b, std::uint16_t distance);
bool ObjInRangeSquare(CBaseObject *a, CBaseObject *b, std::uint16_t distance);
bool ObjInOldRange(CBaseObject *a, CBaseObject *b, std::uint16_t distance);
bool ObjInOldRangeSquare(CBaseObject *a, CBaseObject *b, std::uint16_t distance);
bool CharInRange(CChar *a, CChar *b);
std::uint16_t GetDist(CBaseObject *a, CBaseObject *b);
std::uint16_t GetDist(Point3 a, Point3 b);
std::uint16_t GetOldDist(CBaseObject *a, CBaseObject *b);
std::uint16_t GetDist3D(CBaseObject *a, CBaseObject *b);
std::uint16_t GetDist3D(Point3 a, Point3 b);
auto FindPlayersInVisrange(CBaseObject *myObj) -> std::vector<CSocket *>;
auto FindPlayersInOldVisrange(CBaseObject *myObj) -> std::vector<CSocket *>;
auto FindNearbyPlayers(std::int16_t x, std::int16_t y, std::int8_t z, std::uint16_t distance) -> std::vector<CSocket *>;
auto FindNearbyPlayers(CBaseObject *myObj, std::uint16_t distance) -> std::vector<CSocket *>;
auto FindNearbyPlayers(CBaseObject *myObj) -> std::vector<CSocket *>;
auto FindNearbyPlayers(CChar *mChar) -> std::vector<CSocket *>;

// o------------------------------------------------------------------------------------------------o
//  Multi functions
// o------------------------------------------------------------------------------------------------o
CMultiObj *FindMulti(std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::uint16_t instanceId);
CMultiObj *FindMulti(CBaseObject *i);

// o------------------------------------------------------------------------------------------------o
//  Item functions
// o------------------------------------------------------------------------------------------------o
CItem *GetItemAtXYZ(std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::uint16_t instanceId);
CItem *FindItemNearXYZ(std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::uint16_t id, std::uint16_t instanceId);

// o------------------------------------------------------------------------------------------------o
//  Calculation functions (socket, char, item and so forth)
// o------------------------------------------------------------------------------------------------o
CItem *CalcItemObjFromSer(serial_t targSerial);
CChar *CalcCharObjFromSer(serial_t targSerial);
CMultiObj *CalcMultiFromSer(serial_t targSerial);
inline std::uint32_t CalcSerial(std::uint8_t a1, std::uint8_t a2, std::uint8_t a3, std::uint8_t a4) {
    return ((a1 << 24) + (a2 << 16) + (a3 << 8) + a4);
}

// o------------------------------------------------------------------------------------------------o
//  Socket stuff
// o------------------------------------------------------------------------------------------------o
auto SendVecsAsGump(CSocket *sock, std::vector<std::string> &one, std::vector<std::string> &two,
                    std::uint32_t type, serial_t serial) -> void;
void SendMapChange(std::uint8_t worldNumber, CSocket *sock, bool initialLogin = false);
bool IsOnline(CChar &mChar);

// o------------------------------------------------------------------------------------------------o
//  Light related functions
// o------------------------------------------------------------------------------------------------o
void DoLight(CSocket *s, std::uint8_t level);
void DoLight(CChar *mChar, std::uint8_t level);
void DoLight(CItem *mItem, std::uint8_t level);

// o------------------------------------------------------------------------------------------------o
//  Poison related functions
// o------------------------------------------------------------------------------------------------o
timerval_t GetPoisonDuration(std::uint8_t poisonStrength);
timerval_t GetPoisonTickTime(std::uint8_t poisonStrength);

// o------------------------------------------------------------------------------------------------o
//  Amount related
// o------------------------------------------------------------------------------------------------o
std::uint32_t GetItemAmount(CChar *s, std::uint16_t realId, std::uint16_t realColour = 0x0000, std::uint32_t realMoreVal = 0x0,
                            bool colorCheck = false, bool moreCheck = false, std::string sectionId = "");
std::uint32_t GetTotalItemCount(CItem *objCont);
std::uint32_t DeleteItemAmount(CChar *s, std::uint32_t amount, std::uint16_t realId, std::uint16_t realColour = 0x0000,
                               std::uint32_t realMoreVal = 0x0, bool colorCheck = false, bool moreCheck = false,
                               std::string sectionId = "");
std::uint32_t DeleteSubItemAmount(CItem *p, std::uint32_t amount, std::uint16_t realId, std::uint16_t realColour = 0x0000,
                                  std::uint32_t realMoreVal = 0x0, bool colorCheck = false, bool moreCheck = false,
                                  std::string sectionId = "");
std::uint32_t GetBankCount(CChar *p, std::uint16_t itemId, std::uint16_t realColour = 0x0000, std::uint32_t realMoreVal = 0x0);
std::uint32_t DeleteBankItem(CChar *p, std::uint32_t amt, std::uint16_t itemId, std::uint16_t realColour = 0x0000,
                             std::uint32_t realMoreVal = 0x0);

// o------------------------------------------------------------------------------------------------o
//  Region related
// o------------------------------------------------------------------------------------------------o
CTownRegion *CalcRegionFromXY(std::int16_t x, std::int16_t y, std::uint8_t worldNumber, std::uint16_t instanceId,
                              CBaseObject *mObj = nullptr);
void CheckCharInsideBuilding(CChar *c, CSocket *mSock, bool doWeatherStuff);

// o------------------------------------------------------------------------------------------------o
//  Find functions
// o------------------------------------------------------------------------------------------------o
CChar *FindItemOwner(CItem *p);
CBaseObject *FindItemOwner(CItem *i, CBaseObject::type_t &objType);
CItem *FindRootContainer(CItem *i);
CItem *FindItemOfType(CChar *toFind, itemtypes_t type);
CItem *FindItemOfSectionId(CChar *toFind, std::string sectionId);
CItem *FindItem(CChar *toFind, std::uint16_t itemId);

// o------------------------------------------------------------------------------------------------o
//  Reputation Stuff
// o------------------------------------------------------------------------------------------------o
void Karma(CChar *nCharId, CChar *nKilledId, const std::int16_t nKarma);
void Fame(CChar *nCharId, const std::int16_t nFame);
void UpdateFlag(CChar *mChar);

// o------------------------------------------------------------------------------------------------o
//  Combat Stuff
// o------------------------------------------------------------------------------------------------o
void MakeCriminal(CChar *c);
void FlagForStealing(CChar *c);
bool WillResultInCriminal(CChar *mChar, CChar *targ);
void CallGuards(CChar *mChar, CChar *targChar);
void CallGuards(CChar *mChar);

// o------------------------------------------------------------------------------------------------o
//  Time Functions
// o------------------------------------------------------------------------------------------------o
inline timerval_t BuildTimeValue(R32 timeFromNow) {
    return static_cast<timerval_t>(cwmWorldState->GetUICurrentTime() +
                                   (static_cast<R32>(1000) * timeFromNow));
}

std::uint32_t GetClock();
inline char *RealTime(char *time_str) {
    auto timet = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm curtime;
    lcltime(timet, curtime);
    strftime(time_str, 256, "%B %d %I:%M:%S %p", &curtime);
    return time_str;
}
inline char *RealTime24(char *time_str) {
    auto timet = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm curtime;
    lcltime(timet, curtime);
    strftime(time_str, 256, "%B %d %H:%M:%S", &curtime);
    return time_str;
}
inline char *RealTimeDate(char *time_str) {
    auto timet = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm curtime;
    lcltime(timet, curtime);
    strftime(time_str, 256, "%B %d", &curtime);
    return time_str;
}

#if P_TIMESTAMP
inline std::string TimeStamp() {
    auto stamp = []() {
        auto milli = std::chrono::duration_cast<std::chrono::milliseconds>(
                                                                           std::chrono::system_clock::now().time_since_epoch())
        .count();
        auto hours = (((milli / 1000) / 60) / 60);
        milli = milli - (hours * 60 * 60 * 1000);
        auto minutes = ((milli / 1000) / 60);
        milli = milli - (minutes * 1000 * 60);
        auto seconds = milli / 1000;
        milli = milli - (seconds * 1000);
        std::stringstream value;
        char timeStamp[512];
        RealTime(timeStamp);
        value << " [" << timeStamp << "][" << milli << "]";
        return value.str();
    };
    
    return stamp();
}
#else
inline std::string TimeStamp() { return ""; }
#endif
inline void StartMilliTimer(std::uint32_t &Seconds, std::uint32_t &Milliseconds) {
    auto timenow = std::chrono::system_clock::now().time_since_epoch();
    auto sec = std::chrono::duration_cast<std::chrono::seconds>(timenow).count();
    auto milli =
    std::chrono::duration_cast<std::chrono::milliseconds>(timenow).count() - (1000 * sec);
    Seconds = static_cast<std::uint32_t>(sec);
    Milliseconds = static_cast<std::uint32_t>(milli);
}
inline std::uint32_t CheckMilliTimer(std::uint32_t &Seconds, std::uint32_t &Milliseconds) {
    auto timenow = std::chrono::system_clock::now().time_since_epoch();
    auto sec = std::chrono::duration_cast<std::chrono::seconds>(timenow).count();
    auto milli =
    std::chrono::duration_cast<std::chrono::milliseconds>(timenow).count() - (1000 * sec);
    return static_cast<std::uint32_t>((1000 * (sec - Seconds)) + (milli - Milliseconds));
}

inline std::uint32_t GetMinutesSinceEpoch() {
    return static_cast<std::uint32_t>(std::chrono::duration_cast<std::chrono::minutes>(
                                                                                       std::chrono::system_clock::now().time_since_epoch())
                                      .count());
}

// o------------------------------------------------------------------------------------------------o
//  Misc Functions
// o------------------------------------------------------------------------------------------------o
R32 RoundNumber(R32 toRound);
bool IsNumber(const std::string &str);
bool FileExists(const std::string &filepath);
void DismountCreature(CChar *s);
size_t GetTileName(CItem &mItem, std::string &itemname);
std::string GetNpcDictName(CChar *mChar, CSocket *tSock = nullptr, std::uint8_t requestSource = 0);
std::string GetNpcDictTitle(CChar *mChar, CSocket *tSock = nullptr);
bool LineOfSight(CSocket *s, CChar *mChar, std::int16_t x2, std::int16_t y2, std::int8_t z2, std::uint8_t checkfor,
                 bool useSurfaceZ, std::int8_t z2Top = 0, bool checkDistance = true);
bool CheckItemLineOfSight(CChar *mChar, CItem *i);
void Shutdown(std::int32_t retCode);
void HandleDeath(CChar *mChar, CChar *attacker);
void NpcResurrectTarget(CChar *s);

inline bool ValidateObject(const CBaseObject *toValidate) {
    bool rValue = true;
    try {
        if (toValidate == nullptr) {
            rValue = false;
        }
        else if (toValidate->IsDeleted()) {
            rValue = false;
        }
    } catch (...) {
        rValue = false;
        Console::shared().error(util::format("Invalid Object found: 0x%X", (std::uint64_t)toValidate));
    }
    return rValue;
}

template <class T> inline T RandomNum(T nLowNum, T nHighNum) {
    auto high = std::max<T>(nLowNum, nHighNum);
    auto low = std::min<T>(nLowNum, nHighNum);
    if (std::is_floating_point<T>::value) {
        auto distribution = std::uniform_real_distribution<double>(low, high);
        return distribution(generator);
    }
    auto distribution = std::uniform_int_distribution<T>(low, high);
    return distribution(generator);
}

template <class T> inline T HalfRandomNum(T HighRange) {
    T halfSize = static_cast<T>(HighRange / 2);
    return RandomNum(halfSize, HighRange);
}

#endif
