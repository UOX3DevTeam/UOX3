//------------------------------------------------------------------------
//  commands.cpp
//
//------------------------------------------------------------------------
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 1998 - 2001 by Marcus Rating (Cironian)
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

#include <uox3.h>
#include <debug.h>
#include <cmdtable.h>
#define DBGFILE "commands.cpp"


// inline command to do targeting - coulda made this a
// macro but this is pretty much as fast...
static inline void _do_target(int s, TARGET_S *ts) {
	target(s, ts->a1, ts->a2, ts->a3, ts->a4, ts->txt);
	return;
}

/* extensively modified 8/2/99 crackerjack@crackerjack.net -
 * see cmdtable.cpp for more details */
void cCommands::Command( UOXSOCKET s ) // Client entered a command like 'ADD
{

	int i=9;
	char *comm;
	char nonuni[512];
	cmd_offset=9; //LB bugfix
	
	int y;
	
	if(!(chars[currchar[s]].unicode))	
	{
		while (buffer[s][i]!='\x00')
		{
			tbuffer[i]=buffer[s][i];
			buffer[s][i]=toupper(buffer[s][i]);
			i++;
		}
		tbuffer[i]='\x00';
		cline = (char *)&buffer[s][8];
		splitline();
		if (tnum<1) return;
		comm = (char *)&buffer[s][9]; 
	}
	else
	{
		i=0;
		cmd_offset=14;
		// we will have to convert from unicode to non-unicode
		for (i=13;i<(buffer[s][1]<<8)+buffer[s][2];i=i+2)
		{
			nonuni[(i-13)/2]=buffer[s][i];
		} 
		i=14;
		while (nonuni[i-13]!='\x00')
		{
			tbuffer[i]=nonuni[i-13];
			nonuni[i-13]=toupper(nonuni[i-13]);
			i++;
		} 
		tbuffer[i]='\x00'; 
		cline = &nonuni[1];
		splitline();
		if (tnum<1) return; 
		comm=&nonuni[1];
	} 

	i=0; y=-1;
	while((command_table[i].cmd_name)&&(y==-1)) 
	{
		if(!(strcmp(command_table[i].cmd_name, comm))) 
			y=i;
		i++;
	}

	if(y==-1) 
	{
		sysmessage(s, "Unrecognized command.");
		return;
	} 
	else
	{
		bool clearance = false;
		clearance = ( chars[currchar[s]].commandLevel >= command_table[y].cmd_priv_m );
//		bool gmclear = false;
//		bool counselorclear = false;
//		gmclear = ( command_table[y].cmd_priv_m <= 2 && chars[currchar[s]].priv&0x01 );
//		counselorclear = ( command_table[y].cmd_priv_m <= 1 && chars[currchar[s]].priv&0x80 );

//		if((chars[currchar[s]].account!=0)&&(command_table[y].cmd_priv_m!=255)&&
//			(!(chars[currchar[s]].priv3[command_table[y].cmd_priv_m]&
//			(0-0xFFFFFFFF<<command_table[y].cmd_priv_b))))
//		if( !gmclear && !counselorclear && command_table[y].cmd_priv_m != 0 )
		if( !clearance )
		{
			sysmessage(s, "Access denied.");
			return;
		}

		switch(command_table[y].cmd_type) {
		case CMD_FUNC:
			(*((CMD_EXEC)command_table[y].cmd_extra)) (s);
			break;
		case CMD_ITEMMENU:
			itemmenu(s, (int)command_table[y].cmd_extra);
			break;
		case CMD_TARGET:
			_do_target(s, (TARGET_S *)command_table[y].cmd_extra);
			break;
		case CMD_TARGETX:
			if(tnum==2) 
			{
				addx[s]=makenumber(1);
				_do_target(s, (TARGET_S *)command_table[y].cmd_extra);
			} 
			else 
			{
				sysmessage(s, "This command takes one number as an argument.");
			}
			break;
		case CMD_TARGETXY:
			if(tnum==3) 
			{
				addx[s]=makenumber(1);
				addy[s]=makenumber(2);
				_do_target(s, (TARGET_S *)command_table[y].cmd_extra);
			} 
			else 
			{
				sysmessage(s, "This command takes two numbers as arguments.");
			}
			break;
		case CMD_TARGETXYZ:
			if(tnum==4) 
			{
				addx[s]=makenumber(1);
				addy[s]=makenumber(2);
				addz[s]=makenumber(3);
				_do_target(s, (TARGET_S *)command_table[y].cmd_extra);
			} 
			else 
			{
				sysmessage(s, "This command takes three numbers as arguments.");
			}
			break;
		case CMD_TARGETHX:
			if(tnum==2) 
			{
				addx[s]=hexnumber(1);
				_do_target(s, (TARGET_S *)command_table[y].cmd_extra);
			} 
			else 
			{
				sysmessage(s, "This command takes one hex number as an argument.");
			}
			break;
		case CMD_TARGETHXY:
			if(tnum==3) 
			{
				addx[s]=hexnumber(1);
				addy[s]=hexnumber(2);
				_do_target(s, (TARGET_S *)command_table[y].cmd_extra);
			} 
			else 
			{
				sysmessage(s, "This command takes two hex numbers as arguments.");
			}
			break;
		case CMD_TARGETHXYZ:
			if(tnum==4) 
			{
				addx[s]=hexnumber(1);
				addy[s]=hexnumber(2);
				addz[s]=hexnumber(3);
				_do_target(s, (TARGET_S *)command_table[y].cmd_extra);
			} 
			else 
			{
				sysmessage(s, "This command takes three hex numbers as arguments.");
			}
			break;
		case CMD_TARGETID1:
			if(tnum==2) 
			{
				addid1[s]=makenumber(1);
				_do_target(s, (TARGET_S *)command_table[y].cmd_extra);
			} 
			else 
			{
				sysmessage(s, "This command takes one number as an argument.");
			}
			break;
		case CMD_TARGETID2:
			if(tnum==3) 
			{
				addid1[s]=makenumber(1);
				addid2[s]=makenumber(2);
				_do_target(s, (TARGET_S *)command_table[y].cmd_extra);
			} 
			else 
			{
				sysmessage(s, "This command takes two numbers as arguments.");
			}
			break;
		case CMD_TARGETID3:
			if(tnum==4) 
			{
				addid1[s]=makenumber(1);
				addid2[s]=makenumber(2);
				addid3[s]=makenumber(3);
				_do_target(s, (TARGET_S *)command_table[y].cmd_extra);
			} 
			else 
			{
				sysmessage(s, "This command takes three numbers as arguments.");
			}
			break;
		case CMD_TARGETID4:
			if(tnum==5) 
			{
				addid1[s]=makenumber(1);
				addid2[s]=makenumber(2);
				addid3[s]=makenumber(3);
				addid4[s]=makenumber(4);
				_do_target(s, (TARGET_S *)command_table[y].cmd_extra);
			} 
			else 
			{
				sysmessage(s, "This command takes four numbers as arguments.");
			}
			break;
		case CMD_TARGETHID1:
			if(tnum==2) 
			{
				addid1[s]=hexnumber(1);
				_do_target(s, (TARGET_S *)command_table[y].cmd_extra);
			} 
			else 
			{
				sysmessage(s, "This command takes one hex number as an argument.");
			}
			break;
		case CMD_TARGETHID2:
			if(tnum==3) 
			{
				addid1[s]=hexnumber(1);
				addid2[s]=hexnumber(2);
				_do_target(s, (TARGET_S *)command_table[y].cmd_extra);
			} 
			else 
			{
				sysmessage(s, "This command takes two hex numbers as arguments.");
			}
			break;
		case CMD_TARGETHID3:
			if(tnum==4) 
			{
				addid1[s]=hexnumber(1);
				addid2[s]=hexnumber(2);
				addid3[s]=hexnumber(3);
				_do_target(s, (TARGET_S *)command_table[y].cmd_extra);
			} 
			else 
			{
				sysmessage(s, "This command takes three hex numbers as arguments.");
			}
			break;
		case CMD_TARGETHID4:
			if(tnum==5) 
			{
				addid1[s]=hexnumber(1);
				addid2[s]=hexnumber(2);
				addid3[s]=hexnumber(3);
				addid4[s]=hexnumber(4);
				_do_target(s, (TARGET_S *)command_table[y].cmd_extra);
			} 
			else 
			{
				sysmessage(s, "This command takes four hex numbers as arguments.");
			}
			break;
		case CMD_TARGETTMP:
			if(tnum==2) 
			{
				tempint[s]=makenumber(1);
				_do_target(s, (TARGET_S *)command_table[y].cmd_extra);
			} 
			else 
			{			
				sysmessage(s, "This command takes a number as an argument.");
			}
			break;
		case CMD_TARGETHTMP:
			if(tnum==2) 
			{
				tempint[s]=hexnumber(1);			
				_do_target(s, (TARGET_S *)command_table[y].cmd_extra);
			} 
			else 
			{
				sysmessage(s, "This command takes a hex number as an argument.");
			}
			break;
		default:
			sysmessage(s, "BUG: Command has a bad command type set!");
			break;
		}
		return;
	}

	sysmessage(s, "BUG: Should never reach end of command() function!");
}


