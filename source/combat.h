#ifndef __COMBAT_H__
#define __COMBAT_H__

#include <cstdint>
#include <string>

#include "typedefs.h"

class CSocket;
class CChar;
class CItem;
class CBaseObject;

enum WeaponTypes {
    DEF_SWORDS = 1,
    SLASH_SWORDS,
    ONEHND_LG_SWORDS,
    TWOHND_LG_SWORDS,
    BARDICHE,
    ONEHND_AXES,
    TWOHND_AXES,
    DEF_MACES,
    LG_MACES,
    BOWS,
    XBOWS,
    DEF_FENCING,
    TWOHND_FENCING,
    DUAL_SWORD,
    DUAL_FENCING_STAB,
    DUAL_FENCING_SLASH,
    BLOWGUNS,
    THROWN
};

class CHandleCombat {
private:
    bool HandleCombat(CSocket *mSock, CChar &mChar, CChar *ourTarg);
    
    bool CastSpell(CChar *mChar, CChar *ourTarg, std::int8_t spellNum);
    
    std::int16_t CalcDamage(CChar *mChar, CChar *ourTarg, std::uint8_t getFightSkill, std::uint8_t hitLoc);
    
    void PlaySwingAnimations(CChar *mChar);
    void PlayHitSoundEffect(CChar *mChar, CItem *mWeapon);
    void PlayMissedSoundEffect(CChar *mChar);
    
    void HandleNPCSpellAttack(CChar *mChar, CChar *ourTarg, std::uint16_t playerDistance);
    
    CItem *CheckDef(CItem *checkItem, CItem *currItem, std::int32_t &currDef, weathertype_t resistType);
    CItem *GetArmorDef(CChar *mChar, std::int32_t &totalDef, std::uint8_t bodyLoc, bool findTotal = false,
                       weathertype_t resistType = NONE);
    
public:
    bool StartAttack(CChar *mChar, CChar *ourTarg);
    void InvalidateAttacker(CChar *mChar);
    
    R32 GetCombatTimeout(CChar *mChar);
    void PlayerAttack(CSocket *s);
    void AttackTarget(CChar *mChar, CChar *ourTarg);
    void PetGuardAttack(CChar *mChar, CChar *owner, CBaseObject *guarded,
                        CChar *petGuard = nullptr);
    
    void CombatLoop(CSocket *mSock, CChar &mChar);
    void Kill(CChar *mChar, CChar *ourTarg);
    
    void DoHitMessage(CChar *mChar, CChar *ourTarg, std::int8_t hitLoc, std::int16_t damage);
    std::int8_t CalculateHitLoc();
    std::uint16_t CalcDef(CChar *mChar, std::uint8_t hitLoc, bool doDamage = false, weathertype_t element = PHYSICAL);
    std::int16_t CalcAttackPower(CChar *mChar, bool doDamage = false);
    std::int16_t CalcLowDamage(CChar *p);
    std::int16_t CalcHighDamage(CChar *p);
    std::uint8_t GetCombatSkill(CItem *wItem);
    std::uint8_t GetBowType(CItem *bItem);
    std::uint8_t GetWeaponType(CItem *i);
    
    CItem *GetShield(CChar *mChar);
    CItem *GetWeapon(CChar *mChar);
    
    std::int16_t ApplyDamageBonuses(weathertype_t damageType, CChar *mChar, CChar *ourTarg,
                                    std::uint8_t getFightSkill, std::uint8_t hitLoc, std::int16_t baseDamage);
    std::int16_t ApplyDefenseModifiers(weathertype_t damageType, CChar *mChar, CChar *ourTarg,
                                       std::uint8_t getFightSkill, std::uint8_t hitLoc, std::int16_t baseDamage,
                                       bool doArmorDamage);
    
    std::int16_t AdjustRaceDamage(CChar *attack, CChar *defend, CItem *weapon, std::int16_t bDamage, std::uint8_t hitLoc,
                                  std::uint8_t getFightSkill);
    std::int16_t AdjustArmorClassDamage(CChar *attack, CChar *defend, CItem *weapon, std::int16_t bDamage,
                                        std::uint8_t hitLoc);
    void SpawnGuard(CChar *mChar, CChar *targChar, std::int16_t x, std::int16_t y, std::int8_t z);
    
    // Combat Animations & Sound Effects
    void CombatAnimsNew(CChar *mChar);
    void CombatOnHorse(CChar *mChar);
    void CombatOnFoot(CChar *mChar);
};

extern CHandleCombat *Combat;

#endif
