#ifndef __CSOCKET_H__
#define __CSOCKET_H__

namespace UOX
{

enum ClientTypes
{
	CV_NORMAL = 0,
	CV_T2A,
	CV_UO3D,
	CV_KRRIOS,
	CV_UNKNOWN,
	CV_COUNT
};

enum cS_TID
{
	// PC Timers
	tPC_SKILLDELAY = 0,
	tPC_OBJDELAY,
	tPC_SPIRITSPEAK,
	tPC_TRACKING,
	tPC_FISHING,
	tPC_MUTETIME,
	tPC_TRACKINGDISPLAY,
	tPC_COUNT
};

class CSocket
{
public:
	// Account Related Member(s)
	ACCOUNTSBLOCK&	GetAccount( void );
	void			SetAccount( ACCOUNTSBLOCK& actbBlock );
	//

	void			InternalReset( void );

private:
	std::vector< UI16 >				trigWords;
	std::vector< UI16 >::iterator	twIter;
	UI16			accountNum;

	CChar *			currCharObj;
	SI32			idleTimeout;
	bool			wasIdleWarned;

	UI08			buffer[MAXBUFFER];
	UI08			outbuffer[MAXBUFFER];
	// These vectors are for dealing with packets that are larger than the buffer size
	// While admittedly not thread friendly, the number of times these buffers are used
	// should be very small and right now, is an implementation that will increase clieht
	// compatability
	std::vector< UI08 >	largeBuffer;
	std::vector< UI08 >	largePackBuffer;

	std::string		xtext;

	//	Temporary variables (For targeting commands, etc)
	CBaseObject *	tmpObj;
	SI08			clickz;
	UI08			addid[4];
	SI32			tempint;
	UI08			dyeall;

	bool			newClient;
	bool			firstPacket;
	bool			cryptclient;

	size_t			cliSocket;		// client
	UI08			clientip[4];
	SI16			walkSequence;

	UI08			currentSpellType;

	int				outlength;
	int				inlength;

	bool			logging;

	UI32			bytesReceived;
	UI32			bytesSent;

	bool			targetok;

	std::vector< SERIAL >				postAcked;
	std::vector< SERIAL >::iterator		ackIter;
	size_t			postAckCount;

	SI16			clickx;
	SI16			clicky;

	PickupLocations	pSpot;
	SERIAL			pFrom;
	SI16			pX;
	SI16			pY;
	SI08			pZ;

	char			authorbuffer[32]; 
	char			titlebuffer[62];
	char			pagebuffer[512]; // 256 may be enough but 512 is for safety

	UI32			Pack( void *pvIn, void *pvOut, int len );

	UnicodeTypes	lang;
	UI32			clientVersion;
	ClientTypes		cliType;
	UI08			range;

	bool			receivedVersion;
	bool			loginComplete;

	// Timer Vals moved here from CChar due to their inherently temporary nature and to reduce wasted memory
	TIMERVAL		pcTimers[tPC_COUNT];
public:
					CSocket( size_t sockNum );
					~CSocket();

	UI32			ClientVersion( void ) const;
	void			ClientVersion( UI32 newVer );
	void			ClientVersion( UI08 major, UI08 minor, UI08 sub, UI08 letter );

	void			ClientVersionMajor(		UI08 value );
	void			ClientVersionMinor(		UI08 value );
	void			ClientVersionSub(		UI08 value );
	void			ClientVersionLetter(	UI08 value );

	UI08			ClientVersionMajor( void ) const;
	UI08			ClientVersionMinor( void ) const;
	UI08			ClientVersionSub( void ) const;
	UI08			ClientVersionLetter( void ) const;

	ClientTypes		ClientType( void ) const;
	void			ClientType( ClientTypes newVer );

	void			ClearAuthor( void );
	void			ClearTitle( void );
	void			ClearPage( void );
	void			AddTrigWord( UI16 );
	// Accessors

	bool			LoginComplete( void ) const;

	const char *	AuthorBuffer( void ) const;
	const char *	TitleBuffer( void ) const;
	const char *	PageBuffer( void ) const;

	SI16			PickupX( void ) const;
	SI16			PickupY( void ) const;
	SI08			PickupZ( void ) const;
	PickupLocations	PickupSpot( void ) const;
	SERIAL			PickupSerial( void ) const;
	bool			FirstPacket( void ) const;
	SI32			IdleTimeout( void ) const;
	bool			WasIdleWarned( void ) const;
	UI08 *			Buffer( void );
	UI08 *			OutBuffer( void );
	SI16			WalkSequence( void ) const;
	UI16			AcctNo( void ) const;
	std::string		XText( void );
	bool			CryptClient( void ) const;
	size_t			CliSocket( void ) const;
	UI08			CurrentSpellType( void ) const;
	int				OutLength( void ) const;
	int				InLength( void ) const;
	bool			Logging( void ) const;
	CChar *			CurrcharObj( void ) const;
	UI08			ClientIP1( void ) const;
	UI08			ClientIP2( void ) const;
	UI08			ClientIP3( void ) const;
	UI08			ClientIP4( void ) const;
	bool			NewClient( void ) const;
	bool			TargetOK( void ) const;
	UI16			FirstTrigWord( void );
	UI16			NextTrigWord( void );
	bool			FinishedTrigWords( void );
	void			ClearTrigWords( void );

