#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include "cmdtable.h"

class cCommands
{
private:
	std::vector< commandLevel_st *> clearance;
	void InitClearance( void );
	CmdTableIterator	cmdPointer;

public:
	commandLevel_st *GetClearance( const char *clearName );			// return by command name
	commandLevel_st *GetClearance( UI08 commandLevel );	// return by command level
	UI16			GetColourByLevel( UI08 commandLevel );
	void			showQue( cSocket *s, bool isGM );
	void			nextCall( cSocket *s, bool isGM );
	void			closeCall( cSocket *s, bool isGM );
	void			KillSpawn( cSocket *s, int r );
	void			RegSpawnMax( cSocket *s, cSpawnRegion *spawnReg );
	void			RegSpawnNum( cSocket *s, cSpawnRegion *spawnReg, int n );
	void			KillAll( cSocket *s, int percent, const char * sysmsg );
	void			AddHere( cSocket *s, SI08 z );
	void			Wipe( cSocket *s );
	void			CPage( cSocket *s, const char * reason );
	void			GMPage( cSocket *s, const char * reason );
	void			MakePlace( cSocket *s, int i );
	void			Command( cSocket *s );
	void			MakeShop( CChar *c );
	void			RemoveShop( cSocket *s );
	void			DyeTarget( cSocket *s );
	CItem *			DupeItem( cSocket *s, CItem *i, SI32 amount );
	void			Possess( cSocket *s );
	void			Load( void );
	int				cmd_offset;
	void			Log( char *command, CChar *player1, CChar *player2, char *extraInfo );

	bool			CommandExists( char *cmdName );
	const char *	FirstCommand( void );
	const char *	NextCommand( void );
	bool			FinishedCommandList( void );

	cmdtable_mapentry *CommandDetails( char *cmdName );

	cCommands();
};

#endif

