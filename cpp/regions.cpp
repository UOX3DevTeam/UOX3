//------------------------------------------------------------------------
//  regions.cpp
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
#include "uox3.h"

// -- Region class  Added by Tauriel 3/6/1999 To lookup items by region
//									This should help for now. BTW- my first attempt at C++
//									So forgive any newbie mistakes :)
//									-- Side note, I wanted regions to be more generic, but
//									now that I have to do this, time doesn't allow for it yet.

cRegion::cRegion() //constructor
{
	int i=0;
	GridSize=32;
	ColSize=128;
	
	for (i=0;i<33000;i++)  // AntiChrist -- trying to use LordB values ( 33000 )
	{
		int memerrflg =0;
		//if ((MapCells[i].pointer = (int *) malloc(1 * sizeof(int)))== NULL)
		//	memerrflg=1;
		MapCells[i].pointer=new int[25];
		if (MapCells[i].pointer==NULL) memerrflg=1;

		if (!memerrflg)
		{
			MapCells[i].max=25;
			//for (a=0;a<25;a++) MapCells[i].pointer[a]=-1; // this is way slow - fur
			memset(MapCells[i].pointer, 0xFF, 25 * sizeof(int));
		} else {
			printf("Error allocating mapRegion memory!\n");
			return;
		}
	}
}

cRegion::~cRegion() //destructor
{
  for (int i = 0; i < 33000; ++i)
    delete [] MapCells[i].pointer;
}

// - Adds the item to a cell
SI16 cRegion::AddItem(unsigned long nItem) //Char mapRegions		// was unsigned int
{
	//unsigned int uiCell = GetCell(items[nItem].x,items[nItem].y); //Zippy
	int z = 0;
	unsigned int uiCell;
	if(nItem<CharacterOffset) 
	{
		if (nItem>=imem || nItem<0) return -1;
		uiCell = GetCell(items[nItem].x,items[nItem].y);
	} else {
	   z=nItem-CharacterOffset;
	   if (z>=cmem || z<0) return -1;
	   uiCell = GetCell(chars[z].x,chars[z].y); //
	}

    //	printf("item# %i added to mapcell %i [%i,%i,%i]\n", nItem, uiCell, items[nItem].x, items[nItem].y, items[nItem].z);
	
    if (uiCell<=32999) 
	{
	  setptr(&MapCells[uiCell], nItem); //set item in pointer array
	  return 0;
	} else return -1;
}

// - Removes the item from a cell
SI16 cRegion::RemoveItem(unsigned long nItem)//Char mapRegions	// was unsigned int
{
	int z=0;
	unsigned int uiCell;
	if(nItem<CharacterOffset) 
	{ 
		if ((nItem>=imem) || (nItem<0)) return -1;
		uiCell = GetCell(items[nItem].x,items[nItem].y);
	} else {
	   z=nItem-CharacterOffset;
	   if ((z>=cmem) || (z<0))  return -1;
	   uiCell = GetCell(chars[z].x,chars[z].y);		
	}

	if(uiCell<=32999)
	{
		// printf("item# %i removed from mapcell %i [%i,%i,%i]\n", nItem, uiCell, items[nItem].x, items[nItem].y, items[nItem].z);
		removefromptr(&MapCells[uiCell], nItem);
	} else return -1;
	
	return 0;
}

//- Returns the cell the character/item is in
unsigned int cRegion::GetCell(unsigned int x, unsigned int y)
{
	int cell = myGridx(x) + myGridy(y) + (myGridx(x) * (ColSize-1));
	return (unsigned int) ((cell<0) ? 0 : cell);  // - Return 0 if negative otherwise cell #
}

// - Get the next item pointer in a cell return -1 if done
long cRegion::GetNextItem(unsigned int cell, unsigned int Last)
{
	if (Last==-1) Last=0;
	else Last++;
	if (cell>32998 || cell<0) return -1;
	for (int i=Last;i<MapCells[cell].max;i++)
	{
		if (MapCells[cell].pointer[i] != -1) return i;
	}
	return -1; //didn't find another one
}

// - Get an item in a cell return -1 if done
long cRegion::GetItem(unsigned int cell, unsigned int item)
{
	if (cell>32999 || cell<0) return -1;
	/*printf("Map region %i dump.\n", cell);
	UI16 i;
	for (i=0;i<MapCells[cell].max;i++)
		printf("   %i: %i\n", i, MapCells[cell].pointer[i]);
	printf("Done.\n");*/
	if (item>MapCells[cell].max || item<0) return -1;
	return MapCells[cell].pointer[item];
}

// - Get starting grid for lookup 96x96 box
// - (we check the 8 surrounding cells and the cell char/item is in)
unsigned int cRegion::StartGrid(unsigned int x, unsigned int y)
{
	int gridx=myGridx(x)-1, gridy=myGridy(y)-1;
	if (gridx<0) gridx=0;
	if (gridy<0) gridy=0;
	return (unsigned int) (gridx + gridy + (gridx * (ColSize-1)));
}


RegionIterator::RegionIterator(int myX, int myY, RegionIteratorDomain dm) :
	x(myX), y(myY), mapitemptr(-1), mapitem(-1), domain(dm)
{
	getcell = mapRegions->GetCell(x,y);
}

int RegionIterator::First()
{
	mapitemptr = mapitem = -1;
	return Next();
}

int RegionIterator::Next()
{
	mapitemptr = mapRegions->GetNextItem(getcell, mapitemptr);
	if (mapitemptr==-1) 
		return -1;

	mapitem = mapRegions->GetItem(getcell, mapitemptr);
	if (domain == ItemsOnly && !IsItem())
		return Next();
	if (domain == CharsOnly && !IsChar())
		return Next();
	return mapitem;
}

bool RegionIterator::End()
{
	return (mapitem == -1);// || mapitemptr == -1 );
}

bool RegionIterator::IsItem() const
{
	return mapitem >= 0 && mapitem < CharacterOffset;
}

bool RegionIterator::IsChar() const
{
	return mapitem >= CharacterOffset;
}

bool RegionIterator::IsMulti() const
{
	return items[mapitem].id1 >= 0x40;
}


