//------------------------------------------------------------------------
//  skills.cpp
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
/* 
*  UOX3 Skills
*/

/*
** 10/2/99 - Krazyglue
** Replaced approx. 50 instances of sprintf with strcpy both for
** speed and to fix possible bugs, ie: sprintf(var, data); where
** data might have contained a %s or other special sprintf tag.
** Also strcpy is a much more efficient routine than sprintf as
** it does not have to parse every byte of the field being copied.
*/
#include "uox3.h"
#include "debug.h"
//int goldsmithing;
//1=iron, 2=gold, 3=agapite, 4=adamantium, 5=mythril, 6=bronze, 7=verite, 8=merkite, 9=copper, 10=silver
int ingottype=0;//will hold number of ingot type to be deleted
int carptype=0;

#define DBGFILE "skills.cpp"


//**********************************************************************************************
// Get amount of ingot type used for smithing : Cork
// added color checking
// *********************************************************************************************
int cSkills::GetIngotAmt(int s, unsigned char id1, unsigned char id2, unsigned char color1, unsigned char color2)
{
	int i,p,serial,serhash,ci;
	int total=0;
	
	p=packitem(s);
	if (p==-1) return 0; //LB
	serial=items[p].serial;
	serhash=serial%HASHMAX;
	for (ci=0;ci<contsp[serhash].max;ci++)
	{
		i=contsp[serhash].pointer[ci];
		if( i >-1 )
		{
			if (items[i].contserial==serial)
			{
				if ((items[i].id1==id1 && items[i].id2==id2) &&
					(items[i].color1==color1 && items[i].color2==color2)) total=total+items[i].amount;
				if (items[i].type==1) total=total+Skills->GetSubIngotAmt(i, id1, id2, color1, color2);
			}
		}
	}
	return total;
}
//***********************************************************************************************

//***********************************************************************************************
//getsubingotamt routine : Cork
//added color checking
//***********************************************************************************************
int cSkills::GetSubIngotAmt(int p, char id1, char id2, char color1, char color2)
{
	int i,serial,serhash,ci;
	int total=0;
	serial=items[p].serial;
	serhash=serial%HASHMAX;
	for (ci=0;ci<contsp[serhash].max;ci++)
	{
		i=contsp[serhash].pointer[ci];
		if ( i > -1 )
		{
			if (items[i].contserial==serial)
			{
				if ((items[i].id1==id1 && items[i].id2==id2) &&
					(items[i].color1==color1 && items[i].color2==color2)) total=total+items[i].amount;
				if (items[i].type==1) total=total+Skills->GetSubIngotAmt(i, id1, id2, color1, color2);
			}
		}
	}
	return total;
}
//***********************************************************************************************

//***********************************************************************************************
//Skills->DeleIngot routine for smithing skill  : Cork
//added color checking on item
//***********************************************************************************************
void cSkills::DeleIngot(int s, int id1, int id2, int color1, int color2, int amount)
{
	int i/*, j*/, p, total, serial, serhash, ci;
	total=amount;
	p=packitem(s);
	if (p==-1) return;
	
	serial=items[p].serial;
	serhash=serial%HASHMAX;
	for (ci=0;ci<contsp[serhash].max;ci++)
	{
		i=contsp[serhash].pointer[ci];
		if( i > -1 )
		{
			if (items[i].contserial==serial)
			{
				if (items[i].type==1)
				{
					total=total-Skills->DeleSubIngot(i, id1, id2, color1, color2, total);
				}
				if ((items[i].id1==id1 && items[i].id2==id2) &&
					(items[i].color1==color1 && items[i].color2==color2))
				{
					if (items[i].amount<=total)
					{
						total=total-items[i].amount;
						Items->DeleItem(i);
					}
					else
					{
						items[i].amount=items[i].amount-total;
						total=0;
						//						for (j=0;j<now;j++) if (perm[j]) senditem(j,i);
						RefreshItem( i ); // AntiChrist
					}
				}
				if (total==0) return;
			}
		}
	}
}
//*********************************************************************************************************

//*********************************************************************************************************
//delsubingot routine for smithing skill  : Cork
//added color checking on item
//*********************************************************************************************************
int cSkills::DeleSubIngot(int p, int id1, int id2, int color1, int color2, int amount)
{
	int i/*, j*/, k, serial, serhash, ci;
	int total, totaldel=0;
	total=amount;
	serial=items[p].serial;
	serhash=serial%HASHMAX;
	for (ci=0;ci<contsp[serhash].max;ci++)
	{
		i=contsp[serhash].pointer[ci];
		if( i > -1 )
		{
			if (items[i].contserial==serial)
			{
				if (items[i].type==1)
				{
					k=Skills->DeleSubIngot(i, id1, id2, color1, color2, total);
					total=total-k;
					totaldel=totaldel+k;
				}
				if ((items[i].id1==id1 && items[i].id2==id2) &&
					(items[i].color1==color1 && items[i].color2==color2))
				{
					if (items[i].amount<=total)
					{
						total=total-items[i].amount;
						totaldel=totaldel+items[i].amount;
						Items->DeleItem(i);
					}
					else
					{
						items[i].amount=items[i].amount-total;
						totaldel=totaldel+total;
						total=0;
						//						for (j=0;j<now;j++) if (perm[j]) senditem(j,i);
						RefreshItem( i ); // AntiChrist
					}
				}
				if (total==0) return totaldel;
			}
		}
	}
	return totaldel;
}
//***********************************************************************************************************

