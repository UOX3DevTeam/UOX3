//------------------------------------------------------------------------
//  boats.cpp
//
//------------------------------------------------------------------------
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 1998 - 2001 by Unknown real name (Zippy)
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
#include <boats.h>

#define XP 0
#define YP 1

//============================================================================================
//UooS Item translations - You guys are the men! :o)

//[4]=direction of ship
//[4]=Which Item (PT Plank, SB Plank, Hatch, TMan)
//[2]=Coord (x,y) offsets
signed short int iSmallShipOffsets[4][4][2]=
// X  Y  X  Y  X  Y  X  Y
{ -2, 0, 2, 0, 0,-4, 1, 4,//Dir
0,-2, 0, 2, 4, 0,-4, 0,
2, 0,-2, 0, 0, 4, 0,-4,
0, 2, 0,-2,-4, 0, 4, 0 };
//  P1    P2   Hold  Tiller
signed short int iMediumShipOffsets[4][4][2]=
// X  Y  X  Y  X  Y  X  Y
{ -2, 0, 2, 0, 0,-4, 1, 5,
0,-2, 0, 2, 4, 0,-5, 0,
2, 0,-2, 0, 0, 4, 0,-5,
0, 2, 0,-2,-4, 0, 5, 0 };
signed short int iLargeShipOffsets[4][4][2]=
// X  Y  X  Y  X  Y  X  Y
{ -2,-1, 2,-1, 0,-5, 1, 5,
1,-2, 1, 2, 5, 0,-5, 0,
2, 1,-2, 1, 0, 5, 0,-5,
-1, 2,-1,-2,-5, 0, 5, 0 };
//Ship Items
//[4] = direction
//[6] = Which Item (PT Plank Up,PT Plank Down, SB Plank Up, SB Plank Down, Hatch, TMan)
unsigned char cShipItems[4][6]=
{0xB1,0xD5,0xB2,0xD4,0xAE,0x4E,
0x8A,0x89,0x85,0x84,0x65,0x53,
0xB2,0xD4,0xB1,0xD5,0xB9,0x4B,
0x85,0x84,0x8A,0x89,0x93,0x50 };
//============================================================================================


void sendinrange(int i)//Send this item to all online people in range
{//(Decided this was better than writting 1000 for loops to send an item.
	for(unsigned int a=0;a<now;a++)
	{
		if(perm[a] && iteminrange(a,i,Races->getVisRange( chars[currchar[a]].race ) ) )
			senditem(a,i);
	}
}

int dist( int a, int b, int type )//Distance from A to B (type = 1 (a is a char) type=0 (a is an item))
{
	short xa, ya;
	if( type )
	{
		xa = chars[a].x;
		ya = chars[a].y;
	} 
	else 
	{
		xa = items[a].x;
		ya = items[a].y;
	}
	short dx = abs( xa - items[b].x );
	short dy = abs( ya - items[b].y );

	return (int)( hypot( dx, dy ) );
}

int findmulti(int x, int y, signed char z)//Sortta like getboat() only more general... use this for other multi stuff!
{
	int lastdist=30;
	int multi=-1;
	int ret,dx,dy;
	
	int	StartGrid = mapRegions->StartGrid( x, y );
	
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
				if (mapitem>-1 && mapitem<999999)
				{
					if (items[mapitem].id1>=0x40)
					{
						dx = abs( x - items[mapitem].x );
						dy = abs( y - items[mapitem].y );
						ret = (int)( hypot( dx, dy ) );
						if (ret<=lastdist)
						{
							lastdist=ret;
							if (inmulti(x,y,z,mapitem))
								multi=mapitem;
						}
					}
				}
			} while (mapitem>-1);
		}//For a
	}//For checkgrid
	return multi;
}

int inmulti(int x, int y, signed char z, int m)//see if they are in the multi at these chords (Z is NOT checked right now)
// PARAM WARNING: z is unreferenced
{
	int j;
	SI32 length;			// signed long int on Intel
	st_multi multi;
	UOXFile *mfile;
	Map->SeekMulti(((items[m].id1<<8)+items[m].id2)-0x4000, &mfile, &length);
	length=length/sizeof(st_multi);
	if (length == -1 || length>=17000000)//Too big...
	{
		printf("inmulti() - Bad length in multi file. Avoiding stall. (Item Name: %s %i) (Length: %d)\n", items[m].name, items[m].serial, length );
		length = 0;
	}
	for (j=0;j<length;j++)
	{
	        mfile->get_st_multi(&multi);
		/*printf("DEBUG: Multi { vis=%i - (%i,%i) } check(%i,%i,%i)   -   total(%i,%i)\n",
		multi.visible,multi.x,multi.y,x,y,z,multi.x+items[m].x,items[m].y+multi.y);*/
		if ((multi.visible)&&(items[m].x+multi.x == x) && (items[m].y+multi.y == y))
		{
			return 1;
		}
	}
	return 0;
}

