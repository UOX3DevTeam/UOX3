#include "uox3.h"
#include "network.h"
#include "movement.h"
#include "wholist.h"
#include "skills.h"
#include "cMagic.h"
#include "CJSMapping.h"
#include "cScript.h"
#include "cEffects.h"
#include "cThreadQueue.h"
#include "CPacketSend.h"
#include "scriptc.h"
#include "ssection.h"
#include "cServerDefinitions.h"
#include "Dictionary.h"
#include "mapstuff.h"

#include "ObjectFactory.h"
#include "StringUtility.hpp"

#if PLATFORM != WINDOWS
#include <sys/ioctl.h>
#endif


cNetworkStuff *Network = nullptr;

#define _DEBUG_PACKET	0

fd_set conn;
fd_set all;
fd_set errsock;

void killTrades( CChar *i );
void DoorMacro( CSocket *s );
void sysBroadcast( const std::string& txt );

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void ClearBuffers( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends ALL buffered data
//o-----------------------------------------------------------------------------------------------o
void cNetworkStuff::ClearBuffers( void )
{
	SOCKLIST_CITERATOR toClear;
	for( toClear = connClients.begin(); toClear != connClients.end(); ++toClear )
		(*toClear)->FlushBuffer();
	for( toClear = loggedInClients.begin(); toClear != loggedInClients.end(); ++toClear )
		(*toClear)->FlushBuffer();
}

//
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void setLastOn( CSocket *s )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the laston character member value to the current date/time
//o-----------------------------------------------------------------------------------------------o
void cNetworkStuff::setLastOn( CSocket *s )
{
	assert( s != nullptr );

	time_t ltime;
	time( &ltime );
	char *t = ctime( &ltime );

	// some ctime()s like to stick \r\n on the end, we don't want that
	size_t mLen = strlen( t );
	for( size_t end = mLen - 1; end >= 0 && isspace( t[end] ) && end < mLen; --end )
		t[end] = '\0';

	if( s->CurrcharObj() != nullptr )
	{
		s->CurrcharObj()->SetLastOn( t );
		s->CurrcharObj()->SetLastOnSecs( static_cast<UI32>(ltime) );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void Disconnect( UOXSOCKET s )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Forces player to disconnect/instalog
//o-----------------------------------------------------------------------------------------------o
void cNetworkStuff::Disconnect( UOXSOCKET s )
{
	setLastOn( connClients[s] );
	CChar *currChar = connClients[s]->CurrcharObj();
	Console << "Client " << (UI32)s << " disconnected. [Total:" << (SI32)(cwmWorldState->GetPlayersOnline()-1) << "]" << myendl;



	if( currChar != nullptr )
	{
		if( currChar->GetAccount().wAccountIndex == connClients[s]->AcctNo() && cwmWorldState->ServerData()->ServerJoinPartAnnouncementsStatus() )
		{

			sysBroadcast( strutil::format(1024, Dictionary->GetEntry( 752 ), currChar->GetName().c_str() ) );//message upon leaving a server
		}
	}
	if( connClients[s]->AcctNo() != AB_INVALID_ID )
	{
		CAccountBlock &actbAccount = connClients[s]->GetAccount();
		if( actbAccount.wAccountIndex != AB_INVALID_ID )
		{
			actbAccount.wFlags.set( AB_FLAGS_ONLINE, false );
		}
	}
	//Instalog
	if( currChar != nullptr )
	{
		if( !currChar->isFree() && connClients[s] != nullptr )
		{
			// October 6, 2002 - Support for the onLogout event.
			bool returnState = false;
			std::vector<UI16> scriptTriggers = currChar->GetScriptTriggers();
			for( auto i : scriptTriggers )
			{
				cScript *toExecute = JSMapping->GetScript( i );
				if( toExecute != nullptr )
				{
					if( toExecute->OnLogout(connClients[s], currChar ))
					{
						returnState = true;
					}
				}
			}

			if( !returnState )
			{
				// No individual scripts handling OnLogout were found - let's check global script!
				cScript *toExecute = JSMapping->GetScript( (UI16)0 );
				if( toExecute != nullptr )
				{
					toExecute->OnLogout(connClients[s],currChar);
				}
			}
			LogOut( connClients[s] );
		}
	}
	try
	{
		connClients[s]->AcctNo( AB_INVALID_ID );
		connClients[s]->IdleTimeout( -1 );
		connClients[s]->FlushBuffer();
		connClients[s]->ClearTimers();
		connClients[s]->CloseSocket();
		if( currConnIter != connClients.begin() && currConnIter >= ( connClients.begin() + s) )
			--currConnIter;
		for( size_t q = 0; q < connIteratorBackup.size(); ++q )
		{
			if( connIteratorBackup[q] != connClients.begin() && connIteratorBackup[q] >= (connClients.begin() + s) )
				--connIteratorBackup[q];
		}
		delete connClients[s];
		connClients.erase( connClients.begin() + s );
	}
	catch( ... )
	{
		Console << "| CATCH: Invalid connClients[] encountered. Ignoring." << myendl;
	}
	cwmWorldState->DecPlayersOnline();
	WhoList->FlagUpdate();
	OffList->FlagUpdate();
}


//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void LogOut( CSocket *s )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Process client logout, and process instalog settings
//|
//|	Changes		-	Date Unknown - Added multi checked to instalog processing
//o-----------------------------------------------------------------------------------------------o
void cNetworkStuff::LogOut( CSocket *s )
{
	// This would probably be a good place to do the disconnect
	CChar *p = s->CurrcharObj();
	bool valid = false;
	SI16 x = p->GetX(), y = p->GetY();
	UI08 world = p->WorldNumber();

	killTrades( p );

	if( p->GetCommandLevel() >= CL_CNS || p->GetAccount().wAccountIndex == 0 )
		valid = true;
	else
	{
		for( size_t a = 0; a < cwmWorldState->logoutLocs.size(); ++a )
		{
			if( cwmWorldState->logoutLocs[a].x1 <= x && cwmWorldState->logoutLocs[a].y1 <= y && cwmWorldState->logoutLocs[a].x2 >= x && cwmWorldState->logoutLocs[a].y2 >= y && cwmWorldState->logoutLocs[a].worldNum == world )
			{
				valid = true;
				break;
			}
		}
	}
	if( !valid )
	{
		CMultiObj *multi = nullptr;
		if( !ValidateObject( p->GetMultiObj() ) )
			multi = findMulti( p );
		else
			multi = static_cast< CMultiObj * >(p->GetMultiObj() );
		if( ValidateObject( multi ) )
		{
			if( multi->IsOwner( p ) )
				valid = true;
			else if( !p->IsGM() )
			{
				SI16 sx, sy, ex, ey;
				Map->MultiArea( multi, sx, sy, ex, ey );
				p->SetLocation( ex, ey + 1, p->GetZ() );
			}
		}
	}
	//If player is holding an item on the cursor, and it was picked up off the ground, drop it back to the ground
	if( s->GetCursorItem() )
	{
		if( s->PickupSpot() == PL_GROUND )
		{
			CItem * i = s->GetCursorItem();
			i->SetCont( nullptr );
			i->SetLocation( p->GetX(), p->GetY(), p->GetZ() );
		}
	}

	CAccountBlock& actbAccount = s->GetAccount();
	if( valid )
	{
		if( actbAccount.wAccountIndex != AB_INVALID_ID )
			actbAccount.dwInGame = INVALIDSERIAL;
		p->SetTimer( tPC_LOGOUT, 0 );
		s->ClearTimers();
	}
	else
	{
		if( actbAccount.wAccountIndex != AB_INVALID_ID )
			actbAccount.dwInGame = p->GetSerial();
		p->SetTimer( tPC_LOGOUT, cwmWorldState->ServerData()->SystemTimer( tSERVER_LOGINTIMEOUT ) );
	}
	s->LoginComplete( false );
	actbAccount.wFlags.set( AB_FLAGS_ONLINE, false );
	p->SetSocket( nullptr );
	p->SetLocation( p );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void sockInit( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Initializes and binds sockets during startup
//o-----------------------------------------------------------------------------------------------o
void cNetworkStuff::sockInit( void )
{
	SI32 bcode;

	cwmWorldState->SetKeepRun( true );
	cwmWorldState->SetError( false );

#if PLATFORM != WINDOWS
	SI32 on = 1;
#endif

	a_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( a_socket < 0 )
	{
		Console.error( " Unable to create socket");
#if PLATFORM == WINDOWS
		Console.error( strutil::format(" Code %i", WSAGetLastError()) );
#else
		Console << myendl;
#endif
		cwmWorldState->SetKeepRun( false );
		cwmWorldState->SetError( true );
		Shutdown( FATAL_UOX3_ALLOC_NETWORK );
		return;
	}
#if PLATFORM != WINDOWS
	[[maybe_unused]] int result = setsockopt( a_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof( on ) );
#endif

	UI32 len_connection_addr = sizeof( struct sockaddr_in );
	sockaddr_in connection;
	memset( (char *) &connection, 0, len_connection_addr );
	connection.sin_family = AF_INET;
	connection.sin_addr.s_addr = htonl( INADDR_ANY );
	connection.sin_port = htons( cwmWorldState->ServerData()->ServerPort() );
	bcode = bind( a_socket, (struct sockaddr *)&connection, len_connection_addr );

	if( bcode < 0 )
	{
		Console.error( strutil::format(" Unable to bind socket 1 - Error code: %i", bcode) );
		cwmWorldState->SetKeepRun( false );
		cwmWorldState->SetError( true );
		Shutdown( FATAL_UOX3_ALLOC_NETWORK );
		return;
	}


	unsigned long mode = 1;
	// set the socket to nonblocking
	ioctlsocket( a_socket, FIONBIO, &mode );

	listen( a_socket, 42 );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SockClose( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Closes all sockets for shutdown
//o-----------------------------------------------------------------------------------------------o
void cNetworkStuff::SockClose( void )
{
	closesocket( a_socket );
	for( SOCKLIST_CITERATOR toClose = connClients.begin(); toClose != connClients.end(); ++toClose )
	{
		(*toClose)->CloseSocket();
	}
}

#if PLATFORM != WINDOWS
#ifdef FD_SETSIZE
#if FD_SETSIZE < 256
#undef FD_SETSIZE
#define FD_SETSIZE 256
#endif
#else
#define FD_SETSIZE 256
#endif
#endif

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CheckConn( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks for connection requests
//o-----------------------------------------------------------------------------------------------o
void cNetworkStuff::CheckConn( void )
{
	FD_ZERO( &conn );
	FD_SET( a_socket, &conn );
	SI32 nfds = a_socket + 1;
	SI32 s = select( nfds, &conn, nullptr, nullptr, &cwmWorldState->uoxtimeout );
	if( s > 0 )
	{
		SI32 len = sizeof( struct sockaddr_in );
		size_t newClient;
#if PLATFORM == WINDOWS
		newClient = accept( a_socket, (struct sockaddr *)&client_addr, &len );
#else
		newClient = accept( a_socket, (struct sockaddr *)&client_addr, (socklen_t *)&len );
		if( newClient >= FD_SETSIZE )
		{
			Console.error( "accept() returning unselectable fd!" );
			closesocket( static_cast<UOXSOCKET>( newClient ) );
			return;
		}
#endif
		CSocket *toMake = new CSocket( newClient );
		// set the ip address of the client ;
		toMake->ipaddress = IP4Address( ntohl(client_addr.sin_addr.s_addr) );
		
		if( newClient < 0 )
		{
#if PLATFORM == WINDOWS
			SI32 errorCode = WSAGetLastError();
			if( errorCode == WSAEWOULDBLOCK )
#else
				SI32 errorCode = errno;
			if( errorCode == EWOULDBLOCK )
#endif
			{
				delete toMake;
				return;
			}
			Console.error( "Error at client connection!" );
			cwmWorldState->SetKeepRun( true );
			cwmWorldState->SetError( true );
			delete toMake;
			return;
		}
		//	April 5, 2000
		//	Due to an attack on the shard, and the true inability to determine who did what
		//	I am implementing a ShitList, any IP on this list will be immediatly dropped.
		//	This is a light weight firewall attempt to stop some people that are attacking
		//	a shard that they can easily create new accounts automatically on.
		//	ListFormat: x.x.x.x --> * means any for that domain class
		//   _  _  _
		UI08 part[4];
		part[0] = (UI08)(client_addr.sin_addr.s_addr&0xFF);
		part[1] = (UI08)((client_addr.sin_addr.s_addr&0xFF00)>>8);
		part[2] = (UI08)((client_addr.sin_addr.s_addr&0xFF0000)>>16);
		part[3] = (UI08)((client_addr.sin_addr.s_addr&0xFF000000)>>24);

		if( IsFirewallBlocked( part ) )
		{

			messageLoop << strutil::format( "FIREWALL: Blocking address %i.%i.%i.%i --> Blocked!", part[0], part[1], part[2], part[3] );
			closesocket( static_cast<UOXSOCKET>(newClient) );
			delete toMake;
			return;
		}
		//Firewall-messages are really only needed when firewall blocks, not when it lets someone through. Leads to information overload in console. Commenting out.

		messageLoop << strutil::format( "Client %zu [%i.%i.%i.%i] connected [Total:%lu]", cwmWorldState->GetPlayersOnline(), part[0], part[1], part[2], part[3], cwmWorldState->GetPlayersOnline() + 1 );
		loggedInClients.push_back( toMake );
		toMake->ClientIP( client_addr.sin_addr.s_addr );
		return;
	}
	if( s < 0 )
	{
		Console.error( " Select (Conn) failed!" );
		cwmWorldState->SetKeepRun( false );
		cwmWorldState->SetError( true );
		return;
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool IsFirewallBlocked( UI08 part[4] )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if IP of connecting client is blocked by firewall
//o-----------------------------------------------------------------------------------------------o
bool cNetworkStuff::IsFirewallBlocked( UI08 part[4] )
{
	bool match[4];
	for( size_t i = 0; i < slEntries.size(); ++i )
	{
		for( UI08 k = 0; k < 4; ++k )
		{
			if( slEntries[i].b[k] == -1 )
				match[k] = true;
			else
				match[k] = ( slEntries[i].b[k] == part[k] );
		}
		if( match[0] && match[1] && match[2] && match[3] )
			return true;
	}
	return false;
}
cNetworkStuff::~cNetworkStuff()
{
	size_t i = 0;
	size_t s = 0;
	for( i = 0; i < loggedInClients.size(); ++i )
	{
		loggedInClients[i]->FlushBuffer();
		loggedInClients[i]->CloseSocket();
		delete loggedInClients[i];
	}
	for( i = 0; i < connClients.size(); ++i )
	{
		connClients[i]->FlushBuffer();
		connClients[i]->CloseSocket();
		s = std::max( s, connClients[i]->CliSocket() + 1 );
		delete connClients[i];
	}

	loggedInClients.resize( 0 );
	connClients.resize( 0 );
	closesocket( static_cast<UOXSOCKET>(s) );
#if PLATFORM == WINDOWS
	WSACleanup();
#endif
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CheckMessage( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Check for messages from the clients
//o-----------------------------------------------------------------------------------------------o
void cNetworkStuff::CheckMessage( void )
{
	FD_ZERO(&all);
	FD_ZERO(&errsock);
	SI32 nfds = 0;
	for( SOCKLIST_CITERATOR toCheck = connClients.begin(); toCheck != connClients.end(); ++toCheck )
	{
		UOXSOCKET clientSock = static_cast<UOXSOCKET>((*toCheck)->CliSocket());
		FD_SET( clientSock, &all );
		FD_SET( clientSock, &errsock );
		if( static_cast<int>(clientSock) + 1 > nfds )
			nfds = clientSock + 1;
	}
	SI32 s = select( nfds, &all, nullptr, &errsock, &cwmWorldState->uoxtimeout );
	if( s > 0 )
	{
		size_t oldnow = cwmWorldState->GetPlayersOnline();
		for( size_t i = 0; i < cwmWorldState->GetPlayersOnline(); ++i )
		{
			if( FD_ISSET( connClients[i]->CliSocket(), &errsock ) )
			{
				Disconnect( static_cast<UOXSOCKET>(i) );
			}
			if( ( FD_ISSET( connClients[i]->CliSocket(), &all ) ) && ( oldnow == cwmWorldState->GetPlayersOnline() ) )
			{
				try
				{
					GetMsg( static_cast<UOXSOCKET>(i) );
				}
				catch( socket_error& blah )
				{
#if PLATFORM != WINDOWS
					Console << "Client disconnected" << myendl;
#else
					if( blah.ErrorNumber() == WSAECONNRESET )
						Console << "Client disconnected" << myendl;
					else if( blah.ErrorNumber() != -1 )
						Console << "Socket error: " << (SI32)blah.ErrorNumber() << myendl;
#endif
					Disconnect( i );	// abnormal error
				}
			}
		}
	}
	else if( s == -1 )
	{
	}
}

cNetworkStuff::cNetworkStuff() : peakConnectionCount( 0 ) // Initialize sockets
{
	FD_ZERO( &conn );
	sockInit();
	LoadFirewallEntries();
}

CSocket *cNetworkStuff::GetSockPtr( UOXSOCKET s )
{
	if( s >= connClients.size() )
		return nullptr;
	return connClients[s];
}

CPInputBuffer *WhichPacket( UI08 packetID, CSocket *s );
CPInputBuffer *WhichLoginPacket( UI08 packetID, CSocket *s );
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void GetMsg( UOXSOCKET s )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Receive message from client
//o-----------------------------------------------------------------------------------------------o
void cNetworkStuff::GetMsg( UOXSOCKET s )
{
	if( s >= connClients.size() )
		return;

	CSocket *mSock = connClients[s];

	if( mSock == nullptr )
		return;

	if( mSock->NewClient() )
	{
		SI32 count = mSock->Receive( 4 );
		if( mSock->Buffer()[0] == 0x21 && count < 4 )	// UOMon
		{
			SI32 ho, mi, se, total;
			total = ( cwmWorldState->GetUICurrentTime() - cwmWorldState->GetStartTime() ) / 1000;
			ho = total / 3600;
			total -= ho * 3600;
			mi = total / 60;
			total -= mi * 60;
			se = total;

			auto uoxmonitor = strutil::format( "UOX LoginServer\r\nUOX3 Server: up for %ih %im %is\r\n", ho, mi, se );
			mSock->Send( uoxmonitor.c_str(), static_cast<SI32>(uoxmonitor.size()) );
			mSock->NewClient( false );
		}
		else
		{
			mSock->NewClient( false );
			if( mSock->GetDWord( 0 ) == 0x12345678 )
				Disconnect( s );
			mSock->FirstPacket( true );
		}
	}
	else
	{
		mSock->InLength( 0 );
		CChar *ourChar = mSock->CurrcharObj();
		UI08 *buffer = mSock->Buffer();
		if( mSock->Receive( 1, false ) > 0 )
		{

			SI32 book;
			UI08 packetID = buffer[0];
			if( mSock->FirstPacket() && packetID != 0x80 && packetID != 0x91 )
			{
				// April 5, 2004 - There is a great chance that alot of the times this will be UOG2 connecting to get information from the server
				if(cwmWorldState->ServerData()->ServerUOGEnabled())
					Console << "UOG Stats request completed. Disconnecting client. [" << (SI32)mSock->ClientIP4() << "." << (SI32)mSock->ClientIP3() << "." << (SI32)mSock->ClientIP2() << "." << (SI32)mSock->ClientIP1() << "]" << myendl;
				else
					Console << "Encrypted client attempting to cut in, disconnecting them: IP " << (SI32)mSock->ClientIP1() << "." << (SI32)mSock->ClientIP2() << "." << (SI32)mSock->ClientIP3() << "." << (SI32)mSock->ClientIP4() << myendl;
				Disconnect( s );
				return;
			}
#if _DEBUG_PACKET
			Console.print( strutil::format( "Packet ID: 0x%x\n", packetID ));
#endif
			if( packetID != 0x73 && packetID != 0xA4 && packetID != 0x80 && packetID != 0x91 )
			{
				if( !mSock->ForceOffline() ) //Don't refresh idle-timer if character is being forced offline
					mSock->IdleTimeout( cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_LOGINTIMEOUT ) );
			}

			bool doSwitch		= true;
			if( cwmWorldState->ServerData()->ServerOverloadPackets() )
			{
				std::map< UI16, UI16 >::const_iterator pFind = packetOverloads.find( packetID );
				if( pFind != packetOverloads.end() )
				{
					cScript *pScript = JSMapping->GetScript( pFind->second );
					if( pScript != nullptr )
					{
						doSwitch = !pScript->OnPacketReceive( mSock, packetID );
					}
				}
			}
			if( doSwitch )
			{
				CPInputBuffer *test	= WhichPacket( packetID, mSock );
				if( test != nullptr )
				{
					mSock->ReceiveLogging( test );
					if( test->Handle() )
						doSwitch = false;
					delete test;
				}
			}
			if( doSwitch )
			{
				size_t j = 0;
				switch( packetID )
				{
					case 0x01: // Main Menu on the character select screen
						Disconnect( s );
						break;
					case 0x72:// Combat Mode
						mSock->Receive( 5 );
						ourChar->SetWar( buffer[1] != 0 );
						ourChar->SetTarg( nullptr );
						if( ourChar->GetTimer( tCHAR_TIMEOUT ) <= cwmWorldState->GetUICurrentTime() )
							ourChar->SetTimer( tCHAR_TIMEOUT, BuildTimeValue( 1 ) );
						else
							ourChar->SetTimer( tCHAR_TIMEOUT, ourChar->GetTimer( tCHAR_TIMEOUT ) + 1000 );
						mSock->Send( buffer, 5 );
#if defined( _MSC_VER )
#pragma note( "Flush location" )
#endif
						mSock->FlushBuffer();
						Movement->CombatWalk( ourChar );
						Effects->doSocketMusic( mSock );
						ourChar->BreakConcentration( mSock );
						break;
					case 0x12:// Ext. Command
						mSock->Receive( 3 );
						mSock->Receive( mSock->GetWord( 1 ) );
						ourChar->BreakConcentration( mSock );
						if( buffer[3] == 0xC7 ) // Action
						{
							if( ourChar->IsOnHorse() || ourChar->IsFlying() )
								return; // can't bow or salute on horse or while flying
							if( !strcmp( (char *)&buffer[4], "bow" ) )
							{
								if( ourChar->GetBodyType() == BT_GARGOYLE 
									|| ( cwmWorldState->ServerData()->ForceNewAnimationPacket() 
										&& ( ourChar->GetSocket() == nullptr || ourChar->GetSocket()->ClientType() >= CV_SA2D )))
								{
									// If gargoyle, human or elf, and new animation packet is enabled
									Effects->PlayNewCharacterAnimation( ourChar, N_ACT_EMOTE, S_ACT_EMOTE_BOW );
								}
								else if(( ourChar->GetBodyType() == BT_HUMAN || ourChar->GetBodyType() == BT_ELF )
									&& !cwmWorldState->ServerData()->ForceNewAnimationPacket() )
								{
									// If human or elf, and new animation packet is disabled
									Effects->PlayCharacterAnimation( ourChar, ACT_EMOTE_BOW ); // 0x20
								}
								else
								{
									// If polymorphed to other creatures
									Effects->PlayCharacterAnimation( ourChar, 0x12 ); // Monster fidget 1
								}
							}
							if( !strcmp( (char *)&buffer[4], "salute" ) )
							{
								if( ourChar->GetBodyType() == BT_GARGOYLE 
									|| ( cwmWorldState->ServerData()->ForceNewAnimationPacket() 
										&& ( ourChar->GetSocket() == nullptr || ourChar->GetSocket()->ClientType() >= CV_SA2D )))
								{
									// If gargoyle, human or elf, and new animation packet is enabled
									Effects->PlayNewCharacterAnimation( ourChar, N_ACT_EMOTE, S_ACT_EMOTE_SALUTE );
								}
								else if(( ourChar->GetBodyType() == BT_HUMAN || ourChar->GetBodyType() == BT_ELF )
									&& !cwmWorldState->ServerData()->ForceNewAnimationPacket() )
								{
									// If human or elf, and new animation packet is disabled
									Effects->PlayCharacterAnimation( ourChar, ACT_EMOTE_SALUTE ); // 0x21
								}
								else
								{
									// If polymorphed to other creatures
									Effects->PlayCharacterAnimation( ourChar, 0x11 ); // Monster fidget 2
								}
							}
							break;
						}
						else if( buffer[3] == 0x58 )
						{
							DoorMacro( mSock );
							break;
						}
						else if( buffer[3] == 0x24 ) // Skill
						{
							j = 4;
							while( buffer[j] != ' ' )
								++j;
							buffer[j] = 0;
							Skills->SkillUse( mSock, static_cast<UI08>(std::stoul(std::string( (char*)&buffer[4] )) ));
							break;
						}
						else if( buffer[3] == 0x27 || buffer[3] == 0x56 )  // Spell
						{
							// This'll find our spellbook for us
							CItem *sBook	= FindItemOfType( ourChar, IT_SPELLBOOK );
							CItem *p		= ourChar->GetPackItem();
							if( ValidateObject( sBook ) )
							{
								bool validLoc	= false;
								if( sBook->GetCont() == ourChar )
									validLoc = true;
								else if( ValidateObject( p ) && sBook->GetCont() == p )
									validLoc = true;

								if( validLoc )
								{
									book = buffer[4] - 0x30;
									if( buffer[5] > 0x20 )
										book = ( book * 10 ) + ( buffer[5] - 0x30 );
									if( Magic->CheckBook( ( ( book - 1 ) / 8 ) + 1, ( book - 1 ) % 8, sBook ) )
									{
										if( ourChar->IsFrozen() )
										{
											if( ourChar->IsCasting() )
												mSock->sysmessage( 762 );
											else
												mSock->sysmessage( 763 );
										}
										else
										{
											mSock->CurrentSpellType( 0 );
											Magic->SelectSpell( mSock, book );
										}
									}
									else
										mSock->sysmessage( 764 );
								}
								else
									mSock->sysmessage( 765 );
							}
						}
						else
						{
							if( buffer[2] == 0x05 && buffer[3] == 0x43 )  // Open spell book
								Magic->SpellBook( mSock );
							break;
						}
						break;
						// client sends them out if the title and/or author gets changed on a writeable book
						// its NOT send (anymore?) when a book is closed as many packet docus state.
						// 7-dec 1999
					case 0x93:	// Old Book Packet, no longer sent by the client
						mSock->Receive( 99 );
						break;
					case 0x69:// Client text change
						mSock->Receive( 3 );// What a STUPID message...  It would be useful if
						mSock->Receive( mSock->GetWord( 1 ) );// it included the color changed to, but it doesn't!
						break;
					case 0xB5: // Open Chat Window
						mSock->Receive( 64 );
						break;
					case 0xB6:// T2A Popuphelp request
						mSock->Receive( 9 );
						break;
					case 0xB8:// T2A Profile request
						mSock->Receive( 3 );
						mSock->Receive( mSock->GetWord( 1 ) );		// this is how it's really done

						if( buffer[3] == 1 )		// Set profile, not feasible yet
							mSock->sysmessage( 768 );
						else
						{
							SERIAL targSerial = mSock->GetDWord( 4 );
							CChar *player = calcCharObjFromSer( targSerial );
							if( !ValidateObject( player ) )
								mSock->sysmessage( 769 );
							else
							{
								// Still doesn't work
								//	Request Char Profile (Variable # of bytes)
								//	BYTE cmd
								//	BYTE[2] blockSize
								//	BYTE mode (CLIENT ONLY! Does not exist in server message.)
								//	BYTE[4] id
								//	If request, ends here.
								//	If Update request
								//	BYTE[2] cmdType (0x0001 – Update)
								//	BYTE[2] msglen (# of unicode characters)
								//	BYTE[msglen][2] new profile, in unicode, not null terminated.
								//	Else If from server
								//	BYTE[?] title (not unicode, null terminated.)
								//	BYTE[?][2] static profile  ( unicode string can't  be edited )
								//	BYTE[2] (0x0000) (static profile terminator)
								//	BYTE[?][2] profile (in unicode, can be edited)
								//	BYTE[2] (0x0000) (profile terminator)

								UI08 tempBuffer[1024];
								std::string ourMessage;
								memset( tempBuffer, 0, sizeof( tempBuffer[0] ) * 1024 );	// zero it out
								tempBuffer[0] = 0xB8;
								tempBuffer[3] = buffer[4];
								tempBuffer[4] = buffer[5];
								tempBuffer[5] = buffer[6];
								tempBuffer[6] = buffer[7];
								size_t plNameLen = player->GetName().size();
								strncpy( (char *)&tempBuffer[7], player->GetName().c_str(), 30 );
								size_t mj = 8 + plNameLen;
								size_t plTitleLen = player->GetName().size() + 1;
								for( size_t k = 0; k < plTitleLen; ++k )
								{
									tempBuffer[mj++] = 0;
									tempBuffer[mj++] = player->GetName().data()[k];
								}
								mj += 2;
								ourMessage = "Test of Char Profile";
								for( j = 0; j < ourMessage.size(); ++j )
								{
									tempBuffer[mj++] = 0;
									tempBuffer[mj++] = ourMessage[j];
								}
								mj += 2;
								tempBuffer[2] = mj;
								mSock->Send( tempBuffer, static_cast<SI32>(mj) );
#if defined( _MSC_VER )
#pragma note( "Flush location" )
#endif
								mSock->FlushBuffer();
							}
						}
						break;
					case 0xBB:
						//	Ultima Messenger (9 bytes)
						//	BYTE cmd
						//	BYTE[4] id1
						//	BYTE[4] id2
						//	Note: This is both a client and server message.
						mSock->Receive( 9 );
						break;
					case 0x3A:	// skills management packet
						mSock->Receive( 6 );	// it's always 6 so Westy tells me... let's not be intelligent =)
						UI16 skillNum;
						skillNum = mSock->GetWord( 3 );
						ourChar->SetSkillLock( (SkillLock)buffer[5], (UI08)skillNum );
						break;

					case 0x56:
						Console << "'Plot Course' button on a map clicked." << myendl;
						break;
					case 0xD0:
						// Configuration file
						mSock->Receive( 3 );
						mSock->Receive( mSock->GetWord( 1 ) );
						break;
					case 0xD9: // Spy On Client
						break;
					case 0xFB: // Update View Public House Contents
						mSock->Receive( 2 );
						break;
					case 0xF0: // ClassicUO Map Tracker, handled by CPIKrriosClientSpecial in CPacketReceive.cpp
						break;
					default:
						FD_ZERO( &all );
						FD_SET( mSock->CliSocket(), &all );
						SI32 nfds;
						nfds = static_cast<int>(mSock->CliSocket()) + 1;
						if( select( nfds, &all, nullptr, nullptr, &cwmWorldState->uoxtimeout ) > 0 )
							mSock->Receive( 2560 );

						Console << strutil::format("Unknown message from client: 0x%02X - Ignored", packetID ) << myendl;
						break;
				}
			}
		}
		else	// Sortta error type thing, they disconnected already
			Disconnect( s );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CheckLoginMessage( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	 Check for messages from the clients
//o-----------------------------------------------------------------------------------------------o
void cNetworkStuff::CheckLoginMessage( void )
{
	fd_set all;
	fd_set errsock;
	size_t i;

	cwmWorldState->uoxtimeout.tv_sec = 0;
	cwmWorldState->uoxtimeout.tv_usec = 1;

	FD_ZERO( &all );
	FD_ZERO( &errsock );

	SI32 nfds = 0;
	for( i = 0; i < loggedInClients.size(); ++i )
	{
		size_t clientSock = loggedInClients[i]->CliSocket();
		FD_SET( clientSock, &all );
		FD_SET( clientSock, &errsock );
		if( static_cast<int>(clientSock) + 1 > nfds )
			nfds = static_cast<int>(clientSock) + 1;
	}
	SI32 s = select( static_cast<UOXSOCKET>(nfds), &all, nullptr, &errsock, &cwmWorldState->uoxtimeout );
	if( s > 0 )
	{
		size_t oldnow = loggedInClients.size();
		for( i = 0; i < loggedInClients.size(); ++i )
		{
			if( FD_ISSET( loggedInClients[i]->CliSocket(), &errsock ) )
			{
				LoginDisconnect( static_cast<UOXSOCKET>(i) );
				continue;
			}
			if( ( FD_ISSET( loggedInClients[i]->CliSocket(), &all ) ) && ( oldnow == loggedInClients.size() ) )
			{
				try
				{
					GetLoginMsg( static_cast<UOXSOCKET>(i) );
				}
				catch( socket_error& blah )
				{
#if PLATFORM != WINDOWS
					messageLoop << "Client disconnected";
#else
					if( blah.ErrorNumber() == WSAECONNRESET )
						messageLoop << "Client disconnected";
					else if( blah.ErrorNumber() != -1 )
					{


						messageLoop << strutil::format("Socket error: %i", blah.ErrorNumber() );
					}
#endif
					LoginDisconnect( i );	// abnormal error
				}
			}
		}
	}
	else if( s == -1 )
	{
#if PLATFORM == WINDOWS
		SI32 errorCode = WSAGetLastError();
		if( errorCode != 10022 )
			Console << (SI32)errorCode << myendl;
#endif
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void LoginDisconnect( UOXSOCKET s )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Force disconnection of player //Instalog
//o-----------------------------------------------------------------------------------------------o
void cNetworkStuff::LoginDisconnect( UOXSOCKET s )
{


	messageLoop << strutil::format( "LoginClient %u disconnected.", s );
	loggedInClients[s]->FlushBuffer();
	loggedInClients[s]->CloseSocket();

	// 9/17/01 : fix for clients disconnecting during login not being able to reconnect.
	if( loggedInClients[s]->AcctNo() != AB_INVALID_ID )
	{
		CAccountBlock& actbAccount = loggedInClients[s]->GetAccount();
		if( actbAccount.wAccountIndex != AB_INVALID_ID )
		{
			actbAccount.wFlags.set( AB_FLAGS_ONLINE, false );
		}
	}

	for( size_t q = 0; q < loggIteratorBackup.size(); ++q )
	{
		if( loggIteratorBackup[q] != loggedInClients.begin() && loggIteratorBackup[q] >= (loggedInClients.begin() + s) )
			--loggIteratorBackup[q];
	}
	delete loggedInClients[s];
	loggedInClients.erase( loggedInClients.begin() + s );
}

void cNetworkStuff::LoginDisconnect( CSocket *s ) // Force disconnection of player //Instalog
{
	UOXSOCKET i = FindLoginPtr( s );
	LoginDisconnect( i );
}

void cNetworkStuff::Disconnect( CSocket *s ) // Force disconnection of player //Instalog
{
	UOXSOCKET i = FindNetworkPtr( s );
	Disconnect( i );
}

UOXSOCKET cNetworkStuff::FindLoginPtr( CSocket *s )
{
	for( UOXSOCKET i = 0; i < loggedInClients.size(); ++i )
	{
		if( loggedInClients[i] == s )
			return i;
	}
	return 0xFFFFFFFF;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void Transfer( CSocket *mSock )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Transfers from the logged in queue to the in world queue
//|
//|	Notes		-	Takes something out of the logging in queue and places it in the in world queue
//|					REQUIRES THREAD SAFETY
//o-----------------------------------------------------------------------------------------------o
void cNetworkStuff::Transfer( CSocket *mSock )
{
	UOXSOCKET s = FindLoginPtr( mSock );
	if( s >= loggedInClients.size() )
		return;
	//std::scoped_lock lock(internallock);
	connClients.push_back( loggedInClients[s] );
	loggedInClients.erase( loggedInClients.begin() + s );
	cwmWorldState->SetPlayersOnline( connClients.size() );
	if( cwmWorldState->GetPlayersOnline() > peakConnectionCount )
		peakConnectionCount = cwmWorldState->GetPlayersOnline();
	return;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void GetLoginMsg( UOXSOCKET s )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets login message from socket
//o-----------------------------------------------------------------------------------------------o
void cNetworkStuff::GetLoginMsg( UOXSOCKET s )
{
	CSocket *mSock = loggedInClients[s];
	if( mSock == nullptr )
		return;
	if( mSock->NewClient() )
	{
		SI32 count;
		count = mSock->Receive( 4 );
		auto packetID = mSock->Buffer()[0];

		if( cwmWorldState->ServerData()->ServerOverloadPackets() )
		{
			// Allow overloading initial connection packets
			std::map< UI16, UI16 >::const_iterator pFind = packetOverloads.find( packetID );
			if( pFind != packetOverloads.end() )
			{
				cScript *pScript = JSMapping->GetScript( pFind->second );
				if( pScript != nullptr )
				{
					pScript->OnPacketReceive( mSock, packetID );
				}
			}
		}

		if( packetID == 0xEF || packetID == 0xC0 || packetID == 0xFF )
		{
			WhichLoginPacket( packetID, mSock );
			//mSock->Receive( 21, false );
		}

		mSock->NewClient( false );
		if( mSock->GetDWord( 0 ) == 0x12345678 )
			LoginDisconnect( s );
		mSock->FirstPacket( true );
	}
	else
	{
		mSock->InLength( 0 );
		UI08 *buffer = mSock->Buffer();
		if( mSock->Receive( 1, false ) > 0 )
		{

			UI08 packetID = buffer[0];
			if( mSock->FirstPacket() && packetID != 0x80 && packetID != 0x91 && packetID != 0xE4 && packetID != 0xF1 )
			{
				// April 5, 2004 - Hmmm there are two of these ?
				if(cwmWorldState->ServerData()->ServerUOGEnabled()){
					messageLoop << strutil::format("UOG Stats Sent or Encrypted client detected. [%i.%i.%i.%i]", mSock->ClientIP4(), mSock->ClientIP3(), mSock->ClientIP2(), mSock->ClientIP1() );
				}
				else{
					messageLoop <<strutil::format("Encrypted client detected. [%i.%i.%i.%i]", mSock->ClientIP4(), mSock->ClientIP3(), mSock->ClientIP2(), mSock->ClientIP1() );
				}
				LoginDisconnect( s );
				return;
			}
			else if( mSock->FirstPacket() && packetID == 0 )
			{

				messageLoop << strutil::format("Buffer is empty, no packets to read. Disconnecting client. [%i.%i.%i.%i]", mSock->ClientIP4(), mSock->ClientIP3(), mSock->ClientIP2(), mSock->ClientIP1() );
				LoginDisconnect( s );
				return;
			}
			bool doSwitch = true;
			if( cwmWorldState->ServerData()->ServerOverloadPackets() )
			{
				std::map< UI16, UI16 >::const_iterator pFind = packetOverloads.find( packetID );
				if( pFind != packetOverloads.end() )
				{
					cScript *pScript = JSMapping->GetScript( pFind->second );
					if( pScript != nullptr )
					{
						doSwitch = !pScript->OnPacketReceive( mSock, packetID );
					}
				}
			}
			if( doSwitch )
			{
				CPInputBuffer *test = nullptr;
				try
				{
					test = WhichLoginPacket( packetID, mSock );
				}
				catch( socket_error& )
				{
					Console.warning(strutil::format("Bad packet request thrown! [packet ID: 0x%x]", packetID) );
					mSock->FlushIncoming();
					return;
				}
				if( test != nullptr )
				{
#ifdef _DEBUG_PACKET
					Console.log( strutil::format("Logging packet ID: 0x%X", packetID), "loginthread.txt" );
#endif
					mSock->ReceiveLogging( test );
					if( test->Handle() )
						doSwitch = false;
					delete test;
				}
			}
			if( doSwitch )
			{
				switch( packetID )
				{
					case 0x01:
						//Disconnect Notification (5 bytes)
						//	BYTE cmd
						//	BYTE[4] pattern (0xFFFFFFFF)
						//Note:  Client message
						//Note:  Sent when the user chooses to return to the main menu from the character select menu.
						//Note:  Since the character select menu no longer has a main menu button, this message is no longer sent.
						//Note:  It’s send again in current 3.0.x clients. When player does "paper-doll logout"
						LoginDisconnect( s );
						break;
					case 0xBB:
						// Ultima Messenger  (9 bytes)
						//	BYTE cmd
						//	BYTE[4] id1
						//	BYTE[4] id2
						//Note: This is both a client and server message.
						mSock->Receive( 9 );
						break;
					case 0xBF:
						mSock->Receive( 3 );
						mSock->Receive( mSock->GetWord( 1 ) );
						break;
					default:
						SI32 nfds;
						fd_set all;
						FD_ZERO( &all );
						FD_SET( mSock->CliSocket(), &all );
						nfds = static_cast<UOXSOCKET>(mSock->CliSocket()) + 1;
						if( select( nfds, &all, nullptr, nullptr, &cwmWorldState->uoxtimeout ) > 0 )
							mSock->Receive( 2560 );
						messageLoop << strutil::format("Unknown message from client: 0x%02X - Ignored", packetID );
						break;
				}
			}
		}
		else	// Sortta error type thing, they disconnected already
			LoginDisconnect( s );
	}
}

UOXSOCKET cNetworkStuff::FindNetworkPtr( CSocket *toFind )
{
	for( UOXSOCKET i = 0; i < connClients.size(); ++i )
	{
		if( connClients[i] == toFind )
			return i;
	}
	return 0xFFFFFFFF;
}

CSocket *cNetworkStuff::FirstSocket( void )
{
	CSocket *retSock = nullptr;
	currConnIter = connClients.begin();
	if( !FinishedSockets() )
		retSock = (*currConnIter);
	return retSock;
}
CSocket *cNetworkStuff::NextSocket( void )
{
	CSocket *retSock = nullptr;
	if( !FinishedSockets() )
	{
		++currConnIter;
		if( !FinishedSockets() )
			retSock = (*currConnIter);
	}
	return retSock;
}
bool cNetworkStuff::FinishedSockets( void )
{
	return ( currConnIter == connClients.end() );
}

CSocket *cNetworkStuff::PrevSocket( void )
{
	if( currConnIter == connClients.begin() )
		return nullptr;
	--currConnIter;
	return (*currConnIter);
}
CSocket *cNetworkStuff::LastSocket( void )
{
	currConnIter = connClients.end();
	if( currConnIter != connClients.begin() )
	{
		--currConnIter;
		return (*currConnIter);
	}
	else
		return nullptr;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void cNetworkStuff::LoadFirewallEntries( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads list of banned IPs from firewall list
//o-----------------------------------------------------------------------------------------------o
void cNetworkStuff::LoadFirewallEntries( void )
{
	std::string token;
	std::string fileToUse;
	if( !FileExists( "banlist.ini" ) )
	{
		if( FileExists( "firewall.ini" ) )
		{
			fileToUse = "firewall.ini";
		}
	}
	else
	{
		fileToUse = "banlist.ini";
	}
	if( !fileToUse.empty() )
	{
		Script *firewallData = new Script( fileToUse, NUM_DEFS, false );
		if( firewallData != nullptr )
		{
			SI16 p[4];
			ScriptSection *firewallSect = nullptr;
			std::string tag, data;
			for( firewallSect = firewallData->FirstEntry(); firewallSect != nullptr; firewallSect = firewallData->NextEntry() )
			{
				if( firewallSect != nullptr )
				{
					for( tag = firewallSect->First(); !firewallSect->AtEnd(); tag = firewallSect->Next() )
					{
						if( strutil::toupper( tag ) == "IP" )
						{
							data = firewallSect->GrabData();
							data = strutil::stripTrim( data );
							if( !data.empty() )
							{
								auto psecs = strutil::sections( data, "." );
								if( psecs.size() == 4 )	// Wellformed IP address
								{
									for( UI08 i = 0; i < 4; ++i )
									{
										token = psecs[i];
										if( token == "*" )
										{
											p[i] = -1;
										}
										else
										{
											p[i] = static_cast<SI16>(std::stoi(token, nullptr, 0));
										}
									}
									slEntries.push_back( FirewallEntry( p[0], p[1], p[2], p[3] ) );
								}
								else if( data != "\n" && data != "\r" )
									Console.error( strutil::format("Malformed IP address in banlist.ini (line: %s)", data.c_str() ));
							}
						}
					}
				}
			}
		}
		delete firewallData;
	}
}

void cNetworkStuff::RegisterPacket( UI08 packet, UI08 subCmd, UI16 scriptID )
{
	//UI16 packetID = static_cast<UI16>((subCmd<<8) + packet); // Registration of subCmd disabled until it can be fully implemented
	UI16 packetID = static_cast<UI16>(packet);
	packetOverloads[packetID] = scriptID;
}

void cNetworkStuff::CheckConnections( void )
{
	CheckConn();
}

void cNetworkStuff::CheckMessages( void )
{
	CheckLoginMessage();
	CheckMessage();
}

size_t cNetworkStuff::PeakConnectionCount( void ) const
{
	return peakConnectionCount;
}



//	nonlock methods, but this whole approach is questionable
void cNetworkStuff::pushConn() {
	connIteratorBackup.push_back( currConnIter );

}
void cNetworkStuff::popConn( void )
{
	currConnIter = connIteratorBackup.back();
	connIteratorBackup.pop_back();
}
void cNetworkStuff::pushLogg( void )
{

	loggIteratorBackup.push_back( currLoggIter );
}
void cNetworkStuff::popLogg( void )
{
	currLoggIter = loggIteratorBackup.back();
	loggIteratorBackup.pop_back();

}

void cNetworkStuff::PushConn( void )
{
	// This is horrible, to lock the mutex, and release elsewhere,
	// Dont do this boys and girls, refactor your code!
	// Leaving this here for now, as we transistion to c++17 thread. But later..
	internallock.lock() ;
	connIteratorBackup.push_back( currConnIter );
}

void cNetworkStuff::PopConn( void )
{
	currConnIter = connIteratorBackup.back();
	connIteratorBackup.pop_back();
	internallock.unlock() ;
}

void cNetworkStuff::PushLogg( void )
{
	internallock.lock() ;
	loggIteratorBackup.push_back( currLoggIter );
}
void cNetworkStuff::PopLogg( void )
{
	currLoggIter = loggIteratorBackup.back();
	loggIteratorBackup.pop_back();
	internallock.unlock() ;
}
