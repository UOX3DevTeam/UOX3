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
		if (perm[i] && inrange1p(npc, currchar[i]))
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
		if (perm[i] && inrange1p(npc, currchar[i]))
			npcemote(i, npc, txt, antispam );
}


void talking( UOXSOCKET s ) // PC speech
{
	int mapitemptr,mapitem,mapchar,a,checkgrid,increment,StartGrid,getcell,ab;
	int tl, i, j, resp=0, found, x1, x2, y1, y2, match, m2, sml, grrr;
	char sect[512];
	
	
//	if ((buffer[s][8]=='/') || ((buffer[s][8]=='.') && (buffer[s][9]!='.'))) Commands->Command(s);
	if( ( buffer[s][8] == server_data.commandPrefix ) || ( ( buffer[s][8] == '.' ) && ( buffer[s][9] != '.' ) ) )
		Commands->Command( s );
	else
	{	// we don't want them becoming visible if they're using a command!
		//AntiChrist
		if((chars[currchar[s]].hidden)&&(!(chars[currchar[s]].priv2&8)))
		{
			chars[currchar[s]].hidden=0;
			chars[currchar[s]].stealth=-1;
			updatechar(currchar[s]);
		}
		resp=response(s);
		if (resp!=0) return;  //Vendor responded already
		if( ( buffer[s][3] == 0x09 ) && ( chars[currchar[s]].priv&2 ) )
		{
			broadcast(s);
		}
		else
		{
			char text[512];
			strcpy(text,(char *)&buffer[s][8]);
			if (!strcmp(strupr(text),"I RESIGN FROM MY GUILD")) Guilds->Resign(s);
			tl=44+strlen((char *)&buffer[s][8])+1;
			talk[1]=tl>>8;
			talk[2]=tl%256;
			talk[3]=chars[currchar[s]].ser1;
			talk[4]=chars[currchar[s]].ser2;
			talk[5]=chars[currchar[s]].ser3;
			talk[6]=chars[currchar[s]].ser4;
			talk[7]=chars[currchar[s]].id1;
			talk[8]=chars[currchar[s]].id2;
			talk[9]=buffer[s][3]; // Type
			talk[10]=buffer[s][4];
			talk[11]=buffer[s][5];
			talk[12]=buffer[s][6];
			//   talk[13]=buffer[s][7]; // Font
			talk[13]=chars[currchar[s]].fonttype;
			Network->xSend(s, talk, 14, 0);
			Network->xSend(s, chars[currchar[s]].name, 30, 0);
			Network->xSend(s, &buffer[s][8], strlen((char *)&buffer[s][8])+1, 0);
			
			if (talk[9]==0)
			{
				chars[currchar[s]].saycolor1=buffer[s][4];
				chars[currchar[s]].saycolor2=buffer[s][5];
			}
			if (talk[9]==2)
			{
				chars[currchar[s]].emotecolor1=buffer[s][4];
				chars[currchar[s]].emotecolor2=buffer[s][5];
				return;
			}
			if (server_data.log==2) //Logging -- Zippy
			{
				char temp2[512];
				sprintf(temp2, "%s.log",chars[currchar[s]].name);
				sprintf(temp,"%s [%x %x %x %x] [%i]: %s\n",chars[currchar[s]].name,chars[currchar[s]].ser1,chars[currchar[s]].ser2,chars[currchar[s]].ser3,chars[currchar[s]].ser4,chars[currchar[s]].account,&buffer[s][8]);
				savelog(temp,temp2);
			}
			
			char search1[10];
			strcpy( search1, "GUARDS" );
			char *response1;
			char str[256];
			strcpy( str, (char *)&buffer[s][8] );
			strupr( str );
			response1 = ( strstr( str, search1 ) );
			if( response1 )
				callguards( currchar[s] );
            Boats->Speech(s,&buffer[s][8]);//Boats 
			
			// AntiChrist bugfix (6/10/99)
			house_speech( s, &buffer[s][8] ); // houses crackerjack 8/12/99
			
			for (i=0;i<now;i++)
			{
				//if ((inrange1(i, s)&&perm[i]))
				if ( perm[i] && inrange1(i, s) )// && //LOS check commented out for now -- AntiChrist
					//line_of_sight(s, chars[currchar[s]].x, chars[currchar[s]].y, chars[currchar[s]].z, chars[currchar[i]].x, chars[currchar[i]].y, chars[currchar[i]].z, WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING))
				{
					Network->xSend(i, talk, 14, 0);
					Network->xSend(i, chars[currchar[s]].name, 30, 0);
					//					if((chars[currchar[i]].priv&0x01)||(chars[currchar[i]].priv&0x80));// GM/Counselors can see ghosts talking always  Seers?
					//					else if(chars[currchar[i]].spiritspeaktimer==0) // If the spiritspeaktimer is set, you can talk to the dead
					// modified by AntiChrist
					if( !(chars[currchar[i]].priv&1 ) && !(chars[currchar[i]].priv&0x80 ) && chars[currchar[i]].spiritspeaktimer == 0 ) // GM/Counselors can see ghosts talking always  Seers?
					{
						if(chars[currchar[s]].dead==1&&chars[currchar[i]].dead==0) 
						{
							grrr=strlen((char *)&buffer[s][8])+8; // bugfix by lord binary (ghost speak crash)
							for(j=8;j<grrr;j++) // convert the string to ghost talk
							{
								if(buffer[s][j]==32)
									buffer[s][j]=32;
								else if(buffer[s][j]%2)
									buffer[s][j]='O';
								else
									buffer[s][j]='o';
							}
						}
					}
					else if( chars[currchar[i]].race != chars[currchar[s]].race && chars[currchar[i]].priv&0x01 != 0x01 )
					{
						if( !Skills->CheckSkill( currchar[i], SPIRITSPEAK, Races->getLanguageMin( chars[currchar[s]].race ), 1000 ) )
						{
							for(j=13;j<((buffer[s][1]<<8)+buffer[s][2])-2;j=j+2) // bugfix (ghost-speak crash) by lord binary
							{
								if(buffer[s][j]==32) {;}
								else if(buffer[s][j]%2)     
									buffer[s][j]='O';
								else
									buffer[s][j]='o';
							}					 
						}
					}
					Network->xSend(i, &buffer[s][8], strlen((char *)&buffer[s][8])+1, 0);
				}
			}
			if(chars[currchar[s]].dead==1)  // this makes it so npcs do not respond to dead people
				return;
			
			i=0;
			found=0;
			x1=chars[currchar[s]].x;
			y1=chars[currchar[s]].y;
			StartGrid=mapRegions->StartGrid(chars[currchar[s]].x,chars[currchar[s]].y);
			getcell=mapRegions->GetCell(chars[currchar[s]].x,chars[currchar[s]].y);
						
			increment=0;
			ab=0;
			for (checkgrid=StartGrid+(increment*mapRegions->GetColSize());increment<3;increment++, checkgrid=StartGrid+(increment*mapRegions->GetColSize()))
			{	
				for (a=0;a<3;a++)
				{					
					mapitemptr=-1;
					mapitem=-1;
					mapchar=-1;
					do //check all items in this cell
					{
						mapchar=-1;
						mapitemptr=mapRegions->GetNextItem(checkgrid+a, mapitemptr);
						if (mapitemptr==-1) break;
						mapitem=mapRegions->GetItem(checkgrid+a, mapitemptr);
						if(mapitem>999999) mapchar=mapitem-1000000;
						if (mapitem!=-1 && mapitem>=1000000)
						{
						    i=mapchar;			
							if ((i!=currchar[s]) && (chars[i].npc))
							{
					          x2=chars[i].x;
					          y2=chars[i].y;
							  if ( (abs(x1-x2)<=2) && (abs(y1-y2)<=2) ) 
							  {
								  found=i+1;
								  ab=1;
								  break;
							  }
							}
						}
					} while (mapitemptr != -1 && !found);
					if (found) break;
				}
				if (found) break;
			}
			if (found && chars[found-1].speech )
			{
				responsevendor(s);
				found--;
				for (i=0;i<strlen((char *)&buffer[s][8]);i++) buffer[s][i+8]=toupper(buffer[s][i+8]);
//				talkingto[found]=currchar[s];
				openscript("speech.scp");
				sprintf(sect, "SPEECH %i", chars[found].speech);
				if (!i_scripts[speech_script]->find(sect))
				{
					closescript();
					return;
				}
				match=0;
				strcpy(sect, "NO DEFAULT TEXT DEFINED");
				do
				{
					read2();
					if (script1[0]!='}')
					{
						if (!(strcmp("DEFAULT",script1)))
						{
							strcpy(sect, script2);
						}
						if (!(strcmp("ON",script1)))
						{
							j=0;
							do
							{
								m2=1;
								sml=strlen(script2);
								if (strlen((char *)&buffer[s][8+j])<sml)
								{
									//         sml=strlen(&buffer[s][8+j]);
									m2=0;
								}
								else
									for (i=0;i<sml;i++)
									{
										if (buffer[s][i+8+j]!=toupper(script2[i]))
										{
											m2=0;
										}
									}
									j++;
							}
							while ((j<strlen((char *)&buffer[s][8]))&&(m2==0));
							if (m2==1) match=1;
						}
						if (!(strcmp("SAY",script1)))
						{
							if (match==1)
							{
								npctalk(s, found, script2, 0);
								match=2;
							}
						}
					}
				}
				while (script1[0]!='}');
				if (match==0)
				{
					npctalk(s, found, sect, 0);
				}
				closescript();
			}
  }
 }
}


