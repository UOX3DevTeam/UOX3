#include "skills.h"

#include <algorithm>

#include "cchar.h"
#include "ceffects.h"
#include "citem.h"
#include "cjsmapping.h"
#include "classes.h"
#include "cmagic.h"
#include "cmultiobj.h"
#include "combat.h"
#include "subsystem/console.hpp"
#include "cpacketsend.h"
#include "craces.h"
#include "cscript.h"
#include "cserverdefinitions.h"
#include "csocket.h"
#include "dictionary.h"
#include "funcdecl.h"
#include "magic.h"
#include "movement.h"
#include "regions.h"
#include "scriptc.h"
#include "configuration/serverconfig.hpp"
#include "ssection.h"
#include "stringutility.hpp"
#include "utility/strutil.hpp"
#include "townregion.h"

#include "weight.h"

extern CDictionaryContainer worldDictionary ;

using namespace std::string_literals;

bool CheckItemRange(CChar *mChar, CItem *i);

CSkills *Skills = nullptr;

const std::uint16_t CREATE_MENU_OFFSET = 5000; // This is how we differentiate a menu button from an item
// button (and the limit on ITEM=# in create.dfn)

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::CalcRankAvg()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate average rank of item based on player's skillpoints in
// skills |					required to craft item
// o------------------------------------------------------------------------------------------------o
std::int32_t CSkills::CalcRankAvg(CChar *player, CreateEntry_st &skillMake) {
    // If rank system is not enabled, assume perfectly crafted item each time
    if (!ServerConfig::shared().enabled(ServerSwitch::RANKSYSTEM))
        return 10;
    
    float rankSum = 0;
    std::int32_t rk_range, rank;
    float sk_range, randnum, randnum1;
    
    // Calculate base chance of success at crafting based on primary skill, to use as a modifier for
    // exceptional chance later
    float chanceSkillSuccess = 0;
    chanceSkillSuccess = static_cast<float>(player->GetSkill(skillMake.skillReqs[0].skillNumber) - skillMake.skillReqs[0].minSkill);
    chanceSkillSuccess /= (static_cast<float>(skillMake.skillReqs[0].maxSkill) - static_cast<float>(skillMake.skillReqs[0].minSkill));
    
    // Loop through all skills required to craft the item and calculate average rank value for each
    for (size_t i = 0; i < skillMake.skillReqs.size(); ++i) {
        // Fetch the range of rank (min/max) values the item being crafted can have
        rk_range = skillMake.maxRank - skillMake.minRank;
        
        // Fetch range of skill we have to work with for this skill based on player's skill value
        // and minimum skill required to craft the item
        sk_range = static_cast<float>(player->GetSkill(skillMake.skillReqs[i].skillNumber) - skillMake.skillReqs[i].minSkill);
        if (sk_range <= 0) {
            sk_range = skillMake.minRank * 10;
        }
        else if (sk_range >= 1000) {
            sk_range = skillMake.maxRank * 10;
        }
        
        // If generated number is under skill range value...
        randnum = static_cast<float>(RandomNum(0, 999));
        if (randnum <= sk_range) {
            rank = skillMake.maxRank; // ...assume perfectly crafted item!
        }
        else {
            // ...otherwise, generate a random number modified by skill range and difficulty level
            // of rank system from uox.ini
            randnum1 = static_cast<float>(RandomNum(0, 999)) - ((randnum - sk_range) / (11 - ServerConfig::shared().ushortValues[UShortValue::SKILLLEVEL] ));
            
            // Modify random number based on base chance of success at crafting
            randnum1 *= chanceSkillSuccess;
            
            // Calculate rank value for this skill
            rank = static_cast<std::int32_t>((randnum1 * rk_range) / 1000);
            rank += skillMake.minRank - 1;
            if (rank > skillMake.maxRank) {
                rank = skillMake.maxRank;
            }
            if (rank < skillMake.minRank) {
                rank = skillMake.minRank;
            }
        }
        
        // Add rank value for this skill to the total
        rankSum += rank;
    }
    
    // Return the average rank value of all skills required to craft the item
    return static_cast<std::int32_t>(rankSum / skillMake.skillReqs.size());
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::ApplyRank()
//|	Date		-	24 August 1999
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Modify item properties based on item's rank.
// o------------------------------------------------------------------------------------------------o
void CSkills::ApplyRank(CSocket *s, CItem *c, std::uint8_t rank, std::uint8_t maxrank) {
    char tmpmsg[512];
    *tmpmsg = '\0';
    
    // Only apply rank system if enabled in ini, and only for non-pileable items!
    if (ServerConfig::shared().enabled(ServerSwitch::RANKSYSTEM) && !c->IsPileable()) {
        c->SetRank(rank);
        
        if (c->GetLoDamage() > 0) {
            c->SetLoDamage(static_cast<std::int16_t>((rank * c->GetLoDamage()) / 10));
        }
        if (c->GetHiDamage() > 0) {
            c->SetHiDamage(static_cast<std::int16_t>((rank * c->GetHiDamage()) / 10));
        }
        if (c->GetResist(Weather::PHYSICAL) > 0) {
            c->SetResist(static_cast<std::uint16_t>((rank * c->GetResist(Weather::PHYSICAL)) / 10), Weather::PHYSICAL);
        }
        if (c->GetHP() > 0) {
            c->SetHP(static_cast<std::int16_t>((rank * c->GetHP()) / 10));
        }
        if (c->GetMaxHP() > 0) {
            c->SetMaxHP(static_cast<std::int16_t>((rank * c->GetMaxHP()) / 10));
        }
        if (c->GetBuyValue() > 0) {
            c->SetBuyValue(static_cast<std::uint32_t>((rank * c->GetBuyValue()) / 10));
        }
        if (c->GetMaxUses() > 0) {
            c->SetUsesLeft(static_cast<std::uint16_t>((rank * c->GetMaxUses()) / 10));
        }
        if (c->GetId() == 0x22c5 && c->GetMaxHP() > 0) // Runebook
        {
            // Max charges for runebook stored in maxHP property, defaults to 10, ranges from 5-10
            // based on rank
            c->SetMaxHP(static_cast<std::uint16_t>(std::max(static_cast<std::uint16_t>(5), static_cast<std::uint16_t>((rank * c->GetMaxHP()) / 10))));
        }
        
        // Convert item's rank to a value between 1 and 10, to fit rank system messages
        std::uint8_t tempRank = floor(static_cast<float>(((rank * 100) / maxrank) / 10));
        
        if (tempRank >= 1 && tempRank <= 10) {
            s->SysMessage(783 + tempRank); // =You cannot use that material for carpentry.
        }
        else if (tempRank < 1) {
            s->SysMessage(784); // You made an item with no quality!
        }
    }
    else {
        c->SetRank(rank);
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::RegenerateOre()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Regenerate Ore in a resource region based on UOX.INI Ore respawn
// settings
// o------------------------------------------------------------------------------------------------o
void CSkills::RegenerateOre(std::int16_t grX, std::int16_t grY, std::uint8_t worldNum) {
    MapResource_st *orePart = MapRegion->GetResource(grX, grY, worldNum);
    std::int16_t oreCeiling = ServerConfig::shared().ushortValues[UShortValue::OREPERAREA] ;
    std::uint16_t oreTimer = ServerConfig::shared().timerSetting[TimerSetting::ORE] ;
    if (static_cast<std::uint32_t>(orePart->oreTime) <= worldMain.GetUICurrentTime()) { // regenerate some more?
        for (std::int16_t counter = 0; counter < oreCeiling; ++counter) { // keep regenerating ore
            if (orePart->oreAmt < oreCeiling &&
                static_cast<std::uint32_t>(orePart->oreAmt + counter * oreTimer * 1000) < worldMain.GetUICurrentTime()) {
                ++orePart->oreAmt;
            }
            else {
                break;
            }
        }
        orePart->oreTime = BuildTimeValue(static_cast<float>(oreTimer));
    }
    if (orePart->oreAmt > oreCeiling) {
        orePart->oreAmt = oreCeiling;
    }
}

// o------------------------------------------------------------------------------------------------o
//| Function	-	CSkills::SmeltOre();
//| Modified	-	(February 19, 2000)
// o------------------------------------------------------------------------------------------------o
//| Purpose		-	Rewritten to use case and structure, you'll find it is easier to
//make it
//|					scriptable now. The structure is pretty much all that'd be
//needed for any |					future ore->ingot conversions. Scripting the
//ore would probably be even |					simpler, requires less info
// o------------------------------------------------------------------------------------------------o
void CSkills::SmeltOre(CSocket *s) {
    VALIDATESOCKET(s);
    CChar *chr = s->CurrcharObj();
    CItem *itemToSmelt = static_cast<CItem *>(s->TempObj());
    s->TempObj(nullptr);
    CItem *forge = CalcItemObjFromSer(s->GetDWord(7));
    
    if (itemToSmelt->IsHeldOnCursor()) {
        s->SysMessage(400); // That is too far away!
        return;
    }
    
    if (ValidateObject(forge)) // if we have a forge
    {
        CItem *playerPack = chr->GetPackItem();
        if (ValidateObject(playerPack)) {
            if (FindRootContainer(itemToSmelt) != playerPack && FindRootContainer(forge) != playerPack) {
                if (!ObjInRange(chr, forge, DIST_NEARBY) || !CheckItemRange(chr, itemToSmelt)) { // Check if forge & item to melt are in range
                    s->SysMessage(400); // That is too far away!
                    return;
                }
            }
        }
        
        std::uint16_t smeltItemId = itemToSmelt->GetId();
        std::uint16_t forgeId = forge->GetId();
        
        // Is player trying to combine ore?
        // if( forgeId == 0x19B7 || forgeId == 0x19B8 || forgeId == 0x19BA )
        if (forgeId >= 0x19B7 && forgeId <= 0x19BA) {
            if (forgeId == smeltItemId) {
                s->SysMessage(1806); // You cannot combine these ore types
                return;
            }
            
            if (forge->GetColour() != itemToSmelt->GetColour()) {
                s->SysMessage(1807); // You cannot combine ores of different metals.
                return;
            }
            
            if (forge->GetMovable() == 2 || forge->GetMovable() == 3) {
                s->SysMessage(1808); // You cannot do that.
                return;
            }
            
            // Player targeted some small or medium ore! Combine ores, if possible
            std::int32_t sourceAmount = itemToSmelt->GetAmount();
            std::int32_t targetAmount = forge->GetAmount();
            std::uint8_t amountMultiplier = 1;
            
            if (smeltItemId == 0x19B9 && forgeId == 0x19B7) { // Large ore, targeted small ore
                amountMultiplier = 4;
            }
            else if (smeltItemId == 0x19B9 && (forgeId == 0x19B8 || forgeId == 0x19BA)) { // Large ore, targeted medium ore
                amountMultiplier = 2;
            }
            else if ((smeltItemId == 0x19B8 || smeltItemId == 0x19BA) && forgeId == 0x19B7) { // Medium ore, targeted small ore
                amountMultiplier = 2;
            }
            else {
                // Small ore, targeted at larger ore
                s->SysMessage(1809); // You can only combine larger ores with smaller ores!
                return;
            }
            
            // Combine source stack into target stack, if it fits
            if (targetAmount + (sourceAmount * amountMultiplier) <= MAX_STACK) {
                CChar *forgeOwner = FindItemOwner(forge);
                
                std::int32_t newTargetWeight =
                (targetAmount + (sourceAmount * amountMultiplier)) * forge->GetBaseWeight();
                std::int32_t subtractWeight = (targetAmount * forge->GetBaseWeight()) + (sourceAmount * itemToSmelt->GetBaseWeight());
                
                if (ValidateObject(forgeOwner) && forgeOwner == chr) {
                    if ((newTargetWeight + chr->GetWeight() - subtractWeight) > 200000) { // 2000 stones
                        s->SysMessage(1743); // That person can not possibly hold more weight
                        return;
                    }
                }
                else if (forge->GetCont() != nullptr) {
                    if ((newTargetWeight + forge->GetCont()->GetWeight() - subtractWeight) > 200000) { // 2000 stones
                        s->SysMessage(1810); // The weight is too great to combine in a container.
                        return;
                    }
                }
                
                // Good to go, combine ores and remove source stack!
                forge->SetAmount(targetAmount + (sourceAmount * amountMultiplier));
                
                // Remove from multi before deleting
                if (itemToSmelt->IsLockedDown() && ValidateObject(itemToSmelt->GetMultiObj())) {
                    itemToSmelt->GetMultiObj()->ReleaseItem(itemToSmelt);
                }
                
                itemToSmelt->Delete();
                s->SysMessage(1811); // You combine the ore with the stack of smaller ore.
            }
            else if (targetAmount < MAX_STACK) {
                // New targetStack would exceed MAX_STACK, but there might still be some room left!
                // Cap amount at MAX_STACK
                std::uint16_t amountLeft = MAX_STACK - targetAmount;
                if (amountLeft < amountMultiplier) {
                    // Not enough space left in stack to combine any more ore
                    s->SysMessage(1812); // That stack cannot hold any more items
                    return;
                }
                else {
                    // There's still some room
                    std::uint16_t amountToRemove = static_cast<std::uint16_t>(floor(amountLeft / amountMultiplier));
                    itemToSmelt->SetAmount(sourceAmount - amountToRemove);
                    forge->SetAmount(MAX_STACK);
                    s->SysMessage(1813); // You combine a portion of your ore into the stack of smaller ore.
                }
            }
            else {
                // targetStack is already at max cap and cannot hold any more ore
                s->SysMessage(1812); // That stack cannot hold any more items
                return;
            }
            
            return;
        }
        
        switch (forgeId) // Check to ensure it is a forge
        {
            case 0x0FB1:
            case 0x197A:
            case 0x19A9:
            case 0x197E:
            case 0x1982:
            case 0x1986:
            case 0x198A:
            case 0x198E:
            case 0x1992:
            case 0x1996:
            case 0x199A:
            case 0x19A6:
            case 0x19A2:
            case 0x199E:
                if (ObjInRange(chr, forge, DIST_NEARBY)) { // Check if the forge is in range
                    std::uint16_t targColour = itemToSmelt->GetColour();
                    MiningData_st *oreType = FindOre(targColour);
                    if (oreType == nullptr) {
                        s->SysMessage(814); // That material is foreign to you.
                        return;
                    }
                    
                    if (chr->GetSkill(MINING) < oreType->minSkill) {
                        s->SysMessage(815); // You have no idea what to do with this strange ore.
                        return;
                    }
                    
                    if (!CheckSkill(chr, MINING, oreType->minSkill, 1000)) // if we do not have minimum skill to use it
                    {
                        if (itemToSmelt->GetAmount() > 1) // If more than 1 ore, half it
                        {
                            s->SysMessage(817); // Your hand slips and some of your materials are destroyed.
                            itemToSmelt->SetAmount(itemToSmelt->GetAmount() / 2);
                        }
                        else {
                            s->SysMessage(816); // Your hand slips and the last of your materials are destroyed.
                            // Remove from multi before deleting
                            if (itemToSmelt->IsLockedDown() && ValidateObject(itemToSmelt->GetMultiObj())) {
                                itemToSmelt->GetMultiObj()->ReleaseItem(itemToSmelt);
                            }
                            
                            itemToSmelt->Delete();
                        }
                        return;
                    }
                    
                    std::uint16_t ingotNum = 1;
                    switch (smeltItemId) {
                        case 0x19B7:                                 // Small ore
                            ingotNum = itemToSmelt->GetAmount() / 2; // 0.5 ingot per ore
                            break;
                        case 0x19B8:                             // Medium ore
                        case 0x19BA:                             // Medium ore
                            ingotNum = itemToSmelt->GetAmount(); // 1.0 ingot per ore
                            break;
                        case 0x19B9:                                 // Large ore
                            ingotNum = itemToSmelt->GetAmount() * 2; // 2.0 ingot per ore
                            break;
                    }
                    
                    CItem *ingot = Items->CreateScriptItem(s, chr, "0x1BF2", ingotNum, CBaseObject::OT_ITEM, true, oreType->colour);
                    if (ingot != nullptr) {
                        ingot->SetName(util::format("%s Ingot", oreType->name.c_str()));
                    }
                    
                    if (smeltItemId == 0x19B7 && itemToSmelt->GetAmount() % 2 != 0) {
                        itemToSmelt->SetAmount(1); // There's still some ore left, as the pile of small
                        // ore is not divisible by two
                    }
                    else {
                        // Remove from multi before deleting
                        if (itemToSmelt->IsLockedDown() && ValidateObject(itemToSmelt->GetMultiObj())) {
                            itemToSmelt->GetMultiObj()->ReleaseItem(itemToSmelt);
                        }
                        
                        itemToSmelt->Delete(); // delete the ore
                    }
                    
                    s->SysMessage(818);                        // You have smelted your ore.
                    s->SysMessage(819, oreType->name.c_str()); // You place some %s ingots in your pack.
                }
                break;
            default:
                s->SysMessage(820); // You can't smelt here.
                break;
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::CalculatePetControlChance()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate a player's chance (returned as value between 0 to 1000) of
// controlling |					(having commands accepted) a follower
// o------------------------------------------------------------------------------------------------o
std::uint16_t CSkills::CalculatePetControlChance(CChar *mChar, CChar *Npc) {
    std::int16_t petOrneriness = static_cast<std::int16_t>(Npc->GetOrneriness());
    
    // If difficulty to control is below a certain treshold (29.1) or it's a summoned creature, 100%
    // chance to control creature
    if ((petOrneriness == 0 && Npc->GetTaming() <= 291) || petOrneriness <= 291 || Npc->IsDispellable())
        return static_cast<std::int16_t>(1000);
    
    // TODO: Check if NPC can be tamed via necromancer Dark Wolf Familiar
    // if( petOrneriness > 249 && CheckFamiliarMastery( mChar, Npc ))
    // toTame = 249;
    
    // Fetch player's skill values (with modifiers)
    std::int16_t tamingSkill = static_cast<std::int16_t>(mChar->GetSkill(TAMING));
    std::int16_t loreSkill = static_cast<std::int16_t>(mChar->GetSkill(ANIMALLORE));
    
    std::int16_t bonusChance = 0;
    std::int16_t totalChance = 700; // base chance of success
    std::int16_t tamingSkillMod = 6;
    std::int16_t animalLoreMod = 6;
    
    // Base bonuses based on skill vs orneriness
    std::int16_t tamingSkillBonus = static_cast<std::int16_t>(tamingSkill - petOrneriness);
    std::int16_t animalLoreBonus = static_cast<std::int16_t>(loreSkill - petOrneriness);
    
    // Calculated bonus modifiers
    if (tamingSkillBonus < 0) {
        tamingSkillMod = 28;
    }
    if (animalLoreBonus < 0) {
        animalLoreMod = 14;
    }
    
    tamingSkillBonus *= tamingSkillMod;
    animalLoreBonus *= animalLoreMod;
    
    // Bonus chance equals average of bonuses for tamingskill and animallore
    bonusChance = static_cast<std::int16_t>((tamingSkillBonus + animalLoreBonus) / 2);
    totalChance += bonusChance;
    
    // Modify chance based on NPCs loyalty (or lack thereof)
    totalChance -= (Npc->GetMaxLoyalty() - Npc->GetLoyalty()) * 10;
    
    // Finally, apply a 15% penalty to chance for friends of the pet trying to control
    if (Npcs->CheckPetFriend(mChar, Npc)) {
        totalChance -= static_cast<std::int16_t>(totalChance * 0.15);
    }
    
    // Clamp lower chance to 20% and upper chance to 99%
    totalChance = std::clamp(totalChance, static_cast<std::int16_t>(200), static_cast<std::int16_t>(990));
    
    return static_cast<std::uint16_t>(totalChance);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::CheckSkill()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Used to check a players skill based on the highskill and lowskill it
// was called
//|					with.  If skill is < than lowskill check will fail, but player
//will gain in the
//|					skill, if the players skill is > than highskill player will
//not gain
// o------------------------------------------------------------------------------------------------o
bool CSkills::CheckSkill(CChar *s, std::uint8_t sk, std::int16_t lowSkill, std::int16_t highSkill, bool isCraftSkill) {
    bool skillCheck = false;
    bool exists = false;
    std::vector<std::uint16_t> scriptTriggers = s->GetScriptTriggers();
    for (auto scriptTrig : scriptTriggers) {
        cScript *toExecute = JSMapping->GetScript(scriptTrig);
        if (toExecute != nullptr) {
            // If script returns true/1, allows skillcheck to proceed, but also prevents other
            // scripts with event from running
            if (toExecute->OnSkillCheck(s, sk, lowSkill, highSkill, isCraftSkill) == 1) {
                exists = true;
                break;
            }
        }
    }
    
    // o----------------------------------------------------------------------------o
    // | 15 March, 2002
    // | | Comment   :Now lets make this more readable, and even more mathematical:)	|
    // o----------------------------------------------------------------------------o
    // | 1. if a player has skill > highskill, checkskill should return true		|
    // | 2. if a player has skill < lowskill, checkskill returns false				|
    // | 3. we have statmodifiers defined in skills.dfn, now lets make it influence	|
    // |	checkskill with a certain additional bonus through the players stat.	|
    // | 4. we dont need to throw the dices for skillgain... we must do this in		|
    // |	AdvanceSkill(...), so just call it after calculation
    // | o----------------------------------------------------------------------------o | how to
    // calculate if a skill succeeds?
    // | | 1. we have a certain range between lowskill and highskill, on lowskill the	| |
    // chance is 0%, on highskill 100%.
    // | | 2. get the statmodifiers and the players stat. make 3 special dices for	| | each
    // stat. make them a small bonus for skillsuccess						|
    // | 3. execute the 4 calculated dices one after the other by adding them		|
    // o----------------------------------------------------------------------------o
    
    if (!exists) {
        CSocket *mSock = s->GetSocket();
        float chanceSkillSuccess = 0;
        
        if ((highSkill - lowSkill) <= 0 || !ValidateObject(s) || s->GetSkill(sk) < lowSkill)
            return false;
        
        if (s->IsDead() && mSock != nullptr) {
            mSock->SysMessage(1487); // You are dead and cannot gain skill.
            return false;
        }
        
        if (s->GetSkill(sk) >= highSkill)
            return true;
        
        // Calculate base chance of success at using a skill
        chanceSkillSuccess = (static_cast<float>(s->GetSkill(sk)) - static_cast<float>(lowSkill));
        
        // Modify chance based on the range of highSkill vs lowSkill
        chanceSkillSuccess /= (static_cast<float>(highSkill) - static_cast<float>(lowSkill));
        
        // Let's work with whole numbers again
        chanceSkillSuccess *= 1000;
        
        if (isCraftSkill) {
            // Give players at least 50% chance at success if this is a crafting skill and their
            // skill is above minimum requirement
            chanceSkillSuccess = std::max(static_cast<float>(500), chanceSkillSuccess);
        }
        
        // Cap chance of success at 1000 (100.0%)
        chanceSkillSuccess = std::min(static_cast<float>(1000), chanceSkillSuccess);
        
        if (ServerConfig::shared().enabled(ServerSwitch::STATIMPACTSKILL)) {
            // Modify base chance of success with bonuses from stats, if this feature is enabled in
            // ini
            chanceSkillSuccess += static_cast<float>(s->GetStrength() * worldMain.skill[sk].strength) / 1000.0f;
            chanceSkillSuccess += static_cast<float>(s->GetDexterity() * worldMain.skill[sk].dexterity) / 1000.0f;
            chanceSkillSuccess += static_cast<float>(s->GetIntelligence() * worldMain.skill[sk].intelligence) / 1000.0f;
        }
        
        // If player's command-level is equal to Counselor or higher, pass the skill-check
        // automatically Same if chance of success is 100% already - no need to proceed!
        if (s->GetCommandLevel() > 0 || chanceSkillSuccess == 1000) {
            if (s->GetCommandLevel() > 0 && mSock != nullptr) {
                // Inform the counselor/gm to make it obvious why skillcheck always succeeds
                mSock->SysMessage(6279); // Skill check success guaranteed due to elevated command level!
            }
            skillCheck = true;
        }
        else {
            // Generate a random number between 0 and highSkill (if less than 1000) or 1000 (if
            // higher than 1000)
            std::int16_t rnd = RandomNum(0, std::min(1000, (highSkill + 100)));
            
            // Compare to chanceOfSkillSuccess to see if player succeeds!
            skillCheck = (static_cast<std::int16_t>(chanceSkillSuccess) >= rnd);
        }
        
        if (mSock != nullptr) {
            bool mageryUp = true;
            mageryUp = (mSock->CurrentSpellType() == 0);
            
            if (s->GetBaseSkill(sk) < highSkill) {
                if (sk != MAGERY || mageryUp) {
                    // Advance player's skill based on result of skillCheck
                    if (AdvanceSkill(s, sk, skillCheck)) {
                        UpdateSkillLevel(s, sk);
                        mSock->UpdateSkill(sk);
                    }
                }
            }
        }
        else if ((s->IsTamed() && ServerConfig::shared().enabled(ServerSwitch::PETCOMBATTRAINING)) || (!s->IsTamed() && s->CanBeHired() && ServerConfig::shared().enabled(ServerSwitch::HIRELINGTRAINING)) || (!s->IsTamed() && !s->CanBeHired() && s->IsNpc() && ServerConfig::shared().enabled(ServerSwitch::NPCCOMBAT))) {
            if (s->GetBaseSkill(sk) < highSkill) {
                // Advance pet/NPC's skill based on result of skillCheck
                if (AdvanceSkill(s, sk, skillCheck)) {
                    UpdateSkillLevel(s, sk);
                }
            }
        }
    }
    else {
        skillCheck = true;
    }
    return skillCheck;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::HandleSkillChange()
//|	Date		-	Jan 29, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Do atrophy for player c:
//|					find sk in our cronological list of atrophy skills, move it to
//the front, check
//|					total aginst skillCap to see if we need to lower a skill, if we
//do, again search
//|					skills for a skill that can be lowered, if one is found lower
//it and increase
//|					sk, if we can't find one, do nothing if atrophy is not need,
// increase sk.
// o------------------------------------------------------------------------------------------------o
void CSkills::HandleSkillChange(CChar *c, std::uint8_t sk, std::int8_t skillAdvance, bool success) {
    std::uint32_t totalSkill = 0;
    std::uint8_t rem = 0;
    std::uint8_t atrop[ALLSKILLS + 1];
    std::uint8_t toDec = 0xFF;
    std::uint8_t counter = 0;
    CSocket *mSock = c->GetSocket();
    
    std::vector<std::uint16_t> scriptTriggers = c->GetScriptTriggers();
    
    std::uint8_t amtToGain = 1;
    if (success) {
        amtToGain = worldMain.skill[sk].advancement[skillAdvance].amtToGain;
    }
    std::uint16_t skillCap = ServerConfig::shared().ushortValues[UShortValue::SKILLCAP] ;
    
    if (c->IsNpc()) {
        // Check for existence of onSkillGain event for NPC
        for (auto scriptTrig : scriptTriggers) {
            cScript *toExecute = JSMapping->GetScript(scriptTrig);
            if (toExecute != nullptr) {
                // If retVal is -1, event doesn't exist in script
                // If retVal is 0, event exists, but returned false/0, and handles item usage. Don't
                // proceed with hard code (or other scripts!) If retVal is 1, event exists, proceed
                // with hard code/other scripts
                if (!toExecute->OnSkillGain(c, sk, amtToGain)) {
                    return;
                }
            }
        }
        
        // Increase base skill of NPC
        c->SetBaseSkill(c->GetBaseSkill(sk) + amtToGain, sk);
        
        // Check for existence of onSkillChange event for NPC
        for (auto scriptTrig : scriptTriggers) {
            cScript *toExecute = JSMapping->GetScript(scriptTrig);
            if (toExecute != nullptr) {
                toExecute->OnSkillChange(c, sk, amtToGain);
            }
        }
        return;
    }
    
    if (mSock == nullptr)
        return;
    
    // srand( GetClock() ); // Randomize
    
    atrop[ALLSKILLS] = 0; // set the last of out copy array
    for (counter = 0; counter < ALLSKILLS; ++counter) {
        atrop[counter] = c->GetAtrophy(counter);
    }
    
    for (counter = ALLSKILLS; counter > 0; --counter) {
        // add up skills and find the one being increased
        std::uint8_t atrSkill = c->GetAtrophy(static_cast<std::uint8_t>(counter - 1));
        
        if (c->GetBaseSkill(atrSkill) >= amtToGain && c->GetSkillLock(atrSkill) == SKILL_DECREASE && atrSkill != sk) {
            toDec = atrSkill; // we found a skill that can be decreased, save it for later.
        }
        
        totalSkill += c->GetBaseSkill(static_cast<std::uint8_t>(counter - 1));
        
        atrop[counter] = atrop[counter - 1];
        
        if (atrop[counter] == sk) {
            rem = counter; // remember this number
        }
    }
    
    atrop[0] = sk; // set the first one to our current skill
    
    // copy it back in
    for (counter = 0; counter < rem; ++counter) {
        c->SetAtrophy(atrop[counter], counter);
    }
    if (rem != ALLSKILLS) { // in the middle somewhere or first
        for (counter = static_cast<std::uint8_t>(rem + 1); counter <= ALLSKILLS; ++counter) {
            c->SetAtrophy(atrop[counter], (counter - 1));
        }
    }
    
    if (RandomNum(static_cast<std::uint16_t>(0), skillCap) <= static_cast<std::uint16_t>(totalSkill)) {
        if (toDec != 0xFF) {
            // Check for existence of onSkillLoss event for player
            for (auto scriptTrig : scriptTriggers) {
                cScript *toExecute = JSMapping->GetScript(scriptTrig);
                if (toExecute != nullptr) {
                    // If retVal is -1, event doesn't exist in script
                    // If retVal is 0, event exists, but returned false/0, and handles item usage.
                    // Don't proceed with hard code (or other scripts!) If retVal is 1, event
                    // exists, proceed with hard code/other scripts
                    if (!toExecute->OnSkillLoss(c, toDec, amtToGain)) {
                        return;
                    }
                }
            }
            
            // Reduce base skill of player
            totalSkill -= amtToGain;
            c->SetBaseSkill(c->GetBaseSkill(toDec) - amtToGain, toDec);
            
            // Check for existence of onSkillChange event for player
            for (auto scriptTrig : scriptTriggers) {
                cScript *toExecute = JSMapping->GetScript(scriptTrig);
                if (toExecute != nullptr) {
                    toExecute->OnSkillChange(c, toDec, (amtToGain * -1));
                }
            }
            mSock->UpdateSkill(toDec);
        }
    }
    
    if (skillCap > static_cast<std::uint16_t>(totalSkill)) {
        // Check for existence of onSkillGain event for player
        for (auto scriptTrig : scriptTriggers) {
            cScript *toExecute = JSMapping->GetScript(scriptTrig);
            if (toExecute != nullptr) {
                // If retVal is -1, event doesn't exist in script
                // If retVal is 0, event exists, but returned false/0, and handles item usage. Don't
                // proceed with hard code (or other scripts!) If retVal is 1, event exists, proceed
                // with hard code/other scripts
                if (!toExecute->OnSkillGain(c, sk, amtToGain)) {
                    return;
                }
            }
        }
        
        // Increase base skill of player
        c->SetBaseSkill(c->GetBaseSkill(sk) + amtToGain, sk);
        
        // Check for existence of onSkillChange event for player
        for (auto scriptTrig : scriptTriggers) {
            cScript *toExecute = JSMapping->GetScript(scriptTrig);
            if (toExecute != nullptr) {
                toExecute->OnSkillChange(c, sk, amtToGain);
            }
        }
        mSock->UpdateSkill(sk);
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::SkillUse()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Called when player uses a skill from the skill list
// o------------------------------------------------------------------------------------------------o
// void ClilocMessage( CSocket *mSock, speechtype_t speechType, std::uint16_t hue, std::uint16_t font, std::uint32_t messageNum,
// const char *types = "", ... );
void CSkills::SkillUse(CSocket *s, std::uint8_t x) {
    VALIDATESOCKET(s);
    CChar *mChar = s->CurrcharObj();
    if (mChar->IsDead()) {
        // ClilocMessage( s, SYSTEM, worldMain.ServerData()->SysMsgColour(), FNT_NORMAL, 500012
        // );
        s->SysMessage(9054); // You cannot use skills while dead.
        return;
    }
    if ((x != STEALTH && mChar->GetVisible() == VT_TEMPHIDDEN) || mChar->GetVisible() == VT_INVISIBLE) {
        mChar->ExposeToView();
    }
    mChar->BreakConcentration(s);
    if (mChar->GetTimer(tCHAR_SPELLTIME) != 0 || mChar->IsCasting()) {
        s->SysMessage(854); // You can't do that while you are casting!
        return;
    }
    if (s->GetTimer(tPC_SKILLDELAY) <= worldMain.GetUICurrentTime() || mChar->IsGM()) {
        s->SkillDelayMsgShown(false);
        bool doSwitch = true;
        std::vector<std::uint16_t> scriptTriggers = mChar->GetScriptTriggers();
        for (auto scriptTrig : scriptTriggers) {
            // Loop through attached scripts
            cScript *toExecute = JSMapping->GetScript(scriptTrig);
            if (toExecute != nullptr) {
                if (toExecute->OnSkill(mChar, x)) {
                    // Look for onSkill JS event in script
                    doSwitch = false;
                }
            }
        }
        
        // If no onSkill event was found in a script already, check if skill has a script
        // attached directly, with onSkill event
        if (doSwitch && worldMain.skill[x].jsScript != 0xFFFF) {
            cScript *toExecute = JSMapping->GetScript(worldMain.skill[x].jsScript);
            if (toExecute != nullptr) {
                if (toExecute->OnSkill(mChar, x)) {
                    doSwitch = false;
                }
            }
        }
        
        // If no onSkill event has run yet, run the hardcoded version
        if (doSwitch) {
            switch (x) {
                case TRACKING:
                    TrackingMenu(s, TRACKINGMENUOFFSET);
                    break;
                default:
                    s->SysMessage(871);
                    break; // That skill has not been implemented yet.
            }
        }
        
        // If skillDelay timer is still below currenttimer, it wasn't modified in onSkill event. So
        // let's update it now!
        if (s->GetTimer(tPC_SKILLDELAY) <= worldMain.GetUICurrentTime()) {
            if (worldMain.skill[x].skillDelay != -1) {
                // Use skill-specific skill delay if one has been set
                s->SetTimer(tPC_SKILLDELAY, BuildTimeValue(static_cast<float>(worldMain.skill[x].skillDelay)));
            }
            else {
                // Otherwise use global skill delay from uox.ini
                s->SetTimer(tPC_SKILLDELAY, BuildTimeValue(static_cast<float>(ServerConfig::shared().ushortValues[UShortValue::SKILLDELAY])));
            }
        }
        return;
    }
    else {
        if (!s->SkillDelayMsgShown()) {
            s->SysMessage(872); // You must wait a few moments before using another skill.
            s->SkillDelayMsgShown(true);
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::Tracking()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Start tracking selected NPC/PC
// o------------------------------------------------------------------------------------------------o
void CSkills::Tracking(CSocket *s, std::int32_t selection) {
    VALIDATESOCKET(s);
    CChar *i = s->CurrcharObj();
    
    // sets trackingtarget that was selected in the gump
    i->SetTrackingTarget(i->GetTrackingTargets(selection));
    
    // tracking time in seconds ... gm tracker -> basetimer + 1 seconds, 0 tracking -> 1 sec, new
    // calc
    s->SetTimer(tPC_TRACKING, BuildTimeValue(static_cast<float>(ServerConfig::shared().timerSetting[TimerSetting::TRACKING] * i->GetSkill(TRACKING) / 1000 + 1)));
    s->SetTimer(tPC_TRACKINGDISPLAY, BuildTimeValue(static_cast<float>(ServerConfig::shared().ushortValues[UShortValue::MSGREDISPLAYTIME])));
    if (ValidateObject(i->GetTrackingTarget())) {
        std::string trackingTargetName = GetNpcDictName(i->GetTrackingTarget(), nullptr, NRS_SPEECH);
        s->SysMessage(1644, trackingTargetName.c_str()); // You are now tracking %s.
    }
    Track(i);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::CreateTrackingMenu()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Brings up Tracking Menu, Called when player uses Tracking Skill
// o------------------------------------------------------------------------------------------------o
void CSkills::CreateTrackingMenu(CSocket *s, std::uint16_t m) {
    VALIDATESOCKET(s);
    const std::string sect = std::string("TRACKINGMENU ") + util::ntos(m);
    CScriptSection *TrackStuff = FileLookup->FindEntry(sect, menus_def);
    if (TrackStuff == nullptr)
        return;
    
    enum CreatureTypes { CT_ANIMAL = 0, CT_CREATURE, CT_PERSON };
    
    CChar *mChar = s->CurrcharObj();
    std::uint16_t id = 0;
    CreatureTypes creatureType = CT_ANIMAL;
    std::int32_t type = 887; // You see no signs of any animals.
    std::uint8_t MaxTrackingTargets = 0;
    const std::uint16_t distance = (ServerConfig::shared().ushortValues[UShortValue::BASERANGE] + (mChar->GetSkill(TRACKING) / 50));
    unicodetypes_t mLang = s->Language();
    
    if (m == (2 + TRACKINGMENUOFFSET)) {
        creatureType = CT_CREATURE;
        type = 888; // You see no signs of any creatures.
    }
    else if (m == (3 + TRACKINGMENUOFFSET)) {
        creatureType = CT_PERSON;
        type = 889; // You see no signs of anyone.
    }
    
    std::string line;
    CPOpenGump toSend(*mChar);
    if (m >= TRACKINGMENUOFFSET) {
        toSend.GumpIndex(m);
    }
    else {
        toSend.GumpIndex(m + TRACKINGMENUOFFSET);
    }
    std::string tag = TrackStuff->First();
    std::string data = TrackStuff->GrabData();
    
    line = tag + " " + data;
    toSend.Question(line);
    
    for (auto &MapArea : MapRegion->PopulateList(mChar)) {
        if (MapArea) {
            auto regChars = MapArea->GetCharList();
            for (const auto &tempChar : regChars->collection()) {
                if (MaxTrackingTargets < ServerConfig::shared().ushortValues[UShortValue::MAXTARGET]) {
                    if (ValidateObject(tempChar) && (tempChar->GetInstanceId() == mChar->GetInstanceId())) {
                        id = tempChar->GetId();
                        if ((!worldMain.creatures[id].IsHuman() || creatureType == CT_PERSON) && (!worldMain.creatures[id].IsAnimal() || creatureType == CT_ANIMAL)) {
                            const bool cmdLevelCheck = (IsOnline((*tempChar)) && (mChar->GetCommandLevel() >= tempChar->GetCommandLevel()));
                            if (tempChar != mChar && ObjInRange(tempChar, mChar, distance) && !tempChar->IsDead() && (cmdLevelCheck || tempChar->IsNpc())) {
                                mChar->SetTrackingTargets(tempChar, MaxTrackingTargets);
                                ++MaxTrackingTargets;
                                
                                std::int32_t dirMessage = 898; // right next to you.
                                switch (Movement->Direction(mChar, tempChar->GetX(), tempChar->GetY())) {
                                    case NORTH:
                                        dirMessage = 890;
                                        break; // to the North.
                                    case NORTHWEST:
                                        dirMessage = 891;
                                        break; // to the Northwest.
                                    case NORTHEAST:
                                        dirMessage = 892;
                                        break; // to the Northeast.
                                    case SOUTH:
                                        dirMessage = 893;
                                        break; // to the South.
                                    case SOUTHWEST:
                                        dirMessage = 894;
                                        break; // to the Southwest.
                                    case SOUTHEAST:
                                        dirMessage = 895;
                                        break; // to the Southeast.
                                    case WEST:
                                        dirMessage = 896;
                                        break; // to the West.
                                    case EAST:
                                        dirMessage = 897;
                                        break; // to the East.
                                    default:
                                        dirMessage = 898;
                                        break; // right next to you.
                                }
                                auto tempName = GetNpcDictName(tempChar, nullptr, NRS_SYSTEM);
                                line = tempName + " "s + worldDictionary.GetEntry(dirMessage, mLang);
                                toSend.AddResponse(worldMain.creatures[id].Icon(), 0, line);
                            }
                        }
                    }
                }
                else {
                    break;
                }
            }
        }
    }
    
    if (MaxTrackingTargets == 0) {
        s->SysMessage(type);
        return;
    }
    toSend.Finalize();
    s->Send(&toSend);
}

void HandleCommonGump(CSocket *mSock, CScriptSection *gumpScript, std::uint16_t gumpIndex);
// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::TrackingMenu()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Brings up additional tracking menu with options listed in
// tracking.dfn
// o------------------------------------------------------------------------------------------------o
void CSkills::TrackingMenu(CSocket *s, std::uint16_t gmindex) {
    VALIDATESOCKET(s);
    const std::string sect = std::string("TRACKINGMENU ") + util::ntos(gmindex);
    CScriptSection *TrackStuff = FileLookup->FindEntry(sect, menus_def);
    if (TrackStuff == nullptr) {
        return;
    }
    HandleCommonGump(s, TrackStuff, gmindex);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::Track()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Tracking stuff for older client
// o------------------------------------------------------------------------------------------------o
void CSkills::Track(CChar *i) {
    CSocket *s = i->GetSocket();
    VALIDATESOCKET(s);
    CChar *trackTarg = i->GetTrackingTarget();
    if (!ValidateObject(trackTarg) || trackTarg->IsDeleted() || trackTarg->GetY() == -1) {
        s->SysMessage(2059); // You have lost your quarry.
        s->SetTimer(tPC_TRACKING, 0);
        CPTrackingArrow tSend;
        tSend.Active(0);
        if (s->clientType() >= ClientType::HS2D) {
            tSend.AddSerial(i->GetTrackingTargetSerial());
        }
        s->Send(&tSend);
        return;
    }
    CPTrackingArrow tSend = (*trackTarg);
    tSend.Active(1);
    if (s->clientType() >= ClientType::HS2D) {
        tSend.AddSerial(i->GetTrackingTarget()->GetSerial());
    }
    s->Send(&tSend);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::UpdateSkillLevel()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate the skill of this character based on the characters
// baseskill and stats
// o------------------------------------------------------------------------------------------------o
void CSkills::UpdateSkillLevel(CChar *c, std::uint8_t s) const {
    std::uint16_t sStr = worldMain.skill[s].strength;
    std::int16_t aStr = c->ActualStrength();
    std::uint16_t sDex = worldMain.skill[s].dexterity;
    std::int16_t aDex = c->ActualDexterity();
    std::uint16_t sInt = worldMain.skill[s].intelligence;
    std::int16_t aInt = c->ActualIntelligence();
    std::uint16_t bSkill = c->GetBaseSkill(s);
    
    std::uint16_t temp = (((sStr * aStr) / 100 + (sDex * aDex) / 100 + (sInt + aInt) / 100) * (1000 - bSkill)) / 1000 + bSkill;
    c->SetSkill(std::max(bSkill, temp), s);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::Persecute()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Called when a Ghost attacks a Player.  If entry in UOX.INI
//|					is enabled, players mana decreases each time you try to
//|					persecute him
// o------------------------------------------------------------------------------------------------o
void CSkills::Persecute(CSocket *s) {
    VALIDATESOCKET(s);
    CChar *c = s->CurrcharObj();
    CChar *targChar = c->GetTarg();
    if (!ValidateObject(targChar) || targChar->IsGM())
        return;
    
    std::int32_t decrease =
    static_cast<std::int32_t>((c->GetSkill(SPIRITSPEAK) / 100) + (c->GetIntelligence() / 20)) + 3;
    
    if (s->GetTimer(tPC_SKILLDELAY) <= worldMain.GetUICurrentTime() || c->IsGM()) {
        if ((RandomNum(0, 19) + c->GetIntelligence()) > 45) // not always
        {
            CSocket *tSock = targChar->GetSocket();
            targChar->SetMana(targChar->GetMana() - decrease); // decrease mana
            s->SysMessage(972); // Your spiritual forces disturb the enemy!
            if (tSock != nullptr) {
                tSock->SysMessage(973); // A damned soul is disturbing your mind!
            }
            
            if (worldMain.skill[SPIRITSPEAK].skillDelay != -1) {
                // Use skill-specific skill delay if one has been set
                s->SetTimer(tPC_SKILLDELAY, BuildTimeValue(static_cast<float>(worldMain.skill[SPIRITSPEAK].skillDelay)));
            }
            else {
                // Otherwise use global skill delay from uox.ini
                s->SetTimer(tPC_SKILLDELAY, BuildTimeValue(static_cast<float>(ServerConfig::shared().ushortValues[UShortValue::SKILLDELAY] )));
            }
            
            std::string targCharName = GetNpcDictName(targChar, nullptr, NRS_SPEECH);
            targChar->TextMessage(nullptr, 974, EMOTE, 1, targCharName.c_str()); // %s is persecuted by a ghost!
        }
        else {
            s->SysMessage(975); // Your mind is not strong enough to disturb the enemy
        }
    }
    else {
        s->SysMessage(976); // You are unable to persecute him now. Rest a little.
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::Smith()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Called when player attempts to smith an item
// o------------------------------------------------------------------------------------------------o
void CSkills::Smith(CSocket *s) {
    VALIDATESOCKET(s);
    CChar *mChar = s->CurrcharObj();
    CItem *packnum = mChar->GetPackItem();
    
    // Check if item used to initialize target cursor is still within range
    CItem *tempObj = static_cast<CItem *>(s->TempObj());
    s->TempObj(nullptr);
    if (ValidateObject(tempObj)) {
        if (tempObj->IsHeldOnCursor() || !CheckItemRange(mChar, tempObj)) {
            s->SysMessage(400); // That is too far away!
            return;
        }
    }
    
    if (!ValidateObject(packnum)) {
        s->SysMessage(773); // Time to buy a backpack.
        return;
    }
    
    CItem *i = CalcItemObjFromSer(s->GetDWord(7));
    if (ValidateObject(i)) {
        if (i->IsHeldOnCursor() || !CheckItemRange(mChar, i)) {
            s->SysMessage(400); // That is too far away!
            return;
        }
        
        if (i->GetId() >= 0x1BE3 && i->GetId() <= 0x1BF9) // is it an ingot?
        {
            MiningData_st *oreType = FindOre(i->GetColour());
            if (oreType == nullptr) {
                s->SysMessage(977); // Unknown ingot type.
                return;
            }
            if (FindItemOwner(i) != mChar) {
                s->SysMessage(978, oreType->name.c_str()); // You can't smith %s ingots outside your backpack.
            }
            else {
                AnvilTarget(s, (*i), oreType);
            }
        }
        else {
            // something we might repair?
            RepairMetal(s);
        }
        return;
    }
    s->SysMessage(979); // You cannot use that material for blacksmithing!
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::AnvilTarget()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check for the presence of an anvil when player attempts to use
// smithing skill,
//|					then verify that there is enough of the chosen ingot type in
//their backpack(s)
// o------------------------------------------------------------------------------------------------o
void CSkills::AnvilTarget(CSocket *s, CItem &item, MiningData_st *oreType) {
    VALIDATESOCKET(s);
    CChar *mChar = s->CurrcharObj();
    
    for (auto &MapArea : MapRegion->PopulateList(mChar)) {
        if (MapArea) {
            auto regItems = MapArea->GetItemList();
            for (const auto &tempItem : regItems->collection()) {
                if (ValidateObject(tempItem) &&
                    (tempItem->GetInstanceId() == mChar->GetInstanceId())) {
                    if (tempItem->GetId() == 0x0FAF || tempItem->GetId() == 0x0FB0) {
                        if (ObjInRange(mChar, tempItem, DIST_NEARBY)) {
                            std::uint32_t getAmt = GetItemAmount(mChar, item.GetId(), item.GetColour(), item.GetTempVar(CITV_MORE));
                            if (getAmt == 0) {
                                s->SysMessage(980, oreType->name.c_str()); // You don't have enough %s
                                // ingots to make anything.
                                return;
                            }
                            NewMakeMenu(s, oreType->makemenu, BLACKSMITHING);
                            return;
                        }
                    }
                }
            }
        }
    }
    s->SysMessage(981); // The anvil is too far away.
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::CSkills()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Default constructor
// o------------------------------------------------------------------------------------------------o
CSkills::CSkills() { ores.resize(0); }
CSkills::~CSkills() {
    ores.resize(0);
    skillMenus.clear();
    itemsForMenus.clear();
    actualMenus.clear();
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::LoadMiningData()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Load mining resource stuff from scripts
// o------------------------------------------------------------------------------------------------o
bool CSkills::LoadMiningData() {
    ores.resize(0);
    // Let's first get our ore list, in SECTION ORE_LIST
    CScriptSection *oreList = FileLookup->FindEntry("ORE_LIST", skills_def);
    bool rValue = false;
    if (oreList != nullptr) {
        std::vector<std::string> oreNameList;
        std::string tag;
        std::string data;
        std::string UTag;
        for (tag = oreList->First(); !oreList->AtEnd(); tag = oreList->Next()) {
            oreNameList.push_back(tag);
        }
        if (!oreNameList.empty()) {
            rValue = true;
            CScriptSection *individualOre = nullptr;
            for (auto &oreName : oreNameList) {
                individualOre = FileLookup->FindEntry(oreName, skills_def);
                if (individualOre != nullptr) {
                    MiningData_st toAdd;
                    toAdd.colour = 0;
                    toAdd.makemenu = 0;
                    toAdd.minSkill = 0;
                    toAdd.name = oreName;
                    toAdd.oreName = oreName;
                    toAdd.oreChance = 0;
                    toAdd.scriptID = 0;
                    for (const auto &sec : individualOre->collection()) {
                        tag = sec->tag;
                        data = sec->data;
                        UTag = util::upper(tag);
                        data = util::trim(util::strip(data, "//"));
                        switch ((UTag.data()[0])) // break on tag
                        {
                            case 'C':
                                if (UTag == "COLOUR") {
                                    toAdd.colour = static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
                                }
                                break;
                            case 'F':
                                break;
                            case 'M':
                                if (UTag == "MAKEMENU") {
                                    toAdd.makemenu = std::stoi(data, nullptr, 0);
                                }
                                else if (UTag == "MINSKILL") {
                                    toAdd.minSkill = static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
                                }
                                break;
                            case 'N':
                                if (UTag == "NAME") {
                                    toAdd.name = data;
                                }
                                break;
                            case 'O':
                                if (UTag == "ORECHANCE") {
                                    toAdd.oreChance = static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
                                }
                                break;
                            case 'S':
                                if (UTag == "SCRIPT") {
                                    toAdd.scriptID = static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
                                }
                                break;
                            default:
                                Console::shared() << "Unknown mining tag " << tag << " with data " << data << " in SECTION " << oreName << myendl;
                                break;
                        }
                    }
                    ores.push_back(toAdd);
                }
            }
        }
    }
    return rValue;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::load()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Load mining stuff from scripts
// o------------------------------------------------------------------------------------------------o
void CSkills::load() {
    Console::shared() << "Loading custom ore data        ";
    
    if (!LoadMiningData()) {
        Shutdown(FATAL_UOX3_ORELIST);
        return;
    }
    
    Console::shared().printDone();
    
    Console::shared() << "Loading creation menus         ";
    LoadCreateMenus();
    Console::shared().printDone();
    
    CJSMappingSection *skillSection = JSMapping->GetSection(CJSMappingSection::SCPT_SKILLUSE);
    for (cScript *ourScript = skillSection->First(); !skillSection->Finished();
         ourScript = skillSection->Next()) {
        if (ourScript != nullptr) {
            ourScript->ScriptRegistration("Skill");
        }
    }
    
    Console::shared().printSectionBegin();
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::GetNumberOfOres()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns number of custom ores
// o------------------------------------------------------------------------------------------------o
size_t CSkills::GetNumberOfOres() { return ores.size(); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::GetOre()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a handle to the data about the ore
// o------------------------------------------------------------------------------------------------o
MiningData_st *CSkills::GetOre(size_t number) {
    if (number >= ores.size())
        return nullptr;
    
    return &ores[number];
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::FindOre()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a handle to the data about the ore based on it's name
// o------------------------------------------------------------------------------------------------o
MiningData_st *CSkills::FindOre(std::string const &name) {
    std::vector<MiningData_st>::iterator oreIter;
    for (oreIter = ores.begin(); oreIter != ores.end(); ++oreIter) {
        if ((*oreIter).oreName == name) {
            return &(*oreIter);
        }
    }
    return nullptr;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	*CSkills::FindOre()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Find ore color
// o------------------------------------------------------------------------------------------------o
MiningData_st *CSkills::FindOre(std::uint16_t const &colour) {
    std::vector<MiningData_st>::iterator oreIter;
    for (oreIter = ores.begin(); oreIter != ores.end(); ++oreIter) {
        if ((*oreIter).colour == colour) {
            return &(*oreIter);
        }
    }
    return nullptr;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::LoadCreateMenus()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Load Create menus from the create DFNs
// o------------------------------------------------------------------------------------------------o
auto CSkills::LoadCreateMenus() -> void {
    actualMenus.clear();
    skillMenus.clear();
    itemsForMenus.clear();
    
    std::uint16_t ourEntry; // our actual entry number
    for (Script *ourScript = FileLookup->FirstScript(create_def);
         !FileLookup->FinishedScripts(create_def); ourScript = FileLookup->NextScript(create_def)) {
        if (ourScript == nullptr) {
            continue;
        }
        
        for (const auto &[eName1, toSearch] : ourScript->collection()) {
            auto eName = eName1;
            if ("SUBMENU" == eName.substr(0, 7)) { // is it a menu? (not really SUB, just to avoid picking up MAKEMENUs)
                eName = util::trim(util::strip(eName, "//"));
                auto ssecs = oldstrutil::sections(eName, " ");
                ourEntry = static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(ssecs[1], "//")), nullptr, 0));
                for (const auto &sec : toSearch->collection()) {
                    auto tag = sec->tag;
                    auto data = sec->data;
                    auto UTag = util::upper(tag);
                    data = util::trim(util::strip(data, "//"));
                    if (UTag == "MENU") {
                        actualMenus[ourEntry].menuEntries.push_back(static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)));
                    }
                    else if (UTag == "ITEM") {
                        actualMenus[ourEntry].itemEntries.push_back(static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)));
                    }
                }
            }
            else if ("ITEM" == eName.substr(0, 4)) // is it an item?
            {
                auto ssecs = oldstrutil::sections(eName, " ");
                
                ourEntry = static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(ssecs[1], "//")), nullptr, 0));
                CreateEntry_st tmpEntry;
                tmpEntry.minRank = 0;
                tmpEntry.maxRank = 10;
                tmpEntry.colour = 0;
                tmpEntry.targId = 1;
                tmpEntry.soundPlayed = 0;
                
                for (const auto &sec : toSearch->collection()) {
                    auto tag = sec->tag;
                    auto data = sec->data;
                    auto UTag = util::upper(tag);
                    data = util::trim(util::strip(data, "//"));
                    if (UTag == "COLOUR") {
                        tmpEntry.colour = static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
                    }
                    else if (UTag == "ID") {
                        tmpEntry.targId = static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
                    }
                    else if (UTag == "MINRANK") {
                        tmpEntry.minRank = static_cast<std::uint8_t>(std::stoul(data, nullptr, 0));
                    }
                    else if (UTag == "MAXRANK") {
                        tmpEntry.maxRank = static_cast<std::uint8_t>(std::stoul(data, nullptr, 0));
                    }
                    else if (UTag == "NAME") {
                        tmpEntry.name = data;
                    }
                    else if (UTag == "SOUND") {
                        tmpEntry.soundPlayed = static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
                    }
                    else if (UTag == "ADDITEM") {
                        tmpEntry.addItem = data;
                    }
                    else if (UTag == "DELAY") {
                        tmpEntry.delay = static_cast<std::int16_t>(std::stoi(data, nullptr, 0));
                    }
                    else if (UTag == "RESOURCE") {
                        ResAmountPair_st tmpResource;
                        auto ssecs = oldstrutil::sections(data, " ");
                        if (ssecs.size() > 1) {
                            if (ssecs.size() == 4) {
                                tmpResource.moreVal = static_cast<std::uint32_t>(std::stoul(util::trim(util::strip(ssecs[3], "//")), nullptr, 0));
                            }
                            if (ssecs.size() >= 3) {
                                tmpResource.colour = static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(ssecs[2], "//")), nullptr, 0));
                            }
                            if (ssecs.size() >= 2) {
                                tmpResource.amountNeeded = static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(ssecs[1], "//")), nullptr, 0));
                            }
                        }
                        auto resType = "RESOURCE "s + util::trim(util::strip(ssecs[0], "//"));
                        auto resList = FileLookup->FindEntry(resType, create_def);
                        if (resList) {
                            for (const auto &sec : resList->collection()) {
                                tmpResource.idList.push_back(static_cast<std::uint16_t>(std::stoul(sec->data, nullptr, 0)));
                            }
                        }
                        else {
                            tmpResource.idList.push_back(static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(ssecs[0], "//")), nullptr, 0)));
                        }
                        
                        tmpEntry.resourceNeeded.push_back(tmpResource);
                    }
                    else if (UTag == "SKILL") {
                        ResSkillReq_st tmpResource;
                        auto ssecs = oldstrutil::sections(data, " ");
                        if (ssecs.size() == 1) {
                            tmpResource.maxSkill = 1000;
                            tmpResource.minSkill = 0;
                            tmpResource.skillNumber =
                            static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
                        }
                        else {
                            if (ssecs.size() == 2) {
                                tmpResource.maxSkill = 1000;
                                tmpResource.minSkill = static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(ssecs[1], "//")), nullptr, 0));
                            }
                            else {
                                tmpResource.minSkill = static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(ssecs[1], "//")), nullptr, 0));
                                tmpResource.maxSkill = static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(ssecs[2], "//")), nullptr, 0));
                            }
                            tmpResource.skillNumber = static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(ssecs[0], "//")), nullptr, 0));
                        }
                        tmpEntry.skillReqs.push_back(tmpResource);
                    }
                    else if (UTag == "SPELL") {
                        tmpEntry.spell = static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
                    }
                }
                itemsForMenus[ourEntry] = tmpEntry;
            }
            else if ("MENUENTRY" == eName.substr(0, 9)) {
                auto ssecs = oldstrutil::sections(eName, " ");
                ourEntry = static_cast<std::uint16_t>(
                                                      std::stoul(util::trim(util::strip(ssecs[1], "//")), nullptr, 0));
                for (const auto &sec : toSearch->collection()) {
                    auto tag = sec->tag;
                    auto data = sec->data;
                    auto UTag = util::upper(tag);
                    data = util::trim(util::strip(data, "//"));
                    if (UTag == "ID") {
                        skillMenus[ourEntry].targId =
                        static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
                    }
                    else if (UTag == "COLOUR") {
                        skillMenus[ourEntry].colour =
                        static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
                    }
                    else if (UTag == "NAME") {
                        skillMenus[ourEntry].name = data;
                    }
                    else if (UTag == "SUBMENU") {
                        skillMenus[ourEntry].subMenu =
                        static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
                    }
                }
            }
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::AdvanceSkill()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Advance players skill based on success or failure in CheckSkill()
// o------------------------------------------------------------------------------------------------o
bool CSkills::AdvanceSkill(CChar *s, std::uint8_t sk, bool skillUsed) {
    bool advSkill = false;
    std::int16_t skillGain;
    
    std::int8_t skillAdvance = FindSkillPoint(sk, s->GetBaseSkill(sk));
    
    if (skillUsed) {
        skillGain = (worldMain.skill[sk].advancement[skillAdvance].success);
    }
    else {
        skillGain = (worldMain.skill[sk].advancement[skillAdvance].failure);
    }
    
    if (skillGain > RandomNum(1, 100)) {
        advSkill = true;
        if (s->GetSkillLock(sk) == SKILL_INCREASE) {
            HandleSkillChange(s, sk, skillAdvance, skillUsed);
        }
    }
    
    if (s->GetSkillLock(sk) != SKILL_LOCKED) // if it's locked, stats can't advance
    {
        AdvanceStats(s, sk, skillUsed);
    }
    return advSkill;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::FindSkillPoint()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Find skillpoint advancement parameters from a skill in skills.dfn,
//|					based on a specified skill value
// o------------------------------------------------------------------------------------------------o
std::int8_t CSkills::FindSkillPoint(std::uint8_t sk, std::int32_t value) {
    std::int8_t retVal = -1;
    for (size_t iCounter = 0; iCounter < worldMain.skill[sk].advancement.size() - 1;
         ++iCounter) {
        if (worldMain.skill[sk].advancement[iCounter].base <= value &&
            value < worldMain.skill[sk].advancement[iCounter + 1].base) {
            retVal = static_cast<std::int8_t>(iCounter);
            break;
        }
    }
    if (retVal == -1) {
        retVal = static_cast<std::int8_t>(worldMain.skill[sk].advancement.size() - 1);
    }
    return retVal;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::AdvanceStats()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Advance players stats
// o------------------------------------------------------------------------------------------------o
void CSkills::AdvanceStats(CChar *s, std::uint8_t sk, bool skillsuccess) {
    CRace *pRace = Races->Race(s->GetRace());
    
    // If the Race is invalid just use the default race
    if (pRace == nullptr) {
        pRace = Races->Race(0);
    }
    
    // make sure socket is no npc
    if (s->IsNpc())
        return;
    
    std::uint32_t serverStatCap = ServerConfig::shared().ushortValues[UShortValue::STATCAP];
    std::uint32_t ttlStats = s->ActualStrength() + s->ActualDexterity() + s->ActualIntelligence();
    std::int16_t chanceStatGain = 0; // 16bit because of freaks that raises it > 100
    std::int32_t statCount, nCount;
    std::int32_t toDec = 255;
    std::uint16_t maxChance = 100;
    std::int16_t ActualStat[3] = {s->ActualStrength(), s->ActualDexterity(), s->ActualIntelligence()};
    std::uint16_t StatModifier[3] = {worldMain.skill[sk].strength, worldMain.skill[sk].dexterity, worldMain.skill[sk].intelligence};
    skilllock_t StatLocks[3] = {s->GetSkillLock(STRENGTH), s->GetSkillLock(DEXTERITY), s->GetSkillLock(INTELLECT)};
    
    std::vector<std::uint16_t> skillUpdTriggers = s->GetScriptTriggers();
    
    for (statCount = STRENGTH; statCount <= INTELLECT; ++statCount) {
        nCount = statCount - ALLSKILLS - 1;
        
        // if current stat isn't allowed to increase skip it.
        if (StatLocks[nCount] == SKILL_INCREASE) {
            //  the following will calculate the chances for str/dex/int to increase
            //
            //  it is divided into 2 "dices":
            //  first dice: get the success-skillpoint of the stat out of skills.dfn in dependence
            //				of the racial statCap for this stat... x means stat is x% of
            // statCap
            //				=> get skillpoint for x
            //  sec. dice:	get the chance for this stat to increase when the used skill has
            //  been used
            //				(out of skills.dfn) => get x = statmodifier of skill
            
            //  last make it a integer between 0 and 1000 normally (negative or 0==no chance)
            
            //	special dice 1: the stat wont increase above x% of the racial statCap. x% is
            // equivalent to dice 2.
            //  special dice 2: skill failed: decrease chance by 50%
            
            //  k, first let us calculate both dices
            std::uint8_t modifiedStatLevel = FindSkillPoint(statCount - 1, static_cast<std::int32_t>(static_cast<float>(ActualStat[nCount]) / static_cast<float>(pRace->Skill(statCount)) * 100));
            chanceStatGain = static_cast<std::int16_t>((static_cast<float>(worldMain.skill[statCount - 1].advancement[modifiedStatLevel].success) / 100) * (static_cast<float>(static_cast<float>(StatModifier[nCount]) / 10) / 100) * 1000);
            // some mathematics in it;)
            
            // now, lets implement the special dice 1 and additionally check for onStatGain
            // javascript method
            if (StatModifier[nCount] <= static_cast<std::int32_t>(static_cast<float>(ActualStat[nCount]) / static_cast<float>(pRace->Skill(statCount)) * 100)) {
                chanceStatGain = 0;
            }
            
            // special dice 2
            if (!skillsuccess) {
                maxChance *= 2;
            }
            
            // if stat of char < racial statCap and chance for statgain > random number from 0 to
            // 100
            if (ActualStat[nCount] < pRace->Skill(statCount) && chanceStatGain > RandomNum(static_cast<std::uint16_t>(0), maxChance)) {
                // Check if we have to decrease a stat
                if ((ttlStats + 1) >= RandomNum(serverStatCap - 10, serverStatCap)) {
                    for (std::int32_t i = 0; i < 3; i++) {
                        if (StatLocks[i] == SKILL_DECREASE) {
                            // Decrease the highest stat, that is allowed to decrease
                            if (toDec == 255) {
                                toDec = i;
                            }
                            else {
                                if (ActualStat[i] > ActualStat[toDec]) {
                                    toDec = i;
                                }
                            }
                        }
                    }
                    
                    switch (toDec) {
                        case 0: // Decrease Strength stat
                            // First trigger onStatLoss event
                            for (auto skillTrig : skillUpdTriggers) {
                                cScript *toExecute = JSMapping->GetScript(skillTrig);
                                if (toExecute != nullptr) {
                                    if (!toExecute->OnStatLoss(s, STRENGTH, 1))
                                        return;
                                }
                            }
                            
                            // Do the actual stat decrease
                            s->IncStrength(-1);
                            ttlStats--;
                            
                            // Trigger onStatChange event if onStatLoss either didn't exist, or returned
                            // true
                            for (auto skillTrig : skillUpdTriggers) {
                                cScript *toExecute = JSMapping->GetScript(skillTrig);
                                if (toExecute != nullptr) {
                                    toExecute->OnStatChange(s, STRENGTH, -1);
                                }
                            }
                            break;
                        case 1: // Decrease Dexterity stat
                            // First trigger onStatLoss event
                            for (auto skillTrig : skillUpdTriggers) {
                                cScript *toExecute = JSMapping->GetScript(skillTrig);
                                if (toExecute != nullptr) {
                                    if (!toExecute->OnStatLoss(s, DEXTERITY, 1))
                                        return;
                                }
                            }
                            
                            // Do the actual stat decrease
                            s->IncDexterity(-1);
                            ttlStats--;
                            
                            // Trigger onStatChange event if onStatLoss either didn't exist, or returned
                            // true
                            for (auto skillTrig : skillUpdTriggers) {
                                cScript *toExecute = JSMapping->GetScript(skillTrig);
                                if (toExecute != nullptr) {
                                    toExecute->OnStatChange(s, DEXTERITY, -1);
                                }
                            }
                            break;
                        case 2: // Decrease Intelligence stat
                            // First trigger onStatLoss event
                            for (auto skillTrig : skillUpdTriggers) {
                                cScript *toExecute = JSMapping->GetScript(skillTrig);
                                if (toExecute != nullptr) {
                                    if (!toExecute->OnStatLoss(s, INTELLECT, 1))
                                        return;
                                }
                            }
                            
                            // Do the actual stat decrease
                            s->IncIntelligence(-1);
                            ttlStats--;
                            
                            // Trigger onStatChange event if onStatLoss either didn't exist, or returned
                            // true
                            for (auto skillTrig : skillUpdTriggers) {
                                cScript *toExecute = JSMapping->GetScript(skillTrig);
                                if (toExecute != nullptr) {
                                    toExecute->OnStatChange(s, INTELLECT, -1);
                                }
                            }
                            break;
                        default:
                            break;
                    }
                }
                
                // Do we still hit the stat limit?
                if ((ttlStats + 1) <= serverStatCap) {
                    // First trigger onStatGained
                    for (auto skillTrig : skillUpdTriggers) {
                        cScript *toExecute = JSMapping->GetScript(skillTrig);
                        if (toExecute != nullptr) {
                            if (!toExecute->OnStatGained(s, statCount, sk, 1))
                                return;
                        }
                    }
                    
                    // Do the actual stat increase
                    switch (statCount) {
                        case STRENGTH:
                            s->IncStrength();
                            break;
                        case DEXTERITY:
                            s->IncDexterity();
                            break;
                        case INTELLECT:
                            s->IncIntelligence();
                            break;
                        default:
                            break;
                    }
                    
                    // Trigger onStatChange event if onStatGained either didn't exist, or returned
                    // true
                    for (auto skillTrig : skillUpdTriggers) {
                        cScript *toExecute = JSMapping->GetScript(skillTrig);
                        if (toExecute != nullptr) {
                            toExecute->OnStatChange(s, statCount, 1);
                        }
                    }
                    
                    break; // only one stat at a time fellas
                }
            }
        }
    }
    
    s->Dirty(UT_STATWINDOW);
    for (std::uint8_t i = 0; i < ALLSKILLS; ++i) {
        UpdateSkillLevel(s, i);
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::NewMakeMenu()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	New make menu system, based on create DFNs
// o------------------------------------------------------------------------------------------------o
void CSkills::NewMakeMenu(CSocket *s, std::int32_t menu, [[maybe_unused]] std::uint8_t skill) {
    VALIDATESOCKET(s);
    CChar *ourChar = s->CurrcharObj();
    s->AddId(menu);
    std::map<std::uint16_t, CreateMenu_st>::const_iterator p = actualMenus.find(menu);
    if (p == actualMenus.end())
        return;
    
    std::uint16_t background = ServerConfig::shared().ushortValues[UShortValue::BACKGROUNDPIC] ;
    std::uint16_t btnCancel = ServerConfig::shared().ushortValues[UShortValue::BUTTONCANCEL];
    [[maybe_unused]] std::uint16_t btnLeft = ServerConfig::shared().ushortValues[UShortValue::BUTTONLEFT];
    std::uint16_t btnRight = ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT];
    
    CPSendGumpMenu toSend;
    toSend.UserId(INVALIDSERIAL);
    toSend.GumpId(12);
    
    CreateMenu_st ourMenu = p->second;
    std::uint32_t textCounter = 0;
    CScriptSection *GumpHeader = FileLookup->FindEntry("ADDMENU HEADER", misc_def);
    if (GumpHeader == nullptr) {
        toSend.addCommand(util::format("resizepic 0 0 %i 400 320", background));
        toSend.addCommand("page 0");
        toSend.addCommand("text 200 20 0 0");
        toSend.addText("Create menu");
        ++textCounter;
        toSend.addCommand(util::format("button 360 15 %i %i 1 0 1", btnCancel, btnCancel + 1));
    }
    else {
        std::string tag, data, UTag;
        for (tag = GumpHeader->First(); !GumpHeader->AtEnd(); tag = GumpHeader->Next()) {
            UTag = util::upper(tag);
            data = GumpHeader->GrabData();
            data = util::trim(util::strip(data, "//"));
            if (UTag == "BUTTONLEFT") {
                btnLeft = static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
            }
            else if (UTag == "BUTTONRIGHT") {
                btnRight = static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
            }
            else if (UTag == "BUTTONCANCEL") {
                btnCancel = static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
            }
            else {
                std::string built = tag;
                if (!data.empty()) {
                    built += " ";
                    built += data;
                }
                toSend.addCommand(built);
            }
        }
        CScriptSection *GumpText = FileLookup->FindEntry("ADDMENU TEXT", misc_def);
        if (GumpText != nullptr) {
            for (tag = GumpText->First(); !GumpText->AtEnd(); tag = GumpText->Next()) {
                data = GumpText->GrabData();
                std::string built = tag;
                if (!data.empty()) {
                    built += " ";
                    built += data;
                }
                toSend.addText(built);
                ++textCounter;
            }
        }
    }
    
    std::uint16_t xLoc = 60, yLoc = 40;
    std::map<std::uint16_t, CreateEntry_st>::iterator imIter;
    std::map<std::uint16_t, CreateMenuEntry_st>::iterator smIter;
    std::int32_t actualItems = 0;
    for (ourMenu.iIter = ourMenu.itemEntries.begin(); ourMenu.iIter != ourMenu.itemEntries.end();
         ++ourMenu.iIter) {
        if ((actualItems % 6) == 0 && actualItems != 0) {
            xLoc += 180;
            yLoc = 40;
        }
        imIter = itemsForMenus.find((*ourMenu.iIter));
        if (imIter != itemsForMenus.end()) {
            CreateEntry_st iItem = imIter->second;
            bool canMake = true;
            for (size_t sCounter = 0; sCounter < iItem.skillReqs.size() && canMake; ++sCounter) {
                std::uint8_t skillNum = iItem.skillReqs[sCounter].skillNumber;
                std::uint16_t ourSkill = ourChar->GetSkill(skillNum);
                std::uint16_t minSkill = iItem.skillReqs[sCounter].minSkill;
                canMake = (ourSkill >= minSkill);
                // it was not thought that way, its not logical, maxSkill should say, that you can
                // get maxRank with maxSkill and higher
            }
            if (iItem.spell) { // Should only display the spell if character has a spellbook and the
                // spell in his book
                CItem *spellBook = FindItemOfType(ourChar, IT_SPELLBOOK);
                if (!ValidateObject(spellBook)) {
                    canMake = false;
                }
                else {
                    std::int32_t getCir = static_cast<std::int32_t>(iItem.spell * .1);
                    std::int32_t getSpell = iItem.spell - (getCir * 10) + 1;
                    if (!Magic->CheckBook(getCir, getSpell - 1, spellBook)) {
                        canMake = false;
                    }
                }
            }
            if (canMake) {
                toSend.addCommand(util::format("button %i %i %i %i 1 0 %i", xLoc - 40, yLoc, btnRight, btnRight + 1, (*ourMenu.iIter)));
                if (iItem.targId) {
                    toSend.addCommand(util::format("tilepic %i %i %i", xLoc - 20, yLoc, iItem.targId));
                }
                toSend.addCommand(util::format("text %i %i 35 %i", xLoc + 20, yLoc, textCounter++));
                toSend.addText(iItem.name);
                yLoc += 40;
                ++actualItems;
            }
        }
    }
    
    actualItems = 0;
    for (ourMenu.mIter = ourMenu.menuEntries.begin(); ourMenu.mIter != ourMenu.menuEntries.end();
         ++ourMenu.mIter) {
        if ((actualItems % 6) == 0 && actualItems != 0) {
            xLoc += 180;
            yLoc = 40;
        }
        smIter = skillMenus.find((*ourMenu.mIter));
        if (smIter != skillMenus.end()) {
            CreateMenuEntry_st iMenu = smIter->second;
            toSend.addCommand(util::format("button %i %i %i %i 1 0 %i", xLoc - 40, yLoc, btnRight, btnRight + 1, CREATE_MENU_OFFSET + (*ourMenu.mIter)));
            if (iMenu.targId) {
                toSend.addCommand(util::format("tilepic %i %i %i", xLoc - 20, yLoc, iMenu.targId));
            }
            toSend.addCommand(util::format("text %i %i 35 %i", xLoc + 20, yLoc, textCounter++));
            toSend.addText(iMenu.name);
            yLoc += 40;
            ++actualItems;
        }
    }
    toSend.Finalize();
    s->Send(&toSend);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::HandleMakeMenu()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles the button pressed in the new make menu
// o------------------------------------------------------------------------------------------------o
void CSkills::HandleMakeMenu(CSocket *s, std::int32_t button, std::int32_t menu) {
    VALIDATESOCKET(s);
    CChar *ourChar = s->CurrcharObj();
    s->AddId(0);
    std::map<std::uint16_t, CreateMenu_st>::const_iterator p = actualMenus.find(menu);
    if (p == actualMenus.end())
        return;
    
    CreateMenu_st ourMenu = p->second;
    if (button >= CREATE_MENU_OFFSET) {// menu pressed
        std::map<std::uint16_t, CreateMenuEntry_st>::const_iterator q = skillMenus.find(button - CREATE_MENU_OFFSET);
        if (q == skillMenus.end())
            return;
        
        NewMakeMenu(s, q->second.subMenu, 0);
    }
    else {// item to make
        std::map<std::uint16_t, CreateEntry_st>::iterator r = itemsForMenus.find(button);
        if (r == itemsForMenus.end())
            return;
        
        MakeItem(r->second, ourChar, s, button);
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::FindItem()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a handle to a CreateEntry_st based on an itemNum
// o------------------------------------------------------------------------------------------------o
CreateEntry_st *CSkills::FindItem(std::uint16_t itemNum) {
    std::map<std::uint16_t, CreateEntry_st>::iterator r = itemsForMenus.find(itemNum);
    if (r == itemsForMenus.end())
        return nullptr;
    
    return &(r->second);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::MakeItem()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Makes an item selected in the new make menu system
// o------------------------------------------------------------------------------------------------o
void CSkills::MakeItem(CreateEntry_st &toMake, CChar *player, CSocket *sock, std::uint16_t itemEntry, std::uint16_t resourceColour) {
    VALIDATESOCKET(sock);
    
    if (!ValidateObject(player))
        return;
    
    // Get potential tag with ID of a targeted sub-resource
    std::uint16_t targetedSubResourceId = 0;
    auto tempTagObj = player->GetTempTag("targetedSubResourceId");
    if (tempTagObj.m_ObjectType == TagMap::TAGMAP_TYPE_INT && tempTagObj.m_IntValue > 0) {
        targetedSubResourceId = static_cast<std::uint16_t>(tempTagObj.m_IntValue);
    }
    
    std::vector<ResAmountPair_st>::const_iterator resCounter;
    std::vector<ResSkillReq_st>::const_iterator sCounter;
    ResAmountPair_st resEntry;
    std::uint16_t toDelete;
    std::uint16_t targColour;
    std::uint16_t targId;
    std::uint32_t targMoreVal;
    bool canDelete = true;
    
    // Moved resource-check to top of file to disallow gaining skill by attempting to
    // craft items without having enough resources to do so :P
    for (resCounter = toMake.resourceNeeded.begin(); resCounter != toMake.resourceNeeded.end(); ++resCounter) {
        resEntry = (*resCounter);
        toDelete = resEntry.amountNeeded;
        targColour = resEntry.colour;
        targMoreVal = resEntry.moreVal;
        
        if (resCounter == toMake.resourceNeeded.begin() && resourceColour != 0) {
            // If a specific colour was provided for the function as the intended colour of the
            // material, use a material of that colour
            targColour = resourceColour;
        }
        for (std::vector<std::uint16_t>::const_iterator idCounter = resEntry.idList.begin(); idCounter != resEntry.idList.end(); ++idCounter) {
            targId = (*idCounter);
            if (targetedSubResourceId == 0 || resCounter == toMake.resourceNeeded.begin()) {
                // Primary resource, or generic subresource
                toDelete -= std::min(GetItemAmount(player, targId, targColour, targMoreVal, true), static_cast<std::uint32_t>(toDelete));
            }
            else {
                if (targetedSubResourceId > 0 && targId == targetedSubResourceId) {
                    // Player specifically targeted a secondary resource
                    toDelete -= std::min(GetItemAmount(player, targId, targColour, targMoreVal, true), static_cast<std::uint32_t>(toDelete));
                }
            }
            
            if (toDelete == 0)
                break;
        }
        if (toDelete > 0) {
            canDelete = false;
        }
    }
    if (!canDelete) {
        sock->SysMessage(1651); // You have insufficient resources on hand to do that
        return;
    }
    
    bool canMake = true;
    
    // Loop through the skills listed as skill requirement for crafting the item
    for (sCounter = toMake.skillReqs.begin(); sCounter != toMake.skillReqs.end(); ++sCounter) {
        if (player->SkillUsed((*sCounter).skillNumber)) {
            sock->SysMessage(1650); // You are already using that skill
            return;
        }
        
        if (sCounter == toMake.skillReqs.begin()) {
            // Only perform skill check for first and main skill in skill requirement
            if (!CheckSkill(player, (*sCounter).skillNumber, (*sCounter).minSkill, (*sCounter).maxSkill, true)) {
                canMake = false;
                break;
            }
        }
        else {
            // For supporting skills, we only check if player has more than or equal to the minimum
            // skill required
            if (player->GetSkill((*sCounter).skillNumber) < (*sCounter).minSkill) {
                canMake = false;
                break;
            }
        }
    }
    
    if (!canMake) {
        // delete anywhere up to half of the resources needed
        if (toMake.soundPlayed) {
            Effects->PlaySound(sock, toMake.soundPlayed, true);
        }
        sock->SysMessage(984); // You fail to create the item.
    }
    else {
        // Store temp tag on player with colour of item to craft
        TagMap tagObject;
        tagObject.m_Destroy = false;
        tagObject.m_StringValue = "";
        tagObject.m_IntValue = resourceColour;
        tagObject.m_ObjectType = TagMap::TAGMAP_TYPE_INT;
        player->SetTempTag("craftItemColor", tagObject);
        
        for (sCounter = toMake.skillReqs.begin(); sCounter != toMake.skillReqs.end(); ++sCounter) {
            player->SkillUsed(true, (*sCounter).skillNumber);
        }
        
        Effects->TempEffect(player, player, 41, toMake.delay, itemEntry, 0);
        if (toMake.soundPlayed) {
            if (toMake.delay > 300) {
                for (std::int16_t i = 0; i < (toMake.delay / 300); ++i) {
                    Effects->TempEffect(player, player, 42, 300 * i, toMake.soundPlayed, 0);
                }
            }
        }
    }
    
    if (!canMake || !canDelete) {
        // Trigger onMakeItem() JS event for character who tried to craft item, even though they
        // failed
        std::vector<std::uint16_t> scriptTriggers = player->GetScriptTriggers();
        for (auto scriptTrig : scriptTriggers) {
            cScript *toExecute = JSMapping->GetScript(scriptTrig);
            if (toExecute != nullptr) {
                toExecute->OnMakeItem(sock, player, nullptr, itemEntry);
            }
        }
    }
    for (resCounter = toMake.resourceNeeded.begin(); resCounter != toMake.resourceNeeded.end(); ++resCounter) {
        resEntry = (*resCounter);
        toDelete = resEntry.amountNeeded;
        if (!canMake) {
            toDelete = RandomNum(0, std::max(1, toDelete / 2));
        }
        targColour = resEntry.colour;
        targMoreVal = resEntry.moreVal;
        
        if (resCounter == toMake.resourceNeeded.begin() && resourceColour != 0) {
            // If a specific colour was provided for the function as the intended colour of the
            // material, use a material of that colour
            targColour = resourceColour;
        }
        for (std::vector<std::uint16_t>::const_iterator idCounter = resEntry.idList.begin(); idCounter != resEntry.idList.end(); ++idCounter) {
            targId = (*idCounter);
            if (targetedSubResourceId == 0 || resCounter == toMake.resourceNeeded.begin()) {
                // Primary resource, or generic subresource
                toDelete -= DeleteItemAmount(player, toDelete, targId, targColour, targMoreVal);
            }
            else {
                if (targetedSubResourceId > 0 && targId == targetedSubResourceId) {
                    // Player specifically targeted a secondary resource
                    toDelete -= DeleteItemAmount(player, toDelete, targId, targColour, targMoreVal);
                }
            }
            
            if (toDelete == 0)
                break;
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::RepairMetal()
//|	Date		-	October 16, 2000
//|	Modified	-	November 13, 2001 - changed to a metal repair function
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Repair armor and weapons.
// o------------------------------------------------------------------------------------------------o
void CSkills::RepairMetal(CSocket *s) {
    std::int16_t minSkill = 999, maxSkill = 1000;
    
    CChar *mChar = s->CurrcharObj();
    // Check if item used to initialize target cursor is still within range
    CItem *tempObj = static_cast<CItem *>(s->TempObj());
    s->TempObj(nullptr);
    if (ValidateObject(tempObj)) {
        if (tempObj->IsHeldOnCursor() || !CheckItemRange(mChar, tempObj)) {
            s->SysMessage(400); // That is too far away!
            return;
        }
    }
    
    CItem *j = CalcItemObjFromSer(s->GetDWord(7));
    if (!ValidateObject(j) || !j->IsMetalType()) {
        s->SysMessage(986); // You cannot repair that.
        return;
    }
    
    if (j->IsHeldOnCursor() || !CheckItemRange(mChar, j)) {
        s->SysMessage(400); // That is too far away!
        return;
    }
    
    if (j->GetHP() < j->GetMaxHP()) {
        if (j->GetResist(Weather::PHYSICAL) > 0) {
            // Items with > 12 def would have impossible skill req's, with this equation
            if (j->GetResist(Weather::PHYSICAL) <= 12) {
                // Minimum Skill = 61.0 + Defense - 1 / 3 * 100 (0-3 = 61.0, 4-6 = 71.0, ect)
                minSkill = (610 + static_cast<std::int32_t>((j->GetResist(Weather::PHYSICAL) - 1) / 3) * 100);
                // Maximum Skill = 84.9 + Defense - 1 / 3 * 50 (0-3 = 84.9, 4-6 = 89.9, ect)
                maxSkill = (849 + static_cast<std::int32_t>((j->GetResist(Weather::PHYSICAL) - 1) / 3) * 50);
            }
        }
        else if (j->GetHiDamage() > 0) {
            std::int32_t offset = (j->GetLoDamage() + j->GetHiDamage()) / 2;
            // Items with > 25 Avg Damage would have impossible skill req's, with this equation
            if (offset <= 25) {
                // Minimum Skill = 51.0 + Avg Damage - 1 / 5 * 100 (0-5 = 51.0, 6-10 = 61.0, ect)
                minSkill = (510 + static_cast<std::int32_t>((offset - 1) / 5) * 100);
                // Maximum Skill = 79.9 + Avg Damage - 1 / 5 * 50 (0-5 = 79.9, 6-10 = 84.9, ect)
                maxSkill = (799 + static_cast<std::int32_t>((offset - 1) / 5) * 50);
            }
        }
        else {
            s->SysMessage(987); // You cannot repair this item.
            return;
        }
    }
    else {
        s->SysMessage(988); // That item is already fully repaired.
        return;
    }
    if (CheckSkill(mChar, BLACKSMITHING, minSkill, maxSkill)) {
        j->SetHP(j->GetMaxHP());
        s->SysMessage(989); // You repair the item successfully.
        Effects->PlaySound(s, 0x002A, true);
    }
    else {
        s->SysMessage(990); // You fail to repair the item.
    }
}

void CallGuards(CChar *mChar, CChar *targChar);
// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::Snooping()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Called when player snoops another PC/NPC's or a tamed animals pack
//|					Snooping functionality now handled by skill script:
// js/skill/snooping.js
// o------------------------------------------------------------------------------------------------o
void CSkills::Snooping(CSocket *s, CChar *target, CItem *pack) {
    CChar *mChar = s->CurrcharObj();
    [[maybe_unused]] CSocket *tSock = target->GetSocket();
    
    std::vector<std::uint16_t> scriptTriggers = mChar->GetScriptTriggers();
    for (auto scriptTrig : scriptTriggers) {
        cScript *toExecute = JSMapping->GetScript(scriptTrig);
        if (toExecute != nullptr) {
            // If script returns false, prevent hard-code and other scripts with event from running
            if (toExecute->OnSnoopAttempt(target, pack, mChar) == 0) {
                return;
            }
        }
    }
    
    // Check for global script version of event
    cScript *toExecute = JSMapping->GetScript(static_cast<std::uint16_t>(0));
    if (toExecute != nullptr) {
        if (toExecute->OnSnoopAttempt(target, pack, mChar) == 0) {
            return;
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::MakeNecroReg()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Create reagents for necromancer spells
// o------------------------------------------------------------------------------------------------o
void CSkills::MakeNecroReg(CSocket *nSocket, CItem *nItem, std::uint16_t itemId) {
    CItem *iItem = nullptr;
    CChar *iCharId = nSocket->CurrcharObj();
    
    if (itemId >= 0x1B11 && itemId <= 0x1B1C) {// Make bone powder.
        iCharId->TextMessage(nullptr, 741, EMOTE, 1, iCharId->GetName().c_str()); // %s is grinding some bone into powder
        Effects->TempEffect(iCharId, iCharId, 9, 0, 0, 0);
        Effects->TempEffect(iCharId, iCharId, 9, 0, 3, 0);
        Effects->TempEffect(iCharId, iCharId, 9, 0, 6, 0);
        Effects->TempEffect(iCharId, iCharId, 9, 0, 9, 0);
        iItem = Items->CreateItem(nSocket, iCharId, 0x0F8F, 1, 0, CBaseObject::OT_ITEM, true);
        if (iItem == nullptr)
            return;
        
        iItem->SetName("bone powder");
        iItem->SetTempVar(CITV_MOREX, 666);
        iItem->SetTempVar(CITV_MORE, 1, 1); // this will fill more with info to tell difference between ash and bone
        nItem->Delete();
    }
    if (itemId == 0x0E24) { // Make vial of blood.
        if (nItem->GetTempVar(CITV_MORE, 1) == 1) {
            iItem = Items->CreateItem(nSocket, iCharId, 0x0F82, 1, 0, CBaseObject::OT_ITEM, true);
            if (iItem == nullptr)
                return;
            
            iItem->SetBuyValue(15);
            iItem->SetTempVar(CITV_MOREX, 666);
        }
        else {
            iItem = Items->CreateItem(nSocket, iCharId, 0x0F7D, 1, 0, CBaseObject::OT_ITEM, true);
            if (iItem == nullptr)
                return;
            
            iItem->SetBuyValue(10);
            iItem->SetTempVar(CITV_MOREX, 666);
        }
        nItem->IncAmount(-1);
    }
}
