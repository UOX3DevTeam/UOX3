//------------------------------------------------------------------------
//  trigger.cpp
//
//------------------------------------------------------------------------
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 1997 - 2001 by unknown real name (Zadius)
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
// *******************************************************************
// trigger.cpp (Trigger processing module)
//
// The trigger is activated here.  When the users doubleclicks on a triggered
// item or a triggered NPC, this routine will load the trigger from the
// triggers.scp (for dynamic), ntrigrs.scp (for NPCs) file or wtrigrs.scp
// (static world objects) and process the tokens for the trigger.
// NOTE: By using this source you are agreeing to all the terms of the GNU
// agreement.  Any use of this code, sniplets of this code, concepts of
// this code or ideas derived from this code requires that any program(s)
// and source containing any of the above mentioned states of the code, must
// be released in its entirety.
//
// DO NOT MODIFY THIS CODE UNLESS you are sure you know what your doing.
// The trigger system is not designed for making tokens that do just one
// thing.  Remember, if it can't be used for multiple things, it shouldnt
// be here. Do not distribute your changes unless its either by consent of
// the author(Zadius) or if the change and/or addition is by popular
// demand and it has been thouroughly tested.
// 
// Zadius 11-4-98 (linkage@xmission.com)
// *******************************************************************


#include <errno.h>
#include <uox3.h>

