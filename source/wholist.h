#ifndef __WHOLIST_H__
#define __WHOLIST_H__

class CWhoList
{
private:
	bool		needsUpdating;					// true if the list needs updating (new player online, new char made)
	SI32		gmCount;						// number of GMs already in it
	bool		online;

	std::vector<SERIAL>	whoMenuData;
	std::vector<std::string> one, two;		// replacement for entries1, entries2

	void Update( void );				// force the list to update
	void ResetUpdateFlag( void );
	void AddSerial( SERIAL toAdd );
	void Delete( void );
	void Command( CSocket *toSendTo, UI08 type, UI16 buttonPressed );
public:
	CWhoList( bool trulyOnline = true );
	~CWhoList();
	void	FlagUpdate( void );
	void	SendSocket( CSocket *toSendTo );
	void	GMEnter( void );
	void	GMLeave( void );
	void	ButtonSelect( CSocket *toSendTo, UI16 buttonPressed, UI08 type );
	void	ZeroWho( void );
	void	SetOnline( bool newValue );
};

extern CWhoList *WhoList;
extern CWhoList *OffList;

#endif

