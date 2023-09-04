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

struct DamageTrackEntry_st {
    DamageTrackEntry_st()
        : damager(INVALIDSERIAL), damageDone(0), lastDamageType(PHYSICAL),
          lastDamageDone(INVALIDSERIAL) {}
    DamageTrackEntry_st(serial_t dmgr, std::int32_t dmgDn, WeatherType dmgType, timerval_t lstDmgDn)
        : damager(dmgr), damageDone(dmgDn), lastDamageType(dmgType), lastDamageDone(lstDmgDn) {}
    serial_t damager;             // who did the damage?
    std::int32_t damageDone;            // how much damage has been accumulated?
    WeatherType lastDamageType; // what type of damage was dealt most recently?
    timerval_t lastDamageDone;    // when was the last time that damage was done?
};

bool DTEgreater(DamageTrackEntry_st &elem1, DamageTrackEntry_st &elem2);

class CChar : public CBaseObject {
  private:

    struct NPCValues_st {
        NPCValues_st();
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

    struct PlayerValues_st {
        PlayerValues_st();
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
        SkillLock lockState[INTELLECT + 1]; // state of the skill locks

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
    NPCValues_st *mNPC;
    PlayerValues_st *mPlayer;

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

    std::map<ItemLayers, CItem *> itemLayers;
    std::map<ItemLayers, CItem *>::iterator layerCtr;
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

    GenericList<DamageTrackEntry_st *> damageDealt;
    GenericList<DamageTrackEntry_st *> damageHealed;

    virtual bool DumpHeader(std::ostream &outStream) const override;
    virtual bool DumpBody(std::ostream &outStream) const override;
    virtual bool HandleLine(std::string &UTag, std::string &data) override;
    virtual bool LoadRemnants(void) override;

    void CopyData(CChar *target);

    void CreateNPC(void);
    void CreatePlayer(void);

    bool IsValidNPC(void) const;
    bool IsValidPlayer(void) const;

  public:
    BodyType GetBodyType(void);

    virtual void SetWeight(std::int32_t newVal, bool doWeightUpdate = true) override;

    bool GetUpdate(updatetypes_t updateType) const;
    void ClearUpdate(void);
    virtual void Dirty(updatetypes_t updateType) override;
    void UpdateRegion(void);

    void UpdateDamageTrack(void);
    auto CheckDamageTrack(serial_t serialToCheck, timerval_t lastXSeconds) -> bool;

    void SetPoisonStrength(std::uint8_t value);
    std::uint8_t GetPoisonStrength(void) const;

    GenericList<CChar *> *GetPetList(void);
    GenericList<CChar *> *GetFollowerList(void);
    GenericList<CChar *> *GetPetOwnerList(void);

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

    void DoLoyaltyUpdate(void);
    void DoHunger(CSocket *mSock);
    void DoThirst(CSocket *mSock);
    void CheckPetOfflineTimeout(void);
    std::int8_t GetHunger(void) const;
    std::int8_t GetThirst(void) const;
    std::uint16_t GetTamedHungerRate(void) const;
    std::uint16_t GetTamedThirstRate(void) const;
    std::uint8_t GetTamedHungerWildChance(void) const;
    std::uint8_t GetTamedThirstWildChance(void) const;
    std::uint16_t GetTown(void) const;
    std::string GetFood(void) const;

    bool SetHunger(std::int8_t newValue);
    bool SetThirst(std::int8_t newValue);
    void SetTamedHungerRate(std::uint16_t newValue);
    void SetTamedThirstRate(std::uint16_t newValue);
    void SetTamedHungerWildChance(std::uint8_t newValue);
    void SetTamedThirstWildChance(std::uint8_t newValue);
    void SetTown(std::uint16_t newValue);
    void SetFood(std::string food);

    std::uint8_t GetBrkPeaceChanceGain(void) const;
    void SetBrkPeaceChanceGain(std::uint8_t newValue);

    std::uint8_t GetBrkPeaceChance(void) const;
    void SetBrkPeaceChance(std::uint8_t newValue);

    void SetMounted(bool newValue);
    bool GetMounted(void) const;