//This routine processes both Static and Dynamic items
void triggerwitem(int ts, int ti, int ttype)
{
	char sect[512];
	char buff[512];
	char effect[29];
	char fmsg[512];
	char cmsg[512]; //completed trigger message
	char clr1;
	char clr2;
	int serial,serhash=-1,ci;
	unsigned int i,uiTempi, uiCompleted=0;
	int tl;
	int p,j;
	int c;
	int r;
	int itemnum=-1;
	int needitem=-1;
	int npcnum=-1;
	int trig=0;
	int x1=0,y1=0,x2=0,y2=0,z2=0,dx,dy;
	long int pos;
	// Addons by Magius(CHE)
	int evti = -1;
	tile_st tile;
	char tempname[512], tempname2[512], tempname3[512], tempstr[512];
	tempname[0] = 0;
	tempname2[0] = 0;
	tempname3[0] = 0;
	tempstr[0] = 0;
	cmsg[0] = 0;
	// end declaration for Magius
	
	fmsg[0] = 0;
	
	if( ttype == 1 )
	{
		if (chars[currchar[ts]].targtrig)
			trig=chars[currchar[ts]].targtrig;
		else
			trig=items[ti].trigger;
		chars[currchar[ts]].targtrig=0;
		openscript("triggers.scp");
		sprintf(sect, "TRG %i", trig);
		if (!i_scripts[trigger_script]->find(sect))
		{
			closescript();
			return;
		}
	}
	else
	{
    /*if ((buffer[ts][7]!=0)&&(buffer[ts][8]!=0)&&(buffer[ts][9]!=0)&&(buffer[ts][10]!=0))
    {
	return;
    }*/
		openscript("wtrigrs.scp");
		sprintf(buff,"x%x%x",buffer[ts][0x11],buffer[ts][0x12]);
		if (!i_scripts[wtrigger_script]->isin(buff))
		{
			sprintf(sect,"You cant think of a way to use that.");
			sysmessage(ts,sect);
			closescript();
			return;
		}
	}
	// Get Temporany Name of the Item (Magius(CHE))
	if( ti > -1 ) // crashfix LB
	{
		if( items[ti].name[0] != '#' ) 
			strcpy( tempname, items[ti].name );
		else		
		{
			Map->SeekTile( ( items[ti].id1<<8 ) + items[ti].id2, &tile );
			strcpy( tempname, (char *)tile.name );
		}
		if( items[ti].disabled > uiCurrentTime )
		{
			strcpy( temp, "That doesn't seem to work right now." );
			sysmessage( ts, temp );
			closescript();
			return;
		}
		// End addons by magius
	}
	do
	{
		read2();
		if (script1[0]!='}')
		{
			// Start Tauriel's new trigger Tokens
			if (!(strcmp("END_TRIGGER",script1)))  //End execution of a trigger
			{
				uiTempi=str2num(script2);
				if ((uiTempi && uiCompleted) || (!uiTempi && !uiCompleted))
				{
					closescript();
					return;
				}
			}
			// End Tauriel's new trigger token
			if (!(strcmp("RANGE",script1)))  //Player in in range
			{
				if( ti > -1 )
				{
					p = currchar[ts];
					r = packitem(currchar[ts]);
					if( r != -1 )
						if( items[ti].contserial != items[r].serial )
						{
							if( itemdist( currchar[ts], ti ) > str2num( script2 ) )
							{
								strcpy(sect,"You are not close enough to use that.");
								sysmessage(ts,sect);
								closescript();
								return;
							}
						}
				} 
				else 
				{
					x2 = (buffer[ts][11]<<8)+buffer[ts][12];
					y2 = (buffer[ts][13]<<8)+buffer[ts][14];
					z2 = (buffer[ts][15]<<8)+buffer[ts][16];
					dx = abs( x1 - x2 );
					dy = abs( y1 - y2 );
					if( str2num( script2 ) < (int)hypot( dx, dy ) )
					{
						strcpy(sect,"You are not close enough to use that.");
						sysmessage(ts,sect);
						closescript();
						return;
					}
				}
			}
			if (!(strcmp("EMT",script1)))  //Player says something when trigger is activated
			{
				strcpy( sect, script2 );
				for( i = 0; i < now; i++ )
				{
					if( perm[i] && inrange1p( currchar[ts], currchar[i] ) )
					{
						tl=44+strlen(sect)+1;
						talk[1] = (unsigned char)tl>>8;
						talk[2] = (unsigned char)tl%256;
						talk[3]=chars[currchar[ts]].ser1;
						talk[4]=chars[currchar[ts]].ser2;
						talk[5]=chars[currchar[ts]].ser3;
						talk[6]=chars[currchar[ts]].ser4;
						talk[7]=chars[currchar[ts]].id1;
						talk[8]=chars[currchar[ts]].id2;
						talk[9]=0; // Type
						talk[10]=chars[currchar[ts]].saycolor1;
						talk[11]=chars[currchar[ts]].saycolor2;
						talk[12]=0;
						talk[13]=chars[currchar[ts]].fonttype;
						Network->xSend(i, talk, 14, 0);
						Network->xSend(i, chars[currchar[ts]].name, 30, 0);
						Network->xSend(i, sect, strlen(sect)+1, 0);
					}
				}
			}
			if (!(strcmp("MSG",script1)))  //Display a message when trigger is activated
			{
				strcpy(sect, script2);
				sysmessage(ts,sect);
			}
			// Start Tauriel's new trigger Tokens
			if (!(strcmp("RANDOM_ITEM",script1)))  //%chance to IADD item
			{
				uiCompleted=0;
				cline = &script2[0];
				splitline();
				unsigned int uiItemNum=makenumber(0);
				unsigned int uiChance=makenumber(1);
				unsigned int InBackpack = makenumber( 2 );
				
				if (uiChance >= ((rand()%100)+1))
				{
					strcpy(script1,"IADD");
					sprintf(script2,"%i %i",uiItemNum, InBackpack);
					uiCompleted = 1;
				}
			}
			if (!(strcmp("RANDOM_NPC",script1)))  //%chance to NADD item
			{
				uiCompleted=0;
				cline = &script2[0];
				splitline();
				unsigned int uiItemNum=makenumber(0);
				unsigned int uiChance=makenumber(1);
				
				if (uiChance >= ((rand()%100)+1))
				{
					strcpy(script1,"NADD");
					sprintf(script2,"%i",uiItemNum);
					uiCompleted = 1;
				}
			}
			if (!(strcmp("CMSG",script1)))  //Set Token Completed message
			{
				if (uiCompleted)
				{
					strcpy(cmsg, script2);
					if (strlen(cmsg)) sysmessage(ts,cmsg);
				}
			}
			//End Tauriel's new trigger Tokens
			
			if (!(strcmp("FMSG",script1)))  //Set fail message
			{
				strcpy(fmsg, script2);
			}
			if (!(strcmp("SND",script1)))  //Play a sound when trigger is activated
			{
				cline = &script2[0];
				splitline();
				soundeffects(ts, hexnumber(0), hexnumber(1), true);
			}
			if (!(strcmp("REMOVE",script1)))  //Remove item after triggered
			{
				if (ti>-1)
				{
					if (items[ti].amount>1)
					{
						items[ti].amount--;
					} else {
						Items->DeleItem(ti);
					}
				}
			}
			if (!(strcmp("SETTRG",script1)))  //Set items trigger to new trigger
			{
				if (ti>-1) items[ti].trigger=str2num(script2);
			}
			if (!(strcmp("SETID",script1)))  //Set items id to new id
			{
				if (ti>-1)
				{
					cline = &script2[0];
					splitline();
					items[ti].id1=hexnumber(0);
					items[ti].id2=hexnumber(1);
					RefreshItem( ti ); // AntiChrist
					itemnum=ti;
				}
			}
			if (!(strcmp("COLOR",script1)))  //Set the color check
			{
				cline = &script2[0];
				splitline();
				clr1=hexnumber(0);
				clr2=hexnumber(1);
			}
			if (!(strcmp("SETEVID",script1)))  //Set envoked items id to new id
			{
				if (chars[currchar[ts]].envokeitem>-1)
				{
					cline = &script2[0];
					splitline();
					items[chars[currchar[ts]].envokeitem].id1=hexnumber(0);
					items[chars[currchar[ts]].envokeitem].id2=hexnumber(1);
					RefreshItem( chars[currchar[ts]].envokeitem ); // AntiChrist
					itemnum=chars[currchar[ts]].envokeitem;
				}
			}
			if (!(strcmp("IFOWNER",script1)))  //If item is owned by triggerer
			{
				if (ti>-1)
				{
					p=currchar[ts];
					if (items[ti].ownserial!=chars[p].serial)
					{
						sysmessage(ts,"You do not own that.");
						closescript();
						return;
					}
				}
			}
			if (!(strcmp("IFSKL",script1)))  //If skill meets a certain criteria
			{
				cline = &script2[0];
				splitline();
				p=currchar[ts];
				i=makenumber(0);
				j=makenumber(1);
				if (j>=0)
				{
					if (!(chars[p].baseskill[i]>=j))
					{
						if (strlen(fmsg)) sysmessage(ts,fmsg);
						closescript();
						return;
					}
				} else {
					if (!(chars[p].baseskill[i]<=abs(j)))
					{
						if (strlen(fmsg)) sysmessage(ts,fmsg);
						closescript();
						return;
					}
				}
			}
			if (!(strcmp("EVDUR",script1)))  //Math on Evoked item HP ---- Magius(CHE)
			{
				j=str2num(script2);
				c=items[evti].hp;
				if (evti>-1) {
					if ((c>=items[evti].maxhp) && (j>0)) {
						sprintf(tempstr,"Your %s is already totally repaired!",tempname2);
						sysmessage(ts,tempstr);
					}
					items[evti].hp+=j;
					if (items[evti].hp>=items[evti].maxhp) items[evti].hp=items[evti].maxhp;
					if (items[evti].hp-c>0) {
						if (strlen(cmsg)) sysmessage(ts,cmsg);
						else {
							sprintf(tempstr,"Your %s is now totally repaired!",tempname2);
							sysmessage(ts,tempstr);
						}
					} else if (items[evti].hp-c<0) {
						if (strlen(fmsg)) sysmessage(ts,fmsg);
						else {
							sprintf(tempstr,"Your %s appears to be more ruined than before!",tempname2);
							sysmessage(ts,tempstr);
						}
					}
					if (items[evti].hp<=0) {
						sprintf(tempstr,"Your %s was too old and it has been destroyed!",tempname2);
						sysmessage(ts,tempstr);
						if (items[evti].amount>1) items[evti].amount--;
						else Items->DeleItem(evti);
					}
				}
			}
			if (!(strcmp("EVMAXDUR",script1)))  //Math on Evoked item MAXHP ---- Magius(CHE)
			{
				if (evti>-1) { 
					items[evti].maxhp+=str2num(script2);
					if (items[evti].hp>=items[evti].maxhp) items[evti].hp=items[evti].maxhp;
					if (str2num(script2)>=0) {
						if (strlen(cmsg)) sysmessage(ts,cmsg);
						else {
							sprintf(tempstr,"You increased the maximum durability of your %s !",tempname2);
							sysmessage(ts,tempstr);
						}
					} else {
						if (strlen(fmsg)) sysmessage(ts,fmsg);
						else {
							sprintf(tempstr,"Your %s appears to be not resistant as before!",tempname2);
							sysmessage(ts,tempstr);
						}
					}
					if (items[evti].maxhp<=0) {
						sprintf(tempstr,"Your %s was too old and it has been destroyed!",tempname2);
						sysmessage(ts,tempstr);
						if (items[evti].amount>1) items[evti].amount--;
						else Items->DeleItem(evti);
					}
				}
			}
			if (!(strcmp("DUR",script1)))  //Math on item HP ---- Magius(CHE)
			{
				j=str2num(script2);
				c=items[ti].hp;
				if (ti>-1) {
					if ((c>=items[ti].maxhp) && (j>0)) {
						sprintf(tempstr,"Your %s is already totally repaired!",tempname);
						sysmessage(ts,tempstr);
					}
					items[ti].hp+=j;
					if (items[ti].hp>=items[ti].maxhp) items[ti].hp=items[ti].maxhp;
					if (items[ti].hp-c>0) {
						if (strlen(cmsg)) sysmessage(ts,cmsg);
						else {
							sprintf(tempstr,"Your %s is now totally repaired!",tempname);
							sysmessage(ts,tempstr);
						}
					} else if (items[ti].hp-c<0) {
						if (strlen(fmsg)) sysmessage(ts,fmsg);
						else {
							sprintf(tempstr,"Your %s appears to be more ruined than before!",tempname);
							sysmessage(ts,tempstr);
						}
					}
					if (items[ti].hp<=0) {
						sprintf(tempstr,"Your %s was too old and it has been destroyed!",tempname);
						sysmessage(ts,tempstr);
						if (items[ti].amount>1) items[ti].amount--;
						else Items->DeleItem(ti);
					}
				}
			}
			if (!(strcmp("MAXDUR",script1)))  //Math on item MAXHP ---- Magius(CHE)
			{
				if (ti>-1) { 
					items[ti].maxhp+=str2num(script2);
					if (items[ti].hp>=items[ti].maxhp) items[ti].hp=items[ti].maxhp;
					if (str2num(script2)>=0) {
						if (strlen(cmsg)) sysmessage(ts,cmsg);
						else {
							sprintf(tempstr,"You increased the maximum durability of your %s !",tempname);
							sysmessage(ts,tempstr);
						}
					} else {
						if (strlen(fmsg)) sysmessage(ts,fmsg);
						else {
							sprintf(tempstr,"Your %s appears to be more ruined than before!",tempname);
							sysmessage(ts,tempstr);
						}
					}
					if (items[ti].maxhp<=0) {
						sprintf(tempstr,"Your %s was too old and it has been destoryed!",tempname);
						sysmessage(ts,tempstr);
						if (items[ti].amount>1) items[ti].amount--;
						else Items->DeleItem(ti);
					}
				}
			}
			if (!(strcmp("IFHUNGER",script1)))  //If hunger meets a certain criteria - Magius(CHE)
			{
				j=str2num(script2);
				p=currchar[ts];
				if (j>=0)
				{
					if (!(chars[p].hunger>=j))
					{
						if (strlen(fmsg)) sysmessage(ts,fmsg);
						closescript();
						return;
					}
				} else {
					if (!(chars[p].hunger<=abs(j)))
					{
						if (strlen(fmsg)) sysmessage(ts,fmsg);
						closescript();
						return;
					}
				}
			}
			if (!(strcmp("HUNGER",script1)))  //Do math on players hunger from 0 to 6 - Magius(CHE)
			{
				j=str2num(script2);
				chars[currchar[ts]].hunger += j;
				if (chars[currchar[ts]].hunger>6) chars[currchar[ts]].hunger=6;
				if (chars[currchar[ts]].hunger<1) chars[currchar[ts]].hunger=1;
				switch(chars[currchar[ts]].hunger)
				{
				case 0:  sysmessage(ts, "You eat the food, but are still extremely hungry.");
					break;
				case 1:  sysmessage(ts, "You eat the food, but are still extremely hungry.");
					break;
				case 2:  sysmessage(ts, "After eating the food, you feel much less hungry.");
					break;
				case 3:  sysmessage(ts, "You eat the food, and begin to feel more satiated.");
					break;
				case 4:  sysmessage(ts, "You feel quite full after consuming the food.");
					break;
				case 5:  sysmessage(ts, "You are nearly stuffed, but manage to eat the food.");
					break;
				case 6:  sysmessage(ts, "You are simply too full to eat any more!");
					break;
				}
			}
			if (!(strcmp("IFKARMA",script1)))  //If karma meets a certain criteria - Magius(CHE)
			{
				char opt;
				p=currchar[ts];
				gettokennum(script2, 0);
				opt=gettokenstr[0];
				gettokennum(script2, 1);
				j=str2num(gettokenstr);
				//printf("Script2: %s\nOPT: %c\n%i %c %i\n",script2,opt,chars[p].karma,opt,j);
				if (opt=='>')
				{
					if (!(chars[p].karma>=j))
					{
						if (strlen(fmsg)) sysmessage(ts,fmsg);
						else sysmessage(ts,"You need more Karma to do that!");
						closescript();
						return;
					}
				} else if (opt=='<') {
					if (!(chars[p].karma<=j))
					{
						if (strlen(fmsg)) sysmessage(ts,fmsg);
						else sysmessage(ts,"You need less Karma to do that!");
						closescript();
						return;
					}
				} else ConOut("TRIGGER.SCP:Triggerwitem() Syntax Error in IFKARMA token.\n   IFKARMA <opt> <value>\n    <opt>= '<' or '>'\n    <value> range from '-10000' to '10000'.\n");
			}
			if (!(strcmp("KARMA",script1)))  //Do math on the players karma - Magius(CHE)
			{
				cline = &script2[0];
				splitline();
				j=makenumber(0);
				c=chars[currchar[ts]].karma;
				if ((j>0) && (c>=10000)) sysmessage(ts,"You are already the most honest person in this world!");
				if ((j<0) && (c<=-10000)) sysmessage(ts,"You are already the most evil person in this world!");
				chars[currchar[ts]].karma+=j;
				if (chars[currchar[ts]].karma>10000) chars[currchar[ts]].karma=10000;
				if (chars[currchar[ts]].karma<-10000) chars[currchar[ts]].karma=-10000;
				j=chars[currchar[ts]].karma-c;
				if (j>0)
				{
					if (j<25) sysmessage(ts,"You have gained a little karma!");
					else if (j<75) sysmessage(ts,"You have gained some karma!");
					else if (j<100) sysmessage(ts,"You have gained a lot of karma!");
					else sysmessage(ts,"You have gained a huge amount of karma!");
				} else if (j<0){
					if (abs(j)<25) sysmessage(ts,"You have lost a little karma!");
					else if (abs(j)<75) sysmessage(ts,"You have lost some karma!");
					else if (abs(j)<100) sysmessage(ts,"You have lost a lot of karma!");
					else sysmessage(ts,"You have lost a huge amount of karma!");
				}
			}
			if (!(strcmp("IFFAME",script1)))  //If karma meets a certain criteria - Magius(CHE)
			{
				char opt;
				p=currchar[ts];
				gettokennum(script2, 0);
				opt=gettokenstr[0];
				gettokennum(script2, 1);
				j=str2num(gettokenstr);
				if (opt=='>')
				{
					if (!(chars[p].fame>=j))
					{
						if (strlen(fmsg)) sysmessage(ts,fmsg);
						else sysmessage(ts,"You need more Fame to do that!");
						closescript();
						return;
					}
				} else if (opt=='<') {
					if (!(chars[p].fame<=abs(j)))
					{
						if (strlen(fmsg)) sysmessage(ts,fmsg);
						else sysmessage(ts,"You need less Fame to do that!");
						closescript();
						return;
					}
				} else ConOut("TRIGGER.SCP:Triggerwitem() Syntax Error in IFFAME token.\n   IFFAME <opt> <value>\n    <opt>= '<' or '>'\n    <value> range from '-10000' to '10000'.\n");
			}
			if (!(strcmp("FAME",script1)))  //Do math on the players fame - Magius(CHE)
			{
				cline = &script2[0];
				splitline();
				j=makenumber(0);
				c=chars[currchar[ts]].fame;
				if ((j>0) && (c>=10000)) sysmessage(ts,"You are already the most glorious person in this world!");
				if ((j<0) && (c<=-10000)) sysmessage(ts,"You are already the most unknown person in this world!");
				chars[currchar[ts]].fame+=j;
				if (chars[currchar[ts]].fame>10000) chars[currchar[ts]].fame=10000;
				if (chars[currchar[ts]].fame<-10000) chars[currchar[ts]].fame=-10000;
				j=chars[currchar[ts]].fame-c;
				if (j>0)
				{
					if (j<25) sysmessage(ts,"You have gained a little fame!");
					else if (j<75) sysmessage(ts,"You have gained some fame!");
					else if (j<100) sysmessage(ts,"You have gained a lot of fame!");
					else sysmessage(ts,"You have gained a huge amount of fame!");
				} else if (j<0) {
					if (abs(j)<25) sysmessage(ts,"You have lost a little fame!");
					else if (abs(j)<75) sysmessage(ts,"You have lost some fame!");
					else if (abs(j)<100) sysmessage(ts,"You have lost a lot of fame!");
					else sysmessage(ts,"You have lost a huge amount of fame!");
				}
				// refresh title
			}
			if (!(strcmp("SETOWNER",script1)))  //Set ownership of item
			{
				if (ti>-1)
				{
					p=currchar[ts];
					if (items[ti].ownserial!=-1) removefromptr(&ownsp[items[ti].ownserial%HASHMAX], ti);
					setserial(ti,p,3);
				}
			}
			if (!(strcmp("NEWOWNER",script1)))  //Set ownership of item
			{
				if (itemnum>-1)
				{
					p=currchar[ts];
					if (items[itemnum].ownserial!=-1) removefromptr(&ownsp[items[itemnum].ownserial%HASHMAX], itemnum);
					setserial(itemnum,p,3);
				}
				if (npcnum>-1)
				{
					p=currchar[ts];
					if (chars[npcnum].ownserial!=-1) removefromptr(&cownsp[chars[npcnum].ownserial%HASHMAX], npcnum);
					setserial(npcnum,p,5);
				}
			}
			if (!(strcmp("NADD",script1)))  //Add NPC at given location
			{
				if (ti>-1)
				{
					p=currchar[ts];
					if (items[ti].contserial==items[chars[p].packitem].serial)
					{
						triggerx=chars[p].x+1;
						triggery=chars[p].y+1;
						triggerz=chars[p].z;
					} else {
						triggerx=items[ti].x;
						triggery=items[ti].y;
						triggerz=items[ti].z;
					}
				}
				else
				{
					x2=(buffer[ts][11]<<8)+buffer[ts][12];
					y2=(buffer[ts][13]<<8)+buffer[ts][14];
					z2=(buffer[ts][15]<<8)+buffer[ts][16];
					triggerx=x2;
					triggery=y2;
					triggerz=z2;
				}
				pos=ftell(scpfile);
				closescript();
				Npcs->AddRespawnNPC(ti,-1, str2num(script2),1);
				//				Npcs->AddRespawnNPC( ti, str2num( script2 ), 1 );
				//Npcs->AddRespawnNPC(ts,str2num(script2),1);
				npcnum=triggerx;
				triggerx=0;
				if (ttype) openscript("triggers.scp");
				else openscript("wtrigrs.scp");
				fseek(scpfile, pos, SEEK_SET);
				strcpy(script1, "DUMMY");
			}
			if (!(strcmp("IDADD",script1)))  //Add item in front of player by ID
			{
				pos = ftell( scpfile );
				closescript();
				unsigned int low=0,high=0;
				cline = &script2[0];
				splitline();
				p=packitem(currchar[ts]);
				if (p==-1) p=0;
				
				low=makenumber(2);
				high = (tnum==4) ? makenumber(3) : low;
				r=RandomNum(low,high);
				c=Items->SpawnItem(ts,r,"#",1,hexnumber(0),hexnumber(1),0,0,1,1);
				itemnum=c;
				if( ttype == 1 )
				{
					openscript("triggers.scp");
				}
				else
				{
					openscript("wtrigrs.scp");
				}
				fseek( scpfile, pos, SEEK_SET );
				strcpy( script1, "DUMMY" );
			}
			if ((!(strcmp("NEWTYPE",script1)))||(!(strcmp("SETTYPE",script1))))  //Set active item type
			{
				if (itemnum>-1)
					items[itemnum].type=str2num(script2);
				else
				{
					if (ti>-1)
						items[ti].type=str2num(script2);
				}
			}
			if (!(strcmp("IADD",script1)))  //Add item in front of player // if 2nd param. is 1, add item into player's backpack - AntiChrist (with autostack)
			{
				cline = &script2[0];
				splitline();
				makenumber( 0 );
				unsigned int InBackpack = makenumber( 1 );
				switch (chars[currchar[ts]].dir)
				{
				case 0: {
					triggerx=chars[currchar[ts]].x;
					triggery=chars[currchar[ts]].y-1;
					break;
						}
				case 1: {
					triggerx=chars[currchar[ts]].x+1;
					triggery=chars[currchar[ts]].y-1;
					break;
						}
				case 2: {
					triggerx=chars[currchar[ts]].x+1;
					triggery=chars[currchar[ts]].y;
					break;
						}
				case 3: {
					triggerx=chars[currchar[ts]].x+1;
					triggery=chars[currchar[ts]].y+1;
					break;
						}
				case 4: {
					triggerx=chars[currchar[ts]].x;
					triggery=chars[currchar[ts]].y+1;
					break;
						}
				case 5: {
					triggerx=chars[currchar[ts]].x-1;
					triggery=chars[currchar[ts]].y+1;
					break;
						}
				case 6: {
					triggerx=chars[currchar[ts]].x-1;
					triggery=chars[currchar[ts]].y;
					break;
						}
				case 7: {
					triggerx=chars[currchar[ts]].x-1;
					triggery=chars[currchar[ts]].y-1;
					break;
						}
				default: {
					triggerx=chars[currchar[ts]].x+1;
					triggery=chars[currchar[ts]].y+1;
					break;
						 }
				}
				triggerz=chars[currchar[ts]].z;
				pos=ftell(scpfile);
				closescript();
				int i=Targ->AddMenuTarget(ts,1,str2num(script2));
				triggerx=0;
				int pack = packitem( currchar[ts] );
				
				if( pack != -1 && i != -1 ) 
				{ // if player has a backpack
					if( InBackpack )
					{ // and item has to be added in player's backpack
						if( items[i].contserial != -1 ) removefromptr( &contsp[items[pack].serial%HASHMAX], i );
						
						removeitem[1] = items[i].ser1;
						removeitem[2] = items[i].ser2;
						removeitem[3] = items[i].ser3;
						removeitem[4] = items[i].ser4;
						
						//AUTOSTACK code
						int ptr, stack, hash;
						hash = items[pack].serial%HASHMAX;
						for( ptr = 0; ptr < contsp[hash].max; ptr++ )
						{
							stack = contsp[hash].pointer[ptr];
							if( stack != -1 )
							{
								if( items[stack].contserial == items[pack].serial )
									// btw, this is the "true" bugfix for the gold-stacking bug, morolan :)
								{
									if( items[stack].pileable && items[i].pileable && items[stack].serial != items[i].serial &&
										( items[stack].id1 == items[i].id1 && items[stack].id2 == items[i].id2 ) && 
										( items[stack].color1 == items[i].color1 && items[stack].color2 == items[i].color2 ) )
									{ // Time to stack.
										if( items[stack].amount + items[i].amount > 65535 )
										{
											items[i].x = items[stack].x;
											items[i].y = items[stack].y;
											items[i].z = 9;
											items[i].amount = ( items[stack].amount + items[i].amount ) - 65535;
											items[stack].amount = 65535;
											unsetserial( i, 1 );	//remove from the old one first
											setserial( i, pack, 1 ); // add to container hash
											Network->xSend( ts, removeitem, 5, 0 );
											RefreshItem( i ); // AntiChrist
										} else
										{
											items[stack].amount = items[stack].amount + items[i].amount;
											Items->DeleItem( i );
										}
										Network->xSend( ts, removeitem, 5, 0 ); // Morrolan fix
										RefreshItem( stack ); // AntiChrist
										itemsfx( ts, items[stack].id1, items[stack].id2 );
										Weight->NewCalc( currchar[ts] );
										statwindow( ts, currchar[ts] );
										break; // break FOR cycle
									} // end if pillable
								} // if stack != -1
							}
							else // no autostacking
							{
								unsetserial( i, 1 );
								setserial( i, pack, 1 );       // no autostacking -> add it to container hash !
								items[i].x = (short int)20 + ( rand()%100 ); // and set new random pack coords
								items[i].y = (short int)40 + ( rand()%80 ); 
								items[i].z = 9;
								UOXSOCKET k;
								for( k = 0; k < now; k++ )
								{
									if( perm[k] )
									{
										Network->xSend( k, removeitem, 5, 0 );
									}
									RefreshItem( i ); // AntiChrist
								} // end for k
								break; // break FOR cycle
							} // else
						} // end for
					} // if InBackpack
				} // if player has backpack
				if (ttype) openscript("triggers.scp");
				else openscript("wtrigrs.scp");
				fseek(scpfile, pos, SEEK_SET);
				strcpy(script1, "DUMMY");
			}
			if (!(strcmp("DISABLE",script1)))  //Disable Item for n seconds
			{
				if (ti>-1) 
					items[ti].disabled = (unsigned int)(uiCurrentTime+( CLOCKS_PER_SEC*str2num(script2) ));
			}
			if (!(strcmp("PUT",script1)))  //Send player to X Y Z when triggered
			{
				cline = &script2[0];
				splitline();
				
				mapRegions->RemoveItem(currchar[ts]+1000000); //LB
				chars[currchar[ts]].x=makenumber(0);
				chars[currchar[ts]].y=makenumber(1);
				chars[currchar[ts]].z=makenumber(2);
				chars[currchar[ts]].dispz = makenumber(3);
				mapRegions->AddItem(currchar[ts]+1000000);
				teleport(currchar[ts]);
			}
			if (!(strcmp("ACT",script1)))  //Make player perform an action
			{
				cline = &script2[0];
				splitline();
				action(ts,hexnumber(0));
			}
			if (!(strcmp("HEA",script1)))  //Do math on players health
			{
				j=str2num(script2);
				if((j<0)&&(chars[currchar[ts]].hp<abs(j))&&(!(chars[currchar[ts]].priv&4)))
				{
					if( !chars[currchar[ts]].dead )
					{
						deathstuff(currchar[ts]);
					}
					closescript();
					return;
				}
				chars[currchar[ts]].hp += j;
				if (chars[currchar[ts]].hp>chars[currchar[ts]].st) chars[currchar[ts]].hp=chars[currchar[ts]].st;
				updatestats(currchar[ts],0);
			}
			if (!(strcmp("MAN",script1)))  //Do math on players mana
			{
				j=str2num(script2);
				if((j<0)&&(chars[currchar[ts]].mn<abs(j)))
				{
					sysmessage(ts,"Your mind is too tired to do that.");
					closescript();
					return;
				}
				chars[currchar[ts]].mn += j;
				if (chars[currchar[ts]].mn>chars[currchar[ts]].in) chars[currchar[ts]].mn=chars[currchar[ts]].in;
				updatestats(currchar[ts],1);
			}
			if (!(strcmp("STAM",script1)))  //Do math on players stamina
			{
				j=str2num(script2);
				if((j<0)&&(chars[currchar[ts]].stm<abs(j)))
				{
					sysmessage(ts,"You are too tired to do that.");
					closescript();
					return;
				}
				chars[currchar[ts]].stm += j;
				if (chars[currchar[ts]].stm>chars[currchar[ts]].dx) chars[currchar[ts]].stm=chars[currchar[ts]].dx;
				updatestats(currchar[ts],2);
			}
			if (!(strcmp("STR",script1)))  //Do math on players strength
			{
				j=str2num(script2);
				if(j<0)
				{
					sysmessage(ts,"You lost strength!");
				} else {
					sysmessage(ts,"You are getting stronger!");
				}
				chars[currchar[ts]].st += j;
				if (chars[currchar[ts]].st<1) chars[currchar[ts]].st=1;
			}
			if (!(strcmp("INT",script1)))  //Do math on players intelligence
			{
				j=str2num(script2);
				if(j<0)
				{
					sysmessage(ts,"Your mind is growing weaker!");
				} else {
					sysmessage(ts,"Your mind is getting stronger!");
				}
				chars[currchar[ts]].in += j;
				if (chars[currchar[ts]].in<1) chars[currchar[ts]].in=1;
			}
			if (!(strcmp("DEX",script1)))  //Do math on players dexterity
			{
				j=str2num(script2);
				if(j<0)
				{
					sysmessage(ts,"You are getting clumsier!");
				} else {
					sysmessage(ts,"You are getting more agile!");
				}
				chars[currchar[ts]].dx += j;
				if (chars[currchar[ts]].dx<1) chars[currchar[ts]].dx=1;
			}
			if (!(strcmp("CSKL",script1)))  //Make a check on the players skill
			{
				i=(rand()%1000)+1;
				//Taur 69.02 added to get some chance of
				// skill gain on failure
				unsigned int skill=str2num(script2);
				unsigned int baseskill=chars[currchar[ts]].baseskill[skill];
				if (i > baseskill)
				{
					Skills->AdvanceSkill(currchar[ts],skill,0);
					Skills->updateSkillLevel(currchar[ts], skill);
					updateskill(ts, skill);
					sysmessage(ts, "You fail in your attempt...");
					closescript();
					return;
				} //Taur end 69.02 change
			}
			if (!(strcmp("SKL",script1)))  //Do math on the players skill
			{
				cline = &script2[0];
				splitline();
				p=makenumber(0);
				j=makenumber(1);
				
				if (j)
				{
					if (chars[currchar[ts]].baseskill[p]<1000)
					{
						chars[currchar[ts]].baseskill[p] += (unsigned short int)j;
						if (chars[currchar[ts]].baseskill[p]>1000) chars[currchar[ts]].baseskill[p]=1000;
						Skills->AdvanceStats(currchar[ts], p);
						Skills->updateSkillLevel(currchar[ts], p);
						updateskill(ts, p);
					}
				} else {
					Skills->AdvanceSkill(currchar[ts],p,1);
					Skills->updateSkillLevel(currchar[ts], p);
					updateskill(ts, p);
				}
			}
			if (!(strcmp("REQ",script1)))  //Check if envoked by certain item.
			{
				sprintf(sect,"x%x%x",chars[currchar[ts]].envokeid1,chars[currchar[ts]].envokeid2);
				if (!strstr(script2,sect))
				{
					sysmessage(ts,"That didn't seem to work.");
					closescript();
					return;
				} else { // Lines Added By Magius(CHE) to fix Targ trigger
					evti = chars[currchar[ts]].envokeitem;
					if( items[chars[currchar[ts]].envokeitem].name[0] != '#' ) // Get Temporany Name of the REQUIRED Item - Magius(CHE)
						strcpy( tempname2, items[chars[currchar[ts]].envokeitem].name );
					else {
						Map->SeekTile(( items[chars[currchar[ts]].envokeitem].id1<<8) + items[chars[currchar[ts]].envokeitem].id2, &tile );
						strcpy( tempname2, (char *)tile.name );
					}
				} // End Addon
			}
			if (!(strcmp("NEED",script1)))  //The item here is required and will be removed
			{
				p=packitem(currchar[ts]);
				if (p!=-1)
				{
					serial=items[p].serial;
					serhash=serial%HASHMAX;
					for (ci=0;ci<contsp[serhash].max;ci++)
					{
						int i;
						i=contsp[serhash].pointer[ci];
						if (i!=-1)
						{
							sprintf(sect,"x%x%x",items[i].id1,items[i].id2);
							if (strstr(script2,sect))
							{
								needitem=i;
								break;
							}
						}
					}
				}
				
				if (needitem<0)
				{
					if( strlen(fmsg)) sysmessage( ts, fmsg ); // Added by Magius(CHE)
					else sysmessage(ts,"You don't have the neccessary supplies to do that."); // Changed by Magius(CHE)
					closescript();
					return;
				}
			}
			if (!(strcmp("USEUP",script1)))  //The item here is required and will be removed
			{
				p=packitem(currchar[ts]);
				if (p==-1) p=0;
				
				if (needitem<0)
				{
					for (ci=0;ci<contsp[serhash].max;ci++)
					{
						int i;
						i=contsp[serhash].pointer[ci];
						if (i!=-1)
						{
							sprintf(sect,"x%x%x",items[i].id1,items[i].id2);
							if (strstr(script2,sect))
							{
								needitem=i;
								break;
							}
						}
					}
				}
				if (needitem<0)
				{
					sysmessage(ts,"It appears as though you have insufficient supplies to make that with.");
					closescript();
					return;
				} else {
					if (items[needitem].amount>1)
						items[needitem].amount--;
					else
						Items->DeleItem(needitem);
				}
			}
			if (!(strcmp("MAKE",script1)))  //Give user the make menu
			{
				//itemmenu(ts,str2num(script2));
				cline = &script2[0];
				splitline();
				itemmake[ts].materialid1=hexnumber(2);
				itemmake[ts].materialid2=hexnumber(3);
				itemmake[ts].has=getamount(currchar[ts], itemmake[ts].materialid1, itemmake[ts].materialid2); 
				itemmake[ts].has2=getamount(currchar[ts], itemmake[ts].materialid1b, itemmake[ts].materialid2b);
				pos=ftell(scpfile);
				closescript();
				Skills->MakeMenu(ts,makenumber(0),makenumber(1));
				if (ttype) openscript("triggers.scp");
				else openscript("wtrigrs.scp");
				fseek(scpfile, pos, SEEK_SET);
				strcpy(script1, "DUMMY");
			}
			if (!(strcmp("CALCUSES",script1)))  //Calculate an items uses
			{
				if (ti>-1)
				{
					if (items[ti].tuses)
					{
						items[ti].tuses+=str2num(script2);
						closescript();
						return;
					}
				}
			}
			if (!(strcmp("SETUSES",script1)))  //Set an items uses
			{
				if (ti>-1)
				{
					items[ti].tuses=str2num(script2);
				}
			}
			if (!(strcmp("RNDUSES",script1)))  //Randomly set an items uses
			{
				if (ti>-1)
				{
					if (!items[ti].tuses)
					{
						cline = &script2[0];
						splitline();
						items[ti].tuses=(rand()%(makenumber(1)))+makenumber(0);
					}
				}
			}
			if (!(strcmp("IDFX",script1)))  //Makes an effect at players by ID
			{
				for (i=0;i<29;i++)
				{
					effect[i]=0;
				}
				if (ti>-1)
				{
					j=packitem(currchar[ts]);
					p=currchar[ts];
					/*if ((items[ti].cont1==items[j].ser1)&&(items[ti].cont2==items[j].ser2)&&
					(items[ti].cont3==items[j].ser3)&&(items[ti].cont4==items[j].ser4))*/
					if (items[ti].contserial==items[j].serial)
					{
						closescript();
						return;
					}
					else
					{
						triggerx=items[ti].x;
						triggery=items[ti].y;
						triggerz=items[ti].z;
					}
				}
				else {
					triggerx=(buffer[ts][11]<<8)+buffer[ts][12];
					triggery=(buffer[ts][13]<<8)+buffer[ts][14];
					triggerz=(buffer[ts][15]<<8)+buffer[ts][16];
				}
				cline = &script2[0];
				splitline();
				effect[0]=0x70; // Effect message
				effect[1]=0x00; // Moving effect
				effect[2]=chars[currchar[ts]].ser1;
				effect[3]=chars[currchar[ts]].ser2;
				effect[4]=chars[currchar[ts]].ser3;
				effect[5]=chars[currchar[ts]].ser4;
				effect[10]=hexnumber(0);// Object id of the effect
				effect[11]=hexnumber(1);
				effect[12] = (char)chars[currchar[ts]].x>>8;
				effect[13] = (char)chars[currchar[ts]].x%256;
				effect[14] = (char)chars[currchar[ts]].y>>8;
				effect[15] = (char)chars[currchar[ts]].y%256;
				effect[16] = (char)chars[currchar[ts]].z;
				effect[17] = (char)triggerx>>8;
				effect[18] = (char)triggerx%256;
				effect[19] = (char)triggery>>8;
				effect[20] = (char)triggery%256;
				effect[21] = triggerz;
				effect[22] = 0x09;
				effect[23] = 0x06; // 0 is really long.  1 is the shortest.
				effect[24] = 0; // This value is unknown
				effect[25] = 0; // This value is unknown
				effect[26] = 0; // This value is unknown
				effect[27] = 0x01; // This value is used for moving effects that explode on impact.
				for( uiTempi = 0; uiTempi < now; uiTempi++ )
				{
					if( perm[uiTempi] && inrange1p( currchar[uiTempi], ts ) )
					{
						Network->xSend( uiTempi, effect, 28, 0 );
					}
				}
				
			}
			if (!(strcmp("RAND",script1)))  //Does a random check
			{
				cline = &script2[0];
				splitline();
				p=(rand()%(makenumber(0)))+1;
				if (p>makenumber(1))
				{
					if (strlen(fmsg)) sysmessage(ts,fmsg);
					closescript();
					return;
				}
			}
			if (!(strcmp("TARG",script1)))  //Give a targeter with trigger number --- Fixed By Magius(CHE)
			{
				target(ts,0,1,0,204,"Select a target");
				chars[currchar[ts]].targtrig=str2num(script2);
			}
			if ((!(strcmp("NEWNAME",script1))))  //Give the new item/npc a name
			{
				if (itemnum>-1)
				{
					strcpy(items[itemnum].name,script2);
				}
				if (npcnum>-1)
				{
					strcpy(chars[npcnum].name,script2);
				}
			}
			if (!(strcmp("NEWTRIG",script1)))  //Give the new item/npc a dynamic trigger number
			{
				if (itemnum>-1)
				{
					items[itemnum].trigger=str2num(script2);
				}
				if (npcnum>-1)
				{
					chars[npcnum].trigger=str2num(script2);
				}
			}
			if (!(strcmp("NEWWORD",script1)))  //Give the new npc a trigger word
			{
				if (npcnum>-1)
				{
					strcpy(chars[npcnum].trigword,script2);
				}
			}
			if (!(strcmp("MISC",script1)))  //Perform a miscellaneous function
			{
				if (!(strcmp("bank",strlwr(script2)))) openbank(ts,currchar[ts]);
			}
			// End World Triggers
    }
  }
  while (script1[0]!='}');
  closescript();
}

