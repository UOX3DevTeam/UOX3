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

#ifndef __CLASSES_H
#define	__CLASSES_H

//	System Includes

//	Namespace

	

//	Third Party Libraries



//	Forward Class Declaration
class cAdmin ;

//	UOX3 includes



//++++++++++++++++++ Class Definition ++++++++++++++++++++++++++++++++++++++++++++++++++++
class cAdmin
{
public:
	void LoadAccounts() ;
	void ReadIni();
	void LoadWipe(void);
	void GumpAMenu( int s, int j) ;
	void CheckLocks( int nAcct) ;
private:
	void Account() ;
	void ReadString();
	void Wiped(void);
	void GumpALine(int line) ;
	void GumpAText(int line, int s) ;
} ;
#endif
