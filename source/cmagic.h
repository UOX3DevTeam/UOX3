#ifndef __CMAGIC_H__
#define __CMAGIC_H__

#include <cstdint>
#include <string>
#include <vector>

#include "typedefs.h"

class CItem;
class CSocket;
class CChar;
class CSpellInfo;

struct Reag_st;

class CMagic {
public:
    // adding constructor/destructor
    CMagic();
    ~CMagic(); // no need for a virtual destructor as long as no subclasses us
    bool HasSpell(CItem *book, std::int32_t spellNum);
    void AddSpell(CItem *book, std::int32_t spellNum);
    void RemoveSpell(CItem *book, std::int32_t spellNum);
    
    void LoadScript(); // adding function for spell system "cache"
    void SpellBook(CSocket *mSock);
    void GateCollision(CSocket *mSock, CChar *mChar, CItem *itemCheck, itemtypes_t type);
    bool SelectSpell(CSocket *mSock, std::int32_t num);
    void DoMoveEffect(std::int32_t num, CBaseObject *target, CChar *source);
    void DoStaticEffect(CChar *source, std::int32_t num);
    void PlaySound(CChar *source, std::int32_t num);
    void DelReagents(CChar *s, Reag_st reags);
    void CastSpell(CSocket *s, CChar *caster); // changed for NPC casting
    bool CheckResist(CChar *attacker, CChar *defender, std::int32_t circle);
    bool CheckResist(std::int16_t resistDifficulty, CChar *defender, std::int32_t circle);
    void PoisonDamage(CChar *p, std::int32_t posion);
    void CheckFieldEffects(CChar &mChar);
    bool HandleFieldEffects(CChar *mChar, CItem *fieldItem, std::uint16_t id);
    bool CheckBook(std::int32_t circle, std::int32_t spell, CItem *i);
    bool CheckReagents(CChar *s, const Reag_st *reagents);
    bool CheckMana(CChar *s, std::int32_t num);
    bool CheckStamina(CChar *s, std::int32_t num);
    bool CheckHealth(CChar *s, std::int32_t num);
    bool CheckMagicReflect(CChar *i);
    
    void MagicDamage(CChar *p, std::int16_t amount, CChar *attacker = nullptr, weathertype_t element = NONE);
    void SpellFail(CSocket *s);
    void SubtractMana(CChar *s, std::int32_t mana);
    void SubtractStamina(CChar *s, std::int32_t stamina);
    void SubtractHealth(CChar *s, std::int32_t health, std::int32_t spellNum);
    void MagicTrap(CChar *s, CItem *i);
    void Polymorph(CSocket *s, std::uint16_t polyId);
    void BoxSpell(CSocket *s, CChar *caster, std::int16_t &x1, std::int16_t &x2, std::int16_t &y1, std::int16_t &y2, std::int8_t &z1,
                  std::int8_t &z2);
    void SummonMonster(CSocket *s, CChar *caster, std::uint16_t id, std::int16_t x, std::int16_t y, std::int8_t z);
    void PolymorphMenu(CSocket *s, std::uint16_t gmindex);
    
    std::vector<CSpellInfo> spells; // adding variable for spell system "cache"
    
    void registerSpell(cScript *toRegister, std::int32_t spellNumber, bool isEnabled);
    void SetSpellStatus(std::int32_t spellNumber, bool isEnabled);
    
private:
    std::uint8_t GetFieldDir(CChar *s, std::int16_t x, std::int16_t y);
    bool RegMsg(CChar *s, Reag_st failmsg);
    void LogSpell(std::string spell, CChar *player1, CChar *player2, const std::string &extraInfo);
    std::uint8_t spellCount;
};

extern CMagic *Magic;

#endif
