//------------------------------------------------------------------------
//  gumps.cpp
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
#include <uox3.h>
#include <debug.h>

#define DBGFILE "gumpmenus.cpp"

void cGump::Button(int s, int button, unsigned char tser1, unsigned char tser2, unsigned char tser3, unsigned char tser4, char type)
{
	int j=-1,serial,i;
	
	if( button > 10000 ) 
	{
		i = button - 10000;
		Menu( s, i );
		return;
	}
	int curLock = 0, b = 0, a = 0;
	bool done = false;
	serial = calcserial( tser1, tser2, tser3, tser4 );
	if (type > 11) 
		return; //increase this value with each new gump added.
	if (button==1) 
	{
		if( type == 4 )
			WhoList->GMLeave();
		else if( type == 11 )
			OffList->GMLeave();
		return;
	}
	int is;
	// gump types
	// 1	Tweak Item (needs bugfixing/expanding)
	// 2	Tweak Char (needs bugfixing/expanding)
	// 3	Townstones
	// 4	WhoList
	// 5	House functions (TBR)
	// 7	Accounts (TBR)
	// 10	Safe sex window (TBD)
	// 11	Who's Offline
	switch( type )
	{
		case 7:
		case 2:	
			j = calcCharFromSer( serial );
			break;
		case 1:
			j = calcItemFromSer( serial );
			break;
		case 6:
			is = calcserial(addid1[s],addid2[s],addid3[s],addid4[s]);
			j = calcItemFromSer( is );
			break;
		case 3:	
		case 4:
		case 11:
			j = 1; //cj
			break;
		case 5:	// House funcs, WILL EVENTUALLY DISAPPEAR
			is = calcserial(addid1[s],addid2[s],addid3[s],addid4[s]);
			j = calcItemFromSer( is );
			if(j>-1) 
			{
				is = calcserial( items[j].more1, items[j].more2, items[j].more3, items[j].more4 ); 
				j = calcItemFromSer( is );
			}
			break;
	}
#ifdef DEBUG
	printf("Type is... %i button is %i\n", type, button );
#endif

	if( button == 1 )
	{
		if( type == 4 )
			WhoList->GMLeave();
		else if( type == 11 )
			OffList->GMLeave();
		return;
	}
	if( j == -1 ) 
		return;
	if( type == 7 ) // Revana*
	{
		if( button == 2 )
		{
			if( chars[currchar[s]].priv&1 && !chars[j].npc )
			{
				sysmessage( s, "You have banned that players account!" );
				sysmessage( j, "You have been banned!" );
				acctx[chars[j].account].ban = 1;
				acctx[chars[j].account].banSerial = chars[currchar[s]].serial;
				if( online( j ) ) 
					Network->Disconnect( j );
			}
			else 
				sysmessage( s, "That player cannot be banned!" );
		}
		if( button == 3 )
		{
			if( chars[currchar[s]].priv&1 && !chars[j].npc )
			{
				sysmessage( s, "You have given that player's account a time ban!" );
				sysmessage( j, "You have been banned for 24 hours!" );
				acctx[chars[j].account].ban = 1;
				acctx[chars[j].account].banTime = (int)(uiCurrentTime + (60 * 60 * 24 * CLOCKS_PER_SEC ));

				if( online( j ) ) 
					Network->Disconnect( j );
			}
			else 
				sysmessage( s, "That player cannot be banned!" );
		}
		if( button == 4 )
		{
			if( chars[currchar[s]].priv&1 && !chars[j].npc )
			{
				sysmessage( s, "Account is being wiped.." );
				sysmessage( j, "Your account no longer exist." );
				acctx[chars[j].account].wipe = 1;
				acctx[chars[j].account].wipeSerial = chars[currchar[s]].serial;

				if( online( j ) ) 
					Network->Disconnect( j );
			}
			else 
				sysmessage( s, "That player's account cannot be banned!" );
		}
		if( button == 5 )
		{
			if( chars[currchar[s]].priv&1 && !chars[j].npc )
			{
				sysmessage( s, "Locking account players for this shard.." );
				sysmessage( j, "All your characters are being locked out.." );
				for( b=0; b < charcount; b++ )
				{
					if( chars[b].account == acctno[j] && curLock < MAX_ACCT_LOCK)
					{
						acctx[chars[j].account].lock[curLock] = chars[b].serial;
						curLock++;
					}
				}
				printf("Locked %i characters from %i\n", curLock, acctno[b] );

				if( online( j ) ) 
					Network->Disconnect( j );
			}
			else 
				sysmessage( s, "That is not a player!" );
		}
		if( button == 6 )
		{
			if( chars[currchar[s]].priv&1 && !chars[j].npc )
			{
				sysmessage( s, "You have locked that character out" );
				sysmessage( j, "Your character has been locked out" );
				for( a = 0; a < MAX_ACCT_LOCK; a++ )
				{
					if( acctx[chars[j].account].lock[a] == 0 && done == false )
					{
						acctx[chars[j].account].lock[a] = chars[j].serial;
						done = true;
					}
				}
				if( done == false )
				{
					sysmessage( s, "Error in locking! Too many locked!" );
				}

				if( online( j ) ) 
					Network->Disconnect( j );
			}
		}
		if( button == 7 )
		{
			if( chars[currchar[s]].priv&1 && !chars[j].npc )
			{
				acctx[chars[j].account].warning++;
				sysmessage( s, "You have placed a warning" );
				if( acctx[chars[j].account].warning >= 10 )
				{
					sysmessage( j, "You have been banned for too many warnings!" );
					acctx[chars[j].account].ban = 1;
					Network->Disconnect( j );
				}
			}
		}
	}

	if (type==1) // Item
	{
		switch( button )
		{
		case 2:		entrygump( s, serial, type, button, 50, "Enter a new name for the item. (# = default name)" );		break;
		case 3:		entrygump( s, serial, type, button, 4, "Enter the new ID number for the item in hex." );			break;
		case 4:		entrygump( s, serial, type, button, 4, "Enter the new hue for the item in hex." );					break;
		case 5:		entrygump( s, serial, type, button, 4, "Enter the new X coordinate for the item in decimal." );		break;
		case 6:		entrygump( s, serial, type, button, 4, "Enter the new Y coordinate for the item in decimal." );		break;
		case 7:		entrygump( s, serial, type, button, 4, "Enter the new Z coordinate for the item in decimal." );		break;
		case 8:		entrygump( s, serial, type, button, 4, "Enter the new type for the item in decimal." );				break;
		case 9:		entrygump( s, serial, type, button, 4, "Enter the new itemhand for the item in decimal." );			break;//Xuri
		case 10:	entrygump( s, serial, type, button, 4, "Enter the new layer for the item in decimal." );			break;
		case 11:	entrygump( s, serial, type, button, 4, "Enter the new amount for the item in decimal." );			break;
		case 12:	entrygump( s, serial, type, button, 8, "Enter the new More for the item in hex." );					break;
		case 13:	entrygump( s, serial, type, button, 8, "Enter the new MoreB for the item in hex." );				break;
		case 14:	entrygump( s, serial, type, button, 1, "Enter the new stackable toggle for the item. (0/1)" );		break;
		case 15:	entrygump( s, serial, type, button, 1, "Enter the new dyeable toggle for the item. (0/1)" );		break;
		case 16:	entrygump( s, serial, type, button, 1, "Enter the new corpse toggle for the item. (0/1)" );			break;
		case 17:	entrygump( s, serial, type, button, 5, "Enter the new LODAMAGE value for the item in decimal." );	break;//|
		case 18:	entrygump( s, serial, type, button, 5, "Enter the new HIDAMAGE value for the item in decimal." );	break;//| both of these replace the old "attack value" (Xuri)
		case 19:	entrygump( s, serial, type, button, 5, "Enter the new defence value for the item in decimal." );	break;
		case 20:	entrygump( s, serial, type, button, 1, "Enter the new magic value for the item in decimal." );		break;
		case 21:	entrygump( s, serial, type, button, 1, "Enter the new visible value for the item in decimal." );	break;
		//start addons by Xuri
		case 22:	entrygump( s, serial, type, button, 5, "Enter the new HP value for the item in decimal." );			break;
		case 23:	entrygump( s, serial, type, button, 5, "Enter the new Max HP value for the item in decimal." );		break;
		case 24:	entrygump( s, serial, type, button, 5, "Enter the new Speed value for the item in decimal." );		break;
		case 25:	entrygump( s, serial, type, button, 5, "Enter the new Rank value for the item in decimal." );		break;
		case 26:	entrygump( s, serial, type, button, 5, "Enter the new Value for the item in decimal." );			break;
		case 27:	entrygump( s, serial, type, button, 5, "Enter the new Good value for the item in decimal." );		break;
		case 28:	entrygump( s, serial, type, button, 5, "Enter the new Made Skill value for the item in decimal." );	break;
		case 29:	entrygump( s, serial, type, button, 50, "Enter the new Creator name for the item." );				break;
		//end addons by Xuri
		default:	printf( "Unknown button pressed %i", button );														break;
		}
	}
	if( type == 2 ) // Char
	{
		switch( button )
		{
		case 2:	entrygump( s, serial, type, button, 50, "Enter a new Name for the character." );							break;
		case 3:	entrygump( s, serial, type, button, 50, "Enter a new Title for the character." );							break;
		case 4:	entrygump( s, serial, type, button, 4, "Enter a new X coordinate for the character in decimal." );			break;
		case 5:	entrygump( s, serial, type, button, 4, "Enter a new Y coordinate for the character in decimal." );			break;
		case 6:	entrygump( s, serial, type, button, 4, "Enter a new Z coordinate for the character in decimal." );			break;
		case 7:	entrygump( s, serial, type, button, 3, "Enter a new Direction for the character in decimal." );				break;
		case 8:	entrygump( s, serial, type, button, 4, "Enter a new Body Type for the character in hex." );					break;
		case 9:	entrygump( s, serial, type, button, 4, "Enter a new Skin Hue for the character in hex." );					break;
		case 10: entrygump( s, serial, type, button, 3, "Enter a new Defence value for the character in decimal." );		break;	
		case 11: entrygump( s, serial, type, button, 3, "Enter a new Race number for the character in decimal." );			break;	
		case 12: entrygump( s, serial, type, button, 1, "Enter a new Hunger value for the character in decimal(0-6).");		break;			
		case 13: entrygump( s, serial, type, button, 5, "Enter a new Strength value for the character in decimal.");		break;			
		case 14: entrygump( s, serial, type, button, 5, "Enter a new Dexterity value for the character in decimal.");		break;			
		case 15: entrygump( s, serial, type, button, 5, "Enter a new Intelligence value for the character in decimal.");	break;
		}
	}
	if (type==3) // Townstones
	{
		if (button==2) // Leave/Join
		{
			if (chars[currchar[s]].town)
			{
				//They are a resident of a region (not necessarily this one).
				if (chars[currchar[s]].town==calcRegionFromXY(items[j].x, items[j].y)) {
					//They are a resident of this region, set their .town property to 0.
					if(chars[currchar[s]].townpriv==2) { //They were a mayor. Find new one.
						chars[currchar[s]].townpriv=0;
						Towns->CalcNewMayor(chars[currchar[s]].town);
					}
					chars[currchar[s]].town=0;
					chars[currchar[s]].townpriv=0;
				} else {
					//They are not a resident of this region, set their .town property to
					//the region associated with the townstone.
					if(chars[currchar[s]].townpriv==2) { //They were a mayor. Find new one.
						chars[currchar[s]].townpriv=0;
						Towns->CalcNewMayor(chars[currchar[s]].town);
					}
					chars[currchar[s]].town=calcRegionFromXY(items[j].x, items[j].y);
					chars[currchar[s]].townpriv=1;
					Towns->Menu(s, j, 3);
				}
			} else {
				//They are not a resident of any region, set their .town property to the
				//region associated with this townstone.
				chars[currchar[s]].town=calcRegionFromXY(items[j].x, items[j].y);
				chars[currchar[s]].townpriv=1;
				Towns->Menu(s, j, 3);
			}
			chars[currchar[s]].townvote1=0;
			chars[currchar[s]].townvote2=0;
			chars[currchar[s]].townvote3=0;
			chars[currchar[s]].townvote4=0;
			chars[currchar[s]].towntitle=0;
		}
		if (button==3) { // Toggle Title
			if(chars[currchar[s]].towntitle) {
				chars[currchar[s]].towntitle=0;
			} else {
				chars[currchar[s]].towntitle=1;
			}
			Towns->Menu(s, j, 3);
		}
		if (button==4) { // Vote for Mayor
			target(s,0,1,0,161,"Select person to vote for.");
		}
	}
	if (type==4)
	{
		WhoList->ButtonSelect( s, button, type );
		return;
	}
	if( type == 11 )
	{
		OffList->ButtonSelect( s, button, type );
		return;
	}
	if( type == 6 )
	{
		// we only HAVE one button, so we'll assume safely that the colour has changed if we hit here
		short int dyeColour;
		dyeColour = ( buffer[s][21]<<8 ) + buffer[s][22];	// radio selection
		UseHairDye( s, dyeColour, j );
		return;
	}
	if(type==5) { // House Signs - Crackerjack 8/8/99
		i=(buffer[s][21]<<8)+buffer[s][22];
		if(button!=20) {
			addid1[s]=items[j].ser1;
			addid2[s]=items[j].ser2;
			addid3[s]=items[j].ser3;
			addid4[s]=items[j].ser4;
		}
		switch(button)
		{
			case 20: // Change house sign's appearance
				if(i>0) {
					buffer[s][7]=addid1[s];
					buffer[s][8]=addid2[s];
					buffer[s][9]=addid3[s];
					buffer[s][10]=addid4[s];
					addid1[s]=buffer[s][21];
					addid2[s]=buffer[s][22];
					Targ->IDtarget(s);
					sysmessage(s, "House sign changed.");
				}
				return;
			case 0:
				return;
			case 2:  // Bestow ownership upon someone else
				target(s, 0, 1, 0, 227, "Select person to transfer ownership to.");
				return;
			case 3:  // Turn house into a deed
				deedhouse(s, j);
				return;
			case 4:  // Kick someone out of house
				target(s, 0, 1, 0, 228, "Select person to eject from house.");
				return;
			case 5:  // Ban somebody
				target(s, 0, 1, 0, 229, "Select person to ban from house.");
				return;
			case 6:
			case 8: // Remove someone from house list
				target(s, 0, 1, 0, 231, "Select person to remove from house registry.");
				return;
			case 7: // Make someone a friend
				target(s, 0, 1, 0, 230, "Select person to make a friend of the house.");
				return;
			default:
				sprintf(temp, "HouseGump Called - Button=%i", button);
				sysmessage(s, temp);
				return;

		}
	}
}

