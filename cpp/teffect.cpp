//------------------------------------------------------------------------
//  teffect.cpp
//
//------------------------------------------------------------------------
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 1999 - 2001 by Daniel Stratton (Abaddon)
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
#include "uox3.h"

cTEffect::cTEffect()
{
	internalData.resize( 0 );
	internalData.clear();
	effectCount = 0;
	currentEffect = 0;
	delFlag = false;
};

cTEffect::~cTEffect()
{
	for( int i = 0; i < effectCount; i++ )
		delete internalData[i];
}

teffect_st *cTEffect::First( void )		// returns the first entry
{
	currentEffect = 0;
	delFlag = false;
	if( effectCount == 0 )
		return NULL;
	return internalData[0];
	
}

teffect_st *cTEffect::Current( void )	// returns current entry
{
	if( currentEffect >= effectCount )
		return NULL;
	return internalData[currentEffect];
}

teffect_st *cTEffect::Next( void )		// returns the next entry
{
	currentEffect++;
	if( currentEffect >= effectCount )
	{
		delFlag = false;
		return NULL;
	}
	if( delFlag )
	{
		currentEffect--;
		delFlag = false;
	}
	return internalData[currentEffect];
}

bool cTEffect::AtEnd( void )			// have we hit the end?
{
	return( currentEffect >= effectCount );
}

bool cTEffect::Add( teffect_st toAdd )	// adds another entry
{
	internalData.resize( effectCount + 1 );
	internalData[effectCount] = new teffect_st;
	memcpy( internalData[effectCount], &toAdd, sizeof( teffect_st ) );
	effectCount++;
	return true;
}

bool cTEffect::DelCurrent( void )	// deletes the current entry
{
	if( currentEffect >= effectCount )
		return false;
	delete internalData[currentEffect];
	internalData.erase(internalData.begin()+currentEffect, internalData.begin()+currentEffect+1);

	effectCount--;
	delFlag = true;
	return true;
}

UI16 cTEffect::Count( void )
{
	return effectCount;
}

teffect_st *cTEffect::GrabSpecific( UI16 index )
{
	if( index >= effectCount )
		return NULL;
	currentEffect = index;
	return internalData[index];
}