cBoat::cBoat()//Consturctor
{
}

cBoat::~cBoat()//Destructor
{
}

void cBoat::PlankStuff(int s, int p)//double click Will send them here
{
	int boat=GetBoat(s),a,b,serhash=chars[currchar[s]].serial%HASHMAX;
	int boat2, mser;

	if(boat==-1)//They aren't on a boat, so put then on the plank.
	{
		mapRegions->RemoveItem(currchar[s]+1000000);
		chars[currchar[s]].x=items[p].x;
		chars[currchar[s]].y=items[p].y;
		chars[currchar[s]].z=chars[currchar[s]].dispz=items[p].z+5;
		mapRegions->AddItem(currchar[s]+1000000);

		mser = calcserial(items[p].more1, items[p].more2, items[p].more3, items[p].more4 );
		
		boat2 = calcItemFromSer( mser );

		if ( boat2 != -1 )
		{
			for(a=0;a<ownsp[serhash].max;a++)//Put all their Pets/Hirlings on the boat too
			{
				b=ownsp[serhash].pointer[a];
				if (b<cmem && b>-1)
				{
					if (chars[b].npc && chars[b].ownserial==chars[currchar[s]].serial && inrange1p(currchar[s], b)<=15)
					{
						mapRegions->RemoveItem(b+1000000);
						chars[b].x = (short int)items[boat2].x+1;
						chars[b].y = (short int)items[boat2].y+1;
						chars[b].z = chars[b].dispz=items[boat2].z+4;
						mapRegions->AddItem(b+1000000);
						setserial(b,boat2,8);
						
						teleport(b);
					}
				}
			}
		}
		
		if (boat2!=-1)
		{
			sysmessage( s, "You board the boat." );
			setserial(currchar[s],boat2,8);
		} else 
			sysmessage(s, "There was no boat found there.");
	} else {
		LeaveBoat(s,p);//They are on a boat, get off
	}
	teleport(currchar[s]);//Show them they moved.
}


void cBoat::LeaveBoat(int s, int p)//Get off a boat (dbl clicked an open plank while on the boat.
{
	int x, x2 = items[p].x;
	int y, y2 = items[p].y;
	signed char z = items[p].z, mz, sz, typ;
	int boat = GetBoat( s );
	int a, b, serhash = chars[currchar[s]].serial%HASHMAX;
	
	if (boat==-1) return;
	
	for(x=x2-2;x<x2+3;x++)
	{
		for(y=y2-2;y<y2+3;y++)
		{
			sz = (signed char) Map->StaticTop( x, y, z ); // MapElevation() doesnt work cauz we are in a multi !!
			mz = (signed char) Map->MapElevation( x, y );
			if( sz == illegal_z ) 
				typ = 0;
			else 
				typ = 1;
			
			if((typ==0 && mz!=5) || (typ==1 && sz!=-5))// everthing the blocks a boat is ok to leave the boat ... LB
			{
				for(a=0;a<ownsp[serhash].max;a++)//Put all their Pets/Hirlings on the boat too
				{
					b=ownsp[serhash].pointer[a];
					if (b>-1 && b<cmem)
					{
						if (chars[b].npc && chars[b].ownserial==chars[currchar[s]].serial && inrange1p(currchar[s], b)<=15)
						{
							mapRegions->RemoveItem(b+1000000);
							chars[b].x=x;
							chars[b].y=y;
							if( typ ) 
								chars[b].z = chars[b].dispz = sz; 
							else 
								chars[b].z = chars[b].dispz = mz;
							mapRegions->AddItem( b + 1000000 );
							
							if( chars[b].multis != -1 )
							{
								unsetserial( b, 8 );
							}
							
							teleport(b);
						}
					}
				}
				
                mapRegions->RemoveItem(currchar[s]+1000000);
				chars[currchar[s]].x=x;
				chars[currchar[s]].y=y;
				unsetserial( currchar[s], 8 );
				
				if (typ) 
					chars[currchar[s]].z=chars[currchar[s]].dispz=sz; 
				else  
					chars[currchar[s]].z=chars[currchar[s]].dispz=mz;
				
				mapRegions->AddItem(currchar[s]+1000000);
				
				sysmessage(s,"You left the boat.");			
				return;
			}
		}//for y
	}//for x
	sysmessage(s,"You cannot get off here!");
}