void cCommands::MakeShop(int c)
{
	int i,n,serial,serhash,ci, skip=0;
	chars[c].shop=1;
  serial=chars[c].serial;
  serhash=serial%HASHMAX;
  for (ci=0;ci<contsp[serhash].max;ci++)
  {
    i=contsp[serhash].pointer[ci];
	if (i!=-1)
    if ((items[i].contserial==serial) && (items[i].layer==0x1A))
    {
      skip=1;
      break;
    }
  }
	if (skip==0)
	{
		n=Items->SpawnItem(calcSocketFromChar(c),c,1,"#",0,0x2A,0xF8,0,0,0,0);
		if( n > -1 )
		{
			setserial(n,c,4);
			items[n].layer=0x1A;
			items[n].type=1;
			items[n].priv=items[n].priv|0x02;
		}
	}
	skip=0;
	
  serial=chars[c].serial;
  serhash=serial%HASHMAX;
  for (ci=0;ci<contsp[serhash].max;ci++)
  {
    i=contsp[serhash].pointer[ci];
	if (i!=-1)
    if ((items[i].contserial==serial) && (items[i].layer==0x1B))
    {
      skip=1;
      break;
    }
  }
	if (skip==0)
	{
		n=Items->SpawnItem(calcSocketFromChar(c),c,1,"#",0,0x2A,0xF8,0,0,0,0);
		if( n > -1 )
		{
			setserial(n,c,4);
			items[n].layer=0x1B;
			items[n].type=1;
			items[n].priv=items[n].priv|0x02;
		}
	}
  serial=chars[c].serial;
  serhash=serial%HASHMAX;
  for (ci=0;ci<contsp[serhash].max;ci++)
  {
    i=contsp[serhash].pointer[ci];
	if (i!=-1)
    if ((items[i].contserial==serial) && (items[i].layer==0x1C))
    {
      skip=1;
      break;
    }
  }
	if (skip==0)
	{
		n=Items->SpawnItem(calcSocketFromChar(c),c,1,"#",0,0x2A,0xF8,0,0,0,0);
		if( n > -1 )
		{
			setserial(n,c,4);
			items[n].layer=0x1C;
			items[n].type=1;
			items[n].priv=items[n].priv|0x02;
		}
	}
}

