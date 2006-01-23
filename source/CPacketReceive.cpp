#include "uox3.h"
#include "CPacketSend.h"
#include "movement.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "cThreadQueue.h"
#include "combat.h"
#include "cScript.h"
#include "CJSMapping.h"
#include "PageVector.h"
#include "cEffects.h"
#include "Dictionary.h"

namespace UOX
{

void pSplit( const std::string pass0, std::string &pass1, std::string &pass2 ) // Split login password into UOX password and UO password
{
	int i = 0;
	pass1 = "";
	int pass0Len = pass0.length();
	while( pass0[i] != '/' && i < pass0Len ) 
		++i;
	pass1 = pass0.substr( 0, i );
	if( i < pass0Len ) 
		pass2 = pass0.substr( i );
}

void PackShort( UI08 *toPack, int offset, UI16 value )
{
	toPack[offset+0] = (UI08)((value&0xFF00)>>8);
	toPack[offset+1] = (UI08)((value&0x00FF)%256);
}

CPInputBuffer *WhichLoginPacket( UI08 packetID, CSocket *s )
{
	switch( packetID )
	{
		case 0x00:	return ( new CPICreateCharacter( s )	);	// Character Create
		case 0x01:	return NULL;								// Main Menu on the character select screen
		case 0x04:	return ( new CPIGodModeToggle( s )		);
		case 0x5D:	return ( new CPIPlayCharacter( s )		);	// Character Select
		case 0x73:	return ( new CPIKeepAlive( s )			);
		case 0x80:	return ( new CPIFirstLogin( s )			);
		case 0x83:	return ( new CPIDeleteCharacter( s )	);	// Character Delete
		case 0x91:	return ( new CPISecondLogin( s )		);
		case 0xA0:	return ( new CPIServerSelect( s )		);
		case 0xA4:	return ( new CPISpy( s )				);
		case 0xBB:	return NULL;								// No idea
		case 0xBD:	return ( new CPIClientVersion( s )		);
		case 0xBF:	return NULL;	
		case 0xD9:	return ( new CPIMetrics( s )			);	// Client Hardware / Metrics
		default:	break;
	}
	throw socket_error( "Bad packet request" );
	return NULL;
}

CPInputBuffer *WhichPacket( UI08 packetID, CSocket *s )
{
	switch( packetID )
	{
		case 0x00:	return ( new CPICreateCharacter( s )	);
		case 0x02:	return ( new CPIMoveRequest( s )		);
		case 0x03:	return ( new CPITalkRequestAscii( s )	);
		case 0x04:	return ( new CPIGodModeToggle( s )		);
		case 0x05:	return ( new CPIAttack( s )				);
		case 0x06:	return ( new CPIDblClick( s )			);
		case 0x07:	return ( new CPIGetItem( s )			);
		case 0x08:	return ( new CPIDropItem( s )			);
		case 0x09:	return ( new CPISingleClick( s )		);
		case 0x12:	return NULL;								// Request Skill / Action / Magic Usage
		case 0x13:	return ( new CPIEquipItem( s )			);
		case 0x22:	return ( new CPIResyncReq( s )			);
		case 0x2C:	return ( new CPIResMenuChoice( s )		);
		case 0x34:	return ( new CPIStatusRequest( s )		);
		case 0x3A:	return NULL;								// Skills management packet
		case 0x3B:	return ( new CPIBuyItem( s )			);
		case 0x56:	return NULL;								// Map Related
		case 0x5D:	return ( new CPIPlayCharacter( s )		);
		case 0x66:	return ( new CPIBookPage( s )			);	// Player Turns the Page (or closes) a Book
		case 0x69:	return NULL;								// Client text change
		case 0x6C:	return ( new CPITargetCursor( s )		);
		case 0x6F:	return ( new CPITradeMessage( s )		);
		case 0x71:	return ( new CPIMsgBoardEvent( s )		);	// Message Board Item
		case 0x72:	return NULL;								// Combat mode
		case 0x73:	return ( new CPIKeepAlive( s )			);
		case 0x75:	return ( new CPIRename( s )				);
		case 0x7D:	return ( new CPIGumpChoice( s )			);
		case 0x80:	return ( new CPIFirstLogin( s )			);
		case 0x83:	return ( new CPIDeleteCharacter( s )	);
		case 0x91:	return ( new CPISecondLogin( s )		);
		case 0x93:	return NULL;								// Books - title page
		case 0x95:	return ( new CPIDyeWindow( s )			);
		case 0x98:	return ( new CPIAllNames3D( s )			);
		case 0x9B:	return ( new CPIHelpRequest( s )		);
		case 0x9F:	return ( new CPISellItem( s )			);
		case 0xA0:	return ( new CPIServerSelect( s )		);
		case 0xA4:	return ( new CPISpy( s )				);
		case 0xA7:	return ( new CPITips( s )				);
		case 0xAC:	return ( new CPIGumpInput( s )			);
		case 0xAD:	return ( new CPITalkRequestUnicode( s ) );
		case 0xB1:	return ( new CPIGumpMenuSelect( s )		);
		case 0xB6:	return NULL;								// T2A Popup Help Request
		case 0xB8:	return NULL;								// T2A Profile Request
		case 0xBB:	return NULL;								// Ultima Messenger
		case 0xBD:	return ( new CPIClientVersion( s )		);
		case 0xBF:	return NULL;								// Assorted commands
		case 0xC8:	return ( new CPIUpdateRangeChange( s )	);
		case 0xD0:	return NULL;								// Configuration File
		case 0xD1:	return NULL;								// Logout Status
		case 0xD4:	return ( new CPINewBookHeader( s )		);	// New Book Header
		case 0xD9:	return ( new CPIMetrics( s )			);	// Client Hardware / Metrics
		default:	return NULL;
	}
	return NULL;
}

//	0x80 Packet
//	Last Modified on Monday, 13-Apr-1998 17:06:30 EDT 
//	Login Request (62 bytes) 
//	BYTE cmd 
//	BYTE[30] userid 
//	BYTE[30] password 
//	BYTE unknown1 (not usually 0x00 - so not NULL) 

void CPIFirstLogin::Log( std::ofstream &outStream, bool fullHeader )
{
	if( fullHeader )
		outStream << "[RECV]Packet   : CPIFirstLogin 0x80 --> Length: 62" << TimeStamp() << std::endl;
	outStream << "UserID         : " << Name() << std::endl;
	outStream << "Password       : " << Pass() << std::endl;
	outStream << "Unknown        : " << (SI16)Unknown() << std::endl;
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}

bool CPIFirstLogin::Handle( void )
{
	tSock->AcctNo( AB_INVALID_ID );
	LoginDenyReason t = LDR_NODENY;

	std::string username = Name();

	ACCOUNTSBLOCK * actbTemp = &Accounts->GetAccountByName( username );
	if( actbTemp->wAccountIndex != AB_INVALID_ID )
		tSock->SetAccount( (*actbTemp) );

	std::string pass0, pass1, pass2;
	pass0 = Pass();
	pSplit( pass0, pass1, pass2 );

	// Need auto account creation code here
	if( tSock->AcctNo() == AB_INVALID_ID )
	{
		if( cwmWorldState->ServerData()->InternalAccountStatus() )
		{
			Accounts->AddAccount( username, pass1, "" );
			actbTemp = &Accounts->GetAccountByName( username );
			if( actbTemp->wAccountIndex != AB_INVALID_ID )	// grab our account number
				tSock->SetAccount( (*actbTemp) );
		}
	}
	if( tSock->AcctNo() != AB_INVALID_ID )
	{
		if( actbTemp->wFlags&AB_FLAGS_BANNED )
			t = LDR_ACCOUNTDISABLED;
		else if( actbTemp->sPassword != pass1 )
			t = LDR_BADPASSWORD;
	}
	else
		t = LDR_UNKNOWNUSER;
	if( t == LDR_NODENY && ( actbTemp->wFlags&AB_FLAGS_ONLINE ) )
		t = LDR_ACCOUNTINUSE;
	if( t != LDR_NODENY )
	{
		CPLoginDeny pckDeny( t );
		tSock->Send( &pckDeny );
		tSock->AcctNo( AB_INVALID_ID );
		Network->LoginDisconnect( tSock );
		return true;
	}
	if( tSock->AcctNo() != AB_INVALID_ID )
	{
		char temp[1024];
		actbTemp->dwLastIP = calcserial( tSock->ClientIP4(), tSock->ClientIP3(), tSock->ClientIP2(), tSock->ClientIP1() );
		sprintf( temp, "Client [%i.%i.%i.%i] connected using Account '%s'.", tSock->ClientIP4(), tSock->ClientIP3(), tSock->ClientIP2(), tSock->ClientIP1(), username.c_str() );
		Console.Log( temp, "server.log" );
		messageLoop << temp;

		actbTemp->wFlags |= AB_FLAGS_ONLINE;
//		Accounts->ModAccount( actbTemp->sUsername, AB_FLAGS, (*actbTemp) );

		UI16 servcount = cwmWorldState->ServerData()->ServerCount();
		CPGameServerList toSend( servcount );
		for( UI16 i = 0; i < servcount; ++i )
		{
			physicalServer *sData = cwmWorldState->ServerData()->ServerEntry( i );
			toSend.AddServer( i, sData );
		}
		tSock->Send( &toSend );
	}
	CPEnableClientFeatures ii;
	tSock->Send( &ii );
	return true;
}

CPIFirstLogin::CPIFirstLogin()
{
	InternalReset();
}
CPIFirstLogin::CPIFirstLogin( CSocket *s ) : CPInputBuffer( s )
{
	InternalReset();
	Receive();
}
void CPIFirstLogin::Receive( void )
{
	tSock->FirstPacket( false );
	tSock->Receive( 62, false );

	// Copy data over into internal variables
	char temp[30];
	// Grab our username
	memcpy( temp, &tSock->Buffer()[1], 30 );
	userID = temp;

	// Grab our password
	memcpy( temp, &tSock->Buffer()[31], 30 );
	password = temp;

	unknown = tSock->GetByte( 61 );
	// Done with our buffer, we can clear it out now
}
void CPIFirstLogin::InternalReset( void )
{
	userID.reserve( 30 );
	password.reserve( 30 );
}

const std::string CPIFirstLogin::Name( void )
{
	return userID;
}
const std::string CPIFirstLogin::Pass( void )
{
	return password;
}
UI08 CPIFirstLogin::Unknown( void )
{
	return unknown;
}

//	0xA0 Packet
//	Last Modified on Monday, 13-Apr-1998 17:06:46 EDT 
//	Select Server (3 bytes) 
//	BYTE cmd 
//	BYTE[2] server # chosen 
//	0x80 Packet
void CPIServerSelect::Log( std::ofstream &outStream, bool fullHeader )
{
	if( fullHeader )
		outStream << "[RECV]Packet   : CPIServerSelect 0xA0 --> Length: 3" << TimeStamp() << std::endl;
	outStream << "Server         : " << ServerNum() << std::endl;
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}

void CPIServerSelect::InternalReset( void )
{
}
CPIServerSelect::CPIServerSelect()
{
	InternalReset();
}
CPIServerSelect::CPIServerSelect( CSocket *s ) : CPInputBuffer( s )
{
	InternalReset();
	Receive();
}
void CPIServerSelect::Receive( void )
{
	tSock->Receive( 3, false );
}
SI16 CPIServerSelect::ServerNum( void )
{
	// EviLDeD - Sept 19, 2002
	// Someone said that there was an issue with False logins that request server 0. Default to server 1.
	SI16 temp = tSock->GetWord( 1 );
	if( temp == 0 )
		return 1;
	else
		return temp;
}

bool CPIServerSelect::Handle( void )
{
	UI16 selectedServer		= ServerNum() - 1;
	physicalServer *tServer = cwmWorldState->ServerData()->ServerEntry( selectedServer );
	UI32 ip = htonl( inet_addr( tServer->getIP().c_str() ) );
	CPRelay toSend( ip, tServer->getPort() );
	tSock->Send( &toSend );
	// Mark packet as sent. No we need to change how we network.
	return true;
}

//	0x91 Packet
//	Last Modified on Monday, 13-Apr-1998 17:06:45 EDT 
//	Game Server Login (65 bytes) 
//	BYTE cmd 
//	BYTE[4] key used 
//	BYTE[30] sid 
//	BYTE[30] password 
void CPISecondLogin::Log( std::ofstream &outStream, bool fullHeader )
{
	if( fullHeader )
		outStream << "[RECV]Packet   : CPISecondLogin 0x91 --> Length: 65" << TimeStamp() << std::endl;
	
	outStream << "Key Used       : " << Account() << std::endl;
	outStream << "SID            : " << Name() << std::endl;
	outStream << "Password       : " << Pass() << std::endl;
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}
void CPISecondLogin::InternalReset( void )
{
	sid.reserve( 30 );
	password.reserve( 30 );
}
CPISecondLogin::CPISecondLogin()
{
	InternalReset();
}
CPISecondLogin::CPISecondLogin( CSocket *s ) : CPInputBuffer( s )
{
	InternalReset();
	Receive();
}
void CPISecondLogin::Receive( void )
{
	tSock->FirstPacket( false );
	tSock->Receive( 65, false );
	tSock->CryptClient( true );
	
	// Copy data over into internal variables
	char temp[30];
	keyUsed = tSock->GetDWord( 1 );

	// Grab our username
	memcpy( temp, &tSock->Buffer()[5], 30 );
	sid = temp;

	// Grab our password
	memcpy( temp, &tSock->Buffer()[35], 30 );
	password = temp;

	// Done with our buffer, we can clear it out now
}
UI32 CPISecondLogin::Account( void )
{
	return keyUsed;
}
const std::string CPISecondLogin::Name( void )
{
	return sid;
}
const std::string CPISecondLogin::Pass( void )
{
	return password;
}

bool CPISecondLogin::Handle( void )
{
	LoginDenyReason t = LDR_NODENY;
	tSock->AcctNo( AB_INVALID_ID );
	ACCOUNTSBLOCK& actbTemp = Accounts->GetAccountByName( Name() );
	if( actbTemp.wAccountIndex != AB_INVALID_ID )
		tSock->SetAccount( actbTemp );

	std::string pass0, pass1, pass2;
	pass0 = Pass();
	pSplit( pass0, pass1, pass2 );

	if( tSock->AcctNo() != AB_INVALID_ID )
	{
		if( actbTemp.wFlags&AB_FLAGS_BANNED )
			t = LDR_ACCOUNTDISABLED;
		else if( pass1 != actbTemp.sPassword )
			t = LDR_BADPASSWORD;
	}
	else
		t = LDR_UNKNOWNUSER;
	if( t != LDR_NODENY )
	{
		tSock->AcctNo( AB_INVALID_ID );
		CPLoginDeny pckDeny( t );
		tSock->Send( &pckDeny );
		Network->LoginDisconnect( tSock );
		return true;
	}
	else
	{
		UI08 charCount = 0;
		for( UI08 i = 0; i < 6; ++i )
		{
			if( actbTemp.dwCharacters[i] != INVALIDSERIAL )
				++charCount;
		}
		CServerData *sd		= cwmWorldState->ServerData();
		size_t serverCount	= sd->NumServerLocations();
		CPCharAndStartLoc toSend( actbTemp, charCount, static_cast<UI08>(serverCount) );
		for( size_t j = 0; j < serverCount; ++j )
		{
			toSend.AddStartLocation( sd->ServerLocation( j ), static_cast<UI08>(j) );
		}
		CPEnableClientFeatures ii;
		tSock->Send( &ii );
		tSock->Send( &toSend );
	}
	return true;
}

//	0xBD Packet
//	Last Modified on Wednesday, 28-Aug-2002
//	Client Version Message (Variable # of bytes) 
//	BYTE cmd 
//	BYTE[2] len
//	If (client-version of packet) 
//		BYTE[len-3] string stating the client version (0 terminated)  (like: “1.26.4”)
//	Note: Client + Server Message
//	Client version : client sends its version string (e.g “3.0.8j”)
//	Server version : 0xbd 0x0 0x3 (client replies with client version of this packet)
//	Note2:  clients sends a client version of this packet ONCE at login (without server request.)
void CPIClientVersion::Log( std::ofstream &outStream, bool fullHeader )
{
	if( fullHeader )
		outStream << "[RECV]Packet   : CPIClientVersion 0xBD --> Length: " << std::dec << tSock->GetWord( 1 ) << TimeStamp() << std::endl;
	outStream << "Version        : " << Offset() << std::endl;
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}
void CPIClientVersion::InternalReset( void )
{
	len = 0;
}
CPIClientVersion::CPIClientVersion()
{
	InternalReset();
}
CPIClientVersion::CPIClientVersion( CSocket *s ) : CPInputBuffer( s )
{
	InternalReset();
	Receive();
}
void CPIClientVersion::Receive( void )
{
	tSock->Receive( 3, false );
	len = tSock->GetWord( 1 );
	tSock->Receive( len, false );
}

//o---------------------------------------------------------------------------o
//|   Function    -	UI08 ShiftValue( UI08 toShift, UI08 base, UI08 upper, bool extra)
//|   Date        -	21st November, 2001
//|   Programmer  -	Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -	If the value is between base and upper (inclusive)
//|					it shifts the data down by base (if extra) or base - 1 (if not extra)
//o---------------------------------------------------------------------------o
UI08 ShiftValue( UI08 toShift, UI08 base, UI08 upper, bool extra )
{
	if( extra )
	{
		if( toShift >= base && toShift <= upper )
			toShift -= base;
	}
	else
	{
		if( toShift >= base && toShift <= upper )
			toShift = toShift - base + 1;
	}
	return toShift;
}
char *CPIClientVersion::Offset( void )
{
	return (char *)&(tSock->Buffer()[3]);
}
bool CPIClientVersion::Handle( void )
{
	char *verString	= Offset();
	verString[len]	= 0;
	UI08 major, minor, sub, letter;

	std::string s( verString );
	std::istringstream ss( s );
	char period;
	ss >> major >> period;
	ss >> minor >> period;
	ss >> sub >> letter;

	major	= ShiftValue( major,  '0', '9', true );
	minor	= ShiftValue( minor,  '0', '9', true );
	sub		= ShiftValue( sub,    '0', '9', true );
	letter	= ShiftValue( letter, 'a', 'z', false );
	letter	= ShiftValue( letter, 'A', 'Z', false );

	tSock->ClientVersion( major, minor, sub, letter );

	Console << verString << myendl;

	if( strstr( verString, "Dawn" ) )
		tSock->ClientType( CV_UO3D );
	else if( strstr( verString, "Krrios" ) )
		tSock->ClientType( CV_KRRIOS );
	tSock->ReceivedVersion( true );
	return true;
}

//	0xC8 Packet
//	Last Modified on Monday, 4’th-Sep-2002  
//	Client view range (2 Bytes)
//	BYTE cmd 
//	BYTE range 
//	Note: client+server packet.
//	Range: how far away client wants to see (=get send) items/npcs
//	Update: since client 3.0.8o it actually got activated.(in a useful way)
//	When increase/decrease macro send, client sends a 0xc8.
//	If and only if server *relays* the packet (sending back the same data) range stuff gets activated client side.
//	"Greying" has no packet, purely client internal.
//	Minimal value:5, maximal: 18
void CPIUpdateRangeChange::Log( std::ofstream &outStream, bool fullHeader )
{
	if( fullHeader )
		outStream << "[RECV]Packet   : CPIUpdateRangeChange 0xC8 --> Length: 2 " << std::endl;
	outStream << "Range			 : " << (int)tSock->GetByte( 1 ) << std::endl;
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}
void CPIUpdateRangeChange::InternalReset( void )
{
}
CPIUpdateRangeChange::CPIUpdateRangeChange()
{
	InternalReset();
}
CPIUpdateRangeChange::CPIUpdateRangeChange( CSocket *s ) : CPInputBuffer( s )
{
	InternalReset();
	Receive();
}
void CPIUpdateRangeChange::Receive( void )
{
	tSock->Receive( 2, false );
}
bool CPIUpdateRangeChange::Handle( void )
{
	// Byte 0 == 0xC8
	// Byte 1 == 0x0F (by default)  This is the distance, we believe

	// SURPRISE! This value, if used by the server, WILL NOT SHOW THE ITEMS IN A REAL CLIENT!
	// Krrios' client behaves (go Krrios!), but the UO ones don't
	switch( tSock->ClientType() )
	{
		case CV_NORMAL:
		case CV_T2A:
		case CV_UO3D:
			tSock->Range( tSock->GetByte( 1 ) - 4 );
			break;
		default:
			tSock->Range( tSock->GetByte( 1 ) );
			break;
	}
	tSock->Send( tSock->Buffer(), 2 );	// we want to echo it back to the client
#if defined( _MSC_VER )
#pragma note( "Flush location" )
#endif
	tSock->FlushBuffer();
	return true;
}

//0xA7 Packet
//Last Modified on Wednesday, 06-May-1998 23:30:53 EDT 
//Request Tips/Notice (4 bytes) 
//	BYTE cmd 
//	BYTE[2] last tip # 
//	BYTE flag 
//		0x00 - tips window 
//		0x01 - notice window 
void CPITips::InternalReset( void )
{
}
CPITips::CPITips()
{
	InternalReset();
}
CPITips::CPITips( CSocket *s ) : CPInputBuffer( s )
{
	InternalReset();
	Receive();
}
void CPITips::Receive( void )
{
	tSock->Receive( 4, false );
}
bool CPITips::Handle( void )
{
	ScriptSection *Tips = FileLookup->FindEntry( "TIPS", misc_def );
	if( Tips == NULL )
		return true;

	UI16 i = static_cast<UI16>(tSock->GetWord( 1 ) + 1);
	if( i == 0 ) 
		i = 1;

	int x = i;
	UString tag;
	UString data;
	UString sect;
	for( tag = Tips->First(); !Tips->AtEnd(); tag = Tips->Next() )
	{
		if( !tag.empty() && tag.upper() == "TIP" )
			--x;
		if( x <= 0 )
			break;
	}
	if( x > 0 )
		tag = Tips->Prev();
	data = Tips->GrabData();
	sect = "TIP " + data.stripWhiteSpace();
	Tips = FileLookup->FindEntry( sect, misc_def );
	if( Tips == NULL )
		return true;
	x = -1;
	char tipData[2048];
	tipData[0] = 0;
	CPUpdScroll toSend( 0, (UI08)i );
	for( tag = Tips->First(); !Tips->AtEnd(); tag = Tips->Next() )
	{
		data = Tips->GrabData();
		sprintf( tipData, "%s%s %s ", tipData, tag.c_str(), data.c_str() );
	}

	toSend.AddString( tipData );
	toSend.Finalize();
	tSock->Send( &toSend );

	return true;
}

//
//0x75 Packet
//Last Modified on Sunday, 13-Feb-2000 
//Rename Character (35 bytes) 
//	BYTE cmd 
//	BYTE[4] id 
//	BYTE[30] new name 

void CPIRename::InternalReset( void )
{
}
CPIRename::CPIRename()
{
	InternalReset();
}
CPIRename::CPIRename( CSocket *s ) : CPInputBuffer( s )
{
	InternalReset();
	Receive();
}
void CPIRename::Receive( void )
{
	tSock->Receive( 0x23, false );
}
bool CPIRename::Handle( void )
{
	SERIAL serial = tSock->GetDWord( 1 );
	if( serial == INVALIDSERIAL ) 
		return true;
	CChar *c = calcCharObjFromSer( serial );
	if( ValidateObject( c ) )
		c->SetName( (char *)&tSock->Buffer()[5] );
	return true;
}

//0x73 Packet
//Last Modified on Friday, 20-Nov-1998 
//Ping message (2 bytes) 
//	BYTE cmd 
//	BYTE seq 
CPIKeepAlive::CPIKeepAlive()
{
}
CPIKeepAlive::CPIKeepAlive( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}
void CPIKeepAlive::Receive( void )
{
	tSock->Receive( 2, false );
}
bool CPIKeepAlive::Handle( void )
{
	tSock->Send( tSock->Buffer(), 2 );
#if defined( _MSC_VER )
#pragma note( "Flush location" )
#endif
	tSock->FlushBuffer();
	return true;
}

//0x34 Packet
//Last modified on Thursday, 19-Nov-1998 
//Get Player Status (10 bytes) 
//	BYTE cmd 
//	BYTE[4] pattern (0xedededed) 
//	BYTE getType 
//		0x04 - Basic Stats (Packet 0x11 Response) 
//		0x05 = Request Skills (Packet 0x3A Response) 
//	BYTE[4] playerID 
CPIStatusRequest::CPIStatusRequest()
{
}
CPIStatusRequest::CPIStatusRequest( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}
void CPIStatusRequest::Receive( void )
{
	tSock->Receive( 10, false );

	pattern		= tSock->GetDWord( 1 );
	getType		= tSock->GetByte(  5 );
	playerID	= tSock->GetDWord( 6 );
}

void CPIStatusRequest::Log( std::ofstream &outStream, bool fullHeader )
{
	if( fullHeader )
		outStream << "[RECV]Packet   : CPIStatusRequest 0x34 --> Length: 10" << TimeStamp() << std::endl;
	outStream << "Pattern        : " << pattern << std::endl;
	outStream << "Request Type   : " << (int)getType << std::endl;
	outStream << "PlayerID       : " << playerID << std::endl;
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}

bool CPIStatusRequest::Handle( void )
{
	if( getType == 4 )
		tSock->statwindow( calcCharObjFromSer( playerID ) );
	if( getType == 5 )
	{
	// Check if onSkillGump event exists
	CChar *myChar	= tSock->CurrcharObj();
	UI16 charTrig		= myChar->GetScriptTrigger();
	cScript *toExecute	= JSMapping->GetScript( charTrig );
	if( toExecute != NULL )
	{
		if( toExecute->OnSkillGump( myChar ) == 1 )	// if it exists and we don't want hard code, return
			return true;
	}

		CPSkillsValues toSend;
//		if( tSock->ReceivedVersion() )
//		{
//			if( tSock->ClientVersionMajor() >= 4 && tSock->ClientVersionSub() >= 0 && tSock->ClientVersionLetter() >= 6 )
				toSend.NumSkills( ALLSKILLS );
//			else
//				toSend.NumSkills( ALLSKILLS_COMPAT );
			toSend.SetCharacter( *(tSock->CurrcharObj()) );
			tSock->Send( &toSend );
//		}
	}
	return true;
}

//0xA4 Packet
//Last Modified on Friday, 15-May-2000 
//Spy on Client (149 bytes) 
//	BYTE cmd 
//	BYTE[148] Unknown (previously, this has had info such as your graphics card name, free HD space, number of processors, etc.)
CPISpy::CPISpy()
{
}
CPISpy::CPISpy( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}
void CPISpy::Receive( void )
{
	tSock->Receive( 0x95, false );
}
bool CPISpy::Handle( void )
{
	return true;
}

//0x04 Packet
//Last Modified on Friday, 19-May-2000 
//Request (2 bytes) 
//	BYTE cmd 
//	BYTE mode (0=off, 1=on)  
//Note: Client Message
CPIGodModeToggle::CPIGodModeToggle()
{
}
CPIGodModeToggle::CPIGodModeToggle( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}
void CPIGodModeToggle::Receive( void )
{
	tSock->Receive( 2, false );
}
bool CPIGodModeToggle::Handle( void )
{
	CChar *ourChar = tSock->CurrcharObj();
	if( ourChar->IsGM() )
	{
		CPGodModeToggle cpgmt = tSock;
		tSock->Send( &cpgmt );
	}
	else
	{
		tSock->sysmessage( 1641 );
		Console << "Godclient detected - Account[" << ourChar->GetAccount().wAccountIndex << "] Username[" << ourChar->GetAccount().sUsername << ". Client disconnected!" << myendl;
		Network->Disconnect( tSock );
	}
	return true;
}

//0x06 Packet
//Last Modified on Friday, 19-May-2000 
//Double click (5 bytes) 
//	BYTE cmd 
//	BYTE[4] ID of double clicked object 
//Note: Client Message
CPIDblClick::CPIDblClick()
{
}
CPIDblClick::CPIDblClick( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}
void CPIDblClick::Receive( void )
{
	tSock->Receive( 5, false );

	objectID	= tSock->GetDWord( 1 );
}

void CPIDblClick::Log( std::ofstream &outStream, bool fullHeader )
{
	if( fullHeader )
		outStream << "[RECV]Packet   : CPIDblClick 0x06 --> Length: 5" << TimeStamp() << std::endl;
	outStream << "ClickedID      : " << objectID << std::endl;
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}

// bool CPIDblClick::Handle() is implemented in cplayeraction.cpp


//0x09 Packet
//Last Modified on Friday, 19-May-2000 
//Single click (5 bytes) 
//	BYTE cmd 
//	BYTE[4] ID of single clicked object 
//Note: Client Message
CPISingleClick::CPISingleClick()
{
}
CPISingleClick::CPISingleClick( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}
void CPISingleClick::Receive( void )
{
	tSock->Receive( 5, false );

	objectID	= tSock->GetDWord( 1 );
}

void CPISingleClick::Log( std::ofstream &outStream, bool fullHeader )
{
	if( fullHeader )
		outStream << "[RECV]Packet   : CPISingleClick 0x09 --> Length: 5" << TimeStamp() << std::endl;
	outStream << "ClickedID      : " << objectID << std::endl;
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}

// bool CPISingleClick::Handle() is implemented in cplayeraction.cpp

//0x02 Packet
//Last Modified on Friday, 19-May-2000
//Move Request (7 bytes) 
//	BYTE cmd 
//	BYTE direction 
//	BYTE sequence number 
//	BYTE[4] fastwalk prevention key
//Note: Client Message
//Note: sequence number starts at 0, after a reset.  However, if 255 is reached, the next seq # is 1, not 0.
//Fastwalk prevention notes: each 0x02 pops the top element from fastwalk key stack. (0xbf sub1 init. fastwalk stack, 0xbf sub2 pushes an element to stack)
//If stack is empty key value is 0. ( never set keys to 0 in 0xbf sub 1/2)
//Because client sometimes sends bursts of 0x02’s DON’T check for a certain top stack value.
//The only safe way to detect fastwalk: push a key after EACH x021, 0x22, (=send 0xbf sub 2) check in 0x02 for stack emptyness.
//If empty -> fastwalk alert.
//Note that actual key values are irrelevant. (just don’t use 0)
//Of course without perfect 0x02/0x21/0x22 synch (serverside) it’s useless to use fastwalk detection.
//Last but not least: fastwalk detection adds 9 bytes per step and player !
CPIMoveRequest::CPIMoveRequest()
{
}
CPIMoveRequest::CPIMoveRequest( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}
void CPIMoveRequest::Receive( void )
{
	tSock->Receive( 7, false );
}
bool CPIMoveRequest::Handle( void )
{
	CChar *ourChar = tSock->CurrcharObj();
	Movement->Walking( tSock, ourChar, tSock->GetByte( 1 ), tSock->GetByte( 2 ) );
	ourChar->BreakConcentration( tSock );
	return true;
}

//0x22 Packet
//Last Modified on Wednesday, 11-Nov-1998 
//Character Move ACK/ Resync Request(3 bytes) 
//	BYTE cmd 
//	BYTE sequence (matches sent sequence) 
//	BYTE (0x00) 
CPIResyncReq::CPIResyncReq()
{
}
CPIResyncReq::CPIResyncReq( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}
void CPIResyncReq::Receive( void )
{
	tSock->Receive( 3, false );
}
bool CPIResyncReq::Handle( void )
{
	return true;
}

//0x2C Packet
//Last Modified on Friday, 19-May-2000
//Resurrection Menu Choice (2 bytes) 
//	BYTE cmd 
//	BYTE action (2=ghost, 1=resurrect, 0=from server)
//Note: Client and Server Message
//Note: Resurrection menu has been removed from UO.
CPIResMenuChoice::CPIResMenuChoice()
{
}
CPIResMenuChoice::CPIResMenuChoice( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}
void CPIResMenuChoice::Receive( void )
{
	tSock->Receive( 2, false );
}
bool CPIResMenuChoice::Handle( void )
{
	UI08 cmd = tSock->GetByte( 1 );
	if( cmd == 0x02 ) 
		tSock->sysmessage( 766 );
	if( cmd == 0x01 ) 
		tSock->sysmessage( 767 );
	return true;
}

//0x05 Packet
//Last Modified on Friday, 19-May-2000 
//Attack Request (5 bytes) 
//	BYTE cmd 
//	BYTE[4] ID to be attacked 
//Note: Client Message
CPIAttack::CPIAttack()
{
}
CPIAttack::CPIAttack( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}
void CPIAttack::Receive( void )
{
	tSock->Receive( 5, false );
}
bool CPIAttack::Handle( void )
{
	CChar *ourChar = tSock->CurrcharObj();
	ourChar->BreakConcentration( tSock );
	Combat->PlayerAttack( tSock );
	return true;
}

//0x6C Packet
//Last Modified on Sunday, 13-Feb-2000 
//Targeting Cursor Commands (19 bytes) 
//	BYTE cmd 
//	BYTE type 
//		0x00 = Select Object
//		0x01 = Select X, Y, Z 
//	BYTE[4] cursorID 
//	BYTE Cursor Type 
//	Always 0 now  
//	The following are always sent but are only valid if sent by client 
//	BYTE[4] Clicked On ID 
//	BYTE[2] click xLoc 
//	BYTE[2] click yLoc 
//	BYTE unknown (0x00) 
//	BYTE click zLoc 
//	BYTE[2] model # (if a static tile, 0 if a map/landscape tile)
//Note: the model # shouldn’t be trusted.
CPITargetCursor::CPITargetCursor()
{
}
CPITargetCursor::CPITargetCursor( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}
void CPITargetCursor::Receive( void )
{
	tSock->Receive( 19, false );
}
// bool CPITargetCursor::Handle() in targeting.cpp

//0x13 Packet
//Last Modified on Thursday, 19-Nov-1998 
//Drop->Wear Item (10 bytes) 
//	BYTE cmd 
//	BYTE[4] itemid 
//	BYTE layer (see layer list at top) 
//	BYTE[4] playerID 
//Note: The layer byte should not be trusted.  
CPIEquipItem::CPIEquipItem()
{
}
CPIEquipItem::CPIEquipItem( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}
void CPIEquipItem::Receive( void )
{
	tSock->Receive( 10, false );
}

// bool CPIEquipItem::Handle() implemented in cplayeraction.cpp

//0x07 Packet
//Last Modified on Friday, 19-May-2000
//Pick Up Item(s) (7 bytes) 
//	BYTE cmd 
//	BYTE[4] item id 
//	BYTE[2] # of items in stack 
//Note: Client Message
CPIGetItem::CPIGetItem()
{
}
CPIGetItem::CPIGetItem( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}
void CPIGetItem::Receive( void )
{
	tSock->Receive( 7, false );
}

// bool CPIGetItem::Handle() implemented in cplayeraction.cpp

//0x08 Packet
//Last Modified on Friday, 19-May-2000 
//Drop Item(s) (14 bytes) 
//	BYTE cmd 
//	BYTE[4] item id 
//	BYTE[2] X Location 
//	BYTE[2] Y Location
//	BYTE Z Location
//	BYTE[4] Move Into Container ID (FF FF FF FF if normal world) 
//Note: Client Message
//Note: 3D clients sometimes sends 2 of them (bursts) for ONE drop action. 
//The last one having –1’s in X/Y locs.
//Be very careful how to handle this odd “bursts” server side, neither always process, nor always skipping is correct.
CPIDropItem::CPIDropItem()
{
}
CPIDropItem::CPIDropItem( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}
void CPIDropItem::Receive( void )
{
	tSock->Receive( 14, false );
}

// bool CPIDropItem::Handle() implemented in cplayeraction.cpp

//0xB1 Packet
//Last Modified on Sunday, 28-Jul-2002
//Gump Menu Selection (Variable # of bytes) 
//	BYTE cmd													0
//	BYTE[2] blockSize											1
//	BYTE[4] id (first Id in 0xb0)								3
//	BYTE[4] gumpId (second Id in 0xb0)							7
//	BYTE[4] buttonId (which button perssed ? 0 if closed)		11
//	BYTE[4] switchcount  (response info for radio buttons and checkboxes, any switches listed here are switched on)	15
//	For each switch
//		BYTE[4] SwitchId
//	BYTE[4] textcount  (response info for textentries)			19 + switchcount * 4
//	For each textentry
//		BYTE[2] textId
//		BYTE[2] textlength
//		BYTE[length*2] Unicode text (not nullterminated)
CPIGumpMenuSelect::CPIGumpMenuSelect()
{
}
CPIGumpMenuSelect::CPIGumpMenuSelect( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}
void CPIGumpMenuSelect::Receive( void )
{
	tSock->Receive( 3, false );
	tSock->Receive( tSock->GetWord( 1 ), false );
}
// bool CPIGumpMenuSelect::Handle() implemented in gumps.cpp

SERIAL CPIGumpMenuSelect::ButtonID( void ) const
{
	return buttonID;
}
SERIAL CPIGumpMenuSelect::GumpID( void ) const
{
	return gumpID;
}
SERIAL CPIGumpMenuSelect::ID( void ) const
{
	return id;
}
UI32 CPIGumpMenuSelect::SwitchCount( void ) const
{
	return switchCount;
}
UI32 CPIGumpMenuSelect::TextCount( void ) const
{
	return textCount;
}

UI32 CPIGumpMenuSelect::SwitchValue( UI32 index ) const
{
	if( index >= switchCount )
		return INVALIDSERIAL;
	// we use a 0 based counter, which means that the first switch is at 19, not 15
	return tSock->GetDWord( 19 + 4 * index );
}

UI16 CPIGumpMenuSelect::GetTextID( UI08 number ) const
{
	if( number >= textCount )
		return 0xFFFF;
	return tSock->GetWord( textLocationOffsets[number] );
}
UI16 CPIGumpMenuSelect::GetTextLength( UI08 number ) const
{
	if( number >= textCount )
		return 0xFFFF;
	return tSock->GetWord( textLocationOffsets[number] + 2 );
}
std::string CPIGumpMenuSelect::GetTextString( UI08 number ) const
{
	if( number >= textCount )
		return "";
	UI08 bufferLen		= GetTextLength( number ) * 2 + 1;
	UI08 *buffer		= new UI08[bufferLen];
	UI16 bufferOffset	= textLocationOffsets[number] + 4;
	strncpy( (char *)buffer, (const char *)&(tSock->Buffer()[bufferOffset]), bufferLen );
	buffer[bufferLen-1] = 0;
	std::string toReturn = (char *)buffer;
	delete [] buffer;
	return toReturn;

}
std::string CPIGumpMenuSelect::GetTextUString( UI08 number ) const
{
	if( number >= textCount )
		return "";
	std::string toReturn = "";
	UI16 offset = textLocationOffsets[number] + 4;
	for( UI16 counter = 0; counter < GetTextLength( number ); ++counter )
		toReturn += tSock->GetByte( offset + counter * 2 + 1 );
	return toReturn;
}
void CPIGumpMenuSelect::BuildTextLocations( void )
{
	size_t i = textOffset + 4;	// first is textOffset + 4, to walk past the number of text strings
	textLocationOffsets.resize( textCount );
	for( size_t j = 0; j < textCount; ++j )
	{
		textLocationOffsets[j] = static_cast<wchar_t>(i);
		i += 2;	// skip the text ID
		UI16 textLen = tSock->GetWord( i );
		i += ( 2 * textLen + 2 );	// we need to add the + 2 for the text len field
	}
}

//	COLOUR				textColour;
//	UI16				strLen;
//	UI16				fontUsed;
//	char				txtSaid[4096];
CPITalkRequest::CPITalkRequest()
{
	InternalReset();
}
CPITalkRequest::CPITalkRequest( CSocket *s ) : CPInputBuffer( s )
{
	InternalReset();
}
void CPITalkRequest::InternalReset( void )
{
	textColour	= 0x3B2;
	strLen		= 0;
	fontUsed	= 0;
	typeUsed	= UNKNOWN;
	memset( txtSaid, 0, 4096 );
	isUnicode	= false;
}

COLOUR CPITalkRequest::TextColour( void ) const
{
	return textColour;
}
UI16 CPITalkRequest::Length( void ) const
{
	return strLen;
}
UI16 CPITalkRequest::Font( void ) const
{
	return fontUsed;
}

SpeechType CPITalkRequest::Type( void ) const
{
	return typeUsed;
}

bool CPITalkRequest::IsUnicode( void ) const
{
	return isUnicode;
}

std::string CPITalkRequest::TextString( void )	const
{
	return txtSaid;
}
char * CPITalkRequest::Text( void ) const
{
	return (char *)&txtSaid[0];
}

bool CPITalkRequest::HandleCommon( void )
{
	CChar *ourChar = tSock->CurrcharObj();
	ourChar->BreakConcentration( tSock );
	ourChar->setUnicode( false );
	CItem *speechItem = ourChar->GetSpeechItem();

	UI32 j = 0;
	char temp[1024];

	CSocket *tmpSock = NULL;

	switch( ourChar->GetSpeechMode() )
	{
		case 3: // Player vendor item pricing
			j = UString( Text() ).toULong();
			if( j > 0 )
			{
				speechItem->SetBuyValue( j );
				tSock->sysmessage( 753, j );
			} 
			else 
				tSock->sysmessage( 754, speechItem->GetBuyValue() );
			tSock->sysmessage( 755 );
			ourChar->SetSpeechMode( 4 );
			break;
		case 4: // Player vendor item describing
			speechItem->SetDesc( Text() );
			tSock->sysmessage( 756, Text() );
			ourChar->SetSpeechMode( 0 );
			break;
		case 7: // Rune renaming
			char tempname[512];
			sprintf( tempname, Dictionary->GetEntry( 684 ).c_str(), Text() );
			speechItem->SetName( tempname );
			tSock->sysmessage( 757, Text() );
			ourChar->SetSpeechMode( 0 );
			break;
		case 6: // Name deed
			ourChar->SetName( Text() );
			tSock->sysmessage( 758, Text() );
			ourChar->SetSpeechMode( 0 );
			speechItem->Delete();
			ourChar->SetSpeechItem( NULL );
			break;
		case 5:	// Key renaming
		case 8: // Sign renaming
			speechItem->SetName( Text() );
			tSock->sysmessage( 759, Text() );
			ourChar->SetSpeechMode( 0 );
			break;
		case 9: // JavaScript speech
		{
			cScript *myScript	= ourChar->GetSpeechCallback();
			if( myScript != NULL )
				myScript->OnSpeechInput( ourChar, speechItem, Text() );

			ourChar->SetSpeechMode( 0 );
			break;
		}
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

				tempPage->Reason( Text() );
				tempPage->WhoPaging( ourChar->GetSerial() );
				sprintf( temp, "GM Page from %s [%x %x %x %x]: %s", ourChar->GetName().c_str(), a1, a2, a3, a4, tempPage->Reason().c_str() );
				bool x = false;
				Network->PushConn();
				for( tmpSock = Network->FirstSocket(); !Network->FinishedSockets(); tmpSock = Network->NextSocket() )
				{
					CChar *tChar = tmpSock->CurrcharObj();
					if( !ValidateObject( tChar ) )
						continue;

					if( tChar->IsGMPageable() )
					{
						x = true;
						tmpSock->sysmessage( temp );
					}
				}
				Network->PopConn();
				if( x )
					tSock->sysmessage( 363 );
				else 
					tSock->sysmessage( 364 );
			}
			ourChar->SetSpeechMode( 0 );
			break;
		case 2: // Counselor Page
			a1 = ourChar->GetSerial( 1 );
			a2 = ourChar->GetSerial( 2 );
			a3 = ourChar->GetSerial( 3 );
			a4 = ourChar->GetSerial( 4 );

			HelpRequest *tempPageCns;
			if( CounselorQueue->GotoPos( CounselorQueue->FindCallNum( ourChar->GetPlayerCallNum() ) ) )
			{
				tempPageCns = CounselorQueue->Current();
				tempPageCns->Reason( Text() );
				tempPageCns->WhoPaging( ourChar->GetSerial() );
				sprintf( temp, "Counselor Page from %s [%x %x %x %x]: %s", ourChar->GetName().c_str(), a1, a2, a3, a4, tempPageCns->Reason().c_str() );
				bool x = false;
				Network->PushConn();
				for( tmpSock = Network->FirstSocket(); !Network->FinishedSockets(); tmpSock = Network->NextSocket() )
				{
					CChar *tChar = tmpSock->CurrcharObj();
					if( !ValidateObject( tChar ) )
						continue;

					if( tChar->IsGMPageable() )
					{
						x = true;
						tmpSock->sysmessage( temp );
					}
				}
				Network->PopConn();
				if( x )
					tSock->sysmessage( 360 );
				else 
					tSock->sysmessage( 361 );
			}
			ourChar->SetSpeechMode( 0 );
			break;
		case 0: // normal speech
		default:
			if( ourChar->GetSquelched() )
				tSock->sysmessage( 760 );
			else 
				return false;
	//			Speech->talking( this );
			break;
	}
	return true;
}

