#ifndef __CCHAR_H__
#define __CCHAR_H__

#include <bitset>
#include <cstdint>
#include <deque>
#include <string>
#include <unordered_map>

#include "cbaseobject.h"
#include "genericlist.h"
#include "subsystem/account.hpp"
#include "typedefs.h"

class cScript;
class CItem;
class CTownRegion;

enum cnpc_flag_t { fNPC_NEUTRAL = 0, fNPC_INNOCENT, fNPC_EVIL };

enum cc_tid_t {
    // Global Character Timers
    tCHAR_TIMEOUT = 0,
    tCHAR_INVIS,
    tCHAR_HUNGER,
    tCHAR_THIRST,
    tCHAR_POISONTIME,
    tCHAR_POISONTEXT,
    tCHAR_POISONWEAROFF,
    tCHAR_SPELLTIME,
    tCHAR_SPELLRECOVERYTIME,
    tCHAR_ANTISPAM,
    tCHAR_CRIMFLAG,
    tCHAR_STEALFLAG,
    tCHAR_MURDERRATE,
    tCHAR_PEACETIMER,
    tCHAR_FLYINGTOGGLE,
    tCHAR_FIREFIELDTICK,
    tCHAR_POISONFIELDTICK,
    tCHAR_PARAFIELDTICK,
    tCHAR_YOUNGHEAL,
    tCHAR_YOUNGMESSAGE,
    // NPC Timers
    tNPC_MOVETIME,
    tNPC_SPATIMER,
    tNPC_SUMMONTIME,
    tNPC_EVADETIME,
    tNPC_LOYALTYTIME,
    tNPC_IDLEANIMTIME,
    tNPC_PATHFINDDELAY,
    tNPC_FLEECOOLDOWN,
    // PC Timers
    tPC_LOGOUT,
    tCHAR_COUNT
};

struct TargetInfo {
    std::uint32_t timestamp;
    bool isNpc;
};

struct DamageTrackEntry {
    DamageTrackEntry()
    : damager(INVALIDSERIAL), damageDone(0), lastDamageType(PHYSICAL),
    lastDamageDone(INVALIDSERIAL) {}
    DamageTrackEntry(serial_t dmgr, std::int32_t dmgDn, weathertype_t dmgType, timerval_t lstDmgDn)
    : damager(dmgr), damageDone(dmgDn), lastDamageType(dmgType), lastDamageDone(lstDmgDn) {}
    serial_t damager;             // who did the damage?
    std::int32_t damageDone;            // how much damage has been accumulated?
    weathertype_t lastDamageType; // what type of damage was dealt most recently?
    timerval_t lastDamageDone;    // when was the last time that damage was done?
};

bool DTEgreater(DamageTrackEntry &elem1, DamageTrackEntry &elem2);

class CChar : public CBaseObject {
private:
    
    struct NPCValues {
        NPCValues();
        void DumpBody(std::ostream &outStream);
        
        std::int8_t wanderMode;    // NPC Wander Mode
        std::int8_t oldWanderMode; // Used for fleeing npcs
        serial_t fTarg;       // NPC Follow Target
        std::int16_t fx[2];         // NPC Wander Point x
        std::int16_t fy[2];         // NPC Wander Point y
        std::int8_t fz;            // NPC Wander Point z
        std::int16_t aiType;
        std::int16_t spellAttack;
        std::int8_t spellDelay; // won't time out for more than 255 seconds!
        std::int16_t taming;
        std::int16_t fleeAt;       // HP Level to flee at
        std::int16_t reAttackAt;   // HP Level to re-Attack at
        std::uint8_t fleeDistance; // Maximum distance in tiles the NPC can flee in one go
        std::uint8_t splitNum;
        std::uint8_t splitChance;
        std::uint8_t trainingPlayerIn;
        std::uint32_t goldOnHand;
        std::uint8_t questType;
        std::uint8_t questDestRegion;
        std::uint8_t questOrigRegion;
        CBaseObject *petGuarding;
        cnpc_flag_t npcFlag;
        std::bitset<8> boolFlags;
        std::int16_t peaceing;
        std::int16_t provoing;
        
        std::uint16_t tamedHungerRate;  // The rate at which hunger decreases when char is tamed
        std::uint16_t tamedThirstRate;  // The rate at which thirst decreases when char is tamed
        std::uint8_t hungerWildChance; // The chance that the char goes wild when hungry
        std::uint8_t thirstWildChance; // The chance that the char goes wild when thirsty
        R32 walkingSpeed;
        R32 runningSpeed;
        R32 fleeingSpeed;
        std::int8_t pathFail;
        std::int8_t pathResult;
        std::uint16_t pathTargX;
        std::uint16_t pathTargY;
        
