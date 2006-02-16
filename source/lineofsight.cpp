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

bool MapTileBlocks( CSocket *mSock, staticrecord *stat, line3D LoS, SI16 x1, SI16 y1, SI08 z, SI16 x2, SI16 y2, UI08 worldNum )
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

bool CheckIDs( SI32 typeToCheck, UI16 idToCheck, SI08 z1, SI08 z2 )
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

//o--------------------------------------------------------------------------
//|	Function		-	bool LineOfSight( CSocket *mSock, CChar *mChar, SI16 x2, SI16 y2, SI08 z2, int checkfor )
//|	Date			-	03 July, 2001
//|	Programmer		-	Abaddon
//|	Modified		-	18 March, 2002, sereg
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns true if there is line of sight between src and trg
//o--------------------------------------------------------------------------
bool LineOfSight( CSocket *mSock, CChar *mChar, SI16 koxn, SI16 koym, SI08 koz2, UI08 checkfor )
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
	  
		
		  This whole thing is based on the Pythagorean Theorem.  What I have done is
		  taken the coordinates from both chars and created a right triange with the
		  hypotenuse as the line of sight.  Then by sectioning off side "a" into a number
		  of equal lengths and finding the other sides lengths according to that one,  I 
		  have been able to find the coordinates of the tiles that lie along the line of
		  sight(line "c").  Then these tiles are searched from an item that would block 
		  the line of sight. 
		  
			<----- it WAS based on the P.T., now its based on linear algebra ;) (sereg)
	*/

	const bool blocked = false;
	const bool not_blocked = true;

	if( koxn == -1 && koym == -1 )
		return not_blocked;		// target canceled

	SI16 kox1 = mChar->GetX(), koy1 = mChar->GetY();
	SI08 koz1 = mChar->GetZ();

	if( (kox1 == koxn) && (koy1 == koym) && (koz1 == koz2) )
		return not_blocked;		// if source and target are on the same position

	koz1 += 15; // standard eye height of most bodies

	UI08 worldNumber = mChar->WorldNumber();

	SI32 n = ( koxn - kox1 ), m = ( koym - koy1 ), i = 0;
	SI08 sgn_x = ( kox1 <= koxn ) ? 1 : (-1); // signum for x
	SI08 sgn_y = ( koy1 <= koym ) ? 1 : (-1); // signum for y
	SI08 sgn_z = ( koz1 <= koz2 ) ? 1 : (-1); // signum for z
	if( kox1 == koxn )
		sgn_x = 0;
	if( koy1 == koym )
		sgn_y = 0;
	if( koz1 == koz2 )
		sgn_z = 0;

	line3D lineofsight = line3D( vector3D( kox1, koy1, koz1 ), vector3D( (koxn-kox1), (koym-koy1), (koz2-koz1) ) );
	double rBlah = abs( koxn - kox1 ) * abs( koxn - kox1 ) + abs( koym - koy1 ) * abs( koym - koy1 );
	SI32 distance = (SI32)sqrt(rBlah);

	if( distance > 18 )
		return blocked;

	vector3D collisions[ MAX_COLLISIONS ];

	// initialize array
	for( i = 0; i < (distance * 2); ++i )
		collisions[i] = vector3D( -1, -1, -1 );

	SI32 collisioncount = 0;
	SI32 dz = 0; // dz is needed later for collisions of the ray with floor tiles
	if( sgn_x == 0 || m > n )
		dz = (SI32)floor( lineofsight.dzInDirectionY() );
	else
		dz = (SI32)floor( lineofsight.dzInDirectionX() );

	if( sgn_x == 0 && sgn_y == 0 && sgn_z != 0 ) // should fix shooting through floor issues
	{
		for( i = 0; i < abs( koz2 - koz1 ); ++i )
		{
			collisions[collisioncount] = vector3D( kox1, koy1, koz1 + sgn_z );
			++collisioncount;
		}
	}
	else if( sgn_x == 0 ) // if we are on the same x-level, just push every x/y coordinate in y-direction from src to trg into the array
	{
		for( i = 0; i < (sgn_y * m); ++i )
		{
			collisions[collisioncount] = vector3D( kox1, koy1 + (sgn_y * i), (SI08)(koz1 + floor(lineofsight.dzInDirectionY() * (R32)i)) );
			++collisioncount;
		}
	}
	else if( sgn_y == 0 ) // if we are on the same y-level, just push every x/y coordinate in x-direction from src to trg into the array
	{
		for( i = 0; i < (sgn_x * n); ++i )
		{
			collisions[collisioncount] = vector3D( kox1 + (sgn_x * i), koy1, (SI08)(koz1 + floor(lineofsight.dzInDirectionX() * (R32)i)) );
			++collisioncount;
		}
	}
	else
	{
		for( i = 0; (n >= m) && (i < (sgn_x * n)); ++i )
		{
			line2D toCollide = line2D( vector2D( (R32)( kox1 + (sgn_x * i) ), 0.0f ), vector2D( 0.0f, 1.0f ) );
			vector2D temp = lineofsight.Projection2D().CollideLines2D( toCollide );

			if( ( temp.x != -1 ) && ( temp.y != -1 ) )
			{
				// the next one is somewhat tricky, if the line of sight exactly cuts a coordinate,
				// we just have to take that coordinate...
				if( floor( temp.y ) == temp.y )
				{
					collisions[collisioncount] = ( vector3D( (long)floor( temp.x ), (long)floor( temp.y ), (signed char)(koz1 + floor(lineofsight.dzInDirectionX() * (R32)i)) ) );
					collisioncount += 1;
				}
				// but if not, we have to take BOTH coordinates, which the calculated collision is between!
				else
				{
					collisions[collisioncount] = ( vector3D( (long)floor( temp.x ), (long)floor( temp.y ), (signed char)(koz1 + floor(lineofsight.dzInDirectionX() * (R32)i)) ) );
					collisions[collisioncount+1] = ( vector3D( (long)ceil( temp.x ), (long)ceil( temp.y ), (signed char)(koz1 + floor(lineofsight.dzInDirectionX() * (R32)i)) ) );
					collisioncount += 2;
				}
			}
		}

		for( i = 0; (m > n) && (i < (sgn_y * m)); ++i )
		{
			line2D toCollide = line2D( vector2D( 0.0f, (R32)( koy1 + (sgn_y * i) ) ), vector2D( 1.0f, 0.0f ) );
			vector2D temp = lineofsight.Projection2D().CollideLines2D( toCollide );

			if( ( temp.x != -1 ) && ( temp.y != -1 ) )
			{
				if( floor( temp.x ) == temp.x )
				{
					collisions[collisioncount] = ( vector3D( (long)floor( temp.x ), (long)floor( temp.y ), (signed char)(koz1 + floor(lineofsight.dzInDirectionY() * (R32)i)) ) );
					collisioncount += 1;
				}
				else
				{
					collisions[collisioncount] = ( vector3D( (long)floor( temp.x ), (long)floor( temp.y ), (signed char)(koz1 + floor(lineofsight.dzInDirectionY() * (R32)i)) ) );
					collisions[collisioncount+1] = ( vector3D( (long)ceil( temp.x ), (long)ceil( temp.y ), (signed char)(koz1 + floor(lineofsight.dzInDirectionY() * (R32)i)) ) );
					collisioncount += 2;
				}
			}
		}
	}
	///////////////////////////////////////////////////////////
	/////////////////  These next lines initialize arrays
	/*
	This function has to search the items array a number of times which
	caused a bit of lag.  I made this item cache to be used instead.  
	The items array is only search once for items in a 40 tile area.
	if an item is found, it is put into the cache, which is then used
	by the rest of the function.  This way it doesn't have to check the 
	entire array each time.
	*/
	// - Tauriel's region stuff 3/6/99

	ITEMLIST loscache;
	SI16 x1, y1, x2, y2;
	if( kox1 < koxn )
	{
		x1 = kox1;
		x2 = koxn;
	}
	else
	{
		x1 = koxn;
		x2 = kox1;
	}
	if( koy1 < koym )
	{
		y1 = koy1;
		y2 = koym;
	}
	else
	{
		y1 = koym;
		y2 = koy1;
	}

	SI16 tempX = -1, tempY = -1;
	REGIONLIST nearbyRegions = MapRegion->PopulateList( kox1, koy1, worldNumber );
	for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
	{
		CMapRegion *MapArea = (*rIter);
		if( MapArea == NULL )	// no valid region
			continue;
		CDataList< CItem * > *regItems = MapArea->GetItemList();
		regItems->Push();
		for( CItem *tempItem = regItems->First(); !regItems->Finished(); tempItem = regItems->Next() )
		{
			if( !ValidateObject( tempItem ) )
				continue;

			tempX = tempItem->GetX();
			tempY = tempItem->GetY();
			if( ( tempX >= x1 && tempX <= x2 ) && ( tempY >= y1 && tempY <= y2 ) )
			{
				for( i = 0; i < collisioncount; ++i )
				{
					if( ( tempX == collisions[i].x ) && ( tempY == collisions[i].y ) )
						loscache.push_back( tempItem );
				}
			}
		}
		regItems->Pop();
	}

	////////////End Initilzations
	//////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////
	////////////////  This determines what to check for
	UI08 checkthis[ITEM_TYPE_CHOICES];
	size_t checkthistotal = 0;
	UI08 itemtype = 1;

	while( checkfor )
	{
		if( ( checkfor >= itemtype ) && ( checkfor < ( itemtype * 2 ) ) && ( checkfor ) )
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

	///////////////////////////////////////////////////////////////////////////
	////////////////////  This next stuff is what searches each tile for things
	SI32 length;
	size_t j;
	std::vector< UI16 > itemids;
	for( i = 0; i < collisioncount; ++i )
	{
		MapStaticIterator msi( collisions[i].x, collisions[i].y, worldNumber );
		staticrecord *stat = msi.First();

		// Texture mapping
		if( MapTileBlocks( mSock, stat, lineofsight, collisions[i].x, collisions[i].y, collisions[i].z, collisions[i].x + sgn_x, collisions[i].y + sgn_y, worldNumber ) )
			return blocked;
			
		// Statics
		CTile tile;
		while( stat != NULL )
		{
			msi.GetTile( &tile );
			if(	( collisions[i].z >= stat->zoff && collisions[i].z <= ( stat->zoff + tile.Height() ) ) ||
				( tile.Height() <= 2 && abs( collisions[i].z - stat->zoff ) <= abs( dz ) ) )
			{
				itemids.push_back( stat->itemid );
			}
			stat = msi.Next();
		}

		// Items
		CItem *dyncount;
		for( ITEMLIST_CITERATOR losIter = loscache.begin(); losIter != loscache.end(); ++losIter )
		{
			dyncount = (*losIter);
			if( dyncount == NULL )
				continue;
			if( !dyncount->CanBeObjType( OT_MULTI ) )
			{ // Dynamic items
				Map->SeekTile( dyncount->GetID(), &tile);
				if( ( dyncount->GetX() == collisions[i].x ) && (dyncount->GetY() == collisions[i].y ) &&
					( collisions[i].z >= dyncount->GetZ() ) && ( collisions[i].z <= ( dyncount->GetZ() + tile.Height() ) ) &&
					( dyncount->GetVisible() == VT_VISIBLE ) )
				{
					itemids.push_back( dyncount->GetID() );
				}
			}
			else
			{	// Multi's
				if( ( abs( kox1 - koxn ) <= DIST_BUILDRANGE ) && ( abs( koy1 - koym ) <= DIST_BUILDRANGE ) )
				{
					st_multi *test;
					int multiID = ( dyncount->GetID() ) - 0x4000;
					Map->SeekMulti( multiID, &length );
					if( length == -1 || length >= 17000000 )//Too big... bug fix hopefully (Abaddon 13 Sept 1999)
					{
						Console << "LoS - Bad length in multi file. Avoiding stall" << myendl;
						CLand land;
						const map_st map1 = Map->SeekMap( dyncount->GetX(), dyncount->GetY(), dyncount->WorldNumber() );
						Map->SeekLand( map1.id, &land );
						if( land.LiquidWet() ) // is it water?
							dyncount->SetID( 0x4001 );
						else
							dyncount->SetID( 0x4064 );
						length = 0;
					}
					for( SI32 k = 0; k < length; ++k )
					{
						test = Map->SeekIntoMulti( multiID, k );
						if( ( test->visible ) && ( dyncount->GetX() + test->x == collisions[i].x ) &&
							( dyncount->GetY() + test->y == collisions[i].y ) )
						{
							Map->SeekTile( test->tile, &tile );
							if( ( collisions[i].z >= dyncount->GetZ() + test->z ) &&
								( collisions[i].z <= (dyncount->GetZ() + test->z + tile.Height() ) ) )
							{
								itemids.push_back( test->tile );
							}
						}
					}
				}
			}
		}
	}

	for( std::vector<UI16>::const_iterator iIter = itemids.begin(); iIter != itemids.end(); ++iIter )
	{
		for( j = 0; j < checkthistotal; ++j )
		{
			if( CheckIDs( checkthis[j], (*iIter), koz1, koz2 ) )
				return blocked;
		}
	}
	return not_blocked;
}

}
