/*

  Ultima Offline eXperiment III (UOX3)
  UO Server Emulation Program
  
	Copyright 1997, 98 by Marcus Rating (Cironian)
	
	  This program is free software; you can redistribute it and/or modify
	  it under the terms of the GNU General Public License as published by
	  the Free Software Foundation; either version 2 of the License, or
	  (at your option) any later version.
	  
		This program is distributed in the hope that it will be useful,
		but WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
		GNU General Public License for more details.
		
		  You should have received a copy of the GNU General Public License
		  along with this program; if not, write to the Free Software
		  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
		  
			* In addition to that license, if you are running this program or modified  *
			* versions of it on a public system you HAVE TO make the complete source of *
			* the version used by you available or provide people with a location to    *
			* download it.                                                              *
			
			  You can contact the author by sending email to <cironian@stratics.com>.
			  
*/

#include "uox3.h"
#include "cmdtable.h"
#include "debug.h"
#include "ssection.h"

#ifdef __LINUX__
#include <errno.h>
#include <signal.h>
#endif

#include "stream.h"
#include <queue>
#include <set>

#include "cVersionClass.h"

//using namespace std;

extern cVersionClass CVC;

void Bounce( cSocket *bouncer, CItem *bouncing );
void LoadCreatures( void );
void DumpCreatures( void );
void LoadINIFile( void );

#ifdef __LINUX__
	typedef void *HANDLE;
	pthread_t cons, netw;
#endif

bool cluox_io          = false;   // is cluox-IO enabled?
bool cluox_nopipe_fill = false;   // the stdin-pipe is known to be none-empty, no need to fill it.
HANDLE cluox_stdin_writeback = 0; // the write-end of the stdin-pipe

//o---------------------------------------------------------------------------o
//|	Function	-	cl_getch
//|	Programmer	-	knoxos
//o---------------------------------------------------------------------------o
//|	Purpose		-	Read a character from stdin, in a cluox compatble way.
//|                 This routine is non-blocking!
//|	Returns		-	>0 -> character read
//|                 -1 -> no char available.
//o---------------------------------------------------------------------------o
//
// now cluox is GUI wrapper over uox using stdin and stdout redirection to capture
// the console, if it is active uox can't use kbhit() to determine if there is a 
// character aviable, it can only get one directly by getch().
// However the problem arises that uox will get blocked if none is aviable.
// The solution to this problem is that cluox also hands over the second pipe-end
// of stdin so uox can write itself into this stream. To determine if a character is 
// now done that way. UOX write's itself a ZERO on the other end of the pipe, and reads
// a charecter, if it is again the same ZERO just putted in nothing was entered. However
// it is not a zero the user has entered a char.
// 
int cl_getch( void )
{
#ifdef __LINUX__
	// first the linux style, don't change it's behavoir
	UI08 c = 0;
	fd_set KEYBOARD;
	FD_ZERO( &KEYBOARD );
	FD_SET( 0, &KEYBOARD );
	int s = select( 1, &KEYBOARD, NULL, NULL, &uoxtimeout );
	if( s < 0 )
	{
		Console.Error( 1, "Error scanning key press" );
		Shutdown( 10 );
	}
	if( s > 0 )
	{
		read( 0, &c, 1 );
		if( c == 0x0A )
			return -1;
	}
#elif defined(__NT__)
	// now the windows one
	if( !cluox_io ) 
	{
		// uox is not wrapped simply use the kbhit routine
		if( kbhit() )
			return getch();
		else 
			return -1;
	}
	// the wiered cluox getter.
	UI08 c = 0;
	UI32 bytes_written = 0;
	int asw = 0;
	if( !cluox_nopipe_fill )
		asw = WriteFile( cluox_stdin_writeback, &c, 1, &bytes_written, NULL );
	if( bytes_written != 1 || asw == 0 ) 
	{
		Console.Warning( 1, "Using cluox-io" );
		Shutdown( 10 );
	}
	c = (UI08)fgetc( stdin );
	if( c == 0 )
	{
		cluox_nopipe_fill = false;
		return -1;
	}
#else
#  error Unknown operating system.
#endif
	// here an actual charater is read in
	return c;
}

//o---------------------------------------------------------------------------o
//|   Function    :  void DoMessageLoop( void )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Watch for messages thrown by UOX
//o---------------------------------------------------------------------------o
void DoMessageLoop( void )
{
	while( !messageLoop.Empty() )
	{
		MessagePassed tVal = messageLoop.GrabMessage();
		switch( tVal.actualMessage )
		{
		case MSG_SHUTDOWN:		keeprun = false;								break;
		case MSG_COUNT:															break; 
		case MSG_WORLDSAVE:		oldtime = 0;									break;
		case MSG_PRINT:			Console << tVal.data << myendl;				break;
		case MSG_RELOADJS:		Trigger->ReloadJS();	Console.PrintDone();	break;
		case MSG_CONSOLEBCAST:	consolebroadcast( tVal.data );					break;
		case MSG_PRINTDONE:		Console.PrintDone();							break;
		case MSG_PRINTFAILED:	Console.PrintFailed();							break;
		case MSG_SECTIONBEGIN:	Console.PrintSectionBegin();					break;
		case MSG_UNKNOWN:
		default:				Console.Error( 2, "Unknown message type" );		break;
		}
	}
}
//------------------------------------------------------------
//---------------------------------------------------------------------------

#undef DBGFILE
#define DBGFILE "uox3.cpp"

//	EviLDeD	-	June 21, 1999
//	Ok here is thread number one its a simple thread for the checkkey() function
#ifndef __LINUX__
CRITICAL_SECTION sc;	//
#endif
bool conthreadcloseok = false;
bool netpollthreadclose = false;
void NetworkPollConnectionThread( void *params );

#ifdef __LINUX__
void *CheckConsoleKeyThread( void *params )
#else
void CheckConsoleKeyThread( void *params )
#endif
{
	messageLoop << "Thread: CheckConsoleThread has started";
	conthreadcloseok = false;
	while( !conthreadcloseok )
	{
		checkkey();
		UOXSleep( 500 );
	}
#ifndef __LINUX__
	_endthread();		// linux will kill the thread when it returns
#endif
	messageLoop << "Thread: CheckConsoleKeyThread Closed";
#ifdef __LINUX__
	return NULL;
#endif
}
//	EviLDeD	-	End

#ifdef __NT__
///////////////////

//HANDLE hco;
//CONSOLE_SCREEN_BUFFER_INFO csbi;

///////////////////
#endif

#ifndef __NT__

void closesocket( UOXSOCKET s )
{
	shutdown( s, 2 );
	close( s );
}
#endif

//o---------------------------------------------------------------------------o
//|   Function    :  void numtostr( int i, char *string )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Wrapping of the stdlib num-2-str functions
//o---------------------------------------------------------------------------o
void numtostr( int i, char *string )
{
#ifdef __NT__
	itoa( i, string, 10 );
#else
	sprintf(string, "%d", i );
#endif
}

//o--------------------------------------------------------------------------o
//|	Function			-	void safeCopy(char *dest, const char *src, UI32 maxLen )
//|	Date					-	
//|	Developers		-	
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Safely copy a string that might be longer than the 
//|									destination Will truncate if needed, but will never copy 
//|									over too much to avoid possible crashes.
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
void safeCopy(char *dest, const char *src, UI32 maxLen )
{
	assert( src );
	assert( dest );
	assert( maxLen );
	
	strncpy( dest, src, maxLen );
	dest[maxLen - 1] = '\0';
}

//o--------------------------------------------------------------------------o
//|	Function			-	bool isOnline( CChar *c )
//|	Date					-	
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Check if the socket owning character c is still connected
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
bool isOnline( CChar *c )
{
	if( c == NULL )
		return false;
	if( c->IsNpc() )
		return false;
	ACCOUNTSBLOCK actbTemp = c->GetAccount();
	if(actbTemp.wAccountIndex != AB_INVALID_ID)
	{
		if(actbTemp.dwInGame == c->GetSerial() )
			return true;
	}
	Network->PushConn();
	for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
	{
		if( tSock->CurrcharObj() == c )
		{
			Network->PopConn();
			return true;
		}
	}
	Network->PopConn();
	return false;
}

//o---------------------------------------------------------------------------o
//|   Function    :  UI08 bestskill( CChar *p )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Return characters highest skill
//o---------------------------------------------------------------------------o
UI08 bestskill( CChar *p )
{
	if( p == NULL )
		return 0;
	int b = 0;
	UI08 a = 0;

	for( UI08 i = 0; i < TRUESKILLS; i++ )
	{
		if( p->GetBaseSkill( i ) > b )
		{
			a = i;
			b = p->GetBaseSkill( i );
		}
	}
	return a;
}

