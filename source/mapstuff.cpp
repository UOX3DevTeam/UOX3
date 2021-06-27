#include "uox3.h"
#include "regions.h"
#include "townregion.h"
#include "fileio.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "scriptc.h"
#include <filesystem>

CMulHandler *Map				= nullptr;

const UI16 LANDDATA_SIZE		= 0x4000; //(512 * 32)

//! these are the fixed record lengths as determined by the .mul files from OSI
//! i made them longs because they are used to calculate offsets into the files
const UI32 TileRecordSize			= 26L;
const UI32 TileRecordSizeHS			= 41L;
const UI32 MapRecordSize			= 3L;
const UI32 StaticRecordSize			= 7L;
const UI32 StaticIndexRecordSize	= 12L;
const UI32 MapBlockSize				= 196L;

/*! New Notes: 11/3/1999
 ** I changed all of the places where z was handled as a signed char and
 ** upped them to an int. While the basic terrain never goes above 127,
 ** calculations sure can. For example, if you are at a piece of land at 100
 ** and put a 50 high static object, its going to roll over.
 */

/*!
 ** New rewrite plans, its way confusing having every function in here using 'Height' to mean
 ** elevation or actual height of a tile.  Here's the new renaming plan:
 ** Elevation - the z value of the bottom edge of an item
 ** Height - the height of the item, it is independant of the elevation, like a person is 6ft tall
 ** Top - the z value of the top edge of an item, is always Elevation + Height of item
 */

/*!
 ** I thought it might be helpful to others to include a quick guide to what things
 ** meant in this code.
 ** Tile - a single item in the game, maybe static or dynamic
 ** Land - a single square from the map, the ground/water
 ** Static - a single item that comes as part of the basic map. NOTE: Land and Statics are both
 **			saved in the same .mul file!  A lot of statics look like land as well.
 ** Dynamic - any item added that doesn't come as part of the basic map
 ** Single - an item in the game that will fit in a single cell
 ** Multi - any item in the game that is composed of multiple single items across multiple cells
 **			maybe static or dynamic, may have multiple singles within same cell
 **
 */

MapData_st::~MapData_st()
{
	if( mapObj != nullptr )
	{
		delete mapObj;
		mapObj = nullptr;
	}
	if( staticsObj != nullptr )
	{
		delete staticsObj;
		staticsObj = nullptr;
	}
	if( staidxObj != nullptr )
	{
		delete staidxObj;
		staidxObj = nullptr;
	}
	if( mapDiffObj != nullptr )
	{
		delete mapDiffObj;
		mapDiffObj = nullptr;
	}
	if( staticsDiffObj != nullptr )
	{
		delete staticsDiffObj;
		staticsDiffObj = nullptr;
	}
}

/*! Use Memory-Mapped file to do caching instead
 ** 'verdata.mul', 'tiledata.mul', 'multis.mul' and 'multi.idx' is changed so
 **  it will reads into main memory and access directly.  'map*.mul', 'statics*.mul' and
 **  'staidx.mul' is being managed by memory-mapped files, it faster than manual caching
 **  and less susceptible to bugs.
 */
CMulHandler::CMulHandler() : landTile( 0 ),  staticTile( 0 ),  tileDataSize( 0 )
{
	LoadMapsDFN();
}

