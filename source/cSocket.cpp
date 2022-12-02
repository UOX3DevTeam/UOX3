#include <algorithm>
#include "uox3.h"
#include "CPacketSend.h"
#include "speech.h"
#include "cRaces.h"
#include "cGuild.h"
#include "PartySystem.h"
#include "commands.h"
#include "combat.h"
#include "classes.h"
#include "Dictionary.h"
#include "CJSEngine.h"
#include "StringUtility.hpp"
#include "typedefs.h"

#if PLATFORM != WINDOWS
#include <sys/ioctl.h>
#endif


//	1.0		29th November, 2000
//			Initial implementation
//			Stores almost all information currently separated into different vars
//			Also has logging support, and non-blocking IO support
//			Makes use of a socket_error exception class

#if defined( UOX_DEBUG_MODE )
const bool LOGDEFAULT = true;
#else
const bool LOGDEFAULT = false;
#endif

//o------------------------------------------------------------------------------------------------o
//|	Function	-	DumpStream()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Dumps packet stream to log file
//o------------------------------------------------------------------------------------------------o
void DumpStream( std::ofstream &outStream, const char *strToDump, UI08 num )
{
	outStream << "  ";
	for( UI08 parseBuff = 0; parseBuff < num; ++parseBuff )
	{
		if( strToDump[parseBuff] && strToDump[parseBuff] != 0x0A && strToDump[parseBuff] != 0x0D && strToDump[parseBuff] != 0x09 )
		{
			outStream << strToDump[parseBuff];
		}
		else
		{
			outStream << ".";
		}
	}
	outStream << '\n';
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	DoPacketLogging()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Log packet stream in large buffer
//o------------------------------------------------------------------------------------------------o
void DoPacketLogging( std::ofstream &outStream, size_t buffLen, std::vector<UI08>& myBuffer )
{
	outStream << std::hex;
	char qbuffer[8];
	memset( qbuffer, 0x00, 8 );
	UI08 j = 0;
	for( size_t i = 0; i < buffLen; ++i )
	{
		qbuffer[j++] = myBuffer[i];
		outStream << " " << (myBuffer[i] < 16?"0":"") << static_cast<UI16>( myBuffer[i] );
		if( j > 7 )
		{
			DumpStream( outStream, qbuffer, 8 );
			j = 0;
		}
	}
	if( j < 8 )
	{
		for( UI08 k = j; k < 8; ++k )
		{
			outStream << " --";
		}
		DumpStream( outStream, qbuffer, j );
	}
	outStream << std::endl << std::endl;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	DoPacketLogging()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Log packet stream in regular buffer
//o------------------------------------------------------------------------------------------------o
void DoPacketLogging( std::ofstream &outStream, size_t buffLen, const UI08 *myBuffer )
{
	outStream << std::hex;
	char qbuffer[8];
	memset( qbuffer, 0x00, 8 );
	UI08 j = 0;
	for( size_t i = 0; i < buffLen; ++i )
	{
		qbuffer[j++] = myBuffer[i];
		outStream << " " << (myBuffer[i] < 16?"0":"") << static_cast<UI16>( myBuffer[i] );
		if( j > 7 )
		{
			DumpStream( outStream, qbuffer, 8 );
			j = 0;
		}
	}
	if( j < 8 )
	{
		for( UI08 k = j; k < 8; ++k )
		{
			outStream << " --";
		}
		DumpStream( outStream, qbuffer, j );
	}
	outStream << std::endl << std::endl;
}

UI32 socket_error::ErrorNumber( void ) const
{
	return errorNum;
}

const char *socket_error::what( void ) const throw()
{
	return runtime_error::what();
}

socket_error::socket_error( void ) : runtime_error( "" ), errorNum( -1 )
{
}

socket_error::socket_error( const std::string& what_arg ) : runtime_error( what_arg ), errorNum( -1 )
{
}

socket_error::socket_error( const UI32 errorNumber ) : runtime_error( "" ), errorNum( errorNumber )
{
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::CliSocket()
//|	Date		-	November 29th, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets socket identifier for our socket
//o------------------------------------------------------------------------------------------------o
size_t CSocket::CliSocket( void ) const
{
	return cliSocket;
}
void CSocket::CliSocket( size_t newValue )
{
	cliSocket = newValue;
	unsigned long mode = 1;
	// set the socket to nonblocking
	ioctlsocket( static_cast<UOXSOCKET>( cliSocket ), FIONBIO, &mode );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::CryptClient()
//|	Date		-	November 29th, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether socket is set to crypt mode
//o------------------------------------------------------------------------------------------------o
bool CSocket::CryptClient( void ) const
{
	return cryptclient;
}
void CSocket::CryptClient( bool newValue )
{
	cryptclient = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::XText()
//|	Date		-	November 29th, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the socket's xtext buffer
//o------------------------------------------------------------------------------------------------o
std::string CSocket::XText( void )
{
	return xtext;
}
void CSocket::XText( const std::string &newValue )
{
	xtext = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::TempObj()
//|	Date		-	October 31, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets temporary storage for CChar and CItem objects
//o------------------------------------------------------------------------------------------------o
CBaseObject *CSocket::TempObj( void ) const
{
	return tmpObj;
}
void CSocket::TempObj( CBaseObject *newValue )
{
	tmpObj = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::TempObj2()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets temporary storage for CChar and CItem objects
//o------------------------------------------------------------------------------------------------o
CBaseObject *CSocket::TempObj2( void ) const
{
	return tmpObj2;
}
void CSocket::TempObj2( CBaseObject *newValue )
{
	tmpObj2 = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::TempInt()
//|	Date		-	November 29th, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets tempint of the socket
//o------------------------------------------------------------------------------------------------o
SI32 CSocket::TempInt( void ) const
{
	return tempint;
}
void CSocket::TempInt( SI32 newValue )
{
	tempint = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::TempInt2()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets tempint2 of the socket
//o------------------------------------------------------------------------------------------------o
SI32 CSocket::TempInt2( void ) const
{
	return tempint2;
}
void CSocket::TempInt2( SI32 newValue )
{
	tempint2 = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::ClickZ()
//|	Date		-	November 29th, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the socket's addz
//o------------------------------------------------------------------------------------------------o
SI08 CSocket::ClickZ( void ) const
{
	return clickz;
}
void CSocket::ClickZ( SI08 newValue )
{
	clickz = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::AddId()
//|	Date		-	November 29th, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the addid associated with the socket
//o------------------------------------------------------------------------------------------------o
void CSocket::AddId( UI32 newValue )
{
	addid[0] = static_cast<UI08>( newValue >> 24 );
	addid[1] = static_cast<UI08>( newValue >> 16 );
	addid[2] = static_cast<UI08>( newValue >> 8 );
	addid[3] = static_cast<UI08>( newValue % 256 );
}
UI32 CSocket::AddId( void ) const
{
	return CalcSerial( addid[0], addid[1], addid[2], addid[3] );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::AddId1()
//|	Date		-	November 29th, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the first addid associated with the socket
//o------------------------------------------------------------------------------------------------o
UI08 CSocket::AddId1( void ) const
{
	return addid[0];
}
void CSocket::AddId1( UI08 newValue )
{
	addid[0] = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::AddId2()
//|	Date		-	November 29th, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the second addid associated with the socket
//o------------------------------------------------------------------------------------------------o
UI08 CSocket::AddId2( void ) const
{
	return addid[1];
}
void CSocket::AddId2( UI08 newValue )
{
	addid[1] = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::AddId3()
//|	Date		-	November 29th, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the third addid associated with the socket
//o------------------------------------------------------------------------------------------------o
UI08 CSocket::AddId3( void ) const
{
	return addid[2];
}
void CSocket::AddId3( UI08 newValue )
{
	addid[2] = newValue;
}
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::AddId4()
//|	Date		-	November 29th, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the fourth addid associated with the socket
//o------------------------------------------------------------------------------------------------o
UI08 CSocket::AddId4( void ) const
{
	return addid[3];
}
void CSocket::AddId4( UI08 newValue )
{
	addid[3] = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::DyeAll()
//|	Date		-	November 29th, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets DyeAll status of a socket
//o------------------------------------------------------------------------------------------------o
UI08 CSocket::DyeAll( void ) const
{
	return dyeall;
}
void CSocket::DyeAll( UI08 newValue )
{
	dyeall = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::CloseSocket()
//|	Date		-	November 29th, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Closes the open socket
//o------------------------------------------------------------------------------------------------o
void CSocket::CloseSocket( void )
{
	closesocket( static_cast<UOXSOCKET>( cliSocket ));
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::FirstPacket()
//|	Date		-	November 29th, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether the socket has received its first packet yet
//o------------------------------------------------------------------------------------------------o
bool CSocket::FirstPacket( void ) const
{
	return firstPacket;
}
void CSocket::FirstPacket( bool newValue )
{
	firstPacket = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::ForceOffline()
//|	Date		-	March 1st, 2012
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets socket's forceOffline property. Used by client-restriction code to
//|					mark connections for delayed kicking
//o------------------------------------------------------------------------------------------------o
bool CSocket::ForceOffline( void ) const
{
	return forceOffline;
}
void CSocket::ForceOffline( bool newValue )
{
	forceOffline = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::IdleTimeout()
//|	Date		-	November 29th, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the time point at which the char times out
//o------------------------------------------------------------------------------------------------o
SI32 CSocket::IdleTimeout( void ) const
{
	return idleTimeout;
}
void CSocket::IdleTimeout( SI32 newValue )
{
	idleTimeout = newValue;
	wasIdleWarned = false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::WasIdleWarned()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether player has been warned about being idle
//o------------------------------------------------------------------------------------------------o
bool CSocket::WasIdleWarned( void ) const
{
	return wasIdleWarned;
}
void CSocket::WasIdleWarned( bool value )
{
	wasIdleWarned = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::objDelayMsgShown()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether player has been shown message about object use delay
//o------------------------------------------------------------------------------------------------o
bool CSocket::ObjDelayMsgShown( void ) const
{
	return objDelayMsgShown;
}
void CSocket::ObjDelayMsgShown( bool value )
{
	objDelayMsgShown = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::SkillDelayMsgShown()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether player has been shown message about skill use delay
//o------------------------------------------------------------------------------------------------o
bool CSocket::SkillDelayMsgShown( void ) const
{
	return skillDelayMsgShown;
}
void CSocket::SkillDelayMsgShown( bool value )
{
	skillDelayMsgShown = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::NegotiateTimeout()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the time point at which the player gets kicked if assistant tool
//|					has not responded to request to negotiate for features
//o------------------------------------------------------------------------------------------------o
SI32 CSocket::NegotiateTimeout( void ) const
{
	return negotiateTimeout;
}
void CSocket::NegotiateTimeout( SI32 newValue )
{
	negotiateTimeout = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::NegotiatedWithAssistant()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether player's assistant tool has responded to server's request
//|					for negotiating features
//o------------------------------------------------------------------------------------------------o
bool CSocket::NegotiatedWithAssistant( void ) const
{
	return negotiatedWithAssistant;
}
void CSocket::NegotiatedWithAssistant( bool value )
{
	negotiatedWithAssistant = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::Buffer()
//|	Date		-	November 29th, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a pointer to the buffer of the socket
//o------------------------------------------------------------------------------------------------o
UI08 *CSocket::Buffer( void )
{
	return &buffer[0];
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::OutBuffer()
//|	Date		-	November 29th, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a pointer to the outgoing buffer of the socket
//o------------------------------------------------------------------------------------------------o
UI08 *CSocket::OutBuffer( void )
{
	return &outbuffer[0];
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::WalkSequence()
//|	Date		-	November 29th, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the walk sequence value of the socket
//o------------------------------------------------------------------------------------------------o
SI16 CSocket::WalkSequence( void ) const
{
	return walkSequence;
}
void CSocket::WalkSequence( SI16 newValue )
{
	walkSequence = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::AddTrigWord()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds trigger word to list of trigger words detected in player's speech
//o------------------------------------------------------------------------------------------------o
void CSocket::AddTrigWord( UI16 toAdd )
{
	trigWords.push_back( toAdd );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::FirstTrigWord()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the first of potentially many trigger word detected in player's speech
//o------------------------------------------------------------------------------------------------o
UI16 CSocket::FirstTrigWord( void )
{
	UI16 retVal	= 0xFFFF;
	twIter		= trigWords.begin();
	if( !FinishedTrigWords() )
	{
		retVal = ( *twIter );
	}
	return retVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::NextTrigWord()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the next trigger word in list of such words detected in player's speech
//o------------------------------------------------------------------------------------------------o
UI16 CSocket::NextTrigWord( void )
{
	UI16 retVal = 0xFFFF;
	if( !FinishedTrigWords() )
	{
		++twIter;
		if( !FinishedTrigWords() )
		{
			retVal = ( *twIter );
		}
	}
	return retVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::FinishedTrigWords()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if end of list of trigger words has been reached
//o------------------------------------------------------------------------------------------------o
bool CSocket::FinishedTrigWords( void )
{
	return ( twIter == trigWords.end() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::ClearTrigWords()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Clears list of trigger words stored in socket
//o------------------------------------------------------------------------------------------------o
void CSocket::ClearTrigWords( void )
{
	trigWords.resize( 0 );
	twIter = trigWords.end();
}

const SI32				DEFSOCK_IDLETIMEOUT				= -1;
const SI32				DEFSOCK_TEMPINT					= 0;
const UI08				DEFSOCK_DYEALL					= 0;
const SI08				DEFSOCK_CLICKZ					= -1;
const SI16				DEFSOCK_CLICKX					= -1;
const SI16				DEFSOCK_CLICKY					= -1;
const bool				DEFSOCK_NEWCLIENT				= true;
const bool				DEFSOCK_FIRSTPACKET				= true;
const UI08				DEFSOCK_RANGE					= 18;
const bool				DEFSOCK_CRYPTCLIENT				= false;
const SI16				DEFSOCK_WALKSEQUENCE			= -1;
const char				DEFSOCK_CURSPELLTYPE			= 0;
const SI32				DEFSOCK_OUTLENGTH				= 0;
const SI32				DEFSOCK_INLENGTH				= 0;
const bool				DEFSOCK_LOGGING					= LOGDEFAULT;
const SI32				DEFSOCK_POSTACKCOUNT			= 0;
const PickupLocations	DEFSOCK_PSPOT					= PL_NOWHERE;
const SERIAL			DEFSOCK_PFROM					= INVALIDSERIAL;
const SI16				DEFSOCK_PX						= 0;
const SI16				DEFSOCK_PY						= 0;
const SI08				DEFSOCK_PZ						= 0;
const UnicodeTypes		DEFSOCK_LANG					= UT_ENU;
const ClientTypes		DEFSOCK_CLITYPE					= CV_DEFAULT;
const ClientVersions	DEFSOCK_CLIVERSHORT				= CVS_DEFAULT;
const UI32				DEFSOCK_CLIENTVERSION			= CalcSerial( 4, 0, 0, 0 );
const UI32				DEFSOCK_BYTESSENT				= 0;
const UI32				DEFSOCK_BYTESSENTWARNINGCOUNT	= 0;
const UI32				DEFSOCK_BYTESRECEIVED			= 0;
const UI32				DEFSOCK_BYTESRECEIVEDWARNINGCOUNT	= 0;
const bool				DEFSOCK_RECEIVEDVERSION			= false;
const bool				DEFSOCK_LOGINCOMPLETE			= false;

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket constructor
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	This function basically does what the name implies
//o------------------------------------------------------------------------------------------------o
CSocket::CSocket( size_t sockNum ) : currCharObj( nullptr ), idleTimeout( DEFSOCK_IDLETIMEOUT ), clickx( DEFSOCK_CLICKX ), clicky( DEFSOCK_CLICKY ),
clickz( DEFSOCK_CLICKZ ), currentSpellType( DEFSOCK_CURSPELLTYPE ), outlength( DEFSOCK_OUTLENGTH ), inlength( DEFSOCK_INLENGTH ), logging( DEFSOCK_LOGGING ),
range( DEFSOCK_RANGE ), cryptclient( DEFSOCK_CRYPTCLIENT ), cliSocket( sockNum ), walkSequence( DEFSOCK_WALKSEQUENCE ), postAckCount( DEFSOCK_POSTACKCOUNT ),
pSpot( DEFSOCK_PSPOT ), pFrom( DEFSOCK_PFROM ), pX( DEFSOCK_PX ), pY( DEFSOCK_PY ), pZ( DEFSOCK_PZ ), lang( DEFSOCK_LANG ), cliType( DEFSOCK_CLITYPE ),
cliVerShort( DEFSOCK_CLIVERSHORT), clientVersion( DEFSOCK_CLIENTVERSION ), bytesReceived( DEFSOCK_BYTESRECEIVED ), bytesSent( DEFSOCK_BYTESSENT ),
receivedVersion( DEFSOCK_RECEIVEDVERSION ), tmpObj( nullptr ), tmpObj2( nullptr ), tempint( DEFSOCK_TEMPINT ), dyeall( DEFSOCK_DYEALL ),
newClient( DEFSOCK_NEWCLIENT ), firstPacket( DEFSOCK_FIRSTPACKET ), loginComplete( DEFSOCK_LOGINCOMPLETE ), cursorItem( nullptr ),
bytesRecvWarningCount( DEFSOCK_BYTESRECEIVEDWARNINGCOUNT ), bytesSentWarningCount( DEFSOCK_BYTESSENTWARNINGCOUNT )
{
	InternalReset();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket deconstructor
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Cleans up after CSocket object and closes actual socket
//o------------------------------------------------------------------------------------------------o
CSocket::~CSocket()
{
	JSEngine->ReleaseObject( IUE_SOCK, this );

	if( ValidateObject( currCharObj ))
	{
		currCharObj->SetSocket( nullptr );
	}
	closesocket( static_cast<UOXSOCKET>( cliSocket ));
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	void InternalReset( void )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets default values for various socket properties
//o------------------------------------------------------------------------------------------------o
void CSocket::InternalReset( void )
{
	memset( buffer, 0, MAXBUFFER );
	memset( outbuffer, 0, MAXBUFFER );
	xtext.reserve( MAXBUFFER );
	addid[0] = addid[1] = addid[2] = addid[3] = 0;
	clientip[0] = clientip[1] = clientip[2] = clientip[3] = 0;
	// set the socket to nonblocking
	unsigned long mode = 1;
	ioctlsocket( static_cast<UOXSOCKET>( cliSocket ), FIONBIO, &mode );
	for( SI32 mTID = static_cast<SI32>( tPC_SKILLDELAY ); mTID < static_cast<SI32>( tPC_COUNT ); ++mTID )
	{
		pcTimers[mTID] = 0;
	}
	accountNum = AB_INVALID_ID;
	trigWords.resize( 0 );
	twIter = trigWords.end();
	postAcked.clear();
	ackIter = postAcked.end();
	largeBuffer.resize( 0 );
	largePackBuffer.resize( 0 );
	NegotiateTimeout( -1 );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::LoginComplete()
//|	Date		-	August 26th, 2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether this socket has fully logged in
//o------------------------------------------------------------------------------------------------o
bool CSocket::LoginComplete( void ) const
{
	return loginComplete;
}
void CSocket::LoginComplete( bool newVal )
{
	loginComplete = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::CurrentSpellType()
//|	Date		-	November 29th, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the current spell type of the socket
//|						0 - Normal spellcast
//|						1 - Scroll
//|						2 - Wand
//o------------------------------------------------------------------------------------------------o
UI08 CSocket::CurrentSpellType( void ) const
{
	return currentSpellType;
}
void CSocket::CurrentSpellType( UI08 newValue )
{
	currentSpellType = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::FlushBuffer()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends all buffered data in regular buffer immediately
//o------------------------------------------------------------------------------------------------o
bool CSocket::FlushBuffer( bool doLog )
{
	if( outlength > 0 )
	{
		if( cryptclient )
		{
			UI32 len;
			UI08 xoutbuffer[MAXBUFFER*2];
			len = Pack( outbuffer, xoutbuffer, outlength );
			[[maybe_unused]] auto sendResult = send( static_cast<UOXSOCKET>( cliSocket ), ( char * )xoutbuffer, len, 0 );
#if defined( UOX_DEBUG_MODE )
			if( sendResult != len )
			{
				std::cerr << "DANGER DANGER WILL ROBINSON, socket send was less then requested at 739" << std::endl;
			}
#endif
		}
		else
		{
			[[maybe_unused]] auto sendResult = send( static_cast<UOXSOCKET>( cliSocket ), ( char * )&outbuffer[0], outlength, 0 );
#if defined( UOX_DEBUG_MODE )
			if( sendResult != outlength )
			{
				std::cerr << "DANGER DANGER WILL ROBINSON, socket send was less then requested at 744" << std::endl;
			}
#endif
		}
		if(( cwmWorldState->ServerData()->ServerNetworkLog() || Logging() ) && doLog )
		{
			SERIAL toPrint;
			if( !ValidateObject( currCharObj ))
			{
				toPrint = INVALIDSERIAL;
			}
			else
			{
				toPrint = currCharObj->GetSerial();
			}
			std::string logFile = cwmWorldState->ServerData()->Directory( CSDDP_LOGS ) + oldstrutil::number( toPrint ) + std::string( ".snd" );
			std::ofstream logDestination;
			logDestination.open( logFile.c_str(), std::ios::out | std::ios::app );
			if( logDestination.is_open() )
			{
				logDestination << "[SEND]Packet: 0x" << ( outbuffer[0] < 16 ? "0" : "" ) << std::hex << static_cast<UI16>( outbuffer[0] ) << "--> Length: " << std::dec << outlength << TimeStamp() << std::endl;
				DoPacketLogging( logDestination, outlength, outbuffer );
				logDestination.close();
			}
			else
			{
				Console.Error( oldstrutil::format( "Failed to open socket log %s", logFile.c_str() ));
			}
			bytesSent += outlength;
		}
		outlength = 0;
		return true;
	}
	else
		return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::FlushLargeBuffer()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends all buffered data in large buffer immediately
//o------------------------------------------------------------------------------------------------o
bool CSocket::FlushLargeBuffer( bool doLog )
{
	if( outlength > 0 )
	{
		if( cryptclient )
		{
			largePackBuffer.resize( static_cast<size_t>( outlength ) * static_cast<size_t>( 2 ));
			SI32 len = Pack( &largeBuffer[0], &largePackBuffer[0], outlength );
			[[maybe_unused]] auto sendResult = send( static_cast<UOXSOCKET>( cliSocket ), ( char * )&largePackBuffer[0], len, 0 );
#if defined( UOX_DEBUG_MODE )
			if( sendResult != len )
			{
				std::cerr << "DANGER DANGER WILL ROBINSON, socket send was less then requested at 789" << std::endl;
			}
#endif
		}
		else
		{
			[[maybe_unused]] auto sendResult = send( static_cast<UOXSOCKET>( cliSocket ), ( char * )&largeBuffer[0], outlength, 0 );
#if defined( UOX_DEBUG_MODE )
			if( sendResult != outlength )
			{
				std::cerr << "DANGER DANGER WILL ROBINSON, socket send was less then requested at 796" << std::endl;
			}
#endif
		}
		if(( cwmWorldState->ServerData()->ServerNetworkLog() || Logging() ) && doLog )
		{
			SERIAL toPrint;
			if( !ValidateObject( currCharObj ))
			{
				toPrint = INVALIDSERIAL;
			}
			else
			{
				toPrint = currCharObj->GetSerial();
			}
			std::string logFile = cwmWorldState->ServerData()->Directory( CSDDP_LOGS ) + oldstrutil::number( toPrint ) + std::string( ".snd" );
			std::ofstream logDestination;
			logDestination.open( logFile.c_str(), std::ios::out | std::ios::app );
			if( logDestination.is_open() )
			{
				logDestination << "[SEND]Packet: 0x" << ( outbuffer[0] < 16 ? "0" : "" ) << std::hex << static_cast<UI16>( outbuffer[0] ) << "--> Length: " << std::dec << outlength << TimeStamp() << std::endl;
				DoPacketLogging( logDestination, outlength, largeBuffer );
				logDestination.close();
			}
			else
			{
				Console.Error( oldstrutil::format( "Failed to open socket log %s", logFile.c_str() ));
			}
			bytesSent += outlength;
		}
		outlength = 0;
		return true;
	}
	else
		return false;
}

// Huffman Code Table - used with Huffman algorithm for compressing outgoing network packets
static UI32 bit_table[257][2] =
{
	{0x02, 0x00}, 	{0x05, 0x1F}, 	{0x06, 0x22}, 	{0x07, 0x34}, 	{0x07, 0x75}, 	{0x06, 0x28}, 	{0x06, 0x3B}, 	{0x07, 0x32},
	{0x08, 0xE0}, 	{0x08, 0x62}, 	{0x07, 0x56}, 	{0x08, 0x79}, 	{0x09, 0x19D},	{0x08, 0x97}, 	{0x06, 0x2A}, 	{0x07, 0x57},
	{0x08, 0x71}, 	{0x08, 0x5B}, 	{0x09, 0x1CC},	{0x08, 0xA7}, 	{0x07, 0x25}, 	{0x07, 0x4F}, 	{0x08, 0x66}, 	{0x08, 0x7D},
	{0x09, 0x191},	{0x09, 0x1CE}, 	{0x07, 0x3F}, 	{0x09, 0x90}, 	{0x08, 0x59}, 	{0x08, 0x7B}, 	{0x08, 0x91}, 	{0x08, 0xC6},
	{0x06, 0x2D}, 	{0x09, 0x186}, 	{0x08, 0x6F}, 	{0x09, 0x93}, 	{0x0A, 0x1CC},	{0x08, 0x5A}, 	{0x0A, 0x1AE},	{0x0A, 0x1C0},
	{0x09, 0x148},	{0x09, 0x14A}, 	{0x09, 0x82}, 	{0x0A, 0x19F}, 	{0x09, 0x171},	{0x09, 0x120}, 	{0x09, 0xE7}, 	{0x0A, 0x1F3},
	{0x09, 0x14B},	{0x09, 0x100},	{0x09, 0x190},	{0x06, 0x13}, 	{0x09, 0x161},	{0x09, 0x125},	{0x09, 0x133},	{0x09, 0x195},
	{0x09, 0x173},	{0x09, 0x1CA},	{0x09, 0x86}, 	{0x09, 0x1E9}, 	{0x09, 0xDB}, 	{0x09, 0x1EC},	{0x09, 0x8B}, 	{0x09, 0x85},
	{0x05, 0x0A}, 	{0x08, 0x96}, 	{0x08, 0x9C}, 	{0x09, 0x1C3}, 	{0x09, 0x19C},	{0x09, 0x8F}, 	{0x09, 0x18F},	{0x09, 0x91},
	{0x09, 0x87}, 	{0x09, 0xC6}, 	{0x09, 0x177},	{0x09, 0x89}, 	{0x09, 0xD6}, 	{0x09, 0x8C}, 	{0x09, 0x1EE},	{0x09, 0x1EB},
	{0x09, 0x84}, 	{0x09, 0x164}, 	{0x09, 0x175},	{0x09, 0x1CD}, 	{0x08, 0x5E}, 	{0x09, 0x88}, 	{0x09, 0x12B},	{0x09, 0x172},
	{0x09, 0x10A},	{0x09, 0x8D}, 	{0x09, 0x13A},	{0x09, 0x11C}, 	{0x0A, 0x1E1},	{0x0A, 0x1E0}, 	{0x09, 0x187},	{0x0A, 0x1DC},
	{0x0A, 0x1DF},	{0x07, 0x74}, 	{0x09, 0x19F},	{0x08, 0x8D},	{0x08, 0xE4}, 	{0x07, 0x79}, 	{0x09, 0xEA}, 	{0x09, 0xE1},
	{0x08, 0x40}, 	{0x07, 0x41}, 	{0x09, 0x10B},	{0x09, 0xB0}, 	{0x08, 0x6A}, 	{0x08, 0xC1}, 	{0x07, 0x71}, 	{0x07, 0x78},
	{0x08, 0xB1}, 	{0x09, 0x14C}, 	{0x07, 0x43}, 	{0x08, 0x76}, 	{0x07, 0x66}, 	{0x07, 0x4D}, 	{0x09, 0x8A}, 	{0x06, 0x2F},
	{0x08, 0xC9},	{0x09, 0xCE}, 	{0x09, 0x149},	{0x09, 0x160}, 	{0x0A, 0x1BA}, 	{0x0A, 0x19E}, 	{0x0A, 0x39F}, 	{0x09, 0xE5},
	{0x09, 0x194}, 	{0x09, 0x184}, 	{0x09, 0x126}, 	{0x07, 0x30}, 	{0x08, 0x6C}, 	{0x09, 0x121}, 	{0x09, 0x1E8}, 	{0x0A, 0x1C1},
	{0x0A, 0x11D}, 	{0x0A, 0x163}, 	{0x0A, 0x385}, 	{0x0A, 0x3DB}, 	{0x0A, 0x17D}, 	{0x0A, 0x106}, 	{0x0A, 0x397}, 	{0x0A, 0x24E},
	{0x07, 0x2E}, 	{0x08, 0x98}, 	{0x0A, 0x33C}, 	{0x0A, 0x32E}, 	{0x0A, 0x1E9}, 	{0x09, 0xBF}, 	{0x0A, 0x3DF}, 	{0x0A, 0x1DD},
	{0x0A, 0x32D}, 	{0x0A, 0x2ED}, 	{0x0A, 0x30B}, 	{0x0A, 0x107}, 	{0x0A, 0x2E8}, 	{0x0A, 0x3DE}, 	{0x0A, 0x125}, 	{0x0A, 0x1E8},
	{0x09, 0xE9}, 	{0x0A, 0x1CD}, 	{0x0A, 0x1B5}, 	{0x09, 0x165}, 	{0x0A, 0x232}, 	{0x0A, 0x2E1}, 	{0x0B, 0x3AE}, 	{0x0B, 0x3C6},
	{0x0B, 0x3E2}, 	{0x0A, 0x205}, 	{0x0A, 0x29A}, 	{0x0A, 0x248}, 	{0x0A, 0x2CD}, 	{0x0A, 0x23B}, 	{0x0B, 0x3C5}, 	{0x0A, 0x251},
	{0x0A, 0x2E9}, 	{0x0A, 0x252}, 	{0x09, 0x1EA}, 	{0x0B, 0x3A0}, 	{0x0B, 0x391}, 	{0x0A, 0x23C}, 	{0x0B, 0x392}, 	{0x0B, 0x3D5},
	{0x0A, 0x233}, 	{0x0A, 0x2CC}, 	{0x0B, 0x390}, 	{0x0A, 0x1BB}, 	{0x0B, 0x3A1}, 	{0x0B, 0x3C4}, 	{0x0A, 0x211}, 	{0x0A, 0x203},
	{0x09, 0x12A}, 	{0x0A, 0x231}, 	{0x0B, 0x3E0}, 	{0x0A, 0x29B}, 	{0x0B, 0x3D7}, 	{0x0A, 0x202}, 	{0x0B, 0x3AD}, 	{0x0A, 0x213},
	{0x0A, 0x253}, 	{0x0A, 0x32C}, 	{0x0A, 0x23D}, 	{0x0A, 0x23F}, 	{0x0A, 0x32F}, 	{0x0A, 0x11C}, 	{0x0A, 0x384}, 	{0x0A, 0x31C},
	{0x0A, 0x17C}, 	{0x0A, 0x30A}, 	{0x0A, 0x2E0}, 	{0x0A, 0x276}, 	{0x0A, 0x250}, 	{0x0B, 0x3E3}, 	{0x0A, 0x396}, 	{0x0A, 0x18F},
	{0x0A, 0x204}, 	{0x0A, 0x206}, 	{0x0A, 0x230}, 	{0x0A, 0x265}, 	{0x0A, 0x212}, 	{0x0A, 0x23E}, 	{0x0B, 0x3AC}, 	{0x0B, 0x393},
	{0x0B, 0x3E1}, 	{0x0A, 0x1DE}, 	{0x0B, 0x3D6}, 	{0x0A, 0x31D}, 	{0x0B, 0x3E5}, 	{0x0B, 0x3E4}, 	{0x0A, 0x207}, 	{0x0B, 0x3C7},
	{0x0A, 0x277}, 	{0x0B, 0x3D4}, 	{0x08, 0xC0},	{0x0A, 0x162}, 	{0x0A, 0x3DA}, 	{0x0A, 0x124}, 	{0x0A, 0x1B4}, 	{0x0A, 0x264},
	{0x0A, 0x33D}, 	{0x0A, 0x1D1}, 	{0x0A, 0x1AF}, 	{0x0A, 0x39E}, 	{0x0A, 0x24F}, 	{0x0B, 0x373}, 	{0x0A, 0x249}, 	{0x0B, 0x372},
	{0x09, 0x167}, 	{0x0A, 0x210}, 	{0x0A, 0x23A}, 	{0x0A, 0x1B8}, 	{0x0B, 0x3AF}, 	{0x0A, 0x18E}, 	{0x0A, 0x2EC}, 	{0x07, 0x62},
	{0x04, 0x0D}
};

//o------------------------------------------------------------------------------------------------o
//|	Function	-	DoPack()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Compress outgoing buffer contents and calculate total length of compressed data
//o------------------------------------------------------------------------------------------------o
UI32 DoPack( UI08 *pIn, UI08 *pOut, SI32 len )
{
	UI32 packedLength	= 0;
	SI32 bitByte		= 0;
	SI32 nrBits;
	UI32 value;

	while( len-- )
	{
		nrBits	= bit_table[*pIn][0];
		value	= bit_table[*pIn++][1];

		while( nrBits-- )
		{
			pOut[packedLength] = static_cast<UI08>(( pOut[packedLength] << 1 ) | static_cast<UI08>((value >> nrBits) & 0x1 ));

			bitByte = ( bitByte + 1 ) & 0x07;
			if( !bitByte )
			{
				++packedLength;
			}
		}
	}

	nrBits	= bit_table[256][0];
	value	= bit_table[256][1];

	while( nrBits-- )
	{
		pOut[packedLength] = static_cast<UI08>(( pOut[packedLength] << 1 ) | static_cast<UI08>(( value >> nrBits ) & 0x1 ));

		bitByte = ( bitByte + 1 ) & 0x07;
		if( !bitByte )
		{
			++packedLength;
		}
	}

	if( bitByte )
	{
		while( bitByte < 8 )
		{
			pOut[packedLength] <<= 1;
			++bitByte;
		}
		++packedLength;
	}
	return packedLength;
}

UI32 CSocket::Pack( void *pvIn, void *pvOut, SI32 len )
{
	UI08 *pIn = static_cast<UI08 *>( pvIn );
	UI08 *pOut = static_cast<UI08 *>( pvOut );

	return DoPack( pIn, pOut, len );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::Send()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Buffering send function
//o------------------------------------------------------------------------------------------------o
void CSocket::Send( const void *point, SI32 length )
{
	if( outlength + length > MAXBUFFER )
	{
		FlushBuffer();
	}
	if( outlength > 0 )
	{
		Console.Print( oldstrutil::format( "Fragmented packet! [packet: %i]\n", outbuffer[0] ));
	}
	// sometimes we send enormous packets... oh what fun
	if( length > MAXBUFFER )
	{
#if defined( UOX_DEBUG_MODE )
		Console.Print( oldstrutil::format( "Large packet found [%i]\n", outbuffer[0] ));
#endif
		largeBuffer.resize( length );
		memcpy( &largeBuffer[0], point, length );
		outlength = length;
		FlushLargeBuffer();
		return;
	}
	else
	{
		memcpy( &outbuffer[outlength], point, length );
	}

	outlength += length;
}

#if PLATFORM != WINDOWS
SI32 GrabLastError( void )
{
	return errno;
}
#else
SI32 GrabLastError( void )
{
	return WSAGetLastError();
}
#endif

void CSocket::FlushIncoming( void )
{
	SI32 count = 0;
	do
	{
		count = static_cast<int>( recv( static_cast<UOXSOCKET>( cliSocket ), ( char * )&buffer[static_cast<int>( inlength )], 1, 0 ));
	} while( count > 0 );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::ReceiveLogging()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Logs received network packets
//o------------------------------------------------------------------------------------------------o
void CSocket::ReceiveLogging( CPInputBuffer *toLog )
{
	if( cwmWorldState->ServerData()->ServerNetworkLog() || Logging() )
	{
		SERIAL toPrint;
		if( !ValidateObject( currCharObj ))
		{
			toPrint = INVALIDSERIAL;
		}
		else
		{
			toPrint = currCharObj->GetSerial();
		}
		std::string logFile = cwmWorldState->ServerData()->Directory( CSDDP_LOGS ) + oldstrutil::number( toPrint ) + std::string( ".rcv" );
		std::ofstream logDestination;
		logDestination.open( logFile.c_str(), std::ios::out | std::ios::app );
		if( !logDestination.is_open() )
		{
			Console.Error( oldstrutil::format( "Failed to open socket log %s", logFile.c_str() ));
			return;
		}
		if( toLog != nullptr )
		{
			toLog->Log( logDestination );
		}
		else
		{
			logDestination << "[RECV]Packet: 0x" << std::hex << ( buffer[0] < 10 ? "0" : "" ) << static_cast<UI16>( buffer[0] ) << " --> Length: " << std::dec << inlength << TimeStamp() << std::endl;
			DoPacketLogging( logDestination, inlength, buffer );
		}
		logDestination.close();
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::Receive()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles receiving of network packets
//o------------------------------------------------------------------------------------------------o
SI32 CSocket::Receive( SI32 x, bool doLog )
{
	SI32 count			= 0;
	UI08 recvAttempts	= 0;
	UI32 curTime		= GetClock();
	UI32 nexTime		= curTime;
	do
	{
		count = static_cast<int>( recv( static_cast<UOXSOCKET>( cliSocket ), ( char * )&buffer[inlength], x - inlength, 0 ));
		if( count > 0 )
		{
			inlength += count;
		}
		else if( count == -1 )
		{
			SI32 lastError = GrabLastError();
#if PLATFORM != WINDOWS
			if( lastError != EWOULDBLOCK )
#else
				if( lastError != WSAEWOULDBLOCK )
#endif
					throw socket_error( lastError );
			std::this_thread::sleep_for( std::chrono::milliseconds( 20 ));
		}
		++recvAttempts;
		nexTime = GetClock();
		// You will find the values for the following in the uox.ini file as NETRCVTIMEOUT, and NETRETRYCOUNT respectively
		if( recvAttempts == cwmWorldState->ServerData()->ServerNetRetryCount() || (nexTime - curTime) > static_cast<UI32>( cwmWorldState->ServerData()->ServerNetRcvTimeout() * 1000 ))
		{ // looks like we're not going to get it!
			// April 3, 2004 - If we have some data, then we need to return it. Some of the network logic is looking at count size. this way we can also validate on the calling side so we ask for 4 bytes, but only 3 were sent back, adn let the calling routing handle it, if we call for 4 and get get NOTHING then throw... Just my thoughts -
			if( count <= 0 )
			{
				throw socket_error( "Socket receive error" );
			}
		}
	}
	while( x != MAXBUFFER && x != inlength );
	if( doLog )
	{
		ReceiveLogging( nullptr );
	}
	bytesReceived += count;
	return count;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::OutLength()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets outlength value
//o------------------------------------------------------------------------------------------------o
SI32 CSocket::OutLength( void ) const
{
	return outlength;
}
void CSocket::OutLength( SI32 newValue )
{
	outlength = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::InLength()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets inlength value
//o------------------------------------------------------------------------------------------------o
SI32 CSocket::InLength( void ) const
{
	return inlength;
}
void CSocket::InLength( SI32 newValue )
{
	inlength = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::Logging()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether logging is enabled or disabled for socket
//o------------------------------------------------------------------------------------------------o
bool CSocket::Logging( void ) const
{
	return logging;
}
void CSocket::Logging( bool newValue )
{
	logging = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::CurrcharObj()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets current character object for socket
//o------------------------------------------------------------------------------------------------o
CChar *CSocket::CurrcharObj( void ) const
{
	return currCharObj;
}
void CSocket::CurrcharObj( CChar *newValue )
{
	if( ValidateObject( currCharObj ))
	{
		if( currCharObj->GetSocket() == this )
		{
			currCharObj->SetSocket( nullptr );
		}
	}
	if( ValidateObject( newValue ))
	{
		newValue->SetSocket( this );
	}
	currCharObj = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::GetAccount()
//|					CSocket::SetAccount()
//|	Date		-	1/17/2003 6:21:59 AM
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the socket's account reference
//o------------------------------------------------------------------------------------------------o
CAccountBlock_st& CSocket::GetAccount( void )
{
	return Accounts->GetAccountById( accountNum );
}
void CSocket::SetAccount( CAccountBlock_st& actbBlock )
{
	accountNum = actbBlock.wAccountIndex;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::AcctNo()
//|	Date		-	November 29th, 2000
//|	Changes		-	February 3, 2003 - reduced to UI16 to deal with accounts changes
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the account ID socket belongs to
//o------------------------------------------------------------------------------------------------o
UI16 CSocket::AcctNo( void ) const
{
	return accountNum;
}
void CSocket::AcctNo( UI16 newValue )
{
	accountNum = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::ClientIP1()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets part 1 of client IP
//o------------------------------------------------------------------------------------------------o
UI08 CSocket::ClientIP1( void ) const
{
	return clientip[0];
}
void CSocket::ClientIP1( UI08 newValue )
{
	clientip[0] = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::ClientIP2()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets part 2 of client IP
//o------------------------------------------------------------------------------------------------o
UI08 CSocket::ClientIP2( void ) const
{
	return clientip[1];
}
void CSocket::ClientIP2( UI08 newValue )
{
	clientip[1] = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::ClientIP3()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets part 3 of client IP
//o------------------------------------------------------------------------------------------------o
UI08 CSocket::ClientIP3( void ) const
{
	return clientip[2];
}
void CSocket::ClientIP3( UI08 newValue )
{
	clientip[2] = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::ClientIP4()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets part 4 of client IP
//o------------------------------------------------------------------------------------------------o
UI08 CSocket::ClientIP4( void ) const
{
	return clientip[3];
}
void CSocket::ClientIP4( UI08 newValue )
{
	clientip[3] = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::NewClient()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the socket property used to determine if connection is new or old
//o------------------------------------------------------------------------------------------------o
bool CSocket::NewClient( void ) const
{
	return newClient;
}
void CSocket::NewClient( bool newValue )
{
	newClient = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::GetDWord()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Retrieves 4 bytes of data from socket buffer at specified offset
//o------------------------------------------------------------------------------------------------o
UI32 CSocket::GetDWord( size_t offset )
{
	UI32 retVal = 0;
	if( offset + 3 >= MAXBUFFER )
	{
		Console.Error( oldstrutil::format( "GetDWord was passed an invalid offset value 0x%X", offset ));
	}
	else
	{
		retVal = CalcSerial( buffer[offset], buffer[offset+1], buffer[offset+2], buffer[offset+3] );
	}
	return retVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::GetWord()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Retrieves 2 bytes of data from socket buffer at specified offset
//o------------------------------------------------------------------------------------------------o
UI16 CSocket::GetWord( size_t offset )
{
	UI16 retVal = 0;
	if( offset+1 >= MAXBUFFER )
	{
		Console.Error( oldstrutil::format( "GetWord was passed an invalid offset value 0x%X", offset ));
	}
	else
	{
		retVal = static_cast<UI16>(( buffer[offset] << 8 ) + buffer[offset+1] );
	}
	return retVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::GetByte()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Retrieves 1 byte of data from socket buffer at specified offset
//o------------------------------------------------------------------------------------------------o
UI08 CSocket::GetByte( size_t offset )
{
	UI08 retVal = 0;
	if( offset >= MAXBUFFER )
	{
		Console.Error( oldstrutil::format( "GetByte was passed an invalid offset value 0x%X", offset ));
	}
	else
	{
		retVal = buffer[offset];
	}
	return retVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::SetDWord()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Inserts 4 bytes of data in socket buffer at specified offset
//o------------------------------------------------------------------------------------------------o
void CSocket::SetDWord( size_t offset, UI32 newValue )
{
	buffer[offset]   = static_cast<UI08>( newValue >> 24 );
	buffer[offset+1] = static_cast<UI08>( newValue >> 16 );
	buffer[offset+2] = static_cast<UI08>( newValue >> 8 );
	buffer[offset+3] = static_cast<UI08>( newValue % 256 );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::SetWord()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Inserts 2 bytes of data in socket buffer at specified offset
//o------------------------------------------------------------------------------------------------o
void CSocket::SetWord( size_t offset, UI16 newValue )
{
	buffer[offset]   = static_cast<UI08>( newValue >> 8 );
	buffer[offset+1] = static_cast<UI08>( newValue % 256 );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::SetByte()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Inserts 1 byte of data in socket buffer at specified offset
//o------------------------------------------------------------------------------------------------o
void CSocket::SetByte( size_t offset, UI08 newValue )
{
	buffer[offset] = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::ClientIP()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the four parts of the client IP to specified value
//o------------------------------------------------------------------------------------------------o
void CSocket::ClientIP( UI32 newValue )
{
	clientip[0] = static_cast<UI08>( newValue >> 24 );
	clientip[1] = static_cast<UI08>( newValue >> 16 );
	clientip[2] = static_cast<UI08>( newValue >> 8 );
	clientip[3] = static_cast<UI08>( newValue % 256 );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::TargetOK()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets socket property to indicate whether waiting for client target
//o------------------------------------------------------------------------------------------------o
void CSocket::TargetOK( bool newValue )
{
	targetok = newValue;
}
bool CSocket::TargetOK( void ) const
{
	return targetok;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::ClickX()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets socket property that temporarily stores small values
//o------------------------------------------------------------------------------------------------o
void CSocket::ClickX( SI16 newValue )
{
	clickx = newValue;
}
SI16 CSocket::ClickX( void ) const
{
	return clickx;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::ClickY()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets socket property that temporarily stores small values
//o------------------------------------------------------------------------------------------------o
void CSocket::ClickY( SI16 newValue )
{
	clicky = newValue;
}
SI16 CSocket::ClickY( void ) const
{
	return clicky;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::FirstPostAck()
//|	Date		-	July 14, 2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Moves to the start of the post ack list
//o------------------------------------------------------------------------------------------------o
SERIAL CSocket::FirstPostAck( void )
{
	SERIAL retVal = INVALIDSERIAL;
	ackIter = postAcked.begin();
	if( !FinishedPostAck() )
	{
		retVal = ( *ackIter );
	}
	return retVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::NextPostAck()
//|	Date		-	July 14, 2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Moves to the next post to ack in the list
//o------------------------------------------------------------------------------------------------o
SERIAL CSocket::NextPostAck( void )
{
	SERIAL retVal = INVALIDSERIAL;
	if( !FinishedPostAck() )
	{
		++ackIter;
		if( !FinishedPostAck() )
		{
			retVal = ( *ackIter );
		}
	}
	return retVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::FinishedPostAck()
//|	Date		-	July 14, 2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if the iterator is at the end of the list
//o------------------------------------------------------------------------------------------------o
bool CSocket::FinishedPostAck( void )
{
	return ( ackIter == postAcked.end() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-  CSocket::RemovePostAck()
//|	Date		-  July 16, 2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-  Removes post from the queue
//o------------------------------------------------------------------------------------------------o
SERIAL CSocket::RemovePostAck( void )
{
	SERIAL retVal = INVALIDSERIAL;
	if( !FinishedPostAck() )
	{
		ackIter = postAcked.erase( ackIter );
		if( !FinishedPostAck() )
		{
			retVal = ( *ackIter );
		}
	}
	return retVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::PostCount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets number of posts in queue waiting to be acknowledged by client
//o------------------------------------------------------------------------------------------------o
size_t CSocket::PostCount( void ) const
{
	return postAcked.size();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::PostAckCount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets postAckCount
//o------------------------------------------------------------------------------------------------o
size_t CSocket::PostAckCount( void ) const
{
	return postAckCount;
}
void CSocket::PostAckCount( size_t newValue )
{
	postAckCount = newValue;
}
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::PostClear()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Clears list of messageboard posts waiting to be acknowledged by the client
//o------------------------------------------------------------------------------------------------o
void CSocket::PostClear( void )
{
	postAcked.clear();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::PostAcked()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds serial of messageboard post to list of posts waiting
//|					to be acknowledged by the client
//o------------------------------------------------------------------------------------------------o
void CSocket::PostAcked( SERIAL newValue )
{
	postAcked.push_back( newValue );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::Send()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends buffer to network socket
//o------------------------------------------------------------------------------------------------o
void CSocket::Send( CPUOXBuffer *toSend )
{
	if( toSend == nullptr )
		return;

	// If the client cannot receive it validly, abort, abort!
	if( !toSend->ClientCanReceive( this ))
		return;

	UI32 len = 0;
	if( cryptclient )
	{
		len = toSend->Pack();
		[[maybe_unused]] auto sendResult = send( static_cast<UOXSOCKET>( cliSocket ), ( char * )toSend->PackedPointer(), len, 0 );
#if defined( UOX_DEBUG_MODE )
		if( sendResult != len )
		{
			std::cerr << "DANGER DANGER WILL ROBINSON, socket send was less then requested at 1492" << std::endl;
		}
#endif
	}
	else
	{
		len = static_cast<UI32>( toSend->GetPacketStream().GetSize() );
		[[maybe_unused]] auto sendResult = send( static_cast<UOXSOCKET>( cliSocket ), ( char * )toSend->GetPacketStream().GetBuffer(), len, 0 );
#if defined( UOX_DEBUG_MODE )
		if( sendResult != len )
		{
			std::cerr << "DANGER DANGER WILL ROBINSON, socket send was less then requested at 1501" << std::endl;
		}
#endif
	}

	bytesSent += len;

	if( cwmWorldState->ServerData()->ServerNetworkLog() || Logging() )
	{
		SERIAL toPrint;
		if( !ValidateObject( currCharObj ))
		{
			toPrint = INVALIDSERIAL;
		}
		else
		{
			toPrint = currCharObj->GetSerial();
		}
		std::string logFile = cwmWorldState->ServerData()->Directory( CSDDP_LOGS ) + oldstrutil::number( toPrint ) + std::string( ".snd" );
		std::ofstream logDestination;
		logDestination.open( logFile.c_str(), std::ios::out | std::ios::app );
		if( !logDestination.is_open() )
		{
			Console.Error( oldstrutil::format( "Failed to open socket log %s", logFile.c_str() ));
			return;
		}
		toSend->Log( logDestination );
		logDestination.close();
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::PickupSpot()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the spot item was picked up from
//o------------------------------------------------------------------------------------------------o
void CSocket::PickupSpot( PickupLocations newValue )
{
	pSpot = newValue;
}
PickupLocations	CSocket::PickupSpot( void ) const
{
	return pSpot;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::PickupSerial()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the serial of the object item was picked up from
//o------------------------------------------------------------------------------------------------o
SERIAL CSocket::PickupSerial( void ) const
{
	return pFrom;
}
void CSocket::PickupSerial( SERIAL pickupSerial )
{
	pFrom = pickupSerial;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::PickupX()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the X world coordinate the item was picked up from
//o------------------------------------------------------------------------------------------------o
SI16 CSocket::PickupX( void ) const
{
	return pX;
}
void CSocket::PickupX( SI16 x )
{
	pX = x;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::PickupY()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the Y world coordinate the item was picked up from
//o------------------------------------------------------------------------------------------------o
SI16 CSocket::PickupY( void ) const
{
	return pY;
}
void CSocket::PickupY( SI16 y )
{
	pY = y;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::PickupZ()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the Z world coordinate the item was picked up from
//o------------------------------------------------------------------------------------------------o
SI08 CSocket::PickupZ( void ) const
{
	return pZ;
}
void CSocket::PickupZ( SI08 z )
{
	pZ = z;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::PickupLocation()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the X, Y and Z world coordinates the item was picked up from
//o------------------------------------------------------------------------------------------------o
void CSocket::PickupLocation( SI16 x, SI16 y, SI08 z )
{
	pX = x;
	pY = y;
	pZ = z;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::GetCursorItem()
//|					CSocket::SetCursorItem()
//|	Date		-	March 20th, 2012
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item being held on a player's mouse cursor
//o------------------------------------------------------------------------------------------------o
CItem *CSocket::GetCursorItem( void ) const
{
	return cursorItem;
}
void CSocket::SetCursorItem( CItem *newCursorItem )
{
	if( ValidateObject( newCursorItem ) || newCursorItem == nullptr )
	{
		cursorItem = newCursorItem;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::GetSocket()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets socket from input buffer
//o------------------------------------------------------------------------------------------------o
CSocket *CPInputBuffer::GetSocket( void ) const
{
	return tSock;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::Language()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets language for socket
//o------------------------------------------------------------------------------------------------o
UnicodeTypes CSocket::Language( void ) const
{
	return lang;
}
void CSocket::Language( UnicodeTypes newVal )
{
	lang = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::ClientVersion()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets a calculated serial of player's client version
//o------------------------------------------------------------------------------------------------o
UI32 CSocket::ClientVersion( void ) const
{
	return clientVersion;
}
void CSocket::ClientVersion( UI32 newVer )
{
	clientVersion = newVer;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::ClientVersion()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculates serial based on major, minor, sub and letter client version parts
//o------------------------------------------------------------------------------------------------o
void CSocket::ClientVersion( UI08 major, UI08 minor, UI08 sub, UI08 letter )
{
	ClientVersion( CalcSerial( major, minor, sub, letter ));
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::ClientType()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets client type associated with socket
//o------------------------------------------------------------------------------------------------o
ClientTypes CSocket::ClientType( void ) const
{
	return cliType;
}
void CSocket::ClientType( ClientTypes newVer )
{
	cliType = newVer;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::ClientVerShort()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the short version of client version associated with socket
//o------------------------------------------------------------------------------------------------o
ClientVersions CSocket::ClientVerShort( void ) const
{
	return cliVerShort;
}
void CSocket::ClientVerShort( ClientVersions newVer )
{
	cliVerShort = newVer;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::ClientVersionMajor()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets major part of client version associated with socket
//o------------------------------------------------------------------------------------------------o
UI08 CSocket::ClientVersionMajor( void ) const
{
	return static_cast<UI08>( clientVersion >> 24 );
}
void CSocket::ClientVersionMajor( UI08 value )
{
	ClientVersion( value, ClientVersionMinor(), ClientVersionSub(), ClientVersionLetter() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::ClientVersionMinor()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets minor part of client version associated with socket
//o------------------------------------------------------------------------------------------------o
UI08 CSocket::ClientVersionMinor( void ) const
{
	return static_cast<UI08>( clientVersion >> 16 );
}
void CSocket::ClientVersionMinor( UI08 value )
{
	ClientVersion( ClientVersionMajor(), value, ClientVersionSub(), ClientVersionLetter() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::ClientVersionSub()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets sub part of client version associated with socket
//o------------------------------------------------------------------------------------------------o
UI08 CSocket::ClientVersionSub( void ) const
{
	return static_cast<UI08>( clientVersion >> 8 );
}
void CSocket::ClientVersionSub( UI08 value )
{
	ClientVersion( ClientVersionMajor(), ClientVersionMinor(), value, ClientVersionLetter() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::ClientVersionLetter()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets letter part of client version associated with socket
//o------------------------------------------------------------------------------------------------o
UI08 CSocket::ClientVersionLetter( void ) const
{
	return static_cast<UI08>( clientVersion % 256 );
}
void CSocket::ClientVersionLetter( UI08 value )
{
	ClientVersion( ClientVersionMajor(), ClientVersionMinor(), ClientVersionSub(), value );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::Range()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the range for which the client receives updates on objects
//o------------------------------------------------------------------------------------------------o
UI08 CSocket::Range( void ) const
{
	return range;
}
void CSocket::Range( UI08 value )
{
	if( ClientVerShort() < CVS_705527 )
	{
		range = std::min( value, static_cast<UI08>( 18 )); // 18 is max range for 2D clients prior to 7.0.55.27
	}
	else
	{
		range = std::min( value, static_cast<UI08>( 24 )); // 24 is max range for 2D clients after that, or enhanced client
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::SysMessage()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Displays specified system message in lower left corner of client screen
//o------------------------------------------------------------------------------------------------o
void CSocket::SysMessage( const std::string txt, ... )
{
	va_list argptr;
	if( txt.empty() )
		return;

	CChar *mChar = CurrcharObj();
	if( !ValidateObject( mChar ))
		return;

	va_start( argptr, txt );
	auto msg = oldstrutil::format( txt, argptr );
	if( msg.size() > 512 )
	{
		msg = msg.substr( 0, 512 );
	}

	if( cwmWorldState->ServerData()->UseUnicodeMessages() )
	{
		CPUnicodeMessage unicodeMessage;
		unicodeMessage.Message( msg );
		unicodeMessage.Font( 4 );
		unicodeMessage.Colour( cwmWorldState->ServerData()->SysMsgColour() );
		unicodeMessage.Type( SYSTEM );
		unicodeMessage.Language( "ENG" );
		unicodeMessage.Name( "System" );
		unicodeMessage.ID( INVALIDID );
		unicodeMessage.Serial( INVALIDSERIAL );

		Send( &unicodeMessage );
	}
	else
	{
		CSpeechEntry& toAdd = SpeechSys->Add();
		toAdd.Speech( msg );
		toAdd.Font( FNT_NORMAL );
		toAdd.Speaker( INVALIDSERIAL );
		toAdd.SpokenTo( mChar->GetSerial() );
		toAdd.Colour( cwmWorldState->ServerData()->SysMsgColour() );
		toAdd.Type( SYSTEM );
		toAdd.At( cwmWorldState->GetUICurrentTime() );
		toAdd.TargType( SPTRG_INDIVIDUAL );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::SysMessageJS()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Displays specified system message in lower left corner of client screen
//o------------------------------------------------------------------------------------------------o
void CSocket::SysMessageJS( const std::string& uformat, UI16 txtColor, const std::string& data )
{
	CChar *mChar = CurrcharObj();
	if( !ValidateObject( mChar ))
		return;

	auto msg = oldstrutil::formatMessage( uformat, data );
	if( msg.size() > 512 )
	{
		msg = msg.substr( 0, 512 );
	}
	
	if( txtColor == 0 )
	{
		txtColor = cwmWorldState->ServerData()->SysMsgColour();
	}
	
	if( cwmWorldState->ServerData()->UseUnicodeMessages() )
	{
		CPUnicodeMessage unicodeMessage;
		unicodeMessage.Message( msg );
		unicodeMessage.Font( 4 );
		unicodeMessage.Colour( txtColor );
		unicodeMessage.Type( SYSTEM );
		unicodeMessage.Language( "ENG" );
		unicodeMessage.Name( "System" );
		unicodeMessage.ID( INVALIDID );
		unicodeMessage.Serial( INVALIDSERIAL );

		Send( &unicodeMessage );
	}
	else
	{
		CSpeechEntry& toAdd = SpeechSys->Add();
		toAdd.Unicode( true );
		toAdd.Speech( msg );
		toAdd.Font( FNT_NORMAL );
		toAdd.Speaker( INVALIDSERIAL );
		toAdd.SpokenTo( mChar->GetSerial() );
		toAdd.Colour( txtColor );
		toAdd.Type( SYSTEM );
		toAdd.At( cwmWorldState->GetUICurrentTime() );
		toAdd.TargType( SPTRG_INDIVIDUAL );
	}
}
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::SysMessage()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Displays dictionary-based system message in lower left corner of client screen
//o------------------------------------------------------------------------------------------------o
void CSocket::SysMessage( SI32 dictEntry, ... )
{
	CChar *mChar = CurrcharObj();
	if( !ValidateObject( mChar ))
		return;

	va_list argptr;
	std::string txt = Dictionary->GetEntry( dictEntry, Language() );
	if( txt.empty() )
		return;

	va_start( argptr, dictEntry );
	auto msg = oldstrutil::format( txt, argptr );
	if( msg.size() > 512 )
	{
		msg = msg.substr( 0, 512 );
	}

	if( cwmWorldState->ServerData()->UseUnicodeMessages() )
	{
		CPUnicodeMessage unicodeMessage;
		unicodeMessage.Message( msg );
		unicodeMessage.Font( 4 );
		unicodeMessage.Colour( cwmWorldState->ServerData()->SysMsgColour() );
		unicodeMessage.Type( SYSTEM );
		unicodeMessage.Language( "ENG" );
		unicodeMessage.Name( "System" );
		unicodeMessage.ID( INVALIDID );
		unicodeMessage.Serial( INVALIDSERIAL );

		Send( &unicodeMessage );
	}
	else
	{
		CSpeechEntry& toAdd = SpeechSys->Add();
		toAdd.Unicode( true );
		toAdd.Speech( msg );
		toAdd.Font( FNT_NORMAL );
		toAdd.Speaker( INVALIDSERIAL );
		toAdd.SpokenTo( mChar->GetSerial() );
		toAdd.Colour( cwmWorldState->ServerData()->SysMsgColour() );
		toAdd.Type( SYSTEM );
		toAdd.At( cwmWorldState->GetUICurrentTime() );
		toAdd.TargType( SPTRG_INDIVIDUAL );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::ObjMessage()
//|	Date		-	2/11/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Shows information on items when clicked or guild info (if any) for players
//o------------------------------------------------------------------------------------------------o
void CSocket::ObjMessage( SI32 dictEntry, CBaseObject *getObj, R32 secsFromNow, UI32 Colour, ... )
{
	if( !ValidateObject( getObj ))
		return;

	std::string txt = Dictionary->GetEntry( dictEntry, Language() );
	if( txt.empty() )
		return;

	va_list argptr;
	va_start( argptr, Colour );

	ObjMessage( oldstrutil::format( txt, argptr ).c_str(), getObj, secsFromNow, Colour );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::ObjMessage()
//|	Date		-	2/11/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Shows information on items when clicked or guild info (if any) for players
//o------------------------------------------------------------------------------------------------o
void CSocket::ObjMessage( const std::string& txt, CBaseObject *getObj, R32 secsFromNow, UI16 Colour )
{
	UI16 targColour = Colour;

	if( txt.empty() )
		return;

	auto temp = txt;
	if( temp.size() >= 512 )
	{
		temp = txt.substr( 0, 512 );
	}
	CChar *mChar		= CurrcharObj();

	if( cwmWorldState->ServerData()->UseUnicodeMessages() )
	{
		CPUnicodeMessage unicodeMessage;
		unicodeMessage.Message( temp );
		unicodeMessage.Font( FNT_NORMAL );
		unicodeMessage.Colour( 0x0048 );
		unicodeMessage.Type( SYSTEM );
		unicodeMessage.Language( "ENG" );
		unicodeMessage.Name( "System" );
		unicodeMessage.ID( INVALIDID );
		unicodeMessage.Serial( getObj->GetSerial() );

		if( getObj->GetObjType() == OT_ITEM )
		{
			CItem *getItem = static_cast<CItem *>( getObj );
			if( getItem->IsCorpse() )
			{
				CChar *targChar = getItem->GetOwnerObj();
				if( ValidateObject( targChar ))
				{
					targColour = GetFlagColour( mChar, targChar );
				}
				else
				{
					UI08 flag = getItem->GetTempVar( CITV_MOREZ ); // Get flag, to help determine color of corpse
					switch( flag )
					{
						case 0x01:	targColour = 0x0026;	break;	// Murderer, red
						case 0x02:	// Criminal, gray
							[[fallthrough]];
						case 0x08:	targColour = 0x03B2;	break;	// Neutral, gray
						default:
						case 0x04:	targColour = 0x005A;	break;	// Innocent, blue
					}
				}
			}
		}

		if( targColour == 0x0 || targColour == 0x1700 )
		{
			unicodeMessage.Colour( 0x03B2 );
		}
		else
		{
			unicodeMessage.Colour( targColour );
		}

		Send( &unicodeMessage );
	}
	else
	{
		CSpeechEntry& toAdd = SpeechSys->Add();
		toAdd.Speech( temp );
		toAdd.Font( FNT_NORMAL );
		toAdd.Speaker( getObj->GetSerial() );
		toAdd.SpokenTo( mChar->GetSerial() );
		toAdd.Type( SYSTEM );
		toAdd.At( BuildTimeValue( secsFromNow ));
		toAdd.TargType( SPTRG_ONLYRECEIVER );

		if( getObj->GetObjType() == OT_ITEM )
		{
			CItem *getItem = static_cast<CItem *>( getObj );
			if( getItem->IsCorpse() )
			{
				CChar *targChar = getItem->GetOwnerObj();
				if( ValidateObject( targChar ))
				{
					targColour = GetFlagColour( mChar, targChar );
				}
				else
				{
					UI08 flag = getItem->GetTempVar( CITV_MOREZ ); // Get flag, to help determine color of corpse
					switch( flag )
					{
						case 0x01:	targColour = 0x0026;	break;	// Murderer, red
						case 0x02:	// Criminal, gray
							[[fallthrough]];
						case 0x08:	targColour = 0x03B2;	break;	// Neutral, gray
						default:
						case 0x04:	targColour = 0x005A;	break;	// Innocent, blue
					}
				}
			}
		}

		if( targColour == 0x0 || targColour == 0x1700)
		{
			toAdd.Colour( 0x03B2 );
		}
		else
		{
			toAdd.Colour( targColour );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::ShowCharName()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Shows character name/text on single click (and via all-names macro)
//o------------------------------------------------------------------------------------------------o
void CSocket::ShowCharName( CChar *i, bool showSer )
{
	UI08 a1 = i->GetSerial( 1 );
	UI08 a2 = i->GetSerial( 2 );
	UI08 a3 = i->GetSerial( 3 );
	UI08 a4 = i->GetSerial( 4 );
	std::string charName = GetNpcDictName( i, this );
	CChar *mChar = CurrcharObj();
	if( mChar->GetSingClickSer() || showSer )
	{
		ObjMessage( 1737, i, 0.0f, 0x03B2, a1, a2, a3, a4 );
	}
	if( !i->IsNpc() )
	{
		if( i->GetSquelched() )
		{
			ObjMessage( 1736, i );
		}
		if( i->GetCommandLevel() < CL_CNS && i->GetFame() >= 10000 )	//  only normal players have titles now
		{
			if( i->GetId( 2 ) == 0x91 )
			{
				charName = oldstrutil::format( Dictionary->GetEntry( 1740, Language() ).c_str(), charName.c_str() );	// Morrolan, added Lord/Lady to title overhead
			}
			else if( i->GetId( 1 ) == 0x90 )
			{
				charName = oldstrutil::format( Dictionary->GetEntry( 1739, Language() ).c_str(), charName.c_str() );
			}
		}
		if( cwmWorldState->ServerData()->ShowRaceWithName() && i->GetRace() != 0 && i->GetRace() != 65535 )	// need to check for placeholder race ( )
		{
			charName += " (";
			charName += Races->Name( i->GetRace() );
			charName += ")";
		}
		if( i->GetTownPriv() == 2 )
		{
			charName = oldstrutil::format( Dictionary->GetEntry( 1738, Language() ).c_str(), charName.c_str() );
		}
		if( !IsOnline(( *i )))
		{
			charName += " (OFF)";
		}
	}
	else
	{
		if( i->IsTamed() && ValidateObject( i->GetOwnerObj() ) && !cwmWorldState->creatures[i->GetId()].IsHuman() )
		{
			charName += " (tame) ";
		}

		// Show NPC title over their head?
		auto npcTitle = GetNpcDictTitle( i, this );
		if( cwmWorldState->ServerData()->ShowNpcTitlesOverhead() && npcTitle != "" )
		{
			charName += " " + npcTitle;
		}
	}

	// Show (invulnerable) tags over the heads of invulnerable characters?
	if( i->IsInvulnerable() && cwmWorldState->ServerData()->ShowInvulnerableTagOverhead() )
	{
		charName += " (invulnerable)";
	}
	if( i->IsFrozen() )
	{
		charName += " (frozen) ";
	}
	if( i->IsGuarded() )
	{
		charName += " (guarded)";
	}
	if( i->GetGuildNumber() != -1 && !i->IsIncognito() )
	{
		GuildSys->DisplayTitle( this, i );
	}

	if( cwmWorldState->ServerData()->UseUnicodeMessages() )
	{
		CPUnicodeMessage unicodeMessage;
		unicodeMessage.Message( charName );
		unicodeMessage.Font( FNT_NORMAL );
		unicodeMessage.Colour( GetFlagColour( mChar, i ));
		unicodeMessage.Type( SYSTEM );
		unicodeMessage.Language( "ENG" );
		unicodeMessage.Name( "System" );
		unicodeMessage.ID( INVALIDID );
		unicodeMessage.Serial( i->GetSerial() );
		Send( &unicodeMessage );
	}
	else
	{
		CSpeechEntry& toAdd = SpeechSys->Add();
		toAdd.Speech( charName );
		toAdd.Font( FNT_NORMAL );
		toAdd.Speaker( i->GetSerial() );
		toAdd.SpokenTo( mChar->GetSerial() );
		toAdd.Colour( GetFlagColour( mChar, i ));
		toAdd.Type( SYSTEM );
		toAdd.At( cwmWorldState->GetUICurrentTime() );
		toAdd.TargType( SPTRG_ONLYRECEIVER );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::GetFlagColour()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the current flag colour associated with the socket
//o------------------------------------------------------------------------------------------------o
COLOUR CSocket::GetFlagColour( CChar *src, CChar *trg )
{
	COLOUR retVal = 0x0058;
	switch( trg->FlagColour( src ))
	{
		case FC_INNOCENT:		retVal = 0x0059;		break;	// Blue
		case FC_NEUTRAL:
		case FC_CRIMINAL:
		default:				retVal = 0x03B2;		break;	// Gray
		case FC_MURDERER:		retVal = 0x0022;		break;	// Red
		case FC_FRIEND:			retVal = 0x003F;		break;	// Green
		case FC_ENEMY:			retVal = 0x0090;		break;	// Orange
		case FC_INVULNERABLE:	retVal = 0x0035;		break;	// Yellow
	}

	return retVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::SendTargetCursor()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Send targeting cursor to client, along with a system message
//o------------------------------------------------------------------------------------------------o
void CSocket::SendTargetCursor( UI08 targType, UI08 targId, const std::string& txt, UI08 cursorType )
{
	CPTargetCursor toSend;
	toSend.ID( CalcSerial( 0, 1, targType, targId ));
	toSend.Type( 1 );
	toSend.CursorType( cursorType );
	TargetOK( true );
	SysMessage( txt );
	Send( &toSend );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::SendTargetCursor()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Send targeting cursor to client, along with a dictionary-based system message
//o------------------------------------------------------------------------------------------------o
void CSocket::SendTargetCursor( UI08 targType, UI08 targId, UI08 cursorType, SI32 dictEntry, ... )
{
	std::string txt = Dictionary->GetEntry( dictEntry, Language() );
	if( txt.empty() )
		return;

	va_list argptr;
	va_start( argptr, dictEntry );
	auto msg = oldstrutil::format( txt, argptr );
	if( msg.size() > 512 )
	{
		msg = msg.substr( 0, 512 );
	}
	SendTargetCursor( targType, targId, msg, cursorType );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::mtarget()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Send a "multi" targeting cursor to client, with preview for placement of multi
//o------------------------------------------------------------------------------------------------o
void CSocket::mtarget( UI16 itemId, SI32 dictEntry )
{
	std::string txt = Dictionary->GetEntry( dictEntry, Language() );
	if( txt.empty() )
		return;

	CPMultiPlacementView toSend( CalcSerial( 0, 1, 0, TARGET_BUILDHOUSE ));
	toSend.MultiModel( itemId );
	toSend.RequestType( 1 );

	if( ClientVersion() >= CV_HS2D && ClientVerShort() >= CVS_7090 )
	{
		toSend.SetHue( 0 );
	}

	SysMessage( txt.c_str() );
	TargetOK( true );
	Send( &toSend );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::ReceivedVersion()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether server has received a client version from the client already
//o------------------------------------------------------------------------------------------------o
bool CSocket::ReceivedVersion( void ) const
{
	return receivedVersion;
}
void CSocket::ReceivedVersion( bool value )
{
	receivedVersion = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::BytesSent()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the amount of bytes sent to socket
//o------------------------------------------------------------------------------------------------o
UI32 CSocket::BytesSent( void ) const
{
	return bytesSent;
}
void CSocket::BytesSent( UI32 newValue )
{
	bytesSent = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::BytesReceived()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the amount of bytes received from socket
//o------------------------------------------------------------------------------------------------o
UI32 CSocket::BytesReceived( void ) const
{
	return bytesReceived;
}
void CSocket::BytesReceived( UI32 newValue )
{
	bytesReceived = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::BytesReceivedWarning()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets warning count for player having exceeded received bytes cap
//o------------------------------------------------------------------------------------------------o
UI16 CSocket::BytesReceivedWarning( void ) const
{
	return bytesRecvWarningCount;
}
void CSocket::BytesReceivedWarning( UI16 newValue )
{
	bytesRecvWarningCount = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::BytesSentWarning()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets warning count for player having exceeded sent bytes cap
//o------------------------------------------------------------------------------------------------o
UI16 CSocket::BytesSentWarning( void ) const
{
	return bytesSentWarningCount;
}
void CSocket::BytesSentWarning( UI16 newValue )
{
	bytesSentWarningCount = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::StatWindow()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens the status window/health bar for characters 
//|					and/or items/multis with damageable flag set
//o------------------------------------------------------------------------------------------------o
void CSocket::StatWindow( CBaseObject *targObj, bool updateParty )
{
	if( !ValidateObject( targObj ))
		return;

	if( !LoginComplete() )
		return;

	CChar *mChar = CurrcharObj();

	if( targObj->CanBeObjType(OT_CHAR) )
	{
		// Character specific
		CChar *targChar = static_cast<CChar*>( targObj );

		if( mChar->GetCommandLevel() < CL_CNS && (( mChar != targChar && targChar->GetVisible() != VT_VISIBLE )
			|| ( !targChar->IsNpc() && !IsOnline( *targChar ))))
			return;

		if( !CharInRange( mChar, targChar ))
			return;

		if( ClientType() >= CV_SA2D )
		{
			// Send poison state of healthbar
			CPHealthBarStatus hpBarStatus1(( *targChar ), (( *this )), 1 );
			Send( &hpBarStatus1 );

			// Send invulnerable state of healthbar
			CPHealthBarStatus hpBarStatus2(( *targChar ), (( *this )), 2 );
			Send( &hpBarStatus2 );
		}

		CPStatWindow toSend(( *targObj ), ( *this ));

		// 9/17/01 : fixed bug of your name on your own stat window
		toSend.NameChange( mChar != targChar && ( mChar->GetCommandLevel() >= CL_GM || targChar->GetOwnerObj() == mChar ));
		if( !targChar->IsNpc() && mChar == targChar )
		{
			toSend.Gold( GetItemAmount( targChar, 0x0EED ));
			toSend.AC( Combat->CalcDef( targChar, 0, false ));
		}

		Send( &toSend );

		if( !targChar->IsNpc() && mChar == targChar )
		{
			CPExtendedStats exStats(( *targChar ));
			Send( &exStats );
		}

		if( !updateParty )
			return;

		// If targObj is a party member, also send their updated mana and stamina!
		Party * myParty = PartyFactory::GetSingleton().Get( mChar );
		if( myParty != nullptr && IsOnline( *mChar ))
		{
			// Fetch list of party members
			std::vector<CPartyEntry *> *mList = myParty->MemberList();
			if( mList != nullptr )
			{
				for( size_t j = 0; j < mList->size(); ++j )
				{
					CPartyEntry *mEntry = ( *mList )[j];
					CChar *partyMember = mEntry->Member();

					if( !IsOnline( *partyMember ) || !IsOnline( *mChar) || partyMember->GetSerial() == mChar->GetSerial() )
						continue;

					// If partyMember is online, send their info to each other
					if( targObj->GetSerial() == partyMember->GetSerial() )
					{
						CSocket *s = partyMember->GetSocket();
						if( s == nullptr )
							break;

						// Send stat window update for new member to existing party members
						s->StatWindow( mChar, false );

						// Prepare the stat update packet for new member to existing party members
						CPUpdateStat toSendHp(( *mChar ), 0, true );
						s->Send( &toSendHp );
						CPUpdateStat toSendMana(( *mChar ), 1, true );
						s->Send( &toSendMana );
						CPUpdateStat toSendStam(( *mChar ), 2, true );
						s->Send( &toSendStam );

						// Also send info on the existing party member to the new member!
						// Send stat window update packet for existing member to new party member
						StatWindow( partyMember, false );

						// Prepare the stat update packet for existing member to new party members
						CPUpdateStat toSendHp2(( *partyMember ), 0, true );
						Send( &toSendHp2 );
						CPUpdateStat toSendMana2(( *partyMember ), 1, true );
						Send( &toSendMana2 );
						CPUpdateStat toSendStam2(( *partyMember ), 2, true );
						Send( &toSendStam2 );
						break;
					}
				}
			}
		}
	}
	else
	{
		// Item specific
		SI16 visRange = MAX_VISRANGE;
		if( mChar->GetSocket() != nullptr )
		{
			visRange = mChar->GetSocket()->Range() + Races->VisRange( mChar->GetRace() );
		}
		else
		{
			visRange += Races->VisRange( mChar->GetRace() );
		}

		if( mChar->GetCommandLevel() < CL_CNS &&
			( targObj->GetVisible() != VT_VISIBLE || !ObjInRange( mChar, targObj, static_cast<UI16>( visRange ))))
			return;

		CPStatWindow toSend(( *targObj ), ( *this ));
		Send( &toSend );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::UpdateSkill()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Update a certain skill
//o------------------------------------------------------------------------------------------------o
void CSocket::UpdateSkill( UI08 skillnum )
{
	CChar *mChar = CurrcharObj();
	CPUpdIndSkill toSend(( *mChar ), static_cast<UI08>( skillnum ));
	Send( &toSend );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::OpenPack()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Open backpack and display contents
//o------------------------------------------------------------------------------------------------o
void CSocket::OpenPack( CItem *i, bool isPlayerVendor )
{
	if( !ValidateObject( i ))
	{
		Console.Warning( "OpenPack() was passed an invalid item" );
		return;
	}
	CPDrawContainer contSend = ( *i );
	contSend.Model( 0x3C );

	if( i->GetId( 1 ) == 0x3E )            // boats
	{
		contSend.Model( 0x4C );
	}
	else
	{
		switch( Items->GetPackType( i ))
		{
			case PT_COFFIN:
				contSend.Model( 0x09 );
				break;
			case PT_PACK:
			case PT_PACK2:
				contSend.Model( 0x3C );
				break;
			case PT_BAG:
				contSend.Model( 0x3D );
				break;
			case PT_BARREL:
				contSend.Model( 0x3E );
				break;
			case PT_SQBASKET:
				contSend.Model( 0x3F );
				break;
			case PT_RBASKET:
				contSend.Model( 0x41 );
				break;
			case PT_GCHEST:
				contSend.Model( 0x42 );
				break;
			case PT_WBOX:
				contSend.Model( 0x43 );
				break;
			case PT_CRATE:
				contSend.Model( 0x44 );
				break;
			case PT_SHOPPACK:
				contSend.Model( 0x47 );
				break;
			case PT_DRAWER:
				contSend.Model( 0x48 );
				break;
			case PT_WCHEST:
				contSend.Model( 0x49 );
				break;
			case PT_MBOX:
				contSend.Model( 0x4B );
				break;
			case PT_SCHEST:
			case PT_BANK:
				contSend.Model( 0x4A );
				break;
			case PT_BOOKCASE:
				contSend.Model( 0x4D );
				break;
			case PT_FARMOIRE:
				contSend.Model( 0x4E );
				break;
			case PT_WARMOIRE:
				contSend.Model( 0x4F );
				break;
			case PT_DRESSER:
				contSend.Model( 0x51 );
				break;
			case PT_STOCKING:
				contSend.Model( 0x103 );
				break;
			case PT_GIFTBOX1:
				contSend.Model( 0x102 );
				break;
			case PT_GIFTBOX2:
				contSend.Model( 0x11E );
				break;
			case PT_GIFTBOX3:
				contSend.Model( 0x11B );
				break;
			case PT_GIFTBOX4:
				contSend.Model( 0x11C );
				break;
			case PT_GIFTBOX5:
				contSend.Model( 0x11D );
				break;
			case PT_GIFTBOX6:
				contSend.Model( 0x11F );
				break;
			case PT_SEBASKET:
				contSend.Model( 0x108 );
				break;
			case PT_SEARMOIRE1:
				contSend.Model( 0x105 );
				break;
			case PT_SEARMOIRE2:
				contSend.Model( 0x106 );
				break;
			case PT_SEARMOIRE3:
				contSend.Model( 0x107 );
				break;
			case PT_SECHEST1:
				contSend.Model( 0x109 );
				break;
			case PT_SECHEST2:
				contSend.Model( 0x10B );
				break;
			case PT_SECHEST3:
				contSend.Model( 0x10A );
				break;
			case PT_SECHEST4:
				contSend.Model( 0x10C );
				break;
			case PT_SECHEST5:
				contSend.Model( 0x10D );
				break;
			case PT_GAME_BACKGAMMON:
				contSend.Model( 0x92E ); // Backgammon board
				break;
			case PT_GAME_CHESS:
				contSend.Model( 0x91A ); // Chess board
				break;
			case PT_MAILBOX1:
				contSend.Model( 0x6D3 ); // Dolphin mailbox
				break;
			case PT_MAILBOX2:
				contSend.Model( 0x6D4 ); // Squirrel mailbox
				break;
			case PT_MAILBOX3:
				contSend.Model( 0x6D5 ); // Barrel mailbox
				break;
			case PT_MAILBOX4:
				contSend.Model( 0x6D6 ); // Light mailbox
				break;
			case PT_MAILBOX5:
				contSend.Model( 0x9D37 ); // Sitting kitten mailbox
				break;
			case PT_MAILBOX6:
				contSend.Model( 0x9D38 ); // Standing kitten mailbox
				break;
			case PT_MAILBOX7:
				contSend.Model( 0x9D39 ); // Scarecrow mailbox
				break;
			case PT_MAILBOX8:
				contSend.Model( 0x9D3A ); // Lion mailbox
				break;
			case PT_MAILBOX9:
				contSend.Model( 0x11A ); // Square gray mailbox
				break;
			case PT_UNKNOWN:
				Console.Warning( oldstrutil::format( "OpenPack() passed an invalid container type: 0x%X", i->GetSerial() ));
				return;
		}
	}
	if( ClientType() >= CV_HS2D && ClientVerShort() >= CVS_7090 )
	{
		contSend.ContType( 0x7D );
	}

	Send( &contSend );
	CPItemsInContainer itemsIn( this, i, 0x0, isPlayerVendor );
	Send( &itemsIn );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::OpenBank( CChar *i )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens players bank box
//o------------------------------------------------------------------------------------------------o
void CSocket::OpenBank( CChar *i )
{
	CItem *bankBox = nullptr;
	bankBox = i->GetItemAtLayer( IL_BANKBOX );
	if( ValidateObject( bankBox ))
	{
		if( bankBox->GetType() == IT_CONTAINER && bankBox->GetTempVar( CITV_MOREX ) == 1 )
		{
			CPWornItem toWearO = ( *bankBox );
			Send( &toWearO );
			OpenPack( bankBox );
			return;
		}
	}
	
	// No bankbox was found, so let's create one!
	auto temp = oldstrutil::format( 1024, Dictionary->GetEntry( 1283 ).c_str(), i->GetName().c_str() ); // %s's bank box.
	bankBox = Items->CreateItem( nullptr, i, 0x09AB, 1, 0, OT_ITEM );
	bankBox->SetName( temp );
	bankBox->SetLayer( IL_BANKBOX );
	bankBox->SetOwner( i );
	bankBox->SetDecayable( false );
	bankBox->SetMaxItems( cwmWorldState->ServerData()->MaxPlayerBankItems() );
	if( !bankBox->SetCont( i ))
		return;

	bankBox->SetTempVar( CITV_MOREX, 1 );
	bankBox->SetType( IT_CONTAINER );

	// Make another attempt to open bank box
	CPWornItem toWear = ( *bankBox );
	Send( &toWear );
	OpenPack( bankBox );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::GetTimer()
//|					CSocket::SetTimer()
//|	Date		-	September 25, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets temporary Timer values associated with character connected to the socket
//o------------------------------------------------------------------------------------------------o
TIMERVAL CSocket::GetTimer( cS_TID timerId ) const
{
	TIMERVAL rValue = 0;
	if( timerId != tPC_COUNT )
	{
		rValue = pcTimers[timerId];
	}
	return rValue;
}
void CSocket::SetTimer( cS_TID timerId, TIMERVAL value )
{
	if( timerId != tPC_COUNT )
	{
		pcTimers[timerId] = value;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::ClearTimers()
//|	Date		-	September 25, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Resets all timers (but mutetime) to 0 used when player logs out
//o------------------------------------------------------------------------------------------------o
void CSocket::ClearTimers( void )
{
	for( SI32 mTID = static_cast<SI32>( tPC_SKILLDELAY ); mTID < static_cast<SI32>( tPC_COUNT ); ++mTID )
	{
		if( mTID != tPC_MUTETIME )
		{
			pcTimers[mTID] = 0;
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket::OpenURL()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Launch a webpage from within the client
//o------------------------------------------------------------------------------------------------o
void CSocket::OpenURL( const std::string& txt )
{
	SysMessage( 1210 ); // Launching your web browser. Please wait...
	CPWebLaunch toSend( txt );
	Send( &toSend );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CPUOXBuffer::CPUOXBuffer()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Constructor for CPUOXBuffer class
//o------------------------------------------------------------------------------------------------o
CPUOXBuffer::CPUOXBuffer()
{
	InternalReset();
}

CPUOXBuffer::CPUOXBuffer( CPUOXBuffer *initBuffer )
{
	InternalReset();
	pStream.ReserveSize( initBuffer->GetPacketStream().GetSize() );
	pStream.WriteArray( 0, initBuffer->GetPacketStream().GetBuffer(), initBuffer->GetPacketStream().GetSize() );
}

CPUOXBuffer::~CPUOXBuffer()
{
	packedBuffer.resize( 0 );
}

CPUOXBuffer &CPUOXBuffer::operator=( CPUOXBuffer &copyFrom )
{
	size_t packetSize = copyFrom.GetPacketStream().GetSize();
	pStream.ReserveSize( packetSize );
	pStream.WriteArray( 0, copyFrom.GetPacketStream().GetBuffer(), packetSize );
	return ( *this );
}

void CPUOXBuffer::InternalReset( void )
{
	packedBuffer.resize( 0 );
	isPacked				=	false;
	packedLength			=	0;
}

const UI08 *CPUOXBuffer::PackedPointer( void ) const
{
	return ( const UI08 * )&packedBuffer[0];
}

bool CPUOXBuffer::ClientCanReceive( [[maybe_unused]] CSocket *mSock )
{
	// Default implementation, all clients can receive all packets
	return true;
}

UI32 CPUOXBuffer::Pack( void )
{
	if( isPacked )
		return packedLength;

	packedBuffer.resize( pStream.GetSize() * 2 );

	size_t len	= pStream.GetSize();

	UI08 *pIn	= ( UI08 * )pStream.GetBuffer();
	UI08 *pOut	= ( UI08 * )&packedBuffer[0];

	isPacked	= true;

	packedLength = DoPack( pIn, pOut, static_cast<SI32>( len ));
	return packedLength;
}

ByteBuffer_t& CPUOXBuffer::GetPacketStream()
{
	return pStream;
}

UI32 CPUOXBuffer::PackedLength( void ) const
{
	return packedLength;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CPUOXBuffer::Log()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Log sent packets to log file
//o------------------------------------------------------------------------------------------------o
void CPUOXBuffer::Log( std::ofstream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[SEND]Packet: 0x" << ( pStream.GetByte( 0 ) < 16 ? "0" : "" ) << std::hex << static_cast<UI16>( pStream.GetByte( 0 )) << "--> Length:" << std::dec << pStream.GetSize() << TimeStamp() << std::endl;
	}
	DoPacketLogging( outStream, pStream.GetSize(), pStream.GetBuffer() );
}

CPInputBuffer::CPInputBuffer() : tSock( nullptr )
{
}
CPInputBuffer::CPInputBuffer( CSocket *input )
{
	SetSocket( input );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CPInputBuffer::Log()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Log received packets to log file
//o------------------------------------------------------------------------------------------------o
void CPInputBuffer::Log( std::ofstream &outStream, bool fullHeader )
{
	UI08 *buffer	= tSock->Buffer();
	const UI32 len	= tSock->InLength();
	if( fullHeader )
	{
		outStream << "[RECV]Packet Class Generic: 0x" << std::hex << ( buffer[0] < 10 ? "0" : "" ) << static_cast<UI16>( buffer[0] ) << " --> Length: " << std::dec << len << TimeStamp() << std::endl;
	}
	DoPacketLogging( outStream, len, buffer );
}

bool CPInputBuffer::Handle( void )
{
	return false;
}

void CPInputBuffer::SetSocket( CSocket *toSet )
{
	tSock = toSet;
}

