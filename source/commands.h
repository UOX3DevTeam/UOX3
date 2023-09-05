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
    
    CommandLevel() : name(""), title(""), commandLevel(0), defaultPriv(0), nickColour(0), allSkillVals(0), targBody(0), bodyColour(0) {
        stripOff.reset();
    }
};

class CCommands {
public:
    static std::map<std::string, CommandMapEntry> commandMap ;
    static std::map<std::string, TargetMapEntry> targetMap;
    static std::map<std::string, JSCommandEntry> jscommandMap ;
private:
    std::vector<std::unique_ptr<CommandLevel>> clearance;
    std::map<std::string, CommandMapEntry>::iterator cmdPointer;
    std::map<std::string, TargetMapEntry>::iterator targPointer;
    std::string cmdString;
    
    void initClearance();
    void resetCommand();
    
public:
    std::uint8_t numArguments();
    std::int32_t argument(std::uint8_t argNum);
    std::string commandString(std::uint8_t section, std::uint8_t end = 0);
    void commandString(std::string newValue);
    
    CommandLevel *getClearance(std::string clearName); // return by command name
    CommandLevel *getClearance(std::uint8_t commandLevel);     // return by command level
    std::uint16_t getColourByLevel(std::uint8_t commandLevel);
    void command(CSocket *s, CChar *c, std::string text, bool checkSocketAccess = false);
    void load();
    void log(const std::string &command, CChar *player1, CChar *player2,
             const std::string &extraInfo);
    
    bool commandExists(const std::string &cmdName);
    const std::string firstCommand();
    const std::string nextCommand();
    bool finishedCommandList();
    
    CommandMapEntry *commandDetails(const std::string &cmdName);
    
    CCommands() = default;
    auto startup() -> void;
    ~CCommands() = default;
    
    void registerCommand(const std::string &cmdName, std::uint16_t scriptId, std::uint8_t cmdLevel, bool isEnabled);
    void unRegisterCommand(const std::string &cmdName, cScript *toRegister);
    void setCommandStatus(const std::string &cmdName, bool isEnabled);
};

extern CCommands serverCommands;

#endif
