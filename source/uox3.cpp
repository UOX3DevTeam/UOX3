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
#include "CJSMapping.h"
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
#include "PartySystem.h"
#include "CJSEngine.h"

namespace UOX
{

#if UOX_PLATFORM != PLATFORM_WIN32
	pthread_t cons, netw;
#else
	#include <process.h>
	#include <conio.h>
#endif

timeval current;

#undef DBGFILE
#define DBGFILE "uox3.cpp"

ObjectFactory *	objFactory;
PartyFactory *	partySys;

//o---------------------------------------------------------------------------o
// FileIO Pre-Declarations
//o---------------------------------------------------------------------------o
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
void		restockNPC( CChar& i, bool stockAll );
void		clearTrades( void );
void		sysBroadcast( const std::string txt );
void		MoveBoat( UI08 dir, CBoatObj *boat );
bool		DecayItem( CItem& toDecay, const UI32 nextDecayItems );
void		CheckAI( CChar& mChar );

void UnloadSpawnRegions( void )
{
	SPAWNMAP_CITERATOR spIter	= cwmWorldState->spawnRegions.begin();
	SPAWNMAP_CITERATOR spEnd	= cwmWorldState->spawnRegions.end();
	while( spIter != spEnd )
	{
		if( spIter->second != NULL )
			delete spIter->second;
		++spIter;
	}
	cwmWorldState->spawnRegions.clear();
}

void UnloadRegions( void )
{
	TOWNMAP_CITERATOR tIter	= cwmWorldState->townRegions.begin();
	TOWNMAP_CITERATOR tEnd	= cwmWorldState->townRegions.end();
	while( tIter != tEnd )
	{
		if( tIter->second != NULL )
			delete tIter->second;
		++tIter;
	}
	cwmWorldState->townRegions.clear();
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
			case MSG_RELOADJS:		JSEngine->Reload();
									JSMapping->Reload();
									Console.PrintDone();
									Commands->Load();								break;
			case MSG_CONSOLEBCAST:	sysBroadcast( tVal.data );						break;
			case MSG_PRINTDONE:		Console.PrintDone();							break;
			case MSG_PRINTFAILED:	Console.PrintFailed();							break;
			case MSG_SECTIONBEGIN:	Console.PrintSectionBegin();					break;
			case MSG_RELOAD:		
				if( !cwmWorldState->GetReloadingScripts() )
				{
					cwmWorldState->SetReloadingScripts( true );
					switch( tVal.data[0] )
					{
					case '0':	cwmWorldState->ServerData()->Load();		break;	// Reload INI file
					case '1':	Accounts->Load();							break;	// Reload accounts
					case '2':	UnloadRegions();	LoadRegions();			break;	// Reload regions
					case '3':	UnloadSpawnRegions();	LoadSpawnRegions();	break;	// Reload spawn regions
					case '4':	Magic->LoadScript();						break;	// Reload spells
					case '5':	JSMapping->Reload( SCPT_COMMAND );	
								Commands->Load();							break;	// Reload commands
					case '6':	FileLookup->Reload();
								LoadCreatures();
								LoadCustomTitle();
								LoadSkills();
								LoadPlaces();
								Skills->Load();								break;	// Reload definition files
					case '7':	JSEngine->Reload();
								JSMapping->Reload();	
								Console.PrintDone(); 	
								Commands->Load();
								Skills->Load();								break;	// Reload JS
					case '8':	HTMLTemplates->Unload();
								HTMLTemplates->Load();						break;	// Reload HTML
					}
					cwmWorldState->SetReloadingScripts( false );
				}
				break;
			case MSG_UNKNOWN:
			default:				Console.Error( "Unknown message type" );		break;
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

bool conthreadcloseok	= false;
bool netpollthreadclose	= false;

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

#if UOX_PLATFORM != PLATFORM_WIN32
void *CheckConsoleKeyThread( void *params )
#else
void CheckConsoleKeyThread( void *params )
#endif
{
	messageLoop << "Thread: CheckConsoleThread has started";
	Console.Registration();
	conthreadcloseok = false;
	while( !conthreadcloseok )
	{
		Console.Poll();
		UOXSleep( 500 );
	}
#if UOX_PLATFORM == PLATFORM_WIN32
	_endthread();		// linux will kill the thread when it returns
#endif
	messageLoop << "Thread: CheckConsoleKeyThread Closed";
#if UOX_PLATFORM != PLATFORM_WIN32
	pthread_exit( NULL );
#endif
}
//	EviLDeD	-	End

#if UOX_PLATFORM != PLATFORM_WIN32

void closesocket( UOXSOCKET s )
{
	shutdown( s, 2 );
	close( s );
}
#endif

//o--------------------------------------------------------------------------o
//|	Function		-	bool isOnline( CChar& mChar )
//|	Date			-	
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Check if the socket owning character c is still connected
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
bool isOnline( CChar& mChar )
{
	if( mChar.IsNpc() )
		return false;
	CAccountBlock& actbTemp = mChar.GetAccount();
	if( actbTemp.wAccountIndex != AB_INVALID_ID )
	{
		if( actbTemp.dwInGame == mChar.GetSerial() )
			return true;
	}
	Network->PushConn();
	for( CSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
	{
		if( tSock->CurrcharObj() == &mChar )
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
		if( !(*cIter)->LoginComplete() )
			continue;
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
	QUEUEMAP_CITERATOR delqIter		= cwmWorldState->deletionQueue.begin();
	QUEUEMAP_CITERATOR delqIterEnd	= cwmWorldState->deletionQueue.end();
	while( delqIter != delqIterEnd )
	{
		CBaseObject *mObj = delqIter->first;
		++delqIter;
		if( mObj == NULL || mObj->isFree() || !mObj->isDeleted() )
		{
			Console.Warning( "Invalid object found in Deletion Queue" );
			continue;
		}
		ObjectFactory::getSingleton().DestroyObject( mObj );
		++objectsDeleted;
	}
	cwmWorldState->deletionQueue.clear();

	Console << " Removed " << objectsDeleted << " objects" << myendl;

	JSEngine->CollectGarbage();
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
	if( s->IsOnHorse() )
		return;

	if( !objInRange( s, x, DIST_NEXTTILE ) )
		return;
	if( x->GetOwnerObj() == s || Npcs->checkPetFriend( s, x ) || s->IsGM() )
	{
		if( !cwmWorldState->ServerData()->CharHideWhileMounted() )
			s->ExposeToView();

		s->SetOnHorse( true );
		CItem *c = Items->CreateItem( NULL, s, 0x0915, 1, x->GetSkin(), OT_ITEM );
		c->SetName( x->GetName() );
		c->SetDecayable( false );
		c->SetLayer( IL_MOUNT );

		if( cwmWorldState->creatures[x->GetID()].MountID() != 0 )
			c->SetID( cwmWorldState->creatures[x->GetID()].MountID() );
		else
			c->SetID( 0x3E00 );

		if( !c->SetCont( s ) )
		{
			s->SetOnHorse( false );	// let's get off our horse again
			c->Delete();
			return;
		}

		SOCKLIST nearbyChars = FindNearbyPlayers( s );
		for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
		{
			s->SendWornItems( (*cIter) );
		}

		if( x->GetTarg() != NULL )	// zero out target, under all circumstances
		{
			x->SetTarg( NULL );
			if( x->IsAtWar() )
				x->ToggleCombat();
		}
		if( ValidateObject( x->GetAttacker() ) )
			x->GetAttacker()->SetTarg( NULL );
		x->SetFrozen( true );
		x->SetMounted( true );
		x->SetInvulnerable( true );
		x->SetLocation( 7000, 7000, 0 );

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
		tMount->SetMounted( false );
		tMount->SetInvulnerable( false );
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
	const UI32 igetclock = cwmWorldState->GetUICurrentTime();
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
	Console.Error( "Illegal Instruction Signal caught - attempting shutdown" );
	endmessage( x );
}

void aus( int signal )
{
	Console.Error( "Server crash averted! Floating point exception caught." );
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
				Combat->SpawnGuard( mChar, attacker, attacker->GetX(), attacker->GetY(), attacker->GetZ() );
				return;
			}
		}
	}

	CMapRegion *toCheck = MapRegion->GetMapRegion( mChar );
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
				Combat->SpawnGuard( mChar, tempChar, tempChar->GetX(), tempChar->GetY(), tempChar->GetZ() );
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
//|	Function	-	bool genericCheck( CSocket *mSock, CChar *mChar )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check characters status.  Returns true if character was killed
//o---------------------------------------------------------------------------o
bool genericCheck( CSocket *mSock, CChar& mChar, bool checkFieldEffects, bool doWeather )
{
	if( mChar.IsDead() )
		return false;
	
	LIGHTLEVEL toShow;
	UI16 c;
	if( mChar.GetHP() > mChar.GetMaxHP() )
		mChar.SetHP( mChar.GetMaxHP() );
	if( mChar.GetStamina() > mChar.GetMaxStam() )
		mChar.SetStamina( mChar.GetMaxStam() );
	if( mChar.GetMana() > mChar.GetMaxMana() )
		mChar.SetMana( mChar.GetMaxMana() );

	if( mChar.GetRegen( 0 ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
	{
		if( mChar.GetHP() < mChar.GetMaxHP() )
		{
			if( mChar.GetHunger() > 0 || ( !Races->DoesHunger( mChar.GetRace() ) && ( cwmWorldState->ServerData()->SystemTimer( tSERVER_HUNGERRATE ) == 0 || mChar.IsNpc() ) ) )
			{
				for( c = 0; c < mChar.GetMaxHP() + 1; ++c )
				{
					if( mChar.GetHP() <= mChar.GetMaxHP() && ( mChar.GetRegen( 0 ) + ( c * cwmWorldState->ServerData()->SystemTimer( tSERVER_HITPOINTREGEN ) * 1000) ) <= cwmWorldState->GetUICurrentTime() )
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
		}
		mChar.SetRegen( cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_HITPOINTREGEN ), 0 );
	}
	if( mChar.GetRegen( 1 ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
	{
		if( mChar.GetStamina() < mChar.GetMaxStam() )
		{
			for( c = 0; c < mChar.GetMaxStam() + 1; ++c )
			{
				if( ( mChar.GetRegen( 1 ) + ( c * cwmWorldState->ServerData()->SystemTimer( tSERVER_STAMINAREGEN ) * 1000) ) <= cwmWorldState->GetUICurrentTime() && mChar.GetStamina() <= mChar.GetMaxStam() )
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
		mChar.SetRegen( cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_STAMINAREGEN ), 1 );
	}

	// CUSTOM START - SPUD:MANA REGENERATION:Rewrite of passive and active meditation code
	if( mChar.GetRegen( 2 ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
	{
		if( mChar.GetMana() < mChar.GetMaxMana() )
		{
			for( c = 0; c < mChar.GetMaxMana() + 1; ++c )
			{
				if( mChar.GetRegen( 2 ) + ( c * cwmWorldState->ServerData()->SystemTimer( tSERVER_MANAREGEN ) * 1000) <= cwmWorldState->GetUICurrentTime() && mChar.GetMana() <= mChar.GetMaxMana() )
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
		const R32 MeditationBonus = ( .00075f * mChar.GetSkill( MEDITATION ) );	// Bonus for Meditation
		int NextManaRegen = static_cast<int>(cwmWorldState->ServerData()->SystemTimer( tSERVER_MANAREGEN ) * ( 1 - MeditationBonus ) * 1000);
		if( cwmWorldState->ServerData()->ArmorAffectManaRegen() )	// If armor effects mana regeneration...
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

	// Hunger Code
	mChar.DoHunger( mSock );
	
	if( !mChar.IsInvulnerable() && mChar.GetPoisoned() > 0 )
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
							mChar.TextMessage( NULL, 1240, EMOTE, true, mChar.GetName().c_str() );
						}
						mChar.Damage( (SI16)RandomNum( 1, 2 ) );
						break;
					case 2:
						mChar.SetTimer( tCHAR_POISONTIME, BuildTimeValue( 4 ) );
						if( mChar.GetTimer( tCHAR_POISONTEXT ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
						{
							mChar.SetTimer( tCHAR_POISONTEXT, BuildTimeValue( 10 ) );
							mChar.TextMessage( NULL, 1241, EMOTE, true, mChar.GetName().c_str() );
						}
						pcalc = (SI16)( ( mChar.GetHP() * RandomNum( 2, 5 ) / 100 ) + RandomNum( 0, 2 ) ); // damage: 1..2..5% of hp's+ 1..2 constant
						mChar.Damage( (SI16)pcalc );
						break;
					case 3:
						mChar.SetTimer( tCHAR_POISONTIME, BuildTimeValue( 3 ) );
						if( mChar.GetTimer( tCHAR_POISONTEXT ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
						{
							mChar.SetTimer( tCHAR_POISONTEXT, BuildTimeValue( 10 ) );
							mChar.TextMessage( NULL, 1242, EMOTE, true, mChar.GetName().c_str() );
						}
						pcalc = (SI16)( ( mChar.GetHP() * RandomNum( 5, 10 ) / 100 ) + RandomNum( 1, 3 ) ); // damage: 5..10% of hp's+ 1..2 constant
						mChar.Damage( (SI16)pcalc );
						break;
					case 4:
						mChar.SetTimer( tCHAR_POISONTIME, BuildTimeValue( 3 ) );
						if( mChar.GetTimer( tCHAR_POISONTEXT ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
						{
							mChar.SetTimer( tCHAR_POISONTEXT, BuildTimeValue( 10 ) );
							mChar.TextMessage( NULL, 1243, EMOTE, true, mChar.GetName().c_str() );
						}
						pcalc = (SI16)( mChar.GetHP() / 5 + RandomNum( 3, 6 ) ); // damage: 20% of hp's+ 3..6 constant, quite deadly <g>
						mChar.Damage( (SI16)pcalc );
						break;
					default:
						Console.Error( " Fallout of switch statement without default. uox3.cpp, genericCheck()" );
						mChar.SetPoisoned( 0 );
						break;
				}
				if( mChar.GetHP() < 1 && !mChar.IsDead() )
				{
					HandleDeath( (&mChar) );
					if( mSock != NULL )
						mSock->sysmessage( 1244 );
				} 
			}
		}
	}

	if( mChar.GetTimer( tCHAR_POISONWEAROFF ) <= cwmWorldState->GetUICurrentTime() )
	{
		if( mChar.GetPoisoned() > 0 )
		{
            mChar.SetPoisoned( 0 );
			if( mSock != NULL )
				mSock->sysmessage( 1245 );
		}
	}

	if( !mChar.GetCanAttack() && mChar.GetTimer( tCHAR_PEACETIMER ) <= cwmWorldState->GetUICurrentTime() )
	{
		mChar.SetCanAttack( true );
		if( mSock != NULL )
			mSock->sysmessage( 1779 );
	}
	
	if( mChar.IsCriminal() && mChar.GetTimer( tCHAR_CRIMFLAG ) && ( mChar.GetTimer( tCHAR_CRIMFLAG ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() ) )
	{
		if( mSock != NULL )
			mSock->sysmessage( 1238 );
		mChar.SetTimer( tCHAR_CRIMFLAG, 0 );
		UpdateFlag( &mChar );
	}
	if( mChar.GetKills() && ( mChar.GetTimer( tCHAR_MURDERRATE ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() ) )
	{
		mChar.SetKills( static_cast<SI16>( mChar.GetKills() - 1 ) );

		if( mChar.GetKills() )
			mChar.SetTimer( tCHAR_MURDERRATE, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_MURDERDECAY ) );
		else
			mChar.SetTimer( tCHAR_MURDERRATE, 0 );
		if( mSock != NULL && mChar.GetKills() == cwmWorldState->ServerData()->RepMaxKills() )
			mSock->sysmessage( 1239 );
		UpdateFlag( &mChar );
	}

	if( doWeather )
	{
		const UI08 curLevel = cwmWorldState->ServerData()->WorldLightCurrentLevel();
		if( Races->VisLevel( mChar.GetRace() ) > curLevel )
			toShow = 0;
		else
			toShow = static_cast<UI08>( curLevel - Races->VisLevel( mChar.GetRace() ) );
		if( mChar.IsNpc() )
			doLight( &mChar, toShow );
		else
			doLight( mSock, toShow );
	}

	Weather->doLightEffect( mSock, mChar );
	Weather->doWeatherEffect( mSock, mChar, RAIN );
	Weather->doWeatherEffect( mSock, mChar, SNOW );
	Weather->doWeatherEffect( mSock, mChar, HEAT );
	Weather->doWeatherEffect( mSock, mChar, COLD );
	Weather->doWeatherEffect( mSock, mChar, STORM );

	if( checkFieldEffects )
		Magic->CheckFieldEffects( mChar );

	mChar.UpdateDamageTrack();

	if( mChar.IsDead() )
		return true;
	else if( mChar.GetHP() <= 0 )
	{
		HandleDeath( (&mChar) );
		return true;
	}
	return false;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void checkPC( CChar& mChar, bool doWeather )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check a PC's status
//o---------------------------------------------------------------------------o
void checkPC( CSocket *mSock, CChar& mChar )
{
	Combat->CombatLoop( mSock, mChar );
	
	if( mChar.GetSquelched() == 2 )
	{
		if( mSock->GetTimer( tPC_MUTETIME ) != 0 && ( mSock->GetTimer( tPC_MUTETIME ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() ) )
		{
			mChar.SetSquelched( 0 );
			mSock->SetTimer( tPC_MUTETIME, 0 );
			mSock->sysmessage( 1237 );
		}
	}

	if( mChar.IsCasting() && !mChar.IsJSCasting() )	// Casting a spell
	{
		mChar.SetNextAct( mChar.GetNextAct() - 1 );
		if( mChar.GetTimer( tCHAR_SPELLTIME ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )//Spell is complete target it.
		{
			if( Magic->spells[mChar.GetSpellCast()].RequireTarget() )
			{
				mChar.SetCasting( false );
				mChar.SetFrozen( false );
				mSock->target( 0, TARGET_CASTSPELL, Magic->spells[mChar.GetSpellCast()].StringToSay().c_str() );
			}
			else
			{
				mChar.SetCasting( false );
				Magic->CastSpell( mSock, &mChar );
				mChar.SetTimer( tCHAR_SPELLTIME, 0 );
				mChar.SetFrozen( false );
			}
		} 
		else if( mChar.GetNextAct() <= 0 )//redo the spell action
		{
			mChar.SetNextAct( 75 );
			if( !mChar.IsOnHorse() )
				Effects->PlaySpellCastingAnimation( &mChar, Magic->spells[mChar.GetSpellCast()].Action() );
		}
	}
	
	if( cwmWorldState->ServerData()->WorldAmbientSounds() >= 1 )
	{
		if( cwmWorldState->ServerData()->WorldAmbientSounds() > 10 )
			cwmWorldState->ServerData()->WorldAmbientSounds( 10 );
		const SI16 soundTimer = static_cast<SI16>(cwmWorldState->ServerData()->WorldAmbientSounds() * 100);
		if( !mChar.IsDead() && ( RandomNum( 0, soundTimer - 1 ) ) == ( soundTimer / 2 ) )
			Effects->PlayBGSound( (*mSock), mChar );
	}
	
	if( mSock->GetTimer( tPC_SPIRITSPEAK ) > 0 && mSock->GetTimer( tPC_SPIRITSPEAK) < cwmWorldState->GetUICurrentTime() )
		mSock->SetTimer( tPC_SPIRITSPEAK, 0 );
	
	if( mSock->GetTimer( tPC_TRACKING ) > cwmWorldState->GetUICurrentTime() )
	{
		if( mSock->GetTimer( tPC_TRACKINGDISPLAY ) <= cwmWorldState->GetUICurrentTime() )
		{
			mSock->SetTimer( tPC_TRACKINGDISPLAY, BuildTimeValue( (R32)cwmWorldState->ServerData()->TrackingRedisplayTime() ) );
			Skills->Track( &mChar );
		}
	}
	else
	{
		if( mSock->GetTimer( tPC_TRACKING ) > ( cwmWorldState->GetUICurrentTime() / 10 ) ) // dont send arrow-away packet all the time
		{
			mSock->SetTimer( tPC_TRACKING, 0 );
			if( ValidateObject( mChar.GetTrackingTarget() ) )
			{
				CPTrackingArrow tSend = (*mChar.GetTrackingTarget());
				tSend.Active( 0 );
				mSock->Send( &tSend );
			}
		}
	}
	
	if( mSock->GetTimer( tPC_FISHING ) )
	{
		if( mSock->GetTimer( tPC_FISHING ) <= cwmWorldState->GetUICurrentTime() )
		{
			Skills->Fish( mSock, &mChar );
			mSock->SetTimer( tPC_FISHING, 0 );
		}
	}
	
	if( mChar.IsOnHorse() )
	{
		CItem *horseItem = mChar.GetItemAtLayer( IL_MOUNT );
		if( !ValidateObject( horseItem ) )
			mChar.SetOnHorse( false );	// turn it off, we aren't on one because there's no item!
		else if( horseItem->GetDecayTime() != 0 && ( horseItem->GetDecayTime() <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() ) )
		{
			mChar.SetOnHorse( false );
			horseItem->Delete();
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void checkNPC( CChar& mChar, bool checkAI, bool doRestock )
//|	Programmer	-	UOX Devteam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check NPC's status
//o---------------------------------------------------------------------------o
void checkNPC( CChar& mChar, bool checkAI, bool doRestock, bool doPetOfflineCheck )
{
	// okay, this will only ever trigger after we check an npc...  Question is:
	// should we remove the time delay on the AI check as well?  Just stick with AI/movement
	// AI can never be faster than how often we check npcs

	const UI16 AITrig	= mChar.GetScriptTrigger();
	cScript *toExecute	= JSMapping->GetScript( AITrig );
	bool doAICheck		= true;
	if( toExecute != NULL )
	{
		if( toExecute->OnAISliver( &mChar ) )
			doAICheck = false;
	}
	if( doAICheck && checkAI )
		CheckAI( mChar );
	Movement->NpcMovement( mChar );

	if( doRestock )
		restockNPC( mChar, false );

	if( doPetOfflineCheck )
	{
		mChar.checkPetOfflineTimeout();
	}

	if( mChar.GetTimer( tNPC_SUMMONTIME ) )
	{
		if( mChar.GetTimer( tNPC_SUMMONTIME ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
		{
			// Dupois - Added Dec 20, 1999
			// QUEST expire check - after an Escort quest is created a timer is set
			// so that the NPC will be deleted and removed from the game if it hangs around
			// too long without every having its quest accepted by a player so we have to remove 
			// its posting from the messageboard before icing the NPC
			// Only need to remove the post if the NPC does not have a follow target set
			if( mChar.GetQuestType() == QT_ESCORTQUEST && !ValidateObject( mChar.GetFTarg() ) )
			{
				MsgBoardQuestEscortRemovePost( &mChar );
				mChar.Delete();
				return;
			}
			// Dupois - End
			if( mChar.GetNPCAiType() == AI_GUARD && mChar.IsAtWar() )
			{
				mChar.SetTimer( tNPC_SUMMONTIME, BuildTimeValue( 25 ) );
				return;
			}
			Effects->PlaySound( &mChar, 0x01FE );
			mChar.SetDead( true );
			mChar.Delete();
			return;
		}
	}
	
	if( mChar.GetFleeAt() == 0 )
		mChar.SetFleeAt( cwmWorldState->ServerData()->CombatNPCBaseFleeAt() );
	if( mChar.GetReattackAt() == 0 )
		mChar.SetReattackAt( cwmWorldState->ServerData()->CombatNPCBaseReattackAt() );
	
	if( mChar.GetNpcWander() != WT_FLEE && ( mChar.GetHP() < mChar.GetMaxHP() * mChar.GetFleeAt() / 100 ) )
	{
		mChar.SetOldNpcWander( mChar.GetNpcWander() );
		mChar.SetNpcWander( WT_FLEE );
		mChar.SetTimer( tNPC_MOVETIME, BuildTimeValue( mChar.GetFleeingSpeed() ) );
	}
	else if( mChar.GetNpcWander() == WT_FLEE && (mChar.GetHP() > mChar.GetMaxHP() * mChar.GetReattackAt() / 100))
	{
		mChar.SetNpcWander( mChar.GetOldNpcWander() );
		mChar.SetTimer( tNPC_MOVETIME, BuildTimeValue( mChar.GetWalkingSpeed() ) );
		mChar.SetOldNpcWander( WT_NONE ); // so it won't save this at the wsc file
	}
	Combat->CombatLoop( NULL, mChar );
}

void checkItem( CMapRegion *toCheck, bool checkItems, UI32 nextDecayItems )
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
			case IT_ITEMSPAWNER:
			case IT_NPCSPAWNER:
			case IT_SPAWNCONT:
			case IT_LOCKEDSPAWNCONT:
			case IT_UNLOCKABLESPAWNCONT:
			case IT_AREASPAWNER:
			case IT_ESCORTNPCSPAWNER:
				if( itemCheck->GetTempTimer() <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
				{
					if( itemCheck->GetObjType() == OT_SPAWNER )
					{
						CSpawnItem *spawnItem = static_cast<CSpawnItem *>(itemCheck);
						if( spawnItem->DoRespawn() )
						continue;
						spawnItem->SetTempTimer( BuildTimeValue( static_cast<R32>(RandomNum( spawnItem->GetInterval( 0 ) * 60, spawnItem->GetInterval( 1 ) * 60 ) ) ) );
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
		if( itemCheck->CanBeObjType( OT_BOAT ) )
		{
			CBoatObj *mBoat = static_cast<CBoatObj *>(itemCheck);
			if( ValidateObject( mBoat ) && mBoat->GetMoveType() && 
				( mBoat->GetMoveTime() <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() ) )
			{
				if( mBoat->GetMoveType() == 1 )
					MoveBoat( itemCheck->GetDir(), mBoat );
				else if( mBoat->GetMoveType() == 2 )
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
	bool doPetOfflineCheck				= false;
	MAPUSERNAMEID_ITERATOR I;

	// modify this stuff to take into account more variables
	if( accountFlush <= GetUICurrentTime() || GetOverflow() )
	{
		bool reallyOn = false;
		// time to flush our account status!
		for( I = Accounts->begin(); I != Accounts->end(); ++I )
		{
			CAccountBlock& actbTemp = I->second;
			if( actbTemp.wAccountIndex == AB_INVALID_ID)
				continue;

			if( actbTemp.wFlags.test( AB_FLAGS_ONLINE ) )
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
				{
					actbTemp.wFlags.reset( AB_FLAGS_ONLINE );
				}
			}
		}
		accountFlush = BuildTimeValue( (R32)ServerData()->AccountFlushTimer() );
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
		TOWNMAP_CITERATOR tIter	= cwmWorldState->townRegions.begin();
		TOWNMAP_CITERATOR tEnd	= cwmWorldState->townRegions.end();
		while( tIter != tEnd )
		{
			CTownRegion *myReg = tIter->second;
			if( myReg != NULL )
				myReg->PeriodicCheck();
			++tIter;
		}
		nextCheckTownRegions = BuildTimeValue( 10 );	// do checks every 10 seconds or so, rather than every single time
		JailSys->PeriodicCheck();
	}

	if( nextCheckSpawnRegions <= GetUICurrentTime() && ServerData()->CheckSpawnRegionSpeed() != -1 )//Regionspawns
	{
		UI16 itemsSpawned	= 0;
		UI16 npcsSpawned	= 0;
		SPAWNMAP_CITERATOR spIter	= cwmWorldState->spawnRegions.begin();
		SPAWNMAP_CITERATOR spEnd	= cwmWorldState->spawnRegions.end();
		while( spIter != spEnd )
		{
			CSpawnRegion *spawnReg = spIter->second;
			if( spawnReg != NULL )
			{
				if( spawnReg->GetNextTime() <= GetUICurrentTime() )
                    spawnReg->doRegionSpawn( itemsSpawned, npcsSpawned );
			}
			++spIter;
		}
		nextCheckSpawnRegions = BuildTimeValue( (R32)ServerData()->CheckSpawnRegionSpeed() );//Don't check them TOO often (Keep down the lag)
	}
	
	HTMLTemplates->Poll( ETT_ALLTEMPLATES );

	const UI32 saveinterval = ServerData()->ServerSavesTimerStatus();
	if( saveinterval != 0 )
	{
		time_t oldTime = GetOldTime();
		if( !GetAutoSaved() )
		{
			SetAutoSaved( true );
			time(&oldTime);
			SetOldTime( oldTime );
		}
		time_t newTime = GetNewTime();
		time(&newTime);
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
	
	time_t oldIPTime = GetOldIPTime();
	if( !GetIPUpdated() )
	{
		SetIPUpdated( true );
		time(&oldIPTime);
		SetOldIPTime( oldIPTime );
	}
	time_t newIPTime = GetNewIPTime();
	time(&newIPTime);
	SetNewIPTime( newIPTime );
	
	if( difftime( GetNewIPTime(), GetOldIPTime() ) >= 120 )
	{
		ServerData()->RefreshIPs();
		SetIPUpdated( false );
	}
	
	//Time functions
	if( GetUOTickCount() <= GetUICurrentTime() || ( GetOverflow() ) )
	{
		UI08 oldHour = ServerData()->ServerTimeHours();
		if( ServerData()->incMinute() )
			Weather->NewDay();
		if( oldHour != ServerData()->ServerTimeHours() )
			Weather->NewHour();

		SetUOTickCount( BuildTimeValue( ServerData()->ServerSecondsPerUOMinute() ) );
	}
	
	if( GetTimer( tWORLD_LIGHTTIME ) <= GetUICurrentTime() || GetOverflow() )
	{
		doWorldLight();  //Changes lighting, if it is currently time to.
		Weather->DoStuff();	// updates the weather types
		SetTimer( tWORLD_LIGHTTIME, ServerData()->BuildSystemTimeValue( tSERVER_WEATHER ) );
		doWeather = true;
	}

	if( GetTimer( tWORLD_PETOFFLINECHECK ) <= GetUICurrentTime() || GetOverflow() )
	{
		SetTimer( tWORLD_PETOFFLINECHECK, ServerData()->BuildSystemTimeValue( tSERVER_PETOFFLINECHECK ) );
		doPetOfflineCheck = true;
	}

	bool checkFieldEffects = false;
	if( GetTimer( tWORLD_NEXTFIELDEFFECT ) <= GetUICurrentTime() || GetOverflow() )
	{
		checkFieldEffects = true;
		SetTimer( tWORLD_NEXTFIELDEFFECT, BuildTimeValue( 0.5f ) );
	}
	std::set< CMapRegion * > regionList;
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
			genericCheck( iSock, (*mChar), checkFieldEffects, doWeather );
			checkPC( iSock, (*mChar) );

			SI16 xOffset = MapRegion->GetGridX( mChar->GetX() );
			SI16 yOffset = MapRegion->GetGridY( mChar->GetY() );
			for( SI08 counter = -1; counter <= 1; ++counter ) // Check 3 x colums
			{
				for( SI08 ctr2 = -1; ctr2 <= 1; ++ctr2 ) // Check 3 y colums
				{
					CMapRegion *tC = MapRegion->GetMapRegion( xOffset + counter, yOffset + ctr2, worldNumber );
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
		nextDecayItems = ServerData()->BuildSystemTimeValue( tSERVER_DECAY );
		checkItems = true;
	}
	if( GetTimer( tWORLD_NEXTNPCAI ) <= GetUICurrentTime() || GetOverflow() )
	{
		SetTimer( tWORLD_NEXTNPCAI, BuildTimeValue( (R32)ServerData()->CheckNpcAISpeed() ) );
		checkAI = true;
	}
	if( GetTimer( tWORLD_SHOPRESTOCK ) <= GetUICurrentTime() || GetOverflow() )
	{
		SetTimer( tWORLD_SHOPRESTOCK, ServerData()->BuildSystemTimeValue( tSERVER_SHOPSPAWN ) );
		doRestock = true;
	}

	std::set< CMapRegion * >::const_iterator tcCheck = regionList.begin();
	while( tcCheck != regionList.end() )
	{
		CMapRegion *toCheck = (*tcCheck);
		CDataList< CChar * > *regChars = toCheck->GetCharList();
		regChars->Push();
		for( CChar *charCheck = regChars->First(); !regChars->Finished(); charCheck = regChars->Next() )
		{
			if( !ValidateObject( charCheck ) || charCheck->isFree() )
				continue;
			if( charCheck->IsNpc() )
			{
				if( !genericCheck( NULL, (*charCheck), checkFieldEffects, doWeather ) )
				{
					if( setNPCFlags )
						UpdateFlag( charCheck );	 // only set flag on npcs every 60 seconds (save a little extra lag)
					checkNPC( (*charCheck), checkAI, doRestock, doPetOfflineCheck );
				}
			}
			else if( charCheck->GetTimer( tPC_LOGOUT ) )
			{
				CAccountBlock& actbTemp = charCheck->GetAccount();
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
	QUEUEMAP_CITERATOR rqIter			= cwmWorldState->refreshQueue.begin();
	QUEUEMAP_CITERATOR rqIterEnd		= cwmWorldState->refreshQueue.end();
	while( rqIter != rqIterEnd )
	{
		CBaseObject *mObj = rqIter->first;
		if( ValidateObject( mObj ) )
		{
			if( mObj->CanBeObjType( OT_CHAR ) )
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
				else if( uChar->GetUpdate( UT_HIDE ) )
				{
					uChar->RemoveFromSight();
					uChar->Update();
				}
				else if( uChar->GetUpdate( UT_UPDATE ) )
					uChar->Update();
				else if( uChar->GetUpdate( UT_STATWINDOW ) )
				{
					CSocket *uSock = uChar->GetSocket();
					if( uSock != NULL )
						uSock->statwindow( uChar );
				}

				uChar->ClearUpdate();
			}
			else
				mObj->Update();
		}
		++rqIter;
	}
	cwmWorldState->refreshQueue.clear();
}

//o---------------------------------------------------------------------------o
//|	Function	-	void InitClasses( void )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Initialize UOX classes
//o---------------------------------------------------------------------------o
void InitClasses( void )
{
	cwmWorldState->ClassesInitialized( true );

	JSMapping		= NULL;	Effects		= NULL;
	Commands		= NULL;	Combat		= NULL;
	Items			= NULL;	Map			= NULL;
	Npcs			= NULL;	Skills		= NULL;	
	Weight			= NULL;	JailSys		= NULL;
	Network			= NULL;	Magic		= NULL;		
	Races			= NULL;	Weather		= NULL;
	Movement		= NULL;	GuildSys	= NULL;
	WhoList			= NULL;	OffList		= NULL;
	Books			= NULL;	GMQueue		= NULL;	
	Dictionary		= NULL;	Accounts	= NULL;
	MapRegion		= NULL;	SpeechSys	= NULL;
	CounselorQueue	= NULL;
	HTMLTemplates	= NULL;
	FileLookup		= NULL;
	objFactory		= NULL; partySys	= NULL;

	objFactory		= new ObjectFactory;
	partySys		= new PartyFactory;
	JSEngine		= new CJSEngine;
	// MAKE SURE IF YOU ADD A NEW ALLOCATION HERE THAT YOU FREE IT UP IN Shutdown(...)
	if(( FileLookup		= new CServerDefinitions() )			== NULL ) Shutdown( FATAL_UOX3_ALLOC_SCRIPTS );
	if(( Dictionary		= new CDictionaryContainer )			== NULL ) Shutdown( FATAL_UOX3_ALLOC_DICTIONARY );
	if(( Combat			= new CHandleCombat )					== NULL ) Shutdown( FATAL_UOX3_ALLOC_COMBAT );
	if(( Commands		= new cCommands )						== NULL ) Shutdown( FATAL_UOX3_ALLOC_COMMANDS );
	if(( Items			= new cItem )							== NULL ) Shutdown( FATAL_UOX3_ALLOC_ITEMS );
	if(( Map			= new CMulHandler )						== NULL ) Shutdown( FATAL_UOX3_ALLOC_MAP );
	if(( Npcs			= new cCharStuff )						== NULL ) Shutdown( FATAL_UOX3_ALLOC_NPCS );
	if(( Skills			= new cSkills )							== NULL ) Shutdown( FATAL_UOX3_ALLOC_SKILLS );
	if(( Weight			= new CWeight )							== NULL ) Shutdown( FATAL_UOX3_ALLOC_WEIGHT );
	if(( Network		= new cNetworkStuff )					== NULL ) Shutdown( FATAL_UOX3_ALLOC_NETWORK );
	if(( Magic			= new cMagic )							== NULL ) Shutdown( FATAL_UOX3_ALLOC_MAGIC );
	if(( Races			= new cRaces )							== NULL ) Shutdown( FATAL_UOX3_ALLOC_RACES );
	if(( Weather		= new cWeatherAb )						== NULL ) Shutdown( FATAL_UOX3_ALLOC_WEATHER );
	if(( Movement		= new cMovement )						== NULL ) Shutdown( FATAL_UOX3_ALLOC_MOVE );
	if(( WhoList		= new cWhoList )						== NULL ) Shutdown( FATAL_UOX3_ALLOC_WHOLIST );	// wholist
	if(( OffList		= new cWhoList( false ) )				== NULL ) Shutdown( FATAL_UOX3_ALLOC_WHOLIST );	// offlist
	if(( Books			= new cBooks )							== NULL ) Shutdown( FATAL_UOX3_ALLOC_BOOKS );
	if(( GMQueue		= new PageVector( "GM Queue" ) )		== NULL ) Shutdown( FATAL_UOX3_ALLOC_PAGEVECTOR );
	if(( CounselorQueue	= new PageVector( "Counselor Queue" ) )	== NULL ) Shutdown( FATAL_UOX3_ALLOC_PAGEVECTOR );
	if(( JSMapping		= new CJSMapping )						== NULL ) Shutdown( FATAL_UOX3_ALLOC_TRIGGERS );
	JSMapping->ResetDefaults();
	JSMapping->GetEnvokeByID()->Parse();
	JSMapping->GetEnvokeByType()->Parse();
	if(( MapRegion		= new CMapHandler )						== NULL ) Shutdown( FATAL_UOX3_ALLOC_MAPREGION );
	if(( Effects		= new cEffects )						== NULL ) Shutdown( FATAL_UOX3_ALLOC_EFFECTS );
	if(( HTMLTemplates	= new cHTMLTemplates )					== NULL ) Shutdown( FATAL_UOX3_ALLOC_HTMLTEMPLATES );
	if(( Accounts		= new cAccountClass( cwmWorldState->ServerData()->Directory( CSDDP_ACCOUNTS ) ) ) == NULL ) Shutdown( FATAL_UOX3_ALLOC_ACCOUNTS );
	if(( SpeechSys		= new CSpeechQueue()	)				== NULL ) Shutdown( FATAL_UOX3_ALLOC_SPEECHSYS );
	if(( GuildSys		= new CGuildCollection() )				== NULL ) Shutdown( FATAL_UOX3_ALLOC_GUILDS );
	if(( JailSys		= new JailSystem() )					== NULL ) Shutdown( FATAL_UOX3_ALLOC_JAILSYS );
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
		if( !strcmp( argv[i], "-ERROR" ) )
		{
			cwmWorldState->SetErrorCount( UString( argv[i+1] ).toULong() );
			++i;
		}
		else if( !strcmp( argv[i], "-dumptags" ) )
		{
			cwmWorldState->ServerData()->dumpLookup( 0 );
			cwmWorldState->ServerData()->save( "./uox.tst.ini" );
			Shutdown( FATAL_UOX3_SUCCESS );
		}
		else if( !strcmp( argv[i], "-cluox100" ) )
		{
			++i;
			if( i > argc )
			{
				Console.Error( "Fatal error in CLUOX arguments" );
				Shutdown( 10 );
			}
			Console.Cloak( argv[i] );
		}
	}
}

bool FindMultiFunctor( CBaseObject *a, UI32 &b, void *extraData )
{
	if( ValidateObject( a ) )
	{
		CMultiObj *multi = findMulti( a );
		if( multi != NULL )
			a->SetMulti( multi );
		else
			a->SetMulti( INVALIDSERIAL );
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
			
			sprintf( temp, "Server crash #%lu from unknown error, restarting.", cwmWorldState->GetErrorCount() );
			Console.Log( temp, "server.log" );
			Console << temp << myendl;
			
			sprintf(temp, "uox.exe -ERROR %lu", cwmWorldState->GetErrorCount() );
			
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

	if( cwmWorldState->ServerData()->ServerCrashProtectionStatus() >= 1 && retCode && cwmWorldState && cwmWorldState->GetLoaded() && cwmWorldState->GetWorldSaveProgress() != SS_SAVING )
	{//they want us to save, there has been an error, we have loaded the world, and WorldState is a valid pointer.
		do
		{
			cwmWorldState->SaveNewWorld( true );
		} while( cwmWorldState->GetWorldSaveProgress() == SS_SAVING );
	}

	if( cwmWorldState->ClassesInitialized() )
	{
		if( HTMLTemplates )
		{
			Console << "HTMLTemplates object detected. Writing Offline HTML Now..." << myendl;
			HTMLTemplates->Poll( ETT_OFFLINE );
		}
		else
			Console << "HTMLTemplates object not found." << myendl;

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
		delete WhoList;
		delete OffList;
		delete Books;
		delete GMQueue;
		delete HTMLTemplates;
		delete CounselorQueue;
		delete Dictionary;
		delete Accounts;
		delete JSMapping;
		delete MapRegion;
		delete SpeechSys;
		delete GuildSys;
		delete FileLookup;
		delete JailSys;
		delete Effects;

		UnloadSpawnRegions();
		UnloadRegions();

		delete JSEngine;

		Console.PrintDone();
	}

	//Lets wait for console thread to quit here
#if UOX_PLATFORM != PLATFORM_WIN32
	pthread_join( cons, NULL );
#ifdef __LOGIN_THREAD__
	pthread_join( netw, NULL );
#endif
#endif

	// don't leave file pointers open, could lead to file corruption

	Console.PrintSectionBegin();
	if( retCode && cwmWorldState->GetLoaded() )//do restart unless we have crashed with some error.
		Restart( (UI16)retCode );
	else
		delete cwmWorldState;

	Console.TurnGreen();

	delete partySys;
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
		DFNTAGS tag			= DFNTAG_COUNTOFTAGS;
		UString cdata;
		SI32 ndata			= -1, odata = -1;
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
				case DFNTAG_BUSHIDO:			skillToSet = BUSHIDO;							break;
				case DFNTAG_CAMPING:			skillToSet = CAMPING;							break;
				case DFNTAG_CARPENTRY:			skillToSet = CARPENTRY;							break;
				case DFNTAG_CARTOGRAPHY:		skillToSet = CARTOGRAPHY;						break;
				case DFNTAG_CHIVALRY:			skillToSet = CHIVALRY;							break;
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
													{
														retitem->SetCont( applyTo->GetPackItem() );
														retitem->PlaceInPack();
													}
												}
												break;
				case DFNTAG_FAME:				applyTo->SetFame( static_cast<SI16>(ndata) );	break;
				case DFNTAG_FENCING:			skillToSet = FENCING;							break;
				case DFNTAG_FISHING:			skillToSet = FISHING;							break;
				case DFNTAG_FOCUS:				skillToSet = FOCUS;								break;
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
				case DFNTAG_NECROMANCY:			skillToSet = NECROMANCY;					break;
				case DFNTAG_NINJITSU:			skillToSet = NINJITSU;						break;
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
		CSocket *sock = applyTo->GetSocket();
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
}

//o---------------------------------------------------------------------------o
//|	Function	-	R32 roundNumber( R32 toRound )
//|	Programmer	-	Grimson
//o---------------------------------------------------------------------------o
//|	Purpose		-	rounds a number up or down depending on it's value
//o---------------------------------------------------------------------------o
R32 roundNumber( R32 toRound)
{
	R32 flVal = floor( toRound );
	if( flVal < floor( toRound + 0.5 ) )
		return ceil( toRound );
	return flVal;
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
	
	if( (Races->Affect( mChar->GetRace(), LIGHT )) && mChar->GetWeathDamage( LIGHT ) == 0 )
		mChar->SetWeathDamage( static_cast<UI32>(BuildTimeValue( static_cast<R32>(Races->Secs( mChar->GetRace(), LIGHT )) )), LIGHT );
	
	if( mChar->GetFixedLight() != 255 )
	{
		toSend.Level( mChar->GetFixedLight() );
		s->Send( &toSend );
		Weather->DoPlayerStuff( s, mChar );
		return;
	}

	CTownRegion *curRegion	= mChar->GetRegion();
	CWeather *wSys			= Weather->Weather( curRegion->GetWeather() );
	LIGHTLEVEL toShow;

	LIGHTLEVEL dunLevel = cwmWorldState->ServerData()->DungeonLightLevel();
	// we have a valid weather system
	if( wSys != NULL )
	{
		const R32 lightMin = wSys->LightMin();
		const R32 lightMax = wSys->LightMax();
		if( lightMin < 300 && lightMax < 300 )
		{
			R32 i = wSys->CurrentLight();
			if( Races->VisLevel( mChar->GetRace() ) > i )
				toShow = 0;
			else
				toShow = static_cast<LIGHTLEVEL>(roundNumber( i - Races->VisLevel( mChar->GetRace() )));
			toSend.Level( toShow );
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
				toShow = static_cast<LIGHTLEVEL>(roundNumber( dunLevel - Races->VisLevel( mChar->GetRace() )));
			toSend.Level( toShow );
		}
	}
	s->Send( &toSend );

	cScript *onLightChangeScp = JSMapping->GetScript( mChar->GetScriptTrigger() );
	if( onLightChangeScp != NULL )
		onLightChangeScp->OnLightChange( mChar, toShow );
	else 
	{
		onLightChangeScp = JSMapping->GetScript( (UI16)0 );
		
		if( onLightChangeScp != NULL )
			onLightChangeScp->OnLightChange( mChar, toShow );
	}

	Weather->DoPlayerStuff( s, mChar );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void doLight( CSocket *s, UI08 level )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Send light level to players client
//o---------------------------------------------------------------------------o
void doLight( CChar *mChar, UI08 level )
{
	if( (Races->Affect( mChar->GetRace(), LIGHT )) && mChar->GetWeathDamage( LIGHT ) == 0 )
		mChar->SetWeathDamage( static_cast<UI32>(BuildTimeValue( static_cast<R32>(Races->Secs( mChar->GetRace(), LIGHT )) )), LIGHT );
	
	CTownRegion *curRegion	= mChar->GetRegion();
	CWeather *wSys			= Weather->Weather( curRegion->GetWeather() );
	
	LIGHTLEVEL toShow = level;

	LIGHTLEVEL dunLevel = cwmWorldState->ServerData()->DungeonLightLevel();
	
	// we have a valid weather system
	if( wSys != NULL )
	{
		const R32 lightMin = wSys->LightMin();
		const R32 lightMax = wSys->LightMax();
		if( lightMin < 300 && lightMax < 300 )
		{
			R32 i = wSys->CurrentLight();
			if( Races->VisLevel( mChar->GetRace() ) > i )
				toShow = 0;
			else
				toShow = static_cast<LIGHTLEVEL>(roundNumber( i - Races->VisLevel( mChar->GetRace() )));
		}
	}
	else
	{
		if( mChar->inDungeon() )
		{
			if( Races->VisLevel( mChar->GetRace() ) > dunLevel )
				toShow = 0;
			else
				toShow = static_cast<LIGHTLEVEL>(roundNumber( dunLevel - Races->VisLevel( mChar->GetRace() )));
		}
	}

	cScript *onLightChangeScp = JSMapping->GetScript( mChar->GetScriptTrigger() );
	if( onLightChangeScp != NULL )
		onLightChangeScp->OnLightChange( mChar, toShow );
	else 
	{
		onLightChangeScp = JSMapping->GetScript( (UI16)0 );
		
		if( onLightChangeScp != NULL )
			onLightChangeScp->OnLightChange( mChar, toShow );
	}
	Weather->DoNPCStuff( mChar );
}

//o---------------------------------------------------------------------------o
//|	Function	-	size_t getTileName( CItem& mItem, std::string& itemname )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Returns the length of an items name from tiledata.mul and
//|					sets itemname to the name.
//|					The format it accepts is same as UO style - %plural/single% or %plural%
//|						arrow%s%
//|						loa%ves/f% of bread
//o---------------------------------------------------------------------------o
size_t getTileName( CItem& mItem, std::string& itemname )
{
	CTile& tile = Map->SeekTile( mItem.GetID() );
	UString temp	= mItem.GetName() ;
	temp			= temp.simplifyWhiteSpace();
	if( temp.substr( 0, 1 ) == "#" )
	{
		temp =  static_cast< UString >( tile.Name() );
	}
	
	const UI16 getAmount = mItem.GetAmount();
	if( getAmount == 1 )
	{
		if( tile.CheckFlag( TF_DISPLAYAN ) )
			temp = "an " + temp;
		else if( tile.CheckFlag( TF_DISPLAYA ) )
			temp = "a " + temp;
	}

	// Find out if the name has a % in it
	if( temp.sectionCount( "%" ) > 0 )
	{
		UString single;
		const UString first	= temp.section( "%", 0, 0 );
		UString plural		= temp.section( "%", 1, 1 );
		const UString rest	= temp.section( "%", 2 );
		if( plural.sectionCount( "/" ) > 0 )
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
//|	Function	-	void checkRegion( CChar& mChar )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check what region a character is in, updating it if necesarry.
//o---------------------------------------------------------------------------o
void checkRegion( CSocket *mSock, CChar& mChar, bool forceUpdateLight)
{
	CTownRegion *iRegion	= mChar.GetRegion();
	CTownRegion *calcReg	= calcRegionFromXY( mChar.GetX(), mChar.GetY(), mChar.WorldNumber() );
	if( iRegion == NULL && calcReg != NULL )
		mChar.SetRegion( calcReg->GetRegionNum() );
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
				if( calcReg->IsGuarded() || iRegion->IsGuarded() )
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
					UpdateFlag( &mChar );
				}
				if( calcReg->GetAppearance() != iRegion->GetAppearance() )	 // if the regions look different
				{
					CPWorldChange wrldChange( calcReg->GetAppearance(), 1 );
					mSock->Send( &wrldChange );
				}
				if( calcReg == cwmWorldState->townRegions[mChar.GetTown()] )	// enter our home town
				{
					mSock->sysmessage( 1364 );
					CItem *packItem = mChar.GetPackItem();
					if( ValidateObject( packItem ) )
					{
						CDataList< CItem * > *piCont = packItem->GetContainsList();
						for( CItem *toScan = piCont->First(); !piCont->Finished(); toScan = piCont->Next() )
						{
							if( ValidateObject( toScan ) )
							{
								if( toScan->GetType() == IT_TOWNSTONE )
								{
									CTownRegion *targRegion = cwmWorldState->townRegions[static_cast<UI08>(toScan->GetTempVar( CITV_MOREX ))];
									mSock->sysmessage( 1365, targRegion->GetName().c_str() );
									targRegion->DoDamage( targRegion->GetHealth() );	// finish it off
									targRegion->Possess( calcReg );
									mChar.SetFame( (SI16)( mChar.GetFame() + mChar.GetFame() / 5 ) );	// 20% fame boost
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
			UI16 leaveScript = mChar.GetScriptTrigger();
			cScript *tScript = JSMapping->GetScript( leaveScript );
			if( tScript != NULL )
			{
				tScript->OnLeaveRegion( &mChar, iRegion->GetRegionNum() );
				tScript->OnEnterRegion( &mChar, calcReg->GetRegionNum() );
			}

			UI16 regLeaveScript	= iRegion->GetScriptTrigger();
			cScript *trScript	= JSMapping->GetScript( regLeaveScript );
			if( trScript != NULL )
				trScript->OnLeaveRegion( &mChar, iRegion->GetRegionNum() );

			UI16 regEnterScript	= calcReg->GetScriptTrigger();
			cScript *teScript	= JSMapping->GetScript( regEnterScript );
			if( teScript != NULL )
				teScript->OnEnterRegion( &mChar, calcReg->GetRegionNum() );
		}
		if( calcReg != NULL )
			mChar.SetRegion( calcReg->GetRegionNum() );
		if( mSock != NULL )
		{
			Effects->dosocketmidi( mSock );
			doLight( mSock, cwmWorldState->ServerData()->WorldLightCurrentLevel() );
		}
	} 
	else if( forceUpdateLight )
	{
		if( mSock != NULL )
		{
			doLight( mSock, cwmWorldState->ServerData()->WorldLightCurrentLevel() );
		}
	}
}

void CheckCharInsideBuilding( CChar *c, CSocket *mSock, bool doWeatherStuff )
{
	if( c->GetMounted() || c->GetStabled() )
		return;

	bool wasInBuilding = c->inBuilding();
	bool isInBuilding = Map->inBuilding( c->GetX(), c->GetY(), c->GetZ(), c->WorldNumber() );
	if( wasInBuilding != isInBuilding )
	{
		c->SetInBuilding( isInBuilding );
		if( doWeatherStuff )
			if( c->IsNpc() )
				Weather->DoNPCStuff( c );
			else
				Weather->DoPlayerStuff( mSock, c );
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool WillResultInCriminal( CChar *mChar, CChar *targ )
//|	Programmer	-	UOX DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check flagging, race, and guild info to find if character
//|					should be flagged criminal (returns true if so)
//o---------------------------------------------------------------------------o
bool WillResultInCriminal( CChar *mChar, CChar *targ )
{
	CChar *tOwner = targ->GetOwnerObj();
	if( !ValidateObject( mChar ) || !ValidateObject( targ ) || mChar == targ ) 
		return false;
	else if( !GuildSys->ResultInCriminal( mChar, targ ) || Races->Compare( mChar, targ ) != 0 ) 
		return false;
	else if( targ->DidAttackFirst() && targ->GetTarg() == mChar)
		return false;
	else if( ValidateObject( tOwner ) )
	{
		if( tOwner == mChar || tOwner == mChar->GetOwnerObj() )
			return false;
	}
	else if( targ->IsInnocent() )
		return true;
	return false;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void criminal( CChar *c )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Make character a criminal
//o---------------------------------------------------------------------------o
void criminal( CChar *c )
{
	c->SetTimer( tCHAR_CRIMFLAG, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_CRIMINAL ) );
	if( !c->IsCriminal() && !c->IsMurderer() )
	{
		CSocket *cSock = c->GetSocket();
		if( cSock != NULL )
			cSock->sysmessage( 1379 );
		UpdateFlag( c );
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void setcharflag( CChar *c )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Set characters flag
//o---------------------------------------------------------------------------o
void UpdateFlag( CChar *mChar )
{
	if( !ValidateObject( mChar ) )
		return;

	UI08 oldFlag = mChar->GetFlag();

	if( mChar->IsTamed() )
	{
		CChar *i = mChar->GetOwnerObj();
		if( ValidateObject( i ) )
			mChar->SetFlag( i->GetFlag() );
		else
		{
			mChar->SetFlagBlue();
			Console.Warning( "Tamed Creature has an invalid owner, Serial: 0x%X", mChar->GetSerial() );
		}
	}
	else
	{
		if( mChar->GetKills() > cwmWorldState->ServerData()->RepMaxKills() )
			mChar->SetFlagRed();
		else if( mChar->GetTimer( tCHAR_CRIMFLAG ) != 0 )
			mChar->SetFlagGray();
		else
		{
			if( mChar->IsNpc() )
			{
				bool doSwitch = true;
				if( cwmWorldState->creatures[mChar->GetID()].IsAnimal() && mChar->GetNPCAiType() != AI_EVIL )
				{
					if( cwmWorldState->ServerData()->CombatAnimalsGuarded() && mChar->GetRegion()->IsGuarded() )
					{
						mChar->SetFlagBlue();
						doSwitch = false;
					}
				}

				if( doSwitch )
				{
					switch( mChar->GetNPCFlag() )
					{
					case fNPC_NEUTRAL:
					default:
						mChar->SetFlagNeutral();
						break;
					case fNPC_INNOCENT:
						mChar->SetFlagBlue();
						break;
					case fNPC_EVIL:
						mChar->SetFlagRed();
						break;
					}
				}
			}
			else
				mChar->SetFlagBlue();	
		}
	}

	UI08 newFlag = mChar->GetFlag();
	if( oldFlag != newFlag )
	{
		UI16 targTrig = mChar->GetScriptTrigger();
		cScript *toExecute = JSMapping->GetScript( targTrig );
		if( toExecute != NULL )
			toExecute->OnFlagChange( mChar, newFlag, oldFlag );
		mChar->Dirty( UT_UPDATE );
	}
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
			toMove->SetLocation( (SI16)gate->GetTempVar( CITV_MOREX ), (SI16)gate->GetTempVar( CITV_MOREY ), (SI08)gate->GetTempVar( CITV_MOREZ ), tWorldNum );
			break;
	}
	SendMapChange( tWorldNum, sock );
}

//o--------------------------------------------------------------------------
//|	Function		-	DoorMacro( CSocket *s )
//|	Date			-	11th October, 1999
//|	Programmer		-	Zippy
//|	Modified		-	Abaddon (support CSocket *s and door blocking)
//o--------------------------------------------------------------------------
//|	Purpose			-	Door use macro support.
//o--------------------------------------------------------------------------
void DoorMacro( CSocket *s )
{
	CChar *mChar = s->CurrcharObj();
	SI16 xc = mChar->GetX(), yc = mChar->GetY();
	switch( mChar->GetDir() )
	{
		case 0 : --yc;				break;
		case 1 : { ++xc; --yc; }	break;
		case 2 : ++xc;				break;
		case 3 : { ++xc; ++yc; }	break;
		case 4 : ++yc;				break;
		case 5 : { --xc; ++yc; }	break;
		case 6 : --xc;				break;
		case 7 : { --xc; --yc; }	break;
	}

	REGIONLIST nearbyRegions = MapRegion->PopulateList( mChar );
	for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
	{
		CMapRegion *toCheck = (*rIter);
		if( toCheck == NULL )	// no valid region
			continue;
		CDataList< CItem * > *regItems = toCheck->GetItemList();
		regItems->Push();
		for( CItem *itemCheck = regItems->First(); !regItems->Finished(); itemCheck = regItems->Next() )
		{
			if( !ValidateObject( itemCheck ) )
				continue;
			SI16 distZ = abs( itemCheck->GetZ() - mChar->GetZ() );
			if( itemCheck->GetX() == xc && itemCheck->GetY() == yc && distZ < 7 )
			{
				if( itemCheck->GetType() == IT_DOOR || itemCheck->GetType() == IT_LOCKEDDOOR )	// only open doors
				{
					if( JSMapping->GetEnvokeByType()->Check( static_cast<UI16>(itemCheck->GetType()) ) )
					{
						UI16 envTrig = JSMapping->GetEnvokeByType()->GetScript( static_cast<UI16>(itemCheck->GetType()) );
						cScript *envExecute = JSMapping->GetScript( envTrig );
						if( envExecute != NULL )
							envExecute->OnUse( mChar, itemCheck );

						regItems->Pop();
						return;
					}
				}
			}
		}
		regItems->Pop();
	}
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
	UI32 tempSecs, tempMilli, tempTime;
	UI32 loopSecs, loopMilli;

	// EviLDeD: 042102: I moved this here where it basically should be for any windows application or dll that uses WindowsSockets.
#if UOX_PLATFORM == PLATFORM_WIN32
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD( 2, 0 );
	SI32 err = WSAStartup( wVersionRequested, &wsaData );
	if( err )
	{
		Console.Error( "Winsock 2.0 not found on your system..." );
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

		sprintf( temp, "%s v%s.%s", CVersionClass::GetProductName().c_str(), CVersionClass::GetVersion().c_str(), CVersionClass::GetBuild().c_str() );
		Console.Start( temp );

#if UOX_PLATFORM != PLATFORM_WIN32
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

		if(( cwmWorldState = new CWorldMain ) == NULL ) 
			Shutdown( FATAL_UOX3_ALLOC_WORLDSTATE );
		cwmWorldState->ServerData()->Load();

		Console << "Initializing and creating class pointers... " << myendl;
		InitClasses();
		cwmWorldState->SetUICurrentTime( currentTime );

		ParseArgs( argc, argv );
		Console.PrintSectionBegin();

		cwmWorldState->ServerData()->LoadTime();

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

		CJSMappingSection *packetSection = JSMapping->GetSection( SCPT_PACKET );
		for( cScript *ourScript = packetSection->First(); !packetSection->Finished(); ourScript = packetSection->Next() )
		{
			if( ourScript != NULL )
				ourScript->ScriptRegistration( "Packet" );
		}

		// moved all the map loading into CMulHandler - fur
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
		Weather->NewHour();
		Console.PrintDone();

		Console << "Loading Commands               " << myendl;
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

		Console << "Starting World Timers          ";
		cwmWorldState->SetTimer( tWORLD_LIGHTTIME, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_WEATHER ) );
		cwmWorldState->SetTimer( tWORLD_NEXTNPCAI, BuildTimeValue( (R32)cwmWorldState->ServerData()->CheckNpcAISpeed() ) );
		cwmWorldState->SetTimer( tWORLD_NEXTFIELDEFFECT, BuildTimeValue( 0.5f ) );
		cwmWorldState->SetTimer( tWORLD_SHOPRESTOCK, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_SHOPSPAWN ) );
		cwmWorldState->SetTimer( tWORLD_PETOFFLINECHECK, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_PETOFFLINECHECK ) );
		
		Console.PrintDone();

		DisplayBanner();

		Console << "Loading Accounts               ";
		Accounts->Load();

		Console.Log( "-=Server Startup=-\n=======================================================================", "server.log" );

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
#else
		TIMERVAL uiNextCheckConn = 0;
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
				Console.Poll();
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

