//------------------------------------------------------------------------
//  regions.h
//
//------------------------------------------------------------------------
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 1997 - 2001 by Unknown real name (Tauriel)
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
// -- Region class  Added by Tauriel 3/6/1999 To lookup items by region
//									This should help for now. BTW- my first attempt at C++
//									So forgive any newbie mistakes :)
//									-- Side note, I wanted regions to be more generic, but
//									now that I have to do this, time doesn't allow for it yet.

#ifndef __Region_h
#define __Region_h

#ifdef _BORLAND_
#include "typedefs.h"	// sth: the local one... ICK
#endif
class cRegion
{
private:
	unsigned char GridSize;
	unsigned char ColSize;
	lookuptr_st MapCells[33000]; //A very large pointer structure array

public:
	cRegion(); //constructor
	virtual ~cRegion(); //destructor
	unsigned int GetCell(unsigned int x, unsigned int y);
	unsigned int StartGrid(unsigned int x, unsigned int y);
	SI16 AddItem(unsigned long nItem);
	SI16 RemoveItem(unsigned long nItem);
	long GetNextItem(unsigned int cell, unsigned int Last);
	long GetItem(unsigned int cell, unsigned int item);
	unsigned int GetColSize() const {return ColSize;};
	int myGridx(unsigned int x) {return x/GridSize;};
	int myGridy(unsigned int y) {return y/GridSize;};

};

const int CharacterOffset = 1000000;

enum RegionIteratorDomain { ItemsOnly, CharsOnly, ItemsAndChars };
class RegionIterator
{
private:
	int x, y, getcell, mapitemptr, mapitem;
	RegionIteratorDomain domain;

public:
	RegionIterator(int myX, int myY, RegionIteratorDomain dm = ItemsAndChars);
	~RegionIterator() { }

	int First();
	int Next();
	bool End();
	bool IsItem() const;
	bool IsChar() const;
	bool IsMulti() const;
	int GetX() const { return x; }
	int GetY() const { return y; }
};

#endif
