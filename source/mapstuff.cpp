#include "uox3.h"

#undef DBGFILE
#define DBGFILE "mapstuff.cpp"

const SI32 VERFILE_MAP			= 0x00;
const SI32 VERFILE_STAIDX		= 0x01;
const SI32 VERFILE_STATICS		= 0x02;
const SI32 VERFILE_ARTIDX		= 0x03;
const SI32 VERFILE_ART			= 0x04;
const SI32 VERFILE_ANIMIDX		= 0x05;
const SI32 VERFILE_ANIM			= 0x06;
const SI32 VERFILE_SOUNDIDX		= 0x07;
const SI32 VERFILE_SOUND		= 0x08;
const SI32 VERFILE_TEXIDX		= 0x09;
const SI32 VERFILE_TEXMAPS		= 0x0A;
const SI32 VERFILE_GUMPIDX		= 0x0B;
const SI32 VERFILE_GUMPART		= 0x0C;
const SI32 VERFILE_MULTIIDX		= 0x0D;
const SI32 VERFILE_MULTI		= 0x0E;
const SI32 VERFILE_SKILLSIDX	= 0x0F;
const SI32 VERFILE_SKILLS		= 0x10;
const SI32 VERFILE_TILEDATA		= 0x1E;
const SI32 VERFILE_ANIMDATA		= 0x1F;

const SI32 TILEDATA_TILES		= 0x68800;

/*
** Hey, its me fur again. I'm going to tear through this file next
** so I can get a better understanding of the map/tile stuff and
** especially the z values so we can nail down the walking bugs once
** and for all.  Gosh, I feel like a high-tech version of Steve the
** Crocodile Hunter these days..  C'mon mate, lets go squish some bugs!
*/

/*
** New Notes: 11/3/1999
** I changed all of the places where z was handled as a signed char and
** upped them to an int. While the basic terrain never goes above 127, 
** calculations sure can. For example, if you are at a piece of land at 100
** and put a 50 high static object, its going to roll over. - fur
*/

/*
** Tile Flags Bit Definitions - Please fill in if you know what something does!!
** I updated some of these with what LB knew 11/17/1999
**
** (tile.flag1&0x01)		1 = At Floor Level(?)
** (tile.flag1&0x02)		1 = Wearable/Holdable(?)
** (tile.flag1&0x04)		1 = Signs, Guilds, banners, (?)
** (tile.flag1&0x08)		1 = Web, dirt, blood, footsteps(?)
** (tile.flag1&0x10)		1 = Wall/Vertical Tile(?)
** (tile.flag1&0x20)		1 = Causes Damage(?)
** (tile.flag1&0x40)		1 = Blocking, cannot travel over
** (tile.flag1&0x80)		1 = IsWet, Maybe Water or Blood
**
** (tile.flag2&0x01)		Nothing Uses This Bit
** (tile.flag2&0x02)		Standable, Mostly Flat Surfaces(?)
** (tile.flag2&0x04)		Climbable, eg. Stairs/Ladders(?)
** (tile.flag2&0x08)		Pileable
** (tile.flag2&0x10)		Windows/Doors(?)
** (tile.flag2&0x20)		Cannot shoot thru
** (tile.flag2&0x40)		Display as 'a xxxx'
** (tile.flag2&0x80)		Display as 'an xxxx;
**
** (tile.flag3&0x01)		Internal Only Tile(?)
** (tile.flag3&0x02)		Fades with CoT(?)
** (tile.flag3&0x04)		(?)
** (tile.flag3&0x08)		Never used(?)
** (tile.flag3&0x10)		Map(?)
** (tile.flag3&0x20)		Container(?)
** (tile.flag3&0x40)		Wearable(?)
** (tile.flag3&0x80)		Lightsource(?)
**
**
** (tile.flag4&0x01)		Animated Tile(?)
** (tile.flag4&0x02)		(?)
** (tile.flag4&0x04)		Walk(?)
** (tile.flag4&0x08)		Whole bodyitem/Items you cannot remove from paper doll(?)
** (tile.flag4&0x10)		Wall/Roof? Weapon(?)
** (tile.flag4&0x20)		Door(?)
** (tile.flag4&0x40)		Climbable direction (2-bits)
** (tile.flag4&0x80)		Climbable direction(?)
**
** Other things, one bit somewhere is probably whether items decay or not, whether
** something is magical or not, one bit for editible or not, bit for dyeable, can be poisioned,
** wearable and/or wear it can be worn, regeants bit? required str? damage for weapons?
** readable? nameable?
*/

/*
** 11/17/1999 Rewrite plans
**
** In reality most of the 'caching' code found within is really 'shadowing' code. That is,
** its all just slurped up into memory once at startup (very slow)  I plan on rewriting all
** of this so that its actually cached, that is if a piece is already in memory it will be
** used, otherwise gotten from disk.  There's no need to load them all at start up, in fact
** this is wasteful, because you wait at startup loading a lot of pieces you will never use.
** Also the map cache is retarded because it is alway shifting the entire buffer on a miss,
** rather than using a rolling queue. Finally, the verdata.mul stuff is not cached at all
** and has to be a huge performance hit constantly sifting through the index for that file.
** -fur
*/

/*
** 12/08/1999 New Info
**
** OK, Zippy had taken a shot at the circular cache for the map0. He was close, but I had to
** fix a few bugs.  Also, I implemented the caching of the verdata.mul. Its important to note
** that the verdata.mul has lots of patches for all sorts of .mul files, I only kept the ones
** we were actively using.
**
** New rewrite plans, its way confusing having every function in here using 'Height' to mean
** elevation or actual height of a tile.  Here's the new renaming plan:
** Elevation - the z value of the bottom edge of an item
** Height - the height of the item, it is independant of the elevation, like a person is 6ft tall
** Top - the z value of the top edge of an item, is always Elevation + Height of item
** -fur
*/

