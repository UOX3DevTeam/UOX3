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


#ifndef __WHOLIST_H
#define	__WHOLIST_H

//	System Includes
#include <iostream>

//	Namespace

using namespace std;	

//	Third Party Libraries



//	Forward Class Declaration
class cWhoList;

//	UOX3 includes
#include <defines.h>
#include <basetype.h>
#include <structs.h>

//++++++++++++++++++ Class Definition ++++++++++++++++++++++++++++++++++++++++++++++++++++
// This class is designed as a replacement for the teffect array (which is nasty, and too big)
// It'll grow and shrink as required, and will hopefully help reduce memory troubles
class cWhoList
{
public:
	cWhoList( bool trulyOnline = true );
	~cWhoList();
	void FlagUpdate( void );
	void SendSocket( UOXSOCKET toSendTo, unsigned char option = 1 );
	void GMEnter( void );
	void GMLeave( void );
	long int GrabSerial( int index );
	void ButtonSelect( UOXSOCKET toSendTo, unsigned short int buttonPressed, unsigned char type );
	void ZeroWho( void );
	void SetOnline( bool newValue );
	
private:
	bool needsUpdating;					// true if the list needs updating (new player online, new char made)
	int  gmCount;						// number of GMs already in it
	bool	  online;

	vector< SERIAL > whoMenuData;
	stringList one, two;				// replacement for entries1, entries2

	void Update( void );				// force the list to update
	void ResetUpdateFlag( void );
	void AddSerial( SERIAL toAdd );
	void Delete( void );
	void Command( UOXSOCKET toSendTo, unsigned char type, unsigned short int buttonPressed );

};

#endif