// bool CPITalkRequest::Handle() is located in speech.cpp

//Talk Request (Variable # of bytes) 
//	BYTE cmd 
//	BYTE[2] blockSize 
//	BYTE SpeechType 
//	BYTE[2] Color 
//	BYTE[2] SpeechFont 
//	BYTE[?] msg - Null Terminated (blockSize - 8)
CPITalkRequestAscii::CPITalkRequestAscii() : CPITalkRequest()
{
}
CPITalkRequestAscii::CPITalkRequestAscii( CSocket *s ) : CPITalkRequest( s )
{
	Receive();
}
void CPITalkRequestAscii::Receive( void )
{
	tSock->Receive( 3, false );
	UI16 blockLen	= tSock->GetWord( 1 );
	tSock->Receive( blockLen, false );

	strLen			= blockLen - 8;
	typeUsed		= (SpeechType)tSock->GetByte( 3 );
	textColour		= tSock->GetWord( 4 );
	fontUsed		= tSock->GetWord( 6 );
	
	strcpy( txtSaid, (char *)&(tSock->Buffer()[8]) );
}

//0xAD Packet
//Last Modified on Monday, 18-Feb-2002 
//Unicode/Ascii speech request (Variable # of bytes)
//BYTE cmd												0
//	BYTE[2] blockSize									1
//	BYTE Type											3
//	BYTE[2] Color										4
//	BYTE[2] Font										6
//	BYTE[4] Language (Null Terminated)					8
//		“enu“ - United States English
//		“des” - German Swiss
//		“dea” - German Austria
//		“deu” - German Germany
//		... for a complete list see langcode.iff
//	if (Type & 0xc0) 
//		BYTE[1,5] Number of distinct Trigger words (NUMWORDS).
//			12 Bit number, Byte #13  = Bit 11…4 of NUMWORDS, Hi-Nibble of Byte #14 (Bit 7…4)  = Bit  0…3 of NUMWORDS
//		BYTE[1,5] Index to speech.mul
//			12 Bit number, Low Nibble of Byte #14 (Bits 3…0) = Bits 11..8 of Index, Byte #15  = Bits 7…0 of Index
//		UNKNOWNS = ( (NUMWORDS div 2) *3 ) + (NUMWORDS % 2)  – 1
//			div = Integeger division, % = modulo operation, NUMWORDS >= 1
//			examples: UNKNOWNS(1)=0, UNKNOWNS(2)=2, UNKNOWNS(3)=3, UNKNOWNS(4)=5, UNKNOWNS(5)=6, 
//			UNKNOWNS(6)=8, UNKNOWNS(7)=9, UNKNOWNS(8)=11, UNKNOWNS(9)=12 
//		BYTE[UNKNOWNS]
//			Idea behind this is getting speech parsing load client side.
//			Thus this contains data OSI server use for easier parsing.
//			It’s client side hardcoded and exact details are unkown.
//		BYTE[?] Ascii Msg – Null Terminated (blockSize – (15+UNKNOWNS) )
//	else 
//		BYTE[?][2] Unicode Msg - Null Terminated (blockSize - 12)
//	Note1:  For pre 2.0.7 clients Type is always < 0xc0
//	Note2: Uox based emus convert post 2.0.7 data of this packet to pre 2.0.7 data if Type >=0xc0
//	Note3: (different view of it)
//	If Mode&0xc0 then there are keywords (from speech.mul) present. 
//	Keywords:
//	The first 12 bits = the number of keywords present. The keywords are included right after this, each one is 12 bits also. 
//	The keywords are padded to the closest byte. For example, if there are 2 keywords, it will take up 5 bytes. 12bits for the number, and 12 bits for each keyword. 12+12+12=36. Which will be padded 4 bits to 40 bits or 5 bytes.

