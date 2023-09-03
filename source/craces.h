#ifndef __Races__
#define __Races__

#include <array>
#include <bitset>
#include <cstdint>
#include <string>
#include <unordered_set>
#include <vector>

#include "typedefs.h"

class CChar;

class CRace {
  private:
    struct ColourPair_st {
        COLOUR cMin;
        COLOUR cMax;
        ColourPair_st(COLOUR a, COLOUR b) : cMin(a), cMax(b) {}
        ColourPair_st() : cMin(0), cMax(0) {}
    };

    typedef std::vector<ColourPair_st> COLOURLIST;
    typedef std::vector<RaceRelate> RACEIDLIST;
    typedef std::unordered_set<std::uint16_t> ALLOWEQUIPLIST;
    typedef std::unordered_set<std::uint16_t> BANEQUIPLIST;

    std::int16_t HPMod;
    std::int16_t ManaMod;
    std::int16_t StamMod;

  private:
    std::array<SKILLVAL, SKILLS> iSkills;
    // SKILLVAL			iSkills[SKILLS];
    std::string raceName;

    std::bitset<8> bools;
    RANGE visDistance;
    LIGHTLEVEL nightVision;
    ARMORCLASS armourRestrict;
    LIGHTLEVEL lightLevel;
    GENDER restrictGender;
    SKILLVAL languageMin;
    R32 poisonResistance; // % of poison to cancel
    R32 magicResistance;  // % of magic to cancel

    COLOURLIST beardColours;
    COLOURLIST hairColours;
    COLOURLIST skinColours;
    COLOUR bloodColour;

    RACEIDLIST racialEnemies;

    COLDLEVEL coldLevel;
    HEATLEVEL heatLevel;

    ALLOWEQUIPLIST allowedEquipment;
    BANEQUIPLIST bannedEquipment;

    std::bitset<WEATHNUM> weatherAffected;
    std::array<SECONDS, WEATHNUM> weathSecs;
    // SECONDS					weathSecs[WEATHNUM];
    std::array<std::int8_t, WEATHNUM> weathDamage;
    // std::int8_t					weathDamage[WEATHNUM];

    bool doesHunger;
    bool doesThirst;
    std::uint16_t hungerRate;
    std::uint16_t thirstRate;
    std::int16_t hungerDamage;
    std::int16_t thirstDrain;

  public:
    CRace();
    CRace(std::int32_t numRaces);
    ~CRace();

    SKILLVAL Skill(std::int32_t skillNum) const;
    const std::string Name(void) const;
    bool RequiresBeard(void) const;
    bool NoBeard(void) const;
    bool IsPlayerRace(void) const;
    bool AffectedBy(WeatherType iNum) const;
    void AffectedBy(bool value, WeatherType iNum);
    bool NoHair(void) const;
    bool CanEquipItem(std::uint16_t itemId) const;

    GENDER GenderRestriction(void) const;
    LIGHTLEVEL LightLevel(void) const;
    COLDLEVEL ColdLevel(void) const;
    HEATLEVEL HeatLevel(void) const;
    LIGHTLEVEL NightVision(void) const;
    ARMORCLASS ArmourClassRestriction(void) const;
    SECONDS WeatherSeconds(WeatherType iNum) const;
    std::int8_t WeatherDamage(WeatherType iNum) const;
    R32 MagicResistance(void) const;
    R32 PoisonResistance(void) const;

    SKILLVAL LanguageMin(void) const;
    RANGE VisibilityRange(void) const;
    RaceRelate RaceRelation(RACEID race) const;

    std::uint16_t GetHungerRate(void) const;
    std::uint16_t GetThirstRate(void) const;
    void SetHungerRate(std::uint16_t newValue);
    void SetThirstRate(std::uint16_t newValue);
    std::int16_t GetHungerDamage(void) const;
    std::int16_t GetThirstDrain(void) const;
    void SetHungerDamage(std::int16_t newValue);
    void SetThirstDrain(std::int16_t newValue);
    bool DoesHunger(void) const;
    bool DoesThirst(void) const;
    void DoesHunger(bool newValue);
    void DoesThirst(bool newValue);

    void Skill(SKILLVAL newValue, std::int32_t iNum);
    void Name(const std::string &newName);
    void RequiresBeard(bool newValue);
    void NoBeard(bool newValue);
    void IsPlayerRace(bool newValue);
    void NoHair(bool newValue);
    void RestrictGear(bool newValue);

    std::int16_t HPModifier(void) const;
    void HPModifier(std::int16_t value);
    std::int16_t ManaModifier(void) const;
    void ManaModifier(std::int16_t value);
    std::int16_t StamModifier(void) const;
    void StamModifier(std::int16_t value);

    void MagicResistance(R32 value);
    void PoisonResistance(R32 value);

    void GenderRestriction(GENDER newValue);
    void LightLevel(LIGHTLEVEL newValue);
    void ColdLevel(COLDLEVEL newValue);
    void HeatLevel(HEATLEVEL newValue);
    void NightVision(LIGHTLEVEL newValue);
    void ArmourClassRestriction(ARMORCLASS newValue);
    void WeatherSeconds(SECONDS newValue, WeatherType iNum);
    void WeatherDamage(std::int8_t newValue, WeatherType iNum);
    void LanguageMin(SKILLVAL newValue);
    void VisibilityRange(RANGE newValue);

