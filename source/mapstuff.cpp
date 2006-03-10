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

CMapStuff *Map					= NULL;

const UI16 LANDDATA_SIZE		= 0x4000; //(512 * 32);

//! these are the fixed record lengths as determined by the .mul files from OSI
//! i made them longs because they are used to calculate offsets into the files
const UI32 MultiRecordSize		= 12L;
const UI32 LandRecordSize		= 26L;
const UI32 TileRecordSize		= 37L;
const UI32 MapRecordSize		= 3L;
const UI32 MultiIndexRecordSize = 12L;
const UI32 StaticRecordSize		= 7L;

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

void CMapStuff::MultiItemsIndex::Include( SI16 x, SI16 y, SI08 z )
{
	// compute new bounding box, by include the new point
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
CMapStuff::CMapStuff() : landTile( 0 ), staticTile( 0 ), multiItems( 0 ), multiIndex( 0 ), multiIndexSize( 0 ), multiSize( 0 ), tileDataSize( 0 )
{
	LoadMapsDFN();
}

CMapStuff::~CMapStuff()
{
	if ( landTile )
		delete[] landTile;
	if ( staticTile )   
		delete[] staticTile;
	if ( multiItems )  
		delete[] multiItems;
	if ( multiIndex )
		delete[] multiIndex;
}

void CMapStuff::LoadMapsDFN( void )
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

//!--------------------------------------------------------------------------o
//|	Function/Class	-	void CMapStuff::Load( void )
//|	Date			-	03/12/2002
//|	Developer(s)	-	Unknown / EviLDeD 
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	Prepare access streamultidxms to the server required mul files. 
//|									This function basicaly just opens the streams and validates
//|									that the file is open and available
//o--------------------------------------------------------------------------o
//|	Returns				-	N/A
//o--------------------------------------------------------------------------o
UOXFile * loadFile( const std::string fullName )
{
	UOXFile *toLoad = new UOXFile( fullName.c_str(), "rb" );
	Console << "\t" << fullName << "\t\t";

	if( toLoad != NULL && toLoad->ready() )
		Console.PrintDone();
	else
		Console.PrintSpecial( CBLUE, "not found" );

	return toLoad;
}

void CMapStuff::LoadMapAndStatics( MapData_st& mMap, const std::string basePath, UI08 &totalMaps )
{
	char buffer[4];
	UString lName		= basePath + mMap.mapFile;
	mMap.mapObj			= new UOXFile( lName.c_str(), "rb" );
	Console << "\t" << lName << "\t\t";

	if( mMap.mapObj != NULL && mMap.mapObj->ready() )
	{
		mMap.fileSize = mMap.mapObj->getLength();

		SI32 checkSize = (mMap.fileSize / 196);
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
			size_t offset = 4;
			size_t blockID = 0;
			mdlFile.seekg( 0, std::ios::beg );
			while( !mdlFile.fail() )
			{
				mdlFile.read( buffer, 4 );
				if( !mdlFile.fail() )
				{
					blockID = calcserial( buffer[0], buffer[1], buffer[2], buffer[3] );
					mMap.mapDiffList[blockID] = offset;
				}
				offset += 196;
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
					sdlFile.read( buffer, 4 );
					blockID = calcserial( buffer[0], buffer[1], buffer[2], buffer[3] );
					sdiFile.read( buffer, 4 );
					mMap.staticsDiffIndex[blockID].offset = calcserial( buffer[0], buffer[1], buffer[2], buffer[3] );
					sdiFile.read( buffer, 4 );
					mMap.staticsDiffIndex[blockID].length = calcserial( buffer[0], buffer[1], buffer[2], buffer[3] );
					sdiFile.read( buffer, 4 );
					mMap.staticsDiffIndex[blockID].unknown = calcserial( buffer[0], buffer[1], buffer[2], buffer[3] );
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

void CMapStuff::LoadTileData( const std::string basePath )
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

	// first we have 512*32 pieces of land tile
	landTile		= new CLand[LANDDATA_SIZE];
	CLand *landPtr	= landTile;
	for( UI16 i = 0; i < 512; ++i )	
	{
		tileFile.seek(4, SEEK_CUR);			// skip the dummy header
		for( j = 0; j < 32; ++j )
			landPtr[j].Read( &tileFile );
		landPtr += 32;
	}

	// now get the 512*32 static tile pieces,
	tileDataSize	= (((tileFile.getLength() - (((32*26)+4)*512))/1188)*32);
	staticTile		= new CTile[tileDataSize];
	CTile *tilePtr	= staticTile;
	while( !tileFile.eof() )
	{
		tileFile.seek(4, SEEK_CUR);			// skip the dummy header
		for( j = 0; j < 32; ++j )
			tilePtr[j].Read( &tileFile );
		tilePtr += 32;
	}
	Console.PrintDone();
}

void CMapStuff::Load( void )
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
		Console.Error( 1, " Fatal Error: No maps found" );
		Console.Error( 1, " Check the settings for DATADIRECTORY in uox.ini" );
		Shutdown( FATAL_UOX3_MAP_NOT_FOUND );
	}

	LoadTileData( basePath );
	LoadMultis( basePath );
	LoadDFNOverrides();

	FileLookup->Dispose( maps_def );
	Console.PrintSectionBegin();
}

void CMapStuff::LoadMultis( const std::string basePath )
{
	// now main memory multiItems
	Console << "Caching Multis....  "; 

	// only turn it off for now because we are trying to fill the cache.
	UString lName	= basePath + "multi.idx";
	UOXFile multiIDX( lName.c_str(), "rb" );
	if( !multiIDX.ready() )
	{
		Console.PrintFailed();
		Console.Error( 1, "Can't cache %s!  File cannot be opened", lName.c_str() );
		return;
	}
	lName			= basePath + "multi.mul";
	UOXFile multis( lName.c_str(), "rb" );
	if( !multis.ready() )
	{
		Console.PrintFailed();
		Console.Error( 1, "Can't cache %s!  File cannot be opened", lName.c_str() );
		return;
	}

	//! first reads in Multi_st completely
	multiSize		= multis.getLength() / MultiRecordSize;
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
	multiIndex		= new MultiItemsIndex[multiIndexSize];
	// now rejig the multiIDX to point to the cache directly, and calculate the size
	for( MultiItemsIndex *ptr = multiIndex; ptr != (multiIndex+multiIndexSize); ++ptr )
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
	Console.PrintDone();
}

size_t CMapStuff::GetTileMem( void ) const
{
	return (LANDDATA_SIZE * sizeof( CLand ) + tileDataSize * sizeof( CTile ));
}

size_t CMapStuff::GetMultisMem( void ) const
{
	return (multiSize * sizeof( Multi_st ) + multiIndexSize * sizeof( MultiItemsIndex ) );
}

//o-------------------------------------------------------------o
//|   Function    :  SI08 TileHeight( UI16 tilenum )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o-------------------------------------------------------------o
//|   Purpose     :  Height of a gien tile (If climbable we return 1/2 its height).
//o-------------------------------------------------------------o
SI08 CMapStuff::TileHeight( UI16 tilenum )
{
	CTile& tile = SeekTile( tilenum );
	
	// For Stairs+Ladders
	if( tile.Climbable() ) 
		return static_cast<SI08>(tile.Height()/2);
	return tile.Height();
}


//o-------------------------------------------------------------o
//|   Function    :  SI08 StaticTop( SI16, SI16 y, SI08 oldz);
//|   Date        :  Unknown     Touched: Dec 21, 1998
//|   Programmer  :  Unknown
//o-------------------------------------------------------------o
//|   Purpose     :  Top of statics at/above given coordinates
//o-------------------------------------------------------------o
SI08 CMapStuff::StaticTop( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber, SI08 maxZ )
{
	SI08 top = ILLEGAL_Z;

	MapStaticIterator msi( x, y, worldNumber );
	for( Static_st *stat = msi.First(); stat != NULL; stat = msi.Next() )
	{
		SI08 tempTop = (SI08)(stat->zoff + TileHeight(stat->itemid));
		if( ( tempTop <= oldz + maxZ ) && ( tempTop > top ) )
			top = tempTop;
	}
	return top;
}

//o--------------------------------------------------------------------------
//|	Function		-	void SeekMultiSizes( UI16 multiNum, SI16& x1, SI16& x2, SI16& y1, SI16& y2 )
//|	Date			-	26th September, 2001
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Updates the box coordinates based on cached information
//o--------------------------------------------------------------------------
void CMapStuff::SeekMultiSizes( UI16 multiNum, SI16& x1, SI16& x2, SI16& y1, SI16& y2 )
{
	x1 = multiIndex[multiNum].lx;		// originally lx 
	x2 = multiIndex[multiNum].hx;		// originally ly presumablely bugs?
	y1 = multiIndex[multiNum].ly;		// originally ly 
	y2 = multiIndex[multiNum].hy;		// originally hy presumablely bugs?
}

SI32 CMapStuff::SeekMulti( UI16 multinum )
{
	SI32 retVal = -1;
	if( multinum < multiIndexSize )
		retVal = multiIndex[multinum].size;
	return retVal;
}

Multi_st& CMapStuff::SeekIntoMulti( UI16 multinum, SI32 number )
{
	return *(multiIndex[multinum].items + number);
}

//o--------------------------------------------------------------------------
//|	Function		-	void MultiArea( CMultiObj *i, SI16 &x1, SI16 &y1, SI16 &x2, SI16 &y2 )
//|	Date			-	6th September, 1999
//|	Programmer		-	Crackerjack
//|	Modified		-	Abaddon, 26th September, 2001 - fetches from cache
//o--------------------------------------------------------------------------
//|	Purpose			-	Finds the corners of a multi object
//o--------------------------------------------------------------------------
void CMapStuff::MultiArea( CMultiObj *i, SI16 &x1, SI16 &y1, SI16 &x2, SI16 &y2 )
{
	x1 = y1 = x2 = y2 = 0;
	if( !ValidateObject( i ) )
		return;
	const SI16 xAdd = i->GetX();
	const SI16 yAdd = i->GetY();
	SeekMultiSizes( static_cast<UI16>(i->GetID() - 0x4000), x1, x2, y1, y2 );

	x1 += xAdd;
	x2 += xAdd;
	y1 += yAdd;
	y2 += yAdd;
}


// return the height of a multi item at the given x,y. this seems to actually return a height
SI08 CMapStuff::MultiHeight( CItem *i, SI16 x, SI16 y, SI08 oldz, SI08 maxZ )
{
	UI16 multiID = static_cast<UI16>(i->GetID() - 0x4000);
	SI32 length = SeekMulti( multiID );

	if( length == -1 || length >= 17000000 ) //Too big... bug fix hopefully (Abaddon 13 Sept 1999)                                                                                                                          
		length = 0;

	for( SI32 j = 0; j < length; ++j )
	{
		Multi_st& multi = SeekIntoMulti( multiID, j );
		if( multi.visible && ( i->GetX() + multi.x == x) && ( i->GetY() + multi.y == y ) )
		{
			SI08 tmpTop = static_cast<SI08>(i->GetZ() + multi.z);
			if( ( tmpTop <= oldz + maxZ ) && ( tmpTop >= oldz - 1 ) )
				return tmpTop + 1;
			else if( ( tmpTop >= oldz - maxZ ) && ( tmpTop < oldz - 1 ) )
				return tmpTop + 1;
		}                                                                                                                 
	}
	return ILLEGAL_Z;                                                                                                                     
} 


// This was fixed to actually return the *elevation* of dynamic items at/above given coordinates
SI08 CMapStuff::DynamicElevation( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber, SI08 maxZ )
{
	SI08 z = ILLEGAL_Z;
	
	CMapRegion *MapArea = MapRegion->GetMapRegion( MapRegion->GetGridX( x ), MapRegion->GetGridY( y ), worldNumber );
	if( MapArea == NULL )	// no valid region
		return z;
	CDataList< CItem * > *regItems = MapArea->GetItemList();
	regItems->Push();
	for( CItem *tempItem = regItems->First(); !regItems->Finished(); tempItem = regItems->Next() )
	{
		if( !ValidateObject( tempItem ) )
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


UI16 CMapStuff::MultiTile( CItem *i, SI16 x, SI16 y, SI08 oldz )
{
	if( !i->CanBeObjType( OT_MULTI ) )
		return 0;
	UI16 multiID = static_cast<UI16>(i->GetID() - 0x4000);
	SI32 length = SeekMulti( multiID );
	if( length == -1 || length >= 17000000 )//Too big... bug fix hopefully (Abaddon 13 Sept 1999)
	{
		Console << "CMapStuff::MultiTile->Bad length in multi file. Avoiding stall." << myendl;
		const map_st map1 = Map->SeekMap( i->GetX(), i->GetY(), i->WorldNumber() );
		CLand& land = Map->SeekLand( map1.id );
		if( land.LiquidWet() ) // is it water?
			i->SetID( 0x4001 );
		else
			i->SetID( 0x4064 );
		length = 0;
	}

	for( SI32 j = 0; j < length; ++j )
	{
		Multi_st& multi = SeekIntoMulti( multiID, j );
		if( ( multi.visible && ( i->GetX() + multi.x == x) && (i->GetY() + multi.y == y)
			&& ( abs( i->GetZ() + multi.z - oldz ) <= 1 ) ) )
		{
			return multi.tile;
		}
		
	}
	return 0;
}


// returns which dynamic tile is present at (x,y) or -1 if no tile exists
UI16 CMapStuff::DynTile( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber )
{
	CMapRegion *MapArea = MapRegion->GetMapRegion( MapRegion->GetGridX( x ), MapRegion->GetGridY( y ), worldNumber );
	if( MapArea == NULL )	// no valid region
		return INVALIDID;

	CDataList< CItem * > *regItems = MapArea->GetItemList();
	regItems->Push();
	for( CItem *tempItem = regItems->First(); !regItems->Finished(); tempItem = regItems->Next() )
	{
		if( !ValidateObject( tempItem ) )
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


// return the elevation of MAP0.MUL at given coordinates, we'll assume since its land
// the height is inherently 0
SI08 CMapStuff::MapElevation( SI16 x, SI16 y, UI08 worldNumber )
{
	const map_st map = SeekMap( x, y, worldNumber );
	// make sure nothing can move into black areas
	if( 430 == map.id || 475 == map.id || 580 == map.id || 610 == map.id ||
		611 == map.id || 612 == map.id || 613 == map.id)
		return ILLEGAL_Z;
	return map.z;
}


// compute the 'average' map height by looking at three adjacent cells
SI08 CMapStuff::AverageMapElevation( SI16 x, SI16 y, UI16 &id, UI08 worldNumber )
{
	// first thing is to get the map where we are standing
	const map_st map1 = Map->SeekMap( x, y, worldNumber );
	id = map1.id;
	// if this appears to be a valid land id, <= 2 is invalid
	if( map1.id > 2 && ILLEGAL_Z != MapElevation( x, y, worldNumber ) )
	{
		// get three other nearby maps to decide on an average z?
		const SI08 map2z = MapElevation( x + 1, y, worldNumber );
		const SI08 map3z = MapElevation( x, y + 1, worldNumber );
		const SI08 map4z = MapElevation( x + 1, y + 1, worldNumber );
		
		SI08 testz = 0;
		if( abs( map1.z - map4z ) <= abs( map2z - map3z ) )
		{
			if( ILLEGAL_Z == map4z )
				testz = map1.z;
			else
			{
				testz = (SI08)((map1.z + map4z) >> 1);
				if( testz%2 < 0 ) 
					--testz;
				// ^^^ Fix to make it round DOWN, not just in the direction of zero
			}
		}
		else
		{
			if( ILLEGAL_Z == map2z || ILLEGAL_Z == map3z )
				testz = map1.z;
			else
			{
				testz = (SI08)((map2z + map3z) >> 1);
				if( testz%2<0 )
					--testz;
				// ^^^ Fix to make it round DOWN, not just in the direction of zero
			}
		}
		return testz;
	}
	return ILLEGAL_Z;
}

bool CMapStuff::IsValidTile( UI16 tileNum )
{
	bool retVal = true;
	if( tileNum == INVALIDID || tileNum >= tileDataSize )
		retVal = false;

	return retVal;
}

CTile& CMapStuff::SeekTile( UI16 tileNum )
{
	if( !IsValidTile( tileNum ) )
	{
		Console.Warning( 2, "Invalid tile access, the offending tile number is %u", tileNum );
		static CTile emptyTile;
		return emptyTile;
	}
	else
		return staticTile[tileNum];
}

CLand& CMapStuff::SeekLand( UI16 landNum )
{
	if( landNum == INVALIDID || landNum >= LANDDATA_SIZE )
	{
		Console.Warning( 2, "Invalid land access, the offending land number is %u", landNum );
		static CLand emptyTile;
		return emptyTile;
	}
	else
		return landTile[landNum];
}


bool CMapStuff::InsideValidWorld( SI16 x, SI16 y, UI08 worldNumber )
{
	if( worldNumber >= MapList.size() )
		return false;

	return ( ( x >= 0 && x < (MapList[worldNumber].xBlock/8) ) && ( y >= 0 && y < (MapList[worldNumber].yBlock/8) ) );
}


/*!
** Use this iterator class anywhere you would have used SeekInit() and SeekStatic()
** Unlike those functions however, it will only return the location of tiles that match your
** (x,y) EXACTLY.  They also should be significantly faster since the iterator saves
** a lot of info that was recomputed over and over and it returns a pointer instead 
** of an entire structure on the stack.  You can call First() if you need to reset it.
** This iterator hides all of the map implementation from other parts of the program.
** If you supply the exact variable, it tells it whether to iterate over those items
** with your exact (x,y) otherwise it will loop over all items in the same cell as you.
** (Thats normally only used for filling the cache)
** 
** Usage:
**		MapStaticIterator msi( x, y, worldNumber );
**
**      for( Static_st *stat = msi.First(); stat != NULL; stat = msi.Next() )
**      {
**          CTile& tile = Map->SeekTile( stat->itemid );
**  		    ... your code here...
**	  	}
*/
MapStaticIterator::MapStaticIterator( SI16 x, SI16 y, UI08 world, bool exact ) : baseX( static_cast<SI16>(x / 8) ), 
baseY( static_cast<SI16>(y / 8) ), pos( 0 ), remainX( static_cast<UI08>(x % 8 )), remainY( static_cast<UI08>(y % 8 )), 
index( 0 ), length( 0 ), exactCoords( exact ), worldNumber( world ), useDiffs( false )
{
	if( !Map->InsideValidWorld( static_cast<SI16>(baseX), static_cast<SI16>(baseY), world ) )
	{
		Console.Error( 3, "ASSERT: MapStaticIterator(); Not inside a valid world" );
		return;
	}

	MapData_st& mMap = Map->GetMapData( world );

	const SI16 TileHeight = (mMap.yBlock/8);
	const size_t indexPos = (( baseX * TileHeight * 12L ) + ( baseY * 12L ));

	std::map< UI32, StaticsIndex_st >::const_iterator diffIter = mMap.staticsDiffIndex.find( indexPos/12 );
	if( diffIter != mMap.staticsDiffIndex.end() )
	{
		const StaticsIndex_st &sdiRecord = diffIter->second;
		
		if( sdiRecord.offset == 0xFFFFFFFF )
		{
			length = 0;
			return;
		}

		useDiffs = true;
		pos		= sdiRecord.offset;
		length	= sdiRecord.length;
	}
	else
	{
		UOXFile *toRead = mMap.staidxObj;
		if( toRead == NULL )
		{
			length = 0;
			return;
		}

		toRead->seek( indexPos, SEEK_SET );
		if( !toRead->eof() )
		{
			toRead->getLong( &pos );
			if( pos != -1 )
			{
				toRead->getULong( &length );
				length /= StaticRecordSize;
				useDiffs = false;
			}
		}
	}
}

Static_st *MapStaticIterator::First( void )
{
	index = 0;
	return Next();
}

Static_st *MapStaticIterator::Next( void )
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
	
	// this was sizeof(OldStaRec) which SHOULD be 7, but on some systems which don't know how to pack, 
	const size_t pos2 = static_cast<size_t>(pos + (StaticRecordSize * index));	// skip over all the ones we've read so far
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

map_st CMapStuff::SeekMap( SI16 x, SI16 y, UI08 worldNumber )
{
	map_st map = {0, 0};
	if( worldNumber >= MapList.size() )
		return map;

	MapData_st& mMap = MapList[worldNumber];
	// index to the world
	const SI16 x1 = static_cast<SI16>(x / 8), y1 = static_cast<SI16>(y / 8);
	const size_t blockID = x1 * (mMap.yBlock/8) + y1;
	std::map< UI32, UI32 >::const_iterator diffIter = mMap.mapDiffList.find( blockID );
	if( diffIter != mMap.mapDiffList.end() )
	{
		mMap.mapDiffObj->seek( diffIter->second, SEEK_SET );
		mMap.mapDiffObj->getUShort( &map.id );
		mMap.mapDiffObj->getChar( &map.z );
	}
	else
	{
		const UI08 x2 = static_cast<UI08>(x % 8), y2 = static_cast<UI08>(y % 8);
		const size_t pos = ( x1 * (mMap.yBlock/8) * 196 ) + ( y1 * 196 ) + ( y2 * 24 ) + ( x2 * 3 ) + 4;
		mMap.mapObj->seek( pos, SEEK_SET );
		mMap.mapObj->getUShort( &map.id );
		mMap.mapObj->getChar( &map.z );
	}
	return map;
}

// Blocking statics at/above given coordinates?
bool CMapStuff::DoesStaticBlock( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber, bool checkWater )
{
	MapStaticIterator msi( x, y, worldNumber );
	for( Static_st *stat = msi.First(); stat != NULL; stat = msi.Next() )
	{
		const SI08 elev = static_cast<SI08>(stat->zoff + TileHeight( stat->itemid ));
		CTile& tile = SeekTile( stat->itemid );;
		if( elev >= oldz && stat->zoff <= oldz && ( tile.Blocking() || ( checkWater && tile.LiquidWet() ) ) )
			return true;
	}
	return false;
}

// Is there a static at the given coordinates, and if yes is it a surface
bool CMapStuff::IsStaticSurface( SI16 x, SI16 y, SI08 z, UI08 worldNumber )
{
	MapStaticIterator msi( x, y, worldNumber );
	for( Static_st *stat = msi.First(); stat != NULL; stat = msi.Next() )
	{
		const SI08 elev = static_cast<SI08>(stat->zoff + TileHeight( stat->itemid ));
		if( elev == z && !SeekTile( stat->itemid ).Standable() )
			return false;
	}
	return true;
}

// Is there a static at the given coordinates, and if yes is it wet
bool CMapStuff::IsStaticWet( SI16 x, SI16 y, SI08 z, UI08 worldNumber )
{
	MapStaticIterator msi( x, y, worldNumber );
	for( Static_st *stat = msi.First(); stat != NULL; stat = msi.Next() )
	{
		const SI08 elev = static_cast<SI08>(stat->zoff + TileHeight( stat->itemid ));
		if( elev == z && !SeekTile( stat->itemid ).LiquidWet() )
			return false;
	}
	return true;
}

// Return new height of player who walked to X/Y but from OLDZ
SI08 CMapStuff::Height( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber )
{
	// let's check in this order.. dynamic, static, then the map
	const SI08 dynz = DynamicElevation( x, y, oldz, worldNumber, MAX_Z_STEP );
	if( ILLEGAL_Z != dynz )
		return dynz;

	const SI08 staticz = StaticTop( x, y, oldz, worldNumber, MAX_Z_STEP );
	if( ILLEGAL_Z != staticz )
		return staticz;

	return MapElevation( x, y, worldNumber );
}

// Return whether give coordinates are inside a building by checking if there is a multi or static above them
bool CMapStuff::inBuilding( SI16 x, SI16 y, SI08 z, UI08 worldNumber )
{
		const SI08 dynz = Map->DynamicElevation( x, y, z, worldNumber, (SI08)127 );
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

// can the monster move here from an adjacent cell at elevation 'oldz'
// use illegal_z if they are teleporting from an unknown z
bool CMapStuff::CanMonsterMoveHere( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber, bool checkWater )
{
	if( worldNumber >= MapList.size() )
		return false;

	MapData_st& mMap = MapList[worldNumber];
	if( x < 0 || y < 0 || x >= mMap.xBlock || y >= mMap.yBlock  )
		return false;
    const SI08 elev = Height( x, y, oldz, worldNumber );
	if( ILLEGAL_Z == elev )
		return false;

	// is it too great of a difference z-value wise?
	if( oldz != ILLEGAL_Z )
	{
		// you can climb MaxZstep, but fall up to 15
		if( elev - oldz > MAX_Z_STEP )
			return false;
		else if( oldz - elev > 15 )
			return false;
	}

    // get the tile id of any dynamic tiles at this spot
    const UI16 dt = DynTile( x, y, elev, worldNumber );
	if( IsValidTile( dt ) )
	{
		CTile &tile = SeekTile( dt );
		if( tile.Blocking() || (checkWater && tile.LiquidWet()) /*|| !tile.Standable()*/ )
			return false;
	}

	// if there's a static block here in our way, return false
	if( DoesStaticBlock( x, y, elev, worldNumber, checkWater ) )
		return false;
	
	// if the static isn't a surface return false
	if( !IsStaticSurface( x, y, elev, worldNumber ) )
		return false;

	if( checkWater )
	{
		const map_st map = SeekMap( x, y, worldNumber );
		if( map.z == elev )
		{
			CLand& land = SeekLand( map.id );
			if( land.LiquidWet() )
				return false;
		}
	}
	return true;
}

// can the sea monster move here from an adjacent cell at elevation 'oldz'
// use illegal_z if they are teleporting from an unknown z
bool CMapStuff::CanSeaMonsterMoveHere( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber )
{
	if( worldNumber >= MapList.size() )
		return false;

	MapData_st& mMap = MapList[worldNumber];
	if( x < 0 || y < 0 || x >= mMap.xBlock || y >= mMap.yBlock  )
		return false;
    const SI08 elev = Height( x, y, oldz, worldNumber );
	if( ILLEGAL_Z == elev )
		return false;

	// is it too great of a difference z-value wise?
	if( oldz != ILLEGAL_Z )
	{
		// you can climb MaxZstep, but fall up to 15
		if( elev - oldz > MAX_Z_STEP )
			return false;
		else if( oldz - elev > 15 )
			return false;
	}

    // get the tile id of any dynamic tiles at this spot
    const UI16 dt = DynTile( x, y, elev, worldNumber );
	if( IsValidTile( dt ) && !SeekTile( dt ).LiquidWet() )
		return false;

    // if there's a static block here in our way, return false
    if( DoesStaticBlock( x, y, elev, worldNumber, false ) )
		return false;

	if( !IsStaticWet( x, y, elev, worldNumber ) )
		return false;

	const map_st map = SeekMap( x, y, worldNumber );
	if( map.z == elev )
	{
		CLand& land = SeekLand( map.id );
		if( !land.LiquidWet() )
			return false;
	}
    return true;
}

//o--------------------------------------------------------------------------
//|	Function		-	bool MapExists( UI08 worldNumber )
//|	Date			-	27th September, 2001
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns true if the server has that map in memory
//o--------------------------------------------------------------------------
bool CMapStuff::MapExists( UI08 worldNumber )
{
	return ( (worldNumber < MapList.size()) && (MapList[worldNumber].mapObj != NULL) );
}

MapData_st& CMapStuff::GetMapData( UI08 worldNumber )
{
	return MapList[worldNumber];
}

UI08 CMapStuff::MapCount( void ) const
{
	return static_cast<UI08>(MapList.size());
}

void CMapStuff::LoadDFNOverrides( void )
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
							tile->AtFloorLevel( (data.toInt() != 0) );
						else if( UTag == "HOLDABLE" )
							tile->Holdable( (data.toInt() != 0) );
						else if( UTag == "SIGNGUILDBANNER" )
							tile->SignGuildBanner( (data.toInt() != 0) );
						else if( UTag == "WEBDIRTBLOOD" )
							tile->WebDirtBlood( (data.toInt() != 0) );
						else if( UTag == "WALLVERTTILE" )
							tile->WallVertTile( (data.toInt() != 0) );
						else if( UTag == "DAMAGING" )
							tile->Damaging( (data.toInt() != 0) );
						else if( UTag == "BLOCKING" )
							tile->Blocking( (data.toInt() != 0) );
						else if( UTag == "LIQUIDWET" )
							tile->LiquidWet( (data.toInt() != 0) );

						// BaseTile Flag 2
						else if( UTag == "UNKNOWNFLAG1" )
							tile->UnknownFlag1( (data.toInt() != 0) );
						else if( UTag == "STANDABLE" )
							tile->Standable( (data.toInt() != 0) );
						else if( UTag == "CLIMBABLE" )
							tile->Climbable( (data.toInt() != 0) );
						else if( UTag == "STACKABLE" )
							tile->Stackable( (data.toInt() != 0) );
						else if( UTag == "WINDOWARCHDOOR" )
							tile->WindowArchDoor( (data.toInt() != 0) );
						else if( UTag == "CANNOTSHOOTTHRU" )
							tile->CannotShootThru( (data.toInt() != 0) );
						else if( UTag == "DISPLAYASA" )
							tile->DisplayAsA( (data.toInt() != 0) );
						else if( UTag == "DISPLAYASAN" )
							tile->DisplayAsAn( (data.toInt() != 0) );

						// BaseTile Flag 3
						else if( UTag == "DESCRIPTIONTILE" )
							tile->DescriptionTile( (data.toInt() != 0) );
						else if( UTag == "FADEWITHTRANS" )
							tile->FadeWithTrans( (data.toInt() != 0) );
						else if( UTag == "PARTIALHUE" )
							tile->PartialHue( (data.toInt() != 0) );
						else if( UTag == "UNKNOWNFLAG2" )
							tile->UnknownFlag2( (data.toInt() != 0) );
						else if( UTag == "MAP" )
							tile->Map( (data.toInt() != 0) );
						else if( UTag == "CONTAINER" )
							tile->Container( (data.toInt() != 0) );
						else if( UTag == "EQUIPABLE" )
							tile->Equipable( (data.toInt() != 0) );
						else if( UTag == "LIGHTSOURCE" )
							tile->LightSource( (data.toInt() != 0) );

						// BaseTile Flag 4
						else if( UTag == "ANIMATED" )
							tile->Animated( (data.toInt() != 0) );
						else if( UTag == "NODIAGONAL" )
							tile->NoDiagonal( (data.toInt() != 0) );
						else if( UTag == "UNKNOWNFLAG3" )
							tile->UnknownFlag3( (data.toInt() != 0) );
						else if( UTag == "WHOLEBODYITEM" )
							tile->WholeBodyItem( (data.toInt() != 0) );
						else if( UTag == "WALLROOFWEAP" )
							tile->WallRoofWeap( (data.toInt() != 0) );
						else if( UTag == "DOOR" )
							tile->Door( (data.toInt() != 0) );
						else if( UTag == "CLIMBABLEBIT1" )
							tile->ClimbableBit1( (data.toInt() != 0) );
						else if( UTag == "CLIMBABLEBIT2" )
							tile->ClimbableBit2( (data.toInt() != 0) );
					}
				}
			}
			else if( titlePart == "LAND" && entryNum )
			{	// let's not deal with this just yet
			}
		}
	}
}