        std::deque<std::uint8_t> pathToFollow; // let's use a queue of directions to follow
        
        std::uint8_t controlSlots;                 // Amount of control slots taken up by a particular NPC
        std::vector<CChar *> petFriends;   // Temporary list of friends a pet has
        GenericList<CChar *> petOwnerList; // Persistent list of owners a pet has previously had
        std::unordered_map<serial_t, TargetInfo>
        combatIgnore; // Chars this char ignores as targets in combat, with timestamps
        std::uint16_t maxLoyalty;  // Pet's maximum loyalty to its master
        std::uint16_t loyalty;     // Pet's current loyalty to its master
        std::uint16_t orneriness;  // Difficulty to control pet
        
        std::string foodList;
        
        R32 mountedWalkingSpeed;
        R32 mountedRunningSpeed;
        R32 mountedFleeingSpeed;
    };
    
    struct PlayerValues {
        PlayerValues();
        void DumpBody(std::ostream &outStream);
        
        serial_t callNum;        // Callnum GM or Counsellor is on
        serial_t playerCallNum;  // Players call number in GM or Counsellor requestQueue
        serial_t trackingTarget; // Tracking target ID
        std::uint8_t squelched;        // Squelching
        std::uint8_t commandLevel;     // 0 = player, 1 = counselor, 2 = GM
        std::uint8_t postType;
        std::uint16_t hairStyle;
        std::uint16_t beardStyle;
        colour_t hairColour;
        colour_t beardColour;
        CItem *speechItem;
        std::uint8_t speechMode;
        std::uint8_t speechId;
        cScript *speechCallback;
        serial_t robe;
        std::uint16_t accountNum;
        std::uint16_t origSkin;
        std::uint16_t origId;     // Backup of body type for polymorph
        std::uint8_t fixedLight; // Fixed lighting level (For chars in dungeons, where they dont see the
        // night)
        std::uint16_t deaths;
        CSocket *socket;
        
        std::vector<CChar *> trackingTargets;
        
        std::string origName; // original name - for Incognito
        
        std::string lastOn; // Last time a character was on
        std::uint32_t lastOnSecs;    // Last time a character was on in seconds.
        
        serial_t townVote;
        std::int8_t townPriv; // 0=non resident (Other privledges added as more functionality added)
        std::uint8_t controlSlotsUsed; // The total number of control slots currently taken up by
        // followers/pets
        std::uint32_t createdOn;        // Timestamp for when player character was created
        std::uint32_t
        npcGuildJoined; // Timestamp for when player character joined NPC guild (0=never joined)
        std::uint32_t playTime;      // Character's full playtime
        
        std::uint8_t atrophy[INTELLECT + 1];
        skilllock_t lockState[INTELLECT + 1]; // state of the skill locks
        
        // speechMode valid values
        // 0 normal speech
        // 1 GM page
        // 2 Counselor page
        // 3 Player Vendor item pricing
        // 4 Player Vendor item describing
        // 5 Key renaming
        // 6 Name deed
        // 7 Rune renaming
        // 8 Sign renaming
        // 9 JavaScript speech
    };
    // Base Characters
protected:
    NPCValues *mNPC;
    PlayerValues *mPlayer;
    
    std::bitset<64> bools; // lots of flags
    std::int8_t fontType;         // Speech font to use
    std::uint16_t maxHP;
    std::uint16_t maxHP_oldstr;
    raceid_t oldRace;
    std::int16_t maxMana;
    std::uint16_t maxMana_oldint;
    std::int16_t maxStam;
    std::uint16_t maxStam_olddex;
    colour_t sayColor;
    colour_t emoteColor;
    std::int8_t cell;       // Reserved for jailing players
    CItem *packItem; // Characters backpack
    serial_t targ;     // Current combat target
    serial_t attacker; // Character who attacked this character
    std::int8_t hunger;     // Level of hungerness, 6 = full, 0 = "empty"
    std::int8_t thirst;     // Level of thirstiness, 6 = full, 0 = "empty"
    std::uint16_t regionNum;
    std::uint16_t town; // Matches Region number in regions.dfn
    
    std::uint8_t brkPeaceChanceGain;
    std::uint8_t brkPeaceChance;
    std::uint16_t advObj;        // Has used advance gate?
    serial_t guildFealty; // Serial of player you are loyal to (default=yourself) (DasRaetsel)
    std::int16_t guildNumber;   // Number of guild player is in (0=no guild)     (DasRaetsel)
    
