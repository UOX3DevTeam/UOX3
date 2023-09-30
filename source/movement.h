#ifndef __MOVEMENT_H__
#define __MOVEMENT_H__

#include <cstdint>
#include <deque>
#include <map>
#include <string>
#include <vector>

#include "typedefs.h"
#include "uodata/uotile.hpp"
class CChar;
// Maximum Return Value: Number of steps to return (Replaces PATHNUM)
// NOTE: P_PF_MRV CANNOT EXCEED THE VALUE OF PATHNUM FOR THE TIME BEING
constexpr auto P_PF_MRV = 22;
// Minimum Flee Distance: MFD
constexpr auto P_PF_MFD = 15;
// Maximum Flee Distance: MAXFD
constexpr auto P_PF_MAXFD = 50;

struct PfNode_st {
    std::uint16_t hCost;
    std::uint8_t gCost;
    size_t parent;
    std::int8_t z;
    PfNode_st() : hCost(0), gCost(0), parent(0), z(0) {}
    PfNode_st(std::uint16_t nHC, std::uint8_t nGC, std::uint32_t nPS, std::int8_t nZ) : hCost(nHC), gCost(nGC), parent(nPS), z(nZ) {}
};

struct NodeFCost_st {
    std::uint32_t xySer;
    std::uint16_t fCost;
    NodeFCost_st() : xySer(0), fCost(0) {}
    NodeFCost_st(std::uint16_t nFC, std::uint32_t nS) : xySer(nS), fCost(nFC) {}
};

class CMovement {
    // Function declarations
public:
    bool AdvancedPathfinding(CChar *mChar, std::uint16_t targX, std::uint16_t targY, bool willRun = false, std::uint16_t maxSteps = 0);
    auto IgnoreAndEvadeTarget(CChar *mChar) -> void;
    void Walking(CSocket *mSock, CChar *s, std::uint8_t dir, std::int16_t sequence);
    void CombatWalk(CChar *i);
    void NpcMovement(CChar &mChar);
    void PathFind(CChar *c, std::int16_t gx, std::int16_t gy, bool willRun = false, std::uint8_t pathLen = P_PF_MRV);
    std::uint8_t Direction(CChar *c, std::int16_t x, std::int16_t y);
    bool CheckForCharacterAtXYZ(CChar *c, std::int16_t cx, std::int16_t cy, std::int8_t cz);
    std::int8_t CalcWalk(CChar *c, std::int16_t x, std::int16_t y, std::int16_t oldx, std::int16_t oldy, std::int8_t oldz, bool justask, bool waterWalk = false);
    
private:
    bool PFGrabNodes(CChar *mChar, std::uint16_t targX, std::uint16_t targY, std::uint16_t curX, std::uint16_t curY, std::int8_t curZ, std::uint32_t parentSer, std::map<std::uint32_t, PfNode_st> &openList, std::map<std::uint32_t, std::uint32_t> &closedList, std::deque<NodeFCost_st> &fCostList);
    bool CalcMove(CChar *c, std::int16_t x, std::int16_t y, std::int8_t &z, std::uint8_t dir);
    
    bool HandleNPCWander(CChar &mChar);
    bool IsValidDirection(std::uint8_t dir);
    bool IsFrozen(CChar *c, CSocket *mSock, std::int16_t sequence);
    bool IsOverloaded(CChar *c, CSocket *mSock, std::int16_t sequence);
    
    bool IsOk(std::vector<uo::UOTile> &xyblock, std::uint16_t &xycount, std::uint8_t world, std::int8_t ourZ, std::int8_t ourTop, std::int16_t x, std::int16_t y, std::uint16_t instanceId, bool ignoreDoor, bool waterWalk);
    void GetAverageZ(std::uint8_t nm, std::int16_t x, std::int16_t y, std::int8_t &z, std::int8_t &avg, std::int8_t &top);
    void GetStartZ(std::uint8_t world, CChar *c, std::int16_t x, std::int16_t y, std::int8_t z, std::int8_t &zlow, std::int8_t &ztop, std::uint16_t instanceId, bool waterWalk);
    
    void GetBlockingStatics(std::int16_t x, std::int16_t y, std::vector<uo::UOTile> &xyblock, std::uint16_t &xycount, std::uint8_t worldNumber);
    void GetBlockingDynamics(std::int16_t x, std::int16_t y, std::vector<uo::UOTile> &xyblock, std::uint16_t &xycount,  std::uint8_t worldNumber, std::uint16_t instanceId);
    
    std::uint8_t Direction(std::int16_t sx, std::int16_t sy, std::int16_t dx, std::int16_t dy);
    
    void NpcWalk(CChar *i, std::uint8_t j, std::int8_t getWander);
    std::int16_t GetXfromDir(std::uint8_t dir, std::int16_t x);
    std::int16_t GetYfromDir(std::uint8_t dir, std::int16_t y);
    
    void BoundingBoxTeleport(CChar *c, std::uint16_t fx2Actual, std::uint16_t fy2Actual, std::int16_t newz, std::int16_t newy);
    
    bool VerifySequence(CChar *c, CSocket *mSock, std::int16_t sequence);
    bool CheckForRunning(CChar *c, std::uint8_t dir);
    bool CheckForStealth(CChar *c);
    bool CheckForHouseBan(CChar *c, CSocket *mSock);
    void MoveCharForDirection(CChar *c, std::int16_t newX, std::int16_t newY, std::int8_t newZ);
    void SendWalkToPlayer(CChar *c, CSocket *mSock, std::int16_t sequence);
    void SendWalkToOtherPlayers(CChar *c, std::uint8_t dir, std::int16_t oldx, std::int16_t oldy);
    void OutputShoveMessage(CChar *c, CSocket *mSock);
    void HandleItemCollision(CChar *c, CSocket *mSock, std::int16_t oldx, std::int16_t oldy);
    bool IsGMBody(CChar *c);
    
    void DenyMovement(CSocket *mSock, CChar *s, std::int16_t sequence);
};


#endif
