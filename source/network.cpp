#include "uox3.h"
#include "debug.h"

#ifdef __LINUX__
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/ioctl.h>
#endif

#undef DBGFILE
#define DBGFILE "Network.cpp"

//#define DEBUG_PACKET

void cNetworkStuff::FlushBuffer( UOXSOCKET s ) // Sends buffered data at once
{
	connClients[s]->FlushBuffer();
}

void cNetworkStuff::ClearBuffers( void ) // Sends ALL buffered data
{
	for( int i = 0; i < now; i++ )
		FlushBuffer( i );
	for( UI32 j = 0; j < loggedInClients.size(); j++ )
		loggedInClients[j]->FlushBuffer();
	for( UI32 k = 0; k < xgmClients.size(); k++ )
		xgmClients[k]->FlushBuffer();
}

// set the laston character member value to the current date/time
void cNetworkStuff::setLastOn( cSocket *s )
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
		for( int end = strlen(t) - 1; end >= 0 && isspace(t[end]); --end )
			t[end] = '\0';
	}
	if( s->CurrcharObj() != NULL )
		s->CurrcharObj()->SetLastOn( t );
}

void cNetworkStuff::Disconnect( UOXSOCKET s ) // Force disconnection of player //Instalog
{
	setLastOn( connClients[s] );
	CChar *currChar = connClients[s]->CurrcharObj();
	Console << "Client " << (UI32)s << " disconnected. [Total:" << (SI32)(now-1) << "]" << myendl;

	char temp[1024];

	if( currChar != NULL )
	{
		if( currChar->GetAccount().wAccountIndex == connClients[s]->AcctNo() && cwmWorldState->ServerData()->GetServerJoinPartAnnouncementsStatus() ) 
		{
			sprintf( temp, Dictionary->GetEntry( 752 ), currChar->GetName() );
			sysbroadcast( temp );//message upon leaving a server 
		} 
	}
	if( connClients[s]->AcctNo() != -1 ) 
	{
		ACCOUNTSBLOCK actbAccount;
		actbAccount=connClients[s]->GetAccount();
		if(actbAccount.wAccountIndex!=AB_INVALID_ID)
		{
			if(actbAccount.wAccountIndex!=AB_INVALID_ID)
				actbAccount.wFlags&=0xFFFFFFF7;
		}
	}
	//Instalog
	if( currChar != NULL )
	{
		if( !currChar->isFree() && isOnline( currChar ) )
		{
			// October 6, 2002 - Brakhtus Support for the onLogout event.
			cScript *onLogoutScp = Trigger->GetScript(currChar->GetScriptTrigger());
			if(onLogoutScp!=NULL)
				onLogoutScp->OnLogout(connClients[s], currChar);
			else
			{
				onLogoutScp = Trigger->GetScript(0);
				if(onLogoutScp!=NULL)
					onLogoutScp->OnLogout(connClients[s],currChar);
			}
			LogOut( s );
		}
	}
	try
	{
		connClients[s]->AcctNo( -1 );
		connClients[s]->IdleTimeout( -1 );
		connClients[s]->FlushBuffer();
		connClients[s]->CloseSocket();
		delete connClients[s];
		connClients.erase( connClients.begin() + s );
		for( int q = 0; q < connIteratorBackup.size(); q++ )
		{
			if( connIteratorBackup[q] >= s )
				connIteratorBackup[q]--;
		}
	}
	catch(...)
	{
		Console << "| CATCH: Invalid connClients[] encountered. Ignoring." << myendl;
	}
	now--;
	WhoList->FlagUpdate();
	OffList->FlagUpdate();
}


//o--------------------------------------------------------------------------o
//|	Function			-	void cNetworkStuff::Login1( cSocket *s )
//|	Date					-	
//|	Developers		-	
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Process phase one of the login server process.
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
void cNetworkStuff::Login1( cSocket *s )
{	
	s->AcctNo( -1 );
	LoginDenyReason t = LDR_NODENY;
	ACCOUNTSBLOCK actbTemp;
	if(Accounts->GetAccountByName((char *)&s->Buffer()[1],actbTemp))
		s->SetAccount(actbTemp);
	// Need auto account creation code here
	if( s->AcctNo() == -1 || s->AcctNo()==0xffff)
	{
		if( cwmWorldState->ServerData()->GetInternalAccountStatus() )
		{
			pSplit( (char *)&s->Buffer()[31] );
			Accounts->AddAccount( (char *)&s->Buffer()[1], pass1, "" );
			if(Accounts->GetAccountByName( (char *)&s->Buffer()[1],actbTemp))	// grab our account number
				s->SetAccount(actbTemp);
		}
	}
	if( s->AcctNo() != -1 )
	{
		pSplit( (char *)&s->Buffer()[31] );
		if(actbTemp.wFlags&AB_FLAGS_BANNED)
			t = LDR_ACCOUNTDISABLED;
		else if( strcmp(actbTemp.sPassword.c_str(), pass1 ) )
			t = LDR_BADPASSWORD;
		if( t != LDR_NODENY )
		{
			CPLoginDeny pckDeny( t );
			s->Send( &pckDeny );
			s->AcctNo( -1 );
			LoginDisconnect( s );
			return;
		}
	}
	else
	{
		CPLoginDeny noAcct( LDR_UNKNOWNUSER );
		s->Send( &noAcct );
		LoginDisconnect( s );
		return;
	}
	if(actbTemp.wFlags&AB_FLAGS_ONLINE)
	{
		CPLoginDeny inUse( LDR_ACCOUNTINUSE );
		s->Send( &inUse );
		LoginDisconnect( s );
		return;
	}
	if( s->AcctNo() != -1 )
		Login2( s, actbTemp );

}

//o--------------------------------------------------------------------------o
//|	Function			-	void cNetworkStuff::Login2(cSocket *s, ACCOUNTSBLOCK &actbBlock)
//|	Date					-	
//|	Developers		-	
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Process the second phase of a standard login procedure.
//|									Usualy the only time this function will be called is when
//|									a successfull phase one login, and correct server selection
//|									has been made, and submitted by the client
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
void cNetworkStuff::Login2(cSocket *s, ACCOUNTSBLOCK &actbBlock)
{
	char temp[1024];
	char newlist1[7] = "\xA8\x01\x23\xFF\x00\x01";
	cPBuffer bufNewlist1( newlist1, 6 );
	char newlist2[41] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x12\x01\x7F\x00\x00\x01";
	actbBlock.dwLastIP=calcserial( s->ClientIP4(), s->ClientIP3(), s->ClientIP2(), s->ClientIP1() );
	sprintf( temp, "Client [%i.%i.%i.%i] connected using Account '%s'.", s->ClientIP4(), s->ClientIP3(), s->ClientIP2(), s->ClientIP1() , &s->Buffer()[1]);
	savelog( temp, "server.log" );
	messageLoop << temp;

	actbBlock.wFlags|=AB_FLAGS_ONLINE;

	UI32 servcount = cwmWorldState->ServerData()->GetServerCount();
	UI32 tlen = 6 + ( servcount * 40 );
	bufNewlist1[1] = (UI08)( tlen>>8 );
	bufNewlist1[2] = (UI08)( tlen%256 );
	bufNewlist1[4] = (UI08)( servcount>>8 );
	bufNewlist1[5] = (UI08)( servcount%256 );

	s->Send( &bufNewlist1 );
	for( UI16 i = 0; i < servcount; i++ )
	{
		physicalServer *sData = cwmWorldState->ServerData()->GetServerEntry( i );
		newlist2[0] = (UI08)((i+1)>>8);
		newlist2[1] = (UI08)((i+1)%256);
		strcpy( &newlist2[2], sData->getName().c_str() );
		UI32 ip = htonl( inet_addr( sData->getIP().c_str() ) );
		newlist2[36] = (UI08)( ip>>24 );
		newlist2[37] = (UI08)( ip>>16 );
		newlist2[38] = (UI08)( ip>>8 );
		newlist2[39] = (UI08)( ip%256 );
		s->Send( newlist2, 40 );
	}
}