void cBoat::OpenPlank(int p)//Open, or close the plank (called from keytarget() )
{
	switch(items[p].id2)
	{
		//Open plank->
	case 0xE9: items[p].id2=0x84; break;
	case 0xB1: items[p].id2=0xD5; break;
	case 0xB2: items[p].id2=0xD4; break;
	case 0x8A: items[p].id2=0x89; break;
	case 0x85: items[p].id2=0x84; break;
		//Close Plank->
	case 0x84: items[p].id2=0xE9; break;
	case 0xD5: items[p].id2=0xB1; break;
	case 0xD4: items[p].id2=0xB2; break;
	case 0x89: items[p].id2=0x8A; break;
	default: printf("WARNING: Invalid plank ID called! Plank %i '%s' [%x %x]\n",p,items[p].name,items[p].id1,items[p].id2); break;
	}
}

bool cBoat::Build(int s, int b, char id2)//Build a boat! (Do stuff NESSICARY for boats, called from buildhouse() )
{
	int tiller,p1,p2,hold;
	int nid2=id2;

	if( b == -1 ) 
	{
		sysmessage(s, "There was an error creating that boat.");
		return false;
	}
	
	if(id2!=0x00 && id2!=0x04 && id2!=0x08 && id2!=0x0C && id2!=0x10 && id2 != 0x14)//Valid boat ids (must start pointing north!)
	{
		sysmessage(s, "The deed is broken, please contact a Game Master.");
		return false;
	}
	//Start checking for a valid possition:

	if( Block( b, 0, 0, 0 ) )
	{
		sysmessage( s, "You cannot place your boat there." );
		return false;
	}
	
	// Okay we found a good  place....

	items[b].more1=id2;//Set min ID
	items[b].more2=nid2+3;//set MAX id
	items[b].type=117;//Boat type
	items[b].z=-5;//Z in water
	
	strcpy(items[b].name,"a mast");//Name is something other than "%s's house"
	tiller=Items->SpawnItem(s,1,"a tiller man",0,0x3E,0x4E,0,0,0,0);
	if( tiller == -1 ) return false;
	items[tiller].z=-5;
	items[tiller].priv=0;
	p2=Items->SpawnItem(s,1,"#",0,0x3E,0xB2,0,0,0,0);//Plank2 is on the RIGHT side of the boat
	if( p2 == -1 ) return false;
	items[p2].type=117;
	items[p2].type2=3;
	items[p2].more1=items[b].ser1;//Lock this item!
	items[p2].more2=items[b].ser2;
	items[p2].more3=items[b].ser3;
	items[p2].more4=items[b].ser4;
	items[p2].z=-5;
	items[p2].priv=0;//Nodecay
	p1=Items->SpawnItem(s,1,"#",0,0x3E,0xB1,0,0,0,0);//Plank1 is on the LEFT side of the boat
	if( p1 == -1 ) return false;
	items[p1].type=117;//Boat type
	items[p1].type2=3;//Plank sub type
	items[p1].more1=items[b].ser1;
	items[p1].more2=items[b].ser2;//Lock this
	items[p1].more3=items[b].ser3;
	items[p1].more4=items[b].ser4;
	items[p1].z=-5;
	items[p1].priv=0;
	hold=Items->SpawnItem(s,1,"#",0,0x3E,0xAE,0,0,0,0);
	if( hold == -1 ) return false;
	items[hold].more1=items[b].ser1;//Lock this too :-)
	items[hold].more2=items[b].ser2;
	items[hold].more3=items[b].ser3;
	items[hold].more4=items[b].ser4;
	items[hold].type=1;//Conatiner
	items[hold].z=-5;
	items[hold].priv=0;
	
	items[b].moreb1=items[tiller].ser1;//Tiller ser stored in boat's Moreb
	items[b].moreb2=items[tiller].ser2;
	items[b].moreb3=items[tiller].ser3;
	items[b].moreb4=items[tiller].ser4;
	items[b].morex=items[p1].serial;//Store the other stuff anywhere it will fit :-)
	items[b].morey=items[p2].serial;
	items[b].morez=items[hold].serial;
	
	switch(id2)//Give everything the right Z for it size boat
	{
	case 0x00:
	case 0x04:
		items[tiller].x=items[b].x+1;
		items[tiller].y=items[b].y+4;
		items[p2].x=items[b].x+2;
		items[p2].y=items[b].y;
		items[p1].x=items[b].x-2;
		items[p1].y=items[b].y;
		items[hold].x=items[b].x;
		items[hold].y=items[b].y-4;
		break;
	case 0x08:
	case 0x0C:
		items[tiller].x=items[b].x+1;
		items[tiller].y=items[b].y+5;
		items[p2].x=items[b].x+2;
		items[p2].y=items[b].y;
		items[p1].x=items[b].x-2;
		items[p1].y=items[b].y;
		items[hold].x=items[b].x;
		items[hold].y=items[b].y-4;
		break;
	case 0x10:
	case 0x14:
		items[tiller].x=items[b].x+1;
		items[tiller].y=items[b].y+5;
		items[p2].x=items[b].x+2;
		items[p2].y=items[b].y-1;
		items[p1].x=items[b].x-2;
		items[p1].y=items[b].y-1;
		items[hold].x=items[b].x;
		items[hold].y=items[b].y-5;
		break;
	}
	mapRegions->AddItem(tiller);//Make sure everything is in da regions!
	mapRegions->AddItem(p1);
	mapRegions->AddItem(p2);
	mapRegions->AddItem(hold);
	mapRegions->AddItem(b);
	
	//their x pos is set by BuildHouse(), so just fix their Z...
	chars[currchar[s]].z=chars[currchar[s]].dispz=items[b].z+3;//Char Z, try and keep it right.
    setserial(currchar[s],b,8);
	return true;
}

