//------------------------------------------------------------------------
//  p_ai.cpp
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
#include "uox3.h"
#include "debug.h"

#define DBGFILE "p_ai.cpp"


void cCharStuff::CheckAI(unsigned int currenttime, int i) //Lag Fix -- Zippy
{
	int k, j, d, d2, onl;
	
	if (i<0 || i>cmem) return;

	if (nextnpcaitime<=currenttime||(overflow)) 
	{
		switch(chars[i].npcaitype)
		{
		case 1 : // Good Healers
			for (k=0;k<now;k++) 
			{
				if (chars[currchar[k]].dead && chars[currchar[k]].flag&0x04 && chardist(i, currchar[k])<=3 && chars[currchar[k]].flag!=0x02 && chars[currchar[k]].flag!=0x01) 
				{
					npcaction(i, 0x10);
					Targ->NpcResurrectTarget(currchar[k]);
					staticeffect(currchar[k], 0x37, 0x6A, 0x09, 0x06);
					switch(rand()%5) 
					{
					case 0: npctalkall(i, "Thou art dead, but 'tis within my power to resurrect thee.  Live!", 0); break;
					case 1: npctalkall(i, "Allow me to resurrect thee ghost.  Thy time of true death has not yet come.", 0); break;
					case 2: npctalkall(i, "Perhaps thou shouldst be more careful.  Here, I shall resurrect thee.", 0); break;
					case 3: npctalkall(i, "Live again, ghost!  Thy time in this world is not yet done.", 0); break;
					case 4: npctalkall(i, "I shall attempt to resurrect thee.", 0); break;
					}
				} else if (chars[currchar[k]].dead && chardist(i, currchar[k])<=3 && chars[currchar[k]].flag&0x01)
				{
					npctalkall(i, "I will nay give life to a scoundrel like thee!",1);
				}
				else if (chars[currchar[k]].dead && chardist(i, currchar[k])<=3 && chars[currchar[k]].flag&0x02)
				{
					npctalkall(i, "I will nay give life to thee for thou art a criminal!",1);
				}
			}
			break;
		case 666 : //Evil Healers
			for (k=0;k<now;k++) 
			{
				if (chars[currchar[k]].dead && chardist(i, currchar[k])<=3 && chars[currchar[k]].flag&0x01) 
				{
					npcaction(i, 0x10);
					Targ->NpcResurrectTarget(currchar[k]);
					staticeffect(currchar[k], 0x37, 0x09, 0x09, 0x19); //Flamestrike effect
					switch(rand()%5) 
					{
					case 0: npctalkall(i, "Fellow minion of Mondain, Live!!", 0); break;
					case 1: npctalkall(i, "Thou has evil flowing through your vains, so I will bring you back to life.", 0); break;
					case 2: npctalkall(i, "If I ressurect thee, promise to raise more hell!.", 0); break;
					case 3: npctalkall(i, "From hell to Britannia, come alive!.", 0); break;
					case 4: npctalkall(i, "Since you are Evil, I will bring you back to consciouness.", 0); break;
					}
				} else if (chars[currchar[k]].flag!=0x01 && chars[currchar[k]].dead && chardist(i, currchar[k]) <= 3 ) {
					npctalkall(i, "I dispise all things good. I shall not give thee another chance!", 1);
				}
			}
			break;
		case 2 : // Monsters, PK's - (stupid NPCs)
			if (!(chars[i].war)) 
			{
				d2 = combat.maxRange;
				j = -1;
				
				//Char mapRegions
				int	StartGrid = mapRegions->StartGrid( chars[i].x,chars[i].y );
				
				unsigned int increment=0;
				for (unsigned int checkgrid=StartGrid+(increment*mapRegions->GetColSize());increment<3;increment++, checkgrid=StartGrid+(increment*mapRegions->GetColSize()))
				{
					for (int a=0;a<3;a++)
					{
						int mapitemptr=-1;
						int	mapitem=-1;
						int mapchar=-1;
						do
						{
							mapchar=-1;
							mapitemptr=mapRegions->GetNextItem(checkgrid+a, mapitemptr);
							if (mapitemptr==-1) break;
							mapitem=mapRegions->GetItem(checkgrid+a, mapitemptr);
							if(mapitem>999999) mapchar=mapitem-1000000;
							if (mapitem!=-1 && mapitem>=1000000)
							{
								k = mapchar;
								if( k == -1 )
									continue;
								if( i == k )
									continue;
								if( chars[i].ownserial != 0xFFFFFFFF && chars[i].ownserial == chars[k].serial )
									continue;
								d = chardist( i, k );
								if( d > d2 )
									continue;
								if( (chars[k].priv&0x04) || chars[k].hidden != 0 || chars[k].dead )
									continue;
								onl = online( k );
								if( !onl && !chars[k].npc )
									continue;
								if( ( chars[k].npcaitype == 0x02 || chars[k].npcaitype == 0x01 ) && chars[k].race == 0 )
									continue;
								if( server_data.monsters_vs_animals == 0 && strlen( chars[k].title ) > 0 && !( chars[k].id1 == 0x01 && ( chars[k].id2 >= 0x90 || chars[k].id2 <= 0x93 ) ) )
									continue;
								if( server_data.monsters_vs_animals == 1 && RandomNum( 1, 100 ) > server_data.animals_attack_chance )
									continue;
								if( chars[i].race != 0 && chars[i].race == chars[k].race && RandomNum( 0, 100 ) >= 10 )	// 10% chance of turning on own race
									continue;
								int raceComp;
								raceComp = Races->Compare( i, k );
								if( raceComp == 2 )	// Allies
									continue;
								npcattacktarget( k, i );
							}
						} while (mapitem!=-1);
					}
				}
#ifdef DEBUG
				printf( "ERROR: Unable to find a target to kill: character %i at (%i, %i, %i)\n", chars[i].serial, chars[i].x, chars[i].y, chars[i].z );
#endif
			}
			break;
		case 4 : // Guards
			if (!(chars[i].war)) 
			{
				d2=10;
				j=-1;
				
				//Char mapRegions
				int	StartGrid=mapRegions->StartGrid(chars[i].x,chars[i].y);
				
				unsigned int increment=0;
				for (unsigned int checkgrid=StartGrid+(increment*mapRegions->GetColSize());increment<3;increment++, checkgrid=StartGrid+(increment*mapRegions->GetColSize()))
				{
					for (int a=0;a<3;a++)
					{
						int mapitemptr=-1;
						int	mapitem=-1;
						int mapchar=-1;
						do
						{
							mapchar=-1;
							mapitemptr=mapRegions->GetNextItem(checkgrid+a, mapitemptr);
							if (mapitemptr==-1) break;
							mapitem=mapRegions->GetItem(checkgrid+a, mapitemptr);
							if(mapitem>999999) mapchar=mapitem-1000000;
							if (mapitem!=-1 && mapitem>=1000000)
							{
								k=mapchar;
								d=chardist(i, k);
								if ((k!=i)&&(d<=10)&&(!(chars[k].priv&0x04))&&
									(!(chars[k].dead))&&(chars[k].npcaitype&0x02)) 
								{
									if (k!=-1) 
									{
										// matches all other npcattack stuff
										npcattacktarget(k, i);
										npctalkall(i, "Thou shalt regret thine actions, swine!", 1);
									}
									return;
								}
							}
						} while (mapitem!=-1);
					}
				}
			}
			break;
		case 30:
			if (!(chars[i].war))
			{
				d2=10;
				j=-1;
				
				//Char mapRegions
				int	StartGrid=mapRegions->StartGrid(chars[i].x,chars[i].y);
				
				unsigned int increment=0;
				for (unsigned int checkgrid=StartGrid+(increment*mapRegions->GetColSize());increment<3;increment++, checkgrid=StartGrid+(increment*mapRegions->GetColSize()))
				{
					for (int a=0;a<3;a++)
					{
						int mapitemptr=-1;
						int	mapitem=-1;
						int mapchar=-1;
						do
						{
							mapchar=-1;
							mapitemptr=mapRegions->GetNextItem(checkgrid+a, mapitemptr);
							if (mapitemptr==-1) break;
							mapitem=mapRegions->GetItem(checkgrid+a, mapitemptr);
							if(mapitem>999999) mapchar=mapitem-1000000;
							if (mapitem!=-1 && mapitem>=1000000)
							{
								k=mapchar;
								d=chardist(i, k);
								if ((k!=i)&&(d<=10)&&(!(chars[k].priv&0x04))&&
									(!(chars[k].dead))&&(chars[k].npcaitype&0x02)) 
								{
									/*d2=d;
									j=k;
									break;*/
									if (k!=-1) npcattacktarget(k, i);
									return;
								}
							}
						} while (mapitem!=-1);
					}
				}
			}
			break;
		case 0x50://Morrolan EV/BS logic
			if (!(chars[i].war)) 
			{
				d2=10;
				j=-1;
				
				//Char mapRegions
				int     StartGrid=mapRegions->StartGrid(chars[i].x,chars[i].y);
				
				unsigned int increment=0;
				for (unsigned int checkgrid=StartGrid+(increment*mapRegions->GetColSize());increment<3;increment++, checkgrid=StartGrid+(increment*mapRegions->GetColSize()))
				{
					for (int a=0;a<3;a++)
					{
						int mapitemptr=-1;
						int mapitem=-1;
						int mapchar=-1;
						do
						{
							mapchar=-1;
							mapitemptr=mapRegions->GetNextItem(checkgrid+a, mapitemptr);
							if (mapitemptr==-1) break;
							mapitem=mapRegions->GetItem(checkgrid+a, mapitemptr);
							if(mapitem>999999) mapchar=mapitem-1000000;
							if (mapitem!=-1 && mapitem>=1000000)
							{
								k=mapchar;
								onl=online(k);
								d=chardist(i, k);
								if (
									(k!=i)&&(d<=d2)&&(!(chars[k].priv&0x04))&& //Morrolan EV/BS now attack hidden chars
									(!(chars[k].dead))&&
									(onl || (chars[k].npc))
									)
								{
									/*d2=d;
									j=k;
									break;*/
									if (k!=-1) npcattacktarget(k, i);
									return;
								}
							}
						} while (mapitem!=-1);
					}
				}
			}
			break;
		case 0: break; //morrolan - nothing
		case 8: break; //morrolan - old banker
		case 5: break; //morrolan - personal guard?
		case 17: break; //Zippy Player Vendors.
		case 32: break;	// Guard AI
		default:
			printf("ERROR: cCharStuff::CheckAI-> Error npc %i (%x %x %x %x) has invalid AI type %i\n",i,chars[i].ser1,chars[i].ser2,chars[i].ser3,chars[i].ser4,chars[i].npcaitype); //Morrolan
			return;
		}// switch
	}// if (nextnpcaitime<=currenttime||(overflow))
}// void checknpcai