/*
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

#ifdef DEBUG_MAP_STUFF
void bitprint(FILE *fp, UI08 x )
{
	for( int i = 7; i >= 0; --i )
	{
		if( ( x & 0x80 ) == 0 )
			fprintf( fp, "0" );
		else
			fprintf( fp, "1" );
		if( 4 == i )
			fprintf( fp, " " );
		x = x << 1;
	}
}
#endif


cMapStuff::cMapStuff() : versionCache( NULL ), versionRecordCount( 0 ), versionMemory( 0 ), StaMem( 0 ), TileMem( 0 ),
Cache( false ), Map0CacheHit( 0 ), Map0CacheMiss( 0 ), StaticBlocks( 0 ), verfile( NULL ), tilefile( NULL ), multifile( NULL ), midxfile( NULL )
{
	// after a mess of bugs with the structures not matching the physical record sizes
	// i've gotten paranoid...
	assert( sizeof( versionrecord ) >= VersionRecordSize );
	assert( sizeof( st_multi ) >= MultiRecordSize );
	assert( sizeof( map_st ) >= MapRecordSize );
	assert( sizeof( st_multiidx ) >= MultiIndexRecordSize );
	// staticrecord is not listed here because we explicitly don't read in some
	// unknown bytes to save memory
	
	for( UI32 i = 0; i < NumberOfWorlds; i++ )
	{
		mapArrays[i] = NULL;
		statArrays[i] = NULL;
		sidxArrays[i] = NULL;
	}
	mapname[0] = sidxname[0] = statname[0] = vername[0] = tilename[0] = multiname[0] = midxname[0] = '\0';
	
	memset( tilecache, 0x00, sizeof( tilecache ) );
}

cMapStuff::~cMapStuff()
{
	if( versionCache ) 
		delete [] versionCache;
	for( UI32 i = 0; i < NumberOfWorlds; i++ )
	{
		if( mapArrays[i] != NULL )
		{
			delete mapArrays[i];
			mapArrays[i] = NULL;
		}
		if( statArrays[i] != NULL )
		{
			delete statArrays[i];
			statArrays[i] = NULL;
		}
		if( sidxArrays[i] != NULL )
		{
			delete sidxArrays[i];
			sidxArrays[i] = NULL;
		}
	}
	
	if( verfile )   
		delete verfile;
	if( tilefile )  
		delete tilefile;
	if( multifile ) 
		delete multifile;
	if( midxfile )  
		delete midxfile;
	for( UI32 j = 0; j < multiCache.size(); j++ )
	{
		delete[] multiCache[j]->cache;
		delete multiCache[j];
	}
	multiCache.resize( 0 );
}


//o--------------------------------------------------------------------------o
//|	Function/Class-	void cMapStuff::Load( void )
//|	Date					-	03/12/2002
//|	Developer(s)	-	Unknown / EviLDeD 
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	Prepare access streams to the server required mul files. 
//|									This function basicaly just opens the streams and validates
//|									that the file is open and available
//o--------------------------------------------------------------------------o
//|	Returns				-	N/A
//o--------------------------------------------------------------------------o	
void cMapStuff::Load( void )
{
	Console.PrintSectionBegin();
	Console << "Preparing to open *.mul files..." << myendl << "(If they don't open, fix your paths in the uox3.ini)" << myendl;

	std::string temp = "";
	char lookupName[MAX_PATH];
	UI32 i;
	for( i = 0; i < NumberOfWorlds; i++ )
	{
		// Don't even bother to load unknown maps
		if( uomapTypes[i] != UOMT_UNKNOWN )
		{
			sprintf( lookupName, "map%i.mul", i );
			temp = BuildFilePath( lookupName );
			mapArrays[i] = new UOXFile( temp.c_str(), "rb" );
			Console << "\t" << temp.c_str() << "\t\t";
			if( mapArrays[i] != NULL && mapArrays[i]->ready() )
			{
				uomapTypes[i] = CalcFromFileLength( mapArrays[i] );
				if (uomapTypes[i] == UOMT_UNKNOWN)
				        Console.PrintFailed();
				else
				        Console.PrintDone();
			}
			else
			{
				Console.PrintPassed();
				// If the map file doesn't exist, there's no reason to keep looking for the other files
				uomapTypes[i] = UOMT_UNKNOWN;
				continue;
			}

			sprintf( lookupName, "staidx%i.mul", i );
			temp = BuildFilePath( lookupName );
			sidxArrays[i] = new UOXFile( temp.c_str(), "rb" );
			Console << "\t" << temp.c_str() << "\t";
			if( sidxArrays[i] != NULL && sidxArrays[i]->ready() )
			{
				Console.PrintDone();
			}
			else
			{
				uomapTypes[i] = UOMT_UNKNOWN;
				Console.PrintFailed();				
			}
			
			sprintf( lookupName, "statics%i.mul", i );
			temp = BuildFilePath( lookupName );
			statArrays[i] = new UOXFile( temp.c_str(), "rb" );
			Console << "\t" << temp.c_str() << "\t";
			if( statArrays[i] != NULL && statArrays[i]->ready() )
			{
				Console.PrintDone();
			}
			else
			{
				Console.PrintFailed();
				uomapTypes[i] = UOMT_UNKNOWN;
			}
		}
	}
	for( i = 0; i < NumberOfWorlds; i++ )
	{
		if( uomapTypes[i] != UOMT_UNKNOWN )
			break;
	}
	if( i == NumberOfWorlds )
	{
		// Hmm, no maps were valid, so not much sense in continuing
		Console.Error( 1, " Fatal Error: No maps found" );
		Console.Error( 1, " Check the settings for DATADIRECTORY in uox.ini" );
		Shutdown( FATAL_UOX3_MAP_NOT_FOUND );
	}

	temp = BuildFilePath( "verdata.mul" );
	Console << "\t" << temp.c_str() << "\t";
	verfile = new UOXFile( temp.c_str(), "rb" );
	if( verfile == NULL || !verfile->ready() )
    {
		Console.PrintPassed();
	}
    else
	{
		Console.PrintDone();
    }
	

	temp = BuildFilePath( "tiledata.mul" );
	Console << "\t" << temp.c_str() << "\t";
	tilefile = new UOXFile( temp.c_str(), "rb" );
	if( tilefile == NULL || !tilefile->ready() )
    {
		Console.PrintFailed();
		Shutdown( FATAL_UOX3_TILEDATA_NOT_FOUND );
    }
	Console.PrintDone();

	temp = BuildFilePath( "multi.mul" );
	Console << "\t" << temp.c_str() << "\t\t";
	multifile = new UOXFile( temp.c_str(), "rb" );
	if( multifile == NULL || !multifile->ready() )
    {
		Console.PrintFailed();
		Shutdown( FATAL_UOX3_MULTI_DATA_NOT_FOUND );
    }
	Console.PrintDone();

	temp = BuildFilePath( "multi.idx" );
	Console << "\t" << temp.c_str() << "\t\t";
	midxfile = new UOXFile( temp.c_str(), "rb" );
	if( midxfile == NULL || !midxfile->ready() )
    {
		Console.PrintFailed();
		Shutdown( FATAL_UOX3_MULTI_INDEX_NOT_FOUND );
    }
	Console.PrintDone();

	Console << "MUL files successfully opened (but not yet loaded.)" << myendl;
	Console.PrintSectionBegin();
	
	CacheVersion();
	CacheMultis();
	if( Cache )
    {
		CacheTiles(); // has to be exactly here, or loadnewlorld cant access correct tiles
		CacheStatics();
    }

	Console.PrintSectionBegin();
}

// this stuff is rather buggy thats why I separted it from uox3.cpp
// feel free to correct it, but be carefull
// bugfixing this stuff often has a domino-effect with walking etc.
// LB 24/7/99

// oh yah, well that's encouraging.. NOT! at least LB was kind enough to
// move this out into a separate file. he gets kudos for that!
//int cMapStuff::TileHeight(int tilenum)
SI08 cMapStuff::TileHeight( UI16 tilenum )
{
	CTile tile;
	SeekTile(tilenum, &tile);
	
	// For Stairs+Ladders
	if( tile.Climbable() ) 
		return (SI08)(tile.Height()/2);	// hey, lets just RETURN half!
	return tile.Height();
}

//o-------------------------------------------------------------o
//|   Function    :  SI08 StaticTop( SI16, SI16 y, SI08 oldz);
//|   Date        :  Unknown     Touched: Dec 21, 1998
//|   Programmer  :  Unknown
//o-------------------------------------------------------------o
//|   Purpose     :  Top of statics at/above given coordinates
//o-------------------------------------------------------------o
SI08 cMapStuff::StaticTop( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber )
{
	SI08 top = illegal_z;

	MapStaticIterator msi( x, y, worldNumber );
	staticrecord *stat = NULL;
	while( stat = msi.Next() )
	{
		SI08 tempTop = (SI08)(stat->zoff + TileHeight(stat->itemid));
		if( ( tempTop <= oldz + MaxZstep ) && ( tempTop > top ) )
			top = tempTop;
	}
	return top;
}	


//
// i guess this function returns where the tile is a 'blocker' meaning you can't pass through it
bool cMapStuff::DoesTileBlock(int tilenum)
{
	CTile tile;
	
	SeekTile(tilenum, &tile);
	
	return tile.Blocking();
}

//o--------------------------------------------------------------------------
//|	Function		-	void SeekMultiSizes( UI16 multiNum, SI16& x1, SI16& x2, SI16& y1, SI16& y2 )
//|	Date			-	26th September, 2001
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Updates the box coordinates based on cached information
//o--------------------------------------------------------------------------
void cMapStuff::SeekMultiSizes( UI16 multiNum, SI16& x1, SI16& x2, SI16& y1, SI16& y2 )
{
	x1 = multiCache[multiNum]->lx;
	x2 = multiCache[multiNum]->ly;
	y1 = multiCache[multiNum]->hx;
	y2 = multiCache[multiNum]->hy;
}

//o--------------------------------------------------------------------------
//|	Function		-	void MultiArea( CMultiObj *i, SI16 &x1, SI16 &y1, SI16 &x2, SI16 &y2 )
//|	Date			-	6th September, 1999
//|	Programmer		-	Crackerjack
//|	Modified		-	Abaddon, 26th September, 2001 - fetches from cache
//o--------------------------------------------------------------------------
//|	Purpose			-	Finds the corners of a multi object
//o--------------------------------------------------------------------------
void cMapStuff::MultiArea( CMultiObj *i, SI16 &x1, SI16 &y1, SI16 &x2, SI16 &y2 )
{
	x1 = y1 = x2 = y2 = 0;
	if( i == NULL )
		return;
	const SI16 xAdd = i->GetX();
	const SI16 yAdd = i->GetY();
	SeekMultiSizes( (UI16)(i->GetID() - 0x4000), x1, x2, y1, y2 );

	x1 += xAdd;
	x2 += xAdd;
	y1 += yAdd;
	y2 += yAdd;
}

// return the height of a multi item at the given x,y. this seems to actually return a height
SI08 cMapStuff::MultiHeight( CItem *i, SI16 x, SI16 y, SI08 oldz )
{                                                                                                                                  	
	SI32 length = 0;
	UI16 multiID = ( i->GetID() ) - 0x4000;
	SeekMulti( multiID, &length );
	st_multi *multi = NULL;

	if( length == -1 || length >= 17000000 ) //Too big... bug fix hopefully (Abaddon 13 Sept 1999)                                                                                                                          
		length = 0;
	
	for( int j = 0; j < length; j++ )
	{
		multi = SeekIntoMulti( multiID, j );
		if( multi->visible && ( i->GetX() + multi->x == x) && ( i->GetY() + multi->y == y ) )
		{
			int tmpTop = i->GetZ() + multi->z;
			if( ( tmpTop <= oldz + MaxZstep ) && ( tmpTop >= oldz - 1 ) )
			{
				return multi->z;
			}
			else if( ( tmpTop >= oldz - MaxZstep ) && ( tmpTop < oldz - 1 ) )
			{
				return multi->z;
			}
		}                                                                                                                 
	}

	return 0;                                                                                                                     
} 

// This was fixed to actually return the *elevation* of dynamic items at/above given coordinates
SI08 cMapStuff::DynamicElevation( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber )
{
	SI08 z = illegal_z;
	
	SubRegion *MapArea = MapRegion->GetCell( x, y, worldNumber );
	if( MapArea == NULL )	// no valid region
		return z;
	MapArea->PushItem();
	for( CItem *tempItem = MapArea->FirstItem(); !MapArea->FinishedItems(); tempItem = MapArea->GetNextItem() )
	{
		if( tempItem == NULL )
			continue;
		if( tempItem->GetID( 1 ) >= 0x40 )
		{
			z = MultiHeight( tempItem, x, y, oldz );
			z += tempItem->GetZ() + 1;
		}
		if( tempItem->GetX() == x && tempItem->GetY() == y && tempItem->GetID( 1 ) < 0x40 )
		{
			SI08 ztemp = (SI08)(tempItem->GetZ() + TileHeight( tempItem->GetID() ));
			if( ( ztemp <= oldz + MaxZstep ) && ztemp > z )
				z = ztemp;
		}
	}
	MapArea->PopItem();
	return z;
}

int cMapStuff::MultiTile( CItem *i, SI16 x, SI16 y, SI08 oldz)
{
	SI32 length = 0;
	st_multi *multi = NULL;
	UI16 multiID = (UI16)(i->GetID() - 0x4000);
	SeekMulti( multiID, &length );
	if( length == -1 || length >= 17000000 )//Too big... bug fix hopefully (Abaddon 13 Sept 1999)
	{
		Console << "cMapStuff::MultiTile->Bad length in multi file. Avoiding stall." << myendl;
		length = 0;
	}
	for( int j = 0; j < length; j++ )
	{
		multi = SeekIntoMulti( multiID, j );
		if( ( multi->visible && ( i->GetX() + multi->x == x) && (i->GetY() + multi->y == y)
			&& ( abs( i->GetZ() + multi->z - oldz ) <= 1 ) ) )
		{
			return multi->tile;
		}
		
	}
	return 0;
}

// returns which dynamic tile is present at (x,y) or -1 if no tile exists
int cMapStuff::DynTile( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber )
{
	SubRegion *MapArea = MapRegion->GetCell( x, y, worldNumber );
	if( MapArea == NULL )	// no valid region
		return -1;
	MapArea->PushItem();
	for( CItem *tempItem = MapArea->FirstItem(); !MapArea->FinishedItems(); tempItem = MapArea->GetNextItem() )
	{
		if( tempItem == NULL )
			continue;
		if( tempItem->GetID( 1 ) >= 0x40 )
		{
			MapArea->PopItem();
			return MultiTile( tempItem, x, y, oldz );
		}
		else if( tempItem->GetX() == x && tempItem->GetY() == y )
		{
			MapArea->PopItem();
			return tempItem->GetID();
		}
	}
	MapArea->PopItem();
	return -1;
}

// return the elevation of MAP0.MUL at given coordinates, we'll assume since its land
// the height is inherently 0
SI08 cMapStuff::MapElevation( SI16 x, SI16 y, UI08 worldNumber )
{
	map_st map = SeekMap0( x, y, worldNumber );
	// make sure nothing can move into black areas
	if( 430 == map.id || 475 == map.id || 580 == map.id || 610 == map.id ||
		611 == map.id || 612 == map.id || 613 == map.id)
		return illegal_z;
	return map.z;
}

// compute the 'average' map height by looking at three adjacent cells
SI08 cMapStuff::AverageMapElevation( SI16 x, SI16 y, UI16 &id, UI08 worldNumber )
{
	// first thing is to get the map where we are standing
	map_st map1 = Map->SeekMap0( x, y, worldNumber );
	id = map1.id;
	// if this appears to be a valid land id, <= 2 is invalid
	if( map1.id > 2 && illegal_z != MapElevation( x, y, worldNumber ) )
	{
		// get three other nearby maps to decide on an average z?
		SI08 map2z = MapElevation( x + 1, y, worldNumber );
		SI08 map3z = MapElevation( x, y + 1, worldNumber );
		SI08 map4z = MapElevation( x + 1, y + 1, worldNumber );
		
		SI08 testz = 0;
		if( abs( map1.z - map4z ) <= abs( map2z - map3z ) )
		{
			if( illegal_z == map4z )
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
			if( illegal_z == map2z || illegal_z == map3z )
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
	return illegal_z;
}

// since the version data will potentiall affect every map related operation
// we are always going to cache it.   we are going to allocate maxRecordCount as
// given by the file, but actually we aren't going to use all of them, since we
// only care about the patches made to the 6 files the server needs.  so the
// versionRecordCount hold how many we actually saved
void cMapStuff::CacheVersion( void )
{
	if( verfile == NULL || !verfile->ready() )
	{
		return;
	}

	verfile->seek( 0, SEEK_SET );
	UI32 maxRecordCount = 0;
	verfile->getULong( &maxRecordCount );
	
	if( 0 == maxRecordCount )
		return;
	if( NULL == ( versionCache = new versionrecord[maxRecordCount] ) )
		return;
	
	Console << "Caching version data...";
	versionMemory = maxRecordCount * sizeof( versionrecord );
	for( UI32 i = 0; i < maxRecordCount; ++i )
    {
		if( verfile->eof() )
		{
			Console.Error( 1, " Avoiding bad read crash with verdata.mul.\n" );
			return;
		}
		versionrecord *ver = versionCache + versionRecordCount;
		assert( ver );
		verfile->get_versionrecord( ver );
		
		// see if its a record we care about
		switch( ver->file )
		{
		case VERFILE_MULTIIDX:
		case VERFILE_MULTI:
			Console << "Hit on Multi file!" << myendl;
		case VERFILE_TILEDATA:
			++versionRecordCount;
			break;
		case VERFILE_MAP:
		case VERFILE_STAIDX:
		case VERFILE_STATICS:
			// at some point we may need to handle these cases, but OSI hasn't patched them as of
			// yet, so no need slowing things down processing them
//			Console.Error( "Eeek! OSI has patched the static data and I don't know what to do!" );
			break;
		default:
			// otherwise its for a file we don't care about
			break;
		}
    }
	Console << "Done (Cached " << versionRecordCount << "/" << maxRecordCount << " patches)" << myendl;
}

SI32 cMapStuff::VerSeek( SI32 file, SI32 block )
{
	for( UI32 i = 0; i < versionRecordCount; ++i )
	{
		if( versionCache[i].file == file && versionCache[i].block == block )
		{
			verfile->seek( versionCache[i].filepos, SEEK_SET );
			return versionCache[i].length;
		}
	}
	return 0;
}

bool cMapStuff::VerTile(int tilenum, CTile *tile)
{
	if( tilenum == -1 )
		return false;
	
	const SI32 block=(tilenum/32);
	if( VerSeek( VERFILE_TILEDATA, block + 0x200 ) == 0 )
		return false;
	else
	{
		const SI32 pos=4+(TileRecordSize*(tilenum%32)); // correct
		verfile->seek(pos, SEEK_CUR);
		verfile->get_tile_st(tile);
		return true;
	}
}

void cMapStuff::SeekTile(int tilenum, CTile *tile)
{
//	assert(tilenum >= 0);
	assert(tile);
	if( tilenum < 0 )
	{
		tile->Flag1( 0 );
		tile->Flag2( 0 );
		tile->Flag3( 0 );
		tile->Flag4( 0 );
		tile->Weight( 255 );
		tile->Height( 0 );
		tile->Name( "bad tile" );
		return;
	}
	if( tilenum >= 0x4000 )
	{
		tile->Name( "multi" );
		tile->Flag1( 0 );
		tile->Flag2( 0 );
		tile->Flag3( 0 );
		tile->Flag4( 0 );
		tile->Weight( 255 );
		tile->Height( 0 );
		return;
	}
	
	if( Cache )
	{
		// fill it up straight from the cache
		memcpy(tile, tilecache + tilenum, sizeof( CTile ));
#ifdef DEBUG_MAP_STUFF
		Console << "SeekTile - cache hit!" << myendl;
#endif
	}
	else
	{
		if( VerTile( tilenum, tile ) )
		{
#ifdef DEBUG_MAP_STUFF
			Console << "Loaded tile " << tilenum << " from verdata.mul" << myendl;
#endif
		}
		else
		{
			// TILEDATA_TILE is the amount to skip past all of the land_st's
			// plus skip 4 bytes per block for the long separating them
			const SI32 block = ( tilenum / 32 );
			const SI32 pos = TILEDATA_TILES + ( ( block + 1 ) * 4 ) + ( TileRecordSize * tilenum ); // correct
			tilefile->seek( pos, SEEK_SET );
			tilefile->get_tile_st( tile );
		}
		
#ifdef DEBUG_MAP_STUFF
		printf("Tile #%d is '%s' ", tilenum, tile->name);
		printf("flag1: "); bitprint(stdout, tile->flag1);
		printf("flag2: "); bitprint(stdout, tile->flag2);
		printf("flag3: "); bitprint(stdout, tile->flag3);
		printf("flag4: "); bitprint(stdout, tile->flag4);
		printf("\n");
#endif
	}
}

void cMapStuff::CacheTiles( void )
{
	// temp disable caching so we can fill the cache
	Cache = false;
	
	Console << "Caching tiledata...  ";
	Console.TurnYellow();
	Console << " 0%";

	TileMem = 0x4000 * sizeof( CTile );
	memset(tilecache, 0, TileMem);
	
	const int tenPercent = 0x4000 / 10;
	for( UI32 i = 0; i < 0x4000; ++i )
    {
		SeekTile(i, tilecache + i);
		
		if( i % tenPercent == 0 )
		{
			if( i/tenPercent < 2 )
				Console << "\b\b" << (SI16)( i / tenPercent * 10 ) << "%";
			else 
				Console << "\b\b\b" << (SI16)( i / tenPercent * 10 ) << "%";
		}
			//Console.Print( "\b\b%d0%%\n", 1 + (i / tenPercent));
    }
	Console << "\b\b\b\b";
	Console.PrintDone();
	Cache = true;
	
#ifdef DEBUG_TILE_BITS
	for( int bit = 0x01; bit <= 0x0080; bit = bit << 1 )
	{
		char buf[30];
		sprintf( buf, "static1-%d.txt", bit );
		FILE *fp = fopen( buf, "w" );
		for( int i = 0; i < 0x4000; ++i )
		{
			CTile *tile = tilecache + i;
			if( ( tile->flag1 & bit ) == bit)
			{
				fprintf( fp, "%04x tile '%-20.20s'\t", i, tile->name );
				fprintf( fp, "flag1: "); bitprint( fp, tile->flag1 );
				fprintf( fp, " flag2: "); bitprint( fp, tile->flag2 );
				fprintf( fp, " flag3: "); bitprint( fp, tile->flag3 );
				fprintf( fp, " flag4: "); bitprint( fp, tile->flag4 );
				fprintf( fp, "\n");
			}
		}
		fclose(fp);
		sprintf(buf, "static2-%d.txt", bit);
		fp = fopen(buf, "w");
		for( int i = 0; i < 0x4000; ++i )
		{
			CTile *tile = tilecache + i;
			if( ( tile->flag2 & bit ) == bit )
			{
				fprintf( fp, "%04x tile '%-20.20s'\t", i, tile->name );
				fprintf( fp, "flag1: "); bitprint( fp, tile->flag1 );
				fprintf( fp, " flag2: "); bitprint( fp, tile->flag2 );
				fprintf( fp, " flag3: "); bitprint( fp, tile->flag3 );
				fprintf( fp, " flag4: "); bitprint( fp, tile->flag4 );
				fprintf( fp, "\n");
			}
		}
		fclose( fp );
		sprintf( buf, "static3-%d.txt", bit );
		fp = fopen( buf, "w" );
		for( int i = 0; i < 0x4000; ++i )
		{
			CTile *tile = tilecache + i;
			if( ( tile->flag3 & bit ) == bit )
			{
				fprintf( fp, "%04x tile '%-20.20s'\t", i, tile->name );
				fprintf( fp, "flag1: "); bitprint( fp, tile->flag1 );
				fprintf( fp, " flag2: "); bitprint( fp, tile->flag2 );
				fprintf( fp, " flag3: "); bitprint( fp, tile->flag3 );
				fprintf( fp, " flag4: "); bitprint( fp, tile->flag4 );
				fprintf( fp, "\n");
			}
		}
		fclose(fp);
		sprintf(buf, "static4-%d.txt", bit);
		fp = fopen(buf, "w");
		for( int i = 0; i < 0x4000; ++i )
		{
			CTile *tile = tilecache + i;
			if( ( tile->flag4 & bit ) == bit )
			{
				fprintf( fp, "%04x tile '%-20.20s'\t", i, tile->name );
				fprintf( fp, "flag1: "); bitprint( fp, tile->flag1 );
				fprintf( fp, " flag2: "); bitprint( fp, tile->flag2 );
				fprintf( fp, " flag3: "); bitprint( fp, tile->flag3 );
				fprintf( fp, " flag4: "); bitprint( fp, tile->flag4 );
				fprintf( fp, "\n" );
			}
		}
		fclose( fp );
	}
#endif
}

bool cMapStuff::VerLand(int landnum, CLand *land)
{
	const SI32 block=(landnum/32);
	if( VerSeek( VERFILE_TILEDATA, block ) == 0 )
		return false;
	const SI32 pos = 4 + ( LandRecordSize * ( landnum % 32 ) ); // correct
	verfile->seek( pos, SEEK_CUR );
	verfile->get_land_st( land );
	return true;
}

void cMapStuff::SeekLand( int landnum, CLand *land)
{
	const SI32 block = ( landnum / 32 );
	if( !VerLand( landnum, land ) )
	{
		const SI32 pos = ( ( block + 1 ) * 4 ) + ( LandRecordSize * landnum ); // correct
		tilefile->seek( pos, SEEK_SET );
		tilefile->get_land_st( land );
	}
}

bool cMapStuff::InsideValidWorld( SI16 x, SI16 y, UI08 worldNumber )
{
	if( x < 0 || y < 0 )
		return false;
	if( worldNumber >= NumberOfWorlds )
		return false;
	UOMapType toGet = GetMapType( worldNumber );
	return( x >= 0 && x < MapTileWidths[toGet] && y >= 0 && y < MapTileHeights[toGet] );
}

/*
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
**      staticrecord *stat = NULL;
**      CTile tile;
**      while( stat = msi.Next() )
**      {
**          msi.GetTile( &tile );
**  		    ... your code here...
**	  	}
*/
MapStaticIterator::MapStaticIterator( UI32 x, UI32 y, UI08 world, bool exact ) : worldNumber( world ), baseX( static_cast<SI32>(x / 8 )), baseY( static_cast<SI32>(y / 8) ), remainX( static_cast<UI08>(x % 8 )), remainY( static_cast<UI08>(y % 8 )), length( 0 ), index( 0 ), pos( 0 ), exactCoords( exact ), tileid( 0 )
{
	if( !Map->InsideValidWorld( static_cast<SI16>(baseX), static_cast<SI16>(baseY), world ) )
	{
		Console.Error( 3, "ASSERT: MapStaticIterator(); Not inside a valid world" );
		return;
	}
	if( Map->Cache )
	{
		StaCache_st *cacheEntry = Map->GrabCacheEntry( static_cast<SI16>(baseX), static_cast<SI16>(baseY), world );
		if( cacheEntry == NULL )
			length = 0;
		else
			length = cacheEntry->CacheLen;
	}
	else
	{
		UOXFile *reading = Map->sidxArrays[world];
		if( reading == NULL )
		{
			length = 0;
			return;
		}
		UOMapType gMap = Map->GetMapType( world );
		const SI16 TileHeight = MapTileHeights[gMap];
		const SI32 indexPos = (( baseX * TileHeight * 12L ) + ( baseY * 12L ));
		reading->seek( indexPos, SEEK_SET );
		if( !reading->eof() )
		{
			reading->getLong( &pos );
			if( pos != -1 )
			{
				reading->getULong( &length );
				length /= StaticRecordSize;
			}
		}
	}
}

