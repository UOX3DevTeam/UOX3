// o------------------------------------------------------------------------------------------------o
//| console.cpp
// o------------------------------------------------------------------------------------------------o
//| In essence, Console is our nice generic class for outputting display.
//| As long as the interface is kept the same, we can display whatever we wish
//| too. We store a set of coordinates being the window corner and size, for
//| systems with windowing support, which describes the window we are in.
// o------------------------------------------------------------------------------------------------o
//| Version History
//| 1.7		Added comments (29. October 2001)
//| 2.0		Added filter masking (22nd January 2002)
// o------------------------------------------------------------------------------------------------o

// Header
#include "console.hpp"

#include <cctype>
#include <fstream>
#include <iostream>
#include <stdexcept>
#if !defined(_WIN32)
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
struct termios initial_terminal_state;
#else
#include <windows.h>
#include <conio.h>
DWORD initial_terminal_state;
#endif

#include "cchar.h"
#include "cguild.h"
#include "chtmlsystem.h"
#include "citem.h"
#include "cjsmapping.h"
#include "cmagic.h"
#include "commands.h"
#include "craces.h"
#include "cscript.h"
#include "cserverdefinitions.h"
#include "csocket.h"
#include "cthreadqueue.h"
#include "funcdecl.h"
#include "magic.h"
#include "network.h"
#include "objectfactory.h"
#include "regions.h"
#include "configuration/serverconfig.hpp"
#include "ssection.h"
#include "stringutility.hpp"
#include "utility/strutil.hpp"

#include "teffect.h"
#include "other/uoxglobal.hpp"

using namespace std::string_literals;
CEndL myendl;

constexpr auto NORMALMODE = std::uint8_t(0);
constexpr auto WARNINGMODE = std::uint8_t(1);
constexpr auto ERRORMODE = std::uint8_t(2);
// const std::uint8_t COLOURMODE			= 3;
const std::string Console::ESC = "\x1b"s;
const std::string Console::CSI = Console::ESC + "["s;
const std::string Console::BEL = "\x07"s;
//  Command sequences
const std::string Console::ATTRIBUTE =
Console::CSI + "#m"s; // find/replace on # with attribute number
const std::string Console::MOVE =
Console::CSI + "ROW;COLH"s; // we are going to find/replace on ROW/COL
const std::string Console::HORIZMOVE = Console::CSI + "COLG"s;
const std::string Console::VERTMOVE = Console::CSI + "ROWd"s;
const std::string Console::OFFCURSOR = Console::CSI + "?25l"s;
const std::string Console::ONCURSOR = Console::CSI + "?25h"s;

const std::string Console::ONGRAPHIC = Console::ESC + "(0"s;
const std::string Console::OFFGRAPHIC = Console::ESC + "(B"s;

const std::string Console::SETTITLE = Console::ESC + "]2;TITLEs"s + Console::BEL;

const std::string Console::CLEAR = Console::CSI + "2J"s;

