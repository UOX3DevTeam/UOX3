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


#ifndef __TEFFECT_H
#define	__TEFFECT_H

//	System Includes
#include <iostream>

//	Namespace

using namespace std;	

//	Third Party Libraries



//	Forward Class Declaration
class cTEffect;

//	UOX3 includes
#include <defines.h>
#include <basetype.h>
#include <structs.h>

//++++++++++++++++++ Class Definition ++++++++++++++++++++++++++++++++++++++++++++++++++++
// This class is designed as a replacement for the teffect array (which is nasty, and too big)
// It'll grow and shrink as required, and will hopefully help reduce memory troubles
class cTEffect
{
private:
	vector< teffect_st * > internalData;
	UI16 effectCount;
	UI16 currentEffect;
	bool delFlag;

public:
	cTEffect();
	~cTEffect();
	teffect_st *First( void );				// returns the first entry
	teffect_st *Current( void );			// returns current entry
	teffect_st *Next( void );				// returns the next entry
	bool AtEnd( void );						// have we hit the end?
	bool Add( teffect_st toAdd );			// adds another entry
	bool DelCurrent( void );				// deletes the current entry
	UI16 Count( void );						// returns count of number of effects
	teffect_st *GrabSpecific( UI16 index );	// grabs a specific index
};

#endif
