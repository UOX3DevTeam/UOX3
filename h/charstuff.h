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


#ifndef __CHARSTUFF_H
#define	__CHARSTUFF_H

//	System Includes
#include <iostream>

//	Namespace

using namespace std;	

//	Third Party Libraries



//	Forward Class Declaration
class cCharStuff;

//	UOX3 includes
#include <defines.h>
#include <basetype.h>
#include <structs.h>

//++++++++++++++++++ Class Definition ++++++++++++++++++++++++++++++++++++++++++++++++++++
class cCharStuff
{
public:
	void DeleteChar(int k);
	int MemCharFree();
	int AddRandomLoot(int s, char * lootlist);
	int AddRandomNPC(int s, char *npclist, int spawnpoint);
	int AddRespawnNPC(int s, int region, int npcNum, int type);
	int AddNPCxyz(int s, int npcNum, int type, int x1, int y1, signed char z1);
	int Split(int k);
	void CheckAI(unsigned int currenttime, int i);
	void InitChar(int nChar, char ser=1);
	int FindItem( CHARACTER toFind, unsigned char type );
	int FindItem( CHARACTER toFind, unsigned char id1, unsigned char id2 );
	
private:
	void FindSpotForNPC(int c, int originX, int originY, int xAway, int yAway, int elev);
	int  SearchSubPackForItem( ITEM toSearch, unsigned char type );
	int  SearchSubPackForItem( ITEM toSearch, unsigned char id1, unsigned char id2 );


};

#endif