//o--------------------------------------------------------------------------o
//|	Function			-	void cNetworkStuff::GoodAuth( cSocket *s, ACCOUNTSBLOCK& actbBlock)
//|	Date					-	
//|	Developers		-	
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Send this connectiona successful authentication packet
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
void cNetworkStuff::GoodAuth( cSocket *s, ACCOUNTSBLOCK& actbBlock)
{
	UI08 charCount = 0;
	for( UI32 i = 0; i < 5; i++ )
	{
		if(actbBlock.dwCharacters[i] != 0xffffffff )
			charCount++;
	}
	cServerData *sd = cwmWorldState->ServerData();
	UI32 serverCount = sd->GetNumServerLocations();
	CPCharAndStartLoc toSend(actbBlock, charCount, serverCount );
	for( UI32 j = 0; j < serverCount; j++ )
	{
		toSend.AddStartLocation( sd->GetServerLocation( j ), j );
	}
	s->Send( &toSend );
}

void cNetworkStuff::pSplit( char *pass0 ) // Split login password into UOX password and UO password
{
	int i = 0;
	pass1[0] = 0;
	while( pass0[i] != '/' && pass0[i] != 0 ) 
		i++;
	strncpy( pass1, pass0, i );
	pass1[i] = 0;
	if( pass0[i] != 0 ) 
		strcpy( pass2, pass0 + i + 1 );
}

//o--------------------------------------------------------------------------o
//|	Function			-	void cNetworkStuff::LogOut( UOXSOCKET s )
//|	Date					-	
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Process client logout, and process instalog settings
//|
//|	Modification	-	Date Unknown - Added multi checked to instalog processing
//o--------------------------------------------------------------------------o
void cNetworkStuff::LogOut( UOXSOCKET s )
{
	// This would probably be a good place to do the disconnect
	CChar *p = connClients[s]->CurrcharObj();
	int a;
	bool valid = false;
	SI16 x = p->GetX(), y = p->GetY();
	CMultiObj *multi = NULL;

	if( p->GetCommandLevel() >= CNSCMDLEVEL || p->GetAccount().wAccountIndex == 0 ) 
		valid = true;
	else 
	{
		for( a = 0; a < logoutcount; a++ )
		{
			if( logout[a].x1 <= x && logout[a].y1 <= y && logout[a].x2 >= x && logout[a].y2 >= y )
			{
				valid = true;
				break;
			}
		}
	}

	if( !valid )
	{
		if( p->GetMulti() == INVALIDSERIAL )
			multi = findMulti( x, y, p->GetZ(), p->WorldNumber() );
		else 
			multi = static_cast<CMultiObj *>(calcItemObjFromSer( p->GetMulti() ));
	}
	
	if( multi != NULL && !valid )//It they are in a multi... and it's not already valid (if it is why bother checking?)
	{
		CItem *b = getPack( p );
		if( b != NULL )
		{
			for( CItem *getItem = b->FirstItemObj(); !b->FinishedItems(); getItem = b->NextItemObj() )
			{
				if( getItem != NULL )
				{
					if( getItem->GetType() == 7 && getItem->GetMore() == multi->GetSerial() )
					{//a key to this multi
						valid = true;//Log 'em out now!
						break;
					}
				}
			}
		}
	}
	
	ACCOUNTSBLOCK actbAccount;
	actbAccount=p->GetAccount();
	if( valid )
	{
		if(actbAccount.wAccountIndex!=0xffff)
			actbAccount.dwInGame=INVALIDSERIAL;
		p->SetLogout( -1 );
	}
	else
	{
		if(actbAccount.wAccountIndex!=0xffff)
			actbAccount.dwInGame = p->GetSerial();
		p->SetLogout( BuildTimeValue( cwmWorldState->ServerData()->GetSystemTimerStatus( LOGIN_TIMEOUT ) ) );
	}
	p->Teleport();
}

void cNetworkStuff::sockInit( void )
{
	char h1=0, h2=0, h3=0, h4=0;
	int bcode;
	
	kr = true;
	faul = false;
	
#ifndef __NT__
	int on = 1;
#endif
	
	a_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( a_socket < 0 )
	{
		Console.Error( 0, " Unable to create socket");
#ifdef __NT__
		Console.Error( 0, " Code %i", WSAGetLastError() );
#else
		Console << myendl;
#endif
		keeprun = false;
		error = true;
		kr = false;
		faul = true;
		Shutdown( FATAL_UOX3_ALLOC_NETWORK );
		return;
	}
#ifndef __NT__
	setsockopt( a_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof( on ) );
#endif
	
	UI32 len_connection_addr = sizeof( struct sockaddr_in );
	sockaddr_in connection;
	memset( (char *) &connection, 0, len_connection_addr );
	connection.sin_family = AF_INET;
	connection.sin_addr.s_addr = htonl( INADDR_ANY );
	connection.sin_port = htons( cwmWorldState->ServerData()->GetServerPort() );
	bcode = bind( a_socket, (struct sockaddr *)&connection, len_connection_addr );
	
	if( bcode < 0 )
	{
		Console.Error( 0, " Unable to bind socket 1 - Error code: %i", bcode );
		keeprun = false;
		error = true;
		kr = false;
		faul = true;
		Shutdown( FATAL_UOX3_ALLOC_NETWORK );
		return;
	}
    
	
	UI32 mode = 1;
	// set the socket to nonblocking
	ioctlsocket( a_socket, FIONBIO, &mode );

	listen( a_socket, 42 );
	ph1 = h1;
	ph2 = h2;
	ph3 = h3;
	ph4 = h4;
}

void cNetworkStuff::SockClose( void ) // Close all sockets for shutdown
{
	int i;
	closesocket( a_socket );
	for( i = 0; i < now; i++ )
		connClients[i]->CloseSocket();
}