staticrecord *MapStaticIterator::First( void )
{
	index = 0;
	return Next();
}

staticrecord *MapStaticIterator::Next( void )
{
	tileid = 0;
	if( index >= length )
		return NULL;
	if( Map->Cache )
	{
#ifdef MAP_CACHE_DEBUG
		// turn this on for debugging z-level stuff where you want to see where
		// the characters are in relation to their cell, and which places in the 8x8
		// cell have static tiles defined.
		if( index == 0 )
		{
			Console << "baseX: " << baseX << ", baseY: " << baseY << ", remX: " << (int)remainX << ", remY: " << (int)remainY << myendl;
			Console << " 01234567" << myendl;
			char testmap[9][9];
			memset(testmap, ' ', 9*9);
			for( int tmp = 0; tmp < length; ++tmp )
			{
				staticrecord *ptr = Map->StaticCache[baseX][baseY].Cache + tmp;
				testmap[ptr->yoff][ptr->xoff] = 'X';
			}
			testmap[remainY][remainX] = 'O';
			for( int foo = 0; foo < 8; ++foo )
			{
				testmap[foo][8] = '\0';
				Console << foo << testmap[foo] << myendl;
			}
		}
#endif
		StaCache_st *mCache = Map->GrabCacheEntry( static_cast<SI16>(baseX), static_cast<SI16>(baseY), worldNumber );
		if( mCache != NULL )
		{
			do
			{
				staticrecord *ptr = (mCache->Cache) + index++;
				if( !exactCoords || ( ptr->xoff == remainX && ptr->yoff == remainY ) )
				{
					tileid = ptr->itemid;
					return ptr;
				}
			} while( index < length );
		}
		return NULL;
	}
	
	UOXFile *mFile = Map->statArrays[worldNumber];
	if( mFile == NULL )
		return NULL;
	
	// this was sizeof(OldStaRec) which SHOULD be 7, but on some systems which don't know how to pack, 
	const SI32 pos2 = pos + (StaticRecordSize * index);	// skip over all the ones we've read so far
	mFile->seek( pos2, SEEK_SET );
	do
	{
		if( mFile->eof( ) )
			return NULL;
		
		mFile->get_staticrecord( &staticArray );
		++index;
		//assert( staticArray.itemid >= 0 ); //itemid must always be >=0, because it is a Unsigned integer. - Shadowlord
		// if these are ever larger than 7 we've gotten out of sync
		assert( staticArray.xoff < 0x08 );
		assert( staticArray.yoff < 0x08 );
		if( !exactCoords || ( staticArray.xoff == remainX && staticArray.yoff == remainY ) )
		{
#ifdef DEBUG_MAP_STUFF
			Console << "Found static at index: " << index << ", Length: " << length << ", indepos: " << pos2 << myendl;
			Console << "item is " << (int)staticArray.itemid << ", x" << (int)staticArray.xoff << ", y: " << (int) staticArray.yoff << myendl;
#endif
			tileid = staticArray.itemid;
			return &staticArray;
		}
	} while( index < length );
	
	return NULL;
}

