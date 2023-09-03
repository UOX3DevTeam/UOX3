#ifndef __CITEM_H__
#define __CITEM_H__
#include <array>
#include <bitset>
#include <cstdint>
#include <string>
#include <vector>

#include "cbaseobject.h"
#include "genericlist.h"

class CTownRegion;

enum CITempVars {
    CITV_MORE = 0,
    CITV_MOREX,
    CITV_MOREY,
    CITV_MOREZ,
    CITV_MORE0,
    CITV_MORE1,
    CITV_MORE2,
    CITV_COUNT
};

class CItem : public CBaseObject {
  protected:
    GenericList<CItem *> Contains;
    GenericList<CSocket *> contOpenedBy;

    CBaseObject *contObj;
    std::uint8_t glowEffect;
    SERIAL glow; // identifies glowing objects
    COLOUR glowColour;
    std::int8_t madeWith;     // Store the skills used to make this item
    std::int32_t rndValueRate; // Store the value calculated base on RANDOMVALUE in region.dfn
    std::int16_t good;         // Store type of GOODs to trade system! (Plz not set as UNSIGNED)

    // for example: RANK 5 --> 5*10 = 50% of malus
    // this item has same values decreased by 50%..
    // RANK 1 --> 1*10=10% this item has 90% of malus!
    // RANK 10 --> 10*10 = 100% this item has no malus! RANK 10 is automatically setted if you
    // select RANKSYSTEM 0. Vars: LODAMAGE, HIDAMAGE, ATT, DEF, HP, MAXHP
    std::int8_t rank; // for rank system, this value is the LEVEL of the item from 1 to 10.  Simply
               // multiply t he rank*10 and calculate the MALUS this item has from the original.
    ARMORCLASS armorClass;
    std::uint16_t restock; // Number up to which shopkeeper should restock this item
    std::int8_t movable; // 0=Default as stored in client, 1=Always movable, 2=Never movable, 3=Owner
                  // movable.
    TIMERVAL tempTimer;
    TIMERVAL decayTime;
    std::uint8_t spd;         // The speed of the weapon
    std::uint16_t maxHp;       // Max number of hit points an item can have.
    std::uint16_t amount;      // Amount of items in pile
    ItemLayers layer; // Layer if equipped on paperdoll
    ItemTypes type;   // For things that do special things on doubleclicking
    std::int8_t offspell;
    std::uint16_t entryMadeFrom;
    SERIAL creator; // Store the serial of the player made this item
    std::int8_t gridLoc;
    std::int32_t weightMax;  // Maximum weight a container can hold
    std::int32_t baseWeight; // Base weight of item. Applied when item is created for the first time, based
                     // on weight. Primarily used to determine base weight of containers
    std::uint16_t maxItems;   // Maximum amount of items a container can hold
    std::uint8_t maxRange;   // Max range of ranged weapon
    std::uint8_t baseRange;  // Base range of thrown weapon
    std::uint16_t maxUses;    // Max number of uses an item can have
    std::uint16_t usesLeft;   // Current number of uses left on an item
    std::uint16_t regionNum;
    TIMERVAL tempLastTraded; // Temporary timestamp for when item was last traded between players
                             // via secure trade window (not saved)
    std::uint8_t stealable; // 0=Not stealable, 1=Stealable (default, most items), 2=Special Stealable (town
                    // rares, etc)

    std::bitset<8> bools;
    std::bitset<8> priv; // Bit 0, decay off/on.  Bit 1, newbie item off/on.  Bit 2 Dispellable

    std::string name2;
    std::string desc;
    std::string eventName; // Name of custom event item belongs to

    std::uint32_t tempVars[CITV_COUNT];
    std::uint8_t dir; // direction an item can have

    std::uint32_t value[3];  // Price a shopkeep buys and sells items for, with price on player vendor as
                    // optional third value
    std::uint16_t ammo[2];   // Ammo ID and Hue
    std::uint16_t ammoFX[3]; // Ammo-effect ID, Hue and rendermode

    std::bitset<WEATHNUM> weatherBools; // For elemental weaponry.  So a Heat weapon would be a fire
                                        // weapon, and does elemental damage to Heat weak races

