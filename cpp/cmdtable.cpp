//------------------------------------------------------------------------
//  cmdtable.cpp
//
//------------------------------------------------------------------------
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 1999 - 2001 by Unknown real name (Crackerjack)
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//	
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//   
//------------------------------------------------------------------------

/*
* This code is an attempt to clean up the messy "if/then/else" routines
* currently in use for GM commands, as well as adding more functionality
* and more potential for functionality.
*
* Current features:
* - Actual table of commands to execute, what perms are required, dialog
*   messages for target commands, etc handled by a central system
*
*/

#include <uox3.h>
#include <cmdtable.h>
#include <debug.h>

extern int tempint[MAXCLIENT];

TARGET_S target_acct = { 0, 1, 0, 54, "Select the player." };	// 310 will never be found, range is 0 - 255
TARGET_S target_use = { 0, 1, 0, 24, "What object will you use?" };
TARGET_S target_jail = { 0, 1, 0, 126, "Select player to jail." };
TARGET_S target_release = { 0, 1, 0, 127, "Select player to release from jail." };
TARGET_S target_istats = { 0, 1, 0, 12, "Select item to inspect." };
TARGET_S target_cstats = { 0, 1, 0, 13, "Select char to inspect." };
TARGET_S target_wstats = { 0, 1, 0, 55, "Select char to inspect." };
TARGET_S target_tele = { 0, 1, 0, 2, "Select teleport target." };
TARGET_S target_xbank = { 0, 1, 0, 107, "Select target to open bank of." };
TARGET_S target_xsbank = { 0, 1, 0, 107, "Select target to open special bank of." }; // AntiChrist
TARGET_S target_remove = { 0, 1, 0, 3, "Select item to remove." };
TARGET_S target_makegm = { 0, 1, 0, 14, "Select character to make a GM." };
TARGET_S target_makecns = { 0, 1, 0, 15, "Select character to make a Counselor." };
TARGET_S target_killhair = { 0, 1, 0, 16, "Select character for cutting hair." };
TARGET_S target_killbeard = { 0, 1, 0, 17, "Select character for shaving." };
TARGET_S target_kill = { 0, 1, 0, 20, "Select character to kill." };
TARGET_S target_resurrect = { 0, 1, 0, 21, "Select character to resurrect." };
TARGET_S target_bolt = { 0, 1, 0, 22, "Select character to bolt." };
// This fires a harmless bolt at the user.
TARGET_S target_kick = { 0, 1, 0, 25, "Select character to kick." };
// This disconnects the player targeted from the game. They
// can still log back in.
TARGET_S target_movetobag = { 0, 1, 0, 111, "Select an item to move into your bag." };
TARGET_S target_xgo = { 0, 1, 0, 8, "Select char to teleport." };
TARGET_S target_setmorex = { 0, 1, 0, 63, "Select object to set morex on." };
TARGET_S target_setmorey = { 0, 1, 0, 64, "Select object to set morey on." };
TARGET_S target_setmorez = { 0, 1, 0, 65, "Select object to set morez on." };
TARGET_S target_setmorexyz = { 0, 1, 0, 66, "Select object to set morex, morey, and morez on." };
TARGET_S target_sethexmorexyz = { 0, 1, 0, 66, "Select object to set hex morex, morey, and morez on." };
TARGET_S target_setnpcai = { 0, 1, 0, 106, "Select npc to set AI type on." };
TARGET_S target_newz = { 0, 1, 0, 5, "Select item to reposition." };
TARGET_S target_settype = { 0, 1, 0, 6, "Select item to edit type." };
TARGET_S target_itrig = { 0, 1, 0, 200, "Select item to trigger." };
TARGET_S target_ctrig = { 0, 1, 0, 201, "Select NPC to trigger." };
TARGET_S target_ttrig = { 0, 1, 0, 202, "Select item to set trigger type." };
TARGET_S target_setid = { 0, 1, 0, 7, "Select item to polymorph." };
TARGET_S target_setmore = { 0, 1, 0, 10, "Select item to edit 'more' value." };
TARGET_S target_setfont = { 0, 1, 0, 19, "Select character to change font." };
TARGET_S target_npcaction = { 0, 1, 0, 53, "Select npc to make act." };
TARGET_S target_setamount = { 0, 1, 0, 23, "Select item to edit amount." };
TARGET_S target_setamount2 = { 0, 1, 0, 129, "Select item to edit amount." };
TARGET_S target_setmovable = { 0, 1, 0, 28, "Select item to edit mobility." };
TARGET_S target_setvisible = { 0, 1, 0, 61, "Select item to edit visibility." };
TARGET_S target_setdir = { 0, 1, 0, 88, "Select item to edit direction." };
TARGET_S target_setspeech = { 0, 1, 0, 135, "Select NPC to edit speech." };
TARGET_S target_setowner = { 0, 1, 0, 30, "Select NPC or OBJECT to edit owner." };
TARGET_S target_freeze = { 0, 1, 0, 34, "Select player to freeze in place." };
TARGET_S target_unfreeze = { 0, 1, 0, 35, "Select player to unfreeze." };
TARGET_S target_tiledata = { 0, 1, 0, 46, "Select item to inspect." };
TARGET_S target_recall = { 0, 1, 0, 38, "Select rune from which to recall." };
TARGET_S target_mark = { 0, 1, 0, 39, "Select rune to mark." };
TARGET_S target_gate = { 0, 1, 0, 43, "Select rune from which to gate." };
TARGET_S target_heal = { 0, 1, 0, 44, "Select person to heal." };
TARGET_S target_npctarget = { 0, 1, 0, 56, "Select player for the NPC to follow." };
TARGET_S target_tweak = { 0, 1, 0, 62, "Select item or character to tweak." };
TARGET_S target_sbopen = { 0, 1, 0, 87, "Select spellbook to open as a container." };
TARGET_S target_mana = { 0, 1, 0, 113, "Select person to restore mana to." };
TARGET_S target_stamina = { 0, 1, 0, 114, "Select person to refresh." };
TARGET_S target_makeshop = { 0, 1, 0, 116, "Select the character to add shopkeeper buy containers to." };
TARGET_S target_buy = { 0, 1, 0, 121, "Select the shopkeeper you'd like to buy from." };
TARGET_S target_setvalue = { 0, 1, 0, 122, "Select item to edit value." };
TARGET_S target_setrestock = { 0, 1, 0, 123, "Select item to edit amount to restock." };
TARGET_S target_sell = { 0, 1, 0, 112, "Select the NPC to sell to." };
TARGET_S target_setspattack = { 0, 1, 0, 150, "Select creature to set SPATTACK on." };
TARGET_S target_setspadelay = { 0, 1, 0, 177, "Select creature to set SPADELAY on." };
TARGET_S target_setpoison = { 0, 1, 0, 175, "Select creature to set POISON." };
TARGET_S target_setpoisoned = { 0, 1, 0, 176, "Select creature to set POISONED." };
TARGET_S target_setadvobj = { 0, 1, 0, 178, "Select creature to set ADVOBJ." };
TARGET_S target_setwipe = { 0, 1, 0, 133, "Select item to modify." };
TARGET_S target_fullstats = { 0, 1, 0, 151, "Select creature to restore full stats." };
TARGET_S target_hide = { 0, 1, 0, 131, "Select creature to hide." };
TARGET_S target_unhide = { 0, 1, 0, 132, "Select creature to reveal." };
TARGET_S target_house = { 0, 1, 0, 207, "Select location for house." };
TARGET_S target_split = { 0, 1, 0, 209, "Select creature to make able to split." };
TARGET_S target_splitchance = { 0, 1, 0, 210, "Select creature to set it's chance of splitting." };
TARGET_S target_possess = { 0, 1, 0, 212, "Select creature to possess." };
TARGET_S target_telestuff = { 0, 1, 0, 222, "Select player/object to teleport." };
TARGET_S target_killpack = { 0, 1, 0, 18, "Select character to remove pack." };
TARGET_S target_trainer = { 0, 1, 0, 206, "Select character to become a trainer." };
TARGET_S target_ban = { 0, 1, 0, 235, "Select character to BAN." };
TARGET_S target_newx = { 0, 1, 0, 251, "Select item to reposition." };
TARGET_S target_newy = { 0, 1, 0, 252, "Select item to reposition." };
TARGET_S target_incx = { 0, 1, 0, 253, "Select item to reposition." };
TARGET_S target_incy = { 0, 1, 0, 254, "Select item to reposition." };
TARGET_S target_incz = { 0, 1, 0, 250, "Select item to reposition." };
TARGET_S target_glow = { 0, 1, 0, 255, "Select item to make glowing." };
TARGET_S target_unglow = { 0, 1, 0, 249, "Select item to deactivate glowing." };
TARGET_S target_showskills = { 0, 1, 0, 247, "Select char to see skills" };
TARGET_S target_showdetail = { 0, 1, 0, 48, "Select item to get detailed info on" };
TARGET_S target_removeshop = { 0, 1, 0, 105, "Select shopkeeper to remove shop packs from" };
TARGET_S target_commandlevel = { 0, 1, 0, 101, "Select which player you want to adjust the command level of" };

