//------------------------------------------------------------------------
//  dbl_sinlge_click.cpp
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

#include "uox3.h"
#include "debug.h"
#include "cmdtable.h"

#define DBGFILE "dbl_single_click.cpp"


void doubleclick(int s) // Completely redone by Morrolan 07.20.99
{
	unsigned char a1, a2, a3, a4;
	int x, i, j, c, w = 0, k, keyboard, serial, z, y;
	char pdoll[67]= {"\x88\x00\x05\xA8\x90", };	// knox: comma at end tells compiler to fill the rest with NULLs
	char map1[20]="\x90\x40\x01\x02\x03\x13\x9D\x00\x00\x00\x00\x13\xFF\x0F\xA0\x01\x90\x01\x90";
	char map2[12] = {"\x56\x40\x01\x02\x03\x05", };
	int p = -1;
	unsigned long int itemids;
	unsigned int itype;
	
	a1 = buffer[s][1]&0x7F;
	a2 = buffer[s][2];
	a3 = buffer[s][3];
	a4 = buffer[s][4];
	serial = calcserial(a1,a2,a3,a4);
	keyboard=buffer[s][1]&0x80;
	char temp[512];
	
	x = calcCharFromSer( serial );
	if( x != -1 ) 
	{ // Begin Characters/NPCs Section 
		unsigned char cID2 = chars[x].id2;
		if( ( chars[x].npc && chars[x].id1 == 0x00 ) &&
			( cID2 == 0xC8 || cID2 == 0xE2 || cID2 == 0xE4 || cID2 == 0xCC ||
			  cID2 == 0xDC || cID2 == 0xD2 || cID2 == 0xDA || cID2 == 0xDB ) )
		{//if mount
			if( chardist( currchar[s], x ) < 2 )
			{
				if( chars[currchar[s]].id1!=chars[currchar[s]].orgid1 || chars[currchar[s]].id2!=chars[currchar[s]].orgid2)
				{
					sysmessage( s, "You cannot ride anything under polymorph effect.");
					return;
				}
				if (chars[currchar[s]].dead) sysmessage(s,"You are dead and cannot do that.");
				else mounthorse(s, x);
			}
			else sysmessage(s, "You need to get closer.");
			return; 
		}//if mount
		else if ((chars[x].npc)&&((chars[x].id1!=0x01)||(chars[x].id2<0x90)||(chars[x].id2>0x93)))
		{//if monster
			if (chars[x].id1==0x01&&(chars[x].id2==0x23||chars[x].id2==0x24))
			{//if packhorse or packlhama added by JustMichael 8/31/99
				if (chars[x].ownserial==chars[currchar[s]].serial)
				{
					y=packitem(x);
					if( y != -1 )
					{
						backpack(s,items[y].ser1,items[y].ser2,items[y].ser3,items[y].ser4);
					}
					else
					{
						printf( "Pack animal %i has no backpack!\n", chars[x].serial );
					}
				}
				else
				{
					sysmessage(s, "That is not your beast of burden!");
				}
				return;
			}
			else
			{
				sysmessage(s, "You cannot open monsters paperdolls.");
			}
			return; 
		}//if monster
		else 
		{//char
			if (chars[x].npcaitype==17)//PlayerVendors
			{
				npctalk(s,x,"Take a look at my goods.", 0);
				y=packitem(x);
				if (y!=-1) backpack(s,items[y].ser1,items[y].ser2,items[y].ser3,items[y].ser4); // rippers bugfix for vendor bags not opening !!!
				return;
			}
			if (chars[currchar[s]].serial==serial)
			{//dbl-click self
				if ((!keyboard)&&(unmounthorse(s)==0)) return; //on horse
				//if not on horse, treat ourselves as any other char
			}//self
			pdoll[1]=a1;
			pdoll[2]=a2;
			pdoll[3]=a3;
			pdoll[4]=a4;
			if (chars[x].priv&0x01)	// not GM!!	// 0x01 is GM bit, not 0x10
			{//GM.
				sprintf(&pdoll[5], "%s %s", chars[x].name, chars[x].title);
			}
			else if( chars[x].npc )
			{
				sprintf( &pdoll[5], "%s %s %s", title3(x), chars[x].name, chars[x].title );
			}
			// ripper ..rep stuff
			// Abaddon... ummm... no race!?... otherwise there is NO indication what a person's race is!
			// Also, if you are going to change things, please leave a name!!!!
			else if ((chars[x].crimflag>0) && (!(chars[x].dead) && (chars[x].kills<4)))
			{
				sprintf(&pdoll[5], "The Criminal %s, %s%s %s", chars[x].name, chars[x].title, title1(x), title2(x));
			}
			else if ((chars[x].kills>=5) && (!(chars[x].dead) && (chars[x].kills<10)))
			{
				sprintf(&pdoll[5], "The Serial Killer %s, %s%s %s", chars[x].name, chars[x].title, title1(x), title2(x));
			}
			else if ((chars[x].kills>=10) && (!(chars[x].dead) && (chars[x].kills<20)))
			{
				sprintf(&pdoll[5], "The Murderer %s, %s%s %s", chars[x].name, chars[x].title, title1(x), title2(x));
			}
			else if ((chars[x].kills>=20) && (!(chars[x].dead) && (chars[x].kills<50)))
			{
				sprintf(&pdoll[5], "The Mass Murderer %s, %s%s %s", chars[x].name, chars[x].title, title1(x), title2(x));
			}
			else if ((chars[x].kills>=50) && (!(chars[x].dead) && (chars[x].kills<100)))
			{
				sprintf(&pdoll[5], "The Evil Dread Murderer %s, %s%s %s", chars[x].name, chars[x].title, title1(x), title2(x));
			}
			else if ((chars[x].kills>=100) && (!(chars[x].dead)))
			{
				sprintf(&pdoll[5], "The Evil Emperor %s, %s%s %s", chars[x].name, chars[x].title, title1(x), title2(x));
			} // end of rep stuff
			else
			{//Player.
				sprintf(&pdoll[5], "%s%s", title3(x), chars[x].name);      //Repuation + Name
				if ((chars[x].towntitle)||(chars[x].townpriv==2))
				{//TownTitle.
					if (chars[x].townpriv==2)
					{//Mayor
						sprintf(&pdoll[5], "The Mayor %s of %s, %s %s", chars[x].name, Towns->TownName(chars[x].town,3), title1(x), title2(x));
					}
					else
					{//Resident
						sprintf(&pdoll[5], "%s of %s, %s %s", chars[x].name, Towns->TownName(chars[x].town,3), title1(x), title2(x));
					}
				}
				else
				{//NoTownTitle
					strcpy(temp,&pdoll[5]);                                 //?
					if (strlen(chars[x].title)>0)
					{//Titled & Skill
						sprintf(&pdoll[5], "%s %s, %s %s", temp, chars[x].title, title1(x), title2(x));
					}
					else
					{//Just skilled
						sprintf(&pdoll[5], "%s, %s %s", temp, title1(x), title2(x));
					}
				}
			}
			Network->xSend(s, pdoll, 66, 0);
			return;
		}//char
	}//End of Characters/NPCs Section
	if( ( chars[currchar[s]].objectdelay >= uiCurrentTime || overflow ) && !( chars[currchar[s]].priv&0x01 ) )
	{
		sysmessage( s, "You must wait to perform another action." );
		return;
	}
	else
		chars[currchar[s]].objectdelay = (unsigned int)( server_data.objectdelay * CLOCKS_PER_SEC + uiCurrentTime );
	
	x = calcItemFromSer( serial );
	//PlayerVendors
	if( x != -1 )
	{
		if( items[x].cont1 >= 0x40 && items[x].type != 1 && items[x].contserial != -1 )
		{	//Can't use stuff that isn't in your pack.
			p = GetPackOwner( calcItemFromSer( items[x].contserial ) );
			if( p != -1 )
				if( p != currchar[s] )
					return;
		} else if ( items[x].cont1<0x40 && items[x].type!=-1 && items[x].contserial!=-1)
		{//in a character.
			p = calcCharFromSer( items[x].contserial );
			if( p != -1 )
				if( p != currchar[s] && items[x].layer != 15 && items[x].type != 1 )
					return;
		}
		if (items[x].type==217)//PlayerVendors deed
		{
			p=packitem(currchar[s]);
			int m = Npcs->AddNPCxyz(-1,2117,0,chars[currchar[s]].x,chars[currchar[s]].y,chars[currchar[s]].z);
			
			chars[m].npcaitype=17;
			chars[m].priv=chars[m].priv|4;//make 'em invul
			chars[m].hidden=0;
			chars[m].dir = chars[currchar[s]].dir;
			chars[m].npcWander=0;
			chars[m].flag=4;
			setserial(m,currchar[s],5);
			sprintf(temp, "Hello sir! My name is %s and i will be working for you.",chars[m].name);
			npctalk(s,m,temp, 0);
			Items->DeleItem(x);
			updatechar(m);
			teleport(m);
			return;
		}
		
		
		// doesn't work all the time
		if( !(chars[currchar[s]].priv&0x01) && items[x].multis != chars[currchar[s]].multis )	// different multis!
		{
			if( items[x].contserial == -1 && items[x].type != 12 && items[x].type != 13 )	// only check things on the ground
			{
				sysmessage( s, "That is too far away and you cannot reach it" );
				return;
			}
		}
		// Begin Items/Guildstones Section 
		itype=items[x].type;
		itemids = (unsigned long int)items[x].id1 << 8;
		itemids += (unsigned int)items[x].id2;
		if( itype == 16 )			// Check for 'resurrect item type' this is the ONLY type one can use if dead.
		{
			if (chars[currchar[s]].dead==1)
			{
				Targ->NpcResurrectTarget( currchar[s] );
				sysmessage(s, "You have been resurrected.");
				return;
			} 
			else 
			{
				sysmessage(s, "You are already living!");
				return;
			}
		}
		else if (chars[currchar[s]].dead==1) //if you are dead and it's not an ankh, FORGET IT!
		{
			sysmessage(s, "You may not do that as a ghost.");
			return;
		}
		else if  (!(chars[currchar[s]].priv&0x01) && (items[x].layer!=0)&&
			(items[x].contserial!=chars[currchar[s]].serial))
		{//can't use other people's things!
			if( !(items[x].layer==0x15 && server_data.rogue==1))  // bugfix for snooping not working, LB
			{
				sysmessage(s, "You cannot use items equipped by other players.");
				return;
			}
		}
		// Begin checking objects that we force an object delay for (std objects)
		//			else if ((chars[currchar[s]].objectdelay<=uiCurrentTime)||(chars[currchar[s]].priv&1))
		else if( s >= 0 )
		{
			//chars[currchar[s]].objectdelay=uiCurrentTime+(server_data.objectdelay*CLOCKS_PER_SEC);
			//start trigger stuff
			if (items[x].trigger > 0)
			{
				// if (iteminrange(s,x,1))  // bugfix for trigger not working in backpacks, lb
				if (items[x].trigtype==0)
				{
					if (!items[x].disabled)
					{
						triggerwitem(s, x, 1); //if players uses trigger
						return;
					} 
					else 
					{
						if( ( items[x].disabled != 0 ) && ( ( items[x].disabled <= uiCurrentTime ) || (overflow)))
						{
							items[x].disabled = 0;	// re-enable it
							triggerwitem( s, x, 1 );
						}
						else
							sysmessage(s,"That doesnt seem to work right now.");
						return;
					}
				}
				else 
				{
					sysmessage(s, "You are not close enough to use that.");
					return;
				}
			}
			//check this on trigger in the event that the .trigger property is not set on the item
			//trigger code.  Check to see if item is envokable by id
			else if (checkenvoke(items[x].id1,items[x].id2))
			{
				chars[currchar[s]].envokeitem=x;
				chars[currchar[s]].envokeid1=items[x].id1;
				chars[currchar[s]].envokeid2=items[x].id2;
				target(s, 0, 1, 0, 24, "What will you use this on?");
				return;
			}
			// end trigger stuff
			// BEGIN Check items by type 
		}
		//taken from 6904t2(5/10/99) - AntiChrist
		/*else // Lb, bugkilling, moved from end to here
		{
		sysmessage( s, "You must wait to perform another action." );
			}*/
		UI16 wx, wy;
		switch (itype)
		{
		case 181: //Fireworks wands
			srand( uiCurrentTime );
			if (items[x].morex<=0)
			{
				sysmessage(s,  "That is out of charges.");
				return;
			}
			items[x].morex--;
			sysmessage(s, "Your wand now has %i charges left", items[x].morex);
			
			for (x=0;x<(rand()%4+2);x++)
			{
				wx = (chars[currchar[s]].x+(rand()%11-5));
				wy = (chars[currchar[s]].y+(rand()%11-5));
				movingeffect3(currchar[s], (unsigned short)(wx), (unsigned short)(wy), chars[currchar[s]].z+10, (unsigned char)(0x36),(unsigned char)(0xE4),17,0,rand()%2);
				switch( (char)(rand()%3) )
				{
				case 0:
					staticeffect3(wx,wy,chars[currchar[s]].z+10,0x37,0x3A,0x09,0,0);
					break;
				case 1:
					staticeffect3(wx,wy,chars[currchar[s]].z+10,0x37,0x4A,0x09,0,0);
					break;
				case 2:
					staticeffect3(wx,wy,chars[currchar[s]].z+10,0x37,0x5A,0x09,0,0);
					break;
				case 3:
					staticeffect3(wx,wy,chars[currchar[s]].z+10,0x37,0x6A,0x09,0,0);
					break;
				case 4:
					staticeffect3(wx,wy,chars[currchar[s]].z+10,0x37,0x7A,0x09,0,0);
					break;
				}
			}
			return;
			
		case 117:// backpacks - snooping a la Zippy - add check for server_data.rogue later- Morrolan
			
			//Boats ->
			//if (items[x].type2==3)
			//{
				if(iteminrange(s,x,6))
				{
					if (items[x].id2==0x84 || items[x].id2==0xD5 || items[x].id2==0xD4 || items[x].id2==0x89)
						Boats->PlankStuff(s,x);
					else {
						sysmessage(s,"That is locked.");//Locked
					}
				} else sysmessage(s, "You can't reach that!");
				return;
			//}
			//End Boats --^ 
		case 1: // bugfix for snooping not working, lb
		case 63: 
			if( items[x].moreb1 ) Magic->MagicTrap( currchar[s], x ); // added by AntiChrist
			// only 1 and 63 can be trapped, so pleaz leave it here :) - Anti
		case 65:
		case 87:  // <<- What is this people? Keep labeling things this is stupid.
			int npc, contser;
			contser=items[x].contserial; //calcserial(items[x].cont1,items[x].cont2,items[x].cont3,items[x].cont4);

			int packOwner;
			packOwner = GetPackOwner( x );
			
			npc=findbyserial(&charsp[items[x].contserial%HASHMAX], items[x].contserial, 1);
			
			if ((npcinrange(s,npc,2))||(chars[currchar[s]].priv&0x01)||(iteminrange(s,x,2)))
			{	
				if (((npc==-1) || (chars[currchar[s]].serial == chars[npc].serial)) || ((chars[currchar[s]].priv&0x80) || (chars[currchar[s]].priv&0x01))) // Lord Vader fix
					backpack(s, a1, a2, a3, a4);
				else
				{
					if( ( packOwner != currchar[s] ) && ( !( chars[currchar[s]].priv&0x01 ) ) )	// SNOOPING
					{
						if ((chars[npc].priv&0x80) || (chars[npc].priv&0x01))//Counselor or GM
						{
							sysmessage(s,"You failed to peek into that container.");
							sprintf(temp, "%s is snooping you!",chars[currchar[s]].name);
							sysmessage(calcSocketFromChar(npc), temp);
							return;
						}
						else if (Skills->CheckSkill(currchar[s],SNOOPING,0,1000))
						{
							backpack(s, a1, a2, a3, a4);
							sysmessage(s,"You successfully peek into that container.");
						}
						else
						{
							sysmessage(s,"You failed to peek into that container.");
							if (chars[npc].npc) npctalk(s,npc, "Art thou attempting to disturb my privacy?", 0);
							sprintf(temp,"You notice %s trying to peek into your pack!",chars[currchar[s]].name);
							z=calcSocketFromChar(npc);
							if (z!=-1) sysmessage(z,temp); 
							if (chars[currchar[s]].karma<=1000) chars[currchar[s]].karma-=10;
						}
					}
				} 
				
			}
			else
			{
				sysmessage(s,"You are too far away!");
			}
			return;
			
		case 2: // Order gates?
			for (j=0;j<itemcount;j++) if (items[j].type==3)
			{
				if (items[j].morez==1)
				{
					items[j].morez=2;
					items[j].z = items[j].z+17;
					//								for (k=0;k<now;k++) if (perm[k]) senditem(k,j);
					RefreshItem( j ); // AntiChrist
					w=1;
				}
				else if (items[j].morez==2)
				{
					items[j].morez=1;
					items[j].z = items[j].z-17;
					//								for (k=0;k<now;k++) if (perm[k]) senditem(k,j);
					RefreshItem( j ); // AntiChrist
					w=0;
				}
			}
			//						if (w==1) sysmessage(s, "Order Gate opened.");
			//						else sysmessage(s, "Order Gate closed.");
			return;//order gates
		case 4: // Chaos gates?
			for (j=0;j<itemcount;j++) if (items[j].type==5)
			{
				if (items[j].morez==3)
				{
					items[j].morez=4;
					items[j].z = items[j].z+17;
					//								for (k=0;k<now;k++) if (perm[k]) senditem(k,j);
					RefreshItem( j ); // AntiChrist
					w=1;
				}
				else if (items[j].morez==4)
				{
					items[j].morez=3;
					items[j].z = items[j].z - 17;
					//								for (k=0;k<now;k++) if (perm[k]) senditem(k,j);
					RefreshItem( j ); // AntiChrist
					w=0;
				}
			}
			//						if (w==1) sysmessage(s, "Chaos Gate opened.");
			//						else sysmessage(s, "Chaos Gate closed.");
			return;//chaos gates
		case 6: //teleport item (ring?)
			target(s, 0, 1, 0, 2, "Select teleport target.");
			return;//case 6
		case 7: //key
			addid1[s]=items[x].more1;
			addid2[s]=items[x].more2;
			addid3[s]=items[x].more3;
			addid4[s]=items[x].more4;
			if (items[x].more1==255) addid1[s]=255;
			target(s, 0, 1, 0, 11, "Select item to use the key on.");
			return;//case 7 (keys)
		case 8:
		case 64: //locked container //Morrolan traps?
			// Added traps effects by AntiChrist
			if( items[x].moreb1 ) Magic->MagicTrap( currchar[s], x ); 
			sysmessage(s, "This item is locked.");
			return;//case 8/64 (locked container)
		case 9: //spellbook
			k=packitem(currchar[s]);
			if (k!=-1) //morrolan
			{
				if ((items[x].contserial==items[k].serial) || (items[x].contserial==chars[currchar[s]].serial) && (items[x].layer==1))
				{
					Magic->SpellBook(s);
				}
				else
				{
					sysmessage(s, "If you wish to open a spellbook, it must be equipped or in your main backpack.");
				}
			}
			return;//spellbook
		case 10: //map?
			map1[1]=items[x].ser1;
			map1[2]=items[x].ser2;
			map1[3]=items[x].ser3;
			map1[4]=items[x].ser4;
			map2[1]=items[x].ser1;
			map2[2]=items[x].ser2;
			map2[3]=items[x].ser3;
			map2[4]=items[x].ser4;
			Network->xSend(s, map1, 19, 0);
			Network->xSend(s, map2, 11, 0);
			return;//maps
		case 11: //book (not spellbooks)
			if (items[x].morex!=666 && items[x].morex!=999) 
				Books->openbook_old(s, x); 
			else if (items[x].morex==666) 
				Books->openbook_new(s,x,1); 
			else 
				Books->openbook_new(s,x,0);
		    return;//book
		case 12: //door(unlocked)
			dooruse(s, x);
            chars[currchar[s]].objectdelay = (unsigned int) (( server_data.objectdelay * CLOCKS_PER_SEC + uiCurrentTime ) / 2);
			return; //doors
		case 13: //locked door
			p=packitem(currchar[s]);
			if (p>-1) //LB
			{
				/*for (j=0;j<itemcount;j++) //Morrolan come back here and change this to search only backpack items 
				{
					if (items[j].type==7 && p!=-1)
					{
						if	(((items[j].more1==items[x].more1)&&(items[j].more2==items[x].more2)&&
							(items[j].more3==items[x].more3)&&(items[j].more4==items[x].more4))&&
							((items[j].cont1==items[p].ser1)&&(items[j].cont2==items[p].ser2)&&
							(items[j].cont3==items[p].ser3)&&(items[j].cont4==items[p].ser4)))
						{
							npctalk(s,currchar[s],"You quickly unlock, use, and then relock the door.", 0);
							dooruse(s, x);
							return;
						}//if
						}
				}//for
				*/
				for (j=0;j<contsp[items[p].serial%HASHMAX].max;j++)
				{
					k=contsp[items[p].serial%HASHMAX].pointer[j];
					if (k > -1 && items[k].contserial==items[p].serial)
					{
						if	(((items[k].more1==items[x].more1)&&(items[k].more2==items[x].more2)&&
							(items[k].more3==items[x].more3)&&(items[k].more4==items[x].more4))&&
							((items[k].cont1==items[p].ser1)&&(items[k].cont2==items[p].ser2)&&
							(items[k].cont3==items[p].ser3)&&(items[k].cont4==items[p].ser4)))
						{
							npctalk(s,currchar[s],"You quickly unlock, use, and then relock the door.", 0);
							dooruse(s, x);
							chars[currchar[s]].objectdelay = (unsigned int) (( server_data.objectdelay * CLOCKS_PER_SEC + uiCurrentTime ) / 2);
							return;
						}//if
					}
				}//for
			} // end if p!=-1
			sysmessage(s, "This door is locked.");
			return;//case 13 (locked door)
		case 14: // For eating food
			if (chars[currchar[s]].hunger >= 6)
			{
				sysmessage(s, "You are simply too full to eat any more!");
				return;
			}
			else
			{
				int foodsnd; 
				foodsnd=rand()%3;
				switch(foodsnd)
				{
				case 0: soundeffect2(currchar[s], 0x00, 0x3A);
					break;
				case 1: soundeffect2(currchar[s], 0x00, 0x3B);
					break;
				case 2: soundeffect2(currchar[s], 0x00, 0x3C);
					break;
				default: soundeffect2(currchar[s], 0x00, 0x3A);
					break;
				}//switch(foodsnd)
				
				switch(chars[currchar[s]].hunger)
				{
				case 0:  sysmessage(s, "You eat the food, but are still extremely hungry.");
					break;
				case 1:  sysmessage(s, "You eat the food, but are still extremely hungry.");
					break;
				case 2:  sysmessage(s, "After eating the food, you feel much less hungry.");
					break;
				case 3:  sysmessage(s, "You eat the food, and begin to feel more satiated.");
					break;
				case 4:  sysmessage(s, "You feel quite full after consuming the food.");
					break;
				case 5:  sysmessage(s, "You are nearly stuffed, but manage to eat the food.");
					break;
				case 6:  sysmessage(s, "You are simply too full to eat any more!");
					break;
				default:  sysmessage(s, "You are simply too full to eat any more!");
					break;
				}//switch(chars[currchar[s]].hunger)
				
				if( ( items[x].poisoned ) && ( chars[currchar[s]].poisoned < items[x].poisoned ) )
				{

					switch(rand()%3)
					{
					case 0:
						sysmessage( s, "You feel disoriented and nauseous");
						break;
					case 1:
						sysmessage( s, "That tasted funny");
						break;
					case 2:
						sysmessage( s, "Your stomach turns over");
						break;
					}

					soundeffect2( currchar[s], 0x02, 0x46 ); //poison sound - SpaceDog
					chars[currchar[s]].poisoned = items[x].poisoned;
					chars[currchar[s]].poisonwearofftime = ( uiCurrentTime + ( CLOCKS_PER_SEC * server_data.poisontimer ) );
					impowncreate( s, currchar[s], 1 ); // sends the green bar
				}
				//Remove a food item
				if( items[x].amount != 1 )
				{
					items[x].amount--;
					RefreshItem( x );
				}
				else
				{
					Items->DeleItem(x);
				}
				chars[currchar[s]].hunger++;
			}
			return; //case 14 (food)
		case 15: //magic items (do we still need this with itemspell?) //Morrolan
			if (items[x].morez!=0)
			{
				// modified by AntiChrist for use with newSelectSpell2Cast
				//							if(Magic->CastSpell(s, (8*(items[x].morex-1))+items[x].morey, 2, x))
				currentSpellType[s] = 2; // set the spell type to cast ( an item spell, so no mana req, etc.....)
				if( Magic->newSelectSpell2Cast( s, (8*(items[x].morex-1))+items[x].morey ) ) 
					
				{ 
					items[x].morez--;
					if (items[x].morez == 0)
					{
						items[x].type = items[x].type2;
						items[x].morex = 0;
						items[x].morey = 0;
						items[x].offspell = 0;
					}
				}
			}
			return; //case 15 (magic items)
		case 18: //crystal ball?
//			int a1, a2, a3, a4, j;
			a1 = items[x].ser1;
			a2 = items[x].ser2;
			a3 = items[x].ser3;
			a4 = items[x].ser4;
			j = rand()%10;
			switch(j)
			{
			case 0:  itemmessage(s,"Seek out the mystic llama herder.", a1, a2, a3, a4);
				break;
			case 1:  itemmessage(s,"Wherever you go, there you are.", a1, a2, a3, a4);
				break;
			case 2:  itemmessage(s,"Quick! Lord British is giving away gold at the castle!", a1, a2, a3, a4);
				break;
			case 3:  itemmessage(s,"Remember, change your underwear once a day.", a1, a2, a3, a4);
				break;
			case 4:  itemmessage(s,"The message appears to be too cloudy to make anything out of it.", a1, a2, a3, a4);
				break;
			case 5:  itemmessage(s,"You have just lost five strength.. not!", a1, a2, a3, a4);
				break;
			case 6:  itemmessage(s,"You're really playing a game you know", a1, a2, a3, a4);
				break;
			case 7:  itemmessage(s,"You will be successful in all you do.", a1, a2, a3, a4);
				break;
			case 8:  itemmessage(s,"You are a person of culture.", a1, a2, a3, a4);
				break;
			case 9:  itemmessage(s,"Give me a break! How much good fortune do you expect!", a1, a2, a3, a4);
				break;
			default: itemmessage(s,"Give me a break! How much good fortune do you expect!", a1, a2, a3, a4);
				break;
			}//switch
			soundeffect(s, 0x01, 0xEC);
			return;//case 18 (crystal ball?)
			case 19: //potions
				if( chars[p].usepotion == 1 )
					sysmessage( s, "You must wait a while before using another potion" );
				else
					usepotion( currchar[s], x );
				return; //case 19 (potions)
			case 35: //townstones
				// mithos fixes to make deeds work
				if(( itemids == 0x14F0 ))	// Check for Deed
				{
					chars[currchar[s]].fx1 = x;
					comm[1] = "3298";
					command_additem( s );
					Items->DeleItem( x );
					return;
				}
				else
				{
					
					Towns->Menu(s, x, 3);
					return;
				}
				
			case 50: //rune
				if ( items[x].morex == 0 && items[x].morey == 0 && items[x].morez == 0 )	// changed, to fix, Lord Vader
				{	
					sysmessage(s,"That rune is not yet marked!");
				}
				else
				{
					chars[currchar[s]].runenumb=x;
					sysmessage(s,"Enter new rune name.");
				}
				return;//case 50 (rune)
			case 51: // Moongates
			case 52:
			case 60:
				if( iteminrange( s, x, 2 ) )
				{
					// Teleport the current character
					chars[currchar[s]].x = items[x].x;
					chars[currchar[s]].y = items[x].y;
					chars[currchar[s]].z = chars[currchar[s]].dispz = items[x].z;
					teleport( s );
					Magic->GateCollision( currchar[s] );
				}
				return; // case Moongates
			case 185: // let's smoke! :)
				chars[currchar[s]].smoketimer = (items[x].morex * CLOCKS_PER_SEC + uiCurrentTime);
				sysmessage( s, "You feel much better after you smoke... *cough* *splutter*" );
				Items->DeleItem( x );
				return;
			case 186: // rename deed! -- eagle 1/29/00
				if ( items[x].type != 186 )	
				{	
					sysmessage(s,"This is not a rename deed!");
				}
				else
				{
					chars[currchar[s]].namedeed=x;
					sysmessage(s,"Enter your new name.");
					Items->DeleItem( x );
				}
				return;// rename deed! -- eagle 1/29/00

			case 100:  //type 100?  this ain't in the docs... - Morrolan
				for(i=0;i<itemcount;i++)
				{
					if(((items[i].moreb1==items[x].morex)&&(items[i].moreb2==items[x].morey)&&(items[i].moreb3==items[x].morez))||((items[i].morex==items[x].morex)&&(items[i].morey==items[x].morey)&&(items[i].morez==items[x].morez))&&((i!=x)&&(items[x].morex!=0)&&(items[x].morey!=0)&&(items[x].morez!=0)))
					{ 
						
						if((items[i].morex==0)&&(items[i].morey==0)&&(items[i].morez==0))
						{ 
							items[i].morex=items[i].moreb1;
							items[i].morey=items[i].moreb2;
							items[i].morez=items[i].moreb3;
							items[i].visible=0;
							//									for (j=0;j<now;j++) if (perm[j]) senditem(j,i);
							RefreshItem( i ); // AntiChrist
						} 
						else 
						{
							items[i].moreb1=items[i].morex;
							items[i].moreb2=items[i].morey;
							items[i].moreb3=items[i].morez;
							items[i].morex=0;
							items[i].morey=0;
							items[i].morez=0;
							items[i].visible=2;
							//									for (j=0;j<now;j++) if (perm[j]) senditem(j,i);
							RefreshItem( i ); // AntiChrist
						}
					}
				}
				return; //case 100
			case 101: //??
				i=items[x].morex;
				chars[currchar[s]].id1 = (unsigned char)(i>>8);
				chars[currchar[s]].id2 = (unsigned char)(i%256); 
				teleport(currchar[s]);
				items[x].type=102;
				return; //case 101
			case 102: //??
				chars[currchar[s]].id1=chars[currchar[s]].xid1; 
				chars[currchar[s]].id2=chars[currchar[s]].xid2; 
				teleport(currchar[s]);
				items[x].type=101;
				return; //case 102
			case 103: //Army enlistment
				enlist(s, items[x].morex);
				Items->DeleItem(x);
				return; //case 103 (army enlistment object)
			case 104: // Teleport object
				mapRegions->RemoveItem(currchar[s]+1000000); //lb
				chars[currchar[s]].x=items[x].morex;
				chars[currchar[s]].y=items[x].morey;
				mapRegions->AddItem(currchar[s]+1000000);
				chars[currchar[s]].z=chars[currchar[s]].dispz=items[x].morez;
				teleport(currchar[s]);
				return; //case 104 (teleport object (again?))
			case 105:  // For drinking
				soundeffect2(currchar[s], 0x00, 0x30+rand()%2);
				//Remove a drink
				if (rand()%2)
					npctalk(s, currchar[s], "Ahh...", 0);
				else
					npcemote(s, currchar[s], "*Gulp*", 0);

				if (items[x].amount>1)
				{
					items[x].amount--;
					RefreshItem( x ); // AntiChrist
				}
				else if (!(rand()%5) || items[x].amount==1)
				{//they emptied it.
					items[x].type = items[x].amount = 0;
					sysmessage(s, "You drank the whole thing!");
					switch(itemids)
					{
					case 0x09F0:
					case 0x09AD:
					case 0x0FF8:
					case 0x0FF9: 
					case 0x1F95:
					case 0x1F96:
					case 0x1F97:
					case 0x1F98:
					case 0x1F99:
					case 0x1F9A:
					case 0x1F9B:
					case 0x1F9C:
					case 0x1F9D:
					case 0x1F9E://Pitchers
						items[x].id1 = 0x09;
						items[x].id2 = 0xA7; 
						break;
						
					case 0x09EE:
					case 0x09EF:
					case 0x1F7D:
					case 0x1F7E:
					case 0x1F7F:
					case 0x1F80:
					case 0x1F85:
					case 0x1F86:
					case 0x1F87:
					case 0x1F88:
					case 0x1F89:
					case 0x1F8A:
					case 0x1F8B:
					case 0x1F8C:
					case 0x1F8D:
					case 0x1F8E:
					case 0x1F8F:
					case 0x1F90:
					case 0x1F91:
					case 0x1F92:
					case 0x1F93:
					case 0x1F94://glasses
						items[x].id1 = 0x1F;
						items[x].id2 = 0x84;
						break;
					}
				}
				return; //	case 105 (drinks)
			case 202:
					if((itemids==0x14F0)||(itemids==0x1869))
						// Check for Deed/Teleporter + Guild Type
					{
						chars[currchar[s]].fx1=x;
						Guilds->StonePlacement(s);
						return;
					}
					else if(itemids==0x0ED5)
						// Check for Guildstone + Guild Type
					{
						chars[currchar[s]].fx1=x;
						Guilds->Menu(s,1);
						return;
					}
					else printf("Unhandled guild item type named: %s with ID of: %X\n", items[x].name, itemids);
					return;
					// End of guild stuff
				case 203: // Open a gumpmenu - Crackerjack 8/9/99
					int sec;
					sec=0;
					switch(items[x].morey) 
					{
					case 1: // Must Be Owner
						if(items[x].ownserial!=chars[currchar[s]].serial) sec=1;
						break;
					default:
						break;
					}
					if((sec)&&(items[x].morez==0)) 
					{
						sysmessage(s, "You can not use that.");
						return;
					}
					addid1[s]=items[x].ser1;
					addid2[s]=items[x].ser2;
					addid3[s]=items[x].ser3;
					addid4[s]=items[x].ser4;
					if(sec) 
					{
						Gumps->Menu(s, items[x].morez);
					} else 
					{
						Gumps->Menu(s, items[x].morex);
					}
					return;
				case 206: // Gate Switch by WIG based on Chaos&Order gates
					// These still need to be rethought out.  Doing an itemcount loop can be VERY expensive
					for( j = 0; j < itemcount; j++ ) 
					{
						if( items[j].type == 205 && items[j].morey == items[x].morey )
						{
							if( items[j].morez == 1)
							{
								items[j].morez = 2;
								items[j].z = items[j].z + 17;
								RefreshItem( j );
							}
							else if( items[j].morez == 2)
							{
								items[j].morez = 1;
								items[j].z = items[j].z - 17;
								RefreshItem( j );
							}
						}
					}
					return;
				default:
					if (itype) printf("Unhandled item type for item: %s of type: %d\n",items[x].name,items[x].type);
					break; // case (itype!) 
				}// switch (itype)
				// END Check items by type
				
				// Begin checking objects by ID strings
				switch (itemids)
				{
				case 0x0FA9://dye
					dyeall[s]=0;
					target(s, 0, 1, 0, 31, "Which dye vat will you use this on?");
					return;//dye
				case 0x0FAB://dye vat
					addid1[s]=items[x].color1;
					addid2[s]=items[x].color2;
					target(s, 0, 1, 0, 32, "Select the clothing to use this on.");
					return;//dye vat
				case 0x14F0://houses
					if((items[x].type!=103)&&(items[x].type!=202))
					{  //experimental house code
						chars[currchar[s]].making=x;
						chars[currchar[s]].fx1=x; //for deleting it later
						addid3[s]=items[x].morex;
						buildhouse(s,items[x].morex);
						//target(s,0,1,0,207,"Select Location for house.");
					}
					return;//house deeds
				case 0x100A:
				case 0x100B:// archery butte
					Skills->AButte(s,x);
					return;// archery butte
				case 0x0E9C: // drum
					if (Skills->CheckSkill(currchar[s], MUSICIANSHIP,0,1000)) soundeffect2(currchar[s], 0x00, 0x38);
					else soundeffect2(currchar[s], 0x00, 0x39);
					return;
				case 0x0E9D: // tambourine
				case 0x0E9E:
					if (Skills->CheckSkill(currchar[s], MUSICIANSHIP,0,1000)) soundeffect2(currchar[s], 0x00, 0x52);
					else soundeffect2(currchar[s], 0x00, 0x53);
					return;
				case 0x0EB1: // standing harp
				case 0x0EB2: // lap harp
					if (Skills->CheckSkill(currchar[s], MUSICIANSHIP,0,1000)) soundeffect2(currchar[s], 0x00, 0x45);
					else soundeffect2(currchar[s], 0x00, 0x46);
					return;
				case 0x0EB3: // lute
				case 0x0EB4: // lute
					if (Skills->CheckSkill(currchar[s], MUSICIANSHIP,0,1000)) soundeffect2(currchar[s], 0x00, 0x4C);
					else soundeffect2(currchar[s], 0x00, 0x4D);
					return;
					/* -- AXES -- */
				case 0x0F43:// hatchet
				case 0x0F44:// hatchet
				case 0x0F45:// executioner's axe
				case 0x0F46:// executioner's axe
				case 0x0F47:// battle axe
				case 0x0F48:// battle axe
				case 0x0F49:// axe
				case 0x0F4A:// axe
				case 0x0F4B:// double axe
				case 0x0F4C:// double axe
				case 0x0F4D:// bardiche
				case 0x0F4E:// bardiche
				case 0x13AF:// war axe
				case 0x13B0:// war axe
				case 0x13FA:// large battle axe
				case 0x13FB:// large battle axe
				case 0x1442:// two handed axe
				case 0x1443:// two handed axe
				case 0x143E:// halberd
				case 0x143F:// halberd
					target(s, 0, 1, 0, 76, "What would you like to use that on ?");
					return;
					/* -- SMITHY -- */
				case 0x102A: // hammer
				case 0x102B: // hammer
				case 0x0FB4:// sledge hammer
				case 0x0FB5:// sledge hammer
				case 0x0FBB:// tongs
				case 0x0FBC:// tongs
				case 0x13E3:// smith's hammer
				case 0x13E4:// smith's hammer
					target(s, 0, 1, 0, 50, "Select material to use.");
					return; 
					/* -- CARPENTRY -- */
				case 0x1026: // chisels
				case 0x1027: // chisels
				case 0x1028:// Dove Tail Saw
				case 0x1029:// Dove Tail Saw
				case 0x102C:// Moulding Planes
				case 0x102D:// moulding planes
				case 0x102E: // nails - carp. tool but not sure if this works here?
				case 0x102F: // nails - carp. tool but not sure if this works here?
				case 0x1030:// jointing plane
				case 0x1031:// jointing plane
				case 0x1032:// smoothing plane
				case 0x1033:// smoothing plane
				case 0x1034:	// saw
				case 0x1035: // saw
				case 0x10E4: // draw knife
				case 0x10E5: // froe - carp. tool but not sure if this works here?
				case 0x10E6: // inshave - carp. tool but not sure if this works here?
				case 0x10E7: // scorp - carp. tool but not sure if this works here?
					target(s, 0, 1, 0, 134, "Select material to use.");
					return;
					/* -- MINING -- */
				case 0x0E85:// pickaxe
				case 0x0E86:// pickaxe
				case 0x0F39:// shovel
				case 0x0F3A:// shovel
					target(s, 0, 1, 0, 51, "Where do you want to dig?");
					return; 
				case 0x0E24: // empty vial
					p = packitem( currchar[s] );
					if( p != -1 )
					{
						if( items[x].contserial == items[p].serial )
						{
							addx[s] = x; // save the vials number, LB
							target( s, 0, 1, 0, 186, "What do you want to fill the vial with?" );
						}
						else 
							sysmessage( s, "The vial is not in your pack" );
					}
					return;
				case 0x0DF9: // cotton to thread 
					chars[currchar[s]].tailitem=x;   
					target(s, 0, 1, 0, 166, "Select spinning wheel to spin cotton.");
					return;
				case 0x09CC: // raw fish       -- krazyglue: added support for cooking raw fish
				case 0x09CD: // raw fish          should they have to fillet them first then
				case 0x09CE: // raw fish          cook the fillet pieces?  or is this good enough?
				case 0x09CF: // raw fish
					chars[currchar[s]].tailitem = x;
					target( s, 0, 1, 0, 168, "Select where to cook fish on." );
					return;
				case 0x09F1: // 
					chars[currchar[s]].tailitem=x;   
					target(s, 0, 1, 0, 168, "Select where to cook meat on.");
					return;
				case 0x0FA0:
				case 0x0FA1: // thread to cloth
				case 0x0E1D:
				case 0xE11E:  // yarn to cloth
					chars[currchar[s]].tailitem=x;
					target(s, 0, 1, 0, 165, "Select loom to make your cloth");
					return;
				case 0x14ED: // Build cannon
					target(s, 0, 1, 0, 171, "Build this Monster!");
					Items->DeleItem(x);
					return;
				case 0x1BD1:
				case 0x1BD2:
				case 0x1BD3:
				case 0x1BD4:
				case 0x1BD5:
				case 0x1BD6:	// make shafts
					itemmake[s].materialid1=items[x].id1;
					itemmake[s].materialid2=items[x].id2;
					target(s, 0, 1, 0, 172, "What would you like to use this with?"); 
					return;
				case 0x0E73: // cannon ball
					target(s, 0, 1, 0, 170, "Select cannon to load."); 
					Items->DeleItem(x);    
				case 0x0FF8:
				case 0x0FF9: // pitcher of water to flour
					chars[currchar[s]].tailitem=x;
					target(s, 0, 1, 0, 173, "Select flour to pour this on.");  
					return;
				case 0x09C0:
				case 0x09C1: // sausages to dough
					chars[currchar[s]].tailitem=x;
					target(s, 0, 1, 0, 174, "Select dough to put this on.");  
					return;
				case 0x0DF8: // wool to yarn 
					chars[currchar[s]].tailitem=x;   
					target(s, 0, 1, 0, 164, "Select your spin wheel to spin wool.");      
					return;
				case 0x0F9D: // sewing kit for tailoring
					target(s, 0, 1, 0, 167, "Select material to use.");
					return;
				case 0x19B7:
				case 0x19B9:
				case 0x19BA:
				case 0x19B8: // smelt ore
					chars[currchar[s]].smeltitem=x;
					target(s, 0, 1, 0, 52, "Select forge to smelt ore on.");// smelting  for all ore changed by Myth 11/12/98
					return;
				case 0x1E5E:
				case 0x1E5F: // Message board opening
					if( iteminrange( s, x, 2 ) )
						MsgBoardEvent(s);
					else
						sysmessage( s, "You are too far away" );
					return;
				case 0x0EC2: // cleaver
				case 0x0EC3: // cleaver
				case 0x0EC4: // skinning knife
				case 0x0EC5:// skinning knife
				case 0x0F51: // dagger
				case 0x0F52:// dagger
				case 0x0F5E:// broad sword
				case 0x0F5F:// broad sword
				case 0x0F60:// long sword #1
				case 0x0F61:// long sword #1
				case 0x13B5:// scimitar
				case 0x13B6:// scimitar
				case 0x13B7:// long sword #2
				case 0x13B8:// long sword #2
				case 0x13B9:// viking sword
				case 0x13BA:// viking sword
				case 0x13FE:// katana
				case 0x13FF:// katana
				case 0x1400:// kryss
				case 0x1401:// kryss
				case 0x1440:// cutlass
				case 0x1441:// cutlass
					target(s, 0, 1, 0, 86, "What would you like to use that on ?");
					return;
				case 0x0DE1:
				case 0x0DE2: // camping
					if(Skills->CheckSkill(currchar[s],CAMPING, 0, 500)) // Morrolan TODO: insert logout code for campfires here
					{
						c = Items->SpawnItem(s,1,"#",0,0x0D,0xE3,0,0,0,0);
						if( c == -1 )
							return;
						items[c].type=45;
						items[c].dir=2;
						if (items[x].cont1==255 && items[x].cont2==255 && items[x].cont3==255 && items[x].cont4==255)
						{
							items[c].x=items[x].x;
							items[c].y=items[x].y;
							items[c].z=items[x].z;
						}
						else
						{
							items[c].x=chars[currchar[s]].x;
							items[c].y=chars[currchar[s]].y;
							items[c].z=chars[currchar[s]].z;
						}
						mapRegions->RemoveItem( c );
						mapRegions->AddItem( c );
						items[c].priv |= 1;
						items[c].decaytime = (unsigned int)( uiCurrentTime + ( server_data.decaytimer * CLOCKS_PER_SEC ) );
						RefreshItem( c );
						Items->DeleItem( x );
					}
					else
					{
						sysmessage(s, "You fail to light a fire.");
					}
					return; // camping
				case 0x1508: // magic statue?
					if(Skills->CheckSkill(currchar[s],ITEMID, 0, 10))
					{
						items[x].id1=0x15;
						items[x].id2=0x09;
						items[x].type=45;
						//							for (k=0;k<now;k++) if (perm[k]) senditem(k,x);
						RefreshItem( x ); // AntiChrist
					}
					else
					{
						sysmessage(s, "You failed to use this statue.");
					}
					return;
				case 0x1509:
					if(Skills->CheckSkill(currchar[s],ITEMID, 0, 10))
					{
						items[x].id1=0x15;
						items[x].id2=0x08;
						items[x].type=45;
						//							for (k=0;k<now;k++) if (perm[k]) senditem(k,x);
						RefreshItem( x ); // AntiChrist
					}
					else
					{
						sysmessage(s, "You failed to use this statue.");
					}
					return;
				case 0x1230:
				case 0x1246: // guillotines?
					if(Skills->CheckSkill(currchar[s],ITEMID, 0, 10))
					{
						items[x].id1=0x12;
						items[x].id2=0x45;
						items[x].type=45;
						//							for (k=0;k<now;k++) if (perm[k]) senditem(k,x);
						RefreshItem( x ); // AntiChrist
					}
					else
					{
						sysmessage(s, "You failed to use this.");
					}  
					return;
				case 0x1245: // Guillotine stop animation
					if(Skills->CheckSkill(currchar[s],ITEMID, 0, 10))
					{
						items[x].id1=0x12;
						items[x].id2=0x30;
						items[x].type=45;
						//							for (k=0;k<now;k++) if (perm[k]) senditem(k,x);
						RefreshItem( x ); // AntiChrist
					}
					else
					{
						sysmessage(s, "You failed to use this.");
					}
					return;
				case 0x1039:  // closed flour sack
					if(Skills->CheckSkill(currchar[s],ITEMID, 0, 10))
					{
						items[x].id1=0x10;
						items[x].id2=0x3A;
						items[x].type=45;
						//							for (k=0;k<now;k++) if (perm[k]) senditem(k,x);
						RefreshItem( x ); // AntiChrist
					}
					else
					{
						sysmessage(s, "You failed to use this.");
					}
					return;
				case 0x103A: // open flour sack
					if(Skills->CheckSkill(currchar[s],ITEMID, 0, 10))
					{
						items[x].id1=0x10;
						items[x].id2=0x39;
						items[x].type=45;
						//							for (k=0;k<now;k++) if (perm[k]) senditem(k,x);
						RefreshItem( x ); // AntiChrist
					}
					else
					{
						sysmessage(s, "You failed to use this.");
					}
					return;
				case 0x0DBF:	// fishing pole
				case 0x0DC0:	// fishing pole
				case 0x0DC8:	// fishing nets
				case 0x0DC9:
				case 0x0DCA:
				case 0x0DCB:
				case 0x0DD0:
				case 0x0DD1:
				case 0x0DD2:
				case 0x0DD3:
				case 0x0DD4:
				case 0x0DD5:
				case 0x1EA3:	// nets
				case 0x1EA4:
				case 0x1EA5:
				case 0x1EA6:
					if( chars[currchar[s]].fishingtimer )
						sysmessage( s, "You are already fishing" );
					else
						target(s, 0, 1, 0, 45, "Fish where?");
					return;
				case 0x104B: // clock
				case 0x104C: // clock
				case 0x1086: // bracelet
					telltime(s);
					return;
				case 0x0E9B: // Mortar for Alchemy
					if (items[x].type==17)
					{
						addid1[s]=items[x].ser1;
						addid2[s]=items[x].ser2;
						addid3[s]=items[x].ser3;
						addid4[s]=items[x].ser4;
						target(s, 0, 1, 0, 109, "Where is an empty bottle for your potion?");
					}
					else
					{
						addid1[s]=items[x].ser1;
						addid2[s]=items[x].ser2;
						addid3[s]=items[x].ser3;
						addid4[s]=items[x].ser4;
						target(s, 0, 1, 0, 108, "What do you wish to grind with your mortar and pestle?");
					}
					return; // alchemy
				case 0x0F9E:
				case 0x0F9F: // scissors
					target(s, 0, 1, 0, 128, "What cloth should I use these scissors on?");
					return;
				case 0x0E21: // healing
					addmitem[s] = x;
					target(s, 0, 1, 0, 130, "Who will you use the bandages on?");
					chars[currchar[s]].skilldelay = (uiCurrentTime + ( server_data.skilldelay*CLOCKS_PER_SEC ));
					return;
				case 0x1057:
				case 0x1058: // sextants
					sprintf(temp, "You are at: %i East, %i South",chars[currchar[s]].x,chars[currchar[s]].y);
					sysmessage(s, temp);
					return;
				case 0x0E27:
					addid1[s] = items[x].ser1;
					addid2[s] = items[x].ser2;
					addid3[s] = items[x].ser3;
					addid4[s] = items[x].ser4;
					Gumps->Menu( s, 6 );
					return;
				case 0x0EFF:   // Hair Dye
					usehairdye(s, x);
					return;
				case 0x14FB:
				case 0x14FC:
				case 0x14FD:
				case 0x14FE: // lockpicks
					addmitem[s]=x;
					target(s, 0, 1, 0, 162, "What lock would you like to pick?");
					return;
				case 0x0C4F:
				case 0x0C50:
				case 0x0C51:
				case 0x0C52:
				case 0x0C53:
				case 0x0C54: // cotton plants
					if (!chars[s].onhorse) action(s,0x0D);
					else action(s,0x1d);
					soundeffect(s,0x01,0x3E);
					c = Items->SpawnItem(s,1,"#",1,0x0D,0xF9,0,0,1,1);
					if( c == -1 )
						return;
//					setserial(c,packitem(currchar[s]), 1);
					items[chars[currchar[s]].tailitem].priv |= 0x01;
					sysmessage(s, "You reach down and pick some cotton.");
					return; // cotton
				case 0x105B:
				case 0x105C:
				case 0x1053:
				case 0x1054: // tinker axle
					addid1[s]=items[x].ser1;
					addid2[s]=items[x].ser2;
					addid3[s]=items[x].ser3;
					addid4[s]=items[x].ser4;
					target(s, 0, 1, 0, 183, "Select part to combine the it with.");
					//					Items->DeleItem(x);	
					return;
				case 0x1051:
				case 0x1052:
				case 0x1055:
				case 0x1056:
				case 0x105D:
				case 0x105E:
					itemmake[s].materialid1=items[x].id1;
					itemmake[s].materialid2=items[x].id2;
					target(s, 0, 1, 0, 184, "Select part to combine it with.");
					return;
				case 0x104F:
				case 0x1050:
				case 0x104D:
				case 0x104E:// tinker clock
					target(s, 0, 1, 0, 185, "select clock frame to combine with");
					Items->DeleItem(x);
					return;
				case 0x1059:
				case 0x105A://tinker sextant
					if(Skills->CheckSkill(currchar[s],TINKERING, 0, 1000))
					{
						sysmessage(s, "You create the sextant.");
						strcpy(temp,"a sextant");
						c = Items->SpawnItem(s,1,temp,0,0x10,0x57,0,0,1,1);
						if( c == -1 )
							return;
//						setserial(c,packitem(currchar[s]), 1);
						items[chars[currchar[s]].tailitem].priv |= 0x01;
						Items->DeleItem(x);
					}
					else sysmessage(s, "you fail to create the sextant.");
					return;
				case 0x1070:
				case 0x1074: // training dummies
					if (iteminrange(s,x,1)) Skills->TDummy(s);
					else sysmessage(s, "You need to be closer to use that.");
					return;
				case 0x1071:
				case 0x1073:
				case 0x1075:
				case 0x1077:// swinging training dummy
					sysmessage(s, "You must wait for it to stop swinging !");
					return;// swinging training dummy
				case 0x1EBC: // tinker's tools
					target(s, 0, 1, 0, 180, "Select material to use.");
					return;
				case 0x0FAF:
				case 0x0FB0:
					target( s, 0, 1, 0, 49, "What item would you like to repair?" );
					return;

				default:
					//	printf("Unhandled item id for item: %s with id: %X.\n",items[x].name, itemids); //check for unused items - Morrolan
					break;
				}//switch (itemids)
				// END Check items by ID
				
				//	SCROLLS - there are a LOT of choices here.. I'll come back to this - Morrolan
				if (items[x].id1==0x1F && (items[x].id2>0x2C && items[x].id2<0x6D))
				{
					int success=0;
					currentSpellType[s] = 1; // set the spell type to cast ( a scroll spell, so cut mana req )
					if (items[x].id2==0x2D)  // Reactive Armor spell scrolls
					{
						success=Magic->newSelectSpell2Cast( s, 7 ); 
					}
					if ((items[x].id2>=0x2E)&&(items[x].id2<=0x34))  // first circle spell scrolls
					{
						success=Magic->newSelectSpell2Cast( s, items[x].id2-0x2D ); 
					}
					else if ((items[x].id2>=0x35)&&(items[x].id2<=0x6C))  // 2 to 8 circle spell scrolls
					{
						success=Magic->newSelectSpell2Cast( s, items[x].id2-0x2D+1 ); 
					}
					
					// remove scrolls if spell cast Tauriel
					if (success)
					{
						if (items[x].amount!=1)
						{
							items[x].amount--;
							RefreshItem( x ); // AntiChrist
						}
						else
						{
							Items->DeleItem(x);
						}
					}
					return;
				}// scrolls
				sysmessage(s, "You can't think of a way to use that item.");
	}// if x!=-1*/
}// dbl-click

