//------------------------------------------------------------------------
//  xgm.h
//
//------------------------------------------------------------------------
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 1999 - 2001 by Unknown real name (Zippy)
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
#ifndef __XGM_H_
#define __XGM_H_

#define MAX_XGM 10//Max number of xgm clients allowed.

class cRemote
{
//Variables:
private:
public:
	int Acct;//Account number
	int ClientSock;//Socket of Corrisonding Player
	char isClient;//Is this an XGM client?

//Methods:
private:
	void __cdecl Send( UOXSOCKET s, char *fmt, ... ); //sprintf and send a string
	//void Send( UOXSOCKET s, char *toSend ); //Send a pre made string
	void Send( UOXSOCKET s, char *toSend, unsigned int len ); //Send DATA
	void GMQue( UOXSOCKET s );
public:
	void Verify(UOXSOCKET s);//Verify an XGM account
	void CheckMsg(UOXSOCKET s);//Get messages from XGM
};

#endif