CMDTABLE_S command_table[] = {
	{"USE",		2,	1,	CMD_TARGET,	(CMD_DEFINE)&target_use},
	{"RESEND",	1,	2,	CMD_FUNC,	(CMD_DEFINE)&command_resend},
	{"POST",    1,  2,  CMD_FUNC,   (CMD_DEFINE)&command_post},
	{"GPOST",   1,  2,  CMD_FUNC,   (CMD_DEFINE)&command_gpost},
	{"RPOST",   1,  2,  CMD_FUNC,   (CMD_DEFINE)&command_rpost},
	{"LPOST",   1,  2,  CMD_FUNC,   (CMD_DEFINE)&command_lpost},
	{"POINT",	1,	3,	CMD_FUNC,	(CMD_DEFINE)&command_teleport},
	{"WHERE",	1,	4,	CMD_FUNC,	(CMD_DEFINE)&command_where},
	{"ADDU",	2,	5,	CMD_ITEMMENU,	(CMD_DEFINE)1}, // Opens the GM add menu.
	{"Q",		1,	6,	CMD_FUNC,	(CMD_DEFINE)&command_q},
	{"NEXT",	1,	7,	CMD_FUNC,	(CMD_DEFINE)&command_next},
	{"CLEAR",	1,	8,	CMD_FUNC,	(CMD_DEFINE)&command_clear},
	{"GOTOCUR",	1,	9,	CMD_FUNC,	(CMD_DEFINE)&command_gotocur},
	{"GMTRANSFER",	1,	10,	CMD_FUNC,	(CMD_DEFINE)&command_gmtransfer},
	{"JAIL",	1,	11,	CMD_TARGET,	(CMD_DEFINE)&target_jail},
	{"RELEASE",	1,	12,	CMD_TARGET,	(CMD_DEFINE)&target_release},
	{"ISTATS",	1,	13,	CMD_TARGET,	(CMD_DEFINE)&target_istats},
	{"CSTATS",	1,	14,	CMD_TARGET,	(CMD_DEFINE)&target_cstats},
	{"WSTATS",	1,	55,	CMD_TARGET,	(CMD_DEFINE)&target_wstats},
	{"GOPLACE",	1,	15,	CMD_FUNC,	(CMD_DEFINE)&command_goplace},
	{"GOCHAR",	1,	16,	CMD_FUNC,	(CMD_DEFINE)&command_gochar},
	{"FIX",		1,	17,	CMD_FUNC,	(CMD_DEFINE)&command_fix},
	{"XGOPLACE",	2,	18,	CMD_FUNC,	(CMD_DEFINE)&command_xgoplace},
	{"SHOWIDS",	2,	19,	CMD_FUNC,	(CMD_DEFINE)&command_showids},
	{"POLY",	2,	20,	CMD_FUNC,	(CMD_DEFINE)&command_poly},
	{"SKIN",	2,	21,	CMD_FUNC,	(CMD_DEFINE)&command_skin},
	{"ACTION",	2,	22,	CMD_FUNC,	(CMD_DEFINE)&command_action},
	{"TELE",	2,	23,	CMD_TARGET,	(CMD_DEFINE)&target_tele},
	{"XTELE",	1,	24,	CMD_FUNC,	(CMD_DEFINE)&command_xtele},
	{"GO",		2,	25,	CMD_FUNC,	(CMD_DEFINE)&command_go},
	{"XGO",		2,	26,	CMD_TARGETXYZ,	(CMD_DEFINE)&target_xgo},
	{"SETMOREX",	2,	27,	CMD_TARGETX,	(CMD_DEFINE)&target_setmorex},
	{"SETMOREY",	2,	28,	CMD_TARGETX,	(CMD_DEFINE)&target_setmorey},
	{"SETMOREZ",	2,	29,	CMD_TARGETX,	(CMD_DEFINE)&target_setmorez},
	{"ZEROKILLS",	2,	30,	CMD_FUNC,	(CMD_DEFINE)&command_zerokills},
	{"SETMOREXYZ",	2,	31,	CMD_TARGETXYZ,	(CMD_DEFINE)&target_setmorexyz},
	{"SETHEXMOREXYZ",2,	0,	CMD_TARGETHXYZ,	(CMD_DEFINE)&target_sethexmorexyz},
	{"SETNPCAI",	2,	1,	CMD_TARGETX,	(CMD_DEFINE)&target_setnpcai},
	{"XBANK",	2,	2,	CMD_TARGET,	(CMD_DEFINE)&target_xbank},
	{"XSBANK",   2,  2,  CMD_TARGET, (CMD_DEFINE)&target_xsbank }, 
	{"TILE",	2,	3,	CMD_FUNC,	(CMD_DEFINE)&command_tile},
	{"WIPE",	2,	4,	CMD_FUNC,	(CMD_DEFINE)&command_wipe},
	{"IWIPE",	2,	5,	CMD_FUNC,	(CMD_DEFINE)&command_iwipe},
	{"ADD",		2,	6,	CMD_FUNC,	(CMD_DEFINE)&command_add},
	{"ADDX",	2,	7,	CMD_FUNC,	(CMD_DEFINE)&command_addx},
	{"RENAME",	2,	8,	CMD_FUNC,	(CMD_DEFINE)&command_rename},
	{"TITLE",	2,	9,	CMD_FUNC,	(CMD_DEFINE)&command_title},
	{"SAVE",	2,	10,	CMD_FUNC,	(CMD_DEFINE)&command_save},
	{"ACCT",    2,   1, CMD_TARGET, (CMD_DEFINE)&target_acct},
	{"REMOVE",	2,	11,	CMD_TARGET,	(CMD_DEFINE)&target_remove},
	{"TRAINER",	2,	12,	CMD_TARGET,	(CMD_DEFINE)&target_trainer},
	{"DYE",		2,	13,	CMD_FUNC,	(CMD_DEFINE)&command_dye},
	{"NEWZ",	2,	14,	CMD_TARGETX,	(CMD_DEFINE)&target_newz},
	{"SETTYPE",	2,	15,	CMD_TARGETID1,	(CMD_DEFINE)&target_settype},
	{"ITRIG",	2,	16,	CMD_TARGETX,	(CMD_DEFINE)&target_itrig},
	{"CTRIG",	2,	17,	CMD_TARGETX,	(CMD_DEFINE)&target_ctrig},
	{"TTRIG",	2,	18,	CMD_TARGETX,	(CMD_DEFINE)&target_ttrig},
	{"WTRIG",	2,	19,	CMD_FUNC,	(CMD_DEFINE)&command_wtrig},
	{"SETID",	2,	20,	CMD_TARGETHID2,	(CMD_DEFINE)&target_setid},
	{"SETPRIV",	2,	21,	CMD_FUNC,	(CMD_DEFINE)&command_setpriv},
	{"NODECAY",	2,	30,	CMD_FUNC,	(CMD_DEFINE)&command_nodecay},
#ifdef UNRELEASED
	{"SEND",	0,	0,	CMD_FUNC,	(CMD_DEFINE)&command_send},
#endif
	{"SHOWTIME",	1,	23,	CMD_FUNC,	(CMD_DEFINE)&command_showtime},
	{"SETTIME",	2,	2,	CMD_FUNC,	(CMD_DEFINE)&command_settime},
	{"SETMORE",	2,	24,	CMD_TARGETHID4,	(CMD_DEFINE)&target_setmore},
	{"SHUTDOWN",	2,	25,	CMD_FUNC,	(CMD_DEFINE)&command_shutdown},
	{"MAKEGM",	2,	26,	CMD_TARGET,	(CMD_DEFINE)&target_makegm},
	{"MAKECNS",	2,	27,	CMD_TARGET,	(CMD_DEFINE)&target_makecns},
	{"KILLHAIR",	2,	28,	CMD_TARGET,	(CMD_DEFINE)&target_killhair},
	{"KILLBEARD",	2,	29,	CMD_TARGET,	(CMD_DEFINE)&target_killbeard},
	{"KILLPACK",	2,	30,	CMD_TARGET,	(CMD_DEFINE)&target_killpack},
	{"SETFONT",	2,	31,	CMD_TARGETHID1,	(CMD_DEFINE)&target_setfont},
	{"WHOLIST",	2,	0,	CMD_FUNC,	(CMD_DEFINE)&command_wholist},
	{"KILL",	2,	1,	CMD_TARGET,	(CMD_DEFINE)&target_kill},
	{"RESURRECT",	2,	2,	CMD_TARGET,	(CMD_DEFINE)&target_resurrect},
	{"BOLT",	2,	3,	CMD_TARGET,	(CMD_DEFINE)&target_bolt},
#ifdef UNRELEASED
	{"BLT2",	2,	0,	CMD_FUNC,	(CMD_DEFINE)&command_blt2},
#endif
	{"SFX",		2,	4,	CMD_FUNC,	(CMD_DEFINE)&command_sfx},
	{"NPCACTION",	2,	5,	CMD_TARGETHID1,	(CMD_DEFINE)&target_npcaction},
	{"LIGHT",	2,	6,	CMD_FUNC,	(CMD_DEFINE)&command_light},
	{"SETAMOUNT",	2,	7,	CMD_TARGETX,	(CMD_DEFINE)&target_setamount},
	{"SETAMOUNT2",	2,	8,	CMD_TARGETX,	(CMD_DEFINE)&target_setamount2},
#ifdef UNRELEASED
	{"WEB",		2,	0,	CMD_FUNC,	(CMD_DEFINE)&command_web},
#endif
	{"DISCONNECT",	2,	9,	CMD_FUNC,	(CMD_DEFINE)&command_disconnect},
	{"KICK",	2,	10,	CMD_TARGET,	(CMD_DEFINE)&target_kick},
	{"TELL",	2,	11,	CMD_FUNC,	(CMD_DEFINE)&command_tell},
	/* lord binary's debugging comands - commented out by default, but gotta
	be there I guess... */
	// {"NPCS",	255,	0,	CMD_FUNC,	(CMD_DEFINE)&command_npcs},
	// {"NPCSD",	255,	0,	CMD_FUNC,	(CMD_DEFINE)&command_npcsd},
	// {"PCS",		255,	0,	CMD_FUNC,	(CMD_DEFINE)&command_pcs},
	// {"CHARC",	255,	0,	CMD_FUNC,	(CMD_DEFINE)&command_charc},
	// {"ITEMC",	255,	0,	CMD_FUNC,	(CMD_DEFINE)&command_itemc},
	/* end of lord binary's debugging commands */
	{"GMMENU",	2,	15,	CMD_FUNC,	(CMD_DEFINE)&command_gmmenu},
	{"ITEMMENU",	2,	16,	CMD_FUNC,	(CMD_DEFINE)&command_itemmenu},
	{"ADDITEM",	2,	17,	CMD_FUNC,	(CMD_DEFINE)&command_additem},
	{"DUPE",	2,	18,	CMD_FUNC,	(CMD_DEFINE)&command_dupe},
	{"MOVETOBAG",	2,	19,	CMD_TARGET,	(CMD_DEFINE)&target_movetobag},
	{"COMMAND",	2,	20,	CMD_FUNC,	(CMD_DEFINE)&command_command},
	{"GCOLLECT",	2,	21,	CMD_FUNC,	(CMD_DEFINE)&command_gcollect},
	{"ALLMOVEON",	2,	22,	CMD_FUNC,	(CMD_DEFINE)&command_allmoveon},
	{"ALLMOVEOFF",	2,	23,	CMD_FUNC,	(CMD_DEFINE)&command_allmoveoff},
	{"SHOWHS",	2,	24,	CMD_FUNC,	(CMD_DEFINE)&command_showhs},
	{"HIDEHS",	2,	25,	CMD_FUNC,	(CMD_DEFINE)&command_hidehs},
	{"SETMOVABLE",	2,	26,	CMD_TARGETX,	(CMD_DEFINE)&target_setmovable},
	{"SET",		2,	27,	CMD_FUNC,	(CMD_DEFINE)&command_set},
	{"SETVISIBLE",	2,	28,	CMD_TARGETX,	(CMD_DEFINE)&target_setvisible},
	{"SETDIR",	2,	29,	CMD_TARGETX,	(CMD_DEFINE)&target_setdir},
	{"SETSPEECH",	2,	30,	CMD_TARGETX,	(CMD_DEFINE)&target_setspeech},
	{"SETOWNER",	2,	31,	CMD_TARGETHID4,	(CMD_DEFINE)&target_setowner},
	{"ADDNPC",	2,	0,	CMD_FUNC,	(CMD_DEFINE)&command_addnpc},
	{"FREEZE",	2,	1,	CMD_TARGET,	(CMD_DEFINE)&target_freeze},
	{"UNFREEZE",	2,	2,	CMD_TARGET,	(CMD_DEFINE)&target_unfreeze},
	{"READINI",	2,	3,	CMD_FUNC,	(CMD_DEFINE)&command_readini},
	{"GUMPMENU",	2,	4,	CMD_FUNC,	(CMD_DEFINE)&command_gumpmenu},
	{"TILEDATA",	2,	5,	CMD_TARGET,	(CMD_DEFINE)&target_tiledata},
	{"RECALL",	2,	6,	CMD_TARGET,	(CMD_DEFINE)&target_recall},			// these need to be updated for new magic system (Abaddon)
	{"MARK",	2,	7,	CMD_TARGET,	(CMD_DEFINE)&target_mark},
	{"GATE",	2,	8,	CMD_TARGET,	(CMD_DEFINE)&target_gate},
	{"HEAL",	2,	9,	CMD_TARGET,	(CMD_DEFINE)&target_heal},				// to here
	{"NPCTARGET",	2,	10,	CMD_TARGET,	(CMD_DEFINE)&target_npctarget},
	{"CACHESTATS",	2,	11,	CMD_FUNC,	(CMD_DEFINE)&command_cachestats},
	{"NPCRECT",	2,	12,	CMD_FUNC,	(CMD_DEFINE)&command_npcrect},
	{"NPCCIRCLE",	2,	13,	CMD_FUNC,	(CMD_DEFINE)&command_npccircle},
	{"NPCWANDER",	2,	14,	CMD_FUNC,	(CMD_DEFINE)&command_npcwander},
	{"TWEAK",	2,	15,	CMD_TARGET,	(CMD_DEFINE)&target_tweak},
	{"SBOPEN",	2,	16,	CMD_TARGET,	(CMD_DEFINE)&target_sbopen},
	{"SECONDSPERUOMINUTE",	2,	17,	CMD_FUNC,	(CMD_DEFINE)&command_secondsperuominute},
	{"BRIGHTLIGHT",	2,	18,	CMD_FUNC,	(CMD_DEFINE)&command_brightlight},
	{"DARKLIGHT",	2,	19,	CMD_FUNC,	(CMD_DEFINE)&command_darklight},
	{"DUNGEONLIGHT", 2,	20,	CMD_FUNC,	(CMD_DEFINE)&command_dungeonlight},
	{"TIME",	2,	21,	CMD_FUNC,	(CMD_DEFINE)&command_time},
	{"MANA",	2,	22,	CMD_TARGET,	(CMD_DEFINE)&target_mana},
	{"STAMINA",	2,	23,	CMD_TARGET,	(CMD_DEFINE)&target_stamina},
	{"GMOPEN",	2,	24,	CMD_FUNC,	(CMD_DEFINE)&command_gmopen},
	{"MAKESHOP",	2,	25,	CMD_TARGET,	(CMD_DEFINE)&target_makeshop},
	{"BUY",		2,	26,	CMD_TARGET,	(CMD_DEFINE)&target_buy},
	{"SETVALUE",	2,	27,	CMD_TARGETX,	(CMD_DEFINE)&target_setvalue},
	{"SETRESTOCK",	2,	28,	CMD_TARGETX,	(CMD_DEFINE)&target_setrestock},
	{"RESTOCK",	2,	29,	CMD_FUNC,	(CMD_DEFINE)&command_restock},
	{"RESTOCKALL",	2,	30,	CMD_FUNC,	(CMD_DEFINE)&command_restockall},
	{"SETSHOPRESTOCKRATE",	2,	31,	CMD_FUNC,	(CMD_DEFINE)&command_setshoprestockrate},
	{"WHO",		1,	0,	CMD_FUNC,	(CMD_DEFINE)&command_who},
	{"GMS",		1,	1,	CMD_FUNC,	(CMD_DEFINE)&command_gms},
	{"SELL",	2,	2,	CMD_TARGET,	(CMD_DEFINE)&target_sell},
	{"MIDI",	2,	3,	CMD_FUNC,	(CMD_DEFINE)&command_midi},
	{"GUMPOPEN",	2,	4,	CMD_FUNC,	(CMD_DEFINE)&command_gumpopen},
	{"RESPAWN",	2,	5,	CMD_FUNC,	(CMD_DEFINE)&command_respawn},
	{"REGSPAWNALL",	2,	5,	CMD_FUNC,	(CMD_DEFINE)&command_regspawnall},
	{"REGSPAWNMAX",	2,	5,	CMD_FUNC,	(CMD_DEFINE)&command_regspawnmax},
	{"REGSPAWN",	2,	5,	CMD_FUNC,	(CMD_DEFINE)&command_regspawn},
	{"SETSPATTACK",	2,	6,	CMD_TARGETHTMP,	(CMD_DEFINE)&target_setspattack},
	{"SETSPADELAY", 2,	7,	CMD_TARGETHTMP,	(CMD_DEFINE)&target_setspadelay},
	{"SETPOISON",	2,	8,	CMD_TARGETHTMP,	(CMD_DEFINE)&target_setpoison},
	{"SETPOISONED",	2,	9,	CMD_TARGETHTMP,	(CMD_DEFINE)&target_setpoisoned},
	{"SETADVOBJ",	2,	10,	CMD_TARGETHTMP,	(CMD_DEFINE)&target_setadvobj},
	{"SETWIPE",	2,	11,	CMD_TARGETID1,	(CMD_DEFINE)&target_setwipe},
	{"FULLSTATS",	2,	12,	CMD_TARGET,	(CMD_DEFINE)&target_fullstats},
	{"HIDE",	2,	13,	CMD_TARGET,	(CMD_DEFINE)&target_hide},
	{"UNHIDE",	2,	14,	CMD_TARGET,	(CMD_DEFINE)&target_unhide},
	{"RELOADSERVER",	2,	15,	CMD_FUNC,	(CMD_DEFINE)&command_reloadserver},
	{"RELOADACCOUNTS",  2,  15, CMD_FUNC,   (CMD_DEFINE)&command_reloadaccounts},
	{"LOADDEFAULTS",	2,	16,	CMD_FUNC,	(CMD_DEFINE)&command_loaddefaults},
	{"CQ",		1,	17,	CMD_FUNC,	(CMD_DEFINE)&command_cq},
	{"WIPENPCS",	2,	18,	CMD_FUNC,	(CMD_DEFINE)&command_wipenpcs},
	{"CNEXT",	1,	19,	CMD_FUNC,	(CMD_DEFINE)&command_cnext},
	{"CCLEAR",	1,	20,	CMD_FUNC,	(CMD_DEFINE)&command_cclear},
	{"MINECHECK",	2,	21,	CMD_FUNC,	(CMD_DEFINE)&command_minecheck},
	{"INVUL",	2,	22,	CMD_FUNC,	(CMD_DEFINE)&command_invul},
	{"NOINVUL",	2,	23,	CMD_FUNC,	(CMD_DEFINE)&command_noinvul},
	{"GUARDSON",	2,	24,	CMD_FUNC,	(CMD_DEFINE)&command_guardson},
	{"GUARDSOFF",	2,	25,	CMD_FUNC,	(CMD_DEFINE)&command_guardsoff},
	{"HOUSE",	2,	26,	CMD_TARGETHTMP,	(CMD_DEFINE)&target_house},
	{"ANNOUNCEON",	2,	27,	CMD_FUNC,	(CMD_DEFINE)&command_announceon},
	{"ANNOUNCEOFF",	2,	28,	CMD_FUNC,	(CMD_DEFINE)&command_announceoff},
	{"WF",		2,	29,	CMD_FUNC,	(CMD_DEFINE)&command_wf},
	{"DECAY",	2,	22,	CMD_FUNC,	(CMD_DEFINE)&command_decay},
	{"SPLIT",	2,	31,	CMD_TARGETHTMP,	(CMD_DEFINE)&target_split},
	{"SPLITCHANCE",	2,	0,	CMD_TARGETHTMP,	(CMD_DEFINE)&target_splitchance},
	{"POSSESS",	2,	1,	CMD_TARGET,	(CMD_DEFINE)&target_possess},
	{"KILLALL",	2,	3,	CMD_FUNC,	(CMD_DEFINE)&command_killall},
	{"PDUMP",	2,	4,	CMD_FUNC,	(CMD_DEFINE)&command_pdump},
	{"RENAME2",	2,	5,	CMD_FUNC,	(CMD_DEFINE)&command_rename2},
	{"READSPAWNREGIONS",	2,	6,	CMD_FUNC,	(CMD_DEFINE)&command_readspawnregions},
	{"CLEANUP",	2,	7,	CMD_FUNC,	(CMD_DEFINE)&command_cleanup},
	{"GY",		2,	8,	CMD_FUNC,	(CMD_DEFINE)&command_gy},
	{"TILEW",	2,	9,	CMD_FUNC,	(CMD_DEFINE)&command_tilew},
	{"SQUELCH",	2,	10,	CMD_FUNC,	(CMD_DEFINE)&command_squelch},
	{"MUTE",	2,	10,	CMD_FUNC,	(CMD_DEFINE)&command_squelch},
	{"TELESTUFF",	2,	11,	CMD_TARGET,	(CMD_DEFINE)&target_telestuff},
	{"SPAWNKILL",	2,	12,	CMD_FUNC,	(CMD_DEFINE)&command_spawnkill},
	{"NEWX",	2,	14,	CMD_TARGETX,	(CMD_DEFINE)&target_newx},
	{"NEWY",	2,	15,	CMD_TARGETX,	(CMD_DEFINE)&target_newy},
	{"INCX",	2,	16,	CMD_TARGETX,	(CMD_DEFINE)&target_incx},
	{"INCY",	2,	17,	CMD_TARGETX,	(CMD_DEFINE)&target_incy},
	{"INCZ",    2,  17, CMD_TARGETX,    (CMD_DEFINE)&target_incz},
	{"WANIM",	2,	18,	CMD_FUNC,	(CMD_DEFINE)&command_wanim},
	{"SETACCOUNT",2,19,	CMD_FUNC,	(CMD_DEFINE)&command_setacct},
	{"ADDACCOUNT",2,20,	CMD_FUNC,	(CMD_DEFINE)&command_addacct},
	{"BANACCOUNT",2,21, CMD_FUNC,	(CMD_DEFINE)&command_banacct},
	{"BAN",		2,	22, CMD_TARGET,	(CMD_DEFINE)&target_ban},
	{"UNBANACCOUNT",2,23,CMD_FUNC,	(CMD_DEFINE)&command_unbanacct},
	{"REMOVEACCOUNT",2,24,CMD_FUNC,	(CMD_DEFINE)&command_removeacct},
	{"SETRACE", 2,  29, CMD_FUNC,   (CMD_DEFINE)&command_setrace},
	{"SETMURDER", 2, 30, CMD_FUNC,  (CMD_DEFINE)&command_setmurder},	// this matched UNGLOW, so changed (can we have the same?)
	{"GLOW",      2, 25, CMD_TARGET, (CMD_DEFINE)&target_glow },
	{"UNGLOW",    2, 26, CMD_TARGET, (CMD_DEFINE)&target_unglow},
	{"SHOWSKILLS",2, 28, CMD_TARGETX, (CMD_DEFINE)&target_showskills},
	{"SHOWDETAIL", 2, 31, CMD_TARGET, (CMD_DEFINE)&target_showdetail},
	{"GETLIGHT", 2, 32, CMD_FUNC, (CMD_DEFINE)&command_getlight},
	{"REPORTBUG", 0, 33, CMD_FUNC, (CMD_DEFINE)&command_reportbug},
	{"REMOVESHOP", 2, 29, CMD_TARGET, (CMD_DEFINE)&target_removeshop},
	{"SETCOMMANDLEVEL",	2,	27,	CMD_TARGETX,	(CMD_DEFINE)&target_commandlevel},
	/* always end with a section of NULLs to mark end of list */
	{NULL,		0,	0,	0,		NULL}			// Tseramed, cleaner
};