void singleclick(int s)
{
	int i, j, serial;
	unsigned char a1, a2, a3, a4;
	char temp2[100];
	tile_st tile;
	char mode, ok, used;
	
	a1 = buffer[s][1];
	a2 = buffer[s][2];
	a3 = buffer[s][3];
	a4 = buffer[s][4];
	serial = calcserial( a1, a2, a3, a4 );
	// Begin chars/npcs section
	i=findbyserial(&charsp[serial%HASHMAX],serial,1);
	if(i!=-1)		
	{
		showcname(s, i, 0);
		return;
	}
	//End chars/npcs section
	//Begin items section
	i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	if (i==-1)
	{
		printf("UOX3.CPP: singleclick couldn't find item serial: %d\n", serial);
		return;
	}
	else if (chars[currchar[s]].priv&8)
	{
		sprintf(temp, "%i %s [%x %x %x %x]", items[i].amount, items[i].name, a1, a2, a3, a4);
		itemmessage(s, temp, a1, a2, a3, a4);
	}
	else
	{
		if (items[i].contserial!=-1 && items[i].cont1>=0x40)
		{
			j=GetPackOwner(findbyserial(&itemsp[items[i].contserial%HASHMAX],items[i].contserial,0));
			if (j!=-1)
				if (chars[j].npcaitype==17)
				{
					if( strlen( items[i].creator ) > 0 && items[i].madewith > 0 ) sprintf( temp2, "%s %s by %s", items[i].desc, skill[items[i].madewith-1].madeword, items[i].creator ); // Magius(CHE)
					sprintf(temp, "%s at %igp",temp2,items[i].value); // Changed by Magius(CHE)
					itemmessage(s,temp,a1,a2,a3,a4);
				}
		}
		if (items[i].name[0]!='#')
		{
			if ((!(items[i].pileable))||(items[i].amount==1))
			{
				if( strlen(items[i].creator)>0 && items[i].madewith>0) sprintf(temp2, "%s %s by %s",items[i].name,skill[items[i].madewith-1].madeword,items[i].creator); // Magius(CHE)
				else strcpy(temp2, items[i].name); // By Magius(CHE)
				itemmessage(s, temp2, a1, a2, a3, a4); // Changed by Magius(CHE)
			}
			else
			{
				sprintf(temp, "%i %ss", items[i].amount, items[i].name);
				if( strlen(items[i].creator)>0 && items[i].madewith>0) sprintf(temp2, "%s %s by %s",temp,skill[items[i].madewith-1].madeword,items[i].creator); // Magius(CHE)
				else strcpy(temp2, temp); // By Magius(CHE)
				itemmessage(s, temp2, a1, a2, a3, a4); // Changed by Magius(CHE)
			}
		}
		else
		{
			Map->SeekTile((items[i].id1<<8)+items[i].id2, &tile);
			if (items[i].amount==1)
			{
				if (tile.flag2&0x80) strcpy(temp, "an ");
				else if (tile.flag2&0x40) strcpy(temp, "a ");
				else temp[0]=0;
			}
			else sprintf(temp, "%i ", items[i].amount);
			mode=0;
			used=0;
			int namLen = strlen( temp );
			for (j=0;j<strlen((char *) tile.name);j++)
			{
				ok=0;
				if ((tile.name[j]=='%')&&(mode==0)) mode=2;
				else if ((tile.name[j]=='%')&&(mode!=0)) mode=0;
				else if ((tile.name[j]=='/')&&(mode==2)) mode=1;
				else if (mode==0) ok=1;
				else if ((mode==1)&&(items[i].amount==1)) ok=1;
				else if ((mode==2)&&(items[i].amount>1)) ok=1;
				if (ok)
				{
					//:Terrin: yeah, using temp2 will fix it, but lets
					//         do it they way I did in uox3.cpp
					// (which by the way is not as fast as a pointer, but
					// I didn't want to confuse this issue)
					//						strcpy(temp2,temp);
					//						sprintf(temp, "%s%c", temp2, tile.name[j]);
					temp[namLen++] = tile.name[j];
					temp[namLen  ] = '\0';
					if (mode) used=1;
				}
			}
			if ((!used)&&(items[i].amount>1))
			{
				//:Terrin: a "strcat(temp, "s");" would work, but since we
				// already have the length in a variable..
				//strcpy(temp2,temp);
				//sprintf(temp, "%ss", temp2);
				temp[namLen++] = 's';
				temp[namLen  ] = '\0';
			}
			if( items[i].type == 15 )
			{
				int spellNum = ( 8 * ( items[i].morex - 1 ) ) + items[i].morey - 1;	// we pick it up from the array anyway
				if( spellNum >= 0 && spellNum <= 70 )
				{
					char buf[10];
					strcat( temp, " of " );
					strcat( temp, spellname[spellNum] );
					strcat( temp, " with " );
					sprintf( buf, "%d", items[i].morez );
					strcat( temp, buf );
					strcat( temp, " charges" );
				}
			}
			if( items[i].magic == 3 )
				itemmessage( s, "[Locked down]", a1, a2, a3, a4, 0x04, 0x81 );
			if( strlen( items[i].creator ) > 0 && items[i].madewith > 0 ) sprintf( temp2, "%s %s by %s", temp, skill[items[i].madewith-1].madeword, items[i].creator ); // Magius(CHE)
			else strcpy( temp2, temp ); // By Magius(CHE)
			itemmessage(s, temp2, a1, a2, a3, a4); // Changed by Magius(CHE)
		}
	}
}

