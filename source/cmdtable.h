//------------------------------------------------------------------------
//  cmdtable.h
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

/* cmdtable.h - Crackerjack <crackerjack@crackerjack.net> July 24/99
 * This code is an attempt to clean up the messy "if/then/else" routines
 * currently in use for GM commands, as well as adding more functionality
 * and more potential for functionality.
 *
 * Current features:
 * - Actual table of commands to execute, what perms are required, dialog
 *   messages for target commands, etc handled by a central system
 *
 */

#ifndef __CMDTABLE_H
#define __CMDTABLE_H

/* Types of commands */
#define CMD_ITEMMENU	1	/* Open item menu specified in cmd_extra */
#define CMD_TARGET	2	/* Call target struct specified in cmd_extra */
#define	CMD_FUNC	3	/* Call function specified in cmd_extra
					must be of type GMFUNC */
#define CMD_TARGETX	4	/* target with addx[] argument */
#define CMD_TARGETXY	5	/* target with addx & y [] arguments */
#define CMD_TARGETXYZ	6	/* target with addx & y & z [] arguments */
#define CMD_TARGETHX	7	/* target with hex addx[] argument */
#define CMD_TARGETHXY	8	/* target with hex addx & y arguments */
#define CMD_TARGETHXYZ	9	/* target with hex addx & y & z arguments */
#define CMD_TARGETID1	10	/* target with 1 addid # */
#define CMD_TARGETID2	11	/* target with 2 addid #s */
#define CMD_TARGETID3	12	/* target with 3 addid #s */
#define CMD_TARGETID4	13	/* target with 4 addid #s */
#define CMD_TARGETHID1	14	/* target with 1 hex addid # */
#define CMD_TARGETHID2	15	/* target with 2 hex addid #s */
#define CMD_TARGETHID3	16	/* target with 3 hex addid #s */
#define CMD_TARGETHID4	17	/* target with 4 hexaddid #s */
#define CMD_TARGETTMP	18	/* target with tempint */
#define CMD_TARGETHTMP	19	/* target with hex tempint */

typedef struct cmdtable_s CMDTABLE_S;
struct cmdtable_s {
	const char *	cmd_name;
	unsigned int	cmd_priv_m;	/* PRIV3 byte# - 0-6, 255=no privs needed */ // corrupted to GM = 2, CNS = 1, player = 0
	unsigned int	cmd_priv_b;	/* PRIV3 bit within byte - 0-31 */			// going to ignore this
	unsigned int	cmd_type;	/* Type of command - see above */
	void 		(*cmd_extra) ();	/* extra data - see above */
};
extern CMDTABLE_S command_table[];

#define CMD_EXEC	void (*) (UOXSOCKET)
#define CMD_DEFINE	void (*)()

typedef struct target_s TARGET_S;
struct target_s {	/* arguments to the target() function */
	unsigned char	a1, a2, a3, a4;
	char	txt[128];
};

/* All command_ functions take an int value of the player that triggered the command. */
#define CMD_HANDLER(name) extern void name (UOXSOCKET)
#define TAR_HANDLER(name) extern TARGET_S name

/* Defined commands that are just being mapped to internal functions */
#define command_time telltime