void cCommands::NextCall(int s, int type)
{
  // Type is the same as it is in showgmqueue()
	
  int i,j,serial;
  int x=0;
	
  if(chars[currchar[s]].callnum!=0)
  {
    donewithcall(s, type);
  }
  if(type==1) //Player is a GM
  {
    for(i=1;i<MAXPAGES;i++)
    {
      if(gmpages[i].handled==0)
      {
       	serial=calcserial(gmpages[i].ser1,gmpages[i].ser2,gmpages[i].ser3,gmpages[i].ser4);
		j = calcCharFromSer( serial );
		if(j!=-1)
		{
            sysmessage(s,"");
            sprintf(temp,"Transporting to next call: %s", gmpages[i].name);
            sysmessage(s,temp);
            sprintf(temp,"Problem: %s.", gmpages[i].reason);
            sysmessage(s,temp);
            sprintf(temp,"Serial number %x %x %x %x", gmpages[i].ser1,
							gmpages[i].ser2, gmpages[i].ser3, gmpages[i].ser4);
            sysmessage(s,temp);
            sprintf(temp,"Paged at %s.", gmpages[i].timeofcall);
            sysmessage(s,temp);
            gmpages[i].handled=1;
			mapRegions->RemoveItem(currchar[s]+1000000); //LB
            chars[currchar[s]].x=chars[j].x;
            chars[currchar[s]].y=chars[j].y;
			mapRegions->AddItem(currchar[s]+1000000);
            chars[currchar[s]].dispz=chars[currchar[s]].z=chars[j].z;
            chars[currchar[s]].callnum=i;
            teleport(currchar[s]);
            x++;
            //break;
          }// if
        //} for
        if(x>0)break;
      }// if
    }// for
    if(x==0) sysmessage(s,"The GM queue is currently empty");
  } //end first IF
  else //Player is only a counselor
  {
    x=0;
    for(i=1;i<MAXPAGES;i++)
    {
      if(counspages[i].handled==0)
      {
		serial = calcserial(counspages[i].ser1,counspages[i].ser2,counspages[i].ser3,counspages[i].ser4);
		j = calcCharFromSer( serial );
		if(j!=-1)
		{
            sysmessage(s,"");
            sprintf(temp,"Transporting to next call: %s", counspages[i].name);
            sysmessage(s,temp);
            sprintf(temp,"Problem: %s.", counspages[i].reason);
            sysmessage(s,temp);
            sprintf(temp,"Serial number %x %x %x %x", counspages[i].ser1,
							counspages[i].ser2, counspages[i].ser3, counspages[i].ser4);
            sysmessage(s,temp);
            sprintf(temp,"Paged at %s.", counspages[i].timeofcall);
            sysmessage(s,temp);
            gmpages[i].handled=1;
			mapRegions->RemoveItem(currchar[s]+1000000);
            chars[currchar[s]].x=chars[j].x;
            chars[currchar[s]].y=chars[j].y;
			mapRegions->AddItem(currchar[s]+1000000);
            chars[currchar[s]].dispz=chars[currchar[s]].z=chars[j].z;
            chars[currchar[s]].callnum=i;
            teleport(currchar[s]);
            x++;
            break;
          }// if
        }// else
        if(x>0)break;
      }//for
    //}
    if(x==0) sysmessage(s,"The Counselor queue is currently empty");
  }//if
}