// since 99% of the time we want the tile at the requested location, here's a
// helper function.  pass in the pointer to a struct you want filled.
void MapStaticIterator::GetTile( CTile *tile ) const
{
	assert( tile );
	Map->SeekTile( tileid, tile );
}

//	
//	 some clean up to the caching and it wasn't reporting all the memory actually
//	 used by the StaticCache[][] in cMapStuff
//	
void cMapStuff::CacheStatics( void )
{
	// we must be in caching mode, only turn it off for now because we are
	// trying to fill the cache.
	assert( Cache );
	Cache = false;
	
	UI32 tableMemory = 0; // StaticBlocks * sizeof( staticrecord );
	UI32 indexMemory = 0; // StaticBlocks * sizeof( StaCache_st );
	UI32 tenPercent = StaticBlocks / 10;
	UI32 currentBlock = 0;
	for( UI08 wCtr = 0; wCtr < NumberOfWorlds; wCtr++ )
	{
		Console << "Caching Statics for World " << (SI16)wCtr << "...   ";
		Console.TurnYellow();
		Console << "0%";
	
		currentBlock = 0;
		if( statArrays[wCtr] == NULL )
			continue;
		UI08 wrldCtr = uomapTypes[wCtr];
		if( wrldCtr == 0xFF )
		{
			Console << "\b\b";
			Console.PrintFailed();
			Console.TurnRed();
			Console << "Unknown world type found, please verify the map file is correct" << myendl;
			Console.TurnNormal();
			continue;
		}
		if( !statArrays[wCtr]->ready() )
			continue;
		SI16 maxX = MapTileWidths[wrldCtr];
		SI16 maxY = MapTileHeights[wrldCtr];
		StaticBlocks = ( maxX * maxY );

		tableMemory += StaticBlocks * sizeof( staticrecord );
		indexMemory += StaticBlocks * sizeof( StaCache_st );
		tenPercent = StaticBlocks / 10;

		StaticCache[wCtr].Resize( maxX, maxY );
		for( SI16 x = 0; x < maxX; x++ )
		{
			for( SI16 y = 0; y < maxY; y++ )
			{
				StaCache_st *mCache = GrabCacheEntry( x, y, wCtr );
				if( mCache == NULL )
					continue;
				mCache->Cache = NULL;
				mCache->CacheLen = 0;
				
				MapStaticIterator msi( x * 8, y * 8, wrldCtr, false );
				UI32 length = msi.GetLength();
				if( length )
				{
					mCache->CacheLen = static_cast<UI16>(length);
					mCache->Cache = new staticrecord[length];
					// read them all in at once!
					statArrays[wrldCtr]->seek( msi.GetPos(), SEEK_SET );
					statArrays[wrldCtr]->get_staticrecord( mCache->Cache, length );
				}
				if( currentBlock++ % tenPercent == 0 )
				{
					if( currentBlock/tenPercent < 2 )
						Console << "\b\b" << (SI16)(currentBlock/tenPercent*10) << "%";
					else
						Console << "\b\b\b" << (SI16)(currentBlock/tenPercent*10) << "%";
				}
			}
		} 
		Console << "\b\b\b\b";
		Console.PrintDone();
	}	
	// reenable the caching now that its filled
	Cache = true;
	StaMem = tableMemory + indexMemory;
	Console << "Used " << tableMemory << " table bytes + " << indexMemory << " index bytes = " << StaMem << " total bytes to cache statics..." << myendl;
}