void UpdateLanguage( CSocket *s );

CPITalkRequestUnicode::CPITalkRequestUnicode()
{
	isUnicode = true;
	memset( unicodeTxt, 0, 8192 );
}
CPITalkRequestUnicode::CPITalkRequestUnicode( CSocket *s ) : CPITalkRequest( s )
{
	isUnicode = true;
	memset( unicodeTxt, 0, 8192 );
	Receive();
}
void CPITalkRequestUnicode::Receive( void )
{
	tSock->Receive( 3, false );
	UI16 blockLen	= tSock->GetWord( 1 );
	tSock->Receive( blockLen, false );
	tSock->ClearTrigWords();

//	strLen			= blockLen - 8;
	typeUsed		= (SpeechType)tSock->GetByte( 3 );
	textColour		= tSock->GetWord( 4 );
	fontUsed		= tSock->GetWord( 6 );

	UpdateLanguage( tSock );

	CChar *mChar	= tSock->CurrcharObj();
	mChar->setUnicode( true );

	// Check for command word versions of this packet
	int myoffset		= 13;
	int myj				= 12;
	size_t numTrigWords	= 0;
	UI08 *buffer		= tSock->Buffer();
	UI16 j				= 0;

	for( j = 8; j <= 10; ++j )
		langCode[j - 8] = tSock->GetByte( j );

	langCode[3] = 0;

	if( (typeUsed&0xC0) == 0xC0 )
	{
		// numbwe of distinct trigger words
		numTrigWords = (tSock->GetByte( 12 )<<4) + (tSock->GetByte( 13 )>>4);

		size_t byteNum = 13;
		for( size_t tWord = 0; tWord < numTrigWords; )
		{
			tSock->AddTrigWord( (tSock->GetByte( byteNum+tWord )%0x10<<8) + tSock->GetByte( byteNum+tWord+1 ) );
			if( tWord+2 <= numTrigWords )
				tSock->AddTrigWord( (tSock->GetByte( byteNum+tWord+2 )<<8) + (tSock->GetByte( byteNum+tWord+3 )>>4));
			tWord += 2;
			byteNum++;
		}

		myoffset = 15;
		if( (numTrigWords % 2) == 1 )	// odd number ?
			myoffset += ( numTrigWords / 2 ) * 3;
		else
			myoffset += ( ( numTrigWords / 2 ) * 3 ) - 1;

		myj = 12;
		int mysize = blockLen - myoffset;
		for( j = 0; j < mysize; ++j )
			unicodeTxt[j] = buffer[j + myoffset];
		for( j = 0; j < mysize; ++j )
		{	// we would overwrite our buffer, so we need to catch it before we do that.
			buffer[myj++] = 0;
			buffer[myj++] = unicodeTxt[j];
		}
		buffer[myj-1] = 0;
		blockLen = ((blockLen - myoffset) * 2) + 12;
		PackShort( buffer, 1, blockLen );
		// update our unicode text
		memcpy( unicodeTxt, &(tSock->Buffer()[12]), blockLen - 12 );
	}
	else		// NOT a weird configuration, it is unicode however!
	{
		// starts at 12
		memcpy( unicodeTxt, &(tSock->Buffer()[12]), blockLen - 12 );
	}
	strLen = 0;
	for( j = 13; j < blockLen; j += 2 )
	{
		txtSaid[(j-13)/2] = buffer[j];
		++strLen;
	}
}

