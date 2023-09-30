//

#include "uoxuoadapter.hpp"

#include <algorithm>
#include <stdexcept>

#include "cchar.h"
#include "citem.h"
#include "cmultiobj.h"
#include "subsystem/console.hpp"
#include "funcdecl.h"
#include "multicollection.hpp"
#include "regions.h"
#include "utility/strutil.hpp"
#include "townregion.h"
#include "typedefs.h"

#include "uomgr.hpp"
#include "uotile.hpp"


using namespace std::string_literals ;

//======================================================================
extern uo::UOMgr uoManager ;
extern CMapHandler worldMapHandler ;  // We need this for dynmaic items

namespace uo {
    
    
    //======================================================================
    auto validArtTile(std::uint16_t tileid)  ->bool {
        return (tileid != INVALIDID && (tileid < uoManager.sizeArt())) ;
    }
    
    //======================================================================
    auto dynamicTile(int x,int y, int z, int worldNumber, int instanceId, bool checkOnlyMultis, bool checkOnlyNonMultis ) -> CItem *{
        auto rvalue = static_cast<CItem*>(nullptr) ;
        for (const auto &cellResponse:worldMapHandler.PopulateList(x,y,worldNumber)){
            if (cellResponse != nullptr){
                auto regItems = cellResponse->GetItemList();
                for (const auto &item : regItems->collection()){
                    if (ValidateObject(item) && item->GetInstanceId() == instanceId){
                        
                        if (!checkOnlyNonMultis){
                            if (item->GetId(1) >= 0x40 && (item->GetObjType() == CBaseObject::OT_MULTI || item->CanBeObjType(CBaseObject::OT_MULTI))) {
                                auto multiTileId = retrieveMultiTile(item, x, y, z, false);
                                if (multiTileId > 0) {
                                    rvalue = item;
                                    break;
                                }
                            }
                            else if (item->GetMulti() != INVALIDSERIAL || ValidateObject(CalcMultiFromSer(item->GetOwner()))) {
                                if ((item->GetX() == x) && (item->GetY() == y)) {
                                    rvalue = item;
                                    break;
                                }
                            }
                        }
                        else if (!checkOnlyMultis && (item->GetX() == x) && (item->GetY() == y)) {
                            rvalue = item;
                            break;
                        }
                    }
                }
                if (rvalue) {
                    break;
                }
            }
        }
        return rvalue ;
    }
    //======================================================================
    auto heightOfMulti(CItem *i, int x, int y, int oldZ, int maxZ, bool checkHeight) -> int {
        auto mHeight = static_cast<int>(ILLEGAL_Z);
        auto tmpTop = static_cast<int>(ILLEGAL_Z);
        auto multiId = static_cast<std::uint16_t>(i->GetId() - 0x4000);
        if (!uoManager.multiExists(multiId)){
            return mHeight ;
        }
        auto baseX = i->GetX() ;
        auto baseY = i->GetY() ;
        auto baseZ = i->GetZ() ;
        for (const auto &entry:uoManager.multiFor(multiId).tiles){
            if ( (checkHeight || entry.flag.value) && ((baseX + entry.xoffset) == x) && ((baseY + entry.yoffset) == y)) {
                tmpTop = baseZ + entry.zoffset ;
                if (checkHeight){
                    // return the height of the highest point of multi;
                    if ( (tmpTop <= oldZ + maxZ) && tmpTop > oldZ && tmpTop > mHeight) {
                        mHeight = tmpTop ;
                    }
                }
                else {
                    if (std::abs(tmpTop - oldZ) <= maxZ){
                        mHeight = tmpTop + entry.info->climbHeight(true) ;
                        if (mHeight == oldZ) {
                            // We found a surface at the suggested height
                            break;
                        }
                    }
                }
            }
        }
        return mHeight ;
    }
    
