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

#include "uox3.h"
// Types of commands
const UI08 CMD_ITEMMENU		= 1;	// Open item menu specified in cmd_extra
const UI08 CMD_TARGET		= 2;	// Call target struct specified in cmd_extra
const UI08 CMD_FUNC			= 3;	// Call function specified in cmd_extra must be of type GMFUNC
const UI08 CMD_TARGETX		= 4;	// target with addx[] argument
const UI08 CMD_TARGETXY		= 5;	// target with addx & y [] arguments
const UI08 CMD_TARGETXYZ	= 6;	// target with addx & y & z [] arguments
const UI08 CMD_TARGETHX		= 7;	// target with hex addx[] argument
const UI08 CMD_TARGETHXY	= 8;	// target with hex addx & y arguments
const UI08 CMD_TARGETHXYZ	= 9;	// target with hex addx & y & z arguments
const UI08 CMD_TARGETID1	= 10;	// target with 1 addid #
const UI08 CMD_TARGETID2	= 11;	// target with 2 addid #s
const UI08 CMD_TARGETID3	= 12;	// target with 3 addid #s
const UI08 CMD_TARGETID4	= 13;	// target with 4 addid #s
const UI08 CMD_TARGETHID1	= 14;	// target with 1 hex addid #
const UI08 CMD_TARGETHID2	= 15;	// target with 2 hex addid #s
const UI08 CMD_TARGETHID3	= 16;	// target with 3 hex addid #s
const UI08 CMD_TARGETHID4	= 17;	// target with 4 hexaddid #s
const UI08 CMD_TARGETTMP	= 18;	// target with tempint
const UI08 CMD_TARGETHTMP	= 19;	// target with hex tempint

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
typedef std::map< std::string, cmdtable_mapentry >	CmdTableDataType;
typedef CmdTableDataType::iterator			CmdTableIterator;
extern	CmdTableDataType					cmd_table;

#define CMD_EXEC	void (*) ( cSocket * )
#define CMD_DEFINE	void (*)()

typedef struct target_s TARGET_S;
struct target_s 
{	// arguments to the target() function
	UI08 a1, a2, a3;
	int a4;	// Leave this like this for a bit abaddon -EviLDeD
	int		dictEntry;
};

// Defined commands that are just being mapped to internal functions
#define command_time telltime


// All command_ functions take an int value of the player that triggered the command.
#define CMD_HANDLER( name ) extern void name ( cSocket * )
#define TAR_HANDLER( name ) extern TARGET_S name