void cGump::Input( UOXSOCKET s )
{
	char type, index, tser1, tser2, tser3, tser4;
	char *text;
	int j, k, serial; // Removed unreferenced local variables
	
	type=buffer[s][7];
	index=buffer[s][8];
	tser1=buffer[s][3];
	tser2=buffer[s][4];
	tser3=buffer[s][5];
	tser4=buffer[s][6];
	text = (char *)&buffer[s][12];
	serial=calcserial(tser1,tser2,tser3,tser4);

	Guilds->GumpInput(s,type,index,text);

	if (type==1 && (chars[currchar[s]].priv&1))
	{
		j = calcItemFromSer( serial );
		if( j == -1 ) 
			return;
		if( buffer[s][9] == 0 )
		{
			tweakmenu(s, j, type);
			return;
		}
		switch( index )
		{
		case 2:		strcpy( items[j].name, text );	break;	 // Name
		case 3:		k = hstr2num( text );	
					items[j].id1 = (unsigned char)(k>>8);	
					items[j].id2 = (unsigned char)(k%256); 
					break;	 // ID
		case 4:		k = hstr2num( text );	
					items[j].color1 = (unsigned char)(k>>8);
					items[j].color2 = (unsigned char)(k%256); 
					break;	// Hue
		case 5:		k = str2num( text );	items[j].x = k;	break;	// X
		case 6:		k = str2num( text );	items[j].y = k;	break;	// Y
		case 7:		k = str2num( text );	items[j].z = k;	break;	// Z
		case 8:		k = str2num( text );	items[j].type = k;	break;	 // Type
		case 9:		k = str2num( text );	items[j].itmhand = k;	break;	// Itemhand - added by Xuri
		case 10:	k = str2num( text );	items[j].layer = k;	break;	// Layer
		case 11:	k = str2num( text );	items[j].amount = k;	break;	// Amount
		case 12:	k = hstr2num( text );	// More
					items[j].more1 = (unsigned char)(k>>24);
					items[j].more2 = (unsigned char)(k>>16);
					items[j].more3 = (unsigned char)(k>>8);
					items[j].more4 = (unsigned char)(k%256);
					break;
		case 13: 	k = hstr2num( text );	// MoreB
					items[j].moreb1 = (unsigned char)(k>>24);
					items[j].moreb2 = (unsigned char)(k>>16);
					items[j].moreb3 = (unsigned char)(k>>8);
					items[j].moreb4 = (unsigned char)(k%256);
					break;
		case 14: 	k = str2num( text );	items[j].pileable = k;	break;	// Pileable
		case 15:	k = str2num( text );	items[j].dye = k;		break;	// Dye
		case 16:	k = str2num( text );	items[j].corpse = k;	break;	// Corpse
		case 17:	k = str2num( text );	items[j].lodamage = k;	break;	// LoDamage
		case 18:	k = str2num( text );	items[j].hidamage = k;	break;	// HiDamage
		case 19:	k = str2num( text );	items[j].def = k;		break;	// Def
		case 20:	k = str2num( text );	items[j].magic = k;		break;	// Magic
		case 21:	k = str2num( text );	items[j].visible = k;	break;	// Visible
		case 22:	k = str2num( text );	items[j].hp = k;		break;	// Current Hitpoints
		case 23:	k = str2num( text );	items[j].maxhp = k;		break;	// MAX Hitpoints
		case 24:	k = str2num( text );	items[j].spd = k;		break;	// Speed (for Combat)
		case 25:	k = str2num( text );	items[j].rank = k;		break;	// Rank
		case 26:	k = str2num( text );	items[j].value = k;		break;	// Value
		case 27:	k = str2num( text );	items[j].good = k;		break;	// Good(for Adv.Trade system)
		case 28:	k = str2num( text );	items[j].madewith = k;	break;	// Made Skill
		case 29:	strcpy( items[j].creator, text );				break;	// Creator
		}
		RefreshItem( j ); // AntiChrist
		tweakmenu( s, j, type );
	}
	if( type == 2 && ( chars[currchar[s]].priv&1 ) )
	{
		j = calcCharFromSer( serial );
		if( j == -1 ) 
			return;
		if( buffer[s][9] == 0 )
		{
			tweakmenu(s, j, type);
			return;
		}
		switch( index )
		{
		case 2:		strcpy( chars[j].name, text );			break;	// Name
		case 3:		strcpy( chars[j].title, text );			break;	// Title
		case 4:		k = str2num( text );	chars[j].x = k;	break;	// X
		case 5:		k = str2num( text );	chars[j].y = k;	break;	// Y
		case 6:		k = str2num( text ); 	chars[j].z = k;	chars[j].dispz = k;	break;	// Z
		case 7:		k = str2num( text );	chars[j].dir = k&0x0F;	// make sure the high-bits are clear // Dir
		case 8: // Body
			k = hstr2num( text );
			if( k >= 0x000 && k <= 0x3e1 ) // body-values >0x3e crash the client
			{ 
				chars[j].orgid1 = chars[j].xid1 = chars[j].id1 = (unsigned char)(k>>8); // allow only non crashing ones
				chars[j].orgid2 = chars[j].xid2 = chars[j].id2 = (unsigned char)(k%256);
			}
			break;
		case 9:		k = hstr2num( text );		// Skin
					chars[j].xskin1 = chars[j].skin1 = (unsigned char)(k>>8);
					chars[j].xskin2 = chars[j].skin2 = (unsigned char)(k%256);
					break;
		case 10:	k = str2num( text );	chars[j].def = k;	break;	// Defence
		case 11:	k = str2num( text );	chars[j].race = k;	break;	// Race
		case 12:	k = str2num( text );	chars[j].hunger = k;break;	// Hunger
		case 13:	k = str2num( text );	chars[j].st = k;	break;	// Strength
		case 14:	k = str2num( text );	chars[j].dx = k;	break;	// Dexterity
		case 15:	k = str2num( text );	chars[j].in = k;	break;	// Intelligence
		}
		teleport( j );
		tweakmenu( s, j, type );
	}
}


