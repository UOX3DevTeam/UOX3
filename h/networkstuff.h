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


#ifndef __NETWORKTUFF_H
#define	__NETWORKSTUFF_H

//	System Includes
#include <iostream>

//	Namespace

using namespace std;	

//	Third Party Libraries



//	Forward Class Declaration
class cNetworkStuff;

//	UOX3 includes
#include <defines.h>
#include <basetype.h>
#include <structs.h>

//++++++++++++++++++ Class Definition ++++++++++++++++++++++++++++++++++++++++++++++++++++
class cNetworkStuff
{
public:
	cNetworkStuff();
	~cNetworkStuff();
	void xSend(int s, void *point, int length, int test);
	void Disconnect(int s);
	void ClearBuffers();
	void CheckConn();
	void CheckMessage();
	void SockClose();
	void FlushBuffer(int s);
	int kr,faul; // needed because global varaibles cant be changes in constructores LB
private:
	void GenTable(int s, char a1, char a2, char a3, char a4);
	void DoStreamCode(int s);

	int xRecv(int s);
	void Processed(int s, int i);
	void Login1(int s);
	void Relay(int s);
	void GoodAuth(int s);
	void FailAuth(int s);
	void AuthTest(int s);
	void CharList(int s);
	int Receive(int s, int x, int a);
	void GetMsg(int s);
	void LogOut( UOXSOCKET s );
	void pSplit(char *pass0);
	void sockInit( void );
	int Pack(void *pvIn, void *pvOut, int len);
	signed long Authenticate( const char *username, const char *pass );
};

#endif