    //======================================================================
    auto retrieveMultiTile(CItem *i, int x, int y, int oldz, bool checkVisible) -> std::uint16_t {
        auto rvalue = std::uint16_t(0);
        if (!i->CanBeObjType(CBaseObject::OT_MULTI))
            return rvalue;
        auto multiId = static_cast<std::uint16_t>(i->GetId() - 0x4000); ;
        if (!uoManager.multiExists(multiId)){
            Console::shared() << util::format("Multi does not exist in uo data: %04x, creating one (0x0001/0x0064)",multiId) <<myendl ;
            if (uoManager.terrainTileAt(i->WorldNumber(), i->GetX(), i->GetY()).checkFlag(flag_t::WET)) {
                i->SetId(0x4001);
            }
            else {
                i->SetId(0x4064);
            }
        }
        else {
            // Loop through each item that makes up the multi
            // If any of those items intersect with area were are checking, return the ID of that tile
            for (const auto &multi:uoManager.multiFor(multiId).tiles){
                if ((i->GetX() + multi.xoffset == x) && (i->GetY() + multi.yoffset == y)){
                    if (!checkVisible) {
                        rvalue = multi.tileid ;
                        break;
                    }
                    else if ( multi.flag.value > 0 && (std::abs(i->GetZ() + multi.zoffset - oldz) <= 1)){
                        rvalue = multi.tileid ;
                        break;
                    }
                }
            }
        }
        return rvalue ;
        
    }
    
    //======================================================================
    auto doesStaticBlock(int x, int y, int z, int worldNumber,bool checkWater ) -> bool {
        auto rvalue = false ;
        for (const auto &tile:uoManager.artTileAt(worldNumber, x, y)){
            auto elevation = static_cast<int>(tile.climbHeight() + tile.altitude );
            if (checkWater) {
                if (elevation >= z && tile.altitude <=z && (tile.checkFlag(flag_t::BLOCKING) ||tile.checkFlag(flag_t::WET) )){
                    rvalue = true ;
                    break;
                }
            }
            else {
                if (elevation >= z && tile.altitude <=z && (tile.checkFlag(flag_t::BLOCKING) ||!tile.checkFlag(flag_t::WET) )){
                    rvalue = true ;
                    break;
                }
            }
        }
        return rvalue ;
    }
    
    //======================================================================
    auto doesDynamicBlock(int x, int y, int z, int worldNumber,int instanceId, bool checkWater, bool waterWalk,bool checkOnlyMultis, bool checkOnlyNonMultis) -> bool{
        auto rvalue = false ;
        auto dynItem = dynamicTile(x, y, z, worldNumber, instanceId, checkOnlyMultis, checkOnlyNonMultis) ;
        if (ValidateObject(dynItem)){
            // Don't allow placing houses on top of immovable, visible dynamic items
            if (dynItem->GetMovable() == 2 && dynItem->GetVisible() == 0){
                return true ;
            }
            // Ignore items that are permanently invisible or visible to GMs only
            if (dynItem->GetVisible() == 1 || dynItem->GetVisible() == 3){
                return false ;
            }
            auto tileid = dynItem->GetId();
            if (validArtTile(tileid) && tileid != 0){
                const auto &info = uoManager.art(tileid);
                if (waterWalk){
                    if (info.checkFlag(flag_t::WET)) {
                        rvalue = true ;
                    }
                }
                else {
                    if (info.checkFlag(flag_t::ROOF) || info.checkFlag(flag_t::BLOCKING) || (checkWater && info.checkFlag(flag_t::WET))){
                        rvalue = true ;
                    }
                }
            }
        }
        return rvalue ;
    }
    
    //======================================================================
    auto doesCharacterBlock(int x, int y, int z, int worldNumber,int instanceId) ->bool {
        for (auto &cellResponse : worldMapHandler.PopulateList(x, y, worldNumber)) {
            if (cellResponse != nullptr) {
                auto regChars = cellResponse->GetCharList();
                for (const auto &tempChar : regChars->collection()) {
                    if (!(!ValidateObject(tempChar) || (tempChar->GetInstanceId() != instanceId))){
                        // Is character within the range that normally blocks movement?
                        if (tempChar->GetX() == x && tempChar->GetY() == y && tempChar->GetZ() >= z - 2 && tempChar->GetZ() <= z + 2) {
                            // Is character a visible NPC, or a non-dead visible player?
                            if ((tempChar->IsNpc() && tempChar->GetVisible() == VT_VISIBLE) || (!tempChar->IsDead() && tempChar->GetVisible() == VT_VISIBLE)) {
                                // Character found, potentially blocking
                                return true;
                            }
                        }
                    }
                }
            }
        }
        return false;
        
    }
    
