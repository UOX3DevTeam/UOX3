#include "uox3.h"
#include "CPacketSend.h"
#include "xgm.h"
#include "townregion.h"
#include "cWeather.hpp"
#include "cRaces.h"
#include "PageVector.h"

#include "ObjectFactory.h"

namespace UOX
{

void cPXGMLoginResponse::InternalReset( void )
{
	internalBuffer.resize( 3 );
	internalBuffer[0] = 0;
}

cPXGMLoginResponse::cPXGMLoginResponse()
{
	InternalReset();
}

cPXGMLoginResponse::cPXGMLoginResponse( UI08 error, UI08 clearance )
{
	InternalReset();
	Error( error );
	Clearance( clearance );
}

void cPXGMLoginResponse::Error( UI08 error )
{
	internalBuffer[1] = error;
}

void cPXGMLoginResponse::Clearance( UI08 clearance )
{
	internalBuffer[2] = clearance;
}

cPIXGMLogin::cPIXGMLogin()
{
	InternalReset();
}

cPIXGMLogin::cPIXGMLogin( cSocket *s ) : cPInputBuffer( s )
{
	InternalReset();
	Receive();
}

void cPIXGMLogin::Receive( void )
{
	tSock->Receive( 62, false );
}

void cPIXGMLogin::InternalReset( void )
{
}

const char *cPIXGMLogin::Name( void )
{
	return (const char *)&(tSock->Buffer()[1]);
}
const char *cPIXGMLogin::Pass( void )
{
	return (const char *)&(tSock->Buffer()[31]);
}

UI08 cPIXGMLogin::Clearance( void )
{
	return tSock->Buffer()[61];
}

bool cPIXGMLogin::Handle( void )
{
	//Do real authentication here, not false acceptance" )
	cPXGMLoginResponse toSend( 0, Clearance() );
	tSock->Send( &toSend );
	tSock->FirstPacket( false );
	return true;
}

void cPXGMServerStatResponse::InternalReset( void )
{
	internalBuffer.resize( 2 );
	internalBuffer[0] = 0x12;
}
cPXGMServerStatResponse::cPXGMServerStatResponse()
{
	InternalReset();
}
cPXGMServerStatResponse::cPXGMServerStatResponse( XGMServerStat error )
{
	InternalReset();
	ServerStat( error );
}

cPXGMServerStatResponse::~cPXGMServerStatResponse()
{
}

void cPXGMServerStatResponse::ServerStat( XGMServerStat error )
{
	internalBuffer[1] = (UI08)error;
	switch( error )
	{
		case XSS_ONLINECOUNT:
		case XSS_CHARACTERCOUNT:
		case XSS_ITEMCOUNT:
		case XSS_ACCOUNTCOUNT:
		case XSS_SIMULATIONCYCLES:
		case XSS_RACECOUNT:
		case XSS_REGIONCOUNT:
		case XSS_WEATHERCOUNT:
		case XSS_TIMESINCERESTART:
		case XSS_TIMESINCEWORLDSAVE:
		case XSS_GMPAGECOUNT:
		case XSS_CNSPAGECOUNT:
		case XSS_GHOSTCOUNT:
		case XSS_MURDERERCOUNT:
		case XSS_BLUECOUNT:
		case XSS_HOUSECOUNT:
		case XSS_TOTALGOLD:
		case XSS_PEAKCONNECTION:
		case XSS_GMSLOGGEDIN:
		case XSS_CNSLOGGEDIN:
			internalBuffer.resize( 6 );
			break;
		case XSS_LASTPLAYERLOGGEDIN:
			internalBuffer.resize( 6 );
			break;
		case XSS_PINGRESPONSE:
			internalBuffer.resize( 6 );
			break;
	}
}
void cPXGMServerStatResponse::GenericData( UI32 generic )
{
	switch( (XGMServerStat)internalBuffer[1] )
	{
		case XSS_ONLINECOUNT:
		case XSS_CHARACTERCOUNT:
		case XSS_ITEMCOUNT:
		case XSS_ACCOUNTCOUNT:
		case XSS_SIMULATIONCYCLES:
		case XSS_RACECOUNT:
		case XSS_REGIONCOUNT:
		case XSS_WEATHERCOUNT:
		case XSS_TIMESINCERESTART:
		case XSS_TIMESINCEWORLDSAVE:
		case XSS_GMPAGECOUNT:
		case XSS_CNSPAGECOUNT:
		case XSS_GHOSTCOUNT:
		case XSS_MURDERERCOUNT:
		case XSS_BLUECOUNT:
		case XSS_HOUSECOUNT:
		case XSS_TOTALGOLD:
		case XSS_PEAKCONNECTION:
		case XSS_GMSLOGGEDIN:
		case XSS_CNSLOGGEDIN:
		case XSS_LASTPLAYERLOGGEDIN:
		case XSS_PINGRESPONSE:
			internalBuffer[2] = (UI08)(generic>>24);
			internalBuffer[3] = (UI08)(generic>>16);
			internalBuffer[4] = (UI08)(generic>>8);
			internalBuffer[5] = (UI08)(generic%256);
			break;
		default:
			break;
	}
}

cPIXGMServerStat::cPIXGMServerStat()
{
	InternalReset();
}

cPIXGMServerStat::~cPIXGMServerStat()
{
}

cPIXGMServerStat::cPIXGMServerStat( cSocket *s ) : cPInputBuffer( s )
{
	InternalReset();
	Receive();
}

void cPIXGMServerStat::Receive( void )
{
	tSock->Receive( 2, false );
}

void cPIXGMServerStat::InternalReset( void )
{
}

XGMServerStat cPIXGMServerStat::Stat( void )
{
	return (XGMServerStat)tSock->Buffer()[1];
}

//o--------------------------------------------------------------------------o
//|	Function		-	inline UI32 GhostCount( void )
//|	Date			-	
//|	Developers		-	Unknown / EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Create the ghost count(?), and return this count to the
//|									calling function.
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
inline UI32 GhostCount( void )
{
	UI32 sumDead = 0;
	MAPUSERNAMEID_ITERATOR I;
	for(I=Accounts->begin();I!=Accounts->end();++I)
	{
		ACCOUNTSBLOCK& actbTemp = I->second;
		//
		if( actbTemp.wAccountIndex == AB_INVALID_ID )
			break;
		for( UI08 iCtr = 0; iCtr < 6; ++iCtr )
		{
			if( actbTemp.lpCharacters[iCtr] != NULL )
			{
				if( actbTemp.lpCharacters[iCtr]->IsDead() )
					++sumDead;
			}
		}
	}
	return sumDead;
}

//o--------------------------------------------------------------------------o
//|	Function		-	inline UI32 MurdererCount( void )
//|	Date			-	
//|	Developers		-	Unknown / EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Return the current character/npc murder count
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
inline UI32 MurdererCount( void )
{
	UI32 sumMurderer = 0;
	MAPUSERNAMEID_ITERATOR I;
	for( I = Accounts->begin(); I != Accounts->end(); ++I )
	{
		ACCOUNTSBLOCK& actbBlock = I->second;
		//
		if( actbBlock.wAccountIndex == AB_INVALID_ID )
			continue;
		//
		for( UI08 iCtr = 0; iCtr < 6; ++iCtr )
		{
			if( actbBlock.lpCharacters[iCtr] != NULL )
			{
				if( actbBlock.lpCharacters[iCtr]->IsMurderer() )
					++sumMurderer;
			}
		}
	}
	return sumMurderer;
}

//o--------------------------------------------------------------------------o
//|	Function		-	inline UI32 BlueCount( void )
//|	Date			-	
//|	Developers		-	Unknown / EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Return the current total count of all innocent character
//|									that are currently assigned to an account.
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
inline UI32 BlueCount( void )
{
	UI32 sumBlue = 0;
	MAPUSERNAMEID_ITERATOR I;
	//
	for( I = Accounts->begin(); I != Accounts->end(); ++I )
	{
		ACCOUNTSBLOCK& actbBlock = I->second;

		if( actbBlock.wAccountIndex == AB_INVALID_ID )
			continue;
		for( UI08 iCtr = 0; iCtr < 6; ++iCtr )
		{
			if( actbBlock.lpCharacters[iCtr] != NULL )
			{
				if( actbBlock.lpCharacters[iCtr]->IsInnocent() )
					++sumBlue;
			}
		}
	}
	return sumBlue;
}

bool GMLoggedInStub( CChar *toCheck )
{
	return ( toCheck->IsGM() || toCheck->GetCommandLevel() >= GM_CMDLEVEL );
}

bool CnsLoggedInStub( CChar *toCheck )
{
	return ( toCheck->IsCounselor() || ( toCheck->GetCommandLevel() >= CNS_CMDLEVEL && toCheck->GetCommandLevel() < GM_CMDLEVEL ) );
}

inline UI32 CharCountLoop( bool (*trgFunc)( CChar *toCheck ) )
{
	UI32 sumDead = 0;
	Network->PushConn();
	for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
	{
		if( tSock == NULL )
			continue;
		CChar *mChar = tSock->CurrcharObj();
		if( ValidateObject( mChar ) )
		{
			if( (*trgFunc)( mChar ) )
				++sumDead;
		}
	}
	Network->PopConn();
	return sumDead;
}

bool cPIXGMServerStat::Handle( void )
{
	cPXGMServerStatResponse toSend;
	toSend.ServerStat( Stat() );
	
	//HOUSECOUNT, TOTALGOLD, and LASTPLAYERLOGGEDIN need implementation" )
	switch( Stat() )
	{
		case XSS_ONLINECOUNT:			toSend.GenericData( cwmWorldState->GetPlayersOnline() );	break;
		case XSS_CHARACTERCOUNT:		toSend.GenericData( ObjectFactory::getSingleton().CountOfObjects( OT_CHAR ) );	break;
		case XSS_ITEMCOUNT:				toSend.GenericData( ObjectFactory::getSingleton().CountOfObjects( OT_ITEM ) );	break;
		case XSS_ACCOUNTCOUNT:			toSend.GenericData( Accounts->size() );	break;
		case XSS_SIMULATIONCYCLES:
					if( !( cwmWorldState->ServerProfile()->LoopTime() < 0.00000000001f || cwmWorldState->ServerProfile()->LoopTimeCount() < 0.00000000001f ) )
						toSend.GenericData( (unsigned long)(1000.0*(1.0/(R32)((R32)cwmWorldState->ServerProfile()->LoopTime()/(R32)cwmWorldState->ServerProfile()->LoopTimeCount()))) );
					else 
						toSend.GenericData( 0xFFFFFFFF );
					break;
		case XSS_RACECOUNT:				toSend.GenericData( Races->Count() );		break;
		case XSS_REGIONCOUNT:			toSend.GenericData( 256 );					break;
		case XSS_WEATHERCOUNT:			toSend.GenericData( Weather->Count() );		break;
		case XSS_TIMESINCERESTART:
		case XSS_TIMESINCEWORLDSAVE:	toSend.GenericData( (UI32)(difftime( time( NULL ), cwmWorldState->GetOldTime() )) );	break;
		case XSS_GMPAGECOUNT:			toSend.GenericData( GMQueue->NumEntries() );				break;
		case XSS_CNSPAGECOUNT:			toSend.GenericData( CounselorQueue->NumEntries() );			break;
		case XSS_GHOSTCOUNT:			toSend.GenericData( GhostCount() );			break;
		case XSS_MURDERERCOUNT:			toSend.GenericData( MurdererCount() );		break;
		case XSS_BLUECOUNT:				toSend.GenericData( BlueCount() );			break;
		case XSS_HOUSECOUNT:														break;		// unimplemented
		case XSS_TOTALGOLD:															break;		// unimplemented
		case XSS_PEAKCONNECTION:		toSend.GenericData( Network->PeakConnectionCount() );		break;
		case XSS_GMSLOGGEDIN:			toSend.GenericData( CharCountLoop( GMLoggedInStub ) );		break;
		case XSS_CNSLOGGEDIN:			toSend.GenericData( CharCountLoop( CnsLoggedInStub ) );		break;
		case XSS_LASTPLAYERLOGGEDIN:												break;		// unimplemented
		case XSS_PINGRESPONSE:			toSend.GenericData( time( NULL ) );			break;
	}
	tSock->Send( &toSend );
	return true;
}

void cPIXGMClientVer::InternalReset( void )
{
	internalBuffer.resize( 5 );
}
cPIXGMClientVer::cPIXGMClientVer()
{
	InternalReset();
}
cPIXGMClientVer::cPIXGMClientVer( cSocket *s ) : cPInputBuffer( s )
{
	InternalReset();
	Receive();
}
void cPIXGMClientVer::Receive( void )
{
	tSock->Receive( 5, false );
}	
bool cPIXGMClientVer::Handle( void )
{
	tSock->ClientVersion( tSock->GetDWord( 1 ) );
	return true;
}

cPIXGMServerVer::cPIXGMServerVer()
{
}
cPIXGMServerVer::cPIXGMServerVer( cSocket *s ) : cPInputBuffer( s )
{
	Receive();
}
void cPIXGMServerVer::Receive( void )
{
	tSock->Receive( 1, false );
}	
bool cPIXGMServerVer::Handle( void )
{
	cPXGMServerVerResponse toSend( XGMVersion );
	tSock->Send( &toSend );
	return true;
}

void cPXGMServerVerResponse::InternalReset( void )
{
	internalBuffer.resize( 5 );
	internalBuffer[0] = 0x0A;
}
cPXGMServerVerResponse::cPXGMServerVerResponse()
{
	InternalReset();
}
cPXGMServerVerResponse::cPXGMServerVerResponse( UI32 version )
{
	InternalReset();
	Version( version );
}
void cPXGMServerVerResponse::Version( UI32 generic )
{
	internalBuffer[1] = (UI08)(generic>>24);
	internalBuffer[2] = (UI08)(generic>>16);
	internalBuffer[3] = (UI08)(generic>>8);
	internalBuffer[4] = (UI08)(generic%256);
}

cPIXGMWhoOnline::cPIXGMWhoOnline()
{
}
cPIXGMWhoOnline::cPIXGMWhoOnline( cSocket *s ) : cPInputBuffer( s )
{
	Receive();
}
void cPIXGMWhoOnline::Receive( void )
{
	tSock->Receive( 1, false );
}	
bool cPIXGMWhoOnline::Handle( void )
{
	cPXGMWhoResponse toSend;
	toSend.StatusType( tSock->GetByte( 0 ) - 2 );
	cSocket *toGet = NULL;
	//
	CHARLIST charListing;
	size_t i = 0;
	//
	MAPUSERNAMEID_ITERATOR I;

	cSocket *trgSock = NULL;
	switch( tSock->GetByte( 0 ) - 2 )
	{
		case 0:	// online
			toSend.NumEntries( static_cast<UI16>(cwmWorldState->GetPlayersOnline()) );
			Network->PushConn();
			for( toGet = Network->FirstSocket(); !Network->FinishedSockets(); toGet = Network->NextSocket() )
			{
				if( toGet == NULL )
					toSend.AddPlayer( INVALIDSERIAL, "UNKNOWN" );
				else
				{
					CChar *mChar = toGet->CurrcharObj();
					if( !ValidateObject( mChar ) )
						toSend.AddPlayer( INVALIDSERIAL, "UNKNOWN" );
					else
						toSend.AddPlayer( mChar->GetSerial(), mChar->GetName().c_str() );
				}
			}
			Network->PopConn();
			break;
		case 1:	// offline
		{
			for( I = Accounts->begin(); I != Accounts->end(); ++I )
			{
				ACCOUNTSBLOCK& actbBlock = I->second;
				//
				if( actbBlock.wAccountIndex==AB_INVALID_ID )
					continue;
				//
				for( UI08 iCtr = 0; iCtr < 6; ++iCtr )
				{
					if( actbBlock.lpCharacters[iCtr] != NULL && actbBlock.lpCharacters[iCtr]->GetSerial() != actbBlock.dwInGame )	// no players logging out
					{
						bool loopCont = true;
						Network->PushConn();
						for( trgSock = Network->FirstSocket(); !Network->FinishedSockets(); trgSock = Network->NextSocket() )
						{
							if( trgSock == NULL )
								continue;
							if( trgSock->CurrcharObj() == actbBlock.lpCharacters[iCtr] )
							{
								loopCont = false;
								break;
							}
						}
						Network->PopConn();
						if( loopCont )
							charListing.push_back( actbBlock.lpCharacters[iCtr] );
					}
				}
			}
			toSend.NumEntries( charListing.size() );
			for( CHARLIST_CITERATOR I = charListing.begin(); I != charListing.end(); ++I )
				toSend.AddPlayer( (*I)->GetSerial(), (*I)->GetName().c_str() );
			break;
		}
		case 2:	// logging
			for( I = Accounts->begin(); I != Accounts->end(); ++I )
			{
				ACCOUNTSBLOCK& actbBlock = I->second;
				//
				if( actbBlock.wAccountIndex == AB_INVALID_ID )
					continue;
				//
				for( UI08 iCtr = 0; iCtr < 6; ++iCtr )
				{
					if( actbBlock.lpCharacters[iCtr] != NULL && actbBlock.lpCharacters[iCtr]->GetSerial() == actbBlock.dwInGame )	// no players logging out
					{
						bool loopCont2 = true;
						Network->PushConn();
						for( trgSock = Network->FirstSocket(); !Network->FinishedSockets(); trgSock = Network->NextSocket() )
						{
							if( trgSock == NULL )
								continue;
							if( trgSock->CurrcharObj() == actbBlock.lpCharacters[iCtr] )
							{
								loopCont2 = false;
								break;
							}
						}
						Network->PopConn();
						if( loopCont2 )
							charListing.push_back( actbBlock.lpCharacters[iCtr] );
					}
				}
			}
			toSend.NumEntries( charListing.size() );
			for( i = 0; i < charListing.size(); ++i )
				toSend.AddPlayer( charListing[i]->GetSerial(), charListing[i]->GetName().c_str() );
			break;
	}
	tSock->Send( &toSend );
	return true;
}

//o---------------------------------------------------------------------------o
//|   Function    :  cPIXGMChangeLevel()
//|   Date        :  21 August 2001
//|   Programmer  :  JoeMC
//o---------------------------------------------------------------------------o
//|   Purpose     :  Default constructor
//o---------------------------------------------------------------------------o
cPIXGMChangeLevel::cPIXGMChangeLevel()
{
	InternalReset();
}

//o---------------------------------------------------------------------------o
//|   Function    :  cPIXGMChangeLevel( cSocket *s )
//|   Date        :  21 August 2001
//|   Programmer  :  JoeMC
//o---------------------------------------------------------------------------o
//|   Purpose     :  Constructor in which it does the receive it has to
//o---------------------------------------------------------------------------o
cPIXGMChangeLevel::cPIXGMChangeLevel( cSocket *s ) : cPInputBuffer( s )
{
	InternalReset();
	Receive();
}

//o---------------------------------------------------------------------------o
//|   Function    :  void Receive( cSocket *s )
//|   Date        :  21 August 2001
//|   Programmer  :  JoeMC
//o---------------------------------------------------------------------------o
//|   Purpose     :  Receives any data it needs to for processing
//o---------------------------------------------------------------------------o
void cPIXGMChangeLevel::Receive( void )
{
	tSock->Receive( 2, false );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void InternalReset( void )
//|   Date        :  21 August 2001
//|   Programmer  :  JoeMC
//o---------------------------------------------------------------------------o
//|   Purpose     :  Does any internal setup it needs to
//o---------------------------------------------------------------------------o
void cPIXGMChangeLevel::InternalReset( void )
{
}


//o---------------------------------------------------------------------------o
//|   Function    :  UI08 Clearance( void )
//|   Date        :  21 August 2001
//|   Programmer  :  JoeMC
//o---------------------------------------------------------------------------o
//|   Purpose     :  Returns the clearance level requested
//o---------------------------------------------------------------------------o
UI08 cPIXGMChangeLevel::Clearance( void )
{
	return tSock->Buffer()[1];
}

//o---------------------------------------------------------------------------o
//|   Function    :  bool Handle( void )
//|   Date        :  21 August 2001
//|   Programmer  :  JoeMC
//o---------------------------------------------------------------------------o
//|   Purpose     :  Returns true if the packet is successfully handled
//o---------------------------------------------------------------------------o
bool cPIXGMChangeLevel::Handle( void )
{
	cPXGMChangeLevelResponse toSend( 0, Clearance() );
	tSock->Send( &toSend );
	return true;
}

void cPIXGMShowQueueRequest::InternalReset( void )
{
}
cPIXGMShowQueueRequest::cPIXGMShowQueueRequest()
{
	InternalReset();
}
cPIXGMShowQueueRequest::cPIXGMShowQueueRequest( cSocket *s ) : cPInputBuffer( s )
{
	InternalReset();
	Receive();
}
void cPIXGMShowQueueRequest::Receive( void )
{
	tSock->Receive( 2, false );
}
UI08 cPIXGMShowQueueRequest::Queue( void ) const
{
	return tSock->GetByte( 1 );
}

bool cPIXGMShowQueueRequest::Handle( void )
{
	//Do queue request here" )
	return false;
}

void cPXGMWhoResponse::InternalReset( void )
{
	internalBuffer.resize( 4 );
	internalBuffer[0] = 0x02;
	playerNum = 0;
}

cPXGMWhoResponse::cPXGMWhoResponse()
{
	InternalReset();
}

void cPXGMWhoResponse::NumEntries( UI16 numPlayers )
{
	internalBuffer.resize( 4 + numPlayers * 35 );
	internalBuffer[1] = (UI08)(numPlayers>>8);
	internalBuffer[2] = (UI08)(numPlayers%256);
}

cPXGMWhoResponse::cPXGMWhoResponse( UI16 numPlayers )
{
	InternalReset();
	NumEntries( numPlayers );
}
void cPXGMWhoResponse::AddPlayer( SERIAL serToAdd, const char *toAdd )
{
	UI16 baseLocation = 4 + 35 * playerNum;	// find offset into array
	internalBuffer[baseLocation + 0] = (UI08)(serToAdd>>24);
	internalBuffer[baseLocation + 1] = (UI08)(serToAdd>>16);
	internalBuffer[baseLocation + 2] = (UI08)(serToAdd>>8);
	internalBuffer[baseLocation + 3] = (UI08)(serToAdd%256);
	memcpy( &internalBuffer[baseLocation + 4], toAdd, strlen( toAdd ) );
	++playerNum;
}

void cPXGMWhoResponse::StatusType( UI08 statusType )
{
	internalBuffer[3] = statusType;
}


//o---------------------------------------------------------------------------o
//|   Function    :  void InternalReset( void )
//|   Date        :  21 August 2001
//|   Programmer  :  JoeMC
//o---------------------------------------------------------------------------o
//|   Purpose     :  Sets up packet sizes and command byte for packet
//o---------------------------------------------------------------------------o
void cPXGMChangeLevelResponse::InternalReset( void )
{
	internalBuffer.resize( 3 );
	internalBuffer[0] = 1;
}

//o---------------------------------------------------------------------------o
//|   Function    :  cPXGMChangeLevelResponse()
//|   Date        :  21 August 2001
//|   Programmer  :  JoeMC
//o---------------------------------------------------------------------------o
//|   Purpose     :  Default constructor
//o---------------------------------------------------------------------------o
cPXGMChangeLevelResponse::cPXGMChangeLevelResponse()
{
	InternalReset();
}

//o---------------------------------------------------------------------------o
//|   Function    :  cPXGMChangeLevelResponse( UI08 error, UI08 clearance )
//|   Date        :  21 August 2001
//|   Programmer  :  JoeMC
//o---------------------------------------------------------------------------o
//|   Purpose     :  Constructor which accepts an error code and clearance value
//o---------------------------------------------------------------------------o
cPXGMChangeLevelResponse::cPXGMChangeLevelResponse( UI08 error, UI08 clearance )
{
	InternalReset();
	Error( error );
	Clearance( clearance );
}

//o---------------------------------------------------------------------------o
//|   Function    :  Error( UI08 error )
//|   Date        :  21 August 2001
//|   Programmer  :  JoeMC
//o---------------------------------------------------------------------------o
//|   Purpose     :  Sets the error code for the packet, signifying any problems
//|					 0 indicates all clear, 1 indicates failure
//o---------------------------------------------------------------------------o
void cPXGMChangeLevelResponse::Error( UI08 error )
{
	internalBuffer[1] = error;
}

//o---------------------------------------------------------------------------o
//|   Function    :  Clearance( UI08 clearance )
//|   Date        :  21 August 2001
//|   Programmer  :  JoeMC
//o---------------------------------------------------------------------------o
//|   Purpose     :  Sets the new accepted clearance (if any).  Ignored if error
//|					 is set
//|					 Valid values are:
//|					 0	Player
//|					 1	Counselor
//|					 2	GM
//|					 3	Admin
//o---------------------------------------------------------------------------o
void cPXGMChangeLevelResponse::Clearance( UI08 clearance )
{
	internalBuffer[2] = clearance;
}


//o---------------------------------------------------------------------------o
//|   Function    :  cPInputBuffer *WhichXGMPacket( UI08 packetID, cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Returns an instance of a particular packet's handler, if
//|					 a known handler exists.  Otherwise returns NULL
//o---------------------------------------------------------------------------o
cPInputBuffer *WhichXGMPacket( UI08 packetID, cSocket *s )
{
	switch( packetID )
	{
		case 0x00:	return new cPIXGMLogin( s );				// Login / authentication request
		case 0x02:	return new cPIXGMWhoOnline( s );			// Who's Online Request
		case 0x03:	return new cPIXGMWhoOnline( s );			// Who's Offline Request
		case 0x04:	return new cPIXGMWhoOnline( s );			// Who's Logging Out Request
		case 0x08:	return new cPIXGMClientVer( s );			// XGM Client Protocol Version
		case 0x09:	return new cPIXGMServerVer( s );			// XGM Server Protocol Version Request
		case 0x0F:	return new cPIXGMServerStat( s );			// Get Server Stats
		case 0x01:	return new cPIXGMChangeLevel( s );			// Access Level Change Request
		case 0x05:	return new cPIXGMShowQueueRequest( s );		// Show Queue Request
		case 0x06:												// Handle Queue Entry Request
		case 0x07:												// Account Command Request
		case 0x0A:												// Reload Script Request
		case 0x0B:												// Installed Scripts Request
		case 0x0C:												// Get Installed Script
		case 0x0D:												// Get Script Section Names Request
		case 0x0E:												// Get Script Section Request
		default:	return NULL;
	}
	return NULL;
}

}