std::string CPITalkRequestUnicode::UnicodeTextString( void ) const
{
	return unicodeTxt;
}
char * CPITalkRequestUnicode::UnicodeText( void ) const
{
	return (char *)unicodeTxt;
}

char * CPITalkRequestUnicode::Language( void ) const
{
	return (char *)langCode;
}

CPIAllNames3D::CPIAllNames3D() : CPInputBuffer()
{
}
CPIAllNames3D::CPIAllNames3D( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}
void CPIAllNames3D::Receive( void )
{
	tSock->Receive( 3, false );
	tSock->Receive( tSock->GetWord( 1 ), false );
}
bool CPIAllNames3D::Handle( void )
{
	SERIAL objSer		= tSock->GetDWord( 3 );
	CBaseObject *toName = NULL;
	
	if( objSer < BASEITEMSERIAL )
		toName = calcItemObjFromSer( objSer );
	else if( objSer != INVALIDSERIAL )
		toName = calcCharObjFromSer( objSer );

	if( ValidateObject( toName ) )
	{
		CPAllNames3D toSend( (*toName) );
		tSock->Send( &toSend );
	}
	return true;
}

//0x7D Packet
//Last Modified on Friday, 3-Oct-2003 
//Make a choice in a Gump (13 bytes) 
//	BYTE cmd 
//	BYTE[4] dialogID (echoed back from 7C packet) 
//	BYTE[2] menuid (echoed back from 7C packet) 
//	BYTE[2] 1-based index of choice 
//	BYTE[2] model # of choice 
//	BYTE[2] color