map_st cMapStuff::SeekMap0( SI16 x, SI16 y, UI08 worldNumber )
{
	const SI16 x1 = x / 8, y1 = y / 8;
	const UI08 x2 = (UI08)(x % 8), y2 = (UI08)(y % 8);
	static SI16 CurCachePos = 0;
	map_st map;
	memset( &map, 0, sizeof( map_st ) );
	if( worldNumber >= NumberOfWorlds )
		return map;

	UOMapType gMap = GetMapType( worldNumber );
	if( gMap == UOMT_UNKNOWN )
		return map;
	if( !Cache ) 
	{
		const SI32 pos = ( x1 * MapTileHeights[gMap] * 196 ) + ( y1 * 196 ) + ( y2 * 24 ) + ( x2 * 3 ) + 4;
		mapArrays[worldNumber]->seek( pos, SEEK_SET );
		mapArrays[worldNumber]->get_map_st( &map );
		return map;
    }
	
	SI16 i;
	// sorry zip, but these loops should have been checking the newest stuff in the
	// cache first, so its more likely to find a hit faster - fur
	MapCache *mapCache = Map0Cache[worldNumber];
	for( i = CurCachePos; i >= 0; --i )
	{
		if( ( mapCache[i].xb == x1 && mapCache[i].yb == y1 ) && ( mapCache[i].xo == x2 && mapCache[i].yo == y2 ) )
		{
			++Map0CacheHit;
			return mapCache[i].Cache;
		}
	}
	// if still not found, start at the end where the newer items are
	for( i = MAP0CACHE - 1; i > CurCachePos; --i )
	{
		if( ( mapCache[i].xb == x1 && mapCache[i].yb == y1 ) && ( mapCache[i].xo == x2 && mapCache[i].yo == y2 ) )
		{
			++Map0CacheHit;
			return mapCache[i].Cache;
		}
	}

	Map0CacheMiss++;

	const SI32 pos = ( x1 * MapTileHeights[gMap] * 196 ) + ( y1 * 196 ) + ( y2 * 24 ) + ( x2 * 3 ) + 4;
	mapArrays[worldNumber]->seek( pos, SEEK_SET );
	map_st *ptr = &(Map0Cache[worldNumber][CurCachePos].Cache);
	mapArrays[worldNumber]->get_map_st( ptr );

	Map0Cache[worldNumber][CurCachePos].xb = x1;
	Map0Cache[worldNumber][CurCachePos].yb = y1;
	Map0Cache[worldNumber][CurCachePos].xo = x2;
	Map0Cache[worldNumber][CurCachePos].yo = y2;

	// don't increment this until AFTER we have loaded everything, i had to fix what zippy did
	CurCachePos++;
	if( CurCachePos >= MAP0CACHE )
		CurCachePos = 0;

	return *ptr;
}

