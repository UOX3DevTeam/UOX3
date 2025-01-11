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
#include "books.h"
#include "cMagic.h"
#include "skills.h"
#include "PartySystem.h"
#include "cGuild.h"
#include "CResponse.h"
#include "StringUtility.hpp"
#include "cRaces.h"
#include <chrono>
#include "IP4Address.hpp"

//o------------------------------------------------------------------------------------------------o
//| Function	-	pSplit()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Split login password into UOX password and UO password (???)
//o------------------------------------------------------------------------------------------------o
void pSplit( const std::string pass0, std::string &pass1, std::string &pass2 )
{
	size_t i = 0;
	pass1 = "";
	auto pass0Len = pass0.length();
	while( i < pass0Len && pass0[i] != '/' )
	{
		++i;
	}
	pass1 = pass0.substr( 0, i );
	if( i < pass0Len )
	{
		pass2 = pass0.substr( i );
	}
}

void PackShort( UI08 *toPack, SI32 offset, UI16 value )
{
	toPack[offset + 0] = static_cast<UI08>(( value & 0xFF00 ) >> 8 );
	toPack[offset + 1] = static_cast<UI08>(( value & 0x00FF ) % 256 );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	WhichLoginPacket()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles login packets from client
//o------------------------------------------------------------------------------------------------o
CPInputBuffer *WhichLoginPacket( UI08 packetId, CSocket *s )
{
	switch( packetId )
	{
		case 0x00:	return ( new CPICreateCharacter( s ));	// Character Create
		case 0x01:	return nullptr;							// Main Menu on the character select screen
		case 0x04:	return ( new CPIGodModeToggle( s ));
		case 0x5D:	return ( new CPIPlayCharacter( s ));	// Character Select
		case 0x73:	return ( new CPIKeepAlive( s ));
		case 0x80:	return ( new CPIFirstLogin( s ));
		case 0x83:	return ( new CPIDeleteCharacter( s ));	// Character Delete
		case 0x8D:	return ( new CPICreateCharacter( s ));	// Character creation for 3D/Enhanced clients
		case 0x91:	return ( new CPISecondLogin( s ));
		case 0xA0:	return ( new CPIServerSelect( s ));
		case 0xA4:	return ( new CPISpy( s ));
		case 0xBB:	return nullptr;							// Ultima Messenger - old, no longer used
		case 0xBD:	return ( new CPIClientVersion( s ));
		case 0xBF:	return ( new CPISubcommands( s ));		// general overall packet
		case 0xC0:	return ( new CPINewClientVersion( s ));	// LoginSeed/New client-version clients before 6.0.x
		case 0xD9:	return ( new CPIMetrics( s ));			// Client Hardware / Metrics
		case 0xEF:	return ( new CPINewClientVersion( s ));	// LoginSeed/New client-version clients after 6.0.x
		//case 0xE4:	return ( new CPIKREncryptionVerification( s )); // KR Encryption Response verification
		//case 0xF1:	return nullptr;						// Freeshard Server Poll Packet - handled in packet hook JS script
		case 0xF8:	return ( new CPICreateCharacter( s ));	// New Character Create - minor difference from original
		case 0xFF:	return nullptr;							// new CPIKRSeed( s ) - KR client request for encryption response
		default:	break;
	}
	throw socket_error( "Bad packet request" );
	return nullptr;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	WhichPacket()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles packets received from client
//o------------------------------------------------------------------------------------------------o
CPInputBuffer *WhichPacket( UI08 packetId, CSocket *s )
{
	switch( packetId )
	{
		case 0x00:	return ( new CPICreateCharacter( s ));
		case 0x02:	return ( new CPIMoveRequest( s ));
		case 0x03:	return ( new CPITalkRequestAscii( s ));
		case 0x04:	return ( new CPIGodModeToggle( s ));
		case 0x05:	return ( new CPIAttack( s ));
		case 0x06:	return ( new CPIDblClick( s ));
		case 0x07:	return ( new CPIGetItem( s ));
		case 0x08:	return ( new CPIDropItem( s ));
		case 0x09:	return ( new CPISingleClick( s ));
		case 0x12:	return nullptr;								// Request Skill / Action / Magic Usage
		case 0x13:	return ( new CPIEquipItem( s ));
		case 0x22:	return ( new CPIResyncReq( s ));
		case 0x2C:	return ( new CPIResMenuChoice( s ));
		case 0x34:	return ( new CPIStatusRequest( s ));
		case 0x3A:	return nullptr;								// Skills management packet
		case 0x3B:	return ( new CPIBuyItem( s ));
		case 0x56:	return nullptr;								// Map Related
		case 0x5D:	return ( new CPIPlayCharacter( s ));
		case 0x66:	return ( new CPIBookPage( s ));				// Player Turns the Page (or closes) a Book
		case 0x69:	return nullptr;								// Client text change
		case 0x6C:	return ( new CPITargetCursor( s ));
		case 0x6F:	return ( new CPITradeMessage( s ));
		case 0x71:	return ( new CPIMsgBoardEvent( s ));		// Message Board Item
		case 0x72:	return nullptr;								// Combat mode
		case 0x73:	return ( new CPIKeepAlive( s ));
		case 0x75:	return ( new CPIRename( s ));
		case 0x7D:	return ( new CPIGumpChoice( s ));
		case 0x80:	return ( new CPIFirstLogin( s ));
		case 0x83:	return ( new CPIDeleteCharacter( s ));
		case 0x8D:	return ( new CPICreateCharacter( s ));		// Character creation for 3D/Enhanced clients
		case 0x91:	return ( new CPISecondLogin( s ));
		case 0x93:	return nullptr;								// Books - title page
		case 0x95:	return ( new CPIDyeWindow( s ));
		case 0x98:	return ( new CPIAllNames3D( s ));
		case 0x9B:	return ( new CPIHelpRequest( s ));
		case 0x9F:	return ( new CPISellItem( s ));
		case 0xA0:	return ( new CPIServerSelect( s ));
		case 0xA4:	return ( new CPISpy( s ));
		case 0xA7:	return ( new CPITips( s ));
		case 0xAC:	return ( new CPIGumpInput( s ));
		case 0xAD:	return ( new CPITalkRequestUnicode( s ));
		case 0xB1:	return ( new CPIGumpMenuSelect( s ));
		case 0xB6:	return nullptr;								// T2A Popup Help Request
		case 0xB8:	return nullptr;								// T2A Profile Request
		case 0xBB:	return nullptr;								// Ultima Messenger
		case 0xBD:	return ( new CPIClientVersion( s ));
		case 0xBF:	return ( new CPISubcommands( s ));			// Assorted commands
		case 0xC8:	return ( new CPIUpdateRangeChange( s ));
		case 0xC0:	return ( new CPINewClientVersion( s ));		// LoginSeed/New client-version clients before 6.0.x
		case 0xD0:	return nullptr;								// Configuration File
		case 0xD1:	return ( new CPILogoutStatus( s ));			// Logout Status
		case 0xD4:	return ( new CPINewBookHeader( s ));		// New Book Header
		case 0xD6:	return ( new CPIToolTipRequestAoS( s ));	// AoS Request for tooltip data
		case 0xD7:	return ( new CPIAOSCommand( s ));			// AOS Command
		case 0xD9:	return ( new CPIMetrics( s ));				// Client Hardware / Metrics
		case 0xF0:	return ( new CPIKrriosClientSpecial( s ));	// Responses to special client packet also used by assistant tools to negotiate features with server
		case 0xF3:	return nullptr;								// TODO - Object Information (SA)
		case 0xF5:	return nullptr;								// TODO - New Map Message
		case 0xF8:	return ( new CPICreateCharacter( s ));		// New Character Create
		default:	return nullptr;
	}
	return nullptr;
}

void CheckBanTimer( CAccountBlock_st &actbTemp )
{
	// Unban player if bantime is up, but don't do anything if bantime is zero - maybe player is permanently banned!
	if( actbTemp.wFlags.test( AB_FLAGS_BANNED ) && ( actbTemp.wTimeBan > 0 && actbTemp.wTimeBan <= GetMinutesSinceEpoch() ))
	{
		actbTemp.wTimeBan = 0x0;
		actbTemp.wFlags.set( AB_FLAGS_BANNED, false );
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIFirstLogin()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet for client first login - login server
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x80 (Login Request)
//|					Size: 62 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[30] userid
//|						BYTE[30] password
//|						BYTE unknown1 (not usually 0x00 - so not nullptr)
//o------------------------------------------------------------------------------------------------o
void CPIFirstLogin::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[RECV]Packet   : CPIFirstLogin 0x80 --> Length: 62" << TimeStamp() << std::endl;
	}
	outStream << "UserId         : " << Name() << std::endl;
	outStream << "Password       : " << Pass() << std::endl;
	outStream << "Unknown        : " << static_cast<SI16>( Unknown() ) << std::endl;
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}

bool CPIFirstLogin::Handle( void )
{
	tSock->AcctNo( AB_INVALID_ID );
	LoginDenyReason t = LDR_NODENY;

	std::string username = Name();

	CAccountBlock_st * actbTemp = &Accounts->GetAccountByName( username );
	if( actbTemp->wAccountIndex != AB_INVALID_ID )
	{
		tSock->SetAccount(( *actbTemp ));
	}

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
			{
				tSock->SetAccount(( *actbTemp ));
			}
		}
	}
	if( tSock->AcctNo() != AB_INVALID_ID )
	{
		// Check if player's bantime is up, and if so, unban player
		CheckBanTimer( *actbTemp );

		if( actbTemp->wFlags.test( AB_FLAGS_BANNED ))
		{
			t = LDR_ACCOUNTDISABLED;
		}
		else if( actbTemp->sPassword != pass1 )
		{
			t = LDR_BADPASSWORD;
		}
		else
		{
			// Handle client-restrictions, disconnect if client-version isn't supported
			// Client-versions below 6.0.5.0 can only be verified after they're ingame, so can only
			// be blocked in FirstLogin if _all_ client versions below 6.0.5.0 are blocked
			if( tSock->ClientType() == CV_T2A )
			{
				if( !cwmWorldState->ServerData()->ClientSupport4000() && !cwmWorldState->ServerData()->ClientSupport5000() && !cwmWorldState->ServerData()->ClientSupport6000() )
				{
					t = LDR_COMMSFAILURE;
					Console << "Login denied - unsupported client (4.0.0 - 6.0.4.x). See UOX.INI..." << myendl;
				}
			}
			else if( tSock->ClientType() <= CV_KR3D && tSock->ClientType() != CV_DEFAULT )
			{
				if( !cwmWorldState->ServerData()->ClientSupport6050() )
				{
					t = LDR_COMMSFAILURE;
					Console << "Login denied - unsupported client (6.0.5.0 - 6.0.14.2). See UOX.INI..." << myendl;
				}
			}
			else if( tSock->ClientType() <= CV_SA3D && tSock->ClientType() != CV_DEFAULT )
			{
				if( !cwmWorldState->ServerData()->ClientSupport7000() )
				{
					t = LDR_COMMSFAILURE;
					Console << "Login denied - unsupported client (7.0.0.0 - 7.0.8.2). See UOX.INI..." << myendl;
				}
			}
			else if( tSock->ClientType() <= CV_HS3D && tSock->ClientType() != CV_DEFAULT  )
			{
				if( tSock->ClientVerShort() < CVS_70160 )
				{
					if( !cwmWorldState->ServerData()->ClientSupport7090() )
					{
						t = LDR_COMMSFAILURE;
						Console << "Login denied - unsupported client (7.0.9.0 - 7.0.15.1). See UOX.INI..." << myendl;
					}
				}
				else if( tSock->ClientVerShort() < CVS_70240 )
				{
					if( !cwmWorldState->ServerData()->ClientSupport70160() )
					{
						t = LDR_COMMSFAILURE;
						Console << "Login denied - unsupported client (7.0.16.0 - 7.0.23.1). See UOX.INI..." << myendl;
					}
				}
				else if( tSock->ClientVerShort() < CVS_70300 )
				{
					if( !cwmWorldState->ServerData()->ClientSupport70240() )
					{
						t = LDR_COMMSFAILURE;
						Console << "Login denied - unsupported client (7.0.24.0+). See UOX.INI..." << myendl;
					}
				}
				else if( tSock->ClientVerShort() < CVS_70331 )
				{
					if( !cwmWorldState->ServerData()->ClientSupport70300() )
					{
						t = LDR_COMMSFAILURE;
						Console << "Login denied - unsupported client (7.0.30.0+). See UOX.INI..." << myendl;
					}
				}
				else if( tSock->ClientVerShort() < CVS_704565 )
				{
					if( !cwmWorldState->ServerData()->ClientSupport70331() )
					{
						t = LDR_COMMSFAILURE;
						Console << "Login denied - unsupported client (7.0.33.1+). See UOX.INI..." << myendl;
					}
				}
				else if( tSock->ClientVerShort() < CVS_70610 )
				{
					if( !cwmWorldState->ServerData()->ClientSupport704565() )
					{
						t = LDR_COMMSFAILURE;
						Console << "Login denied - unsupported client (7.0.45.65+). See UOX.INI..." << myendl;
					}
				}
				else if( tSock->ClientVerShort() >= CVS_70610 )
				{
					if( !cwmWorldState->ServerData()->ClientSupport70610() )
					{
						t = LDR_COMMSFAILURE;
						Console << "Login denied - unsupported client (7.0.61.0+). See UOX.INI..." << myendl;
					}
				}
			}
		}
	}
	else
	{
		t = LDR_UNKNOWNUSER;
	}

	if( t == LDR_NODENY && actbTemp->wFlags.test( AB_FLAGS_ONLINE ))
	{
		t = LDR_ACCOUNTINUSE;
	}
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
		actbTemp->dwLastIP = CalcSerial( tSock->ClientIP4(), tSock->ClientIP3(), tSock->ClientIP2(), tSock->ClientIP1() );
		auto temp = oldstrutil::format( "Client [%i.%i.%i.%i] connected using Account '%s'.", tSock->ClientIP4(), tSock->ClientIP3(), tSock->ClientIP2(), tSock->ClientIP1(), username.c_str() );
		Console.Log( temp , "server.log" );
		messageLoop << temp;

		actbTemp->wFlags.set( AB_FLAGS_ONLINE, true );

		//Add temp-clientversion info to account here, to be used for second login
		if( actbTemp->dwLastClientVer == 0 || tSock->ClientType() != CV_DEFAULT )
		{
			actbTemp->dwLastClientVer = tSock->ClientVersion();
			actbTemp->dwLastClientType = tSock->ClientType();
			actbTemp->dwLastClientVerShort = tSock->ClientVerShort();
		}

		// change for IP4Address
		auto address = cwmWorldState->ServerData()->matchIP( tSock->ipaddress );
		
		CPGameServerList toSend( 1 );
		toSend.addEntry( cwmWorldState->ServerData()->ServerName(), address.ipaddr( true ));
		tSock->Send( &toSend );
	}
	// If socket's ClientType is still CV_DEFAULT, it's an old client,
	// since there was no packet before 0x80 where ClientType changed
	if( tSock->ClientType() == CV_DEFAULT )
	{
		tSock->ClientType( CV_T2A );
	}
	//CPEnableClientFeatures ii;
	//tSock->Send( &ii );
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
	userId = oldstrutil::trim( temp );

	// Grab our password
	memcpy( temp, &tSock->Buffer()[31], 30 );
	password = temp;

	unknown = tSock->GetByte( 61 );
	// Done with our buffer, we can clear it out now
}
void CPIFirstLogin::InternalReset( void )
{
	userId.reserve( 30 );
	password.reserve( 30 );
	unknown = 0;
}

const std::string CPIFirstLogin::Name( void )
{
	return userId;
}
const std::string CPIFirstLogin::Pass( void )
{
	return password;
}
UI08 CPIFirstLogin::Unknown( void )
{
	return unknown;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIServerSelect()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet for server selection
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xA0 (Select Server)
//|					Size: 3 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] server # chosen
//|						0x80 Packet
//o------------------------------------------------------------------------------------------------o
void CPIServerSelect::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[RECV]Packet   : CPIServerSelect 0xA0 --> Length: 3" << TimeStamp() << std::endl;
	}
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
	// Sept 19, 2002
	// Someone said that there was an issue with False logins that request server 0. Default to server 1.
	SI16 temp = tSock->GetWord( 1 );
	if( temp == 0 )
	{
		return 1;
	}
	else
	{
		return temp;
	}
}

bool CPIServerSelect::Handle( void )
{
	auto ip = cwmWorldState->ServerData()->matchIP( tSock->ipaddress );

	auto name = cwmWorldState->ServerData()->ServerName();
	auto port = cwmWorldState->ServerData()->ServerPort();
	CPRelay toSend( ip.ipaddr( false ), port );
	tSock->Send( &toSend );
	// Mark packet as sent. No we need to change how we network.
	return true;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPISecondLogin()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet for client second login - game server
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x91 (Game Server Login)
//|					Size: 65 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[4] key used
//|						BYTE[30] sid
//|						BYTE[30] password
//o------------------------------------------------------------------------------------------------o
void CPISecondLogin::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[RECV]Packet   : CPISecondLogin 0x91 --> Length: 65" << TimeStamp() << std::endl;
	}

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
	keyUsed = 0;
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
	sid = oldstrutil::trim( temp );

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
	CAccountBlock_st& actbTemp = Accounts->GetAccountByName( Name() );
	if( actbTemp.wAccountIndex != AB_INVALID_ID )
	{
		tSock->SetAccount( actbTemp );
	}

	// Add socket version info from first login, stored in account, to new socket!
	tSock->ClientVersion( actbTemp.dwLastClientVer );
	tSock->ClientType( static_cast<ClientTypes>( actbTemp.dwLastClientType ));
	tSock->ClientVerShort( static_cast<ClientVersions>( actbTemp.dwLastClientVerShort ));

	std::string pass0, pass1, pass2;
	pass0 = Pass();
	pSplit( pass0, pass1, pass2 );

	if( tSock->AcctNo() != AB_INVALID_ID )
	{
		// Check if player's bantime is up, and if so, unban player
		CheckBanTimer( actbTemp );

		if( actbTemp.wFlags.test( AB_FLAGS_BANNED ))
		{
			t = LDR_ACCOUNTDISABLED;
		}
		else if( pass1 != actbTemp.sPassword )
		{
			t = LDR_BADPASSWORD;
		}
	}
	else
	{
		t = LDR_UNKNOWNUSER;
	}

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
		// If first login timestamp has not been set, set it here
		if( actbTemp.wFirstLogin == 0 )
		{
			actbTemp.wFirstLogin = GetMinutesSinceEpoch();
		}

		//Send supported client features before character-list stuff
		CPEnableClientFeatures ii( tSock );
		tSock->Send( &ii );

		UI08 charCount = 0;
		for( UI08 i = 0; i < 7; ++i )
		{
			if( actbTemp.dwCharacters[i] != INVALIDSERIAL )
			{
				++charCount;
			}
		}
		CServerData *sd		= cwmWorldState->ServerData();
		size_t serverCount	= sd->NumServerLocations();
		CPCharAndStartLoc toSend( actbTemp, charCount, static_cast<UI08>( serverCount ), tSock );
		for( size_t j = 0; j < serverCount; ++j )
		{
			if( tSock->ClientType() >= CV_HS2D && tSock->ClientVersionSub() >= 13 )
			{
				toSend.NewAddStartLocation( sd->ServerLocation( j ), static_cast<UI08>( j ));
			}
			else
			{
				toSend.AddStartLocation( sd->ServerLocation( j ), static_cast<UI08>( j ));
			}
		}
		tSock->Send( &toSend );
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPINewClientVersion()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet for client login/seed, with initial client info
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xEF (KR/2D Client Login/Seed)
//|					Size: 21 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[4] seed, usually the client local ip
//|						BYTE[4] client major version
//|						BYTE[4] client minor version
//|						BYTE[4] client revision version
//|						BYTE[4] client prototype version
//|
//|					Notes
//|						Normally older client send a 4 byte seed (local ip).
//|						Newer clients 2.48.0.3+ (KR) and 6.0.5.0+ (2D) are sending this packet.
//o------------------------------------------------------------------------------------------------o
void CPINewClientVersion::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[RECV]Packet   : CPINewClientVersion 0xEF --> Length: " << std::dec << tSock->GetWord( 1 ) << TimeStamp() << std::endl;
	}
	outStream << "Version        : " << tSock->ClientVersion() << std::endl;
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}
void CPINewClientVersion::InternalReset( void )
{
	//	len = 0;
	seed = 0;
	majorVersion = 0;
	minorVersion = 0;
	clientRevision = 0;
	clientPrototype = 0;
}
CPINewClientVersion::CPINewClientVersion()
{
	InternalReset();
}
CPINewClientVersion::CPINewClientVersion( CSocket *s ) : CPInputBuffer( s )
{
	InternalReset();
	Receive();
	//	Handle();
}
void CPINewClientVersion::Receive( void )
{
	if( tSock->GetByte( 0 ) == 0xC0 )
	{
		tSock->ClientType( CV_T2A );
	}
	else if( tSock->GetByte( 0 ) == 0xEF )
	{
		tSock->Receive( 21, false );
		seed		= tSock->GetDWord( 1 );
		majorVersion = tSock->GetDWord( 5 );
		minorVersion = tSock->GetDWord( 9 );
		clientRevision = tSock->GetDWord( 13 );
		clientPrototype = tSock->GetDWord( 17 );
		tSock->ClientVersion( majorVersion, minorVersion, clientRevision, clientPrototype );

		std::string verString = oldstrutil::number( majorVersion ) + std::string( "." ) + 
			oldstrutil::number( minorVersion ) + std::string( ". ") + oldstrutil::number( clientRevision ) +
			std::string( "." ) + oldstrutil::number( clientPrototype );
		Console << verString << myendl;

		// Set client-version based on information received so far. We need this to be able to send the correct info during login
		// Needs to be refined in second client-version pass (CPIClientVersion)
		if( tSock->ClientVersion() <= 100666881 ) // If under or equal to 6.0.14.1, which in reality means between 6.0.5.0 and 6.0.14.1
		{
			tSock->ClientType( CV_KR2D );
		}
		else if( tSock->ClientVersion() >= 10066881 )
		{
			if( tSock->ClientVersion() >= 1000000000 ) // 1124079360 is 4.0.23.1?
			{
				if( tSock->ClientVersion() == 1110912768 )
				{
					// Kingdom Reborn 3D client, build 2.53.0.2
					tSock->ClientType( CV_KR3D );
					tSock->ClientVerShort( CVS_25302 );
				}
				else
				{
					//UO Enhanced client 4.0.23.1 and above
					//should use same version numbering scheme as classic client internally
					tSock->ClientType( CV_HS3D );
					if( clientRevision <= 15 )
					{
						tSock->ClientVerShort( CVS_70151 );
					}
					else if( clientRevision < 24 )
					{
						tSock->ClientVerShort( CVS_70160 );
					}
					else
					{
						tSock->ClientVerShort( CVS_70240 );
					}
				}
			}
			else if( tSock->ClientVersion() == 100666882 ) // 6.0.14.2, but technically first SA2D client
			{
				tSock->ClientType( CV_SA2D );
			}
			else if( tSock->ClientVersion() <= 117442562 && clientRevision < 9 )
			{
				tSock->ClientType( CV_SA2D );
			}
			else if( tSock->ClientVersion() >= 117440814 && clientRevision >= 9 )
			{
				tSock->ClientType( CV_HS2D );
				// Set temporary client-versions to be used by client-support option during login
				if( clientRevision <= 15 )
				{
					tSock->ClientVerShort( CVS_70151 );
				}
				else if( clientRevision < 24 )
				{
					tSock->ClientVerShort( CVS_70160 );
				}
				else if( clientRevision < 30 )
				{
					tSock->ClientVerShort( CVS_70240 );
				}
				else if( clientRevision < 33 )
				{
					tSock->ClientVerShort( CVS_70300 );
				}
				else if( clientRevision < 45 )
				{
					tSock->ClientVerShort( CVS_70331 );
				}
				else if( clientRevision < 46 )
				{
					tSock->ClientVerShort( CVS_704565 );
				}
				else if( clientRevision < 61 )
				{
					tSock->ClientType( CV_TOL2D );
					tSock->ClientVerShort( CVS_705527 );
				}
				else
				{
					tSock->ClientVerShort( CVS_70610 );
				}
			}
			// ??? - 7.0.61.0 - Endless Journey
			// ??? - 7.0.61.0 - Endless Journey
			// 117462287 - 7.0.85.15
		}
		tSock->ReceivedVersion( true );
	}
}

