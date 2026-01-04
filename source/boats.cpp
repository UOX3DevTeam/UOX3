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

struct BoatPartCfg
{
	bool hasItemId = false;
	UI16 itemId = 0;

	bool hasId2 = false;
	UI08 id2[4] = { 0,0,0,0 }; // N,E,S,W (this is ID2, not direction)

	bool hasOffset = false;
	SI16 offX[4] = { 0,0,0,0 };
	SI16 offY[4] = { 0,0,0,0 };
};

struct BoatPartsCfg
{
	BoatPartCfg portPlank;   // plank 0 (left)
	BoatPartCfg starbPlank;  // plank 1 (right)
	BoatPartCfg tiller;
	BoatPartCfg hold;
};

static std::map<UI16, BoatPartsCfg> g_BoatPartsCfg; // key = multiID (houseId)

static bool GetBoatPartsConfig( UI16 multiId, BoatPartsCfg &out )
{
	auto it = g_BoatPartsCfg.find( multiId );
	if( it == g_BoatPartsCfg.end() )
		return false;
	out = it->second;
	return true;
}

// New registry function (replaces RegisterBoatTillerConfig)
void RegisterBoatPartsConfig(
	UI16 multiId,
	// Port plank
	bool portHasItemId, UI16 portItemId,
	bool portHasId2, const UI08 portId2[4],
	bool portHasOffset, const SI16 portOffX[4], const SI16 portOffY[4],

	// Starboard plank
	bool starbHasItemId, UI16 starbItemId,
	bool starbHasId2, const UI08 starbId2[4],
	bool starbHasOffset, const SI16 starbOffX[4], const SI16 starbOffY[4],

	// Hold
	bool holdHasItemId, UI16 holdItemId,
	bool holdHasId2, const UI08 holdId2[4],
	bool holdHasOffset, const SI16 holdOffX[4], const SI16 holdOffY[4],

	// Tiller
	bool tillerHasItemId, UI16 tillerItemId,
	bool tillerHasId2, const UI08 tillerId2[4],
	bool tillerHasOffset, const SI16 tillerOffX[4], const SI16 tillerOffY[4]
)
{
	BoatPartsCfg &cfg = g_BoatPartsCfg[multiId];

	auto CopyId2 = []( BoatPartCfg &p, bool has, const UI08 v[4] )
	{
		p.hasId2 = has;
		if( has )
			for( int i = 0; i < 4; ++i ) p.id2[i] = v[i];
	};

	auto CopyOff = []( BoatPartCfg &p, bool has, const SI16 x[4], const SI16 y[4] )
	{
		p.hasOffset = has;
		if( has )
			for( int i = 0; i < 4; ++i ) { p.offX[i] = x[i]; p.offY[i] = y[i]; }
	};

	// ---- port plank ----
	cfg.portPlank.hasItemId = portHasItemId;
	cfg.portPlank.itemId    = portItemId;
	CopyId2( cfg.portPlank, portHasId2, portId2 );
	CopyOff( cfg.portPlank, portHasOffset, portOffX, portOffY );

	// ---- starboard plank ----
	cfg.starbPlank.hasItemId = starbHasItemId;
	cfg.starbPlank.itemId    = starbItemId;
	CopyId2( cfg.starbPlank, starbHasId2, starbId2 );
	CopyOff( cfg.starbPlank, starbHasOffset, starbOffX, starbOffY );

	// ---- hold ----
	cfg.hold.hasItemId = holdHasItemId;
	cfg.hold.itemId    = holdItemId;
	CopyId2( cfg.hold, holdHasId2, holdId2 );
	CopyOff( cfg.hold, holdHasOffset, holdOffX, holdOffY );

	// ---- tiller ----
	cfg.tiller.hasItemId = tillerHasItemId;
	cfg.tiller.itemId    = tillerItemId;
	CopyId2( cfg.tiller, tillerHasId2, tillerId2 );
	CopyOff( cfg.tiller, tillerHasOffset, tillerOffX, tillerOffY );
}

// ---- block registry ----
// keep these private in Boat.cpp
struct BoatBlockRect
{
    SI16 xmin = 0;
    SI16 xmax = 0;
    SI16 ymin = 0;
    SI16 ymax = 0;
};

