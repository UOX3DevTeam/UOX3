//o-----------------------------------------------------------------------------------------------o
//|	File		-	worldmain.h
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Interface for the CWorldMain class.
//o-----------------------------------------------------------------------------------------------o

#if !defined(__WORLDMAIN_H__)
#define __WORLDMAIN_H__

#include "GenericList.h"
#if PLATFORM == WINDOWS
#include <winsock2.h>
#undef min
#undef max
#endif

enum CWM_TID
{
	tWORLD_NEXTFIELDEFFECT = 0,
	tWORLD_NEXTNPCAI,
	tWORLD_SHOPRESTOCK,
	tWORLD_HUNGERDAMAGE,
	tWORLD_THIRSTDRAIN,
	tWORLD_LIGHTTIME,
	tWORLD_PETOFFLINECHECK,
	tWORLD_COUNT
};

class CServerProfile
{
private:
	UI32 networkTime;
	UI32 timerTime;
	UI32 autoTime;
	UI32 loopTime;
	UI32 networkTimeCount;
	UI32 timerTimeCount;
	UI32 autoTimeCount;
	UI32 loopTimeCount;

	SI32 globalRecv;
	SI32 globalSent;
public:
	CServerProfile();
	~CServerProfile();

	UI32	NetworkTime( void ) const;
	UI32	TimerTime( void ) const;
	UI32	AutoTime( void ) const;
	UI32	LoopTime( void ) const;

	void	NetworkTime( UI32 newVal );
	void	TimerTime( UI32 newVal );
	void	AutoTime( UI32 newVal );
	void	LoopTime( UI32 newVal );

	void	IncNetworkTime( UI32 toInc );
	void	IncTimerTime( UI32 toInc );
	void	IncAutoTime( UI32 toInc );
	void	IncLoopTime( UI32 toInc );

	UI32	NetworkTimeCount( void ) const;
	UI32	TimerTimeCount( void ) const;
	UI32	AutoTimeCount( void ) const;
	UI32	LoopTimeCount( void ) const;

	void	NetworkTimeCount( UI32 newVal );
	void	TimerTimeCount( UI32 newVal );
	void	AutoTimeCount( UI32 newVal );
	void	LoopTimeCount( UI32 newVal );

	void	IncNetworkTimeCount( void );
	void	IncTimerTimeCount( void );
	void	IncAutoTimeCount( void );
	void	IncLoopTimeCount( void );

	SI32	GlobalReceived( void ) const;
	SI32	GlobalSent( void ) const;

	void	GlobalReceived( SI32 newVal );
	void	GlobalSent( SI32 newVal );
};

class CWorldMain
{
private:
	struct skill_st
	{
		UI16 strength			= 0;
		UI16 dexterity			= 0;
		UI16 intelligence		= 0;
		std::string madeword	= "made";
		std::vector< advance_st > advancement;
		UI16 jsScript			= 0xFFFF;
		std::string name		= "";
		skill_st()
		{
			ResetDefaults();
		}
		void ResetDefaults( void )
		{
			advancement.resize( 0 );
		}
	};

	// Custom Titles
	struct title_st
	{
		std::string fame;
		std::string skill;
		title_st() : fame( "" ), skill( "" )
		{
		}
	};

	// Timers
	TIMERVAL	worldTimers[tWORLD_COUNT];

	// Console & Program Level Vars
	bool		error;
	bool		keeprun;
	bool		secure; // Secure mode
	bool		Loaded;

	// Time Functions
	UI32		uotickcount;
	UI32		starttime, endtime, lclock;
	bool		overflow;
	UI32		uiCurrentTime;

	// Worldsave
	UI32		oldtime, newtime;
	bool		autosaved;
	SaveStatus	worldSaveProgress;

	// IP Update
	UI32		oldIPtime, newIPtime;
	bool		ipupdated;

	// Misc
	size_t		playersOnline;					// Players online
	bool		reloadingScripts;
	bool		classesInitialized;

public:
	// Timers
	void		SetTimer( CWM_TID timerID, TIMERVAL newVal );
	TIMERVAL	GetTimer( CWM_TID timerID ) const;

	// Console & Program Level Vars
	void		SetError( bool newVal );
	bool		GetError( void ) const;
	void		SetKeepRun( bool newVal );
	bool		GetKeepRun( void ) const;
	void		SetSecure( bool newVal );
	bool		GetSecure( void ) const;
	void		SetLoaded( bool newVal );
	bool		GetLoaded( void ) const;

	// Time Functions
	void		SetUICurrentTime( UI32 newVal );
	UI32		GetUICurrentTime( void ) const;
	void		SetUOTickCount( UI32 newVal );
	UI32		GetUOTickCount( void ) const;
	void		SetOverflow( bool newVal );
	bool		GetOverflow( void ) const;
	void		SetStartTime( UI32 newVal );
	UI32		GetStartTime( void ) const;
	void		SetEndTime( UI32 newVal );
	UI32		GetEndTime( void ) const;
	void		SetLClock( UI32 newVal );
	UI32		GetLClock( void ) const;

	// Worldsave
	void		SetNewTime( UI32 newVal );
	UI32		GetNewTime( void ) const;
	void		SetOldTime( UI32 newVal );
	UI32		GetOldTime( void ) const;
	void		SetAutoSaved( bool newVal );
	bool		GetAutoSaved( void ) const;
	void		SetWorldSaveProgress( SaveStatus newVal );
	SaveStatus	GetWorldSaveProgress( void ) const;

	// IP update
	UI32		GetNewIPTime( void ) const;
	void		SetNewIPTime( UI32 newVal );
	UI32		GetOldIPTime( void ) const;
	void		SetOldIPTime( UI32 newVal );
	void		SetIPUpdated( bool newVal );
	bool		GetIPUpdated( void ) const;

	// Misc
	void		SetPlayersOnline( size_t newVal );
	size_t		GetPlayersOnline( void ) const;
	void		DecPlayersOnline( void );
	bool		GetReloadingScripts( void ) const;
	void		SetReloadingScripts( bool newVal );
	void		ClassesInitialized( bool newVal );
	bool		ClassesInitialized( void ) const;

	void		CheckAutoTimers( void );

	// Structs
	std::map< UI16, CCreatures >		creatures;
	timeval								uoxtimeout;
	skill_st							skill[INTELLECT+1];				// Skill data
	title_st							title[ALLSKILLS+1];			// For custom titles reads titles.dfn
	std::vector< TitlePair_st >			prowessTitles;
	std::vector< TitlePair_st >			murdererTags;
	std::vector< CTeleLocationEntry >	teleLocs;
	std::vector< LogoutLocationEntry >	logoutLocs;
	std::vector< UI08 >					escortRegions;
	std::map< UI16, GoPlaces_st >		goPlaces;
	SPAWNMAP							spawnRegions;
	TOWNMAP								townRegions;
	GenericList< CTEffect * >			tempEffects;

	QUEUEMAP							refreshQueue;
	QUEUEMAP							deletionQueue;

	void		CheckTimers( void );
	void		doWorldLight( void );
	void		SaveNewWorld( bool x );

	CWorldMain();
	~CWorldMain();
	CServerData *	ServerData( void );
	CServerProfile * ServerProfile( void );
private:
	void			RegionSave( void );
	void			SaveStatistics( void );

	CServerData *	sData;
	CServerProfile * sProfile;
};

extern CWorldMain								*cwmWorldState;

#endif
