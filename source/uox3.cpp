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
#include "weight.h"
#include "books.h"
#include "cGuild.h"
#include "combat.h"
#include "msgboard.h"
#include "townregion.h"
#include "cWeather.hpp"
#include "movement.h"
#include "cRaces.h"
#include "cServerDefinitions.h"
#include "skills.h"
#include "commands.h"
#include "cSpawnRegion.h"
#include "wholist.h"
#include "cMagic.h"
#include "PageVector.h"
#include "speech.h"
#include "cVersionClass.h"
#include "ssection.h"
#include "cHTMLSystem.h"
#include "gump.h"
#include "trigger.h"
#include "cScript.h"
#include "cEffects.h"
#include "teffect.h"
#include "CPacketSend.h"
#include "classes.h"
#include "cThreadQueue.h"
#include "regions.h"
#include "magic.h"
#include "jail.h"
#include "Dictionary.h"
#include "ObjectFactory.h"

namespace UOX
{

#if UOX_PLATFORM != PLATFORM_WIN32
	typedef void *HANDLE;
	pthread_t cons, netw;
#else
	#include <process.h>
	#include <conio.h>
#endif

timeval current;

#undef DBGFILE
#define DBGFILE "uox3.cpp"

ObjectFactory *objFactory;

//o---------------------------------------------------------------------------o
// Weather Pre-Declarations
//o---------------------------------------------------------------------------o
bool		doWeatherEffect( CSocket& mSock, CChar& mChar, WeatherType element );
bool		doLightEffect( CSocket& mSock, CChar& mChar );

//o---------------------------------------------------------------------------o
// FileIO Pre-Declarations
//o---------------------------------------------------------------------------o
void		LoadINIFile( void );
void		LoadCustomTitle( void );
void		LoadSkills( void );
void		LoadSpawnRegions( void );
void		LoadRegions( void );
void		LoadTeleportLocations( void );
void		LoadCreatures( void );
void		LoadPlaces( void );

//o---------------------------------------------------------------------------o
// Misc Pre-Declarations
//o---------------------------------------------------------------------------o
void		checkkey( void );
void		restockNPC( CChar& i, bool stockAll );
void		clearTrades( void );
void		killTrades( CChar& i );
void		sysBroadcast( const std::string txt );
void		MoveBoat( UI08 dir, CBoatObj *boat );
bool		DecayItem( CItem& toDecay, UI32 nextDecayItems );
void		CheckAI( CChar *i );

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
#if UOX_PLATFORM != PLATFORM_WIN32
	// first the linux style, don't change it's behavoir
	UI08 c = 0;
	fd_set KEYBOARD;
	FD_ZERO( &KEYBOARD );
	FD_SET( 0, &KEYBOARD );
	int s = select( 1, &KEYBOARD, NULL, NULL, &cwmWorldState->uoxtimeout );
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
#else
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
			case MSG_SHUTDOWN:		cwmWorldState->SetKeepRun( false );				break;
			case MSG_COUNT:															break; 
			case MSG_WORLDSAVE:		cwmWorldState->SetOldTime( 0 );					break;
			case MSG_PRINT:			Console << tVal.data << myendl;					break;
			case MSG_RELOADJS:		Trigger->ReloadJS();	Console.PrintDone();	break;
			case MSG_CONSOLEBCAST:	sysBroadcast( tVal.data );						break;
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

//	EviLDeD	-	June 21, 1999
//	Ok here is thread number one its a simple thread for the checkkey() function
//	EviLDeD	-	June 12, 2004 (<-- LOL must be an omen)
//	xFTPd - Initial implementation of a very light weight FTP server for UOX3. 
//					Expected functionality to provide file up/download access to shard
//					operators that may not have direct access to their servers. 
//					
//					Commands supported: 
#if UOX_PLATFORM == PLATFORM_WIN32
CRITICAL_SECTION sc;	//
#endif
bool conthreadcloseok = false;
bool netpollthreadclose = false;
bool xFTPdclose = false;
#if UOX_PLATFORM != PLATFORM_WIN32
void *NetworkPollConnectionThread( void *params );
void *xFTPdConnectionThread( void *params );
void *xFTPcConnectionThread( void *params );
#else
void NetworkPollConnectionThread( void *params );
void xFTPdConnectionThread( void *params );
void xFTPcConnectionThread( void *params );
#endif
#if UOX_PLATFORM != PLATFORM_WIN32
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
#if UOX_PLATFORM == PLATFORM_WIN32
	_endthread();		// linux will kill the thread when it returns
#endif
	messageLoop << "Thread: CheckConsoleKeyThread Closed";
#if UOX_PLATFORM != PLATFORM_WIN32
	return NULL;
#endif
}
//	EviLDeD	-	End

#if UOX_PLATFORM == PLATFORM_WIN32
///////////////////

//HANDLE hco;
//CONSOLE_SCREEN_BUFFER_INFO csbi;

///////////////////
#endif

#if UOX_PLATFORM != PLATFORM_WIN32

void closesocket( UOXSOCKET s )
{
	shutdown( s, 2 );
	close( s );
}
#endif

