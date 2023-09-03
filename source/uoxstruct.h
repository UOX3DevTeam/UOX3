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
    struct CreatureAnim_st {
        std::uint8_t animId;
        std::uint8_t animLength;
        CreatureAnim_st() : animId(0), animLength(0) {}
    };

    CreatureAnim_st castAnimArea;
    CreatureAnim_st castAnimTarget;
    CreatureAnim_st castAnim3;
    CreatureAnim_st attackAnim1;
    CreatureAnim_st attackAnim2;
    CreatureAnim_st attackAnim3;

  public:
    CCreatures() : creatureId(0), icon(0), mountId(0) {
        who_am_i.reset();
        memset(soundList, 0x00, SND_COUNT * sizeof(soundList[0]));
    }
    std::uint16_t GetSound(monsterSound soundType) const { return soundList[static_cast<std::uint8_t>(soundType)]; }
    void SetSound(monsterSound soundType, std::uint16_t newVal) {
        soundList[static_cast<std::uint8_t>(soundType)] = newVal;
    }
    std::uint16_t Icon(void) const { return icon; }
    std::uint8_t WhoAmI(void) const { return static_cast<std::uint8_t>(who_am_i.to_ulong()); }

    void Icon(std::uint16_t value) { icon = value; }

    std::uint16_t MountId(void) const { return mountId; }
    void MountId(std::uint16_t value) { mountId = value; }

    // Cast Animation (Area-based)
    std::uint8_t CastAnimAreaId(void) const { return castAnimArea.animId; }
    std::uint8_t CastAnimAreaLength(void) const { return castAnimArea.animLength; }
    void CastAnimArea(std::uint8_t value, std::uint8_t length) {
        castAnimArea.animId = value;
        castAnimArea.animLength = length;
    }

    // Cast Animation (Target-based)
    std::uint8_t CastAnimTargetId(void) const { return castAnimTarget.animId; }
    std::uint8_t CastAnimTargetLength(void) const { return castAnimTarget.animLength; }
    void CastAnimTarget(std::uint8_t value, std::uint8_t length) {
        castAnimTarget.animId = value;
        castAnimTarget.animLength = length;
    }

    // Cast Animation #3
    std::uint8_t CastAnim3Id(void) const { return castAnim3.animId; }
    std::uint8_t CastAnim3Length(void) const { return castAnim3.animLength; }
    void CastAnim3(std::uint8_t value, std::uint8_t length) {
        castAnim3.animId = value;
        castAnim3.animLength = length;
    }

    // Attack Animation #1
    std::uint8_t AttackAnim1Id(void) const { return attackAnim1.animId; }
    std::uint8_t AttackAnim1Length(void) const { return attackAnim1.animLength; }
    void AttackAnim1(std::uint8_t value, std::uint8_t length) {
        attackAnim1.animId = value;
        attackAnim1.animLength = length;
    }

    // Attack Animation #2
    std::uint8_t AttackAnim2Id(void) const { return attackAnim2.animId; }
    std::uint8_t AttackAnim2Length(void) const { return attackAnim2.animLength; }
    void AttackAnim2(std::uint8_t value, std::uint8_t length) {
        attackAnim2.animId = value;
        attackAnim2.animLength = length;
    }

    // Attack Animation #3
    std::uint8_t AttackAnim3Id(void) const { return attackAnim3.animId; }
    std::uint8_t AttackAnim3Length(void) const { return attackAnim3.animLength; }
    void AttackAnim3(std::uint8_t value, std::uint8_t length) {
        attackAnim3.animId = value;
        attackAnim3.animLength = length;
    }

    std::uint16_t CreatureId(void) const { return creatureId; }
    void CreatureId(std::uint16_t value) { creatureId = value; }

    bool IsAnimal(void) const { return who_am_i.test(BIT_ANIMAL); }
    bool IsHuman(void) const { return who_am_i.test(BIT_HUMAN); }
    bool AntiBlink(void) const { return who_am_i.test(BIT_ANTIBLINK); }
    bool CanFly(void) const { return who_am_i.test(BIT_CANFLY); }
    bool IsWater(void) const { return who_am_i.test(BIT_WATER); }
    bool IsAmphibian(void) const { return who_am_i.test(BIT_AMPHI); }
    void IsAnimal(bool value) { who_am_i.set(BIT_ANIMAL, value); }
    void IsHuman(bool value) { who_am_i.set(BIT_HUMAN, value); }
    void AntiBlink(bool value) { who_am_i.set(BIT_ANTIBLINK, value); }
    void CanFly(bool value) { who_am_i.set(BIT_CANFLY, value); }
    void IsWater(bool value) { who_am_i.set(BIT_WATER, value); }
    void IsAmphibian(bool value) { who_am_i.set(BIT_AMPHI, value); }
};

