//------------------------------------------------------------------------
//  house.cpp
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
/* House code for deed creation by Tal Strake, revised by Cironian */

#include <uox3.h>
#include <debug.h>

#define DBGFILE "house.cpp"

void mtarget(int s, unsigned char a1, unsigned char a2, unsigned char a3, unsigned char a4, unsigned char b1, unsigned char b2, char *txt)
{
	char multitarcrs[27]="\x99\x01\x40\x01\x02\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x02\x00\x00\x00\x00\x00\x00";

	targetok[s]=1; 
	multitarcrs[2]=a1;
	multitarcrs[3]=a2;
	multitarcrs[4]=a3;
	multitarcrs[5]=a4;
	multitarcrs[18]=b1;
	multitarcrs[19]=b2;
	sysmessage(s, txt);
	Network->xSend(s, multitarcrs, 26, 0);
}

void buildhouse(int s, int i)
{
//o---------------------------------------------------------------------------o
//|   Function    -  void buildhouse(int s, int i)
//|   Date        -  UnKnown - Rewrite Date 1/24/99
//|   Programmer  -  UnKnown - Rewrite by Zippy (onlynow@earthlink.net)
//o---------------------------------------------------------------------------o
//|   Purpose     -  Triggered by double clicking a deed-> the deed's moreX is read
//|						for the house section in house.scp. Extra items can be added
//|						using HOUSE ITEM, (this includes all doors!) and locked "LOCK"
//|						Space around the house with SPACEX/Y and CHAR offset CHARX/Y/Z
//o---------------------------------------------------------------------------o
	int x,y,house,key;//where they click, and the house/key items
	signed char z;
	int sx=0,sy=0;					//space around the house needed
	int k,l,tmp;						//Temps
	int hitem[100],icount=0;//extra "house items" (up to 100)
	char sect[512];				//file reading
	char id1,id2;					//house ID
	char itemsdecay = 0;		// set to 1 to make stuff decay in houses
	static int looptimes=0;//for targeting
	long int pos;					//for files...
	int cx=0,cy=0,cz=8;		//where the char is moved to when they place the house (Inside, on the steps.. etc...)(Offset)
	int boat=0;//Boats
	int hdeed=0;//deed id #

	int norealmulti = 0;

    if(buffer[s][11]==0xFF && buffer[s][12]==0xFF && buffer[s][13]==0xFF && buffer[s][14]==0xFF) return; // do nothing if user cancels, avoids CRASH!

	hitem[0]=0;//avoid problems if there are no HOUSE_ITEMs by initializing the first one as 0
	if (i)
	{
		openscript("house.scp");
		sprintf(sect, "HOUSE %d", i);//and BTW, .find() adds SECTION on there for you....
		if (!(i_scripts[house_script]->find(sect)))
		{
			closescript();
			return;
		}
		do
		{
			read2();
			if (script1[0]!='}')
			{
				if (!(strcmp(script1,"ID")))
				{
					tmp=hstr2num(script2);
					id1 = (unsigned char)(tmp>>8);
					id2 = (unsigned char)(tmp%256);
				}
				else if (!(strcmp(script1,"SPACEX")))
				{
					sx=str2num(script2)+1;
				}
				else if (!(strcmp(script1,"SPACEY")))
				{
					sy=str2num(script2)+1;
				}
				else if (!(strcmp(script1,"CHARX")))
				{
					cx=str2num(script2);
				}
				else if (!(strcmp(script1,"CHARY")))
				{
					cy=str2num(script2);
				}
				else if (!(strcmp(script1,"CHARZ")))
				{
					cz=str2num(script2);
				}
				else if( !(strcmp(script1, "ITEMSDECAY" )))
				{
					itemsdecay = str2num( script2 );
				}
				else if (!(strcmp(script1,"HOUSE_ITEM")))
				{
					hitem[icount]=str2num(script2);
					icount++;
				}
				else if (!(strcmp(script1, "HOUSE_DEED")))
				{
					hdeed=str2num(script2);
				}
				else if (!(strcmp(script1, "BOAT"))) boat=1;//Boats
				else if (!(strcmp(script1, "NOREALMULTI"))) norealmulti = 1;
			}
		}
		while (strcmp(script1,"}"));
		closescript();
		if (!id1)
		{
			ConOut("ERROR: Bad house script # %i!\n",i);
			return;
		}
	}
	if(!looptimes)
	{
		if (i)
		{
			addid1[s]=0x40;addid2[s]=100;//Used in addtarget
			if (norealmulti) 
				target(s, 0, 1, 0, 245, "Select a place  for your structure"); 
			else
				mtarget(s, 0, 1, 0, 0, id1-0x40, id2, "Select location for building.");
		}
		else
		{
			mtarget(s, 0, 1, 0, 0, addid1[s]-0x40, addid2[s], "Select location for building.");
		}
		looptimes++;//for when we come back after they target something
		return;
	}
	if(looptimes)
	{
		looptimes = 0;
		x = (buffer[s][11]<<8) + buffer[s][12];//where they targeted
		y = (buffer[s][13]<<8) + buffer[s][14];
		z = buffer[s][16] + Map->TileHeight( (buffer[s][17]<<8) + buffer[s][18] );

		if( !(chars[currchar[s]].priv&0x01 ) )
		{
			if( ( x < 200 && y < 200 ) || ( x > 6144 || y > 4096 ) )
			{
				sysmessage(s, "You cannot build your house there!");
				return;
			}


			for (k=-sx;k<sx;k++)//check the SPACEX and SPACEY to make sure they are valid locations....
			{
				for (l=-sy;l<sy;l++)
				{
	//				if ((!validNPCMove(x+k,y+l,z,currchar[s]))&&
					if(( !Movement->validNPCMove( x+k, y+l, z, currchar[s] ) ) &&
	//				signed char myz;
	//				if( ( !Movement->CanCharWalk( currchar[s], x+k, y+l, myz ) ) &&
						((chars[currchar[s]].x!=x+k)&&(chars[currchar[s]].y!=y+l)))
					/*This will take the char making the house out of the space check, be careful 
					you don't build a house on top of your self..... this had to be done So you 
					could extra space around houses, (12+) and they would still be buildable.*/
					{
						sysmessage(s, "You cannot build your house there.");
						return;
						//ConOut("Invalid %i,%i [%i,%i]\n",k,l,x+k,y+l);
					} //else ConOut("DEBUG: Valid at %i,%i [%i,%i]\n",k,l,x+k,y+l);
				}
			}
		}
		//Boats ->
		if(id2>=18) sprintf(temp,"%s's house",chars[currchar[s]].name);//This will make the little deed item you see when you have showhs on say the person's name, thought it might be helpful for GMs.
		else strcpy(temp, "a mast");
		//--^
		house = Items->SpawnItem(s,currchar[s], 1,temp,0,id1,id2,0,0,0,0);
		if( house == -1 )
			return;

		chars[currchar[s]].making=0;

		
		items[house].x=x;
		items[house].y=y;
		items[house].z=z;
		items[house].priv=0;
		items[house].more4 = itemsdecay; // set to 1 to make items in houses decay
		items[house].morex=hdeed; // crackerjack 8/9/99 - for converting back *into* deeds
		items[house].ownserial=chars[currchar[s]].serial;
		items[house].owner1=chars[currchar[s]].ser1;
		items[house].owner2=chars[currchar[s]].ser2;
		items[house].owner3=chars[currchar[s]].ser3;
		items[house].owner4=chars[currchar[s]].ser4;
		if (items[house].contserial==-1) mapRegions->AddItem(house); //add to mapRegions  
		if (!hitem[0] && !boat)
		{
			teleport(currchar[s]);
			all_items(s);
			return;//If there's no extra items, we don't really need a key, or anything else do we? ;-)
		}

		if(boat) //Boats
		{
			if(!Boats->Build(s,house, id2))
			{
				Items->DeleItem(house);
				return;
			} 
		}

		if (i)//Boats->.. Moved from up there ^
			Items->DeleItem(chars[currchar[s]].fx1); // this will del the deed no matter where it is

        chars[currchar[s]].fx1=-1; //reset fx1 so it does not interfere
		                            // bugfix LB ... was too early reseted 

		//Key...
		
		if (id2>=112&&id2<=115) key=Items->SpawnItem(s, currchar[s], 1, "a tent key", 0, 0x10, 0x10,0, 0,1,1);//iron key for tents
		else if(id2<=0x18) key=Items->SpawnItem(s,currchar[s],1,"a ship key",0,0x10,0x13,0,0,1,1);//Boats -Rusty Iron Key
		else key=Items->SpawnItem(s, currchar[s], 1, "a house key", 0, 0x10, 0x0F, 0, 0,1,1);//gold key for everything else

		if( key != -1 )
		{
			items[key].more1=items[house].ser1;//use the house's serial for the more on the key to keep it unique
			items[key].more2=items[house].ser2;
			items[key].more3=items[house].ser3;
			items[key].more4=items[house].ser4;
			items[key].type=7;
		}

		for (k=0;k<icount;k++)//Loop through the HOUSE_ITEMs
		{
			openscript("house.scp");
			sprintf(sect,"HOUSE ITEM %i",hitem[k]);
			if (!i_scripts[house_script]->find(sect))
			{
				closescript();
			}
			else
			{
				do 
				{
					read2();
					if (script1[0]!='}')
					{
						if (!(strcmp(script1,"ITEM")))
						{
							pos=ftell(scpfile);// To prevent accidental exit of loop.
							closescript();
							l=Items->CreateScriptItem(s,str2num(script2),0);//This opens the item script... so we gotta keep track of where we are with the other script.
							openscript("house.scp");
							fseek(scpfile, pos, SEEK_SET);
							strcpy(script1, "ITEM");
							items[l].magic=2;//Non-Movebale by default
							items[l].priv=0;//since even things in houses decay, no-decay by default
							items[l].x=x;
							items[l].y=y;
							items[l].z=z;
							items[l].ownserial=chars[currchar[s]].serial;
							items[l].owner1=chars[currchar[s]].ser1;
							items[l].owner2=chars[currchar[s]].ser2;
							items[l].owner3=chars[currchar[s]].ser3;
							items[l].owner4=chars[currchar[s]].ser4;
							if (!norealmulti)
								setserial( l, house, 7 );
							//ConOut("[%i,%i,%i] Item %i\n",items[l].x,items[l].y,items[l].z,l);
						}
						if (!(strcmp(script1,"DECAY")))
						{
							items[l].priv=items[l].priv|0x01;
						}
						if (!(strcmp(script1,"NODECAY")))
						{
							items[l].priv=0;
						}
						if (!(strcmp(script1,"PACK")))//put the item in the Builder's Backpack
						{
							items[l].cont1=items[packitem(currchar[s])].ser1;
							items[l].cont2=items[packitem(currchar[s])].ser2;
							items[l].cont3=items[packitem(currchar[s])].ser3;
							items[l].cont4=items[packitem(currchar[s])].ser4;
							items[l].contserial=items[packitem(currchar[s])].serial;
							items[l].x=rand()%90+31;
							items[l].y=rand()%90+31;
							items[l].z=9;
						}
						if (!(strcmp(script1,"MOVEABLE")))
						{
							items[l].magic=1;
						}
						if (!(strcmp(script1,"LOCK")))//lock it with the house key
						{
							items[l].more1=items[house].ser1;
							items[l].more2=items[house].ser2;
							items[l].more3=items[house].ser3;
							items[l].more4=items[house].ser4;
						}
						if (!(strcmp(script1,"X")))//offset + or - from the center of the house:
						{
							items[l].x=x+str2num(script2);
						}
						if (!(strcmp(script1,"Y")))
						{ 
							items[l].y=y+str2num(script2);
						}
						if (!(strcmp(script1,"Z")))
						{
							items[l].z=z+str2num(script2);
						}
					}
				}
				while (strcmp(script1,"}"));
				if (items[l].contserial==-1) mapRegions->AddItem(l);  //add to mapRegions
				closescript();
			}
		}
		all_items(s);//make sure they have all the items Sent....
		if (!norealmulti)
		{
			chars[currchar[s]].x=x+cx; //move char inside house
			chars[currchar[s]].y=y+cy;
			chars[currchar[s]].dispz=chars[currchar[s]].z=z+cz;
			teleport(currchar[s]);
		} else {
			Items->DeleItem(house);
		}
	}
}

