//	 cmdtable.h - Crackerjack <crackerjack@crackerjack.net> July 24/99
//	This code is an attempt to clean up the messy "if/then/else" routines
//	currently in use for GM commands, as well as adding more functionality
//	and more potential for functionality.
//
//	Current features:
//	- Actual table of commands to execute, what perms are required, dialog
//	  messages for target commands, etc handled by a central system
//
//

#ifndef __CMDTABLE_H
#define __CMDTABLE_H


// Types of commands
enum commandTypes
{
	CMD_TARGET = 0,			// Call target struct specified in cmd_extra
	CMD_FUNC,			// Call function specified in cmd_extra must be of type GMFUNC
	CMD_SOCKFUNC,		// Call function specified in cmd_extra with a socket
	CMD_TARGETXYZ,		// target with addx & y & z [] arguments
	CMD_TARGETINT,		// target with tempint
	CMD_TARGETTXT,		// target with XText
};

struct CommandMapEntry_st
{
	UI08			cmdLevelReq;
	UI08			cmdType;
	void			( *cmd_extra ) ();	// executable function
	CommandMapEntry_st() : cmdLevelReq( 0 ), cmdType( CMD_SOCKFUNC ), cmd_extra( nullptr )
	{
	}
	CommandMapEntry_st( UI08 cLR, UI08 cT, void (*ce)() ) : cmdLevelReq( cLR ), cmdType( cT ), cmd_extra( ce )
	{
	}
};

struct TargetMapEntry_st
{
	UI08			cmdLevelReq;
	UI08			cmdType;
	TargetIds		targId;
	SI32			dictEntry;
	TargetMapEntry_st() : cmdLevelReq( 0 ), cmdType( CMD_TARGET ), targId( TARGET_NOFUNC ), dictEntry( 0 )
	{
	}
	TargetMapEntry_st( UI08 cLR, UI08 cT, TargetIds tID, SI32 dE ) : cmdLevelReq( cLR ), cmdType( cT ), targId( tID ), dictEntry( dE )
	{
	}
};

struct JSCommandEntry_st
{
	UI08		cmdLevelReq;
	UI16		scriptId;
	bool		isEnabled;
	JSCommandEntry_st() : cmdLevelReq( 0 ), scriptId( 0 ), isEnabled( true )
	{
	}
	JSCommandEntry_st( UI08 cLR, UI16 id, bool iE ) : cmdLevelReq( cLR ), scriptId( id ), isEnabled( iE )
	{
	}
};


typedef std::map<std::string, CommandMapEntry_st>			COMMANDMAP;
typedef std::map<std::string, CommandMapEntry_st>::iterator	COMMANDMAP_ITERATOR;
extern	COMMANDMAP											CommandMap;

#define CMD_EXEC		void (*) ( void )
#define CMD_SOCKEXEC	void (*) ( CSocket * )
#define CMD_DEFINE		void (*)()

typedef std::map<std::string, TargetMapEntry_st>			TARGETMAP;
typedef std::map<std::string, TargetMapEntry_st>::iterator	TARGETMAP_ITERATOR;
extern	TARGETMAP											TargetMap;

typedef std::map<std::string, JSCommandEntry_st>			JSCOMMANDMAP;
typedef std::map<std::string, JSCommandEntry_st>::iterator	JSCOMMANDMAP_ITERATOR;
extern JSCOMMANDMAP											JSCommandMap;

#endif // __CMDTABLE_H
