#include "uox3.h"
#include "debug.h"

#undef DBGFILE
#define DBGFILE "p_ai.cpp"

void cCharStuff::CheckAI( CChar *i )
{
	if( i == NULL ) 
		return;

	UI08 worldNumber = i->WorldNumber();
	int d, d2;
	cSocket *mSock = NULL;
	CChar *realChar = NULL;
	if( nextnpcaitime <= uiCurrentTime || overflow ) 
	{
		switch( i->GetNPCAiType() )
		{
		case 1 : // Good Healers
			Network->PushConn();
			for( mSock = Network->FirstSocket(); !Network->FinishedSockets(); mSock = Network->NextSocket() )
			{
				realChar = mSock->CurrcharObj();
				UI16 distance = getDist( i, realChar );
				if( LineOfSight( mSock, realChar, i->GetX(), i->GetY(), i->GetZ(), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING ) )
				{
					if( realChar->IsDead() && realChar->IsInnocent() && distance <= 3 && !realChar->IsCriminal() && !realChar->IsMurderer() ) 
					{
						npcAction( i, 0x10 );
						Targ->NpcResurrectTarget( realChar );
						staticeffect( realChar, 0x376A, 0x09, 0x06 );
						npcTalkAll( i, ( 316 + RandomNum( 0, 4 ) ), false );
					} 
					else if( realChar->IsDead() && distance <= 3 && realChar->IsMurderer() )
						npcTalkAll( i, 322, true );
					else if( realChar->IsDead() && distance <= 3 && realChar->IsCriminal() )
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
				UI16 eDist = getDist( i, realChar );
				if( realChar->IsDead() && eDist <= 3 && realChar->IsMurderer() ) 
				{
					npcAction( i, 0x10 );
					Targ->NpcResurrectTarget( realChar );
					staticeffect( realChar, 0x3709, 0x09, 0x19 ); //Flamestrike effect
					npcTalkAll( i, ( 323 + RandomNum( 0, 4 ) ), false ); 
				} 
				else if( !realChar->IsMurderer() && realChar->IsDead() && eDist <= 3 ) 
					npcTalkAll( i, 329, true );
			}
			Network->PopConn();
			break;
		case 2 : // Monsters, PK's - (stupid NPCs)
			if( !i->IsAtWar() ) 
			{
				d2 = cwmWorldState->ServerData()->GetCombatMaxRange();
				
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
							if( i->GetOwner() != INVALIDSERIAL && i->GetOwner() == tempChar->GetSerial() )
								continue;
							d = getDist( i, tempChar );
							if( d > d2 )
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
				d2 = 10;
				
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
							d = getDist( i, tempChar );
							if( d <= 10 && !tempChar->IsInvulnerable() && !tempChar->IsDead() && tempChar->GetNPCAiType() == 0x02 )
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
				d2=10;
				
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
							d = getDist( i, tempChar );
							if( d > d2 && !tempChar->IsInvulnerable() && !tempChar->IsDead() && tempChar->GetNPCAiType() != 0x02 )
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
				d2 = 18;
				
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
							if( i->GetOwner() != INVALIDSERIAL && i->GetOwner() == tempChar->GetSerial() )
								continue;
							d = getDist( i, tempChar );
							if( d > d2 )
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
			SERIAL ownerSerial;
			CChar *pOwner;
			ownerSerial = i->GetSerial();
			if( ownerSerial == INVALIDSERIAL )
			{
				i->SetNPCAiType( 0 );	// not guarding because no owner
				return;
			}
			pOwner = calcCharObjFromSer( ownerSerial );
			if( pOwner != NULL )
			{
				if( pOwner->GetTarg() != INVALIDSERIAL )
					npcAttackTarget( &chars[pOwner->GetTarg()], i );
			}
			break;	// Guard AI
		default:
			Console.Error( 2, " cCharStuff::CheckAI-> Error npc %i (%x %x %x %x) has invalid AI type %i", i, i->GetSerial( 1 ), i->GetSerial( 2 ), i->GetSerial( 3 ), i->GetSerial( 4 ), i->GetNPCAiType()); //Morrolan
			return;
		}
	}
}