// turn a house into a deed if possible. - crackerjack 8/9/99
// s = socket of player
// i = house item # in items[]
// morex on the house item must be set to deed item # in items.scp.
void deedhouse(int s, int i) // Ripper & AB
{
	int ii;
	int x1, y1, x2, y2;
	unsigned char ser1, ser2, ser3, ser4;
	int pvDeed, playerCont, keyCount = 0;
	if( i == -1 ) return;
	playerCont = packitem( currchar[s] );
	int mapitemptr,mapitem,mapchar,a,checkgrid,increment,StartGrid,getcell,ab, index;
	if(items[i].ownserial==chars[currchar[s]].serial)
	{
		Map->MultiArea(i, &x1,&y1,&x2,&y2);

		ii=Items->SpawnItemBackpack2(s, currchar[s], items[i].morex, 0);	// need to make before delete
		if( ii == -1 ) return;
        sprintf(temp, "Demolishing House %s", items[i].name);
		sysmessage( s, temp );
		ser1 = items[i].ser1;
		ser2 = items[i].ser2;
		ser3 = items[i].ser3;
		ser4 = items[i].ser4;
		Items->DeleItem(i);
		sprintf(temp, "Converted into a %s.", items[ii].name);
		sysmessage(s, temp);
		// door/sign delete
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
						index=mapchar;			
						if( chars[index].x >= x1 && chars[index].y >= y1 && chars[index].x <= x2 && chars[index].y <= y2 )
						{
							if( chars[index].npcaitype == 17 ) // player vendor in right place
							{
								sprintf( temp, "A vendor deed for %s", chars[index].name );
								pvDeed = Items->SpawnItem( s, currchar[s], 1, temp, 0, 0x14, 0xF0, 0, 0, 1, 0 );
								if( pvDeed != -1 )
								{
									items[pvDeed].type = 217;
									items[pvDeed].value = 2000;
									RefreshItem( pvDeed );
								}
								sprintf(temp, "Packed up vendor %s.", chars[index].name);
								Npcs->DeleteChar( index );
								sysmessage(s, temp);
							}
						}
					}
					else if( mapitem != -1 )
					{
						if( items[mapitem].x >= x1 && items[mapitem].y >= y1 && items[mapitem].x <= x2 && items[mapitem].y <= y2 )
						{
							if( items[mapitem].type == 203 || items[mapitem].type == 12 || items[mapitem].type == 13 )
							{
								Items->DeleItem( mapitem );
							}
						}
					}
				} while( mapitemptr != -1 );	// keep going to make sure we got all the vendors
			}
		}

		sysmessage( s, "Removing excess keys from player's pack" );
		for( a = 0; a < itemcount; a++ )		// we HAVE to have an itemcount, because when you replace the house, the serial will be different
		{										// so we have to remove all keys that used that serial
			if( items[a].more1 == ser1 && items[a].more2 == ser2 &&
				items[a].more3 == ser3 && items[a].more4 == ser4 )
			{
				Items->DeleItem( a );
				keyCount++;
			}
		}
		sysmessage( s, "You deleted %i keys", keyCount );
		return;
	}
}
void addthere(int s, int xx, int yy, int zz, int t)
{
	int c;
	//ConOut("addthere!x=%d y=%d z=%d\n",xx,yy,zz);
	tile_st tile;

	assert(s != -1);
	c = Items->SpawnItem(s,currchar[s], 1,"#",0,addid1[s],addid2[s],0,0,0,0);
	if( c == -1 ) return;
	items[c].x=xx;
	items[c].y=yy;
	items[c].z=zz;
	items[c].amount=1;
	items[c].doordir=0;
	items[c].type=t;
	if (items[c].contserial==-1) mapRegions->AddItem(c); //Add to mapRegions
	Map->SeekTile((items[c].id1<<8)+items[c].id2, &tile);
	if(chars[currchar[s]].making==999) chars[currchar[s]].making=c; // store item #
	if(tile.flag2&0x08) items[c].pileable=1;
//	for (j=0;j<now;j++) if (perm[j]) senditem(j,c);
	RefreshItem( c );
	addid1[s]=0;
	addid2[s]=0;
}