static char *ch="abcdefg";

/* Actual commands go below. Define all the dynamic command_ functions as
* the name of the function in lowercase, a void function, accepting one
* integer, which is the player # that sent the command.
* Like this:
* void command_rain(int s)
*
* If you stick comments (double-slash comments) between the function definition
* and the opening block, they will automatically be used as the
* HTML documentation for that command. The first line will be used as the
* short documentation, any subsequent lines will be used as a seperate
* "more info" page for your command. HTML is allowed, but remember that
* everything is fit into a template.
*/

void command_setrace( UOXSOCKET s )
{
	RACEID race;
	if( tnum == 2 )
	{
		race = makenumber(1);
		if( race >= 0 && race <= 65535 )
			Races->gate( currchar[s], race, 1 );
		else
			sysmessage( s, "Invalid race!" );
	}
}
void command_getlight( UOXSOCKET s )
{
	sysmessage( s, "Light value is currently %i", worldcurlevel );
}
void command_setmurder( UOXSOCKET s )
{
	if( tnum == 2 )
	{
		addmitem[s]=makenumber(1);
		target(s, 0, 1, 0, 240, "Select the person to set the murder count of: ");
	}
}

void command_setacct( UOXSOCKET s )
// PARAM WARNING: s is unreferenced
{
	
}