bool CPINewClientVersion::Handle( void )
{
	return true;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIClientVersion()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet for client version string
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xBD (Client Version Message)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] len
//|						If (client-version of packet)
//|							BYTE[len-3] string stating the client version (0 terminated)  (like: “1.26.4”)
//|
//|					Notes
//|						Client + Server packet
//|						Client version: client sends its version string (e.g “3.0.8j”)
//|						Server version: 0xbd 0x0 0x3 (client replies with client version of this packet)
//|						Clients sends a client version of this packet ONCE at login (without server request.)
//o------------------------------------------------------------------------------------------------o
void CPIClientVersion::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[RECV]Packet   : CPIClientVersion 0xBD --> Length: " << std::dec << tSock->GetWord( 1 ) << TimeStamp() << std::endl;
	}
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

//o------------------------------------------------------------------------------------------------o
//|	Function	-	UI08 ShiftValue( UI08 toShift, UI08 base, UI08 upper, bool extra)
//|	Date		-	21st November, 2001
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	If the value is between base and upper (inclusive)
//|					it shifts the data down by base (if extra) or base - 1 (if not extra)
//o------------------------------------------------------------------------------------------------o
UI08 ShiftValue( UI08 toShift, UI08 base, UI08 upper, bool extra )
{
	if( extra )
	{
		if( toShift >= base && toShift <= upper )
		{
			toShift -= base;
		}
	}
	else
	{
		if( toShift >= base && toShift <= upper )
		{
			toShift = toShift - base + 1;
		}
	}
	return toShift;
}
char *CPIClientVersion::Offset( void )
{
	return ( char * ) & ( tSock->Buffer()[3] );
}
bool CPIClientVersion::Handle( void )
{
	char *verString	= Offset();
	verString[len]	= 0;

	// Only need this bit for clients prior to 6.0.5.0 (6.0.0.0 to 6.0.4.x are classified as CV_T2A up until this point)
	// Version already received in packet 0xEF for 6.0.5.0+
	if( tSock->ClientType() < CV_KR2D )
	{
		UI08 major, minor, sub, letter;
		std::string s( verString );
		std::string us = std::string( s );
		auto dsecs = oldstrutil::sections( s, "." );
		UI08 secCount = static_cast<UI08>( dsecs.size() - 1 );

		std::istringstream ss( s );
		char period;
		ss >> major >> period;
		ss >> minor >> period;
		if( secCount == 3 )
		{
			ss >> sub >> period;
			ss >> letter;
		}
		else
		{
			UI08 temp;
			ss >> sub;
			ss >> temp;
			if( isalpha( temp ))
			{
				letter = temp;
			}
			else
			{
				std::string tempSubString;
				std::stringstream tempHackSS;
				tempHackSS << sub;
				tempHackSS >> tempSubString;
				tempHackSS.str("");
				tempHackSS.clear();
				tempHackSS << ( tempSubString += temp );
				SI32 tempSubInt;
				tempHackSS >> tempSubInt;
				sub = tempSubInt;
				ss >> letter;
			}
		}

		major	= ShiftValue( major,  '0', '9', true );
		minor	= ShiftValue( minor,  '0', '9', true );
		sub		= ShiftValue( sub,    '0', '9', true );
		if( secCount == 3 )
		{
			letter	= ShiftValue( letter, '0', '9', true );
		}
		else
		{
			letter	= ShiftValue( letter, 'a', 'z', false );
			letter	= ShiftValue( letter, 'A', 'Z', false );
		}

		tSock->ClientVersion( major, minor, sub, letter );
		Console << verString << myendl;
	}

	if( strstr( verString, "Dawn" ))
	{
		tSock->ClientType( CV_UO3D );
	}
	else if( strstr( verString, "Krrios" ))
	{
		tSock->ClientType( CV_KRRIOS );
	}
	else
	{
		SetClientVersionShortAndType( tSock, verString );
	}
	tSock->ReceivedVersion( true );
	return true;
}

