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


#ifndef __STRUCTS_H
#define	__STRUCTS_H

//	System Includes


//	Namespace

//using namespace std;	

//	Third Party Libraries



//	Forward Class Declaration

//	UOX3 includes

#include <defines.h>
#include <basetype.h>

//++++++++++++++++++ Class Definition ++++++++++++++++++++++++++++++++++++++++++++++++++++
struct tile_st
{
	SI32 unknown1;  // longs must go at top to avoid bus errors - fur
	SI32 animation;
	UI08 flag1;
	UI08 flag2;
	UI08 flag3;
	UI08 flag4;
	UI08 weight;
	SI08 layer;
	SI08 unknown2;
	SI08 unknown3;
	SI08 height;
	SI08 char name[23];	// manually padded to long to avoid bus errors - fur
} PACK_NEEDED;

struct land_st
{
	UI08 flag1;
	UI08 flag2;
	UI08 flag3;
	UI08 flag4;
	SI08 unknown1;
	SI08 unknown2;
	UI08 name[20];
};

#endif