void command_addacct( UOXSOCKET s )
// PARAM WARNING: s is unreferenced
{
	
}

void command_banacct( UOXSOCKET s )
// PARAM WARNING: s is unreferenced
{
	
}

void command_unbanacct( UOXSOCKET s )
// PARAM WARNING: s is unreferenced
{
	
}

void command_removeacct( UOXSOCKET s )
// PARAM WARNING: s is unreferenced
{
	
}

void command_resend( UOXSOCKET s )
// Resends server data to client.
{
	all_items(s); 
	ConOut("ALERT: all_items() called in command_resend().  This function could cause a lot of lag!\n" );
	teleport(currchar[s]);
	return;
}
// Returns the current bulletin board posting mode for the player
void command_post( UOXSOCKET s )
{
  sprintf( temp, "You are currently posting " );

  switch( chars[currchar[s]].postType )
  {
  case LOCALPOST:
    strcat( temp, "a message to a single board [LOCAL]." );
    break;

  case REGIONALPOST:
    strcat( temp, "a message to all boards in this area [REGIONAL]." );
    break;

  case GLOBALPOST:
    strcat( temp, "a message to all boards in the world [GLOBAL]." );
    break;

  default:
    strcat( temp, "an unknown message type. Setting to normal [LOCAL]." );
    chars[currchar[s]].postType = LOCALPOST;
  }

  sysmessage( s, temp );
	return;
}

// Sets the current bulletin board posting mode for the player to GLOBAL
// ALL bulletin boards will see the next message posted to any bulletin board
void command_gpost( UOXSOCKET s )
{
  chars[currchar[s]].postType = GLOBALPOST;
  sysmessage( s, "Now posting GLOBAL messages." );
	return;
}

// Sets the current bulletin board posting mode for the player to REGIONAL
// All bulletin boards in the same region as the board posted to will show the
// next message posted
void command_rpost( UOXSOCKET s )
{
  chars[currchar[s]].postType = REGIONALPOST;
  sysmessage( s, "Now posting REGIONAL messages." );
	return;
}

// Sets the current bulletin board posting mode for the player to LOCAL
// Only this bulletin board will have this post
void command_lpost( UOXSOCKET s )
{
  chars[currchar[s]].postType = REGIONALPOST;
  sysmessage( s, "Now posting LOCAL messages." );
	return;
}

void command_teleport( UOXSOCKET s )
{
	teleport(currchar[s]); 
	return;
}

void command_where( UOXSOCKET s )
// Prints your current coordinates+region.
// added region-name too, LB
{
	// Region is UNSIGNED CHAR, so never < 0 || > 255
	if( strlen( region[chars[currchar[s]].region].name ) > 0 )
		sysmessage( s, "You are at: %s", region[chars[currchar[s]].region].name );
	else 
		sysmessage( s,"You are at: nirvana" );
	
	sysmessage(s, "%i %i (%i/%i)",chars[currchar[s]].x,chars[currchar[s]].y,chars[currchar[s]].z,chars[currchar[s]].dispz); 
	return;
}

void command_q( UOXSOCKET s )
// Shows the GM or Counsellor queue.
{
	if (!(chars[currchar[s]].priv&0x01)) //They are not a GM
	{
		Commands->ShowGMQue(s, 0);
	} else {
		Commands->ShowGMQue(s, 1); // They are a GM
	}
	return;
}


void command_next( UOXSOCKET s )
// For Counselors or GM's, goes to next call in queue.
{
	if (!(chars[currchar[s]].priv&0x01)) //They are not a GM
	{
		Commands->NextCall(s, 0);
	}
	else
	{
		Commands->NextCall(s, 1); // They are a GM
	}
	return;
	
}

void command_clear( UOXSOCKET s )
// For Counselor's and GM's, removes current call from queue.
{
	if (!(chars[currchar[s]].priv&0x01)) //They are not a GM
	{
		donewithcall(s, 0);
	}
	else
	{
		donewithcall(s, 1); // They are a GM
	}
	return;
	
}

void command_goplace( UOXSOCKET s )
// (d) Teleports you to a location from the LOCATIONS.SCP file.
{
	if (tnum==2)
	{
		Commands->MakePlace(s, makenumber(1));
		if (addx[s]!=0)
		{
			mapRegions->RemoveItem(currchar[s]+1000000); //LB
			chars[currchar[s]].x=addx[s];
			chars[currchar[s]].y=addy[s];
			mapRegions->AddItem(currchar[s]+1000000);
			chars[currchar[s]].dispz=chars[currchar[s]].z=addz[s];
			teleport(currchar[s]);
		}
	}
	return;
	
}

void command_gochar( UOXSOCKET s )
// (h h h h) Teleports you to another character.
{
	int i;
	if (tnum==5)
	{
		int serial = calcserial(hexnumber(1),hexnumber(2),hexnumber(3),hexnumber(4));
		i = findbyserial(&charsp[serial%HASHMAX],serial,1);
		if (i != -1)
		{
			mapRegions->RemoveItem(currchar[s]+1000000);
			chars[currchar[s]].x=chars[i].x;
			chars[currchar[s]].y=chars[i].y;
			chars[currchar[s]].z=chars[i].z;
			chars[currchar[s]].dispz=chars[i].dispz;
			mapRegions->AddItem(currchar[s]+1000000);
			teleport(currchar[s]);
		}
	} else if (tnum==2)
	{
		for (i=0;i<now;i++)
		{
			if (i==makenumber(1))
			{
				mapRegions->RemoveItem(currchar[s]+1000000); //lb
				chars[currchar[s]].x=chars[currchar[i]].x;
				chars[currchar[s]].y=chars[currchar[i]].y;
				chars[currchar[s]].dispz=chars[currchar[s]].z=chars[currchar[i]].z;
				mapRegions->AddItem(currchar[s]+1000000);
				teleport(currchar[s]);
				break;
			}
		}
	}
	return;
}

void command_fix( UOXSOCKET s )
// Try to compensate for messed up Z coordinates. Use this if you find yourself half-embedded in the ground.
{
	if (tnum==2)
	{
		if (validtelepos(s)==-1)
		{
			chars[currchar[s]].dispz=chars[currchar[s]].z=makenumber(1);
		}
		else
		{
			chars[currchar[s]].dispz=chars[currchar[s]].z=validtelepos(s);
		}
		teleport(currchar[s]);
	}
	return;
}

void command_xgoplace( UOXSOCKET s )
// (d) Send another character to a location in your LOCATIONS.SCP file.
{
	if (tnum==2)
	{
		Commands->MakePlace(s, makenumber(1));
		if (addx[s]!=0)
		{
			target(s, 0, 1, 0, 8, "Select char to teleport.");
		}
	}
	return;
	
}

void command_showids( UOXSOCKET s )
// Display the serial number of every item on your screen.
{
	int i;
	//for (i=0;i<charcount;i++)
	int x=chars[currchar[s]].x, y=chars[currchar[s]].y; // bugfix lb
	
	// ConOut("x:%i y:%i\n",x,y);
	
	//Char mapRegions
	int getcell=mapRegions->GetCell(x,y);
	int mapitem=-1;
	int mapitemptr=-1;
	int mapchar=-1;
	do //check all items in this cell
	{
		mapitemptr=mapRegions->GetNextItem(getcell, mapitemptr);
		if (mapitemptr==-1) break;
		mapitem=mapRegions->GetItem(getcell, mapitemptr);
		if (mapitem>99999) mapchar=mapitem-1000000;
		if (mapitem!=-1 && mapitem>=1000000)
		{
			i=mapchar;
			if (inrange1p(currchar[s], i)) showcname(s, i, 1);
		}
	} while (mapitem!=-1);
	return;
	
}

void command_poly( UOXSOCKET s )
// (h h) Polymorph yourself into any other creature.
{
	if (tnum==3)
	{
		int k,c1,b;
		
		k=(hexnumber(1)<<8)+hexnumber(2);
		if (k>=0x000 && k<=0x3e1) // lord binary, body-values >0x3e crash the client
			
		{ 
			//		           chars[currchar[s]].xid1=chars[currchar[s]].id1=k>>8; // allow only non crashing ones
			//		           chars[currchar[s]].xid2=chars[currchar[s]].id2=k%256;	 
			chars[currchar[s]].orgid1=chars[currchar[s]].xid1=chars[currchar[s]].id1=k>>8; // allow only non crashing ones
			chars[currchar[s]].orgid2=chars[currchar[s]].xid2=chars[currchar[s]].id2=k%256;
			
			c1=(chars[currchar[s]].skin1<<8)+chars[currchar[s]].skin2; // transparency for mosnters allowed, not for palyers, 
			// if polymorphing from monster to player we have to switch from transparent to semi-transparent
			// or we have that sit-down-client crash
			
			b=c1&0x4000; 
			if (b==16384 && (k >=0x0190 && k<=0x03e1))
			{
				if (c1!=0x8000)
				{
					chars[currchar[s]].skin1=chars[currchar[s]].xskin1=0xF0;
					chars[currchar[s]].skin2=chars[currchar[s]].xskin2=0;
				}
			}
			
		}
	}
	
	teleport(currchar[s]);
	return;
	
}