void killkeys(unsigned char s1, unsigned char s2, unsigned char s3,
		unsigned char s4) // Crackerjack 8/11/99
// This function is rather CPU-expensive, but AFAIK there is no
// better way to find all keys than to do it this way.. :/
{
	int i;
	for(i=0;i<itemcount;i++) {
		if(items[i].type==7&&items[i].more1==s1&&
			items[i].more2==s2&&items[i].more3==s3&&
			items[i].more4==s4)
		{ // make key uselss
			items[i].more1=0;
			items[i].more2=0;
			items[i].more3=0;
			items[i].more4=0;
		}
	}
	return;
}

// Crackerjack 8/12/99 - House List Functions

// Checks if somebody is on the house list.
// on_hlist(int h (items[] index for house), unsigned char s1, s2, s3, s4 (char serial),
//		int *li (pointer to variable to put items[] index of list item in or NULL))
// Returns:
// 0 - Character is not on house list
// Anything else - Character is on house list, type # is returned.
int on_hlist(int h, unsigned char s1, unsigned char s2, unsigned char s3, unsigned char s4, int *li)
{
	
	   int ci=-1;

	   int  StartGrid=mapRegions->StartGrid(items[h].x,items[h].y);
	   unsigned int increment=0;
	   for (unsigned int checkgrid=StartGrid+(increment*mapRegions->GetColSize());increment<3;increment++, checkgrid=StartGrid+(increment*mapRegions->GetColSize()))
	   {
		for (int a=0;a<3;a++)
		{
			int mapitemptr=-1;
			int mapitem=-1;
			int mapchar=-1;
			do //check all items in this cell
			{
				mapchar=-1;
				mapitemptr=mapRegions->GetNextItem(checkgrid+a, mapitemptr);
				if (mapitemptr==-1) break;
				mapitem=mapRegions->GetItem(checkgrid+a, mapitemptr);
				if(mapitem>999999) mapchar=mapitem-1000000;
				if (mapitem!=-1 && mapitem<1000000)
				{
				   ci=mapitem;
				   	if((items[ci].morey==items[h].serial)&&
				       (items[ci].more1==s1)&&(items[ci].more2==s2)&&
				       (items[ci].more3==s3)&&(items[ci].more4==s4)) 
					{
				       if(li!=NULL) *li=ci;
				       return items[ci].morex;
					}		          
				}								
					
		} while (mapitem!=-1);
	   }
	  } // end of mapregions loop
	
	return 0;

}

