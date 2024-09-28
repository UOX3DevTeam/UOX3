#include "uox3.h"
#include "weight.h"
#include "CPacketSend.h"
#include "classes.h"
#include "regions.h"
#include "CResponse.h"
#include "cEffects.h"
#include "Dictionary.h"
#include "StringUtility.hpp"
#include "CJSMapping.h"
#include "cScript.h"
#include "CJSEngine.h"


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
const UI08 cShipItems[4][6] =
{
	{0xB1, 0xD5, 0xB2, 0xD4, 0xAE, 0x4E},
	{0x8A, 0x89, 0x85, 0x84, 0x65, 0x53},
	{0xB2, 0xD4, 0xB1, 0xD5, 0xB9, 0x4B},
	{0x85, 0x84, 0x8A, 0x89, 0x93, 0x50}
};
//============================================================================================

//o------------------------------------------------------------------------------------------------o
//|	Function	-	GetBoat()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get the boat a character is on
//o------------------------------------------------------------------------------------------------o
CBoatObj * GetBoat( CSocket *s )
{
	CChar *mChar = s->CurrcharObj();

	if( ValidateObject( mChar->GetMultiObj() ))
		return static_cast<CBoatObj *>( mChar->GetMultiObj() );

	return static_cast<CBoatObj *>( FindMulti( mChar ));
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	LeaveBoat()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Teleport player and their followers off the boat and to a nearby valid location
//o------------------------------------------------------------------------------------------------o
auto LeaveBoat( CSocket *s, CItem *p ) -> bool
{
	CBoatObj *boat = GetBoat( s );
	if( !ValidateObject( boat ))
		return false;

	const SI16 x2 = p->GetX();
	const SI16 y2 = p->GetY();
	CChar *mChar = s->CurrcharObj();
	UI08 worldNumber = mChar->WorldNumber();
	UI16 instanceId = mChar->GetInstanceId();
	for( SI16 x = x2 - 3; x <= x2 + 4; ++x )
	{
		for( SI16 y = y2 - 3; y <= y2 + 4; ++y )
		{
			SI08 z = Map->Height( x, y, mChar->GetZ(), worldNumber, instanceId );
			if( Map->ValidSpawnLocation( x, y, z, worldNumber, instanceId, true ) && !FindMulti( x, y, z, worldNumber, instanceId ))
			{
				mChar->SetLocation( x, y, z, worldNumber, instanceId );
				
				// Freeze player temporarily after teleporting
				Effects->TempEffect( nullptr, mChar, 1, 1, 1, 5 ); // 1 second, divided by 5 for 0.2s duration freeze

				auto myFollowers = mChar->GetFollowerList();
				for( const auto &follower : myFollowers->collection() )
				{
					if( ValidateObject( follower ))
					{
						// Only teleport followers with player if they're set to follow player, and within range
						if( !follower->GetMounted() && follower->GetNpcWander() == WT_FOLLOW && ObjInRange( mChar, follower, DIST_SAMESCREEN ))
						{
							follower->SetLocation( x, y, z, worldNumber, instanceId );
						}
					}
				}
				s->SysMessage( 3 ); // You left the boat.
				return true;
			}
		}
	}
	s->SysMessage( 4 ); // You cannot get off here!
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	PlankStuff()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	If not on a boat, will send character to the plank, other
//|					wise will call OpenPlank to open/close the plank
//o------------------------------------------------------------------------------------------------o
void PlankStuff( CSocket *s, CItem *p )
{
	CChar *mChar	= s->CurrcharObj();
	CBoatObj *boat	= GetBoat( s );
	if( !ValidateObject( boat ))
	{
		mChar->SetLocation( p->GetX(), p->GetY(), p->GetZ() + 3 );

		// Freeze player temporarily after teleporting
		Effects->TempEffect( nullptr, mChar, 1, 1, 1, 5 ); // 1 second, divided by 5 for 0.2s duration freeze

		CMultiObj *boat2 = p->GetMultiObj();
		if( ValidateObject( boat2 ))
		{
			auto myFollowers = mChar->GetFollowerList();
			for( const auto &follower : myFollowers->collection() )
			{
				if( ValidateObject( follower ))
				{
					// Only teleport followers with player if they're set to follow and within range
					if( !follower->GetMounted() && follower->GetNpcWander() == WT_FOLLOW && ObjInRange( mChar, follower, DIST_SAMESCREEN ))
					{
						follower->SetLocation( mChar );
					}
				}
			}
		}

		if( ValidateObject( boat2 ))
		{
			s->SysMessage( 1 ); // You board the boat.
		}
		else
		{
			s->SysMessage( 2 ); // There was no boat found there.
		}
	}
	else
	{
		LeaveBoat( s, p );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	OpenPlank()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Open / Close the plank on a boat
//o------------------------------------------------------------------------------------------------o
void OpenPlank( CItem *p )
{
	switch( p->GetId( 2 ))
	{
			//Open plank->
		case 0xE9: p->SetId( 0x84, 2 ); break;
		case 0xB1: p->SetId( 0xD5, 2 ); break;
		case 0xB2: p->SetId( 0xD4, 2 ); break;
		case 0x8A: p->SetId( 0x89, 2 ); break;
		case 0x85: p->SetId( 0x84, 2 ); break;
			//Close Plank->
		case 0x84: p->SetId( 0xE9, 2 ); break;
		case 0xD5: p->SetId( 0xB1, 2 ); break;
		case 0xD4: p->SetId( 0xB2, 2 ); break;
		case 0x89: p->SetId( 0x8A, 2 ); break;
		default: 	Console.Warning( oldstrutil::format( "Invalid plank ID called! Plank 0x%X '%s' [%u]", p->GetSerial(), p->GetName().c_str(), p->GetId() ));
			break;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	BlockBoat()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if a boat will be blocked if trying to move/turn in a specific direction
//o------------------------------------------------------------------------------------------------o
bool BlockBoat( CBoatObj *b, SI16 xmove, SI16 ymove, UI08 moveDir, UI08 boatDir, bool turnBoat )
{
	SI16 cx = b->GetX(), cy = b->GetY();
	const SI08 cz = b->GetZ();
	SI16 x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	UI08 type = 0;

	cx += xmove;
	cy += ymove;

	switch( b->GetId( 2 ))
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
		switch( boatDir & 0x0F )
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
	switch( moveDir & 0x0F )
	{
		case NORTHEAST:	// U
		case SOUTHWEST:	// D
		case NORTH:	// N
		case SOUTH:	// S
			switch( boatDir & 0x0F )
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
			switch( boatDir & 0x0F )
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

	UI08 worldNumber = b->WorldNumber();
	UI16 instanceId = b->GetInstanceId();
	SI08 boatZ = b->GetZ();
	for( SI16 x = x1; x < x2; ++x )
	{
		for( SI16 y = y1; y < y2; ++y )
		{
			// Look for other boats
			CMultiObj * tempBoat = FindMulti( x, y, boatZ, worldNumber, instanceId );
			if( ValidateObject( tempBoat ) && tempBoat->GetSerial() != b->GetSerial() )
				return true;

			// Look for blocking dynamic items at boat's Z level
			CItem *tempItem = GetItemAtXYZ( x, y, boatZ, worldNumber, instanceId );
			if( ValidateObject( tempItem ))
			{
				auto multiSerial = tempItem->GetMulti();
				//auto boatSerial = b->GetSerial();
				if( multiSerial != INVALIDSERIAL && multiSerial != b->GetSerial() )
				{
					CTile& tile = Map->SeekTile( tempItem->GetId() );
					if( tile.CheckFlag( TF_BLOCKING ))
						return true;
				}
			}

			SI08 sz = Map->StaticTop( x, y, boatZ, worldNumber, MAX_Z_STEP );

			if( sz == ILLEGAL_Z ) //map tile
			{
				auto map = Map->SeekMap( x, y, worldNumber );
				if( map.altitude >= cz && !map.CheckFlag( TF_WET ) && map.name() != "water" )//only tiles on/above the water
					return true;
			}
			else
			{
				auto artwork = Map->ArtAt( x, y, worldNumber );
				for( auto &tile : artwork )
				{
					SI08 zt = tile.altitude + tile.height();
					if( !tile.CheckFlag( TF_WET ) && zt >= cz && zt <= ( cz + 20 ) && ( tile.name() != "water" ))
						return true;
				}
			}
		}
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CreateBoat()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Create a boat
//o------------------------------------------------------------------------------------------------o
bool CreateBoat( CSocket *s, CBoatObj *b, UI08 id2, UI08 boattype )
{
	if( !ValidateObject( b ))
	{
		if( s != nullptr )
		{
			s->SysMessage( 5 ); // There was an error creating that boat.
		}
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
		case 0x18:
			break;
		default:
			if( s != nullptr )
			{
				s->SysMessage( 6 ); // The deed is broken, please contact a Game Master
			}
			return false;
	}

	const SERIAL serial = b->GetSerial();
	const UI08 worldNumber = b->WorldNumber();
	const UI16 instanceId = b->GetInstanceId();
	SI32 maxWeight = b->GetWeightMax();
	if( maxWeight == 0 )
	{
		maxWeight = 40000; // 400 stones default, if nothing else is defined
	}
	UI16 maxItems = b->GetMaxItems();
	if( maxItems == 0 )
	{
		maxItems = 125; // Default if nothing else is defined
	}

	const SI16 x = b->GetX(), y = b->GetY();
	SI08 z = Map->MapElevation( x, y, worldNumber );

	const SI08 dynz = Map->DynamicElevation( x, y, z, worldNumber, instanceId, 20 );
	if( ILLEGAL_Z != dynz )
	{
		z = dynz;
	}
	else
	{
		const SI08 staticz = Map->StaticTop( x, y, z, worldNumber, 20 );
		if( ILLEGAL_Z != staticz )
		{
			z = staticz;
		}
	}
	b->SetZ( z ); // Z in water
	b->SetTempVar( CITV_MOREZ, CalcSerial( id2, id2+3, b->GetTempVar( CITV_MOREZ, 3 ), b->GetTempVar( CITV_MOREZ, 4 )));
	b->SetMoveType( BOAT_ANCHORED );

	CChar *mChar = nullptr;
	if( s != nullptr )
	{
		mChar = s->CurrcharObj();
	}

	CItem *tiller = Items->CreateItem( nullptr, mChar, 0x3E4E, 1, 0, OT_ITEM, false, true, worldNumber, instanceId, x, y, z );
	if( tiller == nullptr )
		return false;

	std::string dictName = s != nullptr ? Dictionary->GetEntry( 2035, s->Language() ) : Dictionary->GetEntry( 2035 );
	if( b->GetName().length() > 0 && b->GetName() != dictName ) // a ship
	{
		std::string tillerNameDict = s != nullptr ? Dictionary->GetEntry( 2033, s->Language() ) : Dictionary->GetEntry( 2033 ); // The tiller man of %s
		auto sPos = tillerNameDict.find( "%s" );
		tiller->SetName( tillerNameDict.replace( sPos, 2, b->GetName() ));
	}
	else
	{
		tiller->SetName( Dictionary->GetEntry( 1409 )); // a tiller man
	}
	tiller->SetType( IT_TILLER );
	tiller->SetTempVar( CITV_MOREX, boattype );
	tiller->SetDecayable( false );

	CItem *p2 = Items->CreateItem( nullptr, mChar, 0x3EB2, 1, 0, OT_ITEM, false, true, worldNumber, instanceId, x, y, z ); // Plank2 is on the RIGHT side of the boat
	if( p2 == nullptr )
		return false;

	p2->SetType( IT_PLANK );
	p2->SetDecayable( false );

	// Lock the plank
	TAGMAPOBJECT tagvalObject;
	tagvalObject.m_ObjectType	= TAGMAP_TYPE_INT;
	tagvalObject.m_IntValue		= 1;
	tagvalObject.m_Destroy		= false;
	tagvalObject.m_StringValue	= "";
	p2->SetTag( "plankLocked", tagvalObject );

	CItem *p1 = Items->CreateItem( nullptr, mChar, 0x3EB1, 1, 0, OT_ITEM, false, true, worldNumber, instanceId, x, y, z ); // Plank1 is on the LEFT side of the boat
	if( p1 == nullptr )
		return false;

	p1->SetType( IT_PLANK ); // Boat type
	p1->SetDecayable( false );

	// Lock the plank
	tagvalObject.m_ObjectType	= TAGMAP_TYPE_INT;
	tagvalObject.m_IntValue		= 1;
	tagvalObject.m_Destroy		= false;
	tagvalObject.m_StringValue	= "";
	p1->SetTag( "plankLocked", tagvalObject );

	CItem *hold = Items->CreateItem( nullptr, mChar, 0x3EAE, 1, 0, OT_ITEM, false, true, worldNumber, instanceId, x, y, z );
	if( hold == nullptr )
		return false;

	hold->SetType( IT_CONTAINER ); // Conatiner
	hold->SetDecayable( false );
	hold->SetWeightMax( maxWeight );
	hold->SetMaxItems( maxItems );

	p2->SetTempVar( CITV_MORE, serial );
	p1->SetTempVar( CITV_MORE, serial );

	b->SetTiller( tiller->GetSerial() );
	b->SetPlank( 0, p1->GetSerial() ); // Store the other stuff anywhere it will fit :-)
	b->SetPlank( 1, p2->GetSerial() );
	b->SetHold( hold->GetSerial() );

	switch( id2 ) // Give everything the right Z for it size boat
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

//o------------------------------------------------------------------------------------------------o
//|	Function	-	MoveBoat()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Move the boat and everything on it 1 tile in its current direction
//o------------------------------------------------------------------------------------------------o
void MoveBoat( UI08 dir, CBoatObj *boat )
{
	CItem *tiller = CalcItemObjFromSer( boat->GetTiller() );
	CItem *p1 = CalcItemObjFromSer( boat->GetPlank( 0 ));
	CItem *p2 = CalcItemObjFromSer( boat->GetPlank( 1 ));
	CItem *hold = CalcItemObjFromSer( boat->GetHold() );

	if( !ValidateObject( boat ))
		return;

	if( !ValidateObject( tiller ) || !ValidateObject( p1 ) || !ValidateObject( p2 ) || !ValidateObject( hold ))
		return;

	CPPauseResume prSend( 0 );
	auto nearbyChars = FindNearbyPlayers( boat, DIST_BUILDRANGE );
	for( auto &tSock : nearbyChars )
	{
		tSock->Send( &prSend );
	}

	SI16 tx = 0, ty = 0;
	CheckDirection( dir & 0x0F, tx, ty );

	SI16 x = boat->GetX(), y = boat->GetY();

	auto worldNumber = boat->WorldNumber();
	bool teleportBoat = false;
	if( worldNumber <= 1 && ( x + tx ) < 5100 )
	{
		// Handle wrapping map from one edge to the other when sailing
		if(( x + tx ) <= 50 && tx < 0 )
		{
			// Sailing west
			tx = 5050;
			teleportBoat = true;
		}
		else if(( x + tx ) >= 5050 && tx > 0 )
		{
			// Sailing east
			tx = -5050;
			teleportBoat = true;
		}
		else if(( y + ty ) <= 100 && ty < 0 )
		{
			// Sailing north
			ty = 3896;
			teleportBoat = true;
		}
		else if(( y + ty ) >= 3996 && ty > 0 )
		{
			// Sailing south
			ty = -3896;
			teleportBoat = true;
		}
	}

	// Check if anything blocks boat from moving to new location
	if( BlockBoat( boat, tx, ty, dir, boat->GetDir(), false ))
	{
		boat->SetMoveType( 0 );
		for( auto &tSock : nearbyChars )
		{
			tSock->Send( &prSend );
			tiller->TextMessage( tSock, 9 );
		}
		return;
	}
	
	// Move all the special items along with the boat
	boat->IncLocation( tx, ty );
	tiller->IncLocation( tx, ty );
	p1->IncLocation( tx, ty );
	p2->IncLocation( tx, ty );
	hold->IncLocation( tx, ty );

	// If boat got teleported due to wrapping of map, make sure to
	// remove the boat and everything on it from sight of nearby players
	if( teleportBoat )
	{
		boat->RemoveFromSight();
		boat->Update();
		tiller->RemoveFromSight();
		tiller->Update();
		p1->RemoveFromSight();
		p1->Update();
		p2->RemoveFromSight();
		p2->Update();
		hold->RemoveFromSight();
		hold->Update();
	}

	// Move all items aboard the boat along with the boat
	auto itemList = boat->GetItemsInMultiList();
	for( const auto &bItem : itemList->collection() )
	{
		if( ValidateObject( bItem ))
		{
			if( !(bItem == tiller || bItem == p1 || bItem == p2 || bItem == hold ))
			{
				bItem->IncLocation( tx, ty );
				
				// Remember to remove any items on board from sight of nearby players if boat got teleported
				if( teleportBoat )
				{
					bItem->RemoveFromSight();
					bItem->Update();
				}
			}
		}
	}

	// Move all characters aboard the boat along with the boat
	auto charList = boat->GetCharsInMultiList();
	for( const auto &bChar : charList->collection() )
	{
		if( ValidateObject( bChar ))
		{
			bChar->SetLocation( bChar->GetX() + tx, bChar->GetY() + ty, bChar->GetZ() );
			if( teleportBoat )
			{
				bChar->Update();
			}
		}
	}
	for( auto &tSock: nearbyChars )
	{
		tSock->Send( &prSend );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	TurnStuff()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Turn an item on a boat to match the boat's turning
//o------------------------------------------------------------------------------------------------o
void TurnStuff( CBoatObj *b, CBaseObject *i, bool rightTurn )
{
	if( !ValidateObject( b ))
		return;

	SI16 dx = static_cast<SI16>( i->GetX() - b->GetX() ); // get their distance x
	SI16 dy = static_cast<SI16>( i->GetY() - b->GetY() ); // and distance Y

	if( rightTurn )
	{
		i->SetLocation( static_cast<SI16>( b->GetX() - dy ), static_cast<SI16>( b->GetY() + dx ), i->GetZ() );
	}
	else
	{
		i->SetLocation( static_cast<SI16>( b->GetX() + dy ), static_cast<SI16>( b->GetY() - dx ), i->GetZ() );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	TurnBoat()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Turn the boat and use TurnStuff() to turn all items/chars on it
//o------------------------------------------------------------------------------------------------o
void TurnBoat( CBoatObj *b, bool rightTurn, bool disableChecks )
{
	if( !ValidateObject( b ))
		return;

	SI16 id2	= b->GetId( 2 );
	UI08 olddir = b->GetDir();

	CPPauseResume prSend( 0 );
	auto nearbyChars = FindNearbyPlayers( b, DIST_BUILDRANGE );
	for( auto &tSock :nearbyChars )
	{
		tSock->Send( &prSend );
	}

	CItem *tiller = CalcItemObjFromSer( b->GetTiller() );
	CItem *p1 = CalcItemObjFromSer( b->GetPlank( 0 ));
	CItem *p2 = CalcItemObjFromSer( b->GetPlank( 1 ));
	CItem *hold = CalcItemObjFromSer( b->GetHold() );

	if( !ValidateObject( tiller ) || !ValidateObject( p1 ) || !ValidateObject( p2 ) || !ValidateObject( hold ))
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
	{
		b->SetDir( b->GetDir() + 4 );
	}
	if( b->GetDir() > NORTHWEST )
	{
		b->SetDir( b->GetDir() - 4 );//Make sure we dont have any DIR errors
	}

	if( id2 < b->GetTempVar( CITV_MOREZ, 1 ))
	{
		id2 += 4;//make sure we don't have any id errors either
	}
	if( id2 > b->GetTempVar( CITV_MOREZ, 2 ))
	{
		id2 -= 4;//Now you know what the min/max id is for :-)
	}

	prSend.Mode( 0 );
	if( !disableChecks )
	{
		if( BlockBoat( b, 0, 0, b->GetDir(), olddir, true ))
		{
			b->SetDir( olddir );
			for( auto &tSock :nearbyChars )
			{
				tSock->Send( &prSend );
				tiller->TextMessage( tSock, 1410, 0, 0x0481 );
			}
			return;
		}
	}

	b->SetId( static_cast<UI08>( id2 ), 2 );//set the id

	if( b->GetId( 2 ) == b->GetTempVar( CITV_MOREZ, 1 ))
	{
		b->SetDir( NORTH );//extra DIR error checking
	}
	if( b->GetId( 2 ) == b->GetTempVar( CITV_MOREZ, 2 ))
	{
		b->SetDir( WEST );
	}


	auto itemList = b->GetItemsInMultiList();
	for( const auto &bItem : itemList->collection() )
	{
		if( ValidateObject( bItem ))
		{
			TurnStuff( b, bItem, rightTurn );
		}
	}
	auto charList = b->GetCharsInMultiList();
	for( const auto &bChar : charList->collection() )
	{
		if( ValidateObject( bChar ))
		{
			TurnStuff( b, bChar, rightTurn );
		}
	}

	UI08 dir = ( b->GetDir() & 0x0F ) / 2;

	p1->SetLocation( b );
	p1->SetId( cShipItems[dir][PORT_P_C], 2 );//change the ID

	p2->SetLocation( b );
	p2->SetId( cShipItems[dir][STAR_P_C], 2 );

	tiller->SetLocation( b );
	tiller->SetId( cShipItems[dir][TILLERID], 2 );

	hold->SetLocation( b );
	hold->SetId( cShipItems[dir][HOLDID], 2 );

	switch( b->GetTempVar( CITV_MOREZ, 1 )) // Now set what size boat it is and move the specail items
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
		default: Console.Error( oldstrutil::format( "TurnBoat() more1 error! more1 = %c not found!", b->GetTempVar( CITV_MOREZ, 1 )));
	}

	for( auto &tSock :nearbyChars )
	{
		tSock->Send( &prSend );
	}

	auto scriptTriggers = b->GetScriptTriggers();
    for( auto scriptTrig : scriptTriggers )
    {
        auto toExecute = JSMapping->GetScript( scriptTrig );
        if( toExecute )
        {
            if( toExecute->OnBoatTurn( b, olddir, b->GetDir() ) == 1 )
            {
                // A script with the event returned true; prevent other scripts from running
                break;
            }
        }
    }
}

void TurnBoat( CSocket *mSock, CBoatObj *myBoat, CItem *tiller, UI08 dir, bool rightTurn )
{
	SI16 tx = 0, ty = 0;
	CheckDirection( dir & 0x0F, tx, ty );

	if( !BlockBoat( myBoat, tx, ty, dir, myBoat->GetDir(), true ))
	{
		tiller->TextMessage( mSock, 10 );
		TurnBoat( myBoat, rightTurn, false );
	}
	else
	{
		myBoat->SetMoveType( 0 );
		tiller->TextMessage( mSock, 9 );
	}
}
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBoatResponse::Handle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check for spoken boat commands and handle the response
//o------------------------------------------------------------------------------------------------o
void CBoatResponse::Handle( CSocket *mSock, CChar *mChar )
{
	if( mSock == nullptr )
		return;

	CBoatObj *boat = GetBoat( mSock );
	if( !ValidateObject( boat ))
		return;

	UI08 dir = boat->GetDir() & 0x0F;

	CItem *tiller = CalcItemObjFromSer( boat->GetTiller() );

	UnicodeTypes mLang = mSock->Language();
	if( trigWord != TW_SETNAME && trigWord != TW_BOATANCHORRAISE && trigWord != TW_BOATANCHORDROP && boat->GetMoveType() == -1 )
	{
		tiller->TextMessage( mSock, 2024 ); // Ar, the anchor is down sir!
		mSock->SysMessage( 2023 ); // You must raise the anchor to pilot the ship.
		return;
	}
	switch( trigWord )
	{
		case TW_BOATANCHORDROP:
			if( boat->GetMoveType() != BOAT_ANCHORED )
			{
				boat->SetMoveType( BOAT_ANCHORED );
				tiller->TextMessage( mSock, 2025 ); // Ar, anchor dropped sir.
			}
			else
			{
				tiller->TextMessage( mSock, 2026 ); // Ar, the anchor was already dropped sir.
			}
			break;
		case TW_BOATANCHORRAISE:
			if( boat->GetMoveType() == BOAT_ANCHORED )
			{
				boat->SetMoveType( BOAT_STOP );
				tiller->TextMessage( mSock, 2027 ); // Ar, anchor raised sir.
			}
			else
			{
				tiller->TextMessage( mSock, 2028 ); // Ar, the anchor has not been dropped sir.
			}
			break;
		case TW_BOATTURNRIGHT:
		case TW_BOATSTARBOARD:
			if( dir >= 2 )
			{
				dir -= 2;
			}
			else
			{
				dir	+= 6;
			}
			TurnBoat( mSock, boat, tiller, dir, true );
			break;
		case TW_BOATTURNLEFT:
		case TW_BOATPORT:
			dir += 2;
			if( dir > 7 )
			{
				dir -= 8;
			}
			TurnBoat( mSock, boat, tiller, dir, false );
			break;
		case TW_BOATTURNAROUND:
			tiller->TextMessage( mSock, 10 ); // Aye, sir.
			TurnBoat( boat, true, true );
			TurnBoat( boat, true, true );
			break;
		case TW_SETNAME:
		{
			// Check if player trying to rename the boat is actually the owner
			if( mChar->GetSerial() != boat->GetOwner() )
			{
				tiller->TextMessage( mSock, 2034 ); // Arr! Only the owner of the ship may change its name!
				return;
			}

			// Check if player provided anything after the actual set name command
			std::string cmdString = oldstrutil::upper( Dictionary->GetEntry( 1425, mLang )); // SET NAME
			if( oldstrutil::upper( ourText ).size() == cmdString.size() )
			{
				tiller->TextMessage( mSock, 12 ); // Can ya say that again with an actual name, sir?
				return;
			}

			// Check if we can find the dictionary-based command string in the player's speech
			std::string upperOurText = oldstrutil::upper( ourText );
			size_t cmdStringPos = upperOurText.find( cmdString );
			if( cmdStringPos == std::string::npos )
			{
				// Command string not found in text, something went wrong!
				tiller->TextMessage( mSock, 11 ); // What be that, sir?
				return;
			}

			// Erase cmdString from ourText, leaving us with just the new name of the boat
			size_t pos = upperOurText.find( cmdString );
			if( pos != std::string::npos )
			{
				ourText.erase( pos, cmdString.size() );
			}

			// Trim spaces from start and end of player provided name, and see if there's actually a name there and not just empty spaces!
			ourText = oldstrutil::trim( ourText );
			if( ourText.size() == 0 )
			{
				boat->SetName( Dictionary->GetEntry( 2035, mLang )); // a ship
				tiller->SetName( Dictionary->GetEntry( 1409, mLang )); // a tiller man
				tiller->TextMessage( mSock, 2030 ); // This ship now has no name.
				return;
			}
		
			std::string tillerNameDict = Dictionary->GetEntry( 2033, mLang ); // The tiller man of %s
			if( tillerNameDict.size() + ourText.size() > MAX_NAME - 1 )
			{
				mSock->SysMessage( 1944 ); // That name is too long.
				return;
			}

			// Give boat the new name
			boat->SetName( ourText );

			// Give tiller man a fitting name as well
			auto sPos = tillerNameDict.find("%s");
			tiller->SetName( tillerNameDict.replace( sPos, 2, ourText ));
			break;
		}
		default:
			break;
	}
}

void KillKeys( SERIAL targSerial, SERIAL charSerial = INVALIDSERIAL );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	ModelBoat()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Turn a boat into a boat model that can be re-placed
//o------------------------------------------------------------------------------------------------o
void ModelBoat( CSocket *s, CBoatObj *i )
{
	if( s == nullptr )
		return;

	CItem *tiller	= CalcItemObjFromSer( i->GetTiller() );
	CItem *p1		= CalcItemObjFromSer( i->GetPlank( 0 ));
	CItem *p2		= CalcItemObjFromSer( i->GetPlank( 1 ));
	CItem *hold		= CalcItemObjFromSer( i->GetHold() );

	CChar *mChar = s->CurrcharObj();

	if( !ValidateObject( tiller ) || !ValidateObject( p1 ) || !ValidateObject( p2 ) || !ValidateObject( hold ))
	{
		s->SysMessage( 9014 ); // Something is not right - unable to find tiller, planks or hold! This boat might be bugged...
		return;
	}

	SERIAL serial = i->GetSerial();
	if( i->GetOwnerObj() == mChar )
	{
		// Check if character's backpack can hold more items
		// By default, CreateItem drops item at character's feet if it cannot hold more items, but
		// this is a high-value item, so let's prevent the action instead of dropping the item on the ground!
		CItem *playerPack = mChar->GetPackItem();
		if( ValidateObject( playerPack ))
		{
			if( playerPack->GetContainsList()->Num() >= playerPack->GetMaxItems() )
			{
				s->SysMessage( 1819 ); // Your backpack cannot hold any more items!
				return;
			}
		}

		if( p1->GetId( 2 ) == 0x84 || p1->GetId( 2 ) == 0xD5 || p1->GetId( 2 ) == 0xD4 || p1->GetId( 2 ) == 0x89
			|| p2->GetId( 2 ) == 0x84 || p2->GetId( 2 ) == 0xD5 || p2->GetId( 2 ) == 0xD4 || p2->GetId( 2 ) == 0x89 )
		{
			s->SysMessage( 9015 ); // Planks must be closed before you pack up your ship!
			return;
		}

		if( hold->GetContainsList()->Num() > 0 )
		{
			s->SysMessage( 9016 ); // Make sure your hold is empty, and try again!
			return;
		}

		if( i->GetItemsInMultiList()->Num() > 4 || i->GetCharsInMultiList()->Num() > 0 )
		{
			s->SysMessage( 9017 ); // This boat must be empty before it can be converted to a model!
			return;
		}

		CItem *model = Items->CreateItem( s, mChar, 0x14f3, 1, 0, OT_ITEM, true );
		if( model == nullptr )
			return;

		model->SetTempVar( CITV_MOREX, tiller->GetTempVar( CITV_MOREX ));
		Weight->SubtractItemWeight( mChar, model );
		model->SetWeight( 0 );
		Weight->AddItemWeight( mChar, model );
		model->SetType( IT_MODELMULTI );
		model->SetMovable( 0 );

		if( i->GetName().length() > 0 )
		{
			// Apply boat's name to the ship model
			std::string shipModelNameDict = Dictionary->GetEntry( 2037, s->Language() ); // %s [Dry Docked]
			auto sPos = shipModelNameDict.find( "%s" );
			model->SetName( shipModelNameDict.replace( sPos, 2, i->GetName() ));

			// Also store the original name in the item's title
			model->SetTitle( i->GetName() );
		}

		tiller->Delete();
		p1->Delete();
		p2->Delete();
		hold->Delete();
		i->Delete();
		KillKeys( serial );
	}
}