void command_skin( UOXSOCKET s )
// (h h) Change the hue of your skin.
{
	if (tnum==3)
	{
		
		int k,b,body;
		
		body=(chars[currchar[s]].id1<<8)+chars[currchar[s]].id2;
		k=(hexnumber(1)<<8)+hexnumber(2);
		b=k&0x4000; 
		if (b==16384 && (body >=0x0190 && body<=0x03e1)) k=0xf000; 
		
		if (k!=0x8000)
		{	
			chars[currchar[s]].skin1=k>>8;
			chars[currchar[s]].skin2=k%256;
			chars[currchar[s]].xskin1=k>>8;
			chars[currchar[s]].xskin2=k%256;
			teleport(currchar[s]);
		}
	}
	return;
	
}

void command_action( UOXSOCKET s )
// (h) Preform an animated action sequence.
{
	if (tnum==2) action(s, hexnumber(1));
	return;
}

void command_xtele( UOXSOCKET s )
// (d / h h h h / nothing) Teleport a player to your position.
// <UL><LI>If you specify nothing (/XTELE), you click on the player to teleport in.</LI>
// <LI>If you specify a serial number (/XTELE .. .. .. ..), you teleport that player to you.</LI>
// <LI>If you specify a single number (/XTELE ..), you teleport the player logged in
// under that slot to you.</LI>
// </UL>
{
	if (tnum==5 || tnum==2) Targ->XTeleport(s, tnum);
	else
	{
		target(s, 0, 1, 0, 136, "Select char to teleport to your position.");
	}
	return;
}

void command_go( UOXSOCKET s )
// (d d d) Go to the specified X/Y/Z coordinates
{
	if (tnum==4)
	{
		int x=makenumber(1);int y=makenumber(2);int z=makenumber(3); // LB
		if (x<6144 && y<4096 && z>-127 && z<128)
		{
			mapRegions->RemoveItem(currchar[s]+1000000); //lb
			chars[currchar[s]].x=x;
			chars[currchar[s]].y=y;
			chars[currchar[s]].dispz=chars[currchar[s]].z=z;
			mapRegions->AddItem(currchar[s]+1000000);
			teleport(currchar[s]);
		}
	}
	return;
}

void command_zerokills( UOXSOCKET s )
// Sets all PK counters to 0.
{
	sysmessage(s,"Zeroing all player kills...");
	for(int a=0;a<charcount;a++)
	{
		chars[a].kills=0;
		setcharflag( a );
	}
	sysmessage(s,"All player kills are now 0.");
}

void command_tile( UOXSOCKET s )
// (h h) Tiles the item specified over a square area.
// To find the hexidecimal ID code for an item to tile,
// either create the item with /add or find it in the
// world, and get /ISTATS on the object to get it's ID
// code.
{
	if (tnum==3) {
		addid1[s]=hexnumber(1);
		addid2[s]=hexnumber(2);
		clickx[s]=-1;
		clicky[s]=-1;
		target(s,0,1,0,198,"Select first corner of bounding box.");  // 198 didn't seem taken...
	}
	return;
}

void command_wipe( UOXSOCKET s )
// (d d d d / nothing) Deletes ALL NPC's and items inside a specified square.
// <UL><LI>With no arguments, /WIPE will ask you to click in opposing corners of
// the square.</LI>
// <LI>You may also specify coordinates - X1, Y1, X2, Y2.</LI>
// </UL>
{
	addid1[s]=0; // addid1[s]==0 is used to denote a true wipe
	if (tnum==1) {
		clickx[s]=-1;
		clicky[s]=-1;
		target(s,0,1,0,199,"Select first corner of wiping box.");  // 199 didn't seem taken...
	} else if (tnum==2) {
		if (!strcmp("ALL", (char *)&tbuffer[Commands->cmd_offset+5])) {
			// Really should warn that this will wipe ALL objects...
			Commands->Wipe(s);
	}}
	else if (tnum==5) { // Wipe according to world coordinates
		clickx[s]=makenumber(1);
		clicky[s]=makenumber(2);
		buffer[s][11]=makenumber(3)>>8;buffer[s][12]=makenumber(3)%256; // Do NOT try this at home, kids!
		buffer[s][13]=makenumber(4)>>8;buffer[s][14]=makenumber(4)%256;
		Targ->Wiping(s);
	}
	
	return;
	
}

void command_iwipe( UOXSOCKET s )
// (d d d d / nothing) Deletes ALL NPC's and items NOT inside a specified square.
// <UL><LI>With no arguments, /IWIPE will ask you to click in opposing corners of
// the square.</LI>
// <LI>You may also specify coordinates - X1, Y1, X2, Y2.</LI>
// </UL>
{
	addid1[s]=1;  // addid1[s]==1 is used to denote the INVERSE wipe
	if (tnum==1) {
		clickx[s]=-1;
		clicky[s]=-1;
		target(s,0,1,0,199,"Select first corner of inverse wiping box.");  // 199 didn't seem taken...
	} else if (tnum==2) {
		if (!strcmp("ALL",(char *)&tbuffer[Commands->cmd_offset+6])) {
			sysmessage(s,"Well aren't you the funny one!");
	}}
	else if (tnum==5) { // Wipe according to world coordinates
		clickx[s]=makenumber(1);
		clicky[s]=makenumber(2);
		buffer[s][11]=makenumber(3)>>8;buffer[s][12]=makenumber(3)%256; // Do NOT try this at home, kids!
		buffer[s][13]=makenumber(4)>>8;buffer[s][14]=makenumber(4)%256;
		Targ->Wiping(s);
	}
	
	return;
	
}

void command_eagle( UOXSOCKET s )
// eagle's new test for the add menu
{
	if (tnum==2 || tnum>3) 
	{
		sysmessage( s, "Syntax Error. Usage: /add <id1> <id2>" );
		return;
	} else if( tnum == 3 )
	{
		addid1[s]=hexnumber(1);
		addid2[s]=hexnumber(2);
		if (addid1[s]<0x40)
		{
			target(s, 0, 1, 0, 0, "Select location for item.");
		}
	} else if (tnum==1)
	{
		itemmenu(s, 1);
	}
	return;
}

void command_add( UOXSOCKET s )
// (h h) Adds a new item, or opens the GM menu if no hex codes are specified.
{
	if (tnum==2 || tnum>3) // AntiChrist
	{
		sysmessage( s, "Syntax Error. Usage: /add <id1> <id2>" );
		return;
	} else if( tnum == 3 )
	{
		addid1[s]=hexnumber(1);
		addid2[s]=hexnumber(2);
		if (addid1[s]<0x40)
		{
			target(s, 0, 1, 0, 0, "Select location for item.");
		}/* else { //This was causing server crashes (after added, if someone walked it would get
			//in an endless loop through the mutli section of walking() )
			 addid3[s]=0;
			buildhouse(s,0);
			}*/
	} else if (tnum==1)
	{
		itemmenu(s, 1);
	}
	return;
}


void command_addx( UOXSOCKET s )
// (h h) Adds a new item to your current location.
{
	if (tnum==3)
	{
		addid1[s]=hexnumber(1);
		addid2[s]=hexnumber(2);
		Commands->AddHere(s, chars[currchar[s]].z);
	}
	if (tnum==4)
	{
		addid1[s]=hexnumber(1);
		addid2[s]=hexnumber(2);
		Commands->AddHere(s, makenumber(3));
	}
	return;
}

void command_rename( UOXSOCKET s )
// (text) Renames any dynamic item in the game.
{
	if (tnum>1)
	{
		strcpy(xtext[s], (char *)&tbuffer[Commands->cmd_offset+7]);
		target(s, 0, 1, 0, 1, "Select item or character to rename.");
	}
	return;
	
}

void command_title( UOXSOCKET s )
// (text) Changes the title of any player or NPC.
{
	if (tnum>1)
	{
		strcpy(xtext[s], (char *)&tbuffer[Commands->cmd_offset+6]);
		target(s, 0, 1, 0, 47, "Select character to change the title of.");
	}
	return;
	
}


void command_save( UOXSOCKET s )
// Saves the current world data into ITEMS.WSC and CHARS.WSC.
{
	//savenewworld(1);
	if ( !cwmWorldState->Saving() )
	{
		cwmWorldState->savenewworld(1);
		saveserverscript(1);
	}
	return;
}

void command_dye( UOXSOCKET s )
// (h h/nothing) Dyes an item a specific color, or brings up a dyeing menu if no color is specified.
{
	dyeall[s]=1;
	if (tnum==3)
	{
		addid1[s]=hexnumber(1);
		addid2[s]=hexnumber(2);
	}
	else
	{
		addid1[s]=255;
		addid2[s]=255;
	}
	target(s, 0, 1, 0, 4, "Select item to dye.");
	return;
	
}

void command_wtrig( UOXSOCKET s )
// (word) Sets the trigger word on an NPC.
{
	if (tnum>1)
	{
		strcpy( xtext[s], (char *)&tbuffer[20] );
		target(s, 0, 1, 0, 203, "Select the NPC to set trigger word on.");
	}
	return;
	
}

void command_setpriv( UOXSOCKET s )
// (Chars/NPCs: h h, Objects: h) Sets priviliges on a Character or object.
{
	if (tnum==3)
	{
		addid1[s]=hexnumber(1);
		addid2[s]=hexnumber(2);
		target(s, 0, 1, 0, 9, "Select char to edit priv.");
	}
	if (tnum==2)
	{
		addid1[s]=3;
		addid2[s]=hexnumber(1);
		target(s, 0, 1, 0, 89, "Select object to edit priv.");
	}
	return;
}

void command_nodecay( UOXSOCKET s )
// Prevents an object from ever decaying.
{
	addid1[s]=0; 
	target(s, 0, 1, 0, 89, "Select object to make permenant.");
	return;
}


#ifdef UNRELEASED
void command_send( UOXSOCKET s )
// Debugging command
{
	int i;
	for (i=1;i<tnum;i++) temp[i-1]=hexnumber(i);
	//   sysmessage(s, "Sending...");
	ConOut("Sending to client %i.\n",s);
	Network->xSend(s, temp, tnum-1, 0);
	return;
	
}
#endif

void command_showtime( UOXSOCKET s )
// Displays the current UO time.
{
	if (ampm || (!ampm && hour==12))
		sysmessage(s, "%s %2.2d %s %2.2d %s", "UOX3: Time: ", hour, ":", minute, "PM");
	else
		sysmessage(s, "%s %2.2d %s %2.2d %s", "UOX3: Time: ", hour, ":",minute, "AM");
	return;
	
}

