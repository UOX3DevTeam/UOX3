#include "uox3.h"

#ifdef __LINUX__
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <errno.h>
#endif

const UI32 DefaultClientVersion = calcserial( 2, 0, 0, 0 );
const ClientTypes DefaultClientType = CV_NORMAL;

//	1.0		Abaddon		29th November, 2000
//			Initial implementation
//			Stores almost all information currently separated into different vars
//			Also has logging support, and non-blocking IO support
//			Makes use of a socket_error exception class

const bool LOGDEFAULT = false;

long socket_error::ErrorNumber( void ) const
{
	return errorNum;
}

const char *socket_error::what( void ) const throw()
{
	return runtime_error::what();
}

socket_error::socket_error( void ) : errorNum( -1 ), runtime_error( "" )
{
}

socket_error::socket_error( const std::string& what_arg ) : errorNum( -1 ), runtime_error( what_arg )
{
}

socket_error::socket_error( const long errorNumber ) : errorNum( errorNumber ), runtime_error( "" )
{
}

//o---------------------------------------------------------------------------o
//|   Function    -  int cSocket::CliSocket( void )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the socket identifier for our socket
//o---------------------------------------------------------------------------o
int cSocket::CliSocket( void ) const
{
	return cliSocket;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void cSocket::CliSocket( int newValue )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Assigns a new socket value
//o---------------------------------------------------------------------------o
void cSocket::CliSocket( int newValue )
{
	cliSocket = newValue;
	UI32 mode = 1;
	// set the socket to nonblocking
	ioctlsocket( cliSocket, FIONBIO, &mode );
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool cSocket::CryptClient( void )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the true if the socket is set to crypt mode
//o---------------------------------------------------------------------------o
bool cSocket::CryptClient( void ) const
{
	return cryptclient;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void cSocket::CryptClient( bool newValue )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the value of the socket's crypt mode
//o---------------------------------------------------------------------------o
void cSocket::CryptClient( bool newValue )
{
	cryptclient = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  char *cSocket::XText( void )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the socket's xtext buffer
//o---------------------------------------------------------------------------o
char *cSocket::XText( void )
{
	return &xtext[0];
}

void cSocket::XText( const char *newValue )
{
	strncpy( xtext, newValue, 64 );
	xtext[64] = 0;
}

//o---------------------------------------------------------------------------o
//|   Function    -  int cSocket::AddMItem( void )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the socket's addmitem
//o---------------------------------------------------------------------------o
SI32 cSocket::AddMItem( void ) const
{
	return addmitem;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void cSocket::AddMItem( int newValue )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the socket's addmitem
//o---------------------------------------------------------------------------o
void cSocket::AddMItem( SI32 newValue )
{
	addmitem = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 cSocket::AddZ( void )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Return's the socket's addz
//o---------------------------------------------------------------------------o
SI08 cSocket::AddZ( void ) const
{
	return addz;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void cSocket::AddZ( SI08 newValue )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the socket's addz
//o---------------------------------------------------------------------------o
void cSocket::AddZ( SI08 newValue )
{
	addz = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  int cSocket::AddX( void )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the addx associated with the socket
//o---------------------------------------------------------------------------o
SI32 cSocket::AddX( void ) const
{
	return addx[0];
}

//o---------------------------------------------------------------------------o
//|   Function    -  void cSocket::AddX( int newValue )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the socket's addx value
//o---------------------------------------------------------------------------o
void cSocket::AddX( SI32 newValue )
{
	addx[0] = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  int cSocket::AddY( void )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the addy associated with the socket
//o---------------------------------------------------------------------------o
SI32 cSocket::AddY( void ) const
{
	return addy[0];
}

//o---------------------------------------------------------------------------o
//|   Function    -  void cSocket::AddY( int newValue )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the socket's addy value
//o---------------------------------------------------------------------------o
void cSocket::AddY( SI32 newValue )
{
	addy[0] = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  int cSocket::AddX2( void )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the second addx associated with the socket
//o---------------------------------------------------------------------------o
SI32 cSocket::AddX2( void ) const
{
	return addx[1];
}

//o---------------------------------------------------------------------------o
//|   Function    -  void cSocket::AddX2( int newValue )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the socket's second addx value
//o---------------------------------------------------------------------------o
void cSocket::AddX2( SI32 newValue )
{
	addx[1] = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  int cSocket::AddY2( void )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the socket's second addy value
//o---------------------------------------------------------------------------o
SI32 cSocket::AddY2( void ) const
{
	return addy[1];
}

//o---------------------------------------------------------------------------o
//|   Function    -  void cSocket::AddY2( int newValue )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the socket's second addy value
//o---------------------------------------------------------------------------o
void cSocket::AddY2( SI32 newValue )
{
	addy[1] = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI08 cSocket::AddID1( void )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the first addid associated with the socket
//o---------------------------------------------------------------------------o
UI08 cSocket::AddID1( void ) const
{
	return addid[0];
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI08 cSocket::AddID2( void )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the second addid associated with the socket
//o---------------------------------------------------------------------------o
UI08 cSocket::AddID2( void ) const
{
	return addid[1];
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI08 cSocket::AddID3( void )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the third addid associated with the socket
//o---------------------------------------------------------------------------o
UI08 cSocket::AddID3( void ) const
{
	return addid[2];
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI08 cSocket::AddID4( void )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the fourth addid associated with the socket
//o---------------------------------------------------------------------------o
UI08 cSocket::AddID4( void ) const
{
	return addid[3];
}

//o---------------------------------------------------------------------------o
//|   Function    -  void cSocket::AddID1( UI08 newValue )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the first addid associated with the socket
//o---------------------------------------------------------------------------o
void cSocket::AddID1( UI08 newValue )
{
	addid[0] = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void cSocket::AddID2( UI08 newValue )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the second addid associated with the socket
//o---------------------------------------------------------------------------o
void cSocket::AddID2( UI08 newValue )
{
	addid[1] = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void cSocket::AddID3( UI08 newValue )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the third addid associated with the socket
//o---------------------------------------------------------------------------o
void cSocket::AddID3( UI08 newValue )
{
	addid[2] = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void cSocket::AddID4( UI08 newValue )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the fourth addid associated with the socket
//o---------------------------------------------------------------------------o
void cSocket::AddID4( UI08 newValue )
{
	addid[3] = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  char cSocket::DyeAll( void )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the dyeall status of a socket
//o---------------------------------------------------------------------------o
char cSocket::DyeAll( void ) const
{
	return dyeall;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void cSocket::DyeAll( char newValue )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the dyeall status of a socket
//o---------------------------------------------------------------------------o
void cSocket::DyeAll( char newValue )
{
	dyeall = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void cSocket::CloseSocket( void )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Closes the open socket
//o---------------------------------------------------------------------------o
void cSocket::CloseSocket( void )
{
	closesocket( cliSocket );
}

//o---------------------------------------------------------------------------o
//|   Function    -  CHARACTER cSocket::Currchar( void )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the game character associated with the current socket
//o---------------------------------------------------------------------------o
CHARACTER cSocket::Currchar( void ) const
{
	return currchar;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void cSocket::Currchar( CHARACTER newChar )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the game character associated with the current socket
//o---------------------------------------------------------------------------o
void cSocket::Currchar( CHARACTER newChar )
{
	currchar = newChar;
	if( currchar != INVALIDSERIAL )
		currCharObj = &chars[currchar];
	else
		currCharObj = NULL;
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool cSocket::FirstPacket( void )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if it's the first packet received
//o---------------------------------------------------------------------------o
bool cSocket::FirstPacket( void ) const
{
	return firstPacket;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void cSocket::FirstPacket( bool newValue )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets whether the socket has received it's first packet yet
//o---------------------------------------------------------------------------o
void cSocket::FirstPacket( bool newValue )
{
	firstPacket = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI32 cSocket::IdleTimeout( void )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the time point at which the char times out
//o---------------------------------------------------------------------------o
SI32 cSocket::IdleTimeout( void ) const
{
	return idleTimeout;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void cSocket::IdleTimeout( long newValue )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the time point at which the char times out
//o---------------------------------------------------------------------------o
void cSocket::IdleTimeout( SI32 newValue )
{
	idleTimeout = newValue;
	wasIdleWarned = false;
}

bool cSocket::WasIdleWarned( void ) const
{
	return wasIdleWarned;
}

void cSocket::WasIdleWarned( bool value )
{
	wasIdleWarned = value;
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI08 *cSocket::Buffer( void )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns a pointer to the buffer of the socket
//o---------------------------------------------------------------------------o
UI08 *cSocket::Buffer( void )
{
	return &buffer[0];
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI08 *cSocket::OutBuffer( void )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns a pointer to the outgoing buffer of the socket
//o---------------------------------------------------------------------------o
UI08 *cSocket::OutBuffer( void )
{
	return &outbuffer[0];
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI08 *cSocket::TBuffer( void )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns a pointer to the socket's TBuffer
//o---------------------------------------------------------------------------o
UI08 *cSocket::TBuffer( void )
{
	return &tbuffer[0];
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI16 cSocket::WalkSequence( void )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the point in the walk sequence of the socket
//o---------------------------------------------------------------------------o
SI16 cSocket::WalkSequence( void ) const
{
	return walkSequence;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void cSocket::WalkSequence( SI16 newValue )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the walk sequence value of the socket
//o---------------------------------------------------------------------------o
void cSocket::WalkSequence( SI16 newValue )
{
	walkSequence = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  int cSocket::AcctNo( void )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the ID of the account number socket belongs to
//o---------------------------------------------------------------------------o
SI32 cSocket::AcctNo( void ) const
{
	return wAccountID;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void cSocket::AcctNo( int newValue )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the ID of the account number the socket belongs to
//o---------------------------------------------------------------------------o
void cSocket::AcctNo( SI32 newValue )
{
	if(!Accounts->GetAccountByID(newValue,(ACCOUNTSBLOCK&)actbAccount))
	{
		wAccountID = AB_INVALID_ID;
		actbAccount.wAccountIndex=AB_INVALID_ID;
		return;
	}
	wAccountID = actbAccount.wAccountIndex;
}

//o---------------------------------------------------------------------------o
//|   Function    -  int cSocket::TempInt( void )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the tempint of the socket
//o---------------------------------------------------------------------------o
SI32 cSocket::TempInt( void ) const
{
	return tempint;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void cSocket::TempInt( int newValue )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the tempint value of the socket
//o---------------------------------------------------------------------------o
void cSocket::TempInt( SI32 newValue )
{
	tempint = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 cSocket::AddID5( void )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the addid5 of the socket
//o---------------------------------------------------------------------------o
SI08 cSocket::AddID5( void ) const
{
	return addid5;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void cSocket::AddID5( SI08 newValue )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the fifth addid associated with the socket
//o---------------------------------------------------------------------------o
void cSocket::AddID5( SI08 newValue )
{
	addid5 = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  make_st& cSocket::AddID4( void )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the make_st associated with the socket
//o---------------------------------------------------------------------------o
make_st& cSocket::ItemMake( void )
{
	return itemmake;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void cSocket::AddID4( make_st& newValue )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the make_st associated with the socket
//o---------------------------------------------------------------------------o
void cSocket::ItemMake( make_st& newValue )
{
	itemmake = newValue;
}

cSocket::cSocket() : currCharObj( NULL )/*, actbAccount()*/, idleTimeout( -1 ), currchar( INVALIDSERIAL ), wAccountID(AB_INVALID_ID),
tempint( 0 ), dyeall( 0 ), addz( 0 ), addmitem( INVALIDSERIAL ), newClient( true ), firstPacket( true ), range( 15 ),
cryptclient( false ), cliSocket( INVALID_SOCKET ), walkSequence( -1 ), addid5( 0 ), currentSpellType( 0 ),
outlength( 0 ), inlength( 0 ), logging( LOGDEFAULT ), postAckCount( 0 ), postCount( 0 ), pSpot( PL_NOWHERE ), pFrom( INVALIDSERIAL ),
pX( 0 ), pY( 0 ), pZ( 0 ), lang( UT_ENU ), cliType( DefaultClientType ), clientVersion( DefaultClientVersion ), bytesReceived( 0 ), bytesSent( 0 )
{
	InternalReset();
	actbAccount.wAccountIndex=AB_INVALID_ID;
}

cSocket::~cSocket()
{
	closesocket( cliSocket );
}

//o---------------------------------------------------------------------------o
//|   Function    -  char cSocket::CurrentSpellType( void )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the current spell type of the socket
//o---------------------------------------------------------------------------o
char cSocket::CurrentSpellType( void ) const
{
	return currentSpellType;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void cSocket::CurrentSpellType( char newValue )
//|   Date        -  November 29th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the spell type of the socket
//o---------------------------------------------------------------------------o
void cSocket::CurrentSpellType( char newValue )
{
	currentSpellType = newValue;
}

bool cSocket::FlushBuffer( bool doLog )
{
	if( outlength > 0 )
	{
		int len;
		if( cryptclient )
		{
			char xoutbuffer[MAXBUFFER*2];
			len = Pack( outbuffer, xoutbuffer, outlength );
			send( cliSocket, xoutbuffer, len, 0 );
		}
		else
			send( cliSocket, (char *)&outbuffer[0], outlength, 0 );
		if( Logging() && doLog )
		{
			FILE *fp = NULL;
			char filename[MAX_PATH];
			SERIAL toPrint;
			if( currchar == INVALIDSERIAL )
				toPrint = INVALIDSERIAL;
			else
				toPrint = chars[currchar].GetSerial();
			sprintf( filename, "%s/logs/%i.snd", cwmWorldState->ServerData()->GetRootDirectory(), toPrint );
			char qbuffer[8];
			int j = 0;
			memset( qbuffer, 0x00, 8 );
			if( ( fp = fopen( filename, "a" ) ) != NULL )
			{
				fprintf( fp, "[SEND]Packet: 0x%2X --> Length: %li\n", outbuffer[0], outlength );
				for( int i = 0; i < outlength; i++ )
				{
					qbuffer[j++] = outbuffer[i];
					fprintf( fp, "0x%2X ", outbuffer[i] );
					if( j >= 7 )
					{
						fprintf( fp, "  %s\n", qbuffer );
						j = 0;
					}
				}
				fprintf( fp, "  %s\n\n", qbuffer );
				fclose( fp );
			}
			else
			{
				Console.Error( 1, "Failed to open socket log %s", filename );
			}
			bytesSent += outlength;
		}
		outlength = 0;
		return true;
	}
	else
		return false;
}

static UI32 bit_table[257][2] =
{
	0x02, 0x00, 	0x05, 0x1F, 	0x06, 0x22, 	0x07, 0x34, 	0x07, 0x75, 	0x06, 0x28, 	0x06, 0x3B, 	0x07, 0x32, 
	0x08, 0xE0, 	0x08, 0x62, 	0x07, 0x56, 	0x08, 0x79, 	0x09, 0x19D,	0x08, 0x97, 	0x06, 0x2A, 	0x07, 0x57, 
	0x08, 0x71, 	0x08, 0x5B, 	0x09, 0x1CC,	0x08, 0xA7, 	0x07, 0x25, 	0x07, 0x4F, 	0x08, 0x66, 	0x08, 0x7D, 
	0x09, 0x191,	0x09, 0x1CE, 	0x07, 0x3F, 	0x09, 0x90, 	0x08, 0x59, 	0x08, 0x7B, 	0x08, 0x91, 	0x08, 0xC6, 
	0x06, 0x2D, 	0x09, 0x186, 	0x08, 0x6F, 	0x09, 0x93, 	0x0A, 0x1CC,	0x08, 0x5A, 	0x0A, 0x1AE,	0x0A, 0x1C0, 
	0x09, 0x148,	0x09, 0x14A, 	0x09, 0x82, 	0x0A, 0x19F, 	0x09, 0x171,	0x09, 0x120, 	0x09, 0xE7, 	0x0A, 0x1F3, 
	0x09, 0x14B,	0x09, 0x100,	0x09, 0x190,	0x06, 0x13, 	0x09, 0x161,	0x09, 0x125,	0x09, 0x133,	0x09, 0x195, 
	0x09, 0x173,	0x09, 0x1CA,	0x09, 0x86, 	0x09, 0x1E9, 	0x09, 0xDB, 	0x09, 0x1EC,	0x09, 0x8B, 	0x09, 0x85, 
	0x05, 0x0A, 	0x08, 0x96, 	0x08, 0x9C, 	0x09, 0x1C3, 	0x09, 0x19C,	0x09, 0x8F, 	0x09, 0x18F,	0x09, 0x91, 
	0x09, 0x87, 	0x09, 0xC6, 	0x09, 0x177,	0x09, 0x89, 	0x09, 0xD6, 	0x09, 0x8C, 	0x09, 0x1EE,	0x09, 0x1EB, 
	0x09, 0x84, 	0x09, 0x164, 	0x09, 0x175,	0x09, 0x1CD, 	0x08, 0x5E, 	0x09, 0x88, 	0x09, 0x12B,	0x09, 0x172, 
	0x09, 0x10A,	0x09, 0x8D, 	0x09, 0x13A,	0x09, 0x11C, 	0x0A, 0x1E1,	0x0A, 0x1E0, 	0x09, 0x187,	0x0A, 0x1DC, 
	0x0A, 0x1DF,	0x07, 0x74, 	0x09, 0x19F,	0x08, 0x8D,		0x08, 0xE4, 	0x07, 0x79, 	0x09, 0xEA, 	0x09, 0xE1, 
	0x08, 0x40, 	0x07, 0x41, 	0x09, 0x10B,	0x09, 0xB0, 	0x08, 0x6A, 	0x08, 0xC1, 	0x07, 0x71, 	0x07, 0x78, 
	0x08, 0xB1, 	0x09, 0x14C, 	0x07, 0x43, 	0x08, 0x76, 	0x07, 0x66, 	0x07, 0x4D, 	0x09, 0x8A, 	0x06, 0x2F, 
	0x08, 0xC9,		0x09, 0xCE, 	0x09, 0x149,	0x09, 0x160, 	0x0A, 0x1BA, 	0x0A, 0x19E, 	0x0A, 0x39F, 	0x09, 0xE5, 
	0x09, 0x194, 	0x09, 0x184, 	0x09, 0x126, 	0x07, 0x30, 	0x08, 0x6C, 	0x09, 0x121, 	0x09, 0x1E8, 	0x0A, 0x1C1, 
	0x0A, 0x11D, 	0x0A, 0x163, 	0x0A, 0x385, 	0x0A, 0x3DB, 	0x0A, 0x17D, 	0x0A, 0x106, 	0x0A, 0x397, 	0x0A, 0x24E, 
	0x07, 0x2E, 	0x08, 0x98, 	0x0A, 0x33C, 	0x0A, 0x32E, 	0x0A, 0x1E9, 	0x09, 0xBF, 	0x0A, 0x3DF, 	0x0A, 0x1DD, 
	0x0A, 0x32D, 	0x0A, 0x2ED, 	0x0A, 0x30B, 	0x0A, 0x107, 	0x0A, 0x2E8, 	0x0A, 0x3DE, 	0x0A, 0x125, 	0x0A, 0x1E8, 
	0x09, 0xE9, 	0x0A, 0x1CD, 	0x0A, 0x1B5, 	0x09, 0x165, 	0x0A, 0x232, 	0x0A, 0x2E1, 	0x0B, 0x3AE, 	0x0B, 0x3C6, 
	0x0B, 0x3E2, 	0x0A, 0x205, 	0x0A, 0x29A, 	0x0A, 0x248, 	0x0A, 0x2CD, 	0x0A, 0x23B, 	0x0B, 0x3C5, 	0x0A, 0x251, 
	0x0A, 0x2E9, 	0x0A, 0x252, 	0x09, 0x1EA, 	0x0B, 0x3A0, 	0x0B, 0x391, 	0x0A, 0x23C, 	0x0B, 0x392, 	0x0B, 0x3D5, 
	0x0A, 0x233, 	0x0A, 0x2CC, 	0x0B, 0x390, 	0x0A, 0x1BB, 	0x0B, 0x3A1, 	0x0B, 0x3C4, 	0x0A, 0x211, 	0x0A, 0x203, 
	0x09, 0x12A, 	0x0A, 0x231, 	0x0B, 0x3E0, 	0x0A, 0x29B, 	0x0B, 0x3D7, 	0x0A, 0x202, 	0x0B, 0x3AD, 	0x0A, 0x213, 
	0x0A, 0x253, 	0x0A, 0x32C, 	0x0A, 0x23D, 	0x0A, 0x23F, 	0x0A, 0x32F, 	0x0A, 0x11C, 	0x0A, 0x384, 	0x0A, 0x31C, 
	0x0A, 0x17C, 	0x0A, 0x30A, 	0x0A, 0x2E0, 	0x0A, 0x276, 	0x0A, 0x250, 	0x0B, 0x3E3, 	0x0A, 0x396, 	0x0A, 0x18F, 
	0x0A, 0x204, 	0x0A, 0x206, 	0x0A, 0x230, 	0x0A, 0x265, 	0x0A, 0x212, 	0x0A, 0x23E, 	0x0B, 0x3AC, 	0x0B, 0x393, 
	0x0B, 0x3E1, 	0x0A, 0x1DE, 	0x0B, 0x3D6, 	0x0A, 0x31D, 	0x0B, 0x3E5, 	0x0B, 0x3E4, 	0x0A, 0x207, 	0x0B, 0x3C7, 
	0x0A, 0x277, 	0x0B, 0x3D4, 	0x08, 0xC0,		0x0A, 0x162, 	0x0A, 0x3DA, 	0x0A, 0x124, 	0x0A, 0x1B4, 	0x0A, 0x264, 
	0x0A, 0x33D, 	0x0A, 0x1D1, 	0x0A, 0x1AF, 	0x0A, 0x39E, 	0x0A, 0x24F, 	0x0B, 0x373, 	0x0A, 0x249, 	0x0B, 0x372, 
	0x09, 0x167, 	0x0A, 0x210, 	0x0A, 0x23A, 	0x0A, 0x1B8, 	0x0B, 0x3AF, 	0x0A, 0x18E, 	0x0A, 0x2EC, 	0x07, 0x62, 
	0x04, 0x0D
};

int cSocket::Pack( void *pvIn, void *pvOut, int len )
{
	UI08 *pIn = (UI08 *)pvIn;
	UI08 *pOut = (UI08 *)pvOut;

	int actByte = 0, bitByte = 0;
	int nrBits;
	UI32 value;

	while( len-- )
	{
		nrBits = bit_table[*pIn][0];
		value = bit_table[*pIn++][1];

		while( nrBits-- )
		{
			pOut[actByte] = (pOut[actByte] << 1) | (UI08)((value >> nrBits) & 0x1);

			bitByte = (bitByte + 1) & 0x07;
			if( !bitByte ) 
				actByte++;
		}
	}

	nrBits = bit_table[256][0];
	value = bit_table[256][1];

	while( nrBits-- )
	{
		pOut[actByte] = (pOut[actByte] << 1) | (UI08)((value >> nrBits) & 0x1);

		bitByte = (bitByte + 1) & 0x07;
		if( !bitByte ) 
			actByte++;
	}

	if( bitByte )
	{
		while( bitByte < 8 )
		{
			pOut[actByte] <<= 1;
			bitByte++;
		}
		actByte++;
	}
	return actByte;
}

void cSocket::Send( const void *point, int length ) // Buffering send function
{
	if( outlength + length > MAXBUFFER ) 
		FlushBuffer();
	memcpy( &outbuffer[outlength], point, length );
	outlength += length;
}

#ifdef __LINUX__
int GrabLastError( void )
{
	return errno;
}
#else
int GrabLastError( void )
{
	return WSAGetLastError();
}
#endif

int cSocket::Receive( int x )
{
	int count = 0;
	char recvAttempts = 0;
	long curTime = getclock();
	long nexTime = curTime;
	do
	{
		count = recv( cliSocket, (char *)&buffer[inlength], x - inlength, 0 );
		if( count > 0 )
		{
			inlength += count;
		}
		else if( count == SOCKET_ERROR )
		{
			int lastError = GrabLastError();
#ifdef __LINUX__
			if( lastError != EWOULDBLOCK )
#else
			if( lastError != WSAEWOULDBLOCK )
#endif
				throw socket_error( lastError );
			UOXSleep( 20 );
		}
		recvAttempts++;
		nexTime = getclock();
		if( recvAttempts == 24 || (nexTime - curTime) > 2 * CLOCKS_PER_SEC )	// looks like we're not going to get it!
			throw socket_error( "Socket receive error" );
	}
	while( x != 2560 && x != inlength );
	if( Logging() )
	{
		FILE *fp = NULL;
		char filename[MAX_PATH];
		SERIAL toPrint;
		if( currchar == INVALIDSERIAL )
			toPrint = INVALIDSERIAL;
		else
			toPrint = chars[currchar].GetSerial();
		sprintf( filename, "%s/logs/%i.rcv", cwmWorldState->ServerData()->GetRootDirectory(), toPrint );
		char qbuffer[8];	memset( qbuffer, 0x00, 8 );
		int j = 0;
		if( ( fp = fopen( filename, "a" ) ) != NULL )
		{
			fprintf( fp, "[RECV]Packet: 0x%2X --> Length: %li\n\n", buffer[0], x );
			for( int counter = 0; counter < count; counter++ )
			{
				qbuffer[j++] = buffer[counter];
				fprintf(fp, "0x%2X ", buffer[counter] );
				if( j >= 7 )
				{
					fprintf( fp, "  %s\n", qbuffer );
					j = 0;
				}
			}
			fprintf( fp, "  %s\n\n", qbuffer );
			fclose( fp );
		}
		else
		{
			Console.Error( 1, "Failed to open socket log %s", filename );
		}
		bytesReceived += count;
	}
	return count;
}

void cSocket::InternalReset( void )
{
	memset( &itemmake, 0, sizeof( make_st ) );
	memset( buffer, 0, MAXBUFFER );
	memset( outbuffer, 0, MAXBUFFER );
	memset( tbuffer, 0, MAXBUFFER );
	memset( xtext, 0, 65 );
	ClearAuthor();
	ClearTitle();
	ClearPage();
	addx[0] = addx[1] = 0;
	addy[0] = addy[1] = 0;
	addid[0] = addid[1] = addid[2] = addid[3] = 0;
	clientip[0] = clientip[1] = clientip[2] = clientip[3] = 0;
	// set the socket to nonblocking
	UI32 mode = 1;
	ioctlsocket( cliSocket, FIONBIO, &mode );
}
cSocket::cSocket( int sockNum ) : currCharObj( NULL )/*, actbAccount()*/, idleTimeout( -1 ), currchar( INVALIDSERIAL ), wAccountID(AB_INVALID_ID),
tempint( 0 ), dyeall( 0 ), addz( 0 ), addmitem( INVALIDSERIAL ), newClient( true ), firstPacket( true ), range( 15 ),
cryptclient( false ), cliSocket( sockNum ), walkSequence( -1 ), addid5( 0 ), currentSpellType( 0 ),
outlength( 0 ), inlength( 0 ), logging( LOGDEFAULT ), postCount( 0 ), postAckCount( 0 ), pSpot( PL_NOWHERE ), pFrom( INVALIDSERIAL ),
pX( 0 ), pY( 0 ), pZ( 0 ), lang( UT_ENU ), cliType( DefaultClientType ), clientVersion( DefaultClientVersion ), bytesReceived( 0 ), bytesSent( 0 )
{
	InternalReset();
}

void cSocket::ClearAuthor( void )
{
	memset( authorbuffer, '~', 32 );
}
void cSocket::ClearTitle( void )
{
	memset( titlebuffer, '~', 62 );
}
void cSocket::ClearPage( void )
{
	memset( pagebuffer, '~', 512 );
}

void cSocket::OutLength( int newValue )
{
	outlength = newValue;
}
void cSocket::InLength( int newValue )
{
	inlength = newValue;
}
int cSocket::OutLength( void ) const
{
	return outlength;
}
int cSocket::InLength( void ) const
{
	return inlength;
}

bool cSocket::Logging( void ) const
{
	return logging;
}

void cSocket::Logging( bool newValue )
{
	logging = newValue;
}

CChar *cSocket::CurrcharObj( void ) const
{
	return currCharObj;
}

void cSocket::CurrcharObj( CChar *newValue )
{
	currCharObj = newValue;
	if( newValue != NULL )
		currchar = calcCharFromSer( currCharObj->GetSerial() );
	else
		currchar = INVALIDSERIAL;	// NULL character is invalidserial, for comparisons
}

//o--------------------------------------------------------------------------o
//|	Function			-	ACCOUNTSBLOCK &cSocket::GetAccount(void)
//|	Date					-	1/17/2003 6:21:59 AM
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Return to the calling function this objects accounts 
//|									referance.
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
ACCOUNTSBLOCK &cSocket::GetAccount(void)
{
	return actbAccount;
}

//o--------------------------------------------------------------------------o
//|	Function			-	void cSocket::SetAccount(ACCOUNTSBLOCK &actbBlock)
//|	Date					-	1/17/2003 7:01:23 AM
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
void cSocket::SetAccount(ACCOUNTSBLOCK &actbBlock)
{
	if(actbBlock.wAccountIndex==AB_INVALID_ID)
	{
		actbAccount.wAccountIndex=AB_INVALID_ID;
		return;
	}
	actbAccount=actbBlock;
	wAccountID=actbAccount.wAccountIndex;
}
//
void cSocket::SetAccount(std::string sUsername)
{
	if(!Accounts->GetAccountByName(sUsername,(ACCOUNTSBLOCK&)actbAccount))
	{
		// Ok there was an error setting an account to this character.
		wAccountID=AB_INVALID_ID;
		actbAccount.wAccountIndex=wAccountID;
		return;
	}
	wAccountID = actbAccount.wAccountIndex;
}
//
void cSocket::SetAccount(SI16 wNewAccountID)
{
	if(!Accounts->GetAccountByID(wNewAccountID,(ACCOUNTSBLOCK&)actbAccount))
	{
		// Ok there was an error setting an account to this character.
		wAccountID=AB_INVALID_ID;
		actbAccount.wAccountIndex=wAccountID;
		return;
	}
	wAccountID = actbAccount.wAccountIndex;
}
//


UI08 cSocket::ClientIP1( void ) const
{
	return clientip[0];
}
UI08 cSocket::ClientIP2( void ) const
{
	return clientip[1];
}
UI08 cSocket::ClientIP3( void ) const
{
	return clientip[2];
}
UI08 cSocket::ClientIP4( void ) const
{
	return clientip[3];
}

void cSocket::ClientIP1( UI08 newValue )
{
	clientip[0] = newValue;
}
void cSocket::ClientIP2( UI08 newValue )
{
	clientip[1] = newValue;
}
void cSocket::ClientIP3( UI08 newValue )
{
	clientip[2] = newValue;
}
void cSocket::ClientIP4( UI08 newValue )
{
	clientip[3] = newValue;
}

bool cSocket::NewClient( void ) const
{
	return newClient;
}

void cSocket::NewClient( bool newValue )
{
	newClient = newValue;
}

SI32 cSocket::GetDWord( int offset )
{
	return calcserial( buffer[offset], buffer[offset+1], buffer[offset+2], buffer[offset+3] );
}

SI16 cSocket::GetWord( int offset )
{
	return (SI16)((buffer[offset]<<8) + buffer[offset+1]);
}

UI08 cSocket::GetByte( int offset )
{
	return buffer[offset];
}

void cSocket::SetDWord( int offset, SI32 newValue )
{
	buffer[offset]   = (UI08)( newValue>>24 );
	buffer[offset+1] = (UI08)( newValue>>16 );
	buffer[offset+2] = (UI08)( newValue>>8 );
	buffer[offset+3] = (UI08)( newValue%256 );
}
void cSocket::SetWord( int offset, SI32 newValue )
{
	buffer[offset]   = (UI08)( newValue>>8 );
	buffer[offset+1] = (UI08)( newValue%256 );
}
void cSocket::SetByte( int offset, UI08 newValue )
{
	buffer[offset] = newValue;
}

void cSocket::ClientIP( long newValue )
{
	clientip[0] = (UI08)( newValue>>24 );
	clientip[1] = (UI08)( newValue>>16 );
	clientip[2] = (UI08)( newValue>>8 );
	clientip[3] = (UI08)( newValue%256 );
}

void cSocket::TargetOK( bool newValue )
{
	targetok = true;
}

bool cSocket::TargetOK( void ) const
{
	return targetok;
}

void cSocket::AddID( long newValue )
{
	addid[0] = (UI08)( newValue>>24 );
	addid[1] = (UI08)( newValue>>16 );
	addid[2] = (UI08)( newValue>>8 );
	addid[3] = (UI08)( newValue%256 );
}

SI32 cSocket::AddID( void ) const
{
	return calcserial( addid[0], addid[1], addid[2], addid[3] );
}

void cSocket::ClickX( SI16 newValue )
{
	clickx = newValue;
}
void cSocket::ClickY( SI16 newValue )
{
	clicky = newValue;
}
SI16 cSocket::ClickX( void ) const
{
	return clickx;
}
SI16 cSocket::ClickY( void ) const
{
	return clicky;
}

char cSocket::PostAcked( int x, int y ) const
{
	return postAcked[x][y];
}
int cSocket::PostCount( void ) const
{
	return postCount;
}
int cSocket::PostAckCount( void ) const
{
	return postAckCount;
}

void cSocket::PostAcked( int x, int y, char newValue )
{
	postAcked[x][y] = newValue;
}
void cSocket::PostCount( int newValue )
{
	postCount = newValue;
}
void cSocket::PostAckCount( int newValue )
{
	postAckCount = newValue;
}

void cSocket::Send( cPBuffer *toSend )
{
	if( toSend == NULL )
		return;
	Send( toSend->Pointer(), toSend->Length() );
	if( Logging() )
	{
	FILE *fp = NULL;
	char filename[MAX_PATH];
	SERIAL toPrint;
	if( currchar == INVALIDSERIAL )
		toPrint = INVALIDSERIAL;
	else
		toPrint = chars[currchar].GetSerial();
	sprintf( filename, "%s/logs/%i.snd", cwmWorldState->ServerData()->GetRootDirectory(), toPrint );
	fp = fopen( filename, "a" );
	toSend->Log( fp );
	if( fp != NULL )
		fclose( fp );
	}
	FlushBuffer( false );
}

void cSocket::PickupSpot( PickupLocations newValue )
{
	pSpot = newValue;
}
PickupLocations	cSocket::PickupSpot( void ) const
{
	return pSpot;
}
SERIAL cSocket::PickupSerial( void ) const
{
	return pFrom;
}
void cSocket::PickupSerial( SERIAL pickupSerial )
{
	pFrom = pickupSerial;
}

SI16 cSocket::PickupX( void ) const
{
	return pX;
}

SI16 cSocket::PickupY( void ) const
{
	return pY;
}

SI08 cSocket::PickupZ( void ) const
{
	return pZ;
}

void cSocket::PickupLocation( SI16 x, SI16 y, SI08 z )
{
	pX = x;
	pY = y;
	pZ = z;
}

void cSocket::PickupX( SI16 x )
{
	pX = x;
}

void cSocket::PickupY( SI16 y )
{
	pY = y;
}

void cSocket::PickupZ( SI08 z )
{
	pZ = z;
}

char *cSocket::AuthorBuffer( void )
{
	return authorbuffer;
}
char *cSocket::TitleBuffer( void )
{
	return titlebuffer;
}
char *cSocket::PageBuffer( void )
{
	return pagebuffer;
}
void cSocket::AuthorBuffer( const char *newValue )
{
	strcpy( authorbuffer, newValue );
}
void cSocket::PageBuffer( const char *newValue )
{
	strcpy( pagebuffer, newValue );
}
void cSocket::TitleBuffer( const char *newValue )
{
	strcpy( titlebuffer, newValue );
}

int cPBuffer::Length( void )
{
	return internalBuffer.size();
}

void cPBuffer::Log( FILE *fp )
{
	if( fp == NULL )
		return;
	char qbuffer[8];
	char j = 0;
	fprintf( fp, "[SEND]Packet: 0x%2X --> Length: %li\n", internalBuffer[0], internalBuffer.size() );
	for( UI32 i = 0; i < internalBuffer.size(); i++ )
	{
		qbuffer[j++] = internalBuffer[i];
		if( internalBuffer[i] < 10 )
			fprintf( fp, "0x0%X ", internalBuffer[i] );
		else
			fprintf( fp, "0x%2X ", internalBuffer[i] );
		if( j >= 7 )
		{
			fprintf( fp, "  %s\n", qbuffer );
			j = 0;
		}
	}
	fprintf( fp, "  %s\n\n", qbuffer );
}

UI08& cPBuffer::operator [] ( long int Num )
{
	if( Num < 0 || Num > Length() )
		throw std::runtime_error("out of bounds");
	return internalBuffer[Num];
}

cPBuffer::cPBuffer()
{
	internalBuffer.resize( 0 );
}

cPBuffer::cPBuffer( char *initBuffer, int len )
{
	internalBuffer.resize( len );
	for( int i = 0; i < len; i++ )
		internalBuffer[i] = initBuffer[i];
}

cPBuffer::cPBuffer( cPBuffer *initBuffer )
{
	internalBuffer.resize( initBuffer->Length() );
	for( int i = 0; i < initBuffer->Length(); i++ )
		internalBuffer[i] = (*initBuffer)[i];
}

cPBuffer::~cPBuffer()
{
//	internalBuffer.resize( 0 );
}

void cPBuffer::Resize( int newLen )
{
	internalBuffer.resize( newLen );
}

UI08 *cPBuffer::Pointer( void )
{
	return &internalBuffer[0];
}


cPInputBuffer::cPInputBuffer() : tSock( NULL )
{
}
cPInputBuffer::cPInputBuffer( cSocket *input )
{
	SetSocket( input );

	// recieve is abstract.. you can't call that from here
	//	Receive( input );
}
void cPInputBuffer::Receive( cSocket *input )
{
}
UI08& cPInputBuffer::operator[] ( long int num )
{
	return internalBuffer[num];
}
int cPInputBuffer::Length( void )
{
	return internalBuffer.size();
}
UI08 *cPInputBuffer::Pointer( void )
{
	return &internalBuffer[0];
}
void cPInputBuffer::Log( FILE *fp )
{
	if( fp == NULL )
		return;
	char qbuffer[8];
	char j = 0;
	fprintf( fp, "[RECV]Packet: 0x%2X --> Length: %li\n", internalBuffer[0], internalBuffer.size() );
	for( UI32 i = 0; i < internalBuffer.size(); i++ )
	{
		qbuffer[j++] = internalBuffer[i];
		if( internalBuffer[i] < 10 )
			fprintf( fp, "0x0%X ", internalBuffer[i] );
		else
			fprintf( fp, "0x%2X ", internalBuffer[i] );
		if( j >= 7 )
		{
			fprintf( fp, "  %s\n", qbuffer );
			j = 0;
		}
	}
	fprintf( fp, "  %s\n\n", qbuffer );
}

long cPInputBuffer::DWord( int offset )
{
	return ( (internalBuffer[offset]<<24) + (internalBuffer[offset+1]<<16) + (internalBuffer[offset+2]<<8) + internalBuffer[offset+3] );
}

SI32 cPInputBuffer::Word( int offset )
{
	return ( (internalBuffer[offset]<<8) + internalBuffer[offset+1] );
}

UI08 cPInputBuffer::Byte( int offset )
{
	return internalBuffer[offset];
}


bool cPInputBuffer::Handle( void )
{
	return false;
}

void cPInputBuffer::SetSocket( cSocket *toSet )
{
	tSock = toSet;
}

UnicodeTypes cSocket::Language( void ) const
{
	return lang;
}

void cSocket::Language( UnicodeTypes newVal )
{
	lang = newVal;
}

UI32 cSocket::ClientVersion( void ) const
{
	return clientVersion;
}
void cSocket::ClientVersion( UI32 newVer )
{
	clientVersion = newVer;
}
void cSocket::ClientVersion( UI08 major, UI08 minor, UI08 sub, UI08 letter )
{
	ClientVersion( calcserial( major, minor, sub, letter ) );
}

ClientTypes cSocket::ClientType( void ) const
{
	return cliType;
}
void cSocket::ClientType( ClientTypes newVer )
{
	cliType = newVer;
}


UI08 cSocket::ClientVersionMajor( void ) const
{
	return (UI08)(clientVersion>>24);
}

UI08 cSocket::ClientVersionMinor( void ) const
{
	return (UI08)(clientVersion>>16);
}

UI08 cSocket::ClientVersionSub( void ) const
{
	return (UI08)(clientVersion>>8);
}

UI08 cSocket::ClientVersionLetter( void ) const
{
	return (UI08)(clientVersion%256);
}

void cSocket::ClientVersionMajor( UI08 value )
{
	ClientVersion( value, ClientVersionMinor(), ClientVersionSub(), ClientVersionLetter() );
}

void cSocket::ClientVersionMinor( UI08 value )
{
	ClientVersion( ClientVersionMajor(), value, ClientVersionSub(), ClientVersionLetter() );
}

void cSocket::ClientVersionSub( UI08 value )
{
	ClientVersion( ClientVersionMajor(), ClientVersionMinor(), value, ClientVersionLetter() );
}

void cSocket::ClientVersionLetter( UI08 value )
{
	ClientVersion( ClientVersionMajor(), ClientVersionMinor(), ClientVersionSub(), value );
}

void cSocket::Range( UI08 value )
{
	range = value;
}

UI08 cSocket::Range( void ) const
{
	return range;
}
