//------------------------------------------------------------------------
//  necro.cpp
//
//------------------------------------------------------------------------
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 1998 - 2001 by Unknown real name (Genesis)
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
Module : necro.cpp
Purpose: store all necromancy related functions
Created: Genesis 11-12-1998
History: None
*/

#include "..\h\uox3.h"
#include "..\h\debug.h"

#define DBGFILE "necro.cpp"
void VialTargetItem( UOXSOCKET nSocket, ITEM nItemID );
void VialTargetChar( UOXSOCKET nSocket, CHARACTER trgChar );


int SpawnRandomMonster( UOXSOCKET nCharID, char* cScript, char* cList, char* cNpcID)
{
	/*This function gets the random monster number from the from
	the script and list specified.
	Npcs->AddRespawnNPC passing the new number*/

	char sect[512];
	int i=0,item[256]={0};
 	openscript(cScript);
 	sprintf(sect, "%s %s", cList, cNpcID);
	if(!(strcmp("necro.scp",cScript)))
	{
		if(!i_scripts[necro_script]->find(sect))
		{
  			closescript();
  			return -1;
 		}
	}
	else
	{
		if(!i_scripts[npc_script]->find(sect))
 		{
  			closescript();
			if (n_scripts[custom_npc_script][0]!=0)
			{
				openscript(n_scripts[custom_npc_script]);
				if (!i_scripts[custom_npc_script]->find(sect))
				{
					closescript();
					return -1;
				}
			} else return -1;
		}
	}
 	do
 	{
  		read1();
		if(script1[0]!='}')
		{
			item[i]=str2num(script1);
			i++;
		}
 	}
 	while(script1[0]!='}');
 	closescript();
 	if(i>0)
 	{
  		i=rand()%(i);
		if(item[i]!=-1)
		{
			Npcs->AddRespawnNPC(nCharID,-1,item[i],0);
			return item[i];
		}
	}
	return -1;
}

int SpawnRandomItem(UOXSOCKET nCharID,int nInPack, char* cScript, char* cList, char* cItemID)
{
 	/*This function gets the random item number from the list and recalls
 	  SpawnItemBackpack2 passing the new number*/
	char sect[512];
	int i=0,item[256]={0};
 	openscript(cScript);
	sprintf(sect, "%s %s", cList, cItemID);
	if(!(strcmp("necro.scp",cScript)))
	{
		if(!i_scripts[necro_script]->find(sect))
		{
			closescript();
			return -1;
		}
	}
	else
	{
		if(!i_scripts[items_script]->find(sect))
		{
			closescript();
			if (n_scripts[custom_item_script][0]!=0)
			{
				openscript(n_scripts[custom_item_script]);
				if (!i_scripts[custom_item_script]->find(sect))
				{
					closescript();
					return -1;
				}
			} else return -1;
		}
	}
 	do
 	{
  		read1();
  		if (script1[0]!='}')
  		{
			item[i]=str2num(script1);
   			i++;
		}
	}
	while(script1[0]!='}');
	closescript();
 	if(i>0)
 	{
  		i=rand()%(i);
		if(item[i]!=-1)
		if(nInPack)
		{
			Items->SpawnItemBackpack2(nCharID,currchar[nCharID],item[i],1);
			return item[i];
		}
	}
	return -1;
}

void MakeNecroReg( UOXSOCKET nSocket, ITEM nItem, unsigned char cItemID1, unsigned char cItemID2 )
{
	int iCharID, iItem = 0;
	iCharID = currchar[nSocket];

	if((cItemID1==0x1b)&&((cItemID2>=0x11)&&(cItemID2<=0x1c))) // Make bone powder.
	{
		sprintf( temp, "%s is grinding some bone into powder.", chars[iCharID].name );
		npcemoteall( iCharID, temp, 1);
		tempeffect( iCharID, iCharID, 9, 0, 0, 0 );
		tempeffect( iCharID, iCharID, 9, 0, 3, 0 );
		tempeffect( iCharID, iCharID, 9, 0, 6, 0 );
		tempeffect( iCharID, iCharID, 9, 0, 9, 0 );
		iItem = Items->SpawnItem( nSocket, 1, "bone powder", 1, 0x0F, 0x8F, 0, 0, 1, 1 );
		if( iItem == -1 ) 
			return;
		items[iItem].morex = 666;
		items[iItem].more1 = 1; // this will fill more with info to tell difference between ash and bone
		Items->DeleItem( nItem );
		
	}
	if( cItemID1 == 0x0E && cItemID2 == 0x24 ) // Make vial of blood.
	{
		if( items[nItem].more1 == 1 )
		{
			iItem = Items->SpawnItem( nSocket, iCharID, 1, "#", 1, 0x0F, 0x82, 0, 0, 1, 1 );
			if( iItem == -1 ) 
				return;
			items[iItem].value = 15;
			items[iItem].morex = 666;
		}
		else
		{
			iItem = Items->SpawnItem( nSocket,iCharID, 1, "#", 1, 0x0F, 0x7D, 0, 0, 1, 1 );
			if( iItem == -1 ) 
				return;
			items[iItem].value = 10;
			items[iItem].morex = 666;
		}
		if( items[nItem].amount > 1 )
		{
			items[nItem].amount--;
			if( items[nItem].amount == 1 )
				RefreshItem( nItem );
		}
		else
			Items->DeleItem( nItem );
	}
}