int cBoat::GetBoat(int s)//get the closest boat to the player and check to make sure they are on it
{
	//int x=chars[currchar[s]].x,y=chars[currchar[s]].y,z=chars[currchar[s]].z;
	int mindist=30, boat=-1;
	//int xa, ya, xb, yb, dx, dy;
	
    if (chars[currchar[s]].multis > -1 ) 
		return calcItemFromSer( chars[currchar[s]].multis );
	//else if (chars[currchar[s]].multis==-1) return -1;
	else 
	{
		int	StartGrid=mapRegions->StartGrid(chars[currchar[s]].x,chars[currchar[s]].y);
		//int	getcell=mapRegions->GetCell(chars[currchar[s]].x,chars[currchar[s]].y);
		
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
					if (mapitem>-1 && mapitem<999999 && iteminrange(s,mapitem,20))
					{
						if (items[mapitem].id1>=0x40)
						{
							if (dist(currchar[s],mapitem,1)<=mindist)//they are closer to this than 30, or the last Multi they were near.
							{
								mindist=dist(currchar[s],mapitem,1);//Store closest multi
								//printf("%s\n",items[mapitem].name);
								if (items[mapitem].type==117)//Boat type
									boat=mapitem;
							}
						}
					}
				} while (mapitem!=-1);
			}
		}
		if (boat!=-1)//if we found a boat, make sure they are in it
		{
			if(!inmulti(chars[currchar[s]].x,chars[currchar[s]].y,chars[currchar[s]].z,boat)) 
				boat=-1;
		}
					
		return boat;
	}
}

// This  Boat-blocking method is now WATER PROOF :o)
// Please don't TOUCH !!!
// Rewritten by Zippy February 7, 2000

bool cBoat::Block( int b, short xmove, short ymove, int dir )
{
	MapStaticIterator *msi;
	staticrecord *stat;
	map_st map;
	land_st land;
	tile_st tile;
	short cx = items[b].x, cy=items[b].y;
	short x1 = -1, y1 = -1,x2 = -1,y2 = -1,x,y;
	char type = 0;
	signed char sz, zt;

	cx+=xmove;
	cy+=ymove;

	switch( items[b].id2 )
	{
	case 0:	case 1: case 2: case 3://small
		type = 1; break;
	case 4: case 5: case 6: case 7://small dragon
		type = 1; break;

	case 8: case 9: case 10: case 11://medium
		type = 2; break;
	case 12: case 13: case 14: case 15://medium dragon
		type = 2; break;

	case 16: case 17: case 18: case 19://large
		type = 3; break;
	case 20: case 21: case 22: case 23://large dragon
		type = 3; break;

	default: return true;
	}

	//small = 5,11 
	//medium = 5, 13
	//large = 5, 15
	switch( items[b].dir )	
	{
	/* ----These should never happen.. there's no art for them.----
	case 1: // U
	case 5: // D
		break;
	case 3: // R
	case 7: // L
		break;
	*/
	case 0: // N
	case 4: // S
		x1 = cx - 2;
		x2 = cx + 2;
		switch( type )
		{
		case 1:	y1 = cy - 6; y2 = cy + 6; break;
		case 2:	y1 = cy - 6; y2 = cy + 7; break;
		case 3: y1 = cy - 8; y2 = cy + 8; break;
		}
		break;
	case 2: // E
	case 6: // W
		y1 = cy - 2;
		y2 = cy + 2;
		switch( type )
		{
		case 1:	x1 = cx - 6; x2 = cx + 6; break;
		case 2:	x1 = cx - 7; x2 = cx + 7; break;
		case 3: x1 = cx - 8; x2 = cx + 8; break;
		}
		break;
	default: return true;
	}

	for(x=x1;x<x2;x++)
	{
		for(y=y1;y<y2;y++)
		{
			sz=Map->StaticTop(x,y, items[b].z);

			if (sz==illegal_z) //map tile
			{
				map=Map->SeekMap0( x, y );
				Map->SeekLand(map.id, &land);
				if ( map.z>=-5 && !(land.flag1&0x80) && strcmp((char*)land.name,"water") )//only tiles on/above the water
					return true;
			} 
			else 
			{ //static tile
				msi = new MapStaticIterator(x,y);
				while (stat = msi->Next())
				{
					msi->GetTile(&tile);
					zt=stat->zoff+tile.height;
					if ( !(tile.flag1&0x80) && zt>-5 && zt<=15 && strcmp((char*)tile.name,"water") ) 
					{
						delete msi;
						return true;
					}
				}
				delete msi;
			}//if sz==
		}//for y
	}//for x
	return false;
}

