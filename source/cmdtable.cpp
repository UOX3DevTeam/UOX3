//
//	This code is an attempt to clean up the messy "if/then/else" routines
//	currently in use for GM commands, as well as adding more functionality
//	and more potential for functionality.
//	
//	Current features:
//	- Actual table of commands to execute, what perms are required, dialog
//	messages for target commands, etc handled by a central system
//	
//	

#include "uox3.h"
#include "cmdtable.h"
#include "mstring.h"
#include "msgboard.h"
#include "townregion.h"
#include "cWeather.hpp"
#include "cRaces.h"
#include "cServerDefinitions.h"
#include "commands.h"
#include "wholist.h"
#include "cSpawnRegion.h"
#include "targeting.h"
#include "PageVector.h"
#include "speech.h"
#include "cHTMLSystem.h"
#include "gump.h"
#include "mapstuff.h"
#include "cEffects.h"
#include "network.h"

std::map< std::string, cmdtable_mapentry > cmd_table;
std::map< std::string, targtable_mapentry > targ_table;

void cCommands::CommandReset( void )
{
	int iCounter = 0;
	// { Command Name, Required Command Level, Command Type, Target ID, Dictionary Entry },
	targtable_entry target_table[] =
	{
		// A
		// B
		{ "BAN",			GM_CMDLEVEL,	CMD_TARGET,		235,	253 },
		{ "BOLT",			GM_CMDLEVEL,	CMD_TARGET,		22,		195 },
		{ "BUY",			GM_CMDLEVEL,	CMD_TARGET,		121,	233 },
		// C
		{"COMMANDLEVEL",	GM_CMDLEVEL,	CMD_TARGETX,	104,	264 },
		{"CTRIG",			GM_CMDLEVEL,	CMD_TARGETX,	201,	208 },
		{"CSTATS",			CNS_CMDLEVEL,	CMD_TARGET,		13,		183 },
		// D
		{"DEVINELOCK",		ADMIN_CMDLEVEL,  CMD_TARGET,	68,		265 },
		{"DEVINEUNLOCK",	ADMIN_CMDLEVEL,  CMD_TARGET,	69,		266 },
		{"DELETECHAR",		GM_CMDLEVEL,	CMD_TARGET,		72,		1618 },
		// E
		// F
		{"FREEZE",			GM_CMDLEVEL,	CMD_TARGET,		34,		221 },
		{"FULLSTATS",		GM_CMDLEVEL,	CMD_TARGET,		151,	243 },
		// G
		{"GLOW",			GM_CMDLEVEL,	CMD_TARGET,		255,	258 },
		{"GATE",			GM_CMDLEVEL,	CMD_TARGET,		43,		226 },
		// H
		{"HEAL",			GM_CMDLEVEL,	CMD_TARGET,		44,		227 },
		{"HIDE",			GM_CMDLEVEL,	CMD_TARGET,		131,	244 },
		// I
		{"INCX",			GM_CMDLEVEL,	CMD_TARGETX,	253,	254 },
		{"INCY",			GM_CMDLEVEL,	CMD_TARGETX,	254,	254 },
		{"INCZ",			GM_CMDLEVEL,	CMD_TARGETX,	250,	268 },
		{"ITRIG",			GM_CMDLEVEL,	CMD_TARGETX,	200,	207 },
		{"ISTATS",			CNS_CMDLEVEL,	CMD_TARGET,		12,		182 },
		// J
		{"JAIL",			CNS_CMDLEVEL,	CMD_TARGET,		126,	180 },
		// K
		{"KICK",			GM_CMDLEVEL,	CMD_TARGET,		25,		196 },
		{"KILLHAIR",		GM_CMDLEVEL,	CMD_TARGET,		16,		191 },
		{"KILLBEARD",		GM_CMDLEVEL,	CMD_TARGET,		17,		192 },
		{"KILLPACK",		GM_CMDLEVEL,	CMD_TARGET,		18,		251 },
		{"KILL",			GM_CMDLEVEL,	CMD_TARGET,		20,		193 },
		// L
		// M
		{"MAKESHOP",		GM_CMDLEVEL,	CMD_TARGET,		116,	232 },
		{"MANA",			GM_CMDLEVEL,	CMD_TARGET,		113,	230 },
		{"MARK",			GM_CMDLEVEL,	CMD_TARGET,		39,		225 },
		{"MOVETOBAG",		GM_CMDLEVEL,	CMD_TARGET,		111,	197 },
		// N
		{"NEWX",			GM_CMDLEVEL,	CMD_TARGETX,	251,	254 },
		{"NEWY",			GM_CMDLEVEL,	CMD_TARGETX,	252,	254 },
		{"NEWZ",			GM_CMDLEVEL,	CMD_TARGETX,	5,		205 },
		{"NPCACTION",		GM_CMDLEVEL,	CMD_TARGETID1,	53,		213 },
		{"NPCTARGET",		GM_CMDLEVEL,	CMD_TARGET,		56,		228 },
		// O
		// P
		{"POSSESS",			GM_CMDLEVEL,	CMD_TARGET,		212,	249 },
		// Q
		// R
		{"RECALL",			GM_CMDLEVEL,	CMD_TARGET,		38,		224 },			// these need to be updated for new magic system (Abaddon)
		{"RELEASE",			CNS_CMDLEVEL,	CMD_TARGET,		127,	181 },
		{"REMOVE",			GM_CMDLEVEL,	CMD_TARGET,		3,		188 },
		{"RESURRECT",		GM_CMDLEVEL,	CMD_TARGET,		21,		194 },
		{"REMOVESHOP",		GM_CMDLEVEL,	CMD_TARGET,		105,	262 },
		// S
		{"SETSCPTRIG",		ADMIN_CMDLEVEL,	CMD_TARGETX,	71,		267 },
		{"SHOWSKILLS",		GM_CMDLEVEL,	CMD_TARGETX,	247,	260 },
		{"SHOWDETAIL",		GM_CMDLEVEL,	CMD_TARGET,		48,		261 },
		{"SPLIT",			GM_CMDLEVEL,	CMD_TARGETTMP,	209,	247 },
		{"SPLITCHANCE",		GM_CMDLEVEL,	CMD_TARGETTMP,	210,	248 },
		{"SETSPATTACK",		GM_CMDLEVEL,	CMD_TARGETTMP,	150,	237 },
		{"SETSPADELAY",		GM_CMDLEVEL,	CMD_TARGETTMP,	177,	238 },
		{"SETPOISON",		GM_CMDLEVEL,	CMD_TARGETTMP,	175,	239 },
		{"SETPOISONED",		GM_CMDLEVEL,	CMD_TARGETTMP,	176,	240 },
		{"SETADVOBJ",		GM_CMDLEVEL,	CMD_TARGETTMP,	178,	241 },
		{"SETWIPE",			GM_CMDLEVEL,	CMD_TARGETID1,	133,	242 },
		{"SELL",			GM_CMDLEVEL,	CMD_TARGET,		112,	236 },
		{"SETVALUE",		GM_CMDLEVEL,	CMD_TARGETX,	122,	234 },
		{"SETRESTOCK",		GM_CMDLEVEL,	CMD_TARGETX,	123,	235 },
		{"STAMINA",			GM_CMDLEVEL,	CMD_TARGET,		114,	231 },
		{"SETMOVABLE",		GM_CMDLEVEL,	CMD_TARGETX,	28,		216 },
		{"SETVISIBLE",		GM_CMDLEVEL,	CMD_TARGETX,	61,		217 },
		{"SETDIR",			GM_CMDLEVEL,	CMD_TARGETX,	88,		218 },
		{"SETSPEECH",		GM_CMDLEVEL,	CMD_TARGETX,	135,	219 },
		{"SETOWNER",		GM_CMDLEVEL,	CMD_TARGETID4,	30,		220 },
		{"SETAMOUNT",		GM_CMDLEVEL,	CMD_TARGETX,	23,		214 },
		{"SETFONT",			GM_CMDLEVEL,	CMD_TARGETID1,	19,		212 },
		{"SETMORE",			GM_CMDLEVEL,	CMD_TARGETID4,	10,		211 },
		{"SETID",			GM_CMDLEVEL,	CMD_TARGETID2,	7,		210 },
		{"SETTYPE",			GM_CMDLEVEL,	CMD_TARGETID1,	6,		206 },
		{"SETMOREXYZ",		GM_CMDLEVEL,	CMD_TARGETXYZ,	66,		202 },
		{"SETHEXMOREXYZ",	GM_CMDLEVEL,	CMD_TARGETXYZ,	66,		203 },
		{"SETNPCAI",		GM_CMDLEVEL,	CMD_TARGETX,	106,	204 },
		{"SETMOREX",		GM_CMDLEVEL,	CMD_TARGETX,	63,		199 },
		{"SETMOREY",		GM_CMDLEVEL,	CMD_TARGETX,	64,		200 },
		{"SETMOREZ",		GM_CMDLEVEL,	CMD_TARGETX,	65,		201 },
		// T
		{"TELESTUFF",		GM_CMDLEVEL,	CMD_TARGET,		222,	250 },
		{"TWEAK",			GM_CMDLEVEL,	CMD_TARGET,		62,		229 },
		{"TILEDATA",		GM_CMDLEVEL,	CMD_TARGET,		46,		223 },
		{"TTRIG",			GM_CMDLEVEL,	CMD_TARGETX,	202,	209 },
		{"TRAINER",			GM_CMDLEVEL,	CMD_TARGET,		206,	252 },
		{"TELE",			GM_CMDLEVEL,	CMD_TARGET,		2,		185 },
		// U
		{"USE",				GM_CMDLEVEL,	CMD_TARGET,		24,		179 },
		{"UNHIDE",			GM_CMDLEVEL,	CMD_TARGET,		132,	245 },
		{"UNGLOW",			GM_CMDLEVEL,	CMD_TARGET,		249,	259 },
		{"UNFREEZE",		GM_CMDLEVEL,	CMD_TARGET,		35,		222 },
		// V
		// W
		{"WSTATS",			CNS_CMDLEVEL,	CMD_TARGET,		55,		183 },
		// X
		{"XBANK",			GM_CMDLEVEL,	CMD_TARGET,		107,	186 },
		{"XSBANK",			GM_CMDLEVEL,	CMD_TARGET,		107,	187 }, 
		{"XGO",				GM_CMDLEVEL,	CMD_TARGETXYZ,	8,		198 },
		// Y
		// Z
		{ NULL,				0,				0,				0,		0 }
	};
	for( iCounter = 0; target_table[iCounter].name != NULL; iCounter++ )
	{
		targtable_mapentry newEntry( target_table[iCounter].cmdLevelReq, target_table[iCounter].cmdType, target_table[iCounter].targID, target_table[iCounter].dictEntry );
		targ_table[target_table[iCounter].name] = newEntry;
	}

	cmdtable_entry command_table[] = 
	{
		//A
		{"ADDU",				GM_CMDLEVEL,	CMD_ITEMMENU,	(CMD_DEFINE)1}, // Opens the GM add menu.
		{"AREACOMMAND",			GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_areaCommand},
		{"ADDACCOUNT",			GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_addaccount},
		{"ACTION",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_action},
		{"ADD",					GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_add},
		{"ADDX",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_addx},
		{"ADDITEM",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_additem},
		{"ALLMOVEON",			GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_allmoveon},
		{"ALLMOVEOFF",			GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_allmoveoff},
		{"ADDNPC",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_addnpc},
		{"ANNOUNCEON",			GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_announceon},
		{"ANNOUNCEOFF",			GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_announceoff},
		//B
		{"BRIGHTLIGHT",			GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_brightlight},
		//C
		{"CY",					CNS_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_cy},
		{"CLEANUP",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_cleanup},
		{"CNEXT",				CNS_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_cnext},
		{"CCLEAR",				CNS_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_cclear},
		{"CQ",					CNS_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_cq},
		{"CACHESTATS",			GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_cachestats},
		{"COMMAND",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_command},
		{"CLEAR",				CNS_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_clear},
		//D
		{"DECAY",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_decay},
		{"DARKLIGHT",			GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_darklight},
		{"DUNGEONLIGHT",		GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_dungeonlight},
		{"DUPE",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_dupe},
		{"DISCONNECT",			GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_disconnect},
		{"DYE",					GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_dye},
		{"DELID",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_delid},
		//E
		//F
		{"FORCEWHO",			GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_forcewho},
		{"FIX",					CNS_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_fix},
		//G,
		{"GETLIGHT",			GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_getlight},
		{"GY",					GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_gy},
		{"GUARDSON",			GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_guardson},
		{"GUARDSOFF",			GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_guardsoff},
		{"GUMPOPEN",			GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_gumpopen},
		{"GMS",					CNS_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_gms},
		{"GUMPMENU",			GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_gumpmenu},
		{"GMMENU",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_gmmenu},
		{"GMTRANSFER",			CNS_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_gmtransfer},
		{"GO",					GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_go},
		{"GOPLACE",				CNS_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_goplace},
		{"GOCHAR",				CNS_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_gochar},
		{"GOTOCUR",				CNS_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_gotocur},
		{"GPOST",   			CNS_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_gpost},
		{"GMOPEN",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_gmopen},
		{"GCOLLECT",			GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_gcollect},
		//H
		{"HIDEHS",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_hidehs},
		{"HOWTO",				PLAYER_CMDLEVEL,CMD_FUNC,		(CMD_DEFINE)&command_howto },
		//I
		{"INVUL",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_invul},
		{"IWIPE",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_iwipe},
		{"ITEMMENU",			GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_itemmenu},
		//J
		//K
		{"KILLALL",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_killall},
		//L
		{"LIGHT",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_light},
		{"LPOST",   			CNS_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_lpost},
		{"LOADDEFAULTS",		GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_loaddefaults},
		//M
		{"MAKE",				ADMIN_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_make },
		{"MINECHECK",			GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_minecheck},
		{"MUTE",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_squelch},
		{"MIDI",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_midi},
		//N
		{"NACCT",				ADMIN_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_nacct },
		{"NOINVUL",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_noinvul},
		{"NPCRECT",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_npcrect},
		{"NPCCIRCLE",			GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_npccircle},
		{"NPCWANDER",			GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_npcwander},
		{"NODECAY",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_nodecay},
		{"NEXT",				CNS_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_next},
		//O
		//P
		{"PDUMP",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_pdump},
		{"POLY",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_poly},
		{"POST",    			CNS_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_post},
		{"POINT",				CNS_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_teleport},
		//Q
		{"Q",					CNS_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_q},
		//R
		{"RPOST",   			CNS_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_rpost},
		{"RENAME",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_rename},
		{"RESEND",				CNS_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_resend},
		{"READINI",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_readini},
		{"RESTOCK",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_restock},
		{"RESTOCKALL",			GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_restockall},
		{"RESPAWN",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_respawn},
		{"REGSPAWNALL",			GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_regspawnall},
		{"REGSPAWNMAX",			GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_regspawnmax},
		{"REGSPAWN",			GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_regspawn},
		{"RELOADSERVER",		GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_reloadserver},
		{"RELOADACCOUNTS",		GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_reloadaccounts},
		{"RENAME2",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_rename2},
		{"READSPAWNREGIONS",	GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_readspawnregions},
		{"REPORTBUG",			PLAYER_CMDLEVEL,CMD_FUNC,		(CMD_DEFINE)&command_reportbug},
		{"RELOADDEFS",			GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_reloaddefs },
		//S
		{"SGY",					ADMIN_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_sgy},
		{"SETRACE",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_setrace},
		{"SPAWNKILL",			GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_spawnkill},
		{"SQUELCH",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_squelch},
		{"SETSHOPRESTOCKRATE",	GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_setshoprestockrate},
		{"SECONDSPERUOMINUTE",	GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_secondsperuominute},
		{"SET",					GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_set},
		{"SHOWHS",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_showhs},
		{"SFX",					GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_sfx},
		{"SHOWTIME",			CNS_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_showtime},
		{"SETTIME",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_settime},
		{"SHUTDOWN",			GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_shutdown},
		{"SETPRIV",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_setpriv},
		{"SAVE",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_save},
		{"SKIN",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_skin},
		{"STATUS",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_status},
		{"SHOWIDS",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_showids},
		//T
		{"TEMP",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_temp },
		{"TILEW",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_tilew},
		{"TIME",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_time},
		{"TELL",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_tell},
		{"TILE",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_tile},
		{"TITLE",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_title},
		//U
		//V
		{"VALIDCMD",			PLAYER_CMDLEVEL,CMD_FUNC,		(CMD_DEFINE)&command_validcmd },
		//W
		{"WIPENPCS",			GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_wipenpcs},
		{"WF",					GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_wf},
		{"WHO",					CNS_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_who},
		{"WHOLIST",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_wholist},
		{"WIPE",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_wipe},
		{"WHERE",				CNS_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_where},
		//X
		{"XTELE",				CNS_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_xtele},
		{"XGATE",				GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_xgate },
		{"XGOPLACE",			GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_xgoplace},
		//Y
		//Z
		{"ZEROKILLS",			GM_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_zerokills},
		// End of Table
		{ NULL,					0,	0,				NULL }
	};

	for( iCounter = 0; command_table[iCounter].cmd_name != NULL; iCounter++ )
	{
		cmdtable_mapentry newEntry( command_table[iCounter].cmdLevelReq, command_table[iCounter].cmdType, command_table[iCounter].cmd_extra );
		cmd_table[command_table[iCounter].cmd_name] = newEntry;
	}
}


