#ifndef __CSCRIPT_H__
#define __CSCRIPT_H__

#include <bitset>
#include <cstdint>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include "spidermonkey.h"

#include "typedefs.h"
#include "type/weather.hpp"

class CPIGumpMenuSelect;
class CPIGumpInput;
class CSpawnRegion;
class CChar;
class CBaseObject;
class CItem;
class CMultiObj;

struct JSScript;
struct JSContext;
struct JSObject;


struct SEGump_st {
    std::vector<std::string> *one, *two;
    std::uint32_t textId;
};

struct SEGumpData_st {
    std::vector<std::string> sEdits;
    std::vector<std::int32_t> nButtons;
    std::vector<std::int16_t> nIDs;
};

class cScript {
public:
    enum event_t {
        seOnCreateDFN = 0,
        seOnCreateTile,
        seOnCreatePlayer, //    *    Done for PCs on global script
        seOnCommand,
        seOnDelete,     //    **
        seOnSpeech,     //    *    Missing item response at the moment
        seInRange,      //    *    Missing character in range
        seOnCollide,    //    **    Items only
        seOnMoveDetect, //    ***
        seOnSteal,      //    **
        seOnDispel,     //    **
        seOnSkill,
        seOnStat,
        seOnTooltip,
        seOnNameRequest,
        seOnAttack,
        seOnDefense,
        seOnSkillGain,   //    **
        seOnSkillLoss,   //    **
        seOnSkillChange, //    **
        seOnStatGained,  //    **
        seOnStatGain,    //    **
        seOnStatLoss,    //    **
        seOnStatChange,  //    **
        seOnDrop,
        seOnPickup,
        seOnContRemoveItem,
        seOnSwing,
        seOnDecay,
        seOnTransfer,
        seOnEntrance,       //    **
        seOnLeaving,        //    **
        seOnMultiLogout,    //    **
        seOnEquipAttempt,   //    **
        seOnEquip,          //    **
        seOnUnequipAttempt, //    **
        seOnUnequip,        //    **
        seOnUseChecked,     //    **  the event that replaces hardcoded use-stuff
        seOnUseUnChecked,
        seOutOfRange, //    *    Missing character out of range
        seOnLogin,    //    **
        seOnLogout,
        seOnClick,
        seOnFall,
        seOnSell,
        seOnSellToVendor,
        seOnBuy,
        seOnBuyFromVendor,
        seOnSoldToVendor,
        seOnBoughtFromVendor,
        seOnAISliver, //    **
        seOnSystemSlice,
        seOnUnknownTrigger,
        seOnLightChange,
        seOnWeatherChange,
        seOnTempChange,
        seOnTimer,         //    **
        seOnDeath,         //    **
        seOnResurrect,     //    **
        seOnFlagChange,    //    **
        seOnLoyaltyChange, //    **
        seOnHungerChange,  //    **
        seOnThirstChange,  //  **
        seOnStolenFrom,    //    **
        seOnSnooped,       //    **
        seOnSnoopAttempt,  //    **
        seOnEnterRegion,   //  **
        seOnLeaveRegion,   //    **
        seOnSpellTarget,
        seOnSpellTargetSelect,
        seOnSpellCast,
        seOnSpellSuccess,
        seOnTalk,
        seOnScrollCast,
        seOnSpeechInput,
        seOnSpellGain,
        seOnSpellLoss,
        seOnSkillCheck,
        seOnDropItemOnNpc,
        seOnStart,
        seOnStop,
        seOnIterate,
        seOnIterateSpawnRegions,
        seOnPacketReceive,
        seOnCharDoubleClick, //    **  the event that replaces hardcoded character doubleclick-stuff
        seOnSkillGump,       //    **    allows overriding client's request to open default skill gump
        seOnCombatStart,     //    **    allows overriding what happens when combat is initiated
        seOnAICombatTarget,  //    **    allows overriding target selection taking place for regular
        // AI behaviours
        seOnCombatEnd,       //    **    allows overriding what happens when combat ends
        seOnDeathBlow,
        seOnCombatDamageCalc,
        seOnDamage,
        seOnDamageDeal,
        seOnGumpPress,
        seOnGumpInput,
        seOnScrollingGumpPress,
        seOnDropItemOnItem,
        seOnVirtueGumpPress,
        seOnUseBandageMacro, //    **    allows overriding what happens when client uses bandage
        // macros
        seOnHouseCommand,    //    **    allows overriding what happens when player speaks house
        // commands
        seOnMakeItem,
        seOnPathfindEnd,
        seOnEnterEvadeState,
        seOnCarveCorpse,
        seOnDyeTarget,
        seOnQuestGump,
        seOnHelpButton,
        seOnWarModeToggle,
        seOnSpecialMove,
        seOnFacetChange
    };
    
private:
    JSScript *targScript;
    JSContext *targContext;
    JSObject *targObject;
    