void cNetworkStuff::CheckConn( void ) // Check for connection requests
{
	FD_ZERO( &conn );
	FD_SET( a_socket, &conn );
	int nfds = a_socket + 1;
	int s = select( nfds, &conn, NULL, NULL, &uoxtimeout );
	if( s > 0 )
	{
		int len = sizeof( struct sockaddr_in );
		int newClient;
#ifndef __LINUX__
		newClient = accept( a_socket, (struct sockaddr *)&client_addr, &len );
#else
		newClient = accept( a_socket, (struct sockaddr *)&client_addr, (socklen_t *)&len );
#endif
		cSocket *toMake = new cSocket( newClient );
		if( newClient < 0 )
		{
#ifndef __LINUX__
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
			Console.Error( 0, "Error at client connection!" );
			error = true;
			keeprun = true;
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
		sprintf( temp, "FIREWALL: Forwarding address %i.%i.%i.%i --> Access Granted!", part[0], part[1], part[2], part[3] );
		messageLoop << temp;
		sprintf( temp, "Client %i [%i.%i.%i.%i] connected [Total:%i]", now, part[0], part[1], part[2], part[3], now + 1 );
		messageLoop << temp;
		loggedInClients.push_back( toMake );
		toMake->ClientIP( client_addr.sin_addr.s_addr );
		return;
	}
	if( s < 0 )
	{
		Console.Error( 0, " Select (Conn) failed!" );
		keeprun = false;
		error = true;
		return;
	}
}

bool cNetworkStuff::IsFirewallBlocked( UI08 part[4] )
{
	bool match[4];
	for( int i = 0; i < slEntries.size(); i++ )
	{
		for( UI08 k = 0; k < 4; k++ )
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
	int i, s = 0;
	for( i = 0; i < loggedInClients.size(); i++ )
	{
		loggedInClients[i]->FlushBuffer();
		loggedInClients[i]->CloseSocket();
		delete loggedInClients[i];
	}
	for( i = 0; i < now; i++ )
	{
		FlushBuffer( i );
		connClients[i]->CloseSocket();
		s = max( s, connClients[i]->CliSocket() + 1 );
		delete connClients[i];
	}
	closesocket( s );
	ShutdownXGM();
#ifdef _WIN32
	WSACleanup();
#endif
}

void cNetworkStuff::CheckMessage( void ) // Check for messages from the clients
{
	int i;
	
	FD_ZERO(&all);
	FD_ZERO(&errsock);
	nfds = 0;
	for( i = 0; i < now; i++ )
	{
		int clientSock = connClients[i]->CliSocket();
		FD_SET( clientSock, &all );
		FD_SET( clientSock, &errsock );
		if( clientSock + 1 > nfds )
			nfds = clientSock + 1;
	}
	int s = select( nfds, &all, NULL, &errsock, &uoxtimeout );
	if( s > 0 )
	{
		int oldnow = now;
		for( i = 0; i < now; i++ )
		{
			if( FD_ISSET( connClients[i]->CliSocket(), &errsock ) )
			{
				Disconnect( i );
				//Disconnect( connClients[i]->CliSocket() );
			}
			if( ( FD_ISSET( connClients[i]->CliSocket(), &all ) ) && ( oldnow == now ) )
			{
				try
				{
					GetMsg( i );
					//GetMsg(connClients[i]->CliSocket());
				}
				catch( socket_error& blah )
				{
#ifdef __LINUX__
						Console << "Client disconnected" << myendl;
#else
					if( blah.ErrorNumber() == WSAECONNRESET )
						Console << "Client disconnected" << myendl;
					else if( blah.ErrorNumber() != -1 )
						Console << "Socket error: " << (SI32)blah.ErrorNumber() << myendl;
#endif
					Disconnect( i );	// abnormal error
				}
				if( executebatch ) 
					batchcheck( GetSockPtr( i ) );
			}
		}
	}
	else if( s == SOCKET_ERROR )
	{
//		Console << WSAGetLastError() << myendl;
//		Console << WSANOTINITIALISED << myendl;
	}
}


cNetworkStuff::cNetworkStuff() : xgmRunning( false ), peakConnectionCount( 0 ) // Initialize sockets
{
	sockInit();
	LoadFirewallEntries();
	StartupXGM(32621);
}

cSocket& cNetworkStuff::GetSocket( UOXSOCKET s )
{
	return *(connClients[s]);
}

cSocket *cNetworkStuff::GetSockPtr( UOXSOCKET s )
{
	if( s < 0 || s >= connClients.size() )
		return NULL;
	return connClients[s];
}

cPInputBuffer *WhichPacket( UI08 packetID );

void cNetworkStuff::GetMsg( UOXSOCKET s ) // Receive message from client
{
	if( s == -1 )
		return;

	int count, ho, mi, se, total, j, book;
	SERIAL serial;
	CItem *i = NULL;
	CChar *c = NULL;
	char nonuni[512];
	char temp[1024];
	cSocket *mSock = GetSockPtr( s );
	cSocket *tSock = NULL;

	if( mSock == NULL )
		return;

	int myoffset = 13;
	int myincrement = 2;
	int myj = 12;

	if( mSock->NewClient() )
	{
		count = mSock->Receive( 4 );
		if( mSock->Buffer()[0] == 0x21 && count < 4 )	// UOMon
		{
			total = ( uiCurrentTime - starttime ) / CLOCKS_PER_SEC;
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
		UI08 packetID;
		UI08 *buffer = mSock->Buffer();
		if( mSock->Receive( 1 ) > 0 )
		{
			packetID = buffer[0];
			if( mSock->FirstPacket() && packetID != 0x80 && packetID != 0x91 )
			{
				Console << "Non-ignition client attempting to cut in, disconnecting them: IP " << (SI32)mSock->ClientIP1() << "." << (SI32)mSock->ClientIP2() << "." << (SI32)mSock->ClientIP3() << "." << (SI32)mSock->ClientIP4() << myendl;
				Disconnect( s );
				return;
			}
			if( packetID != 0x73 && packetID != 0xA4 && packetID != 0x80 && packetID != 0x91 )
				mSock->IdleTimeout( BuildTimeValue( cwmWorldState->ServerData()->GetSystemTimerStatus( LOGIN_TIMEOUT ) ) );
			cPInputBuffer *test = WhichPacket( packetID );
			bool doSwitch = true;
			if( test != NULL )
			{
				test->SetSocket( mSock );
				test->Receive( mSock );
				if( test->Handle() )
					doSwitch = false;
				delete test;

			
			}
			if( doSwitch )
			{
				CItem *speechItem = NULL;
				UI16 scpTrig;
				cScript *myScript;
				

				switch( packetID )
				{
				case 0x01: // Main Menu on the character select screen
					Disconnect( s );
					break;
				case 0x04:	// God client toggle
					mSock->Receive( 2 );
					if( ourChar->IsGM() )
					{
						CPGodModeToggle cpgmt = mSock;
						mSock->Send( &cpgmt );
					}
					else
					{
						sysmessage( mSock, 1641 );
						Console << "Godclient detected - Account[" << ourChar->GetAccount().wAccountIndex << "] Usename[" << ourChar->GetAccount().sUsername << ". Client disconnected!" << myendl;
						Disconnect( s );
					}
					break;
				case 0x02:// Walk
					mSock->Receive( 7 );
					Movement->Walking( ourChar, buffer[1], buffer[2] );
					ourChar->BreakConcentration( mSock );
					break;
				case 0x73:// Keep alive
					mSock->Receive( 2 );
					mSock->Send( buffer, 2 );
					break;
				case 0x22:// Resync Request
					mSock->Receive( 3 );
					break;
				case 0x03:// Speech
					mSock->Receive( 3 );
					mSock->Receive( mSock->GetWord( 1 ) );
					ourChar->BreakConcentration( mSock );
					ourChar->setUnicode( false );
					speechItem = calcItemObjFromSer( ourChar->GetSpeechItem() );
					switch( ourChar->GetSpeechMode() )
					{
					case 3: // Player vendor item pricing
						j = makeNum( (char *)&buffer[8] );
						if( j > 0 )
						{
							speechItem->SetValue( j );
							sysmessage( mSock, 753, j );
						} 
						else 
							sysmessage( mSock, 754, speechItem->GetValue() );
						sysmessage( mSock, 755 );
						ourChar->SetSpeechMode( 4 );
						return;
					case 4: // Player vendor item describing
						speechItem->SetDesc( (char *)&buffer[8] );
						sysmessage( mSock, 756, &buffer[8] );
						ourChar->SetSpeechMode( 0 );
						return;
					case 7: // Rune renaming
						char tempname[512];
						sprintf( tempname, Dictionary->GetEntry( 684 ), &buffer[8] );
						speechItem->SetName( tempname );
						sysmessage( mSock, 757, &buffer[8] );
						ourChar->SetSpeechMode( 0 );
						return;
					case 6: // Name deed
						ourChar->SetName( (char*)&buffer[8] );
						sysmessage( mSock, 758, &buffer[8] );
						ourChar->SetSpeechMode( 0 );
						Items->DeleItem( speechItem );
						ourChar->SetSpeechItem( INVALIDSERIAL );
						return;
					case 5:	// Key renaming
					case 8: // Sign renaming
						speechItem->SetName( (char*)&buffer[8] );
						sysmessage( mSock, 759, &buffer[8] );
						ourChar->SetSpeechMode( 0 );
						return;
					case 9: // JavaScript speech
						scpTrig = ourChar->GetSpeechCallback();
						myScript = Trigger->GetScript( scpTrig );
						
						if( myScript != NULL )
							myScript->OnSpeechInput( ourChar, speechItem, (char*)&buffer[8] );

						ourChar->SetSpeechMode( 0 );
						return;
					case 1: // GM Page
						UI08 a1, a2, a3, a4;
						if( GMQueue->GotoPos( GMQueue->FindCallNum( ourChar->GetPlayerCallNum() ) ) )
						{
							HelpRequest *tempPage = NULL;
							tempPage = GMQueue->Current();
							a1 = ourChar->GetSerial( 1 );
							a2 = ourChar->GetSerial( 2 );
							a3 = ourChar->GetSerial( 3 );
							a4 = ourChar->GetSerial( 4 );

							tempPage->Reason( (char*)&buffer[8] );
							tempPage->WhoPaging( ourChar->GetSerial() );
							sprintf( temp, "GM Page from %s [%x %x %x %x]: %s", ourChar->GetName(), a1, a2, a3, a4, tempPage->Reason() );
							bool x = false;
							Network->PushConn();
							for( tSock = FirstSocket(); !FinishedSockets(); tSock = NextSocket() )
							{
								CChar *tChar = tSock->CurrcharObj();
								if( tChar == NULL )
									continue;

								if( tChar->IsGMPageable() )
								{
									x = true;
									sysmessage( tSock, temp );
								}
							}
							Network->PopConn();
							if( x )
								sysmessage( mSock, 363 );
							else 
								sysmessage( mSock, 364 );
						}
						ourChar->SetPageGM( 0 );
						ourChar->SetSpeechMode( 0 );
						return;
					case 2: // Counselor Page
						a1 = ourChar->GetSerial( 1 );
						a2 = ourChar->GetSerial( 2 );
						a3 = ourChar->GetSerial( 3 );
						a4 = ourChar->GetSerial( 4 );

						HelpRequest *tempPageCns;
						if( CounselorQueue->GotoPos( CounselorQueue->FindCallNum( ourChar->GetPlayerCallNum() ) ) )
						{
							tempPageCns = CounselorQueue->Current();
							tempPageCns->Reason( (char*)&buffer[8] );
							tempPageCns->WhoPaging( ourChar->GetSerial() );
							sprintf( temp, "Counselor Page from %s [%x %x %x %x]: %s", ourChar->GetName(), a1, a2, a3, a4, tempPageCns->Reason() );
							bool x = false;
							Network->PushConn();
							for( tSock = FirstSocket(); !FinishedSockets(); tSock = NextSocket() )
							{
								CChar *tChar = tSock->CurrcharObj();
								if( tChar == NULL )
									continue;

								if( tChar->IsGMPageable() )
								{
									x = true;
									sysmessage( tSock, temp );
								}
							}
							Network->PopConn();
							if( x )
								sysmessage( mSock, 360 );
							else 
								sysmessage( mSock, 361 );
						}

						ourChar->SetPageGM( 0 );
						ourChar->SetSpeechMode( 0 );
					case 0: // normal speech
					default:
						if( ourChar->GetSquelched() )
							sysmessage( mSock, 760 );
						else 
							talking( mSock );
						break;
					}
					break;
				case 0xAD: // Unicode Speech
					mSock->Receive( 3 );
					mSock->Receive( mSock->GetWord( 1 ) );
					ourChar->setUnicode( true );

					// Check for command word versions of this packet
					myoffset = 13;
					myincrement = 2;
					myj = 12;

					switch( buffer[3] )
					{
					case 0xC0:
					case 0xC9:
					case 0xC1:
					case 0xC2:
					case 0xC6:
					case 0xC8:
						myincrement = 1;
						buffer[3] &= 0x0F;	// set to normal to send it back
						switch( buffer[13] & 0xF0 )
						{		// Copy the buffer up, and convert it to unicode
						case 0x10:	myoffset = 15;		break;
						case 0x20:	myoffset = 17;		break;
						case 0x30:	myoffset = 18;		break;
						case 0x40:	myoffset = 20;		break;
						}
						//
						//	Now adjust the buffer
						int iWord;
						iWord = mSock->GetWord( 1 );
						myj = 12;
						char mytempbuf[512];
						int mysize;
						mysize = iWord - myoffset;
						for( j = 0; j < mysize; j++ )
							mytempbuf[j] = buffer[j + myoffset];
						for( j = 0; j < mysize; j++ )
						{	// we would overwrite our buffer, so we need to catch it before we do that.
							buffer[myj++] = 0;
							buffer[myj++] = mytempbuf[j];
						}
						buffer[myj-1] = 0;
						iWord = ((iWord - myoffset) * 2) + 12;
						buffer[1] = static_cast< UI08 >((( iWord & 0xFF00 ) >> 8 ));
						buffer[2] = static_cast< UI08 >(iWord & 0x00FF);
						break;
					default:
						break;
					}
					ourChar->BreakConcentration( mSock );
					for( j = 13; j < mSock->GetWord( 1 ); j += 2 )
						nonuni[(j-13)/2] = buffer[j];
					speechItem = calcItemObjFromSer( ourChar->GetSpeechItem() );
					switch( ourChar->GetSpeechMode() )
					{
					case 3:
						j = makeNum( nonuni );
						if( j > 0 )
						{
							speechItem->SetValue( j );
							sysmessage( mSock, 753, j );
							sysmessage( mSock, 755 );
						} 
						else 
						{
							sysmessage( mSock, 754, speechItem->GetValue() );
							sysmessage( mSock, 755 );
						}
						ourChar->SetSpeechMode( 4 );
						return;
					case 4:
						speechItem->SetDesc( nonuni );
						sysmessage( mSock, 756, nonuni );
						ourChar->SetSpeechMode( 0 );
						return;
					case 7:
						char tempname[512];
						sprintf( tempname, Dictionary->GetEntry( 684 ), nonuni );
						speechItem->SetName( tempname );
						sysmessage( mSock, 757, nonuni );
						ourChar->SetSpeechMode( 0 );
						return;
					case 6:
						ourChar->SetName( nonuni );
						sysmessage( mSock, 758, nonuni );
						Items->DeleItem( speechItem );
						ourChar->SetSpeechMode( 0 );
						ourChar->SetSpeechItem( INVALIDSERIAL );
						return;
					case 5:
					case 8:
						speechItem->SetName( nonuni );
						sysmessage( mSock, 759, nonuni );
						ourChar->SetSpeechMode( 0 );
						return;
					case 9: // JavaScript speech
						scpTrig = ourChar->GetSpeechCallback();
						myScript = Trigger->GetScript( scpTrig );
						
						if( myScript != NULL )
							myScript->OnSpeechInput( ourChar, speechItem, nonuni );

						ourChar->SetSpeechMode( 0 );
						return;
					case 1:
						UI08 a1, a2, a3, a4;
						a1 = ourChar->GetSerial( 1 );
						a2 = ourChar->GetSerial( 2 );
						a3 = ourChar->GetSerial( 3 );
						a4 = ourChar->GetSerial( 4 );
						if( GMQueue->GotoPos( GMQueue->FindCallNum( ourChar->GetPlayerCallNum() ) ) )
						{
							HelpRequest *tempPage;
							tempPage = GMQueue->Current();
							tempPage->Reason( nonuni );
							tempPage->WhoPaging( ourChar->GetSerial() );
							sprintf(temp, "GM Page from %s [%x %x %x %x]: %s", ourChar->GetName(), a1, a2, a3, a4, nonuni );
							bool x = false;
							Network->PushConn();
							for( tSock = FirstSocket(); !FinishedSockets(); tSock = NextSocket() )
							{
								CChar *tChar = tSock->CurrcharObj();
								if( tChar == NULL )
									continue;

								if( tChar->IsGMPageable() )
								{
									x = true;
									sysmessage( tSock, temp );
								}
							}
							Network->PopConn();
							if( x )
								sysmessage( mSock, 363 );
							else 
								sysmessage( mSock, 364 );
						}
						ourChar->SetPageGM( 0 );
						ourChar->SetSpeechMode( 0 );
						return;
					case 2:
						a1 = ourChar->GetSerial( 1 );
						a2 = ourChar->GetSerial( 2 );
						a3 = ourChar->GetSerial( 3 );
						a4 = ourChar->GetSerial( 4 );
						if( CounselorQueue->GotoPos( CounselorQueue->FindCallNum( ourChar->GetPlayerCallNum() ) ) )
						{
							HelpRequest *tempPageCns;
							tempPageCns = CounselorQueue->Current();
							tempPageCns->Reason( nonuni );
							tempPageCns->WhoPaging( ourChar->GetSerial() );
							sprintf( temp, "Counselor Page from %s [%x %x %x %x]: %s", ourChar->GetName(), a1, a2, a3, a4, tempPageCns->Reason() );
							bool x = false;
							Network->PushConn();
							for( tSock = FirstSocket(); !FinishedSockets(); tSock = NextSocket() )
							{
								CChar *tChar = tSock->CurrcharObj();
								if( tChar == NULL )
									continue;

								if( tChar->IsGMPageable() )
								{
									x = true;
									sysmessage( tSock, temp );
								}
							}
							Network->PopConn();
							if( x )
								sysmessage( mSock, 360 );
							else 
								sysmessage( mSock, 361 );
						}
						ourChar->SetPageGM( 0 );
						ourChar->SetSpeechMode( 0 );
						return;
					case 0:
					default:
						if( ourChar->GetSquelched() )
							sysmessage( mSock, 760 );
						else 
							unicodetalking( mSock );
						break;
					}
					break;    
				case 0x06:// Doubleclick
					mSock->Receive( 5 );
					doubleClick( mSock );
					ourChar->BreakConcentration( mSock );
					break;
				case 0x09:// Singleclick
					mSock->Receive( 5 );
					singleClick( mSock );
					break;
				case 0x6C:// Targeting
					mSock->Receive( 19 );
					if( mSock->TargetOK() )
						Targ->MultiTarget( mSock );
					ourChar->BreakConcentration( mSock );
					break;
				case 0x13:// Equip Item
					mSock->Receive( 10 );
					wearItem( mSock );
					ourChar->BreakConcentration( mSock );
					break;
				case 0x07:// Get Item
					mSock->Receive( 7 );
					grabItem( mSock );
					ourChar->BreakConcentration( mSock );
					break;
				case 0x08:// Drop Item
					mSock->Receive( 14 );
					if( buffer[10] >= 0x40 && buffer[10] != 0xFF )
						packItem( mSock );
					else 
						dropItem( mSock );
					ourChar->BreakConcentration( mSock );
					break;
				case 0x72:// Combat Mode
					mSock->Receive( 5 );
					ourChar->SetWar( buffer[1] != 0 );
					ourChar->SetTarg( INVALIDSERIAL );
					if( ourChar->GetTimeout() <= uiCurrentTime )
						ourChar->SetTimeout( BuildTimeValue( 1 ) );
					else
						ourChar->SetTimeout( ourChar->GetTimeout() + CLOCKS_PER_SEC );
					mSock->Send( buffer, 5 );
					Movement->CombatWalk( ourChar );
					dosocketmidi( mSock );
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
								action( mSock, 0x20 );
							else
								action( mSock, 0x12 );
						if( !strcmp( (char *)&buffer[4], "salute" ) ) 
							if( ( (ourChar->GetID() >= 0x190) &&
								(ourChar->GetID() <= 0x193) ) ||
								(ourChar->GetID() == 0x3DB) )
								action( mSock, 0x21 );
							else
								action( mSock, 0x11 );
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
							j++;
						buffer[j] = 0;
						Skills->SkillUse( mSock, makeNum( (char*)&buffer[4] ) );
						break;
					} 
					else if( buffer[3] == 0x27 || buffer[3] == 0x56 )  // Spell
					{
						// This'll find our spellbook for us
						CItem *sBook = FindItemOfType( ourChar, 9 );
						CItem *p = getPack( ourChar );
						bool validLoc;
						validLoc = false;
						if( sBook != NULL )
						{
							if( sBook->GetCont() == ourChar->GetSerial() )
								validLoc = true;
							else if( p != NULL && sBook->GetCont() == p->GetSerial() )
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
										if( ourChar->GetCasting() )
											sysmessage( mSock, 762 );
										else
											sysmessage( mSock, 763 );
									}
									else
									{
										mSock->CurrentSpellType( 0 );
										Magic->SelectSpell( mSock, book );
									}
								}
								else
									sysmessage( mSock, 764 );
							}
							else
								sysmessage( mSock, 765 );
						}
					} 
					else 
					{
						if( buffer[2] == 0x05 && buffer[3] == 0x43 )  // Open spell book
							Magic->SpellBook( mSock );
						break;
					}
					break;
				case 0x9B:// GM Page
					mSock->Receive( 0x102 );
					gmmenu( mSock, 1 );
					break;
				case 0x7D:// Choice
					mSock->Receive( 13 );
					choice( mSock );
					break;
				case 0x95:// Color Select
					mSock->Receive( 9 );
					Commands->DyeTarget( mSock );
					break;
				case 0x34:// Status Request
					mSock->Receive( 10 );
					srequest( mSock );
					break;
				case 0x75:// Rename Character
					mSock->Receive( 0x23 );
					serial = mSock->GetDWord( 1 );
					if( serial == INVALIDSERIAL ) 
						return;
					c = calcCharObjFromSer( serial );
					if( c != NULL )
						c->SetName( (char *)&buffer[5] );
					break;
				case 0x66:// Read Book
					int size;
					mSock->Receive( 3 );
					size = mSock->GetWord( 1 );
					mSock->Receive( size );

					i = calcItemObjFromSer( mSock->GetDWord( 3 ) );
					if( i != NULL )
					{		
						if( i->GetMoreX() != 666 && i->GetMoreX() != 999 ) 
							Books->ReadPreDefBook( mSock, i, mSock->GetWord( 9 ) );  // call old books read-method
						if( i->GetMoreX() == 666 ) // writeable book -> copy page data send by client to the class-page buffer
						{
							char *pagebuffer = mSock->PageBuffer();
							for( j = 13; j <= size; j++ ) // copy (written) page data in class-page buffer
								pagebuffer[j-13] = buffer[j];
							Books->ReadWritableBook( mSock, i, mSock->GetWord( 9 ), mSock->GetWord( 11 ) ); 
						}
						if( i->GetMoreX() == 999 ) 
							Books->ReadNonWritableBook( mSock, i, mSock->GetWord( 9 ) ); // new books readonly
					}
					break;

				// client sends them out if the titel and/or author gets changed on a writable book
				// its NOT send (anymore?) when a book is closed as many packet docus state.
				// LB 7-dec 1999
				case 0x93:
					char author[31], title[61], ch;

					mSock->Receive( 99 );
					serial = mSock->GetDWord( 1 );
					i = calcItemObjFromSer( serial );
					if( i == NULL ) 
						return;
					Books->changeAT = true;

					j = 9; ch = 1;
					char *titlebuffer;
					titlebuffer = mSock->TitleBuffer();
					while( ch != 0 )
					{
						ch = buffer[j];
						title[j-9] = ch;
						titlebuffer[j-9] = ch;
						j++;
						if( j > 69 ) 
							ch=0;
					}
					j = 69; ch = 1;
					char *authorbuffer;
					authorbuffer = mSock->AuthorBuffer();
					while( ch != 0 )
					{
						ch = buffer[j];
						author[j-69] = ch;
						authorbuffer[j-69] = ch;
						j++;
						if( j > 99 )
							ch = 0;
					}
					break;

				case 0xA7:// Get Tip
					mSock->Receive( 4 );
					tips( mSock, mSock->GetWord( 1 ) + 1 );
					break;
				case 0xA4:// Spy
					mSock->Receive( 0x95 );
					break;
				case 0x05:// Attack
					mSock->Receive( 5 );
					ourChar->BreakConcentration( mSock );
					PlayerAttack( mSock );
					break;
				case 0xB1:// Gumpmenu choice
					mSock->Receive( 3 );
					mSock->Receive( mSock->GetWord( 1 ) );
					Gumps->Button( mSock, mSock->GetWord( 13 ), buffer[3], buffer[4], buffer[5], buffer[6], mSock->GetDWord( 7 ) );
					break;
				case 0xAC:// Textentry input
					mSock->Receive( 3 );
					mSock->Receive( mSock->GetWord( 1 ) );
					Gumps->Input( mSock );
					break;
				case 0x2C:// Resurrect menu choice
					mSock->Receive( 2 );
					if( buffer[1] == 0x02 ) 
						sysmessage( mSock, 766 );
					if( buffer[1] == 0x01 ) 
						sysmessage( mSock, 767 );
					break;
				case 0x3B:// Buy from vendor...
					mSock->Receive( 3 );
					mSock->Receive( mSock->GetWord( 1 ) );
					buyItem( mSock );
					break;
				case 0x9F:// Sell to vendor...
					mSock->Receive( 3 );
					mSock->Receive( mSock->GetWord( 1 ) );
					sellItem( mSock );
					break;
				case 0x69:// Client text change
					mSock->Receive( 3 );// What a STUPID message...  It would be useful if
					mSock->Receive( mSock->GetWord( 1 ) );// it included the color changed to, but it doesn't!
					break;
				case 0x6F:// Secure Trading message
					mSock->Receive( 3 );
					mSock->Receive( mSock->GetWord( 1 ) );
					doTradeMsg( mSock );
					break;
				case 0xB6:// T2A Popuphelp request
					mSock->Receive( 9 );
					break;
				case 0xB8:// T2A Profile request
					mSock->Receive( 3 );
					mSock->Receive( mSock->GetWord( 1 ) );		// this is how it's really done

					if( buffer[3] == 1 )		// Set profile, not feasible yet
						sysmessage( mSock, 768 );
					else
					{
						SERIAL targSerial;
						CHARACTER player;
						targSerial = mSock->GetDWord( 4 );
						player = calcCharFromSer( targSerial );
						if( player == INVALIDSERIAL )
							sysmessage( mSock, 769 );
						else
						{
							UI08 tempBuffer[1024];
							char ourMessage[20];
							memset( tempBuffer, 0, sizeof( tempBuffer[0] ) * 1024 );	// zero it out
							tempBuffer[0] = 0xB8;
							tempBuffer[3] = buffer[4];
							tempBuffer[4] = buffer[5];
							tempBuffer[5] = buffer[6];
							tempBuffer[6] = buffer[7];
							strncpy( (char *)&tempBuffer[7], chars[player].GetName(), 30 );
							strncpy( (char *)&tempBuffer[38], chars[player].GetTitle(), 30 );
							strcpy( ourMessage, "Test of Char Profile" );
							for( j = 0; j < strlen( ourMessage ); j++ )
							{
								tempBuffer[69 + j * 2] = 0;
								tempBuffer[70 + j * 2] = ourMessage[j];
							}
							tempBuffer[69 + strlen( ourMessage ) * 2] = 0;
							tempBuffer[70 + strlen( ourMessage ) * 2] = 0;
							tempBuffer[2] = 68 + 2 * strlen( ourMessage ) + 2;
							mSock->Send( tempBuffer, tempBuffer[2] );
						}
					}
						// Send this
	//					UI08 m_Cmd;		// 0=0xB8
	//					NWORD m_len;	// 1 - 2
	//					NDWORD m_UID;	// 3-6=uid
	//					char m_desc1[MAX_NAME_SIZE+1];	// ???Description 1 (not unicode!, name, zero terminated)
	//					char m_desc2[MAX_NAME_SIZE+1];	// ???Description 2 (not unicode!, guild?? fame/karma??? zero terminated)
	//					WCHAR m_desc3[1024];					// ???Description 3 (unicode!, zero terminated)
					break;
					// Dupois
				case 0x71:// Message Board Item
					mSock->Receive( 3 );
					mSock->Receive( mSock->GetWord( 1 ) );
					MsgBoardEvent( mSock );
					break;
					// Dupois - End
					
				case 0xBB: // No idea
					mSock->Receive( 9 );
					break;
				case 0x3A:	// skills management packet
					mSock->Receive( 6 );	// it's always 6 so Westy tells me... let's not be intelligent =)
					int skillNum;
					skillNum = mSock->GetWord( 3 );
					ourChar->SetSkillLock( buffer[5], skillNum );
					break;

				case 0xBF:
					mSock->Receive( 3 );
					mSock->Receive( mSock->GetWord( 1 ) );
					UI16 subCmd;
					subCmd = mSock->GetWord( 3 );	// specific sub command
					switch( subCmd )
					{
					case 1:	break;	// Server message sent to client.  6 keys, each a long, Initialize Fast Walk Prevention
					case 2:	break;	// Server message.  1 long, new key.  Add key to fast walk stack
					case 5:	break;	// Unknown.  2 longs.  First is 00000320, second is 00000005
					case 6:			// 1 byte, sub command.  Yippee, sub sub commands
						UI08 partyCmd;
						partyCmd = mSock->GetByte( 5 );
						switch( partyCmd )
						{
						case 1:		// 2 variants.  1 long, if 0 is targeting cursor, id of party member to add.  Client message
									// second variant.  1 byte, number of members.  4 bytes per member, which is ID.  Server message
							break;
						case 2:		// 2 variants.  Remove party member.  single long, serial of party member to remove.  if 0, targeting appears.  Client message
									// second variant.  1 byte, number of members now in party.  1 long, serial of player removed.  NumMembers long follows, ID of each member
							break;
						case 3:		// Tell party member a message. 1 SERIAL (of target, from client, of source, from server).  Null terminated Unicode message
							// Byte[4] ID.  Byte[n][2] Message
							break;
						case 4:		// Tell full party message.  NULL terminated Unicode message
							// Byte[n][2] message. client part.
							// UI08[4] ID.  UI08[n][2] full message.  Server message
							break;
						case 6:		// Party can loot me?  1 byte, canloot.  0 == no, 1 == yes
							break;
						case 8:		// Accept join party invitation.  1 long, party leader's serial
							break;
						case 9:		// Decline party invitation.  1 LONG, party leader's serial
							break;
						}
						break;
					case 8:	// Set cursor hue.  1 byte.  Server message.  0 == felucca, unhued. 1 = Trammel, hued gold
						break;
					case 11:	// Client language.  3 bytes.  "ENU" for english
						break;
					case 12:	// Closed status gump.  4 bytes.  1 long, player serial
						break;

					case 14:	// UOTD actions
						// 9 bytes long
						action( mSock, mSock->GetWord( 7 ) );
						break;
					}
					break;
				
				case 0x56:
					Console << "'Plot Course' button on a map clicked." << myendl;
					break;
				default:
					FD_ZERO( &all );
					FD_SET( mSock->CliSocket(), &all );
					nfds = mSock->CliSocket() + 1;
					if( select( nfds, &all, NULL, NULL, &uoxtimeout ) > 0 ) 
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
	int i;
	
	uoxtimeout.tv_sec = 0;
	uoxtimeout.tv_usec = 1;

	FD_ZERO( &all );
	FD_ZERO( &errsock );
	
	int nfds = 0;
	for( i = 0; i < loggedInClients.size(); i++ )
	{
		int clientSock = loggedInClients[i]->CliSocket();
		FD_SET( clientSock, &all );
		FD_SET( clientSock, &errsock );
		if( clientSock + 1 > nfds )
			nfds = clientSock + 1;
	}
	int s = select( nfds, &all, NULL, &errsock, &uoxtimeout );
	if( s > 0 )
	{
		int oldnow = loggedInClients.size();
		for( i = 0; i < loggedInClients.size(); i++ )
		{
			if( FD_ISSET( loggedInClients[i]->CliSocket(), &errsock ) )
			{
				LoginDisconnect( i );
				//LoginDisconnect(loggedInClients[i]->CliSocket());
				continue;
			}
			if( ( FD_ISSET( loggedInClients[i]->CliSocket(), &all ) ) && ( oldnow == loggedInClients.size() ) )
			{
				try
				{
					GetLoginMsg( i );
					//GetLoginMsg(loggedInClients[i]->CliSocket());
				}
				catch( socket_error& blah )
				{
#ifdef __LINUX__
						messageLoop << "Client disconnected";
#else
					if( blah.ErrorNumber() == WSAECONNRESET )
						messageLoop << "Client disconnected";
					else if( blah.ErrorNumber() != -1 )
					{
						char temp[128];
						sprintf( temp, "Socket error: %i", blah.ErrorNumber() );
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
#ifndef __LINUX__
		int errorCode = WSAGetLastError();
		if( errorCode != 10022 )
			Console << (SI32)errorCode << myendl;
#endif
	}
}

void cNetworkStuff::LoginDisconnect( UOXSOCKET s ) // Force disconnection of player //Instalog
{
	char temp[128];
	sprintf( temp, "LoginClient %i disconnected.", s );
	messageLoop << temp;
	loggedInClients[s]->FlushBuffer();
	loggedInClients[s]->CloseSocket();

	//Zippy 9/17/01 : fix for clients disconnecting during login not being able to reconnect.
	if( loggedInClients[s]->AcctNo() != -1 ) 
	{
		ACCOUNTSBLOCK actbAccount;
		actbAccount=loggedInClients[s]->GetAccount();
		if(actbAccount.wAccountIndex!=0xffff)
			actbAccount.wFlags&=0xFFFFFFF7;
	}

	delete loggedInClients[s];
	loggedInClients.erase( loggedInClients.begin() + s );
	for( int q = 0; q < loggIteratorBackup.size(); q++ )
	{
		if( loggIteratorBackup[q] >= s )
			loggIteratorBackup[q]--;
	}
}

void cNetworkStuff::LoginDisconnect( cSocket *s ) // Force disconnection of player //Instalog
{
	UOXSOCKET i = FindLoginPtr( s );
	LoginDisconnect( i );
	//LoginDisconnect(s->CliSocket());
}

UOXSOCKET cNetworkStuff::FindLoginPtr( cSocket *s )
{
	for( UOXSOCKET i = 0; i < loggedInClients.size(); i++ )
	{
		if( loggedInClients[i] == s )
			return i;
	}
	return -1;
}


// Transfers from the logged in queue to the in world queue
// Takes something out of the logging in queue and places it in the in world queue
// REQUIRES THREAD SAFETY
UOXSOCKET cNetworkStuff::Transfer( UOXSOCKET s )
{
	if( s == -1 )
		return -1;
	InternalControl.On();
	connClients.push_back( loggedInClients[s] );
	loggedInClients.erase( loggedInClients.begin() + s );
	now = connClients.size();
	if( now > peakConnectionCount )
		peakConnectionCount = now;
	InternalControl.Off();
	return now-1;
}

void cNetworkStuff::GetLoginMsg( UOXSOCKET s )
{
	int count, ho, mi, se, total;
	cSocket *mSock = loggedInClients[s];
	if( mSock == NULL )
		return;
	char temp[128];
	if( mSock->NewClient() )
	{
		count = mSock->Receive( 4 );
		if( mSock->Buffer()[0] == 0x21 && count < 4 )	// UOMon
		{
			total = ( uiCurrentTime - starttime ) / CLOCKS_PER_SEC;
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
//			mSock->ClientIP( mSock->GetDWord( 0 ) );
			mSock->NewClient( false );
			if( mSock->GetDWord( 0 ) == 0x12345678 )
				LoginDisconnect( s );
			mSock->FirstPacket( true );
		}
	}
	else
	{
		UOXSOCKET trnsfr;
		mSock->InLength( 0 );
		UI08 packetID;
		UI08 *buffer = mSock->Buffer();
		if( mSock->Receive( 1 ) > 0 )
		{
			packetID = buffer[0];
			if( mSock->FirstPacket() && packetID != 0x80 && packetID != 0x91 )
			{
				sprintf( temp, "Non-ignition client attempting to cut in, disconnecting them: IP %i.%i.%i.%i", mSock->ClientIP1(), mSock->ClientIP2(), mSock->ClientIP3(), mSock->ClientIP4() );
				messageLoop << temp;
				LoginDisconnect( s );
				return;
			}
			cPInputBuffer *test = WhichPacket( packetID );
			bool doSwitch = true;
			if( test != NULL )
			{
				test->SetSocket( mSock );
				test->Receive( mSock );
				if( test->Handle() )
					doSwitch = false;
				delete test;

			}
			if (packetID == 0x5D)
			{

				// ----------------------------------------------------
				// As a first imprention of the 0xB9 feature packet
				// Should add a system to correctly report back to the
				// clients the correct features as this packet will make
				// the LBR creatures invis instead of replacing them
				// with the old animations.
				// Flag 1 Enable chat
				// Flag 2 Enable LBR
				// Rest unknown
				// ----------------------------------------------------
				// Mr. Fixit (03-17-2002)
			    char activatelbr[5] = "\xB9\xFF\xFF\xFF";	
			    mSock->Send( activatelbr, 3 );
			    messageLoop << "Activating everything in client (Chat/LBR etc).";
				// ----------------------------------------------------

			}
			if( doSwitch )
			{

				switch( packetID )
				{
				case 0x01:// Main Menu on the character select screen
					LoginDisconnect( s );
					break;
				case 0x83:// Character Delete
					mSock->Receive( 0x27 );
					deleteChar( mSock );
					break;
				case 0x00:// Character Create
					mSock->Receive( 104 );
					trnsfr = Transfer( s );
					createChar( mSock );
					break;
				case 0x5D:// Character Select
					mSock->Receive( 0x49 );
					trnsfr = Transfer( s );
					playChar( mSock );
					break;
				case 0x73:// Keep alive
					mSock->Receive( 2 );
					mSock->Send( buffer, 2 );
					break;
				case 0x22:// Resync Request
					mSock->Receive( 3 );
					break;
				case 0xA4:// Spy
					mSock->Receive( 0x95 );
					break;
				case 0xBB: // No idea
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
					if( select( nfds, &all, NULL, NULL, &uoxtimeout ) > 0 ) 
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

UOXSOCKET cNetworkStuff::FindNetworkPtr( cSocket *toFind )
{
	for( UOXSOCKET i = 0; i < connClients.size(); i++ )
	{
		if( connClients[i] == toFind )
			return i;
	}
	return -1;
}

cSocket *cNetworkStuff::FirstSocket( void )
{
	currConnIter = 0;
	if( FinishedSockets() )
		return NULL;
	return connClients[currConnIter];
}
cSocket *cNetworkStuff::NextSocket( void )
{
	if( FinishedSockets() )
		return NULL;
	currConnIter++;
	if( FinishedSockets() )
		return NULL;
	return connClients[currConnIter];
}
bool cNetworkStuff::FinishedSockets( void )
{
	return ( currConnIter >= connClients.size() );
}

cSocket *cNetworkStuff::PrevSocket( void )
{
	if( currConnIter == 0 )
		return NULL;
	currConnIter--;
	return connClients[currConnIter];
}
cSocket *cNetworkStuff::LastSocket( void )
{
	currConnIter = connClients.size();
	if( currConnIter != 0 )
	{
		currConnIter--;
		return connClients[currConnIter];
	}
	else
		return NULL;
}

void cNetworkStuff::LoadFirewallEntries( void )
{
	FILE *firewallEntries = NULL;
	char *p, inputline[80];
	SI16 p1, p2, p3, p4;
	firewallEntries = fopen( "shitlist.ini", "rt" );
	if( firewallEntries == NULL )
		firewallEntries = fopen( "firewall.ini", "rt" );
	if( firewallEntries != NULL ) 
	{
		while( fgets( inputline, 79, firewallEntries ) != NULL )
		{
			if( ( p = strtok( inputline, "." ) ) != NULL )
			{
				if( strcmp( p, "*" ) == 0 )
					p1 = -1;
				else
					p1 = makeNum( p );
				if( ( p = strtok( NULL, "." ) ) != NULL )
				{
					if( strcmp( p, "*" ) == 0 )
						p2 = -1;
					else
						p2 = makeNum( p );
					if( ( p = strtok( NULL, "." ) ) != NULL )
					{
						if( strcmp( p, "*" ) == 0 )
							p3 = -1;
						else
							p3 = makeNum( p );
						if( ( p = strtok( NULL, "\0" ) ) != NULL )
						{
							if( strcmp( p, "*" ) == 0 )
								p4 = -1;
							else
								p4 = makeNum( p );

							slEntries.push_back( FirewallEntry( p1, p2, p3, p4 ) );
							p1 = p2 = p3 = p4 = false;
						}
					}
				}
			}
		}
		fclose( firewallEntries );
	}
}

void cNetworkStuff::StartupXGM( int nPortArg )
{
	xgmSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( xgmSocket < 0 )
	{
		Console.Error( 0, "Unable to create XGM socket" );
#ifdef __NT__
		Console.Error( 0, "Code %i", WSAGetLastError() );
#endif
		return;
	}
#ifndef __NT__
	int on;
	setsockopt( xgmSocket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof( on ) );
#endif
	
	UI32 len_connection_addr = sizeof( struct sockaddr_in );
	sockaddr_in connection;
	memset( (char *) &connection, 0, len_connection_addr );
	connection.sin_family = AF_INET;
	connection.sin_addr.s_addr = htonl( INADDR_ANY );
	connection.sin_port = htons( nPortArg );
	int bcode = bind( xgmSocket, (struct sockaddr *)&connection, len_connection_addr );
	
	if( bcode < 0 )
	{
		Console.Error( 0, " Unable to bind socket for XGM - Error code: %i", bcode );
		return;
	}
	listen( xgmSocket, 42 );
	xgmRunning = true;
}

void cNetworkStuff::CheckXGM( void )
{
	fd_set all;
	fd_set errsock;
	FD_ZERO( &all );
	FD_ZERO( &errsock );
	int nfds = 0;

	for( int i = 0; i < xgmClients.size(); i++ )
	{
		int clientSock = xgmClients[i]->CliSocket();
		FD_SET( clientSock, &all );
		FD_SET( clientSock, &errsock );
		if( clientSock + 1 > nfds )
			nfds = clientSock + 1;
	}
	int s = select( nfds, &all, NULL, &errsock, &uoxtimeout );
	if( s > 0 )
	{
		int oldnow = xgmClients.size();
		for( SI32 i = 0; i < xgmClients.size(); i++ )
		{
			cSocket *mXGMSock = xgmClients[i];
			if( mXGMSock == NULL )
			{
				Console << "Lost XGM socket " << i << " is NULL!" << myendl;
				continue;
			}
			if( FD_ISSET( mXGMSock->CliSocket(), &errsock ) )
			{
				XGMDisconnect( i );
				continue;
			}
			if( ( FD_ISSET( mXGMSock->CliSocket(), &all ) ) && ( oldnow == xgmClients.size() ) )
			{
				try
				{
					GetXGMMsg( i );
				}
				catch( socket_error& blah )
				{
#ifdef __LINUX__
					Console << "Client disconnected" << myendl;
#else
					if( blah.ErrorNumber() == WSAECONNRESET )
						Console << "Client disconnected" << myendl;
					else if( blah.ErrorNumber() != -1 )
						Console << "Socket error: " << (SI32)blah.ErrorNumber() << myendl;
#endif
					XGMDisconnect( i );	// abnormal error
				}
			}
		}
	}
	else if( s == SOCKET_ERROR )
	{
//		Console << WSAGetLastError() << myendl;
//		Console << WSANOTINITIALISED << myendl;
	}
}
void cNetworkStuff::ShutdownXGM( void )
{
	closesocket( xgmSocket );
	for( int i = 0; i < xgmClients.size(); i++ )
	{
		xgmClients[i]->CloseSocket();
		delete xgmClients[i];
		xgmClients[i] = NULL;
	}

}

void cNetworkStuff::XGMDisconnect( UOXSOCKET s )
{
	Console << "XGMClient " << s << " disconnected. [Total:" << (SI32)(xgmClients.size()-1) << "]" << myendl;

	xgmClients[s]->FlushBuffer();
	xgmClients[s]->CloseSocket();

	delete xgmClients[s];
	xgmClients.erase( xgmClients.begin() + s );
	for( int q = 0; q < xgmIteratorBackup.size(); q++ )
	{
		if( xgmIteratorBackup[q] >= s )
			xgmIteratorBackup[q]--;
	}
}

void cNetworkStuff::XGMDisconnect( cSocket *s )
{
	UOXSOCKET i = FindXGMPtr( s );
	XGMDisconnect( i );
}

UOXSOCKET cNetworkStuff::FindXGMPtr( cSocket *s )
{
	for( UOXSOCKET i = 0; i < xgmClients.size(); i++ )
	{
		if( xgmClients[i] == s )
			return i;
	}
	return -1;
}

cPInputBuffer *WhichXGMPacket( UI08 packetID );

void cNetworkStuff::GetXGMMsg( UOXSOCKET s )
{
	cSocket *mSock = xgmClients[s];
	if( mSock == NULL )
		return;
	mSock->InLength( 0 );
	UI08 *buffer = mSock->Buffer();
	char temp[1024];
	if( mSock->Receive( 1 ) > 0 )
	{
		UI08 packetID = buffer[0];
		if( mSock->FirstPacket() && packetID != 0x00 )
		{
			sprintf( temp, "Invalid XGM client attempting to connect, disconnecting them: IP %i.%i.%i.%i", mSock->ClientIP1(), mSock->ClientIP2(), mSock->ClientIP3(), mSock->ClientIP4() );
			messageLoop << temp;
			XGMDisconnect( s );
			return;
		}
		cPInputBuffer *test = WhichXGMPacket( packetID );
		if( test != NULL )
		{
			test->SetSocket( mSock );
			test->Receive( mSock );
			test->Handle();
			delete test;
		}
		else
		{
			fd_set all;
			FD_ZERO( &all );
			FD_SET( mSock->CliSocket(), &all );
			int nfds = mSock->CliSocket() + 1;
			if( select( nfds, &all, NULL, NULL, &uoxtimeout ) > 0 ) 
				mSock->Receive( 2560 );
			sprintf( temp, "Unknown message from client: 0x%02X - Ignored", packetID );
			Console << temp << myendl;
		}
	}
	else	// Sortta error type thing, they disconnected already
		XGMDisconnect( s );
}

void cNetworkStuff::CheckConnections( void )
{
	CheckConn();
	CheckXGMConn();
}

void cNetworkStuff::CheckXGMConn( void )
{
	fd_set xgmConn;
	FD_ZERO( &xgmConn );
	FD_SET( xgmSocket, &xgmConn );
	int nfds = xgmSocket + 1;
	int s = select( nfds, &xgmConn, NULL, NULL, &uoxtimeout );
	if( s > 0 )
	{
		sockaddr_in client_addr;
		int len = sizeof( struct sockaddr_in );
#ifndef __LINUX__
		int newClient = accept( xgmSocket, (struct sockaddr *)&client_addr, &len );
#else
		int newClient = accept( xgmSocket, (struct sockaddr *)&client_addr, (socklen_t *)&len );
#endif
		cSocket *toMake = new cSocket( newClient );
		if( newClient < 0 )
		{
			Console.Error( 0, "Error at client xGM connection!" );
			error = true;
			keeprun = true;
			delete toMake;
			return;
		}
		toMake->ClientType( CV_XGM );
		//	EviLDeD	-	April 5, 2000
		//	Due to an attack on the shard, and the true inability to determine who did what 
		//	I am implementing a firewall entry system, any IP on this list will be immediatly dropped.
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
			Console << "FIREWALL: Blocking address " << part[0] << "." << part[1] << "." << part[2] << "." << part[3] << "--> Blocked!" << myendl;
			closesocket( newClient );
			delete toMake;
			return;
		}
		Console << "FIREWALL: Forwarding address " << (SI32)(part[0]) << "." << (SI32)(part[1]) << "." << (SI32)(part[2]) << "." << (SI32)(part[3]) << " --> Access Granted" << myendl;
		Console << "XGMClient " << (SI32)now << " [" << (SI32)(part[0]) << "." << (SI32)(part[1]) << "." << (SI32)(part[2]) << "." << (SI32)(part[3]) << "] connected [Total:" << (SI32)(now+1) << "]" << myendl;
		xgmClients.push_back( toMake );
		return;
	}
	if( s < 0 )
	{
		Console.Error( 0, " Select (Conn) failed!" );
		keeprun = false;
		error = true;
		return;
	}
}

void cNetworkStuff::CheckMessages( void )
{
	CheckLoginMessage();
	CheckMessage();
	CheckXGM();
}

SI32 cNetworkStuff::PeakConnectionCount( void ) const
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
	currLoggIter = connIteratorBackup.back();
	connIteratorBackup.pop_back();

	Off();
}

void cNetworkStuff::PushXGM( void )
{
	On();

	xgmIteratorBackup.push_back( currXGMIter );
}
void cNetworkStuff::PopXGM( void )
{
	currXGMIter = xgmIteratorBackup.back();
	xgmIteratorBackup.pop_back();

	Off();
}
