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
	CMD_ITEMMENU = 1,	// Open item menu specified in cmd_extra
	CMD_TARGET,			// Call target struct specified in cmd_extra
	CMD_FUNC,			// Call function specified in cmd_extra must be of type GMFUNC
	CMD_TARGETX,		// target with addx[] argument
	CMD_TARGETXY,		// target with addx & y [] arguments
	CMD_TARGETXYZ,		// target with addx & y & z [] arguments
	CMD_TARGETID1,		// target with 1 addid #
	CMD_TARGETID2,		// target with 2 addid #s
	CMD_TARGETID3,		// target with 3 addid #s
	CMD_TARGETID4,		// target with 4 addid #s
	CMD_TARGETTMP,		// target with tempint
};

struct cmdtable_entry
{
	const char *	cmd_name;
	UI32			cmdLevelReq;
	UI32			cmdType;
	void			(*cmd_extra) ();	// executable function
};
struct cmdtable_mapentry
{
	UI32	cmdLevelReq;
	UI32	cmdType;
	void	(*cmd_extra) ();	// executable function
	cmdtable_mapentry() : cmdLevelReq( 0 ), cmdType( CMD_FUNC ), cmd_extra( NULL ) { }
	cmdtable_mapentry( UI32 cLR, UI32 cT, void (*ce)() ) : cmdLevelReq( cLR ), cmdType( cT ), cmd_extra( ce ) { }
};

struct targtable_entry {
    const char		*name;
	UI08			cmdLevelReq;
	UI08			cmdType;
    UI08			targID;
    SI32			dictEntry;
};
struct targtable_mapentry {
	UI08			cmdLevelReq;
	UI08			cmdType;
    UI08			targID;
    SI32			dictEntry;
	targtable_mapentry() : cmdLevelReq( 0 ), cmdType( CMD_TARGET ), targID( 0 ), dictEntry( 0 ) { }
	targtable_mapentry( UI08 cLR, UI08 cT, UI08 tID, SI32 dE ) : cmdLevelReq( cLR ), cmdType( cT ), targID( tID ), dictEntry( dE ) { }
};

typedef std::map< std::string, cmdtable_mapentry >	CmdTableDataType;
typedef CmdTableDataType::iterator			CmdTableIterator;
extern	CmdTableDataType					cmd_table;

#define CMD_EXEC	void (*) ( cSocket * )
#define CMD_DEFINE	void (*)()

typedef std::map< std::string, targtable_mapentry >	TargetTableDataType;
typedef TargetTableDataType::iterator			TargetTableIterator;
extern	TargetTableDataType					targ_table;

// Defined commands that are just being mapped to internal functions
#define command_time telltime

typedef void (CommandFunc)( cSocket *s );
// All defined commands
// A
CommandFunc command_add;
CommandFunc command_addx;
CommandFunc command_allmoveon;
CommandFunc command_allmoveoff;
CommandFunc command_additem;
CommandFunc command_areaCommand;
CommandFunc command_action;
CommandFunc command_announceon;
CommandFunc command_announceoff;
CommandFunc command_addnpc;
CommandFunc command_addcharacter;
CommandFunc command_addaccount;
// B
CommandFunc command_brightlight;
// C
CommandFunc command_cleanup;
CommandFunc command_clear;
CommandFunc command_command;
CommandFunc command_cq;
CommandFunc command_cnext;
CommandFunc command_cclear;
CommandFunc command_cachestats;
CommandFunc command_cy;
// D
CommandFunc command_dye;
CommandFunc command_dupe;
CommandFunc command_disconnect;
CommandFunc command_dungeonlight;
CommandFunc command_darklight;
CommandFunc command_decay;
CommandFunc command_delid;
// E
// F
CommandFunc command_fix;
CommandFunc command_forcewho;
// G
CommandFunc command_gms;
CommandFunc command_gmtransfer;
CommandFunc command_gotocur;
CommandFunc command_gpost;
CommandFunc command_goplace;
CommandFunc command_gochar;
CommandFunc command_go;
CommandFunc command_gcollect;
CommandFunc command_gmmenu;
CommandFunc command_gumpmenu;
CommandFunc command_gmopen;
CommandFunc command_gumpopen;
CommandFunc command_guardson;
CommandFunc command_guardsoff;
CommandFunc command_getlight;
CommandFunc command_gy;
// H
CommandFunc command_hidehs;
CommandFunc command_howto;
// I
CommandFunc command_iwipe;
CommandFunc command_itemmenu;
CommandFunc command_invul;
// J
// K
CommandFunc command_killall;
// L
CommandFunc command_lpost;
CommandFunc command_light;
CommandFunc command_loaddefaults;
// M
CommandFunc command_midi;
CommandFunc command_minecheck;
CommandFunc command_make;
// N
CommandFunc command_next;
CommandFunc command_noinvul;
CommandFunc command_nodecay;
CommandFunc command_npcrect;
CommandFunc command_npccircle;
CommandFunc command_npcwander;
CommandFunc command_nacct;
// O
// P
CommandFunc command_post;
CommandFunc command_poly;
CommandFunc command_pdump;
// Q
CommandFunc command_q;
// R
CommandFunc command_regspawnall;
CommandFunc command_resend;
CommandFunc command_rpost;
CommandFunc command_rename;
CommandFunc command_restock;
CommandFunc command_restockall;
CommandFunc command_respawn;
CommandFunc command_regspawnmax;
CommandFunc command_regspawn;
CommandFunc command_reloadserver;
CommandFunc command_reloadaccounts;
CommandFunc command_readspawnregions;
CommandFunc command_reportbug;
CommandFunc command_readini;
CommandFunc command_reloaddefs;
CommandFunc command_rename2;
// S
CommandFunc command_status;
CommandFunc command_showids;
CommandFunc command_skin;
CommandFunc command_save;
CommandFunc command_showtime;
CommandFunc command_setpriv;
CommandFunc command_settime;
CommandFunc command_shutdown;
CommandFunc command_setshoprestockrate;
CommandFunc command_showhs;
CommandFunc command_set;
CommandFunc command_squelch;
CommandFunc command_spawnkill;
CommandFunc command_setrace;
CommandFunc command_sfx;
CommandFunc command_secondsperuominute;
CommandFunc command_sgy;
// T
CommandFunc command_teleport;
CommandFunc command_tile;
CommandFunc command_time;
CommandFunc command_title;
CommandFunc command_tell;
CommandFunc command_tilew;
CommandFunc command_temp;
// U
// V
CommandFunc command_validcmd;
// W
CommandFunc command_who;
CommandFunc command_wipenpcs;
CommandFunc command_where;
CommandFunc command_wipe;
CommandFunc command_wholist;
CommandFunc command_wf;
// X
CommandFunc command_xgoplace;
CommandFunc command_xtele;
CommandFunc command_xgate;
// Y
// Z
CommandFunc command_zerokills;

void HandleHowTo( cSocket *sock, int cmdNumber );

#endif // __CMDTABLE_H
