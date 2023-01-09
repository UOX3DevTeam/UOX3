//o------------------------------------------------------------------------------------------------o
//| cConsole.cpp
//o------------------------------------------------------------------------------------------------o
//| In essence, CConsole is our nice generic class for outputting display.
//| As long as the interface is kept the same, we can display whatever we wish
//| too. We store a set of coordinates being the window corner and size, for
//| systems with windowing support, which describes the window we are in.
//o------------------------------------------------------------------------------------------------o
//| Version History
//| 1.7		Added comments (29. October 2001)
//| 2.0		Added filter masking (22nd January 2002)
//o------------------------------------------------------------------------------------------------o

// Header
#include "uox3.h"
#include "cConsole.h"
#include "cThreadQueue.h"
#include "ObjectFactory.h"
#include "network.h"
#include "ssection.h"
#include "CJSMapping.h"
#include "teffect.h"
#include "cMagic.h"
#include "magic.h"
#include "regions.h"
#include "commands.h"
#include "cServerDefinitions.h"
#include "cHTMLSystem.h"
#include "cRaces.h"
#include "cGuild.h"
#include "cScript.h"
#include "StringUtility.hpp"
#include <iostream>
#include <cctype>
#include <stdexcept>

#if PLATFORM != WINDOWS
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
struct termios initial_terminal_state;
#else

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <conio.h>
#undef min
#undef max
DWORD initial_terminal_state;
#endif
using namespace std::string_literals;
CEndL		myendl;

constexpr auto NORMALMODE = std::uint8_t( 0 );
constexpr auto WARNINGMODE = std::uint8_t( 1 );
constexpr auto ERRORMODE = std::uint8_t( 2 );
//const UI08 COLOURMODE			= 3;
const std::string	CConsole::ESC			= "\x1b"s;
const std::string	CConsole::CSI			= CConsole::ESC + "["s;
const std::string	CConsole::BEL			= "\x07"s;
//  Command sequences
const std::string	CConsole::ATTRIBUTE		= CConsole::CSI +"#m"s;  // find/replace on # with attribute number
const std::string	CConsole::MOVE			= CConsole::CSI + "ROW;COLH"s; // we are going to find/replace on ROW/COL
const std::string	CConsole::HORIZMOVE		= CConsole::CSI + "COLG"s;
const std::string	CConsole::VERTMOVE		= CConsole::CSI + "ROWd"s;
const std::string	CConsole::OFFCURSOR		= CConsole::CSI + "?25l"s;
const std::string	CConsole::ONCURSOR		= CConsole::CSI + "?25h"s;

const std::string	CConsole::ONGRAPHIC		= CConsole::ESC + "(0"s;
const std::string	CConsole::OFFGRAPHIC	= CConsole::ESC + "(B"s;

const std::string	CConsole::SETTITLE		= CConsole::ESC + "]2;TITLEs"s + CConsole::BEL;

const std::string	CConsole::CLEAR			= CConsole::CSI +"2J"s;

// Forward function declarations
auto EndMessage( SI32 x ) -> void;
auto LoadCustomTitle() -> void;
auto LoadSkills() -> void;
auto LoadSpawnRegions() -> void;
auto LoadRegions() -> void;
auto LoadTeleportLocations() -> void;
auto LoadCreatures() -> void;
auto LoadPlaces() -> void;
auto UnloadRegions() -> void;
auto UnloadSpawnRegions() -> void;
auto LoadTeleportLocations() -> void;

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Class Constructor and deconstructor
//o------------------------------------------------------------------------------------------------o
CConsole::CConsole() : width( 80 ), height( 25 ),
currentMode( NORMALMODE ), previousColour( CNORMAL ), logEcho( false )
{
	is_initialized = false;
}

