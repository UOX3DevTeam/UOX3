#include "uox3.h"
#include "weight.h"
#include "CPacketSend.h"
#include "classes.h"
#include "regions.h"
#include "targeting.h"
#include "CResponse.h"
#include "Dictionary.h"

namespace UOX
{

#define XP 0
#define YP 1

enum ShipOffsets
{
	PORT_PLANK	= 0,
	STARB_PLANK,
	HOLD,
	TILLER
};

enum ShipItems
{
	PORT_P_C	= 0,// Port Plank Closed
	PORT_P_O,		// Port Plank Opened
	STAR_P_C,		// Starboard Plank Closed
	STAR_P_O,		// Starb Plank Open
	HOLDID,			// Hold
	TILLERID		// Tiller
};

//============================================================================================
//UooS Item translations - You guys are the men! :o)

//[4]=direction of ship
//[4]=Which Item (PT Plank, SB Plank, Hatch, TMan)
//[2]=Coord (x,y) offsets
SI16 iSmallShipOffsets[4][4][2] =
//	 X	 Y	 X	 Y	 X	 Y	 X	 Y
{ 
	{ {-2,0},	{2,0},	{0,-4},	{1,4} },//Dir
	{ {0,-2},	{0,2},	{4,0},	{-4,0}},
	{ {2,0},	{-2,0},	{ 0,4},	{0,-4}},
	{ {0,2},	{0,-2},	{-4,0},	{4,0} } 
};
//  P1    P2   Hold  Tiller
SI16 iMediumShipOffsets[4][4][2] =
//	 X	 Y	 X	 Y	 X	 Y	 X	 Y
{ 
    { {-2,0},	{2,0},	{0,-4},	{1,5} },
    { {0,-2},	{0,2},	{4,0},	{-5,0}},
    { {2,0},	{-2,0},	{0,4},	{0,-5}},
    { {0,2},	{0,-2},	{-4,0},	{5,0} } 
};
SI16 iLargeShipOffsets[4][4][2] =
//	 X	 Y	 X	 Y	 X	 Y	 X	 Y
{ 
    { {-2,-1},	{2,-1},	{0,-5},	{1,5} },
    { {1,-2},	{1,2},	{5,0},	{-5,0}},
    { {2,1},	{-2,1},	{0,5},	{0,5} },
    { {-1,2},	{-1,-2},{-5, 0},{5,0} } 
};
//Ship Items
//[4] = direction
//[6] = Which Item (PT Plank Up,PT Plank Down, SB Plank Up, SB Plank Down, Hatch, TMan)
UI08 cShipItems[4][6]=
{
    {0xB1, 0xD5, 0xB2, 0xD4, 0xAE, 0x4E},
    {0x8A, 0x89, 0x85, 0x84, 0x65, 0x53},
	{0xB2, 0xD4, 0xB1, 0xD5, 0xB9, 0x4B},
	{0x85, 0x84, 0x8A, 0x89, 0x93, 0x50} 
};
//============================================================================================

//o---------------------------------------------------------------------------o
//|	Function	-	CItem * GetBoat( cSocket *s )
//|	Programmer	-	Zippy
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get the boat a character is on
//o---------------------------------------------------------------------------o
CBoatObj * GetBoat( cSocket *s )
{
	CItem *boat = NULL;
	CChar *mChar = s->CurrcharObj();

	UI08 worldNumber = mChar->WorldNumber();
	if( ValidateObject( mChar->GetMultiObj() ) )
		return static_cast<CBoatObj *>(mChar->GetMultiObj());
	return static_cast<CBoatObj *>(findMulti( mChar ));
}

//o---------------------------------------------------------------------------o
//|	Function	-	void LeaveBoat( cSocket *s, CItem *p )
//|	Programmer	-	Zippy
//o---------------------------------------------------------------------------o
//|	Purpose		-	Leave a boat
//o---------------------------------------------------------------------------o
void LeaveBoat( cSocket *s, CItem *p )
{
	CItem *boat = GetBoat( s );
	if( !ValidateObject( boat ) )
		return;

	SI16 x2 = p->GetX();
	SI16 y2 = p->GetY();
	SI08 z = p->GetZ(), typ;
	CChar *mChar = s->CurrcharObj();
	UI08 worldNumber = mChar->WorldNumber();
	for( SI16 x = x2 - 2; x < x2 + 3; ++x )
	{
		for( SI16 y = y2 - 2; y < y2 + 3; ++y )
		{
			SI08 sz = Map->StaticTop( x, y, z, worldNumber );
			SI08 mz = Map->MapElevation( x, y, worldNumber );
			if( sz == ILLEGAL_Z )
				typ = 0;
			else
				typ	= 1;

			if( ( typ == 0 && mz != 5) || ( typ == 1 && sz != -5 ) )// everthing the blocks a boat is ok to leave the boat ... LB
			{
				CHARLIST *myPets = mChar->GetPetList();
				// Handle this via STL
				CHARLIST_ITERATOR I;
				for( I = myPets->begin(); I != myPets->end(); ++I )
				{
					CChar *pet = (*I);
					if( ValidateObject( pet ) )
					{
						if( !pet->IsMounted() && pet->IsNpc() && objInRange( mChar, pet, DIST_SAMESCREEN ) )
						{
							if( typ )
								pet->SetLocation( x, y, sz );
							else
								pet->SetLocation( x, y, mz );

							if( ValidateObject( pet->GetMultiObj() ) )
								pet->SetMulti( INVALIDSERIAL );
						}
					}
				}
				mChar->SetMulti( INVALIDSERIAL );

				if( typ )
					mChar->SetLocation( x, y, sz );
				else
					mChar->SetLocation( x, y, mz );

				s->sysmessage( 3 );
				return;
			}
		}
	}
	s->sysmessage( 4 );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void PlankStuff( cSocket *s, CItem *p )
//|	Programmer	-	Zippy
//o---------------------------------------------------------------------------o
//|	Purpose		-	If not on a boat, will send character to the plank, other
//|					wise will call OpenPlank to open/close the plank
//o---------------------------------------------------------------------------o
void PlankStuff( cSocket *s, CItem *p )
{
	CChar *mChar	= s->CurrcharObj();
	CItem *boat = GetBoat( s );
	if( !ValidateObject( boat ) )
	{
		mChar->SetLocation( p->GetX(), p->GetY(), p->GetZ() + 5 );
		SERIAL mser			= p->GetTempVar( CITV_MORE );
		CMultiObj *boat2	= static_cast< CMultiObj * >(calcItemObjFromSer( mser ));
		if( ValidateObject( boat2 ) )
		{
			CHARLIST *myPets = mChar->GetPetList();
			// Handle the STL
			CHARLIST_ITERATOR I;
			for( I = myPets->begin(); I != myPets->end(); ++I )
			{
				CChar *pet = (*I);
				if( ValidateObject( pet ) )
				{
					if( !pet->IsMounted() && pet->IsNpc() && objInRange( mChar, pet, DIST_SAMESCREEN ) )
					{
						pet->SetLocation( boat2->GetX() + 1, boat2->GetY() + 1, boat2->GetZ() + 4 );
						pet->SetMulti( boat2 );
					}
				}
			}
		}

		if( ValidateObject( boat2 ) )
		{
			s->sysmessage( 1 );
			mChar->SetMulti( boat2 );
		}
		else
			s->sysmessage( 2 );
	}
	else
		LeaveBoat( s, p );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void OpenPlank( CItem *p )
//|	Programmer	-	Zippy
//o---------------------------------------------------------------------------o
//|	Purpose		-	Open / Close a plank
//o---------------------------------------------------------------------------o
void OpenPlank( CItem *p )
{
	switch( p->GetID( 2 ) )
	{
		//Open plank->
		case 0xE9: p->SetID( 0x84, 2 ); break;
		case 0xB1: p->SetID( 0xD5, 2 ); break;
		case 0xB2: p->SetID( 0xD4, 2 ); break;
		case 0x8A: p->SetID( 0x89, 2 ); break;
		case 0x85: p->SetID( 0x84, 2 ); break;
			//Close Plank->
		case 0x84: p->SetID( 0xE9, 2 ); break;
		case 0xD5: p->SetID( 0xB1, 2 ); break;
		case 0xD4: p->SetID( 0xB2, 2 ); break;
		case 0x89: p->SetID( 0x8A, 2 ); break;
		default: 	Console.Warning( 2, "Invalid plank ID called! Plank 0x%X '%s' [%u]", p->GetSerial(), p->GetName().c_str(), p->GetID() );
			break;
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool BlockBoat( CItem *b, SI16 xmove, SI16 ymove, UI08 dir )
//|	Programmer	-	Zippy
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check if a boat can move to a certain loc
//o---------------------------------------------------------------------------o
bool BlockBoat( CItem *b, SI16 xmove, SI16 ymove, UI08 dir )
{
	MapStaticIterator *msi;
	staticrecord *stat = NULL;
	map_st map;
	CLand land;
	CTile tile;
	SI16 cx = b->GetX(), cy = b->GetY();
	SI16 x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	UI08 type = 0;

	cx += xmove;
	cy += ymove;

	switch( b->GetID( 2 ) )
	{
		case 0:
		case 1:
		case 2:
		case 3://small
		case 4:
		case 5:
		case 6:
		case 7://small dragon
			type = 1;
			break;

		case 8:
		case 9:
		case 10:
		case 11://medium
		case 12:
		case 13:
		case 14:
		case 15://medium dragon
			type = 2;
			break;
		case 16:
		case 17:
		case 18:
		case 19://large
		case 20:
		case 21:
		case 22:
		case 23://large dragon
			type = 3;
			break;
		default:
			return true;
	}

	//small = 5,11 
	//medium = 5, 13
	//large = 5, 15
	switch( dir )
	{
		case NORTHEAST:	// U
		case SOUTHWEST:	// D
		case NORTH:	// N
		case SOUTH:	// S
			x1 = cx - 2;
			x2 = cx + 2;
			switch( type )
			{
				case 1: y1 = cy - 6; y2 = cy + 6; break;
				case 2: y1 = cy - 6; y2 = cy + 7; break;
				case 3: y1 = cy - 8; y2 = cy + 8; break;
				default:	Console.Error( 2, " Fallout of North/South switch() statement in cBoats::BlockBoat()" );	break;
			}
			break;
		case EAST: // E
		case WEST: // W
		case SOUTHEAST:	// E
		case NORTHWEST:	// W
			y1 = cy - 2;
			y2 = cy + 2;
			switch( type )
			{
				case 1: x1 = cx - 6; x2 = cx + 6; break;
				case 2: x1 = cx - 7; x2 = cx + 7; break;
				case 3: x1 = cx - 8; x2 = cx + 8; break;
				default:	Console.Error( 2, " Fallout of East/West switch() statement in cBoats::BlockBoat()" );	break;
			}
			break;
		default: return true;
	}

	UI08 worldNumber = b->WorldNumber();
	for( SI16 x = x1; x < x2; ++x )
	{
		for( SI16 y = y1; y < y2; ++y )
		{
			SI08 sz = Map->StaticTop( x, y, b->GetZ(), worldNumber );

			if( sz == ILLEGAL_Z ) //map tile
			{
				map = Map->SeekMap0( x, y, worldNumber );
				Map->SeekLand( map.id, &land );
				if( map.z >= -5 && !land.LiquidWet() && strcmp( land.Name(), "water" ) )//only tiles on/above the water
					return true;
			}
			else
			{ //static tile
				msi = new MapStaticIterator( x, y, worldNumber );
				while( stat = msi->Next() )
				{
					msi->GetTile( &tile );
					SI08 zt = stat->zoff + tile.Height();
					if( !tile.LiquidWet() && zt > -5 && zt <= 15 && strcmp( (char*)tile.Name(), "water" ) )
					{
						delete msi;
						return true;
					}
				}
				delete msi;
			}
		}
	}
	return false;
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool CreateBoat( cSocket *s, CItem *b, char id2, UI08 boattype )
//|	Programmer	-	Zippy
//o---------------------------------------------------------------------------o
//|	Purpose		-	Create a boat
//o---------------------------------------------------------------------------o
bool CreateBoat( cSocket *s, CBoatObj *b, UI08 id2, UI08 boattype )
{
	if( !ValidateObject( b ) )
	{
		s->sysmessage( 5 );
		return false;
	}
	switch( id2 )
	{
		case 0x00:
		case 0x04:
		case 0x08:
		case 0x0C:
		case 0x10:
		case 0x14:
			break;
		default:
			s->sysmessage( 6 );
			return false;
	}

	if( BlockBoat( b, 0, 0, 0 ) )
	{
		s->sysmessage( 7 );
		return false;
	}

	CChar *mChar = s->CurrcharObj();
	b->SetTempVar( CITV_MORE, calcserial( id2, id2+3, b->GetTempVar( CITV_MORE, 3 ), b->GetTempVar( CITV_MORE, 4 ) ) );
	b->SetType( IT_PLANK );// Boat type
	b->SetZ( -5 );// Z in water
	b->SetName( Dictionary->GetEntry( 1408 ) );//Name is something other than "%s's house"
	SERIAL serial = b->GetSerial();
	CItem *tiller = Items->CreateItem( NULL, mChar, 0x3E4E, 1, 0, OT_ITEM );
	if( tiller == NULL )
		return false;
	tiller->SetName( Dictionary->GetEntry( 1409 ) );
	tiller->SetPriv( 0 );
	tiller->SetType( IT_TILLER );
	tiller->SetTempVar( CITV_MORE, serial );
	tiller->SetTempVar( CITV_MOREX, boattype );

	CItem *p2 = Items->CreateItem( NULL, mChar, 0x3EB2, 1, 0, OT_ITEM );//Plank2 is on the RIGHT side of the boat
	if( p2 == NULL )
		return false;
	p2->SetType( IT_PLANK );
	p2->SetTempVar( CITV_MORE, serial );
	p2->SetPriv( 0 );//Nodecay

	CItem *p1 = Items->CreateItem( NULL, mChar, 0x3EB1, 1, 0, OT_ITEM );//Plank1 is on the LEFT side of the boat
	if( p1 == NULL )
		return false;
	p1->SetType( IT_PLANK );//Boat type
	p1->SetTempVar( CITV_MORE, serial );
	p1->SetPriv( 0 );

	CItem *hold = Items->CreateItem( NULL, mChar, 0x3EAE, 1, 0, OT_ITEM );
	if( hold == NULL )
		return false;
	hold->SetTempVar( CITV_MORE, serial );
	hold->SetType( IT_CONTAINER );//Conatiner
	hold->SetPriv( 0 );

	b->SetTiller( tiller->GetSerial() );
	b->SetPlank( 0, p1->GetSerial() );// Store the other stuff anywhere it will fit :-)
	b->SetPlank( 1, p2->GetSerial() );
	b->SetHold( hold->GetSerial() );

	SI16 x = b->GetX(), y = b->GetY();
	SI08 z = b->GetZ();
	switch( id2 ) //Give everything the right Z for it size boat
	{
		case 0x00:
		case 0x04:
			tiller->SetLocation( x + 1, y + 4, -5 );
			p1->SetLocation( x - 2, y, -5 );
			p2->SetLocation( x + 2, y, -5 );
			hold->SetLocation( x, y - 4, -5 );
			break;
		case 0x08:
		case 0x0C:
			tiller->SetLocation( x + 1, y + 5, -5 );
			p1->SetLocation( x - 2, y, -5 );
			p2->SetLocation( x + 2, y, -5 );
			hold->SetLocation( x, y - 4, -5 );
			break;
		case 0x10:
		case 0x14:
			tiller->SetLocation( x + 1, y + 5, -5 );
			p1->SetLocation( x - 2, y - 1, -5 );
			p2->SetLocation( x + 2, y - 1, -5 );
			hold->SetLocation( x, y - 5, -5 );
			break;
	}
	//their x pos is set by BuildHouse(), so just fix their Z...
	mChar->SetZ( z + 3 );
	mChar->SetDispZ( z + 3 );
	mChar->SetMulti( b );
	return true;
}

void CheckDirection( UI08 dir, SI16& tx, SI16& ty )
{
	switch( dir )
	{
		case NORTH:		--ty;			break;
		case NORTHEAST:	++tx;	--ty;	break;
		case EAST:		++tx;			break;
		case SOUTHEAST:	++tx;	++ty;	break;
		case SOUTH:		++ty;			break;
		case SOUTHWEST:	--tx;	++ty;	break;
		case WEST:		--tx;			break;
		case NORTHWEST:	--tx;	--ty;	break;
		default:						break;
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void MoveBoat( cSocket *s, UI08 dir, CItem *boat )
//|	Programmer	-	Zippy
//o---------------------------------------------------------------------------o
//|	Purpose		-	Move the boat and everything on it 1 tile
//o---------------------------------------------------------------------------o
void MoveBoat( UI08 dir, CBoatObj *boat )
{
	CItem *tiller = calcItemObjFromSer( boat->GetTiller() );
	CItem *p1 = calcItemObjFromSer( boat->GetPlank( 0 ) );
	CItem *p2 = calcItemObjFromSer( boat->GetPlank( 1 ) );
	CItem *hold = calcItemObjFromSer( boat->GetHold() );

	if( !ValidateObject( boat ) )
		return;

	if( !ValidateObject( tiller ) || !ValidateObject( p1 ) || !ValidateObject( p2 ) || !ValidateObject( hold ) )
		return;

	CPPauseResume prSend( 1 );
	SOCKLIST nearbyChars = FindNearbyPlayers( boat, DIST_BUILDRANGE );
	SOCKLIST_ITERATOR cIter;
	for( cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
	{
		(*cIter)->Send( &prSend );
	}

	SI16 tx = 0, ty = 0;
	CheckDirection( dir&0x0F, tx, ty );

	SI16 x = boat->GetX(), y = boat->GetY();
	prSend.Mode( 0 );
	if( ( (x + tx) <= 200 || (x + tx) >= 6000 ) && ( (y + ty) <= 200 || (y + ty) >= 4900 ) )
	{
		boat->SetMoveType( 0 );
		for( cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
		{
			(*cIter)->Send( &prSend );
			tiller->itemTalk( (*cIter), 8 );
		}
		return;
	}
	if( BlockBoat( boat, tx, ty, dir ) )
	{
		boat->SetMoveType( 0 );
		for( cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter  )
		{
			(*cIter)->Send( &prSend );
			tiller->itemTalk( (*cIter), 9 );
		}
		return;
	}

	//Move all the special items
	boat->IncLocation( tx, ty );
	tiller->IncLocation( tx, ty );
	p1->IncLocation( tx, ty );
	//p2->IncLocation( tx, ty );
	hold->IncLocation( tx, ty );

	CBoatObj *realBoat = static_cast< CBoatObj *>(boat);
	for( CItem *bItem = realBoat->FirstItemMulti(); !realBoat->FinishedItemMulti(); bItem = realBoat->NextItemMulti() )
	{
		if( !ValidateObject( bItem ) )
			continue;
		bItem->IncLocation( tx, ty );
	}

	for( CChar *bChar = realBoat->FirstCharMulti(); !realBoat->FinishedCharMulti(); bChar = realBoat->NextCharMulti() )
	{
		if( !ValidateObject( bChar ) )
			continue;
		bChar->SetLocation( bChar->GetX() + tx, bChar->GetY() + ty, bChar->GetZ() );
	}
	for( cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter  )
	{
		(*cIter)->Send( &prSend );
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void TurnStuff( CItem *b, CItem *i, bool rightTurn )
//|	Programmer	-	Zippy
//o---------------------------------------------------------------------------o
//|	Purpose		-	Turn an item on a boat
//o---------------------------------------------------------------------------o
void TurnStuff( CItem *b, cBaseObject *i, bool rightTurn )
{
	if( !ValidateObject( b ) )
		return;

	SI16 dx = static_cast<SI16>(i->GetX() - b->GetX());	//get their distance x
	SI16 dy = static_cast<SI16>(i->GetY() - b->GetY());//and distance Y

	if( rightTurn )
		i->SetLocation( static_cast<SI16>(b->GetX() - dy), static_cast<SI16>(b->GetY() + dx), b->GetZ() );
	else
		i->SetLocation( static_cast<SI16>(b->GetX() + dy), static_cast<SI16>(b->GetY() - dx), b->GetZ() );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void TurnBoat( CItem *b, bool rightTurn )
//|	Programmer	-	Zippy
//o---------------------------------------------------------------------------o
//|	Purpose		-	Turn the boat and use TurnStuff() to turn all items/chars on it
//o---------------------------------------------------------------------------o
void TurnBoat( CBoatObj *b, bool rightTurn )
{
	if( !ValidateObject( b ) )
		return;

	SI16 id2	= b->GetID( 2 );
	UI08 olddir = b->GetDir();
	UI32 a		= 0;

	CPPauseResume prSend( 1 );
	SOCKLIST nearbyChars = FindNearbyPlayers( b, DIST_BUILDRANGE );
	SOCKLIST_ITERATOR cIter;
	for( cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter  )
	{
		(*cIter)->Send( &prSend );
	}

	CItem *tiller = calcItemObjFromSer( b->GetTiller() );
	CItem *p1 = calcItemObjFromSer( b->GetPlank( 0 ) );
	CItem *p2 = calcItemObjFromSer( b->GetPlank( 1 ) );
	CItem *hold = calcItemObjFromSer( b->GetHold() );

	if( !ValidateObject( tiller ) || !ValidateObject( p1 ) || !ValidateObject( p2 ) || !ValidateObject( hold ) )
		return;

	if( rightTurn )
	{
		b->SetDir( olddir + 2 );
		++id2;
	}
	else
	{
		b->SetDir( olddir - 2 );
		--id2;
	}
	if( b->GetDir() > 250 )
		b->SetDir( b->GetDir() + 4 );
	if( b->GetDir() > NORTHWEST )
		b->SetDir( b->GetDir() - 4 );//Make sure we dont have any DIR errors

	if( id2 < b->GetTempVar( CITV_MORE, 1 ) )
		id2 += 4;//make sure we don't have any id errors either
	if( id2 > b->GetTempVar( CITV_MORE, 2 ) )
		id2 -= 4;//Now you know what the min/max id is for :-)

	prSend.Mode( 0 );
	if( BlockBoat( b, 0, 0, b->GetDir() ) )
	{
		b->SetDir( olddir );
		for( cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter  )
		{
			(*cIter)->Send( &prSend );
			tiller->itemTalk( (*cIter), 1410, 0, 0x0481 );
		}
		return;
	}

	b->SetID( static_cast<UI08>(id2), 2 );//set the id

	if( b->GetID( 2 ) == b->GetTempVar( CITV_MORE, 1 ) )
		b->SetDir( NORTH );//extra DIR error checking
	if( b->GetID( 2 ) == b->GetTempVar( CITV_MORE, 2 ) )
		b->SetDir( WEST );


	CBoatObj *realBoat = static_cast< CBoatObj * >(b);

	for( CItem *bItem = realBoat->FirstItemMulti(); !realBoat->FinishedItemMulti(); bItem = realBoat->NextItemMulti() )
	{
		if( !ValidateObject( bItem ) )
			continue;
		TurnStuff( b, bItem, rightTurn );
	}
	for( CChar *bChar = realBoat->FirstCharMulti(); !realBoat->FinishedCharMulti(); bChar = realBoat->NextCharMulti() )
	{
		if( !ValidateObject( bChar ) )
			continue;
		TurnStuff( b, bChar, rightTurn );
	}

	UI08 dir = (b->GetDir()&0x0F)/2;

	p1->SetLocation( b );
	p1->SetID( cShipItems[dir][PORT_P_C], 2 );//change the ID

	p2->SetLocation( b );
	p2->SetID( cShipItems[dir][STAR_P_C], 2 );

	tiller->SetLocation( b );
	tiller->SetID( cShipItems[dir][TILLERID], 2 );

	hold->SetLocation( b );
	hold->SetID( cShipItems[dir][HOLDID], 2 );

	switch( b->GetTempVar( CITV_MORE, 1 ) )//Now set what size boat it is and move the specail items
	{
		case 0x00:
		case 0x04:
			p1->IncLocation( iSmallShipOffsets[dir][PORT_PLANK][XP], iSmallShipOffsets[dir][PORT_PLANK][YP] );
			p2->IncLocation( iSmallShipOffsets[dir][STARB_PLANK][XP], iSmallShipOffsets[dir][STARB_PLANK][YP] );
			tiller->IncLocation( iSmallShipOffsets[dir][TILLER][XP], iSmallShipOffsets[dir][TILLER][YP] );
			hold->IncLocation( iSmallShipOffsets[dir][HOLD][XP], iSmallShipOffsets[dir][HOLD][YP] );
			break;
		case 0x08:
		case 0x0C:
			p1->IncLocation( iMediumShipOffsets[dir][PORT_PLANK][XP], iMediumShipOffsets[dir][PORT_PLANK][YP] );
			p2->IncLocation( iMediumShipOffsets[dir][STARB_PLANK][XP], iMediumShipOffsets[dir][STARB_PLANK][YP] );
			tiller->IncLocation( iMediumShipOffsets[dir][TILLER][XP], iMediumShipOffsets[dir][TILLER][YP] );
			hold->IncLocation( iMediumShipOffsets[dir][HOLD][XP], iMediumShipOffsets[dir][HOLD][YP] );
			break;
		case 0x10:
		case 0x14:
			p1->IncLocation( iLargeShipOffsets[dir][PORT_PLANK][XP], iLargeShipOffsets[dir][PORT_PLANK][YP] );
			p2->IncLocation( iLargeShipOffsets[dir][STARB_PLANK][XP], iLargeShipOffsets[dir][STARB_PLANK][YP] );
			tiller->IncLocation( iLargeShipOffsets[dir][TILLER][XP], iLargeShipOffsets[dir][TILLER][YP] );
			hold->IncLocation( iLargeShipOffsets[dir][HOLD][XP], iLargeShipOffsets[dir][HOLD][YP] );
			break;
		default: Console.Error( 2, "TurnBoat() more1 error! more1 = %c not found!", b->GetTempVar( CITV_MORE, 1 ) );
	}

	for( cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter  )
		(*cIter)->Send( &prSend );
}

void TurnBoat( cSocket *mSock, CBoatObj *myBoat, CItem *tiller, UI08 dir, bool rightTurn )
{
	SI16 tx = 0, ty = 0;
	CheckDirection( dir, tx, ty );

	if( !BlockBoat( myBoat, tx, ty, dir ) )
	{
		tiller->itemTalk( mSock, 10 );
		TurnBoat( myBoat, rightTurn );
	}
	else
	{
		myBoat->SetMoveType( 0 );
		tiller->itemTalk( mSock, 9 ); 
	}
}
//o---------------------------------------------------------------------------o
//|	Function	-	void Speech( cSocket *mSock, CChar *mChar )
//|	Programmer	-	Zippy
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check for boat commands
//o---------------------------------------------------------------------------o
void CBoatResponse::Handle( cSocket *mSock, CChar *mChar )
{
	if( mSock == NULL )
		return;

	CBoatObj *boat = GetBoat( mSock );
	if( !ValidateObject( boat ) )
		return;

	UI08 dir = boat->GetDir()&0x0F;

	CItem *tiller = calcItemObjFromSer( boat->GetTiller() );

	UnicodeTypes mLang = mSock->Language();
	switch( mSock->TriggerWord() )
	{
	case TW_BOATTURNRIGHT:
	case TW_BOATSTARBOARD:
		if( dir >= 2 )
			dir -= 2;
		else
			dir	+= 6;
		TurnBoat( mSock, boat, tiller, dir&0x0F, true );
		break;
	case TW_BOATTURNLEFT:
	case TW_BOATPORT:
		dir += 2;
		if( dir > 7 )
			dir -= 8;
		TurnBoat( mSock, boat, tiller, dir&0x0F, false );
		break;
	case TW_BOATTURNAROUND:
		tiller->itemTalk( mSock, 10 );
		TurnBoat( boat, true );
		TurnBoat( boat, true );
		break;
	case TW_BOATLEFT:
		if( dir >= 2 )
			dir -= 2;
		else
			dir	+= 6;
		tiller->itemTalk( mSock, 10 );
		MoveBoat( dir, boat );
		break;
	case TW_BOATRIGHT:
		dir += 2;
		if( dir > 7 )
			dir -= 8;
		tiller->itemTalk( mSock, 10 );
		MoveBoat( dir, boat );
		break;
	case TW_SETNAME:
		char msg[512];
		strcpy( msg, UString( ourText ).upper().c_str() );
		char *cmd; 
		cmd = strstr( msg, Dictionary->GetEntry( 1425, mLang ).c_str() ); // note: also checking for space
		if( !cmd )
		{
			tiller->itemTalk( mSock, 11 );
			return;
		}
		cmd += 9;
		while( *cmd && *cmd == ' ' )
			++cmd; // remove any extra spaces
		if( !(*cmd) )
		{
			tiller->itemTalk( mSock, 12 );
			return;
		}

		char tempname[512];
		sprintf( tempname, Dictionary->GetEntry( 1426, mLang ).c_str(), &ourText[msg - cmd] );
		tiller->SetName( tempname );
		break;
	default:
		break;
	}
}

void killKeys( SERIAL targSerial );
//o---------------------------------------------------------------------------o
//|	Function	-	void ModelBoat( cSocket *s, CItem *i )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Turn a boat into a boat model that can be re-placed
//o---------------------------------------------------------------------------o
void ModelBoat( cSocket *s, CBoatObj *i )
{
	CItem *tiller	= calcItemObjFromSer( i->GetTiller() );
	CItem *p1		= calcItemObjFromSer( i->GetPlank( 0 ) );
	CItem *p2		= calcItemObjFromSer( i->GetPlank( 1 ) );
	CItem *hold		= calcItemObjFromSer( i->GetHold() );

	CChar *mChar = s->CurrcharObj();

	SERIAL serial = p1->GetTempVar( CITV_MORE );
	if( i->GetOwnerObj() == mChar )
	{
		if( i->NumItemMulti() > 4 || i->NumCharMulti() > 0 )
		{
			s->sysmessage( "This boat must be empty before it can be converted to a model!" );
			return;
		}

		CItem *model = Items->CreateItem( s, mChar, 0x14f3, 1, 0, OT_ITEM, true );
		if( model == NULL )
		{
			Console.Error( 3, " Turning boat into model failed on model creation, attempted by character serial %X", mChar->GetSerial() );
			return;
		}

		model->SetTempVar( CITV_MOREX, tiller->GetTempVar( CITV_MOREX ) );
		Weight->subtractItemWeight( mChar, model );
		model->SetWeight( 0 );
		Weight->addItemWeight( mChar, model );
		model->SetType( IT_MODELMULTI );
		model->SetMovable( 0 );

		for( CItem *a = hold->FirstItem(); !hold->FinishedItems(); a = hold->NextItem() )
			a->Delete();

		tiller->Delete();
		p1->Delete();
		p2->Delete();
		hold->Delete();
		i->Delete();
		killKeys( serial );
	}
}

}
