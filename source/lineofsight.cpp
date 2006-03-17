#include "uox3.h"
#include "cmath"
#include "regions.h"

namespace UOX
{

#define MAX_COLLISIONS 1024

// Contains all current line of sight functionality

/*

  PROPOSED 4 pass Line of Sight algorithm that will take into account current issues.
  Note that it may actually be more suited to a client than server.

Line of Sight algorithm

4 pass algorithm
Pass 1:		Map
Pass 2:		Statics
Pass 3:		Dynamic Items
Pass 4:		Multis		// this pass is often ignored, as multis are rare

Failure at any pass returns false

Flags:

TREES and BUSHES
WALLS_CHIMNEYS
DOORS
ROOFING_SLANTED
FLOORS_FLAT_ROOFING
LAVA_WATER
NO_MAP

ONLY if all tests run through to their conclusion will you ever return a true value

General algorithm:

	// Different Xs
	Find dY
	Find dZ
	// Different Ys
	Find dX
	Find dZ
	// Same X/Y
	// Special case


	// Only pass 1 and 2 are different based on each of these cases
Different Xs algorithm
	
PASS 1
	if map check
		For each x between src and trg
			Increment Y by dY
			Increment Z by dZ
			Get map tile at x/y
			If no overlap z, discard
			elseif flag no match, discard
			else continue
		next x

PASS 2
	For each x between src and trg
		Increment Y by dY
		Increment Z by dZ
		Get static tile at x/y
		If no overlap z, discard
		elseif flag no match, discard
		else continue
	next x

Different Ys algorithm
PASS 1
	if map check
		for each y between src and trg
			Increment X by dX
			Increment Z by dZ
			Get map tile at x/y
			if no overlap z, discard
			elseif flag no match, discard
			else continue
		next y
PASS 2
	for each y between src and trg
		Increment X by dX
		Increment Z by dZ
		Get static tile at x/y
		if no overlap z, discard
		elseif flag no match, discard
		else continue
	next y
Different Zs algorithm
PASS 1
	if map check
		Find tile at x/y
		if z is not between min z and max z, discard
		if flag overlap, blocked
PASS 2
	for each static tile at x/y
		if z is not between min z and max z, discard
		if flag overlap, blocked	
	next tile
PASS 3
	For each item in the same map area
		if item.X < min X, discard
		if item.Y < min Y, discard
		if item.Z < min Z, discard
		if item.X > max X, discard
		if item.Y > min Y, discard
		if item.Z > min Z, discard
		Get item flags
		if no flag overlap, discard
		Construct 6 quads of the item
		Push quads onto list
	Next item
	For each quad in list
		Do intersect test of ray and quad
		if intersect, return false
	next quad

PASS 4
	For each item in the multi
		if item.X < min X, discard
		if item.Y < min Y, discard
		if item.Z < min Z, discard
		if item.X > max X, discard
		if item.Y > min Y, discard
		if item.Z > min Z, discard
		Get item flags
		if no flag overlap, discard
		Construct 6 quads of the item
		Push quads onto list
	Next item
	for each quad in list
		Do intersect test of ray and quad
		if intersect, return false
	next quad

QUAD POLYGON CONSTRUCTION ALGORITHM
	Need to create 6 polys
	One polygon per side of rectangular prism
	Need to use x/y offset
	Also need to use height of tile from tiledata.mul
*/

bool MapTileBlocks( CSocket *mSock, Static_st *stat, line3D LoS, SI16 x1, SI16 y1, SI08 z, SI16 x2, SI16 y2, UI08 worldNum )
{
	const map_st srcMap = Map->SeekMap( x1, y1, worldNum );
	const map_st trgMap = Map->SeekMap( x2, y2, worldNum );

	const UI16 mID1		= srcMap.id;
	const UI16 mID2		= trgMap.id;
	if( mID1 != 2 && mID2 != 2 )
	{
		const SI16 mz1	= srcMap.z;
		const SI16 mz2	= trgMap.z;
		// Mountain walls
		if( ( mz1 < mz2 && z <= mz2 && z >= mz1 ) ||									// 1) Collides with a map "wall"
			( mz1 > mz2 && z <= mz1 && z >= mz2 ) ||
			( z == mz1 && LoS.dir.z != 0 ) ||											// 2) Cuts a map "floor"
			( stat == NULL &&															// Ensure there is no static item
			( ( mID1 >= 431  && mID1 <= 432  ) || ( mID1 >= 467  && mID1 <= 475  ) ||
			(   mID1 >= 543  && mID1 <= 560  ) || ( mID1 >= 1754 && mID1 <= 1757 ) ||
			(   mID1 >= 1787 && mID1 <= 1789 ) || ( mID1 >= 1821 && mID1 <= 1824 ) ||
			(   mID1 >= 1851 && mID1 <= 1854 ) || ( mID1 >= 1881 && mID1 <= 1884 ) ) ) )		// 3) Cuts a mountain
		{
			if( mSock != NULL )
				mSock->sysmessage( 683 );
			return true;
		}
	}
	return false;
}

bool CheckIDs( UI08 typeToCheck, UI16 idToCheck, SI08 z1, SI08 z2 )
{
	switch( typeToCheck )
	{
		case TREES_BUSHES: // Trees, Shrubs, bushes
			if( idToCheck == 3240 || idToCheck == 3242 ||( idToCheck >= 3215 && idToCheck <= 3218 ) ||
				( idToCheck >= 3272 && idToCheck <= 3280 ) || idToCheck == 3283 || idToCheck == 3286 ||
				idToCheck == 3288 || idToCheck == 3290 || idToCheck == 3293 || idToCheck == 3296 ||
				idToCheck == 3299 || idToCheck == 3302 || idToCheck == 3305 || idToCheck == 3306 ||
				idToCheck == 3320 || idToCheck == 3323 || idToCheck == 3326 || idToCheck == 3329 ||
				idToCheck == 3381 || idToCheck == 3383 || idToCheck == 3384 || idToCheck == 3394 ||
				idToCheck == 3395 || ( idToCheck >= 3416 && idToCheck <= 3418 ) ||
				idToCheck == 3440 || idToCheck == 3461 || idToCheck == 3476 || idToCheck == 3480 ||
				idToCheck == 3484 || idToCheck == 3488 || idToCheck == 3492 || idToCheck == 3496 ||
				idToCheck == 3512 || idToCheck == 3513 || ( idToCheck >= 4792 && idToCheck <= 4795 ) )
			{
				return true;
			}
			break;
		case WALLS_CHIMNEYS: // Walls, Chimneys, ovens, not fences
			if( ( idToCheck >= 6 && idToCheck <= 748 ) || ( idToCheck >= 761 && idToCheck <= 881 ) ||
				( idToCheck >= 895 && idToCheck <= 1006 ) || ( idToCheck >= 1057 && idToCheck <= 1061 ) ||
				idToCheck == 1072 || idToCheck == 1073 || ( idToCheck >= 1080 && idToCheck <= 1166 ) ||
				( idToCheck >= 2347 && idToCheck <= 2412 ) || ( idToCheck >= 16114 && idToCheck <= 16134 ) ||
				( idToCheck >= 8538 && idToCheck <= 8553 ) || ( idToCheck >= 9535 && idToCheck <= 9555 ) ||
				idToCheck == 12583 ||
				( idToCheck >= 1801 && idToCheck <= 2000 ) ) //stairs
			{
				return true;
			}
			break;
		case DOORS: // Doors, not gates
			if( ( idToCheck >= 1653 && idToCheck <= 1782 ) || ( idToCheck >= 8173 && idToCheck <= 8188 ) )
			{
				return true;
			}
			break;
		case ROOFING_SLANTED: // Roofing Slanted
			if( ( idToCheck >= 1414 && idToCheck <= 1578 ) || ( idToCheck >= 1587 && idToCheck <= 1590 ) ||
				( idToCheck >= 1608 && idToCheck <= 1617 ) || ( idToCheck >= 1630 && idToCheck <= 1652 ) ||
				( idToCheck >= 1789 && idToCheck <= 1792 ) )
			{
				return true;
			}
			break;
		case FLOORS_FLAT_ROOFING: // Floors & Flat Roofing (Attacking through floors Roofs)
			if( ( idToCheck >= 1169 && idToCheck <= 1413 ) || ( idToCheck >= 1508 && idToCheck <= 1514 ) ||
				( idToCheck >= 1579 && idToCheck <= 1586 ) || ( idToCheck >= 1591 && idToCheck <= 1598 ) )
			{
				if( (z1 - 15) != z2 ) // in case of char and target on same roof
					return true;
			}
			break;
		case LAVA_WATER:  // Lava, water
			if( ( idToCheck >= 4846 && idToCheck <= 4941 ) || ( idToCheck >= 6038 && idToCheck <= 6066 ) ||
				( idToCheck >= 12934 && idToCheck <= 12977 ) || ( idToCheck >= 13371 && idToCheck <= 13420 ) ||
				( idToCheck >= 13422 && idToCheck <= 13638 ) || ( idToCheck >= 13639 && idToCheck <= 13665 ) )
			{
				return true;
			}
			break;
		default:
			break;
	}
	return false;
}

SI08 GetSGN( SI16 startLoc, SI16 destLoc, SI16 &l1, SI16 &l2 )
{
	if( startLoc < destLoc )
	{
		l1			= startLoc;
		l2			= destLoc;
		return 1;
	}
	else
	{
		l1			= destLoc;
		l2			= startLoc;
		if( startLoc > destLoc )
			return -1;
	}
	return 0;
}

UI16 DynamicCanBlock( CItem *toCheck, vector3D *collisions, SI32 collisioncount, SI16 distX, SI16 distY, SI16 x1, SI16 x2, SI16 y1, SI16 y2 )
{
	const SI16 curX		= toCheck->GetX();
	const SI16 curY		= toCheck->GetY();
	const SI08 curZ		= toCheck->GetZ();
	SI32 i				= 0;
	vector3D *checkLoc	= NULL;
	if( !toCheck->CanBeObjType( OT_MULTI ) )
	{
		if( toCheck->GetVisible() == VT_VISIBLE && curX >= x1 && curX <= x2 && curY >= y1 && curY <= y2 )
		{
			CTile& iTile = Map->SeekTile( toCheck->GetID() );
			for( i = 0; i < collisioncount; ++i )
			{
				checkLoc = &collisions[i];
				if( curX == checkLoc->x && curY == checkLoc->y && checkLoc->z >= curZ && checkLoc->z <= (curZ + iTile.Height()) )
					return toCheck->GetID();
			}
		}
	}
	else if( distX <= DIST_BUILDRANGE && distY <= DIST_BUILDRANGE )
	{
		const UI16 multiID = static_cast<UI16>(toCheck->GetID() - 0x4000);
		SI32 length = Map->SeekMulti( multiID );
		if( length == -1 || length >= 17000000 )//Too big... bug fix hopefully (Abaddon 13 Sept 1999)
		{
			Console << "LoS - Bad length in multi file. Avoiding stall" << myendl;
			const map_st map1 = Map->SeekMap( curX, curY, toCheck->WorldNumber() );
			CLand& land = Map->SeekLand( map1.id );
			if( land.CheckFlag( TF_WET ) ) // is it water?
				toCheck->SetID( 0x4001 );
			else
				toCheck->SetID( 0x4064 );
			length = 0;
		}

		for( SI32 k = 0; k < length; ++k )
		{
			Multi_st& multi = Map->SeekIntoMulti( multiID, k );
			if( multi.visible )
			{
				const SI16 checkX = (curX + multi.x);
				const SI16 checkY = (curY + multi.y);
				if( checkX >= x1 && checkX <= x2 && checkY >= y1 && checkY <= y2 )
				{
					const SI08 checkZ = (curZ + multi.z);
					CTile& multiTile = Map->SeekTile( multi.tile );
					for( i = 0; i < collisioncount; ++i )
					{
						checkLoc = &collisions[i];
						if( checkX == checkLoc->x && checkY == checkLoc->y &&
							checkLoc->z >= checkZ && checkLoc->z <= (checkZ + multiTile.Height()) )
						{
								return multi.tile;
						}
					}
				}
			}
		}
	}
	return INVALIDID;
}
//o--------------------------------------------------------------------------
//|	Function		-	bool LineOfSight( CSocket *mSock, CChar *mChar, SI16 x2, SI16 y2, SI08 z2, int checkfor )
//|	Date			-	03 July, 2001
//|	Programmer		-	Abaddon
//|	Modified		-	18 March, 2002, sereg
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns true if there is line of sight between src and trg
//o--------------------------------------------------------------------------
bool LineOfSight( CSocket *mSock, CChar *mChar, SI16 destX, SI16 destY, SI08 destZ, UI08 checkfor )
{
/*
Char (x1, y1, z1) is the char(pc/npc),  Target (x2, y2, z2) is the target.
s is for pc's, in case a message needs to be sent.
the checkfor is what is checked for along the line of sight.  
Look at uox3.h to see options. Works like npc magic.

	#define TREES_BUSHES 1 // Trees and other large vegetaion in the way
	#define WALLS_CHIMNEYS 2  // Walls, chimineys, ovens, etc... in the way
	#define DOORS 4 // Doors in the way
	#define ROOFING_SLANTED 8  // So can't tele onto slanted roofs, basically
	#define FLOORS_FLAT_ROOFING 16  //  For attacking between floors
	#define LAVA_WATER 32  // Don't know what all to use this for yet
  
	Just or (|) the values for the diff things together to get what to search for.
	So put in place of the paramater checkfor for example
	
	if( line_of_sight( s, x1, y1, z1, x2, y2, z2, WALLS_CHIMNEYS | DOORS | ROOFING_SLANTED ) )

	it WAS based on the P.T., now its based on linear algebra ;) (sereg)
*/
	const bool blocked		= false;
	const bool not_blocked	= true;

	if( destX == -1 && destY == -1 )
		return not_blocked;		// target canceled

	const SI16 startX = mChar->GetX(), startY = mChar->GetY();
	const SI08 startZ = mChar->GetZ() + 15; // standard eye height of most bodies

	if( (startX == destX) && (startY == destY) && (startZ == destZ) )
		return not_blocked;		// if source and target are on the same position

	const UI08 worldNumber = mChar->WorldNumber();

	const SI32 distX	= abs( static_cast<SI32>(destX - startX) ), distY = abs( static_cast<SI32>(destY - startY) );
	const SI32 distZ	= abs( static_cast<SI32>(destZ - startZ) );

	line3D lineofsight	= line3D( vector3D( startX, startY, startZ ), vector3D( distX, distY, distZ ) );
	const R64 rBlah		= (distX * distX) + (distY * distY);
	const SI32 distance	= static_cast<SI32>(sqrt( rBlah ));

	if( distance > 18 )
		return blocked;

	//If target is next to us and within our field of view
	if( distance <= 1 && destZ <= (startZ + 3) && destZ >= (startZ - 15 ) )
		return not_blocked;

	vector3D collisions[ MAX_COLLISIONS ];
	SI16 x1, y1, x2, y2;
	SI32 i				= 0;
	const SI08 sgn_x	= GetSGN( startX, destX, x1, x2 );
	const SI08 sgn_y	= GetSGN( startY, destY, y1, y2 );
	SI08 sgn_z			= ( startZ < destZ ) ? 1 : (-1); // signum for z
	if( startZ == destZ )
		sgn_z = 0;

	// initialize array
	for( i = 0; i < (distance * 2); ++i )
		collisions[i] = vector3D( -1, -1, -1 );

	SI32 collisioncount = 0;
	SI32 dz = 0; // dz is needed later for collisions of the ray with floor tiles
	if( sgn_x == 0 || distY > distX )
		dz = (SI32)floor( abs( lineofsight.dzInDirectionY() ) );
	else
		dz = (SI32)floor( abs( lineofsight.dzInDirectionX() ) );

	if( sgn_x == 0 && sgn_y == 0 && sgn_z != 0 ) // should fix shooting through floor issues
	{
		for( i = 1; i <= distZ; ++i )
		{
			collisions[collisioncount] = vector3D( startX, startY, (SI08)(startZ + (i * sgn_z)) );
			++collisioncount;
		}
	}
	else if( sgn_x == 0 ) // if we are on the same x-level, just push every x/y coordinate in y-direction from src to trg into the array
	{
		for( i = 1; i <= distY; ++i )
		{
			collisions[collisioncount] = vector3D( startX, startY + (sgn_y * i), (SI08)(startZ + (dz * (R32)i * sgn_z)) );
			++collisioncount;
		}
	}
	else if( sgn_y == 0 ) // if we are on the same y-level, just push every x/y coordinate in x-direction from src to trg into the array
	{
		for( i = 1; i <= distX; ++i )
		{
			collisions[collisioncount] = vector3D( startX + (sgn_x * i), startY, (SI08)(startZ + (dz * (R32)i * sgn_z)) );
			++collisioncount;
		}
	}
	else if( distX >= distY )
	{
		for( i = 1; i < distX; ++i )
		{
			line2D toCollide = line2D( vector2D( (R32)( startX + (sgn_x * i) ), 0.0f ), vector2D( 0.0f, 1.0f ) );
			vector2D temp = lineofsight.Projection2D().CollideLines2D( toCollide );

			if( ( temp.x != -1 ) && ( temp.y != -1 ) )
			{
				// the next one is somewhat tricky, if the line of sight exactly cuts a coordinate,
				// we just have to take that coordinate...
				collisions[collisioncount] = ( vector3D( (R32)roundNumber( temp.x ), (R32)roundNumber( temp.y ), (SI08)(startZ + (dz * (R32)i * sgn_z))) );
				// but if not, we have to take BOTH coordinates, which the calculated collision is between!
				if( roundNumber( temp.y ) != temp.y )
					collisions[++collisioncount] = ( vector3D( (R32)roundNumber( temp.x )+sgn_x, (R32)roundNumber( temp.y )+sgn_y, (SI08)(startZ + (dz * (R32)i * sgn_z))) );

				++collisioncount;
			}
		}
	}
	else
	{
		for( i = 1; i < distY; ++i )
		{
			line2D toCollide = line2D( vector2D( 0.0f, (R32)( startY + (sgn_y * i) ) ), vector2D( 1.0f, 0.0f ) );
			vector2D temp = lineofsight.Projection2D().CollideLines2D( toCollide );

			if( ( temp.x != -1 ) && ( temp.y != -1 ) )
			{
				collisions[collisioncount] = ( vector3D( (R32)roundNumber( temp.x ), (R32)roundNumber( temp.y ), (SI08)(startZ + (dz * (R32)i * sgn_z))) );
				if( roundNumber( temp.x ) != temp.x )
					collisions[++collisioncount] = ( vector3D( (R32)roundNumber( temp.x )+sgn_x, (R32)roundNumber( temp.y )+sgn_y, (SI08)(startZ + (dz * (R32)i * sgn_z))) );

				++collisioncount;
			}
		}
	}

	////////////////////////////////////////////////////////
	////////////////  This determines what to check for
	UI08 checkthis[ITEM_TYPE_CHOICES];
	size_t checkthistotal = 0;
	UI08 itemtype = 1;

	while( checkfor )
	{
		if( checkfor >= itemtype && checkfor < ( itemtype * 2 ) && checkfor )
		{
			checkthis[checkthistotal] = itemtype;
			checkfor = (checkfor - itemtype);
			++checkthistotal;
			itemtype = 1;
			if( checkthistotal == ITEM_TYPE_CHOICES )
				break;
		}
		else if( checkfor )
			itemtype *= 2;
	}

	std::vector< UI16 > itemids;
	itemids.resize( 0 );

	// We already have to run through all the collisions in this function, so lets just check and push the ID rather than coming back to it later.
	REGIONLIST nearbyRegions = MapRegion->PopulateList( startX, startY, worldNumber );
	for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
	{
		CMapRegion *MapArea = (*rIter);
		if( MapArea == NULL )	// no valid region
			continue;
		CDataList< CItem * > *regItems = MapArea->GetItemList();
		regItems->Push();
		for( CItem *toCheck = regItems->First(); !regItems->Finished(); toCheck = regItems->Next() )
		{
			if( !ValidateObject( toCheck ) )
				continue;

			const UI16 idToPush = DynamicCanBlock( toCheck, collisions, collisioncount, distX, distY, x1, x2, y1, y2 );
			if( idToPush != INVALIDID )
				itemids.push_back( idToPush );
		}
		regItems->Pop();
	}

	for( i = 0; i < collisioncount; ++i )
	{
		vector3D& checkLoc = collisions[i];

		CStaticIterator msi( checkLoc.x, checkLoc.y, worldNumber );
		Static_st *stat = msi.First();

		// Texture mapping
		if( MapTileBlocks( mSock, stat, lineofsight, checkLoc.x, checkLoc.y, checkLoc.z, checkLoc.x + sgn_x, checkLoc.y + sgn_y, worldNumber ) )
			return blocked;
			
		// Statics
		while( stat != NULL )
		{
			CTile& tile = Map->SeekTile( stat->itemid );
			if(	( checkLoc.z >= stat->zoff && checkLoc.z <= ( stat->zoff + tile.Height() ) ) ||
				( tile.Height() <= 2 && abs( checkLoc.z - stat->zoff ) <= dz ) )
			{
				itemids.push_back( stat->itemid );
			}
			stat = msi.Next();
		}
	}

	size_t j;
	for( std::vector<UI16>::const_iterator iIter = itemids.begin(); iIter != itemids.end(); ++iIter )
	{
		for( j = 0; j < checkthistotal; ++j )
		{
			if( CheckIDs( checkthis[j], (*iIter), startZ, destZ ) )
				return blocked;
		}
	}
	return not_blocked;
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool checkItemLineOfSight( CChar *mChar, CItem *i)
//|	Programmer	-	grimson
//o---------------------------------------------------------------------------o
//|	Purpose		-	Checks if an item is within line of sight
//o---------------------------------------------------------------------------o
bool checkItemLineOfSight( CChar *mChar, CItem *i )
{
	if( mChar->IsGM() || mChar->IsCounselor() )
		return true;

	CBaseObject *itemOwner	= i;
	bool inSight			= false;

	if( i->GetCont() != NULL ) // It's inside another container, we need root container to calculate the lof
	{
		ObjectType objType	= OT_CBO;
		CBaseObject *iOwner = FindItemOwner( i, objType );
		if( iOwner != NULL )
			itemOwner = iOwner;
	}
	
	if( itemOwner == mChar )
		inSight = true;
	else
	{
		const SI08 height = Map->TileHeight( itemOwner->GetID() );
		// Can we see the top or bottom of the item
		if( LineOfSight( NULL, mChar, itemOwner->GetX(), itemOwner->GetY(), (itemOwner->GetZ() + height), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING ) ||
			LineOfSight( NULL, mChar, itemOwner->GetX(), itemOwner->GetY(), itemOwner->GetZ(), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING ) )
			inSight = true;
	}

	return inSight;
}
}