/* All defined commands */
CMD_HANDLER(command_cleanup);
CMD_HANDLER(command_regspawnall);
CMD_HANDLER(command_wipenpcs);
CMD_HANDLER(command_gms);
CMD_HANDLER(command_who);
CMD_HANDLER(command_gmtransfer);
CMD_HANDLER(command_gotocur);
CMD_HANDLER(command_resend);
CMD_HANDLER(command_post);
CMD_HANDLER(command_gpost);
CMD_HANDLER(command_rpost);
CMD_HANDLER(command_lpost);
CMD_HANDLER(command_teleport);
CMD_HANDLER(command_where);
CMD_HANDLER(command_q);
CMD_HANDLER(command_next);
CMD_HANDLER(command_clear);
/* CMD_HANDLER(command_gotocur); */
/* CMD_HANDLER(command_gmtransfer); */
CMD_HANDLER(command_goplace);
CMD_HANDLER(command_gochar);
CMD_HANDLER(command_fix);
CMD_HANDLER(command_xgoplace);
CMD_HANDLER(command_showids);
CMD_HANDLER(command_poly);
CMD_HANDLER(command_skin);
CMD_HANDLER(command_action);
CMD_HANDLER(command_xtele);
CMD_HANDLER(command_go);
CMD_HANDLER(command_zerokills);
CMD_HANDLER(command_tile);
CMD_HANDLER(command_wipe);
CMD_HANDLER(command_iwipe);
CMD_HANDLER(command_add);
CMD_HANDLER(command_eagle);
CMD_HANDLER(command_addx);
CMD_HANDLER(command_rename);
CMD_HANDLER(command_title);
CMD_HANDLER(command_save);
CMD_HANDLER(command_dye);
CMD_HANDLER(command_wtrig);
CMD_HANDLER(command_setpriv);
CMD_HANDLER(command_nodecay);
CMD_HANDLER(command_send);
CMD_HANDLER(command_showtime);
CMD_HANDLER(command_settime);
CMD_HANDLER(command_shutdown);
CMD_HANDLER(command_wholist);
CMD_HANDLER(command_blt2);
CMD_HANDLER(command_sfx);
CMD_HANDLER(command_light);
CMD_HANDLER(command_web);
CMD_HANDLER(command_disconnect);
CMD_HANDLER(command_tell);
/* lord binary's debugging code
CMD_HANDLER(command_npcs);
CMD_HANDLER(command_npcsd);
CMD_HANDLER(command_pcs);
CMD_HANDLER(command_charc);
CMD_HANDLER(command_itemc);
 * end of lord binary's debugging code */
CMD_HANDLER(command_gmmenu);
CMD_HANDLER(command_itemmenu);
CMD_HANDLER(command_additem);
CMD_HANDLER(command_dupe);
CMD_HANDLER(command_command);
CMD_HANDLER(command_gcollect);
CMD_HANDLER(command_allmoveon);
CMD_HANDLER(command_allmoveoff);
CMD_HANDLER(command_showhs);
CMD_HANDLER(command_hidehs);
CMD_HANDLER(command_set);
CMD_HANDLER(command_temp);
CMD_HANDLER(command_addnpc);
CMD_HANDLER(command_readini);
CMD_HANDLER(command_gumpmenu);
CMD_HANDLER(command_cachestats);
CMD_HANDLER(command_npcrect);
CMD_HANDLER(command_npccircle);
CMD_HANDLER(command_npcwander);
CMD_HANDLER(command_secondsperuominute);
CMD_HANDLER(command_brightlight);
CMD_HANDLER(command_darklight);
CMD_HANDLER(command_dungeonlight);
CMD_HANDLER(command_time);
CMD_HANDLER(command_gmopen);
CMD_HANDLER(command_restock);
CMD_HANDLER(command_restockall);
CMD_HANDLER(command_setshoprestockrate);
/* CMD_HANDLER(command_who);
CMD_HANDLER(command_gms); */
CMD_HANDLER(command_midi);
CMD_HANDLER(command_gumpopen);
CMD_HANDLER(command_respawn);
/* CMD_HANDLER(command_regspawnall); */
CMD_HANDLER(command_regspawnmax);
CMD_HANDLER(command_regspawn);
CMD_HANDLER(command_reloadserver);
CMD_HANDLER(command_reloadaccounts);
CMD_HANDLER(command_loaddefaults);
CMD_HANDLER(command_cq);
/* CMD_HANDLER(command_wipenpcs); */
CMD_HANDLER(command_cnext);
CMD_HANDLER(command_cclear);
CMD_HANDLER(command_minecheck);
CMD_HANDLER(command_invul);
CMD_HANDLER(command_rename2);
CMD_HANDLER(command_noinvul);
CMD_HANDLER(command_guardson);
CMD_HANDLER(command_guardsoff);
CMD_HANDLER(command_announceon);
CMD_HANDLER(command_announceoff);
CMD_HANDLER(command_wf);
CMD_HANDLER(command_decay);
CMD_HANDLER(command_killall);
CMD_HANDLER(command_pdump);
CMD_HANDLER(command_rename);
CMD_HANDLER(command_readspawnregions);
/* CMD_HANDLER(command_cleanup); */
CMD_HANDLER(command_gy);
CMD_HANDLER(command_tilew);
CMD_HANDLER(command_squelch);
CMD_HANDLER(command_squelch);
CMD_HANDLER(command_spawnkill);
CMD_HANDLER(command_wanim);
CMD_HANDLER(command_setacct);
CMD_HANDLER(command_addacct);
CMD_HANDLER(command_banacct);
CMD_HANDLER(command_unbanacct);
CMD_HANDLER(command_removeacct);
CMD_HANDLER(command_setrace);
CMD_HANDLER(command_setmurder);
CMD_HANDLER(command_getlight);
CMD_HANDLER(command_reportbug);