void CPIClientVersion::SetClientVersionShortAndType( CSocket *tSock, char *verString )
{
	UI08 CliVerMajor = tSock->ClientVersionMajor();
	//UI08 CliVerMinor = tSock->ClientVersionMinor(); //uncomment if needed
	UI08 CliVerSub = tSock->ClientVersionSub();
	UI08 CliVerLetter = tSock->ClientVersionLetter();
	UI32 CliVer = tSock->ClientVersion();

	if( CliVer < 100663559 )
	{
		if( CliVerMajor == 4 )
		{
			tSock->ClientType( CV_T2A );
			if( tSock->ClientVersionSub() < 7 )
			{
				tSock->ClientVerShort( CVS_400 );
			}
			else if( CliVerSub < 11 || ( CliVerSub == 11 && CliVerLetter < 2 ))
			{
				tSock->ClientVerShort( CVS_407a );
			}
			else if( CliVerSub == 11 && CliVerLetter >= 2 )  // 4.0.11f really belongs in the CV_ML type though...
			{
				tSock->ClientVerShort( CVS_4011c );
			}
			if( !cwmWorldState->ServerData()->ClientSupport4000() )
			{
				tSock->ForceOffline( true );
				tSock->IdleTimeout( cwmWorldState->GetUICurrentTime() + 200 );
				tSock->SysMessage( 1796, verString ); // Your current client-version (%s) is not supported by this shard. You will be disconnected.
				Console << "Login denied - unsupported client (4.0.0.0 - 4.0.11f). See UOX.INI..." << myendl;
			}
		}
		else if( CliVerMajor == 5 )
		{
			tSock->ClientType( CV_ML );
			if( tSock->ClientVersionSub() < 2 )
			{
				tSock->ClientVerShort( CVS_500a );
			}
			else if( CliVerSub < 8 || ( CliVerSub == 8 && CliVerLetter < 2 ))
			{
				tSock->ClientVerShort( CVS_502a );
			}
			else if( CliVerSub > 8 || ( CliVerSub == 8 && CliVerLetter >= 2 ))
			{
				tSock->ClientVerShort( CVS_5082 );
			}
			if( !cwmWorldState->ServerData()->ClientSupport5000() )
			{
				tSock->ForceOffline( true );
				tSock->IdleTimeout( cwmWorldState->GetUICurrentTime() + 200 );
				tSock->SysMessage( 1796, verString ); // Your current client-version (%s) is not supported by this shard. You will be disconnected.
				Console << "Login denied - unsupported client (5.0.0.0 - 5.0.9.1). See UOX.INI..." << myendl;
			}
		}
	}
	else if( CliVer >= 100663559 && CliVer <= 100666881 )
	{
		tSock->ClientType( CV_KR2D );
		if( CliVerSub >= 5 && CliVerSub <= 14 )
		{
			tSock->ClientVerShort( CVS_6050 );
		}
		else
		{
			if( CliVerSub < 1 || ( CliVerSub == 1 && CliVerLetter < 7 ))
			{
				tSock->ClientVerShort( CVS_6000 );
			}
			else if( CliVerSub < 5 )
			{
				tSock->ClientVerShort( CVS_6017 );
			}
			if( !cwmWorldState->ServerData()->ClientSupport6000() )
			{
				tSock->ForceOffline( true );
				tSock->IdleTimeout( cwmWorldState->GetUICurrentTime() + 200 );
				tSock->SysMessage( 1796, verString ); // Your current client-version (%s) is not supported by this shard. You will be disconnected.
				Console << "Login denied - unsupported client (6.0.0.0 - 6.0.4.0). See UOX.INI..." << myendl;
			}
		}
	}
	else if( CliVer > 100666881 )
	{
		if( CliVer <= 117442562 && tSock->ClientType() == CV_SA2D )
		{
			tSock->ClientType( CV_SA2D );
			if( CliVerMajor == 6 && CliVerSub == 14 && CliVerLetter >= 2 )
			{
				tSock->ClientVerShort( CVS_60142 );
			}
			else if( CliVerMajor == 7 && CliVerSub < 9 )
			{
				tSock->ClientVerShort( CVS_7000 );
			}
		}
		else if( CliVer >= 117440814 && tSock->ClientType() == CV_HS2D )
		{
			tSock->ClientType( CV_HS2D );
			if( CliVerSub < 13 )
			{
				tSock->ClientVerShort( CVS_7090 );
			}
			else if( CliVerSub < 14 )
			{
				tSock->ClientVerShort( CVS_70130 );
			}
			else if( CliVerSub < 16 )
			{
				tSock->ClientVerShort( CVS_70151 );
			}
			else if( CliVerSub < 24 )
			{
				tSock->ClientVerShort( CVS_70160 );
			}
			else if( CliVerSub < 30 )
			{
				tSock->ClientVerShort( CVS_70240 );
			}
			else if( CliVerSub < 33 )
			{
				tSock->ClientVerShort( CVS_70300 );
			}
			else if( CliVerSub < 45 )
			{
				tSock->ClientVerShort( CVS_70331 );
			}
			else if( CliVerSub < 46)
			{
				tSock->ClientVerShort( CVS_704565 );
			}
			else if( CliVerSub < 61 )
			{
				tSock->ClientType( CV_TOL2D );
				tSock->ClientVerShort( CVS_704565 );
			}
			else
			{
				tSock->ClientVerShort( CVS_70610 );
			}
		}
		else if( CliVer >= 1000000000 && tSock->ClientType() == CV_HS3D ) // 1124079360 is 4.0.23.1?
		{
			//UO Enhanced client 4.0.23.1 and above
			//should use same version numbering scheme as classic client internally
			tSock->ClientType( CV_HS3D );
			if( CliVerSub < 13 )
			{
				tSock->ClientVerShort( CVS_7090 );
			}
			else if( CliVerSub <= 15 && CliVerLetter == 0 )
			{
				tSock->ClientVerShort( CVS_70130 );
			}
			else if( CliVerSub == 15 && CliVerLetter == 1 )
			{
				tSock->ClientVerShort( CVS_70151 );
			}
			else if( CliVerSub >= 16 && CliVerSub < 24 )
			{
				tSock->ClientVerShort( CVS_70160 );
			}
			else if( CliVerSub >= 24 )
			{
				tSock->ClientVerShort( CVS_70240 );
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIUpdateRangeChange()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet for client view range
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xC8 (Client View Range)
//|					Size: 2 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE range
//|
//|					Notes
//|						Client + Server packet
//|						Range: how far away client wants to see (=get send) items/npcs
//|						Update: since client 3.0.8o it actually got activated.(in a useful way)
//|						When increase/decrease macro send, client sends a 0xc8.
//|						If and only if server *relays* the packet (sending back the same data)
//|						range stuff gets activated client side.
//|						"Greying" has no packet, purely client internal.
//|						Minimal value:5, maximal: 18
//o------------------------------------------------------------------------------------------------o
void CPIUpdateRangeChange::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[RECV]Packet   : CPIUpdateRangeChange 0xC8 --> Length: 2 " << std::endl;
	}
	outStream << "Range			 : " << static_cast<SI32>( tSock->GetByte( 1 )) << std::endl;
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

	switch( tSock->ClientType() )
	{
		case CV_DEFAULT:
		case CV_T2A:
		case CV_UO3D:
		case CV_ML:
		case CV_KR2D:
		case CV_KR3D:
		case CV_SA2D:
		case CV_SA3D:
		case CV_HS2D:
		case CV_HS3D:
			tSock->Range( tSock->GetByte( 1 ));
			break;
		default:
			tSock->Range( tSock->GetByte( 1 ));
			break;
	}
	tSock->Send( tSock->Buffer(), 2 );	// we want to echo it back to the client
#if defined( _MSC_VER )
#pragma note( "Flush location" )
#endif
	tSock->FlushBuffer();
	tSock->CurrcharObj()->Teleport(); // TODO - Could this be an Update() instead of Teleport?
	return true;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPILogoutStatus()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet for logout requests
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xD1 (Logout Status)
//|					Size: 2 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE extra
//|
//|					Notes
//|						Client + Server packet
//|						Client will send this packet without 0x01 Byte when the server sends FLAG & 0x02 in the 0xA9 Packet during logon.
//|						Server responds with same packet, plus the 0x01 Byte, allowing client to finish logging out.
//o------------------------------------------------------------------------------------------------o
void CPILogoutStatus::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[RECV]Packet   : CPILogoutStatus 0xD1 --> Length: 2 " << std::endl;
	}
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}
void CPILogoutStatus::InternalReset( void )
{
}
CPILogoutStatus::CPILogoutStatus()
{
	InternalReset();
}
CPILogoutStatus::CPILogoutStatus( CSocket *s ) : CPInputBuffer( s )
{
	InternalReset();
	Receive();
}
void CPILogoutStatus::Receive( void )
{
	tSock->Receive( 2, false );
}
bool CPILogoutStatus::Handle( void )
{
	// Byte 0 == 0xD1
	// Byte 1 == 0x01 in response to logout request sent by server

	// Construct the response, and send it!
	CPLogoutResponse logoutResponse( 0x01 );
	tSock->Send( &logoutResponse );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPITips()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with client request for tips/notice windows
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xA7 (Request Tips/Notice)
//|					Size: 4 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] last tip #
//|						BYTE flag
//|							0x00 - tips window
//|							0x01 - notice window
//o------------------------------------------------------------------------------------------------o
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
auto CPITips::Handle() -> bool
{
	auto tips = FileLookup->FindEntry( "TIPS", misc_def );
	if( tips )
	{
		auto i = static_cast<UI16>( tSock->GetWord( 1 ) + 1 );
		if( i == 0 )
		{
			i = 1;
		}

		SI32 x = i;
		std::string tag, data, sect;

		for( tag = tips->First(); !tips->AtEnd(); tag = tips->Next() )
		{
			if( !tag.empty() && oldstrutil::upper( tag ) == "TIP" )
			{
				--x;
			}
			if( x <= 0 )
			{
				break;
			}
		}
		if( x > 0 )
		{
			tag = tips->Prev();
		}
		data = tips->GrabData();

		sect = "TIP " + oldstrutil::trim( oldstrutil::removeTrailing( data, "//" ));
		tips = FileLookup->FindEntry( sect, misc_def );
		if( tips )
		{
			std::string tipData = "";
			for( tag = tips->First(); !tips->AtEnd(); tag = tips->Next() )
			{
				tipData += tips->GrabData() + " ";
			}

			CPUpdScroll toSend( 0, static_cast<UI08>( i ));
			toSend.AddString( tipData.c_str() );
			toSend.Finalize();
			tSock->Send( &toSend );
		}
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIRename()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with request to rename character
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x75 (Rename Character)
//|					Size: 35 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[4] id
//|						BYTE[30] new name
//o------------------------------------------------------------------------------------------------o
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

	CChar *c = CalcCharObjFromSer( serial );
	if( ValidateObject( c ))
	{
		c->SetName(( char * )&tSock->Buffer()[5] );
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIKeepAlive()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with ping message request
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x73 (Ping Message)
//|					Size: 2 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE seq
//o------------------------------------------------------------------------------------------------o
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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIStatusRequest()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet requesting player status
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x34 (Get Player Status)
//|					Size: 10 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[4] pattern (0xedededed)
//|						BYTE getType
//|							0x00 - GodClient command
//|							0x04 - Basic Stats (Packet 0x11 Response)
//|							0x05 = Request Skills (Packet 0x3A Response)
//|						BYTE[4] playerId
//o------------------------------------------------------------------------------------------------o
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
	playerId	= tSock->GetDWord( 6 );
}

void CPIStatusRequest::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[RECV]Packet   : CPIStatusRequest 0x34 --> Length: 10" << TimeStamp() << std::endl;
	}
	outStream << "Pattern        : " << pattern << std::endl;
	outStream << "Request Type   : " << static_cast<SI32>( getType ) << std::endl;
	outStream << "PlayerID       : " << std::hex << "0x" << playerId << std::endl;
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}

bool CPIStatusRequest::Handle( void )
{
	if( getType == 4 )
	{
		if( playerId >= BASEITEMSERIAL )
		{
			tSock->StatWindow( CalcItemObjFromSer( playerId )); // Item
		}
		else
		{
			tSock->StatWindow( CalcCharObjFromSer( playerId )); // Character
		}
	}
	if( getType == 5 )
	{
		// Check if onSkillGump event exists
		CChar *myChar	= tSock->CurrcharObj();
		std::vector<UI16> scriptTriggers = myChar->GetScriptTriggers();
		for( auto i : scriptTriggers )
		{
			cScript *toExecute = JSMapping->GetScript( i );
			if( toExecute != nullptr )
			{
				UI08 retStatus = toExecute->OnSkillGump( myChar );
				if( retStatus == 0 ) // if it exists and we don't want hard code, return
					return true;
			}
		}

		CPSkillsValues toSend;
		toSend.NumSkills( ALLSKILLS );
		toSend.SetCharacter( *( tSock->CurrcharObj() ));
		tSock->Send( &toSend );
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPISpy()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with client hardware details
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xA4 (Spy on Client)
//|					Size: 149 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[148] Unknown (previously, this has had info such as your graphics card
//|							name, free HD space, number of processors, etc.)
//o------------------------------------------------------------------------------------------------o
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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIKRSeed()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet by KR client with client signal, server responds with
//|					encryption request
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xFF (KR Client Signal/Seed)
//|					Size: 4 bytes
//|
//|					Packet Build
//|						Byte cmd (0xff)
//|						Byte 0xff
//|						Byte 0xff
//|						Byte 0xff
//o------------------------------------------------------------------------------------------------o
CPIKRSeed::CPIKRSeed()
{
}
CPIKRSeed::CPIKRSeed( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}
void CPIKRSeed::Receive( void )
{
	// This would need to be a server option. No way to tell which client is sending this packet
	CPKREncryptionRequest encryptionRequest( tSock );
	tSock->Send( &encryptionRequest );
}
bool CPIKRSeed::Handle( void )
{
	return true;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIKREncryptionVerification()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet by KR client response to encryption request
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xE4 (KR Encryption Verification/Response)
//|					Size: 23 bytes
//|
//|					Packet Build
//|						Byte ID (E4)
//|						BYTE[2] Size
//|						dword length A
//|						byte[lengthA] A
//o------------------------------------------------------------------------------------------------o
CPIKREncryptionVerification::CPIKREncryptionVerification()
{
}
CPIKREncryptionVerification::CPIKREncryptionVerification( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}
void CPIKREncryptionVerification::Receive( void )
{
	tSock->Receive( 23, false );
}
bool CPIKREncryptionVerification::Handle( void )
{
	return true;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIGodModeToggle()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with request to enable God Client features
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x04 (Request God Mode)
//|					Size: 2 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE mode (0=off, 1=on)
//o------------------------------------------------------------------------------------------------o
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
		tSock->SysMessage( 1641 ); // You don't have the privs needed to run the Godclient on this server!
		Console << "Godclient detected - Account[" << ourChar->GetAccount().wAccountIndex << "] Username[" << ourChar->GetAccount().sUsername << ". Client disconnected!" << myendl;
		Network->Disconnect( tSock );
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIDblClick()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with double-click request
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x06 (Double Click)
//|					Size: 5 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[4] ID of double clicked object
//|
//|					NOTE: bool CPIDblClick::Handle() is implemented in cplayeraction.cpp
//o------------------------------------------------------------------------------------------------o
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

	objectId = tSock->GetDWord( 1 );
}

void CPIDblClick::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[RECV]Packet   : CPIDblClick 0x06 --> Length: 5" << TimeStamp() << std::endl;
	}
	outStream << "ClickedID      : " << std::hex << "0x" << objectId << std::endl;
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPISingleClick()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with single-click (and All-Names macro) request
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x09 (Single Click)
//|					Size: 5 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[4] ID of single clicked object
//|
//|					NOTE: bool CPISingleClick::Handle() is implemented in cplayeraction.cpp
//o------------------------------------------------------------------------------------------------o
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

	objectId = tSock->GetDWord( 1 );
}

void CPISingleClick::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[RECV]Packet   : CPISingleClick 0x09 --> Length: 5" << TimeStamp() << std::endl;
	}
	outStream << "ClickedID      : " << std::hex << "0x" << objectId << std::endl;
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIMoveRequest()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with request for movement
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x02 (Move Request)
//|					Size: 7 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE direction
//|						BYTE sequence number
//|						BYTE[4] fastwalk prevention key
//|
//|					Note: sequence number starts at 0, after a reset. However, if 255 is reached,
//|					the next seq # is 1, not 0.
//|					Fastwalk prevention notes: each 0x02 pops the top element from fastwalk key stack.
//|						(0xbf sub1 init. fastwalk stack, 0xbf sub2 pushes an element to stack)
//|						If stack is empty key value is 0 (never set keys to 0 in 0xbf sub 1/2)
//|						Because client sometimes sends bursts of 0x02’s DON’T check for a certain top stack value.
//|						The only safe way to detect fastwalk:
//|							push a key after EACH x021, 0x22, (=send 0xbf sub 2) check in 0x02 for stack emptyness.
//|							If empty -> fastwalk alert.
//|					Note that actual key values are irrelevant. (just don’t use 0)
//|					Of course without perfect 0x02/0x21/0x22 synch (serverside) it’s useless to use fastwalk detection.
//|					Last but not least: fastwalk detection adds 9 bytes per step and player!
//o------------------------------------------------------------------------------------------------o
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
	Movement->Walking( tSock, ourChar, tSock->GetByte( 1 ), tSock->GetByte( 2 ));
	ourChar->BreakConcentration( tSock );
	return true;
}

void CPIMoveRequest::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[RECV]Packet   : CPIMoveRequest 0x02 --> Length: 7" << TimeStamp() << std::endl;
	}
	outStream << "Direction      : " << tSock->GetByte( 1 ) << std::endl;
	outStream << "Sequence Number: " << tSock->GetByte( 2 ) << std::endl;
	outStream << "FW Prevent Key : " << tSock->GetDWord( 3 ) << std::endl;
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIResyncReq()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with character move acknowledgement/resync request
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x22 (Character Move Ack/Resync Request)
//|					Size: 3 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE sequence (matches sent sequence)
//|						BYTE (0x00)
//o------------------------------------------------------------------------------------------------o
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
	CChar *ourChar = tSock->CurrcharObj();
	ourChar->Dirty( UT_LOCATION );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIResMenuChoice()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with resurrection menu choice (old clients)
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x2C (Resurrection Menu Choice)
//|					Size: 2 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE action (2=ghost, 1=resurrect, 0=from server)
//|
//|					Note: Client and Server Message
//|					Note: Resurrection menu has been removed from UO [when?]
//o------------------------------------------------------------------------------------------------o
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
	{
		tSock->SysMessage( 766 ); // You are now a ghost.
	}
	if( cmd == 0x01 )
	{
		tSock->SysMessage( 767 ); // The connection between your spirit and the world is too weak.
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIAttack()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with attack request
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x05 (Attack Request)
//|					Size: 5 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[4] ID to be attacked
//|
//|					Note: Client Message
//o------------------------------------------------------------------------------------------------o
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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPITargetCursor()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with targeting cursor commands
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x6C (Targeting Cursor Commandst)
//|					Size: 19 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE type
//|							0x00 = Select Object
//|							0x01 = Select X, Y, Z
//|						BYTE[4] cursorID
//|						BYTE Cursor Type
//|						Always 0 now
//|						The following are always sent but are only valid if sent by client
//|						BYTE[4] Clicked On ID
//|						BYTE[2] click xLoc
//|						BYTE[2] click yLoc
//|						BYTE unknown (0x00)
//|						BYTE click zLoc
//|						BYTE[2] model # (if a static tile, 0 if a map/landscape tile)
//|
//|					NOTE: The model # shouldn’t be trusted.
//|					NOTE: bool CPITargetCursor::Handle() in targeting.cpp
//o------------------------------------------------------------------------------------------------o
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
void CPITargetCursor::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[RECV]Packet   : CPITargetCursor 0x6C --> Length: 19" << TimeStamp() << std::endl;
	}
	outStream << "Type         : " << static_cast<UI16>( tSock->GetByte( 1 )) << std::endl;
	outStream << "CursorID     : " << tSock->GetDWord( 2 ) << std::endl;
	outStream << "Cursor Type  : " << static_cast<UI16>( tSock->GetByte( 6 )) << std::endl;
	outStream << "Target ID    : " << "0x" << std::hex << tSock->GetDWord( 7 ) << std::endl;
	outStream << "Target X     : " << std::dec << tSock->GetWord( 11 ) << std::endl;
	outStream << "Target Y     : " << tSock->GetWord( 13 ) << std::endl;
	outStream << "Unknown      : " << static_cast<UI16>( tSock->GetByte( 15 )) << std::endl;
	outStream << "Target Z     : " << static_cast<SI16>( tSock->GetByte( 16 )) << std::endl;
	outStream << "Model #      : " << "0x" << std::hex << tSock->GetWord( 17 ) << std::endl;
	outStream << "  Raw dump     :" << std::dec << std::endl;
	CPInputBuffer::Log( outStream, false );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIEquipItem()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with targeting cursor commands
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x13 (Drop->Wear Item)
//|					Size: 10 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[4] itemid
//|						BYTE layer (see layer list at top)
//|						BYTE[4] playerId
//|
//|					NOTE: The layer byte should not be trusted.
//|					NOTE: bool CPIEquipItem::Handle() implemented in cplayeraction.cpp
//o------------------------------------------------------------------------------------------------o
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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIGetItem()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with request to pick up item(s)
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x07 (Pick Up Item(s))
//|					Size: 7 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[4] item id
//|						BYTE[2] # of items in stack
//|
//|					NOTE: Client Message
//|					NOTE: The layer byte should not be trusted.
//|
//|					NOTE: bool CPIGetItem::Handle() implemented in cplayeraction.cpp
//o------------------------------------------------------------------------------------------------o
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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIDropItem()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with request to drop item(s)
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x08 (Drop Item(s))
//|					Size: 14 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[4] item id
//|						BYTE[2] X Location
//|						BYTE[2] Y Location
//|						BYTE Z Location
//|						BYTE[4] Move Into Container ID (FF FF FF FF if normal world)
//|
//|					NOTE: Client Message
//|					NOTE: 3D clients sometimes sends 2 of them (bursts) for ONE drop action.
//|						The last one having –1’s in X/Y locs. Be very careful how to handle this
//|						odd “bursts” server side, neither always process, nor always skipping is correct.
//|
//|					NOTE: bool CPIDropItem::Handle() implemented in cplayeraction.cpp
//o------------------------------------------------------------------------------------------------o
CPIDropItem::CPIDropItem()
{
}
CPIDropItem::CPIDropItem( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}
void CPIDropItem::Receive( void )
{
	uokrFlag = false;
	if( tSock->ClientVerShort() >= CVS_6017 )
	{
		uokrFlag = true;
	}

	tSock->Receive(( uokrFlag ? 15 : 14 ), false );

	item	= tSock->GetDWord( 1 );
	x		= tSock->GetWord( 5 );
	y		= tSock->GetWord( 7 );
	z		= tSock->GetByte( 9 );
	if( uokrFlag )
	{
		gridLoc = tSock->GetByte( 10 );
		dest = tSock->GetDWord( 11 );
	}
	else
	{
		dest = tSock->GetDWord( 10 );
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIGumpMenuSelect()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with gump menu selection data
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xB1 (Gump Menu Selection)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd													0
//|						BYTE[2] blockSize											1
//|						BYTE[4] id (first Id in 0xb0)								3
//|						BYTE[4] gumpId (second Id in 0xb0)							7
//|						BYTE[4] buttonId (which button perssed ? 0 if closed)		11
//|						BYTE[4] switchcount  (response info for radio buttons and checkboxes, any
//|							switches listed here are switched on)					15
//|						For each switch
//|							BYTE[4] SwitchId
//|						BYTE[4] textcount  (response info for textentries)			19 + switchcount * 4
//|						For each textentry
//|							BYTE[2] textId
//|							BYTE[2] textlength
//|							BYTE[length*2] Unicode text (not nullterminated)
//|
//|					NOTE: bool CPIGumpMenuSelect::Handle() implemented in gumps.cpp
//o------------------------------------------------------------------------------------------------o
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

SERIAL CPIGumpMenuSelect::ButtonId( void ) const
{
	return buttonId;
}
SERIAL CPIGumpMenuSelect::GumpId( void ) const
{
	return gumpId;
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
	return tSock->GetDWord( 19 + 4 * static_cast<size_t>( index ));
}

UI16 CPIGumpMenuSelect::GetTextId( UI08 number ) const
{
	if( number >= textCount )
		return 0xFFFF;

	return tSock->GetWord( textLocationOffsets[number] );
}
UI16 CPIGumpMenuSelect::GetTextLength( UI08 number ) const
{
	if( number >= textCount )
		return 0xFFFF;

	return tSock->GetWord( static_cast<size_t>( textLocationOffsets[number] ) + 2 );
}

std::string CPIGumpMenuSelect::GetTextString( UI08 number ) const
{
	if( number >= textCount )
		return "";

	std::string toReturn = "";
	UI16 offset = textLocationOffsets[number] + 4;
	for( UI16 counter = 0; counter < GetTextLength( number ); ++counter )
	{
		toReturn += tSock->GetByte( static_cast<size_t>( offset ) + static_cast<size_t>( counter ) * 2 + 1 );
	}
	return toReturn;
}
void CPIGumpMenuSelect::BuildTextLocations( void )
{
	if( textCount > 0 )
	{
		size_t i = static_cast<size_t>( textOffset ) + 4;	// first is textOffset + 4, to walk past the number of text strings
		textLocationOffsets.resize( textCount );
		for( size_t j = 0; j < textCount; ++j )
		{
			textLocationOffsets[j] = static_cast<wchar_t>( i );
			i += 2;	// skip the text ID
			UI16 textLen = tSock->GetWord( i );
			i += ( 2 * static_cast<size_t>( textLen ) + 2 );	// we need to add the + 2 for the text len field
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPITalkRequest()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with talk request
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x03 (Talk Request)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] length
//|						BYTE SpeechType
//|						BYTE[2] Color
//|						BYTE[2] SpeechFont
//|						BYTE[?] msg - Null Terminated (blockSize - 8)
//|
//|					The various types of text is as follows:
//|						0x00 - Normal
//|						0x01 - Broadcast/System
//|						0x02 - Emote
//|						0x06 - System/Lower Corner
//|						0x07 - Message/Corner With Name
//|						0x08 - Whisper
//|						0x09 - Yell
//|						0x0A - Spell
//|						0x0D - Guild Chat
//|						0x0E - Alliance Chat
//|						0x0F - Command Prompts
//|						0xC0 - Encoded Commands
//|
//|					NOTE: bool CPITalkRequest::Handle() is located in speech.cpp
//o------------------------------------------------------------------------------------------------o
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
	return ( char * )&txtSaid[0];
}

bool CPITalkRequest::HandleCommon( void )
{
	CChar *ourChar = tSock->CurrcharObj();
	ourChar->BreakConcentration( tSock );
	ourChar->SetUnicode( false );
	CItem *speechItem = ourChar->GetSpeechItem();

	UI32 j = 0;

	switch( ourChar->GetSpeechMode() )
	{
		case 3: // Player vendor item pricing
		{
			// Set default value
			j = speechItem->GetBuyValue();

			// Grab some references to container speechItem is in, and the root container as well
			auto iCont = static_cast<CItem *>( speechItem->GetCont() );
			auto rootCont = FindRootContainer( speechItem );
			if( !ValidateObject( iCont ) || !ValidateObject( rootCont ))
				return false;

			try
			{
				j = static_cast<UI32>( std::stoul( std::string( Text() ), nullptr, 0 ));
				if( j > 0 )
				{
					// If the price set is higher than 0, allow it
					speechItem->SetVendorPrice( j );
					tSock->SysMessage( 753, j ); // This item's price has been set to %i.
				}
				else
				{
					// Price is 0, so user is trying to mark item as 'Not for Sale'
					// Only books, keyrings and unlocked containers can be marked as 'Not for Sale' in root vendor pack
					// While any item can be marked as such within another container - if that container is for sale
					bool isInRoot = ( iCont == rootCont );
					auto itemId = speechItem->GetId();
					bool canBeNotForSale = true;

					if( isInRoot && (( itemId != 0x1769 && itemId != 0x176a && itemId != 0x176b ) 
						&& speechItem->GetType() != IT_BOOK && speechItem->GetType() != IT_CONTAINER ))
					{
						// In root, not a book, keyring or container. Disallow 'Not for Sale'
						canBeNotForSale = false;
					}
					else if( !isInRoot
						|| ( isInRoot && (( itemId == 0x1769 || itemId == 0x176a || itemId == 0x176b ) 
							|| speechItem->GetType() == IT_BOOK || speechItem->GetType() == IT_CONTAINER )))
					{
						if( speechItem->GetType() == IT_CONTAINER )
						{
							// Iterate through all items in container and ensure that there are no items marked 'not for sale'
							std::vector<CItem *> contItems;
							auto iContList = speechItem->GetContainsList();
							for( const auto &contItem : iContList->collection() )
							{
								if( !ValidateObject( contItem ))
									continue;

								if( contItem->GetVendorPrice() == 0 )
								{
									// If item is a container, check if _that_ container contains any items with items marked not for sale
									if( contItem->GetType() == IT_CONTAINER )
									{
										std::vector<CItem *> subContItems;
										auto iSubContList = contItem->GetContainsList();
										for( const auto &subContItem : iSubContList->collection() )
										{
											if( !ValidateObject( subContItem ))
												continue;

											if( subContItem->GetVendorPrice() == 0 )
											{
												// Found an item inside sub-container marked not for sale
												canBeNotForSale = false;
												break;
											}
										}
									}
									else
									{
										if( contItem->GetVendorPrice() == 0 )
										{
											// Found item inside container marked not for sale
											canBeNotForSale = false;
											break;
										}
									}
								}

								// Break out of outer for loop as well, if necessary
								if( !canBeNotForSale )
								{
									break;
								}
							}
						}
						else if( iCont->GetVendorPrice() == 0 )
						{
							// Not in root, but parent container is not for sale - disallow 'Not for Sale'
							canBeNotForSale = false;
						}
					}

					if( canBeNotForSale )
					{
						// Item marked as 'Not for Sale'
						tSock->SysMessage( 9181, speechItem->GetBuyValue() ); // The item has been marked as 'not for sale'
						speechItem->SetVendorPrice( static_cast<UI32>( 0 ));
					}
					else
					{
						// Item cannot be marked as 'Not for Sale' - default buy price set instead
						tSock->SysMessage( 9178, speechItem->GetBuyValue() ); // The item cannot be marked as 'not for sale'! Default price applied.
						if( speechItem->GetBuyValue() > 0 )
						{
							// Set default buy value as vendor price
							speechItem->SetVendorPrice( speechItem->GetBuyValue() );
						}
						else
						{
							// No default buy value set on item, setting vendor price to 500 instead
							speechItem->SetVendorPrice( static_cast<UI32>( 500 ));
						}
					}
				}
			}
			catch( ... )
			{
				tSock->SysMessage( 9177, speechItem->GetBuyValue() ); // Invalid price entered, using default item value of %i
				if( speechItem->GetBuyValue() > 0 )
				{
					// Set default buy value as vendor price
					speechItem->SetVendorPrice( speechItem->GetBuyValue() );
				}
				else
				{
					// No default buy value set on item, setting vendor price to 500 instead
					speechItem->SetVendorPrice( static_cast<UI32>( 500 ));
				}
			}

			tSock->SysMessage( 755 ); // Enter a description for this item.
			ourChar->SetSpeechMode( 4 );
			break;
		}
		case 4: // Player vendor item describing
		{
			std::string speechItemDesc = Text();
			if( !speechItemDesc.empty() )
			{
				speechItem->SetDesc( Text() );
			}
			else
			{
				speechItem->SetDesc( speechItem->GetName() );
			}
			speechItem->Dirty( UT_UPDATE );
			tSock->SysMessage( 756, speechItem->GetDesc().c_str() ); // This item is now described as %s,
			ourChar->SetSpeechMode( 0 );
			break;
		}
		case 7: // Rune renaming
			speechItem->SetName( Text() );
			tSock->SysMessage( 757, Text() ); // Rune renamed to: Rune to %s.
			ourChar->SetSpeechMode( 0 );
			break;
		case 6: // Name deed
			ourChar->SetName( Text() );
			tSock->SysMessage( 758, Text() ); // Your new name is: %s.
			ourChar->SetSpeechMode( 0 );
			speechItem->Delete();
			ourChar->SetSpeechItem( nullptr );
			break;
		case 5:	// Key renaming
		case 8: // Sign renaming
			speechItem->SetName( Text() );
			tSock->SysMessage( 759, Text() ); // Renamed to: %s.
			ourChar->SetSpeechMode( 0 );
			break;
		case 9: // JavaScript speech
		{
			cScript *myScript	= ourChar->GetSpeechCallback();
			if( myScript != nullptr )
			{
				myScript->OnSpeechInput( ourChar, speechItem, Text() );
			}

			ourChar->SetSpeechMode( 0 );
			break;
		}
		case 1: // GM Page
			UI08 a1, a2, a3, a4;
			if( GMQueue->GotoPos( GMQueue->FindCallNum( ourChar->GetPlayerCallNum() )))
			{
				CHelpRequest *tempPage = nullptr;
				tempPage = GMQueue->Current();
				a1 = ourChar->GetSerial( 1 );
				a2 = ourChar->GetSerial( 2 );
				a3 = ourChar->GetSerial( 3 );
				a4 = ourChar->GetSerial( 4 );

				tempPage->Reason( Text() );
				tempPage->WhoPaging( ourChar->GetSerial() );
				auto temp = oldstrutil::format( "GM Page from %s [%x %x %x %x]: %s", ourChar->GetName().c_str(), a1, a2, a3, a4, tempPage->Reason().c_str() );
				bool x = false;
				{
					for( auto &tmpSock : Network->connClients )
					{
						CChar *tChar = tmpSock->CurrcharObj();
						if( ValidateObject( tChar ))
						{
							if( tChar->IsGMPageable() )
							{
								x = true;
								tmpSock->SysMessage( temp );
							}
						}
					}
				}
				if( x )
				{
					tSock->SysMessage( 363 ); // Your request for assistance has been logged, and will be handled by a GM as soon as possible!
				}
				else
				{
					tSock->SysMessage( 364 ); // Your request for assistance has been logged, and will be handled by a GM as soon as possible!
				}
			}
			ourChar->SetSpeechMode( 0 );
			break;
		case 2: // Counselor Page
			a1 = ourChar->GetSerial( 1 );
			a2 = ourChar->GetSerial( 2 );
			a3 = ourChar->GetSerial( 3 );
			a4 = ourChar->GetSerial( 4 );

			if( CounselorQueue->GotoPos( CounselorQueue->FindCallNum( ourChar->GetPlayerCallNum() )))
			{
				CHelpRequest *tempPageCns;
				tempPageCns = CounselorQueue->Current();
				tempPageCns->Reason( Text() );
				tempPageCns->WhoPaging( ourChar->GetSerial() );
				auto temp = oldstrutil::format( "Counselor Page from %s [%x %x %x %x]: %s", ourChar->GetName().c_str(), a1, a2, a3, a4, tempPageCns->Reason().c_str() );
				bool x = false;
				{
					for( auto &tmpSock : Network->connClients )
					{
						CChar *tChar = tmpSock->CurrcharObj();
						if( ValidateObject( tChar ))
						{
							if( tChar->IsGMPageable() )
							{
								x = true;
								tmpSock->SysMessage( temp );
							}
						}
					}
				}
				if( x )
				{
					tSock->SysMessage( 360 ); // Your request for info has been logged, and will be handled by a Counselor as soon as possible!
				}
				else
				{
					tSock->SysMessage( 361 ); // Your request for info has been logged, and will be handled by a Counselor as soon as possible!
				}
			}
			ourChar->SetSpeechMode( 0 );
			break;
		case 0: // normal speech
		default:
			if( ourChar->GetSquelched() && !ourChar->IsGM() )
			{
				tSock->SysMessage( 760 ); // You have been squelched.
			}
			else
			{
				return false;
			}
			break;
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPITalkRequestAscii()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles ASCII talk request
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x03 (Talk Request)
//|					Size: Variable
//|
//|					See CPITalkRequest() for details
//o------------------------------------------------------------------------------------------------o
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
	typeUsed		= static_cast<SpeechType>( tSock->GetByte( 3 ));
	textColour		= tSock->GetWord( 4 );
	fontUsed		= tSock->GetWord( 6 );

	strcopy( txtSaid, 4096, ( char * ) & ( tSock->Buffer()[8] ));
}

UnicodeTypes FindLanguage( CSocket *s, UI16 offset );
//o------------------------------------------------------------------------------------------------o
//| Function	-	CPITalkRequestUnicode()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles Unicode/Ascii speech requests
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xAD (Unicode/Ascii Speech Request)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd											0
//|						BYTE[2] blockSize									1
//|						BYTE Type											3
//|						BYTE[2] Color										4
//|						BYTE[2] Font										6
//|						BYTE[4] Language (Null Terminated)					8
//|							“enu“ - United States English
//|							“des” - German Swiss
//|							“dea” - German Austria
//|							“deu” - German Germany
//|							... for a complete list see langcode.iff
//|						if (Type & 0xc0)
//|							BYTE[1,5] Number of distinct Trigger words (NUMWORDS).
//|								12 Bit number, Byte #13  = Bit 11…4 of NUMWORDS, Hi-Nibble of Byte #14 (Bit 7…4)  = Bit  0…3 of NUMWORDS
//|							BYTE[1,5] Index to speech.mul
//|								12 Bit number, Low Nibble of Byte #14 (Bits 3…0) = Bits 11..8 of Index, Byte #15  = Bits 7…0 of Index
//|							UNKNOWNS = ( (NUMWORDS div 2) *3 ) + (NUMWORDS % 2)  – 1
//|								div = Integeger division, % = modulo operation, NUMWORDS >= 1
//|								examples: UNKNOWNS(1)=0, UNKNOWNS(2)=2, UNKNOWNS(3)=3, UNKNOWNS(4)=5, UNKNOWNS(5)=6,
//|								UNKNOWNS(6)=8, UNKNOWNS(7)=9, UNKNOWNS(8)=11, UNKNOWNS(9)=12
//|							BYTE[UNKNOWNS]
//|								Idea behind this is getting speech parsing load client side.
//|								Thus this contains data OSI server use for easier parsing.
//|								It’s client side hardcoded and exact details are unkown.
//|							BYTE[?] Ascii Msg – Null Terminated (blockSize – (15+UNKNOWNS) )
//|						else
//|							BYTE[?][2] Unicode Msg - Null Terminated (blockSize - 12)
//|
//|						NOTE1:  For pre 2.0.7 clients Type is always < 0xc0
//|						NOTE2: Uox based emus convert post 2.0.7 data of this packet to pre 2.0.7
//|							data if Type >=0xc0
//|						NOTE3: (different view of it)
//|						If Mode&0xc0 then there are keywords (from speech.mul) present.
//|						Keywords:
//|						The first 12 bits = the number of keywords present. The keywords are
//|							included right after this, each one is 12 bits also.
//|						The keywords are padded to the closest byte. For example, if there are 2
//|							keywords, it will take up 5 bytes. 12bits for the number, and 12 bits for each keyword. 12+12+12=36. Which will be padded 4 bits to 40 bits or 5 bytes.
//o------------------------------------------------------------------------------------------------o
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

	typeUsed		= static_cast<SpeechType>( tSock->GetByte( 3 ));
	textColour		= tSock->GetWord( 4 );
	fontUsed		= tSock->GetWord( 6 );

	if( tSock->Language() == ZERO )
	{
		tSock->Language( FindLanguage( tSock, 8 ));
	}

	CChar *mChar	= tSock->CurrcharObj();
	mChar->SetUnicode( true );

	// Check for command word versions of this packet
	UI08 *buffer		= tSock->Buffer();
	UI16 j				= 0;

	for( j = 8; j <= 10; ++j )
	{
		langCode[j - 8] = tSock->GetByte( j );
	}

	langCode[3] = 0;

	if(( typeUsed & 0xC0 ) == 0xC0 )
	{
		SI32 myoffset		= 13;
		SI32 myj			= 12;
		size_t numTrigWords	= 0;

		// Let's keep track of trigger words we've already added, so we don't add same one twice
		std::unordered_set<uint16_t> trigWords;

		// number of distinct trigger words
		numTrigWords = ( static_cast<size_t>( tSock->GetByte( 12 )) << 4 ) + ( static_cast<size_t>( tSock->GetByte( 13 )) >> 4 );

		size_t byteNum = 13;
		for( size_t tWord = 0; tWord < numTrigWords; )
		{
			uint16_t word = (( tSock->GetByte( byteNum + tWord ) % 0x10 ) << 8 ) + tSock->GetByte( byteNum + tWord + 1 );
			if( trigWords.find( word ) == trigWords.end() )
			{
				tSock->AddTrigWord( word );
				trigWords.insert( word );
			}
			if( tWord + 2 <= numTrigWords )
			{
				word = ( tSock->GetByte( byteNum + tWord + 2 ) << 4 ) + ( tSock->GetByte( byteNum + tWord + 3 ) >> 4 );
				if( trigWords.find( word ) == trigWords.end() )
				{
					tSock->AddTrigWord( word );
					trigWords.insert( word );
				}
			}
			tWord += 2;
			byteNum++;
		}

		myoffset = 15;
		if(( numTrigWords % 2 ) == 1 )	// odd number ?
		{
			myoffset += ( static_cast<SI32>( numTrigWords ) / 2 ) * 3;
		}
		else
		{
			myoffset += (( static_cast<SI32>( numTrigWords ) / 2 ) * 3 ) - 1;
		}

		myj = 12;
		SI32 mysize = blockLen - myoffset;
		for( j = 0; j < mysize; ++j )
		{
			unicodeTxt[j] = buffer[j + myoffset];
		}
		for( j = 0; j < mysize; ++j )
		{	// we would overwrite our buffer, so we need to catch it before we do that.
			buffer[myj++] = 0;
			buffer[myj++] = unicodeTxt[j];
		}
		buffer[myj - 1] = 0;
		blockLen = (( blockLen - myoffset ) * 2 ) + 12;
		PackShort( buffer, 1, blockLen );
		// update our unicode text
		memcpy( unicodeTxt, &( tSock->Buffer()[12] ), blockLen - 12 );
	}
	else		// NOT a weird configuration, it is unicode however!
	{
		// starts at 12
		memcpy( unicodeTxt, &( tSock->Buffer()[12] ), blockLen - 12 );
	}
	strLen = 0;
	for( j = 13; j < blockLen; j += 2 )
	{
		txtSaid[( j - 13 ) / 2] = buffer[j];
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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIAllNames3D()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with request to show names of all nearby characters
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x98 (All Names (3D Client Only))
//|					Size: 7 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] blocksize
//|						BYTE[4] ID
//|						If (server-reply)
//|							BYTE[30] name (0 terminated)
//|
//|						Only 3D clients send this packet. Server and client packet.
//|						Unsure as of when, 2D client added this ability also.
//|						Client asks for name of object with ID x. Server has to reply with ID +
//|							name. Client automatically knows names of items. Hence it only asks only
//|							for NPC/Player names nearby, but shows bars of items plus NPCs.
//|						Client request has 7 bytes, server-reply 37. Triggered by Crtl + Shift.
//o------------------------------------------------------------------------------------------------o
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
	SERIAL objSer = tSock->GetDWord( 3 );
	CBaseObject *toName = nullptr;

	if( objSer >= BASEITEMSERIAL )
	{
		toName = CalcItemObjFromSer( objSer );
	}
	else if( objSer != INVALIDSERIAL )
	{
		toName = CalcCharObjFromSer( objSer );
	}

	if( ValidateObject( toName ))
	{
		CPAllNames3D toSend(( *toName ));
		tSock->Send( &toSend );
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIGumpChoice()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with choice made in a Gump dialog
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x7D (Make a choice in a Gump)
//|					Size: 13 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[4] dialogID (echoed back from 7C packet)
//|						BYTE[2] menuid (echoed back from 7C packet)
//|						BYTE[2] 1-based index of choice
//|						BYTE[2] model # of choice
//|						BYTE[2] color
//|
//|					NOTE: void CPGumpChoice::Handle() implemented in gumps.cpp
//o------------------------------------------------------------------------------------------------o
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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIBuyItem()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with request to buy items from vendor
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x3B (Buy Item(s))
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] blockSize
//|						BYTE[4] vendorID
//|						BYTE flag
//|							0x00 - no items following
//|							0x02 - items following
//|						For each item
//|							BYTE (0x1A)
//|							BYTE[4] itemID (from 3C packet)
//|							BYTE[2] # bought
//|
//|					NOTE: void CPIBuyItem::Handle() implemented in vendor.cpp
//o------------------------------------------------------------------------------------------------o
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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPISellItem()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with request to sell items to vendor
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x9F (Sell Reply)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] blockSize
//|						BYTE[4] shopkeeperID
//|						BYTE[2] itemCount
//|						For each item, a structure containing:
//|							BYTE[4] itemID
//|							BYTE[2] quantity
//|
//|					void CPISellItem::Handle() implemented in vendor.cpp
//o------------------------------------------------------------------------------------------------o
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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIDeleteCharacter()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with request to delete a specific character
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x83 (Delete Character)
//|					Size: 39 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[30] password
//|						BYTE[4] charIndex
//|						BYTE[4]	clientIP
//|
//|					void CPIDeleteCharacter::Handle() implemented in pcmanage.cpp
//o------------------------------------------------------------------------------------------------o
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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPICreateCharacter()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with request to create a new character
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x00 (Create Character)
//|					Packet: 0xF8 (Create Character, clients 7.0.16+)
//|						See Create2DCharacter() for details
//|
//|					Packet: 0x8D (Create Character, KR + SA 3D clients)
//|						See Create3DCharacter() for details
//o------------------------------------------------------------------------------------------------o
CPICreateCharacter::CPICreateCharacter()
{
}
CPICreateCharacter::CPICreateCharacter( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}
void CPICreateCharacter::Receive( void )
{
	if( tSock->ClientType() == CV_SA3D || tSock->ClientType() == CV_HS3D )
	{
		Create3DCharacter();
	}
	else
	{
		Create2DCharacter();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	Create3DCharacter()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with request to create character in 3D clients
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x8D (Create Character, KR + SA 3D clients)
//|					Size: 146 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] packet length
//|						BYTE[4] pattern1 (0xedededed)
//|						BYTE[4] character index/slot
//|						BYTE[30] character name
//|						BYTE[30] unknown (character password?)
//|						BYTE profession
//|						BYTE client flags (0x01 = Felucca Facet, 0x02 = Trammel Facet,
//|							0x04 = Ilshenar Facet, 0x08 = Malas Facet, 0x10 = Tokuno Facet,
//|							0x20 = Ter Mur Facet, 0x40 = UO3D Client, 0x80 = Reserved for Facet06,
//|							0x100 = UOTD. Also, starting locations? Hm)
//|						BYTE gender (male=0, female=1)
//|						BYTE race (human=0, elf=1, gargoyle=2)
//|						BYTE strength
//|						BYTE dexterity
//|						BYTE intelligence
//|						BYTE[2] skin color
//|						BYTE[4] unknown (0)
//|						BYTE[4] unknown (0)
//|						BYTE skill1
//|						BYTE skill1 value
//|						BYTE skill2
//|						BYTE skill2 value
//|						BYTE skill3
//|						BYTE skill3 value
//|						BYTE skill4
//|						BYTE skill4 value
//|						BYTE[25] unknown (0)
//|						BYTE unknown (0x0B)
//|						BYTE[2] hair color
//|						BYTE[2] hair style
//|						BYTE unknown (0x0C)
//|						BYTE[4] unknown (0)
//|						BYTE unknown (0x0D)
//|						BYTE[2] shirt color
//|						BYTE[2] shirt style/item id
//|						BYTE unknown (0x0F)
//|						BYTE[2] face color
//|						BYTE[2] face style/item id
//|						BYTE unknown (0x10)
//|						BYTE[2] beard color  // the last two might be reversed
//|						BYTE[2] beard style/item id*/
//o------------------------------------------------------------------------------------------------o
void CPICreateCharacter::Create3DCharacter( void )
{
	if( ValidateObject( tSock->CurrcharObj() ))
	{
		Console.Error( oldstrutil::format( "CreateCharacter packet 0x8D detected for socket with pre-existing character (%i) attached. Disconnecting socket as safeguard against corruption!", tSock->CurrcharObj()->GetSerial() ));
		Network->Disconnect( tSock );
		return;
	}

	tSock->Receive( 146, false );
	Network->Transfer( tSock );

	packetSize		= tSock->GetWord( 1 ); // Byte[2]
	pattern1		= tSock->GetDWord( 3 ); // Byte[4]
	slot			= tSock->GetDWord( 7 ); // Byte[4]
	profession		= tSock->GetByte( 71 ); // Byte[1]
	clientFlags		= tSock->GetByte( 72 ); // Byte[1]
	sex				= tSock->GetByte( 73 ); // Byte[1]
	race			= tSock->GetByte( 74 ); // Byte[1]
	str				= tSock->GetByte( 75 ); // Byte[1]
	dex				= tSock->GetByte( 76 ); // Byte[1]
	intel			= tSock->GetByte( 77 ); // Byte[1]
	skinColour		= tSock->GetWord( 78 ); // Byte[2]
	unknown1		= tSock->GetDWord( 80 ); // Byte[4]
	unknown2		= tSock->GetDWord( 84 ); // Byte[4]
	skill[0]		= tSock->GetByte( 88 ); // Byte[1]
	skillValue[0]	= tSock->GetByte( 89 ); // Byte[1]
	skill[1]		= tSock->GetByte( 90 ); // Byte[1]
	skillValue[1]	= tSock->GetByte( 91 ); // Byte[1]
	skill[2]		= tSock->GetByte( 92 ); // Byte[1]
	skillValue[2]	= tSock->GetByte( 93 ); // Byte[1]
	UI08 byteNum = 94;
	if( tSock->ClientType() >= CV_HS2D && tSock->ClientVersionSub() >= 16 )
	{
		skill[3]		= tSock->GetByte( 94 ); // Byte[1]
		skillValue[3]	= tSock->GetByte( 95 ); // Byte[1]
		byteNum = 96;
	}
	unknown4		= tSock->GetByte( static_cast<size_t>( byteNum ) +25 ); // Byte[1] // 121
	hairColour		= tSock->GetWord( static_cast<size_t>( byteNum ) +26 ); // Byte[2] // 122
	hairStyle		= tSock->GetWord( static_cast<size_t>( byteNum ) +28 ); // Byte[2] // 124
	unknown5		= tSock->GetByte( static_cast<size_t>( byteNum ) +30 ); // Byte[1]
	unknown6		= tSock->GetDWord( static_cast<size_t>( byteNum ) +31 ); // Byte[4]
	unknown7		= tSock->GetByte( static_cast<size_t>( byteNum ) +35 ); // Byte[1]
	shirtColour		= tSock->GetWord( static_cast<size_t>( byteNum ) +36 ); // Byte[2]
	shirtId			= tSock->GetWord( static_cast<size_t>( byteNum ) +38 ); // Byte[2]
	unknown8		= tSock->GetByte( static_cast<size_t>( byteNum ) +40 ); // Byte[1]
	faceColour		= tSock->GetWord( static_cast<size_t>( byteNum ) +41 ); // Byte[2]
	faceId			= tSock->GetWord( static_cast<size_t>( byteNum ) +43 ); // Byte[2]
	unknown9		= tSock->GetByte( static_cast<size_t>( byteNum ) +45 ); // Byte[1]
	facialHairColour= tSock->GetWord( static_cast<size_t>( byteNum ) +46 ); // Byte[2]
	facialHair		= tSock->GetWord( static_cast<size_t>( byteNum ) +48 ); // Byte[2]

	memcpy( charName, &tSock->Buffer()[11], 30 ); // Byte[30]
	memcpy( password, &tSock->Buffer()[41], 30 ); // Byte[30]
	memcpy( unknown3, &tSock->Buffer()[96], 25 ); // Byte[25]
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	Create2DCharacter()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with request to create character in 2D clients
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x00 (Create Character)
//|					Packet: 0xF8 (Create Character, clients 7.0.16+)
//|					Size: 104 or 106 Bytes
//|
//|					Packet Build
//|						BYTE cmd								0
//|						BYTE[4] pattern1 (0xedededed)			1
//|						BYTE[4] pattern2 (0xffffffff)			5
//|						BYTE pattern3 (0x00)					9
//|						BYTE[30] char name						10
//|						BYTE[30] char password					40
//|						BYTE sex (0=male, 1=female)				70
//|						BYTE str								71
//|						BYTE dex								72
//|						BYTE int								73
//|						BYTE skill1								74
//|						BYTE skill1value						75
//|						BYTE skill2								76
//|						BYTE skill2value						77
//|						BYTE skill3								78
//|						BYTE skill3value						79
//|						if clientVer > 7.0.16
//|							BYTE skill4							   80
//|							BYTE skill4value					   81
//|						BYTE[2] skinColor						80/82
//|						BYTE[2] hairStyle						82/84
//|						BYTE[2] hairColor						84/86
//|						BYTE[2] facial hair						86/88
//|						BYTE[2] facial hair color				88/90
//|						BYTE[2] location # from starting list	90/92
//|						BYTE[2] unknown1						92/94
//|						BYTE[2] slot							94/96
//|						BYTE[4] clientIP						96/98
//|						BYTE[2] shirt color						100/102
//|						BYTE[2] pants color						102/104
//o------------------------------------------------------------------------------------------------o
void CPICreateCharacter::Create2DCharacter( void )
{
	if( ValidateObject( tSock->CurrcharObj() ))
	{
		Console.Error( oldstrutil::format( "CreateCharacter packet 0x00 or 0xF8 detected for socket with pre-existing character (%i) attached. Disconnecting socket as safeguard against corruption!", tSock->CurrcharObj()->GetSerial() ));
		Network->Disconnect( tSock );
		return;
	}

	if( tSock->ClientType() >= CV_HS2D && tSock->ClientVersionSub() >= 16 )
	{
		tSock->Receive( 106, true );
	}
	else
	{
		tSock->Receive( 104, false );
	}

	Network->Transfer( tSock );

	pattern1		= tSock->GetDWord( 1 );
	pattern2		= tSock->GetDWord( 5 );
	pattern3		= tSock->GetByte( 9 );
	profession		= tSock->GetByte( 54 ); // Byte[54]

	// Fix for ClassicUO client
	if( profession == 255 )
	{
		profession = 0;
	}
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
	UI08 byteNum = 80;
	if( tSock->ClientType() >= CV_HS2D && tSock->ClientVersionSub() >= 16 )
	{
		skill[3]		= tSock->GetByte( 80 );
		skillValue[3]	= tSock->GetByte( 81 );
		byteNum = 82;
	}
	skinColour		= tSock->GetWord( byteNum );
	hairStyle		= tSock->GetWord( static_cast<size_t>( byteNum ) + 2 );
	hairColour		= tSock->GetWord( static_cast<size_t>( byteNum ) + 4 );
	facialHair		= tSock->GetWord( static_cast<size_t>( byteNum ) + 6 );
	facialHairColour= tSock->GetWord( static_cast<size_t>( byteNum ) + 8 );

	unknown			= tSock->GetByte( static_cast<size_t>( byteNum ) + 10 );
	locationNumber	= tSock->GetByte( static_cast<size_t>( byteNum ) + 11 );
	slot			= tSock->GetDWord( static_cast<size_t>( byteNum ) + 12 );
	ipAddress		= tSock->GetDWord( static_cast<size_t>( byteNum ) + 16 );
	shirtColour		= tSock->GetWord( static_cast<size_t>( byteNum ) + 20 );
	pantsColour		= tSock->GetWord( static_cast<size_t>( byteNum ) + 22 );

	memcpy( charName, &tSock->Buffer()[10], 30 );
	memcpy( password, &tSock->Buffer()[40], 30 ); // Does this really have anything to do with passwords?
}

void CPICreateCharacter::Log( std::ostream &outStream, bool fullHeader )
{
	if( tSock->ClientType() == CV_SA3D || tSock->ClientType() == CV_HS3D )
	{
		if( fullHeader )
		{
			outStream << "[RECV]Packet   : CPICreateCharacter 0x8D --> Length: 146" << TimeStamp() << std::endl;
		}
		outStream << "Pattern1       : " << pattern1 << std::endl;
		outStream << "Slot           : " << static_cast<SI32>( slot ) << std::endl;
		outStream << "Character Name : " << charName << std::endl;
		outStream << "Password       : " << password << std::endl;
		outStream << "Profession     : " << static_cast<SI32>( profession ) << std::endl;
		outStream << "Client Flags   : " << static_cast<SI32>( clientFlags ) << std::endl;
		outStream << "Sex            : " << static_cast<SI32>( sex ) << std::endl;
		outStream << "Race           : " << static_cast<SI32>( race ) << std::endl;
		outStream << "Strength       : " << static_cast<SI32>( str ) << std::endl;
		outStream << "Dexterity      : " << static_cast<SI32>( dex ) << std::endl;
		outStream << "Intelligence   : " << static_cast<SI32>( intel ) << std::endl;
		outStream << "Skin Colour    : " << std::hex << skinColour << std::dec << std::endl;
		outStream << "Unknown1       : " << static_cast<SI32>( unknown1 ) << std::endl;
		outStream << "Unknown2       : " << static_cast<SI32>( unknown2 ) << std::endl;
		outStream << "Skills         : " << static_cast<SI32>( skill[0] ) << " " << static_cast<SI32>( skill[1] ) << " " << static_cast<SI32>( skill[2] ) << " " << static_cast<SI32>( skill[3] ) << std::endl;
		outStream << "Skills Values  : " << static_cast<SI32>( skillValue[0] ) << " " << static_cast<SI32>( skillValue[1] ) << " " << static_cast<SI32>( skillValue[2] ) << " " << static_cast<SI32>( skillValue[3] ) << std::endl;
		SI32 temp1;
		for( temp1 = 0; temp1 < 25; temp1++ )
		{
			outStream << "Unknown3 index " << temp1 << " is hex value: " << std::hex << static_cast<UI32>( unknown3[temp1] ) << std::endl;
		}
		outStream << "Unknown4       : " << static_cast<SI32>( unknown4 ) << std::endl;
		outStream << "Hair Colour    : " << std::hex << hairColour << std::dec << std::endl;
		outStream << "Hair Style     : " << std::hex << hairStyle << std::dec << std::endl;
		outStream << "Unknown5       : " << static_cast<SI32>( unknown5 ) << std::endl;
		outStream << "Unknown6       : " << static_cast<SI32>( unknown6 ) << std::endl;
		outStream << "Unknown7       : " << static_cast<SI32>( unknown7 ) << std::endl;
		outStream << "Shirt Colour   : " << std::hex << shirtColour << std::dec << std::endl;
		outStream << "Shirt ID       : " << std::hex << shirtId << std::dec << std::endl;
		outStream << "Unknown8       : " << static_cast<SI32>( unknown8 ) << std::endl;
		outStream << "Face Colour    : " << std::hex << faceColour << std::dec << std::endl;
		outStream << "Face ID		 : " << std::hex << faceId << std::dec << std::endl;
		outStream << "Unknown9       : " << static_cast<SI32>( unknown9 ) << std::endl;
		outStream << "Facial Hair    : " << std::hex << facialHair << std::dec << std::endl;
		outStream << "Facial Hair Colour: " << std::hex << facialHairColour << std::dec << std::endl;
		outStream << "  Raw dump     :" << std::endl;
		CPInputBuffer::Log( outStream, false );
	}
	else
	{
		if( fullHeader )
		{
			if( tSock->GetByte( 0 ) == 0xF8 )
			{
				outStream << "[RECV]Packet   : CPICreateCharacter 0xF8 --> Length: 106" << TimeStamp() << std::endl;
			}
			else
			{
				outStream << "[RECV]Packet   : CPICreateCharacter 0x00 --> Length: 104" << TimeStamp() << std::endl;
			}
		}
		outStream << "Pattern1       : " << pattern1 << std::endl;
		outStream << "Pattern2       : " << pattern2 << std::endl;
		outStream << "Pattern3       : " << static_cast<SI32>( pattern3 ) << std::endl;
		outStream << "Character Name : " << charName << std::endl;
		outStream << "Password       : " << password << std::endl;
		outStream << "Sex            : " << static_cast<SI32>( sex ) << std::endl;
		outStream << "Strength       : " << static_cast<SI32>( str ) << std::endl;
		outStream << "Dexterity      : " << static_cast<SI32>( dex ) << std::endl;
		outStream << "Intelligence   : " << static_cast<SI32>( intel ) << std::endl;
		outStream << "Skills         : " << static_cast<SI32>( skill[0] ) << " " << static_cast<SI32>( skill[1] ) << " " << static_cast<SI32>( skill[2] ) << std::endl;
		outStream << "Skills Values  : " << static_cast<SI32>( skillValue[0] ) << " " << static_cast<SI32>( skillValue[1] ) << " " << static_cast<SI32>( skillValue[2] ) << std::endl;
		outStream << "Skin Colour    : " << std::hex << skinColour << std::dec << std::endl;
		outStream << "Hair Style     : " << std::hex << hairStyle << std::dec << std::endl;
		outStream << "Hair Colour    : " << std::hex << hairColour << std::dec << std::endl;
		outStream << "Facial Hair    : " << std::hex << facialHair << std::dec << std::endl;
		outStream << "Facial Hair Colour: " << std::hex << facialHairColour << std::dec << std::endl;
		outStream << "Location Number: " << locationNumber << std::endl;
		outStream << "Unknown        : " << unknown << std::endl;
		outStream << "Slot           : " << static_cast<SI32>( slot ) << std::endl;
		outStream << "IP Address     : " << ipAddress << std::endl;
		outStream << "Shirt Colour   : " << std::hex << shirtColour << std::dec << std::endl;
		outStream << "Pants Colour   : " << std::hex << pantsColour << std::dec << std::endl;
		outStream << "  Raw dump     :" << std::endl;
		CPInputBuffer::Log( outStream, false );
	}
}

// void CPICreateCharacter::Handle() implimented in pcmanage.cpp

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIPlayCharacter()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with request to login with a specific character
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x5D (Login Character)
//|					Size: 73 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[4] pattern1 (0xedededed)
//|						BYTE[30] char name (0 terminated)
//|						BYTE[33] unknown, mostly 0’s  (Byte# 0x27, 0x28, 0x30  seem to be the only
//|							non 0’s of these 33 bytes. Perhaps password data that’s not send anymore)
//|						BYTE slot choosen (0-based)
//|						BYTE[4] clientIP
//|
//|					void CPIPlayCharacter::Handle() implimented in pcmanage.cpp
//o------------------------------------------------------------------------------------------------o
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

	// Let's store our data more meaningfully

	pattern		= tSock->GetDWord( 1 );
	slotChosen	= tSock->GetByte( 68 );
	ipAddress	= tSock->GetDWord( 69 );

	//Reset client-info stored in account, so it's ready for login with a new client
	//NOTE: We can't do this until after character-selection, in case user
	//goes back to server-selection instead - in which case the socket's clientversion info is lost!
	//	tSock->AcctNo( AB_INVALID_ID );
	CAccountBlock_st& actbTemp = tSock->GetAccount();
	actbTemp.dwLastClientVer = 0;
	actbTemp.dwLastClientType = 0;
	actbTemp.dwLastClientVerShort = 0;

	memcpy( charName, &tSock->Buffer()[5], 30 );
	memcpy( unknown, &tSock->Buffer()[35], 33 );
}

void CPIPlayCharacter::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[RECV]Packet   : CPIPlayCharacter 0x5D --> Length: 73" << TimeStamp() << std::endl;
	}
	outStream << "Pattern1       : " << pattern << std::endl;
	outStream << "Char name      : " << charName << std::endl;
	outStream << "Slot chosen    : " << static_cast<SI32>( slotChosen ) << std::endl;
	outStream << "Client IP      : " << ipAddress << std::endl;
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIGumpInput()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with reply data for gump text entry dialog
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xAC (Gump Text Entry Dialog Reply)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] length
//|						BYTE[4] ID
//|						BYTE type
//|						BYTE index
//|						BYTE[3] unk
//|						BYTE[?] reply
//|
//|					void CPIGumpInput::Handle() implimented in gumps.cpp
//o------------------------------------------------------------------------------------------------o
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
	unk[0]	= tSock->GetByte( 9 ); // 1 == ok, 0 == cancel
	unk[1]	= tSock->GetByte( 10 );
	unk[2]	= tSock->GetByte( 11 ); // Length of reply
	reply	= ( char * ) & ( tSock->Buffer()[12] );

	// If no value was actually returned, treat it as a cancel!
	if( unk[0] == 1 && reply == "" )
	{
		unk[0] = 0;
	}
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
UI08 CPIGumpInput::Unk( SI32 offset ) const
{
	assert( offset >= 0 && offset <=2 );
	return unk[offset];
}
const std::string CPIGumpInput::Reply( void ) const
{
	return reply;
}

void CPIGumpInput::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[RECV]Packet   : CPIGumpInput 0xAC --> Length: " << tSock->GetWord( 1 ) << TimeStamp() << std::endl;
	}
	outStream << "ID             : " << id << std::endl;
	outStream << "Type           : " << static_cast<SI32>( type ) << std::endl;
	outStream << "Index          : " << static_cast<SI32>( index ) << std::endl;
	outStream << "Unknown        : " << static_cast<SI32>( unk[0] ) << " " << static_cast<SI32>( unk[1] ) << " " << static_cast<SI32>( unk[2] ) << std::endl;
	outStream << "Reply          : " << reply << std::endl;
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIHelpRequest()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with help request
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x9B (Request Help)
//|					Size: 258 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[257] (0x00)
//|
//|					void CPIHelpRequest::Handle() implimented in gumps.cpp
//o------------------------------------------------------------------------------------------------o
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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPITradeMessage()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with request for secure trading window
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x6F (Secure Trading)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] blockSize
//|						BYTE action
//|						BYTE[4] id1
//|						BYTE[4] id2
//|						BYTE[4] id3
//|						BYTE nameFollowing (0 or 1)
//|						If (nameFollowing = 1)
//|							BYTE[?] charName
//|
//|					void CPITradeMessage::Handle() implimented in gumps.cpp
//o------------------------------------------------------------------------------------------------o
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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIDyeWindow()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with request to open dye window
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x95 (Dye Window)
//|					Size: 9 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[4] itemID of dye target
//|						BYTE[2] ignored on send, model on return
//|						BYTE[2] model on send, color on return  (default on server send is 0x0FAB)
//o------------------------------------------------------------------------------------------------o
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
	modelId		= tSock->GetWord( 5 );
	newValue	= tSock->GetWord( 7 );
}

void CPIDyeWindow::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[RECV]Packet   : CPIDyeWindow 0x95 --> Length: 9" << TimeStamp() << std::endl;
	}
	outStream << "ItemID         : " << changing << std::endl;
	outStream << "Model          : " << modelId << std::endl;
	outStream << "Colour         : " << newValue << std::endl;
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}

