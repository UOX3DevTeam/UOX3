#include <algorithm>
#include <cmath>

#include "cchar.h"
#include "citem.h"
#include "funcdecl.h"
#include "mapstuff.h"
#include "regions.h"
#include "subsystem/console.hpp"

constexpr auto MAX_COLLISIONS = 1024;
constexpr auto LOSXYMAX = 256; // Maximum items UOX3 can handle on one X/Y square

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
//==================================================================================================
// Structures used
struct Vector2D_st {
    R32 x;
    R32 y;
    Vector2D_st() : x(0.0), y(0.0) {}
    Vector2D_st(R32 X, R32 Y) : x(X), y(Y) {}
};

//==================================================================================================
struct Vector3D_st {
    std::int32_t x;
    std::int32_t y;
    std::int8_t z;
    Vector3D_st() : x(0), y(0), z(0) {}
    Vector3D_st(std::int32_t X, std::int32_t Y, std::int8_t Z) : x(X), y(Y), z(Z) {}
};

//==================================================================================================
inline bool operator==(Vector3D_st const &a, Vector3D_st const &b) {
    return ((a.x == b.x) && (a.y == b.y) && (a.z == b.z));
}

//==================================================================================================
inline bool operator<(Vector3D_st const &a, Vector3D_st const &b) {
    return ((a.x < b.x) && (a.y < b.y) && (a.z < b.z));
}

//==================================================================================================
struct Line2D_st {
    Vector2D_st loc;
    Vector2D_st dir;
    Line2D_st() {}
    Line2D_st(Vector2D_st LOC, Vector2D_st DIR) : loc(LOC), dir(DIR) {}
    auto CollideLines2D(Line2D_st toCollide) const -> Vector2D_st;
};

//==================================================================================================
inline auto Line2D_st::CollideLines2D(Line2D_st toCollide) const -> Vector2D_st {
    if (((dir.x == 0) && (toCollide.dir.x == 0)) || ((dir.y == 0) && (toCollide.dir.y == 0)))
        return Vector2D_st(-1.0f, -1.0f); // error, parallel or invalid lines

    if (((dir.x * toCollide.dir.y) - (toCollide.dir.x * dir.y)) == 0)
        return Vector2D_st(-1.0f, -1.0f); // error, parallel lines

    R32 t = 0.0f; // parameter of toCollide-line
    // linear evaluation of extended 2x2 matrix
    t = ((((loc.x - toCollide.loc.x) * (-dir.y)) + (dir.x * (loc.y - toCollide.loc.y))) /
         ((dir.x * toCollide.dir.y) - (toCollide.dir.x * dir.y)));

    return Vector2D_st((toCollide.loc.x + t * toCollide.dir.x),
                       (toCollide.loc.y + t * toCollide.dir.y));
}

//==================================================================================================
struct Line3D_st {
    Vector3D_st loc;
    Vector3D_st dir;
    Line3D_st() = default;
    Line3D_st(Vector3D_st LOC, Vector3D_st DIR) : loc(LOC), dir(DIR) {}
    auto dzInDirectionX() const -> R32;
    auto dzInDirectionY() const -> R32;
    auto Projection2D() const -> Line2D_st;
};

//==================================================================================================
inline auto Line3D_st::dzInDirectionX() const -> R32 {
    if (dir.x == 0) {
        return static_cast<R32>(dir.z);
    }
    return static_cast<R32>(dir.z) / static_cast<R32>(dir.x);
}

//==================================================================================================
inline auto Line3D_st::dzInDirectionY() const -> R32 {
    if (dir.y == 0) {
        return static_cast<R32>(dir.z);
    }
    return static_cast<R32>(dir.z) / static_cast<R32>(dir.y);
}

