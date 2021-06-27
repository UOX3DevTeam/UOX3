#include "uox3.h"
#include "cmath"
#include "regions.h"


#define MAX_COLLISIONS 1024
#define LOSXYMAX		256		// Maximum items UOX can handle on one X/Y square

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

struct vector2D
{
	R32 x;
	R32 y;
	vector2D(): x( 0 ), y( 0 )
	{
	}
	vector2D( R32 X, R32 Y ) : x(X), y(Y)
	{
	}
};

struct vector3D
{
	SI32 x;
	SI32 y;
	SI08 z;
	vector3D(): x( 0 ), y( 0 ), z( 0 )
	{
	}
	vector3D( SI32 X, SI32 Y, SI08 Z ) : x(X), y(Y), z(Z)
	{
	}
};

inline bool operator== (vector3D const &a, vector3D const &b )
{
	return ( ( a.x == b.x ) && ( a.y == b.y ) && ( a.z == b.z ) );
}

inline bool operator< (vector3D const &a, vector3D const &b )
{
	return ( ( a.x < b.x ) && ( a.y < b.y ) && ( a.z < b.z ) );
}

struct line2D
{
	vector2D loc;
	vector2D dir;
	line2D()
	{
	}
	line2D( vector2D LOC, vector2D DIR ) : loc(LOC), dir(DIR)
	{
	}
	vector2D CollideLines2D( line2D toCollide ) const;
};

inline vector2D line2D::CollideLines2D( line2D toCollide ) const
{
	if( ( ( dir.x == 0 ) && ( toCollide.dir.x == 0 ) ) ||
	   ( ( dir.y == 0 ) && ( toCollide.dir.y == 0 ) ) )
		return vector2D( -1.0f, -1.0f ); // error, parallel or invalid lines
	if( ( ( dir.x * toCollide.dir.y ) - ( toCollide.dir.x * dir.y ) ) == 0 )
		return vector2D( -1.0f, -1.0f ); // error, parallel lines

	R32 t = 0.0f; // parameter of toCollide-line
	// linear evaluation of extended 2x2 matrix
	t = ( ( ( ( loc.x - toCollide.loc.x ) * (- dir.y) ) + ( dir.x * ( loc.y - toCollide.loc.y ) ) ) /
		 ( ( dir.x * toCollide.dir.y ) - ( toCollide.dir.x * dir.y ) ) );

	return vector2D( ( toCollide.loc.x + t * toCollide.dir.x ), ( toCollide.loc.y + t * toCollide.dir.y ) );
}

struct line3D
{
	vector3D loc;
	vector3D dir;
	line3D()
	{
	}
	line3D( vector3D LOC, vector3D DIR ) : loc(LOC), dir(DIR)
	{
	}
	R32 dzInDirectionX( void ) const;
	R32 dzInDirectionY( void ) const;
	line2D Projection2D( void ) const;
};

inline R32 line3D::dzInDirectionX( void ) const
{
	if( dir.x == 0 )
		return (R32)( dir.z );
	else
		return (R32)( (R32)( dir.z ) / (R32)( dir.x ) );
}

inline R32 line3D::dzInDirectionY( void ) const
{
	if( dir.y == 0 )
		return (R32)( dir.z );
	else
		return (R32)( (R32)( dir.z ) / (R32)( dir.y ) );
}

