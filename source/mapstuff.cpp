#include "uox3.h"
#include "regions.h"
#include "townregion.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "scriptc.h"
#include <filesystem>

#include <algorithm>
#include <array>
#include <fstream>

using namespace std::string_literals ;

CMulHandler *Map				= nullptr;

//const UI16 LANDDATA_SIZE		= 0x4000; //(512 * 32)

/*
 ** elevation or actual height of a tile.  Here's the new renaming plan:
 ** Elevation - the z value of the bottom edge of an item
 ** Height - the height of the item, it is independant of the elevation, like a person is 6ft tall
 ** Top - the z value of the top edge of an item, is always Elevation + Height of item
 */



//===============================================================================================
// CMulHandler
//===============================================================================================
//===============================================================================================

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void LoadMapsDFN( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads maps.dfn to see which map files to actually load into memory later
//o-----------------------------------------------------------------------------------------------o
auto CMulHandler::loadMapsDFN(const std::string &uodir) ->std::map<int,mapdfndata_st> {
	auto entrycount = FileLookup->CountOfEntries(maps_def);
	std::map<int,mapdfndata_st> results ;
	auto uopath = std::filesystem::path(uodir) ;
	for( auto i = 0;i<entrycount;i++) {
		auto toFind = FileLookup->FindEntry( "MAP "s + std::to_string(i), maps_def );
		if (toFind) {
			auto entry = mapdfndata_st() ;
			for (const auto &sec :toFind->collection()){
				auto UTAG = oldstrutil::upper(sec->tag);
				auto data = oldstrutil::trim(oldstrutil::removeTrailing(sec->data));
				switch (UTAG.data()[0]){
					case 'M':
						if (UTAG == "MAP"){
							entry.mapfile = (uopath / std::filesystem::path(data)) ;
						}
						else if (UTAG == "MAPUOPWRAP"){
							entry.mapuop = (uopath / std::filesystem::path(data));
						}
						else if (UTAG == "MAPDIFF"){
							entry.mapdiff = (uopath / std::filesystem::path(data));
						}
						else if (UTAG == "MAPDIFFLIST"){
							entry.mapdiffl = (uopath / std::filesystem::path(data));
						}
						break;
					case 'S':
						if (UTAG == "STATICS"){
							entry.stamul = (uopath / std::filesystem::path(data));
						}
						else if (UTAG == "STAIDX"){
							entry.staidx = (uopath / std::filesystem::path(data));
						}
						else if (UTAG == "STATICSDIFF"){
							entry.stadiff = (uopath / std::filesystem::path(data));
						}
						else if (UTAG == "STATICSDIFFLIST"){
							entry.stadiffl = (uopath / std::filesystem::path(data));
						}
						else if (UTAG == "STATICSDIFFINDEX"){
							entry.stadiffi = (uopath / std::filesystem::path(data));
						}
						break;
					case 'X':
						if (UTAG == "X"){
							entry.width = std::stoi(data);
						}
						break;
						
					case 'Y':
						if (UTAG == "Y"){
							entry.height = std::stoi(data);
						}
						break;
						
				}
			}
			results.insert_or_assign(i, entry);
		}
	}
	return results ;
}


//=======================================================================
auto CMulHandler::loadDFNOverrides() ->void {
	std::uint16_t entryNum;
	for (auto &mapScp : FileLookup->ScriptListings[maps_def]){
		if( mapScp ){
			for (const auto &[entryName, toScan]: mapScp->collection()){
				if( toScan ){
					
					entryNum	= oldstrutil::value<std::uint16_t>(oldstrutil::extractSection(entryName," ", 1, 1 ));
					auto titlePart = oldstrutil::upper( oldstrutil::extractSection( entryName, " ", 0, 0 ));
					// have we got an entry starting with TILE ?
					if( titlePart == "TILE" && entryNum ) {
						if( (entryNum != INVALIDID) && (entryNum < tile_info.sizeArt())) {
							auto tile = &tile_info.art(entryNum);
							if(tile) {
								for (const auto &sec : toScan->collection()){
									auto UTag = oldstrutil::upper( sec->tag );
									auto data = oldstrutil::trim( oldstrutil::removeTrailing( sec->data, "//" ));
									// CTile properties
									if( UTag == "WEIGHT" ){
										tile->Weight( static_cast<UI08>(std::stoul(data, nullptr, 0)) );
									}
									else if( UTag == "HEIGHT" ){
										tile->Height( static_cast<SI08>(std::stoi(data, nullptr, 0))) ;
									}
									else if( UTag == "LAYER" ){
										tile->Layer( static_cast<SI08>(std::stoi(data, nullptr, 0))) ;
									}
									else if( UTag == "HUE" ){
										tile->Hue( static_cast<UI08>(std::stoul(data, nullptr, 0)) );
									}
									else if( UTag == "QUANTITY" ){
										tile->Quantity( static_cast<UI08>(std::stoul(data, nullptr, 0)) );
									}
									else if( UTag == "ANIMATION" ){
										tile->Animation( static_cast<UI16>(std::stoul(data, nullptr, 0)) );
									}
									else if( UTag == "NAME" ) {
										tile->Name( data );
									}
									// BaseTile Flag 1
									else if( UTag == "ATFLOORLEVEL" ){
										tile->SetFlag( TF_FLOORLEVEL, (std::stoi(data, nullptr, 0) != 0) );
									}
									else if( UTag == "HOLDABLE" ){
										tile->SetFlag( TF_HOLDABLE, (std::stoi(data, nullptr, 0) != 0) );
									}
									else if( UTag == "SIGNGUILDBANNER" ){
										tile->SetFlag( TF_TRANSPARENT, (std::stoi(data, nullptr, 0) != 0) );
									}
									else if( UTag == "WEBDIRTBLOOD" ){
										tile->SetFlag( TF_TRANSLUCENT, (std::stoi(data, nullptr, 0) != 0) );
									}
									else if( UTag == "WALLVERTTILE" ){
										tile->SetFlag( TF_WALL, (std::stoi(data, nullptr, 0) != 0) );
									}
									else if( UTag == "DAMAGING" ){
										tile->SetFlag( TF_DAMAGING, (std::stoi(data, nullptr, 0) != 0) );
									}
									else if( UTag == "BLOCKING" ){
										tile->SetFlag( TF_BLOCKING, (std::stoi(data, nullptr, 0) != 0) );
									}
									else if( UTag == "LIQUIDWET" ){
										tile->SetFlag( TF_WET, (std::stoi(data, nullptr, 0) != 0) );
									}
									
									// BaseTile Flag 2
									else if( UTag == "UNKNOWNFLAG1" ){
										tile->SetFlag( TF_UNKNOWN1, (std::stoi(data, nullptr, 0) != 0) );
									}
									else if( UTag == "STANDABLE" ){
										tile->SetFlag( TF_SURFACE, (std::stoi(data, nullptr, 0) != 0) );
									}
									else if( UTag == "CLIMBABLE" ){
										tile->SetFlag( TF_CLIMBABLE, (std::stoi(data, nullptr, 0) != 0) );
									}
									else if( UTag == "STACKABLE" ){
										tile->SetFlag( TF_STACKABLE, (std::stoi(data, nullptr, 0) != 0) );
									}
									else if( UTag == "WINDOWARCHDOOR" ){
										tile->SetFlag( TF_WINDOW, (std::stoi(data, nullptr, 0) != 0) );
									}
									else if( UTag == "CANNOTSHOOTTHRU" ){
										tile->SetFlag( TF_NOSHOOT, (std::stoi(data, nullptr, 0) != 0) );
									}
									else if( UTag == "DISPLAYASA" ){
										tile->SetFlag( TF_DISPLAYA, (std::stoi(data, nullptr, 0) != 0) );
									}
									else if( UTag == "DISPLAYASAN" ){
										tile->SetFlag( TF_DISPLAYAN, (std::stoi(data, nullptr, 0) != 0) );
									}
									
									// BaseTile Flag 3
									else if( UTag == "DESCRIPTIONTILE" ){
										tile->SetFlag( TF_DESCRIPTION, (std::stoi(data, nullptr, 0) != 0) );
									}
									else if( UTag == "FADEWITHTRANS" ){
										tile->SetFlag( TF_FOLIAGE, (std::stoi(data, nullptr, 0) != 0) );
									}
									else if( UTag == "PARTIALHUE" ){
										tile->SetFlag( TF_PARTIALHUE, (std::stoi(data, nullptr, 0) != 0) );
									}
									else if( UTag == "UNKNOWNFLAG2" ){
										tile->SetFlag( TF_UNKNOWN2, (std::stoi(data, nullptr, 0) != 0) );
									}
									else if( UTag == "MAP" ){
										tile->SetFlag( TF_MAP, (std::stoi(data, nullptr, 0) != 0) );
									}
									else if( UTag == "CONTAINER" ){
										tile->SetFlag( TF_CONTAINER, (std::stoi(data, nullptr, 0) != 0) );
									}
									else if( UTag == "EQUIPABLE" ){
										tile->SetFlag( TF_WEARABLE, (std::stoi(data, nullptr, 0) != 0) );
									}
									else if( UTag == "LIGHTSOURCE" ){
										tile->SetFlag( TF_LIGHT, (std::stoi(data, nullptr, 0) != 0) );
									}
									
									// BaseTile Flag 4
									else if( UTag == "ANIMATED" ){
										tile->SetFlag( TF_ANIMATED, (std::stoi(data, nullptr, 0) != 0) );
									}
									else if( UTag == "NODIAGONAL" ){
										tile->SetFlag( TF_NODIAGONAL, (std::stoi(data, nullptr, 0) != 0) );
									}
									else if( UTag == "UNKNOWNFLAG3" ){
										tile->SetFlag( TF_UNKNOWN3, (std::stoi(data, nullptr, 0) != 0) );
									}
									else if( UTag == "WHOLEBODYITEM" ){
										tile->SetFlag( TF_ARMOR, (std::stoi(data, nullptr, 0) != 0) );
									}
									else if( UTag == "WALLROOFWEAP" ){
										tile->SetFlag( TF_ROOF, (std::stoi(data, nullptr, 0) != 0) );
									}
									else if( UTag == "DOOR" ){
										tile->SetFlag( TF_DOOR, (std::stoi(data, nullptr, 0) != 0) );
									}
									else if( UTag == "CLIMBABLEBIT1" ){
										tile->SetFlag( TF_STAIRBACK, (std::stoi(data, nullptr, 0) != 0) );
									}
									else if( UTag == "CLIMBABLEBIT2" ){
										tile->SetFlag( TF_STAIRRIGHT, (std::stoi(data, nullptr, 0) != 0) );
									}
								}
							}
						}
					}
					else if( titlePart == "LAND" && entryNum ) {	// let's not deal with this just yet
					}
				}
			}
		}
	}
}
//===================================================================================
auto CMulHandler::loadTileData(const std::string& uodir) ->void {
	std::string lName = uodir + "tiledata.mul"s;
	Console << "\t" << lName << "\t";
	
	if (!tile_info.load(lName)) {
		Console.PrintFailed();
		Shutdown( FATAL_UOX3_TILEDATA_NOT_FOUND );
		
	}
	Console.PrintDone();
}
//===================================================================================
auto CMulHandler::loadMultis( const std::string& uodir ) ->void {
	// now main memory multiItems
	Console << "Loading Multis....  ";
	// Odd we do no check?
	if (!multi_data.load(std::filesystem::path(uodir),&tile_info)) {
		Console.PrintFailed();
		Shutdown( FATAL_UOX3_MULTI_DATA_NOT_FOUND );
	}
	Console.PrintDone();
}
//================================================================================
auto CMulHandler::load() ->void {
	auto uodir = cwmWorldState->ServerData()->Directory( CSDDP_DATA );
	auto mapinfo = loadMapsDFN(uodir) ;
	Console.PrintSectionBegin();
	Console << "Loading uodata (for maps, only mapfile shown)..." << myendl << "(If they don't open, fix your paths in uox.ini or filenames in maps.dfn)" << myendl;
	loadTileData(uodir);
	loadDFNOverrides() ;
	loadMapAndStatics(mapinfo) ;
	if (uoworlds.empty()){
		Console.error( " Fatal Error: No maps found" );
		Console.error( " Check the settings for DATADIRECTORY in uox.ini" );
		Shutdown( FATAL_UOX3_MAP_NOT_FOUND );
	}
	loadMultis(uodir);
	FileLookup->Dispose(maps_def);
	Console.PrintSectionBegin();
}
//===================================================================================
auto CMulHandler::loadMapAndStatics(const std::map<int,mapdfndata_st> &info) ->void{
	for (const auto &[mapnum,dfndata]:info) {
		uoworlds.insert_or_assign(mapnum, ultimamap(mapnum,dfndata.width,dfndata.height,&tile_info)) ;
		auto rvalue = false ;
		
		if (std::filesystem::exists(dfndata.mapuop)) {
			Console << "\t" << dfndata.mapuop.string() << "(/" << dfndata.mapuop.filename().string() << ")\t\t";
			rvalue = uoworlds[mapnum].loadUOPTerrainFile(dfndata.mapuop.string());
		}
		else {
			Console << "\t" << dfndata.mapfile.string() << "(/" << dfndata.mapfile.filename().string() << ")\t\t";
			rvalue = uoworlds[mapnum].loadMulTerrainFile(dfndata.mapfile.string());
			if (rvalue) {
				uoworlds[mapnum].applyTerrainDiff(dfndata.mapdiffl.string(), dfndata.mapdiff.string());
			}
		}
		if (rvalue){
			rvalue = uoworlds[mapnum].loadArt(dfndata.stamul.string(), dfndata.staidx.string()) ;
			if (rvalue) {
				uoworlds[mapnum].applyDiff(dfndata.stadiffl.string(), dfndata.stadiffi.string(), dfndata.stadiff.string());
			}
			else {
				// this is intersting, we found the terrain, but not the art.  Regardless, lets delete it
				auto iter = uoworlds.find(mapnum) ;
				if (iter != uoworlds.end()){
					uoworlds.erase(iter);
				}
			}
		}
		if (!rvalue) {
			Console.PrintSpecial( CBLUE, "not found" );
		}
		else {
			Console.PrintDone();
		}
	}
}
//================================================================================================
auto CMulHandler::artAt(std::int16_t x, std::int16_t y, std::uint8_t world) ->std::vector<tile_t>& {
	static std::vector<tile_t> empty;
	try {
		return uoworlds.at(world).art(x,y);
	}
	catch(...) {
		return empty;
	}
}
//================================================================================================
auto CMulHandler::artAt(std::int16_t x, std::int16_t y, std::uint8_t world) const -> const std::vector<tile_t>& {
	static std::vector<tile_t> empty;
	try {
		return uoworlds.at(world).art(x,y);
	}
	catch(...) {
		return empty;
	}
}
//=============================================================================================
auto CMulHandler::sizeOfMap(std::uint8_t worldNumber) const ->std::pair<int,int> {
	return uoworlds.at(worldNumber).size() ;
}
auto CMulHandler::diffCountForMap(std::uint8_t worldNumber) const ->std::pair<int,int> {
	return std::make_pair(uoworlds.at(worldNumber).diffArt(),uoworlds.at(worldNumber).diffTerrain()) ;
}

//=============================================================================================
auto CMulHandler::MultiHeight(CItem *i, std::int16_t x, std::int16_t y, std::int8_t oldZ, std::int8_t maxZ, bool checkHeight) ->std::int8_t{
	
	UI16 multiID = static_cast<UI16>(i->GetID() - 0x4000);
	
	SI08 mHeight = ILLEGAL_Z;
	SI08 tmpTop = ILLEGAL_Z;
	
	if(multiExists(multiID)){
	
	const SI16 baseX = i->GetX();
	const SI16 baseY = i->GetY();
	const SI08 baseZ = i->GetZ();
	for( auto &multi : SeekMulti( multiID ).items ) {
		if(( checkHeight || multi.flag ) && ( baseX + multi.offsetx ) == x && ( baseY + multi.offsety ) == y ) {
			if( checkHeight ) {
				// Returns height of highest point of multi
				tmpTop = static_cast<SI08>(baseZ + multi.altitude);
				if( ( tmpTop <= oldZ + maxZ ) && tmpTop > oldZ && tmpTop > mHeight ){
					mHeight = tmpTop;
				}
			}
			else {
				tmpTop = static_cast<SI08>( baseZ + multi.altitude );
				if( std::abs( tmpTop - oldZ ) <= maxZ ) {
					mHeight =  tmpTop + multi.info->ClimbHeight();
					break;
				}
			}
		}
	}
	}

	
	return mHeight;
}

//=============================================================================================
auto CMulHandler::MultiTile(CItem *i, std::int16_t x, std::int16_t y, std::int8_t oldz, bool checkVisible) ->std::uint16_t {
	auto rvalue = std::uint16_t() ;
	if( !i->CanBeObjType( OT_MULTI ) )
		return 0;
	UI16 multiID = static_cast<UI16>(i->GetID() - 0x4000);
	SI32 length = 0;
	
	if( !multiExists(multiID)) {
		Console << "CMulHandler::MultiTile->Bad length in multi file. Avoiding stall." << myendl;
		auto map1 = Map->SeekMap( i->GetX(), i->GetY(), i->WorldNumber() );
		if( map1.CheckFlag( TF_WET ) ) // is it water?
			i->SetID( 0x4001 );
		else
			i->SetID( 0x4064 );
		length = 0;
	}
	else {
		// Loop through each item that makes up the multi
		// If any of those items intersect with area were are checking, return the ID of that tile
		for( auto &multi : SeekMulti(multiID ).items) {
			if(!checkVisible) {
				if( ( i->GetX() + multi.offsetx == x ) && ( i->GetY() + multi.offsety == y ) ) {
					rvalue = multi.tileid;
					break;
				}
			}
			else if(multi.flag > 0 && ( std::abs( i->GetZ() + multi.altitude - oldz ) <= 1) ) {
				if( ( i->GetX() + multi.offsetx == x ) && ( i->GetY() + multi.offsety == y ) ) {
					rvalue = multi.tileid;
					break;
				}
			}
		}
	}
	
	return rvalue;
}

//=============================================================================================
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if statics at/above given coordinates blocks movement, etc
//o-----------------------------------------------------------------------------------------------o
auto CMulHandler::DoesStaticBlock(std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, bool checkWater) ->bool {
	auto &artwork = Map->artAt( x, y, worldNumber );
	auto rvalue = false ;
	for (auto &tile:artwork){
		auto elev = static_cast<SI08>(tile.altitude + tile.artInfo->ClimbHeight());
		if( checkWater ) {
			if( elev >= z && tile.altitude <= z && ( tile.CheckFlag( TF_BLOCKING ) || tile.CheckFlag( TF_WET ) )){
				rvalue = true;
				break;
			}
		}
		else{
			if( elev >= z && tile.altitude <= z && ( tile.CheckFlag( TF_BLOCKING ) && !tile.CheckFlag( TF_WET ) )){
				rvalue = true;
				break;
			}
		}
	}
	return rvalue;
}
//=============================================================================================
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if there are any dynamic tiles at given coordinates that block movement
//o-----------------------------------------------------------------------------------------------o
auto CMulHandler::DoesDynamicBlock(std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::uint16_t realm, bool checkWater, bool waterWalk, bool checkOnlyMultis, bool checkOnlyNonMultis) ->bool {
	auto rvalue = false ;
	// get the tile id of any dynamic tiles at this spot
	auto dtItem = DynTile( x, y, z, worldNumber, realm, checkOnlyMultis, checkOnlyNonMultis );
	if( ValidateObject( dtItem )) {
		rvalue = true ;
		// Don't allow placing houses on top of immovable, visible dynamic items
		if( dtItem->GetMovable() != 2 || dtItem->GetVisible() != 0 ){
			// Ignore items that are permanently invisible or visible to GMs only
			rvalue = false ;
			if( dtItem->GetVisible() != 1 && dtItem->GetVisible() != 3 ) {
				const UI16 dt = dtItem->GetID();
				if( IsValidTile( dt ) && dt != 0 ) {
					auto tile = SeekTile( dt );
					if( waterWalk ) {
						if( !tile.CheckFlag( TF_WET ) ){
							rvalue = true ;
						}
					}
					else {
						if( tile.CheckFlag( TF_ROOF ) || tile.CheckFlag( TF_BLOCKING ) || (checkWater && tile.CheckFlag( TF_WET ) ) /*|| !tile.CheckFlag( TF_SURFACE ) */ ){
							rvalue = true ;
						}
					}
				}
			}
		}
	}
	return rvalue;
}
//=============================================================================================
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns which dynamic tile is present at (x,y) or -1 if no tile exists
//o-----------------------------------------------------------------------------------------------o
auto CMulHandler::DynTile( std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::uint16_t realm, bool checkOnlyMultis, bool checkOnlyNonMultis ) ->CItem *{
	auto rvalue = static_cast<CItem*>(nullptr) ;
	for( auto &CellResponse:MapRegion->PopulateList( x, y, worldNumber ) ) {
		if( CellResponse ){
			auto regItems = CellResponse->GetItemList();
			for (const auto &Item : regItems->collection()){
				if( ValidateObject( Item ) && ( Item->GetInstanceID() == realm) ){
					if( !checkOnlyNonMultis ){
						if( Item->GetID( 1 ) >= 0x40 && ( Item->GetObjType() == OT_MULTI || Item->CanBeObjType( OT_MULTI ) ) ){
							auto deetee = MultiTile( Item, x, y, z, false );
							if( deetee > 0 ){
								rvalue = Item;
								break;
							}
						}
						else if( Item->GetMulti() != INVALIDSERIAL || ValidateObject( calcMultiFromSer( Item->GetOwner() ) ) ){
							if( (Item->GetX() == x) && (Item->GetY() == y )){
								rvalue = Item ;
								break;
							}
							
						}
					}
					else if( !checkOnlyMultis && (Item->GetX() == x) && (Item->GetY() == y) ){
						rvalue = Item ;
						break;
					}
				}
			}
			if (rvalue) {
				break;
			}
			
		}
	}
	return rvalue;
}
//=============================================================================================
//=============================================================================================
//|	Purpose		-	Checks if the map tile at the given coordinates block movement
//o-----------------------------------------------------------------------------------------------o
auto CMulHandler::DoesMapBlock(std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, bool checkWater, bool waterWalk, bool checkMultiPlacement, bool checkForRoad) ->bool {
	auto rvalue = false ;
	if( checkWater || waterWalk ) {
		const auto &map = SeekMap( x, y, worldNumber );
		if(( checkMultiPlacement && map.altitude == z ) || ( !checkMultiPlacement && map.altitude >= z && map.altitude - z < 16 )) {
			if( z == ILLEGAL_Z ) {
				rvalue = true ;
			}
			else if( waterWalk ) {
				if( !map.CheckFlag(TF_WET)) {
					rvalue = true ;
				}
			}
			else {
				if( map.CheckFlag( TF_WET ) || map.CheckFlag( TF_BLOCKING )){
					rvalue = true ;
				}
			}
			// Check that player is not attempting to place a multi on a road
			if (!rvalue){
				if( checkWater && checkMultiPlacement && checkForRoad ) {
					if (map.terrainInfo->isRoadID()) {
						rvalue = true  ;
					}
				}
			}
		}
	}
	return rvalue ;
}
//=============================================================================================
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks to see whether any statics at given coordinates has a specific flag
//o-----------------------------------------------------------------------------------------------o
auto CMulHandler::CheckStaticFlag(std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, TileFlags toCheck, bool checkSpawnSurface) ->bool{
	auto rvalue = checkSpawnSurface ;
	for( const auto &tile:Map->artAt( x, y, worldNumber ) ) {
		const SI08 elev = static_cast<SI08>( tile.altitude );
		const SI08 tileHeight = static_cast<SI08>( tile.artInfo->ClimbHeight()  );
		if( checkSpawnSurface ) {
			// Special case used when checking for spawn surfaces
			if(( z >= elev && z <= ( elev + tileHeight )) && !tile.CheckFlag( toCheck ) ){
				rvalue = false ;
				break;
			}
		}
		else {
			// Generic check exposed to JS
			if(( z >= elev && z <= ( elev + tileHeight )) && tile.CheckFlag( toCheck ) ){
				rvalue = true; // Found static with specified flag
				break;
			}
		}
	}
	return rvalue;
}

//=============================================================================================
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks to see whether any dynamics at given coordinates has a specific flag
//o-----------------------------------------------------------------------------------------------o
auto CMulHandler::CheckDynamicFlag(std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::uint16_t realm, TileFlags toCheck) ->bool {
	
	for( auto  &CellResponse: MapRegion->PopulateList( x, y, worldNumber )) {
		if( CellResponse ) {
			
			auto regItems = CellResponse->GetItemList();
			for (const auto &Item : regItems->collection()){
				if( ValidateObject( Item ) && (Item->GetInstanceID() == realm )){
					
					if( (Item->GetID( 1 ) >= 0x40) && ( (Item->GetObjType() == OT_MULTI) || (Item->CanBeObjType( OT_MULTI )) ) ){
						// Found a multi
						// Look for a multi item at specific location
						auto multiID = static_cast<UI16>( Item->GetID() - 0x4000 );
						for( auto &multiItem : SeekMulti( multiID ).items ){
							if( multiItem.flag > 0 && ( abs( Item->GetZ() + multiItem.altitude - z ) <= 1 ) ){
								if( ( Item->GetX() + multiItem.offsetx == x ) && ( Item->GetY() + multiItem.offsety == y ) ){
									if( SeekTile( multiItem.tileid ).CheckFlag( toCheck ) ){
										return true;
									}
								}
							}
						}
					}
					else {
						// Item is not a multi
						if( (Item->GetX() == x) && (Item->GetY() == y) && (Item->GetZ() == z) ){
							
							auto itemZ = Item->GetZ();
							const SI08 tileHeight = static_cast<SI08>( TileHeight( Item->GetID() ) );
							if(( itemZ == z && itemZ + tileHeight > z ) || ( itemZ < z && itemZ + tileHeight >= z )){
								if( SeekTile( Item->GetID() ).CheckFlag( toCheck ) ){
									return true;
								}
							}
						}
					}
				}
			}
		}
	}
	return false;
}

//=============================================================================================
//|	Purpose		-	Checks to see whether given tile ID has a specified flag
//o-----------------------------------------------------------------------------------------------o
auto CMulHandler::CheckTileFlag(std::uint16_t itemID, TileFlags flagToCheck) ->bool {
	if( !SeekTile( itemID ).CheckFlag( flagToCheck )){
		return false;
	}
	return true;
}
//=============================================================================================
//|	Purpose		-	Top of statics at/above given coordinates
//o-----------------------------------------------------------------------------------------------o
auto CMulHandler::StaticTop(std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::int8_t maxZ) ->std::int8_t {
	auto top = ILLEGAL_Z;
	
	const auto &artwork = Map->artAt(x, y, worldNumber );
	for (const auto &tile:artwork){
		auto tempTop = static_cast<std::int8_t>(tile.altitude + tile.artInfo->ClimbHeight());
		if( ( tempTop <= z + maxZ ) && ( tempTop > top ) )
			top = tempTop;
	}
	return top;
}

//=============================================================================================
//|	Purpose		-	This was fixed to actually return the *elevation* of dynamic items at/above given coordinates
//o-----------------------------------------------------------------------------------------------o
auto CMulHandler::DynamicElevation(std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::int8_t maxZ, std::uint16_t realm) ->std::int8_t {
	auto illz = ILLEGAL_Z;
	
	auto MapArea = MapRegion->GetMapRegion( MapRegion->GetGridX( x ), MapRegion->GetGridY( y ), worldNumber );
	if( MapArea )	{
		auto regItems = MapArea->GetItemList();
		for (const auto tempItem : regItems->collection()){
			if( ValidateObject( tempItem ) || tempItem->GetInstanceID() != realm ){
				if( tempItem->GetID( 1 ) >= 0x40 && tempItem->CanBeObjType( OT_MULTI )){
					illz = MultiHeight( tempItem, x, y, z, maxZ );
				}
				else if( tempItem->GetX() == x && tempItem->GetY() == y ){
					SI08 ztemp = (SI08)(tempItem->GetZ() + TileHeight( tempItem->GetID() ));
					if( ( ztemp <= z + maxZ ) && ztemp > z ){
						illz = ztemp;
					}
				}
			}
		}
	}
	return illz;
}
//=============================================================================================
//|	Purpose		-	Returns the map elevation at given coordinates, we'll assume since its land
//|					the height is inherently 0
//o-----------------------------------------------------------------------------------------------o
auto CMulHandler::MapElevation(std::int16_t x, std::int16_t y, std::uint8_t worldNumber) ->std::int8_t {
	const auto &map = SeekMap( x, y, worldNumber );
	// make sure nothing can move into black areas
	if (map.isVoid()){
		return ILLEGAL_Z;
	}
	return map.altitude;
}

//=============================================================================================
//|	Purpose		-	Height of a given tile (If climbable we return 1/2 its height).
//o-----------------------------------------------------------------------------------------------o
auto CMulHandler::TileHeight(std::uint16_t tilenum) ->std::int8_t {
	return SeekTile(tilenum).ClimbHeight() ;
}

//=============================================================================================
//|	Purpose		-	Returns new height of player who walked to X/Y but from OLDZ
//o-----------------------------------------------------------------------------------------------o
auto CMulHandler::Height(std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber,  std::uint16_t realm) ->std::int8_t {
	// let's check in this order.. dynamic, static, then the map
	auto dynz = DynamicElevation( x, y, x, worldNumber, MAX_Z_STEP, realm );
	if( ILLEGAL_Z != dynz )
		return dynz;
	
	auto staticz = StaticTop( x, y, x, worldNumber, MAX_Z_STEP );
	if( ILLEGAL_Z != staticz )
		return staticz;
	
	return MapElevation( x, y, worldNumber );
}

//=============================================================================================
//|	Purpose		-	Returns whether give coordinates are inside a building by checking if there is
//|					a multi or static above them
//o-----------------------------------------------------------------------------------------------o
auto CMulHandler::inBuilding( std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::uint16_t realm) ->bool {
	auto dynz = Map->DynamicElevation( x, y, z, worldNumber, static_cast<std::int8_t>(127), realm );
	if( dynz > ( z + 10)) {
		return true;
	}
	else {
		const SI08 staticz = Map->StaticTop( x, y, z, worldNumber, static_cast<std::int8_t>(127));
		if( staticz > ( z + 10)){
			return true;
		}
	}
	return false;
}

//=============================================================================================
//|	Purpose		-	Finds the corners of a multi object
//o--------------------------------------------------------------------------
auto CMulHandler::MultiArea(CMultiObj *i, std::int16_t &x1, std::int16_t &y1, std::int16_t &x2, std::int16_t &y2) ->void {
	if(ValidateObject(i)){
		const SI16 xAdd = i->GetX();
		const SI16 yAdd = i->GetY();
		
		const UI16 multiNum = static_cast<UI16>(i->GetID() - 0x4000);
		if (multiExists(multiNum)) {
			auto structure = SeekMulti(multiNum);
			x1 = static_cast<SI16>(structure.min_x + xAdd);
			x2 = static_cast<SI16>(structure.max_x + xAdd);
			y1 = static_cast<SI16>(structure.min_y + yAdd);
			y2 = static_cast<SI16>(structure.max_y + yAdd);
		}
	}
}
//=============================================================================
auto CMulHandler::multiExists(std::uint16_t multinum) const ->bool{
	return multi_data.exists(multinum);
}
//=============================================================================================
//|	Purpose		-	Checks if multinum/id can be found in multi data.
//o-----------------------------------------------------------------------------------------------o
auto CMulHandler::SeekMulti( std::uint16_t multinum ) const  -> const collection_item &{
	return multi_data.multi(multinum);
}
//=============================================================================================
//|	Purpose		-	Checks if a given tile number falls within the range of tiles loaded in memory
//o-----------------------------------------------------------------------------------------------o
auto CMulHandler::IsValidTile(std::uint16_t tileNum) const ->bool{
	bool retVal = true;
	if( (tileNum == INVALIDID) || (tileNum >= tile_info.sizeArt())) {
		retVal = false;
	}
	
	return retVal;
}

//=============================================================================================
//|	Purpose		-	Fetches data about a specific tile from memory. Non-High Seas version
//o-----------------------------------------------------------------------------------------------o
auto CMulHandler::SeekTile(std::uint16_t tileNum) ->CTile&{
	//7.0.8.2 tiledata and earlier
	if( !IsValidTile( tileNum ) ) {
		Console.warning( oldstrutil::format("Invalid tile access, the offending tile number is %u", tileNum) );
		static CTile emptyTile;
		return emptyTile;
	}
	else{
		return tile_info.art(tileNum);
	}
}
//=============================================================================================
//|	Purpose		-	Fetches data about a specific tile from memory. Non-High Seas version
//o-----------------------------------------------------------------------------------------------o
auto CMulHandler::SeekTile(std::uint16_t tileNum) const ->const CTile&{
	//7.0.8.2 tiledata and earlier
	if( !IsValidTile( tileNum ) ) {
		Console.warning( oldstrutil::format("Invalid tile access, the offending tile number is %u", tileNum) );
		static CTile emptyTile;
		return emptyTile;
	}
	else{
		return tile_info.art(tileNum);
	}
}
//=============================================================================================
//|	Purpose		-	Fetches data about a specific land tile from memory. Non-High Seas version
//o-----------------------------------------------------------------------------------------------o
auto CMulHandler::SeekLand(std::uint16_t landNum) ->CLand&{
	if( landNum == INVALIDID || landNum >=tile_info.sizeTerrain()){
		Console.warning( oldstrutil::format("Invalid land access, the offending land number is %u", landNum) );
		static CLand emptyTile;
		return emptyTile;
	}
	else {
		return tile_info.terrain(landNum);
	}
}
//=============================================================================================
//|	Purpose		-	Fetches data about a specific land tile from memory. Non-High Seas version
//o-----------------------------------------------------------------------------------------------o
auto CMulHandler::SeekLand(std::uint16_t landNum) const ->const CLand&{
	if( landNum == INVALIDID || landNum >=tile_info.sizeTerrain()){
		Console.warning( oldstrutil::format("Invalid land access, the offending land number is %u", landNum) );
		static CLand emptyTile;
		return emptyTile;
	}
	else {
		return tile_info.terrain(landNum);
	}
}
//=============================================================================================
//|	Purpose		-	Fetches map data for a specific map/world
//o-----------------------------------------------------------------------------------------------o
auto CMulHandler::SeekMap(std::int16_t x, std::int16_t y, std::uint8_t worldNumber) ->tile_t& {
	static tile_t badtile ;
	try {
		return uoworlds.at(worldNumber).terrain(x, y);
	}
	catch (...) {
		std::cerr <<"Bad tile access attempted: "<<x<<","<<y<<  static_cast<int>(worldNumber) << std::endl ;
		return badtile ;
	}
}
//=============================================================================================
auto CMulHandler::SeekMap(std::int16_t x, std::int16_t y, std::uint8_t worldNumber) const -> const tile_t& {
	static tile_t badtile ;
	try {
		return uoworlds.at(worldNumber).terrain(x, y);
	}
	catch (...) {
		std::cerr <<"Bad tile access attempted: "<<x<<","<<y<<  static_cast<int>(worldNumber) << std::endl ;
		return badtile ;
	}
}
//=============================================================================================
//|	Purpose		-	Checks if location at given coordinates is considered valid for spawning objects
//|					Also used to verify teleport location for NPCs teleporting back to bounding box
//o-----------------------------------------------------------------------------------------------o
auto CMulHandler::ValidSpawnLocation(std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::uint16_t realm, bool checkWater) ->bool{
	auto rvalue = false ;
	if( InsideValidWorld( x, y, worldNumber ) ){
		// get the tile id of any dynamic tiles at this spot
		if( !DoesDynamicBlock( x, y, z, worldNumber, realm, checkWater, !checkWater, false, false ) ){
			// if there's a static block here in our way, return false
			if( !DoesStaticBlock( x, y, z, worldNumber, checkWater ) ){
				// if the static isn't a surface return false
				if( CheckStaticFlag( x, y, z, worldNumber, ( checkWater ? TF_SURFACE : TF_WET ), true ) ){
					if( !DoesMapBlock( x, y, z, worldNumber, checkWater, !checkWater, false, false ) ){
						rvalue = true ;
					}
				}
			}
		}
	}
	
	return rvalue;
}
//=============================================================================================
//|	Purpose		-	Checks whether given location is valid for house/boat placement
//o-----------------------------------------------------------------------------------------------o
auto CMulHandler::ValidMultiLocation(std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::uint16_t realm, bool checkWater, bool checkOnlyOtherMultis, bool checkOnlyNonMultis, bool checkForRoads) ->std::uint8_t {
	
	if( !InsideValidWorld( x, y, worldNumber ) )
		return 0;
	
	const SI08 elev = Height( x, y, z, worldNumber, realm );
	if( ILLEGAL_Z == elev )
		return 0;
	
	// We don't want the house to be halfway embedded in a hill... or hanging off one for that matter.
	if( z != ILLEGAL_Z ) {
		if( elev - z > MAX_Z_STEP ) {
			return 0;
		}
		else if( z - elev > MAX_Z_FALL ) {
			return 0;
		}
	}
	
	// get the tile id of any dynamic tiles at this spot
	if( !checkOnlyNonMultis && DoesDynamicBlock( x, y, elev, worldNumber, realm, checkWater, false, checkOnlyOtherMultis, checkOnlyNonMultis ) ) {
		return 2;
	}
	
	// if there's a static block here in our way, return false
	if( !checkOnlyOtherMultis && DoesStaticBlock( x, y, elev, worldNumber, checkWater ) ) {
		return 2;
	}
	
	if( DoesMapBlock( x, y, elev, worldNumber, checkWater, false, true, checkForRoads ) ) {
		return 0;
	}
	
	// Check if house placement is allowed in region
	CTownRegion *calcReg = calcRegionFromXY( x, y, worldNumber, realm );
	if(( !calcReg->CanPlaceHouse() && checkWater) || calcReg->IsDungeon() || ( calcReg->IsGuarded() && checkWater ))
		return 3;
	
	// Else, all good!
	return 1;
}
//=============================================================================================
//|	Purpose		-	Returns true if the server has that map in memory
//o-----------------------------------------------------------------------------------------------o
auto CMulHandler::MapExists(std::uint8_t worldNumber ) const ->bool {
	auto iter = uoworlds.find(worldNumber);
	return iter != uoworlds.end() ;
}
//=============================================================================================
//|	Purpose		-	Checks if a given location is within the bounds of the specified world
//o-----------------------------------------------------------------------------------------------o
auto CMulHandler::InsideValidWorld(std::int16_t x, std::int16_t y, std::uint8_t worldNumber) const ->bool {
	auto rvalue = false ;
	try {
		auto [width,height] = uoworlds.at(worldNumber).size();
		if ((x>=0 && x<width) && (y>=0 && y<height)){
			rvalue = true ;
		}
	}
	catch(...) {
		// Do nothing, all rady set to false
	}
	return rvalue ;
}

//=============================================================================================
//|	Purpose		-	Returns count of how many maps have been loaded
//o-----------------------------------------------------------------------------------------------o
auto CMulHandler::MapCount() const ->std::uint8_t {
	// This ASSUMES the map numbers are consequtive.  You dont have say map 0,1,3 <<<< missing 2
	return static_cast<std::uint8_t>(uoworlds.size());
}

//===============================================================================================
// tileinfo
//===============================================================================================
//===============================================================================================

//===============================================================================================
tileinfo::tileinfo(const std::string &filename):isHS_format(false){
	if (!filename.empty()){
		this->load(filename) ;
	}
}
//===============================================================================================
auto tileinfo::load(const std::string &filename) ->bool {
	auto rvalue = false ;
	art_data.clear() ;
	terrain_data.clear() ;
	isHS_format =false ;
	auto path = std::filesystem::path(filename) ;
	if (std::filesystem::exists(path)) {
		if (std::filesystem::file_size(path)>= hssize) {
			isHS_format = true ;;
		}
		auto input = std::ifstream(filename,std::ios::binary) ;
		if (input.is_open()){
			rvalue = true ;
			processTerrain(input) ;
			processArt(input) ;
		}
	}
	return rvalue ;
}
//===============================================================================================
auto tileinfo::terrain(std::uint16_t tileid) const -> const CLand& {
	return terrain_data[tileid] ;
}
//===============================================================================================
auto tileinfo::terrain(std::uint16_t tileid)  ->  CLand& {
	return terrain_data[tileid] ;
}
//===============================================================================================
auto tileinfo::art(std::uint16_t tileid) const -> const CTile& {
	return art_data[tileid] ;
}
//===============================================================================================
auto tileinfo::art(std::uint16_t tileid)  ->  CTile& {
	return art_data[tileid] ;
}
//===============================================================================================
auto tileinfo::sizeTerrain() const -> size_t {
	return terrain_data.size();
}
//===============================================================================================
auto tileinfo::sizeArt() const -> size_t {
	return art_data.size() ;
}
//===============================================================================================
auto tileinfo::collectionTerrain() const -> const std::vector<CLand>&{
	return terrain_data;
}
//===============================================================================================
auto tileinfo::collectionTerrain() ->std::vector<CLand> &{
	return terrain_data;
}

//===============================================================================================
auto tileinfo::collectionArt() const -> const std::vector<CTile>& {
	return  art_data;
}
//===============================================================================================
auto tileinfo::collectionArt() ->std::vector<CTile>& {
	return  art_data;
}

//===============================================================================================
auto tileinfo::processTerrain(std::ifstream &input) ->void {
	terrain_data.reserve(0x4000);
	std::uint32_t value32 = 0 ;
	std::uint64_t value64 =  0;
	std::array<char,20> string_buffer ;
	string_buffer.fill(0);
	for (auto i=0 ; i< 0x4000;i++){
		// We have to get rid of the header on blocks of information
		// HS has the firt entry messed up
		if (isHS_format) {
			if ( (((i & 0x1F)==0) && (i>0)) || (i == 1)){
				input.read(reinterpret_cast<char*>(&value32),4); // read off the group header
			}
			
		}
		else {
			if ( (i & 0x1f) == 0) {
				input.read(reinterpret_cast<char*>(&value32),4); // read off the group header
			}
		}
		// now create the info_t we will use
		auto info = CLand();
		// read off the flags, 32 bit on pre-HS, 64 bit after
		if (isHS_format) {
			input.read(reinterpret_cast<char*>(&value64),8);
			info.flags = std::bitset<64>(value64);

		}
		
		else{
			input.read(reinterpret_cast<char*>(&value32),4);
			info.flags = std::bitset<64>(value32);
		}
		// only other thing for terrain is the texture
		// and name
		input.read(reinterpret_cast<char*>(&info.textureID),2);
		input.read(string_buffer.data(),20);
		if (input.gcount()==20){
			// We need to find the "0", if any
			auto iter = std::find(string_buffer.begin(),string_buffer.end(),0);
			info.name = std::string(string_buffer.begin(),iter);
			terrain_data.push_back(std::move(info));
			
		}
		else {
			break;
		}
	}

}
//===============================================================================================
auto tileinfo::processArt(std::ifstream &input) ->void {
	art_data.reserve(0xFFFF);
	std::uint32_t value32 = 0 ;
	std::uint64_t value64 =  0;
	std::array<char,20> string_buffer ;
	string_buffer.fill(0);
	if ( !(input.eof() || input.bad())){
		auto loopcount = 0 ;
		do {
			if ( (loopcount & 0x1f) == 0) {
				input.read(reinterpret_cast<char*>(&value32),4); // read off the group header
			}
			auto info = CTile();
			if (isHS_format) {
				input.read(reinterpret_cast<char*>(&value64),8);
				info.flags = std::bitset<64>(value64);

				
			}
			else {
				input.read(reinterpret_cast<char*>(&value32),4);
				info.flags = std::bitset<64>(value32);

			}
			input.read(reinterpret_cast<char*>(&info.weight),1);
			input.read(reinterpret_cast<char*>(&info.layer),1);
			//misc data
			input.read(reinterpret_cast<char*>(&info.unknown1),2);
			//Second unkown
			input.read(reinterpret_cast<char*>(&info.unknown2),1);
			// Quantity
			input.read(reinterpret_cast<char*>(&info.quantity),1);
			// Animation
			input.read(reinterpret_cast<char*>(&info.animation),2);
			
			//Third a byte
			input.read(reinterpret_cast<char*>(&info.unknown3),1);
			
			// Hue/Value
			input.read(reinterpret_cast<char*>(&info.hue),1);
			//stacking offset/value  = 16 bits, but UOX3 doesn't know that, so two eight bit unkowns
			input.read(reinterpret_cast<char*>(&info.unknown4),1);
			input.read(reinterpret_cast<char*>(&info.unknown5),1);
			// Height
			input.read(reinterpret_cast<char*>(&info.height),1);
			// and name
			input.read(string_buffer.data(),20);
			if ((input.gcount()==20) && input.good()){
				// We need to find the "0", if any
				auto iter = std::find(string_buffer.begin(),string_buffer.end(),0);
				info.name = std::string(string_buffer.begin(),iter);
				art_data.push_back(std::move(info));
			}
			loopcount++;
			
		}while( (!input.eof()) && input.good());
		
	}

}
//===============================================================================================
auto tileinfo::totalMemory() const ->size_t {
	auto flag = isHS_format?8:4 ;
	auto terrain = flag + 20 + 2 ;
	auto art = flag + 20 + 23 ;
	return static_cast<size_t>((terrain*terrain_data.size()) + (art * art_data.size()) );
}
//====================================================================================================
// terrainblock
//====================================================================================================
//=========================================================
terrainblock::terrainblock(std::uint8_t *data,const tileinfo *info){
	if (data != nullptr) {
		load(data,info) ;
	}
}
//=========================================================
auto terrainblock::load(std::uint8_t *data,const tileinfo *info) ->void {
	if (data !=nullptr){
		data +=4 ;
		for (auto y= 0 ; y < 8 ;++y){
			for (auto x= 0 ; x < 8 ;++x){
				auto tileid = std::uint16_t(0) ;
				// we copy here, because of potential alignment issues
				std::copy(data,data+2,reinterpret_cast<std::uint8_t*>(&(tileid)));
				_tiles[x][y].tileid = tileid ;
				if (info){
					_tiles[x][y].terrainInfo = &info->terrain(tileid);
				}
				_tiles[x][y].altitude = *(reinterpret_cast<std::int8_t*>(data+2));
				data += 3 ; // advance data three bytes ;
			}
		}
	}
}

//=========================================================
auto terrainblock::tile(int x, int y) ->tile_t& {
	return _tiles.at(x).at(y) ;
}
//=========================================================
auto terrainblock::tile(int x, int y) const ->const tile_t& {
	return _tiles.at(x).at(y) ;
}

//================================================================================
// artblock
//================================================================================

//=========================================================
artblock::artblock(int length,std::uint8_t *data,const tileinfo *info){
	if (data != nullptr) {
		load(length,data,info) ;
	}
}

//=========================================================
auto artblock::tile(int x, int y) ->std::vector<tile_t>& {
	return _tiles.at(x).at(y) ;
}
//=========================================================
auto artblock::tile(int x, int y) const ->const std::vector<tile_t>& {
	return _tiles.at(x).at(y) ;
}



//=========================================================
auto artblock::clear() ->void {
	for (auto &row : _tiles){
		for (auto &tile: row){
			tile.clear();
		}
	}
}
//==========================================================
auto artblock::load(int length, std::istream &input,const tileinfo *info) ->void{
	this->clear() ;
	auto count = length/7 ;
	auto x = std::uint8_t(0);
	auto y = std::uint8_t(0) ;
	auto alt = std::int8_t(0) ;
	for (auto i=0; i<count ;++i){
		auto arttile = tile_t() ;
		arttile.type = tiletype_t::art ;
		input.read(reinterpret_cast<char*>(&arttile.tileid),2) ;
		input.read(reinterpret_cast<char*>(&x),1);
		input.read(reinterpret_cast<char*>(&y),1);
		input.read(reinterpret_cast<char*>(&alt),1);
		arttile.altitude = alt ;
		input.read(reinterpret_cast<char*>(&arttile.static_hue),2);
		if (info){
			arttile.artInfo = &info->art(arttile.tileid);
		}
		tile(x,y).push_back(std::move(arttile));
		
	}
}
//==========================================================
auto artblock::load(int length,std::uint8_t *data,const tileinfo *info) ->void{
	this->clear() ;
	
	auto count = length/7 ;
	auto x = std::uint8_t(0);
	auto y = std::uint8_t(0) ;
	for (auto i=0; i<count ;++i){
		auto arttile = tile_t() ;
		arttile.type = tiletype_t::art ;
		std::copy(data,data+2,reinterpret_cast<char*>(&arttile.tileid)) ;
		x = *(data+2) ;
		y = *(data+3);
		arttile.altitude = static_cast<int>(*(reinterpret_cast<std::int8_t*>(data+4)));
		std::copy(data+5,data+7,reinterpret_cast<char*>(&arttile.static_hue)) ;
		if (info){
			arttile.artInfo = &info->art(arttile.tileid);
		}
		
		tile(x,y).push_back(std::move(arttile));
		data +=7 ;
	}
}

//================================================================================
// ultimamap
//================================================================================
//===============================================================================
ultimamap::ultimamap() :_diffcount(0),_diffterrain(0),_width(0),_height(0),tile_info(nullptr){
}
//=========================================================
ultimamap::ultimamap(int mapnum,int width,int height,const tileinfo *info):ultimamap(){
	_width = width ;
	_height= height ;
	tile_info = info ;
	_mapnum = mapnum ;
	auto size = _mapsizes.at(mapnum) ;
	if (_width == 0){
		_width = size.first ;
	}
	if (_height == 0){
		_height = size.second ;
	}
	_terrain.resize( (_width/8) * (_height/8) );
	_art.resize(_terrain.size()) ;
	isUOP = true ;
}
//=========================================================
auto ultimamap::width() const ->int {
	return _width ;
}
//=========================================================
auto ultimamap::height() const ->int {
	return _height ;
}
//=========================================================
auto ultimamap::setSize(int width, int height) ->void {
	_width = width ;
	_height = height ;
	_terrain.resize( (_width/8) * (_height/8) );
	_art.resize(_terrain.size()) ;
}
//=========================================================
auto ultimamap::size() const ->std::pair<int,int> {
	return std::make_pair(static_cast<int>(_width),static_cast<int>(_height));
}

//=========================================================
auto ultimamap::diffArt() const ->int {
	return _diffcount ;
}
//=========================================================
auto ultimamap::diffTerrain() const ->int {
	return _diffterrain ;
}
//===========================================================
auto ultimamap::loadUOPTerrainFile(const std::string &filename) ->bool {
	auto hash = this->format("build/map%ilegacymul/%s", _mapnum,"%.8u.dat");
	//auto filename = this->format("map%iLegacyMUL.uop",_mapnum);
	isUOP = loadUOP(filename, 0x300, hash);
	return isUOP ;
}
//===========================================================
auto ultimamap::loadMulTerrainFile(const std::string &filename) ->bool {
	auto rvalue = false ;
	auto mul = std::ifstream(filename,std::ios::binary) ;
	if (mul.is_open()){
		rvalue = true;
		isUOP = false ;
		auto data = std::vector<std::uint8_t>(196,0);
		auto blocknum = 0 ;
		while (mul.good() && !mul.eof()) {
			mul.read(reinterpret_cast<char*>(data.data()),196);
			if (mul.gcount() == 196) {
				loadTerrainBlock(blocknum, data.data());
			}
			blocknum++;
		}
	}
	return rvalue ;
}
//=========================================================
auto ultimamap::loadArt(const std::string &mulfile, const std::string &idxfile)->bool {
	auto idx = std::ifstream(idxfile,std::ios::binary);
	auto rvalue = idx.is_open();
	if (rvalue){
		auto mul = std::ifstream(mulfile,std::ios::binary);
		rvalue = mul.is_open();
		if (rvalue) {
			auto offset = std::uint32_t(0) ;
			auto length = std::int32_t(0) ;
			auto extra = std::uint32_t(0) ;
			auto block = 0 ;
			while (idx.good()&& !idx.eof()){
				idx.read(reinterpret_cast<char*>(&offset),4);
				idx.read(reinterpret_cast<char*>(&length),4);
				idx.read(reinterpret_cast<char*>(&extra),4);
				if ((length >0) && (idx.gcount()==4)) {
					mul.seekg(offset,std::ios::beg);
					if (block < _art.size()){
						_art[block].load(length, mul,tile_info);
					}
				}
				++block ;
			}
		}
	}
	return rvalue ;
}
//=========================================================
auto ultimamap::applyDiff(const std::string &difflpath,const std::string &diffipath, const std::string &diffpath) ->int {
	_diffcount = 0 ;
	auto diffblock = std::ifstream(difflpath,std::ios::binary) ;
	if (diffblock.is_open()){
		auto diffidx = std::ifstream(diffipath,std::ios::binary) ;
		if (diffidx.is_open()){
			auto diff = std::ifstream(diffpath,std::ios::binary) ;
			if (diff.is_open()){
				auto blocknum = std::uint32_t(0) ;
				auto offset = std::uint32_t(0);
				auto length = std::int32_t(0) ;
				auto extra = std::uint32_t(0);
				while (diffblock.good() && !diffblock.eof()){
					diffblock.read(reinterpret_cast<char*>(&blocknum),4);
					diffidx.read(reinterpret_cast<char*>(&offset),4);
					diffidx.read(reinterpret_cast<char*>(&length),4);
					diffidx.read(reinterpret_cast<char*>(&extra),4);
					if ((diffblock.gcount()==4) && (diffidx.gcount()==4)){
						if (blocknum< _art.size()){
							if (length >0){
								diff.seekg(offset,std::ios::beg);
								_art[blocknum].load(length, diff,tile_info);
							}
							else {
								_art[blocknum].clear();
							}
						}
						++_diffcount;
					}
				}
			}
		}
		
	}
	return _diffcount ;
}
//=========================================================
auto ultimamap::applyTerrainDiff(const std::string &difflpath,const std::string &diffpath) ->int {
	auto diffblock = std::ifstream(difflpath,std::ios::binary) ;
	_diffterrain = 0 ;
	if (diffblock.is_open()) {
		auto diff = std::ifstream(diffpath,std::ios::binary) ;
		if (diff.is_open()) {
			auto blocknum = std::uint32_t(0) ;
			auto data = std::vector<std::uint8_t>(196,0) ;
			while (diffblock.good() && !diffblock.eof()){
				diffblock.read(reinterpret_cast<char*>(&blocknum),4) ;
				if (diffblock.gcount()==4) {
					diff.read(reinterpret_cast<char*>(data.data()),196);
					if (diff.gcount()==196){
						_terrain[blocknum].load(data.data(),tile_info);
						_diffterrain++;
					}
				}
			}
		}
	}
	return _diffterrain;
	
}
//=========================================================
auto ultimamap::calcBlock(int x,int y) const ->int {
	// How many blocks is x across?
	return  ((x/8)*(_height/8)) + (y/8) ;
}
//=========================================================
auto ultimamap::calcXYOffset(int block) const ->std::pair<int,int> {
	auto x = (block / (_height/8)) * 8 ;
	auto y = (block % (_height/8)) * 8;
	return std::make_pair(x, y);
}
//=========================================================
auto ultimamap::blockAndIndexFor(int x, int y) const ->std::tuple<int, int, int>{
	auto block = calcBlock(x, y);
	auto offset = calcXYOffset(block);
	return std::make_tuple(block,x-offset.first,y-offset.second);
}
//=========================================================
auto ultimamap::processEntry(std::size_t entry, std::size_t index, std::vector<std::uint8_t> &data) ->bool {
	auto count = data.size()/196 ;
	auto block = (static_cast<int>(index) * 0xC4000)/196 ;
	for (auto i=0 ;i<count;++i){
		auto ptr = data.data()+(i*196);
		if (block < _terrain.size()){
			loadTerrainBlock(block, ptr);
		}
		++block ;
	}
	return true ;
}
//=========================================================
auto ultimamap::loadTerrainBlock(int blocknum,std::uint8_t *data)->void {
	_terrain[blocknum].load(data,tile_info);
}

//=========================================================
auto ultimamap::terrain(int x, int y) const ->const tile_t& {
	auto [blocknum,xoffset,yoffset] = blockAndIndexFor(x, y);
	return _terrain[blocknum].tile(xoffset, yoffset);
}
//=========================================================
auto ultimamap::terrain(int x, int y)  ->tile_t& {
	auto [blocknum,xoffset,yoffset] = blockAndIndexFor(x, y);
	return _terrain[blocknum].tile(xoffset, yoffset);
	
}
//=========================================================
auto ultimamap::art(int x, int y) const ->const std::vector<tile_t>& {
	auto [blocknum,xoffset,yoffset] = blockAndIndexFor(x, y);
	return _art[blocknum].tile(xoffset, yoffset);
	
}
//=========================================================
auto ultimamap::art(int x, int y)  ->std::vector<tile_t>& {
	auto [blocknum,xoffset,yoffset] = blockAndIndexFor(x, y);
	return _art[blocknum].tile(xoffset, yoffset);
}