void command_settime( UOXSOCKET s )
// PARAM WARNING: s is unreferenced
// (d d) Sets the current UO time in hours and minutes.
{
	int newhours, newminutes;
	if (tnum==3)
	{
		newhours = makenumber(1);
		newminutes = makenumber(2);
		if ((newhours < 25) && (newhours > 0) && (newminutes > -1) && (newminutes <60))
		{
			if (newhours > 12)
			{
				ampm=1;
				hour=newhours-12;
			}
			else
			{
				ampm=0;
				hour=newhours;
			}
			minute=newminutes;
		}
	}
	return;
	
}

void command_shutdown( UOXSOCKET s )
// PARAM WARNING: s is unreferenced
// (d) Shuts down the server. Argument is how many minutes until shutdown.
{
	if( tnum == 2 )
	{
		endtime = ( uiCurrentTime + ( CLOCKS_PER_SEC * makenumber( 1 ) ) );
		if( makenumber( 1 ) == 0 )
		{
			endtime = 0;
			sysbroadcast( "Shutdown has been interrupted." );
		}
		else 
			endmessage( 0 );
	}
	return;
}

void command_wholist( UOXSOCKET s )
// Brings up an interactive listing of online users.
{
	if( strcmp( (char *)&tbuffer[Commands->cmd_offset+8], "off" ) )
		WhoList->SendSocket( s );
	else
		OffList->SendSocket( s );
	return;
}

#ifdef UNRELEASED
void command_blt2( UOXSOCKET s )
// Debugging command.
{
	if (tnum==4)
	{
		boltstring[1]=hexnumber(1);
		boltstring[2]=0;
		boltstring[3]=0;
		boltstring[4]=0;
		boltstring[5]=1;
		boltstring[10]=hexnumber(2);
		boltstring[11]=hexnumber(3);
		Network->xSend(s, boltstring, 28, 0);
	}
	return;
	
}
#endif

void command_sfx( UOXSOCKET s )
// (h h) Plays the specified sound effect.
{
	if (tnum==3)
	{
		soundeffect(s, hexnumber(1), hexnumber(2));
	}
	return;
}

void command_light( UOXSOCKET s )
// PARAM WARNING: s is unreferenced
// (h) Sets the light level. 0=brightest, 15=darkest, -1=enable day/night cycles.
{
	if (tnum==2)
	{
		worldfixedlevel=hexnumber(1);
		if (worldfixedlevel!=255) setabovelight(worldfixedlevel);
		else setabovelight(worldcurlevel);
	}
	return;
}

#ifdef UNRELEASED
void command_web( UOXSOCKET s )
// Debugging command.
{
	if (tnum>1)
	{
		strcpy(xtext[s], &tbuffer[Commands->cmd_offset+4]);
		weblaunch(s, xtext[s]);
	}
	return;
}
#endif

void command_disconnect( UOXSOCKET s )
// PARAM WARNING: s is unreferenced
// (d) Disconnects the user logged in under the specified slot.
{
	if (tnum==2) Network->Disconnect(makenumber(1));
	return;
}

void command_tell( UOXSOCKET s )
// (d text) Sends an anonymous message to the user logged in under the specified slot.
{
	if (tnum>2) tellmessage(s, makenumber(1), &tbuffer[Commands->cmd_offset+6]);
	return;
}

/* Lord binary's debugging code
void command_npcs( UOXSOCKET s )
{
int i=0,j=0;
for (j=0;j<charcount;j++)
{
if (chars[j].account==-1) i++;
}
ConOut("Total# of npcs: %i\n",i);
}

  void command_npcsd( UOXSOCKET s )
  {
		int i=0,j=0;
		for (j=0;j<charcount;j++)
		{
		if (chars[j].account==-1 && chars[j].free==1) i++;
		}
		ConOut("Total# of npcs: %i\n",i);
		}
		
		  void command_pcs( UOXSOCKET s )
		  {
		  int i=0,j=0;
		  for (j=0;j<charcount;j++)
		  {
		  if (chars[j].account!=-1) i++;
		  }
		  ConOut("Total# of players: %i\n",i);
		  }
		  
			void command_charc( UOXSOCKET s ) { }
			void command_itemc( UOXSOCKET s ) { }
* end of lord binary's debugging code */

void command_gmmenu( UOXSOCKET s )
// (d) Opens the specified GM Menu.
{
	if( tnum == 2 ) 
		gmmenu( s, makenumber( 1 ) );
	return;
}

void command_itemmenu( UOXSOCKET s )
// (d) Opens the specified Item Menu from ITEMS.SCP.
{
	int i;

	if (tnum==2)
	{
		i=makenumber(1);
		
		//if ( i>0 && comm[1][1]!=0 )
		//{
			itemmenu(s, i);
		//else
		//	itemmenu(s, comm[1]);
	}
	return;
}

void command_additem( UOXSOCKET s )
// (d) Adds the specified item from ITEMS.SCP.
{
	if (tnum==2)
	{
		addmitem[s] = makenumber(1); // Anthracks' fix
		sprintf(temp, "Select location for item. [Number: %i]", addmitem[s]);
		target(s, 0, 1, 0, 26, temp);
	}
	return;
	
}

void command_dupe( UOXSOCKET s )
// (d / nothing) Duplicates an item. If a parameter is specified, it's how many copies to make.
{
	if (tnum==2)
	{
		addid1[s]=makenumber(1);
		target(s, 0, 1, 0, 110, "Select an item to dupe.");
	}
	else
	{
		addid1[s]=1;
		target(s, 0, 1, 0, 110, "Select an item to dupe.");
	}
	return;
}


void command_command( UOXSOCKET s )
// Executes a trigger scripting command.
{
	int i;
	char c=0;
	if (tnum>1)
	{
		i=0;
		script1[0]=0;
		script2[0]=0;
		while(tbuffer[Commands->cmd_offset+8+i]!=' ' && tbuffer[Commands->cmd_offset+8+i]!=0) i++;
		strncpy(script1,(char *)&tbuffer[Commands->cmd_offset+8],i);
		script1[i]=0;
		if ((script1[0]!='}')&&(c!=0)) strcpy(script2, (char *)&tbuffer[Commands->cmd_offset+8+i+1]);
		scriptcommand(s, script1, script2);
	}
	return;
}

void command_gcollect( UOXSOCKET s )
// PARAM WARNING: s is unreferenced
// Runs garbage collection routines.
{
	gcollect();
	return;
}

void command_allmoveon( UOXSOCKET s )
// Enables GM ability to pick up all objects.
{
	chars[currchar[s]].priv2=chars[currchar[s]].priv2|0x01;
	teleport(currchar[s]);
	sysmessage(s, "ALLMOVE enabled."); // Crackerjack 07/25/99
	return;
	
}

void command_allmoveoff( UOXSOCKET s )
// Disables GM ability to pick up all objects.
{
	chars[currchar[s]].priv2=chars[currchar[s]].priv2&(0xFF-0x01);
	teleport(currchar[s]);
	sysmessage(s, "ALLMOVE disabled."); // Crackerjack 07/25/99
	return;
	
}

void command_showhs( UOXSOCKET s )
// Makes houses appear as deeds. (The walls disappear and there's a deed on the ground in their place.)
{
	chars[currchar[s]].priv2=chars[currchar[s]].priv2|0x04;
	teleport(currchar[s]);
	sysmessage(s, "House icons visible. (Houses invisible)");
	return;
	
}

void command_hidehs( UOXSOCKET s )
// Makes houses appear as houses (opposite of /SHOWHS).
{
	chars[currchar[s]].priv2=chars[currchar[s]].priv2&(0xFF-0x04);
	teleport(currchar[s]);
	sysmessage(s, "House icons hidden. (Houses visible)");
	return;
}


void command_set( UOXSOCKET s )
// (text, d) Set STR/DEX/INT/Skills on yourself arguments are skill & amount.
{
	int i;
	if (tnum==3)
	{
		i=0;
		script1[0]=0;
		while (tbuffer[Commands->cmd_offset+4+i]!=' ' && tbuffer[Commands->cmd_offset+4+i]!=0) i++;
		strncpy(script1, (char *)&tbuffer[Commands->cmd_offset+4],i);
		script1[i]=0;
		strupr(script1);
		addx[s]=-1;
		
		//				ConOut("callet %s\n",script1);
		
		for (i=0;i<SKILLS;i++)
		{
			if (!(strcmp(skillname[i], script1))) { /*ConOut("%s\n",skillname[i]);*/addx[s]=i;}
		}
		if (addx[s]!=-1)
		{
			addy[s]=makenumber(2);
			target(s, 0, 1, 0, 36, "Select character to modify.");
		}
	}
	return;
}

void command_addnpc( UOXSOCKET s )
// (d or h h) Add the specified NPC from NPC.SCP or by hex ID code.
{
	if (tnum==3)
	{
		addid1[s]=hexnumber(1);
		addid2[s]=hexnumber(2);
		target(s, 0, 1, 0, 33, "Select location for the NPC.");
	}
	else if (tnum==2)
	{
		addmitem[s]=makenumber(1);
		target(s, 0, 1, 0, 27, "Select location for the NPC.");
	}
	return;
	
}

void command_readini( UOXSOCKET s )
// Re-loads the UOX3.INI file.
{
	int i;
	Admin->ReadIni();
	for (i=0;i<servcount;i++)
		if (serv[i][1][0]=='*') sprintf(serv[i][1],"%i.%i.%i.%i",ph1,ph2,ph3,ph4);
		sysmessage(s, "INI file reloaded.");
		return;
		
}

void command_gumpmenu( UOXSOCKET s )
// (d) Opens the specified GUMP menu.
{
	if (tnum==2)
	{
		Gumps->Menu(s, makenumber(1));
	}
	return;
	
}

void command_cachestats( UOXSOCKET s )
// Display some information about the cache.
{
	sysmessage(s, "Hits: %d", stablockcachehit);
	sysmessage(s, "Misses: %d", stablockcachemiss);
	sysmessage(s, "Total: %d", (stablockcachehit+stablockcachemiss));
	return;
}

void command_npcrect( UOXSOCKET s )
// (d d d d) Set bounding box for a NPC with a NPCWANDER of 3.
{
	if (tnum==5)
	{
		addx[s]=makenumber(1); // bugfix, LB, old npcshape worked only if its only excuted by ONE player at the same time
		addy[s]=makenumber(2);
		addx2[s]=makenumber(3);
		addy2[s]=makenumber(4);
		target(s, 0, 1, 0, 67, "Select the NPC to set the bounding rectangle for."); // lb bugfix, was 58 ...
	}
	return;
	
}

void command_npccircle( UOXSOCKET s )
// (d d d) Set bounding circle for a NPC with a NPCWANDER of 2.
{
	if (tnum==4)
	{
		addx[s]=makenumber(1);
		addy[s]=makenumber(2);
		addx2[s]=makenumber(3);
		target(s, 0, 1, 0, 59, "Select the NPC to set the bounding circle for.");
	}
	return;
	
}