//o--------------------------------------------------------------------------o
//|	Function		-	bool isOnline( CChar *c )
//|	Date			-	
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Check if the socket owning character c is still connected
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
bool isOnline( CChar *c )
{
	if( !ValidateObject( c ) )
		return false;
	if( c->IsNpc() )
		return false;
	ACCOUNTSBLOCK& actbTemp = c->GetAccount();
	if( actbTemp.wAccountIndex != AB_INVALID_ID )
	{
		if( actbTemp.dwInGame == c->GetSerial() )
			return true;
	}
	Network->PushConn();
	for( CSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
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
//|	Function	-	void updateStats( CChar *c, char x )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Updates characters stats
//o---------------------------------------------------------------------------o
void updateStats( CChar *mChar, UI08 x )
{
	CPUpdateStat toSend( (*mChar), x );
	SOCKLIST nearbyChars = FindNearbyPlayers( mChar );
	for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
	{
		(*cIter)->Send( &toSend );
		if( (*cIter)->CurrcharObj() == mChar )
			(*cIter)->statwindow( mChar );
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void CollectGarbage( void )
//|	Programmer	-	giwo
//o---------------------------------------------------------------------------o
//|	Purpose		-	Deletes objects in the Deletion Queue
//o---------------------------------------------------------------------------o
void CollectGarbage( void )
{
	Console << "Performing Garbage Collection...";
	UI32 objectsDeleted				= 0;
	QUEUEMAP_CITERATOR delqIter		= deletionQueue.begin();
	QUEUEMAP_CITERATOR delqIterEnd	= deletionQueue.end();
	while( delqIter != delqIterEnd )
	{
		CBaseObject *mObj = delqIter->first;
		++delqIter;
		if( mObj == NULL || mObj->isFree() || !mObj->isDeleted() )
		{
			Console.Warning( 2, "Invalid object found in Deletion Queue" );
			continue;
		}
		ObjectFactory::getSingleton().DestroyObject( mObj );
		++objectsDeleted;
	}
	deletionQueue.clear();

	Console << " Removed " << objectsDeleted << " objects" << myendl;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void MountCreature( CChar *s, CChar *x )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Mount a ridable creature
//|									
//|	Modification	-	09/22/2002	-	Xuri - Unhide players when mounting horses etc.
//o---------------------------------------------------------------------------o
//|	Returns			- N/A
//o--------------------------------------------------------------------------o	
void MountCreature( CSocket *sockPtr, CChar *s, CChar *x )
{
	if( !objInRange( s, x, DIST_NEXTTILE ) )
		return;
	if( x->GetOwnerObj() == s || s->IsGM() )
	{
		if( s->IsOnHorse() )
		{
			if( !cwmWorldState->ServerData()->CharHideWhileMounted() )
				s->ExposeToView();
		}
		s->SetOnHorse( true );
		CItem *c = Items->CreateItem( NULL, s, 0x0915, 1, x->GetSkin(), OT_ITEM );
		c->SetName( x->GetName() );

		// Weazel 12 July, 2001 - Not all 3rd dawn creature mount id's are correct still missing a faction horse/dragon horse and
		// the ethereal llama and ostards.
		switch( x->GetID() )
		{
			case 0x72:	c->SetID( 0x3EA9 );	break;	// Dark Steed
			case 0x73:	c->SetID( 0x3EAA );	break;	// Etheral Horse
			case 0x74:	c->SetID( 0x3EB5 );	break;	// Nightmare
			case 0x75:	c->SetID( 0x3EA8 );	break;	// Silver Steed
			case 0x76:	c->SetID( 0x3EB2 );	break;	// Faction Horse
			case 0x77:	c->SetID( 0x3EB1 );	break;	// Faction Horse
			case 0x78:	c->SetID( 0x3EAF );	break;	// Faction Horse
			case 0x79:	c->SetID( 0x3EB0 );	break;	// Faction Horse
			case 0x7A:	c->SetID( 0x3EB4 );	break;	// Unicorn
			case 0x84:	c->SetID( 0x3EAD );	break;	// Kirin
			case 0x90:	c->SetID( 0x3EB3 );	break;	// Sea Horse
			case 0xAA:	c->SetID( 0x3EAB );	break;	// Etheral Llama
			case 0xAB:	c->SetID( 0x3EAC );	break;	// Etheral Ostard
			case 0xB1:	c->SetID( 0x3EA7 );	break;	// Nightmare
			case 0xB2:	c->SetID( 0x3EA9 );	break;	// Nightmare
			case 0xB3:	c->SetID( 0x3EB7 );	break;	// TD Nightmare
			case 0xBB:	c->SetID( 0x3EB8 );	break;	// Ridgeback
			case 0xBE:	c->SetID( 0x3E9E );	break;	// Fire Steed
			case 0xBF:	c->SetID( 0x3E9C );	break;	// Etheral Kirin
			case 0xCC:	c->SetID( 0x3EA2 );	break;	// horse
			case 0xC0:	c->SetID( 0x3EB4 );	break;	// Etheral Unicorn
			case 0xC1:	c->SetID( 0x3E9A );	break;	// Etheral Ridgeback
			case 0xC2:	c->SetID( 0x3E98 );	break;	// Etheral Swampdragon
			case 0xC3:	c->SetID( 0x3E97 );	break;	// Etheral Beetle
			case 0xC8:	c->SetID( 0x3E9F );	break;	// horse
			case 0xD2:	c->SetID( 0x3EA3 );	break;	// Desert Ostard
			case 0xDA:	c->SetID( 0x3EA4 );	break;	// Frenzied Ostard
			case 0xDB:	c->SetID( 0x3EA5 );	break;	// Forest Ostard
			case 0xDC:	c->SetID( 0x3EA6 );	break;	// llama
			case 0xE2:	c->SetID( 0x3EA0 );	break;	// horse
			case 0xE4:	c->SetID( 0x3EA1 );	break;	// horse
			case 0x319:	c->SetID( 0x3EBB );	break;  // Skeletal Mount 
			case 0x317:	c->SetID( 0x3EBC );	break;  // Giant Beetle  
			case 0x31A:	c->SetID( 0x3EBD );	break;  // Swamp Dragon  
			case 0x31F:	c->SetID( 0x3EBE );	break;  // Armored Swamp Dragon  
			case 0x3E6:	c->SetID( 0x3EAD );	break;	// Kirin
			default:	c->SetID( 0x3E00 );	break;	// Bad
		}
		
		c->SetLayer( IL_MOUNT );

		if( !c->SetCont( s ) )
		{
			s->SetOnHorse( false );	// let's get off our horse again
			return;
		}
		//s->WearItem( c );
		SOCKLIST nearbyChars = FindNearbyPlayers( s );
		for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
		{
			s->SendWornItems( (*cIter) );
		}

		if( x->GetTarg() != NULL )	// zero out target, under all circumstances
		{
			x->SetTarg( NULL );
			x->SetWar( false );
		}
		if( ValidateObject( x->GetAttacker() ) )
			x->GetAttacker()->SetTarg( NULL );
		x->SetLocation( 7000, 7000, 0 );
		x->SetFrozen( true );
		c->SetTempVar( CITV_MOREX, x->GetSerial() );
		if( x->GetTimer( tNPC_SUMMONTIME ) != 0 )
			c->SetDecayTime( x->GetTimer( tNPC_SUMMONTIME ) );
	}
	else
		sockPtr->sysmessage( 1214 );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void DismountCreature( CChar *s )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Dismount a ridable creature
//o---------------------------------------------------------------------------o
void DismountCreature( CChar *s )
{
	if( !ValidateObject( s ) )
		return;
	CItem *ci = s->GetItemAtLayer( IL_MOUNT );
	if( !ValidateObject( ci ) || ci->isFree() )	// if no horse item, or it's our default one
		return;

	s->SetOnHorse( false );
	CChar *tMount = calcCharObjFromSer( ci->GetTempVar( CITV_MOREX ) );
	if( ValidateObject( tMount ) )
	{
		tMount->SetLocation( s );
		tMount->SetFrozen( false );
		if( ci->GetDecayTime() != 0 )
			tMount->SetTimer( tNPC_SUMMONTIME, ci->GetDecayTime() );
		tMount->SetDir( s->GetDir() );
		tMount->SetVisible( VT_VISIBLE );
	}
	ci->Delete();
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
	UI32 igetclock = cwmWorldState->GetUICurrentTime();
	if( cwmWorldState->GetEndTime() < igetclock )
		cwmWorldState->SetEndTime( igetclock );
	char temp[1024];
	sprintf( temp, Dictionary->GetEntry( 1209 ).c_str(), ((cwmWorldState->GetEndTime()-igetclock)/ 1000 ) / 60 );
	sysBroadcast( temp );
}

#if UOX_PLATFORM != PLATFORM_WIN32
void illinst( int x = 0 ) //Thunderstorm linux fix
{
	sysBroadcast( "Fatal Server Error! Bailing out - Have a nice day!" );
	Console.Error( 0, "Illegal Instruction Signal caught - attempting shutdown" );
	endmessage( x );
}

void aus( int signal )
{
	Console.Error( 2, "Server crash averted! Floating point exception caught." );
} 

#endif

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
	if( !ValidateObject( mChar ) )
		return;

	if( !mChar->GetRegion()->IsGuarded() || !cwmWorldState->ServerData()->GuardsStatus() )
		return;

	CChar *attacker = mChar->GetAttacker();
	if( ValidateObject( attacker ) )
	{
		if( !attacker->IsDead() && ( attacker->IsCriminal() || attacker->IsMurderer() ) )
		{
			if( charInRange( mChar, attacker ) )
			{
				callGuards( mChar, attacker );
				return;
			}
		}
	}

	SubRegion *toCheck = MapRegion->GetCell( mChar->GetX(), mChar->GetY(), mChar->WorldNumber() );
	if( toCheck == NULL )
		return;
	CDataList< CChar * > *regChars = toCheck->GetCharList();
	regChars->Push();
	for( CChar *tempChar = regChars->First(); !regChars->Finished(); tempChar = regChars->Next() )
	{
		if( !ValidateObject( tempChar ) )
			break;

		if( !tempChar->IsDead() && ( tempChar->IsCriminal() || tempChar->IsMurderer() ) )
		{
			if( charInRange( tempChar, mChar ) )
			{
				callGuards( mChar, tempChar );
				break;
			}
		}
	}
	regChars->Pop();
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
	if( !ValidateObject( mChar ) || !ValidateObject( targChar ) )
		return;

	if( !mChar->GetRegion()->IsGuarded() || !cwmWorldState->ServerData()->GuardsStatus() )
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
	if( cwmWorldState->ServerData()->ServerBackupStatus() )
		Console << "Enabled]. (" << cwmWorldState->ServerData()->Directory( CSDDP_BACKUP ) << ")" << myendl;
	else 
		Console << "Disabled]" << myendl;
	
	Console << "   -Weapons & Armour Rank System: ";
	Console << activeMap[cwmWorldState->ServerData()->RankSystemStatus()] << myendl;
	
	Console << "   -Vendors buy by item name: ";
	Console << activeMap[cwmWorldState->ServerData()->SellByNameStatus()] << myendl;
	
	Console << "   -Adv. Trade System: ";
	Console << activeMap[cwmWorldState->ServerData()->TradeSystemStatus()] << myendl;
	
	Console << "   -Crash Protection: ";
	if( cwmWorldState->ServerData()->ServerCrashProtectionStatus() < 1 )
		Console << "Disabled!" << myendl;
#ifndef _CRASH_PROTECT_
	else 
		Console << "Unavailable in this version" << myendl;
#else
	else if( cwmWorldState->ServerData()->ServerCrashProtectionStatus() == 1 )
		Console << "Save on crash" << myendl;
	else 
		Console << "Save & Restart Server" << myendl;
#endif

	Console << "   -xGM Remote: ";
	Console << activeMap[cwmWorldState->GetXGMEnabled()] << myendl;

	Console << "   -Races: " << static_cast< UI32 >(Races->Count()) << myendl;
	Console << "   -Guilds: " << static_cast< UI32 >(GuildSys->NumGuilds()) << myendl;
	Console << "   -Char count: " << ObjectFactory::getSingleton().SizeOfObjects( OT_CHAR ) << myendl;
	Console << "   -Item count: " << ObjectFactory::getSingleton().SizeOfObjects( OT_ITEM ) << myendl;
	Console << "   -Num Accounts: " << Accounts->size() << myendl;
	Console << "   Directories: " << myendl;
	Console << "   -Shared:          " << cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) << myendl;
	Console << "   -Archive:         " << cwmWorldState->ServerData()->Directory( CSDDP_BACKUP ) << myendl;
	Console << "   -Data:            " << cwmWorldState->ServerData()->Directory( CSDDP_DATA ) << myendl;
	Console << "   -Defs:            " << cwmWorldState->ServerData()->Directory( CSDDP_DEFS ) << myendl;
	Console << "   -Scripts:         " << cwmWorldState->ServerData()->Directory( CSDDP_SCRIPTS ) << myendl;
	Console << "   -HTML:            " << cwmWorldState->ServerData()->Directory( CSDDP_HTML ) << myendl;
	Console << "   -Books:           " << cwmWorldState->ServerData()->Directory( CSDDP_BOOKS ) << myendl;
	Console << "   -MessageBoards:   " << cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD ) << myendl;
}

void UnloadSpawnRegions( void )
{
	for( size_t mySpReg = 0; mySpReg < spawnregions.size(); ++mySpReg )
	{
		if( spawnregions[mySpReg] != NULL )
		{
			delete spawnregions[mySpReg];
			spawnregions[mySpReg] = NULL;
		}
	}
}

void UnloadRegions( void )
{
	for( size_t myReg = 0; myReg < regions.size(); ++myReg )
	{
		if( regions[myReg] != NULL )
		{
			delete regions[myReg];
			regions[myReg] = NULL;
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void processkey( int c )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Handle keypresses in console
//|	Modification	-	05042004 - EviLDeD added some console debugging stuff.
//|									Ideally this will all be placed onto a DEBUG menu which
//|									a user will be able to select using the <, and > keys
//|									respectivly. This functionality hasn't been implemented
//|									at the current time of writing, but will be when possible.
//o---------------------------------------------------------------------------o

void processkey( int c )
{
	char outputline[128], temp[1024];
	bool kill = false;
	int indexcount = 0;
	int j;
	int keyresp;
	CSocket *tSock;

	if( c == '*' )
	{
		if( cwmWorldState->GetSecure() )
			messageLoop << "Secure mode disabled. Press ? for a commands list";
		else
			messageLoop << "Secure mode re-enabled";
		cwmWorldState->SetSecure( !cwmWorldState->GetSecure() );
		return;
	} 
	else 
	{
		if( cwmWorldState->GetSecure() )
		{
			messageLoop << "Secure mode prevents keyboard commands! Press '*' to disable";
			return;
		}
		
		switch( c )
		{
			case '!':
				// Force server to save accounts file
				messageLoop << "CMD: Saving Accounts... ";
				Accounts->Save();
				messageLoop << MSG_PRINTDONE;
				break;
			case '@':
				// Force server to save all files.(Manual save)
				messageLoop << MSG_WORLDSAVE;
				break;
		case 'Y':
//#if defined( _MSC_VER )
//#pragma note("Console Broadcast needs to not require cout anymore.  Is there a better way?")
//#endif
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
						messageLoop << "CMD: System broadcast canceled.";
						break;
					case 0x08:
						--indexcount;
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
						sprintf( temp, "CMD: System broadcast sent message \"%s\"", outputline );
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
			case '[':
			{
				// We want to group all the contents of the multimap container numerically by group. We rely on the self ordering in the multimap implementation to do this.
				messageLoop << "  ";
				messageLoop << "Auto-AddMenu Statistics";
				messageLoop << "  ";
				char szBuffer[128];
				// We need to get an iteration into the map first of all the top level ULONGs then we can get an equal range.
				std::map<ULONG,UI08> localMap;
				localMap.clear();
				for(ADDMENUMAP_CITERATOR CJ=g_mmapAddMenuMap.begin();CJ != g_mmapAddMenuMap.end();CJ++)
				{
					// check to see if the group id has been checked already
					if(localMap.find(CJ->first)==localMap.end())
					{
						localMap.insert(std::make_pair(CJ->first,0));
						memset(szBuffer,0x00,sizeof(szBuffer));
						sprintf(szBuffer,"AddMenuGroup %i:",CJ->first);
						messageLoop << szBuffer;
						std::pair<ADDMENUMAP_CITERATOR,ADDMENUMAP_CITERATOR> pairRange = g_mmapAddMenuMap.equal_range( CJ->first );
						int count=0;
						for(ADDMENUMAP_CITERATOR CI=pairRange.first;CI != pairRange.second; CI++)
						{
							count++;
						}
						memset(szBuffer,0x00,sizeof(szBuffer));
						sprintf(szBuffer,"   Found %i Auto-AddMenu Item(s).",count);
						messageLoop << szBuffer;
					}
				}
				messageLoop << MSG_SECTIONBEGIN;
				break;
			}
			case '<':
				messageLoop << "Function not implemented.";
				break;
			case '>':
				messageLoop << "Function not implemented.";
				break;
			case 0x1B:
			case 'Q':
				messageLoop << MSG_SECTIONBEGIN;
				messageLoop << "CMD: Immediate Shutdown initialized!";
				messageLoop << MSG_SHUTDOWN;
				break;
			case '1':
				// Reload server ini file
				if( !cwmWorldState->GetReloadingScripts() )
				{
					messageLoop << "CMD: Loading Server INI... ";
					cwmWorldState->SetReloadingScripts( true );
					cwmWorldState->ServerData()->load();
					cwmWorldState->SetReloadingScripts( false );
					messageLoop << MSG_PRINTDONE;
				}
				else
					messageLoop << "Server can only load one script at a time";
				break;
			case '2':
				// Reload accounts, and update Access.adm if new accounts available.
				if( !cwmWorldState->GetReloadingScripts() )
				{
					messageLoop << "CMD: Loading Accounts... ";
					cwmWorldState->SetReloadingScripts( true );
					Accounts->Load();
					cwmWorldState->SetReloadingScripts( false );
					messageLoop << MSG_PRINTDONE;
				}
				else
					messageLoop << "Server can only load one script at a time";
				break;
			case '3':
				// Reload Region Files
				if( !cwmWorldState->GetReloadingScripts() )
				{
					messageLoop << "CMD: Loading Regions... ";
					cwmWorldState->SetReloadingScripts( true );
					UnloadRegions();
					LoadRegions();
					cwmWorldState->SetReloadingScripts( false );
					messageLoop << MSG_PRINTDONE;
				}
				else
					messageLoop << "Server can only load one script at a time";
				break;
			case '4':
				// Reload the serve spawn regions
				if( !cwmWorldState->GetReloadingScripts() )
				{
					messageLoop << "CMD: Loading Spawn Regions... ";
					cwmWorldState->SetReloadingScripts( true );
					UnloadSpawnRegions();
					LoadSpawnRegions();
					cwmWorldState->SetReloadingScripts( false );
					messageLoop << MSG_PRINTDONE;
				}
				else
					messageLoop << "Server can only load one script at a time";
				break;
			case '5':
				// Reload the current Spells
				if( !cwmWorldState->GetReloadingScripts() )
				{
					messageLoop << "CMD: Loading spells... ";
					cwmWorldState->SetReloadingScripts( true );
					Magic->LoadScript();
					cwmWorldState->SetReloadingScripts( false );
					messageLoop << MSG_PRINTDONE;
				}
				else
					messageLoop << "Server can only load one script at a time";
				break;
			case '6':
				// Reload the server command list
				if( !cwmWorldState->GetReloadingScripts() )
				{
					messageLoop << "CMD: Loading commands... ";
					cwmWorldState->SetReloadingScripts( true );
					Commands->Load();
					cwmWorldState->SetReloadingScripts( false );
					messageLoop << MSG_PRINTDONE;
				}
				else
					messageLoop << "Server can only load one script at a time";
				break;
			case '7':
				// Reload the server defantion files
				if( !cwmWorldState->GetReloadingScripts() )
				{
					messageLoop << "CMD: Loading Server DFN... ";
					cwmWorldState->SetReloadingScripts( true );
					FileLookup->Reload();
					cwmWorldState->SetReloadingScripts( false );
					messageLoop << MSG_PRINTDONE;
				}
				else
					messageLoop << "Server can only load one script at a time";
				break;
			case '8':
				// messageLoop access is REQUIRED, as this function is executing in a different thread, so we need thread safety
				if( !cwmWorldState->GetReloadingScripts() )
				{
					messageLoop << "CMD: Loading JSE Scripts... ";
					cwmWorldState->SetReloadingScripts( true );
					messageLoop << MSG_RELOADJS;
					cwmWorldState->SetReloadingScripts( false );
				}
				else
					messageLoop << "Server can only load one script at a time";
				break;
			case '9':
				// Reload the HTML output templates
				if( !cwmWorldState->GetReloadingScripts() )
				{
					messageLoop << "CMD: Loading HTML Templates... ";
					cwmWorldState->SetReloadingScripts( true );
					HTMLTemplates->Unload();
					HTMLTemplates->Load();
					cwmWorldState->SetReloadingScripts( false );
					messageLoop << MSG_PRINTDONE;
				}
				else
					messageLoop << "Server can only load one script at a time";
				break;
			case '0':
				if( !cwmWorldState->GetReloadingScripts() )
				{
					cwmWorldState->SetReloadingScripts( true );
					// Reload all the files. If there are issues with these files change the order reloaded from here first.
					cwmWorldState->ServerData()->load();
					messageLoop << "CMD: Loading All";
					messageLoop << "     Server INI... ";
					// Reload accounts, and update Access.adm if new accounts available.
					messageLoop << "     Loading Accounts... ";
					Accounts->Load();
					messageLoop << MSG_PRINTDONE;
					// Reload Region Files
					messageLoop << "     Loading Regions... ";
					UnloadRegions();
					LoadRegions();
					messageLoop << MSG_PRINTDONE;
					// Reload the serve spawn regions
					messageLoop << "     Loading Spawn Regions... ";
					UnloadSpawnRegions();
					LoadSpawnRegions();
					messageLoop << MSG_PRINTDONE;
					// Reload the server command list
					messageLoop << "     Loading commands... ";
					Commands->Load();
					messageLoop << MSG_PRINTDONE;
					// Reload DFN's
					messageLoop << "     Loading Server DFN... ";
					FileLookup->Reload();
					messageLoop << MSG_PRINTDONE;
					// messageLoop access is REQUIRED, as this function is executing in a different thread, so we need thread safety
					messageLoop << "     Loading JSE Scripts... ";
					messageLoop << MSG_RELOADJS;
					// Reload the current Spells 
					messageLoop << "     Loading spells... ";
					Magic->LoadScript();
					messageLoop << MSG_PRINTDONE;
					// Reload the HTML output templates
					messageLoop << "     Loading HTML Templates... ";
					HTMLTemplates->Unload();
					HTMLTemplates->Load();
					cwmWorldState->SetReloadingScripts( false );
					messageLoop << MSG_PRINTDONE;
				}
				else
					messageLoop << "Server can only load one script at a time";
				break;
			case 'T':
				// Timed shut down(10 minutes)
				messageLoop << "CMD: 10 Minute Server Shutdown Announced(Timed)";
				cwmWorldState->SetEndTime( BuildTimeValue( 600 ) );
				endmessage(0);
				break;
			case  'D':    
				// Disconnect account 0 (useful when client crashes)
				for( tSock = Network->LastSocket(); tSock != NULL; tSock = Network->PrevSocket() )
				{
					if( tSock->AcctNo() == 0 )
						Network->Disconnect( tSock );
				}
				messageLoop << "CMD: Socket Disconnected(Account 0).";
				break;
			case 'K':
			{
				for( tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
				{
					Network->Disconnect( tSock );
				}
				messageLoop << "CMD: All Connections Closed.";
			}
				break;
			case 'P':
				{
				UI32 networkTimeCount	= cwmWorldState->ServerProfile()->NetworkTimeCount();
				UI32 timerTimeCount		= cwmWorldState->ServerProfile()->TimerTimeCount();
				UI32 autoTimeCount		= cwmWorldState->ServerProfile()->AutoTimeCount();
				UI32 loopTimeCount		= cwmWorldState->ServerProfile()->LoopTimeCount();
				// 1/13/2003 - Dreoth - Log Performance Information enhancements
				Console.LogEcho( true );
				Console.Log( "--- Starting Performance Dump ---", "performance.log");
				Console.Log( "\tPerformace Dump:", "performance.log");
				Console.Log( "\tNetwork code: %.2fmsec [%i samples]", "performance.log", (R32)((R32)cwmWorldState->ServerProfile()->NetworkTime()/(R32)networkTimeCount), networkTimeCount);
				Console.Log( "\tTimer code: %.2fmsec [%i samples]", "performance.log", (R32)((R32)cwmWorldState->ServerProfile()->TimerTime()/(R32)timerTimeCount), timerTimeCount);
				Console.Log( "\tAuto code: %.2fmsec [%i samples]", "performance.log", (R32)((R32)cwmWorldState->ServerProfile()->AutoTime()/(R32)autoTimeCount), autoTimeCount);
				Console.Log( "\tLoop Time: %.2fmsec [%i samples]", "performance.log", (R32)((R32)cwmWorldState->ServerProfile()->LoopTime()/(R32)loopTimeCount), loopTimeCount);
				ObjectFactory *ourFac = ObjectFactory::getSingletonPtr();
				Console.Log( "\tCharacters: %i/%i - Items: %i/%i (Dynamic)", "performance.log", ourFac->CountOfObjects( OT_CHAR ), ourFac->SizeOfObjects( OT_CHAR ), ourFac->CountOfObjects( OT_ITEM ), ourFac->SizeOfObjects( OT_ITEM ) );
				Console.Log( "\tSimulation Cycles: %f per sec", "performance.log", (1000.0*(1.0/(R32)((R32)cwmWorldState->ServerProfile()->LoopTime()/(R32)loopTimeCount))));
				Console.Log( "\tBytes sent: %i", "performance.log", cwmWorldState->ServerProfile()->GlobalSent());
				Console.Log( "\tBytes Received: %i", "performance.log", cwmWorldState->ServerProfile()->GlobalReceived());
				Console.Log( "--- Performance Dump Complete ---", "performance.log");
				Console.LogEcho( false );
				break;
				}
			case 'W':                
				// Display logged in chars
				messageLoop << "CMD: Current Users in the World:";
				j = 0;
				CSocket *iSock;
				Network->PushConn();
				for( iSock = Network->FirstSocket(); !Network->FinishedSockets(); iSock = Network->NextSocket() )
				{
					++j;
					CChar *mChar = iSock->CurrcharObj();
					sprintf( temp, "     %i) %s [%x %x %x %x]", j - 1, mChar->GetName().c_str(), mChar->GetSerial( 1 ), mChar->GetSerial( 2 ), mChar->GetSerial( 3 ), mChar->GetSerial( 4 ) );
					messageLoop << temp;
				}
				Network->PopConn();
				sprintf( temp, "     Total users online: %i", j );
				messageLoop << temp;
				break;
			case 'M':
				size_t tmp, total;
				total = 0;
				tmp = 0;
				messageLoop << "CMD: UOX Memory Information:";
				messageLoop << "     Cache:";
				sprintf( temp, "        Tiles: %i bytes", Map->TileMem );
				messageLoop << temp;
				sprintf( temp, "        Statics: %i bytes", Map->StaMem );
				messageLoop << temp;
				sprintf( temp, "        Version: %i bytes", Map->versionMemory );
				messageLoop << temp;
				sprintf( temp, "        Map0: %i bytes [%i Hits - %i Misses]", 9*MAP0CACHE, Map->Map0CacheHit, Map->Map0CacheMiss );
				messageLoop << temp;
				size_t m, n;
				m = ObjectFactory::getSingleton().SizeOfObjects( OT_CHAR );
				total += tmp = m + m*sizeof( CTEffect ) + m*sizeof(char) + m*sizeof(int)*5;
				sprintf( temp, "     Characters: %u bytes [%u chars ( %u allocated )]", tmp, ObjectFactory::getSingleton().CountOfObjects( OT_CHAR ), m );
				messageLoop << temp;
				n = ObjectFactory::getSingleton().SizeOfObjects( OT_ITEM );
				total += tmp = n + n * sizeof( int ) * 4;
				sprintf( temp, "     Items: %u bytes [%u items ( %u allocated )]", tmp, ObjectFactory::getSingleton().CountOfObjects( OT_ITEM ), n );
				messageLoop << temp;
				sprintf( temp, "        You save I: %i & C: %i bytes!", m * sizeof(CItem) - ObjectFactory::getSingleton().CountOfObjects( OT_ITEM ), m * sizeof( CChar ) - ObjectFactory::getSingleton().CountOfObjects( OT_CHAR ) );
				total += tmp = 69 * sizeof( SpellInfo );
				sprintf( temp, "     Spells: %i bytes", tmp );
				messageLoop << "     Sizes:";
				sprintf( temp, "        CItem  : %i bytes", sizeof( CItem ) );
				messageLoop << temp;
				sprintf( temp, "        CChar  : %i bytes", sizeof( CChar ) );
				messageLoop << temp;
				sprintf( temp, "        TEffect: %i bytes (%i total)", sizeof( CTEffect ), sizeof( CTEffect ) * TEffects->Count() );
				messageLoop << temp;
				total += tmp = Map->TileMem + Map->StaMem + Map->versionMemory;
				sprintf( temp, "        Approximate Total: %i bytes", total );
				messageLoop << temp;
				break;
			case '?':
				messageLoop << MSG_SECTIONBEGIN;
				messageLoop << "Console commands:";
				messageLoop << MSG_SECTIONBEGIN;
				messageLoop << " ShardOP:";
				messageLoop << "    * - Lock/Unlock Console ? - Commands list(this)";
				messageLoop << "    C - Configuration       H - Unused";
				messageLoop << "    Y - Console Broadcast   Q - Quit/Exit           ";
				messageLoop << " Load Commands:";
				messageLoop << "    1 - Ini                 2 - Accounts";
				messageLoop << "    3 - Regions             4 - Spawn Regions";
				messageLoop << "    5 - Spells              6 - Commands";
				messageLoop << "    7 - Dfn's               8 - JavaScript";
				messageLoop << "    9 - HTML Templates      0 - ALL(1-9)";
				messageLoop << " Save Commands:";
				messageLoop << "    ! - Accounts            @ - World(w/AccountImport)";
				messageLoop << "    # - Unused              $ - Unused";
				messageLoop << "    % - Unused              ^ - Unused";
				messageLoop << "    & - Unused              ( - Unused";
				messageLoop << "    ) - Unused";
				messageLoop << " Server Maintenence:";
				messageLoop << "    P - Performance         W - Characters Online";
				messageLoop << "    M - Memory Information  T - 10 Minute Shutdown";
				messageLoop << "    V - Dump Lookups(Devs)  F - Display Priority Maps";
				messageLoop << " Network Maintenence:";
				messageLoop << "    D - Disconnect Acct0    K - Disconnect All";
				messageLoop << "    Z - Socket Logging      ";
				messageLoop << MSG_SECTIONBEGIN;
				break;
			case 'v':
			case 'V':
				// Dump look up data to files so developers working with extending the ini will have a table to use
				messageLoop << "| CMD: Creating Server.scp and Uox3.ini Tag Lookup files(For Developers)....";
				cwmWorldState->ServerData()->dumpLookup( 0 );
				cwmWorldState->ServerData()->save( "./uox.tst.ini" );
				messageLoop << MSG_PRINTDONE;
				break;
			case 'z':
			case 'Z':
			{
				// Log socket activity
				Network->PushConn();
				bool loggingEnabled = false;
				CSocket *snSock = Network->FirstSocket();
				if( snSock != NULL )
					loggingEnabled = !snSock->Logging();
				for( ; !Network->FinishedSockets(); snSock = Network->NextSocket() )
				{
					if( snSock != NULL )
						snSock->Logging( !snSock->Logging() );
				}
				Network->PopConn();
				if( loggingEnabled )
					messageLoop << "CMD: Network Logging Enabled.";
				else
					messageLoop << "CMD: Network Logging Disabled.";
				break;
			}
			case 'c':
			case 'C':
				// Shows a configuration header
				DisplaySettings();
				break;
			case 'f':
			case 'F':
				FileLookup->DisplayPriorityMap();
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
			if( cwmWorldState->GetSecure() )
			{
				Console << "Secure mode disabled. Press ? for a commands list." << myendl;
				cwmWorldState->SetSecure( false );
				return;
			}
		}
		c = toupper(c);
		processkey( c );
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool genericCheck( CSocket *mSock, CChar *mChar )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check characters status.  Returns true if character was killed
//o---------------------------------------------------------------------------o
bool genericCheck( CSocket *mSock, CChar& mChar, bool checkFieldEffects, bool checkHunger )
{
	if( mChar.IsDead() )
		return false;

	UI16 c;
	if( mChar.GetHP() > mChar.GetMaxHP() )
		mChar.SetHP( mChar.GetMaxHP() );
	if( mChar.GetStamina() > mChar.GetMaxStam() )
		mChar.SetStamina( mChar.GetMaxStam() );
	if( mChar.GetMana() > mChar.GetMaxMana() )
		mChar.SetMana( mChar.GetMaxMana() );

	if( mChar.GetRegen( 0 ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
	{
		if( mChar.GetHP() < mChar.GetMaxHP() && ( mChar.GetHunger() > (SI16)cwmWorldState->ServerData()->HungerThreshold() || cwmWorldState->ServerData()->HungerRate() == 0 ) )
		{
			for( c = 0; c < mChar.GetMaxHP() + 1; ++c )
			{
				if( mChar.GetHP() <= mChar.GetMaxHP() && ( mChar.GetRegen( 0 ) + ( c * cwmWorldState->ServerData()->SystemTimer( HITPOINT_REGEN ) * 1000) ) <= cwmWorldState->GetUICurrentTime() )
				{
					if( mChar.GetSkill( HEALING ) < 500 )
						mChar.IncHP( 1 );
					else if( mChar.GetSkill( HEALING ) < 800 )
						mChar.IncHP( 2 );
					else 
						mChar.IncHP( 3 );
					if( mChar.GetHP() >= mChar.GetMaxHP() )
					{
						mChar.SetHP( mChar.GetMaxHP() ); 
						break;
					}
				}
				else			// either we're all healed up, or all time periods have passed
					break;
			}
		}
		mChar.SetRegen( BuildTimeValue( (R32)cwmWorldState->ServerData()->SystemTimer( HITPOINT_REGEN ) ), 0 );
	}
	if( mChar.GetRegen( 1 ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
	{
		if( mChar.GetStamina() < mChar.GetMaxStam() )
		{
			for( c = 0; c < mChar.GetMaxStam() + 1; ++c )
			{
				if( ( mChar.GetRegen( 1 ) + ( c * cwmWorldState->ServerData()->SystemTimer( STAMINA_REGEN ) * 1000) ) <= cwmWorldState->GetUICurrentTime() && mChar.GetStamina() <= mChar.GetMaxStam() )
				{
					mChar.IncStamina( 1 );
					if( mChar.GetStamina() >= mChar.GetMaxStam() )
					{
						mChar.SetStamina( mChar.GetMaxStam() );
						break;
					}
				}
				else
					break;
			}
		}
		mChar.SetRegen( BuildTimeValue( (R32)cwmWorldState->ServerData()->SystemTimer( STAMINA_REGEN ) ), 1 );
	}

	// CUSTOM START - SPUD:MANA REGENERATION:Rewrite of passive and active meditation code
	if( mChar.GetRegen( 2 ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
	{
		if( mChar.GetMana() < mChar.GetMaxMana() )
		{
			for( c = 0; c < mChar.GetMaxMana() + 1; ++c )
			{
				if( mChar.GetRegen( 2 ) + ( c * cwmWorldState->ServerData()->SystemTimer( MANA_REGEN ) * 1000) <= cwmWorldState->GetUICurrentTime() && mChar.GetMana() <= mChar.GetMaxMana() )
				{
					Skills->CheckSkill( (&mChar), MEDITATION, 0, 1000 );	// Check Meditation for skill gain ala OSI
					mChar.IncMana( 1 );	// Gain a mana point
					if( mChar.GetMana() == mChar.GetMaxMana() )
					{
						if( mChar.IsMeditating() ) // Morrolan = Meditation
						{
							if( mSock != NULL )
								mSock->sysmessage( 969 );
							mChar.SetMeditating( false );
						}
						break;
					}
				}
			}
		}
		R32 MeditationBonus = ( .00075f * mChar.GetSkill( MEDITATION ) );	// Bonus for Meditation
		int NextManaRegen = static_cast<int>(cwmWorldState->ServerData()->SystemTimer( MANA_REGEN ) * ( 1 - MeditationBonus ) * 1000);
		if( cwmWorldState->ServerData()->SystemTimer( ARMORAFFECTMANA_REGEN ) )	// If armor effects mana regeneration...
		{
			R32 ArmorPenalty = Combat->calcDef( (&mChar), 0, false );	// Penalty taken due to high def
			if( ArmorPenalty > 100 )	// For def higher then 100, penalty is the same...just in case
				ArmorPenalty = 100;
			ArmorPenalty = 1 + (ArmorPenalty / 25);
			NextManaRegen = static_cast<int>(NextManaRegen * ArmorPenalty);
		}
		if( mChar.IsMeditating() )	// If player is meditation...
			mChar.SetRegen( ( cwmWorldState->GetUICurrentTime() + ( NextManaRegen / 2 ) ), 2 );
		else
			mChar.SetRegen( ( cwmWorldState->GetUICurrentTime() + NextManaRegen ), 2 );
	}
	// CUSTOM END
	if( mChar.GetVisible() == VT_INVISIBLE && ( mChar.GetTimer( tCHAR_INVIS ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() ) )
		mChar.ExposeToView();
	if( mChar.WillHunger() && cwmWorldState->ServerData()->HungerRate() > 1 && ( mChar.GetTimer( tCHAR_HUNGER ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() ) )
	{
		if( mChar.GetHunger() > 0 && ( !mChar.IsCounselor() && !mChar.IsGM() ) )
			mChar.DecHunger(); //Morrolan GMs and Counselors don't get hungry
		
		UI16 HungerTrig = mChar.GetScriptTrigger();
		cScript *toExecute = Trigger->GetScript( HungerTrig );
		bool doHunger = true;
		if( toExecute != NULL )
			doHunger = !toExecute->OnHungerChange( (&mChar), mChar.GetHunger() );
		if( doHunger && mSock != NULL )
		{
			switch( mChar.GetHunger() )
			{
				case 6: break;
				case 5:	mSock->sysmessage( 1222 );	break;
				case 4:	mSock->sysmessage( 1223 );	break;
				case 3:	mSock->sysmessage( 1224 );	break;
				case 2:	mSock->sysmessage( 1225 );	break;
				case 1:	mSock->sysmessage( 1226 );	break;
				case 0:
					if( !mChar.IsCounselor() && !mChar.IsGM() )
						mSock->sysmessage( 1227 );
					break;	
			}
		}
		mChar.SetTimer( tCHAR_HUNGER, BuildTimeValue( (R32)cwmWorldState->ServerData()->HungerRate() ) );
	}
	if( checkHunger )
	{
		if( mChar.WillHunger() && mChar.GetHP() > 0 && mChar.GetHunger() < 2 && ( !mChar.IsCounselor() && !mChar.IsGM() ) && !mChar.IsDead() )
		{   
			if( mSock != NULL )
				mSock->sysmessage( 1228 );
			mChar.IncHP( (SI16)( -cwmWorldState->ServerData()->HungerDamage() ) );
			if( mChar.GetHP() <= 0 )
			{ 
				if( mSock != NULL )
					mSock->sysmessage( 1229 );
			}
		}
	}
	if( !mChar.IsInvulnerable() && mChar.GetPoisoned() )
	{
		if( mChar.GetTimer( tCHAR_POISONTIME ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
		{
			if( mChar.GetTimer( tCHAR_POISONWEAROFF ) > cwmWorldState->GetUICurrentTime() )
			{
				SI16 pcalc = 0;
				switch( mChar.GetPoisoned() )
				{
					case 1:
						mChar.SetTimer( tCHAR_POISONTIME, BuildTimeValue( 5 ) );
						if( mChar.GetTimer( tCHAR_POISONTEXT ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
						{
							mChar.SetTimer( tCHAR_POISONTEXT, BuildTimeValue( 10 ) );
							mChar.emoteAll( 1240, true, mChar.GetName().c_str() );
						}
						mChar.IncHP( (SI16)( -RandomNum( 1, 2 ) ) );
						break;
					case 2:
						mChar.SetTimer( tCHAR_POISONTIME, BuildTimeValue( 4 ) );
						if( mChar.GetTimer( tCHAR_POISONTEXT ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
						{
							mChar.SetTimer( tCHAR_POISONTEXT, BuildTimeValue( 10 ) );
							mChar.emoteAll( 1241, true, mChar.GetName().c_str() );
						}
						pcalc = (SI16)( ( mChar.GetHP() * RandomNum( 2, 5 ) / 100 ) + RandomNum( 0, 2 ) ); // damage: 1..2..5% of hp's+ 1..2 constant
						mChar.IncHP( (SI16)( -pcalc ) );
						break;
					case 3:
						mChar.SetTimer( tCHAR_POISONTIME, BuildTimeValue( 3 ) );
						if( mChar.GetTimer( tCHAR_POISONTEXT ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
						{
							mChar.SetTimer( tCHAR_POISONTEXT, BuildTimeValue( 10 ) );
							mChar.emoteAll( 1242, true, mChar.GetName().c_str() );
						}
						pcalc = (SI16)( ( mChar.GetHP() * RandomNum( 5, 10 ) / 100 ) + RandomNum( 1, 3 ) ); // damage: 5..10% of hp's+ 1..2 constant
						mChar.IncHP( (SI16)( -pcalc ) );
						break;
					case 4:
						mChar.SetTimer( tCHAR_POISONTIME, BuildTimeValue( 3 ) );
						if( mChar.GetTimer( tCHAR_POISONTEXT ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
						{
							mChar.SetTimer( tCHAR_POISONTEXT, BuildTimeValue( 10 ) );
							mChar.emoteAll( 1243, true, mChar.GetName().c_str() );
						}
						pcalc = (SI16)( mChar.GetHP() / 5 + RandomNum( 3, 6 ) ); // damage: 20% of hp's+ 3..6 constant, quite deadly <g>
						mChar.IncHP( (SI16)( -pcalc ) );
						break;
					default:
						Console.Error( 2, " Fallout of switch statement without default. uox3.cpp, genericCheck()" );
						mChar.SetPoisoned( 0 );
						break;
				}
				if( mChar.GetHP() < 1 )
				{
					doDeathStuff( (&mChar) );
					if( mSock != NULL )
						mSock->sysmessage( 1244 );
				} 
			}
		}
	}

	if( mChar.GetTimer( tCHAR_POISONWEAROFF ) <= cwmWorldState->GetUICurrentTime() )
	{
		if( mChar.GetPoisoned() )
		{
            mChar.SetPoisoned( 0 );
			if( mSock != NULL )
				mSock->sysmessage( 1245 );
		}
	}

	if( !mChar.IsNpc() && mSock != NULL )
	{
		doLightEffect( (*mSock), mChar );
		doWeatherEffect( (*mSock), mChar, RAIN );
		doWeatherEffect( (*mSock), mChar, SNOW );
		doWeatherEffect( (*mSock), mChar, HEAT );
		doWeatherEffect( (*mSock), mChar, COLD );
	}

	if( checkFieldEffects )
		Magic->CheckFieldEffects( mChar );
	if( mChar.IsDead() )
		return true;
	else if( mChar.GetHP() <= 0 )
	{
		doDeathStuff( (&mChar) );
		return true;
	}
	return false;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void checkPC( CChar *mChar, bool doWeather )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check a PC's status
//o---------------------------------------------------------------------------o
void checkPC( CSocket *mSock, CChar *mChar, bool doWeather )
{
	LIGHTLEVEL toShow;
	Combat->CombatLoop( mSock, mChar );
	
	if( doWeather )
	{
		UI08 curLevel = cwmWorldState->ServerData()->WorldLightCurrentLevel();
		if( Races->VisLevel( mChar->GetRace() ) > curLevel )
			toShow = 0;
		else
			toShow = static_cast<UI08>( curLevel - Races->VisLevel( mChar->GetRace() ) );

		doLight( mSock, toShow );
		Weather->DoPlayerStuff( mSock, mChar );
	}
	
	if( mChar->GetSquelched() == 2 )
	{
		if( mSock->GetTimer( tPC_MUTETIME ) != 0 && ( mSock->GetTimer( tPC_MUTETIME ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() ) )
		{
			mChar->SetSquelched( 0 );
			mSock->SetTimer( tPC_MUTETIME, 0 );
			mSock->sysmessage( 1237 );
		}
	}
	
	if( mChar->IsCriminal() && mChar->GetTimer( tCHAR_CRIMFLAG ) && ( mChar->GetTimer( tCHAR_CRIMFLAG ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() ) )
	{
		if( mSock != NULL )
			mSock->sysmessage( 1238 );
		mChar->SetTimer( tCHAR_CRIMFLAG, 0 );
		setcharflag( mChar );
	}
	if( mChar->GetTimer( tCHAR_MURDERRATE ) && ( mChar->GetTimer( tCHAR_MURDERRATE ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() ) )
	{
		mChar->SetTimer( tCHAR_MURDERRATE, 0 );
		if( mChar->GetKills() )
			mChar->SetKills( static_cast<SI16>( mChar->GetKills() - 1 ) );
		if( mChar->GetKills() )
			mChar->SetTimer( tCHAR_MURDERRATE, BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->RepMurderDecay() ) ) );
		else
			mChar->SetTimer( tCHAR_MURDERRATE, 0 );
		if( mChar->GetKills() == cwmWorldState->ServerData()->RepMaxKills() )
			mSock->sysmessage( 1239 );
		setcharflag( mChar );
	}
	
	if( mChar->IsCasting() && !mChar->IsJSCasting() )	// Casting a spell
	{
		mChar->SetNextAct( mChar->GetNextAct() - 1 );
		if( mChar->GetTimer( tCHAR_SPELLTIME ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )//Spell is complete target it.
		{
			if( Magic->spells[mChar->GetSpellCast()].RequireTarget() )
			{
				mChar->SetCasting( false );
				mChar->SetFrozen( false );
				mSock->target( 0, TARGET_CASTSPELL, Magic->spells[mChar->GetSpellCast()].StringToSay().c_str() );
			}
			else
			{
				mChar->SetCasting( false );
				Magic->CastSpell( mSock, mChar );
				mChar->SetTimer( tCHAR_SPELLTIME, 0 );
				mChar->SetFrozen( false );
			}
		} 
		else if( mChar->GetNextAct() <= 0 )//redo the spell action
		{
			mChar->SetNextAct( 75 );
			if( !mChar->IsOnHorse() )
				Effects->PlaySpellCastingAnimation( mChar, Magic->spells[mChar->GetSpellCast()].Action() );
		}
	}
	
	if( cwmWorldState->ServerData()->WorldAmbientSounds() >= 1 )
	{
		if( cwmWorldState->ServerData()->WorldAmbientSounds() > 10 )
			cwmWorldState->ServerData()->WorldAmbientSounds( 10 );
		SI16 soundTimer = static_cast<SI16>(cwmWorldState->ServerData()->WorldAmbientSounds() * 100);
		if( !mChar->IsDead() && ( RandomNum( 0, soundTimer - 1 ) ) == ( soundTimer / 2 ) )
			Effects->PlayBGSound( (*mSock), (*mChar) ); // bgsound uses array positions not sockets!
	}
	
	if( mSock->GetTimer( tPC_SPIRITSPEAK ) > 0 && mSock->GetTimer( tPC_SPIRITSPEAK) < cwmWorldState->GetUICurrentTime() )
		mSock->SetTimer( tPC_SPIRITSPEAK, 0 );
	
	if( mSock->GetTimer( tPC_TRACKING ) > cwmWorldState->GetUICurrentTime() )
	{
		if( mSock->GetTimer( tPC_TRACKINGDISPLAY ) <= cwmWorldState->GetUICurrentTime() )
		{
			mSock->SetTimer( tPC_TRACKINGDISPLAY, BuildTimeValue( (R32)cwmWorldState->ServerData()->TrackingRedisplayTime() ) );
			Skills->Track( mChar );
		}
	}
	else
	{
		if( mSock->GetTimer( tPC_TRACKING ) > ( cwmWorldState->GetUICurrentTime() / 10 ) ) // dont send arrow-away packet all the time
		{
			mSock->SetTimer( tPC_TRACKING, 0 );
			CPTrackingArrow tSend = (*mChar->GetTrackingTarget());
			tSend.Active( 0 );
			mSock->Send( &tSend );
		}
	}
	
	if( mSock->GetTimer( tPC_FISHING ) )
	{
		if( mSock->GetTimer( tPC_FISHING ) <= cwmWorldState->GetUICurrentTime() )
		{
			Skills->Fish( mChar );
			mSock->SetTimer( tPC_FISHING, 0 );
		}
	}
	
	if( mChar->IsOnHorse() )
	{
		CItem *horseItem = mChar->GetItemAtLayer( IL_MOUNT );
		if( !ValidateObject( horseItem ) )
			mChar->SetOnHorse( false );	// turn it off, we aren't on one because there's no item!
		else if( horseItem->GetDecayTime() != 0 && ( horseItem->GetDecayTime() <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() ) )
		{
			mChar->SetOnHorse( false );
			horseItem->Delete();
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void checkNPC( CChar *i )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check NPC's status
//o---------------------------------------------------------------------------o
void checkNPC( CChar *i, bool checkAI, bool doRestock )
{
	if( !ValidateObject( i ) || i->isFree() )
		return;
	// okay, this will only ever trigger after we check an npc...  Question is:
	// should we remove the time delay on the AI check as well?  Just stick with AI/movement
	// AI can never be faster than how often we check npcs
	// This periodically generates access violations.  No idea why either
	UI16 AITrig			= i->GetScriptTrigger();
	cScript *toExecute	= Trigger->GetScript( AITrig );
	bool doAICheck		= true;
	if( toExecute != NULL )
	{
		if( toExecute->OnAISliver( i ) )
			doAICheck = false;
	}
	if( doAICheck && checkAI )
		CheckAI( i );
	Movement->NpcMovement( i );

	if( doRestock )
		restockNPC( (*i), false );

	if( ValidateObject( i->GetOwnerObj() ) && i->GetHunger() == 0 && i->GetNPCAiType() != aiPLAYERVENDOR ) // tamed animals but not player vendors ;)=
	{
		Effects->tempeffect( i, i, 44, 0, 0, 0 ); // (go wild in some minutes ...)-effect
		i->SetHunger( -1 );
	}
	
	if( i->GetTimer( tNPC_SUMMONTIME ) )
	{
		if( i->GetTimer( tNPC_SUMMONTIME ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
		{
			// Dupois - Added Dec 20, 1999
			// QUEST expire check - after an Escort quest is created a timer is set
			// so that the NPC will be deleted and removed from the game if it hangs around
			// too long without every having its quest accepted by a player so we have to remove 
			// its posting from the messageboard before icing the NPC
			// Only need to remove the post if the NPC does not have a follow target set
			if( i->GetQuestType() == ESCORTQUEST && !ValidateObject( i->GetFTarg() ) )
			{
				MsgBoardQuestEscortRemovePost( i );
				MsgBoardQuestEscortDelete( i );
				return;
			}
			// Dupois - End
			if( i->GetNPCAiType() == aiGUARD && i->IsAtWar() )
			{
				i->SetTimer( tNPC_SUMMONTIME, BuildTimeValue( 25 ) );
				return;
			}
			Effects->PlaySound( i, 0x01FE );
			i->SetDead( true );
			i->Delete();
			return;
		}
	}
	
	if( i->GetFleeAt() == 0 )
		i->SetFleeAt( cwmWorldState->ServerData()->CombatNPCBaseFleeAt() );
	if( i->GetReattackAt() == 0 )
		i->SetReattackAt( cwmWorldState->ServerData()->CombatNPCBaseReattackAt() );
	
	if( i->GetNpcWander() != 5 && ( i->GetHP() < i->GetStrength() * i->GetFleeAt() / 100 ) )
	{
		i->SetOldNpcWander( i->GetNpcWander() );
		i->SetNpcWander( 5 );
		i->SetTimer( tNPC_MOVETIME, BuildTimeValue( (R32)( cwmWorldState->ServerData()->NPCSpeed() / 2 ) ) );	// fleeing enemies are 2x faster
	}
	else if( i->GetNpcWander() == 5 && (i->GetHP() > i->GetStrength() * i->GetReattackAt() / 100))
	{
		i->SetNpcWander( i->GetOldNpcWander() );
		i->SetTimer( tNPC_MOVETIME, BuildTimeValue( (R32)cwmWorldState->ServerData()->NPCSpeed() ) );
		i->SetOldNpcWander( 0 ); // so it won't save this at the wsc file
	}
	Combat->CombatLoop( NULL, i );
}

void checkItem( SubRegion *toCheck, bool checkItems, UI32 nextDecayItems )
{
	CDataList< CItem * > *regItems = toCheck->GetItemList();
	regItems->Push();
	for( CItem *itemCheck = regItems->First(); !regItems->Finished(); itemCheck = regItems->Next() )
	{
		if( !ValidateObject( itemCheck ) || itemCheck->isFree() )
			continue;
		if( checkItems )
		{
			if( itemCheck->isDecayable() && itemCheck->GetCont() == NULL )
			{
				if( itemCheck->GetDecayTime() <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
				{
					if( DecayItem( (*itemCheck), nextDecayItems ) )
						continue;
				}
			}
			switch( itemCheck->GetType() )
			{
			case IT_GATE:
			case IT_ENDGATE:
				if( itemCheck->GetGateTime() <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
				{
					itemCheck->Delete();
					continue;
				}
				break;
			case IT_ITEMSPAWNER:
			case IT_NPCSPAWNER:
			case IT_SPAWNCONT:
			case IT_LOCKEDSPAWNCONT:
			case IT_UNLOCKABLESPAWNCONT:
			case IT_AREASPAWNER:
			case IT_ESCORTNPCSPAWNER:
				if( itemCheck->GetGateTime() <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
				{
					if( itemCheck->GetObjType() == OT_SPAWNER )
					{
						CSpawnItem *spawnItem = static_cast<CSpawnItem *>(itemCheck);
						if( spawnItem->DoRespawn() )
						continue;
						spawnItem->SetGateTime( BuildTimeValue( static_cast<R32>(RandomNum( spawnItem->GetInterval( 0 ) * 60, spawnItem->GetInterval( 1 ) * 60 ) ) ) );
					}
					else
						itemCheck->SetType( IT_NOTYPE );
				}
				break;
			case IT_SOUNDOBJECT:
				if( itemCheck->GetTempVar( CITV_MOREY ) < 25 )
				{
					if( RandomNum( 1, 100 ) <= (SI32)itemCheck->GetTempVar( CITV_MOREZ ) )
					{
						SOCKLIST nearbyChars = FindNearbyPlayers( itemCheck, static_cast<UI16>(itemCheck->GetTempVar( CITV_MOREY )) );
						for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
						{
							Effects->PlaySound( (*cIter), static_cast<UI16>(itemCheck->GetTempVar( CITV_MOREX )), false );
						}
					}
				}
				break;
			default:
				break;
			}
		} 
		if( itemCheck->GetType() == IT_PLANK )
		{
			CBoatObj *mBoat = static_cast<CBoatObj *>(itemCheck);
			if( ValidateObject( mBoat ) && mBoat->GetMoveType() && 
				( mBoat->GetMoveTime() <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() ) )
			{
				if( mBoat->GetMoveType() == 1 )
					MoveBoat( itemCheck->GetDir(), mBoat );
				else 
				{
					UI08 dir = (UI08)( itemCheck->GetDir() + 4 );
					if( dir > 7 )
						dir %= 8;
					MoveBoat( dir, mBoat );
				}
				mBoat->SetMoveTime( BuildTimeValue( (R32)cwmWorldState->ServerData()->CheckBoatSpeed() ) );
			}
		}
	}
	regItems->Pop();
}

//o---------------------------------------------------------------------------o
//|	Function	-	void checkauto( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check automatic and timer controlled functions
//o---------------------------------------------------------------------------o
void CWorldMain::CheckAutoTimers( void )
{
	static UI32 nextCheckSpawnRegions	= 0; 
	static UI32 nextCheckTownRegions	= 0;
	static UI32 nextCheckItems			= 0;
	static UI32 nextDecayItems			= 0;
	static UI32 nextSetNPCFlagTime		= 0;
	static UI32 accountFlush			= 0;
	bool doWeather						= false;
	MAPUSERNAMEID_ITERATOR I;

	// modify this stuff to take into account more variables
	if( accountFlush <= GetUICurrentTime() || GetOverflow() )
	{
		bool reallyOn = false;
		// time to flush our account status!
		//for( ourAccount = Accounts->FirstAccount(); !Accounts->FinishedAccounts(); ourAccount = Accounts->NextAccount() )
		for( I = Accounts->begin(); I != Accounts->end(); ++I )
		{
			ACCOUNTSBLOCK& actbTemp = I->second;
			if( actbTemp.wAccountIndex == AB_INVALID_ID)
				continue;

			if( actbTemp.wFlags&AB_FLAGS_ONLINE )
			{
				reallyOn = false;	// to start with, there's no one really on
				Network->PushConn();
				for( CSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
				{
					CChar *tChar = tSock->CurrcharObj();
					if( !ValidateObject( tChar ) )
						continue;
					if( tChar->GetAccount().wAccountIndex == actbTemp.wAccountIndex )
						reallyOn = true;
				}
				Network->PopConn();
				if( !reallyOn )	// no one's really on, let's set that
					actbTemp.wFlags&=0xFFF7;
			}
		}
		accountFlush = BuildTimeValue( (R32)ServerData()->AccountFlushTimer() * 60 );
	}
	Network->On();

	if( GetWorldSaveProgress() == SS_NOTSAVING )
	{
		Network->PushConn();
		for( CSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
		{
			if( tSock->IdleTimeout() != -1 && (UI32)tSock->IdleTimeout() <= GetUICurrentTime() )
			{
				CChar *tChar = tSock->CurrcharObj();
				if( !ValidateObject( tChar ) )
					continue;
				if( !tChar->IsGM() )
				{
					tSock->IdleTimeout( -1 );
					tSock->sysmessage( 1246 );
					Network->Disconnect( tSock );
				}
			}
			else if( ( ( (UI32)( tSock->IdleTimeout() + 300 * 1000 ) <= GetUICurrentTime() && (UI32)( tSock->IdleTimeout()+200*1000) >= GetUICurrentTime() ) || GetOverflow() ) && !tSock->WasIdleWarned()  )
			{//is their idle time between 3 and 5 minutes, and they haven't been warned already?
				CPIdleWarning warn( 0x07 );
				tSock->Send( &warn );
				tSock->WasIdleWarned( true );
			}
		}
		Network->PopConn();
	}
	else if( GetWorldSaveProgress() == SS_JUSTSAVED )	// if we've JUST saved, do NOT kick anyone off (due to a possibly really long save), but reset any offending players to 60 seconds to go before being kicked off
	{
		Network->PushConn();
		for( CSocket *wsSocket = Network->FirstSocket(); !Network->FinishedSockets(); wsSocket = Network->NextSocket() )
		{
			if( wsSocket != NULL )
			{
				if( (UI32)wsSocket->IdleTimeout() < GetUICurrentTime() )
				{
					wsSocket->IdleTimeout( BuildTimeValue( 60.0f ) );
					wsSocket->WasIdleWarned( true );//don't give them the message if they only have 60s
				}
			}
		}
		Network->PopConn();
		SetWorldSaveProgress( SS_NOTSAVING );
	}
	Network->Off();
	if( nextCheckTownRegions <= GetUICurrentTime() || GetOverflow() )
	{
		std::vector< CTownRegion *>::const_iterator regionCounter;
		for( regionCounter = regions.begin(); regionCounter != regions.end(); ++regionCounter )
		{
			CTownRegion *myReg = (*regionCounter);
			if( myReg != NULL )
				myReg->PeriodicCheck();
		}
		nextCheckTownRegions = BuildTimeValue( 10 );	// do checks every 10 seconds or so, rather than every single time
		JailSys->PeriodicCheck();
	}

	if( nextCheckSpawnRegions <= GetUICurrentTime() && ServerData()->CheckSpawnRegionSpeed() != -1 )//Regionspawns
	{
		UI16 itemsSpawned	= 0;
		UI16 npcsSpawned	= 0;
		std::vector< CSpawnRegion * >::const_iterator spawnCounter;
		for( spawnCounter = spawnregions.begin(); spawnCounter != spawnregions.end(); ++spawnCounter )
		{
			CSpawnRegion *spawnReg = (*spawnCounter);
			if( spawnReg != NULL )
			{
				if( spawnReg->GetNextTime() <= GetUICurrentTime() )
                    spawnReg->doRegionSpawn( itemsSpawned, npcsSpawned );
			}
		}
		nextCheckSpawnRegions = BuildTimeValue( (R32)ServerData()->CheckSpawnRegionSpeed() );//Don't check them TOO often (Keep down the lag)
	}
	
	HTMLTemplates->Poll( ETT_ALLTEMPLATES );

	const UI32 saveinterval = ServerData()->ServerSavesTimerStatus();
	if( saveinterval != 0 )
	{
		UI32 oldTime = GetOldTime();
		if( !GetAutoSaved() )
		{
			SetAutoSaved( true );
			time((time_t *)&oldTime);
			SetOldTime( oldTime );
		}
		UI32 newTime = GetNewTime();
		time((time_t *)&newTime);
		SetNewTime( newTime );

		if( difftime( GetNewTime(), GetOldTime() ) >= saveinterval )
		{
			// Dupois - Added Dec 20, 1999
			// After an automatic world save occurs, lets check to see if
			// anyone is online (clients connected).  If nobody is connected
			// Lets do some maintenance on the bulletin boards.
			if( !GetPlayersOnline() && GetWorldSaveProgress() != SS_SAVING )
			{
				Console << "No players currently online. Starting bulletin board maintenance" << myendl;
				Console.Log( "Bulletin Board Maintenance routine running (AUTO)", "server.log" );
				MsgBoardMaintenance();
			}

			SetAutoSaved( false );
			SaveNewWorld( false );
		}
	}
	
	//Time functions
	if( GetUOTickCount() <= GetUICurrentTime() || ( GetOverflow() ) )
	{
		if( ServerData()->incMinute() )
			Weather->NewDay();
		SetUOTickCount( BuildTimeValue( ServerData()->ServerSecondsPerUOMinute() ) );
	}
	
	if( GetTimer( tWORLD_LIGHTTIME ) <= GetUICurrentTime() || GetOverflow() )
	{
		doWorldLight();  //Changes lighting, if it is currently time to.
		Weather->DoStuff();	// updates the weather types
		SetTimer( tWORLD_LIGHTTIME, BuildTimeValue( (R32)ServerData()->SystemTimer( WEATHER ) ) );	// for testing purposes
		doWeather = true;
	}

	bool checkFieldEffects = false, checkHunger = false;
	if( GetTimer( tWORLD_NEXTFIELDEFFECT ) <= GetUICurrentTime() || GetOverflow() )
	{
		checkFieldEffects = true;
		SetTimer( tWORLD_NEXTFIELDEFFECT, BuildTimeValue( 0.5f ) );
	}
	if( ServerData()->HungerDamage() > 0 && ( GetTimer( tWORLD_HUNGERDAMAGE ) <= GetUICurrentTime() || GetOverflow() ) )	// Damage them if they are very hungry
	{
		SetTimer( tWORLD_HUNGERDAMAGE, BuildTimeValue( (R32)ServerData()->HungerDamageRateTimer() ) );	// set new hungertime
		checkHunger = true;
	}
	std::set< SubRegion * > regionList;	// we'll get around our npc problem this way, hopefully
	Network->PushConn();
	for( CSocket *iSock = Network->FirstSocket(); !Network->FinishedSockets(); iSock = Network->NextSocket() )
	{
		if( iSock == NULL )
			continue;
		CChar *mChar		= iSock->CurrcharObj();
		if( !ValidateObject( mChar ) )
			continue;
		UI08 worldNumber	= mChar->WorldNumber();
		if( mChar->GetAccount().wAccountIndex == iSock->AcctNo() && mChar->GetAccount().dwInGame == mChar->GetSerial() )
		{
			genericCheck( iSock, (*mChar), checkFieldEffects, checkHunger );
			checkPC( iSock, mChar, doWeather );

			SI16 xOffset = MapRegion->GetGridX( mChar->GetX() );
			SI16 yOffset = MapRegion->GetGridY( mChar->GetY() );
			for( SI08 counter = -1; counter <= 1; ++counter ) // Check 3 x colums
			{
				for( SI08 ctr2 = -1; ctr2 <= 1; ++ctr2 ) // Check 3 y colums
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

	// Reduce some lag checking these timers constantly in the loop
	bool setNPCFlags = false, checkItems = false, checkAI = false, doRestock = false;
	if( nextSetNPCFlagTime <= GetUICurrentTime() || GetOverflow() )
	{
		nextSetNPCFlagTime = BuildTimeValue( 30 );	// Slow down lag "needed" for setting flags, they are set often enough ;-)
		setNPCFlags = true;
	}
	if( nextCheckItems <= GetUICurrentTime() || GetOverflow() )
	{
		nextCheckItems = BuildTimeValue( static_cast<R32>(ServerData()->CheckItemsSpeed()) );
		nextDecayItems = BuildTimeValue( static_cast<R32>(ServerData()->SystemTimer( DECAY )) );
		checkItems = true;
	}
	if( GetTimer( tWORLD_NEXTNPCAI ) <= GetUICurrentTime() || GetOverflow() )
	{
		SetTimer( tWORLD_NEXTNPCAI, BuildTimeValue( (R32)ServerData()->CheckNpcAISpeed() ) );
		checkAI = true;
	}
	if( GetTimer( tWORLD_SHOPRESTOCK ) <= GetUICurrentTime() || GetOverflow() )
	{
		SetTimer( tWORLD_SHOPRESTOCK, BuildTimeValue( (R32)( ServerData()->SystemTimer( SHOP_SPAWN ) ) ) );
		doRestock = true;
	}

	std::set< SubRegion * >::const_iterator tcCheck = regionList.begin();
	while( tcCheck != regionList.end() )
	{
		SubRegion *toCheck = (*tcCheck);
		CDataList< CChar * > *regChars = toCheck->GetCharList();
		regChars->Push();
		for( CChar *charCheck = regChars->First(); !regChars->Finished(); charCheck = regChars->Next() )
		{
			if( !ValidateObject( charCheck ) || charCheck->isFree() )
				continue;
			if( charCheck->IsNpc() )
			{
				if( !genericCheck( NULL, (*charCheck), checkFieldEffects, checkHunger ) )
				{
					if( setNPCFlags )
						setcharflag( charCheck );	 // only set flag on npcs every 60 seconds (save a little extra lag)
					checkNPC( charCheck, checkAI, doRestock );
				}
			}
			else if( charCheck->GetTimer( tPC_LOGOUT ) )
			{
				ACCOUNTSBLOCK& actbTemp = charCheck->GetAccount();
				if( actbTemp.wAccountIndex != AB_INVALID_ID )
				{
					SERIAL oaiw = actbTemp.dwInGame;
					if( oaiw == INVALIDSERIAL )
					{
						charCheck->SetTimer( tPC_LOGOUT, 0 );
						charCheck->Update();
					}
					else if( oaiw == charCheck->GetSerial() && ( charCheck->GetTimer( tPC_LOGOUT ) <= GetUICurrentTime() || GetOverflow() ) )
					{
						actbTemp.dwInGame = INVALIDSERIAL;
						charCheck->SetAccount( actbTemp );
						charCheck->SetTimer( tPC_LOGOUT, 0 );
						charCheck->Update();
					}
				}
			}
		}
		regChars->Pop();

		checkItem( toCheck, checkItems, nextDecayItems );
		++tcCheck;
	}

	Effects->checktempeffects();
	SpeechSys->Poll();

	// Implement RefreshItem() / statwindow() queue here

	// Currently this causes issues with walking, I would assume it's due to updating (actually teleporting, currently)
	// the character after we change it's x and y in movement.cpp, perhaps we need a toggle of when to do this update
	// rather than simply relying on Dirty() every time the x, y, z, etc change.
	// Maarc : Another possible solution is a WalkXY() function for a character, which updates x/y without doing the dirty
	// Queue now handles both items and chars in one queue, rather than needlessly splitting them up.
	QUEUEMAP_CITERATOR rqIter			= refreshQueue.begin();
	QUEUEMAP_CITERATOR rqIterEnd		= refreshQueue.end();
	while( rqIter != rqIterEnd )
	{
		CBaseObject *mObj = rqIter->first;
		if( ValidateObject( mObj ) )
		{
			if( mObj->GetObjType() == OT_CHAR )
			{
				CChar *uChar = static_cast<CChar *>(mObj);
				if( uChar->GetUpdate( UT_HITPOINTS ) )
					updateStats( uChar, 0 );
				if( uChar->GetUpdate( UT_STAMINA ) )
					updateStats( uChar, 1 );
				if( uChar->GetUpdate( UT_MANA ) )
					updateStats( uChar, 2 );
				if( uChar->GetUpdate( UT_LOCATION ) )
					uChar->Teleport();
				else if( uChar->GetUpdate( UT_UPDATE ) )
					uChar->Update();
			}
			else
				mObj->Update();
		}
		++rqIter;
	}
	refreshQueue.clear();
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
		UString sLine( line );
		sLine = sLine.removeComment().stripWhiteSpace();
		if( !sLine.empty() )
		{
			engineSize = UOX_MAX( sLine.toLong(), DefEngineSize );
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
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Initialize UOX classes
//o---------------------------------------------------------------------------o
void InitClasses( void )
{
	Trigger			= NULL;
	Commands		= NULL;	Combat		= NULL;
	Items			= NULL;	Map			= NULL;
	Npcs			= NULL;	Skills		= NULL;	
	Weight			= NULL;	JailSys		= NULL;
	Network			= NULL;	Magic		= NULL;		
	Races			= NULL;	Weather		= NULL;
	Movement		= NULL;	TEffects	= NULL;	
	WhoList			= NULL;	OffList		= NULL;
	Books			= NULL;	GMQueue		= NULL;	
	Dictionary		= NULL;	Accounts	= NULL;
	MapRegion		= NULL;	SpeechSys	= NULL;
	CounselorQueue	= NULL;	GuildSys	= NULL;
	HTMLTemplates	= NULL;	Effects		= NULL;
	FileLookup		= NULL;
	objFactory		= NULL;

	objFactory = new ObjectFactory;

	// MAKE SURE IF YOU ADD A NEW ALLOCATION HERE THAT YOU FREE IT UP IN Shutdown(...)
	if(( Dictionary		= new CDictionaryContainer )			== NULL ) Shutdown( FATAL_UOX3_ALLOC_DICTIONARY );
	if(( Combat			= new CHandleCombat )					== NULL ) Shutdown( FATAL_UOX3_ALLOC_COMBAT );
	if(( Commands		= new cCommands )						== NULL ) Shutdown( FATAL_UOX3_ALLOC_COMMANDS );
	if(( Items			= new cItem )							== NULL ) Shutdown( FATAL_UOX3_ALLOC_ITEMS );
	if(( Map			= new cMapStuff )						== NULL ) Shutdown( FATAL_UOX3_ALLOC_MAP );
	if(( Npcs			= new cCharStuff )						== NULL ) Shutdown( FATAL_UOX3_ALLOC_NPCS );
	if(( Skills			= new cSkills )							== NULL ) Shutdown( FATAL_UOX3_ALLOC_SKILLS );
	if(( Weight			= new CWeight )							== NULL ) Shutdown( FATAL_UOX3_ALLOC_WEIGHT );
	if(( Network		= new cNetworkStuff )					== NULL ) Shutdown( FATAL_UOX3_ALLOC_NETWORK );
	if(( Magic			= new cMagic )							== NULL ) Shutdown( FATAL_UOX3_ALLOC_MAGIC );
	if(( Races			= new cRaces )							== NULL ) Shutdown( FATAL_UOX3_ALLOC_RACES );
	if(( Weather		= new cWeatherAb )						== NULL ) Shutdown( FATAL_UOX3_ALLOC_WEATHER );
	if(( Movement		= new cMovement )						== NULL ) Shutdown( FATAL_UOX3_ALLOC_MOVE );
	if(( TEffects		= new CTEffectContainer )				== NULL ) Shutdown( FATAL_UOX3_ALLOC_TEMPEFFECTS );	// addition of TEffect class, memory reduction (Abaddon, 17th Feb 2000)
	if(( WhoList		= new cWhoList )						== NULL ) Shutdown( FATAL_UOX3_ALLOC_WHOLIST );	// wholist
	if(( OffList		= new cWhoList( false ) )				== NULL ) Shutdown( FATAL_UOX3_ALLOC_WHOLIST );	// offlist
	if(( Books			= new cBooks )							== NULL ) Shutdown( FATAL_UOX3_ALLOC_BOOKS );
	if(( GMQueue		= new PageVector( "GM Queue" ) )		== NULL ) Shutdown( FATAL_UOX3_ALLOC_PAGEVECTOR );
	if(( CounselorQueue	= new PageVector( "Counselor Queue" ) )	== NULL ) Shutdown( FATAL_UOX3_ALLOC_PAGEVECTOR );
	if(( Trigger		= new Triggers )						== NULL ) Shutdown( FATAL_UOX3_ALLOC_TRIGGERS );
	if(( MapRegion		= new cMapRegion )						== NULL ) Shutdown( FATAL_UOX3_ALLOC_MAPREGION );
	if(( Effects		= new cEffects )						== NULL ) Shutdown( FATAL_UOX3_ALLOC_EFFECTS );
	if(( HTMLTemplates	= new cHTMLTemplates )					== NULL ) Shutdown( FATAL_UOX3_ALLOC_HTMLTEMPLATES );
	if(( Accounts		= new cAccountClass( cwmWorldState->ServerData()->Directory( CSDDP_ACCOUNTS ) ) ) == NULL ) Shutdown( FATAL_UOX3_ALLOC_ACCOUNTS );
	if(( SpeechSys		= new CSpeechQueue()	)				== NULL ) Shutdown( FATAL_UOX3_ALLOC_SPEECHSYS );
	if(( GuildSys		= new CGuildCollection() )				== NULL ) Shutdown( FATAL_UOX3_ALLOC_GUILDS );
	if(( FileLookup		= new cServerDefinitions() )			== NULL ) Shutdown( FATAL_UOX3_ALLOC_SCRIPTS );
	if(( JailSys		= new JailSystem() )					== NULL ) Shutdown( FATAL_UOX3_ALLOC_WHOLIST );
	Map->Cache			= cwmWorldState->ServerData()->ServerMulCachingStatus();	
}

//o---------------------------------------------------------------------------o
//|	Function	-	void ParseArgs( int argc, char *argv[] )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Unknown
//o---------------------------------------------------------------------------o
void ParseArgs( int argc, char *argv[] )
{
	for( int i = 1; i < argc; ++i )
	{
		if( !strcmp( argv[i], "-xgm" ) )
		{
			Console.PrintSectionBegin();
			Console << "XGM Enabled! Initializing... ";
			cwmWorldState->SetXGMEnabled( true );
			Console << "Done!" << myendl;
		}
		else if( !strcmp( argv[i], "-ERROR" ) )
		{
			cwmWorldState->SetErrorCount( UString( argv[i+1] ).toULong() );
			++i;
		}
		else if( !strcmp( argv[i], "-dumptags" ) )
		{
			cwmWorldState->ServerData()->dumpLookup( 0 );
			cwmWorldState->ServerData()->save( "./uox.tst.ini" );
			Shutdown(FATAL_UOX3_SUCCESS);
		}
		else if( !strcmp( argv[i], "-cluox100" ) )
		{
			Console << "Using CLUOX Streaming-IO" << myendl;
			setvbuf( stdout, NULL, _IONBF, 0 );
			setvbuf( stderr, NULL, _IONBF, 0 );
			cluox_io = true;
			++i;
			if( i > argc )
			{
				Console.Error( 0, "Fatal error in CLUOX arguments" );
				Shutdown( 10 );
			}
			char *dummy;
			cluox_stdin_writeback = (void *)strtol( argv[i], &dummy, 16 );
		}
	}
}

bool FindMultiFunctor( CBaseObject *a, UI32 &b, void *extraData )
{
	if( ValidateObject( a ) )
	{
		if( a->CanBeObjType( OT_CHAR ) )
		{
			CChar *c			= static_cast< CChar * >(a);
			CMultiObj *multi	= findMulti( c );
			if( multi != NULL )
				c->SetMulti( multi );
			else
				c->SetMulti( INVALIDSERIAL );
		}
		else if( ((CItem *)a)->GetCont() == NULL )
		{
			CItem *i			= static_cast< CItem * >(a);
			CMultiObj *multi	= findMulti( i );
			if( ValidateObject( multi ) )
				i->SetMulti( multi );
			else
				i->SetMulti( INVALIDSERIAL );
		}
	}
	return true;
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

	UI32 b		= 0;
	ObjectFactory::getSingleton().IterateOver( OT_ITEM, b, NULL, &FindMultiFunctor );
	ObjectFactory::getSingleton().IterateOver( OT_CHAR, b, NULL, &FindMultiFunctor );

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

	char idname[256];
	sprintf( idname, "%s v%s(%s) [%s]\n| Compiled by %s\n| Programmed by: %s", CVersionClass::GetProductName().c_str(), CVersionClass::GetVersion().c_str(), CVersionClass::GetBuild().c_str(), OS_STR, CVersionClass::GetName().c_str(), CVersionClass::GetProgrammers().c_str() );
 
	Console.TurnYellow();
	Console << "Compiled on ";
	Console.TurnNormal();
	Console << __DATE__ << " (" << __TIME__ << ")" << myendl;

	Console.TurnYellow();
	Console << "Compiled by ";
	Console.TurnNormal();
	Console << CVersionClass::GetName() << myendl;

	Console.TurnYellow();
	Console << "Contact: ";
	Console.TurnNormal();
	Console << CVersionClass::GetEmail() << myendl;
	
	Console.PrintSectionBegin();
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
	if( cwmWorldState->ServerData()->ServerCrashProtectionStatus() > 1 )
	{		
		if( cwmWorldState->GetErrorCount() < 10 )
		{
			cwmWorldState->IncErrorCount();
			
			sprintf( temp, "Server crash #%u from unknown error, restarting.", cwmWorldState->GetErrorCount() );
			Console.Log( temp, "server.log" );
			Console << temp << myendl;
			
			sprintf(temp, "uox.exe -ERROR %u", cwmWorldState->GetErrorCount() );
			
			if( cwmWorldState->GetXGMEnabled() )
				strcat( temp, " -xgm" );
			
			delete cwmWorldState;
			system( temp );
			exit(ErrorCode); // Restart successful Don't give them key presses or anything, just go out.
		} 
		else 
		{
			Console.Log( "10th Server crash, server shutting down.", "server.log" );
			Console << "10th Server crash, server shutting down" << myendl;
		}
	} 
	else 
		Console.Log( "Server crash!", "server.log" );
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

	if( HTMLTemplates )
	{
		Console << "HTMLTemplates object detected. Writing Offline HTML Now..." << myendl;
		HTMLTemplates->Poll( ETT_OFFLINE );
	}
	else
		Console << "HTMLTemplates object not found." << myendl;


	if( cwmWorldState->ServerData()->ServerCrashProtectionStatus() >= 1 && retCode && cwmWorldState && cwmWorldState->GetLoaded() && cwmWorldState->GetWorldSaveProgress() != SS_SAVING )
	{//they want us to save, there has been an error, we have loaded the world, and WorldState is a valid pointer.
		do
		{
			cwmWorldState->SaveNewWorld( true );
		} while( cwmWorldState->GetWorldSaveProgress() == SS_SAVING );
	}
	
	Console << "Cleaning up item and character memory... ";
	ObjectFactory::getSingleton().GarbageCollect();
	Console.PrintDone();

	Console << "Destroying class objects and pointers... ";
	// delete any objects that were created (delete takes care of NULL check =)

	delete Combat;
	delete Commands;
	delete Items;
	try
	{
		delete Map;
	}
	catch( ... )
	{
		Console << "ERROR: Either Map was already deleted, or UOX3 was unable to delete object." << myendl;
	}
	delete Npcs;
	delete Skills;
	delete Weight;
	delete Magic;
	delete Races;
	delete Weather;
	delete Movement;
	delete Network;
	delete TEffects;
	delete WhoList;
	delete OffList;
	delete Books;
	delete GMQueue;
	delete HTMLTemplates;
	delete CounselorQueue;
	delete Dictionary;
	delete Accounts;
	if( Trigger )
		Trigger->Cleanup();//must be called to delete some things the still reference Trigger.
	delete Trigger;
	delete MapRegion;
	delete SpeechSys;
	delete GuildSys;
	delete FileLookup;
	delete JailSys;
	delete Effects;

	UnloadSpawnRegions();
	UnloadRegions();

	regions.clear();
	spawnregions.clear();
	Console.PrintDone();

	//Lets wait for console thread to quit here
#if UOX_PLATFORM != PLATFORM_WIN32
	pthread_join( cons, NULL );
#ifdef __LOGIN_THREAD__
	pthread_join( netw, NULL );
#endif
#endif

	// don't leave file pointers open, could lead to file corruption
	Console << "Destroying JS instances... ";
	JS_DestroyContext( jsContext );
	JS_DestroyRuntime( jsRuntime );
	Console.PrintDone();

	Console.PrintSectionBegin();
	if( retCode && cwmWorldState->GetLoaded() )//do restart unless we have crashed with some error.
		Restart( (UI16)retCode );
	else
		delete cwmWorldState;

	Console.TurnGreen();

	delete objFactory;

	Console << "Server shutdown complete!" << myendl;
	Console << "Thank you for supporting " << CVersionClass::GetName() << myendl;
	Console.TurnNormal();
	Console.PrintSectionBegin();
	
	// dispay what error code we had
	// don't report errorlevel for no errors, this is confusing ppl - fur
	if( retCode )
	{
		Console.TurnRed();
		Console << "Exiting UOX with errorlevel " << retCode << myendl;
		Console.TurnNormal();
#if UOX_PLATFORM == PLATFORM_WIN32
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
//|	Function	-	void advanceObj( CChar *s, UI16 x, bool multiUse )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Handle advancement objects (stat / skill gates)
//o---------------------------------------------------------------------------o
void advanceObj( CChar *applyTo, UI16 advObj, bool multiUse )
{
	if( applyTo->GetAdvObj() == 0 || multiUse )
	{
		Effects->PlayStaticAnimation( applyTo, 0x373A, 0, 15);
		Effects->PlaySound( applyTo, 0x01E9 );
		applyTo->SetAdvObj( advObj );
		UString sect				= "ADVANCEMENT " + UString::number( advObj );
		sect						= sect.stripWhiteSpace();
		ScriptSection *Advancement	= FileLookup->FindEntry( sect, advance_def );
		if( Advancement == NULL )
		{
			Console << "ADVANCEMENT OBJECT: Script section not found, Aborting" << myendl;
			applyTo->SetAdvObj( 0 );
			return;
		}
		CItem *retitem		= NULL;
		CItem *hairobject	= applyTo->GetItemAtLayer( IL_HAIR );
		CItem *beardobject	= applyTo->GetItemAtLayer( IL_FACIALHAIR );
		DFNTAGS tag = DFNTAG_COUNTOFTAGS;
		UString cdata;
		UI32 ndata = INVALIDSERIAL, odata = INVALIDSERIAL;
		UI08 skillToSet = 0;
		for( tag = Advancement->FirstTag(); !Advancement->AtEndTags(); tag = Advancement->NextTag() )
		{
			cdata = Advancement->GrabData( ndata, odata );
			switch( tag )
			{
				case DFNTAG_ALCHEMY:			skillToSet = ALCHEMY;							break;
				case DFNTAG_ANATOMY:			skillToSet = ANATOMY;							break;
				case DFNTAG_ANIMALLORE:			skillToSet = ANIMALLORE;						break;
				case DFNTAG_ARMSLORE:			skillToSet = ARMSLORE;							break;
				case DFNTAG_ARCHERY:			skillToSet = ARCHERY;							break;
				case DFNTAG_ADVOBJ:				applyTo->SetAdvObj( static_cast<UI16>(ndata) );	break;
				case DFNTAG_BEGGING:			skillToSet = BEGGING;							break;
				case DFNTAG_BLACKSMITHING:		skillToSet = BLACKSMITHING;						break;
				case DFNTAG_BOWCRAFT:			skillToSet = BOWCRAFT;							break;
				case DFNTAG_CAMPING:			skillToSet = CAMPING;							break;
				case DFNTAG_CARPENTRY:			skillToSet = CARPENTRY;							break;
				case DFNTAG_CARTOGRAPHY:		skillToSet = CARTOGRAPHY;						break;
#ifdef __NEW_SKILLS__
				case DFNTAG_CHIVALRY:			skillToSet = CHIVALRY;							break;
#endif
				case DFNTAG_COOKING:			skillToSet = COOKING;							break;
				case DFNTAG_DEX:				applyTo->SetDexterity( static_cast<SI16>(RandomNum( ndata, odata )) );	break;
				case DFNTAG_DETECTINGHIDDEN:	skillToSet = DETECTINGHIDDEN;					break;
				case DFNTAG_DYEHAIR:
												if( ValidateObject( hairobject ) )
													hairobject->SetColour( static_cast<UI16>(ndata) );
												break;
				case DFNTAG_DYEBEARD:
												if( ValidateObject( beardobject ) )
													beardobject->SetColour( static_cast<UI16>(ndata) );
												break;
				case DFNTAG_ENTICEMENT:			skillToSet = ENTICEMENT;						break;
				case DFNTAG_EVALUATINGINTEL:	skillToSet = EVALUATINGINTEL;					break;
				case DFNTAG_EQUIPITEM:
												retitem = Items->CreateBaseScriptItem( cdata, applyTo->WorldNumber() );
												if( retitem != NULL )
												{
													if( !retitem->SetCont( applyTo ) )
														retitem->SetCont( applyTo->GetPackItem() );
													retitem->PlaceInPack();
												}
												break;
				case DFNTAG_FAME:				applyTo->SetFame( static_cast<SI16>(ndata) );	break;
				case DFNTAG_FENCING:			skillToSet = FENCING;							break;
				case DFNTAG_FISHING:			skillToSet = FISHING;							break;
#ifdef __NEW_SKILLS__
				case DFNTAG_FOCUS:				skillToSet = FOCUS;								break;
#endif
				case DFNTAG_FORENSICS:			skillToSet = FORENSICS;							break;
				case DFNTAG_HEALING:			skillToSet = HEALING;							break;
				case DFNTAG_HERDING:			skillToSet = HERDING;							break;
				case DFNTAG_HIDING:				skillToSet = HIDING;							break;
				case DFNTAG_INTELLIGENCE:		applyTo->SetIntelligence( static_cast<SI16>(RandomNum( ndata, odata )) );	break;
				case DFNTAG_ITEMID:				skillToSet = ITEMID;							break;
				case DFNTAG_INSCRIPTION:		skillToSet = INSCRIPTION;						break;
				case DFNTAG_KARMA:				applyTo->SetKarma( static_cast<SI16>(ndata) );	break;
				case DFNTAG_KILLHAIR:
												retitem = applyTo->GetItemAtLayer( IL_HAIR );
												if( ValidateObject( retitem ) )
													retitem->Delete();
												break;
				case DFNTAG_KILLBEARD:
												retitem = applyTo->GetItemAtLayer( IL_FACIALHAIR );
												if( ValidateObject( retitem ) )
													retitem->Delete();
												break;
				case DFNTAG_KILLPACK:
												retitem = applyTo->GetItemAtLayer( IL_PACKITEM );
												if( ValidateObject( retitem ) )
													retitem->Delete();
												break;
				case DFNTAG_LOCKPICKING:		skillToSet = LOCKPICKING;					break;
				case DFNTAG_LUMBERJACKING:		skillToSet = LUMBERJACKING;					break;
				case DFNTAG_MAGERY:				skillToSet = MAGERY;						break;
				case DFNTAG_MAGICRESISTANCE:	skillToSet = MAGICRESISTANCE;				break;
				case DFNTAG_MACEFIGHTING:		skillToSet = MACEFIGHTING;					break;
				case DFNTAG_MEDITATION:			skillToSet = MEDITATION;					break;
				case DFNTAG_MINING:				skillToSet = MINING;						break;
				case DFNTAG_MUSICIANSHIP:		skillToSet = MUSICIANSHIP;					break;
#ifdef __NEW_SKILLS__
				case DFNTAG_NECROMANCY:			skillToSet = NECROMANCY;					break;
#endif
				case DFNTAG_PARRYING:			skillToSet = PARRYING;						break;
				case DFNTAG_PEACEMAKING:		skillToSet = PEACEMAKING;					break;
				case DFNTAG_POISONING:			skillToSet = POISONING;						break;
				case DFNTAG_PROVOCATION:		skillToSet = PROVOCATION;					break;
				case DFNTAG_POLY:				applyTo->SetID( static_cast<UI16>(ndata) );	break;
				case DFNTAG_PACKITEM:
												if( ValidateObject( applyTo->GetPackItem() ) )
												{
													if( !cdata.empty() )
													{
														if( cdata.sectionCount( "," ) != 0 )
															retitem = Items->CreateScriptItem( NULL, applyTo, cdata.section( ",", 0, 0 ).stripWhiteSpace(), cdata.section( ",", 1, 1 ).stripWhiteSpace().toUShort(), OT_ITEM, true );
														else
															retitem = Items->CreateScriptItem( NULL, applyTo, cdata, 1, OT_ITEM, true );
													}
												}
												else
													Console << "Warning: Bad NPC Script with problem no backpack for packitem" << myendl;
												break;
				case DFNTAG_REMOVETRAPS:		skillToSet = REMOVETRAPS;					break;
				case DFNTAG_STRENGTH:			applyTo->SetStrength( static_cast<SI16>(RandomNum( ndata, odata )) );			break;
				case DFNTAG_SKILL:				applyTo->SetBaseSkill( static_cast<UI16>(odata), static_cast<UI08>(ndata) );	break;
				case DFNTAG_SKIN:				applyTo->SetSkin( cdata.toUShort() );		break;
				case DFNTAG_SNOOPING:			skillToSet = SNOOPING;						break;
				case DFNTAG_SPIRITSPEAK:		skillToSet = SPIRITSPEAK;					break;
				case DFNTAG_STEALING:			skillToSet = STEALING;						break;
				case DFNTAG_STEALTH:			skillToSet = STEALTH;						break;
				case DFNTAG_SWORDSMANSHIP:		skillToSet = SWORDSMANSHIP;					break;
				case DFNTAG_TACTICS:			skillToSet = TACTICS;						break;
				case DFNTAG_TAILORING:			skillToSet = TAILORING;						break;
				case DFNTAG_TAMING:				skillToSet = TAMING;						break;
				case DFNTAG_TASTEID:			skillToSet = TASTEID;						break;
				case DFNTAG_TINKERING:			skillToSet = TINKERING;						break;
				case DFNTAG_TRACKING:			skillToSet = TRACKING;						break;
				case DFNTAG_VETERINARY:			skillToSet = VETERINARY;					break;
				case DFNTAG_WRESTLING:			skillToSet = WRESTLING;						break;
				default:						Console << "Unknown tag in advanceObj(): " << (SI32)tag << myendl;		break;
			}
			if( skillToSet > 0 )
			{
				applyTo->SetBaseSkill( static_cast<UI16>(RandomNum( ndata, odata )), skillToSet );
				skillToSet = 0;	// reset for next time through
			}
		}
		applyTo->Teleport();
	}
	else
	{
		CSocket *sock = calcSocketObjFromChar( applyTo );
		if( sock != NULL )
			sock->sysmessage( 1366 );
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	UI32 getclock( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Return CPU time used, Emulates clock()
//o---------------------------------------------------------------------------o
UI32 getclock( void )
{
	struct timeval tv;
#if UOX_PLATFORM != PLATFORM_WIN32
	gettimeofday( &tv, NULL );
#else
	timeb local;
	ftime( &local );
	tv.tv_sec	= local.time;
	tv.tv_usec	= local.millitm * 1000;
#endif
	// We want to normalise our timer to that we established at the start.
	// System start up time becomes 0, even though it's not. So we have to
	// subtract the original timer value from our current reading, to get 
	// a process start normalised time.
	tv.tv_usec	-= current.tv_usec;
	if( tv.tv_usec < 0 )	// Start microseconds were higher than our current value
	{
		tv.tv_sec -= 1;		// remove a second and add a second's worth of microseconds
		tv.tv_usec += 1000000;
	}
	// Now we have a microsecond correct seconds value, we normalise our seconds value too
	tv.tv_sec	-= current.tv_sec;
	// We're wanting to return a millisecond value
	// Milliseconds = seconds * 1000 + microseconds / 1000
	return ( tv.tv_sec * 1000 ) + ( tv.tv_usec / 1000 );
//	return tv.tv_sec + tv.tv_usec / 1000;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void doLight( CSocket *s, UI08 level )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Send light level to players client
//o---------------------------------------------------------------------------o
void doLight( CSocket *s, UI08 level )
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

	CTownRegion *curRegion	= mChar->GetRegion();
	CWeather *wSys			=  Weather->Weather( curRegion->GetWeather() );
	LIGHTLEVEL toShow;

	LIGHTLEVEL dunLevel = cwmWorldState->ServerData()->DungeonLightLevel();
	// we have a valid weather system
	if( wSys != NULL )
	{
		R32 lightMin = wSys->LightMin();
		R32 lightMax = wSys->LightMax();
		if( lightMin < 300 && lightMax < 300 )
		{
			R32 i = wSys->CurrentLight();
			if( Races->VisLevel( mChar->GetRace() ) > i )
				toShow = 0;
			else
				toShow = static_cast<LIGHTLEVEL>(i - Races->VisLevel( mChar->GetRace() ));
			toSend.Level( level );
		}
		else
			toSend.Level( level );
	}
	else
	{
		if( mChar->inDungeon() )
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
//|	Function	-	void telltime( CSocket *s )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get in-game time
//o---------------------------------------------------------------------------o
void telltime( CSocket *s )
{
	UI08 hour			= cwmWorldState->ServerData()->ServerTimeHours();
	UI08 minute			= cwmWorldState->ServerData()->ServerTimeMinutes();
	bool ampm			= cwmWorldState->ServerData()->ServerTimeAMPM();
	UnicodeTypes sLang	= s->Language();

	std::string tstring, tstring2;
	if( minute <= 14 )
		tstring = Dictionary->GetEntry( 1248, sLang );
	else if( minute >= 15 && minute <= 30 )
		tstring = Dictionary->GetEntry( 1249, sLang );
	else if( minute >= 30 && minute <= 45 )
		tstring = Dictionary->GetEntry( 1250, sLang );
	else
	{
		tstring = Dictionary->GetEntry( 1251, sLang );
		++hour;
		if( hour == 12 )
			hour = 0;
	}
	if( hour >= 1 && hour <= 11 )
		tstring2 = tstring + Dictionary->GetEntry( 1251 + hour, sLang );
	else if( hour == 1 && ampm )
		tstring2 = tstring + Dictionary->GetEntry( 1263, sLang );
	else
		tstring2 = tstring + Dictionary->GetEntry( 1264, sLang );
	
	if( hour == 0 )
		tstring = tstring2;
	else if( ampm )
	{
		if( hour >= 1 && hour < 6 )
			tstring2 += Dictionary->GetEntry( 1265 );
		else if( hour >= 6 && hour < 9 )
			tstring2 += Dictionary->GetEntry( 1266 );
		else 
			tstring2 += Dictionary->GetEntry( 1267 );
		s->sysmessage( tstring2.c_str() );
		return;
	}
	else
	{
		if( hour >= 1  && hour < 5 )
			tstring2 += Dictionary->GetEntry( 1268 );
		else 
			tstring2 += Dictionary->GetEntry( 1269 );
		s->sysmessage( tstring2.c_str() );
		return;
	}
	s->sysmessage( tstring .c_str());
}

//o---------------------------------------------------------------------------o
//|	Function	-	int getTileName( CItem *i, char* itemname )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Returns the length of an items name from tiledata.mul and
//|					sets itemname to the name
//o---------------------------------------------------------------------------o
// Can't return std::size_t as VC6 says it's not in that namespace
// The format it accepts is same as UO style - %plural/single% or %plural%
// eg arrow%s%
//    loa%ves/f% of bread
size_t getTileName( CItem *i, std::string& itemname )
{
	CTile tile;
	Map->SeekTile( i->GetID(), &tile );
	UString temp	= i->GetName() ;
	temp			= temp.simplifyWhiteSpace();
	if( temp.substr( 0, 1 ) == "#" )
	{
		temp =  static_cast< UString >( tile.Name() );
	}
	
	UI16 getAmount = i->GetAmount();
	if( getAmount == 1 )
	{
		if( tile.DisplayAsAn() )
			temp = "an " + temp;
		else if( tile.DisplayAsA() )
			temp = "a " + temp;
	}
	else
		temp = UString::number( getAmount ) + " " + temp;
	// Find out if the name has a % in it
	if( temp.sectionCount("%") > 0 )
	{
		UString single;
		UString first	= temp.section( "%", 0, 0 );
		UString rest	= temp.section( "%", 2 );
		UString plural	= rest.section( "%", 1, 1 );
		if( plural.sectionCount("/") > 0 )
		{
			single = plural.section( "/", 1 );
			plural = plural.section( "/", 0, 0 );
		}
		if( getAmount < 2 )
			temp = first + single + rest;
		else
			temp = first + plural + rest;
	}
	itemname = static_cast< std::string >( temp.simplifyWhiteSpace() );
	return itemname.size() + 1;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void checkRegion( CChar *i )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check what region a character is in
//o---------------------------------------------------------------------------o
void checkRegion( CSocket *mSock, CChar *i )
{
	if( !ValidateObject( i ) )
		return;
	CTownRegion *iRegion	= i->GetRegion();
	CTownRegion *calcReg	= calcRegionFromXY( i->GetX(), i->GetY(), i->WorldNumber() );
	if( iRegion == NULL && calcReg != NULL )
		i->SetRegion( calcReg->GetRegionNum() );
	else if( calcReg != iRegion )
	{
		if( mSock != NULL )
		{
			if( iRegion != NULL && calcReg != NULL )
			{
				// Drake 08-30-99 If region name are the same, do not display message
				//                for playing music when approaching Tavern
				if( iRegion->GetName() != calcReg->GetName() )
				{
					if( !iRegion->GetName().empty() )
						mSock->sysmessage( 1358, iRegion->GetName().c_str() );

					if( !calcReg->GetName().empty() )
						mSock->sysmessage( 1359, calcReg->GetName().c_str() );
				}
				if( calcReg->IsGuarded() || ( !( calcReg->IsGuarded() && iRegion->IsGuarded() ) ) )
				{
					if( calcReg->IsGuarded() )
					{
						if( calcReg->GetOwner().empty() )
							mSock->sysmessage( 1360 );
						else
							mSock->sysmessage( 1361, calcReg->GetOwner().c_str() );
					} 
					else
					{
						if( iRegion->GetOwner().empty() )
							mSock->sysmessage( 1362 );
						else
							mSock->sysmessage( 1363, iRegion->GetOwner().c_str() );
					}
				}
				if( calcReg->GetAppearance() != iRegion->GetAppearance() )	 // if the regions look different
				{
					CPWorldChange wrldChange( calcReg->GetAppearance(), 1 );
					mSock->Send( &wrldChange );
				}
				if( calcReg == regions[i->GetTown()] )	// enter our home town
				{
					mSock->sysmessage( 1364 );
					CItem *packItem = i->GetPackItem();
					if( ValidateObject( packItem ) )
					{
						CDataList< CItem * > *piCont = packItem->GetContainsList();
						for( CItem *toScan = piCont->First(); !piCont->Finished(); toScan = piCont->Next() )
						{
							if( ValidateObject( toScan ) )
							{
								if( toScan->GetType() == IT_TOWNSTONE )
								{
									CTownRegion *targRegion = regions[static_cast<UI08>(toScan->GetTempVar( CITV_MOREX ))];
									mSock->sysmessage( 1365, targRegion->GetName().c_str() );
									targRegion->DoDamage( targRegion->GetHealth() );	// finish it off
									targRegion->Possess( calcReg );
									i->SetFame( (SI16)( i->GetFame() + i->GetFame() / 5 ) );	// 20% fame boost
									break;
								}
							}
						}
					}
				}
			}
		}
		if( iRegion != NULL && calcReg != NULL )
		{
			UI16 leaveScript = i->GetScriptTrigger();
			cScript *tScript = Trigger->GetScript( leaveScript );
			if( tScript != NULL )
			{
				tScript->OnLeaveRegion( i, iRegion->GetRegionNum() );
				tScript->OnEnterRegion( i, calcReg->GetRegionNum() );
			}

			UI16 regLeaveScript	= iRegion->GetScriptTrigger();
			cScript *trScript	= Trigger->GetScript( regLeaveScript );
			if( trScript != NULL )
				trScript->OnLeaveRegion( i, iRegion->GetRegionNum() );

			UI16 regEnterScript	= calcReg->GetScriptTrigger();
			cScript *teScript	= Trigger->GetScript( regEnterScript );
			if( teScript != NULL )
				teScript->OnEnterRegion( i, calcReg->GetRegionNum() );
		}
		if( calcReg != NULL )
			i->SetRegion( calcReg->GetRegionNum() );
		if( mSock != NULL )
		{
			Effects->dosocketmidi( mSock );
			doLight( mSock, cwmWorldState->ServerData()->WorldLightCurrentLevel() );	// force it to update the light level straight away
			Weather->DoPlayerStuff( mSock, i );	// force a weather update too
		}
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
		CSocket *cSock = calcSocketObjFromChar( c );
		c->SetTimer( tCHAR_CRIMFLAG, BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->RepCrimTime()) ) );
		if( cSock != NULL )
			cSock->sysmessage( 1379 );
		setcharflag( c );
	}
	else	// let's update their flag, as another criminal act will reset the timer
		c->SetTimer( tCHAR_CRIMFLAG, BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->RepCrimTime()) ) );
	// Lets let Npc AI and other code sections take care of calling guards
}

//o---------------------------------------------------------------------------o
//|	Function	-	void setcharflag( CChar *c )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Set characters flag
//o---------------------------------------------------------------------------o
void setcharflag( CChar *c )
{
	if( !ValidateObject( c ) )
		return;
	UI08 oldFlag = c->GetFlag();
	if( !c->IsNpc() )
	{
		if( c->GetKills() > cwmWorldState->ServerData()->RepMaxKills() )
			c->SetFlagRed();
		else if( c->GetTimer( tCHAR_CRIMFLAG ) != 0 )
			c->SetFlagGray();
		else 
			c->SetFlagBlue();
	} 
	else 
	{
		switch( c->GetNPCAiType() )
		{
			case aiEVIL:		// Evil
			case aiHEALER_E:	// Evil healer
			case aiCHAOTIC:		// BS/EV
				c->SetFlagRed();
				break;
			case aiHEALER_G:	// Good Healer
			case aiPLAYERVENDOR:// Player Vendor
			case aiGUARD:		// Guard
			case aiBANKER:		// Banker
				c->SetFlagBlue();
				break;
			default:
				if( c->GetID() == 0x0190 || c->GetID() == 0x0191 )
				{
					c->SetFlagBlue();
					break;
				}
				else if( cwmWorldState->ServerData()->CombatAnimalsGuarded() && cwmWorldState->creatures[c->GetID()].IsAnimal() )
				{
					if( c->GetRegion()->IsGuarded() )	// in a guarded region, with guarded animals, animals == blue
						c->SetFlagBlue();
					else
						c->SetFlagGray();
				}
				else	// if it's not a human form, and animal's aren't guarded, then they're gray
					c->SetFlagGray();
				if( ValidateObject( c->GetOwnerObj() ) && c->IsTamed() )
				{
					CChar *i = c->GetOwnerObj();
					if( ValidateObject( i ) )
						c->SetFlag( i->GetFlag() );
					else
						c->SetFlagBlue();
					if( c->IsInnocent() && !cwmWorldState->ServerData()->CombatAnimalsGuarded() )
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
//|	Function	-	CItem *GenerateCorpse( CChar *mChar )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Generates a corpse based on skin of the character killed
//o---------------------------------------------------------------------------o
void GenerateCorpse( CChar *mChar )
{
	bool createPack = ( mChar->GetID( 2 ) == 0x0D || mChar->GetID( 2 ) == 0x0F || mChar->GetID( 2 ) == 0x10 || mChar->GetID() == 0x023E );
	CItem *iCorpse = NULL;
	if( !createPack )
	{
		iCorpse = Items->CreateItem( NULL, mChar, 0x2006, 1, mChar->GetSkin(), OT_ITEM );
		if( iCorpse == NULL )
			return;
		char temp[512];
		sprintf( temp, Dictionary->GetEntry( 1612 ).c_str(), mChar->GetName().c_str() );
		iCorpse->SetName( temp );
		iCorpse->SetCorpse( true );
		iCorpse->SetCarve( mChar->GetCarve() );
		iCorpse->SetMovable( 2 );//non-movable
		iCorpse->SetDir( mChar->GetDir() );
		iCorpse->SetAmount( mChar->GetID() );
	} 
	else
	{
		iCorpse = Items->CreateItem( NULL, mChar, 0x09B2, 1, 0x0000, OT_ITEM );
		if( iCorpse== NULL )
			return;
		iCorpse->SetName( Dictionary->GetEntry( 1611 ) );
		iCorpse->SetCorpse( false );
	}

	UI08 canCarve = 0;
	if( mChar->GetID( 1 ) == 0x00 && ( mChar->GetID( 2 ) == 0x0C || ( mChar->GetID( 2 ) >= 0x3B && mChar->GetID( 2 ) <= 0x3D ) ) ) // If it's a dragon, 50/50 chance you can carve it
		canCarve = static_cast<UI08>(RandomNum( 0, 1 ));

	iCorpse->SetName2( mChar->GetName().c_str() );
	iCorpse->SetType( IT_CONTAINER );
	iCorpse->SetLocation( mChar );
	iCorpse->SetTempVar( CITV_MOREY, canCarve, 1 );
	iCorpse->SetTempVar( CITV_MOREY, mChar->isHuman(), 2 );
	iCorpse->SetTempVar( CITV_MOREZ, mChar->GetFlag() );
	iCorpse->SetMurderTime( cwmWorldState->GetUICurrentTime() );
	R32 decayTime = static_cast<R32>(cwmWorldState->ServerData()->SystemTimer( DECAY ));
	if( !mChar->IsNpc() )
	{
		UI08 decayMultiplier = static_cast<UI08>(cwmWorldState->ServerData()->SystemTimer( PLAYER_CORPSE )&0xff);
		iCorpse->SetOwner( mChar );
		iCorpse->SetDecayTime( BuildTimeValue( static_cast<R32>(decayTime*decayMultiplier) ) );
	}
	else
		iCorpse->SetDecayTime( BuildTimeValue( decayTime ) );

	CMultiObj *iMulti = findMulti( iCorpse );
	if( ValidateObject( iMulti ) )
		iCorpse->SetMulti( iMulti );

	if( !ValidateObject( mChar->GetAttacker() ) )
		iCorpse->SetTempVar( CITV_MOREX, INVALIDSERIAL );
	else
		iCorpse->SetTempVar( CITV_MOREX, mChar->GetAttacker()->GetSerial() );

	if( cwmWorldState->ServerData()->DeathAnimationStatus() )
		Effects->deathAction( mChar, iCorpse );

	CItem *k			= NULL;
	CItem *packItem		= mChar->GetPackItem();
	bool packIsValid	= ValidateObject( packItem );
	for( CItem *j = mChar->FirstItem(); !mChar->FinishedItems(); j = mChar->NextItem() )
	{
		if( !ValidateObject( j ) )
			continue;

		UI08 iLayer = j->GetLayer();

		switch( iLayer )
		{
		case IL_NONE:
		case IL_BUYCONTAINER:
		case IL_BOUGHTCONTAINER:
		case IL_SELLCONTAINER:
		case IL_BANKBOX:
			continue;
		case IL_HAIR:
		case IL_FACIALHAIR:
			j->SetName( "Hair/Beard" );
			j->SetX( 0x47 );
			j->SetY( 0x93 );
			j->SetZ( 0 );
			break;
		case IL_PACKITEM:
			CDataList< CItem * > *jCont;
			jCont = j->GetContainsList();
			for( k = jCont->First(); !jCont->Finished(); k = jCont->Next() )
			{
				if( !ValidateObject( k ) )
					continue;

				if( !k->isNewbie() && k->GetType() != IT_SPELLBOOK )
				{
					k->SetCont( iCorpse );
					k->SetX( static_cast<SI16>(20 + ( RandomNum( 0, 49 ) )) );
					k->SetY( static_cast<SI16>(85 + ( RandomNum( 0, 75 ) )) );
					k->SetZ( 9 );
				}
			}
			if( !mChar->IsShop() && !createPack )
				j->SetLayer( IL_BUYCONTAINER );
			break;
		default:
			if( packIsValid && j->isNewbie() )
				j->SetCont( packItem );
			else
			{
				j->SetCont( iCorpse );
				j->SetX( static_cast<SI16>( 20 + ( RandomNum( 0, 49 ) ) ) );
				j->SetY( static_cast<SI16>( 85 + ( RandomNum( 0, 74 ) ) ) );
				j->SetZ( 9 );
			}
			break;
		}
	}
}

//o---------------------------------------------------------------------------o
//|   Function    -  void doDeathStuff( CChar *i )
//|   Date        -  UnKnown
//|   Programmer  -  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     -  Performs death stuff. I.E.- creates a corpse, moves items
//|                  to it, take out of war mode, does animation and sound, etc.
//o---------------------------------------------------------------------------o
void doDeathStuff( CChar *i )
{
	if( !ValidateObject( i ) || i->IsDead() || i->IsInvulnerable() )	// don't kill them if they are dead or invulnerable!
		return;

	CSocket *pSock = NULL;
	if( !i->IsNpc() )
		pSock = calcSocketObjFromChar( i );

	if( i->GetID() != i->GetOrgID() )
		i->SetID( i->GetOrgID() );

	if( pSock != NULL )
	{
		DismountCreature( i );
		killTrades( (*i) );
	}

	Effects->playDeathSound( i );

	GenerateCorpse( i );

	i->SetSkin( 0x83EA );
	i->SetDead( true );
	i->SetWar( false );
	i->StopSpell();
	i->SetHP( 0 );
	i->SetPoisoned( 0 );
	i->SetPoisonStrength( 0 );
	if( !i->IsNpc() )
	{ 
		if( i->GetOrgID() == 0x0190 )
			i->SetID( 0x0192 );  // Male or Female
		else
			i->SetID( 0x0193 );

		CItem *c = Items->CreateItem( NULL, i, 0x204E, 1, 0, OT_ITEM );
		if( c == NULL )
			return;
		c->SetName( Dictionary->GetEntry( 1610 ) );
		i->SetRobe( c->GetSerial() );
		c->SetLayer( IL_ROBE );
		if( c->SetCont( i ) )
			c->SetDef( 1 );

		if( i->GetAccount().wAccountIndex != AB_INVALID_ID )
		{
			if( pSock != NULL )
			{
				CPResurrectMenu toSend( 0 );
				pSock->Send( &toSend );
			}
			// The fade to gray is done by the CPResurrectMenu packet
			i->Update();
		}
	}
	
	UI16 targTrig		= i->GetScriptTrigger();
	cScript *toExecute	= Trigger->GetScript( targTrig );
	if( toExecute != NULL )
		toExecute->OnDeath( i );

	if( i->IsNpc() )
		i->Delete();
}

void SendMapChange( UI08 worldNumber, CSocket *sock, bool initialLogin )
{
	if( sock == NULL )
		return;
	CPMapChange mapChange( worldNumber );
	if( !initialLogin && worldNumber > 1 )
	{
		switch( sock->ClientType() )
		{
			case CV_UO3D:
			case CV_KRRIOS:
				break;
			default:
				//mapChange.SetMap( 0 );
				break;
		}
	}
	sock->Send( &mapChange );
	CChar *mChar = sock->CurrcharObj();
	if( !initialLogin )
		mChar->Teleport();
}

void SocketMapChange( CSocket *sock, CChar *charMoving, CItem *gate )
{
	if( !ValidateObject( gate ) || ( sock == NULL && !ValidateObject( charMoving ) ) )
		return;
	UI08 tWorldNum = (UI08)gate->GetTempVar( CITV_MORE );
	if( !Map->MapExists( tWorldNum ) )
		return;
	CChar *toMove = charMoving;
	if( sock != NULL && !ValidateObject( charMoving ) )
		toMove = sock->CurrcharObj();
	if( !ValidateObject( toMove ) )
		return;
	switch( sock->ClientType() )
	{
		case CV_UO3D:
		case CV_KRRIOS:
			toMove->SetLocation( (SI16)gate->GetTempVar( CITV_MOREX ), (SI16)gate->GetTempVar( CITV_MOREY ), (SI08)gate->GetTempVar( CITV_MOREZ ), tWorldNum );
			break;
		default:
			//if( tWorldNum <= 1 )
				toMove->SetLocation( (SI16)gate->GetTempVar( CITV_MOREX ), (SI16)gate->GetTempVar( CITV_MOREY ), (SI08)gate->GetTempVar( CITV_MOREZ ), tWorldNum );
	//		else
	//			toMove->SetLocation( (SI16)gate->GetTempVar( CITV_MOREX ), (SI16)gate->GetTempVar( CITV_MOREY ), (SI08)gate->GetTempVar( CITV_MOREZ ), 0 );
			break;
	}
	SendMapChange( tWorldNum, sock );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void NetworkPollConnectionThread( void *params )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Watch for new connections
//o---------------------------------------------------------------------------o
#if UOX_PLATFORM != PLATFORM_WIN32
void *NetworkPollConnectionThread( void *params )
#else
void NetworkPollConnectionThread( void *params )
#endif
{
	messageLoop << "Thread: NetworkPollConnection has started";
	netpollthreadclose = false;
	while( !netpollthreadclose )
	{
		Network->CheckConnections();
		Network->CheckLoginMessage();
		UOXSleep( 20 );
	}
#if UOX_PLATFORM != PLATFORM_WIN32
	pthread_exit( NULL );
#else
	_endthread();
#endif
	messageLoop << "Thread: NetworkPollConnection has Closed";
}

//o---------------------------------------------------------------------------o
//|	Function		-	void xFTPdConnectionThread( void *params )
//|	Programmer	-	EviLDeD
//o---------------------------------------------------------------------------o
//|	Purpose			-	Watch for new connections
//o---------------------------------------------------------------------------o
#if UOX_PLATFORM != PLATFORM_WIN32
void *xFTPdConnectionThread( void *params )
#else
void xFTPdConnectionThread( void *params )
#endif
{
	messageLoop << "Thread: xFTPd has started";
	xFTPdclose = false;
	while( !xFTPdclose )
	{
		UOXSleep( 50 );
	}
#if UOX_PLATFORM != PLATFORM_WIN32
	pthread_exit( NULL );
#else
	_endthread();
#endif
	messageLoop << "Thread: xFTPd has stopped.";
}

//o---------------------------------------------------------------------------o
//|	Function		-	void xFTPcConnectionThread( void *params )
//|	Programmer		-	EviLDeD
//o---------------------------------------------------------------------------o
//|	Purpose			-	Watch for new connections
//o---------------------------------------------------------------------------o
#if UOX_PLATFORM != PLATFORM_WIN32
void *xFTPcConnectionThread( void *params )
#else
void xFTPcConnectionThread( void *params )
#endif
{
	messageLoop << "Thread: xFTPc has started";
	xFTPdclose = false;
	while( !xFTPdclose )
	{
		UOXSleep( 50 );
	}
#if UOX_PLATFORM != PLATFORM_WIN32
	pthread_exit( NULL );
#else
	_endthread();
#endif
	messageLoop << "Thread: xFTPc has stopped.";
}

}

using namespace UOX;

//o---------------------------------------------------------------------------o
//|	Function	-	int main( int argc, char *argv[] )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	UOX startup stuff
//o---------------------------------------------------------------------------o
int main( int argc, char *argv[] )
{
	UI32 uiNextCheckConn = 0;
	UI32 tempSecs, tempMilli, tempTime;
	UI32 loopSecs, loopMilli;

	// EviLDeD: 042102: I moved this here where it basically should be for any windows application or dll that uses WindowsSockets.
#if UOX_PLATFORM == PLATFORM_WIN32
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD( 2, 0 );
	SI32 err = WSAStartup( wVersionRequested, &wsaData );
	if( err )
	{
		Console.Error( 0, "Winsock 2.0 not found on your system..." );
		return 1;
	}
#endif

	char temp[1024];
#ifdef _CRASH_PROTECT_
	try 
	{// Zippy - Error trapping....
#endif
#if UOX_PLATFORM != PLATFORM_WIN32
		gettimeofday( &current, NULL );
#else
		timeb local;
		::ftime( &local );
		current.tv_sec = local.time;
		current.tv_usec = local.millitm * 1000;
#endif
		const UI32 currentTime = 0;
		
#if UOX_PLATFORM == PLATFORM_WIN32
		sprintf( temp, "%s v%s.%s", CVersionClass::GetProductName().c_str(), CVersionClass::GetVersion().c_str(), CVersionClass::GetBuild().c_str() );
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
		Console << "UOX Server start up!" << myendl << "Welcome to " << CVersionClass::GetProductName() << " v" << CVersionClass::GetVersion() << "." << CVersionClass::GetBuild() << myendl;
		Console.PrintSectionBegin();

		LoadJSEngine();

		if(( cwmWorldState = new CWorldMain ) == NULL ) 
			Shutdown( FATAL_UOX3_ALLOC_WORLDSTATE );
		LoadINIFile();

		Console << "Initializing and creating class pointers... " << myendl;
		InitClasses();
		cwmWorldState->SetUICurrentTime( currentTime );

		ParseArgs( argc, argv );
		Console.PrintSectionBegin();
		
		Console << "Loading skill advancement      ";
		LoadSkills();
		Console.PrintDone();
		
		// Moved BulkStartup here, dunno why that function was there...
		Console << "Loading dictionaries...        " << myendl ;
		Console.PrintBasedOnVal( Dictionary->LoadDictionary() >= 0 );

		Console << "Loading teleport               ";
		LoadTeleportLocations();
		Console.PrintDone();

		Console << "Loading GoPlaces               ";
		LoadPlaces();
		Console.PrintDone();
		
		srand( current.tv_sec ); // initial randomization call
		
		// moved all the map loading into cMapStuff - fur
		Map->Load();
		
		Skills->Load();
		
		Console << "Loading Spawn Regions          ";
		LoadSpawnRegions();
		Console.PrintDone();

		Console << "Loading Regions                ";
		LoadRegions();
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
		MapRegion->Load();

		Console << "Loading Guilds                 ";
		GuildSys->Load();
		Console.PrintDone();

		Console.PrintSectionBegin();
		Console << "Clearing all trades            ";
		clearTrades();
		Console.PrintDone();
		InitMultis();
		
		cwmWorldState->SetStartTime( cwmWorldState->GetUICurrentTime() );

		cwmWorldState->SetEndTime( 0 );
		cwmWorldState->SetLClock( 0 );

		// no longer Que, because that's taken care of by PageVector
		Console << "Initializing Jail system       ";	
		JailSys->ReadSetup();
		JailSys->ReadData();
		Console.PrintDone();
		
		Console << "Initializing Status system     ";	
		HTMLTemplates->Load();
		Console.PrintDone();

		Console << "Loading custom titles          ";
		LoadCustomTitle();
		Console.PrintDone();

		Console << "Loading temporary Effects      ";
		Effects->LoadEffects();
		Console.PrintDone();

		Console << "Loading creatures              ";
		LoadCreatures();
		Console.PrintDone();

		DisplayBanner();
		//DisplaySettings(); << Moved that to the configuration

		Console << "Loading Accounts               ";
		Accounts->Load();
		//Console.PrintDone(); 

		Console.Log( "-=Server Startup=-\n=======================================================================", "server.log" );
//		cwmWorldState->SetUICurrentTime( getclock() );

		Console << myendl << "Creating and Initializing Console Thread      ";
	#if UOX_PLATFORM != PLATFORM_WIN32
		int conthreadok = pthread_create(&cons,NULL,CheckConsoleKeyThread , NULL );
	#else
		int conthreadok = _beginthread( CheckConsoleKeyThread , 0 , NULL );
	#endif
#ifdef __LOGIN_THREAD__
		Console << myendl << "Creating and Initializing xLOGINd Thread      ";
	#if UOX_PLATFORM != PLATFORM_WIN32
		pthread_create( &netw, NULL, NetworkPollConnectionThread,  NULL );
	#else
		_beginthread( NetworkPollConnectionThread, 0, NULL );
	#endif 
#endif
#ifdef __XFTPD_THREAD__
		Console << myendl << "Creating and Initializing xFTPd (Daemon) Thread      ";
	#if UOX_PLATFORM != PLATFORM_WIN32
		pthread_create(&netw,NULL, xFTPdConnectionThread,  NULL );
	#else
		_beginthread( xFTPdConnectionThread, 0, NULL );
	#endif 
#endif
#ifdef __XFTPC_THREAD__
		Console << myendl << "Creating and Initializing xFTPc (Client) Thread      ";
	#if UOX_PLATFORM != PLATFORM_WIN32
		pthread_create(&netw,NULL, xFTPcConnectionThread,  NULL );
	#else
		_beginthread( xFTPcConnectionThread, 0, NULL );
	#endif 
#endif
		Console.PrintDone();

		Console.PrintSectionBegin();
		// we've really finished loading here
		cwmWorldState->SetLoaded( true );
		Console << "UOX: Startup Complete" << myendl;
		Console.PrintSectionBegin();
		
		// MAIN SYSTEM LOOP
		while( cwmWorldState->GetKeepRun() )
		{
			//	EviLDeD	-	February 27, 2000
			//	Just in case the thread doesn't start then use the main threaded copy
			if( conthreadok == -1 )
				checkkey();
			UOXSleep( (cwmWorldState->GetPlayersOnline() ? 10 : 90 ) );
			//	EviLDeD	-	End
			if( cwmWorldState->ServerProfile()->LoopTimeCount() >= 1000 )
			{
				cwmWorldState->ServerProfile()->LoopTimeCount( 0 );
				cwmWorldState->ServerProfile()->LoopTime( 0 );
			}
			cwmWorldState->ServerProfile()->IncLoopTimeCount();
			
			StartMilliTimer( loopSecs, loopMilli );
			
			if( cwmWorldState->ServerProfile()->NetworkTimeCount() >= 1000 )
			{
				cwmWorldState->ServerProfile()->NetworkTimeCount( 0 );
				cwmWorldState->ServerProfile()->NetworkTime( 0 );
			}
			
			StartMilliTimer( tempSecs, tempMilli );
#ifndef __LOGIN_THREAD__
			if( uiNextCheckConn <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() ) // Cut lag on CheckConn by not doing it EVERY loop.
			{
				Network->CheckConnections();
				uiNextCheckConn = BuildTimeValue( 1.0f );
			}
			Network->CheckMessages();
#else
			Network->CheckMessage();
			Network->CheckXGM();
#endif
			
			tempTime = CheckMilliTimer( tempSecs, tempMilli );
			cwmWorldState->ServerProfile()->IncNetworkTime( tempTime );
			cwmWorldState->ServerProfile()->IncNetworkTimeCount();
			
			if( cwmWorldState->ServerProfile()->TimerTimeCount() >= 1000 )
			{
				cwmWorldState->ServerProfile()->TimerTimeCount( 0 );
				cwmWorldState->ServerProfile()->TimerTime( 0 );
			}
			
			StartMilliTimer( tempSecs, tempMilli );
			
			cwmWorldState->CheckTimers();
			cwmWorldState->SetUICurrentTime( getclock() );
			tempTime = CheckMilliTimer( tempSecs, tempMilli );
			cwmWorldState->ServerProfile()->IncTimerTime( tempTime );
			cwmWorldState->ServerProfile()->IncTimerTimeCount();
			
			if( cwmWorldState->ServerProfile()->AutoTimeCount() >= 1000 )
			{
				cwmWorldState->ServerProfile()->AutoTimeCount( 0 );
				cwmWorldState->ServerProfile()->AutoTime( 0 );
			}
			StartMilliTimer( tempSecs, tempMilli );
			
			if( !cwmWorldState->GetReloadingScripts() )
				cwmWorldState->CheckAutoTimers();
			
			tempTime = CheckMilliTimer( tempSecs, tempMilli );
			cwmWorldState->ServerProfile()->IncAutoTime( tempTime );
			cwmWorldState->ServerProfile()->IncAutoTimeCount();
			StartMilliTimer( tempSecs, tempMilli );
			Network->ClearBuffers();
			tempTime = CheckMilliTimer( tempSecs, tempMilli );
			cwmWorldState->ServerProfile()->IncNetworkTime( tempTime );
			tempTime = CheckMilliTimer( loopSecs, loopMilli );
			cwmWorldState->ServerProfile()->IncLoopTime( tempTime );
			DoMessageLoop();
		}

		
		Console.PrintSectionBegin();
		sysBroadcast( "The server is shutting down." );
		Console.PrintDone();
		Console << "Closing sockets...";
		netpollthreadclose = true;
		UOXSleep( 1000 );
		Network->SockClose();
		Console.PrintDone();

		if( cwmWorldState->GetWorldSaveProgress() != SS_SAVING )
		{
			do 
			{
				cwmWorldState->SaveNewWorld( true );
			} while( cwmWorldState->GetWorldSaveProgress() == SS_SAVING );
		}
		
		cwmWorldState->ServerData()->save();

		Console.Log( "Server Shutdown!\n=======================================================================\n" , "server.log" );

		conthreadcloseok = true;	//	This will signal the console thread to close
		Shutdown( 0 );

#ifdef _CRASH_PROTECT_
	}
	catch ( ... ) 
	{//Crappy error handling...
		Console << "Unknown exception caught, hard crash avioded!" << myendl;
		Shutdown( UNKNOWN_ERROR );
	}
#endif
	
	return ( 0 );	
}