    //======================================================================
    auto doesTerrainBlock(int x, int y, int z,int worldNumber,bool checkWater, bool waterWalk, bool checkMultiPlacement,bool checkForRoad) -> bool {
        // THis seems wrong. If we dont check water/water walk, we just say it doesnt block?
        if (checkWater || waterWalk){
            auto terrain = uoManager.terrainTileAt(worldNumber, x, y);
            if ((checkMultiPlacement && terrain.altitude == z) || (!checkMultiPlacement && (terrain.altitude >= z) && (terrain.altitude -z < 16))) {
                if (z == ILLEGAL_Z){
                    return true ;
                }
                if (waterWalk) {
                    if (!terrain.checkFlag(uo::flag_t::WET)){
                        return true ;
                    }
                }
                else {
                    if (terrain.checkFlag(uo::flag_t::WET) || terrain.checkFlag(uo::flag_t::BLOCKING) ){
                        return true ;
                    }
                }
                // Check that player is not attempting to place a multi on a road
                if (checkWater && checkMultiPlacement && checkForRoad) {
                    if (terrain.isRoad()) {
                        return true;
                    }
                }
            }
        }
        return false ;
    }
    
    //======================================================================
    auto checkStaticFlag(int x, int y, int z, int worldNumber, uo::flag_t toCheck,std::uint16_t &foundtileID,bool checkSpawnSurface ) ->bool {
        auto rvalue = checkSpawnSurface ;
        for (const auto &tile:uoManager.artTileAt(worldNumber, x, y)){
            auto elevation = tile.altitude ;
            auto tileHeight = static_cast<int>(tile.climbHeight()) ;
            if (checkSpawnSurface){
                // special case used when checking for spawn surfaces
                if ((z >= elevation && z <= (elevation + tileHeight)) && !tile.checkFlag(toCheck)){
                    rvalue = false ;
                    break;
                }
            }
            else {
                // Generic check exposed to JS
                if ((z >= elevation && z <= (elevation + tileHeight)) && tile.checkFlag(toCheck)) {
                    foundtileID = tile.tileid;
                    rvalue = true; // Found static with specified flag
                    break;
                }
            }
        }
        return rvalue ;
    }
    
    //======================================================================
    auto checkDynamicFlag(int x, int y, int z, int worldNumber,int instanceId, uo::flag_t toCheck, std::uint16_t &foundTileId) -> bool{
        // Special case for handling multis that cross over between multiple map regions because of size
        auto tempMulti = FindMulti(x,y,z,worldNumber,instanceId);
        if (ValidateObject(tempMulti)){
            // Look for a multi at a specifric location
            auto multiId = static_cast<std::uint16_t>(tempMulti->GetId() - 0x4000);
            for (const auto & entry:uoManager.multiFor(multiId).tiles){
                if (entry.flag.value>0 && ( std::abs( tempMulti->GetZ() + entry.zoffset - z) <=1)){
                    if ((tempMulti->GetX() + entry.xoffset == x) && ( tempMulti->GetY() + entry.yoffset == y)){
                        if (entry.checkFlag(toCheck)){
                            foundTileId = entry.tileid ;
                            return true ;
                        }
                    }
                }
            }
        }
        else {
            // Search map region of given location for all potentially blocking dynamic items
            for (auto &cellResponse : worldMapHandler.PopulateList(x, y, worldNumber)) {
                if (cellResponse == nullptr)
                    continue;
                
                auto regItems = cellResponse->GetItemList();
                for (const auto &item : regItems->collection()) {
                    if (!ValidateObject(item) || item->GetInstanceId() != instanceId)
                        continue;
                    
                    if ((item->GetId(1) >= 0x40) && ((item->GetObjType() == CBaseObject::OT_MULTI) || (item->CanBeObjType(CBaseObject::OT_MULTI)))) {
                        // Found a multi
                        // Look for a multi item at specific location
                        auto multiId = static_cast<std::uint16_t>(item->GetId() - 0x4000);
                        for (const auto & entry:uoManager.multiFor(multiId).tiles){
                            if (entry.flag.value>0 && ( std::abs( tempMulti->GetZ() + entry.zoffset - z) <=1)){
                                if ((tempMulti->GetX() + entry.xoffset == x) && ( tempMulti->GetY() + entry.yoffset == y)){
                                    if (entry.checkFlag(toCheck)){
                                        foundTileId = entry.tileid ;
                                        return true ;
                                    }
                                }
                            }
                        }
                    }
                    else {
                        // Item is not a multi
                        if ((item->GetX() == x) && (item->GetY() == y) && (item->GetZ() == z)) {
                            auto itemZ = item->GetZ();
                            auto tileHeight = uoManager.art(item->GetId()).climbHeight() ;
                            if ((itemZ == z && itemZ + tileHeight > z) || (itemZ < z && itemZ + tileHeight >= z)) {
                                if (uoManager.art(item->GetId()).checkFlag(toCheck)) {
                                    foundTileId = item->GetId();
                                    return true;
                                }
                            }
                        }
                        
                    }
                }
            }
        }
        return false ;
    }
    