void cCommands::RepairWorld(int s) //Morrolan repairworldfile
{
        int hits=0;
		int pcs=0;
        char temp[100];
		char temp2[100];
		tile_st tile;

        sysbroadcast( "Repairing worldfile, expect LAG!");
		int i;
        for(i=0;i<itemcount;i++)
        {
			Map->SeekTile((items[i].id1<<8)+items[i].id2, &tile);
			if(tile.flag2&0x08)
			{
				if (items[i].pileable==0) 
				{
					items[i].pileable=1;
					hits++;
				}
			}
        }
		tempeffectsoff();
		for(int j=0;j<charcount;j++)
		{
			if(chars[j].npc==1)
			{
				if(chars[j].st>=30000) chars[j].st=100;
				if(chars[j].in>=30000) chars[j].in=100;
				if(chars[j].dx>=30000) chars[j].dx=100;
				if(chars[j].hidamage>=100) chars[j].hidamage=100;
				if(chars[j].hp>=30000) chars[j].hp=100;
				if(chars[j].mn>=30000) chars[j].mn=100;
				if(chars[j].stm>=30000) chars[j].stm=100;
				if(chars[j].lodamage>=30000) chars[j].lodamage=1;
				if(chars[j].poison>5) chars[j].poison=0;
//				if(chars[j].name=="John Doe") 
				if( !strcmp( chars[j].name, "John Doe" ) )
				{
					removeitem[1] = chars[i].ser1;
					removeitem[2] = chars[i].ser2;
					removeitem[3] = chars[i].ser3;
					removeitem[4] = chars[i].ser4;
					for (int k=0;k<now;k++)
					{
						Network->xSend(k, removeitem, 5, 0);
					}
					Npcs->DeleteChar(i);
				}
				pcs++;
			}
			else if (chars[j].npc==0)
			{
				if(chars[j].st>100) chars[j].st=100;
				if(chars[j].in>100) chars[j].in=100;
				if(chars[j].dx>100) chars[j].dx=100;
				if(chars[j].stm>100) chars[j].stm=100;
				if(chars[j].mn>100) chars[j].mn=100;
				if(chars[j].hp>100) chars[j].hp=100;
				if(chars[j].st<10) chars[j].st=10;
				if(chars[j].hp<10) chars[j].hp=10;
				if(chars[j].in<10) chars[j].in=10;
				if(chars[j].mn<10) chars[j].mn=10;
				if(chars[j].dx<10) chars[j].dx=10;
				if(chars[j].stm<10) chars[j].stm=10;
				chars[j].hunger=0;
				pcs++;
			}
		}
				
        gcollect();
        sysmessage(s, "Done.");
		if (hits > 0) sprintf(temp, "%i items needed to be fixed!",hits);
        sysmessage(s, temp);
		sprintf(temp2, "%i characters were checked.",pcs);
		sysmessage(s, temp2);
}


void cCommands::KillSpawn(int s, int r)  //courtesy of Revana
{
	int i;
	int killed=0;

	char temp[512];

	sysmessage(s,"Killing spawn, this may cause lag...");
	for(i=0;i<charcount;i++)
	{
		if( chars[i].spawn1 < 0x40 && chars[i].spawn3 == r && chars[i].spawn2 == 1 )	// spawn2 == 1 if spawned by region spawn
		{
			bolteffect(i);
			soundeffect2(i, 0x00, 0x29);
			Npcs->DeleteChar(i);
            killed++;
		}
	}
	gcollect();
	sysmessage(s, "Done.");
	sprintf(temp, "%i of Spawn %i have been killed.",killed,r);
	sysmessage(s, temp);
}

void cCommands::RegSpawnMax (int s, int r)
{
	int i;
	unsigned int currenttime = uiCurrentTime;
	
	int spawn = ( spawnregion[r].max - spawnregion[r].current );
	if( spawn > 250 )
	{
		sysmessage( s, "Attempt to spawn more than 250 items/NPCs denied.  Spawning 250 instead." );
		spawn = 250;
	}
	char temps[650];
	sprintf( temps, "Region %s [%d] is Spawning %d items/NPCs, this will cause some lag.", spawnregion[r].name, r, spawn );
	sysbroadcast( temps );

	for( i = 1; i < spawn; i++ )
	{
		doregionspawn(r);
	}	
	
	spawnregion[r].nexttime = (int)( currenttime + ( CLOCKS_PER_SEC * 60 * RandomNum( spawnregion[r].mintime, spawnregion[r].maxtime ) ) );
	sprintf( temps, "Done. %d total NPCs/items spawned in Spawnregion %s [%d].",spawn, spawnregion[r].name, r );
	sysmessage( s, temps );
}