void cSkills::Tailoring( UOXSOCKET s )
{
	int i, packnum, amt;
	
	packnum=packitem(currchar[s]);
	
	if( packnum == -1 ) 
	{
		sysmessage( s, "Time to buy a backpack" ); 
		return; 
	}
	
	i = calcItemFromSer( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	if( i != -1 )
	{
		if( !(chars[currchar[s]].priv&0x01) && items[i].magic == 3 )
		{
			sysmessage( s, "That is locked down and you cannot use it" );
			return;
		}
		unsigned short realID = (items[i].id1<<8) + items[i].id2;
		if( realID >= 0x0F95 && realID <= 0x0F9C || realID >= 0x175D && realID <= 0x1764 || realID == 0x1078 )
		{
			if( GetPackOwner( i ) != currchar[s] )
			{
				sysmessage( s, "You can't use material outside your backpack" );
			}           
			else
			{
				amt = getamount( currchar[s], items[i].id1, items[i].id2 );
				if( items[i].id1 == 0x0F && ( items[i].id2 >= 0x95 && items[i].id2 <= 0x9C ) )
					amt *= 50;
				itemmake[s].has = amt;
				if(amt<1)
				{ 
					sysmessage(s,"You don't have enough material to make anything.");
					return;
				}
				itemmake[s].materialid1=items[i].id1;
				itemmake[s].materialid2=items[i].id2; 
				if (items[i].id1==0x10 && items[i].id2==0x78) 
				{
					if( items[i].color1 == 0x00 && items[i].color2 == 0xEF )
					{
						chars[currchar[s]].runenumb = -17;
						Skills->MakeMenu( s, 1600, TAILORING );
					}
					else
					{
						chars[currchar[s]].runenumb = -1;
						Skills->MakeMenu( s, 40, TAILORING );
					}
				}
				else Skills->MakeMenu(s,30,TAILORING);
			}
			return;
		}
	}
	sysmessage(s,"You cannot use that material for tailoring.");
}   

void cSkills::Fletching(int s)
{
	int i, packnum;
	
	packnum=packitem(currchar[s]);
	if( packnum == -1 ) 
	{
		sysmessage(s,"Time to buy a backpack"); 
		return; 
	}
	
	i = calcItemFromSer( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	if (i!=-1)
	{
		if( !(chars[currchar[s]].priv&0x01) && items[i].magic == 3 )
		{
			sysmessage( s, "That is locked down and you cannot use it" );
			return;
		}
		if (((itemmake[s].materialid2>=0xD4 && itemmake[s].materialid2<=0xD6) && (items[i].id1==0x1B && (items[i].id2>=0xD1 && items[i].id2<=0xD3))) ||
			((itemmake[s].materialid2>=0xD1 && itemmake[s].materialid2<=0xD3) && (items[i].id1==0x1B && (items[i].id2>=0xD4 && items[i].id2<=0xD6))))
		{
			if( GetPackOwner( i ) != currchar[s] )
			{
				sysmessage(s,"You can't use items outside your backpack.");
			}
			else
			{
				itemmake[s].materialid1b=items[i].id1;
				itemmake[s].materialid2b=items[i].id2;
				itemmake[s].has=getamount(currchar[s], itemmake[s].materialid1, itemmake[s].materialid2); 
				itemmake[s].has2=getamount(currchar[s], itemmake[s].materialid1b, itemmake[s].materialid2b);
				MakeMenu( s, 60, BOWCRAFT );
			}
			return;
		}
	}
	sysmessage(s,"You cannot use that for fletching.");
}

void cSkills::BowCraft(int s)
{
	int i, packnum, amt;
	
	packnum = packitem( currchar[s] );
	if( packnum == -1 ) 
	{
		sysmessage(s,"Time to buy a backpack"); 
		return; 
	}
	i = calcItemFromSer( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	if( i != -1 )
	{
		if( !(chars[currchar[s]].priv&0x01) && items[i].magic == 3 )
		{
			sysmessage( s, "That is locked down and you cannot use it" );
			return;
		}
		if((items[i].id1==0x1B) && ((items[i].id2==0xDD) || items[i].id2==0xE0))
		{
			if( GetPackOwner( i ) != currchar[s] )
			{
				sysmessage(s,"You can't carve logs outside your backpack");
				return;
			}           
			else
			{
				itemmake[s].has=amt=getamount(currchar[s], items[i].id1, items[i].id2);    
				if(amt<2)
				{ 
					sysmessage(s,"You don't have enough material to make anything.");
				}
				else 
				{
					if (chars[currchar[s]].onhorse) action(s,0x1C);	// moved here rather then the top of fun
					else action(s,0x0D);							// so that we don't make a motion if invalid target!

					itemmake[s].materialid1=items[i].id1;
					itemmake[s].materialid2=items[i].id2;
					MakeMenu( s, 65, BOWCRAFT );
				}
				return;
			}
		} 
	}
}



void cSkills::Carpentry(int s)
// Essentially rewritten, Abaddon 17th February
// Can now use materials in any of our packs, not just main level
// Also allows nonGMs to use carpentry
{
	int i, packnum, amt;
	
	packnum = packitem(currchar[s]);
	if (packnum==-1) 
	{
		sysmessage(s,"Time to buy a backpack"); 
		return; 
	}
	i = calcItemFromSer( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	if( i != -1 )
	{
		if( !(chars[currchar[s]].priv&0x01) && items[i].magic == 3 )
		{
			sysmessage( s, "That is locked down and you cannot use it" );
			return;
		}
		int realID = (items[i].id1<<8) + items[i].id2;
		if( realID == 0x1BE0 || realID == 0x1BD7 )
		{
			if( GetPackOwner( i ) != currchar[s] )
			{
				sysmessage(s,"You can't use lumber outside your backpack");
				return;
			}         
			else
			{
				itemmake[s].has = amt = getamount( currchar[s], items[i].id1, items[i].id2 );
				itemmake[s].materialid1 = items[i].id1;
				itemmake[s].materialid2 = items[i].id2;
				if(amt<9)
				{	 
					sysmessage(s,"You don't have enough resources to make anything!!");
					return;
				}
				switch( realID )
				{
				case 0x1BE0:	carptype = 1;	break;
				case 0x1BD7:	carptype = 2;	break;
				default:		carptype = 1;	break;
				}           
				MakeMenu( s, 20, CARPENTRY );
				return;
			}
		}
	}
	sysmessage(s,"You cannot use that material for carpentry.");
}

//o--------------------------------------------------------------------------
//| Function    - cSkills::Smith(int s);
//| Date        - January 31, 1999
//| Programmer  - Cork
//| Modified    - Abaddon(February 21, 2000)
//| Modified	- Abaddon(July, 2000)
//o--------------------------------------------------------------------------
//| Purpose     - Rewritten to use switch, You'll find it is easier to make 
//|               it scriptable now
//o--------------------------------------------------------------------------
void cSkills::Smith( UOXSOCKET s )
{
	int i, packnum;
	int realID = 0, realColour = 0;
	packnum=packitem(currchar[s]);
	char name[20];
	char failMessage[100];
	
	if( packnum == -1 ) 
	{
		sysmessage( s, "Time to buy a backpack" );
		return; 
	}
	
	i = calcItemFromSer( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );

	if( i != -1 )
	{
		if( !(chars[currchar[s]].priv&0x01) && items[i].magic == 3 )
		{
			sysmessage( s, "That is locked down and you cannot use it" );
			return;
		}
		itemmake[s].materialid1 = items[i].id1;
		itemmake[s].materialid2 = items[i].id2;
		realID = (items[i].id1<<8) + items[i].id2;
		realColour = (items[i].color1<<8) + items[i].color2;
		switch( realID )
		{
		  case 0x1BEF:
		  case 0x1BF2:	ingottype = 1;  strcpy( name, "iron" );			break;
		  case 0x1BEC:	ingottype = 2;  strcpy( name, "gold" );			break;
		  case 0x1BE6:	ingottype = 9;  strcpy( name, "copper" );		break;
		  case 0x0F2F:	ingottype = 12; strcpy( name, "emerald" );		break;
		  case 0x0F21:	ingottype = 13; strcpy( name, "star sapphire" );break;
		  case 0x0F24:	ingottype = 14; strcpy( name, "citrine" );		break;
		  case 0x0F23:	ingottype = 15; strcpy( name, "citrine" );		break;
		  case 0x0F2C:	ingottype = 16; strcpy( name, "citrine" );		break;
		  case 0x0F2D:	ingottype = 17; strcpy( name, "tourmaline" );	break;
		  case 0x0F22:	ingottype = 18; strcpy( name, "amethyst" );		break;
		  case 0x0F2E:	ingottype = 19; strcpy( name, "amethyst" );		break;
		  case 0x0F2A:	ingottype = 20; strcpy( name, "ruby" );			break;
		  case 0x0F2B:	ingottype = 21; strcpy( name, "ruby" );			break;
		  case 0x1BF8:
			  switch( realColour )
			  {
			    case 0x0150:	ingottype = 3;	strcpy( name, "agapite" );		break;
			    case 0x0386:	ingottype = 4;	strcpy( name, "adamantium" );	break;
			    case 0x0191:	ingottype = 5;	strcpy( name, "mythril" );		break;
			    case 0x02E7:	ingottype = 6;	strcpy( name, "bronze" );		break;
			    case 0x022F:	ingottype = 7;	strcpy( name, "verite" );		break;
			    case 0x02C3:	ingottype = 8;	strcpy( name, "merkite" );		break;
			    case 0x0000:	ingottype = 10;	strcpy( name, "silver" );		break;
			    default:		ingottype = 22;	strcpy( name, "unknown" );		break;
  			}
			break;
			default:	ingottype = 22;	strcpy( name, "unknown" );	break;
	  	}
		sprintf( failMessage, "You can't smith %s ingots outside your backpack", name );
		if( items[i].contserial != items[packnum].serial )
		{	
			sysmessage( s, failMessage );
		}
		else
		{	
			switch( ingottype )
			{	
			case 1:		AnvilTarget( s, items[i], name );		return;
			case 2:		AnvilTarget( s, items[i], name, 50 );	return;
			case 3:		AnvilTarget( s, items[i], name, 806 );	return;
			case 4:		AnvilTarget( s, items[i], name, 800 );	return;
			case 5:		AnvilTarget( s, items[i], name, 803 );	return;
			case 6:		AnvilTarget( s, items[i], name, 801 );	return;
			case 7:		AnvilTarget( s, items[i], name, 802 );	return;
			case 8:		AnvilTarget( s, items[i], name, 804 );	return;
			case 9:		AnvilTarget( s, items[i], name, 814 );	return;
			case 10:	AnvilTarget( s, items[i], name, 813 );	return;
			case 12:	AnvilTarget( s, items[i], name, 1520);	return;
			case 13:	AnvilTarget( s, items[i], name, 1525);	return;
			case 14:	AnvilTarget( s, items[i], name, 1500);	return;
			case 15:	AnvilTarget( s, items[i], name, 1500);	return;
			case 16:	AnvilTarget( s, items[i], name, 1500);	return;
			case 17:	AnvilTarget( s, items[i], name, 1510);	return;
			case 18:	AnvilTarget( s, items[i], name, 1505);	return;
			case 19:	AnvilTarget( s, items[i], name, 1505);	return;
			case 20:	AnvilTarget( s, items[i], name, 1515);	return;
			case 21:	AnvilTarget( s, items[i], name, 1515);	return;
			default:	break;
			}
		}
		itemmake[s].materialid1 = 0x00;
		itemmake[s].materialid2 = 0x00;
	}
	sysmessage(s,"You cannot use that material for blacksmithing");
}
// End my section of code
//*************************************************************************************************************

void cSkills::AnvilTarget( int s, item_st& item, char *ingotName, int makemenu )
{
	int amt;
	
	unsigned int i;
	int	StartGrid = mapRegions->StartGrid(chars[currchar[s]].x, chars[currchar[s]].y );
	
	unsigned int increment=0;
	for (unsigned int checkgrid = StartGrid+(increment*mapRegions->GetColSize());increment<3;increment++, checkgrid=StartGrid+(increment*mapRegions->GetColSize()))
	{
		for( int a = 0; a < 3; a++ )
		{
			int mapitemptr = -1;
			int mapitem = -1;
			int mapchar = -1;
			do //check all items in this cell
			{
				mapchar = -1;
				mapitemptr = mapRegions->GetNextItem( checkgrid + a, mapitemptr );
				if( mapitemptr == -1 ) 
					break;
				mapitem = mapRegions->GetItem( checkgrid + a, mapitemptr );
				if( mapitem > 999999 ) 
					mapchar = mapitem - 1000000;
				if( mapitem != -1 && mapitem < imem )
				{
					i = mapitem;
					short realID = (items[i].id1<<8) + items[i].id2;
					if( realID == 0x0FAF || realID == 0x0FB0 )
					{
						if( iteminrange( s, i, 3 ) )
						{
							if( item.color1 == 0 && item.color2 == 0 )
								itemmake[s].has = amt = getamount( currchar[s], item.id1, item.id2 );
							else
								itemmake[s].has = amt = GetIngotAmt( currchar[s], item.id1, item.id2, item.color1, item.color2 );     
							if( amt < 3 )
							{ 
								sysmessage( s, "You don't have enough %s ingots to make anything.", ingotName );
								return;
							}
							MakeMenu( s, makemenu, BLACKSMITHING );
							return;
						}
					}
				}
			}  while( mapitem != -1 );
		}
	}
	sysmessage(s,"The anvil is too far away.");
}

// Rank-System Made by Magius(CHE)
//o---------------------------------------------------------------------------o
//|   Function    :  int cSkills::CalcRank(int s, int skill)
//|   Date        :  24 August 1999
//|   Programmer  :  Magius(CHE)
//o---------------------------------------------------------------------------o
//|   Purpose     :  Calculate item rank based on player' skill.
//o---------------------------------------------------------------------------o

int cSkills::CalcRank( int s, int skill )
{
	int rk_range, rank;
	float sk_range, randnum, randnum1;
	
	rk_range = itemmake[s].maxrank - itemmake[s].minrank;
	sk_range = (float) 50.00 + chars[currchar[s]].skill[skill] - itemmake[s].minskill;
	if( sk_range <= 0 ) rank = itemmake[s].minrank;
	else if( sk_range >= 1000 ) rank = itemmake[s].maxrank;
	randnum = rand()%1000;
	//randnum = sk_range + 1.0; // for debug
	if( randnum <= sk_range ) rank = itemmake[s].maxrank;
	else
	{
		randnum1 = (float)( rand()%1000 ) - (( randnum - sk_range ) / ( 11 - server_data.skilllevel ) );
		rank = (int)( randnum1*rk_range)/1000;
		rank += itemmake[s].minrank - 1;
		if( rank > itemmake[s].maxrank ) rank = itemmake[s].maxrank;
		if( rank < itemmake[s].minrank ) rank = itemmake[s].minrank;
	}
	return rank;
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::ApplyRank(int s, int c, int rank )
//|   Date        :  24 August 1999
//|   Programmer  :  Magius(CHE)
//o---------------------------------------------------------------------------o
//|   Purpose     :  Modify variables based on item's rank.
//o---------------------------------------------------------------------------o

void cSkills::ApplyRank( int s, int c, int rank )
{
	char tmpmsg[512];
	*tmpmsg='\0';
	
	if( server_data.rank_system == 1 )
	{
		items[c].rank = rank;
		// Variable to change!
		// LODAMAGE, HIDAMAGE, ATT, DEF, HP, MAXHP
		if( items[c].lodamage > 0 ) items[c].lodamage = ( rank * items[c].lodamage ) / 10;
		if( items[c].hidamage > 0 ) items[c].hidamage = ( rank * items[c].hidamage ) / 10;
		if( items[c].att > 0 )      items[c].att      = ( rank * items[c].att      ) / 10;
		if( items[c].def > 0 )      items[c].def      = ( rank * items[c].def      ) / 10;
		if( items[c].hp  > 0 )      items[c].hp       = ( rank * items[c].hp       ) / 10;
		if( items[c].maxhp  > 0 )   items[c].maxhp    = ( rank * items[c].maxhp    ) / 10;
		
		switch( rank )
		{
		case 1:	strcpy( tmpmsg, "You made an item with no quality!" ); break;
		case 2:	strcpy( tmpmsg, "You made an item with very below standard quality!" ); break;
		case 3:	strcpy( tmpmsg, "You made an item below standard quality!" ); break;
		case 4:	strcpy( tmpmsg, "You made a weak quality item!" ); break;
		case 5:	strcpy( tmpmsg, "You made a standard quality item!" ); break;
		case 6:	strcpy( tmpmsg, "You made a nice quality item!" ); break;
		case 7:	strcpy( tmpmsg, "You made a good quality item!" ); break;
		case 8:	strcpy( tmpmsg, "You made a great quality item!" ); break;
		case 9:	strcpy( tmpmsg, "You made a beautiful quality item!" ); break;
		case 10: strcpy( tmpmsg, "You made a perfect quality item!" ); break;
		}
		sysmessage( s, tmpmsg );
	}
	else items[c].rank = rank;
	
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Zero_Itemmake(int s )
//|   Date        :  24 August 1999
//|   Programmer  :  Magius(CHE)
//o---------------------------------------------------------------------------o
//|   Purpose     :  Resets all values into itemmake[s].
//o---------------------------------------------------------------------------o

void cSkills::Zero_Itemmake( int s )
{
	memset( &itemmake[s], 0, sizeof( make_st ) );		// MUCH more efficient
	itemmake[s].minrank = itemmake[s].maxrank = 10;
}
//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Repair(int s)
//|   Date        :  October 16, 2000
//|   Programmer  :  Thaliq
//o---------------------------------------------------------------------------o
//|   Purpose     :  Repair armor and weapons.
//o---------------------------------------------------------------------------o

void cSkills::Repair( UOXSOCKET s )
{
	int j = calcItemFromSer( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	if( j == -1 )
	{
		sysmessage( s, "You cannot repair that" );
		return;
	}
	if( items[j].def <= 0 )
	{
		sysmessage(s, "You cannot repair this item.");
		return;
	}
	if( items[j].hp == items[j].maxhp )
	{
		sysmessage( s, "That item is already fully repaired." );
		return;
	}
	short minSkill = 0, maxSkill = 1000;
	if( items[j].def < 3 )
	{
		minSkill = 600;
		maxSkill = 850;
	}
	else if( items[j].def > 2 && items[j].def < 6 )
	{
		minSkill = 600;
		maxSkill = 850;
	}
	else if( items[j].def > 5 && items[j].def < 9 )
	{
		minSkill = 750;
		maxSkill = 950;
	}
	else if( items[j].def > 8 && items[j].def < 11 )
	{
		minSkill = 850;
		maxSkill = 999;
	}
	else if( items[j].def > 10 && items[j].def < 14 )
	{
		minSkill = 900;
		maxSkill = 999;
	}
	else if( items[j].def > 13 )
	{
		minSkill = 999;
		maxSkill = 1000;
	}
	if( CheckSkill( currchar[s], BLACKSMITHING, minSkill, maxSkill ) )
	{
		items[j].hp = items[j].maxhp;
		sysmessage( s, "You repair the item succesfully." );
		soundeffect( s, 0x00, 0x2a );
	}
	else
	{
		sysmessage( s, "You fail to repair the item." );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void Skills->MakeMenutarget(int s,int x,int skill)
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  create a specified menuitem target.(Can someone elaborate 
//|                  here please (EviLDeD)
//o---------------------------------------------------------------------------o
// ----- Changed by Magius(CHE)
void cSkills::MakeMenuTarget(int s, int x, int skill)
{
	int c, chkskill = 0;
	int rank = 10;
	int tmpneed = 0;
	int amt = 0;
	int btmp = 0;
	if( chars[currchar[s]].making!= 999 )	// when using /add and a door, skill was 1755
		chkskill=Skills->CheckSkill( currchar[s], skill, itemmake[s].minskill, itemmake[s].maxskill );
	if(chars[currchar[s]].making==999) {}
	else 
		if(!chkskill && !(chars[currchar[s]].priv&0x01)) 
		{
			amt = getamount( currchar[s], itemmake[s].materialid1, itemmake[s].materialid2 );
			if( itemmake[s].materialid1 == 0x0F && ( itemmake[s].materialid2 >= 0x95 && itemmake[s].materialid2 <= 0x9C ) )
				amt *= 50;
			if( amt < itemmake[s].needs )
			{
				sysmessage( s, "You do not have enough resources!!!" );
				return;
			}
			tmpneed = itemmake[s].needs / 2;
			if( tmpneed == 0 ) itemmake[s].needs++;
			switch(skill) 
			{
			case BLACKSMITHING: 
				switch( ingottype )
				{
				case 1: DeleIngot( currchar[s], 0x1B, 0xF2, 0x00, 0x00, itemmake[s].needs/2); break; // delete iron
				case 2: DeleIngot( currchar[s], 0x1B, 0xEC, 0x00, 0x00, itemmake[s].needs/2); break; //delete gold
				case 3: DeleIngot( currchar[s], 0x1B, 0xF8, 0x01, 0x50, itemmake[s].needs/2); break; //delete agapite
				case 4: DeleIngot( currchar[s], 0x1B, 0xF8, 0x03, 0x86, itemmake[s].needs/2); break; //delete adamantium
				case 5: DeleIngot( currchar[s], 0x1B, 0xF8, 0x01, 0x91, itemmake[s].needs/2); break; //delete mythril
				case 6: DeleIngot( currchar[s], 0x1B, 0xF8, 0x02, 0xE7, itemmake[s].needs/2); break; //delete bronze
				case 7: DeleIngot( currchar[s], 0x1B, 0xF8, 0x02, 0x2F, itemmake[s].needs/2); break; //delete verite
				case 8: DeleIngot( currchar[s], 0x1B, 0xF8, 0x02, 0xC3, itemmake[s].needs/2); break; //delete merkite
				case 9: DeleIngot( currchar[s], 0x1B, 0xE6, 0x00, 0x00, itemmake[s].needs/2); break; //delete copper
				case 10: DeleIngot( currchar[s], 0x1B, 0xF8, 0x00, 0x00, itemmake[s].needs/2); break; //delete silver
				case 12: DeleIngot( currchar[s], 0x0f, 0x2f, 0x00, 0x00, itemmake[s].needs/2); break; //delete Emerald
				case 13: DeleIngot( currchar[s], 0x0f, 0x21, 0x00, 0x00, itemmake[s].needs/2); break; //delete star sapphire
				case 14: DeleIngot( currchar[s], 0x0f, 0x24, 0x00, 0x00, itemmake[s].needs/2); break; //delete citrine
				case 15: DeleIngot( currchar[s], 0x0f, 0x23, 0x00, 0x00, itemmake[s].needs/2); break; //delete citrine
				case 16: DeleIngot( currchar[s], 0x0f, 0x2c, 0x00, 0x00, itemmake[s].needs/2); break; //delete citrine
				case 17: DeleIngot( currchar[s], 0x0f, 0x2d, 0x00, 0x00, itemmake[s].needs/2); break; //delete tourmaline
				case 18: DeleIngot( currchar[s], 0x0f, 0x22, 0x00, 0x00, itemmake[s].needs/2); break; //delete amethyst
				case 19: DeleIngot( currchar[s], 0x0f, 0x2e, 0x00, 0x00, itemmake[s].needs/2); break; //delete amethyst
				case 20: DeleIngot( currchar[s], 0x0f, 0x2a, 0x00, 0x00, itemmake[s].needs/2); break; //delete ruby
				case 21: DeleIngot( currchar[s], 0x0f, 0x2b, 0x00, 0x00, itemmake[s].needs/2); break; //delete ruby
					// ----- AntiChrist (2) customizable BLACKSMITHING source deletion -----
					// ----- ( using .materialid1 and .materialid2 ) -----
				default:	delequan( currchar[s], itemmake[s].materialid1, itemmake[s].materialid2, itemmake[s].needs / 2 );
				}
				
				soundeffect(s,0x00,0x2a);
				ingottype=0;
				sysmessage(s,"You fail to create the item.");
				break;
				//*************************************************************************************************************
				case CARPENTRY:     
					if (carptype==1) delequan(currchar[s], 0x1B, 0xE0, itemmake[s].needs/2);
					if (carptype==2) delequan(currchar[s], 0x1B, 0xD7, itemmake[s].needs/2);
					soundeffect(s,0x02,0x3d);
					sysmessage(s,"You fail to create the item.");
					break;
				case INSCRIPTION:
					delequan(currchar[s], 0x0E, 0x34, 1);
					sysmessage(s,"You fail to inscribe the spell");
					break;
				case TAILORING:     
					if( chars[currchar[s]].runenumb == -17 )
					{
						DeleIngot( s, itemmake[s].materialid1, itemmake[s].materialid2, 0x00, 0xEF, itemmake[s].needs/2 );
						chars[currchar[s]].runenumb = -1;
					}
					else if( itemmake[s].materialid1 == 0x0F && ( itemmake[s].materialid2 >= 0x95 && itemmake[s].materialid2 <= 0x9C ) )
					{
						btmp = ((itemmake[s].needs / 2) / 50) + 1;
						c = Items->SpawnItem( s, ((btmp*50) - (itemmake[s].needs/2)), "Cut Cloth", 1, 0x17, 0x5F, 0, 0, 1, 1 );
						if( c == -1 )
						{
							sysmessage( s, "You fail to create the item." );
							break;
						}
						delequan(currchar[s], itemmake[s].materialid1, itemmake[s].materialid2, btmp );  
					}
					else if( itemmake[s].materialid1 == 0x10 && itemmake[s].materialid2 == 0x78 )
					{
						DeleIngot( s, itemmake[s].materialid1, itemmake[s].materialid2, 0x00, 0x00, itemmake[s].needs / 2 );
					}
					else 
						delequan(currchar[s], itemmake[s].materialid1, itemmake[s].materialid2, itemmake[s].needs / 2);  

					soundeffect(s,0x02,0x48);
					sysmessage(s,"You fail to create the item.");
					break;
				case COOKING:     
					delequan(currchar[s], 0x17, 0x5D, itemmake[s].needs/2);  
					soundeffect(s,0x02,0x25);
					sysmessage(s,"You fail to create the item.");
					break;
				case BOWCRAFT:      
					if (itemmake[s].has<2) delequan(currchar[s], itemmake[s].materialid1, itemmake[s].materialid2, 1);
					else delequan(currchar[s], itemmake[s].materialid1, itemmake[s].materialid2, itemmake[s].needs/2);
					if (itemmake[s].has2<2) delequan(currchar[s], itemmake[s].materialid1b, itemmake[s].materialid2b, 1);
					else delequan(currchar[s], itemmake[s].materialid1b, itemmake[s].materialid2b, itemmake[s].needs/2);
					soundeffect(s,0x00,0x4A);
					sysmessage(s,"You fail to create the item.");
					itemmake[s].has=0;
					itemmake[s].has2=0;
					break;
				case TINKERING:
					delequan(currchar[s], itemmake[s].materialid1, itemmake[s].materialid2, itemmake[s].needs/2);
					soundeffect(s, 0x00, 0x2A);
					sysmessage(s, "You fail to create the item.");
					break;
					//default:
					//				printf("ERROR: Fallout of switch statement without default. skills.cpp, Skills->MakeMenutarget()/n"); //Morrolan
					// ----- AntiChrist (2) added the way to use every skill for making items not only the ones listed -----
				default:
					delequan( currchar[s], itemmake[s].materialid1, itemmake[s].materialid2, itemmake[s].needs / 2 );
					sysmessage( s, "You fail to create the item." );
			}
			return;
		}  
		amt = getamount( currchar[s], itemmake[s].materialid1, itemmake[s].materialid2 );
		if( itemmake[s].materialid1 == 0x0F && ( itemmake[s].materialid2 >= 0x95 && itemmake[s].materialid2 <= 0x9C ) )
			amt *= 50;
		if( amt < itemmake[s].needs && !chars[currchar[s]].priv&0x01 )
		{
			sysmessage( s, "You do not have enough resources!!!" );
			return;
		}
		
		switch( skill )
		{ // AntiChrist
			
		case BLACKSMITHING:

			switch( ingottype )
			{
			case 1: DeleIngot( currchar[s], 0x1B, 0xF2, 0x00, 0x00, itemmake[s].needs); break; //delete iron
			case 2: DeleIngot( currchar[s], 0x1B, 0xEC, 0x00, 0x00, itemmake[s].needs); break; //delete gold
			case 3: DeleIngot( currchar[s], 0x1B, 0xF8, 0x01, 0x50, itemmake[s].needs); break; //delete agapite
			case 4: DeleIngot( currchar[s], 0x1B, 0xF8, 0x03, 0x86, itemmake[s].needs); break; //delete adamantium
			case 5: DeleIngot( currchar[s], 0x1B, 0xF8, 0x01, 0x91, itemmake[s].needs); break; //delete mythril
			case 6: DeleIngot( currchar[s], 0x1B, 0xF8, 0x02, 0xE7, itemmake[s].needs); break; //delete bronze
			case 7: DeleIngot( currchar[s], 0x1B, 0xF8, 0x02, 0x2F, itemmake[s].needs); break; //delete verite
			case 8: DeleIngot( currchar[s], 0x1B, 0xF8, 0x02, 0xC3, itemmake[s].needs); break; //delete merkite
			case 9: DeleIngot( currchar[s], 0x1B, 0xE6, 0x00, 0x00, itemmake[s].needs); break; //delete copper
			case 10: DeleIngot( currchar[s], 0x1B, 0xF8, 0x00, 0x00, itemmake[s].needs); break; //delete silver
			case 12: DeleIngot( currchar[s], 0x0f, 0x2f, 0x00, 0x00, itemmake[s].needs); break; //delete Emerald
			case 13: DeleIngot( currchar[s], 0x0f, 0x21, 0x00, 0x00, itemmake[s].needs); break; //delete star sapphire
			case 14: DeleIngot( currchar[s], 0x0f, 0x24, 0x00, 0x00, itemmake[s].needs); break; //delete citrine
			case 15: DeleIngot( currchar[s], 0x0f, 0x23, 0x00, 0x00, itemmake[s].needs); break; //delete citrine
			case 16: DeleIngot( currchar[s], 0x0f, 0x2c, 0x00, 0x00, itemmake[s].needs); break; //delete citrine
			case 17: DeleIngot( currchar[s], 0x0f, 0x2d, 0x00, 0x00, itemmake[s].needs); break; //delete tourmaline
			case 18: DeleIngot( currchar[s], 0x0f, 0x22, 0x00, 0x00, itemmake[s].needs); break; //delete amethyst
			case 19: DeleIngot( currchar[s], 0x0f, 0x2e, 0x00, 0x00, itemmake[s].needs); break; //delete amethyst
			case 20: DeleIngot( currchar[s], 0x0f, 0x2a, 0x00, 0x00, itemmake[s].needs); break; //delete ruby
			case 21: DeleIngot( currchar[s], 0x0f, 0x2b, 0x00, 0x00, itemmake[s].needs); break; //delete ruby
				// ----- AntiChrist (2) customizable BLACKSMITHING source deletion -----
				// ----- ( using .materialid1 and .materialid2 ) -----
			default:	delequan( currchar[s], itemmake[s].materialid1, itemmake[s].materialid2, itemmake[s].needs );
			}
			break;
			//********************************************************************************************************************************
			case CARPENTRY:
				if( carptype == 1 ) delequan( currchar[s], 0x1B, 0xE0, itemmake[s].needs );
				if( carptype == 2 ) delequan( currchar[s], 0x1B, 0xD7, itemmake[s].needs );
				break;
			case INSCRIPTION:	delequan( currchar[s], itemmake[s].materialid1, itemmake[s].materialid2, 1 ); break; // don't use default, cauz we delete 1 scroll // use materialid
			case BOWCRAFT:
				delequan(currchar[s], itemmake[s].materialid1, itemmake[s].materialid2, itemmake[s].needs);
				delequan(currchar[s], itemmake[s].materialid1b, itemmake[s].materialid2b, itemmake[s].needs);
				break;		
			case TAILORING:
				if( chars[currchar[s]].runenumb == -17 )
				{
					DeleIngot( s, itemmake[s].materialid1, itemmake[s].materialid2, 0x00, 0xEF, itemmake[s].needs/2 );
					chars[currchar[s]].runenumb = -1;
				}
				else if( itemmake[s].materialid1 == 0x0F && ( itemmake[s].materialid2 >= 0x95 && itemmake[s].materialid2 <= 0x9C ) )
				{
					btmp = ((itemmake[s].needs / 2) / 50) + 1;
					c = Items->SpawnItem( s, ((btmp*50) - (itemmake[s].needs)), "Cut Cloth", 1, 0x17, 0x5F, 0, 0, 1, 1 );
					if( c == -1 )
					{
						printf( "SKILLS.cpp: Tailoring spawn bolts to cloth failure.\n" );
						return;
					}
					delequan(currchar[s], itemmake[s].materialid1, itemmake[s].materialid2, btmp );  
				}
				else if( itemmake[s].materialid1 == 0x10 && itemmake[s].materialid2 == 0x78 )
					DeleIngot( s, itemmake[s].materialid1, itemmake[s].materialid2, 0x00, 0x00, itemmake[s].needs );
				else 
					delequan(currchar[s], itemmake[s].materialid1, itemmake[s].materialid2, itemmake[s].needs);  
				break;
			default:	
				delequan( currchar[s], itemmake[s].materialid1, itemmake[s].materialid2, itemmake[s].needs );
		}
		
		itemmake[s].materialid1=0;
		itemmake[s].materialid2=0;
		
		c=Items->SpawnItemBackpack2(s, currchar[s], x, 0);
		if (c==-1)
		{
			printf("[1]SKILLS.CPP: Skills->MakeMenutarget() bad script item # %d(Item Not found).\n", x);
			return;  //invalid script item
		}
		else
		{
			//			if( items[c].name2 && ( strcmp( items[c].name2, "#" ))) strcpy( items[c].name, items[c].name2 ); // Item identified - Magius(CHE)
			strcpy( items[c].name2, items[c].name );
			if( server_data.rank_system == 1 ) rank = CalcRank( s, skill ); // Magius(CHE)
			else if( server_data.rank_system == 0 ) rank = 10; // Magius(CHE)
			ApplyRank( s, c, rank ); // Magius(CHE)
			
			// AntiChrist -
			// do this only if not a GM! bugfix - to avoid "a door mixed by GM..."
			if(!(chars[currchar[s]].priv&0x01))
			{
				strcpy( items[c].creator, chars[currchar[s]].name ); // Magius(CHE) - Memorize Name of the creator
				if( chars[currchar[s]].skill[skill] > 950 ) items[c].madewith = skill + 1; // Memorize Skill used - Magius(CHE)
				else items[c].madewith = 0 - skill - 1; // Memorize Skill used - Magius(CHE)
			}
			else
			{
				items[c].creator[0] = '\0';
				items[c].madewith = 0;
			}
		} // End Rank System Addon
		
		items[c].magic = 1; // JM's bugfix
		if(chars[currchar[s]].making==999) chars[currchar[s]].making=c; // store item #
		else chars[currchar[s]].making=0;
		if( skill == MINING ) soundeffect( s, 0x00, 0x54 ); // Added by Magius(CHE)
		if (skill==BLACKSMITHING) soundeffect(s,0x00,0x2a);
		if (skill==CARPENTRY) soundeffect(s,0x02,0x3d);
		if (skill==INSCRIPTION) soundeffect(s,0x02,0x49);
		if (skill==TAILORING) soundeffect(s,0x02,0x48);
		if (skill==TINKERING) soundeffect(s,0x00,0x2A);
		if (skill==COOKING) soundeffect(s,0x02,0x25);
		// EviLDeD  -  I noticed that when there was a success there were 2
		//          of the intended item created. Going to comment this out
		//          until another time, or someone says that it should be there
		// December 26, 1998
		//c=SpawnItemBackpack2(s, x, 0);
		//if (c==-1)
		//{
		// printf("[2]SKILLS.CPP:Skills->MakeMenutarget() bad script item # %d.\n", x);
		// return;  //invalid script item
		//}
		if(!chars[currchar[s]].making) sysmessage(s,"You create the item and place it in your backpack.");
		itemmake[s].has=0;
		itemmake[s].has2=0;
		statwindow(s,currchar[s]);
}

void cSkills::MakeMenu(int s, int m, int skill) // Menus for playermade objects
{ // s - character online #, m - menu to use, skill - skill being used
	int total, i;
	char lentext;
	char sect[512];
	char gmtext[30][257];
	int gmid[30];
	int gmnumber=0;
	int gmindex;
	int minres = 0;		// To calculate minimum resources required! By Magius(CHE) for Rank System
	int minskl = 0;		// To calculate minimum skill required! By Magius(CHE) for Rank System
	int tmpgmnumber = 0;// By Magius(CHE) for Rank System
	
	chars[currchar[s]].making=skill;
	
	openscript("create.scp");
	sprintf(sect, "MAKEMENU %i", m);
	if (!i_scripts[create_script]->find(sect)) 
	{
		closescript();
		return;
	}
	gmindex=m;
	read1();
	strcpy(gmtext[0], script1);
	read2(); // Moved By Magius(CHE) for Rank System
	do
	{
		if (script1[0]!='}')
		{
			gmnumber++;
			tmpgmnumber++; // Magius(CHE)
			gmid[gmnumber]=hstr2num(script1);
			strcpy(gmtext[gmnumber], script2);
			read2();
			// Magius(CHE) - Alert Script Error
			if( strcmp( script1, "RESOURCE" ))			
			{
				printf("SKILLS.CPP: MakeMenu(). Expected 'RESOURCE <num>' after '%s'!\n", gmtext[gmnumber] );
				return;
			}
			itemmake[s].needs=str2num(script2);
			read2();
			itemmake[s].minskill=str2num(script2);
			itemmake[s].maxskill=itemmake[s].minskill*server_data.skilllevel; // by Magius(CHE)
			// Magius(CHE) - Alert Script Error
			if( strcmp( script1, "SKILL" ))			
			{
				printf("SKILLS.CPP: MakeMenu(). Expected 'SKILL <num>' after 'RESOURCE %i'!\n", itemmake[s].needs );
				return;
			}
			if (itemmake[s].maxskill<200) itemmake[s].maxskill=200;
			if ((itemmake[s].has<itemmake[s].needs) || ((itemmake[s].has2) &&
				(itemmake[s].has2<itemmake[s].needs)) || (chars[currchar[s]].skill[skill]<itemmake[s].minskill))
				gmnumber--;
			// Start Here new lines for rank system by Magius(CHE)
			read2();
			itemmake[s].number = str2num( script2 );
			if( minres > itemmake[s].needs || !minres ) minres = itemmake[s].needs;
			if( minskl > itemmake[s].minskill || !minskl ) minskl = itemmake[s].minskill;	
			// Magius(CHE) - Alert Script Error
			if( strcmp( script1, "ADDITEM" ) && strcmp( script1, "MAKEMENU" ))			
			{
				printf("SKILLS.CPP: MakeMenu(). Expected 'ADDITEM/MAKEMENU <num>' after 'SKILL %i'!\n", itemmake[s].minskill );
				return;
			}
			// Item Rank System - by Magius(CHE)
			read2(); // Read RANK
			if( !strcmp( script1, "RANK" ) )
			{
				gettokennum( script2, 0 );
				itemmake[s].minrank = str2num( gettokenstr );
				gettokennum( script2, 1 );
				itemmake[s].maxrank = str2num( gettokenstr );
				read2();
			}
			else // Set maximum rank if the item is not ranked!
			{
				itemmake[s].minrank = 10;
				itemmake[s].maxrank = 10;
			}
			if( server_data.rank_system == 0 )
			{
				itemmake[s].minrank = 10;
				itemmake[s].maxrank = 10;
			}
			
		}
	}
	while (script1[0]!='}');
	closescript();
	if (!gmnumber) 
	{
		sysmessage(s,"You aren't skilled enough to make anything with what you have.");
		return; 
	}
	sprintf(temp, "%i: %s", m, gmtext[0]);
	lentext=sprintf(gmtext[0], "%s", temp);
	total=9+1+lentext+1;
	for (i=1;i<=gmnumber;i++)
	{
		total+=4+1+strlen(gmtext[i]);
	}
	gmprefix[1]=total>>8;
	gmprefix[2]=total%256;
	gmprefix[3]=chars[currchar[s]].ser1;
	gmprefix[4]=chars[currchar[s]].ser2;
	gmprefix[5]=chars[currchar[s]].ser3;
	gmprefix[6]=chars[currchar[s]].ser4;
	gmprefix[7]=(gmindex+MAKEMENUOFFSET)>>8;
	gmprefix[8]=(gmindex+MAKEMENUOFFSET)%256;
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
	targetok[s]=1; 
}

//o--------------------------------------------------------------------------
//| Function    - cSkills::Mine(int s);
//| Date        - Unknown
//| Programmer  - Unknown
//| Modified    - Cork(Unknown)/Abaddon(February 19, 2000)
//o--------------------------------------------------------------------------
//| Purpose     - (Fill this in)
//| Comments    - Skill checking now implemented. You cannot minr colored ore
//|               unless you have the proper mining skill for each ore type. -Cork
//|               Rewrote most of it to clear up some of the mess-Abaddon
//o--------------------------------------------------------------------------
void cSkills::Mine(int s)
{
	bool floor = false;
	bool mountain = false;
	static unsigned int oretime[610][410];//610 and 410 were 1000 in LB release
	static short int oreamount[610][410];//for now i'll put zippy values
	unsigned char targetID1, targetID2;
	short int targetX, targetY;
	short int gridX, gridY;
	short int distX, distY;
	short int oreX, oreY;
	signed char targetZ;
	
	if( buffer[s][11] == 0xFF && buffer[s][12] == 0xFF && buffer[s][13] == 0xFF && buffer[s][14] == 0xFF )
		return;	// did we cancel the target?

	if( resource.orearea < 10 )	// make sure there are enough minimum areas
		resource.orearea = 10;

	if( oretime[0][0] == 0 )	// first time execution
	{
		oretime[0][0]=17;
		oreamount[0][0]=resource.ore;
		for( gridX = 1; gridX < 610; gridX++ )
		{
			for( gridY = 1; gridY < 410; gridY++ )
	 		{
				oreamount[gridX][gridY] = resource.ore;
				oretime[gridX][gridY] = (unsigned int)( uiCurrentTime + resource.oretime * CLOCKS_PER_SEC );
			}
		}
	}
	
	targetX = ( buffer[s][0x0B]<<8 ) + buffer[s][0x0C];		// store our target x y and z locations
	targetY = ( buffer[s][0x0D]<<8 ) + buffer[s][0x0E];
	targetZ = buffer[s][0x10];

	distX = abs( chars[currchar[s]].x - targetX );			// find our distance
	distY = abs( chars[currchar[s]].y - targetY );

	if( distX > 5 || distY > 5 )							// too far away?
	{
		sysmessage( s, "You are too far away to mine that!" );
		return;
	}
	
	targetID1 = buffer[s][0x11];							// store ids of what we targetted
	targetID2 = buffer[s][0x12];

	if( targetZ < 28 && targetID1 == 14 )					// if not too high
	{
		switch( targetID2 )
		{
		  case 0xD3:
		  case 0xDF:
		  case 0xE0:
		  case 0xE1:
		  case 0xE8:
			  Skills->GraveDig(s);							// check to see if we targetted a grave, if so, check it
			  break;
		  default:
			  break;
		}
	}
	
	switch( server_data.minecheck )
	{
	case 0:
		floor = true;
		mountain = true;
		break;
	case 1:
		if( targetZ == 0 )			// check to see if we're targetting a dungeon floor
		{
			if( targetID1 == 0x05 )
			{
				if( ( targetID2 >= 0x3B && targetID2 <= 0x4F ) || ( targetID2 >= 0x51 && targetID2 <= 0x53 ) || ( targetID2 == 0x6A ) )
				{
					floor = true;
				}
			}
		}
		if( !floor && ( targetZ >= 0 ) )	// mountain not likely to be below 0 (but you never know, do you? =)
		{
			if( targetID1 != 0 && targetID2 != 0 )	// we might have a static rock or mountain
			{
				MapStaticIterator msi( targetX, targetY );
				tile_st tile;
				staticrecord *stat;
				while ((( stat = msi.Next()) != NULL) && !mountain )
				{
					msi.GetTile(&tile);
					if( targetZ == stat->zoff && ( !strcmp( (char*)tile.name, "rock" ) || !strcmp( (char*)tile.name, "mountain" ) || !strcmp( (char *)tile.name, "cave" ) ) )
					{
						mountain = true;
					}
				}
			}
			else		// or it could be a map only
			{  // manually calculating the ID's if a maptype
				map_st map1;
				land_st land;
				map1 = Map->SeekMap0( targetX, targetY );
				Map->SeekLand(map1.id, &land);
				if( !strcmp("rock",land.name) || !strcmp(land.name, "mountain") || !strcmp( land.name, "cave" ) ) 
					mountain = true; 
			}
  		}
	  	break;
	  case 2:	// need to modify scripts to support this!
		  floor = true;		// we'll default to work everywhere for these scripts
		  mountain = true;
		  break;
	  default:
		  sysmessage( s, "Unknown mine check option, contact your shard op!" );
		  return;
	}

	oreX = targetX / resource.orearea;	// we want the area where we targetted, not where we are
	oreY = targetY / resource.orearea;
	if( (!floor && !mountain) || (oreX >= 610 || oreY >= 410) )		// if we can't mine here or if for some reason it's invalid, clear out
	{
		sysmessage( s, "You cannot mine there" );
		return;
	}
	
	if( oretime[oreX][oreY] <= uiCurrentTime )	// is it time to regenerate some ore?
	{
		for( int counter = 0; counter < resource.ore; counter++ )	// keep regenerating ore (could be a long time since we last did it)
		{
			if( oreamount[oreX][oreY] < resource.ore && ( ( oretime[oreX][oreY] + counter * resource.oretime * CLOCKS_PER_SEC ) < uiCurrentTime ) )
			{
				oreamount[oreX][oreY]++;
			}
			else
				break;
		}
		oretime[oreX][oreY] = (unsigned int)(uiCurrentTime + resource.oretime * CLOCKS_PER_SEC);	// reset ore regen timer
	}
	
	if( oreamount[oreX][oreY] > resource.ore )	// if there's too much ore, then put a ceiling on it
		oreamount[oreX][oreY] = resource.ore;

	if( oreamount[oreX][oreY] <= 0 )			// if we have no ore, clear out
	{
		sysmessage( s, "There is no metal to mine here" );
		return;
	}
	if( chars[currchar[s]].onhorse != 0 )	// do action and sound
		action( s, 0x1A );
	else
		action( s, 0x0B );
	
	soundeffect( s, 0x01, 0x25 );
	
	if( !Skills->CheckSkill( currchar[s], MINING, 0, 1000 ) ) // check to see if our skill is good enough
	{
		sysmessage(s,"You sifted thru the dirt and rocks, but found nothing useable.");
		if( oreamount[oreX][oreY] > 0 && rand()%2 == 1 )	// if we fail, random chance of mineral loss
			oreamount[oreX][oreY]--;
		return;
	} 
	else if( oreamount[oreX][oreY] > 0 )	// remove some more ore
		oreamount[oreX][oreY]--;
	
	if( buffer[s][1] == 1 && buffer[s][2] == 0 && buffer[s][3] == 1 && buffer[s][4] == 0 )	// this part onwards to be scriptable!!!!!!
	{	// I have absolutely no idea why these particular values, someone should have documented why =)
		unsigned char chanceFindingColoured = 0;
		unsigned char chanceFindBigOre = RandomNum( 0, 20 );	// pick random num from 0-20
		chanceFindingColoured = RandomNum( 0, 100 );			// chance of finding coloured ore
		unsigned short int playersSkill = chars[currchar[s]].skill[MINING];
		playersSkill += (unsigned short int)Races->getDamageFromSkill( MINING, chars[currchar[s]].race );	// remember we could be exceptional miners!
		
		//  Cork  - Unknown
		//  If mining skill is lower than 65 can only mine iron ore
		if ( playersSkill < 650 )
		{
			if ( chanceFindBigOre == 10 )	// chance of finding small ore
			{
				Items->SpawnItem( s, 5, "Iron Ore", 1, 0x19, 0xBA, 0, 0, 1, 1 );
				sysmessage(s,"You place some iron ore in your pack.");
			}
			else
			{
				Items->SpawnItem( s, 1, "Iron Ore", 1, 0x19, 0xB9, 0, 0, 1, 1 );
				sysmessage(s,"You place some iron ore in your pack.");
			}
			return;
		}
		//  Cork  - End

		if ( chanceFindingColoured <= 10 )	// small chance of finding coloured ore, make it scriptable?
		{
			unsigned char oreType = RandomNum( 0, 18 );	// pick a random type
			switch( oreType )
			{
			case 1:
				if( playersSkill >= 850 )
				{
					SpawnRandomItem(s,1,"necro.scp","ITEMLIST","999"); 
					sysmessage(s,"You place a gem in your pack.");
					break;
				}
			case 3: 
				if( playersSkill >= 990 )
				{
					Items->SpawnItem( s, 1, "Mythril Ore", 1, 0x19, 0xB9, 0x01, 0x91, 1, 1 );
					sysmessage( s, "You place some Mythril ore in your pack." );
					break;
				}
			case 5:
				if( playersSkill >= 950 )
				{
					Items->SpawnItem( s, 1, "Verite Ore", 1, 0x19, 0xB9, 0x02, 0x2F, 1, 1);
					sysmessage( s, "You place some Verite ore in your pack." );
					break;
				}
			case 7: 
				if( playersSkill >= 900 )
				{
					Items->SpawnItem( s, 1, "Agapite Ore", 1,0x19,0xB9, 0x01, 0x50,1,1);
					sysmessage(s, "You place some Agapite ore in your pack." );
					break;
				}
			case 9: 
				if( playersSkill >= 850 )
				{
					Items->SpawnItem( s, 1, "Gold Ore", 1, 0x19, 0xB9, 0x09, 0x6D, 1, 1);
					sysmessage( s, "You place some Gold ore in your pack." );
					break;
				}
			case 11: 
				if( playersSkill >= 800 )
				{
					Items->SpawnItem( s, 1, "Bronze Ore", 1, 0x19, 0xB9, 0x02, 0xE7, 1, 1);
					sysmessage( s, "You place some Bronze ore in your pack." );
					break;
				}
			case 13:
				if( playersSkill >= 750 )
				{
					Items->SpawnItem( s, 1, "Copper Ore", 1, 0x19, 0xB9, 0x00, 0xF3, 1, 1);
					sysmessage( s, "You place some Copper ore in your pack." );
					break;
				}
			case 15:
				if( playersSkill >= 700 )
				{
					Items->SpawnItem( s, 1, "Merkite Ore", 1, 0x19, 0xB9, 0x02, 0xC3, 1, 1 );
					sysmessage( s, "You place some Merkite ore in your pack." );
					break;   
				}
			case 17:
				if( playersSkill >= 790 )
				{
					Items->SpawnItem( s, 1, "Silver Ore", 1, 0x19, 0xB9, 0x03, 0x8A, 1, 1 );
					sysmessage( s, "You place some Silver ore in your pack." );
					break;   
				}
			default:
				Items->SpawnItem( s, 1, "Black Ore", 1, 0x19, 0xB9, 0x03, 0x86, 1, 1 );
				sysmessage( s, "You place some Black ore in your pack." );
				break;
			}
		}
		else  //  We didnt find any colored ore, so grab some iron ore
		{
			if ( chanceFindBigOre == 10 )
			{
				Items->SpawnItem( s, 5, "Iron Ore", 1, 0x19, 0xBA, 0, 0, 1, 1 );
				sysmessage(s, "You place some iron ore in your pack." );
			}
			else
			{
				Items->SpawnItem( s, 1, "Iron Ore", 1, 0x19, 0xB9, 0, 0, 1, 1 );
				sysmessage(s,"You place some iron ore in your pack.");
			}
		}
	}
}

void cSkills::GraveDig(int s) // added by Genesis 11-4-98
{
	int   nAmount, nFame, nItemID, nCharID;
	char iID=0;
	
	nCharID=currchar[s];
	Karma(nCharID,-1,-2000); // Karma loss no lower than the -2 pier
	
	if(chars[currchar[s]].onhorse)
		action(s,0x1A);
	else
		action(s,0x0b);
	soundeffect(s,0x01,0x25);
	if(!Skills->CheckSkill(currchar[s],MINING, 0, 800)) 
	{
		sysmessage(s,"You sifted through the dirt and found nothing.");
		return;
	}
	
	nFame = chars[nCharID].fame;
	if(chars[nCharID].onhorse)
		action(s,0x1A);
	else  
		action(s,0x0b);
	soundeffect(s,0x01,0x25);     
	int nRandnum=rand()%13;
	switch(nRandnum)
	{
	case 2:
		SpawnRandomMonster(s,"necro.scp","UNDEADLIST","1000"); // Low level Undead - Random
		sysmessage(s,"You have disturbed the rest of a vile undead creature.");
		break;
	case 4:
		nItemID=SpawnRandomItem(s,1,"necro.scp","ITEMLIST","1001"); // Armor and shields - Random
		if((nItemID>=7026)&&(nItemID<=7035))
			sysmessage(s,"You unearthed an old shield and placed it in your pack");
		else
			sysmessage(s,"You have found an old piece armor and placed it in your pack.");
		break;
	case 5:
		//Random treasure between gems and gold
		nRandnum=rand()%2;
		if(nRandnum)
		{  // randomly create a gem and place in backpack
			SpawnRandomItem(s,1,"necro.scp","ITEMLIST","999");
			sysmessage(s,"You place a gem in your pack.");
		}
		else
		{  // Create between 1 and 15 goldpieces and place directly in backpack
			nAmount=1+(rand()%15);
			addgold(nCharID,nAmount);
			goldsfx(s,nAmount);
			if (nAmount==1)
				sprintf(temp,"You unearthed %i gold coin.", nAmount);
			else
				sprintf(temp,"You unearthed %i gold coins.", nAmount);
			sysmessage(s,temp);
		}
		break;
	case 6:
		if(nFame<500)
			SpawnRandomMonster(s,"necro.scp","UNDEADLIST","1000"); // Low level Undead - Random
		else
			SpawnRandomMonster(s,"necro.scp","UNDEADLIST","1001"); // Med level Undead - Random
		sysmessage(s,"You have disturbed the rest of a vile undead creature.");
		break;
	case 8:
		SpawnRandomItem(s,1,"necro.scp","ITEMLIST","1000");
		sysmessage(s,"You unearthed a old weapon and placed it in your pack.");
		break;
	case 10:
		if(nFame<1000)
			SpawnRandomMonster(s,"necro.scp","UNDEADLIST","1001"); // Med level Undead - Random
		else
			SpawnRandomMonster(s,"necro.scp","UNDEADLIST","1002"); // High level Undead - Random
		sysmessage(s,"You have disturbed the rest of a vile undead creature.");
		break;
	case 12:
		if(nFame>1000)
			SpawnRandomMonster(s,"necro.scp","UNDEADLIST","1002"); // High level Undead - Random
		else
			SpawnRandomMonster(s,"necro.scp","UNDEADLIST","1001"); // Med level Undead - Random
		sysmessage(s,"You have disturbed the rest of a vile undead creature.");
		break;
	default:
		nRandnum=rand()%2;
		switch(nRandnum)
		{  
		case 1:
			nRandnum=rand()%12;
			switch(nRandnum)
			{
			case 0: iID=0x11; break;
			case 1: iID=0x12; break;
			case 2: iID=0x13; break;
			case 3: iID=0x14; break;
			case 4: iID=0x15; break;
			case 5: iID=0x16; break;
			case 6: iID=0x17; break;
			case 7: iID=0x18; break;
			case 8: iID=0x19; break;
			case 9: iID=0x1A; break;
			case 10: iID=0x1B; break;
			case 11: iID=0x1C; break;
			}
			Items->SpawnItem(s,1,NULL,0,0x1b,iID,0x00,0x00,1,1);
			sysmessage(s,"You have unearthed some old bones and placed them in your pack.");       
			break;
			default: // found an empty grave
				sysmessage(s,"This grave seems to be empty.");
		}
	}
}

//o--------------------------------------------------------------------------
//| Function    - cSkills::SmeltOre(UOXSOCKET s);
//| Date        - Unknown
//| Programmer  - Unknown
//| Modified    - Abaddon(February 19, 2000)
//o--------------------------------------------------------------------------
//| Purpose     - Rewritten to use case and structure, you'll find it is 
//|               easier to make it scriptable now. The structure is pretty 
//|               much all that'd be needed for any future ore->ingot conversions
//|               scripting the ore would probably be even simpler, requires 
//|               less info
//o--------------------------------------------------------------------------
void cSkills::SmeltOre( UOXSOCKET s )
{
	SERIAL anvilSerial;
	int anvil = 0;
	int smeltedItem = 0;

	int numore = 1;

	bool smelting = true;
	short int realID;
	CHARACTER chr = currchar[s];
	smeltedItem = chars[chr].smeltitem;	

	anvilSerial = calcserial( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	anvil = findbyserial( &itemsp[anvilSerial%HASHMAX], anvilSerial, 0 );				// Let's find our anvil
	
	struct mining_stuff
	{
		short int colour;				// colour of the ore, for colour of ingot
		short int minSkill;				// minimum skill needed to make the ingot
		char *name;						// name of the ingot
		bool foreign;					// if not iron, then it can print out that it's a stranger ore when failing
		short int itemID;				// the item ID of the target ingot
	} miningstuff;
	miningstuff.name = NULL;

	if( anvil != -1 )					// if we have an anvil
	{
		realID = (items[anvil].id1<<8) + items[anvil].id2;
		switch( realID )				// switch based upon the anvil's ID to see if it's a valid anvil
		{
		case 0x0FB1:
		case 0x197A:
		case 0x19A9:
		case 0x197E:
		case 0x1982:
		case 0x1986:
		case 0x198A:
		case 0x198E:
		case 0x1992:
		case 0x1996:
		case 0x199A:
		case 0x19A6:
		case 0x19A2:
		case 0x199E:
			if( iteminrange( s, anvil, 3 ) ) //Check if the forge is in range  
			{
				miningstuff.colour = (items[smeltedItem].color1<<8) + items[smeltedItem].color2;
				switch( miningstuff.colour )	// switch on colour of ore (unique)
				{
				case 0x0000:
					miningstuff.foreign = false;
					miningstuff.minSkill = 0;
					miningstuff.name = new char[5];
					strcpy( miningstuff.name, "Iron" );
					miningstuff.itemID = 0x1BF2;
					break;
				case 0x096D:
					miningstuff.foreign = true;
					miningstuff.minSkill = 850;
					miningstuff.name = new char[5];
					strcpy( miningstuff.name, "Gold" );
					miningstuff.colour = 0x0000;	// overridden because ingots are different type
					miningstuff.itemID = 0x1BEC;
					break;
				case 0x00F3:
					miningstuff.foreign = true;
					miningstuff.minSkill = 750;
					miningstuff.name = new char[7];
					strcpy( miningstuff.name, "Copper" );
					miningstuff.itemID = 0x1BE6;
					miningstuff.colour = 0x0000;
					break;
				case 0x038A:
					miningstuff.foreign = true;
					miningstuff.minSkill = 790;
					miningstuff.name = new char[7];
					strcpy( miningstuff.name, "Silver" );
					miningstuff.colour = 0x0000;
					miningstuff.itemID = 0x1BF8;
					break;
				case 0x0150:
					miningstuff.foreign = true;
					miningstuff.minSkill = 900;
					miningstuff.name = new char[8];
					strcpy( miningstuff.name, "Agapite" );
					miningstuff.itemID = 0x1BF8;
					break;
				case 0x0386:
					miningstuff.foreign = true;
					miningstuff.minSkill = 650;
					miningstuff.name = new char[11];
					strcpy( miningstuff.name, "Adamantium" );
					miningstuff.itemID = 0x1BF8;
					break;
				case 0x022F:
					miningstuff.foreign = true;
					miningstuff.minSkill = 950;
					miningstuff.name = new char[7];
					strcpy( miningstuff.name, "Verite" );
					miningstuff.itemID = 0x1BF8;
					break;
				case 0x02E7:
					miningstuff.foreign = true;
					miningstuff.minSkill = 800;
					miningstuff.name = new char[7];
					strcpy( miningstuff.name, "Bronze" );
					miningstuff.itemID = 0x1BF8;
					break;
				case 0x02C3:
					miningstuff.foreign = true;
					miningstuff.minSkill = 700;
					miningstuff.name = new char[8];
					strcpy( miningstuff.name, "Merkite" );
					miningstuff.itemID = 0x1BF8;
					break;
				case 0x0191:
					miningstuff.foreign = true;
					miningstuff.minSkill = 990;
					miningstuff.name = new char[8];
					strcpy( miningstuff.name, "Mythril" );
					miningstuff.itemID = 0x1BF8;
					break;
				default:
					miningstuff.foreign = false;
					miningstuff.minSkill = 0;
					miningstuff.name = new char[8];
					strcpy( miningstuff.name, "Unknown" );
					miningstuff.itemID = 0x1BF8;
					break;
				}
				char ingotString[100];
				if( miningstuff.foreign )	// if not iron, generally
				{
					short int playersSkill = chars[chr].skill[MINING];
					playersSkill += (short int)Races->getDamageFromSkill( MINING, chars[chr].race );	// apply racial skill bonus
					if( playersSkill < miningstuff.minSkill )
					{
						sysmessage( s, "You have no idea what to do with this strange ore" );
						return;
					}
				}
				if( !Skills->CheckSkill( chr, MINING, miningstuff.minSkill, 1000 ) )	// if we do not have minimum skill to use it
				{
					if( items[smeltedItem].amount == 1 )	// if only one ore, delete it
					{
						sysmessage( s, "Your hand slips and the last of your materials are destroyed." );
						return;
					}
					else
					{
						sysmessage( s, "Your hand slips and some of your materials are destroyed." );
						items[smeltedItem].amount /= 2;		// if more than one ore, halve it
															// potentially, you could make it random between half and full
					}
				}
				numore = items[smeltedItem].amount*2;		// not sure, think 2 ingots per large pile
				sprintf( ingotString, "%s Ingot", miningstuff.name );
				Items->SpawnItem( s, numore, ingotString, 1, (unsigned char)(miningstuff.itemID>>8), (unsigned char)(miningstuff.itemID%256), (unsigned char)(miningstuff.colour>>8), (unsigned char)(miningstuff.colour%256), 1, 1 );
				sysmessage(s,"You have smelted your ore.");
				sysmessage(s,"You place some %s ingots in your pack.", miningstuff.name );
				Items->DeleItem(chars[chr].smeltitem);	// delete the ore
				smelting = true;	// we're smelting
			}
			break;
		default:
			break;
		}     
	} 

	chars[chr].smeltitem=-1;        // let's clear out what we are smelting
	if(!smelting)					// if not smelting, let's tell them we can't smelt here!
		sysmessage(s,"You can't smelt here.");
	Weight->NewCalc( chr );  // Ison 2-20-99
	statwindow(s, chr );   // Ison 2-20-99
	if( miningstuff.name != NULL )
		delete [] miningstuff.name;
}

void cSkills::Wheel(int s, int mat)//Spinning wheel
{
	int i;
	
	int tailme=0;
	
	i = calcItemFromSer( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	if( i != -1 )
	{
		if((items[i].id1==0x10 && items[i].id2==0xA4)||
			(items[i].id1==0x10 && ((items[i].id2>=0xA5)||(items[i].id2<=0xA6))))
		{
			if(iteminrange(s,i,3))  
			{
				if (!Skills->CheckSkill(currchar[s],TAILORING, 0, 1000)) 
				{
					sysmessage(s,"You failed to spin your material.");
					return;
				}   
				sysmessage(s,"You have successfully spun your material.");
				
				if (mat==YARN)
				{
					strcpy(items[chars[currchar[s]].tailitem].name,"#");
					items[chars[currchar[s]].tailitem].id1=0x0E;
					items[chars[currchar[s]].tailitem].id2=0x1D;
					items[chars[currchar[s]].tailitem].amount=items[chars[currchar[s]].tailitem].amount*4;
				}
				else if (mat==THREAD)
				{
					strcpy(items[chars[currchar[s]].tailitem].name,"#");
					items[chars[currchar[s]].tailitem].id1=0x0F;
					items[chars[currchar[s]].tailitem].id2=0xA0;
					items[chars[currchar[s]].tailitem].amount=items[chars[currchar[s]].tailitem].amount*4;
				}
				
				items[chars[currchar[s]].tailitem].priv=items[chars[currchar[s]].tailitem].priv|0x01; // AntiChrist - moved here
																									  /*				for (m=0;m<now;m++) if (perm[m]) 
																									  {
																									  items[chars[currchar[s]].tailitem].priv=items[chars[currchar[s]].tailitem].priv|0x01;
																									  senditem(m,chars[currchar[s]].tailitem);
			}*/
				RefreshItem( chars[currchar[s]].tailitem ); // AntiChrist
				tailme=1;
			}
		}     
	}   
	chars[currchar[s]].tailitem=-1;        
	if(!tailme) sysmessage(s,"You cant tailor here.");
}

void cSkills::Loom(int s)
{
	int i;
	
	int tailme=0;
	i = calcItemFromSer( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	if( i != -1 )
	{
		if((items[i].id1==0x10 && items[i].id2==0x5F)||
			(items[i].id1==0x10 && ((items[i].id2>=0x60)||(items[i].id2<=0x61))))
		{
			if(iteminrange(s,i,3))  
			{
				if (!Skills->CheckSkill(currchar[s],TAILORING, 0, 1000)) 
				{
					sysmessage(s,"You failed to make cloth.");
					return;
				}   
				sysmessage(s,"You have made your cloth.");
				
				strcpy(items[chars[currchar[s]].tailitem].name,"#");
				items[chars[currchar[s]].tailitem].id1=0x17;
				items[chars[currchar[s]].tailitem].id2=0x5D;
				items[chars[currchar[s]].tailitem].priv=items[chars[currchar[s]].tailitem].priv|0x01;
				items[chars[currchar[s]].tailitem].amount=items[chars[currchar[s]].tailitem].amount*2;
				
				RefreshItem( chars[currchar[s]].tailitem ); // AntiChrist
				tailme=1;
			}
		}     
	}   
	chars[currchar[s]].tailitem=-1;        
	if(!tailme) sysmessage(s,"You cant tailor here.");
}

void cSkills::CookMeat(int s)
{
	int i;
	int tailme=0;
	i = calcItemFromSer( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	if (i!=-1)
	{
		if( !(chars[currchar[s]].priv&0x01) && items[i].magic == 3 )
		{
			sysmessage( s, "That is locked down and you cannot use it" );
			return;
		}
		unsigned short realID = (items[i].id1<<8) + items[i].id2;
		if( realID >= 0x0DE3 && realID <= 0x0DE9 || realID == 0x0FAC || realID == 0x0FB1 ||
			realID >= 0x197A && realID <= 0x19B6 || realID >= 0x0461 && realID <= 0x0480 ||
			realID >= 0x0E31 && realID <= 0x0E33 || realID == 0x19BB || realID == 0x1F2B ||
			realID >= 0x092B && realID <= 0x0934 || realID >= 0x0937 && realID <= 0x0942 ||
			realID >= 0x0945 && realID <= 0x0950 || realID >= 0x0953 && realID <= 0x095E ||
			realID >= 0x0961 && realID <= 0x096C )
		{
			if(iteminrange(s,i,3))  
			{
				if( !CheckSkill( currchar[s],COOKING, 0, 1000 ) ) 
				{
					int amntDiff = 0;
					char burntName[50];
					amntDiff = 1+(rand() % (items[chars[currchar[s]].tailitem].amount));
					sprintf( burntName, "%i pieces of burnt meat", amntDiff );
					sysmessage(s,"You failed to cook the meat and drop some into the ashes.");
					items[chars[currchar[s]].tailitem].amount -= amntDiff;
					if (items[chars[currchar[s]].tailitem].amount <=0) Items->DeleItem(chars[currchar[s]].tailitem);
					else
						RefreshItem( chars[currchar[s]].tailitem ); // AntiChrist
					Items->SpawnItem( s, amntDiff, burntName, 0, 0x1E, 0xB0, 0, 0, 1, 1 );
					return;
				}   
				sysmessage(s,"You have cooked the meat,and it smells great.");
				
				strcpy(items[chars[currchar[s]].tailitem].name,"#");
				
				items[chars[currchar[s]].tailitem].id1=0x09;
				items[chars[currchar[s]].tailitem].id2=0xF2;
				items[chars[currchar[s]].tailitem].type=14;
				items[chars[currchar[s]].tailitem].priv |= 0x01; 
				
				RefreshItem( chars[currchar[s]].tailitem ); // AntiChrist
				tailme=1;
			}
		}     
	}   
	chars[currchar[s]].tailitem=-1;        
	if(!tailme) sysmessage(s,"You cant cook here.");
}

void cSkills::MakeDough(int s)
{
	//	unsigned int m;
	int i;
	int tailme=0;
	i = calcItemFromSer( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	if( i != -1 )
	{
		if((items[i].id1==0x10 && items[i].id2==0x3A))
		{
			if(iteminrange(s,i,3))  
			{
				if (!Skills->CheckSkill(currchar[s],COOKING, 0, 1000)) 
				{
					sysmessage(s,"You failed to mix, and spilt your water.");
					return;
				}   
				sysmessage(s,"You have mixed very well to make your dough.");
				
				strcpy(items[chars[currchar[s]].tailitem].name,"#");
				
				items[chars[currchar[s]].tailitem].id1=0x10;
				items[chars[currchar[s]].tailitem].id2=0x3D;
				items[chars[currchar[s]].tailitem].priv=items[chars[currchar[s]].tailitem].priv|0x01;
				items[chars[currchar[s]].tailitem].amount=items[chars[currchar[s]].tailitem].amount*2;
				
				RefreshItem( chars[currchar[s]].tailitem ); // AntiChrist
				tailme=1;
			}
		}     
	}   
	chars[currchar[s]].tailitem=-1;        
	if(!tailme) sysmessage(s,"You cant mix here.");
}

void cSkills::MakePizza(int s)
{
	//	unsigned int m;
	int i,serial;
	
	int tailme=0;
	
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	if (i!=-1)
	{
		if((items[i].id1==0x10 && items[i].id2==0x3D))
		{
			if(iteminrange(s,i,3))  
			{
				if (!Skills->CheckSkill(currchar[s],COOKING, 0, 1000)) 
				{
					sysmessage(s,"You failed to mix.");
					Items->DeleItem(i);
					return;
				}   
				sysmessage(s,"You have made your uncooked pizza, ready to place in oven.");
				
				strcpy(items[chars[currchar[s]].tailitem].name,"#");
				
				items[chars[currchar[s]].tailitem].id1=0x10;
				items[chars[currchar[s]].tailitem].id2=0x83;
				items[chars[currchar[s]].tailitem].priv=items[chars[currchar[s]].tailitem].priv|0x01;
				items[chars[currchar[s]].tailitem].amount=items[chars[currchar[s]].tailitem].amount*2;
				
				/*				for (m=0;m<now;m++) if (perm[m]) 
				{
				senditem(m,chars[currchar[s]].tailitem);
			}       */
				RefreshItem( chars[currchar[s]].tailitem ); // AntiChrist
				tailme=1;
				
			}
		}     
	}   
	chars[currchar[s]].tailitem=1;        
	if(!tailme) sysmessage(s,"You cant mix here.");
}

void cSkills::Hide(int s)
{
	if (chars[currchar[s]].attacker!=-1 && inrange1p(currchar[s],chars[currchar[s]].attacker))
	{
		sysmessage(s,"You cannot hide while fighting.");
		return;
	}
	if( chars[currchar[s]].hidden == 1 )
	{
		sysmessage( s, "You are already hidden" );
		return;
	}
	if (!Skills->CheckSkill(currchar[s],HIDING, 0, 1000)) 
	{
		sysmessage(s,"You are unable to hide here.");
		return;
	}   
	sysmessage(s,"You have hidden yourself well.");
	chars[currchar[s]].hidden = 1;
	chars[currchar[s]].stealth = -1;
	updatechar(currchar[s]);
}

void cSkills::Stealth(int s)//AntiChrist
{
	if (chars[currchar[s]].hidden==0) {
		sysmessage(s,"You must hide first.");
		return;
	}
	if (chars[currchar[s]].skill[HIDING]<700)
	{
		sysmessage(s,"You are not hidden well enough. Become better at hiding.");
		chars[currchar[s]].hidden=0;
		chars[currchar[s]].stealth=-1;
		updatechar(currchar[s]);
		return;
	}   		
	if (!Skills->CheckSkill(currchar[s],STEALTH, 0, 1000)) 
	{
		chars[currchar[s]].hidden=0;
		chars[currchar[s]].stealth=-1;
		updatechar(currchar[s]);
		return;
	}   
	sysmessage(s,"You can move tot steps unseen.");
	chars[currchar[s]].hidden = 1;
	chars[currchar[s]].stealth=0; //AntiChrist -- init. steps already done
	updatechar(currchar[s]);
}

void cSkills::TreeTarget(int s)
{
	int lumber=0;
	int packnum, px, py, cx, cy;
	static long int logtime[610][410];//see mine for values...they were 1000 also here
	static int logamount[610][410];
	int a, b, c;
	long int curtime=uiCurrentTime;
	
	if(resource.logarea<10) resource.logarea=10; //New -- Zippy
	
	if(logtime[0][0]==0)//First time done since server started
	{
		logtime[0][0]=17;//lucky number ;-)
		logamount[0][0]=resource.logs;
		printf("UOX3: Lumberjacking startup, setting tree values and times...");
		
		//for(a=1;a<410;a++)
		for(a=1;a<610;a++)//AntiChrist bug fix
		{
			for(b=1;b<410;b++)
			{
				logamount[a][b]=resource.logs;
				logtime[a][b] = (unsigned int)( curtime + resource.logtime * CLOCKS_PER_SEC );
			}
		}
		printf(" Done.\n");
	}
	
	if( buffer[s][11] == 0xFF && buffer[s][12] == 0xFF && buffer[s][13] == 0xFF && buffer[s][14] == 0xFF )
		return;
	
	px = ((buffer[s][0x0b]<<8) + (buffer[s][0x0c]%256 ));
	py = ((buffer[s][0x0d]<<8) + (buffer[s][0x0e]%256 ));
	cx = abs( chars[currchar[s]].x - px );
	cy = abs( chars[currchar[s]].y - py );

	char dir = chardirxyz( currchar[s], px, py, 0 );
	if( dir != -1 && chars[currchar[s]].dir != dir )
	{
		chars[currchar[s]].dir = dir;
		teleport( s );
	}
	if( cx >= 3 || cy >= 3 )
	{
		sysmessage( s, "You are too far away to reach that" );
		return;
	}
	
	a = chars[currchar[s]].x / resource.logarea; //Zippy
	b = chars[currchar[s]].y / resource.logarea;
	
	
	if(a>=610 || b>=410 ) return; // wih the previous a < 20 || b < 20, wind may not have worked right, as well as some dungeons
	//printf("DEBUG: Cell (%i,%i)  %i logs left [%i/%i logtime]\n",a,b,logamount[a][b],logtime[a][b],curtime);
	
	if(logtime[a][b]<=curtime)
	{
		//Find how many periods (defined in server.scp SECTION RESOURCES)
		// have been by, give 1 more ore for each period.
		for(c=0;c<resource.logs;c++)
		{
			if((logtime[a][b]+(c*resource.logtime*CLOCKS_PER_SEC))<=curtime && logamount[a][b]<resource.logs)
				logamount[a][b]++;
			else break;
		}
		logtime[a][b] = (unsigned int)( curtime + resource.logtime * CLOCKS_PER_SEC );//10 more mins
	}
	
	if(logamount[a][b]>resource.logs) logamount[a][b]=resource.logs;
	
	if(logamount[a][b]<=0)
	{
		sysmessage(s,"There is no more wood here to chop.");
		return;
	}
	
	packnum=packitem(currchar[s]);
	if (packnum==-1) {sysmessage(s,"No backpack to store logs"); return; } //LB
	
	if (chars[currchar[s]].onhorse) action(s,0x1C);
	else action(s,0x0D);
	soundeffect(s,0x01,0x3E);
	
	if (!Skills->CheckSkill(currchar[s],LUMBERJACKING, 0, 1000)) 
	{
		sysmessage(s,"You chop for a while, but fail to produce any usable wood.");
		if(logamount[a][b]>0 && rand()%2==1) logamount[a][b]--;//Randomly deplete resources even when they fail 1/2 chance you'll loose wood.
		return;
	}
	
	if(logamount[a][b]>0) logamount[a][b]--;
	
	if ((buffer[s][1]==1 || buffer[s][1]==0)&&(buffer[s][2]==0)
		&&(buffer[s][3]==1)&&(buffer[s][4]==0))
	{
		c=Items->SpawnItem(s,10,"#",1,0x1B,0xE0,0,0,1,1);
		if( c == -1 )
			return;
		if (items[c].amount>10) sysmessage(s,"You place more logs in your pack.");
		else sysmessage(s,"You place some logs in your pack.");
		
		lumber=1;
	}
}


/*
* I decided to base this on how OSI will be changing detect hidden. 
* The greater your skill, the more of a range you can detect from target position.
* Hiders near the center of the detect circle are easier to detect than ones near
* the edges of the detect circle. Also low skill detecters cannot find high skilled
* hiders as easily as low skilled hiders.
*/

void cSkills::DetectHidden(int s)
{
	if (buffer[s][11]==0xFF && buffer[s][12]==0xFF && buffer[s][13]==0xFF && buffer[s][14]==0xFF) return;
	
	unsigned int i;
	int x,y,z,dx,dy,k,j,low;
	double c,range;         //int is too restricting
	
	x=(buffer[s][11]<<8)+buffer[s][12];
	y=(buffer[s][13]<<8)+buffer[s][14];
	z=buffer[s][16];
	
	j=chars[currchar[s]].skill[DETECTINGHIDDEN];
	
	range = (j*j/1.0E6)*Races->getVisRange( chars[currchar[s]].race ) ;     // this seems like an ok formula
	
	//for (i=0;i<charcount;i++) //Zippy
	//{	
	
	//Char mapRegions
	int	StartGrid=mapRegions->StartGrid(chars[currchar[s]].x,chars[currchar[s]].y);
	//int	getcell=mapRegions->GetCell(chars[currchar[s]].x,chars[currchar[s]].y);
	
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
				if (mapitem!=-1 && mapitem>=1000000)
				{
					i=mapchar;
					if (chars[i].hidden==1) // do not detect invis people only hidden ones
					{
						dx=abs(chars[i].x-x);
						dy=abs(chars[i].y-y);
#ifdef __NT__
						c=sqrt(dx*dx+dy*dy);
#else
						c=hypot(dx, dy);
#endif
						low = (int)(chars[i].skill[HIDING]*chars[i].skill[HIDING]/1E3 - (range*50/Races->getVisRange( chars[currchar[s]].race ) )*(range-c)/range);
						if (low<0) low=0;
						if (low>1000) low=1000;
						
						if ((Skills->CheckSkill(currchar[s],DETECTINGHIDDEN,low,1000))&&(c<=range))
						{
							chars[i].hidden=0;
							chars[i].stealth=-1; //AntiChrist
							updatechar(i);
							k=calcSocketFromChar(i);
							if (k!=-1)
								if ((perm[k])) sysmessage(k,"You were revealed!");
						}
						else sysmessage(s,"You fail to find anyone.");
					}
				}//if mapitem
			} while (mapitem!=-1);
		}
	}
}

void cSkills::PeaceMaking(int s)
{
	unsigned int i;
	int inst, res1, res2, j;
	inst=Skills->GetInstrument(s);
	if (inst==-1) 
	{
		sysmessage(s, "You do not have an instrument to play on!");
		return;
	}
	res1=Skills->CheckSkill(currchar[s], PEACEMAKING, 0, 1000);
	res2=Skills->CheckSkill(currchar[s], MUSICIANSHIP, 0, 1000);
	if (res1 && res2)
	{
		Skills->PlayInstrumentWell(s, inst);
		sysmessage(s, "You play your hypnotic music, stopping the battle.");
		//for (i=0;i<charcount;i++)
		//{
		
		//Char mapRegions
		int	StartGrid=mapRegions->StartGrid(chars[currchar[s]].x,chars[currchar[s]].y);
		
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
						if (inrange1p(i, currchar[s]) && chars[i].war)
						{
							i=mapchar;
							j=calcSocketFromChar(i);
							if (j!=-1)
								if (perm[j]) sysmessage(j, "You hear some lovely music, and forget about fighting.");
								if (chars[i].war) npcToggleCombat(i);
								chars[i].targ=-1;
								chars[i].attacker=-1;
								chars[i].attackfirst=0;
						}
					}//mapitem
				} while (mapitem!=-1);
			}//for a<3
		}//for checkgrid
	} else 
	{
		Skills->PlayInstrumentPoor(s, inst);
		sysmessage(s, "You attempt to calm everyone, but fail.");
	}
}

void cSkills::PlayInstrumentWell( UOXSOCKET s, ITEM i )
{
	switch(items[i].id2)
	{
	case 0x9C:	soundeffect2( currchar[s], 0x00, 0x38 );		break;
	case 0x9D:
	case 0x9E:	soundeffect2( currchar[s], 0x00, 0x52 );		break;
	case 0xB1:	soundeffect2( currchar[s], 0x00, 0x43 );		break;
	case 0xB2:	soundeffect2( currchar[s], 0x00, 0x45 );		break;
	case 0xB3:
	case 0xB4:	soundeffect2( currchar[s], 0x00, 0x4C );		break;
	default:
		printf("ERROR: Fallout of switch statement without default. skills.cpp, Skills->PlayInstrumentWell()/n"); //Morrolan
		return;
		
	}
}

void cSkills::PlayInstrumentPoor( UOXSOCKET s, int i)
{
	switch( items[i].id2 )
	{
	case 0x9C:	soundeffect2( currchar[s], 0x00, 0x39 );		break;
	case 0x9D:
	case 0x9E:	soundeffect2( currchar[s], 0x00, 0x53 );		break;
	case 0xB1:	soundeffect2( currchar[s], 0x00, 0x44 );		break;
	case 0xB2:	soundeffect2( currchar[s], 0x00, 0x46 );		break;
	case 0xB3:
	case 0xB4:	soundeffect2( currchar[s], 0x00, 0x4D );		break;
	default:
		printf("ERROR: Fallout of switch statement without default. skills.cpp, Skills->PlayInstrumentPoor()/n"); //Morrolan
		return;
	}
}

int cSkills::GetInstrument(int s)
{
	int i, x,serial,serhash,ci;
	x=packitem(currchar[s]);
	if (x==-1) return -1; //LB
	serial=items[x].serial;
	serhash=serial%HASHMAX;
	for (ci=0;ci<contsp[serhash].max;ci++)
	{
		i=contsp[serhash].pointer[ci];
		
		if (i!=-1)
			if ((items[i].contserial==items[x].serial) &&
				(items[i].id1==0x0E) && (items[i].id2==0x9C || items[i].id2==0x9D || 
				items[i].id2==0x9E || items[i].id2==0xB1 || items[i].id2==0xB2 || 
				items[i].id2==0xB3 || items[i].id2==0xB4)) 
			{
				return i;
			}
	}
	return -1;
}

void cSkills::ProvocationTarget1( UOXSOCKET s )
{
	if( buffer[s][7]==0xFF && buffer[s][8]==0xFF && buffer[s][9]==0xFF && buffer[s][10]==0xFF ) return;
	
	int serial = calcserial( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	if( findbyserial( &charsp[serial%HASHMAX], serial, 1 ) == -1 ) return;
	int inst;
	inst=Skills->GetInstrument(s);
	if (inst==-1) 
	{
		sysmessage(s, "You do not have an instrument to play on!");
		return;
	}
	addid1[s]=buffer[s][7];
	addid2[s]=buffer[s][8];
	addid3[s]=buffer[s][9];
	addid4[s]=buffer[s][10];
	
	if (calcSocketFromChar(calcCharFromSer(addid1[s], addid2[s], addid3[s], addid4[s]))!=-1)
	{
		sysmessage(s, "You cannot provoke other players.");
	}
	else
	{
		target(s, 0, 1, 0, 80, "You play your music, inciting anger, and your target begins to look furious.  Whom do you wish it to attack?");
		Skills->PlayInstrumentWell(s, inst);
	}
}

void cSkills::EnticementTarget1( UOXSOCKET s )
{
	if( buffer[s][7]==0xFF && buffer[s][8]==0xFF && buffer[s][9]==0xFF && buffer[s][10]==0xFF ) return;
	
	int serial = calcserial( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	if( findbyserial( &charsp[serial%HASHMAX], serial, 1 ) == -1 ) return;
	int inst;
	inst=Skills->GetInstrument(s);
	if (inst==-1) 
	{
		sysmessage(s, "You do not have an instrument to play on!");
		return;
	}
	addid1[s]=buffer[s][7];
	addid2[s]=buffer[s][8];
	addid3[s]=buffer[s][9];
	addid4[s]=buffer[s][10];
	if (calcSocketFromChar(calcCharFromSer(addid1[s], addid2[s], addid3[s], addid4[s]))!=-1)
	{
		sysmessage(s, "You cannot entice other players.");
	}
	else
	{
		target(s, 0, 1, 0, 82, "You play your music, luring them near.  Whom do you wish them to follow?");
		Skills->PlayInstrumentWell(s, inst);
	}
}

void cSkills::EnticementTarget2( UOXSOCKET s )
{
	if( buffer[s][7]==0xFF && buffer[s][8]==0xFF && buffer[s][9]==0xFF && buffer[s][10]==0xFF ) return;
	
	int serial = calcserial( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	if( findbyserial( &charsp[serial%HASHMAX], serial, 1 ) == -1 ) return;
	int inst, target, res1, res2;
	inst=Skills->GetInstrument(s);
	if (inst==-1) 
	{
		sysmessage(s, "You do not have an instrument to play on!");
		return;
	}
	res1=Skills->CheckSkill(currchar[s], ENTICEMENT, 0, 1000);
	res2=Skills->CheckSkill(currchar[s], MUSICIANSHIP, 0, 1000);
	if (res1 && res2)
	{
		if( region[chars[currchar[s]].region].priv&0x01 )
			Combat->SpawnGuard( currchar[s], currchar[s], chars[currchar[s]].x+1, chars[currchar[s]].y, chars[currchar[s]].z );
		target = calcCharFromSer(addid1[s], addid2[s], addid3[s], addid4[s]);
		chars[target].ftarg=calcCharFromSer(buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10]);
		chars[target].npcWander=1;
		sysmessage(s, "You play your hypnotic music, luring them near your target.");
		Skills->PlayInstrumentWell(s, inst);
	}
	else 
	{
		sysmessage(s, "Your music fails to attract them.");
		Skills->PlayInstrumentPoor(s, inst);
	}
}

void cSkills::ProvocationTarget2( UOXSOCKET s )
{
	if( buffer[s][7]==0xFF && buffer[s][8]==0xFF && buffer[s][9]==0xFF && buffer[s][10]==0xFF ) return;
	
	int serial = calcserial( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	if( findbyserial( &charsp[serial%HASHMAX], serial, 1 ) == -1 ) return;
	
	int inst, target, target2, res1, res2;
	unsigned int i;
	inst=Skills->GetInstrument(s);
	if (inst==-1) 
	{
		sysmessage(s, "You do not have an instrument to play on!");
		return;
	}
	target=calcCharFromSer(addid1[s], addid2[s], addid3[s], addid4[s]);
	target2=calcCharFromSer(buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10]);
	if (target==target2)
	{
		sysmessage(s, "Silly bard!  You can't get something to attack itself.");
		return;
	}
	res1=Skills->CheckSkill(currchar[s], PROVOCATION, 0, 1000);
	res2=Skills->CheckSkill(currchar[s], MUSICIANSHIP, 0, 1000);
	if (res2)
	{
		Skills->PlayInstrumentWell(s, inst);
		if (res1)
		{
			if( region[chars[currchar[s]].region].priv&0x01 )
				Combat->SpawnGuard( currchar[s], currchar[s], chars[currchar[s]].x+1, chars[currchar[s]].y, chars[currchar[s]].z ); // ripper
			sysmessage(s, "Your music succeeds as you start a fight.");
			chars[target].targ=target2;
			chars[target2].targ=target;
			if ((chars[target].hidden)&&(!(chars[target].priv2&8)))
			{
				chars[target].hidden=0;
				chars[target].stealth=-1;//AntiChrist
				updatechar(target);
			}
			if (chars[target].med)
			{
				chars[target].med=0; //Morrolan - Meditation
			}
			if ((chars[target2].hidden)&&(!(chars[target2].priv2&8))) //Morrolan bugfix (was target instead of target2)
			{
				chars[target2].hidden=0;
				chars[target2].stealth=-1;
				updatechar(target2);
			}
			if (chars[target2].med)
			{
				chars[target2].med=0; //Morrolan - Meditation
			}
			chars[target].attackfirst=1;
			chars[target2].attackfirst=0;
			chars[target].attacker=target2;
			chars[target2].attacker=target;
			if (chars[target].npc)
			{
				if (!(chars[target].war)) npcToggleCombat(target);
				chars[target].npcmovetime=(unsigned int)((uiCurrentTime+double(NPCSPEED*CLOCKS_PER_SEC)));
			}
			if (chars[target2].npc)
			{
				if (!(chars[target2].war)) npcToggleCombat(target2);
				chars[target2].npcmovetime=(unsigned int)((uiCurrentTime+double(NPCSPEED*CLOCKS_PER_SEC)));
			}
			sprintf(temp, "* You see %s attacking %s *", chars[target].name, chars[target2].name);
			for (i=0;i<now;i++)
			{
				if( perm[i] && inrange1p( currchar[i], target ) )
				{
					itemmessage(i, temp, chars[target].ser1, chars[target].ser2,
						chars[target].ser3, chars[target].ser4);
				}
			}
		}
		else 
		{
			sysmessage(s, "Your music fails to incite enough anger.");
			target2=currchar[s];
			chars[target].targ=target2;
			chars[target2].targ=target;
			if ((chars[target].hidden)&&(!(chars[target].priv2&8)))
			{
				chars[target].hidden=0;
				chars[target].stealth=-1;
				updatechar(target);
			}
			if (chars[target].med)
			{
				chars[target].med=0; //Morrolan - Meditation
			}
			if ((chars[target2].hidden)&&(!(chars[target2].priv2&8))) //Morrolan bugfix (was target instead of target2)
			{
				chars[target2].hidden=0;
				chars[target2].stealth=-1;
				updatechar(target2);
			}
			if (chars[target2].med)
			{
				chars[target2].med=0; //Morrolan - Meditation
			}
			chars[target].attackfirst=1;
			chars[target2].attackfirst=0;
			chars[target].attacker=target2;
			chars[target2].attacker=target;
			if (chars[target].npc)
			{
				if (!(chars[target].war)) npcToggleCombat(target);
				chars[target].npcmovetime=(unsigned int)((uiCurrentTime+double(NPCSPEED*CLOCKS_PER_SEC)));
			}
			if (chars[target2].npc)
			{
				if (!(chars[target2].war)) npcToggleCombat(target2);
				chars[target2].npcmovetime=(unsigned int)((uiCurrentTime+double(NPCSPEED*CLOCKS_PER_SEC)));
			}
			sprintf(temp, "* You see %s attacking %s *", chars[target].name, chars[target2].name);
			for (i=0;i<now;i++)
			{
				if( perm[i] && inrange1p( currchar[i], target ) )
				{
					itemmessage(i, temp, chars[target].ser1, chars[target].ser2,
						chars[target].ser3, chars[target].ser4);
				}
			}
		}
	}
	else
	{
		Skills->PlayInstrumentPoor(s, inst);
		sysmessage(s, "You play rather poorly and to no effect.");
	}
}

void cSkills::AlchemyTarget(int s)
{
	int i, type,serial;
	
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	if (i!=-1)
	{
		type=(items[i].id1<<8)+items[i].id2;
		switch (type)
		{
		case 0x0F7B: // Agility,
			itemmenu(s, 991);
			break;
		case 0x0F84: // Cure, Garlic
			itemmenu(s, 992);
			break;
		case 0x0F8C: // Explosion, Sulfurous Ash
			itemmenu(s, 993);
			break;
		case 0x0F85: // Heal, Ginseng
			itemmenu(s, 994);
			break;
		case 0x0F8D: // Night sight
			itemmenu(s, 995);
			break;
		case 0x0F88: // Poison, Nightshade
			itemmenu(s, 996);
			break;
		case 0x0F7A: // Refresh, 
			itemmenu(s, 997);
			break;
		case 0x0F86: // Strength,
			itemmenu(s, 998);
			break;
		case 0x0E9B: // Mortar
			break;
		default:
			if((items[i].id1==0x1B)&&((items[i].id2>=0x11)&&(items[i].id2<=0x1C)))
			{
				MakeNecroReg(s,i,items[i].id1,items[i].id2);
				sysmessage(s,"You grind some bone into powder.");
				return; // Morrolan was break;
			}
			sysmessage(s, "That is not a valid reagent.");
			return;
		}
	}
}

void cSkills::DoPotion(int s, int type, int sub, int mortar)
{
	int success=0, nochoice=0;
	
	switch((type<<8)+sub)
	{
	case 0x0101://agility
		if (getamount(currchar[s], 0x0F, 0x7B)>=1)
		{
			success=1;
			sprintf(temp, "*%s starts grinding some blood moss in the mortar.*", chars[currchar[s]].name);
			delequan(currchar[s], 0x0F, 0x7B, 1);
		}
		break;
	case 0x0102://greater agility
		if (getamount(currchar[s], 0x0F, 0x7B)>=3)
		{
			success=1;
			sprintf(temp, "*%s starts grinding some blood moss in the mortar.*", chars[currchar[s]].name);
			delequan(currchar[s], 0x0F, 0x7B, 3);
		}
		break;
	case 0x0201://lesser cure
		if (getamount(currchar[s], 0x0F, 0x84)>=1)
		{
			success=1;
			sprintf(temp, "*%s starts grinding some garlic in the mortar.*", chars[currchar[s]].name);
			delequan(currchar[s], 0x0F, 0x84, 1);
		}
		break;
	case 0x0202://cure
		if (getamount(currchar[s], 0x0F, 0x84)>=3)
		{
			success=1;
			sprintf(temp, "*%s starts grinding some garlic in the mortar.*", chars[currchar[s]].name);
			delequan(currchar[s], 0x0F, 0x84, 3);
		}
		break;
	case 0x0203://greater cure
		if (getamount(currchar[s], 0x0F, 0x84)>=6)
		{
			success=1;
			sprintf(temp, "*%s starts grinding some garlic in the mortar.*", chars[currchar[s]].name);
			delequan(currchar[s], 0x0F, 0x84, 6);
		}
		break;
	case 0x0301://lesser explosion
		if (getamount(currchar[s], 0x0F, 0x8C)>=3)
		{
			success=1;
			sprintf(temp, "*%s starts grinding some sulfurous ash in the mortar.*", chars[currchar[s]].name);
			delequan(currchar[s], 0x0F, 0x8C, 3);
		}
		break;
	case 0x0302://explosion
		if (getamount(currchar[s], 0x0F, 0x8C)>=5)
		{
			success=1;
			sprintf(temp, "*%s starts grinding some sulfurous ash in the mortar.*", chars[currchar[s]].name);
			delequan(currchar[s], 0x0F, 0x8C, 5);
		}
		break;
	case 0x0303://greater explosion
		if (getamount(currchar[s], 0x0F, 0x8C)>=10)
		{
			success=1;
			sprintf(temp, "*%s starts grinding some sulfurous ash in the mortar.*", chars[currchar[s]].name);
			delequan(currchar[s], 0x0F, 0x8C, 10);
		}
		break;
	case 0x0401://lesser heal
		if (getamount(currchar[s], 0x0F, 0x85)>=1)
		{
			success=1;
			sprintf(temp, "*%s starts grinding some ginseng in the mortar.*", chars[currchar[s]].name);
			delequan(currchar[s], 0x0F, 0x85, 1);
		}
		break;
	case 0x0402://heal
		if (getamount(currchar[s], 0x0F, 0x85)>=3)
		{
			success=1;
			sprintf(temp, "*%s starts grinding some ginseng in the mortar.*", chars[currchar[s]].name);
			delequan(currchar[s], 0x0F, 0x85, 3);
		}
		break;
	case 0x0403://greater heal
		if (getamount(currchar[s], 0x0F, 0x85)>=7)
		{
			success=1;
			sprintf(temp, "*%s starts grinding some ginseng in the mortar.*", chars[currchar[s]].name);
			delequan(currchar[s], 0x0F, 0x85, 7);
		}
		break;
	case 0x0501://night sight
		if (getamount(currchar[s], 0x0F, 0x8D)>=1)
		{
			success=1;
			sprintf(temp, "*%s starts grinding some spider's silk in the mortar.*", chars[currchar[s]].name);
			delequan(currchar[s], 0x0F, 0x8D, 1);
		}
		break;
	case 0x0601://lesser poison
		if (getamount(currchar[s], 0x0F, 0x88)>=1)
		{
			success=1;
			sprintf(temp, "*%s starts grinding some nightshade in the mortar.*", chars[currchar[s]].name);
			delequan(currchar[s], 0x0F, 0x88, 1);
		}
		break;
	case 0x0602://poison
		if (getamount(currchar[s], 0x0F, 0x88)>=2)
		{
			success=1;
			sprintf(temp, "*%s starts grinding some nightshade in the mortar.*", chars[currchar[s]].name);
			delequan(currchar[s], 0x0F, 0x88, 2);
		}
		break;
	case 0x0603://greater poison
		if (getamount(currchar[s], 0x0F, 0x88)>=4)
		{
			success=1;
			sprintf(temp, "*%s starts grinding some nightshade in the mortar.*", chars[currchar[s]].name);
			delequan(currchar[s], 0x0F, 0x88, 4);
		}
		break;
	case 0x0604://deadly poison
		if (getamount(currchar[s], 0x0F, 0x88)>=8)
		{
			success=1;
			sprintf(temp, "*%s starts grinding some nightshade in the mortar.*", chars[currchar[s]].name);
			delequan(currchar[s], 0x0F, 0x88, 8);
		}
		break;
	case 0x0701://refresh
		if (getamount(currchar[s], 0x0F, 0x7A)>=1)
		{
			success=1;
			sprintf(temp, "*%s starts grinding some black pearl in the mortar.*", chars[currchar[s]].name);
			delequan(currchar[s], 0x0F, 0x7A, 1);
		}
		break;
	case 0x0702://total refreshment
		if (getamount(currchar[s], 0x0F, 0x7A)>=5)
		{
			success=1;
			sprintf(temp, "*%s starts grinding some black pearl in the mortar.*", chars[currchar[s]].name);
			delequan(currchar[s], 0x0F, 0x7A, 5);
		}
		break;
	case 0x0801://strength
		if (getamount(currchar[s], 0x0F, 0x86)>=2)
		{
			success=1;
			sprintf(temp, "*%s starts grinding some mandrake in the mortar.*", chars[currchar[s]].name);
			delequan(currchar[s], 0x0F, 0x86, 2);
		}
		break;
	case 0x0802://greater strength
		if (getamount(currchar[s], 0x0F, 0x86)>=5)
		{
			success=1;
			sprintf(temp, "*%s starts grinding some mandrake in the mortar.*", chars[currchar[s]].name);
			delequan(currchar[s], 0x0F, 0x86, 5);
		}
		break;
	default:
		nochoice=1;
 }
 if (success==0)
 {
	 if (!nochoice) sysmessage(s, "You do not have enough reagents for that potion.");
 } 
 else 
 {
	 npcemoteall(currchar[s], temp, 1);
	 tempeffect(currchar[s], currchar[s], 9, 0, 0, 0);
	 tempeffect(currchar[s], currchar[s], 9, 0, 3, 0);
	 tempeffect(currchar[s], currchar[s], 9, 0, 6, 0);
	 tempeffect(currchar[s], currchar[s], 9, 0, 9, 0);
	 tempeffect2(currchar[s], mortar, 10, type, sub, 0);
 }
}

void cSkills::CreatePotion(int s, char type, char sub, int mortar)
{
	int success=0;
	
	switch((256*type)+sub)
	{
	case 0x0101://agility
		success=Skills->CheckSkill(s, ALCHEMY, 151, 651);
		break;
	case 0x0102://greater agility
		success=Skills->CheckSkill(s, ALCHEMY, 351, 851);
		break;
	case 0x0201://lesser cure
		success=Skills->CheckSkill(s, ALCHEMY, 0, 500);
		break;
	case 0x0202://cure
		success=Skills->CheckSkill(s, ALCHEMY, 251, 751);
		break;
	case 0x0203://greater cure
		success=Skills->CheckSkill(s, ALCHEMY, 651, 1151);
		break;
	case 0x0301://lesser explosion
		success=Skills->CheckSkill(s, ALCHEMY, 51, 551);
		break;
	case 0x0302://explosion
		success=Skills->CheckSkill(s, ALCHEMY, 351, 851);
		break;
	case 0x0303://greater explosion
		success=Skills->CheckSkill(s, ALCHEMY, 651, 1151);
		break;
	case 0x0401://lesser heal
		success=Skills->CheckSkill(s, ALCHEMY, 0, 500);
		break;
	case 0x0402://heal
		success=Skills->CheckSkill(s, ALCHEMY, 151, 651);
		break;
	case 0x0403://greater heal
		success=Skills->CheckSkill(s, ALCHEMY, 551, 1051);
		break;
	case 0x0501://night sight
		success=Skills->CheckSkill(s, ALCHEMY, 0, 500);
		break;
	case 0x0601://lesser poison
		success=Skills->CheckSkill(s, ALCHEMY, 0, 500);
		break;
	case 0x0602://poison
		success=Skills->CheckSkill(s, ALCHEMY, 151, 651);
		break;
	case 0x0603://greater poison
		success=Skills->CheckSkill(s, ALCHEMY, 551, 1051);
		break;
	case 0x0604://deadly poison
		success=Skills->CheckSkill(s, ALCHEMY, 901, 1401);
		break;
	case 0x0701://refresh
		success=Skills->CheckSkill(s, ALCHEMY, 0, 500);
		break;
	case 0x0702://total refreshment
		success=Skills->CheckSkill(s, ALCHEMY, 251, 751);
		break;
	case 0x0801://strength
		success=Skills->CheckSkill(s, ALCHEMY, 251, 751);
		break;
	case 0x0802://greater strength
		success=Skills->CheckSkill(s, ALCHEMY, 451, 951);
		break;
	default:
		printf("ERROR: Fallout of switch statement without default. skills.cpp, createpotion()/n"); //Morrolan
	}
	if (success==0 && !(chars[s].priv&0x01))
	{
		sprintf(temp, "*%s tosses the failed mixture from the mortar, unable to create a potion from it.*", chars[s].name);
		npcemoteall(s, temp, 0);
		return;
	}
	items[mortar].type=17;
	items[mortar].more1=type;
	items[mortar].more2=sub;
	items[mortar].morex=chars[s].skill[ALCHEMY];
	
	if (!(getamount(s, 0x0F, 0x0E)>=1))
	{
		target(calcSocketFromChar(s), 0, 1, 0, 109, "Where is an empty bottle for your potion?");
	}
	else
	{
		// Dupois - Added pouring potion sfx
		// Added Oct 09, 1998
		soundeffect(s, 0x02, 0x40);  // Liquid sfx
		sprintf(temp, "*%s pours the completed potion into a bottle.*", chars[s].name);
		npcemoteall(s, temp, 0);
		delequan(s, 0x0F, 0x0E, 1);
		Skills->PotionToBottle(s, mortar);
	} 
}

void cSkills::BottleTarget(int s)
{
	int i,  mortar,serial;
	//	unsigned int j;
	
	
	mortar=calcItemFromSer(addid1[s], addid2[s], addid3[s], addid4[s]);
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	if (i!=-1)
		if ((items[i].id1==0x0F)&&(items[i].id2==0x0E))
		{
			if (items[i].amount==1) 
			{
				Items->DeleItem(i);
			}
			else 
			{
				items[i].amount--;
				//				for (j=0;j<now;j++) if (perm[j]) senditem(j,i);
				RefreshItem( i ); // AntiChrist
			}
			if( mortar == -1 ) return;
			if (items[mortar].type==17) 
			{
				sprintf(temp, "*%s pours the completed potion into a bottle.*", chars[currchar[s]].name);
				npcemoteall(currchar[s], temp, 0);
				Skills->PotionToBottle(currchar[s], mortar);
			}
		}
		return;
}

void cSkills::PotionToBottle(int s, int mortar)
{
	//	unsigned int i;
	int c,/* p,*/ x, y;
	x=rand()%80;
	y=rand()%80;
	// p=packitem(s);
	
	c=Items->SpawnItem(calcSocketFromChar(s),s,1,"#",0,0x09,0x15,0,0,1,0);
	if( c == -1 ) return;
	items[c].type=19;
	items[c].morex=items[mortar].morex;
	items[c].morey=items[mortar].more1;
	items[c].morez=items[mortar].more2;
	items[c].weight = 100; // Ripper 11-25-99
	
	switch((256*items[mortar].more1)+items[mortar].more2)
	{
	case 0x0101:
		strcpy(items[c].name, "an agility potion");
		items[c].id1=0x0F;
		items[c].id2=0x08;
		break;
	case 0x0102:
		strcpy(items[c].name, "a greater agility potion");
		items[c].id1=0x0F;
		items[c].id2=0x08;
		break;
	case 0x0201:
		strcpy(items[c].name, "a lesser cure potion");
		items[c].id1=0x0F;
		items[c].id2=0x07;
		break;
	case 0x0202:
		strcpy(items[c].name, "a cure potion");
		items[c].id1=0x0F;
		items[c].id2=0x07;
		break;
	case 0x0203:
		strcpy(items[c].name, "a greater cure potion");
		items[c].id1=0x0F;
		items[c].id2=0x07;
		break;
	case 0x0301:
		strcpy(items[c].name, "a lesser explosion potion");
		items[c].id1=0x0F;
		items[c].id2=0x0D;
		break;
	case 0x0302:
		strcpy(items[c].name, "an explosion potion");
		items[c].id1=0x0F;
		items[c].id2=0x0D;
		break;
	case 0x0303:
		strcpy(items[c].name, "a greater explosion potion");
		items[c].id1=0x0F;
		items[c].id2=0x0D;
		break;
	case 0x0401:
		strcpy(items[c].name, "a lesser heal potion");
		items[c].id1=0x0F;
		items[c].id2=0x0C;
		break;
	case 0x0402:
		strcpy(items[c].name, "a heal potion");
		items[c].id1=0x0F;
		items[c].id2=0x0C;
		break;
	case 0x0403:
		strcpy(items[c].name, "a greater heal potion");
		items[c].id1=0x0F;
		items[c].id2=0x0C;
		break;
	case 0x0501:
		strcpy(items[c].name, "a night sight potion");
		items[c].id1=0x0F;
		items[c].id2=0x06;
		break;
	case 0x0601:
		strcpy(items[c].name, "a lesser poison potion");
		items[c].id1=0x0F;
		items[c].id2=0x0A;
		break;
	case 0x0602:
		strcpy(items[c].name, "a poison potion");
		items[c].id1=0x0F;
		items[c].id2=0x0A;
		break;
	case 0x0603:
		strcpy(items[c].name, "a greater poison potion");
		items[c].id1=0x0F;
		items[c].id2=0x0A;
		break;
	case 0x0604:
		strcpy(items[c].name, "a deadly poison potion");
		items[c].id1=0x0F;
		items[c].id2=0x0A;
		break;
	case 0x0701:
		strcpy(items[c].name, "a refresh potion");
		items[c].id1=0x0F;
		items[c].id2=0x0B;
		break;
	case 0x0702:
		strcpy(items[c].name, "a total refreshment potion");
		items[c].id1=0x0F;
		items[c].id2=0x0B;
		break;
	case 0x0801:
		strcpy(items[c].name, "a strength potion");
		items[c].id1=0x0F;
		items[c].id2=0x09;
		break;
	case 0x0802:
		strcpy(items[c].name, "a greater strength potion");
		items[c].id1=0x0F;
		items[c].id2=0x09;
		break;
	default:
		printf("ERROR: Fallout of switch statement without default. skills.cpp, portiontobottle()/n"); //Morrolan
 }
 // for(i=0;i<now;i++) if (perm[i]) senditem(i, c);
	Items->GetScriptItemSetting( c );
	if( !(chars[s].priv&0x01 ) )
	{
		strcpy( items[c].creator, chars[s].name );
		if( chars[s].skill[ALCHEMY]>950 ) items[c].madewith = ALCHEMY+1;
		else items[c].madewith = 0-ALCHEMY-1;
	}
	else
	{
		items[c].creator[0]='\0';
		items[c].madewith = 0;
	}
	RefreshItem( c ); // AntiChrist
	items[mortar].type=0;
	return;
}

char cSkills::CheckSkill(int s, int sk, int low, int high )
{
	char skillused=0;
	float range=800.00, randnum1, randnum, charrange;
	//	char temp[256];
	
	if( chars[s].dead ) // fix for magic resistance exploit and probably others too, LB
	{
		sysmessage( calcSocketFromChar( s ), "Ghosts can't train %s", skillname[sk] );
		return 0;
	}
	
	if( chars[s].commandLevel >= 2 ) // real GM check
		return 1;
	
	if( high > 1200 ) 
		high = 1200;
	if( chars[s].skill[sk] < low )
	{
		skillused = 0;
		return skillused;	// don't even attempt skill gain
	}
	else if( chars[s].skill[sk] > high )
		skillused = 1;
	else
	{
		charrange= (float) (chars[s].skill[sk]-low);
		if(charrange<0) charrange=0;
		//	randnum1=(range/(high-low));
		randnum1=( range / ( high - low + Races->getDamageFromSkill( sk, chars[s].race ) ) );
		randnum=(randnum1*charrange)+100;
		if (randnum>900) randnum=900;
		randnum1 = rand()%1000;
		
		if( randnum >= randnum1 ) 
			skillused = 1; // Changed by Magius(CHE)
	}
	
	if( chars[s].baseskill[sk] < high )
	{
//		if (sk!=MAGERY || (sk==MAGERY && currentSpellType[s]==0))
//		{
			if( Skills->AdvanceSkill( s, sk, skillused ) )
			{
				Skills->updateSkillLevel( s, sk ); 
				updateskill( calcSocketFromChar( s ), sk );
			}
//		}
	}
	//	if (skillused) skillfreq(s, sk); // Morrolan - stat/skill cap
	return skillused;
}          

char cSkills::AdvanceSkill(int s, int sk, char skillused)
{
	int i=0, retval, incval;
	int adIndx;
	
	adIndx = skill[sk].advance_index;
	while (statadvance[1+i+adIndx].skill==sk && 
		statadvance[1+i+adIndx].base<=chars[s].baseskill[sk])
	{
		i++;
	}
	if( skillused )
		incval = (statadvance[i+adIndx].success)*10;
	else
		incval = (statadvance[i+adIndx].failure)*10;
	retval=0;// current lock mode (0 = none (up), 1 = down, 2 = locked)
	if (incval>rand()%1000)
	{
		retval=1;
		if( rand()%100 <= 10 )
		{
			if( chars[s].lockState[sk] == 1 && chars[s].baseskill[sk] > 0 )
			{
				chars[s].baseskill[sk]--;
				retval=1;
			}
		}
		else //its up only, so let it go up!
		{
			if( chars[s].lockState[sk] == 0 )
			{
				Atrophy(s, sk);
				retval=1;
			}
		}
	}
	
	if (retval && chars[s].lockState[sk] != 2 ) // if it's locked, stats can't advance
	{
		Skills->AdvanceStats(s, sk);
	}
	return retval;
}


//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Atrophy(CHARACTER c, unsigned short sk)
//|   Date        :  Jan 29, 2000
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Do atrophy for player c:
//|						find sk in our cronological list of atrophy skills, 
//|						move it to the front, check total aginst skillcap to 
//|						see if we need to lower a skill, if we do, again search 
//|						skills for a skill that can be lowered, if one is found
//|						lower it and increase sk, if we can't find one, do 
//|						nothing if atrophy is not need, increase sk.
//o---------------------------------------------------------------------------o
void cSkills::Atrophy( CHARACTER c, unsigned short sk )
{
	unsigned int a=0, ttl = 0, rem=0;
	unsigned short atrop[ALLSKILLS+1];
	signed short toDec=-1, s=calcSocketFromChar(c);
		
	if (chars[c].npc || chars[c].priv&1 || chars[c].priv&80 || s==-1)
	{//gms,cns,npcs don't need it
		chars[c].baseskill[sk]++;
		if ( s != -1)
			updateskill(s, sk);
		return;
	}

	srand(getclock());//randomize
	
	atrop[ALLSKILLS]=0;//set the last of out copy array
	memcpy(atrop, chars[c].atrophy, (ALLSKILLS)*sizeof(unsigned short));//copy in our atrophy
	
	for (a=TRUESKILLS;a>0;a--)
	{//add up skills and find the one being increased
		if ( chars[c].baseskill[chars[c].atrophy[a-1]]>0 && chars[c].lockState[chars[c].atrophy[a-1]]==1 && chars[c].atrophy[a-1]!=sk)
			toDec = chars[c].atrophy[a-1];//we found a skill that can be decreased, save it for later.

		ttl += chars[c].baseskill[a-1];
		atrop[a]=atrop[a-1];
		if ( atrop[a] == sk )
			rem = a;//remember this number
	}

	atrop[0] = sk;//set the first one to our current skill
	
	//copy it back in
	if ( rem == ALLSKILLS )//it was last
	{
		memcpy(chars[c].atrophy, atrop, ALLSKILLS*sizeof(unsigned short));
	} else {//in the middle somewhere or first
		memcpy(chars[c].atrophy, atrop, (rem)*sizeof(unsigned short));//copy in the part before our skill
		memcpy(&chars[c].atrophy[rem], &atrop[rem+1], (ALLSKILLS-rem)*sizeof(unsigned short));//now copy in the part after
	}

	if ( rand()%(server_data.skillcap) <= ttl )
	{//if the rand is less than their total skills, they loose one.
		if ( toDec != -1 )
		{
			chars[c].baseskill[toDec]--;
			chars[c].baseskill[sk]++;
			updateskill(s, sk);
			updateskill(s, toDec);
			return;
		}
		//we couldn't find a skill to take away from
		return;
	} else {//they don't need to be atrophy'd
		chars[c].baseskill[sk]++;
		updateskill(s, sk);
		return;
	}
}

void cSkills::AdvanceStats(int s, int sk)
{
	int i,j;
	int update=0;
	unsigned int ttl = chars[s].st+chars[s].dx+chars[s].in;
	unsigned char u = 0;
	
	if( skill[sk].st > rand()%250 )
	{
		i=0;
		while (statadvance[1+i+skill[sk].advance_index].skill==sk && 
			statadvance[1+i+skill[sk].advance_index].base<=chars[s].st)
		{
			i++;
		}
		if((RandomNum(0, 1000)) < server_data.stat_advance)
		{
			chars[s].st++;
			update = 1;
			if( !chars[s].npc && !(chars[s].priv&0x01) )	// Need to check GM stuff
			{
				if( chars[s].st > Races->getSkill( STRENGTH, chars[s].race )	)// I believe that this will fix stat caps for races
				{
					chars[s].st = Races->getSkill( STRENGTH, chars[s].race );
					printf("Char %i is at stat limit for str[%i]\n", s, chars[s].st );
					update=0;
				}
			}
			u = STRENGTH;
		}
	}
	
	if( skill[sk].dx > rand()%250 )
	{
		i=0;
		while (statadvance[1+i+skill[sk].advance_index].skill==sk &&
			statadvance[1+i+skill[sk].advance_index].base<=chars[s].dx)
		{
			i++;
		}
		if((RandomNum(0, 1000)) < server_data.stat_advance)
		{
			chars[s].dx++;
			update=1;
			if( !chars[s].npc && !(chars[s].priv&0x01) )	// Need to check GM stuff
			{
				if( chars[s].dx > Races->getSkill( DEXTERITY, chars[s].race ) )
				{
					update=0;
					chars[s].dx = Races->getSkill( DEXTERITY, chars[s].race );
					printf("Char %i is at stat limit for dex[%i]\n", s, chars[s].dx );
				}
			}
			u = DEXTERITY;
		}
	}
	if( skill[sk].in > rand()%250 )
	{
		i=0;
		while (statadvance[1+i+skill[sk].advance_index].skill==sk &&
			statadvance[1+i+skill[sk].advance_index].base<=chars[s].in)
		{
			i++;
		}
		if((RandomNum(0, 1000)) < server_data.stat_advance)
		{
			chars[s].in++;
			update=1;
			if( !chars[s].npc && !(chars[s].priv&0x01) )	// Need to check GM stuff
			{
				if( chars[s].in > Races->getSkill( INTELLECT, chars[s].race ) )
				{
					update=0;
					chars[s].in = Races->getSkill( INTELLECT, chars[s].race );
					printf("Char %i is at stat limit for int[%i]\n", s, chars[s].in );
				}
			}
			u = INTELLECT;
		}
	}
	if( update )
	{
		if( rand()%(server_data.statcap) <= ttl && !(chars[s].npc || chars[s].priv&1 || chars[s].priv&80))
		{
			/* Let's not take away stats until we reach the stat cap. The reasoning for the -1 in the line below is to avoid introducing a new bug. If we use just >=, then we will atrophy at stat_cap - 1. If we use a simple = comparison, then someone with an odd situation that jumpstheir strength over 100 (as an example) would no longer atrophy in any stat. Hence, the total stats -1 -Gunther			*/
			if( (chars[s].in + chars[s].dx + chars[s].st - 1) >= server_data.statcap) 
			{
				switch( u )
				{
				case INTELLECT:
					if ( rand()%2 ) 
					{
						if ( chars[s].st>10 )
							chars[s].st--;
						else if ( chars[s].dx > 10 )
							chars[s].dx--;
						else
							chars[s].in--;//they can't decrease, so take back what they got.
					} else {
						if ( chars[s].dx>10 )
							chars[s].dx--;
						else if ( chars[s].st>10 )
							chars[s].st--;
						else
							chars[s].in--;//they can't decrease, so take back what they got.
					}
					break;
				case DEXTERITY:
					if ( rand()%2 ) 
					{
						if ( chars[s].st>10 )
							chars[s].st--;
						else if ( chars[s].in>10 )
							chars[s].in--;
						else
							chars[s].dx--;//they can't decrease, so take back what they got.
					} else {
						if ( chars[s].in>10 )
							chars[s].in--;
						else if ( chars[s].st>10 )
							chars[s].st--;
						else
							chars[s].dx--;//they can't decrease, so take back what they got.
					}
					break;
				case STRENGTH:
					if( rand()%2 ) 
					{
						if( chars[s].dx > 10 )
							chars[s].dx--;
						else if( chars[s].in > 10 )
							chars[s].in--;
						else
							chars[s].st--; // they can't decrease, so take back what they got.
					} 
					else 
					{
						if( chars[s].in > 10 )
							chars[s].in--;
						else if( chars[s].dx>10 )
							chars[s].dx--;
						else
							chars[s].st--;//they can't decrease, so take back what they got.
					}
					break;
				default:
					printf("STAT Atrophy error in char %i!\n", s);
				}
			}
		}

		if( ( j = calcSocketFromChar( s ) ) != -1 )
		{
			statwindow( j, s );
			for( i = 0; i < ALLSKILLS; i++ )
			{
				updateSkillLevel( s, i );
			}
		}
	}
}
void cSkills::CreateBandageTarget(int s)
{
	int i,c,serial;
	serial = calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i = calcItemFromSer( serial );
	if (i!=-1)
		if (((items[i].id1==0x0f)&&((items[i].id2>=0x95)&&(items[i].id2<=0x9c)))||
			((items[i].id1==0x17)&&((items[i].id2>=0x5d)&&(items[i].id2<=0x64))))
		{
			soundeffect(s,0x02,0x48);        
//			if(!Skills->CheckSkill(currchar[s],TAILORING, 0, 1000)) 
//			{
//				sysmessage(s,"You were unable to cut bandages.");
//			}
//			else
//			{
				sysmessage(s,"You cut some cloth into a bandage, and put it in your backpack");
				c=Items->SpawnItem(s,1,"clean bandages",1,0x0E,0x21,0,0,1,1);
				if( c == -1 ) return;
				items[c].att=9;
				//  EviLDeD  -  If this was a bolt of cloth then make sure to leave 49 cutcloth :)
				//  December 24, 1998
				if( items[i].id1==0x0f && ( (items[i].id2>=0x95)&&(items[i].id2<=0x9c) ) )
				{
					c=Items->SpawnItem(s,49,"Cut Cloth",1,0x17,0x5F,0,0,1,1);
					if( c == -1 ) return;
				}
				//  EviLDeD  -  End
//			}
			if(items[i].amount==1)
			{
				Items->DeleItem(i);
			}
			else
			{
				items[i].amount--;
			}
			return;
		}
		sysmessage(s,"You cannot cut bandages from that item.");
}

void cSkills::HealingSkillTarget(int s)
{
	int i, serial;
	serial = calcserial( buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10] );
	i = findbyserial(&charsp[serial%HASHMAX], serial, 1);
	if (i!=-1)
	{
		unsigned char targetSkill;
		short int charID = ( chars[i].id1<<8 ) + chars[i].id2;
		if( charID >= 0x0190 && charID <= 0x0193 || charID == 0x03DB ) //Used on human
		{
			targetSkill = HEALING;
		}
		else
			targetSkill = VETERINARY;
		if( chars[i].dead == 1 )
		{
			if( chars[currchar[s]].baseskill[targetSkill] >= 800 && chars[currchar[s]].baseskill[ANATOMY] >= 800 )
			{
				if( !CheckSkill( currchar[s], targetSkill, 800, 1000 ) || !CheckSkill( currchar[s], ANATOMY, 800, 1000 ) )
				{
					sysmessage( s, "You have not the ability to resurrect the ghost" );
					if( items[addmitem[s]].amount > 1 )
						items[addmitem[s]].amount--;
					else
						Items->DeleItem( addmitem[s] );	// we tried and failed, so remove bandage
					return;
				}
				tempeffect( currchar[s], i, 23, targetSkill, 0, 0, addmitem[s] );	// sets up timer for resurrect
				return;
			}
			sysmessage( s, "You are not skilled enough to resurrect" );
			return;
		}

		if( chars[i].poisoned > 0 )
		{
			if( chars[currchar[s]].baseskill[targetSkill] >= 600 && chars[currchar[s]].baseskill[ANATOMY] >= 600 )
			{
				if( !CheckSkill( currchar[s], targetSkill, 600, 1000 ) || !CheckSkill( currchar[s], ANATOMY, 600, 1000 ) )
				{
					sysmessage( s, "You fail to counter the poison" );
					if( items[addmitem[s]].amount > 1 )
						items[addmitem[s]].amount--;
					else
						Items->DeleItem( addmitem[s] );	// we tried and failed, so remove bandage
					return;
				}
				tempeffect( currchar[s], i, 24, targetSkill, 0, 0, addmitem[s] );
				return;
			}
			else
			{
				sysmessage( s, "You are not skilled enough to cure poison." );
				sysmessage( s, "The poison in your target's system counters the bandage's effect." );
			}
			return;
		}

		if( chars[i].hp == chars[i].st )
		{
			sysmessage(s,"That being is undamaged");
			return;
		}
		if( !( npcinrange( s, i, 1 ) ) )
		{
			sysmessage(s,"You are not close enough to apply the bandages.");
			return;
		}

		if( !CheckSkill( currchar[s], targetSkill, 0, 1000 ) )
		{
			if( targetSkill == HEALING )
				sysmessage( s, "You are not skilled enough to heal that person." );
			else
				sysmessage( s, "You are not skilled enough to heal that creature." );
			return;
		}
		tempeffect( currchar[s], i, 22, targetSkill, 0, 0, addmitem[s] );
		return;

	}
}

void cSkills::SpiritSpeak(int s)  // spirit speak time, on a base of 30 seconds + skill[SPIRITSPEAK]/50 + INT
{
	//      Unsure if spirit speaking should they attempt again?
	//      Suggestion: If they attempt the skill and the timer is !0 do not have it raise the skill
	
	if(!Skills->CheckSkill(currchar[s],SPIRITSPEAK, 0, 1000))
	{
		sysmessage(s,"You fail your attempt at contacting the netherworld.");
		return;
	}
	
	impaction(s,0x11);     // I heard there is no action...but I decided to add one
	soundeffect(s,0x02,0x4A); // only get the sound if you are successful
	sysmessage(s,"You establish a connection to the netherworld.");
	
	chars[currchar[s]].spiritspeaktimer = (unsigned int)(uiCurrentTime + CLOCKS_PER_SEC * ( spiritspeak_data.spiritspeaktimer + chars[currchar[s]].skill[SPIRITSPEAK] / 10 + chars[currchar[s]].in ) ); // spirit speak duration
}

void cSkills::ArmsLoreTarget(int s)
{
	int i, total,serial;
	float totalhp;
	char temp2[60];
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	if (i!=-1)
	{                                                                                             
		if ((items[i].def==0)||(items[i].pileable))
			if ((items[i].lodamage==0)&&(items[i].hidamage==0) && ((items[i].rank < 1) || ( items[i].rank > 9 ))) // by Magius(CHE)
			{
				sysmessage(s, "That does not appear to be a weapon.");
				return;
			}
			if(chars[currchar[s]].priv&1)
			{
				sprintf(temp, "Attack [%i] Defense [%i] Lodamage [%i] Hidamage [%i]", items[i].att, items[i].def, items[i].lodamage, items[i].hidamage);
				sysmessage(s, temp);
			}
			else
			{
				if (!Skills->CheckSkill(currchar[s],ARMSLORE, 0, 250)) sysmessage(s,"You are not certain...");
				else
				{
					if( items[i].maxhp==0)
					{
						sysmessage(s," Sorry this is a old item and it doesn't have maximum hp");
					}
					else {
						totalhp= (float) ((items[i].hp)/items[i].maxhp);
						
						strcpy(temp,"This items ");
						if      (totalhp>0.9) strcpy(temp2,"is brand new."); 
						else if (totalhp>0.8) strcpy(temp2,"is almost new.");
						else if (totalhp>0.7) strcpy(temp2,"is barely used, with a few nicks and scrapes.");
						else if (totalhp>0.6) strcpy(temp2,"is in fairly good condition.");
						else if (totalhp>0.5) strcpy(temp2,"suffered some wear and tear.");
						else if (totalhp>0.4) strcpy(temp2,"is well used.");
						else if (totalhp>0.3) strcpy(temp2,"is rather battered.");
						else if (totalhp>0.2) strcpy(temp2,"is somewhat badly damaged.");
						else if (totalhp>0.1) strcpy(temp2,"is flimsy and not trustworthy.");
						else                  strcpy(temp2,"is falling apart.");
						strcat(temp,temp2);
					}
					if (Skills->CheckSkill(currchar[s],ARMSLORE, 250, 500))
					{
						if (items[i].hidamage)
						{
							total = (items[i].hidamage + items[i].lodamage)/2;
							if      ( total > 26) strcpy(temp2," Would be extraordinarily deadly.");
							else if ( total > 21) strcpy(temp2," Would be a superior weapon.");
							else if ( total > 16) strcpy(temp2," Would inflict quite a lot of damage and pain."); 
							else if ( total > 11) strcpy(temp2," Would probably hurt your opponent a fair amount.");
							else if ( total > 6)  strcpy(temp2," Would do some damage.");
							else if ( total > 3)  strcpy(temp2," Would do minimal damage.");
							else                  strcpy(temp2," Might scratch your opponent slightly.");
							strcat(temp,temp2);
							
							if (Skills->CheckSkill(currchar[s], ARMSLORE, 500, 750))
							{
								if      (items[i].spd > 35) strcpy(temp2," And is very fast.");
								else if (items[i].spd > 25) strcpy(temp2," And is fast.");
								else if (items[i].spd > 15) strcpy(temp2," And is slow.");
								else                        strcpy(temp2," And is very slow.");
								strcat(temp,temp2);
							}
						}
						else
						{
							if      ( items[i].def> 12 ) strcpy(temp2," Is superbly crafted to provide maximum protection.");
							else if ( items[i].def> 10 ) strcpy(temp2," Offers excellent protection.");
							else if ( items[i].def> 8  ) strcpy(temp2," Is a superior defense against attack.");
							else if ( items[i].def> 6  ) strcpy(temp2," Serves as a sturdy protection.");
							else if ( items[i].def> 4  ) strcpy(temp2," Offers some protection against blows.");
							else if ( items[i].def> 2  ) strcpy(temp2," Provides very little protection.");
							else if ( items[i].def> 0  ) strcpy(temp2," Provides almost no protection.");
							else                         strcpy(temp2," Offers no defense against attackers.");
							strcat(temp,temp2);
						}
					}
					// Added by Magius(CHE) for Rank System
					if(( items[i].rank < 1 ) || ( items[i].rank > 10 )) items[i].rank = 10;
					*temp2 = '\0';
					if( Skills->CheckSkill( currchar[s], ARMSLORE, 250, 500 ) && server_data.rank_system == 1 )
					{
						switch( items[i].rank )
						{
						case 1:	strcpy( temp2, "It seems an item with no quality!" ); break;
						case 2:	strcpy( temp2, "It seems an item with very below standard quality!" ); break;
						case 3:	strcpy( temp2, "It seems an item below standard quality!" ); break;
						case 4:	strcpy( temp2, "It seems a weak quality item!" ); break;
						case 5:	strcpy( temp2, "It seems a standard quality item!" ); break;
						case 6:	strcpy( temp2, "It seems a nice quality item!" ); break;
						case 7:	strcpy( temp2, "It seems a good quality item!" ); break;
						case 8:	strcpy( temp2, "It seems a great quality item!" ); break;
						case 9:	strcpy( temp2, "It seems a beautiful quality item!" ); break;
						case 10:	strcpy( temp2, "It seems a perfect quality item!" ); break;
						}
					}
					// End Addon
					
					sysmessage(s, temp);
					if( temp2 ) sysmessage( s, temp2 ); // Added by Magius(CHE)
				}
			}
	}
}

void cSkills::ItemIdTarget(int s)
{
	int i,serial;
	
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	if (i!=-1)
	{
		if (!Skills->CheckSkill(currchar[s], ITEMID, 0, 250))
		{
			sysmessage(s, "You can't quite tell what this item is...");
		}
		else
		{
			if( items[i].corpse )
			{
				sysmessage( s, "You have to use your forensics evaluation skill to know more about this corpse." );
				return;
			}
			// Identify Item by Antichrist // Changed by Magius(CHE)
			if( Skills->CheckSkill( currchar[s], ITEMID, 250, 500 ))
				if( items[i].name2 && ( strcmp( items[i].name2, "#" ))) strcpy( items[i].name, items[i].name2 ); // Item identified! -- by Magius(CHE)
				//sprintf(temp, "This item appears to be called: %s", items[i].name);
				// ANTICHRIST -- FOR THE "#" BUG -- now you see the real name
				if(items[i].name[0]=='#') getname(i,temp2);
				else strcpy(temp2,items[i].name);
				sprintf(temp, "You found that this item appears to be called: %s", temp2);
				sysmessage(s, temp);
				
				// Show Creator by Magius(CHE)
				if( Skills->CheckSkill( currchar[s], ITEMID, 250, 500 ) )
				{
					if( strlen( items[i].creator ) > 0 )
					{
						if( items[i].madewith > 0 ) 
							sprintf( temp2, "It is %s by %s", skill[items[i].madewith-1].madeword, items[i].creator ); // Magius(CHE)
						else if( items[i].madewith < 0 )
							sprintf( temp2, "It is %s by %s", skill[0-items[i].madewith - 1].madeword, items[i].creator ); // Magius(CHE)
						else
							sprintf(temp2, "It is made by %s", items[i].creator ); // Magius(CHE)
					}
					else
						strcpy( temp2, "You don't know its creator!" );
				}
				else
					strcpy( temp2, "You can't know its creator!" );
				sysmessage( s, temp2 );
				// End Show creator
				
				if (!Skills->CheckSkill(currchar[s], ITEMID, 250, 500))
				{
					sysmessage(s, "You can't tell if it is magical or not.");
				}
				else
				{
					if(items[i].type!=15)
					{
						sysmessage(s, "This item has no hidden magical properties.");
					}
					else
					{
						if (!Skills->CheckSkill(currchar[s], ITEMID, 500, 1000))
						{
							sysmessage(s,"This item is enchanted with a spell, but you cannot determine which");
						}
						else
						{
							if (!Skills->CheckSkill(currchar[s], ITEMID, 750, 1100))
							{
								sprintf(temp, "It is enchanted with the spell %s, but you cannot determine how many charges remain.",spellname[(8*(items[i].morex-1))+items[i].morey-1]);
								sysmessage(s,temp);       
							}
							else
							{
								sprintf(temp, "It is enchanted with the spell %s, and has %d charges remaining.",spellname[(8*(items[i].morex-1))+items[i].morey-1],items[i].morez);
								sysmessage(s,temp);
							}
						}
					}
				}
		}
	}
}

void cSkills::Evaluate_int_Target(int s)
{
	int i, serial;
	char buf[75];
	
	
	if (!Skills->CheckSkill(currchar[s],EVALUATINGINTEL, 0, 1000)) 
	{
		sysmessage(s,"You are not certain..");
		return;
	}
	else 
	{
		serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
		i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
		if (i!=-1)
		{
			if ((chars[i].in == 0)) 
				sysmessage(s, "That does not appear to be a living being.");
			else
			{
				if (chars[i].in <= 10)
					strcpy(buf,"slightly less intelligent than a rock");
				else if (chars[i].in <= 20)
					strcpy(buf,"fairly stupid");
				else if (chars[i].in <= 30)
					strcpy(buf,"not the brightest");
				else if (chars[i].in <= 40)
					strcpy(buf,"about average");
				else if (chars[i].in <= 50)
					strcpy(buf,"moderately intelligent");
				else if (chars[i].in <= 60)
					strcpy(buf,"very intelligent");
				else if (chars[i].in <= 70)
					strcpy(buf,"extraordinarily intelligent");
				else if (chars[i].in <= 80)
					strcpy(buf,"like a formidable intellect, well beyond the ordinary");
				else if (chars[i].in <=90)
					strcpy(buf,"like a definite genius");
				else if (chars[i].in > 90)
					strcpy(buf,"superhumanly intelligent in a manner you cannot comprehend");
				sprintf(temp,"That person looks %s.", buf);
				sysmessage(s, temp);
			}
		}
	}
}

void cSkills::AnatomyTarget(int s)
{
	int i,serial;
	char buf[125];
	char buf2[125];
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
	if (i==-1)
	{
		sysmessage( s, "That is not a player!" );
		return;
	}
	if( chardist( i, currchar[s] ) >= combat.maxRange )
	{
		sysmessage( s, "You needs be closer to find out more about them" );
		return;
	}
	
	if (!Skills->CheckSkill(currchar[s],ANATOMY, 0, 1000)) 
	{
		sysmessage(s,"You are not certain..");
		return;
	}   
	else 
	{
		if ((chars[i].st == 0) && (chars[i].dx == 0)) 
			sysmessage(s, "That does not appear to be a living being.");
		else
		{
			if (chars[i].st <= 10)
				strcpy(buf,"rather feeble");
			else if (chars[i].st <= 20)
				strcpy(buf,"somewhat weak");
			else if (chars[i].st <= 30)
				strcpy(buf,"to be of normal strength");
			else if (chars[i].st <= 40)
				strcpy(buf,"somewhat strong"); 
			else if (chars[i].st <= 50)
				strcpy(buf,"very strong");
			else if (chars[i].st <= 60)
				strcpy(buf,"extremely strong"); 
			else if (chars[i].st <= 70)
				strcpy(buf,"extraordinarily strong");
			else if (chars[i].st <= 80)
				strcpy(buf,"as strong as an ox");
			else if (chars[i].st <= 90)
				strcpy(buf,"like one of the strongest people you have ever seen");
			else if (chars[i].st > 90)
				strcpy(buf,"superhumanly strong"); 
			if (chars[i].dx <= 10) 
				strcpy(buf2,"very clumsy");
			else if (chars[i].dx <= 20)
				strcpy(buf2,"somewhat uncoordinated");
			else if (chars[i].dx <= 30)
				strcpy(buf2,"moderately dexterous");
			else if (chars[i].dx <= 40)
				strcpy(buf2,"somewhat agile");
			else if (chars[i].dx <= 50)
				strcpy(buf2,"very agile");
			else if (chars[i].dx <= 60)
				strcpy(buf2,"extremely agile");
			else if (chars[i].dx <= 70)
				strcpy(buf2,"extraordinarily agile");
			else if (chars[i].dx <= 80)
				strcpy(buf2,"like they move like quicksilver");
			else if (chars[i].dx <= 90)
				strcpy(buf2,"like one of the fastest people you have ever seen");
			else if (chars[i].dx > 90) 
				strcpy(buf2, "superhumanly agile");
			sprintf(temp,"That person looks %s and %s.", buf, buf2); 
			sysmessage(s, temp);
		}
	}
}

void cSkills::TameTarget(int s)
{
	int i = calcCharFromSer( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	if( buffer[s][7] == 0xFF ) 
		return;
	if( i != -1 )
	{
		if( chars[i].npc == 1 && chardist( currchar[s], i ) <= 4 )
		{
			if( chars[i].taming > 1000 || chars[i].taming == 0 ) // If tame required is greater than 100.0 or is == 0.0, can't tame
			{
				sysmessage(s, "You can't tame that creature.");
				return;
			}
			if( chars[i].tamed && chars[i].ownserial == chars[currchar[s]].serial )	// You are the owner and it is already tamed
			{
				sysmessage( s, "You already control that creature!" );
				return;
			}
			if( chars[i].tamed )	// It's already tamed
			{
				if( chars[i].guildfealty != -1 && chars[i].guildfealty == chars[currchar[s]].serial ) // Previously owned it
				{
					sysmessage( s, "That creature is already controlled by another!" );
					return;
				}
			}
			for(int a=0;a<3;a++)
			{
				switch( RandomNum( 0, 3 ) )
				{
				case 0: npctalkall( currchar[s], "I've always wanted a pet like you.", 0 ); break;
				case 1: npctalkall( currchar[s], "Will you be my friend?", 0 ); break;
				case 2: sprintf( temp, "Here %s.", chars[i].name ); npctalkall( currchar[s], temp, 0 ); break;
				case 3: sprintf( temp, "Good %s.", chars[i].name ); npctalkall( currchar[s], temp, 0 ); break;
				}
			}
			if( !Skills->CheckSkill( currchar[s], TAMING, chars[i].taming, chars[i].taming * 2 ) )	// Requires chars[i].taming as min skill, but won't gain skill if you have more than 2 * chars[i].taming
			{
				sysmessage( s, "You were unable to tame it." );
				return;
			}   
			npctalk( s, currchar[s], "It seems to accept you as it's master!", 0 );
			chars[i].guildfealty = chars[i].ownserial;	// Save previous owner.  NPCs aren't in guilds, so it's a safe bet
			setserial( i, currchar[s], 5 );
			chars[i].npcWander = 0;
			chars[i].npcaitype = 0;
			chars[i].tamed = true;
			return;
		}
		if( chars[i].npc && chardist( currchar[s], i ) > 4 )
			sysmessage( s, "Creature is too far away!" );
		sysmessage( s, "You can't tame that!" );
	}
}

void cSkills::FishTarget(int s)
{
	if (buffer[s][11]==0xFF && buffer[s][12]==0xFF && buffer[s][13]==0xFF && buffer[s][14]==0xFF)
		return;
	
	short int targetX, targetY;
	signed char targetZ;
	short int distX, distY;

	targetX = (buffer[s][0x0B]<<8) + buffer[s][0x0C];		// store our target x y and z locations
	targetY = (buffer[s][0x0D]<<8) + buffer[s][0x0E];
	targetZ = buffer[s][0x10];

	distX = abs( chars[currchar[s]].x - targetX );			// find our distance
	distY = abs( chars[currchar[s]].y - targetY );

	unsigned char targetID1, targetID2;
	targetID1 = buffer[s][0x11];
	targetID2 = buffer[s][0x12];

	long targetItem;
	targetItem = calcItemFromSer( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	bool validLocation = false;
	if( targetItem != -1 )
	{
		validLocation = Map->IsTileWet( (items[targetItem].id1<<8) + items[targetItem].id2 );
	}
	else if( targetID1 != 0 && targetID2 != 0 )
	{
		MapStaticIterator msi( targetX, targetY );
		tile_st tile;
		staticrecord *stat;
		while ((( stat = msi.Next()) != NULL) && !validLocation )
		{
			msi.GetTile(&tile);
			if( targetZ == stat->zoff && (tile.flag1&0x80) == 0x80 )	// right place, and wet
				validLocation = true;
		}
	}
	else		// or it could be a map only
	{  // manually calculating the ID's if a maptype
		map_st map1;
		land_st land;
		map1 = Map->SeekMap0( targetX, targetY );
		Map->SeekLand( map1.id, &land );
		if( (land.flag1&0x80) == 0x80 )		// is wet
			validLocation = true; 
	}
	if( validLocation )
	{
		if( distX > 6 || distY > 6 )
		{
			sysmessage( s, "You are too far away from the water" );
			return;
		}
		if( chars[currchar[s]].z < targetZ )
		{
			sysmessage( s, "You cannot fish above you!" );
			return;
		}
		if( chars[currchar[s]].stm - 2 <= 2 )
		{
			sysmessage( s, "You are too tired to fish" );
			return;
		}
		chars[currchar[s]].stm -= 2;
		if( chars[currchar[s]].onhorse )	// do action and sound
			action( s, 0x1A );
		else
			action( s, 0x0B );
		float baseTime;
		baseTime = fishing_data.basetime / 25;
		baseTime += RandomNum( 0, fishing_data.randomtime / 15 );
		chars[currchar[s]].fishingtimer = (unsigned int)(uiCurrentTime + (double)(baseTime * CLOCKS_PER_SEC ) );
		soundeffect(s, 0x02, 0x3F );
	}
	else
		sysmessage( s, "You can't fish there!" );
}

void cSkills::Fish(unsigned int i)
{
	unsigned char idnum = 0xCF;
	int s = calcSocketFromChar( i );
	if( !CheckSkill( i, FISHING, 0, 1000 ) ) 
	{
		sysmessage( s, "You failed to catch anything!" );
		return;
	}
	int randomID;
	randomID = rand()%4;
	idnum = 0xCC + randomID;
	int nRandnum = RandomNum( 0, 25 );
	unsigned short playersSkill = chars[i].skill[FISHING];
	int nAmount;
	switch( nRandnum )
	{
	case 1:
		if( playersSkill > 920 )
		{
			SpawnRandomItem( s, 1, "necro.scp", "ITEMLIST", "1" );	// random paintings
			sysmessage( s, "You fished up an ancient painting!" );
		}
		break;
	case 2:
		if( playersSkill > 970 )
		{
			SpawnRandomItem( s, 1, "necro.scp", "ITEMLIST", "2" );	// Some new weapons
			sysmessage( s, "You fished up a weapon!" );
		}
		break;
	case 3:	// Random gold and gems
		nRandnum = RandomNum( 0, 12 );
		if( nRandnum )
		{
			SpawnRandomItem( s, 1, "necro.scp", "ITEMLIST", "3" ); 
			sysmessage( s, "You fished up a gem." );
		}
		else
		{	// Create between 200 and 1300 gold
			nAmount = RandomNum( 200, 1300 );
			addgold( s, nAmount );
			goldsfx( s, nAmount );
			sprintf( temp, "You fished up %i gold coins!", nAmount );
			sysmessage( s, temp );
		}
		break;
	case 4:
		if( playersSkill > 850 )
		{
			SpawnRandomItem( s, 1, "necro.scp", "ITEMLIST", "4" );	// Random bones and crap
			sysmessage( s, "You fished up some flotsam!" );
		}
		break;
	default:
		SpawnRandomItem( s, 1, "necro.scp", "ITEMLIST", "5" );	// User defined fish
		sysmessage( s, "You pulled out a pretty good size fish!" );
		break;
	}
}

int cSkills::GetCombatSkill(int i)
{
	int j,serial,serhash,ci;
	int skillused = WRESTLING;
	
	GetCombatResult[SKILL] = 0;
	GetCombatResult[WEAP] = 0;
	serial=chars[i].serial;
	serhash=serial%HASHMAX;
	for (ci=0;ci<contsp[serhash].max;ci++)
	{
		j=contsp[serhash].pointer[ci];
		if (j!=-1)
			if ((items[j].contserial==chars[i].serial) && ((items[j].layer==1)||(items[j].layer==2)))
			{
				if (((items[j].id1==0x13)&&(items[j].id2>=0xB5)&&(items[j].id2<=0xBA)) || 
					((items[j].id1==0x0F)&&(items[j].id2>=0x43)&&(items[j].id2<=0x4E)) ||
					((items[j].id1==0x0F)&&(items[j].id2>=0x5E)&&(items[j].id2<=0x61)) ||
					((items[j].id1==0x13)&&(items[j].id2==0xB0))                                       || //war axe
					((items[j].id1==0x13)&&(items[j].id2==0xFA)||(items[j].id2==0xFB)) ||
					((items[j].id1==0x14)&&(items[j].id2>=0x3E)&&(items[j].id2<=0x43)) ||
					((items[j].id1==0x13)&&(items[j].id2==0xFE)||(items[j].id2==0xFF)) ||
					((items[j].id1==0x0E)&&(items[j].id2>=0xC1)&&(items[j].id2<=0xC5))  )
				{
					skillused = SWORDSMANSHIP;
					GetCombatResult[SKILL] = SWORDSMANSHIP;
					GetCombatResult[WEAP] = j;
					break;
				}
				if (((items[j].id1==0x13)&&(items[j].id2==0xB3)||(items[j].id2==0xB4)) ||
					((items[j].id1==0x0F)&&(items[j].id2==0x5C)||(items[j].id2==0x5D)) ||
					((items[j].id1==0x0F)&&(items[j].id2==0xB4)||(items[j].id2==0xB5)) || 
					((items[j].id1==0x13)&&(items[j].id2==0xAF))                                               ||
					((items[j].id1==0x13)&&(items[j].id2==0xE3)||(items[j].id2==0xE4)) ||
					((items[j].id1==0x13)&&(items[j].id2==0xF4)||(items[j].id2==0xF5)) ||
					((items[j].id1==0x13)&&(items[j].id2==0xF8)||(items[j].id2==0xF9)) ||
					((items[j].id1==0x14)&&(items[j].id2>=0x38)&&(items[j].id2<=0x3D)) ||
					((items[j].id1==0x14)&&(items[j].id2==0x06)||(items[j].id2==0x07)) ||
					((items[j].id1==0x0E)&&(items[j].id2==0x89)||(items[j].id2==0x8A)) ||
					((items[j].id1==0x0D)&&(items[j].id2>=0xF0)&&(items[j].id2<=0xF5)) ||
					((items[j].id1==0x0E)&&(items[j].id2==0x81)||(items[j].id2==0x82))  )
				{
					skillused = MACEFIGHTING;
					GetCombatResult[SKILL] = MACEFIGHTING;
					GetCombatResult[WEAP] = j;
					break;
				}
				if (((items[j].id1==0x0F)&&(items[j].id2==0x51)||(items[j].id2==0x52)) ||
					((items[j].id1==0x0F)&&(items[j].id2==0x62)||(items[j].id2==0x63)) ||
					((items[j].id1==0x14)&&(items[j].id2<=0x05)) ||
					((items[j].id1==0x0E)&&(items[j].id2==0x87)||(items[j].id2==0x88))  )
				{
					skillused = FENCING;
					GetCombatResult[SKILL] = FENCING;
					GetCombatResult[WEAP] = j;
					break;
				}
				if (((items[j].id1==0x13)&&(items[j].id2==0xB2)||(items[j].id2==0xFD)) ||
					((items[j].id1==0x0F)&&(items[j].id2==0x4F))                                                ) 
				{
					skillused = ARCHERY;
					GetCombatResult[SKILL] = ARCHERY;
					GetCombatResult[WEAP] = j;
					break;
				}
			}
	}
	return(skillused);
}

int cSkills::GetShield( CHARACTER i )
{
	int j,serial,serhash,ci;
	serial = chars[i].serial;
	serhash = serial%HASHMAX;
	for( ci = 0; ci < contsp[serhash].max; ci++ )
	{
		j = contsp[serhash].pointer[ci];
		if( j != -1 )
		{
			if( items[j].contserial == serial && items[j].layer == 2 )
			{
				if( Items->isShieldType( j ) ) 
					return( j );
			}
		}
	}
	return(-1);
}

int cSkills::GetSecondHand( CHARACTER i )
{
	int j,serial,serhash,ci;
	serial = chars[i].serial;
	serhash = serial%HASHMAX;
	for( ci = 0; ci < contsp[serhash].max; ci++ )
	{
		j = contsp[serhash].pointer[ci];
		if( j != -1 )
		{
			if( items[j].contserial == serial && items[j].layer == 2 )
			{
				if( Items->isLeftHandType( j ) ) 
				{
						return( j );
				}
			}
		}
	}
	return( -1 );
}

void cSkills::SkillUse(int s, int x) // Skill is clicked on the skill list
{
	if (chars[currchar[s]].dead)
	{
		sysmessage(s,"You cannot do that as a ghost.");
		return;
	}
	if ((x!=STEALTH)&&(chars[currchar[s]].hidden)&&(!(chars[currchar[s]].priv2&8))) //AntiChrist - Stealth
	{
		chars[currchar[s]].hidden=0;
		chars[currchar[s]].stealth=-1;
		updatechar(currchar[s]);
	}
	breakConcentration( currchar[s], s );
	if( chars[currchar[s]].spellCast && ( chars[currchar[s]].casting == -1 || chars[currchar[s]].casting == 1 ) )
	{
		sysmessage( s, "You can't do that while you are casting" );
		return;
	}
	if((chars[currchar[s]].skilldelay<=uiCurrentTime) || (chars[currchar[s]].priv&1))
		switch(x)
	{
   case ARMSLORE:
	   target(s, 0, 1, 0, 29, "What item do you wish to get information about?");
	   chars[currchar[s]].skilldelay=uiCurrentTime+(server_data.skilldelay*CLOCKS_PER_SEC);
	   return;
   case ANATOMY:
	   target(s, 0, 1, 0, 37, "Whom shall I examine?");
	   chars[currchar[s]].skilldelay=uiCurrentTime+(server_data.skilldelay*CLOCKS_PER_SEC);
	   return;
   case ITEMID:
	   target(s, 0, 1, 0, 40, "What do you wish to appraise and identify?");
	   chars[currchar[s]].skilldelay=uiCurrentTime+(server_data.skilldelay*CLOCKS_PER_SEC);
	   return;
   case EVALUATINGINTEL:
	   target(s, 0, 1, 0, 41, "What would you like to evaluate?");
	   chars[currchar[s]].skilldelay=uiCurrentTime+(server_data.skilldelay*CLOCKS_PER_SEC);
	   return;
   case TAMING:
	   target(s, 0, 1, 0, 42, "Tame which animal?");
	   chars[currchar[s]].skilldelay=uiCurrentTime+(server_data.skilldelay*CLOCKS_PER_SEC);
	   return;
   case HIDING:
	   Skills->Hide(s);
	   chars[currchar[s]].skilldelay=uiCurrentTime+(server_data.skilldelay*CLOCKS_PER_SEC);
	   return;
   case STEALTH:
	   Skills->Stealth(s);
	   chars[currchar[s]].skilldelay=uiCurrentTime+(server_data.skilldelay*CLOCKS_PER_SEC);
	   return;
   case DETECTINGHIDDEN:
	   target(s, 0, 1, 0, 77, "Where do you wish to search for hidden characters?");
	   chars[currchar[s]].skilldelay=uiCurrentTime+(server_data.skilldelay*CLOCKS_PER_SEC);
	   return;
   case PEACEMAKING:
	   Skills->PeaceMaking(s);
	   chars[currchar[s]].skilldelay=uiCurrentTime+(server_data.skilldelay*CLOCKS_PER_SEC);
	   return;
   case PROVOCATION:
	   target(s, 0, 1, 0, 79, "Whom do you wish to incite?");
	   chars[currchar[s]].skilldelay=uiCurrentTime+(server_data.skilldelay*CLOCKS_PER_SEC);
	   return;
   case ENTICEMENT:
	   target(s, 0, 1, 0, 81, "Whom do you wish to entice?");
	   chars[currchar[s]].skilldelay=uiCurrentTime+(server_data.skilldelay*CLOCKS_PER_SEC);
	   return;
   case SPIRITSPEAK:
	   Skills->SpiritSpeak(s);
	   chars[currchar[s]].skilldelay=uiCurrentTime+(server_data.skilldelay*CLOCKS_PER_SEC);
	   return;
   case STEALING:
	   if (server_data.rogue)
	   {
		   target(s,0,1,0,205, "What do you wish to steal?");
		   chars[currchar[s]].skilldelay=uiCurrentTime+(server_data.skilldelay*CLOCKS_PER_SEC);
		   return;
	   } else {
		   sysmessage(s, "Contact your shard operator if you want stealing available.");
		   return;
	   }
   case INSCRIPTION:
	   target(s, 0, 1, 0, 160, "What do you wish to place a spell on?");
	   chars[currchar[s]].skilldelay=uiCurrentTime+(server_data.skilldelay*CLOCKS_PER_SEC);
	   return;
   case TRACKING:
	   Skills->TrackingMenu(s,TRACKINGMENUOFFSET);
	   chars[currchar[s]].skilldelay=uiCurrentTime+(server_data.skilldelay*CLOCKS_PER_SEC);
	   return;
   case BEGGING:
	   target(s, 0, 1, 0, 152, "Whom do you wish to annoy?");
	   chars[currchar[s]].skilldelay=uiCurrentTime+(server_data.skilldelay*CLOCKS_PER_SEC);
	   return;
   case ANIMALLORE:
	   target(s, 0, 1, 0, 153, "What animal do you wish to get information about?");
	   chars[currchar[s]].skilldelay=uiCurrentTime+(server_data.skilldelay*CLOCKS_PER_SEC);
	   return;
   case FORENSICS:
	   target(s, 0, 1, 0, 154, "What corpse do you want to examine?");
	   chars[currchar[s]].skilldelay=uiCurrentTime+(server_data.skilldelay*CLOCKS_PER_SEC);
	   return;
   case POISONING:
	   target(s, 0, 1, 0, 155, "What poison do you want to apply?");
	   chars[currchar[s]].skilldelay=uiCurrentTime+(server_data.skilldelay*CLOCKS_PER_SEC);
	   return;
   case MEDITATION:  //Morrolan - Meditation
	   if(server_data.armoraffectmana)
	   {
		   Skills->Meditation(s);
		   chars[currchar[s]].skilldelay=uiCurrentTime+(server_data.skilldelay*CLOCKS_PER_SEC);
	   }
	   else sysmessage(s, "Meditation is turned off.  Tell your GM to enable ARMOR_AFFECT_MANA_REGEN in server.scp to enable it.");
	   return;
   default:
	   sysmessage(s, "That skill has not been implemented yet.");
	   return;
	}
	else
		sysmessage(s, "You must wait a few moments before using another skill.");
}

void cSkills::RandomSteal(int s)
{
	int npc, serial, p, i, skill, item;
	char temp2[512];
	tile_st tile;
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	npc=findbyserial(&charsp[serial%HASHMAX], serial,1);
	if (npc==-1) return;
	p=packitem(npc);
	if (p==-1) 
	{
		sysmessage(s,"bad luck, your victim doesnt have a backpack"); 
		return; 
	} //LB
	item=-1;
	i=0;
	do {
		i++;
		item=contsp[items[p].serial%HASHMAX].pointer[rand()%contsp[items[p].serial%HASHMAX].max];
		if (i>=50) return;
	} while (item!=-1);
	if (npc==currchar[s]) 
	{
		sysmessage(s,"You catch yourself red handed.");
		return;
	}
	
	if (chars[npc].npcaitype==17)
	{
		sysmessage(s, "You cannot steal that.");
		return;
	}
	//	if( item == -1 ) return;
	if( item < 0 || item >= imem )
	{
		sysmessage( s, "Muahaha, your victim doesn't have possessions" );
		return;
	}
	//  Abaddon - February 21, 2000
  //  Fix for the Stealing from vendor list bug?
	int itemCont = findbyserial( &itemsp[items[item].contserial%HASHMAX], items[item].contserial, 0); 
	if( itemCont != -1 && items[itemCont].layer >= 0x1a && items[itemCont].layer <= 0x1c ) // is it in the buy or sell layer of a vendor?
	{
		sysmessage( s, "You cannot steal that." );
		return;
	}
	// End Abaddon
	sprintf(temp, "You reach into %s's pack and try to take something...",chars[npc].name, items[item].name);
	sysmessage(s, temp);
	if (npcinrange(s,npc,1))
	{
		if ((items[item].weight>10) && (items[item].type!=1 && items[item].type!=63 &&
			items[item].type!=65 && items[item].type!=87))//Containers
		{
			sysmessage(s,"That is too heavy.");
			return;
		} else if((items[item].type==1 || items[item].type==63 || // lb bugfix, was &&
			items[item].type==65 || items[item].type==87) && (Weight->CalcWeightPack(item)>(10 + chars[currchar[s]].baseskill[STEALING]/4)))
		{
			sysmessage(s,"That is too heavy.");
			return;
		}
		if (chars[npc].priv&1 || chars[npc].priv&0x80)//GM
		{
			sysmessage(s, "You can't steal from gods.");
			return;
		}
		if(items[item].priv&2)//newbie
		{
			sysmessage(s,"That item has no value to you.");
			return;
		}
		
		skill=Skills->CheckSkill(currchar[s],STEALING,0,999);
		if (skill)
		{
			//pack=packitem(currchar[s]);
			unsetserial( item, 1 );
			setserial( item, packitem(currchar[s]), 1 );
			sysmessage( s, "You successfully steal that item." );
			all_items(s);
			printf("ALERT: all_items() called in RandomSteal().  This function could cause a lot of lag!" );
		} else sysmessage(s, "You failed to steal that item.");
		
		if ((!skill && rand()%5+15==17) || (chars[currchar[s]].skill[STEALING]<rand()%1001))
		{//Did they get cought? (If they fail 1 in 5 chance, other wise their skill away from 1000 out of 1000 chance)
			sysmessage(s,"You have been cought!");
			
			if (chars[npc].npc) npctalkall(npc, "Guards!! A thief is amoung us!", 0);
			
			if( (chars[npc].flag & 0x04) && chars[currchar[s]].attacker != npc && Guilds->Compare( currchar[s], npc ) == 0 && Races->Compare( currchar[s], npc ) == 0 )
			{
				//		EviLDeD -		March 1, 2000
				//		Some extra console spam
				printf("DEBUG: [RandomSteal()] %s is being set to criminal\n", chars[currchar[s]].name );
				//		EviLDeD -		End
				criminal( currchar[s] ); //Blue and not attacker and not guild
			}
			if( items[item].name[0] != '#' )
			{
				sprintf(temp,"You notice %s trying to steal %s from you!",chars[currchar[s]].name,items[item].name);
				sprintf(temp2,"You notice %s trying to steal %s from %s!",chars[currchar[s]].name,items[item].name,chars[npc].name);
			} else {
				Map->SeekTile((items[item].id1<<8)+items[item].id2,&tile);
				sprintf(temp,"You notice %s trying to steal %s from you!",chars[currchar[s]].name, tile.name);
				sprintf(temp2,"You notice %s trying to steal %s from %s!",chars[currchar[s]].name,tile.name,chars[npc].name);
			}
			sysmessage( s, temp ); // bugfix, LB
			
			
			for(i=0;i<now;i++)
				if((i!=s)&&(inrange1p(currchar[s],currchar[i]))&&(rand()%10+10==17||(rand()%2==1 && chars[currchar[i]].in>=chars[currchar[s]].in)))
					sysmessage(s,temp2);
		}
	} else sysmessage(s, "You are too far away to steal that item.");
}

void cSkills::StealingTarget(int s)
{
	int item, i, serial, pack, skill, npc = -1, x, cont,b, z, w = 0;
	char temp2[512];
	tile_st tile;
	
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	if (buffer[s][7]<0x40)
	{
		Skills->RandomSteal(s);
		return;
	}
	item=findbyserial(&itemsp[serial%HASHMAX], serial,0);
	x=item;
	if (item!=-1)
	{
		cont=items[item].contserial; 
		z=0;
		if ((items[item].layer==0) && (!(items[item].priv&2)) && (cont!=-1)) 
		{
			do 
			{
				x=findbyserial(&itemsp[items[x].contserial%HASHMAX], items[x].contserial, 0); //calcItemFromSer(items[x].cont1,items[x].cont2,items[x].cont3,items[x].cont4);	
				//printf("Repeat: %i\n",z);
				z++;
				if (x!=-1) 
					b=items[x].cont1; 
				else break;
			} while (b>=0x40 && z < 255 );
			if( x != -1 )
				npc=findbyserial(&charsp[items[x].contserial%HASHMAX], items[x].contserial, 1);
			else
				npc=-1;
			if( x > -1 )
			{
				items[item].weight=w=Weight->ItemWeight( item );
				if ( items[item].type == 1 || items[item].type == 63 || items[item].type == 65 || items[item].type == 87 ) //its a pack!
					w+=(int)(min(Weight->CalcWeightPack( item ), (double)1000));
								if( w > 20 )	// this is too small.  ItemWeight indicates that 100 = 1 stone, so we can never get more than 1/5th of a stone??? looks dodgy to me
//								Swords weigh around 600-800, and they don't weigh less than 1 stone, nor 60stone or more... so 100 must equal 1 stone
				if( w > 100 + chars[currchar[s]].baseskill[STEALING]*2 ) // make it 2 times the base stealing skill... GM thieves can steal up to 21 stones, newbie only 1 stone
				{
					sysmessage( s, "That is too heavy." );
					return;
				}
			}
		} 
		else 
		{
			sysmessage(s,"You cannot steal that.");
			return;
		}
	}
	
	if( npc != -1 )
	{
		if (chars[npc].npcaitype==17)
		{
			sysmessage(s, "You cannot steal that.");
			return;
		}
	} else return;
	
	if (npc==currchar[s]) 
	{
		sysmessage(s,"You catch yourself red handed.");
		return;
	}
	//  Abaddon - February 21, 2000
  //  Same stealing fix?
	int itemCont = findbyserial( &itemsp[items[item].contserial%HASHMAX], items[item].contserial, 0); 
	if( itemCont != -1 && items[itemCont].layer >= 0x1a && items[itemCont].layer <= 0x1c ) // is it in the buy or sell layer of a vendor?
	{
		sysmessage( s, "You cannot steal that." );
		return;
	}
	// End Abaddon
	if ( chars[currchar[s]].baseskill[STEALING] < 50 && w<=100)		// w<=1???  Less than 1/100th of a stone?! how about less than a stone?
		skill = Skills->CheckSkill(currchar[s],STEALING,0,1000);
	else
		skill = Skills->CheckSkill(currchar[s],STEALING,min((w*50), 990),1000);
	
	if (npcinrange(s,npc,1))
	{
		if (skill)
		{
			pack=packitem(currchar[s]);
			unsetserial( item, 1 );
			setserial( item, pack, 1 );
			sysmessage( s, "You successfully steal that item." );
			RefreshItem( item );	// let's reuse some code that will cope with visibility
		} 
		else 
			sysmessage(s, "You failed to steal that item.");
		
		if ((!skill && rand()%16==7) || (chars[currchar[s]].skill[STEALING]<rand()%1002)) //0 to 1001, so ever a gm has a SMALL chance of being caught
		{
			sysmessage(s,"You have been caught!");
			
			if (npc!=-1) //lb
			{
				if (chars[npc].npc) 
					npctalkall(npc, "Guards!! A thief is amoung us!", 0);
				
				if( (chars[npc].flag & 0x04) && chars[currchar[s]].attacker!=npc && Guilds->Compare( currchar[s], npc )==0 && Races->Compare( currchar[s], npc )==0 )
				{
					//		EviLDeD -		March 1, 2000
					//		Some extra console spam
					printf("DEBUG: [StealingTarget()] %s is being set to criminal\n", chars[currchar[s]].name );
					//		EviLDeD -		End
					criminal(currchar[s]);//Blue and not attacker and not guild
				}
				
				if( items[item].name[0] !='#' )
				{
					sprintf(temp,"You notice %s trying to steal %s from you!",chars[currchar[s]].name,items[item].name);
					sprintf(temp2,"You notice %s trying to steal %s from %s!",chars[currchar[s]].name,items[item].name,chars[npc].name);
				} 
				else 
				{
					Map->SeekTile((items[item].id1<<8)+items[item].id2,&tile);
					sprintf(temp,"You notice %s trying to steal %s from you!",chars[currchar[s]].name,tile.name);
					sprintf(temp2,"You notice %s trying to steal a %s from %s!",chars[currchar[s]].name,tile.name,chars[npc].name);
				}
				//				sysmessage(calcSocketFromChar(npc),temp);	// once again, if this really is an npc, it'll cause a crash (Abaddon)
				if( !chars[npc].npc )
					sysmessage( calcSocketFromChar( npc ), temp );	// presuming that just perhaps you might be able to have a pc here
			}
			
			for(i=0;i<now;i++)
			{
				if((i!=s)&&(inrange1p(currchar[s],currchar[i]))&&(rand()%10+10==17||(rand()%2==1 && chars[currchar[i]].in>=chars[currchar[s]].in-rand()%15)))
					sysmessage(i,temp2);
			}
		}
	} else sysmessage(s, "You are too far away to steal that item.");
}

void cSkills::Tracking(int s,int selection)
{
	int i = currchar[s];
	chars[i].trackingtarget = chars[i].trackingtargets[selection]; // sets trackingtarget that was selected in the gump
	chars[i].trackingtimer = ((((tracking_data.basetimer*chars[i].skill[TRACKING])/1000)+1)*CLOCKS_PER_SEC)+uiCurrentTime; // tracking time in seconds ... gm tracker -> basetimer + 1 seconds, 0 tracking -> 1 sec, new calc by LB
	chars[i].trackingdisplaytimer = (unsigned int)( tracking_data.redisplaytime * CLOCKS_PER_SEC + uiCurrentTime );
	sprintf(temp,"You are now tracking %s.",chars[chars[i].trackingtarget].name);
	sysmessage(s,temp);
	Skills->Track(i);
}

void cSkills::CreateTrackingMenu(int s,int m)
{
	unsigned int i;
	int total;
	char lentext;
	char sect[512];
	char gmtext[MAXTRACKINGTARGETS][257]; // crashfix, LB
	int gmid[MAXTRACKINGTARGETS]; // crashfix, LB
	//int gmnumber;
	int id;
	int d;
	int id1=62; // default tracking animals
	int id2=399;
	int icon=8404; 
	int ii = currchar[s];
	char type[40]="You see no signs of any animals.";
	unsigned int MaxTrackingTargets=0;
	unsigned int distance=tracking_data.baserange+chars[currchar[s]].skill[TRACKING]/50;
	
	if(m==(2+TRACKINGMENUOFFSET))
	{
		id1=1;
		id2=61;
		icon=0x20D1;
		strcpy(type,"You see no signs of any creatures.");
	}
	if(m==(3+TRACKINGMENUOFFSET))
	{
		id1=400;
		id2=402;
		icon=8454;
		strcpy(type,"You see no signs of anyone.");
	}
	
	openscript("tracking.scp");
	sprintf(sect, "TRACKINGMENU %i", m);
	if(!i_scripts[tracking_script]->find(sect)) 
	{
		closescript();
		return;
	}
	
	read1();
	lentext=sprintf(gmtext[0], "%s", script1);
	
	//for (i=0;i<charcount;i++)
	//{
	
	//Char mapRegions
	int	StartGrid=mapRegions->StartGrid(chars[ii].x,chars[ii].y);
	
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
				if (mapitem!=-1 && mapitem>=1000000)
				{
					i=mapchar;
					d=chardist(i,currchar[s]);
					id=chars[i].id1;
					id=id<<8;
					id=id|chars[i].id2;
					if((d<=distance)&&(chars[i].dead==0)&&(id>=id1&&id<=id2)&&calcSocketFromChar(i)!=s&&(online(i)||chars[i].npc))
					{     
						chars[ii].trackingtargets[MaxTrackingTargets]=i;
						MaxTrackingTargets++;
						if( MaxTrackingTargets >= MAXTRACKINGTARGETS ) break; // lb crashfix
						switch(Skills->TrackingDirection(s,i))
						{
						case NORTH:
							strcpy(temp,"to the North");
							break;
						case NORTHWEST:
							strcpy(temp,"to the Northwest");
							break;
						case NORTHEAST:
							strcpy(temp,"to the Northeast");
							break;
						case SOUTH:
							strcpy(temp,"to the South");
							break;
						case SOUTHWEST:
							strcpy(temp,"to the Southwest");
							break;
						case SOUTHEAST:
							strcpy(temp,"to the Southeast");
							break;
						case WEST:
							strcpy(temp,"to the West");
							break;
						case EAST:
							strcpy(temp,"to the East");
							break;
						default:
							strcpy(temp,"right next to you");
							break;
						}//switch
						sprintf(gmtext[MaxTrackingTargets], "%s %s",chars[i].name,temp);
						gmid[MaxTrackingTargets] = creatures[(chars[i].id1<<8)+chars[i].id2].icon; // placing correct icon, LB
					}
				}//if mapitem
			} while (mapitem!=-1);
		}//for a<3
	}//for checkgrid
	
	if(MaxTrackingTargets==0)
	{
		sysmessage(s,type);
		closescript();
		return;
	}
	
	total=9+1+lentext+1;
	for (i=1;i<=MaxTrackingTargets;i++) total+=4+1+strlen(gmtext[i]);
	gmprefix[1]=total>>8;
	gmprefix[2]=total%256;
	gmprefix[3]=chars[ii].ser1;
	gmprefix[4]=chars[ii].ser2;
	gmprefix[5]=chars[ii].ser3;
	gmprefix[6]=chars[ii].ser4;
	gmprefix[7]=(m+TRACKINGMENUOFFSET)>>8;
	gmprefix[8]=(m+TRACKINGMENUOFFSET)%256;
	Network->xSend(s, gmprefix, 9, 0);
	Network->xSend(s, &lentext, 1, 0);
	Network->xSend(s, gmtext[0], lentext, 0);
	Network->xSend(s, &MaxTrackingTargets, 1, 0);
	for (i=1;i<=MaxTrackingTargets;i++)
	{
		gmmiddle[0]=gmid[i]>>8;
		gmmiddle[1]=gmid[i]%256;
		Network->xSend(s, gmmiddle, 4, 0);
		lentext=strlen(gmtext[i]);
		Network->xSend(s, &lentext, 1, 0);
		Network->xSend(s, gmtext[i], lentext, 0);
	}
	closescript();
}

void cSkills::TrackingMenu(int s,int gmindex)
{
	int total, i;
	char lentext;
	char sect[512];
	char gmtext[MAXTRACKINGTARGETS][257];
	int gmid[MAXTRACKINGTARGETS];
	int gmnumber=0;
	if( gmindex < 1000 )
	{
		Skills->Tracking( s, gmindex );
		return;
	}
	openscript("tracking.scp");
	sprintf(sect, "TRACKINGMENU %i",gmindex);
	if (!i_scripts[tracking_script]->find(sect)) 
	{
		closescript();
		return;
	}
	read1();
	lentext=sprintf(gmtext[0], "%s", script1);
	do
	{
		read2();
		if (script1[0]!='}')
		{
			gmnumber++;
			gmid[gmnumber]=hstr2num(script1);
			strcpy(gmtext[gmnumber], script2);
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
	gmprefix[7]=(gmindex+TRACKINGMENUOFFSET)>>8;
	gmprefix[8]=(gmindex+TRACKINGMENUOFFSET)%256;
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

void cSkills::Track(int i)
{
	//	int tl;
	int s=calcSocketFromChar(i);
	int direction=5;
	if (chars[chars[currchar[s]].trackingtarget].y==-1) return;
	if((chars[i].y-direction)>=chars[chars[i].trackingtarget].y)  // North
	{
		sprintf(temp,"%s is to the North",chars[chars[i].trackingtarget].name);
		if((chars[i].x-direction)>chars[chars[i].trackingtarget].x)
			sprintf(temp,"%s is to the Northwest",chars[chars[i].trackingtarget].name);
		if((chars[i].x+direction)<chars[chars[i].trackingtarget].x)
			sprintf(temp,"%s is to the Northeast",chars[chars[i].trackingtarget].name);
	}
	else if((chars[i].y+direction)<=chars[chars[i].trackingtarget].y)  // South
	{
		sprintf(temp,"%s is to the South",chars[chars[i].trackingtarget].name);
		if((chars[i].x-direction)>chars[chars[i].trackingtarget].x)
			sprintf(temp,"%s is to the Southwest",chars[chars[i].trackingtarget].name);
		if((chars[i].x+direction)<chars[chars[i].trackingtarget].x)
			sprintf(temp,"%s is to the Southeast",chars[chars[i].trackingtarget].name);
	}
	else if((chars[i].x-direction)>=chars[chars[i].trackingtarget].x)  // West
	{
		sprintf(temp,"%s is to the West",chars[chars[i].trackingtarget].name);
	}
	else if((chars[i].x+direction)<=chars[chars[i].trackingtarget].x)  // East
	{
		sprintf(temp,"%s is to the East",chars[chars[i].trackingtarget].name);
	}
	else sprintf(temp,"%s is right next to you",chars[chars[i].trackingtarget].name);
	
	unsigned char arrow[7];
	arrow[0]=0xBA;
	arrow[1]=1;
	arrow[2] = (unsigned char)(chars[chars[i].trackingtarget].x-1)>>8;
	arrow[3] = (unsigned char)(chars[chars[i].trackingtarget].x-1)%256;
	arrow[4] = (unsigned char)chars[chars[i].trackingtarget].y>>8;
	arrow[5] = (unsigned char)chars[chars[i].trackingtarget].y%256;
	Network->xSend(s, arrow, 6, 0);
}

int cSkills::TrackingDirection(int s,int i)
{
	int direction=5;
	int j = currchar[s];
	if((chars[j].y-direction)>=chars[i].y)  // North
	{
		if((chars[j].x-direction)>chars[i].x)
			return NORTHWEST;
		if((chars[j].x+direction)<chars[i].x)
			return NORTHEAST;
		return NORTH;
	}
	else if((chars[j].y+direction)<=chars[i].y)  // South
	{
		if((chars[j].x-direction)>chars[i].x)
			return SOUTHWEST;
		if((chars[j].x+direction)<chars[i].x)
			return SOUTHEAST;
		return SOUTH;
	}
	else if((chars[j].x-direction)>=chars[i].x)  // West
		return WEST;
	else if((chars[j].x+direction)<=chars[i].x)  // East
		return EAST;
	else return 0;
}

void cSkills::BeggingTarget(int s)
{
	int i,serial;
	addid1[s]=buffer[s][7];
	addid2[s]=buffer[s][8];
	addid3[s]=buffer[s][9];
	addid4[s]=buffer[s][10];
	serial=calcserial(addid1[s],addid2[s],addid3[s],addid4[s]);
	if(calcSocketFromChar(calcCharFromSer(addid1[s], addid2[s], addid3[s], addid4[s]))!=-1)
	{
		sysmessage(s,"Maybe you should just ask.");
		return;
	}
	i=findbyserial(&charsp[serial%HASHMAX],serial,1);
	if (i>-1)
	{
		if(chardist(i,currchar[s])>=begging_data.range)
		{
			sysmessage(s,"You are not close enough to beg.");
			return;
		}
		if((chars[i].id1==0x01)&&((chars[i].id2==0x90)||(chars[i].id2==0x91))&&(chars[i].in != 0)) //Used on human
		{
			npctalkall(currchar[s], begging_data.text[rand()%3], 0); // npcemoteall?
			if (!Skills->CheckSkill(currchar[s],BEGGING, 0, 1000))
				sysmessage(s,"They seem to ignore your begging plees.");
			else
			{
				npctalkall(i,"Ohh thou lookest so poor, Here is some gold I hope this will assist thee.", 0); // zippy
				addgold(s,(10+(rand()%(chars[currchar[s]].skill[BEGGING]+1))/25));		// Newbie beggers with no skill will crash the server (Abaddon)
				sysmessage(s,"Some gold is placed in your pack.");
			}
		}
		else
			sysmessage(s, "That would be foolish.");
	}
}

void cSkills::AnimalLoreTarget(int s)
{
	int i,serial;
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&charsp[serial%HASHMAX],serial,1);
	if (i>-1)
	{
		if(chars[i].priv&0x81)
		{
			sysmessage(s,"Little is known of these robed gods.");
			return;
		}
		if((chars[i].id1==0x01) && ((chars[i].id2==0x90) || (chars[i].id2==0x91))) //Used on human
		{
			sysmessage(s,"The human race should use dvorak!");
			return;
		}
		else // Lore used on a non-human
		{
			if (Skills->CheckSkill(currchar[s], ANIMALLORE, 0, 1000))
			{
				sprintf(temp, "Attack [%i] Defense [%i] Taming [%i] Hit Points [%i]", chars[i].att,chars[i].def,chars[i].taming/10,chars[i].hp);
				npcemote(s,i,temp, 0);
				return;
			}
			else
			{
				sysmessage(s,"You can not think of anything relevant at this time.");
				return;
			}
		}
	}
}

void cSkills::ForensicsTarget(int s) //AntiChrist
{
	int i,serial;
	int curtim=uiCurrentTime;
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&itemsp[serial%HASHMAX],serial,0);
	if (i>-1)
	{
		if (!(items[i].corpse))
		{
			sysmessage(s, "That does not appear to be a corpse.");
			return;
		}
		
		if(chars[currchar[s]].priv&1)
		{
			sprintf(temp,"The %s is %i seconds old and the killer was  %s.", items[i].name, (curtim-items[i].murdertime)/CLOCKS_PER_SEC, items[i].murderer);
			sysmessage(s, temp);
		} else {
			if (!Skills->CheckSkill(currchar[s], FORENSICS, 0, 500)) sysmessage(s,"You are not certain about the corpse."); else
			{
				if(((curtim-items[i].murdertime)/CLOCKS_PER_SEC)<=60) strcpy(temp2,"few");
				if(((curtim-items[i].murdertime)/CLOCKS_PER_SEC)>60) strcpy(temp2,"many");
				if(((curtim-items[i].murdertime)/CLOCKS_PER_SEC)>180) strcpy(temp2,"many many");
				sprintf(temp,"The %s is %s seconds old.",&items[i].name[2],temp2);
				sysmessage(s,temp);
				if (!Skills->CheckSkill(currchar[s], FORENSICS, 500, 1000) || *(items[i].murderer)=='\0') sysmessage(s,"You can't say who was the killer."); else
				{
					sprintf(temp,"The killer was %s.",items[i].murderer);
					sysmessage(s,temp);
				}
				//printf("MURDERER %i\n",items[i].murderer);
			}
		}
	} else {
		sysmessage(s, "That does not appear to be a corpse.");
		return;
	}
}

void cSkills::PoisoningTarget( UOXSOCKET s )
{
	CHARACTER mChar = currchar[s];	// character for our socket
	int serial, poison, success = 0;
	ITEM toPoison;
	poison = calcItemFromSer( chars[mChar].poisonserial );
	chars[mChar].poisonserial = 0;
	serial = calcserial( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	toPoison = calcItemFromSer( serial );
	if( poison != -1 ) 
	{
		if( items[poison].type != 19 || items[poison].morey != 6 ) 
		{
			sysmessage( s, "That is not a valid poison!" );
			return;
		}
	} 
	else 
		return;
	
	if( toPoison != -1 )
	{
		unsigned short realID = (items[toPoison].id1<<8) + items[toPoison].id2;
		if( ( realID >= 0x0F4F && realID <= 0x0F50 ) || // crossbows
			( realID >= 0x13B1 && realID <= 0x13B2 ) || // Bows
			( realID >= 0x13FC && realID <= 0x13FD ) )	// Heavy Crossbows
		{
			sysmessage( s, "Bows cannot be poisoned" );
			return;
		}
		switch( items[poison].morez ) 
		{
		case 1://lesser poison
			success = Skills->CheckSkill( mChar, POISONING, 0, 500 );
			break;
		case 2://poison
			success = Skills->CheckSkill( mChar, POISONING, 151, 651 );
			break;
		case 3://greater poison
			success = Skills->CheckSkill( mChar, POISONING, 551, 1051 );
			break;
		case 4://deadly poison
			success = Skills->CheckSkill( mChar, POISONING, 901, 1401 );
			break;
		default:
			printf("ERROR: Fallout of switch statement without default. skills.cpp, poisoningtarget()/n"); //Morrolan
			return;
		}
		if( success ) 
		{
			soundeffect2( mChar, 0x02, 0x47 ); //poisoning effect
			if( items[toPoison].poisoned < items[poison].morez ) 
				items[toPoison].poisoned = items[poison].morez;
			sysmessage( s, "You successfully poison that item." );
		} 
		else 
		{
			soundeffect2( mChar, 0x02, 0x47 ); //poisoning effect
			sysmessage( s, "You fail to apply the poison." );
		}
		
		//empty bottle after poisoning
		if( items[poison].contserial != -1 ) 
			unsetserial( poison, 1 );
		unsigned char k1 = items[poison].ser1;
		unsigned char k2 = items[poison].ser2;
		unsigned char k3 = items[poison].ser3;
		unsigned char k4 = items[poison].ser4;
		long int kser = items[poison].serial;
		Items->InitItem( poison, 0 );
		items[poison].ser1 = k1;
		items[poison].ser2 = k2;
		items[poison].ser3 = k3;
		items[poison].ser4 = k4;
		items[poison].serial = kser;
		items[poison].id1 = 0x0F;
		items[poison].id2 = 0x0E;
		items[poison].pileable = 1;
		mapRegions->RemoveItem( poison ); //AntiChrist
		items[poison].x = chars[mChar].x;
		items[poison].y = chars[mChar].y;
		items[poison].z = chars[mChar].z;
		items[poison].priv |= 0x01;
		mapRegions->AddItem( poison );
		RefreshItem( poison ); // AntiChrist
	} 
	else 
	{
		sysmessage( s, "You can't poison that item." );
		return;
	}
	chars[mChar].poisonserial = 0;
}

void TellScroll( char *menu_name, int player, long item_param )
// PARAM WARNING: menu_name is never referenced
{
	Skills->Inscribe(player,item_param);
	return;
}

int cSkills::Inscribe(UOXSOCKET s, long snum) 
{
	
	/* this will be the main inscribe/engrave section */
	unsigned cir,spl;
	int i,part,serial;
	int clickitem=-1;
	CHARACTER inscChar = currchar[s];
	ITEM spellBook = -1;
	
	if( snum > 0 ) // if 0 then its the first time
	{
		cir = (int)((snum-800)*.1);
		spl = (((snum-800)-(cir*10))+1);
		spellBook = Npcs->FindItem( inscChar, 9 );
		if( spellBook == -1 )
		{
			sysmessage( s, "You don't have a spellbook to scribe from!" );
			return 0;
		}
		if( !Magic->CheckBook( cir, spl-1, spellBook ) )
		{
			sysmessage( s, "You don't have this spell in your spell book!" );
			return 0;
		}
		i = chars[currchar[s]].making; // lets re-grab the item they clicked on
		chars[currchar[s]].making = 0;          //clear it out now that we are done with it.
		
		
		if ((items[i].id1=='\x0E')&&(items[i].id2=='\x34'))  //is it a scroll?
		{
			itemmake[s].minskill=(snum-810)*10;itemmake[s].maxskill=(snum-490)*10; 
			//set range values based on scroll level
			// changed by lord binary, fixed the problem that scroll-inscription works for ALL circles regardless of skill
			// still needs tweaking ... *10,*9,*8,*7,*6,*5 ... dont know, needs testing.
			Skills->CheckSkill(currchar[s], INSCRIPTION, (snum-810)*10, (snum-490)*10); /* just for skill-advancement */
			int num = ( 8 * ( cir - 1 ) ) + spl;
			if( spells[num].action ) impaction(s, spells[num].action); // Should have a default action instead
			npctalkall(currchar[s], spells[num].mantra, 0);
			
			if( !Magic->CheckReagents( currchar[s], spells[num].reags ))
			{ 
				Magic->SpellFail( s );
				return 0;
			}
			if( !Magic->CheckMana( currchar[s], num ) )
			{
				Magic->SpellFail(s);
				return 0;
			}
			
			Magic->NewDelReagents( currchar[s], spells[num].reags );
			Magic->SubtractMana(currchar[s], spells[num].mana);
			
			// end of lord binary's delete reg changes
			Skills->MakeMenuTarget(s,snum,INSCRIPTION); //put it in your pack
/*			if( items[i].amount <= 1 )
				Items->DeleItem( i );
			else
				items[i].amount--;*/
		}
		else if ((items[i].att>0)||(items[i].def>0)||(items[i].hidamage)) //or is it an item?
		{
			// To Thyme: Nice idea on the scroll check, except for one minor issue
			// You'll never hit this part if it IS a scroll, the if test above is
			// for scrolls.  The variable i points to the item clicked on, not the
			// scroll used
			part=0;
//			items[i].amount--;//delete one from the balnk scrolls!
			switch(cir)
			{
			case 1:
				part += Skills->CheckSkill(currchar[s], INSCRIPTION, 11, 401);
				part += Skills->CheckSkill(currchar[s], MAGERY, 11, 401);
				part += Skills->CheckSkill(currchar[s], TINKERING, 11, 401);
				break;
			case 2:
				part += Skills->CheckSkill(currchar[s], INSCRIPTION, 61, 501);
				part += Skills->CheckSkill(currchar[s], MAGERY, 61, 501);
				part += Skills->CheckSkill(currchar[s], TINKERING, 61, 501);
				break;
			case 3:
				part += Skills->CheckSkill(currchar[s], INSCRIPTION, 161, 601);
				part += Skills->CheckSkill(currchar[s], MAGERY, 161, 601);
				part += Skills->CheckSkill(currchar[s], TINKERING, 161, 601);
				break;
			case 4:
				part += Skills->CheckSkill(currchar[s], INSCRIPTION, 261, 701);
				part += Skills->CheckSkill(currchar[s], MAGERY, 261, 701);
				part += Skills->CheckSkill(currchar[s], TINKERING, 261, 701);
				break;
			case 5:
				part += Skills->CheckSkill(currchar[s], INSCRIPTION, 361, 801);
				part += Skills->CheckSkill(currchar[s], MAGERY, 361, 801);
				part += Skills->CheckSkill(currchar[s], TINKERING, 361, 801);
				break;
			case 6:
				part += Skills->CheckSkill(currchar[s], INSCRIPTION, 461 , 901);
				part += Skills->CheckSkill(currchar[s], MAGERY, 461, 901);
				part += Skills->CheckSkill(currchar[s], TINKERING, 461, 901);
				break;
			case 7:
				part += Skills->CheckSkill(currchar[s], INSCRIPTION, 661, 1101);
				part += Skills->CheckSkill(currchar[s], MAGERY, 661, 1101);
				part += Skills->CheckSkill(currchar[s], TINKERING, 661, 1101);
				break;
			case 8:
				part += Skills->CheckSkill(currchar[s], INSCRIPTION, 761, 1201);
				part += Skills->CheckSkill(currchar[s], MAGERY, 761, 1201);
				part += Skills->CheckSkill(currchar[s], TINKERING, 761, 1201);
				break;
			default:
				printf("ERROR: Fallout of switch statement without default. skills.cpp, inscribe()/n"); //Morrolan
				if( items[i].amount < 1 )
					Items->DeleItem( i );
				return 0;
			}
			
			switch(part) 
			{
			case 0:
				sysmessage(s,"Your hand jerks and you punch a hole in the item");
				items[i].hp -= 3;
				if( items[i].hp < 1 ) 
					items[i].hp = 1;
				break;
			case 1:
				sysmessage(s,"Your hand slips and you dent the item");
				items[i].hp -= 2;
				if( items[i].hp < 1 ) 
					items[i].hp = 1;
				break;
			case 2:
				sysmessage(s,"Your hand cramps and you scratch the item");
				items[i].hp--;
				if( items[i].hp < 1 ) 
					items[i].hp = 1;
				break;
			case 3:
				if( !( items[i].morez == 0 || ( items[i].morex == cir && items[i].morey == spl ) )  )
				{
					sysmessage( s, "This item already has a spell!" );
				}
				else if( items[i].morez >= ( 18 - items[i].morex * 2 ) )	// 2 less charges per circle
				{
					sysmessage( s, "Item at max charges!" );
				}
				else if (items[i].morex == cir && items[i].morey == spl  ) 
				{
					if( EngraveAction(s, i, cir, spl ) )
					{
						items[i].morez++;
						sysmessage( s, "Item successfully Engraved" );
					}
					else 
						sysmessage( s, "You fail to engrave the spell!" );
				}
				else if( EngraveAction( s, i, cir, spl ) )
				{
					items[i].type2 = items[i].type; //kept type of item for returning to this type when item remain no charge 
					items[i].type = 15;  // make it magical
					items[i].morex = cir; // spell circle
					items[i].morey = spl; // spell number
					items[i].morez++;  // charges
					sysmessage( s, "Item successfully Engraved" );
				}
				else 
					sysmessage( s, "You fail to engrave the spell!" );
				break;
			default:
				printf("ERROR: Fallout of switch statement without default. skills.cpp, inscribe()/n"); //Morrolan
				if( items[i].amount < 1 )
					Items->DeleItem( i );
				return(0);
			}//switch
			if( items[i].amount < 1 )
				Items->DeleItem( i );
		}//else if
		return(0);
	}
 
	 // - Find what they clicked on
	 if(buffer[s][11]==0xFF && buffer[s][12]==0xFF && buffer[s][13]==0xFF && buffer[s][14]==0xFF) 
	 {
		 printf("Morrolan - Inscribing click on invalid object?");
		 return 0; // don't do this if the buffer got flushed - Morrolan prevents CRASH!
	 }
	 serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	 i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	 if (i>-1)
	 {
		 clickitem=i;
	 }
 
	 if (clickitem==-1) 
	 {
		 sysmessage(s,"You could not possibly engrave on that!");
		 chars[currchar[s]].making=0;
		 return(0);
	 }
 
	 chars[currchar[s]].making=clickitem;  //we gotta remember what they clicked on!
	 if (((items[clickitem].id1=='\x0E')&&(items[clickitem].id2=='\x34'))||  //its a scroll
		 ((items[clickitem].att>0)||(items[clickitem].def>0)||(items[i].hidamage>0)))               // its something else
	 {
	 /* select spell gump menu system here, must return control to UOX so we dont
	 freeze the game. when returning to this routine, use snum to determine where to go
		 and snum is also the value of what they selected. */
		 
		 im_sendmenu( "InscribeMenu", s );  //this is in im.cpp file using inscribe.gmp
		 return(0);
	 }
	 return(0);
}

int cSkills::EngraveAction(int s, int i, int cir, int spl)
{
	int num = ( 8 * ( cir - 1 ) ) + spl;
	
	if( spells[num].action ) 
		impaction( s, spells[num].action ); // Should have a default action instead
	npctalkall( currchar[s], spells[num].mantra, 0 );
	if( !Magic->CheckReagents( currchar[s], spells[num].reags ) )
		if( !Magic->CheckMana( currchar[s], num ))
		{
			Magic->SpellFail(s);
			return 0;
		}
		
	Magic->NewDelReagents( currchar[s], spells[num].reags );
	Magic->SubtractMana( currchar[s], spells[num].mana );
	
	switch(cir)
	{
	case 1:
		switch(spl)
		{
		case 1: items[i].offspell = 1;	break;      // Clumsy
		case 3:	items[i].offspell = 2;	break;      // Feeblemind
		case 5: items[i].offspell = 3;	break;		// Magic Arrow
		case 7: sysmessage(s, "Sorry this spell is not implemented!");	return 0;	// Reactive Armor
		case 8: items[i].offspell = 4;	break;		// Weaken
		case 2:											// Create Food
		case 4:											// Heal
		case 6:							break;			// Night Sight
		default:	printf( "ERROR: cSkills::Inscribe -> Fallout of switch statement without default\n"); return( 0 );
		}//switch spl
		break; //case 1 (cir)
	case 2:
		switch(spl)
		{
		case 1:			// Agility
		case 2:			// Cunning
		case 8:	break;	// Strength
		case 3:			// Cure
		case 5:			// Magic Trap
		case 6:			// Magic Untrap
		case 7:			// Protection
				sysmessage(s, "Sorry this spell is not implemented!");	return 0;
		case 4:	items[i].offspell = 5;	break; // Harm
		default:	printf( "ERROR: cSkills::Inscribe -> Fallout of switch statement without default\n"); return( 0 );
		}
		break;
	case 3:
		switch (spl)
		{
		case 2:	items[i].offspell = 6;	break;	// Fireball
		case 3: //Magic lock
		case 5: //Telekinesis
				sysmessage(s, "Sorry this spell is not implemented!");	return 0;
		case 1:	// Bless
		case 4: //Poison
		case 6:	// Teleport
		case 7:	// Unlock
		case 8:	// Wall of Stone
				break;
		default:	printf( "ERROR: cSkills::Inscribe -> Fallout of switch statement without default\n"); return( 0 );
		}
		break;
	case 4:
		switch (spl)
		{       
		case 1:// Arch Cure
		case 2:// Arch Protection
		case 7://Mana Drain
				sysmessage(s, "Sorry this spell is not implemented!");	return 0;
		case 3:	items[i].offspell = 8;	break;	// Curse
		case 4:// Fire Field
		case 5:// Greater Heal
		case 8:// Recall
				break;
		case 6:	items[i].offspell = 9;	break;	// Lightning
		default:	printf( "ERROR: cSkills::Inscribe -> Fallout of switch statement without default\n"); return( 0 );
		}
		break;
	case 5:
		switch(spl)
		{
		case 1://Blade Spirit
		case 2://Dispel Field
		case 4://Magic Reflection
		case 7://Poison Field
				break;
		case 3:	//Incognito
		case 8:	//Summon Creature
				sysmessage(s, "Sorry this spell is not implemented!");
				return 0;
		case 5:	items[i].offspell = 11;		break;	//Mind Blast
		case 6:	items[i].offspell = 12;		break;	//Paralyse
		default:	printf( "ERROR: cSkills::Inscribe -> Fallout of switch statement without default\n"); return( 0 );
		}
		break;
	case 6:
		switch(spl)
		{
		case 1://Dispel
		case 4://Invisibility
		case 5://Mark
		case 7:// Paralyze Field
		case 8://Reveal
				break;
		case 2:	items[i].offspell = 13;		break;  //Energy Bolt
		case 3:	items[i].offspell = 14;		break;	//Explosion
		case 6://Mass Curse
				sysmessage(s, "Sorry this spell is not implemented!");	return 0;
		default:	printf( "ERROR: cSkills::Inscribe -> Fallout of switch statement without default\n"); return( 0 );
		}
		break ;
	case 7:
		switch(spl)
		{
		case 1://Chain Lightning
		case 5:// Mana Vampire
		case 6:// Mass Dispel
		case 7:// Meteor Storm
		case 8:// Polymorph
				sysmessage(s, "Sorry this spell is not implemented!");	return 0;
		case 2://Energy Field
		case 4://Gate Travel
				break;
		case 3:	items[i].offspell = 15;	break;	// FlameStrike
		default:	printf( "ERROR: cSkills::Inscribe -> Fallout of switch statement without default\n"); return( 0 );
		}
		break ;
	case 8:
		switch(spl)
		{
		case 1:// Earthquake
				sysmessage(s, "Sorry this spell is not implemented!");	return 0;
		case 2://Energy Vortex
		case 3://Resurrection
		case 4:// Summon Air Elemental
		case 5://Summon Daemon
		case 6:// Summon Earth Elemental
		case 7:// Summon Fire Elemental
		case 8:// Summon Water Elemental
				break;
		default:	printf( "ERROR: cSkills::Inscribe -> Fallout of switch statement without default\n"); return( 0 );
		}
		break;
	default:
		printf( "ERROR: Fallout of switch statement without default. skills.cpp, inscribe()/n"); //Morrolan
		return(0);
  }
  return 1;
}

// Calculate the skill of this character based on the characters baseskill and stats
void cSkills::updateSkillLevel(int c, int s)
{
	int temp = (((skill[s].st * chars[c].st) / 100 +
		(skill[s].dx * chars[c].dx) / 100 +
		(skill[s].in * chars[c].in) / 100)
		*(1000-chars[c].baseskill[s]))/1000+chars[c].baseskill[s];
#ifdef _BORLAND_
	iTempBuff = chars[c].baseskill[s];
	chars[c].skill[s] = max( iTempBuff, temp );
#else
	chars[c].skill[s] = max(chars[c].baseskill[s], (unsigned short)temp);
#endif
}

void cSkills::LockPick( UOXSOCKET s )
{
	int i, success;

	i = calcItemFromSer( buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10] );
	if( i != -1 )
	{
		if(items[i].type==1 || items[i].type==12 || items[i].type==63) 
		{
			sysmessage(s, "That is not locked.");
			return;
		}
		
		if(items[i].type==8 || items[i].type==13 || items[i].type==64)
		{
			if(items[i].more1==0 && items[i].more2==0 && items[i].more3==0 && items[i].more4==0)
			{ //Make sure it isn't an item that has a key (i.e. player house, chest..etc)
				if(addmitem[s]==-1) 
				{
					if (currentSpellType[s] !=2)              // not a wand cast
					{
						success=Magic->SubtractMana(currchar[s], 5);  // subtract mana on scroll or spell
						if (currentSpellType[s] == 0)             // del regs on normal spell
						{
							reag_st toDel;
							memset( &toDel, 0, sizeof( reag_st ) );
							toDel.ash = 1;
							toDel.moss = 1;
							Magic->NewDelReagents( currchar[s], toDel );
						}
					}
					switch(items[i].type)
					{
					case 8: items[i].type=1; break;
					case 13: items[i].type=12; break;
					case 64: items[i].type=63; break;
					default:
						printf("ERROR: Fallout of switch statement without default. skills.cpp, lockpick()/n"); //Morrolan
						return;
					}
					soundeffect3(i, 0x01, 0xFF);
					sysmessage(s, "You manage to pick the lock.");
				} else
					if(Skills->CheckSkill(currchar[s], LOCKPICKING, 0, 1000))
					{
						switch(items[i].type)
						{
						case 8: items[i].type=1; break;
						case 13: items[i].type=12; break;
						case 64: items[i].type=63; break;
						default:
							printf("ERROR: Fallout of switch statement without default. skills.cpp, inscribe()/n"); //Morrolan
							return;
						}
						soundeffect3(i, 0x01, 0xFF);
						sysmessage(s, "You manage to pick the lock.");
					} else
					{
						if((rand()%100)>50) 
						{
							sysmessage(s, "You broke your lockpick!");
							Items->DeleItem(addmitem[s]);
						} else
						{
							sysmessage(s, "You fail to open the lock.");
						}
					}
			} else
			{
				sysmessage(s, "That cannot be unlocked without a key.");
			}
		}
	}
}

void cSkills::TDummy(int s)
{
	int j, serial;
	int type = Combat->GetBowType( currchar[s] );
	
	if (type > 0)
	{
		sysmessage(s, "Practice archery on archery buttes !");
		return;
	}    
	int skillused = Skills->GetCombatSkill(currchar[s]);
	
	if (chars[currchar[s]].onhorse)
		Combat->CombatOnHorse(currchar[s]);
	else
		Combat->CombatOnFoot(currchar[s]);
	
	switch( RandomNum( 0, 2 ) )
	{
	case 0: soundeffect( s, 0x01, 0x3B );		break;
	case 1: soundeffect( s, 0x01, 0x3C );		break;        
	case 2: soundeffect( s, 0x01, 0x3D );		break;
	}            
	serial = calcserial((buffer[s][1]&0x7F),buffer[s][2],buffer[s][3],buffer[s][4]);
	j = calcItemFromSer( serial );
	if (j!=-1)
	{
		if (items[j].id2==0x70) items[j].id2=0x71;
		if (items[j].id2==0x74) items[j].id2=0x75;
		tempeffect2(0, j, 14, 0, 0, 0);
		RefreshItem( j ); // AntiChrist
	}
	if(chars[currchar[s]].skill[skillused] < 300)
	{
		Skills->CheckSkill(currchar[s],skillused, 0, 1000);
		if(chars[currchar[s]].skill[TACTICS] < 300)
			Skills->CheckSkill(currchar[s],TACTICS, 0, 250);  //Dupois - Increase tactics but only by a fraction of the normal rate
	}
	else
		sysmessage(s, "You feel you would gain no more from using that.");   
}                           


void cSkills::NewDummy(unsigned int currenttime)
{
	unsigned int i;//,j;
	for (i=0;i<itemcount;i++)
	{
		if(((items[i].id1==0x10) && (items[i].id2==0x71)) && (items[i].gatetime<=currenttime)) 
		{
			items[i].id2=0x70;
			items[i].gatetime=0;
			RefreshItem( i ); // AntiChrist
		} 
		else 
		{
			if(((items[i].id1==0x10) && (items[i].id2==0x75)) && (items[i].gatetime<=currenttime)) 
			{
				items[i].id2=0x74;
				items[i].gatetime=0;
				RefreshItem( i ); // AntiChrist
			} 
		}
	}
}    

void CollectAmmo(int s, int a, int b)
{
	int c; 
	
	if (a)
	{
		c=Items->SpawnItem(s,a,"#",1,0x0F,0x3F,0,0,1,1);
		if( c == -1 )
			return;
		items[c].att=0;
		sysmessage(s,"You collect the arrows.");
	}
	
	if (b)
	{
		c=Items->SpawnItem(s,b,"#",1,0x1B,0xFB,0,0,1,1);
		if( c == -1 ) return;
		items[c].att=0;
		sysmessage(s,"You collect the bolts.");
	}
}

void cSkills::Tinkering(int s)
{
	int i, packnum, amt, serial;
	
	packnum=packitem(currchar[s]);
	
	if (packnum==-1) {sysmessage(s,"Time to buy a backpack"); return; } //LB
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	if (i!=-1)
	{
		if(((items[i].id1==0x1B)&&((items[i].id2==0xEF)||(items[i].id2==0xF2)))||
			((items[i].id1==0x1B)&&((items[i].id2==0xDD)||(items[i].id2==0xE0))))
		{
			if( GetPackOwner( i ) != currchar[s] )
			{
				sysmessage(s,"You can't use material outside your backpack");
			}           
			else
			{
				itemmake[s].has=amt=getamount(currchar[s], items[i].id1, items[i].id2);    
				if(amt<2)
				{ 
					sysmessage(s,"You don't have enough ingots to make anything.");
					return;
				}
				itemmake[s].materialid1=items[i].id1;
				itemmake[s].materialid2=items[i].id2; 
				if ((items[i].id1==0x1B)&&(items[i].id2==0xDD)||(items[i].id2==0xE0))
				{
					if (amt<4)
					{
						sysmessage(s,"You don't have enough log's to make anything.");
						return;
					} 
					else Skills->MakeMenu(s,70,TINKERING);
				}
				else Skills->MakeMenu(s,80,TINKERING);
			}
			return;
		}
	}     
	sysmessage(s,"You cannot use that material for tinkering.");
}

void cSkills::AButte(int s1, int x)
{
	int v1,/*p,*/i,c;
	int arrowsquant=0;
	int type=Combat->GetBowType(currchar[s1]);
	
	if(items[x].id2==0x0a)
	{ // East Facing Butte
		if ((items[x].x>chars[currchar[s1]].x)||(items[x].y!=chars[currchar[s1]].y))
			v1=-1;
		else v1=chars[currchar[s1]].x-items[x].x;
	}
	else
	{ // South Facing Butte
		if ((items[x].y>chars[currchar[s1]].y)||(items[x].x!=chars[currchar[s1]].x))
			v1=-1;
		else v1=chars[currchar[s1]].y-items[x].y;
	}
	
	if(v1==1)
	{
		//p=packitem(currchar[s1]);
		if(items[x].more1>0)
		{
			c=Items->SpawnItem(s1,items[x].more1,"#",1,0x0F,0x3F,0,0,1,0);
			if( c == -1 ) return;
			RefreshItem( c ); // AntiChrist
		}
		
		if(items[x].more2>0)
		{
			c=Items->SpawnItem(s1,items[x].more2,"#",1,0x1B,0xFB,0,0,1,0);
			if( c == -1 ) return;
			RefreshItem( c ); // AntiChrist
		}
		
		i=0;
		if(items[x].more1>0) i++;
		if(items[x].more2>0) i+=2;
		
		switch(i)
		{
		case 0:
			strcpy(temp,"This target is empty");
			break;
		case 1:
			sprintf(temp,"You pull %d arrows from the target",items[x].more1);
			break;
		case 2:
			sprintf(temp,"You pull %d bolts from the target",items[x].more2);
			break;
		case 3:
			sprintf(temp,"You pull %d arrows and %d bolts from the target",items[x].more1,items[x].more2);
			break;
		default:
			printf("ERROR: Fallout of switch statement without default. skills.cpp, AButte()/n"); //Morrolan
			return;
		}
		sysmessage(s1,temp);
		items[x].more1=0;
		items[x].more2=0;
	}
	
	if((v1>=5)&&(v1<=8))
	{
		if (type == 0)
		{
			sysmessage(s1, "You need to equip a bow to use this.");
			return;
		} 
		if ((items[x].more1+items[x].more2)>99)
		{
			sysmessage(s1, "You should empty the butte first!");
			return;
		}
		if (type==1) arrowsquant=getamount(currchar[s1], 0x0F, 0x3F);
		else arrowsquant=getamount(currchar[s1], 0x1B, 0xFB);
		if (arrowsquant==0) 
		{
			sysmessage(s1, "You have nothing to fire!");
			return;
		}
		if (type==1) 
		{
			delequan(currchar[s1], 0x0F, 0x3F, 1);
			items[x].more1++;
			//add moving effect here to item, not character
		}
		else
		{
			delequan(currchar[s1], 0x1B, 0xFB, 1);
			items[x].more2++;
			//add moving effect here to item, not character
		} 
		if (chars[currchar[s1]].onhorse) Combat->CombatOnHorse(currchar[s1]);
		else Combat->CombatOnFoot(currchar[s1]);
		
		if (chars[currchar[s1]].skill[ARCHERY] < 350)
			Skills->CheckSkill(currchar[s1],ARCHERY, 0, 1000);
		else
			sysmessage(s1, "You learn nothing from practicing here");
		
		switch((chars[currchar[s1]].skill[ARCHERY]+((rand()%200)-100))/100)
		{
		case -1:
		case 0:
		case 1:
			sysmessage(s1, "You miss the target");
			soundeffect(s1, 0x02, 0x38);
			break;
		case 2:
		case 3:
			sysmessage(s1, "You hit the outer ring!");
			soundeffect(s1, 0x02, 0x34);
			break;
		case 4:
		case 5:
		case 6:
			sysmessage(s1, "You hit the middle ring!");
			soundeffect(s1, 0x02, 0x34);
			break;
		case 7:
		case 8:
		case 9:
			sysmessage(s1, "You hit the inner ring!");
			soundeffect(s1, 0x02, 0x34);
			break;
		case 10:
		case 11:
			sysmessage(s1, "You hit the bullseye!!");
			soundeffect(s1, 0x02, 0x34);
			break;
		default:
			break;
		}
	}
	else
		sysmessage( s1, "You can't use that from here." );
}

void cSkills::TinkerAxel(int s)
{
	int i,c,serial;
	long int origPart, origPartSer = -1;
	unsigned long int p1id, p2id;	// ids of the 2 parts;
	int tailme=0;
	origPartSer = calcserial( addid1[s], addid2[s], addid3[s], addid4[s] );
	origPart = findbyserial( &itemsp[origPartSer%HASHMAX], origPartSer, 0 );
	if( origPart == -1 )
	{
		sysmessage( s, "Original part no longer exists" );
		return;
	}
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	if (i!=-1)
	{
		if( i == origPart )
		{
			sysmessage( s, "You can't combine an item with itself!" );
			return;
		}
		p1id = (unsigned long int)items[origPart].id1<<8;
		p1id += (unsigned int)items[origPart].id2;
		p2id = (unsigned long int)items[i].id1<<8;
		p2id += items[i].id2;
		
		switch( p1id )
		{
		case 0x1053:
		case 0x1054:	// gears
			if( p2id == 0x105B || p2id == 0x105C )
				tailme = 2;	// combine these
			else
				tailme = 0;	// can't combine
			break;
		case 0x105B:
		case 0x105C:	// axles
			if( p2id == 0x1053 || p2id == 0x1054 )
				tailme = 2;	// combine these
			else
				tailme = 0;	// can't combine
			break;
		default:	// shouldn't get here
			sysmessage( s, "You shouldn't have got here cSkills::TinkerAxel()" );
			return;
			break;
		}
		if( tailme == 2 && iteminrange( s, i, 3 ) )
		{
			if( !Skills->CheckSkill( currchar[s], TINKERING, 0, 1000 ) )
			{
				sysmessage( s, "You failed to combine the parts." );
			}
			else
			{
				sysmessage(s,"You combined the parts");
				if( items[origPart].amount > 1 )
				{
					items[origPart].amount--;
					RefreshItem( origPart );
				}
				else
					Items->DeleItem( origPart );	// we delete the part ONLY on success
				if( items[i].amount > 1 )
				{
					items[i].amount--;
					RefreshItem( i );
				}
				else
					Items->DeleItem( i );			// delete the parts 
				c=Items->SpawnItem(s,1,"an axle with gears",0,0x10,0x51,0,0,1,1);
				if( c == -1 ) return;
			}
		}
	} 
	chars[currchar[s]].tailitem=-1;        
	if(!tailme) sysmessage(s,"You can't combine these.");
}

void cSkills::TinkerAwg(int s)
{
	int i,c,packnum,serial;
	char id2;
	
	packnum=packitem(currchar[s]);
	
	if (packnum==-1) 
	{
		sysmessage(s,"Time to buy a backpack"); 
		return; 
	}
	
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	if (i!=-1)
	{
		if (((items[i].id1==0x10) && (items[i].id2==0x51 || items[i].id2==0x52)) &&
			((itemmake[s].materialid1==0x10) && (itemmake[s].materialid2==0x5D || itemmake[s].materialid2==0x5E)) ||
			((items[i].id1==0x10) && (items[i].id2==0x5D || items[i].id2==0x5E)) &&
			((itemmake[s].materialid1==0x10) && (itemmake[s].materialid2==0x51 || itemmake[s].materialid2==0x52)))
			id2=0x4F;
		else if (((items[i].id1==0x10) && (items[i].id2==0x51 || items[i].id2==0x52)) &&
			((itemmake[s].materialid1==0x10) && (itemmake[s].materialid2==0x55 || itemmake[s].materialid2==0x56)) ||
			((items[i].id1==0x10) && (items[i].id2==0x55 || items[i].id2==0x56)) &&
			((itemmake[s].materialid1==0x10) && (itemmake[s].materialid2==0x51 || itemmake[s].materialid2==0x52)))
			id2=0x59;
		else
		{
			sysmessage(s,"You can't combine these.");
			return;
		}
		if( GetPackOwner( i ) != currchar[s] )
		{
			sysmessage(s,"You can't combine items outside your backpack");
			return;
		}
		else if (!CheckSkill(currchar[s],TINKERING, 0, 1000))
		{
			sysmessage(s,"Your fingers slip and you break one of the parts.");
			delequan(currchar[s], items[i].id1, items[i].id2, 1);
			return;
		}
		else
		{
			if (id2==0x4F) strcpy(temp,"clock parts");
			if (id2==0x59) strcpy(temp,"sextant parts");
			c=Items->SpawnItem(s,1,temp,0,0x10,id2,0,0,1,1);
			if( c == -1 ) return;
			sysmessage(s,"You combined the parts");
		}
		delequan(currchar[s], itemmake[s].materialid1, itemmake[s].materialid2, 1);
		delequan(currchar[s], items[i].id1, items[i].id2, 1);
	}
}

void cSkills::TinkerClock(int s)
{
	int i,c,serial;
	
	int tailme=0;
	
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
	if (i!=-1)
	{
		if((items[i].id1==0x10)&&(items[i].id2==0x4D)||(items[i].id2==0x4E)||
			(items[i].id1==0x10)&&(items[i].id2==0x4F)||(items[i].id2==0x50))
		{
			if(iteminrange(s,i,3))  
			{
				if (!Skills->CheckSkill(currchar[s],TINKERING, 0, 1000)) 
				{
					sysmessage(s,"You failed to combine the parts.");
				}
				else
				{
					sysmessage(s,"You combined the parts");
					
					c=Items->SpawnItem(s,1,"clock",0,0x10,0x4B,0,0,1,1);
					if( c == -1 ) return;
				}
				if(items[i].amount==1)
				{
					Items->DeleItem(i);
				}
				tailme=1;
			}
		} 
	}   
	chars[currchar[s]].tailitem=-1;        
	if(!tailme) sysmessage(s,"You can't combine these.");
}

void cSkills::Meditation( UOXSOCKET s ) // Morrolan - meditation(int socket)
{
	int weapon = Combat->GetWeapon( currchar[s] );
	if( weapon != -1 )
	{
		if( items[weapon].type == 9 )	// allow us to hold a spellbook
			weapon = -1;
	}
		
	if( Combat->CalcAtt( currchar[s] ) >= 15 )
	{
		sysmessage(s, "The energies cannot be absorbed through your armor.");
		chars[currchar[s]].med = 0;
		return;
	}
	else if( weapon != -1 || GetShield( currchar[s] ) != -1 )
	{
		sysmessage(s, "You cannot meditate with a weapon or shield equipped!");
		chars[currchar[s]].med = 0;
		return;
	}
	else if( chars[currchar[s]].mn == chars[currchar[s]].in )
	{
		sysmessage(s, "You are at peace.");
		chars[currchar[s]].med = 0;
		return;
	}
	else if( !CheckSkill( currchar[s], MEDITATION, 0, 1000 ) ) 
	{
		sysmessage(s, "You cannot focus your concentration.");
		chars[currchar[s]].med = 0;
		return;
	}
	else 
	{
		sysmessage( s, "You enter a meditative trance." );
		chars[currchar[s]].med = 1;
		soundeffect(s, 0x00, 0xf9);
		return;
	}
}

// AntiChrist - 5/11/99
//
//If you are a ghost and attack a player, you can PERSECUTE him
//and his mana decreases each time you try to persecute him
//decrease = 3 + (your int/10)
//
void cSkills::Persecute( UOXSOCKET s ) // AntiChrist - persecute stuff
{
	int target = chars[currchar[s]].targ;
	int c = currchar[s];
	if( chars[target].priv&1 ) return;
	
	int decrease=(chars[c].in/10)+3;
	
	if( ( chars[currchar[s]].skilldelay <= uiCurrentTime ) || ( chars[currchar[s]].priv&1 ) )
	{
		if((( rand()%20 ) + chars[c].in ) > 45 ) // not always
		{
			chars[target].mn -= decrease; // decrease mana
			updatestats( target, 1 ); // update
			sysmessage( s, "Your spiritual forces disturb the enemy!" );
			sysmessage( calcSocketFromChar( target ), "A damned soul is disturbing your mind!" );
			chars[currchar[s]].skilldelay = (unsigned int)(uiCurrentTime + ( server_data.skilldelay * CLOCKS_PER_SEC ));
			sprintf( temp, "%s is persecuted by a ghost!!", chars[target].name );
			for( int j = 0; j < now; j++ )
			{
				if( perm[j] && s != j && inrange1( s, j ) )
				{
					chars[currchar[s]].emotecolor1 = 0x00;
					chars[currchar[s]].emotecolor2 = 0x26;
					npcemote( j, target, temp, 1 );
				}
			}
		}
		else
		{
			sysmessage( s, "Your mind is not strong enough to disturb the enemy." );
		}
	}
	else
	{
		sysmessage( s, "You are unable to persecute him now... rest a little... " );
	}
}

//Snooping skill
// s -> Char's socket using the skill
// target -> Char's beeing snooped on
// serial -> container's serial
void cSkills::Snooping( UOXSOCKET s, CHARACTER target, SERIAL serial ) 
{
	char mChar = currchar[s];
	char temp[64];

	UOXSOCKET tSock = calcSocketFromChar( target );

	if( (chars[target].priv&0x80) || (chars[target].priv&0x01) ) // Snooping Staff, not a good idea.
	{
		sysmessage( s, "You failed to peek into that container." );
		sprintf( temp, "%s is snooping you!", chars[mChar].name );
		sysmessage( tSock, temp );
		return;
	}
	
	if( Skills->CheckSkill( mChar, SNOOPING, 0, 1000 ) )
	{
		backpack( s , (serial>>24), (serial>>16), (serial>>8), (serial%256) );
		sysmessage( s, "You successfully peek into that container." );
	}
	else 
	{
		sysmessage( s, "You failed to peek into that container." );
		if( chars[target].npc )
		{
			if( ishuman( target ) )	// Human shaped NPC?
			{
				switch( RandomNum( 0, 2 ) )
				{
					case 0:	strcpy( temp, "Art thou attempting to disturb my privacy?" );	break;
					case 1:	strcpy( temp, "Stop that!" );									break;
					case 2:	strcpy( temp, "Be aware I am going to call the guards!" );		break;
				}
				npctalk( s, target, temp, 0 );
				if( server_data.snoopiscrime )
				{
					if( RandomNum( 0, 100 ) < 50 && chars[mChar].crimflag > 0 )	//	50% chance of calling guards, on second time
						callguards( mChar );
				}
			}
			else
				playmonstersound( target, chars[target].id1, chars[target].id2, SND_IDLE );	// Play idle sound, if not human
		}
		else	// Must be a PC
		{
			sprintf( temp, "You notice %s trying to peek into your pack!", chars[mChar].name );
			sysmessage( tSock, temp );
		}
		if( server_data.snoopiscrime )
		{
			criminal( mChar );
		}
		if( chars[mChar].karma <= 1000 )
		{
			chars[mChar].karma -= 10;
			sysmessage( s, "You've lost a small bit of karma" );
		}
	}
}