bool CPIDyeWindow::Handle( void )
{
	if( changing >= BASEITEMSERIAL )
	{
		CItem *i = CalcItemObjFromSer( changing );
		if( ValidateObject( i ))
		{
			if( !tSock->DyeAll() )
			{
				if( newValue < 0x0002 || newValue > 0x03E9 )
				{
					newValue = 0x03E9;
				}
			}
			i->SetColour( newValue );
		}
	}
	else
	{
		CChar *mChar	= tSock->CurrcharObj();
		CChar *c		= CalcCharObjFromSer( changing );
		if( ValidateObject( c ))
		{
			if( !mChar->IsGM() )
				return true;

			UI16 body = c->GetId();
			SI32 b = newValue&0x4000;

			if((( newValue >> 8 ) < 0x80 ) && (( body >= 0x0190 && body <= 0x0193 ) || ( body >= 0x025D && body <= 0x0260 ) ||
											   ( body >= 0x00B7 && body <= 0x00BA ) || (body == 0x02EE || body == 0x02EF )))
			{
				newValue += 0x8000;
			}
			if( b == 16384 && ( body >= 0x0190 && body <= 0x03e1 ))
			{
				newValue = 0xF000; // but assigning the only "transparent" value that works, namly semi-trasnparency.
			}

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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIMsgBoardEvent()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet for bulletin board interaction
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x71 (Bulletin Board Message)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] len
//|						BYTE subcmd
//|						BYTE[len-4] submessage
//|
//|					bool CPIMsgBoardEvent::Handle() implemented in msgboard.cpp
//o------------------------------------------------------------------------------------------------o
CPIMsgBoardEvent::CPIMsgBoardEvent()
{
}
CPIMsgBoardEvent::CPIMsgBoardEvent( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}
CPIMsgBoardEvent::CPIMsgBoardEvent( CSocket *s, [[maybe_unused]] bool receive ) : CPInputBuffer( s )
{
}

void CPIMsgBoardEvent::Receive( void )
{
	tSock->Receive( 3, false );
	tSock->Receive( tSock->GetWord( 1 ), false );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPINewBookHeader()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet for book header
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xD4 (Book Header (New))
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] length
//|						BYTE[4] book ID
//|						BYTE flag1 (0 when sent by client)
//|						BYTE flag2 (0 when sent by client)
//|						BYTE[2] number of pages (0 when sent by client)
//|						BYTE[2] length of author (0 terminator included)
//|						BYTE[?][2] author (Unicode, 0 terminated)
//|						BYTE[2] length of title (0 terminator included)
//|						BYTE[?][2] title (Unicode, 0 terminated)
//|
//|					bool CPINewBookHeader::Handle() implemented in books.cpp
//o------------------------------------------------------------------------------------------------o
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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIToolTipRequestAoS()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with request to show gump entry tooltip
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xD6 (Mega Cliloc)
//|					Size: Variable
//|
//|					Packet Build
//|						Client Request
//|							BYTE[1] 0xD6
//|							BYTE[2] Length
//|							Loop for each serial being sent to request tooltip detail about
//|							BYTE[4] Serial
//|
//|						Server Response
//|							BYTE[1] 0xD6
//|							BYTE[2] Length
//|							BYTE[2] 0x0001
//|							BYTE[4] Serial of item/creature
//|							BYTE[2] 0x0000
//|							BYTE[4] Serial of item/creature in all tests. This could be the serial of the item the entry to appear over.
//|
//|							Loop of all the item/creature's properties to display in the order to display them. The name is always the first entry.
//|								BYTE[4] Cliloc ID
//|								BYTE[2] Length of (if any) Text to add into/with the cliloc
//|								BYTE[?] Unicode text to be added into the cliloc. Not sent if Length of text above is 0
//|
//|							BYTE[4] 00000000 - Sent as end of packet/loop
//o------------------------------------------------------------------------------------------------o
CPIToolTipRequestAoS::CPIToolTipRequestAoS() : getSer( INVALIDSERIAL )
{
}
CPIToolTipRequestAoS::CPIToolTipRequestAoS( CSocket *s ) : CPInputBuffer( s ), getSer( INVALIDSERIAL )
{
	Receive();
}

void CPIToolTipRequestAoS::Receive( void )
{
	tSock->Receive( 3, false );

	UI16 blockLen	= tSock->GetWord( 1 );
	if( blockLen == 0 || ( blockLen - 3 ) % 4 != 0 )
		return;

	tSock->Receive( blockLen, false );

	UI16 objCount = ( blockLen - 3 ) / 4;
	size_t offset = 3;
	for( UI16 i = 0; i < objCount; i++ )
	{
		getSer = tSock->GetDWord( offset + ( i * 4 ));
		// Only send tooltip if server feature for tooltips is enabled
		if( cwmWorldState->ServerData()->GetServerFeature( SF_BIT_AOS ))
		{
			CPToolTip tSend( getSer, tSock );
			tSock->Send( &tSend );
		}
	}
}
bool CPIToolTipRequestAoS::Handle( void )
{
	return true;
}
void CPIToolTipRequestAoS::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[RECV]Packet   : 0xD6 Tooltip Request --> Length: " << tSock->GetWord( 1 ) << TimeStamp() << std::endl;
	}
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, true );
}