void cCommands::RegSpawnNum (int s, int r, int n)
{
	int i, spawn=0;
	unsigned int currenttime=uiCurrentTime;
	char temps[650];
	if( n > 250 )
	{
		sysmessage( s, "Attempt to spawn more than 250 items/NPCs denied.  Try /REGSPAWN r n<250 instead." );
		return;
	}//if
	else
	{
		spawn = (spawnregion[r].max-spawnregion[r].current);
		if( spawn < n ) 
		{
			sprintf( temps, "%d too many for region %s [%d], spawning %d to reach MAX:%d instead.",n, spawnregion[r].name, r, spawn, spawnregion[r].max );
			sysmessage( s, temps );
		}
		else 
			spawn = n;
		sprintf( temps, "Region %s [%d] is Spawning: %d NPCs/items, this will cause some lag.", spawnregion[r].name, r, spawn );
		sysbroadcast( temps );

		for( i = 1; i < spawn; i++ )
		{
			doregionspawn( r );
		}
	
		spawnregion[r].nexttime = (int)( currenttime + ( CLOCKS_PER_SEC * 60 * RandomNum( spawnregion[r].mintime, spawnregion[r].maxtime ) ) );
		sprintf( temps, "Done. %d total NPCs/items spawned in Spawnregion %s [%d].",spawn, spawnregion[r].name, r );
		sysmessage( s, temps );
	}
}//regspawnnum

void cCommands::KillAll(int s, int percent, unsigned char* sysmsg)
{
	sysmessage( s, "Killing all characters, this may cause some lag..." );
	sysbroadcast( (char *)sysmsg );
	for( int i = 0; i < charcount; i++ )
	{
		if( !( chars[i].priv&1 ) )
		{
			if( rand()%100 + 1 <= percent )
			{
				bolteffect( i );
				soundeffect2( i, 0x00, 0x29 );
				deathstuff( i );
			}
		}
	}
	sysmessage(s, "Done.");
}

//o---------------------------------------------------------------------------o
//|   Function -  void cpage(int s,char *reason)
//|   Date     -  UnKnown
//|   Programmer  -  UnKnown  (Touched tabstops by EviLDeD Dec 23, 1998)
//o---------------------------------------------------------------------------o
//|   Purpose     -  
//o---------------------------------------------------------------------------o
void cCommands::CPage(int s, char *reason) // Help button (Calls Counselor Call Menus up)
{
	int i, a1, a2, a3, a4, x;
	int x2=0;
	
	x=0;
	a1=chars[currchar[s]].ser1;
	a2=chars[currchar[s]].ser2;
	a3=chars[currchar[s]].ser3;
	a4=chars[currchar[s]].ser4;
	
	for(i=1;i<MAXPAGES;i++)
	{
		if(counspages[i].handled==1)
		{
			counspages[i].handled=0;
			strcpy(counspages[i].name,chars[currchar[s]].name);
			strcpy(counspages[i].reason,reason);
			counspages[i].ser1=a1;
			counspages[i].ser2=a2;
			counspages[i].ser3=a3;
			counspages[i].ser4=a4;
			time_t current_time = time(0);
			struct tm *local = localtime(&current_time);
			sprintf(counspages[i].timeofcall, "%02d:%02d:%02d", local->tm_hour, local->tm_min, local->tm_sec);
			sprintf(temp,"%s [%d][%d][%d][%d] called at %s, %s",counspages[i].name,a1,a2,a3,a4,counspages[i].timeofcall,counspages[i].reason);
			if(heartbeat) Writeslot(temp);
			chars[currchar[s]].playercallnum=i;
			chars[currchar[s]].pagegm=2;
			x2++;
			break;
		}
	}
	if(x2==0)
	{
		sysmessage(s,"The Counselor Queue is currently full. Contact the shard operator");
		sysmessage(s,"and ask them to increase the size of the queue.");
	}
	else
	{
		if(strcmp(reason,"OTHER"))
		{
			chars[currchar[s]].pagegm=0;
			sprintf(temp, "Counselor Page from %s [%x %x %x %x]: %s",
				chars[currchar[s]].name, a1, a2, a3, a4, reason);
			for (i=0;i<now;i++) if (chars[currchar[i]].priv&0x80)
			{
				x=1;
				sysmessage(i, temp);
			}
			if (x==1)
			{
				sysmessage(s, "Available Counselors have been notified of your request.");
			}
			else sysmessage(s, "There was no Counselor available to take your call.");
		}
		else sysmessage(s,"Please enter the reason for your Counselor request");
	}
}

