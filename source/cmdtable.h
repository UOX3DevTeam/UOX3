//	 cmdtable.h - Crackerjack <crackerjack@crackerjack.net> July 24/99
//	This code is an attempt to clean up the messy "if/then/else" routines
//	currently in use for GM commands, as well as adding more functionality
//	and more potential for functionality.
//
//	Current features:
//	- Actual table of commands to execute, what perms are required, dialog
//	  messages for target commands, etc handled by a central system
//
//

#ifndef __CMDTABLE_H
#define __CMDTABLE_H

#include <cstdint>
#include <map>
#include <string>

#include "typedefs.h"

// Types of commands
enum commandTypes {
    CMD_TARGET = 0, // Call target struct specified in cmd_extra
    CMD_FUNC,       // Call function specified in cmd_extra must be of type GMFUNC
    CMD_SOCKFUNC,   // Call function specified in cmd_extra with a socket
    CMD_TARGETXYZ,  // target with addx & y & z [] arguments
    CMD_TARGETINT,  // target with tempint
    CMD_TARGETTXT,  // target with XText
};

struct CommandMapEntry {
    ~CommandMapEntry() = default;
    std::uint8_t cmdLevelReq;
    std::uint8_t cmdType;
    void (*cmd_extra)(); // executable function
    CommandMapEntry() : cmdLevelReq(0), cmdType(CMD_SOCKFUNC), cmd_extra(nullptr) {}
    CommandMapEntry(std::uint8_t cLR, std::uint8_t cT, void (*ce)()) : cmdLevelReq(cLR), cmdType(cT), cmd_extra(ce) {}
};

struct TargetMapEntry {
    std::uint8_t cmdLevelReq;
    std::uint8_t cmdType;
    targetids_t targId;
    std::int32_t dictEntry;
    TargetMapEntry() : cmdLevelReq(0), cmdType(CMD_TARGET), targId(TARGET_NOFUNC), dictEntry(0) {}
    TargetMapEntry(std::uint8_t cLR, std::uint8_t cT, targetids_t tID, std::int32_t dE) : cmdLevelReq(cLR), cmdType(cT), targId(tID), dictEntry(dE) {}
};

struct JSCommandEntry {
    std::uint8_t cmdLevelReq;
    std::uint16_t scriptId;
    bool isEnabled;
    JSCommandEntry() : cmdLevelReq(0), scriptId(0), isEnabled(true) {}
    JSCommandEntry(std::uint8_t cLR, std::uint16_t id, bool iE) : cmdLevelReq(cLR), scriptId(id), isEnabled(iE) {}
};


auto command_fixspawn() ->void ;
auto Command_AddAccount(CSocket *s) ->void ;
auto Command_GetLight(CSocket *s) ->void ;
auto Command_SetPost(CSocket *s) ->void;
auto Command_GetPost(CSocket *s) ->void ;
auto Command_ShowIds(CSocket *s) -> void;
auto Command_Tile(CSocket *s) ->void ;
auto Command_Save() ->void ;
auto Command_Dye(CSocket *s) ->void ;
auto Command_SetTime() ->void ;
auto Command_Shutdown() ->void ;
auto Command_Tell(CSocket *s) ->void ;
auto Command_GmMenu(CSocket *s) ->void ;
auto Command_Command(CSocket *s) ->void ;
auto Command_MemStats(CSocket *s) ->void ;
auto Command_Restock(CSocket *s) ->void ;
auto Command_SetShopRestockRate(CSocket *s) ->void ;
auto Command_Respawn() ->void ;
auto Command_RegSpawn(CSocket *s) ->void ;
auto Command_LoadDefaults() ->void ;
auto Command_CQ(CSocket *s) ->void ;
auto Command_GQ(CSocket *s) ->void ;
auto Command_MineCheck() ->void ;
auto Command_Guards() ->void ;
auto Command_Announce() ->void ;
auto Command_PDump(CSocket *s) ->void ;
auto Command_SpawnKill(CSocket *s) -> void;
auto BuildWhoGump(CSocket *s, std::uint8_t commandLevel, std::string title) ->void ;
auto Command_Who(CSocket *s) ->void ;
auto Command_GMs(CSocket *s) ->void ;
auto Command_ReportBug(CSocket *s) ->void ;
auto Command_ForceWho(CSocket *s) ->void ;
auto Command_ValidCmd(CSocket *s) ->void ;
auto Command_HowTo(CSocket *s) ->void ;
auto Command_Temp(CSocket *s) ->void ;
auto Command_Status(CSocket *s) ->void;




#define CMD_EXEC void (*)()
#define CMD_SOCKEXEC void (*)(CSocket *)
#define CMD_DEFINE void (*)()



#endif // __CMDTABLE_H