CPIGumpChoice::CPIGumpChoice()
{
}
CPIGumpChoice::CPIGumpChoice( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}
void CPIGumpChoice::Receive( void )
{
	tSock->Receive( 13, false );
}

// void CPGumpChoice::Handle() implemented in gumps.cpp

//0x3B Packet
//Last Modified on Friday, 3-Oct-2003 
//Buy Item(s) (Variable # of bytes)
//	BYTE cmd 
//	BYTE[2] blockSize 
//	BYTE[4] vendorID 
//	BYTE flag 
//		0x00 - no items following 
//		0x02 - items following 
//	For each item 
//		BYTE (0x1A) 
//		BYTE[4] itemID (from 3C packet) 
//		BYTE[2] # bought 
CPIBuyItem::CPIBuyItem()
{
}
CPIBuyItem::CPIBuyItem( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}
void CPIBuyItem::Receive( void )
{
	tSock->Receive( 3, false );
	tSock->Receive( tSock->GetWord( 1 ), false );
}

// void CPIBuyItem::Handle() implemented in vendor.cpp

//0x9F Packet
//Last Modified on Friday, 3-Oct-2003 
//Sell Reply (Variable # of bytes) 
//	BYTE cmd 
//	BYTE[2] blockSize
//	BYTE[4] shopkeeperID
//	BYTE[2] itemCount
//	For each item, a structure containing:
//		BYTE[4] itemID
//		BYTE[2] quantity

