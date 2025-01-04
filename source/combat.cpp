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

#define SWINGAT static_cast<UI32>( 1.75 ) * 1000

const UI08 BODYPERCENT = 0;
//const UI08 ARMSPERCENT = 1;
//const UI08 HEADPERCENT = 2;
//const UI08 LEGSPERCENT = 3;
//const UI08 NECKPERCENT = 4;
//const UI08 OTHERPERCENT = 5;
const UI08 TOTALTARGETSPOTS = 6; // Wonder if that should be one?

const UI08 LOCPERCENTAGES[TOTALTARGETSPOTS] = { 44, 14, 14, 14, 7, 7 };

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::StartAttack()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle characters initiating combat with other characters
//o------------------------------------------------------------------------------------------------o
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

	if( cTarget->WorldNumber() != cAttack->WorldNumber() || cTarget->GetInstanceId() != cAttack->GetInstanceId() )
		return false;

	if( !ObjInRange( cAttack, cTarget, DIST_NEXTTILE ) && !LineOfSight( nullptr, cAttack, cTarget->GetX(), cTarget->GetY(), ( cTarget->GetZ() + 15 ), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false ))
		return false;

	if( !cAttack->GetCanAttack() || cAttack->IsEvading() ) // Is the char allowed to attack?
		return false;

	if( cAttack->GetNpcAiType() == AI_DUMMY ) // If passive, don't allow attack
		return false;

	// If two NPCs try to fight, do some extra checks to make sure they're allowed to (ignore if both are pets)
	if( cAttack->IsNpc() && cTarget->IsNpc() && ( cAttack->GetOwner() == INVALIDSERIAL && cTarget->GetOwner() == INVALIDSERIAL ))
	{
		// Are both the NPCs non-human?
		if( !cwmWorldState->creatures[cAttack->GetId()].IsHuman() && !cwmWorldState->creatures[cTarget->GetId()].IsHuman() )
		{
			if( cAttack->GetNpcAiType() == AI_ANIMAL && cTarget->GetNpcAiType() == AI_ANIMAL )
			{
				// Don't allow combat if both attacker and target have AI_ANIMAL, same body type and belong to same race
				if( cAttack->GetId() == cTarget->GetId() && cAttack->GetRace() == cTarget->GetRace() )
				{
					return false;
				}
			}
			else if( cAttack->GetRace() == cTarget->GetRace() )
			{
				// Otherwise, don't allow combat if both attacker and target have same race
				return false;
			}
		}
		else
		{
			// Don't allow human NPC to human NPC combat if they both have same NPC Flag (evil, innocent, neutral)
			if( cAttack->GetNPCFlag() == cTarget->GetNPCFlag() )
				return false;
		}
	}

	// Check if combat is allowed in attacker AND target's regions, but allow it if it's a guard
	if( cAttack->GetNpcAiType() != AI_GUARD && ( cAttack->GetRegion()->IsSafeZone() || cTarget->GetRegion()->IsSafeZone() ))
	{
		// Either attacker or target is in a safe zone where all aggressive actions are forbidden, disallow
		CSocket *cSocket = cAttack->GetSocket();
		if( cSocket )
		{
			cSocket->SysMessage( 1799 ); // Hostile actions are not permitted in this safe area.
		}
		return false;
	}

	bool returningAttack = false;

	cAttack->SetTarg( cTarget );
	cAttack->SetAttacker( cTarget );

	// If cTarget is not already an aggressor to cAttack...
	auto cAttackOwner = cAttack->GetOwnerObj();
	if( ValidateObject( cAttackOwner ) )
	{
		if( !cTarget->CheckAggressorFlag( cAttackOwner->GetSerial() ) )
		{
			// Add both owner and pet/hireling/summon as aggressor of cTarget!
			cAttackOwner->AddAggressorFlag( cTarget->GetSerial() );
			cAttack->AddAggressorFlag( cTarget->GetSerial() );
			cAttackOwner->Dirty( UT_UPDATE );
			cAttack->Dirty( UT_UPDATE );
		}
	}
	else
	{
		if( !cTarget->CheckAggressorFlag( cAttack->GetSerial() ))
		{
			// ...add cAttack as aggressor of cTarget!
			cAttack->AddAggressorFlag( cTarget->GetSerial() );
			cAttack->Dirty( UT_UPDATE );
		}
	}

	// If target doesn't already have cAttack as a target, update their target selection
	//if( !cTarget->IsInvulnerable() && ( !ValidateObject( cTarget->GetTarg() ) || !ObjInRange( cTarget, cTarget->GetTarg(), DIST_INRANGE )))	// Only invuln don't fight back
	if( !cTarget->IsInvulnerable() && !ValidateObject( cTarget->GetTarg() ))	// Only invuln don't fight back
	{
		if( cTarget->GetNpcAiType() != AI_DUMMY )
		{
			cTarget->SetTarg( cAttack );
			cTarget->SetAttacker( cAttack );
			returningAttack = true;

			// If target is not already at war/in combat, make sure to disable their passive mode
			if( !cTarget->IsAtWar() )
			{
				if( cTarget->IsNpc() )
				{
					cTarget->ToggleCombat();
				}
				cTarget->SetPassive( false );
			}
		}

		if( cTarget->GetSocket() != nullptr )
		{
			CPAttackOK tSend = (*cAttack);
			cTarget->GetSocket()->Send( &tSend );
		}
	}

	if( WillResultInCriminal( cAttack, cTarget )) //REPSYS
	{
		MakeCriminal( cAttack );
		bool regionGuarded = ( cTarget->GetRegion()->IsGuarded() );
		if( cwmWorldState->ServerData()->GuardsStatus() && regionGuarded && cTarget->IsNpc() && cTarget->GetNpcAiType() != AI_GUARD && cwmWorldState->creatures[cTarget->GetId()].IsHuman() )
		{
			cTarget->TextMessage( nullptr, 335, TALK, true ); // Help! Guards! I've been attacked!
			CallGuards( cTarget, cAttack );
		}
	}

	if( cAttack->GetVisible() == VT_TEMPHIDDEN || cAttack->GetVisible() == VT_INVISIBLE )
	{
		cAttack->ExposeToView();
	}

	if( !cAttack->IsNpc() )
	{
		cAttack->BreakConcentration( cAttack->GetSocket() );
	}
	else
	{
		UI16 toPlay = cwmWorldState->creatures[cAttack->GetId()].GetSound( SND_STARTATTACK );
		if( toPlay != 0x00 && RandomNum( 1, 3 )) // 33% chance to play the sound
		{
			Effects->PlaySound( cAttack, toPlay );
		}

		// if the source is an npc, make sure they're in war mode and reset their movement time
		if( !cAttack->IsAtWar() )
		{
			cAttack->ToggleCombat();
		}
		if( cAttack->GetMounted() )
		{
			cAttack->SetTimer( tNPC_MOVETIME, BuildTimeValue( cAttack->GetMountedWalkingSpeed() ));
		}
		else
		{
			cAttack->SetTimer( tNPC_MOVETIME, BuildTimeValue( cAttack->GetWalkingSpeed() ));
		}
	}

	// Only unhide the defender, if they're going to return the attack (otherwise they're doing nothing!)
	if( returningAttack )
	{
		if( cTarget->GetVisible() == VT_TEMPHIDDEN || cAttack->GetVisible() == VT_INVISIBLE )
		{
			cTarget->ExposeToView();
		}

		if( !cTarget->IsNpc() )
		{
			cTarget->BreakConcentration( cTarget->GetSocket() );
		}

		// if the target is an npc, and not a guard, make sure they're in war mode and update their movement time
		// ONLY IF THEY'VE CHANGED ATTACKER
		if( cTarget->IsNpc() && cTarget->GetNpcAiType() != AI_GUARD )
		{
			if( !cTarget->IsAtWar() )
			{
				cTarget->ToggleCombat();
			}
			if( cTarget->GetMounted() )
			{
				cTarget->SetTimer( tNPC_MOVETIME, BuildTimeValue( cTarget->GetMountedWalkingSpeed() ));
			}
			else
			{
				cTarget->SetTimer( tNPC_MOVETIME, BuildTimeValue( cTarget->GetWalkingSpeed() ));
			}
		}
	}
	return true;
}

void CallGuards( CChar *mChar, CChar *targChar );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::PlayerAttack()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle player attacking (Double-clicking whilst in war mode)
//o------------------------------------------------------------------------------------------------o
void CHandleCombat::PlayerAttack( CSocket *s )
{
	if( s == nullptr )
		return;

	CChar *ourChar = s->CurrcharObj();
	if( !ValidateObject( ourChar ))
		return;

	// Don't allow attacking other characters if invulnerable
	if( ourChar->IsInvulnerable() )
	{
		s->SysMessage( 1973 ); // You cannot engage in combat while invulnerable!
		return;
	}

	SERIAL serial = s->GetDWord( 1 );
	if( serial == INVALIDSERIAL )
	{
		ourChar->SetTarg( nullptr );
		return;
	}
	CChar *i = CalcCharObjFromSer( serial );
	if( !ValidateObject( i ))
	{
		ourChar->SetTarg( nullptr );
		return;
	}
	if( ourChar->IsDead() )
	{
		if( i->IsNpc() ) // if target is an NPC
		{
			switch( i->GetNpcAiType() )
			{
				case AI_HEALER_G: // Good Healer
					if( ourChar->IsCriminal() || ourChar->IsMurderer() )
					{
						i->TextMessage( nullptr, 322, TALK, true ); // I will not give life to a scoundrel like thee!
					}
					else if( !ObjInRange( i, ourChar, DIST_NEARBY ))
					{
						i->TextMessage( nullptr, 321, TALK, true ); // Come nearer, ghost, and I'll give you life!
					}
					else
					{
						// Proceed with resurrection
						CMultiObj *multiObj = ourChar->GetMultiObj();
						if( !ValidateObject( multiObj ) || multiObj->GetOwner() == ourChar->GetSerial() )
						{
							if( LineOfSight( s, ourChar, i->GetX(), i->GetY(), ( i->GetZ() + 15 ), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false ))
							{
								// Play Resurrect casting animation
								if( i->GetBodyType() == BT_GARGOYLE	|| cwmWorldState->ServerData()->ForceNewAnimationPacket() )
								{
									Effects->PlayNewCharacterAnimation( i, N_ACT_SPELL, S_ACT_SPELL_TARGET ); // Action 0x0b, subAction 0x00
								}
								else
								{
									UI16 castAnim = static_cast<UI16>( cwmWorldState->creatures[i->GetId()].CastAnimTargetId() );
									UI08 castAnimLength = cwmWorldState->creatures[i->GetId()].CastAnimTargetLength();

									// Play cast anim, but fallback to default attack anim (0x04) with anim length of 4 frames if no cast anim was defined in creatures.dfn
									Effects->PlayCharacterAnimation( i, ( castAnim != 0 ? castAnim : 0x04 ), 0, ( castAnimLength != 0 ? castAnimLength : 4 ));
								}

								NpcResurrectTarget( ourChar );
								Effects->PlayStaticAnimation( ourChar, 0x376A, 0x09, 0x06 );
								i->TextMessage( nullptr, ( 316 + RandomNum( 0, 4 )), TALK, false ); // Random resurrection speak from NPC healer
							}
						}
					}
					break;
				case AI_HEALER_E: // Evil Healer
					if( !ourChar->IsMurderer() )
					{
						i->TextMessage( nullptr, 329, TALK, true ); // I despise all things good. I shall not give thee another chance!
					}
					else if( !ObjInRange( i, ourChar, DIST_NEARBY ))	
					{
						i->TextMessage( nullptr, 328, TALK, true ); // Come nearer, evil soul, and I'll give you life!
					}
					else
					{
						// Proceed with resurrection
						CMultiObj *multiObj = ourChar->GetMultiObj();
						if( !ValidateObject( multiObj ) || multiObj->GetOwner() == ourChar->GetSerial() )
						{
							if( LineOfSight( s, ourChar, i->GetX(), i->GetY(), ( i->GetZ() + 15 ), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false ))
							{
								// Play Resurrect casting animation
								if( i->GetBodyType() == BT_GARGOYLE	|| cwmWorldState->ServerData()->ForceNewAnimationPacket() )
								{
									Effects->PlayNewCharacterAnimation( i, N_ACT_SPELL, S_ACT_SPELL_TARGET ); // Action 0x0b, subAction 0x00
								}
								else
								{
									UI16 castAnim = static_cast<UI16>( cwmWorldState->creatures[i->GetId()].CastAnimTargetId() );
									UI08 castAnimLength = cwmWorldState->creatures[i->GetId()].CastAnimTargetLength();

									// Play cast anim, but fallback to default attack anim (0x04) with anim length of 4 frames if no cast anim was defined in creatures.dfn
									Effects->PlayCharacterAnimation( i, ( castAnim != 0 ? castAnim : 0x04 ), 0, ( castAnimLength != 0 ? castAnimLength : 4 ));
								}

								NpcResurrectTarget( ourChar );
								Effects->PlayStaticAnimation( ourChar, 0x3709, 0x09, 0x19 ); //Flamestrike effect
								i->TextMessage( nullptr, ( 323 + RandomNum( 0, 4 )), TALK, false ); // Random resurrection speak from evil NPC healer
							}
						}
					}
					break;
				default:
					s->SysMessage( 330 ); // You are dead and cannot do that.
					break;
			}
			return;
		}
		else // if target is a player
		{
			if( cwmWorldState->ServerData()->PlayerPersecutionStatus() )
			{
				ourChar->SetTarg( i );
				Skills->Persecute( s );
				return;
			}
			else
			{
				s->SysMessage( 330 ); // You are dead and cannot do that.
				return;
			}
		}
	}
	else
	{
		if( ourChar->GetTarg() != i ) // if player is alive
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
		}

		if( i->GetTarg() != ourChar )
		{
			// Also trigger same event for other party in combat, if they're not already engaged in combat with this character
			std::vector<UI16> scriptTriggers = i->GetScriptTriggers();
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

		}

		if( !ourChar->GetCanAttack() ) // Is our char allowed to attack
		{
			s->SysMessage( 1778 ); // You are currently under the effect of peace and can not attack!
			return;
		}

		// Check if combat is allowed in attacker AND target's regions
		if( ourChar->GetRegion()->IsSafeZone() || i->GetRegion()->IsSafeZone() )
		{
			// Either attacker or target is in a safe zone where all aggressive actions are forbidden, disallow
			s->SysMessage( 1799 ); // You are no longer affected by peace!
			return;
		}

		ourChar->SetTarg( i );
		if( ourChar->GetVisible() == VT_TEMPHIDDEN || ourChar->GetVisible() == VT_INVISIBLE )
		{
			ourChar->ExposeToView();
		}
		if( i->IsDead() || i->GetHP() <= 0 )
		{
			s->SysMessage( 331 ); // That person is already dead!
			return;
		}
		if( i->GetNpcAiType() == AI_PLAYERVENDOR ) // PlayerVendors
		{
			s->SysMessage( 332, i->GetName().c_str() ); // %s cannot be harmed.
			return;
		}
		if( i->IsInvulnerable() )
		{
			s->SysMessage( 333 ); // You cannot fight the invincible.
			return;
		}
		if( i->IsEvading() )
		{
			s->SysMessage( 1792 ); // The targeted NPC is in a state of evasion and cannot be attacked.
			return;
		}

		// Deal with players attacking NPCs they're escorting!
		if( i->IsNpc() && i->GetQuestType() == 0xFF && i->GetOwnerObj() == ourChar )
		{
			i->SetNpcWander( WT_FREE );		// Wander freely
			i->SetFTarg( nullptr );			// Reset follow target
			i->SetQuestType( 0 );			// Reset quest type
			i->SetQuestDestRegion( 0 );		// Reset quest destination region
			// Set a timer to automatically delete the NPC
			i->SetTimer( tNPC_SUMMONTIME, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_ESCORTDONE ));
			i->SetOwner( nullptr );
			i->TextMessage( nullptr, 1797, TALK, false ); // Woe is me! My escort has betrayed me!
		}

		// flag player FIRST so that anything attacking them as a result of this is not flagged.
		if( WillResultInCriminal( ourChar, i )) // REPSYS
		{
			MakeCriminal( ourChar );
			bool regionGuarded = ( i->GetRegion()->IsGuarded() );
			if( cwmWorldState->ServerData()->GuardsStatus() && regionGuarded && i->IsNpc() && i->GetNpcAiType() != AI_GUARD && cwmWorldState->creatures[i->GetId()].IsHuman() )
			{
				i->TextMessage( nullptr, 335, TALK, true ); // Help! Guards! I've been attacked!
				CallGuards( i, ourChar );
			}
		}

		// If target is guarded by a pet, cause pet to attack player
		if( i->IsGuarded() )
		{
			PetGuardAttack( ourChar, i, i );
		}

		// Send attacker message to all nearby players, IF player is attacking someone who wasn't already fighting them
		if( i->GetTarg() != ourChar )
		{
			for( auto &tSock : FindNearbyPlayers( ourChar ))
			{
				if( tSock && tSock != ourChar->GetSocket() )
				{
					// Valid socket found
					CChar *witness = tSock->CurrcharObj();
					if( ValidateObject( witness ))
					{
						// Fetch names of attacker and target
						std::string attackerName = GetNpcDictName( ourChar, tSock, NRS_SPEECH );
						std::string targetName = GetNpcDictName( i, tSock, NRS_SPEECH );

						// Send an emote about attacking target to nearby witness
						ourChar->TextMessage( tSock, 334, EMOTE, 0, attackerName.c_str(), targetName.c_str() ); // You see %s attacking %s!
					}
				}
			}
		}

		if( !i->IsNpc() )
		{
			// Send a message about being attacked to the target, if they're a player
			CSocket *iSock = i->GetSocket();
			if( iSock != nullptr )
			{
				std::string attackerName = GetNpcDictName( ourChar, iSock, NRS_SPEECH );
				i->TextMessage( iSock, 1281, EMOTE, 1, attackerName.c_str() ); // %s is attacking you!
			}
		}

		// Keep the target highlighted
		CPAttackOK toSend = (*i);
		s->Send( &toSend );

		if( i->GetNpcAiType() != AI_GUARD && !ValidateObject( i->GetTarg() ))
		{
			// Set ourChar as attacker of target, if target has no target? Should this check if it has an Attacker instead?
			i->SetAttacker( ourChar );
		}

		// Add ourChar as an aggressor to the target, if target is not already an aggressor to ourChar!
		if( !i->CheckAggressorFlag( ourChar->GetSerial() ))
		{
			if( !ourChar->CheckAggressorFlag( i->GetSerial() ))
			{
				ourChar->AddAggressorFlag( i->GetSerial() );
			}
			else
			{
				ourChar->UpdateAggressorFlagTimestamp( i->GetSerial() );
			}
			ourChar->Dirty( UT_UPDATE );
		}

		ourChar->SetAttacker( i );
		AttackTarget( i, ourChar );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::AttackTarget()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle the attacking action in combat