void cGump::Menu(int s, int m)
{
	char sect[512];
	short int length, length2, textlines=0;
	int i;
	
	openscript("misc.scp");
	sprintf(sect, "GUMPMENU %i", m);
	if (!i_scripts[misc_script]->find(sect))
	{
		closescript();
		return;
	}
	length=21;
	length2=1;
	do
	{
		read1();
		if ((script1[0]!='}')&&(strncmp(script1, "type ", 5)!=0))
		{
			length+=strlen(script1)+4;
			length2+=strlen(script1)+4;
		}
	}
	while (script1[0]!='}');
	length+=3;
	sprintf(sect, "GUMPTEXT %i", m);
	if (!i_scripts[misc_script]->find(sect))
	{
		closescript();
		return;
	}
	do
	{
		read1();
		if (script1[0]!='}')
		{
			length+=(strlen(script1)*2)+2;
			textlines++;
		}
	}
	while (script1[0]!='}');
	gump1[1]=length>>8;
	gump1[2]=length%256;
	gump1[3]=chars[currchar[s]].ser1;
	gump1[4]=chars[currchar[s]].ser2;
	gump1[5]=chars[currchar[s]].ser3;
	gump1[6]=chars[currchar[s]].ser4;
	gump1[7]=0;
	gump1[8]=0;
	gump1[9]=0;
	gump1[10]=0x12; // Gump Number
	gump1[19]=length2>>8;
	gump1[20]=length2%256;
	sprintf(sect, "GUMPMENU %i", m);
	if (!i_scripts[misc_script]->find(sect))
	{
		closescript();
		return;
	}
	// typecode setting - Crackerjack 8/8/99
	read1();
	if(!strncmp(script1, "type ", 5)) {
		gump1[10]=atoi(&script1[5]);
		script1[0]='}';
	}
	Network->xSend(s, gump1, 21, 0);
	do
	{
		if(script1[0]!='}') {
			sprintf(sect, "{ %s }", script1);
//			printf("GUMP: %s\n", sect);
			Network->xSend(s, sect, strlen(sect), 0);
		}
		read1();
	}
	while (script1[0]!='}');
	gump2[1]=textlines>>8;
	gump2[2]=textlines%256;
	Network->xSend(s, gump2, 3, 0);
	sprintf(sect, "GUMPTEXT %i", m);
	if (!i_scripts[misc_script]->find(sect))
	{
		closescript();
		return;
	}
	do
	{
		read1();
		if (script1[0]!='}')
		{
			gump3[0]=strlen(script1)>>8;
			gump3[1]=strlen(script1)%256;
			Network->xSend(s, gump3, 2, 0);
			gump3[0]=0;
			for (i=0;i<strlen(script1);i++)
			{
				gump3[1]=script1[i];
				Network->xSend(s, gump3, 2, 0);
			}
		}
	}
	while (script1[0]!='}');
	closescript();
//	sprintf(temp, "Type set to %i", gump1[10]);
//	sysmessage(s, temp);
//	printf("%s\n", temp);
}

