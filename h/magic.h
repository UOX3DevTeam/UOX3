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


#ifndef __MAGIC_H
#define	__MAGIC_H

//	System Includes
#include <iostream>

//	Namespace

using namespace std;	

//	Third Party Libraries



//	Forward Class Declaration
class cMagic;

//	UOX3 includes
#include <defines.h>
#include <basetype.h>
#include <structs.h>

//++++++++++++++++++ Class Definition ++++++++++++++++++++++++++++++++++++++++++++++++++++
class cMagic
{
public:
	//:Terrin: adding constructor/destructor
	cMagic();
	~cMagic(); // no need for a virtual destructor as long as no subclasses us

	void AddSpell( int book, int spellNum );
	unsigned char HasSpell( int book, int spellNum );

	void LoadScript( void ); //:Terrin: adding function for spell system "cache"
	void SpellBook( UOXSOCKET s );
	char GateCollision(int s);
	void GateDestruction(unsigned int currenttime);
	bool newSelectSpell2Cast( UOXSOCKET s, int num );
	bool requireTarget( unsigned char num );
	bool reqItemTarget( int num );
	bool reqLocTarget( int num );
	bool reqCharTarget( int num );
	bool spellReflectable( int num );
	bool travelSpell( int num );
	stat_st getStatEffects( int num );
	move_st getMoveEffects( int num );
	sound_st getSoundEffects( int num );
	bool aggressiveSpell( int num );
	bool fieldSpell( int num );
	void doMoveEffect( int num, int target, int source );
	void doStaticEffect( int source, int num );
	void playSound( int source, int num );
	void NewDelReagents( CHARACTER s, reag_st reags );
	void NewCastSpell( UOXSOCKET s );
	void SbOpenContainer( UOXSOCKET s );
	char CheckResist( CHARACTER attacker, CHARACTER defender, int circle );	// umm, why?
	void PoisonDamage(int p, int posion);
	void CheckFieldEffects2(unsigned int currenttime, int c, char timecheck);
	void PFireballTarget(int i, int k, int j);
	void NPCFireballTarget(int nAttacker, int nDefender);
	void NPCLightningTarget(int nAttacker, int nDefender);
	void NPCCurseTarget(int nAttacker, int nDefender);
	void NPCWeakenTarget(int s, int t);
	void NPCFeebleMindTarget(int s, int t);
	void NPCCLumsyTarget(int s, int t);
	void NPCMindBlastTarget(int s, int t);
	void NPCMagicArrowTarget(int s, int t);
	void NPCClumsyTarget(int s, int t);
	void NPCHarmTarget(int s, int t);
	void NPCHarmtarget(int s, int t);
	void NPCParalyzeTarget(int s, int t);
	void NPCEBoltTarget(int s, int t);
	void NPCExplosionTarget(int s, int t);
	void NPCDispel( CHARACTER s, CHARACTER i );
	char CheckParry(int player, int circle);
	void NPCFlameStrikeTarget(int s, int t);
	void MagicArrowSpellItem(int attacker, int defender);
	void ClumsySpellItem(int attacker, int defender);
	void FeebleMindSpellItem(int attacker, int defender);
	void WeakenSpellItem(int attacker, int defender);
	void HarmSpellItem(int attacker, int defender);
	void FireballSpellItem(int attacker, int defender);
	void CurseSpellItem(int attacker, int defender);
	void LightningSpellItem(int attacker, int defender);
	void MindBlastSpellItem(int attacker, int defender);
	void ParalyzeSpellItem(int attacker, int defender);
	void ExplosionSpellItem(int attacker, int defender);
	void FlameStrikeSpellItem(int attacker, int defender);
	int CheckBook(int circle, int spell, int i);
	char CheckReagents( int s, reag_st reagents );
	char CheckMana( int s, int num );
	bool CheckStamina( CHARACTER s, int num );
	bool CheckHealth( CHARACTER s, int num );
//	void DeleReagents(int s, int ash, int drake, int garlic, int ginseng, int moss, int pearl, int shade, int silk);
	void Recall( UOXSOCKET s); // we need these four for the commands (separately) !!!
	void Mark( UOXSOCKET s);
	void Gate( UOXSOCKET s);
	void Heal( UOXSOCKET s);
	char CheckMagicReflect(int i);
	void MagicDamage(int p, int amount, CHARACTER attacker = -1 );
	void SpellFail( UOXSOCKET s );
	char SubtractMana(int s, int mana);
	char SubtractStamina( CHARACTER s, int stamina );
	char SubtractHealth( CHARACTER s, int health, int spellNum );
	void MagicTrap(int s, int i);	// moved here by AntiChrist (9/99)
	void Polymorph( int s, int gmindex, int creaturenumer); // added by AntiChrist (9/99)


private:
	void SummonMonster( UOXSOCKET s, unsigned char d1, unsigned char id2, char *monstername, unsigned char color1, unsigned char color2, int x, int y, int z);
	void MagicReflect(int s);
	void BoxSpell( UOXSOCKET s, int& x1, int& x2, int& y1, int& y2, int& z1, int& z2);
	int RegMsg( CHARACTER s, reag_st failmsg );
	void DirectDamage(int p, int amount);
	void PolymorphMenu( int s, int gmindex ); // added by AntiChrist (9/99)

};

#endif