    void SetStabled(bool newValue);
    bool GetStabled(void) const;
    void SetFlying(bool newValue);
    bool IsFlying(void) const;

    bool ToggleFlying(void);

    void SetMaxHPFixed(bool newValue);
    bool GetMaxHPFixed(void) const;
    void SetMaxManaFixed(bool newValue);
    bool GetMaxManaFixed(void) const;
    void SetMaxStamFixed(bool newValue);
    bool GetMaxStamFixed(void) const;

    bool DecHunger(const std::int8_t amt = 1);
    bool DecThirst(const std::int8_t amt = 1);

    bool IsUnicode(void) const;
    bool IsNpc(void) const;
    bool IsAwake(void) const;
    bool IsEvading(void) const;
    bool IsShop(void) const;
    bool IsDead(void) const;
    bool GetCanAttack(void) const;
    bool IsAtWar(void) const;
    bool IsPassive(void) const;
    auto HasStolen() -> bool;
    auto HasStolen(bool newValue) -> void;
    bool IsOnHorse(void) const;
    bool GetTownTitle(void) const;
    bool GetReactiveArmour(void) const;
    bool CanTrain(void) const;
    bool CanBeHired(void) const;
    bool GetGuildToggle(void) const;
    bool IsTamed(void) const;
    bool IsGuarded(void) const;
    bool CanRun(void) const;
    bool IsUsingPotion(void) const;
    bool MayLevitate(void) const;
    bool WillHunger(void) const;
    bool WillThirst(void) const;
    bool IsMeditating(void) const;
    bool IsCasting(void) const;
    bool IsJSCasting(void) const;

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

    std::uint32_t GetTownVote(void) const;
    std::uint32_t GetGuildFealty(void) const;

    std::string GetGuildTitle(void) const;
    void SetGuildTitle(const std::string &newValue);

    timerval_t GetTimer(cc_tid_t timerId) const;
    timerval_t GetRegen(std::uint8_t part) const;
    timerval_t GetWeathDamage(std::uint8_t part) const;
    std::uint8_t GetNextAct(void) const;

    void SetTimer(cc_tid_t timerId, timerval_t value);
    void SetRegen(timerval_t newValue, std::uint8_t part);
    void SetWeathDamage(timerval_t newValue, std::uint8_t part);
    void SetNextAct(std::uint8_t newVal);

    colour_t GetEmoteColour(void) const;
    colour_t GetSayColour(void) const;
    std::uint16_t GetSkin(void) const;

    void SetSkin(std::uint16_t value);
    void SetEmoteColour(colour_t newValue);
    void SetSayColour(colour_t newValue);

    std::int8_t GetStealth(void) const;
    std::int8_t GetCell(void) const;
    std::uint8_t GetRunning(void) const;
    std::uint8_t GetStep(void) const;
    CTownRegion *GetRegion(void) const;
    std::uint16_t GetRegionNum(void) const;

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

    CItem *GetPackItem(void);
    CChar *GetTarg(void) const;
    CChar *GetAttacker(void) const;
    std::uint16_t GetAdvObj(void) const;
    raceid_t GetRaceGate(void) const;

    void SetPackItem(CItem *newVal);
    void SetTarg(CChar *newTarg);
    void SetAttacker(CChar *newValue);
    void SetAdvObj(std::uint16_t newValue);
    void SetRaceGate(raceid_t newValue);

    std::int8_t GetSpellCast(void) const;
    void SetSpellCast(std::int8_t newValue);

    std::uint16_t GetPriv(void) const;
    std::int8_t GetTownPriv(void) const;
    bool IsGM(void) const;
    bool CanBroadcast(void) const;
    bool IsInvulnerable(void) const;
    bool GetSingClickSer(void) const;
    bool NoSkillTitles(void) const;
    bool IsGMPageable(void) const;
    bool CanSnoop(void) const;
    bool IsCounselor(void) const;
    bool AllMove(void) const;
    bool IsFrozen(void) const;
    bool ViewHouseAsIcon(void) const;
    bool NoNeedMana(void) const;
    bool IsDispellable(void) const;
    bool IsTempReflected(void) const;
    bool IsPermReflected(void) const;
    bool NoNeedReags(void) const;

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

