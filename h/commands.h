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


#ifndef __COMMANDS_H
#define	__COMMANDS_H

//	System Includes
#include <iostream>

//	Namespace

using namespace std;	

//	Third Party Libraries



//	Forward Class Declaration
class cCommands;

//	UOX3 includes
#include <defines.h>
#include <basetype.h>
#include <structs.h>

//++++++++++++++++++ Class Definition ++++++++++++++++++++++++++++++++++++++++++++++++++++
class cCommands
{
public:
	void NextCall(int s, int type);
	void RepairWorld(int s);
	void KillSpawn(int s, int r);
	void RegSpawnMax(int s, int r);
	void RegSpawnNum(int s, int r, int n);
	void KillAll(int s, int percent, unsigned char * sysmsg);
	void AddHere(int s, char z);
	void ShowGMQue(int s, int type);
	void Wipe(int s);
	void CPage(int s, char * reason);
	void GMPage(int s, char * reason);
	void MakePlace(int s, int i);
	void Command( UOXSOCKET s );
	void MakeShop(int c);
	void RemoveShop(int s);
	void DyeItem(int s);
	void SetItemTrigger(int s);
	void SetTriggerType(int s);
	void SetTriggerWord(int s);
	void SetNPCTrigger(int s);
	void DupeItem(int s, int i, int amount);
	void Possess(int s);
	void Load( void );
	signed int FindIndex( char *toFind );
	int cmd_offset;
};




#endif