CPISellItem::CPISellItem()
{
}
CPISellItem::CPISellItem( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}
void CPISellItem::Receive( void )
{
	tSock->Receive( 3, false );
	tSock->Receive( tSock->GetWord( 1 ), false );
}

// void CPISellItem::Handle() implemented in vendor.cpp

//0x83 Packet
//Last Modified on Sunday, 19-Oct-2003 
//Delete Character (39 bytes)
//	BYTE cmd
//	BYTE[30] password
//	BYTE[4] charIndex
//	BYTE[4]	clientIP
CPIDeleteCharacter::CPIDeleteCharacter()
{
}
CPIDeleteCharacter::CPIDeleteCharacter( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}
void CPIDeleteCharacter::Receive( void )
{
	tSock->Receive( 0x27, false );
}

// void CPIDeleteCharacter::Handle() implimented in pcmanage.cpp	

//0x00 Packet
//Last Modified on Sunday, 19-Oct-2003 
//Create Character (104 bytes) 
//	BYTE cmd								0
//	BYTE[4] pattern1 (0xedededed)			1
//	BYTE[4] pattern2 (0xffffffff)			5
//	BYTE pattern3 (0x00)					9
//	BYTE[30] char name						10
//	BYTE[30] char password					40
//	BYTE sex (0=male, 1=female)				70
//	BYTE str								71
//	BYTE dex								72
//	BYTE int								73
//	BYTE skill1								74
//	BYTE skill1value						75
//	BYTE skill2								76
//	BYTE skill2value						77
//	BYTE skill3								78	
//	BYTE skill3value						79
//	BYTE[2] skinColor						80
//	BYTE[2] hairStyle						82
//	BYTE[2] hairColor						84
//	BYTE[2] facial hair						86
//	BYTE[2] facial hair color				88
//	BYTE[2] location # from starting list	90
//	BYTE[2] unknown1						92
//	BYTE[2] slot							94
//	BYTE[4] clientIP						96
//	BYTE[2] shirt color						100
//	BYTE[2] pants color						102
CPICreateCharacter::CPICreateCharacter()
{
}
CPICreateCharacter::CPICreateCharacter( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}
void CPICreateCharacter::Receive( void )
{
	tSock->Receive( 104, false );
	Network->Transfer( tSock );

	pattern1		= tSock->GetDWord( 1 );
	pattern2		= tSock->GetDWord( 5 );
	pattern3		= tSock->GetByte( 9 );
	sex				= tSock->GetByte( 70 );
	str				= tSock->GetByte( 71 );
	dex				= tSock->GetByte( 72 );
	intel			= tSock->GetByte( 73 );
	skill[0]		= tSock->GetByte( 74 );
	skillValue[0]	= tSock->GetByte( 75 );
	skill[1]		= tSock->GetByte( 76 );
	skillValue[1]	= tSock->GetByte( 77 );
	skill[2]		= tSock->GetByte( 78 );
	skillValue[2]	= tSock->GetByte( 79 );
	skinColour		= tSock->GetWord( 80 );
	hairStyle		= tSock->GetWord( 82 );
	hairColour		= tSock->GetWord( 84 );
	facialHair		= tSock->GetWord( 86 );
	facialHairColour= tSock->GetWord( 88 );
	locationNumber	= tSock->GetWord( 90 );
	unknown			= tSock->GetWord( 92 );
	slot			= tSock->GetWord( 94 );
	ipAddress		= tSock->GetDWord( 96 );
	shirtColour		= tSock->GetWord( 100 );
	pantsColour		= tSock->GetWord( 102 );
	memcpy( charName, &tSock->Buffer()[10], 30 );
	memcpy( password, &tSock->Buffer()[40], 30 );
}