// All defined commands
// A
CMD_HANDLER( command_add );
CMD_HANDLER( command_addx );
CMD_HANDLER( command_allmoveon );
CMD_HANDLER( command_allmoveoff );
CMD_HANDLER( command_additem );
CMD_HANDLER( command_areaCommand );
CMD_HANDLER( command_action );
CMD_HANDLER( command_announceon );
CMD_HANDLER( command_announceoff );
CMD_HANDLER( command_addnpc );
CMD_HANDLER( command_addcharacter );
CMD_HANDLER( command_addaccount );
// B
CMD_HANDLER( command_brightlight );
// C
CMD_HANDLER( command_cleanup );
CMD_HANDLER( command_clear );
CMD_HANDLER( command_command );
CMD_HANDLER( command_cq );
CMD_HANDLER( command_cnext );
CMD_HANDLER( command_cclear );
CMD_HANDLER( command_cachestats );
CMD_HANDLER( command_cy );
// D
CMD_HANDLER( command_dye );
CMD_HANDLER( command_dupe );
CMD_HANDLER( command_disconnect );
CMD_HANDLER( command_dungeonlight );
CMD_HANDLER( command_darklight );
CMD_HANDLER( command_decay );
CMD_HANDLER( command_delid );
// E
// F
CMD_HANDLER( command_fix );
CMD_HANDLER( command_forcewho );
// G
CMD_HANDLER( command_gms );
CMD_HANDLER( command_gmtransfer );
CMD_HANDLER( command_gotocur );
CMD_HANDLER( command_gpost );
CMD_HANDLER( command_goplace );
CMD_HANDLER( command_gochar );
CMD_HANDLER( command_go );
CMD_HANDLER( command_gcollect );
CMD_HANDLER( command_gmmenu );
CMD_HANDLER( command_gumpmenu );
CMD_HANDLER( command_gmopen );
CMD_HANDLER( command_gumpopen );
CMD_HANDLER( command_guardson );
CMD_HANDLER( command_guardsoff );
CMD_HANDLER( command_getlight );
CMD_HANDLER( command_gy );
// H
CMD_HANDLER( command_hidehs );
CMD_HANDLER( command_howto );
// I
CMD_HANDLER( command_iwipe );
CMD_HANDLER( command_itemmenu );
CMD_HANDLER( command_invul );
// J
// K
CMD_HANDLER( command_killall );
// L
CMD_HANDLER( command_lpost );
CMD_HANDLER( command_light );
CMD_HANDLER( command_loaddefaults );
// M
CMD_HANDLER( command_midi );
CMD_HANDLER( command_minecheck );
CMD_HANDLER( command_make );
// N
CMD_HANDLER( command_next );
CMD_HANDLER( command_noinvul );
CMD_HANDLER( command_nodecay );
CMD_HANDLER( command_npcrect );
CMD_HANDLER( command_npccircle );
CMD_HANDLER( command_npcwander );
CMD_HANDLER( command_nacct );
// O
// P
CMD_HANDLER( command_post );
CMD_HANDLER( command_poly );
CMD_HANDLER( command_pdump );
// Q
CMD_HANDLER( command_q );
// R
CMD_HANDLER( command_regspawnall );
CMD_HANDLER( command_resend );
CMD_HANDLER( command_rpost );
CMD_HANDLER( command_rename );
CMD_HANDLER( command_restock );
CMD_HANDLER( command_restockall );
CMD_HANDLER( command_respawn );
CMD_HANDLER( command_regspawnmax );
CMD_HANDLER( command_regspawn );
CMD_HANDLER( command_reloadserver );
CMD_HANDLER( command_reloadaccounts );
CMD_HANDLER( command_rename );
CMD_HANDLER( command_readspawnregions );
CMD_HANDLER( command_reportbug );
CMD_HANDLER( command_readini );
CMD_HANDLER( command_reloaddefs );
CMD_HANDLER( command_rename2 );
// S
CMD_HANDLER( command_status );
CMD_HANDLER( command_showids );
CMD_HANDLER( command_skin );
CMD_HANDLER( command_save );
CMD_HANDLER( command_showtime );
CMD_HANDLER( command_setpriv );
CMD_HANDLER( command_settime );
CMD_HANDLER( command_shutdown );
CMD_HANDLER( command_setshoprestockrate );
CMD_HANDLER( command_showhs );
CMD_HANDLER( command_set );
CMD_HANDLER( command_squelch );
CMD_HANDLER( command_squelch );
CMD_HANDLER( command_spawnkill );
CMD_HANDLER( command_setrace );
CMD_HANDLER( command_sfx );
CMD_HANDLER( command_secondsperuominute );
CMD_HANDLER( command_sgy );
// T
CMD_HANDLER( command_teleport );
CMD_HANDLER( command_tile );
CMD_HANDLER( command_title );
CMD_HANDLER( command_tell );
CMD_HANDLER( command_tilew );
CMD_HANDLER( command_time );
CMD_HANDLER( command_temp );
// U
// V
CMD_HANDLER( command_validcmd );
// W
CMD_HANDLER( command_who );
CMD_HANDLER( command_wipenpcs );
CMD_HANDLER( command_where );
CMD_HANDLER( command_wipe );
CMD_HANDLER( command_wholist );
CMD_HANDLER( command_wf );
// X
CMD_HANDLER( command_xgoplace );
CMD_HANDLER( command_xtele );
CMD_HANDLER( command_xgate );
// Y
// Z
CMD_HANDLER( command_zerokills );