//o------------------------------------------------------------------------------------------------o
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

	if( !StartAttack( cAttack, cTarget )) // Is the char allowed to initiate combat with the target?
		return;

	if( cAttack->CheckAggressorFlag( cTarget->GetSerial() ))
	{
		// Send attacker message to all nearby players
		for( auto &tSock : FindNearbyPlayers( cAttack ))
		{
			if( tSock )
			{
				// Valid socket found
				CChar *witness = tSock->CurrcharObj();
				if( ValidateObject( witness ))
				{
					// Fetch names of attacker and target
					std::string attackerName = GetNpcDictName( cAttack, tSock, NRS_SPEECH );
					std::string targetName = GetNpcDictName( cTarget, tSock, NRS_SPEECH );

					// Send an emote about attacking target to nearby witness
					cAttack->TextMessage( tSock, 334, EMOTE, 0, attackerName.c_str(), targetName.c_str() ); // You see %s attacking target
				}
			}
		}

		if( !cTarget->IsNpc() )
		{
			CSocket *iSock = cTarget->GetSocket();
			if( iSock != nullptr )
			{
				std::string attackerName = GetNpcDictName( cAttack, iSock, NRS_SPEECH );
				cTarget->TextMessage( iSock, 1281, EMOTE, 1, attackerName.c_str() ); // %s is attacking you!
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::GetWeapon()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Find what weapon (if any) character is holding
//o------------------------------------------------------------------------------------------------o
CItem * CHandleCombat::GetWeapon( CChar *i )
{
	if( !ValidateObject( i ))
		return nullptr;

	CItem *j = i->GetItemAtLayer( IL_RIGHTHAND );
	if( ValidateObject( j ))
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

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::GetShield()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check players hands for a shield
//o------------------------------------------------------------------------------------------------o
CItem * CHandleCombat::GetShield( CChar *i )
{
	if( ValidateObject( i ))
	{
		CItem *shield = i->GetItemAtLayer( IL_LEFTHAND );
		if( ValidateObject( shield ) && shield->IsShieldType() )
			return shield;
	}
	return nullptr;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::GetWeaponType()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Find what weapon type a character is holding (based on its ID)
//o------------------------------------------------------------------------------------------------o
UI08 CHandleCombat::GetWeaponType( CItem *i )
{
	if( !ValidateObject( i ))
		return WRESTLING;

	switch( i->GetId() )
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

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::GetBowType()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Find if character is holding a Bow or XBow
//o------------------------------------------------------------------------------------------------o
UI08 CHandleCombat::GetBowType( CItem *bItem )
{
	if( !ValidateObject( bItem ))
		return 0;

	switch( GetWeaponType( bItem ))
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

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::GetCombatSkill()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets your combat skill based on the weapon in your hand (if any)
//o------------------------------------------------------------------------------------------------o
UI08 CHandleCombat::GetCombatSkill( CItem *wItem )
{
	if( !ValidateObject( wItem ))
		return WRESTLING;

	switch( GetWeaponType( wItem ))
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

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::CalcAttackPower()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate total attack power and do damage to the weapon
//o------------------------------------------------------------------------------------------------o
SI16 CHandleCombat::CalcAttackPower( CChar *p, bool doDamage )
{
	if( !ValidateObject( p ))
		return 0;

	SI16 getDamage = 0;

	CItem *weapon = GetWeapon( p );
	if( ValidateObject( weapon ))
	{
		if( weapon->GetLoDamage() > 0 && weapon->GetHiDamage() > 0 )
		{
			// Calculate random damage dealt by weapon based on lo/hi damage from dfns
			if( weapon->GetLoDamage() >= weapon->GetHiDamage() )
			{
				getDamage += weapon->GetLoDamage();
			}
			else
			{
				getDamage += RandomNum( weapon->GetLoDamage(), weapon->GetHiDamage() );
			}

			// Chance to apply damage to (player's) weapon based on ini setting
			if( doDamage && !p->IsNpc() && ( cwmWorldState->ServerData()->CombatWeaponDamageChance() >= RandomNum( 1, 100 )))
			{
				SI08 weaponDamage = 0;
				UI08 weaponDamageMin = 0;
				UI08 weaponDamageMax = 0;

				// Fetch minimum and maximum weapon damage from ini
				weaponDamageMin = cwmWorldState->ServerData()->CombatWeaponDamageMin();
				weaponDamageMax = cwmWorldState->ServerData()->CombatWeaponDamageMax();

				weaponDamage -= static_cast<UI08>( RandomNum( static_cast<UI16>( weaponDamageMin ), static_cast<UI16>( weaponDamageMax )));
				weapon->IncHP( weaponDamage );

				// If weapon hp has reached 0, destroy it
				if( weapon->GetHP() <= 0 )
				{
					CSocket *mSock = p->GetSocket();
					if( mSock != nullptr )
					{
						std::string name;
						GetTileName(( *weapon ), name );
						mSock->SysMessage( 311, name.c_str() ); // Your %s has been destroyed.
					}
					weapon->Delete();
				}
			}
		}
	}
	else if( p->IsNpc() )
	{
		if( p->GetLoDamage() >= p->GetHiDamage() )
		{
			getDamage = p->GetLoDamage();
		}
		else if( p->GetHiDamage() > 2 )
		{
			getDamage = RandomNum( p->GetLoDamage(), p->GetHiDamage() );
		}
	}
	else
	{
		UI16 getWrestSkill = ( p->GetSkill( WRESTLING ) / 65 );
		if( getWrestSkill > 0 )
		{
			getDamage = HalfRandomNum( getWrestSkill );
		}
		else
		{
			getDamage = RandomNum( 1, 2 );
		}
	}
	if( getDamage < 1 )
	{
		getDamage = 1;
	}
	return getDamage;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::CalcLowDamage()
//|	Date		-	11. Mar, 2006
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate low damage value of the char
//o------------------------------------------------------------------------------------------------o
SI16 CHandleCombat::CalcLowDamage( CChar *p )
{
	if( !ValidateObject( p ))
		return 0;

	SI16 getDamage = 0;

	CItem *weapon = GetWeapon( p );
	if( ValidateObject( weapon ))
	{
		if( weapon->GetLoDamage() > 0 && weapon->GetHiDamage() > 0 )
		{
			if( weapon->GetLoDamage() >= weapon->GetHiDamage() )
			{
				getDamage = weapon->GetHiDamage();
			}
			else
			{
				getDamage = weapon->GetLoDamage();
			}
		}
	}
	else if( p->IsNpc() )
	{
		if( p->GetLoDamage() >= p->GetHiDamage() )
		{
			getDamage = p->GetHiDamage();
		}
		else if( p->GetHiDamage() > 2 )
		{
			getDamage = p->GetLoDamage();
		}
	}
	else
	{
		UI16 getWrestSkill = ( p->GetSkill( WRESTLING ) / 65 );
		if( getWrestSkill > 0 )
		{
			getDamage = ( getWrestSkill / 2 );
		}
		else
		{
			getDamage = 1;
		}
	}
	if( getDamage < 1 )
	{
		getDamage = 1;
	}
	return getDamage;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::CalcHighDamage()
//|	Date		-	11. Mar, 2006
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate high damage value of the char
//o------------------------------------------------------------------------------------------------o
SI16 CHandleCombat::CalcHighDamage( CChar *p )
{
	if( !ValidateObject( p ))
		return 0;

	SI16 getDamage = 0;

	CItem *weapon = GetWeapon( p );
	if( ValidateObject( weapon ))
	{
		if( weapon->GetLoDamage() > 0 && weapon->GetHiDamage() > 0 )
		{
			if( weapon->GetLoDamage() >= weapon->GetHiDamage() )
			{
				getDamage = weapon->GetLoDamage();
			}
			else
			{
				getDamage = weapon->GetHiDamage();
			}
		}
	}
	else if( p->IsNpc() )
	{
		if( p->GetLoDamage() >= p->GetHiDamage() )
		{
			getDamage = p->GetLoDamage();
		}
		else if( p->GetHiDamage() > 2 )
		{
			getDamage = p->GetHiDamage();
		}
	}
	else
	{
		UI16 getWrestSkill = ( p->GetSkill( WRESTLING ) / 65 );
		if( getWrestSkill > 0 )
		{
			getDamage = getWrestSkill;
		}
		else
		{
			getDamage = 2;
		}
	}
	if( getDamage < 1 )
	{
		getDamage = 1;
	}
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
			CheckDef( CItem *checkItem, CItem& currItem, SI32 &currDef )
			GetArmorDef( CChar *mChar, CItem& defItem, UI08 bodyLoc, bool findTotal )
			CalcDef( CChar *mChar, UI08 hitLoc, bool doDamage )

	All information on armor defence values, divisors, and combat calculations gleaned from uo.stratics.com
	http://uo.stratics.com/content/arms-armor/armor.shtml
	http://uo.stratics.com/content/arms-armor/combat.shtml
	http://uo.stratics.com/content/arms-armor/clothes.shtml
	http://uo.stratics.com/content/aos/combatchanges.shtml
*/

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::CheckDef()
//|	Date		-	3/03/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks the defense of checkItem vs the defense of currItem and returns
//|							the item with the greater Def and its def value
//o------------------------------------------------------------------------------------------------o
CItem *CHandleCombat::CheckDef( CItem *checkItem, CItem *currItem, SI32 &currDef, WeatherType resistType )
{
	if( ValidateObject( checkItem ) && checkItem->GetResist( resistType ) > currDef )
	{
		currDef = checkItem->GetResist( resistType );
		return checkItem;
	}
	return currItem;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::GetArmorDef()
//|	Date		-	3/03/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Finds the item covering the location bodyLoc with the greatest AR and
//|							returns it along with its def value
//o------------------------------------------------------------------------------------------------o
CItem * CHandleCombat::GetArmorDef( CChar *mChar, SI32 &totalDef, UI08 bodyLoc, bool findTotal, WeatherType resistType )
{
	SI32 armorDef = 0;
	CItem *currItem = nullptr;
	switch( bodyLoc )
	{
		case 1:		// Torso
			currItem = CheckDef( mChar->GetItemAtLayer( IL_INNERSHIRT ), currItem, armorDef, resistType );	// Shirt
			currItem = CheckDef( mChar->GetItemAtLayer( IL_TUNIC ), currItem, armorDef, resistType );		// Torso (Inner - Chest Armor)
			currItem = CheckDef( mChar->GetItemAtLayer( IL_OUTERSHIRT ), currItem, armorDef, resistType );	// Torso (Middle - Tunic, etc)
			currItem = CheckDef( mChar->GetItemAtLayer( IL_CLOAK ), currItem, armorDef, resistType );		// Back (Cloak)
			currItem = CheckDef( mChar->GetItemAtLayer( IL_ROBE ), currItem, armorDef, resistType );		// Torso (Outer - Robe)
			if( findTotal )
			{
				if( cwmWorldState->ServerData()->ExpansionArmorCalculation() < ER_AOS )
				{
					armorDef = static_cast<SI32>( 100 * static_cast<R32>( armorDef / 2.8 ));
				}
			}
			break;
		case 2:		// Arms
			currItem = CheckDef( mChar->GetItemAtLayer( IL_ARMS ), currItem, armorDef, resistType );	// Arms
			if( findTotal )
			{
				if( cwmWorldState->ServerData()->ExpansionArmorCalculation() < ER_AOS )
				{
					armorDef = static_cast<SI32>( 100 * static_cast<R32>( armorDef / 6.8 ));
				}
			}
			break;
		case 3:		// Head
			currItem = CheckDef( mChar->GetItemAtLayer( IL_HELM ), currItem, armorDef, resistType );	// Head
			if( findTotal )
			{
				if( cwmWorldState->ServerData()->ExpansionArmorCalculation() < ER_AOS )
				{
					armorDef = static_cast<SI32>( 100 * static_cast<R32>( armorDef / 7.3 ));
				}
			}
			break;
		case 4:		// Legs
			currItem = CheckDef( mChar->GetItemAtLayer( IL_FOOTWEAR ), currItem, armorDef, resistType );	// Shoes
			currItem = CheckDef( mChar->GetItemAtLayer( IL_PANTS ), currItem, armorDef, resistType );	// Pants
			currItem = CheckDef( mChar->GetItemAtLayer( IL_WAIST ), currItem, armorDef, resistType );	// Waist (Half Apron)
			currItem = CheckDef( mChar->GetItemAtLayer( IL_OUTERLEGGINGS ), currItem, armorDef, resistType );	// Legs (Outer - Skirt, Kilt)
			currItem = CheckDef( mChar->GetItemAtLayer( IL_INNERLEGGINGS ), currItem, armorDef, resistType );	// Legs (Inner - Leg Armor)
			if( findTotal )
			{
				if( cwmWorldState->ServerData()->ExpansionArmorCalculation() < ER_AOS )
				{
					armorDef = static_cast<SI32>( 100 * static_cast<R32>( armorDef / 4.5 ));
				}
			}
			break;
		case 5:		// Neck
			currItem = CheckDef( mChar->GetItemAtLayer( IL_NECK ), currItem, armorDef, resistType );	// Neck
			if( findTotal )
			{
				if( cwmWorldState->ServerData()->ExpansionArmorCalculation() < ER_AOS )
				{
					armorDef = static_cast<SI32>( 100 * static_cast<R32>( armorDef / 14.5 ));
				}
			}
			break;
		case 6:		// Hands
			currItem = CheckDef( mChar->GetItemAtLayer( IL_GLOVES ), currItem, armorDef, resistType );	// Gloves
			if( findTotal )
			{
				if( cwmWorldState->ServerData()->ExpansionArmorCalculation() < ER_AOS )
				{
					armorDef = static_cast<SI32>( 100 * static_cast<R32>( armorDef / 14.5 ));
				}
			}
			break;
		default:
			break;
	}

	if( findTotal )
	{
		totalDef += armorDef;
	}
	else
	{
		totalDef = armorDef;
	}

	return currItem;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::CalcDef()
//|	Date		-	3/03/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Finds the defense value of a specific location or the entire character based on hitLoc
//o------------------------------------------------------------------------------------------------o
UI16 CHandleCombat::CalcDef( CChar *mChar, UI08 hitLoc, bool doDamage, WeatherType resistType )
{
	if( !ValidateObject( mChar ))
		return 0;

	SI32 total = mChar->GetResist( resistType );

	if( !mChar->IsNpc() || cwmWorldState->creatures[mChar->GetId()].IsHuman() ) // Polymorphed Characters and GM's can still wear armor
	{
		CItem *defendItem = nullptr;
		if( hitLoc == 0 )
		{
			for( UI08 getLoc = 1; getLoc < 7; ++getLoc )
			{
				GetArmorDef( mChar, total, getLoc, true, resistType );
			}
			if( cwmWorldState->ServerData()->ExpansionArmorCalculation() < ER_AOS )
			{
				total = ( total / 100 );
			}
		}
		else
		{
			defendItem = GetArmorDef( mChar, total, hitLoc, false, resistType );
		}

		// Deal damage to armor on hit, if enabled
		if( total > 0 && doDamage && ValidateObject( defendItem ) && !mChar->IsNpc() && ( cwmWorldState->ServerData()->CombatArmorDamageChance() >= RandomNum( 1, 100 )))
		{
			SI08 armorDamage = 0;	// Based on OSI standards, each successful hit does 0 to 2 damage to armor hit
			UI08 armorDamageMin = 0;
			UI08 armorDamageMax = 0;

			armorDamageMin = cwmWorldState->ServerData()->CombatArmorDamageMin();
			armorDamageMax = cwmWorldState->ServerData()->CombatArmorDamageMax();

			armorDamage -= static_cast<UI08>( RandomNum( static_cast<UI16>( armorDamageMin ), static_cast<UI16>( armorDamageMax )));
			defendItem->IncHP( armorDamage );

			if( defendItem->GetHP() <= 0 )
			{
				CSocket *mSock = mChar->GetSocket();
				if( mSock != nullptr )
				{
					std::string name;
					GetTileName(( *defendItem ), name );
					mSock->SysMessage( 311, name.c_str() ); // Your %s has been destroyed.
				}
				defendItem->Delete();
			}
		}
	}

	//if( total < 2 && hitLoc != 0 )
	//	total = 2;
	return static_cast<UI16>( total );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::CombatAnimsNew()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Play combat animations for characters (including gargoyles) in clients v7.0.0.0+
//o------------------------------------------------------------------------------------------------o
void CHandleCombat::CombatAnimsNew( CChar *i )
{
	if( !ValidateObject( i ))
		return;

	UI16 animToPlay		= N_ACT_ATT;		// 0x00
	UI16 subAnimToPlay	= S_ACT_WRESTLE;	// 0x00
	CItem *j			= GetWeapon( i );

	if( i->IsFlying() || i->IsOnHorse() )
	{
		switch( GetWeaponType( j ))
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
		switch( GetWeaponType( j ))
		{
			case BOWS:				subAnimToPlay = S_ACT_BOW;	break; // 0x01
			case XBOWS:
			case BLOWGUNS:			subAnimToPlay = S_ACT_XBOW;	break; // 0x02
			case THROWN:			subAnimToPlay = S_ACT_THROW;break; // 0x09
			case DEF_SWORDS:
			case SLASH_SWORDS:
			case ONEHND_LG_SWORDS:
				subAnimToPlay = ( RandomNum( 0, 2 ) > 1 ? ( RandomNum( 0, 3 ) > 2 ? S_ACT_1H_PIERCE : S_ACT_1H_BASH ) : S_ACT_1H_SLASH ); // 0x05, 0x03 and 0x04
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
				subAnimToPlay = ( RandomNum( 0, 2 ) > 1 ? ( RandomNum( 0, 3 ) > 2 ? S_ACT_2H_PIERCE : S_ACT_2H_BASH ) : S_ACT_2H_SLASH ); // 0x08, 0x06 and 0x07
				break;
			case TWOHND_AXES:
			case LG_MACES:			subAnimToPlay = ( RandomNum( 0, 3 ) > 2 ? S_ACT_2H_BASH : S_ACT_2H_SLASH ); break; // 0x06 and 0x07
			case WRESTLING:
			default:				subAnimToPlay = S_ACT_WRESTLE;	break; // 0x00
		}
	}
	Effects->PlayNewCharacterAnimation( i, animToPlay, subAnimToPlay );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::CombatOnHorse()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Play combat animations for mounted characters in clients v6.x and below
//o------------------------------------------------------------------------------------------------o
void CHandleCombat::CombatOnHorse( CChar *i )
{
	if( !ValidateObject( i ))
		return;

	UI16 animToPlay = ACT_MOUNT_ATT_1H; // 0x1A;
	CItem *j		= GetWeapon( i );
	UI08 frameCount = 5;

	// Normal mounted characters
	switch( GetWeaponType( j ))
	{
		case BOWS:				animToPlay = ACT_MOUNT_ATT_BOW;		break; // 0x1B
		case XBOWS:
		case BLOWGUNS:			
			animToPlay = ACT_MOUNT_ATT_XBOW;
			frameCount = 7;
			break; // 0x1C
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
	Effects->PlayCharacterAnimation( i, animToPlay, 0, frameCount );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::CombatOnFoot()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Play combat animations for characters on foot in clients v6.x and below
//o------------------------------------------------------------------------------------------------o
void CHandleCombat::CombatOnFoot( CChar *i )
{
	if( !ValidateObject( i ))
		return;

	UI16 animToPlay	= 0x00;
	CItem *j		= GetWeapon( i );
	switch( GetWeaponType( j ))
	{
		case BOWS:				animToPlay = ACT_ATT_BOW;		break; // 0x12
		case XBOWS:
		case BLOWGUNS:			animToPlay = ACT_ATT_XBOW;		break; // 0x13
		case DEF_SWORDS:
		case SLASH_SWORDS:
		case ONEHND_LG_SWORDS:
		case DUAL_SWORD:
		case DUAL_FENCING_SLASH:
		case ONEHND_AXES:		animToPlay = ( RandomNum( 0, 2 ) < 1 ? ACT_ATT_1H_SLASH : ACT_ATT_1H_PIERCE );	break; // 0x09 and 0x0A
		case DEF_MACES:			animToPlay = ( RandomNum( 0, 2 ) < 2 ? ACT_ATT_1H_SLASH : ACT_ATT_1H_BASH );		break; // 0x09 and 0x0B
		case LG_MACES:
		case TWOHND_LG_SWORDS:
		case BARDICHE:
		case TWOHND_AXES:		animToPlay = ( RandomNum( 0, 1 ) ? ACT_ATT_2H_BASH : ACT_ATT_2H_SLASH );	break; // 0x0C and 0x0D
		case DUAL_FENCING_STAB:
		case DEF_FENCING:		animToPlay = ACT_ATT_1H_PIERCE;	break; // 0x0A
		case TWOHND_FENCING:	animToPlay = ACT_ATT_2H_PIERCE;	break; // 0x0E
		case WRESTLING:
		default:
			switch( RandomNum( 0, 2 ))
			{
				case 0:		animToPlay = ACT_ATT_1H_BASH;		break; // 0x0B
				case 1:		animToPlay = ACT_ATT_1H_SLASH;		break; // 0x09
				default:	animToPlay = ACT_ATT_WRESTLE;		break; // 0x1F
			}
			break;
	}
	Effects->PlayCharacterAnimation( i, animToPlay, 0, 7 );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::PlaySwingAnimations()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Plays the attack/swing-animation for specified character
//o------------------------------------------------------------------------------------------------o
void CHandleCombat::PlaySwingAnimations( CChar *mChar )
{
	UI16 charId = mChar->GetId();
	if( !cwmWorldState->creatures[charId].IsHuman() )
	{
		UI08 attackAnim = 0;
		UI08 attackAnimLength = 0;

		// Get available attack animations for creature
		UI16 attackAnim1 = static_cast<UI16>( cwmWorldState->creatures[mChar->GetId()].AttackAnim1Id() );
		UI16 attackAnim2 = static_cast<UI16>( cwmWorldState->creatures[mChar->GetId()].AttackAnim2Id() );
		UI16 attackAnim3 = static_cast<UI16>( cwmWorldState->creatures[mChar->GetId()].AttackAnim3Id() );
		UI08 attackAnim1Length = cwmWorldState->creatures[mChar->GetId()].AttackAnim1Length();
		UI08 attackAnim2Length = cwmWorldState->creatures[mChar->GetId()].AttackAnim2Length();
		UI08 attackAnim3Length = cwmWorldState->creatures[mChar->GetId()].AttackAnim3Length();

		if( charId == 0x2d8 ) // Special case for Medusa, which has 1 ranged attack anim and 1 melee
		{
			CItem *equippedWeapon = GetWeapon( mChar );
			switch( GetWeaponType( equippedWeapon ))
			{
				case BOWS:
				case XBOWS:
					attackAnim = attackAnim1; // Ranged anim
					attackAnimLength = attackAnim1Length;
					break;
				default:
					attackAnim = attackAnim2; // Melee anim
					attackAnimLength = attackAnim2Length;
					break;
			}
		}
		else
		{
			// Randomize between the available attack animations (as defined in creatures.dfn)
			auto rndAnimNum = -1;
			if( attackAnim1 > 0 && attackAnim2 > 0 && attackAnim3 > 0 )
			{
				rndAnimNum = RandomNum( 0, 2 );
			}
			else if( attackAnim1 > 0 && attackAnim2 > 0 )
			{
				rndAnimNum = RandomNum( 0, 1 );
			}
			else if( attackAnim1 > 0 )
			{
				rndAnimNum = 0;
			}

			if( rndAnimNum > -1 )
			{
				// Select the appropriate animation
				switch( rndAnimNum )
				{
					case 0:
						attackAnim = attackAnim1;
						attackAnimLength = attackAnim1Length;
						break;
					case 1:
						attackAnim = attackAnim2;
						attackAnimLength = attackAnim2Length;
						break;
					case 2:
						attackAnim = attackAnim3;
						attackAnimLength = attackAnim3Length;
						break;
				}
			}
			else
			{
				// No valid attack animation defined, try to use a default attack animation
				attackAnim = 0x05;
				attackAnimLength = 4;
			}
		}

		// Play the selected attack animation
		Effects->PlayCharacterAnimation( mChar, attackAnim, 0, attackAnimLength );

		// Play attack sound effect
		if( RandomNum( 0, 4 )) // 20% chance of playing SFX when attacking
		{
			UI16 toPlay = cwmWorldState->creatures[charId].GetSound( SND_ATTACK );
			if( toPlay != 0x00 )
			{
				Effects->PlaySound( mChar, toPlay );
			}
		}
	}
	else if( mChar->GetBodyType() == BT_GARGOYLE || cwmWorldState->ServerData()->ForceNewAnimationPacket() )
	{
		CombatAnimsNew( mChar );
	}
	else if( mChar->IsOnHorse() )
	{
		CombatOnHorse( mChar );
	}
	else
	{
		CombatOnFoot( mChar );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::PlayMissedSoundEffect()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Do the "Missed" Sound Effect
//o------------------------------------------------------------------------------------------------o
void CHandleCombat::PlayMissedSoundEffect( CChar *p )
{
	CItem *weapon = GetWeapon( p );

	if( !ValidateObject( p ))
		return;

	switch( GetWeaponType( weapon ))
	{
		case BOWS:
		case XBOWS: Effects->PlaySound( p, RandomNum( 0x04c8, 0x04c9 )); break;
		case BLOWGUNS: Effects->PlaySound( p, 0x052F ); break;
		default:
			switch( RandomNum( 0, 2 ))
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

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::PlayHitSoundEffect()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Do the "Hit" Sound Effect
//o------------------------------------------------------------------------------------------------o
void CHandleCombat::PlayHitSoundEffect( CChar *p, CItem *weapon )
{
	if( !ValidateObject( p ) || !ValidateObject( weapon ))
		return;

	switch( GetWeaponType( weapon ))
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
			switch( RandomNum( 0, 3 )) // Wrestling
			{
				case 0:		Effects->PlaySound( p, 0x0135 );	break;
				case 1:		Effects->PlaySound( p, 0x0137 );	break;
				case 2:		Effects->PlaySound( p, 0x013D );	break;
				default:	Effects->PlaySound( p, 0x013B );	break;
			}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::AdjustRaceDamage()
//|	Date		-	3rd July, 2001
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adjusts the damage dealt to defend by weapon based on
//|						race and weather weaknesses
//o------------------------------------------------------------------------------------------------o
SI16 CHandleCombat::AdjustRaceDamage( CChar *attack, CChar *defend, CItem *weapon, SI16 bDamage, UI08 hitLoc, UI08 getFightSkill )
{
	SI16 amount		= 0;

	if( !ValidateObject( defend ) || !ValidateObject( weapon ))
		return bDamage;

	if( weapon->GetRace() == defend->GetRace() )
	{
		amount = bDamage;
	}
	CRace *rPtr = Races->Race( defend->GetRace() );
	if( rPtr != nullptr )
	{
		for( SI32 i = LIGHT; i < WEATHNUM; ++i )
		{
			if( weapon->GetWeatherDamage( static_cast<WeatherType>( i )) && rPtr->AffectedBy( static_cast<WeatherType>( i )))
			{
				amount += ApplyDefenseModifiers( static_cast<WeatherType>( i ), attack, defend, getFightSkill, hitLoc, bDamage, false );
			}
		}
	}
	return ( bDamage + amount );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::AdjustArmorClassDamage()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adjusts the damage dealt to defender based on potential armour class weakness
//o------------------------------------------------------------------------------------------------o
SI16 CHandleCombat::AdjustArmorClassDamage( CChar *attacker, CChar *defender, CItem *attackerWeapon, SI16 baseDamage, UI08 hitLoc )
{
	SI32 bonusDamage = 0;

	if( !ValidateObject( defender ) || !ValidateObject( attackerWeapon ))
		return baseDamage;

	CItem *defenderItem = NULL;
	if( hitLoc != 0 )
	{
		// If hit location system is enabled, check armor class of item at specified hit location
		defenderItem = GetArmorDef( attacker, bonusDamage, hitLoc, false, PHYSICAL );

		if( ValidateObject( defenderItem ) && attackerWeapon->GetArmourClass() == defenderItem->GetArmourClass() )
		{
			// Add bonus damage equal to half of the armor's physical resist/armor rating
			bonusDamage = bonusDamage / 2;
		}
	}
	else
	{
		// Hit location system not enabled. Rely on average AC/physical resist instead
		SI32 bonusPotential = 0;
		for( UI08 getLoc = 1; getLoc < 7; ++getLoc )
		{
			defenderItem = GetArmorDef( attacker, bonusPotential, getLoc, false, PHYSICAL );

			if( ValidateObject( defenderItem ) && attackerWeapon->GetArmourClass() == defenderItem->GetArmourClass() )
			{
				bonusDamage += bonusPotential;
			}
		}

		bonusDamage /= 7; // get average of all 7 armor locations
		bonusDamage /= 2; // then split damage in half.

		if( cwmWorldState->ServerData()->ExpansionArmorCalculation() < ER_AOS )
		{
			bonusDamage = ( bonusDamage / 100 );
		}
	}

	return ( baseDamage + static_cast<SI16>( bonusDamage ));
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::DoHitMessage()
//|	Date		-	3rd July, 2001
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Prints out the hit message, if enabled
//o------------------------------------------------------------------------------------------------o
void CHandleCombat::DoHitMessage( CChar *mChar, CChar *ourTarg, SI08 hitLoc, SI16 damage )
{
	if( !ValidateObject( ourTarg ) || !ValidateObject( mChar ) || ourTarg->IsNpc() )
		return;

	CSocket *targSock = ourTarg->GetSocket();

	if( cwmWorldState->ServerData()->CombatDisplayHitMessage() && targSock != nullptr )
	{
		std::string attackerName = GetNpcDictName( mChar, targSock, NRS_SPEECH );

		// Don't show hit messages for very low amounts of damage
		if( damage < 5 )
			return;

		UI08 randHit = RandomNum( 0, 2 );
		switch( hitLoc )
		{
			case 1:	// Body
				switch ( randHit )
				{
					case 1:
						if( damage < 15  )
						{
							targSock->SysMessage( 284, attackerName.c_str() ); // %s hits you in your Chest!
						}
						else
						{
							targSock->SysMessage( 285, attackerName.c_str() ); // %s lands a terrible blow to your Chest!
						}
						break;
					case 2:
						if( damage < 15 )
						{
							targSock->SysMessage( 286, attackerName.c_str() ); // %s lands a blow to your Stomach!
						}
						else
						{
							targSock->SysMessage( 287, attackerName.c_str() ); // %s knocks the wind out of you!
						}
						break;
					default:
						if( damage < 15 )
						{
							targSock->SysMessage( 288, attackerName.c_str() ); // %s hits you in your Ribs!
						}
						else
						{
							targSock->SysMessage( 289, attackerName.c_str() ); // %s broke your Rib!
						}
						break;
				}
				break;
			case 2:	// Arms
				switch( randHit )
				{
					case 1:		targSock->SysMessage( 290, attackerName.c_str() );		break; // %s hits you in your Left Arm!
					case 2:		targSock->SysMessage( 291, attackerName.c_str() );		break; // %s hits you in your Right Arm!
					default:	targSock->SysMessage( 292, attackerName.c_str() );		break; // %s hits you in your Right Arm!
				}
				break;
			case 3:	// Head
				switch( randHit )
				{
					case 1:
						if( damage < 15 )
						{
							targSock->SysMessage( 293, attackerName.c_str() ); // %s hits you you straight in the Face!
						}
						else
						{
							targSock->SysMessage( 294, attackerName.c_str() ); // %s lands a stunning blow to your Head!
						}
						break;
					case 2:
						if( damage < 15 )
						{
							targSock->SysMessage( 295, attackerName.c_str() ); // %s hits you to your Head!
						}
						else
						{
							targSock->SysMessage( 296, attackerName.c_str() ); // %s smashed a blow across your Face!
						}
						break;
					default:
						if( damage < 15 )
						{
							targSock->SysMessage( 297, attackerName.c_str() ); // %s hits you you square in the Jaw!
						}
						else
						{
							targSock->SysMessage( 298, attackerName.c_str() ); // %s lands a terrible hit to your Temple!
						}
						break;
				}
				break;
			case 4:	// Legs
				switch( randHit )
				{
					case 1:		targSock->SysMessage( 299, attackerName.c_str() );		break; // %s hits you in the Left Thigh!
					case 2:		targSock->SysMessage( 300, attackerName.c_str() );		break; // %s hits you in the Right Thigh!
					default:	targSock->SysMessage( 301, attackerName.c_str() );		break; // %s hits you in the Groin!
				}
				break;
			case 5:	// Neck
				targSock->SysMessage( 302, attackerName.c_str() ); // %s hits you to your Throat!
				break;
			case 6:	// Hands
				switch( randHit )
				{
					case 1:		targSock->SysMessage( 303, attackerName.c_str() );		break; // %s hits you in the Left Hand!
					case 2:		targSock->SysMessage( 304, attackerName.c_str() );		break; // s hits you in the Right Hand!
					default:	targSock->SysMessage( 305, attackerName.c_str() );		break; // %s hits you in the Right Hand!
				}
				break;
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::CalculateHitLoc()
//|	Date		-	3rd July, 2001
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	calculates where on the body the person was hit and returns that
//o------------------------------------------------------------------------------------------------o
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

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::ApplyDamageBonuses()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies damage bonuses based on race/weather weakness and character skills
//o------------------------------------------------------------------------------------------------o
SI16 CHandleCombat::ApplyDamageBonuses( WeatherType damageType, CChar *mChar, CChar *ourTarg, UI08 getFightSkill, UI08 hitLoc, SI16 baseDamage )
{
	if( !ValidateObject( ourTarg ) || !ValidateObject( mChar ))
		return baseDamage;

	R32 multiplier = 1;
	R32 damage = 0;
	SI32 RaceDamage = 0;
	CItem *mWeapon = GetWeapon( mChar );
	CRace *rPtr = Races->Race( ourTarg->GetRace() );
	auto serverData = cwmWorldState->ServerData();

	switch( damageType )
	{
		case NONE:
			damage = static_cast<R32>( baseDamage );
			break;
		case PHYSICAL:
			// Race Dmg Modification: Bonus percentage.
			RaceDamage = Races->DamageFromSkill( getFightSkill, mChar->GetRace() );
			if( RaceDamage != 0 )
			{
				baseDamage += static_cast<SI16>( static_cast<R32>( baseDamage ) * ( static_cast<R32>( RaceDamage ) / 1000 ));
			}

			// Adjust race and weather weakness
			baseDamage = AdjustRaceDamage( mChar, ourTarg, mWeapon, baseDamage, hitLoc, getFightSkill );

			// Adjust for armour class weakness
			if( serverData->CombatArmorClassDamageBonus() )
			{
				baseDamage = AdjustArmorClassDamage( mChar, ourTarg, mWeapon, baseDamage, hitLoc );
			}

			// Publish 5 (April 27, 2000 - UOR patch) had some changes to combat damage:
			// The lumberjacking skill will provide a bonus to damage when the player is using one of the following axes. 
			// The higher the characters lumberjacking skill the more damage they will do up to a 25% bonus for 99.9 lumberjacking.
			// At Grandmaster lumberjacking, the damage bonus is 35%.
			//	Axe
			//	Battle Axe
			//	Double Axe
			//	Executioner’s Axe
			//	Hatchet
			//	Large Battle Axe
			//	Two - handed Axe

			// Publish 13 (Aug 19, 2001, during Third Dawn era) had some changes to combat damage: https://www.uoguide.com/Publish_13
			// Hit points for players: (str/2) + 50
			// Anatomy had a bug prior to this where damage bonus against players was doubled. After fixing this,
			// the bonus from Anatomy was changed to 20% of Anatomy skill (against both monsters and players) for players
			// with less than 100.0 Anatomy, and 30% of skill for players who reach 100.0 Anatomy.
			// Lumberjack damage bonus for axe weapons using same formula as anatomy; 20% bonus up until 99.9 skillpoints, 30% for 100.0 skillpoints
			// Special hits work on monsters as well as players
			// NPCs can do special hits if they have anatomy and two-handed melee weapon equipped
			// All special hits with 2-h weapons now base chance to go off on anatomy skill: Anatomy / 4
			// Pre-casting re-enabled: Cannot equip anything while casting, after you cast a spell it will "time out" after 30 seconds,
			// and can no longer target anything. During those 30 seconds, you can equip a weapon, attack with it, and still keep the stored spell.
			// Upon releasing the spell you automatically unequip the weapon

			// Publish 13 spell changes
			// Spellbooks no longer required to be equipped to cast spells
			// Players can gain wrestling and/or tactics skill while having spellbooks equipped
			// Magic Arrow, Fireball, Lightning Bolt deal noticably more damage than before
			// Energy Bolt, Explosion, Chain Lightning, Meteor Swarm and Flamestrike spells given small damage increase
			// Lower reagent cost: Magic Arrow (1 Sulfurous Ash), Fireball (1 Black Pearl), Lightning (1 Mandrake Root, 1 Sulfurous Ash), Explosion (1 Bloodmoos, 1 Mandrake Root)
			// Harm spell damage now scales inversely with distance to target; the closer to the target, the higher the damage

			// From at least ML forward (https://web.archive.org/web/20080616071554/https://uo.stratics.com/content/arms-armor/combat.php)
			// Tactics Damage Bonus% = Tactics ÷ 1.6 (Add 6.25% if Tactics >= 100)
			// Anatomy Damage Bonus% = (Anatomy ÷ 2) + 5
			// Lumberjack Damage Bonus% = Lumberjack ÷ 5 (Add 10% if Lumberjacking >= 100)
			// Strength Damage Bonus% = Strength * 0.3 (Add 5% if Strength >= 100)
			// Final Damage Bonus% = Tactics Bonus + Anatomy Bonus + Lumber Bonus + Strangth Bonus + Damage Increase Items*
			// Final Damage = Base Damage + (Base Damage * Final Damage Bonus%)
			// * Damage Increase is capped at 100%.

			// ToL/EJ source: https://www.uoguide.com/Damage_Calculations

			// Strength Damage Bonus
			if( serverData->ExpansionStrengthDamageBonus() >= ER_TD )
			{
				// Third Dawn expansion and later
				if( mChar->GetStrength() >= 100 )
				{
					multiplier = static_cast<R32>( mChar->GetStrength() * 0.3) + 5;
				}
				else
				{
					multiplier = static_cast<R32>( mChar->GetStrength() * 0.3);
				}
			}
			else
			{
				// Pre-publish 13 (T2A, UOR, early TD)
				// Capped at 200 strength
				auto strBonusPercent = 20;
				multiplier = static_cast<R32>((( std::min( mChar->GetStrength(), static_cast<SI16>( 200 )) * strBonusPercent ) / 100 ) / 100 ) + 1;
			}

			// Tactics Damage Bonus
			if( serverData->ExpansionTacticsDamageBonus() >= ER_AOS )
			{
				// Age of Shadows expansion and later
				if( mChar->GetSkill( TACTICS ) >= 1000 )
				{
					multiplier += static_cast<R32>((( mChar->GetSkill( TACTICS ) / 10 ) / 1.6 ) + 6.25 );
				}
				else
				{
					multiplier += static_cast<R32>(( mChar->GetSkill( TACTICS ) / 10 ) / 1.6 );
				}
			}
			else
			{
				// Pre-AoS (pre-publish 13 (T2A, UOR, early TD))
				// Tactics Damage Bonus (% = ( Tactics + 50 ))
				multiplier += static_cast<R32>(( mChar->GetSkill( TACTICS ) + 500 ) / 10 );
			}

			// Anatomy Damage Bonus
			if( serverData->ExpansionAnatomyDamageBonus() >= ER_ML )
			{
				// Mondain's Legacy expansion and later
				if( mChar->GetSkill( ANATOMY ) >= 1000 )
				{
					multiplier += static_cast<R32>((( mChar->GetSkill( ANATOMY ) / 2 ) / 10 ) + 5 ); // 50% + 5 bonus damage at GM Skill or above, 
				}
				else
				{
					multiplier += static_cast<R32>(( mChar->GetSkill( ANATOMY ) / 2 ) / 10 ); // Up to 50% bonus damage at 99.9 skillpoints or below
				}
			}
			else if( serverData->ExpansionAnatomyDamageBonus() >= ER_TD )
			{
				// Third Dawn expansion and later
				if( mChar->GetSkill( ANATOMY ) >= 1000 )
				{
					multiplier += 30; // 30% Damage at GM Skill or above,
				}
				else
				{
					multiplier += static_cast<R32>((( mChar->GetSkill( ANATOMY ) / 10 ) / 5 )); // Up to 20% Damage at 99.9 skillpoints or below
				}
			}
			else
			{
				// Prior to Publish 13, Anatomy bonus was double against players (bug, not a feature)
				if( ourTarg->IsNpc() ) // Anatomy PvM damage Bonus, % = ( Anat / 5 )
				{
					multiplier += static_cast<R32>((( mChar->GetSkill( ANATOMY ) / 10 ) / 5 ));
				}
				else // Anatomy PvP damage Bonus, % = ( Anat / 2.5 )
				{
					multiplier += static_cast<R32>((( mChar->GetSkill( ANATOMY ) / 10 ) / 2.5 ));
				}
			}

			// Lumberjacking Damage Bonus
			if( serverData->ExpansionLumberjackDamageBonus() >= ER_TD ) // Third Dawn expansion and later
			{
				if( serverData->ExpansionLumberjackDamageBonus() >= ER_HS && RandomNum( 1, 100 ) <= 10 )
				{
					// High Seas expansion and later
					// Publish 69 added a 10% chance for a lumberjacking damage bonus of 100% from base weapon damage
					// https://www.uoguide.com/Publish_69
					multiplier += 100;
				}
				else
				{
					// Third Dawn Expansion (Publish 13) or later
					if( mChar->GetSkill( LUMBERJACKING ) >= 1000 )
					{
						multiplier += 30; // 30% Damage at GM Skill or above, 
					}
					else
					{
						multiplier += static_cast<R32>((( mChar->GetSkill( LUMBERJACKING ) / 10 ) / 5 )); // up to 20% Damage at 99.9 skillpoints or below
					}
				}
			}
			else
			{
				// UO:R expansion
				if( serverData->ExpansionLumberjackDamageBonus() == ER_UOR )
				{
					if( mChar->GetSkill( LUMBERJACKING ) >= 1000 )
					{
						multiplier += 35; // At GM skill the damage bonus is 35%.
					}
					else
					{
						multiplier += static_cast<R32>((( mChar->GetSkill( LUMBERJACKING ) / 10 ) / 4 )); // up to 25% bonus damage at 99.9 skillpoints or below
					}
				}
			}

			// Racial Bonus (Berserk), gargoyles gains +15% Damage Increase per each 20 HP lost
			if( serverData->ExpansionRacialDamageBonus() >= ER_SA )
			{
				if( mChar->GetBodyType() == BT_GARGOYLE )
				{
					// Find out how much HP the gargoyle player has lost, in percentage
					auto hpDifference = (( mChar->GetMaxHP() - mChar->GetHP() ) / mChar->GetMaxHP() ) * 100;
					if( hpDifference > 20 )
					{
						// Add 15% damage bonus for each 20% HP lost
						multiplier += std::min( static_cast<R32>( floor( static_cast<R32>( hpDifference / 20 )) * static_cast<R32>( 15 )), static_cast<R32>( 60 ));
					}
				}
			}

			// Where does this come from??
			// Defender Tactics Damage Modifier, -20% Damage
			//multiplier += static_cast<R32>(1.0 - ((( ourTarg->GetSkill( TACTICS ) * 20 ) / 1000 ) / 100 ));

			multiplier /= 100;
			damage = baseDamage + static_cast<R32>( baseDamage * multiplier );
			break;
		default:
			damage = static_cast<R32>( baseDamage );

			// If the attack is magic and the target a NPC but not a human, double the damage
			if( getFightSkill == MAGERY && ourTarg->IsNpc() && !cwmWorldState->creatures[ourTarg->GetId()].IsHuman() )
			{
				damage *= 2;
			}
			break;
	}

	// If the race is weak to this element double the damage
	if( rPtr != nullptr )
	{
		if( rPtr->AffectedBy( damageType ))
		{
			damage *= 2;
		}
	}

	if( serverData->ExpansionDamageBonusCap() >= ER_AOS )
	{
		// Cap damage at 300% higher than base damage
		damage = std::min( damage, static_cast<R32>( baseDamage * 4 ));
	}

	return static_cast<SI16>( std::round( damage ));
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::ApplyDefenseModifiers()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies defense modifiers based on shields/parrying, armor values and elemental damage
//o------------------------------------------------------------------------------------------------o
SI16 CHandleCombat::ApplyDefenseModifiers( WeatherType damageType, CChar *mChar, CChar *ourTarg, UI08 getFightSkill, UI08 hitLoc, SI16 baseDamage, bool doArmorDamage )
{
	if( !ValidateObject( ourTarg ))
		return baseDamage;

	UI16 getDef = 0, attSkill = 1000;
	R32 damageModifier = 0;
	R32 damage = static_cast<R32>( baseDamage );

	if( ValidateObject( mChar ))
	{
		attSkill = mChar->GetSkill( getFightSkill );
	}

	CSocket *targSock = ourTarg->GetSocket();
	CItem *shield = GetShield( ourTarg );
	auto serverData = cwmWorldState->ServerData();

	switch( damageType )
	{
		case NONE: break;	//	No Armor protection
		case PHYSICAL:		//	Physical damage
		{
			// Check Shield Defense
			bool parrySuccess = false;
			if( ValidateObject( shield ))
			{
				// Perform a skillcheck to potentially give player a skill increase
				Skills->CheckSkill( ourTarg, PARRYING, 0, 1000 );

				// Get parry skill value
				UI16 defendParry = ourTarg->GetSkill( PARRYING );

				if( serverData->ExpansionShieldParry() <= ER_T2A )
				{
					// T2A parry formula: parryChance = parrySkill / 2
					// AR of shield is then used to absorb a portion of the potential damage dealt; 8 AR shield absorbs 8 damage
					// Source https://forums.uosecondage.com/viewtopic.php?t=13478
					R32 parryChance = static_cast<R32>(( defendParry / 2 ) / 10);
					if( RandomNum( 1, 100 ) < parryChance )
					{
						parrySuccess = true;
					}
				}
				else if( serverData->ExpansionShieldParry() < ER_AOS )
				{
					// Renaissance Publish (UOR) - https://uo.com/wiki/ultima-online-wiki/technical/previous-publishes/2000-2/2000-publish-05-27th-april/
					// The higher a shield's AR, the lower the chance to block, but the more damage is absorbed upon blocking
					// The lower a shield's AR, the higher the chance to block, but the less damage is absorbed upon blocking
					// parryChance = parrySkill - (shield AR * 2) 
					R32 parryChance = (defendParry / 10 ) - ( shield->GetResist( PHYSICAL ) * 2 ); // or is it 1.33?
					if( RandomNum( 1, 100 ) < parryChance )
					{
						parrySuccess = true;
					}
				}
				else if( serverData->ExpansionShieldParry() >= ER_AOS )
				{
					// Post-AoS Parrying with Shield
					UI16 defendBushido = ourTarg->GetSkill( BUSHIDO );

					// % Chance = (Parrying - Bushido) / 4 (If less than 0, the chance is 0)
					R32 parryChance = (( defendParry - defendBushido ) / 4 ) / 10;
					if( defendParry >= 1000 || defendBushido >= 1000 )
					{
						parryChance += 5.0;
					}

					// Dexterity Modifier if dex is less than 80*: (80 - Dexterity) / 100 (If Dexterity is higher than 80, the modifier is 0)
					// Final % Chance of blocking = Base Chance * (1 - Dexterity Modifier)
					R32 dexModifier = ( ourTarg->GetDexterity() > 80 ? 0 : ( 80 - ourTarg->GetDexterity() ) / 100 );
					parryChance *= ( 1 - dexModifier );

					if( RandomNum( 1, 100 ) < parryChance )
					{
						parrySuccess = true;
					}
				}
				/*else
				{
					// Old UOX3 parry chance
					if( HalfRandomNum( defendParry ) >= HalfRandomNum( attSkill ))
						parrySuccess = true;
				}*/

				if( parrySuccess )
				{
					// Play shield parrying FX
					Effects->PlayStaticAnimation( ourTarg, 0x37b9, 10, 16 );

					auto loShieldDamage = cwmWorldState->ServerData()->CombatParryDamageMin();
					auto hiShieldDamage = cwmWorldState->ServerData()->CombatParryDamageMax();
					SI16 shieldDamage = -( RandomNum( static_cast<SI16>( loShieldDamage ), static_cast<SI16>( hiShieldDamage )));

					if( cwmWorldState->ServerData()->CombatDisplayHitMessage() )
					{
						if( targSock != nullptr )
						{
							targSock->SysMessage( 1805 ); // You block the attack!
						}
						else if( ValidateObject( mChar ) && mChar->GetSocket() != nullptr )
						{
							mChar->GetSocket()->SysMessage( 2060 ); // Your attack was blocked!
						}
					}

					if( serverData->ExpansionShieldParry() <= ER_T2A )
					{
						// http://web.archive.org/web/19991009001809/http://uo.stratics.com/combat.htm
						// FORMULA: Melee Damage Absorbed = ( AR of Shield ) / 2 | Archery Damage Absorbed = AR of Shield
						if( getFightSkill == ARCHERY )
						{
							damage -= static_cast<R32>( shield->GetResist( PHYSICAL ));
						}
						else
						{
							damage -= static_cast<R32>( shield->GetResist( PHYSICAL ) / 2 );
						}

						// Calculate defense given by armor
						getDef = HalfRandomNum( CalcDef( ourTarg, hitLoc, doArmorDamage, PHYSICAL ));

						// Apply damage to shield from parrying action?
						if( cwmWorldState->ServerData()->CombatParryDamageChance() >= RandomNum( 1, 100 )) // 20% chance by default
						{
							shield->IncHP( shieldDamage );
						}
					}
					else if( serverData->ExpansionShieldParry() < ER_AOS )
					{
						// Pre-AoS/LBR/UOR
						// FORMULA: Melee Damage Absorbed = ( AR of Shield ) / 2 | Archery Damage Absorbed = AR of Shield
						if( getFightSkill == ARCHERY )
						{
							damage -= static_cast<R32>( shield->GetResist( PHYSICAL ));
						}
						else
						{
							damage -= static_cast<R32>( shield->GetResist( PHYSICAL ) / 2 );
						}

						// Calculate defense given by armor
						getDef = HalfRandomNum( CalcDef( ourTarg, hitLoc, doArmorDamage, PHYSICAL ));

						// Apply damage to shield from parrying action?
						if( cwmWorldState->ServerData()->CombatParryDamageChance() >= RandomNum( 1, 100 )) // 20% chance by default
						{
							shield->IncHP( shieldDamage );
						}
					}
					else if( serverData->ExpansionShieldParry() >= ER_AOS )
					{
						// Block attack completely
						damage = 0;
						getDef = 0;

						if( serverData->ExpansionShieldParry() >= ER_ML )
						{
							// If you successfully parry a blow, shield has 20% chance to take a point of damage
							// Unless attacker's weapon is a mace, in which case chance to take a point of damage is 75%
							bool damageShield = false;
							if( getFightSkill == MACEFIGHTING )
							{
								damageShield = ( RandomNum( 1, 4 ) < 4 ); // 75% chance
							}
							else
							{
								damageShield = ( RandomNum( 1, 5 ) == 1 ); // 20% chance
							}

							if( damageShield )
							{
								shield->IncHP( shieldDamage );
							}
						}
					}
					/*else
					{
						// Old Pre-AoS (LBR, ~Publish 15) block with shield
						damage -= HalfRandomNum( shield->GetResist( PHYSICAL ));
						getDef = HalfRandomNum( CalcDef( ourTarg, hitLoc, doArmorDamage, PHYSICAL ));

						// Apply damage to shield from parrying action?
						if( cwmWorldState->ServerData()->CombatParryDamageChance() >= RandomNum( 1, 100 )) // 20% chance by default
						{
							shield->IncHP( shieldDamage );
						}
					}*/

					if( shield->GetHP() <= 0 )
					{
						if( targSock != nullptr )
						{
							targSock->SysMessage( 283 ); // Your shield has been destroyed!
						}
						shield->Delete();
					}
				}
			}
			else if( serverData->ExpansionWeaponParry() >= ER_AOS )
			{
				// Let's check if character can parry with weapon via Bushido skill
				CItem *mWeapon = GetWeapon( ourTarg );
				if( mWeapon )
				{
					// Perform a skillcheck for Bushido regardless of weapon equipped
					Skills->CheckSkill( ourTarg, BUSHIDO, 0, 1000 );

					// Fetch relevant skill values
					UI16 defendParry = ourTarg->GetSkill( PARRYING );
					UI16 defendBushido = ourTarg->GetSkill( BUSHIDO );
					R32 parryChance = 0;
					R32 dividerValue = 48000; // default for 1H weapon

					if( mWeapon->GetLayer() == IL_LEFTHAND )
					{
						dividerValue = 41140;
					}

					// New = (Parrying * 10) * (Bushido * 10) / dividerValue (Add 5% if Parrying or Bushido skill is 100 or above)
					R32 parryChanceNew = ( defendParry * defendBushido ) / dividerValue;
					if( defendParry >= 1000 || defendBushido >= 1000 )
					{
						parryChanceNew += 50;
					}

					// Legacy = (Parrying * 10) / 80 (Add 5% if Parrying skill if 100 or above)
					R32 parryChanceLegacy = static_cast<R32>( defendParry / 80 );
					if( defendParry >= 1000 )
					{
						parryChanceLegacy += 50;
					}

					// % Chance = Whichever is highest of the New and the Legacy formula.
					parryChance = std::max( parryChanceNew, parryChanceLegacy );

					// Dexterity Modifier if dex is less than 80*: (80 - Dexterity) / 100 (If Dexterity is higher than 80, the modifier is 0)
					// Final % Chance of blocking = Base Chance * (1 - Dexterity Modifier)
					R32 dexModifier = ( ourTarg->GetDexterity() > 80 ? 0 : ( 80 - ourTarg->GetDexterity() ) / 100 );
					parryChance *= ( 1 - dexModifier );

					// Check if parrying succeedes
					if( RandomNum( 0, 1000 ) < parryChance )
					{
						// Successfully parried! Block attack completely
						getDef = 0;
						damage = 0;

						// Play parrying FX
						Effects->PlayStaticAnimation( ourTarg, 0x37b9, 10, 16 );

						if( cwmWorldState->ServerData()->CombatDisplayHitMessage() && targSock != NULL )
						{
							targSock->SysMessage( 1982 ); // You parry the attack!
						}

						if( serverData->ExpansionWeaponParry() >= ER_ML )
						{
							// If parrying with a weapon, 5% chance weapon will take 1 point of damage
							// Unless attacker has a mace weapon, then chance is 75% to take 1 point of damage
							bool damageWeapon = false;
							if( getFightSkill == MACEFIGHTING )
							{
								damageWeapon = ( RandomNum( 1, 4 ) < 4 ); // 75% chance
							}
							else
							{
								damageWeapon = ( RandomNum( 1, 20 ) == 1 ); // 5% chance
							}

							// Apply damage to weapon from parrying action?
							if( damageWeapon )
							{
								mWeapon->IncHP( -1 );
							}
						}
						else
						{
							// Apply damage to weapon from parrying action?
							if( !RandomNum( 0, 5 )) // 16.6% chance of weapon damage when parrying
							{
								mWeapon->IncHP( -1 );
							}
						}

						// Destroy weapon if it ran out of hitpoints
						if( mWeapon->GetHP() <= 0 )
						{
							if( targSock != NULL )
							{
								targSock->SysMessage( 1983 ); // Your weapon has been destroyed!
							}
							mWeapon->Delete();
						}
					}
				}
				else if( serverData->ExpansionWrestlingParry() >= ER_TOL && !cwmWorldState->creatures[ourTarg->GetId()].IsHuman() )
				{
					// In Publish 97, all NPC creatures with Wrestling skill of 100.0 or higher were given a chance to parry attacks
					// https://www.uoguide.com/Publish_97
					R32 parryChance = 0;
					UI16 defendWrestling = ourTarg->GetSkill( WRESTLING );

					if( defendWrestling >= 1000 )
					{
						// ~12.5% chance for a NPC creature with GM Wrestling to parry an attack
						// TODO Turn the wrestling parry chance into a ini setting
						parryChance = HalfRandomNum( defendWrestling ) / 8;

						if( RandomNum( 0, 1000 ) < parryChance )
						{
							damage = 0;
							getDef = 0;

							// Play parrying FX
							Effects->PlayStaticAnimation( ourTarg, 0x37b9, 10, 16 );
						}
					}
				}
			}

			// No shield, no weapon parry, no wrestling parry - armor needs to take the brunt of damage!
			if( damage > 0 && getDef == 0 )
			{
				getDef = HalfRandomNum( CalcDef( ourTarg, hitLoc, doArmorDamage, PHYSICAL ));
			}
			break;
		}
		case POISON:		//	POISON Damage
			damageModifier = ( CalcDef( ourTarg, hitLoc, doArmorDamage, damageType ) / 100 );
			damage = static_cast<SI16>( std::round(( static_cast<R32>( baseDamage ) - ( static_cast<R32>( baseDamage ) * damageModifier ))));
			break;
		default:			//	Elemental damage
			getDef = HalfRandomNum( CalcDef( ourTarg, hitLoc, doArmorDamage, damageType ));
			break;
	}

	if( getDef > 0 )
	{
		damage -= static_cast<R32>(( static_cast<R32>( getDef ) * static_cast<R32>( attSkill )) / 750 );
	}

	return static_cast<SI16>( std::round( damage ));
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::CalcDamage()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate damage based on hit location, damage bonuses, defense modifiers
//o------------------------------------------------------------------------------------------------o
SI16 CHandleCombat::CalcDamage( CChar *mChar, CChar *ourTarg, UI08 getFightSkill, UI08 hitLoc )
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
	{
		return damage;
	}
	else
	{
		damage = 0;
	}

	const SI16 baseDamage = CalcAttackPower( mChar, true );

	if( baseDamage == -1 )  // No damage if weapon breaks
		return 0;

	damage = ApplyDamageBonuses( PHYSICAL, mChar, ourTarg, getFightSkill, hitLoc, baseDamage );

	if( damage < 1 )
		return 0;

	damage = ApplyDefenseModifiers( PHYSICAL, mChar, ourTarg, getFightSkill, hitLoc, damage, true );

	if( damage <= 0 )
	{
		damage = RandomNum( 0, 4 );
	}

	// Half remaining damage by 2 if LBR (Pub15) or earlier
	if( cwmWorldState->ServerData()->ExpansionCoreShardEra() <= ER_LBR )
	{
		damage /= 2;
	}

	// Divide damage dealt by NPCs to players by NPCDAMAGERATE value in uox.ini
	if( mChar->IsNpc() && !ourTarg->IsNpc() )
	{
		damage /= cwmWorldState->ServerData()->CombatNpcDamageRate();
	}

	return damage;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::HandleCombat()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles combat related stuff during combat loop
//o------------------------------------------------------------------------------------------------o
bool CHandleCombat::HandleCombat( CSocket *mSock, CChar& mChar, CChar *ourTarg )
{
	const UI16 ourDist			= GetDist( &mChar, ourTarg );
	//Attacker Skill values
	CItem *mWeapon				= GetWeapon( &mChar );
	const UI08 getFightSkill	= GetCombatSkill( mWeapon );
	const UI16 attackSkill		= std::min( 1000, static_cast<SI32>( mChar.GetSkill( getFightSkill )));

	//Defender Skill values
	CItem *defWeapon			= GetWeapon( ourTarg );
	const UI08 getTargetSkill	= GetCombatSkill( defWeapon );
	const UI16 defendSkill		= std::min( 1000, static_cast<SI32>( ourTarg->GetSkill( getTargetSkill )));

	bool checkDist = ( ourDist <= 1 && abs( mChar.GetZ() - ourTarg->GetZ() ) <= 15 );

	// Trigger onSwing for scripts attached to attacker
	std::vector<UI16> scriptTriggers = mChar.GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			if( toExecute->OnSwing( mWeapon, &mChar, ourTarg ) == 0 )
			{
				return true;
			}
		}
	}

	// Trigger onSwing for scripts attached to attacker's weapon
	if( mWeapon != nullptr )
	{
		std::vector<UI16> weaponScriptTriggers = mWeapon->GetScriptTriggers();
		for( auto scriptTrig : weaponScriptTriggers )
		{
			cScript *toExecute = JSMapping->GetScript( scriptTrig );
			if( toExecute != nullptr )
			{
				if( toExecute->OnSwing( mWeapon, &mChar, ourTarg ) == 0 )
				{
					return true;
				}
			}
		}
	}

	// Allow longer range in combat based on weapon's maxRange
	if( !checkDist && mWeapon != nullptr && ValidateObject( mWeapon ) && mWeapon->GetMaxRange() > 1 )
	{
		// Check line of sight and Z differences if weapon's max range is higher than 1 tile
		checkDist = ( ourDist <= mWeapon->GetMaxRange() && abs( mChar.GetZ() - ourTarg->GetZ() ) <= 15 && LineOfSight( mSock, &mChar, ourTarg->GetX(), ourTarg->GetY(), ( ourTarg->GetZ() + 15 ), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false ));
	}

	if( checkDist )
	{
		CPFightOccurring tSend( mChar, ( *ourTarg ));
		for( auto &tSock : FindNearbyPlayers( &mChar ))
		{
			if( tSock )
			{
				tSock->Send(&tSend);
			}
		}

		if( mChar.IsNpc() )
		{
			if( mChar.GetNpcWander() != WT_FLEE && mChar.GetNpcWander() != WT_SCARED )
			{
				UI08 charDir = Movement->Direction( &mChar, ourTarg->GetX(), ourTarg->GetY() );
				if( mChar.GetDir() != charDir && charDir < 8 )
				{
					mChar.SetDir( charDir );
				}
			}
		}

		if( getFightSkill == ARCHERY && mWeapon != nullptr )
		{
			// If amount of time since character last moved is less than the minimum delay for shooting after coming to a halt, return
			if(( cwmWorldState->GetUICurrentTime() - mChar.LastMoveTime() ) < static_cast<UI32>( cwmWorldState->ServerData()->CombatArcheryShootDelay() * 1000 ))
				return false;

			UI16 ammoId = mWeapon->GetAmmoId();
			UI16 ammoHue = mWeapon->GetAmmoHue();
			UI16 ammoFX = mWeapon->GetAmmoFX();
			UI16 ammoFXHue = mWeapon->GetAmmoFXHue();
			UI16 ammoFXRender = mWeapon->GetAmmoFXRender();

			if( mChar.IsNpc() || ( ammoId != 0 && DeleteItemAmount( &mChar, 1, ammoId,  ammoHue ) == 1 ))
			{
				PlaySwingAnimations( &mChar );
				//Effects->PlayMovingAnimation( &mChar, ourTarg, ammoFX, 0x08, 0x00, 0x00, static_cast<UI32>( ammoFXHue ), static_cast<UI32>( ammoFXRender ));
				Effects->PlayMovingAnimation( mChar.GetX(), mChar.GetY(), mChar.GetZ() + 5, ourTarg->GetX(), ourTarg->GetY(), ourTarg->GetZ(), ammoFX, 0x08, 0x00, 0x00, static_cast<UI32>( ammoFXHue ), static_cast<UI32>( ammoFXRender ));
			}
			else
			{
				if( mSock != nullptr )
				{
					mChar.SetTimer( tCHAR_TIMEOUT, BuildTimeValue( GetCombatTimeout( &mChar )));
					mSock->SysMessage( 309 ); // You are out of ammunitions!
				}
				return false;
			}
		}
		else
		{
			PlaySwingAnimations( &mChar );
		}

		SI16 staminaToLose = cwmWorldState->ServerData()->CombatAttackStamina();
		if( staminaToLose && ( !mChar.IsGM() && !mChar.IsCounselor() ))
		{
			mChar.IncStamina( staminaToLose );
		}

		const UI16 getDefTactics = ourTarg->GetSkill( TACTICS );
		bool skillPassed = false;

		// Do a skill check so the fight skill is increased
		Skills->CheckSkill( &mChar, getFightSkill, 0, std::min( 1000, static_cast<SI32>(( getDefTactics * 1.25 ) + 100 )));

		// Calculate Hit Chance
		R32 hitChance = 0;
		switch( cwmWorldState->ServerData()->ExpansionCombatHitChance() )
		{
			case ER_T2A: // T2A - The Second Age
			case ER_UOR: // UOR - Renaissance
			case ER_TD: // TD - Third Dawn
			case ER_LBR: // LBR - Lord Blackthorn's Revenge (Publish 15)
				// FORMULA: ( Attacker's skill + 50 / ((defender's skill + 50 )* 2 )) * 100
				hitChance = static_cast<R32>((( static_cast<R64>( attackSkill ) + 500.0 ) / (( static_cast<R64>( defendSkill ) + 500.0 ) * 2.0 )) * 100.0 );
				if( hitChance < 0 )
				{
					hitChance = 0;
				}
				else if( hitChance > 100 )
				{
					hitChance = 100;
				}

				// Bonus to hit chance for archery skill since Pub 5/UOR
				if( cwmWorldState->ServerData()->ExpansionCoreShardEra() >= ER_UOR && getFightSkill == ARCHERY )
				{
					hitChance += cwmWorldState->ServerData()->CombatArcheryHitBonus();
				}
				break;
			case ER_AOS: // AoS - Age of Shadows
			case ER_SE: // SE - Samurai Empire
			case ER_ML: // ML - Mondain's Legacy
			case ER_SA: // SA - Stygian Abyss
			case ER_HS: // HS - High Seas
			case ER_TOL: // ToL - Time of Legends
			default:
				// FORMULA: Hit Chance% = (( [Attacker's Combat Ability + 20] * [100% + Attacker's Hit Chance Increase] ) divided by
				//	 ( [Defender's Combat Ability + 20] * [100% + Defender's Defense Chance Increase] * 2 )) * 100
				// For AoS and higher, always give attacker at least 2% chance to hit
				R32 attHitChanceBonus = 0;
				R32 defDefenseChanceBonus = 0;
				R32 maxAttHitChanceBonus = 45;
				if( cwmWorldState->ServerData()->ExpansionCombatHitChance() >= ER_SA && mChar.GetBodyType() == BT_GARGOYLE )
				{
					// If attacker is a Gargoyle player, and ExpansionCombatHitChance is ER_SA or higher, use 50 as hitchance bonus cap instead of 45
					maxAttHitChanceBonus = 50;
				}
				
				// Fetch bonuses to hitChance/defenseChance from AoS item properties, when implemented
				//attHitChanceBonus = GetAttackerHitChanceBonus();
				//defDefenseChanceBonus = GetDefenderDefenseChanceBonus();

				R32 attackerHitChance = ( static_cast<R32>( attackSkill / 10 ) + 20 ) * ( 100 + std::min( attHitChanceBonus, static_cast<R32>( maxAttHitChanceBonus )));
				R32 defenderDefenseChance = ( static_cast<R32>( defendSkill / 10 ) + 20 ) * ( 100 + std::min( defDefenseChanceBonus, static_cast<R32>( 45 )));
				hitChance = ( attackerHitChance / ( defenderDefenseChance * 2 )) * 100;

				// Always leave at least 2% chance to hit
				if( hitChance < 2 )
				{
					hitChance = 2;
				}
				else if( hitChance > 100 )
				{
					hitChance = 100;
				}
				break;
		}

		skillPassed = ( RandomNum( 1, 100 ) <= hitChance );

		if( !skillPassed )
		{
			// It's a miss!
			if( getFightSkill == ARCHERY && mWeapon->GetAmmoId() != 0 && !RandomNum( 0, 2 ))
			{
				UI16 ammoId = mWeapon->GetAmmoId();
				UI16 ammoHue = mWeapon->GetAmmoHue();
				Items->CreateItem( nullptr, ourTarg, ammoId, 1, ammoHue, OT_ITEM, false );
			}

			PlayMissedSoundEffect( &mChar );
		}
		else
		{
			// It's a hit!
			CSocket *targSock = ourTarg->GetSocket();

			Skills->CheckSkill( &mChar, TACTICS, 0, 1000 );
			Skills->CheckSkill( ourTarg, TACTICS, 0, 1000 );

			switch( ourTarg->GetId() )
			{
				case 0x025E:	// elf/human/garg female
				case 0x0191:
				case 0x029B:	Effects->PlaySound( ourTarg, RandomNum( 0x014B, 0x014F ));				break;
				case 0x025D:	// elf/human/garg male
				case 0x0190:
				case 0x029A:	Effects->PlaySound( ourTarg, RandomNum( 0x0155, 0x0158 ));				break;
				default:
				{
					UI16 toPlay = cwmWorldState->creatures[ourTarg->GetId()].GetSound( SND_DEFEND );
					if( toPlay != 0x00 )
					{
						Effects->PlaySound( ourTarg, toPlay );
					}
					break;
				}
			}

			UI08 poisonStrength = mChar.GetPoisonStrength();
			if( poisonStrength && ourTarg->GetPoisoned() < poisonStrength )
			{
				if((( getFightSkill == FENCING || getFightSkill == SWORDSMANSHIP ) && !RandomNum( 0, 2 )) || mChar.IsNpc() )
				{
					auto doPoison = true;
					if( !mChar.IsNpc() && cwmWorldState->ServerData()->YoungPlayerSystem() && !ourTarg->IsNpc() && ourTarg->GetAccount().wFlags.test( AB_FLAGS_YOUNG ))
					{
						doPoison = false;
						if( targSock != nullptr )
						{
							targSock->SysMessage( 18735 ); // You would have been poisoned, were you not new to the land of Britannia. Be careful in the future.
						}
					}
					else if( !mChar.IsNpc() && cwmWorldState->ServerData()->YoungPlayerSystem() && !mChar.IsNpc() && mChar.GetAccount().wFlags.test( AB_FLAGS_YOUNG ) )
					{
						doPoison = false;
						if( mSock != nullptr )
						{
							ourTarg->TextMessage( mSock, 18738, TALK, false ); // * The poison seems to have no effect. *
						}
					}

					if( doPoison )
					{
						// Apply poison on target
						ourTarg->SetPoisoned( poisonStrength );

						// Set time until next time poison "ticks"
						ourTarg->SetTimer( tCHAR_POISONTIME, BuildTimeValue( static_cast<R32>( GetPoisonTickTime( poisonStrength ))));

						// Set time until poison wears off completely
						ourTarg->SetTimer( tCHAR_POISONWEAROFF, ourTarg->GetTimer( tCHAR_POISONTIME ) + ( 1000 * GetPoisonDuration( poisonStrength ))); //wear off starts after poison takes effect

						if( targSock != nullptr )
						{
							targSock->SysMessage( 282 ); // You have been poisoned!
						}
					}
				}
			}

			// Calculate damage
			const UI08 hitLoc = CalculateHitLoc();
			UI16 ourDamage = CalcDamage( &mChar, ourTarg, getFightSkill, hitLoc );
			if( ourDamage > 0 )
			{
				// Show hit messages, if enabled
				DoHitMessage( &mChar, ourTarg, hitLoc, ourDamage );

				for( auto scriptTrig : scriptTriggers )
				{
					cScript *toExecute = JSMapping->GetScript( scriptTrig );
					if( toExecute != nullptr )
					{
						toExecute->OnCombatHit( &mChar, ourTarg );
					}
				}

				// Interrupt Spellcasting
				if( !ourTarg->IsNpc() && targSock != nullptr )
				{
					if( ourTarg->IsCasting() && targSock->CurrentSpellType() == 0 )
					{
						ourTarg->StopSpell();
						ourTarg->SetFrozen( false );
						ourTarg->Dirty( UT_UPDATE );
						targSock->SysMessage( 306 ); // Your concentration has been broken.
					}
				}
				// Reactive Armor
				if( ourTarg->GetReactiveArmour() )
				{
					SI32 retDamage = static_cast<SI32>( ourDamage * ( ourTarg->GetSkill( MAGERY ) / 2000.0 ));
					if( ourTarg->Damage( ourDamage - retDamage, PHYSICAL, &mChar ))
					{
						if( ourTarg->IsNpc() && !mChar.IsNpc() )
						{
							// Divide reactive damage dealt by NPCs to players by NPCDAMAGERATE value in uox.ini
							retDamage /= cwmWorldState->ServerData()->CombatNpcDamageRate();
						}
						mChar.Damage( retDamage, PHYSICAL, &mChar );
						Effects->PlayStaticAnimation( ourTarg, 0x374A, 0, 15 );
					}
				}
				else
				{
					[[maybe_unused]] bool retVal = ourTarg->Damage( ourDamage, PHYSICAL, &mChar, true );
				}
			}
			if( cwmWorldState->creatures[mChar.GetId()].IsHuman() )
			{
				PlayHitSoundEffect( &mChar, mWeapon );
			}

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

		// Refresh aggressor flag timestamp, if it exists
		if( mChar.CheckAggressorFlag( ourTarg->GetSerial() ))
		{
			mChar.UpdateAggressorFlagTimestamp( ourTarg->GetSerial() );
		}
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	QuickSwitch()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Allows beneficial spells to be cast on self during combat
//o------------------------------------------------------------------------------------------------o
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

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::CastSpell()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles spellcasting during combat
//o------------------------------------------------------------------------------------------------o
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
			{
				mChar->SetSpellCast( spellNum );
			}
			else
			{
				return false;
			}
			break;
		case 11:
			if( mChar->GetPoisoned() > 0 )
			{
				QuickSwitch( mChar, ourTarg, spellNum );
			}
			else
			{
				return false;
			}
			break;
		case 29:
			if( mChar->GetMaxHP() > mChar->GetHP() )
			{
				QuickSwitch( mChar, ourTarg, spellNum );
			}
			else
			{
				return false;
			}
			break;
		case 36:
			if( !mChar->IsTempReflected() && !mChar->IsPermReflected() )
			{
				mChar->SetSpellCast( spellNum );
			}
			else
			{
				return false;
			}
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

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::HandleNPCSpellAttack()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles the casting of spells by NPCs during combat
//o------------------------------------------------------------------------------------------------o
void CHandleCombat::HandleNPCSpellAttack( CChar *npcAttack, CChar *cDefend, UI16 playerDistance )
{
	if( !npcAttack->GetCanAttack() || npcAttack->IsEvading() )
		return;

	if( npcAttack->GetTimer( tNPC_SPATIMER ) <= cwmWorldState->GetUICurrentTime() )
	{
		if( playerDistance <= cwmWorldState->ServerData()->CombatMaxSpellRange() )
		{
			SI16 spattacks = npcAttack->GetSpAttack();
			if( spattacks <= 0 )
				return;

			if( spattacks > 8 )
			{
				spattacks = 8;
			}

			npcAttack->StopSpell();

			UI16 offensiveWeight = 0;

			// Establish a simple baseline for letting NPC determine whether to cast offensive or defensive spells
			// The higher the NPC's current health percentage, the more likely they will be to cast offensive spells
			// The lower their health percentage, the more likely they will be to cast defensive spells
			offensiveWeight = static_cast<UI16>( std::round(( npcAttack->GetHP() * 100 ) / npcAttack->GetMaxHP() ));

			UI16 rndNum = RandomNum( 1, 100 );

			// Cut weighting towards offensive spells in half if NPC is poisoned; we want him to cast cures!
			if( npcAttack->GetPoisoned() > 0 )
			{
				offensiveWeight /= 2;
			}

			// Refresh aggressor flag timestamp, if it exists
			if( npcAttack->CheckAggressorFlag( cDefend->GetSerial() ))
			{
				npcAttack->UpdateAggressorFlagTimestamp( cDefend->GetSerial() );
			}

			bool monsterAreaCastAnim = false;
			if( offensiveWeight > rndNum )
			{
				// Cast offensive spells
				switch(( RandomNum( static_cast<SI16>( 0 ), spattacks ) + 1 ))
				{
					case 1: // Spell Circle 1 - Offensive Spells
						switch( RandomNum( 1, 4 ))
						{
							case 1:		CastSpell( npcAttack, cDefend, 1 );			break;	// Clumsy
							case 2:		CastSpell( npcAttack, cDefend, 3 );			break;	// Feeble Mind
							case 3:		CastSpell( npcAttack, cDefend, 5 );			break;	// Magic Arrow
							case 4: 	CastSpell( npcAttack, cDefend, 8 );			break;	// weaken
						}
						break;
					case 2:
						CastSpell( npcAttack, cDefend, 12 ); // Harm
						break;
					case 3:
						CastSpell( npcAttack, cDefend, 18 ); // Fireball
						break;
					case 4:
						CastSpell( npcAttack, cDefend, 30 ); // Lightning
						break;
					case 5:
						switch( RandomNum( 1, 2 ))
						{
							case 1:		
								if( npcAttack->GetIntelligence() > cDefend->GetIntelligence() )
								{
									CastSpell( npcAttack, cDefend, 37 ); // Mind Blast
									break;
								}
								else
								{
									// Avoid dumb caster NPCs from killing themselves, and fallthrough to next spell in list instead
									[[fallthrough]];
								}
							case 2:		CastSpell( npcAttack, cDefend, 38 );		break;	// Paralyze
							//case 3:		CastSpell( npcAttack, cDefend, 33 );		break;	// Blade Spirits
						}
						break;
					case 6:
						if( cDefend->IsNpc() )
						{
							if( cDefend->GetTimer( tNPC_SUMMONTIME ) > 0 && cDefend->IsDispellable() && cDefend->GetNpcAiType() != AI_GUARD )
							{
								if( npcAttack->GetSkill( MAGERY ) > RandomNum( 1, 1000 ))
								{
									CastSpell( npcAttack, cDefend, 41 );
									break;
								}
							}
						}
						switch( RandomNum( 1, 2 ))
						{
							case 1:		CastSpell( npcAttack, cDefend, 42 );		break;	// Energy Bolt
							case 2:		CastSpell( npcAttack, cDefend, 43 );		break;	// Explosion
						}
						break;
					case 7:
						switch( RandomNum( 1, 3 ))
						{
							case 1:		CastSpell( npcAttack, cDefend, 49 );		monsterAreaCastAnim = true;	break;	// Chain Lightning
							case 2:		CastSpell( npcAttack, cDefend, 51 );		monsterAreaCastAnim = true;	break;	// Flamestrike
							case 3:		CastSpell( npcAttack, cDefend, 55 );		monsterAreaCastAnim = true;	break;	// Meteor Swarm
						}
						break;
					case 8:
						CastSpell( npcAttack, cDefend, 57 ); // Earthquake
						monsterAreaCastAnim = true;
						// case 2:		break; //CastSpell( npcAttack, cDefend, 58 ); 	// Energy Vortex
						break;
					// This is where dragon attacks go eventually when the NPC DFNs are fixed...*/
					/*
					case 9:
					case 10:
					case 11:
					*/
						break;
					default:
						break;
				}
			}
			else
			{
				// If situation not too bad yet, let's go for some buffs/debuffs
				if( offensiveWeight > rndNum / 1.5 )
				{
					// Cast buffs/debuffs
					switch(( RandomNum( static_cast<SI16>( 0 ), spattacks ) + 1 ))
					{
						case 1:
							[[fallthrough]];
						case 2:
							switch( RandomNum( 1, 4 ))
							{
								case 1:		CastSpell( npcAttack, cDefend, 1 );			break;	// Clumsy
								case 2:		CastSpell( npcAttack, cDefend, 3 );			break;	// Feeble Mind
								case 3:		CastSpell( npcAttack, cDefend, 7 );			break;	// Reactive Armor (personal...switch target temporarily to self and then back after casting)
								case 4:		CastSpell( npcAttack, cDefend, 8 );			break; 	// weaken
							}
							break;
						case 3:
							if( cDefend->GetPoisoned() == 0 )
							{
								CastSpell( npcAttack, cDefend, 20 ); // Poison
								break;
							}
							if( spattacks + 1 == 3 )
								break;
							[[fallthrough]]; // Fallthrough if target is poisoned already and if NPC can cast spells from next circle
						case 4:
							switch( RandomNum( 1, 2 ))
							{
								case 1:		CastSpell( npcAttack, cDefend, 27 );		break;	// Curse
								case 2:		CastSpell( npcAttack, cDefend, 31 );		break;	// Mana Drain
							}
							break;
						case 5:
							CastSpell( npcAttack, cDefend, 36 ); // Magic Reflection (personal...no target switching required)
							break;
						case 6:
							CastSpell( npcAttack, cDefend, 46 ); // Mass Curse
							monsterAreaCastAnim = true;
							break;
						case 7:
							if( spattacks + 1 == 7 )
								break; // Don't fallthrough if NPC cannot cast spells from next circle!
							[[fallthrough]];
						case 8:
							CastSpell( npcAttack, cDefend, 53 ); // Mana Vampire
							break;
						default:
							break;
					}
				}
				else
				{
					// Critical health levels! Let's focus on heals and cures
					switch(( RandomNum( static_cast<SI16>( 0 ), spattacks ) + 1 ))
					{
						case 1:
							if(( spattacks + 1 >= 2 ) && !CastSpell( npcAttack, cDefend, 11 )) // Cast cure if we're poisoned, else move on to Heal
							{
								CastSpell( npcAttack, cDefend, 4 );	// Heal (personal...switch target temporarily to self and then back after casting)
							}
							break;
						case 2:
							[[fallthrough]];
						case 3:
							if( !CastSpell( npcAttack, cDefend, 11 )) // Cast cure if we're poisoned, else move on to Heal
							{
								CastSpell( npcAttack, cDefend, 4 );	// Heal (personal...switch target temporarily to self and then back after casting)
							}
							break;
						case 4:
						case 5:
						case 6:
						case 7:
						case 8:
							CastSpell( npcAttack, cDefend, 29 ); // Greater Healing (personal...switch target temporarily to self and then back after casting)
							break;
						default:
							break;
					}
				}
			}

			if( npcAttack->GetSpellCast() != -1 )
			{
				if( npcAttack->GetBodyType() == BT_HUMAN || npcAttack->GetBodyType() == BT_ELF || npcAttack->GetBodyType() == BT_GARGOYLE )
				{
					// "Human" casting
					CSpellInfo curSpellCasting = Magic->spells[npcAttack->GetSpellCast()];
					Effects->PlaySpellCastingAnimation( npcAttack, curSpellCasting.Action(), false, false ); // do the action
					npcAttack->SetTimer( tNPC_MOVETIME, BuildTimeValue( 0.75 ));
				}
				else
				{
					// Monster casting
					Effects->PlaySpellCastingAnimation( npcAttack, 0, true, monsterAreaCastAnim );
					npcAttack->SetTimer( tNPC_MOVETIME, BuildTimeValue( 0.75 ));
				}
				Magic->CastSpell( nullptr, npcAttack );
			}

			//Adjust spellDelay based on UOX.INI setting:
			SI08 spellDelay = floor(( npcAttack->GetSpDelay() / cwmWorldState->ServerData()->NPCSpellCastSpeed() ) + 0.5 );

			npcAttack->SetTimer( tNPC_SPATIMER, BuildTimeValue( spellDelay ));
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::GetCombatTimeout()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate delay between attacks in combat
//o------------------------------------------------------------------------------------------------o
R32 CHandleCombat::GetCombatTimeout( CChar *mChar )
{
	SI16 statOffset = 0;
	if( cwmWorldState->ServerData()->CombatAttackSpeedFromStamina() )
	{
		statOffset = mChar->GetStamina();
	}
	else
	{
		statOffset = mChar->GetDexterity();
	}

	R32 getDelay = 0;
	if( mChar->IsNpc() ) // Allow NPCs more speed based off of available stamina than players
	{
		getDelay = static_cast<R32>( static_cast<R32>( std::min( statOffset, static_cast<SI16>( 300 ))) + 100 );
	}
	else
	{
		getDelay = static_cast<R32>( static_cast<R32>( std::min( statOffset, static_cast<SI16>( 100 ))) + 100 );
	}

	SI32 getOffset	= 0;
	SI32 baseValue	= 15000;

	CChar *ourTarg = mChar->GetTarg();

	CItem *mWeapon = GetWeapon( mChar );
	if( ValidateObject( mWeapon ))
	{
		if( mWeapon->GetSpeed() == 0 )
		{
			mWeapon->SetSpeed( 35 );
		}
		getOffset = mWeapon->GetSpeed();
	}
	else
	{
		if( mChar->GetSkill( WRESTLING ) < 800 )
		{
			getOffset = (( static_cast<SI32>( mChar->GetSkill( WRESTLING ) / 200 )) * 5 ) + 30;
		}
		else
		{
			getOffset = 50;
		}
	}

	//Allow faster strikes on fleeing targets
	if( ValidateObject( ourTarg ))
	{
		if( ourTarg->GetNpcWander() == WT_FLEE || ourTarg->GetNpcWander() == WT_SCARED )
		{
			baseValue = 10000;
		}
	}

	R32 globalAttackSpeed = cwmWorldState->ServerData()->GlobalAttackSpeed(); //Defaults to 1.0

	getDelay = ( baseValue / ( getDelay * getOffset )) / globalAttackSpeed;
	return getDelay;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::InvalidateAttacker()
//|	Date		-	3rd July, 2001
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Resets the attacker attack so that it cancels attack setup.
//o------------------------------------------------------------------------------------------------o
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
			{
				ourTarg = nullptr;
			}

			// -1 == event doesn't exist, or returned -1
			// 0 == script returned false, 0, or nothing - don't execute hard code
			// 1 == script returned true or 1
			if( toExecute->OnCombatEnd( mChar, ourTarg ) == 0 )	// if it exists and we don't want hard code, return
			{
				return;
			}
		}
	}

	// Do the same for the opposite party in combat
	if( ValidateObject( ourTarg ))
	{
		scriptTriggers.clear();
		scriptTriggers = ourTarg->GetScriptTriggers();
		for( auto scriptTrig : scriptTriggers )
		{
			cScript *toExecute = JSMapping->GetScript( scriptTrig );
			if( toExecute != nullptr )
			{
				//Check if ourTarg validates as another character. If not, don't use
				if( !ValidateObject( ourTarg ))
				{
					ourTarg = nullptr;
				}

				// -1 == event doesn't exist, or returned -1
				// 0 == script returned false, 0, or nothing - don't execute hard code
				// 1 == script returned true or 1
				if( toExecute->OnCombatEnd( ourTarg, mChar ) == 0 )	// if it exists and we don't want hard code, return
				{
					return;
				}
			}
		}
	}

	if( mChar->IsNpc() && mChar->GetNpcAiType() == AI_GUARD )
	{
		mChar->SetTimer( tNPC_SUMMONTIME, BuildTimeValue( 20 ));
		mChar->SetNpcWander( WT_FREE );
		if( mChar->GetMounted() )
		{
			mChar->SetTimer( tNPC_MOVETIME, BuildTimeValue( mChar->GetMountedWalkingSpeed() ));
		}
		else
		{
			mChar->SetTimer( tNPC_MOVETIME, BuildTimeValue( mChar->GetWalkingSpeed() ));
		}
		mChar->TextMessage( nullptr, 281, TALK, false ); // Thou have suffered thy punishment, scoundrel.
	}

	if( ValidateObject( ourTarg ) && ourTarg->GetTarg() == mChar )
	{
		ourTarg->SetTarg( nullptr );
		if( ourTarg->GetAttacker() == mChar )
		{
			ourTarg->SetAttacker( nullptr );
		}

		// Only remove aggressor flags if aggressor timer has expired
		[[maybe_unused]] bool retVal1 = ourTarg->CheckAggressorFlag( mChar->GetSerial() );
		[[maybe_unused]] bool retVal2 = mChar->CheckAggressorFlag( ourTarg->GetSerial() );

		if( ourTarg->IsAtWar() && ourTarg->IsNpc() )
		{
			ourTarg->ToggleCombat(); // What if character is fighting OTHER characters than mChar as well?
		}
	}
	mChar->SetTarg( nullptr );
	CChar *attAttacker = mChar->GetAttacker();
	if( ValidateObject( attAttacker ))
	{
		if( attAttacker->GetAttacker() == mChar )
		{
			attAttacker->SetAttacker( nullptr );
		}

		// Check and remove aggressor flags if aggressor timer has expired
		[[maybe_unused]] bool retVal1 = attAttacker->CheckAggressorFlag( mChar->GetSerial() );
		[[maybe_unused]] bool retVal2 = mChar->CheckAggressorFlag( attAttacker->GetSerial() );
	}
	mChar->SetAttacker( nullptr );
	if( mChar->IsAtWar() && mChar->IsNpc() )
	{
		mChar->ToggleCombat(); // What if mChar is in combat with other characters?
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::Kill()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle death during combat
//o------------------------------------------------------------------------------------------------o
void CHandleCombat::Kill( CChar *mChar, CChar *ourTarg )
{
	if( ValidateObject( mChar ))
	{
		if( mChar->GetNpcAiType() == AI_GUARD && ourTarg->IsNpc() )
		{
			Effects->PlayCharacterAnimation( ourTarg, ( RandomNum( 0, 1 ) ? ACT_DIE_BACKWARD : ACT_DIE_FORWARD ), 0, 6 ); // 0x15 or 0x16, either is 6 frames long
			Effects->PlayDeathSound( ourTarg );

			ourTarg->Delete(); // NPC was killed by a Guard, don't leave a corpse behind
			if( mChar->IsAtWar() )
			{
				mChar->ToggleCombat();
			}
			return;
		}

		if( mChar->GetNpcAiType() == AI_ANIMAL && !mChar->IsTamed() )
		{
			mChar->SetHunger( 6 );

			if( ourTarg->IsNpc() )
			{
				// Note: What if ourTarg is a non-human NPC which doesn't have death animation 0x15?
				Effects->PlayCharacterAnimation( ourTarg, 0x15 );
				Effects->PlayDeathSound( ourTarg );

				ourTarg->Delete(); // eating animals, don't give body
				if( mChar->IsAtWar() )
				{
					mChar->ToggleCombat();
				}
				return;
			}
		}
		InvalidateAttacker( mChar );
	}
	HandleDeath( ourTarg, mChar );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::CombatLoop()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles the main combat loop for characters
//o------------------------------------------------------------------------------------------------o
void CHandleCombat::CombatLoop( CSocket *mSock, CChar& mChar )
{
	CChar *ourTarg = mChar.GetTarg();
	if( ourTarg == nullptr )
		return;

	if( mChar.IsNpc() && mChar.GetTimer( tNPC_EVADETIME ) > cwmWorldState->GetUICurrentTime() )
	{
#if defined( UOX_DEBUG_MODE ) && defined( DEBUG_COMBAT )
		Console.Warning( "DEBUG: Exited CombatLoop for NPC due to EvadeTimer.\n" );
#endif
		return;
	}

	bool combatHandled = false;
	if( mChar.GetTimer( tCHAR_TIMEOUT ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
	{
		bool validTarg = false;
		if( !mChar.IsDead() && ValidateObject( ourTarg ) && !ourTarg->IsFree() && ( ourTarg->IsNpc() || IsOnline(( *ourTarg ))))
		{
			if( !ourTarg->IsInvulnerable() && !ourTarg->IsDead() && ourTarg->GetNpcAiType() != AI_PLAYERVENDOR && ourTarg->GetVisible() == VT_VISIBLE && !ourTarg->IsEvading() )
			{
				if( CharInRange( &mChar, ourTarg ))
				{
					CItem *equippedWeapon = GetWeapon( &mChar );
					if( ValidateObject( equippedWeapon ) && equippedWeapon->GetMaxRange() > 1 )
					{
						if( GetDist( &mChar, ourTarg ) > equippedWeapon->GetMaxRange() )
						{
							return;
						}
					}

					validTarg = true;
					if( mChar.IsNpc() && mChar.GetSpAttack() > 0 && mChar.GetMana() > 0 && !RandomNum( 0, 2 ))
					{
						HandleNPCSpellAttack( &mChar, ourTarg, GetDist( &mChar, ourTarg ));
					}
					else if(( mChar.IsNpc() && mChar.IsAtWar() ) || ( !mChar.IsNpc() && !mChar.IsPassive() )) // Don't trigger for players who are marked as passive combatants
					{
						combatHandled = HandleCombat( mSock, mChar, ourTarg );
					}

					if( mChar.IsAtWar() && mChar.GetNpcWander() != WT_SCARED && ( !ValidateObject( ourTarg->GetTarg() ) || !ObjInRange( ourTarg, ourTarg->GetTarg(), DIST_INRANGE )))		//if the defender is swung at, and they don't have a target already, set this as their target
					{
						StartAttack( ourTarg, &mChar );
					}
				}
				else if( mChar.IsNpc() && mChar.GetNpcAiType() == AI_GUARD && mChar.GetRegion()->IsGuarded() && cwmWorldState->ServerData()->GuardsStatus() )
				{
					validTarg = true;
					mChar.SetLocation( ourTarg );
					Effects->PlaySound( &mChar, 0x01FE );
					Effects->PlayStaticAnimation( &mChar, 0x372A, 0x09, 0x06 );
					mChar.TextMessage( nullptr, 1616, TALK, true ); // Halt, scoundrel!
				}
				else
				{
					if( !ObjInRange( &mChar, ourTarg, DIST_COMBATRESETRANGE ))
					{
						InvalidateAttacker( &mChar );
					}
					else
					{
						validTarg = true;
					}
				}
			}
		}
		if( !validTarg )
		{
			mChar.SetTarg( nullptr );
			mChar.SetAttacker( nullptr );
			if( mChar.IsAtWar() && mChar.IsNpc() )
			{
				mChar.ToggleCombat();
			}
		}
		else if( combatHandled )
		{
			mChar.SetTimer( tCHAR_TIMEOUT, BuildTimeValue( GetCombatTimeout( &mChar )));
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::SpawnGuard()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle spawning a guard in guarded areas
//|					NEED TO REWORK FOR REGIONAL GUARD STUFF
//o------------------------------------------------------------------------------------------------o
auto  CHandleCombat::SpawnGuard( CChar *mChar, CChar *targChar, SI16 x, SI16 y, SI08 z ) -> void
{
	if( !ValidateObject( mChar ) || !ValidateObject( targChar ))
		return;

	if( targChar->IsDead() || targChar->IsInvulnerable() )
		return;

	auto targRegion = mChar->GetRegion();

	if( !targRegion->IsGuarded() || !cwmWorldState->ServerData()->GuardsStatus() )
		return;

	auto reUseGuard		= false;
	CChar *getGuard		= nullptr;
	auto toCheck		= MapRegion->GetMapRegion( mChar );
	if( toCheck )
	{
		auto regChars = toCheck->GetCharList();
		for( const auto &getGuard : regChars->collection() )
		{
			if( ValidateObject( getGuard ))
			{
				if( getGuard->IsNpc() && ( getGuard->GetNpcAiType() == AI_GUARD ))
				{
					if( !ValidateObject( getGuard->GetTarg() ) || getGuard->GetTarg() == targChar  )
					{
						if( CharInRange( getGuard, targChar ))
						{
							reUseGuard = true;
						}
					}
					else if( getGuard->GetTarg() == targChar )
					{
						return;
					}
				}
			}
		}
		
	}
	
	// 1/13/2003 - Fix for JSE NocSpawner
	if( !reUseGuard )
	{
		getGuard = targRegion->GetRandomGuard();
		if( ValidateObject( getGuard ))
		{
			getGuard->SetLocation( x, y, z, mChar->WorldNumber(), mChar->GetInstanceId() );
			Npcs->PostSpawnUpdate( getGuard );
		}
		else
		{
			return;
		}
	}
	//
	if( ValidateObject( getGuard ))
	{
		getGuard->AddAggressorFlag( targChar->GetSerial() );
		getGuard->SetAttacker( targChar );
		getGuard->SetTarg( targChar );
		getGuard->SetNpcWander( WT_FREE );
		if( !getGuard->IsAtWar() )
		{
			getGuard->ToggleCombat();
		}

		if( reUseGuard )
		{
			getGuard->SetLocation( targChar );
		}
		else
		{
			if( getGuard->GetMounted() )
			{
				getGuard->SetTimer( tNPC_MOVETIME, BuildTimeValue( getGuard->GetMountedWalkingSpeed() ));
			}
			else
			{
				getGuard->SetTimer( tNPC_MOVETIME, BuildTimeValue( getGuard->GetWalkingSpeed() ));
			}
			getGuard->SetTimer( tNPC_SUMMONTIME, BuildTimeValue( 25 ));

			Effects->PlaySound( getGuard, 0x01FE );
			Effects->PlayStaticAnimation( getGuard, 0x372A, 0x09, 0x06 );

			getGuard->TextMessage( nullptr, 313, TALK, true ); // Thou shalt regret thine actions, swine!
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHandleCombat::PetGuardAttack()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get the pet guarding an item / character and have him attack
//|					the person using / attacking the item / character
//o------------------------------------------------------------------------------------------------o
void CHandleCombat::PetGuardAttack( CChar *mChar, CChar *owner, CBaseObject *guarded, CChar *petGuard )
{
	if( !ValidateObject( mChar ) || !ValidateObject( guarded ))
		return;

	if( mChar->IsCounselor() || mChar->IsGM() )
		return;

	if( !ValidateObject( petGuard ))
	{
		// No pet guard was passed into function, so let's look for one ourself
		petGuard = Npcs->GetGuardingFollower( owner, guarded );
	}

	if( ValidateObject( petGuard ) && ObjInRange( mChar, petGuard, cwmWorldState->ServerData()->CombatMaxRange() ))
	{
		if( mChar->GetSerial() == petGuard->GetOwner() )
			return;

		if( !Npcs->CheckPetFriend( mChar, petGuard ))
		{
			AttackTarget( petGuard, mChar );
		}
		else
		{
			CSocket *oSock = owner->GetSocket();
			if( oSock != nullptr )
			{
				oSock->SysMessage( 1629 ); // Your pet refuses to attack his friend!
			}
		}
	}
}