void CPICreateCharacter::Log( std::ofstream &outStream, bool fullHeader )
{
	if( fullHeader )
		outStream << "[RECV]Packet   : CPICreateCharacter 0x00 --> Length: 104" << TimeStamp() << std::endl;

	outStream << "Pattern1       : " << pattern1 << std::endl;
	outStream << "Pattern2       : " << pattern2 << std::endl;
	outStream << "Pattern3       : " << (int)pattern3 << std::endl;
	outStream << "Character Name : " << charName << std::endl;
	outStream << "Password       : " << password << std::endl;
	outStream << "Sex            : " << (int)sex << std::endl;
	outStream << "Strength       : " << (int)str << std::endl;
	outStream << "Dexterity      : " << (int)dex << std::endl;
	outStream << "Intelligence   : " << (int)intel << std::endl;
	outStream << "Skills         : " << (int)skill[0] << " " << (int)skill[1] << " " << (int)skill[2] << std::endl;
	outStream << "Skills Values  : " << (int)skillValue[0] << " " << (int)skillValue[1] << " " << (int)skillValue[2] << std::endl;
	outStream << "Skin Colour    : " << std::hex << skinColour << std::dec << std::endl;
	outStream << "Hair Style     : " << std::hex << hairStyle << std::dec << std::endl;
	outStream << "Hair Colour    : " << std::hex << hairColour << std::dec << std::endl;
	outStream << "Facial Hair    : " << std::hex << facialHair << std::dec << std::endl;
	outStream << "Facial Hair Colour: " << std::hex << facialHairColour << std::dec << std::endl;
	outStream << "Location Number: " << locationNumber << std::endl;
	outStream << "Unknown        : " << unknown << std::endl;
	outStream << "Slot           : " << (int)slot << std::endl;
	outStream << "IP Address     : " << ipAddress << std::endl;
	outStream << "Shirt Colour   : " << std::hex << shirtColour << std::dec << std::endl;
	outStream << "Pants Colour   : " << std::hex << pantsColour << std::dec << std::endl;
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}


// void CPICreateCharacter::Handle() implimented in pcmanage.cpp

//0x5D Packet
//Last Modified on Sunday, 19-Oct-2003 
//Login Character (73 bytes) 
//	BYTE cmd 
//	BYTE[4] pattern1 (0xedededed) 
//	BYTE[30] char name (0 terminated)
//	BYTE[33] unknown, mostly 0’s  (Byte# 0x27, 0x28, 0x30  seem to be the only non 0’s of these 33 bytes. Perhaps password data that’s not send anymore)
//	BYTE slot choosen (0-based) 
//	BYTE[4] clientIP

