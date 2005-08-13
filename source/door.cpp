#include "uox3.h"
#include "cEffects.h"
#include "regions.h"
#include "CJSMapping.h"
#include "cScript.h"

namespace UOX
{

#define DOORTYPES 29

const UI16 doorbase[DOORTYPES] = {
0x00E8, 0x0314, 0x0324, 0x0334, 0x0344, 0x0354, 0x0675, 0x0685, 
0x0695, 0x06A5, 0x06B5, 0x06C5, 0x06D5, 0x06E5, 0x0824, 0x0839, 
0x084C, 0x0866, 0x190E, 0x1FED, 0x241F, 0x2421, 0x2423, 0x26F4, 
0x2990, 0x299E, 0x2A05, 0x2A0D, 0x2A15
};

//o--------------------------------------------------------------------------
//|	Function		-	void useDoor( CSocket *s, CItem *item )
//|	Date			-	Unknown
//o--------------------------------------------------------------------------
//|	Purpose			-	Check if the door is a valid door and if so open / close it
//o--------------------------------------------------------------------------
void useDoor( CSocket *s, CItem *item )
{
	UI16 x, db;
	bool changed = false;

	CChar *mChar = NULL;
	if( s != NULL )
	{
		mChar = s->CurrcharObj();
		if( !objInRange( mChar, item, DIST_NEARBY ) ) 
		{
			s->sysmessage( 1183 );
			return;
		}
		UI16 itemTrig		= item->GetScriptTrigger();
		cScript *toExecute	= JSMapping->GetScript( itemTrig );
		if( toExecute != NULL )
		{
			if( toExecute->OnUse( mChar, item ) == 1 )	// if it exists and we don't want hard code, return
				return;
		}
	}

	for( UI08 i = 0; i < DOORTYPES; ++i )
	{
		db = doorbase[i];
		
		x = item->GetID(); 
		
		// With the new sliding doors, we have to filter out which doors are which
		if( db >= doorbase[0] && db <= doorbase[19])
		{
			// Ok these are the old doors so we use our general logic

			if( x == (db + 0) )
			{
				item->IncID( 1 );
				if( db != 0x190E )
					item->IncLocation( -1, 1 );
				changed = true;
				Effects->doorSound( item, x, false );
				Effects->tempeffect( mChar, item, 13, 0, 0, 0 );
			}
			else if( x == (db + 1) )
			{
				item->IncID( -1 );
				if( db != 0x190E )
					item->IncLocation( 1, -1 );
				changed = true;
				Effects->doorSound( item, x, true );
				item->SetDoorOpen( false );
			}
			else if( x == (db + 2) )
			{
				item->IncID( 1 );
				item->IncLocation( 1, 1 );
				changed = true;
				Effects->doorSound( item, x, false );
				Effects->tempeffect( mChar, item, 13, 0, 0, 0 );
			}
			else if( x == (db + 3) )
			{
				item->IncID( -1 );
				item->IncLocation( -1, -1 );
				changed = true;
				Effects->doorSound( item, x, true );
				item->SetDoorOpen( false );
			}
			else if( x == (db + 4) )
			{
				item->IncID( 1 );
				item->IncLocation( -1, 0 );
				changed = true;
				Effects->doorSound( item, x, false );
				Effects->tempeffect( mChar, item, 13, 0, 0, 0 );
			}
			else if( x == (db + 5) )
			{
				item->IncID( -1 );
				item->IncLocation( 1, 0 );
				changed = true;
				Effects->doorSound( item, x, true );
				item->SetDoorOpen( false );
			}
			else if( x == (db + 6) )
			{
				item->IncID( 1 );
				item->IncLocation( 1, -1 );
				changed = true;
				Effects->doorSound( item, x, false );
				Effects->tempeffect( mChar, item, 13, 0, 0, 0 );
			}
			else if( x == (db + 7) )
			{
				item->IncID( -1 );
				item->IncLocation( -1, 1 );
				changed = true;
				Effects->doorSound( item, x, true );
				item->SetDoorOpen( false );
			}
			else if( x == (db + 8) )
			{
				item->IncID( 1 );
				item->IncLocation( 1, 1 );
				changed = true;
				Effects->doorSound( item, x, false );
				Effects->tempeffect( mChar, item, 13, 0, 0, 0 );
			}
			else if( x == (db + 9) )
			{
				item->IncID( -1 );
				item->IncLocation( -1, -1 );
				changed = true;
				Effects->doorSound( item, x, true );
				item->SetDoorOpen( false );
			}
			else if( x == (db + 10) )
			{
				item->IncID( 1 );
				item->IncLocation( 1, -1 );
				changed = true;
				Effects->doorSound( item, x, false );
				Effects->tempeffect( mChar, item, 13, 0, 0, 0 );
			}
			else if( x == (db + 11) )
			{
				item->IncID( -1 );
				item->IncLocation( -1, 1 );
				changed = true;
				Effects->doorSound( item, x, true );
				item->SetDoorOpen( false );
			}
			else if( x == (db + 12) )
			{
				item->IncID( 1 );
				changed = true;
				Effects->doorSound( item, x, false );
				Effects->tempeffect( mChar, item, 13, 0, 0, 0 );
			}
			else if( x == (db + 13) )
			{
				item->IncID( -1 );
				changed = true;
				Effects->doorSound( item, x, true );
				item->SetDoorOpen( false );
			}
			else if( x == (db + 14) )
			{
				item->IncID( 1 );
				item->IncLocation( 0, -1 );
				changed = true;
				Effects->doorSound( item, x, false );
				Effects->tempeffect( mChar, item, 13, 0, 0, 0 );
			}
			else if( x == (db + 15) )
			{
				item->IncID( -1 );
				item->IncLocation( 0, 1 );
				changed = true;
				Effects->doorSound( item, x, true );
				item->SetDoorOpen( false );
			}
		}
		else if( db == doorbase[20] )
		{
			// normal door
			if ( x == ( db + 0 ) )
			{
				item->IncID( 1 );
				item->IncLocation( 0, -1 );
				changed = true;
				Effects->doorSound( item, x, false );
				Effects->tempeffect( mChar, item, 13, 0, 0, 0 );
			}
			else if( x == ( db + 1 ) )
			{
				item->IncID( -1 );
				item->IncLocation( 0, 1 );
				changed =true;
				Effects->doorSound( item, x, true );
				item->SetDoorOpen( false );
			}
		}
		else if( db == doorbase[21] )
		{
			// this door isn't in the 1/2 format but is now in the 2/1 format.
			if ( x == ( db + 1 ) )
			{
				item->IncID( -1 );
				item->IncLocation( 0, -1 );
				changed = true;
				Effects->doorSound( item, x, false );
				Effects->tempeffect( mChar, item, 13, 0, 0, 0 );
			}
			else if( x == ( db ) )
			{
				item->IncID( 1 );
				item->IncLocation( 0, 1 );
				changed =true;
				Effects->doorSound( item, x, true );
				item->SetDoorOpen( false );
			}
		}
		else if( db == doorbase[22] )
		{
			// this door isn't in the 1/2 format but is now in the 2/1 format.
			if ( x == ( db + 1 ) )
			{
				item->IncID( -1 );
				item->IncLocation( 0, -1 );
				changed = true;
				Effects->doorSound( item, x, false );
				Effects->tempeffect( mChar, item, 13, 0, 0, 0 );
			}
			else if( x == ( db ) )
			{
				item->IncID( 1 );
				item->IncLocation( 0, 1 );
				changed =true;
				Effects->doorSound( item, x, true );
				item->SetDoorOpen( false );
			}
		}
		else if( db == doorbase[23])
		{
			// Last of the doors. But still not a sliding door.
			if ( x == ( db + 0 ) )
			{
				item->IncID( 2 );
				changed = true;
				Effects->doorSound( item, x, false );
				Effects->tempeffect( mChar, item, 13, 0, 0, 0 );
			}
			else if( x == ( db + 2 ) )
			{
				item->IncID( -2 );
				changed =true;
				Effects->doorSound( item, x, true );
				item->SetDoorOpen( false );
			}
		}
		else if( db == doorbase[24] || db == doorbase[25] )
		{
			// the sliding doors
			if ( x == ( db + 0 ) )
			{
				item->IncID( 1 );
				changed = true;
				Effects->doorSound( item, x, false );
				Effects->tempeffect( mChar, item, 13, 0, 0, 0 );
			}
			else if( x == ( db + 1 ) )
			{
				item->IncID( -1 );
				changed =true;
				Effects->doorSound( item, x, true );
				item->SetDoorOpen( false );
			}
		}
		else if( db >= doorbase[26] && db <= doorbase[DOORTYPES-1] )
		{
			//1 the sliding doors
			if ( x == ( db + 0 ) )
			{
				item->IncID( 1 );
				changed = true;
				Effects->doorSound( item, x, false );
				Effects->tempeffect( mChar, item, 13, 0, 0, 0 );
			}
			else if( x == ( db + 1 ) )
			{
				item->IncID( -1 );
				changed =true;
				Effects->doorSound( item, x, true );
				item->SetDoorOpen( false );
			}
			//2
			if ( x == ( db + 2 ) )
			{
				item->IncID( 1 );
				changed = true;
				Effects->doorSound( item, x, false );
				Effects->tempeffect( mChar, item, 13, 0, 0, 0 );
			}
			else if( x == ( db + 3 ) )
			{
				item->IncID( -1 );
				changed =true;
				Effects->doorSound( item, x, true );
				item->SetDoorOpen( false );
			}
			//3
			if ( x == ( db + 4 ) )
			{
				item->IncID( 1 );
				changed = true;
				Effects->doorSound( item, x, false );
				Effects->tempeffect( mChar, item, 13, 0, 0, 0 );
			}
			else if( x == ( db + 5 ) )
			{
				item->IncID( -1 );
				changed =true;
				Effects->doorSound( item, x, true );
				item->SetDoorOpen( false );
			}
			//4
			if ( x == ( db + 6 ) )
			{
				item->IncID( 1 );
				changed = true;
				Effects->doorSound( item, x, false );
				Effects->tempeffect( mChar, item, 13, 0, 0, 0 );
			}
			else if( x == ( db + 7 ) )
			{
				item->IncID( -1 );
				changed =true;
				Effects->doorSound( item, x, true );
				item->SetDoorOpen( false );
			}
		}
	}
	if( changed == false && s != NULL ) 
		s->sysmessage( 1184 );
}

bool keyInPack( CSocket *mSock, CChar *mChar, CItem *pack, CItem *x )
{
	if( ValidateObject( pack ) )
	{
		CDataList< CItem * > *pCont = pack->GetContainsList();
		for( CItem *nItem = pCont->First(); !pCont->Finished(); nItem = pCont->Next() )
		{
			if( ValidateObject( nItem ) )
			{
				if( nItem->GetTempVar( CITV_MORE ) == x->GetTempVar( CITV_MORE ) )
				{
					mChar->talk( mSock, 405, false );
					useDoor( mSock, x );
					mSock->SetTimer( tPC_OBJDELAY, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_OBJECTUSAGE ) );
					return true;
				} 
				else if( nItem->GetType() == IT_CONTAINER )
				{
					if( keyInPack( mSock, mChar, nItem, x ) )
						return true;
				}
			}
		}
	}
	return false;
}