    //======================================================================
    auto staticTop(int x, int y, int z, int worldNumber,int maxZ ) -> int{
        auto top = static_cast<int>(ILLEGAL_Z);
        for (const auto &tile:uoManager.artTileAt(worldNumber, x, y)){
            auto tempTop = tile.altitude + static_cast<int>(tile.climbHeight()) ;
            if (( tempTop <= x +maxZ) && (tempTop>top)) {
                top = tempTop ;
            }
        }
        return top ;
    }
    
    //======================================================================
    auto mapElevation(int x, int y, int worldNumber) -> int {
        auto tile = uoManager.terrainTileAt(worldNumber, x, y) ;
        if (tile.isVoid()){
            return static_cast<int>(ILLEGAL_Z) ;
        }
        return tile.altitude ;
    }
    
    //======================================================================
    auto dynamicElevation(int x, int y, int z, int worldNumber,int instanceId, int maxZ) -> int {
        auto dynZ = static_cast<int>(ILLEGAL_Z) ;
        // Special case for handling multis that cross over between multiple map regions because of size
        auto tempMulti = FindMulti(x, y, z, worldNumber, instanceId);
        if (ValidateObject(tempMulti)) {
            dynZ = heightOfMulti(tempMulti, x, y, z, maxZ);
            // Also check dynamic items inside the multi
            for (const auto &tempMultiItem : tempMulti->GetItemsInMultiList()->collection()) {
                if (tempMultiItem->GetX() == x && tempMultiItem->GetY() == y) {
                    auto  zTemp = tempMultiItem->GetZ() + uoManager.art(tempMultiItem->GetId()).climbHeight() ;
                    if ((zTemp <= z + maxZ) && (zTemp > dynZ || (dynZ >= z + maxZ))) {
                        dynZ = zTemp;
                    }
                }
            }
        }
        else {
            auto mapArea = worldMapHandler.GetMapRegion(worldMapHandler.GetGridX(x), worldMapHandler.GetGridY(y), worldNumber);
            if (mapArea){
                auto regItems = mapArea->GetItemList();
                for (const auto tempItem : regItems->collection()) {
                    if (ValidateObject(tempItem) || tempItem->GetInstanceId() != instanceId) {
                        if (tempItem->GetId(1) >= 0x40 && tempItem->CanBeObjType(CBaseObject::OT_MULTI)) {
                            
                            dynZ = heightOfMulti(tempItem, x, y, z, maxZ);
                        }
                        else if (tempItem->GetX() == x && tempItem->GetY() == y) {
                            auto zTemp = tempItem->GetZ() + uoManager.art(tempItem->GetId()).climbHeight();
                            if ((zTemp <= z + maxZ) && zTemp > dynZ) {
                                dynZ = zTemp;
                            }
                        }
                    }
                }
            }
        }
        return dynZ ;
    }
    
