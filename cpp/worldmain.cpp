//------------------------------------------------------------------------
//  worldmain.cpp
//
//------------------------------------------------------------------------
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 1997 - 2001 by Marcus Rating (Cironian)
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
// WorldMain.cpp: implementation of the CWorldMain class.
//
//////////////////////////////////////////////////////////////////////

#include "..\h\uox3.h"
#include "..\h\worldmain.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CWorldMain::CWorldMain()
{
	announce(false/*0*/);

	iWsc = cWsc = NULL;
	Cur = 0;
	PerLoop = -1;
	isSaving = false;
}

CWorldMain::~CWorldMain()
{
	if (iWsc)
		fclose(iWsc);
	if (cWsc)
		fclose(cWsc);
}


//o---------------------------------------------------------------------------o
//|	Class		:	CWorldMain::savenewworld(char x)
//|	Date		:	Unknown
//|	Programmer	:	Unknown		(Moved from uox3.cpp by EviLDeD - Dec 22, 1998)
//o---------------------------------------------------------------------------o
//| Purpose		:	Save current world state. Stores all values in an easily
//|					readable script file "*.wsc". This stores all world items
//|					and NPC/PC character information for a given shard
//o---------------------------------------------------------------------------o
void CWorldMain::saveAccount()//Revana*
{
	unsigned int i;
	int a;
	
	//	EviLDeD	-	February 10, 2000
	//	If we are using the WWWAccount systen then we have to load the accounts first
	if(server_data.wwwaccounts)
	{
		printf("WWWAccounts: Reloading before update\n");
		Admin->LoadAccounts();
	}
	//	EviLDeD	-	End
	scpfile=fopen("accounts.adm","w");
	if (scpfile)
	{
		fprintf(scpfile, "//AV%s-UV%s-BD%s-DS%li-ED%s\n",SVER,VER,BUILD,time(NULL),BUILD);
		fprintf(scpfile, "//------------------------------------------------------------------------------\n");
		fprintf(scpfile, "//Accounts.adm[TEXT] : UOX3 uses this file for accounts storage for shard access\n");
		fprintf(scpfile, "//------------------------------------------------------------------------------\n");
		for (i=0;i<acctcount;i++)
		{
			if (acctx[i].wipe!=1)
			{
				fprintf(scpfile, "SECTION ACCOUNT\n");
				fprintf(scpfile, "{\n");
				fprintf(scpfile, "NAME %s\n",acctx[i].name);
				fprintf(scpfile, "PASS %s\n",acctx[i].pass);
				for(a=0;a< MAX_ACCT_LOCK;a++) fprintf(scpfile, "LOCK %i\n", acctx[i].lock[a]);
				if (acctx[i].ban==1) fprintf(scpfile, "BAN %i\n",acctx[i].banSerial);
				if (acctx[i].saveChar==1) fprintf(scpfile, "NOSAVECHAR\n");
				if (acctx[i].banTime!=-1) fprintf(scpfile, "TIMEBAN %i\n",acctx[i].banTime);
				if (acctx[i].warning>=1) fprintf(scpfile, "WARNINGS %i\n",acctx[i].warning);
				if (acctx[i].ip1!=0)
				{ 
					fprintf(scpfile, "LASTIP %i.%i.%i.%i\n",acctx[i].ip1,acctx[i].ip2,acctx[i].ip3,acctx[i].ip4);
				}
				else fprintf(scpfile, "LASTIP %s\n",acctx[i].tempIP);
				//	EviLDeD	-	February 10, 2000
				//	When player is wiped make thier name presant for the web
				fprintf(scpfile, "PUBLIC %s\n",acctx[i].listpublic?"ON":"OFF");				
				//	EviLDeD	-	End
				fprintf(scpfile, "CONTACT %s\n", acctx[i].contact);
				
				fprintf(scpfile, "}\n");
			}
		}
		fprintf(scpfile,"\n\nEOF\n\n");
		fclose(scpfile);
	}
	else
	{
		printf("Error, could not open accounts.adm for writing. Check file permissions.\n");
	}

	scpfile=fopen("wiped-acct.adm","w");
	if (scpfile)
	{
		for (i=0;i<wipecount;i++)
		{
			fprintf(scpfile, "WIPED ACCOUNT\n");
			fprintf(scpfile, "{\n");
			fprintf(scpfile, "NAME %s\n",acctx[i].name);
			fprintf(scpfile, "PASS %s\n",acctx[i].pass);
			//	EviLDeD	-	February 10, 2000
			//	When player is wiped make thier name presant for the web
			fprintf(scpfile, "PUBLIC ON\n");				
			//	EviLDeD	-	End
			fprintf(scpfile, "WIPESERIAL %i\n",acctx[i].wipeSerial);
			if (acctx[i].contact[0]!=0) fprintf(scpfile, "CONTACT %s\n", acctx[i].contact);
			fprintf(scpfile, "}\n");
		}
		
		for (i=0;i<acctcount;i++)
		{
			if (acctx[i].wipe==1)
			{
				fprintf(scpfile, "WIPED ACCOUNT\n");
				fprintf(scpfile, "{\n");
				fprintf(scpfile, "NAME %s\n",acctx[i].name);
				fprintf(scpfile, "PASS %s\n",acctx[i].pass);
				fprintf(scpfile, "WIPESERIAL %i\n",wiped[i].wipeSerial);
				if (wiped[i].contact[0]!=0) fprintf(scpfile, "CONTACT %s\n", wiped[i].contact);
				fprintf(scpfile, "}\n");
			}
		}
		
		fprintf(scpfile,"\n\nEOF\n\n");
		fclose(scpfile);
	}
	else
	{
		printf("Error, could not open wiped-acct.adm for writing. Check file permissions.\n");
	}
}