    std::uint8_t flag; // 1=red 2=grey 4=Blue 8=green 10=Orange // should it not be 0x10??? sounds like
    // we're trying to do
    std::int8_t spellCast;
    std::uint8_t nextAct;    // time to next spell action..
    std::int8_t stealth;    // stealth ( steps already done, -1=not using )
    std::uint8_t running;    // Stamina Loose while running
    raceid_t raceGate; // Race gate that has been used
    std::uint8_t step;       // 1 if step 1 0 if step 2 3 if step 1 skip 2 if step 2 skip
    
    std::bitset<16> priv;
    
    std::string guildTitle; // Title Guildmaster granted player
    // (DasRaetsel)
    
    timerval_t charTimers[tCHAR_COUNT];
    
    timerval_t regen[3];
    timerval_t weathDamage[WEATHNUM]; // Light Damage timer
    
    std::uint8_t PoisonStrength;
    BodyType bodyType;
    std::uint32_t lastMoveTime; // Timestamp for when character moved last
    std::uint16_t npcGuild;     // ID of NPC guild character is in (0=no NPC guild)
    
    skillval_t baseskill[ALLSKILLS]; // Base skills without stat modifiers
    skillval_t skill[INTELLECT + 1]; // List of skills (with stat modifiers)
    
    std::map<itemlayers_t, CItem *> itemLayers;
    std::map<itemlayers_t, CItem *>::iterator layerCtr;
    std::unordered_map<serial_t, TargetInfo>
    aggressorFlags; // Chars this char is marked as aggressor to, with timestamps
    std::unordered_map<serial_t, TargetInfo>
    permaGreyFlags; // Chars this char is marked as permanent grey to, with timestamps
    GenericList<CChar *> petsOwned;
    GenericList<CChar *> activeFollowers;
    GenericList<CItem *> ownedCorpses;
    std::vector<CItem *> ownedItems;
    std::bitset<32> skillUsed[2]; // no more than 64 skills
    std::bitset<UT_COUNT> updateTypes;
    
    GenericList<DamageTrackEntry *> damageDealt;
    GenericList<DamageTrackEntry *> damageHealed;
    
    virtual bool DumpHeader(std::ostream &outStream) const override;
    virtual bool DumpBody(std::ostream &outStream) const override;
    virtual bool HandleLine(std::string &UTag, std::string &data) override;
    virtual bool LoadRemnants() override;
    
    void CopyData(CChar *target);
    
    void CreateNPC();
    void CreatePlayer();
    
    bool IsValidNPC() const;
    bool IsValidPlayer() const;
    
public:
    BodyType GetBodyType();
    
    virtual void SetWeight(std::int32_t newVal, bool doWeightUpdate = true) override;
    
    bool GetUpdate(updatetypes_t updateType) const;
    void ClearUpdate();
    virtual void Dirty(updatetypes_t updateType) override;
    void UpdateRegion();
    
    void UpdateDamageTrack();
    auto CheckDamageTrack(serial_t serialToCheck, timerval_t lastXSeconds) -> bool;
    
    void SetPoisonStrength(std::uint8_t value);
    std::uint8_t GetPoisonStrength() const;
    
    GenericList<CChar *> *GetPetList();
    GenericList<CChar *> *GetFollowerList();
    GenericList<CChar *> *GetPetOwnerList();
    
    auto GetOwnedCorpses() -> GenericList<CItem *> *;
    auto GetOwnedItems() -> std::vector<CItem *> *;
    
    auto AddCorpse(CItem *corpseToAdd) -> bool;
    auto RemoveCorpse(CItem *corpseToRemove) -> bool;
    
    auto AddFollower(CChar *npcToAdd) -> bool;
    auto RemoveFollower(CChar *followerToRemove) -> bool;
    
    auto GetAggressorFlags() const -> const std::unordered_map<serial_t, TargetInfo>;
    auto AddAggressorFlag(serial_t toAdd) -> void;
    auto RemoveAggressorFlag(serial_t toRemove) -> void;
    auto CheckAggressorFlag(serial_t toCheck) -> bool;
    auto UpdateAggressorFlagTimestamp(serial_t toUpdate) -> void;
    auto ClearAggressorFlags() -> void;
    auto IsAggressor(bool checkForPlayersOnly) -> bool;
    auto AggressorFlagMaintenance() -> void;
    
    auto GetPermaGreyFlags() const -> const std::unordered_map<serial_t, TargetInfo>;
    auto AddPermaGreyFlag(serial_t toAdd) -> void;
    auto RemovePermaGreyFlag(serial_t toRemove) -> void;
    auto CheckPermaGreyFlag(serial_t toCheck) -> bool;
    auto UpdatePermaGreyFlagTimestamp(serial_t toUpdate) -> void;
    auto ClearPermaGreyFlags() -> void;
    auto IsPermaGrey(bool checkForPlayersOnly) -> bool;
    auto PermaGreyFlagMaintenance() -> void;
    
