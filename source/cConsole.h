#ifndef __CCONSOLE__
#define __CCONSOLE__

// In essence, CConsole is our nice generic class for outputting display
// As long as the interface is kept the same, we can display whatever we wish to
// We store a set of coordinates being the window corner and size, for systems with windowing support, which describes the window we are in

// NOTE: I wish Error, and Log, to subclass from CConsole, so we have multiple streams that we can write to, in essence
//#pragma note( "I wish Error, and Log, to subclass from CConsole, so we have multiple streams that we can write to, in essence" )

namespace UOX
{

#define MAX_CONSOLE_BUFF 512
#if !defined(__unix__)
#define vsnprintf _vsnprintf
#endif

const UI08 CNORMAL	= 0;
const UI08 CBLUE	= 1;
const UI08 CRED		= 2;
const UI08 CGREEN	= 3;
const UI08 CYELLOW	= 4;
const UI08 CBWHITE	= 5;

class CConsole
{
public:
	CConsole();
	~CConsole();

	void Flush( void )
	{
		std::cout.flush();
	}

	CConsole& operator<<( const SI08 *outPut );
	CConsole& operator<<( const char *outPut );
	CConsole& operator<<( const UI08 *outPut );
	CConsole& operator<<( const SI32 &outPut );
	CConsole& operator<<( const UI32 &outPut );
	CConsole& operator<<( const SI08 &outPut );
	CConsole& operator<<( const UI08 &outPut );
	CConsole& operator<<( const cBaseObject *outPut );
	CConsole& operator<<( const SI16 &outPut );
	CConsole& operator<<( const UI16 &outPut );
	CConsole& operator<<( const std::string &outPut );
	CConsole& operator<<( const std::ostream& outPut );
	CConsole& operator<<( const size_t &outPut );

	CConsole& operator<<( cBaseObject *outPut );
	CConsole& operator<<( std::ostream& outPut );
	CConsole& operator<<( CEndL& myObj );

	CConsole& operator<<( const R32 &outPut );
	CConsole& operator<<( const R64 &outPut );

	void	Print( const char *toPrint, ... );
	void	Log( const char *toLog, const char *filename, ... );
	void	Log( const char *toLog, ... );
	void	Error( UI08 level, const char *toWrite, ... );
	void	Warning( UI08 level, const char *toWrite, ... );

	UI16	Left( void ) const;
	UI16	Top( void ) const;
	UI16	Height( void ) const;
	UI16	Width( void ) const;
	bool	CanPrint( UI08 type, UI08 level ) const;
	UI16	FilterSetting( void ) const;
	UI08	CurrentMode( void ) const;
	UI08	CurrentLevel( void ) const;

	void	Left( UI16 nVal );
	void	Top( UI16 nVal );
	void	Height( UI16 nVal );
	void	Width( UI16 nVal );
	void	FilterBit( UI08 type, UI08 level, bool status );
	void	FilterSetting( UI16 value );
	void	CurrentMode( UI08 value );
	void	CurrentLevel( UI08 value );

	void	PrintSectionBegin( void );

	void	TurnYellow( void );
	void	TurnRed( void );
	void	TurnGreen( void );
	void	TurnBlue( void );
	void	TurnNormal( void );
	void	TurnBrightWhite( void );
	void	PrintDone( void );
	void	PrintFailed( void );
	void	PrintPassed( void );

	void	ClearScreen( void );
	void	Start( char *temp );
	void	PrintBasedOnVal( bool value );
	void	MoveTo( int x, int y = -1 );//y=-1 will move on the current line

	bool	LogEcho(void);
	void	LogEcho(bool value);	
	void	PrintSpecial(UI08 color, const char *toPrint,...);
protected:
	UI16	left, top, height, width;	// for differing windows
	UI16	curLeft, curTop;
	UI16	filterSettings;
	UI08	currentMode, currentLevel;
	UI08	previousColour;
	bool	logEcho;
#if !defined(__unix__)
	HANDLE						hco;
	CONSOLE_SCREEN_BUFFER_INFO	csbi;
#endif
	void	PrintStartOfLine( void );
	void	StartOfLineCheck(void);
};

class CEndL
{
public:
	void Action( CConsole& test )
	{
		test << "\n"; test.Flush();
	}
private:

};

extern CConsole								Console;
extern CEndL								myendl;

}

#endif

