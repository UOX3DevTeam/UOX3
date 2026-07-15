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

auto FindNearbyChars( SI16 x, SI16 y, UI08 worldNumber, UI16 instanceId, UI16 distance ) -> std::vector<CChar *>;
auto FindNearbyItems( SI16 x, SI16 y, UI08 worldNumber, UI16 instanceId, UI16 distance ) -> std::vector<CItem *>;


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
		case 24: case 25: case 26: case 27: // Orcish galleon, pristine
		case 28: case 29: case 30: case 31: // Orcish galleon, damaged
		case 32: case 33: case 34: case 35: // Orcish galleon, severely damaged
		case 36: case 37: case 38: case 39: // Gargish galleon, pristine
		case 40: case 41: case 42: case 43: // Gargish galleon, damaged
		case 44: case 45: case 46: case 47: // Gargish galleon, severely damaged
		case 48: case 49: case 50: case 51: // Tokuno galleon, pristine
		case 52: case 53: case 54: case 55: // Tokuno galleon, damaged
		case 56: case 57: case 58: case 59: // Tokuno galleon, severely damaged
		case 64: case 65: case 66: case 67: // Britannian ship, pristine
		case 68: case 69: case 70: case 71: // Britannian ship, damaged
		case 72: case 73: case 74: case 75: // Britannian ship, severely damaged
			type = 3;
			break;
		case 60: case 61: case 62: case 63: // Rowboat
			type = 1;
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

	const UI08 baseId = b->GetTempVar( CITV_MOREZ, 1 );
	const bool highSeasHull = baseId == 0x18 || baseId == 0x24 || baseId == 0x30 || baseId == 0x40;
	if( highSeasHull && !turnBoat )
	{
		// ServUO checks the destination footprint of the actual directional
		// multi. The legacy UOX3 box is only large enough for classic boats.
		const auto &components = Map->SeekMulti( b->GetId() - 0x4000 ).items;
		SI16 minX = 0, maxX = 0, minY = 0, maxY = 0;
		for( const auto &component : components )
		{
			minX = std::min<SI16>( minX, static_cast<SI16>( component.offsetX ));
			maxX = std::max<SI16>( maxX, static_cast<SI16>( component.offsetX ));
			minY = std::min<SI16>( minY, static_cast<SI16>( component.offsetY ));
			maxY = std::max<SI16>( maxY, static_cast<SI16>( component.offsetY ));
		}
		x1 = cx + minX;
		x2 = cx + maxX + 1;
		y1 = cy + minY;
		y2 = cy + maxY + 1;
	}

	UI08 worldNumber = b->WorldNumber();
	UI16 instanceId = b->GetInstanceId();
	SI08 boatZ = b->GetZ();
	const UI16 dynamicRange = static_cast<UI16>( std::max( x2 - x1, y2 - y1 ) + 2 );
	auto nearbyDynamicItems = FindNearbyItems( cx, cy, worldNumber, instanceId, dynamicRange );
	auto movingAwayFrom = [b, cx, cy]( const CMultiObj *other )
	{
		if( !ValidateObject( other ))
			return false;
		const SI32 oldDx = static_cast<SI32>( b->GetX() ) - other->GetX();
		const SI32 oldDy = static_cast<SI32>( b->GetY() ) - other->GetY();
		const SI32 newDx = static_cast<SI32>( cx ) - other->GetX();
		const SI32 newDy = static_cast<SI32>( cy ) - other->GetY();
		return newDx * newDx + newDy * newDy > oldDx * oldDx + oldDy * oldDy;
	};
	for( SI16 x = x1; x < x2; ++x )
	{
		for( SI16 y = y1; y < y2; ++y )
		{
			// Look for other boats
			CMultiObj * tempBoat = FindMulti( x, y, boatZ, worldNumber, instanceId );
			if( ValidateObject( tempBoat ) && tempBoat->GetSerial() != b->GetSerial() && !movingAwayFrom( tempBoat ))
				return true;

			// Look for blocking dynamic items at boat's Z level
			CItem *tempItem = GetItemAtXYZ( x, y, boatZ, worldNumber, instanceId );
			if( ValidateObject( tempItem ))
			{
				auto multiSerial = tempItem->GetMulti();
				//auto boatSerial = b->GetSerial();
				if( multiSerial != INVALIDSERIAL && multiSerial != b->GetSerial() )
				{
					auto *otherMulti = tempItem->GetMultiObj();
					if( ValidateObject( otherMulti ) && movingAwayFrom( otherMulti ))
						continue;
					CTile& tile = Map->SeekTile( tempItem->GetId() );
					if( tile.CheckFlag( TF_BLOCKING ))
						return true;
				}
			}

			// Docks and addon components are often dynamic items above the
			// waterline, so an exact-Z lookup at the boat's Z misses them.
			for( auto *dynamicItem : nearbyDynamicItems )
			{
				if( !ValidateObject( dynamicItem ) || dynamicItem == b || dynamicItem->GetX() != x || dynamicItem->GetY() != y ||
					dynamicItem->GetMulti() == b->GetSerial() )
					continue;
				auto *otherMulti = dynamicItem->GetMultiObj();
				if( ValidateObject( otherMulti ) && movingAwayFrom( otherMulti ))
					continue;
				CTile& dynamicTile = Map->SeekTile( dynamicItem->GetId() );
				const SI16 dynamicTop = dynamicItem->GetZ() + std::max<SI16>( 1, dynamicTile.Height() );
				if( !dynamicTile.CheckFlag( TF_WET ) && dynamicTop >= cz && dynamicItem->GetZ() <= cz + 20 )
					return true;
			}

			SI08 sz = Map->StaticTop( x, y, boatZ, worldNumber, MAX_Z_STEP );

			if( sz == ILLEGAL_Z ) //map tile
			{
				auto map = Map->SeekMap( x, y, worldNumber );
				if( map.terrainInfo == nullptr || ( map.altitude >= cz && !map.CheckFlag( TF_WET ) && map.name() != "water" ))//only tiles on/above the water
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

// Resolve High Seas interactive fixtures from the placeholder components in
// the active client multi instead of using classic-boat offsets.
static bool ConfigureHighSeasFixtures( CBoatObj *boat, CItem *tiller, CItem *portPlank, CItem *starboardPlank, CItem *hold, bool fromConstruct )
{
	if( !ValidateObject( boat ) || !ValidateObject( tiller ) || !ValidateObject( portPlank ) ||
		!ValidateObject( starboardPlank ) || !ValidateObject( hold ))
		return false;

	const UI08 baseId = boat->GetTempVar( CITV_MOREZ, 1 );
	if( baseId != 0x18 && baseId != 0x24 && baseId != 0x30 && baseId != 0x40 )
		return false;

	static const std::unordered_set<UI16> holdIds = {
		30117, 30617, 31117, 31617, 31817, 32018, 32217, 32417, 32617, 32817, 33017, 33217,
		33387, 33692, 33994, 34297, 19003, 35506, 34902, 34598, 36386, 36688, 36084, 35100,
		36969, 37104, 37239, 37374, 37701, 37836, 37971, 38106, 38709, 38849, 38569, 38434,
		23648, 23702, 23594, 23540
	};
	static const std::unordered_set<UI16> wheelIds = {
		30141, 30641, 31141, 31642, 33604, 33906, 34208, 34510,
		37650, 37652, 37654, 37656, 23618
	};
	static const std::unordered_set<UI16> cannonPadIds = {
		30012,30037,30041,30065,30069,30093,30097,30512,30537,30541,30565,30569,30593,30597,
		31012,31037,31041,31065,31069,31093,31097,31512,31537,31541,31565,31569,31593,31597,
		31712,31737,31741,31765,31769,31793,31797,31913,31938,31942,31966,31970,31994,31998,
		32112,32137,32141,32165,32169,32193,32197,32312,32337,32341,32365,32369,32393,32397,
		32512,32537,32541,32565,32569,32593,32597,32712,32737,32741,32765,32769,32793,32797,
		32912,32937,32941,32965,32969,32993,32997,33112,33137,33141,33165,33169,33193,33197,
		18956,18958,18977,18979,19040,19042,19059,33322,33327,33355,33357,33438,33440,33463,
		33627,33632,33660,33662,33743,33745,33768,33929,33934,33962,33964,34045,34047,34070,
		34232,34237,34265,34267,34348,34350,34373,34533,34538,34566,34568,34649,34651,34674,
		34837,34842,34870,34872,34953,34955,34978,35053,35055,35074,35076,35137,35139,35156,
		35441,35446,35474,35476,35557,35559,35582,36037,36039,36058,36060,36121,36123,36140,
		36339,36341,36360,36362,36423,36425,36442,36641,36662,36664,36725,36727,36733,36744,
		36981,36985,37016,37020,37054,37116,37120,37151,37155,37189,37251,37255,37286,37290,
		37324,37386,37390,37421,37425,37459,37713,37717,37748,37752,37786,37848,37852,37883,
		37887,37921,37983,37987,38018,38022,38056,38118,38122,38153,38157,38191,38446,38450,
		38481,38485,38519,38581,38585,38616,38620,38654,38721,38725,38756,38760,38794,38861,
		38865,38896,38900,38934,23504,23522,23523,23524,23526,23529,23531,23533,23535,23558,
		23576,23577,23578,23580,23583,23585,23587,23589,23612,23630,23631,23632,23634,23637,
		23639,23641,23643,23666,23684,23685,23686,23688,23691,23693,23695,23697
	};

	// Placeholder components are not rendered as part of the multi. Preserve
	// their objects across turns, then remap their art at the rotated offsets.
	std::vector<CItem *> oldFixtures;
	for( auto *item : boat->GetItemsInMultiList()->collection() )
	{
		if( ValidateObject( item ) && item->GetTag( "hsGalleonFixture" ).m_IntValue == 1 )
		{
			TAGMAPOBJECT parentTag;
			parentTag.m_Destroy = false;
			parentTag.m_IntValue = boat->GetSerial();
			parentTag.m_ObjectType = TAGMAP_TYPE_INT;
			parentTag.m_StringValue = "";
			item->SetTag( "hsGalleonSerial", parentTag );
			oldFixtures.push_back( item );
		}
	}
	const auto &directionComponents = Map->SeekMulti( boat->GetId() - 0x4000 ).items;
	SI16 fixtureRange = 0;
	for( const auto &component : directionComponents )
		fixtureRange = std::max<SI16>( fixtureRange, std::max<SI16>( std::abs( component.offsetX ), std::abs( component.offsetY )));
	// SetLocation can temporarily recalculate a fixture out of its multi's live
	// collection. Recover it by its persistent parent serial so a later turn can
	// never strand real deck pieces behind the vessel.
	for( auto *candidate : FindNearbyItems( boat->GetX(), boat->GetY(), boat->WorldNumber(), boat->GetInstanceId(),
		static_cast<UI16>( fixtureRange + 2 )))
	{
		if( ValidateObject( candidate ) && candidate->GetTag( "hsGalleonFixture" ).m_IntValue == 1 &&
			candidate->GetTag( "hsGalleonSerial" ).m_IntValue == static_cast<SI32>( boat->GetSerial() ) &&
			std::find( oldFixtures.begin(), oldFixtures.end(), candidate ) == oldFixtures.end() )
			oldFixtures.push_back( candidate );
	}
	// Generated fixtures from older saves can retain their fixture tag while
	// losing MultiID. Reattach only unowned items that exactly match a component
	// (art and world position) in this vessel's current directional multi.
	if( fromConstruct )
	{
		for( auto *candidate : FindNearbyItems( boat->GetX(), boat->GetY(), boat->WorldNumber(), boat->GetInstanceId(),
			static_cast<UI16>( fixtureRange + 1 )))
		{
			if( !ValidateObject( candidate ) || candidate->GetTag( "hsGalleonFixture" ).m_IntValue != 1 )
				continue;
			auto *owner = candidate->GetMultiObj();
			if( ValidateObject( owner ) && owner != boat )
				continue;
			bool exactComponent = false;
			for( const auto &component : directionComponents )
			{
				if(( component.flag == 0 || component.flag == 0x800 ) && candidate->GetId() == component.tileId &&
					candidate->GetX() == boat->GetX() + component.offsetX && candidate->GetY() == boat->GetY() + component.offsetY &&
					candidate->GetZ() == boat->GetZ() + component.altitude )
				{
					exactComponent = true;
					break;
				}
			}
			if( exactComponent )
			{
				candidate->SetMulti( boat );
				if( std::find( oldFixtures.begin(), oldFixtures.end(), candidate ) == oldFixtures.end() )
					oldFixtures.push_back( candidate );
			}
		}
	}
	// Directional High Seas multis do not provide a stable one-to-one fixture
	// identity across every facing. On a turn, rebuild only our generated
	// fixtures from the destination multi records so their art and offsets are
	// exactly those specified for that hull direction.
	if( !fromConstruct )
	{
		for( auto *item : oldFixtures )
		{
			if( ValidateObject( item ))
				item->Delete();
		}
		oldFixtures.clear();
		boat->ClearFixtures();
	}
	bool foundHold = false;
	bool foundWheel = false;
	UI08 mooringCount = 0;
	for( const auto &component : Map->SeekMulti( boat->GetId() - 0x4000 ).items )
	{
		CItem *fixture = nullptr;
		if( !foundHold && holdIds.find( component.tileId ) != holdIds.end() )
		{
			fixture = hold;
			foundHold = true;
		}
		else if( !foundWheel && wheelIds.find( component.tileId ) != wheelIds.end() )
		{
			fixture = tiller;
			foundWheel = true;
		}
		else if(( component.tileId == 0x14F8 || component.tileId == 0x14FA ) && mooringCount < 2 )
		{
			fixture = ( mooringCount++ == 0 ) ? portPlank : starboardPlank;
		}

		if( fixture != nullptr )
		{
			fixture->SetId( component.tileId );
			fixture->SetLocation( boat->GetX() + component.offsetX, boat->GetY() + component.offsetY,
				static_cast<SI08>( boat->GetZ() + component.altitude ), boat->WorldNumber(), boat->GetInstanceId() );
			fixture->SetMulti( boat );
			// Wheel/hold/mooring fixtures keep their serials, so explicitly send
			// their directional art and position during the turn refresh.
			fixture->Update();
		}
		else if( component.flag == 0 || component.flag == 0x800 )
		{
			CItem *deckPiece = nullptr;
			const SI16 fixtureX = boat->GetX() + component.offsetX;
			const SI16 fixtureY = boat->GetY() + component.offsetY;
			const SI08 fixtureZ = static_cast<SI08>( boat->GetZ() + component.altitude );
			for( auto *candidate : oldFixtures )
			{
				if( ValidateObject( candidate ) && candidate->GetId() == component.tileId &&
					candidate->GetX() == fixtureX && candidate->GetY() == fixtureY && candidate->GetZ() == fixtureZ )
				{
					deckPiece = candidate;
					break;
				}
			}
			if( !ValidateObject( deckPiece ))
			{
				deckPiece = Items->CreateItem( nullptr, nullptr, component.tileId, 1, 0, OT_ITEM, false, true,
					boat->WorldNumber(), boat->GetInstanceId(), fixtureX, fixtureY, fixtureZ );
			}
			if( ValidateObject( deckPiece ))
			{
				deckPiece->SetId( component.tileId );
				deckPiece->SetLocation( fixtureX, fixtureY, fixtureZ, boat->WorldNumber(), boat->GetInstanceId() );
				deckPiece->SetMovable( 2 );
				deckPiece->SetDecayable( false );
				deckPiece->SetMulti( boat );
				TAGMAPOBJECT fixtureTag;
				fixtureTag.m_Destroy = false;
				fixtureTag.m_IntValue = 1;
				fixtureTag.m_ObjectType = TAGMAP_TYPE_INT;
				fixtureTag.m_StringValue = "";
				deckPiece->SetTag( "hsGalleonFixture", fixtureTag );
				TAGMAPOBJECT parentTag;
				parentTag.m_Destroy = false;
				parentTag.m_IntValue = boat->GetSerial();
				parentTag.m_ObjectType = TAGMAP_TYPE_INT;
				parentTag.m_StringValue = "";
				deckPiece->SetTag( "hsGalleonSerial", parentTag );
				boat->RegisterFixture( deckPiece->GetSerial() );
				if( cannonPadIds.find( component.tileId ) != cannonPadIds.end() )
					deckPiece->SetTag( "hsWeaponPad", fixtureTag );
				// Newly reconstructed directional fixtures must be sent now. Their
				// ordinary dirty updates can be suppressed by the surrounding boat
				// turn/pause sequence, which otherwise leaves holes until a teleport
				// forces the client to rebuild its world view.
				deckPiece->Update();
			}
		}
	}
	return foundHold && foundWheel;
}

bool RestoreHighSeasBoatFixtures( CBoatObj *boat )
{
	if( !ValidateObject( boat ))
		return false;
	return ConfigureHighSeasFixtures( boat, CalcItemObjFromSer( boat->GetTiller() ), CalcItemObjFromSer( boat->GetPlank( 0 )),
		CalcItemObjFromSer( boat->GetPlank( 1 )), CalcItemObjFromSer( boat->GetHold() ), true );
}

static void ClearLegacyBoatPilotTags( CChar *pilot )
{
	if( !ValidateObject( pilot ))
		return;
	// Older High Seas test builds persisted a captured deck Z and reapplied it
	// during movement, release, and login. ServUO never changes a pilot's Z;
	// remove those legacy values without applying them to the character.
	TAGMAPOBJECT destroyTag;
	destroyTag.m_Destroy = true;
	destroyTag.m_ObjectType = TAGMAP_TYPE_INT;
	destroyTag.m_IntValue = 0;
	destroyTag.m_StringValue = "";
	pilot->SetTag( "hsPilotDeckZ", destroyTag );
	pilot->SetTag( "hsPilotDeckZValid", destroyTag );
	pilot->SetTag( "hsPilotBoatSerial", destroyTag );
}

// ServUO forcibly releases a galleon pilot on disconnection and death. Keep
// that lifecycle in one native helper so every exit path clears movement and
// the boat-side pilot reference before the virtual mount is removed.
void ReleaseBoatPilot( CChar *pilot )
{
	if( !ValidateObject( pilot ))
		return;
	// Resolve the same persisted BoatMountItem -> BaseBoat relationship ServUO
	// uses. Spatial FindMulti can select a touching vessel and is only a fallback
	// for sessions created before the mount relationship was introduced.
	CItem *pilotMount = pilot->GetItemAtLayer( IL_MOUNT );
	SERIAL capturedBoatSerial = INVALIDSERIAL;
	if( ValidateObject( pilotMount ) && pilotMount->GetId() == 0x3E96 )
		capturedBoatSerial = pilotMount->GetTempVar( CITV_MOREX );
	if( capturedBoatSerial == INVALIDSERIAL )
		capturedBoatSerial = static_cast<SERIAL>( pilot->GetTag( "hsPilotBoatSerial" ).m_IntValue );
	CMultiObj *multi = CalcMultiFromSer( capturedBoatSerial );
	if( !ValidateObject( multi ))
		multi = pilot->GetMultiObj();
	if( !ValidateObject( multi ))
		multi = FindMulti( pilot );
	if( ValidateObject( multi ) && multi->CanBeObjType( OT_BOAT ))
	{
		auto *boat = static_cast<CBoatObj *>( multi );
		// Pilot state is deliberately transient, like ServUO's BaseBoat.Pilot.
		// After a restart the boat correctly has no pilot, while an older world
		// save may still contain the character's equipped virtual mount. Clear
		// boat movement only when this character still owns the live session.
		if( boat->GetPilot() == pilot->GetSerial() ||
			( ValidateObject( pilotMount ) && boat->GetPilotMount() == pilotMount->GetSerial() ))
		{
			boat->SetPilot( INVALIDSERIAL );
			boat->SetPilotMount( INVALIDSERIAL );
			boat->SetPilotSpeed( 0 );
			boat->SetMoveType( BOAT_STOP );
			boat->SetMoveTime( 0 );
		}
	}
	// This must also run for a stale post-restart mount. Do not restore the old
	// captured Z: ServUO leaves the mobile's location unchanged when releasing
	// a pilot and only removes/internalizes its virtual mount.
	ClearLegacyBoatPilotTags( pilot );
}

static void RestoreDryDockedCannons( CBoatObj *boat )
{
	if( !ValidateObject( boat )) return;
	const SI32 count = boat->GetTag( "hsDockedCannonCount" ).m_IntValue;
	for( SI32 i = 0; i < count; ++i )
	{
		const auto record = boat->GetTag( oldstrutil::format( "hsDockedCannon%d", i )).m_StringValue;
		const auto fields = oldstrutil::sections( record, "," );
		if( fields.size() < 5 ) continue;
		const SI16 x = boat->GetX() + oldstrutil::value<SI16>( fields[0] );
		const SI16 y = boat->GetY() + oldstrutil::value<SI16>( fields[1] );
		const SI08 z = static_cast<SI08>( boat->GetZ() + oldstrutil::value<SI16>( fields[2] ));
		const SI32 power = fields.size() >= 6 ? oldstrutil::value<SI32>( fields[5] ) : 1;
		const UI16 artOffset = power == 2 ? 4 : 0;
		auto *cannon = Items->CreateItem( nullptr, nullptr, 16920 + artOffset, 1, 0, OT_ITEM, false, true,
			boat->WorldNumber(), boat->GetInstanceId(), x, y, z );
		if( !ValidateObject( cannon )) continue;
		cannon->SetName( power == 2 ? "heavy ship cannon" : "light ship cannon" );
		cannon->SetMovable( 2 );
		cannon->SetDecayable( false );
		cannon->SetMulti( boat );
		cannon->AddScriptTrigger( 5099 );
		auto setIntTag = [cannon]( const std::string &name, SI32 value )
		{
			TAGMAPOBJECT tag;
			tag.m_Destroy = false;
			tag.m_IntValue = value;
			tag.m_ObjectType = TAGMAP_TYPE_INT;
			cannon->SetTag( name, tag );
		};
		setIntTag( "hsCannonKind", 2 );
		setIntTag( "hsCannonPower", power );
		setIntTag( "hsCannonStage", 1 ); // ServUO cannons are constructed clean.
		setIntTag( "hsCannonShots", 0 );
		setIntTag( "hsCannonAmmo", 0 );
		setIntTag( "hsCannonHits", oldstrutil::value<SI32>( fields[3] ));
		setIntTag( "hsPreferredAmmo", oldstrutil::value<SI32>( fields[4] ));
		for( auto *pad : boat->GetItemsInMultiList()->collection() )
		{
			if( !ValidateObject( pad ) || pad->GetTag( "hsWeaponPad" ).m_IntValue != 1 || pad->GetX() != x || pad->GetY() != y ) continue;
			setIntTag( "hsWeaponPadSerial", pad->GetSerial() );
			TAGMAPOBJECT serialTag;
			serialTag.m_Destroy = false;
			serialTag.m_IntValue = cannon->GetSerial();
			serialTag.m_ObjectType = TAGMAP_TYPE_INT;
			pad->SetTag( "hsCannonSerial", serialTag );
			break;
		}
		const UI08 facing = boat->GetDir() & 0x07;
		if( facing == NORTH || facing == SOUTH ) cannon->SetId(( x < boat->GetX() ? 16919 : ( x > boat->GetX() ? 16921 : ( facing == NORTH ? 16920 : 16918 ))) + artOffset );
		else cannon->SetId(( y < boat->GetY() ? 16920 : ( y > boat->GetY() ? 16918 : ( facing == EAST ? 16921 : 16919 ))) + artOffset );
		cannon->SetDir( facing );
		cannon->Update();
	}
}

static UI08 HighSeasDamageValue( const CBoatObj *boat )
{
	const UI08 level = boat->GetHullDamageLevel();
	return level >= 4 ? 2 : ( level >= 2 ? 1 : 0 );
}

static void RefreshHighSeasDamageState( CBoatObj *boat )
{
	if( !ValidateObject( boat ))
		return;
	const UI08 baseId = boat->GetTempVar( CITV_MOREZ, 1 );
	if( baseId != 0x18 && baseId != 0x24 && baseId != 0x30 && baseId != 0x40 )
		return;
	const UI08 directionIndex = static_cast<UI08>(( boat->GetDir() & 0x07 ) / 2 );
	const UI08 targetId = static_cast<UI08>( baseId + directionIndex + HighSeasDamageValue( boat ) * 4 );
	if( boat->GetId( 2 ) == targetId )
		return;
	boat->SetId( targetId, 2 );
	ConfigureHighSeasFixtures( boat, CalcItemObjFromSer( boat->GetTiller() ), CalcItemObjFromSer( boat->GetPlank( 0 )),
		CalcItemObjFromSer( boat->GetPlank( 1 )), CalcItemObjFromSer( boat->GetHold() ), false );
	boat->RemoveFromSight();
	boat->Update();
}

void DamageBoatHull( CBoatObj *boat, SI32 amount )
{
	if( !ValidateObject( boat ) || amount <= 0 || boat->GetHullMaxHits() <= 0 )
		return;
	const UI08 oldLevel = boat->GetHullDamageLevel();
	boat->SetHullHits( boat->GetHullHits() - amount );
	if( boat->GetHullDamageLevel() != oldLevel )
		RefreshHighSeasDamageState( boat );
	if( boat->IsScuttled() )
	{
		boat->SetMoveType( BOAT_STOP );
		boat->SetPilotSpeed( 0 );
	}
}

SI32 RepairBoatHull( CBoatObj *boat, SI32 amount )
{
	if( !ValidateObject( boat ) || amount <= 0 || boat->GetHullMaxHits() <= 0 )
		return 0;
	const SI32 oldHits = boat->GetHullHits();
	const UI08 oldLevel = boat->GetHullDamageLevel();
	boat->SetHullHits( oldHits + amount );
	if( boat->GetHullDamageLevel() != oldLevel )
		RefreshHighSeasDamageState( boat );
	return boat->GetHullHits() - oldHits;
}

bool IsBoatNearLandOrDocks( CBoatObj *boat )
{
	if( !ValidateObject( boat ))
		return false;
	for( SI16 x = boat->GetX() - 12; x <= boat->GetX() + 12; ++x )
	{
		for( SI16 y = boat->GetY() - 12; y <= boat->GetY() + 12; ++y )
		{
			auto land = Map->SeekMap( x, y, boat->WorldNumber() );
			if( land.terrainInfo != nullptr && !land.CheckFlag( TF_BLOCKING ) && !land.CheckFlag( TF_WET ))
				return true;
			for( const auto &tile : Map->ArtAt( x, y, boat->WorldNumber() ))
			{
				const auto name = oldstrutil::lower( tile.name() );
				if( name == "wooden plank" || name == "pier" )
					return true;
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
		case 0x18: // Orcish galleon
		case 0x24: // Gargish galleon
		case 0x30: // Tokuno galleon
		case 0x3C: // Rowboat
		case 0x40: // Britannian ship
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
	// BuildHouse has already resolved the deed target's surface Z. Do not run
	// DynamicElevation/StaticTop after the multi exists: those queries can see
	// the newly created hull (or a nearby dock) and incorrectly raise a vessel
	// placed on water at -5 up to Z 0. ServUO preserves the placement point Z.
	const SI08 z = b->GetZ();
	SI32 hullMaxHits = 0;
	switch( id2 )
	{
		case 0x18: hullMaxHits = 100000; break; // ServUO OrcishGalleon.MaxHits
		case 0x24: hullMaxHits = 140000; break; // ServUO GargishGalleon.MaxHits
		case 0x30: hullMaxHits = 100000; break; // ServUO TokunoGalleon.MaxHits
		case 0x40: hullMaxHits = 200000; break; // ServUO BritannianShip.MaxHits
		default: break;
	}
	if( hullMaxHits > 0 )
	{
		b->SetHullMaxHits( hullMaxHits );
		b->SetHullHits( hullMaxHits );
		b->SetDamageable( true );
	}
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
		case 0x18:
		case 0x24:
		case 0x30:
		case 0x40:
			tiller->SetLocation( x + 1, y + 5, z );
			p1->SetLocation( x - 2, y - 1, z );
			p2->SetLocation( x + 2, y - 1, z );
			hold->SetLocation( x, y - 5, z );
			break;
		case 0x3C:
			tiller->SetLocation( x + 1, y + 4, z );
			p1->SetLocation( x - 2, y, z );
			p2->SetLocation( x + 2, y, z );
			hold->SetLocation( x, y - 4, z );
			break;
	}
	ConfigureHighSeasFixtures( b, tiller, p1, p2, hold, true );
	RestoreDryDockedCannons( b );
	if( id2 == 0x18 || id2 == 0x24 || id2 == 0x30 || id2 == 0x40 )
		b->SetMoveType( BOAT_STOP ); // High Seas galleons do not use classic anchors.
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

// ServUO rebuilds its onboard entity set from the current multi bounds for
// every movement step. Do the same here instead of trusting mutable multi lists.
static void CollectBoatEntities( CBoatObj *boat, std::vector<CItem *>& items, std::vector<CChar *>& characters )
{
	items.clear();
	characters.clear();
	if( !ValidateObject( boat ))
		return;

	std::unordered_set<SERIAL> seen;
	// The live multi collection owns cargo/cannons/addons.  The serialized fixture
	// registry owns generated deck pieces even if a location update temporarily
	// removes one from the live collection.  Geometry is never ownership.
	for( auto *item : boat->GetItemsInMultiList()->collection() )
	{
		if( !ValidateObject( item ) || item == boat )
			continue;
		if( item->GetMultiObj() == boat && seen.insert( item->GetSerial() ).second )
			items.push_back( item );
	}
	for( const auto serial : boat->GetFixtures() )
	{
		auto *item = CalcItemObjFromSer( serial );
		if( ValidateObject( item ) && item->GetTag( "hsGalleonSerial" ).m_IntValue == static_cast<SI32>( boat->GetSerial() ) &&
			seen.insert( serial ).second )
			items.push_back( item );
	}
	for( auto *character : boat->GetCharsInMultiList()->collection() )
	{
		if( !ValidateObject( character ))
			continue;
		if( character->GetMultiObj() == boat && seen.insert( character->GetSerial() ).second )
			characters.push_back( character );
	}
	CChar *pilot = CalcCharObjFromSer( boat->GetPilot() );
	if( ValidateObject( pilot ) && seen.insert( pilot->GetSerial() ).second )
		characters.push_back( pilot );
}

UI08 CheckHighSeasDryDock( CBoatObj *boat )
{
	if( !ValidateObject( boat )) return 7;
	if( boat->GetHullMaxHits() <= 0 || boat->GetHullHits() != boat->GetHullMaxHits() ) return 1;
	auto *hold = CalcItemObjFromSer( boat->GetHold() );
	if( !ValidateObject( hold ) || hold->GetContainsList()->Num() > 0 ) return 2;
	std::vector<CItem *> items;
	std::vector<CChar *> characters;
	CollectBoatEntities( boat, items, characters );
	if( !characters.empty() ) return 3;
	auto *tiller = CalcItemObjFromSer( boat->GetTiller() );
	auto *p1 = CalcItemObjFromSer( boat->GetPlank( 0 ));
	auto *p2 = CalcItemObjFromSer( boat->GetPlank( 1 ));
	for( auto *item : items )
	{
		if( !ValidateObject( item ) || item == tiller || item == p1 || item == p2 || item == hold ||
			item->GetTag( "hsGalleonFixture" ).m_IntValue == 1 ) continue;
		if( item->GetTag( "hsCannonKind" ).m_IntValue == 2 )
		{
			if( item->GetTag( "hsCannonStage" ).m_IntValue > 1 ) return 5;
			continue;
		}
		return 4;
	}
	return 0;
}

bool DeleteHighSeasBoatForDryDock( CBoatObj *boat )
{
	if( CheckHighSeasDryDock( boat ) != 0 ) return false;
	std::vector<CItem *> items;
	std::vector<CChar *> characters;
	CollectBoatEntities( boat, items, characters );
	auto addUnique = [&items]( CItem *item )
	{
		if( ValidateObject( item ) && std::find( items.begin(), items.end(), item ) == items.end() ) items.push_back( item );
	};
	addUnique( CalcItemObjFromSer( boat->GetTiller() ));
	addUnique( CalcItemObjFromSer( boat->GetPlank( 0 )));
	addUnique( CalcItemObjFromSer( boat->GetPlank( 1 )));
	addUnique( CalcItemObjFromSer( boat->GetHold() ));
	for( auto *item : items )
	{
		if( ValidateObject( item )) item->Delete();
	}
	boat->Delete();
	return true;
}

// Smooth boat movement carries the pilot client without running the ordinary
// walking visibility pass. Resend nearby stationary galleons and their owned
// dynamic fixtures so a vessel that re-enters view is complete without a
// teleport-forced world refresh.
static void RefreshNearbyHighSeasBoats( CBoatObj *movingBoat, const std::vector<CSocket *>& sockets )
{
	if( !ValidateObject( movingBoat ))
		return;
	for( auto *socket : sockets )
	{
		if( socket == nullptr || socket->ClientVersion() < CV_HS2D || socket->ClientVerShort() < CVS_7090 )
			continue;
		auto *viewer = socket->CurrcharObj();
		if( !ValidateObject( viewer ))
			continue;
		std::unordered_set<SERIAL> sentBoats;
		for( auto *nearby : FindNearbyItems( viewer->GetX(), viewer->GetY(), viewer->WorldNumber(), viewer->GetInstanceId(), DIST_BUILDRANGE ))
		{
			CBoatObj *otherBoat = nullptr;
			if( ValidateObject( nearby ) && nearby->CanBeObjType( OT_BOAT ))
				otherBoat = static_cast<CBoatObj *>( nearby );
			else if( ValidateObject( nearby ) && ValidateObject( nearby->GetMultiObj() ) && nearby->GetMultiObj()->CanBeObjType( OT_BOAT ))
				otherBoat = static_cast<CBoatObj *>( nearby->GetMultiObj() );
			if( !ValidateObject( otherBoat ) || otherBoat == movingBoat || !sentBoats.insert( otherBoat->GetSerial() ).second )
				continue;
			otherBoat->SendToSocket( socket );
			for( auto *ownedItem : otherBoat->GetItemsInMultiList()->collection() )
			{
				if( ValidateObject( ownedItem ) && ownedItem->GetMultiObj() == otherBoat )
					ownedItem->SendToSocket( socket );
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	MoveBoat()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Move the boat and everything on it 1 tile in its current direction
//o------------------------------------------------------------------------------------------------o
void MoveBoat( UI08 dir, CBoatObj *boat )
{
	if( ValidateObject( boat ) && boat->IsScuttled() )
	{
		boat->SetMoveType( BOAT_STOP );
		boat->SetPilotSpeed( 0 );
		return;
	}
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
		if( tSock->ClientVersion() < CV_HS2D || tSock->ClientVerShort() < CVS_7090 )
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
			tx = 4970;
			teleportBoat = true;
		}
		else if(( x + tx ) >= 5030 && tx > 0 )
		{
			// Sailing east
			tx = -4970;
			teleportBoat = true;
		}
		else if(( y + ty ) <= 15 && ty < 0 )
		{
			// Sailing north
			ty = 4020;
			teleportBoat = true;
		}
		else if(( y + ty ) >= 4060 && ty > 0 )
		{
			// Sailing south
			ty = -4040;
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

	// Moving an item recalculates its multi and can remove it from the boat's
	// live collection. Snapshot the complete onboard set before moving anything.
	std::vector<CItem *> boatItems;
	std::vector<CChar *> boatCharacters;
	CollectBoatEntities( boat, boatItems, boatCharacters );

	if( !teleportBoat )
	{
		const UI08 clientSpeed = boat->GetPilotSpeed() == 1 || boat->GetHullDamageLevel() >= 3 ? 0x02 : 0x04;
		CPBoatSmoothMove smoothMove( boat, boatItems, boatCharacters, dir, clientSpeed, tx, ty );
		for( auto &tSock : nearbyChars )
			tSock->Send( &smoothMove );
	}
	
	// Move all the special items along with the boat
	boat->IncLocation( tx, ty );
	tiller->IncLocation( tx, ty );
	p1->IncLocation( tx, ty );
	p2->IncLocation( tx, ty );
	hold->IncLocation( tx, ty );
	tiller->SetMulti( boat );
	p1->SetMulti( boat );
	p2->SetMulti( boat );
	hold->SetMulti( boat );

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
	for( auto *bItem : boatItems )
	{
		if( ValidateObject( bItem ))
		{
			if( !(bItem == tiller || bItem == p1 || bItem == p2 || bItem == hold ))
			{
				bItem->IncLocation( tx, ty );
				if( bItem->GetMultiObj() != boat )
					bItem->SetMulti( boat );
				
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
	for( auto *bChar : boatCharacters )
	{
		if( ValidateObject( bChar ))
		{
			bChar->SetLocation( bChar->GetX() + tx, bChar->GetY() + ty, bChar->GetZ() );
			if( bChar->GetMultiObj() != boat )
				bChar->SetMulti( boat );
			if( teleportBoat )
			{
				bChar->Update();
			}
		}
	}

	if( !teleportBoat )
	{
		// The 0xF6 packet already moved these objects for High Seas clients.
		// Suppress the ordinary dirty-queue location packets or clients apply
		// both movements and temporarily tear the vessel apart.
		boat->RemoveFromRefreshQueue();
		tiller->RemoveFromRefreshQueue();
		p1->RemoveFromRefreshQueue();
		p2->RemoveFromRefreshQueue();
		hold->RemoveFromRefreshQueue();
		for( auto *bItem : boatItems )
		{
			if( ValidateObject( bItem ))
				bItem->RemoveFromRefreshQueue();
		}
		for( auto *bChar : boatCharacters )
		{
			if( ValidateObject( bChar ))
			{
				bChar->RemoveFromRefreshQueue();
				bChar->ClearUpdate();
			}
		}

		// Clients predating High Seas cannot consume 0xF6, so update their
		// view explicitly after suppressing the global refresh queue.
		for( auto &tSock : nearbyChars )
		{
			if( tSock->ClientVersion() >= CV_HS2D && tSock->ClientVerShort() >= CVS_7090 )
				continue;
			boat->Update( tSock );
			tiller->Update( tSock );
			p1->Update( tSock );
			p2->Update( tSock );
			hold->Update( tSock );
			for( auto *bItem : boatItems )
			{
				if( ValidateObject( bItem ) && bItem != tiller && bItem != p1 && bItem != p2 && bItem != hold )
					bItem->Update( tSock );
			}
			for( auto *bChar : boatCharacters )
			{
				if( ValidateObject( bChar ))
					bChar->Update( tSock );
			}
		}
	}
	for( auto &tSock: nearbyChars )
	{
		if( tSock->ClientVersion() < CV_HS2D || tSock->ClientVerShort() < CVS_7090 )
		tSock->Send( &prSend );
	}
	RefreshNearbyHighSeasBoats( boat, nearbyChars );
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
	if( b->IsScuttled() )
	{
		b->SetMoveType( BOAT_STOP );
		b->SetPilotSpeed( 0 );
		return;
	}

	SI16 id2	= b->GetId( 2 );
	UI08 olddir = b->GetDir();
	const UI08 baseId = b->GetTempVar( CITV_MOREZ, 1 );
	const bool highSeasHull = baseId == 0x18 || baseId == 0x24 || baseId == 0x30 || baseId == 0x40;

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
		b->SetDir( static_cast<UI08>(( olddir + 2 ) & 0x07 ));
		++id2;
	}
	else
	{
		b->SetDir( static_cast<UI08>(( olddir + 6 ) & 0x07 ));
		--id2;
	}

	if( highSeasHull )
	{
		id2 = baseId + (( b->GetDir() & 0x07 ) / 2 ) + HighSeasDamageValue( b ) * 4;
	}
	else if( id2 < b->GetTempVar( CITV_MOREZ, 1 ))
	{
		id2 += 4;//make sure we don't have any id errors either
	}
	if( !highSeasHull && id2 > b->GetTempVar( CITV_MOREZ, 2 ))
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

	// Capture everything aboard while the hull still has its old orientation.
	// Once the multi ID changes, its component bounds describe the destination
	// orientation and can no longer reliably find fixtures at their old offsets.
	std::vector<CItem *> turnItems;
	std::vector<CChar *> turnCharacters;
	CollectBoatEntities( b, turnItems, turnCharacters );

	b->SetId( static_cast<UI08>( id2 ), 2 );//set the id

	if( !highSeasHull && b->GetId( 2 ) == b->GetTempVar( CITV_MOREZ, 1 ))
	{
		b->SetDir( NORTH );//extra DIR error checking
	}
	if( !highSeasHull && b->GetId( 2 ) == b->GetTempVar( CITV_MOREZ, 2 ))
	{
		b->SetDir( WEST );
	}


	for( auto *bItem : turnItems )
	{
		if( ValidateObject( bItem ))
		{
			TurnStuff( b, bItem, rightTurn );
			if( highSeasHull && bItem->GetTag( "hsCannonKind" ).m_IntValue == 2 )
			{
				// Exact ServUO BaseGalleon.UpdateCannonID rule: broadside art
				// is chosen from the cannon's position relative to hull center.
				const UI16 artOffset = bItem->GetTag( "hsCannonPower" ).m_IntValue == 2 ? 4 : 0;
				UI16 cannonId = 16920;
				if( b->GetDir() == NORTH || b->GetDir() == SOUTH )
				{
					if( bItem->GetX() < b->GetX() ) cannonId = 16919;
					else if( bItem->GetX() > b->GetX() ) cannonId = 16921;
					else cannonId = b->GetDir() == NORTH ? 16920 : 16918;
				}
				else
				{
					if( bItem->GetY() < b->GetY() ) cannonId = 16920;
					else if( bItem->GetY() > b->GetY() ) cannonId = 16918;
					else cannonId = b->GetDir() == EAST ? 16921 : 16919;
				}
				bItem->SetId( cannonId + artOffset );
				bItem->SetDir( b->GetDir() & 0x07 );
				bItem->Update();
			}
			if( bItem->GetMultiObj() != b )
				bItem->SetMulti( b );
		}
	}
	for( auto *bChar : turnCharacters )
	{
		if( ValidateObject( bChar ))
		{
			// ServUO rotates every mobile's facing by the same delta as the
			// vessel, in addition to rotating its location around the hull.
			if( b->GetPilot() == bChar->GetSerial() )
			{
				// A mounted High Seas pilot faces with the vessel; this is also how
				// the client presents steering at the wheel.
				bChar->SetDir( b->GetDir() & 0x07 );
			}
			else
			{
				bChar->SetDir( static_cast<UI08>(( static_cast<SI16>( bChar->GetDir() & 0x07 ) -
					static_cast<SI16>( olddir & 0x07 ) + static_cast<SI16>( b->GetDir() & 0x07 ) + 8 ) & 0x07 ));
			}
			TurnStuff( b, bChar, rightTurn );
			if( bChar->GetMultiObj() != b )
				bChar->SetMulti( b );
			bChar->Update();
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
		case 0x18:
		case 0x24:
		case 0x30:
		case 0x40:
			p1->IncLocation( iLargeShipOffsets[dir][PORT_PLANK][XP], iLargeShipOffsets[dir][PORT_PLANK][YP] );
			p2->IncLocation( iLargeShipOffsets[dir][STARB_PLANK][XP], iLargeShipOffsets[dir][STARB_PLANK][YP] );
			tiller->IncLocation( iLargeShipOffsets[dir][TILLER][XP], iLargeShipOffsets[dir][TILLER][YP] );
			hold->IncLocation( iLargeShipOffsets[dir][HOLD][XP], iLargeShipOffsets[dir][HOLD][YP] );
			break;
		case 0x3C:
			p1->IncLocation( iSmallShipOffsets[dir][PORT_PLANK][XP], iSmallShipOffsets[dir][PORT_PLANK][YP] );
			p2->IncLocation( iSmallShipOffsets[dir][STARB_PLANK][XP], iSmallShipOffsets[dir][STARB_PLANK][YP] );
			tiller->IncLocation( iSmallShipOffsets[dir][TILLER][XP], iSmallShipOffsets[dir][TILLER][YP] );
			hold->IncLocation( iSmallShipOffsets[dir][HOLD][XP], iSmallShipOffsets[dir][HOLD][YP] );
			break;
		default: Console.Error( oldstrutil::format( "TurnBoat() more1 error! more1 = %c not found!", b->GetTempVar( CITV_MOREZ, 1 )));
	}
	ConfigureHighSeasFixtures( b, tiller, p1, p2, hold, false );

	// A multi keeps the same serial when its directional ID changes. High Seas
	// clients can retain the old hull geometry if they only receive an ordinary
	// item update, even though all dynamic fixtures have already rotated. Remove
	// and resend the completed multi so the client loads the new directional hull.
	b->RemoveFromSight();
	b->Update();
	// Refresh the mounted pilot after the hull and its directional fixtures are
	// complete. Sending the mobile before the multi replacement can leave the
	// client displaying the pilot at the rotated location on the old hull.
	auto *pilot = CalcCharObjFromSer( b->GetPilot() );
	if( ValidateObject( pilot ))
	{
		pilot->SetDir( b->GetDir() & 0x07 );
		pilot->Update();
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
			if( toExecute->OnBoatTurn( b, olddir, b->GetDir(), tiller ) == 1 )
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


