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
bool isValidAttackTarget( CChar *mChar, CChar *cTarget )
{
	if( ValidateObject( cTarget ) && mChar != cTarget )
	{
		if( cTarget->IsInvulnerable() || cTarget->IsDead() || cTarget->GetVisible() != VT_VISIBLE  )
			return false;
		if( objInRange( mChar, cTarget, cwmWorldState->ServerData()->CombatMaxRange() ) )
		{
			if( isOnline( cTarget ) || cTarget->IsNpc() )
				return true;
		}
	}
	return false;
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool checkForValidOwner( CChar *mChar, CChar *cTarget )
//|	Date			-	12/30/2003
//|	Developers		-	Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Check if mChar owns cTarget or vise-versa
//o--------------------------------------------------------------------------o
bool checkForValidOwner( CChar *mChar, CChar *cTarget )
{
	if( ValidateObject( mChar->GetOwnerObj() ) && mChar->GetOwnerObj() == cTarget )
		return true;
	if( ValidateObject( cTarget->GetOwnerObj() ) && cTarget->GetOwnerObj() == mChar )
		return true;
	return false;
}

//o--------------------------------------------------------------------------o
//|	Function		-	void HandleGuardAI( CChar *mChar )
//|	Date			-	12/30/2003
//|	Developers		-	Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Handles Guard AI Type
//o--------------------------------------------------------------------------o
void HandleGuardAI( CChar *mChar )
{
	if( !mChar->IsAtWar() )
	{
		REGIONLIST nearbyRegions = MapRegion->PopulateList( mChar );
		for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
		{
			SubRegion *MapArea = (*rIter);
			if( MapArea == NULL )	// no valid region
				continue;
			CDataList< CChar * > *regChars = MapArea->GetCharList();
			regChars->Push();
			for( CChar *tempChar = regChars->First(); !regChars->Finished(); tempChar = regChars->Next() )
			{
				if( isValidAttackTarget( mChar, tempChar ) )
				{
					if( tempChar->GetNPCAiType() == aiEVIL || tempChar->GetNPCAiType() == aiCHAOTIC ||
						tempChar->IsCriminal() || tempChar->IsMurderer() )
					{
						Combat->AttackTarget( mChar, tempChar );
						mChar->talkAll( 313, true );
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
//|	Function		-	void HandleHealerAI( CChar *mChar )
//|	Date			-	12/30/2003
//|	Developers		-	Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Handles Wandering Healer AI Type (Raises Innocents)
//o--------------------------------------------------------------------------o
void HandleHealerAI( CChar *mChar )
{
	SOCKLIST nearbyChars = FindNearbyPlayers( mChar, DIST_NEARBY );
	for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
	{
		cSocket *mSock	= (*cIter);
		CChar *realChar = mSock->CurrcharObj();
		if( LineOfSight( mSock, realChar, mChar->GetX(), mChar->GetY(), mChar->GetZ(), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING ) )
		{
			if( realChar->IsDead() && realChar->IsInnocent() && !realChar->IsCriminal() && !realChar->IsMurderer() )
			{
				Effects->PlayCharacterAnimation( mChar, 0x10 );
				NpcResurrectTarget( realChar );
				Effects->PlayStaticAnimation( realChar, 0x376A, 0x09, 0x06 );
				mChar->talkAll( ( 316 + RandomNum( 0, 4 ) ), false );
			}
			else if( realChar->IsDead() && realChar->IsMurderer() )
				mChar->talkAll( 322, true );
			else if( realChar->IsDead() && realChar->IsCriminal() )
				mChar->talkAll( 770, true );
		}
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	void HandleEvilHealerAI( CChar *mChar )
//|	Date			-	12/30/2003
//|	Developers		-	Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Handles Evil Healer AI Type (Raises Murderers)
//o--------------------------------------------------------------------------o
void HandleEvilHealerAI( CChar *mChar )
{
	SOCKLIST nearbyChars = FindNearbyPlayers( mChar, DIST_NEARBY );
	for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
	{
		cSocket *mSock	= (*cIter);
		CChar *realChar	= mSock->CurrcharObj();
		if( realChar->IsDead() && realChar->IsMurderer() )
		{
			Effects->PlayCharacterAnimation( mChar, 0x10 );
			NpcResurrectTarget( realChar );
			Effects->PlayStaticAnimation( realChar, 0x3709, 0x09, 0x19 ); //Flamestrike effect
			mChar->talkAll( ( 323 + RandomNum( 0, 4 ) ), false ); 
		}
		else if( !realChar->IsMurderer() && realChar->IsDead() )
			mChar->talkAll( 329, true );
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	void HandleEvilAI( CChar *mChar )
//|	Date			-	12/30/2003
//|	Developers		-	Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Handles Evil creature AI Type (Attacks innocents)
//o--------------------------------------------------------------------------o
void HandleEvilAI( CChar *mChar )
{
	if( !mChar->IsAtWar() )
	{
		REGIONLIST nearbyRegions = MapRegion->PopulateList( mChar );
		for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
		{
			SubRegion *MapArea = (*rIter);
			if( MapArea == NULL )	// no valid region
				continue;
			CDataList< CChar * > *regChars = MapArea->GetCharList();
			regChars->Push();
			for( CChar *tempChar = regChars->First(); !regChars->Finished(); tempChar = regChars->Next() )
			{
				if( isValidAttackTarget( mChar, tempChar ) && !checkForValidOwner( mChar, tempChar ) )
				{
					if( tempChar->GetNPCAiType() == aiEVIL || tempChar->GetNPCAiType() == aiHEALER_G )
						continue;
					if( !cwmWorldState->ServerData()->CombatMonstersVsAnimals() && cwmWorldState->creatures[tempChar->GetID()].IsAnimal() )
						continue;
					if( cwmWorldState->ServerData()->CombatMonstersVsAnimals() && cwmWorldState->creatures[tempChar->GetID()].IsAnimal() && RandomNum( 1, 100 ) > cwmWorldState->ServerData()->CombatAnimalsAttackChance() )
						continue;
					if( mChar->GetRace() != 0 && mChar->GetRace() == tempChar->GetRace() && RandomNum( 0, 100 ) >= 10 )	// 10% chance of turning on own race
						continue;
					SI08 raceComp = Races->Compare( tempChar, mChar );
					if( raceComp == 2 )	// Allies
						continue;
					Combat->AttackTarget( mChar, tempChar );
					regChars->Pop();	// restore before returning
					return;
				}
			}
			regChars->Pop();
		}
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	void HandleChaoticAI( CChar *mChar )
//|	Date			-	12/30/2003
//|	Developers		-	Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Handles Chaotic creature AI Type (Attacks everything)
//o--------------------------------------------------------------------------o
void HandleChaoticAI( CChar *mChar )
{
	if( !mChar->IsAtWar() )
	{
		REGIONLIST nearbyRegions = MapRegion->PopulateList( mChar );
		for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
		{
			SubRegion *MapArea = (*rIter);
			if( MapArea == NULL )	// no valid region
				continue;
			CDataList< CChar * > *regChars = MapArea->GetCharList();
			regChars->Push();
			for( CChar *tempChar = regChars->First(); !regChars->Finished(); tempChar = regChars->Next() )
			{
				if( isValidAttackTarget( mChar, tempChar ) && !checkForValidOwner( mChar, tempChar ) )
				{
					Combat->AttackTarget( mChar, tempChar );
					regChars->Pop();
					return;
				}
			}
			regChars->Pop();
		}
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	void CheckAI( CChar *i )
//|	Date			-	12/30/2003
//|	Developers		-	Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Switch to easily handle AI types
//o--------------------------------------------------------------------------o
void CheckAI( CChar *i )
{
	if( !ValidateObject( i ) )
		return;

	CChar *realChar			= NULL;
	switch( i->GetNPCAiType() )
	{
	case aiNOAI:									break;
	case aiHEALER_G:		HandleHealerAI( i );	break;	// Good Healers
	case aiEVIL:			HandleEvilAI( i );		break;	// Evil NPC's
	case aiGUARD:			HandleGuardAI( i );		break;	// Guard
	case aiBANKER:									break;  // Banker
	case aiPLAYERVENDOR:							break;  // Player Vendors.
	case aiPET_GUARD:										// Pet Guarding AI
		realChar = i->GetOwnerObj();
		if( !ValidateObject( realChar ) )
		{
			i->SetNPCAiType( aiNOAI );
			return;
		}
		if( ValidateObject( realChar->GetTarg() ) )
			Combat->AttackTarget( i, realChar->GetTarg() );
		break;
	case aiCHAOTIC:			HandleChaoticAI( i );		break;	// Energy Vortex / Blade Spirit
	case aiHEALER_E:		HandleEvilHealerAI( i );	break;	//Evil Healers
	default:
		Console.Error( 2, " cCharStuff::CheckAI-> Error npc %u (0x%X) has invalid AI type %i", i, i->GetSerial(), i->GetNPCAiType());	//Morrolan
		return;
	}
}

}
