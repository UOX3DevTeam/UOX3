
#include <algorithm>

#include "cchar.h"
#include "ceffects.h"
#include "citem.h"
#include "cjsmapping.h"
#include "classes.h"
#include "commands.h"
#include "subsystem/console.hpp"
#include "cpacketsend.h"
#include "cscript.h"
#include "configuration/serverconfig.hpp"
#include "cserverdefinitions.h"
#include "csocket.h"
#include "funcdecl.h"
#include "objectfactory.h"
#include "townregion.h"

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CalcValue()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate the value of an item
// o------------------------------------------------------------------------------------------------o
std::uint32_t CalcValue(CItem *i, std::uint32_t value) {
    if (i->GetType() == IT_POTION) {
        std::uint32_t mod = 10;
        if (i->GetTempVar(CITV_MOREX) > 500) {
            ++mod;
        }
        if (i->GetTempVar(CITV_MOREX) > 900) {
            ++mod;
        }
        if (i->GetTempVar(CITV_MOREX) > 1000) {
            ++mod;
        }
        if (i->GetTempVar(CITV_MOREZ) > 1) {
            mod += (3 * (i->GetTempVar(CITV_MOREZ) - 1));
        }
        value = (value * mod) / 10;
    }
    
    if (i->GetRank() > 0 && i->GetRank() < 10 && ServerConfig::shared().enabled(ServerSwitch::RANKSYSTEM)) {
        value = static_cast<std::uint32_t>(i->GetRank() * value) / 10;
    }
    if (value < 1) {
        value = 1;
    }
    
    if (!i->GetRndValueRate()) {
        i->SetRndValueRate(0);
    }
    if (i->GetRndValueRate() != 0 && ServerConfig::shared().enabled(ServerSwitch::TRADESYSTEM)) {
        value += static_cast<std::uint32_t>(value * i->GetRndValueRate()) / 1000;
    }
    if (value < 1) {
        value = 1;
    }
    return value;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CalcGoodValue()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate the value of a good
// o------------------------------------------------------------------------------------------------o
std::uint32_t CalcGoodValue(CTownRegion *tReg, CItem *i, std::uint32_t value, bool isSelling) {
    if (tReg == nullptr)
        return value;
    
    std::int16_t good = i->GetGood();
    std::int32_t regvalue = 0;
    
    if (good <= -1) {
        return value;
    }
    
    if (isSelling) {
        regvalue = tReg->GetGoodSell(static_cast<std::uint8_t>(i->GetGood()));
    }
    else {
        regvalue = tReg->GetGoodBuy(static_cast<std::uint8_t>(i->GetGood()));
    }
    
    std::uint32_t x = static_cast<std::uint32_t>(value * abs(regvalue)) / 1000;
    
    if (regvalue < 0) {
        value -= x;
    }
    else {
        value += x;
    }
    
    if (value < 1) {
        value = 1;
    }
    
    return value;
}

bool ApplyItemSection(CItem *applyTo, CScriptSection *toApply, std::string sectionId);
// o------------------------------------------------------------------------------------------------o
//|	Function	-	CPIBuyItem::Handle()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Called when player buys an item from a vendor
// o------------------------------------------------------------------------------------------------o
bool CPIBuyItem::Handle() {
    std::uint16_t i;
    std::uint32_t totalPlayerGold = 0, totalGoldCost = 0;
    bool soldout = false, clear = false;
    CChar *mChar = tSock->CurrcharObj();
    CItem *p = mChar->GetPackItem();
    if (!ValidateObject(p))
        return true;
    
    std::vector<CItem *> bItems;
    std::vector<std::uint8_t> layer;
    std::vector<std::uint16_t> amount;
    
    // vector for storing all objects that actually end up in user backpack
    std::vector<CItem *> boughtItems;
    std::vector<std::uint16_t> boughtItemAmounts;
    
    CChar *npc = CalcCharObjFromSer(tSock->GetDWord(3));
    std::uint16_t itemTotal = static_cast<std::uint16_t>((tSock->GetWord(1) - 8) / 7);
    if (itemTotal > 511)
        return true;
    
    boughtItems.reserve(itemTotal);
    boughtItemAmounts.reserve(itemTotal);
    bItems.resize(itemTotal);
    amount.resize(itemTotal);
    layer.resize(itemTotal);
    std::int32_t baseOffset = 0;
    for (i = 0; i < itemTotal; ++i) {
        baseOffset = 7 * i;
        layer[i] = tSock->GetByte(8 + static_cast<size_t>(baseOffset));
        bItems[i] = CalcItemObjFromSer(tSock->GetDWord(9 + static_cast<size_t>(baseOffset)));
        amount[i] = tSock->GetWord(13 + static_cast<size_t>(baseOffset));
        totalGoldCost += (amount[i] * (bItems[i]->GetBuyValue()));
    }
    
    bool didUseBank = true;
    bool tryUsingBank = (totalGoldCost >= static_cast<std::uint32_t>( ServerConfig::shared().shortValues[ShortValue::BANKBUYTHRESHOLD]));
    if (tryUsingBank) {
        // Count gold in bank if amount is higher than threshold
        totalPlayerGold = GetBankCount(mChar, 0x0EED);
    }
    
    if (!tryUsingBank || totalPlayerGold < totalGoldCost) {
        // Count gold in backpack if amount is NOT higher than threshold,
        // or if bank doesn't have enough gold
        totalPlayerGold = GetItemAmount(mChar, 0x0EED);
        didUseBank = false;
    }
    
    if (mChar->IsGM() || ((tryUsingBank && totalPlayerGold >= totalGoldCost) || (!tryUsingBank && totalPlayerGold >= totalGoldCost))) {
        for (i = 0; i < itemTotal; ++i) {
            if (bItems[i]->GetAmount() < amount[i]) {
                soldout = true;
            }
            
            // Check if onBuyFromVendor JS event is present for each item being purchased
            // If return false or 0 has been returned from the script, halt the purchase
            for (auto scriptTrig : bItems[i]->GetScriptTriggers()) {
                cScript *toExecute = JSMapping->GetScript(scriptTrig);
                if (toExecute != nullptr) {
                    if (toExecute->OnBuyFromVendor(tSock, npc, bItems[i], amount[i]) == 0) {
                        bItems.clear();
                        bItems.shrink_to_fit();
                        return true;
                    }
                }
            }
            
            // Also run the event on the vendor itself. If purchase didn't get blocked on the item
            // side, maybe vendor has a script with something to say about it instead
            for (auto scriptTrig : npc->GetScriptTriggers()) {
                cScript *toExecute = JSMapping->GetScript(scriptTrig);
                if (toExecute != nullptr) {
                    if (toExecute->OnBuyFromVendor(tSock, npc, bItems[i], amount[i]) == 0) {
                        bItems.clear();
                        bItems.shrink_to_fit();
                        return true;
                    }
                }
            }
        }
        if (soldout) {
            npc->TextMessage(tSock, 1336, TALK, false); // Alas, I no longer have all those goods in stock.  Let me
            // know if there is something else thou wouldst buy.
            clear = true;
        }
        else {
            if (mChar->IsGM()) {
                npc->TextMessage(nullptr, 1337, TALK, 0, mChar->GetName().c_str()); // Here you are, %s. Someone as special as thee will
                // receive my wares for free of course.
            }
            else {
                if (totalGoldCost == 1) {
                    npc->TextMessage(nullptr, 1338, TALK, 0, mChar->GetName().c_str(), totalGoldCost); // Here you are, %s. That will be %d gold coin.
                    // I thank thee for thy business.
                }
                else {
                    npc->TextMessage(nullptr, 1339, TALK, 0, mChar->GetName().c_str(),
                                     totalGoldCost); // Here you are, %s. That will be %d gold
                    // coins.  I thank thee for thy business.
                }
                
                Effects->GoldSound(tSock, totalGoldCost);
            }
            
            clear = true;
            if (!mChar->IsGM()) {
                if (didUseBank) {
                    // Remove total amount of gold spent, from player's bankbox
                    DeleteBankItem(mChar, totalGoldCost, 0x0EED);
                }
                else {
                    // Remove total amount of gold spent, from player's backpack
                    DeleteItemAmount(mChar, totalGoldCost, 0x0EED);
                }
            }
            CItem *biTemp;
            CItem *iMade = nullptr;
            std::uint16_t j;
            for (i = 0; i < itemTotal; ++i) {
                biTemp = bItems[i];
                iMade = nullptr;
                if (biTemp->GetAmount() > amount[i]) {
                    if (biTemp->IsPileable()) {
                        iMade = Items->DupeItem(tSock, biTemp, amount[i]);
                        if (iMade != nullptr) {
                            iMade->SetCont(p);
                            iMade->PlaceInPack();
                            boughtItems.push_back(iMade);
                            boughtItemAmounts.push_back(amount[i]);
                        }
                    }
                    else {
                        for (j = 0; j < amount[i]; ++j) {
                            iMade = Items->DupeItem(tSock, biTemp, 1);
                            if (iMade != nullptr) {
                                CItem *biTempCont = static_cast<CItem *>(biTemp->GetCont());
                                if (biTempCont->GetLayer() == 0x1A) {
                                    // Only do this for new items sold from vendor's sell container
                                    std::int16_t iMadeHP = iMade->GetHP();
                                    std::int16_t iMadeMaxHP = static_cast<std::int16_t>(iMade->GetMaxHP());
                                    if (iMadeMaxHP > iMadeHP) {
                                        // Randomize the item's HP from HP to MaxHP
                                        iMade->SetHP(static_cast<std::int16_t>(RandomNum(iMadeHP, iMadeMaxHP)));
                                    }
                                }
                                iMade->SetCont(p);
                                iMade->PlaceInPack();
                                boughtItems.push_back(iMade);
                                boughtItemAmounts.push_back(1);
                            }
                        }
                    }
                    biTemp->IncAmount(-amount[i], true);
                    biTemp->SetRestock(biTemp->GetRestock() + amount[i]);
                }
                else {
                    CItem *biTempCont = static_cast<CItem *>(biTemp->GetCont());
                    switch (biTempCont->GetLayer()) {
                        case 0x1A: // Sell Container
                            if (biTemp->IsPileable()) {
                                iMade = Items->DupeItem(tSock, biTemp, amount[i]);
                                if (iMade != nullptr) {
                                    iMade->SetCont(p);
                                    iMade->PlaceInPack();
                                    boughtItems.push_back(iMade);
                                    boughtItemAmounts.push_back(amount[i]);
                                }
                            }
                            else {
                                for (j = 0; j < amount[i]; ++j) {
                                    iMade = Items->DupeItem(tSock, biTemp, 1);
                                    if (iMade != nullptr) {
                                        std::int16_t iMadeHP = iMade->GetHP();
                                        std::int16_t iMadeMaxHP = static_cast<std::int16_t>(iMade->GetMaxHP());
                                        if (iMadeMaxHP > iMadeHP) {
                                            // Randomize the item's HP from HP to MaxHP
                                            iMade->SetHP(static_cast<std::int16_t>(RandomNum(iMadeHP, iMadeMaxHP)));
                                        }
                                        iMade->SetCont(p);
                                        iMade->PlaceInPack();
                                        boughtItems.push_back(iMade);
                                        boughtItemAmounts.push_back(1);
                                    }
                                }
                            }
                            biTemp->IncAmount(-amount[i], true);
                            biTemp->SetRestock(biTemp->GetRestock() + amount[i]);
                            break;
                        case 0x1B: // Bought Container
                            if (biTemp->IsPileable()) {
                                boughtItemAmounts.push_back(biTemp->GetAmount());
                                biTemp->SetCont(p);
                                boughtItems.push_back(biTemp);
                            }
                            else {
                                for (j = 0; j < amount[i] - 1; ++j) {
                                    iMade = Items->DupeItem(tSock, biTemp, 1);
                                    if (iMade != nullptr) {
                                        iMade->SetCont(p);
                                        iMade->PlaceInPack();
                                        boughtItems.push_back(iMade);
                                        boughtItemAmounts.push_back(1);
                                    }
                                }
                                biTemp->SetCont(p);
                                biTemp->SetAmount(1);
                                boughtItems.push_back(biTemp);
                                boughtItemAmounts.push_back(1);
                            }
                            break;
                        default:
                            Console::shared().error(" Fallout of switch statement without default. vendor.cpp, buyItem()");
                            break;
                    }
                }
            }
            for (i = 0; i < boughtItems.size(); ++i) {
                if (boughtItems[i]) {
                    for (auto scriptTrig : boughtItems[i]->GetScriptTriggers()) {
                        cScript *toExecute = JSMapping->GetScript(scriptTrig);
                        if (toExecute != nullptr) {
                            // If script returns 1, prevent other scripts with event from running
                            if (toExecute->OnBoughtFromVendor(tSock, npc, boughtItems[i], boughtItemAmounts[i]) == 1) {
                                break;
                            }
                        }
                    }
                    
                    for (auto scriptTrig : npc->GetScriptTriggers()) {
                        cScript *toExecute = JSMapping->GetScript(scriptTrig);
                        if (toExecute != nullptr) {
                            // If script returns 1, prevent other scripts with event from running
                            if (toExecute->OnBoughtFromVendor(tSock, npc, boughtItems[i],  boughtItemAmounts[i]) == 1) {
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    else {
        npc->TextMessage(nullptr, 1340, TALK, false); // Alas, thou dost not possess sufficient gold for this purchase!
    }
    
    if (clear) {
        CPBuyItem clrSend;
        clrSend.Serial(tSock->GetDWord(3));
        tSock->Send(&clrSend);
    }
    return true;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CPISellItem::Handle()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Player sells an item to the vendor
// o------------------------------------------------------------------------------------------------o
bool CPISellItem::Handle() {
    if (tSock->GetByte(8) != 0) {
        CChar *mChar = tSock->CurrcharObj();
        CChar *n = CalcCharObjFromSer(tSock->GetDWord(3));
        if (!ValidateObject(n) || !ValidateObject(mChar))
            return true;
        
        CItem *buyPack = n->GetItemAtLayer(IL_BUYCONTAINER);
        CItem *boughtPack = n->GetItemAtLayer(IL_BOUGHTCONTAINER);
        CItem *sellPack = n->GetItemAtLayer(IL_SELLCONTAINER);
        if (!ValidateObject(buyPack) || !ValidateObject(sellPack) || !ValidateObject(boughtPack))
            return true;
        
        CItem *j = nullptr, *k = nullptr, *l = nullptr;
        std::uint16_t amt = 0, maxsell = 0;
        std::uint8_t i = 0;
        std::uint32_t totgold = 0, value = 0;
        for (i = 0; i < tSock->GetByte(8); ++i) {
            // j = CalcItemObjFromSer( tSock->GetDWord( 9 + (6 * static_cast<size_t>( i ))));
            amt = tSock->GetWord(13 + (6 * static_cast<size_t>(i)));
            maxsell += amt;
        }
        
        if (maxsell > ServerConfig::shared().shortValues[ShortValue::MAXSELLITEM] ) {
            n->TextMessage(nullptr, 1342, TALK, 0, mChar->GetName().c_str(), ServerConfig::shared().shortValues[ShortValue::MAXSELLITEM]); // Sorry %s, but I can only buy %i items at a time!
            return true;
        }
        
        // Loop through each item being sold
        for (i = 0; i < tSock->GetByte(8); ++i) {
            j = CalcItemObjFromSer(tSock->GetDWord(9 + (6 * static_cast<size_t>(i))));
            amt = tSock->GetWord(13 + (6 * static_cast<size_t>(i)));
            if (ValidateObject(j)) {
                if (j->GetAmount() < amt || FindItemOwner(j) != mChar) {
                    n->TextMessage(nullptr, 1343, TALK, false); // Cheating scum! Leave now, before I call the guards!
                    return true;
                }
                
                // Check if onSellToVendor JS event is present for item being sold
                // If present and a value of "false" or 0 was returned from the script, halt the
                // sale
                bool saleHalted = false;
                std::vector<std::uint16_t> jScriptTriggers = j->GetScriptTriggers();
                for (auto scriptTrig : jScriptTriggers) {
                    cScript *toExecute = JSMapping->GetScript(scriptTrig);
                    if (toExecute != nullptr) {
                        if (toExecute->OnSellToVendor(tSock, n, j, maxsell) == 0) {
                            // Halt sale!
                            saleHalted = true;
                            break;
                        }
                    }
                }
                
                // Also check for same event on vendor itself, for broader usecases that doesn't
                // require scripts on every item sold Only run this check if the sale was not halted
                // by a script on the item itself, though
                if (!saleHalted) {
                    std::vector<std::uint16_t> nScriptTriggers = n->GetScriptTriggers();
                    for (auto scriptTrig : nScriptTriggers) {
                        cScript *toExecute = JSMapping->GetScript(scriptTrig);
                        if (toExecute != nullptr) {
                            if (toExecute->OnSellToVendor(tSock, n, j, maxsell) == 0) {
                                // Halt sale!
                                saleHalted = true;
                                break;
                            }
                        }
                    }
                }
                
                // If sale was halted by onSellToVendor event, continue to next item
                if (saleHalted) {
                    continue;
                }
                
                CItem *join = nullptr;
                GenericList<CItem *> *pCont = boughtPack->GetContainsList();
                for (k = pCont->First(); !pCont->Finished(); k = pCont->Next()) {
                    if (ValidateObject(k)) {
                        if (k->GetId() == j->GetId() && j->GetType() == k->GetType()) {
                            if (j->GetId() != 0x14f0 || (j->GetTempVar(CITV_MOREX) == k->GetTempVar(CITV_MOREX))) {
                                join = k;
                                break; // we found the item we're looking for already
                            }
                        }
                    }
                }
                pCont = buyPack->GetContainsList();
                for (k = pCont->First(); !pCont->Finished(); k = pCont->Next()) {
                    if (ValidateObject(k)) {
                        if (k->GetId() == j->GetId() && j->GetType() == k->GetType()) {
                            if (j->GetId() != 0x14f0 || (j->GetTempVar(CITV_MOREX) == k->GetTempVar(CITV_MOREX))) {
                                value = CalcValue(j, k->GetSellValue());
                                break; // we found the value we're looking for already
                            }
                        }
                    }
                }
                
                // If an object already exist in the boughtPack that this one can be joined to...
                if (ValidateObject(join)) {
                    join->IncAmount(amt);
                    join->SetRestock(join->GetRestock() - amt);
                    l = join;
                    
                    totgold += (amt * value);
                    if (j->GetAmount() == amt) {
                        j->Delete();
                    }
                    else {
                        j->IncAmount(-amt);
                    }
                }
                else {
                    // Otherwise, move this item to the vendor's boughtPack
                    totgold += (amt * value);
                    
                    if (j->GetAmount() != amt) {
                        l = Items->DupeItem(tSock, j, amt);
                        j->SetAmount(j->GetAmount() - amt);
                    }
                    else {
                        l = j;
                    }
                    
                    if (ValidateObject(l)) {
                        l->SetCont(boughtPack);
                        l->RemoveFromSight();
                    }
                }
                if (l) {
                    // Check for onSoldToVendor event on item
                    std::vector<std::uint16_t> lScriptTriggers = l->GetScriptTriggers();
                    for (auto scriptTrig : lScriptTriggers) {
                        cScript *toExecute = JSMapping->GetScript(scriptTrig);
                        if (toExecute != nullptr) {
                            // If script returns true/1, prevent other scripts with event from
                            // running
                            if (toExecute->OnSoldToVendor(tSock, n, l, amt) == 1) {
                                break;
                            }
                        }
                    }
                    
                    // Check for onSoldToVendor event on vendor
                    std::vector<std::uint16_t> nScriptTriggers = n->GetScriptTriggers();
                    for (auto scriptTrig : nScriptTriggers) {
                        cScript *toExecute = JSMapping->GetScript(scriptTrig);
                        if (toExecute != nullptr) {
                            // If script returns true/1, prevent other scripts with event from
                            // running
                            if (toExecute->OnSoldToVendor(tSock, n, l, amt) == 1) {
                                break;
                            }
                        }
                    }
                }
            }
        }
        
        Effects->GoldSound(tSock, totgold);
        while (totgold > MAX_STACK) {
            Items->CreateScriptItem(tSock, mChar, "0x0EED", MAX_STACK, CBaseObject::OT_ITEM, true);
            totgold -= MAX_STACK;
        }
        if (totgold > 0) {
            Items->CreateScriptItem(tSock, mChar, "0x0EED", totgold, CBaseObject::OT_ITEM, true);
        }
    }
    
    CPBuyItem clrSend;
    clrSend.Serial(tSock->GetDWord(3));
    tSock->Send(&clrSend);
    return true;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	restockNPC()
/// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Restock NPC Vendors
// o------------------------------------------------------------------------------------------------o
void restockNPC(CChar &i, bool stockAll) {
    if (!i.IsShop())
        return; // if we aren't a shopkeeper, why bother?
    
    CItem *ci = i.GetItemAtLayer(IL_SELLCONTAINER);
    if (ValidateObject(ci)) {
        GenericList<CItem *> *ciCont = ci->GetContainsList();
        for (CItem *c = ciCont->First(); !ciCont->Finished(); c = ciCont->Next()) {
            if (ValidateObject(c)) {
                if (stockAll) {
                    c->IncAmount(c->GetRestock());
                    c->SetRestock(0);
                }
                else if (c->GetRestock()) {
                    std::uint16_t stockAmt =
                    std::min(c->GetRestock(), static_cast<std::uint16_t>((c->GetRestock() / 2) + 1));
                    c->IncAmount(stockAmt);
                    c->SetRestock(c->GetRestock() - stockAmt);
                }
                if (ServerConfig::shared().enabled(ServerSwitch::TRADESYSTEM)) {
                    CTownRegion *tReg =
                    CalcRegionFromXY(i.GetX(), i.GetY(), i.WorldNumber(), i.GetInstanceId());
                    Items->StoreItemRandomValue(c, tReg);
                }
            }
        }
    }
}

bool RestockFunctor(CBaseObject *a, std::uint32_t &b, [[maybe_unused]] void *extraData) {
    bool retVal = true;
    CChar *c = static_cast<CChar *>(a);
    if (ValidateObject(c)) {
        restockNPC((*c), (b == 1));
    }
    
    return retVal;
}
// o------------------------------------------------------------------------------------------------o
//|	Function	-	Restock()
//|	Date		-	3/15/2003
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Restock all NPC Vendors
// o------------------------------------------------------------------------------------------------o
void Restock(bool stockAll) {
    std::uint32_t b = (stockAll ? 1 : 0);
    ObjectFactory::shared().IterateOver(CBaseObject::OT_CHAR, b, nullptr, &RestockFunctor);
}
