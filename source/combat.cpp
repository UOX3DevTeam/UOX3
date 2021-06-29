#include "uox3.h"
#include "cGuild.h"
#include "combat.h"
#include "townregion.h"
#include "cRaces.h"
#include "skills.h"
#include "cMagic.h"
#include "cEffects.h"
#include "CPacketSend.h"
#include "classes.h"
#include "regions.h"
#include "magic.h"
#include "Dictionary.h"
#include "movement.h"
#include "cScript.h"
#include "CJSMapping.h"
#include <algorithm>

#undef DBGFILE
#define DBGFILE "combat.cpp"
#define DEBUG_COMBAT		0

CHandleCombat *Combat = nullptr;

#define SWINGAT (UI32)1.75 * 1000

const UI08 BODYPERCENT = 0;
//const UI08 ARMSPERCENT = 1;
//const UI08 HEADPERCENT = 2;
//const UI08 LEGSPERCENT = 3;
//const UI08 NECKPERCENT = 4;
//const UI08 OTHERPERCENT = 5;
const UI08 TOTALTARGETSPOTS = 6; // Wonder if that should be one?

const UI08 LOCPERCENTAGES[TOTALTARGETSPOTS] = { 44, 14, 14, 14, 7, 7 };

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool StartAttack( CChar *cAttack, CChar *cTarget )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle characters initiating combat with other characters
//o-----------------------------------------------------------------------------------------------o
bool CHandleCombat::StartAttack( CChar *cAttack, CChar *cTarget )
{
	if( !ValidateObject( cAttack ) || !ValidateObject( cTarget ) || cAttack == cTarget )
		return false;
	if( cAttack->IsDead() || cTarget->IsDead() )
		return false;
	if( cAttack->GetTarg() == cTarget )
		return false;
	if( cTarget->GetVisible() > 2 )
		return false;
	if( !objInRange( cAttack, cTarget, DIST_NEXTTILE ) && !LineOfSight( nullptr, cAttack, cTarget->GetX(), cTarget->GetY(), ( cTarget->GetZ() + 15 ), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false ) )
		return false;

	if( !cAttack->GetCanAttack() || cAttack->IsEvading() ) // Is the char allowed to attack?
		return false;
	if( cAttack->GetNPCAiType() == AI_DUMMY ) // If passive, don't allow attack
		return false;
	if( cAttack->IsNpc() && cTarget->IsNpc() && cAttack->GetRace() == cTarget->GetRace() ) // Don't allow combat between NPCs of the same race
		return false;

	// Check if combat is allowed in attacker AND target's regions, but allow it if it's a guard
	if( cAttack->GetNPCAiType() != AI_GUARD && ( cAttack->GetRegion()->IsSafeZone() || cTarget->GetRegion()->IsSafeZone() ))
	{
		// Either attacker or target is in a safe zone where all aggressive actions are forbidden, disallow
		CSocket *cSocket = cAttack->GetSocket();
		if( cSocket )
			cSocket->sysmessage( 1799 );
		return false;
	}

	bool returningAttack = false;

	cAttack->SetTarg( cTarget );
	cAttack->SetAttacker( cTarget );
	cAttack->SetAttackFirst( ( cTarget->GetTarg() != cAttack ) );
	if( !cTarget->IsInvulnerable() && (!ValidateObject( cTarget->GetTarg() ) || !objInRange( cTarget, cTarget->GetTarg(), DIST_INRANGE )) )	// Only invuln don't fight back
	{
		if( cTarget->GetNPCAiType() != AI_DUMMY )
		{
			cTarget->SetTarg( cAttack );
			cTarget->SetAttacker( cAttack );
			cTarget->SetAttackFirst( false );
			returningAttack = true;
		}

		if( cTarget->GetSocket() != nullptr )
		{
			CPAttackOK tSend = (*cAttack);
			cTarget->GetSocket()->Send( &tSend );
		}
	}

	if( WillResultInCriminal( cAttack, cTarget ) ) //REPSYS
	{
		criminal( cAttack );
		bool regionGuarded = ( cTarget->GetRegion()->IsGuarded() );
		if( cwmWorldState->ServerData()->GuardsStatus() && regionGuarded && cTarget->IsNpc() && cTarget->GetNPCAiType() != AI_GUARD && cwmWorldState->creatures[cTarget->GetID()].IsHuman() )
		{
			cTarget->TextMessage( nullptr, 335, TALK, true );
			callGuards( cTarget, cAttack );
		}
	}

	if( cAttack->GetVisible() == VT_TEMPHIDDEN || cAttack->GetVisible() == VT_INVISIBLE )
		cAttack->ExposeToView();

	if( !cAttack->IsNpc() )
		cAttack->BreakConcentration( cAttack->GetSocket() );
	else
	{
		UI16 toPlay = cwmWorldState->creatures[cAttack->GetID()].GetSound( SND_STARTATTACK );
		if( toPlay != 0x00 )
			Effects->PlaySound( cAttack, toPlay );

		// if the source is an npc, make sure they're in war mode and reset their movement time
		if( !cAttack->IsAtWar() )
			cAttack->ToggleCombat();
		if( cAttack->GetMounted() )
			cAttack->SetTimer( tNPC_MOVETIME, BuildTimeValue( cAttack->GetMountedWalkingSpeed() ) );
		else
			cAttack->SetTimer( tNPC_MOVETIME, BuildTimeValue( cAttack->GetWalkingSpeed() ) );
	}

	// Only unhide the defender, if they're going to return the attack (otherwise they're doing nothing!)
	if( returningAttack )
	{
		if( cTarget->GetVisible() == VT_TEMPHIDDEN || cAttack->GetVisible() == VT_INVISIBLE )
			cTarget->ExposeToView();

		if( !cTarget->IsNpc() )
			cTarget->BreakConcentration( cTarget->GetSocket() );

		// if the target is an npc, and not a guard, make sure they're in war mode and update their movement time
		// ONLY IF THEY'VE CHANGED ATTACKER
		if( cTarget->IsNpc() && cTarget->GetNPCAiType() != AI_GUARD )
		{
			if( !cTarget->IsAtWar() )
				cTarget->ToggleCombat();
			if( cTarget->GetMounted() )
				cTarget->SetTimer( tNPC_MOVETIME, BuildTimeValue( cTarget->GetMountedWalkingSpeed() ) );
			else
				cTarget->SetTimer( tNPC_MOVETIME, BuildTimeValue( cTarget->GetWalkingSpeed() ) );
		}
	}
	return true;
}