CMulHandler::~CMulHandler()
{
	if ( landTile )
		delete[] landTile;
	if ( staticTile )
		delete[] staticTile;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void LoadMapsDFN( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads maps.dfn to see which map files to actually load into memory later
//o-----------------------------------------------------------------------------------------------o
void CMulHandler::LoadMapsDFN( void )
{
	std::string tag, data, UTag;
	const UI08 NumberOfWorlds = static_cast<UI08>(FileLookup->CountOfEntries( maps_def ));
	MapList.reserve( NumberOfWorlds );
	for( UI08 i = 0; i < NumberOfWorlds; ++i )
	{
		ScriptSection *toFind = FileLookup->FindEntry( "MAP " + strutil::number( i ), maps_def );
		if( toFind == nullptr )
			break;

		MapData_st toAdd;
		for( tag = toFind->First(); !toFind->AtEnd(); tag = toFind->Next() )
		{
			UTag = strutil::toupper( tag );
			data = toFind->GrabData();
			data = strutil::stripTrim( data );
			switch( (UTag.data()[0]) )
			{
				case 'M':
					if( UTag == "MAP" )
					{
						toAdd.mapFile = data;
					}
					else if( UTag == "MAPUOPWRAP" )
					{
						toAdd.mapFileUOPWrap = data;
					}
					else if( cwmWorldState->ServerData()->MapDiffsEnabled() )
					{
						if( UTag == "MAPDIFF" )
						{
							toAdd.mapDiffFile = data;
						}
						else if( UTag == "MAPDIFFLIST" )
						{
							toAdd.mapDiffListFile = data;
						}
					}
					break;
				case 'S':
					if( UTag == "STATICS" )
					{
						toAdd.staticsFile = data;
					}
					else if( UTag == "STAIDX" )
					{
						toAdd.staidxFile = data;
					}
					else if( cwmWorldState->ServerData()->MapDiffsEnabled() )
					{
						if( UTag == "STATICSDIFF" )
						{
							toAdd.staticsDiffFile = data;
						}
						else if( UTag == "STATICSDIFFLIST" )
						{
							toAdd.staticsDiffListFile = data;
						}
						else if( UTag == "STATICSDIFFINDEX" )
						{
							toAdd.staticsDiffIndexFile = data;
						}
					}
					break;
				case 'X':
					if( UTag == "X" )
					{
						toAdd.xBlock = static_cast<UI16>(std::stoul(data, nullptr, 0));
					}
					break;
				case 'Y':
					if( UTag == "Y" )
					{
						toAdd.yBlock = static_cast<UI16>(std::stoul(data, nullptr, 0));
					}
					break;
			}
		}
		MapList.push_back( toAdd );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UOXFile * loadFile( const std::string& fullName )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Prints status of loading a file in UOX3 console
//o-----------------------------------------------------------------------------------------------o
UOXFile * loadFile( const std::string& fullName )
{
	UOXFile *toLoad = new UOXFile( fullName.c_str(), "rb" );
	Console << "\t" << fullName << "\t\t";

	if( toLoad != nullptr && toLoad->ready() )
		Console.PrintDone();
	else
		Console.PrintSpecial( CBLUE, "not found" );

	return toLoad;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void LoadMapAndStatics( MapData_st& mMap, const std::string& basePath, UI08 &totalMaps )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads map and statics files into memory
//o-----------------------------------------------------------------------------------------------o
void CMulHandler::LoadMapAndStatics( MapData_st& mMap, const std::string& basePath, UI08 &totalMaps )
{
	std::string mapMUL		= mMap.mapFile;
	std::string mapUOPWrap	= mMap.mapFileUOPWrap;
	std::string lName		= basePath + mapMUL;
	mMap.mapObj			= new UOXFile( lName.c_str(), "rb" );
	Console << "\t" << lName << "(/" << mapUOPWrap << ")\t\t";

	//if no map0.mul was found, check if there's a map#LegacyMul.uop
	if(( mMap.mapObj == nullptr || !mMap.mapObj->ready() ) && !mapUOPWrap.empty() )
	{
		std::string lName	= basePath + mapUOPWrap;
		mMap.mapObj		= new UOXFile( lName.c_str(), "rb" );
	}
	if( mMap.mapObj != nullptr && mMap.mapObj->ready() )
	{
		SI32 checkSize = static_cast<SI32>(mMap.mapObj->getLength() / MapBlockSize);
		if( checkSize / (mMap.xBlock/8) == (mMap.yBlock/8) )
		{
			++totalMaps;
			Console.PrintDone();
		}
		else
		{
			Console.PrintFailed();
		}
	}
	else
	{
		Console.PrintSpecial( CRED, "not found" );
	}

	mMap.staticsObj				= loadFile( basePath + mMap.staticsFile );
	mMap.staidxObj				= loadFile( basePath + mMap.staidxFile );

	if( !mMap.mapDiffFile.empty() )
		mMap.mapDiffObj			= loadFile( basePath + mMap.mapDiffFile );

	if( !mMap.staticsDiffFile.empty() )
		mMap.staticsDiffObj		= loadFile( basePath + mMap.staticsDiffFile );

	if( !mMap.mapDiffListFile.empty() )
	{
		lName = basePath + mMap.mapDiffListFile;
		Console << "\t" << lName << "\t\t";

		std::ifstream mdlFile ( lName.c_str(), std::ios::in | std::ios::binary );
		if( mdlFile.is_open() )
		{
			size_t offset = 0;
			size_t blockID = 0;
			mdlFile.seekg( 0, std::ios::beg );
			while( !mdlFile.fail() )
			{
				mdlFile.read( (char *)&blockID, 4 );
				if( !mdlFile.fail() )
				{
					mMap.mapDiffList[static_cast<UI32>(blockID)] = static_cast<UI32>(offset);
				}
				offset += MapBlockSize;
			}
			mdlFile.close();
			Console.PrintDone();
		}
		else
			Console.PrintSpecial( CBLUE, "not found" );
	}

	if( !mMap.staticsDiffIndexFile.empty() && !mMap.staticsDiffListFile.empty() )
	{
		lName = basePath + mMap.staticsDiffIndexFile;
		Console << "\t" << lName << "\t\t";

		std::ifstream sdiFile ( lName.c_str(), std::ios::in | std::ios::binary );
		if( sdiFile.is_open() )
		{
			Console.PrintDone();
			lName = basePath + mMap.staticsDiffListFile;
			Console << "\t" << lName << "\t\t";

			std::ifstream sdlFile ( lName.c_str(), std::ios::in | std::ios::binary );
			if( sdlFile.is_open() )
			{
				size_t blockID = 0;
				sdiFile.seekg( 0, std::ios::beg );
				sdlFile.seekg( 0, std::ios::beg );
				while( !sdiFile.fail() && !sdlFile.fail() )
				{
					sdlFile.read( (char *)&blockID, 4 );
					sdiFile.read( (char *)&mMap.staticsDiffIndex[static_cast<UI32>(blockID)].offset, 4 );
					sdiFile.read( (char *)&mMap.staticsDiffIndex[static_cast<UI32>(blockID)].length, 4 );
					sdiFile.read( (char *)&mMap.staticsDiffIndex[static_cast<UI32>(blockID)].unknown, 4 );
				}
				sdlFile.close();
				Console.PrintDone();
			}
			else
				Console.PrintSpecial( CBLUE, "not found" );
			sdiFile.close();
		}
		else
			Console.PrintSpecial( CBLUE, "not found" );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void LoadTileData( const std::string& basePath )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads tiledata file into memory
//o-----------------------------------------------------------------------------------------------o
void CMulHandler::LoadTileData( const std::string& basePath )
{
	UI08 j = 0;

	// tiledata.mul is to be cached.
	std::string lName = basePath + "tiledata.mul";
	Console << "\t" << lName << "\t";

	UOXFile tileFile( lName.c_str(), "rb" );
	if( !tileFile.ready() )
	{
		Console.PrintFailed();
		Shutdown( FATAL_UOX3_TILEDATA_NOT_FOUND );
	}

	//Grab length of tileFile to determine version of datafiles used
	size_t tileFileLength = tileFile.getLength();
	bool useHS = false ;
	auto recordSize = TileRecordSize ;
	if( tileFileLength >= 3188736 )
	{
		useHS = true ;
		recordSize = TileRecordSizeHS ;
	}
	
	landTile		= new CLand[LANDDATA_SIZE];
	CLand *landPtr	= landTile;
	for( UI16 i = 0; i < 512; ++i )
	{
		tileFile.seek(4, SEEK_CUR);			// skip the dummy header
		for( j = 0; j < 32; ++j )
			landPtr[j].Read( &tileFile,useHS );
		landPtr += 32;
	}
	
	tileDataSize		= (((tileFile.getLength() - (((32 * recordSize) + 4) * 512)) / 1188) * 32);
	staticTile		= new CTile[tileDataSize];
	CTile *tilePtr	= staticTile;
	
	while( !tileFile.eof() )
	{
		tileFile.seek(4, SEEK_CUR);			// skip the dummy header
		for( j = 0; j < 32; ++j )
			tilePtr[j].Read( &tileFile, useHS );
		tilePtr += 32;
	}	
	
	Console.PrintDone();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void Load( void )
//|	Date		-	03/12/2002
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Prepare access streams to the server required mul files. This function basicaly
//|					just opens the streams and validates that the file is open and available
//o-----------------------------------------------------------------------------------------------o
void CMulHandler::Load( void )
{
	Console.PrintSectionBegin();
	Console << "Preparing to open *.mul files..." << myendl << "(If they don't open, fix your paths in uox.ini or filenames in maps.dfn)" << myendl;

	const std::string basePath = cwmWorldState->ServerData()->Directory( CSDDP_DATA );

	UI08 totalMaps = 0;
	for( MAPLIST_ITERATOR mlIter = MapList.begin(); mlIter != MapList.end(); ++mlIter )
	{
		LoadMapAndStatics( (*mlIter), basePath, totalMaps );
	}
	if( totalMaps == 0 )
	{
		// Hmm, no maps were valid, so not much sense in continuing
		Console.error( " Fatal Error: No maps found" );
		Console.error( " Check the settings for DATADIRECTORY in uox.ini" );
		Shutdown( FATAL_UOX3_MAP_NOT_FOUND );
	}

	LoadTileData( basePath );
	LoadMultis( basePath );
	LoadDFNOverrides();

	FileLookup->Dispose( maps_def );
	Console.PrintSectionBegin();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void LoadMultis( const std::string& basePath )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads multi files into memory
//o-----------------------------------------------------------------------------------------------o
void CMulHandler::LoadMultis( const std::string& basePath )
{
	// now main memory multiItems
	Console << "Caching Multis....  ";
	std::string uopname = basePath + "MultiCollection.uop" ;
	if (std::filesystem::exists(std::filesystem::path(uopname))) {
		_multidata.load(uopname);
	}
	else {
	// only turn it off for now because we are trying to fill the cache.
		std::string idxname	= basePath + "multi.idx";

		std::string mulname	= basePath + "multi.mul";
		_multidata.load(mulname, idxname) ;
	}
	Console.PrintDone();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	size_t GetTileMem( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns amount of memory used to load tiledata
//o-----------------------------------------------------------------------------------------------o
size_t CMulHandler::GetTileMem( void ) const
{
	return (LANDDATA_SIZE * sizeof( CLand ) + tileDataSize * sizeof( CTile ));
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	size_t GetMultisMem( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns amount of memory used to load multis
//o-----------------------------------------------------------------------------------------------o
size_t CMulHandler::GetMultisMem( void ) const
{
	return _multidata.memoryMulti();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 TileHeight( UI16 tilenum )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Height of a given tile (If climbable we return 1/2 its height).
//o-----------------------------------------------------------------------------------------------o
SI08 CMulHandler::TileHeight( UI16 tilenum )
{
	CTile& tile = SeekTile( tilenum );
	// For Stairs+Ladders
	if( tile.CheckFlag( TF_CLIMBABLE ) )
		return static_cast<SI08>(tile.Height()/2);
	return tile.Height();
}


//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 StaticTop( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber, SI08 maxZ )
//|	Date		-	Unknown     Touched: Dec 21, 1998
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Top of statics at/above given coordinates
//o-----------------------------------------------------------------------------------------------o
SI08 CMulHandler::StaticTop( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber, SI08 maxZ )
{
	SI08 top = ILLEGAL_Z;

	CStaticIterator msi( x, y, worldNumber );
	for( Static_st *stat = msi.First(); stat != nullptr; stat = msi.Next() )
	{
		SI08 tempTop = (SI08)(stat->zoff + TileHeight(stat->itemid));
		if( ( tempTop <= oldz + maxZ ) && ( tempTop > top ) )
			top = tempTop;
	}
	return top;
}

//=============================================================================
bool CMulHandler::multiExists(UI16 multinum){
	return _multidata.exists(multinum);
}


//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI32 SeekMulti( UI16 multinum )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if multinum/id can be found in multi data. Non-High Seas version
//o-----------------------------------------------------------------------------------------------o
const multi_structure &CMulHandler::SeekMulti( UI16 multinum )  const
{
	return _multidata.entry(multinum);
}

//o--------------------------------------------------------------------------
//|	Function	-	void MultiArea( CMultiObj *i, SI16 &x1, SI16 &y1, SI16 &x2, SI16 &y2 )
//|	Date		-	6th September, 1999
//|	Changes		-	26th September, 2001 - fetches from cache
//o--------------------------------------------------------------------------
//|	Purpose		-	Finds the corners of a multi object
//o--------------------------------------------------------------------------
void CMulHandler::MultiArea( CMultiObj *i, SI16 &x1, SI16 &y1, SI16 &x2, SI16 &y2 )
{
	if( !ValidateObject( i ) )
		return;
	const SI16 xAdd = i->GetX();
	const SI16 yAdd = i->GetY();

	const UI16 multiNum = static_cast<UI16>(i->GetID() - 0x4000);
	if (!multiExists(multiNum))
	{
		return ;
	}
	auto structure = SeekMulti(multiNum);
	x1 = static_cast<SI16>(structure.minx() + xAdd);
	x2 = static_cast<SI16>(structure.maxx() + xAdd);
	y1 = static_cast<SI16>(structure.miny() + yAdd);
	y2 = static_cast<SI16>(structure.maxy() + yAdd);
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 MultiHeight( CItem *i, SI16 x, SI16 y, SI08 oldz, SI08 maxZ )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Return the height of a multi item at the given x, y.
//|					This seems to actually return a height
//o-----------------------------------------------------------------------------------------------o
SI08 CMulHandler::MultiHeight( CItem *i, SI16 x, SI16 y, SI08 oldZ, SI08 maxZ, bool checkHeight )
{
	UI16 multiID = static_cast<UI16>(i->GetID() - 0x4000);
	
	SI08 mHeight = ILLEGAL_Z;
	SI08 tmpTop = ILLEGAL_Z;

	if( !multiExists(multiID) ){
		return mHeight;
	}

	const SI16 baseX = i->GetX();
	const SI16 baseY = i->GetY();
	const SI08 baseZ = i->GetZ();
	for( auto &multi : SeekMulti( multiID ).allItems() )
	{
		if(( checkHeight || multi.visible ) && ( baseX + multi.xoffset ) == x && ( baseY + multi.yoffset ) == y )
		{
			if( checkHeight )
			{
				// Returns height of highest point of multi
				tmpTop = static_cast<SI08>(baseZ + multi.zoffset);
				if( ( tmpTop <= oldZ + maxZ ) && tmpTop > oldZ && tmpTop > mHeight )
					mHeight = tmpTop;
			}
			else
			{
				tmpTop = static_cast<SI08>( baseZ + multi.zoffset );
				if( abs( tmpTop - oldZ ) <= maxZ )
					return tmpTop + TileHeight( multi.tileid );
			}
		}
	}

	return mHeight;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 DynamicElevation( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber, SI08 maxZ, UI16 instanceID )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	This was fixed to actually return the *elevation* of dynamic items at/above given coordinates
//o-----------------------------------------------------------------------------------------------o
SI08 CMulHandler::DynamicElevation( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber, SI08 maxZ, UI16 instanceID )
{
	SI08 z = ILLEGAL_Z;

	CMapRegion *MapArea = MapRegion->GetMapRegion( MapRegion->GetGridX( x ), MapRegion->GetGridY( y ), worldNumber );
	if( MapArea == nullptr )	// no valid region
		return z;
	GenericList< CItem * > *regItems = MapArea->GetItemList();
	regItems->Push();
	for( CItem *tempItem = regItems->First(); !regItems->Finished(); tempItem = regItems->Next() )
	{
		if( !ValidateObject( tempItem ) || tempItem->GetInstanceID() != instanceID )
			continue;
		if( tempItem->GetID( 1 ) >= 0x40 && tempItem->CanBeObjType( OT_MULTI ))
			z = MultiHeight( tempItem, x, y, oldz, maxZ );
		else if( tempItem->GetX() == x && tempItem->GetY() == y )
		{
			SI08 ztemp = (SI08)(tempItem->GetZ() + TileHeight( tempItem->GetID() ));
			if( ( ztemp <= oldz + maxZ ) && ztemp > z )
				z = ztemp;
		}
	}
	regItems->Pop();
	return z;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 MultiTile( CItem *i, SI16 x, SI16 y, SI08 oldz )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns ID of tile in multi at specified coordinates
//o-----------------------------------------------------------------------------------------------o
UI16 CMulHandler::MultiTile( CItem *i, SI16 x, SI16 y, SI08 oldz, bool checkVisible )
{
	if( !i->CanBeObjType( OT_MULTI ) )
		return 0;
	UI16 multiID = static_cast<UI16>(i->GetID() - 0x4000);
	SI32 length = 0;
	
	if( !multiExists( multiID ))
	{
		Console << "CMulHandler::MultiTile->Bad length in multi file. Avoiding stall." << myendl;
		const map_st map1 = Map->SeekMap( i->GetX(), i->GetY(), i->WorldNumber() );
		
		CLand& land = Map->SeekLand( map1.id );
		if( land.CheckFlag( TF_WET ) ) // is it water?
			i->SetID( 0x4001 );
		else
			i->SetID( 0x4064 );
		length = 0;
	}
	else
	{		
		// Loop through each item that makes up the multi
		// If any of those items intersect with area were are checking, return the ID of that tile
		for( auto &multi : SeekMulti( multiID ).allItems() )
		{
			if( !checkVisible )
			{
				if( ( i->GetX() + multi.xoffset == x ) && ( i->GetY() + multi.yoffset == y ) )
				{
					return multi.tileid;
				}
			}
			else if( multi.visible > 0 && ( abs( i->GetZ() + multi.zoffset - oldz ) <= 1 ) )
			{
				if( ( i->GetX() + multi.xoffset == x ) && ( i->GetY() + multi.yoffset == y ) )
				{
					return multi.tileid;
				}
			}
		}
	}
	
	return 0;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 DynTile( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber, UI16 instanceID, bool checkOnlyMultis )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns which dynamic tile is present at (x,y) or -1 if no tile exists
//o-----------------------------------------------------------------------------------------------o
CItem *CMulHandler::DynTile( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber, UI16 instanceID, bool checkOnlyMultis, bool checkOnlyNonMultis )
{
	REGIONLIST nearbyRegions = MapRegion->PopulateList( x, y, worldNumber );
	for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
	{
		CMapRegion *CellResponse = (*rIter);
		if( CellResponse == nullptr )
			continue;

		GenericList< CItem * > *regItems = CellResponse->GetItemList();
		regItems->Push();
		for( CItem *Item = regItems->First(); !regItems->Finished(); Item = regItems->Next() )
		{
			if( !ValidateObject( Item ) || Item->GetInstanceID() != instanceID )
				continue;
			if( !checkOnlyNonMultis )
			{
				if( Item->GetID( 1 ) >= 0x40 && ( Item->GetObjType() == OT_MULTI || Item->CanBeObjType( OT_MULTI ) ) )
				{
					auto deetee = MultiTile( Item, x, y, oldz, false );
					if( deetee > 0 )
					{
						regItems->Pop();
						return Item;
					}
					else
						continue;
				}
				else if( Item->GetMulti() != INVALIDSERIAL || ValidateObject( calcMultiFromSer( Item->GetOwner() ) ) )
				{
					if( Item->GetX() != x && Item->GetY() != y )
						continue;

					regItems->Pop();
					return Item;
				}
			}
			else if( !checkOnlyMultis && Item->GetX() == x && Item->GetY() == y )
			{
				regItems->Pop();
				return Item;
			}
		}
		regItems->Pop();
	}
	return nullptr;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 MapElevation( SI16 x, SI16 y, UI08 worldNumber )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the map elevation at given coordinates, we'll assume since its land
//|					the height is inherently 0
//o-----------------------------------------------------------------------------------------------o
SI08 CMulHandler::MapElevation( SI16 x, SI16 y, UI08 worldNumber )
{
	const map_st map = SeekMap( x, y, worldNumber );
	// make sure nothing can move into black areas
	if( 430 == map.id || 431 == map.id || 432 == map.id ||
	   433 == map.id || 434 == map.id || 475 == map.id ||
	   580 == map.id || 610 == map.id || 611 == map.id ||
	   612 == map.id || 613 == map.id )
		return ILLEGAL_Z;
	return map.z;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool IsValidTile( UI16 tileNum )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if a given tile number falls within the range of tiles loaded in memory
//o-----------------------------------------------------------------------------------------------o
bool CMulHandler::IsValidTile( UI16 tileNum )
{
	bool retVal = true;
	if( tileNum == INVALIDID || tileNum >= tileDataSize )
		retVal = false;

	return retVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CTile& SeekTile( UI16 tileNum )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Fetches data about a specific tile from memory. Non-High Seas version
//o-----------------------------------------------------------------------------------------------o
CTile& CMulHandler::SeekTile( UI16 tileNum )
{
	//7.0.8.2 tiledata and earlier
	if( !IsValidTile( tileNum ) )
	{
		Console.warning( strutil::format("Invalid tile access, the offending tile number is %u", tileNum) );
		static CTile emptyTile;
		return emptyTile;
	}
	else
		return staticTile[tileNum];
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CLand& SeekLand( UI16 landNum )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Fetches data about a specific land tile from memory. Non-High Seas version
//o-----------------------------------------------------------------------------------------------o
CLand& CMulHandler::SeekLand( UI16 landNum )
{
	if( landNum == INVALIDID || landNum >= LANDDATA_SIZE )
	{
		Console.warning( strutil::format("Invalid land access, the offending land number is %u", landNum) );
		static CLand emptyTile;
		return emptyTile;
	}
	else
		return landTile[landNum];
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool InsideValidWorld( SI16 x, SI16 y, UI08 worldNumber )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if a given location is within the bounds of the specified world
//o-----------------------------------------------------------------------------------------------o
bool CMulHandler::InsideValidWorld( SI16 x, SI16 y, UI08 worldNumber )
{
	if( worldNumber >= MapList.size() )
		return false;

	return ( ( x >= 0 && x < MapList[worldNumber].xBlock ) && ( y >= 0 && y < MapList[worldNumber].yBlock ) );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CStaticIterator( SI16 x, SI16 y, UI08 world, bool exact ) : baseX( static_cast<SI16>(x / 8) ),
//|					baseY( static_cast<SI16>(y / 8) ), pos( 0 ), remainX( static_cast<UI08>(x % 8) ), remainY( static_cast<UI08>(y % 8) ),
//|					index( 0 ), length( 0 ), exactCoords( exact ), worldNumber( world ), useDiffs( false )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Use this iterator class anywhere you would have used SeekInit() and SeekStatic()
//|					Unlike those functions however, it will only return the location of tiles that match your
//|					(x,y) EXACTLY.  They also should be significantly faster since the iterator saves
//|					a lot of info that was recomputed over and over and it returns a pointer instead
//|					of an entire structure on the stack.  You can call First() if you need to reset it.
//|					This iterator hides all of the map implementation from other parts of the program.
//|					If you supply the exact variable, it tells it whether to iterate over those items
//|					with your exact (x,y) otherwise it will loop over all items in the same cell as you.
//|					(Thats normally only used for filling the cache)
//|
//|					Usage:
//|						CStaticIterator msi( x, y, worldNumber );
//|
//|						for( Static_st *stat = msi.First(); stat != nullptr; stat = msi.Next() )
//|						{
//|							CTile& tile = Map->SeekTile( stat->itemid );
//|							... your code here...
//|						}
//o-----------------------------------------------------------------------------------------------o
CStaticIterator::CStaticIterator( SI16 x, SI16 y, UI08 world, bool exact ) : baseX( static_cast<SI16>(x / 8) ),
baseY( static_cast<SI16>(y / 8) ), pos( 0 ), remainX( static_cast<UI08>(x % 8) ), remainY( static_cast<UI08>(y % 8) ),
index( 0 ), length( 0 ), exactCoords( exact ), worldNumber( world ), useDiffs( false )
{
	if( !Map->InsideValidWorld( x, y, world ) )
	{
		Console.error( "ASSERT: CStaticIterator(); Not inside a valid world" );
		return;
	}

	MapData_st& mMap = Map->GetMapData( world );

	const size_t indexPos = ( static_cast<size_t>(baseX) * ( static_cast<size_t>(mMap.yBlock) / 8 ) + static_cast<size_t>(baseY) );

	std::map< UI32, StaticsIndex_st >::const_iterator diffIter = mMap.staticsDiffIndex.find( static_cast<const UI32>(indexPos) );
	if( diffIter != mMap.staticsDiffIndex.end() )
	{
		const StaticsIndex_st &sdiRecord = diffIter->second;

		if( sdiRecord.offset == INVALIDSERIAL )
		{
			length = 0;
			return;
		}

		useDiffs = true;
		pos		= sdiRecord.offset;
		length	= (sdiRecord.length / StaticRecordSize);
	}
	else
	{
		UOXFile *toRead = mMap.staidxObj;
		if( toRead == nullptr )
		{
			length = 0;
			return;
		}

		toRead->seek( (indexPos * StaticIndexRecordSize), SEEK_SET );
		if( !toRead->eof() )
		{
			toRead->getULong( &pos );
			if( pos != INVALIDSERIAL )
			{
				toRead->getULong( &length );
				length /= StaticRecordSize;
				useDiffs = false;
			}
		}
	}
}

Static_st *CStaticIterator::First( void )
{
	index = 0;
	return Next();
}

Static_st *CStaticIterator::Next( void )
{
	if( index >= length )
		return nullptr;

	MapData_st& mMap = Map->GetMapData( worldNumber );
	UOXFile *mFile = nullptr;

	if( useDiffs )
		mFile = mMap.staticsDiffObj;
	else
		mFile = mMap.staticsObj;

	if( mFile == nullptr )
		return nullptr;

	const size_t pos2 = static_cast<size_t>(pos) + static_cast<size_t>(StaticRecordSize) * static_cast<size_t>(index);	// skip over all the ones we've read so far
	mFile->seek( pos2, SEEK_SET );
	do
	{
		if( mFile->eof() )
			return nullptr;

		mFile->getUShort( &staticArray.itemid );
		mFile->getUChar( &staticArray.xoff );
		mFile->getUChar( &staticArray.yoff );
		mFile->getChar( &staticArray.zoff );
		mFile->seek( 2, SEEK_CUR );	// Skip past the unused data

		++index;
		// if these are ever larger than 7 we've gotten out of sync
		assert( staticArray.xoff < 0x08 );
		assert( staticArray.yoff < 0x08 );
		if( !exactCoords || ( staticArray.xoff == remainX && staticArray.yoff == remainY ) )
		{
#ifdef DEBUG_MAP_STUFF
			Console << "Found static at index: " << index << ", Length: " << length << ", indepos: " << pos2 << myendl;
			Console << "item is " << (SI32)staticArray.itemid << ", x" << (SI32)staticArray.xoff << ", y: " << (SI32) staticArray.yoff << myendl;
#endif
			return &staticArray;
		}
	} while( index < length );

	return nullptr;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	map_st SeekMap( SI16 x, SI16 y, UI08 worldNumber )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Fetches map data for a specific map/world
//o-----------------------------------------------------------------------------------------------o
map_st CMulHandler::SeekMap( SI16 x, SI16 y, UI08 worldNumber )
{
	map_st map = {0, 0};
	if( worldNumber >= MapList.size() )
		return map;

	MapData_st& mMap = MapList[worldNumber];

	size_t pos				= 0;
	UOXFile *mFile			= nullptr;

	const SI16 x1 = static_cast<SI16>(x >> 3), y1 = static_cast<SI16>(y >> 3);
	const UI08 x2 = static_cast<UI08>(x % 8), y2 = static_cast<UI08>(y % 8);
	const size_t blockID	= static_cast<size_t>(x1) * ( static_cast<size_t>(mMap.yBlock) >> 3) + static_cast<size_t>(y1);
	const size_t cellOffset = (4 + (static_cast<size_t>(y2) * 8 + static_cast<size_t>(x2)) * static_cast<size_t>(MapRecordSize) );

	std::map< UI32, UI32 >::const_iterator diffIter = mMap.mapDiffList.find( static_cast<const UI32>(blockID) );
	if( diffIter != mMap.mapDiffList.end() )
	{
		mFile	= mMap.mapDiffObj;
		pos		= (diffIter->second + cellOffset);
	}
	else
	{
		mFile	= mMap.mapObj;
		pos		= (blockID * MapBlockSize + cellOffset);
	}

	mFile->seek( pos, SEEK_SET );
	if( mFile->eof() )
	{
#if defined( UOX_DEBUG_MODE )
		Console.warning( strutil::format("SeekMap: Invalid map tile index %u at: X %i, Y %i", pos, x, y ));
#endif
	}
	else
	{
		mFile->getUShort( &map.id );
		mFile->getChar( &map.z );
	}

	return map;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool DoesStaticBlock( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber, bool checkWater )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if statics at/above given coordinates blocks movement, etc
//o-----------------------------------------------------------------------------------------------o
bool CMulHandler::DoesStaticBlock( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber, bool checkWater )
{
	CStaticIterator msi( x, y, worldNumber );
	for( Static_st *stat = msi.First(); stat != nullptr; stat = msi.Next() )
	{
		const SI08 elev = static_cast<SI08>(stat->zoff + TileHeight( stat->itemid ));
		CTile& tile = SeekTile( stat->itemid );
		if( checkWater )
		{
			if( elev >= oldz && stat->zoff <= oldz && ( tile.CheckFlag( TF_BLOCKING ) || tile.CheckFlag( TF_WET ) ))
				return true;
		}
		else
		{
			if( elev >= oldz && stat->zoff <= oldz && ( tile.CheckFlag( TF_BLOCKING ) && !tile.CheckFlag( TF_WET ) ))
				return true;
		}
	}
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CheckStaticFlag( SI16 x, SI16 y, SI08 z, UI08 worldNumber, TileFlags toCheck, bool checkSpawnSurface )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks to see whether any statics at given coordinates has a specific flag
//o-----------------------------------------------------------------------------------------------o
bool CMulHandler::CheckStaticFlag( SI16 x, SI16 y, SI08 z, UI08 worldNumber, TileFlags toCheck, bool checkSpawnSurface )
{
	CStaticIterator msi( x, y, worldNumber );
	for( Static_st *stat = msi.First(); stat != nullptr; stat = msi.Next() )
	{
		const SI08 elev = static_cast<SI08>( stat->zoff );
		const SI08 tileHeight = static_cast<SI08>( TileHeight( stat->itemid ) );
		if( checkSpawnSurface )
		{
			// Special case used when checking for spawn surfaces
			if(( z >= elev && z <= ( elev + tileHeight )) && !SeekTile( stat->itemid ).CheckFlag( toCheck ) )
				return false;
		}
		else
		{
			// Generic check exposed to JS
			if(( z >= elev && z <= ( elev + tileHeight )) && SeekTile( stat->itemid ).CheckFlag( toCheck ) )
				return true; // Found static with specified flag
		}		
	}
	return checkSpawnSurface;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CheckDynamicFlag( SI16 x, SI16 y, SI08 z, UI08 worldNumber, TileFlags toCheck )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks to see whether any dynamics at given coordinates has a specific flag
//o-----------------------------------------------------------------------------------------------o
bool CMulHandler::CheckDynamicFlag( SI16 x, SI16 y, SI08 z, UI08 worldNumber, UI16 instanceID, TileFlags toCheck )
{
	REGIONLIST nearbyRegions = MapRegion->PopulateList( x, y, worldNumber );
	for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
	{
		CMapRegion *CellResponse = ( *rIter );
		if( CellResponse == nullptr )
			continue;

		GenericList< CItem * > *regItems = CellResponse->GetItemList();
		regItems->Push();
		for( CItem *Item = regItems->First(); !regItems->Finished(); Item = regItems->Next() )
		{
			if( !ValidateObject( Item ) || Item->GetInstanceID() != instanceID )
				continue;

			if( Item->GetID( 1 ) >= 0x40 && ( Item->GetObjType() == OT_MULTI || Item->CanBeObjType( OT_MULTI ) ) )
			{
				// Found a multi
				// Look for a multi item at specific location
				UI16 multiID = static_cast<UI16>( Item->GetID() - 0x4000 );
				for( auto &multiItem : SeekMulti( multiID ).allItems() )
				{
					if( multiItem.visible > 0 && ( abs( Item->GetZ() + multiItem.zoffset - z ) <= 1 ) )
					{
						if( ( Item->GetX() + multiItem.xoffset == x ) && ( Item->GetY() + multiItem.yoffset == y ) )
						{
							if( SeekTile( multiItem.tileid ).CheckFlag( toCheck ) )
							{
								return true;
							}
						}
					}
				}
			}
			else
			{
				// Item is not a multi
				if( Item->GetX() == x && Item->GetY() == y && Item->GetZ() == z )
				{
					UI16 itemZ = Item->GetZ();
					const SI08 tileHeight = static_cast<SI08>( TileHeight( Item->GetID() ) );
					if(( itemZ == z && itemZ + tileHeight > z ) || ( itemZ < z && itemZ + tileHeight >= z ))
					{
						if( SeekTile( Item->GetID() ).CheckFlag( toCheck ) )
						{
							return true;
						}
					}
				}
			}
		}
		regItems->Pop();
	}
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CheckTileFlag( UI16 itemID, TileFlags flagToCheck )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks to see whether given tile ID has a specified flag
//o-----------------------------------------------------------------------------------------------o
bool CMulHandler::CheckTileFlag( UI16 itemID, TileFlags flagToCheck )
{
	if( !SeekTile( itemID ).CheckFlag( flagToCheck ))
		return false;
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 Height( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber, UI16 instanceID )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns new height of player who walked to X/Y but from OLDZ
//o-----------------------------------------------------------------------------------------------o
SI08 CMulHandler::Height( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber, UI16 instanceID )
{
	// let's check in this order.. dynamic, static, then the map
	const SI08 dynz = DynamicElevation( x, y, oldz, worldNumber, MAX_Z_STEP, instanceID );
	if( ILLEGAL_Z != dynz )
		return dynz;

	const SI08 staticz = StaticTop( x, y, oldz, worldNumber, MAX_Z_STEP );
	if( ILLEGAL_Z != staticz )
		return staticz;

	return MapElevation( x, y, worldNumber );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool inBuilding( SI16 x, SI16 y, SI08 z, UI08 worldNumber, UI16 instanceID )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns whether give coordinates are inside a building by checking if there is
//|					a multi or static above them
//o-----------------------------------------------------------------------------------------------o
bool CMulHandler::inBuilding( SI16 x, SI16 y, SI08 z, UI08 worldNumber, UI16 instanceID )
{
	const SI08 dynz = Map->DynamicElevation( x, y, z, worldNumber, (SI08)127, instanceID );
	if( dynz > ( z + 10))
		return true;
	else
	{
		const SI08 staticz = Map->StaticTop( x, y, z, worldNumber, (SI08)127 );
		if( staticz > ( z + 10))
			return true;
	}
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool DoesDynamicBlock( SI16 x, SI16 y, SI08 z, UI08 worldNumber, UI16 instanceID, 
//|							bool checkWater, bool waterWalk, bool checkOnlyMultis )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if there are any dynamic tiles at given coordinates that block movement
//o-----------------------------------------------------------------------------------------------o
bool CMulHandler::DoesDynamicBlock( SI16 x, SI16 y, SI08 z, UI08 worldNumber, UI16 instanceID, 
					     bool checkWater, bool waterWalk, bool checkOnlyMultis, bool checkOnlyNonMultis )
{
	// get the tile id of any dynamic tiles at this spot
	CItem * dtItem = DynTile( x, y, z, worldNumber, instanceID, checkOnlyMultis, checkOnlyNonMultis );
	if( ValidateObject( dtItem ))
	{
		// Don't allow placing houses on top of immovable, visible dynamic items
		if( dtItem->GetMovable() == 2 && dtItem->GetVisible() == 0 )
			return true;
		
		// Ignore items that are permanently invisible or visible to GMs only
		if( dtItem->GetVisible() == 1 || dtItem->GetVisible() == 3 )
			return false;

		const UI16 dt = dtItem->GetID();
		if( IsValidTile( dt ) && dt != 0 )
		{
			CTile &tile = SeekTile( dt );
			if( waterWalk )
			{
				if( !tile.CheckFlag( TF_WET ) )
					return true;
			}
			else
			{
				if( tile.CheckFlag( TF_ROOF ) || tile.CheckFlag( TF_BLOCKING ) || (checkWater && tile.CheckFlag( TF_WET ) ) /*|| !tile.CheckFlag( TF_SURFACE ) */ )
					return true;
			}
		}
	}
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool DoesMapBlock( SI16 x, SI16 y, SI08 z, UI08 worldNumber, bool checkWater, bool waterWalk, bool checkMultiPlacement )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if the map tile at the given coordinates block movement
//o-----------------------------------------------------------------------------------------------o
bool CMulHandler::DoesMapBlock( SI16 x, SI16 y, SI08 z, UI08 worldNumber, bool checkWater, bool waterWalk, bool checkMultiPlacement, bool checkForRoad )
{
	if( checkWater || waterWalk )
	{
		const map_st map = SeekMap( x, y, worldNumber );
		if(( checkMultiPlacement && map.z == z ) || ( !checkMultiPlacement && map.z >= z && map.z - z < 16 ))
		{
			if( z == ILLEGAL_Z )
				return true;

			UI16 landID;
			CLand& land = SeekLand( map.id );
			if( waterWalk )
			{
				if( !land.CheckFlag( TF_WET ) )
					return true;
			}
			else
			{
				if( land.CheckFlag( TF_WET ) || land.CheckFlag( TF_BLOCKING ))
					return true;
			}
			
			landID = land.TextureID();			
			
			// Check that player is not attempting to place a multi on a road
			if( checkWater && checkMultiPlacement && checkForRoad )
			{
				// List of road tiles, or road-related tiles, on which houses cannot be placed
				std::vector<UI16> roadIDs = {
					0x0009, 0x0015, // furrows
					0x0071, 0x0078, // dirt
					0x00E8, 0x00EB, // dirt
					0x0150, 0x015C, // furrows
					0x0442, 0x0479, // sand stone
					0x0501, 0x0510, // sand stone
					0x07AE, 0x07B1, // dirt
					0x3FF4, 0x3FF4, // dirt
					0x3FF8, 0x3FFB	// dirt
				};

				for( int i = 0; i < roadIDs.size(); i += 2 )
				{
					if( landID >= roadIDs[i] && landID <= roadIDs[static_cast<size_t>(i) + 1] )
						return true;
				}
			}
		}
	}
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ValidSpawnLocation( SI16 x, SI16 y, SI08 z, UI08 worldNumber, UI16 instanceID, bool checkWater )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if location at given coordinates is considered valid for spawning objects
//|					Also used to verify teleport location for NPCs teleporting back to bounding box
//o-----------------------------------------------------------------------------------------------o
bool CMulHandler::ValidSpawnLocation( SI16 x, SI16 y, SI08 z, UI08 worldNumber, UI16 instanceID, bool checkWater )
{
	if( !InsideValidWorld( x, y, worldNumber ) )
		return false;

	// get the tile id of any dynamic tiles at this spot
	if( DoesDynamicBlock( x, y, z, worldNumber, instanceID, checkWater, !checkWater, false, false ) )
		return false;

	// if there's a static block here in our way, return false
	if( DoesStaticBlock( x, y, z, worldNumber, checkWater ) )
		return false;

	// if the static isn't a surface return false
	if( !CheckStaticFlag( x, y, z, worldNumber, ( checkWater ? TF_SURFACE : TF_WET ), true ) )
		return false;

	if( DoesMapBlock( x, y, z, worldNumber, checkWater, !checkWater, false, false ) )
		return false;

	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ValidMultiLocation( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber, UI16 instanceID, bool checkWater, bool checkOnlyMultis )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks whether given location is valid for house/boat placement
//o-----------------------------------------------------------------------------------------------o
UI08 CMulHandler::ValidMultiLocation( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber, UI16 instanceID, 
						 bool checkWater, bool checkOnlyMultis, bool checkOnlyNonMultis, bool checkForRoad )
{
	if( !InsideValidWorld( x, y, worldNumber ) )
		return 0;

	const SI08 elev = Height( x, y, oldz, worldNumber, instanceID );
	if( ILLEGAL_Z == elev )
		return 0;

	// We don't want the house to be halfway embedded in a hill... or hanging off one for that matter.
	if( oldz != ILLEGAL_Z )
	{
		if( elev - oldz > MAX_Z_STEP )
		{
			return 0;
		}
		else if( oldz - elev > MAX_Z_FALL )
		{
			return 0;
		}
	}

	// get the tile id of any dynamic tiles at this spot
	if( !checkOnlyNonMultis && DoesDynamicBlock( x, y, elev, worldNumber, instanceID, checkWater, false, checkOnlyMultis, checkOnlyNonMultis ) )
	{
		return 2;
	}

	// if there's a static block here in our way, return false
	if( !checkOnlyMultis && DoesStaticBlock( x, y, elev, worldNumber, checkWater ) )
	{
		return 2;
	}

	if( DoesMapBlock( x, y, elev, worldNumber, checkWater, false, true, checkForRoad ) )
	{
		return 0;
	}

	// Check if house placement is allowed in region
	CTownRegion *calcReg = calcRegionFromXY( x, y, worldNumber, instanceID );
	if(( !calcReg->CanPlaceHouse() && checkWater) || calcReg->IsDungeon() || ( calcReg->IsGuarded() && checkWater ))
		return 3;

	// Else, all good!
	return 1;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool MapExists( UI08 worldNumber )
//|	Date		-	27th September, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if the server has that map in memory
//o-----------------------------------------------------------------------------------------------o
bool CMulHandler::MapExists( UI08 worldNumber )
{
	return ( (worldNumber < MapList.size()) && (MapList[worldNumber].mapObj != nullptr) );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	MapData_st& GetMapData( UI08 worldNumber )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns map data for given world number
//o-----------------------------------------------------------------------------------------------o
MapData_st& CMulHandler::GetMapData( UI08 worldNumber )
{
	return MapList[worldNumber];
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 MapCount( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns count of how many maps have been loaded
//o-----------------------------------------------------------------------------------------------o
UI08 CMulHandler::MapCount( void ) const
{
	return static_cast<UI08>(MapList.size());
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void LoadDFNOverrides( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Override flags/properties for tiles based on DFNDATA/MAPS/tiles.dfn
//o-----------------------------------------------------------------------------------------------o
void CMulHandler::LoadDFNOverrides( void )
{
	std::string data, UTag, entryName, titlePart;
	size_t entryNum;

	for( Script *mapScp = FileLookup->FirstScript( maps_def ); !FileLookup->FinishedScripts( maps_def ); mapScp = FileLookup->NextScript( maps_def ) )
	{
		if( mapScp == nullptr )
			continue;
		for( ScriptSection *toScan = mapScp->FirstEntry(); toScan != nullptr; toScan = mapScp->NextEntry() )
		{
			if( toScan == nullptr )
			{
				continue;
			}
			entryName	= mapScp->EntryName();
			entryNum	= strutil::value<std::uint16_t>(strutil::extractSection(entryName," ", 1, 1 ));
			titlePart	= strutil::toupper(strutil::extractSection(entryName, " ", 0, 0 ));
			// have we got an entry starting with TILE ?
			if( titlePart == "TILE" && entryNum )
			{
				if( entryNum == INVALIDID || entryNum >= tileDataSize )
				{
					continue;
				}
				
				CTile *tile = &staticTile[entryNum];
				if( tile != nullptr )
				{
					for( std::string tag = toScan->First(); !toScan->AtEnd(); tag = toScan->Next() )
					{
						data	= toScan->GrabData();
						data 	= strutil::stripTrim( data );
						UTag	= strutil::toupper( tag );
						
						// CTile properties
						if( UTag == "WEIGHT" )
						{
							tile->Weight( static_cast<UI08>(std::stoul(data, nullptr, 0)) );
						}
						else if( UTag == "HEIGHT" )
						{
							tile->Height( static_cast<SI08>(std::stoi(data, nullptr, 0))) ;
						}
						else if( UTag == "LAYER" )
						{
							tile->Layer( static_cast<SI08>(std::stoi(data, nullptr, 0))) ;
						}
						else if( UTag == "HUE" )
						{
							tile->Hue( static_cast<UI08>(std::stoul(data, nullptr, 0)) );
						}
						else if( UTag == "QUANTITY" )
						{
							tile->Quantity( static_cast<UI08>(std::stoul(data, nullptr, 0)) );
						}
						else if( UTag == "ANIMATION" )
						{
							tile->Animation( static_cast<UI16>(std::stoul(data, nullptr, 0)) );
						}
						else if( UTag == "NAME" )
						{
							tile->Name( data.c_str() );
						}
						
						// BaseTile Flag 1
						else if( UTag == "ATFLOORLEVEL" )
						{
							tile->SetFlag( TF_FLOORLEVEL, (std::stoi(data, nullptr, 0) != 0) );
						}
						else if( UTag == "HOLDABLE" )
						{
							tile->SetFlag( TF_HOLDABLE, (std::stoi(data, nullptr, 0) != 0) );
						}
						else if( UTag == "SIGNGUILDBANNER" )
						{
							tile->SetFlag( TF_TRANSPARENT, (std::stoi(data, nullptr, 0) != 0) );
						}
						else if( UTag == "WEBDIRTBLOOD" )
						{
							tile->SetFlag( TF_TRANSLUCENT, (std::stoi(data, nullptr, 0) != 0) );
						}
						else if( UTag == "WALLVERTTILE" )
						{
							tile->SetFlag( TF_WALL, (std::stoi(data, nullptr, 0) != 0) );
						}
						else if( UTag == "DAMAGING" )
						{
							tile->SetFlag( TF_DAMAGING, (std::stoi(data, nullptr, 0) != 0) );
						}
						else if( UTag == "BLOCKING" )
						{
							tile->SetFlag( TF_BLOCKING, (std::stoi(data, nullptr, 0) != 0) );
						}
						else if( UTag == "LIQUIDWET" )
						{
							tile->SetFlag( TF_WET, (std::stoi(data, nullptr, 0) != 0) );
						}
						
						// BaseTile Flag 2
						else if( UTag == "UNKNOWNFLAG1" )
						{
							tile->SetFlag( TF_UNKNOWN1, (std::stoi(data, nullptr, 0) != 0) );
						}
						else if( UTag == "STANDABLE" )
						{
							tile->SetFlag( TF_SURFACE, (std::stoi(data, nullptr, 0) != 0) );
						}
						else if( UTag == "CLIMBABLE" )
						{
							tile->SetFlag( TF_CLIMBABLE, (std::stoi(data, nullptr, 0) != 0) );
						}
						else if( UTag == "STACKABLE" )
						{
							tile->SetFlag( TF_STACKABLE, (std::stoi(data, nullptr, 0) != 0) );
						}
						else if( UTag == "WINDOWARCHDOOR" )
						{
							tile->SetFlag( TF_WINDOW, (std::stoi(data, nullptr, 0) != 0) );
						}
						else if( UTag == "CANNOTSHOOTTHRU" )
						{
							tile->SetFlag( TF_NOSHOOT, (std::stoi(data, nullptr, 0) != 0) );
						}
						else if( UTag == "DISPLAYASA" )
						{
							tile->SetFlag( TF_DISPLAYA, (std::stoi(data, nullptr, 0) != 0) );
						}
						else if( UTag == "DISPLAYASAN" )
						{
							tile->SetFlag( TF_DISPLAYAN, (std::stoi(data, nullptr, 0) != 0) );
						}
						
						// BaseTile Flag 3
						else if( UTag == "DESCRIPTIONTILE" )
						{
							tile->SetFlag( TF_DESCRIPTION, (std::stoi(data, nullptr, 0) != 0) );
						}
						else if( UTag == "FADEWITHTRANS" )
						{
							tile->SetFlag( TF_FOLIAGE, (std::stoi(data, nullptr, 0) != 0) );
						}
						else if( UTag == "PARTIALHUE" )
						{
							tile->SetFlag( TF_PARTIALHUE, (std::stoi(data, nullptr, 0) != 0) );
						}
						else if( UTag == "UNKNOWNFLAG2" )
						{
							tile->SetFlag( TF_UNKNOWN2, (std::stoi(data, nullptr, 0) != 0) );
						}
						else if( UTag == "MAP" )
						{
							tile->SetFlag( TF_MAP, (std::stoi(data, nullptr, 0) != 0) );
						}
						else if( UTag == "CONTAINER" )
						{
							tile->SetFlag( TF_CONTAINER, (std::stoi(data, nullptr, 0) != 0) );
						}
						else if( UTag == "EQUIPABLE" )
						{
							tile->SetFlag( TF_WEARABLE, (std::stoi(data, nullptr, 0) != 0) );
						}
						else if( UTag == "LIGHTSOURCE" )
						{
							tile->SetFlag( TF_LIGHT, (std::stoi(data, nullptr, 0) != 0) );
						}
						
						// BaseTile Flag 4
						else if( UTag == "ANIMATED" )
						{
							tile->SetFlag( TF_ANIMATED, (std::stoi(data, nullptr, 0) != 0) );
						}
						else if( UTag == "NODIAGONAL" )
						{
							tile->SetFlag( TF_NODIAGONAL, (std::stoi(data, nullptr, 0) != 0) );
						}
						else if( UTag == "UNKNOWNFLAG3" )
						{
							tile->SetFlag( TF_UNKNOWN3, (std::stoi(data, nullptr, 0) != 0) );
						}
						else if( UTag == "WHOLEBODYITEM" )
						{
							tile->SetFlag( TF_ARMOR, (std::stoi(data, nullptr, 0) != 0) );
						}
						else if( UTag == "WALLROOFWEAP" )
						{
							tile->SetFlag( TF_ROOF, (std::stoi(data, nullptr, 0) != 0) );
						}
						else if( UTag == "DOOR" )
						{
							tile->SetFlag( TF_DOOR, (std::stoi(data, nullptr, 0) != 0) );
						}
						else if( UTag == "CLIMBABLEBIT1" )
						{
							tile->SetFlag( TF_STAIRBACK, (std::stoi(data, nullptr, 0) != 0) );
						}
						else if( UTag == "CLIMBABLEBIT2" )
						{
							tile->SetFlag( TF_STAIRRIGHT, (std::stoi(data, nullptr, 0) != 0) );
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CTile::Read( UOXFile *toRead )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Reads and stores tiledata flags as part of loading tiledata into memory
//|					Non-High Seas version
//|
//|	Notes		-	BYTE[4]		Flags		(See BaseTile)
//|					BYTE		Weight		Weight of the item, 255 means not movable)
//|					BYTE		Quality		If Wearable, this is a Layer. If Light Source, this is Light ID.
//|					BYTE[2]		Unknown1
//|					BYTE		Unknown2
//|					BYTE		Quantity	If Weapon, Quantity is Weapon Class. If Armor, Quantity is Armor Class.
//|					BYTE[2]		Animation
//|					BYTE		Unknown3
//|					BYTE		Hue
//|					BYTE		Unknown4
//|					BYTE		Unknown5
//|					BYTE		Height		If Container, Height is "Contains" (Something determining how much the container can hold?)
//|					BYTE[20]	Name
//o-----------------------------------------------------------------------------------------------o
void CTile::Read( UOXFile *toRead,bool useHS )
{
	UI32 flagsRead;
	toRead->getULong( &flagsRead );
	flags = flagsRead;
	if( useHS )
	{
		toRead->getULong( &unknown0 );		
	}
	toRead->getUChar( &weight );
	toRead->getChar( &layer );
	toRead->getUShort( &unknown1 );
	toRead->getUChar( &unknown2 );
	toRead->getUChar( &quantity );
	toRead->getUShort( &animation );
	toRead->getUChar( &unknown3 );
	toRead->getUChar( &hue );
	toRead->getUChar( &unknown4 );
	toRead->getUChar( &unknown5 );
	toRead->getChar( &height );
	toRead->getChar( name, 20 );
	name[20] = '\0';
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CLand::Read( UOXFile *toRead )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Reads and stores land tile data as part of loading tiledata into memory
//|					Non-High Seas version
//|
//|	Notes		-	BYTE[4]		Flags
//|					BYTE[2]		TextureID
//|					BYTE[20]	Name
//o-----------------------------------------------------------------------------------------------o
void CLand::Read( UOXFile *toRead,bool useHS )
{
	UI32 flagsRead;
	toRead->getULong( &flagsRead );
	flags = flagsRead;
	if( useHS )
	{
		toRead->getULong( &unknown1 );
	}
	toRead->getUShort( &textureID );
	toRead->getChar( name, 20 );
	name[20] = '\0';
}