CPIPlayCharacter::CPIPlayCharacter()
{
}
CPIPlayCharacter::CPIPlayCharacter( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}
void CPIPlayCharacter::Receive( void )
{
	tSock->Receive( 0x49, false );
	Network->Transfer( tSock );
	messageLoop << "Activating Chat, LBR, AOS, 6Char support on connected client.";

	// Let's store our data more meaningfully

	pattern		= tSock->GetDWord( 1 );
	slotChosen	= tSock->GetByte( 68 );
	ipAddress	= tSock->GetDWord( 69 );
	memcpy( charName, &tSock->Buffer()[5], 30 );
	memcpy( unknown, &tSock->Buffer()[35], 33 );
}

void CPIPlayCharacter::Log( std::ofstream &outStream, bool fullHeader )
{
	if( fullHeader )
		outStream << "[RECV]Packet   : CPICreateCharacter 0x5D --> Length: 73" << TimeStamp() << std::endl;
	outStream << "Pattern1       : " << pattern << std::endl;
	outStream << "Char name      : " << charName << std::endl;
	outStream << "Slot chosen    : " << (int)slotChosen << std::endl;
	outStream << "Client IP      : " << ipAddress << std::endl;
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}

// void CPIPlayCharacter::Handle() implimented in pcmanage.cpp

//0xAC Packet
//Last Modified on Sunday, Saturday, 19-Feb-2000
//Gump Text Entry Dialog Reply (Variable # of bytes) 
//	BYTE cmd 
//	BYTE[2] length
//	BYTE[4] ID
//	BYTE type
//	BYTE index
//	BYTE[3] unk
//	BYTE[?] reply

CPIGumpInput::CPIGumpInput()
{
}
CPIGumpInput::CPIGumpInput( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}
void CPIGumpInput::Receive( void )
{
	tSock->Receive( 3, false );
	UI16 length = tSock->GetWord( 1 );
	tSock->Receive( length, false );

	id		= tSock->GetDWord( 3 );
	type	= tSock->GetByte( 7 );
	index	= tSock->GetByte( 8 );
	unk[0]	= tSock->GetByte( 9 );
	unk[1]	= tSock->GetByte( 10 );
	unk[2]	= tSock->GetByte( 11 );
	reply	= (char *)&(tSock->Buffer()[12]);
}

UI32 CPIGumpInput::ID( void ) const
{
	return id;
}
UI08 CPIGumpInput::Type( void ) const
{
	return type;
}
UI08 CPIGumpInput::Index( void ) const
{
	return index;
}
UI08 CPIGumpInput::Unk( int offset ) const
{
	assert( offset >= 0 && offset <=2 );
	return unk[offset];
}
const UString CPIGumpInput::Reply( void ) const
{
	return reply;
}

void CPIGumpInput::Log( std::ofstream &outStream, bool fullHeader )
{
	if( fullHeader )
		outStream << "[RECV]Packet   : CPIGumpInput 0xAC --> Length: " << tSock->GetWord( 1 ) << TimeStamp() << std::endl;
	outStream << "ID             : " << id << std::endl;
	outStream << "Type           : " << (int)type << std::endl;
	outStream << "Index          : " << (int)index << std::endl;
	outStream << "Unknown        : " << (int)unk[0] << " " << (int)unk[1] << " " << (int)unk[2] << std::endl;
	outStream << "Reply          : " << reply << std::endl;
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}

// void CPIGumpInput::Handle() implimented in gumps.cpp

//0x9B Packet
//Last Modified on Wednesday, 06-May-1998
//Request Help (258 bytes) 
//	BYTE cmd 
//	BYTE[257] (0x00)

CPIHelpRequest::CPIHelpRequest( CSocket *s, UI16 menuVal ) : CPInputBuffer( s )
{
	menuNum = menuVal;
}
CPIHelpRequest::CPIHelpRequest( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}
void CPIHelpRequest::Receive( void )
{
	tSock->Receive( 0x102, false );
	menuNum = 1;
}

// void CPIHelpRequest::Handle() implimented in gumps.cpp

//0x6F Packet
//Last Modified on Friday, 20-Nov-1998 
//Secure Trading (Variable # of bytes) 
//	BYTE cmd 
//	BYTE[2] blockSize 
//	BYTE action
//	BYTE[4] id1
//	BYTE[4] id2
//	BYTE[4] id3
//	BYTE nameFollowing (0 or 1)
//If (nameFollowing = 1) 
//	BYTE[?] charName

CPITradeMessage::CPITradeMessage()
{
}
CPITradeMessage::CPITradeMessage( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}
void CPITradeMessage::Receive( void )
{
	tSock->Receive( 3, false );
	tSock->Receive( tSock->GetWord( 1 ), false );
}

// void CPITradeMessage::Handle() implimented in gumps.cpp

//0x95 Packet
//Last Modified on Thursday, 30-Apr-1998 18:34:08 EDT 
//Dye Window (9 bytes) 
//	BYTE cmd 
//	BYTE[4] itemID of dye target
//	BYTE[2] ignored on send, model on return
//	BYTE[2] model on send, color on return  (default on server send is 0x0FAB)

CPIDyeWindow::CPIDyeWindow()
{
}
CPIDyeWindow::CPIDyeWindow( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}
void CPIDyeWindow::Receive( void )
{
	tSock->Receive( 9, false );

	changing	= tSock->GetDWord( 1 );
	modelID		= tSock->GetWord( 5 );
	newValue	= tSock->GetWord( 7 );
}

void CPIDyeWindow::Log( std::ofstream &outStream, bool fullHeader )
{
	if( fullHeader )
		outStream << "[RECV]Packet   : CPIDyeWindow 0x95 --> Length: 9" << TimeStamp() << std::endl;
	outStream << "ItemID         : " << changing << std::endl;
	outStream << "Model          : " << modelID << std::endl;
	outStream << "Colour         : " << newValue << std::endl;
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}

bool CPIDyeWindow::Handle( void )
{
	if( changing >= BASEITEMSERIAL )
	{
		CItem *i = calcItemObjFromSer( changing );
		if( ValidateObject( i ) )
		{
			if( !tSock->DyeAll() )
			{
				if( newValue < 0x0002 || newValue > 0x03E9 )
					newValue = 0x03E9;
			}
			i->SetColour( newValue );
		}
	}
	else
	{
		CChar *mChar	= tSock->CurrcharObj();
		CChar *c		= calcCharObjFromSer( changing );
		if( ValidateObject( c ) )
		{
			if( !mChar->IsGM() ) 
				return true;

			UI16 body = c->GetID();
			SI32 b = newValue&0x4000; 

			if( ( ( newValue>>8 ) < 0x80 ) && ( (body >= 0x0190 && body <= 0x0193) || (body >= 0x025D && body <= 0x0260) ) )
				newValue += 0x8000;

			if( b == 16384 && (body >= 0x0190 && body <= 0x03e1 ) ) 
				newValue = 0xF000; // but assigning the only "transparent" value that works, namly semi-trasnparency.

			if( newValue != 0x8000 )
			{
				c->SetSkin( newValue );
				c->SetOrgSkin( newValue );
			} 
		}
	}
	Effects->PlaySound( tSock, 0x023E, true );
	return true;
}

//0x71 Packet
//Last Modified on Wednesday, 24-May-2000
//Bulletin Board Message (Variable # of bytes)
//	BYTE cmd
//	BYTE[2] len
//	BYTE subcmd
//	BYTE[len-4] submessage

CPIMsgBoardEvent::CPIMsgBoardEvent()
{
}
CPIMsgBoardEvent::CPIMsgBoardEvent( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}
CPIMsgBoardEvent::CPIMsgBoardEvent( CSocket *s, bool receive ) : CPInputBuffer( s )
{
}

void CPIMsgBoardEvent::Receive( void )
{
	tSock->Receive( 3, false );
	tSock->Receive( tSock->GetWord( 1 ), false );
}

// bool CPIMsgBoardEvent::Handle() implemented in msgboard.cpp

CPINewBookHeader::CPINewBookHeader()
{
}

CPINewBookHeader::CPINewBookHeader( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}

void CPINewBookHeader::Receive( void )
{
	tSock->Receive( 3, false );
	tSock->Receive( tSock->GetWord( 1 ), false );
}

// bool CPINewBookHeader::Handle() implemented in books.cpp


CPIBookPage::CPIBookPage()
{
}

CPIBookPage::CPIBookPage( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}

void CPIBookPage::Receive( void )
{
	tSock->Receive( 3, false );
	tSock->Receive( tSock->GetWord( 1 ), false );
}

// bool CPIBookPage::Handle() implemented in books.cpp

CPIMetrics::CPIMetrics()
{
}

CPIMetrics::CPIMetrics( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}

void CPIMetrics::Receive( void )
{
//	tSock->Receive( 3, false );
//	tSock->Receive( tSock->GetWord( 1 ), false );
	tSock->Receive( 0x010C, false );
}

bool CPIMetrics::Handle()
{
	// we're going to silently swallow this packet, really
	return true;
}

void CPIMetrics::Log( std::ofstream &outStream, bool fullHeader )
{
	if( fullHeader )
		outStream << "[RECV]Packet   : CPIMetrics 0xD9 --> Length: 268" << TimeStamp() << std::endl;
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}

}
