//------------------------------------------------------------------------
//  admin.h
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

/* Admin code by Cironian */

#ifndef __ITEMHANDLE_H
#define	__ITEMHANDLE_H

//	System Includes
#include <vector>
//	Namespace

using namespace std;	

//	Third Party Libraries



//	Forward Class Declaration
class cItemHandle ;

//	UOX3 includes



//++++++++++++++++++ Class Definition ++++++++++++++++++++++++++++++++++++++++++++++++++++
class cItemHandle
{
public:
	cItemHandle(void) ;
	~cItemHandle();
	/// Get Memory for a new item. Freturns Item number
	unsigned long New(void) ;
	/// Free memory used by this item
	void Delete(long int) ;
	/// Return the size (in bytes) of ram items
	unsigned long Size(void) ;
	/// Reserve memory for this number of items
	void Reserve(unsigned int) ;
	/// Return Acctual-> the numver of items in world
	unsigned long Count(void) ;
	/// Reference an item
	item_st& operator[] ( long int) ;
		
private:
	/// The item send if an out of bounds is referenced
	item_st* DefaultItem;
	/// Vector of pointers to items. NULL if no item is at pos
	vector<item_st *> Items ;
	/// Number of items in existance
	unsigned long Acctual;
	///Vector of free item numbers
	vector<unsigned long> FreeNums;
	///Number of free spaces in Acctual (Recyle item numbers)
	unsigned int Free;
	///Check to see if this item is marked free
	bool isFree( unsigned long Num );
	
} ;
#endif
