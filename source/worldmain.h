// WorldMain.h: interface for the CWorldMain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__WORLDMAIN_H__)
#define __WORLDMAIN_H__

#if defined(_MSC_VER)
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#endif
#include "cServerData.h"

class CWorldMain  
{
protected:
	// Timers
	UI32	nextfieldeffecttime;
	UI32	nextnpcaitime;
	UI32	shoprestocktime;			// Shop respawn timer
	UI32	hungerdamagetimer;			// Hunger damage timer
	UI32	polyduration;

	// Items & Characters
	UI32	charcount, itemcount;
	SERIAL	charcount2, itemcount2;
	UI32	imem, cmem;
	UI08	weight_per_str;

	// Console & Program Level Vars
	bool	error;
	bool	keeprun;
	bool	secure; // Secure mode
	UI32	ErrorCount;
	bool	Loaded;

	// Time Functions
	UI16	secondsperuominute;		// Number of seconds for a UOX minute.
	UI32	uotickcount;
	UI32	starttime, endtime, lclock;
	bool	overflow;
	UI32	uiCurrentTime;
	UI32	lighttime;

	// Worldsave
	UI32	oldtime, newtime;
	bool	autosaved;
	UI08	worldSaveProgress;

	// Misc
	SI32	now;					// Players online
	bool	showlayer;
	UI16	totalspawnregions;
	bool	xgm;
	SI32	executebatch;
	UI16	locationcount;
	UI32	logoutcount;				// Instalog
	UI08	escortRegions;
	UI08	validEscortRegion[256];
	SI32	erroredLayers[MAXLAYERS];

public:
	// Timers
	void	SetNextFieldEffectTime( UI32 newVal );
	UI32	GetNextFieldEffectTime( void );
	void	SetNextNPCAITime( UI32 newVal );
	UI32	GetNextNPCAITime( void );
	void	SetShopRestockTime( UI32 newVal );
	UI32	GetShopRestockTime( void );
	void	SetHungerDamageTimer( UI32 newVal );
	UI32	GetHungerDamageTimer( void );
	void	SetPolyDuration( UI32 newVal );
	UI32	GetPolyDuration( void );

	// Items & Characters
	void	SetItemCount( UI32 newVal );
	UI32	GetItemCount( void );
	void	IncItemCount( void );
	void	SetCharCount( UI32 newVal );
	UI32	GetCharCount( void );
	void	IncCharCount( void );
	void	SetItemCount2( SERIAL newVal );
	SERIAL	GetItemCount2( void );
	void	IncItemCount2( void );
	void	SetCharCount2( SERIAL newVal );
	SERIAL	GetCharCount2( void );
	void	IncCharCount2( void );
	void	SetIMem( UI32 newVal );
	UI32	GetIMem( void );
	void	IncIMem( void );
	void	SetCMem( UI32 newVal );
	UI32	GetCMem( void );
	void	IncCMem( void );
	void	SetWeightPerStr( UI08 newVal );
	UI08	GetWeightPerStr( void );

	// Console & Program Level Vars
	void	SetError( bool newVal );
	bool	GetError( void );
	void	SetKeepRun( bool newVal );
	bool	GetKeepRun( void );
	void	SetSecure( bool newVal );
	bool	GetSecure( void );
	void	SetErrorCount( UI32 newVal );
	UI32	GetErrorCount( void );
	void	IncErrorCount( void );
	void	SetLoaded( bool newVal );
	bool	GetLoaded( void );

	// Time Functions
	void	SetUICurrentTime( UI32 newVal );
	UI32	GetUICurrentTime( void );
	void	SetSecondsPerUOMinute( UI16 newVal );
	UI16	GetSecondsPerUOMinute( void );
	void	SetUOTickCount( UI32 newVal );
	UI32	GetUOTickCount( void );
	void	SetOverflow( bool newVal );
	bool	GetOverflow( void );
	void	SetStartTime( UI32 newVal );
	UI32	GetStartTime( void );
	void	SetEndTime( UI32 newVal );
	UI32	GetEndTime( void );
	void	SetLClock( UI32 newVal );
	UI32	GetLClock( void );
	void	SetLightTime( UI32 newVal );
	UI32	GetLightTime( void );

	// Worldsave
	void	SetNewTime( UI32 newVal );
	UI32	GetNewTime( void );
	void	SetOldTime( UI32 newVal );
	UI32	GetOldTime( void );
	void	SetAutoSaved( bool newVal );
	bool	GetAutoSaved( void );
	void	SetWorldSaveProgress( UI08 newVal );
	UI08	GetWorldSaveProgress( void );

	// Misc
	void	SetPlayersOnline( SI32 newVal );
	SI32	GetPlayersOnline( void );
	void	DecPlayersOnline( void );
	void	SetDisplayLayers( bool newVal );
	bool	GetDisplayLayers( void );
	void	SetTotalSpawnRegions( UI16 newVal );
	UI16	GetTotalSpawnRegions( void );
	void	IncTotalSpawnRegions( void );
	void	SetXGMEnabled( bool newVal );
	bool	GetXGMEnabled( void );
	void	SetExecuteBatch( SI32 newVal );
	SI32	GetExecuteBatch( void );
	void	SetLocationCount( UI16 newVal );
	UI16	GetLocationCount( void );
	void	SetLogoutCount( UI32 newVal );
	UI32	GetLogoutCount( void );
	void	IncLogoutCount( void );
	void	SetEscortRegions( UI08 newVal );
	UI08	GetEscortRegions( void );
	void	IncEscortRegions( void );
	void	SetValidEscortRegion( UI08 part, UI08 newVal );
	UI08	GetValidEscortRegion( UI08 part );
	void	SetErroredLayer( UI08 part, SI32 newVal );
	SI32	GetErroredLayer( UI08 part );
	void	IncErroredLayer( UI08 part );

	void	ResetDefaults( void );
	void	CheckTimers( void );
	void	doWorldLight( void );

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

#endif
