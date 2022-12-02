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

using namespace std::string_literals;

CMulHandler *Map				= nullptr;

//const UI16 LANDDATA_SIZE		= 0x4000; //(512 * 32)

/*!
 ** New rewrite plans, its way confusing having every function in here using 'Height' to mean
 ** elevation or actual height of a tile.  Here's the new renaming plan:
 ** Elevation - the z value of the bottom edge of an item
 ** Height - the height of the item, it is independant of the elevation, like a person is 6ft tall
 ** Top - the z value of the top edge of an item, is always Elevation + Height of item
 */

//==================================================================================================
// CMulHandler
//==================================================================================================
//==================================================================================================

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::LoadMapsDFN()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads maps.dfn to see which map files to actually load into memory later
//o------------------------------------------------------------------------------------------------o
auto CMulHandler::LoadMapsDFN( const std::string &uodir ) -> std::map<int, MapDfnData_st>
{
	auto entrycount = FileLookup->CountOfEntries( maps_def );
	std::map<int, MapDfnData_st> results;
	auto uopath = std::filesystem::path( uodir );
	for( size_t i = 0; i < entrycount; i++ )
	{
		auto toFind = FileLookup->FindEntry( "MAP "s + std::to_string( i ), maps_def );
		if( toFind == nullptr )
			break;

		auto entry = MapDfnData_st();
		for( const auto &sec :toFind->collection() )
		{
			auto uTag = oldstrutil::upper( sec->tag );
			auto data = oldstrutil::trim( oldstrutil::removeTrailing( sec->data ));
			switch( uTag.data()[0] )
			{
				case 'M':
					if( uTag == "MAP" )
					{
						entry.mapFile = ( uopath / std::filesystem::path( data ));
					}
					else if( uTag == "MAPUOPWRAP" )
					{
						entry.mapUop = ( uopath / std::filesystem::path( data ));
					}
					else if( cwmWorldState->ServerData()->MapDiffsEnabled() )
					{
						if( uTag == "MAPDIFF" )
						{
							entry.mapDiff = ( uopath / std::filesystem::path( data ));
						}
						else if( uTag == "MAPDIFFLIST" )
						{
							entry.mapDiffl = ( uopath / std::filesystem::path( data ));
						}
					}
					break;
				case 'S':
					if( uTag == "STATICS" )
					{
						entry.staMul = ( uopath / std::filesystem::path( data ));
					}
					else if( uTag == "STAIDX" )
					{
						entry.staIdx = ( uopath / std::filesystem::path( data ));
					}
					else if( cwmWorldState->ServerData()->MapDiffsEnabled() )
					{
						if( uTag == "STATICSDIFF" )
						{
							entry.staDiff = ( uopath / std::filesystem::path( data ));
						}
						else if( uTag == "STATICSDIFFLIST" )
						{
							entry.staDiffl = ( uopath / std::filesystem::path( data ));
						}
						else if( uTag == "STATICSDIFFINDEX" )
						{
							entry.staDiffi = ( uopath / std::filesystem::path( data ));
						}
					}
					break;
				case 'X':
					if( uTag == "X" )
					{
						entry.width = std::stoi( data );
					}
					break;
				case 'Y':
					if( uTag == "Y" )
					{
						entry.height = std::stoi( data );
					}
					break;
			}
			results.insert_or_assign( i, entry );
		}
	}
	return results;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::Load()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Load UO data (tiledata, maps, statics, multis)
//o------------------------------------------------------------------------------------------------o
auto CMulHandler::Load() -> void
{
	auto uodir = cwmWorldState->ServerData()->Directory( CSDDP_DATA );
	auto mapinfo = LoadMapsDFN( uodir );
	Console.PrintSectionBegin();
	Console << "Loading uodata (for maps, only mapfile shown)..." << myendl << "(If they don't open, fix your paths in uox.ini or filenames in maps.dfn)" << myendl;
	LoadTileData( uodir );
	LoadDFNOverrides();
	LoadMapAndStatics( mapinfo );
	if( uoWorlds.empty() )
	{
		Console.Error( " Fatal Error: No maps found" );
		Console.Error( " Check the settings for DATADIRECTORY in uox.ini" );
		Shutdown( FATAL_UOX3_MAP_NOT_FOUND );
	}
	LoadMultis( uodir );
	FileLookup->Dispose( maps_def );
	Console.PrintSectionBegin();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::LoadTileData()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads tiledata file into memory
//o------------------------------------------------------------------------------------------------o
auto CMulHandler::LoadTileData( const std::string& uodir ) -> void
{
	std::string lName = uodir + "tiledata.mul"s;
	Console << "\t" << lName << "\t";

	if( !tileInfo.LoadTiles( lName ))
	{
		Console.PrintFailed();
		Shutdown( FATAL_UOX3_TILEDATA_NOT_FOUND );
	}
	Console.PrintDone();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::LoadDFNOverrides()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Override flags/properties for tiles based on DFNDATA/MAPS/tiles.dfn
//o------------------------------------------------------------------------------------------------o
auto CMulHandler::LoadDFNOverrides() -> void
{
	std::uint16_t entryNum;
	for( auto &mapScp : FileLookup->ScriptListings[maps_def] )
	{
		if( mapScp == nullptr )
			continue;

		for( const auto &[entryName, toScan]: mapScp->collection() )
		{
			if( toScan == nullptr )
				continue;

			entryNum		= oldstrutil::value<std::uint16_t>( oldstrutil::extractSection( entryName, " ", 1, 1 ));
			auto titlePart 	= oldstrutil::upper( oldstrutil::extractSection( entryName, " ", 0, 0 ));
			// have we got an entry starting with TILE ?
			if( titlePart == "TILE" && entryNum )
			{
				if(( entryNum == INVALIDID ) || ( entryNum < tileInfo.SizeArt() ))
					continue;

				auto tile = &tileInfo.ArtInfo( entryNum );
				if( tile )
				{
					for( const auto &sec : toScan->collection() )
					{
						auto UTag = oldstrutil::upper( sec->tag );
						auto data = oldstrutil::trim( oldstrutil::removeTrailing( sec->data, "//" ));
						// CTile properties
						if( UTag == "WEIGHT" )
						{
							tile->Weight( static_cast<UI08>( std::stoul( data, nullptr, 0 )));
						}
						else if( UTag == "HEIGHT" )
						{
							tile->Height( static_cast<SI08>( std::stoi( data, nullptr, 0 )));
						}
						else if( UTag == "LAYER" )
						{
							tile->Layer( static_cast<SI08>( std::stoi( data, nullptr, 0 )));
						}
						else if( UTag == "HUE" )
						{
							tile->Hue( static_cast<UI08>( std::stoul( data, nullptr, 0 )));
						}
						else if( UTag == "QUANTITY" )
						{
							tile->Quantity( static_cast<UI08>( std::stoul( data, nullptr, 0 )));
						}
						else if( UTag == "ANIMATION" )
						{
							tile->Animation( static_cast<UI16>( std::stoul( data, nullptr, 0 )));
						}
						else if( UTag == "NAME" )
						{
							tile->Name( data );
						}
						// BaseTile Flag 1
						else if( UTag == "ATFLOORLEVEL" )
						{
							tile->SetFlag( TF_FLOORLEVEL, ( std::stoi( data, nullptr, 0 ) != 0 ));
						}
						else if( UTag == "HOLDABLE" )
						{
							tile->SetFlag( TF_HOLDABLE, ( std::stoi( data, nullptr, 0 ) != 0 ));
						}
						else if( UTag == "SIGNGUILDBANNER" )
						{
							tile->SetFlag( TF_TRANSPARENT, ( std::stoi( data, nullptr, 0 ) != 0 ));
						}
						else if( UTag == "WEBDIRTBLOOD" )
						{
							tile->SetFlag( TF_TRANSLUCENT, ( std::stoi( data, nullptr, 0 ) != 0 ));
						}
						else if( UTag == "WALLVERTTILE" )
						{
							tile->SetFlag( TF_WALL, ( std::stoi( data, nullptr, 0 ) != 0 ));
						}
						else if( UTag == "DAMAGING" )
						{
							tile->SetFlag( TF_DAMAGING, ( std::stoi( data, nullptr, 0 ) != 0 ));
						}
						else if( UTag == "BLOCKING" )
						{
							tile->SetFlag( TF_BLOCKING, ( std::stoi( data, nullptr, 0 ) != 0 ));
						}
						else if( UTag == "LIQUIDWET" )
						{
							tile->SetFlag( TF_WET, ( std::stoi( data, nullptr, 0 ) != 0 ));
						}
						
						// BaseTile Flag 2
						else if( UTag == "UNKNOWNFLAG1" )
						{
							tile->SetFlag( TF_UNKNOWN1, ( std::stoi( data, nullptr, 0 ) != 0 ));
						}
						else if( UTag == "STANDABLE" )
						{
							tile->SetFlag( TF_SURFACE, ( std::stoi( data, nullptr, 0 ) != 0 ));
						}
						else if( UTag == "CLIMBABLE" )
						{
							tile->SetFlag( TF_CLIMBABLE, ( std::stoi( data, nullptr, 0 ) != 0 ));
						}
						else if( UTag == "STACKABLE" )
						{
							tile->SetFlag( TF_STACKABLE, ( std::stoi( data, nullptr, 0 ) != 0 ));
						}
						else if( UTag == "WINDOWARCHDOOR" )
						{
							tile->SetFlag( TF_WINDOW, ( std::stoi( data, nullptr, 0 ) != 0 ));
						}
						else if( UTag == "CANNOTSHOOTTHRU" )
						{
							tile->SetFlag( TF_NOSHOOT, ( std::stoi( data, nullptr, 0 ) != 0 ));
						}
						else if( UTag == "DISPLAYASA" )
						{
							tile->SetFlag( TF_DISPLAYA, ( std::stoi( data, nullptr, 0 ) != 0 ));
						}
						else if( UTag == "DISPLAYASAN" )
						{
							tile->SetFlag( TF_DISPLAYAN, ( std::stoi( data, nullptr, 0 ) != 0 ));
						}
						
						// BaseTile Flag 3
						else if( UTag == "DESCRIPTIONTILE" )
						{
							tile->SetFlag( TF_DESCRIPTION, ( std::stoi( data, nullptr, 0 ) != 0 ));
						}
						else if( UTag == "FADEWITHTRANS" )
						{
							tile->SetFlag( TF_FOLIAGE, ( std::stoi( data, nullptr, 0 ) != 0 ));
						}
						else if( UTag == "PARTIALHUE" )
						{
							tile->SetFlag( TF_PARTIALHUE, ( std::stoi( data, nullptr, 0 ) != 0 ));
						}
						else if( UTag == "UNKNOWNFLAG2" )
						{
							tile->SetFlag( TF_UNKNOWN2, ( std::stoi( data, nullptr, 0 ) != 0 ));
						}
						else if( UTag == "MAP" )
						{
							tile->SetFlag( TF_MAP, ( std::stoi( data, nullptr, 0 ) != 0 ));
						}
						else if( UTag == "CONTAINER" )
						{
							tile->SetFlag( TF_CONTAINER, ( std::stoi( data, nullptr, 0 ) != 0 ));
						}
						else if( UTag == "EQUIPABLE" )
						{
							tile->SetFlag( TF_WEARABLE, ( std::stoi( data, nullptr, 0 ) != 0 ));
						}
						else if( UTag == "LIGHTSOURCE" )
						{
							tile->SetFlag( TF_LIGHT, ( std::stoi( data, nullptr, 0 ) != 0 ));
						}
						// BaseTile Flag 4
						else if( UTag == "ANIMATED" )
						{
							tile->SetFlag( TF_ANIMATED, (std::stoi(data, nullptr, 0) != 0 ));
						}
						else if( UTag == "NODIAGONAL" )
						{
							tile->SetFlag( TF_NODIAGONAL, ( std::stoi( data, nullptr, 0 ) != 0 ));
						}
						else if( UTag == "UNKNOWNFLAG3" )
						{
							tile->SetFlag( TF_UNKNOWN3, ( std::stoi( data, nullptr, 0 ) != 0 ));
						}
						else if( UTag == "WHOLEBODYITEM" )
						{
							tile->SetFlag( TF_ARMOR, ( std::stoi( data, nullptr, 0 ) != 0 ));
						}
						else if( UTag == "WALLROOFWEAP" )
						{
							tile->SetFlag( TF_ROOF, ( std::stoi( data, nullptr, 0 ) != 0 ));
						}
						else if( UTag == "DOOR" )
						{
							tile->SetFlag( TF_DOOR, ( std::stoi( data, nullptr, 0 ) != 0 ));
						}
						else if( UTag == "CLIMBABLEBIT1" )
						{
							tile->SetFlag( TF_STAIRBACK, ( std::stoi( data, nullptr, 0 ) != 0 ));
						}
						else if( UTag == "CLIMBABLEBIT2" )
						{
							tile->SetFlag( TF_STAIRRIGHT, ( std::stoi( data, nullptr, 0 ) != 0 ));
						}
					}
				}
			}
			else if( titlePart == "LAND" && entryNum )
			{	// let's not deal with this just yet
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::LoadMapAndStatics()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads map and statics files into memory
//o------------------------------------------------------------------------------------------------o
auto CMulHandler::LoadMapAndStatics( const std::map<int, MapDfnData_st> &info ) -> void
{
	for( const auto &[mapNum, dfndata]:info )
	{
		uoWorlds.insert_or_assign( mapNum, UltimaMap( mapNum, dfndata.width, dfndata.height, &tileInfo ));
		auto rValue = false;

		if( std::filesystem::exists( dfndata.mapUop ))
		{
			Console << "\t" << dfndata.mapUop.string() << "(/" << dfndata.mapUop.filename().string() << ")\t\t";
			rValue = uoWorlds[mapNum].LoadUOPTerrainFile( dfndata.mapUop.string() );
		}
		else
		{
			Console << "\t" << dfndata.mapFile.string() << "(/" << dfndata.mapFile.filename().string() << ")\t\t";
			rValue = uoWorlds[mapNum].LoadMulTerrainFile( dfndata.mapFile.string() );
			if( rValue )
			{
				uoWorlds[mapNum].ApplyTerrainDiff( dfndata.mapDiffl.string(), dfndata.mapDiff.string() );
			}
		}
		if( rValue )
		{
			rValue = uoWorlds[mapNum].LoadArt( dfndata.staMul.string(), dfndata.staIdx.string() );
			if( rValue )
			{
				uoWorlds[mapNum].ApplyDiff( dfndata.staDiffl.string(), dfndata.staDiffi.string(), dfndata.staDiff.string() );
			}
			else
			{
				// this is interesting, we found the terrain, but not the art.  Regardless, lets delete it
				auto iter = uoWorlds.find( mapNum );
				if( iter != uoWorlds.end() )
				{
					uoWorlds.erase( iter );
				}
			}
		}
		if( !rValue )
		{
			Console.PrintFailed();
		}
		else
		{
			Console.PrintDone();
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::LoadMultis()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads multi files into memory
//o------------------------------------------------------------------------------------------------o
auto CMulHandler::LoadMultis( const std::string& uodir ) -> void
{
	// now main memory multiItems
	Console << "Loading Multis....  ";
	// Odd we do no check?
	if( !multiData.LoadMultiCollection( std::filesystem::path( uodir ), &tileInfo ))
	{
		Console.PrintFailed();
		Shutdown( FATAL_UOX3_MULTI_DATA_NOT_FOUND );
	}
	Console.PrintDone();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::ArtAt()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Try to find and return map tile at specific location in a world
//o------------------------------------------------------------------------------------------------o
auto CMulHandler::ArtAt( std::int16_t x, std::int16_t y, std::uint8_t world ) -> std::vector<Tile_st>&
{
	static std::vector<Tile_st> empty;
	try
	{
		return uoWorlds.at( world ).ArtAt( x, y );
	}
	catch(...)
	{
		return empty;
	}
}
//=-----------------------------------------------------------------------------------------------=
auto CMulHandler::ArtAt( std::int16_t x, std::int16_t y, std::uint8_t world ) const -> const std::vector<Tile_st>&
{
	static std::vector<Tile_st> empty;
	try
	{
		return uoWorlds.at( world ).ArtAt( x, y );
	}
	catch(...)
	{
		return empty;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::SizeOfMap()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Return x/y size of specified world
//o------------------------------------------------------------------------------------------------o
auto CMulHandler::SizeOfMap( std::uint8_t worldNumber ) const -> std::pair<int, int>
{
	return uoWorlds.at( worldNumber ).Size();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::DiffCountForMap()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Return count of map differences for specified world
//o------------------------------------------------------------------------------------------------o
auto CMulHandler::DiffCountForMap( std::uint8_t worldNumber ) const -> std::pair<int, int>
{
	return std::make_pair( uoWorlds.at( worldNumber ).DiffArt(), uoWorlds.at( worldNumber ).DiffTerrain() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::MultiHeight()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Return the height of a multi item at the given x, y.
//o------------------------------------------------------------------------------------------------o
auto CMulHandler::MultiHeight( CItem *i, std::int16_t x, std::int16_t y, std::int8_t oldZ, std::int8_t maxZ, bool checkHeight ) -> std::int8_t
{	
	UI16 multiId = static_cast<UI16>(i->GetId() - 0x4000);
	
	SI08 mHeight = ILLEGAL_Z;
	SI08 tmpTop = ILLEGAL_Z;

	if( !MultiExists( multiId ))
		return mHeight;

	const SI16 baseX = i->GetX();
	const SI16 baseY = i->GetY();
	const SI08 baseZ = i->GetZ();
	for( auto &multi : SeekMulti( multiId ).items )
	{
		if(( checkHeight || multi.flag ) && ( baseX + multi.offsetX ) == x && ( baseY + multi.offsetY ) == y )
		{
			if( checkHeight )
			{
				// Returns height of highest point of multi
				tmpTop = static_cast<SI08>( baseZ + multi.altitude );
				if(( tmpTop <= oldZ + maxZ ) && tmpTop > oldZ && tmpTop > mHeight )
				{
					mHeight = tmpTop;
				}
			}
			else
			{
				tmpTop = static_cast<SI08>( baseZ + multi.altitude );
				if( std::abs( tmpTop - oldZ ) <= maxZ )
				{
					mHeight = tmpTop + multi.info->ClimbHeight();
					break;
				}
			}
		}
	}
	
	return mHeight;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::MultiTile()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns ID of tile in multi at specified coordinates
//o------------------------------------------------------------------------------------------------o
auto CMulHandler::MultiTile( CItem *i, std::int16_t x, std::int16_t y, std::int8_t oldz, bool checkVisible ) -> std::uint16_t
{
	auto rValue = std::uint16_t();
	if( !i->CanBeObjType( OT_MULTI ))
		return 0;

	UI16 multiId = static_cast<UI16>( i->GetId() - 0x4000 );
	[[maybe_unused]] SI32 length = 0;

	if( !MultiExists( multiId ))
	{
		Console << "CMulHandler::MultiTile->Bad length in multi file. Avoiding stall." << myendl;
		auto map1 = Map->SeekMap( i->GetX(), i->GetY(), i->WorldNumber() );
		if( map1.CheckFlag( TF_WET )) // is it water?
		{
			i->SetId( 0x4001 );
		}
		else
		{
			i->SetId( 0x4064 );
		}
		length = 0;
	}
	else
	{
		// Loop through each item that makes up the multi
		// If any of those items intersect with area were are checking, return the ID of that tile
		for( auto &multi : SeekMulti( multiId ).items )
		{
			if( !checkVisible )
			{
				if(( i->GetX() + multi.offsetX == x ) && ( i->GetY() + multi.offsetY == y ))
				{
					rValue = multi.tileId;
					break;
				}
			}
			else if( multi.flag > 0 && ( std::abs( i->GetZ() + multi.altitude - oldz ) <= 1 ))
			{
				if(( i->GetX() + multi.offsetX == x ) && ( i->GetY() + multi.offsetY == y ))
				{
					rValue = multi.tileId;
					break;
				}
			}
		}
	}
	
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::DynTile()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns which dynamic tile is present at (x,y) or -1 if no tile exists
//o------------------------------------------------------------------------------------------------o
auto CMulHandler::DynTile( std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::uint16_t instanceId, bool checkOnlyMultis, bool checkOnlyNonMultis ) -> CItem *
{
	auto rValue = static_cast<CItem*>( nullptr );
	for( auto &cellResponse : MapRegion->PopulateList( x, y, worldNumber ))
	{
		if( cellResponse == nullptr )
			continue;

		auto regItems = cellResponse->GetItemList();
		for( const auto &item : regItems->collection() )
		{
			if( !ValidateObject( item ) || ( item->GetInstanceId() != instanceId ))
				continue;

			if( !checkOnlyNonMultis )
			{
				if( item->GetId( 1 ) >= 0x40 && ( item->GetObjType() == OT_MULTI || item->CanBeObjType( OT_MULTI )))
				{
					auto multiTileId = MultiTile( item, x, y, z, false );
					if( multiTileId > 0 )
					{
						rValue = item;
						break;
					}
				}
				else if( item->GetMulti() != INVALIDSERIAL || ValidateObject( CalcMultiFromSer( item->GetOwner() )))
				{
					if(( item->GetX() == x ) && ( item->GetY() == y ))
					{
						rValue = item;
						break;
					}
				}
			}
			else if( !checkOnlyMultis && ( item->GetX() == x ) && ( item->GetY() == y ))
			{
				rValue = item;
				break;
			}
		}

		if( rValue )
		{
			break;
		}
		
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::DoesStaticBlock()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if statics at/above given coordinates blocks movement, etc
//o------------------------------------------------------------------------------------------------o
auto CMulHandler::DoesStaticBlock( std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, bool checkWater ) -> bool
{
	auto &artTiles = Map->ArtAt( x, y, worldNumber );
	auto rValue = false;
	for( auto &tile : artTiles )
	{
		auto elev = static_cast<SI08>( tile.altitude + tile.artInfo->ClimbHeight() );
		if( checkWater )
		{
			if( elev >= z && tile.altitude <= z && ( tile.CheckFlag( TF_BLOCKING ) || tile.CheckFlag( TF_WET )))
			{
				rValue = true;
				break;
			}
		}
		else
		{
			if( elev >= z && tile.altitude <= z && ( tile.CheckFlag( TF_BLOCKING ) && !tile.CheckFlag( TF_WET )))
			{
				rValue = true;
				break;
			}
		}
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::DoesDynamicBlock()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if there are any dynamic tiles at given coordinates that block movement
//o------------------------------------------------------------------------------------------------o
auto CMulHandler::DoesDynamicBlock( std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::uint16_t instanceId, bool checkWater, bool waterWalk, bool checkOnlyMultis, bool checkOnlyNonMultis ) -> bool
{
	auto rValue = false;
	// get the tile id of any dynamic tiles at this spot
	auto dtItem = DynTile( x, y, z, worldNumber, instanceId, checkOnlyMultis, checkOnlyNonMultis );
	if( ValidateObject( dtItem ))
	{
		// Don't allow placing houses on top of immovable, visible dynamic items
		if( dtItem->GetMovable() == 2 && dtItem->GetVisible() == 0 )
			return true;
		
		// Ignore items that are permanently invisible or visible to GMs only
		if( dtItem->GetVisible() == 1 || dtItem->GetVisible() == 3 )
			return false;

		rValue = false;
		const UI16 dt = dtItem->GetId();
		if( IsValidTile( dt ) && dt != 0 )
		{
			auto tile = SeekTile( dt );
			if( waterWalk )
			{
				if( !tile.CheckFlag( TF_WET ))
				{
					rValue = true;
				}
			}
			else
			{
				if( tile.CheckFlag( TF_ROOF ) || tile.CheckFlag( TF_BLOCKING ) || ( checkWater && tile.CheckFlag( TF_WET )) /*|| !tile.CheckFlag( TF_SURFACE ) */ )
				{
					rValue = true;
				}
			}
		}
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::DoesMapBlock()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if the map tile at the given coordinates block movement
//o------------------------------------------------------------------------------------------------o
auto CMulHandler::DoesMapBlock( std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, bool checkWater, bool waterWalk, bool checkMultiPlacement, bool checkForRoad ) -> bool
{
	if( checkWater || waterWalk )
	{
		const auto &map = SeekMap( x, y, worldNumber );
		if(( checkMultiPlacement && map.altitude == z ) || ( !checkMultiPlacement && map.altitude >= z && map.altitude - z < 16 ))
		{
			if( z == ILLEGAL_Z )
				return true;

			if( waterWalk )
			{
				if( !map.CheckFlag( TF_WET ))
				{
					return true;
				}
			}
			else
			{
				if( map.CheckFlag( TF_WET ) || map.CheckFlag( TF_BLOCKING ))
				{
					return true;
				}
			}
			// Check that player is not attempting to place a multi on a road
			if( checkWater && checkMultiPlacement && checkForRoad )
			{
				if( map.terrainInfo->IsRoadId() )
				{
					return true;
				}
			}
		}
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::CheckStaticFlag()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks to see whether any statics at given coordinates has a specific flag
//o------------------------------------------------------------------------------------------------o
auto CMulHandler::CheckStaticFlag( std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, TileFlags toCheck, bool checkSpawnSurface ) -> bool
{
	auto rValue = checkSpawnSurface;
	for( const auto &tile : Map->ArtAt( x, y, worldNumber ))
	{
		const SI08 elev = static_cast<SI08>( tile.altitude );
		const SI08 tileHeight = static_cast<SI08>( tile.artInfo->ClimbHeight() );
		if( checkSpawnSurface )
		{
			// Special case used when checking for spawn surfaces
			if(( z >= elev && z <= ( elev + tileHeight )) && !tile.CheckFlag( toCheck ))
			{
				rValue = false;
				break;
			}
		}
		else
		{
			// Generic check exposed to JS
			if(( z >= elev && z <= ( elev + tileHeight )) && tile.CheckFlag( toCheck ))
			{
				rValue = true; // Found static with specified flag
				break;
			}
		}
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::CheckDynamicFlag()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks to see whether any dynamics at given coordinates has a specific flag
//o------------------------------------------------------------------------------------------------o
auto CMulHandler::CheckDynamicFlag( std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::uint16_t instanceId, TileFlags toCheck ) -> bool
{
	for( auto &cellResponse : MapRegion->PopulateList( x, y, worldNumber ))
	{
		if( cellResponse == nullptr )
			continue;

		auto regItems = cellResponse->GetItemList();
		for( const auto &item : regItems->collection() )
		{
			if( !ValidateObject( item ) || item->GetInstanceId() != instanceId )
				continue;
					
			if(( item->GetId( 1 ) >= 0x40 ) && (( item->GetObjType() == OT_MULTI ) || (item->CanBeObjType( OT_MULTI ))))
			{
				// Found a multi
				// Look for a multi item at specific location
				auto multiId = static_cast<UI16>( item->GetId() - 0x4000 );
				for( auto &multiItem : SeekMulti( multiId ).items )
				{
					if( multiItem.flag > 0 && ( abs( item->GetZ() + multiItem.altitude - z ) <= 1 ))
					{
						if(( item->GetX() + multiItem.offsetX == x ) && ( item->GetY() + multiItem.offsetY == y ))
						{
							if( SeekTile( multiItem.tileId ).CheckFlag( toCheck ))
							{
								return true;
							}
						}
					}
				}
			}
			else
			{
				// item is not a multi
				if(( item->GetX() == x ) && ( item->GetY() == y ) && ( item->GetZ() == z ))
				{
					auto itemZ = item->GetZ();
					const SI08 tileHeight = static_cast<SI08>( TileHeight( item->GetId() ));
					if(( itemZ == z && itemZ + tileHeight > z ) || ( itemZ < z && itemZ + tileHeight >= z ))
					{
						if( SeekTile( item->GetId() ).CheckFlag( toCheck ))
						{
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::CheckTileFlag()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks to see whether given tile ID has a specified flag
//o------------------------------------------------------------------------------------------------o
auto CMulHandler::CheckTileFlag( std::uint16_t itemId, TileFlags flagToCheck ) -> bool
{
	if( !SeekTile( itemId ).CheckFlag( flagToCheck ))
	{
		return false;
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::StaticTop()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Top of statics at/above given coordinates
//o------------------------------------------------------------------------------------------------o
auto CMulHandler::StaticTop( std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::int8_t maxZ ) -> std::int8_t
{
	auto top = ILLEGAL_Z;

	const auto &artTiles = Map->ArtAt( x, y, worldNumber );
	for( const auto &tile : artTiles )
	{
		auto tempTop = static_cast<std::int8_t>( tile.altitude + tile.artInfo->ClimbHeight() );
		if(( tempTop <= z + maxZ ) && ( tempTop > top ))
		{
			top = tempTop;
		}
	}
	return top;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::DynamicElevation()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	This was fixed to actually return the *elevation* of dynamic items at/above given coordinates
//o------------------------------------------------------------------------------------------------o
auto CMulHandler::DynamicElevation( std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::uint16_t instanceId, std::int8_t maxZ ) -> std::int8_t
{
	auto dynZ = ILLEGAL_Z;

	auto MapArea = MapRegion->GetMapRegion( MapRegion->GetGridX( x ), MapRegion->GetGridY( y ), worldNumber );
	if( MapArea )
	{
		auto regItems = MapArea->GetItemList();
		for( const auto tempItem : regItems->collection() )
		{
			if( ValidateObject( tempItem ) || tempItem->GetInstanceId() != instanceId )
			{
				if( tempItem->GetId( 1 ) >= 0x40 && tempItem->CanBeObjType( OT_MULTI ))
				{
					dynZ = MultiHeight( tempItem, x, y, z, maxZ );
				}
				else if( tempItem->GetX() == x && tempItem->GetY() == y )
				{
					SI08 zTemp = static_cast<SI08>( tempItem->GetZ() + TileHeight( tempItem->GetId() ));
					if(( zTemp <= z + maxZ ) && zTemp > dynZ )
					{
						dynZ = zTemp;
					}
				}
			}
		}
	}
	return dynZ;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::MapElevation()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the map elevation at given coordinates, we'll assume since its land
//|					the height is inherently 0
//o------------------------------------------------------------------------------------------------o
auto CMulHandler::MapElevation( std::int16_t x, std::int16_t y, std::uint8_t worldNumber ) -> std::int8_t
{
	const auto &map = SeekMap( x, y, worldNumber );
	// make sure nothing can move into black areas
	if( map.isVoid() )
	{
		return ILLEGAL_Z;
	}
	return map.altitude;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::TileHeight()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Height of a given tile (If climbable it return 1/2 its height).
//o------------------------------------------------------------------------------------------------o
auto CMulHandler::TileHeight( std::uint16_t tilenum ) -> std::int8_t
{
	return SeekTile( tilenum ).ClimbHeight();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::Height()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns new height of player who walked to X/Y but from OLDZ
//o------------------------------------------------------------------------------------------------o
auto CMulHandler::Height( std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber,  std::uint16_t instanceId ) -> std::int8_t
{
	// let's check in this order.. dynamic, static, then the map
	auto dynZ = DynamicElevation( x, y, z, worldNumber, instanceId, MAX_Z_STEP );
	if( ILLEGAL_Z != dynZ )
		return dynZ;

	auto staticZ = StaticTop( x, y, z, worldNumber, MAX_Z_STEP );
	if( ILLEGAL_Z != staticZ )
		return staticZ;

	return MapElevation( x, y, worldNumber );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::InBuilding()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns whether give coordinates are inside a building by checking if there is
//|					a multi or static above them
//o------------------------------------------------------------------------------------------------o
auto CMulHandler::InBuilding( std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::uint16_t instanceId) -> bool
{
	auto dynZ = Map->DynamicElevation( x, y, z, worldNumber, instanceId, static_cast<std::int8_t>( 127 ) );
	if( dynZ > ( z + 10 ))
	{
		return true;
	}
	else
	{
		const SI08 staticZ = Map->StaticTop( x, y, z, worldNumber, static_cast<std::int8_t>( 127 ));
		if( staticZ > ( z + 10 ))
		{
			return true;
		}
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::MultiArea()
//o--------------------------------------------------------------------------
//|	Purpose		-	Finds the corners of a multi object
//o--------------------------------------------------------------------------
auto CMulHandler::MultiArea( CMultiObj *i, std::int16_t &x1, std::int16_t &y1, std::int16_t &x2, std::int16_t &y2 ) -> void
{
	if( ValidateObject( i ))
	{
		const SI16 xAdd = i->GetX();
		const SI16 yAdd = i->GetY();

		const UI16 multiNum = static_cast<UI16>( i->GetId() - 0x4000 );
		if( MultiExists( multiNum ))
		{
			auto structure = SeekMulti( multiNum );
			x1 = static_cast<SI16>( structure.minX + xAdd );
			x2 = static_cast<SI16>( structure.maxX + xAdd );
			y1 = static_cast<SI16>( structure.minY + yAdd );
			y2 = static_cast<SI16>( structure.maxY + yAdd );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::MultiExists()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Verify that specified multi exists
//o------------------------------------------------------------------------------------------------o
auto CMulHandler::MultiExists( std::uint16_t multinum ) const -> bool
{
	return multiData.Exists( multinum );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::SeekMulti()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns data for items contained in given multi
//o------------------------------------------------------------------------------------------------o
auto CMulHandler::SeekMulti( std::uint16_t multinum ) const  -> const CollectionItem_st &
{
	return multiData.Multi( multinum );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::IsValidTile()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if a given tile number falls within the range of tiles loaded in memory
//o------------------------------------------------------------------------------------------------o
auto CMulHandler::IsValidTile( std::uint16_t tileNum ) const -> bool
{
	bool retVal = true;
	if(( tileNum == INVALIDID ) || ( tileNum >= tileInfo.SizeArt() ))
	{
		retVal = false;
	}
	
	return retVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::SeekTile()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Fetches data about a specific tile from memory
//o------------------------------------------------------------------------------------------------o
auto CMulHandler::SeekTile( std::uint16_t tileNum ) -> CTile&
{
	if( !IsValidTile( tileNum ))
	{
		Console.Warning( oldstrutil::format( "Invalid tile access, the offending tile number is %u", tileNum ));
		static CTile emptyTile;
		return emptyTile;
	}
	else
	{
		return tileInfo.ArtInfo( tileNum );
	}
}
//=================================================================================================
auto CMulHandler::SeekTile( std::uint16_t tileNum ) const -> const CTile&
{
	if( !IsValidTile( tileNum ))
	{
		Console.Warning( oldstrutil::format( "Invalid tile access, the offending tile number is %u", tileNum ));
		static CTile emptyTile;
		return emptyTile;
	}
	else
	{
		return tileInfo.ArtInfo( tileNum );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::SeekLand()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Fetches data about a specific land tile from memory.
//o------------------------------------------------------------------------------------------------o
auto CMulHandler::SeekLand( std::uint16_t landNum ) ->CLand&
{
	if( landNum == INVALIDID || landNum >= tileInfo.SizeTerrain() )
	{
		Console.Warning( oldstrutil::format( "Invalid land access, the offending land number is %u", landNum ));
		static CLand emptyTile;
		return emptyTile;
	}
	else
	{
		return tileInfo.TerrainInfo( landNum );
	}
}
//=================================================================================================
auto CMulHandler::SeekLand( std::uint16_t landNum ) const -> const CLand&
{
	if( landNum == INVALIDID || landNum >= tileInfo.SizeTerrain() )
	{
		Console.Warning( oldstrutil::format( "Invalid land access, the offending land number is %u", landNum ));
		static CLand emptyTile;
		return emptyTile;
	}
	else
	{
		return tileInfo.TerrainInfo( landNum );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::SeekMap()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Fetches map data for a specific map/world
//o------------------------------------------------------------------------------------------------o
auto CMulHandler::SeekMap( std::int16_t x, std::int16_t y, std::uint8_t worldNumber ) ->Tile_st&
{
	static Tile_st badTile;
	try
	{
		return uoWorlds.at( worldNumber ).TerrainAt( x, y );
	}
	catch (...)
	{
		std::cerr << "Bad tile access attempted: " << x << "," << y << static_cast<int>( worldNumber ) << std::endl;
		return badTile;
	}
}
//=================================================================================================
auto CMulHandler::SeekMap( std::int16_t x, std::int16_t y, std::uint8_t worldNumber ) const -> const Tile_st&
{
	static Tile_st badTile;
	try
	{
		return uoWorlds.at( worldNumber ).TerrainAt( x, y );
	}
	catch (...)
	{
		std::cerr << "Bad tile access attempted: " << x << "," << y << static_cast<int>( worldNumber ) << std::endl;
		return badTile;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::ValidSpawnLocation()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if location at given coordinates is considered valid for spawning objects
//|					Also used to verify teleport location for NPCs teleporting back to bounding box
//o------------------------------------------------------------------------------------------------o
auto CMulHandler::ValidSpawnLocation( std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::uint16_t instanceId, bool checkWater ) -> bool
{
	if( !InsideValidWorld( x, y, worldNumber ))
		return false;

	// get the tile id of any dynamic tiles at this spot
	if( DoesDynamicBlock( x, y, z, worldNumber, instanceId, checkWater, !checkWater, false, false ))
		return false;

	// if there's a static block here in our way, return false
	if( DoesStaticBlock( x, y, z, worldNumber, checkWater ))
		return false;

	// if the static isn't a surface return false
	if( !CheckStaticFlag( x, y, z, worldNumber, ( checkWater ? TF_SURFACE : TF_WET ), true ))
		return false;

	if( DoesMapBlock( x, y, z, worldNumber, checkWater, !checkWater, false, false ))
		return false;

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::ValidMultiLocation()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks whether given location is valid for house/boat placement
//o------------------------------------------------------------------------------------------------o
auto CMulHandler::ValidMultiLocation( std::int16_t x, std::int16_t y, std::int8_t z, std::uint8_t worldNumber, std::uint16_t instanceId,
									bool checkWater, bool checkOnlyOtherMultis, bool checkOnlyNonMultis, bool checkForRoads ) -> std::uint8_t
{
	if( !InsideValidWorld( x, y, worldNumber ))
		return 0;

	const SI08 elev = Height( x, y, z, worldNumber, instanceId );
	if( ILLEGAL_Z == elev )
		return 0;

	// We don't want the house to be halfway embedded in a hill... or hanging off one for that matter.
	if( z != ILLEGAL_Z )
	{
		if( elev - z > MAX_Z_STEP )
		{
			return 0;
		}
		else if( z - elev > MAX_Z_FALL )
		{
			return 0;
		}
	}

	// get the tile id of any dynamic tiles at this spot
	if( !checkOnlyNonMultis && DoesDynamicBlock( x, y, elev, worldNumber, instanceId, checkWater, false, checkOnlyOtherMultis, checkOnlyNonMultis ))
	{
		return 2;
	}

	// if there's a static block here in our way, return false
	if( !checkOnlyOtherMultis && DoesStaticBlock( x, y, elev, worldNumber, checkWater ))
	{
		return 2;
	}

	if( DoesMapBlock( x, y, elev, worldNumber, checkWater, false, true, checkForRoads ))
	{
		return 0;
	}

	// Check if house placement is allowed in region
	CTownRegion *calcReg = CalcRegionFromXY( x, y, worldNumber, instanceId );
	if(( !calcReg->CanPlaceHouse() && checkWater) || calcReg->IsDungeon() || ( calcReg->IsGuarded() && checkWater ))
		return 3;

	// Else, all good!
	return 1;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::MapExists()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if the server has that map in memory
//o------------------------------------------------------------------------------------------------o
auto CMulHandler::MapExists( std::uint8_t worldNumber ) const -> bool
{
	auto iter = uoWorlds.find( worldNumber );
	return iter != uoWorlds.end();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::InsideValidWorld()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if a given location is within the bounds of the specified world
//o------------------------------------------------------------------------------------------------o
auto CMulHandler::InsideValidWorld( std::int16_t x, std::int16_t y, std::uint8_t worldNumber ) const -> bool
{
	auto rValue = false;
	try 
	{
		auto [width, height] = uoWorlds.at( worldNumber ).Size();
		if(( x >= 0 && x < width ) && ( y >= 0 && y < height ))
		{
			rValue = true;
		}
	}
	catch(...)
	{
		// Do nothing, all rady set to false
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulHandler::MapCount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns count of how many maps have been loaded
//o------------------------------------------------------------------------------------------------o
auto CMulHandler::MapCount() const -> std::uint8_t
{
	// This ASSUMES the map numbers are consequtive.  You dont have say map 0,1,3 <<<< missing 2
	return static_cast<std::uint8_t>( uoWorlds.size() );
}

//o------------------------------------------------------------------------------------------------o
//|	TileInfo
//o------------------------------------------------------------------------------------------------o
//o------------------------------------------------------------------------------------------------o
TileInfo::TileInfo( const std::string &filename ) : isHsFormat( false )
{
	if( !filename.empty() )
	{
		this->LoadTiles( filename );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	TileInfo::LoadTiles()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads tiles from tiledata file into memory
//o------------------------------------------------------------------------------------------------o
auto TileInfo::LoadTiles( const std::string &filename ) -> bool
{
	auto rValue = false;
	artData.clear();
	terrainData.clear();
	isHsFormat = false;
	auto path = std::filesystem::path( filename );
	if( std::filesystem::exists( path ))
	{
		if( std::filesystem::file_size( path ) >= hsSize )
		{
			isHsFormat = true;
		}
		auto input = std::ifstream( filename, std::ios::binary );
		if( input.is_open() )
		{
			rValue = true;
			ProcessTerrain( input );
			ProcessArt( input );
		}
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	TileInfo::TerrainInfo()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns terrain data for given tileId
//o------------------------------------------------------------------------------------------------o
auto TileInfo::TerrainInfo( std::uint16_t tileId ) const -> const CLand&
{
	return terrainData[tileId];
}
auto TileInfo::TerrainInfo( std::uint16_t tileId ) -> CLand&
{
	return terrainData[tileId];
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	TileInfo::ArtInfo()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns art/statics data for given tileId
//o------------------------------------------------------------------------------------------------o
auto TileInfo::ArtInfo( std::uint16_t tileId ) const -> const CTile&
{
	return artData[tileId];
}
auto TileInfo::ArtInfo( std::uint16_t tileId) -> CTile&
{
	return artData[tileId];
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	TileInfo::SizeTerrain()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns size of terrain data loaded by UOX3
//o------------------------------------------------------------------------------------------------o
auto TileInfo::SizeTerrain() const -> size_t
{
	return terrainData.size();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	TileInfo::SizeArt()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns size of art/statics data loaded by UOX3
//o------------------------------------------------------------------------------------------------o
auto TileInfo::SizeArt() const -> size_t
{
	return artData.size();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	TileInfo::CollectionTerrain()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns collection of terrain tiles loaded by UOX3
//o------------------------------------------------------------------------------------------------o
auto TileInfo::CollectionTerrain() const -> const std::vector<CLand>&
{
	return terrainData;
}
auto TileInfo::CollectionTerrain() -> std::vector<CLand> &
{
	return terrainData;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	TileInfo::CollectionArt()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns collection of art/statics tiles loaded by UOX3
//o------------------------------------------------------------------------------------------------o
auto TileInfo::CollectionArt() const -> const std::vector<CTile>&
{
	return artData;
}
auto TileInfo::CollectionArt() -> std::vector<CTile>&
{
	return artData;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	TileInfo::ProcessTerrain()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Process terrain data read from tiledata file
//o------------------------------------------------------------------------------------------------o
auto TileInfo::ProcessTerrain( std::ifstream &input ) -> void
{
	terrainData.reserve( 0x4000 );
	std::uint32_t value32 = 0;
	std::uint64_t value64 = 0;
	std::array<char, 20> string_buffer;
	string_buffer.fill( 0 );
	for( auto i = 0; i < 0x4000; i++ )
	{
		// We have to get rid of the header on blocks of information
		// HS has the first entry messed up
		if( isHsFormat )
		{
			if(((( i & 0x1F ) == 0 ) && ( i > 0 )) || ( i == 1 ))
			{
				input.read( reinterpret_cast<char*>( &value32 ), 4 ); // read off the group header
			}
		}
		else
		{
			if(( i & 0x1f ) == 0 )
			{
				input.read( reinterpret_cast<char*>( &value32 ), 4 ); // read off the group header
			}
		}
		// now create the info_t we will use
		auto info = CLand();
		// read off the flags, 32 bit on pre-HS, 64 bit after
		if( isHsFormat )
		{
			input.read( reinterpret_cast<char*>( &value64), 8 );
			info.flags = std::bitset<64>( value64 );
		}
		else
		{
			input.read( reinterpret_cast<char*>( &value32 ), 4 );
			info.flags = std::bitset<64>( value32 );
		}
		// only other thing for terrain is the texture
		// and name
		input.read( reinterpret_cast<char*>( &info.textureId ), 2 );
		input.read( string_buffer.data(), 20 );
		if( input.gcount() == 20 )
		{
			// We need to find the "0", if any
			auto iter = std::find( string_buffer.begin(), string_buffer.end(), 0 );
			info.name = std::string( string_buffer.begin(), iter );
			terrainData.push_back( std::move( info ));
		}
		else
		{
			break;
		}
	}

}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	TileInfo::ProcessArt()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Process art/statics data read from tiledata file
//o------------------------------------------------------------------------------------------------o
auto TileInfo::ProcessArt( std::ifstream &input ) -> void
{
	artData.reserve( 0xFFFF );
	std::uint32_t value32 = 0;
	std::uint64_t value64 = 0;
	std::array<char, 20> string_buffer;
	string_buffer.fill( 0 );
	if( !( input.eof() || input.bad() ))
	{
		auto loopCount = 0;
		do
		{
			if(( loopCount & 0x1f ) == 0 )
			{
				input.read( reinterpret_cast<char*>( &value32 ), 4 ); // read off the group header
			}
			auto info = CTile();
			if( isHsFormat )
			{
				input.read( reinterpret_cast<char*>( &value64 ), 8 );
				info.flags = std::bitset<64>( value64 );

				
			}
			else
			{
				input.read( reinterpret_cast<char*>( &value32 ), 4 );
				info.flags = std::bitset<64>( value32 );

			}
			input.read( reinterpret_cast<char*>( &info.weight ), 1 );
			input.read( reinterpret_cast<char*>( &info.layer ), 1 );
			//misc data
			input.read( reinterpret_cast<char*>( &info.unknown1 ), 2 );
			//Second unkown
			input.read( reinterpret_cast<char*>( &info.unknown2), 1 );
			// Quantity
			input.read( reinterpret_cast<char*>( &info.quantity), 1 );
			// Animation
			input.read( reinterpret_cast<char*>( &info.animation), 2 );
			
			//Third a byte
			input.read( reinterpret_cast<char*>( &info.unknown3), 1 );
			
			// Hue/Value
			input.read( reinterpret_cast<char*>( &info.hue), 1 );
			//stacking offset/value  = 16 bits, but UOX3 doesn't know that, so two eight bit unkowns
			input.read( reinterpret_cast<char*>( &info.unknown4 ), 1 );
			input.read( reinterpret_cast<char*>( &info.unknown5 ), 1 );
			// Height
			input.read( reinterpret_cast<char*>( &info.height ), 1 );
			// and name
			input.read( string_buffer.data(), 20 );
			if(( input.gcount() == 20 ) && input.good() )
			{
				// We need to find the "0", if any
				auto iter = std::find( string_buffer.begin(), string_buffer.end(), 0 );
				info.name = std::string( string_buffer.begin(), iter );
				artData.push_back( std::move( info ));
			}
			loopCount++;
			
		} while(( !input.eof() ) && input.good() );
	}

}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	TileInfo::TotalMemory()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate total memory used for loading art/terrain tiledata
//o------------------------------------------------------------------------------------------------o
auto TileInfo::TotalMemory() const -> size_t
{
	auto flag = isHsFormat ? 8 : 4;
	auto terrain = flag + 20 + 2;
	auto art = flag + 20 + 23;
	return static_cast<size_t>(( terrain * terrainData.size() ) + ( art * artData.size() ));
}

//o------------------------------------------------------------------------------------------------o
//|	TerrainBlock::TerrainBlock()
//o------------------------------------------------------------------------------------------------o
//o------------------------------------------------------------------------------------------------o
TerrainBlock::TerrainBlock( std::uint8_t *data, const TileInfo *info )
{
	if( data != nullptr )
	{
		LoadBlock( data, info );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	TerrainBlock::LoadBlock()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Load details for terrain tiles in a 8x8 block of the map
//o------------------------------------------------------------------------------------------------o
auto TerrainBlock::LoadBlock( std::uint8_t *data, const TileInfo *info ) -> void
{
	if( data != nullptr )
	{
		data += 4;
		for( auto y = 0; y < 8; ++y )
		{
			for( auto x = 0; x < 8; ++x )
			{
				auto tileId = std::uint16_t( 0 );
				// we copy here, because of potential alignment issues
				std::copy( data, data + 2, reinterpret_cast<std::uint8_t*>( &( tileId )));
				_tiles[x][y].tileId = tileId;
				if( info )
				{
					_tiles[x][y].terrainInfo = &info->TerrainInfo( tileId );
				}
				_tiles[x][y].altitude = *( reinterpret_cast<std::int8_t*>( data + 2 ));
				data += 3; // advance data three bytes;
			}
		}
	}
}

//=========================================================
auto TerrainBlock::TerrainTileAt( int x, int y ) -> Tile_st&
{
	return _tiles.at( x ).at( y );
}
//=========================================================
auto TerrainBlock::TerrainTileAt( int x, int y ) const -> const Tile_st&
{
	return _tiles.at( x ).at( y );
}

//================================================================================
// ArtBlock
//================================================================================

//=========================================================
ArtBlock::ArtBlock( int length, std::uint8_t *data, const TileInfo *info )
{
	if( data != nullptr )
	{
		LoadArtBlock( length, data, info );
	}
}

//=========================================================
auto ArtBlock::ArtTileAt( int x, int y ) -> std::vector<Tile_st>&
{
	return _tiles.at( x ).at( y );
}
//=========================================================
auto ArtBlock::ArtTileAt( int x, int y ) const -> const std::vector<Tile_st>&
{
	return _tiles.at( x ).at( y );
}

//=========================================================
auto ArtBlock::Clear() -> void
{
	for( auto &row : _tiles )
	{
		for( auto &tile: row )
		{
			tile.clear();
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ArtBlock::LoadArtBlock()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Load details for all art/statics tiles in a given location from statics files
//o------------------------------------------------------------------------------------------------o
auto ArtBlock::LoadArtBlock( int length, std::istream &input, const TileInfo *info ) -> void
{
	this->Clear();
	auto count = length / 7;
	auto x = std::uint8_t( 0 );
	auto y = std::uint8_t( 0 );
	auto alt = std::int8_t( 0 );
	for( auto i = 0; i < count; ++i )
	{
		auto artTile = Tile_st();
		artTile.type = TileType_t::art;
		input.read( reinterpret_cast<char*>( &artTile.tileId ), 2 );
		input.read( reinterpret_cast<char*>( &x ), 1 );
		input.read( reinterpret_cast<char*>( &y ), 1 );
		input.read( reinterpret_cast<char*>( &alt ), 1 );
		artTile.altitude = alt;
		input.read( reinterpret_cast<char*>( &artTile.staticHue ), 2 );
		if( info )
		{
			artTile.artInfo = &info->ArtInfo( artTile.tileId );
		}
		ArtTileAt( x, y ).push_back( std::move( artTile ));
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ArtBlock::LoadArtBlock()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Retrieve details for all art/statics tiles in a given location from memory
//o------------------------------------------------------------------------------------------------o
auto ArtBlock::LoadArtBlock( int length, std::uint8_t *data, const TileInfo *info ) -> void
{
	this->Clear();
	
	auto count = length / 7;
	auto x = std::uint8_t( 0 );
	auto y = std::uint8_t( 0 );
	for( auto i = 0; i < count; ++i )
	{
		auto artTile = Tile_st();
		artTile.type = TileType_t::art;
		std::copy( data, data + 2, reinterpret_cast<char*>( &artTile.tileId ));
		x = * ( data + 2 );
		y = * ( data + 3 );
		artTile.altitude = static_cast<int>( *( reinterpret_cast<std::int8_t*>( data + 4 )));
		std::copy( data + 5, data + 7, reinterpret_cast<char*>( &artTile.staticHue ));
		if( info )
		{
			artTile.artInfo = &info->ArtInfo( artTile.tileId );
		}
		
		ArtTileAt( x, y ).push_back( std::move( artTile ));
		data += 7;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	UltimaMap
//o------------------------------------------------------------------------------------------------o
//o------------------------------------------------------------------------------------------------o
UltimaMap::UltimaMap() : tileInfo( nullptr ), _width( 0 ), _height( 0 ), _diffCount( 0 ), _diffTerrain( 0 )
{
}
//=========================================================
UltimaMap::UltimaMap( int mapNum, int width, int height, const TileInfo *info ) : UltimaMap()
{
	_width = width;
	_height= height;
	tileInfo = info;
	_mapNum = mapNum;
	auto size = _mapSizes.at( mapNum );
	if( _width == 0 )
	{
		_width = size.first;
	}
	if( _height == 0 )
	{
		_height = size.second;
	}
	_terrain.resize(( _width / 8 ) * ( _height / 8 ));
	_art.resize( _terrain.size() );
	isUop = true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	UltimaMap::Width()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get width of map
//o------------------------------------------------------------------------------------------------o
auto UltimaMap::Width() const -> int
{
	return _width;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	UltimaMap::Height()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get height of map
//o------------------------------------------------------------------------------------------------o
auto UltimaMap::Height() const -> int
{
	return _height;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	UltimaMap::SetSize()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Set width and height of map
//o------------------------------------------------------------------------------------------------o
auto UltimaMap::SetSize( int width, int height ) -> void
{
	_width = width;
	_height = height;
	_terrain.resize(( _width / 8 ) * ( _height / 8 ));
	_art.resize( _terrain.size() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	UltimaMap::Size()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get size of map as width/height pair 
//o------------------------------------------------------------------------------------------------o
auto UltimaMap::Size() const -> std::pair<int, int>
{
	return std::make_pair( static_cast<int>( _width ), static_cast<int>( _height ));
}

//=========================================================
auto UltimaMap::DiffArt() const -> int
{
	return _diffCount;
}
//=========================================================
auto UltimaMap::DiffTerrain() const -> int
{
	return _diffTerrain;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	UltimaMap::LoadUOPTerrainFile()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Load map data from uop-version of map files
//o------------------------------------------------------------------------------------------------o
auto UltimaMap::LoadUOPTerrainFile( const std::string &fileName ) -> bool
{
	auto hash = this->Format( "build/map%ilegacymul/%s", _mapNum, "%.8u.dat" );
	//auto fileName = this->format( "map%iLegacyMUL.uop", _mapNum );
	isUop = LoadUop( fileName, 0x300, hash );
	return isUop;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	UltimaMap::LoadMulTerrainFile()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Load map data from mul-version of map files
//o------------------------------------------------------------------------------------------------o
auto UltimaMap::LoadMulTerrainFile(const std::string &fileName) -> bool {
	auto rValue = false;
	auto mul = std::ifstream( fileName, std::ios::binary );
	if( mul.is_open() )
	{
		rValue = true;
		isUop = false;
		auto data = std::vector<std::uint8_t>( 196, 0 );
		auto blockNum = 0;
		while( mul.good() && !mul.eof() )
		{
			mul.read( reinterpret_cast<char*>( data.data() ), 196 );
			if( mul.gcount() == 196 )
			{
				LoadTerrainBlock( blockNum, data.data() );
			}
			blockNum++;
		}
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	UltimaMap::LoadArt()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Load data from statics/staidx files
//o------------------------------------------------------------------------------------------------o
auto UltimaMap::LoadArt( const std::string &mulFile, const std::string &idxFile ) -> bool
{
	auto idx = std::ifstream( idxFile, std::ios::binary );
	auto rValue = idx.is_open();
	if( rValue )
	{
		auto mul = std::ifstream( mulFile, std::ios::binary );
		rValue = mul.is_open();
		if( rValue )
		{
			auto offset = std::uint32_t( 0 );
			auto length = std::int32_t( 0 );
			auto extra = std::uint32_t( 0 );
			size_t block = 0;
			while( idx.good() && !idx.eof() )
			{
				idx.read( reinterpret_cast<char*>( &offset ), 4 );
				idx.read( reinterpret_cast<char*>( &length ), 4 );
				idx.read( reinterpret_cast<char*>( &extra ), 4 );
				if(( length > 0 ) && ( idx.gcount() == 4 ))
				{
					mul.seekg( offset, std::ios::beg );
					if( block < _art.size() )
					{
						_art[block].LoadArtBlock( length, mul, tileInfo );
					}
				}
				++block;
			}
		}
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	UltimaMap::ApplyDiff()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Apply patches from diff files to statics for map loaded by UOX3
//o------------------------------------------------------------------------------------------------o
auto UltimaMap::ApplyDiff( const std::string &difflPath, const std::string &diffiPath, const std::string &diffPath ) -> int
{
	_diffCount = 0;
	auto diffBlock = std::ifstream( difflPath, std::ios::binary );
	if( diffBlock.is_open() )
	{
		auto diffIdx = std::ifstream( diffiPath, std::ios::binary );
		if( diffIdx.is_open() )
		{
			auto diff = std::ifstream( diffPath, std::ios::binary );
			if( diff.is_open() )
			{
				auto blockNum = std::uint32_t( 0 );
				auto offset = std::uint32_t( 0 );
				auto length = std::int32_t( 0 );
				auto extra = std::uint32_t( 0 );
				while( diffBlock.good() && !diffBlock.eof() )
				{
					diffBlock.read( reinterpret_cast<char*>( &blockNum ), 4 );
					diffIdx.read( reinterpret_cast<char*>( &offset ), 4 );
					diffIdx.read( reinterpret_cast<char*>( &length ), 4 );
					diffIdx.read( reinterpret_cast<char*>( &extra ), 4 );
					if(( diffBlock.gcount() == 4 ) && ( diffIdx.gcount() == 4 ))
					{
						if( blockNum < _art.size() )
						{
							if( length > 0 )
							{
								diff.seekg( offset, std::ios::beg );
								_art[blockNum].LoadArtBlock( length, diff, tileInfo );
							}
							else
							{
								_art[blockNum].Clear();
							}
						}
						++_diffCount;
					}
				}
			}
		}
	}
	return _diffCount;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	UltimaMap::ApplyTerrainDiff()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Apply patches from diff files to map loaded by UOX3
//o------------------------------------------------------------------------------------------------o
auto UltimaMap::ApplyTerrainDiff( const std::string &difflPath, const std::string &diffPath ) -> int
{
	auto diffblock = std::ifstream( difflPath, std::ios::binary );
	_diffTerrain = 0;
	if( diffblock.is_open() )
	{
		auto diff = std::ifstream( diffPath, std::ios::binary );
		if( diff.is_open() )
		{
			auto blocknum = std::uint32_t( 0 );
			auto data = std::vector<std::uint8_t>( 196, 0 );
			while( diffblock.good() && !diffblock.eof() )
			{
				diffblock.read( reinterpret_cast<char*>( &blocknum ), 4 );
				if( diffblock.gcount() == 4 )
				{
					diff.read( reinterpret_cast<char*>( data.data() ), 196 );
					if( diff.gcount() == 196 )
					{
						_terrain[blocknum].LoadBlock( data.data(), tileInfo );
						_diffTerrain++;
					}
				}
			}
		}
	}
	return _diffTerrain;
}
//=========================================================
auto UltimaMap::CalcBlock( int x, int y) const -> int
{
	// How many blocks is x across?
	return (( x / 8 ) * ( _height / 8 )) + ( y / 8 );
}
//=========================================================
auto UltimaMap::CalcXYOffset( int block ) const -> std::pair<int, int>
{
	auto x = ( block / ( _height / 8 )) * 8;
	auto y = ( block % ( _height / 8 )) * 8;
	return std::make_pair( x, y );
}
//=========================================================
auto UltimaMap::BlockAndIndexFor( int x, int y ) const -> std::tuple<int, int, int>
{
	auto block = CalcBlock( x, y );
	auto offset = CalcXYOffset( block );
	return std::make_tuple( block, x - offset.first, y - offset.second );
}
//=========================================================
auto UltimaMap::ProcessEntry( [[maybe_unused]] std::size_t entry, std::size_t index, std::vector<std::uint8_t> &data ) -> bool
{
	auto count = data.size() / 196;
	size_t block = ( static_cast<int>( index ) * 0xC4000 ) / 196;
	for( size_t i = 0; i < count; ++i )
	{
		auto ptr = data.data() + ( i * 196 );
		if( block < _terrain.size() )
		{
			LoadTerrainBlock( block, ptr );
		}
		++block;
	}
	return true;
}
//=========================================================
auto UltimaMap::LoadTerrainBlock( int blocknum, std::uint8_t *data ) -> void
{
	_terrain[blocknum].LoadBlock( data, tileInfo );
}

//=========================================================
auto UltimaMap::TerrainAt( int x, int y ) const -> const Tile_st&
{
	auto [blocknum, xoffset, yoffset] = BlockAndIndexFor( x, y );
	return _terrain[blocknum].TerrainTileAt( xoffset, yoffset );
}
//=========================================================
auto UltimaMap::TerrainAt( int x, int y ) -> Tile_st&
{
	auto [blocknum, xoffset, yoffset] = BlockAndIndexFor( x, y );
	return _terrain[blocknum].TerrainTileAt( xoffset, yoffset );
}
//=========================================================
auto UltimaMap::ArtAt( int x, int y ) const -> const std::vector<Tile_st>&
{
	auto [blocknum, xoffset, yoffset] = BlockAndIndexFor( x, y );
	return _art[blocknum].ArtTileAt( xoffset, yoffset );
}
//=========================================================
auto UltimaMap::ArtAt( int x, int y ) -> std::vector<Tile_st>&
{
	auto [blocknum, xoffset,yoffset] = BlockAndIndexFor( x, y );
	return _art[blocknum].ArtTileAt( xoffset, yoffset );
}