void command_npcwander( UOXSOCKET s )
// (d) Sets the type of wandering a NPC does.
// <UL><LI>0 = NPC Does not move.</LI>
// <LI>1 = NPC Follows specified target. (See
// <A HREF="npctarget.html">/NPCTARGET</A>)</LI>
// <LI>2 = NPC Wanders Freely.</LI>
// <LI>3 = NPC stays in box specified by <A HREF="npcrect.html">NPCRECT</A>.</LI>
// <LI>4 = NPC stays in circle specified by <A HREF="npccircle.html">NPCCIRCLE</A>.</LI></UL>
{
	if (tnum==2)
	{
		npcshape[0]=makenumber(1);
		target(s, 0, 1, 0, 60, "Select the NPC to set the wander method for.");
	}
	return;
	
}

void command_secondsperuominute( UOXSOCKET s )
// (d) Sets the number of real-world seconds that pass for each UO minute.
{
	if (tnum==2)
	{
		secondsperuominute=makenumber(1);
		sysmessage(s, "Seconds per UO minute set.");
	}
	return;
	
}

void command_brightlight( UOXSOCKET s )
// (h) Sets default daylight level.
{
	if (tnum==2)
	{
		worldbrightlevel=hexnumber(1);
		sysmessage(s, "World bright light level set.");
	}
	return;
	
}

void command_darklight( UOXSOCKET s )
// (h) Sets default nighttime light level.
{
	if (tnum==2)
	{
		worlddarklevel=hexnumber(1);
		sysmessage(s, "World dark light level set.");
	}
	return;
	
}

void command_dungeonlight( UOXSOCKET s )
// (h) Sets default dungeon light level.
{
	if (tnum==2)
	{
		dungeonlightlevel=min(hexnumber(1), 27);
		sysmessage(s, "Dungeon light level set.");
	}
	return;
	
}

void command_gmopen( UOXSOCKET s )
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
	if (tnum==2) addmitem[s]=hexnumber(1);
	else addmitem[s]=0x15;
	target(s, 0, 1, 0, 115, "Select the character to open the container on.");
	return;
	
}

void command_restock( UOXSOCKET s )
// Forces a manual vendor restock.
{
	restock(0);
	sysmessage(s, "Manual shop restock has occurred.");
	return;
	
}

void command_restockall( UOXSOCKET s )
// Forces a manual vendor restock to maximum values.
{
	restock(1);
	sysmessage(s, "Restocking all shops to their maximums");
	return;
	
}

void command_setshoprestockrate( UOXSOCKET s )
// (d) Sets the universe's shop restock rate.
{
	if (tnum==2)
	{
		shoprestockrate=makenumber(1);
		sysmessage(s, "NPC shop restock rate changed.");
	}
	else sysmessage(s, "Invalid number of parameters.");
	return;
	
}

void command_midi( UOXSOCKET s )
// (d d) Plays the specified MIDI file.
{
	if (tnum==3) playmidi(s, makenumber(1), makenumber(2));
	return;
	
}

void command_gumpopen( UOXSOCKET s )
// (h h) Opens the specified GUMP menu.
{
	if (tnum==3) Gumps->Open(s, currchar[s], hexnumber(1), hexnumber(2));
	return;
	
}

void command_respawn( UOXSOCKET s )
// PARAM WARNING: s is unreferenced
// Forces a respawn.
{
	respawnnow();
	return;
	
}

void command_regspawnmax( UOXSOCKET s )
// (d) Spawns in all regions up to the specified maximum number of NPCs/Items.
{
	if (tnum==2)
	{
		ConOut("regspawnall called\n");
		Commands->RegSpawnMax(s, makenumber(1));
		return;
	}
	
}

void command_regspawn( UOXSOCKET s )
// (d d) Preforms a region spawn. First argument is region, second argument is max # of items/NPCs to spawn in that region.
{
	if (tnum==3)
	{
		ConOut("regspawnnum called\n");
		Commands->RegSpawnNum(s, makenumber(1), makenumber(2));
		return;
	}
	
}

void command_reloadserver( UOXSOCKET s )
// Reloads the SERVER.SCP file.
{
	loadserverscript();
	sysmessage(s,"Server.scp reloaded.");
	return;
}

void command_loaddefaults( UOXSOCKET s )
// PARAM WARNING: s is unreferenced
// Loads the server defaults.
{
	loadserverdefaults();
	return;
	
}

void command_cq( UOXSOCKET s )
// Display the counselor queue.
{
	Commands->ShowGMQue(s, 0); // Show the Counselor queue, not GM queue   
	return;
}

void command_cnext( UOXSOCKET s )
// Attend to the next call in the counselor queue.
{
	Commands->NextCall(s, 0); // Show the Counselor queue, not GM queue   
	return;
	
}

void command_cclear( UOXSOCKET s )
// Remove the current call from the counselor queue.
{
	donewithcall(s, 0); // Show the Counselor queue, not GM queue   
	return;
	
}

void command_minecheck( UOXSOCKET s )
// PARAM WARNING: s is unreferenced
// (d) Set the server mine check interval in minutes.
{
	if (tnum==2)
		server_data.minecheck=makenumber(1);
	return;
}

void command_invul( UOXSOCKET s )
// Makes the targeted character immortal.
{
	addx[s]=1;
	target(s,0,1,0,179,"Select creature to make invulnerable.");
	
}

void command_noinvul( UOXSOCKET s )
// Makes the targeted character mortal.
{
	addx[s]=0;
	target(s,0,1,0,179,"Select creature to make mortal again.");
	
}

void command_guardson( UOXSOCKET s )
// PARAM WARNING: s is unreferenced
// Activates town guards.
{
	server_data.guardsactive=1;
	sysbroadcast("Guards have been reactivated.");
	
}

void command_guardsoff( UOXSOCKET s )
// PARAM WARNING: s is unreferenced
// Deactivates town guards.
{
	server_data.guardsactive=0;
	sysbroadcast( "Warning: Guards have been deactivated globally.");
	
}

void command_announceon( UOXSOCKET s )
// PARAM WARNING: s is unreferenced
// Enable announcement of world saves.
{
	cwmWorldState->announce((server_data.announceworldsaves=1));
	sysbroadcast( "WorldStat Saves will be displayed.");
	
}

void command_announceoff( UOXSOCKET s )
// PARAM WARNING: s is unreferenced
// Disable announcement of world saves.
{
	cwmWorldState->announce((server_data.announceworldsaves=0));
	sysbroadcast( "WorldStat Saves will not be displayed.");
	
}

void command_wf( UOXSOCKET s )
// Make the specified item worldforge compatible.
{
	if (tnum==2)
	{
		addid1[s]=255;
		target(s, 0, 1, 0, 6, "Select item to make WorldForge compatible.");
	}
	return;
	
}

void command_decay( UOXSOCKET s )
// Enables decay on an object.
{
	addid1[s]=1;
	target(s, 0, 1, 0, 89, "Select object to decay.");
	return;
	
}

void command_killall( UOXSOCKET s )
// (d text) Kills all of a specified item.
{
	if(tnum>2)
	{
		if(makenumber(1)<10)
			Commands->KillAll(s, makenumber(1), &tbuffer[Commands->cmd_offset+9]);
		else if (makenumber(1)<100)
			Commands->KillAll(s, makenumber(1), &tbuffer[Commands->cmd_offset+10]);
		else
			Commands->KillAll(s, makenumber(1), &tbuffer[Commands->cmd_offset+11]);
	}
	return;
	
}

void command_pdump( UOXSOCKET s )
// Display some performance information.
{
	sysmessage(s, "Performace Dump:");
	
	sysmessage(s, "Network code: %fmsec [%i]" _ (float)((float)networkTime/(float)networkTimeCount) _ networkTimeCount);
	sysmessage(s, "Timer code: %fmsec [%i]" _ (float)((float)timerTime/(float)timerTimeCount) _ timerTimeCount);
	sysmessage(s, "Auto code: %fmsec [%i]" _ (float)((float)autoTime/(float)autoTimeCount) _ autoTimeCount);
	sysmessage(s, "Loop Time: %fmsec [%i]" _ (float)((float)loopTime/(float)loopTimeCount) _ loopTimeCount);
	sysmessage(s, "Simulation Cycles/Sec: %f" _ (1000.0*(1.0/(float)((float)loopTime/(float)loopTimeCount))));
}

void command_rename2( UOXSOCKET s )
// (text) Rename an item or character.
{
	 if( tnum > 1 )
	 {
		 addx[s]=1;
		 strcpy(xtext[s], (char *)&tbuffer[Commands->cmd_offset+8]);
		 target(s, 0, 1, 0, 1, "Select item or character to rename.");
	 } 
}

void command_readspawnregions( UOXSOCKET s )
// Re-read the SPAWN.SCP file.
{
	loadspawnregions();
	sysmessage(s,"Spawnregions reloaded.");
}

void command_gy( UOXSOCKET s )
// (text) GM Yell - Announce a message to all online GMs.
{
	if( now == 1 )
	{
		sysmessage( s, "There are no other users connected." );
		return;
	}
	int i;
	int tl;
	sprintf(xtext[s], "(GM ONLY): %s", &tbuffer[Commands->cmd_offset+3]); // AntiChrist bugfix - cms_offset+4, not +7
	tl=44+strlen(&xtext[s][0])+1;			
	
	talk[1]=tl>>8;
	talk[2]=tl%256;
	talk[3]=chars[currchar[s]].ser1;
	talk[4]=chars[currchar[s]].ser2;
	talk[5]=chars[currchar[s]].ser3;
	talk[6]=chars[currchar[s]].ser4;
	talk[7]=chars[currchar[s]].id1;
	talk[8]=chars[currchar[s]].id2;
	talk[9]=1;
	talk[10]=buffer[s][4];
	talk[11]=buffer[s][5];
	talk[12]=buffer[s][6];
	talk[13]=chars[currchar[s]].fonttype;
	
	for (i=0;i<now;i++)
	{
		if (perm[i]&&chars[currchar[i]].priv&0x01)
		{
			Network->xSend(i, talk, 14, 0);
			Network->xSend(i, chars[currchar[s]].name, 30, 0);     
			Network->xSend(i, &xtext[s][0], strlen(&xtext[s][0])+1, 0);   
		}
	}
	
	
}

void command_tilew( UOXSOCKET s )
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
	if(tnum==8)
	{
		addid1[s]=hexnumber(1);//id1
		addid2[s]=hexnumber(2);//id2
		int pile=0;
		tile_st tile;
		Map->SeekTile((addid1[s]<<8)+addid2[s], &tile);
		if (tile.flag2&0x08) pile=1;
		for (int x=makenumber(3);x<=makenumber(4);x++)
		{
			for (int y=makenumber(5);y<=makenumber(6);y++)
			{
				int a=Items->SpawnItem(s, 1, "#", pile, addid1[s], addid2[s], 0, 0, 0,0);
				if( a > -1 )	// Antichrist crash prevention
				{
					// ConOut("ids: %i %i  ",addid1[s],addid2[s]);
					items[a].priv=0; //Make them not decay
					items[a].x=x;
					items[a].y=y;
					items[a].z=makenumber(7);
					mapRegions->AddItem(a); // lord Binary
					//for (int b=0;b< now;b++) if (perm[b] && iteminrange(b,a,18)) senditem(b,a);
					RefreshItem( a ); // AntiChrist
				}
			}
			
			
			
		}
		addid1[s]=0; // lb, i was so free and placed it here so that we dont have y-1 rows of 0-id items ... hope that was not intentinal ..
		addid2[s]=0;
	}  
	else { sysmessage(s, "Format: /tilew ID1 ID2 X1 X2 Y1 Y2 Z"); }
	return;
	
}

