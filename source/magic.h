#ifndef __MAGIC_H
#define __MAGIC_H
#include <cstdint>
#include <string>

#include "typedefs.h"

class CSocket;
class CChar;
class CItem;

class CMagicMove {
private:
    std::uint16_t effect;
    std::uint8_t speed;
    std::uint8_t loop;
    std::uint8_t explode;
    
public:
    CMagicMove() : effect(INVALIDID), speed(0), loop(0), explode(0) {}
    
    std::uint16_t Effect() const { return effect; }
    std::uint8_t Speed() const { return speed; }
    std::uint8_t Loop() const { return loop; }
    std::uint8_t Explode() const { return explode; }
    void Effect(std::uint8_t p1, std::uint8_t p2) { effect = static_cast<std::uint16_t>((p1 << 8) + p2); }
    void Speed(std::uint8_t newVal) { speed = newVal; }
    void Loop(std::uint8_t newVal) { loop = newVal; }
    void Explode(std::uint8_t newVal) { explode = newVal; }
};

class CMagicStat {
private:
    std::uint16_t effect;
    std::uint8_t speed;
    std::uint8_t loop;
    
public:
    CMagicStat() : effect(INVALIDID), speed(0xFF), loop(0xFF) {}
    std::uint16_t Effect() const { return effect; }
    std::uint8_t Speed() const { return speed; }
    std::uint8_t Loop() const { return loop; }
    void Effect(std::uint8_t p1, std::uint8_t p2) { effect = static_cast<std::uint16_t>((p1 << 8) + p2); }
    void Speed(std::uint8_t newVal) { speed = newVal; }
    void Loop(std::uint8_t newVal) { loop = newVal; }
};

struct Reag_st {
    std::uint8_t ginseng;
    std::uint8_t moss;
    std::uint8_t drake;
    std::uint8_t pearl;
    std::uint8_t silk;
    std::uint8_t ash;
    std::uint8_t shade;
    std::uint8_t garlic;
    Reag_st() : ginseng(0), moss(0), drake(0), pearl(0), silk(0), ash(0), shade(0), garlic(0) {}
};

class CSpellInfo {
private:
    std::int16_t mana;
    std::int16_t stamina;
    std::int16_t health;
    float delay;         // Casting time of spell
    float damageDelay;   // Minimum delay between targeting of a damage spell and the application of
    // damage
    float recoveryDelay; // Minimum delay between the end of one spellcast and the start of another
    std::uint16_t action;
    Reag_st reags;
    std::string mantra;
    std::string strToSay; // string visualized with targ. system
    bool enabled;
    std::uint8_t circle;
    std::uint16_t flags;
    CMagicMove moveEffect;
    std::uint16_t effect;
    CMagicStat staticEffect;
    std::int16_t hiskill;
    std::int16_t loskill;
    std::int16_t sclo;
    std::int16_t schi;
    std::uint16_t jsScript;
    std::int16_t baseDmg;
    
public:
    CSpellInfo()
    : mana(0), stamina(0), health(0), delay(0), damageDelay(0), recoveryDelay(1.0f), action(0),
    mantra(""), strToSay(""), enabled(false), circle(1), flags(0), effect(INVALIDID),
    hiskill(0), loskill(0), sclo(0), schi(0), jsScript(0), baseDmg(0) {}
    
    std::uint16_t Action() const { return action; }
    float Delay() const { return delay; }
    float DamageDelay() const { return damageDelay; }
    float RecoveryDelay() const { return recoveryDelay; }
    std::int16_t Health() const { return health; }
    std::int16_t Stamina() const { return stamina; }
    std::int16_t Mana() const { return mana; }
    
