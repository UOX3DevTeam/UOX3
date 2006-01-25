//o---------------------------------------------------------------------------o
//| cConsole.cpp
//o---------------------------------------------------------------------------o
//| In essence, CConsole is our nice generic class for outputting display.
//| As long as the interface is kept the same, we can display whatever we wish
//| too. We store a set of coordinates being the window corner and size, for
//| systems with windowing support, which describes the window we are in.
//o---------------------------------------------------------------------------o
//| Version History
//| 1.7		Added comments (Mr. Fixit 29. October 2001)
//| 2.0		Added filter masking (Abaddon, 22nd January 2002)
//o---------------------------------------------------------------------------o

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

#if UOX_PLATFORM != PLATFORM_WIN32
	#include <stdio.h>
	#include <unistd.h>
	#include <termios.h>
	#include <sys/ioctl.h>
	typedef void *HANDLE;
#else
	#include <process.h>
	#include <conio.h>
#endif


namespace UOX
{

CConsole						Console;	// no *, else we can't overload <<
CEndL							myendl;

const UI08 NORMALMODE			= 0;
const UI08 WARNINGMODE			= 1;
const UI08 ERRORMODE			= 2;
const UI08 COLOURMODE			= 3;

bool cluox_io					= false;   // is cluox-IO enabled?
bool cluox_nopipe_fill			= false;   // the stdin-pipe is known to be none-empty, no need to fill it.
HANDLE cluox_stdin_writeback	= 0; // the write-end of the stdin-pipe


// Forward function declarations

void		endmessage( int x );
void		LoadINIFile( void );
void		LoadCustomTitle( void );
void		LoadSkills( void );
void		LoadSpawnRegions( void );
void		LoadRegions( void );
void		LoadTeleportLocations( void );
void		LoadCreatures( void );
void		LoadPlaces( void );
void		UnloadRegions( void );
void		UnloadSpawnRegions( void );


//o---------------------------------------------------------------------------o
//|   Function    -  CConsole()
//|   Date        -  Unknown
//|   Programmer  -  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     -  Class Constructor and deconstructor
//o---------------------------------------------------------------------------o
CConsole::CConsole() : left( 0 ), top( 0 ), height( 25 ), width( 80 ), filterSettings( 0xFFFF ), 
#if UOX_PLATFORM == PLATFORM_WIN32
currentMode( NORMALMODE ),currentLevel( 0 ), previousColour( CNORMAL ), logEcho( false )
#else
currentMode( NORMALMODE ),currentLevel( 0 ), previousColour( CNORMAL ), logEcho( false ), forceNL( false )
#endif
{
}

CConsole::~CConsole()
{
}


//o---------------------------------------------------------------------------o
//|   Function    -  << Overrriding
//|   Date        -  Unknown
//|   Programmer  -  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     -  Let you use << and >>
//o---------------------------------------------------------------------------o
CConsole& CConsole::operator<<( const SI08 *outPut )
{
	if( CanPrint( currentMode, currentLevel ) )
	{
		StartOfLineCheck();
		(*this) << (const char *)outPut;
	}
	return (*this);
}
CConsole& CConsole::operator<<( const char *outPut )
{
	if( CanPrint( currentMode, currentLevel ) )
	{
		StartOfLineCheck();
		std::cout << outPut;
	}
	return (*this);
}
CConsole& CConsole::operator<<( const UI08 *outPut )
{
	if( CanPrint( currentMode, currentLevel ) )
	{
		StartOfLineCheck();
		(*this) << (const char *)outPut;
	}
	return (*this);
}
CConsole& CConsole::operator<<( const SI32 &outPut )
{
	if( CanPrint( currentMode, currentLevel ) )
	{
		StartOfLineCheck();
		std::cout << outPut;
	}
	return (*this);
}
CConsole& CConsole::operator<<( const UI32 &outPut )
{
	if( CanPrint( currentMode, currentLevel ) )
	{
		StartOfLineCheck();
		std::cout << outPut;
	}
	return (*this);
}
CConsole& CConsole::operator<<( const SI16 &outPut )
{
	if( CanPrint( currentMode, currentLevel ) )
	{
		StartOfLineCheck();
		std::cout << outPut;
	}
	return (*this);
}
CConsole& CConsole::operator<<( const UI16 &outPut )
{
	if( CanPrint( currentMode, currentLevel ) )
	{
		StartOfLineCheck();
		std::cout << outPut;
	}
	return (*this);
}
CConsole& CConsole::operator<<( const SI08 &outPut )
{
	if( CanPrint( currentMode, currentLevel ) )
	{
		StartOfLineCheck();
		std::cout << outPut;
	}
	return (*this);
}
CConsole& CConsole::operator<<( const UI08 &outPut )
{
	if( CanPrint( currentMode, currentLevel ) )
	{
		StartOfLineCheck();
		std::cout << outPut;
	}
	return (*this);
}
CConsole& CConsole::operator<<( const CBaseObject *outPut )
{
	if( CanPrint( currentMode, currentLevel ) )
	{
		StartOfLineCheck();
		(*this) << outPut->GetSerial();
	}
	return (*this);
}
CConsole& CConsole::operator<<( const std::string &outPut )
{
	if( CanPrint( currentMode, currentLevel ) )
	{
		StartOfLineCheck();
		std::cout << outPut.c_str();
	}
	return (*this);
}
CConsole& CConsole::operator<<( const std::ostream& outPut )
{
	if( CanPrint( currentMode, currentLevel ) )
	{
		StartOfLineCheck();
		std::cout << outPut;
	}
	return (*this);
}
CConsole& CConsole::operator<<( CBaseObject *outPut )
{
	if( CanPrint( currentMode, currentLevel ) )
	{
		StartOfLineCheck();
		(*this) << outPut->GetSerial();
	}
	return (*this);
}
CConsole& CConsole::operator<<( std::ostream& outPut )
{
	if( CanPrint( currentMode, currentLevel ) )
	{
		StartOfLineCheck();
		std::cout << outPut;
	}
	return (*this);
}
CConsole& CConsole::operator<<( CEndL& myObj )
{
	if( CanPrint( currentMode, currentLevel ) )
	{
		if( curLeft == 0 )
			PrintStartOfLine();
		std::cout << std::endl;
		curLeft = 0;
		++curTop;
	}
	return (*this);
}
CConsole& CConsole::operator<<( const R32 &outPut )
{
	if( CanPrint( currentMode, currentLevel ) )
	{
		StartOfLineCheck();
		std::cout << outPut;
	}
	return (*this);
}
CConsole& CConsole::operator<<( const R64 &outPut )
{
	if( CanPrint( currentMode, currentLevel ) )
	{
		StartOfLineCheck();
		std::cout << outPut;
	}
	return (*this);
}

CConsole& CConsole::operator<<( const size_t &outPut )
{
	if( CanPrint( currentMode, currentLevel ) )
	{
		StartOfLineCheck();
		std::cout << static_cast< UI32 >(outPut);
	}
	return (*this);
}

//o---------------------------------------------------------------------------o
//|   Function    -  Print
//|   Date        -  Unknown
//|   Programmer  -  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sprintfs text and prints the console with the "|"
//o---------------------------------------------------------------------------o
void CConsole::Print( const char *toPrint, ... )
{
	va_list argptr;
	char msg[MAX_CONSOLE_BUFF];

	va_start( argptr, toPrint );
	vsnprintf( msg, MAX_CONSOLE_BUFF, toPrint, argptr );
	va_end( argptr );

	if( CanPrint( currentMode, currentLevel ) )
	{
		StartOfLineCheck();
		std::cout << msg;
		size_t i = strlen( msg );
		if( i > 0 && msg[i-1] == '\n' )
			curLeft = 0;
	}
}


//o---------------------------------------------------------------------------o
//|   Function    -  Log
//|   Date        -  Unknown
//|   Programmer  -  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     -  Writes to the logfile
//o---------------------------------------------------------------------------o
void CConsole::Log( const char *toLog, const char *filename, ... )
{
	va_list argptr;
	char msg[MAX_CONSOLE_BUFF];

	if( !cwmWorldState->ServerData()->ServerConsoleLogStatus() )
		return;

	va_start( argptr, filename );
	vsnprintf( msg, MAX_CONSOLE_BUFF, toLog, argptr );
	va_end( argptr );

	std::ofstream toWrite;
	std::string realFileName;	// EviLDeD: 022602: in windows a path can be max 512 chars, this at 128 coud potentially cause crashes if the path is longer than 128 chars
	if( cwmWorldState != NULL )
		realFileName = cwmWorldState->ServerData()->Directory( CSDDP_LOGS ) + filename;
	else
		realFileName = filename;

	char timeStr[128];
	RealTime( timeStr );

	toWrite.open( realFileName.c_str(), std::ios::out | std::ios::app );
	if( toWrite.is_open() )
		toWrite << "[" << timeStr << "] " << msg << std::endl;
	toWrite.close();
	if( LogEcho() )
	{
		char mtemp[256];
		sprintf( mtemp, "%s%s\n", timeStr, msg );
		Print( mtemp );
	}
}


//o---------------------------------------------------------------------------o
//|   Function    -  Log
//|   Date        -  Unknown
//|   Programmer  -  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     -  Log to the console.log file
//o---------------------------------------------------------------------------o
void CConsole::Log( const char *toLog, ... )
{
	if( !cwmWorldState->ServerData()->ServerConsoleLogStatus() )
		return;

	va_list argptr;
	char msg[MAX_CONSOLE_BUFF];
	va_start( argptr, toLog );
	vsnprintf( msg, MAX_CONSOLE_BUFF, toLog, argptr );
	va_end( argptr );
	Log( msg, "console.log" );
}


//o---------------------------------------------------------------------------o
//|   Function    -  Error
//|   Date        -  Unknown
//|   Programmer  -  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     -  Writes to the error log and the console.
//o---------------------------------------------------------------------------o
void CConsole::Error( UI08 level, const char *toWrite, ... )
{
	if( !CanPrint( ERRORMODE, level ) )
		return;
	UI08 oldMode = CurrentMode(), oldLevel = CurrentLevel();
	CurrentMode( ERRORMODE );
	CurrentLevel( level );
	va_list argptr;
	char msg[MAX_CONSOLE_BUFF];
	va_start( argptr, toWrite );
	vsnprintf( msg, MAX_CONSOLE_BUFF, toWrite, argptr );
	va_end( argptr );
	Log( msg, "error.log" );
	if( curLeft != 0 )
		(*this) << myendl;
	TurnRed();
	(*this) << "ERROR: " << (const char*)&msg[0] << myendl;
	TurnNormal();
	CurrentMode( oldMode );
	CurrentLevel( oldLevel );
}


//o---------------------------------------------------------------------------o
//|   Function    -  Left, Top, Height, Width
//|   Date        -  Unknown
//|   Programmer  -  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets and gets console size
//o---------------------------------------------------------------------------o
UI16 CConsole::Left( void ) const
{
	return left;
}
UI16 CConsole::Top( void ) const
{
	return top;
}
UI16 CConsole::Height( void ) const
{
	return height;
}
UI16 CConsole::Width( void ) const
{
	return width;
}
void CConsole::Left( UI16 nVal )
{
	left = nVal;
}
void CConsole::Top( UI16 nVal )
{
	top = nVal;
}
void CConsole::Height( UI16 nVal )
{
	height = nVal;
}
void CConsole::Width( UI16 nVal )
{
	width = nVal;
}


//o---------------------------------------------------------------------------o
//|   Function    -  PrintSectionBegin
//|   Date        -  Unknown
//|   Programmer  -  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     -  Prints a section seperator
//o---------------------------------------------------------------------------o
void CConsole::PrintSectionBegin( void )
{
	TurnBrightWhite();
	for( UI16 i = 0; i < left; ++i )
		std::cout << " ";
	std::cout << "o";
	for( int j = left + 1; j < width - 1; ++j )
		std::cout << "-";
	std::cout << "o";
	curLeft = 0;
	curTop = 0;
	TurnNormal();
#if UOX_PLATFORM != PLATFORM_WIN32
	if( forceNL )
	{
		(*this) << myendl;
	} 
#endif
}

//o---------------------------------------------------------------------------o
//|   Function    -  Start
//|   Date        -  24th December, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Migration of constart function to here
//o---------------------------------------------------------------------------o
void CConsole::Start( char *temp )
{
#if UOX_PLATFORM == WIN32
	hco		= GetStdHandle( STD_OUTPUT_HANDLE );
	GetConsoleScreenBufferInfo( hco, &csbi );
	width	= csbi.dwSize.X;
	height	= csbi.dwSize.Y;
	SetConsoleTitle( temp );
#else
	// TODO: unix console handling should really be replaced by (n)curses or
	// something

	if (isatty (0))
	{
		struct termios tio;
		struct winsize winsz;

		// switch to raw mode
		tcgetattr (0, &tio);

		tio.c_lflag &= ~ICANON & ~ECHO;

		tcsetattr (0, TCSAFLUSH, &tio); //ignore errors

		// get window size
		ioctl (0, TIOCGWINSZ, &winsz);

		width = winsz.ws_col;
		height = winsz.ws_row;

		// disable stdout buffering
		setvbuf (stdout, NULL, _IONBF, 0);
	}
	else
	{
		// produce readable log
		forceNL = true;
	} 
#endif
}

//o---------------------------------------------------------------------------o
//|   Function    -  TurnYellow
//|   Date        -  24th December, 2001
//|   Programmer  -  DarkStorm
//o---------------------------------------------------------------------------o
//|   Purpose     -  Turns the text yellow
//o---------------------------------------------------------------------------o
void CConsole::TurnYellow( void )
{
#if UOX_PLATFORM == PLATFORM_WIN32
	SetConsoleTextAttribute( hco, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY );
#else
	std::cout << "\033[1;33m";
#endif
	previousColour = CYELLOW;
}

//o---------------------------------------------------------------------------o
//|   Function    -  TurnRed
//|   Date        -  24th December, 2001
//|   Programmer  -  DarkStorm
//o---------------------------------------------------------------------------o
//|   Purpose     -  Turns the text red
//o---------------------------------------------------------------------------o
void CConsole::TurnRed( void )
{
#if UOX_PLATFORM == PLATFORM_WIN32
	SetConsoleTextAttribute( hco, FOREGROUND_RED | FOREGROUND_INTENSITY );
#else
	std::cout << "\033[1;31m";
#endif
	previousColour = CRED;
}

//o---------------------------------------------------------------------------o
//|   Function    -  TurnGreen
//|   Date        -  24th December, 2001
//|   Programmer  -  DarkStorm
//o---------------------------------------------------------------------------o
//|   Purpose     -  Turns the text green
//o---------------------------------------------------------------------------o
void CConsole::TurnGreen( void )
{
#if UOX_PLATFORM == PLATFORM_WIN32
	SetConsoleTextAttribute( hco, FOREGROUND_GREEN | FOREGROUND_INTENSITY );
#else
	std::cout << "\033[1;32m";
#endif
	previousColour = CGREEN;
}

//o---------------------------------------------------------------------------o
//|   Function    -  TurnBlue
//|   Date        -  24th December, 2001
//|   Programmer  -  DarkStorm
//o---------------------------------------------------------------------------o
//|   Purpose     -  Turns the text blue
//o---------------------------------------------------------------------------o
void CConsole::TurnBlue( void )
{
#if UOX_PLATFORM == PLATFORM_WIN32
	SetConsoleTextAttribute( hco, FOREGROUND_BLUE | FOREGROUND_INTENSITY );
#else
	std::cout << "\033[1;34m";
#endif
	previousColour = CBLUE;
}

//o---------------------------------------------------------------------------o
//|   Function    -  TurnNormal
//|   Date        -  24th December, 2001
//|   Programmer  -  DarkStorm
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns text to normal white
//o---------------------------------------------------------------------------o
void CConsole::TurnNormal( void )
{
#if UOX_PLATFORM == PLATFORM_WIN32
	SetConsoleTextAttribute( hco, FOREGROUND_BLUE  | FOREGROUND_RED | FOREGROUND_GREEN );
#else
	std::cout << "\033[0;37m";
#endif
	previousColour = CNORMAL;
}

//o---------------------------------------------------------------------------o
//|   Function    -  TurnBrightWhite
//|   Date        -  22nd January, 2002
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns text to bright white
//o---------------------------------------------------------------------------o
void CConsole::TurnBrightWhite( void )
{
#if UOX_PLATFORM == PLATFORM_WIN32
	SetConsoleTextAttribute( hco, FOREGROUND_BLUE  | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY );
#else
	std::cout << "\033[1;37m";
#endif
}

//o---------------------------------------------------------------------------o
//|		Function	-	PrintDone( void )
//|		Date		-	24th December, 2001
//|		Programmer	-	DarkStorm
//|		Modification-	Maarc - 7th February, 2003 - Made it use PrintSpecial
//o---------------------------------------------------------------------------o
//|   Purpose     -  Prints colored "[done]" message
//o---------------------------------------------------------------------------o
void CConsole::PrintDone( void )
{
	PrintSpecial( CGREEN, "done" );
}

//o---------------------------------------------------------------------------o
//|		Function    -	PrintFailed( void )
//|		Date        -	24th December, 2001
//|		Programmer  -	DarkStorm
//|		Modification-	Maarc - 7th February, 2003 - Made it use PrintSpecial
//o---------------------------------------------------------------------------o
//|   Purpose     -  Prints colored "[failed]" message
//o---------------------------------------------------------------------------o
void CConsole::PrintFailed( void )
{
	PrintSpecial( CRED, "Failed" );
}

//o---------------------------------------------------------------------------o
//|		Function    -	PrintPassed( void )
//|		Date        -	17th February, 2002
//|		Programmer  -	DarkStorm
//|		Modification-	Maarc - 7th February, 2003 - Made it use PrintSpecial
//o---------------------------------------------------------------------------o
//|   Purpose     -  Prints colored "[passed]" message
//o---------------------------------------------------------------------------o
void CConsole::PrintPassed( void )
{
	PrintSpecial( CYELLOW, "Skipped" );
}

//o---------------------------------------------------------------------------o
//|   Function    -  ClearScreen
//|   Date        -  24th December, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Clears the screen
//o---------------------------------------------------------------------------o
void CConsole::ClearScreen( void )
{
#if UOX_PLATFORM == PLATFORM_WIN32
	UI32 y;
	COORD xy;

	xy.X = 0;
	xy.Y = 0;
	FillConsoleOutputCharacter( hco, ' ', width * height, xy, &y );
	SetConsoleCursorPosition( hco, xy );
#else
// Linux clear screen here
#endif
}

void CConsole::PrintBasedOnVal( bool value )
{
	if( value )
		PrintDone();
	else
		PrintFailed();
}

//o---------------------------------------------------------------------------o
//|   Function    -  Warning
//|   Date        -  21st January, 2002
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Writes to the warning log and the console.
//o---------------------------------------------------------------------------o
void CConsole::Warning( UI08 level, const char *toWrite, ... )
{
	if( !CanPrint( WARNINGMODE, level ) )
		return;
	UI08 oldMode = CurrentMode(), oldLevel = CurrentLevel();
	CurrentMode( WARNINGMODE );
	CurrentLevel( level );
	va_list argptr;
	char msg[MAX_CONSOLE_BUFF];
	va_start( argptr, toWrite );
	vsnprintf( msg, MAX_CONSOLE_BUFF, toWrite, argptr );
	va_end( argptr );
	Log( msg, "warning.log" );
	if( curLeft != 0 )
		(*this) << myendl;
	TurnBlue();
	(*this) << "WARNING: " << (const char*)&msg[0] << myendl;
	TurnNormal();
	CurrentMode( oldMode );
	CurrentLevel( oldLevel );
}

const UI16 ANDSettings[16]	= {	1,	2,	4,	8,	16,	32,	64,	128,	256,	512,	1024,	2048,	4096,	8192,	16384,	32768 };
const UI16 ORTSettings[16]	= {	1,	2,	4,	8,	16,	32,	64,	128,	256,	512,	1024,	2048,	4096,	8192,	16384,	32768 };
const UI16 ORFSettings[16]	= {	0xFFFE,	0xFFFD,	0xFFFB,	0xFFF7,	0xFFEF,	0xFFDF,	0xFFBF,	0xFF7F,	0xFEFF,	0xFDFF,	0xFBFF,	0xF7FF,	0xEFFF,	0xDFFF,	0xBFFF,	0x7FFF };

void CConsole::FilterBit( UI08 type, UI08 level, bool status )
{
	if( level > 4 )
		level = 4;
	UI08 base = 0;
	switch( type )
	{
		case NORMALMODE:							break;
		case WARNINGMODE:	base = 5;				break;
		case ERRORMODE:		base = 10;				break;
		case COLOURMODE:	base = 15;	level = 0;	break;
		default:									return;
	}
	if( status )
		filterSettings |= ORTSettings[base + level];
	else
		filterSettings &= ORFSettings[base + level];
}
void CConsole::FilterSetting( UI16 value )
{
	filterSettings = value;
}
void CConsole::CurrentMode( UI08 value )
{
	currentMode = value;
}
void CConsole::CurrentLevel( UI08 value )
{
	currentLevel = value;
}

bool CConsole::CanPrint( UI08 type, UI08 level ) const
{
	if( level > 4 )
		level = 4;
	switch( type )
	{
		case NORMALMODE:	return ( (filterSettings&ANDSettings[level]) == ANDSettings[level] );
		case WARNINGMODE:	return ( (filterSettings&ANDSettings[5+level]) == ANDSettings[5+level] );
		case ERRORMODE:		return ( (filterSettings&ANDSettings[10+level]) == ANDSettings[10+level] );
		case COLOURMODE:	return ( (filterSettings&ANDSettings[15]) == ANDSettings[15] );
		default:
			return false;
	}
	return false;
}
UI16 CConsole::FilterSetting( void ) const
{
	return filterSettings;
}
UI08 CConsole::CurrentMode( void ) const
{
	return currentMode;
}
UI08 CConsole::CurrentLevel( void ) const
{
	return currentLevel;
}

void CConsole::PrintStartOfLine( void )
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

#if UOX_PLATFORM == PLATFORM_WIN32
void CConsole::MoveTo( int x, int y )
{
	COORD Pos;
	if( y == -1 )
	{
		CONSOLE_SCREEN_BUFFER_INFO ScrBuffInfo;

		GetConsoleScreenBufferInfo( hco, &ScrBuffInfo );
		Pos.Y = ScrBuffInfo.dwCursorPosition.Y;
		Pos.X = (UI16)x;
		SetConsoleCursorPosition( hco, Pos );
	}
	else
	{
		Pos.X = (UI16)x;
		Pos.Y = (UI16)y;
		SetConsoleCursorPosition( hco, Pos );
	}
}
#else
void CConsole::MoveTo( int x, int y )
{
	std::cout << "\033[255D";
	std::cout << "\033[" << x << "C";
}
#endif

//o--------------------------------------------------------------------------o
//|	Function			-	bool CConsole::LogEcho( void )
//|	Date					-	2/03/2003
//|	Developers		-	Maarc
//|	Organization	-	UOX3DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
bool CConsole::LogEcho( void )
{
	return logEcho;
}

//o--------------------------------------------------------------------------o
//|	Function			-	void CConsole::LogEcho( bool value )
//|	Date					-	2/03/2003
//|	Developers		-	Maarc
//|	Organization	-	UOX3DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
void CConsole::LogEcho( bool value )
{
	logEcho = value;
}

//o--------------------------------------------------------------------------o
//|	Function		-	void CConsole::StartOfLineCheck( void )
//|	Date			-	2/3/2003
//|	Developers		-	Maarc
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Reduces excess work in overloaded functions by checking
//|							to see if the start of line has to be done here
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
void CConsole::StartOfLineCheck( void )
{
	if( curLeft == 0 )
	{
		PrintStartOfLine();
		curLeft = 1;
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	void CConsole::PrintSpecial( UI08 colour, const char *toPrint, ... )
//|	Date			-	2/7/2003
//|	Developers		-	Maarc
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	This is much like PrintFailed, PrintDone and so on except 
//|							we specify the text and the colour
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
void CConsole::PrintSpecial( UI08 colour, const char *toPrint, ... )
{
	va_list argptr;
	char msg[MAX_CONSOLE_BUFF];

	va_start( argptr, toPrint );
	vsnprintf( msg, MAX_CONSOLE_BUFF, toPrint, argptr );
	va_end( argptr );

	if( CanPrint( currentMode, currentLevel ) )
	{
		StartOfLineCheck();
		size_t stringLength = strlen( msg ) + 3;
		MoveTo( static_cast< int >(width - stringLength) );
		TurnNormal();
		(*this) << "[";
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
		(*this) << msg;
		TurnNormal();
		(*this) << "]" << myendl;
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	cl_getch
//|	Programmer	-	knoxos
//o---------------------------------------------------------------------------o
//|	Purpose		-	Read a character from stdin, in a cluox compatble way.
//|                 This routine is non-blocking!
//|	Returns		-	>0 -> character read
//|                 -1 -> no char available.
//o---------------------------------------------------------------------------o
//
// now cluox is GUI wrapper over uox using stdin and stdout redirection to capture
// the console, if it is active uox can't use kbhit() to determine if there is a 
// character aviable, it can only get one directly by getch().
// However the problem arises that uox will get blocked if none is aviable.
// The solution to this problem is that cluox also hands over the second pipe-end
// of stdin so uox can write itself into this stream. To determine if a character is 
// now done that way. UOX write's itself a ZERO on the other end of the pipe, and reads
// a charecter, if it is again the same ZERO just putted in nothing was entered. However
// it is not a zero the user has entered a char.
// 
int CConsole::cl_getch( void )
{
#if UOX_PLATFORM != PLATFORM_WIN32
	// first the linux style, don't change it's behavoir
	UI08 c = 0;
	fd_set KEYBOARD;
	FD_ZERO( &KEYBOARD );
	FD_SET( 0, &KEYBOARD );
	int s = select( 1, &KEYBOARD, NULL, NULL, &cwmWorldState->uoxtimeout );
	if( s < 0 )
	{
		Error( 1, "Error scanning key press" );
		messageLoop << MSG_SHUTDOWN;
	}
	if( s > 0 )
	{
		read( 0, &c, 1 );
		if( c == 0x0A )
			return -1;
	}
#else
	// now the windows one
	if( !cluox_io )
	{
		// uox is not wrapped simply use the kbhit routine
		if( _kbhit() )
			return _getch();
		else 
			return -1;
	}
	// the wiered cluox getter.
	UI08 c = 0;
	UI32 bytes_written = 0;
	int asw = 0;
	if( !cluox_nopipe_fill )
		asw = WriteFile( cluox_stdin_writeback, &c, 1, &bytes_written, NULL );
	if( bytes_written != 1 || asw == 0 )
	{
		Warning( 1, "Using cluox-io" );
		messageLoop << MSG_SHUTDOWN;
	}
	c = (UI08)fgetc( stdin );
	if( c == 0 )
	{
		cluox_nopipe_fill = false;
		return -1;
	}
#endif
	// here an actual charater is read in
	return c;
}

//o----------------------------------------------------------------------------o
//|   Function -	 void Poll( void )
//|   Date     -	 Unknown
//|                  25 January, 2006
//|                  Moved into the console class and renamed from checkkey() to
//|                  Poll()
//|   Programmer  -  Unknown  (Touched up by EviLDeD)
//o----------------------------------------------------------------------------o
//|   Purpose     -  Facilitate console control. SysOp keys, and localhost 
//|					 controls.
//o----------------------------------------------------------------------------o
void CConsole::Poll( void )
{
	int c = cl_getch();
	if( c > 0 )
	{
		if( (cluox_io) && ( c == 250 ) )
		{  // knox force unsecure mode, need this since cluox can't know
			//      how the toggle status is.
			if( cwmWorldState->GetSecure() )
			{
				(*this) << "Secure mode disabled. Press ? for a commands list." << myendl;
				cwmWorldState->SetSecure( false );
				return;
			}
		}
		c = toupper(c);
		Process( c );
	}
}

//o---------------------------------------------------------------------------o
//|	Function		-	void Process( int c )
//|	Programmer		-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose			-	Handle keypresses in console
//|	Modification	-	05042004 - EviLDeD added some console debugging stuff.
//|									Ideally this will all be placed onto a DEBUG menu which
//|									a user will be able to select using the <, and > keys
//|									respectivly. This functionality hasn't been implemented
//|									at the current time of writing, but will be when possible.
//o---------------------------------------------------------------------------o

void CConsole::Process( int c )
{
	char outputline[128], temp[1024];
	bool kill		= false;
	int indexcount	= 0;
	int j;
	int keyresp;
	CSocket *tSock	= NULL;

	if( c == '*' )
	{
		if( cwmWorldState->GetSecure() )
			messageLoop << "Secure mode disabled. Press ? for a commands list";
		else
			messageLoop << "Secure mode re-enabled";
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
				cScript *toExecute = JSMapping->GetScript( toFind->second.scriptID );
				if( toExecute != NULL )
				{	// All commands that execute are of the form: command_commandname (to avoid possible clashes)
	#if defined( UOX_DEBUG_MODE )
					Print( "Executing JS keystroke %c %s\n", c, toFind->second.cmdName.c_str() );
	#endif
					toExecute->CallParticularEvent( toFind->second.cmdName.c_str(), NULL, 0 );
				}
				return;
			}
		}
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
			std::cout << "System: ";
			while( !kill )
			{
				keyresp = cl_getch();
				switch( keyresp )
				{
					case -1:	// no key pressed
					case 0:
						break;
					case 0x1B:
						memset( outputline, 0x00, sizeof( outputline ) );
						indexcount = 0;
						kill = true;
						std::cout << std::endl;
						messageLoop << "CMD: System broadcast canceled.";
						break;
					case 0x08:
						--indexcount;
						if( indexcount < 0 )	
							indexcount = 0;
						else
							std::cout << "\b \b";
						break;
					case 0x0A:
					case 0x0D:
						outputline[indexcount] = 0;
						messageLoop.NewMessage( MSG_CONSOLEBCAST, outputline );
						indexcount = 0;
						kill = true;
						std::cout << std::endl;
						sprintf( temp, "CMD: System broadcast sent message \"%s\"", outputline );
						memset( outputline, 0x00, sizeof( outputline ) );
						messageLoop << temp;
						break;
					default:
						if( static_cast<size_t>(indexcount) < sizeof( outputline ) )
						{
							outputline[indexcount++] = (UI08)(keyresp);
							std::cout << (char)keyresp;
						}
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
				char szBuffer[128];
				// We need to get an iteration into the map first of all the top level ULONGs then we can get an equal range.
				std::map< ULONG, UI08 > localMap;
				localMap.clear();
				for( ADDMENUMAP_CITERATOR CJ = g_mmapAddMenuMap.begin(); CJ != g_mmapAddMenuMap.end(); CJ++ )
				{
					// check to see if the group id has been checked already
					if( localMap.find( CJ->first ) == localMap.end() )
					{
						localMap.insert( std::make_pair( CJ->first, 0 ) );
						memset( szBuffer, 0x00, sizeof( szBuffer ) );
						sprintf( szBuffer, "AddMenuGroup %lu:", CJ->first );
						messageLoop << szBuffer;
						std::pair< ADDMENUMAP_CITERATOR, ADDMENUMAP_CITERATOR > pairRange = g_mmapAddMenuMap.equal_range( CJ->first );
						int count = 0;
						for( ADDMENUMAP_CITERATOR CI=pairRange.first;CI != pairRange.second; CI++ )
						{
							count++;
						}
						memset( szBuffer, 0x00, sizeof( szBuffer ) );
						sprintf( szBuffer, "   Found %i Auto-AddMenu Item(s).", count );
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
					cwmWorldState->ServerData()->load();
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
					
					//messageLoop << MSG_RELOADJS;


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
					messageLoop << "Server can only load one script at a time";
				break;
			case 'T':
				// Timed shut down(10 minutes)
				messageLoop << "CMD: 10 Minute Server Shutdown Announced(Timed)";
				cwmWorldState->SetEndTime( BuildTimeValue( 600 ) );
				endmessage(0);
				break;
			case  'D':    
				// Disconnect account 0 (useful when client crashes)
				for( tSock = Network->LastSocket(); tSock != NULL; tSock = Network->PrevSocket() )
				{
					if( tSock->AcctNo() == 0 )
						Network->Disconnect( tSock );
				}
				messageLoop << "CMD: Socket Disconnected(Account 0).";
				break;
			case 'K':
			{
				for( tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
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
				Log( "\tPerformace Dump:", "performance.log");
				Log( "\tNetwork code: %.2fmsec [%i samples]", "performance.log", (R32)((R32)cwmWorldState->ServerProfile()->NetworkTime()/(R32)networkTimeCount), networkTimeCount);
				Log( "\tTimer code: %.2fmsec [%i samples]", "performance.log", (R32)((R32)cwmWorldState->ServerProfile()->TimerTime()/(R32)timerTimeCount), timerTimeCount);
				Log( "\tAuto code: %.2fmsec [%i samples]", "performance.log", (R32)((R32)cwmWorldState->ServerProfile()->AutoTime()/(R32)autoTimeCount), autoTimeCount);
				Log( "\tLoop Time: %.2fmsec [%i samples]", "performance.log", (R32)((R32)cwmWorldState->ServerProfile()->LoopTime()/(R32)loopTimeCount), loopTimeCount);
				ObjectFactory *ourFac = ObjectFactory::getSingletonPtr();
				Log( "\tCharacters: %i/%i - Items: %i/%i (Dynamic)", "performance.log", ourFac->CountOfObjects( OT_CHAR ), ourFac->SizeOfObjects( OT_CHAR ), ourFac->CountOfObjects( OT_ITEM ), ourFac->SizeOfObjects( OT_ITEM ) );
				Log( "\tSimulation Cycles: %f per sec", "performance.log", (1000.0*(1.0/(R32)((R32)cwmWorldState->ServerProfile()->LoopTime()/(R32)loopTimeCount))));
				Log( "\tBytes sent: %i", "performance.log", cwmWorldState->ServerProfile()->GlobalSent());
				Log( "\tBytes Received: %i", "performance.log", cwmWorldState->ServerProfile()->GlobalReceived());
				Log( "--- Performance Dump Complete ---", "performance.log");
				LogEcho( false );
				break;
				}
			case 'W':                
				// Display logged in chars
				messageLoop << "CMD: Current Users in the World:";
				j = 0;
				CSocket *iSock;
				Network->PushConn();
				for( iSock = Network->FirstSocket(); !Network->FinishedSockets(); iSock = Network->NextSocket() )
				{
					++j;
					CChar *mChar = iSock->CurrcharObj();
					sprintf( temp, "     %i) %s [%x %x %x %x]", j - 1, mChar->GetName().c_str(), mChar->GetSerial( 1 ), mChar->GetSerial( 2 ), mChar->GetSerial( 3 ), mChar->GetSerial( 4 ) );
					messageLoop << temp;
				}
				Network->PopConn();
				sprintf( temp, "     Total users online: %i", j );
				messageLoop << temp;
				break;
			case 'M':
				size_t tmp, total;
				total = 0;
				tmp = 0;
				messageLoop << "CMD: UOX Memory Information:";
				messageLoop << "     Cache:";
				sprintf( temp, "        Tiles: %u bytes", Map->TileMem );
				messageLoop << temp;
				sprintf( temp, "        Multis: %u bytes", Map->MultisMem );
				messageLoop << temp;
				size_t m, n;
				m = ObjectFactory::getSingleton().SizeOfObjects( OT_CHAR );
				total += tmp = m + m*sizeof( CTEffect ) + m*sizeof(char) + m*sizeof(int)*5;
				sprintf( temp, "     Characters: %u bytes [%u chars ( %u allocated )]", tmp, ObjectFactory::getSingleton().CountOfObjects( OT_CHAR ), m );
				messageLoop << temp;
				n = ObjectFactory::getSingleton().SizeOfObjects( OT_ITEM );
				total += tmp = n + n * sizeof( int ) * 4;
				sprintf( temp, "     Items: %u bytes [%u items ( %u allocated )]", tmp, ObjectFactory::getSingleton().CountOfObjects( OT_ITEM ), n );
				messageLoop << temp;
				sprintf( temp, "        You save I: %i & C: %i bytes!", m * sizeof(CItem) - ObjectFactory::getSingleton().CountOfObjects( OT_ITEM ), m * sizeof( CChar ) - ObjectFactory::getSingleton().CountOfObjects( OT_CHAR ) );
				total += tmp = 69 * sizeof( SpellInfo );
				sprintf( temp, "     Spells: %i bytes", tmp );
				messageLoop << "     Sizes:";
				sprintf( temp, "        CItem  : %i bytes", sizeof( CItem ) );
				messageLoop << temp;
				sprintf( temp, "        CChar  : %i bytes", sizeof( CChar ) );
				messageLoop << temp;
				sprintf( temp, "        TEffect: %i bytes (%i total)", sizeof( CTEffect ), sizeof( CTEffect ) * TEffects->Count() );
				messageLoop << temp;
				total += tmp = Map->TileMem + Map->MultisMem;
				sprintf( temp, "        Approximate Total: %i bytes", total );
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
				messageLoop << "    V - Dump Lookups(Devs)  F - Display Priority Maps";
				messageLoop << " Network Maintenence:";
				messageLoop << "    D - Disconnect Acct0    K - Disconnect All";
				messageLoop << "    Z - Socket Logging      ";
				messageLoop << MSG_SECTIONBEGIN;
				break;
			case 'v':
			case 'V':
				// Dump look up data to files so developers working with extending the ini will have a table to use
				messageLoop << "| CMD: Creating Server.scp and Uox3.ini Tag Lookup files(For Developers)....";
				cwmWorldState->ServerData()->dumpLookup( 0 );
				cwmWorldState->ServerData()->save( "./uox.tst.ini" );
				messageLoop << MSG_PRINTDONE;
				break;
			case 'z':
			case 'Z':
			{
				// Log socket activity
				Network->PushConn();
				bool loggingEnabled	= false;
				CSocket *snSock		= Network->FirstSocket();
				if( snSock != NULL )
					loggingEnabled = !snSock->Logging();
				for( ; !Network->FinishedSockets(); snSock = Network->NextSocket() )
				{
					if( snSock != NULL )
						snSock->Logging( !snSock->Logging() );
				}
				Network->PopConn();
				if( loggingEnabled )
					messageLoop << "CMD: Network Logging Enabled.";
				else
					messageLoop << "CMD: Network Logging Disabled.";
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
				sprintf( temp, "Key \'%c\' [%i] does not perform a function", (char)c, c );
				messageLoop << temp;
				break;
		}
	}
}

void CConsole::Cloak( char *callback )
{
	(*this) << "Using CLUOX Streaming-IO" << myendl;
	setvbuf( stdout, NULL, _IONBF, 0 );
	setvbuf( stderr, NULL, _IONBF, 0 );
	cluox_io = true;
	char *dummy;
	cluox_stdin_writeback = (void *)strtol( callback, &dummy, 16 );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void DisplaySettings( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	UOX startup stuff
//| Moved that here because we need it in processkey now
//|									
//|	Modification	-	10/21/2002	-	EviLDeD - Xuri found the bug in one spot, just
//|									happened upon this quick fix. for BackUp operation.
//o---------------------------------------------------------------------------o
void CConsole::DisplaySettings( void )
{
	std::map< bool, std::string > activeMap;
	activeMap[true] = "Activated!";
	activeMap[false] = "Disabled!";

	// Server.scp status --- By Magius(CHE)
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
	
	(*this) << "   -Crash Protection: ";
	if( cwmWorldState->ServerData()->ServerCrashProtectionStatus() < 1 )
		(*this) << "Disabled!" << myendl;
#ifndef _CRASH_PROTECT_
	else 
		(*this) << "Unavailable in this version" << myendl;
#else
	else if( cwmWorldState->ServerData()->ServerCrashProtectionStatus() == 1 )
		(*this) << "Save on crash" << myendl;
	else 
		(*this) << "Save & Restart Server" << myendl;
#endif

	(*this) << "   -Races: " << static_cast< UI32 >(Races->Count()) << myendl;
	(*this) << "   -Guilds: " << static_cast< UI32 >(GuildSys->NumGuilds()) << myendl;
	(*this) << "   -Char count: " << ObjectFactory::getSingleton().CountOfObjects( OT_CHAR ) << myendl;
	(*this) << "   -Item count: " << ObjectFactory::getSingleton().CountOfObjects( OT_ITEM ) << myendl;
	(*this) << "   -Num Accounts: " << Accounts->size() << myendl;
	(*this) << "   Directories: " << myendl;
	(*this) << "   -Shared:          " << cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) << myendl;
	(*this) << "   -Archive:         " << cwmWorldState->ServerData()->Directory( CSDDP_BACKUP ) << myendl;
	(*this) << "   -Data:            " << cwmWorldState->ServerData()->Directory( CSDDP_DATA ) << myendl;
	(*this) << "   -Defs:            " << cwmWorldState->ServerData()->Directory( CSDDP_DEFS ) << myendl;
	(*this) << "   -Scripts:         " << cwmWorldState->ServerData()->Directory( CSDDP_SCRIPTS ) << myendl;
	(*this) << "   -HTML:            " << cwmWorldState->ServerData()->Directory( CSDDP_HTML ) << myendl;
	(*this) << "   -Books:           " << cwmWorldState->ServerData()->Directory( CSDDP_BOOKS ) << myendl;
	(*this) << "   -MessageBoards:   " << cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD ) << myendl;
}

void CConsole::RegisterKey( int key, std::string cmdName, UI16 scriptID )
{
#if defined( UOX_DEBUG_MODE )
	Print( "         Registering key \"%c\"\n", key );
#endif
	JSKeyHandler[key] = JSConsoleEntry( scriptID, cmdName );
}

void CConsole::SetKeyStatus( int key, bool isEnabled )
{
	JSCONSOLEKEYMAP_ITERATOR	toFind	= JSKeyHandler.find( key );
	if( toFind != JSKeyHandler.end() )
	{
		toFind->second.isEnabled = isEnabled;
	}
}

void CConsole::RegisterFunc( std::string cmdFunc, std::string cmdName, UI16 scriptID )
{
#if defined( UOX_DEBUG_MODE )
	Print( "         Registering console func \"%s\"\n", cmdFunc.c_str() );
#endif
	UString upper				= cmdFunc;
	upper						= upper.upper();
	JSConsoleFunctions[upper]	= JSConsoleEntry( scriptID, cmdName );
}

void CConsole::SetFuncStatus( std::string cmdFunc, bool isEnabled )
{
	UString upper						= cmdFunc;
	upper								= upper.upper();
	JSCONSOLEFUNCMAP_ITERATOR	toFind	= JSConsoleFunctions.find( upper );
	if( toFind != JSConsoleFunctions.end() )
	{
		toFind->second.isEnabled = isEnabled;
	}
}

void CConsole::Registration( void )
{
	CJSMappingSection *spellSection = JSMapping->GetSection( SCPT_CONSOLE );
	for( cScript *ourScript = spellSection->First(); !spellSection->Finished(); ourScript = spellSection->Next() )
	{
		if( ourScript != NULL )
			ourScript->ScriptRegistration( "Console" );
	}
}

}
