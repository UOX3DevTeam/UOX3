#include "commands.h"

#include <fstream>

#include "cchar.h"
#include "cjsmapping.h"
#include "cpacketsend.h"
#include "cscript.h"
#include "cserverdefinitions.h"
#include "csocket.h"

#include "subsystem/console.hpp"

#include "funcdecl.h"
#include "ssection.h"
#include "stringutility.hpp"
#include "utility/strutil.hpp"


//===================================================================================================
// CCommands
//==================================================================================================
//==================================================================================================
// Define our static maps
auto CCommands::commandMap = std::map<std::string, CommandMapEntry>();
auto CCommands::targetMap = std::map<std::string, TargetMapEntry>();
auto CCommands::jscommandMap = std::map<std::string, JSCommandEntry>();

//==================================================================================================
auto CCommands::Startup() -> void { resetCommand(); }

//==================================================================================================
CCommands::~CCommands() {
    targetMap.clear();
    jscommandMap.clear();
    clearance.clear();
    commandMap.clear();
 
}
// o------------------------------------------------------------------------------------------------o
//|	Function	-	CCommands::numArguments()
//|	Date		-	3/12/2003
//|	Changes		-	4/2/2003 - Reduced to a std::uint8_t
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Number of arguments in a command
// o------------------------------------------------------------------------------------------------o
std::uint8_t CCommands::numArguments() {
    auto secs = oldstrutil::sections(cmdString, " ");
    return static_cast<std::uint8_t>(secs.size());
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CCommands::argument()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Grabs argument argNum and converts it to an integer
// o------------------------------------------------------------------------------------------------o
std::int32_t CCommands::argument(std::uint8_t argNum) {
    std::int32_t retVal = 0;
    std::string tempString = commandString(argNum + 1, argNum + 1);
    if (!tempString.empty()) {
        try {
            retVal = std::stoi(tempString, nullptr, 0);
        } catch (const std::invalid_argument &e) {
            Console::shared().Error(util::format("[%s] Unable to convert command argument ('%s') to integer.", e.what(), tempString.c_str()));
        }
    }
    
    return retVal;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CCommands::commandString()
//|	Date		-	4/02/2003
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the Comm value
// o------------------------------------------------------------------------------------------------o
std::string CCommands::commandString(std::uint8_t section, std::uint8_t end) {
    std::string retString;
    if (end != 0) {
        retString = oldstrutil::extractSection(cmdString, " ", section - 1, end - 1);
    }
    else {
        retString = oldstrutil::extractSection(cmdString, " ", section - 1);
    }
    return retString;
}
void CCommands::commandString(std::string newValue) { cmdString = newValue; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CCommands::command()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles commands sent from client
// o------------------------------------------------------------------------------------------------o
//| Changes		-	25 June, 2003
//|						Made it accept a CPITalkRequest, allowing to remove
//|						the need for Offset and unicode decoding
// o------------------------------------------------------------------------------------------------o
void CCommands::command(CSocket *s, CChar *mChar, std::string text, bool checkSocketAccess) {
    commandString(util::simplify(text));
    if (numArguments() < 1)
        return;
    
    // Discard the leading command prefix
    std::string command = util::upper(commandString(1, 1));
    
    auto toFind = jscommandMap.find(command);
    if (toFind != jscommandMap.end()) {
        if (toFind->second.isEnabled) {
            bool plClearance = false;
            if (checkSocketAccess) {
                plClearance = (s->CurrcharObj()->GetCommandLevel() >= toFind->second.cmdLevelReq ||
                               s->CurrcharObj()->GetAccount().accountNumber == 0);
            }
            else {
                plClearance = (mChar->GetCommandLevel() >= toFind->second.cmdLevelReq ||
                               mChar->GetAccount().accountNumber == 0);
            }
            // from now on, account 0 ALWAYS has admin access, regardless of command level
            if (!plClearance) {
                if (checkSocketAccess) {
                    Log(command, s->CurrcharObj(), nullptr, "Insufficient clearance");
                }
                else {
                    Log(command, mChar, nullptr, "Insufficient clearance");
                }
                s->SysMessage(337); // Access denied.
                return;
            }
            cScript *toExecute = JSMapping->GetScript(toFind->second.scriptId);
            if (toExecute != nullptr) { // All commands that execute are of the form:
                // command_commandname (to avoid possible clashes)
#if defined(UOX_DEBUG_MODE)
                Console::shared().Print(util::format("Executing JS command %s\n", command.c_str()));
#endif
                toExecute->executeCommand(s, "command_" + command, commandString(2));
            }
            if (checkSocketAccess) {
                Log(command, s->CurrcharObj(), nullptr, "Cleared");
            }
            else {
                Log(command, mChar, nullptr, "Cleared");
            }
            return;
        }
    }
    
    auto findTarg = targetMap.find(command);
    if (findTarg != targetMap.end()) {
        bool plClearance = false;
        if (checkSocketAccess) {
            plClearance = (s->CurrcharObj()->GetCommandLevel() >= findTarg->second.cmdLevelReq ||
                           s->CurrcharObj()->GetAccount().accountNumber == 0);
        }
        else {
            plClearance = (mChar->GetCommandLevel() >= findTarg->second.cmdLevelReq ||
                           mChar->GetAccount().accountNumber == 0);
        }
        if (!plClearance) {
            if (checkSocketAccess) {
                Log(command, s->CurrcharObj(), nullptr, "Insufficient clearance");
            }
            else {
                Log(command, mChar, nullptr, "Insufficient clearance");
            }
            s->SysMessage(337); // Access denied.
            return;
        }
        if (checkSocketAccess) {
            Log(command, s->CurrcharObj(), nullptr, "Cleared");
        }
        else {
            Log(command, mChar, nullptr, "Cleared");
        }
        switch (findTarg->second.cmdType) {
            case CMD_TARGET:
                s->SendTargetCursor(0, findTarg->second.targId, 0, findTarg->second.dictEntry);
                break;
            case CMD_TARGETXYZ:
                if (numArguments() == 4) {
                    s->ClickX(static_cast<std::int16_t>(argument(1)));
                    s->ClickY(static_cast<std::int16_t>(argument(2)));
                    s->ClickZ(static_cast<std::int8_t>(argument(3)));
                    s->SendTargetCursor(0, findTarg->second.targId, 0, findTarg->second.dictEntry);
                }
                else {
                    s->SysMessage(340); // This command takes three numbers as arguments.
                }
                break;
            case CMD_TARGETINT:
                if (numArguments() == 2) {
                    s->TempInt(argument(1));
                    s->SendTargetCursor(0, findTarg->second.targId, 0, findTarg->second.dictEntry);
                }
                else {
                    s->SysMessage(338); // This command takes one number as an argument.
                }
                break;
            case CMD_TARGETTXT:
                if (numArguments() > 1) {
                    s->XText(commandString(2));
                    s->SendTargetCursor(0, findTarg->second.targId, 0, findTarg->second.dictEntry);
                }
                else {
                    s->SysMessage(9026); // This command requires more arguments!
                }
                break;
        }
    }
    else {
        auto toFind = commandMap.find(command);
        if (toFind == commandMap.end()) {
            bool cmdHandled = false;
            std::vector<std::uint16_t> scriptTriggers = mChar->GetScriptTriggers();
            for (auto scriptTrig : scriptTriggers) {
                cScript *toExecute = JSMapping->GetScript(scriptTrig);
                if (toExecute != nullptr) {
                    // -1 == event doesn't exist, or returned -1
                    // 0 == script returned false, 0, or nothing
                    // 1 == script returned true or 1, don't execute hard code
                    if (toExecute->OnCommand(s, command) == 1) {
                        cmdHandled = true;
                    }
                }
            }
            
            if (!cmdHandled) {
                s->SysMessage(336); // Unrecognized command.
            }
            return;
        }
        else {
            bool plClearance = false;
            if (checkSocketAccess) {
                plClearance = (s->CurrcharObj()->GetCommandLevel() >= toFind->second.cmdLevelReq ||
                               s->CurrcharObj()->GetAccount().accountNumber == 0);
            }
            else {
                plClearance = (mChar->GetCommandLevel() >= toFind->second.cmdLevelReq ||
                               mChar->GetAccount().accountNumber == 0);
            }
            // from now on, account 0 ALWAYS has admin access, regardless of command level
            if (!plClearance) {
                if (checkSocketAccess) {
                    Log(command, s->CurrcharObj(), nullptr, "Insufficient clearance");
                }
                else {
                    Log(command, mChar, nullptr, "Insufficient clearance");
                }
                s->SysMessage(337); // Access denied.
                return;
            }
            if (checkSocketAccess) {
                Log(command, s->CurrcharObj(), nullptr, "Cleared");
            }
            else {
                Log(command, mChar, nullptr, "Cleared");
            }
            
            switch (toFind->second.cmdType) {
                case CMD_FUNC:
                    (*((CMD_EXEC)toFind->second.cmd_extra))();
                    break;
                case CMD_SOCKFUNC:
                    (*((CMD_SOCKEXEC)toFind->second.cmd_extra))(s);
                    break;
                default:
                    s->SysMessage(346); // BUG: Command has a bad command type set!
                    break;
            }
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CCommands::Load()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Load the command table
// o------------------------------------------------------------------------------------------------o
void CCommands::Load() {
    std::int16_t commandCount = 0;
    CScriptSection *commands = FileLookup->FindEntry("COMMAND_OVERRIDE", command_def);
    if (commands == nullptr) {
        initClearance();
        return ;
    }
    
    std::string tag;
    std::string data;
    std::string UTag;
    
    std::vector<std::string> badCommands;
    for (tag = commands->First(); !commands->AtEnd(); tag = commands->Next()) {
        data = commands->GrabData();
        auto toFind = commandMap.find(tag);
        auto findTarg = targetMap.find(tag);
        if (toFind == commandMap.end() && findTarg == targetMap.end()) {
            badCommands.push_back(tag); // make sure we don't index into array at -1
        }
        else {
            ++commandCount;
            if (toFind != commandMap.end()) {
                toFind->second.cmdLevelReq = static_cast<std::uint8_t>(std::stoul(data, nullptr, 0));
            }
            else if (findTarg != targetMap.end()) {
                findTarg->second.cmdLevelReq = static_cast<std::uint8_t>(std::stoul(data, nullptr, 0));
            }
        }
        // check for commands here
    }
    if (!badCommands.empty()) {
        Console::shared() << myendl;
        std::for_each(badCommands.begin(), badCommands.end(), [](const std::string &entry) {
            Console::shared() << "Invalid command '" << entry.c_str() << "' found in commands.dfn!" << myendl;
        });
    }
    
    Console::shared() << "   o Loading command levels";
#if defined(UOX_DEBUG_MODE)
    Console::shared() << myendl;
#endif
    CScriptSection *cmdClearance = FileLookup->FindEntry("COMMANDLEVELS", command_def);
    if (cmdClearance == nullptr) {
        initClearance();
    }
    else {
        size_t currentWorking;
        for (const auto &sec : cmdClearance->collection()) {
            tag = sec->tag;
            data = sec->data;
            currentWorking = clearance.size();
            clearance.push_back(std::make_unique<CommandLevel>());
            clearance[currentWorking]->name = tag;
            clearance[currentWorking]->commandLevel = static_cast<std::uint8_t>(std::stoul(data, nullptr, 0));
        }
        
        for (auto cIter = clearance.begin(); cIter != clearance.end(); ++cIter) {
            CommandLevel *ourClear = (*cIter).get();
            if (ourClear) {
                cmdClearance = FileLookup->FindEntry(ourClear->name, command_def);
                if (cmdClearance) {
                    for (const auto &sec : cmdClearance->collection()) {
                        tag = sec->tag;
                        data = sec->data;
                        UTag = util::upper(tag);
                        if (UTag == "NICKCOLOUR") {
                            ourClear->nickColour = static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
                        }
                        else if (UTag == "TITLE") {
                            ourClear->title = data;
                        }
                        else if (UTag == "DEFAULTPRIV") {
                            ourClear->defaultPriv = static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
                        }
                        else if (UTag == "BODYID") {
                            ourClear->targBody = static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
                        }
                        else if (UTag == "ALLSKILL") {
                            ourClear->allSkillVals =
                            static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
                        }
                        else if (UTag == "BODYCOLOUR") {
                            ourClear->bodyColour = static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
                        }
                        else if (UTag == "STRIPHAIR") {
                            ourClear->stripOff.set(CommandLevel::BIT_STRIPHAIR, true);
                        }
                        else if (UTag == "STRIPITEMS") {
                            ourClear->stripOff.set(CommandLevel::BIT_STRIPITEMS, true);
                        }
                        else {
                            Console::shared() << myendl << "Unknown tag in " << ourClear->name
                            << ": " << tag << " with data of " << data << myendl;
                        }
                    }
                }
            }
        }
    }
    
    // Now we'll load our JS commands, what fun!
    CJSMappingSection *commandSection = JSMapping->GetSection(CJSMappingSection::SCPT_COMMAND);
    for (cScript *ourScript = commandSection->First(); !commandSection->Finished();
         ourScript = commandSection->Next()) {
        if (ourScript != nullptr) {
            ourScript->ScriptRegistration("Command");
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CCommands::Log()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Writes toLog to a file
// o------------------------------------------------------------------------------------------------o
void CCommands::Log(const std::string &command, CChar *player1, CChar *player2,
                    const std::string &extraInfo) {
    std::string logName = cwmWorldState->ServerData()->Directory(CSDDP_LOGS) + "command.log";
    std::ofstream logDestination;
    logDestination.open(logName.c_str(), std::ios::out | std::ios::app);
    if (!logDestination.is_open()) {
        Console::shared().Error(
                                util::format("Unable to open command log file %s!", logName.c_str()));
        return;
    }
    char dateTime[1024];
    RealTime(dateTime);
    
    logDestination << "[" << dateTime << "] ";
    logDestination << player1->GetName() << " (serial: " << std::hex << player1->GetSerial()
    << ") ";
    logDestination << "used command <" << command << (commandString(2) != "" ? " " : "")
    << commandString(2) << "> ";
    if (ValidateObject(player2)) {
        logDestination << "on player " << player2->GetName() << " (serial: " << player2->GetSerial()
        << " )";
    }
    logDestination << "Extra Info: " << extraInfo << std::endl;
    logDestination.close();
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	*CCommands::getClearance()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get the command level of a character
// o------------------------------------------------------------------------------------------------o
CommandLevel *CCommands::getClearance(std::string clearName) {
    if (clearance.empty()) {
        return nullptr;
    }
    CommandLevel *clearPointer;
    
    for (auto clearIter = clearance.begin(); clearIter != clearance.end(); ++clearIter) {
        clearPointer = (*clearIter).get();
        if (util::upper(clearName) == clearPointer->name) {
            return clearPointer;
        }
    }
    return nullptr;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CCommands::getColourByLevel()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get a players nick color based on his command level
// o------------------------------------------------------------------------------------------------o
std::uint16_t CCommands::getColourByLevel(std::uint8_t commandLevel) {
    size_t clearanceSize = clearance.size();
    if (clearanceSize == 0)
        return 0x005A;
    
    if (commandLevel > clearance[0]->commandLevel)
        return clearance[0]->nickColour;
    
    for (size_t counter = 0; counter < (clearanceSize - 1); ++counter) {
        if (commandLevel <= clearance[counter]->commandLevel &&
            commandLevel > clearance[counter + 1]->commandLevel)
            return clearance[counter]->nickColour;
    }
    return clearance[clearanceSize - 1]->nickColour;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CCommands::initClearance()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Initialize command levels
// o------------------------------------------------------------------------------------------------o
void CCommands::initClearance() {
    clearance.push_back(std::make_unique<CommandLevel>()); // 0 -> 3
    clearance.push_back(std::make_unique<CommandLevel>());
    clearance.push_back(std::make_unique<CommandLevel>());
    clearance.push_back(std::make_unique<CommandLevel>());
    
    clearance[0]->name = "ADMIN";
    clearance[1]->name = "GM";
    clearance[2]->name = "COUNSELOR";
    clearance[3]->name = "PLAYER";
    
    clearance[0]->title = "Admin";
    clearance[1]->title = "GM";
    clearance[2]->title = "Counselor";
    clearance[3]->title = "";
    
    clearance[0]->commandLevel = 5;
    clearance[1]->commandLevel = 2;
    clearance[2]->commandLevel = 1;
    clearance[3]->commandLevel = 0;
    
    clearance[0]->targBody = 0x03DB;
    clearance[1]->targBody = 0x03DB;
    clearance[2]->targBody = 0x03DB;
    clearance[3]->targBody = 0;
    
    clearance[0]->bodyColour = 0x8001;
    clearance[1]->bodyColour = 0x8021;
    clearance[2]->bodyColour = 0x8002;
    
    clearance[0]->defaultPriv = 0x786F;
    clearance[1]->defaultPriv = 0x786F;
    clearance[2]->defaultPriv = 0x0094;
    clearance[3]->defaultPriv = 0;
    
    clearance[0]->nickColour = 0x1332;
    clearance[1]->nickColour = 0x03;
    clearance[2]->nickColour = 0x03;
    clearance[3]->nickColour = 0x005A;
    
    clearance[0]->allSkillVals = 1000;
    clearance[1]->allSkillVals = 1000;
    clearance[2]->allSkillVals = 0;
    clearance[3]->allSkillVals = 0;
    
    // Strip Everything for Admins, GMs and Counselors
    clearance[0]->stripOff.set(CommandLevel::BIT_STRIPHAIR, true);
    clearance[0]->stripOff.set(CommandLevel::BIT_STRIPITEMS, true);
    clearance[1]->stripOff.set(CommandLevel::BIT_STRIPHAIR, true);
    clearance[1]->stripOff.set(CommandLevel::BIT_STRIPITEMS, true);
    clearance[2]->stripOff.set(CommandLevel::BIT_STRIPHAIR, true);
    clearance[2]->stripOff.set(CommandLevel::BIT_STRIPITEMS, true);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CCommands::getClearance()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Geta characters command level
// o------------------------------------------------------------------------------------------------o
CommandLevel *CCommands::getClearance(std::uint8_t commandLevel) {
    size_t clearanceSize = clearance.size();
    if (clearanceSize == 0)
        return nullptr;
    
    if (commandLevel > clearance[0]->commandLevel)
        return clearance[0].get();
    
    for (size_t counter = 0; counter < (clearanceSize - 1); ++counter) {
        if (commandLevel <= clearance[counter]->commandLevel &&
            commandLevel > clearance[counter + 1]->commandLevel)
            return clearance[counter].get();
    }
    return clearance[clearanceSize - 1].get();
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CCommands::commandExists()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if a command is valid
// o------------------------------------------------------------------------------------------------o
bool CCommands::commandExists(const std::string &cmdName) {
    auto toFind = commandMap.find(cmdName);
    return (toFind != commandMap.end());
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CCommands::FirstCommand()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get first command in cmd_table
// o------------------------------------------------------------------------------------------------o
const std::string CCommands::FirstCommand() {
    cmdPointer = commandMap.begin();
    if (FinishedCommandList())
        return "";
    
    return cmdPointer->first;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CCommands::NextCommand()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get next command in cmd_table
// o------------------------------------------------------------------------------------------------o
const std::string CCommands::NextCommand() {
    if (FinishedCommandList())
        return "";
    
    ++cmdPointer;
    if (FinishedCommandList())
        return "";
    
    return cmdPointer->first;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CCommands::FinishedCommandList()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get end of cmd_table
// o------------------------------------------------------------------------------------------------o
bool CCommands::FinishedCommandList() { return (cmdPointer == commandMap.end()); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CCommands::CommandDetails()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get command info
// o------------------------------------------------------------------------------------------------o
CommandMapEntry *CCommands::CommandDetails(const std::string &cmdName) {
    if (!commandExists(cmdName))
        return nullptr;
    
    auto toFind = commandMap.find(cmdName);
    if (toFind == commandMap.end())
        return nullptr;
    
    return &(toFind->second);
}
