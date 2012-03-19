// House code for deed creation by Tal Strake, revised by Cironian

#include "uox3.h"
#include "mapstuff.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "CPacketSend.h"
#include "classes.h"
#include "regions.h"
#include "Dictionary.h"

#undef DBGFILE
#define DBGFILE "house.cpp"

#include "ObjectFactory.h"

namespace UOX
{

bool CreateBoat( CSocket *s, CBoatObj *b, UI08 id2, UI08 boattype );

//o---------------------------------------------------------------------------o
//|   Function    -  void buildHouse( CSocket *s, UI08 i )
//|   Date        -  UnKnown - Rewrite Date 1/24/99
//|   Programmer  -  UnKnown - Rewrite by Zippy (onlynow@earthlink.net)
//o---------------------------------------------------------------------------o
//|   Purpose     -  Triggered by double clicking a deed-> the deed's moreX is read
//|						for the house section in house.scp. Extra items can be added
//|						using HOUSE ITEM, (this includes all doors!) and locked "LOCK"
//|						Space around the house with SPACEX/Y and CHAR offset CHARX/Y/Z
//o---------------------------------------------------------------------------o
void DoHouseTarget( CSocket *mSock, UI08 houseEntry )
{
	UI16 houseID = 0;
	UString tag, data;
	UString sect			= "HOUSE " + UString::number( houseEntry );
	ScriptSection *House	= FileLookup->FindEntry( sect, house_def );
	if( House == NULL )
		return;
	for( tag = House->First(); !House->AtEnd(); tag = House->Next() )
	{
		data = House->GrabData();
		if( tag.upper() == "ID" )
		{
			houseID = data.toUShort();
			break;
		}
	}
	if( !houseID )
		Console.Error( "Bad house script: #%u\n", houseEntry );
	else
	{
		mSock->AddID1( houseEntry );
		if( houseID >= 0x4000 )
			mSock->mtarget( static_cast<UI16>(houseID - 0x4000), 576 );
		else
			mSock->target( 0, TARGET_BUILDHOUSE, 576 );
	}
}
void CreateHouseKey( CSocket *mSock, CChar *mChar, CMultiObj *house, UI16 houseID )
{
	if( houseID >= 0x4000 )
	{
		std::string scriptName;
		if( (houseID%256) >= 0x70 && (houseID%256) <= 0x73 ) // It's a tent
			scriptName = "tent_key";
		else if( (houseID%256) <= 0x18 )					// It's a boat
			scriptName = "boat_key";
		else
			scriptName = "house_key";

		CItem *key = Items->CreateScriptItem( mSock, mChar, scriptName, 1, OT_ITEM, true );
		if( ValidateObject( key ) )
		{
			key->SetTempVar( CITV_MORE, house->GetSerial() );
			key->SetType( IT_KEY );
		}
	}
}
void CreateHouseItems( CChar *mChar, STRINGLIST houseItems, CItem *house, UI16 houseID, SI16 x, SI16 y, SI08 z )
{
	UString tag, data, UTag;
	ScriptSection *HouseItem = NULL;
	CItem *hItem = NULL;
	STRINGLIST_CITERATOR hiIter;
	for( hiIter = houseItems.begin(); hiIter != houseItems.end(); ++hiIter )//Loop through the HOUSE_ITEMs
	{
		UString sect = "HOUSE ITEM " + (*hiIter);
		HouseItem = FileLookup->FindEntry( sect, house_def );
		if( HouseItem != NULL )
		{
			SI16 hiX = x, hiY = y;
			SI08 hiZ = z;
			UI08 hWorld = house->WorldNumber();
			hItem = NULL;	// clear it out
			for( tag = HouseItem->First(); !HouseItem->AtEnd(); tag = HouseItem->Next() )
			{
				UTag = tag.upper();
				data = HouseItem->GrabData();
				if( UTag == "ITEM" )
				{
					hItem = Items->CreateBaseScriptItem( data, mChar->WorldNumber() );
					if( hItem == NULL )
					{
						Console << "Error in house creation, item " << data << " could not be made" << myendl;
						break;
					}
					else
					{
						hItem->SetMovable( 2 );//Non-Moveable by default
						hItem->SetLocation( house );
						hItem->SetOwner( mChar );
					}
				}
				else if( UTag == "DECAY" )
					hItem->SetDecayable( true );
				else if( UTag == "NODECAY" )
					hItem->SetDecayable( false );
				else if( UTag == "PACK" )//put the item in the Builder's Backpack
				{
					CItem *pack = mChar->GetPackItem();
					hItem->SetCont( pack );
					hItem->PlaceInPack();
				}
				else if( UTag == "MOVEABLE" )
					hItem->SetMovable( 1 );
				else if( UTag == "INVISIBLE" )
					hItem->SetVisible( VT_PERMHIDDEN );
				else if( UTag == "LOCK" && houseID >= 0x4000 )//lock it with the house key
					hItem->SetTempVar( CITV_MORE, house->GetSerial() );
				else if( UTag == "X" )//offset + or - from the center of the house:
					hiX += data.toShort();
				else if( UTag == "Y" )
					hiY += data.toShort();
				else if( UTag == "Z" )
					hiZ += data.toShort();
			}
			if( ValidateObject( hItem ) && hItem->GetCont() == NULL )
				hItem->SetLocation( hiX, hiY, hiZ, hWorld );
		}
	}
}

bool CheckForValidHouseLocation( CSocket *mSock, CChar *mChar, SI16 x, SI16 y, SI08 z, SI16 spaceX, SI16 spaceY, bool isBoat, bool isMulti )
{
	const UI08 worldNum = mChar->WorldNumber();

	MapData_st& mMap = Map->GetMapData( worldNum );
	if( ( x+spaceX > mMap.xBlock || x-spaceX < 0 || y+spaceY > mMap.yBlock || y-spaceY < 0 ) && !mChar->IsGM() )
	{
		mSock->sysmessage( 577 );
		return false;
	}

	SI16 curX, curY;
	const SI16 charX = mChar->GetX();
	const SI16 charY = mChar->GetY();
	for( SI16 k = -spaceX; k <= spaceX; ++k )
	{
		curX = x+k;
		for( SI16 l = -spaceY; l <= spaceY; ++l )
		{
			curY = y+l;
			if( ( !Map->ValidMultiLocation( curX, curY, z, worldNum, !isBoat ) && ( charX != curX && charY != curY ) ) ||
				( isMulti && findMulti( curX, curY, z, worldNum ) != NULL ) )// Lets not place a multi on/in another multi
//			This will take the char making the house out of the space check, be careful 
//			you don't build a house on top of your self..... this had to be done So you 
//			could extra space around houses, (12+) and they would still be buildable.
			{
				if( isBoat )
					mSock->sysmessage( 7 );
				else
					mSock->sysmessage( 577 );
				return false;
			}
			else if( !isMulti )
			{
				if( mChar->GetCommandLevel() < CL_GM )
				{
					CMultiObj *mMulti = findMulti( curX, curY, z, worldNum );
					if( !ValidateObject( mMulti ) || !mMulti->IsOwner( mChar ) )
					{
						mSock->sysmessage( "This object must be placed in your house" );
						return false;
					}
				}
			}
		}
	}
	return true;
}

void BuildHouse( CSocket *mSock, UI08 houseEntry )
{
	if( mSock->GetDWord( 11 ) == INVALIDSERIAL )
		return;
	if( !houseEntry )
		return;

	CChar *mChar = mSock->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return;

	const SI16 x = mSock->GetWord( 11 );
	const SI16 y = mSock->GetWord( 13 );
	SI08 z = static_cast<SI08>(mSock->GetByte( 16 ) + Map->TileHeight( mSock->GetWord( 17 ) ));
	UI32 targetSerial = mSock->GetDWord( 7 );
	if( mSock->GetDWord( 7 ) != INVALIDSERIAL || getDist( mChar->GetLocation(), point3( x, y, z ) ) >= DIST_BUILDRANGE )
	{
		mSock->sysmessage( 577 );
		return;
	}

	SI16 sx = 0, sy = 0, cx = 0, cy = 0;
	SI08 cz = 7;
	STRINGLIST houseItems;
	houseItems.resize( 0 );
	bool itemsWillDecay = false;
	bool isBoat			= false;
	UString houseDeed, tag, data, UTag;
	UI16 houseID		= 0;

	UString sect = "HOUSE " + UString::number( houseEntry );
	ScriptSection *House = FileLookup->FindEntry( sect, house_def );
	if( House == NULL )
		return;
	for( tag = House->First(); !House->AtEnd(); tag = House->Next() )
	{
		UTag = tag.upper();
		data = House->GrabData();
		if( UTag == "ID" )
			houseID = data.toUShort();
		else if( UTag == "SPACEX" )
			sx = static_cast<SI16>(data.toShort() + 1);
		else if( UTag == "SPACEY" )
			sy = static_cast<SI16>(data.toShort() + 1);
		else if( UTag == "CHARX" )
			cx = data.toShort();
		else if( UTag == "CHARY" )
			cy = data.toShort();
		else if( UTag == "CHARZ" )
			cz = data.toByte();
		else if( UTag == "Z" ) //to allow offsetting houses in Z to fix multis like Farmer's Cabin
			z = z + data.toByte();
		else if( UTag == "ITEMSDECAY" )
			itemsWillDecay = ( data.toShort() == 1 );
		else if( UTag == "HOUSE_ITEM" )
			houseItems.push_back( data );
		else if( UTag == "HOUSE_DEED" )
			houseDeed = data;
		else if( UTag == "BOAT" ) 
			isBoat = true;	//Boats
	}

	if( !houseID )
	{
		Console.Error( "Bad house script # %u!", houseEntry );
		return;
	}

	const bool isMulti = (houseID >= 0x4000);
	if( !CheckForValidHouseLocation( mSock, mChar, x, y, z, sx, sy, isBoat, isMulti ) )
		return;

	char temp[1024];
	CMultiObj *house = NULL;
	CItem *fakeHouse = NULL;
	if( isMulti )
	{
		if( (houseID%256) > 18 ) 
			sprintf( temp, "%s's house", mChar->GetName().c_str() ); //This will make the little deed item you see when you have showhs on say the person's name, thought it might be helpful for GMs.
		else 
			strcpy( temp, "a mast" );
		house = Items->CreateMulti( mChar, temp, houseID, isBoat );
		if( house == NULL )
			return;

		house->SetLocation( x, y, z );
		house->SetDecayable( itemsWillDecay );
		house->SetDeed( houseDeed ); // crackerjack 8/9/99 - for converting back *into* deeds
		house->SetOwner( mChar );
	}
	else
	{
		fakeHouse = Items->CreateItem( mSock, mChar, houseID, 1, 0, OT_ITEM );
		if( fakeHouse == NULL )
			return;
		fakeHouse->SetLocation( x, y, z );
		fakeHouse->SetOwner( mChar );
		fakeHouse->SetDecayable( false );
	}

	if( isBoat ) //Boats
	{
		CBoatObj *bObj = static_cast<CBoatObj *>(house);
		if( bObj == NULL || !CreateBoat( mSock, bObj, (houseID%256), houseEntry ) )
		{
			house->Delete();
			return;
		} 
	}

	mChar->GetSpeechItem()->Delete();
	mChar->SetSpeechItem( NULL );

	CreateHouseKey( mSock, mChar, house, houseID );
	if( !houseItems.empty() )
	{
		if( isMulti )
			fakeHouse = house;
		CreateHouseItems( mChar, houseItems, fakeHouse, houseID, x, y, z );
	}
	mChar->SetLocation( x + cx, y + cy, z + cz );

	//Teleport pets as well
	CDataList< CChar * > *myPets = mChar->GetPetList();
	for( CChar *pet = myPets->First(); !myPets->Finished(); pet = myPets->Next() )
	{
		if( ValidateObject( pet ) )
		{
			if( !pet->GetMounted() && pet->IsNpc() && objInRange( mChar, pet, DIST_SAMESCREEN ) )
				pet->SetLocation( mChar );
		}
	}
}

bool KillKeysFunctor( CBaseObject *a, UI32 &b, void *extraData )
{
	UI32 *ourData		= (UI32 *)extraData;
	SERIAL targSerial	= ourData[0];
	if( ValidateObject( a ) && a->CanBeObjType( OT_ITEM ) )
	{
		CItem *i = static_cast< CItem * >(a);
		if( i->GetType() == IT_KEY && i->GetTempVar( CITV_MORE ) == targSerial ) // make key uselss
			i->Delete();
	}
	return true;
}
void killKeys( SERIAL targSerial )
// This function is rather CPU-expensive, but AFAIK there is no
// better way to find all keys than to do it this way.. :/
{
	UI32 toPass[1];
	toPass[0]	= targSerial;
	UI32 b		= 0;
	ObjectFactory::getSingleton().IterateOver( OT_ITEM, b, toPass, &KillKeysFunctor );
}

// turn a house into a deed if possible. - crackerjack 8/9/99
// s = socket of player
// i = house object
// morex on the house item must be set to deed item # in items.scp.
void deedHouse( CSocket *s, CMultiObj *i )
{
	if( !ValidateObject( i ) )
		return;

	CChar *mChar = s->CurrcharObj();

	if( i->GetOwnerObj() == mChar || mChar->IsGM() )
	{
		CItem *pvDeed = NULL;
		char temp[1024];

		CItem *ii = Items->CreateScriptItem( s, mChar, i->GetDeed(), 1, OT_ITEM, true );	// need to make before delete
		if( ii == NULL ) 
			return;
        
		s->sysmessage( 578, i->GetName().c_str() );
		s->sysmessage( 579, ii->GetName().c_str() );

		CDataList< CChar * > *charList = i->GetCharsInMultiList();
		for( CChar *tChar = charList->First(); !charList->Finished(); tChar = charList->Next() )
		{
			if( !ValidateObject( tChar ) )
				continue;
			if( tChar->GetMultiObj() == i )
			{
				// Delete Player Vendors
				if( tChar->GetNPCAiType() == AI_PLAYERVENDOR ) // player vendor in right place
				{
					sprintf( temp, Dictionary->GetEntry( 580 ).c_str(), tChar->GetName().c_str() );
					pvDeed = Items->CreateItem( NULL, mChar, 0x14F0, 1, 0, OT_ITEM, true );
					if( ValidateObject( pvDeed ) )
					{
						pvDeed->SetName( temp );
						pvDeed->SetType( IT_PLAYERVENDORDEED );
						pvDeed->SetBuyValue( 2000 );
					}
					s->sysmessage( 581, tChar->GetName().c_str() );
					tChar->Delete();
				}
				else
					tChar->SetMulti( INVALIDSERIAL );
			}
		}

		SERIAL serialToMatch = i->GetSerial();
		s->sysmessage( 582 );
		killKeys( serialToMatch );
		i->Delete();
	}
}

UI08 AddToHouse( CMultiObj *house, CChar *toAdd, UI08 mode )
{
	if( !ValidateObject( house ) || !ValidateObject( toAdd ) )
		return 0;

	if( house->IsOwner( toAdd ) || house->IsOnBanList( toAdd ) )
		return 2;

	const UI08 OWNER	= 0;
	const UI08 BAN		= 1;
	SI16 sx, sy, ex, ey;
	Map->MultiArea( house, sx, sy, ex, ey );
	if( toAdd->GetX() >= sx && toAdd->GetY() >= sy && toAdd->GetX() <= ex && toAdd->GetY() <= ey )
	{
		switch( mode )
		{
			default:
			case OWNER:		house->AddAsOwner( toAdd );		break;
			case BAN:
				toAdd->SetLocation( ex, (ey+1), toAdd->GetZ() );
				house->AddToBanList( toAdd );
				break;
		}
		return 1;
	}
	else
		return 3;
}
bool DeleteFromHouseList( CMultiObj *house, CChar *toDelete, UI08 mode )
{
	if( !ValidateObject( house ) || !ValidateObject( toDelete ) )
		return false;

	if( !house->IsOwner( toDelete ) && !house->IsOnBanList( toDelete ) )
		return false;

	const UI08 OWNER	= 0;
	const UI08 BAN		= 1;
	switch( mode )
	{
		default:
		case OWNER:	// owner
			house->RemoveAsOwner( toDelete );
			break;
		case BAN:	// ban
			house->RemoveFromBanList( toDelete );
			break;
	}
	return true;
}

}