//o--------------------------------------------------------------------------o
//|	Function/Class-	void command_addaccount( cSocket *s)
//|	Date					-	10/17/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//|	Returns				-
//o--------------------------------------------------------------------------o
//|	Notes					-	
//o--------------------------------------------------------------------------o	
void command_addaccount( cSocket *s)
{
	char szBuffer[128];
	char *szCommandLine, *szCommand, *szUsername, *szPassword, *szPrivs;
	ACCOUNTSBLOCK actbTemp;
	UI16 nFlags=0x0000;
	szPassword=szUsername=szCommand=szCommandLine=NULL;
	if( Commands->GetNumArguments() > 1 )
	{
		szCommandLine=(char *)&(s->TBuffer()[Commands->cmd_offset+0]);
		szCommand =strtok(szCommandLine," ");
		szUsername=strtok(NULL," ");
		szPassword=strtok(NULL," ");
		szPrivs=strtok(NULL,"\0");
		if(szPassword==NULL||szUsername==NULL)
			return;
		if(szPrivs!=NULL)
			nFlags=atoi(szPrivs);
		// ok we need to add the account now. We will rely in the internalaccountscreation system for this
		if(Accounts->GetAccountByName(szUsername,actbTemp))
		{
			Accounts->AddAccount(szUsername,szPassword,"NA",nFlags);
			Console << "o Account added ingame: " << szUsername << ":" << szPassword << ":" << nFlags << myendl;
			sprintf(szBuffer,"Account Added: %s:%s:%i",szUsername,szPassword,nFlags);
			sysmessage(s,szBuffer);
		}
		else
		{
			Console << "o Account was not added" << myendl;
			sysmessage(s,"Account not added");
		}
	}
}

void command_setrace( cSocket *s )
{
	if( Commands->GetNumArguments() == 2 )
	{
		RACEID race = makenumber( 1 );
		if( race <= 65535 )
			Races->gate( s->CurrcharObj(), race, 1 );
		else
			sysmessage( s, 13 );
	}
}
void command_getlight( cSocket *s )
// needs redoing to support new lighting system
{
	CChar *mChar = s->CurrcharObj();
	if( mChar == NULL )
		return;
	UI08 tRegion = mChar->GetRegion();
	UI08 wID = region[tRegion]->GetWeather();
	CWeather *sys = Weather->Weather( wID );
	if( sys != NULL )
		sysmessage( s, 1632, sys->CurrentLight() );
	else
		sysmessage( s, 1632, cwmWorldState->ServerData()->GetWorldLightCurrentLevel() );
}

void command_resend( cSocket *s )
// Resends server data to client.
{
	CChar *mChar = s->CurrcharObj();
	if( mChar == NULL )
		return;
	mChar->Teleport();
}
// Returns the current bulletin board posting mode for the player
void command_post( cSocket *s )
{
	CChar *mChar = s->CurrcharObj();
	if( mChar == NULL )
		return;

	UnicodeTypes sLang = s->Language();
	char temp[1024];
	sprintf( temp, Dictionary->GetEntry( 269, sLang ) );

	switch( mChar->GetPostType() )
	{
	case LOCALPOST:		strcat( temp, Dictionary->GetEntry( 270, sLang ) );		break;
	case REGIONALPOST:	strcat( temp, Dictionary->GetEntry( 271, sLang ) );		break;
	case GLOBALPOST:	strcat( temp, Dictionary->GetEntry( 272, sLang ) );		break;
	default:			strcat( temp, Dictionary->GetEntry( 273, sLang ) );
						mChar->SetPostType( LOCALPOST );
						break;
	}
	sysmessage( s, temp );
}

// Sets the current bulletin board posting mode for the player to GLOBAL
// ALL bulletin boards will see the next message posted to any bulletin board
void command_gpost( cSocket *s )
{
	CChar *mChar = s->CurrcharObj();
	if( mChar == NULL )
		return;
	mChar->SetPostType( GLOBALPOST );
	sysmessage( s, 16 );
}

// Sets the current bulletin board posting mode for the player to REGIONAL
// All bulletin boards in the same region as the board posted to will show the
// next message posted
void command_rpost( cSocket *s )
{
	CChar *mChar = s->CurrcharObj();
	if( mChar == NULL )
		return;
	mChar->SetPostType( REGIONALPOST );
	sysmessage( s, 17 );
}

// Sets the current bulletin board posting mode for the player to LOCAL
// Only this bulletin board will have this post
void command_lpost( cSocket *s )
{
	CChar *mChar = s->CurrcharObj();
	if( mChar == NULL )
		return;
	mChar->SetPostType( LOCALPOST );
	sysmessage( s, 18 );
}

void command_teleport( cSocket *s )
{
	CChar *mChar = s->CurrcharObj();
	if( mChar == NULL )
		return;
	mChar->Teleport();
}