    bool isFiring;
    std::uint8_t runTime;
    
    std::bitset<64> eventPresence[3];
    std::bitset<64> needsChecking[3];
    bool EventExists(cScript::event_t eventNum) const;
    bool NeedsChecking(cScript::event_t eventNum) const;
    void SetEventExists(cScript::event_t eventNum, bool status);
    void SetNeedsChecking(cScript::event_t eventNum, bool status);
    bool ExistAndVerify(cScript::event_t eventNum, std::string functionName);
    
    std::vector<SEGump_st *> gumpDisplays;
    
    void Cleanup();
    
public:
    void CollectGarbage();
    
    size_t NewGumpList();
    SEGump_st *GetGumpList(std::int32_t index);
    void RemoveGumpList(std::int32_t index);
    void SendGumpList(std::int32_t index, CSocket *toSendTo);
    
    void HandleGumpPress(CPIGumpMenuSelect *packet);
    void HandleGumpInput(CPIGumpInput *pressing);
    
    cScript(const std::filesystem::path &targFile, std::uint8_t runTime);
    ~cScript();
    
    JSObject *Object() const; // returns object pointer
    
    //|	Modification	-	08162003 - Added these event to handle any script initialization and
    //clean up as the server starts, and is shut down
    bool OnStart();
    bool OnStop();
    //
    bool OnPacketReceive(CSocket *mSock, std::uint16_t packetNum);
    bool OnIterate(CBaseObject *a, std::uint32_t &b);
    bool OnIterateSpawnRegions(CSpawnRegion *a, std::uint32_t &b);
    bool OnCreate(CBaseObject *thingCreated, bool dfnCreated, bool isPlayer);
    bool DoesEventExist(char *eventToFind);
    std::int8_t OnCommand(CSocket *mSock, std::string command);
    bool OnDelete(CBaseObject *thingDestroyed);
    std::int8_t OnSpeech(const char *speech, CChar *personTalking, CBaseObject *talkingTo);
    bool InRange(CBaseObject *srcObj, CBaseObject *objInRange);
    std::int8_t OnCollide(CSocket *targSock, CChar *objColliding, CBaseObject *objCollideWith);
    std::int8_t OnMoveDetect(CBaseObject *sourceObj, CChar *CharInRange, std::uint8_t rangeToChar, std::uint16_t oldCharX,
                             std::uint16_t oldCharY);
    std::int8_t OnSteal(CChar *thief, CItem *theft, CChar *victim);
    std::int8_t OnPathfindEnd(CChar *npc, std::int8_t pathfindResult);
    std::int8_t OnEnterEvadeState(CChar *npc, CChar *enemy);
    std::int8_t OnCarveCorpse(CChar *player, CItem *corpse);
    std::int8_t OnDyeTarget(CChar *player, CItem *dyeTub, CItem *target);
    std::int8_t OnDispel(CBaseObject *dispelled);
    bool OnSkill(CBaseObject *skillUse, std::int8_t skillUsed);
    bool OnStat();
    std::string OnTooltip(CBaseObject *myObj, CSocket *pSocket);
    std::string OnNameRequest(CBaseObject *myObj, CChar *nameRequester, std::uint8_t requestSource);
    bool OnAttack(CChar *attacker, CChar *defender);
    bool OnDefense(CChar *attacker, CChar *defender);
    std::int8_t OnSkillGain(CChar *player, std::int8_t skill, std::uint32_t skillAmtGained);
    std::int8_t OnSkillLoss(CChar *player, std::int8_t skill, std::uint32_t skillAmtLost);
    bool OnSkillChange(CChar *player, std::int8_t skill, std::int32_t skillAmtChanged);
    std::int8_t OnStatGained(CChar *player, std::uint32_t stat, std::int8_t skill, std::uint32_t statGainedAmount);
    bool OnStatGain(CChar *player, std::uint32_t stat, std::int8_t skill, std::uint32_t statGainAmount);
    std::int8_t OnStatLoss(CChar *player, std::uint32_t stat, std::uint32_t statLossAmount);
    bool OnStatChange(CChar *player, std::uint32_t stat, std::int32_t statChangeAmount);
    std::int8_t OnDrop(CItem *item, CChar *dropper);
    std::int8_t OnPickup(CItem *item, CChar *pickerUpper, CBaseObject *objCont);
    bool OnContRemoveItem(CItem *contItem, CItem *item, CChar *pickerUpper);
    std::int8_t OnSwing(CItem *swinging, CChar *swinger, CChar *swingTarg);
    std::int8_t OnDecay(CItem *decaying);
    std::int8_t OnEntrance(CMultiObj *left, CBaseObject *leaving);
    std::int8_t OnLeaving(CMultiObj *left, CBaseObject *leaving);
    std::int8_t OnMultiLogout(CMultiObj *iMulti, CChar *cPlayer);
    std::int8_t OnEquipAttempt(CChar *equipper, CItem *equipping);
    std::int8_t OnEquip(CChar *equipper, CItem *equipping);
    std::int8_t OnUnequipAttempt(CChar *equipper, CItem *equipping);
    std::int8_t OnUnequip(CChar *equipper, CItem *equipping);
    std::int8_t OnUseChecked(CChar *user, CItem *iUsing);
    std::int8_t OnUseUnChecked(CChar *user, CItem *iUsing);
    bool OutOfRange(CBaseObject *srcObj, CBaseObject *objVanish);
    bool OnLogin(CSocket *sockPlayer, CChar *pPlayer);
    bool OnLogout(CSocket *sockPlayer, CChar *pPlayer);
    std::int8_t OnClick(CSocket *sockPlayer, CBaseObject *objClicked);
    bool OnFall(CChar *pFall, std::int8_t fallDistance);
    std::int8_t OnAISliver(CChar *pSliver);
    bool OnSystemSlice();
    std::int8_t OnLightChange(CBaseObject *tObject, std::uint8_t lightLevel);
    bool OnWeatherChange(CBaseObject *tObject, Weather::type_t element);
    bool OnTempChange(CBaseObject *tObject, std::int8_t temp);
    bool OnTimer(CBaseObject *tObject, std::uint16_t timerId);
    std::int8_t OnDeath(CChar *pDead, CItem *iCorpse);
    std::int8_t OnResurrect(CChar *pAlive);
    std::int8_t OnFlagChange(CChar *pChanging, std::uint8_t newStatus, std::uint8_t oldStatus);
    std::int8_t OnLoyaltyChange(CChar *pChanging, std::int8_t newStatus);
    std::int8_t OnHungerChange(CChar *pChanging, std::int8_t newStatus);
    bool OnThirstChange(CChar *pChanging, std::int8_t newStatus);
    std::int8_t OnStolenFrom(CChar *stealing, CChar *stolenFrom, CItem *stolen);
    std::int8_t OnSnooped(CChar *snooped, CChar *snooper, bool success);
    std::int8_t OnSnoopAttempt(CChar *snooped, CItem *pack, CChar *snooper);
    bool OnEnterRegion(CChar *entering, std::uint16_t region);
    bool OnLeaveRegion(CChar *entering, std::uint16_t region);
    std::int8_t OnSpellTarget(CBaseObject *target, CChar *caster, std::uint8_t spellNum);
    std::int8_t OnSpellTargetSelect(CChar *caster, CBaseObject *target, std::uint8_t spellNum);
    bool DoCallback(CSocket *tSock, serial_t targeted, std::uint8_t callNum);
    std::int16_t OnSpellCast(CChar *tChar, std::uint8_t SpellId);
    std::int16_t OnScrollCast(CChar *tChar, std::uint8_t SpellId);
    std::int8_t OnSpellSuccess(CChar *tChar, std::uint8_t SpellId);
    std::int8_t OnTalk(CChar *myChar, const char *mySpeech);
    bool OnSpeechInput(CChar *myChar, CItem *myItem, const char *mySpeech);
    std::int8_t OnSpellGain(CItem *book, const std::uint8_t spellNum);
    std::int8_t OnSpellLoss(CItem *book, const std::uint8_t spellNum);
    std::int8_t OnSkillCheck(CChar *myChar, const std::uint8_t skill, const std::uint16_t lowSkill, const std::uint16_t highSkill,
                             bool isCraftSkill);
    std::int8_t OnDropItemOnNpc(CChar *srcChar, CChar *targChar, CItem *i);
    std::int8_t OnDropItemOnItem(CItem *item, CChar *dropper, CItem *dest);
    std::int8_t OnVirtueGumpPress(CChar *mChar, CChar *tChar, std::uint16_t buttonId);
    std::int8_t OnScrollingGumpPress(CSocket *tSock, std::uint16_t gumpId, std::uint16_t buttonId);
    std::int8_t OnQuestGump(CChar *mChar);
    std::int8_t OnHelpButton(CChar *mChar);
    std::int8_t OnWarModeToggle(CChar *mChar);
    std::int8_t OnSpecialMove(CChar *mChar, std::uint8_t abilityId);
    std::int8_t OnFacetChange(CChar *mChar, const std::uint8_t oldFacet, const std::uint8_t newFacet);
    