void CWorldMain::savenewworld(char x)
{
	static unsigned long ocCount, oiCount;
	static char curAmount = 0;

	tempeffectsoff();

	if ( !Saving() )
	{
		cwmWorldState->saveAccount();
		saveserverscript(x);
		savelog("Server data save\n","server.log");
		// Dupois - If shard op has defined an ARCHIVEPATH in SERVER.SCP
		//          then archive the WSC to the directory specified.
		// Added Oct 29, 1998
		if (strlen(server_data.archivepath)>1)
		{
			save_counter++; // LB - added world backup rate
			if(( save_counter % server_data.backup_save_ratio ) == 0 ) fileArchive("chars.wsc", "items.wsc", server_data.archivepath);
		}
		// End - Dupois

		//	gcollect();
		if ( announce() )
		{
			//	EviLDeD -	Put in the ability to toggle the display of WorldState
			//				for those operators that dont like it
			//	December 22, 1998
			sysbroadcast("World data saving, you may experience some lag for the next several minutes.");
			//	EviLDeD	-	END

			if ( PerLoop > 0 )
			{
				if (x)
					printf("SAVE: Starting manual world data save, saving %d items & chars every loop.\n", PerLoop );
				else
					printf("SAVE: Starting automatic world data save, saving %d items & chars every loop.\n", PerLoop );
			} else {
				if (x)
					printf("SAVE: Starting manual world data save, saving all items & chars now!\n" );
				else
					printf("SAVE: Starting automatic world data save, saving all items & chars now!\n" );
			}
		}


		cWsc=fopen("chars.wsc", "w");
		if (cWsc) 
		{
/*			fprintf(cWsc, "// UOX3 World Script (WSC)[TEXT]\n");
			fprintf(cWsc, "// Generated by UOX3 Version %s(Build:%s)\n",VER,BUILD);
			fprintf(cWsc, "// This file replaces the function of the *.UOX files in pre-0.50 versions[TEXT]\n");
			fprintf(cWsc, "// This file contains UoX3 CHARACTERS.\n");
			fprintf(cWsc, "INITMEM %i //Do NOT edit this line!\n\n",chars.Count()+1);	// Was: +100 (EviLDeD-Feb10,2000)*/
			fprintf( cWsc, "//CV%s-UV%s-BD%s-DS%li-ED%s\n", CVER, VER, BUILD, time(NULL), BUILD );
			fprintf( cWsc, "//o--------------------------------------------------------------------\n");
			fprintf( cWsc, "//| UOX3 - Character World Script \n" );
			fprintf( cWsc, "//| Generated by UOX3 Version %s(Build:%s)\n", VER, BUILD );
			fprintf( cWsc, "//o--------------------------------------------------------------------\n");
			fprintf(cWsc, "INITMEM %i //Do NOT edit this line!\n\n",chars.Count()+1);	// Was: +100 (EviLDeD-Feb10,2000)
		} else {
			printf("Error, couldn't open chars.wsc for writing. Check file permissions.\n");
			return;
		}

		iWsc=fopen("items.wsc", "w");
		if (iWsc)
		{
			//fprintf(wscfile, "ITEMS.wsc for UoX ITEMS and Guild information.\n\n\n");
			fprintf(iWsc, "//IV%s-UV%s-BD%s-DS%li-ED%s\n",IVER,VER,BUILD,time(NULL),BUILD);
			fprintf(iWsc, "//o------------------------------------------------------------------------------\n");
			fprintf(iWsc, "//| UOX3 - Item World Script\n");
			fprintf(iWsc, "//| Generated by UOX3 Version %s(Build:%s)\n",VER,BUILD);
			fprintf(iWsc, "//o------------------------------------------------------------------------------\n");
			fprintf(iWsc, "INITMEM %i //Do NOT edit this line!\n\n",items.Count()+1);	// Was: +100 (EviLDeD-Feb10,2000)
		} else {
			printf("Error, couldn't open items.wsc for writing. Check file permissions.\n");
			return;
		}

		oiCount = itemcount;
		ocCount = charcount;
		Cur = 0;
		isSaving = true;
	}

	if ( PerLoop > 0 )
		Max = min((Cur + PerLoop), max(oiCount, ocCount));
	else
		Max = max(oiCount, ocCount);

	//if (announce()) printf("SAVE: Saving %i to %i... ", Cur, Max);

	for ( ; Cur<Max ; Cur++)
	{
		if ( Cur < ocCount )
			SaveChar( Cur );

		if ( Cur < oiCount )
			SaveItem( Cur );
	}	// end for loop

	//if (announce()) printf("Done.\n");

	if ( Cur == max(oiCount, ocCount) )
	{ //Done saving
		if ( announce() )
		{
			sysbroadcast("World Save Complete.");
			printf("SAVE: World save complete.\n");
		}

		fprintf(cWsc,"\nEOF\n\n");
		fclose(cWsc);

		Guilds->Write( iWsc );
		fprintf(iWsc, "EOF\n\n");
		fclose(iWsc);

		Cur = Max = ocCount = oiCount = 0;
		isSaving = false;

		iWsc = cWsc = NULL;

		//	EviLDeD	-	February 10, 2000
		//	If accounts are to be loaded then they should be loaded
		//	all the time if using the web interface
		if( server_data.wwwaccounts )
		{
			Admin->LoadAccounts();
		}
		//	EviLDeD	-	End
		if( curAmount++ >= 2 && server_data.auto_acct != 0 )
		{
			Admin->LoadAccounts();		// Melen 9-7-99 Autoload accounts file
			curAmount = 0;
		}
	}

	tempeffectson();
	uiCurrentTime = getclock();
}

