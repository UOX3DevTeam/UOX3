
#ifndef __JAIL_H__
#define __JAIL_H__

#include <cstdint>
#include <string>
#include <vector>

#include "typedefs.h"

class CChar;

struct JailOccupant_st {
    serial_t pSerial;
    std::int16_t x;
    std::int16_t y;
    std::int8_t z;
    std::uint8_t world;
    std::uint16_t instanceId;
    time_t releaseTime;
    JailOccupant_st()
    : pSerial(INVALIDSERIAL), x(0), y(0), z(0), world(0), instanceId(0), releaseTime(0) {}
};

class CJailCell {
private:
    std::int16_t x;
    std::int16_t y;
    std::int8_t z;
    std::uint8_t world;
    std::uint16_t instanceId;
    std::vector<JailOccupant_st *> playersInJail;
    
public:
    CJailCell() : x(0), y(0), z(0), world(0), instanceId(0) {}
    ~CJailCell();
    bool IsEmpty() const;
    size_t JailedPlayers() const;
    std::int16_t X() const;
    std::int16_t Y() const;
    std::int8_t Z() const;
    std::uint8_t World() const;
    std::uint16_t InstanceId() const;
    void X(std::int16_t nVal);
    void Y(std::int16_t nVal);
    void Z(std::int8_t nVal);
    void World(std::uint8_t nVal);
    void InstanceId(std::uint16_t nVal);
    void AddOccupant(CChar *pAdd, std::int32_t secsFromNow);
    void EraseOccupant(size_t occupantId);
    JailOccupant_st *Occupant(size_t occupantId);
    void PeriodicCheck();
    void AddOccupant(JailOccupant_st *toAdd);
    void WriteData(std::ostream &outStream, size_t cellNum);
};

class CJailSystem {
private:
    std::vector<CJailCell> jails;
    
public:
    CJailSystem() = default;
    ~CJailSystem() = default;
    void ReadSetup();
    void ReadData();
    void WriteData();
    void PeriodicCheck();
    bool JailPlayer(CChar *toJail, std::int32_t numSecsToJail);
    void ReleasePlayer(CChar *toRelase);
};


#endif
