#include "uox3.h"
#include "weight.h"
#include "CPacketSend.h"
#include "classes.h"
#include "regions.h"
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
const SI16 iSmallShipOffsets[4][4][2] =
//	 X	 Y	 X	 Y	 X	 Y	 X	 Y
{ 
	{ {-2,0},	{2,0},	{0,-4},	{1,4} },//Dir
	{ {0,-2},	{0,2},	{4,0},	{-4,0}},
	{ {2,0},	{-2,0},	{ 0,4},	{0,-4}},
	{ {0,2},	{0,-2},	{-4,0},	{4,0} } 
};
//  P1    P2   Hold  Tiller
const SI16 iMediumShipOffsets[4][4][2] =
//	 X	 Y	 X	 Y	 X	 Y	 X	 Y
{ 
    { {-2,0},	{2,0},	{0,-4},	{1,5} },
    { {0,-2},	{0,2},	{4,0},	{-5,0}},
    { {2,0},	{-2,0},	{0,4},	{0,-5}},
    { {0,2},	{0,-2},	{-4,0},	{5,0} } 
};
const SI16 iLargeShipOffsets[4][4][2] =
//	 X	 Y	 X	 Y	 X	 Y	 X	 Y
{ 
    { {-2,-1},	{2,-1},	{0,-5},	{1,5} },
    { {1,-2},	{1,2},	{5,0},	{-5,0}},
    { {2,1},	{-2,1},	{0,5},	{0,-5} },
    { {-1,2},	{-1,-2},{-5, 0},{5,0} } 
};
//Ship Items
//[4] = direction
//[6] = Which Item (PT Plank Up,PT Plank Down, SB Plank Up, SB Plank Down, Hatch, TMan)
const UI08 cShipItems[4][6]=
{
    {0xB1, 0xD5, 0xB2, 0xD4, 0xAE, 0x4E},
    {0x8A, 0x89, 0x85, 0x84, 0x65, 0x53},
	{0xB2, 0xD4, 0xB1, 0xD5, 0xB9, 0x4B},
	{0x85, 0x84, 0x8A, 0x89, 0x93, 0x50} 
};
//============================================================================================

//o---------------------------------------------------------------------------o
//|	Function	-	CItem * GetBoat( CSocket *s )
//|	Programmer	-	Zippy
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get the boat a character is on
//o---------------------------------------------------------------------------o
CBoatObj * GetBoat( CSocket *s )
{
	CChar *mChar = s->CurrcharObj();

	if( ValidateObject( mChar->GetMultiObj() ) )
		return static_cast<CBoatObj *>(mChar->GetMultiObj());
	return static_cast<CBoatObj *>(findMulti( mChar ));
}

