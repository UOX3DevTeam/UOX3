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


#ifndef __GUILDS_H
#define	__GUILDS_H

//	System Includes
#include <iostream>

//	Namespace

using namespace std;	

//	Third Party Libraries



//	Forward Class Declaration
class cGuilds;

//	UOX3 includes
#include <defines.h>
#include <basetype.h>
#include <structs.h>

//++++++++++++++++++ Class Definition ++++++++++++++++++++++++++++++++++++++++++++++++++++
class cGuilds
{
public:
	guild_st guilds[MAXGUILDS]; //lb, moved from uox3.h cauz global variabels cant be changed in constuctors ...
	cGuilds();
	virtual ~cGuilds();
	void StonePlacement(int s);
	void Menu(int s, int page);
	void Resign(int s);
	void Recruit(int s);
	void TargetWar(int s);
	void StoneMove(int s);
	int Compare(int player1, int player2);
	void GumpInput(int s, int type, int index, char *text);
	void GumpChoice(int s, int main, int sub);
	int SearchByStone(int s);
	//void Init();
	void Title(int s, int player2);
	void Read(int guildnumber);
	void Write( FILE *wscfile );
	
private:
	void EraseMember(int c);
	void EraseGuild(int guildnumber);
	void ToggleAbbreviation(int s);
	int SearchSlot(int guildnumber, int type);
	void ChangeName(int s, char *text);
	void ChangeAbbreviation(int s, char *text);
	void ChangeTitle(int s, char *text);
	void ChangeCharter(int s, char *text);
	void ChangeWebpage(int s, char *text);
	int CheckValidPlace(int x, int y);
	void Broadcast(int guildnumber, char *text);
	void CalcMaster(int guildnumber);
	void SetType(int guildnumber, int type);
	
};




#endif