// all defined target commands
// A
// B
TAR_HANDLER( target_bolt );
TAR_HANDLER( target_ban );
TAR_HANDLER( target_buy );
// C
TAR_HANDLER( target_cstats );
TAR_HANDLER( target_ctrig );
TAR_HANDLER( target_commandlevel );
// D
TAR_HANDLER( target_devinelock );
TAR_HANDLER( target_devineunlock );
TAR_HANDLER( target_deletechar );
// E
// F
TAR_HANDLER( target_freeze );
TAR_HANDLER( target_fullstats );
// G
TAR_HANDLER( target_gate );
TAR_HANDLER( target_glow );
// H
TAR_HANDLER( target_heal );
TAR_HANDLER( target_hide );
// I
TAR_HANDLER( target_istats );
TAR_HANDLER( target_itrig );
TAR_HANDLER( target_incx );
TAR_HANDLER( target_incy );
TAR_HANDLER( target_incz );
// J
// K
TAR_HANDLER( target_killhair );
TAR_HANDLER( target_killbeard );
TAR_HANDLER( target_killpack );
TAR_HANDLER( target_kill );
TAR_HANDLER( target_kick );
// L
// M
TAR_HANDLER( target_movetobag );
TAR_HANDLER( target_mark );
TAR_HANDLER( target_mana );
TAR_HANDLER( target_makeshop );
// N
TAR_HANDLER( target_newz );
TAR_HANDLER( target_npcaction );
TAR_HANDLER( target_npctarget );
TAR_HANDLER( target_newx );
TAR_HANDLER( target_newy );
// O
// P
TAR_HANDLER( target_possess );
// Q
TAR_HANDLER( target_jail );
// R
TAR_HANDLER( target_recall );
TAR_HANDLER( target_removeshop );	// allows us to remove the shopkeeper layers for an npc/pc
TAR_HANDLER( target_resurrect );
TAR_HANDLER( target_remove );
TAR_HANDLER( target_release );
// S
TAR_HANDLER( target_setfont );
TAR_HANDLER( target_setmorex );
TAR_HANDLER( target_setmorey );
TAR_HANDLER( target_setmorez );
TAR_HANDLER( target_setmorexyz );
TAR_HANDLER( target_sethexmorexyz );
TAR_HANDLER( target_setnpcai );
TAR_HANDLER( target_settype );
TAR_HANDLER( target_setid );
TAR_HANDLER( target_setmore );
TAR_HANDLER( target_setamount );
TAR_HANDLER( target_setmovable );
TAR_HANDLER( target_setvisible );
TAR_HANDLER( target_setdir );
TAR_HANDLER( target_setspeech );
TAR_HANDLER( target_setowner );
TAR_HANDLER( target_stamina );
TAR_HANDLER( target_setvalue );
TAR_HANDLER( target_setrestock );
TAR_HANDLER( target_sell );
TAR_HANDLER( target_setspattack );
TAR_HANDLER( target_setspadelay );
TAR_HANDLER( target_setpoison );
TAR_HANDLER( target_setpoisoned );
TAR_HANDLER( target_setadvobj );
TAR_HANDLER( target_setwipe );
TAR_HANDLER( target_split );
TAR_HANDLER( target_splitchance );
TAR_HANDLER( target_showskills );
TAR_HANDLER( target_showdetail );
TAR_HANDLER( target_setscripttrigger );
// T
TAR_HANDLER( target_tele );
TAR_HANDLER( target_ttrig );
TAR_HANDLER( target_trainer );
TAR_HANDLER( target_tiledata );
TAR_HANDLER( target_tweak );
TAR_HANDLER( target_telestuff );
// U
TAR_HANDLER( target_unglow );	
TAR_HANDLER( target_unhide );
TAR_HANDLER( target_unfreeze );
TAR_HANDLER( target_use );
// V
// W
// X
TAR_HANDLER( target_xgo );
TAR_HANDLER( target_xbank );
TAR_HANDLER( target_xsbank );
// Y
// Z

void HandleHowTo( cSocket *sock, int cmdNumber );

#endif // __CMDTABLE_H
