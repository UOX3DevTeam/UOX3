#ifndef __CSOCKET_H__
#define __CSOCKET_H__
#include "IP4Address.hpp"
enum ClientTypes
{
	CV_DEFAULT = 0,
	CV_UNKNOWN,
	CV_KRRIOS,
	CV_T2A,		// From 4.0.0p to 4.0.11c
	CV_UO3D,	// Third Dawn 3D client
	CV_ML,		// From 4.0.11f to 5.0.9.1
	CV_KR2D,	// From 6.0.0.0+ to 6.0.14.1, first packet sent is 0xEF.
	CV_KR3D,
	CV_SA2D,	// From 6.0.14.2 to 7.0.8.2, Stygian Abyss expansion client. First patcket sent is 0xEF, requires 0xB9 size-change from 3 to 5, new 0xF3 packet replacex 0x1A
	CV_SA3D,
	CV_HS2D,	// From 7.0.9.0 to infinity (so far), High Seas expansion client
	CV_HS3D,
	CV_COUNT
};

// Only client-versions with major packet changes included. Clients between two versions "belong" to the nearest previous version
enum ClientVersions
{
	CVS_DEFAULT = 0,
	CVS_400,
	CVS_407a,
	CVS_4011c,
	CVS_500a,
	CVS_502a,
	CVS_5082,
	CVS_6000,
	CVS_6017,
	CVS_6050,
	CVS_25302, // UOKR3D 2.53.0.2
	CVS_60142,
	CVS_7000,
	CVS_7090,
	CVS_70130,
	CVS_70151,
	CVS_70160,
	CVS_70240,
	CVS_70300,
	CVS_70331,
	CVS_704565,
	CVS_705527, // Max update range increase from 18 to 24
	CVS_70610,
	CVS_COUNT
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
	CAccountBlock&	GetAccount( void );
	void			SetAccount( CAccountBlock& actbBlock );
	//

	void			InternalReset( void );
	IP4Address		ipaddress;

private:
	std::vector< UI16 >				trigWords;
	std::vector< UI16 >::iterator	twIter;
	UI16			accountNum;

	CChar *			currCharObj;
	SI32			idleTimeout;
	bool			wasIdleWarned;
	SI32			negotiateTimeout;
	bool			negotiatedWithAssistant;

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
	CBaseObject *	tmpObj2;
	SI08			clickz;
	UI08			addid[4];
	SI32			tempint;
	SI32			tempint2;
	UI08			dyeall;

	bool			newClient;
	bool			firstPacket;
	bool			cryptclient;
	bool			forceOffline;

	size_t			cliSocket;		// client
	UI08			clientip[4];
	SI16			walkSequence;

	UI08			currentSpellType;

	SI32			outlength;
	SI32			inlength;

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
	CItem *			cursorItem; //pointer to item held on mouse cursor


	UI32			Pack( void *pvIn, void *pvOut, SI32 len );

	UnicodeTypes	lang;
	UI32			clientVersion;
	ClientTypes		cliType;
	ClientVersions	cliVerShort;
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

	ClientVersions	ClientVerShort( void ) const;
	void			ClientVerShort( ClientVersions newVer );

	ClientTypes		ClientType( void ) const;
	void			ClientType( ClientTypes newVer );

	void			ClearPage( void );
	void			AddTrigWord( UI16 );
	// Accessors

	bool			LoginComplete( void ) const;

	CItem *			GetCursorItem( void ) const;
	void			SetCursorItem( CItem *newValue );
	SI16			PickupX( void ) const;
	SI16			PickupY( void ) const;
	SI08			PickupZ( void ) const;
	PickupLocations	PickupSpot( void ) const;
	SERIAL			PickupSerial( void ) const;
	bool			FirstPacket( void ) const;
	SI32			IdleTimeout( void ) const;
	bool			WasIdleWarned( void ) const;
	SI32			NegotiateTimeout( void ) const;
	bool			NegotiatedWithAssistant( void ) const;
	UI08 *			Buffer( void );
	UI08 *			OutBuffer( void );
	SI16			WalkSequence( void ) const;
	UI16			AcctNo( void ) const;
	std::string		XText( void );
	bool			CryptClient( void ) const;
	size_t			CliSocket( void ) const;
	UI08			CurrentSpellType( void ) const;
	SI32			OutLength( void ) const;
	SI32			InLength( void ) const;
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
	bool			ForceOffline( void ) const;

	// Temporary Variables
	CBaseObject *	TempObj( void ) const;
	CBaseObject *	TempObj2( void ) const;
	SI32			TempInt( void ) const;
	SI32			TempInt2( void ) const;
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
	void			NegotiateTimeout( SI32 newValue );
	void			NegotiatedWithAssistant( bool value );
	void			WalkSequence( SI16 newValue );
	void			AcctNo( UI16 newValue );
	void			CryptClient( bool newValue );
	void			CliSocket( size_t newValue );
	void			CurrentSpellType( UI08 newValue );
	void			OutLength( SI32 newValue );
	void			InLength( SI32 newValue );
	void			Logging( bool newValue );
	void			CurrcharObj( CChar *newValue );
	void			ClientIP1( UI08 );
	void			ClientIP2( UI08 newValue );
	void			ClientIP3( UI08 newValue );
	void			ClientIP4( UI08 newValue );
	void			NewClient( bool newValue );
	void			Range( UI08 value );
	void			ForceOffline( bool newValue );

	//	Temporary Variables

	// Under protest I add, NEVER NEVER Do this
	cScript 		*scriptForCallBack ;
	// Get rid of above as soon as possible, horible.

	void			TempObj( CBaseObject *newValue );
	void			TempObj2( CBaseObject *newValue );
	void			TempInt( SI32 newValue );
	void			TempInt2( SI32 newValue );
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
	void			Send( const void *point, SI32 length );
	SI32			Receive( SI32 x, bool doLog = true );
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
	void			XText( const std::string &newValue );

	void			Send( CPUOXBuffer *toSend );

	UnicodeTypes	Language( void ) const;
	void			Language( UnicodeTypes newVal );

	void			sysmessage( const std::string txt, ... );
	void			sysmessageJS( const std::string& uformat,const std::string& data );
	void			sysmessage( SI32 dictEntry, ... );
	void			objMessage( const std::string& txt, CBaseObject *getObj, R32 secsFromNow = 0.0f, UI16 Color = 0x03B2 );
	void			objMessage( SI32 dictEntry, CBaseObject *getObj, R32 secsFromNow = 0.0f, UI32 Color = 0x03B2, ... );

	void			ShowCharName( CChar *i, bool showSer  );

	void			target( UI08 targType, UI08 targID, const std::string& txt );
	void			target( UI08 targType, UI08 targID,  SI32 dictEntry, ... );
	void			mtarget( UI16 itemID, SI32 dictEntry );

	void			statwindow( CBaseObject *i );
	void			updateskill( UI08 skillnum );
	void			openPack( CItem *i, bool isPlayerVendor = false );
	void			openBank( CChar *i );
	void			OpenURL( const std::string& txt );

	bool			ReceivedVersion( void ) const;
	void			ReceivedVersion( bool value );

	UI32			BytesSent( void ) const;
	UI32			BytesReceived( void ) const;

	TIMERVAL		GetTimer( cS_TID timerID ) const;
	void			SetTimer( cS_TID timerID, TIMERVAL value );
	void			ClearTimers( void );

private:

	COLOUR			GetFlagColour( CChar *src, CChar *trg );

};
#endif

