//------------------------------------------------------------------------
//  boats.h
//
//------------------------------------------------------------------------
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 1998 - 2001 by unknown real name (Zippy)
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

//For iSizeShipOffsets->
#define PORT_PLANK 0
#define STARB_PLANK 1
#define HOLD 2
#define TILLER 3

//For iShipItems->
#define PORT_P_C 0//Port Plank Closed
#define PORT_P_O 1//Port Planl Opened
#define STAR_P_C 2//Starboard Plank Closed
#define STAR_P_O 3//Starb Plank Open
#define TILLERID 5//Tiller
#define HOLDID 4//Hold

extern signed short int iSmallShipOffsets[4][4][2];
extern signed short int iMediumShipOffsets[4][4][2];
extern signed short int iLargeShipOffsets[4][4][2];
extern unsigned char cShipItems[4][6];

#ifndef __Boats_h
#define __Boats_h

class cBoat 
{
	private:
		void LeaveBoat(int, int);
		bool Block(int, short int, short int,int);
		void TurnStuff(int, int, int, int);

	public:
		cBoat();
		virtual ~cBoat();
		int GetBoat(int);
		void Speech(int, unsigned char *);
		void OpenPlank(int);
		void PlankStuff(int, int);
		bool Build(int, int, char);
		void Move(int, int, int);
		void Turn(int, int);
};
#endif