    auto RemoveSelfFromCont() -> void;
    virtual void RemoveSelfFromOwner(void) override;
    virtual void AddSelfToOwner(void) override;

    auto CheckItemIntegrity() -> void;
    virtual bool DumpHeader(std::ostream &outStream) const override;
    virtual bool LoadRemnants(void) override;

    std::uint32_t spells[3]; // For spellbooks (eventually should be a derived class)

    auto CopyData(CItem *target) -> void;

  public:
    auto GetContainsList() -> GenericList<CItem *> *;
    auto GetContOpenedByList() -> GenericList<CSocket *> *;

    virtual void SetWeight(std::int32_t newVal, bool doWeightUpdate = true) override;
    auto EntryMadeFrom() const -> std::uint16_t;
    auto EntryMadeFrom(std::uint16_t newValue) -> void;

    auto GetWeatherDamage(WeatherType effectNum) const -> bool;
    auto SetWeatherDamage(WeatherType effectNum, bool value) -> void;

    auto Dupe(ObjectType itemType = OT_ITEM) -> CItem *;

    auto GetCont() const -> CBaseObject *;
    auto GetContSerial() const -> SERIAL;

    auto SetCont(CBaseObject *newCont, bool removeFromView = false) -> bool;
    auto SetContSerial(SERIAL newSerial) -> bool;

    auto GetGridLocation() const -> std::int8_t;
    auto SetGridLocation(std::int8_t newLoc) -> void;

    auto GetStealable() const -> std::uint8_t;
    auto SetStealable(std::uint8_t newValue) -> void;

    auto IsDoorOpen() const -> bool;
    auto IsPileable() const -> bool;
    auto IsDyeable() const -> bool;
    auto IsCorpse() const -> bool;
    auto IsHeldOnCursor() const -> bool;
    auto IsGuarded() const -> bool;
    auto IsSpawnerList() const -> bool;
    auto IsMarkedByMaker() const -> bool;

    auto IsNewbie() const -> bool;
    auto IsDecayable() const -> bool;
    auto IsDispellable() const -> bool;
    auto IsDivineLocked() const -> bool;

    auto SetDoorOpen(bool newValue) -> void;
    auto SetPileable(bool newValue) -> void;
    auto SetDye(bool newValue) -> void;
    auto SetCorpse(bool newValue) -> void;
    auto SetHeldOnCursor(bool newValue) -> void;
    virtual void SetGuarded(bool newValue);
    auto SetSpawnerList(bool newValue) -> void;
    auto SetMakersMark(bool newValue) -> void;

    auto SetNewbie(bool newValue) -> void;
    auto SetDecayable(bool newValue) -> void;
    auto SetDispellable(bool newValue) -> void;
    auto SetDivineLock(bool newValue) -> void;

    auto GetName2() const -> const std::string &;
    auto GetCreator() const -> SERIAL;
    auto GetDesc() const -> std::string;
    auto GetEvent() const -> std::string;

    auto SetName2(const std::string &value) -> void;
    auto SetCreator(SERIAL newValue) -> void;
    auto SetDesc(std::string newValue) -> void;
    auto SetEvent(std::string newValue) -> void;

    auto PlaceInPack() -> void;
    virtual void SetOldLocation(std::int16_t newX, std::int16_t newY, std::int8_t newZ) override;
    virtual void SetLocation(const CBaseObject *toSet) override;
    virtual void SetLocation(std::int16_t newX, std::int16_t newY, std::int8_t newZ) override;
    virtual void SetLocation(std::int16_t newX, std::int16_t newY, std::int8_t newZ, std::uint8_t world, std::uint16_t instanceId) override;
    virtual void SetLocation(std::int16_t newX, std::int16_t newY, std::int8_t newZ, std::int8_t newLoc, std::uint8_t world,
                             std::uint16_t instanceId);
    auto IncZ(std::int16_t newValue) -> void;
    auto IncLocation(std::int16_t xInc, std::int16_t yInc) -> void;

    auto SetRegion(std::uint16_t newValue) -> void;
    auto GetRegion() const -> CTownRegion *;
    auto GetRegionNum() const -> std::uint16_t;

    auto InDungeon() -> bool;

