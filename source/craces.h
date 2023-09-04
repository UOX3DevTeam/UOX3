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
    struct ColourPair {
        colour_t cMin;
        colour_t cMax;
        ColourPair(colour_t a, colour_t b) : cMin(a), cMax(b) {}
        ColourPair() : cMin(0), cMax(0) {}
    };
    
    typedef std::vector<ColourPair> COLOURLIST;
    typedef std::vector<RaceRelate> RACEIDLIST;
    typedef std::unordered_set<std::uint16_t> ALLOWEQUIPLIST;
    typedef std::unordered_set<std::uint16_t> BANEQUIPLIST;
    
    std::int16_t HPMod;
    std::int16_t ManaMod;
    std::int16_t StamMod;
    
private:
    std::array<skillval_t, SKILLS> iSkills;
    // SKILLVAL			iSkills[SKILLS];
    std::string raceName;
    
    std::bitset<8> bools;
    range_t visDistance;
    lightlevel_t nightVision;
    armorclass_t armourRestrict;
    lightlevel_t lightLevel;
    gender_t restrictGender;
    skillval_t languageMin;
    R32 poisonResistance; // % of poison to cancel
    R32 magicResistance;  // % of magic to cancel
    
    COLOURLIST beardColours;
    COLOURLIST hairColours;
    COLOURLIST skinColours;
    colour_t bloodColour;
    
    RACEIDLIST racialEnemies;
    
    coldlevel_t coldLevel;
    heatlevel_t heatLevel;
    
    ALLOWEQUIPLIST allowedEquipment;
    BANEQUIPLIST bannedEquipment;
    
    std::bitset<WEATHNUM> weatherAffected;
    std::array<seconds_t, WEATHNUM> weathSecs;
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
    
    skillval_t Skill(std::int32_t skillNum) const;
    const std::string Name() const;
    bool RequiresBeard() const;
    bool NoBeard() const;
    bool IsPlayerRace() const;
    bool AffectedBy(weathertype_t iNum) const;
    void AffectedBy(bool value, weathertype_t iNum);
    bool NoHair() const;
    bool CanEquipItem(std::uint16_t itemId) const;
    
    gender_t GenderRestriction() const;
    lightlevel_t LightLevel() const;
    coldlevel_t ColdLevel() const;
    heatlevel_t HeatLevel() const;
    lightlevel_t NightVision() const;
    armorclass_t ArmourClassRestriction() const;
    seconds_t WeatherSeconds(weathertype_t iNum) const;
    std::int8_t WeatherDamage(weathertype_t iNum) const;
    R32 MagicResistance() const;
    R32 PoisonResistance() const;
    
    skillval_t LanguageMin() const;
    range_t VisibilityRange() const;
    RaceRelate RaceRelation(raceid_t race) const;
    
    std::uint16_t GetHungerRate() const;
    std::uint16_t GetThirstRate() const;
    void SetHungerRate(std::uint16_t newValue);
    void SetThirstRate(std::uint16_t newValue);
    std::int16_t GetHungerDamage() const;
    std::int16_t GetThirstDrain() const;
    void SetHungerDamage(std::int16_t newValue);
    void SetThirstDrain(std::int16_t newValue);
    bool DoesHunger() const;
    bool DoesThirst() const;
    void DoesHunger(bool newValue);
    void DoesThirst(bool newValue);
    
    void Skill(skillval_t newValue, std::int32_t iNum);
    void Name(const std::string &newName);
    void RequiresBeard(bool newValue);
    void NoBeard(bool newValue);
    void IsPlayerRace(bool newValue);
    void NoHair(bool newValue);
    void RestrictGear(bool newValue);
    
    std::int16_t HPModifier() const;
    void HPModifier(std::int16_t value);
    std::int16_t ManaModifier() const;
    void ManaModifier(std::int16_t value);
    std::int16_t StamModifier() const;
    void StamModifier(std::int16_t value);
    
    void MagicResistance(R32 value);
    void PoisonResistance(R32 value);
    
    void GenderRestriction(gender_t newValue);
    void LightLevel(lightlevel_t newValue);
    void ColdLevel(coldlevel_t newValue);
    void HeatLevel(heatlevel_t newValue);
    void NightVision(lightlevel_t newValue);
    void ArmourClassRestriction(armorclass_t newValue);
    void WeatherSeconds(seconds_t newValue, weathertype_t iNum);
    void WeatherDamage(std::int8_t newValue, weathertype_t iNum);
    void LanguageMin(skillval_t newValue);
    void VisibilityRange(range_t newValue);
    
    void NumEnemyRaces(std::int32_t iNum);
    void RaceRelation(RaceRelate value, raceid_t race);
    
    colour_t RandomSkin() const;
    colour_t RandomHair() const;
    colour_t RandomBeard() const;
    colour_t BloodColour() const;
    void BloodColour(colour_t newValue);
    
    bool IsSkinRestricted() const;
    bool IsHairRestricted() const;
    bool IsBeardRestricted() const;
    
    bool IsValidSkin(colour_t val) const;
    bool IsValidHair(colour_t val) const;
    bool IsValidBeard(colour_t val) const;
    
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
    
    void DefaultInitCombat();
    
    // functions - accessors
    
    // Mutators
    void RacialInfo(raceid_t race, raceid_t toSet, RaceRelate value);
    
    bool InvalidRace(raceid_t x) const;
    bool initialized;
    
