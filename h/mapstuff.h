//------------------------------------------------------------------------
//  
//
//------------------------------------------------------------------------
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 1997 - 2001 by Marcus Rating (Cironian)
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//	
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//   
//------------------------------------------------------------------------
// In addition to that license, if you are running this program or modified  *
// versions of it on a public system you HAVE TO make the complete source of *
// the version used by you available or provide people with a location to    *
// download it.                                                              *


#ifndef __MAPSTUFF_H
#define	__MAPSTUFF_H

//	System Includes
#include <iostream>

//	Namespace

using namespace std;	

//	Third Party Libraries



//	Forward Class Declaration
class cMapStuff;

//	UOX3 includes
#include <defines.h>
#include <basetype.h>
#include <structs.h>

//++++++++++++++++++ Class Definition ++++++++++++++++++++++++++++++++++++++++++++++++++++
// full comments on this class are available in mapstuff.cpp
class cMapStuff
{
//Variables
private:
	friend class MapStaticIterator;

        // moved from global vars into here - fur 11/3/1999
        UOXFile *mapfile, *sidxfile, *statfile, *verfile, *tilefile, *multifile, *midxfile;

	// tile caching items
	tile_st tilecache[0x4000];

	// static caching items
	unsigned long StaticBlocks;
	struct StaCache_st
	{
		staticrecord *Cache;
		unsigned short CacheLen;   // i've seen this goto to at least 273 - fur 10/29/1999
	};

	// map caching items
	struct MapCache
	{
		unsigned short xb;
		unsigned short yb;
		unsigned char  xo;
		unsigned char  yo;
		map_st Cache;
	};
	MapCache Map0Cache[MAP0CACHE];

	// version caching items
	versionrecord *versionCache;
	UI32 versionRecordCount;

	// caching functions
	void CacheTiles( void );
	void CacheStatics( void );

public:
	// these used to be [512], thats a little excessive for a filename.. - fur
	char mapname[80], sidxname[80], statname[80], vername[80],
	  tilename[80], multiname[80], midxname[80];
	unsigned long StaMem, TileMem, versionMemory;
	unsigned int Map0CacheHit, Map0CacheMiss;
	// ok this is rather silly, allocating all the memory for the cache, even if
	// they haven't chosen to cache?? - fur
	StaCache_st StaticCache[MapTileWidth][MapTileHeight];
	unsigned char Cache;
	
// Functions
private:
	char VerLand(int landnum, land_st *land);
	signed char MultiHeight(int i, short int x, short int y, signed char oldz);
	int MultiTile(int i, short int x, short int y, signed char oldz);
	SI32 VerSeek(SI32 file, SI32 block);
	char VerTile(int tilenum, tile_st *tile);
	bool TileWalk(int tilenum);
	void CacheVersion();

	int DynTile( short int x, short int y, signed char oldz );
	bool DoesTileBlock(int tilenum);
	bool DoesStaticBlock(short int x, short int y, signed char oldz);

public:
	cMapStuff();
	~cMapStuff();

	void Load();

	// height functions
	bool IsUnderRoof(short int x, short int y, signed char z);
	signed char StaticTop(short int x, short int y, signed char oldz);
	signed char DynamicElevation(short int x, short int y, signed char oldz);
	signed char MapElevation(short int x, short int y);
	signed char AverageMapElevation(short int x, short int y, int &id);
	signed char TileHeight( int tilenum );
	signed char Height(short int x, short int y, signed char oldz);

	// look at tile functions
	void MultiArea(int i, int *x1, int *y1, int *x2, int *y2);
	void SeekTile(int tilenum, tile_st *tile);
	void SeekMulti(int multinum, UOXFile **mfile, SI32 *length);
	void SeekLand(int landnum, land_st *land);
	map_st SeekMap0( unsigned short x, unsigned short y );
	bool IsRoofOrFloorTile( tile_st *tile );
	bool IsRoofOrFloorTile( unitile_st *tile );
	bool IsTileWet(int tilenum);

	// misc functions
	bool CanMonsterMoveHere( short int x, short int y, signed char z );
};


#endif
