#ifndef __UOXSTRUCT_H
#define __UOXSTRUCT_H

#include "typedefs.h"
#include <bitset>
#include <cmath>
#include <cstring>
#include <string>

const std::uint8_t BIT_CANFLY = 0;
const std::uint8_t BIT_ANTIBLINK = 1;
const std::uint8_t BIT_ANIMAL = 2;
const std::uint8_t BIT_WATER = 3;
const std::uint8_t BIT_AMPHI = 4;
const std::uint8_t BIT_HUMAN = 5;

class CCreatures {
    /////////////////////////////////////////////////////////////////////////////////////////////////
    // soundflags  0: normal, 5 sounds (attack-started,idle, attack, defence, dying)
    //             1: birds .. only one "bird-shape" and zillions of sounds ...
    //             2: only 3 sounds ->  (attack, defence, dying)
    //             3: only 4 sounds ->   (attack-started, attack, defence, dying)
    //             4: only 1 sound !!
    //
    // who_am_i bit # 1 creature can fly (must have the animations, so better not change)
    //              # (OUTDATED) 2 anti-blink: these creatures don't have animation #4, if not set
    //              creature will randomly disappear in battle
    //                              if you find a creature that blinks while fighting, set that bit
    //              # 3 animal-bit
    //              # 4 water creatures
    //				# 5 amphibians (water + land)
    //				# 6 human-bit
    //
    // icon: used for tracking, to set the appropriate icon
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    std::uint16_t creatureId;
    std::uint16_t soundList[SND_COUNT];
    std::bitset<6> who_am_i;
    std::uint16_t icon;
    std::uint16_t mountId;
    struct CreatureAnim {
        std::uint8_t animId;
        std::uint8_t animLength;
        CreatureAnim() : animId(0), animLength(0) {}
    };
    
    CreatureAnim castAnimArea;
    CreatureAnim castAnimTarget;
    CreatureAnim castAnim3;
    CreatureAnim attackAnim1;
    CreatureAnim attackAnim2;
    CreatureAnim attackAnim3;
    
public:
    CCreatures() : creatureId(0), icon(0), mountId(0) {
        who_am_i.reset();
        memset(soundList, 0x00, SND_COUNT * sizeof(soundList[0]));
    }
    std::uint16_t GetSound(monstersound_t soundType) const { return soundList[static_cast<std::uint8_t>(soundType)]; }
    void SetSound(monstersound_t soundType, std::uint16_t newVal) {
        soundList[static_cast<std::uint8_t>(soundType)] = newVal;
    }
    std::uint16_t Icon() const { return icon; }
    std::uint8_t WhoAmI() const { return static_cast<std::uint8_t>(who_am_i.to_ulong()); }
    
    void Icon(std::uint16_t value) { icon = value; }
    
    std::uint16_t MountId() const { return mountId; }
    void MountId(std::uint16_t value) { mountId = value; }
    
    // Cast Animation (Area-based)
    std::uint8_t CastAnimAreaId() const { return castAnimArea.animId; }
    std::uint8_t CastAnimAreaLength() const { return castAnimArea.animLength; }
    void CastAnimArea(std::uint8_t value, std::uint8_t length) {
        castAnimArea.animId = value;
        castAnimArea.animLength = length;
    }
    
    // Cast Animation (Target-based)
    std::uint8_t CastAnimTargetId() const { return castAnimTarget.animId; }
    std::uint8_t CastAnimTargetLength() const { return castAnimTarget.animLength; }
    void CastAnimTarget(std::uint8_t value, std::uint8_t length) {
        castAnimTarget.animId = value;
        castAnimTarget.animLength = length;
    }
    
    // Cast Animation #3
    std::uint8_t CastAnim3Id() const { return castAnim3.animId; }
    std::uint8_t CastAnim3Length() const { return castAnim3.animLength; }
    void CastAnim3(std::uint8_t value, std::uint8_t length) {
        castAnim3.animId = value;
        castAnim3.animLength = length;
    }
    
    // Attack Animation #1
    std::uint8_t AttackAnim1Id() const { return attackAnim1.animId; }
    std::uint8_t AttackAnim1Length() const { return attackAnim1.animLength; }
    void AttackAnim1(std::uint8_t value, std::uint8_t length) {
        attackAnim1.animId = value;
        attackAnim1.animLength = length;
    }
    
    // Attack Animation #2
    std::uint8_t AttackAnim2Id() const { return attackAnim2.animId; }
    std::uint8_t AttackAnim2Length() const { return attackAnim2.animLength; }
    void AttackAnim2(std::uint8_t value, std::uint8_t length) {
        attackAnim2.animId = value;
        attackAnim2.animLength = length;
    }
    
    // Attack Animation #3
    std::uint8_t AttackAnim3Id() const { return attackAnim3.animId; }
    std::uint8_t AttackAnim3Length() const { return attackAnim3.animLength; }
    void AttackAnim3(std::uint8_t value, std::uint8_t length) {
        attackAnim3.animId = value;
        attackAnim3.animLength = length;
    }
    
    std::uint16_t CreatureId() const { return creatureId; }
    void CreatureId(std::uint16_t value) { creatureId = value; }
    
