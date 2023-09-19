#ifndef __Classes_h
#define __Classes_h

#include <cstdint>
#include <string>

#include "genericlist.h"
#include "typedefs.h"

class CSocket;
class CItem;
class CScriptSection;
class CChar;
class CMultiObj;
class CTownRegion;
class CSpawnItem;

enum PackTypes {
    PT_UNKNOWN = 0,
    PT_COFFIN,
    PT_PACK,
    PT_PACK2,
    PT_BAG,
    PT_BARREL,
    PT_SQBASKET,
    PT_RBASKET,
    PT_WCHEST,
    PT_SCHEST,
    PT_GCHEST,
    PT_WBOX,
    PT_MBOX,
    PT_CRATE,
    PT_SHOPPACK,
    PT_DRAWER,
    PT_BANK,
    PT_BOOKCASE,
    PT_FARMOIRE,
    PT_WARMOIRE,
    PT_DRESSER,
    PT_STOCKING,
    PT_GIFTBOX1,
    PT_GIFTBOX2,
    PT_GIFTBOX3,
    PT_GIFTBOX4,
    PT_GIFTBOX5,
    PT_GIFTBOX6,
    PT_SECHEST1,
    PT_SECHEST2,
    PT_SECHEST3,
    PT_SECHEST4,
    PT_SECHEST5,
    PT_SEBASKET,
    PT_SEARMOIRE1,
    PT_SEARMOIRE2,
    PT_SEARMOIRE3,
    PT_GAME_BACKGAMMON,
    PT_GAME_CHESS,
    PT_MAILBOX1,
    PT_MAILBOX2,
    PT_MAILBOX3,
    PT_MAILBOX4,
    PT_MAILBOX5,
    PT_MAILBOX6,
    PT_MAILBOX7,
    PT_MAILBOX8,
    PT_MAILBOX9,
};

class cItem {
public:
    CItem *DupeItem(CSocket *s, CItem *i, std::uint32_t amount);
    void GlowItem(CItem *i);
    void AddRespawnItem(CItem *s, const std::string &x, const bool inCont, const bool randomItem = false, std::uint16_t itemAmount = 1, bool useLootlist = false);
    void CheckEquipment(CChar *p);
    void StoreItemRandomValue(CItem *i, CTownRegion *tReg);
    PackTypes GetPackType(CItem *i);
    
    CItem *CreateItem(CSocket *mSock, CChar *mChar, const std::uint16_t itemId, const std::uint16_t iAmount, const std::uint16_t iColour, const CBaseObject::type_t itemType, bool inPack = false, bool shouldSave = true, std::uint8_t worldNumber = 0, std::uint16_t instanceId = 0, std::int16_t xLoc = -1, std::int16_t yLoc = -1, std::int8_t zLoc = 127);
    CItem *CreateScriptItem(CSocket *mSock, CChar *mChar, const std::string &item, const std::uint16_t iAmount, const CBaseObject::type_t itemType, bool inPack = false, const std::uint16_t iColor = 0xFFFF, bool shouldSave = true);
    CItem *CreateBaseScriptItem(CItem *mCont, std::string ourItem, const std::uint8_t worldNumber, const std::uint16_t iAmount, const std::uint16_t instanceId = 0, const CBaseObject::type_t itemType = CBaseObject::OT_ITEM, const std::uint16_t iColor = 0xFFFF,  bool shouldSave = true);
    CMultiObj *CreateMulti(const std::string &cName, const std::uint16_t itemId, const bool isBoat, const std::uint16_t worldNum, const std::uint16_t instanceId, bool isBaseMulti = false);
    CItem *CreateRandomItem(CSocket *mSock, const std::string &itemList);
    CItem *CreateBaseItem(const std::uint8_t worldNumber, const CBaseObject::type_t itemType = CBaseObject::OT_ITEM, const std::uint16_t instanceId = 0, bool shouldSave = true);
    CItem *CreateRandomItem(CItem *mCont, const std::string &sItemList, const std::uint8_t worldNumber, const std::uint16_t instanceId = 0, bool shouldSave = true, bool useLootlist = false);
    
private:
    void GetScriptItemSettings(CItem *iCreated);
    CItem *PlaceItem(CSocket *mSock, CChar *mChar, CItem *iCreated, const bool inPack, std::uint8_t worldNumber = 0, std::uint16_t instanceId = 0, std::int16_t xLoc = -1, std::int16_t yLoc = -1, std::int8_t zLoc = 127);
};

class CCharStuff {
private:
    void FindSpotForNPC(CChar *c, const std::int16_t originX, const std::int16_t originY, const std::int16_t xAway, const std::int16_t yAway, const std::int8_t z, const std::uint8_t worldNumber, const std::uint16_t instanceId);
    void LoadShopList(const std::string &list, CChar *c);
    CItem *AddRandomLoot(CItem *s, const std::string &lootlist, bool shouldSave = true);
    
public:
    CChar *CreateBaseNPC(std::string ourNPC, bool shouldSave = true);
    auto CreateRandomNPC(const std::string &npcList) -> CChar *;
    auto ChooseNpcToCreate(const std::vector<std::pair<std::string, std::uint16_t>> npcListVector) -> std::string;
    auto NpcListLookup(const std::string &npclist) -> std::string;
    
    CChar *CreateNPC(CSpawnItem *iSpawner, const std::string &npc);
    CChar *CreateNPCxyz(const std::string &npc, std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::uint16_t instanceId, bool useNpcList = false);
    void PostSpawnUpdate(CChar *cCreated);
    bool ApplyNpcSection(CChar *applyTo, CScriptSection *NpcCreation, std::string sectionId, bool isGate = false);
    
    bool CanControlPet(CChar *mChar, CChar *Npc, bool isRestricted, bool checkDifficulty, bool ignoreOwnerCheck = false, bool ignoreLoyaltyChanges = false);
    void FinalizeTransfer(CChar *pet, CChar *srcChar, CChar *targChar);
    void ReleasePet(CChar *pet);
    CChar *GetGuardingFollower(CChar *mChar, CBaseObject *guarded);
    bool CheckPetFriend(CChar *mChar, CChar *pet);
    void StopPetGuarding(CChar *pet);
    GenericList<CChar *> *GetAlwaysAwakeNPCs();
};

extern cItem *Items;
extern CCharStuff *Npcs;

#endif
