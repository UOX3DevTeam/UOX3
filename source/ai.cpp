//o------------------------------------------------------------------------------------------------o
//|	File		-	ai.cpp
//|	Date		-	Pre-1999
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	All AI-Related Code goes here (Should eventually be
//|							moved out to JavaScript code).
//o------------------------------------------------------------------------------------------------o
//| Changes		-	Version History
//|
//|						1.1			 		30th December, 2003
//|						Updated to move the majority of AI functionality
//|						into several smaller functions. Should simplify the
//|						process of moving it out to JavaScript in the future.
//o------------------------------------------------------------------------------------------------o
#include "uox3.h"
#include "cRaces.h"
#include "cEffects.h"
#include "regions.h"
#include "combat.h"
#include "CJSMapping.h"
#include "cScript.h"
#include "Dictionary.h"
#include "StringUtility.hpp"

#undef DBGFILE
#define DBGFILE "ai.cpp"


//o------------------------------------------------------------------------------------------------o
//|	Function	-	IsValidAttackTarget()
//|	Date		-	12/30/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if cTarget is a valid attack target for mChar
//o------------------------------------------------------------------------------------------------o
bool IsValidAttackTarget( CChar& mChar, CChar *cTarget )
{
	if( ValidateObject( cTarget ) && &mChar != cTarget )
	{
		if( mChar.IsNpc() && cTarget->IsNpc() )
		{
			//We don't want NPCs to attack one another if either of them are water-walking creatures
			//as they can't reach one another in any case
			bool targetWaterWalk = cwmWorldState->creatures[cTarget->GetId()].IsWater();
			bool attackerWaterWalk = cwmWorldState->creatures[mChar.GetId()].IsWater();
			if(( attackerWaterWalk && !targetWaterWalk ) || ( !attackerWaterWalk && targetWaterWalk ))
			{
				return false;
			}
		}

		if( cTarget->IsInvulnerable() || cTarget->IsDead() || cTarget->GetVisible() != VT_VISIBLE || cTarget->IsEvading() )
		{
			return false;
		}

		if( ObjInRange( &mChar, cTarget, cwmWorldState->ServerData()->CombatMaxRange() ))
		{
			if( LineOfSight( nullptr, (&mChar), cTarget->GetX(), cTarget->GetY(), ( cTarget->GetZ() + 15 ), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false ))
			{
				if( IsOnline(( *cTarget )) || cTarget->IsNpc() )
				{
					return true;
				}
			}
		}
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CheckForValidOwner()
//|	Date		-	12/30/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if mChar owns cTarget or vise-versa
//o------------------------------------------------------------------------------------------------o
bool CheckForValidOwner( CChar& mChar, CChar *cTarget )
{
	if( ValidateObject( mChar.GetOwnerObj() ) && mChar.GetOwnerObj() == cTarget )
		return true;

	if( ValidateObject( cTarget->GetOwnerObj() ) && cTarget->GetOwnerObj() == &mChar )
		return true;

	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	HandleGuardAI()
//|	Date		-	12/30/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles Guard AI Type
//o------------------------------------------------------------------------------------------------o
void HandleGuardAI( CChar& mChar )
{
	if( mChar.IsAtWar() )
		return;

	for( auto &MapArea : MapRegion->PopulateList( &mChar ))
	{
		if( MapArea == nullptr )
			continue;

		auto regChars = MapArea->GetCharList();
		for( const auto &tempChar : regChars->collection() )
		{
			if( IsValidAttackTarget( mChar, tempChar ))
			{
				if( !tempChar->IsDead() && ( tempChar->IsCriminal() || tempChar->IsMurderer() ))
				{
					Combat->AttackTarget( &mChar, tempChar );
					mChar.TextMessage( nullptr, 313, TALK, true );
					return;
				}
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	HandleFighterAI()
//|	Date		-	06/15/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles Fighter AI Type
//o------------------------------------------------------------------------------------------------o
void HandleFighterAI( CChar& mChar )
{
	if( mChar.IsAtWar() )
		return;

	// Fetch scriptTriggers attached to mChar
	std::vector<UI16> scriptTriggers = mChar.GetScriptTriggers();

	for( auto &MapArea : MapRegion->PopulateList( &mChar ))
	{
		if( MapArea == nullptr )
			continue;

		auto regChars = MapArea->GetCharList();
		for( const auto &tempChar : regChars->collection() )
		{
			if( IsValidAttackTarget( mChar, tempChar ))
			{
				// Loop through scriptTriggers attached to mChar and see if any have onCombatTarget event
				// This event will override target selection entirely
				bool invalidTarget = false;
				for( auto scriptTrig : scriptTriggers )
				{
					auto toExecute = JSMapping->GetScript( scriptTrig );
					if( toExecute )
					{
						SI08 retVal = toExecute->OnAICombatTarget( &mChar, tempChar );
						if( retVal == -1 )
						{
							// No such event found, or returned -1
							continue;
						}
						else if( retVal == 0 )
						{
							// Invalid target! But look through other scripts with event first
							invalidTarget = true;
							continue;
						}
						else if( retVal == 1 )
						{
							// Valid target!
							Combat->AttackTarget( &mChar, tempChar );
							return;
						}
					}
				}
				if( !invalidTarget )
				{
					RaceRelate raceComp = Races->Compare( tempChar, &mChar );
					if( !tempChar->IsDead() && ( tempChar->IsCriminal() || tempChar->IsMurderer() || raceComp <= RACE_ENEMY ))
					{
						if( RandomNum( 1, 100 ) < 85 ) // 85% chance to attack current target, 15% chance to pick another
						{
							Combat->AttackTarget( &mChar, tempChar );
							return;
						}
					}
				}
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	HandleHealerAI()
//|	Date		-	12/30/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles Wandering Healer AI Type (Raises Innocents)
//o------------------------------------------------------------------------------------------------o
void HandleHealerAI( CChar& mChar )
{
	for( auto &mSock : FindNearbyPlayers( &mChar, DIST_NEARBY ))
	{
		CChar *realChar = mSock->CurrcharObj();
		CMultiObj *multiObj = realChar->GetMultiObj();
		if( realChar->IsDead() && ( !ValidateObject( multiObj ) || multiObj->GetOwner() == realChar->GetSerial() ))
		{
			if( LineOfSight( mSock, realChar, mChar.GetX(), mChar.GetY(), ( mChar.GetZ() + 15 ), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false ))
			{
				if( realChar->IsMurderer() )
				{
					mChar.TextMessage( nullptr, 322, TALK, true ); // I will not give life to a scoundrel like thee!
				}
				else if( realChar->IsCriminal() )
				{
					mChar.TextMessage( nullptr, 770, TALK, true ); // I will nay give life to thee for thou art a criminal!
				}
				else if( realChar->IsInnocent() )
				{
					if( mChar.GetBodyType() == BT_GARGOYLE || cwmWorldState->ServerData()->ForceNewAnimationPacket() )
					{
						Effects->PlayNewCharacterAnimation( &mChar, N_ACT_SPELL, S_ACT_SPELL_TARGET ); // Action 0x0b, subAction 0x00
					}
					else
					{
						UI16 castAnim = static_cast<UI16>( cwmWorldState->creatures[mChar.GetId()].CastAnimTargetId() );
						UI08 castAnimLength = cwmWorldState->creatures[mChar.GetId()].CastAnimTargetLength();

						// Play cast anim, but fallback to default attack anim (0x04) with anim length of 4 frames if no cast anim was defined in creatures.dfn
						Effects->PlayCharacterAnimation( &mChar, ( castAnim != 0 ? castAnim : 0x04 ), 0, ( castAnimLength != 0 ? castAnimLength : 4 ));
					}

					NpcResurrectTarget( realChar );
					Effects->PlayStaticAnimation( realChar, 0x376A, 0x09, 0x06 );
					mChar.TextMessage( nullptr, ( 316 + RandomNum( 0, 4 )), TALK, false );
				}
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	HandleEvilHealerAI()
//|	Date		-	12/30/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles Evil Healer AI Type (Raises Murderers)
//o------------------------------------------------------------------------------------------------o
void HandleEvilHealerAI( CChar& mChar )
{
	for( auto &mSock : FindNearbyPlayers( &mChar, DIST_NEARBY ))
	{
		CChar *realChar	= mSock->CurrcharObj();
		CMultiObj *multiObj = realChar->GetMultiObj();
		if( realChar->IsDead() && ( !ValidateObject( multiObj ) || multiObj->GetOwner() == realChar->GetSerial() ))
		{
			if( LineOfSight( mSock, realChar, mChar.GetX(), mChar.GetY(), ( mChar.GetZ() + 15 ), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false ))
			{
				if( realChar->IsMurderer() )
				{
					if( mChar.GetBodyType() == BT_GARGOYLE || cwmWorldState->ServerData()->ForceNewAnimationPacket() )
					{
						Effects->PlayNewCharacterAnimation( &mChar, N_ACT_SPELL, S_ACT_SPELL_TARGET ); // Action 0x0b, subAction 0x00
					}
					else
					{
						UI16 castAnim = static_cast<UI16>( cwmWorldState->creatures[mChar.GetId()].CastAnimTargetId() );
						UI08 castAnimLength = cwmWorldState->creatures[mChar.GetId()].CastAnimTargetLength();

						// Play cast anim, but fallback to default attack anim (0x04) with anim length of 4 frames if no cast anim was defined in creatures.dfn
						Effects->PlayCharacterAnimation( &mChar, ( castAnim != 0 ? castAnim : 0x04 ), 0, ( castAnimLength != 0 ? castAnimLength : 4 ));
					}

					NpcResurrectTarget( realChar );
					Effects->PlayStaticAnimation( realChar, 0x3709, 0x09, 0x19 ); //Flamestrike effect
					mChar.TextMessage( nullptr, ( 323 + RandomNum( 0, 4 )), TALK, false );
				}
				else
				{
					mChar.TextMessage( nullptr, 329, TALK, true ); // I despise all things good. I shall not give thee another chance!
				}
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	HandleEvilAI()
//|	Date		-	12/30/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles Evil creature AI Type (Attacks innocents)
//o------------------------------------------------------------------------------------------------o
auto HandleEvilAI( CChar& mChar ) -> void
{
	if( mChar.IsAtWar() )
		return;

	// Fetch scriptTriggers attached to mChar
	std::vector<UI16> scriptTriggers = mChar.GetScriptTriggers();

	for( auto &MapArea : MapRegion->PopulateList( &mChar ))
	{
		if( MapArea == nullptr )
			continue;

		auto regChars = MapArea->GetCharList();
				
		// Chance to reverse list of chars
		if( RandomNum( 0, 1 ))
		{
			regChars->Reverse();
		}
		for( const auto &tempChar : regChars->collection() )
		{
			if( IsValidAttackTarget( mChar, tempChar ) && !CheckForValidOwner( mChar, tempChar ))
			{
				// Loop through scriptTriggers attached to mChar and see if any have onCombatTarget event
				// This event will override target selection entirely
				bool invalidTarget = false;
				for( auto scriptTrig : scriptTriggers )
				{
					auto toExecute = JSMapping->GetScript( scriptTrig );
					if( toExecute == nullptr )
						continue;

					SI08 retVal = toExecute->OnAICombatTarget( &mChar, tempChar );
					if( retVal != -1 )
					{
						if( retVal == 0 )
						{
							// Invalid target! But look through other scripts with event first
							invalidTarget = true;
						}
						else if( retVal == 1 )
						{
							// Valid target!
							Combat->AttackTarget( &mChar, tempChar );
							return;
						}
					}
				}

				if( invalidTarget )
					continue;

				if( tempChar->GetNpcAiType() != AI_HEALER_G )
				{
					if( cwmWorldState->creatures[tempChar->GetId()].IsAnimal() )
					{
						if( !cwmWorldState->ServerData()->CombatMonstersVsAnimals() )
						{
							continue;
						}
						else if( cwmWorldState->ServerData()->CombatAnimalsAttackChance() < RandomNum( 1, 1000 ))
						{
							continue;
						}
					}
					if( !( mChar.GetRace() != 0 && mChar.GetRace() == tempChar->GetRace() && RandomNum( 1, 100 ) > 1 )) // 1% chance of turning on own race
					{
						RaceRelate raceComp = Races->Compare( tempChar, &mChar );
						if( raceComp < RACE_ALLY ) // Allies
						{
							if( !(( tempChar->GetNpcAiType() == AI_EVIL || tempChar->GetNpcAiType() == AI_EVIL_CASTER ) && raceComp > RACE_ENEMY ))
							{
								if( RandomNum( 1, 100 ) < 85 )
								{ // 85% chance to attack current target, 15% chance to pick another
									Combat->AttackTarget( &mChar, tempChar );
									return;
								}
							}
						}
					}
				}
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	HandleChaoticAI()
//|	Date		-	12/30/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles Chaotic creature AI Type (Attacks everything)
//o------------------------------------------------------------------------------------------------o
auto HandleChaoticAI( CChar& mChar ) -> void
{
	if( mChar.IsAtWar() )
		return;

	// Fetch scriptTriggers attached to mChar
	std::vector<UI16> scriptTriggers = mChar.GetScriptTriggers();

	for( auto &MapArea : MapRegion->PopulateList( &mChar ))
	{
		if( MapArea == nullptr )
			continue;

		auto regChars = MapArea->GetCharList();
		for( const auto &tempChar : regChars->collection() )
		{
			if( IsValidAttackTarget( mChar, tempChar ) && !CheckForValidOwner( mChar, tempChar ))
			{
				// Loop through scriptTriggers attached to mChar and see if any have onCombatTarget event
				// This event will override target selection entirely
				bool invalidTarget = false;
				for( auto scriptTrig : scriptTriggers )
				{
					cScript *toExecute = JSMapping->GetScript( scriptTrig );
					if( toExecute == nullptr )
						continue;

					SI08 retVal = toExecute->OnAICombatTarget( &mChar, tempChar );
					if( retVal!= -1 )
					{
						if( retVal == 0 )
						{
							// Invalid target! But look through other scripts with event first
							invalidTarget = true;
						}
						else if( retVal == 1 )
						{
							// Valid target!
							Combat->AttackTarget( &mChar, tempChar );
							return;
						}
					}
				}
				if( !invalidTarget )
				{
					if( RandomNum( 1, 100 ) < 85 ) // 85% chance to attack current target, 15% chance to pick another
					{
						Combat->AttackTarget( &mChar, tempChar );
						return;
					}
				}
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	HandleAnimalAI()
//|	Date		-	21. Feb, 2006
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles Animal AI Type
//o------------------------------------------------------------------------------------------------o
auto HandleAnimalAI( CChar& mChar ) -> void
{
	if( mChar.IsAtWar() )
		return;

	const SI08 hunger = mChar.GetHunger();
	if( hunger > 3 )
		return;

	// Fetch scriptTriggers attached to mChar
	std::vector<UI16> scriptTriggers = mChar.GetScriptTriggers();
		
	for( auto &MapArea : MapRegion->PopulateList( &mChar ))
	{
		if( MapArea == nullptr )
			continue;

		auto regChars = MapArea->GetCharList();
		for( const auto &tempChar : regChars->collection() )
		{
			if( IsValidAttackTarget( mChar, tempChar ))
			{
				// Loop through scriptTriggers attached to mChar and see if any have onCombatTarget event
				// This event will override target selection entirely
				bool invalidTarget = false;
				for( auto scriptTrig : scriptTriggers )
				{
					cScript *toExecute = JSMapping->GetScript( scriptTrig );
					if( toExecute == nullptr )
						continue;

					SI08 retVal = toExecute->OnAICombatTarget( &mChar, tempChar );
					if( retVal != -1 )
					{
						if( retVal == 0 )
						{
							// Invalid target! But look through other scripts with event first
							invalidTarget = true;
						}
						else if( retVal == 1 )
						{
							// Valid target!
							Combat->AttackTarget( &mChar, tempChar );
							return;
						}
					}
				}
				if( invalidTarget )
					continue;

				auto raceComp = Races->Compare( tempChar, &mChar );
				if( raceComp <= RACE_ENEMY || ( cwmWorldState->creatures[tempChar->GetId()].IsAnimal() && tempChar->GetNpcAiType() == AI_NONE )
					|| ( hunger <= 1 && ( tempChar->GetNpcAiType() == AI_ANIMAL || cwmWorldState->creatures[tempChar->GetId()].IsHuman() )))
				{
					if( RandomNum( 1, 100 ) > 95 ) // 5% chance (per AI cycle to attack tempChar)
					{
						Combat->AttackTarget( &mChar, tempChar );
						return;
					}
				}
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CheckAI()
//|	Date		-	12/30/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Switch to easily handle AI types
//o------------------------------------------------------------------------------------------------o
void CheckAI( CChar& mChar )
{
	CChar *realChar = nullptr;
	switch( mChar.GetNpcAiType() )
	{
		case AI_NONE:													// No special AI, default NPC behavior
		case AI_DUMMY:													// Passive AI - doesn't attack nor fight back
		case AI_BANKER:													// Banker
		case AI_STABLEMASTER:											// Stablemaster
		case AI_PLAYERVENDOR:											// Player Vendors.
			break;	// No AI for these special NPC's.
		case AI_HEALER_G:		HandleHealerAI( mChar );		break;	// Good Healers
		case AI_EVIL_CASTER:	[[fallthrough]];						// Evil Caster NPCs, use same AI as AI_EVIL
		case AI_EVIL:			HandleEvilAI( mChar );			break;	// Evil NPC's
		case AI_GUARD:			HandleGuardAI( mChar );			break;	// Guard
		case AI_CASTER:			[[fallthrough]];						// Caster - same as AI_FIGHTER, but keep their distance
		case AI_FIGHTER:		HandleFighterAI( mChar );		break;	// Fighter - same as guard, without teleporting & yelling "HALT!"
		case AI_ANIMAL:			HandleAnimalAI( mChar );		break;	// Hungry animals
		case AI_CHAOTIC:		HandleChaoticAI( mChar );		break;	// Energy Vortex / Blade Spirit
		case AI_HEALER_E:		HandleEvilHealerAI( mChar );	break;	// Evil Healers
		case AI_PET_GUARD:												// Pet Guarding AI
			realChar = mChar.GetOwnerObj();
			if( !ValidateObject( realChar ))
			{
				mChar.SetNPCAiType( AI_NONE );
				return;
			}
			if( ValidateObject( realChar->GetTarg() ))
			{
				Combat->AttackTarget( &mChar, realChar->GetTarg() );
			}
			break;
		default:
		{
			std::string mCharName = GetNpcDictName( &mChar );
			Console.Error( oldstrutil::format( " CheckAI() Error npc %s(0x%X) has invalid AI type %i", mCharName.c_str(), mChar.GetSerial(), mChar.GetNpcAiType() ));	//Morrolan
			return;
		}
	}
}