	// Temporary Variables
	CBaseObject *	TempObj( void ) const;
	SI32			TempInt( void ) const;
	UI32			AddID( void ) const;
	UI08			AddID1( void ) const;
	UI08			AddID2( void ) const;
	UI08			AddID3( void ) const;
	UI08			AddID4( void ) const;
	UI08			DyeAll( void ) const;
	SI08			ClickZ( void ) const;

	SERIAL			FirstPostAck( void );
	SERIAL			NextPostAck( void );
	bool			FinishedPostAck( void );
	SERIAL			RemovePostAck( void );

	size_t			PostCount( void ) const;
	size_t			PostAckCount( void ) const;

	SI16			ClickX( void ) const;
	SI16			ClickY( void ) const;
	UI08			Range( void ) const;

	void			CloseSocket( void );

	// Mutators

	void			LoginComplete( bool newVal );

	void			PickupLocation( SI16 x, SI16 y, SI08 z );
	void			PickupX( SI16 x );
	void			PickupY( SI16 y );
	void			PickupZ( SI08 z );
	void			PickupSpot( PickupLocations newValue );
	void			PickupSerial( SERIAL pickupSerial );
	void			FirstPacket( bool newValue );
	void			IdleTimeout( SI32 newValue );
	void			WasIdleWarned( bool value );
	void			WalkSequence( SI16 newValue );
	void			AcctNo( UI16 newValue );
	void			CryptClient( bool newValue );
	void			CliSocket( size_t newValue );
	void			CurrentSpellType( UI08 newValue );
	void			OutLength( int newValue );
	void			InLength( int newValue );
	void			Logging( bool newValue );
	void			CurrcharObj( CChar *newValue );
	void			ClientIP1( UI08 );
	void			ClientIP2( UI08 newValue );
	void			ClientIP3( UI08 newValue );
	void			ClientIP4( UI08 newValue );
	void			NewClient( bool newValue );
	void			Range( UI08 value );

	//	Temporary Variables
	void			TempObj( CBaseObject *newValue );
	void			TempInt( SI32 newValue );
	void			AddID( UI32 newValue );
	void			AddID1( UI08 newValue );
	void			AddID2( UI08 newValue );
	void			AddID3( UI08 newValue );
	void			AddID4( UI08 newValue );
	void			DyeAll( UI08 newValue );
	void			ClickZ( SI08 newValue );

	bool			FlushBuffer( bool doLog = true );
	bool			FlushLargeBuffer( bool doLog = true );
	void			FlushIncoming( void );
	void			Send( const void *point, int length );
	int				Receive( int x, bool doLog = true );
	void			ReceiveLogging( CPInputBuffer *toLog );

	UI32			GetDWord( size_t offset );
	UI16			GetWord( size_t offset );
	UI08			GetByte( size_t offset );

	void			SetDWord( size_t offset, UI32 newValue );
	void			SetWord( size_t offset, UI16 newValue );
	void			SetByte( size_t offset, UI08 newValue );
	void			ClientIP( UI32 newValue );
	void			TargetOK( bool newValue );
	void			ClickX( SI16 newValue );
	void			ClickY( SI16 newValue );
	void			PostAcked( SERIAL newValue );
	void			PostAckCount( size_t newValue );
	void			PostClear();
	void			XText( std::string newValue );

	void			AuthorBuffer( const char *newValue );
	void			PageBuffer( const char *newValue );
	void			TitleBuffer( const char *newValue );

	void			Send( CPUOXBuffer *toSend );

	UnicodeTypes	Language( void ) const;
	void			Language( UnicodeTypes newVal );

	void			sysmessage( const char *txt, ... );
	void			sysmessage( SI32 dictEntry, ... );
	void			objMessage( const char *txt, CBaseObject *getObj, R32 secsFromNow = 0.0f, UI16 Color = 0x03B2 );
	void			objMessage( SI32 dictEntry, CBaseObject *getObj, R32 secsFromNow = 0.0f, UI16 Color = 0x03B2, ... );

	void			ShowCharName( CChar *i, bool showSer  );

	void			target( UI08 targType, UI08 targID, const char *txt );
	void			target( UI08 targType, UI08 targID,  SI32 dictEntry, ... );
	void			mtarget( UI16 itemID, SI32 dictEntry );

	void			statwindow( CChar *i );
	void			updateskill( UI08 skillnum );
	void			openPack( CItem *i );
	void			openBank( CChar *i );
	void			OpenURL( const std::string txt );

	bool			ReceivedVersion( void ) const;
	void			ReceivedVersion( bool value );

	UI32			BytesSent( void ) const;
	UI32			BytesReceived( void ) const;

	TIMERVAL		GetTimer( cS_TID timerID ) const;
	void			SetTimer( cS_TID timerID, TIMERVAL value );
	void			ClearTimers( void );

private:

	UI16			GetFlagColour( CChar *src, CChar *trg );
	
};

}

#endif