    std::uint16_t GetDeaths(void) const; // can we die 4 billion times?!
    std::int16_t GetGuildNumber(void) const;
    std::uint8_t GetFlag(void) const;
    std::uint8_t GetControlSlotsUsed(void) const;

    void SetDeaths(std::uint16_t newVal);
    void SetFlag(std::uint8_t newValue);
    void SetGuildNumber(std::int16_t newValue);
    void SetControlSlotsUsed(std::uint8_t newValue);

    std::int8_t GetFontType(void) const;
    void SetFontType(std::int8_t newType);

    CSocket *GetSocket(void) const;
    void SetSocket(CSocket *newVal);

    CChar();
    virtual ~CChar();

    CChar *Dupe(void);
    virtual void RemoveFromSight(CSocket *mSock = nullptr);
    virtual void RemoveAllObjectsFromSight(CSocket *mSock = nullptr);
    void SendWornItems(CSocket *s);
    void Teleport(void);
    void ExposeToView(void);
    virtual void Update(CSocket *mSock = nullptr, bool drawGamePlayer = false,
                        bool sendToSelf = true) override;
    virtual void SendToSocket(CSocket *s, bool drawGamePlayer = false) override;

    CItem *GetItemAtLayer(ItemLayers Layer);
    bool WearItem(CItem *toWear);
    bool TakeOffItem(ItemLayers Layer);

    CItem *FirstItem(void);
    CItem *NextItem(void);
    bool FinishedItems(void);

    void BreakConcentration(CSocket *sock = nullptr);

    virtual bool Save(std::ostream &outStream) override;
    virtual void PostLoadProcessing(void) override;

    std::int16_t ActualStrength(void) const;
    virtual std::int16_t GetStrength(void) const override;

    std::int16_t ActualDexterity(void) const;
    virtual std::int16_t GetDexterity(void) const override;

    std::int16_t ActualIntelligence(void) const;
    virtual std::int16_t GetIntelligence(void) const override;

    void IncStrength2(std::int16_t toAdd = 1);
    void IncDexterity2(std::int16_t toAdd = 1);
    void IncIntelligence2(std::int16_t toAdd = 1);

    bool IsMurderer(void) const;
    bool IsCriminal(void) const;
    bool IsInnocent(void) const;
    bool IsNeutral(void) const;

    void SetFlagRed(void);
    void SetFlagBlue(void);
    void SetFlagGray(void);
    void SetFlagNeutral(void);

    void StopSpell(void);
    bool SkillUsed(std::uint8_t skillNum) const;
    void SkillUsed(bool value, std::uint8_t skillNum);

    bool IsPolymorphed(void) const;
    bool IsIncognito(void) const;
    bool IsDisguised(void) const;
    void IsPolymorphed(bool newValue);
    void IsIncognito(bool newValue);
    void IsDisguised(bool newValue);
    bool IsJailed(void) const;

    void SetMaxHP(std::uint16_t newmaxhp, std::uint16_t newoldstr, raceid_t newoldrace);
    void SetFixedMaxHP(std::int16_t newmaxhp);
    void SetMaxMana(std::int16_t newmaxmana, std::uint16_t newoldint, raceid_t newoldrace);
    void SetFixedMaxMana(std::int16_t newmaxmana);
    void SetMaxStam(std::int16_t newmaxstam, std::uint16_t newolddex, raceid_t newoldrace);
    void SetFixedMaxStam(std::int16_t newmaxstam);
    virtual std::uint16_t GetMaxHP(void);
    std::int16_t GetMaxMana(void);
    std::int16_t GetMaxStam(void);
    std::uint16_t GetMaxLoyalty(void) const;
    std::uint16_t GetLoyalty(void) const;
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

    void ToggleCombat(void);

    virtual void SetPoisoned(std::uint8_t newValue) override;

    bool InDungeon(void);
    bool InBuilding(void);

    void TextMessage(CSocket *s, std::string toSay, SpeechType msgType, bool spamTimer);
    void TextMessage(CSocket *s, std::int32_t dictEntry, SpeechType msgType, int spamTimer, ...);