// Adds somebody to a house list.
// add_hlist(int c (chars[] index), int h (items[] index for house), int t (list type))
// Returns:
// 1 - Successful addition to house list
// 2 - Character is already on a house list
// 3 - Character is not on property
int add_hlist(int c, int h, int t)
{
	int sx, sy, ex, ey, i;

	if(on_hlist(h, chars[c].ser1, chars[c].ser2, chars[c].ser3, chars[c].ser4, NULL))
		return 2;

	Map->MultiArea(h, &sx,&sy,&ex,&ey);
	// Make an object with the character's serial & the list type
	// and put it "inside" the house item.
	if(chars[c].x>=sx&&chars[c].y>=sy&&chars[c].x<=ex&&chars[c].y<=ey) {
		i=Items->MemItemFree();	

		Items->InitItem(i);

		items[i].morex=t;
		items[i].more1=chars[c].ser1;
		items[i].more2=chars[c].ser2;
		items[i].more3=chars[c].ser3;
		items[i].more4=chars[c].ser4;
		items[i].morey=items[h].serial;

		/*items[i].cont1=items[h].ser1; // bugfix, LB stuff in container doesnt get inserted in mapregions !!!
		items[i].cont2=items[h].ser2;
		items[i].cont3=items[h].ser3;
		items[i].cont4=items[h].ser4;*/

		items[i].priv=0; // no decay !!
		items[i].visible=0;
		strcpy(items[i].name,"friend of house");


		items[i].x=items[h].x;
		items[i].y=items[h].y;
		items[i].z=items[h].z;

		mapRegions->AddItem(i);
		return 1;
	}

	return 3;
}