void command_where( cSocket *s )
// Prints your current coordinates + region.
{
	CChar *mChar = s->CurrcharObj();
	UI08 plRegion = mChar->GetRegion();
	if( strlen( region[plRegion]->GetName() ) > 0 )
		sysmessage( s, 274, region[plRegion]->GetName() );
	else 
		sysmessage( s, 275 );
	
	sysmessage( s, "%i %i (%i/%i) %i", mChar->GetX(), mChar->GetY(), mChar->GetZ(), mChar->GetDispZ(), mChar->WorldNumber() ); 
}

void command_q( cSocket *s )
// Shows the GM or Counsellor queue.
{
	CChar *mChar = s->CurrcharObj();
	if( mChar == NULL )
		return;
	Commands->showQue( s, mChar->IsGM() );
}


void command_next( cSocket *s )
// For Counselors or GM's, goes to next call in queue.
{
	CChar *mChar = s->CurrcharObj();
	if( mChar == NULL )
		return;
	Commands->nextCall( s, mChar->IsGM() );
}

void command_clear( cSocket *s )
// For Counselor's and GM's, removes current call from queue.
{
	CChar *mChar = s->CurrcharObj();
	if( mChar == NULL )
		return;
	Commands->closeCall( s, mChar->IsGM() );
}

void command_goplace( cSocket *s )
// (d) Teleports you to a location from the LOCATIONS.SCP file.
{
	CChar *mChar = s->CurrcharObj();
	if( Commands->GetNumArguments() == 2 )
	{
		Commands->MakePlace( s, makenumber( 1 ) );
		if( s->AddX() != 0 )
		{
			mChar->SetLocation( static_cast<SI16>(s->AddX()), static_cast<SI16>(s->AddY()), s->AddZ() );
			mChar->Teleport();
		}
	}
}

void command_gochar( cSocket *s )
// (h h h h) Teleports you to another character.
{
	CChar *mChar = s->CurrcharObj();
	if( Commands->GetNumArguments() == 5 )
	{
		SERIAL serial = calcserial( (UI08)makenumber( 1 ), (UI08)makenumber( 2 ), (UI08)makenumber( 3 ), (UI08)makenumber( 4 ) );
		CChar *iTo = calcCharObjFromSer( serial );
		if( iTo != NULL )
		{
			if( iTo->GetCommandLevel() > mChar->GetCommandLevel() )
			{
				sysmessage( s, 19 );
				return;
			}
			if( iTo->WorldNumber() != mChar->WorldNumber() )
			{
				mChar->SetLocation( iTo );
				SendMapChange( iTo->WorldNumber(), s );
			}
			else
				mChar->SetLocation( iTo );
			mChar->Teleport();
		}
		else
			sysmessage( s, "No such character exists" );
	} 
	else if( Commands->GetNumArguments() == 2 )
	{
		cSocket *tSock = calcSocketObjFromSock( makenumber( 1 ) );
		if( tSock != NULL )
		{
			CChar *tChar = tSock->CurrcharObj();
			if( tChar != NULL )
			{
				if( tChar->GetCommandLevel() > mChar->GetCommandLevel() )
				{
					sysmessage( s, 19 );
					return;
				}
				if( tChar->WorldNumber() != mChar->WorldNumber() )
				{
					mChar->SetLocation( tChar );
					SendMapChange( tChar->WorldNumber(), s );
				}
				else
					mChar->SetLocation( tChar );
				mChar->Teleport();
			}
			else
				sysmessage( s, "No such character exists" );
		}
		else
			sysmessage( s, "No such socket exists" );
	}
}

void command_fix( cSocket *s )
// Try to compensate for messed up Z coordinates. Use this if you find yourself half-embedded in the ground.
{ // Rewrite to do it properly
	CChar *mChar = s->CurrcharObj();
	if( Commands->GetNumArguments() == 2 )
	{
		if( validTelePos( s ) == -1 )
		{
			mChar->SetDispZ( (SI08)makenumber( 1 ) );
			mChar->SetZ( (SI08)makenumber( 1 ) );
		}
		else
		{
			mChar->SetDispZ( validTelePos( s ) );
			mChar->SetZ( validTelePos( s ) );
		}
		mChar->Teleport();
	}
}

void command_xgoplace( cSocket *s )
// (d) Send another character to a location in your LOCATIONS.SCP file.
{
	if( Commands->GetNumArguments() == 2 )
	{
		Commands->MakePlace( s, makenumber( 1 ) );
		if( s->AddX() != 0 )
			target( s, 0, 8, 20 );
	}
}

void command_showids( cSocket *s )
// Display the serial number of every item on your screen.
{
	CChar *mChar = s->CurrcharObj();
	SI16 x = mChar->GetX(), y = mChar->GetY();
	
	UI08 worldNumber = mChar->WorldNumber();
	SubRegion *Cell = MapRegion->GetCell( x, y, worldNumber );
	if( Cell == NULL )	// nothing to show
		return;
	Cell->PushChar();
	for( CChar *toShow = Cell->FirstChar(); !Cell->FinishedChars(); toShow = Cell->GetNextChar() )
	{
		if( toShow == NULL ) 
			continue;
		if( charInRange( mChar, toShow ) )
			showcname( s, toShow, 1 );
	}
	Cell->PopChar();
}

void command_poly( cSocket *s )
// (h h) Polymorph yourself into any other creature.
{
	CChar *mChar = s->CurrcharObj();
	if( Commands->GetNumArguments() < 2 || Commands->GetNumArguments() > 3 || !mChar )
		return;
	UI16 targBody = INVALIDID;
	if( Commands->GetNumArguments() == 2 )
		targBody = (UI16)makenumber( 1 );
	else if( Commands->GetNumArguments() == 3 )
		targBody = (UI16)((makenumber( 1 ) << 8 ) + makenumber( 2 ));

	if( targBody <= 0x2FF ) // body-values > 0x3E crash the client
	{ 
		mChar->SetID( targBody );
		mChar->SetxID( targBody );
		mChar->SetOrgID( targBody );

		// if polymorphing from monster to player we have to switch from transparent to semi-transparent
		// or we have that sit-down-client crash
		UI16 c1 = mChar->GetSkin();
		UI16 b = c1&0x4000; 
		if( b == 16384 && targBody >= 0x0190 && targBody <= 0x03E1 )
		{
			if( c1 != 0x8000 )
			{
				mChar->SetSkin( 0xF000 );
				mChar->SetxSkin( 0xF000 );
			}
		}
	}
	
	mChar->Teleport();
}

void command_skin( cSocket *s )
// (h h) Change the hue of your skin.
{
	CChar *mChar = s->CurrcharObj();
	if( Commands->GetNumArguments() == 2 && mChar != NULL )
	{
		int body = mChar->GetID();
		COLOUR k = (COLOUR)makenumber( 1 );
		int b = k&0x4000; 
		if( b == 16384 && ( body >= 0x0190 && body <= 0x03E1 ) )
			k = 0xF000; 
		
		if( k != 0x8000 )
		{	
			mChar->SetSkin( k );
			mChar->SetxSkin( k );
			mChar->Teleport();
		}
	}
	return;
	
}

void command_action( cSocket *s )
// (h) Preform an animated action sequence.
{
	if( Commands->GetNumArguments() == 2 ) 
		Effects->action( s, makenumber( 1 ) );
}

void command_xtele( cSocket *s )
// (d / h h h h / nothing) Teleport a player to your position.
// <UL><LI>If you specify nothing (/XTELE), you click on the player to teleport in.</LI>
// <LI>If you specify a serial number (/XTELE .. .. .. ..), you teleport that player to you.</LI>
// <LI>If you specify a single number (/XTELE ..), you teleport the player logged in
// under that slot to you.</LI>
// </UL>
{
	if( Commands->GetNumArguments() == 5 || Commands->GetNumArguments() == 2 ) 
		Targ->XTeleport( s, static_cast<UI08>(Commands->GetNumArguments() ));
	else
		target( s, 0, 136, 21 );
}

void command_go( cSocket *s )
// (d d d) Go to the specified X/Y/Z coordinates
{
	CChar *mChar = s->CurrcharObj();
	if( Commands->GetNumArguments() == 4 )
	{
		SI16 x = (SI16)makenumber( 1 ); 
		SI16 y = (SI16)makenumber( 2 ); 
		SI08 z = (SI08)(makenumber( 3 ) );
		if( x < 6144 && y < 4096 )
		{
			mChar->SetLocation( x, y, z );
			mChar->Teleport();
		}
	}
}

void command_zerokills( cSocket *s )
// Sets all PK counters to 0.
{
	sysmessage( s, 22 );
	for( UI32 a=0; a < cwmWorldState->GetCharCount(); a++ )
	{
		chars[a].SetKills( 0 );
		setcharflag( &chars[a] );
	}
	sysmessage( s, 23 );
}

void command_tile( cSocket *s )
// (h h) Tiles the item specified over a square area.
// To find the hexidecimal ID code for an item to tile,
// either create the item with /add or find it in the
// world, and get /ISTATS on the object to get it's ID
// code.
{
	s->ClickX( -1 );
	s->ClickY( -1 );
	if( Commands->GetNumArguments() != 2 && Commands->GetNumArguments() != 3 )
		return;
	if( Commands->GetNumArguments() == 2 )
	{
		UI16 targID = (UI16)makenumber( 1 );
		s->AddID1( (UI08)(targID>>8) );
		s->AddID2( (UI08)(targID%256) );
	}
	else
	{
		s->AddID1( (UI08)(makenumber( 1 )) );
		s->AddID2( (UI08)(makenumber( 2 )) );
	}
	target( s, 0, 198, 24 );  // 198 didn't seem taken...
}

void command_wipe( cSocket *s )
// (d d d d / nothing) Deletes ALL NPC's and items inside a specified square.
// <UL><LI>With no arguments, /WIPE will ask you to click in opposing corners of
// the square.</LI>
// <LI>You may also specify coordinates - X1, Y1, X2, Y2.</LI>
// </UL>
{
	s->AddID1( 0 );
	if( Commands->GetNumArguments() == 1 ) 
	{
		s->ClickX( -1 );
		s->ClickY( -1 );
		target( s, 0, 199, 25 );  // 199 didn't seem taken...
	} 
	else if( Commands->GetNumArguments() == 2 ) 
	{
		if( !strcmp( "ALL", (char*)&(s->TBuffer()[Commands->cmd_offset+5]) ) )  // Really should warn that this will wipe ALL objects...
			Commands->Wipe( s );
	}
	else if( Commands->GetNumArguments() == 5 ) 
	{ // Wipe according to world coordinates
		s->ClickX( makenumber( 1 ) );
		s->ClickY( makenumber( 2 ) );
		s->SetWord( 11, (UI16)makenumber( 3 ) );
		s->SetWord( 13, (UI16)makenumber( 4 ) );
		Targ->Wiping( s );
	}
}

void command_areaCommand( cSocket *s )
{
	if( strlen( (char *)&(s->TBuffer()[Commands->cmd_offset+12]) ) > 0 )
	{
		s->XText( (char *)&(s->TBuffer()[Commands->cmd_offset + 12]) );
		s->ClickX( -1 );
		s->ClickY( -1 );
		target( s, 0, 90, 25 );
	}
	else
	{
		sysmessage( s, "Area command requires a subcommand!" );
	}
}