    void AddOwnedItem(CItem *toAdd);
    void RemoveOwnedItem(CItem *toRemove);
    
    void DoLoyaltyUpdate();
    void DoHunger(CSocket *mSock);
    void DoThirst(CSocket *mSock);
    void CheckPetOfflineTimeout();
    std::int8_t GetHunger() const;
    std::int8_t GetThirst() const;
    std::uint16_t GetTamedHungerRate() const;
    std::uint16_t GetTamedThirstRate() const;
    std::uint8_t GetTamedHungerWildChance() const;
    std::uint8_t GetTamedThirstWildChance() const;
    std::uint16_t GetTown() const;
    std::string GetFood() const;
    
    bool SetHunger(std::int8_t newValue);
    bool SetThirst(std::int8_t newValue);
    void SetTamedHungerRate(std::uint16_t newValue);
    void SetTamedThirstRate(std::uint16_t newValue);
    void SetTamedHungerWildChance(std::uint8_t newValue);
    void SetTamedThirstWildChance(std::uint8_t newValue);
    void SetTown(std::uint16_t newValue);
    void SetFood(std::string food);
    
    std::uint8_t GetBrkPeaceChanceGain() const;
    void SetBrkPeaceChanceGain(std::uint8_t newValue);
    
    std::uint8_t GetBrkPeaceChance() const;
    void SetBrkPeaceChance(std::uint8_t newValue);
    
    void SetMounted(bool newValue);
    bool GetMounted() const;
    
    void SetStabled(bool newValue);
    bool GetStabled() const;
    void SetFlying(bool newValue);
    bool IsFlying() const;
    
    bool ToggleFlying();
    
    void SetMaxHPFixed(bool newValue);
    bool GetMaxHPFixed() const;
    void SetMaxManaFixed(bool newValue);
    bool GetMaxManaFixed() const;
    void SetMaxStamFixed(bool newValue);
    bool GetMaxStamFixed() const;
    
    bool DecHunger(const std::int8_t amt = 1);
    bool DecThirst(const std::int8_t amt = 1);
    
    bool IsUnicode() const;
    bool IsNpc() const;
    bool IsAwake() const;
    bool IsEvading() const;
    bool IsShop() const;
    bool IsDead() const;
    bool GetCanAttack() const;
    bool IsAtWar() const;
    bool IsPassive() const;
    auto HasStolen() -> bool;
    auto HasStolen(bool newValue) -> void;
    bool IsOnHorse() const;
    bool GetTownTitle() const;
    bool GetReactiveArmour() const;
    bool CanTrain() const;
    bool CanBeHired() const;
    bool GetGuildToggle() const;
    bool IsTamed() const;
    bool IsGuarded() const;
    bool CanRun() const;
    bool IsUsingPotion() const;
    bool MayLevitate() const;
    bool WillHunger() const;
    bool WillThirst() const;
    bool IsMeditating() const;
    bool IsCasting() const;
    bool IsJSCasting() const;
    
    void SetUnicode(bool newVal);
    void SetNpc(bool newVal);
    void SetAwake(bool newVal);
    void SetEvadeState(bool newVal);
    void SetShop(bool newVal);
    void SetDead(bool newValue);
    void SetCanAttack(bool newValue);
    void SetPeace(std::uint32_t newValue);
    void SetWar(bool newValue);
    void SetPassive(bool newValue);
    void SetOnHorse(bool newValue);
    void SetTownTitle(bool newValue);
    void SetReactiveArmour(bool newValue);
    void SetCanHire(bool newValue);
    void SetCanTrain(bool newValue);
    void SetGuildToggle(bool newValue);
    void SetTamed(bool newValue);
    void SetGuarded(bool newValue);
    void SetRun(bool newValue);
    void SetUsingPotion(bool newVal);
    void SetLevitate(bool newValue);
    void SetHungerStatus(bool newValue);
    void SetThirstStatus(bool newValue);
    void SetMeditating(bool newValue);
    void SetCasting(bool newValue);
    void SetJSCasting(bool newValue);
    void SetInBuilding(bool newValue);
    
    void SetTownVote(std::uint32_t newValue);
    void SetGuildFealty(std::uint32_t newValue);
    
    std::uint32_t GetTownVote() const;
    std::uint32_t GetGuildFealty() const;
    
    std::string GetGuildTitle() const;
    void SetGuildTitle(const std::string &newValue);
    