void unicodetalking( UOXSOCKET s) // PC speech
{
	int mapitemptr,mapitem,mapchar,a,checkgrid,increment,StartGrid,getcell,ab;
	int tl, i, j,resp, found, x1, x2, y1, y2, match, m2, sml;
	char sect[512];
	char nonuni[512];
	unsigned char talk2[19];
	
	tl=48+(buffer[s][1]<<8)+buffer[s][2];
	
	for (i=13;i<(buffer[s][1]<<8)+buffer[s][2];i=i+2)
	{
		nonuni[(i-13)/2]=buffer[s][i];
	} 
	
//	if ((buffer[s][13]=='/') || ((buffer[s][13]=='.') && (buffer[s][14]!='.'))) Commands->Command (s);
//	if( nonuni[0] == '/' || ( nonuni[1] == '.' && nonuni[2] != '.' ) ) Commands->Command( s );
	if( nonuni[0] == server_data.commandPrefix || ( nonuni[1] == '.' && nonuni[2] != '.' ) ) Commands->Command( s );
	else
	{	// we don't want them becoming visible if it was a command!
		//AntiChrist
		if((chars[currchar[s]].hidden)&&(!(chars[currchar[s]].priv2&8)))
		{
			chars[currchar[s]].hidden=0;
			chars[currchar[s]].stealth=-1;
			updatechar(currchar[s]);
		}
		resp=response(s);
		
		if( ( buffer[s][3] == 0x09 ) && ( chars[currchar[s]].priv&2 ) )
		{
			broadcast(s);
		}
		else
		{
			char text[512];
			strcpy( text, nonuni );
			if (!strcmp(strupr(text),"I RESIGN FROM MY GUILD")) Guilds->Resign(s);
			talk2[0]=0xAE;
			talk2[1]=tl>>8;
			talk2[2]=tl&0xFF;
			talk2[3]=chars[currchar[s]].ser1;
			talk2[4]=chars[currchar[s]].ser2;
			talk2[5]=chars[currchar[s]].ser3;
			talk2[6]=chars[currchar[s]].ser4;
			talk2[7]=chars[currchar[s]].id1;
			talk2[8]=chars[currchar[s]].id2;
			talk2[9]=buffer[s][3]; // Type
			talk2[10]=buffer[s][4];
			talk2[11]=buffer[s][5];
			talk2[12]=buffer[s][6];
			talk2[13]=chars[currchar[s]].fonttype;
			talk2[14]=buffer[s][8];
			talk2[15]=buffer[s][9];
			talk2[16]=buffer[s][10];         
			Network->xSend(s, talk2, 18, 0);
			Network->xSend(s, chars[currchar[s]].name, 30, 0);   
			for(i=-1;i<(tl-48)-2;i+=2)	// why aren't we making use of the text variable?
			{
				Network->xSend(s, &buffer[s][i+13], 2, 0);   
			}      
			if (talk2[9]==0)
			{
				chars[currchar[s]].saycolor1=buffer[s][4];
				chars[currchar[s]].saycolor2=buffer[s][5];
			}
			if (talk2[9]==2)
			{
				chars[currchar[s]].emotecolor1=buffer[s][4];
				chars[currchar[s]].emotecolor2=buffer[s][5];
				return;
			}
			if (server_data.log==2) //Logging -- Zippy
			{
				char temp2[512];
				sprintf(temp2, "%s.log",chars[currchar[s]].name);
				sprintf(temp,"%s [%x %x %x %x] [%i]: %s\n",chars[currchar[s]].name,chars[currchar[s]].ser1,chars[currchar[s]].ser2,chars[currchar[s]].ser3,chars[currchar[s]].ser4,chars[currchar[s]].account, &buffer[s][8]);
				savelog(temp,temp2);
			}
			
			char search1[10];
			strcpy( search1, "GUARDS" );
			char *response1;
			char str[256];
			strcpy( str, nonuni );
			strupr( str );
			response1 = ( strstr( str, search1 ) );
			if( response1 )
				callguards( currchar[s] );
			
            Boats->Speech(s, (unsigned char *)nonuni);//Boats
			house_speech(s, (unsigned char *)nonuni); //houses crackerjack 8/12/99			
			for (i=0;i<now;i++)
			{
				//if ((inrange1(i, s)&&perm[i]))
				// AntiChrist - don't check line of sight for talking!!!
				// we can talk normally through walls, can we?  That's new to me (Abaddon)
				if ( perm[i] && inrange1(i, s) )//&&line_of_sight(s, chars[currchar[s]].x, chars[currchar[s]].y, chars[currchar[s]].z, chars[currchar[i]].x, chars[currchar[i]].y, chars[currchar[i]].z, WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING)) //AntiChrist
				{
					Network->xSend(i, talk2, 18, 0);
					Network->xSend(i, chars[currchar[s]].name, 30, 0);          
					//					if((chars[currchar[i]].priv&0x01)||(chars[currchar[i]].priv&0x80));// GM/Counselors can see ghosts talking always  Seers?
					//					else if(chars[currchar[i]].spiritspeaktimer==0) // If the spiritspeaktimer is set, you can talk to the dead
					// modified by AntiChrist
					if(!( chars[currchar[i]].priv&1 ) && !( chars[currchar[i]].priv&0x80 ) && chars[currchar[i]].spiritspeaktimer == 0 )// GM/Counselors can see ghosts talking always Seers?
					{
						if(chars[currchar[s]].dead==1&&chars[currchar[i]].dead==0)  // Ghost can talk normally to other ghosts
						{
							// -2: dont override /0 /0 terminator !
							for(j=13;j<((buffer[s][1]<<8)+buffer[s][2])-2;j=j+2) // bugfix (ghost-speak crash) by lord binary
							{
								if(buffer[s][j]==32) {;}
								else if(buffer[s][j]%2)     
									buffer[s][j]='O';
								else
									buffer[s][j]='o';
							}					 
						}
					}
					else if( chars[currchar[i]].race != chars[currchar[s]].race && chars[currchar[i]].priv&0x01 != 0x01 )
					{
						if( Skills->CheckSkill( currchar[i], SPIRITSPEAK, Races->getLanguageMin( chars[currchar[s]].race ), 1000 ) != 1 )
						{
							for(j=13;j<((buffer[s][1]<<8)+buffer[s][2])-2;j=j+2) // bugfix (ghost-speak crash) by lord binary
							{
								if(buffer[s][j]==32) {;}
								else if(buffer[s][j]%2)     
									buffer[s][j]='O';
								else
									buffer[s][j]='o';
							}					 
						}
					}
					for(j=-1;j<(tl-48)-2;j+=2)
					{
						Network->xSend(i, &buffer[s][j+13], 2, 0);   
					}      
				}
			}
			if(chars[currchar[s]].dead==1)  // this makes it so npcs do not respond to dead people
				return;
			
			i=0;
			found=0;
			x1=chars[currchar[s]].x;
			y1=chars[currchar[s]].y;
			StartGrid=mapRegions->StartGrid(chars[currchar[s]].x,chars[currchar[s]].y);
			getcell=mapRegions->GetCell(chars[currchar[s]].x,chars[currchar[s]].y);
						
			increment=0;
			ab=0;
			for (checkgrid=StartGrid+(increment*mapRegions->GetColSize());increment<3;increment++, checkgrid=StartGrid+(increment*mapRegions->GetColSize()))
			{	
				for (a=0;a<3;a++)
				{					
					mapitemptr=-1;
					mapitem=-1;
					mapchar=-1;
					do //check all items in this cell
					{
						mapchar=-1;
						mapitemptr=mapRegions->GetNextItem(checkgrid+a, mapitemptr);
						if (mapitemptr==-1) break;
						mapitem=mapRegions->GetItem(checkgrid+a, mapitemptr);
						if(mapitem>999999) mapchar=mapitem-1000000;
						if (mapitem!=-1 && mapitem>=1000000)
						{
						    i=mapchar;			
							if ((i!=currchar[s]) && (chars[i].npc))
							{
					          x2=chars[i].x;
					          y2=chars[i].y;
							  if ( (abs(x1-x2)<=2) && (abs(y1-y2)<=2) ) 
							  {
								  found=i+1;
								  ab=1;
								  break;
							  }
							}
						}
					} while (mapitemptr != -1 && !found);
					if (found) break;
				}
				if (found) break;
			}

			if ((found!=0)&&(chars[found-1].speech))
			{
				responsevendor(s);
				found--;
				for (i=0;i<strlen(&nonuni[0]);i++) nonuni[i]=toupper(nonuni[i]);
//				talkingto[found]=currchar[s];
				openscript("speech.scp");
				sprintf(sect, "SPEECH %i", chars[found].speech);
				if (!i_scripts[speech_script]->find(sect))
				{
					closescript();
					return;
				}
				match=0;
				strcpy(sect, "NO DEFAULT TEXT DEFINED");
				do
				{
					read2();
					if (script1[0]!='}')
					{
						if (!(strcmp("DEFAULT",script1)))
						{
							strcpy(sect, script2);
						}
						if (!(strcmp("ON",script1)))
						{
							j=0;
							do
							{
								m2=1;
								sml=strlen(script2);
								if (strlen(&nonuni[j])<sml)
								{
									//         sml=strlen(&buffer[s][8+j]);
									m2=0;
								}
								else
									for (i=0;i<sml;i++)
									{
										if (nonuni[i+j]!=toupper(script2[i]))
										{
											m2=0;
										}
									}
									j++;
							}
							while ((j<strlen(&nonuni[0]))&&(m2==0));
							if (m2==1) match=1;
						}
						if (!(strcmp("SAY",script1)))
						{
							if (match==1)
							{
								npctalk(s, found, script2, 0);
								match=2;
							}
						}
#ifdef UOXPERL
						if(!strcmp("PERL", script1)) // Run a Perl Command (crackerjack 8/1/99)
						{
							if(match==1)
							{ 
								_uoxperl_func(script2, chars[found].serial, chars[s].serial, &nonuni[0]);
								match=2;
							}
						}
#endif
					}
				}
				while (script1[0]!='}');
				if (match==0)
				{
					npctalk(s, found, sect, 0);
				}
				closescript();
			}
  }
 }
}
