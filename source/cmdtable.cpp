//
//	This code is an attempt to clean up the messy "if/then/else" routines
//	currently in use for GM commands, as well as adding more functionality
//	and more potential for functionality.
//
//	Current features:
//	- Actual table of commands to execute, what perms are required, dialog
//	messages for target commands, etc handled by a central system
//
//
#include "cmdtable.h"

#include <fstream>

#include "cbaseobject.h"
#include "cchar.h"
#include "ceffects.h"
#include "cgump.h"
#include "chtmlsystem.h"
#include "citem.h"
#include "classes.h"
#include "commands.h"
#include "cpacketsend.h"
#include "craces.h"
#include "csocket.h"
#include "cspawnregion.h"
#include "cweather.hpp"
#include "dictionary.h"
#include "funcdecl.h"
#include "magic.h"
#include "msgboard.h"
#include "objectfactory.h"
#include "pagevector.h"
#include "regions.h"
#include "configuration/serverconfig.hpp"
#include "speech.h"
#include "stringutility.hpp"
#include "teffect.h"
#include "townregion.h"
#include "useful.h"
#include "utility/strutil.hpp"
#include "wholist.h"
#include "worldmain.h"

using namespace std::string_literals ;

void endMessage(std::int32_t x);
void HandleGumpCommand(CSocket *s, std::string cmd, std::string data);
void Restock(bool stockAll);
void sysBroadcast(const std::string &txt);
void HandleHowTo(CSocket *sock, std::int32_t cmdNumber);

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CloseCall()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Closes an open call in the Que
// o------------------------------------------------------------------------------------------------o
void CloseCall(CSocket *s, bool isGM) {
    CChar *mChar = s->CurrcharObj();
    if (mChar->GetCallNum() != 0) {
        std::uint16_t sysMessage = 1285; // Call removed from the GM queue.
        PageVector *tmpVector = nullptr;
        if (isGM) {
            tmpVector = GMQueue;
        }
        else {
            tmpVector = CounselorQueue;
            ++sysMessage;
        }

        if (tmpVector->GotoPos(tmpVector->FindCallNum(mChar->GetCallNum()))) {
            tmpVector->Remove();
            mChar->SetCallNum(0);
            s->SysMessage(sysMessage);
        }
    }
    else {
        s->SysMessage(1287); // You are currently not on a call.
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CurrentCall()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Send GM/Counselor to the current call in the queue
// o------------------------------------------------------------------------------------------------o
void CurrentCall(CSocket *s, bool isGM) {
    CChar *mChar = s->CurrcharObj();
    if (mChar->GetCallNum() != 0) {
        PageVector *tmpVector = nullptr;
        if (isGM) {
            tmpVector = GMQueue;
        }
        else {
            tmpVector = CounselorQueue;
        }

        if (tmpVector->GotoPos(tmpVector->FindCallNum(mChar->GetCallNum()))) {
            CHelpRequest *currentPage = tmpVector->Current();
            CChar *i = CalcCharObjFromSer(currentPage->WhoPaging());
            if (ValidateObject(i)) {
                s->SysMessage(73); // Transporting to your current call.
                mChar->SetLocation(i);
            }
            currentPage->IsHandled(true);
        }
        else {
            s->SysMessage(75); // No such call exists
        }
    }
    else {
        s->SysMessage(72); // You are not currently on a call.
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	NextCall()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Send GM/Counsellor to next call in the que
// o------------------------------------------------------------------------------------------------o
void NextCall(CSocket *s, bool isGM) {
    CChar *mChar = s->CurrcharObj();
    if (mChar->GetCallNum() != 0) {
        CloseCall(s, isGM);
    }
    if (isGM) {
        if (!GMQueue->AnswerNextCall(s, mChar)) {
            s->SysMessage(347); // The GM queue is currently empty.
        }
    }
    else // Player is a counselor
    {
        if (!CounselorQueue->AnswerNextCall(s, mChar)) {
            s->SysMessage(348); // The Counselor queue is currently empty.
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	FixSpawnFunctor()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Replaces legacy spawner objects that have incorrect item type
// o------------------------------------------------------------------------------------------------o
bool FixSpawnFunctor(CBaseObject *a, [[maybe_unused]] std::uint32_t &b, [[maybe_unused]] void *extraData) {
    bool retVal = true;
    if (ValidateObject(a)) {
        CItem *i = static_cast<CItem *>(a);
        itemtypes_t iType = i->GetType();
        if (iType == IT_ITEMSPAWNER || iType == IT_NPCSPAWNER || iType == IT_SPAWNCONT ||
            iType == IT_LOCKEDSPAWNCONT || iType == IT_UNLOCKABLESPAWNCONT ||
            iType == IT_AREASPAWNER || iType == IT_ESCORTNPCSPAWNER) {
            if (i->GetObjType() != CBaseObject::OT_SPAWNER) {
                CSpawnItem *spawnItem = static_cast<CSpawnItem *>(i->Dupe(CBaseObject::OT_SPAWNER));
                if (ValidateObject(spawnItem)) {
                    spawnItem->SetInterval(0, static_cast<std::uint8_t>(i->GetTempVar(CITV_MOREY)));
                    spawnItem->SetInterval(1, static_cast<std::uint8_t>(i->GetTempVar(CITV_MOREX)));
                    spawnItem->SetSpawnSection(i->GetDesc());
                    spawnItem->IsSectionAList(i->IsSpawnerList());
                }
                i->Delete();
            }
        }
    }
    return retVal;
}

void command_fixspawn() {
    std::uint32_t b = 0;
    ObjectFactory::shared().IterateOver(CBaseObject::OT_ITEM, b, nullptr, &FixSpawnFunctor);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_AddAccount()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	GM command for adding new user accounts while in-game
// o------------------------------------------------------------------------------------------------o
void Command_AddAccount(CSocket *s) {
    VALIDATESOCKET(s);
    if (serverCommands.numArguments() > 1) {
        std::string newUsername = serverCommands.commandString(2, 2);
        std::string newPassword = serverCommands.commandString(3, 3);
        std::uint16_t newFlags = 0x0000;

        if (serverCommands.numArguments() > 2) {
            newFlags = util::ston<std::uint16_t>(serverCommands.commandString(4, 4));
        }

        if (newUsername == "" || newPassword == "") {
            s->SysMessage(9018); // Unable to add account, insufficient data provided (syntax:
                                 // [username] [password] [flags])
            return;
        }

        // ok we need to add the account now. We will rely in the internalaccountscreation system
        // for this
        AccountEntry &actbTemp = Account::shared()[newUsername];
        if (actbTemp.accountNumber == AccountEntry::INVALID_ACCOUNT) {
            Account::shared().createAccount(newUsername, newPassword, newFlags, "NA");
            Console::shared() << "o Account added ingame: " << newUsername << ":" << newPassword
                              << ":" << newFlags << myendl;
            s->SysMessage(9019, newUsername.c_str(), newPassword.c_str(),
                          newFlags); // Account Added: %s:%s:%i
        }
        else {
            Console::shared()
                << "o Account was not added, an account with that username already exists!"
                << myendl;
            s->SysMessage(9020); // Account not added, an account with that username already exists!
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_GetLight()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Displays current in-game light level as a system message
// o------------------------------------------------------------------------------------------------o
//|	Notes		-	Needs redoing to support new (weather-based) lighting system
// o------------------------------------------------------------------------------------------------o
void Command_GetLight(CSocket *s) {
    VALIDATESOCKET(s);
    CChar *mChar = s->CurrcharObj();
    if (!ValidateObject(mChar))
        return;

    CTownRegion *tRegion = mChar->GetRegion();
    std::uint16_t weatherId = tRegion->GetWeather();
    CWeather *sys = Weather->Weather(weatherId);
    if (sys != nullptr) {
        const R32 lightMin = sys->LightMin();
        const R32 lightMax = sys->LightMax();
        if (lightMin < 300 && lightMax < 300) {
            R32 i = sys->CurrentLight();
            if (Races->VisLevel(mChar->GetRace()) > i) {
                s->SysMessage(1632, 0); // Current light level is %i
            }
            else {
                s->SysMessage(
                    1632, static_cast<lightlevel_t>(RoundNumber(
                              i - Races->VisLevel(mChar->GetRace())))); // Current light level is %i
            }
        }
        else {
            s->SysMessage(
                1632,
                cwmWorldState->ServerData()->worldLightCurrentLevel()); // Current light level is %i
        }
    }
    else {
        s->SysMessage(
            1632,
            cwmWorldState->ServerData()->worldLightCurrentLevel()); // Current light level is %i
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_SetPost()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets bulletin board posting mode for user
// o------------------------------------------------------------------------------------------------o
void Command_SetPost(CSocket *s) {
    VALIDATESOCKET(s);

    CChar *mChar = s->CurrcharObj();
    if (!ValidateObject(mChar))
        return;

    PostTypes type = PT_LOCAL;
    std::string upperCommand = util::upper(serverCommands.commandString(2, 2));
    if (upperCommand == "GLOBAL") // user's next post appears in ALL bulletin boards
    {
        type = PT_GLOBAL;
    }
    else if (upperCommand ==
             "REGIONAL") // user's next post appears in all bulletin boards in current region
    {
        type = PT_REGIONAL;
    }
    else if (upperCommand ==
             "LOCAL") // user's next post appears only locally in the next bulletin board used
    {
        type = PT_LOCAL;
    }

    mChar->SetPostType(static_cast<std::uint8_t>(type));

    switch (type) {
    case PT_LOCAL:
        s->SysMessage(726);
        break; // Post type set to LOCAL.
    case PT_REGIONAL:
        s->SysMessage(727);
        break; // Post type set to REGION
    case PT_GLOBAL:
        s->SysMessage(728);
        break; // Post type set to GLOBAL.
    default:
        s->SysMessage(725);
        break; // Invalid post type.
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_GetPost()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns current bulletin board posting mode for user
// o------------------------------------------------------------------------------------------------o
void Command_GetPost(CSocket *s) {
    VALIDATESOCKET(s);

    CChar *mChar = s->CurrcharObj();
    if (!ValidateObject(mChar))
        return;

    switch (mChar->GetPostType()) {
    case PT_LOCAL:
        s->SysMessage(722);
        break; // Currently posting LOCAL messages.
    case PT_REGIONAL:
        s->SysMessage(723);
        break; // Currently posting REGIONAL messages.
    case PT_GLOBAL:
        s->SysMessage(724);
        break; // Currently posting GLOBAL messages.
    default:
        s->SysMessage(725);
        break; // Invalid post type.
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_ShowIds()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Display the serial number of every item on user's screen
// o------------------------------------------------------------------------------------------------o
auto Command_ShowIds(CSocket *s) -> void {
    VALIDATESOCKET(s);
    CChar *mChar = s->CurrcharObj();
    CMapRegion *Cell = MapRegion->GetMapRegion(mChar);

    if (Cell) {
        auto regChars = Cell->GetCharList();
        for (auto &toShow : regChars->collection()) {
            if (ValidateObject(toShow)) {
                if (CharInRange(mChar, toShow)) {
                    s->ShowCharName(toShow, true);
                }
            }
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_Tile()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	(h) Tiles the item specified over a square area.
// o------------------------------------------------------------------------------------------------o
//|	Notes		-	To find the hexidecimal ID code for an item to tile,
//|					either create the item with /add or find it in the
//|					world, and get /ISTATS on the object to get its ID code.
// o------------------------------------------------------------------------------------------------o
void Command_Tile(CSocket *s) {
    VALIDATESOCKET(s);
    if (serverCommands.numArguments() == 0)
        return;

    std::uint16_t targId = 0;
    if (util::upper(serverCommands.commandString(2, 2)) == "STATIC") {
        targId = static_cast<std::uint16_t>(serverCommands.argument(2));
    }
    else {
        targId = static_cast<std::uint16_t>(serverCommands.argument(1));
    }

    // Reset tempint2 on socket
    s->TempInt2(0);

    if (serverCommands.numArguments() == 7 || serverCommands.numArguments() == 8) {
        // tile itemId x1 y1 x2 y2 z rndVal
        std::int32_t rndVal = 0;
        std::uint16_t rndId = 0;

        if (serverCommands.numArguments() == 8) {
            rndVal = static_cast<std::int32_t>(serverCommands.argument(7));
        }

        // tile itemId x1 y1 x2 y2 z
        std::int16_t x1 = static_cast<std::int16_t>(serverCommands.argument(2));
        std::int16_t x2 = static_cast<std::int16_t>(serverCommands.argument(3));
        std::int16_t y1 = static_cast<std::int16_t>(serverCommands.argument(4));
        std::int16_t y2 = static_cast<std::int16_t>(serverCommands.argument(5));
        std::int8_t z = static_cast<std::int8_t>(serverCommands.argument(6));

        for (std::int16_t x = x1; x <= x2; ++x) {
            for (std::int16_t y = y1; y <= y2; ++y) {
                rndId = targId + RandomNum(static_cast<std::uint16_t>(0), static_cast<std::uint16_t>(rndVal));
                CItem *a = Items->CreateItem(nullptr, s->CurrcharObj(), rndId, 1, 0, CBaseObject::OT_ITEM);
                if (ValidateObject(a)) // crash prevention
                {
                    a->SetLocation(x, y, z);
                    a->SetDecayable(false);
                }
            }
        }
    }
    else if (serverCommands.numArguments() == 4) {
        // tile static itemId rndVal
        s->ClickX(-1);
        s->ClickY(-1);
        s->TempInt2(static_cast<std::int32_t>(serverCommands.argument(3)));

        s->AddId1(static_cast<std::uint8_t>(targId >> 8));
        s->AddId2(static_cast<std::uint8_t>(targId % 256));
        s->SendTargetCursor(0, TARGET_TILING, 0, 24);
    }
    else if (serverCommands.numArguments() == 3) {
        // tile itemId rndVal
        // tile static itemId
        s->ClickX(-1);
        s->ClickY(-1);
        if (util::upper(serverCommands.commandString(2, 2)) != "STATIC") {
            s->TempInt2(static_cast<std::int32_t>(serverCommands.argument(2)));
        }

        s->AddId1(static_cast<std::uint8_t>(targId >> 8));
        s->AddId2(static_cast<std::uint8_t>(targId % 256));
        s->SendTargetCursor(0, TARGET_TILING, 0, 24);
    }
    else if (serverCommands.numArguments() == 2) {
        // tile itemId
        s->ClickX(-1);
        s->ClickY(-1);

        s->AddId1(static_cast<std::uint8_t>(targId >> 8));
        s->AddId2(static_cast<std::uint8_t>(targId % 256));
        s->SendTargetCursor(0, TARGET_TILING, 0, 24);
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_Save()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Saves the current world data into .WSC files
// o------------------------------------------------------------------------------------------------o
void Command_Save() {
    if (cwmWorldState->GetWorldSaveProgress() != SS_SAVING) {
        cwmWorldState->SaveNewWorld(true);
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_Dye()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	(h h/nothing) Dyes an item a specific color, or brings up
//|					a dyeing menu if no color is specified.
// o------------------------------------------------------------------------------------------------o
void Command_Dye(CSocket *s) {
    VALIDATESOCKET(s);
    s->DyeAll(1);
    if (serverCommands.numArguments() == 2) {
        std::uint16_t tNum = static_cast<std::int16_t>(serverCommands.argument(1));
        s->AddId1(static_cast<std::uint8_t>(tNum >> 8));
        s->AddId2(static_cast<std::uint8_t>(tNum % 256));
    }
    else if (serverCommands.numArguments() == 3) {
        s->AddId1(static_cast<std::uint8_t>(serverCommands.argument(1)));
        s->AddId2(static_cast<std::uint8_t>(serverCommands.argument(2)));
    }
    else {
        s->AddId1(0xFF);
        s->AddId2(0xFF);
    }
    s->SendTargetCursor(0, TARGET_DYE, 0, 31);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_SetTime()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	(d d) Sets the current UO time in hours and minutes.
// o------------------------------------------------------------------------------------------------o
void Command_SetTime() {
    if (serverCommands.numArguments() == 3) {
        std::uint8_t newhours = static_cast<std::uint8_t>(serverCommands.argument(1));
        std::uint8_t newminutes = static_cast<std::uint8_t>(serverCommands.argument(2));
        if ((newhours < 25) && (newhours > 0) && (newminutes < 60)) {
            cwmWorldState->ServerData()->ServerTimeAMPM((newhours > 12));
            if (newhours > 12) {
                cwmWorldState->ServerData()->ServerTimeHours(static_cast<std::uint8_t>(newhours - 12));
            }
            else {
                cwmWorldState->ServerData()->ServerTimeHours(newhours);
            }
            cwmWorldState->ServerData()->ServerTimeMinutes(newminutes);
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_Shutdown()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	(d) Shuts down the server. Argument is how many minutes
//|					until shutdown.
// o------------------------------------------------------------------------------------------------o
void Command_Shutdown() {
    if (serverCommands.numArguments() == 2) {
        cwmWorldState->SetEndTime(BuildTimeValue(static_cast<R32>(serverCommands.argument(1))));
        if (serverCommands.argument(1) == 0) {
            cwmWorldState->SetEndTime(0);
            sysBroadcast(Dictionary->GetEntry(36));
        }
        else {
            endMessage(0);
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_Tell()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	(d text) Sends an anonymous message to the user logged in under the
// specified slot.
// o------------------------------------------------------------------------------------------------o
void Command_Tell(CSocket *s) {
    VALIDATESOCKET(s);
    if (serverCommands.numArguments() > 2) {
        CSocket *i = Network->GetSockPtr(serverCommands.argument(1));
        std::string txt = serverCommands.commandString(3);
        if (i == nullptr || txt.empty())
            return;

        CChar *mChar = i->CurrcharObj();
        CChar *tChar = s->CurrcharObj();
        std::string temp = mChar->GetNameRequest(tChar, 0) + " tells " +
                           tChar->GetNameRequest(mChar, NRS_SPEECH) + ": " + txt;

        if (cwmWorldState->ServerData()->useUnicodeMessages()) {
            CPUnicodeMessage unicodeMessage;
            unicodeMessage.Message(temp);
            unicodeMessage.Font(static_cast<fonttype_t>(mChar->GetFontType()));
            if (mChar->GetSayColour() == 0x1700) {
                unicodeMessage.Colour(0x5A);
            }
            else if (mChar->GetSayColour() == 0x0) {
                unicodeMessage.Colour(0x5A);
            }
            else {
                unicodeMessage.Colour(mChar->GetSayColour());
            }
            unicodeMessage.Type(TALK);
            unicodeMessage.Language("ENG");
            unicodeMessage.Name(mChar->GetNameRequest(tChar, NRS_SPEECH));
            unicodeMessage.ID(INVALIDID);
            unicodeMessage.Serial(mChar->GetSerial());
            s->Send(&unicodeMessage);
        }
        else {
            CSpeechEntry &toAdd = SpeechSys->Add();
            toAdd.Font(static_cast<fonttype_t>(mChar->GetFontType()));
            toAdd.Speech(temp);
            toAdd.Speaker(mChar->GetSerial());
            toAdd.SpokenTo(tChar->GetSerial());
            toAdd.Type(TALK);
            toAdd.At(cwmWorldState->GetUICurrentTime());
            toAdd.TargType(SPTRG_INDIVIDUAL);
            if (mChar->GetSayColour() == 0x1700) {
                toAdd.Colour(0x5A);
            }
            else if (mChar->GetSayColour() == 0x0) {
                toAdd.Colour(0x5A);
            }
            else {
                toAdd.Colour(mChar->GetSayColour());
            }
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_GmMenu()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	(d) Opens the specified GM Menu from dfndata/menus.dfn
// o------------------------------------------------------------------------------------------------o
void Command_GmMenu(CSocket *s) {
    VALIDATESOCKET(s);
    if (serverCommands.numArguments() == 2) {
        CPIHelpRequest toHandle(s, static_cast<std::uint16_t>(serverCommands.argument(1)));
        toHandle.Handle();
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_Command()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Executes a trigger scripting command.
// o------------------------------------------------------------------------------------------------o
void Command_Command(CSocket *s) {
    VALIDATESOCKET(s);
    if (serverCommands.numArguments() > 1) {
        HandleGumpCommand(s, util::upper(serverCommands.commandString(2, 2)),
                          util::upper(serverCommands.commandString(3)));
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_MemStats()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Display some information about cache and memory usage.
// o------------------------------------------------------------------------------------------------o
void Command_MemStats(CSocket *s) {
    VALIDATESOCKET(s);
    size_t charsSize = ObjectFactory::shared().SizeOfObjects(CBaseObject::OT_CHAR);
    size_t itemsSize = ObjectFactory::shared().SizeOfObjects(CBaseObject::OT_ITEM);
    size_t spellsSize = 69 * sizeof(CSpellInfo);
    size_t teffectsSize = sizeof(CTEffect) * cwmWorldState->tempEffects.Num();
    size_t regionsSize = sizeof(CTownRegion) * cwmWorldState->townRegions.size();
    size_t spawnregionsSize = sizeof(CSpawnRegion) * cwmWorldState->spawnRegions.size();
    size_t total =
        charsSize + itemsSize + spellsSize + regionsSize + spawnregionsSize + teffectsSize;
    CGumpDisplay cacheStats(s, 350, 345);
    cacheStats.setTitle("UOX Memory Information (sizes in bytes)");
    cacheStats.AddData("Total Memory Usage: ", static_cast<std::uint32_t>(total));
    cacheStats.AddData(" Characters: ", ObjectFactory::shared().CountOfObjects(CBaseObject::OT_CHAR));
    cacheStats.AddData("  Allocated Memory: ", static_cast<std::uint32_t>(charsSize));
    cacheStats.AddData("  CChar: ", sizeof(CChar));
    cacheStats.AddData(" Items: ", ObjectFactory::shared().CountOfObjects(CBaseObject::OT_ITEM));
    cacheStats.AddData("  Allocated Memory: ", static_cast<std::uint32_t>(itemsSize));
    cacheStats.AddData("  CItem: ", sizeof(CItem));
    cacheStats.AddData(" Spells Size: ", static_cast<std::uint32_t>(spellsSize));
    cacheStats.AddData(" TEffects: ", static_cast<std::uint32_t>(cwmWorldState->tempEffects.Num()));
    cacheStats.AddData("  Allocated Memory: ", static_cast<std::uint32_t>(teffectsSize));
    cacheStats.AddData("  TEffect: ", sizeof(CTEffect));
    cacheStats.AddData(" Regions Size: ", static_cast<std::uint32_t>(cwmWorldState->townRegions.size()));
    cacheStats.AddData("  Allocated Memory: ", static_cast<std::uint32_t>(regionsSize));
    cacheStats.AddData("  CTownRegion: ", sizeof(CTownRegion));
    cacheStats.AddData(" SpawnRegions ", static_cast<std::uint32_t>(cwmWorldState->spawnRegions.size()));
    cacheStats.AddData("  Allocated Memory: ", static_cast<std::uint32_t>(spawnregionsSize));
    cacheStats.AddData("  CSpawnRegion: ", sizeof(CSpawnRegion));
    cacheStats.Send(0, false, INVALIDSERIAL);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_Restock()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	(nothing / s) Forces a manual vendor restock
// o------------------------------------------------------------------------------------------------o
void Command_Restock(CSocket *s) {
    VALIDATESOCKET(s);
    if (util::upper(serverCommands.commandString(2, 2)) == "ALL") {
        Restock(true);
        s->SysMessage(55); // Restocking all shops to their maximums
    }
    else {
        Restock(false);
        s->SysMessage(54); // Manual shop restock has occurred.
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_SetShopRestockRate()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	(d) Sets the universe's shop restock rate
// o------------------------------------------------------------------------------------------------o
void Command_SetShopRestockRate(CSocket *s) {
    VALIDATESOCKET(s);
    if (serverCommands.numArguments() == 2) {
        cwmWorldState->ServerData()->SystemTimer(tSERVER_SHOPSPAWN,
                                                 static_cast<std::uint16_t>(serverCommands.argument(1)));
        s->SysMessage(56); // NPC shop restock rate changed.
    }
    else
        s->SysMessage(57); // Invalid number of parameters.
}

bool RespawnFunctor(CBaseObject *a, [[maybe_unused]] std::uint32_t &b, [[maybe_unused]] void *extraData) {
    bool retVal = true;
    if (ValidateObject(a)) {
        CItem *i = static_cast<CItem *>(a);
        itemtypes_t iType = i->GetType();
        if (iType == IT_ITEMSPAWNER || iType == IT_NPCSPAWNER || iType == IT_SPAWNCONT ||
            iType == IT_LOCKEDSPAWNCONT || iType == IT_UNLOCKABLESPAWNCONT ||
            iType == IT_AREASPAWNER || iType == IT_ESCORTNPCSPAWNER) {
            if (i->GetObjType() == CBaseObject::OT_SPAWNER) {
                CSpawnItem *spawnItem = static_cast<CSpawnItem *>(i);
                if (!spawnItem->DoRespawn()) {
                    spawnItem->SetTempTimer(BuildTimeValue(static_cast<R32>(RandomNum(
                        spawnItem->GetInterval(0) * 60, spawnItem->GetInterval(1) * 60))));
                }
            }
            else {
                i->SetType(IT_NOTYPE);
            }
        }
    }
    return retVal;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_Respawn()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Forces a respawn of all spawn regions
// o------------------------------------------------------------------------------------------------o
void Command_Respawn() {
    std::uint32_t spawnedItems = 0;
    std::uint32_t spawnedNpcs = 0;
    std::for_each(cwmWorldState->spawnRegions.begin(), cwmWorldState->spawnRegions.end(),
                  [&spawnedItems, &spawnedNpcs](std::pair<std::uint16_t, CSpawnRegion *> entry) {
                      if (entry.second) {
                          entry.second->DoRegionSpawn(spawnedItems, spawnedNpcs);
                      }
                  });

    std::uint32_t b = 0;
    ObjectFactory::shared().IterateOver(CBaseObject::OT_ITEM, b, nullptr, &RespawnFunctor);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_RegSpawn()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	(s/d) Forces a region spawn, First argument is region number or
//"ALL"
// o------------------------------------------------------------------------------------------------o
void Command_RegSpawn(CSocket *s) {
    VALIDATESOCKET(s);

    if (serverCommands.numArguments() == 2) {
        std::uint32_t itemsSpawned = 0;
        std::uint32_t npcsSpawned = 0;

        if (util::upper(serverCommands.commandString(2, 2)) == "ALL") {
            std::for_each(cwmWorldState->spawnRegions.begin(), cwmWorldState->spawnRegions.end(),
                          [&itemsSpawned, &npcsSpawned](std::pair<std::uint16_t, CSpawnRegion *> entry) {
                              if (entry.second) {
                                  entry.second->DoRegionSpawn(itemsSpawned, npcsSpawned);
                              }
                          });
            if (itemsSpawned || npcsSpawned) {
                s->SysMessage(9021, itemsSpawned, npcsSpawned,
                              cwmWorldState->spawnRegions
                                  .size()); // Spawned %u items and %u npcs in %u SpawnRegions
            }
            else {
                s->SysMessage(
                    9022, cwmWorldState->spawnRegions
                              .size()); // Failed to spawn any new items or npcs in %u SpawnRegions
            }
        }
        else {
            std::uint16_t spawnRegNum = static_cast<std::uint16_t>(serverCommands.argument(1));
            if (cwmWorldState->spawnRegions.find(spawnRegNum) !=
                cwmWorldState->spawnRegions.end()) {
                CSpawnRegion *spawnReg = cwmWorldState->spawnRegions[spawnRegNum];
                if (spawnReg != nullptr) {
                    spawnReg->DoRegionSpawn(itemsSpawned, npcsSpawned);
                    if (itemsSpawned || npcsSpawned) {
                        s->SysMessage(9023, spawnReg->GetName().c_str(), spawnRegNum, itemsSpawned,
                                      npcsSpawned); // Region: '%s'(%u) spawned %u items and %u npcs
                    }
                    else {
                        s->SysMessage(
                            9024, spawnReg->GetName().c_str(),
                            spawnRegNum); // Region: '%s'(%u) failed to spawn any new items or npcs
                    }
                    return;
                }
            }
            s->SysMessage(9025, spawnRegNum); // Invalid SpawnRegion %u
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_LoadDefaults()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads the hardcoded server defaults for various settings in uox.ini
// o------------------------------------------------------------------------------------------------o
void Command_LoadDefaults() { cwmWorldState->ServerData()->ResetDefaults(); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_CQ()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Display the Counselor queue when used with no arguments
//|					Can trigger other queue-related commands when arguments are
// provided
// o------------------------------------------------------------------------------------------------o
void Command_CQ(CSocket *s) {
    VALIDATESOCKET(s);
    if (serverCommands.numArguments() == 2) {
        std::string upperCommand = util::upper(serverCommands.commandString(2, 2));
        if (upperCommand == "NEXT") // Go to next call in Counselor queue
        {
            NextCall(s, false);
        }
        else if (upperCommand == "CLEAR") // Close and clear current call as resolved
        {
            CloseCall(s, false);
        }
        else if (upperCommand == "CURR") // Take Counselor to current call they are on
        {
            CurrentCall(s, false);
        }
        else if (upperCommand == "TRANSFER") // Transfer call from Counselor queue to GM queue
        {
            CChar *mChar = s->CurrcharObj();
            if (mChar->GetCallNum() != 0) {
                if (CounselorQueue->GotoPos(CounselorQueue->FindCallNum(mChar->GetCallNum()))) {
                    CHelpRequest pageToAdd;
                    CHelpRequest *currentPage = nullptr;
                    currentPage = CounselorQueue->Current();
                    pageToAdd.Reason(currentPage->Reason());
                    pageToAdd.WhoPaging(currentPage->WhoPaging());
                    pageToAdd.IsHandled(false);
                    pageToAdd.TimeOfPage(time(nullptr));
                    GMQueue->Add(&pageToAdd);
                    s->SysMessage(74); // Call successfully transferred to the GM queue.
                    CloseCall(s, false);
                }
                else {
                    s->SysMessage(75); // No such call exists
                }
            }
            else {
                s->SysMessage(72); // You are not currently on a call.
            }
        }
    }
    else {
        CounselorQueue->SendAsGump(s); // Show the Counselor queue, not GM queue
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_GQ()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Display the GM queue when used with no arguments
//|					Can trigger other queue-related commands when arguments are
// provided
// o------------------------------------------------------------------------------------------------o
void Command_GQ(CSocket *s) {
    VALIDATESOCKET(s);
    if (serverCommands.numArguments() == 2) {
        std::string upperCommand = util::upper(serverCommands.commandString(2, 2));
        if (upperCommand == "NEXT") // Go to next call in GM queue
        {
            NextCall(s, true);
        }
        else if (upperCommand == "CLEAR") // Close and clear current call as resolved
        {
            CloseCall(s, true);
        }
        else if (upperCommand == "CURR") // Take GM to current call they are on
        {
            CurrentCall(s, true);
        }
    }
    else {
        GMQueue->SendAsGump(s);
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_MineCheck()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	(d) Set the server mine check status to determine where mining will
// work |						0 - allow mining everywhere |
// 1 - mountainsides and cave floors |						2 - mining regions
// only
// o------------------------------------------------------------------------------------------------o
void Command_MineCheck() {
    if (serverCommands.numArguments() == 2) {
        cwmWorldState->ServerData()->MineCheck(static_cast<std::uint8_t>(serverCommands.argument(1)));
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_Guards()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Enables (ON) or disables (OFF) town guards globally
// o------------------------------------------------------------------------------------------------o
void Command_Guards() {
    if (util::upper(serverCommands.commandString(2, 2)) == "ON") {
        cwmWorldState->ServerData()->GuardStatus(true);
        sysBroadcast(Dictionary->GetEntry(61)); // Guards have been reactivated.
    }
    else if (util::upper(serverCommands.commandString(2, 2)) == "OFF") {
        cwmWorldState->ServerData()->GuardStatus(false);
        sysBroadcast(Dictionary->GetEntry(62)); // Warning: Guards have been deactivated globally.
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_Announce()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Enables (ON) or disables (OFF) announcement of world saves
// o------------------------------------------------------------------------------------------------o
void Command_Announce() {
    if (util::upper(serverCommands.commandString(2, 2)) == "ON") {
        cwmWorldState->ServerData()->ServerAnnounceSaves(true);
        sysBroadcast(Dictionary->GetEntry(63)); // WorldStat Saves will be displayed.
    }
    else if (util::upper(serverCommands.commandString(2, 2)) == "OFF") {
        cwmWorldState->ServerData()->ServerAnnounceSaves(false);
        sysBroadcast(Dictionary->GetEntry(64)); // WorldStat Saves will not be displayed.
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_PDump()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Display some performance information
// o------------------------------------------------------------------------------------------------o
void Command_PDump(CSocket *s) {
    VALIDATESOCKET(s);
    std::uint32_t networkTimeCount = cwmWorldState->ServerProfile()->NetworkTimeCount();
    std::uint32_t timerTimeCount = cwmWorldState->ServerProfile()->TimerTimeCount();
    std::uint32_t autoTimeCount = cwmWorldState->ServerProfile()->AutoTimeCount();
    std::uint32_t loopTimeCount = cwmWorldState->ServerProfile()->LoopTimeCount();

    s->SysMessage("Performance Dump:");
    s->SysMessage("Network code: %fmsec [%i]",
                  static_cast<R32>(static_cast<R32>(cwmWorldState->ServerProfile()->NetworkTime()) /
                                   static_cast<R32>(networkTimeCount)),
                  networkTimeCount);
    s->SysMessage("Timer code: %fmsec [%i]",
                  static_cast<R32>(static_cast<R32>(cwmWorldState->ServerProfile()->TimerTime()) /
                                   static_cast<R32>(timerTimeCount)),
                  timerTimeCount);
    s->SysMessage("Auto code: %fmsec [%i]",
                  static_cast<R32>(static_cast<R32>(cwmWorldState->ServerProfile()->AutoTime()) /
                                   static_cast<R32>(autoTimeCount)),
                  autoTimeCount);
    s->SysMessage("Loop Time: %fmsec [%i]",
                  static_cast<R32>(static_cast<R32>(cwmWorldState->ServerProfile()->LoopTime()) /
                                   static_cast<R32>(loopTimeCount)),
                  loopTimeCount);
    s->SysMessage(
        "Simulation Cycles/Sec: %f",
        (1000.0 *
         (1.0 / static_cast<R32>(static_cast<R32>(cwmWorldState->ServerProfile()->LoopTime()) /
                                 static_cast<R32>(loopTimeCount)))));
}
// o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_SpawnKill()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	(d) Kills spawns from the specified spawn region in SPAWN.DFN
// o------------------------------------------------------------------------------------------------o
auto Command_SpawnKill(CSocket *s) -> void {
    VALIDATESOCKET(s);
    if (serverCommands.numArguments() == 2) {
        std::uint16_t regNum = static_cast<std::uint16_t>(serverCommands.argument(1));
        if (cwmWorldState->spawnRegions.find(regNum) != cwmWorldState->spawnRegions.end()) {
            auto spawnReg = cwmWorldState->spawnRegions[regNum];
            if (spawnReg) {
                std::int32_t killed = 0;

                s->SysMessage(349); // Killing spawn, this may cause lag...
                std::vector<CChar *> spCharsToDelete;
                auto spCharList = spawnReg->GetSpawnedCharsList();
                for (auto &spChar : spCharList->collection()) {
                    if (ValidateObject(spChar)) {
                        if (spChar->IsSpawned()) {
                            // Store reference to character we want to delete
                            spCharsToDelete.push_back(spChar);
                        }
                    }
                }

                // Loop through the characters we want to delete from spawn region
                std::for_each(spCharsToDelete.begin(), spCharsToDelete.end(),
                              [&killed](CChar *charToDelete) {
                                  Effects->Bolteffect(charToDelete);
                                  Effects->PlaySound(charToDelete, 0x0029);
                                  charToDelete->Delete();
                                  ++killed;
                              });

                s->SysMessage(84);                  // Done.
                s->SysMessage(350, killed, regNum); // %i of Spawn %i have been killed.
            }
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	BuildWhoGump()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Build and send a gump with a list of players based on provided
// criteria
// o------------------------------------------------------------------------------------------------o
void BuildWhoGump(CSocket *s, std::uint8_t commandLevel, std::string title) {
    std::uint16_t j = 0;

    CGumpDisplay Who(s, 400, 300);
    Who.setTitle(title);
    {
        for (auto &iSock : Network->connClients) {
            CChar *iChar = iSock->CurrcharObj();
            if (iChar->GetCommandLevel() >= commandLevel) {
                auto temp = util::format("%i) %s", j, iChar->GetName().c_str());
                Who.AddData(temp, iChar->GetSerial(), 3);
            }
            ++j;
        }
    }
    Who.Send(4, false, INVALIDSERIAL);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_Who()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Displays a list of users currently online
// o------------------------------------------------------------------------------------------------o
void Command_Who(CSocket *s) {
    VALIDATESOCKET(s);
    BuildWhoGump(s, 0, "Who's Online");
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	void Command_GMs( CSocket *s )
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Displays a list of server staff currently online
// o------------------------------------------------------------------------------------------------o
void Command_GMs(CSocket *s) {
    VALIDATESOCKET(s);
    BuildWhoGump(s, CL_CNS, Dictionary->GetEntry(77, s->Language())); // Current Staff online
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_ReportBug()
//| Date		-	9th February, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Writes out a bug reported by player to the bug file
// o------------------------------------------------------------------------------------------------o
void Command_ReportBug(CSocket *s) {
    VALIDATESOCKET(s);
    CChar *mChar = s->CurrcharObj();
    if (!ValidateObject(mChar))
        return;

    auto logName = ServerConfig::shared().directoryFor(dirlocation_t::LOG) / std::filesystem::path("bugs.log");
    std::ofstream logDestination;
    logDestination.open(logName.string(), std::ios::out | std::ios::app);
    if (!logDestination.is_open()) {
        Console::shared().error(util::format("Unable to open bugs log file %s!", logName.string().c_str()));
        return;
    }
    char dateTime[1024];
    RealTime(dateTime);

    logDestination << "[" << dateTime << "] ";
    logDestination << "<" << mChar->GetName() << "> ";
    logDestination << "Reports: " << serverCommands.commandString(2) << std::endl;
    logDestination.close();

    s->SysMessage(87); // Thank you for your continuing support, your feedback is important to us
    bool x = false;
    {
        for (auto &iSock : Network->connClients) {
            CChar *iChar = iSock->CurrcharObj();
            if (ValidateObject(iChar)) {
                if (iChar->IsGMPageable()) {
                    x = true;
                    iSock->SysMessage(277, mChar->GetName().c_str(),serverCommands.commandString(2).c_str()); // User %s reported a bug (%s)
                }
            }
        }
    }
    if (x) {
        s->SysMessage(88); // Available Game Masters have been notified of your bug submission.
    }
    else {
        s->SysMessage(89); // There was no Game Master available to note your bug report.
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_ForceWho()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Brings up an interactive listing of online users.
// o------------------------------------------------------------------------------------------------o
void Command_ForceWho(CSocket *s) {
    VALIDATESOCKET(s);
    WhoList->ZeroWho();
    WhoList->SendSocket(s);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_ValidCmd()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Displays all valid commands for the user's character
// o------------------------------------------------------------------------------------------------o
void Command_ValidCmd(CSocket *s) {
    VALIDATESOCKET(s);
    CChar *mChar = s->CurrcharObj();
    std::uint8_t targetCommand = mChar->GetCommandLevel();
    CGumpDisplay targetCmds(s, 300, 300);
    targetCmds.setTitle("Valid serverCommands");

    for (auto myCounter = CCommands::commandMap.begin(); myCounter != CCommands::commandMap.end();
         ++myCounter) {
        if (myCounter->second.cmdLevelReq <= targetCommand) {
            targetCmds.AddData(myCounter->first, myCounter->second.cmdLevelReq);
        }
    }
    for (auto targCounter = CCommands::targetMap.begin(); targCounter != CCommands::targetMap.end();
         ++targCounter) {
        if (targCounter->second.cmdLevelReq <= targetCommand) {
            targetCmds.AddData(targCounter->first, targCounter->second.cmdLevelReq);
        }
    }
    for (auto jsCounter = CCommands::jscommandMap.begin(); jsCounter != CCommands::jscommandMap.end();
         ++jsCounter) {
        if (jsCounter->second.cmdLevelReq <= targetCommand) {
            targetCmds.AddData(jsCounter->first, jsCounter->second.cmdLevelReq);
        }
    }

    targetCmds.Send(4, false, INVALIDSERIAL);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_HowTo()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens a list of commands, with explanations of how to use each
// specific command
// o------------------------------------------------------------------------------------------------o
void Command_HowTo(CSocket *s) {
    VALIDATESOCKET(s);
    std::string commandStart = util::upper(serverCommands.commandString(2));
    if (commandStart.empty()) {
        CChar *mChar = s->CurrcharObj();
        if (!ValidateObject(mChar))
            return;

        std::int32_t iCmd = 2;
        std::int32_t numAdded = 0;
        std::uint8_t pagenum = 1;
        std::uint16_t position = 40;
        std::uint16_t linenum = 1;

        CPSendGumpMenu toSend;
        toSend.UserId(INVALIDSERIAL);
        toSend.GumpId(13);

        toSend.addCommand(
            util::format("resizepic 0 0 %u 480 320", cwmWorldState->ServerData()->BackgroundPic()));
        toSend.addCommand("page 0");
        toSend.addCommand("text 200 20 0 0");
        toSend.addText("HOWTO");
        toSend.addCommand(util::format("button 440 20 %u %i 1 0 1",
                                       cwmWorldState->ServerData()->ButtonCancel(),
                                       cwmWorldState->ServerData()->ButtonCancel() + 1));

        std::uint8_t currentLevel = mChar->GetCommandLevel();
        auto gAdd = CCommands::commandMap.begin();
        auto tAdd =  CCommands::targetMap.begin();
        auto jAdd = CCommands::jscommandMap.begin();

        toSend.addCommand("page 1");

        bool justDonePageAdd = false;
        while (gAdd !=CCommands::commandMap.end()) {
            if (numAdded > 0 && !(numAdded % 10) && !justDonePageAdd) {
                position = 40;
                ++pagenum;
                toSend.addCommand(util::format("page %u", pagenum));
                justDonePageAdd = true;
            }
            if (gAdd->second.cmdLevelReq <= currentLevel) {
                justDonePageAdd = false;
                toSend.addCommand(util::format("text 50 %u %u %u", position,
                                               cwmWorldState->ServerData()->LeftTextColour(),
                                               linenum));
                toSend.addCommand(util::format("button 20 %u %u %i %u 0 %i", position,
                                               cwmWorldState->ServerData()->ButtonRight(),
                                               cwmWorldState->ServerData()->ButtonRight() + 1,
                                               pagenum, iCmd));
                toSend.addText(gAdd->first);
                ++numAdded;
                ++linenum;
                position += 20;
            }
            ++iCmd;
            ++gAdd;
        }
        while (tAdd != CCommands::targetMap.end()) {
            if (numAdded > 0 && !(numAdded % 10) && !justDonePageAdd) {
                position = 40;
                ++pagenum;
                toSend.addCommand(util::format("page %u", pagenum));
                justDonePageAdd = true;
            }
            if (tAdd->second.cmdLevelReq <= currentLevel) {
                justDonePageAdd = false;
                toSend.addCommand(util::format("text 50 %u %u %u", position,
                                               cwmWorldState->ServerData()->LeftTextColour(),
                                               linenum));
                toSend.addCommand(util::format("button 20 %u %u %i %u 0 %i", position,
                                               cwmWorldState->ServerData()->ButtonRight(),
                                               cwmWorldState->ServerData()->ButtonRight() + 1,
                                               pagenum, iCmd));
                toSend.addText(tAdd->first);
                ++numAdded;
                ++linenum;
                position += 20;
            }
            ++iCmd;
            ++tAdd;
        }
        while (jAdd != CCommands::jscommandMap.end()) {
            if (numAdded > 0 && !(numAdded % 10) && !justDonePageAdd) {
                position = 40;
                ++pagenum;
                toSend.addCommand(util::format("page %u", pagenum));
                justDonePageAdd = true;
            }
            if (jAdd->second.cmdLevelReq <= currentLevel) {
                justDonePageAdd = false;
                toSend.addCommand(util::format("text 50 %u %u %u", position,
                                               cwmWorldState->ServerData()->LeftTextColour(),
                                               linenum));
                toSend.addCommand(util::format("button 20 %u %u %i %u 0 %i", position,
                                               cwmWorldState->ServerData()->ButtonRight(),
                                               cwmWorldState->ServerData()->ButtonRight() + 1,
                                               pagenum, iCmd));
                toSend.addText(jAdd->first);
                ++numAdded;
                ++linenum;
                position += 20;
            }
            ++iCmd;
            ++jAdd;
        }
        pagenum = 1;
        for (std::int32_t i = 0; i < numAdded; i += 10) {
            toSend.addCommand(util::format("page %u", pagenum));
            if (i >= 10) {
                toSend.addCommand(util::format(
                    "button 30 290 %u %i 0 %i", cwmWorldState->ServerData()->ButtonLeft(),
                    cwmWorldState->ServerData()->ButtonLeft() + 1, pagenum - 1)); // back button
            }
            if ((numAdded > 10) && ((i + 10) < numAdded)) {
                toSend.addCommand(util::format(
                    "button 440 290 %u %i 0 %i", cwmWorldState->ServerData()->ButtonRight(),
                    cwmWorldState->ServerData()->ButtonRight() + 1, pagenum + 1));
            }
            ++pagenum;
        }
        toSend.Finalize();
        s->Send(&toSend);
    }
    else {
        std::int32_t i = 0;
        auto toFind = CCommands::commandMap.begin();
        for (toFind = CCommands::commandMap.begin(); toFind != CCommands::commandMap.end(); ++toFind) {
            if (commandStart == toFind->first) {
                break;
            }
            ++i;
        }
        if (toFind == CCommands::commandMap.end()) {
            auto findTarg = CCommands::targetMap.begin();
            for (findTarg = CCommands::targetMap.begin(); findTarg != CCommands::targetMap.end(); ++findTarg) {
                if (commandStart == findTarg->first) {
                    break;
                }
                ++i;
            }
            if (findTarg == CCommands::targetMap.end()) {
                auto findJS = CCommands::jscommandMap.begin();
                for (findJS = CCommands::jscommandMap.begin(); findJS != CCommands::jscommandMap.end(); ++findJS) {
                    if (commandStart == findJS->first) {
                        break;
                    }
                    ++i;
                }
                if (findJS == CCommands::jscommandMap.end()) {
                    s->SysMessage(280); // Error finding command
                    return;
                }
            }
        }
        HandleHowTo(s, i);
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_Temp()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Displays the current temperature in user's current townregion
// o------------------------------------------------------------------------------------------------o
void Command_Temp(CSocket *s) {
    VALIDATESOCKET(s);
    CChar *mChar = s->CurrcharObj();
    if (!ValidateObject(mChar))
        return;

    CTownRegion *reg = mChar->GetRegion();
    auto toGrab = reg->GetWeather();
    if (toGrab != 0xFF) {
        R32 curTemp = Weather->Temp(toGrab);
        s->SysMessage(1751, curTemp); // It is currently %f degrees
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_Status()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens the HTML status information gump
// o------------------------------------------------------------------------------------------------o
void Command_Status(CSocket *s) {
    VALIDATESOCKET(s);
    HTMLTemplates->TemplateInfoGump(s);
}