    timerval_t GetTimer(cc_tid_t timerId) const;
    timerval_t GetRegen(std::uint8_t part) const;
    timerval_t GetWeathDamage(std::uint8_t part) const;
    std::uint8_t GetNextAct() const;
    
    void SetTimer(cc_tid_t timerId, timerval_t value);
    void SetRegen(timerval_t newValue, std::uint8_t part);
    void SetWeathDamage(timerval_t newValue, std::uint8_t part);
    void SetNextAct(std::uint8_t newVal);
    
    colour_t GetEmoteColour() const;
    colour_t GetSayColour() const;
    std::uint16_t GetSkin() const;
    
    void SetSkin(std::uint16_t value);
    void SetEmoteColour(colour_t newValue);
    void SetSayColour(colour_t newValue);
    
    std::int8_t GetStealth() const;
    std::int8_t GetCell() const;
    std::uint8_t GetRunning() const;
    std::uint8_t GetStep() const;
    CTownRegion *GetRegion() const;
    std::uint16_t GetRegionNum() const;
    
    void SetCell(std::int8_t newVal);
    void SetStealth(std::int8_t newValue);
    void SetRunning(std::uint8_t newValue);
    void SetStep(std::uint8_t newValue);
    void SetRegion(std::uint16_t newValue);
    virtual void SetOldLocation(std::int16_t newX, std::int16_t newY, std::int8_t newZ) override;
    virtual void SetLocation(std::int16_t newX, std::int16_t newY, std::int8_t newZ, std::uint8_t world, std::uint16_t instanceId) override;
    virtual void SetLocation(std::int16_t newX, std::int16_t newY, std::int8_t newZ) override;
    virtual void SetLocation(const CBaseObject *toSet) override;
    void WalkZ(std::int8_t newZ);
    void WalkDir(std::int8_t newDir);
    
    CItem *GetPackItem();
    CChar *GetTarg() const;
    CChar *GetAttacker() const;
    std::uint16_t GetAdvObj() const;
    raceid_t GetRaceGate() const;
    
    void SetPackItem(CItem *newVal);
    void SetTarg(CChar *newTarg);
    void SetAttacker(CChar *newValue);
    void SetAdvObj(std::uint16_t newValue);
    void SetRaceGate(raceid_t newValue);
    
    std::int8_t GetSpellCast() const;
    void SetSpellCast(std::int8_t newValue);
    
    std::uint16_t GetPriv() const;
    std::int8_t GetTownPriv() const;
    bool IsGM() const;
    bool CanBroadcast() const;
    bool IsInvulnerable() const;
    bool GetSingClickSer() const;
    bool NoSkillTitles() const;
    bool IsGMPageable() const;
    bool CanSnoop() const;
    bool IsCounselor() const;
    bool AllMove() const;
    bool IsFrozen() const;
    bool ViewHouseAsIcon() const;
    bool NoNeedMana() const;
    bool IsDispellable() const;
    bool IsTempReflected() const;
    bool IsPermReflected() const;
    bool NoNeedReags() const;
    
    void SetGM(bool newValue);
    void SetBroadcast(bool newValue);
    void SetInvulnerable(bool newValue);
    void SetSingClickSer(bool newValue);
    void SetSkillTitles(bool newValue);
    void SetGMPageable(bool newValue);
    void SetSnoop(bool newValue);
    void SetCounselor(bool newValue);
    void SetAllMove(bool newValue);
    void SetFrozen(bool newValue);
    void SetViewHouseAsIcon(bool newValue);
    void SetNoNeedMana(bool newValue);
    void SetDispellable(bool newValue);
    void SetTempReflected(bool newValue);
    void SetPermReflected(bool newValue);
    void SetNoNeedReags(bool newValue);
    
    void SetPriv(std::uint16_t newValue);
    void SetTownpriv(std::int8_t newValue);
    
    std::uint16_t GetBaseSkill(std::uint8_t skillToGet) const;
    std::uint16_t GetSkill(std::uint8_t skillToGet) const;
    
    void SetBaseSkill(skillval_t newSkillValue, std::uint8_t skillToSet);
    void SetSkill(skillval_t newSkillValue, std::uint8_t skillToSet);
    
    std::uint16_t GetDeaths() const; // can we die 4 billion times?!
    std::int16_t GetGuildNumber() const;
    std::uint8_t GetFlag() const;
    std::uint8_t GetControlSlotsUsed() const;
    
    void SetDeaths(std::uint16_t newVal);
    void SetFlag(std::uint8_t newValue);
    void SetGuildNumber(std::int16_t newValue);
    void SetControlSlotsUsed(std::uint8_t newValue);
    
