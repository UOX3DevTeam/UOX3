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

#include <uox3.h>
#include <worldmain.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CWorldMain::CWorldMain()
{
	announce( false );

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
		ConOut("WWWAccounts: Reloading before update\n");
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
		ConOut("Error, could not open accounts.adm for writing. Check file permissions.\n");
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
		ConOut("Error, could not open wiped-acct.adm for writing. Check file permissions.\n");
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
					ConOut("SAVE: Starting manual world data save, saving %d items & chars every loop.\n", PerLoop );
				else
					ConOut("SAVE: Starting automatic world data save, saving %d items & chars every loop.\n", PerLoop );
			} else {
				if (x)
					ConOut("SAVE: Starting manual world data save, saving all items & chars now!\n" );
				else
					ConOut("SAVE: Starting automatic world data save, saving all items & chars now!\n" );
			}
		}


		cWsc=fopen("chars.wsc", "w");
		if (cWsc) 
		{
			fprintf( cWsc, "//CV%s-UV%s-BD%s-DS%li-ED%s\n", CVER, VER, BUILD, time(NULL), BUILD );
			fprintf( cWsc, "//o--------------------------------------------------------------------\n");
			fprintf( cWsc, "//| UOX3 - Character World Script \n" );
			fprintf( cWsc, "//| Generated by UOX3 Version %s(Build:%s)\n", VER, BUILD );
			fprintf( cWsc, "//o--------------------------------------------------------------------\n");
			fprintf(cWsc, "INITMEM %i //Do NOT edit this line!\n\n",chars.Count()+1);	// Was: +100 (EviLDeD-Feb10,2000)
		} else {
			ConOut("Error, couldn't open chars.wsc for writing. Check file permissions.\n");
			return;
		}

		iWsc=fopen("items.wsc", "w");
		if (iWsc)
		{
			fprintf(iWsc, "//IV%s-UV%s-BD%s-DS%li-ED%s\n",IVER,VER,BUILD,time(NULL),BUILD);
			fprintf(iWsc, "//o------------------------------------------------------------------------------\n");
			fprintf(iWsc, "//| UOX3 - Item World Script\n");
			fprintf(iWsc, "//| Generated by UOX3 Version %s(Build:%s)\n",VER,BUILD);
			fprintf(iWsc, "//o------------------------------------------------------------------------------\n");
			fprintf(iWsc, "INITMEM %i //Do NOT edit this line!\n\n",items.Count()+1);	// Was: +100 (EviLDeD-Feb10,2000)
		} else {
			ConOut("Error, couldn't open items.wsc for writing. Check file permissions.\n");
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

	for ( ; Cur<Max ; Cur++)
	{
		if ( Cur < ocCount )
			SaveChar( Cur );

		if ( Cur < oiCount )
			SaveItem( Cur );
	}	// end for loop

	if ( Cur == max(oiCount, ocCount) )
	{ //Done saving
		if ( announce() )
		{
			sysbroadcast("World Save Complete.");
			ConOut("SAVE: World save complete.\n");
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
		DisplayWorldSaves = 0;
	else
		DisplayWorldSaves = 1;
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
	// 7000 is reserved for pets
	if (valid && chars[i].free==0 && chars[i].x > 200 && chars[i].x < 6144 && chars[i].y < 4096 && (strcmp(chars[i].name, "John Doe" )))
		valid=1;
	else valid = 0;
	if( !valid && chars[i].x == 7000 && chars[i].y == 7000 )		// Required for saving of mounts
	if( valid && chars[i].spawn1 < 0x40 && chars[i].spawn2 == 1 )	// don't need npc check, pcs won't ever have a spawn2 of 1
		valid = 0;
				
	if (valid)
	{
		fprintf(cWsc, "SECTION CHARACTER %i\n", i);
		fprintf(cWsc, "{\n");
		fprintf(cWsc, "SERIAL %i\n", chars[i].serial);
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
		if (chars[i].robe != -1)
			fprintf(cWsc, "ROBE %i\n", chars[i].robe);
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

void CWorldMain::loadatrophy( CHARACTER c, char *astr )
{
	char a[3];
	unsigned int l, n=0, sl=strlen(astr);

    //07 12 34 45 etc..
	//number1 number2 space...
	for ( l=0;l<sl;l+=3 )
	{
		a[0] = astr[l];
		a[1] = astr[l+1];
		a[2] = 0;

		chars[c].atrophy[n]=str2num(a);
		n++;
	}
}

void CWorldMain::loadchar( CHARACTER x ) // Load a character from WSC
{
	unsigned long int i,k;
	int j,a=0, loops=0;
	char newpoly=0;

	x = Npcs->MemCharFree();
	if( x == -1 ) 
		return;
	Npcs->InitChar( x, 0 );

	chars[x].dir = 4;
	chars[x].hp = chars[x].st = 10;
	chars[x].stm = chars[x].dx = 10;
	chars[x].mn = chars[x].in = 10;
	chars[x].race = 0;
	chars[x].raceGate = 65535;
	chars[x].tamed = false;
	chars[x].runs = false;
	chars[x].guarded = false;
	chars[x].step = 1;
	signed char lockstate = 0;
	int skl = 0;
	newpoly = 0; // this goes with "ORGBODY" but its setting a default up front
	for (i=0;i<TRUESKILLS;i++)
	{
		chars[x].baseskill[i]=1;
		chars[x].skill[i]=1;
	}
	chars[x].namedeed = 5;
	do
	{
		readw2();
		switch( script1[0] )
		{
		case 'a':
		case 'A':
			if( !strcmp( script1, "ACCOUNT" ) ) chars[x].account = str2num( script2 ); 
			else if( !strcmp( script1, "ADVOBJ" ) ) chars[x].advobj = str2num( script2 ); 
			else if( !strcmp( script1, "ALLMOVE" ) ) chars[x].priv2 = str2num( script2 );
			else if( !strcmp( script1, "ATT" ) ) chars[x].att = str2num( script2 );
			else if( !strcmp( script1, "ATROPHY" ) ) loadatrophy( x, script2 );
			break;
		case 'b':
		case 'B':
			if( !strcmp( script1, "BODY" ) )
			{
				i = str2num( script2 );
				chars[x].id1 = (unsigned char)(i>>8);
				chars[x].id2 = (unsigned char)(i%256);
				chars[x].orgid1 = (unsigned char)(i>>8);
				chars[x].orgid2 = (unsigned char)(i%256);
			}
			break;
		case 'c':
		case 'C':
			if( !strcmp( script1, "COMMANDLEVEL" ) )
				chars[x].commandLevel = str2num( script2 );
			break;
		case 'd':
		case 'D':
			if( !strcmp( script1, "DEAD" ) ) chars[x].dead = str2num( script2 );
			else if( !strcmp( script1, "DEATHS" ) ) chars[x].deaths = str2num( script2 );
			else if( !strcmp( script1, "DEF" ) ) chars[x].def = str2num(script2);
			else if( !strcmp( script1, "DEXTERITY" ) ) chars[x].dx = str2num( script2 );
			else if( !strcmp( script1, "DEXTERITY2" ) ) chars[x].dx2 = str2num( script2 );
			else if( !strcmp( script1, "DIR" ) ) chars[x].dir = str2num( script2 ) & 0x0F;
			else if( !strcmp( script1, "DISABLED" ) ) chars[x].disabled = str2num( script2 );
			else if( !strcmp( script1, "DISPZ" ) ) chars[x].dispz = str2num( script2 );
			break;
		case 'e':
		case 'E':
			if( !strcmp( script1, "EMOTE" ) )
			{
				i = str2num(script2);
				chars[x].emotecolor1 = (unsigned char)(i>>8);
				chars[x].emotecolor2 = (unsigned char)(i%256);
			}
			break;
		case 'f':
		case 'F':
			if( !strcmp( script1, "FAME" ) ) chars[x].fame=str2num(script2);
			else if( !strcmp( script1, "FIXEDLIGHT" ) ) chars[x].fixedlight = str2num( script2 ); 
			else if( !strcmp( script1, "FLEEAT" ) ) chars[x].fleeat = str2num( script2 );
			else if( !strcmp( script1, "FONT" ) ) chars[x].fonttype = str2num( script2 );
			else if( !strcmp( script1, "FX1" ) ) chars[x].fx1 = str2num( script2 );
			else if( !strcmp( script1, "FY1" ) ) chars[x].fy1 = str2num( script2 );
			else if( !strcmp( script1, "FZ1" ) ) chars[x].fz1 = str2num( script2 );
			else if( !strcmp( script1, "FX2" ) ) chars[x].fx2 = str2num( script2 );
			else if( !strcmp( script1, "FY2" ) ) chars[x].fy2 = str2num( script2 );
			break;
		case 'g':
		case 'G':
			// Begin of Guild related character stuff
			if( !strcmp( script1, "GUILDTOGGLE" ) ) chars[x].guildtoggle = str2num( script2 ); 
			else if( !strcmp( script1, "GUILDNUMBER" ) ) chars[x].guildnumber = str2num( script2 ); 
			else if( !strcmp( script1, "GUILDTITLE" ) ) safeCopy( chars[x].guildtitle, script2, MAX_GUILDTITLE ); 
			else if( !strcmp( script1, "GUILDFEALTY" ) ) chars[x].guildfealty = str2num( script2 ); 
			// End of guild stuff
			break;
		case 'h':
		case 'H':
			if( !strcmp( script1, "HIDAMAGE" ) ) chars[x].hidamage = str2num( script2 );
			else if( !strcmp( script1, "HIDDEN" ) ) chars[x].hidden = str2num( script2 );
			else if( !strcmp( script1, "HITPOINTS" ) ) chars[x].hp = str2num( script2 );
			else if( !strcmp( script1, "HUNGER" ) ) chars[x].hunger = str2num( script2 );
			break;
		case 'i':
		case 'I':
			if( !strcmp( script1, "INTELLIGENCE" ) ) chars[x].in = str2num( script2 );
			else if( !strcmp( script1, "INTELLIGENCE2" ) ) chars[x].in2 = str2num( script2 );
			break;
		case 'k':
		case 'K':
			if( !strcmp( script1, "KARMA" ) ) chars[x].karma = str2num( script2 );
			else if( !strcmp( script1, "KILLS" ) ) chars[x].kills = str2num( script2 );
			break;
			
		case 'l':
		case 'L':
			if( !strcmp( script1, "LASTON" ) ) 
			{
				safeCopy( chars[x].laston, script2, MAX_LASTON ); //load last time character was on
			}
			else if( !strcmp( script1, "LODAMAGE" ) ) chars[x].lodamage = str2num( script2 ); 
			break;
			
		case 'm':
		case 'M':
			if( !strcmp( script1, "MANA" ) ) chars[x].mn = str2num( script2 );
			else if( !strcmp( script1, "MURDERRATE" ) ) chars[x].murderrate = str2num( script2 );
			break;
			
		case 'n':
		case 'N':
			if( !strcmp( script1, "NAME" ) ) strcpy( chars[x].name, script2 );
			else if( !strcmp( script1, "NOTRAIN" ) ) chars[x].cantrain = 0;
			else if( !strcmp( script1, "NPC" ) ) chars[x].npc = str2num( script2 );
			else if( !strcmp( script1, "NPCAITYPE" ) ) chars[x].npcaitype = str2num( script2 );
			else if( !strcmp( script1, "NPCWANDER" ) ) chars[x].npcWander = str2num( script2 );
			break;
		case 'o':
		case 'O':
			if( !strcmp( script1, "OLDNPCWANDER" ) ) chars[x].oldnpcWander = str2num( script2 );
			else if( !strcmp( script1, "OLDX" ) ) chars[x].oldx = str2num( script2 );
			else if( !strcmp( script1, "OLDY" ) ) chars[x].oldy = str2num( script2 );
			else if( !strcmp( script1, "OLDZ" ) ) chars[x].oldz = str2num( script2 );
			else if( !strcmp( script1, "ORGBODY" ) )
			{
				i = str2num( script2 );
				chars[x].orgid1 = (unsigned char)(i>>8);
				chars[x].orgid2 = (unsigned char)(i%256);
				newpoly = 1;
			}
			else if( !strcmp( script1, "OWN" ) )
			{
				i = str2num( script2 );
				splitSerial(i, chars[x].own1, chars[x].own2, chars[x].own3, chars[x].own4);
				chars[x].ownserial = i;
				if( chars[x].ownserial != -1 ) 
				{
					setptr( &cownsp[i%HASHMAX], x ); //Load into charsp array
					chars[x].tamed = true;			// Abaddon
				}
			}
			break;
		case 'p':
		case 'P':
			if( !strcmp( script1, "PACKITEM" ) ) chars[x].packitem = str2num( script2 );
			else if( !strcmp( script1, "POISON" ) ) chars[x].poison = str2num( script2 );
			else if( !strcmp( script1, "POISONED" ) ) chars[x].poisoned = str2num( script2 );
			else if( !strcmp( script1, "PRIV" ) ) chars[x].priv = str2num( script2 );
			break;
		case 'q':
		case 'Q':
			if( !strcmp( script1,"QUESTTYPE" ) ) chars[x].questType = str2num( script2 );
			else if( !strcmp( script1,"QUESTDESTREGION" ) ) chars[x].questDestRegion = str2num( script2 );
			else if( !strcmp( script1,"QUESTORIGREGION" ) ) chars[x].questOrigRegion = str2num( script2 );
			break;						
		case 'r':
		case 'R':
			if( !strcmp( script1, "RACE" ) ) chars[x].race = str2num( script2 );
			else if( !strcmp( script1, "RACEGATE" ) ) chars[x].raceGate = str2num( script2 );
			else if( !strcmp( script1, "REATTACKAT" ) ) chars[x].reattackat = str2num( script2 );
			else if( !strcmp( script1, "RESERVED1" ) ) chars[x].cell = str2num( script2 );
			else if( chars[x].cell == 255 ) chars[x].cell = 0;
			// no cells have been marked as -1, but no cell=0
			// with that line its not necassairy to take it manually out. 
			else if( !strcmp( script1, "ROBE" ) ) chars[x].robe = str2num ( script2 );
			else if( !strcmp( script1, "RUNS" ) ) chars[x].runs = true;
			break;
		case 's':
		case 'S':
			if( !strcmp( script1, "SAY" ) )
			{
				i = str2num( script2 );
				chars[x].saycolor1 = (unsigned char)(i>>8);
				chars[x].saycolor2 = (unsigned char)(i%256);
			}
			else if( !strcmp( script1, "SERIAL" ) )
			{
				i = str2num( script2 );
				if( charcount2 <= i ) 
					charcount2 = i + 1;
				chars[x].ser1 = (unsigned char)(i>>24);
				chars[x].ser2 = (unsigned char)(i>>16);
				chars[x].ser3 = (unsigned char)(i>>8);
				chars[x].ser4 = (unsigned char)(i%256);
				chars[x].serial = i;
				setptr( &charsp[i%HASHMAX], x ); //Load into charsp array
			}
			else if( !strcmp( script1, "SHOPSPAWN" ) ) chars[x].shopSpawn = str2num( script2 );
			else if( !strncmp( script1, "SKILL", 5 ) )
			{
				chars[x].baseskill[ j = str2num( &script1[5] ) ] = str2num( script2 );
				Skills->updateSkillLevel( x, j );
			}
			else if( !strncmp( script1, "SKL", 3 ) )		// for skill locking
			{
				skl = str2num( &script1[3] );
				lockstate = str2num( script2 );
				if( lockstate > 2 )	//punt
					lockstate = 0;
				chars[x].lockState[skl] = lockstate;
			}
			else if( !strcmp( script1, "SKIN" ) )
			{
				i = str2num( script2 );
				chars[x].skin1 = (unsigned char)(i>>8);
				chars[x].skin2 = (unsigned char)(i%256);
			}
			else if( !strcmp( script1, "SPAWN" ) )
			{
				i = str2num( script2 );
				splitSerial(i, chars[x].spawn1, chars[x].spawn2, chars[x].spawn3, chars[x].spawn4);
				chars[x].spawnserial=i;
				if( chars[x].spawn1 < 0x40 ) //Not an item (region spawn) //New -- Zippy respawn area (in reality, it's < 0x40, all item serials START at 0x40
					spawnregion[chars[x].spawn3].current++;                                                       
				if( chars[x].spawnserial != 0 && chars[x].spawnserial != -1 )	// redefined to be 0, not -1	// legacy support until next build
					setptr( &cspawnsp[i%HASHMAX], x ); //Load into charsp array
				
			}
			else if( !strcmp( script1, "SPATTACK" ) ) chars[x].spattack = str2num( script2 ); 
			else if( !strcmp( script1, "SPADELAY" ) ) chars[x].spadelay = str2num( script2 ); 
			else if( !strcmp( script1, "SHOP" ) ) chars[x].shop = str2num( script2 ); 
			else if( !strcmp( script1, "SPEECH" ) ) chars[x].speech = str2num( script2 ); 
			else if( !strcmp( script1, "SPLIT" ) ) chars[x].split = str2num( script2 ); 
			else if( !strcmp( script1, "SPLITCHANCE" ) ) chars[x].splitchnc = str2num( script2 ); 
			else if( !strcmp( script1, "STAMINA" ) ) chars[x].stm = str2num( script2 ); 
			else if( !strcmp( script1, "STRENGTH" ) ) chars[x].st = str2num( script2 ); 
			else if( !strcmp( script1, "STRENGTH2" ) ) chars[x].st2 = str2num( script2 ); 
			else if( !strcmp( script1, "SUMMONTIMER" ) ) chars[x].summontimer = str2num( script2 ); 
			break;
		case 't':
		case 'T':
			if( !strcmp( script1, "TAMING" ) ) chars[x].taming = str2num( script2 ); 
			else if( !strcmp( script1, "TITLE" ) ) strcpy(chars[x].title, script2); 
			else if( !strcmp( script1, "TOWN" ) ) chars[x].town = str2num( script2 );
			else if( !strcmp( script1, "TOWNPRIV" ) ) chars[x].townpriv = str2num( script2 );
			else if( !strcmp( script1, "TOWNTITLE" ) ) chars[x].towntitle = str2num( script2 );
			else if( !strcmp( script1, "TOWNVOTE" ) )
			{
				i = str2num( script2 );
				chars[x].townvote1 = (unsigned char)(i>>24);
				chars[x].townvote2 = (unsigned char)(i>>16);
				chars[x].townvote3 = (unsigned char)(i>>8);
				chars[x].townvote4 = (unsigned char)(i%256);
			}
			else if( !strcmp( script1, "TRIGGER" ) ) chars[x].trigger = str2num( script2 );
			else if( !strcmp( script1, "TRIGWORD" ) ) strcpy( chars[x].trigword, script2 ); 
			break;
			
		case 'w':
		case 'W':
			if( !strcmp( script1, "WAR" ) ) chars[x].war = str2num( script2 ); 
			break;
		case 'x':
		case 'X':
			if( !strcmp( script1, "X" ) ) chars[x].x = str2num( script2 ); 
			else if( !strcmp( script1, "XBODY" ) )
			{
				i = str2num( script2 );
				chars[x].xid1 = (unsigned char)(i>>8);
				chars[x].xid2 = (unsigned char)(i%256);
			}
			else if( !strcmp( script1, "XSKIN" ) )
			{
				i = str2num( script2 );
				chars[x].xskin1 = (unsigned char)(i>>8);
				chars[x].xskin2 = (unsigned char)(i%256);
			}
			break;
		case 'y':
		case 'Y':
			if( !strcmp( script1, "Y" ) ) chars[x].y = str2num( script2 );
			break;
		case 'z':
		case 'Z':
			if( !strcmp( script1, "Z" ) ) chars[x].dispz=chars[x].z = str2num( script2 );
			break;
		}
		loops++;
	}
	while( strcmp( script1, "}" ) && loops <= 200 );
	if( chars[x].npc && chars[x].war ) 
		chars[x].war = 0;
 
	setcharflag( x );
 
	 if( chars[x].priv&0x80 && chars[x].commandLevel < 1 && chars[x].priv&0x01 != 0x01 ) // interim line to retain compatibility, MUST BE TAKEN out in the long term!
		 chars[x].commandLevel = 1;
	 if( chars[x].priv&0x01 && chars[x].commandLevel < 2 )	// interim line to retain compatibility, MUST BE TAKEN out in the long term!
		 chars[x].commandLevel = 2;
	 
	 ////////////////////////////////////////////////////////////////////
	 
	 chars[x].region=calcRegionFromXY(chars[x].x,chars[x].y); //LB bugfix
	 chars[x].antispamtimer = 0; // LB - AntiSpam -
	 // Restore original shape if char Polimorphed -  AntiChrist (9/99)
	 if( chars[x].id1 != chars[x].orgid1 || chars[x].id2!= chars[x].orgid2 )
	 {
		 chars[x].id1=chars[x].orgid1;
		 chars[x].id2=chars[x].orgid2;
	 }
	 
	 // lord binarys body/skin autocorrection code

	 k=(chars[x].id1<<8)+chars[x].id2;
	 if (k>=0x000 && k<=0x3e1) 
	 { 
	 } 
	 else  // client crashing body --> delete if non player esle put onl”x a warning on server screen
	 {	 // we dont want to delete that char, dont we ?
		 if (chars[x].account==-1) 
		 { 
			 ConOut("npc: %i[%s] with bugged body value detected, deleted for stability reasons\n",chars[x].serial,chars[x].name);
			 Npcs->DeleteChar(x); 
		 } 
		 else 
		 { 
			 chars[x].id1=0x01;
			 chars[x].id2=0x90;
		 }
	 }
	 
	 a = mapRegions->AddItem( x + 1000000 );
	 
	 if ((chars[x].x < 150 && chars[x].y < 150 && chars[x].account ==-1) || ((chars[x].x>6144 || chars[x].y>4096 || chars[x].x<0 || chars[x].y<0) && chars[x].account==-1)) 
	 {
		 if( chars[x].x != 7000 && chars[x].y != 7000 )		
			 Npcs->DeleteChar(x); //character in an invalid location
	 }
	 if ((chars[x].x < 100 && chars[x].y < 100 && chars[x].account !=-1) || ((chars[x].x>6144 || chars[x].y>4096 || chars[x].x<0 || chars[x].y<0) && chars[x].account!=-1))  
	 { 
		 mapRegions->RemoveItem( x + 1000000 );
		 chars[x].x = 1000; //player in an invalid location
		 chars[x].y = 1000;
		 chars[x].z = 0;
		 mapRegions->AddItem( x + 1000000 );
	 }
}

void CWorldMain::loaditem (ITEM x) // Load an item from WSC
{
	unsigned long int i;
	int loops=0;
	
	x = Items->MemItemFree();
	if( x == -1 ) 
		return;
	Items->InitItem( x, 0 );
	items[x].ser1 = 0x40;
	items[x].id1 = 0x0F;
	items[x].id2 = 0xA6;
	do
	{
		readw2();
		// krazyglue [10/10/99]
		// strcmp's can be pretty slow when you call a lot of them, and switch
		// is very fast check on the first byte to sort out the possibilities.
		switch( script1[0] )
		{
		case 'a':
		case 'A':
			if( !strcmp( script1, "AMOUNT" ) ) items[x].amount=str2num(script2);
			else if( !strcmp( script1, "ATT" ) ) items[x].att=str2num(script2); 
			else if( !strcmp( script1, "AC" ) ) items[x].armorClass = str2num( script2 ); 
			break;
			
		case 'c':
		case 'C':
			if( !strcmp( script1, "COLOR" ) )
			{
				i = str2num( script2 );
				items[x].color1 = (unsigned char)(i>>8);
				items[x].color2 = (unsigned char)(i%256);
			}
			else if( !strcmp( script1, "CONT" ) )
			{
				i = str2num( script2 );
				items[x].cont1 = (unsigned char)(i>>24);
				items[x].cont2 = (unsigned char)(i>>16);
				items[x].cont3 = (unsigned char)(i>>8);
				items[x].cont4 = (unsigned char)(i%256);
				items[x].contserial = i;
				if( i != -1 ) 
					setptr( &contsp[i%HASHMAX], x ); //set item in pointer array
			}
			else if( !strcmp( script1, "CORPSE" ) ) items[x].corpse = str2num( script2 );
			else if( !strcmp( script1, "CREATOR" ) ) strcpy( items[x].creator, script2 );
			break;
			
		case 'd':
		case 'D':
			if( !strcmp( script1, "DEF" ) ) items[x].def = str2num( script2 );
			else if( !strcmp( "DEX", script1 ) ) items[x].dx = str2num( script2 );
			else if( !strcmp( "DEXADD", script1 ) ) items[x].dx2 = str2num( script2 );
			else if( !strcmp( script1, "DIR" ) ) items[x].dir = str2num( script2 );
			else if( !strcmp( script1, "DISABLED" ) ) items[x].disabled = str2num( script2 );
			else if( !strcmp( script1, "DOORFLAG" ) ) items[x].doordir = str2num( script2 );
			else if( !strcmp( "DYE", script1 ) ) items[x].dye = str2num( script2 );
			else if( !strcmp( "DECAY", script1 ) ) items[x].priv |= 0x01;
			else if( !strcmp( "DISPELLABLE", script1 ) ) items[x].priv |= 0x04;
			else if( !strcmp( script1, "DX" ) ) items[x].dx = str2num( script2 );
			else if( !strcmp( script1, "DX2" ) ) items[x].dx2 = str2num( script2 );
			else if( !strcmp( script1, "DYEABLE" ) ) items[x].dye = str2num( script2 );
			else if( !strcmp( script1, "DESC" ) ) strcpy( items[x].desc, script2 );
			break;
			
		case 'g':
		case 'G':
			if( !strcmp( script1, "GLOW" ) ) items[x].glow = str2num( script2 );
			else if( !strcmp( script1, "GLOWBC" ) )
			{
				i = str2num( script2 );
				items[x].glow_c1 = (unsigned char)(i>>8);
				items[x].glow_c2 = (unsigned char)(i%256);
			}
			else if( !strcmp( script1, "GLOWTYPE" ) ) items[x].glow_effect = str2num( script2 );
			else if( !strcmp( script1, "GOOD" ) ) items[x].good = str2num( script2 );
			break;
			
		case 'h':
		case 'H':
			if( !strcmp( script1, "HIDAMAGE" ) ) items[x].hidamage = str2num( script2 );
			else if( !strcmp( script1, "HP" ) ) items[x].hp = str2num( script2 ); 
			break;
			
		case 'i':
		case 'I':
			if( !strcmp( script1, "ID" ) )
			{
				i = str2num( script2 );
				items[x].id1 = (unsigned char)(i>>8);
				items[x].id2 = (unsigned char)(i%256);
			}
			else if( !strcmp( script1, "IN" ) ) items[x].in = str2num( script2 );
			else if( !strcmp( script1, "IN2" ) ) items[x].in2 = str2num( script2 );
			else if( !strcmp( script1, "INT" ) ) items[x].in = str2num( script2 );
			else if( !strcmp( script1, "INTADD" ) ) items[x].in2 = str2num( script2 );
			else if( !strcmp("ITEMHAND", script1 ) ) items[x].itmhand = str2num( script2 );
			break;
			
		case 'l':
		case 'L':
			if( !strcmp( script1, "LAYER" ) ) items[x].layer = str2num( script2 );
			if( !strcmp( script1, "LODAMAGE" ) ) items[x].lodamage = str2num( script2 );
			break;
			
		case 'm':
		case 'M':
			if( !strcmp( script1, "MAXHP" ) ) items[x].maxhp = str2num( script2 );
			else if( !strcmp( script1, "MORE" ) )
			{
				i = str2num( script2 );
				items[x].more1 = (unsigned char)(i>>24);
				items[x].more2 = (unsigned char)(i>>16);
				items[x].more3 = (unsigned char)(i>>8);
				items[x].more4 = (unsigned char)(i%256);
			}
			else if( !strcmp( script1, "MORE2" ) )
			{
				i = str2num( script2 );
				items[x].moreb1 = (unsigned char)(i>>24);
				items[x].moreb2 = (unsigned char)(i>>16);
				items[x].moreb3 = (unsigned char)(i>>8);
				items[x].moreb4 = (unsigned char)(i%256);
			}
			else if( !strcmp( script1, "MOREX" ) ) items[x].morex = str2num( script2 );
			else if( !strcmp( script1, "MOREY" ) ) items[x].morey = str2num( script2 );
			else if( !strcmp( script1, "MOREZ" ) ) items[x].morez = str2num( script2 );
			else if( !strcmp( script1, "MOVABLE" ) ) items[x].magic = str2num( script2 );
			break;
			
		case 'n':
		case 'N':
			if( !strcmp( script1, "NAME" ) ) 
			{ 
				if( !strlen( script2 ) )
					strcpy( items[x].name, "#" );
				else
					strcpy( items[x].name, script2 );
			}
			else if( !strcmp( script1, "NAME2" ) ) 
			{ 
				if( !strlen( script2 ) )
					strcpy( items[x].name, "#" );
				else
					strcpy( items[x].name2, script2 ); 
			}
			break;
			
		case 'o':
		case 'O':
			if( !strcmp( script1, "OFFSPELL" ) ) items[x].offspell = str2num( script2 );
			else if( !strcmp( script1, "OWNER" ) )
			{
				i = str2num( script2 );
				items[x].owner1 = (unsigned char)(i>>24);
				items[x].owner2 = (unsigned char)(i>>16);
				items[x].owner3 = (unsigned char)(i>>8);
				items[x].owner4 = (unsigned char)(i%256);
				items[x].ownserial = i;
				setptr( &ownsp[i%HASHMAX], x ); //set item in pointer array
			}
			break;
			
		case 'p':
		case 'P':
			if( !strcmp( script1, "PILEABLE" ) ) items[x].pileable = str2num( script2 );
			else if( !strcmp( script1, "POISONED" ) ) items[x].poisoned = str2num( script2 );
			else if( !strcmp( script1, "PRIV" ) ) items[x].priv = str2num( script2 );
			break;
			
		case 'r':
		case 'R':
			if( !strcmp( script1, "RESTOCK" ) ) items[x].restock = str2num( script2 );
			else if( !strcmp( "RACE", script1 ) ) items[x].racialEffect = str2num( script2 );
			else if( !strcmp( script1, "RANK" ) ) 
			{
				items[x].rank = str2num( script2 ); // By Magius(CHE)
				if( items[x].rank <= 0 ) 
					items[x].rank = 10;
			}
			break;
			
		case 's':
		case 'S':
			if( !strcmp( script1, "SERIAL" ) )
			{
				i = str2num( script2 );
				if( itemcount2 <= i ) 
					itemcount2 = i + 1;
				splitSerial(i, items[x].ser1, items[x].ser2, items[x].ser3, items[x].ser4);
				items[x].serial = i; //Tauriel
				setptr( &itemsp[i%HASHMAX], x ); //set item in pointer array
			}
			else if( !strcmp( script1, "SPAWN" ) )
			{
				i = str2num( script2 );
				splitSerial(i, items[x].spawn1, items[x].spawn2, items[x].spawn3, items[x].spawn4);
				items[x].spawnserial = i;
				setptr( &spawnsp[i%HASHMAX], x ); //set item in pointer array
			}
			else if( !strcmp( script1, "SPD" ) ) items[x].spd = str2num( script2 );
			else if( !strcmp( script1, "ST" ) ) items[x].st = str2num( script2 );
			else if( !strcmp( script1, "ST2" ) ) items[x].st2 = str2num( script2 );
			else if( !strcmp( script1, "SK_MADE" ) ) items[x].madewith = str2num( script2 );
			else if( !strcmp( script1, "STR" ) ) items[x].st = str2num( script2 );
			else if( !strcmp( script1, "STRADD" ) ) items[x].st2 = str2num( script2 );
			break;
			
		case 't':
		case 'T':
			if( !strcmp( script1, "TRIGGER" ) ) items[x].trigger = str2num( script2 );
			else if( !strcmp( script1, "TRIGTYPE" ) ) items[x].trigtype = str2num( script2 );
			else if( !strcmp( script1, "TYPE" ) ) items[x].type = str2num( script2 );
			else if( !strcmp( script1, "TYPE2" ) ) items[x].type2 = str2num( script2 );
			break;
			
		case 'u':
		case 'U':
			if( !strcmp( script1, "USES" ) ) items[x].tuses = str2num( script2 );
			break;
			
		case 'v':
		case 'V':
			if( !strcmp( script1, "VISIBLE" ) ) items[x].visible = str2num( script2 );
			else if( !strcmp( script1, "VALUE" ) ) items[x].value = str2num( script2 );
			break;
			
		case 'w':
		case 'W':
			if( !strcmp( script1, "WEIGHT" ) ) 
			{
				i = str2num( script2 );			
				items[x].weight = i;
			} 
			else 
				items[x].weight = 0;
			
			if( !strcmp( script1, "WIPE" ) ) items[x].wipe = str2num( script2 );
			break;
			
		case 'x':
		case 'X':
			if( !strcmp( script1, "X" ) ) items[x].x = str2num( script2 );
			break;
			
		case 'y':
		case 'Y':
			if( !strcmp( script1, "Y" ) ) items[x].y = str2num( script2 );
			break;
		case 'z':
		case 'Z':
			if( !strcmp( script1, "Z" ) ) items[x].z = str2num( script2 );
			break;
		}
		
		loops++;
	}
	while( strcmp( script1, "}" ) && loops <= 200 );
 
	StoreItemRandomValue( x, -1 ); // Magius(CHE) (2)
 
	if( items[x].layer == 0x19 )   // It's a mount
	{
		CHARACTER tMounted = calcCharFromSer( items[x].contserial );
		if ( tMounted != -1 )
			chars[tMounted].onhorse = true;
	}
	//add item weight if item doesn't have it yet
	if (items[x].weight<=0) // LB, changed from 29 to 0
	{
		items[x].weight=0;
	}

	if( items[x].maxhp == 0 ) 
		items[x].maxhp = items[x].hp;
    // Tauriel adding region pointers
 
	if (items[x].contserial==-1)
	{ 
		da = mapRegions->AddItem(x);	// it returns 1 if inalid, if invalid it DOESNT get added !!!
		if( items[x].x < 0 || items[x].y < 0 || items[x].x > 6144 || items[x].y > 4096 )
		{  
			Items->DeleItem( x );	//these are invalid locations, delete them!	    
		}
	}          
}

void CWorldMain::LoadWorld ( void )
{
	char outper[4] = {0,}; // Magius(CHE) (1)
	unsigned int i=0;
	unsigned int percent = 0, a = 0, pred = 0, maxm = 0; // Magius(CHE) (1)
	cmem=0;
	imem=0;
	ConOut("Loading World, Building map Regions, checking Item weight...\n");
	charcount=0;
	itemcount=0;
	charcount2=1;
	itemcount2=0x40000000;
	
	wscfile=fopen("chars.wsc","r");
	if (wscfile==NULL)
	{
		ConOut("WARNING: Chars.wsc not found. Defaulting to uox3.wsc\n");
		cmem=100;
		
		ConOut(" Allocating inital dynamic Character memory of %i... ",cmem);
		
		chars.Reserve( cmem );
		
		if(( clickx = new int[cmem] ) == NULL ) Shutdown( FATAL_UOX3_CLICKX );
		if(( clicky = new int[cmem] ) == NULL ) Shutdown( FATAL_UOX3_CLICKY );
		if(( currentSpellType = new int[cmem] )== NULL ) Shutdown( FATAL_UOX3_CURRENTSPELLTYPE );
		if(( targetok = new char[cmem] ) == NULL ) Shutdown( FATAL_UOX3_TARGETOK );   // shouldn't it be sizeof( char )
		
		ConOut("Done\n");
	} 
	else 
	{
		//Get number for inital character memory needed ->
		readw3();
		if (!(strcmp(script1, "INITMEM"))) cmem=str2num(script2);
		maxm = cmem; // Magius(CHE) (1)
		if (cmem<100) cmem=100;
		
		ConOut("Allocating inital dynamic Character memory of %i... ",cmem);
		
		chars.Reserve( cmem );
		
		if(( clickx = new int[cmem] ) == NULL ) Shutdown( FATAL_UOX3_CLICKX );
		if(( clicky = new int[cmem] ) == NULL ) Shutdown( FATAL_UOX3_CLICKY );
		if(( currentSpellType = new int[cmem] ) == NULL ) Shutdown( FATAL_UOX3_CURRENTSPELLTYPE );
		if(( targetok = new char[cmem] ) == NULL ) Shutdown( FATAL_UOX3_TARGETOK );   // shouldn't it be sizeof( char )
		ConOut("Done\n");

		// check if the map-mode (ilshenar or britannia) and wordfiles match , LB
		
		bool ok;
		bool ilsh = MapTileHeight<300;
		bool s=false;

		readw3();

		if (!(strcmp(script1, "ILSHENAR"))) ok = ilsh; 	       // worldfiles with ilsh tag runnning on an ilsh shard -> ok					
		else if (!(strcmp(script1, "BRITANNIA"))) ok = !ilsh;  // worldfiles with brit tag running on a brit shard -> ok		
		else ok = !ilsh;                                       // old britannian worldfiles without map-type tag running on an ilsh shard -> not good
	                                                           // old britannian worldfiles without map-type tag running on a brit sahrd -> ok
		                                                        
        if (!(strcmp(script1, "SECTION"))) s=true;             // old wordfiles that dont have the ilsh or brit tag might have a section tag next, prevents skipping of the first one in that case

		if (ilsh && !ok) 
		{ 		
			ConOut("You are runnning an ILSHENAR map shard, you tryed to load BRITANNIA map shard wordfiles\n");
			exit(-1);
		}

		if (!ilsh && !ok)
		{		
			ConOut("You are runnning a BRITANNIA map shard, you tryed to load ILSHENAR map shard wordfiles\n");
			exit(-1);
		}






		
		ConOut("  Loading characters ");
		do
		{
			readw3();
			if (!(strcmp(script1, "SECTION")))
			{
				if (!(strcmp(script2, "CHARACTER")))
				{
					loadchar(str2num(script3));
					a++;
					pred = percent; // Magius(CHE) (1)
					if (maxm <= 1)
						percent = 100;
					else
						percent = (int)(a*100)/(maxm-1); // Magius(CHE) (1)
					if( strlen( outper ) > 0 ) { // Magius(CHE) (1)
						for( i = 1; i <= strlen( outper ) + 1; i++ ) ConOut("\b" );
						outper[0] = 0;
					}
				}
			}
			if (percent> pred ) // Changed by MAgius(CHE) (1)
			{
				numtostr( percent, outper );
				ConOut("%s%%", outper );
				pred = percent;
				// a=0; MAgius(CHE) (1)
			}
		} while (strcmp(script1,"EOF") && !feof(wscfile));
		fclose(wscfile);
		wscfile = NULL;
		if( strlen( outper ) > 0 ) { // Magius(CHE) (1)
			for( i = 1; i <= strlen( outper ) + 1; i++ )
				ConOut( "\b" );
			outper[0] = 0;
		}
		ConOut("Done.\n");
		wscfile=fopen("items.wsc", "r");
		if (wscfile==NULL)
		{
			ConOut("ERROR: Items.wsc not found. No items will be loaded.\n");
			imem=100;
			
			// 10th October, 1999 removed * sizeof( int ) by Abaddon
			ConOut("Allocating inital dynamic Item memory of %i... ",imem);
			
			items.Reserve( imem );
			
			if(( loscache = new int[imem] ) == NULL ) Shutdown( FATAL_UOX3_LOSCACHE );
			if(( itemids = new int[imem] ) == NULL ) Shutdown( FATAL_UOX3_ITEMIDS );
			ConOut("Done\n");
		} 
		else 
		{
			//Get number for inital character memory needed ->
			readw3();
			if (!(strcmp(script1, "INITMEM"))) imem=str2num(script2);
			maxm = imem; 
			if (imem<100) imem=100;
			
			ConOut("Allocating inital dynamic Item memory of %i... ",imem);
			
			items.Reserve( imem );
			
			if(( loscache = new int[imem] ) == NULL ) Shutdown( FATAL_UOX3_LOSCACHE );
			if(( itemids = new int[imem] ) == NULL ) Shutdown( FATAL_UOX3_ITEMIDS );
			ConOut("Done\n");
			
			a = 0; // Magius(CHE) (2)
			ConOut("  Loading items ");
			do {
				readw3();
				if (!(strcmp(script1, "SECTION")))
				{
					if (!(strcmp(script2, "WORLDITEM"))) 
					{
						loaditem(str2num(script3));
					}
					if (!(strcmp(script2, "GUILD")))
						Guilds->Read(str2num(script3));
					a++;
					pred = percent; // Magius(CHE) (1)
					if (maxm <=1 )
						percent = 100;
					else
						percent = (int)(a*100)/(maxm - 1); // Magius(CHE) (1)
					if( strlen( outper ) > 0 ) { // Magius(CHE) (1)
						for( i = 1; i <= strlen( outper ) + 1; i++ ) 
							ConOut("\b" );
						outper[0] = 0;
					}
				}
				if( percent > pred ) // Changed by MAgius(CHE) (1)
				{
					numtostr( percent, outper );
					ConOut("%s%%", outper );
					pred = percent;
				}
				
			} while (strcmp(script1, "EOF") && !feof(wscfile));
			fclose(wscfile);
			wscfile = NULL;
		}
		if( strlen( outper ) > 0 ) { // Magius(CHE) (1)
			for( i = 1; i <= strlen( outper ) + 1; i++ ) 
				ConOut( "\b" );
			outper[0] = 0;
		}
		ConOut("Done.\n" ); // Magius(CHE)
		ConOut("World Loaded.\n");
		return;
	}
	imem=100;
	// 10th October, 1999 removed * sizeof( int ) by Abaddon
	ConOut("Allocating inital dynamic Item memory of %i... ",imem);
	
	items.Reserve( imem );
	
	if(( loscache = new int[imem] ) == NULL ) Shutdown( FATAL_UOX3_LOSCACHE );
	if(( itemids = new int[imem] ) == NULL ) Shutdown( FATAL_UOX3_ITEMIDS );
	ConOut("Done\n");
	
	wscfile=fopen("uox3.wsc", "r");
	if(wscfile==NULL) 
		ConOut("ERROR: World data not found, using blank world instead.\n");
	else {
		do
		{
			readw3();
			if (!(strcmp(script1, "SECTION")))
			{
				if (!(strcmp(script2, "CHARACTER")))
					loadchar(str2num(script3));
				if (!(strcmp(script2, "WORLDITEM")))
					loaditem(str2num(script3));
				if (!(strcmp(script2, "GUILD")))
					Guilds->Read(str2num(script3));
			}
		}
		while (strcmp(script1, "EOF") && !feof(wscfile));

		ConOut("Saving world in new format.");
		do {
			cwmWorldState->savenewworld(1);
		} while ( cwmWorldState->Saving() );
		ConOut(" Done.\n");

		fclose(wscfile);
		wscfile = NULL;
		ConOut("Done.\n");
	}
}