//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIBookPage()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet for book pages
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x66 (Books (Pages))
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE 0x66
//|						BYTE[2] Length
//|						BYTE[4] Book Serial
//|						BYTE[2] Page Count
//|						Page Loop
//|							BYTE[2] Page #
//|							BYTE[2] Line Count
//|							Line Loop ( if used old packet 0x93 to open the book )
//|								BYTE[var] Line Text, Null Terminated
//|							Line Loop ( if used new packet 0xD4 to open the book )
//|								BYTE[var][2] Unicode Line Text, Null Terminated
//|
//|					bool CPIBookPage::Handle() implemented in books.cpp
//o------------------------------------------------------------------------------------------------o
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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIMetrics()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with client hardware / metrics
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xD9 (Spy on Client)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE unknown (02) Always 0x02 in my tests
//|						BYTE[4] Unique Instance ID of UO
//|						BYTE[4] OS Major
//|						BYTE[4] OS Minor
//|						BYTE[4] OS Revision
//|						BYTE CPU Manufacturer
//|						BYTE[4] CPU Family
//|						BYTE[4] CPU Model
//|						BYTE[4] CPU Clock Speed
//|						BYTE CPU Quantity
//|						BYTE[4] Memory
//|						BYTE[4] Screen Width
//|						BYTE[4] Screen Height
//|						BYTE[4] Screen Depth
//|						BYTE[2] Direct X Version
//|						BYTE[2] Direct X Minor
//|						BYTE[76?] Video Card Description
//|						BYTE[4] Video Card Vendor ID
//|						BYTE[4] Video Card Device ID
//|						BYTE[4] Video Card Memory
//|						BYTE Distribution
//|						BYTE Clients Running
//|						BYTE Clients Installed
//|						BYTE Partial Insstalled
//|						BYTE Unknown
//|						BYTE[4] Language Code
//|						BYTE[67] Unknown Ending
//|
//|					Older clients, from mid-late 2003 was set at 149 bytes length, whereas newest
//|					clients 4.01+ are set to 199 ??
//o------------------------------------------------------------------------------------------------o
CPIMetrics::CPIMetrics()
{
}

CPIMetrics::CPIMetrics( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}

void CPIMetrics::Receive( void )
{
	tSock->Receive( 0x010C, false );
}

bool CPIMetrics::Handle()
{
	// we're going to silently swallow this packet, really
	return true;
}

void CPIMetrics::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[RECV]Packet   : CPIMetrics 0xD9 --> Length: 268" << TimeStamp() << std::endl;
	}
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}


//	UI16			subCmd;
//	UI08			subSubCmd;

void PaperDoll( CSocket *s, CChar *pdoll );
bool BuyShop( CSocket *s, CChar *c );


CPISubcommands::CPISubcommands() : skipOver( false ), subPacket( nullptr )
{
}
CPISubcommands::CPISubcommands( CSocket *s ) : CPInputBuffer( s )
{
	skipOver	= false;
	subPacket	= nullptr;
	Receive();
}

//	Subcommand 1: Initialize Fast Walk Prevention
//	BYTE[4] key1
//	BYTE[4] key2
//	BYTE[4] key3
//	BYTE[4] key4
//	BYTE[4] key5
//	BYTE[4] key6

//	Subcommand 2: Add key to Fast Walk Stack
//	BYTE[4] newkey

//	Subcommand 5: Screen Size
//	BYTE[2]	unknown1, always 0?
//	BYTE[2]	X
//	BYTE[2]	Y
//	BYTE[2]	unknown2

//	Subcommand 8: Set cursor hue / Set MAP
//	BYTE hue (0 = Felucca, unhued / BRITANNIA map. 1 = Trammel, hued gold / BRITANNIA map, 2 = (switch to) ILSHENAR map)

//	Subcommand 0x0a: wrestling stun
//	Sent by using the client's Wrestle Stun Macro key in Options. This is no longer used since AoS was introduced. The Macro selection that used it was removed.

//	Subcommand 0x0b: Client Language
//	BYTE[3] language (ENU, for English)

//	Subcommand 0x0c: Closed Status Gump
//	BYTE[4] id (character id)

//	Subcommand 0x0e: 3D Client Action
//	BYTE[4] Animation ID. See Notes for list.

//	Subcommand 0x0f: unknown , send once at login
//	BYTE[5] unknown (0a 00 00 00 07) here

//	Subcommand 0x10: unknown, related to 0xD6 Mega Cliloc somehow
//	BYTE[4] Item ID
//	BYTE[4] Unknown

//	Subcommand 0x13: Request popup menu
//	BYTE[4] id (character id)

//	Subcommand 0x14: Display Popup menu
//	BYTE[2] Unknown(always 00 01)
//	BYTE[4] Serial
//	BYTE Number of entries in the popup
//	For each Entry
//	· BYTE[2] Entry Tag (this will be returned by the client on selection)
//	· BYTE[2] Text ID ID is the file number for intloc#.language e.g intloc6.enu and the index into that
//	· BYTE[2] Flags 0x01 = locked, 0x02 = arrow, 0x20 = color
//	· If (Flags &0x20)
//	· BYTE[2] color; // rgb 1555 color (ex, 0 = transparent, 0x8000 = solid black, 0x1F = blue, 0x3E0 = green, 0x7C00 = red)

//	Subcommand 0x15: Popup Entry Selection
//	BYTE[4] Character ID
//	BYTE[2] Entry Tag for line selected provided in subcommand 0x14

//	Subcommand 0x17: Codex of wisdom
//	BYTE unknown, always 1. if not 1, packet seems to have no effect
//	BYTE[4] msg number
//	BYTE presentation (0: “?” flashing, 1: directly opening)

//	Subcommand 0x18: Enable map-diff (files)
//	BYTE[4] Number of maps
//	For each map
//	· BYTE[4] Number of map patches in this map
//	· BYTE[4] Number of static patches in this map

//	Subcommand: 0x19: Extended stats
//	BYTE type // always 2 ? never seen other value
//	BYTE[4] serial
//	BYTE unknown // always 0 ?
//	BYTE lockBits // Bits: XXSS DDII (s=strength, d=dex, i=int), 0 = up, 1 = down, 2 = locked

//	Subcommand: 0x1a: Extended stats
//	BYTE stat // 0: str, 1: dex, 2:int
//	BYTE status // 0: up, 1:down, 2: locked

//	Subcommand 0x1b: New Spellbook
//	BYTE[2] unknown, always 1
//	BYTE[4] Spellbook serial
//	BYTE[2] Item Id
//	BYTE[2] scroll offset // 1==regular, 101=necro, 201=paladin
//	BYTE[8] spellbook content // first bit of first byte = spell #1, second bit of first byte = spell #2, first bit of second byte = spell #8, etc

//	Subcommand 0x1c: Spell selected, client side
//	BYTE[2] unknown, always 2
//	BYTE[2] selected spell(0-indexed)+scroll offset from sub 0x1b

//	Subcommand 0x1D: Send House Revision State
//	byte[4] houseserial
//	byte[4] revision state

//	Subcommand 0x1E:
//	byte[4] houseserial

