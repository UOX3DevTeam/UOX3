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


#ifndef __WEIGHT_H
#define	__WEIGHT_H

//	System Includes
#include <iostream>

//	Namespace

using namespace std;	

//	Third Party Libraries



//	Forward Class Declaration
class cTownStones;

//	UOX3 includes
#include <defines.h>
#include <basetype.h>
#include <structs.h>

//++++++++++++++++++ Class Definition ++++++++++++++++++++++++++++++++++++++++++++++++++++
class cWeight
{
private:
	double RecursePacks(int bp);
	int WhereSub(int i, int p);
	char CheckMaxWeightPack(int chars);
	void AddItemWeight(int i, int s);
	void SubtractItemWeight(int i, int s);
	void SubtractQuanWeight(int i, int s, int total);
	void AddAmountWeight(int i, int s, int amount);
	int WhereItem(int p, int i, int s);
public:
	int CheckWeight(int s, int k);
	int CheckWeight2(int s);
	void NewCalc(int p);
	double CalcWeightPack(int backpack);
	int ItemWeight(int item);
	int CheckWhereItem(int pack, int i, int s);
};


#endif
