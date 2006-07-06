//o--------------------------------------------------------------------------o
//|	File			-	ai.cpp
//|	Date			-	Pre-1999
//|	Developers		-	Unknown
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	All AI-Related Code goes here (Should eventually be
//|							moved out to JavaScript code).
//o--------------------------------------------------------------------------o
//| Modifications	-	Version History
//|
//|							1.1			Zane		30th December, 2003
//|							Updated to move the majority of AI functionality
//|							into several smaller functions. Should simplify the
//|							process of moving it out to JavaScript in the future.
//o--------------------------------------------------------------------------o
#include "uox3.h"
#include "cRaces.h"
#include "cEffects.h"
#include "regions.h"
#include "combat.h"

#undef DBGFILE
#define DBGFILE "ai.cpp"

namespace UOX
{

//o--------------------------------------------------------------------------o
//|	Function		-	bool isValidAttackTarget( CChar *mChar, CChar *cTarget )
//|	Date			-	12/30/2003
//|	Developers		-	Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Check if cTarget is a valid attack target for mChar
//o--------------------------------------------------------------------------o
bool isValidAttackTarget( CChar& mChar, CChar *cTarget )
{
	if( ValidateObject( cTarget ) && &mChar != cTarget )
	{
		if( cTarget->IsInvulnerable() || cTarget->IsDead() || cTarget->GetVisible() != VT_VISIBLE  )
			return false;
		if( objInRange( &mChar, cTarget, cwmWorldState->ServerData()->CombatMaxRange() ) )
		{
			if( LineOfSight( NULL, (&mChar), cTarget->GetX(), cTarget->GetY(), ( cTarget->GetZ() + 15 ), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING ) )
			{
				if( isOnline( (*cTarget) ) || cTarget->IsNpc() )
					return true;
			}
		}
	}
	return false;
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool checkForValidOwner( CChar& mChar, CChar *cTarget )
//|	Date			-	12/30/2003
//|	Developers		-	Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Check if mChar owns cTarget or vise-versa
//o--------------------------------------------------------------------------o
bool checkForValidOwner( CChar& mChar, CChar *cTarget )
{
	if( ValidateObject( mChar.GetOwnerObj() ) && mChar.GetOwnerObj() == cTarget )
		return true;
	if( ValidateObject( cTarget->GetOwnerObj() ) && cTarget->GetOwnerObj() == &mChar )
		return true;
	return false;
}

//o--------------------------------------------------------------------------o
//|	Function		-	void HandleGuardAI( CChar& mChar )
//|	Date			-	12/30/2003
//|	Developers		-	Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Handles Guard AI Type
//o--------------------------------------------------------------------------o
void HandleGuardAI( CChar& mChar )
{
	if( !mChar.IsAtWar() )
	{
		REGIONLIST nearbyRegions = MapRegion->PopulateList( &mChar );
		for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
		{
			CMapRegion *MapArea = (*rIter);
			if( MapArea == NULL )	// no valid region
				continue;
			CDataList< CChar * > *regChars = MapArea->GetCharList();
			regChars->Push();
			for( CChar *tempChar = regChars->First(); !regChars->Finished(); tempChar = regChars->Next() )
			{
				if( isValidAttackTarget( mChar, tempChar ) )
				{
					if( !tempChar->IsDead() && ( tempChar->IsCriminal() || tempChar->IsMurderer() ) )
					{
						Combat->AttackTarget( &mChar, tempChar );
						mChar.TextMessage( NULL, 313, TALK, true );
						regChars->Pop();
						return;
					}
				}
			}
			regChars->Pop();
		}
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	void HandleFighterAI( CChar& mChar )
//|	Date			-	06/15/2005
//|	Developers		-	Xuri
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Handles Fighter AI Type
//o--------------------------------------------------------------------------o
void HandleFighterAI( CChar& mChar )
{
	if( !mChar.IsAtWar() )
	{
		REGIONLIST nearbyRegions = MapRegion->PopulateList( &mChar );
		for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
		{
			CMapRegion *MapArea = (*rIter);
			if( MapArea == NULL )	// no valid region
				continue;
			CDataList< CChar * > *regChars = MapArea->GetCharList();
			regChars->Push();
			for( CChar *tempChar = regChars->First(); !regChars->Finished(); tempChar = regChars->Next() )
			{
				if( isValidAttackTarget( mChar, tempChar ) )
				{
					if( !tempChar->IsDead() && ( tempChar->IsCriminal() || tempChar->IsMurderer() ) )
					{
						Combat->AttackTarget( &mChar, tempChar );
						regChars->Pop();
						return;
					}
				}
			}
			regChars->Pop();
		}
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	void HandleHealerAI( CChar& mChar )
//|	Date			-	12/30/2003
//|	Developers		-	Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Handles Wandering Healer AI Type (Raises Innocents)
//o--------------------------------------------------------------------------o
void HandleHealerAI( CChar& mChar )
{
	SOCKLIST nearbyChars = FindNearbyPlayers( &mChar, DIST_NEARBY );
	for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
	{
		CSocket *mSock	= (*cIter);
		CChar *realChar = mSock->CurrcharObj();
		if( realChar->IsDead() )
		{
			if( LineOfSight( mSock, realChar, mChar.GetX(), mChar.GetY(), ( mChar.GetZ() + 15 ), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING ) )
			{
				if( realChar->IsMurderer() )
					mChar.TextMessage( NULL, 322, TALK, true );
				else if( realChar->IsCriminal() )
					mChar.TextMessage( NULL, 770, TALK, true );
				else if( realChar->IsInnocent() )
				{
					Effects->PlayCharacterAnimation( &mChar, 0x10 );
					NpcResurrectTarget( realChar );
					Effects->PlayStaticAnimation( realChar, 0x376A, 0x09, 0x06 );
					mChar.TextMessage( NULL, ( 316 + RandomNum( 0, 4 ) ), TALK, false );
				}
			}
		}
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	void HandleEvilHealerAI( CChar& mChar )
//|	Date			-	12/30/2003
//|	Developers		-	Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Handles Evil Healer AI Type (Raises Murderers)
//o--------------------------------------------------------------------------o
void HandleEvilHealerAI( CChar& mChar )
{
	SOCKLIST nearbyChars = FindNearbyPlayers( &mChar, DIST_NEARBY );
	for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
	{
		CSocket *mSock	= (*cIter);
		CChar *realChar	= mSock->CurrcharObj();
		if( realChar->IsDead() )
		{
			if( LineOfSight( mSock, realChar, mChar.GetX(), mChar.GetY(), ( mChar.GetZ() + 15 ), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING ) )
			{
				if( realChar->IsMurderer() )
				{
					Effects->PlayCharacterAnimation( &mChar, 0x10 );
					NpcResurrectTarget( realChar );
					Effects->PlayStaticAnimation( realChar, 0x3709, 0x09, 0x19 ); //Flamestrike effect
					mChar.TextMessage( NULL, ( 323 + RandomNum( 0, 4 ) ), TALK, false ); 
				}
				else
					mChar.TextMessage( NULL, 329, TALK, true );
			}
		}
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	void HandleEvilAI( CChar& mChar )
//|	Date			-	12/30/2003
//|	Developers		-	Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Handles Evil creature AI Type (Attacks innocents)
//o--------------------------------------------------------------------------o
void HandleEvilAI( CChar& mChar )
{
	if( !mChar.IsAtWar() )
	{
		REGIONLIST nearbyRegions = MapRegion->PopulateList( &mChar );
		for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
		{
			CMapRegion *MapArea = (*rIter);
			if( MapArea == NULL )	// no valid region
				continue;
			CDataList< CChar * > *regChars = MapArea->GetCharList();
			regChars->Push();
			for( CChar *tempChar = regChars->First(); !regChars->Finished(); tempChar = regChars->Next() )
			{
				if( isValidAttackTarget( mChar, tempChar ) && !checkForValidOwner( mChar, tempChar ) )
				{
					if( tempChar->GetNPCAiType() == AI_EVIL || tempChar->GetNPCAiType() == AI_HEALER_G )
						continue;
					if( cwmWorldState->creatures[tempChar->GetID()].IsAnimal() )
					{
						if( !cwmWorldState->ServerData()->CombatMonstersVsAnimals() )
							continue;
						else if( cwmWorldState->ServerData()->CombatAnimalsAttackChance() < RandomNum( 1, 100 ) )
							continue;
					}
					if( mChar.GetRace() != 0 && mChar.GetRace() == tempChar->GetRace() && RandomNum( 0, 100 ) >= 10 )	// 10% chance of turning on own race
						continue;
					SI08 raceComp = Races->Compare( tempChar, &mChar );
					if( raceComp == 2 )	// Allies
						continue;
					Combat->AttackTarget( &mChar, tempChar );
					regChars->Pop();	// restore before returning
					return;
				}
			}
			regChars->Pop();
		}
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	void HandleChaoticAI( CChar& mChar )
//|	Date			-	12/30/2003
//|	Developers		-	Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Handles Chaotic creature AI Type (Attacks everything)
//o--------------------------------------------------------------------------o
void HandleChaoticAI( CChar& mChar )
{
	if( !mChar.IsAtWar() )
	{
		REGIONLIST nearbyRegions = MapRegion->PopulateList( &mChar );
		for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
		{
			CMapRegion *MapArea = (*rIter);
			if( MapArea == NULL )	// no valid region
				continue;
			CDataList< CChar * > *regChars = MapArea->GetCharList();
			regChars->Push();
			for( CChar *tempChar = regChars->First(); !regChars->Finished(); tempChar = regChars->Next() )
			{
				if( isValidAttackTarget( mChar, tempChar ) && !checkForValidOwner( mChar, tempChar ) )
				{
					Combat->AttackTarget( &mChar, tempChar );
					regChars->Pop();
					return;
				}
			}
			regChars->Pop();
		}
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	void HandleAnimalAI( CChar& mChar )
//|	Date			-	21. Feb, 2006
//|	Developers		-	grimson
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Handles Animal AI Type
//o--------------------------------------------------------------------------o
void HandleAnimalAI( CChar& mChar )
{
	if( !mChar.IsAtWar() )
	{
		REGIONLIST nearbyRegions = MapRegion->PopulateList( &mChar );
		for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
		{
			const SI08 hunger = mChar.GetHunger();
			if( hunger <= 4 )
			{
				CMapRegion *MapArea = (*rIter);
				if( MapArea == NULL )	// no valid region
					continue;
				CDataList< CChar * > *regChars = MapArea->GetCharList();
				regChars->Push();
				for( CChar *tempChar = regChars->First(); !regChars->Finished(); tempChar = regChars->Next() )
				{
					if( isValidAttackTarget( mChar, tempChar ) )
					{
						if( ( cwmWorldState->creatures[tempChar->GetID()].IsAnimal() && tempChar->GetNPCAiType() != AI_ANIMAL ) || hunger < 2  )
						{
							Combat->AttackTarget( &mChar, tempChar );
							regChars->Pop();
							return;
						}
					}
				}
				regChars->Pop();
			}
		}
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	void CheckAI( CChar& mChar )
//|	Date			-	12/30/2003
//|	Developers		-	Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Switch to easily handle AI types
//o--------------------------------------------------------------------------o
void CheckAI( CChar& mChar )
{
	CChar *realChar			= NULL;
	switch( mChar.GetNPCAiType() )
	{
	case AI_BANKER:													// Banker
	case AI_PLAYERVENDOR:											// Player Vendors.
	case AI_NONE:											break;	// No AI for these special NPC's.

	case AI_HEALER_G:		HandleHealerAI( mChar );		break;	// Good Healers
	case AI_EVIL:			HandleEvilAI( mChar );			break;	// Evil NPC's
	case AI_GUARD:			HandleGuardAI( mChar );			break;	// Guard
	case AI_FIGHTER:		HandleFighterAI( mChar );		break;	// Fighter - same as guard, without teleporting & yelling "HALT!"
	case AI_ANIMAL:			HandleAnimalAI( mChar );		break;	// Hungry animals
	case AI_CHAOTIC:		HandleChaoticAI( mChar );		break;	// Energy Vortex / Blade Spirit
	case AI_HEALER_E:		HandleEvilHealerAI( mChar );	break;	// Evil Healers
	case AI_PET_GUARD:												// Pet Guarding AI
		realChar = mChar.GetOwnerObj();
		if( !ValidateObject( realChar ) )
		{
			mChar.SetNPCAiType( AI_NONE );
			return;
		}
		if( ValidateObject( realChar->GetTarg() ) )
			Combat->AttackTarget( &mChar, realChar->GetTarg() );
		break;
	default:
		Console.Error( " CheckAI() Error npc %s(0x%X) has invalid AI type %i", mChar.GetName().c_str(), mChar.GetSerial(), mChar.GetNPCAiType() );	//Morrolan
		return;
	}
}

}