void vialtarget( UOXSOCKET nSocket )
{
	if( buffer[nSocket][7] == 0xFF && buffer[nSocket][8] == 0xFF && buffer[nSocket][9] == 0xFF && buffer[nSocket][10] == 0xFF ) 
		return; // check if user canceled operation
	if( buffer[nSocket][7] >= 0x40 )
	{	// it's an item
		ITEM myItem = calcItemFromSer( buffer[nSocket][7], buffer[nSocket][8], buffer[nSocket][9], buffer[nSocket][10] );
		VialTargetItem( nSocket, myItem );
	}
	else
	{	// it's a char
		CHARACTER myChar = calcCharFromSer( buffer[nSocket][7], buffer[nSocket][8], buffer[nSocket][9], buffer[nSocket][10] );
		VialTargetChar( nSocket, myChar );
	}
}

void VialTargetChar( UOXSOCKET nSocket, CHARACTER trgChar ) // bug & crashfixed by LB 25 September 1999
{
	int nVialID = -1, nDagger = -1, nCharID = -1, nDist = 0;
	CHARACTER mChar = currchar[nSocket];
	if( mChar == -1 )
		return;
	nVialID = addx[nSocket];
	if( nVialID == -1 )
		return;
	nCharID = trgChar;
	nDagger = Combat->GetWeapon( mChar );
	if( nDagger == -1 ) 
	{
		sysmessage( nSocket, "You do not have a dagger in your hands" );
		return;
	}
	if( !( items[nDagger].id1 == 0x0F && (items[nDagger].id2 == 0x51 || items[nDagger].id2 == 0x52) ) )
	{
		sysmessage( nSocket, "That is not a dagger" );
		return;
	}

	items[nVialID].more1=0;

	if( nCharID == mChar )
	{
		if( chars[nCharID].hp <= 10 )
		{
			sysmessage( nSocket, "You are too wounded to continue." );
			return;
		}
		else
		{
			sysmessage( nSocket, "You prick your finger and fill the vial." );
			chars[nCharID].hp -= ( rand()%6 + 2 );
			MakeNecroReg( nSocket, nVialID, 0x0E, 0x24 );
			return;
		}
	}
	else
	{
		nDist = chardist( mChar, nCharID );
		if( inrange1p( mChar, nCharID ) && nDist <= 2 )
		{
			sprintf( temp, "%s has pricked you with a dagger and sampled your blood.", chars[mChar].name);
			if( chars[nCharID].npc )
			{
				Karma( mChar, nCharID, -chars[nCharID].karma );
				if( chars[nCharID].id1 == 0x00 && ( chars[nCharID].id2 == 0x0C || ( chars[nCharID].id2 >= 0x3B && chars[nCharID].id2 <= 0x3D ) ) )
					items[nVialID].more1 = 1;
				chars[nCharID].hp -= ( rand()%6 + 2 );
				MakeNecroReg( nSocket, nVialID, 0x0E, 0x24 );
				// Guard be summuned if in town and good npc
				// if good flag criminal
				// if evil npc attack necromancer but don't flag criminal
			}
			else
			{
				Karma( mChar, nCharID, -chars[nCharID].karma );
				chars[nCharID].hp -= ( rand()%6 + 2 );
				sysmessage( calcSocketFromChar( nCharID ), temp );
				MakeNecroReg( nSocket, nVialID, 0x0E, 0x24 );
				// flag criminal						
			}
		}
		else 
		{
			sysmessage( nSocket, "That individual is not anywhere near you." );
			return;
		}
	}
}
void VialTargetItem( UOXSOCKET nSocket, ITEM nItemID ) // bug & crashfixed by LB 25 September 1999
{
	int nVialID = -1, nDagger = -1;
	CHARACTER mChar = currchar[nSocket];
	if( mChar == -1 || nItemID == -1 )
		return;
	nVialID = addx[nSocket];
	if( nVialID == -1 )
		return;

	nDagger = Combat->GetWeapon( mChar );

	if( nDagger == -1 )
	{
		sysmessage( nSocket, "You do not have a dagger in your pack." );
		return;
	}
	if( !( items[nDagger].id1 == 0x0F && (items[nDagger].id2 == 0x51 || items[nDagger].id2 == 0x52) ) )
	{
		sysmessage( nSocket, "That is not a dagger" );
		return;
	}

	items[nVialID].more1 = 0;
	if( !items[nItemID].corpse )
	{
		sysmessage( nSocket, "That is not a corpse!" );
	}
	else
	{
		items[nVialID].more1 = items[nItemID].more1;
		Karma( mChar, -1, -1000 );
		if( items[nItemID].more2 < 4 )
		{
			sysmessage( nSocket, "You take a sample of blood from the corpse." );
			MakeNecroReg( nSocket, nVialID, 0x0E, 0x24 );
			items[nItemID].more2++;
		}
		else 
			sysmessage( nSocket, "You examine the corpse but, decide any further blood samples would be too contaminated." );
	}
}
