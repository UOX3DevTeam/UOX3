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


#ifndef __SKILLS_H
#define	__SKILLS_H

//	System Includes
#include <iostream>

//	Namespace

using namespace std;	

//	Third Party Libraries



//	Forward Class Declaration
class cSkills;

//	UOX3 includes
#include <defines.h>
#include <basetype.h>
#include <structs.h>

//++++++++++++++++++ Class Definition ++++++++++++++++++++++++++++++++++++++++++++++++++++
class cSkills
{
public:
	void Tracking( int s, int selection );
	void CreatePotion(int s, char type, char sub, int mortar);
	void Fish(unsigned int i);
	int GetIngotAmt(int s, unsigned char id1, unsigned char id2, unsigned char color1, unsigned char color2);
	char AdvanceSkill(int s, int sk, char skillused);
	void AdvanceStats(int s, int sk);
	void TinkerAxel(int s);
	void TinkerAwg(int s);
	void TinkerClock(int s);
	void MakeDough(int s);
	void MakePizza(int s);
	void Track(int i);
	void DoPotion(int s, int type, int sub, int mortar);
	void Tailoring( UOXSOCKET s );
	void Fletching(int s);
	void BowCraft(int s);
	void Carpentry(int s);
	void Smith( UOXSOCKET s );
	void Repair( UOXSOCKET s );
	void MakeMenuTarget(int s, int x, int skill);
	void MakeMenu(int s, int m, int skill);
	void Mine(int s);
	void GraveDig(int s);
	void SmeltOre( UOXSOCKET s );
	void Wheel(int s, int mat);
	void Loom(int s);
	void CookMeat(int s);
	void TreeTarget(int s);
	void DetectHidden(int s);
	void ProvocationTarget1( UOXSOCKET s );
	void ProvocationTarget2( UOXSOCKET s );
	void EnticementTarget1( UOXSOCKET s );
	void EnticementTarget2( UOXSOCKET s );
	void AlchemyTarget(int s);
	void BottleTarget(int s);
	void PotionToBottle(int s, int mortar);
	char CheckSkill(int s, int sk, int low, int high);
	void CreateBandageTarget(int s);
	void HealingSkillTarget(int s);
	void SpiritSpeak(int s);
	void ArmsLoreTarget(int s);
	void ItemIdTarget(int s);
	void Evaluate_int_Target(int s);
	void AnatomyTarget(int s);
	void TameTarget(int s);
	void FishTarget(int s);
	int GetCombatSkill(int i);
	int GetShield(int i);
	ITEM GetSecondHand( CHARACTER i );
	void SkillUse(int s, int x);
	void StealingTarget(int s);
	void CreateTrackingMenu(int s, int m);
	void TrackingMenu(int s, int gmindex);
	void BeggingTarget(int s);
	void AnimalLoreTarget(int s);
	void ForensicsTarget(int s);
	void PoisoningTarget( UOXSOCKET s );
	int Inscribe( UOXSOCKET s, long snum);
	int EngraveAction(int s, int i, int cir, int spl);
	void updateSkillLevel(int c, int s);
	void LockPick( UOXSOCKET s );
	void TDummy(int s);
	void NewDummy(unsigned int currenttime);
	void Tinkering(int s);
	void AButte(int s1, int x);
	void Persecute( UOXSOCKET s ); // AntiChrist persecute stuff
	void Snooping( UOXSOCKET s, CHARACTER target, SERIAL serial );
	
private:
	int GetSubIngotAmt(int p, char id1, char id2, char color1, char color2);
	void DeleIngot(int s, int id1, int id2, int color1, int color2, int amount);
	int DeleSubIngot(int p, int id1, int id2, int color1, int color2, int amount);
	void AnvilTarget( int s, item_st& item, char *ingotName, int makemenu = 1 );
	void Atrophy( CHARACTER c, unsigned short sk );
	void Hide(int s);
	void Stealth(int s);
	void PeaceMaking(int s);
	void PlayInstrumentWell( UOXSOCKET s, int i);
	void PlayInstrumentPoor( UOXSOCKET s, int i);
	int GetInstrument(int s);
	void RandomSteal(int s);
	int TrackingDirection(int s, int i);
	void TellScroll(char *menu_name, int player, long item_param);
	void CollectAmmo(int s, int a, int b);
	void Meditation( UOXSOCKET s );
	int CalcRank( int s, int skill ); // by Magius(CHE)
	void ApplyRank( int s, int c, int rank ); // by Magius(CHE)
	void Zero_Itemmake( int s ); // by Magius(CHE)


};

#endif
