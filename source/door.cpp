#include "uox3.h"

//o--------------------------------------------------------------------------
//|	Function		-	void useDoor( cSocket *s, CItem *item )
//|	Date			-	Unknown
//o--------------------------------------------------------------------------
//|	Purpose			-	Check if the door is a valid door and if so open / close it
//o--------------------------------------------------------------------------
void useDoor( cSocket *s, CItem *item )
{
	UI16 x, db;
	bool changed = false;

	CChar *mChar = NULL;
	if( s != NULL )
	{
		mChar = s->CurrcharObj();
		if( itemInRange( mChar, item, 2 ) == 0 ) 
		{
			sysmessage( s, 1183 );
			return;
		}
	}

	for( int i = 0; i < DOORTYPES; i++ )
	{
		db = doorbase[i];
		
		x = item->GetID(); 
		if( x == (db + 0) )
		{
			item->IncID( 1 );
			if( db != 0x190E )
			{
				item->IncX( -1 );
				item->IncY( 1 );
			}
			RefreshItem( item );
			changed = true;
			doorsfx( item, x, false );
			tempeffect( mChar, item, 13, 0, 0, 0 );
		}
		else if( x == (db + 1) )
		{
			item->IncID( -1 );
			if( db != 0x190E )
			{
				item->IncX( 1 );
				item->IncY( -1 );
			}
			RefreshItem( item );
			changed = true;
			doorsfx( item, x, true );
			item->SetDoorOpen( false );
		}
		else if( x == (db + 2) )
		{
			item->IncID( 1 );
			item->IncX( 1 );
			item->IncY( 1 );
			RefreshItem( item );
			changed = true;
			doorsfx( item, x, false );
			tempeffect( mChar, item, 13, 0, 0, 0 );
		}
		else if( x == (db + 3) )
		{
			item->IncID( -1 );
			item->IncX( -1 );
			item->IncY( -1 );
			RefreshItem( item );
			changed = true;
			doorsfx( item, x, true );
			item->SetDoorOpen( false );
		}
		else if( x == (db + 4) )
		{
			item->IncID( 1 );
			item->IncX( -1 );
			RefreshItem( item );
			changed = true;
			doorsfx( item, x, false );
			tempeffect( mChar, item, 13, 0, 0, 0 );
		}
		else if( x == (db + 5) )
		{
			item->IncID( -1 );
			item->IncX( 1 );
			RefreshItem( item );
			changed = true;
			doorsfx( item, x, true );
			item->SetDoorOpen( false );
		}
		else if( x == (db + 6) )
		{
			item->IncID( 1 );
			item->IncX( 1 );
			item->IncY( -1 );
			RefreshItem( item );
			changed = true;
			doorsfx( item, x, false );
			tempeffect( mChar, item, 13, 0, 0, 0 );
		}
		else if( x == (db + 7) )
		{
			item->IncID( -1 );
			item->IncX( -1 );
			item->IncY( 1 );
			RefreshItem( item );
			changed = true;
			doorsfx( item, x, true );
			item->SetDoorOpen( false );
		}
		else if( x == (db + 8) )
		{
			item->IncID( 1 );
			item->IncX( 1 );
			item->IncY( 1 );
			RefreshItem( item );
			changed = true;
			doorsfx( item, x, false );
			tempeffect( mChar, item, 13, 0, 0, 0 );
		}
		else if( x == (db + 9) )
		{
			item->IncID( -1 );
			item->IncX( -1 );
			item->IncY( -1 );
			RefreshItem( item );
			changed = true;
			doorsfx( item, x, true );
			item->SetDoorOpen( false );
		}
		else if( x == (db + 10) )
		{
			item->IncID( 1 );
			item->IncX( 1 );
			item->IncY( -1 );
			RefreshItem( item );
			changed = true;
			doorsfx( item, x, false );
			tempeffect( mChar, item, 13, 0, 0, 0 );
		}
		else if( x == (db + 11) )
		{
			item->IncID( -1 );
			item->IncX( -1 );
			item->IncY( 1 );
			RefreshItem( item );
			changed = true;
			doorsfx( item, x, true );
			item->SetDoorOpen( false );
		}
		else if( x == (db + 12) )
		{
			item->IncID( 1 );
			RefreshItem( item );
			changed = true;
			doorsfx( item, x, false );
			tempeffect( mChar, item, 13, 0, 0, 0 );
		}
		else if( x == (db + 13) )
		{
			item->IncID( -1 );
			RefreshItem( item );
			changed = true;
			doorsfx( item, x, true );
			item->SetDoorOpen( false );
		}
		else if( x == (db + 14) )
		{
			item->IncID( 1 );
			item->IncY( -1 );
			RefreshItem( item );
			changed = true;
			doorsfx( item, x, false );
			tempeffect( mChar, item, 13, 0, 0, 0 );
		}
		else if( x == (db + 15) )
		{
			item->IncID( -1 );
			item->IncY( 1 );
			RefreshItem( item );
			changed = true;
			doorsfx( item, x, true );
			item->SetDoorOpen( false );
		}
	}
	if( changed == false && s != NULL ) 
		sysmessage( s, 1184 );
}