//	Subcommand 0x21: (AOS) Ability icon confirm.
//	Note: no data, just (bf 0 5 21)

//	Subcommand 0x22: Damage
//	BYTE[2] unknown, always 1
//	BYTE[4] Serial
//	BYTE Damage // how much damage was done ?

//	Subcommand 0x24: UnKnown
//	BYTE unknown. UOSE Introduced

// Subcommand 0x2C : Bandage macro
//	BYTE[4]	Item Serial
//	BYTE[4] Target Serial

void CPISubcommands::Receive( void )
{
	tSock->Receive( 3, false );
	tSock->Receive( tSock->GetWord( 1 ), false );
	subCmd = tSock->GetWord( 3 );

	switch( subCmd )
	{
		case 0x01:	skipOver = true;	break;	// Server message sent to client.  6 keys, each a long, Initialize Fast Walk Prevention
		case 0x02:	skipOver = true;	break;	// Server message.  1 long, new key.  Add key to fast walk stack
		case 0x05:	skipOver = true;	break;	// Screen size
		case 0x0A:	skipOver = true;	break;	// Wrestling stun
		//case 0x0C:	skipOver = true;	break;	// Closed Status Gump
		case 0x0C: {	subPacket = new CPIClosedStatusGump( tSock );		}	break;	// Closed Status Gump
		case 0x0F:	skipOver = true;	break;	// Unknown, Sent once at Login
		case 0x24:	skipOver = true;	break;
		default:	Console.Print( oldstrutil::format( "Packet 0xBF: Unhandled Subcommand: 0x%X\n", subCmd ));	skipOver = true;	break;
		case 0x06:	{	subPacket = new CPIPartyCommand( tSock );		}	break;
		case 0x07:	{	subPacket = new CPITrackingArrow( tSock );		}	break;	// Click on Quest/Tracking Arrow
		case 0x0B:	{	subPacket = new CPIClientLanguage( tSock );		}	break;	// Client language.  3 bytes.  "ENU" for english
		case 0x0E:	{	subPacket = new CPIUOTDActions( tSock );		}	break;	// UOTD actions
		case 0x10:	{	subPacket = new CPIToolTipRequest( tSock );		}	break;	// Request for tooltip data
		case 0x13:	{	subPacket = new CPIPopupMenuRequest( tSock );	}	break;	// 0x13 (Request popup menu, 4 byte serial ID) -> Respond with 0x14
		case 0x15:	{	subPacket = new CPIPopupMenuSelect( tSock );	}	break;	// Popup Menu Selection
		case 0x1A:	{	subPacket = new CPIExtendedStats( tSock );		}	break;	// Extended Stats
		case 0x1C:	{	subPacket = new CPISpellbookSelect( tSock );	}	break;	// New SpellBook Selection
		case 0x2C:	{	subPacket = new CPIBandageMacro( tSock );		}	break;	// Bandage macro
		case 0x32:	{	subPacket = new CPIToggleFlying( tSock );		}	break;	// Toggle Flying on/off
	}
}
bool CPISubcommands::Handle( void )
{
	bool retVal = skipOver;
	if( subPacket != nullptr )
	{
		retVal = subPacket->Handle();
		delete subPacket;
	}
	return retVal;
}
void CPISubcommands::Log( std::ostream &outStream, bool fullHeader )
{
	if( subPacket != nullptr )
	{
		subPacket->Log( outStream, fullHeader );
	}
	else
	{
		if( fullHeader )
		{
			outStream << "[RECV]Packet   : CPISubcommands 0xBF --> Length: " << tSock->GetWord( 1 ) << TimeStamp() << std::endl;
		}
		outStream << "  Raw dump     :" << std::endl;
		CPInputBuffer::Log( outStream, false );
	}
}

// Party Command
//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIPartyCommand()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with party command
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xBF (General Information Packet)
//|					Subcommand: 0x06 (Party Command)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd (0xBF)
//|						BYTE[2] Length
//|						BYTE[2] Subcommand (0x06)
//|						Subcommand details
//|							Subsubcommand 1 - Add a party member
//|								Client
//|									BYTE[4]	id (if 0, a targeting cursor appears)
//|								Server
//|									BYTE	memberCount
//|									For each member
//|										BYTE[4] memberID
//|							Subsubcommand 2 - Remove a party member
//|								Client
//|									BYTE[4] id (if 0, a targeting cursor appears)
//|								Server
//|									BYTE	membersInNewParty
//|									BYTE[4]	idOfRemovedPlayer
//|									For each member
//|										BYTE[4]	memberID
//|							Subsubcommand 3 - Tell a party member a message
//|								BYTE[4]		id
//|								BYTE[n][2]	Null terminated Unicode message
//|							Subsubcommand 4 - Tell full party a message
//|								Client
//|									BYTE[n][2] Null terminated unicode message
//|								Server
//|									BYTE[4]	srcMember
//|									BYTE[n][2] Null terminated Unicode message
//|							Subsubcommand 6 - Party can loot me?
//|								Client
//|									BYTE	canLoot	( 0 == no, 1 == yes )
//|							Subsubcommand 7 - Send invitation to party
//|								Server
//|									BYTE[4]	serial of invited player
//|							Subsubcommand 8 - Accept join party invitation
//|								Client
//|									BYTE[4]	leaderID
//|							Subsubcommand 9 - Reject join party invitation
//|								Client
//|									BYTE[4] leaderID
//o------------------------------------------------------------------------------------------------o
CPIPartyCommand::CPIPartyCommand()
{
}
CPIPartyCommand::CPIPartyCommand( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}

void CPIPartyCommand::Receive( void )
{
}
bool CPIPartyCommand::Handle( void )
{
	UI08 partyCmd = tSock->GetByte( 5 );

	const SI32 PARTY_ADD		= 1;
	const SI32 PARTY_REMOVE		= 2;
	const SI32 PARTY_TELLINDIV	= 3;
	const SI32 PARTY_TELLALL	= 4;
	const SI32 PARTY_LOOT		= 6;
	const SI32 PARTY_INVITE		= 7;
	const SI32 PARTY_ACCEPT		= 8;
	const SI32 PARTY_DECLINE	= 9;
	const SI32 BASE_OFFSET		= 6;
	switch( partyCmd )
	{
		case PARTY_ADD:
		{
			SERIAL charToAdd = tSock->GetDWord( BASE_OFFSET );
			if( charToAdd != 0 )
			{	// it really is a serial
				tSock->SetDWord( 7, charToAdd );
				PartyFactory::GetSingleton().CreateInvite( tSock );
			}
			else
			{
				tSock->SendTargetCursor( 0, TARGET_PARTYADD, Dictionary->GetEntry( 9003, tSock->Language() )); // Select the character to add to the party
			}
			break;
		}
		case PARTY_REMOVE:
		{
			Party *ourParty = PartyFactory::GetSingleton().Get( tSock->CurrcharObj() );
			if( ourParty != nullptr )
			{
				SERIAL charToRemove	= tSock->GetDWord( BASE_OFFSET );
				if(( ourParty->Leader() == tSock->CurrcharObj() ) || ( tSock->CurrcharObj()->GetSerial() == charToRemove ))
				{
					if( charToRemove != 0 )
					{	// it really is a serial
						tSock->SetDWord( 7, charToRemove );
						PartyFactory::GetSingleton().Kick( tSock );
					}
					else
					{	// Crap crap crap, what do we do here?
						tSock->SendTargetCursor( 0, TARGET_PARTYREMOVE, Dictionary->GetEntry( 9004, tSock->Language() )); // Select the character to remove from the party
					}
				}
				else
				{
					tSock->SysMessage( 9005 ); // You have to be the leader to do that
				}
			}
			else
			{
				tSock->SysMessage( 9006 ); // You are not in a party
			}
			break;
		}
		case PARTY_TELLINDIV:
		{
			Party *toTellTo	= PartyFactory::GetSingleton().Get( tSock->CurrcharObj() );
			if( toTellTo != nullptr )
			{
				CPPartyTell toTell( this, tSock );
				SERIAL personToTell	= tSock->GetDWord( BASE_OFFSET );
				CChar *charToTell	= CalcCharObjFromSer( personToTell );
				CSocket *charTell	= charToTell->GetSocket();
				if( charTell != nullptr )
				{
					toTellTo->SendPacket( &toTell, charTell );
				}
			}
			else
			{
				tSock->SysMessage( 9006 ); // You are not in a party
			}
		}
			break;
		case PARTY_TELLALL:
		{
			Party *toTellTo	= PartyFactory::GetSingleton().Get( tSock->CurrcharObj() );
			if( toTellTo != nullptr )
			{
				CPPartyTell toTell( this, tSock );
				toTellTo->SendPacket( &toTell );
			}
			else
			{
				tSock->SysMessage( 9006 ); // You are not in a party
			}
			break;
		}
		case PARTY_LOOT:
		{
			Party *toAddTo = PartyFactory::GetSingleton().Get( tSock->CurrcharObj() );
			if( toAddTo != nullptr )
			{
				CPartyEntry *mEntry = toAddTo->Find( tSock->CurrcharObj() );
				if( mEntry != nullptr )
				{
					mEntry->IsLootable( tSock->GetByte( BASE_OFFSET ) != 0 );
				}
				tSock->SysMessage( 9007 ); // You have changed your loot setting
			}
			break;
		}
		case PARTY_INVITE:
		{	// THIS SHOULD NEVER HAPPEN -> Server message!
			break;
		}
		case PARTY_ACCEPT:
		{
			SERIAL leaderSerial	= tSock->GetDWord( BASE_OFFSET );
			Party *toAddTo = PartyFactory::GetSingleton().Get( CalcCharObjFromSer( leaderSerial ));
			if( toAddTo != nullptr )
			{
				toAddTo->AddMember( tSock->CurrcharObj() );
			}
			else
			{
				tSock->SysMessage( 9008 ); // That party does not exist any more
			}
			break;
		}
		case PARTY_DECLINE:
		{
			SERIAL leaderSerial	= tSock->GetDWord( BASE_OFFSET );
			CChar *leader = CalcCharObjFromSer( leaderSerial );
			if( leader != nullptr )
			{
				CSocket *leaderSock = leader->GetSocket();
				if( leaderSock != nullptr )
				{	// is it a PC leader?
					leaderSock->SysMessage( 9009 ); // The player has declined your invitation to party
				}
			}
			break;
		}
	}
	return true;
}
void CPIPartyCommand::Log( std::ostream &outStream, bool fullHeader )
{
	UI08 partyCmd = tSock->GetByte( 5 );

	const SI32 PARTY_ADD		= 1;
	const SI32 PARTY_REMOVE		= 2;
	const SI32 PARTY_TELLINDIV	= 3;
	const SI32 PARTY_TELLALL	= 4;
	const SI32 PARTY_LOOT		= 6;
	const SI32 PARTY_ACCEPT		= 8;
	const SI32 PARTY_DECLINE	= 9;

	const SI32 BASE_OFFSET		= 6;
	switch( partyCmd )
	{
		case PARTY_ADD:
		{
			//	Subcommand 1		Add a party member
			//		Client
			//			BYTE[4]	id		(if 0, a targeting cursor appears)
			if( fullHeader )
			{
				outStream << "[RECV]Packet   : CPISubcommands 0xBF Subpacket Party Command Subcommand PARTY_ADD --> Length: " << tSock->GetWord( 1 ) << TimeStamp() << std::endl;
			}
			outStream << "To add         : 0x" << std::hex << tSock->GetDWord( BASE_OFFSET ) << std::dec << std::endl;
			break;
		}
		case PARTY_REMOVE:
		{
			//	Subcommand 2		Remove a party member
			//		Client
			//			BYTE[4] id		(if 0, a targeting cursor appears)
			if( fullHeader )
			{
				outStream << "[RECV]Packet   : CPISubcommands 0xBF Subpacket Party Command Subcommand PARTY_REMOVE --> Length: " << tSock->GetWord( 1 ) << TimeStamp() << std::endl;
			}
			outStream << "To remove      : 0x" << std::hex << tSock->GetDWord( BASE_OFFSET ) << std::dec << std::endl;
			break;
		}
		case PARTY_TELLINDIV:
		{
			//	Subcommand 3		Tell a party member a message
			//		BYTE[4]		id
			//		BYTE[n][2]	Null terminated Unicode message
			if( fullHeader )
			{
				outStream << "[RECV]Packet   : CPISubcommands 0xBF Subpacket Party Command Subcommand PARTY_TELLINDIV --> Length: " << tSock->GetWord( 1 ) << TimeStamp() << std::endl;
			}
			break;
		}
		case PARTY_TELLALL:
		{
			//	Subcommand 4		Tell full party a message
			//		Client
			//			BYTE[n][2]	Null terminated unicode message
			if( fullHeader )
			{
				outStream << "[RECV]Packet   : CPISubcommands 0xBF Subpacket Party Command Subcommand PARTY_TELLALL --> Length: " << tSock->GetWord( 1 ) << TimeStamp() << std::endl;
			}
			break;
		}
		case PARTY_LOOT:
		{
			//	Subcommand 6		Party can loot me?
			//		Client
			//			BYTE	canLoot	( 0 == no, 1 == yes )
			if( fullHeader )
			{
				outStream << "[RECV]Packet   : CPISubcommands 0xBF Subpacket Party Command Subcommand PARTY_LOOT --> Length: " << tSock->GetWord( 1 ) << TimeStamp() << std::endl;
			}
			outStream << "Can loot       : " << (( tSock->GetByte( BASE_OFFSET ) == 1 ) ? "yes" : "no" ) << std::endl;
			break;
		}
		case PARTY_ACCEPT:
		{
			//	Subcommand 8		Accept join party invitation
			//		Client
			//			BYTE[4]	leaderID
			if( fullHeader )
			{
				outStream << "[RECV]Packet   : CPISubcommands 0xBF Subpacket Party Command Subcommand PARTY_ACCEPT --> Length: " << tSock->GetWord( 1 ) << TimeStamp() << std::endl;
			}
			outStream << "Leader         : 0x" << std::hex << tSock->GetDWord( BASE_OFFSET ) << std::dec << std::endl;
			break;
		}
		case PARTY_DECLINE:
		{
			//	Subcommand 9		Reject join party invitation
			//		Client
			//			BYTE[4] leaderID
			if( fullHeader )
			{
				outStream << "[RECV]Packet   : CPISubcommands 0xBF Subpacket Party Command Subcommand PARTY_DECLINE --> Length: " << tSock->GetWord( 1 ) << TimeStamp() << std::endl;
			}
			outStream << "Character      : 0x" << std::hex << tSock->GetDWord( BASE_OFFSET ) << std::dec << std::endl;
			break;
		}
	}
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPITrackingArrow()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with request to click on Quest/Tracking arrow
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xBF (General Information Packet)
//|					Subcommand: 0x07 (Quest/Tracking Arrow)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd (0xBF)
//|						BYTE[2] Length
//|						BYTE[2] Subcommand (0x07)
//|						Subcommand details
//|							BYTE Right Click (0x01 = yes, 0x00 = no)
//o------------------------------------------------------------------------------------------------o
CPITrackingArrow::CPITrackingArrow()
{
}
CPITrackingArrow::CPITrackingArrow( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}

void CPITrackingArrow::Receive( void )
{
}
bool CPITrackingArrow::Handle( void )
{
	CChar *mChar = tSock->CurrcharObj();
	if( ValidateObject( mChar ) && tSock->GetByte( 5 ) == 0x01 )
	{
		tSock->SetTimer( tPC_TRACKING, 0 );
		CChar *trackingTarg = mChar->GetTrackingTarget();
		if( ValidateObject( trackingTarg ))
		{
			CPTrackingArrow tSend = ( *trackingTarg );
			tSend.Active( 0 );
			if( tSock->ClientVersion() >= CV_HS2D )
			{
				tSend.AddSerial( trackingTarg->GetSerial() );
			}
			tSock->Send( &tSend );
		}
		else
		{
			CPTrackingArrow tSend;
			tSend.Active( 0 );
			if( tSock->ClientVersion() >= CV_HS2D )
			{
				tSend.AddSerial( mChar->GetTrackingTargetSerial() );
			}
			tSock->Send( &tSend );
		}
	}
	return true;
}
void CPITrackingArrow::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[RECV]Packet   : CPISubcommands 0xBF Subpacket Tracking Arrow --> Length: " << tSock->GetWord( 1 ) << TimeStamp() << std::endl;
	}
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIClientLanguage()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with request to set/change client language (?)
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xBF (General Information Packet)
//|					Subcommand: 0x0B (Client Language)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd (0xBF)
//|						BYTE[2] Length
//|						BYTE[2] Subcommand (0x0B)
//|						Subcommand details
//|							BYTE[3] language (ENU, for English)
//o------------------------------------------------------------------------------------------------o
CPIClientLanguage::CPIClientLanguage()
{
}
CPIClientLanguage::CPIClientLanguage( CSocket *s ) : CPInputBuffer( s )
{
	newLang = ZERO;
	Receive();
}

void CPIClientLanguage::Receive( void )
{
	newLang = FindLanguage( tSock, 5 );
}
bool CPIClientLanguage::Handle( void )
{
	tSock->Language( newLang );
	return true;
}
void CPIClientLanguage::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[RECV]Packet   : CPISubcommands 0xBF Subpacket Client Language --> Length: " << tSock->GetWord( 1 ) << TimeStamp() << std::endl;
	}
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIUOTDActions()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with request to perform an action in TD/3D client
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xBF (General Information Packet)
//|					Subcommand: 0x0E (TD/3D Client Action)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd (0xBF)
//|						BYTE[2] Length
//|						BYTE[2] Subcommand (0x0E)
//|						Subcommand details
//|							BYTE[4] Animation ID
//|								00 00 00 06 - Yawn
//|								00 00 00 15 - Faint
//|								00 00 00 20 - Bow
//|								00 00 00 21 - Salute
//|								00 00 00 64 - Applaud
//|								00 00 00 66 - Argue
//|								00 00 00 68 - Blow Kiss
//|								00 00 00 69 - Formal Bow
//|								00 00 00 6B - Cover Ears
//|								00 00 00 6C - Curtsey
//|								00 00 00 6D - Jig
//|								00 00 00 6E - Folk Dance
//|								00 00 00 6F - Dance
//|								00 00 00 70 - Tribal Dance
//|								00 00 00 71 - Fold Arms
//|								00 00 00 72 - Impatient
//|								00 00 00 73 - Lecture
//|								00 00 00 74 - Nod
//|								00 00 00 75 - Point
//|								00 00 00 77 - Greet Salute
//|								00 00 00 79 - Shake Head
//|								00 00 00 7B - Victory
//|								00 00 00 7C - Celebrate
//|								00 00 00 7D - Wave
//|								00 00 00 7E - Two Handed Wave
//|								00 00 00 7F - Long Distance Wave
//|								00 00 00 80 - What?
//o------------------------------------------------------------------------------------------------o
CPIUOTDActions::CPIUOTDActions()
{
}
CPIUOTDActions::CPIUOTDActions( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}

void CPIUOTDActions::Receive( void )
{
	action = tSock->GetWord( 7 );
}
bool CPIUOTDActions::Handle( void )
{
	CChar *ourChar = tSock->CurrcharObj();
	Effects->PlayCharacterAnimation( ourChar, action, 1 );
	return true;
}
void CPIUOTDActions::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[RECV]Packet   : CPISubcommands 0xBF Subpacket UOTD Actions --> Length: " << tSock->GetWord( 1 ) << TimeStamp() << std::endl;
	}
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIToolTipRequest()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with request to show gump entry tooltip
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xBF (General Information Packet)
//|					Subcommand: 0x10 (Popup Entry Select)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd (0xBF)
//|						BYTE[2] Length
//|						BYTE[2] Subcommand (0x10)
//|						Subcommand details
//|							BYTE[4] Item ID
//|							BYTE[4] Unknown
//o------------------------------------------------------------------------------------------------o
CPIToolTipRequest::CPIToolTipRequest() : getSer( INVALIDSERIAL )
{
}
CPIToolTipRequest::CPIToolTipRequest( CSocket *s ) : CPInputBuffer( s ), getSer( INVALIDSERIAL )
{
	Receive();
}

void CPIToolTipRequest::Receive( void )
{
	getSer = tSock->GetDWord( 5 );
}
bool CPIToolTipRequest::Handle( void )
{
	if( getSer != INVALIDSERIAL )
	{
		// Only send tooltip if server feature for tooltips is enabled
		if( cwmWorldState->ServerData()->GetServerFeature( SF_BIT_AOS ))
		{
			CPToolTip tSend( getSer, tSock );
			tSock->Send( &tSend );
		}
	}
	return true;
}
void CPIToolTipRequest::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[RECV]Packet   : CPISubcommands 0xBF Subpacket Tooltip Request --> Length: " << tSock->GetWord( 1 ) << TimeStamp() << std::endl;
	}
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIPopupMenuRequest()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with request to show popup/context menu
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xBF (General Information Packet)
//|					Subcommand: 0x13 (Request Popup Menu)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd (0xBF)
//|						BYTE[2] Length
//|						BYTE[2] Subcommand (0x13)
//|						Subcommand details
//|							BYTE[4] id (character id)
//o------------------------------------------------------------------------------------------------o
CPIPopupMenuRequest::CPIPopupMenuRequest() : mSer( INVALIDSERIAL )
{
}
CPIPopupMenuRequest::CPIPopupMenuRequest( CSocket *s ) : CPInputBuffer( s ), mSer( INVALIDSERIAL )
{
	Receive();
}

