#ifndef __CEFFECTS_H__
#define __CEFFECTS_H__

#include <cstdint>
#include <string>

#include "typedefs.h"
class CChar;
class CItem;
class CBaseObject;
class CSocket;
class CTEffect;

class cEffects {
private:
    void HandleMakeItemEffect(CTEffect *tMake);
    
public:
    void TempEffect(CChar *source, CChar *dest, std::uint8_t num, std::uint16_t more1, std::uint16_t more2, std::uint16_t more3,
                    CItem *targItemPtr = nullptr);
    void TempEffect(CChar *source, CItem *dest, std::uint8_t num, std::uint16_t more1, std::uint16_t more2, std::uint16_t more3);
    void CheckTempeffects();
    void SaveEffects();
    void LoadEffects();
    
    // Visual Effects
    void DeathAction(CChar *s, CItem *x, std::uint8_t fallDirection);
    CItem *SpawnBloodEffect(std::uint8_t worldNum, std::uint16_t instanceId, std::uint16_t bloodColour, bloodtypes_t bloodType);
    void PlayStaticAnimation(CBaseObject *target, std::uint16_t effect, std::uint8_t speed, std::uint8_t loop,
                             bool explode = false);
    void PlayStaticAnimation(std::int16_t x, std::int16_t y, std::int8_t z, std::uint16_t effect, std::uint8_t speed, std::uint8_t loop,
                             bool explode);
    void PlayMovingAnimation(CBaseObject *source, CBaseObject *dest, std::uint16_t effect, std::uint8_t speed,
                             std::uint8_t loop, bool explode, std::uint32_t dwHue = 0, std::uint32_t dwRenderMode = 0,
                             bool playLocalMoveFX = false);
    void PlayMovingAnimation(std::int16_t srcX, std::int16_t srcY, std::int8_t srcZ, std::int16_t x, std::int16_t y, std::int8_t z, std::uint16_t effect,
                             std::uint8_t speed, std::uint8_t loop, bool explode, std::uint32_t dwHue = 0,
                             std::uint32_t dwRenderMode = 0);
    void PlayMovingAnimation(CBaseObject *source, std::int16_t x, std::int16_t y, std::int8_t z, std::uint16_t effect, std::uint8_t speed,
                             std::uint8_t loop, bool explode, std::uint32_t dwHue = 0, std::uint32_t dwRenderMode = 0);
    void PlayCharacterAnimation(CChar *mChar, std::uint16_t actionId, std::uint8_t frameDelay = 0,
                                std::uint8_t frameCount = 7, bool playBackwards = false);
    void PlayNewCharacterAnimation(CChar *mChar, std::uint16_t actionId, std::uint16_t subActionId = 0,
                                   std::uint8_t subSubActionId = 0);
    void PlaySpellCastingAnimation(CChar *mChar, std::uint16_t actionId, bool monsterCast,
                                   bool areaCastAnim);
    void Bolteffect(CChar *player);
    
    // Sound Effects
    void PlaySound(CSocket *mSock, std::uint16_t soundId, bool allHear);
    void PlaySound(CBaseObject *baseObj, std::uint16_t soundId, bool allHear = true);
    void PlayBGSound(CSocket &mSock, CChar &mChar);
    void ItemSound(CSocket *s, CItem *item, bool bAllHear = false);
    void GoldSound(CSocket *s, std::uint32_t goldTotal, bool bAllHear = false);
    void PlayTileSound(CChar *mChar, CSocket *mSock);
    void PlayDeathSound(CChar *i);
    void PlayMusic(CSocket *s, std::uint16_t number);
    void DoSocketMusic(CSocket *s);
};

extern cEffects *Effects;

#endif