void whomenu(int s, int type) //WhoList--By Homey-- Thx Zip and Taur helping me on this
// added also logged out chars+bugfixing, LB
{
	char sect[512];
	short int length, length2, textlines;
	int i,k, j, x;
	int line;
	char menuarray[7*(MAXCLIENT+50)][50]; // there shouldn't be more than 5*MAXCLIENT players, 7 to be on the safe side
	char menuarray1[7*(MAXCLIENT+50)][50];
	
	unsigned int maxArraySize=7*(MAXCLIENT+50);

	unsigned int linecount=0;
	unsigned int linecount1=0,pagenum=1,position=40,linenum=1,buttonnum=7;
	
	j = 0;
	for( k = 0; k < now; k++ )
		if( online( currchar[k] ) ) j++;

	//--static pages
	strcpy(menuarray[linecount++], "nomove");
	strcpy(menuarray[linecount++], "noclose");
	strcpy(menuarray[linecount++], "page 0");
	strcpy(menuarray[linecount++], "resizepic 0 0 5120 320 340");    //The background
	strcpy(menuarray[linecount++], "button 20 300 2130 2129 1 0 1"); //OKAY
	strcpy(menuarray[linecount++], "text 20 10 300 0");           //text <Spaces from Left> <Space from top> <Length, Color?> <# in order>
	
	
	sprintf(menuarray[linecount++], "page %i",pagenum);
	//--Start User List
	k=0;
	for( i = 0; i < charcount; i++ )
	{
		if( (chars[i].account != -1 && chars[i].free == 0 ) )
		{
			if( k > 0 && ( !(k%10)))
			{
				position=40;
				pagenum++;
				sprintf(menuarray[linecount++], "page %i",pagenum);
			}
			k++;
			sprintf(menuarray[linecount++], "text 40 %i 300 %i",position,linenum); //usernames
			sprintf(menuarray[linecount++], "button 20 %i 1209 1210 1 0 %i",position,buttonnum);
			position+=20;
			linenum++;
			if ( linecount+3 >= maxArraySize ) break;
			buttonnum++;
		}
	}
	
	pagenum=1; //lets make some damn buttons
	for (i=0;i<k;i+=10)
	{
		if ( linecount+3 >= maxArraySize ) break;
		sprintf(menuarray[linecount++], "page %i", pagenum);
		if (i>=10)
			sprintf(menuarray[linecount++], "button 150 300 2223 2223  0 %i",pagenum-1); //back button
		if ((k>10) && ((i+10)<k))
			sprintf(menuarray[linecount++], "button %i 300 2224 2224  0 %i", 150+(20*(pagenum>1)),pagenum+1); //forward button
		pagenum++;
	}		
	
			
	length=21;
	length2=1;
	
	for(line=0;line<min(linecount, maxArraySize);line++)
	{
		if (strlen(menuarray[line])==0)
			break;
		{
			length+=strlen(menuarray[line])+4;
			length2+=strlen(menuarray[line])+4;
		}
	}
	
	length+=3;
	textlines=0;
	line=0;
	
	sprintf(menuarray1[linecount1++], "Users currently online: %i",j);
	
	
	//Start user list
	x = 0;
	for( i = 0; i < now; i++ )
	{
		if( online( currchar[i] ) )
		{
			sprintf( menuarray1[linecount1++], "Player %s [online]", chars[currchar[i]].name );
				whomenudata[x++] = chars[currchar[i]].serial;
		}
	}
	for( i = 0; i < charcount; i++ )
	{
		if ( linecount1 >= maxArraySize ) break;
		if( chars[i].account != -1 && chars[i].free == 0 && !online( i ) )
		{
			sprintf( menuarray1[linecount1++], "Player: %s [offline]", chars[i].name );
			whomenudata[x++] = chars[i].serial;
		}
	}
	
	for(line=0;line<max(linecount1, maxArraySize);line++)
	{
		
		if (strlen(menuarray1[line])==0)
			break;
		{
			length+=strlen(menuarray1[line])*2 +2;
			textlines++;
		}
	}
	
	gump1[1]=length>>8;
	gump1[2]=length%256;
	gump1[7]=0;
	gump1[8]=0;
	gump1[9]=0;
	gump1[10]=type; // Gump Number
	gump1[19]=length2>>8;
	gump1[20]=length2%256;
	Network->xSend(s, gump1, 21, 0);
	
	for(line=0;line<linecount;line++)
	{
		sprintf(sect, "{ %s }", menuarray[line]);
		Network->xSend(s, sect, strlen(sect), 0);
	}
	
	gump2[1]=textlines>>8;
	gump2[2]=textlines%256;
	
	Network->xSend(s, gump2, 3, 0);
	
	for(line=0;line<linecount1;line++)
	{
		if (strlen(menuarray1[line])==0)
			break;
		{
			gump3[0]=strlen(menuarray1[line])>>8;
			gump3[1]=strlen(menuarray1[line])%256;
			Network->xSend(s, gump3, 2, 0);
			gump3[0]=0;
			for (i=0;i<strlen(menuarray1[line]);i++)
			{
				gump3[1]=menuarray1[line][i];
				Network->xSend(s, gump3, 2, 0);
			}
		}
	}
}