// these two functions don't look like they are actually used by anything
// anymore, at least we know which bit means wet
bool cMapStuff::IsTileWet(int tilenum)   // lord binary
{
	CTile tile;
	SeekTile( tilenum, &tile );
	return tile.LiquidWet();
}

// i don't know what this bit means exactly, its a walkway? or you are allowed
// to walk?
bool cMapStuff::TileWalk(int tilenum)
{
	CTile tile;
	SeekTile( tilenum, &tile );
	
	return tile.Walk();
}

// Blocking statics at/above given coordinates?
bool cMapStuff::DoesStaticBlock( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber )
{
	MapStaticIterator msi( x, y, worldNumber );
	
	staticrecord *stat = NULL;
	while( stat = msi.Next() )
	{
		const int elev = stat->zoff + TileHeight( stat->itemid );
		if( (elev > oldz) && (stat->zoff <= oldz ) )
		{
			bool btemp = DoesTileBlock( stat->itemid );
			if( btemp )
				return true;
		}
	}
	return false;
}

// Return new height of player who walked to X/Y but from OLDZ
SI08 cMapStuff::Height( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber )
{
	// let's check in this order.. dynamic, static, then the map
	SI08 dynz = DynamicElevation( x, y, oldz, worldNumber );
	if( illegal_z != dynz )
		return dynz;

	SI08 staticz = StaticTop( x, y, oldz, worldNumber );
	if( illegal_z != staticz )
		return staticz;

	return MapElevation( x, y, worldNumber );
}