    void Action(std::uint16_t newVal) { action = newVal; }
    void Delay(float newVal) { delay = newVal; }
    void DamageDelay(float newVal) { damageDelay = newVal; }
    void RecoveryDelay(float newVal) { recoveryDelay = newVal; }
    void Health(std::int16_t newVal) { health = newVal; }
    void Stamina(std::int16_t newVal) { stamina = newVal; }
    void Mana(std::int16_t newVal) { mana = newVal; }
    std::int16_t BaseDmg() const { return baseDmg; }
    Reag_st Reagants() const { return reags; }
    Reag_st *ReagantsPtr() { return &reags; }
    const std::string Mantra() const { return mantra; }
    const std::string StringToSay() const { return strToSay; }
    std::int16_t ScrollLow() const { return sclo; }
    std::int16_t ScrollHigh() const { return schi; }
    std::int16_t HighSkill() const { return hiskill; }
    std::int16_t LowSkill() const { return loskill; }
    std::uint8_t Circle() const { return circle; }
    bool Enabled() const { return enabled; }
    bool RequireTarget() const { return (flags & 0x01) == 0x01; }
    bool RequireItemTarget() const { return (flags & 0x02) == 0x02; }
    bool RequireLocTarget() const { return (flags & 0x04) == 0x04; }
    bool RequireCharTarget() const { return (flags & 0x08) == 0x08; }
    bool RequireNoTarget() const { return (flags & 0x00) == 0x00; }
    bool TravelSpell() const { return (flags & 0x10) == 0x10; }
    bool FieldSpell() const { return (flags & 0x20) == 0x20; }
    bool SpellReflectable() const { return (flags & 0x40) == 0x40; }
    bool AggressiveSpell() const { return (flags & 0x80) == 0x80; }
    bool Resistable() const { return (flags & 0x100) == 0x100; }
    void Mantra(const std::string &toPut) { mantra = toPut; }
    void StringToSay(const std::string &toPut) { strToSay = toPut; }
    void BaseDmg(std::int16_t newVal) { baseDmg = newVal; }
    void ScrollLow(std::int16_t newVal) { sclo = newVal; }
    void ScrollHigh(std::int16_t newVal) { schi = newVal; }
    void HighSkill(std::int16_t newVal) { hiskill = newVal; }
    void LowSkill(std::int16_t newVal) { loskill = newVal; }
    void Circle(std::uint8_t newVal) { circle = newVal; }
    void Enabled(bool newVal) { enabled = newVal; }
    void Flags(std::uint16_t newVal) { flags = newVal; }
    std::uint16_t JSScript() { return jsScript; }
    void JSScript(std::uint16_t scpAssign) { jsScript = scpAssign; }
    std::uint16_t Effect() const { return effect; }
    void Effect(std::uint16_t newVal) { effect = newVal; }
    CMagicMove MoveEffect() const { return moveEffect; }
    CMagicStat StaticEffect() const { return staticEffect; }
    CMagicMove *MoveEffectPtr() { return &moveEffect; }
    CMagicStat *StaticEffectPtr() { return &staticEffect; }
};

#define MAGIC_CHARTARG_LIST CChar *, CChar *, CChar *, std::int8_t
#define MAGIC_ITEMTARG_LIST CSocket *, CChar *, CItem *, std::int8_t
#define MAGIC_LOCATION_LIST CSocket *, CChar *, std::int16_t, std::int16_t, std::int8_t, std::int8_t
#define MAGIC_FIELD_LIST CSocket *, CChar *, std::uint8_t, std::int16_t, std::int16_t, std::int8_t, std::int8_t
#define MAGIC_AREA_STUB_LIST CChar *, CChar *, std::int8_t, std::int8_t
#define MAGIC_NOTARG_LIST CSocket *, CChar *, std::int8_t

#define MAGIC_TEST_LIST CSocket *, CChar *, CChar *, CChar *, std::int8_t

#define MAGIC_TESTFUNC bool (*)(MAGIC_TEST_LIST)
#define MAGIC_CHARFUNC bool (*)(MAGIC_CHARTARG_LIST)
#define MAGIC_ITEMFUNC bool (*)(MAGIC_ITEMTARG_LIST)
#define MAGIC_LOCFUNC bool (*)(MAGIC_LOCATION_LIST)
#define MAGIC_FIELDFUNC bool (*)(MAGIC_FIELD_LIST)
#define MAGIC_NOFUNC bool (*)(MAGIC_NOTARG_LIST)

#define MAGIC_STUBFUNC void (*)(MAGIC_AREA_STUB_LIST)

#define MAG_TESTHANDLER(name) extern bool name(MAGIC_TEST_LIST)
#define MAG_CHARHANDLER(name) extern bool name(MAGIC_CHARTARG_LIST)
#define MAG_ITEMHANDLER(name) extern bool name(MAGIC_ITEMTARG_LIST)
#define MAG_LOCHANDLER(name) extern bool name(MAGIC_LOCATION_LIST)
#define MAG_FIELDHANDLER(name) extern bool name(MAGIC_FIELD_LIST)
#define MAG_NOHANDLER(name) extern bool name(MAGIC_NOTARG_LIST)
#define MAG_AREASTUB(name) extern void name(MAGIC_AREA_STUB_LIST)

#define MAGIC_DEFN bool (*)(...)

struct MagicTable_st {
    std::int32_t spell_name;
    bool (*mag_extra)(...); // extra data - see above
};