struct BoatBlockCfg
{
    bool hasNS = false;
    bool hasEW = false;
    BoatBlockRect ns;
    BoatBlockRect ew;
};

static std::map<UI16, BoatBlockCfg> g_BoatBlockCfg;

// IMPORTANT: this must NOT be static if other code needs it
bool GetBoatBlockConfig( UI16 multiId, BoatBlockCfg &out )
{
    auto it = g_BoatBlockCfg.find( multiId );
    if( it == g_BoatBlockCfg.end() )
        return false;
    out = it->second;
    return true;
}

// Called from house.cpp after DFN parse (NO struct in signature)
void RegisterBoatBlockConfig(
    UI16 multiId,
    bool hasNS, SI16 nsXMin, SI16 nsXMax, SI16 nsYMin, SI16 nsYMax,
    bool hasEW, SI16 ewXMin, SI16 ewXMax, SI16 ewYMin, SI16 ewYMax
)
{
    BoatBlockCfg &cfg = g_BoatBlockCfg[multiId];
    cfg.hasNS = hasNS;
    cfg.hasEW = hasEW;

    if( hasNS )
    {
        cfg.ns.xmin = nsXMin; cfg.ns.xmax = nsXMax;
        cfg.ns.ymin = nsYMin; cfg.ns.ymax = nsYMax;
    }
    if( hasEW )
    {
        cfg.ew.xmin = ewXMin; cfg.ew.xmax = ewXMax;
        cfg.ew.ymin = ewYMin; cfg.ew.ymax = ewYMax;
    }
}

// ---- meta registry (ID2 range + fallback size) ----
struct BoatMetaCfg
{
	bool hasId2Range = false;
	UI08 id2Min = 0; // inclusive
	UI08 id2Max = 0; // inclusive (normally id2Min + 3)

	bool hasSizeType = false;
	UI08 sizeType = 0; // 1=small, 2=medium, 3=large (fallback offset table)
};

static std::map<UI16, BoatMetaCfg> g_BoatMetaCfg; // key = multiId (houseId)

static bool GetBoatMetaConfig( UI16 multiId, BoatMetaCfg &out )
{
	auto it = g_BoatMetaCfg.find( multiId );
	if( it == g_BoatMetaCfg.end() )
		return false;
	out = it->second;
	return true;
}