void CPIPopupMenuRequest::Receive( void )
{
	mSer = tSock->GetDWord( 5 );
}
bool CPIPopupMenuRequest::Handle( void )
{
	// Only show context menus if enabled in ini
	if( !cwmWorldState->ServerData()->ServerContextMenus() )
		return true;
		
	CBaseObject *myObj = nullptr;
 
	if( mSer < BASEITEMSERIAL )
	{
		myObj = static_cast<CBaseObject*>( CalcCharObjFromSer( mSer ));
		if( myObj == nullptr )
			return true;
	}
	else
	{
		myObj = static_cast<CBaseObject*>( CalcItemObjFromSer( mSer ));
		if( myObj == nullptr )
			return true;
	}
	
	std::vector<UI16> scriptTriggers = myObj->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			if( toExecute->OnContextMenuRequest( tSock, myObj ) == 0 )
			{
				return true;
			}
		}
	}
 
	// No individual scripts handling onContextMenu returned true - let's check global script!
	cScript *toExecute = JSMapping->GetScript( static_cast<UI16>( 0 ));
	if( toExecute != nullptr )
	{
		if( toExecute->OnContextMenuRequest( tSock, myObj ) == 0 )
		{
			return true;
		}
	}

	if( !LineOfSight( tSock, tSock->CurrcharObj(), myObj->GetX(), myObj->GetY(), ( myObj->GetZ() + 15 ), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false ))
		return true;

	CPPopupMenu toSend(( *myObj ), ( *tSock ));
	tSock->Send( &toSend );
	return true;
}
void CPIPopupMenuRequest::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[RECV]Packet   : CPISubcommands 0xBF Subpacket Popup Menu Request --> Length: " << tSock->GetWord( 1 ) << TimeStamp() << std::endl;
	}
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIPopupMenuSelect()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with request select entry from popup/context menu
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xBF (General Information Packet)
//|					Subcommand: 0x15 (Popup Entry Select)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd (0xBF)
//|						BYTE[2] Length
//|						BYTE[2] Subcommand (0x15)
//|						Subcommand details
//|							BYTE[4] Character ID
//|							BYTE[2] Entry Tag for line selected provided in subcommand 0x14
//o------------------------------------------------------------------------------------------------o
CPIPopupMenuSelect::CPIPopupMenuSelect() : popupEntry( 0 ), targObj( nullptr )
{
}
CPIPopupMenuSelect::CPIPopupMenuSelect( CSocket *s ) : CPInputBuffer( s ), popupEntry( 0 ), targObj( nullptr )
{
	Receive();
}

void CPIPopupMenuSelect::Receive( void )
{
	popupEntry	= tSock->GetWord( 9 );
 
	SERIAL mSer = tSock->GetDWord( 5 );
	if( mSer < BASEITEMSERIAL )
	{
		targObj = static_cast<CBaseObject*>( CalcCharObjFromSer( mSer ));
	}
	else
	{
		targObj = static_cast<CBaseObject*>( CalcItemObjFromSer( mSer ));
	}
}

bool WhichResponse( CSocket *mSock, CChar *mChar, std::string text, CChar *tChar = nullptr );
bool CPIPopupMenuSelect::Handle( void )
{
	// Only show context menus if enabled in ini
	if( !cwmWorldState->ServerData()->ServerContextMenus() )
		return true;

	CChar *mChar			= tSock->CurrcharObj();
	if( !ValidateObject( targObj ) || !ValidateObject( mChar ))
		return true;

	std::vector<UI16> scriptTriggers = targObj->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			if( toExecute->OnContextMenuSelect( tSock, targObj, popupEntry ) == 0 )
			{
				return true;
			}
		}
	}

	// No individual scripts handling onContextMenu returned true - let's check global script!
	cScript *toExecute = JSMapping->GetScript( static_cast<UI16>( 0 ));
	if( toExecute != nullptr )
	{
		if( toExecute->OnContextMenuSelect( tSock, targObj, popupEntry ) == 0 )
		{
			return true;
		}
	}

	if( targObj->CanBeObjType( OT_ITEM )) // No items with hard coded context menus
		return true;

	CChar *targChar = static_cast<CChar *>( targObj );

	switch( popupEntry )
	{
		case 0x000A:	// Open Paperdoll
			if( cwmWorldState->creatures[targChar->GetId()].IsHuman() )
			{
				PaperDoll( tSock, targChar );
			}
			break;
		case 0x000B:	// Open Backpack
			if( mChar->GetCommandLevel() >= CL_CNS || cwmWorldState->creatures[targChar->GetId()].IsHuman() 
				|| targChar->GetId() == 0x0123 || targChar->GetId() == 0x0124 || targChar->GetId() == 0x0317 )	// Only Humans and Pack Animals have Packs
			{
				if( mChar->IsDead() )
				{
					tSock->SysMessage( 392 ); // You are dead and cannot do that.
				}
				else if( !ObjInRange( mChar, targChar, DIST_NEARBY ))
				{
					tSock->SysMessage( 382 ); // You need to get closer.
				}
				else
				{
					CItem *pack = targChar->GetPackItem();
					if( ValidateObject( pack ))
					{
						if( mChar == targChar || targChar->GetOwnerObj() == mChar || mChar->GetCommandLevel() >= CL_CNS )
						{
							if( targChar->GetNpcAiType() == AI_PLAYERVENDOR )
							{
								targChar->TextMessage( tSock, 385, TALK, false ); // Take a look at my goods.
								tSock->OpenPack( pack, true );
							}
							else
							{
								tSock->OpenPack( pack );
							}
						}
						else if( targChar->GetNpcAiType() == AI_PLAYERVENDOR )
						{
							targChar->TextMessage( tSock, 385, TALK, false ); // Take a look at my goods.
							tSock->OpenPack( pack, true );
						}
						else
						{
							Skills->Snooping( tSock, targChar, pack );
						}
					}
					else
					{
						Console.Warning( oldstrutil::format( "Character 0x%X has no backpack!", targChar->GetSerial() ));
					}
				}
			}
			break;
		case 0x000C:	// Buy Window
			if( targChar->IsShop() )
			{
				if( ObjInRange( mChar, targChar, 8 ))
				{
					targChar->SetTimer( tNPC_MOVETIME, BuildTimeValue( 60 ));
					BuyShop( tSock, targChar );
				}
				else
				{
					tSock->SysMessage( 393 ); // That is too far away
				}
			}
			break;
		case 0x000D:	// Sell Window
			if( targChar->IsShop() )
			{
				if( ObjInRange( mChar, targChar, 8 ))
				{
					targChar->SetTimer( tNPC_MOVETIME, BuildTimeValue( 60 ));
					CPSellList toSend;
					if( toSend.CanSellItems(( *mChar ), ( *targChar )))
					{
						tSock->Send( &toSend );
					}
					else
					{
						targChar->TextMessage( tSock, 1341, TALK, false ); // Thou doth posses nothing of interest to me.
					}
				}
				else
				{
					tSock->SysMessage( 393 ); // That is too far away
				}
			}
			break;
		case 0x000E:	// Open Bankbox
			if( targChar->GetNpcAiType() == AI_BANKER )
			{
				if( ObjInRange( mChar, targChar, 8 ))
				{
					tSock->OpenBank( mChar );
				}
				else
				{
					tSock->SysMessage( 393 ); // That is too far away
				}
			}
			break;
		case 0x000F:	// Tame
			if( cwmWorldState->creatures[targChar->GetId()].IsAnimal() )
			{
				if( ObjInRange( mChar, targChar, 8 ))
				{
					// Set a tag on the player to reference the animal they're about to tame
					TAGMAPOBJECT targCharSerial;
					targCharSerial.m_Destroy = false;
					targCharSerial.m_IntValue = targChar->GetSerial();
					targCharSerial.m_ObjectType = TAGMAP_TYPE_INT;
					targCharSerial.m_StringValue = "";
					mChar->SetTag( "tameSerial", targCharSerial );

					Skills->SkillUse( tSock, TAMING );
				}
				else
				{
					tSock->SysMessage( 393 ); // That is too far away
				}
			}
			break;
		case 0x0010:	// Command: Kill (Pet)
		{
			if( ObjInRange( mChar, targChar, 12 ))
			{
				// Set a tag on the player to reference the pet they are commanding
				TAGMAPOBJECT targCharSerial;
				targCharSerial.m_Destroy = false;
				targCharSerial.m_IntValue = targChar->GetSerial();
				targCharSerial.m_ObjectType = TAGMAP_TYPE_INT;
				targCharSerial.m_StringValue = "";
				mChar->SetTag( "petCommandObj", targCharSerial );

				tSock->ClearTrigWords();
				tSock->AddTrigWord( TW_KILL );
				std::string targName = GetNpcDictName( targChar, tSock, NRS_SYSTEM );
				WhichResponse( tSock, mChar, targName, targChar );
			}
			else
			{
				tSock->SysMessage( 393 ); // That is too far away
			}
			break;
		}
		case 0x0011:	// Command: Stop (Pet)
			if( ObjInRange( mChar, targChar, 12 ))
			{
				// Set a tag on the player to reference the pet they are commanding
				TAGMAPOBJECT targCharSerial;
				targCharSerial.m_Destroy = false;
				targCharSerial.m_IntValue = targChar->GetSerial();
				targCharSerial.m_ObjectType = TAGMAP_TYPE_INT;
				targCharSerial.m_StringValue = "";
				mChar->SetTag( "petCommandObj", targCharSerial );

				tSock->ClearTrigWords();
				tSock->AddTrigWord( TW_STOP );
				std::string targName = GetNpcDictName( targChar, tSock, NRS_SYSTEM );
				WhichResponse( tSock, mChar, targName, targChar );
			}
			else
			{
				tSock->SysMessage( 393 ); // That is too far away
			}
			break;
		case 0x0012:	// Command: Follow (Pet)
			if( ObjInRange( mChar, targChar, 12 ))
			{
				// Set a tag on the player to reference the pet they are commanding
				TAGMAPOBJECT targCharSerial;
				targCharSerial.m_Destroy = false;
				targCharSerial.m_IntValue = targChar->GetSerial();
				targCharSerial.m_ObjectType = TAGMAP_TYPE_INT;
				targCharSerial.m_StringValue = "";
				mChar->SetTag( "petCommandObj", targCharSerial );

				tSock->ClearTrigWords();
				tSock->AddTrigWord( TW_FOLLOW );
				std::string targName = GetNpcDictName( targChar, tSock, NRS_SYSTEM );
				WhichResponse( tSock, mChar, targName, targChar );
			}
			else
			{
				tSock->SysMessage( 393 ); // That is too far away
			}
			break;
		case 0x0013:	// Command: Stay (Pet)
			if( ObjInRange( mChar, targChar, 12 ))
			{
				// Set a tag on the player to reference the pet they are commanding
				TAGMAPOBJECT targCharSerial;
				targCharSerial.m_Destroy = false;
				targCharSerial.m_IntValue = targChar->GetSerial();
				targCharSerial.m_ObjectType = TAGMAP_TYPE_INT;
				targCharSerial.m_StringValue = "";
				mChar->SetTag( "petCommandObj", targCharSerial );

				tSock->ClearTrigWords();
				tSock->AddTrigWord( TW_STAY );
				std::string targName = GetNpcDictName( targChar, tSock, NRS_SYSTEM );
				WhichResponse( tSock, mChar, targName, targChar );
			}
			else
			{
				tSock->SysMessage( 393 ); // That is too far away
			}
			break;
		case 0x0014:	// Command: Guard (Pet)
			if( ObjInRange( mChar, targChar, 12 ))
			{
				// Set a tag on the player to reference the pet they are commanding
				TAGMAPOBJECT targCharSerial;
				targCharSerial.m_Destroy = false;
				targCharSerial.m_IntValue = targChar->GetSerial();
				targCharSerial.m_ObjectType = TAGMAP_TYPE_INT;
				targCharSerial.m_StringValue = "";
				mChar->SetTag( "petCommandObj", targCharSerial );

				tSock->ClearTrigWords();
				tSock->AddTrigWord( TW_GUARD );
				std::string targName = GetNpcDictName( targChar, tSock, NRS_SYSTEM );
				WhichResponse( tSock, mChar, targName, targChar );
			}
			else
			{
				tSock->SysMessage( 393 ); // That is too far away
			}
			break;
		case 0x0015:	// Add Friend (Pet/Hireling)
			if( ObjInRange( mChar, targChar, 12 ))
			{
				// Set a tag on the player to reference the pet they are commanding
				TAGMAPOBJECT targCharSerial;
				targCharSerial.m_Destroy = false;
				targCharSerial.m_IntValue = targChar->GetSerial();
				targCharSerial.m_ObjectType = TAGMAP_TYPE_INT;
				targCharSerial.m_StringValue = "";
				mChar->SetTag( "petCommandObj", targCharSerial );

				tSock->ClearTrigWords();
				tSock->AddTrigWord( TW_FRIEND );
				std::string targName = GetNpcDictName( targChar, tSock, NRS_SYSTEM );
				WhichResponse( tSock, mChar, targName, targChar );
			}
			else
			{
				tSock->SysMessage( 393 ); // That is too far away
			}
			break;
		case 0x0016:	// Transfer (Pet)
			if( ObjInRange( mChar, targChar, 12 ))
			{
				// Set a tag on the player to reference the pet they are commanding
				TAGMAPOBJECT targCharSerial;
				targCharSerial.m_Destroy = false;
				targCharSerial.m_IntValue = targChar->GetSerial();
				targCharSerial.m_ObjectType = TAGMAP_TYPE_INT;
				targCharSerial.m_StringValue = "";
				mChar->SetTag( "petCommandObj", targCharSerial );

				tSock->ClearTrigWords();
				tSock->AddTrigWord( TW_TRANSFER );
				std::string targName = GetNpcDictName( targChar, tSock, NRS_SYSTEM );
				WhichResponse( tSock, mChar, targName, targChar );
			}
			else
			{
				tSock->SysMessage( 393 ); // That is too far away
			}
			break;
		case 0x0017:	// Release (Pet)
			if( ObjInRange( mChar, targChar, 12 ))
			{
				// Set a tag on the player to reference the pet they are commanding
				TAGMAPOBJECT targCharSerial;
				targCharSerial.m_Destroy = false;
				targCharSerial.m_IntValue = targChar->GetSerial();
				targCharSerial.m_ObjectType = TAGMAP_TYPE_INT;
				targCharSerial.m_StringValue = "";
				mChar->SetTag( "petCommandObj", targCharSerial );

				tSock->ClearTrigWords();
				tSock->AddTrigWord( TW_RELEASE );
				std::string targName = GetNpcDictName( targChar, tSock, NRS_SYSTEM );
				WhichResponse( tSock, mChar, targName, targChar );
			}
			else
			{
				tSock->SysMessage( 393 ); // That is too far away
			}
			break;
		case 0x0018:	// Dismiss (Hireling)
			if( ObjInRange( mChar, targChar, 12 ))
			{
				// Set a tag on the player to reference the pet they are commanding
				TAGMAPOBJECT targCharSerial;
				targCharSerial.m_Destroy = false;
				targCharSerial.m_IntValue = targChar->GetSerial();
				targCharSerial.m_ObjectType = TAGMAP_TYPE_INT;
				targCharSerial.m_StringValue = "";
				mChar->SetTag( "hirelingObj", targCharSerial );

				tSock->ClearTrigWords();
				tSock->AddTrigWord( TW_DISMISS );
				std::string targName = GetNpcDictName( targChar, tSock, NRS_SYSTEM );
				WhichResponse( tSock, mChar, targName, targChar );
			}
			else
			{
				tSock->SysMessage( 393 ); // That is too far away
			}
			break;
		case 0x0019:	// Claim All Pets (Stablemaster)
			if( ObjInRange( mChar, targChar, 8 ))
			{
				tSock->ClearTrigWords();
				tSock->AddTrigWord( TW_CLAIM );
				std::string targName = GetNpcDictName( targChar, tSock, NRS_SYSTEM );
				WhichResponse( tSock, mChar, targName, targChar );
			}
			else
			{
				tSock->SysMessage( 393 ); // That is too far away
			}
			break;
		case 0x001a:	// Stable Pet (Stablemaster)
			if( ObjInRange( mChar, targChar, 8 ))
			{
				tSock->ClearTrigWords();
				tSock->AddTrigWord( TW_STABLE );
				std::string targName = GetNpcDictName( targChar, tSock, NRS_SYSTEM );
				WhichResponse( tSock, mChar, targName, targChar );
			}
			else
			{
				tSock->SysMessage( 393 ); // That is too far away
			}
			break;
		case 0x001b:	// Ask Destination (NPC Escort)
			if( ObjInRange( mChar, targChar, 3 ))
			{
				tSock->ClearTrigWords();
				tSock->AddTrigWord( TW_QUESTDEST );
				std::string targName = GetNpcDictName( targChar, tSock, NRS_SYSTEM );
				WhichResponse( tSock, mChar, targName, targChar );
			}
			else
			{
				tSock->SysMessage( 393 ); // That is too far away
			}
			break;
		case 0x001c:	// Accept Escort (NPC Escort)
			if( ObjInRange( mChar, targChar, 3 ))
			{
				tSock->ClearTrigWords();
				tSock->AddTrigWord( TW_QUESTTAKE );
				std::string targName = GetNpcDictName( targChar, tSock, NRS_SYSTEM );
				WhichResponse( tSock, mChar, targName, targChar );
			}
			else
			{
				tSock->SysMessage( 393 ); // That is too far away
			}
			break;
		case 0x001d:	// Abandon Escort (NPC Escort)
			if( targChar->IsNpc() && targChar->GetQuestType() == 0xFF && targChar->GetOwnerObj() == mChar )
			{
				targChar->SetNpcWander( WT_FREE );		// Wander freely
				targChar->SetFTarg( nullptr );			// Reset follow target
				targChar->SetQuestType( 0 );			// Reset quest type
				targChar->SetQuestDestRegion( 0 );		// Reset quest destination region
				// Set a timer to automatically delete the NPC
				targChar->SetTimer( tNPC_SUMMONTIME, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_ESCORTDONE ));
				targChar->SetOwner( nullptr );
				targChar->TextMessage( nullptr, 1797, TALK, false ); // Send out the rant about being abandoned
			}
			break;
		case 0x001e:	// Hire (Hireling)
			if( targChar->IsNpc() && targChar->CanBeHired() )
			{
				if( ObjInRange( mChar, targChar, 3 ))
				{
					tSock->ClearTrigWords();
					tSock->AddTrigWord( TW_HIRE );
					std::string targName = GetNpcDictName( targChar, tSock, NRS_SYSTEM );
					WhichResponse( tSock, mChar, targName, targChar );
				}
				else
				{
					tSock->SysMessage( 393 ); // That is too far away
				}
			}
			break;
		case 0x001f:	// Remove Friend (Pet/Hireling)
			if( ObjInRange( mChar, targChar, 12 ))
			{
				// Store reference to pet in tempObj
				tSock->TempObj( targChar );

				tSock->SendTargetCursor( 0, TARGET_REMOVEFRIEND, 0, 1852 ); // Select player to remove as friend:
			}
			else
			{
				tSock->SysMessage( 393 ); // That is too far away
			}
			break;
		case 0x0100:	// Bulk Order Info
			if( ObjInRange( mChar, targChar, 8 ))
			{
				// Set a tag on the player to reference the NPC they're requesting bulk order from
				TAGMAPOBJECT targCharSerialTag;
				targCharSerialTag.m_Destroy = false;
				targCharSerialTag.m_IntValue = targChar->GetSerial();
				targCharSerialTag.m_ObjectType = TAGMAP_TYPE_INT;
				targCharSerialTag.m_StringValue = "";
				mChar->SetTempTag( "bodShopkeeperSerial", targCharSerialTag );

				tSock->ClearTrigWords();
				tSock->AddTrigWord( TW_BODINFO ); // Custom UOX3 trigger word
				std::string targName = GetNpcDictName( targChar, tSock, NRS_SYSTEM );
				WhichResponse( tSock, mChar, targName, targChar );
			}
			else
			{
				tSock->SysMessage( 393 ); // That is too far away
			}
			break;
		default:
			Console.Print( oldstrutil::format( "Popup Menu Selection Called, Player: 0x%X Selection: 0x%X\n", tSock->GetDWord( 5 ), tSock->GetWord( 9 )));
			break;
	}

	// Let's handle skill training
	if(( popupEntry >= 0x006d && popupEntry <= 0x006f ) || ( popupEntry >= 0x006D && popupEntry <= 0x009c ) 
		|| popupEntry == 0x154 || ( popupEntry >= 0x017c && popupEntry <= 0x017e ))
	{
		tSock->ClearTrigWords();
		tSock->AddTrigWord( static_cast<TriggerWords>( popupEntry ));
		std::string targName = GetNpcDictName( targChar, tSock, NRS_SYSTEM );
		WhichResponse( tSock, mChar, targName, targChar );
	}

	return true;
}
void CPIPopupMenuSelect::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[RECV]Packet   : CPISubcommands 0xBF Subpacket Popup Menu Select --> Length: " << tSock->GetWord( 1 ) << TimeStamp() << std::endl;
	}
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIExtendedStats()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with request to lock/unlock extended stats
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xBF (General Information Packet)
//|					Subcommand: 0x1A (Extended stats)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd (0xBF)
//|						BYTE[2] Length
//|						BYTE[2] Subcommand (0x1A)
//|						Subcommand details
//|							BYTE stat // 0: str, 1: dex, 2:int
//|							BYTE status // 0: up, 1:down, 2: locked
//o------------------------------------------------------------------------------------------------o
CPIExtendedStats::CPIExtendedStats()
{
}
CPIExtendedStats::CPIExtendedStats( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}