    bool AreaObjFunc(char *funcName, CBaseObject *srcObject, CBaseObject *tmpObject, CSocket *s);
    bool CallParticularEvent(const char *eventToCall, jsval *params, std::int32_t numParams,
                             jsval *eventRetVal);
    
    bool ScriptRegistration(std::string scriptType);
    
    bool executeCommand(CSocket *s, std::string funcName, std::string executedString);
    
    bool MagicSpellCast(CSocket *mSock, CChar *tChar, bool directCast, std::int32_t spellNum);
    std::int8_t OnCharDoubleClick(CChar *currChar, CChar *targChar);
    std::int8_t OnSkillGump(CChar *mChar);
    std::int8_t OnUseBandageMacro(CSocket *mSock, CChar *targChar, CItem *bandageItem);
    std::int8_t OnAICombatTarget(CChar *attacker, CChar *target);
    std::int8_t OnCombatStart(CChar *attacker, CChar *defender);
    std::int8_t OnCombatEnd(CChar *attacker, CChar *defender);
    
    std::int8_t OnDeathBlow(CChar *mKilled, CChar *mKiller);
    
    std::int16_t OnCombatDamageCalc(CChar *attacker, CChar *defender, std::uint8_t getFightSkill, std::uint8_t hitLoc);
    std::int8_t OnDamage(CChar *damaged, CChar *attacker, std::int16_t damageValue, Weather::type_t damageType);
    std::int8_t OnDamageDeal(CChar *attacker, CChar *damaged, std::int16_t damageValue, Weather::type_t damageType);
    std::int8_t OnBuy(CSocket *targSock, CChar *objVendor);
    std::int8_t OnBuyFromVendor(CSocket *targSock, CChar *objVendor, CBaseObject *objItemBought,
                                std::uint16_t numItemsBuying);
    std::int8_t OnSellToVendor(CSocket *targSock, CChar *objVendor, CBaseObject *objItemSold,
                               std::uint16_t numItemsSelling);
    std::int8_t OnSell(CSocket *targSock, CChar *objVendor);
    std::int8_t OnBoughtFromVendor(CSocket *targSock, CChar *objVendor, CBaseObject *objItemBought,
                                   std::uint16_t numItemsBought);
    std::int8_t OnSoldToVendor(CSocket *targSock, CChar *objVendor, CBaseObject *objItemSold,
                               std::uint16_t numItemsSold);
    std::int8_t OnHouseCommand(CSocket *targSock, CMultiObj *multiObj, std::uint8_t targId);
    std::int8_t OnMakeItem(CSocket *mSock, CChar *objChar, CItem *objItem, std::uint16_t createEntryId);
    
    //	Critical handler type stuff
    bool IsFiring();
    void Firing();
    void Stop();
};

#endif