// Removes somebody from a house list.
// del_hlist(int c (chars[] index), int h (items[] index for house))
// Returns:
// 0 - Player was not on a list
// # - What list the player was on if any.
int del_hlist(int c, int h)
{
	int hl, li;

	hl=on_hlist(h, chars[c].ser1, chars[c].ser2, chars[c].ser3, chars[c].ser4, &li);
	if(hl) {
		mapRegions->RemoveItem(li);
		Items->DeleItem(li);
	}
	return(hl);
}

// Handles house commands from friends of the house. - Crackerjack 8/12/99
void house_speech(int s, unsigned char *talk)
{
	int i, fr;
	char msg[512];
	if(s<0 || s>MAXCLIENT) return;
	i=findmulti(chars[currchar[s]].x,chars[currchar[s]].y,chars[currchar[s]].z);
	if(i==-1) return; // not in a house, so we don't care.
	fr=on_hlist(i, chars[currchar[s]].ser1, chars[currchar[s]].ser2,
		chars[currchar[s]].ser3, chars[currchar[s]].ser4, NULL);
	if((fr!=H_FRIEND)&&(items[i].ownserial!=chars[currchar[s]].serial))
		return; // not a friend or owner, so we don't care.
//	strcpy(msg, talk);//Capitalize the msg
//	strcpy(msg, strupr(msg));
	strcpy( msg, strupr( (char *)talk ) ); // krazyglue - above two lines aren't necessary
	if(strstr(msg, "I BAN THEE")) { // house ban
		addid1[s]=items[i].ser1;
		addid2[s]=items[i].ser2;
		addid3[s]=items[i].ser3;
		addid4[s]=items[i].ser4;
		target(s, 0, 1, 0, 229, "Select person to ban from house.");
		return;
	}
	if(strstr(msg, "REMOVE THYSELF")) { // kick out of house
		addid1[s]=items[i].ser1;
		addid2[s]=items[i].ser2;
		addid3[s]=items[i].ser3;
		addid4[s]=items[i].ser4;
		target(s, 0, 1, 0, 228, "Select person to eject from house.");
		return;
	}
	if( strstr( msg, "I WISH TO LOCK THIS DOWN" ) )
	{ // lock down item
		// TO DO:	Keep a count of how many things have been locked down and check to make sure we don't go over the limit
		addid1[s]=items[i].ser1;
		addid2[s]=items[i].ser2;
		addid3[s]=items[i].ser3;
		addid4[s]=items[i].ser4;
		target( s, 0, 1, 0, 232, "Select item to lock down." );
		return;
	}
	if( strstr( msg, "I WISH TO RELEASE THIS" ) )
	{ // lock down item
		// TO DO:	Keep a count of how many things have been locked down and check to make sure we don't go over the limit
		addid1[s]=items[i].ser1;
		addid2[s]=items[i].ser2;
		addid3[s]=items[i].ser3;
		addid4[s]=items[i].ser4;
		target( s, 0, 1, 0, 233, "Select item to release." );
		return;
	}
}