void command_iwipe( cSocket *s )
// (d d d d / nothing) Deletes ALL NPC's and items NOT inside a specified square.
// <UL><LI>With no arguments, /IWIPE will ask you to click in opposing corners of
// the square.</LI>
// <LI>You may also specify coordinates - X1, Y1, X2, Y2.</LI>
// </UL>
{
	s->AddID1( 1 );
	if( Commands->GetNumArguments() == 1 ) 
	{
		s->ClickX( -1 );
		s->ClickY( -1 );
		target( s, 0, 199, 26 );  // 199 didn't seem taken...
	} 
	else if( Commands->GetNumArguments() == 2 ) 
	{
		if( !strcmp( "ALL", (char *)&(s->TBuffer()[Commands->cmd_offset+6]) ) ) 
		{
			sysmessage( s, 90 );
		}
	}
	else if( Commands->GetNumArguments() == 5 ) 
	{ // Wipe according to world coordinates
		s->ClickX( makenumber( 1 ) );
		s->ClickY( makenumber( 2 ) );
		s->SetWord( 11, (UI16)makenumber( 3 ) );
		s->SetWord( 13, (UI16)makenumber( 4 ) );
		Targ->Wiping( s );
	}
}

void command_add( cSocket *s )
// (h h) Adds a new item, or opens the GM menu if no hex codes are specified.
{
	if( Commands->GetNumArguments() > 3 )
	{
		sysmessage( s, 27 );
		return;
	} 
	else if( Commands->GetNumArguments() == 3 || Commands->GetNumArguments() == 2 )
	{
		UI16 targID = INVALIDID;
		if( Commands->GetNumArguments() == 3 )
			targID = (UI16)( ( makenumber( 1 ) << 8 ) + makenumber( 2 ) );
		else
			targID = (UI16)makenumber( 1 );
		s->AddID1( (UI08)(targID>>8) );
		s->AddID2( (UI08)(targID%256) );
		if( s->AddID1() < 0x40 )
			target( s, 0, 0, 28 );
	} 
	else if( Commands->GetNumArguments() == 1 )
	{
		NewAddMenu( s, 1 );
	}
}

void command_addx( cSocket *s )
// (h h) Adds a new item to your current location.
{
	CChar *mChar = s->CurrcharObj();
	if( mChar == NULL )
		return;

	UI16 addID = INVALIDID;
	SI08 targZ = mChar->GetZ();
	switch( Commands->GetNumArguments() )
	{
	case 2:	addID = (UI16)makenumber( 1 );	break;
	case 4:	targZ = (SI08)makenumber( 3 );
	case 3: addID = (UI16)( ( makenumber( 1 ) << 8 ) + makenumber( 2 ) );
			break;
	}
	s->AddID1( (UI08)(addID>>8) );
	s->AddID2( (UI08)(addID%256) );
	Commands->AddHere( s, targZ );
}

void command_rename( cSocket *s )
// (text) Renames any dynamic item in the game.
{
	if( Commands->GetNumArguments() > 1 )
	{
		s->XText( (char *)&(s->TBuffer()[Commands->cmd_offset+7]) );
		target( s, 0, 1, 29 );
	}
}

void command_title( cSocket *s )
// (text) Changes the title of any player or NPC.
{
	if( Commands->GetNumArguments() > 1 )
	{
		s->XText( (char *)&(s->TBuffer()[Commands->cmd_offset+6]) );
		target( s, 0, 47, 30 );
	}
}

#pragma note( "Param Warning: in command_save, s is unrefrenced" )
void command_save( cSocket *s )
// Saves the current world data into ITEMS.WSC and CHARS.WSC.
{
	if( !cwmWorldState->Saving() )
	{
		cwmWorldState->savenewworld( true );
	}
}

void command_dye( cSocket *s )
// (h h/nothing) Dyes an item a specific color, or brings up a dyeing menu if no color is specified.
{
	s->DyeAll( 1 );
	if( Commands->GetNumArguments() == 2 )
	{
		UI16 tNum = makenumber( 1 );
		s->AddID1( (UI08)(tNum>>8) );
		s->AddID2( (UI08)(tNum%256) );
	} 
	else if( Commands->GetNumArguments() == 3 )
	{
		s->AddID1( (UI08)makenumber( 1 ) );
		s->AddID2( (UI08)makenumber( 2 ) );
	} 
	else
	{
		s->AddID1( 0xFF );
		s->AddID2( 0xFF );
	}
	target( s, 0, 4, 31 );
}

void command_setpriv( cSocket *s )
// (Chars/NPCs: h h, Objects: h) Sets priviliges on a Character or object.
{
	if( Commands->GetNumArguments() == 3 )
	{
		s->AddID1( (UI08)makenumber( 1 ) );
		s->AddID2( (UI08)makenumber( 2 ) );
		target( s, 0, 9, 33 );
	}
	if( Commands->GetNumArguments() == 2 )
	{
		s->AddID1( 3 );
		s->AddID2( (UI08)makenumber( 1 ) );
		target( s, 0, 89, 34 );
	}
}

void command_nodecay( cSocket *s )
// Prevents an object from ever decaying.
{
	s->AddID1( 0 );
	target( s, 0, 89, 35 );
}


void command_showtime( cSocket *s )
// Displays the current UO time.
{
	UI32 ampm = cwmWorldState->ServerData()->GetServerTimeAMPM();
	SI16 hour = cwmWorldState->ServerData()->GetServerTimeHours();
	SI16 minute = cwmWorldState->ServerData()->GetServerTimeMinutes();
	if( ampm || (!ampm && hour == 12 ) )
		sysmessage( s, "%s %2.2d %s %2.2d %s", "Time: ", hour, ":", minute, "PM" );
	else
		sysmessage( s, "%s %2.2d %s %2.2d %s", "Time: ", hour, ":",minute, "AM" );
}

#pragma note( "Param Warning: in command_settime, s is unrefrenced" )
void command_settime( cSocket *s )
// PARAM WARNING: s is unreferenced
// (d d) Sets the current UO time in hours and minutes.
{
	if( Commands->GetNumArguments() == 3 )
	{
		int newhours = makenumber( 1 );
		int newminutes = makenumber( 2 );
		if( ( newhours < 25 ) && ( newhours > 0 ) && ( newminutes > -1 ) && ( newminutes < 60 ) )
		{
			cwmWorldState->ServerData()->SetServerTimeAMPM( (newhours > 12) );
			if( newhours > 12 )
				cwmWorldState->ServerData()->SetServerTimeHours( newhours - 12 );
			else
				cwmWorldState->ServerData()->SetServerTimeHours( newhours );
			cwmWorldState->ServerData()->SetServerTimeMinutes( newminutes );
		}
	}
}

#pragma note( "Param Warning: in command_shutdown, s is unrefrenced" )
void command_shutdown( cSocket *s )
// PARAM WARNING: s is unreferenced
// (d) Shuts down the server. Argument is how many minutes until shutdown.
{
	if( Commands->GetNumArguments() == 2 )
	{
		cwmWorldState->SetEndTime( BuildTimeValue( static_cast<R32>(makenumber( 1 )) ) );
		if( makenumber( 1 ) == 0 )
		{
			cwmWorldState->SetEndTime( 0 );
			sysbroadcast( Dictionary->GetEntry( 36 ) );
		}
		else 
			endmessage( 0 );
	}
}

void command_wholist( cSocket *s )
// Brings up an interactive listing of online users.
{
	if( strcmp( (char *)&(s->TBuffer()[Commands->cmd_offset+8]), "off" ) )
		WhoList->SendSocket( s );
	else
		OffList->SendSocket( s );
}

void command_sfx( cSocket *s )
// (h h) Plays the specified sound effect.
{
	switch( Commands->GetNumArguments() )
	{
	case 2:		Effects->PlaySound( s, makenumber( 1 ), true );							break;
	case 3:		Effects->PlaySound( s, (makenumber( 1 ) << 8 ) + makenumber( 2 ), true );	break;
	}
}

#pragma note( "Param Warning: in command_light, s is unrefrenced" )
void command_light( cSocket *s)
// PARAM WARNING: s is unreferenced
// (h) Sets the light level. 0=brightest, 15=darkest, -1=enable day/night cycles.
{
	if( Commands->GetNumArguments() == 2 )
	{
		SI16 tLevel = (SI16)makenumber( 1 );
		cwmWorldState->ServerData()->SetWorldFixedLightLevel( tLevel );
	}
}

#pragma note( "Param Warning: in command_disconnect, s is unrefrenced" )
void command_disconnect( cSocket *s )
// PARAM WARNING: s is unreferenced
// (d) Disconnects the user logged in under the specified slot.
{
	if( Commands->GetNumArguments() == 2 ) 
		Network->Disconnect( makenumber( 1 ) );
}

void command_tell( cSocket *s )
// (d text) Sends an anonymous message to the user logged in under the specified slot.
{
	if( Commands->GetNumArguments() > 2 ) 
	{
		int sockNum = makenumber( 1 );
		if ( sockNum < 10 )
			tellmessage( s, calcSocketObjFromSock( sockNum ), (char *)&(s->TBuffer()[Commands->cmd_offset+6]) );
		else if ( sockNum < 100 )
			tellmessage( s, calcSocketObjFromSock( sockNum ), (char *)&(s->TBuffer()[Commands->cmd_offset+7]) );
		else
			tellmessage( s, calcSocketObjFromSock( sockNum ), (char *)&(s->TBuffer()[Commands->cmd_offset+8]) );
	}
}

void command_gmmenu( cSocket *s )
// (d) Opens the specified GM Menu.
{
	if( Commands->GetNumArguments() == 2 ) 
		gmmenu( s, makenumber( 1 ) );
}

void command_itemmenu( cSocket *s )
// (d) Opens the specified Item Menu from ITEMS.SCP.
{
	int i;
	if( Commands->GetNumArguments() == 2 )
	{
		i = makenumber( 1 );
		NewAddMenu( s, i );
	}
}

void command_additem( cSocket *s )
// (d) Adds the specified item from ITEMS.SCP.
{
	char temp[1024];
	if( Commands->GetNumArguments() >= 2 )	// might be quite long
	{
		s->XText( (char *)&(s->TBuffer()[Commands->cmd_offset+8]) );	// let's store what we really want to add
		sprintf( temp, Dictionary->GetEntry( 37, s->Language() ), s->XText() );
		target( s, 0, 26, temp );
	}
}

void command_dupe( cSocket *s )
// (d / nothing) Duplicates an item. If a parameter is specified, it's how many copies to make.
{
	if( Commands->GetNumArguments() == 2 )
	{
		s->AddID1( makenumber( 1 ) );
		target( s, 0, 110, 38 );
	}
	else
	{
		s->AddID1( 1 );
		target( s, 0, 110, 38 );
	}
}