//o--------------------------------------------------------------------------
//|	Function		-	DoorMacro( CSocket *s )
//|	Date			-	11th October, 1999
//|	Programmer		-	Zippy
//|	Modified		-	Abaddon (support CSocket *s and door blocking)
//o--------------------------------------------------------------------------
//|	Purpose			-	Door use macro support.
//o--------------------------------------------------------------------------
void DoorMacro( CSocket *s )
{
	CChar *mChar = s->CurrcharObj();
	SI16 xc = mChar->GetX(), yc = mChar->GetY();
	switch( mChar->GetDir() )
	{
		case 0 : --yc;				break;
		case 1 : { ++xc; --yc; }	break;
		case 2 : ++xc;				break;
		case 3 : { ++xc; ++yc; }	break;
		case 4 : ++yc;				break;
		case 5 : { --xc; ++yc; }	break;
		case 6 : --xc;				break;
		case 7 : { --xc; --yc; }	break;
	}

	REGIONLIST nearbyRegions = MapRegion->PopulateList( mChar );
	for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
	{
		SubRegion *toCheck = (*rIter);
		if( toCheck == NULL )	// no valid region
			continue;
		CDataList< CItem * > *regItems = toCheck->GetItemList();
		regItems->Push();
		for( CItem *itemCheck = regItems->First(); !regItems->Finished(); itemCheck = regItems->Next() )
		{
			if( !ValidateObject( itemCheck ) )
				continue;
			SI16 distZ = abs( itemCheck->GetZ() - mChar->GetZ() );
			if( itemCheck->GetX() == xc && itemCheck->GetY() == yc && distZ < 7 )
			{
				if( itemCheck->GetType() == IT_DOOR || itemCheck->GetType() == IT_LOCKEDDOOR )	// only open doors
				{
					if( mChar->IsGM() )
					{
						useDoor( s, itemCheck );
						regItems->Pop();
						return;
					}
					if( itemCheck->GetType() == IT_LOCKEDDOOR )
					{
						if( keyInPack( s, mChar, mChar->GetPackItem(), itemCheck ) )
						{
							regItems->Pop();
							return;
						}
						s->sysmessage( 1247 );
						regItems->Pop();
						return;
					}
					useDoor( s, itemCheck );
					regItems->Pop();
					return;
				}
			}
		}
		regItems->Pop();
	}
}

