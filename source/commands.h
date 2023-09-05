#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include <bitset>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "cmdtable.h"
#include "typedefs.h"

class CChar;
class cScript;


struct CommandLevel {
    static constexpr auto BIT_STRIPHAIR = std::uint32_t(1);
    static constexpr auto BIT_STRIPITEMS = std::uint32_t(2);

    std::string name;        // name of level
    std::string title;       // Title of level, displayed in front of name
    std::uint8_t commandLevel;       // upper limit of level
    std::uint16_t defaultPriv;        // default privs associated with it
    std::uint16_t nickColour;         // colour of a person's name
    std::uint16_t allSkillVals;       // if 0, skills left same, if not, all skills set to this value
    std::uint16_t targBody;           // target body value
    std::uint16_t bodyColour;         // target body colour
    std::bitset<8> stripOff; // strips off hair, beard and clothes
    CommandLevel()
    : name(""), title(""), commandLevel(0), defaultPriv(0), nickColour(0), allSkillVals(0),
    targBody(0), bodyColour(0) {
        stripOff.reset();
    }
};

class CCommands {
public:
    static std::map<std::string, CommandMapEntry> CommandMap ;
    static std::map<std::string, TargetMapEntry> TargetMap;
    static std::map<std::string, JSCommandEntry> JSCommandMap ;
private:
    std::vector<std::unique_ptr<CommandLevel>> clearance;
    std::map<std::string, CommandMapEntry>::iterator cmdPointer;
    std::map<std::string, TargetMapEntry>::iterator targPointer;
    std::string commandString;
    
    void InitClearance();
    void CommandReset();
    
public:
    std::uint8_t NumArguments();
    std::int32_t Argument(std::uint8_t argNum);
    std::string CommandString(std::uint8_t section, std::uint8_t end = 0);
    void CommandString(std::string newValue);
    
    CommandLevel *GetClearance(std::string clearName); // return by command name
    CommandLevel *GetClearance(std::uint8_t commandLevel);     // return by command level
    std::uint16_t GetColourByLevel(std::uint8_t commandLevel);
    void Command(CSocket *s, CChar *c, std::string text, bool checkSocketAccess = false);
    void Load();
    void Log(const std::string &command, CChar *player1, CChar *player2,
             const std::string &extraInfo);
    
    bool CommandExists(const std::string &cmdName);
    const std::string FirstCommand();
    const std::string NextCommand();
    bool FinishedCommandList();
    
    CommandMapEntry *CommandDetails(const std::string &cmdName);
    
    CCommands() = default;
    auto Startup() -> void;
    ~CCommands();
    
    void Register(const std::string &cmdName, std::uint16_t scriptId, std::uint8_t cmdLevel, bool isEnabled);
    void UnRegister(const std::string &cmdName, cScript *toRegister);
    void SetCommandStatus(const std::string &cmdName, bool isEnabled);
};

extern CCommands *Commands;

#endif