// Called from house.cpp after DFN parse
void RegisterBoatMetaConfig( UI16 multiId, bool hasRange, UI08 id2Min, UI08 id2Max, bool hasSize, UI08 sizeType )
{
	BoatMetaCfg &m = g_BoatMetaCfg[multiId];
	m.hasId2Range = hasRange;
	m.id2Min      = id2Min;
	m.id2Max      = id2Max;
	m.hasSizeType = hasSize;
	m.sizeType    = sizeType;
}

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
		default: 	Console.Warning( oldstrutil::format( "Invalid plank ID2 called! id2=0x%X item=0x%X serial=0x%X name='%s'", p->GetId(2), p->GetId(), p->GetSerial(), p->GetName().c_str() ));
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

	// Pick which direction we should use when deciding NS vs EW rect.
	// - If we're turning, use moveDir (the new direction we're trying to become).
	// - If we're just moving, use boatDir (current direction/orientation).
	const UI08 checkDir = ( turnBoat ? ( moveDir & 0x0F ) : ( boatDir & 0x0F ) );

	// ------------------------------------------------------------
	// DFN override: per-boat blocking rect (new config)
	// Uses checkDir to decide NS vs EW.
	// ------------------------------------------------------------
	bool usedDfnRect = false;

	BoatBlockCfg blkCfg{};
	if( GetBoatBlockConfig( b->GetId(), blkCfg ) )
	{
		const bool isNS =
			( checkDir == NORTHEAST ) || ( checkDir == SOUTHWEST ) ||
			( checkDir == NORTH ) || ( checkDir == SOUTH );

		if( isNS && blkCfg.hasNS )
		{
			x1 = cx + blkCfg.ns.xmin;
			x2 = cx + blkCfg.ns.xmax;
			y1 = cy + blkCfg.ns.ymin;
			y2 = cy + blkCfg.ns.ymax;
			usedDfnRect = true;
		}
		else if( !isNS && blkCfg.hasEW )
		{
			x1 = cx + blkCfg.ew.xmin;
			x2 = cx + blkCfg.ew.xmax;
			y1 = cy + blkCfg.ew.ymin;
			y2 = cy + blkCfg.ew.ymax;
			usedDfnRect = true;
		}
	}

	// ------------------------------------------------------------
	// Old fallback: hardcoded type-based rectangle
	// (only used if DFN did not provide the relevant rect)
	// ------------------------------------------------------------
	if( !usedDfnRect )
	{
		//small = 5,11
		//medium = 5, 13
		//large = 5, 15
		switch( checkDir )
		{
			case NORTHEAST:	// U
			case SOUTHWEST:	// D
			case NORTH:		// N
			case SOUTH:		// S
				switch( boatDir & 0x0F )
				{
					case NORTHEAST:	// U
					case SOUTHWEST:	// D
					case NORTH:		// N
					case SOUTH:		// S
						x1 = cx - 2; //Width of N/S ship as it moves N/S
						x2 = cx + 3; //Width of N/S ship as it moves N/S
						switch( type )
						{
							case 1: y1 = cy - 6; y2 = cy + 6; break; //Length of N/S ship as it moves N/S
							case 2: y1 = cy - 7; y2 = cy + 7; break; //Length of N/S ship as it moves N/S
							case 3: y1 = cy - 8; y2 = cy + 8; break; //Length of N/S ship as it moves N/S
							default: Console.Error( " Fallout of North/South switch() statement in cBoats::BlockBoat()" ); break;
						}
						break;

					case EAST:		// E
					case WEST:		// W
					case SOUTHEAST:	// E
					case NORTHWEST:	// W
						y1 = cy - 2; //Width of E/W ship as it moves N/S
						y2 = cy + 3; //Width of E/W ship as it moves N/S
						switch( type )
						{
							case 1: x1 = cx - 6; x2 = cx + 6; break; //Length of E/W ship as it moves N/S
							case 2: x1 = cx - 7; x2 = cx + 7; break;
							case 3: x1 = cx - 8; x2 = cx + 8; break;
							default: Console.Error( " Fallout of East/West switch() statement in cBoats::BlockBoat()" ); break;
						}
						break;

					default:
						Console.Error( " Fallout of boatDir.switch() statement in cBoats::BlockBoat()" );
						break;
				}
				break;

			case EAST:		// E
			case WEST:		// W
			case SOUTHEAST:	// E
			case NORTHWEST:	// W
				switch( boatDir & 0x0F )
				{
					case EAST:		// E
					case WEST:		// W
					case SOUTHEAST:	// E
					case NORTHWEST:	// W
						y1 = cy - 2; //Width of E/W ship as it moves E/W
						y2 = cy + 3; //Width of E/W ship as it moves E/W
						switch( type )
						{
							case 1: x1 = cx - 6; x2 = cx + 6; break; //Length of E/W ship as it moves E/W
							case 2: x1 = cx - 7; x2 = cx + 7; break;
							case 3: x1 = cx - 8; x2 = cx + 8; break;
							default: Console.Error( " Fallout of East/West switch() statement in cBoats::BlockBoat()" ); break;
						}
						break;

					case NORTHEAST:	// U
					case SOUTHWEST:	// D
					case NORTH:		// N
					case SOUTH:		// S
						x1 = cx - 2; //Width of N/S ship as it moves E/W
						x2 = cx + 3; //Width of N/S ship as it moves E/W
						switch( type )
						{
							case 1: y1 = cy - 6; y2 = cy + 6; break; //Length of N/S ship as it moves E/W
							case 2: y1 = cy - 7; y2 = cy + 7; break;
							case 3: y1 = cy - 8; y2 = cy + 8; break;
							default: Console.Error( " Fallout of North/South switch() statement in cBoats::BlockBoat()" ); break;
						}
						break;

					default:
						Console.Error( " Fallout of boatDir.switch() statement in cBoats::BlockBoat()" );
						break;
				}
				break;

			default:
				return true;
		}
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
				if( map.terrainInfo == nullptr || ( map.altitude >= cz && !map.CheckFlag( TF_WET ) && map.name() != "water" ))
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
	BoatMetaCfg meta{};
	const bool hasMeta = GetBoatMetaConfig( b->GetId(), meta );

	if( hasMeta && meta.hasId2Range )
	{
		// Validate id2 is within the per-boat configured turning range
		if( id2 < meta.id2Min || id2 > meta.id2Max )
		{
			if( s != nullptr )
				s->SysMessage( 6 ); // The deed is broken, please contact a Game Master
			return false;
		}

		// Optional sanity warning (expect 4 consecutive ids)
		if( ( meta.id2Max - meta.id2Min ) != 3 )
		{
			Console.Warning( oldstrutil::format( "Boat multi 0x%X has odd BOAT_ID2 range (%u..%u)", b->GetId(), meta.id2Min, meta.id2Max ) );
		}
	}
	else
	{
		// Backward compatible fallback for legacy boats if DFN meta not present
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
					s->SysMessage( 6 );
				return false;
		}
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

	if( hasMeta && meta.hasId2Range )
	{
		// Store min/max ID2 range for turning logic
		b->SetTempVar( CITV_MOREZ, meta.id2Min, 1 ); // min
		b->SetTempVar( CITV_MOREZ, meta.id2Max, 2 ); // max
	}
	else
	{
		// Legacy behavior
		b->SetTempVar( CITV_MOREZ, CalcSerial( id2, id2 + 3, b->GetTempVar( CITV_MOREZ, 3 ), b->GetTempVar( CITV_MOREZ, 4 ) ) );
	}

	b->SetMoveType( BOAT_ANCHORED );

	CChar *mChar = nullptr;
	if( s != nullptr )
	{
		mChar = s->CurrcharObj();
	}

	BoatPartsCfg cfg;
	const bool hasCfg = GetBoatPartsConfig( b->GetId(), cfg );

	UI16 tillerItemId = 0x3E4E; // default
	if( hasCfg && cfg.tiller.hasItemId && cfg.tiller.itemId > 0 )
		tillerItemId = cfg.tiller.itemId;

	CItem *tiller = Items->CreateItem( nullptr, mChar, tillerItemId, 1, 0, OT_ITEM, false, true, worldNumber, instanceId, x, y, z );
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

	UI16 portPlankItemId  = 0x3EB1; // default
	UI16 starbPlankItemId = 0x3EB2; // default

	if( hasCfg && cfg.portPlank.hasItemId && cfg.portPlank.itemId > 0 )
		portPlankItemId = cfg.portPlank.itemId;

	if( hasCfg && cfg.starbPlank.hasItemId && cfg.starbPlank.itemId > 0 )
		starbPlankItemId = cfg.starbPlank.itemId;

	CItem *p2 = Items->CreateItem( nullptr, mChar, starbPlankItemId, 1, 0, OT_ITEM, false, true, worldNumber, instanceId, x, y, z ); // Plank2 is on the RIGHT side of the boat
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

	CItem *p1 = Items->CreateItem( nullptr, mChar, portPlankItemId, 1, 0, OT_ITEM, false, true, worldNumber, instanceId, x, y, z ); // Plank1 is on the LEFT side of the boat
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

	UI16 holdItemId = 0x3EAE; // default
	if( hasCfg && cfg.hold.hasItemId && cfg.hold.itemId > 0 )
		holdItemId = cfg.hold.itemId;

	CItem *hold = Items->CreateItem( nullptr, mChar, holdItemId, 1, 0, OT_ITEM, false, true, worldNumber, instanceId, x, y, z );
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

	// CreateBoat: initial dir index should come from id2, not b->GetDir()
	int dirIndex = 0; // 0=N,1=E,2=S,3=W

	if( hasMeta && meta.hasId2Range )
		dirIndex = static_cast< int >( ( id2 - meta.id2Min ) & 0x03 );
	else
		dirIndex = static_cast< int >( id2 & 0x03 );

	auto TryPlaceTillerUsingCfgOffsets = [ & ]() -> bool
		{
			BoatPartsCfg cfgLocal;
			if( !GetBoatPartsConfig( b->GetId(), cfgLocal ) || !cfgLocal.tiller.hasOffset )
				return false;

			const SI16 tOffX = cfgLocal.tiller.offX[ dirIndex ];
			const SI16 tOffY = cfgLocal.tiller.offY[ dirIndex ];

			tiller->SetLocation( x + tOffX, y + tOffY, z );
			return true;
		};
	auto TryPlaceHoldUsingCfgOffsets = [ & ]() -> bool
		{
			if( !hasCfg || !cfg.hold.hasOffset )
				return false;

			const SI16 hOffX = cfg.hold.offX[ dirIndex ];
			const SI16 hOffY = cfg.hold.offY[ dirIndex ];

			hold->SetLocation( x + hOffX, y + hOffY, z );
			return true;
		};

	auto TryPlacePortPlankUsingCfgOffsets = [ & ]() -> bool
		{
			if( !hasCfg || !cfg.portPlank.hasOffset )
				return false;

			p1->SetLocation( x + cfg.portPlank.offX[ dirIndex ], y + cfg.portPlank.offY[ dirIndex ], z );
			return true;
		};

	auto TryPlaceStarbPlankUsingCfgOffsets = [ & ]() -> bool
		{
			if( !hasCfg || !cfg.starbPlank.hasOffset )
				return false;

			p2->SetLocation( x + cfg.starbPlank.offX[ dirIndex ], y + cfg.starbPlank.offY[ dirIndex ], z );
			return true;
		};

	if( hasCfg && cfg.tiller.hasId2 )
	{
		tiller->SetId( cfg.tiller.id2[ dirIndex ], 2 );
	}

	if( hasCfg && cfg.hold.hasId2 )
	{
		hold->SetId( cfg.hold.id2[ dirIndex ], 2 );
	}

	if( hasCfg && cfg.portPlank.hasId2 )
		p1->SetId( cfg.portPlank.id2[ dirIndex ], 2 );

	if( hasCfg && cfg.starbPlank.hasId2 )
		p2->SetId( cfg.starbPlank.id2[ dirIndex ], 2 );

	UI08 sizeType = 0;
	if( hasMeta && meta.hasSizeType )
	{
		sizeType = meta.sizeType;
	}
	else
	{
		// Legacy inference (keeps old boats working when BOAT_SIZE isn't defined)
		switch( id2 )
		{
			case 0x00:
			case 0x04: sizeType = 1; break;
			case 0x08:
			case 0x0C: sizeType = 2; break;
			case 0x10:
			case 0x14: sizeType = 3; break;
			default: sizeType = 1; break;
		}
	}

	switch( sizeType )
	{
		case 1: // small
			if( !TryPlaceTillerUsingCfgOffsets() ) tiller->SetLocation( x + 1, y + 4, z );
			if( !TryPlacePortPlankUsingCfgOffsets() ) p1->SetLocation( x - 2, y, z );
			if( !TryPlaceStarbPlankUsingCfgOffsets() ) p2->SetLocation( x + 2, y, z );
			if( !TryPlaceHoldUsingCfgOffsets() ) hold->SetLocation( x, y - 4, z );
			break;

		case 2: // medium
			if( !TryPlaceTillerUsingCfgOffsets() ) tiller->SetLocation( x + 1, y + 5, z );
			if( !TryPlacePortPlankUsingCfgOffsets() ) p1->SetLocation( x - 2, y, z );
			if( !TryPlaceStarbPlankUsingCfgOffsets() ) p2->SetLocation( x + 2, y, z );
			if( !TryPlaceHoldUsingCfgOffsets() ) hold->SetLocation( x, y - 4, z );
			break;

		case 3: // large
			if( !TryPlaceTillerUsingCfgOffsets() ) tiller->SetLocation( x + 1, y + 5, z );
			if( !TryPlacePortPlankUsingCfgOffsets() ) p1->SetLocation( x - 2, y - 1, z );
			if( !TryPlaceStarbPlankUsingCfgOffsets() ) p2->SetLocation( x + 2, y - 1, z );
			if( !TryPlaceHoldUsingCfgOffsets() ) hold->SetLocation( x, y - 5, z );
			break;

		default:
			// Safe default
			if( !TryPlaceTillerUsingCfgOffsets() ) tiller->SetLocation( x + 1, y + 4, z );
			if( !TryPlacePortPlankUsingCfgOffsets() ) p1->SetLocation( x - 2, y, z );
			if( !TryPlaceStarbPlankUsingCfgOffsets() ) p2->SetLocation( x + 2, y, z );
			if( !TryPlaceHoldUsingCfgOffsets() ) hold->SetLocation( x, y - 4, z );
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

	// --- compute boat facing index from current id2, then rotate it ---
	// 0=N,1=E,2=S,3=W (your canonical order)
	BoatMetaCfg meta{};
	const bool hasMeta = GetBoatMetaConfig( b->GetId(), meta );
	// 0=N,1=E,2=S,3=W (canonical)
	UI08 face = 0;

	// Determine facing from current id2
	if( hasMeta && meta.hasId2Range )
	{
		face = ( UI08 ) ( ( ( UI08 ) id2 - meta.id2Min ) & 0x03 );
	}
	else
	{
		// legacy fallback (classic UO boats: id2 low bits encode facing)
		face = ( UI08 ) ( ( UI08 ) id2 & 0x03 );
	}

	// Rotate facing
	face = rightTurn ? ( UI08 ) ( ( face + 1 ) & 3 ) : ( UI08 ) ( ( face + 3 ) & 3 );

	// Set movement direction from face (cardinals only)
	static const UI08 kDirFromFace[ 4 ] = { NORTH, EAST, SOUTH, WEST };
	b->SetDir( kDirFromFace[ face ] );

	// Compute new id2 from face
	if( hasMeta && meta.hasId2Range )
	{
		id2 = ( SI16 ) ( meta.id2Min + face );
	}
	else
	{
		// legacy fallback: preserve higher bits, update only low 2 bits
		id2 = ( SI16 ) ( ( ( UI08 ) id2 & 0xFC ) | face );
	}

	prSend.Mode( 0 );
	if( !disableChecks )
	{ 
		const SI16 oldId2 = id2;

		if( BlockBoat( b, 0, 0, b->GetDir(), olddir, true ) )
		{
			b->SetDir( olddir );
			b->SetId( static_cast< UI08 >( oldId2 ), 2 );

			for( auto& tSock : nearbyChars )
			{
				tSock->Send( &prSend );
				tiller->TextMessage( tSock, 1410, 0, 0x0481 );
			}
			return;
		}
	}

	b->SetId( static_cast<UI08>( id2 ), 2 );//set the id

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

	// 0=N, 1=E, 2=S, 3=W
	//UI08 dir = ( b->GetDir() & 0x0F ) / 2;
	UI08 dir = face;

	p1->SetLocation( b );
	p2->SetLocation( b );
	tiller->SetLocation( b );
	hold->SetLocation( b );

	BoatPartsCfg partsCfg{};
	const bool hasPartsCfg = GetBoatPartsConfig( b->GetId(), partsCfg );

	// Apply base graphics (DFN override if present)
	if( hasPartsCfg && partsCfg.portPlank.hasId2 )
		p1->SetId( partsCfg.portPlank.id2[ dir ], 2 );
	else
		p1->SetId( cShipItems[ dir ][ PORT_P_C ], 2 );

	if( hasPartsCfg && partsCfg.starbPlank.hasId2 )
		p2->SetId( partsCfg.starbPlank.id2[ dir ], 2 );
	else
		p2->SetId( cShipItems[ dir ][ STAR_P_C ], 2 );

	if( hasPartsCfg && partsCfg.tiller.hasId2 )
		tiller->SetId( partsCfg.tiller.id2[ dir ], 2 );
	else
		tiller->SetId( cShipItems[ dir ][ TILLERID ], 2 );

	if( hasPartsCfg && partsCfg.hold.hasId2 )
		hold->SetId( partsCfg.hold.id2[ dir ], 2 );
	else
		hold->SetId( cShipItems[ dir ][ HOLDID ], 2 );

	// Apply per-part DFN offsets (optional), fallback to the hardcoded tables
	auto ApplyPartOffsetOrFallback = [ & ]( CItem* part, const BoatPartCfg& cfg, SI16 fallbackX, SI16 fallbackY )
		{
			if( hasPartsCfg && cfg.hasOffset )
				part->IncLocation( cfg.offX[ dir ], cfg.offY[ dir ] );
			else
				part->IncLocation( fallbackX, fallbackY );
		};

	UI08 sizeType = 0;
	if( hasMeta && meta.hasSizeType )
	{
		sizeType = meta.sizeType;
	}
	else
	{
		// Legacy inference: use the stored min-id2 (what old code used)
		const UI08 legacyMin = static_cast< UI08 >( b->GetTempVar( CITV_MOREZ, 1 ) );
		switch( legacyMin )
		{
			case 0x00:
			case 0x04: sizeType = 1; break;
			case 0x08:
			case 0x0C: sizeType = 2; break;
			case 0x10:
			case 0x14: sizeType = 3; break;
			default: sizeType = 1; break;
		}
	}

	switch( sizeType )
	{
		case 1: // small
			ApplyPartOffsetOrFallback( p1, partsCfg.portPlank, iSmallShipOffsets[ dir ][ PORT_PLANK ][ XP ], iSmallShipOffsets[ dir ][ PORT_PLANK ][ YP ] );
			ApplyPartOffsetOrFallback( p2, partsCfg.starbPlank, iSmallShipOffsets[ dir ][ STARB_PLANK ][ XP ], iSmallShipOffsets[ dir ][ STARB_PLANK ][ YP ] );
			ApplyPartOffsetOrFallback( hold, partsCfg.hold, iSmallShipOffsets[ dir ][ HOLD ][ XP ], iSmallShipOffsets[ dir ][ HOLD ][ YP ] );
			ApplyPartOffsetOrFallback( tiller, partsCfg.tiller, iSmallShipOffsets[ dir ][ TILLER ][ XP ], iSmallShipOffsets[ dir ][ TILLER ][ YP ] );
			break;

		case 2: // medium
			ApplyPartOffsetOrFallback( p1, partsCfg.portPlank, iMediumShipOffsets[ dir ][ PORT_PLANK ][ XP ], iMediumShipOffsets[ dir ][ PORT_PLANK ][ YP ] );
			ApplyPartOffsetOrFallback( p2, partsCfg.starbPlank, iMediumShipOffsets[ dir ][ STARB_PLANK ][ XP ], iMediumShipOffsets[ dir ][ STARB_PLANK ][ YP ] );
			ApplyPartOffsetOrFallback( hold, partsCfg.hold, iMediumShipOffsets[ dir ][ HOLD ][ XP ], iMediumShipOffsets[ dir ][ HOLD ][ YP ] );
			ApplyPartOffsetOrFallback( tiller, partsCfg.tiller, iMediumShipOffsets[ dir ][ TILLER ][ XP ], iMediumShipOffsets[ dir ][ TILLER ][ YP ] );
			break;

		case 3: // large
			ApplyPartOffsetOrFallback( p1, partsCfg.portPlank, iLargeShipOffsets[ dir ][ PORT_PLANK ][ XP ], iLargeShipOffsets[ dir ][ PORT_PLANK ][ YP ] );
			ApplyPartOffsetOrFallback( p2, partsCfg.starbPlank, iLargeShipOffsets[ dir ][ STARB_PLANK ][ XP ], iLargeShipOffsets[ dir ][ STARB_PLANK ][ YP ] );
			ApplyPartOffsetOrFallback( hold, partsCfg.hold, iLargeShipOffsets[ dir ][ HOLD ][ XP ], iLargeShipOffsets[ dir ][ HOLD ][ YP ] );
			ApplyPartOffsetOrFallback( tiller, partsCfg.tiller, iLargeShipOffsets[ dir ][ TILLER ][ XP ], iLargeShipOffsets[ dir ][ TILLER ][ YP ] );
			break;

		default:
			Console.Warning( oldstrutil::format( "TurnBoat(): boat multi 0x%X missing BOAT_SIZE; defaulting small", b->GetId() ) );
			// do small
			ApplyPartOffsetOrFallback( p1, partsCfg.portPlank, iSmallShipOffsets[ dir ][ PORT_PLANK ][ XP ], iSmallShipOffsets[ dir ][ PORT_PLANK ][ YP ] );
			ApplyPartOffsetOrFallback( p2, partsCfg.starbPlank, iSmallShipOffsets[ dir ][ STARB_PLANK ][ XP ], iSmallShipOffsets[ dir ][ STARB_PLANK ][ YP ] );
			ApplyPartOffsetOrFallback( hold, partsCfg.hold, iSmallShipOffsets[ dir ][ HOLD ][ XP ], iSmallShipOffsets[ dir ][ HOLD ][ YP ] );
			ApplyPartOffsetOrFallback( tiller, partsCfg.tiller, iSmallShipOffsets[ dir ][ TILLER ][ XP ], iSmallShipOffsets[ dir ][ TILLER ][ YP ] );
			break;
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

	if( !BlockBoat( myBoat, 0, 0, dir, myBoat->GetDir(), true ))
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

