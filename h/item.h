//------------------------------------------------------------------------
//  
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
// In addition to that license, if you are running this program or modified  *
// versions of it on a public system you HAVE TO make the complete source of *
// the version used by you available or provide people with a location to    *
// download it.                                                              *


#ifndef __ITEM_H
#define	__ITEM_H

//	System Includes
#include <iostream>

//	Namespace

using namespace std;	

//	Third Party Libraries



//	Forward Class Declaration
class cItem;

//	UOX3 includes
#include <defines.h>
#include <basetype.h>
#include <structs.h>

//++++++++++++++++++ Class Definition ++++++++++++++++++++++++++++++++++++++++++++++++++++
class cItem
{
public:
	void GlowItem( UOXSOCKET s, int i );
	int MemItemFree();
	void DeleItem(int i);
	int CreateScriptItem(int s, int itemnum, int nSpawned);
	int CreateRandomItem(char *sItemList);
	int CreateScriptRandomItem(int s, char *sItemList);
	int SpawnItem(UOXSOCKET nSocket, CHARACTER ch, int nAmount, char* cName, int nStackable,
        unsigned char cItemId1, unsigned char cItemId2, unsigned char cColorId1, unsigned char cColorId2, int nPack, int nSend);
	int SpawnItem(UOXSOCKET nSocket, int nAmount, char* cName, int nStackable, unsigned char cItemId1, unsigned char cItemId2, unsigned char cColorId1, unsigned char cColorId2, int nPack, int nSend);
	int SpawnItemBackpack2(UOXSOCKET s, CHARACTER ch, int nItem, int nDigging);
	void GetScriptItemSetting( int c ); // by Magius (CHE)
	void DecayItem(unsigned int currenttime, int i);
	void Decay(unsigned int currenttime);
	void RespawnItem(unsigned int Currenttime, int i);
	void AddRespawnItem(int s, int x, int y);
	void AddRandomItem(int s, char *itemlist, int spawnpoint);
	void InitItem(int nItem, char ser=1);
	char isFieldSpellItem(int i);
	bool isShieldType( int i );
	bool isLeftHandType( int i );
	ARMORCLASS ArmorClass( ITEM i );
	unsigned char PackType( unsigned char id1, unsigned char id2 );
	void CheckEquipment( CHARACTER p );  // AntiChrist
	void BounceInBackpack( CHARACTER p, ITEM i ); // AntiChrist
	void BounceItemOnGround( CHARACTER p, ITEM i ); // AntiChrist
};

#endif
