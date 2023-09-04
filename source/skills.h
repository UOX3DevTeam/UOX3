#ifndef __SKILLS_H__
#define __SKILLS_H__

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "typedefs.h"

class CChar;
class CItem;
class CSocket;

// create DFN revisions
struct ResAmountPair_st {
    std::vector<std::uint16_t> idList;
    std::uint8_t amountNeeded;
    std::uint16_t colour;
    std::uint32_t moreVal;
    ResAmountPair_st() : amountNeeded(1), colour(0), moreVal(0) { idList.resize(0); }
    ~ResAmountPair_st() { idList.resize(0); }
};

struct ResSkillReq_st {
    std::uint8_t skillNumber;
    std::uint16_t minSkill;
    std::uint16_t maxSkill;
    ResSkillReq_st() : skillNumber(0), minSkill(0), maxSkill(0) {}
};

struct CreateEntry_st {
    std::uint16_t colour;
    std::uint16_t targId;
    std::uint16_t soundPlayed;
    std::uint8_t minRank;
    std::uint8_t maxRank;
    std::string addItem;
    std::int16_t delay;
    std::uint16_t spell;
    std::vector<ResAmountPair_st> resourceNeeded;
    std::vector<ResSkillReq_st> skillReqs;
    std::string name;
    CreateEntry_st()
    : colour(0), targId(0), soundPlayed(0), minRank(1), maxRank(10), addItem(""), delay(0),
    spell(0), name("") {
        resourceNeeded.resize(0);
        skillReqs.resize(0);
    }
    ~CreateEntry_st() {
        resourceNeeded.resize(0);
        skillReqs.resize(0);
    }
    R32 AverageMinSkill(void) {
        R32 sum = 0;
        for (size_t i = 0; i < skillReqs.size(); ++i) {
            sum += skillReqs[i].minSkill;
        }
        return sum / skillReqs.size();
    }
    R32 AverageMaxSkill(void) {
        R32 sum = 0;
        for (size_t i = 0; i < skillReqs.size(); ++i) {
            sum += skillReqs[i].maxSkill;
        }
        return sum / skillReqs.size();
    }
};

struct MiningData_st {
    std::string oreName; // ore name from ORE_LIST in skills.dfn
    std::uint16_t colour;         // colour of the ore, for colour of ingot
    std::uint16_t minSkill;       // minimum skill needed to make the ingot
    std::string name;    // name of the ingot: no need to be fixed, as we're loading it dynamically
    std::int32_t makemenu;       // the makemenu required for making with
    std::uint16_t oreChance;      // default chance of finding ore type if nothing else is specified
    std::uint16_t scriptID;       // scriptID assigned to ore items
    
    MiningData_st()
    : oreName(""), colour(0), minSkill(0), name(""), makemenu(0), oreChance(0), scriptID(0) {}
};

class CSkills {
private:
    struct CreateMenu_st {
        std::vector<std::uint16_t> itemEntries;
        std::vector<std::uint16_t> menuEntries;
        
        std::vector<std::uint16_t>::iterator iIter;
        std::vector<std::uint16_t>::iterator mIter;
        
        CreateMenu_st() {
            itemEntries.resize(0);
            menuEntries.resize(0);
            
            iIter = itemEntries.end();
            mIter = menuEntries.end();
        }
        ~CreateMenu_st() {
            itemEntries.resize(0);
            menuEntries.resize(0);
        }
    };
    struct CreateMenuEntry_st {
        std::uint16_t targId;
        std::uint16_t colour;
        std::string name;
        std::uint16_t subMenu;
        CreateMenuEntry_st() : targId(0), colour(0), name(""), subMenu(0) {}
    };
    
    std::vector<MiningData_st> ores;
    std::map<std::uint16_t, CreateMenu_st> actualMenus;
    std::map<std::uint16_t, CreateMenuEntry_st> skillMenus;
    std::map<std::uint16_t, CreateEntry_st> itemsForMenus;
    
private:
    void RegenerateOre(std::int16_t grX, std::int16_t grY, std::uint8_t worldNum);
    void DoStealing(CSocket *s, CChar *mChar, CChar *npc, CItem *item);
    std::int16_t CalcStealDiff(CChar *c, CItem *i);
    
    TargetFunc RandomSteal;
    
    std::int8_t FindSkillPoint(std::uint8_t sk, std::int32_t value);
    void AnvilTarget(CSocket *s, CItem &item, MiningData_st *oreType);
    void HandleSkillChange(CChar *c, std::uint8_t sk, std::int8_t skillAdvance, bool success);
    
    bool LoadMiningData(void);
    void LoadCreateMenus(void);
    bool AdvanceSkill(CChar *s, std::uint8_t sk, bool skillused);
    
public:
    CSkills();
    ~CSkills();
    
    std::int32_t CalcRankAvg(CChar *player, CreateEntry_st &skillMake);
    
    TargetFunc Persecute;
    TargetFunc RepairMetal;
    TargetFunc SmeltOre;
    TargetFunc Smith;
    TargetFunc StealingTarget;
    
    void Load(void);
    
    void NewMakeMenu(CSocket *s, std::int32_t menu, std::uint8_t skill);
    CreateEntry_st *FindItem(std::uint16_t itemNum);
    void MakeItem(CreateEntry_st &toMake, CChar *player, CSocket *sock, std::uint16_t itemEntry,
                  std::uint16_t resourceColour = 0);
    void ApplyRank(CSocket *s, CItem *c, std::uint8_t rank, std::uint8_t maxrank);
    void HandleMakeMenu(CSocket *s, std::int32_t button, std::int32_t menu);
    
    void CreateTrackingMenu(CSocket *s, std::uint16_t m);
    void TrackingMenu(CSocket *s, std::uint16_t gmindex);
    void Track(CChar *i);
    void Tracking(CSocket *s, std::int32_t selection);
    void MakeNecroReg(CSocket *nSocket, CItem *nItem, std::uint16_t itemId);
    
    void Snooping(CSocket *s, CChar *target, CItem *pack);
    
    std::uint16_t CalculatePetControlChance(CChar *mChar, CChar *Npc);
    bool CheckSkill(CChar *s, std::uint8_t sk, std::int16_t lowSkill, std::int16_t highSkill, bool isCraftSkill = false);
    void SkillUse(CSocket *s, std::uint8_t x);
    void UpdateSkillLevel(CChar *c, std::uint8_t s) const;
    void AdvanceStats(CChar *s, std::uint8_t sk, bool skillsuccess);
    
    size_t GetNumberOfOres(void);
    MiningData_st *GetOre(size_t number);
    MiningData_st *FindOre(std::string const &name);
    MiningData_st *FindOre(std::uint16_t const &colour);
};

extern CSkills *Skills;

#endif
