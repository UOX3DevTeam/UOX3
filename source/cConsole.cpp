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
#include "cConsole.h"
#include <cstdarg>

const UI08 NORMALMODE = 0;
const UI08 WARNINGMODE = 1;
const UI08 ERRORMODE = 2;
const UI08 COLOURMODE = 3;

//o---------------------------------------------------------------------------o
//|   Function    -  CConsole()
//|   Date        -  Unknown
//|   Programmer  -  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     -  Class Constructor and deconstructor
//o---------------------------------------------------------------------------o
CConsole::CConsole() : left( 0 ), top( 0 ), height( 25 ), width( 80 ), filterSettings( 0xFFFF ), 
 currentMode( NORMALMODE ),currentLevel( 0 ), previousColour( CNORMAL ), logEcho( false )
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
CConsole& CConsole::operator<<( const cBaseObject *outPut )
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
CConsole& CConsole::operator<<( cBaseObject *outPut )
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
		curTop++;
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
		UI32 i = strlen( msg );
		if( msg[i-1] == '\n' )
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

	if ( !cwmWorldState->ServerData()->GetServerConsoleLogStatus() )
		return;

	va_start( argptr, filename );
	vsnprintf( msg, MAX_CONSOLE_BUFF, toLog, argptr );
	va_end( argptr );

	std::ofstream toWrite;
	char realFileName[MAX_PATH];	// EviLDeD: 022602: in windows a path can be max 512 chars, this at 128 coud potentially cause crashes if the path is longer than 128 chars
	strcpy( realFileName, cwmWorldState->ServerData()->GetLogsDirectory() );
	if( cwmWorldState != NULL )
		if( '\\' != realFileName[strlen(realFileName)-1] && '/' != realFileName[strlen(realFileName)-1] )
		{
			sprintf( realFileName, "%s/%s", cwmWorldState->ServerData()->GetLogsDirectory(), filename );
		}
		else
		{
			sprintf( realFileName, "%s%s", cwmWorldState->ServerData()->GetLogsDirectory(), filename );
		} 
	else
		strcpy( realFileName, filename );

	char timeStr[128];
	struct tm *curtime;
	time_t bintime;
	time(&bintime);
	curtime = localtime(&bintime);
	strftime( timeStr, 256, "[%b %d %I:%M:%S %p] ", curtime );

	toWrite.open( realFileName, std::ios::out | std::ios::app );
	if( toWrite.is_open() )
		toWrite << timeStr << msg << std::endl;
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
	if ( !cwmWorldState->ServerData()->GetServerConsoleLogStatus() )
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
	for( int i = 0; i < left; i++ )
		std::cout << " ";
	std::cout << "o";
	for( int j = left + 1; j < width - 1; j++ )
		std::cout << "-";
	std::cout << "o";
	curLeft = 0;
	curTop = 0;
	TurnNormal();
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
#if !defined(__unix__)
	hco = GetStdHandle( STD_OUTPUT_HANDLE );
	GetConsoleScreenBufferInfo( hco, &csbi );
	SetConsoleTitle( temp );
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
#if !defined(__unix__)
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
#if !defined(__unix__)
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
#if !defined(__unix__)
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
#if !defined(__unix__)
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
#if !defined(__unix__)
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
#if !defined(__unix__)
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
#if !defined(__unix__)
	UI32 y;
	COORD xy;
	
	xy.X = 0;
	xy.Y = 0;
	FillConsoleOutputCharacter( hco, ' ', 80 * 25, xy, &y );
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
	case NORMALMODE:	return( (filterSettings&ANDSettings[level]) == ANDSettings[level] );
	case WARNINGMODE:	return( (filterSettings&ANDSettings[5+level]) == ANDSettings[5+level] );
	case ERRORMODE:		return( (filterSettings&ANDSettings[10+level]) == ANDSettings[10+level] );
	case COLOURMODE:	return( (filterSettings&ANDSettings[15]) == ANDSettings[15] );
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

#if !defined(__unix__)
void CConsole::MoveTo( int x, int y )
{
	COORD Pos;
	if ( y == -1 )
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
//|	Function			-	void CConsole::StartOfLineCheck( void )
//|	Date					-	2/3/2003
//|	Developers		-	Maarc
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Reduces excess work in overloaded functions by checking
//|									to see if the start of line has to be done here
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
//|	Function			-	void CConsole::PrintSpecial( UI08 colour, const char *toPrint, ... )
//|	Date					-	2/7/2003
//|	Developers		-	Maarc
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	This is much like PrintFailed, PrintDone and so on except 
//|									we specify the text and the colour
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
		int stringLength = strlen( msg ) + 3;
		MoveTo( 80 - stringLength );
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