//o--------------------------------------------------------------------------
//|	Function		-	DoorMacro( cSocket *s )
//|	Date			-	11th October, 1999
//|	Programmer		-	Zippy
//|	Modified		-	Abaddon (support cSocket *s and door blocking)
//o--------------------------------------------------------------------------
//|	Purpose			-	Door use macro support.
//o--------------------------------------------------------------------------
void DoorMacro( cSocket *s )
{
	CChar *mChar = s->CurrcharObj();
	SI16 xc = mChar->GetX(), yc = mChar->GetY();
	switch( mChar->GetDir() )
	{
	case 0 : yc--;				break;
	case 1 : { xc++; yc--; }	break;
	case 2 : xc++;				break;
	case 3 : { xc++; yc++; }	break;
	case 4 : yc++;				break;
	case 5 : { xc--; yc++; }	break;
	case 6 : xc--;				break;
	case 7 : { xc--; yc--; }	break;
	}
	
	CItem *pack = NULL, *packItem = NULL;
	int xOffset = MapRegion->GetGridX( mChar->GetX() );
	int yOffset = MapRegion->GetGridY( mChar->GetY() );
	UI08 worldNumber = mChar->WorldNumber();
	for( SI08 counter1 = -1; counter1 <= 1; counter1++ )
	{
		for( SI08 counter2 = -1; counter2 <= 1; counter2++ )
		{
			SubRegion *toCheck = MapRegion->GetGrid( xOffset + counter1, yOffset + counter2, worldNumber );
			if( toCheck == NULL )
				continue;
			toCheck->PushItem();
			for( CItem *itemCheck = toCheck->FirstItem(); !toCheck->FinishedItems(); itemCheck = toCheck->GetNextItem() )
			{
				if( itemCheck == NULL )
					continue;
				SI16 distZ = abs( itemCheck->GetZ() - mChar->GetZ() );
				if( itemCheck->GetX() == xc && itemCheck->GetY() == yc && distZ < 7 )
				{
					if( itemCheck->GetType() == 12 || itemCheck->GetType() == 13 )	// only open doors
					{
						if( mChar->IsGM() )
						{
							useDoor( s, itemCheck );
							toCheck->PopItem();
							return;
						}
						if( itemCheck->GetType() == 13 )
						{
							pack = getPack( mChar );
							if( pack != NULL )
							{
								for( packItem = pack->FirstItemObj(); !pack->FinishedItems(); packItem = pack->NextItemObj() )
								{
									if( packItem != NULL )
									{
										if( packItem->GetMore() == itemCheck->GetMore() )
										{
											npcTalk( s, mChar, 1607, false );
											useDoor( s, itemCheck );
											toCheck->PopItem();
											return;
										}
									}
								}
							} 
							sysmessage( s, 1247 );
							toCheck->PopItem();
							return;
						}
						useDoor( s, itemCheck );
						toCheck->PopItem();
						return;
					}
				}
			}
			toCheck->PopItem();
		}
	}
}

//o--------------------------------------------------------------------------
//|	Function		-	void DoorNewLocation( ITEM item, SI16 &targetX, SI16 &targetY )
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
	for( int i = 0; i < DOORTYPES; i++ )
	{
		db = doorbase[i];
		
		x = item->GetID();
		if( x == ( db + 0 ) )
		{
			if( db != 0x190E )
			{
				targetX--;
				targetY++;
			}
			return;
		} 
		else if( x == ( db + 1 ) )
		{
			if( db != 0x190E )
			{
				targetX++;
				targetY--;
			}
			return;
		} 
		else if( x == ( db + 2 ) )
		{
			targetX++;
			targetY++;
			return;
		} 
		else if( x == ( db + 3 ) )
		{
			targetX--;
			targetY--;
			return;
		} 
		else if( x == ( db + 4 ) )
		{
			targetX--;
			return;
		} 
		else if( x == ( db + 5 ) )
		{
			targetX++;
			return;
		} 
		else if( x == ( db + 6 ) )
		{
			targetX++;
			targetY--;
			return;
		} 
		else if( x == ( db + 7 ) )
		{
			targetX--;
			targetY++;
			return;
		} 
		else if( x == ( db + 8 ) )
		{
			targetX++;
			targetY++;
			return;
		} 
		else if( x == ( db + 9 ) )
		{
			targetX--;
			targetY--;
			return;
		} 
		else if( x == ( db + 10 ) )
		{
			targetX++;
			targetY--;
			return;
		} 
		else if( x == ( db + 11 ) )
		{
			targetX--;
			targetY++;
			return;
		} 
		else if( x == ( db + 14 ) )
		{
			targetY--;
			return;
		} 
		else if( x == ( db + 15 ) )
		{
			targetY++;
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
	if( door == NULL )
		return false;
	SI16 targX = door->GetX(), targY = door->GetY();
	DoorNewLocation( door, targX, targY );

	UI08 worldNumber = door->WorldNumber();
	SubRegion *Cell = MapRegion->GetCell( targX, targY, worldNumber );
	if( Cell != NULL )
	{
		Cell->PushChar();
		for( CChar *mCheck = Cell->FirstChar(); !Cell->FinishedChars(); mCheck = Cell->GetNextChar() )
		{
			if( mCheck->GetX() == targX && mCheck->GetY() == targY && ( mCheck->IsNpc() || isOnline( mCheck ) ) )
			{
				Cell->PopChar();
				return true;
			}
		}
		Cell->PopChar();
	}
	return false;
}
