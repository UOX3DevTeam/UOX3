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


#ifndef __MAPSTATICITERATOR_H
#define	__MAPSTATICITERATOR_H

//	System Includes
#include <iostream>

//	Namespace

using namespace std;	

//	Third Party Libraries



//	Forward Class Declaration
class MapStaticIterator;

//	UOX3 includes
#include <defines.h>
#include <basetype.h>
#include <structs.h>

//++++++++++++++++++ Class Definition ++++++++++++++++++++++++++++++++++++++++++++++++++++
// full comments on this class are available in mapstuff.cpp
class MapStaticIterator
{
public:
	MapStaticIterator(unsigned int x, unsigned int y, bool exact = true);
	~MapStaticIterator() { };

	staticrecord *First();
	staticrecord *Next();
	void GetTile(tile_st *tile) const;
	UI32 GetPos() const { return pos; }
	UI32 GetLength() const { return length; }
	
private:
	staticrecord staticArray;
	SI32 baseX, baseY, pos;
	unsigned char remainX, remainY;
	UI32 index, length, tileid;
	bool exactCoords;

	
};


#endif