    std::int8_t GetFontType() const;
    void SetFontType(std::int8_t newType);
    
    CSocket *GetSocket() const;
    void SetSocket(CSocket *newVal);
    
    CChar();
    virtual ~CChar();
    
    CChar *Dupe();
    virtual void RemoveFromSight(CSocket *mSock = nullptr);
    virtual void RemoveAllObjectsFromSight(CSocket *mSock = nullptr);
    void SendWornItems(CSocket *s);
    void Teleport();
    void ExposeToView();
    virtual void Update(CSocket *mSock = nullptr, bool drawGamePlayer = false,
                        bool sendToSelf = true) override;
    virtual void SendToSocket(CSocket *s, bool drawGamePlayer = false) override;
    
    CItem *GetItemAtLayer(itemlayers_t Layer);
    bool WearItem(CItem *toWear);
    bool TakeOffItem(itemlayers_t Layer);
    
    CItem *FirstItem();
    CItem *NextItem();
    bool FinishedItems();
    
    void BreakConcentration(CSocket *sock = nullptr);
    
    virtual bool Save(std::ostream &outStream) override;
    virtual void PostLoadProcessing() override;
    
    std::int16_t ActualStrength() const;
    virtual std::int16_t GetStrength() const override;
    
    std::int16_t ActualDexterity() const;
    virtual std::int16_t GetDexterity() const override;
    
    std::int16_t ActualIntelligence() const;
    virtual std::int16_t GetIntelligence() const override;
    
    void IncStrength2(std::int16_t toAdd = 1);
    void IncDexterity2(std::int16_t toAdd = 1);
    void IncIntelligence2(std::int16_t toAdd = 1);
    
    bool IsMurderer() const;
    bool IsCriminal() const;
    bool IsInnocent() const;
    bool IsNeutral() const;
    
    void SetFlagRed();
    void SetFlagBlue();
    void SetFlagGray();
    void SetFlagNeutral();
    
    void StopSpell();
    bool SkillUsed(std::uint8_t skillNum) const;
    void SkillUsed(bool value, std::uint8_t skillNum);
    
    bool IsPolymorphed() const;
    bool IsIncognito() const;
    bool IsDisguised() const;
    void IsPolymorphed(bool newValue);
    void IsIncognito(bool newValue);
    void IsDisguised(bool newValue);
    bool IsJailed() const;
    
    void SetMaxHP(std::uint16_t newmaxhp, std::uint16_t newoldstr, raceid_t newoldrace);
    void SetFixedMaxHP(std::int16_t newmaxhp);
    void SetMaxMana(std::int16_t newmaxmana, std::uint16_t newoldint, raceid_t newoldrace);
    void SetFixedMaxMana(std::int16_t newmaxmana);
    void SetMaxStam(std::int16_t newmaxstam, std::uint16_t newolddex, raceid_t newoldrace);
    void SetFixedMaxStam(std::int16_t newmaxstam);
    virtual std::uint16_t GetMaxHP();
    std::int16_t GetMaxMana();
    std::int16_t GetMaxStam();
    std::uint16_t GetMaxLoyalty() const;
    std::uint16_t GetLoyalty() const;
    virtual void SetMana(std::int16_t newValue) override;
    virtual void SetHP(std::int16_t newValue) override;
    virtual void SetStamina(std::int16_t newValue) override;
    virtual void SetStrength(std::int16_t newValue) override;
    virtual void SetDexterity(std::int16_t newValue) override;
    virtual void SetIntelligence(std::int16_t newValue) override;
    virtual void SetStrength2(std::int16_t newValue) override;
    virtual void SetDexterity2(std::int16_t newValue) override;
    virtual void SetIntelligence2(std::int16_t newValue) override;
    void IncStamina(std::int16_t toInc);
    void IncMana(std::int16_t toInc);
    void SetMaxLoyalty(std::uint16_t newMaxLoyalty);
    void SetLoyalty(std::uint16_t newLoyalty);
    
    void ToggleCombat();
    
    virtual void SetPoisoned(std::uint8_t newValue) override;
    
    bool InDungeon();
    bool InBuilding();
    
    void TextMessage(CSocket *s, std::string toSay, speechtype_t msgType, bool spamTimer);
    void TextMessage(CSocket *s, std::int32_t dictEntry, speechtype_t msgType, int spamTimer, ...);
    
    virtual void Cleanup() override;
    virtual void Delete() override;
    virtual bool CanBeObjType(CBaseObject::type_t toCompare) const override;
    