void command_command( cSocket *s )
// Executes a trigger scripting command.
{
	if( Commands->GetNumArguments() > 1 )
	{
		char scp1[1024], scp2[1024];
		char c = 0;
		int i = 0;
		scp1[0] = 0;
		scp2[0] = 0;
		while( s->TBuffer()[Commands->cmd_offset+8+i] != ' ' && s->TBuffer()[Commands->cmd_offset+8+i] != 0 ) 
			i++;
		strncpy( scp1, (char*)&(s->TBuffer()[Commands->cmd_offset+8]), i );
		scp1[i] = 0;
		if( scp1[0] != '}' && c != 0 ) 
			strcpy( scp2, (char *)&(s->TBuffer()[Commands->cmd_offset+8+i+1] ) );
		scriptcommand( s, scp1, scp2 );
	}
}

#pragma note( "Param Warning: in command_gcollect, s is unrefrenced" )
void command_gcollect( cSocket *s )
// PARAM WARNING: s is unreferenced
// Runs garbage collection routines.
{
	doGCollect();
}

void command_allmoveon( cSocket *s )
// Enables GM ability to pick up all objects.
{
	CChar *mChar = s->CurrcharObj();
	mChar->SetAllMove( true );
	mChar->Teleport();
	sysmessage( s, 39 );
}

void command_allmoveoff( cSocket *s )
// Disables GM ability to pick up all objects.
{
	CChar *mChar = s->CurrcharObj();
	mChar->SetAllMove( false );
	mChar->Teleport();
	sysmessage( s, 40 );
}

void command_showhs( cSocket *s )
// Makes houses appear as deeds. (The walls disappear and there's a deed on the ground in their place.)
{
	CChar *mChar = s->CurrcharObj();
	mChar->SetViewHouseAsIcon( true );
	mChar->Teleport();
	sysmessage( s, 41 );
}

void command_hidehs( cSocket *s )
// Makes houses appear as houses (opposite of /SHOWHS).
{
	CChar *mChar = s->CurrcharObj();
	mChar->SetViewHouseAsIcon( false );
	mChar->Teleport();
	sysmessage( s, 42 );
}

//o--------------------------------------------------------------------------
//|	Function		-	void command_set( cSocket *s )
//|	Date			-	February 22, 2002
//|	Programmer		-	DarkStorm
//|	Modified		-
//|
//|	Modification	-	02/22/2002 - Rewrite
//o--------------------------------------------------------------------------
//|	Purpose			-	This Command is used for ingame manipulation of 
//|						items and characters, it'll set different kinds
//|						of properties for both chars and items. 
//o--------------------------------------------------------------------------
void command_set( cSocket *s )
{
	// Log regardless of Success or Failure
	Commands->Log( "/set", s->CurrcharObj(), NULL, (char *)&(s->TBuffer()[Commands->cmd_offset+4]) );

	mstring CommandString = (char*)&(s->TBuffer()[Commands->cmd_offset+4]);
	CommandString.trim();
	
	// Just one paramter
	if( CommandString.find( " " ) == -1 )
	{
		sysmessage( s, "Usage: set <field> <value>" );
		return;
	}

	// We want to display back what the user is going to set the property to
	string Message = "Choose target to set '";
	Message += CommandString;
	Message += "'";
	
	sysmessage( s, Message.c_str() );

	s->XText( CommandString.c_str() );

	// Dont remove the char* or else it'll complain about 
	// the overloaded function
	target( s, 0, 36 , (char *)NULL );

	/*if( Commands->GetNumArguments() == 3 )
	{
		char toSet[1024];
		int i = 0;
		toSet[0] = 0;
		while( s->TBuffer()[Commands->cmd_offset+4+i] != ' ' && s->TBuffer()[Commands->cmd_offset+4+i] != 0 )
			i++;
		strncpy( toSet, (char *)&(s->TBuffer()[Commands->cmd_offset+4]), i );
		toSet[i] = 0;

		// Set Command to uppercase
		strupr( toSet );

		s->AddX( -1 );
		
		char temp[512];
		sprintf( temp, "PreTarget: %s", &(s->TBuffer()[Commands->cmd_offset+4]) );
		Commands->Log( "/set", s->CurrcharObj(), NULL, temp );
		
		for( i = 0; i < SKILLS; i++ )
		{
			if( !strcmp( skillname[i], toSet ) )
			{ 
				s->AddX( i );
				break;
			}
		}

		if( !strcmp( toSet, "KILLS" ) )
			s->AddX( SKILLS + 1 );

		if( s->AddX() != -1 )
		{
			s->AddY( makenumber( 2 ) );
			
		}
	}*/
}

void command_addnpc( cSocket *s )
// (d or h h) Add the specified NPC from NPC.SCP or by hex ID code.
{
	if( Commands->GetNumArguments() == 3 )
	{
		s->AddID1( makenumber( 1 ) );
		s->AddID2( makenumber( 2 ) );
		target( s, 0, 33, 44 );
	}
	else if( Commands->GetNumArguments() == 2 )
	{
		s->XText( (char *)&(s->TBuffer()[Commands->cmd_offset+7]) );
		target( s, 0, 27, 44 );
	}
}

void command_readini( cSocket *s )
// Re-loads the UOX3.INI file.
{
	cwmWorldState->ServerData()->load();
	sysmessage( s, 45 );
}

void command_gumpmenu( cSocket *s )
// (d) Opens the specified GUMP menu.
{
	if( Commands->GetNumArguments() == 2 )
		Gumps->Menu( s, makenumber( 1 ) );
}

#pragma note( "Function Warning: command_cachestats currently does nothing" )
void command_cachestats( cSocket *s )
// Display some information about the cache.
{
}

void command_npcrect( cSocket *s )
// (d d d d) Set bounding box for a NPC with a NPCWANDER of 3.
{
	if( Commands->GetNumArguments() == 5 )
	{
		s->AddX( makenumber( 1 ) );
		s->AddY( makenumber( 2 ) );
		s->AddX2( makenumber( 3 ) );
		s->AddY2( makenumber( 4 ) );
		target( s, 0, 67, 46 );
	}
}

void command_npccircle( cSocket *s )
// (d d d) Set bounding circle for a NPC with a NPCWANDER of 2.
{
	if( Commands->GetNumArguments() == 4 )
	{
		s->AddX( makenumber( 1 ) );
		s->AddY( makenumber( 2 ) );
		s->AddX2( makenumber( 3 ) );
		target( s, 0, 59, 47 );
	}
}

void command_npcwander( cSocket *s )
// (d) Sets the type of wandering a NPC does.
// <UL><LI>0 = NPC Does not move.</LI>
// <LI>1 = NPC Follows specified target. (See
// <A HREF="npctarget.html">/NPCTARGET</A>)</LI>
// <LI>2 = NPC Wanders Freely.</LI>
// <LI>3 = NPC stays in box specified by <A HREF="npcrect.html">NPCRECT</A>.</LI>
// <LI>4 = NPC stays in circle specified by <A HREF="npccircle.html">NPCCIRCLE</A>.</LI></UL>
{
	if( Commands->GetNumArguments() == 2 )
	{
		s->TempInt( makenumber( 1 ) );
		target( s, 0, 60, 48 );
	}
}

void command_secondsperuominute( cSocket *s )
// (d) Sets the number of real-world seconds that pass for each UO minute.
{
	if( Commands->GetNumArguments() == 2 )
	{
		cwmWorldState->SetSecondsPerUOMinute( (UI16)makenumber( 1 ) );
		sysmessage( s, 49 );
	}
}

void command_brightlight( cSocket *s )
// (h) Sets default daylight level.
{
	if( Commands->GetNumArguments() == 2 )
	{
		cwmWorldState->ServerData()->SetWorldLightBrightLevel( (SI16)makenumber( 1 ) );
		sysmessage( s, 50 );
	}
}

void command_darklight( cSocket *s )
// (h) Sets default nighttime light level.
{
	if( Commands->GetNumArguments() == 2 )
	{
		cwmWorldState->ServerData()->SetWorldLightDarkLevel( (SI16)makenumber( 1 ) );
		sysmessage( s, 50 );
	}
}

void command_dungeonlight( cSocket *s )
// (h) Sets default dungeon light level.
{
	if( Commands->GetNumArguments() == 2 )
	{
		cwmWorldState->ServerData()->SetDungeonLightLevel( (SI16)makenumber( 1 ) );
		sysmessage( s, 52 );
	}
}

void command_gmopen( cSocket *s )
// (h / nothing) Opens specified layer on player/NPC, or player's pack if no layer specified.
// <P>Useful hex codes for this command are:</P>
// <TABLE BORDER=1>
// <TR><TD><B>15</B></TD><TD>Backpack</TD></TR>
// <TR><TD><B>1A</B></TD><TD>NPC Buy Restock container</TD></TR>
// <TR><TD><B>1B</B></TD><TD>NPC Buy no restock container</TD></TR>
// <TR><TD><B>1C</B></TD><TD>NPC Sell container</TD></TR>
// <TR><TD><B>1D</B></TD><TD>Bank Box</TD></TR>
// </TABLE>
{
	if( Commands->GetNumArguments() == 2 ) 
		s->TempInt( makenumber( 1 ) );
	else 
		s->TempInt( 0x15 );
	target( s, 0, 115, 53 );
}

void command_restock( cSocket *s )
// Forces a manual vendor restock.
{
	restock( false );
	sysmessage( s, 54 );
}

void command_restockall( cSocket *s )
// Forces a manual vendor restock to maximum values.
{
	restock( true );
	sysmessage( s, 55 );
}

void command_setshoprestockrate( cSocket *s )
// (d) Sets the universe's shop restock rate.
{
	if( Commands->GetNumArguments() == 2 )
	{
		cwmWorldState->ServerData()->SetSystemTimer( SHOP_SPAWN, makenumber( 1 ) );
		sysmessage( s, 56 );
	}
	else 
		sysmessage( s, 57 );
}

void command_midi( cSocket *s )
// (d d) Plays the specified MIDI file.
{
	if( Commands->GetNumArguments() == 2 )
		Effects->playMidi( s, (UI16)makenumber( 1 ) );
	else if( Commands->GetNumArguments() == 3 ) 
		Effects->playMidi( s, (UI16)( ( makenumber( 1 ) << 8 ) + makenumber( 2 ) ) );
}

void command_gumpopen( cSocket *s )
// (h h) Opens the specified GUMP menu.
{
	switch( Commands->GetNumArguments() )
	{
	case 2:		Gumps->Open( s, s->CurrcharObj(), (UI16)( makenumber( 1 ) ) );							break;
	case 3:		Gumps->Open( s, s->CurrcharObj(), (UI16)( (makenumber( 1 )<<8) + makenumber( 2 ) ) );	break;
	}
}

