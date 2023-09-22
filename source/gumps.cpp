#include "cgump.h"

#include <algorithm>
#include <fstream>

#include "cbaseobject.h"
#include "cchar.h"
#include "ceffects.h"
#include "cguild.h"
#include "citem.h"
#include "cjsmapping.h"
#include "classes.h"
#include "cmagic.h"
#include "commands.h"
#include "subsystem/console.hpp"
#include "cpacketsend.h"
#include "cscript.h"
#include "cserverdefinitions.h"
#include "csocket.h"
#include "dictionary.h"
#include "funcdecl.h"
#include "objectfactory.h"
#include "ostype.h"
#include "osunique.hpp"
#include "pagevector.h"
#include "regions.h"
#include "configuration/serverconfig.hpp"
#include "skills.h"
#include "ssection.h"
#include "stringutility.hpp"
#include "utility/strutil.hpp"
#include "townregion.h"
#include "wholist.h"
#include "other/uoxversion.hpp"

extern CDictionaryContainer worldDictionary ;
extern WorldItem worldItem ;

using namespace std::string_literals;

void CollectGarbage();
std::string GetUptime();

// o------------------------------------------------------------------------------------------------o
//|	Function	-	TextEntryGump()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Open entry gump with specified dictionary message and max value
// length
// o------------------------------------------------------------------------------------------------o
void TextEntryGump(CSocket *s, serial_t ser, std::uint8_t type, std::uint8_t index, std::int16_t maxlength, std::int32_t dictEntry) {
    if (s == nullptr)
        return;
    
    std::string txt = worldDictionary.GetEntry(dictEntry, s->Language());
    if (txt.empty()) {
        Console::shared().error("Invalid text in TextEntryGump()");
        return;
    }
    
    auto temp = util::format("(%i chars max)", maxlength);
    CPGumpTextEntry toSend(txt, temp);
    toSend.Serial(ser);
    toSend.ParentId(type);
    toSend.ButtonId(index);
    toSend.Cancel(1);
    toSend.Style(1);
    toSend.Format(maxlength);
    s->Send(&toSend);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	BuildGumpFromScripts()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Pull gump info from misc.dfn
// o------------------------------------------------------------------------------------------------o
void BuildGumpFromScripts(CSocket *s, std::uint16_t m) {
    CPSendGumpMenu toSend;
    toSend.UserId(INVALIDSERIAL);
    
    std::string sect = std::string("GUMPMENU ") + util::ntos(m);
    CScriptSection *gump = FileLookup->FindEntry(sect, misc_def);
    if (gump == nullptr)
        return;
    
    std::uint8_t targType = 0x12;
    std::string tag = gump->First();
    if (util::upper(oldstrutil::extractSection(tag, " ", 0, 0)) == "TYPE") {
        targType = util::ston<std::uint8_t>(oldstrutil::extractSection(tag, " ", 1, 1));
        tag = gump->Next();
    }
    for (; !gump->AtEnd(); tag = gump->Next()) {
        auto temp = util::format("%s %s", tag.c_str(), gump->GrabData().c_str());
        toSend.addCommand(temp);
    }
    sect = std::string("GUMPTEXT ") + util::ntos(m);
    gump = FileLookup->FindEntry(sect, misc_def);
    if (gump == nullptr)
        return;
    
    for (tag = gump->First(); !gump->AtEnd(); tag = gump->Next()) {
        toSend.addText(util::format("%s %s", tag.c_str(), gump->GrabData().c_str()));
    }
    toSend.GumpId(targType);
    toSend.Finalize();
    s->Send(&toSend);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	HandleAccountButton()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles the accounts gump button
// o------------------------------------------------------------------------------------------------o
void HandleAccountButton(CSocket *s, std::uint32_t button, CChar *j) {
    if (!ValidateObject(j))
        return;
    
    CChar *mChar = s->CurrcharObj();
    AccountEntry &actbTemp = Account::shared()[j->GetAccount().accountNumber];
    if (actbTemp.accountNumber == AccountEntry::INVALID_ACCOUNT)
        return;
    
    CSocket *targSocket = j->GetSocket();
    switch (button) {
        case 2:
            if (mChar->IsGM() && !j->IsNpc()) {
                s->SysMessage(487); // You have banned that players account!
                if (targSocket != nullptr) {
                    targSocket->SysMessage(488); // You have been banned!
                }
                actbTemp.flag.set(AccountEntry::attributeflag_t::BANNED, true);
                if (targSocket != nullptr) {
                    Network->Disconnect(targSocket);
                }
            }
            else {
                s->SysMessage(489); // That player cannot be banned!
            }
            break;
        case 3:
            if (mChar->IsGM() && !j->IsNpc()) {
                s->SysMessage(490); // You have given that player's account a time ban!
                if (targSocket != nullptr) {
                    targSocket->SysMessage(491); // You have been banned for 24 hours!
                }
                actbTemp.flag.set(AccountEntry::attributeflag_t::BANNED, true);
                actbTemp.timeBan = GetMinutesSinceEpoch() + static_cast<std::uint32_t>(1440);
                
                if (targSocket != nullptr) {
                    Network->Disconnect(targSocket);
                }
            }
            else {
                s->SysMessage(492); // That player cannot be banned!
            }
            break;
        case 4:
        case 5:
        case 6:
        case 7:
            // (489 + 4) Wipe functionality doesn't exist.
            // (489 + 5) Locking functionality doesn't exist.
            // (489 + 6) Warning functionality doesn't exist.
            // (489 + 7) Enter the new ID number for the item in hex. (???)
            s->SysMessage(489 + button);
            break;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	HandleTownstoneButton()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles button pressed in townstone gump
// o------------------------------------------------------------------------------------------------o
void HandleTownstoneButton(CSocket *s, serial_t button, serial_t ser, serial_t type) {
    CChar *mChar = s->CurrcharObj();
    CTownRegion *targetRegion;
    CTownRegion *ourRegion = worldMain.townRegions[mChar->GetTown()];
    switch (button) {
            // buttons 2-20 are for generic town members
            // buttons 21-40 are for mayoral functions
            // buttons 41-60 are for potential candidates
            // buttons 61-80 are for enemy towns
        case 2: // leave town
            bool result;
            // safe to assume we want to remove ourselves from our only town!
            result = ourRegion->RemoveTownMember((*mChar));
            if (!result) {
                s->SysMessage(540); // There was an error removing you from your town, contact a GM promptly!
            }
            break;
        case 3: // view taxes
            targetRegion = CalcRegionFromXY(mChar->GetX(), mChar->GetY(), mChar->WorldNumber(), mChar->GetInstanceId());
            if (targetRegion != nullptr) {
                targetRegion->ViewTaxes(s);
            }
            else {
                s->SysMessage(541); // Can't view taxes for a town you are not in.
            }
            break;
        case 4: // toggle town title
            mChar->SetTownTitle(!mChar->GetTownTitle());
            ourRegion->DisplayTownMenu(nullptr, s);
            break;
        case 5:
            s->SendTargetCursor(0, TARGET_VOTEFORMAYOR, 0, 542);
            break; // vote for town mayor
        case 6:
            TextEntryGump(s, ser, static_cast<std::uint8_t>(type), static_cast<std::uint8_t>(button), 6, 543);
            break; // gold donation
        case 7:
            ourRegion->ViewBudget(s);
            break; // View Budget
        case 8:
            ourRegion->SendAlliedTowns(s);
            break; // View allied towns
        case 9:
            ourRegion->SendEnemyTowns(s);
            break;
        case 20: // access mayoral functions, never hit here if we don't have mayoral access anyway!
            // also, we'll only get access, if we're in our OWN region
            ourRegion->DisplayTownMenu(nullptr, s, 0x01); // mayor
            break;
        case 21:
            s->SysMessage(544);
            break; // Can't set the taxes yet!
        case 22:
            ourRegion->DisplayTownMembers(s);
            break; // list town members
        case 23:
            ourRegion->ForceEarlyElection();
            break; // force early election
        case 24:
            s->SysMessage(545);
            break; // You cannot purchase more guards at this time.
        case 25:
            s->SysMessage(546);
            break; // You can't fire a guard at the moment.
        case 26:
            s->SendTargetCursor(0, TARGET_TOWNALLY, 0, 547);
            break; // make a town an ally
        case 40:
            ourRegion->DisplayTownMenu(nullptr, s);
            break; // we can't return from mayor menu if we weren't mayor!
        case 41:   // join town!
            if (!(CalcRegionFromXY(mChar->GetX(), mChar->GetY(), mChar->WorldNumber(), mChar->GetInstanceId())->AddAsTownMember((*mChar)))) {
                s->SysMessage(548); // You were unable to be added, contact a GM for further information.
            }
            break;
        case 61:                                                 // seize townstone!
            if (!Skills->CheckSkill(mChar, STEALING, 950, 1000)) {// minimum 95.0 stealing
                targetRegion = CalcRegionFromXY(mChar->GetX(), mChar->GetY(), mChar->WorldNumber(), mChar->GetInstanceId());
                if (targetRegion != nullptr) {
                    std::int32_t chanceToSteal = RandomNum(0, targetRegion->GetHealth() / 2);
                    std::int32_t fudgedStealing = RandomNum(mChar->GetSkill(STEALING),  static_cast<std::uint16_t>(mChar->GetSkill(STEALING) * 2));
                    if (fudgedStealing >= chanceToSteal) {
                        // redo stealing code here
                        s->SysMessage(549, targetRegion->GetName()
                                      .c_str()); // Congrats go to you, for you have successfully
                        // dealt %s a nasty blow this day!
                        targetRegion->DoDamage(targetRegion->GetHealth() / 2); // we reduce the region's health by half
                        for (auto &toCheck : MapRegion->PopulateList(mChar)) {
                            if (toCheck) {
                                CItem *iTownStone = nullptr;
                                auto regItems = toCheck->GetItemList();
                                for (const auto &itemCheck : regItems->collection()) {
                                    if (ValidateObject(itemCheck)) {
                                        if (itemCheck->GetType() == IT_TOWNSTONE && itemCheck->GetId() != 0x14F0) {
                                            // found our townstone
                                            iTownStone = itemCheck;
                                            break;
                                        }
                                    }
                                }
                                
                                if (ValidateObject(iTownStone)) {
                                    auto charPack = mChar->GetPackItem();
                                    if (ValidateObject(charPack)) {
                                        iTownStone->SetCont(charPack);
                                        iTownStone->SetTempVar(CITV_MOREX, targetRegion->GetRegionNum());
                                        s->SysMessage(550); // Quick, make it back to your town with the stone to
                                        // deal the death blow to this region!
                                        targetRegion->TellMembers(551, mChar->GetName().c_str()); // Quickly, %s has stolen
                                        // your treasured townstone!
                                        return;                             // dump out
                                    }
                                }
                            }
                        }
                    }
                    else {
                        s->SysMessage(552); // Try as you might, you have not the ability to seize the day.
                    }
                }
                else {
                    s->SysMessage(553); // This is not a valid region!
                }
            }
            else {
                s->SysMessage(554); // You have insufficient ability to seize the townstone.
            }
            break;
        case 62: // attack townstone
            targetRegion = CalcRegionFromXY(mChar->GetX(), mChar->GetY(), mChar->WorldNumber(),
                                            mChar->GetInstanceId());
            for (std::uint8_t counter = 0; counter < RandomNum(5, 10); ++counter) {
                Effects->PlayMovingAnimation(mChar, mChar->GetX() + RandomNum(-6, 6), mChar->GetY() + RandomNum(-6, 6), mChar->GetZ(), 0x36E4, 17, 0, (RandomNum(0, 1) == 1));
                Effects->PlayStaticAnimation(mChar->GetX() + RandomNum(-6, 6), mChar->GetY() + RandomNum(-6, 6), mChar->GetZ(), 0x373A + RandomNum(0, 4) * 0x10, 0x09, 0, 0);
            }
            targetRegion->DoDamage(RandomNum(0, targetRegion->GetHealth() / 8)); // we reduce the region's health by half
            break;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	HandleAccountModButton()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles button pressed in account gump
// o------------------------------------------------------------------------------------------------o
void HandleAccountModButton(CPIGumpMenuSelect *packet) {
    CSocket *s = packet->GetSocket();
    
    std::string username = "";
    std::string password = "";
    std::string emailAddy = "";
    
    for (std::uint32_t i = 0; i < packet->TextCount(); ++i) {
        std::uint16_t textId = packet->GetTextId(i);
        switch (textId) {
            case 1000:
                username = packet->GetTextString(i);
                break;
            case 1001:
                password = packet->GetTextString(i);
                break;
            case 1002:
                emailAddy = packet->GetTextString(i);
                break;
            default:
                Console::shared().warning(util::format("Unknown textId %i with string %s", textId, packet->GetTextString(i).c_str()));
        }
    }
    
    AccountEntry &actbAccountFind = Account::shared()[username];
    if (actbAccountFind.accountNumber != AccountEntry::INVALID_ACCOUNT) {
        s->SysMessage(555); // An account by that name already exists!
        return;
    }
    Console::shared().print(util::format("Attempting to add username %s with password %s at emailaddy %s", username.c_str(), password.c_str(), emailAddy.c_str()));
}

// o------------------------------------------------------------------------------------------------o
//|	Function		-	BuildAddMenuGump()
// o------------------------------------------------------------------------------------------------o
//|	Purpose			-	Menu for item creation based on ITEMMENU entries in the
// ItemMenu DFNs
//|
//|	Modification	-	04042004 - Added support for the new Auto-AddMenu
//|									items if they exist. Remember the
//multimap is groupId based
//|									so it should order it accordingly we
//only have to look for |									the
// entries.
// o------------------------------------------------------------------------------------------------o
void BuildAddMenuGump(CSocket *s, std::uint16_t m) {
    std::uint32_t pagenum = 1, position = 40, linenum = 1, buttonnum = 7;
    std::uint8_t i = 0;
    CChar *mChar = s->CurrcharObj();
    
    if (!mChar->IsGM() && m > 990 && m < 999) { // 990 - 999 reserved for online help system
        s->SysMessage(337); // Unrecognized command.
        return;
    }
    
#if defined(UOX_DEBUG_MODE)
    Console::shared() << "Menu: " << m << myendl;
#endif
    
    CPSendGumpMenu toSend;
    toSend.UserId(INVALIDSERIAL);
    toSend.GumpId(9);
    
    ////////////////////////////////////////////////
    s->TempInt(m); // Store what menu they are in
    
    std::string sect = std::string("ITEMMENU ") + util::ntos(m);
    CScriptSection *ItemMenu = FileLookup->FindEntry(sect, items_def);
    if (ItemMenu == nullptr)
        return;
    
    // page header
    toSend.addCommand("nodispose");
    toSend.addCommand("page 0");
    
    // Need more control over this aspect of uox3 from outside some how.. first step to get the
    // variables more flexible. later we will emporer them to a template, or a script or something
    std::uint16_t xStart = 0, xWidth = 680;
    std::uint16_t yStart = 0, yWidth = 420;
    
    std::uint32_t bgImage = static_cast<std::uint32_t>(ServerConfig::shared().ushortValues[UShortValue::BACKGROUNDPIC]) ;
    
    // Set and resize the gumps background image.
    toSend.addCommand(util::format("resizepic %u %u %u %u %u", xStart, yStart, bgImage, xWidth, yWidth));
    toSend.addCommand(util::format("checkertrans %u %u %u %u", xStart + 5, yStart + 5, xWidth - 10, yWidth - 11));
    
    // Grab the first tag/value pair from the gump itemmenu respectivly
    std::string tag = ItemMenu->First();
    std::string data = ItemMenu->GrabData();
    toSend.addCommand(util::format("resizepic %u %u %u %u %u", 2, 4, 0xDAC, 675, 40));
    // Create the text stuff for what appears to be the title of the gump. This appears to change
    // from page to page.
    
    // Next we create and position the close window button as well set its Down, and Up states.
    toSend.addCommand(util::format("button %u %u %u %u %u %u %u", xWidth - 32, yStart + 10, 0xA51, 0xA50, 1, 0, 1));
    
    linenum = 0;
    toSend.addCommand(util::format("text %u %u %u %u", 30, yStart + 13, 39, linenum++));
    toSend.addText("Players: ");
    // Player count
    auto szBuffer = util::format("%4i ", worldMain.GetPlayersOnline());
    toSend.addCommand(util::format("text %u %u %u %u", 80, yStart + 13, 25, linenum++));
    toSend.addText(szBuffer);
    // Gm Pages
    toSend.addCommand(util::format("text %u %u %u %u", 118, yStart + 13, 39, linenum++));
    toSend.addText("GM Pages: ");
    szBuffer = "0";
    toSend.addCommand(util::format("text %u %u %u %u", 200, yStart + 13, 25, linenum++));
    toSend.addText(szBuffer);
    // Current Time/Date
    toSend.addCommand(util::format("text %u %u %u %u", 230, yStart + 13, 39, linenum++));
    toSend.addText("Time: ");
    // Current server time
    auto timet = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm ttoday;
    auto today = lcltime(timet, ttoday);
    char tmpBuffer[200];
    bool isAM = true;
    strftime(tmpBuffer, 128, "%b %d, %Y", today);
    if (today->tm_hour > 12) {
        isAM = false;
        today->tm_hour -= 12;
    }
    else if (today->tm_hour == 0) {
        isAM = true;
        today->tm_hour = 12;
    }
    // Draw the current date to the gump
    char tbuffer[100];
    szBuffer = util::format("%s %.8s%s", tmpBuffer, asciitime(tbuffer, 100, *today) + 11, ((isAM) ? "Am" : "Pm"));
    toSend.addCommand(util::format("text %u %u %u %u", 280, yStart + 13, 25, linenum++));
    toSend.addText(szBuffer);
    
    // add the next gump portion. New server level services, in the form of a gump Configuration,
    // and Accounts tabs to start. These are default tabs
    toSend.addCommand(util::format("resizepic %u %u %u %u %u", xStart + 10, yStart + 62, 5054, 190, 340));
    std::uint32_t tabNumber = 1;
    
    // Do the shard tab
    toSend.addCommand(util::format("button %u %u %u %u %u %u %u", 17, yStart + 47, 0x138E, 0x138F, 0, 1, 0));
    szBuffer = "Objects";
    toSend.addCommand(util::format("text %u %u %u %u", 42, yStart + 46, 47, linenum++));
    toSend.addText(szBuffer);
    // Do the server tab
    toSend.addCommand(util::format("button %u %u %u %u %u %u %u", 105, yStart + 47, 0x138E, 0x138F, 0, 30, 1));
    toSend.addCommand(util::format("text %u %u %u %u", 132, yStart + 46, 47, linenum++));
    toSend.addText("Settings");
    
    // Need a seperator
    toSend.addCommand(util::format("gumppictiled %u %u %u %u %u", xStart + 22, yWidth - 50, 165, 5, 0x2393));
    
    // Ok, now the job of pulling the rest of the first itemmenu information and making tabs for
    // them
    toSend.addCommand(util::format("button %u %u %u %u %u %u %u", 65, yWidth - 40, 0x4B9, 0x4BA, 1, 0, 50002));
    toSend.addCommand(util::format("croppedtext %u %u %u %u %u %u", 85, yWidth - 42, 150, 20, 94, linenum++));
    toSend.addText("Home");
    
    // Ok here we have some conditions that we need to filter. First being the menu called.
    std::uint32_t xOffset;
    std::uint32_t yOffset;
#define SXOFFSET 210
#define SYOFFSET 44
#define YOFFSET 110
#define XOFFSET 110
    if (m == 1) {
        // Now we make the first page that will escentially display our list of Main GM Groups.
        pagenum = 2;
        toSend.addCommand("page 1");
        // Do the shard tab
        toSend.addCommand(util::format("gumppic %u %u %u ", 17, yStart + 47, 0x138F));
        szBuffer = "Objects";
        toSend.addCommand(util::format("text %u %u %u %u", 42, yStart + 46, 70, linenum++));
        toSend.addText(szBuffer);
        
        toSend.addCommand(util::format("htmlgump %i %i %i %i %u %i %i", 245, 70, 220, 30, linenum++, 0, 0));
        toSend.addText("<CENTER><BIG><BASEFONT color=#F64040>Ultima Offline eXperiment 3</BASEFONT></BIG></CENTER>");
        
        // Shard Menu Version
        szBuffer = "v"s + UOXVersion::version + "."s + UOXVersion::build;
        toSend.addCommand(util::format("text %u %u %u %u", 320, 90, 27, linenum++));
        toSend.addText(szBuffer);
        
        // Shard Menu Description
        szBuffer = "Greetings! Using these menus you can";
        toSend.addCommand(util::format("text %u %u %u %u", 225, 120, 94, linenum++));
        toSend.addText(szBuffer);
        
        szBuffer = "quickly and easily add Items, Spawners,";
        toSend.addCommand(util::format("text %u %u %u %u", 225, 135, 94, linenum++));
        toSend.addText(szBuffer);
        
        szBuffer = "NPCs and more to your shard, in order";
        toSend.addCommand(util::format("text %u %u %u %u", 225, 150, 94, linenum++));
        toSend.addText(szBuffer);
        
        szBuffer = "to customize it to your needs.";
        toSend.addCommand(util::format("text %u %u %u %u", 225, 165, 94, linenum++));
        toSend.addText(szBuffer);
        
        szBuffer = "Objects tab is used to find stuff to";
        toSend.addCommand(util::format("text %u %u %u %u", 225, 190, 94, linenum++));
        toSend.addText(szBuffer);
        
        szBuffer = "add. Use Settings tab to configure";
        toSend.addCommand(util::format("text %u %u %u %u", 225, 205, 94, linenum++));
        toSend.addText(szBuffer);
        
        szBuffer = "various options for this menu!";
        toSend.addCommand(util::format("text %u %u %u %u", 225, 220, 94, linenum++));
        toSend.addText(szBuffer);
        
        szBuffer = "Don't forget to check out the UOX3 Docs!";
        toSend.addCommand(util::format("text %u %u %u %u", 225, 260, 27, linenum++));
        toSend.addText(szBuffer);
        
        toSend.addCommand(util::format("button %u %u %u %u %u %u %u", 295, 295, 0x5d1, 0x5d3, 1, 0, 50020));
        
        // here we hunt tags to make sure that we get them all from the itemmenus etc.
        std::uint8_t numCounter = 0;
        position = 80;
        xOffset = SXOFFSET;
        yOffset = SYOFFSET;
        for (tag = ItemMenu->Next(); !ItemMenu->AtEnd(); tag = ItemMenu->Next()) {
            data = ItemMenu->GrabData();
            if (numCounter > 0 && (!(numCounter % 12))) {
                position = 80;
                toSend.addCommand(util::format("page %i", ++pagenum));
                xOffset = SXOFFSET;
                yOffset = SYOFFSET;
            }
            // Drop in the page number text area image
            toSend.addCommand(util::format("gumppic %u %u %u", xStart + 300, yWidth - 28, 0x98E));
            // Add the page number text to the text area for display
            toSend.addCommand(
                              util::format("text %u %u %u %u", xStart + 335, yWidth - 27, 39, linenum++));
            szBuffer = util::format("Menu %i - Page %i", m, pagenum - 1);
            toSend.addText(szBuffer);
            // Spin the tagged items loaded in from the dfn files.
            toSend.addCommand(util::format("button %u %u %u %u %u %u %u", 15, position, 0x4B9,
                                           0x4BA, 1, 0, buttonnum));
            toSend.addCommand(util::format("croppedtext %u %u %u %u %u %u", 35, position - 3, 150,
                                           20, 50, linenum++));
            toSend.addText(data);
            if (tag.data()[0] != '<' &&
                tag.data()[0] != ' ') // it actually has a picture, well bugger me! :>
            {
                // Draw a frame for the item to make it stand out a touch more.
                toSend.addCommand(util::format("resizepic %u %u %u %u %u", xOffset, yOffset, 0x53, 65, 100));
                toSend.addCommand(util::format("checkertrans %u %u %u %u", xOffset + 7, yOffset + 9, 52, 82));
                toSend.addCommand(util::format("tilepic %u %u %i", xOffset + 5, yOffset + 10, std::stoi(tag, nullptr, 0)));
                toSend.addCommand(util::format("croppedtext %u %u %u %u %u %u", xOffset, yOffset + 65, 65, 20, 55, linenum++));
                toSend.addText(data);
                xOffset += XOFFSET;
                if (xOffset > 480) {
                    xOffset = SXOFFSET;
                    yOffset += YOFFSET;
                }
            }
            position += 20;
            ++buttonnum;
            ++numCounter;
            
            tag = ItemMenu->Next();
        }
        std::uint32_t currentPage = 1;
        
        for (i = 0; i < numCounter; i += 12) {
            toSend.addCommand(util::format("page %i", currentPage));
            if (i >= 10) {
                toSend.addCommand(util::format("button %u %u %u %u %u %u", xStart + 240,  yWidth - 25, 0x25EB, 0x25EA, 0, currentPage - 1));
            }
            if ((numCounter > 12) && ((i + 12) < numCounter)) {
                toSend.addCommand(util::format("button %u %u %u %u %u %u", xWidth - 60, yWidth - 25, 0x25E7, 0x25E6, 0, currentPage + 1));
            }
            currentPage++;
        }
    }
    else // m != 1
    {
        // Now we make the first page that will essentially display our list of Main GM Groups.
        pagenum = 2;
        toSend.addCommand("page 1");
        // Do the shard tab
        toSend.addCommand(util::format("gumppic %u %u %u ", 17, yStart + 47, 0x138F));
        szBuffer = "Shard";
        toSend.addCommand(util::format("text %u %u %u %u", 42, yStart + 46, 39, linenum++));
        toSend.addText(szBuffer);
        // here we hunt tags to make sure that we get them all from the itemmenus etc.
        std::uint8_t numCounter = 0;
        position = 80;
        xOffset = SXOFFSET;
        yOffset = SYOFFSET;
        
        // Drop in the page number text area image
        toSend.addCommand(util::format("gumppic %u %u %u", xStart + 300, yWidth - 28, 0x98E));
        // Add the page number text to the text area for display
        toSend.addCommand(util::format("text %u %u %u %u", xStart + 335, yWidth - 27, 39, linenum++));
        szBuffer = util::format("Menu %i - Page %i", m, pagenum - 1);
        toSend.addText(szBuffer);
        
        for (tag = ItemMenu->Next(); !ItemMenu->AtEnd(); tag = ItemMenu->Next()) {
            data = ItemMenu->GrabData();
            
            if (numCounter > 0 && (!(numCounter % 12))) {
                position = 80;
                toSend.addCommand(util::format("page %i", pagenum++));
                xOffset = SXOFFSET;
                yOffset = SYOFFSET;
                
                // Drop in the page number text area image
                toSend.addCommand(util::format("gumppic %u %u %u", xStart + 300, yWidth - 28, 0x98E));
                // Add the page number text to the text area for display
                toSend.addCommand(util::format("text %u %u %u %u", xStart + 335, yWidth - 27, 39, linenum++));
                szBuffer = util::format("Menu %i - Page %i", m, pagenum - 1);
                toSend.addText(szBuffer);
            }
            // Drop in the page number text area image
            if (util::upper(tag) == "INSERTADDMENUITEMS") {
                // Check to see if the desired menu has any items to add
                if (g_mmapAddMenuMap.find(m) == g_mmapAddMenuMap.end()) {
                    continue;
                }
                // m contains the groupId that we need to do fetch the auto-addmenu items
                auto pairRange =
                g_mmapAddMenuMap.equal_range(m);
                for (auto CI = pairRange.first; CI != pairRange.second; CI++) {
                    toSend.addCommand(util::format("button %u %u %u %u %u %u %u", 15, position,0x4B9, 0x4BA, 1, 0, buttonnum));
                    toSend.addCommand(util::format("croppedtext %u %u %u %u %u %u", 35,position - 3, 150, 20, 40, linenum));
                    toSend.addText(CI->second.itemName);
                    // check to make sure that we have an image now, seeing as we might not have one
                    // with the new changes in 0.98.01.2+
                    if (CI->second.tileId != 0) {
                        // Draw a frame for the item to make it stand out a touch more.
                        toSend.addCommand(util::format("resizepic %u %u %u %u %u", xOffset, yOffset,0x53, 65, 100));
                        toSend.addCommand(util::format("checkertrans %u %u %u %u", xOffset + 7,yOffset + 9, 52, 82));
                        toSend.addCommand(util::format("tilepic %u %u %i", xOffset + 5,yOffset + 10, CI->second.tileId));
                        toSend.addCommand(util::format("croppedtext %u %u %u %u %u %u", xOffset,yOffset + 65, 65, 20, 55, linenum++));
                        toSend.addText(CI->second.itemName);
                        toSend.addText(CI->second.itemName.c_str());
                        xOffset += XOFFSET;
                        if (xOffset > 480) {
                            xOffset = SXOFFSET;
                            yOffset += YOFFSET;
                        }
                    }
                    position += 20;
                    ++linenum;
                    ++buttonnum;
                    ++numCounter;
                }
                continue;
            }
            toSend.addCommand(util::format("button %u %u %u %u %u %u %u", 15, position, 0x4B9,0x4BA, 1, 0, buttonnum));
            toSend.addCommand(util::format("croppedtext %u %u %u %u %u %u", 35, position - 3, 150,20, 50, linenum++));
            toSend.addText(data);
            if (tag.data()[0] != '<' &&
                tag.data()[0] != ' ') // it actually has a picture, well bugger me! :>
            {
                // Draw a frame for the item to make it stand out a touch more.
                toSend.addCommand(util::format("checkertrans %u %u %u %u", xOffset + 7, yOffset + 9, 110, 110));
                toSend.addCommand(util::format("buttontileart %u %u 0x0a9f 0x0aa1 %u %u %u %u %u %u %u", xOffset,yOffset, 1, 0, buttonnum, std::stoi(tag, nullptr, 0), 0, 25, 25));
                toSend.addCommand(util::format("tooltip 1042971 @%s@", data.c_str()));
                toSend.addCommand(util::format("croppedtext %u %u %u %u %u %u", xOffset + 15,yOffset + 85, 100, 20, 50, linenum++));
                toSend.addText(data);
                xOffset += XOFFSET;
                if (xOffset > 640) {
                    xOffset = SXOFFSET;
                    yOffset += YOFFSET;
                }
            }
            position += 20;
            ++buttonnum;
            ++numCounter;
            
            tag = ItemMenu->Next();
        }
        
        std::uint32_t currentPage = 1;
        for (i = 0; i < numCounter; i += 12) {
            toSend.addCommand(util::format("page %i", currentPage));
            if (i >= 10) {
                toSend.addCommand(util::format("button %u %u %u %u %u %u", xStart + 240,yWidth - 25, 0x25EB, 0x25EA, 0,currentPage - 1)); // back button
            }
            if ((numCounter > 12) && ((i + 12) < numCounter)) {
                toSend.addCommand(util::format("button %u %u %u %u %u %u", xWidth - 60, yWidth - 25,0x25E7, 0x25E6, 0, currentPage + 1));
            }
            ++currentPage;
        }
    }
    
    // Reserved page # 800 - 899 for the server support pages
    toSend.addCommand("page 30");
    // Show the selected tab image for this page.
    toSend.addCommand(util::format("gumppic %u %u %u", 105, yStart + 47, 0x138F));
    toSend.addCommand(util::format("text %u %u %u %u", 132, yStart + 46, 70, linenum++));
    toSend.addText("Settings");
    
    // Create the Quick Access Menu
    toSend.addCommand(util::format("text %u %u %u %u", 57, yStart + 76, 52, linenum++));
    toSend.addText("Quick-Access");
    
    // Need a seperator
    toSend.addCommand(util::format("gumppictiled %u %u %u %u %u", xStart + 21, yStart + 105, 165, 5, 0x2393));
    
    // Create the Commandlist button
    toSend.addCommand(util::format("button %u %u %u %u %u %u %u", 45, yStart + 125, 0x2A30, 0x2A44, 1, 4, 50008));
    toSend.addCommand(util::format("text %u %u %u %u", 65, yStart + 129, 52, linenum++));
    toSend.addText("Commandlist");
    
    // Create the Wholist Online button
    toSend.addCommand(util::format("button %u %u %u %u %u %u %u", 45, yStart + 155, 0x2A30, 0x2A44, 1, 4, 50009));
    toSend.addCommand(util::format("text %u %u %u %u", 62, yStart + 159, 52, linenum++));
    toSend.addText("Who is Online");
    
    // Create the Wholist Offline button
    toSend.addCommand(util::format("button %u %u %u %u %u %u %u", 45, yStart + 185, 0x2A30, 0x2A44, 1, 4, 50010));
    toSend.addCommand(util::format("text %u %u %u %u", 60, yStart + 189, 52, linenum++));
    toSend.addText("Who is Offline");
    
    // Create the Reload DFNs button
    toSend.addCommand(util::format("button %u %u %u %u %u %u %u", 45, yStart + 215, 0x2A30, 0x2A44, 1, 4, 50011));
    toSend.addCommand(util::format("text %u %u %u %u", 60, yStart + 219, 52, linenum++));
    toSend.addText("Reload DFNs");
    
    // Create the Server Shutdown button
    if (mChar->GetCommandLevel() >= CL_ADMIN) {
        toSend.addCommand(util::format("button %u %u %u %u %u %u %u", 45, yStart + 275, 0x2A58,0x2A44, 1, 4, 50012));
        toSend.addCommand(util::format("text %u %u %u %u", 73, yStart + 279, 52, linenum++));
        toSend.addText("Shutdown");
    }
    
    // Settings Backgrounds
    toSend.addCommand(util::format("resizepic %u %u %u %u %u", 210, 55, 5120, 250, 150));
    toSend.addCommand(util::format("resizepic %u %u %u %u %u", 210, 205, 5120, 250, 150));
    
    // Settings Header 1
    toSend.addCommand(util::format("htmlgump %i %i %i %i %u %i %i", 235, 60, 200, 60, linenum++, 0, 0));
    toSend.addText("<CENTER><BIG><BASEFONT color=#EECD8B>Menu Settings</BASEFONT></BIG></CENTER>");
    
    // Settings Header 2
    toSend.addCommand(util::format("htmlgump %i %i %i %i %u %i %i", 235, 220, 200, 60, linenum++, 0, 0));
    toSend.addText("<CENTER><BIG><BASEFONT color=#EECD8B>Item Settings</BASEFONT></BIG></CENTER>");
    
    // Settings Options START
    // Add at Location
    auto addAtLoc = mChar->GetTag("addAtLoc");
    if (addAtLoc.m_IntValue == 1) {
        toSend.addCommand(util::format("button %u %u %u %u %u %u %u", 225, 80, 0x869, 0x867, 1, 0, 50000));
    }
    else {
        toSend.addCommand(util::format("button %u %u %u %u %u %u %u", 225, 80, 0x867, 0x869, 1, 0, 50000));
    }
    szBuffer = "Add item at specific location";
    toSend.addCommand(util::format("text %u %u %u %u", 255, 80, 94, linenum++));
    toSend.addText(szBuffer);
    
    szBuffer = "instead of in GM's backpack";
    toSend.addCommand(util::format("text %u %u %u %u", 255, 95, 94, linenum++));
    toSend.addText(szBuffer);
    
    // Repeat Add Object
    auto repeatAdd = mChar->GetTag("repeatAdd");
    if (repeatAdd.m_IntValue == 1) {
        toSend.addCommand(util::format("button %u %u %u %u %u %u %u", 250, 120, 0x869, 0x867, 1, 0, 50001));
    }
    else {
        toSend.addCommand(util::format("button %u %u %u %u %u %u %u", 250, 120, 0x867, 0x869, 1, 0, 50001));
    }
    szBuffer = "Add item repeatedly";
    toSend.addCommand(util::format("text %u %u %u %u", 280, 120, 94, linenum++));
    toSend.addText(szBuffer);
    
    szBuffer = "until cancelled";
    toSend.addCommand(util::format("text %u %u %u %u", 280, 135, 94, linenum++));
    toSend.addText(szBuffer);
    
    // Auto-reopen Menu
    auto reopenMenu = mChar->GetTag("reopenMenu");
    if (reopenMenu.m_IntValue == 1) {
        toSend.addCommand(util::format("button %u %u %u %u %u %u %u", 225, 160, 0x869, 0x867, 1, 0, 50003));
    }
    else {
        toSend.addCommand(util::format("button %u %u %u %u %u %u %u", 225, 160, 0x867, 0x869, 1, 0, 50003));
    }
    szBuffer = "Automatically reopen menu";
    toSend.addCommand(util::format("text %u %u %u %u", 255, 160, 94, linenum++));
    toSend.addText(szBuffer);
    
    szBuffer = "after selecting object to add";
    toSend.addCommand(util::format("text %u %u %u %u", 255, 175, 94, linenum++));
    toSend.addText(szBuffer);
    
    // Force-Decayable Off
    auto forceDecayOff = mChar->GetTag("forceDecayOff");
    if (forceDecayOff.m_IntValue == 1) {
        toSend.addCommand(util::format("button %u %u %u %u %u %u %u", 240, 260, 0x16ca, 0x16cb, 1, 0, 50004));
    }
    else {
        toSend.addCommand(util::format("button %u %u %u %u %u %u %u", 240, 260, 0x16c6, 0x16c7, 1, 0, 50004));
    }
    // Force-Decayable On
    auto forceDecayOn = mChar->GetTag("forceDecayOn");
    if (forceDecayOn.m_IntValue == 1) {
        toSend.addCommand(util::format("button %u %u %u %u %u %u %u", 350, 260, 0x16c4, 0x16c5, 1, 0, 50005));
    }
    else {
        toSend.addCommand(util::format("button %u %u %u %u %u %u %u", 350, 260, 0x16c0, 0x16c1, 1, 0, 50005));
    }
    szBuffer = "Default Decayable Status of Items";
    toSend.addCommand(util::format("text %u %u %u %u", 230, 240, 94, linenum++));
    toSend.addText(szBuffer);
    szBuffer = "Force OFF";
    toSend.addCommand(util::format("text %u %u %u %u", 275, 265, 27, linenum++));
    toSend.addText(szBuffer);
    szBuffer = "Force ON";
    toSend.addCommand(util::format("text %u %u %u %u", 385, 265, 72, linenum++));
    toSend.addText(szBuffer);
    
    // Force-Movable Off
    auto forceMovableOff = mChar->GetTag("forceMovableOff");
    if (forceMovableOff.m_IntValue == 1) {
        toSend.addCommand(util::format("button %u %u %u %u %u %u %u", 240, 320, 0x16ca, 0x16cb, 1, 0, 50006));
    }
    else {
        toSend.addCommand(util::format("button %u %u %u %u %u %u %u", 240, 320, 0x16c6, 0x16c7, 1, 0, 50006));
    }
    // Force-Movable On
    auto forceMovableOn = mChar->GetTag("forceMovableOn");
    if (forceMovableOn.m_IntValue == 1) {
        toSend.addCommand(util::format("button %u %u %u %u %u %u %u", 350, 320, 0x16c4, 0x16c5, 1, 0, 50007));
    }
    else {
        toSend.addCommand(util::format("button %u %u %u %u %u %u %u", 350, 320, 0x16c0, 0x16c1, 1, 0, 50007));
    }
    szBuffer = "Default Movable State of Items";
    toSend.addCommand(util::format("text %u %u %u %u", 230, 300, 94, linenum++));
    toSend.addText(szBuffer);
    szBuffer = "Force OFF";
    toSend.addCommand(util::format("text %u %u %u %u", 275, 325, 27, linenum++));
    toSend.addText(szBuffer);
    szBuffer = "Force ON";
    toSend.addCommand(util::format("text %u %u %u %u", 385, 325, 72, linenum++));
    toSend.addText(szBuffer);
    // Settings Options END
    
    // Reserved pages 900-999 for the online help system. (comming soon)
    toSend.addCommand("page 31");
    // Ok display the scroll that we use to display our help information
    toSend.addCommand(util::format("resizepic %u %u %u %u %u", xStart + 205, yStart + 62, 0x1432, 175, 200));
    // Write out what page were on (Mainly for debug purposes
    szBuffer = util::format("%5u", 31);
    toSend.addCommand(util::format("text %u %u %u %u", xWidth - 58, yWidth - 25, 110, linenum++));
    toSend.addText(szBuffer);
    // Ok, now the job of pulling the rest of the first itemmenu information and making tabs for
    // them
    szBuffer = "Page 31";
    toSend.addCommand(util::format("text %u %u %u %u", 30, yStart + 200, 87, linenum++));
    toSend.addText(szBuffer);
    toSend.addCommand(util::format("button %u %u %u %u %u %u %u", 104, yStart + 300, 0x138E, 0x138E, 0, 1, tabNumber++));
    
#if defined(UOX_DEBUG_MODE)
    Console::shared() << "==============================" << myendl;
#endif
    // Finish up and send the gump to the client socket.
    toSend.Finalize();
    s->Send(&toSend);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CPIHelpRequest::Handle()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens the "Help" Menu from the paperdoll
// o------------------------------------------------------------------------------------------------o
bool CPIHelpRequest::Handle() {
    std::uint16_t gmnumber = 0;
    CChar *mChar = tSock->CurrcharObj();
    
    std::vector<std::uint16_t> scriptTriggers = mChar->GetScriptTriggers();
    for (auto scriptTrig : scriptTriggers) {
        cScript *toExecute = JSMapping->GetScript(scriptTrig);
        if (toExecute != nullptr) {
            if (toExecute->OnHelpButton(mChar) == 0) {
                return true;
            }
        }
    }
    
    // No individual scripts handling OnHelpButton returned true - let's check global script!
    cScript *toExecute = JSMapping->GetScript(static_cast<std::uint16_t>(0));
    if (toExecute != nullptr) {
        if (toExecute->OnHelpButton(mChar) == 0) {
            return true;
        }
    }
    
    std::string sect = std::string("GMMENU ") + util::ntos(menuNum);
    CScriptSection *GMMenu = FileLookup->FindEntry(sect, menus_def);
    if (GMMenu == nullptr)
        return true;
    
    std::string line;
    CPOpenGump toSend(*mChar);
    toSend.GumpIndex(menuNum);
    std::string tag = GMMenu->First();
    std::string data = GMMenu->GrabData();
    line = tag + " " + data;
    toSend.Question(line);
    for (tag = GMMenu->Next(); !GMMenu->AtEnd(); tag = GMMenu->Next()) {
        data = GMMenu->GrabData();
        line = tag + " " + data;
        toSend.AddResponse(gmnumber++, 0, line);
        tag = GMMenu->Next();
    }
    toSend.Finalize();
    tSock->Send(&toSend);
    return true;
}

// o------------------------------------------------------------------------------------------------o
//|   Function	  -  CPage()
// o------------------------------------------------------------------------------------------------o
//|   Purpose     -  Used when player pages a counselor
// o------------------------------------------------------------------------------------------------o
void CPage(CSocket *s, const std::string &reason) {
    CChar *mChar = s->CurrcharObj();
    std::uint8_t a1 = mChar->GetSerial(1);
    std::uint8_t a2 = mChar->GetSerial(2);
    std::uint8_t a3 = mChar->GetSerial(3);
    std::uint8_t a4 = mChar->GetSerial(4);
    
    CHelpRequest pageToAdd;
    pageToAdd.Reason(reason);
    pageToAdd.WhoPaging(mChar->GetSerial());
    pageToAdd.IsHandled(false);
    pageToAdd.TimeOfPage(time(nullptr));
    
    serial_t callNum = CounselorQueue->Add(&pageToAdd);
    if (callNum != INVALIDSERIAL) {
        mChar->SetPlayerCallNum(callNum);
        if (reason == "OTHER") {
            mChar->SetSpeechMode(2);
            s->SysMessage(359); // Please enter the reason for your Counselor request.
        }
        else {
            bool x = false;
            for (auto &iSock : Network->connClients) {
                CChar *iChar = iSock->CurrcharObj();
                if (iChar->IsGMPageable() || iChar->IsCounselor()) {
                    x = true;
                    iSock->SysMessage(oldstrutil::format(1024, "Counselor Page from %s [%x %x %x %x]: %s",mChar->GetName().c_str(), a1, a2, a3, a4, reason.c_str()));
                }
            }
            if (x) {
                s->SysMessage(360); // Your request for info has been logged, and will be handled by
                // a Counselor as soon as possible!
            }
            else {
                s->SysMessage(361); // Your request for info has been logged, and will be handled by
                // a Counselor as soon as possible!
            }
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|   Function	  :  GMPage()
// o------------------------------------------------------------------------------------------------o
//|   Purpose     :  Used when player calls a GM
// o------------------------------------------------------------------------------------------------o
void GMPage(CSocket *s, const std::string &reason) {
    CChar *mChar = s->CurrcharObj();
    std::uint8_t a1 = mChar->GetSerial(1);
    std::uint8_t a2 = mChar->GetSerial(2);
    std::uint8_t a3 = mChar->GetSerial(3);
    std::uint8_t a4 = mChar->GetSerial(4);
    
    CHelpRequest pageToAdd;
    pageToAdd.Reason(reason);
    pageToAdd.WhoPaging(mChar->GetSerial());
    pageToAdd.IsHandled(false);
    pageToAdd.TimeOfPage(time(nullptr));
    serial_t callNum = GMQueue->Add(&pageToAdd);
    if (callNum != INVALIDSERIAL) {
        mChar->SetPlayerCallNum(callNum);
        if (reason == "OTHER") {
            mChar->SetSpeechMode(1);
            s->SysMessage(362); // Please enter the reason for your GM request.
        }
        else {
            bool x = false;
            for (auto &iSock : Network->connClients) {
                CChar *iChar = iSock->CurrcharObj();
                if (ValidateObject(iChar)) {
                    if (iChar->IsGMPageable()) {
                        x = true;
                        iSock->SysMessage(oldstrutil::format(1024, "Page from %s [%x %x %x %x]: %s", mChar->GetName().c_str(),a1, a2, a3, a4, reason.c_str()));
                    }
                }
            }
            if (x) {
                s->SysMessage(363); // Your request for assistance has been logged, and will be
                // handled by a GM as soon as possible!
            }
            else {
                s->SysMessage(364); // Your request for assistance has been logged, and will be
                // handled by a GM as soon as possible!
            }
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	HandleGumpCommand()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Execute a command from scripts
// o------------------------------------------------------------------------------------------------o
void HandleGumpCommand(CSocket *s, std::string cmd, std::string data) {
    if (!s || cmd.empty())
        return;
    
    CChar *mChar = s->CurrcharObj();
    
    cmd = util::upper(cmd);
    data = util::upper(data);
    std::string builtString;
    
    switch (cmd.data()[0]) {
        case 'A':
            if (!data.empty()) {
                bool runCommand = false;
                auto itemType = CBaseObject::OT_ITEM;
                if (cmd == "ADDITEM") {
                    runCommand = true;
                }
                else if (cmd == "ADDSPAWNER") {
                    runCommand = true;
                    itemType = CBaseObject::OT_SPAWNER;
                }
                auto reopenMenu = mChar->GetTag("reopenMenu");
                if (runCommand) {
                    auto addAtLoc = mChar->GetTag("addAtLoc");
                    auto repeatAdd = mChar->GetTag("repeatAdd");
                    auto forceDecayOn = mChar->GetTag("forceDecayOn");
                    auto forceDecayOff = mChar->GetTag("forceDecayOff");
                    auto forceMovableOn = mChar->GetTag("forceMovableOn");
                    auto forceMovableOff = mChar->GetTag("forceMovableOff");
                    auto secs = oldstrutil::sections(data, ",");
                    if (secs.size() > 1) {
                        std::string tmp = util::trim(util::strip(secs[0], "//"));
                        std::uint16_t num = util::ston<std::uint16_t>(util::trim(util::strip(secs[1], "//")));
                        
                        if (addAtLoc.m_IntValue == 1) {
                            std::string addCmd = "";
                            if (repeatAdd.m_IntValue == 1) {
                                if (itemType == CBaseObject::OT_SPAWNER) {
                                    addCmd = util::format("raddspawner %s", tmp.c_str());
                                }
                                else {
                                    addCmd = util::format("radditem %s", tmp.c_str());
                                }
                            }
                            else {
                                if (itemType == CBaseObject::OT_SPAWNER) {
                                    addCmd = util::format("add spawner %s", tmp.c_str());
                                }
                                else {
                                    addCmd = util::format("add item %s", tmp.c_str());
                                }
                            }
                            serverCommands.command(s, mChar, addCmd);
                        }
                        else if (repeatAdd.m_IntValue == 1) {
                            std::string addCmd = "";
                            if (itemType == CBaseObject::OT_SPAWNER) {
                                addCmd = util::format("raddspawner %s", tmp.c_str());
                            }
                            else {
                                addCmd = util::format("radditem %s", tmp.c_str());
                            }
                            serverCommands.command(s, mChar, addCmd);
                        }
                        else {
                            CItem *newItem = worldItem.CreateScriptItem(s, mChar, tmp, num, itemType, true);
                            if (ValidateObject(newItem)) {
                                if (forceDecayOff.m_IntValue == 1) {
                                    newItem->SetDecayable(false);
                                }
                                else if (forceDecayOn.m_IntValue == 1) {
                                    newItem->SetDecayable(true);
                                }
                                if (forceMovableOff.m_IntValue == 1) {
                                    newItem->SetMovable(2);
                                }
                                else if (forceMovableOn.m_IntValue == 1) {
                                    newItem->SetMovable(1);
                                }
                            }
                            else if (!ValidateObject(mChar->GetItemAtLayer(IL_PACKITEM))) {
                                s->SysMessage(9150); // No backpack detected, unable to add item! Try
                                // the 'addpack command?
                            }
                        }
                    }
                    else {
                        if (addAtLoc.m_IntValue == 1) {
                            std::string addCmd = "";
                            if (repeatAdd.m_IntValue == 1) {
                                if (itemType == CBaseObject::OT_SPAWNER) {
                                    addCmd = util::format("raddspawner %s", data.c_str());
                                }
                                else {
                                    addCmd = util::format("radditem %s", data.c_str());
                                }
                            }
                            else {
                                if (itemType == CBaseObject::OT_SPAWNER) {
                                    addCmd = util::format("add spawner %s", data.c_str());
                                }
                                else {
                                    addCmd = util::format("add item %s", data.c_str());
                                }
                            }
                            serverCommands.command(s, mChar, addCmd);
                        }
                        else if (repeatAdd.m_IntValue == 1) {
                            std::string addCmd = "";
                            if (itemType == CBaseObject::OT_SPAWNER) {
                                addCmd = util::format("raddspawner %s", data.c_str());
                            }
                            else {
                                addCmd = util::format("radditem %s", data.c_str());
                            }
                            serverCommands.command(s, mChar, addCmd);
                        }
                        else {
                            CItem *newItem = worldItem.CreateScriptItem(s, mChar, data, 0, itemType, true);
                            if (ValidateObject(newItem)) {
                                if (forceDecayOff.m_IntValue == 1) {
                                    newItem->SetDecayable(false);
                                }
                                else if (forceDecayOn.m_IntValue == 1) {
                                    newItem->SetDecayable(true);
                                }
                                if (forceMovableOff.m_IntValue == 1) {
                                    newItem->SetMovable(2);
                                }
                                else if (forceMovableOn.m_IntValue == 1) {
                                    newItem->SetMovable(1);
                                }
                            }
                            else if (!ValidateObject(mChar->GetItemAtLayer(IL_PACKITEM))) {
                                s->SysMessage(9150); // No backpack detected, unable to add item! Try
                                // the 'addpack command?
                            }
                        }
                    }
                }
                if (reopenMenu.m_IntValue == 1) {
                    std::string menuString = util::format("itemmenu %d", s->TempInt());
                    serverCommands.command(s, mChar, menuString);
                }
            }
            break;
        case 'C':
            if (cmd == "CPAGE") {
                if (data.empty())
                    return;
                
                CPage(s, data);
            }
            break;
        case 'G':
            if (cmd == "GMMENU") {
                if (data.empty())
                    return;
                
                CPIHelpRequest toHandle(s, static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(data, "//")), nullptr, 0)));
                toHandle.Handle();
            }
            else if (cmd == "GMPAGE") {
                if (data.empty())
                    return;
                
                GMPage(s, data);
            }
            else if (cmd == "GCOLLECT") {
                CollectGarbage();
            }
            else if (cmd == "GOPLACE") {
                if (data.empty())
                    return;
                
                std::uint16_t placeNum = static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(data, "//")), nullptr, 0));
                if (worldMain.goPlaces.find(placeNum) != worldMain.goPlaces.end()) {
                    GoPlaces toGoTo = worldMain.goPlaces[placeNum];
                    
                    if (toGoTo.worldNum == 0 && mChar->WorldNumber() <= 1) {
                        // Stay in same world if already in world 0 or 1
                        mChar->SetLocation(toGoTo.x, toGoTo.y, toGoTo.z, mChar->WorldNumber(), mChar->GetInstanceId());
                        
                        // Additional update required for regular UO client
                        mChar->Update();
                    }
                    else if (toGoTo.worldNum != mChar->WorldNumber()) {
                        // Change map!
                        mChar->SetLocation(toGoTo.x, toGoTo.y, toGoTo.z, toGoTo.worldNum, mChar->GetInstanceId());
                        SendMapChange(toGoTo.worldNum, s);
                        
                        // Additional update required for regular UO client
                        mChar->Update();
                    }
                }
            }
            else if (cmd == "GUIINFORMATION") {
                CGumpDisplay guiInfo(s, 400, 300);
                guiInfo.setTitle(UOXVersion::productName + " Status"s);
                builtString = GetUptime();
                guiInfo.AddData("Version",  UOXVersion::version + "."s + UOXVersion::build + " ["s + OS_STR + "]"s);
                //                guiInfo.AddData("Compiled By", UOXVersion::name);
                guiInfo.AddData("Compiled By", "punt");
                guiInfo.AddData("Uptime", builtString);
                guiInfo.AddData("Accounts", static_cast<std::uint32_t>(Account::shared().size()));
                guiInfo.AddData("Items", ObjectFactory::shared().CountOfObjects(CBaseObject::OT_ITEM));
                guiInfo.AddData("Chars", ObjectFactory::shared().CountOfObjects(CBaseObject::OT_CHAR));
                guiInfo.AddData("Players in world", static_cast<std::uint32_t>(worldMain.GetPlayersOnline()));
                guiInfo.Send(0, false, INVALIDSERIAL);
            }
            break;
        case 'I':
            if (cmd == "ITEMMENU") {
                if (data.empty())
                    return;
                
                BuildAddMenuGump(s, static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(data, "//")), nullptr, 0)));
            }
            else if (cmd == "INFORMATION") {
                builtString = GetUptime();
                s->SysMessage(1211, builtString.c_str(), worldMain.GetPlayersOnline(), Account::shared().size(), ObjectFactory::shared().CountOfObjects(CBaseObject::OT_ITEM), ObjectFactory::shared().CountOfObjects(CBaseObject::OT_CHAR));
            }
            break;
        case 'M':
            if (cmd == "MAKEMENU") {
                if (data.empty())
                    return;
                
                Skills->NewMakeMenu(s, std::stoi(util::trim(util::strip(data, "//")), nullptr, 0), static_cast<std::uint8_t>(s->AddId()));
            }
            break;
        case 'N':
            if (cmd == "NPC") {
                if (data.empty())
                    return;
                
                std::string menuString = util::format("itemmenu %d", s->TempInt());
                serverCommands.command(s, mChar, menuString);
                
                auto repeatAdd = mChar->GetTag("repeatAdd");
                if (repeatAdd.m_IntValue == 1) {
                    std::string addCmd = util::format("raddnpc %s", data.c_str());
                    serverCommands.command(s, mChar, addCmd);
                }
                else {
                    s->XText(data);
                    s->SendTargetCursor(0, TARGET_ADDSCRIPTNPC, 0, 1212, data.c_str()); // Select location for NPC. [Number: %s]
                }
            }
            break;
        case 'P':
            if (cmd == "POLYMORPH") {
                if (data.empty())
                    return;
                
                std::uint16_t newBody = static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
                mChar->SetId(newBody);
                mChar->SetOrgId(newBody);
            }
            break;
        case 'S':
            if (cmd == "SYSMESSAGE") {
                if (data.empty())
                    return;
                
                s->SysMessage(data.c_str());
            }
            else if (cmd == "SKIN") {
                if (data.empty())
                    return;
                
                auto newSkin = static_cast<colour_t>(std::stoul(data, nullptr, 0));
                mChar->SetSkin(newSkin);
                mChar->SetOrgSkin(newSkin);
            }
            break;
        case 'V':
            if (cmd == "VERSION") {
                //                s->SysMessage("%s v%s(%s) [%s] Compiled by %s ", UOXVersion::productName.c_str(), UOXVersion::version.c_str(), UOXVersion::build.c_str(), OS_STR,  UOXVersion::name.c_str());
                s->SysMessage("%s v%s(%s) [%s] Compiled by %s ", "UOXVersion::productName.c_str()", UOXVersion::version.c_str(), UOXVersion::build.c_str(), OS_STR,  "punt");
            }
            break;
        case 'W':
            if (cmd == "WEBLINK") {
                if (data.empty())
                    return;
                
                s->OpenURL(data);
            }
            break;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	HandleAddMenuButton()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles button pressed in add menu gump
// o------------------------------------------------------------------------------------------------o
void HandleAddMenuButton(CSocket *s, std::uint32_t button) {
    std::int32_t addMenuLoc = s->TempInt();
    std::string sect = std::string("ITEMMENU ") + util::ntos(addMenuLoc);
    CChar *mChar = s->CurrcharObj();
    
    if (button >= 50000 && button <= 50020) {
        auto addAtLoc = mChar->GetTag("addAtLoc");
        auto repeatAdd = mChar->GetTag("repeatAdd");
        auto reopenMenu = mChar->GetTag("reopenMenu");
        auto forceDecayOn = mChar->GetTag("forceDecayOn");
        auto forceDecayOff = mChar->GetTag("forceDecayOff");
        auto forceMovableOn = mChar->GetTag("forceMovableOn");
        auto forceMovableOff = mChar->GetTag("forceMovableOff");
        
        std::string tagName = "";
        std::int32_t tagVal = 0;
        TagMap customTag;
        std::map<std::string, TagMap> customTagMap;
        if (button == 50000) { // Add item at specific location
            tagName = "addAtLoc";
            auto addAtLoc = mChar->GetTag("addAtLoc");
            if (addAtLoc.m_IntValue == 1) {
                tagVal = 0;
                if (repeatAdd.m_IntValue == 1) {
                    customTag.m_Destroy = false;
                    customTag.m_IntValue = 0;
                    customTag.m_ObjectType = TagMap::TAGMAP_TYPE_INT;
                    customTag.m_StringValue = "";
                    customTagMap.insert(std::pair<std::string, TagMap>("repeatAdd", customTag));
                }
            }
            else {
                tagVal = 1;
            }
        }
        else if (button == 50001) { // Repeatedly add items until cancelled
            tagName = "repeatAdd";
            auto addAtLoc = mChar->GetTag("repeatAdd");
            if (addAtLoc.m_IntValue == 1) {
                tagVal = 0;
            }
            else {
                tagVal = 1;
                if (addAtLoc.m_IntValue == 0) {
                    customTag.m_Destroy = false;
                    customTag.m_IntValue = 1;
                    customTag.m_ObjectType = TagMap::TAGMAP_TYPE_INT;
                    customTag.m_StringValue = "";
                    customTagMap.insert(std::pair<std::string, TagMap>("addAtLoc", customTag));
                }
            }
        }
        else if (button == 50002) {
            // Return to home page of menu
            serverCommands.command(s, mChar, "add");
            return;
        }
        else if (button == 50003) { // Automatically reopen menu after adding objects
            tagName = "reopenMenu";
            auto reopenMenu = mChar->GetTag("reopenMenu");
            if (reopenMenu.m_IntValue == 1) {
                tagVal = 0;
            }
            else {
                tagVal = 1;
                if (reopenMenu.m_IntValue == 0) {
                    customTag.m_Destroy = false;
                    customTag.m_IntValue = 1;
                    customTag.m_ObjectType = TagMap::TAGMAP_TYPE_INT;
                    customTag.m_StringValue = "";
                    customTagMap.insert(std::pair<std::string, TagMap>("reopenMenu", customTag));
                }
            }
        }
        else if (button == 50004) { // Toggle Force Decay state for items on/off
            tagName = "forceDecayOff";
            auto forceDecayOff = mChar->GetTag("forceDecayOff");
            if (forceDecayOff.m_IntValue == 1) {
                tagVal = 0;
            }
            else {
                tagVal = 1;
                if (forceDecayOn.m_IntValue == 1) {
                    customTag.m_Destroy = false;
                    customTag.m_IntValue = 0;
                    customTag.m_ObjectType = TagMap::TAGMAP_TYPE_INT;
                    customTag.m_StringValue = "";
                    customTagMap.insert(std::pair<std::string, TagMap>("forceDecayOn", customTag));
                }
            }
        }
        else if (button == 50005) { // Toggle Force Decay state for items on/off
            tagName = "forceDecayOn";
            auto forceDecayOn = mChar->GetTag("forceDecayOn");
            if (forceDecayOn.m_IntValue == 1) {
                tagVal = 0;
            }
            else {
                tagVal = 1;
                if (forceDecayOff.m_IntValue == 1) {
                    customTag.m_Destroy = false;
                    customTag.m_IntValue = 0;
                    customTag.m_ObjectType = TagMap::TAGMAP_TYPE_INT;
                    customTag.m_StringValue = "";
                    customTagMap.insert(std::pair<std::string, TagMap>("forceDecayOff", customTag));
                }
            }
        }
        else if (button == 50006) { // Toggle Force OFF Movable state for items
            tagName = "forceMovableOff";
            auto forceMovableOff = mChar->GetTag("forceMovableOff");
            if (forceMovableOff.m_IntValue == 1) {
                tagVal = 0;
            }
            else {
                tagVal = 1;
                if (forceMovableOn.m_IntValue == 1) {
                    customTag.m_Destroy = false;
                    customTag.m_IntValue = 0;
                    customTag.m_ObjectType = TagMap::TAGMAP_TYPE_INT;
                    customTag.m_StringValue = "";
                    customTagMap.insert(std::pair<std::string, TagMap>("forceMovableOn", customTag));
                }
            }
        }
        else if (button == 50007) { // Toggle Force ON Movable state for items
            tagName = "forceMovableOn";
            auto forceMovableOn = mChar->GetTag("forceMovableOn");
            if (forceMovableOn.m_IntValue == 1) {
                tagVal = 0;
            }
            else {
                tagVal = 1;
                if (forceMovableOff.m_IntValue == 1) {
                    customTag.m_Destroy = false;
                    customTag.m_IntValue = 0;
                    customTag.m_ObjectType = TagMap::TAGMAP_TYPE_INT;
                    customTag.m_StringValue = "";
                    customTagMap.insert(std::pair<std::string, TagMap>("forceMovableOff", customTag));
                }
            }
        }
        else if (button == 50008) {
            // Show command list
            serverCommands.command(s, mChar, "howto");
            return;
        }
        else if (button == 50009) {
            // Show wholist online
            serverCommands.command(s, mChar, "wholist on");
            return;
        }
        else if (button == 50010) {
            // Show wholist offline
            serverCommands.command(s, mChar, "wholist off");
            return;
        }
        else if (button == 50011) {
            // Reload DFNs
            serverCommands.command(s, mChar, "reloaddefs");
            return;
        }
        else if (button == 50012) {
            // Shutdown Server
            if (mChar->GetCommandLevel() >= CL_ADMIN)
                serverCommands.command(s, mChar, "shutdown 1");
            return;
        }
        else if (button == 50020) {
            // Browse UOX3 Docs online
            serverCommands.command(s, mChar, "browse https://www.uox3.org/docs");
            
            // Reopen menu
            serverCommands.command(s, mChar, sect);
            return;
        }
        customTag.m_Destroy = false;
        customTag.m_IntValue = tagVal;
        customTag.m_ObjectType = TagMap::TAGMAP_TYPE_INT;
        customTag.m_StringValue = "";
        customTagMap.insert(std::pair<std::string, TagMap>(tagName, customTag));
        
        // Add custom tags to multi
        for (const auto &[key, value] : customTagMap) {
            mChar->SetTag(key, value);
        }
        
        // Reopen menu
        serverCommands.command(s, mChar, sect);
        return;
    }
    
    CScriptSection *ItemMenu = FileLookup->FindEntry(sect, items_def);
    if (ItemMenu == nullptr)
        return;
    
    // If we get here we have to check to see if there are any other entryies added via the
    // auto-addmenu code. Each item == 2 entries IE: IDNUM=Text name of Item, and ADDITEM=itemID to
    // add
    auto pairRange = g_mmapAddMenuMap.equal_range(addMenuLoc);
    std::uint32_t autoAddMenuItemCount = 0;
    for (auto CI = pairRange.first; CI != pairRange.second; ++CI) {
        autoAddMenuItemCount += 2; // Need to inicrement by 2 because each entry is measured in the
        // dfn' as two lines. Used in teh calculation below.
    }
    // let's skip over the name, and get straight to where we should be headed
    size_t entryNum = ((static_cast<size_t>(button) - 6) * 2);
    autoAddMenuItemCount += static_cast<std::uint32_t>(ItemMenu->NumEntries());
    if (autoAddMenuItemCount >= entryNum) {
        std::string tag = ItemMenu->moveTo(entryNum);
        std::string data = ItemMenu->GrabData();
        HandleGumpCommand(s, tag, data);
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	HandleHowTo()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response when player enters HOWTO command
// o------------------------------------------------------------------------------------------------o
void HandleHowTo(CSocket *sock, std::int32_t cmdNumber) {
    if (cmdNumber == -2)
        return;
    
    std::int32_t iCounter = 0;
    std::uint8_t cmdLevelReq = 0xFF;
    std::uint8_t cmdType = 0xFF;
    std::string cmdName = "";
    bool found = false;
    for (auto itr = CCommands::commandMap.begin(); itr != CCommands::commandMap.end(); ++itr) {
        if (iCounter == cmdNumber) {
            cmdName = itr->first;
            cmdLevelReq = itr->second.cmdLevelReq;
            cmdType = itr->second.cmdType;
            found = true;
            break;
        }
        ++iCounter;
    }
    
    if (!found) {
        for (auto itr = CCommands::targetMap.begin(); itr != CCommands::targetMap.end(); ++itr) {
            if (iCounter == cmdNumber) {
                cmdName = itr->first;
                cmdLevelReq = itr->second.cmdLevelReq;
                found = true;
                break;
            }
            ++iCounter;
        }
    }
    
    if (!found) {
        for (auto itr = CCommands::jscommandMap.begin(); itr != CCommands::jscommandMap.end(); ++itr) {
            if (iCounter == cmdNumber) {
                cmdName = itr->first;
                cmdLevelReq = itr->second.cmdLevelReq;
                break;
            }
            ++iCounter;
        }
    }
    
    if (iCounter == cmdNumber) {
        // Build gump structure here, with basic information like Command Level, Name, Command Type,
        // and parameters (if any, from table)
        CGumpDisplay CommandInfo(sock, 480, 320);
        CommandInfo.setTitle(cmdName);
        
        CommandInfo.AddData("Minimum Command Level", cmdLevelReq);
        switch (cmdType) {
            case CMD_TARGET:
                CommandInfo.AddData("Syntax", "None (generic target)");
                break;
            case CMD_FUNC:
                CommandInfo.AddData("Syntax", "None (generic command)");
                break;
            case CMD_SOCKFUNC:
                CommandInfo.AddData("Syntax", "None (generic command)");
                break;
            case CMD_TARGETINT:
                CommandInfo.AddData("Syntax", "arg1 (hex or decimal)");
                break;
            case CMD_TARGETXYZ:
                CommandInfo.AddData("Syntax", "arg1 arg2 arg3 (hex or decimal)");
                break;
            case CMD_TARGETTXT:
                CommandInfo.AddData("Syntax", "String");
                break;
            default:
                break;
        }
        
        auto filename = util::format("help/commands/%s.txt", cmdName.c_str());
        
        auto toOpen = std::ifstream(filename);
        if (!toOpen.is_open()) {
            CommandInfo.AddData("", "No extra information is available about this command", 7);
        }
        else {
            char cmdLine[129];
            while (!toOpen.eof() && !toOpen.fail()) {
                toOpen.getline(cmdLine, 128);
                cmdLine[toOpen.gcount()] = 0;
                if (cmdLine[0] != 0) {
                    CommandInfo.AddData("", cmdLine, 7);
                }
            };
            toOpen.close();
        }
        CommandInfo.Send(4, false, INVALIDSERIAL);
    }
    else {
        sock->SysMessage(280); // Error finding command
    }
}

void HandleHowToButton(CSocket *s, std::uint32_t button) { HandleHowTo(s, button - 2); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CPIGumpMenuSelect::Handle()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles button press in gumps
// o------------------------------------------------------------------------------------------------o
bool CPIGumpMenuSelect::Handle() {
    id = tSock->GetDWord(3);
    gumpId = tSock->GetDWord(7);
    buttonId = tSock->GetDWord(11);
    switchCount = tSock->GetDWord(15);
    
#if defined(UOX_DEBUG_MODE)
    Console::shared() << "CPIGumpMenuSelect::Handle()" << myendl;
    Console::shared() << "        GumpId : " << gumpId << myendl;
    Console::shared() << "      ButtonId : " << buttonId << myendl;
    Console::shared() << "   SwitchCount : " << switchCount << myendl;
#endif
    
    if (gumpId == 461) { // Virtue gump
        CChar *targChar = nullptr;
        if (buttonId == 1 && switchCount > 0) { // Clicked on a players Virtue Gump icon
            serial_t targSer = tSock->GetDWord(19);
            targChar = CalcCharObjFromSer(targSer);
        }
        else { // Clicked an item on the virtue gump
            targChar = CalcCharObjFromSer(id);
        }
        
        cScript *toExecute = nullptr;
        std::vector<std::uint16_t> scriptTriggers = tSock->CurrcharObj()->GetScriptTriggers();
        for (auto scriptTrig : scriptTriggers) {
            toExecute = JSMapping->GetScript(scriptTrig);
            if (toExecute != nullptr) {
                if (toExecute->OnVirtueGumpPress(tSock->CurrcharObj(), targChar, buttonId) == 1) {
                    return true;
                }
            }
        }
        
        toExecute = JSMapping->GetScript(static_cast<std::uint16_t>(0)); // Global script
        if (toExecute != nullptr) {
            toExecute->OnVirtueGumpPress(tSock->CurrcharObj(), targChar, buttonId);
        }
        
        return true;
    }
    
    textOffset = 19 + (4 * switchCount);
    textCount = tSock->GetDWord(textOffset);
    
#if defined(UOX_DEBUG_MODE)
    Console::shared() << "    TextOffset : " << textOffset << myendl;
    Console::shared() << "     TextCount : " << textCount << myendl;
#endif
    
    BuildTextLocations();
    
    if (buttonId > 10000 && buttonId < 20000) {
        BuildGumpFromScripts(tSock, static_cast<std::uint16_t>(buttonId - 10000));
        return true;
    }
    if (gumpId >= 8000 && gumpId <= 8020) {
        GuildSys->GumpChoice(tSock);
        // guild collection call goes here
        return true;
    }
    else if (gumpId >= 0xFFFF) { // script gump
        cScript *toExecute = JSMapping->GetScript((gumpId - 0xFFFF));
        if (toExecute != nullptr) {
            toExecute->HandleGumpPress(this);
        }
    }
    else if (gumpId == 21) { // Multi functional gump
        MultiGumpCallback(tSock, id, buttonId);
        return true;
    }
    else if (gumpId > 13)
        return true; // increase this value with each new gump added.
    
    if (buttonId == 1) {
        if (gumpId == 4) {
            WhoList->GMLeave();
        }
        else if (gumpId == 11) {
            OffList->GMLeave();
        }
        return true;
    }
    
    // gump types
    // TBD  To Be Done
    // TBDU To Be Decided Upon
    // 1	Tweak Item
    // 2	Tweak Char
    // 3	Townstones
    // 4	WhoList
    // 7	Accounts (TBR)
    // 8	Racial editor (TBDU)
    // 9	Add menu
    // 10	Unused
    // 11	Who's Offline
    // 12	New Make Menu
    // 13	HOWTO
    
#if defined(UOX_DEBUG_MODE)
    Console::shared() << "Type is " << gumpId << " button is " << buttonId << myendl;
#endif
    
    switch (gumpId) {
        case 3:
            HandleTownstoneButton(tSock, buttonId, id, gumpId);
            break; // Townstones
        case 4:
            WhoList->ButtonSelect(tSock, static_cast<std::uint16_t>(buttonId), static_cast<std::uint8_t>(gumpId));
            break; // Wholist
        case 7:    // Accounts
            CChar *c;
            c = CalcCharObjFromSer(id);
            if (ValidateObject(c)) {
                HandleAccountButton(tSock, buttonId, c);
            }
            break;
        case 8:
            HandleAccountModButton(this);
            break; // Race Editor
        case 9:
            HandleAddMenuButton(tSock, buttonId);
            break; // Add Menu
        case 11:
            OffList->ButtonSelect(tSock, static_cast<std::uint16_t>(buttonId), static_cast<std::uint8_t>(gumpId));
            break; // Who's Offline
        case 12:
            Skills->HandleMakeMenu(tSock, buttonId, static_cast<std::int32_t>(tSock->AddId()));
            break; // New Make Menu
        case 13:
            HandleHowToButton(tSock, buttonId);
            break; // Howto
        default:
            break;
    }
    return true;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CPIGumpInput::HandleTownstoneText()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles new values for townstones
// o------------------------------------------------------------------------------------------------o
void CPIGumpInput::HandleTownstoneText(std::uint8_t index) {
    CChar *mChar = tSock->CurrcharObj();
    std::uint16_t resourceId;
    std::uint32_t amountToDonate;
    switch (index) {
        case 6: { // it's our donate resource button
            CTownRegion *ourRegion = worldMain.townRegions[mChar->GetTown()];
            amountToDonate = static_cast<std::uint32_t>(std::stoul(reply, nullptr, 0));
            if (amountToDonate == 0) {
                tSock->SysMessage(562); // You are donating nothing!
                return;
            }
            resourceId = ourRegion->GetResourceId();
            std::uint32_t numResources = GetItemAmount(mChar, resourceId);
            
            if (amountToDonate > numResources) {
                tSock->SysMessage(563, numResources); // You have insufficient resources to donate that
                // much!  You only have %i of them.
            }
            else {
                DeleteItemAmount(mChar, amountToDonate, resourceId);
                ourRegion->DonateResource(tSock, amountToDonate);
            }
            break;
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CPIGumpInput::Handle()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles new values for gumps
// o------------------------------------------------------------------------------------------------o
bool CPIGumpInput::Handle() {
    switch (type) {
        case 3:
            HandleTownstoneText(index);
            break;
        case 20: {
            CChar *mChar = CalcCharObjFromSer(id);
            if (ValidateObject(mChar)) {
                CSocket *mSock = mChar->GetSocket();
                if (mSock != nullptr) {
                    std::uint32_t scriptNo = mSock->AddId();
                    if (scriptNo >= 0xFFFF) {
                        cScript *toExecute = JSMapping->GetScript((scriptNo - 0xFFFF));
                        if (toExecute != nullptr) {
                            toExecute->HandleGumpInput(this);
                        }
                        mSock->AddId(0);
                    }
                }
            }
        }
            [[fallthrough]]; // Indicate to compiler that fallthrough is intentional to suppress warning
        case 100:
            GuildSys->GumpInput(this);
            break;
        default:
            break;
    }
    return true;
}

std::string GrabMenuData(std::string sect, size_t entryNum, std::string &tag) {
    std::string data;
    CScriptSection *sectionData = FileLookup->FindEntry(sect, menus_def);
    if (sectionData != nullptr) {
        if (sectionData->NumEntries() >= entryNum) {
            tag = sectionData->moveTo(entryNum);
            data = sectionData->GrabData();
        }
    }
    return data;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CPIGumpChoice::Handle()
//|	Modified	-	2/10/2003
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles choice "Help" menu, Tracking Menu, and Polymorph menu
// o------------------------------------------------------------------------------------------------o
bool CPIGumpChoice::Handle() {
    std::string sect;
    std::string tag;
    std::string data;
    std::uint16_t main = tSock->GetWord(5);
    std::uint16_t sub = tSock->GetWord(7);
    CChar *mChar = tSock->CurrcharObj();
    
    if (main >= JSGUMPMENUOFFSET) { // Between 0x4000 and 0xFFFF
        // Handle button presses via global JS script
        cScript *toExecute = JSMapping->GetScript(static_cast<std::uint16_t>(0));
        if (toExecute != nullptr) {
            if (toExecute->OnScrollingGumpPress(tSock, main, sub) == 0) {
                return true;
            }
        }
    }
    else if (main >= POLYMORPHMENUOFFSET) {
        sect = std::string("POLYMORPHMENU ") + util::ntos(main);
        data = GrabMenuData(sect, (static_cast<size_t>(sub) * 2), tag);
        if (!data.empty()) {
            if (main == POLYMORPHMENUOFFSET) {
                Magic->PolymorphMenu(tSock, static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)));
            }
            else {
                if (mChar->IsOnHorse()) {
                    DismountCreature(mChar); // can't be polymorphed on a horse
                }
                else if (mChar->IsFlying()) {
                    mChar->ToggleFlying();
                }
                Magic->Polymorph(tSock, static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)));
            }
        }
    }
    else if (main >= TRACKINGMENUOFFSET) {
        if (main == TRACKINGMENUOFFSET) {
            sect = std::string("TRACKINGMENU ") + util::ntos(main);
            data = GrabMenuData(sect, (static_cast<size_t>(sub) * 2), tag);
            if (!data.empty() && tag != "What") {
                Skills->CreateTrackingMenu(tSock, static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)));
            }
        }
        else {
            if (!Skills->CheckSkill(mChar, TRACKING, 0, 1000)) {
                tSock->SysMessage(575); // You fail your attempt at tracking.
                return true;
            }
            Skills->Tracking(tSock, static_cast<std::int32_t>(sub - 1));
        }
    }
    else if (main < ITEMMENUOFFSET) // GM Menus
    {
        sect = std::string("GMMENU ") + util::ntos(main);
        data = GrabMenuData(sect, (static_cast<size_t>(sub) * 2), tag);
        if (!tag.empty()) {
            HandleGumpCommand(tSock, tag, data);
        }
    }
    return true;
}

//============================================================================================
auto HandleCommonGump(CSocket *mSock, CScriptSection *gumpScript, std::uint16_t gumpIndex) -> void {
    auto mChar = mSock->CurrcharObj();
    std::string line;
    std::uint16_t modelId = 0;
    std::uint16_t modelColour = 0;
    std::string tag = gumpScript->First();
    std::string data = gumpScript->GrabData();
    line = tag + " "s + data;
    CPOpenGump toSend(*mChar);
    toSend.GumpIndex(gumpIndex);
    toSend.Question(line);
    for (tag = gumpScript->Next(); !gumpScript->AtEnd(); tag = gumpScript->Next()) {
        data = gumpScript->GrabData();
        modelId = static_cast<std::uint16_t>(stoul(tag, nullptr, 0));
        toSend.AddResponse(modelId, modelColour, data);
        tag = gumpScript->Next();
    }
    toSend.Finalize();
    mSock->Send(&toSend);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CGumpDisplay::AddData()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds the data to the gump to send
// o------------------------------------------------------------------------------------------------o
void CGumpDisplay::AddData(GumpInfo_st *toAdd) {
    GumpInfo_st *gAdd = new GumpInfo_st;
    gAdd->name = toAdd->name;
    gAdd->value = toAdd->value;
    gAdd->type = toAdd->type;
    gAdd->stringValue = toAdd->stringValue;
    gumpData.push_back(gAdd);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CGumpDisplay::AddData()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds the data to the gump to send
// o------------------------------------------------------------------------------------------------o
void CGumpDisplay::AddData(std::string toAdd, std::uint32_t value, std::uint8_t type) {
    if (toAdd.empty())
        return;
    
    GumpInfo_st *gAdd = new GumpInfo_st;
    gAdd->name = toAdd;
    gAdd->stringValue = "";
    gAdd->value = value;
    gAdd->type = type;
    gumpData.push_back(gAdd);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CGumpDisplay::AddData()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds the data to the gump to send
// o------------------------------------------------------------------------------------------------o
void CGumpDisplay::AddData(std::string toAdd, const std::string &toSet, std::uint8_t type) {
    if (toAdd.empty() && type != 7)
        return;
    
    GumpInfo_st *gAdd = new GumpInfo_st;
    gAdd->name = toAdd;
    gAdd->stringValue = toSet;
    gAdd->type = type;
    gumpData.push_back(gAdd);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CGumpDisplay::CGumpDisplay()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Begin CGumpDisplay stuff by setting the target, clearing any
// existing data, and setting the w / h
// o------------------------------------------------------------------------------------------------o
CGumpDisplay::CGumpDisplay(CSocket *target) : toSendTo(target) {
    gumpData.resize(0);
    width = 340;
    height = 320;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CGumpDisplay::CGumpDisplay( CSocket *target, std::uint16_t gumpWidth, std::uint16_t
// gumpHeight )
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Begin CGumpDisplay stuff by setting the target, clearing any
// existing data, and setting the w / h
// o------------------------------------------------------------------------------------------------o
CGumpDisplay::CGumpDisplay(CSocket *target, std::uint16_t gumpWidth, std::uint16_t gumpHeight) : width(gumpWidth), height(gumpHeight), toSendTo(target) {
    gumpData.resize(0);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CGumpDisplay::~CGumpDisplay()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Clear any existing CGumpDisplay data
// o------------------------------------------------------------------------------------------------o
CGumpDisplay::~CGumpDisplay() {
    for (size_t i = 0; i < gumpData.size(); ++i) {
        delete gumpData[i];
    }
    Delete();
    gumpData.resize(0);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CGumpDisplay::Delete()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Delete all CGumpDisplay entries
// o------------------------------------------------------------------------------------------------o
void CGumpDisplay::Delete() {
    one.resize(0);
    two.resize(0);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CGumpDisplay::setTitle()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Set a gumps title
// o------------------------------------------------------------------------------------------------o
void CGumpDisplay::setTitle(const std::string &newTitle) { title = newTitle; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SendVecsAsGump()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends to socket sock the data in one and two.  One is control, two
// is data
// o------------------------------------------------------------------------------------------------o
void SendVecsAsGump(CSocket *sock, std::vector<std::string> &one, std::vector<std::string> &two, std::uint32_t type, serial_t serial) {
    CPSendGumpMenu toSend;
    toSend.GumpId(type);
    toSend.UserId(serial);
    
    size_t line = 0;
    for (line = 0; line < one.size(); ++line) {
        toSend.addCommand(one[line]);
    }
    
    for (line = 0; line < two.size(); ++line) {
        toSend.addText(two[line]);
    }
    
    toSend.Finalize();
    sock->Send(&toSend);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CGumpDisplay::Send()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends gump data to the socket
// o------------------------------------------------------------------------------------------------o
void CGumpDisplay::Send(std::uint32_t gumpNum, bool isMenu, serial_t serial) {
    constexpr auto maxsize = 512;
    std::string temp;
    size_t i;
    std::uint32_t pagenum = 1, position = 40, linenum = 1, buttonnum = 7;
    std::uint8_t numToPage = 10, stringWidth = 10;
    
    if (!one.empty() && !two.empty()) {
        SendVecsAsGump(toSendTo, one, two, gumpNum, serial);
        return;
    }
    std::uint8_t ser1, ser2, ser3, ser4;
    //--static pages
    one.push_back("page 0");
    temp = oldstrutil::format(maxsize, "resizepic 0 0 %i %i %i", ServerConfig::shared().ushortValues[UShortValue::BACKGROUNDPIC], width, height);
    one.push_back(temp);
    temp = oldstrutil::format(maxsize, "button %i 15 %i %i 1 0 1", width - 40, ServerConfig::shared().ushortValues[UShortValue::BUTTONCANCEL], ServerConfig::shared().ushortValues[UShortValue::BUTTONCANCEL] + 1);
    one.push_back(temp);
    temp = oldstrutil::format(maxsize, "text 45 15 %i 0", ServerConfig::shared().ushortValues[UShortValue::TITLECOLOR]);
    one.push_back(temp);
    
    temp = oldstrutil::format(maxsize, "page %u", pagenum);
    one.push_back(temp);
    
    if (title.length() == 0) {
        temp = "General Gump";
    }
    else {
        temp = title;
    }
    two.push_back(temp);
    
    numToPage = static_cast<std::uint8_t>(((height - 30) / 20) - 2);
    stringWidth = static_cast<std::uint8_t>((width / 2) / 10);
    std::uint32_t lineForButton;
    for (i = 0, lineForButton = 0; i < gumpData.size(); ++i, ++lineForButton) {
        if (lineForButton > 0 && (!(lineForButton % numToPage))) {
            position = 40;
            ++pagenum;
            temp = oldstrutil::format(maxsize, "page %u", pagenum);
            one.push_back(temp);
        }
        if (gumpData[i]->type != 7) {
            temp = oldstrutil::format(maxsize, "text 50 %u %i %u", position, ServerConfig::shared().ushortValues[UShortValue::LEFTTEXTCOLOR], linenum++);
            one.push_back(temp);
            if (isMenu) {
                temp =
                oldstrutil::format(maxsize, "button 20 %u %i %i 1 0 %u", position, ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT], ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT] + 1, buttonnum);
                one.push_back(temp);
            }
            temp = oldstrutil::format(maxsize, "text %i %u %i %u", (width / 2) + 10, position, ServerConfig::shared().ushortValues[UShortValue::RIGHTTEXTCOLOR], linenum++);
            one.push_back(temp);
            two.push_back(gumpData[i]->name);
        }
        else {
            temp = oldstrutil::format(maxsize, "text 30 %u %i %u", position, ServerConfig::shared().ushortValues[UShortValue::LEFTTEXTCOLOR], linenum++);
            one.push_back(temp);
        }
        
        std::uint32_t value = gumpData[i]->value;
        switch (gumpData[i]->type) {
            case 0: // all numbers of sorts
                temp = std::to_string(value);
                break;
            case 1:
                temp = util::format("%x", static_cast<std::uint32_t>(value));
                break;
            case 2:
                ser1 = static_cast<std::uint8_t>(value >> 24);
                ser2 = static_cast<std::uint8_t>(value >> 16);
                ser3 = static_cast<std::uint8_t>(value >> 8);
                ser4 = static_cast<std::uint8_t>(value % 256);
                temp = util::format("%i %i %i %i", ser1, ser2, ser3, ser4);
                break;
            case 3:
                ser1 = static_cast<std::uint8_t>(value >> 24);
                ser2 = static_cast<std::uint8_t>(value >> 16);
                ser3 = static_cast<std::uint8_t>(value >> 8);
                ser4 = static_cast<std::uint8_t>(value % 256);
                temp = util::format("%x %x %x %x", ser1, ser2, ser3, ser4);
                break;
            case 4:
                if (gumpData[i]->stringValue.empty()) {
                    temp = "nullptr POINTER";
                }
                else {
                    temp = gumpData[i]->stringValue;
                }
                if (temp.size() > stringWidth) { // too wide for one line, CRAP!
                    std::string temp2;
                    std::string temp3;
                    
                    size_t tempWidth = temp.size() - stringWidth;
                    temp2 = temp.substr(0, stringWidth);
                    
                    two.push_back(temp2);
                    for (std::uint32_t tempCounter = 0; tempCounter < tempWidth / (static_cast<size_t>(stringWidth) * 2) + 1; ++tempCounter) {
                        // LOOKATME
                        position += 20;
                        ++lineForButton;
                        temp3 = util::format("text %i %u %i %u", 30, position, ServerConfig::shared().ushortValues[UShortValue::RIGHTTEXTCOLOR], linenum++);
                        one.push_back(temp3);
                        auto remaining = std::min<std::size_t>((temp.size() - (static_cast<size_t>(tempCounter) + 1) * static_cast<size_t>(stringWidth) * 2), static_cast<size_t>(stringWidth) * 2);
                        
                        temp2 = temp.substr(static_cast<size_t>(stringWidth) + static_cast<size_t>(tempCounter) * static_cast<size_t>(stringWidth) * 2, remaining);
                        two.push_back(temp2);
                    }
                    // be stupid for the moment and do no text wrapping over pages
                }
                else {
                    two.push_back(temp);
                }
                break;
            case 5:
                ser1 = static_cast<std::uint8_t>(value >> 8);
                ser2 = static_cast<std::uint8_t>(value % 256);
                temp = util::format("0x%02x%02x", ser1, ser2);
                break;
            case 6:
                ser1 = static_cast<std::uint8_t>(value >> 8);
                ser2 = static_cast<std::uint8_t>(value % 256);
                temp = util::format("%i %i", ser1, ser2);
                break;
            case 7:
                if (gumpData[i]->stringValue.empty()) {
                    temp = "Null Pointer";
                }
                else {
                    temp = gumpData[i]->stringValue;
                }
                std::int32_t sWidth;
                sWidth = stringWidth * 2;
                if (temp.size() > static_cast<size_t>(sWidth)) // too wide for one line, CRAP!
                {
                    std::string temp2;
                    std::string temp3;
                    size_t tempWidth = temp.size() - sWidth;
                    temp2 = temp.substr(0, sWidth);
                    
                    two.push_back(temp2);
                    for (std::uint32_t tempCounter = 0; tempCounter < tempWidth / sWidth + 1; ++tempCounter) {
                        position += 20;
                        ++lineForButton;
                        temp3 = oldstrutil::format(512, "text %i %u %i %u", 30, position, ServerConfig::shared().ushortValues[UShortValue::LEFTTEXTCOLOR], linenum++);
                        one.push_back(temp3);
                        auto remaining =
                        std::min<std::size_t>(temp.size() - (static_cast<size_t>(tempCounter) + 1) * static_cast<size_t>(sWidth), static_cast<std::size_t>(sWidth));
                        
                        temp2 = temp.substr((static_cast<size_t>(tempCounter) + 1) * static_cast<size_t>(sWidth), remaining);
                        
                        two.push_back(temp2);
                    }
                    // be stupid for the moment and do no text wrapping over pages
                }
                else {
                    two.push_back(temp);
                }
                break;
            case 8:
                if (gumpData[i]->stringValue.empty()) {
                    temp = "0.00";
                }
                else {
                    
                    temp = util::format("%.2f", std::stof(gumpData[i]->stringValue));
                }
                break;
            default:
                temp = std::to_string(value);
                break;
        }
        if (gumpData[i]->type != 4 && gumpData[i]->type != 7) {
            two.push_back(temp);
        }
        position += 20;
        ++buttonnum;
    }
    
    pagenum = 1;
    for (i = 0; static_cast<std::uint32_t>(i) <= lineForButton; i += numToPage) {
        temp = "page " + std::to_string(pagenum);
        one.push_back(temp);
        if (i >= 10) {
            temp = util::format("button 10 %i %i %i 0 %u", height - 40, ServerConfig::shared().ushortValues[UShortValue::BUTTONLEFT], ServerConfig::shared().ushortValues[UShortValue::BUTTONLEFT] + 1, pagenum - 1); // back button
            one.push_back(temp);
        }
        if (lineForButton > numToPage && static_cast<std::uint32_t>((i + numToPage)) < lineForButton) {
            temp = util::format("button %i %i %i %i 0 %u", width - 40, height - 40, ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT], ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT] + 1, pagenum + 1); // forward button
            one.push_back(temp);
        }
        ++pagenum;
    }
    SendVecsAsGump(toSendTo, one, two, gumpNum, serial);
}
