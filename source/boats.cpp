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

namespace
{
	constexpr UI08 HIGH_SEAS_MAX_PAINT_COATS = 4;

	UI64 HighSeasPaintDecaySeconds()
	{
		return static_cast<UI64>( cwmWorldState->ServerData()->BoatPaintDecaySeconds() );
	}

	bool IsHighSeasHull( const CBoatObj *boat )
	{
		if( !ValidateObject( boat ))
			return false;
		const UI08 baseId = boat->GetTempVar( CITV_MOREZ, 1 );
		return baseId == 0x18 || baseId == 0x24 || baseId == 0x30 || baseId == 0x40;
	}

	void ApplyHighSeasBoatPaintColour( CBoatObj *boat, UI16 hue )
	{
		if( !IsHighSeasHull( boat ))
			return;
		boat->SetColour( hue );
		auto *tiller = CalcItemObjFromSer( boat->GetTiller() );
		auto *portPlank = CalcItemObjFromSer( boat->GetPlank( 0 ));
		auto *starboardPlank = CalcItemObjFromSer( boat->GetPlank( 1 ));
		auto *hold = CalcItemObjFromSer( boat->GetHold() );
		for( auto *item : boat->GetItemsInMultiList()->collection() )
		{
			if( !ValidateObject( item ) || item == tiller || item == portPlank || item == starboardPlank )
				continue;
			if( item == hold || boat->IsFixture( item->GetSerial() ))
			{
				item->SetColour( hue );
				item->Update();
			}
		}
		boat->Update();
	}

	UI16 PaintedHueForCoats( UI16 basePaintHue, UI08 coats )
	{
		// UO brightens these two palettes and darkens the remaining paints
		// one hue step per coat.
		if( basePaintHue == 1900 || basePaintHue == 2213 )
			return static_cast<UI16>( basePaintHue + coats );
		return static_cast<UI16>( basePaintHue - coats );
	}