inline line2D line3D::Projection2D( void ) const
{
	if( ( dir.x == 0 ) && ( dir.y == 0 ) )
		return line2D( vector2D( -1.0f, -1.0f ), vector2D( -1.0f, -1.0f ) );
	else
		return line2D( vector2D( (R32)loc.x, (R32)loc.y ), vector2D( (R32)dir.x, (R32)dir.y ) );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool MapTileBlocks( CSocket *mSock, Static_st *stat, line3D LoS, SI16 x1, SI16 y1, SI08 z, SI16 x2, SI16 y2, UI08 worldNum, bool showBlockedMsg )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if maptile blocks Line of Sight
//o-----------------------------------------------------------------------------------------------o
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
		   ( stat == nullptr &&															// Ensure there is no static item
			( ( mID1 >= 431  && mID1 <= 432  ) || ( mID1 >= 467  && mID1 <= 475  ) ||
			 (   mID1 >= 543  && mID1 <= 560  ) || ( mID1 >= 1754 && mID1 <= 1757 ) ||
			 (   mID1 >= 1787 && mID1 <= 1789 ) || ( mID1 >= 1821 && mID1 <= 1824 ) ||
			 (   mID1 >= 1851 && mID1 <= 1854 ) || ( mID1 >= 1881 && mID1 <= 1884 ) ) ) )		// 3) Cuts a mountain
		{
			return true;
		}
	}
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CheckFlags( UI08 typeToCheck, CTileUni *toCheck, SI08 startZ, SI08 destZ, bool useSurfaceZ )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Check tiledata flags for tile
//o-----------------------------------------------------------------------------------------------o
bool CheckFlags( UI08 typeToCheck, CTileUni *toCheck, SI08 startZ, SI08 destZ, bool useSurfaceZ )
{
	switch( typeToCheck )
	{
		case TREES_BUSHES: // Trees, Shrubs, bushes - if it's blocking but has neither of the flags listed below, assume it's a tree! :P


			if( toCheck->CheckFlag( TF_FOLIAGE ) || ( (toCheck->CheckFlag( TF_BLOCKING ) &&
													   !toCheck->CheckFlag( TF_WALL ) && !toCheck->CheckFlag( TF_SURFACE ) && !toCheck->CheckFlag( TF_WINDOW )) ||
													 (!toCheck->CheckFlag( TF_CLIMBABLE ) && !toCheck->CheckFlag( TF_WET ) && !toCheck->CheckFlag( TF_ROOF )) ||
													 !toCheck->CheckFlag( TF_CONTAINER )))
				return false;
			break;
		case WALLS_CHIMNEYS: // Walls, Chimneys, ovens, not fences
			if( toCheck->CheckFlag( TF_WALL ) || ( toCheck->CheckFlag( TF_NOSHOOT ) && !toCheck->CheckFlag( TF_SURFACE) ) || toCheck->CheckFlag( TF_WINDOW ))
				return true;
			break;
		case DOORS: // Doors, not gates
			if( toCheck->CheckFlag( TF_DOOR ))
				return true;
			break;
		case ROOFING_SLANTED: // Roofing Slanted
			if( toCheck->CheckFlag( TF_ROOF ))
				return true;
			break;
		case FLOORS_FLAT_ROOFING: // Floors & Flat Roofing (Attacking through floors Roofs)
			if( toCheck->CheckFlag( TF_SURFACE ))
			{
				if( useSurfaceZ ? ( startZ != destZ ) : (( startZ - 15) != destZ ))
				{
					return true;
				}
			}
			break;
		case LAVA_WATER:  // Lava, water
			if( toCheck->CheckFlag( TF_WET ))
				return true;
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 DynamicCanBlock( CItem *toCheck, vector3D *collisions, SI32 collisioncount, SI16 distX, SI16 distY, SI16 x1, SI16 x2, SI16 y1, SI16 y2, SI32 dz )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if dynamic item will block Line of Sight
//o-----------------------------------------------------------------------------------------------o
UI16 DynamicCanBlock( CItem *toCheck, vector3D *collisions, SI32 collisioncount, SI16 distX, SI16 distY, SI16 x1, SI16 x2, SI16 y1, SI16 y2, SI32 dz )
{
	const SI16 curX		= toCheck->GetX();
	const SI16 curY		= toCheck->GetY();
	const SI08 curZ		= toCheck->GetZ();
	SI32 i				= 0;
	vector3D *checkLoc	= nullptr;
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
		SI32 length = 0;
		
		if( !Map->multiExists( multiID ))
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
		else
		{
			for( auto &multi : Map->SeekMulti( multiID ).allItems() )
			{
				
				if( multi.visible )
				{
					const SI16 checkX = (curX + multi.xoffset);
					const SI16 checkY = (curY + multi.yoffset);
					if( checkX >= x1 && checkX <= x2 && checkY >= y1 && checkY <= y2 )
					{
						const SI08 checkZ = (curZ + multi.zoffset);
						CTile& multiTile = Map->SeekTile( multi.tileid );
						for( i = 0; i < collisioncount; ++i )
						{
							checkLoc = &collisions[i];
							if( checkX == checkLoc->x && checkY == checkLoc->y &&
							   ( ( checkLoc->z >= checkZ && checkLoc->z <= (checkZ + multiTile.Height()) ) ||
							    ( multiTile.Height() <= 2 && abs( checkLoc->z - checkZ ) <= dz ) ) )
							{
								return multi.tileid;
							}
						}
					}
				}
			}
		}
	}
	return INVALIDID;
}
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool LineOfSight( CSocket *mSock, CChar *mChar, SI16 destX, SI16 destY, SI08 destZ, UI08 checkfor, bool useSurfaceZ )
//|	Date		-	03 July, 2001
//|	Changes		-	18 March, 2002
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if there is line of sight between src and trg
//|
//|	Notes		-	Char (x1, y1, z1) is the char(pc/npc),  Target (x2, y2, z2) is the target.
//|					s is for pc's, in case a message needs to be sent.
//|					the checkfor is what is checked for along the line of sight.
//|					Look at uox3.h to see options. Works like npc magic.
//|
//|					#define TREES_BUSHES 1 // Trees and other large vegetaion in the way
//|					#define WALLS_CHIMNEYS 2  // Walls, chimineys, ovens, etc... in the way
//|					#define DOORS 4 // Doors in the way
//|					#define ROOFING_SLANTED 8  // So can't tele onto slanted roofs, basically
//|					#define FLOORS_FLAT_ROOFING 16  //  For attacking between floors
//|					#define LAVA_WATER 32  // Don't know what all to use this for yet
//|
//|					Just or (|) the values for the diff things together to get what to search for.
//|					So put in place of the paramater checkfor for example
//|
//|					if( line_of_sight( s, x1, y1, z1, x2, y2, z2, WALLS_CHIMNEYS | DOORS | ROOFING_SLANTED ) )
//|
//|					it WAS based on the P.T., now its based on linear algebra ;)
//o-----------------------------------------------------------------------------------------------o
bool LineOfSight( CSocket *mSock, CChar *mChar, SI16 destX, SI16 destY, SI08 destZ, UI08 checkfor, bool useSurfaceZ )
{
	const bool blocked		= false;
	const bool not_blocked	= true;

	if( destX == -1 && destY == -1 )
		return not_blocked;		// target canceled

	const SI16 startX = mChar->GetX(), startY = mChar->GetY();
	const SI08 startZ = ( useSurfaceZ ? mChar->GetZ() : ( mChar->GetZ() + 15 )); // standard eye height of most bodies if useSurfaceZ is false, use feet height if true

	if( (startX == destX) && (startY == destY) && (startZ == destZ) )
		return not_blocked;		// if source and target are on the same position

	const UI08 worldNumber = mChar->WorldNumber();
	const UI16 instanceID = mChar->GetInstanceID();

	const SI32 distX	= abs( static_cast<SI32>(destX - startX) ), distY = abs( static_cast<SI32>(destY - startY) );
	const SI32 distZ	= abs( static_cast<SI32>(destZ - startZ) );

	line3D lineofsight	= line3D( vector3D( startX, startY, startZ ), vector3D( distX, distY, distZ ) );

	const R64 rBlah		= (static_cast<R64>(distX) * static_cast<R64>(distX)) + (static_cast<R64>(distY) * static_cast<R64>(distY));
	const SI32 distance	= static_cast<SI32>(sqrt( rBlah ));

	if( distance > 18 )
		return blocked;

	// DISABLED; Allows placing items within walls when standing next to them...
	//If target is next to us and within our field of view
	//if( distance <= 1 && destZ <= (startZ + 3) && destZ >= (startZ - 15 ) )
		//return not_blocked;

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
	else if( distX == distY ) // if we're on a perfect diagonal, we can just go up all coords in both x and y at the same time
	{
		for( i = 1; i <= distX; ++i )
		{
			collisions[collisioncount] = vector3D( startX + (sgn_x * i), startY + (sgn_y * i), (SI08)(startZ + (dz * (R32)i * sgn_z)) );
			++collisioncount;
		}
	}
	else
	{
		R32 steps = 0;
		if(distX > distY)
		{
			steps = (R32)distX/(R32)distY;
			if( steps == 0 )
				steps = 1;
			for( i = 1; i < distX; ++i )
			{
				collisions[collisioncount] = vector3D( startX + (sgn_x * i), startY + (sgn_y * (R32)roundNumber(i/steps)), (SI08)(startZ + (dz * (R32)i * sgn_z)) );
				++collisioncount;
			}
		}
		else if(distY > distX)
		{
			steps = (R32)distY/(R32)distX;
			if( steps == 0 )
				steps = 1;
			for( i = 1; i < distY; ++i )
			{
				collisions[collisioncount] = vector3D( startX + (sgn_x * (R32)roundNumber(i/steps)), startY + (sgn_y * i), (SI08)(startZ + (dz * (R32)i * sgn_z)) );
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

	CTileUni losItemList[LOSXYMAX];
	UI16 itemCount		= 0;

	// We already have to run through all the collisions in this function, so lets just check and push the ID rather than coming back to it later.
	REGIONLIST nearbyRegions = MapRegion->PopulateList( startX, startY, worldNumber );
	for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
	{
		CMapRegion *MapArea = (*rIter);
		if( MapArea == nullptr )	// no valid region
			continue;
		GenericList< CItem * > *regItems = MapArea->GetItemList();
		regItems->Push();
		for( CItem *toCheck = regItems->First(); !regItems->Finished(); toCheck = regItems->Next() )
		{
			if( !ValidateObject( toCheck ) || toCheck->GetInstanceID() != instanceID )
				continue;

			// If item toCheck is at the exact same spot as the target location, it should not block LoS.
			if( toCheck->GetX() == destX && toCheck->GetY() == destY && toCheck->GetZ() == destZ )
				continue;

			const UI16 idToPush = DynamicCanBlock( toCheck, collisions, collisioncount, distX, distY, x1, x2, y1, y2, dz );
			if( idToPush != INVALIDID )
			{
				CTile& itemToCheck = Map->SeekTile( idToPush );
				losItemList[itemCount].Height(itemToCheck.Height());
				losItemList[itemCount].SetID( idToPush );
				losItemList[itemCount].Flags( itemToCheck.Flags() );

				++itemCount;
				if( itemCount >= LOSXYMAX )	// don't overflow
					break;
			}
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
		while( stat != nullptr )
		{
			CTile& tile = Map->SeekTile( stat->itemid );
			if(	( checkLoc.z >= stat->zoff && checkLoc.z <= ( stat->zoff + tile.Height() ) ) ||
			   ( tile.Height() <= 2 && abs( checkLoc.z - stat->zoff ) <= dz ) )
			{
				losItemList[itemCount].Height(tile.Height());
				losItemList[itemCount].SetID( stat->itemid );
				losItemList[itemCount].Flags( tile.Flags() );
			}

			++itemCount;
			if( itemCount >= LOSXYMAX )	// don't overflow
				break;
			stat = msi.Next();
		}
	}

	CTileUni *tb;
	size_t j;

	for( i = 0; i < itemCount; ++i )
	{
		for( j = 0; j < checkthistotal; ++j )
		{
			tb = &losItemList[i];
			//if( !mChar->IsGM() && CheckFlags( checkthis[j], tb, startZ, destZ, useSurfaceZ ))
			if( CheckFlags( checkthis[j], tb, startZ, destZ, useSurfaceZ ))
				return blocked;
		}
	}
	return not_blocked;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool checkItemLineOfSight( CChar *mChar, CItem *i)
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if an item is within line of sight
//o-----------------------------------------------------------------------------------------------o
bool checkItemLineOfSight( CChar *mChar, CItem *i )
{
	if( mChar->IsGM() || mChar->IsCounselor() )
		return true;

	CBaseObject *itemOwner	= i;
	bool inSight			= false;

	if( i->GetCont() != nullptr ) // It's inside another container, we need root container to calculate the lof
	{
		ObjectType objType	= OT_CBO;
		CBaseObject *iOwner = FindItemOwner( i, objType );
		if( iOwner != nullptr )
			itemOwner = iOwner;
	}

	if( itemOwner == mChar )
		inSight = true;
	else
	{
		if( mChar->GetInstanceID() != i->GetInstanceID() )
			return false;

		const SI08 height = Map->TileHeight( itemOwner->GetID() ); // Retrieves actual height of item, unrelated to world-coordinate
		// Can we see the top or bottom of the item
		if( LineOfSight( nullptr, mChar, itemOwner->GetX(), itemOwner->GetY(), itemOwner->GetZ(), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false ) )
		{
			inSight = true;
		}
		else if( height > 0 ) // Only bother checking for the top of the item if the item has an actual height value, otherwise it's essentially same check twice
		{
			if( LineOfSight( nullptr, mChar, itemOwner->GetX(), itemOwner->GetY(), (itemOwner->GetZ() + height), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false ) )
			{
				inSight = true;
			}
		}
		if( inSight == false )// If both the previous checks failed, try checking from character's Z location to top of item instead
		{
			if( LineOfSight( nullptr, mChar, itemOwner->GetX(), itemOwner->GetY(), (itemOwner->GetZ() + height), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, true ) )
			{
				inSight = true;
			}
		}
	}

	return inSight;
}
