//------------------------------------------------------------------------
//  speech.cpp
//
//------------------------------------------------------------------------
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 2001 - 2001 by Marcus Rating (Cironian)
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
#include "uox3.h"

void npctalk(int s, int npc, char *txt, char antispam) // NPC speech
{
	int tl;
	char machwas;
	
	if (npc==-1 || s==-1) return; //lb
	
	if( antispam )
	{
		if( chars[npc].antispamtimer < uiCurrentTime )
		{
			chars[npc].antispamtimer = uiCurrentTime + CLOCKS_PER_SEC*10;
			machwas = 1;
		} 
		else
			machwas = 0;
	}
	else
		machwas = 1;
	
	if( machwas )
	{
		
		
		tl=44+strlen(txt)+1;
		talk[1]=tl>>8;
		talk[2]=tl%256;
		talk[3]=chars[npc].ser1;
		talk[4]=chars[npc].ser2;
		talk[5]=chars[npc].ser3;
		talk[6]=chars[npc].ser4;
		talk[7]=chars[npc].id1;
		talk[8]=chars[npc].id2;
		talk[9]=0; // Type
		talk[10]=chars[npc].saycolor1=0x00;
		talk[11]=chars[npc].saycolor2=0x5b;
		talk[12]=0;
		talk[13]=chars[currchar[s]].fonttype;
		if( chars[npc].npcaitype == 0x02 ) // bad npcs speech (red)..Ripper
		{
			talk[10] = 0x00;
			talk[11] = 0x26;
		}
		Network->xSend(s, talk, 14, 0);
		Network->xSend(s, chars[npc].name, 30, 0);
		Network->xSend(s, txt, strlen(txt)+1, 0);
	}
}

void npctalkall(int npc, char *txt, char antispam ) // NPC speech to all in range.
{
	
	if (npc==-1) return;
	
	int i;
	
	for (i=0;i<now;i++)
		if (inrange1p(npc, currchar[i])&&perm[i])
			npctalk(i, npc, txt, antispam );
}

void npcemote(int s, int npc, char *txt, char antispam ) // NPC speech
{
	int tl;
	char machwas;
	
	if( s == -1 || npc == -1 ) 
		return;
	
	if( antispam )
	{
		if( chars[npc].antispamtimer < uiCurrentTime )
		{
			chars[npc].antispamtimer = uiCurrentTime + CLOCKS_PER_SEC*10;
			machwas = 1;
		}
		else
			machwas = 0;
	}
	else
		machwas = 1;
	
	if( machwas )
	{
		tl = 44 + strlen(txt)+1;
		talk[1] = tl>>8;
		talk[2] = tl%256;
		talk[3] = chars[npc].ser1;
		talk[4] = chars[npc].ser2;
		talk[5] = chars[npc].ser3;
		talk[6] = chars[npc].ser4;
		talk[7] = chars[npc].id1;
		talk[8] = chars[npc].id2;
		talk[9] = 2; // Type
		talk[10] = chars[npc].emotecolor1;
		talk[11] = chars[npc].emotecolor2;
//		talk[10]=chars[npc].emotecolor1=0x00;
//		talk[11]=chars[npc].emotecolor2=0x26;
		talk[12] = 0;
		talk[13] = chars[currchar[s]].fonttype;
		Network->xSend(s, talk, 14, 0);
		Network->xSend(s, chars[npc].name, 30, 0);
		Network->xSend(s, txt, strlen(txt)+1, 0);
	}
}

void npcemoteall(int npc, char *txt, char antispam ) // NPC speech to all in range.
{
	int i;
	
	if (npc==-1) return;
	
	for (i=0;i<now;i++)
		if (inrange1p(npc, currchar[i])&&perm[i])
			npcemote(i, npc, txt, antispam );
}