	void ProcessHighSeasPaintDecay( CBoatObj *boat, UI64 now )
	{
		UI08 coats = boat->GetPaintCoats();
		UI64 nextDecay = boat->GetPaintDecayAt();
		if( coats == 0 || nextDecay == 0 || now < nextDecay )
			return;
		const UI64 paintDecaySeconds = HighSeasPaintDecaySeconds();
		const UI64 elapsedPeriods = 1 + (( now - nextDecay ) / paintDecaySeconds );
		coats = static_cast<UI08>( elapsedPeriods >= coats ? 0 : coats - elapsedPeriods );
		if( coats == 0 )
		{
			const UI16 baseBoatHue = boat->GetPaintBaseBoatHue();
			boat->ClearPaintState();
			ApplyHighSeasBoatPaintColour( boat, baseBoatHue );
		}
		else
		{
			const UI16 basePaintHue = boat->GetPaintHue();
			boat->SetPaintState( boat->GetPaintBaseBoatHue(), basePaintHue, coats,
				nextDecay + elapsedPeriods * paintDecaySeconds );
			ApplyHighSeasBoatPaintColour( boat, PaintedHueForCoats( basePaintHue, coats ));
		}
	}
}


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
	for( SI16 distance = 1; distance <= 8; ++distance )
	{
		for( SI16 x = x2 - distance; x <= x2 + distance; ++x )
		{
			for( SI16 y = y2 - distance; y <= y2 + distance; ++y )
			{
				if( x != x2 - distance && x != x2 + distance && y != y2 - distance && y != y2 + distance )
				{
					continue;
				}
				SI08 z = Map->Height( x, y, mChar->GetZ(), worldNumber, instanceId );
				auto *destinationMulti = FindMulti( x, y, z, worldNumber, instanceId );
				if( Map->ValidSpawnLocation( x, y, z, worldNumber, instanceId, true ) &&
					( !ValidateObject( destinationMulti ) || !destinationMulti->CanBeObjType( OT_BOAT )))
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
		case 80: case 81: case 82: case 83: // Pumpkin rowboat
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
	const bool highSeasHull = baseId == 0x18 || baseId == 0x24 || baseId == 0x30 || baseId == 0x3C || baseId == 0x40;
	std::vector<std::pair<SI16, SI16>> collisionPoints;
	if( highSeasHull && !turnBoat )
	{
		const auto &components = Map->SeekMulti( b->GetId() - 0x4000 ).items;
		SI16 minX = 0, maxX = 0, minY = 0, maxY = 0;
		for( const auto &component : components )
		{
			minX = std::min<SI16>( minX, static_cast<SI16>( component.offsetX ));
			maxX = std::max<SI16>( maxX, static_cast<SI16>( component.offsetX ));
			minY = std::min<SI16>( minY, static_cast<SI16>( component.offsetY ));
			maxY = std::max<SI16>( maxY, static_cast<SI16>( component.offsetY ));
			const auto point = std::make_pair( static_cast<SI16>( cx + component.offsetX ),
				static_cast<SI16>( cy + component.offsetY ));
			if( std::find( collisionPoints.begin(), collisionPoints.end(), point ) == collisionPoints.end() )
			{
				collisionPoints.push_back( point );
			}
		}
		x1 = cx + minX;
		x2 = cx + maxX + 1;
		y1 = cy + minY;
		y2 = cy + maxY + 1;
	}
	else
	{
		for( SI16 x = x1; x < x2; ++x )
		{
			for( SI16 y = y1; y < y2; ++y )
			{
				collisionPoints.emplace_back( x, y );
			}
		}
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
	for( const auto &[x, y] : collisionPoints )
	{
		// Look for other boats
		CMultiObj *tempBoat = FindMulti( x, y, boatZ, worldNumber, instanceId );
		if( ValidateObject( tempBoat ) && tempBoat->GetSerial() != b->GetSerial() && !movingAwayFrom( tempBoat ))
		{
			return true;
		}

		// Look for blocking dynamic items at boat's Z level
		CItem *tempItem = GetItemAtXYZ( x, y, boatZ, worldNumber, instanceId );
		if( ValidateObject( tempItem ))
		{
			auto multiSerial = tempItem->GetMulti();
			if( multiSerial != INVALIDSERIAL && multiSerial != b->GetSerial() )
			{
				auto *otherMulti = tempItem->GetMultiObj();
				if( ValidateObject( otherMulti ) && movingAwayFrom( otherMulti ))
				{
					continue;
				}
				CTile &tile = Map->SeekTile( tempItem->GetId() );
				if( tile.CheckFlag( TF_BLOCKING ))
				{
					return true;
				}
			}
		}

		// Docks and addon components are often dynamic items above the
		// waterline, so an exact-Z lookup at the boat's Z misses them.
		for( auto *dynamicItem : nearbyDynamicItems )
		{
			if( !ValidateObject( dynamicItem ) || dynamicItem == b || dynamicItem->GetX() != x || dynamicItem->GetY() != y ||
				dynamicItem->GetMulti() == b->GetSerial() )
			{
				continue;
			}
			auto *otherMulti = dynamicItem->GetMultiObj();
			if( ValidateObject( otherMulti ) && movingAwayFrom( otherMulti ))
			{
				continue;
			}
			CTile &dynamicTile = Map->SeekTile( dynamicItem->GetId() );
			const SI16 dynamicTop = dynamicItem->GetZ() + std::max<SI16>( 1, dynamicTile.Height() );
			if( !dynamicTile.CheckFlag( TF_WET ) && dynamicTop >= cz && dynamicItem->GetZ() <= cz + 20 )
			{
				return true;
			}
		}

		SI08 sz = Map->StaticTop( x, y, boatZ, worldNumber, MAX_Z_STEP );
		if( sz == ILLEGAL_Z ) // map tile
		{
			auto map = Map->SeekMap( x, y, worldNumber );
			if( map.terrainInfo == nullptr || ( map.altitude >= cz && !map.CheckFlag( TF_WET ) && map.name() != "water" ))
			{
				return true;
			}
		}
		else
		{
			auto artwork = Map->ArtAt( x, y, worldNumber );
			for( auto &tile : artwork )
			{
				SI08 zt = tile.altitude + tile.height();
				if( !tile.CheckFlag( TF_WET ) && zt >= cz && zt <= ( cz + 20 ) && tile.name() != "water" )
				{
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
	tiller->AddScriptTrigger( 5101 );

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

	// The boat's serialized fixture registry is the authoritative ownership
	// relationship for generated deck pieces, including while SetLocation
	// temporarily removes an item from the live multi collection.
	std::vector<CItem *> oldFixtures;
	for( const auto fixtureSerial : boat->GetFixtures() )
	{
		auto *fixture = CalcItemObjFromSer( fixtureSerial );
		if( ValidateObject( fixture ))
			oldFixtures.push_back( fixture );
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
		const bool mooringLine = component.tileId == 0x14F8 || component.tileId == 0x14FA;
		const auto componentName = oldstrutil::lower( Map->SeekTile( component.tileId ).Name() );
		const bool wheelComponent = wheelIds.find( component.tileId ) != wheelIds.end() ||
			componentName.find( "wheel" ) != std::string::npos;
		if( !foundHold && holdIds.find( component.tileId ) != holdIds.end() )
		{
			fixture = hold;
			foundHold = true;
		}
		else if( !foundWheel && wheelComponent )
		{
			fixture = tiller;
			foundWheel = true;
			// High Seas uses an interactive wheel item in place of the classic
			// tiller-man fixture. Do not inherit classic boat naming.
			tiller->SetName( "ship wheel" );
			// The wheel artwork's base is below the character walk surface. Keep
			// its directional multi-component altitude for relocation instead of
			// using the static target Z intended for a mobile pilot.
			boat->SetTillermanArtZ( component.altitude );
		}
		else if( mooringLine && mooringCount < 2 )
		{
			fixture = ( mooringCount++ == 0 ) ? portPlank : starboardPlank;
		}

		if( fixture != nullptr )
		{
			fixture->SetId( component.tileId );
			SI16 fixtureX = static_cast<SI16>( boat->GetX() + component.offsetX );
			SI16 fixtureY = static_cast<SI16>( boat->GetY() + component.offsetY );
			SI08 fixtureZ = static_cast<SI08>( boat->GetZ() + component.altitude );
			if( fixture == tiller && boat->IsTillermanMoved() )
			{
				const SI16 localX = boat->GetTillermanLocalX();
				const SI16 localY = boat->GetTillermanLocalY();
				const UI08 facing = boat->GetDir() & 0x06;
				if( facing == EAST )
				{
					fixtureX = static_cast<SI16>( boat->GetX() - localY );
					fixtureY = static_cast<SI16>( boat->GetY() + localX );
				}
				else if( facing == SOUTH )
				{
					fixtureX = static_cast<SI16>( boat->GetX() - localX );
					fixtureY = static_cast<SI16>( boat->GetY() - localY );
				}
				else if( facing == WEST )
				{
					fixtureX = static_cast<SI16>( boat->GetX() + localY );
					fixtureY = static_cast<SI16>( boat->GetY() - localX );
				}
				else
				{
					fixtureX = static_cast<SI16>( boat->GetX() + localX );
					fixtureY = static_cast<SI16>( boat->GetY() + localY );
				}
				// Relocation changes X/Y only. Reapply the destination facing's
				// native wheel-art altitude on every fixture refresh or boat turn.
				fixtureZ = static_cast<SI08>( boat->GetZ() + component.altitude );
			}
			fixture->SetLocation( fixtureX, fixtureY, fixtureZ, boat->WorldNumber(), boat->GetInstanceId() );
			fixture->SetMulti( boat );
			// The hold is paintable; the wheel and mooring lines intentionally
			// retain their native appearance.
			if( fixture == hold )
				fixture->SetColour( boat->GetColour() );
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
				if( mooringLine )
				{
					deckPiece->SetType( IT_PLANK );
				}
				else
				{
					deckPiece->SetColour( boat->GetColour() );
				}
				boat->RegisterFixture( deckPiece->GetSerial() );
				if( cannonPadIds.find( component.tileId ) != cannonPadIds.end() )
					deckPiece->SetCannonRole( CannonRole::WeaponPad );
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

bool RestoreRowboatFixtures( CBoatObj *boat )
{
	if( !ValidateObject( boat ) || boat->GetTempVar( CITV_MOREZ, 1 ) != 0x3C )
	{
		return false;
	}
	auto *rudder = CalcItemObjFromSer( boat->GetTiller() );
	auto *line = CalcItemObjFromSer( boat->GetPlank( 0 ));
	auto *hiddenPlank = CalcItemObjFromSer( boat->GetPlank( 1 ));
	auto *hiddenHold = CalcItemObjFromSer( boat->GetHold() );
	if( !ValidateObject( rudder ) || !ValidateObject( line ) || !ValidateObject( hiddenPlank ) || !ValidateObject( hiddenHold ))
	{
		return false;
	}
	CItem *handle = nullptr;
	for( const auto serial : boat->GetFixtures() )
	{
		auto *candidate = CalcItemObjFromSer( serial );
		if( ValidateObject( candidate ) && candidate->GetType() == IT_TILLER && candidate != rudder )
		{
			handle = candidate;
			break;
		}
	}
	if( !ValidateObject( handle ))
	{
		handle = Items->CreateItem( nullptr, nullptr, 16063, 1, 0, OT_ITEM, false, true,
			boat->WorldNumber(), boat->GetInstanceId(), boat->GetX(), boat->GetY(), boat->GetZ() );
		if( !ValidateObject( handle ))
		{
			return false;
		}
		boat->RegisterFixture( handle->GetSerial() );
	}
	rudder->SetType( IT_TILLER );
	rudder->AddScriptTrigger( 5101 );
	handle->SetType( IT_TILLER );
	handle->AddScriptTrigger( 5101 );
	handle->SetTempVar( CITV_MOREX, rudder->GetTempVar( CITV_MOREX ));
	line->SetType( IT_PLANK );
	boat->SetDamageable( false );
	boat->SetHullMaxHits( 0 );
	boat->SetHullHits( 0 );
	const SI16 x = boat->GetX();
	const SI16 y = boat->GetY();
	const SI08 z = boat->GetZ();
	switch( boat->GetDir() & 0x06 )
	{
		case SOUTH:
			rudder->SetId( 16068 );
			rudder->SetLocation( x, y - 4, z );
			line->SetId( 5368 );
			line->SetLocation( x, y + 2, static_cast<SI08>( z + 5 ));
			handle->SetId( 16067 );
			handle->SetLocation( x, y - 3, z );
			break;
		case EAST:
			rudder->SetId( 15971 );
			rudder->SetLocation( x - 4, y, z );
			line->SetId( 5368 );
			line->SetLocation( x + 2, y, static_cast<SI08>( z + 5 ));
			handle->SetId( 15970 );
			handle->SetLocation( x - 3, y, z );
			break;
		case WEST:
			rudder->SetId( 15990 );
			rudder->SetLocation( x + 4, y, z );
			line->SetId( 5368 );
			line->SetLocation( x - 2, y, static_cast<SI08>( z + 5 ));
			handle->SetId( 15991 );
			handle->SetLocation( x + 3, y + 1, z );
			break;
		default:
			rudder->SetId( 16062 );
			rudder->SetLocation( x, y + 4, z );
			line->SetId( 5368 );
			line->SetLocation( x, y - 2, static_cast<SI08>( z + 5 ));
			handle->SetId( 16063 );
			handle->SetLocation( x + 1, y + 3, z );
			break;
	}
	rudder->SetName( "rowboat rudder" );
	handle->SetName( "rowboat rudder handle" );
	line->SetName( "rope" );
	hiddenPlank->SetId( 0x0001 );
	hiddenHold->SetId( 0x0001 );
	hiddenPlank->SetName( "internal rowboat component" );
	hiddenHold->SetName( "internal rowboat component" );
	hiddenPlank->SetVisible( VT_PERMHIDDEN );
	hiddenHold->SetVisible( VT_PERMHIDDEN );
	hiddenPlank->SetLocation( boat );
	hiddenHold->SetLocation( boat );
	for( auto *item : { rudder, line, hiddenPlank, hiddenHold, handle })
	{
		item->SetMovable( 2 );
		item->SetDecayable( false );
		item->SetMulti( boat, false );
		item->Update();
	}
	return true;
}

// UO seasonal pumpkin rowboat is multi 0x50-0x53. It has no cargo hold
// or conventional planks: the visible components are a rudder, separate handle,
// and one universally accessible mooring block. UOX still keeps hidden stand-in
// serials for its legacy four-component boat contract.
bool RestorePumpkinBoatFixtures( CBoatObj *boat )
{
	if( !ValidateObject( boat ) || boat->GetTempVar( CITV_MOREZ, 1 ) != 0x50 )
		return false;
	auto *rudder = CalcItemObjFromSer( boat->GetTiller() );
	auto *line = CalcItemObjFromSer( boat->GetPlank( 0 ));
	auto *hiddenPlank = CalcItemObjFromSer( boat->GetPlank( 1 ));
	auto *hiddenHold = CalcItemObjFromSer( boat->GetHold() );
	if( !ValidateObject( rudder ) || !ValidateObject( line ) || !ValidateObject( hiddenPlank ) || !ValidateObject( hiddenHold ))
		return false;

	CItem *handle = nullptr;
	for( const auto serial : boat->GetFixtures() )
	{
		auto *candidate = CalcItemObjFromSer( serial );
		if( ValidateObject( candidate ) && candidate->GetType() == IT_TILLER && candidate->GetSerial() != boat->GetTiller() )
		{
			handle = candidate;
			break;
		}
	}
	if( !ValidateObject( handle ))
	{
		handle = Items->CreateItem( nullptr, nullptr, 16061, 1, 0, OT_ITEM, false, true,
			boat->WorldNumber(), boat->GetInstanceId(), boat->GetX(), boat->GetY(), boat->GetZ() );
		if( !ValidateObject( handle ))
			return false;
		boat->RegisterFixture( handle->GetSerial() );
	}
	// The rudder doubles as the rowboat's tiller. Off the boat its context menu
	// exposes the owner-only dry-dock action; aboard it remains the mouse pilot.
	rudder->AddScriptTrigger( 5101 );
	handle->SetType( IT_TILLER );
	handle->AddScriptTrigger( 5101 );
	handle->SetTempVar( CITV_MOREX, rudder->GetTempVar( CITV_MOREX ));
	// Rowboats have no combat durability in UO. They disappear only through
	// the ordinary boat-decay/sinking lifecycle.
	boat->SetDamageable( false );
	boat->SetHullMaxHits( 0 );
	boat->SetHullHits( 0 );

	const SI16 x = boat->GetX();
	const SI16 y = boat->GetY();
	const SI08 z = boat->GetZ();
	switch( boat->GetDir() & 0x06 )
	{
		case SOUTH:
			rudder->SetId( 42073 ); rudder->SetLocation( x, y - 2, z );
			line->SetId( 42088 ); line->SetLocation( x, y + 1, static_cast<SI08>( z + 2 ));
			handle->SetId( 16067 ); handle->SetLocation( x + 1, y - 1, static_cast<SI08>( z + 9 ));
			break;
		case EAST:
			rudder->SetId( 42058 ); rudder->SetLocation( x - 2, y, z );
			line->SetId( 42087 ); line->SetLocation( x + 1, y, static_cast<SI08>( z + 2 ));
			handle->SetId( 15970 ); handle->SetLocation( x - 1, y + 1, static_cast<SI08>( z + 9 ));
			break;
		case WEST:
			rudder->SetId( 42044 ); rudder->SetLocation( x + 3, y + 1, z );
			line->SetId( 42087 ); line->SetLocation( x - 1, y, static_cast<SI08>( z + 2 ));
			handle->SetId( 15991 ); handle->SetLocation( x + 2, y + 1, static_cast<SI08>( z + 2 ));
			break;
		default:
			rudder->SetId( 42030 ); rudder->SetLocation( x + 1, y + 3, z );
			line->SetId( 42088 ); line->SetLocation( x, y - 1, static_cast<SI08>( z + 2 ));
			handle->SetId( 16061 ); handle->SetLocation( x + 1, y + 2, static_cast<SI08>( z + 2 ));
			break;
	}
	rudder->SetName( "pumpkin rowboat rudder" );
	handle->SetName( "pumpkin rowboat tiller" );
	line->SetName( "mooring block" );
	rudder->SetVisible( VT_VISIBLE );
	line->SetVisible( VT_VISIBLE );
	handle->SetVisible( VT_VISIBLE );
	// Permanent-hidden objects remain visible to staff clients. Replace the
	// legacy plank and hold art with the client no-draw tile as well, otherwise
	// GMs see a floating hatch on a rowboat which has no cargo hold in UO.
	hiddenPlank->SetId( 0x0001 );
	hiddenHold->SetId( 0x0001 );
	hiddenPlank->SetName( "internal rowboat component" );
	hiddenHold->SetName( "internal rowboat component" );
	hiddenPlank->SetVisible( VT_PERMHIDDEN );
	hiddenHold->SetVisible( VT_PERMHIDDEN );
	hiddenPlank->SetLocation( boat );
	hiddenHold->SetLocation( boat );
	for( auto *item : { rudder, line, hiddenPlank, hiddenHold, handle })
	{
		item->SetMovable( 2 );
		item->SetDecayable( false );
		item->SetMulti( boat, false );
		item->Update();
	}
	return true;
}

// UO forcibly releases a galleon pilot on disconnection and death. Keep
// that lifecycle in one native helper so every exit path clears movement and
// the boat-side pilot reference before the virtual mount is removed.
void ReleaseBoatPilot( CChar *pilot )
{
	if( !ValidateObject( pilot ))
		return;

	CItem *pilotMount = pilot->GetItemAtLayer( IL_MOUNT );
	SERIAL capturedBoatSerial = INVALIDSERIAL;
	if( ValidateObject( pilotMount ) && pilotMount->GetId() == 0x3E96 )
		capturedBoatSerial = pilotMount->GetTempVar( CITV_MOREX );
	CMultiObj *multi = CalcMultiFromSer( capturedBoatSerial );
	if( !ValidateObject( multi ))
		multi = pilot->GetMultiObj();
	if( !ValidateObject( multi ))
		multi = FindMulti( pilot );
	if( ValidateObject( multi ) && multi->CanBeObjType( OT_BOAT ))
	{
		auto *boat = static_cast<CBoatObj *>( multi );
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
}

static void RestoreDryDockedCannons( CBoatObj *boat )
{
	if( !ValidateObject( boat )) return;
	const auto cannons = boat->GetDockedCannons();
	boat->ClearDockedCannons();
	for( const auto& record : cannons )
	{
		SI16 dx = record.localX;
		SI16 dy = record.localY;
		const UI08 facing = boat->GetDir() & 0x07;
		if( facing == EAST )
		{
			dx = -record.localY;
			dy = record.localX;
		}
		else if( facing == SOUTH )
		{
			dx = -record.localX;
			dy = -record.localY;
		}
		else if( facing == WEST )
		{
			dx = record.localY;
			dy = -record.localX;
		}
		const SI16 x = boat->GetX() + dx;
		const SI16 y = boat->GetY() + dy;
		const SI08 z = static_cast<SI08>( boat->GetZ() + record.localZ );
		const UI08 power = record.power == 4 ? 4 : ( record.power == 2 ? 2 : 1 );
		const UI16 artOffset = power == 2 ? 4 : 0;
		auto *cannon = Items->CreateItem( nullptr, nullptr, power == 4 ? 41981 : 16920 + artOffset, 1, 0, OT_ITEM, false, true,
			boat->WorldNumber(), boat->GetInstanceId(), x, y, z );
		if( !ValidateObject( cannon ))
		{
			continue;
		}

		cannon->SetName( power == 4 ? "pumpkin cannon" : ( power == 2 ? "heavy ship cannon" : "light ship cannon" ));
		cannon->SetMovable( 2 );
		cannon->SetDecayable( false );
		cannon->SetMulti( boat );
		cannon->AddScriptTrigger( 5099 );
		cannon->SetCannonRole( CannonRole::Cannon );
		cannon->SetCannonPower( static_cast<UI08>( power ));
		cannon->SetCannonStage( 1 );
		cannon->SetCannonDirectionArt( 0, power == 4 ? 41981 : 16920 + artOffset );
		cannon->SetCannonDirectionArt( 1, power == 4 ? 41982 : 16921 + artOffset );
		cannon->SetCannonDirectionArt( 2, power == 4 ? 41979 : 16918 + artOffset );
		cannon->SetCannonDirectionArt( 3, power == 4 ? 41980 : 16919 + artOffset );
		cannon->SetMaxHP( 100 );
		cannon->SetHP( record.hits > 0 ? record.hits : 100 );
		for( auto *pad : boat->GetItemsInMultiList()->collection() )
		{
			if( !ValidateObject( pad ) || pad->GetCannonRole() != CannonRole::WeaponPad || pad->GetX() != x || pad->GetY() != y ) continue;
			cannon->SetCannonLinkSerial( pad->GetSerial() );
			pad->SetCannonLinkSerial( cannon->GetSerial() );
			break;
		}
		if( power == 4 )
		{
			if( facing == NORTH || facing == SOUTH ) cannon->SetId( x < boat->GetX() ? 41980 : ( x > boat->GetX() ? 41982 : ( facing == NORTH ? 41981 : 41979 )));
			else cannon->SetId( y < boat->GetY() ? 41981 : ( y > boat->GetY() ? 41979 : ( facing == EAST ? 41982 : 41980 )));
		}
		else if( facing == NORTH || facing == SOUTH ) cannon->SetId(( x < boat->GetX() ? 16919 : ( x > boat->GetX() ? 16921 : ( facing == NORTH ? 16920 : 16918 ))) + artOffset );
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

SI08 HighSeasBoatDeckZ( const CBoatObj *boat )
{
	if( !ValidateObject( boat ))
	{
		return ILLEGAL_Z;
	}

	SI08 surfaceOffset = 0;
	switch( boat->GetTempVar( CITV_MOREZ, 1 ))
	{
		case 0x3C: surfaceOffset = 3;  break; // rowboat deck
		case 0x18: surfaceOffset = 14; break; // OrcishGalleon.ZSurface
		case 0x24: surfaceOffset = 16; break; // GargishGalleon.ZSurface
		case 0x30: surfaceOffset = 7;  break; // TokunoGalleon.ZSurface
		case 0x40: surfaceOffset = 18; break; // BritannianShip.ZSurface
		case 0x50: surfaceOffset = 3;  break; // pumpkin rowboat deck
		default: return ILLEGAL_Z;
	}
	return static_cast<SI08>( boat->GetZ() + surfaceOffset );
}

bool HighSeasBoatContainsXY( const CBoatObj *boat, SI16 x, SI16 y )
{
	const SI08 deckZ = HighSeasBoatDeckZ( boat );
	if( deckZ == ILLEGAL_Z )
		return false;

	const UI16 multiId = static_cast<UI16>( boat->GetId() - 0x4000 );
	if( !Map->MultiExists( multiId ))
		return false;

	const SI08 deckOffset = static_cast<SI08>( deckZ - boat->GetZ() );
	for( const auto &component : Map->SeekMulti( multiId ).items )
	{
		// Components at or below the main deck describe the physical hull
		// footprint. Excluding higher-only rigging prevents sails from becoming
		// artificial walk/drop surfaces.
		if( component.altitude <= deckOffset && boat->GetX() + component.offsetX == x &&
			boat->GetY() + component.offsetY == y )
			return true;
	}
	return false;
}

static bool HighSeasBoatTravelTileClear( const CBoatObj *boat, SI16 x, SI16 y, SI08 deckZ )
{
	if( !ValidateObject( boat ) || !HighSeasBoatContainsXY( boat, x, y ))
		return false;

	const SI16 deckOffset = static_cast<SI16>( deckZ - boat->GetZ() );
	const UI16 multiId = static_cast<UI16>( boat->GetId() - 0x4000 );
	for( const auto &component : Map->SeekMulti( multiId ).items )
	{
		if( boat->GetX() + component.offsetX != x || boat->GetY() + component.offsetY != y )
			continue;
		const SI16 componentBottom = component.altitude;
		const auto &componentTile = Map->SeekTile( component.tileId );
		const SI16 componentTop = static_cast<SI16>( componentBottom + std::max<SI16>( 1, componentTile.Height() ));
		// A deck surface may finish at the travel Z, but rigging, rails, masts and
		// other components must not occupy the mobile/gate volume above it.
		if(( componentBottom > deckOffset && componentBottom < deckOffset + 20 ) ||
			( componentBottom <= deckOffset && componentTop > deckOffset + 2 ) ||
			( componentBottom >= deckOffset - 2 && componentTile.CheckFlag( TF_BLOCKING )))
			return false;
	}

	for( auto *character : FindNearbyChars( x, y, boat->WorldNumber(), boat->GetInstanceId(), 0 ))
	{
		if( ValidateObject( character ) && character->GetX() == x && character->GetY() == y )
			return false;
	}
	for( auto *item : FindNearbyItems( x, y, boat->WorldNumber(), boat->GetInstanceId(), 0 ))
	{
		if( !ValidateObject( item ) || item == boat || item->GetX() != x || item->GetY() != y )
			continue;
		const auto &itemTile = Map->SeekTile( item->GetId() );
		const SI16 itemBottom = item->GetZ();
		const SI16 itemTop = static_cast<SI16>( itemBottom + std::max<SI16>( 1, itemTile.Height() ));
		const bool namedFixture = item->GetSerial() == boat->GetTiller() || item->GetSerial() == boat->GetHold() ||
			item->GetSerial() == boat->GetPlank( 0 ) || item->GetSerial() == boat->GetPlank( 1 );
		const bool registeredFixture = boat->IsFixture( item->GetSerial() );
		const bool fixtureObstacle = registeredFixture &&
			( namedFixture || item->GetCannonRole() == CannonRole::WeaponPad || itemTile.CheckFlag( TF_BLOCKING ) || itemTile.Height() > 2 );
		if( fixtureObstacle )
			return false;
		const bool floorPiece = registeredFixture && itemTop <= deckZ + 2;
		if( !floorPiece && itemBottom < deckZ + 20 && itemTop > deckZ )
			return false;
	}
	return true;
}

bool GetHighSeasBoatDeckLocation( const CBoatObj *boat, SI16 preferredX, SI16 preferredY, SI16& x, SI16& y, SI08& z )
{
	if( !ValidateObject( boat ))
		return false;
	const SI08 deckZ = HighSeasBoatDeckZ( boat );
	if( deckZ == ILLEGAL_Z )
		return false;

	z = deckZ;
	for( SI16 range = 0; range <= 12; ++range )
	{
		for( SI16 dx = static_cast<SI16>( -range ); dx <= range; ++dx )
		{
			for( SI16 dy = static_cast<SI16>( -range ); dy <= range; ++dy )
			{
				if( range > 0 && std::abs( dx ) != range && std::abs( dy ) != range )
					continue;
				const SI16 candidateX = static_cast<SI16>( preferredX + dx );
				const SI16 candidateY = static_cast<SI16>( preferredY + dy );
				if( HighSeasBoatTravelTileClear( boat, candidateX, candidateY, deckZ ))
				{
					x = candidateX;
					y = candidateY;
					return true;
				}
			}
		}
	}
	return false;
}

bool GetHighSeasBoatRecallLocation( const CBoatObj *boat, SI16& x, SI16& y, SI08& z )
{
	if( !ValidateObject( boat ))
		return false;
	const SI08 deckZ = HighSeasBoatDeckZ( boat );
	if( deckZ == ILLEGAL_Z )
		return false;

	SI16 runeOffset = 0;
	switch( boat->GetTempVar( CITV_MOREZ, 1 ))
	{
		case 0x18: runeOffset = -3; break; // Orcish galleon
		case 0x24: runeOffset = 3;  break; // Gargish galleon
		case 0x30: runeOffset = -3; break; // Tokuno galleon
		case 0x40: runeOffset = -5; break; // Britannian ship
		default: return false;
	}

	x = boat->GetX();
	y = boat->GetY();
	switch( boat->GetDir() & 0x06 )
	{
		case NORTH: y = static_cast<SI16>( y + runeOffset ); break;
		case SOUTH: y = static_cast<SI16>( y - runeOffset ); break;
		case EAST:  x = static_cast<SI16>( x - runeOffset ); break;
		case WEST:  x = static_cast<SI16>( x + runeOffset ); break;
	}
	// Client multi revisions can vary slightly, and the nominal rune offset can
	// overlap a mast or another fixture. Select the nearest clear deck tile to
	// the intended UO location, not the usually occupied ship center.
	const SI16 preferredX = x;
	const SI16 preferredY = y;
	return GetHighSeasBoatDeckLocation( boat, preferredX, preferredY, x, y, z );
}

bool RelocateHighSeasTillerman( CBoatObj *boat, SI16 x, SI16 y, SI08 z )
{
	if( !ValidateObject( boat ) || !HighSeasBoatContainsXY( boat, x, y ))
		return false;
	const SI08 deckZ = HighSeasBoatDeckZ( boat );
	if( deckZ == ILLEGAL_Z || z < boat->GetZ() - 8 || z > deckZ + 24 )
		return false;
	for( auto *character : FindNearbyChars( x, y, boat->WorldNumber(), boat->GetInstanceId(), 0 ))
	{
		if( ValidateObject( character ) && character->GetX() == x && character->GetY() == y )
			return false;
	}
	for( auto *item : FindNearbyItems( x, y, boat->WorldNumber(), boat->GetInstanceId(), 0 ))
	{
		if( !ValidateObject( item ) || item->GetX() != x || item->GetY() != y || item == CalcItemObjFromSer( boat->GetTiller() ))
			continue;
		if( item->GetMultiObj() != boat || !boat->IsFixture( item->GetSerial() ) ||
			item->GetCannonRole() == CannonRole::WeaponPad )
			return false;
	}

	const SI16 dx = static_cast<SI16>( x - boat->GetX() );
	const SI16 dy = static_cast<SI16>( y - boat->GetY() );
	SI16 localX = dx;
	SI16 localY = dy;
	const UI08 facing = boat->GetDir() & 0x06;
	if( facing == EAST )
	{
		localX = dy;
		localY = static_cast<SI16>( -dx );
	}
	else if( facing == SOUTH )
	{
		localX = static_cast<SI16>( -dx );
		localY = static_cast<SI16>( -dy );
	}
	else if( facing == WEST )
	{
		localX = static_cast<SI16>( -dy );
		localY = dx;
	}
	auto *tiller = CalcItemObjFromSer( boat->GetTiller() );
	if( !ValidateObject( tiller ))
		return false;
	boat->SetTillermanOffset( localX, localY );
	// The target Z is a character/static surface height. This fixture is wheel
	// artwork and must retain the lower base altitude from its multi component.
	SI16 wheelArtZ = boat->GetTillermanArtZ();
	if( wheelArtZ == 0 )
		wheelArtZ = static_cast<SI16>( tiller->GetZ() - boat->GetZ() );
	tiller->SetName( "ship wheel" );
	tiller->SetLocation( x, y, static_cast<SI08>( boat->GetZ() + wheelArtZ ), boat->WorldNumber(), boat->GetInstanceId() );
	tiller->SetMulti( boat );
	tiller->Update();
	return true;
}

CBoatObj *FindHighSeasBoatAtXY( SI16 x, SI16 y, UI08 worldNumber, UI16 instanceId )
{
	for( auto *mapArea : MapRegion->PopulateList( x, y, worldNumber ))
	{
		if( mapArea == nullptr )
			continue;
		for( auto *item : mapArea->GetItemList()->collection() )
		{
			if( ValidateObject( item ) && item->GetInstanceId() == instanceId && item->CanBeObjType( OT_BOAT ))
			{
				auto *boat = static_cast<CBoatObj *>( item );
				if( HighSeasBoatContainsXY( boat, x, y ))
					return boat;
			}
		}
	}
	return nullptr;
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
		// A scuttled vessel cannot accept mouse movement, so retaining its
		// virtual pilot mount traps the player at the wheel. UO releases the
		// galleon pilot when control can no longer continue; clear both sides of
		// that relationship as soon as hull damage crosses the threshold.
		auto *pilot = CalcCharObjFromSer( boat->GetPilot() );
		if( ValidateObject( pilot ))
		{
			ReleaseBoatPilot( pilot );
			DismountCreature( pilot );
			pilot->SetOnHorse( false );
			pilot->Update();
			if( auto *pilotSocket = pilot->GetSocket(); pilotSocket != nullptr )
				pilotSocket->SysMessage( "The vessel is too badly damaged to control. You release the wheel." );
		}
		else
		{
			auto *staleMount = CalcItemObjFromSer( boat->GetPilotMount() );
			if( ValidateObject( staleMount ))
				staleMount->Delete();
			boat->SetPilot( INVALIDSERIAL );
			boat->SetPilotMount( INVALIDSERIAL );
			boat->SetMoveTime( 0 );
		}
		boat->Update();
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
		case 0x50: // Pumpkin rowboat
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
	// placed on water at -5 up to Z 0. UO preserves the placement point Z.
	const SI08 z = b->GetZ();
	SI32 hullMaxHits = 0;
	switch( id2 )
	{
		case 0x18: hullMaxHits = 100000; break; // OrcishGalleon.MaxHits
		case 0x24: hullMaxHits = 140000; break; // GargishGalleon.MaxHits
		case 0x30: hullMaxHits = 100000; break; // TokunoGalleon.MaxHits
		case 0x40: hullMaxHits = 200000; break; // BritannianShip.MaxHits
		default: break;
	}
	if( hullMaxHits > 0 )
	{
		b->SetHullMaxHits( hullMaxHits );
		b->SetHullHits( hullMaxHits );
		b->SetDamageable( true );
	}
	b->RefreshBoatDecay();
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

	const bool usesMooringLines = id2 == 0x18 || id2 == 0x24 || id2 == 0x30 || id2 == 0x3C || id2 == 0x40 || id2 == 0x50;
	if( !usesMooringLines )
	{
		// Lock the classic plank.
		TAGMAPOBJECT tagvalObject;
		tagvalObject.m_ObjectType = TAGMAP_TYPE_INT;
		tagvalObject.m_IntValue = 1;
		tagvalObject.m_Destroy = false;
		tagvalObject.m_StringValue = "";
		p2->SetTag( "plankLocked", tagvalObject );
	}

	CItem *p1 = Items->CreateItem( nullptr, mChar, 0x3EB1, 1, 0, OT_ITEM, false, true, worldNumber, instanceId, x, y, z ); // Plank1 is on the LEFT side of the boat
	if( p1 == nullptr )
		return false;

	p1->SetType( IT_PLANK ); // Boat type
	p1->SetDecayable( false );

	if( !usesMooringLines )
	{
		// Lock the classic plank.
		TAGMAPOBJECT tagvalObject;
		tagvalObject.m_ObjectType = TAGMAP_TYPE_INT;
		tagvalObject.m_IntValue = 1;
		tagvalObject.m_Destroy = false;
		tagvalObject.m_StringValue = "";
		p1->SetTag( "plankLocked", tagvalObject );
	}

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
		case 0x50:
			tiller->SetLocation( x + 1, y + 4, z );
			p1->SetLocation( x - 2, y, z );
			p2->SetLocation( x + 2, y, z );
			hold->SetLocation( x, y - 4, z );
			break;
	}
	ConfigureHighSeasFixtures( b, tiller, p1, p2, hold, true );
	RestoreRowboatFixtures( b );
	RestorePumpkinBoatFixtures( b );
	RestoreDryDockedCannons( b );
	if( id2 == 0x18 || id2 == 0x24 || id2 == 0x30 || id2 == 0x3C || id2 == 0x40 )
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
		// A multi can never be cargo aboard another multi. This guard also makes
		// touching/overlapping ships safe if stale world data ever cross-registers
		// one hull in the other's live item list.
		if( !ValidateObject( item ) || item == boat || item->CanBeObjType( OT_MULTI ))
			continue;
		if( item->GetMultiObj() == boat && seen.insert( item->GetSerial() ).second )
			items.push_back( item );
	}
	for( const auto serial : boat->GetFixtures() )
	{
		auto *item = CalcItemObjFromSer( serial );
		if( ValidateObject( item ) && seen.insert( serial ).second )
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
	const UI08 baseId = boat->GetTempVar( CITV_MOREZ, 1 );
	const bool rowBoat = baseId == 0x3C || baseId == 0x50;
	// UO rowboats do not have galleon hull durability. A zero hit pool is
	// therefore pristine for a rowboat, not a damaged-vessel failure.
	if( !rowBoat && ( boat->GetHullMaxHits() <= 0 || boat->GetHullHits() != boat->GetHullMaxHits() )) return 1;
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
			boat->IsFixture( item->GetSerial() )) continue;
		if( item->GetCannonRole() == CannonRole::Cannon )
		{
			if( item->GetCannonStage() > 1 ) return 5;
			continue;
		}
		return 4;
	}
	return 0;
}

bool DeleteHighSeasBoatForDryDock( CBoatObj *boat, CItem *deed )
{
	if( CheckHighSeasDryDock( boat ) != 0 || !ValidateObject( deed )) return false;
	std::vector<CItem *> items;
	std::vector<CChar *> characters;
	CollectBoatEntities( boat, items, characters );
	deed->ClearDockedCannons();
	const UI08 facing = boat->GetDir() & 0x07;
	for( auto *item : items )
	{
		if( !ValidateObject( item ) || item->GetCannonRole() != CannonRole::Cannon )
			continue;
		const SI16 dx = item->GetX() - boat->GetX();
		const SI16 dy = item->GetY() - boat->GetY();
		SI16 localX = dx;
		SI16 localY = dy;
		if( facing == EAST )
		{
			localX = dy;
			localY = -dx;
		}
		else if( facing == SOUTH )
		{
			localX = -dx;
			localY = -dy;
		}
		else if( facing == WEST )
		{
			localX = -dy;
			localY = dx;
		}
		const UI08 storedPower = item->GetCannonPower();
		const UI08 power = storedPower == 4 ? 4 : ( storedPower == 2 ? 2 : 1 );
		deed->AddDockedCannon( localX, localY, static_cast<SI16>( item->GetZ() - boat->GetZ() ),
			item->GetHP() > 0 ? item->GetHP() : 100, power );
	}
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

// Return values are consumed by the paint-item script:
// 1 temporary coat applied, 2 permanent paint applied,
// -1 incompatible existing paint, -2 maximum coats already present.
SI08 PaintHighSeasBoat( CBoatObj *boat, UI16 paintHue, bool permanent )
{
	if( !IsHighSeasHull( boat ) || paintHue == 0 )
		return 0;
	const UI08 coats = boat->GetPaintCoats();
	if( permanent )
	{
		if( coats != 0 )
			return -1;
		boat->ClearPaintState();
		ApplyHighSeasBoatPaintColour( boat, paintHue );
		return 2;
	}

	const UI16 existingPaintHue = boat->GetPaintHue();
	if( coats > 0 && existingPaintHue != paintHue )
		return -1;
	if( coats >= HIGH_SEAS_MAX_PAINT_COATS )
		return -2;
	const UI08 newCoats = static_cast<UI08>( coats + 1 );
	const UI16 baseBoatHue = coats > 0 ? boat->GetPaintBaseBoatHue() : boat->GetColour();
	boat->SetPaintState( baseBoatHue, paintHue, newCoats,
		static_cast<UI64>( std::time( nullptr )) + HighSeasPaintDecaySeconds() );
	ApplyHighSeasBoatPaintColour( boat, PaintedHueForCoats( paintHue, newCoats ));
	return 1;
}

bool RemoveHighSeasBoatPaint( CBoatObj *boat )
{
	if( !IsHighSeasHull( boat ) || boat->GetPaintCoats() == 0 )
		return false;
	const UI16 baseBoatHue = boat->GetPaintBaseBoatHue();
	boat->ClearPaintState();
	ApplyHighSeasBoatPaintColour( boat, baseBoatHue );
	return true;
}

bool ProcessBoatDecay( CBoatObj *boat )
{
	if( !ValidateObject( boat ))
		return false;
	const UI64 now = static_cast<UI64>( std::time( nullptr ));
	ProcessHighSeasPaintDecay( boat, now );
	if( !cwmWorldState->ServerData()->BoatDecay() )
	{
		boat->SetBoatDecayAt( 0 );
		boat->SetNextSinkAt( 0 );
		boat->SetSinkStep( 0 );
		return false;
	}
	if( boat->GetBoatDecayAt() == 0 )
	{
		boat->RefreshBoatDecay();
		return false;
	}

	if( boat->IsSinking() )
	{
		if( now < boat->GetNextSinkAt() )
			return false;

		if( boat->GetSinkStep() > 5 )
		{
			std::vector<CItem *> items;
			std::vector<CChar *> characters;
			CollectBoatEntities( boat, items, characters );
			auto addUnique = [&items]( CItem *item )
			{
				if( ValidateObject( item ) && std::find( items.begin(), items.end(), item ) == items.end() )
					items.push_back( item );
			};
			addUnique( CalcItemObjFromSer( boat->GetTiller() ));
			addUnique( CalcItemObjFromSer( boat->GetPlank( 0 )));
			addUnique( CalcItemObjFromSer( boat->GetPlank( 1 )));
			addUnique( CalcItemObjFromSer( boat->GetHold() ));
			for( const auto fixtureSerial : boat->GetFixtures() )
				addUnique( CalcItemObjFromSer( fixtureSerial ));
			for( auto *item : items )
			{
				if( ValidateObject( item ))
					item->Delete();
			}
			boat->Delete();
			return true;
		}

		std::vector<CItem *> items;
		std::vector<CChar *> characters;
		CollectBoatEntities( boat, items, characters );
		auto addUnique = [&items]( CItem *item )
		{
			if( ValidateObject( item ) && std::find( items.begin(), items.end(), item ) == items.end() )
				items.push_back( item );
		};
		addUnique( CalcItemObjFromSer( boat->GetTiller() ));
		addUnique( CalcItemObjFromSer( boat->GetPlank( 0 )));
		addUnique( CalcItemObjFromSer( boat->GetPlank( 1 )));
		addUnique( CalcItemObjFromSer( boat->GetHold() ));
		for( const auto fixtureSerial : boat->GetFixtures() )
			addUnique( CalcItemObjFromSer( fixtureSerial ));
		boat->SetLocation( boat->GetX(), boat->GetY(), static_cast<SI08>( boat->GetZ() - 1 ));
		for( auto *item : items )
		{
			if( ValidateObject( item ))
			{
				item->SetLocation( item->GetX(), item->GetY(), static_cast<SI08>( item->GetZ() - 1 ));
				item->SetMulti( boat, false );
				item->Update();
			}
		}
		for( auto *character : characters )
		{
			if( ValidateObject( character ))
			{
				character->SetLocation( character->GetX(), character->GetY(), static_cast<SI08>( character->GetZ() - 1 ));
				character->SetMulti( boat, false );
				character->Update();
			}
		}
		boat->RemoveFromSight();
		boat->Update();
		boat->SetSinkStep( static_cast<UI08>( boat->GetSinkStep() + 1 ));
		boat->SetNextSinkAt( now + 5 );
		return false;
	}

	if( now < boat->GetBoatDecayAt() ||
		( boat->GetMoveType() != BOAT_STOP && boat->GetMoveType() != BOAT_ANCHORED ))
		return false;

	std::vector<CItem *> items;
	std::vector<CChar *> characters;
	CollectBoatEntities( boat, items, characters );
	for( auto *character : characters )
	{
		if( ValidateObject( character ) && !character->IsNpc() )
		{
			boat->RefreshBoatDecay();
			return false;
		}
	}

	boat->SetMoveType( BOAT_STOP );
	boat->SetPilotSpeed( 0 );
	boat->SetSinkStep( 1 );
	boat->SetNextSinkAt( now + 1 );
	for( auto *socket : FindNearbyPlayers( boat, DIST_BUILDRANGE ))
	{
		if( socket != nullptr )
			socket->SysMessage( "This ship has begun to sink!" );
	}
	return false;
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
	if( !ValidateObject( boat ))
		return;
	if( boat->IsScuttled() || boat->IsSinking() )
	{
		boat->SetMoveType( BOAT_STOP );
		boat->SetPilotSpeed( 0 );
		return;
	}
	CItem *tiller = CalcItemObjFromSer( boat->GetTiller() );
	CItem *p1 = CalcItemObjFromSer( boat->GetPlank( 0 ));
	CItem *p2 = CalcItemObjFromSer( boat->GetPlank( 1 ));
	CItem *hold = CalcItemObjFromSer( boat->GetHold() );

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
	const SI08 highSeasDeckZ = HighSeasBoatDeckZ( boat );
	if( highSeasDeckZ != ILLEGAL_Z )
	{
		for( auto *boatCharacter : boatCharacters )
		{
			if( ValidateObject( boatCharacter ) && boatCharacter->GetZ() < highSeasDeckZ )
				boatCharacter->SetLocation( boatCharacter->GetX(), boatCharacter->GetY(), highSeasDeckZ );
		}
	}

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
	if( b->IsScuttled() || b->IsSinking() )
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
			if( highSeasHull && bItem->GetCannonRole() == CannonRole::Cannon )
			{
				const SI32 cannonPower = bItem->GetCannonPower();
				const UI16 artOffset = cannonPower == 2 ? 4 : 0;
				const UI16 northArt = bItem->GetCannonDirectionArt( 0 ) ? bItem->GetCannonDirectionArt( 0 ) : ( cannonPower == 4 ? 41981 : 16920 + artOffset );
				const UI16 eastArt = bItem->GetCannonDirectionArt( 1 ) ? bItem->GetCannonDirectionArt( 1 ) : ( cannonPower == 4 ? 41982 : 16921 + artOffset );
				const UI16 southArt = bItem->GetCannonDirectionArt( 2 ) ? bItem->GetCannonDirectionArt( 2 ) : ( cannonPower == 4 ? 41979 : 16918 + artOffset );
				const UI16 westArt = bItem->GetCannonDirectionArt( 3 ) ? bItem->GetCannonDirectionArt( 3 ) : ( cannonPower == 4 ? 41980 : 16919 + artOffset );
				UI16 cannonId = northArt;
				if( b->GetDir() == NORTH || b->GetDir() == SOUTH )
				{
					if( bItem->GetX() < b->GetX() ) cannonId = westArt;
					else if( bItem->GetX() > b->GetX() ) cannonId = eastArt;
					else cannonId = b->GetDir() == NORTH ? northArt : southArt;
				}
				else
				{
					if( bItem->GetY() < b->GetY() ) cannonId = northArt;
					else if( bItem->GetY() > b->GetY() ) cannonId = southArt;
					else cannonId = b->GetDir() == EAST ? eastArt : westArt;
				}
				bItem->SetId( cannonId );
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
			// UO rotates every mobile's facing by the same delta as the
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
		case 0x50:
			p1->IncLocation( iSmallShipOffsets[dir][PORT_PLANK][XP], iSmallShipOffsets[dir][PORT_PLANK][YP] );
			p2->IncLocation( iSmallShipOffsets[dir][STARB_PLANK][XP], iSmallShipOffsets[dir][STARB_PLANK][YP] );
			tiller->IncLocation( iSmallShipOffsets[dir][TILLER][XP], iSmallShipOffsets[dir][TILLER][YP] );
			hold->IncLocation( iSmallShipOffsets[dir][HOLD][XP], iSmallShipOffsets[dir][HOLD][YP] );
			break;
		default: Console.Error( oldstrutil::format( "TurnBoat() more1 error! more1 = %c not found!", b->GetTempVar( CITV_MOREZ, 1 )));
	}
	ConfigureHighSeasFixtures( b, tiller, p1, p2, hold, false );
	RestoreRowboatFixtures( b );
	RestorePumpkinBoatFixtures( b );

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
	const UI08 boatBaseId = boat->GetTempVar( CITV_MOREZ, 1 );
	const bool highSeasShip = boatBaseId == 0x18 || boatBaseId == 0x24 || boatBaseId == 0x30 || boatBaseId == 0x40;
	// High Seas rowboats are controlled exclusively through the mouse-piloting
	// rudder. They do not understand classic tillerman speech or anchor orders.
	if( boatBaseId == 0x3C || boatBaseId == 0x50 )
		return;
	if( highSeasShip && !boat->CanCommand( mChar ))
	{
		mSock->SysMessage( 2034 );
		return;
	}

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
			auto *shipOwner = boat->GetOwnerObj();
			const bool highSeasOwner = mChar->IsGM() || boat->IsOwner( mChar ) ||
				( ValidateObject( shipOwner ) && shipOwner->GetAccountNum() == mChar->GetAccountNum() );
			if(( highSeasShip && !highSeasOwner ) ||
				( !highSeasShip && mChar->GetSerial() != boat->GetOwner() ))
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

		const bool pumpkinBoat = i->GetTempVar( CITV_MOREZ, 1 ) == 0x50;
		const UI32 componentCount = pumpkinBoat ? 5 : 4;
		if( i->GetItemsInMultiList()->Num() > componentCount || i->GetCharsInMultiList()->Num() > 0 )
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
		if( pumpkinBoat )
		{
			for( const auto fixtureSerial : i->GetFixtures() )
			{
				auto *fixture = CalcItemObjFromSer( fixtureSerial );
				if( ValidateObject( fixture ) && fixture->GetType() == IT_TILLER && fixture->GetSerial() != i->GetTiller() )
					fixture->Delete();
			}
		}
		i->Delete();
		KillKeys( serial );
	}
}