#pragma note( "Param Warning: in command_respawn, s is unrefrenced" )
void command_respawn( cSocket *s )
// PARAM WARNING: s is unreferenced
// Forces a respawn.
{
	for( UI16 region = 1; region < cwmWorldState->GetTotalSpawnRegions(); region++ )
	{
		if( spawnregion[region] != NULL )
			spawnregion[region]->doRegionSpawn();
	}

	bool k = false;
	UI32 ci;
	ITEM j;
	for( ITEM i = 0; i < cwmWorldState->GetItemCount(); i++ )  // Item Spawner
	{
		if( items[i].GetType() == 61 )
		{
			k = false;
			HashBucketMulti< ITEM > *hashBucket = nspawnsp.GetBucket( (items[i].GetSerial())%HASHMAX );
			for( ci = 0; ci < hashBucket->NumEntries(); ci++ )
			{
				j = hashBucket->GetEntry( ci );
				if( j != INVALIDSERIAL )
				{
					if( i != j && items[j].GetX() == items[i].GetX() && items[j].GetY() == items[i].GetY() && items[j].GetZ() == items[i].GetZ() )
					{
						if( items[i].GetSerial() == items[j].GetSpawn() )
						{
							k = true;
							break;
						}
					}
				}
			}
			if( !k )
			{
				const char *nDesc = items[i].GetDesc();
				if( nDesc[0] != 0 )	// not NULL terminated, so we can do something!
					Items->AddRespawnItem( &items[i], nDesc, true, items[i].isSpawnerList() );
				else if( items[i].GetMoreX() != 0 )
					Items->AddRespawnItem( &items[i], items[i].GetMoreX(), false );
				items[i].SetGateTime( 0 );
			}
		}
		
		if( items[i].GetType() == 62 || items[i].GetType() == 69 || items[i].GetType() == 125 )  // NPC Spawner
		{
			int amt = 0;
			HashBucketMulti< CHARACTER > *hashBucket = ncspawnsp.GetBucket( (items[i].GetSerial())%HASHMAX );
			for( ci = 0; ci < hashBucket->NumEntries(); ci++ )
			{
				j = hashBucket->GetEntry( ci );
				if( j != INVALIDSERIAL )
				{
					if( items[i].GetSerial() == items[j].GetSpawn() )
						amt++;
				}
			} 
			
			if( amt < items[i].GetAmount() )
			{
				const char *mDesc = items[i].GetDesc();
				if( mDesc[0] != 0 )	// not NULL terminated, so we can do something!
				{
					Npcs->SpawnNPC( &items[i], mDesc, items[i].WorldNumber(), items[i].isSpawnerList() );
				}
				else if( items[i].GetMoreX() != 0 )
				{
					char temp[512];
					sprintf( temp, "%i", items[i].GetMoreX() );
					Npcs->SpawnNPC( &items[i], temp, items[i].WorldNumber(), false );
				}
				items[i].SetGateTime( 0 );
			}
		}
	}
}

void command_regspawnmax( cSocket *s )
// (d) Spawns in all regions up to the specified maximum number of NPCs/Items.
{
	if( Commands->GetNumArguments() == 2 )
	{
		Console << "Regspawnmax called" << myendl;
		Commands->RegSpawnMax( s, spawnregion[makenumber( 1 )] );
	}
}

void command_regspawn( cSocket *s )
// (d d) Preforms a region spawn. First argument is region, second argument is max # of items/NPCs to spawn in that region.
{
	if( Commands->GetNumArguments() == 3 )
	{
		Console << "Regspawn called" << myendl;
		Commands->RegSpawnNum( s, spawnregion[makenumber( 1 )], makenumber( 2 ) );
	}
}

void command_reloadserver( cSocket *s )
// Reloads the SERVER.SCP file.
{
	cwmWorldState->ServerData()->load();
	sysmessage( s, 58 );
}

#pragma note( "Param Warning: in command_loaddefaults, s is unrefrenced" )
void command_loaddefaults( cSocket *s )
// PARAM WARNING: s is unreferenced
// Loads the server defaults.
{
	cwmWorldState->ServerData()->ResetDefaults();
}

void command_cq( cSocket *s )
// Display the counselor queue.
{
	Commands->showQue( s, false ); // Show the Counselor queue, not GM queue   
}

void command_cnext( cSocket *s )
// Attend to the next call in the counselor queue.
{
	Commands->nextCall( s, false ); // Show the Counselor queue, not GM queue   
}

void command_cclear( cSocket *s )
// Remove the current call from the counselor queue.
{
	Commands->closeCall( s, false ); // Show the Counselor queue, not GM queue   
	return;
}

#pragma note( "Param Warning: in command_minecheck, s is unrefrenced" )
void command_minecheck( cSocket *s )
// PARAM WARNING: s is unreferenced
// (d) Set the server mine check interval in minutes.
{
	if( Commands->GetNumArguments() == 2 )
		cwmWorldState->ServerData()->SetMineCheck( makenumber( 1 ) );
	return;
}

void command_invul( cSocket *s )
// Makes the targeted character immortal.
{
	s->AddX( 1 );
	target( s, 0, 179, 59 );
	
}

void command_noinvul( cSocket *s )
// Makes the targeted character mortal.
{
	s->AddX( 0 );
	target( s, 0, 179, 60 );
	
}

#pragma note( "Param Warning: in command_guardson, s is unrefrenced" )
void command_guardson( cSocket *s )
// PARAM WARNING: s is unreferenced
// Activates town guards.
{
	cwmWorldState->ServerData()->SetGuardStatus( true );
	sysbroadcast( Dictionary->GetEntry( 61 ) );
	
}

#pragma note( "Param Warning: in command_guardsoff, s is unrefrenced" )
void command_guardsoff( cSocket *s )
// PARAM WARNING: s is unreferenced
// Deactivates town guards.
{
	cwmWorldState->ServerData()->SetGuardStatus( false );
	sysbroadcast( Dictionary->GetEntry( 62 ) );
	
}

#pragma note( "Param Warning: in command_announceon, s is unrefrenced" )
void command_announceon( cSocket *s )
// PARAM WARNING: s is unreferenced
// Enable announcement of world saves.
{
	cwmWorldState->ServerData()->SetServerAnnounceSaves( true );
	cwmWorldState->announce( 1 );
	sysbroadcast( Dictionary->GetEntry( 63 ) );
	
}

#pragma note( "Param Warning: in command_announceoff, s is unrefrenced" )
void command_announceoff( cSocket *s )
// PARAM WARNING: s is unreferenced
// Disable announcement of world saves.
{
	cwmWorldState->ServerData()->SetServerAnnounceSaves( false );
	cwmWorldState->announce( 0 );
	sysbroadcast( Dictionary->GetEntry( 64 ) );
	
}

void command_wf( cSocket *s )
// Make the specified item worldforge compatible.
{
	if( Commands->GetNumArguments() == 2 )
	{
		s->AddID1( 0xFF );
		target( s, 0, 6, 65 );
	}
	return;
	
}

void command_decay( cSocket *s )
// Enables decay on an object.
{
	s->AddID( 1 );
	target( s, 0, 89, 66 );
	return;
	
}

void command_killall( cSocket *s )
// (d text) Kills all of a specified item.
{
	if( Commands->GetNumArguments() > 2 )
	{
		if( makenumber( 1 ) < 10 )
			Commands->KillAll( s, makenumber(1), (char *)&(s->TBuffer()[Commands->cmd_offset+9]) );
		else if( makenumber( 1 ) < 100 )
			Commands->KillAll( s, makenumber(1), (char *)&(s->TBuffer()[Commands->cmd_offset+10]) );
		else
			Commands->KillAll( s, makenumber(1), (char *)&(s->TBuffer()[Commands->cmd_offset+11]) );
	}
	return;
	
}

void command_pdump( cSocket *s )
// Display some performance information.
{
	sysmessage( s, "Performace Dump:" );
	
	sysmessage( s, "Network code: %fmsec [%i]", (R32)((R32)networkTime/(R32)networkTimeCount), networkTimeCount );
	sysmessage( s, "Timer code: %fmsec [%i]", (R32)((R32)timerTime/(R32)timerTimeCount), timerTimeCount );
	sysmessage( s, "Auto code: %fmsec [%i]", (R32)((R32)autoTime/(R32)autoTimeCount), autoTimeCount );
	sysmessage( s, "Loop Time: %fmsec [%i]", (R32)((R32)loopTime/(R32)loopTimeCount), loopTimeCount );
	sysmessage( s, "Simulation Cycles/Sec: %f", (1000.0*(1.0/(R32)((R32)loopTime/(R32)loopTimeCount ) ) ) );
}

void command_rename2( cSocket *s )
// (text) Rename an item or character.
{
	if( Commands->GetNumArguments() > 1 )
	{
		s->AddX( 1 );
		s->XText( (char *)&(s->TBuffer()[Commands->cmd_offset+8]) );
		target( s, 0, 1, 67 );
	} 
	return;
}

void command_readspawnregions( cSocket *s )
// Re-read the SPAWN.SCP file.
{
	FileIO->LoadSpawnRegions();
	sysmessage( s, 68 );
	return;
	
}

void command_gy( cSocket *s )
// (text) GM Yell - Announce a message to all online GMs.
{
	if( s == NULL ) 
		return;

	if( cwmWorldState->GetPlayersOnline() == 1 )
	{
		sysmessage( s, 69 );
		return;
	}
	char tmpString[512];
	CChar *me = s->CurrcharObj();
	if( me == NULL ) 
		return;

	sprintf( tmpString, "%s (GM ONLY): %s", me->GetName(), &(s->TBuffer()[Commands->cmd_offset+3]) );

	CSpeechEntry *toAdd = SpeechSys->Add();
	toAdd->Speech( tmpString );
	toAdd->Font( (FontType)me->GetFontType() );
	toAdd->Speaker( INVALIDSERIAL );
	toAdd->Colour( s->GetWord( 4 ) );
	toAdd->Type( SYSTEM );
	toAdd->At( cwmWorldState->GetUICurrentTime() );
	toAdd->TargType( SPTRG_BROADCASTALL );
	toAdd->CmdLevel( 2 );
}

void command_tilew( cSocket *s )
// (h h d d d d d) id id x1 x2 y1 y2 z - Tile an object using specified id & coordinates.
// <UL><LI>The first two numbers are hexidecimal, and are the ID codes for
// the item to tile.</LI>
// <LI>The second pair of numbers is the decimal coordinates of the upper
// left hand corner of the box being tiled. To get this value, go to the
// upper left hand corner and type /WHERE.</LI>
// <LI>The third pair of numbers is the lower right hand corner of the
// box being tiled.</LI>
// <LI>The final number is the Z-Axis of the box being tiled.</LI></UL>
{
	UI16 targID = INVALIDID;
	SI16 x1, x2, y1, y2;
	SI08 z;
	if( Commands->GetNumArguments() == 7 )
	{
		targID = (UI16)makenumber( 1 );
		x1 = (SI16)makenumber( 2 );
		x2 = (SI16)makenumber( 3 );
		y1 = (SI16)makenumber( 4 );
		y2 = (SI16)makenumber( 5 );
		z  = (SI08)makenumber( 6 );
	}
	else if( Commands->GetNumArguments() == 8 )
	{
		targID = (UI16)(( (makenumber( 1 ))<< 8 ) + makenumber( 2 ));
		x1 = (SI16)makenumber( 3 );
		x2 = (SI16)makenumber( 4 );
		y1 = (SI16)makenumber( 5 );
		y2 = (SI16)makenumber( 6 );
		z  = (SI08)makenumber( 7 );
	}
	else
	{
		sysmessage( s, 70 );
		return;
	}
	s->AddID1( (UI08)(targID>>8) );
	s->AddID2( (UI08)(targID%256) );
	CTile tile;
	Map->SeekTile( targID, &tile );
	bool pile = tile.Stackable();
	for( SI16 x = x1; x <= x2; x++ )
	{
		for( SI16 y = y1; y <= y2; y++ )
		{
			CItem *a = Items->SpawnItem( NULL, s->CurrcharObj(), 1, "#", pile, targID, 0, false, false );
			if( a != NULL )	// Antichrist crash prevention
			{
				a->SetPriv( 0 ); //Make them not decay
				a->SetLocation( x, y, z );
				RefreshItem( a );
			}
		}
	}
	s->AddID1( 0 );
	s->AddID2( 0 );
}  