// can the monster move here from an adjacent cell at elevation 'oldz'
// use illegal_z if they are teleporting from an unknown z
bool cMapStuff::CanMonsterMoveHere( SI16 x, SI16 y, SI08 oldz, UI08 worldNumber )
{
	UOMapType gMap = GetMapType( worldNumber );
	if( x < 0 || y < 0 || x >= MapWidths[gMap] || y >= MapHeights[gMap]  )
		return false;
    const SI08 elev = Height( x, y, oldz, worldNumber );
	if( illegal_z == elev )
		return false;

	// is it too great of a difference z-value wise?
	if( oldz != illegal_z )
	{
		// you can climb MaxZstep, but fall up to 15
		if( elev - oldz > MaxZstep )
			return false;
		else if( oldz - elev > 15 )
			return false;
	}

    // get the tile id of any dynamic tiles at this spot
    const int dt = DynTile( x, y, elev, worldNumber );
	
    // if there is a dynamic tile at this spot, check to see if its a blocker
    // if it does block, might as well short-circuit and return right away
    if( dt >= 0 && DoesTileBlock( dt ) )
		return false;
	
    // if there's a static block here in our way, return false
    if( DoesStaticBlock( x, y, elev, worldNumber ) )
		return false;
	
    return true;
}

bool cMapStuff::IsRoofOrFloorTile( CTile *tile )
// checks to see if the tile is either a roof or floor tile
{
	if( tile->AtFloorLevel() )
		return true; // check the floor bit
	
	if( strstr( "roof", tile->Name()) || strstr( "shingle", tile->Name() ) )
		return true;

	if( strstr( "floor", tile->Name() ) )
		return true;
	// now why would not want to check the z value of wooden boards first??
	// this was after the if( .. > z ), i'm moving this up inside of it
	if( !strcmp( tile->Name(), "wooden boards" ) )
		return true;
	// i'll stick these back in. even if these were bogus tile names it can't hurt
	if( !strcmp( tile->Name(), "wooden board" ) ||
		!strcmp( tile->Name(), "stone pavern" ) ||
		!strcmp( tile->Name(), "stone pavers" ) )
		return true;

	return false;
}

bool cMapStuff::IsRoofOrFloorTile( CTileUni *tile )
// checks to see if the tile is either a roof or floor tile
{
	CTile newTile;
	SeekTile( tile->ID(), &newTile );
	return IsRoofOrFloorTile( &newTile );
}

void cMapStuff::CalculateMultiSizes( void )
{
	for( UI32 i = 0; i < multiCache.size(); i++ )
	{
		if( multiCache[i] == NULL )
			continue;
		st_multi *multi = multiCache[i]->cache;
		if( multi == NULL )
			continue;
		SI16 lx = 0, ly = 0, hx = 0, hy = 0;
		for( int j = 0; j < multiCache[i]->length; j++ )
		{
			if( multi[j].x < lx ) 
				lx = multi[j].x;
			if( multi[j].x > hx ) 
				hx = multi[j].x;
			if( multi[j].y < ly ) 
				ly = multi[j].y;
			if( multi[j].y > hy ) 
				hy = multi[j].y;
		}
		multiCache[i]->lx = lx;
		multiCache[i]->ly = ly;
		multiCache[i]->hx = hx;
		multiCache[i]->hy = hy;
	}
}

