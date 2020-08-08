#include "uox3.h"
#include "regions.h"
#include "fileio.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "scriptc.h"

#undef DBGFILE
#define DBGFILE "mapstuff.cpp"

namespace UOX
{

CMulHandler *Map				= NULL;

const UI16 LANDDATA_SIZE		= 0x4000; //(512 * 32)

//! these are the fixed record lengths as determined by the .mul files from OSI
//! i made them longs because they are used to calculate offsets into the files
const UI32 MultiRecordSize			= 12L;
const UI32 MultiRecordSizeHS		= 16L;
const UI32 TileRecordSize			= 26L;
const UI32 TileRecordSizeHS			= 41L;
const UI32 MapRecordSize			= 3L;
const UI32 MapRecordSizeHS			= 4L;
const UI32 MultiIndexRecordSize		= 12L;
const UI32 StaticRecordSize			= 7L;
const UI32 StaticIndexRecordSize	= 12L;
const UI32 MapBlockSize				= 196L;

/*! New Notes: 11/3/1999
** I changed all of the places where z was handled as a signed char and
** upped them to an int. While the basic terrain never goes above 127, 
** calculations sure can. For example, if you are at a piece of land at 100
** and put a 50 high static object, its going to roll over. - fur
*/

/*!
** New rewrite plans, its way confusing having every function in here using 'Height' to mean
** elevation or actual height of a tile.  Here's the new renaming plan:
** Elevation - the z value of the bottom edge of an item
** Height - the height of the item, it is independant of the elevation, like a person is 6ft tall
** Top - the z value of the top edge of an item, is always Elevation + Height of item
** -fur
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
	if( mapObj != NULL )
	{
		delete mapObj;
		mapObj = NULL;
	}
	if( staticsObj != NULL )
	{
		delete staticsObj;
		staticsObj = NULL;
	}
	if( staidxObj != NULL )
	{
		delete staidxObj;
		staidxObj = NULL;
	}
	if( mapDiffObj != NULL )
	{
		delete mapDiffObj;
		mapDiffObj = NULL;
	}
	if( staticsDiffObj != NULL )
	{
		delete staticsDiffObj;
		staticsDiffObj = NULL;
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void MultiItemsIndex_st::Include( SI16 x, SI16 y, SI08 z )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Compute new bounding box, by include the new point. < High Seas version
//o-----------------------------------------------------------------------------------------------o
void CMulHandler::MultiItemsIndex_st::Include( SI16 x, SI16 y, SI08 z )
{
	if( x < lx ) 
		lx = x;
	if( x > hx ) 
		hx = x;
	if( y < ly ) 
		ly = y;
	if( y > hy )
		hy = y;
	if( z < lz )
		lz = z;
	if( z > hz )
		hz = z;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void MultiItemsIndexHS_st::Include( SI16 x, SI16 y, SI08 z )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Compute new bounding box, by include the new point. > High Seas version
//o-----------------------------------------------------------------------------------------------o
void CMulHandler::MultiItemsIndexHS_st::Include( SI16 x, SI16 y, SI08 z )
{
	if( x < lx ) 
		lx = x;
	if( x > hx ) 
		hx = x;
	if( y < ly ) 
		ly = y;
	if( y > hy )
		hy = y;
	if( z < lz )
		lz = z;
	if( z > hz )
		hz = z;
}


/*! Use Memory-Mapped file to do caching instead
** 'verdata.mul', 'tiledata.mul', 'multis.mul' and 'multi.idx' is changed so
**  it will reads into main memory and access directly.  'map*.mul', 'statics*.mul' and
**  'staidx.mul' is being managed by memory-mapped files, it faster than manual caching 
**  and less susceptible to bugs.
** -lingo
*/
CMulHandler::CMulHandler() : landTile( 0 ), landTileHS( 0 ), staticTile( 0 ), staticTileHS( 0 ), multiItems( 0 ), multiItemsHS( 0 ), multiIndex( 0 ), multiIndexHS( 0 ), multiIndexSize( 0 ), multiSize( 0 ), tileDataSize( 0 )
{
	LoadMapsDFN();
}

CMulHandler::~CMulHandler()
{
	if ( landTile )
		delete[] landTile;
	if ( landTileHS )
		delete[] landTileHS;
	if ( staticTile )   
		delete[] staticTile;
	if ( staticTileHS )   
		delete[] staticTileHS;
	if ( multiItems )  
		delete[] multiItems;
	if( multiItemsHS )
		delete[] multiItemsHS;
	if ( multiIndex )
		delete[] multiIndex;
	if( multiIndexHS )
		delete[] multiIndexHS;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void LoadMapsDFN( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads maps.dfn to see which map files to actually load into memory later
//o-----------------------------------------------------------------------------------------------o
void CMulHandler::LoadMapsDFN( void )
{
	UString tag, data, UTag;
	const UI08 NumberOfWorlds = static_cast<UI08>(FileLookup->CountOfEntries( maps_def ));
	MapList.reserve( NumberOfWorlds );
	for( UI08 i = 0; i < NumberOfWorlds; ++i )
	{
		ScriptSection *toFind = FileLookup->FindEntry( "MAP " + UString::number( i ), maps_def );
		if( toFind == NULL )
			break;

		MapData_st toAdd;
		for( tag = toFind->First(); !toFind->AtEnd(); tag = toFind->Next() )
		{
			UTag = tag.upper();
			data = toFind->GrabData();
			switch( (UTag.data()[0]) )
			{
			case 'M':
				if( UTag == "MAP" )
					toAdd.mapFile = data;
				else if( UTag == "MAPDIFF" )
					toAdd.mapDiffFile = data;
				else if( UTag == "MAPDIFFLIST" )
					toAdd.mapDiffListFile = data;
				else if( UTag == "MAPUOPWRAP" )
					toAdd.mapFileUOPWrap = data;
				break;
			case 'S':
				if( UTag == "STATICS" )
					toAdd.staticsFile = data;
				else if( UTag == "STAIDX" )
					toAdd.staidxFile = data;
				else if( UTag == "STATICSDIFF" )
					toAdd.staticsDiffFile = data;
				else if( UTag == "STATICSDIFFLIST" )
					toAdd.staticsDiffListFile = data;
				else if( UTag == "STATICSDIFFINDEX" )
					toAdd.staticsDiffIndexFile = data;
				break;
			case 'X':
				if( UTag == "X" )
					toAdd.xBlock = data.toUShort();
				break;
			case 'Y':
				if( UTag == "Y" )
					toAdd.yBlock = data.toUShort();
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

	if( toLoad != NULL && toLoad->ready() )
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
	UString mapMUL		= mMap.mapFile;
	UString mapUOPWrap	= mMap.mapFileUOPWrap;
	UString lName		= basePath + mapMUL;
	mMap.mapObj			= new UOXFile( lName.c_str(), "rb" );
	Console << "\t" << lName << "(/" << mapUOPWrap << ")\t\t";

	//if no map0.mul was found, check if there's a map#LegacyMul.uop
	if(( mMap.mapObj == NULL || !mMap.mapObj->ready() ) && !mapUOPWrap.empty() )
	{
		UString lName	= basePath + mapUOPWrap;
		mMap.mapObj		= new UOXFile( lName.c_str(), "rb" );
		if( mMap.mapObj->ready() )
		{
			cwmWorldState->ServerData()->MapIsUOPWrapped( totalMaps, true );
		}
	}
	if( mMap.mapObj != NULL && mMap.mapObj->ready() )
	{
		SI32 checkSize = (mMap.mapObj->getLength() / MapBlockSize);
		if( checkSize / (mMap.xBlock/8) == (mMap.yBlock/8) )
		{
			++totalMaps;
			Console.PrintDone();
		}
		else
			Console.PrintFailed();
	}
	else
		Console.PrintSpecial( CRED, "not found" );

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
					mMap.mapDiffList[blockID] = offset;
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
					sdiFile.read( (char *)&mMap.staticsDiffIndex[blockID].offset, 4 );
					sdiFile.read( (char *)&mMap.staticsDiffIndex[blockID].length, 4 );
					sdiFile.read( (char *)&mMap.staticsDiffIndex[blockID].unknown, 4 );
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
	UString lName = basePath + "tiledata.mul";
	Console << "\t" << lName << "\t";

	UOXFile tileFile( lName.c_str(), "rb" );
	if( !tileFile.ready() )
	{
		Console.PrintFailed();
		Shutdown( FATAL_UOX3_TILEDATA_NOT_FOUND );
	}

	//Grab length of tileFile to determine version of datafiles used
	size_t tileFileLength = tileFile.getLength();
	if( tileFileLength >= 3188736 )
	{
		// 7.0.9.0 tiledata and above
		cwmWorldState->ServerData()->ServerUsingHSTiles( true );

		landTileHS		= new CLandHS[LANDDATA_SIZE];
		CLandHS *landPtr	= landTileHS;
		for( UI16 i = 0; i < 512; ++i )
		{
			tileFile.seek(4, SEEK_CUR);			// skip the dummy header
			for( j = 0; j < 32; ++j )
				landPtr[j].Read( &tileFile );
			landPtr += 32;
		}

		tileDataSize		= (((tileFile.getLength() - (((32 * TileRecordSizeHS) + 4) * 512)) / 1188) * 32);
		staticTileHS		= new CTileHS[tileDataSize];
		CTileHS *tilePtr	= staticTileHS;

		while( !tileFile.eof() )
		{
			tileFile.seek(4, SEEK_CUR);			// skip the dummy header
			for( j = 0; j < 32; ++j )
				tilePtr[j].Read( &tileFile );
			tilePtr += 32;
		}
	}
	else
	{
		// 7.0.8.2 tiledata and below
		cwmWorldState->ServerData()->ServerUsingHSTiles( false );

		landTile		= new CLand[LANDDATA_SIZE];
		CLand *landPtr	= landTile;
		for( UI16 i = 0; i < 512; ++i )	
		{
			tileFile.seek(4, SEEK_CUR);			// skip the dummy header
			for( j = 0; j < 32; ++j )
				landPtr[j].Read( &tileFile );
			landPtr += 32;
		}

		tileDataSize	= (((tileFile.getLength() - (((32 * TileRecordSize) + 4) * 512)) / 1188) * 32);
		staticTile		= new CTile[tileDataSize];
		CTile *tilePtr	= staticTile;

		while( !tileFile.eof() )
		{
			tileFile.seek(4, SEEK_CUR);			// skip the dummy header
			for( j = 0; j < 32; ++j )
				tilePtr[j].Read( &tileFile );
			tilePtr += 32;
		}
	}
	Console.PrintDone();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void Load( void )
//|	Date		-	03/12/2002
//|	Programmer	-	Unknown / EviLDeD 
//|	Org/Team	-	UOX3 DevTeam
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
		Console.Error( " Fatal Error: No maps found" );
		Console.Error( " Check the settings for DATADIRECTORY in uox.ini" );
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

	// only turn it off for now because we are trying to fill the cache.
	UString lName	= basePath + "multi.idx";
	UOXFile multiIDX( lName.c_str(), "rb" );
	if( !multiIDX.ready() )
	{
		Console.PrintFailed();
		Console.Error( "Can't cache %s!  File cannot be opened", lName.c_str() );
		return;
	}
	lName			= basePath + "multi.mul";
	UOXFile multis( lName.c_str(), "rb" );
	if( !multis.ready() )
	{
		Console.PrintFailed();
		Console.Error( "Can't cache %s!  File cannot be opened", lName.c_str() );
		return;
	}

	// multiLength determines version of datafiles used by server
	//	994832 - 7.0.85.15 - possibly earlier
	//	962928 - 7.0.15.1 to 7.0.23.1 - Additional SI32 unknown1 property added
	//	908592 - 7.0.9.0
	//	596976 - 7.0.8.2
	//	593616 - 5.0.9.1 to 6.1.14.1
	//	592560 - 4.0.11c
	size_t multiLength = multis.getLength();
	multiSize	= multis.getLength() / (( multiLength >= 908592 ) ? MultiRecordSizeHS : MultiRecordSize );

	//! first reads in Multi_st completely
	if( multiLength >= 908592 )
	{
		cwmWorldState->ServerData()->ServerUsingHSMultis( true );

		multiItemsHS	= new MultiHS_st[multiSize];
		for( size_t i = 0; i < multiSize; ++i )
		{
			multis.getUShort( &multiItemsHS[i].tile );
			multis.getShort( &multiItemsHS[i].x );
			multis.getShort( &multiItemsHS[i].y );
			multis.getChar( &multiItemsHS[i].z );
			multis.getChar( &multiItemsHS[i].empty );
			multis.getLong( &multiItemsHS[i].visible );
			multis.getLong( &multiItemsHS[i].unknown1 );
		}
		multiIndexSize	= multiIDX.getLength() / MultiIndexRecordSize;
		multiIndexHS	= new MultiItemsIndexHS_st[multiIndexSize];

		// now rejig the multiIDX to point to the cache directly, and calculate the size
		for( MultiItemsIndexHS_st *ptr = multiIndexHS; ptr != (multiIndexHS+multiIndexSize); ++ptr )
		{
			MultiIndex_st multiidx;
			multiIDX.getLong( &multiidx.start );
			multiIDX.getLong( &multiidx.length );
			multiIDX.getLong( &multiidx.unknown );

			ptr->size = multiidx.length;
			if( ptr->size != -1 )
			{
				ptr->size /= MultiRecordSizeHS;		// convert byte size to record size
				ptr->items = (MultiHS_st*)((char*)multiItemsHS + multiidx.start);
				for( MultiHS_st* items = ptr->items; items != (ptr->items+ptr->size); ++items )
				{
					ptr->Include( items->x, items->y, items->z );
				}
			}
		}
	}
	else
	{
		cwmWorldState->ServerData()->ServerUsingHSMultis( false );

		multiItems		= new Multi_st[multiSize];
		for( size_t i = 0; i < multiSize; ++i )
		{
			multis.getUShort( &multiItems[i].tile );
			multis.getShort( &multiItems[i].x );
			multis.getShort( &multiItems[i].y );
			multis.getChar( &multiItems[i].z );
			multis.getChar( &multiItems[i].empty );
			multis.getLong( &multiItems[i].visible );
		}
		multiIndexSize	= multiIDX.getLength() / MultiIndexRecordSize;
		multiIndex		= new MultiItemsIndex_st[multiIndexSize];

		// now rejig the multiIDX to point to the cache directly, and calculate the size
		for( MultiItemsIndex_st *ptr = multiIndex; ptr != (multiIndex+multiIndexSize); ++ptr )
		{
			MultiIndex_st multiidx;
			multiIDX.getLong( &multiidx.start );
			multiIDX.getLong( &multiidx.length );
			multiIDX.getLong( &multiidx.unknown );

			ptr->size = multiidx.length;
			if( ptr->size != -1 )
			{
				ptr->size /= MultiRecordSize;		// convert byte size to record size
				ptr->items = (Multi_st*)((char*)multiItems + multiidx.start);
				for( Multi_st* items = ptr->items; items != (ptr->items+ptr->size); ++items )
				{
					ptr->Include( items->x, items->y, items->z );
				}
			}
		}
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
	if( cwmWorldState->ServerData()->ServerUsingHSTiles() ) //7.0.9.0 tiledata and later
		return (LANDDATA_SIZE * sizeof( CLandHS ) + tileDataSize * sizeof( CTileHS ));
	else //7.0.8.2 tiledata and earlier
		return (LANDDATA_SIZE * sizeof( CLand ) + tileDataSize * sizeof( CTile ));
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	size_t GetMultisMem( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns amount of memory used to load multis
//o-----------------------------------------------------------------------------------------------o
size_t CMulHandler::GetMultisMem( void ) const
{
	if( cwmWorldState->ServerData()->ServerUsingHSMultis() ) //7.0.9.0 tiledata and later
		return (multiSize * sizeof( MultiHS_st ) + multiIndexSize * sizeof( MultiItemsIndexHS_st ) );
	else //7.0.8.2 tiledata and earlier
		return (multiSize * sizeof( Multi_st ) + multiIndexSize * sizeof( MultiItemsIndex_st ) );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 TileHeight( UI16 tilenum )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Height of a gien tile (If climbable we return 1/2 its height).
//o-----------------------------------------------------------------------------------------------o
SI08 CMulHandler::TileHeight( UI16 tilenum )
{
	if( cwmWorldState->ServerData()->ServerUsingHSTiles() )
	{
		//7.0.9.0 tiledata and later
		CTileHS& tile = SeekTileHS( tilenum );
		// For Stairs+Ladders
		if( tile.CheckFlag( TF_CLIMBABLE ) ) 
			return static_cast<SI08>(tile.Height()/2);
		return tile.Height();
	}
	else
	{
		//7.0.8.2 tiledata and earlier
		CTile& tile = SeekTile( tilenum );
		// For Stairs+Ladders
		if( tile.CheckFlag( TF_CLIMBABLE ) ) 
			return static_cast<SI08>(tile.Height()/2);
		return tile.Height();
	}		
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
	for( Static_st *stat = msi.First(); stat != NULL; stat = msi.Next() )
	{
		SI08 tempTop = (SI08)(stat->zoff + TileHeight(stat->itemid));
		if( ( tempTop <= oldz + maxZ ) && ( tempTop > top ) )
			top = tempTop;
	}
	return top;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI32 SeekMulti( UI16 multinum )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if multinum/id can be found in multi data. Non-High Seas version
//o-----------------------------------------------------------------------------------------------o
SI32 CMulHandler::SeekMulti( UI16 multinum )
{
	SI32 retVal = -1;
	if( multinum < multiIndexSize )
		retVal = multiIndex[multinum].size;
	return retVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI32 SeekMultiHS( UI16 multinum )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if multinum/id can be found in multi data. High Seas version
//o-----------------------------------------------------------------------------------------------o
SI32 CMulHandler::SeekMultiHS( UI16 multinum )
{
	SI32 retVal = -1;
	if( multinum < multiIndexSize )
		retVal = multiIndexHS[multinum].size;
	return retVal;
}

Multi_st& CMulHandler::SeekIntoMulti( UI16 multinum, SI32 number )
{
	return *(multiIndex[multinum].items + number);
}

MultiHS_st& CMulHandler::SeekIntoMultiHS( UI16 multinum, SI32 number )
{
	return *(multiIndexHS[multinum].items + number);
}

//o--------------------------------------------------------------------------
//|	Function	-	void MultiArea( CMultiObj *i, SI16 &x1, SI16 &y1, SI16 &x2, SI16 &y2 )
//|	Date		-	6th September, 1999
//|	Programmer	-	Crackerjack
//|	Changes		-	Abaddon, 26th September, 2001 - fetches from cache
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
	if( multiNum >= multiIndexSize )
		return;

	if( cwmWorldState->ServerData()->ServerUsingHSMultis() )
	{
		//7.0.9.0 tiledata and later
		x1 = static_cast<SI16>(multiIndexHS[multiNum].lx + xAdd); 
		x2 = static_cast<SI16>(multiIndexHS[multiNum].hx + xAdd);
		y1 = static_cast<SI16>(multiIndexHS[multiNum].ly + yAdd);
		y2 = static_cast<SI16>(multiIndexHS[multiNum].hy + yAdd);
	}
	else
	{
		//7.0.8.2 tiledata and earlier
		x1 = static_cast<SI16>(multiIndex[multiNum].lx + xAdd); 
		x2 = static_cast<SI16>(multiIndex[multiNum].hx + xAdd);
		y1 = static_cast<SI16>(multiIndex[multiNum].ly + yAdd);
		y2 = static_cast<SI16>(multiIndex[multiNum].hy + yAdd);
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 MultiHeight( CItem *i, SI16 x, SI16 y, SI08 oldz, SI08 maxZ )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Return the height of a multi item at the given x, y. 
//|					This seems to actually return a height
//o-----------------------------------------------------------------------------------------------o
SI08 CMulHandler::MultiHeight( CItem *i, SI16 x, SI16 y, SI08 oldz, SI08 maxZ )
{
	UI16 multiID = static_cast<UI16>(i->GetID() - 0x4000);
	SI32 length = 0;
	if( cwmWorldState->ServerData()->ServerUsingHSMultis() )
		length = SeekMultiHS( multiID ); //7.0.9.0 tiledata and later
	else 
		length = SeekMulti( multiID ); //7.0.8.2 tiledata and earlier

	if( length == -1 || length >= 17000000 ) //Too big... bug fix hopefully (Abaddon 13 Sept 1999)                                                                                                                          
		length = 0;

	const SI16 baseX = i->GetX();
	const SI16 baseY = i->GetY();
	const SI08 baseZ = i->GetZ();
	if( cwmWorldState->ServerData()->ServerUsingHSMultis() )
	{
		for( SI32 j = 0; j < length; ++j )
		{
			MultiHS_st& multi = SeekIntoMultiHS( multiID, j );
			if( multi.visible && (baseX + multi.x) == x && (baseY + multi.y) == y )
			{
				SI08 tmpTop = static_cast<SI08>(baseZ + multi.z);
				if( abs( tmpTop - oldz ) <= maxZ )
					return tmpTop + TileHeight( multi.tile );
			}                                                                                                                 
		}
	}
	else
	{
		for( SI32 j = 0; j < length; ++j )
		{
			Multi_st& multi = SeekIntoMulti( multiID, j );
			if( multi.visible && (baseX + multi.x) == x && (baseY + multi.y) == y )
			{
				SI08 tmpTop = static_cast<SI08>(baseZ + multi.z);
				if( abs( tmpTop - oldz ) <= maxZ )
					return tmpTop + TileHeight( multi.tile );
			}                                                                                                                 
		}
	}
	return ILLEGAL_Z;                                                                                                                     
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
	if( MapArea == NULL )	// no valid region
		return z;
	CDataList< CItem * > *regItems = MapArea->GetItemList();
	regItems->Push();
	for( CItem *tempItem = regItems->First(); !regItems->Finished(); tempItem = regItems->Next() )
	{
		if( !ValidateObject( tempItem ) || tempItem->GetInstanceID() != instanceID )
			continue;
		if( tempItem->GetID( 1 ) >= 0x40 )
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
UI16 CMulHandler::MultiTile( CItem *i, SI16 x, SI16 y, SI08 oldz )
{
	if( !i->CanBeObjType( OT_MULTI ) )
		return 0;
	UI16 multiID = static_cast<UI16>(i->GetID() - 0x4000);
	SI32 length = 0;
	if( cwmWorldState->ServerData()->ServerUsingHSTiles() )
		length = SeekMultiHS( multiID ); //7.0.9.0 tiledata and later
	else
		length = SeekMulti( multiID ); //7.0.8.2 tiledata and earlier
	if( length == -1 || length >= 17000000 )//Too big... bug fix hopefully (Abaddon 13 Sept 1999)
	{
		Console << "CMulHandler::MultiTile->Bad length in multi file. Avoiding stall." << myendl;
		const map_st map1 = Map->SeekMap( i->GetX(), i->GetY(), i->WorldNumber() );
		if( cwmWorldState->ServerData()->ServerUsingHSTiles() )
		{
			//7.0.9.0 tiledata and later
			CLandHS& land = Map->SeekLandHS( map1.id );
			if( land.CheckFlag( TF_WET ) ) // is it water?
				i->SetID( 0x4001 );
			else
				i->SetID( 0x4064 );
		}
		else
		{
			//7.0.8.2 tiledata and earlier
			CLand& land = Map->SeekLand( map1.id );
			if( land.CheckFlag( TF_WET ) ) // is it water?
				i->SetID( 0x4001 );
			else
				i->SetID( 0x4064 );
		}		
		length = 0;
	}

	if( cwmWorldState->ServerData()->ServerUsingHSMultis() )
	{
		for( SI32 j = 0; j < length; ++j )
		{
			MultiHS_st& multi = SeekIntoMultiHS( multiID, j );
			if( ( multi.visible && ( i->GetX() + multi.x == x) && (i->GetY() + multi.y == y)
				&& ( abs( i->GetZ() + multi.z - oldz ) <= 1 ) ) )
			{
				return multi.tile;
			}
		}
	}
	else
	{
		for( SI32 j = 0; j < length; ++j )
		{
			Multi_st& multi = SeekIntoMulti( multiID, j );
			if( ( multi.visible && ( i->GetX() + multi.x == x) && (i->GetY() + multi.y == y)
				&& ( abs( i->GetZ() + multi.z - oldz ) <= 1 ) ) )
			{
				return multi.tile;
			}
		}
	}
	return 0;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 DynTile( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber, UI16 instanceID )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns which dynamic tile is present at (x,y) or -1 if no tile exists
//o-----------------------------------------------------------------------------------------------o
UI16 CMulHandler::DynTile( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber, UI16 instanceID )
{
	CMapRegion *MapArea = MapRegion->GetMapRegion( MapRegion->GetGridX( x ), MapRegion->GetGridY( y ), worldNumber );
	if( MapArea == NULL )	// no valid region
		return INVALIDID;

	CDataList< CItem * > *regItems = MapArea->GetItemList();
	regItems->Push();
	for( CItem *tempItem = regItems->First(); !regItems->Finished(); tempItem = regItems->Next() )
	{
		if( !ValidateObject( tempItem ) || tempItem->GetInstanceID() != instanceID )
			continue;
		if( tempItem->GetID( 1 ) >= 0x40 )
		{
			regItems->Pop();
			return MultiTile( tempItem, x, y, oldz );
		}
		else if( tempItem->GetX() == x && tempItem->GetY() == y )
		{
			regItems->Pop();
			return tempItem->GetID();
		}
	}
	regItems->Pop();
	return INVALIDID;
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
		Console.Warning( "Invalid tile access, the offending tile number is %u", tileNum );
		static CTile emptyTile;
		return emptyTile;
	}
	else
		return staticTile[tileNum];
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CTileHS& SeekTileHS( UI16 tileNum )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Fetches data about a specific tile from memory. High Seas version
//o-----------------------------------------------------------------------------------------------o
CTileHS& CMulHandler::SeekTileHS( UI16 tileNum )
{
	//7.0.9.2 tiledata and later
	if( !IsValidTile( tileNum ) )
	{
		Console.Warning( "Invalid tile access, the offending tile number is %u", tileNum );
		static CTileHS emptyTile;
		return emptyTile;
	}
	else
		return staticTileHS[tileNum];
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
		Console.Warning( "Invalid land access, the offending land number is %u", landNum );
		static CLand emptyTile;
		return emptyTile;
	}
	else
		return landTile[landNum];
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CLandHS& SeekLandHS( UI16 landNum )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Fetches data about a specific land tile from memory. High Seas version
//o-----------------------------------------------------------------------------------------------o
CLandHS& CMulHandler::SeekLandHS( UI16 landNum )
{
	if( landNum == INVALIDID || landNum >= LANDDATA_SIZE )
	{
		Console.Warning( "Invalid land access, the offending land number is %u", landNum );
		static CLandHS emptyTile;
		return emptyTile;
	}
	else
		return landTileHS[landNum];
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
//|						for( Static_st *stat = msi.First(); stat != NULL; stat = msi.Next() )
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
		Console.Error( "ASSERT: CStaticIterator(); Not inside a valid world" );
		return;
	}

	MapData_st& mMap = Map->GetMapData( world );

	const size_t indexPos = (baseX * (mMap.yBlock/8) + baseY);

	std::map< UI32, StaticsIndex_st >::const_iterator diffIter = mMap.staticsDiffIndex.find( indexPos );
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
		if( toRead == NULL )
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
		return NULL;

	MapData_st& mMap = Map->GetMapData( worldNumber );
	UOXFile *mFile = NULL;
	
	if( useDiffs )
		mFile = mMap.staticsDiffObj;
	else
		mFile = mMap.staticsObj;

	if( mFile == NULL )
		return NULL;

	const size_t pos2 = static_cast<size_t>(pos + StaticRecordSize * index);	// skip over all the ones we've read so far
	mFile->seek( pos2, SEEK_SET );
	do
	{
		if( mFile->eof() )
			return NULL;

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
			Console << "item is " << (int)staticArray.itemid << ", x" << (int)staticArray.xoff << ", y: " << (int) staticArray.yoff << myendl;
#endif
			return &staticArray;
		}
	} while( index < length );
	
	return NULL;
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
	UOXFile *mFile			= NULL;

	const SI16 x1 = static_cast<SI16>(x >> 3), y1 = static_cast<SI16>(y >> 3);
	const UI08 x2 = static_cast<UI08>(x % 8), y2 = static_cast<UI08>(y % 8);
	const size_t blockID	= x1 * (mMap.yBlock >> 3) + y1;
	const size_t cellOffset = (4 + (y2 * 8 + x2) * MapRecordSize );

	std::map< UI32, UI32 >::const_iterator diffIter = mMap.mapDiffList.find( blockID );
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
	if( cwmWorldState->ServerData()->MapIsUOPWrapped( worldNumber ) == true )
	{
		SI32 blockOffset = pos / 0xC4000;
		pos += 3464 + ( 3412 * ( blockOffset / 100 )) + ( 12 * blockOffset );
	}
	mFile->seek( pos, SEEK_SET );
	if( mFile->eof() )
	{
#if defined( UOX_DEBUG_MODE )
		Console.Warning( "SeekMap: Invalid map tile index %u at: X %i, Y %i", pos, x, y );
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
	for( Static_st *stat = msi.First(); stat != NULL; stat = msi.Next() )
	{
		const SI08 elev = static_cast<SI08>(stat->zoff + TileHeight( stat->itemid ));
		if( cwmWorldState->ServerData()->ServerUsingHSTiles() )
		{
			//7.0.9.0 data and later
			CTileHS& tile = SeekTileHS( stat->itemid );
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
		else
		{
			//7.0.8.2 data and earlier
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
	}
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CheckStaticFlag( SI16 x, SI16 y, SI08 z, UI08 worldNumber, TileFlags toCheck )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks to see whether any statics at given coordinates has a specific flag
//o-----------------------------------------------------------------------------------------------o
bool CMulHandler::CheckStaticFlag( SI16 x, SI16 y, SI08 z, UI08 worldNumber, TileFlags toCheck )
{
	CStaticIterator msi( x, y, worldNumber );
	for( Static_st *stat = msi.First(); stat != NULL; stat = msi.Next() )
	{
		const SI08 elev = static_cast<SI08>(stat->zoff + TileHeight( stat->itemid ));
		if( cwmWorldState->ServerData()->ServerUsingHSTiles() )
		{
			//7.0.9.0 data and later
			if( elev == z && !SeekTileHS( stat->itemid ).CheckFlag( toCheck ) )
				return false;
		}
		else
		{
			//7.0.8.2 data and earlier
			if( elev == z && !SeekTile( stat->itemid ).CheckFlag( toCheck ) )
				return false;
		}
	}
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
//|	Function	-	bool DoesDynamicBlock( SI16 x, SI16 y, SI08 z, UI08 worldNumber, UI16 instanceID, bool checkWater, bool waterWalk )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if there are any dynamic tiles at given coordinates that block movement
//o-----------------------------------------------------------------------------------------------o
bool CMulHandler::DoesDynamicBlock( SI16 x, SI16 y, SI08 z, UI08 worldNumber, UI16 instanceID, bool checkWater, bool waterWalk )
{
    // get the tile id of any dynamic tiles at this spot
    const UI16 dt = DynTile( x, y, z, worldNumber, instanceID );
	if( IsValidTile( dt ) )
	{
		if( cwmWorldState->ServerData()->ServerUsingHSTiles() )
		{
			//7.0.9.0 data and later
			CTileHS &tile = SeekTileHS( dt );
			if( waterWalk )
			{
				if( !tile.CheckFlag( TF_WET ) )
					return true;
			}
			else
			{
				if( tile.CheckFlag( TF_BLOCKING ) || (checkWater && tile.CheckFlag( TF_WET ) ) /*|| !tile.CheckFlag( TF_SURFACE ) */ )
					return true;
			}
		}
		else
		{
			//7.0.8.2 data and earlier
			CTile &tile = SeekTile( dt );
			if( waterWalk )
			{
				if( !tile.CheckFlag( TF_WET ) )
					return true;
			}
			else
			{
				if( tile.CheckFlag( TF_BLOCKING ) || (checkWater && tile.CheckFlag( TF_WET ) ) /*|| !tile.CheckFlag( TF_SURFACE ) */ )
					return true;
			}
		}
	}
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool DoesMapBlock( SI16 x, SI16 y, SI08 z, UI08 worldNumber, bool checkWater, bool waterWalk )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if the map tile at the given coordinates block movement
//o-----------------------------------------------------------------------------------------------o
bool CMulHandler::DoesMapBlock( SI16 x, SI16 y, SI08 z, UI08 worldNumber, bool checkWater, bool waterWalk )
{
	if( checkWater || waterWalk )
	{
		const map_st map = SeekMap( x, y, worldNumber );
		if( map.z == z )
		{
			if( z == ILLEGAL_Z )
				return true;
			if( cwmWorldState->ServerData()->ServerUsingHSTiles() )
			{
				//7.0.9.0 tiledata and later
				CLandHS& land = SeekLandHS( map.id );
				if( land.CheckFlag( TF_BLOCKING ) ) // is it impassable?
					return true;
				if( waterWalk )
				{
					if( !land.CheckFlag( TF_WET ) )
						return true;
				}
				else
				{
					if( land.CheckFlag( TF_WET ) )
						return true;
				}
			}
			else
			{
				//7.0.8.2 tiledata and earlier
				CLand& land = SeekLand( map.id );
				if( land.CheckFlag( TF_BLOCKING ) ) // is it impassable?
					return true;
				if( waterWalk )
				{
					if( !land.CheckFlag( TF_WET ) )
						return true;
				}
				else
				{
					if( land.CheckFlag( TF_WET ) )
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
	if( DoesDynamicBlock( x, y, z, worldNumber, checkWater, !checkWater, instanceID ) )
		return false;

	// if there's a static block here in our way, return false
	if( DoesStaticBlock( x, y, z, worldNumber, checkWater ) )
		return false;

	// if the static isn't a surface return false
	if( !CheckStaticFlag( x, y, z, worldNumber, ( checkWater ? TF_SURFACE : TF_WET ) ) )
		return false;

	if( DoesMapBlock( x, y, z, worldNumber, checkWater, !checkWater ) )
		return false;

	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ValidMultiLocation( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber, UI16 instanceID, bool checkWater )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks whether given location is valid for house/boat placement
//o-----------------------------------------------------------------------------------------------o
bool CMulHandler::ValidMultiLocation( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber, UI16 instanceID, bool checkWater )
{
	if( !InsideValidWorld( x, y, worldNumber ) )
		return false;

    const SI08 elev = Height( x, y, oldz, worldNumber, instanceID );
	if( ILLEGAL_Z == elev )
		return false;

	// We don't want the house to be halfway embedded in a hill... or hanging off one for that matter.
	if( oldz != ILLEGAL_Z )
	{
		if( elev - oldz > MAX_Z_STEP )
			return false;
		else if( oldz - elev > MAX_Z_FALL )
			return false;
	}

    // get the tile id of any dynamic tiles at this spot
	if( DoesDynamicBlock( x, y, elev, worldNumber, checkWater, instanceID, false ) )
		return false;

	// if there's a static block here in our way, return false
	if( DoesStaticBlock( x, y, elev, worldNumber, checkWater ) )
		return false;

	if( DoesMapBlock( x, y, elev, worldNumber, checkWater, false ) )
		return false;

	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool MapExists( UI08 worldNumber )
//|	Date		-	27th September, 2001
//|	Programmer	-	Abaddon
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if the server has that map in memory
//o-----------------------------------------------------------------------------------------------o
bool CMulHandler::MapExists( UI08 worldNumber )
{
	return ( (worldNumber < MapList.size()) && (MapList[worldNumber].mapObj != NULL) );
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
	UString data, UTag, entryName, titlePart;
	size_t entryNum;

	for( Script *mapScp = FileLookup->FirstScript( maps_def ); !FileLookup->FinishedScripts( maps_def ); mapScp = FileLookup->NextScript( maps_def ) )
	{
		if( mapScp == NULL )
			continue;
		for( ScriptSection *toScan = mapScp->FirstEntry(); toScan != NULL; toScan = mapScp->NextEntry() )
		{
			if( toScan == NULL )
				continue;
			entryName	= mapScp->EntryName();
			entryNum	= entryName.section( " ", 1, 1 ).toULong();
			titlePart	= entryName.section( " ", 0, 0 ).upper();
			// have we got an entry starting with TILE ?
			if( titlePart == "TILE" && entryNum )
			{
				if( entryNum == INVALIDID || entryNum >= tileDataSize )
					continue;
				if( cwmWorldState->ServerData()->ServerUsingHSTiles() )
				{
					//7.0.9.0 data and later
					CTileHS *tile = &staticTileHS[entryNum];
					if( tile != NULL )
					{
						for( UString tag = toScan->First(); !toScan->AtEnd(); tag = toScan->Next() )
						{
							data	= toScan->GrabData();
							UTag	= tag.upper();

							// CTile properties
							if( UTag == "WEIGHT" )
								tile->Weight( data.toUByte() );
							else if( UTag == "HEIGHT" )
								tile->Height( data.toByte() );
							else if( UTag == "LAYER" )
								tile->Layer( data.toByte() );
							else if( UTag == "HUE" )
								tile->Hue( data.toUByte() );
							else if( UTag == "QUANTITY" )
								tile->Quantity( data.toUByte() );
							else if( UTag == "ANIMATION" )
								tile->Animation( data.toUShort() );
							else if( UTag == "NAME" )
								tile->Name( data.c_str() );

							// BaseTile Flag 1
							else if( UTag == "ATFLOORLEVEL" )
								tile->SetFlag( TF_FLOORLEVEL, (data.toInt() != 0) );
							else if( UTag == "HOLDABLE" )
								tile->SetFlag( TF_HOLDABLE, (data.toInt() != 0) );
							else if( UTag == "SIGNGUILDBANNER" )
								tile->SetFlag( TF_TRANSPARENT, (data.toInt() != 0) );
							else if( UTag == "WEBDIRTBLOOD" )
								tile->SetFlag( TF_TRANSLUCENT, (data.toInt() != 0) );
							else if( UTag == "WALLVERTTILE" )
								tile->SetFlag( TF_WALL, (data.toInt() != 0) );
							else if( UTag == "DAMAGING" )
								tile->SetFlag( TF_DAMAGING, (data.toInt() != 0) );
							else if( UTag == "BLOCKING" )
								tile->SetFlag( TF_BLOCKING, (data.toInt() != 0) );
							else if( UTag == "LIQUIDWET" )
								tile->SetFlag( TF_WET, (data.toInt() != 0) );

							// BaseTile Flag 2
							else if( UTag == "UNKNOWNFLAG1" )
								tile->SetFlag( TF_UNKNOWN1, (data.toInt() != 0) );
							else if( UTag == "STANDABLE" )
								tile->SetFlag( TF_SURFACE, (data.toInt() != 0) );
							else if( UTag == "CLIMBABLE" )
								tile->SetFlag( TF_CLIMBABLE, (data.toInt() != 0) );
							else if( UTag == "STACKABLE" )
								tile->SetFlag( TF_STACKABLE, (data.toInt() != 0) );
							else if( UTag == "WINDOWARCHDOOR" )
								tile->SetFlag( TF_WINDOW, (data.toInt() != 0) );
							else if( UTag == "CANNOTSHOOTTHRU" )
								tile->SetFlag( TF_NOSHOOT, (data.toInt() != 0) );
							else if( UTag == "DISPLAYASA" )
								tile->SetFlag( TF_DISPLAYA, (data.toInt() != 0) );
							else if( UTag == "DISPLAYASAN" )
								tile->SetFlag( TF_DISPLAYAN, (data.toInt() != 0) );

							// BaseTile Flag 3
							else if( UTag == "DESCRIPTIONTILE" )
								tile->SetFlag( TF_DESCRIPTION, (data.toInt() != 0) );
							else if( UTag == "FADEWITHTRANS" )
								tile->SetFlag( TF_FOLIAGE, (data.toInt() != 0) );
							else if( UTag == "PARTIALHUE" )
								tile->SetFlag( TF_PARTIALHUE, (data.toInt() != 0) );
							else if( UTag == "UNKNOWNFLAG2" )
								tile->SetFlag( TF_UNKNOWN2, (data.toInt() != 0) );
							else if( UTag == "MAP" )
								tile->SetFlag( TF_MAP, (data.toInt() != 0) );
							else if( UTag == "CONTAINER" )
								tile->SetFlag( TF_CONTAINER, (data.toInt() != 0) );
							else if( UTag == "EQUIPABLE" )
								tile->SetFlag( TF_WEARABLE, (data.toInt() != 0) );
							else if( UTag == "LIGHTSOURCE" )
								tile->SetFlag( TF_LIGHT, (data.toInt() != 0) );

							// BaseTile Flag 4
							else if( UTag == "ANIMATED" )
								tile->SetFlag( TF_ANIMATED, (data.toInt() != 0) );
							else if( UTag == "NODIAGONAL" )
								tile->SetFlag( TF_NODIAGONAL, (data.toInt() != 0) );
							else if( UTag == "UNKNOWNFLAG3" )
								tile->SetFlag( TF_UNKNOWN3, (data.toInt() != 0) );
							else if( UTag == "WHOLEBODYITEM" )
								tile->SetFlag( TF_ARMOR, (data.toInt() != 0) );
							else if( UTag == "WALLROOFWEAP" )
								tile->SetFlag( TF_ROOF, (data.toInt() != 0) );
							else if( UTag == "DOOR" )
								tile->SetFlag( TF_DOOR, (data.toInt() != 0) );
							else if( UTag == "CLIMBABLEBIT1" )
								tile->SetFlag( TF_STAIRBACK, (data.toInt() != 0) );
							else if( UTag == "CLIMBABLEBIT2" )
								tile->SetFlag( TF_STAIRRIGHT, (data.toInt() != 0) );
						}
					}
				}
				else
				{
					//7.0.8.2 data and earlier
					CTile *tile = &staticTile[entryNum];
					if( tile != NULL )
					{
						for( UString tag = toScan->First(); !toScan->AtEnd(); tag = toScan->Next() )
						{
							data	= toScan->GrabData();
							UTag	= tag.upper();

							// CTile properties
							if( UTag == "WEIGHT" )
								tile->Weight( data.toUByte() );
							else if( UTag == "HEIGHT" )
								tile->Height( data.toByte() );
							else if( UTag == "LAYER" )
								tile->Layer( data.toByte() );
							else if( UTag == "HUE" )
								tile->Hue( data.toUByte() );
							else if( UTag == "QUANTITY" )
								tile->Quantity( data.toUByte() );
							else if( UTag == "ANIMATION" )
								tile->Animation( data.toUShort() );
							else if( UTag == "NAME" )
								tile->Name( data.c_str() );

							// BaseTile Flag 1
							else if( UTag == "ATFLOORLEVEL" )
								tile->SetFlag( TF_FLOORLEVEL, (data.toInt() != 0) );
							else if( UTag == "HOLDABLE" )
								tile->SetFlag( TF_HOLDABLE, (data.toInt() != 0) );
							else if( UTag == "SIGNGUILDBANNER" )
								tile->SetFlag( TF_TRANSPARENT, (data.toInt() != 0) );
							else if( UTag == "WEBDIRTBLOOD" )
								tile->SetFlag( TF_TRANSLUCENT, (data.toInt() != 0) );
							else if( UTag == "WALLVERTTILE" )
								tile->SetFlag( TF_WALL, (data.toInt() != 0) );
							else if( UTag == "DAMAGING" )
								tile->SetFlag( TF_DAMAGING, (data.toInt() != 0) );
							else if( UTag == "BLOCKING" )
								tile->SetFlag( TF_BLOCKING, (data.toInt() != 0) );
							else if( UTag == "LIQUIDWET" )
								tile->SetFlag( TF_WET, (data.toInt() != 0) );

							// BaseTile Flag 2
							else if( UTag == "UNKNOWNFLAG1" )
								tile->SetFlag( TF_UNKNOWN1, (data.toInt() != 0) );
							else if( UTag == "STANDABLE" )
								tile->SetFlag( TF_SURFACE, (data.toInt() != 0) );
							else if( UTag == "CLIMBABLE" )
								tile->SetFlag( TF_CLIMBABLE, (data.toInt() != 0) );
							else if( UTag == "STACKABLE" )
								tile->SetFlag( TF_STACKABLE, (data.toInt() != 0) );
							else if( UTag == "WINDOWARCHDOOR" )
								tile->SetFlag( TF_WINDOW, (data.toInt() != 0) );
							else if( UTag == "CANNOTSHOOTTHRU" )
								tile->SetFlag( TF_NOSHOOT, (data.toInt() != 0) );
							else if( UTag == "DISPLAYASA" )
								tile->SetFlag( TF_DISPLAYA, (data.toInt() != 0) );
							else if( UTag == "DISPLAYASAN" )
								tile->SetFlag( TF_DISPLAYAN, (data.toInt() != 0) );

							// BaseTile Flag 3
							else if( UTag == "DESCRIPTIONTILE" )
								tile->SetFlag( TF_DESCRIPTION, (data.toInt() != 0) );
							else if( UTag == "FADEWITHTRANS" )
								tile->SetFlag( TF_FOLIAGE, (data.toInt() != 0) );
							else if( UTag == "PARTIALHUE" )
								tile->SetFlag( TF_PARTIALHUE, (data.toInt() != 0) );
							else if( UTag == "UNKNOWNFLAG2" )
								tile->SetFlag( TF_UNKNOWN2, (data.toInt() != 0) );
							else if( UTag == "MAP" )
								tile->SetFlag( TF_MAP, (data.toInt() != 0) );
							else if( UTag == "CONTAINER" )
								tile->SetFlag( TF_CONTAINER, (data.toInt() != 0) );
							else if( UTag == "EQUIPABLE" )
								tile->SetFlag( TF_WEARABLE, (data.toInt() != 0) );
							else if( UTag == "LIGHTSOURCE" )
								tile->SetFlag( TF_LIGHT, (data.toInt() != 0) );

							// BaseTile Flag 4
							else if( UTag == "ANIMATED" )
								tile->SetFlag( TF_ANIMATED, (data.toInt() != 0) );
							else if( UTag == "NODIAGONAL" )
								tile->SetFlag( TF_NODIAGONAL, (data.toInt() != 0) );
							else if( UTag == "UNKNOWNFLAG3" )
								tile->SetFlag( TF_UNKNOWN3, (data.toInt() != 0) );
							else if( UTag == "WHOLEBODYITEM" )
								tile->SetFlag( TF_ARMOR, (data.toInt() != 0) );
							else if( UTag == "WALLROOFWEAP" )
								tile->SetFlag( TF_ROOF, (data.toInt() != 0) );
							else if( UTag == "DOOR" )
								tile->SetFlag( TF_DOOR, (data.toInt() != 0) );
							else if( UTag == "CLIMBABLEBIT1" )
								tile->SetFlag( TF_STAIRBACK, (data.toInt() != 0) );
							else if( UTag == "CLIMBABLEBIT2" )
								tile->SetFlag( TF_STAIRRIGHT, (data.toInt() != 0) );
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
void CTile::Read( UOXFile *toRead )
{
	UI32 flagsRead;
	toRead->getULong( &flagsRead );
	flags = flagsRead;
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
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CTileHS::Read( UOXFile *toRead )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Reads and stores tiledata flags as part of loading tiledata into memory
//|					High Seas version
//|
//|	Notes		-	BYTE[4]		Flags		(See BaseTile)
//|					BYTE[4]		Unknown0
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
void CTileHS::Read( UOXFile *toRead )
{
	UI32 flagsRead;
	toRead->getULong( &flagsRead );
	flags = flagsRead;
	toRead->getULong( &unknown0 );
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
void CLand::Read( UOXFile *toRead )
{
	UI32 flagsRead;
	toRead->getULong( &flagsRead );
	flags = flagsRead;
	toRead->getUShort( &textureID );
	toRead->getChar( name, 20 );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void Read( UOXFile *toRead )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Reads and stores land tile data as part of loading tiledata into memory
//|					High Seas version
//|
//|	Notes		-	BYTE[4]		Flags
//|					BYTE[4]		Unknown
//|					BYTE[2]		TextureID
//|					BYTE[20]	Name
//o-----------------------------------------------------------------------------------------------o
void CLandHS::Read( UOXFile *toRead )
{
	UI32 flagsRead;
	toRead->getULong( &flagsRead );
	flags = flagsRead;
	toRead->getULong( &unknown1 );
	toRead->getUShort( &textureID );
	toRead->getChar( name, 20 );
}

}