//o---------------------------------------------------------------------------o
//|   Function    :  void loadcustomtitle( void )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Loads players titles (Karma, Fame, Murder, ect)
//o---------------------------------------------------------------------------o
void loadcustomtitle( void )
{ 
	int titlecount = 0;
	char sect[512]; 
	
	strcpy( sect, "SKILL" );
	ScriptSection *CustomTitle = FileLookup->FindEntry( sect, titles_def );
	if( CustomTitle == NULL)
		return;
	const char *tag = NULL;
	const char *data = NULL;
	char tempSkill[256];
	for( tag = CustomTitle->First(); !CustomTitle->AtEnd(); tag = CustomTitle->Next() )
	{
		data = CustomTitle->GrabData();
		sprintf( tempSkill, "%s %s", tag, data );
		strcpy( title[titlecount++].skill, tempSkill );
	}
	strcpy( sect, "PROWESS" );
	CustomTitle = FileLookup->FindEntry( sect, titles_def );
	if( CustomTitle == NULL)
		return;
	titlecount = 0;
	for( tag = CustomTitle->First(); !CustomTitle->AtEnd(); tag = CustomTitle->Next() )
		strcpy( title[titlecount++].prowess, tag );

	strcpy( sect, "FAME" );
	CustomTitle = FileLookup->FindEntry( sect, titles_def );
	titlecount = 0;

	for( tag = CustomTitle->First(); !CustomTitle->AtEnd(); tag = CustomTitle->Next() )
	{
		strcpy( title[titlecount].fame, tag );
		if( titlecount == 23 )
		{
			title[titlecount].fame[0] = 0;
			strcpy( title[++titlecount].fame, tag );
		}
		titlecount++;
	}

	// Murder tags now scriptable in SECTION MURDER - Titles.scp - Thanks Ab - Zane
	CustomTitle = FileLookup->FindEntry( "MURDERER", titles_def );
	if( CustomTitle == NULL )	// couldn't find it
		return;
	for( tag = CustomTitle->First(); !CustomTitle->AtEnd(); tag = CustomTitle->Next() )
	{
		data = CustomTitle->GrabData();
		murdererTags.push_back( MurderPair( (SI16)makeNum( tag ), data ) );
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	char *title1( CChar *p )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Fetches characters "prowess" title based on his skill and
//|                 entries in titles.scp
//|	Modified	-	Modified to match OSI's prowess titles by Zane
//o---------------------------------------------------------------------------o
char *title1( CChar *p )
{
	if( p == NULL )
		return NULL;
	int titlenum = 0;
	int x = p->GetBaseSkill( bestskill( p ) );
  
	if( x < 1000 ) 
		titlenum = (int)((x - 10) / 100 - 2);
	else
		titlenum = 8;
	strcpy( prowesstitle, title[titlenum].prowess );
	return prowesstitle;
}

//o---------------------------------------------------------------------------o
//|	Function	-	char *title2( CChar *p )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Returns players bestskill for prowess title
//o---------------------------------------------------------------------------o
char *title2( CChar *p )
{
	int titlenum = bestskill( p ) + 1;
	strcpy( skilltitle, title[titlenum].skill );
	return skilltitle;
}

//o---------------------------------------------------------------------------o
//|	Function	-	char *title3( CChar *p )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Returns players reputation title based on their Fame and Karma
//o---------------------------------------------------------------------------o
char *title3( CChar *p )
{
	if( p == NULL )
		return NULL;
	char thetitle[50];
	UI08 titlenum = 0;
	
	SI32 k = p->GetKarma();
	UI32 f = p->GetFame();
	thetitle[0] = 0;
	
	if( k >= 10000 )
	{
		if( f >= 5000 )
			titlenum = 0;
		else if( f >= 2500 )
			titlenum = 1;
		else if( f >= 1250 )
			titlenum = 2;
		else
			titlenum = 3;
	}
	else if( k >= 5000 )
	{
		if( f >= 5000 ) 
			titlenum = 4;
		else if( f >= 2500 ) 
			titlenum = 5;
		else if( f >= 1250 ) 
			titlenum = 6;
		else
			titlenum = 7;
	}
	else if( k >= 2500 )
	{
		if( f >= 5000 ) 
			titlenum = 8;
		else if( f >= 2500 ) 
			titlenum = 9;
		else if( f >= 1250 ) 
			titlenum = 10;
		else
			titlenum = 11;
	}
	else if( k >= 1250)
	{
		if( f >= 5000 ) 
			titlenum = 12;
		else if( f >= 2500 ) 
			titlenum = 13;
		else if( f >= 1250 ) 
			titlenum = 14;
		else
			titlenum = 15;
	}
	else if( k >= 625 )
	{
		if( f >= 5000 ) 
			titlenum = 16;
		else if( f >= 1000 ) 
			titlenum = 17;
		else if( f >= 500 )
			titlenum = 18;
		else
			titlenum = 19;
	}
	else if( k > -625 )
	{
		if( f >= 5000 ) 
			titlenum = 20;
		else if( f >= 2500 ) 
			titlenum = 21;
		else if( f >= 1250 ) 
			titlenum = 22;
		else
			titlenum = 23;
	}
	else if( k > -1250 )
	{
		if( f >= 10000 ) 
			titlenum = 28;
		else if( f >= 5000 ) 
			titlenum = 27;
		else if( f >= 2500 ) 
			titlenum = 26;
		else if( f >= 1250 ) 
			titlenum = 25;
		else
			titlenum = 24;
	}
	else if( k > -2500 )
	{
		if( f >= 5000 ) 
			titlenum = 32;
		else if( f >= 2500 ) 
			titlenum = 31;
		else if( f >= 1250 ) 
			titlenum = 30;
		else
			titlenum = 29;
	}
	else if( k > -5000 )
	{
		if( f >= 10000 ) 
			titlenum = 37;
		else if( f >= 5000 ) 
			titlenum = 36;
		else if( f >= 2500 ) 
			titlenum = 35;
		else if( f >= 1250 ) 
			titlenum = 34;
		else
			titlenum = 33;
	}
	else if( k > -10000 )
	{
		if( f >= 5000 ) 
			titlenum = 41;
		else if( f >= 2500 ) 
			titlenum = 40;
		else if( f >= 1250 ) 
			titlenum = 39;
		else
			titlenum = 38;
	}
	else if( k <= -10000 )
	{
		if( f >= 5000 ) 
			titlenum = 45;
		else if( f >= 2500 ) 
			titlenum = 44;
		else if( f >= 1250 ) 
			titlenum = 43;
		else
			titlenum = 42;
	}
	if( p->GetRace() != 0 && p->GetRace() != 65535 )
		sprintf( thetitle, "%s %s ", title[titlenum].fame, Races->Name( p->GetRace() ) );
	else
		sprintf( thetitle, "%s ", title[titlenum].fame );
	if( f >= 10000 ) // Morollans bugfix for repsys
	{
		if( p->GetKills() > cwmWorldState->ServerData()->GetRepMaxKills() )
        {
			if( p->GetID( 2 ) == 0x91 ) 
				sprintf( fametitle, Dictionary->GetEntry( 1177 ), Races->Name( p->GetRace() ) );
			else 
				sprintf( fametitle, Dictionary->GetEntry( 1178 ), Races->Name( p->GetRace() ) );
        }
		else if( p->GetID( 2 ) == 0x91 ) 
			sprintf( fametitle, Dictionary->GetEntry( 1179 ), thetitle );
		else 
			sprintf( fametitle, Dictionary->GetEntry( 1180 ), thetitle );
	}
	else
	{
		if( p->GetKills() > cwmWorldState->ServerData()->GetRepMaxKills() )
			strcpy( fametitle, Dictionary->GetEntry( 1181 ) );
		else if( strcmp( thetitle, " " ) ) 
			sprintf( fametitle, Dictionary->GetEntry( 1182 ), thetitle );
		else 
			fametitle[0] = 0;
	}
	return fametitle;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void gcollect( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Remove items in invalid locations (cleanup deleted packs and
//|					their items)
//o---------------------------------------------------------------------------o
void gcollect( void )
{
	CChar *c;
	CItem *j;
	SERIAL serial;
	SI32 rtotal = 0, removed = 0;
	bool idelete = false;
	uiCurrentTime = 0;
	
	Console << "Performing Garbage Collection...";
	do
	{
		removed = 0;
		for( ITEM i = 0; i < itemcount; i++ )
		{
			if( !items[i].isFree() )
			{
				idelete = false; 
				serial = items[i].GetCont();
				if( serial != INVALIDSERIAL )
				{
					idelete = true;
					if( items[i].GetCont( 1 ) < 0x40 ) // container is a character...verify the character??
					{
						c = calcCharObjFromSer( serial );
						if( c != NULL )
						{
							if( !c->isFree() )
								idelete = false;
						}
					} 
					else 
					{// find the container if there is one.
						j = calcItemObjFromSer( serial );
						if( j != NULL )
						{
							if( !j->isFree() )
								idelete = false;
						}
					}
				}
				if( idelete )
				{
					Items->DeleItem( &items[i] );
					removed++;
				}
			}
		}
		rtotal += removed;
	} while( removed > 0 );
	
	
	uiCurrentTime = getclock();
    Console << " Removed " << rtotal << " items." << myendl;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void item_test( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check for and correct bugged items
//o---------------------------------------------------------------------------o
void item_test( void )
{
	SERIAL serial;
	uiCurrentTime = 0;
	
	Console << "Starting item-consistency check...";
	for( ITEM a = 0; a < itemcount; a++ )
	{
		if( !items[a].isFree() )
		{
			serial = items[a].GetSerial();
			if( serial == INVALIDSERIAL )
			{
				Console << "WARNING: item " << items[a].GetName() << " [" << a << "] has an invalid serial number" << myendl;
				continue;
			}

			if( serial == items[a].GetCont() )
			{
				Console << "ALERT ! item " << items[a].GetName() << " [" << a << "] [serial: " << items[a].GetSerial() << "] has dangerous container value, autocorrecting" << myendl;
				items[a].SetCont( INVALIDSERIAL );
			}
			if( serial == items[a].GetOwner() )
			{
				Console << "ALERT ! item " << items[a].GetName() << " [" << a << "] [serial: " << items[a].GetSerial() << "] has dangerous owner value" << myendl;
				items[a].SetOwner( INVALIDSERIAL );
			}
			if( serial != 0 && serial == items[a].GetSpawn()  )
			{
				Console << "ALERT ! item " << items[a].GetName() << " [" << a << "] [serial: " << items[a].GetSerial() << "] has dangerous spawner value" << myendl;
				items[a].SetSpawn( INVALIDSERIAL, a );
				nspawnsp.Remove( serial, a );
			}
		}
	}
	uiCurrentTime = getclock();
	Console.PrintDone();
}

//o---------------------------------------------------------------------------o
//|	Function	-	char *RealTime( char *time_str )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Returns the real time
//o---------------------------------------------------------------------------o
char *RealTime(char *time_str)
{
	struct tm *curtime;
	time_t bintime;
	time(&bintime);
	curtime = localtime(&bintime);
	strftime(time_str, 256, "%B %d %I:%M:%S %p", curtime);
	return time_str;
}

//o---------------------------------------------------------------------------o
//|	Function	-	int makenumber( int countx )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Converts decimal string comm[count] to an integer
//o---------------------------------------------------------------------------o
int makenumber( int countx )
{
	if( comm[countx] == NULL )
		return 0;
	return makeNum( comm[countx] );
}

SI32 makeNum( const std::string *data )
{
	return makeNum( data->c_str() );
}
//o---------------------------------------------------------------------------o
//|	Function	-	SI32 makeNum( const char *s )
//|	Programmer	-	seank
//o---------------------------------------------------------------------------o
//|	Purpose		-	Converts a string to an integer
//o---------------------------------------------------------------------------o
SI32 makeNum( const char *data )
{
	if(!data/* == NULL*/)
		return 0;

	char o, h;
	// NOTE: You MUST leave ret as an unsigned 32 bit integer rather than signed, due to the stupidity of VC
	// VC will NOT cope with a string like "4294967295" if it's SI32, but will if it's UI32.  Noteably,
	// VC WILL cope if it's UI32 and the string is "-1".

	UI32 ret = 0;

	std::string s( data );
	std::istringstream ss( s );
	ss >> o;
	if( o == '0' )			// oct and hex both start with 0
	{
		ss >> h;
		if( h == 'x' || h == 'X' )
			ss >> std::hex >> ret >> std::dec;	// it's hex
		else
		{
			ss.unget();
			ss >> std::oct >> ret >> std::dec;	// it's octal
		}
	}
	else
	{
		ss.unget();
		ss >> std::dec >> ret;		// it's decimal
	}
	return ret;
}

//o---------------------------------------------------------------------------o
//|	Function	-	CItem *getPack( CChar *p )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get characters main pack
//o---------------------------------------------------------------------------o
CItem *getPack( CChar *p )
{
	if( p == NULL ) 
		return NULL;
	CItem *i = p->GetPackItem();
	
	if( i != NULL && p != NULL )
	{
		if( i->GetCont() == p->GetSerial() && i->GetLayer() == 0x15 )
			return i;
	}

	CItem *packItem = FindItemOnLayer( p, 0x15 );
	if( packItem != NULL )
	{
		p->SetPackItem( packItem );
		return packItem;
	}
	return NULL;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void action( cSocket *s, SI16 x )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Character does a certain action
//o---------------------------------------------------------------------------o
void action( cSocket *s, SI16 x )
{
	CChar *mChar = s->CurrcharObj();
	
	CPCharacterAnimation toSend = (*mChar);
	toSend.Action( x );
	s->Send( &toSend );
	Network->PushConn();
	for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
	{
		if( tSock != s && charInRange( s, tSock ) )
			tSock->Send( &toSend );
	}
	Network->PopConn();
}

//o---------------------------------------------------------------------------o
//|	Function	-	void wornItems( cSocket *s, CChar *j )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Find all items a character is wearing
//o---------------------------------------------------------------------------o
void wornItems( cSocket *s, CChar *j )
{
	j->SetOnHorse( false );
	CPWornItem toSend;
	for( CItem *i = j->FirstItem(); !j->FinishedItems(); i = j->NextItem() )
	{
		if( i != NULL && !i->isFree() )
		{
			if( i->GetLayer() == 0x19 ) 
				j->SetOnHorse( true );
			toSend = (*i);
			s->Send( &toSend );
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void openPack( cSocket *s, SERIAL serial )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Open backpack and display contents
//o---------------------------------------------------------------------------o
void openPack( cSocket *s, SERIAL serial )
{
	if( serial == INVALIDSERIAL ) 
		return;
	CItem *i = calcItemObjFromSer( serial );
	if( i == NULL )
	{
		Console << "WARNING: openPack() couldn't find backpack: " << serial << myendl;
		return;
	}
	openPack( s, i );
}
//o---------------------------------------------------------------------------o
//|	Function	-	void openPack( cSocket *s, CItem *i )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Open backpack and display contents
//o---------------------------------------------------------------------------o
void openPack( cSocket *s, CItem *i )
{
	if( i == NULL )
	{
		Console << "WARNING: openPack() couldn't find backpack: " << myendl;
		return;
	}
	CPDrawContainer contSend = (*i);
	contSend.Model( 0x3C );

	if( i->GetID( 1 ) == 0x3E )            // boats
		contSend.Model( 0x4C );
	else
	{
		switch( i->GetID() )
		{
		case 0x2006:      // coffin
			contSend.Model( 0x09 );
			break;
		case 0x0E75:      // backpack
		case 0x0E79:      // box/pouch
		case 0x09B0:
			contSend.Model( 0x3C );
			break;
		case 0x0E76:      // leather bag
			contSend.Model( 0x3D );
			break;
		case 0x0E77:      // barrel
		case 0x0E7F:      // keg
		case 0x0E83:
		case 0x0FAE:	// barrel with lids
		case 0x1AD7:	// potion kegs
		case 0x1940:	// barrel with lids
			contSend.Model( 0x3E );
			break;
		case 0x0E7A:      // square basket
			contSend.Model( 0x3F );
			break;
		case 0x0990:      // round basket
		case 0x09AC:
		case 0x09B1:
			contSend.Model( 0x41 );
			break;
		case 0x0E40:      // metal & gold chest
		case 0x0E41:      // metal & gold chest
			contSend.Model( 0x42 );
			break;
		case 0x0E7D:      // wooden box
		case 0x09AA:      // wooden box
			contSend.Model( 0x43 );
			break;
		case 0x0E3C:      // large wooden crate
		case 0x0E3D:      // large wooden crate
		case 0x0E3E:      // small wooden create
		case 0x0E3F:      // small wooden crate
		case 0x0E7E:      // wooden crate
		case 0x09A9:      // small wooden crate
			contSend.Model( 0x44 );
			break;
		case 0x2AF8:		// Shopkeeper buy, sell and sold layers
			contSend.Model( 0x47 );
			break;
		case 0x0A30:   // chest of drawers (fancy)
		case 0x0A38:   // chest of drawers (fancy)
			contSend.Model( 0x48 );
			break;
		case 0x0E42:      // wooden & gold chest
		case 0x0E43:      // wooden & gold chest
			contSend.Model( 0x49 );
			break;
		case 0x0E80:      // brass box
		case 0x09A8:      // metal box
			contSend.Model( 0x4B );
			break;
		case 0x0E7C:      // silver chest
		case 0x09AB:      // metal & silver chest
			contSend.Model( 0x4A );
			break;
			
		case 0x0A97:   // bookcase
		case 0x0A98:   // bookcase
		case 0x0A99:   // bookcase
		case 0x0A9A:   // bookcase
		case 0x0A9B:   // bookcase
		case 0x0A9C:   // bookcase
		case 0x0A9D:	// bookcase (empty)
		case 0x0A9E:	// bookcase (empty)
			contSend.Model( 0x4D );
			break;

		case 0x0A4C:   // fancy armoire (open)
		case 0x0A4D:   // fancy armoire
		case 0x0A50:   // fancy armoire (open)
		case 0x0A51:   // fancy armoire
			contSend.Model( 0x4E );
			break;
			
		case 0x0A4E:   // wooden armoire (open)
		case 0x0A4F:   // wooden armoire
		case 0x0A52:   // wooden armoire (open)
		case 0x0A53:   // wooden armoire
			contSend.Model( 0x4F );
			break;
		case 0x0A2C:   // chest of drawers (wood)
		case 0x0A34:   // chest of drawers (wood)
		case 0x0A35:   // dresser
		case 0x0A3C:   // dresser
		case 0x0A3D:   // dresser
		case 0x0A44:   // dresser
			contSend.Model( 0x51 );
			break;
		case 0x09B2:      // bank box ..OR.. backpack 2
			if( i->GetMoreX() == 1 )
				contSend.Model( 0x4A );
			else 
				contSend.Model( 0x3C );
			break;
		}
	}   
	s->Send( &contSend );
	CPItemsInContainer itemsIn( i );
	s->Send( &itemsIn );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void openCorpse( cSocket *s, SERIAL serial )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Send corpse container gump and all items it contains
//o---------------------------------------------------------------------------o
void openCorpse( cSocket *s, SERIAL serial )
{
	int count = 0, count2;
	char bpopen2[6]="\x3C\x00\x05\x00\x00";
	char display1[8]="\x89\x00\x0D\x40\x01\x02\x03";
	char display2[6]="\x01\x40\x01\x02\x03";

	CItem *i = calcItemObjFromSer( serial );
	if( i == NULL )
	{
		Console << "WARNING: openCorpse() couldn't find backpack: " << serial << myendl;
		return;
	}

	CItem *ci;
	for( ci = i->FirstItemObj(); !i->FinishedItems(); ci = i->NextItemObj() )
	{
		if( ci != NULL )
		{
			if( ci->GetLayer() != 0 && !ci->isFree() )
				count++;
		}
	}
	count2 = ( count * 5 ) + 7 + 1; // 5 bytes per object, 7 for this header and 1 for
	// terminator
	display1[1] = (UI08)(count2>>8);
	display1[2] = (UI08)(count2%256);
	display1[3] = (UI08)(serial>>24);
	display1[4] = (UI08)(serial>>16);
	display1[5] = (UI08)(serial>>8);
	display1[6] = (UI08)(serial%256);
	s->Send( display1, 7 );
	for( ci = i->FirstItemObj(); !i->FinishedItems(); ci = i->NextItemObj() )
	{
		if( ci != NULL )
		{
			if( ci->GetLayer() != 0 && !ci->isFree() )
			{
				display2[0] = ci->GetLayer();
				display2[1] = ci->GetSerial( 1 );
				display2[2] = ci->GetSerial( 2 );
				display2[3] = ci->GetSerial( 3 );
				display2[4] = ci->GetSerial( 4 );
				s->Send( display2, 5 );
			}
		}
	}
	
	// Terminate with a 0
	char nul = 0;
	s->Send( &nul, 1 );
	
	bpopen2[3] = (UI08)(count>>8);
	bpopen2[4] = (UI08)(count%256);
	count2 = ( count * 19 ) + 5;
	bpopen2[1] = (UI08)(count2>>8);
	bpopen2[2] = (UI08)(count2%256);
	s->Send( bpopen2, 5 );
	for( ci = i->FirstItemObj(); !i->FinishedItems(); ci = i->NextItemObj() )
	{
		if( ci != NULL )
		{
			if( ci->GetLayer() != 0 && !ci->isFree() )
			{
			   bpitem[0] = ci->GetSerial( 1 );
			   bpitem[1] = ci->GetSerial( 2 );
			   bpitem[2] = ci->GetSerial( 3 );
			   bpitem[3] = ci->GetSerial( 4 );
			   bpitem[4] = ci->GetID( 1 );
			   bpitem[5] = ci->GetID( 2 );
			   bpitem[7] = (UI08)(ci->GetAmount()>>8);
			   bpitem[8] = (UI08)(ci->GetAmount()%256);
			   bpitem[9] = (UI08)(ci->GetX()>>8);
			   bpitem[10] = (UI08)(ci->GetX()%256);
			   bpitem[11] = (UI08)(ci->GetY()>>8);
			   bpitem[12] = (UI08)(ci->GetY()%256);
			   bpitem[13] = (UI08)(serial>>24);
			   bpitem[14] = (UI08)(serial>>16);
			   bpitem[15] = (UI08)(serial>>8);
			   bpitem[16] = (UI08)(serial%256);
			   bpitem[17] = ci->GetColour( 1 );
			   bpitem[18] = ci->GetColour( 2 );
			   bpitem[19] = 0;
				ci->SetDecayTime( 0 );// reseting the decaytimer in the backpack  //moroallan
				s->Send( bpitem, 19 );
			}
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void sendPackItem( cSocket *s, cItem *i )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Update a single item in a backpack
//o---------------------------------------------------------------------------o
void sendPackItem( cSocket *s, CItem *i )
{
	CPAddItemToCont itemSend = (*i);

	CChar *mChar = s->CurrcharObj();

	if( i->GetGlow() > 0 )
		Items->GlowItem( i );

	if( mChar->IsGM() && i->GetID() == 0x1647 )
	{
		itemSend.Model( 0x0A0F );
		itemSend.Colour( 0x00C6 );
	}
	i->SetDecayTime( 0 );

	CChar *c = getPackOwner( i );
	if( c == NULL )
	{
		CItem *j = getRootPack( i );
		if( j != NULL && itemInRange( mChar, j ) )
			s->Send( &itemSend );
	}
	else if( charInRange( mChar, c ) )
		s->Send( &itemSend );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void sendItem( cSocket *s, CItem *i )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Send items on the ground
//o---------------------------------------------------------------------------o
void sendItem( cSocket *s, CItem *i )
{	
	if( i == NULL || s == NULL ) 
		return;

	CChar *mChar = s->CurrcharObj();
	if( i->GetVisible() > 1 && !mChar->IsGM() )
		return;

	bool pack = false;
	UI08 itmput[20] = "\x1A\x00\x13\x40\x01\x02\x03\x20\x42\x00\x32\x06\x06\x06\x4A\x0A\x00\x00\x00";

	// meaning of the item's attribute visible
	// Visible 0 -> visible to everyone
	// Visible 1 -> only visible to owner and gm's (for owners normal for gm's grayish/hidden color)
	// Visible 2 -> only visible to gm's (greyish/hidden color)
	if( i->GetGlow() > 0 )
		Items->GlowItem( i );

	if( i->GetCont( 1 ) != 255 )
	{
		pack = true;
		if( i->GetCont( 1 ) < 0x40 )
		{
			SERIAL serial = i->GetCont();
			if( serial == INVALIDSERIAL ) 
				return;
			CChar *j = calcCharObjFromSer( serial );
			if( j != NULL )
			{
				if( j->GetSerial() == serial )
					pack = false;
			}
		}
		if( pack )
		{
			if( i->GetID( 1 ) < 0x40 ) // Client crashfix, no show multis in BP
			{
				sendPackItem( s, i );
				return;
			}
		}
	}
	
	if( i->GetCont() == INVALIDSERIAL && itemInRange( mChar, i ) )
	{
		itmput[3] = (UI08)((i->GetSerial( 1 ) )+0x80); // Enable Piles
		itmput[4] = i->GetSerial( 2 );
		itmput[5] = i->GetSerial( 3 );
		itmput[6] = i->GetSerial( 4 );

		// if player is a gm, this item
		// is shown like a candle (so that he can move it),
		// ....if not, the item is a normal
		// invisible light source!
		if( mChar->IsGM() && i->GetID() == 0x1647 ) // items[i].id1 == 0x16 && items[i].id2 == 0x47 )
		{
			itmput[7] = 0x0A;
			itmput[8] = 0x0F;
		}
		else
		{
			itmput[7] = i->GetID( 1 );
			itmput[8] = i->GetID( 2 );
		}

		itmput[9] = (UI08)(i->GetAmount()>>8);
		itmput[10] = (UI08)(i->GetAmount()%256);
		itmput[11] = (UI08)(i->GetX()>>8);
		itmput[12] = (UI08)(i->GetX()%256);
		itmput[13] = (UI08)((i->GetY()>>8) + 0xC0); // Enable Dye and Move
		itmput[14] = (UI08)(i->GetY()%256);
		itmput[15] = i->GetZ();
		if( mChar->IsGM() && i->GetID() == 0x1647 ) // items[i].id1 == 0x16 && items[i].id2 == 0x47 )
		{
			itmput[16] = 0x00;
			itmput[17] = 0xC6;
		}
		else
		{
			itmput[16] = i->GetColour( 1 );
			itmput[17] = i->GetColour( 2 );
		}
		itmput[18] = 0;
		bool dontsendcandidate = false;
		if( i->GetVisible() == 1 )
		{
			if( mChar->GetSerial() != i->GetOwner() )
			{
				dontsendcandidate = true;
				itmput[18] |= 0x80;
			}
		}
		if( dontsendcandidate && !mChar->IsGM() ) 
			return;
		if( i->GetVisible() >= 2 )
			itmput[18] |= 0x80;
		
		if( i->GetMagic() == 1 ) 
			itmput[18]+=0x20;
		if( mChar->AllMove() ) 
			itmput[18]+=0x20;
		if( ( i->GetMagic() == 3 || i->GetMagic() == 4 ) && mChar->GetSerial() == i->GetOwner() )
			itmput[18]+=0x20;
		if( mChar->ViewHouseAsIcon() )
		{
			if( i->GetID( 1 ) >= 0x40 )
			{
				itmput[7] = 0x14;
				itmput[8] = 0xF0;
			}
		}
		UI08 dir = 0;
		if( i->GetDir() )
		{
			dir = 1;
			itmput[19] = itmput[18];
			itmput[18] = itmput[17];
			itmput[17] = itmput[16];
			itmput[16] = itmput[15];
			itmput[15] = i->GetDir();
			itmput[2] = 0x14;
			itmput[11] += 0x80;
		}
		itmput[2] = (UI08)(0x13 + dir);
		s->Send( itmput, 19 + dir );
		if( i->GetID() == 0x2006 )
			openCorpse( s, i->GetSerial() );
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void sendItemsInRange( cSocket *mSock )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Send all items to a player
//o---------------------------------------------------------------------------o
void sendItemsInRange( cSocket *mSock )
{
	if( mSock == NULL )
		return;

	CChar *mChar = mSock->CurrcharObj();
	if( mChar == NULL )
		return;

	int xOffset = MapRegion->GetGridX( mChar->GetX() );
	int yOffset = MapRegion->GetGridY( mChar->GetY() );
	UI08 worldNumber = mChar->WorldNumber();
	SI16 visrange = mSock->Range() + Races->VisRange( mChar->GetRace() );
	SubRegion *toCheck = NULL;

	for( SI08 counter1 = -1; counter1 <= 1; counter1++ )
	{
		for( SI08 counter2 = -1; counter2 <= 1; counter2++ )
		{
			toCheck = MapRegion->GetGrid( xOffset + counter1, yOffset + counter2, worldNumber );
			if( toCheck == NULL )
				continue;

			toCheck->PushItem();
			for( CItem *j = toCheck->FirstItem(); !toCheck->FinishedItems(); j = toCheck->GetNextItem() )
			{
				if( itemInRange( mChar, j, visrange ) ) 
				{
					if( !j->GetVisible() || ( j->GetVisible() && mChar->IsGM() ) )
						sendItem( mSock, j );
				}
			}
			toCheck->PopItem();
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void target( cSocket *s, UI08 a1, UI08 a2, UI08 a3, UI08 a4, const char *txt )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Send targeting cursor to client
//o---------------------------------------------------------------------------o
void target( cSocket *s, UI08 a1, UI08 a2, UI08 a3, UI08 a4, const char *txt )
{
	target( s, calcserial( a1, a2, a3, a4 ), txt );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void target( cSocket *s, SERIAL ser, const char *txt )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Send targeting cursor to client
//o---------------------------------------------------------------------------o
void target( cSocket *s, SERIAL ser, const char *txt )
{
	CPTargetCursor toSend;
	toSend.ID( ser );
	toSend.Type( 1 );
	s->TargetOK( true );
	sysmessage( s, txt );
	s->Send( &toSend );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void target( cSocket *s, UI08 a1, UI08 a2, UI08 a3, UI08 a4, SI32 dictEntry )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Send targeting cursor to client
//o---------------------------------------------------------------------------o
void target( cSocket *s, UI08 a1, UI08 a2, UI08 a3, UI08 a4, SI32 dictEntry )
{
	target( s, calcserial( a1, a2, a3, a4 ), dictEntry );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void target( cSocket *s, SERIAL ser, SI32 dictEntry )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Send targeting cursor to client
//o---------------------------------------------------------------------------o
void target( cSocket *s, SERIAL ser, SI32 dictEntry )
{
	if( s == NULL )
		return;
	target( s, ser, Dictionary->GetEntry( dictEntry, s->Language() ) );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void teleporters( CChar *s )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	If character is on a teleport location, teleport him
//o---------------------------------------------------------------------------o
void teleporters( CChar *s )
{
	if( s == NULL )
		return;
	UI08 charWorld = s->WorldNumber();
	for( UI32 i = 0; i < teleLocs.size(); i++ )
	{
		if( teleLocs[i].srcWorld == -1 || teleLocs[i].srcWorld == charWorld )
		{
			if( s->GetX() == teleLocs[i].src[0] )
			{
				if( s->GetY() == teleLocs[i].src[1] )
				{
					if( teleLocs[i].src[2] == 999 || s->GetZ() == teleLocs[i].src[2] )
					{
						if( teleLocs[i].trgWorld != charWorld )
						{
							s->SetLocation( teleLocs[i].trg[0], teleLocs[i].trg[1], (SI08)teleLocs[i].trg[2], teleLocs[i].trgWorld );
							if( !s->IsNpc() )
								SendMapChange( teleLocs[i].trgWorld, calcSocketObjFromChar( s ) );
						}
						else
							s->SetLocation( teleLocs[i].trg[0], teleLocs[i].trg[1], (SI08)teleLocs[i].trg[2] );
						s->Teleport();
					}
				}
				else if( s->GetY() < teleLocs[i].src[1] )
					break;
			}
			else if( s->GetX() < teleLocs[i].src[0] )
				break;
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void read_in_teleport( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Load teleport locations
//o---------------------------------------------------------------------------o
void read_in_teleport( void )
{
	char text[256];
	int i;
	char seps[]   = " ,\t\n";
	char *token;
	char filename[MAX_PATH];
	sprintf( filename, "%steleport.scp", cwmWorldState->ServerData()->GetScriptsDirectory() ); 
	FILE *fp = fopen( filename, "r" );
	teleLocs.resize( 0 );
	
	if( fp == NULL )
	{
		Console << myendl;
		Console.Error( 1, " Failed to open teleport data script %s", filename );
		Console.Error( 1, " Teleport Data not found" );
		error = true;
		keeprun = false;
		return;
	}
	
	while( !feof( fp ) )
	{
		fgets( text, 255, fp );
		
		if( text[0] != ';' )
		{
			TeleLocationEntry toAdd;
			token = strtok( text, seps );
			i = 0;
			while( token != NULL )
			{
				if( i == 2 )
				{
					if( token[0] == 'A' )
						toAdd.src[2] = 999;
					else
						toAdd.src[2] = (UI16)(makeNum( token ) );
				}
				else if( i < 2 )
					toAdd.src[i] = (UI16)(makeNum( token ) );
				else if( i > 2 && i < 6 )
					toAdd.trg[i - 3] = (UI16)(makeNum( token ) );
				else if( i == 6 )
					toAdd.srcWorld = (SI16)(makeNum( token ) );
				else if( i == 7 )
					toAdd.trgWorld = (UI08)(makeNum( token ) );
					
				i++;
				if( i == 8 )
					break;
				
				token = strtok( NULL, seps );
			}
			teleLocs.push_back( toAdd );
		}
	}
	fclose( fp );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void explodeItem( cSocket *mSock, CItem *nItem )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Explode an item
//o---------------------------------------------------------------------------o
void explodeItem( cSocket *mSock, CItem *nItem )
{
	CChar *c = mSock->CurrcharObj();
	UI32 dmg = 0;
	UI32 dx, dy, dz;
	// - send the effect (visual and sound)
	if( nItem->GetCont() != INVALIDSERIAL )
	{
		staticeffect( c, 0x36B0, 0x00, 0x09 );
		nItem->SetCont( INVALIDSERIAL );
		nItem->SetLocation(  c );
		soundeffect( c, 0x0207 );
	}
	else
	{
		staticeffect( nItem, 0x36B0, 0x00, 0x09, 0x00);
		soundeffect( nItem, 0x0207 );
	}
	UI32 len = nItem->GetMoreX() / 250; //4 square max damage at 100 alchemy
	dmg = RandomNum( nItem->GetMoreZ() * 5, nItem->GetMoreZ() * 10 );
	
	if( dmg < 5 ) 
		dmg = RandomNum( 5, 10 );  // 5 points minimum damage
	if( len < 2 ) 
		len = 2;  // 2 square min damage range
	SI16 xOffset = MapRegion->GetGridX( nItem->GetX() );
	SI16 yOffset = MapRegion->GetGridY( nItem->GetY() );
	
	UI08 worldNumber = nItem->WorldNumber();

	for( dx = -1; dx <= 1; dx++ )
		for( dy = -1; dy <= 1; dy++ )
		{
			SubRegion *Cell = MapRegion->GetGrid( xOffset+dx, yOffset+dy, worldNumber );
			bool chain = false;
	
			Cell->PushChar();
			Cell->PushItem();
			for( CChar *tempChar = Cell->FirstChar(); !Cell->FinishedChars(); tempChar = Cell->GetNextChar() )
			{
				dx = abs( tempChar->GetX() - nItem->GetX() );
				dy = abs( tempChar->GetY() - nItem->GetY() );
				dz = abs( tempChar->GetZ() - nItem->GetZ() );
				if( dx <= len && dy <= len && dz <= len )
				{
					if( !tempChar->IsGM() && !tempChar->IsInvulnerable() && ( tempChar->IsNpc() || isOnline( tempChar ) ) )
					{
						if( tempChar->IsInnocent() && tempChar->GetSerial() != c->GetSerial() )
							criminal( c );
						tempChar->SetHP( (UI16)( tempChar->GetHP() - ( dmg + ( 2 - min( dx, dy ) ) ) ) );
						updateStats( tempChar, 0 );
						if( tempChar->GetHP() <= 0 )
							doDeathStuff( tempChar );
						else
							npcAttackTarget( tempChar, c );
					}
				}
			}
			Cell->PopChar();
			for( CItem *tempItem = Cell->FirstItem(); !Cell->FinishedItems(); tempItem = Cell->GetNextItem() )
			{
				if( tempItem->GetID() == 0x0F0D && tempItem->GetType() == 19 )
				{
					dx = abs( nItem->GetX() - tempItem->GetX() );
					dy = abs( nItem->GetY() - tempItem->GetY() );
					dz = abs( nItem->GetZ() - tempItem->GetZ() );
			
					if( dx <= 2 && dy <= 2 && dz <= 2 && !chain ) // only trigger if in a 2*2*2 cube
					{
						if( !( dx == 0 && dy == 0 && dz == 0 ) )
						{
							if( RandomNum( 0, 1 ) == 1 ) 
								chain = true;
							tempeffect( c, tempItem, 17, 0, 1, 0 );
						}
					}
				}
			}
			Cell->PopItem();
		}
	Items->DeleItem( nItem );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void skillwindow( cSocket *s )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Opens the skills list
//o---------------------------------------------------------------------------o
void skillwindow( cSocket *s )
{
	CPSkillsValues toSend = (*(s->CurrcharObj()));
	s->Send( &toSend );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void updateStats( CChar *c, char x )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Updates characters stats
//o---------------------------------------------------------------------------o
void updateStats( CChar *c, char x)
{
	CPUpdateStat toSend( (*c), x );
	Network->PushConn();
	for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
	{
		if( charInRange( tSock->CurrcharObj(), c ) )
			tSock->Send( &toSend );
	}
	Network->PopConn();
}

//o---------------------------------------------------------------------------o
//|	Function	-	void statwindow( cSocket *s, CChar *i )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Opens the status window
//o---------------------------------------------------------------------------o
void statwindow( cSocket *s, CChar *i )
{
	if( s == NULL || i == NULL ) 
		return;

	CPStatWindow toSend = (*i);
	
	CChar *mChar = s->CurrcharObj();
	//Zippy 9/17/01 : fixed bug of your name on your own stat window
	toSend.NameChange( mChar != i && ( mChar->IsGM() || i->GetOwner() == mChar->GetSerial() ) );
	toSend.Gold( calcGold( i ) );
	toSend.AC( Combat->calcDef( i, 0, false ) );
	toSend.Weight( i->GetWeight() );
	s->Send( &toSend );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void srequest( cSocket *s )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Updates players status
//o---------------------------------------------------------------------------o
void srequest( cSocket *s )
{
	if( s->GetByte( 5 ) == 4 )
		statwindow( s, calcCharObjFromSer( s->GetDWord( 6 ) ) );
	if( s->GetByte( 5 ) == 5 )
		skillwindow( s );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void updates( cSocket *s )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Opens the Updates window
//o---------------------------------------------------------------------------o
void updates( cSocket *s )
{
	ScriptSection *Updates = FileLookup->FindEntry( "MOTD", misc_def );
	if( Updates == NULL )
		return;
	int y = 0;
	char updateData[2048];
	const char *tag = NULL;
	const char *data = NULL;
	updateData[0] = 0;
	for( tag = Updates->First(); !Updates->AtEnd(); tag = Updates->Next() )
	{
		data = Updates->GrabData();
		sprintf( updateData, "%s%s %s ", updateData, tag, data );
	}
	y += strlen( updateData );
	y += 10;
	updscroll[1] = (UI08)(y>>8);
	updscroll[2] = (UI08)(y%256);
	updscroll[3] = 2;
	updscroll[8] = (UI08)((y-10)>>8);
	updscroll[9] = (UI08)((y-10)%256);
	s->Send( updscroll, 10 );
	s->Send( updateData, strlen( updateData ) );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void tips( cSocket *s, int I )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Opens Tip of the Day window
//o---------------------------------------------------------------------------o
void tips( cSocket *s, UI08 i )
{
	ScriptSection *Tips = FileLookup->FindEntry( "TIPS", misc_def );
	if( Tips == NULL )
		return;

	if( i == 0 ) 
		i = 1;

	int x = i;
	const char *tag = NULL;
	const char *data = NULL;
	char temp[1024];
	for( tag = Tips->First(); !Tips->AtEnd(); tag = Tips->Next() )
	{
		if( tag != NULL && !strcmp( tag, "TIP" ) )
			x--;
		if( x <= 0 )
			break;
	}
	if( x > 0 )
		tag = Tips->Prev();
	data = Tips->GrabData();
	sprintf(temp, "TIP %i", makeNum( data ) );
	Tips = FileLookup->FindEntry( temp, misc_def );
	if( Tips == NULL )
		return;
	x = -1;
	int y = 0;
	char tipData[2048];
	tipData[0] = 0;
	for( tag = Tips->First(); !Tips->AtEnd(); tag = Tips->Next() )
	{
		data = Tips->GrabData();
		sprintf( tipData, "%s%s %s ", tipData, tag, data );
	}

	y += strlen( tipData );
	y += 10;
	updscroll[1] = (UI08)(y>>8);
	updscroll[2] = (UI08)(y%256);
	updscroll[3] = 0;
	updscroll[7] = i;
	updscroll[8]= (UI08)((y-10)>>8);
	updscroll[9]= (UI08)((y-10)%256);
	s->Send( updscroll, 10 );
	s->Send( tipData, strlen( tipData ) );
}

//o---------------------------------------------------------------------------o
//|   Function    :  bool autoStack( cSocket *mSock, CItem *i, CItem *pack )
//|   Date        :  8/14/01
//|   Programmer  :  Zane
//|	  Modified	  :	 Abaddon, 9th September, 2001, returns true if item deleted
//o---------------------------------------------------------------------------o
//|   Purpose     :  Searches pack for pileable items that match the item being
//|					 dropped into said pack (only if it's pileable), if found
//|					 ensures the amount won't go over 65535 (the limit how large
//|					 an item can stack) then stacks it. If the item is not stackable
//|					 or it cannot stack the item with a pile and have an amount that
//|					 is <= 65355 then it creates a new pile.
//|									
//|	Modification	-	09/25/2002	-	Brakthus - Weight fixes
//o---------------------------------------------------------------------------o
bool autoStack( cSocket *mSock, CItem *i, CItem *pack )
{
	if( !mSock ) 
		return false;
	CChar *mChar = mSock->CurrcharObj();
	if( mChar == NULL || i == NULL || pack == NULL )
		return false;

	UI32 newAmt;
	cSocket *tSock = NULL;

	if( i->isPileable() )
	{
		for( CItem *stack = pack->FirstItemObj(); !pack->FinishedItems(); stack = pack->NextItemObj() )
		{
			if( stack == NULL )
				continue;

			if( stack->isPileable() && stack->GetSerial() != i->GetSerial() &&
				stack->GetID() == i->GetID() && stack->GetColour() == i->GetColour() &&
				stack->GetAmount() < MAX_STACK )
			{ // Autostack
				newAmt = stack->GetAmount() + i->GetAmount();
				if( newAmt > MAX_STACK )
				{
					i->SetAmount( ( newAmt - MAX_STACK ) );
					stack->SetAmount( MAX_STACK );
					RefreshItem( stack );
				}
				else
				{
					CPRemoveItem toRemove = (*i);
					Network->PushConn();
					for( tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
						tSock->Send( &toRemove );
					Network->PopConn();

					stack->SetAmount( newAmt );
					// Sept 25, 2002 - Brakthus
					Weight->AddItemWeight(i, mChar); 
					Items->DeleItem( i );
					RefreshItem( stack );
					if( mSock != NULL )
					{
						statwindow( mSock, mChar );
						itemsfx( mSock, stack->GetID(), false );
					}
					return true;
				}
			}
		}
	}

	i->SetCont( pack->GetSerial() );

	i->SetX( (UI16)( 20 + RandomNum( 0, 79 ) ) );
	i->SetY( (UI16)( 40 + RandomNum( 0, 99 ) ) );
	i->SetZ( 9 );

	CPRemoveItem toRemove = (*i);
	Network->PushConn();
	for( tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
		tSock->Send( &toRemove );
	Network->PopConn();

	RefreshItem( i );

	if( mSock != NULL )
	{
		statwindow( mSock, mChar );
		itemsfx( mSock, i->GetID(), false );
	}
	return false;
}

//o---------------------------------------------------------------------------o
//|   Function    :  void grabItem( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when a player picks up an item
//o---------------------------------------------------------------------------o
void grabItem( cSocket *mSock )
{
	SERIAL serial = mSock->GetDWord( 1 );
	if( serial == INVALIDSERIAL ) 
		return;

	CItem *i = calcItemObjFromSer( serial );
	if( i == NULL ) 
		return;

	CChar *mChar = mSock->CurrcharObj();

	mChar->BreakConcentration( mSock );
	CPBounce bounce( 0 );

	CItem *x = i;
	CChar *npc = NULL;
	if( i->GetCont() != INVALIDSERIAL )  //Find character owning item
	{
		mSock->PickupSerial( i->GetCont() );
		if( i->GetCont() < 0x40000000 )
			mSock->PickupSpot( PL_PAPERDOLL );
		else
		{
			if( getPackOwner( i ) != mChar )
				mSock->PickupSpot( PL_OTHERPACK );
			else
				mSock->PickupSpot( PL_OWNPACK );
		}
		CChar *npc = getPackOwner( i );
		if( npc != NULL )
		{
			if( !mChar->IsGM() && npc != mChar && npc->GetOwner() != mChar->GetSerial() )
			{
				mSock->Send( &bounce );
				return;
			}
		}
		else
		{
			x = getRootPack( i );
			if( x != NULL )
			{
				if( x->isCorpse() )
				{
					CChar *corpseTargChar = (CChar *)x->GetOwnerObj();
					if( corpseTargChar != NULL )
					{
						if( corpseTargChar->IsGuarded() ) // Is the corpse being guarded?
							petGuardAttack( mChar, corpseTargChar, corpseTargChar->GetSerial() );
						else if( x->isGuarded() )
							petGuardAttack( mChar, corpseTargChar, x->GetSerial() );
					}
				}
				else if( x->GetLayer() == 0 && x->GetID() == 0x1E5E ) // Trade Window
				{
					serial = x->GetMoreB();
					if( serial == INVALIDSERIAL ) 
						return;
					CItem *z = calcItemObjFromSer( serial );
					if( z != NULL )
					{
						if( z->GetMoreZ() || x->GetMoreZ() )
						{
							z->SetMoreZ( 0 );
							x->SetMoreZ( 0 );
							sendTradeStatus( z, x );
						}
						// Default item pick up sound sent to other player involved in trade
						cSocket *zSock = calcSocketObjFromChar( calcCharFromSer( z->GetCont() ) );
					}
				}
			}
		}
	}
	else
	{
		mSock->PickupSpot( PL_GROUND );
		mSock->PickupLocation( i->GetX(), i->GetY(), i->GetZ() );
	}

	if( i->isCorpse() || !checkItemRange( mChar, x, 3 ) )
	{
		mSock->Send( &bounce );
		return;
	}

	if( x->GetMulti() != INVALIDSERIAL )
	{
		if( ( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND ) && x->GetMulti() != mChar->GetMulti() ) 
		{
			mSock->Send( &bounce );
			return;
		}
		i->SetMulti( INVALIDSERIAL );
	}

	if(i->isDecayable())
		i->SetDecayTime( BuildTimeValue( (R32)cwmWorldState->ServerData()->GetSystemTimerStatus( DECAY ) ) );

	SERIAL OwnSer = i->GetCont();
	if( OwnSer < 0x40000000  )
	{
		CChar *pChar = calcCharObjFromSer( OwnSer );
		if( pChar ) 
			pChar->TakeOffItem( i->GetLayer() );
	} 
	else 
	{
		CItem *pItem = calcItemObjFromSer( OwnSer );
		if( pItem ) 
			pItem->ReleaseItem( i );
	}

	if( i->isGuarded() )
	{
		if( npc != NULL && mSock->PickupSpot() == PL_OTHERPACK )
			petGuardAttack( mChar, npc, i->GetSerial() );

		CChar *petGuard = Npcs->getGuardingPet( mChar, i->GetSerial() );
		if( petGuard != NULL )
			petGuard->SetGuarding( INVALIDSERIAL );
		i->SetGuarded( false );
	}

	CTile tile;
	Map->SeekTile( i->GetID(), &tile );
	if( ( ( i->GetMagic() == 2 || ( tile.Weight() == 255 && i->GetMagic() != 1 ) ) && !mChar->AllMove() ) || 
		( i->GetMagic() == 3 &&  i->GetOwner() != mChar->GetSerial() ) )
	{
		mSock->Send( &bounce );
		if( i->GetID( 1 ) >= 0x40 )
			sendItem( mSock, i );
	}
	else
	{
		soundeffect( mSock, 0x0057, true );
		if( i->GetAmount() > 1 )
		{
			UI16 amount = mSock->GetWord( 5 );
			if( amount > i->GetAmount() ) 
				amount = i->GetAmount();
			if( amount < i->GetAmount() )
			{
				ITEM tItem;
				CItem *c = i->Dupe( tItem );
				if( c != NULL )
				{
					c->SetAmount( i->GetAmount() - amount );
					c->SetCont( i->GetCont() );
					if( c->GetSpawn() != 0 )
						nspawnsp.AddSerial( c->GetSpawn(), tItem );

					RefreshItem( c );
				}
			}
			i->SetAmount( amount );
			if( i->GetID() == 0x0EED )
			{
				CItem *packnum = getPack( mChar );
				if( packnum != NULL )
				{
					if( x->GetCont() == packnum->GetSerial() )
						statwindow( mSock, mChar );
				}
			}
		}
		
		i->SetCont( INVALIDSERIAL );
		// Sept 25, 2002 - Brakthus - Weight fixes
		if(mSock->PickupSpot()!=PL_OWNPACK && mSock->PickupSpot()!=PL_PAPERDOLL)
			Weight->AddItemWeight(i, mChar);
		MapRegion->RemoveItem( i );
		CPRemoveItem remove( *i );
		Network->PushConn();
		for( cSocket *pSock = Network->FirstSocket(); !Network->FinishedSockets(); pSock = Network->NextSocket() )
		{
			if( !pSock )
				continue;
			if( isOnline( pSock->CurrcharObj() ) )
				pSock->Send( &remove );
		}
		Network->PopConn();
	}

	if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
	{
		// EviLDeD: 031002: Why would we be adding weight when the object bounces?
		// Weight->AddItemWeight( i, mChar );
		statwindow( mSock, mChar );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void wearItem( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when an item is dropped on a players paperdoll
//o---------------------------------------------------------------------------o
void wearItem( cSocket *mSock )
{
	CChar *mChar = mSock->CurrcharObj();

	SERIAL cserial = mSock->GetDWord( 6 );
	SERIAL iserial = mSock->GetDWord( 1 );
	if( cserial == INVALIDSERIAL || iserial == INVALIDSERIAL ) 
		return;
	
	CPBounce bounce( 5 );
	CItem *i = calcItemObjFromSer( iserial );
	CChar *k = calcCharObjFromSer( cserial );
	if( i == NULL )
		return;

	if( !mChar->IsGM() && k != mChar )	// players cant equip items on other players or npc`s paperdolls.  // GM PRIVS
	{
		Bounce( mSock, i );
		sysmessage( mSock, 1186 );
		if( i->GetID( 1 ) >= 0x40 )
			sendItem( mSock, i );
		return;
	}
	if( i->GetCont() != INVALIDSERIAL )
	{
		Bounce( mSock, i );
		if( i->GetID( 1 ) >= 0x40 )
			sendItem( mSock, i );
		return;
	}
	if( mChar->IsDead() )
	{
		sysmessage( mSock, 1185 );
		return;
	}
	if( k == NULL ) 
		return;

	UI08 ac1 = Races->ArmorRestrict( k->GetRace() );
	UI08 ac2 = i->GetArmourClass();

	if( ac1 != 0 && ( (ac1&ac2) == 0 ) )	// bit comparison, if they have ANYTHING in common, they can wear it
	{
		sysmessage( mSock, 1187 );
		Bounce( mSock, i );
		if( i->GetID( 1 ) >= 0x40 ) 
			sendItem( mSock, i );
		if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
		{
			//Weight->SubtractItemWeight( i, mChar );
			statwindow( mSock, mChar );
		}
		return;
	}
	if( k == mChar )
	{
		bool canWear = false;
		if( i->GetStrength() > k->GetStrength() )
			sysmessage( mSock, 1188 );
		else if( i->GetDexterity() > k->GetDexterity() )
			sysmessage( mSock, 1189 );
		else if( i->GetIntelligence() > k->GetIntelligence() )
			sysmessage( mSock, 1190 );
		else
			canWear = true;
		if( !canWear )
		{
			Bounce( mSock, i );

			if( i->GetID( 1 ) >= 0x40 )  //????
				sendItem( mSock, i );

			if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
			{
				Weight->SubtractItemWeight( i, mChar );
				statwindow( mSock, mChar );
				itemsfx( mSock, i->GetID(), true );
				
			}
			else
				itemsfx( mSock, i->GetID(), false );

			RefreshItem( i );

			return;
		}
	}
	CTile tile;
	Map->SeekTile( i->GetID(), &tile);
	if( ( ( i->GetMagic() == 2 || ( tile.Weight() == 255 && i->GetMagic() != 1 ) ) && !mChar->AllMove() ) ||
		( i->GetMagic() == 3 && i->GetOwner() != mChar->GetSerial() ) )
	{
		Bounce( mSock, i );
		if( i->GetID( 1 ) >= 0x40 ) 
			sendItem( mSock, i );
		return;
	}

	if( i->GetLayer() == 0 )
		i->SetLayer( mSock->GetByte( 5 ) );

	CItem *j = NULL;

  int layer = i->GetLayer();
	// 1/13/2003 - Xuri - Fix for equiping an item to more than one hand, or multiple equiping.
  j = FindItemOnLayer( k, layer );
  if( j != NULL )
  {
		sysmessage( mSock, "You can't equip two items in the same slot." );
    if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
			Weight->SubtractItemWeight( i, mChar );
    statwindow( mSock, mChar );
    Bounce( mSock, i );
    RefreshItem( i );
    if( i->GetID( 1 ) >= 0x40 )
			sendItem( mSock, i );
    return;
  }

/*	if( i->GetLayer() == 1 )
	{
		j = Combat->getWeapon( k );
		if( j != NULL )
		{
			Bounce( mSock, i );
			// Sept 25, 2002 - Xuri - Weight Fixes
			RefreshItem(i);
			sysmessage( mSock, 1192 );
			if( i->GetID( 1 ) >= 0x40 )
				sendItem( mSock, i );
			return;
		}
	}
	else if( i->GetLayer() == 2 )
	{
		j = FindItemOnLayer( k, 2 );
		if( j != NULL )
		{
			Bounce( mSock, i );
			sysmessage( mSock, 1191 );
			if( i->GetID( 1 ) >= 0x40 )
				sendItem( mSock, i );
			return;
		}
	}
*/
	i->SetCont( cserial );

	// Add item weight if pickupspot is not ownpack or paperdoll

	if( showlayer ) 
		Console << "Item equipped on layer " << i->GetLayer() << myendl;

	CPRemoveItem toRemove = (*i);

	Network->PushConn();
	for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
		tSock->Send( &toRemove );
	Network->PopConn();

	RefreshItem( i );

	// Moved that here from Grabitem.
	soundeffect( mSock, 0x0057, false );
	statwindow( mSock, mChar );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void dropItemOnChar( cSocket *mSock, CChar *targChar, CItem *i )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//|	  Modified	  :	 Abaddon, September 14th, 2001, returns true if item deleted
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when an item is dropped on a character
//|									
//|	Modification	-	09/25/2002	-	Xuri/Brakthus - Weight fixes
//o---------------------------------------------------------------------------o
bool dropItemOnChar( cSocket *mSock, CChar *targChar, CItem *i )
{
	CChar *mChar = mSock->CurrcharObj();
	if( mChar == NULL )
		return false;

	bool stackDeleted = false;
	if( targChar == mChar )
	{
		CItem *pack = getPack( mChar );
		if( pack == NULL ) // if player has no pack, put it at its feet
		{ 
			i->SetLocation( mChar );
			// Sept 25, 2002 - Xuri - Weight bugs
			Weight->SubtractItemWeight(i, mChar);
			RefreshItem( i );
		} 
		else
		{
			//if( targChar == mChar && !( mSock->PickupSpot() == PL_OWNPACK || mSock->PickupSpot() == PL_PAPERDOLL ) )
			//	Weight->AddItemWeight( i, mChar );
			stackDeleted = autoStack( mSock, i, pack );
			return true;
		}
	}
	else if( targChar->IsNpc() )
	{
		if( !( targChar->GetTaming() > 1000 || targChar->GetTaming() == 0 ) && i->GetType() == 14 && 
			targChar->GetHunger() <= (SI32)( targChar->ActualStrength() / 10 ) ) // do food stuff
		{
			soundeffect( mSock, 0x003A + RandomNum( 0, 2 ), true );
			npcAction( targChar, 3 );

			if( i->GetPoisoned() && targChar->GetPoisoned() < i->GetPoisoned() )
			{
				soundeffect( mSock, 0x0246, true ); //poison sound - SpaceDog
				targChar->SetPoisoned( i->GetPoisoned() );
				targChar->SetPoisonWearOffTime( BuildTimeValue( cwmWorldState->ServerData()->GetSystemTimerStatus( POISON ) ) );
				targChar->SendToSocket( mSock, true, mChar );
			}
			//Remove a food item
			i = decItemAmount( i );
			targChar->SetHunger( targChar->GetHunger() + 1 );
			if( i == NULL )
				return true; //stackdeleted
		}
		if( targChar->GetID() != 0x0190 && targChar->GetID() != 0x0191 )
		{
			// Sept 25, 2002 - Xuri - Weight fixes
			if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
				Weight->SubtractItemWeight( i, mChar );
			//
			Bounce( mSock, i );
			if( i->GetCont() == INVALIDSERIAL )
			{
				if( i->GetID( 1 ) >= 0x40 ) 
					sendItem( mSock, i );
			}
			else
				sendItem( mSock, i );
		}
		else if( mChar->GetTrainer() != targChar->GetSerial() )
		{
			// Sept 25, 2002 - Xuri - weight fix
			if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
				Weight->SubtractItemWeight( i, mChar );
			//
			npcTalk( mSock, targChar, 1197, false );
			Bounce( mSock, i );
			if( i->GetCont() == INVALIDSERIAL )
			{
				if( i->GetID( 1 ) >= 0x40 ) 
					sendItem( mSock, i );
			}
			else
				sendItem( mSock, i );
		}
		else // This NPC is training the player
		{
			if( i->GetID() == 0x0EED ) // They gave the NPC gold
			{
				Weight->SubtractItemWeight( i, mChar );

				UI08 trainedIn = targChar->GetTrainingPlayerIn();
				npcTalk( mSock, targChar, 1198, false );
				UI16 oldskill = mChar->GetBaseSkill( trainedIn ); 
				mChar->SetBaseSkill( (UI16)( mChar->GetBaseSkill( trainedIn ) + i->GetAmount() ), trainedIn );
				if( mChar->GetBaseSkill( trainedIn ) > 250 ) 
					mChar->SetBaseSkill( 250, trainedIn );
				Skills->updateSkillLevel( mChar, trainedIn );
				updateskill( mSock, trainedIn );
				UI16 sfxID = i->GetID();
				if( i->GetAmount() > 250 ) // Paid too much
				{
					i->SetAmount( i->GetAmount() - 250 - oldskill );
					Bounce( mSock, i );
					if( i->GetCont() == INVALIDSERIAL )
					{
						if( i->GetID( 1 ) >= 0x40 ) 
							RefreshItem(i);
							//sendItem( mSock, i );
					}
					else
						sendItem( mSock, i );
				}
				else  // Gave exact change
				{
					Items->DeleItem( i );
					stackDeleted = true;
				}
				mChar->SetTrainer( INVALIDSERIAL );
				targChar->SetTrainingPlayerIn( 255 );
				itemsfx( mSock, sfxID, false );
			}
			else // Did not give gold
			{
				npcTalk( mSock, targChar, 1199, false );
				Bounce( mSock, i );
				if( i->GetCont() == INVALIDSERIAL )
				{
					if( i->GetID( 1 ) >= 0x40 ) 
						RefreshItem(i);
						//sendItem( mSock, i );
				}
				else
					sendItem( mSock, i );
			}
			//RefreshItem(i); 
		}
	}
	else // Trade stuff
	{
		if( isOnline( targChar ) )
		{
			CItem *j = startTrade( mSock, targChar );
			if ( j )
			{
				i->SetCont( j->GetSerial() );
				i->SetX( 30 );
				i->SetY( 30 );
				i->SetZ( 9 );
				CPRemoveItem toRemove = (*i);

				Network->PushConn();
				for( cSocket *bSock = Network->FirstSocket(); !Network->FinishedSockets(); bSock = Network->NextSocket() )
					bSock->Send( &toRemove );
				Network->PopConn();
		
				RefreshItem( i );
			}
		}
		else if( mChar->GetCommandLevel() >= CNSCMDLEVEL )
		{
			CItem *p = getPack( targChar );
			if( p == NULL )
			{
				Bounce( mSock, i );
				return stackDeleted;
			}
			CPRemoveItem toRemove = (*i);
			Network->PushConn();
			for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
				tSock->Send( &toRemove );
			Network->PopConn();

			Weight->SubtractItemWeight( i, mChar );
			if( mSock->PickupSpot() == PL_OWNPACK || mSock->PickupSpot() == PL_GROUND )
				Weight->AddItemWeight( i, targChar );

			stackDeleted = autoStack( calcSocketObjFromChar( targChar ), i, p );
			if( !stackDeleted )
				RefreshItem( i );
		}
		else
			Bounce( mSock, i );
	}
	return stackDeleted;
}

//o---------------------------------------------------------------------------o
//|   Function    :  void dropItem( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when an item is dropped on the ground
//o---------------------------------------------------------------------------o
void dropItem( cSocket *mSock ) // Item is dropped on ground
{
	CChar *nChar = mSock->CurrcharObj();
	SERIAL serial = mSock->GetDWord( 1 );
	CItem *i = calcItemObjFromSer( serial );
	bool stackDeleted = false;
	
	CPBounce bounce( 5 );
	if( i == NULL ) 
	{ 
		sendItemsInRange( mSock );
		Console << "ALERT: sendItemsInRange() called in dump_item().  This function could cause a lot of lag!" << myendl;
		return;
	}
	if( i->GetCont() != INVALIDSERIAL )
	{
		Bounce( mSock, i );
		if( i->GetID( 1 ) >= 0x40 ) 
			RefreshItem(i);
			//sendItem( mSock, i );
		return;
	}

	CTile tile;
	Map->SeekTile( i->GetID(), &tile);
	if( ( ( i->GetMagic() == 2 || ( tile.Weight() == 255 && i->GetMagic() != 1 ) ) && !nChar->AllMove() ) ||
		( i->GetMagic() == 3 &&  i->GetOwner() != nChar->GetSerial() ) )
	{
		Bounce( mSock, i );
		if( i->GetID( 1 ) >= 0x40 )
			RefreshItem(i);
			//sendItem( mSock, i );
		return;
	}
	
	if( mSock->GetByte( 5 ) != 0xFF )
	{
		// No Glow - Zane
		//Items->GlowItem( i );

		CPRemoveItem toRemove = (*i);
		
		Network->PushConn();
		for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
			tSock->Send( &toRemove );
		Network->PopConn();
		i->SetLocation( mSock->GetWord( 5 ), mSock->GetWord( 7 ), mSock->GetByte( 9 ), nChar->WorldNumber() );
		RefreshItem( i );
		Weight->SubtractItemWeight( i, nChar );
	}
  else
  {
		CChar *t = calcCharObjFromSer( mSock->GetDWord( 10 ) );
    if( t != NULL )
			stackDeleted = dropItemOnChar( mSock, t, i );
    else
    {
			//Bounces items dropped in illegal locations in 3D UO client!!!
      if( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND )
				Weight->SubtractItemWeight( i, nChar );
      statwindow( mSock, nChar );
      Bounce( mSock, i );
      RefreshItem( i );
      if( i->GetID( 1 ) >= 0x40 )
				sendItem( mSock, i );
      return;
    }
	}
	
	statwindow( mSock, nChar );
	if( !stackDeleted )
	{
		if( nChar->GetMulti() != INVALIDSERIAL )
		{
			CMultiObj *multi = findMulti( i->GetX(), i->GetY(), i->GetZ(), i->WorldNumber() );
			if( multi != NULL )
				i->SetMulti( multi );
		}
		itemsfx( mSock, i->GetID(), ( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND ) );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void packItem( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when an item is dropped in a container or on another item
//o---------------------------------------------------------------------------o
void packItem( cSocket *mSock )
{
	CChar *mChar = mSock->CurrcharObj();
	SERIAL serial = mSock->GetDWord( 10 );
	bool stackDeleted = false;

	CItem *nCont = calcItemObjFromSer( serial );
	if( nCont == NULL || mChar == NULL )
		return;

	CItem *nItem = calcItemObjFromSer( mSock->GetDWord( 1 ) );
	if( nItem == NULL ) 
		return;

	char temp[1024];
	CPBounce bounce( 5 );

	if( nCont->GetLayer() == 0 && nCont->GetID() == 0x1E5E && nCont->GetCont() == mChar->GetSerial() )
	{	// Trade window
		CItem *z = calcItemObjFromSer( nCont->GetMoreB() );
		if( z != NULL )
		{
			if( z->GetMoreZ() || nCont->GetMoreZ() )
			{
				z->SetMoreZ( 0 );
				nCont->SetMoreZ( 0 );
				sendTradeStatus( z, nCont );
			}
			cSocket *zSock = calcSocketObjFromChar( calcCharObjFromSer( z->GetCont() ) );
			if( zSock != NULL )
				itemsfx( zSock, nCont->GetID(), ( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND ) );
		}
		return;
	}

	if( cwmWorldState->ServerData()->GetWildernessBankStatus() ) // only if special bank is activated
	{
		if( nCont->GetMoreY() == 123 && nCont->GetMoreX() == 1 && nCont->GetType() == 1 )
		{
			if( nItem->GetID() == 0x0EED )
				goldsfx( mSock, 2 );
			else // If not gold, bounce to the ground
			{
				sysmessage( mSock, 1200 );

				nItem->SetCont( INVALIDSERIAL );
				nItem->SetLocation( mChar );
				RefreshItem( nItem );
				itemsfx( mSock, nItem->GetID(), false );
			}
			Weight->SubtractItemWeight( nItem, mChar );
			statwindow( mSock, mChar );
			return;
		}
	}

	if( nItem->GetCont() != INVALIDSERIAL )
	{
		Bounce( mSock, nItem );
		if( nItem->GetID( 1 ) >= 0x40 )
			RefreshItem(nItem);
			//sendItem( mSock, nItem );
		return;
	}

	CTile tile;
	Map->SeekTile( nItem->GetID(), &tile);
	if( ( ( nItem->GetMagic() == 2 || ( tile.Weight() == 255 && nItem->GetMagic() != 1 && !nCont->isCorpse() ) ) && mChar->AllMove() ) ||
		( nItem->GetMagic() == 3 && nItem->GetOwner() != mChar->GetSerial() ) )
	{
		Bounce( mSock, nItem );
		RefreshItem(nItem);
		if( nCont->GetID( 1 ) >= 0x40 ) 
			sendItem( mSock, nCont );
		return;
	}

	if( nCont->GetType() == 87 )	// Trash container
	{
		soundeffect( mSock, 0x0042, false );
		Weight->SubtractItemWeight( nItem, mChar );
		statwindow( mSock, mChar );
		Items->DeleItem( nItem );
		sysmessage( mSock, 1201 );
		return;
	}
	else if( nCont->GetType() == 9 )	// Spell Book
	{
		if( nItem->GetID( 1 ) != 0x1F || nItem->GetID( 2 ) < 0x2D || nItem->GetID( 2 ) > 0x72 )
		{
			Bounce( mSock, nItem );
			sysmessage( mSock, 1202 );
			if( nCont->GetID( 1 ) >= 0x40 ) 
				sendItem( mSock, nCont );
			return;
		}
		CChar *c = getPackOwner( nCont );
		if( c != NULL && c != mChar && !mChar->CanSnoop() )
		{
			Bounce( mSock, nItem );
			RefreshItem(nItem);
			sysmessage( mSock, 1203 );
			if( nCont->GetID( 1 ) >= 0x40 ) 
				sendItem( mSock, nCont );
			return;
		}
		if( nItem->GetName()[0] == '#' )
			getTileName( nItem, temp );
		else
			strcpy( temp, nItem->GetName() );

		if( nCont->GetMore( 1 ) == 1 )	// using more1 to "lock" a spellbook for RP purposes
		{
			sysmessage( mSock, 1204 );
			Bounce( mSock, nItem );
			return;
		}

		if( !strcmp( temp, Dictionary->GetEntry( 1605 ) ) )
		{
			if( nCont->GetMoreX() == 0xFFFFFFFF && nCont->GetMoreY() == 0xFFFFFFFF && nCont->GetMoreZ() == 0xFFFFFFFF )
			{
				sysmessage( mSock, 1205 );
				Bounce( mSock, nItem );
				return;
			}
			nCont->SetMoreX( 0xFFFFFFFF );
			nCont->SetMoreY( 0xFFFFFFFF );
			nCont->SetMoreZ( 0xFFFFFFFF );
		}
		else
		{
			int targSpellNum = nItem->GetID() - 0x1F2D;
			if( Magic->HasSpell( nCont, targSpellNum ) )
			{
				sysmessage( mSock, 1206 );
				Bounce( mSock, nItem );
				return;
			}
			else
				Magic->AddSpell( nCont, targSpellNum );
		}
		soundeffect( mSock, 0x0042, false );
		Weight->SubtractItemWeight( nItem, mChar );
		statwindow( mSock, mChar );
		Items->DeleItem( nItem );
		return;
	}
	else if( nCont->isPileable() && nItem->isPileable() && nCont->GetID() == nItem->GetID() && nCont->GetColour() == nItem->GetColour() )
	{	// Stacking		
		if( (UI32)(nCont->GetAmount() + nItem->GetAmount()) > MAX_STACK )
		{
			nItem->SetAmount( ( nItem->GetAmount() + nCont->GetAmount() ) - MAX_STACK );
			nCont->SetAmount( MAX_STACK );
			Bounce( mSock, nItem );
			RefreshItem( nItem );
		}
		else
		{
			CPRemoveItem toRemove = (*nItem);
			
			CChar *nChar = getPackOwner( nCont );
			nCont->SetAmount( nCont->GetAmount() + nItem->GetAmount() );
			RefreshItem(nCont);
			if(nChar)
				Weight->AddItemWeight(nItem,nChar);
			if(mChar)
				Weight->SubtractItemWeight( nItem, mChar );

			Items->DeleItem( nItem );

			Network->PushConn();
			for( cSocket *bSock = Network->FirstSocket(); !Network->FinishedSockets(); bSock = Network->NextSocket() )
				bSock->Send( &toRemove );
			Network->PopConn();
		}

		RefreshItem( nCont );
		itemsfx( mSock, nCont->GetID(), true );
		statwindow( mSock, mChar );
		return;
	}                                                                                                                     
	else if( nCont->GetType() == 1 )
	{
		CChar *j = NULL;
		j = getPackOwner( nCont );

		nItem->SetCont( nCont->GetSerial() );
		if( mSock->GetByte( 5 ) != 0xFF )
		{
			nItem->SetX( mSock->GetWord( 5 ) );
			nItem->SetY( mSock->GetWord( 7 ) );
			if( j != NULL )
			{
				if( j->IsNpc() && j->GetNPCAiType() == 17 && j->GetOwner() == mChar->GetSerial() )
				{
					mChar->SetSpeechMode( 3 );
					mChar->SetSpeechItem( nItem->GetSerial() );
					sysmessage( mSock, 1207 );
					Weight->SubtractItemWeight(nItem,mChar);
				}
				else if( j != mChar && mChar->GetCommandLevel() < CNSCMDLEVEL )
				{
					sysmessage( mSock, 1630 );
					Bounce( mSock, nItem );
					RefreshItem(nItem);
					return;
				}
			}
			else
			{
				Weight->SubtractItemWeight(nItem,mChar);
			}
			nItem->SetZ( 9 );

			CPRemoveItem toRemove = (*nItem);
			Network->PushConn();
			for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
				tSock->Send( &toRemove );
			Network->PopConn();

			RefreshItem( nItem );

			//if( j != NULL && j != mChar )
			//	Weight->SubtractItemWeight( nItem, mChar );
			//else if( j != NULL && j == mChar && !( mSock->PickupSpot() == PL_OWNPACK || mSock->PickupSpot() == PL_PAPERDOLL ) )
			//	Weight->AddItemWeight( nItem, mChar );

			statwindow( mSock, mChar );

		} 
		else
		{
			if( j != NULL )
			{
				if( j->GetNPCAiType() == 17 && j->IsNpc() && j->GetOwner() == mChar->GetSerial() )
				{
					mChar->SetSpeechMode( 3 );
					mChar->SetSpeechItem( nItem->GetSerial() );
					sysmessage( mSock, 1207 );
				}
				else if( j->GetSerial() != mChar->GetSerial() && !mChar->IsGM() && !mChar->IsCounselor() )
				{
					sysmessage( mSock, 1630 );
					Bounce( mSock, nItem );
					return;
				}
			}
			stackDeleted = autoStack( mSock, nItem, nCont );
		}
	}
	else if( nCont->GetType() == 63 || nCont->GetType() == 65 )
	{	// - Unlocked item spawner or unlockable item spawner
		MapRegion->RemoveItem( nItem );
		nItem->SetCont( mSock->GetDWord( 10 ) );
		
		nItem->SetX( mSock->GetWord( 5 ) );
		nItem->SetY( mSock->GetWord( 7 ) );
		nItem->SetZ( mSock->GetByte( 9 ) );

		CPRemoveItem toRemove = (*nItem);
		Network->PushConn();
		for( cSocket *jSock = Network->FirstSocket(); !Network->FinishedSockets(); jSock = Network->NextSocket() )
			jSock->Send( &toRemove );
		Network->PopConn();
		RefreshItem( nItem );
		Weight->SubtractItemWeight( nItem, mChar );
		statwindow( mSock, mChar );
	}
	else
	{
		MapRegion->RemoveItem( nItem );
		nItem->SetX( mSock->GetWord( 5 ) );
		nItem->SetY( mSock->GetWord( 7 )  );
		nItem->SetZ( mSock->GetByte( 9 ) );
		nItem->SetCont( INVALIDSERIAL );
		MapRegion->AddItem( nItem );
		
		CPRemoveItem toRemove = (*nItem);
		Network->PushConn();
		for( cSocket *aSock = Network->FirstSocket(); !Network->FinishedSockets(); aSock = Network->NextSocket() )
			aSock->Send( &toRemove );
		Network->PopConn();
		RefreshItem( nCont );
		Weight->SubtractItemWeight( nItem, mChar );
		statwindow( mSock, mChar );
	}
	if( !stackDeleted )
	{
		// No Glow - Zane
		// Items->GlowItem( nItem );
		if( getPackOwner( nItem ) == mChar )
			soundeffect( mSock, 0x0057, false );
		else
			itemsfx( mSock, nItem->GetID(), ( mSock->PickupSpot() == PL_OTHERPACK || mSock->PickupSpot() == PL_GROUND ) );
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void MountCreature( CChar *s, CChar *x )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Mount a ridable creature
//|									
//|	Modification	-	09/22/2002	-	Xuri - Unhide players when mounting horses etc.
//o---------------------------------------------------------------------------o
//|	Returns				- N/A
//o--------------------------------------------------------------------------o	
void MountCreature( CChar *s, CChar *x )
{
	cSocket *sockPtr = calcSocketObjFromChar( s );
	if( objInRange( s, x, 2 ) == 0 ) 
		return;
	char temp[1024];
	if( x->GetOwner() == s->GetSerial() || s->IsGM() )
	{
		if(s->IsOnHorse() )
		{
			if(!cwmWorldState->ServerData()->GetCharHideWhileMounted())
				s->ExposeToView();
		}
		strcpy( temp, x->GetName() );
		s->SetOnHorse( true );
		CItem *c = Items->SpawnItem( sockPtr, 1, temp, false, 0x0915, x->GetSkin(), false, false );

		// Weazel 12 July, 2001 - Not all 3rd dawn creature mount id's are correct still missing a faction horse/dragon horse and
		// the ethereal llama and ostards.
		switch( x->GetID() )
		{
		case 0xC8:	c->SetID( 0x3E9F );	break;	// horse
		case 0xE2:	c->SetID( 0x3EA0 );	break;	// horse
		case 0xE4:	c->SetID( 0x3EA1 );	break;	// horse
		case 0xCC:	c->SetID( 0x3EA2 );	break;	// horse
		case 0xD2:	c->SetID( 0x3EA3 );	break;	// Desert Ostard
		case 0xDA:	c->SetID( 0x3EA4 );	break;	// Frenzied Ostard
		case 0xDB:	c->SetID( 0x3EA5 );	break;	// Forest Ostard
		case 0xDC:	c->SetID( 0x3EA6 );	break;	// llama
		case 0x75:	c->SetID( 0x3EA8 );	break;	// Silver Steed
		case 0x72:	c->SetID( 0x3EA9 );	break;	// Dark Steed
		case 0x73:	c->SetID( 0x3EAA );	break;	// Etheral Horse
		case 0xAA:	c->SetID( 0x3EAB );	break;	// Etheral Llama
		case 0xAB:	c->SetID( 0x3EAC );	break;	// Etheral Ostard
		case 0x84:	c->SetID( 0x3EAD );	break;	// Unicorn
		case 0x78:	c->SetID( 0x3EAF );	break;	// Faction Horse
		case 0x79:	c->SetID( 0x3EB0 );	break;	// Faction Horse
		case 0x77:	c->SetID( 0x3EB1 );	break;	// Faction Horse
		case 0x76:	c->SetID( 0x3EB2 );	break;	// Faction Horse
		case 0x8A:	c->SetID( 0x3EB4 );	break;	// Dragon Horse
		case 0x74:	c->SetID( 0x3EB5 );	break;	// Nightmare
		case 0xBB:	c->SetID( 0x3EB8 );	break;	// Ridgeback
		case 0x319:     c->SetID( 0x3EBB );     break;  // Skeletal Mount 
		case 0x317:     c->SetID( 0x3EBC );     break;  // Giant Beetle  
		case 0x31A:     c->SetID( 0x3EBD );     break;  // Swamp Dragon  
		case 0x31F:     c->SetID( 0x3EBE );     break;  // Armored Swamp Dragon  

		default:	c->SetID( 0x3E00 );	break;	// Bad
		}
		
		c->SetLayer( 0x19 );

		if( !c->SetCont( s->GetSerial() ) )
		{
			s->SetOnHorse( false );	// let's get off our horse again
			return;
		}
		//s->WearItem( c );
		wornItems( sockPtr, s ); // send update to current socket
		Network->PushConn();
		for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() ) // and to all inrange sockets (without re-sending to current socket )
		{
			if( sockPtr != tSock && charInRange( sockPtr, tSock ) )
				wornItems( tSock, s );
		}
		Network->PopConn();

		if( x->GetTarg() != INVALIDSERIAL )	// zero out target, under all circumstances
		{
			x->SetTarg( INVALIDSERIAL );
			x->SetWar( false );
		}
		if( x->GetAttacker() != INVALIDSERIAL && x->GetAttacker() < cmem )
			chars[x->GetAttacker()].SetTarg( INVALIDSERIAL );
		x->SetLocation( 7000, 7000, 0 );
		x->SetFrozen( true );
		c->SetMoreX( x->GetSerial() );
		if( x->GetSummonTimer() != 0 )
			c->SetDecayTime( x->GetSummonTimer() );

		x->Update();
	}
	else
		sysmessage( sockPtr, 1214 );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void DismountCreature( CChar *s )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Dismount a ridable creature
//o---------------------------------------------------------------------------o
void DismountCreature( CChar *s )
{
	if( s == NULL )
		return;
	CItem *ci = s->GetItemAtLayer( 0x19 );
	if( ci == NULL || ci->isFree() )	// if no horse item, or it's our default one
		return;

	s->SetOnHorse( false );
	CChar *tMount = calcCharObjFromSer( ci->GetMoreX() );
	if( tMount != NULL )
	{
		tMount->SetLocation( s );
		tMount->SetFrozen( false );
		if( ci->GetDecayTime() != 0 )
			tMount->SetSummonTimer( ci->GetDecayTime() );
		tMount->SetVisible( 0 );
		tMount->Update();
	}
	Items->DeleItem( ci );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void endmessage( int x )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Global message players with shutdown message
//o---------------------------------------------------------------------------o
void endmessage( int x )
{
	x = 0;
	UI32 igetclock = uiCurrentTime;
	if( endtime < igetclock )
		endtime = igetclock;
	char temp[1024];
	sprintf( temp, Dictionary->GetEntry( 1209 ), ((endtime-igetclock)/CLOCKS_PER_SEC) / 60 );
	sysbroadcast( temp );
}

#ifdef __LINUX__
void illinst( int x = 0 ) //Thunderstorm linux fix
{
	sysbroadcast( "Fatal Server Error! Bailing out - Have a nice day!" );
	Console.Error( 0, "Illegal Instruction Signal caught - attempting shutdown" );
	endmessage( x );
}

void aus( int signal )
{
	Console.Error( 2, "Server crash averted! Floating point exception caught." );
} 

#endif

//o---------------------------------------------------------------------------o
//|	Function	-	void weblaunch( cSocket *s, char *txt )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Launch a webpage from within the client
//o---------------------------------------------------------------------------o
void weblaunch( cSocket *s, const char *txt )
{
	sysmessage( s, 1210 );
	CPWebLaunch toSend( txt );
	s->Send( &toSend );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void scriptcommand( cSocket *s, char *cmd, char *data )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Execute a command from scripts
//o---------------------------------------------------------------------------o
void scriptcommand( cSocket *s, const char *cmd2, const char *data2 )
{
	char tstring[1024];
	int total, ho, mi, se, tmp;
	CChar *mChar =s->CurrcharObj();
	char cmd[512], data[512];
	if ( !s || !cmd2 || !data2 )
		return;

	strcpy( cmd, cmd2 );
	strcpy( data, data2 );
	strupr( cmd );
	strupr( data );
	
	switch( cmd[0] )
	{
	case 'A':
		if( !strcmp( "ADDITEM", cmd ) )
		{
			Skills->MakeMenuTarget( s, data, mChar->GetMaking() );
			mChar->SetMaking( 0 );
		}
		break;
	case 'B':
		if( !strcmp( "BATCH", cmd ) )
			executebatch = makeNum( data );
		break;
	case 'C':
		if( !strcmp( "CPAGE", cmd ) )
			Commands->CPage( s, data );
		else if( !strcmp( "CREATETRACKINGMENU", cmd ) )
			Skills->CreateTrackingMenu( s, makeNum( data ) );
		break;
	case 'G':
		if( !strcmp( "GMMENU", cmd ) )
			gmmenu( s, makeNum( data ) );
		else if( !strcmp( "GMPAGE", cmd ) )
			Commands->GMPage( s, data );
		else if( !strcmp( "GCOLLECT", cmd ) )
			gcollect();
		else if( !strcmp( "GOPLACE", cmd ) )
		{
			tmp = makeNum( data );
			Commands->MakePlace( s, tmp );
			if( s->AddX() != 0 )
			{
				mChar->SetLocation( (UI16)s->AddX(), (UI16)s->AddY(), s->AddZ() );
				mChar->Teleport();
			}
		}
		else if( !strcmp( "GUIINFORMATION", cmd ) )
		{
			GumpDisplay guiInfo( s, 400, 300 );
			guiInfo.SetTitle( "Server status" );

			total = (uiCurrentTime - starttime ) / CLOCKS_PER_SEC;
			ho = total / 3600;
			total -= ho * 3600;
			mi = total / 60;
			total -= mi * 60;
			se = total;
			total = 0;
			char hoStr[4];
			char miStr[4];
			char seStr[4];
			if( ho < 10 )
				sprintf( hoStr, "0%i", ho );
			else
				sprintf( hoStr, "%i", ho );
			if( mi < 10 )
				sprintf( miStr, "0%i", mi );
			else
				sprintf( miStr, "%i", mi );
			if( se < 10 )
				sprintf( seStr, "0%i", se );
			else
				sprintf( seStr, "%i", se );
			sprintf( tstring, "%s:%s:%s", hoStr, miStr, seStr );
			guiInfo.AddData( "Uptime", tstring );
			guiInfo.AddData( "Accounts", Accounts->size() );
			guiInfo.AddData( "Items", items.Count() );
			guiInfo.AddData( "Chars", chars.Count() );
			guiInfo.AddData( "Players in world", now );
			sprintf( idname, "%s v%s(%s) [%s] Compiled by %s ", CVC.GetProductName(), CVC.GetVersion(), CVC.GetBuild(), OS_STR, CVC.GetName() );
			guiInfo.AddData( idname, idname, 7 );
			guiInfo.Send( 0, false, INVALIDSERIAL );
		}
		break;
	case 'I':
		if( !strcmp( "ITEMMENU", cmd ) )
			NewAddMenu( s, makeNum( data ) );
		else if( !strcmp( "INFORMATION", cmd ) )
		{
			total = (uiCurrentTime - starttime ) / CLOCKS_PER_SEC;
			ho = total / 3600;
			total -= ho * 3600;
			mi = total / 60;
			total -= mi * 60;
			se = total;
			total = 0;
			sysmessage( s, 1211, ho, mi, se, now, Accounts->size(), items.Count(), chars.Count() );
		}
		break;
	case 'M':
		if( !strcmp( "MAKEMENU", cmd ) )
			Skills->NewMakeMenu( s, makeNum( data ), (UI08)mChar->GetMaking() );
		break;
	case 'N':
		if( !strcmp( "NPC", cmd ) )
		{
			s->XText( data );
			sprintf( tstring, Dictionary->GetEntry( 1212 ), s->XText() );
			target( s, 0, 1, 0, 27, tstring );
		}
		break;
	case 'P':
		if( !strcmp( "POLY", cmd ) )
		{
			UI16 newBody = (UI16)makeNum( data );
			mChar->SetID( newBody );
			mChar->SetxID( newBody );
			mChar->SetOrgID( newBody );
			mChar->Teleport();	// why this is needed I'm not sure
		}
		break;
	case 'S':
		if( !strcmp( "SYSMESSAGE", cmd ) )
			sysmessage( s, data );
		else if( !strcmp( "SKIN", cmd ) )
		{
			COLOUR newSkin = (COLOUR)makeNum( data );
			mChar->SetSkin( newSkin );
			mChar->SetxSkin( newSkin );
			mChar->Teleport();
		}
		break;
	case 'T':
		if( !strcmp( "TRACKINGMENU", cmd ) )
			Skills->TrackingMenu( s, makeNum( data ) );
		break;
	case 'V':
		if( !strcmp( "VERSION", cmd ) )
		{
			sprintf( idname, "%s v%s(%s) [%s] Compiled by %s ", CVC.GetProductName(), CVC.GetVersion(), CVC.GetBuild(), OS_STR, CVC.GetName() );
			sysmessage( s, idname );
		}
		break;
	case 'W':
		if( !strcmp( "WEBLINK", cmd ) )
			weblaunch( s, data );
		break;
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void batchcheck( cSocket *s )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check if we run a batch file
//o---------------------------------------------------------------------------o
void batchcheck( cSocket *s )
{
	char temp[1024];
	sprintf( temp, "BATCH %i", executebatch );
	ScriptSection *Batch = FileLookup->FindEntry( temp, menus_def );
	if( Batch == NULL )
		return;
	const char *data = NULL;
	const char *tag = NULL;
	for( tag = Batch->First(); !Batch->AtEnd(); tag = Batch->Next() )
	{
		data = Batch->GrabData();
		scriptcommand( s, tag, data );
	}
	executebatch = 0;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void callguards( CChar *mChar )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Used when a character calls "Guards" Will look for a criminal
//|					first checking for anyone attacking him. If no one is attacking
//|					him it will look for any people nearby who are criminal or
//|					murderers
//o---------------------------------------------------------------------------o
void callGuards( CChar *mChar )
{
	if( mChar == NULL )
		return;

	if( !region[mChar->GetRegion()]->IsGuarded() || !cwmWorldState->ServerData()->GetGuardsStatus() )
		return;

	int xOffset = MapRegion->GetGridX( mChar->GetX() );
	int yOffset = MapRegion->GetGridY( mChar->GetY() );

	SubRegion *toCheck = MapRegion->GetGrid( xOffset, yOffset, mChar->WorldNumber() );
	if( toCheck == NULL )
		return;

	CHARACTER attacker = mChar->GetAttacker();
	if( attacker != INVALIDSERIAL )
	{
		CChar *aChar = &chars[attacker];
		if( aChar != NULL )
		{
			if( !aChar->IsDead() && ( aChar->IsCriminal() || aChar->IsMurderer() ) )
			{
				if( charInRange( mChar, aChar ) )
				{
					callGuards( mChar, aChar );
					return;
				}
			}
		}
	}

	for( CChar *tempChar = toCheck->FirstChar(); !toCheck->FinishedChars(); tempChar = toCheck->GetNextChar() )
	{
		if( tempChar == NULL )
			break;

		if( !tempChar->IsDead() && ( tempChar->IsCriminal() || tempChar->IsMurderer() ) )
		{
			if( charInRange( tempChar, mChar ) )
			{
				callGuards( mChar, tempChar );
				return;
			}
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void callguards( CChar *mChar, CChar *targChar )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Used when a character calls guards on another character, will
//|					ensure that character is not dead and is either a criminal or
//|					murderer, and that he is in visual range of the victim, will
//|					then spawn a guard to take care of the criminal.
//o---------------------------------------------------------------------------o
void callGuards( CChar *mChar, CChar *targChar )
{
	if( mChar == NULL || targChar == NULL ) 
		return;

	if( !region[mChar->GetRegion()]->IsGuarded() || !cwmWorldState->ServerData()->GetGuardsStatus() )
		return;

	if( !targChar->IsDead() && ( targChar->IsCriminal() || targChar->IsMurderer() ) )
	{
		if( charInRange( mChar, targChar ) )
			Combat->SpawnGuard( mChar, targChar, targChar->GetX(), targChar->GetY(), targChar->GetZ() );
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void DisplaySettings( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	UOX startup stuff
//| Moved that here because we need it in processkey now
//|									
//|	Modification	-	10/21/2002	-	EviLDeD - Xuri found the bug in one spot, just
//|									happened upon this quick fix. for BackUp operation.
//o---------------------------------------------------------------------------o
void DisplaySettings( void )
{
	std::map< bool, std::string > activeMap;
	activeMap[true] = "Activated!";
	activeMap[false] = "Disabled!";

	// Server.scp status --- By Magius(CHE)
	Console << "Server Settings:" << myendl;
	
	Console << "   -Archiving[";
	if(cwmWorldState->ServerData()->GetServerBackupStatus() )
		Console << "Enabled]. (" << cwmWorldState->ServerData()->GetBackupDirectory() << ")" << myendl;
	else 
		Console << "Disabled]" << myendl;
	
	Console << "   -Weapons & Armour Rank System: ";
	Console << activeMap[cwmWorldState->ServerData()->GetRankSystemStatus()].c_str() << myendl;
	
	Console << "   -Vendors buy by item name: ";
	Console << activeMap[cwmWorldState->ServerData()->GetSellByNameStatus()].c_str() << myendl;
	
	Console << "   -Adv. Trade System: ";
	Console << activeMap[cwmWorldState->ServerData()->GetTradeSystemStatus()].c_str() << myendl;
	
	Console << "   -Special Bank stuff: ";
	Console << activeMap[cwmWorldState->ServerData()->GetWildernessBankStatus()].c_str() << myendl;
	
	Console << "   -Crash Protection: ";
	if( cwmWorldState->ServerData()->GetServerCrashProtectionStatus() < 1 ) 
		Console << "Disabled!" << myendl;
#ifndef _CRASH_PROTECT_
	else 
		Console << "Unavailable in this version" << myendl;
#else
	else if( cwmWorldState->ServerData()->GetServerCrashProtectionStatus() == 1 ) 
		Console << "Save on crash" << myendl;
	else 
		Console << "Save & Restart Server" << myendl;
#endif

	Console << "   -xGM Remote: ";
	Console << activeMap[xgm != 0].c_str() << myendl;

	Console << "   -Races: " << Races->Count() << myendl;
	Console << "   -Guilds: " << GuildSys->NumGuilds() << myendl;
	Console << "   -Char count: " << cmem << myendl;
	Console << "   -Item count: " << imem << myendl;
	Console << "   -Num Accounts: " << Accounts->size() << myendl;
	Console << "   Directories: " << myendl;
	Console << "   -Shared:          " << cwmWorldState->ServerData()->GetSharedDirectory() << myendl;
	Console << "   -Archive:         " << cwmWorldState->ServerData()->GetBackupDirectory() << myendl;
	Console << "   -Data:            " << cwmWorldState->ServerData()->GetDataDirectory() << myendl;
	Console << "   -Defs:            " << cwmWorldState->ServerData()->GetDefsDirectory() << myendl;
	Console << "   -Scripts:         " << cwmWorldState->ServerData()->GetScriptsDirectory() << myendl;
	Console << "   -HTML:            " << cwmWorldState->ServerData()->GetHTMLDirectory() << myendl;
	Console << "   -Books:           " << cwmWorldState->ServerData()->GetBooksDirectory() << myendl;
	Console << "   -MessageBoards:   " << cwmWorldState->ServerData()->GetMsgBoardDirectory() << myendl;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void processkey( int c )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Handle keypresses in console
//o---------------------------------------------------------------------------o
void processkey( int c )
{
	char outputline[128], temp[1024];
	bool kill = false;
	int indexcount = 0;
	int i, j;
	int keyresp;

	if( c == '*' )
	{
		if( secure )
			messageLoop << "Secure mode disabled. Press ? for a commands list";
		else
			messageLoop << "Secure mode re-enabled";
		secure = !secure;
		return;
	} 
	else 
	{
		if( secure )
		{
			messageLoop << "Secure mode prevents keyboard commands! Press '*' to disable";
			return;
		}
		
		switch( c )
		{
			case '@':
				// Force server to save all files.(Manual save)
				messageLoop << MSG_WORLDSAVE;
				break;
		case 'Y':
#pragma note("Console Broadcast needs to not require cout anymore.  Is there a better way?")
			//messageLoop << "Console> ";
			std::cout << "System: ";
			while( !kill )
			{
				keyresp = cl_getch();
				switch( keyresp )
				{
				case -1:	// no key pressed
				case 0:
					break;
				case 0x1B:
					memset( outputline, 0x00, sizeof( outputline ) );
					indexcount = 0;
					kill = true;
					std::cout << std::endl;
					messageLoop << "| CMD: System broadcast canceled.";
					break;
				case 0x08:
					indexcount--;
					if( indexcount < 0 )	
						indexcount = 0;
					else
						std::cout << "\b \b";
					break;
				case 0x0A:
				case 0x0D:
					outputline[indexcount] = 0;
					messageLoop.NewMessage( MSG_CONSOLEBCAST, outputline );
					indexcount = 0;
					kill = true;
					std::cout << std::endl;
					sprintf( temp, "| CMD: System broadcast sent message \"%s\"", outputline );
					memset( outputline, 0x00, sizeof( outputline ) );
					messageLoop << temp;
					break;
				default:
					if( indexcount < sizeof( outputline ) )
					{
						outputline[indexcount++] = (UI08)(keyresp);
						std::cout << (char)keyresp;
					}
					break;
				}
				keyresp = 0x00;
			}
			break;
			case 0x1B:
			case 'Q':
				messageLoop << MSG_SECTIONBEGIN;
				messageLoop << "| CMD: Immediate Shutdown initialized!";
				messageLoop << MSG_SHUTDOWN;
				break;
			case '1':
				// Reload server ini file
				messageLoop << "| CMD: Loading Server INI... ";
				cwmWorldState->ServerData()->load();
				messageLoop << MSG_PRINTDONE;
				break;
			case '2':
				// Reload accounts, and update Access.adm if new accounts available.
				messageLoop << "| CMD: Loading Accounts... ";
				Accounts->Load();
				messageLoop << MSG_PRINTDONE;
				break;
			case '3':
				// Reload Region Files
				messageLoop << "| CMD: Loading Regions... ";
				loadregions();
				messageLoop << MSG_PRINTDONE;
				break;
			case '4':
				// Reload the serve spawn regions
				messageLoop << "| CMD: Loading Spawn Regions... ";
				loadSpawnRegions();
				messageLoop << MSG_PRINTDONE;
				break;
			case '5':
				// Reload the current Spells 
				messageLoop << "| CMD: Loading spells... ";
				Magic->LoadScript();
				messageLoop << MSG_PRINTDONE;
				break;
			case '6':
				// Reload the server command list
				messageLoop << "| CMD: Loading commands... ";
				Commands->Load();
				messageLoop << MSG_PRINTDONE;
				break;
			case '7':
				// Reload the server defantion files
				messageLoop << "| CMD: Loading Server DFN... ";
				FileLookup->Reload();
				messageLoop << MSG_PRINTDONE;
				break;
			case '8':
				// messageLoop access is REQUIRED, as this function is executing in a different thread, so we need thread safety
				messageLoop << "| CMD| Loading JSE Scripts... ";
				messageLoop << MSG_RELOADJS;
				break;
			case '9':
				// Reload the HTML output templates
				messageLoop << "| CMD: Loading HTML Templates... ";
				HTMLTemplates->Load();
				messageLoop << MSG_PRINTDONE;
				break;
			case '0':
				// Reload all the files. If there are issues with these files change the order reloaded from here first.
				cwmWorldState->ServerData()->load();
				messageLoop << "| CMD: Loading All";
				messageLoop << "|      Server INI... ";
				// Reload accounts, and update Access.adm if new accounts available.
				messageLoop << "|      Loading Accounts... ";
				Accounts->Load();
				messageLoop << MSG_PRINTDONE;
				// Reload Region Files
				messageLoop << "|      Loading Regions... ";
				loadregions();
				messageLoop << MSG_PRINTDONE;
				// Reload the serve spawn regions
				messageLoop << "|      Loading Spawn Regions... ";
				loadSpawnRegions();
				messageLoop << MSG_PRINTDONE;
				// Reload the current Spells 
				messageLoop << "|      Loading spells... ";
				Magic->LoadScript();
				messageLoop << MSG_PRINTDONE;
				// Reload the server command list
				messageLoop << "|      Loading commands... ";
				Commands->Load();
				messageLoop << MSG_PRINTDONE;
				// messageLoop access is REQUIRED, as this function is executing in a different thread, so we need thread safety
				messageLoop << "|      Loading JSE Scripts... ";
				messageLoop << MSG_RELOADJS;
				// Reload the HTML output templates
				messageLoop << "|      Loading HTML Templates... ";
				HTMLTemplates->Load();
				messageLoop << MSG_PRINTDONE;
				break;
		case 'T':
			// Timed shut down(10 minutes)
			messageLoop << "| CMD: 10 Minute Server Shutdown Announced(Timed)";
			endtime = BuildTimeValue( 600 );
			endmessage(0);
			break;
		case 'L':
			// Show Layer info
			if( showlayer )
				messageLoop << "| CMD: Show Layer Disabled";
			else
				messageLoop << "| CMD: Show Layer Enabled";
			showlayer = !showlayer;
			break;
		case  'D':    
			// Disconnect account 0 (useful when client crashes)
			cSocket *tSock;
			for( tSock = Network->LastSocket(); tSock != NULL; tSock = Network->PrevSocket() )
			{
				if( tSock->AcctNo() == 0 )
					Network->Disconnect( calcSocketFromSockObj( tSock ) );
			}
			messageLoop << "| CMD: Socket Disconnected(Account 0).";
			break;
		case 'K':		
			// mass disconnect
			for( i = now - 1; i >= 0; i-- )
				Network->Disconnect( i );
			messageLoop << "| CMD: All Connections Closed.";
			break;
		case 'H':                
			// Enable/Disable heartbeat
			if( heartbeat == 1 ) 
				messageLoop << "| CMD: Heartbeat Disabled";
			else 
				messageLoop << "| CMD: Heartbeat Enabled";
			heartbeat = !heartbeat;
			break;
		case 'P':                
			// 1/13/2003 - Dreoth - Log Performance Information enhancements
			messageLoop << "CMD: Performance Dump ";
			Console.Log( "--- Starting Performance Dump ---", "performance.log");
			Console.Log( "Performace Dump:", "performance.log");
			Console.Log( "Network code: %.2fmsec [%i samples]", "performance.log", (R32)((R32)networkTime/(R32)networkTimeCount), networkTimeCount);
			Console.Log( "Timer code: %.2fmsec [%i samples]", "performance.log", (R32)((R32)timerTime/(R32)timerTimeCount), timerTimeCount);
			Console.Log( "Auto code: %.2fmsec [%i samples]", "performance.log", (R32)((R32)autoTime/(R32)autoTimeCount), autoTimeCount);
			Console.Log( "Loop Time: %.2fmsec [%i samples]", "performance.log", (R32)((R32)loopTime/(R32)loopTimeCount), loopTimeCount);
			Console.Log( "Characters: %i/%i - Items: %i/%i (Dynamic)", "performance.log", chars.Count(), cmem, items.Count(), imem);
			Console.Log( "Simulation Cycles: %f per sec", "performance.log", (1000.0*(1.0/(R32)((R32)loopTime/(R32)loopTimeCount))));
			Console.Log( "Bytes sent: %i", "performance.log", globalSent);
			Console.Log( "Bytes Received: %i", "performance.log", globalRecv);
			Console.Log( "--- Performance Dump Complete ---", "performance.log");
			messageLoop << "Performance Dump Complete ";
			// Echo output to console as well.
			printf("| Performance Dump:\n");
			printf("|   Network code: %.2fmsec [%i samples]\n", (R32)((R32)networkTime/(R32)networkTimeCount), networkTimeCount);
			printf("|   Timer code: %.2fmsec [%i samples]\n", (R32)((R32)timerTime/(R32)timerTimeCount), timerTimeCount);
			printf("|   Auto code: %.2fmsec [%i samples]\n", (R32)((R32)autoTime/(R32)autoTimeCount), autoTimeCount);
			printf("|   Loop Time: %.2fmsec [%i samples]\n", (R32)((R32)loopTime/(R32)loopTimeCount), loopTimeCount);
			printf("|   Characters: %i/%i - Items: %i/%i (Dynamic)\n", chars.Count(), cmem, items.Count(), imem);
			printf("|   Simulation Cycles: %f per sec\n", (1000.0*(1.0/(R32)((R32)loopTime/(R32)loopTimeCount))));
			printf("|   Bytes sent: %i\n", globalSent);
			printf("|   Bytes Received: %i\n", globalRecv);
			printf("| Performance Dump Complete.\n");
			break;
		case 'W':                
			// Display logged in chars
			messageLoop << "| CMD: Current Users in the World:";
			j = 0;
			cSocket *iSock;
			Network->PushConn();
			for( iSock = Network->FirstSocket(); !Network->FinishedSockets(); iSock = Network->NextSocket() )
			{
				j++;
				CChar *mChar = iSock->CurrcharObj();
				sprintf( temp, "|      %i) %s [%i %i %i %i]", j - 1, mChar->GetName(), mChar->GetSerial( 1 ), mChar->GetSerial( 2 ), mChar->GetSerial( 3 ), mChar->GetSerial( 4 ) );
				messageLoop << temp;
			}
			Network->PopConn();
			sprintf( temp, "|      Total users online: %i", j );
			messageLoop << temp;
			break;
		case 'M':
			UI32 tmp, total;
			total = 0;
			tmp = 0;
			messageLoop << "| CMD: UOX Memory Information:";
			messageLoop << "|      Cache:";
			sprintf( temp, "|         Tiles: %i bytes", Map->TileMem );
			messageLoop << temp;
			sprintf( temp, "|         Statics: %i bytes", Map->StaMem );
			messageLoop << temp;
			sprintf( temp, "|         Version: %i bytes", Map->versionMemory );
			messageLoop << temp;
			sprintf( temp, "|         Map0: %i bytes [%i Hits - %i Misses]", 9*MAP0CACHE, Map->Map0CacheHit, Map->Map0CacheMiss );
			messageLoop << temp;
			total += tmp = chars.Size() + cmem*sizeof( teffect_st ) + cmem*sizeof(char) + cmem*sizeof(int)*5;
			sprintf( temp, "|      Characters: %i bytes [%i chars ( %i allocated )]", tmp, chars.Count(), cmem );
			messageLoop << temp;
			total += tmp = items.Size() + imem*sizeof(int)*4;
			sprintf( temp, "|      Items: %i bytes [%i items ( %i allocated )]", tmp, items.Count(), imem );
			messageLoop << temp;
			sprintf( temp, "|         You save I: %i & C: %i bytes!", imem * sizeof(CItem) - items.Size(), cmem * sizeof( CChar ) - chars.Size() );
			total += tmp = 69 * sizeof( SpellInfo );
			sprintf( temp, "|      Spells: %i bytes", tmp );
			messageLoop << "|      Sizes:";
			sprintf( temp, "|         CItem  : %i bytes", sizeof( CItem ) );
			messageLoop << temp;
			sprintf( temp, "|         CChar  : %i bytes", sizeof( CChar ) );
			messageLoop << temp;
			sprintf( temp, "|         TEffect: %i bytes (%i total)", sizeof( teffect_st ), sizeof( teffect_st ) * Effects->Count() );
			messageLoop << temp;
			total += tmp = Map->TileMem + Map->StaMem + Map->versionMemory;
			sprintf( temp, "|         Approximate Total: %i bytes", total );
			messageLoop << temp;
			break;
		case 'e':
		case 'E':
			// Toggle Layer errors
			j = 0;
			for( i = 0; i < MAXLAYERS; i++ )
			{
				if( erroredLayers[i] != 0 )
				{
					j ++;
					if( i < 10 )
						sprintf( temp, "| ERROR: Layer 0%i -> %i errors", i, erroredLayers[i] );
					else
						sprintf( temp, "| ERROR: Layer %i -> %i errors", i, erroredLayers[i] );
					messageLoop << temp;
				}
			}
			sprintf( temp, "| ERROR: Found errors on %i layers.", j );
			messageLoop << temp;
			break;
		case '?':
			messageLoop << "o-------------------------";
			messageLoop << "| Console commands:";
			messageLoop << "o-------------------------";
			messageLoop << "|  ShardOP:";
			messageLoop << "|     * - Lock/Unlock Console ? - Commands list(this)";
			messageLoop << "|     C - Configuration       H - Heart Beat";
			messageLoop << "|     Y - Console Broadcast   Q - Quit/Exit           ";
			messageLoop << "|  Load Commands:";
			messageLoop << "|     1 - Ini                 2 - Accounts";
			messageLoop << "|     3 - Regions             4 - Spawn Regions";
			messageLoop << "|     5 - Spells              6 - Commands";
			messageLoop << "|     7 - Dfn's               8 - JavaScript";
			messageLoop << "|     9 - HTML Templates      0 - ALL(1-9)";
			messageLoop << "|  Save Commands:";
			messageLoop << "|     ! - Accounts            @ - World";
			messageLoop << "|     # - Unused              $ - Unused";
			messageLoop << "|     % - Unused              ^ - Unused";
			messageLoop << "|     & - Unused              ( - Unused";
			messageLoop << "|     ) - Unused";
			messageLoop << "|  Server Maintenence:";
			messageLoop << "|     P - Performance         W - Characters Online";
			messageLoop << "|     M - Memory Information  T - 10 Minute Shutdown";
			messageLoop << "|     V - Dump Lookups(Devs)  N - Dump Npc.Dat";
			messageLoop << "|     L - Layer Information"; 
			messageLoop << "|  Network Maintenence:";
			messageLoop << "|     D - Disconnect Acct0    K - Disconnect All";
			messageLoop << "|     Z - Socket Logging      ";
			messageLoop << "o-------------------------";
			break;
		case 'v':
		case 'V':
			// Dump look up data to files so developers working with extending the ini will have a table to use
			messageLoop << "| CMD: Creating Server.scp and Uox3.ini Tag Lookup files(For Developers)....";
			cwmWorldState->ServerData()->DumpLookup( 0 );
			cwmWorldState->ServerData()->DumpLookup( 1 );
			cwmWorldState->ServerData()->save( "./uox.tst.ini" );
			messageLoop << MSG_PRINTDONE;
			break;
		case 'z':
		case 'Z':
			// Log socket activity
			cSocket *snSock;
			Network->PushConn();
			for( snSock = Network->FirstSocket(); !Network->FinishedSockets(); snSock = Network->NextSocket() )
			{
				if( snSock != NULL )
				{
					if(	snSock->Logging() )
						snSock->Logging( false );
					else
						snSock->Logging( true );
				}
			}
			Network->PopConn();
			if( snSock->Logging() )
				messageLoop << "| CMD: Network Logging Enabled.";
			else
				messageLoop << "| CMD: Network Logging Disabled.";
			break;
		case 'N':
			//. Dump a file that contains the id and sound some toher misc data about monster NPC.
			DumpCreatures();
			break;
		case 'c':
		case 'C':
			// Shows a configuration header
			DisplaySettings();
			break;
		default:
			sprintf( temp, "Key \'%c\' [%i] does not perform a function", (char)c, c );
			messageLoop << temp;
			break;
		}
	}
}

//o----------------------------------------------------------------------------o
//|   Function -	 void checkkey( void )
//|   Date     -	 Unknown
//|   Programmer  -  Unknown  (Touched up by EviLDeD)
//o----------------------------------------------------------------------------o
//|   Purpose     -  Facilitate console control. SysOp keys, and localhost 
//|					 controls.
//o----------------------------------------------------------------------------o
void checkkey( void )
{
	int c = cl_getch();
	if( c > 0 )
	{
		if( (cluox_io) && ( c == 250 ) )
		{  // knox force unsecure mode, need this since cluox can't know
			//      how the toggle status is.
			if( secure )
			{
				Console << "Secure mode disabled. Press ? for a commands list." << myendl;
				secure = false;
				return;
			}
		}
		c = toupper(c);
		processkey( c );
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void checktimers( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check shutdown timers
//o---------------------------------------------------------------------------o
void checktimers( void )
{
	overflow = ( lclock > uiCurrentTime );
	if( endtime )
	{
		if( endtime <= uiCurrentTime ) 
			keeprun = false;
	}
	lclock = uiCurrentTime;
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool genericCheck( CChar *i )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check characters status.  Returns true if character was killed
//o---------------------------------------------------------------------------o
bool genericCheck( CChar *i )
{
	if( i->IsDead() )
		return false;

	SI32 c;
	bool strUpdate = false;
	bool intUpdate = false;
	bool dexUpdate = false;

	bool isOn = isOnline( i ), isNPC = i->IsNpc();
	
	if( i->GetHP() > i->GetMaxHP() )
	{
		i->SetHP( i->GetMaxHP() );
		strUpdate = true;
	}
	if( i->GetStamina() > i->GetMaxStam() )
	{
		i->SetStamina( i->GetMaxStam() );
		dexUpdate = true;
	}
	if( i->GetMana() > i->GetMaxMana() )
	{
		i->SetMana( i->GetMaxMana() );
		intUpdate = true;
	}

	if( i->GetRegen( 0 ) <= uiCurrentTime || overflow )
	{
		if( i->GetHP() < i->GetMaxHP() && ( i->GetHunger() > (SI16)cwmWorldState->ServerData()->GetHungerThreshold() || cwmWorldState->ServerData()->GetHungerRate() == 0 ) )
		{
			for( c = 0; c < i->GetMaxHP() + 1; c++ )
			{
				if( i->GetHP() <= i->GetMaxHP() && ( i->GetRegen( 0 ) + ( c * cwmWorldState->ServerData()->GetSystemTimerStatus( HITPOINT_REGEN ) * CLOCKS_PER_SEC) ) <= uiCurrentTime )
				{
					if( i->GetSkill( HEALING ) < 500 ) 
						i->IncHP( 1 );
					else if( i->GetSkill( HEALING ) < 800 ) 
						i->IncHP( 2 );
					else 
						i->IncHP( 3 );
					if( i->GetHP() > i->GetMaxHP() ) 
					{
						i->SetHP( i->GetMaxHP() ); 
						break;
					}
					strUpdate = true;
				}
				else			// either we're all healed up, or all time periods have passed
					break;
			}
		}
		i->SetRegen( BuildTimeValue( (R32)cwmWorldState->ServerData()->GetSystemTimerStatus( HITPOINT_REGEN ) ), 0 );
	}
	if( i->GetRegen( 1 ) <= uiCurrentTime || overflow )
	{
		if( i->GetStamina() < i->GetMaxStam() )
		{
			for( c = 0; c < i->GetMaxStam() + 1; c++ )
			{
				if( ( i->GetRegen( 1 ) + ( c * cwmWorldState->ServerData()->GetSystemTimerStatus( STAMINA_REGEN ) * CLOCKS_PER_SEC) ) <= uiCurrentTime && i->GetStamina() <= i->GetMaxStam() )
				{
					i->SetStamina( (SI16)( i->GetStamina() + 1 ) );
					if( i->GetStamina() > i->GetMaxStam() ) 
					{
						i->SetStamina( i->GetMaxStam() );
						break;
					}
					dexUpdate = true;
				}
				else
					break;
			}
		}
		i->SetRegen( BuildTimeValue( (R32)cwmWorldState->ServerData()->GetSystemTimerStatus( STAMINA_REGEN ) ), 1 );
	}

	// CUSTOM START - SPUD:MANA REGENERATION:Rewrite of passive and active meditation code
	if( i->GetRegen( 2 ) <= uiCurrentTime || overflow )
	{
		if( i->GetMana() < i->GetMaxMana() )
		{
			for( c = 0; c < i->GetMaxMana() + 1; c++ )
			{
				if( i->GetRegen( 2 ) + ( c * cwmWorldState->ServerData()->GetSystemTimerStatus( MANA_REGEN ) * CLOCKS_PER_SEC) <= uiCurrentTime && i->GetMana() <= i->GetMaxMana() )
				{
					Skills->CheckSkill( i, MEDITATION, 0, 1000 );	// Check Meditation for skill gain ala OSI
					i->SetMana( (SI16)( i->GetMana() + 1 ) );	// Gain a mana point
					if( i->GetMana() > i->GetMaxMana() ) 
					{
						if( i->GetMed() ) // Morrolan = Meditation
						{
							sysmessage( calcSocketObjFromChar( i ), 969 );
							i->SetMed( 0 );
						}
						i->SetMana( i->GetMaxMana() ); 
						break;
					}
					intUpdate = true;
				}
			}
		}
		R32 MeditationBonus = ( .00075f * i->GetSkill( MEDITATION ) );	// Bonus for Meditation
		int NextManaRegen = static_cast<int>(cwmWorldState->ServerData()->GetSystemTimerStatus( MANA_REGEN ) * ( 1 - MeditationBonus ) * CLOCKS_PER_SEC);
		if( cwmWorldState->ServerData()->GetServerArmorAffectManaRegen() )	// If armor effects mana regeneration...
		{
			R32 ArmorPenalty = Combat->calcDef( i, 0, false );	// Penalty taken due to high def
			if( ArmorPenalty > 100 )	// For def higher then 100, penalty is the same...just in case
				ArmorPenalty = 100;
			ArmorPenalty = 1 + (ArmorPenalty / 25);
			NextManaRegen = static_cast<int>(NextManaRegen * ArmorPenalty);
		}
		if( i->GetMed() )	// If player is meditation...
			i->SetRegen( ( uiCurrentTime + ( NextManaRegen / 2 ) ), 2 );
		else
			i->SetRegen( ( uiCurrentTime + NextManaRegen ), 2 );
	}
	// CUSTOM END
	if( i->GetHidden() == 2 && ( i->GetInvisTimeout() <= uiCurrentTime || overflow ) && !i->IsPermHidden() )
		i->ExposeToView();
	cSocket *s = NULL;
	if( !isNPC )
		s = calcSocketObjFromChar( i );
	if( isOn || isNPC )
	{
		if( cwmWorldState->ServerData()->GetHungerRate() > 1 && ( i->GetHungerTime() <= uiCurrentTime || overflow ) )
		{
			if( i->GetHunger() > 0 && !i->IsCounselor() && !i->IsGM() )
				i->DecHunger(); //Morrolan GMs and Counselors don't get hungry
			
			UI16 HungerTrig = i->GetScriptTrigger();
			cScript *toExecute = Trigger->GetScript( HungerTrig );
			bool doHunger = true;
			if( toExecute != NULL )
				doHunger = !toExecute->OnHungerChange( i, i->GetHunger() );
			if( doHunger )
			{
				switch( i->GetHunger() )
				{
				case 6: break;
				case 5:	sysmessage( s, 1222 );	break;
				case 4:	sysmessage( s, 1223 );	break;
				case 3:	sysmessage( s, 1224 );	break;
				case 2:	sysmessage( s, 1225 );	break;
				case 1:	sysmessage( s, 1226 );	break;
				case 0:
					if( !i->IsGM() && !i->IsCounselor() ) 
						sysmessage( s, 1227 );
					break;	
				}
			}
			i->SetHungerTime( BuildTimeValue( (R32)cwmWorldState->ServerData()->GetHungerRate() ) );
		}
		if( ( hungerdamagetimer <= uiCurrentTime || overflow ) && cwmWorldState->ServerData()->GetHungerDamage() > 0 ) // Damage them if they are very hungry
		{
			hungerdamagetimer = BuildTimeValue( (R32)cwmWorldState->ServerData()->GetHungerDamageRateTimer() ); // set new hungertime
			if( i->GetHP() > 0 && i->GetHunger() < 2 && !i->IsCounselor() && !i->IsDead() )
			{     
				sysmessage( s, 1228 );
				i->IncHP( (SI16)( -cwmWorldState->ServerData()->GetHungerDamage() ) );
				updateStats( i, 0 );
				if( i->GetHP() <= 0 )
				{ 
					sysmessage( s, 1229 );
					doDeathStuff( i );
				}
			}
		}
	}
	if( !i->IsInvulnerable() && i->GetPoisoned() && ( isNPC || isOn ) )
	{
		if( i->GetPoisonTime() <= uiCurrentTime || overflow )
		{
			if( i->GetPoisonWearOffTime() > uiCurrentTime )
			{
				char t[128];
				SI16 pcalc = 0;
				switch( i->GetPoisoned() )
				{
				case 1:
					i->SetPoisonTime( BuildTimeValue( 5 ) );
					if( i->GetPoisonTextTime() <= uiCurrentTime || overflow )
					{
						i->SetPoisonTextTime( BuildTimeValue( 10 ) );
						sprintf( t, Dictionary->GetEntry( 1240 ), i->GetName() );
						npcEmoteAll( i, t, true );
					}
					i->IncHP( (SI16)( -RandomNum( 1, 2 ) ) );
					strUpdate = true;
					break;
				case 2:
					i->SetPoisonTime( BuildTimeValue( 4 ) );
					if( i->GetPoisonTextTime() <= uiCurrentTime || overflow )
					{
						i->SetPoisonTextTime( BuildTimeValue( 10 ) );
						sprintf( t, Dictionary->GetEntry( 1241 ), i->GetName() );
						npcEmoteAll( i, t, true );
					}
					pcalc = (SI16)( ( i->GetHP() * RandomNum( 2, 5 ) / 100 ) + RandomNum( 0, 2 ) ); // damage: 1..2..5% of hp's+ 1..2 constant
					i->IncHP( (SI16)( -pcalc ) );
					strUpdate = true;
					break;
				case 3:
					i->SetPoisonTime( BuildTimeValue( 3 ) );
					if( i->GetPoisonTextTime() <= uiCurrentTime || overflow )
					{
						i->SetPoisonTextTime( BuildTimeValue( 10 ) );
						sprintf( t, Dictionary->GetEntry( 1242 ), i->GetName() );
						npcEmoteAll( i, t, true );
					}
					pcalc = (SI16)( ( i->GetHP() * RandomNum( 5, 10 ) / 100 ) + RandomNum( 1, 3 ) ); // damage: 5..10% of hp's+ 1..2 constant
					i->IncHP( (SI16)( -pcalc ) );
					strUpdate = true;
					break;
				case 4:
					i->SetPoisonTime( BuildTimeValue( 3 ) );
					if( i->GetPoisonTextTime() <= uiCurrentTime || overflow ) 
					{
						i->SetPoisonTextTime( BuildTimeValue( 10 ) );
						sprintf( t, Dictionary->GetEntry( 1243 ), i->GetName() );
						npcEmoteAll( i, t, true );
					}
					pcalc = (SI16)( i->GetHP() / 5 + RandomNum( 3, 6 ) ); // damage: 20% of hp's+ 3..6 constant, quite deadly <g>
					i->IncHP( (SI16)( -pcalc ) );
					strUpdate = true;
					break;
				default:
					Console.Error( 2, " Fallout of switch statement without default. uox3.cpp, genericCheck()" );
					i->SetPoisoned( 0 );
					break;
				}
				if( i->GetHP() < 1 )
				{
					doDeathStuff( i );
					sysmessage( s, 1244 );
				} 
			}
		}
	}

	if( i->GetPoisonWearOffTime() <= uiCurrentTime )
	{
		if( i->GetPoisoned() )
		{
            i->SetPoisoned( 0 );
			i->SendToSocket( s, true, i );
            sysmessage( s, 1245 );
		}
	}

	bool part1 = doLightEffect( i );
	bool part2 = doRainEffect( i );
	bool part3 = doSnowEffect( i );
	bool part4 = doHeatEffect( i );
	bool part5 = doColdEffect( i );

	if( strUpdate || part1 || part2 || part3 || strUpdate || part4 || part5 )
		updateStats( i, 0 );

	if( dexUpdate || part4 )
		updateStats( i, 2 );

	if( intUpdate )
		updateStats( i, 1 );
	
	if( i->GetHP() <= 0 && !i->IsDead() )
	{
		doDeathStuff( i );
		return true;
	}
	return false;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void checkPC( CChar *i, bool doWeather )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check a PC's status
//o---------------------------------------------------------------------------o
void checkPC( CChar *i, bool doWeather )
{
	int timer;
	
	LIGHTLEVEL toShow;
	cSocket *mSock = calcSocketObjFromChar( i );
	
	Magic->CheckFieldEffects2( i, 1 );
	
	bool isOn = isOnline( i );

	if( !i->IsDead() )
	{
		if( i->GetSwingTarg() == INVALIDSERIAL )
			Combat->DoCombat( i );
		else if( Combat->TimerOk( i ) )
			Combat->CombatHit( i, &chars[i->GetSwingTarg()] );
	}	
	
	if( doWeather )
	{
		SI16 curLevel = cwmWorldState->ServerData()->GetWorldLightCurrentLevel();
		if( Races->VisLevel( i->GetRace() ) > curLevel )
			toShow = 0;
		else
			toShow = (UI08)( curLevel - Races->VisLevel( i->GetRace() ) );

		doLight( mSock, toShow );
		Weather->DoPlayerStuff( i );
	}
	
	if( i->GetSmokeTimer() > uiCurrentTime )
	{
		if( i->GetSmokeDisplayTimer() <= uiCurrentTime )
		{
			i->SetSmokeDisplayTimer( BuildTimeValue( 5 ) );
			staticeffect( i, 0x3735, 0, 30 );
			soundeffect( i, 0x002B );
			npcEmote( mSock, i, 1230 + RandomNum( 0, 6 ), true );
		}
	}
	
	if( isOn && i->GetSquelched() == 2 )
	{
		if( i->GetMuteTime() !=-1)
		{
			if( (UI32)i->GetMuteTime() <= uiCurrentTime || overflow )
			{
				i->SetSquelched( 0 );
				i->SetMuteTime( -1 );
				sysmessage( mSock, 1237 );
			}
		}
	}
	
	if( isOn )
	{
		if( (i->GetCrimFlag() > 0 ) && ( (UI32)i->GetCrimFlag() <= uiCurrentTime || overflow ) && i->IsCriminal() )
		{
			sysmessage( mSock, 1238 );
			i->SetCrimFlag( -1 );
			setcharflag( i );
		}
		i->SetMurderRate( i->GetMurderRate() - 1 );
		
		if( (UI32)i->GetMurderRate() <= uiCurrentTime )
		{
			if( i->GetKills() > 0 )
				i->SetKills( (SI16)( i->GetKills() - 1 ) );
			if( i->GetKills() == cwmWorldState->ServerData()->GetRepMaxKills() ) 
				sysmessage( mSock, 1239 );
			i->SetMurderRate( BuildTimeValue( (R32)cwmWorldState->ServerData()->GetRepMurderDecay() ) );//Murder rate (in mins) to seconds. (checkauto is done about 25 times per second)
		}
		setcharflag( i );
	}
	
	if( i->GetCasting() == 1 )	// Casting a spell
	{
		i->SetNextAct( i->GetNextAct() - 1 );
		if( (UI32)i->GetSpellTime() <= uiCurrentTime || overflow )//Spell is complete target it.
		{
			if( spells[i->GetSpellCast()].RequireTarget() )
				target( mSock, 0, 1, 0, 100, spells[i->GetSpellCast()].StringToSay() );
			else
				Magic->CastSpell( mSock, i );
			i->SetCasting( -1 );
			i->SetSpellTime( 0 );
			i->SetFrozen( false );
		} 
		else if( i->GetNextAct() <= 0 )//redo the spell action
		{
			i->SetNextAct( 75 );
			if( !i->IsOnHorse() )
				impaction( mSock, i->GetSpellAction() );
		}
	}
	
	if( cwmWorldState->ServerData()->GetWorldAmbientSounds() >= 1 )
	{
		if( cwmWorldState->ServerData()->GetWorldAmbientSounds() > 10 ) 
			cwmWorldState->ServerData()->SetWorldAmbientSounds( 10 );
		timer = cwmWorldState->ServerData()->GetWorldAmbientSounds() * 100;
		if( isOn && !i->IsDead() && ( rand()%( timer ) ) == ( timer / 2 ) ) 
			bgsound( i ); // bgsound uses array positions not sockets!
	}
	
	if( i->GetSpiritSpeakTimer() > 0 && i->GetSpiritSpeakTimer() < uiCurrentTime )
		i->SetSpiritSpeakTimer( 0 );
	
	if( i->GetTrackingTimer() > uiCurrentTime && isOn )
	{
		if( i->GetTrackingDisplayTimer() <= uiCurrentTime )
		{
			i->SetTrackingDisplayTimer( BuildTimeValue( (R32)cwmWorldState->ServerData()->GetTrackingRedisplayTime() ) );
			Skills->Track( i );
		}
	}
	else
	{
		if( i->GetTrackingTimer() > ( uiCurrentTime / 10 ) ) // dont send arrow-away packet all the time
		{
			i->SetTrackingTimer( 0 );
			CPTrackingArrow tSend = chars[i->GetTrackingTarget()];
			tSend.Active( 0 );
			mSock->Send( &tSend );
		}
	}
	
	if( i->GetFishingTimer() )
	{
		if( i->GetFishingTimer() <= uiCurrentTime )
		{
			Skills->Fish( i );
			i->SetFishingTimer( 0 );
		}
	}
	
	if( i->IsOnHorse() )
	{
		CItem *horseItem = FindItemOnLayer( i, 0x19 );
		if( horseItem == NULL )
			i->SetOnHorse( false );	// turn it off, we aren't on one because there's no item!
		else if( horseItem->GetDecayTime() != 0 && ( horseItem->GetDecayTime() <= uiCurrentTime || overflow ) )
		{
			i->SetOnHorse( false );
			Items->DeleItem( horseItem );
		}
	}
	
}

//o---------------------------------------------------------------------------o
//|	Function	-	void checkNPC( CChar *i )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check NPC's status
//o---------------------------------------------------------------------------o
void checkNPC( CChar *i )
{
	if( i == NULL || i->isFree() )
		return;
	// okay, this will only ever trigger after we check an npc...  Question is:
	// should we remove the time delay on the AI check as well?  Just stick with AI/movement
	// AI can never be faster than how often we check npcs
	// This periodically generates access violations.  No idea why either
	UI16 AITrig = i->GetScriptTrigger();
	cScript *toExecute = Trigger->GetScript( AITrig );
	bool doAICheck = true;
	if( toExecute != NULL )
	{
		if( toExecute->OnAISliver( i ) )
			doAICheck = false;
	}
	if( doAICheck )
		Npcs->CheckAI( i );
	Movement->NpcMovement( i );
	setcharflag( i );		// possibly not... How many times do we want to set this? We've set it twice in the calling function
	
	restockNPC( i );

	if( i->GetOwnerObj() != NULL && i->GetHunger() == 0 && i->GetNPCAiType() != 17 ) // tamed animals but not player vendors ;)=
	{
		tempeffect( i, i, 44, 0, 0, 0 ); // (go wild in some minutes ...)-effect
		i->SetHunger( -1 );
	}
	
	if( i->GetSummonTimer() <= uiCurrentTime || overflow )
	{
		if( i->GetSummonTimer() > 0 )
		{
			// Dupois - Added Dec 20, 1999
			// QUEST expire check - after an Escort quest is created a timer is set
			// so that the NPC will be deleted and removed from the game if it hangs around
			// too long without every having its quest accepted by a player so we have to remove 
			// its posting from the messageboard before icing the NPC
			// Only need to remove the post if the NPC does not have a follow target set
			if( i->GetQuestType() == ESCORTQUEST && i->GetFTarg() == INVALIDSERIAL )
			{
				MsgBoardQuestEscortRemovePost( i );
				MsgBoardQuestEscortDelete( i );
				return;
			}
			// Dupois - End
			if( i->GetNPCAiType() == 4 && i->IsAtWar() )
			{
				i->SetSummonTimer( BuildTimeValue( 25 ) );
				return;
			}
			soundeffect( i, 0x01FE );
			i->SetDead( true );
			Npcs->DeleteChar( i );
			return;
		}
	}
	
	if( i->GetFleeAt() == 0 ) 
		i->SetFleeAt( NPC_BASE_FLEEAT );
	if( i->GetReattackAt() == 0 ) 
		i->SetReattackAt( NPC_BASE_REATTACKAT );
	
	if( i->GetNpcWander() != 5 && ( i->GetHP() < i->GetStrength() * i->GetFleeAt() / 100 ) )
	{
		i->SetOldNpcWander( i->GetNpcWander() );
		i->SetNpcWander( 5 );
		i->SetNpcMoveTime( BuildTimeValue( (R32)( cwmWorldState->ServerData()->GetNPCSpeed() / 2 ) ) );	// fleeing enemies are 2x faster
	}
	
	if( i->GetNpcWander() == 5 && (i->GetHP() > i->GetStrength() * i->GetReattackAt() / 100))
	{
		i->SetNpcWander( i->GetOldNpcWander() );
		i->SetNpcMoveTime( BuildTimeValue( (R32)cwmWorldState->ServerData()->GetNPCSpeed() ) );
		i->SetOldNpcWander( 0 ); // so it won't save this at the wsc file
	}

	Magic->CheckFieldEffects2( i, 0 );

	if( !i->IsDead() )
	{
		if( i->GetSwingTarg() == INVALIDSERIAL )
			Combat->DoCombat( i );
		else if( Combat->TimerOk( i ) )
			Combat->CombatHit( i, &chars[i->GetSwingTarg()] );
	}
}

void checkItem( SubRegion *toCheck, UI32 checkitemstime )
{
	for( CItem *itemCheck = toCheck->FirstItem(); !toCheck->FinishedItems(); itemCheck = toCheck->GetNextItem() )
	{
		if( itemCheck == NULL )
			break;
		if( checkitemstime <= uiCurrentTime || overflow )
		{
			Items->RespawnItem( itemCheck );
			if( itemCheck->GetType() == 51 || itemCheck->GetType() == 52 )
			{
				if( itemCheck->GetGateTime() <= uiCurrentTime )
				{
					Items->DeleItem( itemCheck );
					continue;
				}
			}
			if( Items->DecayItem( itemCheck ) )
				continue;

			if( itemCheck->GetType() == 88 && itemCheck->GetMoreY() < 25 )
			{
				Network->PushConn();
				for( cSocket *eSock = Network->FirstSocket(); !Network->FinishedSockets(); eSock = Network->NextSocket() )
				{
					if( getItemDist( eSock->CurrcharObj(), itemCheck ) <= itemCheck->GetMoreY() )
					{
						if( (UI32)RandomNum( 1, 100 ) <= itemCheck->GetMoreZ() )
							soundeffect( itemCheck, eSock, (UI16)itemCheck->GetMoreX() );
					}
				}
				Network->PopConn();
			}
		} 
		if( itemCheck->GetType() == 117 && ( itemCheck->GetType2() == 1 || itemCheck->GetType2() == 2 ) && 
			( itemCheck->GetGateTime() <= uiCurrentTime || overflow ) )
		{
			Network->PushConn();
			for( cSocket *bSock = Network->FirstSocket(); !Network->FinishedSockets(); bSock = Network->NextSocket() )
			{
				if( getItemDist( bSock->CurrcharObj(), itemCheck ) <= 18 )
				{
					if( itemCheck->GetType2() == 1 ) 
						Boats->MoveBoat( bSock, itemCheck->GetDir(), itemCheck );
					else 
					{
						UI08 dir = (UI08)( itemCheck->GetDir() + 4 );
						if( dir > 7 ) 
							dir -= 8;
						Boats->MoveBoat( bSock, dir, itemCheck );
					}
				}
			}
			Network->PopConn();
			itemCheck->SetGateTime( BuildTimeValue( (R32)cwmWorldState->ServerData()->GetCheckBoatSpeed() ) );
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void checkauto( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check automatic and timer controlled functions
//o---------------------------------------------------------------------------o
void checkauto( void )
{
	UI32 i; //\/ getclock only once
	static UI32 checkspawnregions=0; 
	static UI32 checknpcs=0;
	static UI32 checkitemstime=0;
	static UI32 uiSetFlagTime = 0;
	static UI32 accountFlush = 0;
	static UI32 regionCheck = 0;
	static UI32 counter = 0;
	bool doWeather = false;
	ACCOUNTSBLOCK actbTemp;
	MAPUSERNAMEID_ITERATOR I;
	//
	Trigger->PeriodicTriggerCheck();
	// modify this stuff to take into account more variables
	if( /*cwmWorldState->ServerData()->GetAccountFlushTimer() != 0 &&*/ accountFlush <= uiCurrentTime || overflow )
	{
		bool reallyOn = false;
		// time to flush our account status!
		//for( ourAccount = Accounts->FirstAccount(); !Accounts->FinishedAccounts(); ourAccount = Accounts->NextAccount() )
		for(I=Accounts->Begin();I!=Accounts->End();I++)
		{
			ACCOUNTSBLOCK actbBlock;
			actbBlock=I->second;
			if( actbBlock.wAccountIndex==AB_INVALID_ID)
				continue;

			if(actbTemp.wFlags&AB_FLAGS_ONLINE)
			{
				reallyOn = false;	// to start with, there's no one really on
				Network->PushConn();
				for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
				{
					CChar *tChar = tSock->CurrcharObj();
					if( tChar == NULL )
						continue;
					if( tChar->GetAccount().wAccountIndex == actbTemp.wAccountIndex)
						reallyOn = true;
				}
				Network->PopConn();
				if( !reallyOn )	// no one's really on, let's set that
					actbTemp.wFlags&=0xFFF7;
			}
		}
		accountFlush = BuildTimeValue( (R32)cwmWorldState->ServerData()->GetAccountFlushTimer() * 60 );
	}
	Network->On();

	if( worldSaveProgress == 0 )
	{
		for( SI32 ij = now - 1; ij >= 0; ij-- )
		{
			cSocket *tSock = calcSocketObjFromSock( ij );
			if( tSock->IdleTimeout() != -1 && (UI32)tSock->IdleTimeout() <= uiCurrentTime )
			{
			CChar *tChar = tSock->CurrcharObj();
			if( tChar == NULL )
				continue;
			if( !tChar->IsGM() )
			{
				tSock->IdleTimeout( -1 );
				sysmessage( tSock, 1246 );
				Network->Disconnect( ij );
			}
		}
			else if( ( ( (UI32)( tSock->IdleTimeout()+300*CLOCKS_PER_SEC ) <= uiCurrentTime && (UI32)( tSock->IdleTimeout()+200*CLOCKS_PER_SEC ) >= uiCurrentTime ) || overflow ) && !tSock->WasIdleWarned()  )
			{//is their idle time between 3 and 5 minutes, and they haven't been warned already?
				CPIdleWarning warn( 0x07 );
				tSock->Send( &warn );
				tSock->WasIdleWarned( true );
			}
		}
	}
	else if( worldSaveProgress == 2 )	// if we've JUST saved, do NOT kick anyone off (due to a possibly really long save), but reset any offending players to 60 seconds to go before being kicked off
	{
		Network->PushConn();
		for( cSocket *wsSocket = Network->FirstSocket(); !Network->FinishedSockets(); wsSocket = Network->NextSocket() )
		{
			if( wsSocket != NULL )
			{
				if( (UI32)wsSocket->IdleTimeout() < uiCurrentTime )
				{
					wsSocket->IdleTimeout( BuildTimeValue( 60.0f ) );
					wsSocket->WasIdleWarned( true );//don't give them the message if they only have 60s
				}
			}
		}
		Network->PopConn();
		worldSaveProgress = 0;
	}
	Network->Off();
	if( regionCheck <= uiCurrentTime || overflow )
	{
		for( UI16 regionCounter = 0; regionCounter < 256; regionCounter++ )
			region[regionCounter]->PeriodicCheck();
		regionCheck = BuildTimeValue( 10 );	// do checks every 10 seconds or so, rather than every single time
		JailSys->PeriodicCheck();
	}

	if( checkspawnregions <= uiCurrentTime && cwmWorldState->ServerData()->GetCheckSpawnRegionSpeed() != -1 )//Regionspawns
	{
		for( i = 1; i < totalspawnregions; i++ )
		{
			cSpawnRegion *spawnReg = spawnregion[i];
			if( spawnReg != NULL && (UI32)spawnReg->GetNextTime() <= uiCurrentTime )
			{
				spawnReg->doRegionSpawn();
				spawnReg->SetNextTime( BuildTimeValue( (R32)( 60 * RandomNum( spawnReg->GetMinTime(), spawnReg->GetMaxTime() ) ) ) );
			}
		}
		checkspawnregions = BuildTimeValue( (R32)cwmWorldState->ServerData()->GetCheckSpawnRegionSpeed() );//Don't check them TOO often (Keep down the lag)
	}
	
	HTMLTemplates->Poll( ETT_PERIODIC );

	int saveinterval = cwmWorldState->ServerData()->GetServerSavesTimerStatus();
	if( saveinterval != 0 )
	{
		if( !autosaved )
		{
			autosaved = true;
#if defined(__LINUX__) || defined(__MINGW32__)
			time((time_t *)&oldtime);
#else
			time(&oldtime);
#endif
		}
#if defined(__LINUX__) || defined(__MINGW32__)
		time((time_t *)&newtime);
#else
		time(&newtime);
#endif

		if( difftime( newtime, oldtime ) >= saveinterval || cwmWorldState->Saving() )
		{
			// Dupois - Added Dec 20, 1999
			// After an automatic world save occurs, lets check to see if
			// anyone is online (clients connected).  If nobody is connected
			// Lets do some maintenance on the bulletin boards.
			if( !now && !cwmWorldState->Saving() )
			{
				Console << "No players currently online. Starting bulletin board maintenance" << myendl;
				savelog( "Bulletin Board Maintenance routine running (AUTO)\n", "server.log" );
				MsgBoardMaintenance();
			}

			autosaved = false;
			cwmWorldState->savenewworld( false );
		}
	}
	
	//Time functions
	if( uotickcount <= uiCurrentTime || ( overflow ) )
	{
		if( cwmWorldState->ServerData()->IncMinute() )
			Weather->NewDay();
		uotickcount = BuildTimeValue( secondsperuominute );
	}
	
	if( lighttime <= uiCurrentTime || ( overflow ) )
	{
		counter = 0;		
		doWorldLight();  //Changes lighting, if it is currently time to.
		Weather->DoStuff();	// updates the weather types
		lighttime = BuildTimeValue( (R32)cwmWorldState->ServerData()->GetSystemTimerStatus( WEATHER ) );	// for testing purposes
		doWeather = true;
	}
	else
		doWeather = false;

	std::set< SubRegion * > regionList;	// we'll get around our npc problem this way, hopefully
	Network->PushConn();
	for( cSocket *iSock = Network->FirstSocket(); !Network->FinishedSockets(); iSock = Network->NextSocket() )
	{
		if( iSock == NULL )
			continue;
		CChar *mChar = iSock->CurrcharObj();
		UI08 worldNumber = mChar->WorldNumber();
		if( isOnline( mChar ) && mChar->GetAccount().wAccountIndex == iSock->AcctNo() )
		{
			if( uiSetFlagTime <= uiCurrentTime || overflow )
				setcharflag( mChar ); // only set flag on npcs every 60 seconds (save a little extra lag)
			genericCheck( mChar );
			checkPC( mChar, doWeather );

			int xOffset = MapRegion->GetGridX( mChar->GetX() );
			int yOffset = MapRegion->GetGridY( mChar->GetY() );
			for( SI08 counter = -1; counter <= 1; counter ++ ) // Check 3 x colums
			{
				for( SI08 ctr2 = -1; ctr2 <= 1; ctr2++ ) // Check 3 y colums
				{
					SubRegion *tC = MapRegion->GetGrid( xOffset + counter, yOffset + ctr2, worldNumber );
					if( tC == NULL )
						continue;
					regionList.insert( tC );
				}
			}
		}
	}
	Network->PopConn();
	std::set< SubRegion * >::iterator tcCheck = regionList.begin();
	while( tcCheck != regionList.end() )
	{
		SubRegion *toCheck = (*tcCheck);
		toCheck->PushChar();
		toCheck->PushItem();
		for( CChar *charCheck = toCheck->FirstChar(); !toCheck->FinishedChars(); charCheck = toCheck->GetNextChar() )
		{
			if( charCheck == NULL )//|| charCheck (int)charCheck == 0xCDCDCDCD )	// for some reason, 0xcdcdcdcd is always the bad thing here
				continue;
			if( charCheck->isFree() ) 
				continue;
			if( uiSetFlagTime <= uiCurrentTime || overflow )
				setcharflag( charCheck ); // only set flag on npcs every 60 seconds (save a little extra lag)
			if( charCheck->IsNpc() ) 
			{
				bool delChar = genericCheck( charCheck );
				if( !delChar )
					checkNPC( charCheck );
			}
			else if( charCheck->GetLogout() != -1 )
			{
				actbTemp = charCheck->GetAccount();
				if(actbTemp.wAccountIndex != AB_INVALID_ID)
				{
					SERIAL oaiw = actbTemp.dwInGame;
					if( oaiw == INVALIDSERIAL )
					{
						charCheck->SetLogout( -1 );
						charCheck->Update();
					}
					else if( oaiw == charCheck->GetSerial() && ( (UI32)charCheck->GetLogout() <= uiCurrentTime || overflow ) )
					{
						actbTemp.dwInGame = INVALIDSERIAL;
						charCheck->SetLogout( -1 );
						charCheck->Update();
					}
				}
			}
		}
		toCheck->PopChar();

		checkItem( toCheck, checkitemstime );

		toCheck->PopItem();
		tcCheck++;
	}
	checktempeffects();
	SpeechSys->Poll();
	if( uiSetFlagTime <= uiCurrentTime ) 
		uiSetFlagTime = BuildTimeValue( 30 ); // Slow down lag "needed" for setting flags, they are set often enough ;-)
	if( checknpcs <= uiCurrentTime ) 
		checknpcs = BuildTimeValue( (R32)cwmWorldState->ServerData()->GetCheckNpcSpeed() );
	if( checkitemstime <= uiCurrentTime ) 
		checkitemstime = BuildTimeValue( (R32)cwmWorldState->ServerData()->GetCheckItemsSpeed() );
	if( shoprestocktime <= uiCurrentTime ) 
		shoprestocktime = BuildTimeValue( (R32)( shoprestockrate * 60 ) );
	if( nextnpcaitime <= uiCurrentTime ) 
		nextnpcaitime = BuildTimeValue( (R32)cwmWorldState->ServerData()->GetCheckNpcAISpeed() );
	if( nextfieldeffecttime <= uiCurrentTime ) 
		nextfieldeffecttime = BuildTimeValue( 0.5f );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void LoadJSEngine( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Loads JavaScript engine
//o---------------------------------------------------------------------------o
void LoadJSEngine( void )
{
	const SI32 DefEngineSize = 0x1000000;

	std::ifstream engineData( "engine.dat" );
	SI32 engineSize = DefEngineSize;
	if( engineData.is_open() )
	{
		char line[1024];
		engineData.getline( line, 1024 );
		if( strlen( line ) > 0 )
		{
			engineSize = makeNum( line );
			if( engineSize < DefEngineSize )
				engineSize = DefEngineSize;
		}
		engineData.close();
	}
	jsRuntime = JS_NewRuntime( engineSize );
	
	Console.PrintSectionBegin();
	Console << "Starting JavaScript Engine...." << myendl;
	
	if( jsRuntime == NULL )
		Shutdown( FATAL_UOX3_JAVASCRIPT );
	jsContext = JS_NewContext( jsRuntime, 0x2000 );
	if( jsContext == NULL )
		Shutdown( FATAL_UOX3_JAVASCRIPT );
	jsGlobal = JS_NewObject( jsContext, &global_class, NULL, NULL ); 
	if( jsGlobal == NULL )
		Shutdown( FATAL_UOX3_JAVASCRIPT );
	JS_InitStandardClasses( jsContext, jsGlobal ); 
	Console << "JavaScript engine startup complete." << myendl;
	Console.PrintSectionBegin();
}

//o---------------------------------------------------------------------------o
//|	Function	-	void InitClasses( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Initialize UOX classes
//o---------------------------------------------------------------------------o
void InitClasses( void )
{
	Console << "Initializing and creating class pointers... " << myendl;
	
	Boats = NULL;		Gumps = NULL;		
	Combat = NULL;		Commands = NULL;
	Items = NULL;		Map = NULL;
	Npcs = NULL;		Skills = NULL;	
	Weight = NULL;		Targ = NULL;
	Network = NULL;		Magic = NULL;		
	Races = NULL;		Weather = NULL;
	Movement = NULL;	Effects = NULL;	
	WhoList = NULL;		OffList = NULL;
	Books = NULL;		GMQueue = NULL;	
	Dictionary = NULL;	Accounts = NULL;
	MapRegion = NULL;	SpeechSys = NULL;
	CounselorQueue = NULL;	GuildSys = NULL;
	HTMLTemplates = NULL;

	// MAKE SURE IF YOU ADD A NEW ALLOCATION HERE THAT YOU FREE IT UP IN Shutdown(...)
	if(( Dictionary = new CDictionaryContainer ) == NULL ) Shutdown( FATAL_UOX3_ALLOC_DICTIONARY );
	if(( Boats = new cBoat )              == NULL ) Shutdown( FATAL_UOX3_ALLOC_BOATS );
	if(( Combat = new cCombat )           == NULL ) Shutdown( FATAL_UOX3_ALLOC_COMBAT );
	if(( Commands = new cCommands )       == NULL ) Shutdown( FATAL_UOX3_ALLOC_COMMANDS );
	if(( Gumps = new cGump )              == NULL ) Shutdown( FATAL_UOX3_ALLOC_GUMPS );
	if(( Items = new cItem )              == NULL ) Shutdown( FATAL_UOX3_ALLOC_ITEMS );
	if(( Map = new cMapStuff )            == NULL ) Shutdown( FATAL_UOX3_ALLOC_MAP );
	if(( Npcs = new cCharStuff )          == NULL ) Shutdown( FATAL_UOX3_ALLOC_NPCS );
	if(( Skills = new cSkills )           == NULL ) Shutdown( FATAL_UOX3_ALLOC_SKILLS );
	if(( Weight = new cWeight )           == NULL ) Shutdown( FATAL_UOX3_ALLOC_WEIGHT );
	if(( Targ = new cTargets )            == NULL ) Shutdown( FATAL_UOX3_ALLOC_TARG );
	if(( Network = new cNetworkStuff )    == NULL ) Shutdown( FATAL_UOX3_ALLOC_NETWORK );
	if(( Magic = new cMagic )             == NULL ) Shutdown( FATAL_UOX3_ALLOC_MAGIC );
	if(( Races = new cRaces )             == NULL ) Shutdown( FATAL_UOX3_ALLOC_RACES );
	if(( Weather = new cWeatherAb )       == NULL ) Shutdown( FATAL_UOX3_ALLOC_WEATHER );
	if(( Movement = new cMovement )       == NULL ) Shutdown( FATAL_UOX3_ALLOC_MOVE );
	if(( Effects = new cTEffect )         == NULL ) Shutdown( FATAL_UOX3_ALLOC_EFFECTS );	// addition of TEffect class, memory reduction (Abaddon, 17th Feb 2000)
	if(( WhoList = new cWhoList )         == NULL ) Shutdown( FATAL_UOX3_ALLOC_WHOLIST );	// wholist
	if(( OffList = new cWhoList( false ) )== NULL ) Shutdown( FATAL_UOX3_ALLOC_WHOLIST );	// offlist
	if(( Books = new cBooks )			  == NULL ) Shutdown( FATAL_UOX3_ALLOC_BOOKS );	// temp value
	if(( GMQueue = new PageVector( "GM Queue" ) )		  == NULL ) Shutdown( FATAL_UOX3_ALLOC_PAGEVECTOR );
	if(( CounselorQueue = new PageVector( "Counselor Queue" ) )== NULL ) Shutdown( FATAL_UOX3_ALLOC_PAGEVECTOR );
	if(( Trigger = new Triggers )		  == NULL ) Shutdown( FATAL_UOX3_ALLOC_TRIGGERS );
	if(( MapRegion = new cMapRegion )	  == NULL ) Shutdown( FATAL_UOX3_ALLOC_MAPREGION );
	
	HTMLTemplates = new cHTMLTemplates;

	//const char *path = cwmWorldState->ServerData()->GetAccountsDirectory();
	//char tbuffer[512];
	//UI32 slen = strlen( path );
	//if( path[slen-1] == '\\' || path[slen-1] == '/' )
	//	sprintf( tbuffer, "%saccounts.adm", path );
	//else
	//	sprintf( tbuffer, "%s/accounts.adm", path );
	
	if(( Accounts = new cAccountClass( cwmWorldState->ServerData()->GetAccountsDirectory()/*tbuffer*/ ) ) == NULL ) Shutdown( FATAL_UOX3_ALLOC_ACCOUNTS );
	if(( SpeechSys = new CSpeechQueue()	)  == NULL ) Shutdown( FATAL_UOX3_ALLOC_SPEECH );
	if(( GuildSys = new CGuildCollection() ) == NULL ) Shutdown( FATAL_UOX3_ALLOC_GUILDS );
	if(( FileLookup = new cServerDefinitions() ) == NULL )	Shutdown( FATAL_UOX3_ALLOC_SCRIPTS );
	if(( JailSys = new JailSystem() ) == NULL ) Shutdown( FATAL_UOX3_ALLOC_WHOLIST );
	Map->Cache = cwmWorldState->ServerData()->GetServerMulCachingStatus();	
	//Console << " Done loading classes!" << myendl;
	DefBase = new cBaseObject();
	DefChar = new CChar(INVALIDSERIAL,true);
	DefItem = new CItem(INVALIDSERIAL,true);
}

//o---------------------------------------------------------------------------o
//|	Function	-	void ParseArgs( int argc, char *argv[] )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Unknown
//o---------------------------------------------------------------------------o
void ParseArgs( int argc, char *argv[] )
{
	for( int i = 1; i < argc; i++ )
	{
		if( !strcmp( argv[i], "-xgm" ) ) 
		{
			Console.PrintSectionBegin();
			Console << "XGM Enabled! Initializing... ";
			xgm = 1;
			Console << "Done!" << myendl;
		}
		else if( !strcmp( argv[i], "-ERROR" ) )
		{
			ErrorCount = makeNum( argv[i+1] );
			i++;
		}
		else if( !strcmp( argv[i], "-cluox100" ) )
		{
			Console << "Using CLUOX Streaming-IO" << myendl;
			setvbuf( stdout, NULL, _IONBF, 0 );
			setvbuf( stderr, NULL, _IONBF, 0 );
			cluox_io = true;
			i++;
			if( i > argc )
			{
				Console.Error( 0, "Fatal error in CLUOX arguments" );
				Shutdown( 10 );
			}
			char *dummy;
			cluox_stdin_writeback = (void *)strtol( argv[i], &dummy, 16 );
		}
		else if( !strncmp( argv[i], "+add:", sizeof(char)*5 ) )
		{
			//	EviLDeD:	030902:	Added this so people could add an account at the command line when they started the server
			Console << "|  Importing Accounts command line \n"; 
			std::string username,password,email;
			char *left=strtok(argv[1],":");
			username=strtok(NULL,",");
			password=strtok(NULL,",");
			email=strtok(NULL,"\n");
			if( left==NULL || username.empty() || password.empty() || email.empty() )
			{
				// there is an error with the command line so we don't want to do anything
				break;
			}
			ACCOUNTSBLOCK actbTemp;
			if(!Accounts->GetAccountByName(username.c_str(),actbTemp))
			{
				Accounts->AddAccount( username, password, email );
				Console << "| AccountImport: Added: " << username << " @ " << email << "\n";
			}
			else
			{
				Console << "| AccountImport: Failure\n";
			}
		}
		else if( !strncmp( argv[i], "+import:", sizeof(char)*8 ) )
		{
			//	EviLDeD:	030902:	Added this so people could add accounts froma file that contains username/password/email format per line
			std::string  filename,username,password,email;
			char *left;
			left			= strtok(argv[1],":");
			filename	= strtok(NULL,",");
			std::ifstream inFile;
			inFile.open( filename.c_str() );
			char szBuffer[127];
			if(inFile.is_open())
			{
				// Ok the files is open lets read it. Otherwise just skip it
				Console << "|  Importing Accounts from \"" << filename << "\"\n"; 
				inFile.getline( szBuffer,127 );
				while(  !inFile.eof() )
				{
					if( (left = strtok(szBuffer,":")) == NULL)
					{
						inFile.getline(szBuffer,127);
						continue;
					}
					left			= strtok(szBuffer,":");
					username	= strtok(NULL,",");
					password	= strtok(NULL,",");
					email			= strtok(NULL,"\n");
					if( left==NULL || username.empty() || password.empty() || email.empty() )
					{
						inFile.getline(szBuffer,127);
						continue;
					}
					ACCOUNTSBLOCK actbTemp;
					if(!Accounts->GetAccountByName(username.c_str(),actbTemp))
					{
						Accounts->AddAccount(username, password, email );
						Console << "| AccountImport: Added: " << username << " @ " << email << "\n";
					}
					else
					{
						Console << "| AccountImport: Failure\n";
					}
					inFile.getline( szBuffer,127);
				};
			}
		}
	}
	
}

//o---------------------------------------------------------------------------o
//|	Function	-	void ResetVars( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Reset global variables
//o---------------------------------------------------------------------------o
void ResetVars( void )
{
	Console.PrintSectionBegin();
	Console << "Initializing global variables  ";
	memset( erroredLayers, 0, sizeof( erroredLayers[0] ) * MAXLAYERS );
	globalSent = 0;
	globalRecv = 0;
	imem = 0;
	cmem = 0;
	uoxtimeout.tv_sec = 0;
	uoxtimeout.tv_usec = 0;
	keeprun = true;
	error = false;
	now = 0;
	secure = true;
	charcount = 0;
	itemcount = 0;
	charcount2 = 1;
	itemcount2 = 0x40000000;
	executebatch = 0;
	showlayer = false;
	autosaved = false;
	
	strcpy( skill[ALCHEMY].madeword,		"mixed" );
	strcpy( skill[ANATOMY].madeword,		"made" );
	strcpy( skill[ANIMALLORE].madeword,		"made" );
	strcpy( skill[ITEMID].madeword,			"made" );
	strcpy( skill[ARMSLORE].madeword,		"made" );
	strcpy( skill[PARRYING].madeword,		"made" );
	strcpy( skill[BEGGING].madeword,		"made" );
	strcpy( skill[BLACKSMITHING].madeword,	"forged" );
	strcpy( skill[BOWCRAFT].madeword,		"bowcrafted" );
	strcpy( skill[PEACEMAKING].madeword,	"made" );
	strcpy( skill[CAMPING].madeword,		"made" );
	strcpy( skill[CARPENTRY].madeword,		"made" );
	strcpy( skill[CARTOGRAPHY].madeword,	"wrote" );
	strcpy( skill[COOKING].madeword,		"cooked" );
	strcpy( skill[DETECTINGHIDDEN].madeword, "made" );
	strcpy( skill[ENTICEMENT].madeword,		"made" );
	strcpy( skill[EVALUATINGINTEL].madeword, "made" );
	strcpy( skill[HEALING].madeword,		"made" );
	strcpy( skill[FISHING].madeword,		"made" );
	strcpy( skill[FORENSICS].madeword,		"made" );
	strcpy( skill[HERDING].madeword,		"made" );
	strcpy( skill[HIDING].madeword,			"made" );
	strcpy( skill[PROVOCATION].madeword,	"made" );
	strcpy( skill[INSCRIPTION].madeword,	"wrote" );
	strcpy( skill[LOCKPICKING].madeword,	"made" );
	strcpy( skill[MAGERY].madeword,			"envoked" );
	strcpy( skill[MAGICRESISTANCE].madeword, "made" );
	strcpy( skill[TACTICS].madeword,		"made" );
	strcpy( skill[SNOOPING].madeword,		"made" );
	strcpy( skill[MUSICIANSHIP].madeword,	"made" );
	strcpy( skill[POISONING].madeword,		"made" );
	strcpy( skill[ARCHERY].madeword,		"made" );
	strcpy( skill[SPIRITSPEAK].madeword,	"made" );
	strcpy( skill[STEALING].madeword,		"made" );
	strcpy( skill[TAILORING].madeword,		"sewn" );
	strcpy( skill[TAMING].madeword,			"made" );
	strcpy( skill[TASTEID].madeword,		"made" );
	strcpy( skill[TINKERING].madeword,		"made" );
	strcpy( skill[TRACKING].madeword,		"made" );
	strcpy( skill[VETERINARY].madeword,		"made" );
	strcpy( skill[SWORDSMANSHIP].madeword,	"made" );
	strcpy( skill[MACEFIGHTING].madeword,	"made" );
	strcpy( skill[FENCING].madeword,		"made" );
	strcpy( skill[WRESTLING].madeword,		"made" );
	strcpy( skill[LUMBERJACKING].madeword,	"made" );
	strcpy( skill[MINING].madeword,			"smelted" );
	strcpy( skill[MEDITATION].madeword,		"envoked" );
	strcpy( skill[STEALTH].madeword,		"made" );
	strcpy( skill[REMOVETRAPS].madeword,	"made" );

	Console.PrintDone();
}

//o---------------------------------------------------------------------------o
//|	Function	-	void InitMultis( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Initialize Multis
//o---------------------------------------------------------------------------o
void InitMultis( void )
{
	Console << "Initializing multis            ";

	CMultiObj *multi = NULL;
	for( CHARACTER c = 0; c < charcount; c++ )
	{
		if( !chars[c].isFree() )
		{
			multi = findMulti( chars[c].GetX(), chars[c].GetY(), chars[c].GetZ(), chars[c].WorldNumber() );
			if( multi != NULL )
				chars[c].SetMulti( multi );
			else
				chars[c].SetMulti( INVALIDSERIAL );
		}
	}
	int fiveCount = itemcount / 22;
	if( fiveCount == 0 )
		fiveCount = 1;
	for( ITEM i = 0; i < itemcount; i++ )
	{
		if( !items[i].isFree() && items[i].GetCont() == INVALIDSERIAL )
		{
			multi = findMulti( items[i].GetX(), items[i].GetY(), items[i].GetZ(), items[i].WorldNumber() );
			if( multi != NULL )
			{
				if( multi != &items[i] ) 
					items[i].SetMulti( multi );
				else 
					items[i].SetMulti( INVALIDSERIAL );
			}
			else
				items[i].SetMulti( INVALIDSERIAL );
		}
	}

	Console.PrintDone();
}

//o---------------------------------------------------------------------------o
//|	Function	-	void DisplayBanner( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	UOX startup stuff
//o---------------------------------------------------------------------------o
void DisplayBanner( void )
{
	Console.PrintSectionBegin();
		
	sprintf( idname, "%s v%s(%s) [%s]\n| Compiled by %s\n| Programmed by: %s", CVC.GetProductName(), CVC.GetVersion(), CVC.GetBuild(), OS_STR, CVC.GetName(), CVC.GetProgrammers() );
 
	/*Console << myendl;
	Console << "Configured for connections with Ignition support and 2.x compatability" << myendl;
	Console << "Copyright (C) 1997, 98 Marcus Rating (Cironian)" << myendl;//<< "|" << myendl;
	Console << "This program is free software; you can redistribute it and/or modify" << myendl;
	Console << "it under the terms of the GNU General Public License as published by" << myendl;
	Console << "the Free Software Foundation; either version 2 of the License, or" << myendl;
	Console << "(at your option) any later version." << myendl << "|" << myendl;
	Console << myendl;*/
	
	Console.TurnYellow();
	Console << "Compiled on ";
	Console.TurnNormal();
	Console << __DATE__ << " (" << __TIME__ << ")" << myendl;

	Console.TurnYellow();
	Console << "Compiled by ";
	Console.TurnNormal();
	Console << CVC.GetName() << myendl;

	Console.TurnYellow();
	Console << "Contact: ";
	Console.TurnNormal();
	Console << CVC.GetEmail() << myendl;
	
	Console.PrintSectionBegin();
}

//o---------------------------------------------------------------------------o
//|	Function	-	void SleepNiceness( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Based on server.scp entry, choose how much CPU UOX will hog
//o---------------------------------------------------------------------------o
void SleepNiceness( void )
{
	switch( cwmWorldState->ServerData()->GetNiceness() )
	{
	case 0:						break;  // very unnice - hog all cpu time
	case 1:
		if( now != 0 ) 
			UOXSleep( 10 );
		else 
			UOXSleep( 90 );
		break;
	case 2: UOXSleep( 10 );		break;
	case 3: UOXSleep( 40 );		break;// very nice
	default: UOXSleep( 10 );	break;
	}

}

//o---------------------------------------------------------------------------o
//|	Function	-	void StartupClearTrades( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	UOX startup stuff
//o---------------------------------------------------------------------------o
void StartupClearTrades( void )
{
	Console.PrintSectionBegin();
	Console << "Clearing all trades            ";
	Loaded = 1;
	clearTrades();
	Console.PrintDone();
}

//o---------------------------------------------------------------------------o
//|	Function	-	int main( int argc, char *argv[] )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	UOX startup stuff
//o---------------------------------------------------------------------------o
int main( int argc, char *argv[] )
{
	UI32 uiNextCheckConn=0;
	UI32 tempSecs, tempMilli, tempTime;
	UI32 loopSecs, loopMilli;
	
	Loaded=0;
	ErrorCount=0;

	// EviLDeD: 042102: I moved this here where it basically should be for any windows application or dll that uses WindowsSockets.
#ifdef __NT__
	wVersionRequested = MAKEWORD( 2, 0 );
	err = WSAStartup( wVersionRequested, &wsaData );
	if( err )
	{
		Console.Error( 0, "Winsock 2.0 not found on your system..." );
		return 1;
	}
#endif

	char temp[1024];
#ifdef _CRASH_PROTECT_
	try {//Crappy error trapping....
#endif
		uiCurrentTime = getclock();
		
#ifdef __NT__
		sprintf( temp, "%s v%s(%s)", CVC.GetProductName(), CVC.GetVersion(), CVC.GetBuild() );
		Console.Start( temp );
#else
		signal( SIGPIPE, SIG_IGN ); // This appears when we try to write to a broken network connection
		signal( SIGTERM, &endmessage );
		signal( SIGQUIT, &endmessage );
		signal( SIGINT, &endmessage ); 
		signal( SIGILL, &illinst );
		signal( SIGFPE, &aus );
		
#endif
		
		Console.PrintSectionBegin();
		Console << "UOX Server start up!" << myendl << "Welcome to " << CVC.GetProductName() << " v" << CVC.GetVersion() << "(" << CVC.GetBuild() << ")" << myendl;
		Console.PrintSectionBegin();

		LoadJSEngine();

		if(( cwmWorldState = new CWorldMain ) == NULL ) 
			Shutdown( FATAL_UOX3_ALLOC_WORLDSTATE );
		LoadINIFile();
		InitClasses();

		ParseArgs( argc, argv );
		ResetVars();
		
		Console << "Loading skill advancement      ";
		loadskills();
		Console.PrintDone();
		
		Console << "Loading IM Menus               ";
		im_loadmenus( "inscribe.gmp", TellScroll ); //loading gump for inscribe()
		Console.PrintDone();
		
		keeprun = Network->kr; // for some technical reasons global varaibles CANT be changed in constructors in c++.
		error = Network->faul;  // i hope i can find a cleaner solution for that, but this works !!!
		// has to here and not at the cal cause it would get overriten later

		// Moved BulkStartup here, dunno why that function was there...
		Console << "Loading dictionaries...        ";
		Console.PrintBasedOnVal( Dictionary->LoadDictionary() >= 0 );

		Console << "Loading teleport               ";
		read_in_teleport();
		Console.PrintDone();
		
		srand( uiCurrentTime ); // initial randomization call
		
		// moved all the map loading into cMapStuff - fur
		Map->Load();
		
		Skills->Load();
		
		Console << "Loading Spawn Regions          ";
		loadSpawnRegions();
		Console.PrintDone();

		Console << "Loading Regions                ";
		loadregions();
		Console.PrintDone();

		Magic->LoadScript();
		
		Console << "Loading Races                  ";
		Races->load();
		Console.PrintDone();

		Console << "Loading Weather                ";
		Weather->Load();
		Weather->NewDay();
		Console.PrintDone();

		Console << "Loading Commands               ";
		Commands->Load();
		Console.PrintDone();
		
		// Rework that...
		Console << "Loading World now              ";
		loadnewworld();

		StartupClearTrades();
		InitMultis();
		
		starttime = uiCurrentTime;
		gcollect();

		FD_ZERO( &conn );
		endtime = 0;
		lclock = 0;

		// no longer Que, because that's taken care of by PageVector
		Console << "Initializing Jail system       ";	
		JailSys->ReadSetup();
		JailSys->ReadData();
		Console.PrintDone();
		
		Console << "Initializing Status system     ";	
		HTMLTemplates->Load();
		Console.PrintDone();

		Console << "Loading custom titles          ";
		loadcustomtitle();
		Console.PrintDone();

		Console << "Loading temporary Effects      ";
		LoadEffects();
		Console.PrintDone();
			

		
		if( cwmWorldState->ServerData()->GetServerAnnounceSavesStatus() )
			cwmWorldState->announce( 1 );
		else
			cwmWorldState->announce( 0 );

		LoadCreatures();
		DisplayBanner();
		//DisplaySettings(); << Moved that to the configuration
		item_test();

		Console << "Loading Accounts               ";
		Accounts->Load();
		Console.PrintDone(); 

		savelog( "-=Server Startup=-\n=======================================================================\n", "server.log" );
		uiCurrentTime = getclock();

		Console << "Initialize Console Thread      ";
#ifdef __LINUX__
		int conthreadok = pthread_create(&cons,NULL,CheckConsoleKeyThread , NULL );
#else
		int conthreadok = _beginthread( CheckConsoleKeyThread , 0 , NULL );
#endif
#ifdef __LOGIN_THREAD__
 #ifdef __LINUX__
		pthread_create(&netw,NULL, NetworkPollConnectionThread,  NULL );
 #else
		_beginthread( NetworkPollConnectionThread, 0, NULL );
 #endif //linux
#endif
		Console.PrintDone();

		Console.PrintSectionBegin();
		Console << "UOX: Startup Complete" << myendl;
		Console.PrintSectionBegin();
		
		// MAIN SYSTEM LOOP
		while( keeprun )
		{
			//	EviLDeD	-	February 27, 2000
			//	Just in case the thread doesn't start then use the main threaded copy
			if( conthreadok == -1 )
				checkkey();
			SleepNiceness();
			//	EviLDeD	-	End
			if(loopTimeCount >= 1000)       
			{
				loopTimeCount = 0;
				loopTime = 0;
			}
			loopTimeCount++;
			
			StartMilliTimer( loopSecs, loopMilli );
			
			if( networkTimeCount >= 1000 )
			{
				networkTimeCount = 0;
				networkTime = 0;
			}
			
			StartMilliTimer( tempSecs, tempMilli );
#ifndef __LOGIN_THREAD__
			if( uiNextCheckConn <= uiCurrentTime || overflow ) // Cut lag on CheckConn by not doing it EVERY loop.
			{
				Network->CheckConnections();
				uiNextCheckConn = BuildTimeValue( 1.0f );
			}
			Network->CheckMessages();
#else
			Network->CheckMessage();
//			Network->CheckXGM();
#endif
			
			tempTime = CheckMilliTimer( tempSecs, tempMilli );
			networkTime += tempTime;
			networkTimeCount++;
			
			if( timerTimeCount >= 1000 )
			{
				timerTimeCount = 0;
				timerTime = 0;
			}
			
			StartMilliTimer( tempSecs, tempMilli );
			
			checktimers();
			uiCurrentTime = getclock(); // getclock() only once
			tempTime = CheckMilliTimer( tempSecs, tempMilli );
			timerTime += tempTime;
			timerTimeCount++;
			
			if( autoTimeCount >= 1000 )
			{
				autoTimeCount = 0;
				autoTime = 0;
			}
			StartMilliTimer( tempSecs, tempMilli );
			
			checkauto();
			
			tempTime = CheckMilliTimer( tempSecs, tempMilli );
			autoTime  += tempTime;
			autoTimeCount++;
			StartMilliTimer( tempSecs, tempMilli );
			Network->ClearBuffers();
			tempTime = CheckMilliTimer( tempSecs, tempMilli );
			networkTime += tempTime;
			tempTime = CheckMilliTimer( loopSecs, loopMilli );
			loopTime += tempTime;
			DoMessageLoop();
		}

		
		Console.PrintSectionBegin();
		sysbroadcast( "The server is shutting down." );
		Console << "Clearing IM Menus...";
		im_clearmenus();
		Console.PrintDone();
		Console << "Closing sockets...";
		netpollthreadclose = true;
		UOXSleep( 1000 );
		Network->SockClose();
		Console.PrintDone();

		if( !cwmWorldState->Saving() )
		{
			do 
			{
				cwmWorldState->savenewworld( true );
			} while( cwmWorldState->Saving() );
		}
		
		cwmWorldState->ServerData()->save();

		savelog("Server Shutdown!\n=======================================================================\n\n\n","server.log");

		conthreadcloseok = true;	//	This will signal the console thread to close
		Shutdown( 0 );

#ifdef _CRASH_PROTECT_
	} catch ( ... ) 
	{//Crappy error handling...
		Console << "Unknown exception caught, hard crash avioded!" << myendl;
		Shutdown( UNKNOWN_ERROR );
	}
#endif
	
	return( 0 );	
}

void LoadINIFile( void )
{
	// hmm, fileExists isn't a valid func... let's call our
	bool uox3test = FileExists( "uox3test.ini" );
	bool uox3     = FileExists( "uox3.ini" );
	bool uox      = FileExists( "uox.ini" );
	if( !uox )
	{
		if( uox3 ) 
		{
			Console << "NOTICE: uox3.ini is no longer needed." << myendl;
			Console << "Rewriting as uox.ini." << myendl;
			cwmWorldState->ServerData()->load( "uox3.ini" );//load this anyway, in case they don't have the other one.
			cwmWorldState->ServerData()->save();
		}
		else if( uox3test ) 
		{
			Console << "NOTICE: uox3test.ini is no longer needed." << myendl;
			Console << "Rewriting as uox.ini." << myendl;
			cwmWorldState->ServerData()->load( "uox3test.ini" );//load this anyway, in case they don't have the other one.
			cwmWorldState->ServerData()->save();
		}
	}
	else if( uox3 || uox3test )
	{
		Console << "You have both old style (uox3.ini and/or uox3test.ini) and new style (uox.ini) files." << myendl;
		Console << "We will only be reading the uox.ini file" << myendl;
	}
	cwmWorldState->ServerData()->load();
}


//o---------------------------------------------------------------------------o
//|            Function     - Restart()
//|            Date         - 1/7/00
//|            Programmer   - Zippy
//o---------------------------------------------------------------------------o
//|            Purpose      - Restarts the server, passes the server number of 
//|								Number of crashes so far, if < 10 then the
//|								Server will restart itself.
//o---------------------------------------------------------------------------o
void Restart( UI16 ErrorCode = UNKNOWN_ERROR )
{
	if( !ErrorCode )
		return;
	char temp[1024];
	if( cwmWorldState->ServerData()->GetServerCrashProtectionStatus() > 1 )
	{		
		if( ErrorCount < 10 )
		{
			ErrorCount++;
			
			sprintf( temp, "Server crash #%i from unknown error, restarting.", ErrorCount );
			savelog( temp, "server.log" );
			Console << temp << myendl;
			
			sprintf(temp, "uox.exe -ERROR %i", ErrorCount);
			
			if( xgm )
				strcat( temp, " -xgm" );
			
			system( temp );
			exit(ErrorCode); // Restart successful Don't give them key presses or anything, just go out.
		} 
		else 
		{
			savelog("10th Server crash, server shutting down.", "server.log");
			Console << "10th Server crash, server shutting down" << myendl;
		}
	} 
	else 
		savelog("Server crash!","server.log");
}

//o---------------------------------------------------------------------------o
//|            Function     - void Shutdown( int retCode )
//|            Date         - Oct. 09, 1999
//|            Programmer   - Krazyglue
//o---------------------------------------------------------------------------o
//|            Purpose      - Handled deleting / free() ing of pointers as neccessary
//|                                   as well as closing open file handles to avoid file
//|                                   file corruption.
//|                                   Exits with proper error code.
//o---------------------------------------------------------------------------o
void Shutdown( SI32 retCode )
{
	Console.PrintSectionBegin();
	Console << "Beginning UOX final shut down sequence..." << myendl;

	if (HTMLTemplates)
	{
		Console << "HTMLTemplates object detected. Writing Offline HTML Now..." << myendl;
	  HTMLTemplates->Poll( true /*ETT_OFFLINE */);
	}
	else
		Console << "HTMLTemplates object not found." << myendl;


	if( cwmWorldState->ServerData()->GetServerCrashProtectionStatus() >= 1 && retCode && Loaded && cwmWorldState && !cwmWorldState->Saving() )
	{//they want us to save, there has been an error, we have loaded the world, and WorldState is a valid pointer.
		do
		{
			cwmWorldState->savenewworld( true );
		} while( cwmWorldState->Saving() );
	}
	
	Console << "Cleaning up item and character memory... ";
	items.Cleanup();
	chars.Cleanup();
	Console.PrintDone();

	Console << "Destroying class objects and pointers... ";
	// delete any objects that were created (delete takes care of NULL check =)
	delete DefBase;
	delete DefChar;
	delete DefItem;

	delete cwmWorldState;
	delete Boats;
	delete Combat;
	delete Commands;
	delete Gumps;
	delete Items;
	delete Map;
	delete Npcs;
	delete Skills;
	delete Weight;
	delete Targ;
	delete Magic;
	delete Races;
	delete Weather;
	delete Movement;
	delete Network;
	delete Effects;
	delete WhoList;
	delete OffList;
	delete Books;
	delete GMQueue;
	delete HTMLTemplates;
	delete CounselorQueue;
	delete Dictionary;
	delete Accounts;
	if (Trigger)
	        Trigger->Cleanup();//must be called to delete some things the still reference Trigger.
	delete Trigger;
	delete MapRegion;
	delete SpeechSys;
	delete GuildSys;
	delete FileLookup;
	delete JailSys;
	Console.PrintDone();

	//Lets wait for console thread to quit here
#ifdef __LINUX__
	pthread_join( cons, NULL );
#ifdef __LOGIN_THREAD__
	pthread_join( newt, NULL );
#endif
#endif

	// don't leave file pointers open, could lead to file corruption
	if( loscache )			
		delete [] loscache;
	if( itemids )			
		delete [] itemids;
	Console << "Destroying JS instances... ";
	JS_DestroyContext( jsContext );
	JS_DestroyRuntime( jsRuntime );
	Console.PrintDone();

	Console.PrintSectionBegin();
	if( retCode && Loaded )//do restart unless we have crashed with some error.
		Restart( (UI16)retCode );

	Console.TurnGreen();
	Console << "Server shutdown complete!" << myendl;
	Console << "Thank you for supporting " << CVC.GetName() << myendl;
	Console.TurnNormal();
	Console.PrintSectionBegin();
	
	// dispay what error code we had
	// don't report errorlevel for no errors, this is confusing ppl - fur
	if( retCode )
	{
		Console.TurnRed();
		Console << "Exiting UOX with errorlevel " << retCode << myendl;
		Console.TurnNormal();
#ifndef __LINUX__
		Console << "Press Return to exit " << myendl;
		std::string throwAway;
		std::getline(std::cin, throwAway);
#endif
	}
	else
	{
		Console.TurnGreen();
		Console << "Exiting UOX with no errors..." << myendl;
		Console.TurnNormal();
	}
	
	Console.PrintSectionBegin();
	exit(retCode);
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool isHuman( CChar *p )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check if character is Human or NPC
//o---------------------------------------------------------------------------o
bool isHuman( CChar *p )
{
	if( p->GetxID() == 0x0190 || p->GetxID() == 0x0191 )
		return true;
	else 
		return false;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void objTeleporters( CChar *s )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Called when a character walks into/on an object teleporter
//o---------------------------------------------------------------------------o
void objTeleporters( CChar *s )
{
	SI16 x = s->GetX(), y = s->GetY();
	
	int xOffset = MapRegion->GetGridX( s->GetX() );
	int yOffset = MapRegion->GetGridY( s->GetY() );
	UI08 worldNumber = s->WorldNumber();
	for( SI08 counter1 = -1; counter1 <= 1; counter1++ )
	{
		for( SI08 counter2 = -1; counter2 <= 1; counter2++ )
		{
			SubRegion *toCheck = MapRegion->GetGrid( xOffset + counter1, yOffset + counter2, worldNumber );
			if( toCheck == NULL )
				continue;
			toCheck->PushItem();
			for( CItem *itemCheck = toCheck->FirstItem(); !toCheck->FinishedItems(); itemCheck = toCheck->GetNextItem() )
			{
				if( itemCheck == NULL )
					continue;
				if( itemCheck->GetX() == x && itemCheck->GetY() == y && 
					( abs( itemCheck->GetZ() ) + 10 >= abs( s->GetZ() ) ) && 
					( abs( itemCheck->GetZ() ) - 10 <= abs( s->GetZ() ) ) )
				{
					switch( itemCheck->GetType() )
					{
					case 60:														// teleporters
						if( itemCheck->GetMoreX() + itemCheck->GetMoreY() + itemCheck->GetMoreZ() > 0 )
						{
							s->SetLocation( (UI16)itemCheck->GetMoreX(), (UI16)itemCheck->GetMoreY(), (SI08)itemCheck->GetMoreZ() );
							s->Teleport();
						}
						break;
					case 80:														// advancement gates
					case 81:
						if( !s->IsNpc() )
						{
							if( itemCheck->GetMore() != 0 )
							{
								if( s->GetSerial() == itemCheck->GetMore() )
									advanceObj( s, itemCheck->GetMoreX(), ( itemCheck->GetType() == 81 ) );
							}
							else
								advanceObj( s, itemCheck->GetMoreX(), ( itemCheck->GetType() == 81 ) );
						}
						break;
					case 82:	MonsterGate( s, itemCheck->GetMoreX() );	break;	// monster gates
					case 83:														// race gates
						Races->gate( s, itemCheck->GetMoreX(), itemCheck->GetMoreY() != 0 );
						break;
					case 85:														// damage objects
						if( !s->IsInvulnerable() )
						{
							s->SetHP( (SI16)( s->GetHP() - ( itemCheck->GetMoreX() + RandomNum( itemCheck->GetMoreY(), itemCheck->GetMoreZ() ) ) ) );
							if( s->GetHP() < 1 ) 
								s->SetHP( 0 );
							updateStats( s, 0 );
							if( s->GetHP() <= 0 ) 
								doDeathStuff( s );
						}
						break;
					case 86:														// sound objects
						if( (UI32)RandomNum(1,100) <= itemCheck->GetMoreZ() )
							soundeffect( itemCheck, (UI16)( (itemCheck->GetMoreX()<<8) + itemCheck->GetMoreY() ) );
						break;

					case 89:
						SocketMapChange( calcSocketObjFromChar( s ), s, itemCheck );
						break;
					case 111:	s->SetKills( 0 );		break;			// zero kill gate
					case 234:	
						if( itemCheck->GetType() == 234 && !s->IsNpc() )				// world change gate
							SendWorldChange( (WorldType)itemCheck->GetMoreX(), calcSocketObjFromChar( s ) );
						break;
					}
				}
			}
			toCheck->PopItem();
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool checkBoundingBox( SI16 xPos, SI16 yPos, SI16 fx1, SI16 fy1, SI08 fz1, SI16 fx2, SI16 fy2 )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check bounding box
//o---------------------------------------------------------------------------o
bool checkBoundingBox( SI16 xPos, SI16 yPos, SI16 fx1, SI16 fy1, SI08 fz1, SI16 fx2, SI16 fy2, UI08 worldNumber )
{
	if( xPos >= ( ( fx1 < fx2 ) ? fx1 : fx2 ) && xPos <= ( ( fx1 < fx2 ) ? fx2 : fx1 ) )
	{
		if( yPos >= ( ( fy1 < fy2 ) ? fy1 : fy2 ) && yPos <= ( ( fy1 < fy2 ) ? fy2 : fy1 ) )
		{
			if( fz1 == -1 || abs( fz1 - Map->Height( xPos, yPos, fz1, worldNumber ) ) <= 5 )
				return true;
		}
	}
	return false;
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool checkBoundingCircle( SI16 xPos, SI16 yPos, SI16 fx1, SI16 fy1, SI08 fz1, int radius )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check bounding circle
//o---------------------------------------------------------------------------o
bool checkBoundingCircle( SI16 xPos, SI16 yPos, SI16 fx1, SI16 fy1, SI08 fz1, int radius, UI08 worldNumber )
{
	if( ( xPos - fx1 ) * ( xPos - fx1 ) + ( yPos - fy1 ) * ( yPos - fy1 ) <= radius * radius )
	{
		if( fz1 == -1 || abs( fz1 - Map->Height( xPos, yPos, fz1, worldNumber ) ) <= 5 )
			return true;
	}
	return false;
}

//o---------------------------------------------------------------------------o
//|	Function	-	UI32 getclock( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Return CPU time used, Emulates clock()
//o---------------------------------------------------------------------------o
#ifndef __NT__
UI32 getclock( void )
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	// We want to keep the value within 32 bits; we could also substract
	// startup time I suppose
	return ((tv.tv_sec - 900000000) * CLOCKS_PER_SEC) +
		tv.tv_usec / (1000000 / CLOCKS_PER_SEC);
}
#endif

//o---------------------------------------------------------------------------o
//|	Function	-	void doLight( cSocket *s, char level )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Send light level to players client
//o---------------------------------------------------------------------------o
void doLight( cSocket *s, char level )
{
	if( s == NULL )
		return;

	CChar *mChar = s->CurrcharObj();
	CPLightLevel toSend( level );

	if( mChar->GetFixedLight() != 255 )
	{
		toSend.Level( 0 );
		s->Send( &toSend );
		return;
	}

	UI08 curRegion = mChar->GetRegion();
	CWeather *wSys =  Weather->Weather( region[curRegion]->GetWeather() );
	LIGHTLEVEL toShow;

	SI16 dunLevel = cwmWorldState->ServerData()->GetDungeonLightLevel();
	// we have a valid weather system
	if( wSys != NULL )
	{
		R32 lightMin = wSys->LightMin();
		R32 lightMax = wSys->LightMax();
		//if( lightMin > 300 && lightMax < 300 )
		//{
		//	R32 i = wSys->CurrentLight();
		//	if( Races->VisLevel( mChar->GetRace() ) > i )
		//		toShow = 0;
		//	else
		//		toShow = static_cast<LIGHTLEVEL>(i - Races->VisLevel( mChar->GetRace() ));
			toSend.Level( level /*toShow*/ );
		//}
		//else
			//toSend.Level( 0 );
	}
	else
	{
		if( inDungeon( mChar ) )
		{
			if( Races->VisLevel( mChar->GetRace() ) > dunLevel )
				toShow = 0;
			else
				toShow = static_cast<LIGHTLEVEL>(dunLevel - Races->VisLevel( mChar->GetRace() ));
			toSend.Level( toShow );
		}
	}
	s->Send( &toSend );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void doWorldLight( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Set world light level based on time of day and script settings
//o---------------------------------------------------------------------------o
void doWorldLight( void )
{
	SI16 worlddarklevel = cwmWorldState->ServerData()->GetWorldLightDarkLevel();
	SI16 worldbrightlevel = cwmWorldState->ServerData()->GetWorldLightBrightLevel();

	bool ampm = cwmWorldState->ServerData()->GetServerTimeAMPM();

	R32 hourIncrement = R32( fabs( ( worlddarklevel - worldbrightlevel ) / 12.0f ) );	// we want the amount to subtract from LightMax in the morning / add to LightMin in evening
	if( ampm )
		cwmWorldState->ServerData()->SetWorldLightCurrentLevel( (UI16)( worldbrightlevel + hourIncrement ) );
	else
		cwmWorldState->ServerData()->SetWorldLightCurrentLevel( (UI16)( worlddarklevel - hourIncrement ) );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void telltime( cSocket *s )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get in-game time
//o---------------------------------------------------------------------------o
void telltime( cSocket *s )
{
	char tstring[100];
	char tstring2[100];
	UI08 hour = cwmWorldState->ServerData()->GetServerTimeHours();
	UI08 minute = cwmWorldState->ServerData()->GetServerTimeMinutes();
	bool ampm = cwmWorldState->ServerData()->GetServerTimeAMPM();

	int lhour = hour;
	
	if( minute <= 14 ) 
		strcpy( tstring, Dictionary->GetEntry( 1248 ) );
	else if( minute >= 15 && minute <= 30 ) 
		strcpy( tstring, Dictionary->GetEntry( 1249 ) );
	else if( minute >= 30 && minute <= 45 ) 
		strcpy( tstring, Dictionary->GetEntry( 1250 ) );
	else
	{
		strcpy( tstring, Dictionary->GetEntry( 1251 ) );
		lhour++;
		if( lhour == 12 ) 
			lhour = 0;
	}
	if( lhour >= 1 && lhour <= 11 )
		sprintf( tstring2, Dictionary->GetEntry( 1252 ), tstring );
	else if( lhour == 1 && ampm )
		sprintf( tstring2, Dictionary->GetEntry( 1263 ), tstring );
	else
		sprintf( tstring2, Dictionary->GetEntry( 1264 ), tstring );
	
	if( lhour == 0 ) 
		strcpy( tstring, tstring2 );
	else if( ampm )
	{
		if( lhour >= 1 && lhour < 6 ) 
		{
			sysmessage( s, 1265, tstring2 );
			return;
		}
		else if( lhour >= 6 && lhour < 9 ) 
		{
			sysmessage( s, 1266, tstring2 );
			return;
		}
		else 
		{
			sysmessage( s, 1267, tstring2 );
			return;
		}
	}
	else
	{
		if( lhour >= 1  && lhour < 5 ) 
		{
			sysmessage( s, 1268, tstring2 );
			return;
		}
		else 
		{
			sysmessage( s, 1269, tstring2 );
			return;
		}
	}
	
	sysmessage( s, tstring );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void updateskill( cSocket *mSock, UI08 skillnum )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Update a certain skill
//o---------------------------------------------------------------------------o
void updateskill( cSocket *mSock, UI08 skillnum )
{
	CChar *mChar = mSock->CurrcharObj();
	CPUpdIndSkill toSend( (*mChar), (UI08)skillnum );
	mSock->Send( &toSend );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void impaction( cSocket *s, int act )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Unknown
//o---------------------------------------------------------------------------o
void impaction( cSocket *s, int act )
{
	CChar *mChar = s->CurrcharObj();
	if( mChar->IsOnHorse() && ( act == 0x10 || act == 0x11 ) )
	{
		action( s, 0x1B );
		return;
	}
	if( ( mChar->IsOnHorse() || ( mChar->GetID( 1 ) && mChar->GetID( 2 ) < 90 ) ) && act == 0x22 )
		return;
	action( s, (SI16)act );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void getCharDir( CChar *a, SI16 x, SI16 y )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get characters direction
//o---------------------------------------------------------------------------o
UI08 getCharDir( CChar *a, SI16 x, SI16 y )
{
	SI16 xdif = (SI16)( x - a->GetX() );
	SI16 ydif = (SI16)( y - a->GetY() );
	
	if( xdif == 0 && ydif < 0 ) 
		return NORTH;
	else if( xdif > 0 && ydif < 0 ) 
		return NORTHEAST;
	else if( xdif > 0 && ydif ==0 ) 
		return EAST;
	else if( xdif > 0 && ydif > 0 ) 
		return SOUTHEAST;
	else if( xdif ==0 && ydif > 0 ) 
		return SOUTH;
	else if( xdif < 0 && ydif > 0 ) 
		return SOUTHWEST;
	else if( xdif < 0 && ydif ==0 ) 
		return WEST;
	else if( xdif < 0 && ydif < 0 ) 
		return NORTHWEST;
	else 
		return UNKNOWNDIR;
}

//o---------------------------------------------------------------------------o
//|	Function	-	UI08 getFieldDir( CChar *s, SI16 x, SI16 y )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get field direction
//o---------------------------------------------------------------------------o
UI08 getFieldDir( CChar *s, SI16 x, SI16 y )
{
	UI08 dir = getCharDir( s, x, y );
	switch( dir )
	{
	case 0:
	case 4:
		return 0;
	case 2:
	case 6:
		return 1;
	case 1:
	case 3:
	case 5:
	case 7:
	case 0xFF:
		switch( s->GetDir() )
		{
		case 0:
		case 4:
			return 0;
		case 2:
		case 6:
			return 1;
		case 1:
		case 3:
		case 5:
		case 7:
			return 1;
		default:
			Console.Error( 2, " Fallout of switch statement without default. uox3.cpp, fielddir()" );
			return 0;
		}
		default:
			Console.Error( 2, " Fallout of switch statement without default. uox3.cpp, fielddir()" );
			return 0;
	}
	return 1;
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool indungeon( CChar *s )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Determine if player is inside a dungeon
//o---------------------------------------------------------------------------o
bool inDungeon( CChar *s )
{
	return region[s->GetRegion()]->IsDungeon();
}

//o---------------------------------------------------------------------------o
//|	Function	-	void openBank( cSocket *s, CChar *i )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Opens players bank box
//o---------------------------------------------------------------------------o
void openBank( cSocket *s, CChar *i )
{
	char temp[1024];
	ITEMLIST *ownedItems = i->GetOwnedItems();

	for( UI32 ci = 0; ci < ownedItems->size(); ci++ )
	{
		CItem *oItem = (*ownedItems)[ci];
		if( oItem != NULL )
		{
			if( oItem->GetType() == 1 && oItem->GetMoreX() == 1 )
			{
				if( cwmWorldState->ServerData()->GetWildernessBankStatus() )
				{
					if( oItem->GetMoreY() == 0 && oItem->GetMoreZ() == 0 ) // if not initialized yet for the special bank
						oItem->SetMoreY( 123 ); // convert to new special bank
				}
				if( cwmWorldState->ServerData()->GetWildernessBankStatus() )
				{
					if( oItem->GetMoreY() == 123 ) // check if a goldbank
					{
						CPWornItem toWear = (*oItem);
						s->Send( &toWear );
						openPack( s, oItem );
						return;
					}
				} 
				else// else if not using specialbank
				{ // don't check for goldbank
					CPWornItem toWearO = (*oItem);
					s->Send( &toWearO );
					openPack(s, oItem );
					return;
				}
				
			}
		}
	}
	
	sprintf( temp, Dictionary->GetEntry( 1283 ), i->GetName() );
	CItem *c = Items->SpawnItem( s, 1, temp, false, 0x09AB, 0, false, false );
	
	c->SetLayer( 0x1D );
	c->SetOwner( i->GetSerial() );
	if( !c->SetCont( i->GetSerial() ) )
		return;
	c->SetMoreX( 1 );
	if( cwmWorldState->ServerData()->GetWildernessBankStatus() ) // Special Bank
		c->SetMoreY( 123 ); // gold only bank
	c->SetType( 1 );
	CPWornItem toWear = (*c);
	s->Send( &toWear );
	openPack( s, c );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void openSpecialBank( cSocket *s, CChar *i )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Opens players special bank box (Can only hold items on a
//|					town by town basis)
//o---------------------------------------------------------------------------o
void openSpecialBank( cSocket *s, CChar *i )
{
	CChar *mChar = s->CurrcharObj();
	ITEMLIST *ownedItems = i->GetOwnedItems();
	for( UI32 ci = 0; ci < ownedItems->size(); ci++ )
	{
		CItem *oItem = (*ownedItems)[ci];
		if( oItem != NULL )
		{
			if( oItem->GetType() == 1 && oItem->GetMoreX() == 1 && oItem->GetMoreY() != 123 ) // specialbank and the current region
			{
				if( oItem->GetMoreZ() == 0 ) // convert old banks into new banks
					oItem->SetMoreZ( mChar->GetRegion() );
				if( oItem->GetMoreZ() == mChar->GetRegion() )
				{
					CPWornItem toWear = (*oItem);
					s->Send( &toWear );
					openPack( s, oItem );
					return;
				}
			}
		}
	}

	char temp[1024];
	sprintf( temp, Dictionary->GetEntry( 1284 ), i->GetName() );
	CItem *c = Items->SpawnItem( s, 1, temp, false, 0x09AB, 0, false, false );
	
	c->SetLayer( 0x1D );
	c->SetOwner( i->GetSerial() );
	if( !c->SetCont( i->GetSerial() ) )
		return;
	c->SetMoreX( 1 );
	c->SetMoreY( 0 ); // this's an all-items bank
	c->SetMoreZ( mChar->GetRegion() ); // let's store the region
	c->SetType( 1 );
	
	CPWornItem toWear = (*c);
	s->Send( &toWear );
	openPack( s, c );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void deathAction( CChar *s, CItem *x )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Plays a characters death animation
//o---------------------------------------------------------------------------o
void deathAction( CChar *s, CItem *x )
{
	CPDeathAction toSend( (*s), (*x) );
	toSend.FallDirection( (UI08)RandomNum( 0, 1 ) );
	Network->PushConn();
	for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
	{
		if( tSock->CurrcharObj() != s && charInRange( s, tSock->CurrcharObj() ) )
			tSock->Send( &toSend );
	}
	Network->PopConn();
}

//o---------------------------------------------------------------------------o
//|	Function	-	void deathMenu( cSocket *s )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Called when player dies, sends the "death menu"
//o---------------------------------------------------------------------------o
void deathMenu( cSocket *s )
{
	CPResurrectMenu toSend( 0 );
	s->Send( &toSend );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void gettokennum( char *s, int num )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Unknown
//o---------------------------------------------------------------------------o
void gettokennum( const char * s, int num, char *gettokenstr )
{
	memset( gettokenstr, 0, 255 );
	
	int i = 0;
	while( num != 0 )
	{
		if( s[i] == 0 )
			num--;
		else
		{
			if( s[i] == ' ' && i != 0 && s[i-1] != ' ' )
				num--;
			i++;
		}
	}
	int j = 0;
	while( num != -1 )
	{
		if( s[i] == 0 )
			num--;
		else
		{
			if( s[i] == ' ' && i != 0 && s[i-1] != ' ')
				num--;
			else
			{
				gettokenstr[j] = s[i];
				j++;
			}
			i++;
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void setRandomName( CChar *s, char *namelist )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Sets a character with a random name from NPC.scp namelist
//o---------------------------------------------------------------------------o
void setRandomName( CChar *s, const char *namelist )
{
	char sect[512];
	
	sprintf( sect, "RANDOMNAME %s", namelist );
	
	char tempName[512];

	ScriptSection *RandomName = FileLookup->FindEntry( sect, npc_def );
	if( RandomName == NULL )
	{
		sprintf( tempName, "Error Namelist %s Not Found", namelist );
		s->SetName( tempName );
		return;
	}
	
	int i = RandomName->NumEntries();

	sprintf( tempName, "namecount %i", i );
	s->SetName( tempName );
	if( i > 0 )
	{
		i = rand()%(i);
		const char *tag = RandomName->MoveTo( (SI16)i );
		s->SetName( tag );
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	CItem *getRootPack( CItem *p )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Gets the root container an item is in (if any)
//o---------------------------------------------------------------------------o
CItem *getRootPack( CItem *p )
{
	UI08 a = 0;
	CItem *currentContainer = NULL;
	while( a < 50 ) 
	{
		if( p == NULL )				// Non existent item
			return currentContainer;
		if( p->GetCont() == INVALIDSERIAL )	// It's on the ground, so it MUST be the root pack
			return p;
		if( p->GetCont( 1 ) < 0x40 )		// It's a character
			return currentContainer;	// Return current container
		
		p = calcItemObjFromSer( p->GetCont() );
		if( p != NULL )
			currentContainer = p;
		a++;
	}
	return currentContainer;
}

//o---------------------------------------------------------------------------o
//|	Function	-	CChar *getPackOwner( CItem *p )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Returns a containers owner
//o---------------------------------------------------------------------------o
CChar *getPackOwner( CItem *p )
{
	if( p == NULL || p->GetCont() == INVALIDSERIAL ) 
		return NULL;
	if( p->GetCont( 1 ) < 0x40 ) 
		return calcCharObjFromSer( p->GetCont() );
	int a = 0, b;
	CItem *x = p;
	do 
	{
		if( a >= 50 || x == NULL ) 
			return NULL; // Too many packs! must stop endless loop!
		if( x->GetCont() == INVALIDSERIAL ) 
			return NULL;
		if( x->GetCont( 1 ) >= 0x40 )
			x = calcItemObjFromSer( x->GetCont() );
		if( x != NULL ) 
			b = x->GetCont( 1 ); 
		else 
			b = 0x42;
		a++;
	} while( b >= 0x40 );
	return calcCharObjFromSer( x->GetCont() );
}

//o---------------------------------------------------------------------------o
//|	Function	-	int getTileName( CItem *i, char* itemname )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Returns the lenght of an items name from tiledata.mul and
//|					sets itemname to the name
//o---------------------------------------------------------------------------o
int getTileName( CItem *i, char *itemname )
{
	if( i->GetName()[0] != '#' )
	{
		strcpy( itemname, i->GetName() );
		return strlen(itemname)+1;
	}
	CTile tile;
	Map->SeekTile( i->GetID(), &tile);
	if( tile.DisplayAsAn() ) 
		strcpy( itemname, "an " );
	else if( tile.DisplayAsA() ) 
		strcpy( itemname, "a " );
	else 
		itemname[0] = 0;

	int namLen = strlen( itemname );
	const char *tName = tile.Name();
	UI08 mode = 0;
	for( UI32 j = 0; j < strlen( tName ); j++ )
	{
		if( tName[j] == '%' )
		{
			if( mode )
				mode = 0;
			else
				mode = 2;
		}
		else if( tName[j] == '/' && mode == 2 )
			mode = 1;
		else if( mode == 0 || ( mode == 1 && i->GetAmount() == 1 ) || ( mode == 2 && i->GetAmount() > 1 ) )
		{
			itemname[namLen++] = tName[j];
			itemname[namLen  ] = '\0';
		}
	}
	return strlen(itemname)+1;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void addgold( cSocket *s, int totgold )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Adds gold to characters total gold
//o---------------------------------------------------------------------------o
void addgold( cSocket *s, SI32 totgold )
{
	Items->SpawnItem( s, totgold, "#", true, 0x0EED, 0, true, true );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void usePotion( CChar *p, CItem *i )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Character uses a potion
//o---------------------------------------------------------------------------o
void usePotion( CChar *p, CItem *i )
{
	if( p == NULL || i == NULL )
		return;
	int x;

	cSocket *mSock = calcSocketObjFromChar( p );
	if( cwmWorldState->ServerData()->GetPotionDelay() != 0 )
		tempeffect( p, p, 26, 0, 0, 0 );
	switch( i->GetMoreY() )
	{
	case 1: // Agility Potion
		staticeffect( p, 0x373A, 0, 15 );
		switch( i->GetMoreZ() )
		{
		case 1:
			tempeffect( p, p, 6, (UI16)RandomNum( 6, 15 ), 0, 0 );
			sysmessage( mSock, 1608 );
			break;
		case 2:
			tempeffect( p, p, 6, (UI16)RandomNum( 11, 30 ), 0, 0 );
			sysmessage( mSock, 1609 );
			break;
		default:
			Console.Error( 2, " Fallout of switch statement without default. uox3.cpp, usepotion()" );
			return;
		}
		soundeffect( p, 0x01E7 );
		if( mSock != NULL ) 
			updateStats( p, 2 );
		break;
	case 2: // Cure Potion
		if( p->GetPoisoned() < 1 ) 
			sysmessage( mSock, 1344 );
		else
		{
			switch( i->GetMoreZ() )
			{
			case 1:
				x = RandomNum( 1, 100 );
				if( p->GetPoisoned() == 1 && x < 81 ) 
					p->SetPoisoned( 0 );
				else if( p->GetPoisoned() == 2 && x < 41 ) 
					p->SetPoisoned( 0 );
				else if( p->GetPoisoned() == 3 && x < 21 ) 
					p->SetPoisoned( 0 );
				else if( p->GetPoisoned() == 4 && x < 6 )  
					p->SetPoisoned( 0 );
				break;
			case 2:
				x = RandomNum( 1, 100 );
				if( p->GetPoisoned() == 1 ) 
					p->SetPoisoned( 0 );
				else if( p->GetPoisoned() == 2 && x < 81 ) 
					p->SetPoisoned( 0 );
				else if( p->GetPoisoned() == 3 && x < 41 ) 
					p->SetPoisoned( 0 );
				else if( p->GetPoisoned() == 4 && x < 21 )  
					p->SetPoisoned( 0 );
				break;
			case 3:
				x = RandomNum( 1, 100 );
				if( p->GetPoisoned() == 1 ) 
					p->SetPoisoned( 0 );
				else if( p->GetPoisoned() == 2 ) 
					p->SetPoisoned( 0 );
				else if( p->GetPoisoned() == 3 && x < 81 ) 
					p->SetPoisoned( 0 );
				else if( p->GetPoisoned() == 4 && x < 61 ) 
					p->SetPoisoned( 0 );
				break;
			default:
				Console.Error( 2, " Fallout of switch statement without default. uox3.cpp, usepotion()" );
				return;
			}
			
			if( p->GetPoisoned() ) 
				sysmessage( mSock, 1345 ); 
			else
			{
				staticeffect( p, 0x373A, 0, 15 );
				soundeffect( p, 0x01E0 ); 
				sysmessage( mSock, 1346 );
			} 
		}
		p->SendToSocket( mSock, true, p );
		break;
	case 3: // Explosion Potion
		if( region[p->GetRegion()]->IsGuarded() )
		{
			sysmessage( mSock, 1347 );
			return;
		}
		mSock->AddID( i->GetSerial() );
		sysmessage( mSock, 1348 );
		tempeffect( p, p, 16, 0, 1, 3 );
		tempeffect( p, p, 16, 0, 2, 2 );
		tempeffect( p, p, 16, 0, 3, 1 );
		tempeffect( p, i, 17, 0, 4, 0 );
		target( mSock, 0, 1, 0, 207, "" );
		return;
	case 4: // Heal Potion
		switch( i->GetMoreZ() )
		{
		case 1:
			p->SetHP( min( (SI16)(p->GetHP() + 5 + RandomNum( 1, 5 ) + p->GetSkill( 17 ) / 100 ), p->GetMaxHP() ) );
			sysmessage( mSock, 1349 );
			break;
		case 2:
			p->SetHP( min( (SI16)(p->GetHP() + 15 + RandomNum( 1, 10 ) + p->GetSkill( 17 ) / 50 ), p->GetMaxHP() ) );
			sysmessage( mSock, 1350 );
			break;
		case 3:
			p->SetHP( min( (SI16)(p->GetHP() + 20 + RandomNum( 1, 20 ) + p->GetSkill( 17 ) / 40 ), p->GetMaxHP() ) );
			sysmessage( mSock, 1351 );
			break;
		default:
			Console.Error( 2, " Fallout of switch statement without default. uox3.cpp, usepotion()" );
			return;
		}
		if( mSock != NULL ) 
			updateStats( p, 0 );
		staticeffect( p, 0x376A, 0x09, 0x06); // Sparkle effect
		soundeffect( p, 0x01F2 ); //Healing Sound - SpaceDog
		break;
	case 5: // Night Sight Potion
		//{
		staticeffect( p, 0x376A, 0x09, 0x06 );
		tempeffect( p, p, 2, 0, 0, 0 );
		soundeffect( p, 0x01E3 );
		break;
		//}
	case 6: // Poison Potion
		if( p->GetPoisoned() < (SI08)i->GetMoreZ() ) 
			p->SetPoisoned( (SI08)i->GetMoreZ() );
		if( i->GetMoreZ() > 4 ) 
			i->SetMoreZ( 4 );
		p->SetPoisonWearOffTime( BuildTimeValue( (R32)cwmWorldState->ServerData()->GetSystemTimerStatus( POISON ) ) );
		p->SendToSocket( mSock, true, p );
		soundeffect( p, 0x0246 );
		sysmessage( mSock, 1352 );
		break;
	case 7: // Refresh Potion
		switch( i->GetMoreZ() )
		{
		case 1:
			p->SetStamina( min( (SI16)(p->GetStamina() + 20 + RandomNum( 1, 10 )), p->GetMaxStam() ) );
			sysmessage( mSock, 1353 );
			break;
		case 2:
			p->SetStamina( min( (SI16)(p->GetStamina() + 40 + RandomNum( 1, 30 )), p->GetMaxStam() ) );
			sysmessage( mSock, 1354 );
			break;
		default:
			Console.Error( 2, " Fallout of switch statement without default. uox3.cpp, usepotion()" );
			return;
		}
		if( mSock != NULL ) 
			updateStats( p, 2 );
		staticeffect( p, 0x376A, 0x09, 0x06); // Sparkle effect
		soundeffect( p, 0x01F2 ); //Healing Sound
		break;
	case 8: // Strength Potion
		staticeffect( p, 0x373A, 0, 15 );
		switch( i->GetMoreZ() )
		{
		case 1:
			tempeffect( p, p, 8, (UI16)( 5 + RandomNum( 1, 10 ) ), 0, 0);
			sysmessage( mSock, 1355 );
			break;
		case 2:
			tempeffect( p, p, 8, (UI16)( 10 + RandomNum( 1, 20 ) ), 0, 0);
			sysmessage( mSock, 1356 );
			break;
		default:
			Console.Error( 2, " Fallout of switch statement without default. uox3.cpp, usepotion()" );
			return;
		}
		soundeffect( p, 0x01EE );     
		break;
	case 9: // Mana Potion
		switch( i->GetMoreZ() )
		{
		case 1:
			p->SetMana( min( (SI16)(p->GetMana() + 10 + i->GetMoreX()/100), p->GetMaxMana() ) );
			break;
		case 2:
			p->SetMana( min( (SI16)(p->GetMana() + 20 + i->GetMoreX()/50), p->GetMaxMana() ) );
			break;
		default:
			Console.Error( 2, " Fallout of switch statement without default. uox3.cpp, usepotion()" );
			return;
		}
		if( mSock != NULL ) 
			updateStats( p, 1 );
		staticeffect( p, 0x376A, 0x09, 0x06); // Sparkle effect
		soundeffect( p, 0x01E7); //agility sound - SpaceDog
		break;
	default:
		Console.Error( 2, " Fallout of switch statement without default. uox3.cpp, usepotion()" );
		return;
	}
	soundeffect( p, 0x0030 );
	if( p->GetID( 1 ) >= 1 && p->GetID( 2 )>90 && !p->IsOnHorse() ) 
		npcAction( p, 0x22);
	decItemAmount( i );
	CItem *bPotion = Items->SpawnItem( mSock, p, 1, "#", true, 0x0F0E, 0, true, false );
	if( bPotion != NULL )
	{
		if( bPotion->GetCont() == INVALIDSERIAL )
			bPotion->SetLocation( p );
		bPotion->SetDecayable( true );
		RefreshItem( bPotion );
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void loadSpawnRegions( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Loads spawning regions
//o---------------------------------------------------------------------------o
void loadSpawnRegions( void )
{
	UI32 i = 0;

	spawnregion[i] = new cSpawnRegion( i );
	
	i++;
	totalspawnregions = 0;
	
	ScriptSection *toScan = NULL;
	ScpList *tScn = FileLookup->GetFiles( spawn_def );
	if( tScn == NULL )
		return;
	for( UI32 iCtr = 0; iCtr < tScn->size(); iCtr++ )
	{
		Script *spnScp = (*tScn)[iCtr];
		if( spnScp == NULL )
			continue;
		for( toScan = spnScp->FirstEntry(); toScan != NULL; toScan = spnScp->NextEntry() )
		{
			if( toScan == NULL )
				continue;
			if( !strncmp( "REGIONSPAWN", spnScp->EntryName(), 11 ) ) // Is it a region spawn entry?
			{
				spawnregion[i] = new cSpawnRegion( i );
				spawnregion[i]->Load( toScan );
				i++;
			}
		}
	}
	totalspawnregions = i;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void loadpredefspawnregion( SI32 r, char *name )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Load scripted spawn regions
//o---------------------------------------------------------------------------o
void loadPreDefSpawnRegion( SI32 r, std::string name )
{
	char sect[512];
	sprintf( sect, "PREDEFINED_SPAWN %s", name.c_str() );
	ScriptSection *predefSpawn = FileLookup->FindEntry( sect, spawn_def );
	if( predefSpawn == NULL )
	{
		Console << "WARNING: Undefined region spawn " << name << ", check your regions.scp and spawn.scp files" << myendl;
		return;
	}

	spawnregion[r]->Load( predefSpawn );
	Console << sect << " loaded into spawn region #" << r << myendl;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void checkRegion( CChar *i )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check what region a character is in
//o---------------------------------------------------------------------------o
void checkRegion( CChar *i )
{
	if( i == NULL )
		return;
	int j;
	UI08 calcReg = calcRegionFromXY( i->GetX(), i->GetY(), i->WorldNumber() );
	if( calcReg != i->GetRegion() )
	{
		UI08 iRegion = i->GetRegion();
		cSocket *s = calcSocketObjFromChar( i );
		if( s != NULL )
		{
			if( region[iRegion] != NULL && region[calcReg] != NULL )
			{
				// Drake 08-30-99 If region name are the same, do not display message
				//                for playing music when approaching Tavern
				j = strcmp( region[iRegion]->GetName(), region[calcReg]->GetName() );
				if( j )
				{
					const char *iRegionName = region[iRegion]->GetName();
					//Changed this and four others to compare the first character of the string to '\0' rather than
					//to NULL.
					//-Shadowlord Nov-3-2001
					if( iRegionName != NULL && iRegionName[0] != '\0')
						sysmessage( s, 1358, iRegionName );

					const char *calcRegName = region[calcReg]->GetName();
					if( calcRegName != NULL && calcRegName[0] != '\0' )
						sysmessage( s, 1359, calcRegName );
				}
				j = strcmp( region[calcReg]->GetOwner(), region[iRegion]->GetOwner() );
				if( ( region[calcReg]->IsGuarded() && j ) || ( !( region[calcReg]->IsGuarded() && region[iRegion]->IsGuarded() ) ) )
				{
					if( region[calcReg]->IsGuarded() )
					{
						const char *calcRegOwner = region[calcReg]->GetOwner();
						if( calcRegOwner != NULL )
						{
							if( calcRegOwner[0] == '\0' )
								sysmessage( s, 1360 );
							else
								sysmessage( s, 1361, calcRegOwner );
						}
					} 
					else
					{
						const char *iRegionOwner = region[iRegion]->GetOwner();
						if( iRegionOwner != NULL )
						{
							if( iRegionOwner[0] == '\0' )
								sysmessage( s, 1362 );
							else
								sysmessage( s, 1363, iRegionOwner );
						}
					}
				}
				if( region[calcReg]->GetAppearance() != region[iRegion]->GetAppearance() )	 // if the regions look different
					SendWorldChange( (WorldType)region[calcReg]->GetAppearance(), s );
				if( calcReg == i->GetTown() )	// enter our home town
				{
					sysmessage( s, 1364 );
					CItem *packItem = getPack( i );
					if( packItem != NULL )
					{
						for( CItem *toScan = packItem->FirstItemObj(); !packItem->FinishedItems(); toScan = packItem->NextItemObj() )
						{
							if( toScan != NULL )
							{
								if( toScan->GetType() == 35 )
								{
									UI08 targRegion = (UI08)toScan->GetMoreX();
									sysmessage( s, 1365, region[targRegion]->GetName() );
									region[targRegion]->DoDamage( region[targRegion]->GetHealth() );	// finish it off
									region[targRegion]->Possess( calcReg );
									i->SetFame( (SI16)( i->GetFame() + i->GetFame() / 5 ) );	// 20% fame boost
									break;
								}
							}
						}
					}
				}
			}
		}
		UI16 leaveScript = i->GetScriptTrigger();
		cScript *tScript = Trigger->GetScript( leaveScript );
		if( tScript != NULL )
		{
			tScript->OnLeaveRegion( i, i->GetRegion() );
			tScript->OnEnterRegion( i, calcReg );
		}
		i->SetRegion( calcReg );
		if( s != NULL ) 
		{
			dosocketmidi( s );
			doLight( s, (SI08)cwmWorldState->ServerData()->GetWorldLightCurrentLevel() );	// force it to update the light level straight away
			Weather->DoPlayerStuff( i );	// force a weather update too
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void dosocketmidi( cSocket *s )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Send midi to client
//o---------------------------------------------------------------------------o
void dosocketmidi( cSocket *s )
{
	int i = 0;
	char midiarray[50];
	char sect[512];

	CChar *mChar = s->CurrcharObj();
	int midiList = region[mChar->GetRegion()]->GetMidiList();
	
	if( midiList != 0 )
		return;

	if( mChar->IsAtWar() )
		strcpy( sect, "MIDILIST COMBAT" );
	else
		sprintf(sect, "MIDILIST %i", midiList );

	ScriptSection *MidiList = FileLookup->FindEntry( sect, regions_def );
	if( MidiList == NULL )
		return;
	const char *data = NULL;
	for( const char *tag = MidiList->First(); !MidiList->AtEnd(); tag = MidiList->Next() )
	{
		data = MidiList->GrabData();
		if( !strcmp( "MIDI", tag ) )
			midiarray[i++] = (SI08)makeNum( data );
	}
	if( i != 0 )
	{
		i = rand()%(i);
		playMidi( s, midiarray[i] );
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void respawnnow( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	NPC / Item spawning stuff
//o---------------------------------------------------------------------------o
void respawnnow( void )
{
	for( UI32 region = 1; region < totalspawnregions; region++ )
	{
		if( spawnregion[region] != NULL )
			spawnregion[region]->doRegionSpawn();
	}

	bool k = false;
	UI32 ci;
	ITEM j;
	for( ITEM i = 0; i < itemcount; i++ )  // Item Spawner
	{
		if( items[i].GetType() == 61 )
		{
			k = false;
			HashBucketMulti< ITEM > *hashBucket = nspawnsp.GetBucket( (items[i].GetSerial())%HASHMAX );
			for( ci = 0; ci < hashBucket->NumEntries(); ci++ )
			{
				j = hashBucket->GetEntry( ci );
				if( j != INVALIDSERIAL )
				{
					if( i != j && items[j].GetX() == items[i].GetX() && items[j].GetY() == items[i].GetY() && items[j].GetZ() == items[i].GetZ() )
					{
						if( items[i].GetSerial() == items[j].GetSpawn() )
						{
							k = true;
							break;
						}
					}
				}
			}
			if( !k )
			{
				const char *nDesc = items[i].GetDesc();
				if( nDesc[0] != 0 )	// not NULL terminated, so we can do something!
				{
					Items->AddRespawnItem( &items[i], nDesc, true, items[i].isSpawnerList() );
				}
				else if( items[i].GetMoreX() != 0 )
				{
					Items->AddRespawnItem( &items[i], items[i].GetMoreX(), false );
				}
				items[i].SetGateTime( 0 );
			}
		}
		
		if( items[i].GetType() == 62 || items[i].GetType() == 69 || items[i].GetType() == 125 )  // NPC Spawner
		{
			int amt = 0;
			HashBucketMulti< CHARACTER > *hashBucket = ncspawnsp.GetBucket( (items[i].GetSerial())%HASHMAX );
			for( ci = 0; ci < hashBucket->NumEntries(); ci++ )
			{
				j = hashBucket->GetEntry( ci );
				if( j != INVALIDSERIAL )
				{
					if( items[i].GetSerial() == items[j].GetSpawn() )
						amt++;
				}
			} 
			
			if( amt < items[i].GetAmount() )
			{
				const char *mDesc = items[i].GetDesc();
				if( mDesc[0] != 0 )	// not NULL terminated, so we can do something!
				{
					Npcs->SpawnNPC( &items[i], mDesc, items[i].WorldNumber(), items[i].isSpawnerList() );
				}
				else if( items[i].GetMoreX() != 0 )
				{
					char temp[512];
					sprintf( temp, "%i", items[i].GetMoreX() );
					Npcs->SpawnNPC( &items[i], temp, items[i].WorldNumber(), false );
				}
				items[i].SetGateTime( 0 );
			}
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void loadskills( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Load skills
//o---------------------------------------------------------------------------o
void loadskills( void )
{
	char sect[512];
	ScriptSection *SkillList = NULL;
	const char *tag = NULL;
	const char *data = NULL;
	for( int i = 0; i <= ALLSKILLS + 3; i++)
	{
		skill[i].strength = 0;
		skill[i].dexterity = 0;
		skill[i].intelligence = 0;
		sprintf( sect, "SKILL %i", i );
		SkillList = FileLookup->FindEntry( sect, skills_def );
		if( SkillList != NULL )
		{
			for( tag = SkillList->First(); !SkillList->AtEnd(); tag = SkillList->Next() )
			{
				data = SkillList->GrabData();
				if( !strcmp( "STR", tag ) )
					skill[i].strength = (UI16)makeNum( data );
				else if( !strcmp( "DEX", tag ) )
					skill[i].dexterity = (UI16)makeNum( data );
				else if( !strcmp( "INT", tag ) )
					skill[i].intelligence = (UI16)makeNum( data );
				else if( !strcmp( "SKILLPOINT", tag ) )
				{
					advance_st tempAdvance;
					char temp[256];
					gettokennum( data, 0, temp );
					tempAdvance.base = (UI16)makeNum( temp );
					gettokennum( data, 1, temp );
					tempAdvance.success = (UI16)makeNum( temp );
					gettokennum( data, 2, temp );
					tempAdvance.failure = (UI16)makeNum( temp );
					skill[i].advancement.push_back( tempAdvance );
				}
			}
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	int getStringValue( char *string )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Pulls tokens from a string, if one value returns it, if two
//|					values it finds a random number between the values
//o---------------------------------------------------------------------------o
int getStringValue( const char *string ) 
{
	char temp[256], temp2[256];
	gettokennum( string, 0, temp );
	SI32 lovalue = makeNum( temp );
	gettokennum( string, 1, temp2 );
	SI32 hivalue = makeNum( temp2 );
	
	if( hivalue ) 
		return RandomNum( lovalue, hivalue );
	else 
		return lovalue;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void advanceObj( CChar *s, SI32 x, bool multiUse )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Handle advancement objects (stat / skill gates)
//o---------------------------------------------------------------------------o
void advanceObj( CChar *s, SI32 x, bool multiUse )
{
	char sect[512];
	int val = 0, num;

	CItem *i = NULL;
	CItem *retitem = NULL;
	if( s->GetAdvObj() == 0 || multiUse )
	{
		staticeffect( s, 0x373A, 0, 15);
		soundeffect( s, 0x01E9 );
		s->SetAdvObj( x );
		sprintf( sect, "ADVANCEMENT %i", x );
		ScriptSection *Advancement = FileLookup->FindEntry( sect, advance_def );
		if( Advancement == NULL )
		{
			Console << "ADVANCEMENT OBJECT: Script section not found, Aborting" << myendl;
			s->SetAdvObj( 0 );
			return;
		}
		else
		{
			const char *tag = NULL;
			const char *data = NULL;
			for( tag = Advancement->First(); !Advancement->AtEnd(); tag = Advancement->Next() )
			{
				data = Advancement->GrabData();
				switch( tag[0] )
				{
				case 'a':
				case 'A':
					if( !strcmp( "ALCHEMY", tag ) ) 
						val = ALCHEMY;
					else if( !strcmp( "ANATOMY", tag ) ) 
						val = ANATOMY;
					else if( !strcmp( "ANIMALLORE", tag ) ) 
						val = ANIMALLORE;
					else if( !strcmp( "ARMSLORE", tag ) ) 
						val = ARMSLORE;
					else if( !strcmp( "ARCHERY", tag ) ) 
						val = ARCHERY;
					else if( !strcmp( "ADVOBJ", tag ) )
						s->SetAdvObj( makeNum( data ) );
					break;
					
				case 'b':
				case 'B':
					if( !strcmp( "BEGGING", tag ) ) 
						val = BEGGING;
					else if( !strcmp( "BLACKSMITHING", tag ) ) 
						val = BLACKSMITHING;
					else if( !strcmp( "BOWCRAFT", tag ) ) 
						val = BOWCRAFT;
					break;
					
				case 'c':
				case 'C':
					if( !strcmp( "CAMPING", tag ) ) 
						val = CAMPING;
					else if( !strcmp( "CARPENTRY", tag ) ) 
						val = CARPENTRY;
					else if( !strcmp( "CARTOGRAPHY", tag ) ) 
						val = CARTOGRAPHY;
					else if( !strcmp( "COOKING", tag ) ) 
						val = COOKING;
					break;
					
				case 'd':
				case 'D':
					if( !strcmp( "DEX", tag ) || !strcmp( "DEXTERITY", tag ) )
						s->SetDexterity( (UI16)getStringValue( data ) );
					else if( !strcmp( "DETECTINGHIDDEN", tag ) ) 
						val = DETECTINGHIDDEN;
					else if( !strcmp( "DYEHAIR", tag ) ) 
					{
						CItem *hairobject = FindItemOnLayer( s, 0x0B );
						if( hairobject != NULL )
						{
							num = makeNum( data );
							hairobject->SetColour( (UI16)num );
							RefreshItem( hairobject );
						}
					}
					else if( !strcmp( "DYEBEARD", tag ) ) 
					{
						CItem *beardobject = FindItemOnLayer( s, 0x10 );
						if( beardobject != NULL )
						{
							num = makeNum( data );
							beardobject->SetColour( (UI16)num );
							RefreshItem( beardobject );
						}
					}
					break;
					
				case 'e':
				case 'E':
					if( !strcmp( "ENTICEMENT", tag ) ) 
						val = ENTICEMENT;
					else if( !strcmp( "EVALUATINGINTEL", tag ) ) 
						val = EVALUATINGINTEL;
					break;
					
				case 'f':
				case 'F':
					if( !strcmp( "FAME", tag ) ) 
						s->SetFame( (SI16)makeNum( data ) );
					else if( !strcmp( "FENCING", tag ) ) 
						val = FENCING;
					else if( !strcmp( "FISHING", tag ) ) 
						val = FISHING;
					else if( !strcmp( "FORENSICS", tag ) ) 
						val = FORENSICS;
					break;
					
				case 'h':
				case 'H':
					if( !strcmp( "HEALING", tag ) ) 
						val = HEALING;
					else if( !strcmp( "HERDING", tag ) ) 
						val = HERDING;
					else if( !strcmp( "HIDING", tag ) ) 
						val = HIDING;
					break;
					
				case 'i':
				case 'I':
					if( !strcmp( "INT", tag ) || !strcmp( "INTELLIGENCE", tag ) )
						s->SetIntelligence( (SI16)getStringValue( data ) );
					else if( !strcmp( "ITEMID", tag ) ) 
						val = ITEMID;
					else if( !strcmp( "INSCRIPTION", tag ) ) 
						val = INSCRIPTION;
					else if( !strcmp( "ITEM", tag ) )
					{
						retitem = Items->CreateScriptItem( NULL, data, false, s->WorldNumber() );
						CItem *packnum = getPack( s );
						if( retitem != NULL )
						{
							retitem->SetX( (UI16)( 50 + (RandomNum( 0, 79 )) ) );
							retitem->SetY( (UI16)( 50 + (RandomNum( 0, 79 )) ) ); 
							retitem->SetZ( 9 );
							if( retitem->GetLayer() == 0x0B || retitem->GetLayer() == 0x10 )
							{
								if( !retitem->SetCont( s->GetSerial() ) )
									retitem = NULL;
							}
							else
								retitem->SetCont( packnum->GetSerial() );
							if( retitem != NULL )
								RefreshItem( retitem );
						}
					}
					break;
					
				case 'k':
				case 'K':
					if( !strcmp( "KARMA", tag ) ) 
						s->SetKarma( (SI16)makeNum( data ) );
					else if( !strcmp( "KILLHAIR", tag ) )
					{
						i = FindItemOnLayer( s, 0x0B );
						if( i != NULL )
							Items->DeleItem( i );
					}
					else if( !strcmp( "KILLBEARD", tag ) )
					{
						i = FindItemOnLayer( s, 0x10 );
						if( i != NULL )
							Items->DeleItem( i );

					}
					else if( !strcmp( "KILLPACK", tag ) )
					{
						i = FindItemOnLayer( s, 0x15 );
						if( i != NULL )
							Items->DeleItem( i );

					}
					break;
					
				case 'l':
				case 'L':
					if( !strcmp( "LOCKPICKING", tag ) ) 
						val = LOCKPICKING;
					else if( !strcmp( "LUMBERJACKING", tag ) ) 
						val = LUMBERJACKING;
					break;
					
				case 'm':
				case 'M':
					if( !strcmp( "MAGERY", tag ) ) 
						val = MAGERY;
					else if( !strcmp( "MAGICRESISTANCE", tag ) ) 
						val = MAGICRESISTANCE;
					else if( !strcmp( "MACEFIGHTING", tag ) ) 
						val = MACEFIGHTING;
					else if( !strcmp( "MEDITATION", tag ) ) 
						val = MEDITATION;
					else if( !strcmp( "MINING", tag ) ) 
						val = MINING;
					else if( !strcmp( "MUSICIANSHIP", tag ) ) 
						val = MUSICIANSHIP;
					break;
					
				case 'p':
				case 'P':
					if( !strcmp( "PARRYING", tag ) ) 
						val = PARRYING;
					else if( !strcmp( "PEACEMAKING", tag ) ) 
						val = PEACEMAKING;
					else if( !strcmp( "POISONING", tag ) ) 
						val = POISONING;
					else if( !strcmp( "PROVOCATION", tag ) ) 
						val = PROVOCATION;
					else if( !strcmp( "POLY", tag ) )
					{
						num = makeNum( data );
						s->SetID( (UI16)num );
						s->SetxID( (UI16)num );
						s->SetOrgID( (UI16)num );
					}
					break;
				case 'r':
				case 'R':
					if( !strcmp( "REMOVETRAPS", tag ) ) 
						val = REMOVETRAPS;
					break;
					
				case 's':
				case 'S':
					if( !strcmp( "STR", tag ) || !strcmp( "STRENGTH", tag ) )
						s->SetStrength( (SI16)getStringValue( data ) );
					else if( !strncmp( "SKILL", tag, 5 ) )	// get number code
						val = makeNum( tag + 5 );
					else if( !strcmp( tag, "SKIN" ) )
					{
						num = makeNum( data );
						s->SetSkin( (UI16)num );
						s->SetxSkin( (UI16)num );
					}
					else if( !strcmp( "SNOOPING", tag ) ) 
						val = SNOOPING;
					else if( !strcmp( "SPIRITSPEAK", tag ) ) 
						val = SPIRITSPEAK;
					else if( !strcmp( "STEALING", tag ) ) 
						val = STEALING;
					else if( !strcmp( "STEALTH", tag ) ) 
						val = STEALTH;
					else if( !strcmp( "SWORDSMANSHIP", tag ) ) 
						val = SWORDSMANSHIP;
					break;
					
				case 't':
				case 'T':
					if( !strcmp( "TACTICS", tag ) ) 
						val = TACTICS;
					else if( !strcmp( "TAILORING", tag ) ) 
						val = TAILORING;
					else if( !strcmp( "TAMING", tag ) ) 
						val = TAMING;
					else if( !strcmp( "TASTEID", tag ) ) 
						val = TASTEID;
					else if( !strcmp( "TINKERING", tag ) ) 
						val = TINKERING;
					else if( !strcmp( "TRACKING", tag ) ) 
						val = TRACKING;
					break;
					
				case 'v':
				case 'V':
					if( !strcmp( "VETERINARY", tag ) ) 
						val = VETERINARY;
					break;

				case 'w':
				case 'W':
					if( !strcmp( "WRESTLING", tag ) ) 
						val = WRESTLING;
					break;
				}
				
				if( val > 0 )
				{
					s->SetBaseSkill( (UI16)getStringValue( data ), (UI08)val );
					val = 0;	// reset for next time through
				}
			}
			s->Teleport();	// don't need to teleport.  Why?? because our location never changes!
//			s->Update( s );
			
		}
	}
	else 
		sysmessage( calcSocketObjFromChar( s ), 1366 );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void DumpCreatures( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Dumps creatures to npc.dat
//o---------------------------------------------------------------------------o
void DumpCreatures( void )
{
	std::ofstream toWrite( "npc.dat" );
	if( !toWrite.is_open() )
		return;
	for( int i = 0; i < 2048; i++ )
	{
		toWrite << "[CREATURE " << i << "]" << std::endl << "{" << std::endl;
		toWrite << "BASESOUND=" << creatures[i].BaseSound() << std::endl;
		toWrite << "ICON=" << (int)creatures[i].Icon() << std::endl;
		toWrite << "SOUNDFLAG=" << (SI32)creatures[i].SoundFlag() << std::endl;
		if( creatures[i].CanFly() )
			toWrite << "FLIES" << std::endl;
		if( creatures[i].AntiBlink() )
			toWrite << "ANTIBLINK" << std::endl;
		if( creatures[i].IsAnimal() )
			toWrite << "ANIMAL" << std::endl;
		if( creatures[i].IsWater() )
			toWrite << "WATERCREATURE" << std::endl;
		toWrite << "}" << std::endl << std::endl;
	}
	toWrite.close();
}

//o---------------------------------------------------------------------------o
//|	Function	-	void LoadCreatures( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Loads creatures from npc.dat
//o---------------------------------------------------------------------------o
void LoadCreatures( void )
{
	FILE *npcExists = fopen( "npc.dat", "r" );
	if( npcExists == NULL )
	{
		Console << "Loading creatures from internal memory...";
		init_creatures();
	}
	else
	{
		fclose( npcExists );
		Script *npcData = new Script( "npc.dat", NUM_DEFS );
		Console << "Loading creatures from file...";
		memset( creatures, 0, sizeof( creat_st ) * 2048 );	// init all creatures to 0
		char sect[128];
		const char *tag = NULL;
		const char *data = NULL;
		for( int iCounter = 0; iCounter < 2048; iCounter++ )
		{
			sprintf( sect, "CREATURE %i", iCounter );
			ScriptSection *creatureData = npcData->FindEntry( sect );
			if( creatureData != NULL )
			{
				for( tag = creatureData->First(); !creatureData->AtEnd(); tag = creatureData->Next() )
				{
					if( tag == NULL )
						continue;
					data = creatureData->GrabData();
					switch( tag[0] )
					{
					case 'A':
						if( !strcmp( "ANTIBLINK", tag ) )
							creatures[iCounter].AntiBlink( true );
						else if( !strcmp( "ANIMAL", tag ) )
							creatures[iCounter].IsAnimal( true );
						break;
					case 'B':
						if( !strcmp( "BASESOUND", tag ) )
							creatures[iCounter].BaseSound( makeNum( data ) );
						break;
					case 'F':
						if( !strcmp( "FLIES", tag ) )
							creatures[iCounter].CanFly( true );
						break;
					case 'I':
						if( !strcmp( "ICON", tag ) )
							creatures[iCounter].Icon( makeNum( data ) );
						break;
					case 'S':
						if( !strcmp( "SOUNDFLAG", tag ) )
							creatures[iCounter].SoundFlag( (UI08)makeNum( data ) );
						break;
					case 'W':
						if( !strcmp( "WATERCREATURE", tag ) )
							creatures[iCounter].IsWater( true );
						break;
					}
				}
			}
		}
		delete npcData;
	}
	Console.PrintDone();
}

//o---------------------------------------------------------------------------o
//|	Function	-	void init_creatures( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Initialize NPC's, assigning basesound, soundflag, and
//|					who_am_i flag
//o---------------------------------------------------------------------------o
void init_creatures(void)
{
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// soundflags  0: normal, 5 sounds (attack-started,idle, attack, defence, dying, see uox.h)
    //             1: birds .. only one "bird-shape" and zillions of sounds ...
	//             2: only 3 sounds ->  (attack,defence,dying)    
	//             3: only 4 sounds ->   (attack-started,attack,defnce,dying)
	//             4: only 1 sound !!
	//
	// who_am_i bit # 1 creature can fly (must have the animations, so better not change)
	//              # 2 anti-blink: these creatures don't have animation #4, if not set creature will randomly disappear in battle
	//                              if you find a creature that blinks while fighting, set that bit
	//              # 3 animal-bit (currently not used/set)
	//              # 4 water creatures (currently not used/set)
	// icon: used for tracking, to set the appropriate icon
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	creatures[0x01].BaseSound( 0x01AB );                            // Ogre
	creatures[0x01].Icon( 8415 );
    creatures[0x02].BaseSound( 0x016F );                            // Ettin 				
	creatures[0x02].Icon( 8408 );
    creatures[0x03].BaseSound( 0x01D7 );                            // Zombie
	creatures[0x03].Icon( 8428 );
    creatures[0x04].BaseSound( 0x0174 );                            // Gargoyle
	creatures[0x04].CanFly( true ); // set can_fly_bit
	creatures[0x04].Icon( 8409 );
    creatures[0x05].BaseSound( 0x008F );                            // Eagle
	creatures[0x05].CanFly( true ); // set can_fly bit
	creatures[0x05].AntiBlink( true ); // set anti blink bit
	creatures[0x05].IsAnimal( true ); // set anti blink bit
	creatures[0x05].Icon( 8434 );
    creatures[0x06].BaseSound( 0x007D );                            // Bird
	creatures[0x06].CanFly( true ); //set fly bit
	creatures[0x06].IsAnimal( true );
	creatures[0x06].SoundFlag( 1 ); // birds need special treatment cause there are about 20 bird-sounds
	creatures[0x06].Icon( 8430 );
	creatures[0x07].BaseSound( 0x01B0 );                            // Orc	                      
	creatures[0x07].Icon( 8416 );
    creatures[0x08].BaseSound( 0x01BA );                            // corpser
	creatures[0x08].SoundFlag( 3 );
	creatures[0x08].Icon( 8402 );
	creatures[0x09].BaseSound( 0x0165 );                            // daemon 
	creatures[0x09].CanFly( true );
	creatures[0x09].Icon( 8403 );
	creatures[0x0a].BaseSound( 0x0165 );                            // daemon 2
	creatures[0x0a].CanFly( true );
	creatures[0x0a].Icon( 8403 );
	
	
	creatures[0x0c].BaseSound( 362 );                               // Green dragon
	creatures[0x0c].CanFly( true ); // flying creature
	creatures[0x0c].Icon( 8406 );
	creatures[0x0d].BaseSound( 263 );                               // air-ele
	creatures[0x0d].Icon( 8429 );
	creatures[0x0e].BaseSound( 268 );                               // earth-ele		
	creatures[0x0e].Icon( 8407 );
	creatures[0x0f].BaseSound( 273 );                               // fire-ele
	creatures[0x0f].Icon( 8435 );
	creatures[0x10].BaseSound( 0x0116 ); 	                         // water ele
	creatures[0x10].Icon( 8459 );
    creatures[0x11].BaseSound( 0x01B0 );                            // Orc	2
	creatures[0x11].Icon( 8416 );
    creatures[0x12].BaseSound( 0x016F );                           // Ettin 2
	creatures[0x12].Icon( 8408 );
	
	
	creatures[0x15].BaseSound( 219 );                               // Giant snake
	creatures[0x15].Icon( 8446 );
	creatures[0x16].BaseSound( 377 );                               // gazer
	creatures[0x16].Icon( 8426 );
	
	creatures[0x18].BaseSound( 412 );                               // liche
	creatures[0x18].Icon( 8440 );									// counldnt find a better one :(
	
	creatures[0x1a].BaseSound( 382 );                               // ghost 1
	creatures[0x1a].Icon( 8457 );
	
	creatures[0x1c].BaseSound( 387 );                               // giant spider
    creatures[0x1c].Icon( 8445 );
	creatures[0x1d].BaseSound( 158 );                               // gorialla
	creatures[0x1d].Icon( 8437 );
	creatures[0x1e].BaseSound( 402 );                               // harpy			
	creatures[0x1e].Icon( 8412 );
	creatures[0x1f].BaseSound( 407 );                               // headless
	creatures[0x1f].Icon( 8458 );
	
	creatures[0x21].BaseSound( 417 );                               // lizardman
	creatures[0x23].BaseSound( 417 );                            
	creatures[0x24].BaseSound( 417 );        
	creatures[0x25].BaseSound( 417 );
	creatures[0x26].BaseSound( 417 );
	creatures[0x21].Icon( 8414 );
	creatures[0x23].Icon( 8414 );
	creatures[0x24].Icon( 8414 );
	creatures[0x25].Icon( 8414 );
	creatures[0x26].Icon( 8414 );
	
	
	creatures[0x27].BaseSound( 422 );                               // mongbat
	creatures[0x27].CanFly( true ); // yes, they can fly
	creatures[0x27].Icon( 8441 );
	
	creatures[0x29].BaseSound( 0x01B0 );                            // orc 3
	creatures[0x29].Icon( 8416 );

	creatures[0x2A].BaseSound( 437 );                               // ratman
	creatures[0x2C].BaseSound( 437 );                            
	creatures[0x2D].BaseSound( 437 );                            
	creatures[0x2A].Icon( 8419 );
	creatures[0x2C].Icon( 8419 );
	creatures[0x2D].Icon( 8419 );
	
	creatures[0x2F].BaseSound( 0x01BA );                            // Reaper			
	creatures[0x2F].Icon( 8442 );
	creatures[0x30].BaseSound( 397 );                               // giant scorprion	
	creatures[0x30].Icon( 8420 );
	
	creatures[0x32].BaseSound( 452 );                               // skeleton 2
	creatures[0x32].Icon( 8423 );
	creatures[0x33].BaseSound( 456 );                               // slime	
    creatures[0x33].Icon( 8424 );
	creatures[0x34].BaseSound( 219 );                               // Snake
	creatures[0x34].Icon( 8444 );
	creatures[0x34].IsAnimal( true ); // set anti blink bit
    creatures[0x35].BaseSound( 461 );                               // troll 				
    creatures[0x35].Icon( 8425 );
    creatures[0x36].BaseSound( 461 );                               // troll 2
	creatures[0x36].Icon( 8425 );
    creatures[0x37].BaseSound( 461 );                               // troll 3
	creatures[0x37].Icon( 8425 );
    creatures[0x38].BaseSound( 452 );                               // skeleton 3
	creatures[0x38].Icon( 8423 );
    creatures[0x39].BaseSound( 452 );                               // skeleton 4
	creatures[0x39].Icon( 8423 );
	creatures[0x3A].BaseSound( 466 );                               // wisp	                      
	creatures[0x3A].Icon( 8448 );
    creatures[0x3B].BaseSound( 362 );                               // red dragon
	creatures[0x3B].CanFly( true ); // set fly bit
	creatures[0x3C].BaseSound( 362 );                               // smaller red dragon
	creatures[0x3C].CanFly( true );
	creatures[0x3D].BaseSound( 362 );                               // smaller green dragon
	creatures[0x3D].CanFly( true );
	creatures[0x3B].Icon( 8406 );
	creatures[0x3C].Icon( 8406 );
	creatures[0x3D].Icon( 8406 );
	
	
	creatures[0x96].BaseSound( 477 );                               // sea serpant
	creatures[0x96].SoundFlag( 3 );
	creatures[0x96].Icon( 8446 ); // normal serpant icon
	creatures[0x97].BaseSound( 138 );                               // dolphin
	creatures[0x97].Icon( 8433 ); // correct icon ???
	
	creatures[0xC8].BaseSound( 168 );                               // white horse		
    creatures[0xC8].Icon( 8479 );
	creatures[0xC8].IsAnimal( true ); // set anti blink bit
	creatures[0xC9].BaseSound( 105 );                               // cat
	creatures[0xC9].AntiBlink( true ); // set blink flag
	creatures[0xC9].IsAnimal( true );
    creatures[0xC9].Icon( 8475 );
	creatures[0xCA].BaseSound( 90 );   	                         // alligator
    creatures[0xCA].Icon( 8410 );
	creatures[0xCA].IsAnimal( true );
    creatures[0xCB].BaseSound( 196 );                               // small pig
    creatures[0xCB].Icon( 8449 );
	creatures[0xCB].IsAnimal( true );
    creatures[0xCC].BaseSound( 168 );                               // brown horse
    creatures[0xCC].Icon( 8481 );
	creatures[0xCC].IsAnimal( true );
	creatures[0xCD].BaseSound( 201 );                               // rabbit
	creatures[0xCD].SoundFlag( 2 );                                 // rabbits only have 3 sounds, thus need special treatment
	creatures[0xCD].Icon( 8485 );
	creatures[0xCD].IsAnimal( true );
	
	creatures[0xCF].BaseSound( 214 );                               // wooly sheep
	creatures[0xCF].Icon( 8427 );
	creatures[0xCF].IsAnimal( true );
	
	creatures[0xD0].BaseSound( 110 );                               // chicken
	creatures[0xD0].Icon( 8401 );
	creatures[0xD0].IsAnimal( true );
	creatures[0xD1].BaseSound( 153 );                               // goat
	creatures[0xD1].Icon( 8422 ); // theres no goat icon, so i took a (differnt) sheep
	creatures[0xD1].IsAnimal( true );
	
	creatures[0xD3].BaseSound( 95 );                                // brown bear
    creatures[0xD3].Icon( 8399 );
	creatures[0xD3].IsAnimal( true );
	creatures[0xD4].BaseSound( 95 );                                // grizzly bear
	creatures[0xD4].Icon( 8411 );
	creatures[0xD4].IsAnimal( true );
	creatures[0xD5].BaseSound( 95 );                                // polar bear	
	creatures[0xD5].Icon( 8417 );
	creatures[0xD5].IsAnimal( true );
	creatures[0xD6].BaseSound( 186 );                               // panther
	creatures[0xD6].AntiBlink( true );
    creatures[0xD6].Icon( 8473 );
	creatures[0xD7].BaseSound( 392 );                               // giant rat
	creatures[0xD7].Icon( 8400 );
	creatures[0xD8].BaseSound( 120 );                               // cow
    creatures[0xD8].Icon( 8432 );
	creatures[0xD8].IsAnimal( true );
	creatures[0xD9].BaseSound( 133 );                               // dog
	creatures[0xD9].Icon( 8405 );
	creatures[0xD9].IsAnimal( true );
	
	creatures[0xDC].BaseSound( 183 );                               // llama
	creatures[0xDC].SoundFlag( 2 );
	creatures[0xDC].Icon( 8438 );
	creatures[0xDC].IsAnimal( true );
	
	creatures[0xDD].BaseSound( 224 );                               // walrus
	creatures[0xDD].Icon( 8447 );
	creatures[0xDD].IsAnimal( true );
	
	creatures[0xDF].BaseSound( 216 );                               // lamb/shorn sheep
	creatures[0xDF].SoundFlag( 2 );
	creatures[0xDF].Icon( 8422 );
	creatures[0xDF].IsAnimal( true );
	creatures[0xE1].BaseSound( 229 );                               // jackal
	creatures[0xE1].SoundFlag( 2 );
	creatures[0xE1].AntiBlink( true ); // set anti blink bit
    creatures[0xE1].Icon( 8426 );
	creatures[0xE2].BaseSound( 168 );                               // yet another horse
	creatures[0xE2].Icon( 8484 );
	creatures[0xE2].IsAnimal( true );
	
	creatures[0xE4].BaseSound( 168 );                               // horse ...
	creatures[0xE4].Icon( 8480 );
	creatures[0xE4].IsAnimal( true );
	
	creatures[0xE7].BaseSound( 120 );                               // brown cow
	creatures[0xE7].AntiBlink( true );
	creatures[0xE7].Icon( 8432 );
	creatures[0xE7].IsAnimal( true );

	creatures[0xE8].BaseSound( 100 );                               // bull
	creatures[0xE8].AntiBlink( true );
	creatures[0xE8].Icon( 8431 );
	creatures[0xE8].IsAnimal( true );

	creatures[0xE9].BaseSound( 120 );                               // b/w cow
	creatures[0xE9].AntiBlink( true );
	creatures[0xE9].Icon( 8451 );
	creatures[0xE9].IsAnimal( true );

	creatures[0xEA].BaseSound( 130 );                               // deer
	creatures[0xEA].SoundFlag( 2 );
	creatures[0xEA].Icon( 8404 );
	creatures[0xEA].IsAnimal( true );
	
	creatures[0xED].BaseSound( 130 );                               // small deer
	creatures[0xED].SoundFlag( 2 );
    creatures[0xED].Icon( 8404 );
	creatures[0xED].IsAnimal( true );

	creatures[0xEE].BaseSound( 204 );                               // rat
	creatures[0xEE].Icon( 8483 );
	creatures[0xEE].IsAnimal( true );
	
	creatures[0x122].BaseSound( 196 );                                // large pig
    creatures[0x122].Icon( 8449 );
	creatures[0x122].IsAnimal( true );

	creatures[0x123].BaseSound( 168 );                                // pack horse
	creatures[0x123].Icon( 8486 );
	creatures[0x123].IsAnimal( true );

	creatures[0x124].BaseSound( 183 );                                // pack llama	
	creatures[0x124].SoundFlag( 2 );
	creatures[0x124].Icon( 8487 );
	creatures[0x124].IsAnimal( true );
	
	creatures[0x23D].BaseSound( 263 );                                 // e-vortex
	creatures[0x23E].BaseSound( 512 );                                 // blade spritit
	creatures[0x23E].SoundFlag( 4 );
	
	creatures[0x600].BaseSound( 115 );                                // cougar;
	creatures[0x600].Icon( 8473 );
	creatures[0x600].IsAnimal( true );
	
	creatures[0x190].Icon( 8454 );
	creatures[0x191].Icon( 8455 );
	
}

//o---------------------------------------------------------------------------o
//|	Function	-	void MonsterGate( CChar *s, SI32 x )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Handle monster gates (polymorphs players into monster bodies)
//o---------------------------------------------------------------------------o
void MonsterGate( CChar *s, SI32 x )
{
	CItem *mypack = NULL, *retitem = NULL;
	char sect[512];
	char rndlootlist[20];
	
	if( s->IsNpc() ) 
		return;
	
	cSocket *mSock = calcSocketObjFromChar( s );

	sprintf( sect, "%i", x );
	ScriptSection *Monster = FileLookup->FindEntry( sect, npc_def );
	if( Monster == NULL )
		return;
	
	s->SetTitle( "\0" );

	CItem *n;
	for( CItem *z = s->FirstItem(); !s->FinishedItems(); z = s->NextItem() )
	{
		if( z != NULL )
		{
			if( z->isFree() )
				continue;
			if( z->GetLayer() != 0x15 && z->GetLayer() != 0x1D && z->GetLayer() != 0x10 && z->GetLayer() != 0x0B )
			{
				if( mypack == NULL )
					mypack = getPack( s );
				if( mypack == NULL )
				{
					n = Items->SpawnItem( mSock, s, 1, "#", false, 0x0E75, 0, false, false );
					s->SetPackItem( n );
					if( n == NULL ) 
						return;
					n->SetLayer( 0x15 );
					if( n->SetCont( s->GetSerial() ) )
					{
						n->SetType( 1 );
						n->SetDye( true );
						mypack = n;
						retitem = n;
					}
				}
				z->SetX( (SI16)( ( RandomNum( 0, 79 ) ) + 50 ) );
				z->SetY( (SI16)( ( RandomNum( 0, 79 ) ) + 50 ) );
				z->SetZ( 9 );
				z->SetCont( mypack->GetSerial() );
				
				RefreshItem( z );
			}
			else if( z->GetLayer() == 0x0B || z->GetLayer() == 0x10 )
				Items->DeleItem( z );
		}
	}
	
	DFNTAGS tag = DFNTAG_COUNTOFTAGS;
	const char *cdata = NULL;
	UI32 ndata = INVALIDSERIAL, odata = INVALIDSERIAL;
	for( tag = Monster->FirstTag(); !Monster->AtEndTags(); tag = Monster->NextTag() )
	{
		cdata = Monster->GrabData( ndata, odata );
		switch( tag )
		{
		case DFNTAG_ALCHEMY:			s->SetBaseSkill( RandomNum( ndata, odata ), ALCHEMY );			break;
		case DFNTAG_ANATOMY:			s->SetBaseSkill( RandomNum( ndata, odata ), ANATOMY );			break;
		case DFNTAG_ANIMALLORE:			s->SetBaseSkill( RandomNum( ndata, odata ), ANIMALLORE );		break;
		case DFNTAG_ARMSLORE:			s->SetBaseSkill( RandomNum( ndata, odata ), ARMSLORE );			break;
		case DFNTAG_ARCHERY:			s->SetBaseSkill( RandomNum( ndata, odata ), ARCHERY );			break;
		case DFNTAG_BEGGING:			s->SetBaseSkill( RandomNum( ndata, odata ), BEGGING );			break;
		case DFNTAG_BLACKSMITHING:		s->SetBaseSkill( RandomNum( ndata, odata ), BLACKSMITHING );	break;
		case DFNTAG_BOWCRAFT:			s->SetBaseSkill( RandomNum( ndata, odata ), BOWCRAFT );			break;
		case DFNTAG_DAMAGE:
		case DFNTAG_ATT:				s->SetLoDamage( ndata );	s->SetHiDamage( odata );			break;
		case DFNTAG_COLOUR:				if( retitem != NULL )
											retitem->SetColour( ndata );
										break;
		case DFNTAG_CAMPING:			s->SetBaseSkill( RandomNum( ndata, odata ), CAMPING );			break;
		case DFNTAG_CARPENTRY:			s->SetBaseSkill( RandomNum( ndata, odata ), CARPENTRY );		break;
		case DFNTAG_CARTOGRAPHY:		s->SetBaseSkill( RandomNum( ndata, odata ), CARTOGRAPHY );		break;
		case DFNTAG_COOKING:			s->SetBaseSkill( RandomNum( ndata, odata ), COOKING );			break;
		case DFNTAG_DEX:				s->SetDexterity( RandomNum( ndata, odata ) );
										s->SetStamina( (SI16)s->GetMaxStam() );
										break;
		case DFNTAG_DEF:				s->SetDef( RandomNum( ndata, odata ) );							break;
		case DFNTAG_DETECTINGHIDDEN:	s->SetBaseSkill( RandomNum( ndata, odata ), DETECTINGHIDDEN );	break;
		case DFNTAG_ENTICEMENT:			s->SetBaseSkill( RandomNum( ndata, odata ), ENTICEMENT );		break;
		case DFNTAG_EVALUATINGINTEL:	s->SetBaseSkill( RandomNum( ndata, odata ), EVALUATINGINTEL );	break;
		case DFNTAG_FAME:				s->SetFame( ndata );											break;
		case DFNTAG_FISHING:			s->SetBaseSkill( RandomNum( ndata, odata ), FISHING );			break;
		case DFNTAG_FORENSICS:			s->SetBaseSkill( RandomNum( ndata, odata ), FORENSICS );		break;
		case DFNTAG_FENCING:			s->SetBaseSkill( RandomNum( ndata, odata ), FENCING );			break;
		case DFNTAG_GOLD:				retitem = n = Items->SpawnItem( mSock, s, 1, "#", true, 0x0EED, 0, true, false );
										if( n == NULL ) 
											break;
										n->SetAmount( (UI32)(ndata + ( rand()%( odata - ndata ) )) );
										break;
		case DFNTAG_HIDAMAGE:			s->SetHiDamage( ndata );										break;
		case DFNTAG_HEALING:			s->SetBaseSkill( RandomNum( ndata, odata ), HEALING );			break;
		case DFNTAG_HERDING:			s->SetBaseSkill( RandomNum( ndata, odata ), HERDING );			break;
		case DFNTAG_HIDING:				s->SetBaseSkill( RandomNum( ndata, odata ), HIDING );			break;
		case DFNTAG_ID:					s->SetID( ndata );
										s->SetxID( ndata );
										s->SetOrgID( odata );
										break;
		case DFNTAG_ITEM:				retitem = Items->CreateScriptItem( NULL, cdata, false, s->WorldNumber() );
										if( retitem != NULL )
										{
											if( !retitem->SetCont( s->GetSerial() ) )
												retitem = NULL;
											else if( retitem->GetLayer() == 0 )
												Console << "Warning: Bad NPC Script " << x << " with problem item " << cdata << " executed!" << myendl;
										}
										break;
		case DFNTAG_INTELLIGENCE:		s->SetIntelligence( RandomNum( ndata, odata ) );
										s->SetMana( s->GetMaxMana() );
										break;
		case DFNTAG_ITEMID:				s->SetBaseSkill( RandomNum( ndata, odata ), ITEMID );			break;
		case DFNTAG_INSCRIPTION:		s->SetBaseSkill( RandomNum( ndata, odata ), INSCRIPTION );		break;
		case DFNTAG_KARMA:				s->SetKarma( ndata );
		case DFNTAG_LOOT:				strcpy( rndlootlist, cdata );
										retitem = Npcs->addRandomLoot( mypack, rndlootlist );
										break;
		case DFNTAG_LODAMAGE:			s->SetLoDamage( ndata );										break;
		case DFNTAG_LOCKPICKING:		s->SetBaseSkill( RandomNum( ndata, odata ), LOCKPICKING );		break;
		case DFNTAG_LUMBERJACKING:		s->SetBaseSkill( RandomNum( ndata, odata ), LUMBERJACKING );	break;
		case DFNTAG_MACEFIGHTING:		s->SetBaseSkill( RandomNum( ndata, odata ), MACEFIGHTING );		break;
		case DFNTAG_MINING:				s->SetBaseSkill( RandomNum( ndata, odata ), MINING );			break;
		case DFNTAG_MEDITATION:			s->SetBaseSkill( RandomNum( ndata, odata ), MEDITATION );		break;
		case DFNTAG_MAGERY:				s->SetBaseSkill( RandomNum( ndata, odata ), MAGERY );			break;
		case DFNTAG_MAGICRESISTANCE:	s->SetBaseSkill( RandomNum( ndata, odata ), MAGICRESISTANCE );	break;
		case DFNTAG_MUSICIANSHIP:		s->SetBaseSkill( RandomNum( ndata, odata ), MUSICIANSHIP );		break;
		case DFNTAG_NAME:				s->SetName( cdata );											break;
		case DFNTAG_NAMELIST:			setRandomName( s, cdata );										break;
		case DFNTAG_PACKITEM:			retitem = Items->CreateScriptItem( NULL, cdata, false, s->WorldNumber() );
										if( retitem != NULL )
										{
											retitem->SetCont( mypack->GetSerial() );
											retitem->SetX( (UI16)( 50 + RandomNum( 0, 79 ) ) );
											retitem->SetY( (UI16)( 50 + RandomNum( 0, 79 ) ) );
											retitem->SetZ( 9 );
										}
										break;
		case DFNTAG_POISON:				s->SetPoison( ndata );											break;
		case DFNTAG_PARRYING:			s->SetBaseSkill( RandomNum( ndata, odata ), PARRYING );			break;
		case DFNTAG_PEACEMAKING:		s->SetBaseSkill( RandomNum( ndata, odata ), PEACEMAKING );		break;
		case DFNTAG_PROVOCATION:		s->SetBaseSkill( RandomNum( ndata, odata ), PROVOCATION );		break;
		case DFNTAG_POISONING:			s->SetBaseSkill( RandomNum( ndata, odata ), POISONING );		break;
		case DFNTAG_REMOVETRAPS:		s->SetBaseSkill( RandomNum( ndata, odata ), REMOVETRAPS );		break;
		case DFNTAG_SKIN:				s->SetSkin( ndata );	s->SetxSkin( ndata );					break;
		case DFNTAG_STRENGTH:			s->SetStrength( RandomNum( ndata, odata ) );
										s->SetHP( s->GetMaxHP() );
										break;
		case DFNTAG_SKILL:				s->SetBaseSkill( odata, ndata );								break;
/*			else if( !strncmp( tag, "SKILL", 5 ) )
			{
				skill = makeNum( &tag[5] );
				s->SetBaseSkill( (UI16)makeNum( data ), (UI08)skill );
			}*/
		case DFNTAG_SNOOPING:			s->SetBaseSkill( RandomNum( ndata, odata ), SNOOPING );			break;
		case DFNTAG_SPIRITSPEAK:		s->SetBaseSkill( RandomNum( ndata, odata ), SPIRITSPEAK );		break;
		case DFNTAG_STEALING:			s->SetBaseSkill( RandomNum( ndata, odata ), STEALING );			break;
		case DFNTAG_SWORDSMANSHIP:		s->SetBaseSkill( RandomNum( ndata, odata ), SWORDSMANSHIP );	break;
		case DFNTAG_STEALTH:			s->SetBaseSkill( RandomNum( ndata, odata ), STEALTH );			break;
		case DFNTAG_TITLE:				s->SetTitle( cdata );											break;
		case DFNTAG_TAILORING:			s->SetBaseSkill( RandomNum( ndata, odata ), TAILORING );		break;
		case DFNTAG_TAMING:				s->SetBaseSkill( RandomNum( ndata, odata ), TAMING );			break;
		case DFNTAG_TASTEID:			s->SetBaseSkill( RandomNum( ndata, odata ), TASTEID );			break;
		case DFNTAG_TINKERING:			s->SetBaseSkill( RandomNum( ndata, odata ), TINKERING );		break;
		case DFNTAG_TRACKING:			s->SetBaseSkill( RandomNum( ndata, odata ), TRACKING );			break;
		case DFNTAG_TACTICS:			s->SetBaseSkill( RandomNum( ndata, odata ), TACTICS );			break;
		case DFNTAG_VETERINARY:			s->SetBaseSkill( RandomNum( ndata, odata ), VETERINARY );		break;
		case DFNTAG_WRESTLING:			s->SetBaseSkill( RandomNum( ndata, odata ), WRESTLING );		break;
		case DFNTAG_ID2:
			s->SetID2(ndata);
			break;
		case DFNTAG_SKIN2:
			s->SetSkin2(ndata);
			break;
		case DFNTAG_STAMINA:
			s->SetStamina(ndata);
			break;
		case DFNTAG_MANA:
			s->SetMana(ndata);
			break;
		case DFNTAG_NOMOVE:
			s->SetNoMove(ndata);
			break;
		case DFNTAG_POISONCHANCE:
			s->SetPoisonChance(ndata);
			break;
		case DFNTAG_POISONSTRENGTH:
			s->SetPoisonStrength(ndata);
			break;
		default:						break;
		}
	}
 
	//Now find real 'skill' based on 'baseskill' (stat modifiers)
	for( int j = 0; j < TRUESKILLS; j++ )
		Skills->updateSkillLevel( s, j );
	s->Teleport();
	staticeffect( s, 0x373A, 0, 15 );
	soundeffect( s, 0x01E9 );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void Karma( CChar *nCharID, CChar *nKilledID, int nKarma )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Handle karma addition/subtraction when character kills
//|					another Character / NPC
//o---------------------------------------------------------------------------o
void Karma( CChar *nCharID, CChar *nKilledID, int nKarma )
{	// nEffect = 1 positive karma effect
	int nCurKarma = 0, nChange = 0;
	bool nEffect = false;
	//
	nCurKarma = nCharID->GetKarma();
	if(nCurKarma > 10000 )
		nCharID->SetKarma( 10000 );
	else if( nCurKarma < -10000 ) 
		nCharID->SetKarma( -10000 );
	//
	if( nCurKarma < nKarma && nKarma > 0 )
	{
		nChange = ( ( nKarma - nCurKarma ) / 75 );
		nCharID->SetKarma( (SI16)( nCurKarma + nChange ) );
		nEffect = true;
	}
	if( nCurKarma > nKarma && ( nKilledID == NULL || nKilledID->GetKarma() > 0 ) )
	{
		nChange = ( ( nCurKarma - nKarma ) / 50 );
		nCharID->SetKarma( (SI16)( nCurKarma - nChange ) );
		nEffect = false;
	}
	if( nChange == 0 )	// NPCs CAN gain/lose karma
		return;

	cSocket *mSock = calcSocketObjFromChar( nCharID );
	if( nCharID->IsNpc() || mSock == NULL )
		return;
	if( nChange <= 25 )
	{
		if( nEffect )
			sysmessage( mSock, 1367 );
		else
			sysmessage( mSock, 1368 );
		return;
	}
	if( nChange <= 50 )
	{
		if( nEffect )
			sysmessage( mSock, 1369 );
		else
			sysmessage( mSock, 1370 );
		return;
	}
	if( nEffect )
		sysmessage( mSock, 1371 );
	else
		sysmessage( mSock, 1372 );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void Fame( CChar *nCharID, int nFame )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Handle fame addition when character kills another
//|					Character / NPC
//o---------------------------------------------------------------------------o
void Fame( CChar *nCharID, int nFame )
{
	int nCurFame, nChange = 0;
	bool nEffect = false;
	nCurFame = nCharID->GetFame();
	if( nCharID->IsDead() )
	{
		if( nCurFame <= 0 )
			nCharID->SetFame( 0 );
		else
		{
			nChange = ( nCurFame - 0 ) / 25;
			nCharID->SetFame( (SI16)( nCurFame - nChange ) );
		}
		nCharID->SetDeaths( (UI16)( nCharID->GetDeaths() + 1 ) );
		nEffect = false;
	}
	else if( nCurFame <= nFame )
	{
		nChange = ( nFame - nCurFame ) / 75;
		nCharID->SetFame( (SI16)( nCurFame + nChange ) );
		nEffect = true;
		if( nCharID->GetFame() > 10000 )
			nCharID->SetFame( 10000 );
	}
	else
		return;	// current fame is greater than target fame, and we're not dead
	if( nChange == 0 )
		return;
	cSocket *mSock = calcSocketObjFromChar( nCharID );
	if( mSock == NULL || nCharID->IsNpc() )
		return;
	if( nChange <= 25 )
	{
		if( nEffect )
			sysmessage( mSock, 1373 );
		else
			sysmessage( mSock, 1374 );
	}
	else if( nChange <= 50 )
	{
		if( nEffect )
			sysmessage( mSock, 1375 );
		else
			sysmessage( mSock, 1376 );
	}
	else
	{
		if( nEffect )
			sysmessage( mSock, 1377 );
		else
			sysmessage( mSock, 1378 );
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void enlist( cSocket *mSock, SI32 listnum )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Handle enlistment objects
//o---------------------------------------------------------------------------o
void enlist( cSocket *mSock, SI32 listnum )
{
	char sect[50];
	
	char realSect[50];
	sprintf( sect, "ITEMLIST %i", listnum );
	strcpy( realSect, sect );
	ScriptSection *Enlist = FileLookup->FindEntry( realSect, items_def );
	if( Enlist == NULL )
	{
		Console << "ITEMLIST " << listnum << " not found, aborting" << myendl;
		return;
	}
	
	const char *tag = NULL;
	CItem *j = NULL;
	for( tag = Enlist->First(); !Enlist->AtEnd(); tag = Enlist->Next() )
	{
		j = Items->SpawnItemToPack( mSock, mSock->CurrcharObj(), tag, false );
		RefreshItem( j );
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void criminal( CChar *c )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Make character a criminal
//o---------------------------------------------------------------------------o
void criminal( CChar *c )
{
	if( !c->IsCriminal() )
	{
		c->SetCrimFlag( BuildTimeValue( (R32)cwmWorldState->ServerData()->GetRepCrimTime() ) );
		sysmessage( calcSocketObjFromChar( c ), 1379 );
		setcharflag( c );
		if( region[c->GetRegion()]->IsGuarded() && cwmWorldState->ServerData()->GetGuardsStatus() )
			Combat->SpawnGuard( c, c, c->GetX(), c->GetY(), c->GetZ() );
	}
	else
	{		// let's update their flag, as another criminal act will reset the timer
		c->SetCrimFlag( BuildTimeValue( (R32)cwmWorldState->ServerData()->GetRepCrimTime() ) );
		// chcek to see if there is a guard nearby, otherwise spawn us a new one
		if( !region[c->GetRegion()]->IsGuarded() || !cwmWorldState->ServerData()->GetGuardsStatus() )
			return;
		int xOffset = MapRegion->GetGridX( c->GetX() );
		int yOffset = MapRegion->GetGridY( c->GetY() );
		UI08 worldNumber = c->WorldNumber();
		for( SI08 counter1 = -1; counter1 <= 1; counter1++ )
		{
			for( SI08 counter2 = -1; counter2 <= 1; counter2++ )
			{
				SubRegion *toCheck = MapRegion->GetGrid( xOffset + counter1, yOffset + counter2, worldNumber );
				if( toCheck == NULL )
					continue;
				toCheck->PushChar();
				for( CChar *charCheck = toCheck->FirstChar(); !toCheck->FinishedChars(); charCheck = toCheck->GetNextChar() )
				{
					if( charCheck == NULL )
						continue;
					if( charCheck->GetNPCAiType() == 0x04 )
					{
						if( getCharDist( c, charCheck ) < 18 )
						{
							npcAttackTarget( c, charCheck );
							toCheck->PopChar();
							return;
						}
					}
				}
				toCheck->PopChar();
			}
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void setcharflag( CChar *c )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Set characters flag
//o---------------------------------------------------------------------------o
void setcharflag( CChar *c )
{
	if( c == NULL )
		return;
	UI08 oldFlag = c->GetFlag();
	if( !c->IsNpc() )
	{
		if( c->GetKills() > cwmWorldState->ServerData()->GetRepMaxKills() ) 
			c->SetFlagRed();
		else if( c->GetCrimFlag() > 0 || c->GetCrimFlag() < -1 )
			c->SetFlagGray();
		else 
			c->SetFlagBlue();
	} 
	else 
	{
		switch( c->GetNPCAiType() )
		{
		case 2://evil
		case 0x50://?
		case 666://evil healer
		case 0x58://bs/ev
			c->SetFlagRed();
			break;
		case 1://good
		case 17://Player Vendor
		case 4://healer
		case 30://?
		case 40://?
		case 0x08://banker
			c->SetFlagBlue();
			break;
		default:
			if( c->GetID() == 0x0190 || c->GetID() == 0x0191 )
			{
				c->SetFlagBlue();
				break;
			}
			else if( cwmWorldState->ServerData()->GetCombatAnimalsGuarded() && creatures[c->GetID()].IsAnimal() )
			{
				if( region[c->GetRegion()]->IsGuarded() )	// in a guarded region, with guarded animals, animals == blue
					c->SetFlagBlue();
				else
					c->SetFlagGray();
			}
			else	// if it's not a human form, and animal's aren't guarded, then they're gray
				c->SetFlagGray();
			if( c->GetOwner() != INVALIDSERIAL && c->IsTamed() )
			{
				CChar *i = (CChar *)c->GetOwnerObj();
				if( i != NULL )
					c->SetFlag( i->GetFlag() );
				else
					c->SetFlagBlue();
				if( c->IsInnocent() && !cwmWorldState->ServerData()->GetCombatAnimalsGuarded() )
					c->SetFlagBlue();
			}
			break;
		}
	}
	UI08 newFlag = c->GetFlag();
	if( oldFlag != newFlag )
	{
		UI16 targTrig = c->GetScriptTrigger();
		cScript *toExecute = Trigger->GetScript( targTrig );
		if( toExecute != NULL )
			toExecute->OnFlagChange( c, newFlag, oldFlag );
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void playTileSound cSocket *mSock )
//|	Programmer	-	Abaddon
//o---------------------------------------------------------------------------o
//|	Purpose		-	Play a sound based on the tile character is on
//o---------------------------------------------------------------------------o
void playTileSound( cSocket *mSock )
{
	if( mSock == NULL )
		return;

	CTile tile;
	char search1[10];
	UI16 sndid = 0;
	enum TileType
	{
		TT_NORMAL = 0,
		TT_WATER,
		TT_STONE,
		TT_OTHER,
		TT_WOODEN,
		TT_GRASS
	};
	TileType tileType = TT_NORMAL;
	bool onHorse = false;

	CChar *mChar = mSock->CurrcharObj();
	
	if( mChar->GetHidden() || mChar->GetCommandLevel() >= CNSCMDLEVEL )
		return;
	
	if( mChar->IsOnHorse() )
		onHorse = true;
	
	if( mChar->GetStep() == 1 || mChar->GetStep() == 0 )	// if we play a sound
	{
		SI16 x = mChar->GetX();
		SI16 y = mChar->GetY();
		
		MapStaticIterator msi( x, y, mChar->WorldNumber() );
		staticrecord *stat = msi.Next();
		if( stat )
			msi.GetTile(&tile);
	}
	
	if( tile.LiquidWet() )
		tileType = TT_WATER;
	strcpy( search1, "wood" );
	if( strstr( tile.Name(), search1 ) )
		tileType = TT_WOODEN;
	strcpy( search1, "ston" );
	if( strstr( tile.Name(), search1 ) )
		tileType = TT_STONE;
	strcpy( search1, "gras" );
	if( strstr( tile.Name(), search1 ) )
		tileType = TT_GRASS;
	
	switch( mChar->GetStep() )	// change step info
	{
	case 0:
		mChar->SetStep( 3 );	// step 2
		switch( tileType )
		{
		case TT_NORMAL:
			if( onHorse )
				sndid = 0x024C;
			else
				sndid = 0x012B; 
			break;
		case TT_WATER:	// water
			break;
		case TT_STONE: // stone
			sndid = 0x0130;
			break;
		case TT_OTHER: // other
		case TT_WOODEN: // wooden
			sndid = 0x0123;
			break;
		case TT_GRASS: // grass
			sndid = 0x012D;
			break;
		}
		break;
	case 1:
		mChar->SetStep( 0 );	// step 1
		switch( tileType )
		{
		case TT_NORMAL:
			if( onHorse )
				sndid = 0x024B;
			else
				sndid = 0x012C; 
			break;
		case TT_WATER:	// water
			break;
		case TT_STONE: // stone
			sndid = 0x012F;
			break;
		case TT_OTHER: // other
		case TT_WOODEN: // wooden
			sndid = 0x0122;
			break;
		case TT_GRASS: // grass
			sndid = 0x012E;
			break;
		}
		break;
	case 2:
	case 3:
	default:
		mChar->SetStep( 1 );	// pause
		break;
	}
	if( sndid )			// if we have a valid sound
		soundeffect( mSock, sndid, true );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void RefreshItem( CItem *i )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Send this item to all online people in range
//o---------------------------------------------------------------------------o
void RefreshItem( CItem *i )
{
	if( i == NULL ) 
		return;

	if( i->GetCont() == i->GetSerial() )
	{
		Console << myendl << "ALERT! item " << i->GetName() << " [serial: " << i->GetSerial() << "] has dangerous container value, autocorrecting" << myendl;
		i->SetCont( INVALIDSERIAL );
	}

	SERIAL iCont = i->GetCont();

	if( iCont == INVALIDSERIAL )
	{
		Network->PushConn();
		for( cSocket *aSock = Network->FirstSocket(); !Network->FinishedSockets(); aSock = Network->NextSocket() ) // send this item to all the sockets in range
		{
			CChar *aChar = aSock->CurrcharObj();
			if( aChar == NULL )
				continue;

			if( itemInRange( aChar, i, aSock->Range() + Races->VisRange( aChar->GetRace() ) ) )
			{
				if( i->GetVisible() == 0 || ( ( i->GetVisible() == 1 || i->GetVisible() == 2 ) && aChar->IsGM() ) )// we're a GM, or not hidden
					sendItem( aSock, i );
			}
		}
		Network->PopConn();
		return;
	}
	else if( iCont < 0x40000000 )
	{
		if( i->GetGlow() > 0 )
			Items->GlowItem( i );

		CChar *charCont = calcCharObjFromSer( iCont );
		if( charCont != NULL )
		{
			CPWornItem toWear = (*i);
			Network->PushConn();
			for( cSocket *cSock = Network->FirstSocket(); !Network->FinishedSockets(); cSock = Network->NextSocket() )
			{
				if( charInRange( cSock->CurrcharObj(), charCont ) )
					cSock->Send( &toWear );
			}
			Network->PopConn();
			return;
		}
	}
	else
	{
		CItem *itemCont = calcItemObjFromSer( iCont );
		if( itemCont != NULL )
		{
			Network->PushConn();
			for( cSocket *bSock = Network->FirstSocket(); !Network->FinishedSockets(); bSock = Network->NextSocket() )
				sendPackItem( bSock, i );
			Network->PopConn();
			return;
		}
	}
	Console.Error( 2, " RefreshItem(%i): cannot determine container type!", i );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void loadregions( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Load regions from regions.wsc
//o---------------------------------------------------------------------------o
void loadregions( void )
{
	int i, l = 0;
	const char *tag = NULL;
	const char *data = NULL;
	bool performLoad = false;
	Script *ourRegions = NULL;
	char regionsFile[MAX_PATH];
	sprintf(regionsFile, "%s%s", cwmWorldState->ServerData()->GetSharedDirectory(), "regions.wsc");
	FILE *ourReg = fopen( regionsFile, "r" );
	if( ourReg != NULL )
	{
		performLoad = true;
		fclose( ourReg );
		ourRegions = new Script( regionsFile, NUM_DEFS, false );
	}
	for( i = 0; i < 256; i++ )
	{
		region[i] = new cTownRegion( (UI08)i );
		region[i]->InitFromScript( l );
		if( performLoad )
			region[i]->Load( ourRegions );
	}
	if( performLoad )
	  {
		delete ourRegions;
		ourRegions = NULL;
	  }
	locationcount = (UI16)l;
	logoutcount = 0;
	//Instalog
	ScriptSection *InstaLog = FileLookup->FindEntry( "INSTALOG", regions_def );
	if( InstaLog == NULL ) 
		return;
	for( tag = InstaLog->First(); !InstaLog->AtEnd(); tag = InstaLog->Next() )
	{
		data = InstaLog->GrabData();
		if( !strcmp( tag,"X1" ) ) 
			logout[logoutcount].x1 = (SI16)makeNum( data );
		else if( !strcmp( tag, "Y1" ) ) 
			logout[logoutcount].y1 = (SI16)makeNum( data );
		else if( !strcmp( tag, "X2" ) ) 
			logout[logoutcount].x2 = (SI16)makeNum( data );
		else if( !strcmp( tag, "Y2" ) )
		{
			logout[logoutcount].y2 = (SI16)makeNum( data );
			logoutcount++;
		}
	}
}

//o---------------------------------------------------------------------------o
//|   Function    -  void doDeathStuff( CChar *i )
//|   Date        -  UnKnown
//|   Programmer  -  UnKnown  (Touched tabstops by Tauriel Dec 28, 1998)
//o---------------------------------------------------------------------------o
//|   Purpose     -  Performs death stuff. I.E.- creates a corpse, moves items
//|                  to it, take out of war mode, does animation and sound, etc.
//o---------------------------------------------------------------------------o
void doDeathStuff( CChar *i )
{
	if( i == NULL || i->IsDead() || i->IsInvulnerable() )	// don't kill them if they are dead or invulnerable!
		return;

	int ele;
	
	cSocket *pSock = calcSocketObjFromChar( i );
	int nType = 0;
	char temp[1024];

	if( i->GetID() != i->GetOrgID() )
	{
		i->SetID( i->GetOrgID() );
		i->Teleport();
	}

	i->SetxID( i->GetID() );
	i->SetxSkin( i->GetSkin() );
	if( ( i->GetID( 1 ) == 0x00 ) && ( i->GetID( 2 ) == 0x0C || ( i->GetID( 2 ) >= 0x3B && i->GetID( 2 ) <= 0x3D ) ) ) // If it's a dragon, 50/50 chance you can carve it
		nType = RandomNum( 0, 1 );

	CChar *murderer = NULL;
	if( i->GetAttacker() != INVALIDSERIAL ) 
		murderer = &chars[i->GetAttacker()]; 

	if( pSock != NULL )
		DismountCreature( i );
	killTrades( i );

	ele = 0;
	
	if( !i->IsNpc() ) 
	{
	//	i->SetID( 0x01, 1 ); // Character is a ghost
	if( i->GetxID( 2 ) == 0x91)
		i->SetID( 0x193 );  // Male or Female
	else
		i->SetID( 0x192 );
	}
	playDeathSound( i );
	i->SetSkin( 0x0000 );
	i->SetDead( true );
	i->StopSpell();
	i->SetHP( 0 );
	i->SetPoisoned( 0 );
	i->SetPoison( 0 );

	CItem *corpsenum = GenerateCorpse( i, nType, murderer );

	if( !i->IsNpc() )
	{ 
		strcpy( temp, Dictionary->GetEntry( 1610 ) );

		//Zippy 9/17/01 Fix for killing offline players.
		CItem *c = NULL;
		if( !isOnline( i ) )
			c = Items->SpawnItem( NULL, i,  1, temp, 0, 0x204E, 0, false, false );//we can spawn item with a NULL socket as long as we don't Send
		else
			c = Items->SpawnItem( pSock, 1, temp, 0, 0x204E, 0, false, false );

		if( c == NULL ) 
			return;
		i->SetRobe( c->GetSerial() );
		c->SetLayer( 0x16 );
		if( c->SetCont( i->GetSerial() ) )
			c->SetDef( 1 );
	}
	if( cwmWorldState->ServerData()->GetDeathAnimationStatus() )
		deathAction( i, corpsenum );
	if( i->GetAccount().wAccountIndex != -1 )
	{
		i->Teleport();
		if( pSock != NULL ) 
			deathMenu( pSock );
	}		
	
	RefreshItem( corpsenum );

	UI16 targTrig = i->GetScriptTrigger();
	cScript *toExecute = Trigger->GetScript( targTrig );
	if( toExecute != NULL )
		toExecute->OnDeath( i );

//	if( ele == 60000 || i->GetNPCAiType() == 80 )
//		Items->DeleItem( corpsenum );
	if( i->IsNpc() ) 
		Npcs->DeleteChar( i );
}

//o---------------------------------------------------------------------------o
//|	Function	-	CItem *GenerateCorpse( CChar *i, int nType, char *murderername )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Generates a corpse based on skin of the character killed
//o---------------------------------------------------------------------------o
CItem *GenerateCorpse( CChar *i, int nType, CChar *murderer )
{
	char temp[512];
	CItem *k, *c, *p = getPack( i );
	sprintf( temp, Dictionary->GetEntry( 1612 ), i->GetName() );
	bool corpse = !( ( i->GetxID() >= 13 && i->GetxID() <= 16 && i->GetxID() != 14 ) || i->GetxID() == 574 );

	if( corpse )
	{
		c = Items->SpawnItem( NULL, i, 1, temp, false, 0x2006, i->GetxSkin(), false, false );
		if( c == NULL ) 
			return NULL;
		c->SetCorpse( true );
		c->SetCarve( i->GetCarve() );
		c->SetMagic( 2 );//non-movable
		c->SetAmount( i->GetxID() );
		c->SetDir( i->GetDir() );
	} 
	else 
	{
		c = Items->SpawnItem( NULL, i, 1, Dictionary->GetEntry( 1611 ), false, 0x09B2, 0x0000, false, false );
		if( c== NULL ) 
			return NULL;
		c->SetCorpse( false );
	}
	c->SetMoreY( isHuman( i ) );
	c->SetName2( i->GetName() );

	c->SetType( 1 );
	c->SetLocation( i );

	c->SetMore( (UI08)nType, 1 );
	c->SetDecayTime( BuildTimeValue( (R32)cwmWorldState->ServerData()->GetSystemTimerStatus( DECAY ) ) );
	
	CMultiObj *iMulti = findMulti( c->GetX(), c->GetY(), c->GetZ(), c->WorldNumber() );
	if( iMulti != NULL ) 
		c->SetMulti( iMulti );

	if( !i->IsNpc() )
	{
		c->SetOwner( i->GetSerial() );
		c->SetMore( char( cwmWorldState->ServerData()->GetSystemTimerStatus( PLAYER_CORPSE )&0xff ), 4 ); // how many times longer for the player's corpse to decay
	}
	c->SetMoreZ( i->GetFlag() );
	if( murderer == NULL )
		c->SetMurderer( INVALIDSERIAL );
	else
		c->SetMurderer( murderer->GetSerial() );
	c->SetMurderTime( uiCurrentTime );
	
	for( CItem *j = i->FirstItem(); !i->FinishedItems(); j = i->NextItem() )
	{
		if( j == NULL ) 
			continue;
		
		if( j->GetLayer() != 0x0B && j->GetLayer() != 0x10 )
		{
			if( j->GetType() == 1 && j->GetLayer() != 0x1A && j->GetLayer() != 0x1B && j->GetLayer() != 0x1C && j->GetLayer() != 0x1D )
			{
				for( k = j->FirstItemObj(); !j->FinishedItems(); k = j->NextItemObj() )
				{
					if( k == NULL ) 
						continue;
				
							if( !k->isNewbie() && k->GetType() != 9 )
							{
								k->SetCont( c->GetSerial() );
								k->SetX( (UI16)( 20 + ( RandomNum( 0, 49 ) ) ) );
								k->SetY( (UI16)( 85 + ( RandomNum( 0, 75 ) ) ) );
								k->SetZ( 9 );
								RefreshItem( k );
							}
						}
					}

				if( j != p && j->GetLayer() != 0x1D )
				{
					if( j->isNewbie() && p != NULL )
						j->SetCont( p->GetSerial() );
					else
						j->SetCont( c->GetSerial() );
				}

			if( j->GetLayer() == 0x15 && !i->IsShop() && corpse )
					j->SetLayer( 0x1A );
			j->SetX( (UI16)( 20 + ( RandomNum( 0, 49 ) ) ) );
			j->SetY( (UI16)( 85 + ( RandomNum( 0, 74 ) ) ) );
				j->SetZ( 9 );

			CPRemoveItem toRemove( (*j) );
			Network->PushConn();
			for( cSocket *kSock = Network->FirstSocket(); !Network->FinishedSockets(); kSock = Network->NextSocket() )
				kSock->Send( &toRemove );
			Network->PopConn();
			if( j != p )
				RefreshItem( j );
		}
		
//		if( !corpse ) 
//			j->SetLayer( 0 );
		
			if( j->GetLayer() == 0x0B || j->GetLayer() == 0x10 )
			{
				j->SetName( "Hair/Beard" );
				j->SetX( 0x47 );
				j->SetY( 0x93 );
				j->SetZ( 0 );
			}
	}//for loop
	return c;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void SendWorldChange( WorldType season, cSocket *sock )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Send world changes to socket
//o---------------------------------------------------------------------------o
void SendWorldChange( WorldType season, cSocket *sock )
{
	CPWorldChange wrldChange( season, 1 );
	sock->Send( &wrldChange );
}

void SendMapChange( UI08 worldNumber, cSocket *sock, bool initialLogin )
{
	if( sock == NULL )
		return;
	CMapChange mapChange( worldNumber );
	if( !initialLogin && worldNumber > 1 )
	{
		switch( sock->ClientType() )
		{
		case CV_UO3D:
		case CV_KRRIOS:
			break;
		default:
			mapChange.SetMap( 0 );
			break;
		}
	}
	sock->Send( &mapChange );
}

void SocketMapChange( cSocket *sock, CChar *charMoving, CItem *gate )
{
	if( gate == NULL || ( sock == NULL && charMoving == NULL ) )
		return;
	UI08 tWorldNum = (UI08)gate->GetMore();
	if( !Map->MapExists( tWorldNum ) )
		return;
	CChar *toMove = charMoving;
	if( sock != NULL && charMoving == NULL )
		toMove = sock->CurrcharObj();
	if( toMove == NULL )
		return;
	switch( sock->ClientType() )
	{
	case CV_UO3D:
	case CV_KRRIOS:
		toMove->SetLocation( (SI16)gate->GetMoreX(), (SI16)gate->GetMoreY(), (SI08)gate->GetMoreZ(), tWorldNum );
		break;
	default:
		if( tWorldNum <= 1 )
			toMove->SetLocation( (SI16)gate->GetMoreX(), (SI16)gate->GetMoreY(), (SI08)gate->GetMoreZ(), tWorldNum );
		else
			toMove->SetLocation( (SI16)gate->GetMoreX(), (SI16)gate->GetMoreY(), (SI08)gate->GetMoreZ(), 0 );
		break;
	}
	toMove->RemoveFromSight();
	SendMapChange( tWorldNum, sock );
	toMove->Teleport();

}

//o---------------------------------------------------------------------------o
//|	Function	-	void UseHairDye( cSocket *s, UI16 colour, CItem *x )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Dye hair certain color based on hair dye used
//o---------------------------------------------------------------------------o
void UseHairDye( cSocket *s, UI16 colour, CItem *x )
{
	CChar *dest = s->CurrcharObj();
	CItem *beardobject = FindItemOnLayer( dest, 0x10 );
	CItem *hairobject  = FindItemOnLayer( dest, 0x0B );

	if( hairobject != NULL )
	{
		hairobject->SetColour( colour );
		RefreshItem( hairobject );
	}
	if( beardobject != NULL )
	{
		beardobject->SetColour( colour );
		RefreshItem( beardobject );
	}
	Items->DeleItem( x );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void Bounce( cSocket *bouncer, CItem *bouncing )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Bounce items back from where they came
//o---------------------------------------------------------------------------o
void Bounce( cSocket *bouncer, CItem *bouncing )
{
	if( bouncer == NULL || bouncing == NULL )
		return;
	CPBounce bounce( 5 );
	PickupLocations from = bouncer->PickupSpot();
	SERIAL spot = bouncer->PickupSerial();
	switch( from )
	{
	default:
	case PL_NOWHERE:	break;
	case PL_GROUND:		
		SI16 x, y;
		SI08 z;
		x = bouncer->PickupX();
		y = bouncer->PickupY();
		z = bouncer->PickupZ();
		if( bouncing->GetX() != x || bouncing->GetY() != y || bouncing->GetZ() != z )
			bouncing->SetLocation( x, y, z );
		break;
	case PL_OWNPACK:	
	case PL_OTHERPACK:
	case PL_PAPERDOLL:	
		if( bouncing->GetCont() != spot )
			bouncing->SetCont( spot );	
		break;
	}
	bouncer->Send( &bounce );
	bouncer->PickupSpot( PL_NOWHERE );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void NetworkPollConnectionThread( void *params )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Watch for new connections
//o---------------------------------------------------------------------------o
void NetworkPollConnectionThread( void *params )
{
	messageLoop << "Thread: NetworkPollConnection has started";
	netpollthreadclose = false;
	while( !netpollthreadclose )
	{
		Network->CheckConnections();
		Network->CheckLoginMessage();
		UOXSleep( 20 );
	}
#ifdef __LINUX__
	pthread_exit( NULL );
#else
	_endthread();
#endif
	messageLoop << "Thread: NetworkPollConnection has Closed";
}

//o---------------------------------------------------------------------------o
//|	Function	-	CMultiObj *findMulti( SI16 x, SI16 y, SI08 z, UI08 worldNumber )
//|	Programmer	-	Zippy
//o---------------------------------------------------------------------------o
//|	Purpose		-	Find a multi at x,y,z
//o---------------------------------------------------------------------------o
CMultiObj *findMulti( SI16 x, SI16 y, SI08 z, UI08 worldNumber )
{
	int lastdist=30;
	CMultiObj *multi = NULL;
	int ret, dx, dy;

	int xOffset = MapRegion->GetGridX( x );
	int yOffset = MapRegion->GetGridY( y );
	for( SI08 counter1 = -1; counter1 <= 1; counter1++ )
	{
		for( SI08 counter2 = -1; counter2 <= 1; counter2++ )
		{
			SubRegion *toCheck = MapRegion->GetGrid( xOffset + counter1, yOffset + counter2, worldNumber );
			if( toCheck == NULL )
				continue;
			toCheck->PushItem();
			for( CItem *itemCheck = toCheck->FirstItem(); !toCheck->FinishedItems(); itemCheck = toCheck->GetNextItem() )
			{
				if( itemCheck == NULL )
					continue;
				if( itemCheck->GetID( 1 ) >= 0x40 )
				{
					dx = abs( x - itemCheck->GetX() );
					dy = abs( y - itemCheck->GetY() );
					ret = (int)( hypot( dx, dy ) );
					if( ret <= lastdist )
					{
						lastdist = ret;
						if( inMulti( x, y, z, itemCheck, worldNumber ) )
						{
							multi = static_cast<CMultiObj *>(itemCheck);
							toCheck->PopItem();
							return multi;
						}
					}
				}
			}
			toCheck->PopItem();
		}
	}
	return multi;
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool inMulti( SI16 x, SI16 y, SI08 z, CItem *m, UI08 worldNumber )
//|	Programmer	-	Zippy
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check if item is in a multi
//o---------------------------------------------------------------------------o
bool inMulti( SI16 x, SI16 y, SI08 z, CItem *m, UI08 worldNumber )
{
	if( m == NULL )
		return false;
	SI32 length;
	st_multi *multi = NULL;
	UI16 multiID = (UI16)(( m->GetID() ) - 0x4000);
	Map->SeekMulti( multiID, &length );

	if( length == -1 || length >= 17000000)
	{
		Console << "inmulti() - Bad length in multi file, avoiding stall. Item Name: " << m->GetName() << " " << m->GetSerial() << myendl;
		length = 0;
	}
	for( SI32 j = 0; j < length; j++ )
	{
		multi = Map->SeekIntoMulti( multiID, j );

		if( multi->visible && ( m->GetX() + multi->x == x ) && ( m->GetY() + multi->y == y ) )
			return true;
	}
	return false;
}

