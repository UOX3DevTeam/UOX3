#ifndef __CNETWORK_H__
#define __CNETWORK_H__

#include <stdexcept>
//using namespace std;

//#define __LOGIN_THREAD__

class socket_error : public std::runtime_error
{
protected:
	long		errorNum;
public:
				socket_error( const std::string& what_arg );
				socket_error( const long errorNumber );
				socket_error( void );
	long		ErrorNumber( void ) const;
	const char *what( void ) const throw();
};

class cPBuffer
{
protected:
	std::vector< UI08 > internalBuffer;
public:
							cPBuffer();
							cPBuffer( char *initBuffer, int len );
	virtual					~cPBuffer();
							cPBuffer( cPBuffer *initBuffer );
	UI08& operator[] ( long int Num );
	virtual int				Length( void );
	virtual void			Resize( int newLen );
	virtual UI08 *			Pointer( void );
	virtual void			Log( FILE *toWriteTo );
};

class cPInputBuffer
{
protected:
	std::vector< UI08 > internalBuffer;
	cSocket *tSock;
public:
							cPInputBuffer();
							cPInputBuffer( cSocket *input );
	virtual void			Receive( cSocket *input ) = 0;
	UI08& operator[] ( long int num );
	virtual int				Length( void );
	virtual UI08 *			Pointer( void );
	virtual void			Log( FILE *toWriteTo );
	long					DWord( int offset );
	SI32					Word( int offset );
	UI08					Byte( int offset );
	virtual bool			Handle( void );
	virtual void			SetSocket( cSocket *toSet );
};

typedef std::vector< cSocket * >	SOCKLIST;

class cNetworkStuff
{
public:
				cNetworkStuff();
	virtual		~cNetworkStuff();
	void		Disconnect( UOXSOCKET s );
	void		ClearBuffers( void );
	void		CheckConn( void );
	void		CheckMessage( void );
	void		CheckLoginMessage( void );
	void		SockClose( void );
	void		FlushBuffer( UOXSOCKET s );
	void		setLastOn( cSocket *s );
	cSocket&	GetSocket( UOXSOCKET s );
	cSocket *	GetSockPtr( UOXSOCKET s );
	UOXSOCKET	FindNetworkPtr( cSocket *toFind );

	cSocket *	FirstSocket( void );
	cSocket *	NextSocket( void );
	cSocket *	PrevSocket( void );
	cSocket *	LastSocket( void );
	bool		FinishedSockets( void );
	void		LoadFirewallEntries( void );

	bool		kr, faul; // needed because global varaibles cant be changes in constructores

	void		CheckConnections( void );
	void		CheckMessages( void );

	void		On( void ) { InternalControl.On(); }
	void		Off( void ) { InternalControl.Off(); }
	UOXSOCKET	Transfer( UOXSOCKET s );
	void		pSplit( char *pass0 );

	SI32		PeakConnectionCount( void ) const;

	void		PushConn( void );
	void		PopConn( void );

	void		PushLogg( void );
	void		PopLogg( void );

	void		PushXGM( void );
	void		PopXGM( void );
	
	// Login Specific
	void		Login1(cSocket *s);
	void		Login2(cSocket *s, ACCOUNTSBLOCK& actbAccount);
	void		LoginDisconnect(UOXSOCKET s);
	void		LoginDisconnect(cSocket *s);
	void		LogOut(UOXSOCKET s);
	void		GoodAuth(cSocket *s, ACCOUNTSBLOCK& actbBlock);
	//
	
	void		GetMsg( UOXSOCKET s );
	void		sockInit( void );
	void		GetLoginMsg( UOXSOCKET s );
	UOXSOCKET	FindLoginPtr( cSocket *s );

	void		StartupXGM( int nPortArg );
	void		CheckXGM( void );
	void		ShutdownXGM( void );
	void		XGMDisconnect( UOXSOCKET s );
	void		XGMDisconnect( cSocket *s );
	UOXSOCKET	FindXGMPtr( cSocket *s );
	void		GetXGMMsg( UOXSOCKET s );
	void		CheckXGMConn( void );
	bool		IsFirewallBlocked( UI08 part[4] );

	std::vector< FirewallEntry >	slEntries;
	int						a_socket, xgmSocket;
	SOCKLIST				connClients, loggedInClients, xgmClients;

	bool					xgmRunning;
	struct sockaddr_in		client_addr;

	ThreadSafeObject		InternalControl;
	SI32					peakConnectionCount;

	std::vector< SI32 >			connIteratorBackup, loggIteratorBackup, xgmIteratorBackup;
	SI32					currConnIter, currLoggIter, currXGMIter;
};

#endif