    virtual void Cleanup(void) override;
    virtual void Delete(void) override;
    virtual bool CanBeObjType(ObjectType toCompare) const override;

    FlagColors FlagColour(CChar *toCompare);
    void Heal(std::int16_t healValue, CChar *healer = nullptr);
    bool Damage(std::int16_t damageValue, WeatherType damageType, CChar *attacker = nullptr,
                bool doRepsys = false);
    std::int16_t GetKarma(void) const;
    void ReactOnDamage(WeatherType damageType, CChar *attacker = nullptr);
    void Die(CChar *attacker, bool doRepsys);

    // Values determining if the character is in a party or not, save us shortcutting in a few
    // places These values don't get saved or loaded, as only NPC parties get rebuilt, and that will
    // be done via the PartyFactory Load/Save routines, and not through here
    void InParty(bool value);
    bool InParty(void) const;

    // NPC Characters
  protected:
    virtual void RemoveSelfFromOwner(void) override;
    virtual void AddSelfToOwner(void) override;

  public:
    void ClearFriendList(void);
    auto GetFriendList() -> std::vector<CChar *> *;

    void ClearPetOwnerList(void);
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

    std::int16_t GetNpcAiType(void) const;
    std::uint16_t GetNPCGuild(void) const;
    std::int16_t GetTaming(void) const;
    std::int16_t GetPeaceing(void) const;
    std::int16_t GetProvoing(void) const;
    std::uint8_t GetTrainingPlayerIn(void) const;
    std::uint32_t GetHoldG(void) const;
    std::uint8_t GetSplit(void) const;
    std::uint8_t GetSplitChance(void) const;
    std::uint8_t GetOwnerCount(void);
    std::uint8_t GetControlSlots(void) const;
    std::uint16_t GetOrneriness(void) const;

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

    std::int8_t GetPathFail(void) const;
    void SetPathFail(std::int8_t newValue);

    std::int8_t GetPathResult(void) const;
    void SetPathResult(std::int8_t newValue);

    std::uint16_t GetPathTargX(void) const;
    void SetPathTargX(std::uint16_t newValue);
    std::uint16_t GetPathTargY(void) const;
    void SetPathTargY(std::uint16_t newValue);

    void SetGuarding(CBaseObject *newValue);

    CBaseObject *GetGuarding(void) const;

    std::int16_t GetFx(std::uint8_t part) const;
    std::int16_t GetFy(std::uint8_t part) const;
    std::int8_t GetFz(void) const;
    std::int8_t GetNpcWander(void) const;
    std::int8_t GetOldNpcWander(void) const;

    void SetFx(std::int16_t newVal, std::uint8_t part);
    void SetFy(std::int16_t newVal, std::uint8_t part);
    void SetFz(std::int8_t newVal);
    void SetNpcWander(std::int8_t newValue, bool initArea = false);
    void SetOldNpcWander(std::int8_t newValue);

    CChar *GetFTarg(void) const;
    void SetFTarg(CChar *newTarg);

    std::int16_t GetSpAttack(void) const;
    std::int8_t GetSpDelay(void) const;

    void SetSpAttack(std::int16_t newValue);
    void SetSpDelay(std::int8_t newValue);

    std::uint8_t GetQuestType(void) const;
    std::uint8_t GetQuestOrigRegion(void) const;
    std::uint8_t GetQuestDestRegion(void) const;

    void SetQuestDestRegion(std::uint8_t newValue);
    void SetQuestType(std::uint8_t newValue);
    void SetQuestOrigRegion(std::uint8_t newValue);

    std::int16_t GetFleeAt(void) const;
    std::int16_t GetReattackAt(void) const;
    std::uint8_t GetFleeDistance(void) const;

    void SetFleeAt(std::int16_t newValue);
    void SetReattackAt(std::int16_t newValue);
    void SetFleeDistance(std::uint8_t newValue);

    std::uint8_t PopDirection(void);
    void PushDirection(std::uint8_t newDir, bool pushFront = false);
    bool StillGotDirs(void) const;
    void FlushPath(void);

    cnpc_flag_t GetNPCFlag(void) const;
    void SetNPCFlag(cnpc_flag_t flagType);