    flagcolors_t FlagColour(CChar *toCompare);
    void Heal(std::int16_t healValue, CChar *healer = nullptr);
    bool Damage(std::int16_t damageValue, weathertype_t damageType, CChar *attacker = nullptr,
                bool doRepsys = false);
    std::int16_t GetKarma() const;
    void ReactOnDamage(weathertype_t damageType, CChar *attacker = nullptr);
    void Die(CChar *attacker, bool doRepsys);
    
    // Values determining if the character is in a party or not, save us shortcutting in a few
    // places These values don't get saved or loaded, as only NPC parties get rebuilt, and that will
    // be done via the PartyFactory Load/Save routines, and not through here
    void InParty(bool value);
    bool InParty() const;
    
    // NPC Characters
protected:
    virtual void RemoveSelfFromOwner() override;
    virtual void AddSelfToOwner() override;
    
public:
    void ClearFriendList();
    auto GetFriendList() -> std::vector<CChar *> *;
    
    void ClearPetOwnerList();
    bool AddPetOwnerToList(CChar *toAdd);
    bool RemovePetOwnerFromList(CChar *toRemove);
    bool IsOnPetOwnerList(CChar *toCheck);
    
    bool AddFriend(CChar *toAdd);
    bool RemoveFriend(CChar *toRemove);
    
    auto GetCombatIgnore() const -> const std::unordered_map<serial_t, TargetInfo>;
    auto AddToCombatIgnore(serial_t toAdd, bool isNpc) -> void;
    auto RemoveFromCombatIgnore(serial_t toRemove) -> void;
    auto CheckCombatIgnore(serial_t toCheck) -> bool;
    auto ClearCombatIgnore() -> void;
    auto CombatIgnoreMaintenance() -> void;
    
    std::int16_t GetNpcAiType() const;
    std::uint16_t GetNPCGuild() const;
    std::int16_t GetTaming() const;
    std::int16_t GetPeaceing() const;
    std::int16_t GetProvoing() const;
    std::uint8_t GetTrainingPlayerIn() const;
    std::uint32_t GetHoldG() const;
    std::uint8_t GetSplit() const;
    std::uint8_t GetSplitChance() const;
    std::uint8_t GetOwnerCount();
    std::uint8_t GetControlSlots() const;
    std::uint16_t GetOrneriness() const;
    
    void SetNPCAiType(std::int16_t newValue);
    void SetNPCGuild(std::uint16_t newValue);
    void SetTaming(std::int16_t newValue);
    void SetPeaceing(std::int16_t newValue);
    void SetProvoing(std::int16_t newValue);
    void SetTrainingPlayerIn(std::uint8_t newValue);
    void SetHoldG(std::uint32_t newValue);
    void SetSplit(std::uint8_t newValue);
    void SetSplitChance(std::uint8_t newValue);
    void SetControlSlots(std::uint8_t newVal);
    void SetOrneriness(std::uint16_t newVal);
    
    std::int8_t GetPathFail() const;
    void SetPathFail(std::int8_t newValue);
    
    std::int8_t GetPathResult() const;
    void SetPathResult(std::int8_t newValue);
    
    std::uint16_t GetPathTargX() const;
    void SetPathTargX(std::uint16_t newValue);
    std::uint16_t GetPathTargY() const;
    void SetPathTargY(std::uint16_t newValue);
    
    void SetGuarding(CBaseObject *newValue);
    
    CBaseObject *GetGuarding() const;
    
    std::int16_t GetFx(std::uint8_t part) const;
    std::int16_t GetFy(std::uint8_t part) const;
    std::int8_t GetFz() const;
    std::int8_t GetNpcWander() const;
    std::int8_t GetOldNpcWander() const;
    
    void SetFx(std::int16_t newVal, std::uint8_t part);
    void SetFy(std::int16_t newVal, std::uint8_t part);
    void SetFz(std::int8_t newVal);
    void SetNpcWander(std::int8_t newValue, bool initArea = false);
    void SetOldNpcWander(std::int8_t newValue);
    
    CChar *GetFTarg() const;
    void SetFTarg(CChar *newTarg);
    
    std::int16_t GetSpAttack() const;
    std::int8_t GetSpDelay() const;
    
    void SetSpAttack(std::int16_t newValue);
    void SetSpDelay(std::int8_t newValue);
    
    std::uint8_t GetQuestType() const;
    std::uint8_t GetQuestOrigRegion() const;
    std::uint8_t GetQuestDestRegion() const;
    
    void SetQuestDestRegion(std::uint8_t newValue);
    void SetQuestType(std::uint8_t newValue);
    void SetQuestOrigRegion(std::uint8_t newValue);
    
