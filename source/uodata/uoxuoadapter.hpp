//

#ifndef uoxuoadapter_hpp
#define uoxuoadapter_hpp

#include <cstdint>
#include <iostream>
#include <string>
#include <tuple>
#include <utility>
#include "uoflag.hpp"

class CItem ;
class CMultiObj ;
//======================================================================
//======================================================================
// UOX3 requires
constexpr auto MAX_Z_STEP = std::uint8_t(9);
constexpr auto MAX_Z_FALL = std::uint8_t(20);

namespace uo {
    
    auto validArtTile(std::uint16_t tileid)  ->bool ;
    // Returns which dynamic tile is present at (x,y) or -1 if no tile exists  (why isnt this part of region?)
    auto dynamicTile(int x,int y, int z, int worldNumber, int instanceId, bool checkOnlyMultis, bool checkOnlyNonMultis ) -> CItem *;
    // Return the height of a multi item at the given x, y.  (replaced MultiHeight)
    auto heightOfMulti(CItem *i, int x, int y, int oldZ, int maxZ, bool checkHeight = false) -> int ;
    // Returns ID of tile in multi at specified coordinates (replaced MultiTile)
    auto retrieveMultiTile(CItem *i, int x, int y, int oldz, bool checkVisible) -> std::uint16_t ;
    // Checks if statics at/above given coordinates blocks movement, etc
    auto doesStaticBlock(int x, int y, int z, int worldNumber,bool checkWater = false) -> bool ;
    // Checks if there are any dynamic tiles at given coordinates that block movement (why isnt this part of region?)
    auto doesDynamicBlock(int x, int y, int z, int worldNumber,int instanceId, bool checkWater, bool waterWalk,bool checkOnlyMultis, bool checkOnlyNonMultis) -> bool;
    // Checks if there are any characters at given coordinates that block movement (why isnt this part of region?)
    auto doesCharacterBlock(int x, int y, int z, int worldNumber,int instanceId) ->bool ;
    // Checks if the terrain tile at the given coordinates block movement
    auto doesTerrainBlock(int x, int y, int z,int worldNumber,bool checkWater, bool waterWalk, bool checkMultiPlacement,bool checkForRoad) -> bool ;
    // Checks to see whether any statics at given coordinates has a specific flag
    auto checkStaticFlag(int x, int y, int z, int worldNumber, uo::flag_t toCheck,std::uint16_t &foundtileID,bool checkSpawnSurface ) ->bool ;
    // Checks to see whether any dynamics at given coordinates has a specific flag
    auto checkDynamicFlag(int x, int y, int z, int worldNumber,int instanceId, uo::flag_t toCheck, std::uint16_t &foundTileId) -> bool;
    
    
    // Top of statics at/above given coordinates
    auto staticTop(int x, int y, int z, int worldNumber,int maxZ ) -> int ;
    // Returns the map elevation at given coordinates
    auto mapElevation(int x, int y, int worldNumber) -> int ;
    // This return the *elevation* of dynamic items at/above given coordinates
    auto dynamicElevation(int x, int y, int z, int worldNumber,int instanceId, int maxZ) -> int ;
    // Returns new height of player who walked to X/Y but from OLDZ
    auto playerElevation(int x, int y, int z, int worldNumber, int instanceId) -> int ;
    // Returns whether give coordinates are inside a building by checking if there is a multi or static above them
    auto inBuilding(int x, int y, int z, int worldNumber, int instanceId) ->bool ;
    
    auto multiCorners(const CMultiObj *i)->std::tuple<int,int,int,int>;
    // Checks if location at given coordinates is considered valid for spawning objects
    // Also used to verify teleport location for NPCs teleporting back to bounding box
    auto validSpawnLocation(int x, int y, int z, int worldNumber,int instanceId, bool checkWater = true) -> bool;
    // Checks whether given location is valid for house/boat placement
    auto validMultiLocation(int x, int y, int z, int worldNumber,int instanceId, bool checkWater, bool checkOnlyOtherMultis, bool checkOnlyNonMultis, bool checkForRoads) -> int;
}

#endif /* uoxuoadapter_hpp */
