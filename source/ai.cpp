//o--------------------------------------------------------------------------o
//|	File					-	ai.cpp
//|	Date					-	
//|	Developers		-	
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
#include "uox3.h"
#include "cRaces.h"
#include "targeting.h"
#include "cEffects.h"
#include "network.h"

#undef DBGFILE
#define DBGFILE "ai.cpp"

void cCharStuff::CheckAI( CChar *i )
{
	if( i == NULL ) 
		return;

	const SI16 combatRange = cwmWorldState->ServerData()->GetCombatMaxRange();
	UI08 worldNumber = i->WorldNumber();
	cSocket *mSock = NULL;
	CChar *realChar = NULL;
	if( cwmWorldState->GetNextNPCAITime() <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() ) 
	{
		switch( i->GetNPCAiType() )
		{
		case 1 : // Good Healers
			Network->PushConn();
			for( mSock = Network->FirstSocket(); !Network->FinishedSockets(); mSock = Network->NextSocket() )
			{
				realChar = mSock->CurrcharObj();
				bool inRange = objInRange(i,realChar,3);
				if( LineOfSight( mSock, realChar, i->GetX(), i->GetY(), i->GetZ(), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING ) )
				{
					if( realChar->IsDead() && realChar->IsInnocent() && inRange && !realChar->IsCriminal() && !realChar->IsMurderer() ) 
					{
						npcAction( i, 0x10 );
						Targ->NpcResurrectTarget( realChar );
						Effects->staticeffect( realChar, 0x376A, 0x09, 0x06 );
						npcTalkAll( i, ( 316 + RandomNum( 0, 4 ) ), false );
					} 
					else if( realChar->IsDead() && inRange && realChar->IsMurderer() )
						npcTalkAll( i, 322, true );
					else if( realChar->IsDead() && inRange && realChar->IsCriminal() )
						npcTalkAll( i, 770, true );
				}
			}
			Network->PopConn();
			break;
		case 666 : //Evil Healers
			Network->PushConn();
			for( mSock = Network->FirstSocket(); !Network->FinishedSockets(); mSock = Network->NextSocket() )
			{
				realChar = mSock->CurrcharObj();
				bool inRange=objInRange(i,realChar,3);
				if( realChar->IsDead() && inRange && realChar->IsMurderer() ) 
				{
					npcAction( i, 0x10 );
					Targ->NpcResurrectTarget( realChar );
					Effects->staticeffect( realChar, 0x3709, 0x09, 0x19 ); //Flamestrike effect
					npcTalkAll( i, ( 323 + RandomNum( 0, 4 ) ), false ); 
				} 
				else if( !realChar->IsMurderer() && realChar->IsDead() && inRange ) 
					npcTalkAll( i, 329, true );
			}
			Network->PopConn();
			break;
		case 2 : // Monsters, PK's - (stupid NPCs)
			if( !i->IsAtWar() ) 
			{
				int xOffset = MapRegion->GetGridX( i->GetX() );
				int yOffset = MapRegion->GetGridY( i->GetY() );
				for( SI08 counter1 = -1; counter1 <= 1; counter1++ )
				{
					for( SI08 counter2 = -1; counter2 <= 1; counter2++ )
					{
						SubRegion *MapArea = MapRegion->GetGrid( xOffset + counter1, yOffset + counter2, worldNumber );	// check 3 cols... do we really NEED to?
						if( MapArea == NULL )	// no valid region
							continue;
						CChar *tempChar;
						MapArea->PushChar();
						for( tempChar = MapArea->FirstChar(); !MapArea->FinishedChars(); tempChar = MapArea->GetNextChar() )
						{
							if( tempChar == NULL )
								continue;
							if( i == tempChar )
								continue;
							if( i->GetOwnerObj() != NULL && i->GetOwnerObj() == tempChar )
								continue;
							if( !objInRange( i, tempChar, combatRange) )
								continue;
							if( tempChar->IsInvulnerable() || tempChar->GetHidden() != 0 || tempChar->IsDead() )
								continue;
							if( !isOnline( tempChar ) && !tempChar->IsNpc() )
								continue;
							if( tempChar->GetNPCAiType() == 0x02 || tempChar->GetNPCAiType() == 0x01 )
								continue;
							if( !cwmWorldState->ServerData()->GetCombatMonstersVsAnimals() && creatures[tempChar->GetID()].IsAnimal() )
								continue;
							if( cwmWorldState->ServerData()->GetCombatMonstersVsAnimals() && creatures[tempChar->GetID()].IsAnimal() && RandomNum( 1, 100 ) > cwmWorldState->ServerData()->GetCombatAnimalsAttackChance() )
								continue;
							if( i->GetRace() != 0 && i->GetRace() == tempChar->GetRace() && RandomNum( 0, 100 ) >= 10 )	// 10% chance of turning on own race
								continue;
							int raceComp;
							raceComp = Races->Compare( tempChar, i );
							if( raceComp == 2 )	// Allies
								continue;
							npcAttackTarget( tempChar, i );
							MapArea->PopChar();	// restore before returning
							return;
						}
						MapArea->PopChar();
					}
				}
			}
			break;
		case 4 : // Guards
			if( !i->IsAtWar() ) 
			{
				int xOffset = MapRegion->GetGridX( i->GetX() );
				int yOffset = MapRegion->GetGridY( i->GetY() );
				for( SI08 counter1 = -1; counter1 <= 1; counter1++ )
				{
					for( SI08 counter2 = -1; counter2 <= 1; counter2++ )
					{
						SubRegion *MapArea = MapRegion->GetGrid( xOffset + counter1, yOffset + counter2, worldNumber );	// check 3 cols... do we really NEED to?
						if( MapArea == NULL )	// no valid region
							continue;
						CChar *tempChar;
						MapArea->PushChar();
						for( tempChar = MapArea->FirstChar(); !MapArea->FinishedChars(); tempChar = MapArea->GetNextChar() )
						{
							if( tempChar == NULL )
								continue;
							if( tempChar == i )
								continue;
							if( objInRange( i, tempChar, combatRange ) && !tempChar->IsInvulnerable() && !tempChar->IsDead() && tempChar->GetNPCAiType() == 0x02 ) 
							{
								npcAttackTarget( tempChar, i );
								npcTalkAll( i, 313, true );
								MapArea->PopChar();
								return;
							}
						}
						MapArea->PopChar();
					}
				}
			}
			break;
		case 30:
			if( !i->IsAtWar() )
			{
				int xOffset = MapRegion->GetGridX( i->GetX() );
				int yOffset = MapRegion->GetGridY( i->GetY() );
				for( SI08 counter1 = -1; counter1 <= 1; counter1++ )
				{
					for( SI08 counter2 = -1; counter2 <= 1; counter2++ )
					{
						SubRegion *MapArea = MapRegion->GetGrid( xOffset + counter1, yOffset + counter2, worldNumber );	// check 3 cols... do we really NEED to?
						if( MapArea == NULL )	// no valid region
							continue;
						CChar *tempChar;
						MapArea->PushChar();
						for( tempChar = MapArea->FirstChar(); !MapArea->FinishedChars(); tempChar = MapArea->GetNextChar() )
						{
							if( tempChar == NULL )
								continue;
							if( tempChar == i )
								continue;
							if( objInRange( i, tempChar, combatRange) && !tempChar->IsInvulnerable() && !tempChar->IsDead() && tempChar->GetNPCAiType() != 0x02 )
							{
								npcAttackTarget( tempChar, i );
								MapArea->PopChar();
								return;
							}
						}
						MapArea->PopChar();
					}
				}
			}
			break;
		case 0x50://Morrolan EV/BS logic
			if( !i->IsAtWar() ) 
			{
				int xOffset = MapRegion->GetGridX( i->GetX() );
				int yOffset = MapRegion->GetGridY( i->GetY() );
				for( SI08 counter1 = -1; counter1 <= 1; counter1++ )
				{
					for( SI08 counter2 = -1; counter2 <= 1; counter2++ )
					{
						SubRegion *MapArea = MapRegion->GetGrid( xOffset + counter1, yOffset + counter2, worldNumber );
						if( MapArea == NULL )	// no valid region
							continue;
						CChar *tempChar;
						MapArea->PushChar();
						for( tempChar = MapArea->FirstChar(); !MapArea->FinishedChars(); tempChar = MapArea->GetNextChar() )
						{
							if( tempChar == NULL || tempChar == i )
								continue;
							if( i->GetOwnerObj() != NULL && i->GetOwnerObj() == tempChar )
								continue;
							if( !objInRange( i, tempChar, combatRange ) )
								continue;
							if( tempChar->IsInvulnerable() || tempChar->IsDead() )
								continue;
							if( !isOnline( tempChar ) && !tempChar->IsNpc() )
								continue;
							npcAttackTarget( tempChar, i );
							MapArea->PopChar();
							return;
						}
						MapArea->PopChar();
					}
				}
			}
			break;
		case 0: break; //morrolan - nothing
		case 8: break; //morrolan - old banker
		case 5: break; //morrolan - personal guard?
		case 17: break; //Zippy Player Vendors.
		case 32: 
			CChar *pOwner;
			pOwner = (CChar*)i->GetOwnerObj();
			if( pOwner == NULL )
			{
				i->SetNPCAiType( 0 );	// not guarding because no owner
				return;
			}
			if( pOwner->GetTarg() != INVALIDSERIAL )
				npcAttackTarget( &chars[pOwner->GetTarg()], i );
			break;	// Guard AI
		default:
			Console.Error( 2, " cCharStuff::CheckAI-> Error npc %i (%x %x %x %x) has invalid AI type %i", i, i->GetSerial( 1 ), i->GetSerial( 2 ), i->GetSerial( 3 ), i->GetSerial( 4 ), i->GetNPCAiType()); //Morrolan
			return;
		}
	}
}
