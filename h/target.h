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


#ifndef __TARGETS_H
#define	__TARGETS_H

//	System Includes
#include <iostream>

//	Namespace

using namespace std;	

//	Third Party Libraries



//	Forward Class Declaration
class cTargets;

//	UOX3 includes
#include <defines.h>
#include <basetype.h>
#include <structs.h>

//++++++++++++++++++ Class Definition ++++++++++++++++++++++++++++++++++++++++++++++++++++
class cTargets
{
private:
	void CommandLevelTarget( UOXSOCKET s ) ;
	void GuardTarget( UOXSOCKET s );
	void GlowTarget( UOXSOCKET s );
	void UnglowTarget( UOXSOCKET s );
	void PlVBuy(int s);
	void RenameTarget(int s);
	void AddTarget(int s);
	void TeleTarget( UOXSOCKET s );
	void RemoveTarget(int s);
	void DyeTarget(int s);
	void NewzTarget(int s);
	void TypeTarget(int s);
	void XgoTarget(int s);
	void MoreXYZTarget(int s);
	void MoreXTarget(int s);
	void MoreYTarget(int s);
	void MoreZTarget(int s);
	void PrivTarget(int s);
	void MoreTarget(int s);
	void KeyTarget(int s);
	void IstatsTarget(int s);
	void WstatsTarget(int s);
	void GMTarget(int s);
	void CnsTarget(int s);
	void KillTarget(int s, int ly);
	void FontTarget(int s);
	void GhostTarget(int s);
	void AmountTarget(int s);
	void SetAmount2Target(int s);
	void CloseTarget(int s);
	void VisibleTarget( UOXSOCKET s );
	void OwnerTarget(int s);
	void ColorsTarget(int s);
	void DvatTarget(int s);
	void AddNpcTarget(int s);
	void FreezeTarget(int s);
	void UnfreezeTarget(int s);
	void AllSetTarget(int s);
	void InfoTarget(int s);
	void LoadCannon(int s);
	void SetInvulFlag(int s);
	void Tiling(int s);
	void ExpPotionTarget(int s);
	void SquelchTarg(int s);
	void TeleStuff(int s);
	void SwordTarget(int s);
	void CorpseTarget(int s);
	void CarveTarget( UOXSOCKET s, int feat, int ribs, int hides, int fur, int wool);
	void newCarveTarget( UOXSOCKET s, ITEM i );
	void TitleTarget(int s);
	void NpcTarget( UOXSOCKET s );
	void NpcTarget2( UOXSOCKET s );
	void NpcRectTarget( UOXSOCKET s );
	void NpcCircleTarget( UOXSOCKET s );
	void NpcWanderTarget( UOXSOCKET s );
	void NpcAITarget(int s);
	void xBankTarget(int s);
	void xSpecialBankTarget( int s ); // AntiChrist
	void DupeTarget(int s);
	void MoveToBagTarget(int s);
	void SellStuffTarget(int s);
	void GmOpenTarget(int s);
	void StaminaTarget(int s);
	void ManaTarget(int s);
	void MakeShopTarget(int s);
	void AttackTarget( UOXSOCKET s );
	void FollowTarget(int s);
	void TransferTarget(int s);
	void BuyShopTarget(int s);
	void SetValueTarget(int s);
	void SetRestockTarget(int s);
	void permHideTarget(int s);
	void unHideTarget(int s);
	void SetWipeTarget(int s);
	void SetSpeechTarget(int s);
	void SetSpAttackTarget(int s);
	void SetPoisonTarget(int s);
	void SetPoisonedTarget(int s);
	void FullStatsTarget(int s);
	void SetAdvObjTarget(int s);
	void CanTrainTarget(int s);
	void SetSplitTarget(int s);
	void SetSplitChanceTarget(int s);
	void AxeTarget(int s);
	void ObjPrivTarget(int s);
	void SetSpaDelayTarget(int s);
	void NewXTarget(int s);
	void NewYTarget(int s);
	void IncXTarget(int s);
	void IncYTarget(int s);
	void IncZTarget( UOXSOCKET s );
	void BoltTarget(int s);
	void MovableTarget(int s);
	void SetDirTarget(int s);
	void HouseOwnerTarget(int s);
	void HouseEjectTarget(int s);
	void HouseBanTarget(int s);
	void HouseFriendTarget(int s);
	void HouseUnlistTarget(int s);
	void BanTarg(int s);
	void triggertarget( int ts ); // By Magius(CHE)
	void ShowSkillTarget( int s );
	void ResurrectionTarget( UOXSOCKET s );
	void SetMurderCount( int s );	// Abaddon 12 Sept 1999
	void HouseLockdown( UOXSOCKET s ); // Abaddon 17th December, 1999
	void HouseRelease( UOXSOCKET s ); // Abaddon 17th December 1999
	void ShowDetail( UOXSOCKET s ); // Abaddon 11th January, 2000
public:
	void CstatsTarget( UOXSOCKET s );
	void TweakTarget( UOXSOCKET s );
	void GetAccount( int s );
	void IDtarget(int s);
	void MultiTarget(int s);
	void Wiping(int s);
	int NpcMenuTarget(int s);
	void NpcResurrectTarget(int s);
	void JailTarget(int s, int c);
	void ReleaseTarget(int s, int c);
	int AddMenuTarget(int s, int x, int addmitem);
	void XTeleport(int s, int x);
	int BuyShop(int s,int c);
};


#endif