void callGuards( CChar *mChar, CChar *targChar );
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void PlayerAttack( CSocket *s )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle player attacking (Double-clicking whilst in war mode)
//o-----------------------------------------------------------------------------------------------o
void CHandleCombat::PlayerAttack( CSocket *s )
{
	if( s == nullptr )
		return;

	CChar *ourChar = s->CurrcharObj();
	if( !ValidateObject( ourChar ) )
		return;

	// Don't allow attacking other characters if invulnerable
	if( ourChar->IsInvulnerable() )
	{
		s->sysmessage( 1973 ); // You cannot engage in combat while invulnerable!
		return;
	}

	SERIAL serial = s->GetDWord( 1 );
	if( serial == INVALIDSERIAL )
	{
		ourChar->SetTarg( nullptr );
		return;
	}
	CChar *i = calcCharObjFromSer( serial );
	if( !ValidateObject( i ) )
	{
		ourChar->SetTarg( nullptr );
		return;
	}
	if( ourChar->IsDead() )
	{
		if( i->IsNpc() )
		{
			switch( i->GetNPCAiType() )
			{
				case AI_HEALER_G:	// Good Healer
					if( !ourChar->IsCriminal() && !ourChar->IsMurderer() )
					{ // Character is innocent
						if( objInRange( i, ourChar, DIST_NEARBY ) )
						{
							CMultiObj *multiObj = ourChar->GetMultiObj();
							if( !ValidateObject( multiObj ) || multiObj->GetOwner() == ourChar->GetSerial() )
							{
								if( LineOfSight( s, ourChar, i->GetX(), i->GetY(), ( i->GetZ() + 15 ), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false ) )
								{
									//let's resurrect him!
									Effects->PlayCharacterAnimation( i, 0x10 );
									NpcResurrectTarget( ourChar );
									Effects->PlayStaticAnimation( ourChar, 0x376A, 0x09, 0x06 );
									i->TextMessage( nullptr, ( 316 + RandomNum( 0, 4 ) ), TALK, false );
								}
							}
						}
						else
							i->TextMessage( nullptr, 321, TALK, true );
					}
					else // Character is criminal or murderer
						i->TextMessage( nullptr, 322, TALK, true );
					break;
				case AI_HEALER_E: // Evil Healer
					if( ourChar->IsMurderer() )
					{
						if( objInRange( i, ourChar, DIST_NEARBY ) )	// let's resurrect him
						{
							CMultiObj *multiObj = ourChar->GetMultiObj();
							if( !ValidateObject( multiObj ) || multiObj->GetOwner() == ourChar->GetSerial() )
							{
								if( LineOfSight( s, ourChar, i->GetX(), i->GetY(), ( i->GetZ() + 15 ), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false ) )
								{
									if( i->GetBodyType() == BT_GARGOYLE
										|| ( cwmWorldState->ServerData()->ForceNewAnimationPacket() 
											&& ( i->GetBodyType() == BT_HUMAN || i->GetBodyType() == BT_ELF )))
										Effects->PlayNewCharacterAnimation( i, N_ACT_SPELL, S_ACT_SPELL_TARGET ); // Action 0x0b, subAction 0x00
									else
										Effects->PlayCharacterAnimation( i, ACT_SPELL_TARGET ); // 0x10
									NpcResurrectTarget( ourChar );
									Effects->PlayStaticAnimation( ourChar, 0x3709, 0x09, 0x19 ); //Flamestrike effect
									i->TextMessage( nullptr, ( 323 + RandomNum( 0, 4 ) ), TALK, false );
								}
							}
						}
						else
							i->TextMessage( nullptr, 328, TALK, true );
					}
					else
						i->TextMessage( nullptr, 329, TALK, true );
					break;
				default:
					s->sysmessage( 330 );
					break;
			}
			return;
		} //if isNpc
		else
		{
			if( cwmWorldState->ServerData()->PlayerPersecutionStatus() )
			{
				ourChar->SetTarg( i );
				Skills->Persecute( s );
				return;
			}
			else
			{
				s->sysmessage( 330 );
				return;
			}
		}//if isNpc
	}//if isDead
	else if( ourChar->GetTarg() != i )
	{
		std::vector<UI16> scriptTriggers = ourChar->GetScriptTriggers();
		for( auto scriptTrig : scriptTriggers )
		{
			cScript *toExecute = JSMapping->GetScript( scriptTrig );
			if( toExecute != nullptr )
			{
				// -1 == script doesn't exist, or returned -1
				// 0 == script returned false, 0, or nothing - don't execute hard code
				// 1 == script returned true or 1
				if( toExecute->OnCombatStart( ourChar, i ) == 0 )
				{
					return;
				}
			}
		}

		if( !ourChar->GetCanAttack() ) //Is our char allowed to attack
		{
			s->sysmessage( 1778 );
			return;
		}

		// Check if combat is allowed in attacker AND target's regions
		if( ourChar->GetRegion()->IsSafeZone() || i->GetRegion()->IsSafeZone() )
		{
			// Either attacker or target is in a safe zone where all aggressive actions are forbidden, disallow
			s->sysmessage( 1799 );
			return;
		}

		ourChar->SetTarg( i );
		if( ourChar->GetVisible() == VT_TEMPHIDDEN || ourChar->GetVisible() == VT_INVISIBLE )
			ourChar->ExposeToView();
		if( i->IsDead() || i->GetHP() <= 0 )
		{
			s->sysmessage( 331 );
			return;
		}
		if( i->GetNPCAiType() == AI_PLAYERVENDOR ) // PlayerVendors
		{
			s->sysmessage( 332, i->GetName().c_str() );
			return;
		}
		if( i->IsInvulnerable() )
		{
			s->sysmessage( 333 );
			return;
		}
		if( i->IsEvading() )
		{
			s->sysmessage( 1792 );
			return;
		}

		// Deal with players attacking NPCs they're escorting!
		if( i->IsNpc() && i->GetQuestType() == 0xFF && i->GetOwnerObj() == ourChar )
		{
			i->SetNpcWander( WT_FREE );   // Wander freely
			i->SetFTarg( nullptr );			 // Reset follow target
			i->SetQuestType( 0 );         // Reset quest type
			i->SetQuestDestRegion( 0 );   // Reset quest destination region
			// Set a timer to automatically delete the NPC
			i->SetTimer( tNPC_SUMMONTIME, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_ESCORTDONE ) );
			i->SetOwner( nullptr );
			i->TextMessage( nullptr, 1797, TALK, false ); // Send out the rant about being abandoned
		}

		//flag them FIRST so that anything attacking them as a result of this is not flagged.
		if( WillResultInCriminal( ourChar, i ) ) //REPSYS
		{
			criminal( ourChar );
			bool regionGuarded = ( i->GetRegion()->IsGuarded() );
			if( cwmWorldState->ServerData()->GuardsStatus() && regionGuarded && i->IsNpc() && i->GetNPCAiType() != AI_GUARD && cwmWorldState->creatures[i->GetID()].IsHuman() )
			{
				i->TextMessage( nullptr, 335, TALK, true );
				callGuards( i, ourChar );
			}
		}

		if( i->IsGuarded() )
			petGuardAttack( ourChar, i, i );

		ourChar->TextMessage( nullptr, 334, EMOTE, 1, ourChar->GetName().c_str(), i->GetName().c_str() );	// Attacker emotes "You see attacker attacking target" to all nearby

		if( !i->IsNpc() )
		{
			CSocket *iSock = i->GetSocket();
			if( iSock != nullptr )
				i->TextMessage( iSock, 1281, EMOTE, 1, ourChar->GetName().c_str() ); // "Attacker is attacking you!" sent to target socket only
		}

		// keep the target highlighted
		CPAttackOK toSend = (*i);
		s->Send( &toSend );

		if( i->GetNPCAiType() != AI_GUARD && !ValidateObject( i->GetTarg() ) )
		{
			i->SetAttacker( ourChar );
			i->SetAttackFirst( false );
		}
		ourChar->SetAttackFirst( true );
		ourChar->SetAttacker( i );
		AttackTarget( i, ourChar );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void AttackTarget( CChar *cAttack, CChar *cTarget )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle the attacking action in combat
//o-----------------------------------------------------------------------------------------------o
void CHandleCombat::AttackTarget( CChar *cAttack, CChar *cTarget )
{
	// Check if OnCombatStart event exists, necessary here to make event run for NPCs attacking
	std::vector<UI16> scriptTriggers = cAttack->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			// -1 == script doesn't exist, or returned -1
			// 0 == script returned false, 0, or nothing - don't execute hard code
			// 1 == script returned true or 1
			if( toExecute->OnCombatStart( cAttack, cTarget ) == 0 )
			{
				return;
			}
		}
	}

	if( !cAttack->GetCanAttack() || cAttack->IsEvading() ) // Is the char allowed to attack?
		return;

	if( !StartAttack( cAttack, cTarget ) )
		return;

	// If the target is an innocent, not a racial or guild ally/enemy, then flag them as criminal
	// and, of course, call the guards ;>
	if( WillResultInCriminal( cAttack, cTarget ) )
	{
		criminal( cAttack );
		bool regionGuarded = ( cTarget->GetRegion()->IsGuarded() );
		if( cwmWorldState->ServerData()->GuardsStatus() && regionGuarded )
		{
			if( cTarget->IsNpc() && cTarget->GetNPCAiType() != AI_GUARD && cwmWorldState->creatures[cTarget->GetID()].IsHuman() )
			{
				cTarget->TextMessage( nullptr, 1282, TALK, true );
				callGuards( cTarget, cAttack );
			}
		}
	}
	if( cAttack->DidAttackFirst() )
	{
		cAttack->TextMessage( nullptr, 334, EMOTE, 1, cAttack->GetName().c_str(), cTarget->GetName().c_str() );  // NPC should emote "Source is attacking Target" to all nearby
		if( !cTarget->IsNpc() )
		{
			CSocket *iSock = cTarget->GetSocket();
			if( iSock != nullptr )
				cTarget->TextMessage( iSock, 1281, EMOTE, 1, cAttack->GetName().c_str() );	// Target should get an emote only to his socket "Target is attacking you!"
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CItem * getWeapon( CChar *i )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Find what weapon (if any) character is holding
//o-----------------------------------------------------------------------------------------------o
CItem * CHandleCombat::getWeapon( CChar *i )
{
	if( !ValidateObject( i ) )
		return nullptr;

	CItem *j = i->GetItemAtLayer( IL_RIGHTHAND );
	if( ValidateObject( j ) )
	{
		if( j->GetType() == IT_SPELLBOOK )	// spell books aren't weapons
			return nullptr;
		return j;
	}
	j = i->GetItemAtLayer( IL_LEFTHAND );
	if( ValidateObject( j ) && !j->IsShieldType() )
		return j;
	return nullptr;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CItem * getShield( CChar *i )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Check players hands for a shield
//o-----------------------------------------------------------------------------------------------o
CItem * CHandleCombat::getShield( CChar *i )
{
	if( ValidateObject( i ) )
	{
		CItem *shield = i->GetItemAtLayer( IL_LEFTHAND );
		if( ValidateObject( shield ) && shield->IsShieldType() )
			return shield;
	}
	return nullptr;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 getWeaponType( CItem *i )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Find what weapon type a character is holding (based on its ID)
//o-----------------------------------------------------------------------------------------------o
UI08 CHandleCombat::getWeaponType( CItem *i )
{
	if( !ValidateObject( i ) )
		return WRESTLING;

	switch( i->GetID() )
	{
			// Slashing Swords
		case 0x0EC4: //skinning knife
		case 0x0EC5: //skinning knife
		case 0x0F60: //longsword
		case 0x0F61: //longsword
		case 0x13B7: //long sword
		case 0x13B8: //long sword
		case 0x1440: //cutlass
		case 0x1441: //cutlass
		case 0x26BB: //bone harvester - AoS
		case 0x26C5: //bone harvester - AoS
		case 0x2573: //magic sword - LBR
		case 0x2574: //magic sword - LBR
		case 0x2575: //magic sword - LBR
		case 0x2576: //magic sword - LBR
		case 0x257E: //short sword - LBR
		case 0x27A4: //wakizashi - SE
		case 0x27EF: //wakizashi - SE
		case 0x0907: //gargish shortblade - SA
		case 0x4076: //gargish shortblade - SA
		case 0x48C6: //gargish bone harvester - SA
		case 0x48C7: //gargish bone harvester - SA
			return DEF_SWORDS;
			// Default Swords
		case 0x13F6: //butcher knife
		case 0x13F7: //butcher knife
		case 0x13FE: //katana
		case 0x13FF: //katana
		case 0x13B5: //scimitar
		case 0x13B6: //scimitar
		case 0x13B9: //viking sword
		case 0x13BA: //viking sword
		case 0x255E: //ratman sword - LBR
		case 0x2560: //skeleton scimitar - LBR
		case 0x090C: //gargish glass sword - SA
		case 0x4073: //gargish glass sword - SA
		case 0x0900: //gargish stone war sword - SA
		case 0x4071: //gargish stone war sword - SA
		case 0x48B6: //gargish butcherknife - SA
		case 0x48B7: //gargish butcherknife - SA
		case 0x48BA: //gargish katana - SA
		case 0x48BB: //gargish katana - SA
		case 0xA28B: //bladed whip - ToL
		case 0xA293: //bladed whip - ToL
			return SLASH_SWORDS;
			// One-Handed Lg. Swords
		case 0x0F5E: //broadsword
		case 0x0F5F: //broadsword
		case 0x257D: //longsword - LBR
		case 0x26CE: //paladin sword - AoS
		case 0x26CF: //paladin sword - AoS
		case 0x2554: //daemon sword - LBR
		case 0x2D27: //radiant scimitar - ML
		case 0x2D33: //radiant scimitar - ML
		case 0x2D29: //elven machete - ML
		case 0x2D35: //elven machete - ML
		case 0x090B: //gargish dread sword - SA
		case 0x4074: //gargish dread sword - SA
		case 0xA33B: //antique sword 1 south
		case 0xA33C: //antique sword 1 east
		case 0xA33D: //antique sword 2 south
		case 0xA33E: //antique sword 2 east
		case 0xA33F: //antique sword 3 south
		case 0xA340: //antique sword 3 east
		case 0xA341: //skull sword south
		case 0xA342: //skull sword east
		case 0xA345: //gargoyle skull sword south
		case 0xA346: //gargoyle skull sword east
			return ONEHND_LG_SWORDS;
			// Two-Handed Lg. Swords
		case 0x143E: //halberd
		case 0x143F: //halberd
		case 0x26BD: //bladed staff - AoS
		case 0x26C7: //bladed staff - AoS
		case 0x26C1: //crescent blade - AoS
		case 0x26CB: //crescent blade - AoS
		case 0x2578: //no-dachi - LBR
		case 0x27A2: //no-dachi - SE
		case 0x27ED: //no-dachi - SE
		case 0x27A8: //bokuto - SE
		case 0x27F3: //bokuto - SE
		case 0x2D26: //rune blade - ML
		case 0x2D32: //rune blade - ML
		case 0x0908: //gargish talwar - SA
		case 0x4075: //gargish talwar - SA
			return TWOHND_LG_SWORDS;
			// Bardiche
		case 0x0F4D: //bardiche
		case 0x0F4E: //bardiche
		case 0x26BA: //scythe - AoS
		case 0x26C4: //scythe - AoS
		case 0x255B: //ophidian bardiche - LBR
		case 0x2577: //naginata - LBR
		case 0x48B4: //gargish bardiche - SA
		case 0x48B5: //gargish bardiche - SA
		case 0x48C4: //gargish scythe - SA
		case 0x48C5: //gargish scythe - SA
			return BARDICHE;
			// One-Handed Axes
		case 0x0EC2: //cleaver
		case 0x0EC3: //cleaver
		case 0x0E85: //pickaxe
		case 0x0E86: //pickaxe
		case 0x2567: //orc lord battleaxe - LBR
		case 0x2579: //pick - LBR
		case 0x48AE: //gargish cleaver
		case 0x48AF: //gargish cleaver
			return ONEHND_AXES;
			// Two-Handed Axes
		case 0x13FA: //large battle axe
		case 0x13FB: //large battle axe
		case 0x1442: //two handed axe
		case 0x1443: //two handed axe
		case 0x0F43: //hatchet
		case 0x0F44: //hatchet
		case 0x0F45: //executioner's axe
		case 0x0F46: //executioner's axe
		case 0x0F47: //battle axe
		case 0x0F48: //battle axe
		case 0x0F49: //axe
		case 0x0F4A: //axe
		case 0x0F4B: //double axe
		case 0x0F4C: //double axe
		case 0x255F: //skeleton axe - LBR
		case 0x255D: //ratman axe - LBR
		case 0x2564: //troll axe - LBR
		case 0x2570: //hatchet - LBR
		case 0x2D28: //ornate axe - ML
		case 0x2D34: //ornate axe - ML
		case 0x48B0: //gargish battleaxe - SA
		case 0x48B1: //gargish battleaxe - SA
		case 0x48B2: //gargish axe - SA
		case 0x48B3: //gargish axe - SA
			return TWOHND_AXES;
			// Default Maces
		case 0x13E3: //smith's hammer
		case 0x13E4: //smith's hammer
		case 0x13B3: //club
		case 0x13B4: //club
		case 0x0FB4: //sledge hammer
		case 0x0FB5: //sledge hammer
		case 0x0F5C: //mace
		case 0x0F5D: //mace
		case 0x1406: //war mace
		case 0x1407: //war mace
		case 0x143C: //hammer pick
		case 0x143D: //hammer pick
		case 0x13AF: //war axe
		case 0x13B0: //war axe
		case 0x143A: //maul
		case 0x143B: //maul
		case 0x26BC: //scepter - AoS
		case 0x26C6: //scepter - AoS
		case 0x2557: //lizardman's mace - LBR
		case 0x255C: //orc club - LBR
		case 0x256F: //smyth's hammer - LBR
		case 0x257F: //war mace - LBR
		case 0x2D24: //diamond mace - ML
		case 0x2D30: //diamond mace - ML
		case 0x2D25: //wild staff - ML
		case 0x2D31: //wild staff - ML
		case 0x0903: //gargish disc mace - SA
		case 0x406E: //gargish disc mace - SA
		case 0x48C2: //gargish maul - SA
		case 0x48C3: //gargish maul - SA
		case 0xA289: //barbed whip - ToL
		case 0xA291: //barbed whip - ToL
			return DEF_MACES;
			// Large Maces
		case 0x13F4: //crook
		case 0x13F5: //crook
		case 0x13F8: //gnarled staff
		case 0x13F9: //gnarled staff
		case 0x0DF0: //black staff
		case 0x0DF1: //black staff
		case 0x0E81: //shepherd's crook
		case 0x0E82: //shepherd's crook
		case 0x0E89: //quarter staff
		case 0x0E8A: //quarter staff
		case 0x1438: //war hammer
		case 0x1439: //war hammer
		case 0x2555: //ettin hammer - LBR
		case 0x2556: //liche's staff - LBR
		case 0x2559: //ogre's club - LBR
		case 0x255A: //ophidian staff - LBR
		case 0x2561: //terathan staff - LBR
		case 0x2565: //troll maul - LBR
		case 0x2566: //frost troll club - LBR
		case 0x2568: //orc mage staff - LBR
		case 0x2569: //bone mage staff - LBR
		case 0x256B: //magic staff - LBR
		case 0x256C: //magic staff - LBR
		case 0x256D: //magic staff - LBR
		case 0x256E: //magic staff - LBR
		case 0x27A3: //tessen - SE
		case 0x27EE: //tessen - SE
		case 0x27A6: //tetsubo - SE
		case 0x27F1: //tetsubo - SE
		case 0x27AE: //nunchako - SE
		case 0x27F9: //nunchako - SE
		case 0x48C0: //gargish war hammer - SA
		case 0x48C1: //gargish war hammer - SA
		case 0x48CC: //gargish tessen - SA
		case 0x48CD: //gargish tessen - SA
		case 0x0905: //gargish glass staff - SA
		case 0x4070: //gargish glass staff - SA
		case 0x0906: //gargish serpentstone staff - SA
		case 0x406F: //gargish serpentstone staff - SA
		case 0xA343: //skull staff south
		case 0xA344: //skull staff east
		case 0xA347: //gargoyle skull staff south
		case 0xA348: //gargoyle skull staff east
			return LG_MACES;
			// Bows
		case 0x13B1: //bow
		case 0x13B2: //bow
		case 0x26C2: //composite bow - AoS
		case 0x26CC: //composite bow - AoS
		case 0x2571: //horseman's bow - LBR
		case 0x27A5: //yumi - SE
		case 0x27F0: //yumi - SE
		case 0x2D1E: //elven composite longbow - ML
		case 0x2D2A: //elven composite longbow - ML
		case 0x2D1F: //magical shortbow - ML
		case 0x2D2B: //magical shortbow - ML
			return BOWS;
			// Crossbows
		case 0x0F4F: //crossbow
		case 0x0F50: //crossbow
		case 0x13FC: //heavy crossbow
		case 0x13FD: //heavy crossbow
		case 0x26C3: //repeating crossbow - AoS
		case 0x26CD: //repeating crossbow - AoS
			return XBOWS;
		case 0x27AA: //fukiya - SE - Blowgun, uses Dart ammo (0x2806 or 0x2804)
		case 0x27F5: //fukiya - SE - Blowgun, uses Dart ammo (0x2806 or 0x2804)
			return BLOWGUNS;
			// Normal Fencing Weapons
		case 0x0F51: //dagger
		case 0x0F52: //dagger
		case 0x1400: //kryss
		case 0x1401: //kryss
		case 0x1402: //short spear
		case 0x1403: //short spear
		case 0x1404: //war fork
		case 0x1405: //war fork
		case 0x257C: //sword(rapier) - LBR
		case 0x2D20: //elven spellblade - ML
		case 0x2D2C: //elven spellblade - ML
		case 0x2D21: //assassin spike - ML
		case 0x2D2D: //assassin spike - ML
		case 0x2D22: //leafblade - ML
		case 0x2D2E: //leafblade - ML
		case 0x2D23: //war cleaver - ML
		case 0x2D2F: //war cleaver - ML
		case 0x0902: //gargish dagger - SA
		case 0x406A: //gargish dagger
		case 0x08FE: //gargish bloodblade - SA
		case 0x4072: //gargish bloodblade - SA
		case 0x48BC: //gargish kryss - SA
		case 0x48BD: //gargish kryss - SA
		case 0xA28A: //spiked whip - ToL
		case 0xA292: //spiked whip - ToL
			return DEF_FENCING;
			// Stabbing Fencing Weapons
		case 0x0E87: //pitchfork
		case 0x0E88: //pitchfork
		case 0x0F62: //spear
		case 0x0F63: //spear
		case 0x26BE: //pike - AoS
		case 0x26C8: //pike - AoS
		case 0x26BF: //double bladed staff - AoS
		case 0x26C9: //double bladed staff - AoS
		case 0x26C0: //lance - AoS
		case 0x26CA: //lance - AoS
		case 0x2558: //lizardman's spear - LBR
		case 0x2562: //terathan spear - LBR
		case 0x2572: //horseman's javelin - LBR
		case 0x257A: //spear - LBR
		case 0x257B: //spear - LBR
		case 0x27A7: //lajatang - SE
		case 0x27F2: //lajatang - SE
		case 0x0904: //gargish dual pointed spear - SA
		case 0x406D: //gargish dual pointed spear - SA
		case 0x48C8: //gargish pike - SA
		case 0x48C9: //gargish pike - SA
		case 0x48CA: //gargish lance - SA
		case 0x48CB: //gargish lance - SA
			return TWOHND_FENCING;
		case 0x27AF: //sai - SE
		case 0x27FA: //sai - SE
			return DUAL_FENCING_STAB;
		case 0x27AB: //tekagi - SE
		case 0x27F6: //tekagi - SE
		case 0x27AD: //kama - SE
		case 0x27F8: //kama - SE
		case 0x48CE: //gargish tekagi - SA
		case 0x48CF: //gargish tekagi - SA
			return DUAL_FENCING_SLASH;
		case 0x27A9: //daisho - SE
		case 0x27F4: //daisho - SE
		case 0x08FD: //gargish dual short axes - SA
		case 0x4068: //gargish dual short axes - SA
		case 0x48D0: //gargish daisho - SA
		case 0x48D1: //gargish daisho - SA
			return DUAL_SWORD;
		case 0x08FF: //gargish boomerang - SA
		case 0x4067: //gargish boomerang - SA
		case 0x0901: //gargish cyclone - SA
		case 0x406C: //gargish cyclone - SA
		case 0x090A: //gargish soul glaive - SA
		case 0x406B: //gargish soul glaive - SA
			return THROWN;
		default: // Wrestling
			return WRESTLING;
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 getBowType( CItem *bItem )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Find if character is holding a Bow or XBow
//o-----------------------------------------------------------------------------------------------o
UI08 CHandleCombat::getBowType( CItem *bItem )
{
	if( !ValidateObject( bItem ) )
		return 0;

	switch( getWeaponType( bItem ) )
	{
		case BOWS:
			return BOWS;
		case XBOWS:
			return XBOWS;
		case BLOWGUNS:
			return BLOWGUNS;
		default:
			return 0;
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 getCombatSkill( CItem *wItem )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets your combat skill based on the weapon in your hand (if any)
//o-----------------------------------------------------------------------------------------------o
UI08 CHandleCombat::getCombatSkill( CItem *wItem )
{
	if( !ValidateObject( wItem ) )
		return WRESTLING;

	switch( getWeaponType( wItem ) )
	{
		case DEF_SWORDS:
		case SLASH_SWORDS:
		case ONEHND_LG_SWORDS:
		case TWOHND_LG_SWORDS:
		case ONEHND_AXES:
		case TWOHND_AXES:
		case BARDICHE:
		case DUAL_SWORD:
			return SWORDSMANSHIP;
		case DEF_MACES:
		case LG_MACES:
			return MACEFIGHTING;
		case DEF_FENCING:
		case TWOHND_FENCING:
		case DUAL_FENCING_SLASH:
		case DUAL_FENCING_STAB:
			return FENCING;
		case BOWS:
		case XBOWS:
		case BLOWGUNS:
			return ARCHERY;
		case THROWN:
			return THROWING;
		case WRESTLING:
		default:
			return WRESTLING;
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 calcAtt( CChar *p, bool doDamage  )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate total attack power and do damage to the weapon
//o-----------------------------------------------------------------------------------------------o
SI16 CHandleCombat::calcAtt( CChar *p, bool doDamage )
{
	if( !ValidateObject( p ) )
		return 0;

	SI16 getDamage = 0;

	CItem *weapon = getWeapon( p );
	if( ValidateObject( weapon ) )
	{
		if( weapon->GetLoDamage() > 0 && weapon->GetHiDamage() > 0 )
		{
			if( weapon->GetLoDamage() >= weapon->GetHiDamage() ) {
				getDamage += weapon->GetLoDamage();
			}
			else {
				getDamage += RandomNum( weapon->GetLoDamage(), weapon->GetHiDamage() );
			}

			if( doDamage && !p->IsNpc() && ( cwmWorldState->ServerData()->CombatWeaponDamageChance() >= RandomNum( 1, 100 )))
			{
				SI08 weaponDamage = 0;
				UI08 weaponDamageMin = 0;
				UI08 weaponDamageMax = 0;

				weaponDamageMin = cwmWorldState->ServerData()->CombatWeaponDamageMin();
				weaponDamageMax = cwmWorldState->ServerData()->CombatWeaponDamageMax();

				weaponDamage -= static_cast<std::uint8_t>( RandomNum( static_cast<std::uint16_t>(weaponDamageMin), static_cast<std::uint16_t>(weaponDamageMax) ));
				weapon->IncHP( weaponDamage );

				if( weapon->GetHP() <= 0 )
				{
					CSocket *mSock = p->GetSocket();
					if( mSock != nullptr )
					{
						std::string name;
						getTileName( (*weapon), name );
						mSock->sysmessage( 311, name.c_str() );
					}
					weapon->Delete();
				}
			}
		}
	}
	else if( p->IsNpc() )
	{
		if( p->GetLoDamage() >= p->GetHiDamage() ){
			getDamage = p->GetLoDamage();
		}
		else if( p->GetHiDamage() > 2 ){
			getDamage = RandomNum( p->GetLoDamage(), p->GetHiDamage() );
		}
	}
	else
	{
		UI16 getWrestSkill = ( p->GetSkill( WRESTLING ) / 65 );
		if( getWrestSkill > 0 ) {
			getDamage = HalfRandomNum( getWrestSkill );
		}
		else{
			getDamage = RandomNum( 1, 2 );
		}
	}
	if( getDamage < 1 )
		getDamage = 1;
	return getDamage;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 calcLowDamage( CChar *p )
//|	Date		-	11. Mar, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate low damage value of the char
//o-----------------------------------------------------------------------------------------------o
SI16 CHandleCombat::calcLowDamage( CChar *p )
{
	if( !ValidateObject( p ) )
		return 0;

	SI16 getDamage = 0;

	CItem *weapon = getWeapon( p );
	if( ValidateObject( weapon ) )
	{
		if( weapon->GetLoDamage() > 0 && weapon->GetHiDamage() > 0 )
		{
			if( weapon->GetLoDamage() >= weapon->GetHiDamage() )
				getDamage = weapon->GetHiDamage();
			else
				getDamage = weapon->GetLoDamage();
		}
	}
	else if( p->IsNpc() )
	{
		if( p->GetLoDamage() >= p->GetHiDamage() )
			getDamage = p->GetHiDamage();
		else if( p->GetHiDamage() > 2 )
			getDamage = p->GetLoDamage();
	}
	else
	{
		UI16 getWrestSkill = ( p->GetSkill( WRESTLING ) / 65 );
		if( getWrestSkill > 0 )
			getDamage = ( getWrestSkill / 2);
		else
			getDamage = 1;
	}
	if( getDamage < 1 )
		getDamage = 1;
	return getDamage;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 calcHighDamage( CChar *p )
//|	Date		-	11. Mar, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate high damage value of the char
//o-----------------------------------------------------------------------------------------------o
SI16 CHandleCombat::calcHighDamage( CChar *p )
{
	if( !ValidateObject( p ) )
		return 0;

	SI16 getDamage = 0;

	CItem *weapon = getWeapon( p );
	if( ValidateObject( weapon ) )
	{
		if( weapon->GetLoDamage() > 0 && weapon->GetHiDamage() > 0 )
		{
			if( weapon->GetLoDamage() >= weapon->GetHiDamage() )
				getDamage = weapon->GetLoDamage();
			else
				getDamage = weapon->GetHiDamage();

		}
	}
	else if( p->IsNpc() )
	{
		if( p->GetLoDamage() >= p->GetHiDamage() )
			getDamage = p->GetLoDamage();
		else if( p->GetHiDamage() > 2 )
			getDamage = p->GetHiDamage();
	}
	else
	{
		UI16 getWrestSkill = ( p->GetSkill( WRESTLING ) / 65 );
		if( getWrestSkill > 0 )
			getDamage = getWrestSkill;
		else
			getDamage = 2;
	}
	if( getDamage < 1 )
		getDamage = 1;
	return getDamage;
}

/*
	New Defense Calculations
	Date: 3/03/2003

	Purpose:
		To emulate OSI's armor calculation methods for specific hit locations and total AR level

	How we do this:
		We need to first determine what we are calculating, total AR (hitLoc 0) or the armor level of a specific locations (hitLoc 1-6).
		Next we need to loop through the items a player has equipped searching for items that match the following criteria:
			Defence > 0
			On a specific layer (depending on the hit location)
			The item with the most AR covering that location
		Lastly we have to return this information. If we are returning the AR of a specific hit location, then we return the actual Def value of the item
			if we are returning the total AR of a character, we need to find the AR / the divisor used for that specific location.

		Divisors based upon location / Armor (as best I can match it to OSI)
			Torso (hitLoc 1):	AR / 2.8
			Arms (hitLoc 2):	AR / 6.8
			Head (hitLoc 3):	AR / 7.3
			Legs (hitLoc 4):	AR / 4.5
			Neck (hitLoc 5):	AR / 14.5
			Hands (hitLoc 6):	AR / 14.5

		Functions:
			checkDef( CItem *checkItem, CItem& currItem, SI32 &currDef )
			getArmorDef( CChar *mChar, CItem& defItem, UI08 bodyLoc, bool findTotal )
			calcDef( CChar *mChar, UI08 hitLoc, bool doDamage )

	All information on armor defence values, divisors, and combat calculations gleaned from uo.stratics.com
	http://uo.stratics.com/content/arms-armor/armor.shtml
	http://uo.stratics.com/content/arms-armor/combat.shtml
	http://uo.stratics.com/content/arms-armor/clothes.shtml
	http://uo.stratics.com/content/aos/combatchanges.shtml
*/

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CItem *checkDef( CItem *checkItem, CItem *currItem, SI32 &currDef, WeatherType resistType )
//|	Date		-	3/03/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks the defense of checkItem vs the defense of currItem and returns
//|							the item with the greater Def and its def value
//o-----------------------------------------------------------------------------------------------o
CItem *CHandleCombat::checkDef( CItem *checkItem, CItem *currItem, SI32 &currDef, WeatherType resistType )
{
	if( ValidateObject( checkItem ) && checkItem->GetResist( resistType ) > currDef )
	{
		currDef = checkItem->GetResist( resistType );
		return checkItem;
	}
	return currItem;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CItem * getArmorDef( CChar *mChar, SI32 &totalDef, UI08 bodyLoc, bool findTotal, WeatherType resistType )
//|	Date		-	3/03/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Finds the item covering the location bodyLoc with the greatest AR and
//|							returns it along with its def value
//o-----------------------------------------------------------------------------------------------o
CItem * CHandleCombat::getArmorDef( CChar *mChar, SI32 &totalDef, UI08 bodyLoc, bool findTotal, WeatherType resistType )
{
	SI32 armorDef = 0;
	CItem *currItem = nullptr;
	switch( bodyLoc )
	{
		case 1:		// Torso
			currItem = checkDef( mChar->GetItemAtLayer( IL_INNERSHIRT ), currItem, armorDef, resistType );	// Shirt
			currItem = checkDef( mChar->GetItemAtLayer( IL_TUNIC ), currItem, armorDef, resistType );	// Torso (Inner - Chest Armor)
			currItem = checkDef( mChar->GetItemAtLayer( IL_OUTERSHIRT ), currItem, armorDef, resistType );	// Torso (Middle - Tunic, etc)
			currItem = checkDef( mChar->GetItemAtLayer( IL_CLOAK ), currItem, armorDef, resistType );	// Back (Cloak)
			currItem = checkDef( mChar->GetItemAtLayer( IL_ROBE ), currItem, armorDef, resistType );	// Torso (Outer - Robe)
			if( findTotal )
				armorDef = (SI32)(100 * (R32)( armorDef / 2.8 ));
			break;
		case 2:		// Arms
			currItem = checkDef( mChar->GetItemAtLayer( IL_ARMS ), currItem, armorDef, resistType );	// Arms
			if( findTotal )
				armorDef = (SI32)(100 * (R32)( armorDef / 6.8 ));
			break;
		case 3:		// Head
			currItem = checkDef( mChar->GetItemAtLayer( IL_HELM ), currItem, armorDef, resistType );	// Head
			if( findTotal )
				armorDef = (SI32)(100 * (R32)( armorDef / 7.3 ));
			break;
		case 4:		// Legs
			currItem = checkDef( mChar->GetItemAtLayer( IL_FOOTWEAR ), currItem, armorDef, resistType );	// Shoes
			currItem = checkDef( mChar->GetItemAtLayer( IL_PANTS ), currItem, armorDef, resistType );	// Pants
			currItem = checkDef( mChar->GetItemAtLayer( IL_WAIST ), currItem, armorDef, resistType );	// Waist (Half Apron)
			currItem = checkDef( mChar->GetItemAtLayer( IL_OUTERLEGGINGS ), currItem, armorDef, resistType );	// Legs (Outer - Skirt, Kilt)
			currItem = checkDef( mChar->GetItemAtLayer( IL_INNERLEGGINGS ), currItem, armorDef, resistType );	// Legs (Inner - Leg Armor)
			if( findTotal )
				armorDef = (SI32)(100 * (R32)( armorDef / 4.5 ));
			break;
		case 5:		// Neck
			currItem = checkDef( mChar->GetItemAtLayer( IL_NECK ), currItem, armorDef, resistType );	// Neck
			if( findTotal )
				armorDef = (SI32)(100 * (R32)( armorDef / 14.5 ));
			break;
		case 6:		// Hands
			currItem = checkDef( mChar->GetItemAtLayer( IL_GLOVES ), currItem, armorDef, resistType );	// Gloves
			if( findTotal )
				armorDef = (SI32)(100 * (R32)( armorDef / 14.5 ));
			break;
		default:
			break;
	}
	if( findTotal )
		totalDef += armorDef;
	else
		totalDef = armorDef;

	return currItem;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 calcDef( CChar *mChar, UI08 hitLoc, bool doDamage, WeatherType resistType )
//|	Date		-	3/03/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Finds the defense value of a specific location or the entire character based on hitLoc
//o-----------------------------------------------------------------------------------------------o
UI16 CHandleCombat::calcDef( CChar *mChar, UI08 hitLoc, bool doDamage, WeatherType resistType )
{
	if( !ValidateObject( mChar ) )
		return 0;

	SI32 total = mChar->GetResist( resistType );

	if( !mChar->IsNpc() || cwmWorldState->creatures[mChar->GetID()].IsHuman() ) // Polymorphed Characters and GM's can still wear armor
	{
		CItem *defendItem = nullptr;
		if( hitLoc == 0 )
		{
			for( UI08 getLoc = 1; getLoc < 7; ++getLoc )
				getArmorDef( mChar, total, getLoc, true, resistType );
			total = (total / 100);
		}
		else
			defendItem = getArmorDef( mChar, total, hitLoc, false, resistType );

		if( total > 0 && doDamage && ValidateObject( defendItem ) && !mChar->IsNpc() && ( cwmWorldState->ServerData()->CombatArmorDamageChance() >= RandomNum( 1, 100 )))
		{
			SI08 armorDamage = 0;	// Based on OSI standards, each successful hit does 0 to 2 damage to armor hit
			UI08 armorDamageMin = 0;
			UI08 armorDamageMax = 0;

			armorDamageMin = cwmWorldState->ServerData()->CombatArmorDamageMin();
			armorDamageMax = cwmWorldState->ServerData()->CombatArmorDamageMax();

			armorDamage -= static_cast<std::uint8_t>(RandomNum( static_cast<std::uint16_t>(armorDamageMin), static_cast<std::uint16_t>(armorDamageMax )));
			defendItem->IncHP( armorDamage );

			if( defendItem->GetHP() <= 0 )
			{
				CSocket *mSock = mChar->GetSocket();
				if( mSock != nullptr )
				{
					std::string name;
					getTileName( (*defendItem), name );
					mSock->sysmessage( 311, name.c_str() );
				}
				defendItem->Delete();
			}
		}
	}

	//if( total < 2 && hitLoc != 0 )
	//	total = 2;
	return (UI16)total;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CombatAnimsNew( CChar *i )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Play combat animations for characters (including gargoyles) in clients v7.0.0.0+
//o-----------------------------------------------------------------------------------------------o
void CHandleCombat::CombatAnimsNew( CChar *i )
{
	if( !ValidateObject( i ) )
		return;

	UI16 animToPlay		= N_ACT_ATT;		// 0x00
	UI16 subAnimToPlay	= S_ACT_WRESTLE;	// 0x00
	CItem *j		= getWeapon( i );

	if( i->IsFlying() || i->IsOnHorse() )
	{
		switch( getWeaponType( j ) )
		{
			case BOWS:
			case XBOWS:
			case DEF_SWORDS:
			case SLASH_SWORDS:
			case ONEHND_LG_SWORDS:
			case ONEHND_AXES:
			case DEF_MACES:
			case DEF_FENCING:
			case WRESTLING:
			case BLOWGUNS:			subAnimToPlay = S_ACT_2H_SLASH;	break; // 0x07
			case DUAL_SWORD:
			case DUAL_FENCING_SLASH:
			case DUAL_FENCING_STAB:
			case TWOHND_FENCING:
			case TWOHND_LG_SWORDS:
			case BARDICHE:
			case TWOHND_AXES:
			case LG_MACES:
			case THROWN:
			default:				subAnimToPlay = S_ACT_WRESTLE;	break; // 0x00
		}
	}
	else
	{
		switch( getWeaponType( j ) )
		{
			case BOWS:				subAnimToPlay = S_ACT_BOW;	break; // 0x01
			case XBOWS:
			case BLOWGUNS:			subAnimToPlay = S_ACT_XBOW;	break; // 0x02
			case THROWN:			subAnimToPlay = S_ACT_THROW;break; // 0x09
			case DEF_SWORDS:
			case SLASH_SWORDS:
			case ONEHND_LG_SWORDS:
				subAnimToPlay = ( RandomNum( 0, 2 ) > 1 ? (RandomNum( 0, 3 ) > 2 ? S_ACT_1H_PIERCE : S_ACT_1H_BASH ) : S_ACT_1H_SLASH ); // 0x05, 0x03 and 0x04
				break;
			case ONEHND_AXES:
			case DEF_MACES:			subAnimToPlay = ( RandomNum( 0, 3 ) > 2 ? S_ACT_1H_SLASH : S_ACT_1H_BASH ); break; // 0x04 and 0x03
			case DUAL_FENCING_STAB: 
			case DEF_FENCING:		subAnimToPlay = ( RandomNum( 0, 3 ) > 2 ? S_ACT_1H_SLASH : S_ACT_1H_PIERCE ); break; // 0x04 and 0x05
			case TWOHND_FENCING:	subAnimToPlay = S_ACT_2H_PIERCE;	break; // 0x08
			case TWOHND_LG_SWORDS:
			case DUAL_SWORD:
			case DUAL_FENCING_SLASH:
			case BARDICHE:
				subAnimToPlay = ( RandomNum( 0, 2 ) > 1 ? (RandomNum( 0, 3 ) > 2 ? S_ACT_2H_PIERCE : S_ACT_2H_BASH ) : S_ACT_2H_SLASH ); // 0x08, 0x06 and 0x07
				break;
			case TWOHND_AXES:
			case LG_MACES:			subAnimToPlay = ( RandomNum( 0, 3 ) > 2 ? S_ACT_2H_BASH : S_ACT_2H_SLASH ); break; // 0x06 and 0x07
			case WRESTLING:
			default:				subAnimToPlay = S_ACT_WRESTLE;	break; // 0x00
		}
	}
	Effects->PlayNewCharacterAnimation( i, animToPlay, subAnimToPlay );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CombatOnHorse( CChar *i )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Play combat animations for mounted characters in clients v6.x and below
//o-----------------------------------------------------------------------------------------------o
void CHandleCombat::CombatOnHorse( CChar *i )
{
	if( !ValidateObject( i ) )
		return;

	UI16 animToPlay = ACT_MOUNT_ATT_1H; // 0x1A;
	CItem *j		= getWeapon( i );

	// Normal mounted characters
	switch( getWeaponType( j ) )
	{
		case BOWS:				animToPlay = ACT_MOUNT_ATT_BOW;		break; // 0x1B
		case XBOWS:
		case BLOWGUNS:			animToPlay = ACT_MOUNT_ATT_XBOW;	break; // 0x1C
		case THROWN:
		case DEF_SWORDS:
		case SLASH_SWORDS:
		case DUAL_SWORD:
		case ONEHND_LG_SWORDS:
		case ONEHND_AXES:
		case DEF_MACES:
		case DUAL_FENCING_SLASH:
		case DUAL_FENCING_STAB:
		case DEF_FENCING:		animToPlay = ACT_MOUNT_ATT_1H;	break; // 0x1A
		case TWOHND_FENCING:
		case TWOHND_LG_SWORDS:
		case BARDICHE:
		case TWOHND_AXES:
		case LG_MACES:			animToPlay = ACT_MOUNT_ATT_2H;	break; // 0x1D
		case WRESTLING:
		default:				animToPlay = ACT_MOUNT_ATT_1H;	break; // 0x1A
	}
	Effects->PlayCharacterAnimation( i, animToPlay );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CombatOnFoot( CChar *i )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Play combat animations for characters on foot in clients v6.x and below
//o-----------------------------------------------------------------------------------------------o
void CHandleCombat::CombatOnFoot( CChar *i )
{
	if( !ValidateObject( i ) )
		return;

	UI16 animToPlay	= 0x00;
	CItem *j		= getWeapon( i );
	switch( getWeaponType( j ) )
	{
		case BOWS:				animToPlay = ACT_ATT_BOW;		break; // 0x12
		case XBOWS:
		case BLOWGUNS:			animToPlay = ACT_ATT_XBOW;		break; // 0x13
		case DEF_SWORDS:
		case SLASH_SWORDS:
		case ONEHND_LG_SWORDS:
		case DUAL_SWORD:
		case DUAL_FENCING_SLASH:
		case ONEHND_AXES:		animToPlay = (RandomNum( 0, 2 ) < 1 ? ACT_ATT_1H_SLASH : ACT_ATT_1H_PIERCE );	break; // 0x09 and 0x0A
		case DEF_MACES:			animToPlay = (RandomNum( 0, 2 ) < 2 ? ACT_ATT_1H_SLASH : ACT_ATT_1H_BASH );		break; // 0x09 and 0x0B
		case LG_MACES:
		case TWOHND_LG_SWORDS:
		case BARDICHE:
		case TWOHND_AXES:		animToPlay = (RandomNum( 0, 1 ) ? ACT_ATT_2H_BASH : ACT_ATT_2H_SLASH );	break; // 0x0C and 0x0D
		case DUAL_FENCING_STAB:
		case DEF_FENCING:		animToPlay = ACT_ATT_1H_PIERCE;	break; // 0x0A
		case TWOHND_FENCING:	animToPlay = ACT_ATT_2H_PIERCE;	break; // 0x0E
		case WRESTLING:
		default:
			switch( RandomNum( 0, 2 ) )
			{
				case 0:		animToPlay = ACT_ATT_1H_BASH;		break; // 0x0B
				case 1:		animToPlay = ACT_ATT_1H_SLASH;		break; // 0x09
				default:	animToPlay = ACT_ATT_WRESTLE;		break; // 0x1F
			}
			break;
	}
	Effects->PlayCharacterAnimation( i, animToPlay );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void PlaySwingAnimations( CChar *mChar )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Plays the attack/swing-animation for specified character
//o-----------------------------------------------------------------------------------------------o
void CHandleCombat::PlaySwingAnimations( CChar *mChar )
{
	UI16 charID = mChar->GetID();
	if( !cwmWorldState->creatures[charID].IsHuman() )
	{
		UI08 aa = 0;
		if( cwmWorldState->creatures[charID].AntiBlink() )
			aa = RandomNum( 5, 6 ); // some creatures dont have animation #4, or it's not an attack
		else
			aa = RandomNum( 4, 6 );
		if( charID == 5 ) // eagles need special treatment
		{
			switch( RandomNum( 0, 2 ) )
			{
				case 0: aa = 0x1;  break;
				case 1: aa = 0x14; break;
				case 2: aa = 0x4;  break;
			}
		}
		Effects->PlayCharacterAnimation( mChar, aa );
		if( RandomNum( 0, 4 ) )
		{
			UI16 toPlay = cwmWorldState->creatures[charID].GetSound( SND_ATTACK );
			if( toPlay != 0x00 )
				Effects->PlaySound( mChar, toPlay );
		}
	}
	else if( mChar->GetBodyType() == BT_GARGOYLE 
		|| ( cwmWorldState->ServerData()->ForceNewAnimationPacket() && ( mChar->GetSocket() == nullptr || mChar->GetSocket()->ClientType() >= CV_SA2D ))) //mChar->GetSocket()->ClientVerShort() >= CVS_7000 )))
		CombatAnimsNew( mChar );
	else if( mChar->IsOnHorse() )
		CombatOnHorse( mChar );
	else
		CombatOnFoot( mChar );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void PlayMissedSoundEffect( CChar *p )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Do the "Missed" Sound Effect
//o-----------------------------------------------------------------------------------------------o
void CHandleCombat::PlayMissedSoundEffect( CChar *p )
{
	CItem *weapon = getWeapon( p );

	if( !ValidateObject( p ) )
		return;

	switch( getWeaponType( weapon ) )
	{
		case BOWS:
		case XBOWS: Effects->PlaySound( p, RandomNum( 0x04c8, 0x04c9 )); break;
		case BLOWGUNS: Effects->PlaySound( p, 0x052F ); break;
		default:
			switch( RandomNum( 0, 2 ) )
			{
				case 0:
					Effects->PlaySound( p, 0x0238 );
					break;
				case 1:
					Effects->PlaySound( p, 0x0239 );
					break;
				default:
					Effects->PlaySound( p, 0x023A );
					break;
			}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void PlayHitSoundEffect( CChar *p, CItem *weapon )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Do the "Hit" Sound Effect
//o-----------------------------------------------------------------------------------------------o
void CHandleCombat::PlayHitSoundEffect( CChar *p, CItem *weapon )
{
	if( !ValidateObject( p ) || !ValidateObject( weapon ) )
		return;

	switch( getWeaponType( weapon ) )
	{
		case ONEHND_AXES:
		case TWOHND_AXES:
		case DEF_MACES:
		case LG_MACES:
			Effects->PlaySound( p, RandomNum( 0x0232, 0x0233 )); // Whoosh Weapons
			break;
		case DEF_SWORDS:
		case DEF_FENCING:
		case TWOHND_FENCING:
		case DUAL_FENCING_STAB:
			Effects->PlaySound( p, RandomNum( 0x023B, 0x023C )); // Stabbing Weapons
			break;
		case BARDICHE:
			Effects->PlaySound( p, RandomNum( 0x0236, 0x0237 )); // Bardiche
			break;
		case SLASH_SWORDS:
		case DUAL_SWORD:
		case DUAL_FENCING_SLASH:
		case ONEHND_LG_SWORDS:
			Effects->PlaySound( p, RandomNum( 0x023B, 0x023C )); // Slashing Weapons
			break;
		case TWOHND_LG_SWORDS:
			Effects->PlaySound( p, RandomNum( 0x0236, 0x0237 )); // Large Swords
			break;
		case BOWS:
		case XBOWS:
			Effects->PlaySound( p, 0x0234 ); // Bows
			break;
		case BLOWGUNS:
			Effects->PlaySound( p, RandomNum( 0x0223, 0x0224 )); //Darts
			break;
		case WRESTLING:
		default:
			switch( RandomNum( 0, 3 ) ) // Wrestling
			{
				case 0:		Effects->PlaySound( p, 0x0135 );	break;
				case 1:		Effects->PlaySound( p, 0x0137 );	break;
				case 2:		Effects->PlaySound( p, 0x013D );	break;
				default:	Effects->PlaySound( p, 0x013B );	break;
			}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 AdjustRaceDamage( CChar *attack, CChar *defend, CItem *weapon, SI16 bDamage, UI08 hitLoc, UI08 getFightSkill )
//|	Date		-	3rd July, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Adjusts the damage dealt to defend by weapon based on
//|						race and weather weaknesses
//o-----------------------------------------------------------------------------------------------o
SI16 CHandleCombat::AdjustRaceDamage( CChar *attack, CChar *defend, CItem *weapon, SI16 bDamage, UI08 hitLoc, UI08 getFightSkill )
{
	SI16 amount		= 0;

	if( !ValidateObject( defend ) || !ValidateObject( weapon ) )
		return bDamage;

	if( weapon->GetRace() == defend->GetRace() )
		amount = bDamage;
	CRace *rPtr = Races->Race( defend->GetRace() );
	if( rPtr != nullptr )
	{
		for( SI32 i = LIGHT; i < WEATHNUM; ++i )
		{
			if( weapon->GetWeatherDamage( (WeatherType)i ) && rPtr->AffectedBy( (WeatherType)i ) )
			{
				amount += ApplyDefenseModifiers( (WeatherType)i, attack, defend, getFightSkill, hitLoc, bDamage, false );
			}
		}
	}
	return (bDamage	+ amount);
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void DoHitMessage( CChar *mChar, CChar *ourTarg, SI08 hitLoc, SI16 damage )
//|	Date		-	3rd July, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Prints out the hit message, if enabled
//o-----------------------------------------------------------------------------------------------o
void CHandleCombat::DoHitMessage( CChar *mChar, CChar *ourTarg, SI08 hitLoc, SI16 damage )
{
	if( !ValidateObject( ourTarg ) || !ValidateObject( mChar ) || ourTarg->IsNpc() )
		return;

	CSocket *targSock = ourTarg->GetSocket();

	if( cwmWorldState->ServerData()->CombatDisplayHitMessage() && targSock != nullptr )
	{
		UI08 randHit = RandomNum( 0, 2 );
		switch( hitLoc )
		{
			case 1:	// Body
				switch ( randHit )
				{
					case 1:
						if( damage < 10  )
							targSock->sysmessage( 284, mChar->GetName().c_str() );
						else
							targSock->sysmessage( 285, mChar->GetName().c_str() );
						break;
					case 2:
						if( damage < 10 )
							targSock->sysmessage( 286, mChar->GetName().c_str() );
						else
							targSock->sysmessage( 287, mChar->GetName().c_str() );
						break;
					default:
						if( damage < 10 )
							targSock->sysmessage( 288, mChar->GetName().c_str() );
						else
							targSock->sysmessage( 289, mChar->GetName().c_str() );
						break;
				}
				break;
			case 2:	// Arms
				switch( randHit )
				{
					case 1:		targSock->sysmessage( 290, mChar->GetName().c_str() );		break;
					case 2:		targSock->sysmessage( 291, mChar->GetName().c_str() );		break;
					default:	targSock->sysmessage( 292, mChar->GetName().c_str() );		break;
				}
				break;
			case 3:	// Head
				switch( randHit )
				{
					case 1:
						if( damage <  10 )
							targSock->sysmessage( 293, mChar->GetName().c_str() );
						else
							targSock->sysmessage( 294, mChar->GetName().c_str() );
						break;
					case 2:
						if( damage <  10 )
							targSock->sysmessage( 295, mChar->GetName().c_str() );
						else
							targSock->sysmessage( 296, mChar->GetName().c_str() );
						break;
					default:
						if( damage <  10 )
							targSock->sysmessage( 297, mChar->GetName().c_str() );
						else
							targSock->sysmessage( 298, mChar->GetName().c_str() );
						break;
				}
				break;
			case 4:	// Legs
				switch( randHit )
				{
					case 1:		targSock->sysmessage( 299, mChar->GetName().c_str() );		break;
					case 2:		targSock->sysmessage( 300, mChar->GetName().c_str() );		break;
					default:	targSock->sysmessage( 301, mChar->GetName().c_str() );		break;
				}
				break;
			case 5:	// Neck
				targSock->sysmessage( 302, mChar->GetName().c_str() );
				break;
			case 6:	// Hands
				switch( randHit )
				{
					case 1:		targSock->sysmessage( 303, mChar->GetName().c_str() );		break;
					case 2:		targSock->sysmessage( 304, mChar->GetName().c_str() );		break;
					default:	targSock->sysmessage( 305, mChar->GetName().c_str() );		break;
				}
				break;
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 CalculateHitLoc( void )
//|	Date		-	3rd July, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	calculates where on the body the person was hit and returns that
//o-----------------------------------------------------------------------------------------------o
SI08 CHandleCombat::CalculateHitLoc( void )
{
	SI08 hitLoc = RandomNum( 0, 99 ); // Determine area of Body Hit
	for( UI08 t = BODYPERCENT; t < TOTALTARGETSPOTS; ++t )
	{
		hitLoc -= LOCPERCENTAGES[t];
		if( hitLoc < 0 )
		{
			hitLoc = t + 1;
			break;
		}
	}
	return hitLoc;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 ApplyDamageBonuses( WeatherType damageType, CChar *mChar, CChar *ourTarg, UI08 getFightSkill, UI08 hitLoc, SI16 baseDamage )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies damage bonuses based on race/weather weakness and character skills
//o-----------------------------------------------------------------------------------------------o
SI16 CHandleCombat::ApplyDamageBonuses( WeatherType damageType, CChar *mChar, CChar *ourTarg, UI08 getFightSkill, UI08 hitLoc, SI16 baseDamage )
{
	if( !ValidateObject( ourTarg ) || !ValidateObject( mChar ) )
		return baseDamage;

	R32 multiplier;
	R32 damage = 0;
	SI32 RaceDamage = 0;
	CItem *mWeapon = getWeapon( mChar );
	CRace *rPtr = Races->Race( ourTarg->GetRace() );

	switch( damageType )
	{
		case NONE:
			damage = (R32)baseDamage;
			break;
		case PHYSICAL:
			// Race Dmg Modification: Bonus percentage.
			RaceDamage = Races->DamageFromSkill( getFightSkill, mChar->GetRace() );
			if( RaceDamage != 0 )
				baseDamage += (SI16)( (R32)baseDamage * ( (R32)RaceDamage / 1000 ) );

			// Adjust race and weather weakness
			baseDamage = AdjustRaceDamage( mChar, ourTarg, mWeapon, baseDamage, hitLoc, getFightSkill );

			// Strength Damage Bonus, +20% Damage
			multiplier = static_cast<R32>( ( ( std::min( mChar->GetStrength(), static_cast<SI16>(200) ) * 20 ) / 100 ) / 100 ) + 1;
			damage = (R32)(baseDamage * multiplier);

			// Tactics Damage Bonus, % = ( Tactics + 50 )
			multiplier = static_cast<R32>( ( mChar->GetSkill( TACTICS ) + 500 ) / 1000 );
			damage += (R32)(baseDamage * multiplier);

			if( ourTarg->IsNpc() ) // Anatomy PvM damage Bonus, % = ( Anat / 5 )
				multiplier = static_cast<R32>( ( mChar->GetSkill( ANATOMY ) / 5 ) / 1000 );
			else // Anatomy PvP damage Bonus, % = ( Anat / 2.5 )
				multiplier = static_cast<R32>( ( mChar->GetSkill( ANATOMY ) / 2.5 ) / 1000 );
			damage += (R32)(baseDamage * multiplier);

			// Lumberjacking Bonus ( 30% Damage at GM Skill )
			if( mChar->GetSkill( LUMBERJACKING ) >= 1000 )
				damage += (R32)(baseDamage * .3);

			// Defender Tactics Damage Modifier, -20% Damage
			multiplier = static_cast<R32>(1.0 - ( ( ( ourTarg->GetSkill( TACTICS ) * 20 ) / 1000 ) / 100 ));
			damage = (R32)(damage * multiplier);
			break;
		default:
			damage = (R32)baseDamage;
			// If the race is weak to this element double the damage
			if( rPtr != nullptr )
			{
				if( rPtr->AffectedBy( damageType ) )
					damage *= 2;
			}

			// If the attack is magic and the target a NPC but not a human, double the damage
			if( getFightSkill == MAGERY && ourTarg->IsNpc() && !cwmWorldState->creatures[ourTarg->GetID()].IsHuman() )
				damage *= 2;
			break;
	}
	return (SI16)roundNumber( damage );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 ApplyDefenseModifiers( WeatherType damageType, CChar *mChar, CChar *ourTarg, UI08 getFightSkill, UI08 hitLoc, SI16 baseDamage, bool doArmorDamage )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies defense modifiers based on shields/parrying, armor values and elemental damage
//o-----------------------------------------------------------------------------------------------o
SI16 CHandleCombat::ApplyDefenseModifiers( WeatherType damageType, CChar *mChar, CChar *ourTarg, UI08 getFightSkill, UI08 hitLoc, SI16 baseDamage, bool doArmorDamage )
{
	if( !ValidateObject( ourTarg ) )
		return baseDamage;

	UI16 getDef = 0, attSkill = 1000;
	R32 damageModifier = 0;
	R32 damage = (R32)baseDamage;

	if( ValidateObject( mChar ) )
		attSkill = mChar->GetSkill( getFightSkill );

	CSocket *targSock = ourTarg->GetSocket();
	CItem *shield = getShield( ourTarg );

	switch( damageType )
	{
		case NONE: break;	//	No Armor protection
		case PHYSICAL:		//	Physical damage
			// Check Shield Defense
			if( ValidateObject( shield ) )
			{
				Skills->CheckSkill( ourTarg, PARRYING, 0, 1000 );
				// Chance to block with Shield ( % = Skill / 2 )
				const UI16 defendParry = ourTarg->GetSkill( PARRYING );

				if( HalfRandomNum( defendParry ) >= HalfRandomNum( attSkill ) )
				{
					// Play shield parrying FX
					Effects->PlayStaticAnimation( ourTarg, 0x37b9, 10, 16 );

					if( cwmWorldState->ServerData()->CombatDisplayHitMessage() && targSock != nullptr )
					{
						targSock->sysmessage( 1805 ); // You block the attack!
					}

					damage -= HalfRandomNum( shield->GetResist( PHYSICAL ) );
					if( !RandomNum( 0, 5 ) )
						shield->IncHP( -1 );
					if( shield->GetHP() <= 0 )
					{
						if( targSock != nullptr )
							targSock->sysmessage( 283 ); // Your shield has been destroyed!
						shield->Delete();
					}
				}
			}

			getDef = HalfRandomNum( calcDef( ourTarg, hitLoc, doArmorDamage, PHYSICAL ) );
			break;
		case POISON:		//	POISON Damage
			damageModifier = ( calcDef( ourTarg, hitLoc, doArmorDamage, damageType ) / 100 );
			damage = (SI16)roundNumber( ((R32)baseDamage - ( (R32)baseDamage * damageModifier )) );
			break;
		default:			//	Elemental damage
			getDef = HalfRandomNum( calcDef( ourTarg, hitLoc, doArmorDamage, damageType ) );
			break;
	}

	if( getDef > 0 )
		damage -= (R32)( ( (R32)getDef * (R32)attSkill ) / 750 );

	return (SI16)roundNumber( damage );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 calcDamage( CChar *mChar, CChar *ourTarg, UI08 getFightSkill, UI08 hitLoc )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate damage based on hit location, damage bonuses, defense modifiers
//o-----------------------------------------------------------------------------------------------o
SI16 CHandleCombat::calcDamage( CChar *mChar, CChar *ourTarg, UI08 getFightSkill, UI08 hitLoc )
{
	SI16 damage = -1;

	std::vector<UI16> scriptTriggers = mChar->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			// -1 == event doesn't exist, default to hard code
			// Other value = calculated damage from script
			damage = toExecute->OnCombatDamageCalc( mChar, ourTarg, getFightSkill, hitLoc );
		}
	}

	if( damage >= 0 )
		return damage;
	else
		damage = 0;

	const SI16 baseDamage = calcAtt( mChar, true );

	if( baseDamage == -1 )  // No damage if weapon breaks
		return 0;

	damage = ApplyDamageBonuses( PHYSICAL, mChar, ourTarg, getFightSkill, hitLoc, baseDamage );

	if( damage < 1 )
		return 0;

	damage = ApplyDefenseModifiers( PHYSICAL, mChar, ourTarg, getFightSkill, hitLoc, damage, true );

	if( damage <= 0 )
		damage = RandomNum( 0, 4 );

	if( !ourTarg->IsNpc() )
		damage /= cwmWorldState->ServerData()->CombatNPCDamageRate(); // Rate damage against other players

	return damage;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void HandleCombat( CSocket *mSock, CChar& mChar, CChar *ourTarg )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles combat related stuff during combat loop
//o-----------------------------------------------------------------------------------------------o
void CHandleCombat::HandleCombat( CSocket *mSock, CChar& mChar, CChar *ourTarg )
{
	const UI16 ourDist			= getDist( &mChar, ourTarg );
	//Attacker Skill values
	CItem *mWeapon				= getWeapon( &mChar );
	const UI08 getFightSkill	= getCombatSkill( mWeapon );
	const UI16 attackSkill		= std::min( 1000, (SI32)mChar.GetSkill( getFightSkill ) );

	//Defender Skill values
	CItem *defWeapon			= getWeapon( ourTarg );
	const UI08 getTargetSkill	= getCombatSkill( defWeapon );
	const UI16 defendSkill		= std::min( 1000, (SI32)ourTarg->GetSkill( getTargetSkill ) );

	bool checkDist		= (ourDist <= 1 && abs( mChar.GetZ() - ourTarg->GetZ() ) <= 15 );

	// Trigger onSwing for scripts attached to attacker
	std::vector<UI16> scriptTriggers = mChar.GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			if( toExecute->OnSwing( mWeapon, &mChar, ourTarg ) == 0 )
			{
				return;
			}
		}
	}

	// Trigger onSwing for scripts attached to attacker's weapon
	if( mWeapon )
	{
		std::vector<UI16> weaponScriptTriggers = mWeapon->GetScriptTriggers();
		for( auto scriptTrig : weaponScriptTriggers )
		{
			cScript *toExecute = JSMapping->GetScript( scriptTrig );
			if( toExecute != nullptr )
			{
				if( toExecute->OnSwing( mWeapon, &mChar, ourTarg ) == 0 )
				{
					return;
				}
			}
		}
	}

	if( !checkDist && getFightSkill == ARCHERY )
		checkDist = LineOfSight( mSock, &mChar, ourTarg->GetX(), ourTarg->GetY(), ( ourTarg->GetZ() + 15 ), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false );

	if( checkDist )
	{
		CPFightOccurring tSend( mChar, (*ourTarg) );
		SOCKLIST nearbyChars = FindNearbyPlayers( &mChar );
		for( SOCKLIST_ITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
		{
			if( (*cIter) != nullptr )
				(*cIter)->Send( &tSend );
		}

		if( mChar.IsNpc() )
		{
			if( mChar.GetNpcWander() != WT_FLEE )
			{
				UI08 charDir = Movement->Direction( &mChar, ourTarg->GetX(), ourTarg->GetY() );
				if( mChar.GetDir() != charDir && charDir < 8 )
					mChar.SetDir( charDir );
			}
		}

		if( getFightSkill == ARCHERY )
		{
			UI16 ammoID = mWeapon->GetAmmoID();
			UI16 ammoHue = mWeapon->GetAmmoHue();
			UI16 ammoFX = mWeapon->GetAmmoFX();
			UI16 ammoFXHue = mWeapon->GetAmmoFXHue();
			UI16 ammoFXRender = mWeapon->GetAmmoFXRender();

			if( mChar.IsNpc() || ( ammoID != 0 && DeleteItemAmount( &mChar, 1, ammoID,  ammoHue ) == 1 ))
			{
				PlaySwingAnimations( &mChar );
				Effects->PlayMovingAnimation( &mChar, ourTarg, ammoFX, 0x08, 0x00, 0x00, static_cast<UI32>( ammoFXHue ), static_cast<UI32>( ammoFXRender ));
			}
			else
			{
				if( mSock != nullptr )
					mSock->sysmessage( 309 );
				return;
			}
		}
		else
			PlaySwingAnimations( &mChar );

		SI16 staminaToLose = cwmWorldState->ServerData()->CombatAttackStamina();
		if( staminaToLose && ( !mChar.IsGM() && !mChar.IsCounselor() ) )
			mChar.IncStamina( staminaToLose );

		const UI16 getDefSkill		= ourTarg->GetSkill( TACTICS );
		bool skillPassed = false;

		// Do a skill check so the fight skill is increased
		Skills->CheckSkill( &mChar, getFightSkill, 0, std::min( 1000, (SI32)((getDefSkill * 1.25) + 100) ) );
		const R32 hitChance = ( ( ( (R32)attackSkill + 500.0 ) / ( ( (R32)defendSkill + 500.0 ) * 2.0) ) * 100.0 );
		skillPassed = ( RandomNum(0, 100) <= hitChance );

		if( !skillPassed )
		{
			if( getFightSkill == ARCHERY && mWeapon->GetAmmoID() != 0 && !RandomNum( 0, 2 ) )
			{
				UI16 ammoID = mWeapon->GetAmmoID();
				UI16 ammoHue = mWeapon->GetAmmoHue();
				Items->CreateItem( nullptr, ourTarg, ammoID, 1, ammoHue, OT_ITEM, false );
			}

			PlayMissedSoundEffect( &mChar );
		}
		else
		{
			CSocket *targSock = ourTarg->GetSocket();

			Skills->CheckSkill( &mChar, TACTICS, 0, 1000 );
			Skills->CheckSkill( ourTarg, TACTICS, 0, 1000 );

			switch( ourTarg->GetID() )
			{
				case 0x025E:	// elf/human/garg female
				case 0x0191:
				case 0x029B:	Effects->PlaySound( ourTarg, RandomNum( 0x014B, 0x014F ));				break;
				case 0x025D:	// elf/human/garg male
				case 0x0190:
				case 0x029A:	Effects->PlaySound( ourTarg, RandomNum( 0x0155, 0x0158 ));				break;
				default:
				{
					UI16 toPlay = cwmWorldState->creatures[ourTarg->GetID()].GetSound( SND_DEFEND );
					if( toPlay != 0x00 )
						Effects->PlaySound( ourTarg, toPlay );
					break;
				}
			}

			if( mChar.GetPoisonStrength() && ourTarg->GetPoisoned() < mChar.GetPoisonStrength() )
			{
				if( (( getFightSkill == FENCING || getFightSkill == SWORDSMANSHIP ) && !RandomNum( 0, 2 )) || mChar.IsNpc() )
				{
					ourTarg->SetPoisoned( mChar.GetPoisonStrength() );
					ourTarg->SetTimer( tCHAR_POISONTIME, BuildTimeValue(static_cast<R32> (40 / ourTarg->GetPoisoned() )) ); // a lev.1 poison takes effect after 40 secs, a deadly pois.(lev.4) takes 40/4 secs
					ourTarg->SetTimer( tCHAR_POISONWEAROFF, ourTarg->GetTimer( tCHAR_POISONTIME ) + ( 1000 * cwmWorldState->ServerData()->SystemTimer( tSERVER_POISON ) ) ); //wear off starts after poison takes effect
					if( targSock != nullptr )
						targSock->sysmessage( 282 );
				}
			}

			const UI08 hitLoc = CalculateHitLoc();
			UI16 ourDamage = calcDamage( &mChar, ourTarg, getFightSkill, hitLoc );
			if( ourDamage > 0 )
			{
				// Show hit messages, if enabled
				DoHitMessage( &mChar, ourTarg, hitLoc, ourDamage );

				// Interrupt Spellcasting
				if( !ourTarg->IsNpc() && targSock != nullptr )
				{
					if( ourTarg->IsCasting() && targSock->CurrentSpellType() == 0 )
					{
						ourTarg->StopSpell();
						ourTarg->SetFrozen( false );
						targSock->sysmessage( 306 );
					}
				}
				// Reactive Armor
				if( ourTarg->GetReactiveArmour() )
				{
					SI32 retDamage = (SI32)( ourDamage * ( ourTarg->GetSkill( MAGERY ) / 2000.0 ) );
					ourTarg->Damage( ourDamage - retDamage, &mChar );
					if( ourTarg->IsNpc() )
						retDamage *= cwmWorldState->ServerData()->CombatNPCDamageRate();
					mChar.Damage( retDamage, &mChar );
					Effects->PlayStaticAnimation( ourTarg, 0x374A, 0, 15 );
				}
				else
				{
					ourTarg->Damage( ourDamage, &mChar, true );
				}
			}
			if( cwmWorldState->creatures[mChar.GetID()].IsHuman() )
				PlayHitSoundEffect( &mChar, mWeapon );

			for( auto scriptTrig : scriptTriggers )
			{
				cScript *toExecute = JSMapping->GetScript( scriptTrig );
				if( toExecute != nullptr )
				{
					toExecute->OnAttack( &mChar, ourTarg );
				}
			}

			std::vector<UI16> defScriptTriggers = ourTarg->GetScriptTriggers();
			for( auto scriptTrig : defScriptTriggers )
			{
				cScript *toExecute = JSMapping->GetScript( scriptTrig );
				if( toExecute != nullptr )
				{
					toExecute->OnDefense( &mChar, ourTarg );
				}
			}
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	inline void QuickSwitch( CChar *mChar, CChar *ourTarg, SI08 spellNum )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Allows beneficial spells to be cast on self during combat
//o-----------------------------------------------------------------------------------------------o
inline void QuickSwitch( CChar *mChar, CChar *ourTarg, SI08 spellNum )
{
	if( !ValidateObject( mChar ) || !ValidateObject( ourTarg ) || mChar == ourTarg )
		return;

	mChar->SetSpellCast( spellNum );
	mChar->SetTarg( mChar );
	Magic->CastSpell( nullptr, mChar );
	mChar->StopSpell();
	mChar->SetTarg( ourTarg );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CastSpell( CChar *mChar, CChar *ourTarg, SI08 spellNum )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles spellcasting during combat
//o-----------------------------------------------------------------------------------------------o
bool CHandleCombat::CastSpell( CChar *mChar, CChar *ourTarg, SI08 spellNum )
{
	if( !ValidateObject( mChar ) || !ValidateObject( ourTarg ) || mChar == ourTarg )
		return false;

	if( mChar->GetMana() <= Magic->spells[spellNum].Mana() )
		return false;
	switch( spellNum )
	{
		case 7:
			if( !mChar->GetReactiveArmour() )
				mChar->SetSpellCast( spellNum );
			else
				return false;
			break;
		case 11:
			if( mChar->GetPoisoned() > 0 )
				QuickSwitch( mChar, ourTarg, spellNum );
			else
				return false;
			break;
		case 29:
			if( mChar->GetMaxHP() > mChar->GetHP() )
				QuickSwitch( mChar, ourTarg, spellNum );
			else
				return false;
			break;
		case 36:
			if( !mChar->IsPermReflected() )
				mChar->SetSpellCast( spellNum );
			else
				return false;
			break;
		case 1:
		case 3:
		case 4:
		case 5:
		case 12:
		case 18:
		case 20:
		case 27:
		case 30:
		case 31:
		case 33:
		case 37:
		case 38:
		case 41:
		case 42:
		case 43:
		case 46:
		case 49:
		case 51:
		case 53:
		case 55:
		case 57:
		case 58:
			mChar->SetSpellCast( spellNum );
			break;
	}
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void HandleNPCSpellAttack( CChar *npcAttack, CChar *cDefend, UI16 playerDistance )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles the casting of spells by NPCs during combat
//o-----------------------------------------------------------------------------------------------o
void CHandleCombat::HandleNPCSpellAttack( CChar *npcAttack, CChar *cDefend, UI16 playerDistance )
{
	if( !npcAttack->GetCanAttack() || npcAttack->IsEvading() )
		return;

	if( npcAttack->GetTimer( tNPC_SPATIMER ) <= cwmWorldState->GetUICurrentTime() )
	{
		if( playerDistance < cwmWorldState->ServerData()->CombatMaxSpellRange() )
		{
			SI16 spattacks = npcAttack->GetSpAttack();
			if( spattacks <= 0 )
				return;
			if( spattacks > 8 )
				spattacks = 8;

			npcAttack->StopSpell();
			switch( ( RandomNum( static_cast< SI16 >(0), spattacks ) + 1 ) )
			{
				case 1:
					switch( RandomNum( 1, 6 ) )
					{
						case 1:		CastSpell( npcAttack, cDefend, 1 );			break;	// Clumsy
						case 2:		CastSpell( npcAttack, cDefend, 3 );			break;	// Feeble Mind
						case 3:		CastSpell( npcAttack, cDefend, 4 );			break;	// Heal (personal...switch target temporarily to self and then back after casting)
						case 4:		CastSpell( npcAttack, cDefend, 5 );			break;	// Magic Arrow
						case 5:		CastSpell( npcAttack, cDefend, 7 );			break;	// Reactive Armor (personal...switch target temporarily to self and then back after casting)
						case 6:			// Check if poisoned and cast cure (switch to self)
							if( !CastSpell( npcAttack, cDefend, 11 ) )					// cure
								CastSpell( npcAttack, cDefend, 8 );						// weaken
							break;
					}
					break;
				case 2:			CastSpell( npcAttack, cDefend, 12 );		break;	// Harm
				case 3:
					switch( RandomNum( 1, 2 ) )
					{
						case 1:		CastSpell( npcAttack, cDefend, 18 );		break;	// Fireball
						case 2:		CastSpell( npcAttack, cDefend, 20 );		break;	// Poison
					}
					break;
				case 4:
					switch( RandomNum( 1, 4 ) )
					{
						case 1:		CastSpell( npcAttack, cDefend, 27 );		break;	// Curse
						case 2:		CastSpell( npcAttack, cDefend, 29 );		break;	// Greater Healing (personal...switch target temporarily to self and then back after casting)
						case 3:		CastSpell( npcAttack, cDefend, 30 );		break;	// Lightning
						case 4:		CastSpell( npcAttack, cDefend, 31 );		break;	// Mana Drain
					}
					break;
				case 5:
					switch( RandomNum( 1, 3 ) )
					{
						case 1:		CastSpell( npcAttack, cDefend, 36 );		break;	// Magic Reflection (personal...no target switching required)
						case 2:		CastSpell( npcAttack, cDefend, 37 );		break;	// Mind Blast
						case 3:		CastSpell( npcAttack, cDefend, 38 );		break;	// Paralyze
							//case 4:		CastSpell( npcAttack, cDefend, 33 );		break;	// Blade Spirits
					}
					break;
				case 6:	// If NPC is fighting summoned NPC, cast dispel...else cast other spells
					if( cDefend->IsNpc() )
					{
						if( cDefend->GetTimer( tNPC_SUMMONTIME ) > 0 && cDefend->GetNPCAiType() != AI_GUARD )
						{
							CastSpell( npcAttack, cDefend, 41 );
							break;
						}
					}
					switch( RandomNum( 1, 3 ) )
					{
						case 1:		CastSpell( npcAttack, cDefend, 42 );		break;	// Energy Bolt
						case 2:		CastSpell( npcAttack, cDefend, 43 );		break;	// Explosion
						case 3:		CastSpell( npcAttack, cDefend, 46 );		break;	// Mass Curse
					}
					break;
				case 7:
					switch( RandomNum( 1, 4 ) )
					{
						case 1:		CastSpell( npcAttack, cDefend, 49 );		break;	// Chain Lightning
						case 2:		CastSpell( npcAttack, cDefend, 51 );		break;	// Flamestrike
						case 3:		CastSpell( npcAttack, cDefend, 53 );		break;	// Mana Vampire
						case 4:		CastSpell( npcAttack, cDefend, 55 );		break;	// Meteor Swarm
					}
					break;
				case 8:
					switch( RandomNum( 1, 2 ) )
					{
						case 1:		CastSpell( npcAttack, cDefend, 57 );		break;	// Earthquake
						case 2:		break; //CastSpell( npcAttack, cDefend, 58 );		break;	// Energy Vortex
					}
					break;
					// This is where dragon attacks go eventually when the NPC DFNs are fixed...
					/*
					 case 9:
					 case 10:
					 case 11:
					 */
				default:
					break;
			}
			if( npcAttack->GetSpellCast() != -1 )
				Magic->CastSpell( nullptr, npcAttack );

			//Adjust spellDelay based on UOX.INI setting:
			SI08 spellDelay = floor( ( npcAttack->GetSpDelay() / cwmWorldState->ServerData()->NPCSpellCastSpeed() ) + 0.5 );

			npcAttack->SetTimer( tNPC_SPATIMER, BuildTimeValue( spellDelay ) );
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 GetCombatTimeout( CChar *mChar )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate delay between attacks in combat
//o-----------------------------------------------------------------------------------------------o
R32 CHandleCombat::GetCombatTimeout( CChar *mChar )
{
	SI16 statOffset = 0;
	if( cwmWorldState->ServerData()->CombatAttackSpeedFromStamina() )
		statOffset = mChar->GetStamina();
	else
		statOffset = mChar->GetDexterity();

	R32 getDelay = 0;
	if( mChar->IsNpc() ) // Allow NPCs more speed based off of available stamina than players
		getDelay	= (R32)( (R32)std::min( statOffset, static_cast<SI16>(300) ) + 100 );
	else
		getDelay	= (R32)( (R32)std::min( statOffset, static_cast<SI16>(100) ) + 100 );

	SI32 getOffset	= 0;
	SI32 baseValue	= 15000;

	CChar *ourTarg = mChar->GetTarg();

	CItem *mWeapon = getWeapon( mChar );
	if( ValidateObject( mWeapon ) )
	{
		if( mWeapon->GetSpeed() == 0 )
			mWeapon->SetSpeed( 35 );
		getOffset = mWeapon->GetSpeed();
	}
	else
	{
		if( mChar->GetSkill( WRESTLING ) < 800 )
			getOffset = (((SI32)(mChar->GetSkill( WRESTLING ) / 200 )) * 5) + 30;
		else
			getOffset = 50;
	}

	//Allow faster strikes on fleeing targets
	if( ValidateObject(ourTarg) )
		if( ourTarg->GetNpcWander() == WT_FLEE)
			baseValue = 10000;

	R32 globalAttackSpeed = cwmWorldState->ServerData()->GlobalAttackSpeed(); //Defaults to 1.0

	getDelay = (baseValue / (getDelay * getOffset)) / globalAttackSpeed;
	return getDelay;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void InvalidateAttacker( CChar *mChar )
//|	Date		-	3rd July, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Resets the attacker attack so that it cancels attack setup.
//o-----------------------------------------------------------------------------------------------o
void CHandleCombat::InvalidateAttacker( CChar *mChar )
{
	CChar *ourTarg = mChar->GetTarg();

	// Check if OnCombatEnd event exists.
	std::vector<UI16> scriptTriggers = mChar->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			//Check if ourTarg validates as another character. If not, don't use
			if( !ValidateObject( ourTarg ))
				ourTarg = nullptr;

			// -1 == event doesn't exist, or returned -1
			// 0 == script returned false, 0, or nothing - don't execute hard code
			// 1 == script returned true or 1
			if( toExecute->OnCombatEnd( mChar, ourTarg ) == 0 )	// if it exists and we don't want hard code, return
			{
				return;
			}
		}
	}

	if( mChar->IsNpc() && mChar->GetNPCAiType() == AI_GUARD )
	{
		mChar->SetTimer( tNPC_SUMMONTIME, BuildTimeValue( 20 ) );
		mChar->SetNpcWander( WT_FREE );
		if( mChar->GetMounted() )
			mChar->SetTimer( tNPC_MOVETIME, BuildTimeValue( mChar->GetMountedWalkingSpeed() ) );
		else
			mChar->SetTimer( tNPC_MOVETIME, BuildTimeValue( mChar->GetWalkingSpeed() ) );
		mChar->TextMessage( nullptr, 281, TALK, false );
	}

	if( ValidateObject( ourTarg ) && ourTarg->GetTarg() == mChar )
	{
		ourTarg->SetTarg( nullptr );
		ourTarg->SetAttacker( nullptr );
		ourTarg->SetAttackFirst( false );
		if( ourTarg->IsAtWar() )
			ourTarg->ToggleCombat();
	}
	mChar->SetTarg( nullptr );
	CChar *attAttacker = mChar->GetAttacker();
	if( ValidateObject( attAttacker ) )
	{
		attAttacker->SetAttackFirst( false );
		attAttacker->SetAttacker( nullptr );
	}
	mChar->SetAttacker( nullptr );
	mChar->SetAttackFirst( false );
	if( mChar->IsAtWar() && mChar->IsNpc() )
		mChar->ToggleCombat();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void Kill( CChar *mChar, CChar *ourTarg )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle death during combat
//o-----------------------------------------------------------------------------------------------o
void CHandleCombat::Kill( CChar *mChar, CChar *ourTarg )
{
	if( ValidateObject( mChar ) )
	{
		if( mChar->GetNPCAiType() == AI_GUARD && ourTarg->IsNpc() )
		{
			Effects->PlayCharacterAnimation( ourTarg, ( RandomNum( 0, 1 ) ? ACT_DIE_BACKWARD : ACT_DIE_FORWARD )); // 0x15 or 0x16
			Effects->playDeathSound( ourTarg );

			ourTarg->Delete(); // NPC was killed by a Guard, don't leave a corpse behind
			if( mChar->IsAtWar() )
				mChar->ToggleCombat();
			return;
		}

		if( mChar->GetNPCAiType() == AI_ANIMAL && !mChar->IsTamed() )
		{
			mChar->SetHunger( 6 );

			if( ourTarg->IsNpc() )
			{
				Effects->PlayCharacterAnimation( ourTarg, 0x15 );
				Effects->playDeathSound( ourTarg );

				ourTarg->Delete(); // eating animals, don't give body
				if( mChar->IsAtWar() )
					mChar->ToggleCombat();
				return;
			}
		}
		InvalidateAttacker( mChar );
	}
	HandleDeath( ourTarg, mChar );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CombatLoop( CSocket *mSock, CChar& mChar )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles the main combat loop for characters
//o-----------------------------------------------------------------------------------------------o
void CHandleCombat::CombatLoop( CSocket *mSock, CChar& mChar )
{
	CChar *ourTarg = mChar.GetTarg();
	if( ourTarg == nullptr )
		return;

	if( mChar.IsNpc() && mChar.GetTimer( tNPC_EVADETIME ) > cwmWorldState->GetUICurrentTime() )
	{
#ifdef DEBUG_COMBAT
		Console.warning( "DEBUG: Exited CombatLoop for NPC due to EvadeTimer.\n" );
#endif
		return;
	}

	if( mChar.GetTimer( tCHAR_TIMEOUT ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
	{
		bool validTarg = false;
		if( !mChar.IsDead() && ValidateObject( ourTarg ) && !ourTarg->isFree() && ( ourTarg->IsNpc() || isOnline( (*ourTarg) ) ) )
		{
			if( !ourTarg->IsInvulnerable() && !ourTarg->IsDead() && ourTarg->GetNPCAiType() != AI_PLAYERVENDOR && ourTarg->GetVisible() == VT_VISIBLE )
			{
				if( charInRange( &mChar, ourTarg ) )
				{
					CItem *equippedWeapon = getWeapon( &mChar );
					if( getCombatSkill( equippedWeapon ) == ARCHERY || getCombatSkill( equippedWeapon ) == THROWING )
					{
						if( getDist( &mChar, ourTarg ) > equippedWeapon->GetMaxRange() )
						{
							return;
						}
					}

					validTarg = true;
					if( mChar.IsNpc() && mChar.GetSpAttack() > 0 && mChar.GetMana() > 0 && !RandomNum( 0, 2 ) )
						HandleNPCSpellAttack( &mChar, ourTarg, getDist( &mChar, ourTarg ) );
					else
						HandleCombat( mSock, mChar, ourTarg );

					if( !ValidateObject( ourTarg->GetTarg() ) || !objInRange( ourTarg, ourTarg->GetTarg(), DIST_INRANGE ) )		//if the defender is swung at, and they don't have a target already, set this as their target
						StartAttack( ourTarg, &mChar );
				}
				else if( mChar.IsNpc() && mChar.GetNPCAiType() == AI_GUARD && mChar.GetRegion()->IsGuarded() && cwmWorldState->ServerData()->GuardsStatus() )
				{
					validTarg = true;
					mChar.SetLocation( ourTarg );
					Effects->PlaySound( &mChar, 0x01FE );
					Effects->PlayStaticAnimation( &mChar, 0x372A, 0x09, 0x06 );
					mChar.TextMessage( nullptr, 1616, TALK, true );
				}
				else
					InvalidateAttacker( &mChar );
			}
		}
		if( !validTarg )
		{
			mChar.SetTarg( nullptr );
			mChar.SetAttacker( nullptr );
			mChar.SetAttackFirst( false );
			if( mChar.IsAtWar() )
				mChar.ToggleCombat();
		}
		else
			mChar.SetTimer( tCHAR_TIMEOUT, BuildTimeValue( GetCombatTimeout( &mChar ) ) );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SpawnGuard( CChar *mChar, CChar *targChar, SI16 x, SI16 y, SI08 z )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle spawning a guard in guarded areas
//|					NEED TO REWORK FOR REGIONAL GUARD STUFF
//o-----------------------------------------------------------------------------------------------o
void CHandleCombat::SpawnGuard( CChar *mChar, CChar *targChar, SI16 x, SI16 y, SI08 z )
{
	if( !ValidateObject( mChar ) || !ValidateObject( targChar ) )
		return;

	if( targChar->IsDead() || targChar->IsInvulnerable() )
		return;

	CTownRegion *targRegion = mChar->GetRegion();

	if( !targRegion->IsGuarded() || !cwmWorldState->ServerData()->GuardsStatus() )
		return;

	bool reUseGuard		= false;
	CChar *getGuard		= nullptr;
	CMapRegion *toCheck	= MapRegion->GetMapRegion( mChar );

	if( toCheck != nullptr )
	{
		GenericList< CChar * > *regChars = toCheck->GetCharList();
		regChars->Push();
		for( getGuard = regChars->First(); !regChars->Finished(); getGuard = regChars->Next() )
		{
			if( !ValidateObject( getGuard ) )
				continue;

			if( !getGuard->IsNpc() || getGuard->GetNPCAiType() != AI_GUARD )
				continue;

			if( !ValidateObject( getGuard->GetTarg() ) || getGuard->GetTarg() == targChar  )
			{
				if( charInRange( getGuard, targChar ) )
					reUseGuard = true;
			}
			else if( getGuard->GetTarg() == targChar )
			{
				regChars->Pop();
				return;
			}
		}
		regChars->Pop();
	}
	// 1/13/2003 - Fix for JSE NocSpawner
	if( !reUseGuard )
	{
		getGuard = targRegion->GetRandomGuard();
		if( ValidateObject( getGuard ) )
		{
			getGuard->SetLocation( x, y, z, mChar->WorldNumber(), mChar->GetInstanceID() );
			Npcs->PostSpawnUpdate( getGuard );
		}
		else
			return;
	}
	//
	if( ValidateObject( getGuard ) )
	{
		getGuard->SetAttackFirst( true );
		getGuard->SetAttacker( targChar );
		getGuard->SetTarg( targChar );
		getGuard->SetNpcWander( WT_FREE );
		if( !getGuard->IsAtWar() )
			getGuard->ToggleCombat();

		if( reUseGuard )
			getGuard->SetLocation( targChar );
		else
		{
			if( getGuard->GetMounted() )
				getGuard->SetTimer( tNPC_MOVETIME, BuildTimeValue( getGuard->GetMountedWalkingSpeed() ) );
			else
				getGuard->SetTimer( tNPC_MOVETIME, BuildTimeValue( getGuard->GetWalkingSpeed() ) );
			getGuard->SetTimer( tNPC_SUMMONTIME, BuildTimeValue( 25 ) );

			Effects->PlaySound( getGuard, 0x01FE );
			Effects->PlayStaticAnimation( getGuard, 0x372A, 0x09, 0x06 );

			getGuard->TextMessage( nullptr, 313, TALK, true );
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void petGuardAttack( CChar *mChar, CChar *owner, CBaseObject *guarded )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Get the pet guarding an item / character and have him attack
//|					the person using / attacking the item / character
//o-----------------------------------------------------------------------------------------------o
void CHandleCombat::petGuardAttack( CChar *mChar, CChar *owner, CBaseObject *guarded )
{
	if( !ValidateObject( mChar ) || !ValidateObject( owner ) || !ValidateObject( guarded ) )
		return;

	if( mChar->GetSerial() == owner->GetSerial() || mChar->IsCounselor() || mChar->IsGM() )
		return;

	CChar *petGuard = Npcs->getGuardingPet( owner, guarded );
	if( ValidateObject( petGuard ) && objInRange( mChar, petGuard, cwmWorldState->ServerData()->CombatMaxRange() ) )
	{
		if( !Npcs->checkPetFriend( mChar, petGuard ) )
			AttackTarget( petGuard, mChar );
		else
		{
			CSocket *oSock = owner->GetSocket();
			if( oSock != nullptr )
				oSock->sysmessage( 1629 );
		}
	}
}
