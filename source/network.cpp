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

#if UOX_PLATFORM != PLATFORM_WIN32
	#include <sys/ioctl.h>
#endif

#undef DBGFILE
#define DBGFILE "Network.cpp"

namespace UOX
{

cNetworkStuff *Network = NULL;

#define _DEBUG_PACKET	0

fd_set conn;
fd_set all;
fd_set errsock;

void killTrades( CChar *i );
void DoorMacro( CSocket *s );
void sysBroadcast( const std::string& txt );

void cNetworkStuff::ClearBuffers( void ) // Sends ALL buffered data
{
	SOCKLIST_CITERATOR toClear;
	for( toClear = connClients.begin(); toClear != connClients.end(); ++toClear )
		(*toClear)->FlushBuffer();
	for( toClear = loggedInClients.begin(); toClear != loggedInClients.end(); ++toClear )
		(*toClear)->FlushBuffer();
}

// set the laston character member value to the current date/time
void cNetworkStuff::setLastOn( CSocket *s )
{
	assert( s != NULL );
	
	time_t ltime;
	time( &ltime );
	char *t = ctime( &ltime );
	// just to be paranoid and avoid crashing
	if( NULL == t )
		t = "";
	else
	{
		// some ctime()s like to stick \r\n on the end, we don't want that
		size_t mLen = strlen( t );
		for( size_t end = mLen - 1; end >= 0 && isspace( t[end] ) && end < mLen; --end )
			t[end] = '\0';
	}
	if( s->CurrcharObj() != NULL )
	{
		s->CurrcharObj()->SetLastOn( t );
		s->CurrcharObj()->SetLastOnSecs( ltime );
	}
}

void cNetworkStuff::Disconnect( UOXSOCKET s ) // Force disconnection of player //Instalog
{
	setLastOn( connClients[s] );
	CChar *currChar = connClients[s]->CurrcharObj();
	Console << "Client " << (UI32)s << " disconnected. [Total:" << (SI32)(cwmWorldState->GetPlayersOnline()-1) << "]" << myendl;

	char temp[1024];

	if( currChar != NULL )
	{
		if( currChar->GetAccount().wAccountIndex == connClients[s]->AcctNo() && cwmWorldState->ServerData()->ServerJoinPartAnnouncementsStatus() ) 
		{
			sprintf( temp, Dictionary->GetEntry( 752 ).c_str(), currChar->GetName().c_str() );
			sysBroadcast( temp );//message upon leaving a server 
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
	if( currChar != NULL )
	{
		if( !currChar->isFree() && connClients[s] != NULL )
		{
			// October 6, 2002 - Brakhtus Support for the onLogout event.
			cScript *onLogoutScp = JSMapping->GetScript(currChar->GetScriptTrigger());
			if( onLogoutScp!=NULL )
				onLogoutScp->OnLogout(connClients[s], currChar);
			else
			{
				onLogoutScp = JSMapping->GetScript( (UI16)0 );
				if( onLogoutScp!=NULL )
					onLogoutScp->OnLogout(connClients[s],currChar);
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


//o--------------------------------------------------------------------------o
//|	Function		-	void cNetworkStuff::LogOut( UOXSOCKET s )
//|	Date			-	
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Process client logout, and process instalog settings
//|
//|	Modification	-	Date Unknown - Added multi checked to instalog processing
//o--------------------------------------------------------------------------o
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
		CMultiObj *multi = NULL;
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
			i->SetCont( NULL );
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
	p->SetSocket( NULL );
	p->SetLocation( p );
}

void cNetworkStuff::sockInit( void )
{
	int bcode;
	
	cwmWorldState->SetKeepRun( true );
	cwmWorldState->SetError( false );
	
#if UOX_PLATFORM != PLATFORM_WIN32
	int on = 1;
#endif
	
	a_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( a_socket < 0 )
	{
		Console.Error( " Unable to create socket");
#if UOX_PLATFORM == PLATFORM_WIN32
		Console.Error( " Code %i", WSAGetLastError() );
#else
		Console << myendl;
#endif
		cwmWorldState->SetKeepRun( false );
		cwmWorldState->SetError( true );
		Shutdown( FATAL_UOX3_ALLOC_NETWORK );
		return;
	}
#if UOX_PLATFORM != PLATFORM_WIN32
	setsockopt( a_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof( on ) );
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
		Console.Error( " Unable to bind socket 1 - Error code: %i", bcode );
		cwmWorldState->SetKeepRun( false );
		cwmWorldState->SetError( true );
		Shutdown( FATAL_UOX3_ALLOC_NETWORK );
		return;
	}
    
	
	UI32 mode = 1;
	// set the socket to nonblocking
	ioctlsocket( a_socket, FIONBIO, &mode );

	listen( a_socket, 42 );
}

void cNetworkStuff::SockClose( void ) // Close all sockets for shutdown
{
	closesocket( a_socket );
	for( SOCKLIST_CITERATOR toClose = connClients.begin(); toClose != connClients.end(); ++toClose )
	{
		(*toClose)->CloseSocket();
	}
}

#if UOX_PLATFORM != PLATFORM_WIN32
	#ifdef FD_SETSIZE
		#if FD_SETSIZE < 256
			#undef FD_SETSIZE
			#define FD_SETSIZE 256 
		#endif
	#else
		#define FD_SETSIZE 256 
	#endif
#endif

void cNetworkStuff::CheckConn( void ) // Check for connection requests
{
	FD_ZERO( &conn );
	FD_SET( a_socket, &conn );
	int nfds = a_socket + 1;
	int s = select( nfds, &conn, NULL, NULL, &cwmWorldState->uoxtimeout );
	if( s > 0 )
	{
		int len = sizeof( struct sockaddr_in );
		size_t newClient;
#if UOX_PLATFORM == PLATFORM_WIN32
		newClient = accept( a_socket, (struct sockaddr *)&client_addr, &len );
#else
		newClient = accept( a_socket, (struct sockaddr *)&client_addr, (socklen_t *)&len );
		if( newClient >= FD_SETSIZE )
		{
			Console.Error( "accept() returning unselectable fd!" );
			return;
		}
#endif
		CSocket *toMake = new CSocket( newClient );
		if( newClient < 0 )
		{
#if UOX_PLATFORM == PLATFORM_WIN32
			int errorCode = WSAGetLastError();
			if( errorCode == WSAEWOULDBLOCK )
#else
			int errorCode = errno;
			if( errorCode == EWOULDBLOCK )
#endif
			{
				delete toMake;
				return;
			}
			Console.Error( "Error at client connection!" );
			cwmWorldState->SetKeepRun( true );
			cwmWorldState->SetError( true );
			delete toMake;
			return;
		}
		//	EviLDeD	-	April 5, 2000
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
		char temp[128];
		if( IsFirewallBlocked( part ) )
		{
			sprintf( temp, "FIREWALL: Blocking address %i.%i.%i.%i --> Blocked!", part[0], part[1], part[2], part[3] );
			messageLoop << temp;
			closesocket( newClient );
			delete toMake;
			return;
		}
		//Firewall-messages are really only needed when firewall blocks, not when it lets someone through. Leads to information overload in console. Commenting out.
		//sprintf( temp, "FIREWALL: Forwarding address %i.%i.%i.%i --> Access Granted!", part[0], part[1], part[2], part[3] );
		//messageLoop << temp;
		sprintf( temp, "Client %i [%i.%i.%i.%i] connected [Total:%i]", cwmWorldState->GetPlayersOnline(), part[0], part[1], part[2], part[3], cwmWorldState->GetPlayersOnline() + 1 );
		messageLoop << temp;
		loggedInClients.push_back( toMake );
		toMake->ClientIP( client_addr.sin_addr.s_addr );
		return;
	}
	if( s < 0 )
	{
		Console.Error( " Select (Conn) failed!" );
		cwmWorldState->SetKeepRun( false );
		cwmWorldState->SetError( true );
		return;
	}
}

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
		s = UOX_MAX( s, connClients[i]->CliSocket() + 1 );
		delete connClients[i];
	}

	loggedInClients.resize( 0 );
	connClients.resize( 0 );
	closesocket( s );
#if UOX_PLATFORM == PLATFORM_WIN32
	WSACleanup();
#endif
}

void cNetworkStuff::CheckMessage( void ) // Check for messages from the clients
{
	FD_ZERO(&all);
	FD_ZERO(&errsock);
	int nfds = 0;
	for( SOCKLIST_CITERATOR toCheck = connClients.begin(); toCheck != connClients.end(); ++toCheck )
	{
		int clientSock = (*toCheck)->CliSocket();
		FD_SET( clientSock, &all );
		FD_SET( clientSock, &errsock );
		if( clientSock + 1 > nfds )
			nfds = clientSock + 1;
	}
	int s = select( nfds, &all, NULL, &errsock, &cwmWorldState->uoxtimeout );
	if( s > 0 )
	{
		size_t oldnow = cwmWorldState->GetPlayersOnline();
		for( size_t i = 0; i < cwmWorldState->GetPlayersOnline(); ++i )
		{
			if( FD_ISSET( connClients[i]->CliSocket(), &errsock ) )
			{
				Disconnect( i );
			}
			if( ( FD_ISSET( connClients[i]->CliSocket(), &all ) ) && ( oldnow == cwmWorldState->GetPlayersOnline() ) )
			{
				try
				{
					GetMsg( i );
				}
				catch( socket_error& blah )
				{
#if UOX_PLATFORM != PLATFORM_WIN32
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
	else if( s == SOCKET_ERROR )
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
		return NULL;
	return connClients[s];
}

CPInputBuffer *WhichPacket( UI08 packetID, CSocket *s );
CPInputBuffer *WhichLoginPacket( UI08 packetID, CSocket *s );
void cNetworkStuff::GetMsg( UOXSOCKET s ) // Receive message from client
{
	if( s >= connClients.size() )
		return;

	CSocket *mSock = connClients[s];

	if( mSock == NULL )
		return;
	
	if( mSock->NewClient() )
	{
		int count = mSock->Receive( 4 );
		if( mSock->Buffer()[0] == 0x21 && count < 4 )	// UOMon
		{
			int ho, mi, se, total;
			total = ( cwmWorldState->GetUICurrentTime() - cwmWorldState->GetStartTime() ) / 1000;
			ho = total / 3600;
			total -= ho * 3600;
			mi = total / 60;
			total -= mi * 60;
			se = total;
			UI08 uoxmonitor[100];
			sprintf( (char *)uoxmonitor, "UOX LoginServer\r\nUOX3 Server: up for %ih %im %is\r\n", ho, mi, se );
			mSock->Send( uoxmonitor, strlen( (char *)uoxmonitor ) );
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
			char temp[1024];
			int book;
			UI08 packetID = buffer[0];
			if( mSock->FirstPacket() && packetID != 0x80 && packetID != 0x91 )
			{
				// April 5, 2004 - EviLDeD - There is a great chance that alot of the times this will be UOG2 connecting to get information from the server
				if(cwmWorldState->ServerData()->ServerUOGEnabled())
					Console << "UOG Stats request completed. Disconnecting client. [" << (SI32)mSock->ClientIP4() << "." << (SI32)mSock->ClientIP3() << "." << (SI32)mSock->ClientIP2() << "." << (SI32)mSock->ClientIP1() << "]" << myendl;
				else
					Console << "Encrypted client attempting to cut in, disconnecting them: IP " << (SI32)mSock->ClientIP1() << "." << (SI32)mSock->ClientIP2() << "." << (SI32)mSock->ClientIP3() << "." << (SI32)mSock->ClientIP4() << myendl;
				Disconnect( s );
				return;
			}
#if _DEBUG_PACKET
			Console.Print( "Packet ID: 0x%x\n", packetID );
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
					if( pScript != NULL )
					{
						doSwitch = false;
						pScript->OnPacketReceive( mSock, packetID );
					}
				}
			}
			if( doSwitch )
			{
				CPInputBuffer *test	= WhichPacket( packetID, mSock );
				if( test != NULL )
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
						ourChar->SetTarg( NULL );
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
						Effects->dosocketmidi( mSock );
						ourChar->BreakConcentration( mSock );
						break;
					case 0x12:// Ext. Command
						mSock->Receive( 3 );
						mSock->Receive( mSock->GetWord( 1 ) );
						ourChar->BreakConcentration( mSock );
						if( buffer[3] == 0xC7 ) // Action
						{
							if( ourChar->IsOnHorse() ) // Ripper
								return; // can't bow or salute on horse
							if( !strcmp( (char *)&buffer[4], "bow" ) ) 
								if( ( (ourChar->GetID() >= 0x190) &&
									(ourChar->GetID() <= 0x193) ) ||
									(ourChar->GetID() == 0x3DB) )
									Effects->PlayCharacterAnimation( ourChar, 0x20 );
								else
									Effects->PlayCharacterAnimation( ourChar, 0x12 );
							if( !strcmp( (char *)&buffer[4], "salute" ) ) 
								if( ( (ourChar->GetID() >= 0x190) &&
									(ourChar->GetID() <= 0x193) ) ||
									(ourChar->GetID() == 0x3DB) )
									Effects->PlayCharacterAnimation( ourChar, 0x21 );
								else
									Effects->PlayCharacterAnimation( ourChar, 0x11 );
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
							Skills->SkillUse( mSock, UString( (char*)&buffer[4] ).toUByte() );
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
					// LB 7-dec 1999
					case 0x93:	// Old Book Packet, no longer sent by the client
						mSock->Receive( 99 );
						break;
					case 0x69:// Client text change
						mSock->Receive( 3 );// What a STUPID message...  It would be useful if
						mSock->Receive( mSock->GetWord( 1 ) );// it included the color changed to, but it doesn't!
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
	//	BYTE[2] cmdType (0x0001 � Update)
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
								mSock->Send( tempBuffer, mj );
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
					case 0xD9:
						break;
					default:
						FD_ZERO( &all );
						FD_SET( mSock->CliSocket(), &all );
						int nfds;
						nfds = mSock->CliSocket() + 1;
						if( select( nfds, &all, NULL, NULL, &cwmWorldState->uoxtimeout ) > 0 ) 
							mSock->Receive( 2560 );
						sprintf( temp, "Unknown message from client: 0x%02X - Ignored", packetID );
						Console << temp << myendl;
						break;
				}
			}
		}
		else	// Sortta error type thing, they disconnected already
			Disconnect( s );
	}
}


void cNetworkStuff::CheckLoginMessage( void ) // Check for messages from the clients
{
	fd_set all;
	fd_set errsock;
	size_t i;
	
	cwmWorldState->uoxtimeout.tv_sec = 0;
	cwmWorldState->uoxtimeout.tv_usec = 1;

	FD_ZERO( &all );
	FD_ZERO( &errsock );
	
	size_t nfds = 0;
	for( i = 0; i < loggedInClients.size(); ++i )
	{
		size_t clientSock = loggedInClients[i]->CliSocket();
		FD_SET( clientSock, &all );
		FD_SET( clientSock, &errsock );
		if( clientSock + 1 > nfds )
			nfds = clientSock + 1;
	}
	int s = select( nfds, &all, NULL, &errsock, &cwmWorldState->uoxtimeout );
	if( s > 0 )
	{
		size_t oldnow = loggedInClients.size();
		for( i = 0; i < loggedInClients.size(); ++i )
		{
			if( FD_ISSET( loggedInClients[i]->CliSocket(), &errsock ) )
			{
				LoginDisconnect( i );
				continue;
			}
			if( ( FD_ISSET( loggedInClients[i]->CliSocket(), &all ) ) && ( oldnow == loggedInClients.size() ) )
			{
				try
				{
					GetLoginMsg( i );
				}
				catch( socket_error& blah )
				{
#if UOX_PLATFORM != PLATFORM_WIN32
						messageLoop << "Client disconnected";
#else
					if( blah.ErrorNumber() == WSAECONNRESET )
						messageLoop << "Client disconnected";
					else if( blah.ErrorNumber() != -1 )
					{
						char temp[128];
						sprintf( temp, "Socket error: %li", blah.ErrorNumber() );
						messageLoop << temp;
					}
#endif
					LoginDisconnect( i );	// abnormal error
				}
			}
		}
	}
	else if( s == SOCKET_ERROR )
	{
#if UOX_PLATFORM == PLATFORM_WIN32
		int errorCode = WSAGetLastError();
		if( errorCode != 10022 )
			Console << (SI32)errorCode << myendl;
#endif
	}
}

void cNetworkStuff::LoginDisconnect( UOXSOCKET s ) // Force disconnection of player //Instalog
{
	char temp[128];
	sprintf( temp, "LoginClient %lu disconnected.", s );
	messageLoop << temp;
	loggedInClients[s]->FlushBuffer();
	loggedInClients[s]->CloseSocket();

	//Zippy 9/17/01 : fix for clients disconnecting during login not being able to reconnect.
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


// Transfers from the logged in queue to the in world queue
// Takes something out of the logging in queue and places it in the in world queue
// REQUIRES THREAD SAFETY
void cNetworkStuff::Transfer( CSocket *mSock )
{
	UOXSOCKET s = FindLoginPtr( mSock );
	if( s >= loggedInClients.size() )
		return;
	InternalControl.On();
	connClients.push_back( loggedInClients[s] );
	loggedInClients.erase( loggedInClients.begin() + s );
	cwmWorldState->SetPlayersOnline( connClients.size() );
	if( cwmWorldState->GetPlayersOnline() > peakConnectionCount )
		peakConnectionCount = cwmWorldState->GetPlayersOnline();
	InternalControl.Off();
	return;
}

void cNetworkStuff::GetLoginMsg( UOXSOCKET s )
{
	CSocket *mSock = loggedInClients[s];
	if( mSock == NULL )
		return;
	if( mSock->NewClient() )
	{
		int count, ho, mi, se, total;
		count = mSock->Receive( 4 );
		// March 1, 2004 - EviLDeD - Implemented support for UOG request for client connection count and possibly other server values
		if( memcmp(mSock->Buffer(),"UOG\0",sizeof(UI08)*4) == 0 && cwmWorldState->ServerData()->ServerUOGEnabled() ) // || (mSock->Buffer()[0]==46 && count<4)) // Commented out becuase the timing cycle in the recieve() member function in CSocket returns to fast and doesn't get the correct revieved byte count.
		{
			// ok this is UOG calling for server info. Just return the current data count and be done with it.
			UI08 szTBuf[512];
			memset((char*)szTBuf,0x00,sizeof(UI08)*512);
			total	= ( cwmWorldState->GetUICurrentTime() - cwmWorldState->GetStartTime() ) / 1000;
			ho		= total / 3600;
			total	-= ho * 3600;
			mi		= total / 60;
			total	-= mi * 60;
			se		= total;
			// April 5, 2004 - EviLDeD - Please leave the place holders incode. They are not read in from the ini as of yet but will be as I get time and solidify the exact values needed
			sprintf( (char*)szTBuf, "UOX3:sn=%s,cs=0x%04X,st=[ut:%02i:%02i:%02i][cn:%i][ic:%i][cc:%i][me:0x%08X][ma:0x%04X,%s,%s,%s,%s]\x0", cwmWorldState->ServerData()->ServerName().c_str(), cwmWorldState->ServerData()->GetClientFeatures(), ho, mi, se, cwmWorldState->GetPlayersOnline()+1, ObjectFactory::getSingleton().CountOfObjects( OT_ITEM ), ObjectFactory::getSingleton().CountOfObjects( OT_CHAR ), 0xDEADFEED, 0x000D, "Felucia", "Trammel", "Ilshenar", "Malas" );
			mSock->Send( (char*)szTBuf,strlen((char*)szTBuf)+1);
			mSock->NewClient( false );
		}
		else if( mSock->Buffer()[0] == 0x21 && count < 4 )	// UOMon
		{
			total	= ( cwmWorldState->GetUICurrentTime() - cwmWorldState->GetStartTime() ) / 1000;
			ho		= total / 3600;
			total	-= ho * 3600;
			mi		= total / 60;
			total	-= mi * 60;
			se		= total;
			UI08 uoxmonitor[100];
			sprintf( (char *)uoxmonitor, "UOX LoginServer\r\nUOX3 Server: up for %ih %im %is\r\n", ho, mi, se );
			mSock->Send( uoxmonitor, strlen( (char *)uoxmonitor ) );
			mSock->NewClient( false );
		} 
		else
		{
			if( mSock->Buffer()[0] == 0xEF || mSock->Buffer()[0] == 0xC0 )
			{
				WhichLoginPacket( mSock->Buffer()[0], mSock );
				//mSock->Receive( 21, false );
			}

			mSock->NewClient( false );
			if( mSock->GetDWord( 0 ) == 0x12345678 )
				LoginDisconnect( s );
			mSock->FirstPacket( true );
		}
	}
	else
	{
		mSock->InLength( 0 );
		UI08 *buffer = mSock->Buffer();
		if( mSock->Receive( 1, false ) > 0 )
		{
			char temp[128];
			UI08 packetID = buffer[0];
			if( mSock->FirstPacket() && packetID != 0x80 && packetID != 0x91 )
			{
				// April 5, 2004 - EviLDeD -  Hmmm there are two of these ?
				if(cwmWorldState->ServerData()->ServerUOGEnabled())
					sprintf( temp, "UOG Stats Sent or Encrypted client detected. [%i.%i.%i.%i]", mSock->ClientIP4(), mSock->ClientIP3(), mSock->ClientIP2(), mSock->ClientIP1() );
				else
					sprintf( temp, "Encrypted client detected. [%i.%i.%i.%i]", mSock->ClientIP4(), mSock->ClientIP3(), mSock->ClientIP2(), mSock->ClientIP1() );
				messageLoop << temp;
				LoginDisconnect( s );
				return;
			}
			else if( mSock->FirstPacket() && packetID == 0 )
			{
				sprintf( temp, "Buffer is empty, no packets to read. Disconnecting client. [%i.%i.%i.%i]", mSock->ClientIP4(), mSock->ClientIP3(), mSock->ClientIP2(), mSock->ClientIP1() ); 
				messageLoop << temp;
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
					if( pScript != NULL )
						doSwitch = !pScript->OnPacketReceive( mSock, packetID );
				}
			}
			if( doSwitch )
			{
				CPInputBuffer *test = NULL;
				try
				{
					test = WhichLoginPacket( packetID, mSock );
				}
				catch( socket_error& )
				{
					Console.Warning( "Bad packet request thrown! [packet ID: 0x%x]", packetID );
					mSock->FlushIncoming();
					return;
				}
				if( test != NULL )
				{
#ifdef _DEBUG_PACKET
					Console.Log( "Logging packet ID: 0x%X", "loginthread.txt", packetID );
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
						//Note:  It�s send again in current 3.0.x clients. When player does "paper-doll logout"
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
						int nfds;
						fd_set all;
						FD_ZERO( &all );
						FD_SET( mSock->CliSocket(), &all );
						nfds = mSock->CliSocket() + 1;
						if( select( nfds, &all, NULL, NULL, &cwmWorldState->uoxtimeout ) > 0 ) 
							mSock->Receive( 2560 );
						sprintf( temp, "Unknown message from client: 0x%02X - Ignored", packetID );
						messageLoop << temp;
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
	return INVALID_SOCKET;
}

CSocket *cNetworkStuff::FirstSocket( void )
{
	CSocket *retSock = NULL;
	currConnIter = connClients.begin();
	if( !FinishedSockets() )
		retSock = (*currConnIter);
	return retSock;
}
CSocket *cNetworkStuff::NextSocket( void )
{
	CSocket *retSock = NULL;
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
		return NULL;
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
		return NULL;
}

void cNetworkStuff::LoadFirewallEntries( void )
{
	UString token;
	std::string fileToUse;
	if( !FileExists( "banlist.ini" ) )
	{
		if( FileExists( "firewall.ini" ) )
			fileToUse = "firewall.ini";
	}
	else
		fileToUse = "banlist.ini";
	if( !fileToUse.empty() ) 
	{
		Script *firewallData = new Script( fileToUse, NUM_DEFS, false );
		if( firewallData != NULL )
		{
			SI16 p[4];
			ScriptSection *firewallSect = NULL;
			UString tag, data;
			for( firewallSect = firewallData->FirstEntry(); firewallSect != NULL; firewallSect = firewallData->NextEntry() )
			{
				if( firewallSect != NULL )
				{
					for( tag = firewallSect->First(); !firewallSect->AtEnd(); tag = firewallSect->Next() )
					{
						if( tag.upper() == "IP" )
						{
							data = firewallSect->GrabData();
							if( !data.empty() )
							{
								if( data.sectionCount( "." ) == 3 )	// Wellformed IP address
								{
									for( UI08 i = 0; i < 4; ++i )
									{
										token = data.section( ".", i, i );
										if( token == "*" )
											p[i] = -1;
										else
											p[i] = token.toShort();
									}
									slEntries.push_back( FirewallEntry( p[0], p[1], p[2], p[3] ) );
								}
								else if( data != "\n" && data != "\r" )
									Console.Error( "Malformed IP address in banlist.ini (line: %s)", data.c_str() );
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
	UI16 packetID = static_cast<UI16>((subCmd<<8) + packet);
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

void cNetworkStuff::PushConn( void )
{
	On();
	connIteratorBackup.push_back( currConnIter );
}
void cNetworkStuff::PopConn( void )
{
	currConnIter = connIteratorBackup.back();
	connIteratorBackup.pop_back();
	Off();
}

void cNetworkStuff::PushLogg( void )
{
	On();
	loggIteratorBackup.push_back( currLoggIter );
}
void cNetworkStuff::PopLogg( void )
{
	currLoggIter = loggIteratorBackup.back();
	loggIteratorBackup.pop_back();
	Off();
}

}
