//""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
//  magic.cpp
//
//""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
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
//""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""


/*  
*   UOX3 Magic stuff
*
*	Completed the restructuring by AntiChrist (9/99)
*   Functions' description added by AntiChrist (9/99)
*/

#include "uox3.h"
#include "debug.h"

#define DBGFILE "magic.cpp"

/////////////////////////////////////////////////////////////////
/// INDEX:
//              - misc magic functions
//              - NPCs casting spells related functions
//              - ITEMs magic powers related functions
//              - PCs casting spells related functions
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////// MISC MAGIC FUNCTIONS ////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

//:Terrin: added constructor/destructor
cMagic::cMagic()
{
	// it would be nice to go ahead and cache them now, but the
	// script files won't have been loaded yet..
	spells = NULL;
}

cMagic::~cMagic()
{
	// delete any allocations
	if (spells)
		delete [] spells;
}
//o---------------------------------------------------------------------------o
//|     Class         :          ::SpellBook( UOXSOCKET s )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Sends the spellbook item (with all the 
//|                              memorized spells) to player when 
//|                              doubleclicked.
//o---------------------------------------------------------------------------o

void cMagic::SpellBook( UOXSOCKET s )
{
	int i, j, scount, item, x,serial,serhash,ci;
	int spellsList[70];
	char sbookstart[8]="\x24\x40\x01\x02\x03\xFF\xFF";
	char sbookinit[6]="\x3C\x00\x3E\x00\x03";
	char sbookspell[20]="\x40\x01\x02\x03\x1F\x2E\x00\x00\x01\x00\x48\x00\x7D\x40\x01\x02\x03\x00\x00";
	
	serial=calcserial(buffer[s][1]&0x7F,buffer[s][2],buffer[s][3],buffer[s][4]);
	item=findbyserial(&itemsp[serial%HASHMAX],serial,0);
	
	x=packitem(currchar[s]);
	if (item!=-1 && x!=-1) //lb
		if (items[item].contserial!=items[x].serial && items[item].contserial!=chars[currchar[s]].serial)
		{
			if( items[findbyserial(&itemsp[items[item].contserial%HASHMAX], items[item].contserial, 0 )].type != 9 )
			{
				sysmessage(s, "In order to open spellbook, it must be equipped in your hand or in the first layer of your backpack.");
			}
			return;
		}
		
	if (item==-1)
	{
		item=-1;
		if (x!=-1)
		{
			serial=items[x].serial;
			serhash=serial%HASHMAX;
			
			// AntiChrist Fix -- itemcount loop removed by LB
			
			for (ci=0;ci<contsp[serhash].max;ci++)
			{
				i=contsp[serhash].pointer[ci];
				if (i!=-1)
					if (((items[i].type==9)&&(items[i].contserial==serial)))
					{
						item=i;
						break;
					}
			} 
		}
	}
		// LB remark: If you want more than one spellbook per player working predictable
		// quite a lot of that function needs to be rewritten !
		// reason: just have a look at the loop above ...
		
	if (item==-1)
	{
		serial=chars[currchar[s]].serial;
		serhash=serial%HASHMAX;
		for (ci=0;ci<contsp[serhash].max;ci++)
		{
			i=contsp[serhash].pointer[ci];
			if (i!=-1)
				if ((items[i].contserial==serial) && (items[i].free==0))
				{
					if ((items[i].layer==1) && items[i].type==9)
					{
						item=i;
						break;
					}
				}
		}	
	}
	
	if (item==-1)
	{
		sysmessage(s, "In order to open spellbook, it must be equipped in your hand or in the first layer of your backpack.");
		return;
	}
	
	
	if (items[item].layer!=1) senditem(s,item); // prevents crash if pack not open
	
	// Network->xSend(s, pause, 2, 0);
	sbookstart[1]=items[item].ser1;
	sbookstart[2]=items[item].ser2;
	sbookstart[3]=items[item].ser3;
	sbookstart[4]=items[item].ser4;
	Network->xSend(s, sbookstart, 7, 0);
	scount=0;

	for (i=0;i<70;i++)
	{
		spellsList[i]=0;
	}
	
	if( items[item].morex == 0 && items[item].morey == 0 && items[item].morez == 0 )
	{
		serial=items[item].serial;
		serhash=serial%HASHMAX;
		for (ci=0;ci<contsp[serhash].max;ci++)
		{
			j=contsp[serhash].pointer[ci];
			if (j!=-1)
				if ((items[j].contserial==serial))
				{
					if (items[j].id1==0x1F && items[j].id2>=0x2D && items[j].id2<=0x72) 
					{
						spellsList[items[j].id2-0x2D]=1;
					}
				}
		}
	}
	else
	{
		for( j = 0; j < 70; j++ )
			spellsList[j] = HasSpell( item, j );
	}

	// Fix for Reactive Armor/Bird's Eye dumbness. :)
	i = spellsList[0];
	spellsList[0] = spellsList[1];
	spellsList[1] = spellsList[2];
	spellsList[2] = spellsList[3];
	spellsList[3] = spellsList[4];
	spellsList[4] = spellsList[5];
	spellsList[5] = spellsList[6]; // Morac is right! :)
	spellsList[6] = i;
	// End fix.
	
	if (spellsList[64])
	{
		for (i=0;i<70;i++)
			spellsList[i]=1;
		spellsList[64]=0;
	}
	spellsList[64]=spellsList[65];
	spellsList[65]=0;
	
	for (i=0;i<70;i++)
	{
		if (spellsList[i]) scount++;
	}
	sbookinit[1]= (char) ((scount*19)+5)>>8;
	sbookinit[2]= (char) ((scount*19)+5)%256;
	sbookinit[3]= (char) scount>>8;
	sbookinit[4]= (char) scount%256;
	if (scount>0) Network->xSend(s, sbookinit, 5, 0);
	for (i=0;i<70;i++) 
	{
		if (spellsList[i])
		{
			sbookspell[0]=0x41;
			sbookspell[1]=0x00;
			sbookspell[2]=0x00;
			sbookspell[3]= (char) i+1;
			sbookspell[8]= (char) i+1;
			sbookspell[13]=items[item].ser1;
			sbookspell[14]=items[item].ser2;
			sbookspell[15]=items[item].ser3;
			sbookspell[16]=items[item].ser4;
			Network->xSend(s, sbookspell, 19, 0);
		} 
		// Network->xSend(s, restart, 2, 0);
	}
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::GateCollision( PLAYER s )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//|                              Modified by AntiChrist - collision dir
//o---------------------------------------------------------------------------o
//|     Purpose       :          used when a PLAYER passes through a gate.  Takes the player
//|                              to the other side of the gate-link.
//o---------------------------------------------------------------------------o

char cMagic::GateCollision( PLAYER s )
{
	unsigned int n;
	long index;
	
	// - Tauriel's region stuff 3/6/99
	int getcell=mapRegions->GetCell(chars[s].x,chars[s].y);
	
	int mapitem=-1;
	int mapitemptr=-1;
	do //check all items in this cell
	{
		mapitemptr=mapRegions->GetNextItem(getcell, mapitemptr);
		if (mapitemptr==-1) break;
		mapitem=mapRegions->GetItem(getcell, mapitemptr);
		if (mapitem!=-1 && mapitem<999999)
		{
			if( items[mapitem].type == 51 || items[mapitem].type == 52 )
			{
				n = 52 - items[mapitem].type;
				if( ( chars[s].x == items[mapitem].x ) &&
					( chars[s].y == items[mapitem].y ) &&
					( chars[s].z >= items[mapitem].z ) )
				{
					mapRegions->RemoveItem( s + 1000000 );
					// Dupois - Check for any NPC's that are following this player
					//          There has to be a better way than this...
					//          Think about it some more and change this.
					// If this is a player character
					if( !chars[s].npc )
					{
						int getcell2 = mapRegions->GetCell(chars[s].x,chars[s].y);
	
						int mapitem2=-1;
						int mapitemptr2=-1;
						do //check all items in this cell
						{
							mapitemptr2=mapRegions->GetNextItem(getcell2, mapitemptr2);
							if (mapitemptr2==-1) break;
							mapitem2=mapRegions->GetItem(getcell2, mapitemptr2);
							if (mapitem2>999999)
							{
								index = mapitem2 - 1000000;
								if ( (chars[index].npc==1) && (chars[index].ftarg==s) )
								{
									// If the NPC that is following this player character is within 5 paces 
									if( chardist(s, index) <= 4 )
									{
										// Teleport the NPC along with the player
										mapRegions->RemoveItem( index + 1000000 );
										chars[index].x = gatex[items[mapitem].gatenumber][n];
										chars[index].y = gatey[items[mapitem].gatenumber][n];
										chars[index].dispz = chars[index].z = gatez[items[mapitem].gatenumber][n];
										mapRegions->AddItem( index + 1000000 );
										teleport( index );
									}
								}
							}
						} while ( mapitemptr2 != -1 );
					}
					// Set the characters destination
					chars[s].x = gatex[items[mapitem].gatenumber][n];
					chars[s].y = gatey[items[mapitem].gatenumber][n] + 1;
					chars[s].dispz=chars[s].z=gatez[items[mapitem].gatenumber][n];
					
					// Teleport the current character
					mapRegions->AddItem(s+1000000); //LB, add with new x,y
					teleport(s);
					soundeffect( calcSocketFromChar( s ), 0x01, 0xFE );
					staticeffect( s, 0x37, 0x2A, 0x09, 0x06 );
				}
			}
		}
	}
	while (mapitemptr!=-1);
	return(1);
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::GateDestruction( unsigned int currenttime )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Check all the gates in the world
//|                              and destroy a gate if its gatetime elapsed.
//o---------------------------------------------------------------------------o

void cMagic::GateDestruction(unsigned int currenttime)
{
	unsigned int i,k;
	
	for (i=0;i<itemcount;i++)
	{
		if((items[i].type==51) || items[i].type==52) 
		{
			if(items[i].gatetime<=currenttime)
			{
				for (k=0;k<2;k++) Items->DeleItem(i); 
			}
		}
	}
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::SummonMonster(UOXSOCKET s, unsigned char id1, unsigned char id2, char * monstername, unsigned char color1, unsigned char color2, int x, int y, int z)
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Summon a monster (dispellable with DISPEL).
//o---------------------------------------------------------------------------o
void cMagic::SummonMonster(UOXSOCKET s, unsigned char id1, unsigned char id2, char * monstername, unsigned char color1, unsigned char color2, int x, int y, int z)
{
	int i;
	unsigned int c;
	
	c=Npcs->MemCharFree ();
	i=(id1<<8)+id2;
	Npcs->InitChar(c);
	switch(i)
	{
	case 0x0000:	// summon monster
		// Revised Abaddon 17th February 2000
		// Now successfully summons a monster, if you have an npc list 10000
		Npcs->DeleteChar( c );
		soundeffect( s, 0x02, 0x17 );
		c=Npcs->AddRandomNPC( s, "10000", -1 );
		if( c == -1 )
		{
			sysmessage( s, "Contact your shard op to setup the summon list!" );
			return;
		}
		setserial( c, currchar[s], 5 );
		mapRegions->RemoveItem( c + 1000000 );
		chars[c].x = chars[currchar[s]].x;
		chars[c].y = chars[currchar[s]].y;
		chars[c].z = chars[currchar[s]].z;
		mapRegions->AddItem( c + 1000000 );
		chars[c].summontimer = (unsigned int) (uiCurrentTime+((chars[currchar[s]].skill[MAGERY]/10)*(CLOCKS_PER_SEC*2)));
		updatechar(c);
		npcaction( c, 0x0C);
		return;
	case 0x000D: // Energy Vortex & Air elemental
		if (color1==0x00 && color2==0x75)
		{
			soundeffect(s, 0x02, 0x16); // EV
			chars[c].def=22;
			chars[c].lodamage=10;
			chars[c].hidamage=30;
			//			chars[c].spattack=8191; // 1-7 level spells
			chars[c].baseskill[MAGERY]=950; // 95 magery
			chars[c].baseskill[TACTICS]=2000; // 200 tactics
			chars[c].skill[MAGICRESISTANCE]=850;
			chars[c].st=chars[c].hp=400;
			chars[c].dx=chars[c].stm=160;
			chars[c].in=chars[c].mn=180;
			chars[c].npcWander=2;
			chars[c].npcaitype=0x50; // morollan
			chars[c].poison = 3;
			
		}
		else
		{
			soundeffect(s, 0x02, 0x17); // AE
			chars[c].def=19;
			chars[c].lodamage=5;
			chars[c].hidamage=13;
			chars[c].spattack=4095; // 1-6 level spells
			chars[c].baseskill[MAGERY]=750; // 75 magery
			chars[c].baseskill[TACTICS]=700; // 70 tactics               
			chars[c].skill[MAGICRESISTANCE]=450;
			chars[c].st=chars[c].hp=125;
			chars[c].dx=chars[c].stm=100;
			chars[c].in=chars[c].mn=80;
		}
		break;
	case 0x000A: // Daemon
		soundeffect(s, 0x02, 0x16);
		chars[c].def=20;
		chars[c].lodamage=10;
		chars[c].hidamage=45;
		chars[c].spattack=8191; // 1-7 level spells
		chars[c].baseskill[MAGERY]=900; // 90 magery
		chars[c].baseskill[TACTICS]=700; // 70 tactics 
		chars[c].skill[MAGICRESISTANCE]=650;
		chars[c].st=chars[c].hp=400;
		chars[c].dx=chars[c].stm=70;
		chars[c].in=chars[c].mn=400;
		break;
	case 0x000E: //Earth
		soundeffect(s, 0x02, 0x17);
		chars[c].def=15;
		chars[c].lodamage=3;
		chars[c].hidamage=18;
		chars[c].baseskill[TACTICS]=850; // 85 tactics 
		chars[c].skill[MAGICRESISTANCE]=350;
		chars[c].st=chars[c].hp=125;
		chars[c].dx=chars[c].stm=90;
		chars[c].in=chars[c].mn=70;
		break;
	case 0x000F: //Fire
	case 0x0010: //Water
		soundeffect(s, 0x02, 0x17);
		chars[c].def=19;
		chars[c].lodamage=4;
		chars[c].hidamage=12;
		chars[c].spattack=4095; // 1-6 level spells
		chars[c].baseskill[MAGERY]=800; // 80 magery
		chars[c].baseskill[TACTICS]=800; // 80 tactics 
		chars[c].skill[MAGICRESISTANCE]=450;
		chars[c].st=chars[c].hp=120;
		chars[c].dx=chars[c].stm=95;
		chars[c].in=chars[c].mn=70;
		break;
	case 0x023E: //Blade Spirits
		soundeffect(s, 0x02, 0x12); // I don't know if this is the right effect...
		chars[c].def=24;
		chars[c].lodamage=5;
		chars[c].hidamage=10;
		chars[c].baseskill[TACTICS]=950; // 95 tactics 
		chars[c].skill[MAGICRESISTANCE]=650;
		chars[c].st=chars[c].hp=200;
		chars[c].dx=chars[c].stm=95;
		chars[c].in=chars[c].mn=70;
		chars[c].npcWander=2;
		chars[c].npcaitype = 0x50;
		chars[c].poison = 2;
		break;
	case 0x03e2: // Dupre The Hero
		soundeffect(s, 0x02, 0x46);
		chars[c].def=50;
		chars[c].lodamage=50;
		chars[c].hidamage=100;
		chars[c].spattack=8191; // 1-7 level spells
		chars[c].baseskill[MAGERY]=900; // 90 magery
		chars[c].baseskill[TACTICS]=1000; // 70 tactics 
		chars[c].baseskill[SWORDSMANSHIP]=1000;
		chars[c].baseskill[PARRYING]=1000;
		chars[c].skill[MAGICRESISTANCE]=650;
		chars[c].st=chars[c].hp=600;
		chars[c].dx=chars[c].stm=70;
		chars[c].in=chars[c].mn=100;
		chars[c].fame=10000;
		chars[c].karma=10000;
		break;
	case 0x000B: // Black Night
		soundeffect(s, 0x02, 0x16);
		chars[c].def=50;
		chars[c].lodamage=50;
		chars[c].hidamage=100;
		chars[c].spattack=8191; // 1-7 level spells
		chars[c].baseskill[MAGERY]=1000; // 100 magery
		chars[c].baseskill[TACTICS]=1000;// 100 tactics
		chars[c].baseskill[SWORDSMANSHIP]=1000;
		chars[c].baseskill[PARRYING]=1000;
		chars[c].skill[MAGICRESISTANCE]=1000;
		chars[c].st=chars[c].hp=600;
		chars[c].dx=chars[c].stm=70;
		chars[c].in=chars[c].mn=100;
		break;
	case 0x0190: // Death Knight
		soundeffect(s, 0x02, 0x46);
		chars[c].def=20;
		chars[c].lodamage=10;
		chars[c].hidamage=45;
		chars[c].spattack=4095; // 1-7 level spells
		chars[c].baseskill[MAGERY]=500; // 90 magery
		chars[c].baseskill[TACTICS]=1000; // 70 tactics 
		chars[c].baseskill[SWORDSMANSHIP]=1000;
		chars[c].baseskill[PARRYING]=1000;
		chars[c].skill[MAGICRESISTANCE]=650;
		chars[c].st=chars[c].hp=600;
		chars[c].dx=chars[c].stm=70;
		chars[c].in=chars[c].mn=100;
		chars[c].fame=-10000;
		chars[c].karma=-10000;
		break;
	default:
		soundeffect(s, 0x02, 0x15);
	}
	strcpy(chars[c].name, monstername);
	chars[c].id1=chars[c].xid1=id1;
	chars[c].xid2=chars[c].id2=id2;
	chars[c].skin1=chars[c].xskin1=color1;
	chars[c].skin2=chars[c].xskin2=color2;
	chars[c].orgid1=id1;
	chars[c].orgid2=id2;
	chars[c].priv2=0x20;
	chars[c].npc=1;
	if (i!=0x023E && !(i==0x000d && color1==0 && color2==0x75))  // don't own BS or EV.
		setserial(c, currchar[s], 5);
	mapRegions->RemoveItem(c+1000000);
	
	if (x==0)
	{
		chars[c].x = (short int) chars[currchar[s]].x-1;
		chars[c].y = (short int) chars[currchar[s]].y;
		chars[c].dispz=chars[c].z=chars[currchar[s]].z;
	}
	else
	{
		chars[c].x=x;
		chars[c].y=y;
		chars[c].dispz=chars[c].z=z;
	}
	
	mapRegions->AddItem(c+1000000); 
	
	chars[c].spadelay=10;
	chars[c].summontimer = (unsigned int) (uiCurrentTime+((chars[currchar[s]].skill[MAGERY]/10)*(CLOCKS_PER_SEC*2)));
	updatechar(c);
	npcaction(c, 0x0C);
	// AntiChrist (9/99) - added the chance to make the monster attack
	// the person you targeted ( if you targeted a char, naturally :) )
	if( buffer[s][7] == 0xFF && buffer[s][8] == 0xFF && buffer[s][9] == 0xFF && buffer[s][10] == 0xFF ) 
      return;
	int serial = calcserial( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	i = findbyserial( &charsp[serial%HASHMAX], serial, 1 );
	if (i == -1)
      return;
	npcattacktarget( i, c );
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::CheckBook( int circle, int spell, int i )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Check if the spell is memorized into the spellbook.
//o---------------------------------------------------------------------------o

int cMagic::CheckBook(int circle, int spell, int i)
{
	int j, spellnum, raflag=0,serial,serhash,ci;
	
	spellnum=spell+(circle-1)*8;
	// Fix for OSI stupidity. :) 
	if (spellnum==6) raflag=1;
	if (spellnum>=0 && spellnum<6) spellnum++;
	if (raflag) spellnum=0;
	serial=items[i].serial;
	serhash=serial%HASHMAX;
	//j=findbyserial(&itemsp[serial%256], serial,0);
	if( items[i].morex == 0 && items[i].morey == 0 && items[i].morez == 0 )
	{
		for (ci=0;ci<contsp[serhash].max;ci++)
		{
			j=contsp[serhash].pointer[ci];
			if (j!=-1)
				if((items[j].id1==0x1F && items[j].id2==(0x2D+spellnum) ||
					items[j].id2==0x6D))
				{
					return 1;
				}
		}
	}
	else
	{
		return HasSpell( i, spellnum );
	}

	return 0;
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::SbOpenContainer( UOXSOCKET s )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Open player's spellbook as a container.
//|                              (used for SBOPEN command)
//o---------------------------------------------------------------------------o

void cMagic::SbOpenContainer( UOXSOCKET s )
{
	int i,serial;
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	if (i!=-1)
	{
		if ((items[i].type==9))
		{
			backpack(s,buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
		} else sysmessage(s,"That is not a spellbook.");
	}
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::CheckMana( CHARACTER s, int circle )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//                               Modified by AntiChrist to use spells[] array.
//o---------------------------------------------------------------------------o
//|     Purpose       :          Check if character has enough mana to cast
//|                              a spell of that circle.
//o---------------------------------------------------------------------------o

char cMagic::CheckMana( CHARACTER s, int num )
{
	int p;
	if (chars[s].priv2&0x10)
		return 1;
	if( chars[s].mn >= spells[num].mana ) 
		return 1;
	p = calcSocketFromChar( s );
	if( p != -1 ) 
		sysmessage( p, "You have insufficient mana to cast that spell." );
	return 0;
}

bool cMagic::CheckStamina( CHARACTER s, int num )
{
	int p;
	if( chars[s].priv2&0x10 )
		return true;
	if( chars[s].stm >= spells[num].stamina ) 
		return true;
	p = calcSocketFromChar( s );
	if( p != -1 ) 
		sysmessage( p, "You have insufficient stamina to cast that spell." );
	return false;
}

bool cMagic::CheckHealth( CHARACTER s, int num )
{
	int p;
	if( chars[s].priv2&0x10 )
		return true;
	if( spells[num].health == 0 )
		return true;
	if( spells[num].health > 0 )
	{
		if( chars[s].hp >= spells[num].health ) 
			return true;
	}
	else
		return true;	// if it's negative, it depends on the TARGET's value
	p = calcSocketFromChar( s );
	if( p != -1 ) 
		sysmessage( p, "You have insufficient health to cast that spell." );
	return false;
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::SubtractMana( CHARACTER s, int mana )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Subtract the required mana from character's
//|                              mana reserve.
//o---------------------------------------------------------------------------o

char cMagic::SubtractMana( CHARACTER s, int mana)
{
	int p;
	char retval=1;
	
	if (chars[s].priv2&0x10)
		return 1;
	
	chars[s].mn-=mana;
	if(chars[s].mn<0) chars[s].mn=0;//Bug Fix -- Zippy
	p=calcSocketFromChar(s);
	if (p!=-1) updatestats(p, 1);
	
	return retval;
}

char cMagic::SubtractStamina( CHARACTER s, int stamina )
{
	int p;
	if( chars[s].priv2&0x10 )
		return 1;
	
	chars[s].stm -= stamina;
	if( chars[s].stm < 0 ) 
		chars[s].stm = 0;
	p = calcSocketFromChar( s );
	if( p != -1 ) 
		updatestats( p, 2 );
	return 1;
}

char cMagic::SubtractHealth( CHARACTER s, int health, int spellNum )
{
	int p;
	if( chars[s].priv2&0x10 || spells[spellNum].health == 0 )
		return 1;
	if( spells[spellNum].health < 0 )
	{
		if( abs( spells[spellNum].health * health ) > chars[s].hp )
			chars[s].hp = 0;
		else
			chars[s].hp += (spells[spellNum].health * health);
	}
	else
		chars[s].hp -= health;
	if( chars[s].hp < 0 ) 
		chars[s].hp = 0;
	p = calcSocketFromChar( s );
	if( p != -1 ) 
		updatestats( p, 0 );
	return 1;
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::CheckMagicReflect( CHARACTER i )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Check if character is protected by MagicReflect;
//|                              if yes, remove the protection and do visual effect.
//o---------------------------------------------------------------------------o

char cMagic::CheckMagicReflect( CHARACTER i ) 
{
	if (chars[i].priv2&0x40)
	{
		chars[i].priv2=chars[i].priv2&0xBF;
		staticeffect(i, 0x37, 0x3A, 0, 15);
		return 1;
	}
	return 0;
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::CheckResist( CHARACTER player, int circle )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//|                              Modified by AntiChrist to add EV.INT. check
//o---------------------------------------------------------------------------o
//|     Purpose       :          Check character's magic resistance.
//o---------------------------------------------------------------------------o

char cMagic::CheckResist( CHARACTER attacker, CHARACTER defender, int circle)
{
	char i=Skills->CheckSkill(defender, MAGICRESISTANCE, 80*circle, 800+(80*circle));;
	int s;
	if (i)
	{
		if( attacker >= 0 ) // NOTE: only do the EV.INT. check if attacker >= 0
		{
			// printf("DIFFERENCE: %d\n", (chars[defender].skill[MAGICRESISTANCE] - chars[attacker].skill[EVALUATINGINTEL] ) );
			if((chars[defender].skill[MAGICRESISTANCE]-chars[attacker].skill[EVALUATINGINTEL] ) >= 0 )
			{
				s = calcSocketFromChar( defender );
				if (s!=-1)
				{
					sysmessage(s, "You feel yourself resisting magical energy!");
				}
			}
			else
			{
				i = 0;
			}
		}
		else
		{
			s = calcSocketFromChar( defender );
			if( s != -1 )
			{
				sysmessage( s, "You feel yourself resisting magical energy!" );
			}
		}
	}
	return i;
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::MagicDamage( CHARACTER p )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//|		Debugging	  :			 Oakwood
//o---------------------------------------------------------------------------o
//|     Purpose       :          Calculate and inflict magic damage.
//o---------------------------------------------------------------------------o

void cMagic::MagicDamage( CHARACTER p, int amount, CHARACTER attacker )
{
	int s;
	if( chars[p].dead || chars[p].hp <= 0 || (chars[p].priv&0x04) )
		return;
	char eval = Skills->CheckSkill( p, EVALUATINGINTEL, 0, 1000 );
	if( eval ) amount = max( 1, amount - ( amount * ( chars[p].skill[EVALUATINGINTEL]/10000 ) ) ); // Take off 0 to 10% damage but still hurt at least 1hp (1000/10000=0.10)
	if ((chars[p].priv2&0x02) && (chars[p].dx>0))
	{
		chars[p].priv2=chars[p].priv2&0xFD; // unfreeze
		s=calcSocketFromChar(p);
		if( s != -1 ) 
			sysmessage( s, "You are no longer frozen." );
	}
	if( !(chars[p].priv&0x04) && (region[chars[p].region].priv&0x40) )
	{
		if( chars[p].npc )
			amount *= 2;      // double damage against non-players
		chars[p].hp = max(0, chars[p].hp - amount);
		updatestats( p, 0 );
		if( chars[p].hp <= 0 )
		{
			if( attacker != -1 && p != attacker )	// can't gain fame and karma for suicide :>
			{
				Kill( attacker, p );
			}
			deathstuff( p );
		}
		else if( !chars[p].npc && chars[p].casting > 0 )
		{
			s = calcSocketFromChar( p );
			if( currentSpellType[s] == 0 )
			{
				chars[p].spellCast = -1;
				chars[p].casting = 0;
				chars[p].priv2 &= 0xFD;
				sysmessage( s, "Your concentration has been broken" );
			}
		}
	}
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::PoisonDamage( CHARACTER p )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Apply the poison to the character.
//o---------------------------------------------------------------------------o

void cMagic::PoisonDamage( CHARACTER p, int poison )
{
	int s;
	if( chars[p].priv2&0x02 )
	{
		chars[p].priv2 &= 0xFD;
		s = calcSocketFromChar( p );
		if( s != -1 ) 
			sysmessage( s, "You are no longer frozen." );
	}           
	if( ( !( chars[p].priv&0x04 ) ) && (!(region[chars[p].region].priv&0x40)))
	{
		if( poison > 5 ) 
			poison = 5;
		if( poison < 0 ) 
			poison = 1;
		chars[p].poisoned = poison;
		chars[p].poisonwearofftime = (unsigned int) (uiCurrentTime + (CLOCKS_PER_SEC * server_data.poisontimer));
		if( !chars[p].npc ) 
			impowncreate( calcSocketFromChar( p ), p, 1 );
		
	}
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::CheckFieldEffects2( unsigned int currenttime, CHARACTER c, char timecheck )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Check if character stands on a magic-field,
//|                              and apply effects.
//o------------------------------------------------------------------------

//o------------------------------------------------------------------------
// timecheck: 0: always executed no matter of the nextfieldspelltime value
// timecheck: 1: only executed if the time is right for next fieldeffect check
// we need this cause its called from npccheck and pc-check
// npc-check already has its own timer, pc check not.
// thus in npccheck its called with 0, in pc check with 1
// we could add the fieldeffect check time the server.scp but I think this solution is better.
// LB October 99
//o---------------------------------------------------------------------------o

void cMagic::CheckFieldEffects2(unsigned int currenttime, CHARACTER c, char timecheck )//c=character (Not socket) //Lag fix -- Zippy
{
	// - Tauriel's region stuff 3/6/99
	int StartGrid=mapRegions->StartGrid(chars[c].x,chars[c].y);
	//int getcell=mapRegions->GetCell(chars[c].x,chars[c].y);
	
	int i, j;
	if( timecheck )
	{
		if (nextfieldeffecttime<=currenttime)
			j = 1;
		else
			j = 0;
	}
	else
		j = 1;
	if( j )
	{
		//printf("charname: %s\n",chars[c].name);
		unsigned int increment=0;
		for (unsigned int checkgrid=StartGrid+(increment*mapRegions->GetColSize());increment<3;increment++, checkgrid=StartGrid+(increment*mapRegions->GetColSize()))
		{
			for (i=0;i<3;i++)
			{
				int mapitem=-1;
				int mapitemptr=-1;
				do //check all items in this cell
				{   
					mapitemptr=mapRegions->GetNextItem(checkgrid+i, mapitemptr);
					if (mapitemptr==-1) break;
					mapitem=mapRegions->GetItem(checkgrid+i, mapitemptr);
					if (mapitem!=-1 && mapitem<1000000)
					{
						//printf("itemname: %s\n",items[mapitem].name);// perfect for mapregion debugging, LB
						if ((items[mapitem].x==chars[c].x)&&(items[mapitem].y==chars[c].y))  // lb
						{
							// printf("firefield1\n");
							if (items[mapitem].id1==0x39 && (items[mapitem].id2==0x96 || items[mapitem].id2==0x8C))
							{
								// printf("firefield\n");
								
								if (!CheckResist(-1, c, 4))	MagicDamage(c, items[mapitem].morex/100);
								else 
									MagicDamage(c, items[mapitem].morex/200);
								
								soundeffect2(c, 2, 8);
								return;
							} else if (items[mapitem].id1==0x39 && (items[mapitem].id2==0x15 || items[mapitem].id2==0x20))
							{//Poison Field
								if (!CheckResist(-1, c, 5))
								{
									if ((items[mapitem].morex<997))
										PoisonDamage(c,2);
									
									else PoisonDamage(c,3); // gm mages can cast greater poison field, LB
								} else PoisonDamage(c,1); // cant be completly resited
								
								soundeffect2(c, 2, 8);
								return;
							} else if (items[mapitem].id1==0x39 && (items[mapitem].id2==0x79 || items[mapitem].id2==0x67))
							{//Para Field
								if (!CheckResist(-1, c, 6))
									tempeffect(c, c, 1, 0, 0, 0);
								
								soundeffect2(c, 0x02, 0x04);     
								return;
							} 
							break;
						}
					}
				} while (mapitem!=-1);
			} //- end of itemcount for loop
		}
	}	
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::BoxSpell( UOXSOCKET s, int& x1, int& x2, int& y1, int& y2, int& z1, int& z2 )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Calculate the spell box effect, depending 
//|                              on character's magery skill.
//o---------------------------------------------------------------------------o

void cMagic::BoxSpell( UOXSOCKET s, int& x1, int& x2, int& y1, int& y2, int& z1, int& z2)
{
	int x, y, z, lenght;
	
	x=(buffer[s][11]<<8)+buffer[s][12];
	y=(buffer[s][13]<<8)+buffer[s][14];
	z=buffer[s][16];
	
	lenght=chars[currchar[s]].skill[MAGERY]/130; // increasde max-range, LB
	
	x1=x-lenght;
	x2=x+lenght;
	y1=y-lenght;
	y2=y+lenght;
	z1=z;
	z2=z+3;
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::MagicTrap( PLAYER s, int i )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Do the visual effect and apply magic damage
//|                              when a player opens a trapped container.
//o---------------------------------------------------------------------------o

void cMagic::MagicTrap( PLAYER s, int i)
{
    staticeffect(s, 0x36, 0xB0, 0x09, 0x09);
    soundeffect2(s, 0x02, 0x07);   
	if(CheckResist(-1, s, 4)) MagicDamage(s,items[i].moreb2/2);
	else MagicDamage(s,items[i].moreb2/2);
	items[i].moreb1=0;
	items[i].moreb2=0;
	items[i].moreb3=0;
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::DeleReagents( CHARACTER s, int ash, int drake, int garlic, int ginseng, int moss, int pearl, int shade, int silk )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Delete required reagents to cast a spell.
//o---------------------------------------------------------------------------o

void cMagic::DeleReagents( CHARACTER s, int ash, int drake, int garlic, int ginseng, int moss, int pearl, int shade, int silk)
{
	if (chars[s].priv2&0x80) return;
	delequan(s, 0x0F, 0x7A, pearl);
	delequan(s, 0x0F, 0x7B, moss);
	delequan(s, 0x0F, 0x84, garlic);
	delequan(s, 0x0F, 0x85, ginseng);
	delequan(s, 0x0F, 0x86, drake);
	delequan(s, 0x0F, 0x88, shade);
	delequan(s, 0x0F, 0x8C, ash);
	delequan(s, 0x0F, 0x8D, silk);
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::CheckReagents( CHARACTER s, int ash, int drake, int garlic, int ginseng, int moss, int pearl, int shade, int silk )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//|                              Modified by AntiChrist to use reag-st
//o---------------------------------------------------------------------------o
//|     Purpose       :          Check for required reagents in player's backpack.
//o---------------------------------------------------------------------------o

char cMagic::CheckReagents(CHARACTER s,  reag_st reagents )
{
	reag_st failmsg;

	if (chars[s].priv2&0x80) return 1;
	memset( &failmsg, 0, sizeof( reag_st ) ); // set all members to 0

	if (reagents.ash!=0 && getamount(s, 0x0F, 0x8C)<reagents.ash) //{RegMsg(s); return 0;}
		failmsg.ash = 1;
	if (reagents.drake!=0 && getamount(s, 0x0F, 0x86)<reagents.drake) //{RegMsg(s); return 0;}
		failmsg.drake = 1;
	if (reagents.garlic!=0 && getamount(s, 0x0F, 0x84)<reagents.garlic) //{RegMsg(s); return 0;}
		failmsg.garlic = 1;
	if (reagents.ginseng!=0 && getamount(s, 0x0F, 0x85)<reagents.ginseng) //{RegMsg(s); return 0;}
		failmsg.ginseng = 1;
	if (reagents.moss!=0 && getamount(s, 0x0F, 0x7B)<reagents.moss) //{RegMsg(s); return 0;}
		failmsg.moss = 1;
	if (reagents.pearl!=0 && getamount(s, 0x0F, 0x7A)<reagents.pearl) //{RegMsg(s); return 0;}
		failmsg.pearl = 1;
	if (reagents.shade!=0 && getamount(s, 0x0F, 0x88)<reagents.shade) //{RegMsg(s); return 0;}
		failmsg.shade = 1;
	if (reagents.silk!=0 && getamount(s, 0x0F, 0x8D)<reagents.silk) //{RegMsg(s); return 0;}
		failmsg.silk = 1;
	
	char fail = (char) RegMsg( s, failmsg );
	
	return fail;
	
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::RegMsg( CHARACTER s )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//|                              Reprogrammed by AntiChrist to display
//|                              missing reagents types.
//o---------------------------------------------------------------------------o
//|     Purpose       :          Display an error message if character has not enough regs.
//o---------------------------------------------------------------------------o

int cMagic::RegMsg(CHARACTER s, reag_st failmsg )
{
	int display = 0;
	char message[100];
	
	strcpy( message, "You do not have enough reagents to cast that spell.[" );
	if( failmsg.ash ) { display = 1; sprintf( message, "%sSa, ", message ); }
	if( failmsg.drake ) { display = 1; sprintf( message, "%sMr, ", message ); }
	if( failmsg.garlic ) { display = 1; sprintf( message, "%sGa, ", message ); }
	if( failmsg.ginseng ) { display = 1; sprintf( message, "%sGi, ", message ); }
	if( failmsg.moss ) { display = 1; sprintf( message, "%sBm, ", message ); }
	if( failmsg.pearl ) { display = 1; sprintf( message, "%sBp, ", message ); }
	if( failmsg.shade ) { display = 1; sprintf( message, "%sNs, ", message ); }
	if( failmsg.silk ) { display = 1; sprintf( message, "%sSs, ", message ); }
	
	message[strlen( message ) - 1] = ']';
	
	if( display )
	{
		int i = calcSocketFromChar( s );
		if( i != -1 )
			sysmessage( i, message );
		return 0;
	}
	return 1;
	
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::DirectDamage( CHARACTER p, int amount )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Inflict the magic damage.
//o---------------------------------------------------------------------------o

void cMagic::DirectDamage( CHARACTER p, int amount)
{
	int s;
	if( chars[p].dead || chars[p].hp <= 0 || (chars[p].priv&0x04) )	// don't kill something already dead
		return;
	if( region[chars[p].region].priv&0x40 )
		return;
	if (chars[p].priv2&0x02)
	{
		chars[p].priv2 = (unsigned char) chars[p].priv2&0xFD;
		s=calcSocketFromChar(p);
		if (s!=-1) sysmessage(s, "You are no longer frozen.");
	}           
	chars[p].hp = max(0, chars[p].hp-amount);
	updatestats( p, 0 );
	if( chars[p].hp <= 0 ) 
		deathstuff( p );
}


//o---------------------------------------------------------------------------o
//|     Class         :          ::PFireballTarget( int i, int k, int j )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Calculate and inflict a magic damage.
//o---------------------------------------------------------------------------o

void cMagic::PFireballTarget(int i, int k, int j) //j = % dammage
{
	int dmg;
	movingeffect(i, k, 0x36, 0xD5, 0x05, 0x00, 0x01);
	soundeffect2(i, 0x1, 0x5E);
	//dmg=(int)(((float)chars[i].hp/100) * j);
	dmg=(int)(((float)chars[k].hp/100 ) * j );
	DirectDamage(k, dmg);
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::SpellFail( UOXSOCKET s )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Do visual and sound effects when a player
//|                              fails to cast a spell.
//o---------------------------------------------------------------------------o

void cMagic::SpellFail( UOXSOCKET s)
{
	// Use Reagents on failure ( if casting from a spellbook )
	if( currentSpellType[s] == 0 ) NewDelReagents( currchar[s], spells[chars[currchar[s]].spellCast].reags );
	staticeffect(currchar[s], 0x37, 0x35, 0, 30);
	soundeffect2(currchar[s], 0x00, 0x5C);
	npcemote(s, currchar[s], "The spell fizzles.", 0);
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////// NPCs CASTING SPELLS RELATED FUNCTIONS ///////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

//o---------------------------------------------------------------------------o
//|     Class         :          ::NPCFireballTarget( CHARACTER nAttacker, CHARACTER nDefender )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Used for NPC; cast a fireball spell.
//o---------------------------------------------------------------------------o

void cMagic::NPCFireballTarget( CHARACTER nAttacker, CHARACTER nDefender)
{
	int src, trg;
	if( CheckMagicReflect( nDefender ) )
	{
		src = nDefender;
		trg = nAttacker;
	}
	else 
	{
		trg = nDefender;
		src = nAttacker;
	}
	
	SubtractMana( nAttacker, 9 );
	movingeffect( src, trg, 0x36, 0xD5, 0x07, 0x00, 0x01 );
	soundeffect2( src, 0x1, 0x5E );
	if( CheckResist( nAttacker, trg, 3 ) )
		MagicDamage( trg, chars[nAttacker].skill[MAGERY] / 280 + 1, src );
	else 
		MagicDamage( trg, chars[nAttacker].skill[MAGERY] / 140 + RandomNum( 1, 4 ), src );
	return;
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::NPCLightningTarget( CHARACTER nAttacker, CHARACTER nDefender )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Used for NPC; cast a lightning spell.
//o---------------------------------------------------------------------------o

void cMagic::NPCLightningTarget( CHARACTER nAttacker, CHARACTER nDefender )
{
	int src, trg;
	if( CheckMagicReflect( nDefender ) )
	{
		src = nDefender;
		trg = nAttacker;
	}
	else 
	{
		trg = nDefender;
		src = nAttacker;
	}
	
	SubtractMana(nAttacker, 11);
	bolteffect(trg);
	soundeffect2(trg, 0x00, 0x29);
	
	if( CheckResist( nAttacker, trg, 4 ) )
		MagicDamage( trg, chars[nAttacker].skill[MAGERY] / 180 + RandomNum( 1, 2 ), src );
	else 
		MagicDamage( trg, chars[nAttacker].skill[MAGERY] / 90 + RandomNum( 1, 5 ), src );
	
	return;
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::NPCCurseTarget( CHARACTER nAttacker, CHARACTER nDefender )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Used for NPC; cast a curse spell.
//o---------------------------------------------------------------------------o

void cMagic::NPCCurseTarget( CHARACTER nAttacker, CHARACTER nDefender)
{
	int i, src, trg;
	
	if (CheckMagicReflect(nDefender))
	{
		src=nDefender;
		trg=nAttacker;
	}
	else 
	{
		trg=nDefender;
		src=nAttacker;
	}
	
	SubtractMana(nAttacker, 11);
	staticeffect(trg, 0x37, 0x4A, 0, 15); 
	soundeffect2(trg, 0x01, 0xE1);     
	
	if(CheckResist(nAttacker, trg, 1)) return;
	
	i=chars[nAttacker].skill[MAGERY]/100;
	tempeffect(src, trg, 12, i, i, i);
	
	return;
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::NPCWeakenTarget( CHARACTER s, CHARACTER t )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Used for NPC; cast a weaken spell.
//o---------------------------------------------------------------------------o

void cMagic::NPCWeakenTarget( CHARACTER s, CHARACTER t)
{
	if (CheckMagicReflect(t)) t=s;
	
	SubtractMana(s, 4);
	staticeffect(t, 0x37, 0x4A, 0, 15); 
	soundeffect2(t, 0x01, 0xE6);     
	
	if(CheckResist(s, t, 1)) return;
	
	tempeffect(s, t, 5, chars[s].skill[MAGERY]/100, 0, 0);
	
	return;
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::NPCFeebleMindTarget( CHARACTER s, CHARACTER t )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Used for NPC; cast a feeblemind spell.
//o---------------------------------------------------------------------------o

void cMagic::NPCFeebleMindTarget( CHARACTER s, CHARACTER t)
{
	if (CheckMagicReflect(t)) t=s;
	
	SubtractMana(s, 4);
	staticeffect(t, 0x37, 0x4A, 0, 15); 
	soundeffect2(t, 0x01, 0xE4);     
	
	if (CheckResist(s, t, 1)) return;
	
	tempeffect(s, t, 4, chars[s].skill[MAGERY]/100, 0, 0);
	
	return;
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::NPCClumsyTarget( CHARACTER s, CHARACTER t )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Used for NPC; cast a clumsy spell.
//o---------------------------------------------------------------------------o

void cMagic::NPCClumsyTarget( CHARACTER s, CHARACTER t)
{
	if (CheckMagicReflect(t)) t=s;
	
	SubtractMana(s, 4);
	staticeffect(t, 0x37, 0x4A, 0, 15); 
	soundeffect2(t, 0x01, 0xDF);
	
	if (CheckResist(s, t, 1)) return;
	
	tempeffect(s, t, 3, chars[s].skill[MAGERY]/100, 0, 0);
	
	return;
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::NPCMindBlastTarget( CHARACTER s, CHARACTER t )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Used for NPC; cast a mindblast spell.
//o---------------------------------------------------------------------------o

void cMagic::NPCMindBlastTarget( CHARACTER s, CHARACTER t )
{
	if( !CheckMagicReflect( t ) )
	{
		SubtractMana( s, 14 );
		staticeffect( t, 0x37, 0x4A, 0, 15 );
		soundeffect2( s, 0x02, 0x13 );
		
		if( CheckResist( s, t, 5 ) )
		{
			MagicDamage( t, max( ( ( chars[s].in - chars[t].in ) / 4 ), 2 ), s );
		}
		else
		{
			MagicDamage( t, max( ( ( chars[s].in - chars[t].in ) / 2 ), 5 ), s );
		}
		return;
	} 
	else 
	{
		staticeffect(s, 0x37, 0x4A, 0, 15);
		soundeffect2(t, 0x02, 0x13);
		
		if (chars[s].in>chars[t].in)
		{
			if (CheckResist(s, t, 5))
			{
				MagicDamage(t, max(((chars[s].in-chars[t].in)/2), 5), s );
			}
			else
			{
				MagicDamage(t, max(((chars[s].in-chars[t].in)/2), 5), s );
			}
		}
		else
		{
			if (CheckResist(t, s, 5))
			{
				MagicDamage(s, max(((chars[s].in-chars[t].in)/2), 5), t );
			}
			else
			{
				MagicDamage(s, max(((chars[s].in-chars[t].in)/2), 5), t );
			}
		}
		return;
	}
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::NPCMagicArrowTarget( CHARACTER s, CHARACTER t )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Used for NPC; cast a magicarrow spell.
//o---------------------------------------------------------------------------o

void cMagic::NPCMagicArrowTarget( CHARACTER s, CHARACTER t)
{
	int src, trg;
	
	//npcattacktarget(t, s);
	if (CheckMagicReflect(t)) 
	{
		src=t;
		trg=s;
	}
	else
	{
		src=s;
		trg=t;
	}
	SubtractMana(src, 4);
	movingeffect(src, trg, 0x36, 0xE4, 0x07, 0x00, 0x01); 
	soundeffect2(src, 0x1, 0xE5);
	
	if (CheckResist(src, trg, 1))
		MagicDamage(trg, (2+(rand()%3)+1)*(chars[src].skill[MAGERY]/1000+1), src );
	else 
		MagicDamage(trg, (2+(rand()%3)+2)*(chars[src].skill[MAGERY]/750+1), src );
	
	return;
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::NPCHarmTarget( CHARACTER s, CHARACTER t )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Used for NPC; cast a harm spell.
//o---------------------------------------------------------------------------o

void cMagic::NPCHarmTarget( CHARACTER s, CHARACTER t)
{
	int trg, src;
	if( CheckMagicReflect( t ) ) 
	{
		trg = s;
		src = t;
	}
	else
	{
		trg = t;
		src = s;
	}
	SubtractMana( s, 6 );
	staticeffect( trg, 0x37, 0x4A, 0x09, 0x06 );
	soundeffect2( trg, 0x01, 0xF1 );
	if( CheckResist( src, trg, 2 ) )
		MagicDamage( trg, chars[s].skill[MAGERY]/500+1, src );
	else 
		MagicDamage( trg, chars[s].skill[MAGERY]/250+RandomNum(1,2), src );
	return;
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::NPCDispel(CHARACTER s, CHARACTER i)
//|     Date          :          2000.12.02
//|     Programmer    :          kkung, From WolfPack
//o---------------------------------------------------------------------------o
//|     Purpose       :          Used for NPC; cast a dispel to summon monster.
//o---------------------------------------------------------------------------o
// NPCDispel 함수 - 제작 kkung , 출처 : WolfPack 

void cMagic::NPCDispel( CHARACTER s, CHARACTER i )
{
    int loskill = spells[41].loskill;
    int hiskill = spells[41].hiskill;
	if( !Skills->CheckSkill( s, MAGERY, loskill, hiskill ) )
	{
		UOXSOCKET ss = calcSocketFromChar( s );
		if( ss > -1 )
		{
		   SpellFail( ss );
		}
		return;	
	}
	if( Magic->CheckMana( s, 41 ) )
	{
		if( chars[i].priv2&0x20 )
		{
			Magic->SubtractMana(s,20);
			staticeffect( i, 0x37, 0x2A, 0x09, 0x06 );
			if( chars[i].npc ) 
				Npcs->DeleteChar( i );
			else 
				deathstuff( i );
		}
	}
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::NPCParalyzeTarget( CHARACTER s, CHARACTER t )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Used for NPC; cast a paralyze spell.
//o---------------------------------------------------------------------------o

void cMagic::NPCParalyzeTarget( CHARACTER s, CHARACTER t )
{
	int src, trg;
	if(CheckMagicReflect(t)) 
	{
		trg = s;
		src = t;
	}
	else
	{
		trg = t;
		src = s;
	}
	
	SubtractMana(s, 14);
	soundeffect2( trg, 0x02, 0x04);     
	
	if (CheckResist(src, trg, 5)) return;
	
	tempeffect(src, trg, 1, 0, 0, 0);
	
	return;
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::NPCEBoltTarget( CHARACTER s, CHARACTER t )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Used for NPC; cast an energy bolt spell.
//o---------------------------------------------------------------------------o

void cMagic::NPCEBoltTarget( CHARACTER s, CHARACTER t)
{
	int src, trg;
	if (CheckMagicReflect(t))
	{
		src=t;
		trg=s;
	}
	else 
	{
		src=s;
		trg=t;
	}
	SubtractMana(src, 20);
	movingeffect(src, t, 0x37, 0x9F, 0x07, 0x00, 0x01);
	soundeffect2(src, 0x2, 0x0A);
	
	if (CheckResist(s, t, 6))
		MagicDamage( t, chars[s].skill[MAGERY]/120, s );
	else 
		MagicDamage( t, chars[s].skill[MAGERY]/35+RandomNum(1,10), s );
	
	return;
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::NPCExplosionTarget( CHARACTER s, CHARACTER t )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Used for NPC; cast an explosion spell.
//o---------------------------------------------------------------------------o

void cMagic::NPCExplosionTarget(CHARACTER s, CHARACTER t)
{
	unsigned int i;
	int src, trg;
	if( CheckMagicReflect( t ) )
	{
		src = t;
		trg = s;
	}
	else
	{
		src = s;
		trg = t;
	}
	
	SubtractMana(s, 20);
	staticeffect( trg, 0x36, 0xB0, 0x09, 0x09);
	soundeffect2( trg, 0x02, 0x07);   
	
	//Char mapRegions
	int StartGrid=mapRegions->StartGrid( chars[src].x, chars[src].y );
	
	unsigned int increment=0;
	for (unsigned int checkgrid=StartGrid+(increment*mapRegions->GetColSize());increment<3;increment++, checkgrid=StartGrid+(increment*mapRegions->GetColSize()))
	{
		for (int a=0;a<3;a++)
		{
			int mapitemptr=-1;
			int	mapitem=-1;
			int mapchar=-1;
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
					if( chars[i].x == chars[trg].x && chars[i].y == chars[trg].y && chars[i].z == chars[trg].z )
					{
						if( CheckResist(src, i, 6))
							MagicDamage(i, chars[src].skill[MAGERY]/120+RandomNum(1,5), src );
						else 
							MagicDamage(i, chars[src].skill[MAGERY]/40+RandomNum(1,10), src );
					}
				}//if mapitem
			}while(mapitem!=-1);
		}
	}
	return;
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::CheckParry( CHARACTER player, int circle )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Check player's parrying skill (for cannonblast).
//o---------------------------------------------------------------------------o

char cMagic::CheckParry( CHARACTER player, int circle)
{
	char i=Skills->CheckSkill(player, PARRYING, 80*circle, 800+(80*circle));;
	int s;
    if(i)
	{
		s=calcSocketFromChar(player);
		if (s!=-1)
		{
			sysmessage(s, "You have dogged the cannon blast, and have taken less damage.");
		}
	}
	return i;
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::NPCFlameStrikeTarget( CHARACTER s, CHARACTER t )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Used for NPC; cast a flame strike  spell.
//o---------------------------------------------------------------------------o

void cMagic::NPCFlameStrikeTarget(CHARACTER s, CHARACTER t)
{
	int src, trg;
	if( CheckMagicReflect( t ) ) 
	{
		trg = s;
		src = t;
	}
	else
	{
		trg = t;
		src = s;
	}
	
	SubtractMana( s, 40);
	staticeffect( trg, 0x37, 0x09, 0x09, 0x19 );
	soundeffect2( trg, 0x02, 0x08 );
	
	if( CheckResist( src, trg, 7 ) )
		MagicDamage( trg, chars[s].skill[MAGERY]/80, src );
	else 
		MagicDamage( trg, chars[s].skill[MAGERY]/40+RandomNum(1,25), src );
	return;
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////// ITEMs MAGIC POWER RELATED FUNCTIONS /////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////


//o---------------------------------------------------------------------------o
//|     Class         :          ::MagicArrowSpellItem( CHARACTER attaker, CHARACTER defender )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Used for ITEMS; cast a magic arrow spell.
//o---------------------------------------------------------------------------o

void cMagic::MagicArrowSpellItem( CHARACTER attaker, CHARACTER defender)
{
	int  src;
	if( attaker <0 || defender < 0 ) return;
	if (CheckMagicReflect(defender)) 
	{
		src=defender;
		defender=attaker;
	}
	else
	{
		src=attaker;
	}
	movingeffect(src, defender, 0x36, 0xE4, 0x05, 0x00, 0x01); 
	soundeffect2(src, 0x1, 0xE5);
	if (CheckResist(src, defender, 1))
	{
		MagicDamage(defender, (1+(rand()%1)+1)*(chars[src].skill[MAGERY]/2000+1), src );
	}
	else 
	{
		MagicDamage(defender, (1+(rand()%1)+2)*(chars[src].skill[MAGERY]/1500+1), src );
	}
	return;
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::ClumsySpellItem( CHARACTER attaker, CHARACTER defender )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Used for ITEMS; cast a clumsy spell.
//o---------------------------------------------------------------------------o

void cMagic::ClumsySpellItem( CHARACTER attaker, CHARACTER defender)
{
	if (CheckMagicReflect(defender)) 
	{
		int t = defender;
		defender = attaker;
		attaker = t;
	}
	staticeffect(defender, 0x37, 0x4A, 0, 15); 
	soundeffect2(defender, 0x01, 0xDF);
	if (CheckResist(attaker, defender, 1)) return;		
	tempeffect(attaker, defender, 3, chars[attaker].skill[MAGERY]/100, 0, 0);
	return;
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::FeebleMindSpellItem( CHARACTER attaker, CHARACTER defender )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Used for ITEMS; cast a feeble mind spell.
//o---------------------------------------------------------------------------o

void cMagic::FeebleMindSpellItem( CHARACTER attaker, CHARACTER defender)
{
	if (CheckMagicReflect(defender)) 
	{
		int t = defender;
		defender = attaker;
		attaker = t;
	}
	staticeffect(defender, 0x37, 0x4A, 0, 15); 
	soundeffect2(defender, 0x01, 0xE4);     
	if (CheckResist(attaker, defender, 1)) return;
	tempeffect(attaker, defender, 4, chars[attaker].skill[MAGERY]/100, 0, 0);
	return;
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::WeakenSpellItem( CHARACTER attaker, CHARACTER defender )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Used for ITEMS; cast a weaken spell.
//o---------------------------------------------------------------------------o

void cMagic::WeakenSpellItem( CHARACTER attaker, CHARACTER defender)
{
	if (CheckMagicReflect(defender))
	{
		int t = defender;
		defender = attaker;
		attaker = t;
	}
	staticeffect(defender, 0x37, 0x4A, 0, 15); 
	soundeffect2(defender, 0x01, 0xE6);     
	if(CheckResist(attaker, defender, 1)) return;
	tempeffect(attaker, defender, 5, chars[attaker].skill[MAGERY]/100, 0, 0);
	return;
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::HarmSpellItem( CHARACTER attaker, CHARACTER defender )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Used for ITEMS; cast a harm spell.
//o---------------------------------------------------------------------------o

void cMagic::HarmSpellItem( CHARACTER attaker, CHARACTER defender)
{
	if (CheckMagicReflect(defender)) 
	{
		int t = defender;
		defender = attaker;
		attaker = t;
	}
	staticeffect(defender, 0x37, 0x4A, 0x09, 0x06);
	soundeffect2(defender, 0x01, 0xF1);     
	if (CheckResist(attaker, defender, 2))
	{
		MagicDamage(defender, chars[attaker].skill[MAGERY]/500+1, attaker );
	}
	else 
	{
		MagicDamage(defender, chars[attaker].skill[MAGERY]/250+RandomNum(1,2), attaker );
	}
	return;
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::FireballSpellItem( CHARACTER attaker, CHARACTER defender )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Used for ITEMS; cast a fireball spell.
//o---------------------------------------------------------------------------o

void cMagic::FireballSpellItem( CHARACTER attaker, CHARACTER defender)
{
	if (CheckMagicReflect(defender))
	{
		int t = defender;
		defender = attaker;
		attaker = t;
	}
	movingeffect(attaker, defender, 0x36, 0xD5, 0x07, 0x00, 0x01);
	soundeffect2(attaker, 0x1, 0x5E);
	if( CheckResist( attaker, defender, 3 ) ) 
		MagicDamage( defender, chars[attaker].skill[MAGERY]/280+1, attaker );
	else 
		MagicDamage( defender, chars[attaker].skill[MAGERY]/140+RandomNum(1,4), attaker );
	
	return;
}
//o---------------------------------------------------------------------------o
//|     Class         :          ::CurseSpellItem( CHARACTER attaker, CHARACTER defender )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Used for ITEMS; cast a curse spell.
//o---------------------------------------------------------------------------o

void cMagic::CurseSpellItem( CHARACTER attaker, CHARACTER defender)
{
	int j;
	if (CheckMagicReflect(defender))
	{
		int t = defender;
		defender = attaker;
		attaker = t;
	}
	staticeffect(defender, 0x37, 0x4A, 0, 15); 
	soundeffect2(defender, 0x01, 0xE1);     
	if(CheckResist(attaker, defender, 1)) return;
	j=chars[attaker].skill[MAGERY]/100;
	tempeffect(attaker, defender, 12, j, j, j);
	return;
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::LightningSpellItem( CHARACTER attaker, CHARACTER defender )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Used for ITEMS; cast a lightning spell.
//o---------------------------------------------------------------------------o

void cMagic::LightningSpellItem( CHARACTER attaker, CHARACTER defender )
{
	if (CheckMagicReflect(defender)) 
	{
		int t = defender;
		defender = attaker;
		attaker = t;
	}
	bolteffect(defender);
	soundeffect2(defender, 0x00, 0x29);
	if (CheckResist(attaker, defender, 4))
	{
		MagicDamage( defender, chars[attaker].skill[MAGERY]/180+RandomNum(1,2), attaker );
	}
	else 
	{
		MagicDamage(defender, chars[attaker].skill[MAGERY]/90+RandomNum(1,5), attaker );
	}
	return;
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::MindBlastSpellItem( CHARACTER attaker, CHARACTER defender )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Used for ITEMS; cast a mind blast spell.
//o---------------------------------------------------------------------------o

void cMagic::MindBlastSpellItem( CHARACTER attaker, CHARACTER defender)
{
	if( CheckMagicReflect( defender ) )
	{
		int t = defender;
		defender = attaker;
		attaker = t;
	}
	
	staticeffect( defender, 0x37, 0x4A, 0, 15 );
	soundeffect2( defender, 0x02, 0x13 );
	if( chars[attaker].in > chars[defender].in )
	{
		if( CheckResist( attaker, defender, 5 ) )
			MagicDamage( defender, (chars[attaker].in-chars[defender].in)/4, attaker );
		else
			MagicDamage( defender, (chars[attaker].in-chars[defender].in)/2, attaker );
	}
	else
	{
		if( CheckResist( defender, attaker, 5 ) )
			MagicDamage( attaker, (chars[defender].in-chars[attaker].in)/4, defender );
		else
			MagicDamage( attaker, (chars[defender].in-chars[attaker].in)/2, defender );
	}
	
	return;
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::ParalyzeSpellItem( CHARACTER attaker, CHARACTER defender )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Used for ITEMS; cast a paralyze spell.
//o---------------------------------------------------------------------------o

void cMagic::ParalyzeSpellItem(CHARACTER attaker, CHARACTER defender)
{
	if (CheckMagicReflect(defender))
	{
		int t = defender;
		defender=attaker;
		attaker = t;
	}
	//staticeffect(currchar[s], 0x37, 0x35, 0, 30); //No effect for paralyze?
	soundeffect2(defender, 0x02, 0x04);     
	if (CheckResist(attaker, defender, 5)) return;
	tempeffect(attaker, defender, 1, 0, 0, 0);
	return;
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::ExplosionSpellItem( CHARACTER attaker, CHARACTER defender )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Used for ITEMS; cast an explosion spell.
//o---------------------------------------------------------------------------o

void cMagic::ExplosionSpellItem( CHARACTER attaker, CHARACTER defender)
{
	if (CheckMagicReflect(defender))
	{
		int t = defender;
		defender=attaker;
		attaker = t;
	}
	staticeffect(defender, 0x36, 0xB0, 0x09, 0x09);
	soundeffect2(defender, 0x02, 0x07);   
	MagicDamage( defender, chars[attaker].skill[MAGERY]/120 + RandomNum( 1, 5 ), attaker );
	return;
}

//o---------------------------------------------------------------------------o
//|     Class         :          ::FlameStrikeSpellItem( CHARACTER attaker, CHARACTER defender )
//|     Date          :          Unknown
//|     Programmer    :          Unknown
//o---------------------------------------------------------------------------o
//|     Purpose       :          Used for ITEMS; cast a flame strike spell.
//o---------------------------------------------------------------------------o

void cMagic::FlameStrikeSpellItem( CHARACTER attaker, CHARACTER defender)
{
	if (CheckMagicReflect(defender)) 
	{
		int t = defender;
		defender=attaker;
		attaker = t;
	}
	staticeffect(defender, 0x37, 0x09, 0x09, 0x19);
	soundeffect2(defender, 0x02, 0x08);     
	MagicDamage( defender, chars[attaker].skill[MAGERY]/(40 + RandomNum( 1, 25 ) ), attaker );
	return;
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////// PCs CASTING SPELLS RELATED FUNCTIONS ////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////


//o---------------------------------------------------------------------------o
//|     Class         :          ::newSelectSpell2Cast( UOXSOCKET s, int num )
//|     Date          :          28 August 1999 / 10 September 1999
//|     Programmer    :          Abaddon / AntiChrist
//o---------------------------------------------------------------------------o
//|     Purpose       :          Execute the selected spell to cast.
//o---------------------------------------------------------------------------o

bool cMagic::newSelectSpell2Cast( UOXSOCKET s, int num )
// PRE:		s is a valid socket, num is a valid spell number
// POST:	Spell selected to cast
// Comments: Written by Abaddon (28 August 1999)
//:Terrin: Use spells (memory version of script info )
//Abaddon: Made memory optionable
//Abaddon: Perma cached
{
	int success = 1;
	int loskill=0, hiskill=0;
	
	splInfo_st curSpellCasting;
	
	int i=0;	
	
	int type = currentSpellType[s];
	chars[currchar[s]].spellCast = num;
	curSpellCasting = spells[num];

	if( chars[currchar[s]].spelltime > uiCurrentTime )
	{
		switch( chars[currchar[s]].casting )
		{
		case 1:
		case -1:
			sysmessage( s, "You are already casting a spell" );
			return false;
		default:
		case 0:
			sysmessage( s, "Your last attempt failed and you must wait a little longer before casting" );
			return false;
		}
	}
	if ((chars[currchar[s]].cell !=0) && (!chars[currchar[s]].priv&0x01))
	{
		sysmessage(s,"You are in jail and cannot cast spells");
		chars[currchar[s]].spellCast = 0;
		chars[currchar[s]].casting = 0;
		return false;
	}
	int curRegion = chars[currchar[s]].region;
	
	if( !(region[curRegion].priv&0x40) && aggressiveSpell( chars[currchar[s]].spellCast ) ) // ripper 9-11-99 not in town :)
	{
		sysmessage( s, "This is not allowed in town." );
		chars[currchar[s]].spellCast = 0;
		chars[currchar[s]].casting = 0;
		return false;
	}
	
	if( !curSpellCasting.enabled  )
	{
		sysmessage( s, "That spell is currently not enabled" );
		chars[currchar[s]].spellCast = 0;
		chars[currchar[s]].casting = 0;
		return false;
	}
	// (Abaddon) Region checks
	unsigned char regByte = region[chars[currchar[s]].region].priv;
	if( (!(regByte&0x02) && num == 45) || (!(regByte&0x04) && num == 52) || (!(regByte&0x08) && num == 32) )
	{
		sysmessage( s, "This is not allowed here" );
		chars[currchar[s]].spellCast = 0;
		chars[currchar[s]].casting = 0;
		return false;
	}
	
	//Cut the casting requirment on scrolls
	if (type==1 && server_data.cutscrollreq ) // only if enabled
	{
		loskill = curSpellCasting.sclo;
		hiskill = curSpellCasting.schi;
	}
	else
	{
		loskill = curSpellCasting.loskill;
		hiskill = curSpellCasting.hiskill;
	}
	
	
	// The following loop checks to see if any item is currently equipped (if not a GM)
	if (!chars[currchar[s]].priv&0x01)
	{
		int serial, serhash, ci;
		serial = chars[currchar[s]].serial;
		serhash = serial%HASHMAX;
		for( ci = 0; ci < contsp[serhash].max; ci++ )
		{
			i = contsp[serhash].pointer[ci];
			if( i != -1 )
				if(( items[i].contserial == serial ))
				{
					if( type != 2 && ( items[i].layer == 2 || ( items[i].layer == 1 && items[i].type != 9 )))
					{
						sysmessage( s, "You cannot cast with a weapon equipped." );
						chars[currchar[s]].spellCast = 0;
						chars[currchar[s]].casting = 0;
						return false;
					}
				}
		}
	}
	
	if ((chars[currchar[s]].hidden)&&(!(chars[currchar[s]].priv2&8)))
	{
		chars[currchar[s]].hidden=0;
		chars[currchar[s]].stealth=-1;
		updatechar(currchar[s]);
	}
	breakConcentration( currchar[s], s );
	
	if (!(chars[currchar[s]].priv&0x01))
	{
		//Check for enough reagents
		if( type == 0 && (!CheckReagents( currchar[s], curSpellCasting.reags ) ) )
		{
			chars[currchar[s]].spellCast = 0;
			chars[currchar[s]].casting = 0;
			return false;
		}
		
		if( type != 2 )
		{
			if ((chars[currchar[s]].mn < curSpellCasting.mana) && !(chars[currchar[s]].priv2&0x10)) // was 0x01, thats allmove !!!
			{
				success=0;
				sysmessage(s, "You have insufficient mana to cast that spell.");
				chars[currchar[s]].spellCast = 0;
				chars[currchar[s]].casting = 0;
				return false;
			}
			if ((chars[currchar[s]].stm < curSpellCasting.stamina) && !(chars[currchar[s]].priv2&0x10)) // was 0x01, thats allmove !!!
			{
				sysmessage(s, "You have insufficient stamina to cast that spell.");
				chars[currchar[s]].spellCast = 0;
				chars[currchar[s]].casting = 0;
				return false;
			}
			if((chars[currchar[s]].hp < curSpellCasting.health) && !(chars[currchar[s]].priv2&0x10)) // was 0x01, thats allmove !!!
			{
				sysmessage(s, "You have insufficient health to cast that spell.");
				chars[currchar[s]].spellCast = 0;
				chars[currchar[s]].casting = 0;
				return false;
			}
			//Check success of casting Except for GM's
			if( !Skills->CheckSkill( currchar[s], MAGERY, loskill, hiskill ) ) //Morrolan - this was again checking Allmove
			{
				npctalkall( currchar[s], curSpellCasting.mantra, 0 );
				SpellFail( s );
				chars[currchar[s]].spellCast = 0;
				chars[currchar[s]].casting = 0;
				chars[currchar[s]].spelltime = (unsigned int) (((curSpellCasting.delay / 10) * CLOCKS_PER_SEC) + uiCurrentTime);
				return false;
			}
		}
	}	
	   
	chars[currchar[s]].casting = 1;
	chars[currchar[s]].spellaction = curSpellCasting.action;
	chars[currchar[s]].nextact = 75;		// why 75?
	if( type==0 && (!(chars[currchar[s]].priv&1 ))) // if they are a gm they don't have a delay :-)
	{
		chars[currchar[s]].spelltime = (unsigned int) (((curSpellCasting.delay / 10) * CLOCKS_PER_SEC) + uiCurrentTime);
		chars[currchar[s]].priv2 = (unsigned char) chars[currchar[s]].priv2 | 2; //freeze
	}
	else
	{
		chars[currchar[s]].spelltime = 0;
	}
	
	if( !chars[currchar[s]].onhorse )
		impaction( s, curSpellCasting.action ); // do the action
	
	npctalkall( currchar[s], curSpellCasting.mantra, 0 );
	chars[currchar[s]].casting = 1;
	return true;
	
}

void cMagic::NewCastSpell( UOXSOCKET s )
{
	// for LocationTarget spell like ArchCure, ArchProtection etc...
	int range, distx, disty, x1, y1, x2, y2, z1, z2;
	unsigned int ii;
	int StartGrid;
	int getcell;
	int dmg, dmgmod;
	unsigned int increment;
	unsigned int checkgrid;
	int mapitemptr = -1;
	int mapitem = -1;
	int mapchar = -1;
	int curSpell = chars[currchar[s]].spellCast;
	if( chars[currchar[s]].casting == 0 && chars[currchar[s]].spellCast == -1 )
		return;		// it was cancelled
	chars[currchar[s]].casting = 0;
	chars[currchar[s]].spellCast = -1;
	int i, defender, serial;
	int src=currchar[s];
	int item;
	int c, a;
	double d;
	bool recalled;
	int n;
	int j;
	int x, y, z, dx, dy, dz;
	int fx[5], fy[5];
	unsigned char id1, id2;
	
	// AntiChrist-
	// ONLY NOW we can delete reagents & mana!
	// so if clients decided to abort.. no problems..... mana and
	// reagents not consumed!
	if( currentSpellType[s] != 2 )
	{
		SubtractMana( currchar[s], spells[curSpell].mana );
		if( spells[curSpell].health > 0 ) 
			SubtractHealth( currchar[s], spells[curSpell].health, curSpell );
		SubtractStamina( currchar[s], spells[curSpell].stamina );
	}
	if( currentSpellType[s] == 0 ) NewDelReagents( currchar[s], spells[curSpell].reags );
	
	if( aggressiveSpell( curSpell ) && !(region[chars[currchar[s]].region].priv&0x40) )
	{
		sysmessage( s, "You can't cast in town!" );
		return;
	}
	
	if( requireTarget( curSpell ) )					// target spells if true
	{
		if( travelSpell( curSpell )	)				// travel spells.... mark, recall and gate
		{
			// mark, recall and gate go here
			item = calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);	// item we are targeting
			i = findbyserial(&itemsp[item%HASHMAX], item, 0);  // was searching for char
			
			unsigned char regByte = region[chars[currchar[s]].region].priv;
			if( (!(regByte&0x02) && curSpell == 45) || (!(regByte&0x04) && curSpell == 52) || (!(regByte&0x08) && curSpell == 32) )
			{
				sysmessage( s, "This is not allowed here" );
				chars[currchar[s]].spellCast = 0;
				chars[currchar[s]].casting = 0;
				return;
			}
			if( i != -1 )
			{
				if( items[i].contserial != -1 || line_of_sight( s, chars[currchar[s]].x, chars[currchar[s]].y, chars[currchar[s]].z, items[i].x, items[i].y, items[i].z, WALLS_CHIMNEYS+DOORS+FLOORS_FLAT_ROOFING ) || chars[currchar[s]].priv&0x01 ) // bugfix LB
				{
					if( ( items[i].type==50) )
					{
						playSound( src, curSpell );
						doMoveEffect( curSpell, i, src );
						doStaticEffect( src, curSpell );
						switch( curSpell )
						{
							//////////// (32) RECALL ////////////////
						case 32:
							if ( items[i].morex <= 200 && items[i].morey <= 200 )
							{
								sysmessage(s,"That rune has not been marked yet!");
								recalled = false;
							} 
							else if (!chars[currchar[s]].priv&1 && Weight->CheckWeight2(currchar[s])) //Morrolan no recall if too heavy, GM's excempt
							{
								sysmessage(s, "You are too heavy to do that!");
								sysmessage(s, "You feel drained from the attempt.");
								statwindow(s, currchar[s]);
								recalled=false;
							}
							else
							{
								mapRegions->RemoveItem(currchar[s]+1000000); //LB
								chars[currchar[s]].x = (short int) items[i].morex;
								chars[currchar[s]].y = (short int) items[i].morey;
								chars[currchar[s]].dispz=chars[currchar[s]].z=items[i].morez;
								mapRegions->AddItem(currchar[s]+1000000); //LB
								teleport(currchar[s]);
								sysmessage(s,"You have recalled from the rune.");
								recalled=true;
							}
							break;
							//////////// (45) MARK //////////////////
						case 45:
							items[i].morex=chars[currchar[s]].x;
							items[i].morey=chars[currchar[s]].y;
							items[i].morez=chars[currchar[s]].z;
							if (region[chars[currchar[s]].region].name[0]!=0)
								sprintf(items[i].name, "a recall rune for %s",region[chars[currchar[s]].region].name);
							else 
								strcpy(items[i].name, " a recall rune for An Unknown Location");
							
							sysmessage(s,"Recall rune marked."); 
							break;
							//////////// (52) GATE //////////////////
						case 52:
							if ( items[i].morex<=200 && items[i].morey<=200 )
							{
								sysmessage( s, "That rune has not been marked yet!" );
								recalled = false;
							}
							else
							{
								gatex[gatecount][0] = chars[currchar[s]].x+1;  //create gate a player location
								gatey[gatecount][0] = chars[currchar[s]].y+1;
								gatez[gatecount][0] = chars[currchar[s]].z;
								
								gatex[gatecount][1] = items[i].morex; //create gate at marked location
								gatey[gatecount][1] = items[i].morey;
								gatez[gatecount][1] = items[i].morez;
								recalled=true;
								
								for (n=0;n<2;n++)  
								{
									strcpy(temp,"a blue moongate");
									c=Items->SpawnItem(s,1,"#",0,0x0f,0x6c,0,0,0,0);
									if( c > -1 )
									{
										items[c].type=51+n;
										items[c].x = gatex[gatecount][n];
										items[c].y = gatey[gatecount][n];
										items[c].z = gatez[gatecount][n];
										items[c].gatetime = (unsigned int) (uiCurrentTime + (server_data.gatetimer * CLOCKS_PER_SEC));
										items[c].gatenumber=gatecount;
										items[c].dir=1;
										
										mapRegions->AddItem(c);  //add gate to list of items in the region
										RefreshItem( c ); // AntiChrist
									}
									if (n==1)
									{
										gatecount++;
										if (gatecount>MAXGATES) gatecount=0;
									}
								}
							}
							break;
						default:
							printf("MAGIC-ERROR: Unknown Travel spell %i, magic.cpp\n", curSpell );
							break;
						}
					}
					else
						sysmessage( s, "That item is not a recall rune." );
				}
			}
			else
				sysmessage( s, "Not a valid target on item!" );
			chars[currchar[s]].spellCast = 0;
			chars[currchar[s]].casting = 0;
			return;
		}
		
		if( reqCharTarget( curSpell ) )
		{
			// TARGET CALC HERE
			defender = calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);	// character we are attacking is HERE
			i = calcCharFromSer( defender );
			
			// IF TARGET VALID
			if( i != -1 )																	// we have to have targetted a person to kill them :)
			{
				if( chardist( i, currchar[s] ) > combat.maxRangeSpell )
				{
					sysmessage( s, "You can't cast on someone that far away!" );
					return;
				}
				if ((line_of_sight( s, chars[currchar[s]].x, chars[currchar[s]].y, chars[currchar[s]].z, chars[i].x, chars[i].y,chars[i].z,WALLS_CHIMNEYS+DOORS+FLOORS_FLAT_ROOFING)||
					(chars[currchar[s]].priv&0x01)))
				{
					if( aggressiveSpell( curSpell ) )
					{
						if( !(region[chars[currchar[s]].region].priv&0x40) ) // only magic damage stuff 
						{
							sysmessage( s, "You can't cast in town!" );
							return;
						}
						if( chars[i].npcaitype == 17 || chars[i].priv&0x04 )
						{
							sysmessage( s, "They are invulnerable!" );
							return;
						}
						npcattacktarget( i, currchar[s] );
						if( chars[i].flag&0x04 && i != currchar[s] )
							criminal( currchar[s] );
					}
					if( spellReflectable( curSpell ) )
					{
						if (CheckMagicReflect(i))
						{
							src=i;
							i=currchar[s];
						}
					}
					else 
					{
						src=currchar[s];
					}
					if( curSpell != 43 )
					{
						playSound( src, curSpell );
						doMoveEffect( curSpell, i, src );
						doStaticEffect( i, curSpell );
					}
					switch( curSpell )
					{
					case 1:  // Clumsy
						tempeffect( src, i, 3, chars[currchar[s]].skill[MAGERY]/100, 0, 0);
						break; 
					case 3:  // Feeblemind
						tempeffect( src, i, 4, chars[currchar[s]].skill[MAGERY]/100, 0, 0);
						break; 
					case 4:    // Heal 2-26-00 changed by Homey, used OSI values from UO book
						int bonus;//also fixes problem with low mages getting 1 or nothing from heal spell
						bonus = (chars[currchar[s]].skill[MAGERY]/500) + ( chars[currchar[s]].skill[MAGERY]/100 );
						if( bonus != 0 )
							chars[i].hp = ( chars[i].hp + rand()%6 + bonus );
						else
							chars[i].hp = chars[i].hp+4;
						SubtractHealth( currchar[s], bonus, curSpell );
						updatestats( i, 0 );  
						break; 
					case 5:  // Magic Arrow
						MagicDamage(i, (1+(rand()%1)+1)*(chars[currchar[s]].skill[MAGERY]/2000+1), src );
						break; 
					case 6:  // Night Sight
						tempeffect( src, i, 2, 0, 0, 0);
						break; 
					case 7:
						tempeffect( src, i, 15, chars[currchar[s]].skill[MAGERY]/100, 0, 0 );
						chars[i].ra=1;
						break;
					case 8:  // Weaken
						tempeffect( src, i, 5, chars[currchar[s]].skill[MAGERY]/100, 0, 0);
						break; 
					case 9:  // Agility
						tempeffect( src, i, 6, chars[currchar[s]].skill[MAGERY]/100, 0, 0);
						break; 
					case 10: // Cunning
						tempeffect( src, i, 7, chars[currchar[s]].skill[MAGERY]/100, 0, 0);
						break; 
					case 11: // Cure
						chars[i].poisoned=0;
						chars[i].poisonwearofftime=uiCurrentTime;
						impowncreate(s,i,1); // updating to blue bar
						break; 
					case 12: // Harm
						if (CheckResist(currchar[s], i, 2))
							MagicDamage(i, chars[currchar[s]].skill[MAGERY]/500+1, src );
						else 
							MagicDamage(i, chars[currchar[s]].skill[MAGERY]/250+RandomNum(1,2), src );
						break; 
					case 15: // Protection
						// Boosts parrying
						tempeffect( src, i, 21, chars[currchar[s]].skill[MAGERY]/10, 0, 0 );
						break; 
					case 16: // Strength
						tempeffect( src, i, 8, chars[currchar[s]].skill[MAGERY]/100, 0, 0);
						break; 
					case 17: // Bless
						j=chars[currchar[s]].skill[MAGERY]/100;
						tempeffect( src, i, 11, j, j, j);
						break; 
					case 18: // Fireball
						if (CheckResist(currchar[s], i, 3))
							MagicDamage(i, chars[currchar[s]].skill[MAGERY]/280+1, src );
						else 
							MagicDamage(i, chars[currchar[s]].skill[MAGERY]/140+RandomNum(1,4), src );
						break; 
					case 20: // Poison
						if(CheckResist(currchar[s], i, 1)) return;
						{
							chars[i].poisoned=2;   
							chars[i].poisonwearofftime = (unsigned int) (uiCurrentTime + (CLOCKS_PER_SEC * server_data.poisontimer)); 
							impowncreate(s, i, 1); //Lb, sends the green bar ! 
						}
						break; 
					case 27: // Curse
						if(!CheckResist(currchar[s], i, 1))
						{
							j=chars[currchar[s]].skill[MAGERY]/100;
							tempeffect(currchar[s], i, 12, j, j, j);
						}
						break; 
					case 29: // Greater Heal
						j=chars[i].hp+(chars[currchar[s]].skill[MAGERY]/30+RandomNum(1,12));
						chars[i].hp = min(chars[i].st, j);
						updatestats(i, 0);        
						SubtractHealth( currchar[s], min( chars[i].st, j ), curSpell );
						break; 
					case 30: // Lightning
						bolteffect(i);
						if (CheckResist(currchar[s], i, 4))
							MagicDamage(i, chars[currchar[s]].skill[MAGERY]/180+RandomNum(1,2), src );
						else 
							MagicDamage(i, chars[currchar[s]].skill[MAGERY]/90+RandomNum(1,5), src );
						break; 
					case 31: // Mana drain
						if(!CheckResist(currchar[s], i, 4))
						{
							chars[i].mn-=chars[currchar[s]].skill[MAGERY]/35;
							if (chars[i].mn<0) chars[i].mn=0;
							updatestats(i, 1);
						}
						break; 
					case 37: // Mind Blast
						if (chars[currchar[s]].in>chars[i].in)
						{
							if (CheckResist(currchar[s], i, 5))
								MagicDamage(i, (chars[src].in-chars[i].in)/4, src );
							else
								MagicDamage(i, (chars[src].in-chars[i].in)/2, src );
						}
						else
						{
							if (CheckResist(currchar[s], src, 5))
								MagicDamage(src, (chars[i].in-chars[src].in)/4, i );
							else
								MagicDamage(src, (chars[i].in-chars[src].in)/2, i );
						}
						break; 
					case 38: // Paralyze
						if (!CheckResist(currchar[s], i, 7))
							tempeffect(currchar[s], i, 1, 0, 0, 0);
						break; 
					case 41: // Dispel
						if (chars[i].priv2&0x20)
						{
							staticeffect( i, 0x37, 0x2A, 0x09, 0x06 );		// why the specifics here?
							if (chars[i].npc) Npcs->DeleteChar(i);
							else deathstuff(i);
						}
						break; 
					case 42: // Energy Bolt
						if (CheckResist(currchar[s], i, 6)) 
							MagicDamage(i, chars[currchar[s]].skill[MAGERY]/120, src );
						else 
							MagicDamage(i, chars[currchar[s]].skill[MAGERY]/35+RandomNum(1,10), src );
						break; 
					case 43: // Explosion
						if (CheckResist(currchar[s], i, 6))
							tempeffect( src, i, 26, (chars[currchar[s]].skill[MAGERY]/120 + RandomNum( 1, 5)), 0, 0 );
//							MagicDamage(i, chars[currchar[s]].skill[MAGERY]/120+RandomNum(1,5), src);
						else 
							tempeffect( src, i, 26, (chars[currchar[s]].skill[MAGERY]/40 + RandomNum(1, 10)), 0, 0 );
//							MagicDamage(i, chars[currchar[s]].skill[MAGERY]/40+RandomNum(1,10), src);
						break; 
					case 44: // Invisibility
						chars[i].hidden=2;
						updatechar(i);
						chars[i].invistimeout = (unsigned int) (uiCurrentTime + (server_data.invisibiliytimer * CLOCKS_PER_SEC));
						break; 
					case 51: // Flamestrike
						if (CheckResist(currchar[s], i, 7))
							MagicDamage(i, chars[currchar[s]].skill[MAGERY]/80, src );
						else 
							MagicDamage(i, chars[currchar[s]].skill[MAGERY]/40+RandomNum(1,25), src );
						break; 
					case 53: // Mana Vampire
						if(!CheckResist(currchar[s], i, 7))
						{
							if (chars[i].mn<40)
							{
								chars[currchar[s]].mn += chars[i].mn;
								chars[i].mn=0;
							}
							else
							{
								chars[i].mn-=40;
								chars[currchar[s]].mn+=40;
							}
							updatestats(i, 1);
							updatestats(currchar[s], 1);
						}
						break; 
					case 59: // Resurrection
						if (chars[i].dead==1)
						{
							Targ->NpcResurrectTarget(i);
							return;
						}
						else ; //sysmessage(s,"That player isn't dead!");
						break; 
					case 66: // Cannon Firing
						if (CheckParry(i, 6))
							MagicDamage(i, chars[currchar[s]].skill[TACTICS]/50, src );
						else
							MagicDamage(i, chars[currchar[s]].skill[TACTICS]/25, src );
						break; 
					default:
						printf("MAGIC-ERROR: Unknown CharacterTarget spell %i, magic.cpp\n", curSpell );
						break;
					}
				}
				else
				{
					sysmessage(s,"There seems to be something in the way.");
				}
			}
			else
				sysmessage(s,"That is not a person.");
		}
		else if( reqLocTarget( curSpell ) )
		{
			x=fx[0]=(buffer[s][11]<<8)+buffer[s][12];
			y=fy[0]=(buffer[s][13]<<8)+buffer[s][14];
			z=buffer[s][16] + Map->TileHeight((buffer[s][17]<<8)+buffer[s][18]);
			if( calcserial( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] ) != 0 )
			{	// we targetted something not a location!
				sysmessage( s, "You must target a location, not an item or character" );
				return;
			}
			// AntiChrist - location check (it shouldn't get here anyway)
			if(!( buffer[s][11] == 0xFF && buffer[s][12] == 0xFF && buffer[s][13] == 0xFF && buffer[s][14] == 0xFF ))
			{
				
				if ((line_of_sight( s, chars[currchar[s]].x, chars[currchar[s]].y, chars[currchar[s]].z, x, y, z,WALLS_CHIMNEYS+DOORS+FLOORS_FLAT_ROOFING)||
					(chars[currchar[s]].priv&0x01)))
				{
					if( fieldSpell( curSpell ) )
					{
						j=fielddir(currchar[s], x, y, z);
						if (j)
						{
							fx[0]=fx[1]=fx[2]=fx[3]=fx[4]=x;
							fy[0]=y-1;
							fy[1]=y+1;
							fy[2]=y-2;
							fy[3]=y+2;
							fy[4]=y;
						}  
						else 
						{	
							fy[0]=fy[1]=fy[2]=fy[3]=fy[4]=y;
							fx[0]=x-1;	
							fx[1]=x+1;
							fx[2]=x-2;
							fx[3]=x+2;
							fx[4]=x;
						}	// end else
					}	// end if
					
					playSound( src, curSpell );
					doStaticEffect( src, curSpell );
					switch( curSpell )
					{
					case 22:// Teleport
						int m;
						m = findmulti( x, y, z );
						if( m != -1 )
						{
							sysmessage( s, "You can't teleport here!" );
							return;
						}
						tile_st tile;
						Map->SeekTile(((buffer[s][0x11]<<8)+buffer[s][0x12]), &tile);
						if( (!strcmp((char *)tile.name, "water")) || (tile.flag1&0x80) )
						{
							sysmessage(s,"Give up wannabe Jesus !");
							return;
						}						
						if( (tile.flag4&0x10) == 0x10 || (tile.flag1&0x40) == 0x40 ) // slanted roof tile!!! naughty naught
						{
							sysmessage( s, "You cannot teleport there" );
							return;
						}
						mapRegions->RemoveItem(currchar[s]+1000000);
						
						chars[currchar[s]].x=x;
						chars[currchar[s]].y=y;
						chars[currchar[s]].dispz=chars[currchar[s]].z=z;
						
						mapRegions->AddItem(currchar[s]+1000000);
						teleport( currchar[s] );
						doStaticEffect( src, curSpell );
						break;
					case 24:// Wall of Stone
						id1 = 0x00;
						id2 = 0x80;
						break;
						
					case 25://Arch Cure
						x1 = x2 = y1 = y2 = z1 = z2 = 0;
						BoxSpell( s, x1, x2, y1, y2, z1, z2 );
						StartGrid = mapRegions->StartGrid( chars[currchar[s]].x, chars[currchar[s]].y );
						getcell = mapRegions->GetCell( chars[currchar[s]].x, chars[currchar[s]].y );
						
						increment = 0;
						for( checkgrid = StartGrid + ( increment*mapRegions->GetColSize()); increment < 3; increment++, checkgrid = StartGrid + ( increment*mapRegions->GetColSize()) )
						{
							for( a = 0; a < 3; a++ )
							{
								mapitemptr = -1;
								mapitem = -1;
								mapchar = -1;
								do
								{
									mapchar = -1;
									mapitemptr = mapRegions->GetNextItem( checkgrid+a, mapitemptr );
									if( mapitemptr == -1 ) break;
									mapitem = mapRegions->GetItem( checkgrid + a, mapitemptr );
									if( mapitem > 999999 ) mapchar = mapitem - 1000000;
									if( mapitem != -1 && mapitem >= 1000000 )
									{
										ii = mapchar;
										if (( online( ii ) || ( chars[ii].npc )) && ( chars[ii].x >= x1 && chars[ii].x <= x2 ) &&
											( chars[ii].y >= y1 && chars[ii].y <= y2 ) &&
											( chars[ii].z >= z1 && chars[ii].z <= z2 ) && !(chars[ii].priv&0x04 ) )
										{
											if(( line_of_sight( s, chars[currchar[s]].x, chars[currchar[s]].y, chars[currchar[s]].z, chars[ii].x, chars[ii].y, chars[ii].z, WALLS_CHIMNEYS+DOORS+FLOORS_FLAT_ROOFING)||
												( chars[currchar[s]].priv&0x01 )))
											{
												staticeffect( ii, 0x37, 0x6A, 0x09, 0x06 );
												soundeffect2( ii, 0x01, 0xE9 );
												chars[ii].poisoned = 0;
											}
											else
											{
												sysmessage( s, "There seems to be something in the way between you and %s.", chars[ii].name );
											}
										}
									} // if mapitem
								} while( mapitem != -1 );
							} // for a < 3
						} // for checkgrid
						break;
					case 26://Arch Protection
						x1 = x2 = y1 = y2 = z1 = z2 = 0;
						BoxSpell( s, x1, x2, y1, y2, z1, z2 );
						StartGrid = mapRegions->StartGrid( chars[currchar[s]].x, chars[currchar[s]].y );
						getcell = mapRegions->GetCell( chars[currchar[s]].x, chars[currchar[s]].y );
						
						increment = 0;
						for( checkgrid = StartGrid + ( increment*mapRegions->GetColSize()); increment < 3; increment++, checkgrid = StartGrid + ( increment*mapRegions->GetColSize()) )
						{
							for( a = 0; a < 3; a++ )
							{
								mapitemptr = -1;
								mapitem = -1;
								mapchar = -1;
								do
								{
									mapchar = -1;
									mapitemptr = mapRegions->GetNextItem( checkgrid+a, mapitemptr );
									if( mapitemptr == -1 ) break;
									mapitem = mapRegions->GetItem( checkgrid + a, mapitemptr );
									if( mapitem > 999999 ) mapchar = mapitem - 1000000;
									if( mapitem != -1 && mapitem >= 1000000 )
									{
										ii = mapchar;
										if (( online( ii ) || ( chars[ii].npc )) && ( chars[ii].x >= x1 && chars[ii].x <= x2 ) &&
											( chars[ii].y >= y1 && chars[ii].y <= y2 ) &&
											( chars[ii].z >= z1 && chars[ii].z <= z2 ))
										{
											if(( line_of_sight( s, chars[currchar[s]].x, chars[currchar[s]].y, chars[currchar[s]].z, chars[ii].x, chars[ii].y, chars[ii].z, WALLS_CHIMNEYS+DOORS+FLOORS_FLAT_ROOFING)||
												( chars[currchar[s]].priv&0x01 )))
											{
												playSound( ii, curSpell );
												doStaticEffect( ii, 15 );	// protection
												tempeffect( src, ii, 21, chars[currchar[s]].skill[MAGERY]/10, 0, 0 );
											}
											else
											{
												sysmessage( s, "There seems to be something in the way between you and %s.", chars[ii].name );
											}
										}
									} // if mapitem
								} while( mapitem != -1 );
							} // for a < 3
						} // for checkgrid
						break;
					case 28:// Fire Field
						if (j)
						{
							id1=0x39;
							id2=0x96;
						}		// end if
						else 
						{	
							id1=0x39;
							id2=0x8C;
						}		// end else
						break;
					case 33:// Blade Spirits
						SummonMonster( s, 0x02, 0x3E, "a blade spirit", 0x00, 0x00, x, y, z );
						break;
					case 34:// Dispel Field
						serial = calcserial( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
						i = findbyserial( &itemsp[serial%HASHMAX], serial, 0 );
						if( i != -1 )
						{
							if(( line_of_sight( calcSocketFromChar( s ), chars[currchar[s]].x, chars[currchar[s]].y, chars[currchar[s]].z, items[i].x, items[i].y, items[i].z, WALLS_CHIMNEYS+DOORS+FLOORS_FLAT_ROOFING )||
								(chars[currchar[s]].priv&0x01 )))
							{
								if( items[i].priv&5 || items[i].priv&4 ) Items->DeleItem( i );
								soundeffect2( i, 0x02, 0x01 );
							}
							else
							{
								sysmessage( s, "There seems to be something in the way." );
							}
						}
						break;
					case 39:// Poison Field
						if (j)
						{
							id1=0x39;
							id2=0x20;
						} 
						else 
						{
							id1=0x39;
							id2=0x15;
						}
						break;
					case 46:// Mass curse
						x1 = x2 = y1 = y2 = z1 = z2 = 0;
						BoxSpell( s, x1, x2, y1, y2, z1, z2 );
						StartGrid = mapRegions->StartGrid( chars[currchar[s]].x, chars[currchar[s]].y );
						getcell = mapRegions->GetCell( chars[currchar[s]].x, chars[currchar[s]].y );
						
						increment = 0;
						for( checkgrid = StartGrid + ( increment*mapRegions->GetColSize()); increment < 3; increment++, checkgrid = StartGrid + ( increment*mapRegions->GetColSize()) )
						{
							for( a = 0; a < 3; a++ )
							{
								mapitemptr = -1;
								mapitem = -1;
								mapchar = -1;
								do
								{
									mapchar = -1;
									mapitemptr = mapRegions->GetNextItem( checkgrid+a, mapitemptr );
									if( mapitemptr == -1 ) break;
									mapitem = mapRegions->GetItem( checkgrid + a, mapitemptr );
									if( mapitem > 999999 ) mapchar = mapitem - 1000000;
									if( mapitem != -1 && mapitem >= 1000000 )
									{
										ii = mapchar;
										if (( online( ii ) || ( chars[ii].npc )) && ( chars[ii].x >= x1 && chars[ii].x <= x2 ) &&
											( chars[ii].y >= y1 && chars[ii].y <= y2 ) &&
											( chars[ii].z >= z1 && chars[ii].z <= z2 ))
										{
											if(( line_of_sight( s, chars[currchar[s]].x, chars[currchar[s]].y, chars[currchar[s]].z, chars[ii].x, chars[ii].y, chars[ii].z, WALLS_CHIMNEYS+DOORS+FLOORS_FLAT_ROOFING)||
												( chars[currchar[s]].priv&0x01 )))
											{
												if( chars[ii].npc ) npcattacktarget( ii, currchar[s] );
												staticeffect( ii, 0x37, 0x4A, 0, 15 );
												soundeffect2( ii, 0x01, 0xFC );
												if( CheckResist( currchar[s], ii, 6 )) j = chars[currchar[s]].skill[MAGERY]/200;
												else j = chars[currchar[s]].skill[MAGERY]/75;
												tempeffect( currchar[s], ii, 12, j, j, j );
											}
											else
											{
												sprintf( temp, "Try as you might, but you cannot see %s well enough to cover.", chars[ii].name );
												sysmessage( s, temp );
											}
										}
									} // if mapitem
								} while( mapitem != -1 );
							} // for
						} // for checkgrid
						break;
					case 47:// Paralyze Field
						if (j)
						{
							id1=0x39;
							id2=0x79;
						} 
						else 
						{
							id1=0x39;
							id2=0x67;
						}
						break;
					case 48: 
						if ((line_of_sight(calcSocketFromChar(s),chars[currchar[s]].x,chars[currchar[s]].y,chars[currchar[s]].z, x, y, z,WALLS_CHIMNEYS+DOORS+FLOORS_FLAT_ROOFING)||
							(chars[currchar[s]].priv&0x01)))
						{ 
							j=chars[currchar[s]].skill[MAGERY]; 
							range=(((j-261)*(15))/739)+5;
							//If the caster has a Magery of 26.1 (min to cast reveal w/ scroll), range  radius is
							//5 tiles, if magery is maxed out at 100.0 (except for gms I suppose), range is 20
							
							//Char mapRegions
							StartGrid=mapRegions->StartGrid(chars[currchar[s]].x,chars[currchar[s]].y);
							getcell=mapRegions->GetCell(chars[currchar[s]].x,chars[currchar[s]].y);
							
							increment=0;
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
											ii=mapchar;
											if ((chars[ii].hidden==2) || (chars[ii].hidden==1)) //  does reveal hidden people as well now :)
											{
												dx=abs(chars[ii].x-x);
												dy=abs(chars[ii].y-y);
												dz=abs(chars[ii].z-z);  // new--difference in z coords
#ifdef __NT__
												d=sqrt(dx*dx+dy*dy);
#else
												d=hypot(dx, dy);
#endif
												if ((d<=range)&&(dz<=15)) //char to reveal is within radius or range and no more than 15 z coords away
												{
													if( chars[ii].hidden && !(chars[ii].priv2&8 ) )
													{
														chars[ii].hidden=0;
														chars[ii].stealth=-1;
														updatechar(ii);
													}
												}
											}
										}
									} while (mapitem!=-1);
								}//for a<3
							}//for checkgrid
							soundeffect(s,0x01,0xFD);
						}
						else
						{
							sysmessage(s, "You would like to see if anything was there, but there is too much stuff in the way.");
						}
						break;
					case 49: ///////// Chain Lightning ///////////
						///////// Forgotten, added by LB ////
						
						x1 = x2 = y1 = y2 = z1 = z2 = 0;						
						BoxSpell(s, x1, x2, y1, y2, z1, z2);
						
						//Char mapRegions
						StartGrid=mapRegions->StartGrid(chars[currchar[s]].x,chars[currchar[s]].y);
						getcell=mapRegions->GetCell(chars[currchar[s]].x,chars[currchar[s]].y);
						
						increment=0;
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
										ii=mapchar;				
										if ((online(ii)||(chars[ii].npc)) && (chars[ii].x>=x1&&chars[ii].x<=x2)&&
											(chars[ii].y>=y1&&chars[ii].y<=y2)&&
											(chars[ii].z>=z1&&chars[ii].z<=z2) && !(chars[ii].priv&0x04) )
										{
											if ((line_of_sight(s,chars[currchar[s]].x,chars[currchar[s]].y,chars[currchar[s]].z,chars[ii].x,chars[ii].y,chars[ii].z,WALLS_CHIMNEYS+DOORS+FLOORS_FLAT_ROOFING)||
												(chars[currchar[s]].priv&0x01)))
											{
												if (chars[ii].npc) npcattacktarget(currchar[s],ii);
												bolteffect(ii);
												soundeffect2(currchar[s], 0x00, 0x29); //Homey fix for chainlightning sound
												int def, att;
												if( CheckMagicReflect( ii ) )
												{
													def = currchar[s];
													att = ii;
												}
												else
												{
													def = ii;
													att = currchar[s];
												}
												if (CheckResist(att, def, 7))
												{
													MagicDamage(def, chars[att].skill[MAGERY]/70, att );
												}
												else 
												{
													MagicDamage(def, chars[att].skill[MAGERY]/50, att );
												}
											}
											else
											{
												sysmessage(s, "You cannot see the target well.");
											}
										}
									}//if Mapitem
								} while (mapitem!=-1);
							}//for a<3
						}//for checkgrid
						
						
						break;
						
					case 50:// Energy Field
						if (j)
						{
							id1=0x39;
							id2=0x56;
						} 
						else 
						{
							id1=0x39;
							id2=0x46;
						}
						break;
					case 54://Mass Dispel
						x1 = x2 = y1 = y2 = z1 = z2 = 0;
						
						BoxSpell(s, x1, x2, y1, y2, z1, z2);
						
						//Char mapRegions
						StartGrid=mapRegions->StartGrid(chars[currchar[s]].x,chars[currchar[s]].y);
						getcell=mapRegions->GetCell(chars[currchar[s]].x,chars[currchar[s]].y);
						
						increment=0;
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
										ii=mapchar;				 
										if ((online(ii)||(chars[ii].npc)) && (chars[ii].priv2&0x20)&&
											(chars[ii].x>=x1&&chars[ii].x<=x2)&&
											(chars[ii].y>=y1&&chars[ii].y<=y2)&&
											(chars[ii].z>=z1&&chars[ii].z<=z2))
										{
											if ((line_of_sight(s,chars[currchar[s]].x,chars[currchar[s]].y,chars[currchar[s]].z,chars[ii].x,chars[ii].y,chars[ii].z,WALLS_CHIMNEYS+DOORS+FLOORS_FLAT_ROOFING)||
												(chars[currchar[s]].priv&0x01)))
											{ 
												if(CheckResist(currchar[s], ii, 7) && rand()%2==0 )  // cant be 100% resisted , LB, osi
												{
													if (chars[ii].npc) npcattacktarget(ii, currchar[s]);
													return;
												}
												if (chars[ii].npc) 
													deathstuff(ii); // LB !!!!
												soundeffect2(ii, 0x02, 0x04);
												staticeffect(ii, 0x37, 0x2A, 0x09, 0x06);
											}
										}
									}//if mapitem
								} while(mapitem!=-1);
							}//for a<3
						}//for checkgrid
						break;
					case 55://Meteor Swarm
						x1 = x2 = y1 = y2 = z1 = z2 = 0;
						
						BoxSpell(s, x1, x2, y1, y2, z1, z2);
						
						//Char mapRegions
						StartGrid=mapRegions->StartGrid(chars[currchar[s]].x,chars[currchar[s]].y);
						getcell=mapRegions->GetCell(chars[currchar[s]].x,chars[currchar[s]].y);
						
						increment=0;
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
										ii=mapchar;				
										if ((online(ii)||(chars[ii].npc)) && (chars[ii].x>=x1&&chars[ii].x<=x2)&&
											(chars[ii].y>=y1&&chars[ii].y<=y2)&&
											(chars[ii].z>=z1&&chars[ii].z<=z2) && !(chars[ii].priv&0x04) )
										{
											if ((line_of_sight(s,chars[currchar[s]].x,chars[currchar[s]].y,chars[currchar[s]].z,chars[ii].x,chars[ii].y,chars[ii].z,WALLS_CHIMNEYS+DOORS+FLOORS_FLAT_ROOFING)||
												(chars[currchar[s]].priv&0x01)))
											{
												if (chars[ii].npc) npcattacktarget(currchar[s],ii);
												soundeffect2(ii, 0x1, 0x60); //sound fix for swarm--Homey
												movingeffect(currchar[s], ii, 0x36, 0xD5, 0x07, 0x00, 0x01);
												if (CheckResist(currchar[s], ii, 7))
												{
													MagicDamage(ii, chars[currchar[s]].skill[MAGERY]/80, currchar[s] );
												}
												else 
												{
													MagicDamage(ii, chars[currchar[s]].skill[MAGERY]/40, currchar[s] );
												}
											}
											else
											{
												sysmessage(s, "You cannot see the target well.");
											}
										}
									}//if Mapitem
								} while (mapitem!=-1);
							}//for a<3
						}//for checkgrid
						break;
					case 58:// Energy Vortex
						SummonMonster( s, 0x00, 0x0d, "an energy vortex", 0x00, 0x75, x, y, z );
						break;
					default:
						printf("MAGIC-ERROR: Unknown LocationTarget spell %i\n", curSpell );
						break;
					}
					
					if( fieldSpell( curSpell ) )
					{
						for( j=0; j<5; j++ )		// how about we make this 5, huh?  missing part of the field
						{
							i=Items->SpawnItem(s,1,"#",0,id1,id2,0,0,0,0);
							if( i > -1 )
							{
								items[i].priv=items[i].priv|0x05;
								items[i].priv=items[i].priv|0x01;
								items[i].decaytime = (unsigned int)( uiCurrentTime+((chars[currchar[s]].skill[MAGERY]/15)*CLOCKS_PER_SEC) );
								items[i].morex=chars[currchar[s]].skill[MAGERY]; // remember casters magery skill for damage, LB
								mapRegions->RemoveItem( i );
								items[i].x=fx[j];
								items[i].y=fy[j];
								items[i].z=Map->Height( fx[j], fy[j], z );
								mapRegions->AddItem(i); // lord Binary
								items[i].dir=29;
								items[i].magic = 2;
								RefreshItem( i ); // AntiChrist
							}
						}
					}
				} // if los
			} else 
			{
				sysmessage( s, "That is not a valid location." );
			}
		}
		else if( reqItemTarget( curSpell ) )
		{
			
			// ITEM TARGET
			item=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);	// item we are targeting
			i=findbyserial(&itemsp[item%HASHMAX], item, 0);	// modified by Abaddon, 17th February, 2000, was doing a char lookup
			if( i != -1 )
			{
				if( itemdist( currchar[s], i ) > combat.maxRangeSpell )
				{
					sysmessage( s, "You can't cast on an item that far away!" );
					return;
				}
				if ((line_of_sight( s, chars[currchar[s]].x, chars[currchar[s]].y, chars[currchar[s]].z, items[i].x, items[i].y, items[i].z, WALLS_CHIMNEYS+DOORS+FLOORS_FLAT_ROOFING)||
					(chars[currchar[s]].priv&0x01)))
				{
					
					switch( curSpell )
					{
						
					case 13://Magic Trap
						if((items[i].type==1 || items[i].type==63 ||
							items[i].type==8 || items[i].type==64)
							&&( items[i].id1!=0x0E || items[i].id2!=0x75))  
						{
							items[i].moreb1=1;
							items[i].moreb2=chars[currchar[s]].skill[MAGERY]/20;
							items[i].moreb3=chars[currchar[s]].skill[MAGERY]/10;
							soundeffect3(i, 0x01, 0xF0);
						} else sysmessage(currchar[s],"You cannot trap this!!!");
						break;
					case 14://Magic Untrap
						if((items[i].type==1 || items[i].type==63 ||
							items[i].type==8 || items[i].type==64))
						{
							if(items[i].moreb1==1)
							{
								if(rand()%100<=50+(chars[currchar[s]].skill[MAGERY]/10)-items[i].moreb3)
								{
									items[i].moreb1=0;
									items[i].moreb2=0;
									items[i].moreb3=0;
									soundeffect3(i, 0x01, 0xF1);
									sysmessage(s,"You successful untrap this item!");
								}
								else sysmessage(s,"You miss to untrap this item!");
							}
							else sysmessage(s,"This items doesn't seem to be trapped!");
						}
						else sysmessage(s,"This items cannot be trapped!");
						break;
					case 19:
						if((items[i].type==1 || items[i].type==12 || items[i].type==63)
							&&( items[i].id1!=0x0E || items[i].id2!=0x75))  
						{
							switch(items[i].type)
							{
							case 1:  items[i].type=8; break;
							case 12: items[i].type=13; break;
							case 63: items[i].type=64; break;
							default:
								printf("ERROR: Fallout of switch statement without default. magic.cpp, magiclocktarget()/n"); //Morrolan
								break;
							}
							soundeffect3(i, 0x02, 0x00);
						}
						else sysmessage(currchar[s],"You cannot lock this!!!"); 
						break;
					case 23:
						if( items[i].type==8)
						{
							items[i].type=1;
							sysmessage( s, "You unlock the item." );
						} else if ( items[i].type==64 )
						{
							items[i].type=63;
							sysmessage( s, "You unlock the item." );
						} else if ( items[i].type==1 || items[i].type==63 || items[i].type == 65 || items[i].type == 87 )
							sysmessage( s, "That is not locked." );
						else if( items[i].type == 12 )
							sysmessage( s, "That may only be locked with the key.");
						else sysmessage( s, "That does not have a lock.");
						
						break;
						//						case 41:
						//							if (items[i].priv&0x04) Items->DeleItem(i);	break;
					default:
						printf("MAGIC-ERROR: Unknown ItemTarget spell %i, magic.cpp\n", curSpell );
						break;
					}
				}
				else
					sysmessage( s, "There is something in the way!" );
			}
			else
				sysmessage( s, "That is not a valid item" );
		}
		else
			sysmessage( s, "Can't cope with this spell, requires a target but it doesn't specify what type" );
		chars[currchar[s]].spellCast = 0;
		chars[currchar[s]].casting = 0;
		return;
	}
	else
	{
		// non targetted spells
		playSound( src, curSpell );
		doStaticEffect( src, curSpell );
		switch( curSpell )
		{
		case 2: // Create Food
			j=Items->SpawnItem( s, 1, "#", 1, 0x09, 0xD3, 0x00, 0x00, 1, 1 );
			if( j > -1 )
			{
				items[j].type=14;
				RefreshItem( j );
			}
			
			break; // LB crashfix
		case 40: // Summon Monster	// summon monster is 40, not 33
			SummonMonster( s, 0, 0, "#", 0, 0, chars[currchar[s]].x+1, chars[currchar[s]].y+1, chars[currchar[s]].z );
			break;
		case 35:
			printf("INCOGNITO SPELL START!!\n");
			int serhash,ci;
			i=currchar[s];
			
			// ------ SEX ------
			chars[i].orgid2=chars[i].id2;
			if((rand()%2)==0) chars[i].id2=0x90; else chars[i].id2=0x91;
			
			// ------ NAME -----
			strcpy(chars[i].orgname,chars[i].name);
			
			//wait a moment: if player already has a beard
			//he can't be morphed into a women!
			
			if(chars[i].id2==0x90) setrandomname(i,"1");//get a name from male list
			else setrandomname(i,"2");//get a name from female list
			
			serial=chars[i].serial;
			serhash=serial%HASHMAX;
			for (ci=0;ci<contsp[serhash].max;ci++)
			{
				j=contsp[serhash].pointer[ci];
				if( j != -1 ) 
				{
					//
					//damn..this formula seems to include also some bad color...
					//AntiChrist
					//
					int color = 0x044E + (rand()%( 0x04AD - 0x044E ) );
					
					// ------ HAIR -----
					if(items[j].layer==0x0B) { //change hair style/color
						printf("HAIR FOUND!!\n");
						//stores old hair values
						chars[i].haircolor1=items[j].color1;
						chars[i].haircolor2=items[j].color2;
						chars[i].hairstyle1=items[j].id1;
						chars[i].hairstyle2=items[j].id2;
						//changes them with random ones
						switch(rand()%10)
						{
						case 0: items[j].id2='\x3B'; break;
						case 1: items[j].id2='\x3C'; break;
						case 2:	items[j].id2='\x3D'; break;
						case 3:	items[j].id2='\x44'; break;
						case 4:	items[j].id2='\x45'; break;
						case 5:	items[j].id2='\x46'; break;
						case 6:	items[j].id2='\x47'; break;
						case 7:	items[j].id2='\x48'; break;
						case 8:	items[j].id2='\x49'; break;
						case 9:	items[j].id2='\x4A'; break;
						default: items[j].id2='\x4A'; break;
						}
						
						items[j].color1 = (unsigned char)(color>>8);
						items[j].color2 = (unsigned char)(color%256);
						
						if ((((items[j].color1<<8)+items[j].color2)<0x044E) ||
							(((items[j].color1<<8)+items[j].color2)>0x04AD) )
						{
							items[j].color1=0x04;
							items[j].color2=0x4E;
						}
						//wornitems(s, currchar[s]);
						//teleport(currchar[s]);
						//impowncreate(s, currchar[s], 0);
					}
					
					// -------- BEARD --------
					// only if a men :D
					if(chars[i].id2==0x90)
						if(items[j].layer==0x10) { //change beard style/color
							printf("BEARD FOUND!!\n");
							//stores old beard values
							chars[i].beardcolor1=items[j].color1;
							chars[i].beardcolor2=items[j].color2;
							chars[i].beardstyle1=items[j].id1;
							chars[i].beardstyle2=items[j].id2;
							//changes them with random ones
							switch(rand()%7)
							{
							case 0: items[j].id2='\x3E'; break;
							case 1: items[j].id2='\x3F'; break;
							case 2:	items[j].id2='\x40'; break;
							case 3:	items[j].id2='\x41'; break;
							case 4:	items[j].id2='\x4B'; break;
							case 5:	items[j].id2='\x4C'; break;
							case 6:	items[j].id2='\x4D'; break;
							default: items[j].id2='\x4D'; break;
							}
							
							items[j].color1=color>>8;
							items[j].color2=color%256;
							
							if ((((items[j].color1<<8)+items[j].color2)<0x044E) ||
								(((items[j].color1<<8)+items[j].color2)>0x04AD) )
							{
								items[j].color1=0x04;
								items[j].color2=0x4E;
							}
							//wornitems(s, currchar[s]);
							//teleport(currchar[s]);
							//impowncreate(s, currchar[s], 0);
						}
				}
			}
			//only refresh once
			wornitems(s, currchar[s]);
			teleport(currchar[s]);
			impowncreate(s, currchar[s], 0);
			
			soundeffect2(currchar[s], 0x02, 0x03);
			tempeffect(currchar[s],currchar[s],19,0,0,0);
			
			break;
			
			case 36:// Magic Reflection
				chars[currchar[s]].priv2=chars[currchar[s]].priv2|0x40;
				break;
			case 56:
				PolymorphMenu( s, POLYMORPHMENUOFFSET ); // Antichrists Polymorph
				break;
			case 57://Earthquake
				dmg=(chars[currchar[s]].skill[MAGERY]/40)+(rand()%20-10);
				dmgmod;
				//		EviLDeD -		March 1, 2000
				//		Some extra console spam
				printf("DEBUG: [NewCastSpell()] %s is being set to criminal\n", chars[currchar[s]].name );
				//		EviLDeD -		End

				criminal(currchar[s]);
				
				//Char mapRegions
				StartGrid=mapRegions->StartGrid(chars[currchar[s]].x,chars[currchar[s]].y);
				getcell=mapRegions->GetCell(chars[currchar[s]].x,chars[currchar[s]].y);
				
				increment=0;
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
								ii=mapchar;
								distx=abs(chars[ii].x-chars[currchar[s]].x);
								disty=abs(chars[ii].y-chars[currchar[s]].y);
								if( distx <= 15 && disty <= 15 && !(chars[ii].priv&0x04) && ( chars[ii].npc || online(ii) ) )
								{
									// dmgmod=unmounthorse(i);
									
									dmgmod = min(distx,disty);
									dmgmod = -(dmgmod - 7);
									chars[ii].hp -=  dmg+dmgmod;
									chars[ii].stm -= rand()%10+5;
									
									if(chars[ii].stm<0)  chars[ii].stm=0;
									if(chars[ii].hp<0) chars[ii].hp=0;						 							
									
									if (!chars[ii].npc && online(ii))
									{
										if(rand()%2) npcaction(ii, 0x15); else npcaction(ii, 0x16);
										if(chars[ii].hp==0) 
										{
											
											deathstuff(ii);                              
											if(chars[ii].flag&0x04 && Guilds->Compare( currchar[s], ii )==0 && Races->Compare( currchar[s], ii ) == 0)
											{
												chars[currchar[s]].kills++;
												sprintf(temp, "You have killed %i innocent people.", chars[currchar[s]].kills);
												sysmessage(s,temp);
												if (chars[currchar[s]].kills==repsys.maxkills+1)
													sysmessage(s,"You are now a murderer!");
											}
											
											sprintf(temp,"%s was killed by %s! [Earthquake]\n",chars[ii].name,chars[currchar[s]].name);
											savelog(temp,"PvP.log");
										}
									} else { 
										if (chars[ii].hp<=0) deathstuff(ii); 
										else { 
											if (chars[ii].npc) { 
												npcaction(ii, 0x2); 
												npcattacktarget(currchar[s],ii); 
											}
										}
									} 
								} //if Distance
							}//if mapitem
						} while(mapitem!=-1);
					}
				}
				break;
			case 60:// Summon Air Elemental
				SummonMonster( s, 0x00, 0x0d, "Air", 0, 0, chars[currchar[s]].x+1, chars[currchar[s]].y+1, chars[currchar[s]].z );
				break;
			case 61:// Summon Daemon
				SummonMonster( s, 0x00, 0x0A, "Bob", 0, 0, chars[currchar[s]].x+1, chars[currchar[s]].y+1, chars[currchar[s]].z );
				break;
			case 62:// Summon Earth Elemental
				SummonMonster( s, 0x00, 0x0E, "Earth", 0, 0, chars[currchar[s]].x+1, chars[currchar[s]].y+1, chars[currchar[s]].z );
				break;
			case 63:// Summon Fire Elemental
				SummonMonster( s, 0x00, 0x0F, "Fire", 0, 0, chars[currchar[s]].x+1, chars[currchar[s]].y+1, chars[currchar[s]].z );
				break;
			case 64:// Summon Water Elemental
				SummonMonster( s, 0x00, 0x10, "Water", 0, 0, chars[currchar[s]].x+1, chars[currchar[s]].y+1, chars[currchar[s]].z );
				break;
			case 65:// Summon Hero
				SummonMonster( s, 0x03, 0xE2, "Dupre the Hero", 0, 0, chars[currchar[s]].x+1, chars[currchar[s]].y+1, chars[currchar[s]].z );
				break;
			case 67:
				SummonMonster( s, 0x00, 0x0A, "Black Night", (unsigned char)(5000>>8), (unsigned char)(5000%256), chars[currchar[s]].x+1, chars[currchar[s]].y+1, chars[currchar[s]].z );
				break;
			default:	
				printf("MAGIC-ERROR: Unknown NonTarget spell %i, magic.cpp\n", curSpell );
				break;
		}
		chars[currchar[s]].spellCast = 0;
		chars[currchar[s]].casting = 0;
		return;
	}
	
}


//:Terrin: Avoid multiple reading of the spell script every time a spell is
//         cast to avoid crippling the server when a mage enters combat
void cMagic::LoadScript( void )
{
	if( spells )
	{
		delete[] spells;
	}	
	printf("\nLoading spells"); fflush(stdout);
	
	   
    // for some strange reason, spells go from index 1 to SPELL_MAX and 
    // apparently index 0 is left unused - fur 
	spells = new splInfo_st[SPELL_MAX + 1];
	memset( spells, 0, sizeof( splInfo_st ) * ( SPELL_MAX + 1 ) );
	
	int i=0;
	unsigned int spellCount = 1;
	int enabled;
	bool done = false;
	int tempValReflect = 0;
	int moveFX[5];
	int sFX[2];
	int stFX[4];
	unsigned char quickCounter = 0;
	openscript("spells.scp");
	char sect[512];
	while ( !done )
	{
		sprintf(sect, "SPELL %d", spellCount);
		if (!i_scripts[spells_script]->find(sect))
		{
			done = true;
		}
		else
			spellCount++;
	}
	
	--spellCount; // spell count will be one too high after that loop - fur 
    
	if (spellCount > SPELL_MAX) 
	{ 
		printf("ERROR: Too many spells (%d) in spells.scp, ones after %d will be ignored.\n", spellCount, SPELL_MAX); 
		spellCount = SPELL_MAX; 
	} 
	
	for( i=1; i <= spellCount; i++ )
	{
		sprintf(sect, "SPELL %d", i);
		printf("."); fflush(stdout);
		i_scripts[spells_script]->find(sect);
		spells[i].enabled = false;
		spells[i].soundEffect[0] = spells[i].soundEffect[1] = -1;
		spells[i].staticEffect[0] = spells[i].staticEffect[1] = spells[i].staticEffect[2] = spells[i].staticEffect[3] = -1;
		spells[i].moveEffect[0] = spells[i].moveEffect[1] = spells[i].moveEffect[2] = spells[i].moveEffect[3] = spells[i].moveEffect[4] = -1;
		spells[i].health = spells[i].stamina = spells[i].mana = 0;
		moveFX[0] = moveFX[1] = moveFX[2] = moveFX[3] = moveFX[4] = -1;
		sFX[0] = sFX[1] = -1;
		stFX[0] = stFX[1] = stFX[2] = stFX[3] = -1;
		do 
		{
			read2();
			if( !(strcmp("ENABLE",  script1 ) ) )   // presence of enable is enough to enable it
			{ 
				enabled = atoi( script2 ); 
				if( enabled != 0 ) 
					spells[i].enabled = true;
				else 
					spells[i].enabled = false;
			}
			else if( !strcmp("CIRCLE",  script1 ) ) spells[i].circle        = atoi( script2 );
			else if( !strcmp("MANA",    script1 ) ) spells[i].mana          = atoi( script2 );
			else if( !strcmp("STAMINA", script1 ) ) spells[i].stamina		= atoi( script2 );
			else if( !strcmp("HEALTH",  script1 ) ) spells[i].health		= atoi( script2 );
			else if( !strcmp("LOSKILL", script1 ) ) spells[i].loskill       = atoi( script2 );
			else if( !strcmp("HISKILL", script1 ) ) spells[i].hiskill       = atoi( script2 );
			else if( !strcmp("SCLO",    script1 ) ) spells[i].sclo          = atoi( script2 );
			else if( !strcmp("SCHI",    script1 ) ) spells[i].schi          = atoi( script2 );
			else if( !strcmp("MANTRA",  script1 ) ) strcpy( spells[i].mantra, script2 );
			else if( !strcmp("ACTION",  script1 ) ) spells[i].action        = atoi( script2 );
			else if( !strcmp("DELAY",   script1 ) ) spells[i].delay         = atoi( script2 );
			else if( !strcmp("ASH",     script1 ) ) spells[i].reags.ash     = atoi( script2 );
			else if( !strcmp("DRAKE",   script1 ) ) spells[i].reags.drake   = atoi( script2 );
			else if( !strcmp("GARLIC",  script1 ) ) spells[i].reags.garlic  = atoi( script2 );
			else if( !strcmp("GINSING", script1 ) ) spells[i].reags.ginseng = atoi( script2 );
			else if( !strcmp("MOSS",    script1 ) ) spells[i].reags.moss    = atoi( script2 );
			else if( !strcmp("PEARL",   script1 ) ) spells[i].reags.pearl   = atoi( script2 );
			else if( !strcmp("SHADE",   script1 ) ) spells[i].reags.shade   = atoi( script2 );
			else if( !strcmp("SILK",    script1 ) ) spells[i].reags.silk    = atoi( script2 );
			else if( !strcmp("TARG",    script1 ) ) strcpy( spells[i].strToSay, script2 );
			else if( !strcmp("REFLECT", script1 ) ) 
			{
				tempValReflect = atoi( script2 );
				spells[i].reflect = ( tempValReflect == 1 );	// if val is 1, then reflect, if 2, then no reflect
			}
			else if( !strcmp("RESISTABLE", script1 ) ) 
			{
				tempValReflect = atoi( script2 );
				spells[i].resistable = ( tempValReflect != 0 );
			}
			else if( !strcmp("AGGRESSIVE", script1 ) ) 
			{
				tempValReflect = atoi( script2 );
				spells[i].aggressive = ( tempValReflect != 0 );
			}
			else if( !strcmp("SOUNDFX", script1 ) ) 
			{
				sscanf( script2, "%x%x", &sFX[0], &sFX[1] );
				for( quickCounter = 0; quickCounter < 2; quickCounter++ )
				{
					spells[i].soundEffect[quickCounter] = (sFX[quickCounter]);
				}
			}
			else if( !strcmp("STATFX", script1 ) ) 
			{
				sscanf( script2, "%x%x%x%x", &stFX[0], &stFX[1], &stFX[2], &stFX[3] );
				for( quickCounter = 0; quickCounter < 4; quickCounter++ )
				{
					spells[i].staticEffect[quickCounter] = (stFX[quickCounter]);
				}
			}
			else if( !strcmp("MOVEFX", script1 ) ) 
			{
				sscanf( script2, "%x%x%x%x%x", &moveFX[0], &moveFX[1], &moveFX[2], &moveFX[3], &moveFX[4] );
				for( quickCounter = 0; quickCounter < 5; quickCounter++ )
				{
					spells[i].moveEffect[quickCounter] = (moveFX[quickCounter]);
				}
			}
				
		} while(script1[0]!='}' && strcmp( script1, "EOF" ) );
	}
	closescript();
	printf("\n");
}


bool cMagic::requireTarget( unsigned char num )
{
	
	switch( num )
	{
	case 1: // Clumsy
		//	case 2: // Create Food		 
	case 3: // Feeblemind
	case 4: // Heal
	case 5: // Magic Arrow
	case 6: // Night Sight
	case 7:	// Reactive Armour...
	case 8: // Weaken
	case 9: // Agility
	case 10:// Cunning
	case 11:// Cure
	case 12:// Harm
	case 13://Magic Trap
	case 14://Magic Untrap
	case 15: //Protection
	case 16: // Strength
	case 17:// Bless
	case 18:// Fireball
	case 19://Magic Lock
	case 20:// Poison
	case 22:// Teleport
	case 23:// Unlock
	case 24:// Wall of Stone
	case 25://Arch Cure
	case 26:// Arch protection
	case 27:// Curse
	case 28:// Fire Field
	case 29:// Greater Heal
	case 30:// Lightning
	case 31://Mana drain
	case 32:// Recall
	case 33:// Blade Spirits
	case 34:// Dispel Field
	case 37:// Mind Blast
	case 38:// Paralyze
	case 39:// Poison Field
	case 41:// Dispel
	case 42:// Energy Bolt
	case 43://Explosion
	case 44:// Invisibility
	case 45:// Mark
	case 46:// Mass curse
	case 47:// Paralyze Field
	case 48:// Reveal
	case 49:// Chain Lightning
	case 50:// Energy Field
	case 51:// Flamestrike
	case 52:// Gate Travel
	case 53://Mana Vampire
	case 54://Mass Dispel
	case 55://Meteor Swarm
	case 58:// Energy Vortex
	case 59:// Resurrection
	case 66://Cannon Firing
		return true;
		
		
	case 36:// Magic Reflection
	case 56: // Polymorph
	case 57://Earthquake
	case 60:// Summon Air Elemental
	case 61:// Summon Daemon
	case 62:// Summon Earth Elemental
	case 63:// Summon Fire Elemental
	case 64:// Summon Water Elemental
	case 65:// Summon Hero
	case 67:// Summon Black Night
	default: 
		return false;
	}
	return false;
}

void cMagic::NewDelReagents( CHARACTER s, reag_st reags )
{
	if (chars[s].priv2&0x80) return;
	delequan(s, 0x0F, 0x7A, reags.pearl);
	delequan(s, 0x0F, 0x7B, reags.moss);
	delequan(s, 0x0F, 0x84, reags.garlic);
	delequan(s, 0x0F, 0x85, reags.ginseng);
	delequan(s, 0x0F, 0x86, reags.drake);
	delequan(s, 0x0F, 0x88, reags.shade);
	delequan(s, 0x0F, 0x8C, reags.ash);
	delequan(s, 0x0F, 0x8D, reags.silk);
}

bool cMagic::spellReflectable( int num )
{
	// AntiChrist - customizable in spells.scp
	if( spells[num].reflect )
		return true;
	else 
		return false;
}


bool cMagic::travelSpell( int num )
{
	switch( num )
	{
	case 52:
	case 32:
	case 45:
		return true;
	default:
		return false;
	}
	return false;
}

bool cMagic::reqCharTarget( int num )
{
	switch( num )
	{
	case 1:  // Clumsy
	case 3:  // Feeblemind
	case 4:  // Heal
	case 5:  // Magic Arrow
	case 6:  // Night Sight
	case 7:	 // Reactive armour
	case 8:  // Weaken
	case 9:  // Agility
	case 10: // Cunning
	case 11: // Cure
	case 12: // Harm
	case 15: // Protection
	case 16: // Strength
	case 17: // Bless
	case 18: // Fireball
	case 20: // Poison
	case 27: // Curse
	case 29: // Greater Heal
	case 30: // Lightning
	case 31: // Mana drain
	case 37: // Mind Blast
	case 38: // Paralyze
	case 41: // Dispel
	case 42: // Energy Bolt
	case 43: // Explosion
	case 44: // Invisibility
	case 51: // Flamestrike
	case 53: // Mana Vampire
	case 56:	// Polymorph
	case 59: // Resurrection
	case 66: // Cannon Firing
		return true;
	default:
		return false;
	}
	return false;
}

bool cMagic::reqLocTarget( int num )
{
	switch( num )
	{
	case 22:// Teleport
	case 24:// Wall of Stone
	case 25://Arch Cure
	case 26:// Arch protection
	case 28:// Fire Field
	case 33:// Blade Spirits
	case 34:// Dispel Field
	case 39:// Poison Field
	case 46:// Mass curse
	case 47:// Paralyze Field
	case 48:// Reveal
	case 49: // Chainlightning
	case 50:// Energy Field
	case 54://Mass Dispel
	case 55://Meteor Swarm
	case 58:// Energy Vortex
		return true;
	default:
		return false;
	}
	return false;
}

bool cMagic::reqItemTarget( int num )
{
	switch( num )
	{
	case 13://Magic Trap
	case 14://Magic Untrap
	case 19://Magic Lock
	case 23:// Unlock
		return true;
	default:
		return false;
	}
	return false;
}

move_st cMagic::getMoveEffects( int num )
{
	move_st temp;
	temp.effect[0] = -1;
	temp.effect[1] = -1;
	temp.effect[2] = -1;
	temp.effect[3] = -1;
	temp.effect[4] = -1;
	
	for( int quickQ = 0; quickQ < 5; quickQ++ )
		temp.effect[quickQ] = spells[num].moveEffect[quickQ];
	return temp;
}

stat_st cMagic::getStatEffects( int num )
{
	stat_st temp;
	
	temp.effect[0] = -1;
	temp.effect[1] = -1;
	temp.effect[2] = -1;
	temp.effect[3] = -1;
	
	for( int quickQ = 0; quickQ < 4; quickQ++ )
		temp.effect[quickQ] = spells[num].staticEffect[quickQ];
	return temp;
}

sound_st cMagic::getSoundEffects( int num )
{
	sound_st temp;
	temp.effect[0] = -1;
	temp.effect[1] = -1;
	
	for( int quickQ = 0; quickQ < 2; quickQ++ )
		temp.effect[quickQ] = spells[num].soundEffect[quickQ];
	return temp;
}

void cMagic::playSound( int source, int num )
{
	
	sound_st temp;
	temp.effect[0] = -1;
	temp.effect[1] = -1;
	
	temp = getSoundEffects( num );
	if( temp.effect[0] != -1 && temp.effect[1] != -1 )
		soundeffect2( source, temp.effect[0], temp.effect[1] );
	
}

void cMagic::doStaticEffect( int source, int num )
{
	stat_st temp;
	temp.effect[0] = -1;
	temp.effect[1] = -1;
	temp.effect[2] = -1;
	temp.effect[3] = -1;
	
	temp = getStatEffects( num );
	
	if( temp.effect[0] != -1 && temp.effect[1] != -1 && temp.effect[2] != -1 && temp.effect[3] != -1 )
	{
		staticeffect( source, temp.effect[0], temp.effect[1], temp.effect[2], temp.effect[3] );
	}
}

void cMagic::doMoveEffect( int num, int target, int source )
{
	move_st temp;
	temp.effect[0] = -1;
	temp.effect[1] = -1;
	temp.effect[2] = -1;
	temp.effect[3] = -1;
	temp.effect[4] = -1;
	
	temp = getMoveEffects( num );
	
	if( temp.effect[0] != -1 && temp.effect[1] != -1 && temp.effect[2] != -1 && temp.effect[3] != -1 && temp.effect[4] != -1 )
		movingeffect(source, target, temp.effect[0], temp.effect[1], temp.effect[2], temp.effect[3], temp.effect[4] );
}

bool cMagic::aggressiveSpell( int num )
{
	
	return spells[num].aggressive;
}

bool cMagic::fieldSpell( int num )
{
	switch( num )
	{
	case 24:// Wall of Stone
	case 28:// Fire Field
	case 39:// Poison Field
	case 47:// Paralyze Field
	case 50:// Energy Field
		return true;
	default:
		return false;
	}
}

// added by AntiChrist (9/99)
void cMagic::PolymorphMenu(int s,int gmindex)
{
	//printf("polymorphmenu\n");
	int total, i;
	char lentext;
	char sect[512];
	char gmtext[30][257];
	int gmid[30];
	int gmnumber=0,dummy=0;
	
	openscript("polymorph.scp");
	sprintf(sect, "POLYMORPHMENU %i",gmindex);
	if (!i_scripts[polymorph_script]->find(sect))
	{
		closescript();
		return;
	}
	read1();
	lentext=sprintf(gmtext[0], "%s", script1);
	do
	{
		read2();
		if (script1[0]!='}' && strcmp("POLYMORPHID",script1))
		{
			gmnumber++;
			gmid[gmnumber]=hstr2num(script1);
			strcpy(gmtext[gmnumber], script2);
			read1();
		}
		
	}
	while (script1[0]!='}');
	
	// reading polymorph duration time ...
	sprintf(sect, "POLYMORPHDURATION %i",dummy);
	if (!i_scripts[polymorph_script]->find(sect))
	{
		closescript();
		return;
	}
	
	do
	{
		read2();
		//printf("yaba: %s daba: %s\n",script1,script2);
		if (script1[0]!='}')
		{
			polyduration=str2num(script1);
			printf("polydur: %i\n",polyduration);
			read1();
		}
		
	}
	
	while (script1[0]!='}');
	closescript();
	
	total=9+1+lentext+1;
	for (i=1;i<=gmnumber;i++) total+=4+1+strlen(gmtext[i]);
	gmprefix[1]=total>>8;
	gmprefix[2]=total%256;
	gmprefix[3]=chars[currchar[s]].ser1;
	gmprefix[4]=chars[currchar[s]].ser2;
	gmprefix[5]=chars[currchar[s]].ser3;
	gmprefix[6]=chars[currchar[s]].ser4;
	gmprefix[7]=gmindex>>8;
	gmprefix[8]=gmindex%256;
	Network->xSend(s, gmprefix, 9, 0);
	Network->xSend(s, &lentext, 1, 0);
	Network->xSend(s, gmtext[0], lentext, 0);
	lentext=gmnumber;
	Network->xSend(s, &lentext, 1, 0);
	for (i=1;i<=gmnumber;i++)
	{
		gmmiddle[0]=gmid[i]>>8;
		gmmiddle[1]=gmid[i]%256;
		Network->xSend(s, gmmiddle, 4, 0);
		lentext=strlen(gmtext[i]);
		Network->xSend(s, &lentext, 1, 0);
		Network->xSend(s, gmtext[i], lentext, 0);
	}
}

// added by AntiChrist (9/99)
void cMagic::Polymorph(int s, int gmindex, int creaturenumber)
{
	//printf("polymorph\n");
	//printf("creaturenumber %i\n",creaturenumber);
	int i,k;
	int id1,id2;
	
	char sect[512];
	openscript("polymorph.scp");
	sprintf(sect, "POLYMORPHMENU %i",gmindex);
	if (!i_scripts[polymorph_script]->find(sect))
	{
		closescript();
		return;
	}
	read1();
	
	i=0,k=0;
	
	do
	{
		//read1();
		read2();
		printf("%s %s\n",script1,script2);
		if (script1[0]!='}' && strcmp("POLYMORPHID",script1))
		{
			i++;
		}
	}
	while (script1[0]!='}' && i<creaturenumber);
	
	read2();
	//printf("%s %s\n",script1,script2);
	k=hstr2num(script2);
	
	closescript();
	
	//printf("K: %x\n",k);
	
	id1=k>>8;
	id2=k%256;
	
	soundeffect2(currchar[s], 0x02, 0x0F);
	tempeffect(currchar[s],currchar[s],18,id1,id2,0);
	
	teleport(currchar[s]);
}

// only used for the /heal command
// LB
void cMagic::Heal( UOXSOCKET s)
{
	int defender, i, c = currchar[s];
	defender = calcserial( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	i = findbyserial( &charsp[defender%HASHMAX], defender, 1 );
	
	if (i!=-1)
	{
		playSound( c, 4 );
		doStaticEffect( i, 4 );
		chars[i].hp=chars[i].st;
		updatestats(i, 0);  
	}
	else
	{
		sysmessage(s,"No valid heal target");
	}
}

// only used for the /recall command
// AntiChrist
void cMagic::Recall( UOXSOCKET s )
{
	// Targeted item
	int item = calcserial( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	int i = findbyserial( &itemsp[item%HASHMAX], item, 0 );
	
	if( i == -1 ) return; // LB crashfix
	if( items[i].morex <= 200 && items[i].morey <= 200 )
	{
		sysmessage( s, "That rune has not been marked yet!" );
	}
	else
	{
		mapRegions->RemoveItem(currchar[s]+1000000); //LB
		
		chars[currchar[s]].x=items[i].morex;
		chars[currchar[s]].y=items[i].morey;
		chars[currchar[s]].dispz=chars[currchar[s]].z=items[i].morez;
		
		mapRegions->AddItem(currchar[s]+1000000); //LB
		
		teleport(currchar[s]);
		sysmessage(s,"You have recalled from the rune.");
	}
}

// only used for the /mark command
// AntiChrist
void cMagic::Mark( UOXSOCKET s )
{
	// Targeted item
	int item = calcserial( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	int i = findbyserial( &itemsp[item%HASHMAX], item, 0 );
	if( i == -1 ) return; // lb crashfix
	
	items[i].morex=chars[currchar[s]].x;
	items[i].morey=chars[currchar[s]].y;
	items[i].morez=chars[currchar[s]].z;
	
	if (region[chars[currchar[s]].region].name[0]!=0)
		sprintf(items[i].name, "a recall rune for %s",region[chars[currchar[s]].region].name);
	else 
		strcpy(items[i].name, " a recall rune for An Unknown Location");
	
	sysmessage(s,"Recall rune marked."); 
}

// only used for the /gate command
// AntiChrist
void cMagic::Gate( UOXSOCKET s)
{
	int c, n;
	
	// Targeted item
	int item = calcserial( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	int i = findbyserial( &itemsp[item%HASHMAX], item, 0 );
	if( i == -1 ) return;
	
	if ( items[i].morex<=200 && items[i].morey<=200)
	{
		sysmessage( s, "That rune has not been marked yet!" );
	}
	else
	{
		gatex[gatecount][0]=chars[currchar[s]].x;  //create gate a player location
		gatey[gatecount][0]=chars[currchar[s]].y;
		gatez[gatecount][0]=chars[currchar[s]].z;
		
		gatex[gatecount][1]=items[i].morex; //create gate at marked location
		gatey[gatecount][1]=items[i].morey;
		gatez[gatecount][1]=items[i].morez;
		
		for (n=0;n<2;n++)  
		{
			//strcpy(temp,"a blue moongate");
			c=Items->SpawnItem(s,1,"#",0,0x0f,0x6c,0,0,0,0);
			if( c > -1 )
			{
				items[c].type=51+n;
				items[c].x=gatex[gatecount][n];
				items[c].y=gatey[gatecount][n];
				items[c].z=gatez[gatecount][n];
				items[c].gatetime = (unsigned int)(uiCurrentTime+(server_data.gatetimer*CLOCKS_PER_SEC));
				items[c].gatenumber=gatecount;
				items[c].dir=1;
				
				mapRegions->AddItem(c);  //add gate to list of items in the region
				//for (j=0;j<now;j++) if (perm[j]) senditem(j,c);
				RefreshItem( c ); // AntiChrist
			}
			if (n==1)
			{
				gatecount++;
				if (gatecount>MAXGATES) gatecount=0;
			}
			addid1[s]=0;
			addid2[s]=0;
		}
	}
}

unsigned char cMagic::HasSpell( int book, int spellNum )
{
	if( book == -1 )
		return 0;
	int wordNum = spellNum / 32;
	int bitNum = (spellNum % 32);
	int sourceAmount;
	switch( wordNum )
	{
	case 0:	sourceAmount = items[book].morex;	break;
	case 1:	sourceAmount = items[book].morey;	break;
	case 2:	sourceAmount = items[book].morez;	break;
	default: sourceAmount = 0;					break;
	};
	return((sourceAmount>>bitNum)%2);
}
void cMagic::AddSpell( int book, int spellNum )
{
	if( book == -1 )
		return;
	int wordNum = spellNum / 32;
	int bitNum = (spellNum % 32);
	int flagToSet = (1<<bitNum);
	switch( wordNum )
	{
	case 0:		items[book].morex |= flagToSet;		return;
	case 1:		items[book].morey |= flagToSet;		return;
	case 2:		items[book].morez |= flagToSet;		return;
	default:										return;
	};
}