// Tweak should be completely rewritten.  Using tline and ttext is just plain silly
void tline( int line, int j, char type )
{
	line--; if( line == 0 ) strcpy( script1, "page 0" );
	line--; if( line == 0 ) strcpy( script1, "resizepic 0 0 2520 400 350" );
	line--; if( line == 0 ) strcpy( script1, "text 40 10 32 0" );
	line--; if( line == 0 ) strcpy( script1, "button 35 280 2130 2129 1 0 1" );
	if( type == 1 ) 
	{ 
		line--; 
		if( line == 0 ) 
			sprintf( script1, "tilepic 300 180 %i", (items[j].id1<<8)+items[j].id2); 
	}
	line--; if( line == 0 ) strcpy( script1, "page 1" );
		line--; if( line == 0 ) strcpy( script1, "button 366 320 2224 2224 0 2");
		line--; if( line == 0 ) strcpy( script1, "text 288 317 32 1");
	line--; if( line == 0 ) strcpy( script1, "text 90 70 0 3");
	line--; if( line == 0 ) strcpy( script1, "text 200 70 16 4");
	line--; if( line == 0 ) strcpy( script1, "button 30 70 2116 2115 1 0 2");
	line--; if( line == 0 ) strcpy( script1, "text 90 95 0 5");
	line--; if( line == 0 ) strcpy( script1, "text 200 95 16 6");
	line--; if( line == 0 ) strcpy( script1, "button 30 95 2116 2115 1 0 3");
	line--; if( line == 0 ) strcpy( script1, "text 90 120 0 7");
	line--; if( line == 0 ) strcpy( script1, "text 200 120 16 8");
	line--; if( line == 0 ) strcpy( script1, "button 30 120 2116 2115 1 0 4");
	line--; if( line == 0 ) strcpy( script1, "text 90 145 0 9");
	line--; if( line == 0 ) strcpy( script1, "text 200 145 16 10");
	line--; if( line == 0 ) strcpy( script1, "button 30 145 2116 2115 1 0 5");
	line--; if( line == 0 ) strcpy( script1, "text 90 170 0 11");
	line--; if( line == 0 ) strcpy( script1, "text 200 170 16 12");
	line--; if( line == 0 ) strcpy( script1, "button 30 170 2116 2115 1 0 6");
	line--; if( line == 0 ) strcpy( script1, "text 90 195 0 13");
	line--; if( line == 0 ) strcpy( script1, "text 200 195 16 14");
	line--; if( line == 0 ) strcpy( script1, "button 30 195 2116 2115 1 0 7");
	line--; if( line == 0 ) strcpy( script1, "text 90 220 0 15");
	line--; if( line == 0 ) strcpy( script1, "text 200 220 16 16");
	line--; if( line == 0 ) strcpy( script1, "button 30 220 2116 2115 1 0 8");
	if( type == 2 )
	{
		line--; if( line == 0 ) strcpy( script1, "text 90 245 0 17");
		line--; if( line == 0 ) strcpy( script1, "text 200 245 16 18");
		line--; if( line == 0 ) strcpy( script1, "button 30 245 2116 2115 1 0 9");
	}
	if( type == 1 )
	{
		line--; if( line == 0 ) strcpy( script1, "text 90 245 0 19");
		line--; if( line == 0 ) strcpy( script1, "text 200 245 16 20");
		line--; if( line == 0 ) strcpy( script1, "button 30 245 2116 2115 1 0 10");
	}
	line--; if( line == 0 ) strcpy( script1, "page 2");
	line--; if( line == 0 ) strcpy( script1, "button 40 320 2223 2223 0 1");
	line--; if( line == 0 ) strcpy( script1, "text 65 317 32 2");
	line--; if( line == 0 ) strcpy( script1, "button 366 320 2224 2224 0 3");
	line--; if( line == 0 ) strcpy( script1, "text 288 317 32 1");
	if( type == 1 )
	{
		line--; if( line == 0 ) strcpy( script1, "text 90 70 0 17");
		line--; if( line == 0 ) strcpy( script1, "text 200 70 16 18");
		line--; if( line == 0 ) strcpy( script1, "button 30 70 2116 2115 1 0 9");
		line--; if( line == 0 ) strcpy( script1, "text 90 95 0 21");
		line--; if( line == 0 ) strcpy( script1, "text 200 95 16 22");
		line--; if( line == 0 ) strcpy( script1, "button 30 95 2116 2115 1 0 11");
		line--; if( line == 0 ) strcpy( script1, "text 90 120 0 23");
		line--; if( line == 0 ) strcpy( script1, "text 200 120 16 24");
		line--; if( line == 0 ) strcpy( script1, "button 30 120 2116 2115 1 0 12");
		line--; if( line == 0 ) strcpy( script1, "text 90 145 0 25");
		line--; if( line == 0 ) strcpy( script1, "text 200 145 16 26");
		line--; if( line == 0 ) strcpy( script1, "button 30 145 2116 2115 1 0 13");
		line--; if( line == 0 ) strcpy( script1, "text 90 170 0 27");
		line--; if( line == 0 ) strcpy( script1, "text 200 170 16 28");
		line--; if( line == 0 ) strcpy( script1, "button 30 170 2116 2115 1 0 14");
		line--; if( line == 0 ) strcpy( script1, "text 90 195 0 29");
		line--; if( line == 0 ) strcpy( script1, "text 200 195 16 30");
		line--; if( line == 0 ) strcpy( script1, "button 30 195 2116 2115 1 0 15");
		line--; if( line == 0 ) strcpy( script1, "text 90 220 0 31");
		line--; if( line == 0 ) strcpy( script1, "text 200 220 16 32");
		line--; if( line == 0 ) strcpy( script1, "button 30 220 2116 2115 1 0 16");
		line--; if( line == 0 ) strcpy( script1, "text 90 245 0 33");
		line--; if( line == 0 ) strcpy( script1, "text 200 245 16 34");
		line--; if( line == 0 ) strcpy( script1, "button 30 245 2116 2115 1 0 17");
	}
	if( type == 2 )
	{
		line--; if( line == 0 ) strcpy( script1, "text 90 70 0 19");
		line--; if( line == 0 ) strcpy( script1, "text 200 70 16 20");
		line--; if( line == 0 ) strcpy( script1, "button 30 70 2116 2115 1 0 10");
		line--; if( line == 0 ) strcpy( script1, "text 90 95 0 21");
		line--; if( line == 0 ) strcpy( script1, "text 200 95 16 22");
		line--; if( line == 0 ) strcpy( script1, "button 30 95 2116 2115 1 0 11");
		line--; if( line==0 ) strcpy( script1, "text 90 120 0 23");
		line--; if( line==0 ) strcpy( script1, "text 200 120 16 24");
		line--; if( line==0 ) strcpy( script1, "button 30 120 2116 2115 1 0 12");
		line--; if( line==0 ) strcpy( script1, "text 90 145 0 25");
		line--; if( line==0 ) strcpy( script1, "text 200 145 16 26");
		line--; if( line==0 ) strcpy( script1, "button 30 145 2116 2115 1 0 13");
		line--; if( line==0 ) strcpy( script1, "text 90 170 0 27");
		line--; if( line==0 ) strcpy( script1, "text 200 170 16 28");
		line--; if( line==0 ) strcpy( script1, "button 30 170 2116 2115 1 0 14");
		line--; if( line==0 ) strcpy( script1, "text 90 195 0 29");
		line--; if( line==0 ) strcpy( script1, "text 200 195 16 30");
		line--; if( line==0 ) strcpy( script1, "button 30 195 2116 2115 1 0 15");
	}
	line--; if( line == 0 ) strcpy( script1, "page 3");
	line--; if( line == 0 ) strcpy( script1, "button 40 320 2223 2223 0 2");
	line--; if( line == 0 ) strcpy( script1, "text 65 317 32 2");
	line--;	if( line == 0 ) strcpy( script1, "button 366 320 2224 2224 0 4" );
	line--; if( line == 0 ) strcpy( script1, "text 288 317 32 1" );
	if( type == 1 )
	{
		line--; if( line == 0 ) strcpy( script1, "text 90 70 0 35");
		line--; if( line == 0 ) strcpy( script1, "text 200 70 16 36");
		line--; if( line == 0 ) strcpy( script1, "button 30 70 2116 2115 1 0 18");
		line--; if( line == 0 ) strcpy( script1, "text 90 95 0 37");
		line--; if( line == 0 ) strcpy( script1, "text 200 95 16 38");
		line--; if( line == 0 ) strcpy( script1, "button 30 95 2116 2115 1 0 19");
		line--; if( line == 0 ) strcpy( script1, "text 90 120 0 39");
		line--; if( line == 0 ) strcpy( script1, "text 200 120 16 40");
		line--; if( line == 0 ) strcpy( script1, "button 30 120 2116 2115 1 0 20");
		line--; if( line == 0 ) strcpy( script1, "text 90 145 0 41");
		line--; if( line == 0 ) strcpy( script1, "text 200 145 16 42");
		line--; if( line == 0 ) strcpy( script1, "button 30 145 2116 2115 1 0 21");
		line--; if( line == 0 ) strcpy( script1, "text 90 170 0 43");
		line--; if( line == 0 ) strcpy( script1, "text 200 170 16 44");
		line--; if( line == 0 ) strcpy( script1, "button 30 170 2116 2115 1 0 22");
		line--; if( line == 0 ) strcpy( script1, "text 90 195 0 45");
		line--; if( line == 0 ) strcpy( script1, "text 200 195 16 46");
		line--; if( line == 0 ) strcpy( script1, "button 30 195 2116 2115 1 0 23");
		line--; if( line == 0 ) strcpy( script1, "text 90 220 0 47");
		line--; if( line == 0 ) strcpy( script1, "text 200 220 16 48");
		line--; if( line == 0 ) strcpy( script1, "button 30 220 2116 2115 1 0 24");
		line--; if( line == 0 ) strcpy( script1, "text 90 245 0 49");
		line--; if( line == 0 ) strcpy( script1, "text 200 245 16 50");
		line--; if( line == 0 ) strcpy( script1, "button 30 245 2116 2115 1 0 25");
	}
	line--; if( line == 0 ) strcpy( script1, "page 4");
	line--; if( line == 0 ) strcpy( script1, "button 40 320 2223 2223 0 3");
	line--; if( line == 0 ) strcpy( script1, "text 65 317 32 2");
	if (type==1)
	{
		line--; if( line == 0 ) strcpy( script1, "text 90 70 0 51");
		line--; if( line == 0 ) strcpy( script1, "text 200 70 16 52");
		line--; if( line == 0 ) strcpy( script1, "button 30 70 2116 2115 1 0 26");
		line--; if( line == 0 ) strcpy( script1, "text 90 95 0 53");
		line--; if( line == 0 ) strcpy( script1, "text 200 95 16 54");
		line--; if( line == 0 ) strcpy( script1, "button 30 95 2116 2115 1 0 27");
		line--; if( line == 0 ) strcpy( script1, "text 90 120 0 55");
		line--; if( line == 0 ) strcpy( script1, "text 200 120 16 56");
		line--; if( line == 0 ) strcpy( script1, "button 30 120 2116 2115 1 0 28");
		line--; if( line == 0 ) strcpy( script1, "text 90 145 0 57");
		line--; if( line == 0 ) strcpy( script1, "text 200 145 16 58");
		line--; if( line == 0 ) strcpy( script1, "button 30 145 2116 2115 1 0 29");

		//end addons by Xuri
	}
	line--; if (line==0) strcpy( script1,  "}");
}

