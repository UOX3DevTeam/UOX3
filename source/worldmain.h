// WorldMain.h: interface for the CWorldMain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORLDMAIN_H__FA78B1D3_99E2_11D2_8143_00104B73C455__INCLUDED_)
#define AFX_WORLDMAIN_H__FA78B1D3_99E2_11D2_8143_00104B73C455__INCLUDED_

#if defined(_MSC_VER)
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#endif

#include "cServerData.h"

extern UI08 worldSaveProgress;	// 0 = not saving, 1 = saving, 2 = just saved

class CWorldMain  
{
public:
	virtual bool	announce( void );
	virtual void	announce( bool choice );
	virtual void	savenewworld( bool x );
					CWorldMain();
	virtual			~CWorldMain();
	bool			Saving( void );
	cServerData *	ServerData( void );
private:
	bool			isSaving;
	bool			DisplayWorldSaves;

	void			RegionSave( void );

	cServerData *	sData;
};

#endif // !defined(AFX_WORLDMAIN_H__FA78B1D3_99E2_11D2_8143_00104B73C455__INCLUDED_)