void command_squelch( cSocket *s )
// (d / nothing) Squelchs specified player. (Makes them unnable to speak.)
{
	
	if( Commands->GetNumArguments() == 2 )
	{
		if( makenumber( 1 ) != 0 || makenumber( 1 ) != -1 )
		{
			s->AddID1( 0xFF );
			s->AddID2( (UI08)( makenumber( 1 ) ) );
		}
	}
	target( s, 0, 223, 71 );
	return;
	
}

void command_spawnkill( cSocket *s )
// (d) Kills spawns from the specified spawn region in SPAWN.SCP.
{
	if( Commands->GetNumArguments() == 2 )
	{
		Commands->KillSpawn( s, makenumber( 1 ) );
	}
	return;
}

void command_gotocur( cSocket *s)
// Goes to the current call in the GM/Counsellor Queue
{
	CChar *mChar = s->CurrcharObj();
	
	if( mChar->GetCallNum() == 0 )
		sysmessage( s, 72 );
	else
	{
		CChar *i = NULL;
		HelpRequest *currentPage = NULL;
		if( GMQueue->GotoPos( GMQueue->FindCallNum( mChar->GetCallNum() ) ) )
		{
			currentPage = GMQueue->Current();
			SERIAL serial = currentPage->WhoPaging();
			i = calcCharObjFromSer( serial );
			if( i != NULL )
			{
				sysmessage( s, 73 );
				mChar->SetLocation( i );
				mChar->Teleport();
			}  
			currentPage->IsHandled( true );
		}
		else if( CounselorQueue->GotoPos( CounselorQueue->FindCallNum( mChar->GetCallNum() ) ) )
		{
			currentPage = CounselorQueue->Current();
			SERIAL serial = currentPage->WhoPaging();
			i = calcCharObjFromSer( serial );
			if( i != NULL )
			{
				sysmessage( s, 73 );
				mChar->SetLocation( i );
				mChar->Teleport();
			}  
			currentPage->IsHandled( true );
		}
	}
}

void command_gmtransfer( cSocket *s )
// Escilate a Counsellor Page into the GM Queue
{
	CChar *mChar = s->CurrcharObj();
	if( mChar->GetCallNum() != 0 )
	{
		if( mChar->GetCommandLevel() >= CNS_CMDLEVEL )
		{
			HelpRequest pageToAdd;
			HelpRequest *currentPage = NULL;
			if( CounselorQueue->GotoPos( CounselorQueue->FindCallNum( mChar->GetCallNum() ) ) )
			{
				currentPage = CounselorQueue->Current();
				pageToAdd.Reason( currentPage->Reason() );
				pageToAdd.WhoPaging( currentPage->WhoPaging() );
				pageToAdd.IsHandled( false );
				pageToAdd.TimeOfPage( time( NULL ) );
				GMQueue->Add( &pageToAdd );
				sysmessage( s, 74 );
				Commands->closeCall( s, true );
			}
			else
				sysmessage( s, 75 );
		}
		else
			sysmessage( s, 76 );
	}
	else
		sysmessage( s, 72 );
}

void command_who( cSocket *s )
// Displays a list of users currently online.
{
	if( cwmWorldState->GetPlayersOnline() == 1 )
	{
		sysmessage( s, 69 );
		return;
	}
	int j = 0;
	
	Network->PushConn();
	sysmessage( s, "Who's online:" );
	for( cSocket *iSock = Network->FirstSocket(); !Network->FinishedSockets(); iSock = Network->NextSocket() )
	{
		CChar *iChar = iSock->CurrcharObj();
		j++;
		sysmessage( s, "%i) %s", ( j - 1 ), iChar->GetName() );
	}
	Network->PopConn();
	sysmessage( s, "Total: %i users online.", j );
}

void command_gms( cSocket *s )
{
	int j = 0;
	GumpDisplay Who( s, 400, 300 );
	char temp[512];
	Who.SetTitle( Dictionary->GetEntry( 77, s->Language() ) );
	Network->PushConn();
	for( cSocket *iSock = Network->FirstSocket(); !Network->FinishedSockets(); iSock = Network->NextSocket() )
	{
		CChar *iChar = iSock->CurrcharObj();
		j++;
		if( iChar->GetCommandLevel() >= CNS_CMDLEVEL )
		{
			sprintf( temp, "%i) %s", ( j - 1 ), iChar->GetName() );
			Who.AddData( temp, iChar->GetSerial(), 3 );
		}
	}
	Network->PopConn();
	Who.Send( 4, false, INVALIDSERIAL );
}

void command_regspawnall( cSocket *s )
{
	SI32 spawn = 0;
	UI16 i;
	cSpawnRegion *spawnReg = NULL;
	for( i = 1; i < cwmWorldState->GetTotalSpawnRegions(); i++ )
	{
		spawnReg = spawnregion[i];
		if( spawnReg != NULL )
			spawn += ( spawnReg->GetMaxSpawn() - spawnReg->GetCurrent() );
	}
	if( spawn > 250 )
	{
		sysmessage( s, 78 );
		return;
	}
	sysbroadcast( Dictionary->GetEntry( 79 ) );
	
	for( i = 1; i < cwmWorldState->GetTotalSpawnRegions(); i++ )
	{
		spawnReg = spawnregion[i];
		if( spawnReg != NULL )
		{
			SI32 k = ( spawnReg->GetMaxSpawn() - spawnReg->GetCurrent() );
			for( SI32 j = 1; j < k; j++ )
				spawnReg->doRegionSpawn();
			spawnReg->SetNextTime( BuildTimeValue(static_cast<R32>( 60 * RandomNum( spawnReg->GetMinTime(), spawnReg->GetMaxTime() ) )) );
		}
	}
	sysmessage( s, 80, spawn, cwmWorldState->GetTotalSpawnRegions() );
}

void command_wipenpcs( cSocket *s )
{
	CChar *mChar = s->CurrcharObj();
	if( mChar == NULL )
		return;

	int deleted = 0;
	Console.Print( Dictionary->GetEntry( 81 ), mChar->GetName() );
	
	for( CHARACTER j = 0; j < cwmWorldState->GetCharCount(); j++ )
	{
		if( chars[j].isFree() )
			continue;
		if( chars[j].IsNpc() && chars[j].GetNPCAiType() != 17 && !chars[j].IsTamed() ) // PV and pets don't wipe
		{			
			MapRegion->RemoveChar( &chars[j] );
			Npcs->DeleteChar( &chars[j] );
			deleted++;
		}
	}
	
	Console.Print( "\ndeleted: %i npcs\n", deleted );
	doGCollect();

	sysbroadcast( Dictionary->GetEntry( 82 ) );
}

void command_cleanup( cSocket *s )
{
	int corpses = 0;
	sysmessage( s, 83 );
	for( UI32 i = 0; i < cwmWorldState->GetItemCount(); i++ )
	{
		if( items[i].isCorpse() || items[i].GetType() == 51 || items[i].GetType() == 52 )
		{
			Items->DeleItem( &items[i] );
			corpses++;
		}
	}
	doGCollect();
	sysmessage( s, 84 );
	sysmessage( s, 85, corpses );
}

//o--------------------------------------------------------------------------o
//|	Function			-	void command_reloadaccounts( cSocket *s )
//|	Date					-	
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Reload accounts from the console interface.
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
void command_reloadaccounts( cSocket *s )
{
	if(Accounts->Load())
		sysmessage(s, 86);
	return;
}

void command_reportbug( cSocket *s )
// DESC:	Writes out a bug to the bug file
// DATE:	9th February, 2000
// CODER:	Abaddon
{
	FILE *bugs = fopen( "bugs.lst", "a+" );
	if( bugs == NULL )
	{
		Console.Error( 1, "Unable to open bug list bugs.lst!" );
		return;
	}
	CChar *mChar = s->CurrcharObj();
	if( mChar == NULL )
	{
		fclose(bugs);
		return;
	}
	char dateTime[1024];
	time_t ltime;
	time( &ltime );
	char *t = ctime(&ltime);
	// just to be paranoid and avoid crashing
	if( NULL == t )
		t = "";
	else
	{
		// some ctime()s like to stick \r\n on the end, we don't want that
		for( int end = strlen(t) - 1; end >= 0 && isspace( t[end] ); --end )
			t[end] = '\0';
	}
	safeCopy( dateTime, t, 1024);

	fprintf( bugs, "[%s] <%s> Reports: %s\n", dateTime, mChar->GetName(), &(s->TBuffer()[Commands->cmd_offset+9]) );
	fclose( bugs );
	sysmessage( s, 87 );
	bool x = false;
	Network->PushConn();
	for( cSocket *iSock = Network->FirstSocket(); !Network->FinishedSockets(); iSock = Network->NextSocket() )
	{
		CChar *iChar = iSock->CurrcharObj();
		if( iChar == NULL )
			continue;
		if( iChar->IsGMPageable() )
		{
			x = true;
			sysmessage( iSock, 277, mChar->GetName(), &(s->TBuffer()[Commands->cmd_offset + 9] ) );
		}
	}
	Network->PopConn();
	if( x )
		sysmessage( s, 88 );
	else
		sysmessage( s, 89 );
}

void command_delid( cSocket *s )
// (h h) Deletes all items in the world with a particular id
{
	UI16 tID = INVALIDID;
	switch( Commands->GetNumArguments() )
	{
	case 3:		tID = (UI16)( (makenumber( 1 )<<8) + makenumber( 2 ) );	break;
	case 2:		tID = (UI16)( makenumber( 1 ) );						break;
	default:	sysmessage( s, 1634 );									return;
	}

	for( UI32 i = 0; i < cwmWorldState->GetItemCount(); i++ )
	{
		if( items[i].GetID() == tID )
			Items->DeleItem( &items[i] );
	}
}

void command_forcewho( cSocket *s )
// Brings up an interactive listing of online users.
{
	WhoList->ZeroWho();
	WhoList->SendSocket( s );
	return;
}