    //======================================================================
    auto playerElevation(int x, int y, int z, int worldNumber, int instanceId) -> int {
        // let's check in this order.. dynamic, static, then the map
        auto elevation = dynamicElevation(x, y, z, worldNumber, instanceId, MAX_Z_STEP);
        if (ILLEGAL_Z != elevation)
            return elevation;
        
        elevation = staticTop(x, y, z, worldNumber, MAX_Z_STEP);
        if (ILLEGAL_Z != elevation)
            return elevation;
        
        return mapElevation(x, y, worldNumber);
        
    }
    
    //======================================================================
    auto inBuilding(int x, int y, int z, int worldNumber, int instanceId) ->bool {
        auto elevation = dynamicElevation(x, y, z, worldNumber, instanceId, 127) ;
        if (elevation > (z+10)){
            return true ;
        }
        elevation = staticTop(x, y, z, worldNumber, 127);
        if (elevation > (z+10)){
            return true ;
        }
        return false ;
    }
    
    //======================================================================
    auto multiCorners(const CMultiObj *i)->std::tuple<int,int,int,int>{
        if (ValidateObject(i)) {
            auto xadd = i->GetX() ;
            auto yadd = i->GetY() ;
            const auto & multi = uoManager.multiFor(i->GetX()-0x4000) ;
            return std::make_tuple(multi.minXOffset+xadd,multi.minYOffset+yadd,multi.maxXOffset+xadd,multi.maxYOffset + yadd) ;
        }
        return std::make_tuple(0,0,0,0) ;
    }
    
    //======================================================================
    auto validSpawnLocation(int x, int y, int z, int worldNumber,int instanceId, bool checkWater) -> bool {
        if (uoManager.validLocation(x, y, worldNumber)){
            // get the tile id of any dynamic tiles at this spot
            if (!doesDynamicBlock(x, y, z, worldNumber, instanceId, checkWater, !checkWater, false, false)){
                if (!doesStaticBlock(x, y, z, worldNumber, checkWater)){
                    // if the static isn't a surface return false
                    [[maybe_unused]] std::uint16_t ignoreMe = 0;
                    if (checkStaticFlag(x, y, z, worldNumber, (checkWater ? uo::flag_t::SURFACE : uo::flag_t::WET), ignoreMe, true)){
                        if (!doesTerrainBlock(x, y, z, worldNumber, checkWater, !checkWater, false, false)){
                            return true ;
                        }
                    }
                }
            }
        }
        return false;
    }
    
    //======================================================================
    auto validMultiLocation(int x, int y, int z, int worldNumber,int instanceId, bool checkWater, bool checkOnlyOtherMultis, bool checkOnlyNonMultis, bool checkForRoads) -> int{
        if (!uoManager.validLocation(x, y, worldNumber)){
            return 0 ;
        }
        auto elevation = playerElevation(x, y, z, worldNumber, instanceId);
        if (elevation == ILLEGAL_Z){
            return 0 ;
        }
        // We don't want the house to be halfway embedded in a hill... or hanging off one for that
        // matter.
        if (z != ILLEGAL_Z){
            if (elevation - z > MAX_Z_STEP) {
                return 0 ;
            }
            else if (z - elevation > MAX_Z_FALL) {
                return 0 ;
            }
        }
        // get the tile id of any dynamic tiles at this spot
        
        if (!checkOnlyOtherMultis && doesDynamicBlock(x, y, elevation, worldNumber,instanceId,checkWater,false,checkOnlyOtherMultis,checkOnlyNonMultis)){
            return 2 ;
        }
        // if there's a static block here in our way, return false
        if (!checkOnlyOtherMultis && doesStaticBlock(x,y,elevation, worldNumber,checkWater)){
            return 2 ;
        }
        if (doesTerrainBlock(x, y, z, worldNumber, checkWater, false, true, checkForRoads)){
            return 0 ;
        }
        // Check if house placement is allowed in region
        auto calcReg = CalcRegionFromXY(x, y, worldNumber, instanceId);
        if ((!calcReg->CanPlaceHouse() && checkWater) || calcReg->IsDungeon() | (calcReg->IsGuarded() && checkWater)) {
            return 3 ;
        }
        // Ok, it is fine
        return 1 ;
    }
}