void cMapStuff::CacheMultis( void )
{
	UI32 indexMemoryUsed = 0;
	UI32 multiMemoryUsed = 0;

	Console << "Caching Multis....  "; 

	// we must be in caching mode, only turn it off for now because we are
	// trying to fill the cache.

	std::string temp = BuildFilePath( "multi.idx" );
	FILE *multiIDXRec = fopen( temp.c_str(), "rb" );
	if( multiIDXRec == NULL )
	{
		Console.PrintFailed();
		Console.Error( 1, "Can't cache %s!  File cannot be opened", temp.c_str() );
		return;
	}
	temp = BuildFilePath( "multi.mul" );
	UOXFile *multis = new UOXFile( temp.c_str(), "rb" );
	if( multis == NULL || !multis->ready() )
	{
		Console.PrintFailed();
		Console.Error( 1, "Can't cache %s!  File cannot be opened", temp.c_str() );
		fclose(multiIDXRec);
		return;
	}

	fseek( multiIDXRec, 0, SEEK_END );
	SI32 fileLen = ftell( multiIDXRec );
	fclose( multiIDXRec );

	UI32 numRecords = fileLen / MultiIndexRecordSize;

	temp = BuildFilePath( "multi.idx" );
	UOXFile *multiIDX = new UOXFile( temp.c_str(), "rb" );
	if( multiIDX == NULL || !multiIDX->ready() )
	{
		Console.Error( 1, "Can't cache %s!  File cannot be opened", temp.c_str() );
		return;
	}

	multiCache.resize( numRecords );					// do one big resize here
	indexMemoryUsed = multiCache.size() * sizeof( MultiCache );

	const UI32 tenPercent = numRecords / 10;

	st_multiidx multiidx;

	Console.TurnYellow();
	Console << " 0%";
	for( UI32 counter = 0; counter < numRecords; counter++ )
	{
		multiCache[counter] = new MultiCache;

        multiIDX->seek( counter * MultiIndexRecordSize, SEEK_SET );
		multiIDX->get_st_multiidx( &multiidx );

		multiCache[counter]->length = multiidx.length;
		if( multiCache[counter]->length != -1 )
		{
			multiCache[counter]->length /= MultiRecordSize;
			multiCache[counter]->cache = new st_multi[multiCache[counter]->length];
			multiMemoryUsed += ( multiCache[counter]->length * sizeof( st_multi ) );
			multis->seek( multiidx.start, SEEK_SET );
			
			for( int multiCounter = 0; multiCounter < multiCache[counter]->length; multiCounter++ )
				multis->get_st_multi( &multiCache[counter]->cache[multiCounter] );
		}
		if( counter % tenPercent == 0 )
		{
			if( counter/tenPercent < 2 )
				Console << "\b\b" << (SI16)(counter/tenPercent*10) << "%";
			else
				Console << "\b\b\b" << (SI16)(counter/tenPercent*10) << "%";
		}
	}
	Console << "\b\b\b\b";
	Console.TurnNormal();
	CalculateMultiSizes();
	// reenable the caching now that its filled
	Console.PrintDone();
	Console << "Multi caching taking up " << indexMemoryUsed + multiMemoryUsed << " bytes, " << indexMemoryUsed << " in index, " << multiMemoryUsed << " in data..." << myendl;
	delete multiIDX;
}

void cMapStuff::SeekMulti( UI32 multinum, SI32 *length )
// multinum is also the index into the array!!!  Makes our life MUCH easier, doesn't it?
// TODO: Make it verseek compliant!!!!!!!!!
// Currently, there is no hit on either of the multi files, so we're fairly safe
// But we probably want to not rely on this being always true
// especially with the extra land patch coming up!
{
	if( multinum >= multiCache.size() )
		*length = -1;
	else
		*length = multiCache[multinum]->length;
}

st_multi *cMapStuff::SeekIntoMulti( int multinum, int number )
{
	return &multiCache[multinum]->cache[number];
}


std::string cMapStuff::BuildFilePath( const char *fName ) 
{
	char temp[MAX_PATH];
	sprintf( temp, "%s/%s", cwmWorldState->ServerData()->GetDataDirectory(), fName );
	return temp;
}


UOMapType cMapStuff::GetMapType( UI08 worldNumber )
{
	if( worldNumber >= NumberOfWorlds )
		return UOMT_BRITANNIA;
	return uomapTypes[worldNumber];
}

void CTile::Read( UOXFile *toRead )
{
	toRead->getUChar( &flag1 );
	toRead->getUChar( &flag2 );
	toRead->getUChar( &flag3 );
	toRead->getUChar( &flag4 );
	toRead->getUChar( &weight );
	toRead->getChar(  &layer );
	toRead->getLong(  &unknown1 );
	toRead->getLong(  &animation );
	toRead->getChar(  &unknown2 );
	toRead->getChar(  &unknown3 );
	toRead->getChar(  &height );
	toRead->getChar(  name, 20 );

}
void CLand::Read( UOXFile *toRead )
{
	toRead->getUChar( &flag1 );
	toRead->getUChar( &flag2 );
	toRead->getUChar( &flag3 );
	toRead->getUChar( &flag4 );
	toRead->getChar( &unknown1 );
	toRead->getChar( &unknown2 );
	toRead->getChar( name, 20 );
}

StaWorldCache::StaWorldCache( SI16 maxX, SI16 maxY )
{
	Resize( maxX, maxY );
}

//o--------------------------------------------------------------------------
//|	Function		-	~StaWorldCache()
//|	Date			-	26th September, 2001
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Deallocates any memory associated with the world cache
//o--------------------------------------------------------------------------
StaWorldCache::~StaWorldCache()
{
}
//o--------------------------------------------------------------------------
//|	Function		-	StaCacheLine *GrabCacheLine( SI16 x )
//|	Date			-	26th September, 2001
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the cache line associated with line x
//o--------------------------------------------------------------------------
StaCacheLine *StaWorldCache::GrabCacheLine( SI16 trgX )
{
	if( trgX >= (SI16)CacheList.size() || trgX < 0 )
		return NULL;
	return &(CacheList[trgX]);
}

StaWorldCache::StaWorldCache( void )
{
}

//o--------------------------------------------------------------------------
//|	Function		-	Resize( SI16 maxX, SI16 maxY )
//|	Date			-	26th September, 2001
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Resizes the world cache to a grid size of maxX, maxY
//o--------------------------------------------------------------------------
void StaWorldCache::Resize( SI16 maxX, SI16 maxY )
{
	CacheList.resize( maxX );
	for( SI16 i = 0; i < maxX; i++ )
		CacheList[i].resize( maxY );
}

//o--------------------------------------------------------------------------
//|	Function		-	StaCache_st *GrabCacheEntry( SI16 x, SI16 y, UI08 worldNumber )
//|	Date			-	26th September, 2001
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns a pointer to the static cache entry for the 
//|						grid x,y in worldNumber
//o--------------------------------------------------------------------------
StaCache_st *cMapStuff::GrabCacheEntry( SI16 x, SI16 y, UI08 worldNumber )
{
	if( worldNumber >= NumberOfWorlds )
		return NULL;
	StaCacheLine *line = StaticCache[worldNumber].GrabCacheLine( x );
	if( line == NULL )
		return NULL;
	return &((*line)[y]);
}

//o--------------------------------------------------------------------------
//|	Function		-	UOMapType CalcFromFileLength( UOXFile *toCalcFrom )
//|	Date			-	26th September, 2001
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	It returns the map type, based on the map's file size
//o--------------------------------------------------------------------------
UOMapType cMapStuff::CalcFromFileLength( UOXFile *toCalcFrom )
{
	if( toCalcFrom == NULL )
	{
		Console << "Invalid file in cMapStuff:CaclFromFileLength";
		return UOMT_UNKNOWN;
	}
	SI32 fLength = toCalcFrom->getLength();
	for( UI32 i = 0; i < UOMT_COUNT; i++ )
	{
		if( MapFileLengths[i] == fLength )
			return (UOMapType)i;
	}
	Console << "Incorrect map Length " << fLength;
	return UOMT_UNKNOWN;
}

//o--------------------------------------------------------------------------
//|	Function		-	bool MapExists( UI08 worldNumber )
//|	Date			-	27th September, 2001
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns true if the server has that map in memory
//o--------------------------------------------------------------------------
bool cMapStuff::MapExists( UI08 worldNumber )
{
	if( worldNumber >= NumberOfWorlds )
		return false;
	return (mapArrays[worldNumber] != NULL);
}
