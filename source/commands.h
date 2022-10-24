#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include "cmdtable.h"


const UI32 BIT_STRIPHAIR	= 1;
const UI32 BIT_STRIPITEMS	= 2;

struct CommandLevel_st
{
	std::string			name;			// name of level
	std::string			title;			// Title of level, displayed in front of name
	UI08				commandLevel;	// upper limit of level
	UI16				defaultPriv;	// default privs associated with it
	UI16				nickColour;		// colour of a person's name
	UI16				allSkillVals;	// if 0, skills left same, if not, all skills set to this value
	UI16				targBody;		// target body value
	UI16				bodyColour;		// target body colour
	std::bitset<8>		stripOff;		// strips off hair, beard and clothes
	CommandLevel_st() : name( "" ), title( "" ), commandLevel( 0 ), defaultPriv( 0 ), nickColour( 0 ),
	allSkillVals( 0 ), targBody( 0 ), bodyColour( 0 )
	{
		stripOff.reset();
	}
};

class CCommands
{
private:
	std::vector<CommandLevel_st *>	clearance;
	COMMANDMAP_ITERATOR				cmdPointer;
	TARGETMAP_ITERATOR				targPointer;
	std::string						commandString;

	void			InitClearance();
	void			CommandReset();
public:
	UI08			NumArguments( void );
	SI32			Argument( UI08 argNum );
	std::string		CommandString( UI08 section, UI08 end = 0 );
	void			CommandString( std::string newValue );

	CommandLevel_st *GetClearance( std::string clearName );	// return by command name
	CommandLevel_st *GetClearance( UI08 commandLevel );		// return by command level
	UI16			GetColourByLevel( UI08 commandLevel );
	void			Command( CSocket *s, CChar *c, std::string text, bool checkSocketAccess = false );
	void			Load( void );
	void			Log( const std::string &command, CChar *player1, CChar *player2, const std::string &extraInfo );

	bool			CommandExists( const std::string& cmdName );
	const std::string FirstCommand( void );
	const std::string NextCommand( void );
	bool			FinishedCommandList( void );

	CommandMapEntry_st *CommandDetails( const std::string& cmdName );

	CCommands() = default;
	auto Startup() -> void;
	~CCommands();

	void			Register( const std::string &cmdName, UI16 scriptId, UI08 cmdLevel, bool isEnabled );
	void			UnRegister( const std::string &cmdName, cScript *toRegister );
	void			SetCommandStatus( const std::string &cmdName, bool isEnabled );
};

extern CCommands	*Commands;

#endif