    bool IsAnimal() const { return who_am_i.test(BIT_ANIMAL); }
    bool IsHuman() const { return who_am_i.test(BIT_HUMAN); }
    bool AntiBlink() const { return who_am_i.test(BIT_ANTIBLINK); }
    bool CanFly() const { return who_am_i.test(BIT_CANFLY); }
    bool IsWater() const { return who_am_i.test(BIT_WATER); }
    bool IsAmphibian() const { return who_am_i.test(BIT_AMPHI); }
    void IsAnimal(bool value) { who_am_i.set(BIT_ANIMAL, value); }
    void IsHuman(bool value) { who_am_i.set(BIT_HUMAN, value); }
    void AntiBlink(bool value) { who_am_i.set(BIT_ANTIBLINK, value); }
    void CanFly(bool value) { who_am_i.set(BIT_CANFLY, value); }
    void IsWater(bool value) { who_am_i.set(BIT_WATER, value); }
    void IsAmphibian(bool value) { who_am_i.set(BIT_AMPHI, value); }
};

struct Point3 {
    R32 x, y, z;
    Point3() : x(0), y(0), z(0) {}
    Point3(std::uint16_t X, std::uint16_t Y, std::int8_t Z) : x(X), y(Y), z(Z) {}
    Point3(R32 X, R32 Y, R32 Z) : x(X), y(Y), z(Z) {}
    void Assign(std::uint16_t X, std::uint16_t Y, std::int8_t Z);
    void Assign(R32 X, R32 Y, R32 Z);
    R64 Mag() const;
    R32 MagSquared() const;
    R64 Mag3D() const;
    R32 MagSquared3D() const;
    void Normalize();
};

inline bool operator==(Point3 const &a, Point3 const &b) {
    return (a.x == b.x && a.y == b.y && a.z == b.z);
}
inline Point3 operator+(Point3 const &a, Point3 const &b) {
    return (Point3(a.x + b.x, a.y + b.y, a.z + b.z));
}

inline Point3 operator-(Point3 const &a, Point3 const &b) {
    return (Point3(a.x - b.x, a.y - b.y, a.z - b.z));
}
inline Point3 operator*(Point3 const &a, R32 const &b) {
    return Point3(a.x * b, a.y * b, a.z * b);
}
inline Point3 operator*(R32 const &a, Point3 const &b) {
    return Point3(a * b.x, a * b.y, a * b.z);
}
inline Point3 operator/(Point3 const &a, R32 const &b) {
    R32 inv = 1.f / b;
    return Point3(a.x * inv, a.y * inv, a.z * inv);
}
inline void Point3::Assign(std::uint16_t X, std::uint16_t Y, std::int8_t Z) {
    x = X;
    y = Y;
    z = Z;
}
inline void Point3::Assign(R32 X, R32 Y, R32 Z) {
    x = X;
    y = Y;
    z = Z;
}
inline R64 Point3::Mag3D() const { return static_cast<R32>(sqrt(x * x + y * y + z * z)); }
inline R32 Point3::MagSquared3D() const { return (x * x + y * y + z * z); }

inline R64 Point3::Mag() const { return static_cast<R32>(sqrt(x * x + y * y)); }
inline R32 Point3::MagSquared() const { return (x * x + y * y); }

inline void Point3::Normalize() {
    R32 foo = static_cast<R32>(1 / Mag3D());
    x *= foo;
    y *= foo;
    z *= foo;
}

struct UOXFileWrapper {
    FILE *mWrap;
};

struct GoPlaces {
    std::int16_t x;
    std::int16_t y;
    std::int8_t z;
    std::uint8_t worldNum;
    std::uint16_t instanceId;
    GoPlaces() : x(-1), y(-1), z(-1), worldNum(0), instanceId(0) {}
};

// Teleport Locations
class CTeleLocationEntry {
private:
    Point3 src;
    std::uint8_t srcWorld;
    Point3 trg;
    std::uint8_t trgWorld;
    
public:
    CTeleLocationEntry() : srcWorld(0xFF), trgWorld(0) {
        src.Assign(0, 0, ILLEGAL_Z);
        trg.Assign(0, 0, ILLEGAL_Z);
    }
    Point3 SourceLocation() const { return src; }
    void SourceLocation(std::uint16_t x, std::uint16_t y, std::int8_t z) { src.Assign(x, y, z); }
    std::uint8_t SourceWorld() const { return srcWorld; }
    void SourceWorld(std::uint8_t newVal) { srcWorld = newVal; }
    Point3 TargetLocation() const { return trg; }
    void TargetLocation(std::uint16_t x, std::uint16_t y, std::int8_t z) { trg.Assign(x, y, z); }
    std::uint8_t TargetWorld() const { return trgWorld; }
    void TargetWorld(std::uint8_t newVal) { trgWorld = newVal; }
};

// Instalog Locations
struct LogoutLocationEntry_st {
    std::int16_t x1 = 0;
    std::int16_t y1 = 0;
    std::int16_t x2 = 0;
    std::int16_t y2 = 0;
    std::uint8_t worldNum = 0;
    std::uint16_t instanceId = 0;
};

// SOS Locations
struct SOSLocationEntry {
    std::int16_t x1 = 0;
    std::int16_t y1 = 0;
    std::int16_t x2 = 0;
    std::int16_t y2 = 0;
    std::uint8_t worldNum = 0;
    std::uint16_t instanceId = 0;
};

struct Advance {
    std::uint16_t base;
    std::uint8_t success;
    std::uint8_t failure;
    std::uint8_t amtToGain;
    Advance() : base(0), success(0), failure(0), amtToGain(1) {}
};

struct TitlePair {
    std::int16_t lowBound;
    std::string toDisplay;
    TitlePair() : lowBound(0), toDisplay("") {}
    TitlePair(std::int16_t lB, const std::string &toDisp) : lowBound(lB) { toDisplay = toDisp; }
};
#endif