void command_squelch( UOXSOCKET s )
// (d / nothing) Squelchs specified player. (Makes them unnable to speak.)
{
	
	if (tnum==2)
	{
		if (makenumber(1)!=0 || makenumber(1)!=-1)
		{
			addid1[s]=255;				// not good for unsigned
			addid1[s]=makenumber(1);
		}
	}
	target(s,0,1,0,223,"Select character to squelch.");
	return;
	
}

void command_spawnkill( UOXSOCKET s )
// (d) Kills spawns from the specified spawn region in SPAWN.SCP.
{
	if (tnum==2)
	{
		Commands->KillSpawn(s, makenumber(1));
	}
}


void command_wanim( UOXSOCKET s )
// (d d) Changes server lighting animation.
{
	if(tnum==3)
	{
		w_anim[0]=(signed char) hexnumber(1);
		w_anim[1]=(signed char) hexnumber(2);
		//ConOut("%i %i\n",w_anim[0],w_anim[1]);
		sysmessage(s,"new lightening animation set!");
	}
}

void command_gotocur( UOXSOCKET s )
// Goes to the current call in the GM/Counsellor Queue
{
	int i;
	int x=0;
	
	if(chars[currchar[s]].callnum==0)
	{
		sysmessage(s,"You are not currently on a call.");
	}
	else
	{  
		{
			int serial=calcserial(gmpages[chars[currchar[s]].callnum].ser1,gmpages[chars[currchar[s]].callnum].ser2,gmpages[chars[currchar[s]].callnum].ser3,gmpages[chars[currchar[s]].callnum].ser4);
			i=findbyserial(&charsp[serial%HASHMAX],serial,1);
			if(i!=-1)
			{
				mapRegions->RemoveItem(currchar[s]+1000000); //lb
				chars[currchar[s]].x=chars[i].x;
				chars[currchar[s]].y=chars[i].y;
				mapRegions->AddItem(currchar[s]+1000000); //lb
				chars[currchar[s]].dispz=chars[currchar[s]].z=chars[i].z;
				sysmessage(s,"Transporting to your current call.");
				teleport(currchar[s]);
				x++;
			}  
			if(x==0)
			{
				int serial=calcserial(gmpages[chars[currchar[s]].callnum].ser1,gmpages[chars[currchar[s]].callnum].ser2,gmpages[chars[currchar[s]].callnum].ser3,gmpages[chars[currchar[s]].callnum].ser4);
				i=findbyserial(&charsp[serial%HASHMAX],serial,1);
				if(i!=-1)
				{
					mapRegions->RemoveItem(currchar[s]+1000000); //lb
					chars[currchar[s]].x=chars[i].x;
					chars[currchar[s]].y=chars[i].y;
					chars[currchar[s]].dispz=chars[currchar[s]].z=chars[i].z;
					mapRegions->AddItem(currchar[s]+1000000); //lb
					sysmessage(s,"Transporting to your current call.");
					teleport(currchar[s]);
				}
			}
		}
	}
	
}

void command_gmtransfer( UOXSOCKET s )
// Escilate a Counsellor Page into the GM Queue
{
	int i;
	int x2=0;
	
	if(chars[currchar[s]].callnum!=0)
	{
		if(!(chars[currchar[s]].priv&1)) //Char is a counselor
		{
			for(i=1;i<MAXPAGES;i++)
			{
				if(gmpages[i].handled==1)
				{
					gmpages[i].handled=0;
					strcpy(gmpages[i].name,counspages[chars[currchar[s]].callnum].name);
					strcpy(gmpages[i].reason,counspages[chars[currchar[s]].callnum].reason);
					gmpages[i].ser1=counspages[chars[currchar[s]].callnum].ser1;
					gmpages[i].ser2=counspages[chars[currchar[s]].callnum].ser2;
					gmpages[i].ser3=counspages[chars[currchar[s]].callnum].ser3;
					gmpages[i].ser4=counspages[chars[currchar[s]].callnum].ser4;
					time_t current_time = time(0);
					struct tm *local = localtime(&current_time);
					sprintf(gmpages[i].timeofcall, "%02d:%02d:%02d", local->tm_hour, local->tm_min, local->tm_sec);
					x2++;
					break;
				}
			}
			if (x2==0)
			{
				sysmessage(s,"The GM Queue is currently full. Contact the shard operator");
				sysmessage(s,"and ask them to increase the size of the queue.");
			}
			else
			{
				sysmessage(s,"Call successfully transferred to the GM queue.");
				donewithcall(s,1);
			}
		}
		else
		{
			sysmessage(s,"Only Counselors may use this command.");
		}
	}
	else
	{
		sysmessage(s,"You are not currently on a call");
	}
}

void command_who( UOXSOCKET s )
// Displays a list of users currently online.
{
	if( now == 1 )
	{
		sysmessage( s, "There are no other users connected." );
		return;
	}
	int i, j=0;
	sysmessage(s,"Current Users in the World:");
	for (i=0;i<now;i++)
	{
		if(perm[i]) //Keeps NPC's from appearing on the list
		{
			j++;
			sysmessage(s, "%i) %s [%x %x %x %x]", (j-1), chars[currchar[i]].name, chars[currchar[i]].ser1, chars[currchar[i]].ser2, chars[currchar[i]].ser3, chars[currchar[i]].ser4);
		}
	}
	sysmessage(s,"Total Users Online: %d", j);
	sysmessage(s,"End of userlist");
}

void command_gms( UOXSOCKET s )
{
	int i, j=0;
	sysmessage(s,"Current GMs and Counselors in the world:");
	for (i=0;i<now;i++)
	{
		if(perm[i] && ((chars[currchar[i]].priv&0x01)||(chars[currchar[i]].priv&0x80))) //Keeps NPC's from appearing on the list
		{
			j++;
			sysmessage(s, chars[currchar[i]].name);
		}
	}
	sysmessage(s, "Total Staff Online: %d\n", j);
	sysmessage(s,"End of stafflist");
}

void command_regspawnall( UOXSOCKET s )
{
	int i, j, k, spawn=0;
	//	unsigned int currenttime=getclock();
	unsigned int currenttime=uiCurrentTime;
	for (int n=1;n<totalspawnregions;n++)
	{
		spawn += (spawnregion[n].max-spawnregion[n].current);
	}
	if (spawn > 250)
	{
		sysmessage(s, "Attempt to spawn more than 250 items/NPCs denied.  Try /REGSPAWN r n<250 instead.");
		return;
	}
	
	sysbroadcast( "ALL Regions Spawning to MAX, this will cause some lag.");
	
	for(i=1;i<totalspawnregions;i++)
	{
		k = (spawnregion[i].max-spawnregion[i].current);
		for(j=1;j<k;j++)
		{
			doregionspawn(i);
		}
		spawnregion[i].nexttime = (int)(currenttime + ( CLOCKS_PER_SEC * 60 * RandomNum( spawnregion[i].mintime, spawnregion[i].maxtime ) ) );
	}
	char temps[60];
	sprintf(temps, "Done. %6d total NPCs/items spawned in %6d regions.",spawn,totalspawnregions);
	sysmessage(s, temps);
}

void command_wipenpcs( UOXSOCKET s )
{
	int deleted=0;
	
	ConOut("UOX3: %s has initiated an NPC wipe\n",chars[currchar[s]].name);
	
	for (int j=0;j<charcount;j++)
	{
		// this is insanely wasteful, not only are we deleting npcs that have
		// already been freed, but we send the remove packet ourselves, and
		// DeleteChar() turns around and sends another remove packet. additionally,
		// both of these were broadcasting all deletes to all players, when they
		// could send to those that are within range of each other
		if(chars[j].npc && chars[j].free == 0 && chars[j].npcaitype != 17 && !chars[j].tamed ) // PV and pets don't wipe
		{			
			Npcs->DeleteChar(j);
			deleted++;
		}
	}
	
	// Reset all spawn regions back or they won't spawn again
	for( int w = 0; w <= totalspawnregions; w++ )
	{
		spawnregion[w].current = 0;
	}
	
	ConOut("deleted: %i npcs\n",deleted);
	gcollect();
	
	// charcount=charcount-deleted;  deadly, lord binary !!!!
	
	sysbroadcast( "All NPC's have been wiped.");
	
}

void command_cleanup( UOXSOCKET s )
{
	int corpses=0;
	//        char temp[100];
	
	sysmessage(s,"Cleaning corpses and closing gates...");
	for(int i=0;i<itemcount;i++)
	{
		if((items[i].corpse==1)||(items[i].type==51)||(items[i].type==52))
		{
			Items->DeleItem(i);
			corpses++;
		}
	}
	gcollect();
	sysmessage(s, "Done.");
	sysmessage(s, "%i corpses or gates have been cleaned.",corpses);
}

void command_reloadaccounts( UOXSOCKET s )
// Reloads the ACCOUNTS.ADM file.
{
	Admin->LoadAccounts();
	sysmessage( s, "accounts.adm reloaded." );
	return;
}
/* new commands go just above this line. :-) */

void command_reportbug( UOXSOCKET s )
// DESC:	Writes out a bug to the bug file
// DATE:	9th February, 2000
// CODER:	Abaddon
{
	FILE *bugs;
	bugs = fopen( "bugs.lst", "a+" );
	if( bugs == NULL )
	{
		ConOut( "ERROR: Unable to open bug list!\n" );
		return;
	}
	char dateTime[1024];
	time_t ltime;
	time( &ltime );
	char *t = ctime(&ltime);
	// just to be paranoid and avoid crashing
	if (NULL == t)
		t = "";
	else
	{
		// some ctime()s like to stick \r\n on the end, we don't want that
		for (int end = strlen(t) - 1; end >= 0 && isspace(t[end]); --end)
			t[end] = '\0';
	}
	safeCopy( dateTime, t, 1024);

	fprintf( bugs, "[%s] <%s> Reports: %s\n", dateTime, chars[currchar[s]].name, &tbuffer[Commands->cmd_offset+9]);
	fclose( bugs );
	sysmessage( s, "Thank you for your continuing support, your feedback is important to us" );
	bool x = false;
	for( int i = 0; i < now; i++ )
	{
		if( chars[currchar[i]].priv&0x20 )
		{
			x = true;
			sysmessage( i, temp );
		}
	}
	if( x )
	{
		sysmessage( s, "Available Game Masters have been notified of your bug submission." );
	}
	else
		sysmessage( s, "There was no Game Master available to note your bug report." );
}