    auto GetLayer() const -> ItemLayers;
    auto SetLayer(ItemLayers newValue) -> void;

    auto GetType() const -> ItemTypes;
    auto SetType(ItemTypes newValue) -> void;

    auto GetOffSpell() const -> std::int8_t;
    auto SetOffSpell(std::int8_t newValue) -> void;

    auto GetTempVar(CITempVars whichVar) const -> std::uint32_t;
    auto SetTempVar(CITempVars whichVar, std::uint32_t newVal) -> void;
    auto GetTempVar(CITempVars whichVar, std::uint8_t part) const -> std::uint8_t;
    auto SetTempVar(CITempVars whichVar, std::uint8_t part, std::uint8_t newVal) -> void;

    auto GetAmount() const -> std::uint16_t;
    auto SetAmount(std::uint32_t newValue) -> void;
    auto IncAmount(std::int32_t incValue, bool noDelete = false) -> bool;

    virtual std::uint16_t GetMaxHP(void) const;
    virtual void SetMaxHP(std::uint16_t newValue);

    auto GetSpeed() const -> std::uint8_t;
    auto SetSpeed(std::uint8_t newValue) -> void;

    auto GetMaxRange() const -> std::uint8_t;
    auto SetMaxRange(std::uint8_t newValue) -> void;

    auto GetBaseRange() const -> std::uint8_t;
    auto SetBaseRange(std::uint8_t newValue) -> void;

    auto GetMovable() const -> std::int8_t;
    auto SetMovable(std::int8_t newValue) -> void;

    auto GetTempLastTraded() const -> TIMERVAL;
    auto GetTempTimer() const -> TIMERVAL;
    auto GetDecayTime() const -> TIMERVAL;

    auto SetTempLastTraded(TIMERVAL newValue) -> void;
    auto SetTempTimer(TIMERVAL newValue) -> void;
    auto SetDecayTime(TIMERVAL newValue) -> void;

    virtual std::uint8_t GetPriv(void) const;
    virtual void SetPriv(std::uint8_t newValue);

    auto GetBuyValue() const -> std::uint32_t;
    auto SetBuyValue(std::uint32_t newValue) -> void;
    auto GetSellValue() const -> std::uint32_t;
    auto SetSellValue(std::uint32_t newValue) -> void;
    auto GetVendorPrice() const -> std::uint32_t;
    auto SetVendorPrice(std::uint32_t newValue) -> void;

    auto GetRestock() const -> std::uint16_t;
    auto SetRestock(std::uint16_t newValue) -> void;

    auto GetMaxUses() const -> std::uint16_t;
    auto SetMaxUses(std::uint16_t newValue) -> void;

    auto GetUsesLeft() const -> std::uint16_t;
    auto SetUsesLeft(std::uint16_t newValue) -> void;

    auto GetArmourClass() const -> ARMORCLASS;
    auto SetArmourClass(ARMORCLASS newValue) -> void;

    auto GetRank() const -> std::int8_t;
    auto SetRank(std::int8_t newValue) -> void;

    auto GetGood() const -> std::int16_t;
    auto SetGood(std::int16_t newValue) -> void;

    auto GetRndValueRate() const -> std::int32_t;
    auto SetRndValueRate(std::int32_t newValue) -> void;

    auto GetMadeWith() const -> std::int8_t;
    auto SetMadeWith(std::int8_t newValue) -> void;

    auto GetAmmoId() const -> std::uint16_t;
    auto SetAmmoId(std::uint16_t newValue) -> void;

    auto GetAmmoHue() const -> std::uint16_t;
    auto SetAmmoHue(std::uint16_t newValue) -> void;

    auto GetAmmoFX() const -> std::uint16_t;
    auto SetAmmoFX(std::uint16_t newValue) -> void;

    auto GetAmmoFXHue() const -> std::uint16_t;
    auto SetAmmoFXHue(std::uint16_t newValue) -> void;

    auto GetAmmoFXRender() const -> std::uint16_t;
    auto SetAmmoFXRender(std::uint16_t newValue) -> void;

    auto GetWeightMax() const -> std::int32_t;
    auto SetWeightMax(std::int32_t newValue) -> void;