    std::int16_t GetFleeAt() const;
    std::int16_t GetReattackAt() const;
    std::uint8_t GetFleeDistance() const;
    
    void SetFleeAt(std::int16_t newValue);
    void SetReattackAt(std::int16_t newValue);
    void SetFleeDistance(std::uint8_t newValue);
    
    std::uint8_t PopDirection();
    void PushDirection(std::uint8_t newDir, bool pushFront = false);
    bool StillGotDirs() const;
    void FlushPath();
    
    cnpc_flag_t GetNPCFlag() const;
    void SetNPCFlag(cnpc_flag_t flagType);
    
    R32 GetWalkingSpeed() const;
    void SetWalkingSpeed(R32 newValue);
    
    R32 GetRunningSpeed() const;
    void SetRunningSpeed(R32 newValue);
    
    R32 GetFleeingSpeed() const;
    void SetFleeingSpeed(R32 newValue);
    
    R32 GetMountedWalkingSpeed() const;
    void SetMountedWalkingSpeed(R32 newValue);
    
    R32 GetMountedRunningSpeed() const;
    void SetMountedRunningSpeed(R32 newValue);
    
    R32 GetMountedFleeingSpeed() const;
    void SetMountedFleeingSpeed(R32 newValue);
    
    // Player Characters
public:
    void SetAccount(AccountEntry &actbAccount);
    AccountEntry &GetAccount();
    std::uint16_t GetAccountNum() const;
    void SetAccountNum(std::uint16_t newVal);
    
    void SetRobe(serial_t newValue);
    serial_t GetRobe() const;
    
    std::uint16_t GetOrgId() const;
    void SetOrgSkin(std::uint16_t value);
    void SetOrgId(std::uint16_t value);
    std::uint16_t GetOrgSkin() const;
    std::string GetOrgName() const;
    void SetOrgName(std::string newName);
    
    std::uint8_t GetCommandLevel() const;
    void SetCommandLevel(std::uint8_t newValue);
    std::uint8_t GetPostType() const;
    void SetPostType(std::uint8_t newValue);
    void SetPlayerCallNum(serial_t newValue);
    void SetCallNum(serial_t newValue);
    
    serial_t GetCallNum() const;
    serial_t GetPlayerCallNum() const;
    
    void SetLastOn(std::string newValue);
    std::string GetLastOn() const;
    void SetLastOnSecs(std::uint32_t newValue);
    std::uint32_t GetLastOnSecs() const;
    
    auto GetPlayTime() const -> std::uint32_t;
    auto SetPlayTime(std::uint32_t newValue) -> void;
    
    void SetCreatedOn(std::uint32_t newValue);
    std::uint32_t GetCreatedOn() const;
    
    void SetNPCGuildJoined(std::uint32_t newValue);
    std::uint32_t GetNPCGuildJoined() const;
    
    std::uint32_t LastMoveTime() const;
    void LastMoveTime(std::uint32_t newValue);
    
    CChar *GetTrackingTarget() const;
    CChar *GetTrackingTargets(std::uint8_t targetNum) const;
    serial_t GetTrackingTargetSerial() const;
    void SetTrackingTarget(CChar *newValue);
    void SetTrackingTargets(CChar *newValue, std::uint8_t targetNum);
    
    std::uint8_t GetSquelched() const;
    void SetSquelched(std::uint8_t newValue);
    
    CItem *GetSpeechItem() const;
    std::uint8_t GetSpeechMode() const;
    std::uint8_t GetSpeechId() const;
    cScript *GetSpeechCallback() const;
    
    void SetSpeechMode(std::uint8_t newValue);
    void SetSpeechId(std::uint8_t newValue);
    void SetSpeechCallback(cScript *newValue);
    void SetSpeechItem(CItem *newValue);
    
    std::uint16_t GetHairStyle() const;
    std::uint16_t GetBeardStyle() const;
    colour_t GetHairColour() const;
    colour_t GetBeardColour() const;
    
    void SetHairColour(colour_t value);
    void SetBeardColour(colour_t value);
    void SetHairStyle(std::uint16_t value);
    void SetBeardStyle(std::uint16_t value);
    
    std::uint8_t GetFixedLight() const;
    void SetFixedLight(std::uint8_t newVal);
    
    std::uint8_t GetAtrophy(std::uint8_t skillToGet) const;
    skilllock_t GetSkillLock(std::uint8_t skillToGet) const;
    void SetAtrophy(std::uint8_t newValue, std::uint8_t skillToSet);
    void SetSkillLock(skilllock_t newValue, std::uint8_t skillToSet);
    
    std::uint32_t CountHousesOwned(bool countCoOwnedHouses);
};

#endif