//o--------------------------------------------------------------------------
//|	Function		-	void DoorNewLocation( CItem *item, SI16 &targetX, SI16 &targetY )
//|	Date			-	3 July, 2001
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Modifies targetX and targetY to the location of the door
//|						when it is closed.
//o--------------------------------------------------------------------------
void DoorNewLocation( CItem *item, SI16 &targetX, SI16 &targetY )
{
	targetX = item->GetX();
	targetY = item->GetY();

	UI16 db, x;
	for( UI08 i = 0; i < DOORTYPES; ++i )
	{
		db = doorbase[i];
		
		x = item->GetID();
		if( x == ( db + 0 ) )
		{
			if( db != 0x190E )
			{
				--targetX;
				++targetY;
			}
			return;
		} 
		else if( x == ( db + 1 ) )
		{
			if( db != 0x190E )
			{
				++targetX;
				--targetY;
			}
			return;
		} 
		else if( x == ( db + 2 ) )
		{
			++targetX;
			++targetY;
			return;
		} 
		else if( x == ( db + 3 ) )
		{
			--targetX;
			--targetY;
			return;
		} 
		else if( x == ( db + 4 ) )
		{
			--targetX;
			return;
		} 
		else if( x == ( db + 5 ) )
		{
			++targetX;
			return;
		} 
		else if( x == ( db + 6 ) )
		{
			++targetX;
			--targetY;
			return;
		} 
		else if( x == ( db + 7 ) )
		{
			--targetX;
			++targetY;
			return;
		} 
		else if( x == ( db + 8 ) )
		{
			++targetX;
			++targetY;
			return;
		} 
		else if( x == ( db + 9 ) )
		{
			--targetX;
			--targetY;
			return;
		} 
		else if( x == ( db + 10 ) )
		{
			++targetX;
			--targetY;
			return;
		} 
		else if( x == ( db + 11 ) )
		{
			--targetX;
			++targetY;
			return;
		} 
		else if( x == ( db + 14 ) )
		{
			--targetY;
			return;
		} 
		else if( x == ( db + 15 ) )
		{
			++targetY;
			return;
		}
	}
}

//o--------------------------------------------------------------------------
//|	Function		-	isDoorBlocked
//|	Date			-	3 July, 2001
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns true if the ITEM door would be closed if closed
//o--------------------------------------------------------------------------
bool isDoorBlocked( CItem *door )
{
	if( !ValidateObject( door ) )
		return false;
	SI16 targX = door->GetX(), targY = door->GetY();
	DoorNewLocation( door, targX, targY );

	UI08 worldNumber = door->WorldNumber();
	SubRegion *Cell = MapRegion->GetCell( targX, targY, worldNumber );
	if( Cell != NULL )
	{
		CDataList< CChar * > *regChars = Cell->GetCharList();
		regChars->Push();
		for( CChar *mCheck = regChars->First(); !regChars->Finished(); mCheck = regChars->Next() )
		{
			if( ( ValidateObject( mCheck ) && mCheck->GetX() == targX && mCheck->GetY() == targY )
				&& ( !mCheck->IsDead() && ( mCheck->IsNpc() || isOnline( (*mCheck) ) ) ) )
			{
				regChars->Pop();
				return true;
			}
		}
		regChars->Pop();
	}
	return false;
}

}