    auto GetBaseWeight() const -> std::int32_t;
    auto SetBaseWeight(std::int32_t newValue) -> void;

    auto GetMaxItems() const -> std::uint16_t;
    auto SetMaxItems(std::uint16_t newValue) -> void;

    // Note: Value range to -ALLSKILLS-1 to ALLSKILLS+1
    // To calculate skill used to made this item:
    // if is a positive value, substract 1 it.
    //    Ex) madeWith = 34, 34 - 1 = 33, 33 = STEALING
    // if is a negative value, add 1 from it and invert value.
    //    Ex) madeWith = -34, -34 + 1 = -33, Abs(-33) = 33 = STEALING.
    // 0 = nullptr
    // So... a positive value is used when the item is made by a
    // player with 95.0+ at that skill. Infact in this way when
    // you click on the item appear its name and the name of the
    // creator. A negative value if the play is not skilled
    // enough!

    auto GetGlow() const -> SERIAL;
    auto SetGlow(SERIAL newValue) -> void;

    auto GetGlowColour() const -> COLOUR;
    auto SetGlowColour(COLOUR newValue) -> void;

    auto GetGlowEffect() const -> std::uint8_t;
    auto SetGlowEffect(std::uint8_t newValue) -> void;

    CItem();
    virtual ~CItem();

    auto IsFieldSpell() const -> std::uint8_t;
    auto IsLockedDown() const -> bool;
    auto IsShieldType() const -> bool;
    auto IsMetalType() const -> bool;
    auto IsLeatherType() const -> bool;
    auto CanBeLockedDown() const -> bool;
    auto LockDown(CMultiObj *multiObj = nullptr) -> void;
    auto IsContType() const -> bool;
    auto UpdateRegion() -> void;

    auto TextMessage(CSocket *s, std::int32_t dictEntry, R32 secsFromNow = 0.0f, std::uint16_t Colour = 0x005A)
        -> void;
    virtual void Update(CSocket *mSock = nullptr, bool drawGamePlayer = false,
                        bool sendToSelf = true) override;
    virtual void SendToSocket(CSocket *mSock, bool drawGamePlayer = false) override;
    auto SendPackItemToSocket(CSocket *mSock) -> void;
    virtual void RemoveFromSight(CSocket *mSock = nullptr);

    virtual bool Save(std::ostream &outStream) override;
    virtual bool DumpBody(std::ostream &outStream) const override;
    virtual bool HandleLine(std::string &UTag, std::string &data) override;
    virtual void PostLoadProcessing(void) override;
    virtual void Cleanup(void) override;
    virtual void Delete(void) override;
    virtual bool CanBeObjType(ObjectType toCompare) const override;

    auto GetSpell(std::uint8_t part) const -> std::uint32_t;
    auto SetSpell(std::uint8_t part, std::uint32_t newValue) -> void;
};

class CSpawnItem : public CItem {
  protected:
    std::uint8_t spawnInterval[2];
    std::string spawnSection;
    bool isSectionAList;

    auto CopyData(CSpawnItem *target) -> void;

  public:
    GenericList<CBaseObject *> spawnedList;

    CSpawnItem();
    virtual ~CSpawnItem() {}

    auto GetInterval(std::uint8_t part) const -> std::uint8_t;
    auto SetInterval(std::uint8_t part, std::uint8_t newVal) -> void;
    auto GetSpawnSection() const -> std::string;
    auto SetSpawnSection(const std::string &newVal) -> void;
    auto IsSectionAList() const -> bool;
    auto IsSectionAList(bool newVal) -> void;

    virtual bool DumpHeader(std::ostream &outStream) const override;
    virtual bool DumpBody(std::ostream &outStream) const override;

    virtual bool HandleLine(std::string &UTag, std::string &data) override;

    auto DoRespawn() -> bool; // Will replace RespawnItem() eventually
    auto HandleItemSpawner() -> bool;
    auto HandleNPCSpawner() -> bool;
    auto HandleSpawnContainer() -> bool;

    virtual void Cleanup(void) override;

    virtual bool CanBeObjType(ObjectType toCompare) const override;

    auto Dupe() -> CSpawnItem *;
};

#endif