void cBoat::Move(int s, int dir, int boat)
{//Move the boat and all it's items 1 square
	int tx=0,ty=0, c=0;
	int a,tiller=-1, p1=-1, p2=-1, hold=-1,serial;
	//get the #s of special items
	
	serial = calcserial(items[boat].moreb1,items[boat].moreb2,items[boat].moreb3,items[boat].moreb4);
	tiller = calcItemFromSer( serial );
	p1 = calcItemFromSer( items[boat].morex );
	p2 = calcItemFromSer( items[boat].morey );
	hold = calcItemFromSer( items[boat].morez );
	
	if (boat==-1) return;
	
	if (( s<0) || s>MAXCLIENT || tiller<0 || tiller>=imem || p1<0 || p1>=imem || p2<0 || p2>imem || hold<0 || hold >=imem) return;
	
	Network->xSend(s,pausex,2,0);
	
	switch(dir&0x0F)//Which DIR is it going in?
	{
	case '\x00' : 		ty--;		break;
	case '\x01' : 		tx++; 		ty--;		break;
	case '\x02' :		tx++;		break;
	case '\x03' :		tx++;		ty++;		break;
	case '\x04' : 		ty++;		break;
	case '\x05' :		tx--;		ty++;		break;
	case '\x06' : 		tx--;		break;
	case '\x07' : 		tx--; 		ty--;		break;
	default:
		printf("ERROR: Boat direction error: %i int boat %i\n",items[boat].dir&0x0F,items[boat].serial);
		break;
	}
	
	if( ( items[boat].x+tx<=200 || items[boat].x+tx>=6000 ) && ( items[boat].y+ty<=200 || items[boat].y+ty>=4900) )
	{
		items[boat].type2=0;
		itemtalk(s,tiller,"Arr, Sir, we've hit rough waters!");
		Network->xSend(s,restart,2,0);
		return;
	}
	//if(!validNPCMove(items[boat].x+tx,items[boat].y+ty,items[boat].z,currchar[s]))
	if(Block(boat,tx,ty,dir))
	{
		// printf("boat: %i dir: %i b-x: %i b-y: %i b-z: %i tx: %i ty: %i\n",boat,dir,items[boat].x,items[boat].y,items[boat].z,tx,ty);
		items[boat].type2=0;
		itemtalk(s, tiller, "Arr, somethings in the way!");
		Network->xSend(s,restart,2,0);
		return;
	}
	
	//Move all the special items
	mapRegions->RemoveItem(boat);
	items[boat].x+=tx;
	items[boat].y+=ty;
	mapRegions->AddItem(boat);
	sendinrange(boat);
	
    mapRegions->RemoveItem(tiller);
	items[tiller].x+=tx;
	items[tiller].y+=ty;
    mapRegions->AddItem(tiller);
	sendinrange(tiller);
	
    mapRegions->RemoveItem(p1);
	items[p1].x+=tx;
	items[p1].y+=ty;
    mapRegions->AddItem(p1);
	sendinrange(p1);
	
    mapRegions->RemoveItem(p2);
	items[p2].x+=tx;
	items[p2].y+=ty;
    mapRegions->AddItem(p2);
	sendinrange(p2);
	
    mapRegions->RemoveItem(hold);
	items[hold].x+=tx;
	items[hold].y+=ty;
    mapRegions->AddItem(hold);
	sendinrange(hold);
	
    serial=items[boat].serial;
	
	for (a=0;a<imultisp[serial%HASHMAX].max;a++)
	{
		c=imultisp[serial%HASHMAX].pointer[a];
		if(c!=-1)
		{
			mapRegions->RemoveItem(c);
			items[c].x+=tx;
			items[c].y+=ty;
			sendinrange(c);
			mapRegions->AddItem(c);
		}
	}
	
	for (a=0;a<cmultisp[serial%HASHMAX].max;a++)
	{
		c=cmultisp[serial%HASHMAX].pointer[a];
		if (c!=-1)
		{
			mapRegions->RemoveItem(c+1000000);
			chars[c].x+=tx;
			chars[c].y+=ty;
			teleport(c);
			mapRegions->AddItem(c+1000000);
		}
	}
	Network->xSend(s,restart,2,0);
}

