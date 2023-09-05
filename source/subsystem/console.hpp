#ifndef console_hpp
#define console_hpp

// In essence, Console is our nice generic class for outputting display
// As long as the interface is kept the same, we can display whatever we wish to
// We store a set of coordinates being the window corner and size, for systems with windowing
// support, which describes the window we are in
#include <bitset>
#include <iostream>
#include <map>
#include <string>
#include <utility>

#include "typedefs.h"

class CBaseObject;
class CEndL;
// o------------------------------------------------------------------------------------------------o
constexpr auto MAX_CONSOLE_BUFF = 512;

const std::uint8_t CNORMAL = 0;
const std::uint8_t CBLUE = 1;
const std::uint8_t CRED = 2;
const std::uint8_t CGREEN = 3;
const std::uint8_t CYELLOW = 4;
const std::uint8_t CBWHITE = 5;

// o------------------------------------------------------------------------------------------------o
//  Console
// o------------------------------------------------------------------------------------------------o
class Console {
    Console();
    
public:
    Console(const Console &) = delete;
    Console(Console &&) = delete;
    auto operator=(const Console &) -> Console & = delete;
    auto operator=(Console &&) -> Console & = delete;
    static auto shared() -> Console &;
    
    ~Console();
    auto Initialize() -> void;
    
    auto Flush() -> void { std::cout.flush(); }
    // streaming overloads, we take pointers and references
    
    auto operator<<(const char *output) -> Console &;
    auto operator<<(const std::int8_t *output) -> Console &;
    auto operator<<(const std::uint8_t *output) -> Console &;
    
    auto operator<<(const std::uint8_t &output) -> Console &;
    auto operator<<(const std::int8_t &output) -> Console &;
    auto operator<<(const std::uint16_t &output) -> Console &;
    auto operator<<(const std::int16_t &output) -> Console &;
    auto operator<<(const std::uint32_t &output) -> Console &;
    auto operator<<(const std::int32_t &output) -> Console &;
    auto operator<<(const std::uint64_t &output) -> Console &;
    auto operator<<(const std::int64_t &output) -> Console &;
    auto operator<<(const float &output) -> Console &;
    auto operator<<(const double &output) -> Console &;
    auto operator<<(const long double &output) -> Console &;
    
    auto operator<<(CBaseObject *output) -> Console &;
    auto operator<<(const CBaseObject *output) -> Console &;
    auto operator<<(const CBaseObject &output) -> Console &;
    
    auto operator<<(const std::string &outPut) -> Console &;
    
    auto operator<<(CEndL &myObj) -> Console &;
    
    auto print(const std::string &toPrint) -> void;
    auto log(const std::string &msg, const std::string &filename) -> void;
    auto log(const std::string &msg) -> void;
    auto error(const std::string &msg) -> void;
    auto warning(const std::string &msg) -> void;
    
    auto currentMode() const -> std::uint8_t;
    auto currentMode(std::uint8_t value) -> void;
    
    auto printSectionBegin() -> void;
    
    auto turnYellow() -> void;
    auto turnRed() -> void;
    auto turnGreen() -> void;
    auto turnBlue() -> void;
    auto turnNormal() -> void;
    auto turnBrightWhite() -> void;
    auto printDone() -> void;
    auto printFailed() -> void;
    auto printPassed() -> void;
    
    auto clearScreen() -> void;
    auto start(const std::string &temp) -> void;
    auto printBasedOnVal(bool value) -> void;
    auto moveTo(std::int32_t x, std::int32_t y = -1) -> void; // y=-1 will move on the current line
    
    auto logEcho() const -> bool;
    auto logEcho(bool value) -> void;
    auto printSpecial(std::uint8_t color, const std::string &msg) -> void;
    auto poll() -> void;
    
    auto registerKey(std::int32_t key, std::string cmdName, std::uint16_t scriptId) -> void;
    auto registerFunc(const std::string &key, const std::string &cmdName, std::uint16_t scriptId) -> void;
    auto setKeyStatus(std::int32_t key, bool isEnabled) -> void;
    auto setFuncStatus(const std::string &key, bool isEnabled) -> void;
    auto registration() -> void;
    
private:
    auto reset() -> void;
    auto windowSize() -> std::pair<int, int>;
    auto setTitle(const std::string &title) -> void;
    auto sendCMD(const std::string &cmd) -> Console &;
    
    auto printStartOfLine() -> void;
    auto startOfLineCheck() -> void;
    auto cl_getch() -> std::int32_t;
    auto process(std::int32_t c) -> void;
    auto displaySettings() -> void;
    
    struct JSConsoleEntry {
        std::uint16_t scriptId;
        bool isEnabled;
        std::string cmdName;
        JSConsoleEntry() : scriptId(0), isEnabled(true), cmdName("") {}
        JSConsoleEntry(std::uint16_t id, const std::string &cName)
        : scriptId(id), isEnabled(true), cmdName(cName) {}
    };
    
    
    std::map<std::int32_t, JSConsoleEntry> jskeyHandler;
    std::map<std::string, JSConsoleEntry> jsconsoleFunctions;
    std::uint16_t width, height; // for differing windows
    std::uint16_t curLeft, curTop;
    std::bitset<16> filterSettings;
    std::uint8_t curMode;
    std::uint8_t previousColour;
    bool enableLogEcho;
    bool is_initialized;
    
    static const std::string ESC;
    static const std::string CSI;
    static const std::string BEL;
    //  Command sequences
    static const std::string ATTRIBUTE; // find/replace on # with attribute number
    static const std::string MOVE;      // we are going to find/replace on ROW/COL
    static const std::string HORIZMOVE;
    static const std::string VERTMOVE;
    static const std::string OFFCURSOR;
    static const std::string ONCURSOR;
    
    static const std::string ONGRAPHIC;
    static const std::string OFFGRAPHIC;
    
    static const std::string SETTITLE;
    
    static const std::string CLEAR;
};

// o------------------------------------------------------------------------------------------------o
//  CEndL
// o------------------------------------------------------------------------------------------------o
// o------------------------------------------------------------------------------------------------o
class CEndL {
public:
    void Action(Console &test) {
        test << "\n";
        test.Flush();
    }
};
// o------------------------------------------------------------------------------------------------o
extern CEndL myendl;

#endif