void command_cy( cSocket *s )
// (text) Counselor Yell - Announce a message to all online counselors and above.
{
	if( cwmWorldState->GetPlayersOnline() == 1 )
	{
		sysmessage( s, 69 );
		return;
	}
	char tmpString[512];
	sprintf( tmpString, "(CY): %s", &(s->TBuffer()[Commands->cmd_offset+3] ) );

	CChar *me = s->CurrcharObj();
	CSpeechEntry *toAdd = SpeechSys->Add();
	toAdd->Speech( tmpString );
	toAdd->Font( (FontType)me->GetFontType() );
	toAdd->Speaker( me->GetSerial() );
	toAdd->Colour( s->GetWord( 4 ) );
	toAdd->Type( SYSTEM );
	toAdd->At( cwmWorldState->GetUICurrentTime() );
	toAdd->TargType( SPTRG_BROADCASTPC );
	toAdd->CmdLevel( 1 );
}
void command_sgy( cSocket *s )
// (text) GM Yell - Announce a message to all online GMs.
{
	if( cwmWorldState->GetPlayersOnline() == 1 )
	{
		sysmessage( s, 69 );
		return;
	}
	char tmpString[512];
	sprintf( tmpString, "(SuperGM): %s", &(s->TBuffer()[Commands->cmd_offset+4]) );

	CChar *me = s->CurrcharObj();
	CSpeechEntry *toAdd = SpeechSys->Add();
	toAdd->Speech( tmpString );
	toAdd->Font( (FontType)me->GetFontType() );
	toAdd->Speaker( me->GetSerial() );
	toAdd->Colour( s->GetWord( 4 ) );
	toAdd->Type( SYSTEM );
	toAdd->At( cwmWorldState->GetUICurrentTime() );
	toAdd->TargType( SPTRG_BROADCASTPC );
	toAdd->CmdLevel( 3 );
}

void command_validcmd( cSocket *s )
{
	CChar *mChar = s->CurrcharObj();
	UI08 targetCommand = mChar->GetCommandLevel();
	GumpDisplay targetCmds( s, 300, 300 );
	targetCmds.SetTitle( "Valid Commands" );

	map< string, cmdtable_mapentry >::iterator myCounter;

	myCounter = cmd_table.begin();
	while( myCounter != cmd_table.end() )
	{
		if( myCounter->second.cmdLevelReq <= targetCommand )
			targetCmds.AddData( myCounter->first.c_str(), myCounter->second.cmdLevelReq );
		myCounter++;
	};

	targetCmds.Send( 4, false, INVALIDSERIAL );
}

void command_nacct( cSocket *s )
{
	char commandExecuted[20];
	char *commandStart, *commandEnd;
	commandStart = (char *)&(s->TBuffer()[Commands->cmd_offset+6]);
	commandEnd = commandStart;
	while( *(commandEnd) != ' ' && *(commandEnd) != 0x00 )
	{
		commandEnd++;
	}
	strncpy( commandExecuted, commandStart, commandEnd - commandStart );
	strupr( commandExecuted );
	sysmessage( s, commandExecuted );
	if( !strcmp( commandExecuted, "PLAYER" ) )
		target( s, 0, 54, 278 );
	else if( !strcmp( commandExecuted, "ADD" ) )
		Gumps->Menu( s, 8 );
	else if( !strcmp( commandExecuted, "BAN" ) )
	{
	}
	else if( !strcmp( commandExecuted, "EDIT" ) )
		Gumps->Menu( s, 10 );
	else if( !strcmp( commandExecuted, "VIEW" ) )
		Gumps->Menu( s, 9 );
	else if( !strcmp( commandExecuted, "LIST" ) )
	{
//		Accounts->SendAccountsAsGump( s );
	}
	else	// by default list them
		Gumps->Menu( s, 7 );
}

void command_make( cSocket *sock )
{
	sock->XText( (char *)&(sock->TBuffer()[Commands->cmd_offset + 5]) );
	target( sock, 0, 70, 279 );
}

void command_howto( cSocket *sock )
{
	char temp[1024];
	char *commandStart = (char *)&(sock->TBuffer()[Commands->cmd_offset+6]);
	if( commandStart == NULL || strlen( commandStart ) == 0 )
	{
		CChar *mChar = sock->CurrcharObj();
		if( mChar == NULL )
			return;

		int iCmd = 2;
		int numAdded = 0;
		UI08 pagenum = 1;
		UI16 position = 40;
		UI16 linenum = 1;
		STRINGLIST one, two;	
		char tempString[128];
		
		one.push_back( "noclose" );
		sprintf( tempString, "resizepic 0 0 %i 480 320", cwmWorldState->ServerData()->GetBackgroundPic() );
		one.push_back( tempString );
		one.push_back( "page 0" );
		one.push_back( "text 200 20 0 0" );
		two.push_back( "HOWTO" );
		sprintf( tempString, "button 440 20 %i %i 1 0 1", cwmWorldState->ServerData()->GetButtonCancel(), cwmWorldState->ServerData()->GetButtonCancel() + 1 );
		one.push_back( tempString );

		UI08 currentLevel = mChar->GetCommandLevel();
		CmdTableIterator gAdd = cmd_table.begin();
		
		one.push_back( "page 1" );

		bool justDonePageAdd = false;
		while( gAdd != cmd_table.end() )
		{
			if( numAdded > 0 && !(numAdded%10) && !justDonePageAdd )
			{
				position = 40;
				pagenum++;
				sprintf( temp, "page %i", pagenum );
				one.push_back( temp );
				justDonePageAdd = true;
			}
			if( gAdd->second.cmdLevelReq <= currentLevel )
			{
				justDonePageAdd = false;
				sprintf( temp, "text 50 %i %i %i", position, cwmWorldState->ServerData()->GetLeftTextColour(), linenum ); 
				one.push_back( temp );
				sprintf( temp, "button 20 %i %i %i %i 0 %i", position, cwmWorldState->ServerData()->GetButtonRight(), cwmWorldState->ServerData()->GetButtonRight() + 1, pagenum, iCmd );
				one.push_back( temp );
				two.push_back( gAdd->first.c_str() );
				numAdded++;
				linenum++;
				position += 20;
			}
			iCmd++;
			gAdd++;
		}

		pagenum = 1; 
		for( int i = 0; i < numAdded; i += 10 )
		{
			sprintf( temp, "page %i", pagenum );
			one.push_back( temp );
			if( i >= 10 )
			{
				sprintf( temp, "button 30 290 %i %i 0 %i", cwmWorldState->ServerData()->GetButtonLeft(), cwmWorldState->ServerData()->GetButtonLeft() + 1, pagenum - 1 ); //back button
				one.push_back( temp );
			}
			if( ( numAdded > 10 ) && ( ( i + 10 ) < numAdded ) )
			{
				sprintf( temp, "button 440 290 %i %i 0 %i", cwmWorldState->ServerData()->GetButtonRight(), cwmWorldState->ServerData()->GetButtonRight() + 1, pagenum + 1 );
				one.push_back( temp );
			}
			pagenum++;
		}

		SendVecsAsGump( sock, one, two, 13, INVALIDSERIAL );
	}
	else
	{
		int i = 0;
		CmdTableIterator toFind = cmd_table.begin();
		strupr( commandStart );	// need to convert to uppercase to compare
		while( toFind != cmd_table.end() )
		{
			if( commandStart[0] == toFind->first.c_str()[0] )
			{
				if( !strcmp( commandStart, toFind->first.c_str() ) )
					break;
			}
			i++;
			toFind++;
		}
		if( toFind == cmd_table.end() )
			sysmessage( sock, 280 );
		else
			HandleHowTo( sock, i );
	}
}

void HandleHowTo( cSocket *sock, int cmdNumber )
{
	int iCounter = 0;
	CmdTableIterator toFind = cmd_table.begin();
	while( iCounter != cmdNumber && toFind != cmd_table.end() )
	{
		iCounter++;
		toFind++;
	}
	if( iCounter == cmdNumber )
	{
		// Build gump structure here, with basic information like Command Level, Name, Command Type, and parameters (if any, from table)
		GumpDisplay CommandInfo( sock, 480, 320 );
		CommandInfo.SetTitle( toFind->first.c_str() );

		CommandInfo.AddData( "Minimum Command Level", toFind->second.cmdLevelReq );
		switch( toFind->second.cmdType )
		{
		case CMD_ITEMMENU:		CommandInfo.AddData( "Syntax", "None (opens item menu)" );				break;
		case CMD_TARGET:		CommandInfo.AddData( "Syntax", "None (generic target)" );				break;
		case CMD_FUNC:			CommandInfo.AddData( "Syntax", "None (generic command)" );				break;
		case CMD_TARGETX:
		case CMD_TARGETID1:
		case CMD_TARGETTMP:		CommandInfo.AddData( "Syntax", "arg1 (hex or decimal)" );				break;
		case CMD_TARGETXY:
		case CMD_TARGETID2:		CommandInfo.AddData( "Syntax", "arg1 arg2 (hex or decimal)" );				break;
		case CMD_TARGETXYZ:
		case CMD_TARGETID3:		CommandInfo.AddData( "Syntax", "arg1 arg2 arg3 (hex or decimal)" );			break;
		case CMD_TARGETID4:		CommandInfo.AddData( "Syntax", "arg1 arg2 arg3 arg4 (hex or decimal)" );	break;
		}

		char filename[256];
		sprintf( filename, "help/commands/%s.txt", toFind->first.c_str() );
		std::ifstream toOpen( filename );
		if( !toOpen.is_open() )
			CommandInfo.AddData( "", "No extra information is available about this command", 7 );
		else
		{
			char cmdLine[128];
			while( !toOpen.eof() && !toOpen.fail() )
			{
				toOpen.getline( cmdLine, 128 );
				if( cmdLine[0] != 0 )
					CommandInfo.AddData( "", cmdLine, 7 );
			};
			toOpen.close();
		}
		CommandInfo.Send( 4, false, INVALIDSERIAL );
	}
	else
		sysmessage( sock, 280 );
}

// command_xgate - Gates from locations.scp - Retalin 9/17/2001
void command_xgate( cSocket *s )
{
	if( s == NULL )
		return;
	CChar *mChar = s->CurrcharObj();
	if( mChar == NULL )
		return;
	if( Commands->GetNumArguments() == 2 )
	{
		Commands->MakePlace( s, makenumber( 1 ) );
		if( s->AddX() != 0 )
			SpawnGate( s, mChar, mChar->GetX(), mChar->GetY(), mChar->GetZ(), static_cast<SI16>(s->AddX()), static_cast<SI16>(s->AddY()), s->AddZ() );
	}
}

void command_reloaddefs( cSocket *s )
{
	if( s == NULL )
		return;

	sysbroadcast( "Possible pause... reloading server scripts" );
	sysmessage( s, "Deallocating old scripts..." );
	SpeechSys->Poll();
	delete FileLookup;
	sysmessage( s, "Reloading new scripts..." );
	SpeechSys->Poll();
	if(( FileLookup = new cServerDefinitions() ) == NULL )	
	{
		sysbroadcast( "Server scripts reload failure, shutting server down" );
		Shutdown( FATAL_UOX3_ALLOC_SCRIPTS );
	}
	else
		sysbroadcast( "Server scripts reloaded" );
	SpeechSys->Poll();
}

void command_temp( cSocket *s )
{
	if( s == NULL )
		return;
	CChar *mChar = s->CurrcharObj();
	if( mChar == NULL )
		return;
	UI08 reg = mChar->GetRegion();
	weathID toGrab = region[reg]->GetWeather();
	if( toGrab != 0xFF )
	{
		R32 curTemp = Weather->Temp( toGrab );
		sysmessage( s, "It is currently %f degrees", curTemp );
	}
}

void command_status( cSocket *s )
// Opens the HTML status information gump
{
	HTMLTemplates->TemplateInfoGump( s );
}
