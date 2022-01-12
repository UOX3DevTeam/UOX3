#ifndef __CNETWORK_H__
#define __CNETWORK_H__

#include <mutex>
#if PLATFORM != WINDOWS
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <winsock2.h>
#undef min
#undef max
#endif
class socket_error : public std::runtime_error
{
private:
	UI32		errorNum;
public:
	socket_error( const std::string& what_arg );
	socket_error( const UI32 errorNumber );
	socket_error( void );
	UI32		ErrorNumber( void ) const;
	const char *what( void ) const throw();
};

class CPacketStream
{
private:
	std::vector< UI08 >		internalBuffer;

public:
	CPacketStream()
	{
		internalBuffer.resize( 0 );
	}
	~CPacketStream()
	{
		internalBuffer.clear();
	}

	void	ReserveSize( size_t len )
	{
		internalBuffer.resize( len );
	}
	void	WriteByte( size_t pos, UI08 toWrite )
	{
		if( pos >= internalBuffer.size() )
			internalBuffer.resize( pos+1 );

		internalBuffer[pos] = toWrite;
	}
	void	WriteShort( size_t pos, SI32 toWrite )
	{
		if( (pos+1) >= internalBuffer.size() )
			internalBuffer.resize( pos+2 );

		internalBuffer[pos+0] = static_cast<UI08>((toWrite&0xFF00)>>8);
		internalBuffer[pos+1] = static_cast<UI08>((toWrite&0x00FF)%256);
	}
	void	WriteLong( size_t pos, UI32 toWrite )
	{
		if( (pos+3) >= internalBuffer.size() )
			internalBuffer.resize( pos+4 );

		internalBuffer[pos+0] = static_cast<UI08>((toWrite&0xFF000000)>>24);
		internalBuffer[pos+1] = static_cast<UI08>((toWrite&0x00FF0000)>>16);
		internalBuffer[pos+2] = static_cast<UI08>((toWrite&0x0000FF00)>>8);
		internalBuffer[pos+3] = static_cast<UI08>((toWrite&0x000000FF)%256);
	}
	void	WriteString( size_t pos, const std::string& toWrite, size_t len )
	{
		if(( len == 0 ) || toWrite.empty() )
		{
			return;
		}
		if( len > toWrite.size() )
		{
			len = toWrite.size()+1;
		}

		if( (pos+len) >= internalBuffer.size() )
			internalBuffer.resize( pos+len );

		strncpy( (char *)&internalBuffer[pos], toWrite.c_str(), len );
	}
	void	WriteArray( size_t pos, const UI08 *toWrite, size_t len )
	{
		if( (pos+len) >= internalBuffer.size() )
			internalBuffer.resize( pos+len );

		memcpy( &internalBuffer[pos], toWrite, len );
	}
	UI08	GetByte( size_t pos ) const
	{
		UI08 retVal = 0;
		if( pos < internalBuffer.size() )
			retVal = internalBuffer[pos];
		return retVal;
	}
	SI16	GetShort( size_t pos ) const
	{
		SI16 retVal = 0;
		if( pos < internalBuffer.size()+1 )
			retVal = static_cast<SI16>((internalBuffer[pos+0]<<8) + internalBuffer[pos+1]);
		return retVal;
	}
	UI16	GetUShort( size_t pos ) const
	{
		return static_cast<UI16>(GetShort( pos ));
	}
	SI32	GetLong( size_t pos ) const
	{
		SI32 retVal = 0;
		if( pos < internalBuffer.size()+3 )
			retVal = (internalBuffer[pos+0]<<24) + (internalBuffer[pos+1]<<16) + (internalBuffer[pos+2]<<8) + internalBuffer[pos+3];
		return retVal;
	}
	UI32	GetULong( size_t pos ) const
	{
		return static_cast<UI32>(GetLong( pos ));
	}

	const UI08 *GetBuffer( void ) const
	{
		return (const UI08 *)&internalBuffer[0];
	}

	size_t GetSize( void ) const
	{
		return internalBuffer.size();
	}
};

class CPUOXBuffer
{
private:
	std::vector< UI08 >		packedBuffer;
	bool					isPacked;
	UI32					packedLength;

protected:
	CPacketStream			pStream;

	virtual void			InternalReset( void );

public:
	CPUOXBuffer();
	virtual					~CPUOXBuffer();
	CPUOXBuffer( CPUOXBuffer *initBuffer );
	CPUOXBuffer &operator=( CPUOXBuffer &copyFrom );

	UI32					Pack( void );
	virtual bool			ClientCanReceive( CSocket *mSock );
	CPacketStream&			GetPacketStream( void );

	UI32					PackedLength( void ) const;
	const UI08 *			PackedPointer( void ) const;

	virtual void			Log( std::ofstream &outStream, bool fullHeader = true );
};

class CPInputBuffer
{
protected:
	CSocket *				tSock;

public:
	CPInputBuffer();
	CPInputBuffer( CSocket *input );
	virtual					~CPInputBuffer()
	{
	}

	virtual void			Receive( void ) = 0;
	virtual void			Log( std::ofstream &outStream, bool fullHeader = true );
	virtual bool			Handle( void );
	void					SetSocket( CSocket *toSet );
	CSocket *				GetSocket( void ) const;
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

	void		Transfer( CSocket *s );

	size_t		PeakConnectionCount( void ) const;

	void		PushConn( void );
	void		PopConn( void );
	void 		pushConn(void) ;
	void 		popConn(void) ;

	void		pushLogg() ;
	void 		popLogg() ;

	void		PushLogg( void );
	void		PopLogg( void );

	// Login Specific
	void		LoginDisconnect(UOXSOCKET s);
	void		LoginDisconnect(CSocket *s);

	void		RegisterPacket( UI08 packet, UI08 subCmd, UI16 scriptID );

	// We don't want to do this, but given we have outside classes
	// we can either friend a lot of things, or just put it out here
	std::mutex				internallock;

private:
	struct FirewallEntry
	{
		SI16 b[4];
		FirewallEntry( SI16 p1, SI16 p2, SI16 p3, SI16 p4 )
		{
			b[0] = p1; b[1] = p2; b[2] = p3; b[3] = p4;
		}
	};

	std::map< UI16, UI16 >			packetOverloads;
	std::vector< FirewallEntry >	slEntries;
	SI32					a_socket;
	SOCKLIST				connClients, loggedInClients;

	struct sockaddr_in		client_addr;

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

#endif