//**************************************************************************
//This routine processes tokens for the NPC triggers.
void triggernpc(int ts,int ti)
{
	char sect[512];
	char effect[29];
	signed int j;
	int p;
	int c;
	unsigned int i;
	int itemnum=-1;
	int npcnum=-1;
	int needitem=-1;
	long int pos;
	char fmsg[512];
	
	*fmsg='\0'; // was sprintf(fmsg,"");
	
	openscript("ntrigrs.scp");
	sprintf(sect, "TRG %i", chars[ti].trigger);
	if (!i_scripts[ntrigger_script]->find(sect))
	{
		closescript();
		return;
	}
	do
	{
		read2();
		if (script1[0]!='}')
		{
			if(!(strcmp("IFHUNGER", script1 ))) // If hunger meets a certain criteria - Magius(CHE)
			{
				j = str2num( script2 );
				p = currchar[ts];
				if( j >= 0 )
				{
					if(!(chars[p].hunger >= j ))
					{
						if( strlen( fmsg )) sysmessage( ts, fmsg );
						closescript();
						return;
					}
				} else {
					if( !(chars[p].hunger <= abs(j)))
					{
						if( strlen( fmsg ) ) sysmessage( ts, fmsg );
						closescript();
						return;
					}
				}
			}
			if( !(strcmp("HUNGER", script1 ))) // Do math on players hunger from 0 to 6 - Magius(CHE)
			{
				j = str2num( script2 );
				chars[currchar[ts]].hunger += j;
				if( chars[currchar[ts]].hunger > 6 ) chars[currchar[ts]].hunger = 6;
				if( chars[currchar[ts]].hunger < 1 ) chars[currchar[ts]].hunger = 1;
				switch( chars[currchar[ts]].hunger )
				{
				case 0:	sysmessage( ts, "You eat the food, but are still extremely hungry." );
					break;
				case 1:	sysmessage( ts, "You eat the food, but are still extremely hungry." );
					break;
				case 2:	sysmessage( ts, "After eating the food, you feel much less hungry." );
					break;
				case 3:	sysmessage( ts, "You eat the food, and begin to feel more satiated." );
					break;
				case 4:	sysmessage( ts, "You feel quite full after consuming the food." );
					break;
				case 5:	sysmessage( ts, "You are nearly stuffed, but manage to eat the food." );
					break;
				case 6:	sysmessage( ts, "You are simply too full to eat any more!" );
					break;
				}
			}
			if(!(strcmp("IFKARMA", script1 ))) // If karma meets a certain criteria - Magius(CHE)
			{
				char opt;
				p = currchar[ts];
				gettokennum( script2, 0 );
				opt = gettokenstr[0];
				gettokennum( script2, 1 );
				j = str2num( gettokenstr );
				if( opt == '>' )
				{
					if(!(chars[p].karma >= j ))
					{
						if( strlen( fmsg ) ) sysmessage( ts, fmsg );
						else sysmessage( ts, "You need more Karma to do that!" );
						closescript();
						return;
					}
				} else if( opt == '<' ) {
					if(!(chars[p].karma <= j ) )
					{
						if( strlen( fmsg ) ) sysmessage( ts, fmsg );
						else sysmessage( ts, "You need less Karma to do that!" );
						closescript();
						return;
					}
				} else ConOut("TRIGGER.SCP: Triggerwitem() Syntax Error in IFKARMA token.\n    IFKARMA <opt> <value> \n    <opt> = '<' or '>' \n    <value> range from '-10000' to '10000'.\n" );
			}
			
			if (!(strcmp("KARMA",script1)))  //Do math on the players karma - Magius(CHE)
			{
				cline = &script2[0];
				splitline();
				j=makenumber(0);
				c=chars[currchar[ts]].karma;
				if ((j>0) && (c>=10000)) sysmessage(ts,"You are already the most honest person in this world!");
				if ((j<0) && (c<=-10000)) sysmessage(ts,"You are already the most evil person in this world!");
				chars[currchar[ts]].karma+=j;
				if (chars[currchar[ts]].karma>10000) chars[currchar[ts]].karma=10000;
				if (chars[currchar[ts]].karma<-10000) chars[currchar[ts]].karma=-10000;
				j=chars[currchar[ts]].karma-c;
				if (j>0)
				{
					if (j<25) sysmessage(ts,"You have gained a little karma!");
					else if (j<75) sysmessage(ts,"You have gained some karma!");
					else if (j<100) sysmessage(ts,"You have gained a lot of karma!");
					else sysmessage(ts,"You have gained a huge amount of karma!");
				} else if (j<0){
					if (abs(j)<25) sysmessage(ts,"You have lost a little karma!");
					else if (abs(j)<75) sysmessage(ts,"You have lost some karma!");
					else if (abs(j)<100) sysmessage(ts,"You have lost a lot of karma!");
					else sysmessage(ts,"You have lost a huge amount of karma!");
				}
			}
			if (!(strcmp("IFFAME",script1)))  //If karma meets a certain criteria - Magius(CHE)
			{
				char opt;
				p=currchar[ts];
				gettokennum(script2, 0);
				opt=gettokenstr[0];
				gettokennum(script2, 1);
				j=str2num(gettokenstr);
				if (opt=='>')
				{
					if (!(chars[p].fame>=j))
					{
						if (strlen(fmsg)) sysmessage(ts,fmsg);
						else sysmessage(ts,"You need more Fame to do that!");
						closescript();
						return;
					}
				} else if (opt=='<') {
					if (!(chars[p].fame<=abs(j)))
					{
						if (strlen(fmsg)) sysmessage(ts,fmsg);
						else sysmessage(ts,"You need less Fame to do that!");
						closescript();
						return;
					}
				} else ConOut("TRIGGER.SCP:Triggerwitem() Syntax Error in IFFAME token.\n   IFFAME <opt> <value>\n    <opt>= '<' or '>'\n    <value> range from '-10000' to '10000'.\n");
			}
			if (!(strcmp("FAME",script1)))  //Do math on the players fame - Magius(CHE)
			{
				cline = &script2[0];
				splitline();
				j=makenumber(0);
				c=chars[currchar[ts]].fame;
				if ((j>0) && (c>=10000))	sysmessage(ts,"You are already the most gloriosus person in this world!");
				if ((j<0) && (c<=-10000)) sysmessage(ts,"You are already the most unknown person in this world!");
				chars[currchar[ts]].fame+=j;
				if (chars[currchar[ts]].fame>10000) chars[currchar[ts]].fame=10000;
				if (chars[currchar[ts]].fame<-10000) chars[currchar[ts]].fame=-10000;
				j=chars[currchar[ts]].fame-c;
				if (j>0)
				{
					if (j<25) sysmessage(ts,"You have gained a little fame!");
					else if (j<75) sysmessage(ts,"You have gained some fame!");
					else if (j<100) sysmessage(ts,"You have gained a lot of fame!");
					else sysmessage(ts,"You have gained a huge amount of fame!");
				} else if (j<0) {
					if (abs(j)<25) sysmessage(ts,"You have lost a little fame!");
					else if (abs(j)<75) sysmessage(ts,"You have lost some fame!");
					else if (abs(j)<100) sysmessage(ts,"You have lost a lot of fame!");
					else sysmessage(ts,"You have lost a huge amount of fame!");
				}
				// refresh title
			}
			if (!(strcmp("EMT",script1)))  //Player says something when trigger is activated
			{
				strcpy(sect, script2);
				npctalkall(ti,sect, 0);
			}
			if (!(strcmp("MSG",script1)))  //Display a message when trigger is activated
			{
				if (!(chars[currchar[ts]].npc))
				{
					strcpy(sect, script2);
					sysmessage(ts,sect);
				}
			}
			if (!(strcmp("FMSG",script1)))  //Set fail message
			{
				strcpy(fmsg, script2);
			}
			if (!(strcmp("SND",script1)))  //Play a sound when trigger is activated
			{
				cline = &script2[0];
				splitline();
				soundeffect2(ti, hexnumber(0), hexnumber(1));
			}
			if (!(strcmp("SETTRG",script1)))  //Set npcs trigger to new trigger
			{
				chars[ti].trigger=str2num(script2);
			}
			if (!(strcmp("SETID",script1)))  //Set chars id to new id
			{
				cline = &script2[0];
				splitline();
				chars[ti].id1 = (unsigned char)hexnumber(0);
				chars[ti].id2 = (unsigned char)hexnumber(1);
				chars[ti].xid1 = (unsigned char)hexnumber(0);
				chars[ti].xid2 = (unsigned char)hexnumber(1);
				chars[ti].orgid1 = (unsigned char)hexnumber(0);
				chars[ti].orgid2 = (unsigned char)hexnumber(1);
				for (j=0;j<now;j++) if (perm[j] && inrange1p( currchar[j], ti )) updatechar(ti);
			}
			if (!(strcmp("NADD",script1)))  //Add NPC at given location
			{
				p=currchar[ts];
				triggerx=chars[p].x+1;
				triggery=chars[p].y+1;
				triggerz=chars[p].z;
				pos=ftell(scpfile);
				closescript();
				Npcs->AddRespawnNPC(ti,-1, str2num(script2),1);
				//				Npcs->AddRespawnNPC( ti, str2num( script2 ), 1 );
				npcnum=triggerx;
				triggerx=0;
				openscript("ntrigrs.scp");
				fseek(scpfile, pos, SEEK_SET);
				strcpy(script1, "DUMMY");
				
			}
			if (!(strcmp("IDADD",script1)))  //Add item in player pack by ID
			{
				cline = &script2[0];
				splitline();
				//c=memitemfree();
				p=packitem(currchar[ts]);
				if (p==-1) p=0;
				c=Items->SpawnItem(ts,1,"#",1,hexnumber(0),hexnumber(1),0,0,1,1);
			}
			if (!(strcmp("IADD",script1)))  //Add item in front of player
			{
				switch (chars[currchar[ts]].dir)
				{
				case 0: {
					triggerx=chars[currchar[ts]].x;
					triggery=chars[currchar[ts]].y-1;
					break;
						}
				case 1: {
					triggerx=chars[currchar[ts]].x+1;
					triggery=chars[currchar[ts]].y-1;
					break;
						}
				case 2: {
					triggerx=chars[currchar[ts]].x+1;
					triggery=chars[currchar[ts]].y;
					break;
						}
				case 3: {
					triggerx=chars[currchar[ts]].x+1;
					triggery=chars[currchar[ts]].y+1;
					break;
						}
				case 4: {
					triggerx=chars[currchar[ts]].x;
					triggery=chars[currchar[ts]].y+1;
					break;
						}
				case 5: {
					triggerx=chars[currchar[ts]].x-1;
					triggery=chars[currchar[ts]].y+1;
					break;
						}
				case 6: {
					triggerx=chars[currchar[ts]].x-1;
					triggery=chars[currchar[ts]].y;
					break;
						}
				case 7: {
					triggerx=chars[currchar[ts]].x-1;
					triggery=chars[currchar[ts]].y-1;
					break;
						}
				default: {
					triggerx=chars[currchar[ts]].x+1;
					triggery=chars[currchar[ts]].y+1;
					break;
						 }
				}
				triggerz=chars[currchar[ts]].z;
				pos=ftell(scpfile);
				closescript();
				itemnum=Targ->AddMenuTarget(ts,1,str2num(script2));
				triggerx=0;
				openscript("ntrigrs.scp");
				fseek(scpfile, pos, SEEK_SET);
				strcpy(script1, "DUMMY");
			}
			if (!(strcmp("DISABLE",script1)))  //Disable NPC for n seconds
			{
				chars[ti].disabled = (unsigned int)(uiCurrentTime+(CLOCKS_PER_SEC*str2num(script2)));
			}
			if (!(strcmp("PUT",script1)))  //Send player to X Y Z when triggered
			{
				cline = &script2[0];
				splitline();
				
				chars[currchar[ts]].x=makenumber(0);
				chars[currchar[ts]].y=makenumber(1);
				chars[currchar[ts]].z=makenumber(2);
				teleport(currchar[ts]);
			}
			if (!(strcmp("ACT",script1)))  //Make player perform an action
			{
				cline = &script2[0];
				splitline();
				action(ts,hexnumber(0));
			}
			if (!(strcmp("NPCACT",script1)))  //Make player perform an action
			{
				cline = &script2[0];
				splitline();
				npcaction(ti,hexnumber(0));
			}
			if (!(strcmp("HEA",script1)))  //Do math on players health
			{
				j=str2num(script2);
				if((j<0)&&(chars[currchar[ts]].hp<abs(j))&&(!(chars[currchar[ts]].priv&4)))
				{
					if( !chars[currchar[ts]].dead ) // only kill it if it's not already dead
					{
						deathstuff(currchar[ts]);
					}
					closescript();
					return;
				}
				chars[currchar[ts]].hp += j;
				if (chars[currchar[ts]].hp>chars[currchar[ts]].st) chars[currchar[ts]].hp=chars[currchar[ts]].st;
				updatestats(currchar[ts],0);
			}
			if (!(strcmp("MAN",script1)))  //Do math on players mana
			{
				j=str2num(script2);
				if((j<0)&&(chars[currchar[ts]].mn<abs(j)))
				{
					sysmessage(ts,"Your mind is too tired to do that.");
					closescript();
					return;
				}
				chars[currchar[ts]].mn += j;
				if (chars[currchar[ts]].mn>chars[currchar[ts]].in) chars[currchar[ts]].mn=chars[currchar[ts]].in;
				updatestats(currchar[ts],1);
			}
			if (!(strcmp("STAM",script1)))  //Do math on players stamina
			{
				j=str2num(script2);
				if((j<0)&&(chars[currchar[ts]].stm<abs(j)))
				{
					sysmessage(ts,"You are too tired to do that.");
					closescript();
					return;
				}
				chars[currchar[ts]].stm += j;
				if (chars[currchar[ts]].stm>chars[currchar[ts]].dx) chars[currchar[ts]].stm=chars[currchar[ts]].dx;
				updatestats(currchar[ts],2);
			}
			if (!(strcmp("STR",script1)))  //Do math on players strength
			{
				j=str2num(script2);
				if(j<0)
				{
					sysmessage(ts,"You lost strength!");
				} else {
					sysmessage(ts,"You are getting stronger!");
				}
				chars[currchar[ts]].st += j;
				if (chars[currchar[ts]].st<1) chars[currchar[ts]].st=1;
			}
			if (!(strcmp("INT",script1)))  //Do math on players intelligence
			{
				j=str2num(script2);
				if(j<0)
				{
					sysmessage(ts,"Your mind is growing weaker!");
				} else {
					sysmessage(ts,"Your mind is getting stronger!");
				}
				chars[currchar[ts]].in += j;
				if (chars[currchar[ts]].in<1) chars[currchar[ts]].in=1;
			}
			if (!(strcmp("DEX",script1)))  //Do math on players dexterity
			{
				j=str2num(script2);
				if(j<0)
				{
					sysmessage(ts,"You are getting clumsier!");
				} else {
					sysmessage(ts,"You are getting more agile!");
				}
				chars[currchar[ts]].dx += j;
				if (chars[currchar[ts]].dx<1) chars[currchar[ts]].dx=1;
			}
			if (!(strcmp("CSKL",script1)))  //Make a check on the players skill
			{
				i=(rand()%1000)+1;
				if (i>chars[currchar[ts]].baseskill[str2num(script2)])
				{
					if (strlen(fmsg)) sysmessage(ts,fmsg);
					closescript();
					return;
				}
			}
			if (!(strcmp("SKL",script1)))  //Do math on the players skill
			{
				cline = &script2[0];
				splitline();
				p=makenumber(0);
				j=makenumber(1);
				
				if (j)
				{
					if (chars[currchar[ts]].baseskill[p]<1000)
					{
						chars[currchar[ts]].baseskill[p] += (unsigned short int)j;
						if (chars[currchar[ts]].baseskill[p]>1000) chars[currchar[ts]].baseskill[p]=1000;
					}
				} else {
					Skills->AdvanceSkill(currchar[ts],p,1);
				}
			}
			if (!(strcmp("IFOWNER",script1)))  //If character is owned by triggerer
			{
				if (ti>-1)
				{
					p=currchar[ts];
					if (chars[ti].ownserial!=chars[p].serial)
					{
						sysmessage(ts,"You do not own this creature.");
						closescript();
						return;
					}
				}
			}
			if (!(strcmp("SETOWNER",script1)))  //Set ownership of NPC
			{
				if (ti>-1)
				{
					p=currchar[ts];
					if (chars[ti].ownserial!=-1)
						removefromptr(&cownsp[chars[ti].ownserial%HASHMAX], ti);
					setserial(ti,p,5);
				}
			}
			if (!(strcmp("NEWOWNER",script1)))  //Set ownership of NPC
			{
				if (itemnum>-1)
				{
					p=currchar[ts];
					if (items[itemnum].ownserial!=-1)
						removefromptr(&ownsp[items[itemnum].ownserial%HASHMAX], itemnum);
					setserial(itemnum,p,3);
				}
				if (npcnum>-1)
				{
					p=currchar[ts];
					if (chars[npcnum].ownserial!=-1)
						removefromptr(&cownsp[chars[npcnum].ownserial%HASHMAX], npcnum);
					setserial(npcnum,p,5);
				}
			}
			if (!(strcmp("NEED",script1)))  //The item here is required and will be removed
			{
				p=packitem(currchar[ts]);
				//for (i=0;i<itemcount;i++)
				//{
				if (p!=-1)
				{
					for (itemnum=0;itemnum<contsp[items[p].serial%HASHMAX].max;itemnum++)
					{
						i = contsp[items[p].serial%HASHMAX].pointer[itemnum];
						if (i!=-1)
						{
							if (items[i].contserial==items[p].serial)
							{
								sprintf(sect,"x%x%x",items[i].id1,items[i].id2);
								if (strstr(script2,sect))
								{
									needitem=i;
									break;
								}
							}
						}
					}
				}
				if (needitem<0)
				{
					if( strlen( fmsg ) ) sysmessage( ts, fmsg ); // Added by Magius(CHE)
					else sysmessage(ts,"You don't have the neccessary supplies to do that."); // Changed by Magius(CHE)
					closescript();
					return;
				}
			}
			if (!(strcmp("USEUP",script1)))  //The item here is required and will be removed
			{
				p=packitem(currchar[ts]);
				if (p!=-1) //lb
				{
					if (needitem<0)
					{
						//for (i=0;i<itemcount;i++)
						for (itemnum=0;itemnum<contsp[items[p].serial%HASHMAX].max;itemnum++)
						{
							i=contsp[items[p].serial%HASHMAX].pointer[itemnum];
							if ((i!=-1)&&(items[i].cont1==items[p].ser1)&&(items[i].cont2==items[p].ser2)&&
								(items[i].cont3==items[p].ser3)&&(items[i].cont4==items[p].ser4))
							{
								sprintf(sect,"x%x%x",items[i].id1,items[i].id2);
								if (strstr(script2,sect))
								{
									needitem=i;
									break;
								}
							}
						}
					}
					if (needitem<0)
					{
						sysmessage(ts,"It appears as though you have insufficient supplies to make that with.");
						closescript();
						return;
					} else {
						if (items[needitem].amount>1)
							items[needitem].amount--;
						else
							Items->DeleItem(needitem);
					}
				}
			}
			if (!(strcmp("MAKE",script1)))  //Give user the make menu
			{
				cline = &script2[0];
				splitline();
				itemmake[ts].materialid1 = (unsigned char)hexnumber(2);
				itemmake[ts].materialid2 = (unsigned char)hexnumber(3);
				itemmake[ts].has=getamount(currchar[ts], itemmake[ts].materialid1, itemmake[ts].materialid2); 
				itemmake[ts].has2=getamount(currchar[ts], itemmake[ts].materialid1b, itemmake[ts].materialid2b);
				pos=ftell(scpfile);
				closescript();
				Skills->MakeMenu(ts,makenumber(0),makenumber(1));
				openscript("ntrigrs.scp");
				fseek(scpfile, pos, SEEK_SET);
				strcpy(script1, "DUMMY");
			}
			if ((!(strcmp("NEWTYPE",script1)))||(!(strcmp("SETTYPE",script1))))  //Set active item type
			{
				if (itemnum>-1)
					items[itemnum].type=str2num(script2);
			}
			if (!(strcmp("RAND",script1)))  //Does a random check
			{
				cline = &script2[0];
				splitline();
				p=(rand()%(makenumber(0)))+1;
				if (p>makenumber(1))
				{
					if (strlen(fmsg)) sysmessage(ts,fmsg);
					closescript();
					return;
				}
			}
			if (!(strcmp("SETWORD",script1)))  //Sets the trigger word of an NPC
			{
				strcpy(chars[ti].trigword, script2);
			}
			if (!(strcmp("IDFX",script1)))  //Makes an effect at players by ID
			{
				for (i=0;i<29;i++)
				{
					effect[i]=0;
				}
				cline = &script2[0];
				splitline();
				effect[0]=0x70; // Effect message
				effect[1]=0x00; // Moving effect
				effect[2]=chars[ti].ser1;
				effect[3]=chars[ti].ser2;
				effect[4]=chars[ti].ser3;
				effect[5]=chars[ti].ser4;
				effect[6]=chars[currchar[ts]].ser1;
				effect[7]=chars[currchar[ts]].ser2;
				effect[8]=chars[currchar[ts]].ser3;
				effect[9]=chars[currchar[ts]].ser4;
				effect[10] = (char)hexnumber(0);// Object id of the effect
				effect[11] = (char)hexnumber(1);
				effect[12] = (char)chars[ti].x>>8;
				effect[13] = (char)chars[ti].x%256;
				effect[14] = (char)chars[ti].y>>8;
				effect[15] = (char)chars[ti].y%256;
				effect[16] = chars[ti].z;
				effect[17] = (char)chars[currchar[ts]].x>>8;
				effect[18] = (char)chars[currchar[ts]].x%256;
				effect[19] = (char)chars[currchar[ts]].y>>8;
				effect[20] = (char)chars[currchar[ts]].y%256;
				effect[21] = chars[currchar[ts]].z;
				effect[22]=0x09;
				effect[23]=0x06; // 0 is really long.  1 is the shortest.
				effect[24]=0; // This value is unknown
				effect[25]=0; // This value is unknown
				effect[26]=0; // This value is unknown
				effect[27]=0x00; // This value is used for moving effects that explode on impact.
				for( j = 0; j < now; j++ )
				{
					if( perm[j] && inrange1p( currchar[j], currchar[ts] ) )
					{
						Network->xSend(j, effect, 28, 0);
					}
				}
				
			}
			if (!(strcmp("NEWNAME",script1)))  //Give the new item/npc a name
			{
				if (itemnum>-1)
				{
					strcpy(items[itemnum].name,script2);
				}
				if (npcnum>-1)
				{
					strcpy(chars[npcnum].name,script2);
				}
			}
			if (!(strcmp("NEWTRIG",script1)))  //Give the new item/npc a dynamic trigger number
			{
				if (itemnum>-1)
				{
					items[itemnum].trigger=str2num(script2);
				}
				if (npcnum>-1)
				{
					chars[npcnum].trigger=str2num(script2);
				}
			}
			if (!(strcmp("NEWWORD",script1)))  //Give the new npc a triggerword
			{
				if (npcnum>-1)
				{
					strcpy(chars[npcnum].trigword,script2);
				}
			}
			if (!(strcmp("MISC",script1)))  //Perform a miscellaneous function
			{
				if (!(strcmp("bank",strlwr(script2)))) openbank(ts,currchar[ts]);
			}
			// End NPC Triggers
    }
  }
  while (script1[0]!='}');
  closescript();
}
//**************************************************************************
int checkenvoke(char eid1, char eid2)
{
	FILE *envokefile;
	char buf[1024];
	
	if (!(envokefile = fopen("envoke.scp", "r"))) {
		fprintf(stderr, "Cannot open envoke.scp: %s", strerror(errno));
		exit(1);
	}
	while(fgets(buf, sizeof(buf), envokefile))
	{
		sprintf(temp,"x%x%xx",eid1,eid2);      // antichrist
		sprintf(temp2,"x%x%x ",eid1,eid2);	  // antichrist
		if ((strstr(buf, temp) > 0)||(strstr(buf, temp2) > 0))
		{
			fclose(envokefile);
			return 1;
		}
	}
	fclose(envokefile);
	
	return 0;
}
