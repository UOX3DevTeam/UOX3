#ifndef __CSOCKET_H__
#define __CSOCKET_H__

class cSocket
{
public:
	// Account Related Member(s)
	UI16			wAccountID;
	ACCOUNTSBLOCK actbAccount;
	ACCOUNTSBLOCK &GetAccount(void);
	void SetAccount(ACCOUNTSBLOCK &actbBlock);
	void SetAccount(SI16 wNewAccountID);
	void SetAccount(std::string sUsername);
	//

	void			InternalReset( void );

	CChar *			currCharObj;
	CHARACTER		currchar;
	SI32			idleTimeout;
	bool			wasIdleWarned;
	SI32			tempint;
	//make_st			itemmake;
	char			dyeall;

	UI08			buffer[MAXBUFFER];
	UI08			outbuffer[MAXBUFFER];
	UI08			tbuffer[MAXBUFFER];
	char			xtext[65];

	SI32			addx[2];
	SI32			addy[2];
	SI08			addz;
	UI08			addid[4];
	SI08			addid5;
	CItem*		addmitem;

	bool			newClient;
	bool			firstPacket;
	bool			cryptclient;

	int				cliSocket;		// client
	UI08			clientip[4];
	SI16			walkSequence;

	char			currentSpellType;

	int				outlength;
	int				inlength;

	bool			logging;

	long			bytesReceived;
	long			bytesSent;

	bool			targetok;

	char			postAcked[MAXPOSTS][5];
	int				postCount;
	int				postAckCount;

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

	int				Pack( void *pvIn, void *pvOut, int len );

	UnicodeTypes	lang;
	UI32			clientVersion;
	ClientTypes		cliType;
	UI08			range;

public:
					cSocket();
					cSocket( int sockNum );
	virtual			~cSocket();

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
	// Accessors

	char *			AuthorBuffer( void );
	char *			TitleBuffer( void );
	char *			PageBuffer( void );

	SI16			PickupX( void ) const;
	SI16			PickupY( void ) const;
	SI08			PickupZ( void ) const;
	PickupLocations	PickupSpot( void ) const;
	SERIAL			PickupSerial( void ) const;
	CHARACTER		Currchar( void ) const;
	bool			FirstPacket( void ) const;
	SI32			IdleTimeout( void ) const;
	bool			WasIdleWarned( void ) const;
	UI08 *			Buffer( void );
	UI08 *			OutBuffer( void );
	UI08 *			TBuffer( void );
	SI16			WalkSequence( void ) const;
	UI16			AcctNo( void ) const;
	SI32			TempInt( void ) const;
	SI08			AddID5( void ) const;
	//make_st&		ItemMake( void );
	UI08			AddID1( void ) const;
	UI08			AddID2( void ) const;
	UI08			AddID3( void ) const;
	UI08			AddID4( void ) const;
	char			DyeAll( void ) const;
	SI32			AddX( void ) const;
	SI32			AddY( void ) const;
	SI32			AddX2( void ) const;
	SI32			AddY2( void ) const;
	SI08			AddZ( void ) const;
	CItem*		AddMItem( void ) const;
	char *			XText( void );
	bool			CryptClient( void ) const;
	int				CliSocket( void ) const;
	char			CurrentSpellType( void ) const;
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
	SI32			AddID( void ) const;

	char			PostAcked( int x, int y ) const;
	int				PostCount( void ) const;
	int				PostAckCount( void ) const;

	SI16			ClickX( void ) const;
	SI16			ClickY( void ) const;
	UI08			Range( void ) const;

	void			CloseSocket( void );

	// Mutators

	void			PickupLocation( SI16 x, SI16 y, SI08 z );
	void			PickupX( SI16 x );
	void			PickupY( SI16 y );
	void			PickupZ( SI08 z );
	void			PickupSpot( PickupLocations newValue );
	void			PickupSerial( SERIAL pickupSerial );
	void			AddID5( SI08 newValue );
	void			Currchar( CHARACTER newChar );
	void			FirstPacket( bool newValue );
	void			IdleTimeout( SI32 newValue );
	void			WasIdleWarned( bool value );
	void			WalkSequence( SI16 newValue );
	void			AcctNo( UI16 newValue );
	void			TempInt( SI32 newValue );
	//void			ItemMake( make_st& newValue );
	void			AddID1( UI08 newValue );
	void			AddID2( UI08 newValue );
	void			AddID3( UI08 newValue );
	void			AddID4( UI08 newValue );
	void			DyeAll( char newValue );
	void			AddX( SI32 newValue );
	void			AddY( SI32 newValue );
	void			AddX2( SI32 newValue );
	void			AddY2( SI32 newValue );
	void			AddZ( SI08 newValue );
	void			AddMItem( CItem *newValue );
	void			CryptClient( bool newValue );
	void			CliSocket( int newValue );
	void			CurrentSpellType( char newValue );
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

	bool			FlushBuffer( bool doLog = true );
	void			Send( const void *point, int length );
	int				Receive( int x );

	SI32			GetDWord( int offset );
	SI16			GetWord( int offset );
	UI08			GetByte( int offset );

	void			SetDWord( int offset, SI32 newValue );
	void			SetWord( int offset, SI32 newValue );
	void			SetByte( int offset, UI08 newValue );
	void			ClientIP( long newValue );
	void			TargetOK( bool newValue );
	void			AddID( long newValue );
	void			ClickX( SI16 newValue );
	void			ClickY( SI16 newValue );
	void			PostAcked( int x, int y, char newValue );
	void			PostCount( int newValue );
	void			PostAckCount( int newValue );
	void			XText( const char *newValue );

	void			AuthorBuffer( const char *newValue );
	void			PageBuffer( const char *newValue );
	void			TitleBuffer( const char *newValue );

	void			Send( cPBuffer *toSend );

	UnicodeTypes	Language( void ) const;
	void			Language( UnicodeTypes newVal );
	
};

#endif