void ttext(int line, int j, char type)
{
	if (type==1) { line--; if( line == 0 ) strcpy( script1, "Item Properties"); }
	if (type==2) { line--; if( line == 0 ) strcpy( script1, "Character Properties"); }
	line--; if( line == 0 ) strcpy( script1, "Next page");
	line--; if( line == 0 ) strcpy( script1, "Previous page");
	if (type==1)
	{
		line--; if( line == 0 ) strcpy( script1, "Name");
		line--; if( line == 0 ) strcpy( script1, items[j].name );
		line--; if( line == 0 ) strcpy( script1, "ID");
		line--; if( line == 0 ) sprintf( script1,"0x%x (%i)", (items[j].id1<<8)+items[j].id2, (items[j].id1<<8)+items[j].id2);
		line--; if( line == 0 ) strcpy( script1, "Hue");
		line--; if( line == 0 ) sprintf( script1,"0x%x (%i)", (items[j].color1<<8)+items[j].color2, (items[j].color1<<8)+items[j].color2);
		line--; if( line == 0 ) strcpy( script1, "X");
		line--; if( line == 0 ) sprintf( script1,"%i (0x%x)", items[j].x, items[j].x);
		line--; if( line == 0 ) strcpy( script1, "Y");
		line--; if( line == 0 ) sprintf( script1,"%i (0x%x)", items[j].y, items[j].y);
		line--; if( line == 0 ) strcpy( script1, "Z");
		line--; if( line == 0 ) sprintf( script1,"%i (0x%x)", items[j].z, items[j].z);
		line--; if( line == 0 ) strcpy( script1, "Type");
		line--; if( line == 0 ) sprintf( script1,"%i", items[j].type);
		line--; if( line == 0 ) strcpy( script1,  "ItemHand" );
		line--; if( line == 0 ) sprintf( script1, "%i", items[j].itmhand );
		line--; if( line == 0 ) strcpy( script1, "Layer");
		line--; if( line == 0 ) sprintf( script1,"%i (0x%x)", items[j].layer, items[j].layer);
		line--; if( line == 0 ) strcpy( script1, "Amount");
		line--; if( line == 0 ) sprintf( script1,"%i", items[j].amount);
		line--; if( line == 0 ) strcpy( script1, "More");
		line--; if( line == 0 ) sprintf( script1,"0x%x", (items[j].more1<<24)+(items[j].more2<<16)+(items[j].more3<<8)+items[j].more4);
		line--; if( line == 0 ) strcpy( script1, "MoreB");
		line--; if( line == 0 ) sprintf( script1,"0x%x", (items[j].moreb1<<24)+(items[j].moreb2<<16)+(items[j].moreb3<<8)+items[j].moreb4);
		line--; if( line == 0 ) strcpy( script1, "Stackable");
		line--; if( line == 0 ) sprintf( script1,"%i", items[j].pileable);
		line--; if( line == 0 ) strcpy( script1, "Dyeable");
		line--; if( line == 0 ) sprintf( script1,"%i", items[j].dye );
		line--; if( line == 0 ) strcpy( script1, "Corpse");
		line--; if( line == 0 ) sprintf( script1,"%i", items[j].corpse );
		line--; if( line == 0 ) strcpy( script1, "LoDamage" );
		line--; if( line == 0 ) sprintf( script1,"%i", items[j].lodamage );
		line--; if( line == 0 ) strcpy( script1, "HiDamage" );
		line--; if( line == 0 ) sprintf( script1,"%i", items[j].hidamage );
		line--; if( line == 0 ) strcpy( script1, "Defense");
		line--; if( line == 0 ) sprintf( script1,"%i", items[j].def);
		line--; if( line == 0 ) strcpy( script1, "Magic");
		line--; if( line == 0 ) sprintf( script1,"%i", items[j].magic);
		line--; if( line == 0 ) strcpy( script1, "Visible");
		line--; if( line == 0 ) sprintf( script1,"%i", items[j].visible);
		line--; if( line == 0 ) strcpy( script1, "HitPoints" );
		line--; if( line == 0 ) sprintf( script1, "%i", items[j].hp );
		line--; if( line == 0 ) strcpy( script1, "MaxHitPoints" );
		line--; if( line == 0 ) sprintf( script1, "%i", items[j].maxhp );
		line--; if( line == 0 ) strcpy( script1, "Speed" );
		line--; if( line == 0 ) sprintf( script1, "%i", items[j].spd );
		line--; if( line == 0 ) strcpy( script1, "Rank");
		line--; if( line == 0 ) sprintf( script1,"%i", items[j].rank);
		line--; if( line == 0 ) strcpy( script1, "Value");
		line--; if( line == 0 ) sprintf( script1,"%i", items[j].value);
		line--; if( line == 0 ) strcpy( script1, "Good" );
		line--; if( line == 0 ) sprintf( script1,"%i", items[j].good );
		line--; if( line == 0 ) strcpy( script1, "Made Skill" );
		line--; if( line == 0 ) sprintf( script1, "%i", items[j].madewith );
		line--; if( line == 0 ) strcpy( script1, "Creator" );
		line--; if( line == 0 ) strcpy( script1, items[j].creator );

	}
	if( type == 2 )
	{
		line--; if( line == 0 ) strcpy( script1, "Name" );
		line--; if( line == 0 ) strcpy( script1,  chars[j].name );
		line--; if( line == 0 ) strcpy( script1, "Title" );
		line--; if( line == 0 ) strcpy( script1,  chars[j].title );
		line--; if( line == 0 ) strcpy( script1, "X" );
		line--; if( line == 0 ) sprintf( script1,"%i", chars[j].x );
		line--; if( line == 0 ) strcpy( script1, "Y" );
		line--; if( line == 0 ) sprintf( script1,"%i", chars[j].y );
		line--; if( line == 0 ) strcpy( script1, "Z" );
		line--; if( line == 0 ) sprintf( script1,"%i", chars[j].z );
		line--; if( line == 0 ) strcpy( script1, "Direction" );
		line--; if( line == 0 ) sprintf( script1,"%i", chars[j].dir );
		line--; if( line == 0 ) strcpy( script1, "Body" );
		line--; if( line == 0 ) sprintf( script1,"(0x%x) %i", (chars[j].id1<<8)+chars[j].id2, (chars[j].id1<<8)+chars[j].id2 );
		line--; if( line == 0 ) strcpy( script1, "Skin" );
		line--; if( line == 0 ) sprintf( script1, "(0x%x) %i", (chars[j].skin1<<8)+chars[j].skin2, (chars[j].skin1<<8)+chars[j].skin2 );
		line--; if( line == 0 ) strcpy( script1, "Defence" );
		line--; if( line == 0 ) sprintf( script1,"%i", chars[j].def );
		line--; if( line == 0 ) strcpy( script1, "Race" );
		line--; if( line == 0 ) sprintf( script1,"%i", chars[j].race );
		line--; if( line == 0 ) strcpy( script1, "Hunger" );
		line--; if( line == 0 ) sprintf( script1,"%i", chars[j].hunger );
		line--; if( line == 0 ) strcpy( script1, "Strength" );
		line--; if( line == 0 ) sprintf( script1,"%i", chars[j].st );
		line--; if( line == 0 ) strcpy( script1, "Dexterity" );
		line--; if( line == 0 ) sprintf( script1,"%i", chars[j].dx );
		line--; if( line == 0 ) strcpy( script1, "Intelligence" );
		line--; if( line == 0 ) sprintf( script1,"%i", chars[j].in );
	}
	line--; if( line == 0) strcpy( script1, "}");
}