/* all defined target commands */
TAR_HANDLER(target_acct);
TAR_HANDLER(target_use);
TAR_HANDLER(target_jail);
TAR_HANDLER(target_release);
TAR_HANDLER(target_istats);
TAR_HANDLER(target_cstats);
TAR_HANDLER(target_tele);
TAR_HANDLER(target_xgo);
TAR_HANDLER(target_setmorex);
TAR_HANDLER(target_setmorey);
TAR_HANDLER(target_setmorez);
TAR_HANDLER(target_setmorexyz);
TAR_HANDLER(target_sethexmorexyz);
TAR_HANDLER(target_setnpcai);
TAR_HANDLER(target_xbank);
TAR_HANDLER(target_xsbank); // AntiChrist
TAR_HANDLER(target_remove);
TAR_HANDLER(target_newz);
TAR_HANDLER(target_settype);
TAR_HANDLER(target_itrig);
TAR_HANDLER(target_ctrig);
TAR_HANDLER(target_ttrig);
TAR_HANDLER(target_setid);
TAR_HANDLER(target_trainer);
TAR_HANDLER(target_setmore);
TAR_HANDLER(target_makegm);
TAR_HANDLER(target_makecns);
TAR_HANDLER(target_killhair);
TAR_HANDLER(target_killbeard);
TAR_HANDLER(target_killpack);
TAR_HANDLER(target_setfont);
TAR_HANDLER(target_kill);
TAR_HANDLER(target_resurrect);
TAR_HANDLER(target_bolt);
TAR_HANDLER(target_npcaction);
TAR_HANDLER(target_setamount);
TAR_HANDLER(target_setamount2);
TAR_HANDLER(target_kick);
TAR_HANDLER(target_movetobag);
TAR_HANDLER(target_setmovable);
TAR_HANDLER(target_setvisible);
TAR_HANDLER(target_setdir);
TAR_HANDLER(target_setspeech);
TAR_HANDLER(target_setowner);
TAR_HANDLER(target_freeze);
TAR_HANDLER(target_unfreeze);
TAR_HANDLER(target_tiledata);
TAR_HANDLER(target_recall);
TAR_HANDLER(target_mark);
TAR_HANDLER(target_gate);
TAR_HANDLER(target_heal);
TAR_HANDLER(target_npctarget);
TAR_HANDLER(target_tweak);
TAR_HANDLER(target_sbopen);
TAR_HANDLER(target_mana);
TAR_HANDLER(target_stamina);
TAR_HANDLER(target_makeshop);
TAR_HANDLER(target_buy);
TAR_HANDLER(target_setvalue);
TAR_HANDLER(target_setrestock);
TAR_HANDLER(target_sell);
TAR_HANDLER(target_setspattack);
TAR_HANDLER(target_setspadelay);
TAR_HANDLER(target_setpoison);
TAR_HANDLER(target_setpoisoned);
TAR_HANDLER(target_setadvobj);
TAR_HANDLER(target_setwipe);
TAR_HANDLER(target_fullstats);
TAR_HANDLER(target_hide);
TAR_HANDLER(target_unhide);
TAR_HANDLER(target_house);
TAR_HANDLER(target_split);
TAR_HANDLER(target_splitchance);
TAR_HANDLER(target_possess);
TAR_HANDLER(target_telestuff);
TAR_HANDLER(target_newx);
TAR_HANDLER(target_newy);
TAR_HANDLER(target_incx);
TAR_HANDLER(target_incy);
TAR_HANDLER(target_incz);
TAR_HANDLER(target_ban);
TAR_HANDLER(target_glow);
TAR_HANDLER(target_unglow);	
TAR_HANDLER(target_showskills);
TAR_HANDLER(target_showdetail);
TAR_HANDLER(target_removeshop);	// allows us to remove the shopkeeper layers for an npc/pc
TAR_HANDLER(target_commandlevel);
#endif /* __CMDTABLE_H */
