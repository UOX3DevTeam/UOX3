// House code for deed creation by Tal Strake, revised by Cironian

#include "uox3.h"
#include "ssection.h"

#undef DBGFILE
#define DBGFILE "house.cpp"

void mtarget( cSocket *s, SERIAL ser, UI16 itemID, const char *txt )
{
	CPMultiPlacementView toSend( ser );
	toSend.MultiModel( itemID );
	toSend.RequestType( 1 );

	sysmessage( s, txt );
	s->TargetOK( true );
	s->Send( &toSend );
}

void mtarget( cSocket *s, UI08 a1, UI08 a2, UI08 a3, UI08 a4, UI08 b1, UI08 b2, const char *txt )
{
	mtarget( s, calcserial( a1, a2, a3, a4 ), (UI16)((b1<<8) + b2), txt );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void buildHouse( cSocket *s, UI32 i )
//|   Date        -  UnKnown - Rewrite Date 1/24/99
//|   Programmer  -  UnKnown - Rewrite by Zippy (onlynow@earthlink.net)
//o---------------------------------------------------------------------------o
//|   Purpose     -  Triggered by double clicking a deed-> the deed's moreX is read
//|						for the house section in house.scp. Extra items can be added
//|						using HOUSE ITEM, (this includes all doors!) and locked "LOCK"
//|						Space around the house with SPACEX/Y and CHAR offset CHARX/Y/Z
//o---------------------------------------------------------------------------o
void buildHouse( cSocket *s, UI32 i )
{
	SI16 x, y, sx = 0, sy = 0, cx = 0, cy = 0;
	SI08 z, cz = 8;
	int k, l;					//Temps
	int hitem[100],icount=0;	//extra "house items" (up to 100)
	char sect[512];				//file reading
	char itemsdecay = 0;		// set to 1 to make stuff decay in houses
	static int looptimes = 0;	//for targeting
	bool boat = false;			//Boats
	int hdeed = 0;				//deed id #
	const char *tag = NULL;
	const char *data = NULL;
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return;

	UI16 houseID = 0;
	hitem[0] = 0;//avoid problems if there are no HOUSE_ITEMs by initializing the first one as 0
	if( i )
	{
		sprintf( sect, "HOUSE %d", i );//and BTW, .find() adds SECTION on there for you....
		ScriptSection *House = FileLookup->FindEntry( sect, house_def );
		if( House == NULL )
			return;
		for( tag = House->First(); !House->AtEnd(); tag = House->Next() )
		{
			data = House->GrabData();
			if( !strcmp( tag,"ID") )
				houseID = (UI16)makeNum( data );
			else if( !strcmp( tag,"SPACEX" ) )
				sx = static_cast<SI16>(makeNum( data ) + 1);
			else if( !strcmp( tag,"SPACEY" ) )
				sy = static_cast<SI16>(makeNum( data ) + 1);
			else if( !strcmp( tag,"CHARX" ) )
				cx = static_cast<SI16>(makeNum( data ));
			else if( !strcmp( tag,"CHARY" ) )
				cy = static_cast<SI16>(makeNum( data ));
			else if( !strcmp( tag,"CHARZ" ) )
				cz = static_cast<SI08>(makeNum( data ));
			else if( !strcmp( tag, "ITEMSDECAY" ) )
				itemsdecay = static_cast<char>(makeNum( data ));
			else if( !strcmp( tag,"HOUSE_ITEM" ) )
			{
				hitem[icount] = makeNum( data );
				icount++;
			}
			else if( !strcmp( tag, "HOUSE_DEED" ) )
				hdeed = makeNum( data );
			else if( !strcmp( tag, "BOAT") ) 
				boat = true;	//Boats
		}
		if( !houseID )
		{
			Console.Error( 1, "Bad house script # %i!\n",i);
			return;
		}
	}
	if( !looptimes )
	{
		if( i )
		{
			s->AddID1( 0x40 );
			s->AddID2( 100 );
			if( houseID >= 0x4000 )
				mtarget( s, calcserial( 0, 1, 0, 0 ), houseID - 0x4000, Dictionary->GetEntry( 576, s->Language() ) );
			else
				target( s, 0, 1, 0, 0, 576 );
		}
		else
			mtarget( s, 0, 1, 0, 0, s->AddID1() - 0x40, s->AddID2(), Dictionary->GetEntry( 576, s->Language() ) );

		looptimes++;//for when we come back after they target something
		return;
	}
	if( looptimes )
	{
		CChar *mChar = s->CurrcharObj();
		if( mChar == NULL )
			return;
		UI08 worldNumber = mChar->WorldNumber();
		looptimes = 0;
		x = s->GetWord( 11 );
		y = s->GetWord( 13 );
		z = s->GetByte( 16 ) + Map->TileHeight( s->GetWord( 17 ) );
		if( !mChar->IsGM() )
		{
			if( x > 6200 || y > 4200 )
			{
				sysmessage( s, 577 );
				return;
			}

			for( k = -sx; k < sx; k++ ) //check the SPACEX and SPACEY to make sure they are valid locations....
			{
				for( l = -sy; l < sy; l++ )
				{
					if( ( !Movement->validNPCMove( x+k, y+l, z, mChar ) ) &&
						( ( mChar->GetX() != x+k ) && ( mChar->GetY() != y+l ) ) )
//					This will take the char making the house out of the space check, be careful 
//					you don't build a house on top of your self..... this had to be done So you 
//					could extra space around houses, (12+) and they would still be buildable.
					{
						sysmessage( s, 577 );
						return;
					}
				}
			}
		}
		char temp[1024];
		CMultiObj *house = NULL;
		CItem *fakeHouse = NULL;
		if( houseID >= 0x4000 )
		{
			if( (houseID%256) >= 18 ) 
				sprintf( temp, "%s's house", mChar->GetName() ); //This will make the little deed item you see when you have showhs on say the person's name, thought it might be helpful for GMs.
			else 
				strcpy( temp, "a mast" );
			house = Items->SpawnMulti( mChar, temp, houseID );
			if( house == NULL )
				return;

			house->SetLocation( x, y, z );
			house->SetPriv( 0 );
			house->SetMore( itemsdecay, 4 ); // set to 1 to make items in houses decay
			house->SetMoreX( hdeed ); // crackerjack 8/9/99 - for converting back *into* deeds
			house->SetOwner( mChar );
		}
		else
		{
			strcpy( temp, "#" );
			fakeHouse = Items->SpawnItem( s, mChar, 1, temp, false, houseID, 0, false, true );
			if( fakeHouse == NULL )
				return;
			fakeHouse->SetLocation( x, y, z );

			fakeHouse->SetPriv( 0 );
			fakeHouse->SetOwner( (cBaseObject *)mChar );
			fakeHouse->SetDecayable( false );
		}

		mChar->SetMaking( 0 );
		
		if( !hitem[0] && !boat )
		{
			mChar->Teleport();
			sendItemsInRange( s );
			return;//If there's no extra items, we don't really need a key, or anything else do we? ;-)
		}

		if( boat ) //Boats
		{
			if( !Boats->CreateBoat( s, house, (houseID%256), i ) )
			{
				Items->DeleItem( house );
				return;
			} 
		}

		if( i )//Boats->.. Moved from up there ^
			Items->DeleItem( calcItemObjFromSer( mChar->GetSpeechItem() ) );

		mChar->SetSpeechItem( INVALIDSERIAL );

		//Key...
		CItem *key = NULL;
		if( houseID >= 0x4000 )
		{
			if( (houseID%256) >= 112 && (houseID%256) <= 115 ) 
				key = Items->SpawnItem( s, mChar, 1, "a tent key", false, 0x1010, 0, true, true );//iron key for tents
			else if( (houseID%256) <= 0x18 ) 
				key = Items->SpawnItem( s, mChar,1,"a ship key", false, 0x1013, 0, true, true );//Boats -Rusty Iron Key
			else 
				key = Items->SpawnItem( s, mChar, 1, "a house key", false, 0x100F, 0, true, true );//gold key for everything else

			if( key != NULL )
			{
				key->SetMore( house->GetSerial() );
				key->SetType( 7 );
			}
		}
		ScriptSection *HouseItem = NULL;
		CItem *hItem = NULL;
		for( k = 0; k < icount; k++ )//Loop through the HOUSE_ITEMs
		{
			sprintf( sect, "HOUSE ITEM %i", hitem[k] );
			HouseItem = FileLookup->FindEntry( sect, house_def );
			if( HouseItem != NULL )
			{
				hItem = NULL;	// clear it out
				for( tag = HouseItem->First(); !HouseItem->AtEnd(); tag = HouseItem->Next() )
				{
					data = HouseItem->GrabData();
					if( !strcmp( tag, "ITEM" ) )
					{
						hItem = Items->CreateScriptItem( s, data, false, worldNumber );
						if( hItem == NULL )
						{
							Console << "Error in house creation, item " << data << " could not be made" << myendl;
							break;
						}
						else
						{
							hItem->SetMagic( 2 );//Non-Moveable by default
							hItem->SetPriv( 0 );//since even things in houses decay, no-decay by default
							hItem->SetLocation( x, y, z );
							hItem->SetOwner( (cBaseObject *)mChar );
							if( houseID >= 0x4000 )
								hItem->SetMulti( house );
						}
					}
					else if( !strcmp( tag, "DECAY" ) )
						hItem->SetDecayable( true );
					else if( !strcmp( tag, "NODECAY" ) )
						hItem->SetDecayable( false );
					else if( !strcmp( tag, "PACK" ) )//put the item in the Builder's Backpack
					{
						CItem *pack = getPack( mChar );
						hItem->SetCont( pack );
						hItem->SetX( RandomNum( 31, 120 ) );
						hItem->SetY( RandomNum( 31, 120 ) );
						hItem->SetZ( 9 );
					}
					else if( !strcmp( tag, "MOVEABLE" ) )
						hItem->SetMagic( 1 );
					else if( !strcmp( tag, "LOCK" ) && houseID >= 0x4000 )//lock it with the house key
						hItem->SetMore( house->GetSerial() );
					else if( !strcmp( tag, "X" ) )//offset + or - from the center of the house:
						hItem->SetX( x + static_cast<SI16>(makeNum( data )) );
					else if( !strcmp( tag, "Y" ) )
						hItem->SetY( y + static_cast<SI16>(makeNum( data )) );
					else if( !strcmp( tag, "Z" ) )
						hItem->SetZ( z + static_cast<SI08>(makeNum( data )) );
				}
				if( hItem != NULL && hItem->GetCont() == NULL )
					MapRegion->AddItem( hItem );
			}
		}
		sendItemsInRange( s );//make sure they have all the items Sent....
		// map regions not done here... hmmm... not good!!!  Not a safe assumption we're in same region
		mChar->SetLocation( x + cx, y + cy, z + cz );
		mChar->Teleport();
	}
}

// turn a house into a deed if possible. - crackerjack 8/9/99
// s = socket of player
// i = house item # in items[]
// morex on the house item must be set to deed item # in items.scp.
void deedHouse( cSocket *s, CItem *i )
{
	if( i == NULL ) 
		return;

	SI16 x1, y1, x2, y2;
	int keyCount = 0;
	CItem *pvDeed = NULL;
	CChar *mChar = s->CurrcharObj();
	char temp[1024];

	if( i->GetOwnerObj() == mChar || mChar->IsGM() )
	{
		Map->MultiArea( (CMultiObj *)i, x1, y1, x2, y2 );

#pragma note( "DEPENDENT ON NUMERIC ITEM SECTION" )
		CItem *ii = Items->SpawnItemToPack( s, mChar, i->GetMoreX(), false );	// need to make before delete
		if( ii == NULL ) 
			return;
        
		sysmessage( s, 578, i->GetName() );
		UI08 ser1 = i->GetSerial( 1 );
		UI08 ser2 = i->GetSerial( 2 );
		UI08 ser3 = i->GetSerial( 3 );
		UI08 ser4 = i->GetSerial( 4 );
		Items->DeleItem( i );
		sysmessage( s, 579, ii->GetName() );
		// door/sign delete
		int xOffset = MapRegion->GetGridX( mChar->GetX() );
		int yOffset = MapRegion->GetGridY( mChar->GetY() );
		UI08 worldNumber = mChar->WorldNumber();
		for( SI08 counter1 = -1; counter1 <= 1; counter1++ )
		{
			for( SI08 counter2 = -1; counter2 <= 1; counter2++ )
			{
				SubRegion *toCheck = MapRegion->GetGrid( xOffset + counter1, yOffset + counter2, worldNumber );
				if( toCheck == NULL )	continue;
				CItem *itemCheck;
				CChar *charCheck;
				toCheck->PushChar();
				toCheck->PushItem();
				for( itemCheck = toCheck->FirstItem(); !toCheck->FinishedItems(); itemCheck = toCheck->GetNextItem() )
				{
					if( itemCheck == NULL )		
						continue;
					if( itemCheck->GetX() >= x1 && itemCheck->GetY() >= y1 && itemCheck->GetX() <= x2 && itemCheck->GetY() <= y2 )
					{
						if( itemCheck->GetType() == 203 || itemCheck->GetType() == 12 || itemCheck->GetType() == 13 )
							Items->DeleItem( itemCheck );
					}
				}
				for( charCheck = toCheck->FirstChar(); !toCheck->FinishedChars(); charCheck = toCheck->GetNextChar() )
				{
					if( charCheck == NULL )
						continue;
					if( charCheck->GetX() >= x1 && charCheck->GetY() >= y1 && charCheck->GetX() <= x2 && charCheck->GetY() <= y2 )
					{
						if( charCheck->GetNPCAiType() == 17 ) // player vendor in right place
						{
							sprintf( temp, Dictionary->GetEntry( 580 ), charCheck->GetName() );
							pvDeed = Items->SpawnItem( NULL, mChar, 1, temp, false, 0x14F0, 0, true, false );
							if( pvDeed != NULL )
							{
								pvDeed->SetType( 217 );
								pvDeed->SetValue( 2000 );
								RefreshItem( pvDeed );
							}
							sysmessage( s, 581, charCheck->GetName() );
							Npcs->DeleteChar( charCheck );
						}
					}
				}
				toCheck->PopChar();
				toCheck->PopItem();
			}
		}
		SERIAL serialToMatch = calcserial( ser1, ser2, ser3, ser4 );
		sysmessage( s, 582 );
		for( ITEM a = 0; a < itemcount; a++ )		// we HAVE to have an itemcount, because when you replace the house, the serial will be different
		{										// so we have to remove all keys that used that serial
			if( items[a].GetMore() == serialToMatch && items[a].GetType() == 7 )
			{
				Items->DeleItem( &items[a] );
				keyCount++;
			}
		}
		sysmessage( s, 583, keyCount );
		return;
	}
}

void killKeys( SERIAL targSerial )
// This function is rather CPU-expensive, but AFAIK there is no
// better way to find all keys than to do it this way.. :/
{
	for( ITEM i = 0; i < itemcount; i++ ) 
	{
		if( items[i].GetType() == 7 && items[i].GetMore() == targSerial ) // make key uselss
			items[i].SetMore( 0 );
	}
}

bool OnHouseList( CMultiObj *house, CChar *toCheck )
{
	if( house == NULL || toCheck == NULL )
		return false;
	return house->IsOwner( toCheck );
}
UI08 AddToHouse( CMultiObj *house, CChar *toAdd, UI08 mode )
{
	if( house == NULL || toAdd == NULL )
		return 0;
	const UI08 OWNER = 0;
	const UI08 BAN = 1;
	if( house->IsOwner( toAdd ) || house->IsOnBanList( toAdd ) )
		return 2;

	SI16 sx, sy, ex, ey;
	Map->MultiArea( house, sx, sy, ex, ey );
	// Make an object with the character's serial & the list type
	// and put it "inside" the house item.
	if( toAdd->GetX() >= sx && toAdd->GetY() >= sy && toAdd->GetX() <= ex && toAdd->GetY() <= ey )
	{
		switch( mode )
		{
		default:
		case OWNER:		house->AddAsOwner( toAdd );		break;
		case BAN:		house->AddToBanList( toAdd );	break;
		}
		return 1;
	}
	else
		return 3;
}
bool DeleteFromHouseList( CMultiObj *house, CChar *toDelete, UI08 mode )
{
	if( house == NULL || toDelete == NULL )
		return false;
	switch( mode )
	{
	default:
	case 0:	// owner
		house->RemoveAsOwner( toDelete );
		break;
	case 1:	// ban
		house->RemoveFromBanList( toDelete );
		break;
	}
	return true;
}
void house_speech( cSocket *mSock, const char *talk )
{
	if( mSock == NULL ) 
		return;

	UnicodeTypes sLang = mSock->Language();

	char msg[512], msg2[512];
	CChar *mChar = mSock->CurrcharObj();
	CMultiObj *realHouse = findMulti( mChar );
	if( realHouse == NULL ) 
		return; // not in a house, so we don't care.
	if( realHouse->GetObjType() != OT_MULTI )
		return;

	strcpy( msg2, talk );
	strcpy( msg, strupr( msg2 ) ); // krazyglue - above two lines aren't necessary
	if( strstr( msg, Dictionary->GetEntry( 584, sLang ) ) ) 
	{ // realHouse ban
		mSock->AddID( realHouse->GetSerial() );
		target( mSock, 0, 1, 0, 229, 585 );
		return;
	}
	if( strstr( msg, Dictionary->GetEntry( 586, sLang ) ) ) 
	{ // kick out of realHouse
		mSock->AddID( realHouse->GetSerial() );
		target( mSock, 0, 1, 0, 228, 587 );
		return;
	}
	if( strstr( msg, Dictionary->GetEntry( 588, sLang ) ) )
	{ // lock down item
		if( realHouse->GetLockDownCount() < realHouse->GetMaxLockDowns() )
		{
			mSock->AddID( realHouse->GetSerial() );
			target( mSock, 0, 1, 0, 232, 589 );
		}
		return;
	}
	if( strstr( msg, Dictionary->GetEntry( 590, sLang ) ) )
	{ // lock down item
		if( realHouse->GetLockDownCount() > 0 )
		{
			mSock->AddID( realHouse->GetSerial() );
			target( mSock, 0, 1, 0, 233, 591 );
		}
		return;
	}
}