void cBoat::TurnStuff(int b, int i, int dir, int type)//Turn an item that was on the boat when the boat was turned.
{
	int dx, dy;
	
    if (i<0 || i>=imem || b<0 || b>=imem) return;
	
    //printf("ts2\n");
	
	if(type)//item
	{
		dx=items[i].x-items[b].x;//get their distance x
		dy=items[i].y-items[b].y;//and distance Y
		
        mapRegions->RemoveItem(i);
		
		items[i].x=items[b].x;
		items[i].y=items[b].y;
		
		if(dir)//turning right
		{
			items[i].x+=dy*-1;
			items[i].y+=dx;
		} else {//turning left
			items[i].x+=dy;
			items[i].y+=dx*-1;
		}
		
		
		mapRegions->AddItem(i);
		
		sendinrange(i);
		
	} else {//Character
		dx=chars[i].x-items[b].x;
		dy=chars[i].y-items[b].y;
		//printf("name: %s\n",chars[i].name);
		
		mapRegions->RemoveItem(i+1000000);
        
		chars[i].x=items[b].x;
		chars[i].y=items[b].y;
		
		if(dir)
		{
			chars[i].x+=dy*-1;
			chars[i].y+=dx;
		} else {
			chars[i].x+=dy;
			chars[i].y+=dx*-1;
		}
		//Set then in their new cell
		
		mapRegions->AddItem(i+1000000);
		
		teleport(i);
	}
}