CConsole::~CConsole()
{
	if( is_initialized )
	{
		MoveTo( 1, height );
		Reset();
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole::Initialize()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Prepare console/terminal for UOX3 output
//o------------------------------------------------------------------------------------------------o
auto CConsole::Initialize() -> void
{
	is_initialized = true;
#if !defined(_WIN32)
	tcgetattr( 1, &initial_terminal_state ); // get the current state of the terminal
	auto temp = initial_terminal_state;
	temp.c_lflag = temp.c_lflag & ( ~ECHO ) & ( ~ICANON ); // Disable echo and canonical (line) mode
	temp.c_cc[VMIN] = 0;  		// in non canonical mode we non blocking read
	temp.c_cc[VTIME] = 0; // in non canonical mode we non blocking read
	tcsetattr( 1, TCSANOW, &temp );
#else
	// Set the input to non echo and non canonical (line) mode
	auto hco = GetStdHandle( STD_OUTPUT_HANDLE );
	GetConsoleMode( hco, &initial_terminal_state );
	auto temp = initial_terminal_state;
	temp = ( temp & ( ~ENABLE_ECHO_INPUT ) & ( ~ENABLE_LINE_INPUT ) & ( ~ENABLE_MOUSE_INPUT ));
	SetConsoleMode( hco, initial_terminal_state );
#endif
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole()::Reset()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Reset console/terminal to its original state
//o------------------------------------------------------------------------------------------------o
auto CConsole::Reset() -> void
{
	std::cout.flush();
#if !defined( _WIN32 )
	tcsetattr( 1, TCSAFLUSH, &initial_terminal_state );
	std::cout << std::endl;
#else
	auto hco = GetStdHandle( STD_OUTPUT_HANDLE );
	SetConsoleMode( hco, initial_terminal_state );
#endif
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole::SetTitle()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Set console/terminal window title
//o------------------------------------------------------------------------------------------------o
auto CConsole::SetTitle( const std::string &value ) -> void
{
#if !defined( _WIN32 )
	auto cmd = SETTITLE;
	auto loc = cmd.find( "TITLE" );
	cmd.replace( loc, 5, value );
	SendCMD( cmd );
#else
	SetConsoleTitle( value.c_str() );
#endif
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole::SendCMD()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Send cmd to terminal
//o------------------------------------------------------------------------------------------------o
auto CConsole::SendCMD( const std::string& cmd ) ->CConsole&
{
	std::cout << cmd;
	std::cout.flush();
	return *this;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole::WindowSize()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Set window size in columns and rows
//o------------------------------------------------------------------------------------------------o
auto CConsole::WindowSize() -> std::pair<int, int>
{
	int row =0;
	int col =0;

#if !defined( _WIN32 )
	// Get the window size
	winsize winsz;
	ioctl( 0, TIOCGWINSZ, &winsz );
	col = winsz.ws_col;
	row = winsz.ws_row;
#else
	HANDLE hco = GetStdHandle( STD_OUTPUT_HANDLE );
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo( hco, &csbi );
	col	= csbi.dwSize.X;
	row	= csbi.dwSize.Y;
#endif
	return std::make_pair( row, col );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole()::DoClearScreen()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Clears the screen
//o------------------------------------------------------------------------------------------------o
auto CConsole::DoClearScreen() -> void
{
#if defined( _WIN32 )
	unsigned long y;
	COORD xy;

	xy.X = 0;
	xy.Y = 0;
	auto hco = GetStdHandle( STD_OUTPUT_HANDLE );
	FillConsoleOutputCharacter( hco, ' ', width * height, xy, &y );
	SetConsoleCursorPosition( hco, xy );
#else
	std::string cmd = CSI;
	cmd = cmd + std::string( "2J" );
	SendCMD( cmd );
#endif
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	<< Overrriding
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Let you use << and >>
//o------------------------------------------------------------------------------------------------o
auto CConsole::operator << ( const char *output ) ->CConsole&
{
	StartOfLineCheck();
#if PLATFORM == WINDOWS
	CONSOLE_SCREEN_BUFFER_INFO ScrBuffInfo;
	auto hco = GetStdHandle( STD_OUTPUT_HANDLE );
	GetConsoleScreenBufferInfo( hco, &ScrBuffInfo );
	std::string toDisplay = output;
	if( ScrBuffInfo.dwCursorPosition.X + toDisplay.length() > ScrBuffInfo.dwSize.X )
	{
		bool wrapDone = false;
		while( !wrapDone )
		{
			GetConsoleScreenBufferInfo( hco, &ScrBuffInfo );
			SI32 diff = ScrBuffInfo.dwSize.X - ScrBuffInfo.dwCursorPosition.X - 1;
			if( diff > toDisplay.length() )
			{
				std::cout << toDisplay;
				wrapDone = true;
			}
			else
			{
				std::cout << toDisplay.substr( 0, diff );
				( *this ) << myendl;
				StartOfLineCheck();
				toDisplay = toDisplay.substr( diff );
			}
		}
	}
	else
	{
		std::cout << output;
	}
#else
	std::cout << output;
#endif
	return ( *this );
}
//================================================================================================
auto CConsole::operator << ( const std::uint8_t *output ) -> CConsole&
{
	StartOfLineCheck();
	std::cout << reinterpret_cast<const char*>( output );
	return ( *this );
}
//================================================================================================
auto CConsole::operator << ( const std::int8_t *output ) -> CConsole&
{
	StartOfLineCheck();
	std::cout << reinterpret_cast<const char*>( output );
	return ( *this );
}
//================================================================================================
auto CConsole::operator << ( const std::uint8_t &output ) -> CConsole&
{
	StartOfLineCheck();
	std::cout << static_cast<UI16>( output );
	return ( *this );
}
//================================================================================================
auto CConsole::operator << ( const std::int8_t &output ) -> CConsole&
{
	StartOfLineCheck();
	std::cout << static_cast<std::int16_t>( output );
	return ( *this );
}
//================================================================================================
auto CConsole::operator << ( const UI16 &output ) -> CConsole&
{
	StartOfLineCheck();
	std::cout << output;
	return ( *this );
}
//================================================================================================
auto CConsole::operator << ( const std::int16_t &output ) -> CConsole&
{
	StartOfLineCheck();
	std::cout << output;
	return ( *this );
}
//================================================================================================
auto CConsole::operator << ( const std::uint32_t &output ) -> CConsole&
{
	StartOfLineCheck();
	std::cout << output;
	return ( *this );
}
//================================================================================================
auto CConsole::operator << ( const std::int32_t &output ) -> CConsole&
{
	StartOfLineCheck();
	std::cout << output;
	return ( *this );
}

//================================================================================================
auto CConsole::operator << ( const std::uint64_t &output ) -> CConsole&
{
	StartOfLineCheck();
	std::cout << output;
	return ( *this );
}
//================================================================================================
auto CConsole::operator << ( const std::int64_t &output ) -> CConsole&
{
	StartOfLineCheck();
	std::cout << output;
	return ( *this );
}

//================================================================================================
auto CConsole::operator << ( const float &output ) -> CConsole&
{
	StartOfLineCheck();
	std::cout << output;
	return ( *this );

}
//================================================================================================
auto CConsole::operator << ( const double &output ) -> CConsole&
{
	StartOfLineCheck();
	std::cout << output;
	return ( *this );
}
//================================================================================================
auto CConsole::operator << ( const long double &output ) -> CConsole&
{
	StartOfLineCheck();
	std::cout << output;
	return ( *this );
}

//================================================================================================
auto CConsole::operator << ( CBaseObject *output ) -> CConsole&
{
	StartOfLineCheck();
	(*this) << output->GetSerial();
	return ( *this );
}
//================================================================================================
auto CConsole::operator << ( const CBaseObject *output ) -> CConsole&
{
	StartOfLineCheck();
	(*this) << output->GetSerial();
	return ( *this );
}
//================================================================================================
auto CConsole::operator << ( const CBaseObject &output ) -> CConsole&
{
	StartOfLineCheck();
	(*this) << output.GetSerial();
	return ( *this );
}

//================================================================================================
auto CConsole::operator << ( const std::string &output ) -> CConsole&
{
	StartOfLineCheck();
	std::cout << output;
	return ( *this );
}

//================================================================================================
auto CConsole::operator << ( [[maybe_unused]] CEndL& myObj ) -> CConsole&
{
	if( curLeft == 0 )
	{
		PrintStartOfLine();
	}
	std::cout << std::endl;
	curLeft = 0;
	++curTop;
	return ( *this );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CConsole::Print()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Prints the console with the "|"
//o------------------------------------------------------------------------------------------------o
auto CConsole::Print( const std::string& msg ) -> void
{
	StartOfLineCheck();
	std::cout << msg;
	if(( msg.size() > 0 ) && ( msg[msg.size() - 1] == '\n' ))
	{
		curLeft = 0;
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CConsole::Log()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Writes to the logfile
//o------------------------------------------------------------------------------------------------o
auto CConsole::Log( const std::string& msg, const std::string& filename ) -> void
{
	if( cwmWorldState )
	{
		if( cwmWorldState->ServerData()->ServerConsoleLog() )
		{
			std::ofstream toWrite;
			std::string realFileName;	// 022602: in windows a path can be max 512 chars, this at 128 coud potentially cause crashes if the path is longer than 128 chars
			if( cwmWorldState != nullptr )
			{
				realFileName = cwmWorldState->ServerData()->Directory( CSDDP_LOGS ) + filename;
			}
			else
			{
				realFileName = filename;
			}

			char timeStr[256];
			RealTime( timeStr );

			toWrite.open( realFileName.c_str(), std::ios::out | std::ios::app );
			if( toWrite.is_open() )
			{
				toWrite << "[" << timeStr << "] " << msg << std::endl;
			}
			toWrite.close();
			if( LogEcho() )
			{
				Print( oldstrutil::format( "%s%s\n", timeStr, msg.c_str() ));
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CConsole::Log()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Log to the console.log file
//o------------------------------------------------------------------------------------------------o
auto CConsole::Log( const std::string& msg ) -> void
{
	if( cwmWorldState->ServerData()->ServerConsoleLog() )
	{
		Log( msg, "console.log" );
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CConsole::Error()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Writes to the error log and the console.
//o------------------------------------------------------------------------------------------------o
auto CConsole::Error(const std::string& msg) -> void
{
	try
	{
		UI08 oldMode = CurrentMode();
		CurrentMode( ERRORMODE );
		Log( msg, "error.log" );
		if( curLeft != 0 )
		{
			( *this ) << myendl;
		}
		TurnRed();
		( *this ) << "ERROR: " << ( const char* ) & msg[0] << myendl;
		TurnNormal();
		CurrentMode( oldMode );
	}
	catch( const std::exception &e )
	{
		std::cerr << "Error print reporting 'error'.  Error was: " << msg << std::endl;
		exit( 1 );
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CConsole::PrintSectionBegin()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Prints a section seperator
//o------------------------------------------------------------------------------------------------o
auto CConsole::PrintSectionBegin() -> void
{
	TurnBrightWhite();
	std::cout << "o";
	for( auto j = 1; j < width - 1; ++j )
	{
		std::cout << "-";
	}
	std::cout << "o";
	curLeft = 0;
	curTop = 0;
	TurnNormal();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CConsole::Start()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Migration of constart function to here
//o------------------------------------------------------------------------------------------------o
auto CConsole::Start(const std::string& temp) -> void
{
	auto size = WindowSize();
	width = std::get<1>( size );
	height = std::get<0>( size );
	SetTitle( temp );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CConsole::TurnYellow()
///o------------------------------------------------------------------------------------------------o
//| Purpose		-	Turns the text yellow
//o------------------------------------------------------------------------------------------------o
auto CConsole::TurnYellow() -> void
{
#if PLATFORM == WINDOWS
	auto hco = GetStdHandle( STD_OUTPUT_HANDLE );
	SetConsoleTextAttribute( hco, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY );
#else
	std::cout << "\033[1;33m";
#endif
	previousColour = CYELLOW;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CConsole::TurnRed()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Turns the text red
//o------------------------------------------------------------------------------------------------o
auto CConsole::TurnRed() -> void
{
#if PLATFORM == WINDOWS
	auto hco = GetStdHandle( STD_OUTPUT_HANDLE );
	SetConsoleTextAttribute( hco, FOREGROUND_RED | FOREGROUND_INTENSITY );
#else
	std::cout << "\033[1;31m";
#endif
	previousColour = CRED;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CConsole::TurnGreen()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Turns the text green
//o------------------------------------------------------------------------------------------------o
auto CConsole::TurnGreen() -> void
{
#if PLATFORM == WINDOWS
	auto hco = GetStdHandle( STD_OUTPUT_HANDLE );
	SetConsoleTextAttribute( hco, FOREGROUND_GREEN | FOREGROUND_INTENSITY );
#else
	std::cout << "\033[1;32m";
#endif
	previousColour = CGREEN;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CConsole::TurnBlue()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Turns the text blue
//o------------------------------------------------------------------------------------------------o
auto CConsole::TurnBlue() -> void
{
#if PLATFORM == WINDOWS
	auto hco = GetStdHandle( STD_OUTPUT_HANDLE );
	SetConsoleTextAttribute( hco, FOREGROUND_BLUE | FOREGROUND_INTENSITY );
#else
	std::cout << "\033[1;34m";
#endif
	previousColour = CBLUE;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CConsole::TurnNormal()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns text to normal white
//o------------------------------------------------------------------------------------------------o
auto CConsole::TurnNormal() -> void
{
#if PLATFORM == WINDOWS
	auto hco = GetStdHandle( STD_OUTPUT_HANDLE );
	SetConsoleTextAttribute( hco, FOREGROUND_BLUE  | FOREGROUND_RED | FOREGROUND_GREEN );
#else
	std::cout << "\033[0;37m";
#endif
	previousColour = CNORMAL;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CConsole::TurnBrightWhite()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns text to bright white
//o------------------------------------------------------------------------------------------------o
auto CConsole::TurnBrightWhite() -> void
{
#if PLATFORM == WINDOWS
	auto hco = GetStdHandle( STD_OUTPUT_HANDLE );
	SetConsoleTextAttribute( hco, FOREGROUND_BLUE  | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY );
#else
	std::cout << "\033[1;37m";
#endif
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole::PrintDone()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Prints colored "[done]" message
//o------------------------------------------------------------------------------------------------o
auto CConsole::PrintDone() -> void
{
	PrintSpecial( CGREEN, "done" );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole::PrintFailed()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Prints colored "[failed]" message
//o------------------------------------------------------------------------------------------------o
auto CConsole::PrintFailed() -> void
{
	PrintSpecial( CRED, "Failed" );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole::PrintPassed()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Prints colored "[skipped]" message
//o------------------------------------------------------------------------------------------------o
auto CConsole::PrintPassed() -> void
{
	PrintSpecial( CYELLOW, "Skipped" );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CConsole::ClearScreen()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Clears the screen
//o------------------------------------------------------------------------------------------------o
auto CConsole::ClearScreen() -> void
{
	DoClearScreen();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CConsole::PrintBasedOnVal()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Prints "done" or "failed" messages based on boolean passed in
//o------------------------------------------------------------------------------------------------o
auto CConsole::PrintBasedOnVal( bool value ) -> void
{
	if( value )
	{
		PrintDone();
	}
	else
	{
		PrintFailed();
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole::Warning()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Writes to the warning log and the console.
//o------------------------------------------------------------------------------------------------o
auto CConsole::Warning( const std::string& msg ) -> void
{
	UI08 oldMode = CurrentMode();
	CurrentMode( WARNINGMODE );
	Log( msg, "warning.log" );
	if( curLeft != 0 )
	{
		( *this ) << myendl;
	}
	TurnBlue();
	( *this ) << "WARNING: " << ( const char* ) & msg[0] << myendl;
	TurnNormal();
	CurrentMode( oldMode );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Console::CurrentMode()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets console's output mode
//o------------------------------------------------------------------------------------------------o
auto CConsole::CurrentMode() const -> std::uint8_t
{
	return currentMode;
}
auto CConsole::CurrentMode( std::uint8_t value ) -> void
{
	currentMode = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole::PrintStartOfLine()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Set colour of console text at start of a new line
//o------------------------------------------------------------------------------------------------o
auto CConsole::PrintStartOfLine() -> void
{
	TurnBrightWhite();
	std::cout << "| ";
	switch( previousColour )
	{
		case CBLUE:		TurnBlue();		break;
		case CRED:		TurnRed();		break;
		case CGREEN:	TurnGreen();	break;
		case CYELLOW:	TurnYellow();	break;
		case CBWHITE:					break;	// current colour
		case CNORMAL:
		default:		TurnNormal();	break;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole::MoveTo( SI32 x, SI32 y )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Set console cursor position
//o------------------------------------------------------------------------------------------------o
#if PLATFORM == WINDOWS
auto CConsole::MoveTo( SI32 x, SI32 y ) -> void
{
	auto hco = GetStdHandle( STD_OUTPUT_HANDLE );
	COORD Pos;
	if( y == -1 )
	{
		CONSOLE_SCREEN_BUFFER_INFO ScrBuffInfo;

		GetConsoleScreenBufferInfo( hco, &ScrBuffInfo );
		Pos.Y = ScrBuffInfo.dwCursorPosition.Y;
		Pos.X = static_cast<UI16>( x );
		SetConsoleCursorPosition( hco, Pos );
	}
	else
	{
		Pos.X = static_cast<UI16>( x );
		Pos.Y = static_cast<UI16>( y );
		SetConsoleCursorPosition( hco, Pos );
	}
}
#else
void CConsole::MoveTo( SI32 x, [[maybe_unused]] SI32 y )
{
	std::cout << "\033[255D";
	std::cout << "\033[" << x << "C";
}
#endif

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole::LogEcho()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks whether console should echo log messages
//o------------------------------------------------------------------------------------------------o
auto CConsole::LogEcho() const -> bool
{
	return logEcho;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole::LogEcho()
//|	Date		-	2/03/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Enables/Disables whether console should echo log messages
//o------------------------------------------------------------------------------------------------o
auto CConsole::LogEcho( bool value ) -> void
{
	logEcho = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole::StartOfLineCheck()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Reduces excess work in overloaded functions by checking
//|					to see if the start of line has to be done here
//o------------------------------------------------------------------------------------------------o
auto CConsole::StartOfLineCheck() -> void
{
	if( curLeft == 0 )
	{
		PrintStartOfLine();
		curLeft = 1;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole::PrintSpecial()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	This is much like PrintFailed, PrintDone and so on except
//|					we specify the text and the colour
//o------------------------------------------------------------------------------------------------o
auto CConsole::PrintSpecial( UI08 colour, const std::string& msg ) -> void
{
	StartOfLineCheck();
	size_t stringLength = msg.size() + 3;
	MoveTo( static_cast<SI32>( width - stringLength ));
	TurnNormal();
	( *this ) << "[";
	switch( colour )
	{
		default:
		case CNORMAL:						break;
		case CBLUE:		TurnBlue();			break;
		case CRED:		TurnRed();			break;
		case CGREEN:	TurnGreen();		break;
		case CYELLOW:	TurnYellow();		break;
		case CBWHITE:	TurnBrightWhite();	break;
	}
	( *this ) << msg;
	TurnNormal();
	( *this ) << "]" << myendl;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole::cl_getch()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Read a character from stdin, in a cluox compatble way.
//|					This routine is non-blocking!
//|	Returns		-	>0 -> character read
//|					-1 -> no char available.
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	now cluox is GUI wrapper over uox using stdin and stdout redirection to capture
//|					the console, if it is active uox can't use kbhit() to determine if there is a
//|					character aviable, it can only get one directly by getch().
//|					However the problem arises that uox will get blocked if none is aviable.
//|					The solution to this problem is that cluox also hands over the second pipe-end
//|					of stdin so uox can write itself into this stream. To determine if a character is
//|					now done that way. UOX write's itself a ZERO on the other end of the pipe, and reads
//|					a charecter, if it is again the same ZERO just putted in nothing was entered. However
//|					it is not a zero the user has entered a char.
//o------------------------------------------------------------------------------------------------o
auto CConsole::cl_getch() -> std::int32_t
{
#if PLATFORM != WINDOWS
	char data = 0;
	auto a = ::read( 0, &data, 1); // This doesn't block on getting a line due to initalization
	if( a > 0 )
	{
		// Look for escape!
		if( data == 27 )
		{
			// This was an escape, see if we have another item
			a = ::read( 0, &data, 1 );
			if( a > 0 )
			{
				// See if this is a CSI
				if( data == '[' )
				{
					// It is an control sequence!
					// Some sequences end with a ~, some dont.  So, we now get to see
					a = ::read( 0, &data, 1 );
					while( a > 0 )
					{
						if(( data == 'A' ) || ( data == 'B' ) || ( data == 'C' ) || ( data == 'D' ) || (( data == 'H' ) && ( data == 'F' )) || ( data == '~' ))
						{
							break;
						}
						a = ::read( 0, &data, 1 );
					}
					return -1;
				}
				else if( data == 'O' ) // this could be F1-F4
				{
					a = ::read( 0, &data, 1 );
					if( a > 0 )
					{
						if(( data == 'P' ) || ( data == 'Q' ) || ( data == 'R' ) || ( data == 'S' ))
						{
							// It was F1,F2,F3,F4
							return -1;
						}
						// Soooooo, we have now lost two characters, this one and the preceeding O that where after the ESC
						return 27;
					}
					// It wasnt, so we have a choice, return -1 (since we got ESC O, or return ESC and lose O
					return 27;
				}
				else
				{
					// Ok, it isn't an CSI (Control Sequence).  But we have all ready lost/read the next
					// character.  That will be lost, return escape
					return 27;
				}
			}
			else
			{
				// There was nothing after the escape, so just send the escape
				return 27; // return escape
			}
		}
		// Not escape if here, so just return it
		return static_cast<SI32>( data );
	}
	else
	{
		return -1;
	}

#else
	// uox is not wrapped simply use the kbhit routine
	if( _kbhit() )
	{
		return _getch();
	}
	else
	{
		return -1;
	}
#endif
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CConsole::Poll()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Facilitate console control. SysOp keys, and localhost
//|					controls.
//o------------------------------------------------------------------------------------------------o
auto CConsole::Poll() -> void
{
	SI32 c = cl_getch();
	if( c > 0 )
	{
		c = std::toupper( c );
		Process( c );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole::Process()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle keypresses in console
//|	Changes		-	05042004 - added some console debugging stuff.
//|					Ideally this will all be placed onto a DEBUG menu which
//|					a user will be able to select using the <, and > keys
//|					respectivly. This functionality hasn't been implemented
//|					at the current time of writing, but will be when possible.
//o------------------------------------------------------------------------------------------------o
auto CConsole::Process(std::int32_t c) -> void
{
	if( c == '*' )
	{
		if( cwmWorldState->GetSecure() )
		{
			messageLoop << "Secure mode disabled. Press ? for a commands list";
		}
		else
		{
			messageLoop << "Secure mode re-enabled";
		}
		cwmWorldState->SetSecure( !cwmWorldState->GetSecure() );
		return;
	}
	else
	{
		if( cwmWorldState->GetSecure() )
		{
			messageLoop << "Secure mode prevents keyboard commands! Press '*' to disable";
			return;
		}

		JSCONSOLEKEYMAP_ITERATOR toFind = JSKeyHandler.find( c );
		if( toFind != JSKeyHandler.end() )
		{
			if( toFind->second.isEnabled )
			{
				cScript *toExecute = JSMapping->GetScript( toFind->second.scriptId );
				if( toExecute )
				{
					// All commands that execute are of the form: command_commandname (to avoid possible clashes)
#if defined( UOX_DEBUG_MODE )
					Print( oldstrutil::format( "Executing JS keystroke %c %s\n", c, toFind->second.cmdName.c_str() ));
#endif
					jsval eventRetVal;
					toExecute->CallParticularEvent( toFind->second.cmdName.c_str(), nullptr, 0, &eventRetVal );
				}
				return;
			}
		}
		CSocket *tSock	= nullptr;
		//char outputline[128], temp[1024];
		std::string outputline, temp;
		SI32 indexcount	= 0;
		bool kill		= false;
		SI32 j = 0;
		switch( c )
		{
			case '!':
				// Force server to save accounts file
				messageLoop << "CMD: Saving Accounts... ";
				Accounts->Save();
				messageLoop << MSG_PRINTDONE;
				break;
			case '@':
				// Force server to save all files.(Manual save)
				messageLoop << MSG_WORLDSAVE;
				break;
			case 'Y':
				SI32 keyresp;
				std::cout << "System: ";
				std::cout.flush();
				while( !kill )
				{
					keyresp = cl_getch();
					switch( keyresp )
					{
						case -1:	// no key pressed
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
							if( indexcount < 0 )
							{
								indexcount = 0;
							}
							else
							{
								std::cout << "\b \b";
							}
							break;
						case 0x0A:
						case 0x0D:
							outputline[indexcount] = 0;
							messageLoop.NewMessage( MSG_CONSOLEBCAST, outputline.c_str() );
							indexcount = 0;
							kill = true;
							std::cout << std::endl;
							temp = oldstrutil::format( "CMD: System broadcast sent message \"%s\"", outputline.c_str() );
							outputline = "";
							messageLoop << temp;
							break;
						default:
							outputline = outputline + std::string( 1, static_cast<SI08>( keyresp ));
							indexcount = indexcount + 1;
							std::cout << static_cast<SI08>( keyresp );
							std::cout.flush();
							break;
					}
					keyresp = 0x00;
				}
				break;
			case '[':
			{
				// We want to group all the contents of the multimap container numerically by group. We rely on the self ordering in the multimap implementation to do this.
				messageLoop << "  ";
				messageLoop << "Auto-AddMenu Statistics";
				messageLoop << "  ";
				std::string szBuffer;
				// We need to get an iteration into the map first of all the top level ULONGs then we can get an equal range.
				std::map<UI32, UI08> localMap;
				localMap.clear();
				for( ADDMENUMAP_CITERATOR CJ = g_mmapAddMenuMap.begin(); CJ != g_mmapAddMenuMap.end(); CJ++ )
				{
					// check to see if the group id has been checked already
					if( localMap.find( CJ->first ) == localMap.end() )
					{
						localMap.insert( std::make_pair( CJ->first, 0 ));
						szBuffer = "";
						szBuffer = oldstrutil::format( "AddMenuGroup %u:", CJ->first );
						messageLoop << szBuffer;
						std::pair< ADDMENUMAP_CITERATOR, ADDMENUMAP_CITERATOR > pairRange = g_mmapAddMenuMap.equal_range( CJ->first );
						SI32 count = 0;
						for( ADDMENUMAP_CITERATOR CI=pairRange.first;CI != pairRange.second; CI++ )
						{
							count++;
						}
						szBuffer = "";
						szBuffer = oldstrutil::format( "   Found %i Auto-AddMenu Item(s).", count );
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
				if( !cwmWorldState->GetReloadingScripts() )
				{
					cwmWorldState->SetReloadingScripts( true );
					// Reload all the files. If there are issues with these files change the order reloaded from here first.
					cwmWorldState->ServerData()->Load();
					messageLoop << "CMD: Loading All";
					messageLoop << "     Server INI... ";
					// Reload accounts, and update Access.adm if new accounts available.
					messageLoop << "     Loading Accounts... ";
					Accounts->Load();
					messageLoop << MSG_PRINTDONE;
					// Reload Region Files
					messageLoop << "     Loading Regions... ";
					UnloadRegions();
					LoadRegions();
					messageLoop << MSG_PRINTDONE;
					messageLoop << "     Loading Teleport Locations... ";
					LoadTeleportLocations();
					messageLoop << MSG_PRINTDONE;
					// Reload the serve spawn regions
					messageLoop << "     Loading Spawn Regions... ";
					UnloadSpawnRegions();
					LoadSpawnRegions();
					messageLoop << MSG_PRINTDONE;
					// Reload the server command list
					messageLoop << "     Loading commands... ";
					Commands->Load();
					messageLoop << MSG_PRINTDONE;
					// Reload DFN's
					messageLoop << "     Loading Server DFN... ";
					FileLookup->Reload();
					messageLoop << MSG_PRINTDONE;
					// messageLoop access is REQUIRED, as this function is executing in a different thread, so we need thread safety
					messageLoop << "     Loading JSE Scripts... ";

					// Reload the current Spells
					messageLoop << "     Loading spells... ";
					Magic->LoadScript();
					messageLoop << MSG_PRINTDONE;
					// Reload the HTML output templates
					messageLoop << "     Loading HTML Templates... ";
					HTMLTemplates->Unload();
					HTMLTemplates->Load();
					cwmWorldState->SetReloadingScripts( false );
					messageLoop << MSG_PRINTDONE;
				}
				else
				{
					messageLoop << "Server can only load one script at a time";
				}
				break;
			case 'T':
				// Timed shut down(10 minutes)
				messageLoop << "CMD: 10 Minute Server Shutdown Announced(Timed)";
				cwmWorldState->SetEndTime( BuildTimeValue( 600 ));
				EndMessage( 0 );
				break;
			case  'D':
				// Disconnect account 0 (useful when client crashes)
				for( tSock = Network->LastSocket(); tSock != nullptr; tSock = Network->PrevSocket() )
				{
					if( tSock->AcctNo() == 0 )
					{
						Network->Disconnect( tSock );
					}
				}
				messageLoop << "CMD: Socket Disconnected(Account 0).";
				break;
			case 'K':
			{
				for( auto &tSock : Network->connClients )
				{
					Network->Disconnect( tSock );
				}
				messageLoop << "CMD: All Connections Closed.";
			}
				break;
			case 'P':
			{
				UI32 networkTimeCount	= cwmWorldState->ServerProfile()->NetworkTimeCount();
				UI32 timerTimeCount		= cwmWorldState->ServerProfile()->TimerTimeCount();
				UI32 autoTimeCount		= cwmWorldState->ServerProfile()->AutoTimeCount();
				UI32 loopTimeCount		= cwmWorldState->ServerProfile()->LoopTimeCount();
				// 1/13/2003 - Dreoth - Log Performance Information enhancements
				LogEcho( true );
				Log( "--- Starting Performance Dump ---", "performance.log");
				Log( "\tPerformance Dump:", "performance.log");
				Log( oldstrutil::format( "\tNetwork code: %.2fmsec [%i samples]", static_cast<R32>( static_cast<R32>( cwmWorldState->ServerProfile()->NetworkTime() ) / static_cast<R32>( networkTimeCount )), networkTimeCount ), "performance.log" );
				Log( oldstrutil::format( "\tTimer code: %.2fmsec [%i samples]", static_cast<R32>( static_cast<R32>( cwmWorldState->ServerProfile()->TimerTime() ) / static_cast<R32>( timerTimeCount )), timerTimeCount ), "performance.log" );
				Log( oldstrutil::format( "\tAuto code: %.2fmsec [%i samples]", static_cast<R32>( static_cast<R32>( cwmWorldState->ServerProfile()->AutoTime() ) / static_cast<R32>( autoTimeCount )), autoTimeCount ), "performance.log" );
				Log( oldstrutil::format( "\tLoop Time: %.2fmsec [%i samples]", static_cast<R32>( static_cast<R32>( cwmWorldState->ServerProfile()->LoopTime() ) / static_cast<R32>( loopTimeCount )), loopTimeCount ), "performance.log" );

				Log( oldstrutil::format( "\tCharacters: %i/%i - Items: %i/%i (Dynamic)", ObjectFactory::GetSingleton().CountOfObjects( OT_CHAR ), ObjectFactory::GetSingleton().SizeOfObjects( OT_CHAR ), ObjectFactory::GetSingleton().CountOfObjects( OT_ITEM ), ObjectFactory::GetSingleton().SizeOfObjects( OT_ITEM )), "performance.log" );
				Log( oldstrutil::format( "\tSimulation Cycles: %f per sec", (1000.0*(1.0/static_cast<R32>( static_cast<R32>( cwmWorldState->ServerProfile()->LoopTime() ) / static_cast<R32>( loopTimeCount ))))), "performance.log" );
				Log( oldstrutil::format( "\tBytes sent: %i", cwmWorldState->ServerProfile()->GlobalSent() ), "performance.log" );
				Log( oldstrutil::format( "\tBytes Received: %i", cwmWorldState->ServerProfile()->GlobalReceived() ), "performance.log" );
				Log( "--- Performance Dump Complete ---", "performance.log");
				LogEcho( false );
				break;
			}
			case 'W':
			{
				// Display logged in chars
				messageLoop << "CMD: Current Users in the World:";
				{
					for( auto &iSock : Network->connClients )
					{
						++j;
						CChar *mChar = iSock->CurrcharObj();
						
						temp = oldstrutil::format( "     %i) %s [%x %x %x %x]", j - 1, mChar->GetName().c_str(), mChar->GetSerial( 1 ), mChar->GetSerial( 2 ), mChar->GetSerial( 3 ), mChar->GetSerial( 4 ));
						messageLoop << temp;
					}
				}

				temp = oldstrutil::format( "     Total users online: %i", j );
				messageLoop << temp;
				break;
			}
			case 'M':
				UI32 tmp, total;
				total = 0;
				tmp = 0;
				messageLoop << "CMD: UOX Memory Information:";
				UI32 m, n;
				m = static_cast<std::uint32_t>( ObjectFactory::GetSingleton().SizeOfObjects( OT_CHAR ));
				total += tmp = m + m * sizeof( CTEffect ) + m * sizeof( SI08 ) + m * sizeof( intptr_t ) * 5;
				temp = oldstrutil::format( "     Characters: %u bytes [%u chars ( %u allocated )]", tmp, ObjectFactory::GetSingleton().CountOfObjects( OT_CHAR ), m );
				messageLoop << temp;
				n = static_cast<std::uint32_t>( ObjectFactory::GetSingleton().SizeOfObjects( OT_ITEM ));
				total += tmp = n + n * sizeof( intptr_t ) * 4;
				temp = oldstrutil::format( "     Items: %u bytes [%u items ( %u allocated )]", tmp, ObjectFactory::GetSingleton().CountOfObjects( OT_ITEM ), n );
				messageLoop << temp;
				temp = oldstrutil::format( "        You save I: %lu & C: %lu bytes!", m * sizeof(CItem) - ObjectFactory::GetSingleton().CountOfObjects( OT_ITEM ), m * sizeof( CChar ) - ObjectFactory::GetSingleton().CountOfObjects( OT_CHAR ));
				total += tmp = 69 * sizeof( CSpellInfo );
				temp = oldstrutil::format( temp, "     Spells: %i bytes", tmp );
				messageLoop << "     Sizes:";
				temp = oldstrutil::format("        CItem  : %lu bytes", sizeof( CItem ));
				messageLoop << temp;
				temp = oldstrutil::format( "        CChar  : %lu bytes", sizeof( CChar ));
				messageLoop << temp;
				temp = oldstrutil::format( "        TEffect: %lu bytes %lui total)", sizeof( CTEffect ), sizeof( CTEffect ) * cwmWorldState->tempEffects.Num() );
				messageLoop << temp;
				temp = oldstrutil::format( "        Approximate Total: %i bytes", total );
				messageLoop << temp;
				break;
			case '?':
				messageLoop << MSG_SECTIONBEGIN;
				messageLoop << "Console commands:";
				messageLoop << MSG_SECTIONBEGIN;
				messageLoop << " ShardOP:";
				messageLoop << "    * - Lock/Unlock Console ? - Commands list(this)";
				messageLoop << "    C - Configuration       H - Unused";
				messageLoop << "    Y - Console Broadcast   Q - Quit/Exit           ";
				messageLoop << " Load Commands:";
				messageLoop << "    1 - Ini                 2 - Accounts";
				messageLoop << "    3 - Regions             4 - Spawn Regions";
				messageLoop << "    5 - Spells              6 - Commands";
				messageLoop << "    7 - Dfn's               8 - JavaScript";
				messageLoop << "    9 - HTML Templates      0 - ALL(1-9)";
				messageLoop << " Save Commands:";
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
			case 'Z':
			{
				auto loggingEnabled = false;
				{
					for( auto &snSock : Network->connClients )
					{
						if( snSock )
						{
							snSock->Logging( !snSock->Logging() );
						}
					}
					auto iter = Network->connClients.begin();
					if( iter != Network->connClients.end() )
					{
						loggingEnabled = ( *iter )->Logging();
					}
				}
				if( loggingEnabled )
				{
					messageLoop << "CMD: Network Logging Enabled.";
				}
				else
				{
					messageLoop << "CMD: Network Logging Disabled.";
				}
				break;
			}
			case 'c':
			case 'C':
				// Shows a configuration header
				DisplaySettings();
				break;
			case 'f':
			case 'F':
				FileLookup->DisplayPriorityMap();
				break;
			default:
				temp = oldstrutil::format( "Key \'%c\' [%i] does not perform a function", static_cast<SI08>( c ), c );
				messageLoop << temp;
				break;
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole::DisplaySettings()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	UOX startup stuff
//|					Moved that here because we need it in processkey now
//|
//|	Changes		-	10/21/2002 - found the bug in one spot, just
//|									happened upon this quick fix. for BackUp operation.
//o------------------------------------------------------------------------------------------------o
auto CConsole::DisplaySettings() -> void
{
	std::map< bool, std::string > activeMap;
	activeMap[true] = "Activated!";
	activeMap[false] = "Disabled!";

	// UOX.ini status
	(*this) << "Server Settings:" << myendl;

	(*this) << "   -Archiving[";
	if( cwmWorldState->ServerData()->ServerBackupStatus() )
		(*this) << "Enabled]. (" << cwmWorldState->ServerData()->Directory( CSDDP_BACKUP ) << ")" << myendl;
	else
		(*this) << "Disabled]" << myendl;

	(*this) << "   -Weapons & Armour Rank System: ";
	(*this) << activeMap[cwmWorldState->ServerData()->RankSystemStatus()] << myendl;

	(*this) << "   -Vendors buy by item name: ";
	(*this) << activeMap[cwmWorldState->ServerData()->SellByNameStatus()] << myendl;

	(*this) << "   -Adv. Trade System: ";
	(*this) << activeMap[cwmWorldState->ServerData()->TradeSystemStatus()] << myendl;


	(*this) << "   -Races: " << static_cast<UI32>( Races->Count() ) << myendl;
	(*this) << "   -Guilds: " << static_cast<UI32>( GuildSys->NumGuilds() ) << myendl;
	(*this) << "   -Char count: " << ObjectFactory::GetSingleton().CountOfObjects( OT_CHAR ) << myendl;
	(*this) << "   -Item count: " << ObjectFactory::GetSingleton().CountOfObjects( OT_ITEM ) << myendl;
	(*this) << "   -Num Accounts: " << Accounts->size() << myendl;
	(*this) << "   Directories: " << myendl;
	(*this) << "   -Shared:          " << cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) << myendl;
	(*this) << "   -Archive:         " << cwmWorldState->ServerData()->Directory( CSDDP_BACKUP ) << myendl;
	(*this) << "   -Data:            " << cwmWorldState->ServerData()->Directory( CSDDP_DATA ) << myendl;
	(*this) << "   -Defs:            " << cwmWorldState->ServerData()->Directory( CSDDP_DEFS ) << myendl;
	(*this) << "   -Scripts:         " << cwmWorldState->ServerData()->Directory( CSDDP_SCRIPTS ) << myendl;
	(*this) << "   -ScriptData:      " << cwmWorldState->ServerData()->Directory( CSDDP_SCRIPTDATA ) << myendl;
	(*this) << "   -HTML:            " << cwmWorldState->ServerData()->Directory( CSDDP_HTML ) << myendl;
	(*this) << "   -Books:           " << cwmWorldState->ServerData()->Directory( CSDDP_BOOKS ) << myendl;
	(*this) << "   -MessageBoards:   " << cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD ) << myendl;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	void RegisterKey()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Registers key input for detection in console
//o------------------------------------------------------------------------------------------------o
auto CConsole::RegisterKey( SI32 key, std::string cmdName, UI16 scriptId ) -> void
{
#if defined( UOX_DEBUG_MODE )
	messageLoop << oldstrutil::format( "         Registering key \"%c\"", key );
#endif
	JSKeyHandler[key] = JSConsoleEntry_st( scriptId, cmdName );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole::SetKeyStatus()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Enabling/Disabling specific key input in console
//o------------------------------------------------------------------------------------------------o
auto CConsole::SetKeyStatus( SI32 key, bool isEnabled ) -> void
{
	auto toFind	= JSKeyHandler.find( key );
	if( toFind != JSKeyHandler.end() )
	{
		toFind->second.isEnabled = isEnabled;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole::RegisterFunc()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Registers console function
//o------------------------------------------------------------------------------------------------o
auto CConsole::RegisterFunc( const std::string &cmdFunc, const std::string &cmdName, UI16 scriptId ) -> void
{
#if defined( UOX_DEBUG_MODE )
	Print( oldstrutil::format( "         Registering console func \"%s\"\n", cmdFunc.c_str() ));
#endif
	JSConsoleFunctions[oldstrutil::upper( cmdFunc )]	= JSConsoleEntry_st( scriptId, cmdName );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole::SetFuncStatus()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Enables/disables console function
//o------------------------------------------------------------------------------------------------o
auto CConsole::SetFuncStatus( const std::string &cmdFunc, bool isEnabled ) -> void
{
	auto upper	 = oldstrutil::upper( cmdFunc );
	auto toFind	= JSConsoleFunctions.find( upper );
	if( toFind != JSConsoleFunctions.end() )
	{
		toFind->second.isEnabled = isEnabled;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole::Registration()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Registers console script
//o------------------------------------------------------------------------------------------------o
auto CConsole::Registration() -> void
{
	auto spellSection = JSMapping->GetSection( SCPT_CONSOLE );
	for( const auto &[spellname, ourScript] : spellSection->collection() )
	{
		if( ourScript )
		{
			ourScript->ScriptRegistration( "Console" );
		}
	}
}