//==================================================================================================
inline auto Line3D_st::Projection2D(void) const -> Line2D_st {
    if ((dir.x == 0) && (dir.y == 0)) {
        return Line2D_st(Vector2D_st(-1.0f, -1.0f), Vector2D_st(-1.0f, -1.0f));
    }
    return Line2D_st(Vector2D_st(static_cast<R32>(loc.x), static_cast<R32>(loc.y)),
                     Vector2D_st(static_cast<R32>(dir.x), static_cast<R32>(dir.y)));
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	MapTileBlocks()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if maptile blocks Line of Sight
// o------------------------------------------------------------------------------------------------o
bool MapTileBlocks([[maybe_unused]] CSocket *mSock, bool nostatic, Line3D_st LoS, std::int16_t x1, std::int16_t y1,
                   [[maybe_unused]] std::int8_t z, std::int16_t x2, std::int16_t y2, std::uint8_t worldNum, std::int8_t z2Top) {
    // Map tile at previous coordinate along the LoS path
    auto srcMap = Map->SeekMap(x1, y1, worldNum);

    // Map tile at next coordinate along the LoS path
    auto trgMap = Map->SeekMap(x2, y2, worldNum);

    // Get tileIDs for previous tile in LoS path, and next one
    const std::uint16_t mID1 = srcMap.tileId;
    const std::uint16_t mID2 = trgMap.tileId;

    // Continue if neither of the two tiles is a NoDraw tile, or a cave entrance tile
    if ((mID1 != 2 && mID2 != 2) && (mID1 != 475 && mID2 != 475)) {
        // Get z for previous tile in LoS path, and for next one
        auto mz1 = srcMap.altitude;
        auto mz2 = trgMap.altitude;
        auto startLocZ = LoS.loc.z;

        // Check if LoS intersects with map/mountain walls
        // if(( mz1 < mz2 && z2Top <= mz2 && z >= mz1 ) ||
        // // 1) Collides with a map "wall" ( mz1 > mz2 && z2Top <= mz1 && z >= mz2 ) ||
        /*if((( startLocZ > mz1 && ( z < mz1 && z2Top < mz1 )) || ( startLocZ < mz1 && z > mz1 )) ||
                (( startLocZ > mz2 && ( z < mz2 && z2Top < mz2 )) || ( startLocZ < mz2 && z > mz2 ))
           || ( startLocZ > mz1 && ( z2Top < mz1 )) || ( startLocZ > mz2 && ( z2Top < mz2 )) ||*/
        //( z == mz1 && LoS.dir.z != 0 ) ||
        //// 2) Cuts a map "floor"
        if ((startLocZ > mz2 && z2Top < mz2) || (startLocZ < mz2 && z2Top > mz2) ||
            (startLocZ > mz1 && (mz1 < mz2 && (mz2 > z2Top))) ||
            (nostatic && // Ensure there is no static item
             ((mID1 >= 431 && mID1 <= 432) || (mID1 >= 467 && mID1 <= 474) ||
              (mID1 >= 543 && mID1 <= 560) || (mID1 >= 1754 && mID1 <= 1757) ||
              (mID1 >= 1787 && mID1 <= 1789) || (mID1 >= 1821 && mID1 <= 1824) ||
              (mID1 >= 1851 && mID1 <= 1854) ||
              (mID1 >= 1881 && mID1 <= 1884)))) // 3) Cuts a mountain
        {
            return true;
        }
    }
    return false;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CheckFlags()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check tiledata flags for tile
// o------------------------------------------------------------------------------------------------o
bool CheckFlags(std::uint8_t typeToCheck, Tile_st &toCheck, std::int8_t startZ, std::int8_t destZ, bool useSurfaceZ) {
    switch (typeToCheck) {
    case TREES_BUSHES: // Trees, Shrubs, bushes - if it's blocking but has neither of the flags
                       // listed below, assume it's a tree! :P
        if (toCheck.CheckFlag(TF_FOLIAGE) ||
            ((toCheck.CheckFlag(TF_BLOCKING) && !toCheck.CheckFlag(TF_WALL) &&
              !toCheck.CheckFlag(TF_SURFACE) && !toCheck.CheckFlag(TF_WINDOW)) ||
             (!toCheck.CheckFlag(TF_CLIMBABLE) && !toCheck.CheckFlag(TF_WET) &&
              !toCheck.CheckFlag(TF_ROOF)) ||
             !toCheck.CheckFlag(TF_CONTAINER)))
            return false;
        break;
    case WALLS_CHIMNEYS: // Walls, Chimneys, ovens, not fences
        if (toCheck.CheckFlag(TF_WALL) ||
            (toCheck.CheckFlag(TF_NOSHOOT) && !toCheck.CheckFlag(TF_SURFACE)) ||
            toCheck.CheckFlag(TF_WINDOW))
            return true;
        break;
    case DOORS: // Doors, not gates
        if (toCheck.CheckFlag(TF_DOOR))
            return true;
        break;
    case ROOFING_SLANTED: // Roofing Slanted
        if (toCheck.CheckFlag(TF_ROOF))
            return true;
        break;
    case FLOORS_FLAT_ROOFING: // Floors & Flat Roofing (Attacking through floors Roofs)
        if (toCheck.CheckFlag(TF_SURFACE)) {
            if (useSurfaceZ ? (startZ != destZ) : ((startZ - 15) != destZ)) {
                return true;
            }
        }
        break;
    case LAVA_WATER: // Lava, water
        if (toCheck.CheckFlag(TF_WET))
            return true;
        break;
    default:
        break;
    }
    return false;
}

std::int8_t GetSGN(std::int16_t startLoc, std::int16_t destLoc, std::int16_t &l1, std::int16_t &l2) {
    if (startLoc < destLoc) {
        l1 = startLoc;
        l2 = destLoc;
        return 1;
    }
    else {
        l1 = destLoc;
        l2 = startLoc;
        if (startLoc > destLoc)
            return -1;
    }
    return 0;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	DynamicCanBlock()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if dynamic item will block Line of Sight
// o------------------------------------------------------------------------------------------------o
std::uint16_t DynamicCanBlock(CItem *toCheck, Vector3D_st *collisions, std::int32_t collisioncount, std::int16_t distX,
                     std::int16_t distY, std::int16_t x1, std::int16_t x2, std::int16_t y1, std::int16_t y2, std::int32_t dz) {
    const std::int16_t curX = toCheck->GetX();
    const std::int16_t curY = toCheck->GetY();
    const std::int8_t curZ = toCheck->GetZ();
    std::int32_t i = 0;
    Vector3D_st *checkLoc = nullptr;
    if (!toCheck->CanBeObjType(OT_MULTI)) {
        if (toCheck->GetVisible() == VT_VISIBLE && curX >= x1 && curX <= x2 && curY >= y1 &&
            curY <= y2) {
            CTile &iTile = Map->SeekTile(toCheck->GetId());
            for (i = 0; i < collisioncount; ++i) {
                checkLoc = &collisions[i];
                if (curX == checkLoc->x && curY == checkLoc->y && checkLoc->z >= curZ &&
                    checkLoc->z <= (curZ + iTile.Height()))
                    return toCheck->GetId();
            }
        }
    }
    else if (distX <= DIST_BUILDRANGE && distY <= DIST_BUILDRANGE) {
        const std::uint16_t multiId = static_cast<std::uint16_t>(toCheck->GetId() - 0x4000);
        [[maybe_unused]] std::int32_t length = 0;

        if (!Map->MultiExists(multiId)) {
            Console::shared() << "LoS - Bad length in multi file. Avoiding stall" << myendl;
            auto map1 = Map->SeekMap(curX, curY, toCheck->WorldNumber());

            if (map1.CheckFlag(TF_WET)) // is it water?
            {
                toCheck->SetId(0x4001);
            }
            else {
                toCheck->SetId(0x4064);
            }
            length = 0;
        }
        else {
            for (auto &multi : Map->SeekMulti(multiId).items) {
                if (multi.flag) {
                    const std::int16_t checkX = (curX + multi.offsetX);
                    const std::int16_t checkY = (curY + multi.offsetY);
                    if (checkX >= x1 && checkX <= x2 && checkY >= y1 && checkY <= y2) {
                        const std::int8_t checkZ = (curZ + multi.altitude);
                        CTile &multiTile = Map->SeekTile(multi.tileId);
                        for (i = 0; i < collisioncount; ++i) {
                            checkLoc = &collisions[i];
                            if (checkX == checkLoc->x && checkY == checkLoc->y &&
                                ((checkLoc->z >= checkZ &&
                                  checkLoc->z <= (checkZ + multiTile.Height())) ||
                                 (multiTile.Height() <= 2 && abs(checkLoc->z - checkZ) <= dz))) {
                                return multi.tileId;
                            }
                        }
                    }
                }
            }
        }
    }
    return INVALIDID;
}
// o------------------------------------------------------------------------------------------------o
//|	Function	-	LineOfSight()
//|	Date		-	03 July, 2001
//|	Changes		-	18 March, 2002
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if there is line of sight between src and trg
//|
//|	Notes		-	Char (x1, y1, z1) is the char(pc/npc),  Target (x2, y2, z2) is the
// target. |					s is for pc's, in case a message needs to be sent.
//|					the checkfor is what is checked for along the line of sight.
//|
//|					#define TREES_BUSHES 1 // Trees and other large vegetaion in
//the way
//|					#define WALLS_CHIMNEYS 2  // Walls, chimineys, ovens, etc... in
//the way |					#define DOORS 4 // Doors in the way | #define
// ROOFING_SLANTED 8  // So can't tele onto slanted roofs, basically | #define FLOORS_FLAT_ROOFING
// 16  //  For attacking between floors |					#define LAVA_WATER
//32  // Don't know what all to use this for yet
//|
//|					Just or (|) the values for the diff things together to get what
//to search for. |					So put in place of the paramater checkfor
// for example
//|
//|					if( line_of_sight( s, x1, y1, z1, x2, y2, z2, WALLS_CHIMNEYS
//| DOORS | ROOFING_SLANTED ))
//|
//|					it WAS based on the P.T., now its based on linear algebra;)
// o------------------------------------------------------------------------------------------------o
auto LineOfSight(CSocket *mSock, CChar *mChar, std::int16_t destX, std::int16_t destY, std::int8_t destZ, std::uint8_t checkfor,
                 bool useSurfaceZ, std::int8_t destZTop, bool checkDistance) -> bool {
    const bool blocked = false;
    const bool not_blocked = true;

    if (destX == -1 && destY == -1)
        return not_blocked; // target canceled

    const std::int16_t startX = mChar->GetX(), startY = mChar->GetY();
    const std::int8_t startZ =
        (useSurfaceZ ? mChar->GetZ()
                     : (mChar->GetZ() + 15)); // standard eye height of most bodies if useSurfaceZ
                                              // is false, use feet height if true

    if ((startX == destX) && (startY == destY) && (startZ == destZ))
        return not_blocked; // if source and target are on the same position

    const std::uint8_t worldNumber = mChar->WorldNumber();
    const std::uint16_t instanceId = mChar->GetInstanceId();

    const std::int32_t distX = abs(static_cast<std::int32_t>(destX - startX)),
               distY = abs(static_cast<std::int32_t>(destY - startY));
    const std::int32_t distZ = destZ - startZ; // abs( static_cast<std::int32_t>( destZ - startZ ));

    Line3D_st lineofsight =
        Line3D_st(Vector3D_st(startX, startY, startZ), Vector3D_st(distX, distY, distZ));

    const R64 rBlah = (static_cast<R64>(distX) * static_cast<R64>(distX)) +
                      (static_cast<R64>(distY) * static_cast<R64>(distY));
    const std::int32_t distance = static_cast<std::int32_t>(sqrt(rBlah));

    // Let's provide some leeway based on height of object
    destZTop = destZ + destZTop;

    if (checkDistance && distance > 18)
        return blocked;

    // DISABLED; Allows placing items within walls when standing next to them...
    // If target is next to us and within our field of view
    // if( distance <= 1 && destZ <= (startZ + 3) && destZ >= (startZ - 15 ))
    // return not_blocked;
    if (distance == 0 && destZ <= (startZ + 3) &&
        destZTop >= (useSurfaceZ ? (startZ) : (startZ - 15)))
        return not_blocked;

    // Vector3D_st *collisions = new Vector3D_st[ MAX_COLLISIONS ];
    std::vector<Vector3D_st> vec;
    vec.resize(MAX_COLLISIONS);
    auto collisions = vec.data();

    std::int16_t x1, y1, x2, y2;
    std::int32_t i = 0;
    const std::int8_t sgn_x = GetSGN(startX, destX, x1, x2);
    const std::int8_t sgn_y = GetSGN(startY, destY, y1, y2);
    std::int8_t sgn_z = (startZ < destZ) ? 1 : (-1); // signum for z
    if (startZ == destZ) {
        sgn_z = 0;
    }

    // initialize array
    for (i = 0; i < (distance * 2); ++i) {
        collisions[i] = Vector3D_st(-1, -1, -1);
    }

    std::int32_t collisioncount = 0;
    std::int32_t dz = 0; // dz is needed later for collisions of the ray with floor tiles
    if (sgn_x == 0 || distY > distX) {
        dz = static_cast<std::int32_t>(floor(abs(lineofsight.dzInDirectionY())));
    }
    else {
        dz = static_cast<std::int32_t>(floor(abs(lineofsight.dzInDirectionX())));
    }

    if (sgn_x == 0 && sgn_y == 0 && sgn_z != 0) // should fix shooting through floor issues
    {
        for (i = 1; i <= abs(distZ); ++i) {
            collisions[collisioncount] =
                Vector3D_st(startX, startY, static_cast<std::int8_t>(startZ + (i * sgn_z)));
            ++collisioncount;
        }
    }
    else if (sgn_x == 0) // if we are on the same x-level, just push every x/y coordinate in
                         // y-direction from src to trg into the array
    {
        for (i = 1; i <= distY; ++i) {
            collisions[collisioncount] =
                Vector3D_st(startX, startY + (sgn_y * i),
                            static_cast<std::int8_t>(startZ + (dz * static_cast<R32>(i) * sgn_z)));
            ++collisioncount;
        }
    }
    else if (sgn_y == 0) // if we are on the same y-level, just push every x/y coordinate in
                         // x-direction from src to trg into the array
    {
        for (i = 1; i <= distX; ++i) {
            collisions[collisioncount] =
                Vector3D_st(startX + (sgn_x * i), startY,
                            static_cast<std::int8_t>(startZ + (dz * static_cast<R32>(i) * sgn_z)));
            ++collisioncount;
        }
    }
    else if (distX == distY) // if we're on a perfect diagonal, we can just go up all coords in both
                             // x and y at the same time
    {
        for (i = 1; i <= distX; ++i) {
            collisions[collisioncount] =
                Vector3D_st(startX + (sgn_x * i), startY + (sgn_y * i),
                            static_cast<std::int8_t>(startZ + (dz * static_cast<R32>(i) * sgn_z)));
            ++collisioncount;
        }
    }
    else {
        R32 steps = 0;
        if (distX > distY) {
            steps = (R32)distX / (R32)distY;
            if (steps == 0) {
                steps = 1;
            }
            for (i = 1; i < distX; ++i) {
                collisions[collisioncount] =
                    Vector3D_st(startX + (sgn_x * i),
                                startY + (sgn_y * static_cast<R32>(RoundNumber(i / steps))),
                                static_cast<std::int8_t>(startZ + (dz * static_cast<R32>(i) * sgn_z)));
                ++collisioncount;
            }
        }
        else if (distY > distX) {
            steps = static_cast<R32>(distY) / static_cast<R32>(distX);
            if (steps == 0) {
                steps = 1;
            }
            for (i = 1; i < distY; ++i) {
                collisions[collisioncount] =
                    Vector3D_st(startX + (sgn_x * static_cast<R32>(RoundNumber(i / steps))),
                                startY + (sgn_y * i),
                                static_cast<std::int8_t>(startZ + (dz * static_cast<R32>(i) * sgn_z)));
                ++collisioncount;
            }
        }
    }

    ////////////////////////////////////////////////////////
    ////////////////  This determines what to check for
    std::uint8_t checkthis[ITEM_TYPE_CHOICES];
    size_t checkthistotal = 0;
    std::uint8_t itemtype = 1;

    while (checkfor) {
        if (checkfor >= itemtype && checkfor < (itemtype * 2) && checkfor) {
            checkthis[checkthistotal] = itemtype;
            checkfor = (checkfor - itemtype);
            ++checkthistotal;
            itemtype = 1;
            if (checkthistotal == ITEM_TYPE_CHOICES)
                break;
        }
        else if (checkfor) {
            itemtype *= 2;
        }
    }

    std::vector<Tile_st> losItemList;
    std::uint16_t itemCount = 0;

    // We already have to run through all the collisions in this function, so lets just check and
    // push the ID rather than coming back to it later.
    for (auto &MapArea : MapRegion->PopulateList(startX, startY, worldNumber)) {
        if (MapArea == nullptr)
            continue;

        auto regItems = MapArea->GetItemList();
        for (const auto &toCheck : regItems->collection()) {
            if (!ValidateObject(toCheck) || toCheck->GetInstanceId() != instanceId)
                continue;

            // If item toCheck is at the exact same spot as the target location, it should not block
            // LoS.
            if (toCheck->GetX() == destX && toCheck->GetY() == destY && toCheck->GetZ() == destZ)
                continue;

            const std::uint16_t idToPush = DynamicCanBlock(toCheck, collisions, collisioncount, distX, distY,
                                                  x1, x2, y1, y2, dz);
            if (idToPush != INVALIDID) {
                auto tile = Tile_st(tiletype_t::art);
                tile.artInfo = &Map->SeekTile(idToPush);
                tile.tileId = idToPush;

                losItemList.push_back(tile);

                ++itemCount;
                if (itemCount >= LOSXYMAX) // don't overflow
                    break;
            }
        }
    }

    for (i = 0; i < collisioncount; ++i) {
        Vector3D_st &checkLoc = collisions[i];
        auto artwork = Map->ArtAt(checkLoc.x, checkLoc.y, worldNumber);

        // Texture mapping
        if (checkLoc.x == destX && checkLoc.y == destY) {
            // Don't overshoot. We don't care about height of tile BEHIND our target
            if (MapTileBlocks(mSock, artwork.empty(), lineofsight, checkLoc.x, checkLoc.y,
                              checkLoc.z, checkLoc.x, checkLoc.y, worldNumber, destZTop)) {
                // delete[] collisions;
                return blocked;
            }
        }
        else {
            // Check next tile along the LoS path
            if (MapTileBlocks(mSock, artwork.empty(), lineofsight, checkLoc.x, checkLoc.y,
                              checkLoc.z, checkLoc.x + sgn_x, checkLoc.y + sgn_y, worldNumber,
                              destZTop)) {
                // delete[] collisions;
                return blocked;
            }
        }

        // Statics
        for (auto &tile : artwork) {
            if ((checkLoc.z >= tile.altitude && checkLoc.z <= (tile.altitude + tile.height())) ||
                (tile.height() <= 2 && std::abs(checkLoc.z - tile.altitude) <= dz)) {
                losItemList.push_back(tile);
            }

            ++itemCount;
            if (itemCount >= LOSXYMAX) // don't overflow
                break;
        }
    }

    size_t j;

    for (auto &tile : losItemList) {
        for (j = 0; j < checkthistotal; ++j) {
            // if( !mChar->IsGM() && CheckFlags( checkthis[j], tb, startZ, destZ, useSurfaceZ ))
            if (CheckFlags(checkthis[j], tile, startZ, destZ, useSurfaceZ)) {
                // delete[] collisions;
                return blocked;
            }
        }
    }
    // delete[] collisions;
    return not_blocked;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CheckItemLineOfSight()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if an item is within line of sight
// o------------------------------------------------------------------------------------------------o
bool CheckItemLineOfSight(CChar *mChar, CItem *i) {
    if (mChar->IsGM() || mChar->IsCounselor())
        return true;

    CBaseObject *itemOwner = i;
    bool inSight = false;

    if (i->GetCont() !=
        nullptr) // It's inside another container, we need root container to calculate the lof
    {
        ObjectType objType = OT_CBO;
        CBaseObject *iOwner = FindItemOwner(i, objType);
        if (iOwner != nullptr) {
            itemOwner = iOwner;
        }
    }

    if (itemOwner == mChar) {
        inSight = true;
    }
    else {
        if (mChar->GetInstanceId() != itemOwner->GetInstanceId())
            return false;

        const std::int8_t height = Map->TileHeight(
            itemOwner->GetId()); // Retrieves actual height of item, unrelated to world-coordinate
        // Can we see the top or bottom of the item
        if (LineOfSight(nullptr, mChar, itemOwner->GetX(), itemOwner->GetY(), itemOwner->GetZ(),
                        WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false)) {
            inSight = true;
        }
        else if (height > 0) // Only bother checking for the top of the item if the item has an
                             // actual height value, otherwise it's essentially same check twice
        {
            if (LineOfSight(nullptr, mChar, itemOwner->GetX(), itemOwner->GetY(),
                            (itemOwner->GetZ() + height),
                            WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false)) {
                inSight = true;
            }
        }
        if (inSight == false) // If both the previous checks failed, try checking from character's Z
                              // location to top of item instead
        {
            if (LineOfSight(nullptr, mChar, itemOwner->GetX(), itemOwner->GetY(),
                            (itemOwner->GetZ() + height),
                            WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, true)) {
                inSight = true;
            }
        }
    }

    return inSight;
}
