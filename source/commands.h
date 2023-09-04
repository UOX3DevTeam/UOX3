#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include <bitset>
#include <cstdint>
#include <string>
#include <vector>

#include "cmdtable.h"
#include "typedefs.h"

class CChar;
class cScript;

const std::uint32_t BIT_STRIPHAIR = 1;
const std::uint32_t BIT_STRIPITEMS = 2;

struct CommandLevel_st {
    std::string name;        // name of level
    std::string title;       // Title of level, displayed in front of name
    std::uint8_t commandLevel;       // upper limit of level
    std::uint16_t defaultPriv;        // default privs associated with it
    std::uint16_t nickColour;         // colour of a person's name
    std::uint16_t allSkillVals;       // if 0, skills left same, if not, all skills set to this value
    std::uint16_t targBody;           // target body value
    std::uint16_t bodyColour;         // target body colour
    std::bitset<8> stripOff; // strips off hair, beard and clothes
    CommandLevel_st()
    : name(""), title(""), commandLevel(0), defaultPriv(0), nickColour(0), allSkillVals(0),
    targBody(0), bodyColour(0) {
        stripOff.reset();
    }
};

class CCommands {
private:
    std::vector<CommandLevel_st *> clearance;
    COMMANDMAP_ITERATOR cmdPointer;
    TARGETMAP_ITERATOR targPointer;
    std::string commandString;
    
    void InitClearance();
    void CommandReset();
    
public:
    std::uint8_t NumArguments();
    std::int32_t Argument(std::uint8_t argNum);
    std::string CommandString(std::uint8_t section, std::uint8_t end = 0);
    void CommandString(std::string newValue);
    
    CommandLevel_st *GetClearance(std::string clearName); // return by command name
    CommandLevel_st *GetClearance(std::uint8_t commandLevel);     // return by command level
    std::uint16_t GetColourByLevel(std::uint8_t commandLevel);
    void Command(CSocket *s, CChar *c, std::string text, bool checkSocketAccess = false);
    void Load();
    void Log(const std::string &command, CChar *player1, CChar *player2,
             const std::string &extraInfo);
    
    bool CommandExists(const std::string &cmdName);
    const std::string FirstCommand();
    const std::string NextCommand();
    bool FinishedCommandList();
    
    CommandMapEntry_st *CommandDetails(const std::string &cmdName);
    
    CCommands() = default;
    auto Startup() -> void;
    ~CCommands();
    
    void Register(const std::string &cmdName, std::uint16_t scriptId, std::uint8_t cmdLevel, bool isEnabled);
    void UnRegister(const std::string &cmdName, cScript *toRegister);
    void SetCommandStatus(const std::string &cmdName, bool isEnabled);
};

extern CCommands *Commands;

#endif
