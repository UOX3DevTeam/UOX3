#ifndef __WHOLIST_H__
#define __WHOLIST_H__

class cWhoList
{
private:
	bool needsUpdating;					// true if the list needs updating (new player online, new char made)
	int  gmCount;						// number of GMs already in it
	bool	  online;

	std::vector< SERIAL > whoMenuData;
	stringList one, two;				// replacement for entries1, entries2

	void Update( void );				// force the list to update
	void ResetUpdateFlag( void );
	void AddSerial( SERIAL toAdd );
	void Delete( void );
	void Command( cSocket *toSendTo, UI08 type, UI16 buttonPressed );
public:
			cWhoList( bool trulyOnline = true );
	virtual	~cWhoList();
	void	FlagUpdate( void );
	void	SendSocket( cSocket *toSendTo, UI08 option = 1 );
	void	GMEnter( void );
	void	GMLeave( void );
	SERIAL	GrabSerial( CHARACTER index );
	void	ButtonSelect( cSocket *toSendTo, UI16 buttonPressed, UI08 type );
	void	ZeroWho( void );
	void	SetOnline( bool newValue );
};


#endif