    R32 GetWalkingSpeed(void) const;
    void SetWalkingSpeed(R32 newValue);

    R32 GetRunningSpeed(void) const;
    void SetRunningSpeed(R32 newValue);

    R32 GetFleeingSpeed(void) const;
    void SetFleeingSpeed(R32 newValue);

    R32 GetMountedWalkingSpeed(void) const;
    void SetMountedWalkingSpeed(R32 newValue);

    R32 GetMountedRunningSpeed(void) const;
    void SetMountedRunningSpeed(R32 newValue);

    R32 GetMountedFleeingSpeed(void) const;
    void SetMountedFleeingSpeed(R32 newValue);

    // Player Characters
  public:
    void SetAccount(AccountEntry &actbAccount);
    AccountEntry &GetAccount(void);
    std::uint16_t GetAccountNum(void) const;
    void SetAccountNum(std::uint16_t newVal);

    void SetRobe(serial_t newValue);
    serial_t GetRobe(void) const;

    std::uint16_t GetOrgId(void) const;
    void SetOrgSkin(std::uint16_t value);
    void SetOrgId(std::uint16_t value);
    std::uint16_t GetOrgSkin(void) const;
    std::string GetOrgName(void) const;
    void SetOrgName(std::string newName);

    std::uint8_t GetCommandLevel(void) const;
    void SetCommandLevel(std::uint8_t newValue);
    std::uint8_t GetPostType(void) const;
    void SetPostType(std::uint8_t newValue);
    void SetPlayerCallNum(serial_t newValue);
    void SetCallNum(serial_t newValue);

    serial_t GetCallNum(void) const;
    serial_t GetPlayerCallNum(void) const;

    void SetLastOn(std::string newValue);
    std::string GetLastOn(void) const;
    void SetLastOnSecs(std::uint32_t newValue);
    std::uint32_t GetLastOnSecs(void) const;

    auto GetPlayTime() const -> std::uint32_t;
    auto SetPlayTime(std::uint32_t newValue) -> void;

    void SetCreatedOn(std::uint32_t newValue);
    std::uint32_t GetCreatedOn(void) const;

    void SetNPCGuildJoined(std::uint32_t newValue);
    std::uint32_t GetNPCGuildJoined(void) const;

    std::uint32_t LastMoveTime(void) const;
    void LastMoveTime(std::uint32_t newValue);

    CChar *GetTrackingTarget(void) const;
    CChar *GetTrackingTargets(std::uint8_t targetNum) const;
    serial_t GetTrackingTargetSerial(void) const;
    void SetTrackingTarget(CChar *newValue);
    void SetTrackingTargets(CChar *newValue, std::uint8_t targetNum);

    std::uint8_t GetSquelched(void) const;
    void SetSquelched(std::uint8_t newValue);

    CItem *GetSpeechItem(void) const;
    std::uint8_t GetSpeechMode(void) const;
    std::uint8_t GetSpeechId(void) const;
    cScript *GetSpeechCallback(void) const;

    void SetSpeechMode(std::uint8_t newValue);
    void SetSpeechId(std::uint8_t newValue);
    void SetSpeechCallback(cScript *newValue);
    void SetSpeechItem(CItem *newValue);

    std::uint16_t GetHairStyle(void) const;
    std::uint16_t GetBeardStyle(void) const;
    colour_t GetHairColour(void) const;
    colour_t GetBeardColour(void) const;

    void SetHairColour(colour_t value);
    void SetBeardColour(colour_t value);
    void SetHairStyle(std::uint16_t value);
    void SetBeardStyle(std::uint16_t value);

    std::uint8_t GetFixedLight(void) const;
    void SetFixedLight(std::uint8_t newVal);

    std::uint8_t GetAtrophy(std::uint8_t skillToGet) const;
    SkillLock GetSkillLock(std::uint8_t skillToGet) const;
    void SetAtrophy(std::uint8_t newValue, std::uint8_t skillToSet);
    void SetSkillLock(SkillLock newValue, std::uint8_t skillToSet);

    std::uint32_t CountHousesOwned(bool countCoOwnedHouses);
};

#endif