public:
    // Functions
    ~cRaces();
    cRaces();
    void Load(); // loads races
    RaceRelate Compare(CChar *player1, CChar *player2) const;
    RaceRelate CompareByRace(raceid_t race1, raceid_t race2) const; // compares chars
    void ApplyRace(CChar *s, raceid_t x, bool always = false);    // Race Gate
    bool BeardInRange(colour_t color, raceid_t x) const;
    bool SkinInRange(colour_t color, raceid_t x) const;
    bool HairInRange(colour_t color, raceid_t x) const;
    bool BeardRestricted(raceid_t x) const;
    bool HairRestricted(raceid_t x) const;
    bool SkinRestricted(raceid_t x) const;
    
    CRace *Race(raceid_t x);
    
    // Accessors
    seconds_t Secs(raceid_t race, weathertype_t element) const;
    std::int8_t Damage(raceid_t race, weathertype_t element) const;
    bool Affect(raceid_t race, weathertype_t element) const;
    const std::string Name(raceid_t race) const; // Returns race name of player
    skillval_t Skill(std::int32_t skill, raceid_t race) const;
    gender_t GenderRestrict(raceid_t race) const;
    bool RequireBeard(raceid_t race) const;
    bool IsPlayerRace(raceid_t race) const;
    lightlevel_t LightLevel(raceid_t race) const;
    coldlevel_t ColdLevel(raceid_t race) const;
    heatlevel_t HeatLevel(raceid_t race) const;
    bool DoesHunger(raceid_t race) const;
    bool DoesThirst(raceid_t race) const;
    std::uint16_t GetHungerRate(raceid_t race) const;
    std::uint16_t GetThirstRate(raceid_t race) const;
    std::int16_t GetHungerDamage(raceid_t race) const;
    std::int16_t GetThirstDrain(raceid_t race) const;
    armorclass_t ArmorRestrict(raceid_t race) const;
    colour_t RandomSkin(raceid_t x) const;
    colour_t RandomHair(raceid_t x) const;
    colour_t RandomBeard(raceid_t x) const;
    colour_t BloodColour(raceid_t x) const;
    std::int32_t DamageFromSkill(std::int32_t skill, raceid_t x) const;
    std::int32_t FightPercent(std::int32_t skill, raceid_t x) const;
    skillval_t LanguageMin(raceid_t x) const;
    lightlevel_t VisLevel(raceid_t x) const;
    range_t VisRange(raceid_t x) const;
    bool NoBeard(raceid_t x) const;
    
    // Mutators
    void Secs(raceid_t race, weathertype_t element, seconds_t value);
    void Damage(raceid_t race, weathertype_t element, std::int8_t value);
    void Affect(raceid_t race, weathertype_t element, bool value);
    void Skill(std::int32_t skill, std::int32_t value, raceid_t race);
    void GenderRestrict(gender_t gender, raceid_t race);
    void RequireBeard(bool value, raceid_t race);
    void NoBeard(bool value, raceid_t race);
    void LightLevel(raceid_t race, lightlevel_t value);
    void ColdLevel(raceid_t race, coldlevel_t value);
    void HeatLevel(raceid_t race, heatlevel_t value);
    void DoesHunger(raceid_t race, bool value);
    void DoesThirst(raceid_t race, bool value);
    void SetHungerRate(raceid_t race, std::uint16_t value);
    void SetThirstRate(raceid_t race, std::uint16_t value);
    void SetHungerDamage(raceid_t race, std::int16_t value);
    void SetThirstDrain(raceid_t race, std::int16_t value);
    void ArmorRestrict(raceid_t race, armorclass_t value);
    void RacialEnemy(raceid_t race, raceid_t enemy);
    void RacialAlly(raceid_t race, raceid_t ally);
    void RacialNeutral(raceid_t race, raceid_t neutral);
    void LanguageMin(skillval_t toSetTo, raceid_t race);
    void BloodColour(raceid_t race, colour_t newValue);
    
    void VisLevel(raceid_t x, lightlevel_t bonus);
    void VisRange(raceid_t x, range_t range);
    void IsPlayerRace(raceid_t x, bool value);
    
    void DebugPrint(raceid_t race);
    void DebugPrintAll();
    
    size_t Count() const;
};

extern cRaces *Races;

#endif