int CWorldMain::announce()
{
	return DisplayWorldSaves;
}

void CWorldMain::announce(int choice)
{
	if(choice<1)
		DisplayWorldSaves=0;
	else
		DisplayWorldSaves=1;
}

void CWorldMain::SetLoopSaveAmt( long toSet )
{
	if ( toSet <= 0 )
		PerLoop = -1;
	else
		PerLoop = toSet;
}

long CWorldMain::LoopSaveAmt( void )
{
	return PerLoop;
}

bool CWorldMain::Saving( void )
{
	return isSaving;
}

void CWorldMain::SaveChar( long i )
{
	char valid=0;
	int j;

	if (chars[i].account < MAXACCT && chars[i].account > -1)
	{
		if( acctx[chars[i].account].saveChar != 1 && acctx[chars[i].account].wipe != 1)
			valid=1;
	}
	else
		valid=1;
	// removed y > 200 (wind errors) and 6044 changed to 6144 (Abaddon )
	if (valid && chars[i].free==0 && chars[i].x > 200 && chars[i].x < 6144 && chars[i].y < 4096 && (strcmp(chars[i].name, "John Doe" )))
		valid=1;
	else valid=0;
	if( valid && chars[i].spawn1 < 0x40 && chars[i].spawn2 == 1 )	// don't need npc check, pcs won't ever have a spawn2 of 1
		valid = 0;
				
	if (valid)
		//		if( acctx[chars[i].account].saveChar != 1 && acctx[chars[i].account].wipe != 1 && chars[i].free==0 && chars[i].x > 200 && chars[i].y > 200 && chars[i].x < 6044 && chars[i].y < 4096  && (strcmp(chars[i].name, "John Doe" )))
	{
		fprintf(cWsc, "SECTION CHARACTER %i\n", i);
		fprintf(cWsc, "{\n");
		fprintf(cWsc, "SERIAL %i\n", (chars[i].ser1<<24)+(chars[i].ser2<<16)+(chars[i].ser3<<8)+chars[i].ser4);
		fprintf(cWsc, "NAME %s\n", chars[i].name);
		if( chars[i].title[0] != '\0' )
			fprintf(cWsc, "TITLE %s\n", chars[i].title);
		fprintf(cWsc, "ACCOUNT %i\n", chars[i].account);
		if (chars[i].laston[0] != '\0' )
			fprintf(cWsc, "LASTON %s\n", chars[i].laston);
		if (chars[i].x)
			fprintf(cWsc, "X %i\n", chars[i].x);
		if (chars[i].y)
			fprintf(cWsc, "Y %i\n", chars[i].y);
		if (chars[i].z)
			fprintf(cWsc, "Z %i\n", chars[i].z);
		if (chars[i].dispz)
			fprintf(cWsc, "DISPZ %i\n", chars[i].dispz);
		if (chars[i].oldx)
			fprintf(cWsc, "OLDX %i\n", chars[i].oldx);
		if (chars[i].oldy)
			fprintf(cWsc, "OLDY %i\n", chars[i].oldy);
		if (chars[i].oldz)
			fprintf(cWsc, "OLDZ %i\n", chars[i].oldz);
		if (chars[i].dir)
			fprintf(cWsc, "DIR %i\n", chars[i].dir);
		if ((chars[i].id1<<8)+chars[i].id2)
			fprintf(cWsc, "BODY %i\n", (chars[i].id1<<8)+chars[i].id2);
		if ((chars[i].xid1<<8)+chars[i].xid2)
			fprintf(cWsc, "XBODY %i\n", (chars[i].xid1<<8)+chars[i].xid2);
		if (( chars[i].orgid1<<8)+chars[i].orgid2 )
			fprintf( cWsc, "ORGBODY %i\n", (chars[i].orgid1<<8)+chars[i].orgid2 );
		if ((chars[i].skin1<<8)+chars[i].skin2)
			fprintf(cWsc, "SKIN %i\n", (chars[i].skin1<<8)+chars[i].skin2);
		if ((chars[i].xskin1<<8)+chars[i].xskin2)
			fprintf(cWsc, "XSKIN %i\n", (chars[i].xskin1<<8)+chars[i].xskin2);
		if (chars[i].priv)
			fprintf(cWsc, "PRIV %i\n", chars[i].priv);
		if( chars[i].runs && chars[i].npc )				// Abaddon, running
			fprintf(cWsc, "RUNS\n" );
		
		if (chars[i].priv2)
			fprintf(cWsc, "ALLMOVE %i\n", chars[i].priv2);
		if (chars[i].fonttype)
			fprintf(cWsc, "FONT %i\n", chars[i].fonttype);
		if ((chars[i].saycolor1<<8)+chars[i].saycolor2)
			fprintf(cWsc, "SAY %i\n", (chars[i].saycolor1<<8)+chars[i].saycolor2);
		if ((chars[i].emotecolor1<<8)+chars[i].emotecolor2)
			fprintf(cWsc, "EMOTE %i\n", (chars[i].emotecolor1<<8)+chars[i].emotecolor2);
		if (chars[i].st)
			fprintf(cWsc, "STRENGTH %i\n", chars[i].st);
		if (chars[i].st2)
			fprintf(cWsc, "STRENGTH2 %i\n", chars[i].st2);
		if (chars[i].dx)
			fprintf(cWsc, "DEXTERITY %i\n", chars[i].dx);
		if (chars[i].dx2)
			fprintf(cWsc, "DEXTERITY2 %i\n", chars[i].dx2);
		if (chars[i].in)
			fprintf(cWsc, "INTELLIGENCE %i\n", chars[i].in);
		if (chars[i].in2)
			fprintf(cWsc, "INTELLIGENCE2 %i\n", chars[i].in2);
		if (chars[i].hp)
			fprintf(cWsc, "HITPOINTS %i\n", chars[i].hp);
		if (chars[i].stm)
			fprintf(cWsc, "STAMINA %i\n", chars[i].stm);
		if (chars[i].mn)
			fprintf(cWsc, "MANA %i\n", chars[i].mn);
		if (chars[i].npc)
			fprintf(cWsc, "NPC %i\n", chars[i].npc);
		if (chars[i].shop)
			fprintf(cWsc, "SHOP %i\n", chars[i].shop);
		if ((chars[i].own1<<24)+(chars[i].own2<<16)+(chars[i].own3<<8)+chars[i].own4)
			fprintf(cWsc, "OWN %i\n", (chars[i].own1<<24)+(chars[i].own2<<16)+(chars[i].own3<<8)+chars[i].own4);
		if ((chars[i].robe1<<24)+(chars[i].robe2<<16)+(chars[i].robe3<<8)+chars[i].robe4)
			fprintf(cWsc, "ROBE %i\n", (chars[i].robe1<<24)+(chars[i].robe2<<16)+(chars[i].robe3<<8)+chars[i].robe4);
		if (chars[i].karma)
			fprintf(cWsc, "KARMA %i\n", chars[i].karma);
		if (chars[i].fame)
			fprintf(cWsc, "FAME %i\n", chars[i].fame);
		if (chars[i].kills)
			fprintf(cWsc, "KILLS %i\n", chars[i].kills);
		if( ( chars[i].shopSpawn ) || ( chars[i].shopSpawn==0 ) )
			fprintf( cWsc, "SHOPSPAWN %i\n", chars[i].shopSpawn );
		if (chars[i].deaths)
			fprintf(cWsc, "DEATHS %i\n", chars[i].deaths);
		if (chars[i].dead)
			fprintf(cWsc, "DEAD %i\n", chars[i].dead);
		if (chars[i].packitem)
			fprintf(cWsc, "PACKITEM %i\n", chars[i].packitem);
		if (chars[i].fixedlight)
			fprintf(cWsc, "FIXEDLIGHT %i\n", chars[i].fixedlight);
		if (chars[i].speech)
			fprintf(cWsc, "SPEECH %i\n", chars[i].speech);
		if (chars[i].trigger)
			fprintf(cWsc, "TRIGGER %i\n", chars[i].trigger);
		if (chars[i].trigword[0] != '\0')
			fprintf(cWsc, "TRIGWORD %s\n", chars[i].trigword);
		if (chars[i].disabled)
			fprintf(cWsc, "DISABLED %i\n", chars[i].disabled);
		
		for (j=0;j<TRUESKILLS;j++)
		{
			// Don't save the default value given by initchar
			if ((chars[i].baseskill[j] != 10)&&(chars[i].baseskill[j]>1)) 
				fprintf(cWsc, "SKILL%i %i\n", j, chars[i].baseskill[j]);
			// Since unsigned chars this will always be true >= 0.  So ....
			//if( chars[i].lockState[j] >= 0 && chars[i].lockState[j] <= 2 )
			if (chars[i].lockState[j] <= 2)    //punt
				fprintf(cWsc, "SKL%i %i\n", j, chars[i].lockState[j] );
		}
		//			for (j=0;j<ALLSKILLS;j++)	// commented out because frankly, skills aren't used when writing
		//			{
		// Don't save the default value given by initchar
		if( chars[i].cantrain==0 )
			fprintf(cWsc, "NOTRAIN\n");
		//			}
		
		if (chars[i].cell)
			fprintf(cWsc, "RESERVED1 %i\n", chars[i].cell);
		if (chars[i].att)
			fprintf(cWsc, "ATT %i\n", chars[i].att);
		if (chars[i].def)
			fprintf(cWsc, "DEF %i\n", chars[i].def);
		if (chars[i].lodamage)
			fprintf(cWsc, "LODAMAGE %i\n", chars[i].lodamage);
		if (chars[i].hidamage)
			fprintf(cWsc, "HIDAMAGE %i\n", chars[i].hidamage);
		if (chars[i].war)
			fprintf(cWsc, "WAR %i\n", chars[i].war);
		if (chars[i].npcWander)
			fprintf(cWsc, "NPCWANDER %i\n", chars[i].npcWander);
		if (chars[i].oldnpcWander)
			fprintf(cWsc, "OLDNPCWANDER %i\n", chars[i].oldnpcWander);
		if (chars[i].fx1)
			fprintf(cWsc, "FX1 %i\n", chars[i].fx1);
		if (chars[i].fy1)
			fprintf(cWsc, "FY1 %i\n", chars[i].fy1);
		if (chars[i].fz1)
			fprintf(cWsc, "FZ1 %i\n", chars[i].fz1);
		if (chars[i].fx2)
			fprintf(cWsc, "FX2 %i\n", chars[i].fx2);
		if (chars[i].fy2)
			fprintf(cWsc, "FY2 %i\n", chars[i].fy2);
		if ((chars[i].spawn1<<24)+(chars[i].spawn2<<16)+(chars[i].spawn3<<8)+chars[i].spawn4>-1)
			fprintf(cWsc, "SPAWN %i\n", (chars[i].spawn1<<24)+(chars[i].spawn2<<16)+(chars[i].spawn3<<8)+chars[i].spawn4);
		if (chars[i].hidden)
			fprintf(cWsc, "HIDDEN %i\n", chars[i].hidden);
		if (chars[i].hunger)
			fprintf(cWsc, "HUNGER %i\n", chars[i].hunger);
		if (chars[i].npcaitype)
			fprintf(cWsc, "NPCAITYPE %i\n", chars[i].npcaitype);
		if (chars[i].spattack)
			fprintf(cWsc, "SPATTACK %i\n", chars[i].spattack);
		if (chars[i].spadelay)
			fprintf(cWsc, "SPADELAY %i\n", chars[i].spadelay);
		if (chars[i].taming)
			fprintf(cWsc, "TAMING %i\n", chars[i].taming);
		if (chars[i].summontimer)
			fprintf(cWsc, "SUMMONTIMER %i\n", chars[i].summontimer);
		if (chars[i].town)
			fprintf(cWsc, "TOWN %i\n", chars[i].town);
		if ((chars[i].townvote1<<24)+(chars[i].townvote2<<16)+(chars[i].townvote3<<8)+chars[i].townvote4)
			fprintf(cWsc, "TOWNVOTE %i\n", (chars[i].townvote1<<24)+(chars[i].townvote2<<16)+(chars[i].townvote3<<8)+chars[i].townvote4);
		if (chars[i].towntitle)
			fprintf(cWsc, "TOWNTITLE %i\n", chars[i].towntitle);
		if (chars[i].townpriv)
			fprintf(cWsc, "TOWNPRIV %i\n", chars[i].townpriv);
		if (chars[i].advobj)
			fprintf(cWsc, "ADVOBJ %i\n", chars[i].advobj);
		if (chars[i].poison)
			fprintf(cWsc, "POISON %i\n", chars[i].poison);
		if (chars[i].poisoned)
			fprintf(cWsc, "POISONED %i\n", chars[i].poisoned);
		if (chars[i].fleeat)
			fprintf(cWsc, "FLEEAT %i\n", chars[i].fleeat);
		if (chars[i].reattackat)
			fprintf(cWsc, "REATTACKAT %i\n", chars[i].reattackat);
		if (chars[i].split)
			fprintf(cWsc, "SPLIT %i\n", chars[i].split);
		if (chars[i].splitchnc)
			fprintf(cWsc, "SPLITCHANCE %i\n", chars[i].splitchnc);
		// Begin of Guild related things (DasRaetsel)
		if (chars[i].guildtoggle)
			fprintf(cWsc, "GUILDTOGGLE %i\n", chars[i].guildtoggle);  
		if (chars[i].guildnumber)
			fprintf(cWsc, "GUILDNUMBER %i\n", chars[i].guildnumber);  
		if (chars[i].guildtitle[0] != '\0')
			fprintf(cWsc, "GUILDTITLE %s\n", chars[i].guildtitle);  
		if (chars[i].guildfealty)
			fprintf(cWsc, "GUILDFEALTY %i\n", chars[i].guildfealty);  
		if (chars[i].murderrate )			// used to be guildfealty
			fprintf(cWsc, "MURDERRATE %li\n",chars[i].murderrate);
		// End of guild stuff
		// Begin of Race related things (Abaddon)
		fprintf(cWsc, "RACE %i\n", chars[i].race);
		fprintf(cWsc, "RACEGATE %i\n", chars[i].raceGate);
		// End of Race stuff
		fprintf(cWsc, "COMMANDLEVEL %i\n", chars[i].commandLevel );	// command level
		if( chars[i].questType )
			fprintf(cWsc, "QUESTTYPE %i\n", chars[i].questType );	
		if( chars[i].questDestRegion)
			fprintf(cWsc, "QUESTDESTREGION %i\n", chars[i].questDestRegion);	
		if( chars[i].questOrigRegion )
			fprintf(cWsc, "QUESTORIGREGION %i\n", chars[i].questOrigRegion );	

		if ( !(chars[i].npc || chars[i].priv&1 || chars[i].priv&80) )
		{
			fprintf(cWsc, "ATROPHY");
			for ( j=0;j<ALLSKILLS;j++ )
			{
				if ( chars[i].atrophy[j] >= 10 )
					fprintf(cWsc, " %i", chars[i].atrophy[j]);
				else 
					fprintf(cWsc, " 0%i", chars[i].atrophy[j]);
			}
			fprintf(cWsc, "\n");
		}
		
		fprintf(cWsc, "}\n\n");
	}
}