struct Point3_st {
    R32 x, y, z;
    Point3_st() : x(0), y(0), z(0) {}
    Point3_st(std::uint16_t X, std::uint16_t Y, std::int8_t Z) : x(X), y(Y), z(Z) {}
    Point3_st(R32 X, R32 Y, R32 Z) : x(X), y(Y), z(Z) {}
    void Assign(std::uint16_t X, std::uint16_t Y, std::int8_t Z);
    void Assign(R32 X, R32 Y, R32 Z);
    R64 Mag(void) const;
    R32 MagSquared(void) const;
    R64 Mag3D(void) const;
    R32 MagSquared3D(void) const;
    void Normalize(void);
};

inline bool operator==(Point3_st const &a, Point3_st const &b) {
    return (a.x == b.x && a.y == b.y && a.z == b.z);
}
inline Point3_st operator+(Point3_st const &a, Point3_st const &b) {
    return (Point3_st(a.x + b.x, a.y + b.y, a.z + b.z));
}

inline Point3_st operator-(Point3_st const &a, Point3_st const &b) {
    return (Point3_st(a.x - b.x, a.y - b.y, a.z - b.z));
}
inline Point3_st operator*(Point3_st const &a, R32 const &b) {
    return Point3_st(a.x * b, a.y * b, a.z * b);
}
inline Point3_st operator*(R32 const &a, Point3_st const &b) {
    return Point3_st(a * b.x, a * b.y, a * b.z);
}
inline Point3_st operator/(Point3_st const &a, R32 const &b) {
    R32 inv = 1.f / b;
    return Point3_st(a.x * inv, a.y * inv, a.z * inv);
}
inline void Point3_st::Assign(std::uint16_t X, std::uint16_t Y, std::int8_t Z) {
    x = X;
    y = Y;
    z = Z;
}
inline void Point3_st::Assign(R32 X, R32 Y, R32 Z) {
    x = X;
    y = Y;
    z = Z;
}
inline R64 Point3_st::Mag3D(void) const { return static_cast<R32>(sqrt(x * x + y * y + z * z)); }
inline R32 Point3_st::MagSquared3D(void) const { return (x * x + y * y + z * z); }

inline R64 Point3_st::Mag(void) const { return static_cast<R32>(sqrt(x * x + y * y)); }
inline R32 Point3_st::MagSquared(void) const { return (x * x + y * y); }

inline void Point3_st::Normalize(void) {
    R32 foo = static_cast<R32>(1 / Mag3D());
    x *= foo;
    y *= foo;
    z *= foo;
}

struct UOXFileWrapper_st {
    FILE *mWrap;
};

struct GoPlaces_st {
    std::int16_t x;
    std::int16_t y;
    std::int8_t z;
    std::uint8_t worldNum;
    std::uint16_t instanceId;
    GoPlaces_st() : x(-1), y(-1), z(-1), worldNum(0), instanceId(0) {}
};

// Teleport Locations
class CTeleLocationEntry {
  private:
    Point3_st src;
    std::uint8_t srcWorld;
    Point3_st trg;
    std::uint8_t trgWorld;

  public:
    CTeleLocationEntry() : srcWorld(0xFF), trgWorld(0) {
        src.Assign(0, 0, ILLEGAL_Z);
        trg.Assign(0, 0, ILLEGAL_Z);
    }
    Point3_st SourceLocation(void) const { return src; }
    void SourceLocation(std::uint16_t x, std::uint16_t y, std::int8_t z) { src.Assign(x, y, z); }
    std::uint8_t SourceWorld(void) const { return srcWorld; }
    void SourceWorld(std::uint8_t newVal) { srcWorld = newVal; }
    Point3_st TargetLocation(void) const { return trg; }
    void TargetLocation(std::uint16_t x, std::uint16_t y, std::int8_t z) { trg.Assign(x, y, z); }
    std::uint8_t TargetWorld(void) const { return trgWorld; }
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
struct SOSLocationEntry_st {
    std::int16_t x1 = 0;
    std::int16_t y1 = 0;
    std::int16_t x2 = 0;
    std::int16_t y2 = 0;
    std::uint8_t worldNum = 0;
    std::uint16_t instanceId = 0;
};

struct Advance_st {
    std::uint16_t base;
    std::uint8_t success;
    std::uint8_t failure;
    std::uint8_t amtToGain;
    Advance_st() : base(0), success(0), failure(0), amtToGain(1) {}
};

struct TitlePair_st {
    std::int16_t lowBound;
    std::string toDisplay;
    TitlePair_st() : lowBound(0), toDisplay("") {}
    TitlePair_st(std::int16_t lB, const std::string &toDisp) : lowBound(lB) { toDisplay = toDisp; }
};
#endif