bool AreaAffectSpell(CSocket *sock, CChar *caster, void (*trgFunc)(MAGIC_AREA_STUB_LIST),
                     std::int8_t curSpell);
bool DiamondSpell(CSocket *sock, CChar *caster, std::uint16_t id, std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t length);
bool FieldSpell(CChar *caster, std::uint16_t id, std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t fieldDir, std::int8_t spellNum);
bool FloodSpell(CSocket *sock, CChar *caster, std::uint16_t id, std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t length);
bool SquareSpell(CSocket *sock, CChar *caster, std::uint16_t id, std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t length);
std::int16_t CalcSpellDamageMod(CChar *caster, CChar *target, std::int16_t baseDamage, bool spellResisted);

extern const MagicTable_st magic_table[];

MAG_CHARHANDLER(splClumsy);
MAG_NOHANDLER(splCreateFood);
MAG_CHARHANDLER(splFeeblemind);
MAG_CHARHANDLER(splHeal);
MAG_CHARHANDLER(splMagicArrow);
MAG_CHARHANDLER(splNightSight);
MAG_CHARHANDLER(splReactiveArmor);
MAG_CHARHANDLER(splWeaken);
MAG_CHARHANDLER(splAgility);
MAG_CHARHANDLER(splCunning);
MAG_CHARHANDLER(splCure);
MAG_CHARHANDLER(splHarm);
MAG_ITEMHANDLER(splMagicTrap);
MAG_ITEMHANDLER(splMagicUntrap);
MAG_CHARHANDLER(splProtection);
MAG_CHARHANDLER(splStrength);
MAG_CHARHANDLER(splBless);
MAG_CHARHANDLER(splFireball);
MAG_ITEMHANDLER(splMagicLock);
MAG_CHARHANDLER(splPoison);
MAG_ITEMHANDLER(splTelekinesis);
MAG_LOCHANDLER(splTeleport);
MAG_ITEMHANDLER(splUnlock);
MAG_FIELDHANDLER(splWallOfStone);
MAG_TESTHANDLER(splArchCure);
MAG_TESTHANDLER(splArchProtection);
MAG_CHARHANDLER(splCurse);
MAG_FIELDHANDLER(splFireField);
MAG_CHARHANDLER(splGreaterHeal);
MAG_CHARHANDLER(splLightning);
MAG_CHARHANDLER(splManaDrain);
MAG_ITEMHANDLER(splRecall);
MAG_LOCHANDLER(splBladeSpirits);
MAG_NOHANDLER(splDispelField);
MAG_NOHANDLER(splIncognito);
MAG_NOHANDLER(splMagicReflection);
MAG_CHARHANDLER(splMindBlast);
MAG_CHARHANDLER(splParalyze);
MAG_FIELDHANDLER(splPoisonField);
MAG_LOCHANDLER(splSummonCreature);
MAG_CHARHANDLER(splDispel);
MAG_CHARHANDLER(splEnergyBolt);
MAG_CHARHANDLER(splExplosion);
MAG_CHARHANDLER(splInvisibility);
MAG_ITEMHANDLER(splMark);
MAG_TESTHANDLER(splMassCurse);
MAG_FIELDHANDLER(splParalyzeField);
MAG_LOCHANDLER(splReveal);
MAG_TESTHANDLER(splChainLightning);
MAG_FIELDHANDLER(splEnergyField);
MAG_CHARHANDLER(splFlameStrike);
MAG_ITEMHANDLER(splGateTravel);
MAG_CHARHANDLER(splManaVampire);
MAG_TESTHANDLER(splMassDispel);
MAG_TESTHANDLER(splMeteorSwarm);
MAG_NOHANDLER(splPolymorph);
MAG_NOHANDLER(splEarthquake);
MAG_LOCHANDLER(splEnergyVortex);
MAG_CHARHANDLER(splResurrection);
MAG_NOHANDLER(splSummonAir);
MAG_NOHANDLER(splSummonDaemon);
MAG_NOHANDLER(splSummonEarth);
MAG_NOHANDLER(splSummonFire);
MAG_NOHANDLER(splSummonWater);
MAG_NOHANDLER(splRandom);
MAG_CHARHANDLER(splNecro1);
MAG_NOHANDLER(splNecro2);
MAG_CHARHANDLER(splNecro3);
MAG_CHARHANDLER(splNecro4);
MAG_CHARHANDLER(splNecro5);

// All command_ functions take an std::int32_t value of the player that triggered the command.

#endif
