//------------------------------------------------------------------------
//  worldmain.h
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
// WorldMain.h: interface for the CWorldMain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORLDMAIN_H__FA78B1D3_99E2_11D2_8143_00104B73C455__INCLUDED_)
#define AFX_WORLDMAIN_H__FA78B1D3_99E2_11D2_8143_00104B73C455__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWorldMain  
{
public:
	virtual int announce();
	virtual void announce(int choice);
	virtual void savenewworld(char x);
	void saveAccount( void );
	CWorldMain();
	virtual ~CWorldMain();

	void SetLoopSaveAmt( long toSet );
	long LoopSaveAmt( void );
	bool Saving( void );
private:
	bool isSaving;
	int DisplayWorldSaves;
	FILE *iWsc, *cWsc;
	unsigned long Cur, Max;
	long PerLoop;

	void SaveChar( long i );
	void SaveItem( long i );
};

#endif // !defined(AFX_WORLDMAIN_H__FA78B1D3_99E2_11D2_8143_00104B73C455__INCLUDED_)
