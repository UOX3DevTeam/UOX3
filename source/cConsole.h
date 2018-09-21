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
	CConsole& operator<<( const CBaseObject *outPut );
	CConsole& operator<<( const SI16 &outPut );
	CConsole& operator<<( const UI16 &outPut );
	CConsole& operator<<( const std::string &outPut );
	CConsole& operator<<( const std::ostream& outPut );
	CConsole& operator<<( const size_t &outPut );

	CConsole& operator<<( CBaseObject *outPut );
	CConsole& operator<<( std::ostream& outPut );
	CConsole& operator<<( CEndL& myObj );

	CConsole& operator<<( const R32 &outPut );
	CConsole& operator<<( const R64 &outPut );

	void	Print( const char *toPrint, ... );
	void	Log( const char *toLog, const char *filename, ... );
	void	Log( const char *toLog, ... );
	void	Error( const char *toWrite, ... );
	void	Warning( const char *toWrite, ... );

	UI08	CurrentMode( void ) const;
	void	CurrentMode( UI08 value );

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

	bool	LogEcho( void );
	void	LogEcho( bool value );	
	void	PrintSpecial( UI08 color, const char *toPrint, ... );
	void	Poll( void );
	void	Cloak( char *callback );

	void	RegisterKey( int key, std::string cmdName, UI16 scriptID );
	void	RegisterFunc( const std::string &key, const std::string &cmdName, UI16 scriptID );
	void	SetKeyStatus( int key, bool isEnabled );
	void	SetFuncStatus( const std::string &key, bool isEnabled );
	void	Registration( void );

private:

	struct JSConsoleEntry
	{
		UI16		scriptID;
		bool		isEnabled;
		std::string cmdName;
		JSConsoleEntry() : scriptID( 0 ), isEnabled( true ), cmdName( "" )
		{
		}
		JSConsoleEntry( UI16 id, const std::string &cName ) : scriptID( id ), isEnabled( true ), cmdName( cName )
		{
		}
	};

	typedef std::map< std::string, JSConsoleEntry >				JSCONSOLEFUNCMAP;
	typedef std::map< std::string, JSConsoleEntry >::iterator	JSCONSOLEFUNCMAP_ITERATOR;
	typedef std::map< int, JSConsoleEntry >						JSCONSOLEKEYMAP;
	typedef std::map< int, JSConsoleEntry >::iterator			JSCONSOLEKEYMAP_ITERATOR;

	JSCONSOLEKEYMAP		JSKeyHandler;
	JSCONSOLEFUNCMAP	JSConsoleFunctions;
	UI16				height, width;	// for differing windows
	UI16				curLeft, curTop;
	std::bitset< 16 >	filterSettings;
	UI08				currentMode;
	UI08				previousColour;
	bool				logEcho;
#if UOX_PLATFORM == PLATFORM_WIN32
	HANDLE						hco;
	CONSOLE_SCREEN_BUFFER_INFO	csbi;
#else
	bool			forceNL;
#endif
	void	PrintStartOfLine( void );
	void	StartOfLineCheck(void);
	int		cl_getch( void );
	void	Process( int c );
	void	DisplaySettings( void );
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