void CPIExtendedStats::Receive( void )
{
	statToSet	= tSock->GetByte( 5 ) + ( ALLSKILLS + 1 );
	value		= tSock->GetByte( 6 );
}
bool CPIExtendedStats::Handle( void )
{
	CChar *ourChar	= tSock->CurrcharObj();
	ourChar->SetSkillLock( static_cast<SkillLock>( value ), statToSet );
	return true;
}
void CPIExtendedStats::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[RECV]Packet   : CPISubcommands 0xBF Subpacket Extended Stats --> Length: " << tSock->GetWord( 1 ) << TimeStamp() << std::endl;
	}
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIBandageMacro()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with bandage macro request
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xBF (General Information Packet)
//|					Subcommand: 0x1C (Bandage Macro/Use targeted item)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd (0xBF)
//|						BYTE[2] Length
//|						BYTE[2] Subcommand (0x1C)
//|						Subcommand details
//|							Byte[4] Item Serial
//|							Byte[4] Target Serial
//o------------------------------------------------------------------------------------------------o
CPIBandageMacro::CPIBandageMacro()
{
}
CPIBandageMacro::CPIBandageMacro( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}

void CPIBandageMacro::Receive(void)
{
	Handle();
}
bool CPIBandageMacro::Handle(void)
{
	CChar *ourChar = tSock->CurrcharObj();
	if( ValidateObject( ourChar ))
	{
		// Check if onUseBandageMacro event exists
		std::vector<UI16> scriptTriggers = ourChar->GetScriptTriggers();
		for( auto i : scriptTriggers )
		{
			cScript *toExecute = JSMapping->GetScript( i );
			if( toExecute != nullptr )
			{
				SERIAL bandageSerial = tSock->GetDWord( 5 );
				SERIAL targetSerial = tSock->GetDWord( 9 );

				if( bandageSerial == INVALIDSERIAL )
				{
					Console << "Bandage Macro detected, but no bandage found!\n";
					return false;
				}

				if( targetSerial == INVALIDSERIAL )
				{
					Console << "Bandage Macro detected, but no target found!\n";
					return false;
				}

				CItem *bandageItem = CalcItemObjFromSer( bandageSerial );
				CChar *targetChar = CalcCharObjFromSer( targetSerial );

				if( !ValidateObject( bandageItem ) || !ValidateObject( targetChar ))
					return false;

				if( toExecute->OnUseBandageMacro( tSock, targetChar, bandageItem ) == 1 )	// if it exists and we don't want hard code, return
				{
					return true;
				}
			}				
		}
	}
	return false;
}

void CPIBandageMacro::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[RECV]Packet   : CPISubcommands 0xBF Subpacket Bandage Macro --> Length: " << tSock->GetWord( 1 ) << TimeStamp() << std::endl;
	}
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIClosedStatusGump()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with bandage macro request
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xBF (General Information Packet)
//|					Subcommand: 0x0C (Closed Status Gump)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd (0xBF)
//|						BYTE[2] Length
//|						BYTE[2] Subcommand (0x0C)
//|						Subcommand details
//|							Byte[4] Character Serial
//o------------------------------------------------------------------------------------------------o
CPIClosedStatusGump::CPIClosedStatusGump()
{
}
CPIClosedStatusGump::CPIClosedStatusGump( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}

void CPIClosedStatusGump::Receive( void )
{
	Handle();
}
bool CPIClosedStatusGump::Handle( void )
{
	CChar *ourChar = tSock->CurrcharObj();
	if( ValidateObject( ourChar ))
	{
		SERIAL statusGumpCharSerial = tSock->GetDWord( 5 );
		CChar *statusGumpChar = CalcCharObjFromSer( statusGumpCharSerial );
		if( ValidateObject( statusGumpChar ))
		{
			//tSock->StatWindow( statusGumpChar ); // Send statwindow update on closing the gump? Why? Doesn't seem like it's needed...
			return true;
		}
	}
	return false;
}

void CPIClosedStatusGump::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[RECV]Packet   : CPISubcommands 0xBF Subpacket Bandage Macro --> Length: " << tSock->GetWord( 1 ) << TimeStamp() << std::endl;
	}
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIToggleFlying()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Toggles flying on/off for gargoyle characters
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xBF (General Information Packet)
//|					Subcommand: 0x32 (Toggle Gargoyle Flying)
//|					Size: 11 bytes?
//|
//|					Packet Build
//|						BYTE cmd (0xBF)
//|						BYTE[2] Length
//|						BYTE[2] Subcommand (0x32)
//|						Subcommand details
//|							Byte[4] unk1 (always 0x0100)
//|							Byte[2] unk2(always 0x0)
//o------------------------------------------------------------------------------------------------o
CPIToggleFlying::CPIToggleFlying()
{
}
CPIToggleFlying::CPIToggleFlying( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}

void CPIToggleFlying::Receive(void)
{
	Handle();
}
bool CPIToggleFlying::Handle(void)
{
	CChar *ourChar = tSock->CurrcharObj();
	if( ValidateObject( ourChar ))
	{
		ourChar->ToggleFlying();
	}
	return true;
}

void CPIToggleFlying::Log(std::ostream &outStream, bool fullHeader)
{
	if( fullHeader )
	{
		outStream << "[RECV]Packet   : CPISubcommands 0xBF Subpacket Toggle Gargoyle Flying --> Length: " << tSock->GetWord( 1 ) << TimeStamp() << std::endl;
	}
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIKrriosClientSpecial()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet for party and guild tracking, assistant feature
//|					negotiation, and potentially other purposes (like new movement mode)
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xF0 (KrriosClientSpecial)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] len
//|						BYTE subcmd
//|							0x00 - Party info
//|							0x01 - Guild Tracker
//|								BYTE unknown // always 1?
//|							0xff - Response to server's request for Razor to negotiate features
//|								Byte 0 == 0xf0
//|								Byte 1 == 0x00
//|								Byte 2 == 0x04
//|								Byte 3 == 0xff
//|									If both ASSISTANTNEGOTIATION and KICKONASSISTANTSILENCE uox.ini
//|									settings are enabled, player will get kicked within 30 seconds
//|									if this packet is not received by the server.
//o------------------------------------------------------------------------------------------------o
void CPIKrriosClientSpecial::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[RECV]Packet   : CPKrriosClientSpecial 0xF0 --> Length: " << tSock->GetWord( 1 ) << TimeStamp() << std::endl;
	}
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}
void CPIKrriosClientSpecial::InternalReset( void )
{
}
CPIKrriosClientSpecial::CPIKrriosClientSpecial()
{
	InternalReset();
}
CPIKrriosClientSpecial::CPIKrriosClientSpecial( CSocket *s ) : CPInputBuffer( s )
{
	InternalReset();
	Receive();
}
void CPIKrriosClientSpecial::Receive( void )
{
	tSock->Receive( 4, false );
	blockLen = tSock->GetWord( 1 );
	tSock->Receive( blockLen, false );

	if( blockLen > 5 )
	{
		// New Movement Request Packet
		type = 0xaa;
	}
	else
	{
		type = tSock->GetByte( 3 );
	}
}
bool CPIKrriosClientSpecial::Handle( void )
{
	locations = false;

	switch( type )
	{
		case 0x00: // custom party info
		case 0x01: // guild track info
		{
			// If ini-setting for worldmap packets is enabled
			if( cwmWorldState->ServerData()->GetClassicUOMapTracker() )
			{
				CChar *mChar = tSock->CurrcharObj();

				if( type == 0x00 )
				{
					// Character is not in a party - nothing to track
					if( !mChar->InParty() )
						break;
				}
				else if( type == 0x01 )
				{
					locations = ( blockLen == 5 ? static_cast<bool>( tSock->GetByte( 4 )) : false );

					// Character is not in a guild - nothing to track
					if( mChar->GetGuildNumber() == -1 )
					{
						break;
					}

					const CGuild * const mGuild = GuildSys->Guild( mChar->GetGuildNumber() );
					if( mGuild != nullptr )
					{
						// Count the number of recruits and regular members, only proceeed
						// if there's more than one (that would be the player themselves)
						size_t numRecruits = mGuild->NumRecruits();
						size_t numMembers = mGuild->NumMembers();
						if( numRecruits + numMembers <= 1 )
							break;
					}
				}

				// Construct the response, and send it!
				CPKrriosClientSpecial krriosResponse( tSock, mChar, type, locations );
				tSock->Send( &krriosResponse );
			}
			break;
		}
		case 0xaa: // New Movement Request Packet
			break;
		case 0xff: // razor feature negotiation
		{
			if( tSock->GetByte( 1 ) == 0x00 && tSock->GetByte( 2 ) == 0x04 )
			{
				tSock->NegotiatedWithAssistant( true );
				tSock->NegotiateTimeout( -1 );
				tSock->SysMessage( 9010 ); // Features successfully negotiated with assistant tool.
				return true;
			}
			break;
		}
		default: // default
			break;
	}

	return locations;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPISpellbookSelect()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with info on selected spell from spellbook
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xBF (General Information Packet)
//|					Subcommand: 0x1D (Spell selected)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd (0xBF)
//|						BYTE[2] Length
//|						BYTE[2] Subcommand (0x1D)
//|						Subcommand details
//|							BYTE[2] unknown, always 2
//|							BYTE[2] selected spell(0-indexed)+scroll offset from sub 0x1b
//o------------------------------------------------------------------------------------------------o
CPISpellbookSelect::CPISpellbookSelect()
{
}
CPISpellbookSelect::CPISpellbookSelect( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}

void CPISpellbookSelect::Receive( void )
{
}
bool CPISpellbookSelect::Handle( void )
{
    CChar *ourChar = tSock->CurrcharObj();
    CItem *sBook = FindItemOfType( ourChar, IT_SPELLBOOK );
    CItem *pBook = FindItemOfType( ourChar, IT_PALADINBOOK );
	CItem *nBook = FindItemOfType(ourChar, IT_NECROBOOK);
    CItem *packItem = ourChar->GetPackItem();
    UI08 *buffer = tSock->Buffer();

    if( !ValidateObject( sBook ) && !ValidateObject( pBook ))
    {
        tSock->SysMessage( 765 ); // "To cast spells, your spellbook must be in your hands or in the first layer of your pack."
        return true;
    }

    // Validate location
	bool validLoc = 
	    ( ValidateObject( sBook ) && 
	     (sBook->GetCont() == ourChar || ( ValidateObject(packItem ) && sBook->GetCont() == packItem ))) ||
	    ( ValidateObject( pBook ) && 
	     (pBook->GetCont() == ourChar || ( ValidateObject(packItem ) && pBook->GetCont() == packItem ))) ||
	    ( ValidateObject( nBook ) && 
	     ( nBook->GetCont() == ourChar || ( ValidateObject(packItem ) && nBook->GetCont() == packItem )));

    if( !validLoc )
    {
        tSock->SysMessage( 765 ); // "Your spellbook must be equipped or in your pack."
        return true;
    }

	// Determine the spellbook type and offset
	int offset = 0;
	CItem *activeBook = nullptr;

	// Use Buffer[8] to determine the spell number
	int spellNumber = buffer[8];

	if( ValidateObject( pBook ) && spellNumber >= 201 && spellNumber <= 210 ) // Paladin spells
	{
	    activeBook = pBook;
	    offset = 200; // Paladin spells offset starts from 201
	}
	else if( ValidateObject( nBook ) && spellNumber >= 101 && spellNumber <= 117 ) // Necromancer spells
	{
	    activeBook = nBook;
	    offset = 100; // Necromancer spells offset starts from 101
	}
	else if( ValidateObject( sBook ) && spellNumber < 201 ) // Regular spells
	{
	    activeBook = sBook;
	    offset = 0; // Regular spells offset starts from 1
	}

	if( !ValidateObject( activeBook ))
	{
	    tSock->SysMessage(764); // "You do not have that spell."
	    return true;
	}

    // Calculate the actual spell number
    SI32 book = spellNumber - offset;

    // Validate the spell in the book
    if( Magic->CheckBook((( book - 1 ) / 8 ) + 1, ( book - 1 ) % 8, activeBook ))
    {
        if( ourChar->IsFrozen() )
        {
            if( ourChar->IsCasting() )
            {
                tSock->SysMessage( 762 ); // "You are already casting a spell."
            }
            else
            {
                tSock->SysMessage( 763 ); // "You cannot cast spells while frozen."
            }
        }
        else
        {
            tSock->CurrentSpellType(0);
            Magic->SelectSpell( tSock, book + offset ); // Ensure the correct spell is cast
        }
    }
    else
    {
        tSock->SysMessage( 764 ); // "You do not have that spell."
    }

    return true;
}
void CPISpellbookSelect::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[RECV]Packet   : CPISubcommands 0xBF Subpacket Spellbook Select --> Length: " << tSock->GetWord( 1 ) << TimeStamp() << std::endl;
	}
	outStream << "  Raw dump     :" << std::endl;
	CPInputBuffer::Log( outStream, false );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIAOSCommand()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles incoming packet with requests for various AoS features
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xD7 (Generic AOS Commands)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[4] id
//|						BYTE[30] new name
//|
//|						BYTE cmd
//|						BYTE[2]	length
//|						BYTE[4] player serial
//|						BYTE[2] subcommand
//|							0x02 - House Customization :: Backup
//|								BYTE unknown (0x07)
//|							0x03 - House Customization :: Restore
//|								BYTE unknown (0x07)
//|							0x04 - House Customization :: Commit
//|								BYTE unknown (0x07)
//|							0x05 - House Customization :: Destroy Item
//|								BYTE unknown (0)
//|								BYTE[2] unknown (0)
//|								BYTE[2] item graphic
//|								BYTE unknown (0)
//|								BYTE[4] X Pos
//|								BYTE unknown (0)
//|								BYTE[4] Y Pos
//|								BYTE unknown (0)
//|								BYTE[4] z Pos (7)
//|								BYTE terminator (0x07)
//|							0x06 - House Customization :: Place Item
//|								BYTE unknown (0)
//|								BYTE[2] unknown (0)
//|								BYTE[2] itemGraphic
//|								BYTE unknown (0)
//|								BYTE[4] XPos
//|								BYTE unknown (0)
//|								BYTE[4] YPos
//|								BYTE terminator (0x07)
//|							0x0C - House Customization :: Exit
//|								BYTE unknown (0x07)
//|							0x0D - House Customization :: Place Multi (Stairs)
//|								BYTE unknown (0x00)
//|								BYTE[2] X Position (Relative to center of house)
//|								BYTE unknown (0x00)
//|								BYTE[2] Y Position (Relative to center of house)
//|								BYTE unknown (0x00)
//|							0x0E - House Customization :: Synch
//|								BYTE unknown (0x07)
//|							0x10 - House Customization :: Clear
//|								BYTE unknown (0x07)
//|							0x12 - House Customization :: Switch Floors
//|								BYTE[4] unknown (0)
//|								BYTE Floor#
//|								BYTE terminator (0x07)
//|							0x19 - Special Moves :: Activate/Deactivate
//|								Sent when client pushes the icones from the combat book.
//|								The server uses an 0xBF Subcommand 0x21 Packet to cancel the red color of
//|								icons, and reset the status of them on client.
//|								Valid Ability Numbers:
//|									0x00 = Cancel Ability Attempt
//|									0x01 = Armor Ignore
//|									0x02 = Bleed Attack
//|									0x03 = Concusion Blow
//|									0x04 = Crushing Blow
//|									0x05 = Disarm
//|									0x06 = Dismount
//|									0x07 = Double Strike
//|									0x08 = Infecting
//|									0x09 = Mortal Strike
//|									0x0A = Moving Shot
//|									0x0B = Paralyzing Blow
//|									0x0C = Shadow Strike
//|									0x0D = Whirlwind Attack
//|									0x0E = Riding Swipe
//|									0x0F = Frenzied Whirlwind
//|									0x10 = Block
//|									0x11 = Defense Mastery
//|									0x12 = Nerve Strike
//|									0x13 = Talon Strike
//|									0x14 = Feint
//|									0x15 = Dual Wield
//|									0x16 = Double shot
//|									0x17 = Armor Peirce
//|									0x18 = Bladeweave
//|									0x19 = Force Arrow
//|									0x1A = Lightning Arrow
//|									0x1B = Psychic Attack
//|									0x1C = Serpent Arrow
//|									0x1D = Force of Nature
//|							0x1A - House Customization :: Revert
//|								BYTE unknown (0x07)
//|							0x28 - Quests :: Paperdoll button
//|								BYTE unknown (0x07)
//|							0x32 - Guild  :: Paperdoll button
//|								BYTE unknown (0x07)
//o------------------------------------------------------------------------------------------------o
CPIAOSCommand::CPIAOSCommand()
{
}
CPIAOSCommand::CPIAOSCommand( CSocket *s ) : CPInputBuffer( s )
{
	Receive();
}

void CPIAOSCommand::Receive( void )
{
	tSock->Receive( 3, false );
	UI16 len = tSock->GetWord( 1 );
	tSock->Receive( len, false );
}
bool CPIAOSCommand::Handle( void )
{
	switch( tSock->GetWord( 7 ))	// Which subcommand?
	{
			/*case 0x0002:	break;	//House Customisation :: Backup
			 case 0x0003:	break;	//House Customisation :: Restore
			 case 0x0004:	break;	//House Customisation :: Commit
			 case 0x0005:	break;	//House Customisation :: Destroy Item
			 case 0x0006:	break;	//House Customisation :: Place Item
			 case 0x000C:	break;	//House Customisation :: Exit
			 case 0x000D:	break;	//House Customisation :: Place Multi (Stairs)
			 case 0x000E:	break;	//House Customisation :: Synch
			 case 0x0010:	break;	//House Customisation :: Clear
			 case 0x0012:	break;	//House Customisation :: Switch Floors*/
		case 0x0019: //Special Moves :: Activate / Deactivate
		{
			//UI32 unknown = tSock->GetDWord( 9 );
			UI08 abilityId = tSock->GetByte( 13 );
			//UI08 unknown2 = tSock->GetByte( 15 );

			CChar *myChar = tSock->CurrcharObj();
			std::vector<UI16> scriptTriggers = myChar->GetScriptTriggers();
			for( auto i : scriptTriggers )
			{
				cScript *toExecute = JSMapping->GetScript( i );
				if( toExecute != nullptr )
				{
					if( toExecute->OnSpecialMove( myChar, abilityId ) == 1 )
					{
						return true;
					}
				}
			}

			// No individual scripts handling OnSpecialMove return true - let's check global script!
			cScript *toExecute = JSMapping->GetScript( static_cast<UI16>( 0 ));
			if( toExecute != nullptr )
			{
				toExecute->OnSpecialMove( myChar, abilityId );
			}
			return true;
		}
		/*case 0x001A:	break;	//House Customisation :: Revert*/
		case 0x0028:			//Guild :: Paperdoll button
			if( cwmWorldState->ServerData()->PaperdollGuildButton() )
			{
				if( tSock->CurrcharObj()->GetGuildNumber() != - 1 )
				{
					GuildSys->Menu( tSock, BasePage + 1, static_cast<GUILDID>( tSock->CurrcharObj()->GetGuildNumber() ));
				}
				else
				{
					tSock->SysMessage( 1793 ); // You are not currently in a guild. Go buy a guildstone!
				}
				return true;
			}
			break;
		case 0x0032:			//Quests :: Unknown
		{
			CChar *myChar	= tSock->CurrcharObj();
			std::vector<UI16> scriptTriggers = myChar->GetScriptTriggers();
			for( auto scriptTrig : scriptTriggers )
			{
				cScript *toExecute = JSMapping->GetScript( scriptTrig );
				if( toExecute != nullptr )
				{
					if( toExecute->OnQuestGump( myChar ) == 1 )
					{
						return true;
					}
				}
			}

			// No individual scripts handling OnQuestGump returned true - let's check global script!
			cScript *toExecute = JSMapping->GetScript( static_cast<UI16>( 0 ));
			if( toExecute != nullptr )
			{
				toExecute->OnQuestGump( myChar );
			}
			return true;
		}
		default:		break;
	}

	return false;
}
void CPIAOSCommand::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[RECV]Packet   : CPIAOSCommand 0xD7 --> Length: " << tSock->GetWord( 1 ) << TimeStamp() << std::endl;
	}
	outStream << "Character      : 0x" << std::hex << tSock->GetDWord( 3 ) << std::dec << std::endl;
	outStream << "Command        : ";
	switch( tSock->GetWord( 7 ))
	{
		case 0x0002:	outStream << "House Customisation :: Backup";					break;
		case 0x0003:	outStream << "House Customisation :: Restore";					break;
		case 0x0004:	outStream << "House Customisation :: Commit";					break;
		case 0x0005:	outStream << "House Customisation :: Destroy Item";				break;
		case 0x0006:	outStream << "House Customisation :: Place Item";				break;
		case 0x000C:	outStream << "House Customisation :: Exit";						break;
		case 0x000D:	outStream << "House Customisation :: Place Multi (Stairs)";		break;
		case 0x000E:	outStream << "House Customisation :: Synch";					break;
		case 0x0010:	outStream << "House Customisation :: Clear";					break;
		case 0x0012:	outStream << "House Customisation :: Switch Floors";			break;
		case 0x0019:	outStream << "Special Moves :: Activate / Deactivate";			break;
		case 0x001A:	outStream << "House Customisation :: Revert";					break;
		case 0x0028:	outStream << "Guild :: Unknown";								break;
		case 0x0032:	outStream << "Quests :: Unknown";								break;
		default:		outStream << "Unknown (0x" << std::hex << tSock->GetWord( 7 ) << std::dec << ")";			break;
	}
	outStream << std::endl;
	outStream << "  Raw dump     : " << std::endl;
	CPInputBuffer::Log( outStream, false );
}