void tweakmenu(int s, int j, int type)
{
	char sect[512];
	short int length, length2, textlines;
	int i;
	int line;
	
	length=21;
	length2=1;
	line=0;
	do
	{
		line++;
		tline(line, j, type);
		if (script1[0]!='}')
		{
			length+=strlen(script1)+4;
			length2+=strlen(script1)+4;
		}
	}
	while (script1[0]!='}');
	length+=3;
	textlines=0;
	line=0;
	do
	{
		line++;
		ttext(line, j, type);
		if (script1[0]!='}')
		{
			length+=(strlen(script1)*2)+2;
			textlines++;
		}
	}
	while (script1[0]!='}');
	gump1[1]=length>>8;
	gump1[2]=length%256;
	if (type==1)
	{
		gump1[3]=items[j].ser1;
		gump1[4]=items[j].ser2;
		gump1[5]=items[j].ser3;
		gump1[6]=items[j].ser4;
	}
	if (type==2)
	{
		gump1[3]=chars[j].ser1;
		gump1[4]=chars[j].ser2;
		gump1[5]=chars[j].ser3;
		gump1[6]=chars[j].ser4;
	}
	gump1[7]=0;
	gump1[8]=0;
	gump1[9]=0;
	gump1[10]=type; // Gump Number
	gump1[19]=length2>>8;
	gump1[20]=length2%256;
	Network->xSend(s, gump1, 21, 0);
	line=0;
	do
	{
		line++;
		tline(line, j, type);
		if (script1[0]!='}')
		{
			sprintf(sect, "{ %s }", script1);
			Network->xSend(s, sect, strlen(sect), 0);
		}
	}
	while (script1[0]!='}');
	gump2[1]=textlines>>8;
	gump2[2]=textlines%256;
	Network->xSend(s, gump2, 3, 0);
	line=0;
	do
	{
		line++;
		ttext(line, j, type);
		if (script1[0]!='}')
		{
			gump3[0]=strlen(script1)>>8;
			gump3[1]=strlen(script1)%256;
			Network->xSend(s, gump3, 2, 0);
			gump3[0]=0;
			for (i=0;i<strlen(script1);i++)
			{
				gump3[1]=script1[i];
				Network->xSend(s, gump3, 2, 0);
			}
		}
	}
	while (script1[0]!='}');
}

void entrygump(UOXSOCKET s, SERIAL serial, char type, char index, short int maxlength, char *text1)
{
	short int length;
	unsigned char textentry1[12]="\xAB\x01\x02\x01\x02\x03\x04\x00\x01\x12\x34";
	char textentry2[9]="\x01\x01\x00\x00\x12\x34\x12\x34";
	unsigned char tser1, tser2, tser3, tser4;
	splitSerial(serial, tser1, tser2, tser3, tser4);
	
	sprintf(temp, "(%i chars max)", maxlength);
	length=11+strlen(text1)+1+8+strlen(temp)+1;
	textentry1[1]=length>>8;
	textentry1[2]=length%256;
	textentry1[3]=tser1;
	textentry1[4]=tser2;
	textentry1[5]=tser3;
	textentry1[6]=tser4;
	textentry1[7]=type;
	textentry1[8]=index;
	textentry1[9]=(strlen(text1)+1)>>8;
	textentry1[10]=(strlen(text1)+1)%256;
	Network->xSend(s, textentry1, 11, 0);
	Network->xSend(s, text1, strlen(text1)+1, 0);
	textentry2[4]=maxlength>>8;
	textentry2[5]=maxlength%256;
	textentry2[6]=(strlen(temp)+1)>>8;
	textentry2[7]=(strlen(temp)+1)%256;
	Network->xSend(s, textentry2, 8, 0);
	Network->xSend(s, temp, strlen(temp)+1, 0);
}


void choice(int s) // Choice from GMMenu, Itemmenu or Makemenu received
{ // This routine is changed by Magius(CHE) to add Rank_system!
	// What the hell is a rank system?
	int main, sub;
	char sect[512];
	int i;
	char lscomm[512], lsnum[512]; // Magius(CHE) for Rank-System
	Script *script;
	
	main=(buffer[s][5]<<8)+buffer[s][6];
	sub=(buffer[s][7]<<8)+buffer[s][8];
	
    //printf("main:%i sub:%i \n",main,sub);
	// if ((main!=0) && (sub==0)) printf("add menu (gm menu) closed- including its submenus\n");

	if ((main>=8000)&&(main<=8100)) Guilds->GumpChoice(s,main,sub);
	
	if( (main&0xFF00)==0xFF00)
	{
		if (im_choice(s, main, sub)==0) return;
	}

	if (main>1246 && main<1255)
	{
		Skills->DoPotion(s, main-1246, sub, calcItemFromSer(addid1[s], addid2[s], addid3[s], addid4[s]));
		return;
	}
	else if (main<ITEMMENUOFFSET) // GM Menus
	{
		openscript("menus.scp");
		sprintf(sect, "GMMENU %i", main);
		script = i_scripts[menus_script];
	}
	/*else if ((main>=5256) && (main<8192)) // Tracking fix 12-30-98
	{
		openscript("items.scp");
		sprintf(sect, "ITEMMENU %i", main-256);
		script = i_scripts[items_script];
	}*/
	else if ((main>=ITEMMENUOFFSET && main<MAKEMENUOFFSET) || (main>=5256) && (main<8192))
	{
		openscript("items.scp");
		//openscript("addmenus.scp");
		//if ( main-ITEMMENUOFFSET < nameMenus.start )
			sprintf(sect, "ITEMMENU %i", main-ITEMMENUOFFSET);
		/*else
			sprintf(sect, "ITEMMENU %s", nameMenus.menu[main-ITEMMENUOFFSET-nameMenus.start]);*/

		script = i_scripts[items_script];//addmenu_script];
	}
	else if(main>=MAKEMENUOFFSET && main<TRACKINGMENUOFFSET)
	{
		openscript("create.scp");
		sprintf(sect, "MAKEMENU %i", main-MAKEMENUOFFSET);
		script = i_scripts[create_script];
	} // PolyMorph spell menu (scriptable) by AntiChrist (9/99)
	else if ( main >= POLYMORPHMENUOFFSET && main < POLYMORPHMENUOFFSET+50 )
	{
		Magic->Polymorph( s, POLYMORPHMENUOFFSET, sub );
		return;
	}
	else // Tracking
	{
		if((main-TRACKINGMENUOFFSET)>=TRACKINGMENUOFFSET+1&&(main-TRACKINGMENUOFFSET)<=TRACKINGMENUOFFSET+3)
		{
			if(!sub) return;
			if(!Skills->CheckSkill(currchar[s],TRACKING, 0, 1000))
			{
				sysmessage(s,"You fail your attempt at tracking.");
				return;
			}
			Skills->TrackingMenu(s,sub-1);
		}
		openscript("tracking.scp");
		sprintf(sect, "TRACKINGMENU %i", main-TRACKINGMENUOFFSET);
		script = i_scripts[tracking_script];
	}
	
	if (!script->find(sect))
	{
		closescript();
		if (n_scripts[custom_item_script][0]!=0)
		{
			openscript(n_scripts[custom_item_script]);
			if (!i_scripts[custom_item_script]->find(sect))
			{
				closescript();
				if (n_scripts[custom_npc_script][0]!=0)
				{
					openscript(n_scripts[custom_npc_script]);
					if (!i_scripts[custom_npc_script]->find(sect))
					{
						closescript();
						return;
					}
				}
			}
		} else return;
	}
	read1();
	i=0;
	read2(); // Moved by Magius(CHE) for Rank System
	do
	{
		if (script1[0]!='}')
		{
			i++;
			if (main>=MAKEMENUOFFSET && main<TRACKINGMENUOFFSET)
			{ // Start Here Changes by Magius(CHE)
				read2();
				itemmake[s].needs=str2num(script2);
				read2();
				itemmake[s].minskill=str2num(script2);
				itemmake[s].maxskill=itemmake[s].minskill*server_data.skilllevel;
			}
			read2();
			strcpy( lscomm, script1 );
			strcpy( lsnum, script2 );
			itemmake[s].number = str2num( script2 );
			read2(); // Rank System - Magius(CHE)
			if( !(strcmp(script1, "RANK" ) ) )
			{
				gettokennum( script2, 0 );
				itemmake[s].minrank = str2num( gettokenstr );
				gettokennum( script2, 1 );
				itemmake[s].maxrank = str2num( gettokenstr );
				read2();
			} 
			else 
			{ // Set maximum rank if the item is not ranked!
				itemmake[s].minrank = 10;
				itemmake[s].maxrank = 10;
			}
			if( server_data.rank_system == 0 )
			{
				itemmake[s].minrank = 10;
				itemmake[s].maxrank = 10;
			}

			//   printf("%s %i\n",script1,str2num(script2));
			if (i==sub)
			{
				closescript();
				scriptcommand( s, lscomm, lsnum );
				if (itemmake[s].maxskill<200) itemmake[s].maxskill=200;
			} // End Here Changes by Magius(CHE)
		}
	}
	while ((script1[0]!='}')&&(i!=sub));
	if (i!=sub) closescript();
}