//o---------------------------------------------------------------------------o
//|	Function	-	void LeaveBoat( CSocket *s, CItem *p )
//|	Programmer	-	Zippy
//o---------------------------------------------------------------------------o
//|	Purpose		-	Leave a boat
//o---------------------------------------------------------------------------o
void LeaveBoat( CSocket *s, CItem *p )
{
	CBoatObj *boat = GetBoat( s );
	if( !ValidateObject( boat ) )
		return;

	const SI16 x2 = p->GetX();
	const SI16 y2 = p->GetY();
	CChar *mChar = s->CurrcharObj();
	UI08 worldNumber = mChar->WorldNumber();
	for( SI16 x = x2 - 2; x < x2 + 3; ++x )
	{
		for( SI16 y = y2 - 2; y < y2 + 3; ++y )
		{
			SI08 z = Map->Height( x, y, mChar->GetZ(), worldNumber );
			if( Map->ValidMultiLocation( x, y, z, worldNumber, true ) && !findMulti( x, y, z, worldNumber ) )
			{
				mChar->SetLocation( x, y, z, worldNumber );
				CDataList< CChar * > *myPets = mChar->GetPetList();
				for( CChar *pet = myPets->First(); !myPets->Finished(); pet = myPets->Next() )
				{
					if( ValidateObject( pet ) )
					{
						if( !pet->GetMounted() && pet->IsNpc() && objInRange( mChar, pet, DIST_SAMESCREEN ) )
							pet->SetLocation( x, y, z, worldNumber );
					}
				}
				s->sysmessage( 3 );
				return;
			}
		}
	}
	s->sysmessage( 4 );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void PlankStuff( CSocket *s, CItem *p )
//|	Programmer	-	Zippy
//o---------------------------------------------------------------------------o
//|	Purpose		-	If not on a boat, will send character to the plank, other
//|					wise will call OpenPlank to open/close the plank
//o---------------------------------------------------------------------------o
void PlankStuff( CSocket *s, CItem *p )
{
	CChar *mChar	= s->CurrcharObj();
	CBoatObj *boat	= GetBoat( s );
	if( !ValidateObject( boat ) )
	{
		mChar->SetLocation( p->GetX(), p->GetY(), p->GetZ() + 3 );
		CMultiObj *boat2	= p->GetMultiObj();
		if( ValidateObject( boat2 ) )
		{
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

		if( ValidateObject( boat2 ) )
			s->sysmessage( 1 );
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
		default: 	Console.Warning( "Invalid plank ID called! Plank 0x%X '%s' [%u]", p->GetSerial(), p->GetName().c_str(), p->GetID() );
			break;
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool BlockBoat( CItem *b, SI16 xmove, SI16 ymove, UI08 dir )
//|	Programmer	-	Zippy
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check if a boat can move to a certain loc
//o---------------------------------------------------------------------------o
bool BlockBoat( CBoatObj *b, SI16 xmove, SI16 ymove, UI08 moveDir, UI08 boatDir, bool turnBoat )
{
	map_st map;
	SI16 cx = b->GetX(), cy = b->GetY();
	const SI08 cz = b->GetZ();
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

	// If boat-movement includes turning, boat-direction needs to be switched around
	if( turnBoat )
	{
		switch( boatDir&0x0F )
		{
			case NORTHEAST:	// U
			case SOUTHWEST:	// D
			case NORTH:	// N
			case SOUTH:	// S
				boatDir = 2;
				break;
			case EAST: // E
			case WEST: // W
			case SOUTHEAST:	// E
			case NORTHWEST:	// W
				boatDir = 0;
				break;
			default: break;
		}
	}

	//small = 5,11 
	//medium = 5, 13
	//large = 5, 15
	switch( moveDir&0x0F )
	{
		case NORTHEAST:	// U
		case SOUTHWEST:	// D
		case NORTH:	// N
		case SOUTH:	// S
			switch( boatDir&0x0F )
			{
				case NORTHEAST:	// U
				case SOUTHWEST:	// D
				case NORTH:	// N
				case SOUTH:	// S
					x1 = cx - 2; //Width of N/S ship as it moves N/S
					x2 = cx + 3; //Width of N/S ship as it moves N/S
					switch( type )
					{
						case 1: y1 = cy - 6; y2 = cy + 6; break; //Length of N/S ship as it moves N/S
						case 2: y1 = cy - 7; y2 = cy + 7; break; //Length of N/S ship as it moves N/S
						case 3: y1 = cy - 8; y2 = cy + 8; break; //Length of N/S ship as it moves N/S
						default:	Console.Error( " Fallout of North/South switch() statement in cBoats::BlockBoat()" );	break;
					}
					break;
				case EAST: // E
				case WEST: // W
				case SOUTHEAST:	// E
				case NORTHWEST:	// W
					y1 = cy - 2; //Width of E/W ship as it moves N/S
					y2 = cy + 3; //Width of E/W ship as it moves N/S
					switch( type )
					{
						case 1: x1 = cx - 6; x2 = cx + 6; break; //Length of E/W ship as it moves N/S
						case 2: x1 = cx - 7; x2 = cx + 7; break; //Length of E/W ship as it moves N/S
						case 3: x1 = cx - 8; x2 = cx + 8; break; //Length of E/W ship as it moves N/S
						default:	Console.Error( " Fallout of East/West switch() statement in cBoats::BlockBoat()" );	break;
					}
					break;
				default:	Console.Error( " Fallout of boatDir.switch() statement in cBoats::BlockBoat()" );	break;
			}
			break;
		case EAST: // E
		case WEST: // W
		case SOUTHEAST:	// E
		case NORTHWEST:	// W
			switch( boatDir&0x0F )
			{
				case EAST: // E
				case WEST: // W
				case SOUTHEAST:	// E
				case NORTHWEST:	// W
					y1 = cy - 2; //Width of E/W ship as it moves E/W
					y2 = cy + 3; //Width of E/W ship as it moves E/W
					switch( type )
					{
						case 1: x1 = cx - 6; x2 = cx + 6; break; //Length of E/W ship as it moves E/W
						case 2: x1 = cx - 7; x2 = cx + 7; break; //Length of E/W ship as it moves E/W
						case 3: x1 = cx - 8; x2 = cx + 8; break; //Length of E/W ship as it moves E/W
						default:	Console.Error( " Fallout of East/West switch() statement in cBoats::BlockBoat()" );	break;
					}
					break;
				case NORTHEAST:	// U
				case SOUTHWEST:	// D
				case NORTH:	// N
				case SOUTH:	// S
					x1 = cx - 2; //Width of N/S ship as it moves E/W
					x2 = cx + 3; //Width of N/S ship as it moves E/W
					switch( type )
					{
						case 1: y1 = cy - 6; y2 = cy + 6; break; //Length of N/S ship as it moves E/W
						case 2: y1 = cy - 7; y2 = cy + 7; break; //Length of N/S ship as it moves E/W
						case 3: y1 = cy - 8; y2 = cy + 8; break; //Length of N/S ship as it moves E/W
						default:	Console.Error( " Fallout of North/South switch() statement in cBoats::BlockBoat()" );	break;
					}
					break;
				default:	Console.Error( " Fallout of boatDir.switch() statement in cBoats::BlockBoat()" );	break;
			}
			break;
		default: return true;
	}

	CStaticIterator *msi;
	UI08 worldNumber = b->WorldNumber();
	for( SI16 x = x1; x < x2; ++x )
	{
		for( SI16 y = y1; y < y2; ++y )
		{
			SI08 sz = Map->StaticTop( x, y, b->GetZ(), worldNumber, MAX_Z_STEP );

			if( sz == ILLEGAL_Z ) //map tile
			{
				map = Map->SeekMap( x, y, worldNumber );
				if( cwmWorldState->ServerData()->ServerUsingHSTiles() )
				{
					//7.0.9.0 tiledata and later
					CLandHS& land = Map->SeekLandHS( map.id );
					if( map.z >= cz && !land.CheckFlag( TF_WET ) && strcmp( land.Name(), "water" ) )//only tiles on/above the water
						return true;
				}
				else
				{
					//7.0.8.2 tiledata and earlier
					CLand& land = Map->SeekLand( map.id );
					if( map.z >= cz && !land.CheckFlag( TF_WET ) && strcmp( land.Name(), "water" ) )//only tiles on/above the water
						return true;
				}		
			}
			else
			{ //static tile
				msi = new CStaticIterator( x, y, worldNumber );
				for( Static_st *stat = msi->First(); stat != NULL; stat = msi->Next() )
				{
					if( cwmWorldState->ServerData()->ServerUsingHSTiles() )
					{
						//7.0.9.0 data and later
						CTileHS& tile = Map->SeekTileHS( stat->itemid );
						SI08 zt = stat->zoff + tile.Height();
						if( !tile.CheckFlag( TF_WET ) && zt >= cz && zt <= (cz + 20) && strcmp( (char*)tile.Name(), "water" ) )
						{
							delete msi;
							return true;
						}
					}
					else
					{
						//7.0.8.2 data and earlier
						CTile& tile = Map->SeekTile( stat->itemid );
						SI08 zt = stat->zoff + tile.Height();
						if( !tile.CheckFlag( TF_WET ) && zt >= cz && zt <= (cz + 20) && strcmp( (char*)tile.Name(), "water" ) )
						{
							delete msi;
							return true;
						}
					}		
				}
				delete msi;
			}
		}
	}
	return false;
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool CreateBoat( CSocket *s, CItem *b, char id2, UI08 boattype )
//|	Programmer	-	Zippy
//o---------------------------------------------------------------------------o
//|	Purpose		-	Create a boat
//o---------------------------------------------------------------------------o
bool CreateBoat( CSocket *s, CBoatObj *b, UI08 id2, UI08 boattype )
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

	if( BlockBoat( b, 0, 0, 0, 0, false ) )
	{
		s->sysmessage( 7 );
		return false;
	}

	const SERIAL serial = b->GetSerial();
	const UI08 worldNumber = b->WorldNumber();
	const SI16 x = b->GetX(), y = b->GetY();
	SI08 z = Map->MapElevation( x, y, worldNumber );
		
	const SI08 dynz = Map->DynamicElevation( x, y, z, worldNumber, 20 );
	if( ILLEGAL_Z != dynz )
		z = dynz;
	else
	{
		const SI08 staticz = Map->StaticTop( x, y, z, worldNumber, 20 );
		if( ILLEGAL_Z != staticz )
			z = staticz;
	}
	b->SetZ( z );// Z in water
	b->SetName( Dictionary->GetEntry( 1408 ) );//Name is something other than "%s's house"
	b->SetTempVar( CITV_MOREZ, calcserial( id2, id2+3, b->GetTempVar( CITV_MOREZ, 3 ), b->GetTempVar( CITV_MOREZ, 4 ) ) );

	CChar *mChar = s->CurrcharObj();

	CItem *tiller = Items->CreateItem( NULL, mChar, 0x3E4E, 1, 0, OT_ITEM );
	if( tiller == NULL )
		return false;
	tiller->SetName( Dictionary->GetEntry( 1409 ) );
	tiller->SetType( IT_TILLER );
	tiller->SetTempVar( CITV_MOREX, boattype );
	tiller->SetDecayable( false );

	CItem *p2 = Items->CreateItem( NULL, mChar, 0x3EB2, 1, 0, OT_ITEM );//Plank2 is on the RIGHT side of the boat
	if( p2 == NULL )
		return false;
	p2->SetType( IT_PLANK );
	p2->SetDecayable( false );

	CItem *p1 = Items->CreateItem( NULL, mChar, 0x3EB1, 1, 0, OT_ITEM );//Plank1 is on the LEFT side of the boat
	if( p1 == NULL )
		return false;
	p1->SetType( IT_PLANK );//Boat type
	p1->SetDecayable( false );

	CItem *hold = Items->CreateItem( NULL, mChar, 0x3EAE, 1, 0, OT_ITEM );
	if( hold == NULL )
		return false;
	hold->SetType( IT_CONTAINER );//Conatiner
	hold->SetDecayable( false );

	p2->SetTempVar( CITV_MORE, serial );
	p1->SetTempVar( CITV_MORE, serial );

	b->SetTiller( tiller->GetSerial() );
	b->SetPlank( 0, p1->GetSerial() );// Store the other stuff anywhere it will fit :-)
	b->SetPlank( 1, p2->GetSerial() );
	b->SetHold( hold->GetSerial() );

	switch( id2 ) //Give everything the right Z for it size boat
	{
		case 0x00:
		case 0x04:
			tiller->SetLocation( x + 1, y + 4, z );
			p1->SetLocation( x - 2, y, z );
			p2->SetLocation( x + 2, y, z );
			hold->SetLocation( x, y - 4, z );
			break;
		case 0x08:
		case 0x0C:
			tiller->SetLocation( x + 1, y + 5, z );
			p1->SetLocation( x - 2, y, z );
			p2->SetLocation( x + 2, y, z );
			hold->SetLocation( x, y - 4, z );
			break;
		case 0x10:
		case 0x14:
			tiller->SetLocation( x + 1, y + 5, z );
			p1->SetLocation( x - 2, y - 1, z );
			p2->SetLocation( x + 2, y - 1, z );
			hold->SetLocation( x, y - 5, z );
			break;
	}
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
//|	Function	-	void MoveBoat( CSocket *s, UI08 dir, CItem *boat )
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

	CPPauseResume prSend( 0 );
	SOCKLIST nearbyChars = FindNearbyPlayers( boat, DIST_BUILDRANGE );
	SOCKLIST_CITERATOR cIter;
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
			tiller->TextMessage( (*cIter), 8 );
		}
		return;
	}
	if( BlockBoat( boat, tx, ty, dir, boat->GetDir(), false ) )
	{
		boat->SetMoveType( 0 );
		for( cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter  )
		{
			(*cIter)->Send( &prSend );
			tiller->TextMessage( (*cIter), 9 );
		}
		return;
	}

	//Move all the special items
	boat->IncLocation( tx, ty );
	tiller->IncLocation( tx, ty );
	p1->IncLocation( tx, ty );
	p2->IncLocation( tx, ty );
	hold->IncLocation( tx, ty );

	CDataList< CItem * > *itemList = boat->GetItemsInMultiList();
	for( CItem *bItem = itemList->First(); !itemList->Finished(); bItem = itemList->Next() )
	{
		if( !ValidateObject( bItem ) )
			continue;
		if( bItem == tiller || bItem == p1 || bItem == p2 || bItem == hold )
			continue;
		bItem->IncLocation( tx, ty );
	}

	CDataList< CChar * > *charList = boat->GetCharsInMultiList();
	for( CChar *bChar = charList->First(); !charList->Finished(); bChar = charList->Next() )
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
void TurnStuff( CBoatObj *b, CBaseObject *i, bool rightTurn )
{
	if( !ValidateObject( b ) )
		return;

	SI16 dx = static_cast<SI16>(i->GetX() - b->GetX());	//get their distance x
	SI16 dy = static_cast<SI16>(i->GetY() - b->GetY());//and distance Y

	if( rightTurn )
		i->SetLocation( static_cast<SI16>(b->GetX() - dy), static_cast<SI16>(b->GetY() + dx), i->GetZ() );
	else
		i->SetLocation( static_cast<SI16>(b->GetX() + dy), static_cast<SI16>(b->GetY() - dx), i->GetZ() );
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

	CPPauseResume prSend( 0 );
	SOCKLIST nearbyChars = FindNearbyPlayers( b, DIST_BUILDRANGE );
	SOCKLIST_CITERATOR cIter;
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

	if( id2 < b->GetTempVar( CITV_MOREZ, 1 ) )
		id2 += 4;//make sure we don't have any id errors either
	if( id2 > b->GetTempVar( CITV_MOREZ, 2 ) )
		id2 -= 4;//Now you know what the min/max id is for :-)

	prSend.Mode( 0 );
	if( BlockBoat( b, 0, 0, b->GetDir(), olddir, true ) )
	{
		b->SetDir( olddir );
		for( cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter  )
		{
			(*cIter)->Send( &prSend );
			tiller->TextMessage( (*cIter), 1410, 0, 0x0481 );
		}
		return;
	}

	b->SetID( static_cast<UI08>(id2), 2 );//set the id

	if( b->GetID( 2 ) == b->GetTempVar( CITV_MOREZ, 1 ) )
		b->SetDir( NORTH );//extra DIR error checking
	if( b->GetID( 2 ) == b->GetTempVar( CITV_MOREZ, 2 ) )
		b->SetDir( WEST );


	CDataList< CItem * > *itemList = b->GetItemsInMultiList();
	for( CItem *bItem = itemList->First(); !itemList->Finished(); bItem = itemList->Next() )
	{
		if( !ValidateObject( bItem ) )
			continue;
		TurnStuff( b, bItem, rightTurn );
	}
	CDataList< CChar * > *charList = b->GetCharsInMultiList();
	for( CChar *bChar = charList->First(); !charList->Finished(); bChar = charList->Next() )
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

	switch( b->GetTempVar( CITV_MOREZ, 1 ) )//Now set what size boat it is and move the specail items
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
		default: Console.Error( "TurnBoat() more1 error! more1 = %c not found!", b->GetTempVar( CITV_MOREZ, 1 ) );
	}

	for( cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter  )
		(*cIter)->Send( &prSend );
}

void TurnBoat( CSocket *mSock, CBoatObj *myBoat, CItem *tiller, UI08 dir, bool rightTurn )
{
	SI16 tx = 0, ty = 0;
	CheckDirection( dir&0x0F, tx, ty );

	if( !BlockBoat( myBoat, tx, ty, dir, myBoat->GetDir(), true ) )
	{
		tiller->TextMessage( mSock, 10 );
		TurnBoat( myBoat, rightTurn );
	}
	else
	{
		myBoat->SetMoveType( 0 );
		tiller->TextMessage( mSock, 9 ); 
	}
}
//o---------------------------------------------------------------------------o
//|	Function	-	void Speech( CSocket *mSock, CChar *mChar )
//|	Programmer	-	Zippy
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check for boat commands
//o---------------------------------------------------------------------------o
void CBoatResponse::Handle( CSocket *mSock, CChar *mChar )
{
	if( mSock == NULL )
		return;

	CBoatObj *boat = GetBoat( mSock );
	if( !ValidateObject( boat ) )
		return;
	
	UI08 dir = boat->GetDir()&0x0F;

	CItem *tiller = calcItemObjFromSer( boat->GetTiller() );

	UnicodeTypes mLang = mSock->Language();
	switch( trigWord )
	{
	case TW_BOATTURNRIGHT:
	case TW_BOATSTARBOARD:
		if( dir >= 2 )
			dir -= 2;
		else
			dir	+= 6;
		TurnBoat( mSock, boat, tiller, dir, true );
		break;
	case TW_BOATTURNLEFT:
	case TW_BOATPORT:
		dir += 2;
		if( dir > 7 )
			dir -= 8;
		TurnBoat( mSock, boat, tiller, dir, false );
		break;
	case TW_BOATTURNAROUND:
		tiller->TextMessage( mSock, 10 );
		TurnBoat( boat, true );
		TurnBoat( boat, true );
		break;
	case TW_BOATLEFT:
		if( mChar->GetTimer( tCHAR_ANTISPAM ) > cwmWorldState->GetUICurrentTime() )
			break;
		else
			mChar->SetTimer( tCHAR_ANTISPAM, BuildTimeValue( (R32)cwmWorldState->ServerData()->CheckBoatSpeed() * 1.5 ) );

		if( dir >= 2 )
			dir -= 2;
		else
			dir	+= 6;
		tiller->TextMessage( mSock, 10 );
		MoveBoat( dir, boat );
		break;
	case TW_BOATRIGHT:
		if( mChar->GetTimer( tCHAR_ANTISPAM ) > cwmWorldState->GetUICurrentTime() )
			break;
		else
			mChar->SetTimer( tCHAR_ANTISPAM, BuildTimeValue( (R32)cwmWorldState->ServerData()->CheckBoatSpeed() * 1.5 ) );

		dir += 2;
		if( dir > 7 )
			dir -= 8;
		tiller->TextMessage( mSock, 10 );
		MoveBoat( dir, boat );
		break;
	case TW_SETNAME:
		char msg[512];
		strcpy( msg, UString( ourText ).upper().c_str() );
		char *cmd; 
		cmd = strstr( msg, Dictionary->GetEntry( 1425, mLang ).c_str() ); // note: also checking for space
		if( !cmd )
		{
			tiller->TextMessage( mSock, 11 );
			return;
		}
		cmd += 9;
		while( *cmd && *cmd == ' ' )
			++cmd; // remove any extra spaces
		if( !(*cmd) )
		{
			tiller->TextMessage( mSock, 12 );
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
//|	Function	-	void ModelBoat( CSocket *s, CItem *i )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Turn a boat into a boat model that can be re-placed
//o---------------------------------------------------------------------------o
void ModelBoat( CSocket *s, CBoatObj *i )
{
	CItem *tiller	= calcItemObjFromSer( i->GetTiller() );
	CItem *p1		= calcItemObjFromSer( i->GetPlank( 0 ) );
	CItem *p2		= calcItemObjFromSer( i->GetPlank( 1 ) );
	CItem *hold		= calcItemObjFromSer( i->GetHold() );

	CChar *mChar = s->CurrcharObj();

	SERIAL serial = i->GetSerial();
	if( i->GetOwnerObj() == mChar )
	{
		if( mChar->GetMultiObj() == i && getDist( mChar, p1 ) <= getDist( mChar, p2 ))
			LeaveBoat( s, p1 );
		else if( mChar->GetMultiObj() == i && getDist( mChar, p2 ) <= getDist( mChar, p1 ))
			LeaveBoat( s, p2 );

		if( i->GetItemsInMultiList()->Num() > 4 || i->GetCharsInMultiList()->Num() > 0 )
		{
			s->sysmessage( "This boat must be empty before it can be converted to a model!" );
			return;
		}

		CItem *model = Items->CreateItem( s, mChar, 0x14f3, 1, 0, OT_ITEM, true );
		if( model == NULL )
		{
			Console.Error( " Turning boat into model failed on model creation, attempted by character serial %X", mChar->GetSerial() );
			return;
		}

		model->SetTempVar( CITV_MOREX, tiller->GetTempVar( CITV_MOREX ) );
		Weight->subtractItemWeight( mChar, model );
		model->SetWeight( 0 );
		Weight->addItemWeight( mChar, model );
		model->SetType( IT_MODELMULTI );
		model->SetMovable( 0 );

		tiller->Delete();
		p1->Delete();
		p2->Delete();
		hold->Delete();
		i->Delete();
		killKeys( serial );
	}
}

}
