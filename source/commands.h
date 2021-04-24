#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include "cmdtable.h"


const UI32 BIT_STRIPHAIR	= 1;
const UI32 BIT_STRIPITEMS	= 2;

struct commandLevel_st
{
	std::string			name;			// name of level
	UI08				commandLevel;	// upper limit of level
	UI16				defaultPriv;	// default privs associated with it
	UI16				nickColour;		// colour of a person's name
	UI16				allSkillVals;	// if 0, skills left same, if not, all skills set to this value
	UI16				targBody;		// target body value
	UI16				bodyColour;		// target body colour
	std::bitset< 8 >	stripOff;		// strips off hair, beard and clothes
	commandLevel_st() : name( "" ), commandLevel( 0 ), defaultPriv( 0 ), nickColour( 0 ),
	allSkillVals( 0 ), targBody( 0 ), bodyColour( 0 )
	{
		stripOff.reset();
	}
};

class cCommands
{
private:
	std::vector< commandLevel_st *> clearance;
	COMMANDMAP_ITERATOR				cmdPointer;
	TARGETMAP_ITERATOR				targPointer;
	std::string						commandString;

	void			InitClearance( void );
	void			CommandReset( void );
public:
	UI08			NumArguments( void );
	SI32			Argument( UI08 argNum );
	std::string		CommandString( UI08 section, UI08 end = 0 );
	void			CommandString( std::string newValue );

	commandLevel_st *GetClearance( std::string clearName );	// return by command name
	commandLevel_st *GetClearance( UI08 commandLevel );		// return by command level
	UI16			GetColourByLevel( UI08 commandLevel );
	void			Command( CSocket *s, CChar *c, std::string text );
	void			Load( void );
	void			Log( const std::string &command, CChar *player1, CChar *player2, const std::string &extraInfo );

	bool			CommandExists( const std::string& cmdName );
	const std::string FirstCommand( void );
	const std::string NextCommand( void );
	bool			FinishedCommandList( void );

	CommandMapEntry *CommandDetails( const std::string& cmdName );

	cCommands();
	~cCommands();

	void			Register( const std::string &cmdName, UI16 scriptID, UI08 cmdLevel, bool isEnabled );
	void			UnRegister( const std::string &cmdName, cScript *toRegister );
	void			SetCommandStatus( const std::string &cmdName, bool isEnabled );
};

extern cCommands	*Commands;

#endif

