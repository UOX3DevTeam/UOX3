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


#ifndef __MOVEMENT_H
#define	__MOVEMENT_H

//	System Includes
#include <iostream>

//	Namespace

using namespace std;	

//	Third Party Libraries



//	Forward Class Declaration
class cMovement;

//	UOX3 includes
#include <defines.h>
#include <basetype.h>
#include <structs.h>

//++++++++++++++++++ Class Definition ++++++++++++++++++++++++++++++++++++++++++++++++++++
class cMovement
{
	// Variable/Type definitions
private:
	signed char z, dispz;
	// Function declarations
public:
	void Walking( CHARACTER s, int dir, int seq );
	void CombatWalk( int s );
	int  calc_walk( CHARACTER c, unsigned int x, unsigned int y, unsigned int oldx, unsigned int oldy, bool justask );
	bool calc_move( CHARACTER c, short int x, short int y, signed char &z, int dir );
	int validNPCMove( short int x, short int y, signed char z, CHARACTER s );
	void NpcMovement( unsigned int currenttime, int i );
private:

	bool MoveHeightAdjustment( int MoveType, unitile_st *thisblock, int &ontype, signed int &nItemTop, signed int &nNewZ );
	bool isValidDirection(int dir);
	bool isFrozen(CHARACTER c, UOXSOCKET socket, int sequence);
	bool isOverloaded(CHARACTER c, UOXSOCKET socket, int sequence);

	bool CanGMWalk(unitile_st xyb);
	bool CanPlayerWalk(unitile_st xyb);
	bool CanNPCWalk(unitile_st xyb);
	bool CanFishWalk(unitile_st xyb);
	bool CanBirdWalk(unitile_st xyb);

	void FillXYBlockStuff(short int x, short int y, unitile_st *xyblock, int &xycount);
	void GetBlockingMap(SI16 x, SI16 y, unitile_st *xyblock, int &xycount, unsigned short oldx, unsigned short oldy );
	void GetBlockingStatics(SI16 x, SI16 y, unitile_st *xyblock, int &xycount);
	void GetBlockingDynamics(SI16 x, SI16 y, unitile_st *xyblock, int &xycount);

	short int Distance(short int sx, short int sy, short int dx, short int dy);
	short int Direction(short int sx, short int sy, short int dx, short int dy);

	short int CheckMovementType(CHARACTER c);
	bool CheckForCharacterAtXY(CHARACTER c);
	bool CheckForCharacterAtXYZ(CHARACTER c, short int cx, short int cy, signed char cz);
	void NpcWalk( CHARACTER i, int j, int type );
	unsigned short GetXfromDir( int dir, unsigned short x );
	unsigned short GetYfromDir( int dir, unsigned short y );
	void PathFind( CHARACTER c, unsigned short gx, unsigned short gy );

	bool VerifySequence(CHARACTER c, UOXSOCKET socket, int sequence);
	bool CheckForRunning(CHARACTER c, UOXSOCKET socket, int dir);
	bool CheckForStealth(CHARACTER c, UOXSOCKET socket);
	bool CheckForHouseBan(CHARACTER c, UOXSOCKET socket);
	void MoveCharForDirection(CHARACTER c, int dir);
	void HandleRegionStuffAfterMove(CHARACTER c, short int oldx, short int oldy);
	void SendWalkToPlayer(CHARACTER c, UOXSOCKET socket, short int sequence);
	void SendWalkToOtherPlayers(CHARACTER c, int dir, short int oldx, short int oldy);
	void OutputShoveMessage(CHARACTER c, UOXSOCKET socket, short int oldx, short int oldy);
	void HandleItemCollision( CHARACTER c, UOXSOCKET socket, bool amTurning, unsigned short oldx, unsigned short oldy );
	void HandleTeleporters(CHARACTER c, UOXSOCKET socket, short int oldx, short int oldy);
	void HandleWeatherChanges(CHARACTER c, UOXSOCKET socket);
	void HandleGlowItems(CHARACTER c, UOXSOCKET socket);
	bool IsGMBody(CHARACTER c);
	signed char CheckWalkable( CHARACTER c, unitile_st *xyblock, int xycount );
	bool CrazyXYBlockStuff(CHARACTER c, UOXSOCKET socket, short int oldx, short int oldy, int sequence);
	void FillXYBlockStuff(CHARACTER c, unitile_st *xyblock, int &xycount, unsigned short oldx, unsigned short oldy );

	void deny( int k, int s, int sequence );
};


#endif