    void NumEnemyRaces(std::int32_t iNum);
    void RaceRelation(RaceRelate value, RACEID race);

    COLOUR RandomSkin(void) const;
    COLOUR RandomHair(void) const;
    COLOUR RandomBeard(void) const;
    COLOUR BloodColour(void) const;
    void BloodColour(COLOUR newValue);

    bool IsSkinRestricted(void) const;
    bool IsHairRestricted(void) const;
    bool IsBeardRestricted(void) const;

    bool IsValidSkin(COLOUR val) const;
    bool IsValidHair(COLOUR val) const;
    bool IsValidBeard(COLOUR val) const;

    void Load(size_t sectNum, std::int32_t modCount);
    CRace &operator=(CRace &trgRace);
};

class cRaces {
  private:
    struct CombatModifiers_st {
        std::uint8_t value;
    };
    typedef std::vector<CRace *> RACELIST;
    typedef std::vector<CombatModifiers_st> MODIFIERLIST;
    // data
    RACELIST races;
    MODIFIERLIST combat;

    void DefaultInitCombat(void);

    // functions - accessors

    // Mutators
    void RacialInfo(RACEID race, RACEID toSet, RaceRelate value);

    bool InvalidRace(RACEID x) const;
    bool initialized;

  public:
    // Functions
    ~cRaces();
    cRaces();
    void Load(void); // loads races
    RaceRelate Compare(CChar *player1, CChar *player2) const;
    RaceRelate CompareByRace(RACEID race1, RACEID race2) const; // compares chars
    void ApplyRace(CChar *s, RACEID x, bool always = false);    // Race Gate
    bool BeardInRange(COLOUR color, RACEID x) const;
    bool SkinInRange(COLOUR color, RACEID x) const;
    bool HairInRange(COLOUR color, RACEID x) const;
    bool BeardRestricted(RACEID x) const;
    bool HairRestricted(RACEID x) const;
    bool SkinRestricted(RACEID x) const;

    CRace *Race(RACEID x);

    // Accessors
    SECONDS Secs(RACEID race, WeatherType element) const;
    std::int8_t Damage(RACEID race, WeatherType element) const;
    bool Affect(RACEID race, WeatherType element) const;
    const std::string Name(RACEID race) const; // Returns race name of player
    SKILLVAL Skill(std::int32_t skill, RACEID race) const;
    GENDER GenderRestrict(RACEID race) const;
    bool RequireBeard(RACEID race) const;
    bool IsPlayerRace(RACEID race) const;
    LIGHTLEVEL LightLevel(RACEID race) const;
    COLDLEVEL ColdLevel(RACEID race) const;
    HEATLEVEL HeatLevel(RACEID race) const;
    bool DoesHunger(RACEID race) const;
    bool DoesThirst(RACEID race) const;
    std::uint16_t GetHungerRate(RACEID race) const;
    std::uint16_t GetThirstRate(RACEID race) const;
    std::int16_t GetHungerDamage(RACEID race) const;
    std::int16_t GetThirstDrain(RACEID race) const;
    ARMORCLASS ArmorRestrict(RACEID race) const;
    COLOUR RandomSkin(RACEID x) const;
    COLOUR RandomHair(RACEID x) const;
    COLOUR RandomBeard(RACEID x) const;
    COLOUR BloodColour(RACEID x) const;
    std::int32_t DamageFromSkill(std::int32_t skill, RACEID x) const;
    std::int32_t FightPercent(std::int32_t skill, RACEID x) const;
    SKILLVAL LanguageMin(RACEID x) const;
    LIGHTLEVEL VisLevel(RACEID x) const;
    RANGE VisRange(RACEID x) const;
    bool NoBeard(RACEID x) const;

    // Mutators
    void Secs(RACEID race, WeatherType element, SECONDS value);
    void Damage(RACEID race, WeatherType element, std::int8_t value);
    void Affect(RACEID race, WeatherType element, bool value);
    void Skill(std::int32_t skill, std::int32_t value, RACEID race);
    void GenderRestrict(GENDER gender, RACEID race);
    void RequireBeard(bool value, RACEID race);
    void NoBeard(bool value, RACEID race);
    void LightLevel(RACEID race, LIGHTLEVEL value);
    void ColdLevel(RACEID race, COLDLEVEL value);
    void HeatLevel(RACEID race, HEATLEVEL value);
    void DoesHunger(RACEID race, bool value);
    void DoesThirst(RACEID race, bool value);
    void SetHungerRate(RACEID race, std::uint16_t value);
    void SetThirstRate(RACEID race, std::uint16_t value);
    void SetHungerDamage(RACEID race, std::int16_t value);
    void SetThirstDrain(RACEID race, std::int16_t value);
    void ArmorRestrict(RACEID race, ARMORCLASS value);
    void RacialEnemy(RACEID race, RACEID enemy);
    void RacialAlly(RACEID race, RACEID ally);
    void RacialNeutral(RACEID race, RACEID neutral);
    void LanguageMin(SKILLVAL toSetTo, RACEID race);
    void BloodColour(RACEID race, COLOUR newValue);

    void VisLevel(RACEID x, LIGHTLEVEL bonus);
    void VisRange(RACEID x, RANGE range);
    void IsPlayerRace(RACEID x, bool value);

    void DebugPrint(RACEID race);
    void DebugPrintAll(void);

    size_t Count(void) const;
};

extern cRaces *Races;

#endif