void cBoat::Turn(int b, int turn)//Turn the boat item, and send all the people/items on the boat to turnboatstuff()
{
	int id2 = items[b].id2, olddir = items[b].dir;
	unsigned short int Send[MAXCLIENT];
	int serial;
	int tiller, p1, p2, hold;
	int a,c,dir, d=0;
	
	if( b < 0 || b >= imem ) 
		return; 
	
	for( a = 0; a < now; a++ )
	{
		if( perm[a] && iteminrange( a, b, BUILDRANGE ) )
		{
			Send[d]=a;
			Network->xSend(a,pausex,2,0);
			d++;
		}
	}
	
	//Of course we need the boat items!
	serial = calcserial(items[b].moreb1,items[b].moreb2,items[b].moreb3,items[b].moreb4);
	tiller = calcItemFromSer( serial );
	p1 = calcItemFromSer( items[b].morex );
	p2 = calcItemFromSer( items[b].morey );
	hold = calcItemFromSer( items[b].morez );
	
    if (tiller<0 || tiller>=imem || p1<0 || p1>=imem || p2<0 || p2>imem || hold<0 || hold >=imem)
		return;
	
	if(turn)//Right
	{
		items[b].dir+=2;
		id2++;
	} else {//Left
		items[b].dir-=2;
		id2--;
	}
	if(items[b].dir>7) items[b].dir-=4;//Make sure we dont have any DIR errors
	if(items[b].dir<0) items[b].dir+=4;
	
	if(id2<items[b].more1) id2+=4;//make sure we don't have any id errors either
	if(id2>items[b].more2) id2-=4;//Now you know what the min/max id is for :-)
	
	if( Block( b, 0, 0, items[b].dir ) )
	{
		items[b].dir = olddir;
		for( a = 0; a < d; a++ )
		{
			Network->xSend( Send[a], restart, 2, 0 );
			itemtalk( Send[a], tiller, "Arr, something's in the way!" );
		}
		return;
	}
	items[b].id2=id2;//set the id
	
	if(items[b].id2==items[b].more1) items[b].dir=0;//extra DIR error checking
	if(items[b].id2==items[b].more2) items[b].dir=6;
	
    serial=items[b].serial; // lb !!!
	
	
    for (a=0;a<imultisp[serial%HASHMAX].max;a++)
	{
		c=imultisp[serial%HASHMAX].pointer[a];
		if (c!=-1)
			TurnStuff(b,c,turn,1);
	}
	
	for (a=0;a<cmultisp[serial%HASHMAX].max;a++)
	{
		c=cmultisp[serial%HASHMAX].pointer[a];
		if (c!=-1)
			TurnStuff(b,c,turn,0);
	}
	
	//Set the DIR for use in the Offsets/IDs array
	dir=(items[b].dir&0x0F)/2;
	
	//set it's Z to 0,0 inside the boat
	
    mapRegions->RemoveItem(p1);
	items[p1].x=items[b].x;
	items[p1].y=items[b].y;
	items[p1].id2=cShipItems[dir][PORT_P_C];//change the ID
	
    mapRegions->RemoveItem(p2);
	items[p2].x=items[b].x;
	items[p2].y=items[b].y;
	items[p2].id2=cShipItems[dir][STAR_P_C];
	
    mapRegions->RemoveItem(tiller);
	items[tiller].x=items[b].x;
	items[tiller].y=items[b].y;
	items[tiller].id2=cShipItems[dir][TILLERID];
	
    mapRegions->RemoveItem(hold);
	items[hold].x=items[b].x;
	items[hold].y=items[b].y;
	items[hold].id2=cShipItems[dir][HOLDID];
	
	switch(items[b].more1)//Now set what size boat it is and move the specail items
	{
	case 0x00:
	case 0x04:
		items[p1].x+=iSmallShipOffsets[dir][PORT_PLANK][XP];
		items[p1].y+=iSmallShipOffsets[dir][PORT_PLANK][YP];
		items[p2].x+=iSmallShipOffsets[dir][STARB_PLANK][XP];
		items[p2].y+=iSmallShipOffsets[dir][STARB_PLANK][YP];
		items[tiller].x+=iSmallShipOffsets[dir][TILLER][XP];
		items[tiller].y+=iSmallShipOffsets[dir][TILLER][YP];
		items[hold].x+=iSmallShipOffsets[dir][HOLD][XP];
		items[hold].y+=iSmallShipOffsets[dir][HOLD][YP];
		break;
	case 0x08:
	case 0x0C:
		items[p1].x+=iMediumShipOffsets[dir][PORT_PLANK][XP];
		items[p1].y+=iMediumShipOffsets[dir][PORT_PLANK][YP];
		items[p2].x+=iMediumShipOffsets[dir][STARB_PLANK][XP];
		items[p2].y+=iMediumShipOffsets[dir][STARB_PLANK][YP];
		items[tiller].x+=iMediumShipOffsets[dir][TILLER][XP];
		items[tiller].y+=iMediumShipOffsets[dir][TILLER][YP];
		items[hold].x+=iMediumShipOffsets[dir][HOLD][XP];
		items[hold].y+=iMediumShipOffsets[dir][HOLD][YP];
		
		break;
	case 0x10:
	case 0x14:
		items[p1].x += (short int)iLargeShipOffsets[dir][PORT_PLANK][XP];
		items[p1].y += (short int)iLargeShipOffsets[dir][PORT_PLANK][YP];
		items[p2].x += (short int)iLargeShipOffsets[dir][STARB_PLANK][XP];
		items[p2].y += (short int)iLargeShipOffsets[dir][STARB_PLANK][YP];
		items[tiller].x += (short int)iLargeShipOffsets[dir][TILLER][XP];
		items[tiller].y += (short int)iLargeShipOffsets[dir][TILLER][YP];
		items[hold].x += (short int)iLargeShipOffsets[dir][HOLD][XP];
		items[hold].y += (short int)iLargeShipOffsets[dir][HOLD][YP];
		
		break;
	default: printf("DEBUG: Turnboatstuff() more1 error! more1 = %c not found!\n",items[b].more1);
	}
	mapRegions->AddItem( p1 );
	mapRegions->AddItem( p2 );
	mapRegions->AddItem( tiller );
	mapRegions->AddItem( hold );
	sendinrange(p1);
	sendinrange(p2);
	sendinrange(hold);
	sendinrange(tiller);
	
	for (a=0;a<d;a++)
		Network->xSend(Send[a],restart,2,0);
}