void CTile::Read( UOXFile *toRead )
{
//	BYTE[4]		Flags		(See BaseTile)
//	BYTE		Weight		Weight of the item, 255 means not movable)
//	BYTE		Quality		If Wearable, this is a Layer. If Light Source, this is Light ID.
//  BYTE[2]		Unknown1
//	BYTE		Unknown2
//	BYTE		Quantity	If Weapon, Quantity is Weapon Class. If Armor, Quantity is Armor Class.
//  BYTE[2]		Animation
//	BYTE		Unknown3
//	BYTE		Hue
//	BYTE		Unknown4
//	BYTE		Unknown5
//	BYTE		Height		If Container, Height is "Contains" (Something determining how much the container can hold?)
//	BYTE[20]	Name
	toRead->getULong( &flags );
	toRead->getUChar( &weight );
	toRead->getChar(  &layer );
	toRead->getUShort( &unknown1 );
	toRead->getUChar( &unknown2 );
	toRead->getUChar( &quantity );
	toRead->getUShort(  &animation );
	toRead->getUChar(  &unknown3 );
	toRead->getUChar(  &hue );
	toRead->getUChar( &unknown4 );
	toRead->getUChar( &unknown5 );
	toRead->getChar(  &height );
	toRead->getChar(  name, 20 );

}

//	BYTE[4]		Flags
//	BYTE[2]		TextureID
//	BYTE[20]	Name
void CLand::Read( UOXFile *toRead )
{
	toRead->getULong( &flags );
	toRead->getUShort( &textureID );
	toRead->getChar( name, 20 );
}


}