void gmmenu(int s, int m) // Open one of the gray GM Call menus
{
	int total, i;
	int lentext;
	char sect[512];
	char gmtext[11][257];
	int gmnumber=0;
	int gmindex;
	
	openscript("menus.scp");
	sprintf(sect, "GMMENU %i", m);
	if (!i_scripts[menus_script]->find(sect))
	{
		closescript();
		return;
	}
	gmindex=m;
	read1();
	lentext=sprintf(gmtext[0], "%s", script1);
	do
	{
		read1();
		if (script1[0]!='}')
		{
			gmnumber++;
			strcpy(gmtext[gmnumber], script1);
			read1();
		}
	}
	while (script1[0]!='}');
	closescript();
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
	gmprefix[7]=gmindex>>8;
	gmprefix[8]=gmindex%256;
	Network->xSend(s, gmprefix, 9, 0);
	Network->xSend(s, &lentext, 1, 0);
	Network->xSend(s, gmtext[0], lentext, 0);
	lentext=gmnumber;
	Network->xSend(s, &lentext, 1, 0);
	for (i=1;i<=gmnumber;i++)
	{
		gmmiddle[0]=(i-1)>>8;
		gmmiddle[1]=(i-1)%256;
		Network->xSend(s, gmmiddle, 4, 0);
		lentext=strlen(gmtext[i]);
		Network->xSend(s, &lentext, 1, 0);
		Network->xSend(s, gmtext[i], lentext, 0);
	}
}


void itemmenu(int s, int m) // Menus for item creation
{
	int total, i;
	int lentext;
	char sect[512];
	char gmtext[30][257];
	int gmid[30];
	int gmnumber=0;
	int gmindex;
	
	//openscript("addmenus.scp");
	openscript("items.scp");
	sprintf(sect, "ITEMMENU %i", m);
	if (!i_scripts[items_script]->find(sect))
	{
		closescript();
		if (n_scripts[custom_item_script][0]!=0)
		{
			openscript(n_scripts[custom_item_script]);
			if (!i_scripts[custom_item_script]->find(sect))
			{
				closescript();
				return;
			}
		} else return;
	}
	gmindex=m;
	// printf("gmindex: %i\n", gmindex );

	if( !(chars[currchar[s]].priv&0x01) && m < 990 && m > 999 )
	{
		sysmessage( s, "Access denied" );
		return;
	}

	////////////////////////////////////////////////
	read1();
	strcpy( gmtext[0], script1 );
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
	gmprefix[7]=(gmindex+ITEMMENUOFFSET)>>8;
	gmprefix[8]=(gmindex+ITEMMENUOFFSET)%256;
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

void cGump::Open(int s, int i, int num1, int num2)
{
	char shopgumpopen[8]="\x24\x00\x00\x00\x01\x00\x30";
	shopgumpopen[1]=chars[i].ser1;
	shopgumpopen[2]=chars[i].ser2;
	shopgumpopen[3]=chars[i].ser3;
	shopgumpopen[4]=chars[i].ser4;
	shopgumpopen[5]=num1;
	shopgumpopen[6]=num2;
	Network->xSend(s, shopgumpopen, 7, 0);
}

void SendVecsAsGump( UOXSOCKET sock, stringList& one, stringList& two, unsigned char type )
// PRE:		Sock exists, one and two exists, type is valid num
// POST:	Sends to socket sock the data in one and two.  One is control, two is data
// CODER:	Abaddon
{
	unsigned int linecount = 0, linecount1 = 0;
	unsigned char i = 0;
	char sect[512];
	int length, length2;

	length = 21;
	length2 = 1;

	linecount = one.size();
	linecount1 = two.size();
	int line, textlines;
	for( line = 0; line < linecount; line++ )
	{
		if( one[line]->length() == 0 )
			break;
		length += one[line]->length() + 4;
		length2 += one[line]->length() + 4;
	}
	
	length += 3;
	textlines = 0;
	line = 0;

	for(line = 0; line < linecount1; line++ )
	{
		if( two[line]->length() == 0 )
			break;
		length += two[line]->length() * 2 + 2;
		textlines++;
	}
	
	gump1[1] = length>>8;
	gump1[2] = length%256;
	gump1[7] = 0;
	gump1[8] = 0;
	gump1[9] = 0;
	gump1[10] = type; // Gump Number
	gump1[19] = length2>>8;
	gump1[20] = length2%256;
	Network->xSend( sock, gump1, 21, 0);
	
	for( line = 0; line < linecount; line++ )
	{
		sprintf(sect, "{ %s }", one[line]->c_str() );
		Network->xSend( sock, sect, strlen( sect ), 0 );
	}
	
	gump2[1] = textlines>>8;
	gump2[2] = textlines%256;
	
	Network->xSend( sock, gump2, 3, 0);

	for( line = 0; line < linecount1; line++ )
	{
		if( two[line]->length() == 0 )
			break;
		gump3[0] = ( two[line]->length() )>>8;
		gump3[1] = ( two[line]->length() )%256;
		Network->xSend( sock, gump3, 2, 0);
		gump3[0]=0;
		for ( i = 0; i < two[line]->length(); i++ )
		{
			gump3[1] = (*two[line])[i];
			Network->xSend( sock, gump3, 2, 0);
		}
	}

}