void cBoat::Speech(int s, unsigned char *talk)//See if they said a command.
{
	int boat = GetBoat( s );
	if( boat == -1 ) 
		return;//if they aren't on a boat, then we don't care what they said
	int dir = items[boat].dir&0x0F;
	int serial, tiller;
	char msg[128];	// No one can type more than 80 chars in UO Client
	
	strcpy( msg, (char *)talk );
	if (s<0 || s>=MAXCLIENT) return;
	
	//get the tiller man's item #
	serial=calcserial(items[boat].moreb1,items[boat].moreb2,items[boat].moreb3,items[boat].moreb4);
	tiller = calcItemFromSer( serial );
	
	//if(dist(currchar[s],tiller,1)>4) return;
	
	strcpy( msg, strupr( msg ) );
	/*forward, backward, right, left, anchor down, raise anchor, one left, one right, one */
	if(strstr(msg,"FORWARD") || strstr(msg,"UNFURL SAIL"))
	{
		items[boat].type2=1;//Moving
		itemtalk(s, tiller, "Aye, sir.");
//		Move(s,dir,boat);
	} else if(strstr(msg,"BACKWARD") || strstr(msg, "REVERSE"))
	{
		items[boat].type2=2;//Moving backward
		if(dir>=4) dir-=4; 
		else dir+=4;
		itemtalk(s, tiller, "Aye, sir.");
//		Move(s,dir,boat);		
	}  else if(strstr(msg,"ONE") || strstr(msg,"DRIFT"))
	{
		if(strstr(msg,"LEFT"))
		{
			dir-=2;
			if(dir<0) dir+=8;
			itemtalk(s, tiller, "Aye, sir.");
			items[boat].type2 = 0;
			Move(s,dir,boat);
		} else if(strstr(msg,"RIGHT"))
		{
			dir+=2;
			if(dir>=8) dir-=8; 			
			itemtalk(s, tiller, "Aye, sir.");
			items[boat].type2 = 0;
			Move(s,dir,boat);
		}
	} 
	else if(strstr(msg,"STOP") || strstr(msg,"FURL SAIL")) 
	{ 
		items[boat].type2=0;
		itemtalk(s, tiller, "Aye, sir."); 
	}//Moving is type2 1 and 2, so stop is 0 :-)
	else if((strstr(msg,"TURN") && (strstr(msg,"AROUND") || strstr(msg,"LEFT") || strstr(msg,"RIGHT")))
		|| strstr(msg,"PORT") || strstr(msg,"STARBOARD") || strstr(msg,"COME ABOUT"))
	{
		if(strstr(msg,"RIGHT") || strstr(msg,"STARBOARD")) 
		{
			dir-=2; if(dir<0) dir+=8;
			int tx=0,ty=0;
			
			switch(dir&0x0F) // little reminder for myself: move this swtich to a function to have less code ... LB
			{
	           case '\x00' :	ty--;			break;
			   case '\x01' :	tx++;	ty--;	break;
			   case '\x02' :	tx++;			break;
			   case '\x03' :	tx++;	ty++;	break;
			   case '\x04' :	ty++;			break;
			   case '\x05' :	tx--;	ty++;	break;
			   case '\x06' :	tx--;			break;
			   case '\x07' :	tx--;	ty--;	break;
			}
			
			if (!Block(boat,tx,ty,dir))
			{
				itemtalk(s, tiller, "Aye, sir.");
				Turn(boat,1);
			} else { 
				items[boat].type2=0;
				itemtalk(s,tiller,"Arr,somethings in the way"); 
			}   
		}
		else if(strstr(msg, "LEFT") || strstr(msg,"PORT")) 
		{
			dir+=2; if(dir>7) dir-=8;
			int tx=0,ty=0;
			
			switch(dir&0x0F)
			{
	           case '\x00' :	ty--;			break;
			   case '\x01' :	tx++;	ty--;	break;
			   case '\x02' :	tx++;			break;
			   case '\x03' :	tx++;	ty++;	break;
			   case '\x04' :	ty++;			break;
			   case '\x05' :	tx--;	ty++;	break;
			   case '\x06' :	tx--;			break;
			   case '\x07' :	tx--;	ty--;	break;
			}
			
			if( !Block( boat,tx,ty,dir ) )
			{
				itemtalk(s, tiller, "Aye, sir.");
				Turn(boat,0);			
			} else 
			{ 
				itemtalk(s,tiller,"Arr,somethings in the way"); 
				items[boat].type2=0;
			}
		}
		else if(strstr(msg,"COME ABOUT") || strstr(msg,"AROUND"))
		{
			itemtalk(s, tiller, "Aye, sir.");
			Turn(boat,1);
			Turn(boat,1);
		}
	}
	else if(strstr(msg,"SET NAME"))
	{
		//:Terrin: not only fix sprintf problem but cannot assume that the
		//         "SET NAME" was the first thing on the line
		// do some checking on what they typed
		char *cmd = strstr( msg, "SET NAME "); // note: also checking for space
		if( !cmd )
		{
			itemtalk( s, tiller, "What be that, sir?" );
			return;
		}
		cmd += 9;
		while (*cmd && *cmd == ' ') ++cmd; // remove any extra spaces
		if( !*cmd )
		{
			itemtalk( s, tiller, "Can ya say that again with an actual name, sir?" );
			return;
		}
		// okay, proceed
		strcpy( items[tiller].name, "a ship named " );
		// translate position back to original message to preserve case
		strcat( items[tiller].name, (char *)&talk[msg - cmd] );
	}
}
