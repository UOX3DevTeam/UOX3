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


#ifndef __COMBAT_H
#define	__COMBAT_H

//	System Includes
#include <iostream>

//	Namespace

using namespace std;	

//	Third Party Libraries



//	Forward Class Declaration
class cCombat;

//	UOX3 includes
#include <defines.h>
#include <basetype.h>
#include <structs.h>

//++++++++++++++++++ Class Definition ++++++++++++++++++++++++++++++++++++++++++++++++++++
class cCombat
{
public:
	int GetSwingRate( int iNPC, int weapon );
	int GetArrowType( int i );
	int GetBowType(int i);
	int GetWeapon(int i);
	int CalcAtt(int p);
	int CalcDef(int p, int x);
	void CombatOnHorse(int i);
	void CombatOnFoot(int i);
	void CombatHit(int a, int d, unsigned int currenttime, signed int arrowType = -1 );
	void DoCombat(int a, unsigned int currenttime);
	void SpawnGuard( CHARACTER s, CHARACTER i, int x, int y, signed char z);
	bool weapon2Handed( int weapon );

private:
	void ItemCastSpell(int s, int c, int i);
	int TimerOk(int c);
	void ItemSpell(int attacker, int defender);
	void doSoundEffect( CHARACTER p, int fightskill, ITEM weapon ); // AntiChrist
	void doMissedSoundEffect( CHARACTER p ); // AntiChrist


};


#endif