//o---------------------------------------------------------------------------o
//|   Function :  void gmpage(int s,char *reason)
//|   Date     :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Help button (Calls GM Call Menus up)
//o---------------------------------------------------------------------------o
void cCommands::GMPage(int s, char *reason)
{
	int i, a1, a2, a3, a4, x=0;
	int x2=0;
	
	a1=chars[currchar[s]].ser1;
	a2=chars[currchar[s]].ser2;
	a3=chars[currchar[s]].ser3;
	a4=chars[currchar[s]].ser4;
	
	for(i=1;i<MAXPAGES;i++)
	{
		if(gmpages[i].handled==1)
		{
			gmpages[i].handled=0;
			strcpy(gmpages[i].name,chars[currchar[s]].name);
			strcpy(gmpages[i].reason,reason);
			gmpages[i].ser1=a1;
			gmpages[i].ser2=a2;
			gmpages[i].ser3=a3;
			gmpages[i].ser4=a4;
			time_t current_time = time(0);
			struct tm *local = localtime(&current_time);
			sprintf(gmpages[i].timeofcall, "%02d:%02d:%02d", local->tm_hour, local->tm_min, local->tm_sec);
			sprintf(temp,"%s [%d][%d][%d][%d] called at %s, %s",gmpages[i].name,a1,a2,a3,a4,gmpages[i].timeofcall,gmpages[i].reason);
			if(heartbeat) Writeslot(temp);
			chars[currchar[s]].playercallnum=i;
			chars[currchar[s]].pagegm=1;
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
		if(strcmp(reason,"OTHER"))
		{
			chars[currchar[s]].pagegm=0;
			sprintf(temp, "Page from %s [%x %x %x %x]: %s",
				chars[currchar[s]].name, a1, a2, a3, a4, reason);
			for (i=0;i<now;i++) if (chars[currchar[i]].priv&0x20)
			{
				x=1;
				sysmessage(i, temp);
			}
			if (x==1)
			{
				sysmessage(s, "Available Game Masters have been notified of your request.");
			}
			else sysmessage(s, "There was no Game Master available to take your call.");
		}
		else sysmessage(s,"Please enter the reason for your GM request");
	}
}

void cCommands::DyeItem(int s) // Rehue an item
{
	int body, b, k;
	unsigned char c1, c2;
	int serial=calcserial(buffer[s][1],buffer[s][2],buffer[s][3],buffer[s][4]);
	int i = calcItemFromSer( serial );
	if(i!=-1)
	{
			c1=buffer[s][7];
			c2=buffer[s][8];
			
               
			   if(!(dyeall[s]))
               {
				 if ((((c1<<8)+c2)<0x0002) ||
				    	(((c1<<8)+c2)>0x03E9))
				 {
					c1=0x03;
					c2=0xE9;
				 }
			   }
		
           	b=((((c1<<8)+c2)&0x4000)>>14)+((((c1<<8)+c2)&0x8000)>>15);	       
			if (!b)
            {

              items[i].color1=c1;
			  items[i].color2=c2;
			}

			if (((c1<<8)+c2)==17969)
			{
				items[i].color1 = c1;
				items[i].color2 = c2;
			}

			RefreshItem( i ); // AntiChrist
			
			soundeffects( s, 0x02, 0x3e, true ); // plays the dye sound, LB
			return;
	}

	i = calcCharFromSer( serial );
	if(i!=-1)
	{
		if( !(chars[currchar[s]].priv&1 ) ) return; // Only gms dye characters
		  k=(buffer[s][7]<<8)+buffer[s][8];


		 body=(chars[i].id1<<8)+chars[i].id2;
         b=k&0x4000; 

		 if( ( ( k>>8 ) < 0x80 ) && body >= 0x0190 && body <= 0x0193 ) k+= 0x8000;

		 if (b==16384 && (body >=0x0190 && body<=0x03e1)) k=0xf000; // but assigning the only "transparent" value that works, namly semi-trasnparency.

         if (k!=0x8000) 
		 {	
		 
			chars[i].skin1 = (unsigned char)(k>>8);
			chars[i].skin2 = (unsigned char)(k%256);
			chars[i].xskin1 = (unsigned char)(k>>8);
			chars[i].xskin2 = (unsigned char)(k%256);
			updatechar(i);
		  	//break;
         } 
		//} for
	}
	soundeffects( s, 0x02, 0x3e, true ); // plays the dye sound, LB
}


void cCommands::SetItemTrigger( UOXSOCKET s )
{
  int serial = calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
  int i = calcItemFromSer( serial );
  if (i!=-1)
  {
		sysmessage(s,"Item triggered");
		items[i].trigger=addx[s];
  }
}

void cCommands::SetTriggerType( UOXSOCKET s )
{
  int serial = calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
  int i = calcItemFromSer( serial );
  if (i!=-1)
  {
		sysmessage(s,"Trigger type set");
		items[i].trigtype=addx[s];
  }
}

void cCommands::SetTriggerWord( UOXSOCKET s )
{
  int serial = calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
  int i = calcCharFromSer( serial );
  if (i!=-1)
  {
		sysmessage(s,"Trigger word set");
		safeCopy(chars[i].trigword,xtext[s], MAX_TRIGWORD);
  }
}

void cCommands::AddHere(int s, char z)
{
	int c, pileable = 0;
	tile_st tile;
	
	Map->SeekTile((addid1[s]<<8)+addid2[s], &tile);
	if (tile.flag2&0x08) pileable=1;
	
	c=Items->SpawnItem(s, 1, "#", pileable, addid1[s], addid2[s], 0, 0, 0, 0);
	if( c > -1 )
	{
		items[c].x=chars[currchar[s]].x;
		items[c].y=chars[currchar[s]].y;
		items[c].z=z;

		mapRegions->AddItem(c);

		if (items[c].contserial==-1) mapRegions->AddItem(c); //add to region item list
		items[c].doordir=0;
		items[c].priv=0;
		RefreshItem( c ); // AntiChrist
	}
	addid1[s]=0;
	addid2[s]=0;
}


void cCommands::SetNPCTrigger( UOXSOCKET s )
{
  int serial = calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
  int i = calcCharFromSer( serial );
  if (i!=-1)
  {
    sysmessage(s,"NPC triggered");
    chars[i].trigger=addx[s];
  }
}

void cCommands::MakePlace(int s, int i) // Decode a teleport location number into X/Y/Z
{
	int x=0, y=0, z=0;
	
	openscript("location.scp");
	sprintf(temp, "LOCATION %i", i);
	if (i_scripts[location_script]->find(temp))
	{
		do
		{
			read2();
			if (!(strcmp(script1,"X")))
			{
				x=str2num(script2);
			}
			if (!(strcmp(script1,"Y")))
			{
				y=str2num(script2);
			}
			if (!(strcmp(script1,"Z")))
			{
				z=str2num(script2);
			}
		}
		while (strcmp(script1,"}"));
	}
	addx[s]=x;
	addy[s]=y;
	addz[s]=z;
	closescript();
}


void cCommands::DupeItem(UOXSOCKET s, int i, int amount)
{
	int p, c;
	p=packitem(currchar[s]);
	if( p == -1 ) return;
	if (items[i].corpse==0)
	{
		c=Items->MemItemFree ();
		Items->InitItem(c,0);
		//Tauriel - Crap... another one that can't use the standard stuff. (2nd so far)
		memcpy(&items[c], &items[i], sizeof(item_st));
		splitSerial(itemcount2, items[c].ser1, items[c].ser2, items[c].ser3, items[c].ser4);
		items[c].serial=itemcount2;
		// This is... bad, to say the least.  It doesn't unset itself via the old serial
		// I don't think (needs testing) that we really need to set it's serial some more
		// memitemfree and initmem get it a serial already
		setptr(&itemsp[itemcount2%HASHMAX], c); //set item in pointer array
		itemcount2++;
		setserial(c,p,1);
		items[c].amount=amount;
		if (items[c].ownserial!=-1) setptr(&ownsp[items[c].ownserial%HASHMAX],c);
		if( items[c].spawnserial != -1 && items[c].spawnserial != 0 ) 
			setptr(&spawnsp[items[c].spawnserial%HASHMAX],c);
		
		if (c==itemcount) itemcount++;
		itemcount2++;
		RefreshItem( c ); // AntiChrist
	}
}

void cCommands::ShowGMQue(int s, int type) // Shows next unhandled call in the GM queue
{
	
	// Type is 0 if it is a Counselor doing the command (or a GM doing /cq) and 1 if it is a GM
	
	int i;
	int x=0;
	
	if(type==1) //Player is a GM
	{
		for(i=1;i<MAXPAGES;i++)
		{
			if (gmpages[i].handled==0)
			{
				if(x==0)
				{
					sysmessage(s,"");
					sprintf(temp,"Next unhandled page from %s", gmpages[i].name);
					sysmessage(s,temp);
					sprintf(temp,"Problem: %s.", gmpages[i].reason);
					sysmessage(s,temp);
					sprintf(temp,"Serial number %x %x %x %x", gmpages[i].ser1, gmpages[i].ser2, gmpages[i].ser3, gmpages[i].ser4);
					sysmessage(s,temp);
					sprintf(temp,"Paged at %s.", gmpages[i].timeofcall);
					sysmessage(s,temp);
				}
				x++;
			}
		}
		if (x>0)
		{
			sprintf(temp,"Total pages in queue: %i",x);
			sysmessage(s,"");
			sysmessage(s,temp);
		}
		else sysmessage(s,"The GM queue is currently empty");
	} //end of first if
	else //Player is a counselor so show counselor queue
	{
		for(i=1;i<MAXPAGES;i++)
		{
			if (counspages[i].handled==0)
			{
				if(x==0)
				{
					sysmessage(s,"");
					sprintf(temp,"Next unhandled page from %s", counspages[i].name);
					sysmessage(s,temp);
					sprintf(temp,"Problem: %s.", counspages[i].reason);
					sysmessage(s,temp);
					sprintf(temp,"Serial number %x %x %x %x", counspages[i].ser1, counspages[i].ser2, counspages[i].ser3, counspages[i].ser4);
					sysmessage(s,temp);
					sprintf(temp,"Paged at %s.", counspages[i].timeofcall);
					sysmessage(s,temp);
				}
				x++;
			}
		}
		if (x>0)
		{
			sprintf(temp,"Total pages in queue: %i",x);
			sysmessage(s,"");
			sysmessage(s,temp);
		}
		else sysmessage(s,"The Counselor queue is currently empty");
	}
}
// new wipe function, basically it prints output on the console when someone wipes so that
// if a malicious GM wipes the world you know who to blame

void cCommands::Wipe(int s)
{
	int k;
	
	ConOut("UOX3: %s has initiated an item wipe\n",chars[currchar[s]].name);
	
	for(k=0;k<=itemcount;k++)
	{
		if(items[k].contserial==-1 && items[k].wipe==0)
		{
			Items->DeleItem(k);
		}
	}
	sysbroadcast("All items have been wiped."); 
}

void cCommands::Possess( UOXSOCKET s ) 
{
	int tmp;
	int serial = calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	int i = calcCharFromSer( serial );

	if(i!=-1)
	{
			
		if (chars[i].shop)
		{
			sysmessage(s,"You cannot use shopkeepers.");
			return;
		}
		else if (!chars[i].npc)
		{
			sysmessage( s, "You can only possess NPCs." );
			return;
		}
		else if( chars[i].npc == 17 ) // Char's old body
		{
			tmp = chars[i].priv;
			chars[i].priv = chars[currchar[s]].priv;
			chars[currchar[s]].priv = tmp;

			tmp = chars[i].priv2;
			chars[i].priv2 = chars[currchar[s]].priv2;
			chars[currchar[s]].priv2 = tmp;
			tmp = chars[i].commandLevel;
			chars[i].commandLevel = chars[currchar[s]].commandLevel;
			chars[currchar[s]].commandLevel = tmp;
			chars[i].npc = 0;
			chars[currchar[s]].npc = 1;
			chars[currchar[s]].account = -1;
			currchar[s] = i;
			startchar( s );
			sysmessage( s, "Welcome back to your old body." );
		} else if( chars[i].npc )
		{
			tmp = chars[i].priv;
			chars[i].priv = chars[currchar[s]].priv;
			chars[currchar[s]].priv = tmp;

			tmp = chars[i].priv2;
			chars[i].priv2 = chars[currchar[s]].priv2;
			chars[currchar[s]].priv2 = tmp;

			tmp = chars[i].commandLevel;
			chars[i].commandLevel = chars[currchar[s]].commandLevel;
			chars[currchar[s]].commandLevel = tmp;

			chars[i].npc = 0;
			chars[i].account = chars[currchar[s]].account;
			chars[currchar[s]].npc = 17;
			chars[currchar[s]].npcWander = 0;
			currchar[s] = i;
			startchar( s );
			sysmessage( s, "Welcome to %s's body!", chars[i].name );
		} else sysmessage( s, "Possession error." );
	}
}

void cCommands::Load( void )
// PRE: Command table exists, script exists
// POST: Command table loaded, or defaults used
{

	int tablePos;
	int retVal = openscript("commands.scp");
	if( retVal == -1 )
	{
		closescript();
		return;
	}
	ConOut("Loading commands now");
	do 
	{
		read2();
		tablePos = FindIndex( script1 );
		if( tablePos == -1)
		{
		  // make sure we don't index into array at -1
		  if ( (strcmp( script1, "EOF" )))
		    ConOut("Invalid command '%s' found in commands.scp!", script1 );
		}
		else
		{
			command_table[tablePos].cmd_priv_m = str2num( script2 );	// loading priv for command!
			ConOut(".");
		}
		// check for commands here
	} while(script1[0]!='}'	&& (strcmp(script1, "EOF")) );
	closescript();
	ConOut( "Done! \n" );
}

signed int cCommands::FindIndex( char *toFind )
{
	bool found = false;
	int curPointer = 0;

	while( !found && command_table[curPointer].cmd_name != NULL )
	{
		if(!(strcmp( command_table[curPointer].cmd_name, toFind ) ) )
			found = true;
		else
			curPointer++;
	}
	if( !found )
		return -1;
	else
		return curPointer;
}

void cCommands::RemoveShop( UOXSOCKET s )
// PRE:	Target is valid, s is a valid socket
// POST:	Removes the shop keeper layers from an npc or pc
// CODER:	Abaddon
// DATE:	February 17th, 2000
{
	SERIAL serial = calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	PLAYER i = calcCharFromSer( serial );

	if (i==-1)
	{
		sysmessage( s, "Target character not found..." );
		return;
	}

	int buyLayer = -1, sellLayer = -1, otherLayer = -1;

	int ci, serialHash;
	int itemToCheck;
	serialHash = serial%HASHMAX;
	chars[i].shop = 0;
	for( ci = 0; ci < contsp[serialHash].max; ci++ )
	{
		itemToCheck = contsp[serialHash].pointer[ci];
		if( itemToCheck != -1 )
		{
			if( items[itemToCheck].contserial == serial )
			{
				if( items[itemToCheck].layer == 0x1A )
					buyLayer = itemToCheck;
				else if( items[itemToCheck].layer == 0x1B )
					sellLayer = itemToCheck;
				else if( items[itemToCheck].layer == 0x1C )
					otherLayer = itemToCheck;
			}
		}
	}
	if( buyLayer != -1 )
		Items->DeleItem( buyLayer );
	if( sellLayer != -1 )
		Items->DeleItem( sellLayer );
	if( otherLayer != -1 )
		Items->DeleItem( otherLayer );
	sysmessage( s, "The character's vendor packs have been removed" );

}