void CWorldMain::SaveItem( long i )
{
	long serial, serhash, j, multi;
	int ci;
	unsigned int k;


	if (items[i].priv&0x01 && items[i].contserial == -1 && items[i].free==0)
	{
		if( items[i].decaytime == 0)
		{
			items[i].decaytime=server_data.decaytimer*CLOCKS_PER_SEC+uiCurrentTime;
		}
		else if (items[i].decaytime<uiCurrentTime)
		{
			if (items[i].corpse==1)
			{
				serial=items[i].serial;
				serhash=serial%HASHMAX;
				for (ci=0;ci<contsp[serhash].max;ci++)
				{
					j=contsp[serhash].pointer[ci];
					if( j != -1 )
					{
						if ((items[j].contserial==items[i].serial) &&
							(items[j].layer!=0x0B)&&(items[j].layer!=0x10))
						{
							removefromptr(&contsp[items[j].contserial%HASHMAX], j);
							items[j].cont1=255;
							items[j].cont2=255;
							items[j].cont3=255;
							items[j].cont4=255;
							items[j].contserial=-1;
							// Tauriel remove item from world mapcells
							mapRegions->RemoveItem(i); //remove this item from a map cell
							
							items[j].x=items[i].x;
							items[j].y=items[i].y;
							items[j].z=items[i].z;
							mapRegions->AddItem(j); //add this item to a map cell
							for (k=0;k<now;k++)
							{
								if (perm[k] && inrange2(k, j))
								{
									senditem(k,j);
								}
							}
						}
						if ((items[j].contserial==items[i].serial) &&
							(items[j].free==0)&&
							((items[j].layer==0x0B)||(items[j].layer==0x10)))
						{
							Items->DeleItem(j);
						}
					}
				}
			}
			else
			{
				if ( items[i].multis == -1 )
				{
					multi = findmulti( items[i].x, items[i].y, items[i].z );
					if( multi == -1 )
					{
						Items->DeleItem(i);
					}
				}
			}
		}
	}
	
	// removed y > 200 (wind errors) and 6044 changed to 6144 (Abaddon)
	if (items[i].free==0 && ( items[i].contserial>-1 || (items[i].x > 200 && items[i].x < 6144 && items[i].y < 4096 )))
	{
		fprintf(iWsc, "SECTION WORLDITEM %i\n", i);
		fprintf(iWsc, "{\n");
		fprintf(iWsc, "SERIAL %i\n", (items[i].ser1<<24)+(items[i].ser2<<16)+(items[i].ser3<<8)+items[i].ser4);
		fprintf(iWsc, "NAME %s\n", items[i].name);
		if (strlen(items[i].name2)>0)
			fprintf(iWsc, "NAME2 %s\n", items[i].name2);
		if( strlen( items[i].creator ) > 0 ) 
			fprintf(iWsc, "CREATOR %s\n", items[i].creator ); // by Magius(CHE)
		if( items[i].madewith ) 
			fprintf( iWsc, "SK_MADE %i\n", items[i].madewith ); // by Magius(CHE)
		
		fprintf(iWsc, "ID %i\n", (items[i].id1<<8)+items[i].id2);
		
		if (items[i].x)
			fprintf(iWsc, "X %i\n", items[i].x);
		if (items[i].y)
			fprintf(iWsc, "Y %i\n", items[i].y);
		if (items[i].z)
			fprintf(iWsc, "Z %i\n", items[i].z);
		if ((items[i].color1<<8)+items[i].color2)
			fprintf(iWsc, "COLOR %i\n", (items[i].color1<<8)+items[i].color2);
		if ((items[i].cont1<<24)+(items[i].cont2<<16)+(items[i].cont3<<8)+items[i].cont4)
			fprintf(iWsc, "CONT %i\n", (items[i].cont1<<24)+(items[i].cont2<<16)+(items[i].cont3<<8)+items[i].cont4);
		if (items[i].layer)
			fprintf(iWsc, "LAYER %i\n", items[i].layer);
		if( items[i].itmhand )
			fprintf( iWsc, "ITEMHAND %i\n", items[i].itmhand );
		if (items[i].type)
			fprintf(iWsc, "TYPE %i\n", items[i].type);
		if (items[i].type2)
			fprintf(iWsc, "TYPE2 %i\n", items[i].type2);
		if (items[i].offspell)
			fprintf(iWsc, "OFFSPELL %i\n", items[i].offspell);
		if ((items[i].more1<<24)+(items[i].more2<<16)+(items[i].more3<<8)+items[i].more4) //; 
			fprintf(iWsc, "MORE %i\n", (items[i].more1<<24)+(items[i].more2<<16)+(items[i].more3<<8)+items[i].more4);
		if ((items[i].moreb1<<24)+(items[i].moreb2<<16)+(items[i].moreb3<<8)+items[i].moreb4)
			fprintf(iWsc, "MORE2 %i\n", (items[i].moreb1<<24)+(items[i].moreb2<<16)+(items[i].moreb3<<8)+items[i].moreb4);
		if (items[i].morex)
			fprintf(iWsc, "MOREX %i\n", items[i].morex);
		if (items[i].morey)
			fprintf(iWsc, "MOREY %i\n", items[i].morey);
		if (items[i].morez)
			fprintf(iWsc, "MOREZ %i\n", items[i].morez);
		if (items[i].amount)
			fprintf(iWsc, "AMOUNT %i\n", items[i].amount);
		if (items[i].pileable)
			fprintf(iWsc, "PILEABLE %i\n", items[i].pileable);
		if (items[i].doordir)
			fprintf(iWsc, "DOORFLAG %i\n", items[i].doordir);
		if (items[i].dye)
			fprintf(iWsc, "DYEABLE %i\n", items[i].dye);
		if (items[i].corpse)
			fprintf(iWsc, "CORPSE %i\n", items[i].corpse);
		if (items[i].att)
			fprintf(iWsc, "ATT %i\n", items[i].att);
		if (items[i].def)
			fprintf(iWsc, "DEF %i\n", items[i].def);
		if (items[i].hidamage)
			fprintf(iWsc, "HIDAMAGE %i\n", items[i].hidamage);
		if (items[i].lodamage)
			fprintf(iWsc, "LODAMAGE %i\n", items[i].lodamage);
		if (items[i].st)
			fprintf(iWsc, "ST %i\n", items[i].st);
		if (items[i].weight)
			fprintf(iWsc, "WEIGHT %i\n", items[i].weight);
		if (items[i].hp)
			fprintf(iWsc, "HP %i\n", items[i].hp);
		if( items[i].maxhp )
			fprintf( iWsc, "MAXHP %i\n", items[i].maxhp ); // Magius(CHE)
		if( items[i].rank >0 && items[i].rank < 10 )
			fprintf( iWsc, "RANK %i\n", items[i].rank ); // Magius(CHE)
		if (items[i].st2)
			fprintf(iWsc, "ST2 %i\n", items[i].st2);
		if (items[i].dx)
			fprintf(iWsc, "DX %i\n", items[i].dx);
		if (items[i].dx2)
			fprintf(iWsc, "DX2 %i\n", items[i].dx2);
		if (items[i].in)
			fprintf(iWsc, "IN %i\n", items[i].in);
		if (items[i].in2)
			fprintf(iWsc, "IN2 %i\n", items[i].in2);
		if (items[i].spd)
			fprintf(iWsc, "SPD %i\n", items[i].spd);
		if (items[i].poisoned) //AntiChrist -- poisoning skill
			fprintf(iWsc, "POISONED %i\n", items[i].poisoned);
		if (items[i].wipe)
			fprintf(iWsc, "WIPE %i\n", items[i].wipe);
		if (items[i].magic)
			fprintf(iWsc, "MOVABLE %i\n", items[i].magic);
		if ((items[i].owner1<<24)+(items[i].owner2<<16)+(items[i].owner3<<8)+items[i].owner4)
			fprintf(iWsc, "OWNER %i\n", (items[i].owner1<<24)+(items[i].owner2<<16)+(items[i].owner3<<8)+items[i].owner4);
		if (items[i].visible)
			fprintf(iWsc, "VISIBLE %i\n", items[i].visible);
		if ((items[i].spawn1<<24)+(items[i].spawn2<<16)+(items[i].spawn3<<8)+items[i].spawn4)
			fprintf(iWsc, "SPAWN %i\n", (items[i].spawn1<<24)+(items[i].spawn2<<16)+(items[i].spawn3<<8)+items[i].spawn4);
		if (items[i].dir)
			fprintf(iWsc, "DIR %i\n", items[i].dir);
		if (items[i].priv)
			fprintf(iWsc, "PRIV %i\n", items[i].priv);
		if (items[i].value)
			fprintf(iWsc, "VALUE %i\n", items[i].value);
		if (items[i].restock)
			fprintf(iWsc, "RESTOCK %i\n", items[i].restock);
		if (items[i].trigger)
			fprintf(iWsc, "TRIGGER %i\n", items[i].trigger);
		if (items[i].trigtype)
			fprintf(iWsc, "TRIGTYPE %i\n", items[i].trigtype);
		if (items[i].disabled)
			fprintf(iWsc, "DISABLED %i\n", items[i].disabled);
		if (items[i].tuses)
			fprintf(iWsc, "USES %i\n", items[i].tuses);
		if (items[i].good >= 0 )
			fprintf(iWsc, "GOOD %i\n", items[i].good); // Magius(CHE)
		if( items[i].glow )
			fprintf( iWsc, "GLOW %i\n", items[i].glow );
		if( (items[i].glow_c1<<8) + items[i].glow_c2 )
			fprintf( iWsc, "GLOWBC %i\n", (items[i].glow_c1<<8) + items[i].glow_c2 );
		if( items[i].glow_effect )
			fprintf( iWsc, "GLOWTYPE %i\n", items[i].glow_effect );
		if( items[i].racialEffect != 65535 )
			fprintf(iWsc, "RACE %i\n", items[i].racialEffect);
		if( strlen( items[i].desc ) > 0 )
			fprintf( iWsc, "DESC %i\n", items[i].desc );	// save out our vendor description
		
		fprintf(iWsc, "}\n\n");
	}
}

