#ifndef __CNETWORK_H__
#define __CNETWORK_H__

#include "threadsafeobject.h"

namespace UOX
{

#define __LOGIN_THREAD__

class socket_error : public std::runtime_error
{
private:
	long		errorNum;
public:
				socket_error( const std::string& what_arg );
				socket_error( const long errorNumber );
				socket_error( void );
	long		ErrorNumber( void ) const;
	const char *what( void ) const throw();
};

class cPBaseBuffer
{
protected:
	std::vector< UI08 >		internalBuffer;
	std::vector< UI08 >		packedBuffer;
	bool					isPacked;
	UI32					packedLength;
	void					InternalReset( void );
	size_t					internalBufferOffset;		// this is to track offsets for operator overloading
public:
							cPBaseBuffer();
							cPBaseBuffer( char *initBuffer, size_t len );
	virtual					~cPBaseBuffer();
							cPBaseBuffer( cPBaseBuffer *initBuffer );
	UI08& operator[] ( size_t Num );
	virtual size_t			Length( void ) const;
	virtual UI32			PackedLength( void ) const;
	virtual void			Resize( size_t newLen );
	virtual const UI08 *	Pointer( void ) const;
	virtual const UI08 *	PackedPointer( void ) const;
	virtual void			Log( std::ofstream &outStream, bool fullHeader = true );
	virtual UI32			Pack( void ) = 0;
	cPBaseBuffer&			operator<<( const UI08 value );
	cPBaseBuffer&			operator<<( const UI16 value );
	cPBaseBuffer&			operator<<( const UI32 value );
	cPBaseBuffer&			operator<<( const SI08 value );
	cPBaseBuffer&			operator<<( const SI16 value );
	cPBaseBuffer&			operator<<( const SI32 value );
	cPBaseBuffer&			operator<<( const std::string& value );
};

class cPUOXBuffer : public cPBaseBuffer
{
public:
							cPUOXBuffer();
							cPUOXBuffer( char *initBuffer, size_t len );
	virtual					~cPUOXBuffer();
							cPUOXBuffer( cPBaseBuffer *initBuffer );
	virtual UI32			Pack( void );
	virtual bool			ClientCanReceive( CSocket *mSock );
};

class cPInputBuffer
{
protected:
	std::vector< UI08 > internalBuffer;
	CSocket *			tSock;
public:
							cPInputBuffer();
							cPInputBuffer( CSocket *input );
	virtual void			Receive( void ) = 0;
	UI08& operator[] ( size_t num );
	virtual size_t			Length( void );
	virtual UI08 *			Pointer( void );
	virtual void			Log( std::ofstream &outStream, bool fullHeader = true );
	long					DWord( size_t offset );
	SI32					Word( size_t offset );
	UI08					Byte( size_t offset );
	virtual bool			Handle( void );
	virtual void			SetSocket( CSocket *toSet );
	CSocket *				GetSocket( void ) const;
	virtual ~cPInputBuffer()
	{
	}
};

class cNetworkStuff
{
public:
				cNetworkStuff();
				~cNetworkStuff();
	void		Disconnect( UOXSOCKET s );
	void		Disconnect( CSocket *s );
	void		ClearBuffers( void );
	void		CheckLoginMessage( void );
	void		CheckMessage( void );
	void		SockClose( void );
	void		setLastOn( CSocket *s );
	CSocket *	GetSockPtr( UOXSOCKET s );
	UOXSOCKET	FindNetworkPtr( CSocket *toFind );

	CSocket *	FirstSocket( void );
	CSocket *	NextSocket( void );
	CSocket *	PrevSocket( void );
	CSocket *	LastSocket( void );
	bool		FinishedSockets( void );

	void		CheckConnections( void );
	void		CheckMessages( void );

	void		On( void )
	{
		InternalControl.On();
	}
	void		Off( void )
	{
		InternalControl.Off();
	}
	void		Transfer( CSocket *s );

	size_t		PeakConnectionCount( void ) const;

	void		PushConn( void );
	void		PopConn( void );

	void		PushLogg( void );
	void		PopLogg( void );
	
	// Login Specific
	void		LoginDisconnect(UOXSOCKET s);
	void		LoginDisconnect(CSocket *s);
	
private:
	struct FirewallEntry
	{
		SI16 b[4];
		FirewallEntry( SI16 p1, SI16 p2, SI16 p3, SI16 p4 )
		{
			b[0] = p1; b[1] = p2; b[2] = p3; b[3] = p4; 
		}
	};

	std::vector< FirewallEntry >	slEntries;
	int						a_socket;
	SOCKLIST				connClients, loggedInClients;

	struct sockaddr_in		client_addr;

	ThreadSafeObject		InternalControl;
	size_t					peakConnectionCount;

	std::vector< SOCKLIST_ITERATOR >	connIteratorBackup, loggIteratorBackup;
	SOCKLIST_ITERATOR					currConnIter, currLoggIter;

	void		LoadFirewallEntries( void );
	void		GetMsg( UOXSOCKET s );
	void		sockInit( void );
	void		GetLoginMsg( UOXSOCKET s );
	UOXSOCKET	FindLoginPtr( CSocket *s );

	void		CheckConn( void );
	void		LogOut( CSocket *s );

	bool		IsFirewallBlocked( UI08 part[4] );

};

extern cNetworkStuff *Network;

}

#endif