// Forward function declarations
auto endMessage(std::int32_t x) -> void;
auto loadCustomTitle() -> void;
auto loadSkills() -> void;
auto loadSpawnRegions() -> void;
auto loadRegions() -> void;
auto loadTeleportLocations() -> void;
auto loadCreatures() -> void;
auto loadPlaces() -> void;
auto UnloadRegions() -> void;
auto UnloadSpawnRegions() -> void;
auto loadTeleportLocations() -> void;

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Console()
// o------------------------------------------------------------------------------------------------o
//| Purpose		-	Class Constructor and deconstructor
// o------------------------------------------------------------------------------------------------o
Console::Console(): width(80), height(25), curMode(NORMALMODE), previousColour(CNORMAL), enableLogEcho(false) {
    is_initialized = false;
}
//===============================================================================
auto Console::shared() -> Console & {
    static Console instance;
    return instance;
}
//===============================================================================
Console::~Console() {
    if (is_initialized) {
        moveTo(1, height);
        reset();
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Console::Initialize()
// o------------------------------------------------------------------------------------------------o
//| Purpose		-	Prepare console/terminal for UOX3 output
// o------------------------------------------------------------------------------------------------o
auto Console::Initialize() -> void {
    is_initialized = true;
#if !defined(_WIN32)
    tcgetattr(1, &initial_terminal_state); // get the current state of the terminal
    auto temp = initial_terminal_state;
    temp.c_lflag = temp.c_lflag & (~ECHO) & (~ICANON); // Disable echo and canonical (line) mode
    temp.c_cc[VMIN] = 0;                               // in non canonical mode we non blocking read
    temp.c_cc[VTIME] = 0;                              // in non canonical mode we non blocking read
    tcsetattr(1, TCSANOW, &temp);
#else
    // Set the input to non echo and non canonical (line) mode
    auto hco = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleMode(hco, &initial_terminal_state);
    auto temp = initial_terminal_state;
    temp = (temp & (~ENABLE_ECHO_INPUT) & (~ENABLE_LINE_INPUT) & (~ENABLE_MOUSE_INPUT));
    SetConsoleMode(hco, initial_terminal_state);
#endif
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Console()::reset()
// o------------------------------------------------------------------------------------------------o
//| Purpose		-	Reset console/terminal to its original state
// o------------------------------------------------------------------------------------------------o
auto Console::reset() -> void {
    std::cout.flush();
#if !defined(_WIN32)
    tcsetattr(1, TCSAFLUSH, &initial_terminal_state);
    std::cout << std::endl;
#else
    auto hco = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleMode(hco, initial_terminal_state);
#endif
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Console::setTitle()
// o------------------------------------------------------------------------------------------------o
//| Purpose		-	Set console/terminal window title
// o------------------------------------------------------------------------------------------------o
auto Console::setTitle(const std::string &value) -> void {
#if !defined(_WIN32)
    auto cmd = SETTITLE;
    auto loc = cmd.find("TITLE");
    cmd.replace(loc, 5, value);
    sendCMD(cmd);
#else
    SetConsoleTitle(value.c_str());
#endif
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Console::sendCMD()
// o------------------------------------------------------------------------------------------------o
//| Purpose		-	Send cmd to terminal
// o------------------------------------------------------------------------------------------------o
auto Console::sendCMD(const std::string &cmd) -> Console & {
    std::cout << cmd;
    std::cout.flush();
    return *this;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Console::windowSize()
// o------------------------------------------------------------------------------------------------o
//| Purpose		-	Set window size in columns and rows
// o------------------------------------------------------------------------------------------------o
auto Console::windowSize() -> std::pair<int, int> {
    int row = 0;
    int col = 0;
    
#if !defined(_WIN32)
    // Get the window size
    winsize winsz;
    ioctl(0, TIOCGWINSZ, &winsz);
    col = winsz.ws_col;
    row = winsz.ws_row;
#else
    HANDLE hco = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hco, &csbi);
    col = csbi.dwSize.X;
    row = csbi.dwSize.Y;
#endif
    return std::make_pair(row, col);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Console()::clearScreen()
// o------------------------------------------------------------------------------------------------o
//| Purpose		-	Clears the screen
// o------------------------------------------------------------------------------------------------o
auto Console::clearScreen() -> void {
#if defined(_WIN32)
    unsigned long y;
    COORD xy;
    
    xy.X = 0;
    xy.Y = 0;
    auto hco = GetStdHandle(STD_OUTPUT_HANDLE);
    FillConsoleOutputCharacter(hco, ' ', width * height, xy, &y);
    SetConsoleCursorPosition(hco, xy);
#else
    std::string cmd = CSI;
    cmd = cmd + std::string("2J");
    sendCMD(cmd);
#endif
}

// o------------------------------------------------------------------------------------------------o
//| Function	-	<< Overrriding
// o------------------------------------------------------------------------------------------------o
//| Purpose		-	Let you use << and >>
// o------------------------------------------------------------------------------------------------o
auto Console::operator<<(const char *output) -> Console & {
    startOfLineCheck();
#if defined(_WIN32)
    CONSOLE_SCREEN_BUFFER_INFO ScrBuffInfo;
    auto hco = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hco, &ScrBuffInfo);
    std::string toDisplay = output;
    if (ScrBuffInfo.dwCursorPosition.X + toDisplay.length() > ScrBuffInfo.dwSize.X) {
        bool wrapDone = false;
        while (!wrapDone) {
            GetConsoleScreenBufferInfo(hco, &ScrBuffInfo);
            std::int32_t diff = ScrBuffInfo.dwSize.X - ScrBuffInfo.dwCursorPosition.X - 1;
            if (diff > toDisplay.length()) {
                std::cout << toDisplay;
                wrapDone = true;
            }
            else {
                std::cout << toDisplay.substr(0, diff);
                (*this) << myendl;
                startOfLineCheck();
                toDisplay = toDisplay.substr(diff);
            }
        }
    }
    else {
        std::cout << output;
    }
#else
    std::cout << output;
#endif
    return (*this);
}
//================================================================================================
auto Console::operator<<(const std::uint8_t *output) -> Console & {
    startOfLineCheck();
    std::cout << reinterpret_cast<const char *>(output);
    return (*this);
}
//================================================================================================
auto Console::operator<<(const std::int8_t *output) -> Console & {
    startOfLineCheck();
    std::cout << reinterpret_cast<const char *>(output);
    return (*this);
}
//================================================================================================
auto Console::operator<<(const std::uint8_t &output) -> Console & {
    startOfLineCheck();
    std::cout << static_cast<std::uint16_t>(output);
    return (*this);
}
//================================================================================================
auto Console::operator<<(const std::int8_t &output) -> Console & {
    startOfLineCheck();
    std::cout << static_cast<std::int16_t>(output);
    return (*this);
}
//================================================================================================
auto Console::operator<<(const std::uint16_t &output) -> Console & {
    startOfLineCheck();
    std::cout << output;
    return (*this);
}
//================================================================================================
auto Console::operator<<(const std::int16_t &output) -> Console & {
    startOfLineCheck();
    std::cout << output;
    return (*this);
}
//================================================================================================
auto Console::operator<<(const std::uint32_t &output) -> Console & {
    startOfLineCheck();
    std::cout << output;
    return (*this);
}
//================================================================================================
auto Console::operator<<(const std::int32_t &output) -> Console & {
    startOfLineCheck();
    std::cout << output;
    return (*this);
}

//================================================================================================
auto Console::operator<<(const std::uint64_t &output) -> Console & {
    startOfLineCheck();
    std::cout << output;
    return (*this);
}
//================================================================================================
auto Console::operator<<(const std::int64_t &output) -> Console & {
    startOfLineCheck();
    std::cout << output;
    return (*this);
}

//================================================================================================
auto Console::operator<<(const float &output) -> Console & {
    startOfLineCheck();
    std::cout << output;
    return (*this);
}
//================================================================================================
auto Console::operator<<(const double &output) -> Console & {
    startOfLineCheck();
    std::cout << output;
    return (*this);
}
//================================================================================================
auto Console::operator<<(const long double &output) -> Console & {
    startOfLineCheck();
    std::cout << output;
    return (*this);
}

//================================================================================================
auto Console::operator<<(CBaseObject *output) -> Console & {
    startOfLineCheck();
    (*this) << output->GetSerial();
    return (*this);
}
//================================================================================================
auto Console::operator<<(const CBaseObject *output) -> Console & {
    startOfLineCheck();
    (*this) << output->GetSerial();
    return (*this);
}
//================================================================================================
auto Console::operator<<(const CBaseObject &output) -> Console & {
    startOfLineCheck();
    (*this) << output.GetSerial();
    return (*this);
}

//================================================================================================
auto Console::operator<<(const std::string &output) -> Console & {
    startOfLineCheck();
    std::cout << output;
    return (*this);
}

//================================================================================================
auto Console::operator<<([[maybe_unused]] CEndL &myObj) -> Console & {
    if (curLeft == 0) {
        printStartOfLine();
    }
    std::cout << std::endl;
    curLeft = 0;
    ++curTop;
    return (*this);
}

// o------------------------------------------------------------------------------------------------o
//| Function	-	Console::print()
// o------------------------------------------------------------------------------------------------o
//| Purpose		-	Prints the console with the "|"
// o------------------------------------------------------------------------------------------------o
auto Console::print(const std::string &msg) -> void {
    startOfLineCheck();
    std::cout << msg;
    if ((msg.size() > 0) && (msg[msg.size() - 1] == '\n')) {
        curLeft = 0;
    }
}

// o------------------------------------------------------------------------------------------------o
//| Function	-	Console::log()
// o------------------------------------------------------------------------------------------------o
//| Purpose		-	Writes to the logfile
// o------------------------------------------------------------------------------------------------o
auto Console::log(const std::string &msg, const std::string &filename) -> void {
        if (ServerConfig::shared().enabled(ServerSwitch::CONSOLELOG)) {
            std::ofstream toWrite;
            auto realFileName = std::filesystem::path() ;
            realFileName = ServerConfig::shared().directoryFor(dirlocation_t::LOG) / std::filesystem::path(filename);
            
            char timeStr[256];
            RealTime(timeStr);
            
            toWrite.open(realFileName.string(), std::ios::out | std::ios::app);
            if (toWrite.is_open()) {
                toWrite << "[" << timeStr << "] " << msg << std::endl;
            }
            toWrite.close();
            if (logEcho()) {
                print(util::format("%s%s\n", timeStr, msg.c_str()));
            }
        }
}

// o------------------------------------------------------------------------------------------------o
//| Function	-	Console::log()
// o------------------------------------------------------------------------------------------------o
//| Purpose		-	Log to the console.log file
// o------------------------------------------------------------------------------------------------o
auto Console::log(const std::string &msg) -> void {
    if (ServerConfig::shared().enabled(ServerSwitch::CONSOLELOG)) {
        log(msg, "console.log");
    }
}

// o------------------------------------------------------------------------------------------------o
//| Function	-	Console::error()
// o------------------------------------------------------------------------------------------------o
//| Purpose		-	Writes to the error log and the console.
// o------------------------------------------------------------------------------------------------o
auto Console::error(const std::string &msg) -> void {
    try {
        std::uint8_t oldMode = currentMode();
        currentMode(ERRORMODE);
        log(msg, "error.log");
        if (curLeft != 0) {
            (*this) << myendl;
        }
        turnRed();
        (*this) << "ERROR: " << (const char *)&msg[0] << myendl;
        turnNormal();
        currentMode(oldMode);
    }
    catch (const std::exception &e) {
        std::cerr << "Error print reporting 'error'.  Error was: " << e.what() << std::endl;
        exit(1); // This seems pretty dangerous, as UOX3 starts other threads this doenst let them
        // get joined
    }
}

// o------------------------------------------------------------------------------------------------o
//| Function	-	Console::printSectionBegin()
// o------------------------------------------------------------------------------------------------o
//| Purpose		-	Prints a section seperator
// o------------------------------------------------------------------------------------------------o
auto Console::printSectionBegin() -> void {
    turnBrightWhite();
    std::cout << "o";
    for (auto j = 1; j < width - 1; ++j) {
        std::cout << "-";
    }
    std::cout << "o";
    curLeft = 0;
    curTop = 0;
    turnNormal();
}

// o------------------------------------------------------------------------------------------------o
//| Function	-	Console::start()
// o------------------------------------------------------------------------------------------------o
//| Purpose		-	Migration of constart function to here
// o------------------------------------------------------------------------------------------------o
auto Console::start(const std::string &temp) -> void {
    auto size = windowSize();
    width = std::get<1>(size);
    height = std::get<0>(size);
    setTitle(temp);
}

// o------------------------------------------------------------------------------------------------o
//| Function	-	Console::turnYellow()
/// o------------------------------------------------------------------------------------------------o
//| Purpose		-	Turns the text yellow
// o------------------------------------------------------------------------------------------------o
auto Console::turnYellow() -> void {
#if defined(_WIN32)
    auto hco = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hco, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
#else
    std::cout << "\033[1;33m";
#endif
    previousColour = CYELLOW;
}

// o------------------------------------------------------------------------------------------------o
//| Function	-	Console::turnRed()
// o------------------------------------------------------------------------------------------------o
//| Purpose		-	Turns the text red
// o------------------------------------------------------------------------------------------------o
auto Console::turnRed() -> void {
#if defined(_WIN32)
    auto hco = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hco, FOREGROUND_RED | FOREGROUND_INTENSITY);
#else
    std::cout << "\033[1;31m";
#endif
    previousColour = CRED;
}

// o------------------------------------------------------------------------------------------------o
//| Function	-	Console::turnGreen()
// o------------------------------------------------------------------------------------------------o
//| Purpose		-	Turns the text green
// o------------------------------------------------------------------------------------------------o
auto Console::turnGreen() -> void {
#if defined(_WIN32)
    auto hco = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hco, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#else
    std::cout << "\033[1;32m";
#endif
    previousColour = CGREEN;
}

// o------------------------------------------------------------------------------------------------o
//| Function	-	Console::turnBlue()
// o------------------------------------------------------------------------------------------------o
//| Purpose		-	Turns the text blue
// o------------------------------------------------------------------------------------------------o
auto Console::turnBlue() -> void {
#if defined(_WIN32)
    auto hco = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hco, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#else
    std::cout << "\033[1;34m";
#endif
    previousColour = CBLUE;
}

// o------------------------------------------------------------------------------------------------o
//| Function	-	Console::turnNormal()
// o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns text to normal white
// o------------------------------------------------------------------------------------------------o
auto Console::turnNormal() -> void {
#if defined(_WIN32)
    auto hco = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hco, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN);
#else
    std::cout << "\033[0;37m";
#endif
    previousColour = CNORMAL;
}

// o------------------------------------------------------------------------------------------------o
//| Function	-	Console::turnBrightWhite()
// o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns text to bright white
// o------------------------------------------------------------------------------------------------o
auto Console::turnBrightWhite() -> void {
#if defined(_WIN32)
    auto hco = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hco, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#else
    std::cout << "\033[1;37m";
#endif
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Console::printDone()
// o------------------------------------------------------------------------------------------------o
//| Purpose		-	Prints colored "[done]" message
// o------------------------------------------------------------------------------------------------o
auto Console::printDone() -> void { printSpecial(CGREEN, "done"); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Console::printFailed()
// o------------------------------------------------------------------------------------------------o
//| Purpose		-	Prints colored "[failed]" message
// o------------------------------------------------------------------------------------------------o
auto Console::printFailed() -> void { printSpecial(CRED, "Failed"); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Console::printPassed()
// o------------------------------------------------------------------------------------------------o
//| Purpose		-	Prints colored "[skipped]" message
// o------------------------------------------------------------------------------------------------o
auto Console::printPassed() -> void { printSpecial(CYELLOW, "Skipped"); }


// o------------------------------------------------------------------------------------------------o
//| Function	-	Console::printBasedOnVal()
// o------------------------------------------------------------------------------------------------o
//| Purpose		-	Prints "done" or "failed" messages based on boolean passed in
// o------------------------------------------------------------------------------------------------o
auto Console::printBasedOnVal(bool value) -> void {
    if (value) {
        printDone();
    }
    else {
        printFailed();
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Console::warning()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Writes to the warning log and the console.
// o------------------------------------------------------------------------------------------------o
auto Console::warning(const std::string &msg) -> void {
    std::uint8_t oldMode = currentMode();
    currentMode(WARNINGMODE);
    log(msg, "warning.log");
    if (curLeft != 0) {
        (*this) << myendl;
    }
    turnBlue();
    (*this) << "WARNING: " << (const char *)&msg[0] << myendl;
    turnNormal();
    currentMode(oldMode);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Console::currentMode()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets console's output mode
// o------------------------------------------------------------------------------------------------o
auto Console::currentMode() const -> std::uint8_t { return curMode; }
auto Console::currentMode(std::uint8_t value) -> void { curMode = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Console::printStartOfLine()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Set colour of console text at start of a new line
// o------------------------------------------------------------------------------------------------o
auto Console::printStartOfLine() -> void {
    turnBrightWhite();
    std::cout << "| ";
    switch (previousColour) {
        case CBLUE:
            turnBlue();
            break;
        case CRED:
            turnRed();
            break;
        case CGREEN:
            turnGreen();
            break;
        case CYELLOW:
            turnYellow();
            break;
        case CBWHITE:
            break; // current colour
        case CNORMAL:
        default:
            turnNormal();
            break;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Console::moveTo( std::int32_t x, std::int32_t y )
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Set console cursor position
// o------------------------------------------------------------------------------------------------o
#if defined(_WIN32)
auto Console::moveTo(std::int32_t x, std::int32_t y) -> void {
    auto hco = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD Pos;
    if (y == -1) {
        CONSOLE_SCREEN_BUFFER_INFO ScrBuffInfo;
        
        GetConsoleScreenBufferInfo(hco, &ScrBuffInfo);
        Pos.Y = ScrBuffInfo.dwCursorPosition.Y;
        Pos.X = static_cast<std::uint16_t>(x);
        SetConsoleCursorPosition(hco, Pos);
    }
    else {
        Pos.X = static_cast<std::uint16_t>(x);
        Pos.Y = static_cast<std::uint16_t>(y);
        SetConsoleCursorPosition(hco, Pos);
    }
}
#else
void Console::moveTo(std::int32_t x, [[maybe_unused]] std::int32_t y) {
    std::cout << "\033[255D";
    std::cout << "\033[" << x << "C";
}
#endif

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Console::logEcho()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks whether console should echo log messages
// o------------------------------------------------------------------------------------------------o
auto Console::logEcho() const -> bool { return enableLogEcho; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Console::logEcho()
//|	Date		-	2/03/2003
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Enables/Disables whether console should echo log messages
// o------------------------------------------------------------------------------------------------o
auto Console::logEcho(bool value) -> void { enableLogEcho = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Console::startOfLineCheck()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Reduces excess work in overloaded functions by checking
//|					to see if the start of line has to be done here
// o------------------------------------------------------------------------------------------------o
auto Console::startOfLineCheck() -> void {
    if (curLeft == 0) {
        printStartOfLine();
        curLeft = 1;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Console::printSpecial()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	This is much like PrintFailed, PrintDone and so on except
//|					we specify the text and the colour
// o------------------------------------------------------------------------------------------------o
auto Console::printSpecial(std::uint8_t colour, const std::string &msg) -> void {
    startOfLineCheck();
    size_t stringLength = msg.size() + 3;
    moveTo(static_cast<std::int32_t>(width - stringLength));
    turnNormal();
    (*this) << "[";
    switch (colour) {
        default:
        case CNORMAL:
            break;
        case CBLUE:
            turnBlue();
            break;
        case CRED:
            turnRed();
            break;
        case CGREEN:
            turnGreen();
            break;
        case CYELLOW:
            turnYellow();
            break;
        case CBWHITE:
            turnBrightWhite();
            break;
    }
    (*this) << msg;
    turnNormal();
    (*this) << "]" << myendl;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Console::cl_getch()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Read a character from stdin, in a cluox compatble way.
//|					This routine is non-blocking!
//|	Returns		-	>0 -> character read
//|					-1 -> no char available.
// o------------------------------------------------------------------------------------------------o
//|	Notes		-	now cluox is GUI wrapper over uox using stdin and stdout redirection
// to capture
//|					the console, if it is active uox can't use kbhit() to determine
//if
// there is a |					character aviable, it can only get one directly by
// getch().
//|					However the problem arises that uox will get blocked if none
//is aviable.
//|					The solution to this problem is that cluox also hands over
//the second pipe-end
//|					of stdin so uox can write itself into this stream. To determine
//if a character is
//|					now done that way. UOX write's itself a ZERO on the other end
//of the pipe, and reads |					a charecter, if it is again the same
//ZERO just putted in nothing was entered. However
//|					it is not a zero the user has entered a char.
// o------------------------------------------------------------------------------------------------o
auto Console::cl_getch() -> std::int32_t {
#if !defined(_WIN32)
    char data = 0;
    auto a = ::read(0, &data, 1); // This doesn't block on getting a line due to initalization
    if (a > 0) {
        // Look for escape!
        if (data == 27) {
            // This was an escape, see if we have another item
            a = ::read(0, &data, 1);
            if (a > 0) {
                // See if this is a CSI
                if (data == '[') {
                    // It is an control sequence!
                    // Some sequences end with a ~, some dont.  So, we now get to see
                    a = ::read(0, &data, 1);
                    while (a > 0) {
                        if ((data == 'A') || (data == 'B') || (data == 'C') || (data == 'D') || ((data == 'H') && (data == 'F')) || (data == '~')) {
                            break;
                        }
                        a = ::read(0, &data, 1);
                    }
                    return -1;
                }
                else if (data == 'O'){ // this could be F1-F4
                    a = ::read(0, &data, 1);
                    if (a > 0) {
                        if ((data == 'P') || (data == 'Q') || (data == 'R') || (data == 'S')) {
                            // It was F1,F2,F3,F4
                            return -1;
                        }
                        // Soooooo, we have now lost two characters, this one and the preceeding O
                        // that where after the ESC
                        return 27;
                    }
                    // It wasnt, so we have a choice, return -1 (since we got ESC O, or return ESC
                    // and lose O
                    return 27;
                }
                else {
                    // Ok, it isn't an CSI (Control Sequence).  But we have all ready lost/read the
                    // next character.  That will be lost, return escape
                    return 27;
                }
            }
            else {
                // There was nothing after the escape, so just send the escape
                return 27; // return escape
            }
        }
        // Not escape if here, so just return it
        return static_cast<std::int32_t>(data);
    }
    else {
        return -1;
    }
    
#else
    // uox is not wrapped simply use the kbhit routine
    if (_kbhit()) {
        return _getch();
    }
    else {
        return -1;
    }
#endif
}

// o------------------------------------------------------------------------------------------------o
//| Function	-	Console::poll()
// o------------------------------------------------------------------------------------------------o
//| Purpose		-	Facilitate console control. SysOp keys, and localhost
//|					controls.
// o------------------------------------------------------------------------------------------------o
auto Console::poll() -> void {
    std::int32_t c = cl_getch();
    if (c > 0) {
        c = std::toupper(c);
        process(c);
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Console::process()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle keypresses in console
//|	Changes		-	05042004 - added some console debugging stuff.
//|					Ideally this will all be placed onto a DEBUG menu which
//|					a user will be able to select using the <, and > keys
//|					respectivly. This functionality hasn't been implemented
//|					at the current time of writing, but will be when possible.
// o------------------------------------------------------------------------------------------------o
auto Console::process(std::int32_t c) -> void {
    if (c == '*') {
        if (worldMain.GetSecure()) {
            messageLoop << "Secure mode disabled. Press ? for a commands list";
        }
        else {
            messageLoop << "Secure mode re-enabled";
        }
        worldMain.SetSecure(!worldMain.GetSecure());
        return;
    }
    else {
        if (worldMain.GetSecure()) {
            messageLoop << "Secure mode prevents keyboard commands! Press '*' to disable";
            return;
        }
        
        auto toFind = jskeyHandler.find(c);
        if (toFind != jskeyHandler.end()) {
            if (toFind->second.isEnabled) {
                cScript *toExecute = JSMapping->GetScript(toFind->second.scriptId);
                if (toExecute) {
                    // All commands that execute are of the form: command_commandname (to avoid
                    // possible clashes)
#if defined(UOX_DEBUG_MODE)
                    print(util::format("Executing JS keystroke %c %s\n", c, toFind->second.cmdName.c_str()));
#endif
                    jsval eventRetVal;
                    [[maybe_unused]] JSBool retVal = toExecute->CallParticularEvent(toFind->second.cmdName.c_str(), nullptr, 0, &eventRetVal);
                }
                return;
            }
        }
        [[maybe_unused]] CSocket *tSock = nullptr;
        // char outputline[128], temp[1024];
        std::string outputline, temp;
        std::int32_t indexcount = 0;
        bool kill = false;
        std::int32_t j = 0;
        switch (c) {
            case '!':
                // Force server to save accounts file
                messageLoop << "CMD: Saving Accounts... ";
                Account::shared().save();
                messageLoop << MSG_PRINTDONE;
                break;
            case '@':
                // Force server to save all files.(Manual save)
                messageLoop << MSG_WORLDSAVE;
                break;
            case 'Y':
                std::int32_t keyresp;
                std::cout << "System: ";
                std::cout.flush();
                while (!kill) {
                    keyresp = cl_getch();
                    switch (keyresp) {
                        case -1: // no key pressed
                        case 0:
                            break;
                        case 0x1B:
                            outputline = "";
                            indexcount = 0;
                            kill = true;
                            std::cout << std::endl;
                            messageLoop << "CMD: System broadcast canceled.";
                            break;
                        case 0x08:
                            --indexcount;
                            if (indexcount < 0) {
                                indexcount = 0;
                            }
                            else {
                                std::cout << "\b \b";
                            }
                            break;
                        case 0x0A:
                        case 0x0D:
                            outputline[indexcount] = 0;
                            messageLoop.NewMessage(MSG_CONSOLEBCAST, outputline.c_str());
                            indexcount = 0;
                            kill = true;
                            std::cout << std::endl;
                            temp = util::format("CMD: System broadcast sent message \"%s\"", outputline.c_str());
                            outputline = "";
                            messageLoop << temp;
                            break;
                        default:
                            outputline = outputline + std::string(1, static_cast<std::int8_t>(keyresp));
                            indexcount = indexcount + 1;
                            std::cout << static_cast<std::int8_t>(keyresp);
                            std::cout.flush();
                            break;
                    }
                    keyresp = 0x00;
                }
                break;
            case '[': {
                // We want to group all the contents of the multimap container numerically by group. We
                // rely on the self ordering in the multimap implementation to do this.
                messageLoop << "  ";
                messageLoop << "Auto-AddMenu Statistics";
                messageLoop << "  ";
                std::string szBuffer;
                // We need to get an iteration into the map first of all the top level ULONGs then we
                // can get an equal range.
                std::map<std::uint32_t, std::uint8_t> localMap;
                localMap.clear();
                for (auto CJ = g_mmapAddMenuMap.begin(); CJ != g_mmapAddMenuMap.end();
                     CJ++) {
                    // check to see if the group id has been checked already
                    if (localMap.find(CJ->first) == localMap.end()) {
                        localMap.insert(std::make_pair(CJ->first, 0));
                        szBuffer = "";
                        szBuffer = util::format("AddMenuGroup %u:", CJ->first);
                        messageLoop << szBuffer;
                        auto pairRange =
                        g_mmapAddMenuMap.equal_range(CJ->first);
                        std::int32_t count = 0;
                        for (auto CI = pairRange.first; CI != pairRange.second; CI++) {
                            count++;
                        }
                        szBuffer = "";
                        szBuffer = util::format("   Found %i Auto-AddMenu Item(s).", count);
                        messageLoop << szBuffer;
                    }
                }
                messageLoop << MSG_SECTIONBEGIN;
                break;
            }
            case '<':
                messageLoop << "Function not implemented.";
                break;
            case '>':
                messageLoop << "Function not implemented.";
                break;
            case 0x1B:
            case 'Q':
                messageLoop << MSG_SECTIONBEGIN;
                messageLoop << "CMD: Immediate Shutdown initialized!";
                messageLoop << MSG_SHUTDOWN;
                break;
            case '0':
                if (!worldMain.GetReloadingScripts()) {
                    worldMain.SetReloadingScripts(true);
                    // Reload all the files. If there are issues with these files change the order
                    // reloaded from here first.
                    ServerConfig::shared().loadConfig(std::filesystem::path());
                    messageLoop << "CMD: Loading All";
                    messageLoop << "     Server INI... ";
                    // Reload accounts, and update Access.adm if new accounts available.
                    messageLoop << "     Loading Accounts... ";
                    Account::shared().load();
                    messageLoop << MSG_PRINTDONE;
                    // Reload Region Files
                    messageLoop << "     Loading Regions... ";
                    UnloadRegions();
                    loadRegions();
                    messageLoop << MSG_PRINTDONE;
                    messageLoop << "     Loading Teleport Locations... ";
                    loadTeleportLocations();
                    messageLoop << MSG_PRINTDONE;
                    // Reload the serve spawn regions
                    messageLoop << "     Loading Spawn Regions... ";
                    UnloadSpawnRegions();
                    loadSpawnRegions();
                    messageLoop << MSG_PRINTDONE;
                    // Reload the server command list
                    messageLoop << "     Loading commands... ";
                    serverCommands.load();
                    messageLoop << MSG_PRINTDONE;
                    // Reload DFN's
                    messageLoop << "     Loading Server DFN... ";
                    FileLookup->Reload();
                    messageLoop << MSG_PRINTDONE;
                    // messageLoop access is REQUIRED, as this function is executing in a different
                    // thread, so we need thread safety
                    messageLoop << "     Loading JSE Scripts... ";
                    
                    // Reload the current Spells
                    messageLoop << "     Loading spells... ";
                    Magic->LoadScript();
                    messageLoop << MSG_PRINTDONE;
                    // Reload the HTML output templates
                    messageLoop << "     Loading HTML Templates... ";
                    HTMLTemplates->Unload();
                    HTMLTemplates->load();
                    worldMain.SetReloadingScripts(false);
                    messageLoop << MSG_PRINTDONE;
                }
                else {
                    messageLoop << "Server can only load one script at a time";
                }
                break;
            case 'T':
                // Timed shut down(10 minutes)
                messageLoop << "CMD: 10 Minute Server Shutdown Announced(Timed)";
                worldMain.SetEndTime(BuildTimeValue(600));
                endMessage(0);
                break;
            case 'D':
                // Disconnect account 0 (useful when client crashes)
                for (auto &tSock : Network->connClients) {
                    if (tSock->AcctNo() == 0) {
                        Network->Disconnect(tSock);
                    }
                }
                messageLoop << "CMD: Socket Disconnected(Account 0).";
                break;
            case 'K': {
                for (auto &tSock : Network->connClients) {
                    Network->Disconnect(tSock);
                }
                messageLoop << "CMD: All Connections Closed.";
            } break;
            case 'P': {
                std::uint32_t networkTimeCount = worldMain.ServerProfile()->NetworkTimeCount();
                std::uint32_t timerTimeCount = worldMain.ServerProfile()->TimerTimeCount();
                std::uint32_t autoTimeCount = worldMain.ServerProfile()->AutoTimeCount();
                std::uint32_t loopTimeCount = worldMain.ServerProfile()->LoopTimeCount();
                // 1/13/2003 - Dreoth - Log Performance Information enhancements
                logEcho(true);
                log("--- Starting Performance Dump ---", "performance.log");
                log("\tPerformance Dump:", "performance.log");
                log(util::format("\tNetwork code: %.2fmsec [%i samples]", static_cast<float>(static_cast<float>(worldMain.ServerProfile()->NetworkTime()) /  static_cast<float>(networkTimeCount)), networkTimeCount), "performance.log");
                log(util::format("\tTimer code: %.2fmsec [%i samples]", static_cast<float>(static_cast<float>(worldMain.ServerProfile()->TimerTime()) / static_cast<float>(timerTimeCount)), timerTimeCount), "performance.log");
                log(util::format("\tAuto code: %.2fmsec [%i samples]", static_cast<float>(static_cast<float>(worldMain.ServerProfile()->AutoTime()) / static_cast<float>(autoTimeCount)), autoTimeCount), "performance.log");
                log(util::format("\tLoop Time: %.2fmsec [%i samples]", static_cast<float>(static_cast<float>(worldMain.ServerProfile()->LoopTime()) / static_cast<float>(loopTimeCount)), loopTimeCount), "performance.log");
                
                log(util::format("\tCharacters: %i/%i - Items: %i/%i (Dynamic)", ObjectFactory::shared().CountOfObjects(CBaseObject::OT_CHAR), ObjectFactory::shared().SizeOfObjects(CBaseObject::OT_CHAR), ObjectFactory::shared().CountOfObjects(CBaseObject::OT_ITEM), ObjectFactory::shared().SizeOfObjects(CBaseObject::OT_ITEM)), "performance.log");
                log(util::format("\tSimulation Cycles: %f per sec", (1000.0 * (1.0 / static_cast<float>(static_cast<float>(worldMain.ServerProfile()->LoopTime()) / static_cast<float>(loopTimeCount))))), "performance.log");
                log(util::format("\tBytes sent: %i", worldMain.ServerProfile()->GlobalSent()), "performance.log");
                log(util::format("\tBytes Received: %i", worldMain.ServerProfile()->GlobalReceived()), "performance.log");
                log("--- Performance Dump Complete ---", "performance.log");
                logEcho(false);
                break;
            }
            case 'W': {
                // Display logged in chars
                messageLoop << "CMD: Current Users in the World:";
                {
                    for (auto &iSock : Network->connClients) {
                        ++j;
                        CChar *mChar = iSock->CurrcharObj();
                        
                        temp = util::format("     %i) %s [%x %x %x %x]", j - 1, mChar->GetName().c_str(), mChar->GetSerial(1), mChar->GetSerial(2), mChar->GetSerial(3), mChar->GetSerial(4));
                        messageLoop << temp;
                    }
                }
                
                temp = util::format("     Total users online: %i", j);
                messageLoop << temp;
                break;
            }
            case 'M':
                std::uint32_t tmp, total;
                total = 0;
                tmp = 0;
                messageLoop << "CMD: UOX Memory Information:";
                std::uint32_t m, n;
                m = static_cast<std::uint32_t>(ObjectFactory::shared().SizeOfObjects(CBaseObject::OT_CHAR));
                total += tmp = m + m * sizeof(CTEffect) + m * sizeof(std::int8_t) + m * sizeof(intptr_t) * 5;
                temp = util::format("     Characters: %u bytes [%u chars ( %u allocated )]", tmp, ObjectFactory::shared().CountOfObjects(CBaseObject::OT_CHAR), m);
                messageLoop << temp;
                n = static_cast<std::uint32_t>(ObjectFactory::shared().SizeOfObjects(CBaseObject::OT_ITEM));
                total += tmp = n + n * sizeof(intptr_t) * 4;
                temp = util::format("     Items: %u bytes [%u items ( %u allocated )]", tmp, ObjectFactory::shared().CountOfObjects(CBaseObject::OT_ITEM), n);
                messageLoop << temp;
                temp = util::format("        You save I: %lu & C: %lu bytes!", m * sizeof(CItem) - ObjectFactory::shared().CountOfObjects(CBaseObject::OT_ITEM), m * sizeof(CChar) - ObjectFactory::shared().CountOfObjects(CBaseObject::OT_CHAR));
                total += tmp = 69 * sizeof(CSpellInfo);
                temp = util::format(temp, "     Spells: %i bytes", tmp);
                messageLoop << "     Sizes:";
                temp = util::format("        CItem  : %lu bytes", sizeof(CItem));
                messageLoop << temp;
                temp = util::format("        CChar  : %lu bytes", sizeof(CChar));
                messageLoop << temp;
                temp = util::format("        TEffect: %lu bytes %lui total)", sizeof(CTEffect), sizeof(CTEffect) * worldMain.tempEffects.Num());
                messageLoop << temp;
                temp = util::format("        Approximate Total: %i bytes", total);
                messageLoop << temp;
                break;
            case '?':
                messageLoop << MSG_SECTIONBEGIN;
                messageLoop << "Console commands:";
                messageLoop << MSG_SECTIONBEGIN;
                messageLoop << " ShardOP:";
                messageLoop << "    * - Lock/Unlock Console ? - serverCommands list(this)";
                messageLoop << "    C - Configuration       H - Unused";
                messageLoop << "    Y - Console Broadcast   Q - Quit/Exit           ";
                messageLoop << " Load serverCommands:";
                messageLoop << "    1 - Ini                 2 - Accounts";
                messageLoop << "    3 - Regions             4 - Spawn Regions";
                messageLoop << "    5 - Spells              6 - serverCommands";
                messageLoop << "    7 - Dfn's               8 - JavaScript";
                messageLoop << "    9 - HTML Templates      0 - ALL(1-9)";
                messageLoop << " Save serverCommands:";
                messageLoop << "    ! - Accounts            @ - World(w/AccountImport)";
                messageLoop << "    # - Unused              $ - Unused";
                messageLoop << "    % - Unused              ^ - Unused";
                messageLoop << "    & - Unused              ( - Unused";
                messageLoop << "    ) - Unused";
                messageLoop << " Server Maintenence:";
                messageLoop << "    P - Performance         W - Characters Online";
                messageLoop << "    M - Memory Information  T - 10 Minute Shutdown";
                messageLoop << "    F - Display Priority Maps";
                messageLoop << " Network Maintenence:";
                messageLoop << "    D - Disconnect Acct0    K - Disconnect All";
                messageLoop << "    Z - Socket Logging      ";
                messageLoop << MSG_SECTIONBEGIN;
                break;
            case 'z':
            case 'Z': {
                auto loggingEnabled = false;
                for (auto &snSock : Network->connClients) {
                    if (snSock) {
                        snSock->Logging(!snSock->Logging());
                    }
                }
                auto iter = Network->connClients.begin();
                if (iter != Network->connClients.end()) {
                    loggingEnabled = (*iter)->Logging();
                }
                if (loggingEnabled) {
                    messageLoop << "CMD: Network Logging Enabled.";
                }
                else {
                    messageLoop << "CMD: Network Logging Disabled.";
                }
                break;
            }
            case 'c':
            case 'C':
                // Shows a configuration header
                displaySettings();
                break;
            case 'f':
            case 'F':
                FileLookup->DisplayPriorityMap();
                break;
            default:
                temp = util::format("Key \'%c\' [%i] does not perform a function", static_cast<std::int8_t>(c), c);
                messageLoop << temp;
                break;
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Console::displaySettings()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	UOX startup stuff
//|					Moved that here because we need it in processkey now
//|
//|	Changes		-	10/21/2002 - found the bug in one spot, just
//|									happened upon this quick fix. for
//BackUp operation.
// o------------------------------------------------------------------------------------------------o
auto Console::displaySettings() -> void {
    std::map<bool, std::string> activeMap;
    activeMap[true] = "Activated!";
    activeMap[false] = "Disabled!";
    
    // UOX.ini status
    (*this) << "Server Settings:" << myendl;
    
    (*this) << "   -Archiving[";
    if (ServerConfig::shared().enabled(ServerSwitch::BACKUP))
        (*this) << "Enabled]. (" << ServerConfig::shared().directoryFor(dirlocation_t::BACKUP).string() << ")" << myendl;
    else
        (*this) << "Disabled]" << myendl;
    
    (*this) << "   -Weapons & Armour Rank System: ";
    (*this) << activeMap[ServerConfig::shared().enabled(ServerSwitch::RANKSYSTEM)] << myendl;
    
    (*this) << "   -Vendors buy by item name: ";
    (*this) << activeMap[ServerConfig::shared().enabled(ServerSwitch::SELLBYNAME)] << myendl;
    
    (*this) << "   -Adv. Trade System: ";
    (*this) << activeMap[ServerConfig::shared().enabled(ServerSwitch::TRADESYSTEM)] << myendl;
    
    (*this) << "   -Races: " << static_cast<std::uint32_t>(Races->Count()) << myendl;
    (*this) << "   -Guilds: " << static_cast<std::uint32_t>(GuildSys->NumGuilds()) << myendl;
    (*this) << "   -Char count: " << ObjectFactory::shared().CountOfObjects(CBaseObject::OT_CHAR) << myendl;
    (*this) << "   -Item count: " << ObjectFactory::shared().CountOfObjects(CBaseObject::OT_ITEM) << myendl;
    (*this) << "   -Num Accounts: " << static_cast<std::uint32_t>(Account::shared().size()) << myendl;
    (*this) << "   Directories: " << myendl;
    (*this) << "   -Shared:          " << ServerConfig::shared().directoryFor(dirlocation_t::SAVE).string() << myendl;
    (*this) << "   -Archive:         " << ServerConfig::shared().directoryFor(dirlocation_t::BACKUP).string() << myendl;
    (*this) << "   -Data:            " << ServerConfig::shared().directoryFor(dirlocation_t::UODIR).string() << myendl;
    (*this) << "   -Defs:            " << ServerConfig::shared().directoryFor(dirlocation_t::DEFINITION).string() << myendl;
    (*this) << "   -Scripts:         " << ServerConfig::shared().directoryFor(dirlocation_t::SCRIPT).string() << myendl;
    (*this) << "   -ScriptData:      " << ServerConfig::shared().directoryFor(dirlocation_t::SCRIPTDATA).string() << myendl;
    (*this) << "   -HTML:            " << ServerConfig::shared().directoryFor(dirlocation_t::HTML).string() << myendl;
    (*this) << "   -Books:           " << ServerConfig::shared().directoryFor(dirlocation_t::BOOK).string() << myendl;
    (*this) << "   -MessageBoards:   " << ServerConfig::shared().directoryFor(dirlocation_t::MSGBOARD).string() << myendl;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	void registerKey()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Registers key input for detection in console
// o------------------------------------------------------------------------------------------------o
auto Console::registerKey(std::int32_t key, std::string cmdName, std::uint16_t scriptId) -> void {
#if defined(UOX_DEBUG_MODE)
    messageLoop << util::format("         Registering key \"%c\"", key);
#endif
    jskeyHandler[key] = JSConsoleEntry(scriptId, cmdName);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Console::setKeyStatus()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Enabling/Disabling specific key input in console
// o------------------------------------------------------------------------------------------------o
auto Console::setKeyStatus(std::int32_t key, bool isEnabled) -> void {
    auto toFind = jskeyHandler.find(key);
    if (toFind != jskeyHandler.end()) {
        toFind->second.isEnabled = isEnabled;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Console::registerFunc()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Registers console function
// o------------------------------------------------------------------------------------------------o
auto Console::registerFunc(const std::string &cmdFunc, const std::string &cmdName, std::uint16_t scriptId) -> void {
#if defined(UOX_DEBUG_MODE)
    print(util::format("         Registering console func \"%s\"\n", cmdFunc.c_str()));
#endif
    jsconsoleFunctions[util::upper(cmdFunc)] = JSConsoleEntry(scriptId, cmdName);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Console::setFuncStatus()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Enables/disables console function
// o------------------------------------------------------------------------------------------------o
auto Console::setFuncStatus(const std::string &cmdFunc, bool isEnabled) -> void {
    auto upper = util::upper(cmdFunc);
    auto toFind = jsconsoleFunctions.find(upper);
    if (toFind != jsconsoleFunctions.end()) {
        toFind->second.isEnabled = isEnabled;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	Console::registration()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Registers console script
// o------------------------------------------------------------------------------------------------o
auto Console::registration() -> void {
    auto spellSection = JSMapping->GetSection(CJSMappingSection::SCPT_CONSOLE);
    for (const auto &[spellname, ourScript] : spellSection->collection()) {
        if (ourScript) {
            ourScript->ScriptRegistration("Console");
        }
    }
}
