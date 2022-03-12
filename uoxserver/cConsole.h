#ifndef __CCONSOLE__
#define __CCONSOLE__

// In essence, CConsole is our nice generic class for outputting display
// As long as the interface is kept the same, we can display whatever we wish to
// We store a set of coordinates being the window corner and size, for systems with windowing support, which describes the window we are in
#include <tuple>


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
	CConsole& operator<<( const SI08 &outPut );
	CConsole& operator<<( const UI08 &outPut );
	CConsole& operator<<( const SI16 &outPut );
	CConsole& operator<<( const UI16 &outPut );
	CConsole& operator<<( const SI32 &outPut );
	CConsole& operator<<( const UI32 &outPut );
	CConsole& operator<<( const UI64 &outPut );
	CConsole& operator<<( const SI64 &outPut );
	CConsole& operator<<( const CBaseObject *outPut );
	CConsole& operator<<( const std::string &outPut );


	CConsole& operator<<( CBaseObject *outPut );
	//CConsole& operator<<( std::ostream& outPut );
	CConsole& operator<<( CEndL& myObj );

	CConsole& operator<<( const R32 &outPut );
	CConsole& operator<<( const R64 &outPut );


	void    print(const std::string& toPrint);
	void	log( const std::string& msg, const std::string& filename);
	void	log( const std::string& msg );
	void	error( const std::string& msg);
	void	warning( const std::string& msg );

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
	void	Start( const std::string& temp );
	void	PrintBasedOnVal( bool value );
	void	MoveTo( SI32 x, SI32 y = -1 );//y=-1 will move on the current line

	bool	LogEcho( void );
	void	LogEcho( bool value );
	void	PrintSpecial( UI08 color, const std::string& msg );
	void	Poll( void );

	void	RegisterKey( SI32 key, std::string cmdName, UI16 scriptID );
	void	RegisterFunc( const std::string &key, const std::string &cmdName, UI16 scriptID );
	void	SetKeyStatus( SI32 key, bool isEnabled );
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
	typedef std::map< SI32, JSConsoleEntry >					JSCONSOLEKEYMAP;
	typedef std::map< SI32, JSConsoleEntry >::iterator			JSCONSOLEKEYMAP_ITERATOR;

	JSCONSOLEKEYMAP		JSKeyHandler;
	JSCONSOLEFUNCMAP	JSConsoleFunctions;
	UI16				height, width;	// for differing windows
	UI16				curLeft, curTop;
	std::bitset< 16 >	filterSettings;
	UI08				currentMode;
	UI08				previousColour;
	bool				logEcho;

	void				initialize();
	void				reset();
	std::tuple<int,int>	windowSize();
	void				clearScreen();
	void				setTitle(const std::string& title);
	CConsole& 			sendCMD(const std::string& cmd);

	void	PrintStartOfLine( void );
	void	StartOfLineCheck(void);
	SI32		cl_getch( void );
	void	Process( SI32 c );
	void	DisplaySettings( void );

	const std::string 	ESC 		= "\x1b";
	const std::string 	CSI 		= ESC + "[";
	const std::string   BEL			= "\x07";
	//  Command sequences
	const std::string   ATTRIBUTE	= CSI +"#m";  // find/replace on # with attribute number
	const std::string	MOVE		= CSI + "ROW;COLH"; // we are going to find/replace on ROW/COL
	const std::string	HORIZMOVE	= CSI + "COLG";
	const std::string	VERTMOVE	= CSI + "ROWd";
	const std::string	OFFCURSOR	= CSI + "?25l";
	const std::string	ONCURSOR	= CSI + "?25h";

	const std::string	ONGRAPHIC	= ESC + "(0";
	const std::string	OFFGRAPHIC	= ESC + "(B";

	const std::string	SETTITLE	= ESC + "]2;TITLE" + BEL;

	const std::string	CLEAR		= CSI +"2J";
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

#endif

