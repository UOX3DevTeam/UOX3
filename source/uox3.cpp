/* *************************************************************************

 Ultima Offline eXperiment III (UOX3)
 UO Server Emulation Program

 Copyright 1998 - 2022 by UOX3 contributors
 Copyright 1997, 98 by Marcus Rating (Cironian)
 ***************************************************************************

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

 *************************************************************************** */



#include <chrono>
#include <random>
#include <thread>

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
#include "StringUtility.hpp"
#include "EventTimer.hpp"

std::thread cons;
std::thread netw;

#if PLATFORM == WINDOWS
#include <process.h>
#include <conio.h>
#endif

std::chrono::time_point<std::chrono::system_clock> current;

std::mt19937 generator;

//o-----------------------------------------------------------------------------------------------o
// FileIO Pre-Declarations
//o-----------------------------------------------------------------------------------------------o
void		LoadCustomTitle( void );
void		LoadSkills( void );
void		LoadSpawnRegions( void );
void		LoadRegions( void );
void		LoadTeleportLocations( void );
void		LoadCreatures( void );
void		LoadPlaces( void );

//o-----------------------------------------------------------------------------------------------o
// Misc Pre-Declarations
//o-----------------------------------------------------------------------------------------------o
void		restockNPC( CChar& i, bool stockAll );
void		clearTrades( void );
void		sysBroadcast( const std::string& txt );
void		MoveBoat( UI08 dir, CBoatObj *boat );
bool		DecayItem( CItem& toDecay, const UI32 nextDecayItems, const UI32 nextDecayItemsInHouses );
void		CheckAI( CChar& mChar );


bool isWorldSaving = false;
#if PLATFORM == WINDOWS
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	BOOL WINAPI exit_handler(DWORD dwCtrlType)
//|					void app_stopped(int sig)
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Prevent closing of console via CTRL+C/or CTRL+BREAK keys during worldsaves
//o-----------------------------------------------------------------------------------------------o
BOOL WINAPI exit_handler( DWORD dwCtrlType )
{
	switch( dwCtrlType )
	{
		
		case CTRL_C_EVENT:
		case CTRL_BREAK_EVENT:
		case CTRL_CLOSE_EVENT:
			std::cout << std::endl << "World save in progress - closing UOX3 before it completes may result in corrupted save data!" << std::endl;

			// Shutdown of the application will only be halted for as long as the exit_handler is doing something,
			// so do some non-work while isWorldSaving is true to prevent shutdown during save
			while( isWorldSaving == true ) {
				Sleep(0);
			}

			return true;
		default:
			return false;
	}
}
#else
void app_stopped(int sig)
{
	// function called when signal is received.
	if( isWorldSaving == false )
	{
		cwmWorldState->SetKeepRun( false );
	}
}
#endif

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void UnloadSpawnRegions( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Unload spawn regions on server shutdown or when reloading spawn regions
//o-----------------------------------------------------------------------------------------------o
void UnloadSpawnRegions( void )
{
	for (auto [spwnnum,spawnReg]:cwmWorldState->spawnRegions){
		if( spawnReg != nullptr )
		{
			// Iterate over list of spawned characters and delete them if no player has tamed them/hired them
			auto spawnedCharsList = spawnReg->GetSpawnedCharsList();
			for( auto cCheck = spawnedCharsList->First(); !spawnedCharsList->Finished(); cCheck = spawnedCharsList->Next() )
			{
				if( !ValidateObject( cCheck ) )
					continue;

				if( !ValidateObject( cCheck->GetOwnerObj() ))
				{
					cCheck->Delete();
				}
			}

			// Iterate over list of spawned items and delete them if no player has picked them up
			auto spawnedItemsList = spawnReg->GetSpawnedItemsList();
			for( auto iCheck = spawnedItemsList->First(); !spawnedItemsList->Finished(); iCheck = spawnedItemsList->Next() )
			{
				if( !ValidateObject( iCheck ) )
					continue;

				if( iCheck->GetContSerial() != INVALIDSERIAL || !ValidateObject( iCheck->GetOwnerObj() ))
				{
					iCheck->Delete();
				}
			}

			delete spawnReg;
		}
		
	}
	cwmWorldState->spawnRegions.clear();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void UnloadRegions( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Unload town regions on server shutdown or when reloading town regions
//o-----------------------------------------------------------------------------------------------o
void UnloadRegions( void )
{
	for (auto [twnnum,town]:cwmWorldState->townRegions ){
		if (town != nullptr){
			delete town ;
		}
	}
	cwmWorldState->townRegions.clear();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void DoMessageLoop( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Watch for messages thrown by UOX
//o-----------------------------------------------------------------------------------------------o
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
						case '2':	UnloadRegions();
							LoadRegions();
							LoadTeleportLocations();					break;	// Reload regions/TeleportLocations
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
			default:				Console.error( "Unknown message type" );		break;
		}
	}
}
//------------------------------------------------------------
//---------------------------------------------------------------------------

// June 21, 1999
//	Ok here is thread number one its a simple thread for the checkkey() function
// June 12, 2004 (<-- LOL must be an omen)
//	xFTPd - Initial implementation of a very light weight FTP server for UOX3.
//	Expected functionality to provide file up/download access to shard
//	operators that may not have direct access to their servers.
//Nov 20, 2005
//	Removed xFTP Server code as UOX3 should be a game server, not an FTP server.
//
//					Commands supported:


bool conthreadcloseok	= false;
bool netpollthreadclose	= false;

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void NetworkPollConnectionThread( void *params )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Watch for new connections
//o-----------------------------------------------------------------------------------------------o
void NetworkPollConnectionThread(  )

{
	messageLoop << "Thread: NetworkPollConnection has started";
	netpollthreadclose = false;
	while( !netpollthreadclose )
	{
		Network->CheckConnections();
		Network->CheckLoginMessage();
		std::this_thread::sleep_for(std::chrono::milliseconds( 20 ));
	}
	messageLoop << "Thread: NetworkPollConnection has Closed";


}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CheckConsoleKeyThread( void *params )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Listen for key inputs in server console
//o-----------------------------------------------------------------------------------------------o
void CheckConsoleKeyThread(  )
{
	messageLoop << "Thread: CheckConsoleThread has started";
	Console.Registration();
	conthreadcloseok = false;
	while( !conthreadcloseok )
	{
		Console.Poll();
		std::this_thread::sleep_for(std::chrono::milliseconds( 500 ));
	}
	messageLoop << "Thread: CheckConsoleKeyThread Closed";
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool isOnline( CChar& mChar )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if the socket owning character c is still connected
//o-----------------------------------------------------------------------------------------------o
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
	{
		//std::scoped_lock lock(Network->internallock);
		Network->pushConn();
		for( CSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = 	Network->NextSocket() )
		{
			if( tSock->CurrcharObj() == &mChar )
			{
				Network->popConn();
				return true;
			}
		}
		Network->popConn();
	}
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void updateStats( CBaseObject *mObj, UI08 x )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Updates object's stats
//o-----------------------------------------------------------------------------------------------o
void updateStats( CBaseObject *mObj, UI08 x )
{
	auto nearbyChars = FindNearbyPlayers( mObj );
	for( auto &sock:nearbyChars ){
		
		if( sock->LoginComplete() ){
			
			// Normalize stats if we're updating our stats for other players
			bool normalizeStats = true;
			if( sock->CurrcharObj()->GetSerial() == mObj->GetSerial() )
			{
				sock->statwindow( mObj );
				normalizeStats = false;
			}
			
			// Prepare the stat update packet
			CPUpdateStat toSend( (*mObj), x, normalizeStats );
			
			// Send the stat update packet
			sock->Send( &toSend );
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CollectGarbage( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Deletes objects in the Deletion Queue
//o-----------------------------------------------------------------------------------------------o
void CollectGarbage( void )
{
	Console << "Performing Garbage Collection...";
	UI32 objectsDeleted				= 0;
	for (auto &entry : cwmWorldState->deletionQueue){
		CBaseObject *mObj = entry.first;
		if( mObj == nullptr || mObj->isFree() || !mObj->isDeleted() )
		{
			Console.warning( "Invalid object found in Deletion Queue" );
			continue;
		}
		ObjectFactory::getSingleton().DestroyObject( mObj );
		++objectsDeleted;
	}
	cwmWorldState->deletionQueue.clear();

	Console << " Removed " << objectsDeleted << " objects";

	JSEngine->CollectGarbage();
	Console.PrintDone();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void MountCreature( CSocket *sockPtr, CChar *s, CChar *x )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Mount a ridable creature
//|
//|	Changes		-	09/22/2002	-	Unhide players when mounting horses etc.
//o-----------------------------------------------------------------------------------------------o
void MountCreature( CSocket *sockPtr, CChar *s, CChar *x )
{
	if( s->IsOnHorse() )
		return;

	//No mounting horses for gargoyles!
	if( s->GetID() == 0x029A || s->GetID() == 0x029B )
	{
		sockPtr->sysmessage( 1798 );
		return;
	}

	if( !objInRange( s, x, DIST_NEXTTILE ) )
		return;
	if( x->GetOwnerObj() == s || Npcs->checkPetFriend( s, x ) || s->IsGM() )
	{
		if( !cwmWorldState->ServerData()->CharHideWhileMounted() )
			s->ExposeToView();

		s->SetOnHorse( true );
		CItem *c = Items->CreateItem( nullptr, s, 0x0915, 1, x->GetSkin(), OT_ITEM );

		std::string xName = getNpcDictName( x, sockPtr );
		c->SetName( xName );
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

		auto nearbyChars = FindNearbyPlayers( s );
		for( auto &sock:nearbyChars ){
			s->SendWornItems( sock );
		}

		if( x->GetTarg() != nullptr )	// zero out target, under all circumstances
		{
			x->SetTarg( nullptr );
			if( x->IsAtWar() )
				x->ToggleCombat();
		}
		if( ValidateObject( x->GetAttacker() ) )
			x->GetAttacker()->SetTarg( nullptr );
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void DismountCreature( CChar *s )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Dismount a ridable creature
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void endmessage( SI32 x )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Global message players with shutdown message
//o-----------------------------------------------------------------------------------------------o
void endmessage( SI32 x )
{
	x = 0;
	const UI32 igetclock = cwmWorldState->GetUICurrentTime();
	if( cwmWorldState->GetEndTime() < igetclock )
		cwmWorldState->SetEndTime( igetclock );


	sysBroadcast( oldstrutil::format( Dictionary->GetEntry( 1209 ), ((cwmWorldState->GetEndTime()-igetclock)/ 1000 ) / 60 ) );
}

#if PLATFORM != WINDOWS
void illinst( SI32 x = 0 ) //Thunderstorm linux fix
{
	sysBroadcast( "Fatal Server Error! Bailing out - Have a nice day!" );
	Console.error( "Illegal Instruction Signal caught - attempting shutdown" );
	endmessage( x );
}

void aus( SI32 signal )
{
	Console.error( "Server crash averted! Floating point exception caught." );
}

#endif

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void callguards( CChar *mChar )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Used when a character calls "Guards" Will look for a criminal
//|					first checking for anyone attacking him. If no one is attacking
//|					him it will look for any people nearby who are criminal or
//|					murderers
//o-----------------------------------------------------------------------------------------------o
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
	if( toCheck == nullptr )
		return;
	GenericList< CChar * > *regChars = toCheck->GetCharList();
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void callguards( CChar *mChar, CChar *targChar )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Used when a character calls guards on another character, will
//|					ensure that character is not dead and is either a criminal or
//|					murderer, and that he is in visual range of the victim, will
//|					then spawn a guard to take care of the criminal.
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool genericCheck( CSocket *mSock, CChar& mChar, bool checkFieldEffects, bool doWeather )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Check characters status.  Returns true if character was killed
//o-----------------------------------------------------------------------------------------------o
bool genericCheck( CSocket *mSock, CChar& mChar, bool checkFieldEffects, bool doWeather )
{
	UI16 c;
	if( !mChar.IsDead() )
	{
		const auto maxHP = mChar.GetMaxHP();
		const auto maxStam = mChar.GetMaxStam();
		const auto maxMana = mChar.GetMaxMana();

		if( mChar.GetHP() > maxHP )
			mChar.SetHP( maxHP );
		if( mChar.GetStamina() > maxStam )
			mChar.SetStamina( maxStam );
		if( mChar.GetMana() > maxMana )
			mChar.SetMana( maxMana );

		if( mChar.GetRegen( 0 ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
		{
			if( mChar.GetHP() < maxHP )
			{
				if( !cwmWorldState->ServerData()->HungerSystemEnabled() || mChar.GetHunger() > 0 
					|| ( !Races->DoesHunger( mChar.GetRace() ) && ( cwmWorldState->ServerData()->SystemTimer( tSERVER_HUNGERRATE ) == 0 || mChar.IsNpc() ) ) )
				{
					for( c = 0; c < maxHP + 1; ++c )
					{
						if( mChar.GetHP() <= maxHP && ( mChar.GetRegen( 0 ) + ( c * cwmWorldState->ServerData()->SystemTimer( tSERVER_HITPOINTREGEN ) * 1000 ) ) <= cwmWorldState->GetUICurrentTime() )
						{
							if( mChar.GetSkill( HEALING ) < 500 )
								mChar.IncHP( 1 );
							else if( mChar.GetSkill( HEALING ) < 800 )
								mChar.IncHP( 2 );
							else
								mChar.IncHP( 3 );
							if( mChar.GetHP() >= maxHP )
							{
								mChar.SetHP( maxHP );
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
			auto mStamina = mChar.GetStamina();
			if( mStamina < maxStam )
			{
				// Continue with stamina regen if  character is not yet fully parched, or if character is parched but has less than 25% stamina, or if char belongs to race that does not thirst
				if( !cwmWorldState->ServerData()->ThirstSystemEnabled() || mChar.GetThirst() > 0 || ( mChar.GetThirst() == 0 && mStamina < static_cast<SI16>( maxStam * 0.25 ) ) 
					|| ( !Races->DoesThirst( mChar.GetRace() ) && ( cwmWorldState->ServerData()->SystemTimer( tSERVER_THIRSTRATE ) == 0 || mChar.IsNpc() ) ) )
				{
					for( c = 0; c < maxStam + 1; ++c )
					{
						if( ( mChar.GetRegen( 1 ) + ( c * cwmWorldState->ServerData()->SystemTimer( tSERVER_STAMINAREGEN ) * 1000 ) ) <= cwmWorldState->GetUICurrentTime() && mChar.GetStamina() <= maxStam )
						{
							mChar.IncStamina( 1 );
							if( mChar.GetStamina() >= maxStam )
							{
								mChar.SetStamina( maxStam );
								break;
							}
						}
						else
							break;
					}
				}
			}
			mChar.SetRegen( cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_STAMINAREGEN ), 1 );
		}

		// CUSTOM START - SPUD:MANA REGENERATION:Rewrite of passive and active meditation code
		if( mChar.GetRegen( 2 ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
		{
			if( mChar.GetMana() < maxMana )
			{
				for( c = 0; c < maxMana + 1; ++c )
				{
					if( mChar.GetRegen( 2 ) + ( c * cwmWorldState->ServerData()->SystemTimer( tSERVER_MANAREGEN ) * 1000 ) <= cwmWorldState->GetUICurrentTime() && mChar.GetMana() <= maxMana )
					{
						Skills->CheckSkill( ( &mChar ), MEDITATION, 0, 1000 );	// Check Meditation for skill gain ala OSI
						mChar.IncMana( 1 );	// Gain a mana point
						if( mChar.GetMana() == maxMana )
						{
							if( mChar.IsMeditating() ) // Morrolan = Meditation
							{
								if( mSock != nullptr )
									mSock->sysmessage( 969 );
								mChar.SetMeditating( false );
							}
							break;
						}
					}
				}
			}
			const R32 MeditationBonus = ( .00075f * mChar.GetSkill( MEDITATION ) );	// Bonus for Meditation
			SI32 NextManaRegen = static_cast<SI32>( cwmWorldState->ServerData()->SystemTimer( tSERVER_MANAREGEN ) * ( 1 - MeditationBonus ) * 1000 );
			if( cwmWorldState->ServerData()->ArmorAffectManaRegen() )	// If armor effects mana regeneration...
			{
				R32 ArmorPenalty = Combat->calcDef( ( &mChar ), 0, false );	// Penalty taken due to high def
				if( ArmorPenalty > 100 )	// For def higher then 100, penalty is the same...just in case
					ArmorPenalty = 100;
				ArmorPenalty = 1 + ( ArmorPenalty / 25 );
				NextManaRegen = static_cast<SI32>( NextManaRegen * ArmorPenalty );
			}
			if( mChar.IsMeditating() )	// If player is meditation...
				mChar.SetRegen( ( cwmWorldState->GetUICurrentTime() + ( NextManaRegen / 2 ) ), 2 );
			else
				mChar.SetRegen( ( cwmWorldState->GetUICurrentTime() + NextManaRegen ), 2 );
		}
	}
	// CUSTOM END
	if( mChar.GetVisible() == VT_INVISIBLE && ( mChar.GetTimer( tCHAR_INVIS ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() ) )
		mChar.ExposeToView();

	// Take NPC out of EvadeState
	if( mChar.IsNpc() && mChar.IsEvading() && mChar.GetTimer( tNPC_EVADETIME ) <= cwmWorldState->GetUICurrentTime() )
	{
		mChar.SetEvadeState( false );
#if defined( UOX_DEBUG_MODE ) && defined( DEBUG_COMBAT )
		std::string mCharName = getNpcDictName( &mChar );
		Console.print( oldstrutil::format( "DEBUG: EvadeTimer ended for NPC (%s, 0x%X, at %i, %i, %i, %i).\n", mCharName.c_str(), mChar.GetSerial(), mChar.GetX(), mChar.GetY(), mChar.GetZ(), mChar.WorldNumber() ));
#endif
	}

	if( !mChar.IsDead() )
	{
		// Hunger/Thirst Code
		mChar.DoHunger( mSock );
		mChar.DoThirst( mSock );

		// Loyalty update for pets
		mChar.DoLoyaltyUpdate();

		if( !mChar.IsInvulnerable() && mChar.GetPoisoned() > 0 )
		{
			if( mChar.GetTimer( tCHAR_POISONTIME ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
			{
				if( mChar.GetTimer( tCHAR_POISONWEAROFF ) > cwmWorldState->GetUICurrentTime() )
				{
					std::string mCharName = getNpcDictName( &mChar );

					switch( mChar.GetPoisoned() )
					{
						case 1: // Lesser Poison
						{
							mChar.SetTimer( tCHAR_POISONTIME, BuildTimeValue( 2 ) );
							if( mChar.GetTimer( tCHAR_POISONTEXT ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
							{
								mChar.SetTimer( tCHAR_POISONTEXT, BuildTimeValue( 6 ) );
								mChar.TextMessage( nullptr, 1240, EMOTE, 1, mCharName.c_str() ); // * %s looks a bit nauseous *
							}
							SI16 poisonDmgPercent = RandomNum( 3, 6 ); // 3% to 6% of current health per tick
							SI16 poisonDmg = static_cast<SI16>((mChar.GetHP() * poisonDmgPercent) / 100);
							[[maybe_unused]] bool retVal = mChar.Damage( std::max(static_cast<SI16>(3), poisonDmg), POISON ); // Minimum 3 damage per tick
							break;
						}
						case 2: // Normal Poison
						{
							mChar.SetTimer( tCHAR_POISONTIME, BuildTimeValue( 3 ) );
							if( mChar.GetTimer( tCHAR_POISONTEXT ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
							{
								mChar.SetTimer( tCHAR_POISONTEXT, BuildTimeValue( 10 ) );
								mChar.TextMessage( nullptr, 1241, EMOTE, 1, mCharName.c_str() ); // * %s looks disoriented and nauseous! *
							}
							SI16 poisonDmgPercent = RandomNum( 4, 8 ); // 4% to 8% of current health per tick
							SI16 poisonDmg = static_cast<SI16>((mChar.GetHP() * poisonDmgPercent) / 100);
							[[maybe_unused]] bool retVal = mChar.Damage( std::max(static_cast<SI16>(5), poisonDmg), POISON ); // Minimum 5 damage per tick
							break;
						}
						case 3: // Greater Poison
						{
							mChar.SetTimer( tCHAR_POISONTIME, BuildTimeValue( 4 ) );
							if( mChar.GetTimer( tCHAR_POISONTEXT ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
							{
								mChar.SetTimer( tCHAR_POISONTEXT, BuildTimeValue( 10 ) );
								mChar.TextMessage( nullptr, 1242, EMOTE, 1, mCharName.c_str() ); // * %s is in severe pain! *
							}
							SI16 poisonDmgPercent = RandomNum( 8, 12 ); // 8% to 12% of current health per tick
							SI16 poisonDmg = static_cast<SI16>((mChar.GetHP() * poisonDmgPercent) / 100);
							[[maybe_unused]] bool retVal = mChar.Damage( std::max(static_cast<SI16>(8), poisonDmg), POISON ); // Minimum 8 damage per tick
							break;
						}
						case 4: // Deadly Poison
						{
							mChar.SetTimer( tCHAR_POISONTIME, BuildTimeValue( 5 ) );
							if( mChar.GetTimer( tCHAR_POISONTEXT ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
							{
								mChar.SetTimer( tCHAR_POISONTEXT, BuildTimeValue( 10 ) );
								mChar.TextMessage( nullptr, 1243, EMOTE, 1, mCharName.c_str() ); // * %s looks extremely weak and is wrecked in pain! *
							}
							SI16 poisonDmgPercent = RandomNum( 12, 25 ); // 12% to 25% of current health per tick
							SI16 poisonDmg = static_cast<SI16>((mChar.GetHP() * poisonDmgPercent) / 100);
							[[maybe_unused]] bool retVal = mChar.Damage( std::max(static_cast<SI16>(14), poisonDmg), POISON ); // Minimum 14 damage per tick
							break;
						}
						case 5: // Lethal Poison - Used by monsters only
						{
							mChar.SetTimer( tCHAR_POISONTIME, BuildTimeValue( 5 ) );
							if( mChar.GetTimer( tCHAR_POISONTEXT ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
							{
								mChar.SetTimer( tCHAR_POISONTEXT, BuildTimeValue( 10 ) );
								mChar.TextMessage( nullptr, 1243, EMOTE, 1, mCharName.c_str() ); // * %s looks extremely weak and is wrecked in pain! *
							}
							SI16 poisonDmgPercent = RandomNum( 25, 50 ); // 25% to 50% of current health per tick
							SI16 poisonDmg = static_cast<SI16>((mChar.GetHP() * poisonDmgPercent) / 100);
							[[maybe_unused]] bool retVal = mChar.Damage( std::max(static_cast<SI16>(17), poisonDmg), POISON ); // Minimum 14 damage per tick
							break;
						}
						default:
							Console.error( " Fallout of switch statement without default. uox3.cpp, genericCheck(), mChar.GetPoisoned() not within valid range." );
							mChar.SetPoisoned( 0 );
							break;
					}
					if( mChar.GetHP() < 1 && !mChar.IsDead() )
					{
						std::vector<UI16> scriptTriggers = mChar.GetScriptTriggers();
						for( auto i : scriptTriggers )
						{
							cScript *toExecute = JSMapping->GetScript( i );
							if( toExecute != nullptr )
							{
								SI08 retStatus = toExecute->OnDeathBlow( &mChar, mChar.GetAttacker() );

								// -1 == script doesn't exist, or returned -1
								// 0 == script returned false, 0, or nothing - don't execute hard code
								// 1 == script returned true or 1
								if( retStatus == 0 )
									return false;
							}
						}

						HandleDeath( ( &mChar ), nullptr );
						if( mSock != nullptr )
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
				if( mSock != nullptr )
					mSock->sysmessage( 1245 );
			}
		}
	}

	if( !mChar.GetCanAttack() && mChar.GetTimer( tCHAR_PEACETIMER ) <= cwmWorldState->GetUICurrentTime() )
	{
		mChar.SetCanAttack( true );
		if( mSock != nullptr )
			mSock->sysmessage( 1779 );
	}

	if( mChar.IsCriminal() && mChar.GetTimer( tCHAR_CRIMFLAG ) && ( mChar.GetTimer( tCHAR_CRIMFLAG ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() ) )
	{
		if( mSock != nullptr )
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
		if( mSock != nullptr && mChar.GetKills() == cwmWorldState->ServerData()->RepMaxKills() )
			mSock->sysmessage( 1239 );
		UpdateFlag( &mChar );
	}

	if( !mChar.IsDead() )
	{
		if( doWeather )
		{
			const UI08 curLevel = cwmWorldState->ServerData()->WorldLightCurrentLevel();
			LIGHTLEVEL toShow;
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
	}

	if( mChar.IsDead() )
		return true;
	else if( mChar.GetHP() <= 0 )
	{
		std::vector<UI16> scriptTriggers = mChar.GetScriptTriggers();
		for( auto i : scriptTriggers )
		{
			cScript *toExecute = JSMapping->GetScript( i );
			if( toExecute != nullptr )
			{
				SI08 retStatus = toExecute->OnDeathBlow( &mChar, mChar.GetAttacker() );

				// -1 == script doesn't exist, or returned -1
				// 0 == script returned false, 0, or nothing - don't execute hard code
				// 1 == script returned true or 1
				if( retStatus == 0 )
					return false;
			}
		}

		HandleDeath( (&mChar), nullptr );
		return true;
	}
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void checkPC( CSocket *mSock, CChar& mChar )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Check a PC's status
//o-----------------------------------------------------------------------------------------------o
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

	if( mChar.IsCasting() && !mChar.IsJSCasting() && mChar.GetSpellCast() != -1 )	// Casting a spell
	{
		auto spellNum = mChar.GetSpellCast();
		mChar.SetNextAct( mChar.GetNextAct() - 1 );
		if( mChar.GetTimer( tCHAR_SPELLTIME ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )//Spell is complete target it.
		{
			// Set the recovery time before another spell can be cast
			mChar.SetTimer( tCHAR_SPELLRECOVERYTIME, BuildTimeValue( static_cast<R32>( Magic->spells[spellNum].RecoveryDelay()) ));

			if( Magic->spells[spellNum].RequireTarget() )
			{
				mChar.SetCasting( false );
				mChar.SetFrozen( false );
				UI08 cursorType = 0;
				if( Magic->spells[spellNum].AggressiveSpell() )
					cursorType = 1;
				else if( spellNum == 4 || spellNum == 6 || spellNum == 7 || spellNum == 9 || spellNum == 10 || spellNum == 11 || spellNum == 15 || spellNum == 16 || spellNum == 17
					|| spellNum == 25 || spellNum == 26 || spellNum == 29 || spellNum == 44 || spellNum == 59 )
					cursorType = 2;
				mSock->target( 0, TARGET_CASTSPELL, Magic->spells[spellNum].StringToSay().c_str(), cursorType );
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
			if( !mChar.IsOnHorse() && !mChar.IsFlying() ) // Consider Gargoyle flying as mounted here
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
				if( mSock->ClientType() >= CV_HS2D )
					tSend.AddSerial( mChar.GetTrackingTarget()->GetSerial() );
				mSock->Send( &tSend );
			}
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

	if( mChar.GetTimer( tCHAR_FLYINGTOGGLE ) > 0 && mChar.GetTimer( tCHAR_FLYINGTOGGLE ) < cwmWorldState->GetUICurrentTime() )
	{
		mChar.SetTimer( tCHAR_FLYINGTOGGLE, 0 );
		mChar.SetFrozen( false );
		mChar.Teleport();
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void checkNPC( CChar& mChar, bool checkAI, bool doRestock, bool doPetOfflineCheck )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Check NPC's status
//o-----------------------------------------------------------------------------------------------o
void checkNPC( CChar& mChar, bool checkAI, bool doRestock, bool doPetOfflineCheck )
{
	// okay, this will only ever trigger after we check an npc...  Question is:
	// should we remove the time delay on the AI check as well?  Just stick with AI/movement
	// AI can never be faster than how often we check npcs

	bool doAICheck = true;
	std::vector<UI16> scriptTriggers = mChar.GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			if( toExecute->OnAISliver( &mChar ) == 1 )
			{
				// Script returned true or 1, don't do hard-coded AI check
				doAICheck = false;
			}
		}
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
			// Added Dec 20, 1999
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

	if( mChar.GetNpcWander() != WT_FLEE && mChar.GetNpcWander() != WT_FROZEN && ( mChar.GetHP() < mChar.GetMaxHP() * mChar.GetFleeAt() / 100 ) )
	{
		CChar *mTarget = mChar.GetTarg();
		if( ValidateObject( mTarget ) && !mTarget->IsDead() && objInRange( &mChar, mTarget, DIST_SAMESCREEN ))
		{
			mChar.SetOldNpcWander( mChar.GetNpcWander() );
			mChar.SetNpcWander( WT_FLEE );
			if( mChar.GetMounted() )
				mChar.SetTimer( tNPC_MOVETIME, BuildTimeValue( mChar.GetMountedFleeingSpeed() ) );
			else
				mChar.SetTimer( tNPC_MOVETIME, BuildTimeValue( mChar.GetFleeingSpeed() ) );
		}
	}
	else if( mChar.GetNpcWander() == WT_FLEE && (mChar.GetHP() > mChar.GetMaxHP() * mChar.GetReattackAt() / 100))
	{
		mChar.SetNpcWander( mChar.GetOldNpcWander() );
		if( mChar.GetMounted() )
			mChar.SetTimer( tNPC_MOVETIME, BuildTimeValue( mChar.GetMountedWalkingSpeed() ) );
		else
			mChar.SetTimer( tNPC_MOVETIME, BuildTimeValue( mChar.GetWalkingSpeed() ) );
		mChar.SetOldNpcWander( WT_NONE ); // so it won't save this at the wsc file
	}
	Combat->CombatLoop( nullptr, mChar );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void checkItem( CMapRegion *toCheck, bool checkItems, UI32 nextDecayItems, UI32 nextDecayItemsInHouses )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Check item decay, spawn timers and boat movement in a given region
//o-----------------------------------------------------------------------------------------------o
void checkItem( CMapRegion *toCheck, bool checkItems, UI32 nextDecayItems, UI32 nextDecayItemsInHouses, bool doWeather )
{
	GenericList< CItem * > *regItems = toCheck->GetItemList();
	regItems->Push();
	for( CItem *itemCheck = regItems->First(); !regItems->Finished(); itemCheck = regItems->Next() )
	{
		if( !ValidateObject( itemCheck ) || itemCheck->isFree() )
			continue;
		if( checkItems )
		{
			if( itemCheck->isDecayable() && itemCheck->GetCont() == nullptr )
			{
				if( itemCheck->GetType() == IT_HOUSESIGN && itemCheck->GetTempVar( CITV_MORE ) > 0 )
				{
					// Don't decay signs that belong to houses
					itemCheck->SetDecayable( false );
				}
				if( itemCheck->GetDecayTime() <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
				{
					std::vector<UI16> scriptTriggers = itemCheck->GetScriptTriggers();
					for( auto scriptTrig : scriptTriggers )
					{
						cScript *toExecute = JSMapping->GetScript( scriptTrig );
						if( toExecute != nullptr )
						{
							if( toExecute->OnDecay( itemCheck ) == 0 )	// if it exists and we don't want hard code, return
							{
								return;
							}
						}
					}

					// Check global script! Maybe there's another event there
					cScript *toExecute = JSMapping->GetScript( static_cast<UI16>(0) );
					if( toExecute != nullptr )
					{
						if( toExecute->OnDecay( itemCheck ) == 0 )	// if it exists and we don't want hard code, return
						{
							return;
						}
					}

					if( DecayItem( (*itemCheck), nextDecayItems, nextDecayItemsInHouses ) )
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
						{
							itemCheck->SetType( IT_NOTYPE );
							Console.warning( "Invalid spawner object detected; item type reverted to 0. All spawner objects have to be added using 'ADD SPAWNER # command." );
						}
					}
					break;
				case IT_SOUNDOBJECT:
					if( itemCheck->GetTempVar( CITV_MOREY ) < 25 )
					{
						if( RandomNum( 1, 100 ) <= (SI32)itemCheck->GetTempVar( CITV_MOREZ ) )
						{
							auto nearbyChars = FindNearbyPlayers( itemCheck, static_cast<UI16>(itemCheck->GetTempVar( CITV_MOREY )) );
							for( auto &sock:nearbyChars ){
							
								Effects->PlaySound( sock, static_cast<UI16>(itemCheck->GetTempVar( CITV_MOREX )), false );
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
			SI08 boatMoveType = mBoat->GetMoveType();
			if( ValidateObject( mBoat ) && boatMoveType &&
			   ( mBoat->GetMoveTime() <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() ) )
			{
				if( boatMoveType != BOAT_ANCHORED )
				{
					switch( boatMoveType )
					{
						//case BOAT_ANCHORED:
						//case BOAT_STOP:
						case BOAT_FORWARD:
						case BOAT_SLOWFORWARD:
						case BOAT_ONEFORWARD:
							MoveBoat( itemCheck->GetDir(), mBoat );
							break;
						case BOAT_BACKWARD:
						case BOAT_SLOWBACKWARD:
						case BOAT_ONEBACKWARD:
						{
							UI08 dir = static_cast<UI08>( itemCheck->GetDir() + 4 );
							if( dir > 7 )
								dir %= 8;
							MoveBoat( dir, mBoat );
							break;
						}
						case BOAT_LEFT:
						case BOAT_SLOWLEFT:
						case BOAT_ONELEFT:
						{
							UI08 dir = static_cast<UI08>( itemCheck->GetDir() - 2 );

							dir %= 8;
							MoveBoat( dir, mBoat );
							break;
						}
						case BOAT_RIGHT:
						case BOAT_SLOWRIGHT:
						case BOAT_ONERIGHT:
						{
							// Right / One Right
							UI08 dir = static_cast<UI08>( itemCheck->GetDir() + 2 );

							dir %= 8;
							MoveBoat( dir, mBoat );
							break;
						}
						case BOAT_FORWARDLEFT:
						case BOAT_SLOWFORWARDLEFT:
						case BOAT_ONEFORWARDLEFT:
						{
							UI08 dir = static_cast<UI08>( itemCheck->GetDir() - 1 );

							dir %= 8;
							MoveBoat( dir, mBoat );
							break;
						}
						case BOAT_FORWARDRIGHT:
						case BOAT_SLOWFORWARDRIGHT:
						case BOAT_ONEFORWARDRIGHT:
						{
							UI08 dir = static_cast<UI08>( itemCheck->GetDir() + 1 );

							dir %= 8;
							MoveBoat( dir, mBoat );
							break;
						}
						case BOAT_BACKWARDLEFT:
						case BOAT_SLOWBACKWARDLEFT:
						case BOAT_ONEBACKWARDLEFT:
						{
							UI08 dir = static_cast<UI08>( itemCheck->GetDir() + 5 );
							if( dir > 7 )
								dir %= 8;
							MoveBoat( dir, mBoat );
							break;
						}
						case BOAT_BACKWARDRIGHT:
						case BOAT_SLOWBACKWARDRIGHT:
						case BOAT_ONEBACKWARDRIGHT:
						{
							UI08 dir = static_cast<UI08>( itemCheck->GetDir() + 3 );
							if( dir > 7 )
								dir %= 8;
							MoveBoat( dir, mBoat );
							break;
						}
						default:
							break;
					}

					// One-step boat commands, so reset move type to 0 after the initial move
					if( boatMoveType == BOAT_LEFT || boatMoveType == BOAT_RIGHT )
					{
						// Move 50% slower left/right than forward/back
						mBoat->SetMoveTime( BuildTimeValue( (R32)cwmWorldState->ServerData()->CheckBoatSpeed() * 1.5 ) );
					}
					else if( boatMoveType >= BOAT_ONELEFT && boatMoveType <= BOAT_ONEBACKWARDRIGHT )
					{
						mBoat->SetMoveType( 0 );

						// Set timer to restrict movement to normal boat speed if player spams command
						mBoat->SetMoveTime( BuildTimeValue( (R32)cwmWorldState->ServerData()->CheckBoatSpeed() * 1.5 ) );
					}
					else if( boatMoveType >= BOAT_SLOWLEFT && boatMoveType <= BOAT_SLOWBACKWARDLEFT )
					{
						// Set timer to slowly move the boat forward
						mBoat->SetMoveTime( BuildTimeValue( (R32)cwmWorldState->ServerData()->CheckBoatSpeed() * 2.0 ) );
					}
					else
					{
						// Set timer to move the boat forward at normal speed
						mBoat->SetMoveTime( BuildTimeValue( (R32)cwmWorldState->ServerData()->CheckBoatSpeed() ) );
					}
				}
			}
		}

		// Do JS Weather for item
		if( doWeather )
		{
			doLight( itemCheck, cwmWorldState->ServerData()->WorldLightCurrentLevel() );
		}
	}
	regItems->Pop();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CheckAutoTimers( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Check automatic and timer controlled functions
//o-----------------------------------------------------------------------------------------------o
void CWorldMain::CheckAutoTimers( void )
{
	static UI32 nextCheckSpawnRegions	= 0;
	static UI32 nextCheckTownRegions	= 0;
	static UI32 nextCheckItems			= 0;
	static UI32 nextDecayItems			= 0;
	static UI32 nextDecayItemsInHouses	= 0;
	static UI32 nextSetNPCFlagTime		= 0;
	static UI32 accountFlush			= 0;
	bool doWeather						= false;
	bool doPetOfflineCheck				= false;
	CServerData *serverData				= ServerData();

	// modify this stuff to take into account more variables
	if( accountFlush <= GetUICurrentTime() || GetOverflow() )
	{
		bool reallyOn = false;
		// time to flush our account status!
		MAPUSERNAMEID_ITERATOR I;
		for( I = Accounts->begin(); I != Accounts->end(); ++I )
		{
			CAccountBlock& actbTemp = I->second;
			if( actbTemp.wAccountIndex == AB_INVALID_ID)
				continue;

			if( actbTemp.wFlags.test( AB_FLAGS_ONLINE ) )
			{
				reallyOn = false;	// to start with, there's no one really on
				{
					//std::scoped_lock lock(Network->internallock);
					Network->pushConn();
					for( CSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
					{
						CChar *tChar = tSock->CurrcharObj();
						if( !ValidateObject( tChar ) )
							continue;
						if( tChar->GetAccount().wAccountIndex == actbTemp.wAccountIndex )
							reallyOn = true;
					}
					Network->popConn();
				}
				if( !reallyOn )	// no one's really on, let's set that
				{
					actbTemp.wFlags.reset( AB_FLAGS_ONLINE );
				}
			}
		}
		accountFlush = BuildTimeValue( (R32)serverData->AccountFlushTimer() );
	}
	//Network->On();   //<<<<<< WHAT the HECK, this is why you dont bury mutex locking
					// PushConn and PopConn lock and unlock as well (yes, bad)
					// But now we are doing recursive lock here

	if( GetWorldSaveProgress() == SS_NOTSAVING )
	{
		{
			//std::scoped_lock lock(Network->internallock);
			Network->pushConn();
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

				if( serverData->KickOnAssistantSilence() )
				{
					if( !tSock->NegotiatedWithAssistant() && tSock->NegotiateTimeout() != -1 && static_cast<UI32>(tSock->NegotiateTimeout()) <= GetUICurrentTime() )
					{
						const CChar *tChar = tSock->CurrcharObj();
						if( !ValidateObject( tChar ) )
							continue;
						if( !tChar->IsGM() )
						{
							tSock->IdleTimeout( -1 );
							tSock->sysmessage( 9047 ); // Failed to negotiate features with assistant tool. Disconnecting client...
							Network->Disconnect( tSock );
						}
					}
				}

				// Check player's network traffic usage versus caps set in ini
				if( tSock->LoginComplete() && tSock->AcctNo() != 0 && tSock->GetTimer( tPC_TRAFFICWARDEN ) <= GetUICurrentTime() )
				{
					if( !ValidateObject( tSock->CurrcharObj() ) || tSock->CurrcharObj()->IsGM() )
						continue;

					bool tempTimeBan = false;
					if( tSock->BytesReceived() > serverData->MaxClientBytesIn() )
					{
						// Player has exceeded the cap! Send one warning - next time kick player
						tSock->sysmessage( Dictionary->GetEntry( 9082, tSock->Language() )); // Excessive data usage detected! Sending too many requests to the server in a short amount of time will get you banned.
						tSock->BytesReceivedWarning( tSock->BytesReceivedWarning() + 1 );
						if( tSock->BytesReceivedWarning() > 2 )
						{
							// If it happens 3 times in the same session, give player a temporary ban
							tempTimeBan = true;
						}
					}

					if( tSock->BytesSent() > serverData->MaxClientBytesOut() )
					{
						// This is data sent from server, so should be more lenient before a kick (though could still be initiated by player somehow)
						tSock->sysmessage( Dictionary->GetEntry( 9082, tSock->Language() )); // Excessive data usage detected! Sending too many requests to the server in a short amount of time will get you banned.
						tSock->BytesSentWarning( tSock->BytesSentWarning() + 1 );
						if( tSock->BytesSentWarning() > 2 )
						{
							// If it happens 3 times or more in the same session, give player a temporary ban
							tempTimeBan = true;
						}
					}

					if( tempTimeBan )
					{
						// Give player a 30 minute temp ban
						CAccountBlock& myAccount = Accounts->GetAccountByID( tSock->GetAccount().wAccountIndex );
						myAccount.wFlags.set( AB_FLAGS_BANNED, true );
						myAccount.wTimeBan = GetMinutesSinceEpoch() + serverData->NetTrafficTimeban();
						Network->Disconnect( tSock );
						continue;
					}

					// Reset amount of bytes received and sent, and restart timer
					tSock->BytesReceived( 0 );
					tSock->BytesSent( 0 );
					tSock->SetTimer( tPC_TRAFFICWARDEN, BuildTimeValue( static_cast<R32>( 10 )) );
				}
			}
			Network->popConn();
		}
	}
	else if( GetWorldSaveProgress() == SS_JUSTSAVED )	// if we've JUST saved, do NOT kick anyone off (due to a possibly really long save), but reset any offending players to 60 seconds to go before being kicked off
	{
		{
			//std::scoped_lock lock(Network->internallock);
			Network->pushConn();
			for( CSocket *wsSocket = Network->FirstSocket(); !Network->FinishedSockets(); wsSocket = Network->NextSocket() )
			{
				if( wsSocket != nullptr )
				{
					if( (UI32)wsSocket->IdleTimeout() < GetUICurrentTime() )
					{
						wsSocket->IdleTimeout( BuildTimeValue( 60.0F ) );
						wsSocket->WasIdleWarned( true );//don't give them the message if they only have 60s
					}
					if( cwmWorldState->ServerData()->KickOnAssistantSilence() )
					{
						if( !wsSocket->NegotiatedWithAssistant() && static_cast<UI32>(wsSocket->NegotiateTimeout()) < GetUICurrentTime() )
						{
							wsSocket->NegotiateTimeout( BuildTimeValue( 60.0F ) );
						}
					}
				}
			}
			Network->popConn();
		}
		SetWorldSaveProgress( SS_NOTSAVING );
	}
	//Network->Off();
	if( nextCheckTownRegions <= GetUICurrentTime() || GetOverflow() )
	{
		for (auto [twnnum,myReg]:cwmWorldState->townRegions){
			if( myReg != nullptr )
				myReg->PeriodicCheck();
		}
		nextCheckTownRegions = BuildTimeValue( 10 );	// do checks every 10 seconds or so, rather than every single time
		JailSys->PeriodicCheck();
	}

	if( nextCheckSpawnRegions <= GetUICurrentTime() && serverData->CheckSpawnRegionSpeed() != -1 )//Regionspawns
	{
		UI32 itemsSpawned		= 0;
		UI32 npcsSpawned		= 0;
		UI32 totalItemsSpawned	= 0;
		UI32 totalNpcsSpawned	= 0;
		UI32 maxItemsSpawned	= 0;
		UI32 maxNpcsSpawned		= 0;

		for (auto [spwnnum,spawnReg]:cwmWorldState->spawnRegions){
		
			if( spawnReg != nullptr )
			{
				if( spawnReg->GetNextTime() <= GetUICurrentTime() )
					spawnReg->doRegionSpawn( itemsSpawned, npcsSpawned );

				// Grab some info from the spawn region anyway, even if it's not time to spawn
				totalItemsSpawned += static_cast<UI32>(spawnReg->GetCurrentItemAmt());
				totalNpcsSpawned += static_cast<UI32>(spawnReg->GetCurrentCharAmt());
				maxItemsSpawned += static_cast<UI32>(spawnReg->GetMaxItemSpawn());
				maxNpcsSpawned += static_cast<UI32>(spawnReg->GetMaxCharSpawn());
			}
		}

		// Adaptive spawn region check timer. The closer spawn regions as a whole are to being at their defined max capacity,
		// the less frequently UOX3 will check spawn regions again. Similarly, the more room there is to spawn additional
		// stuff, the more frequently UOX3 will check spawn regions
		auto checkSpawnRegionSpeed = static_cast<R32>(serverData->CheckSpawnRegionSpeed());
		UI16 itemSpawnCompletionRatio = ( maxItemsSpawned > 0 ? (( totalItemsSpawned * 100.0 ) / maxItemsSpawned ) : 100 );
		UI16 npcSpawnCompletionRatio = ( maxNpcsSpawned > 0 ? (( totalNpcsSpawned * 100.0 ) / maxNpcsSpawned ) : 100 );
		
		SI32 avgCompletionRatio = ( itemSpawnCompletionRatio + npcSpawnCompletionRatio ) / 2;
		if( avgCompletionRatio == 100 )
			checkSpawnRegionSpeed *= 3;
		else if( avgCompletionRatio >= 90 )
			checkSpawnRegionSpeed *= 2;
		else if( avgCompletionRatio >= 75 )
			checkSpawnRegionSpeed *= 1.5;
		else if( avgCompletionRatio >= 50 )
			checkSpawnRegionSpeed *= 1.25;

		nextCheckSpawnRegions = BuildTimeValue( checkSpawnRegionSpeed );//Don't check them TOO often (Keep down the lag)
	}

	HTMLTemplates->Poll( ETT_ALLTEMPLATES );

	const UI32 saveinterval = serverData->ServerSavesTimerStatus();
	if( saveinterval != 0 )
	{
		time_t oldTime = GetOldTime();
		if( !GetAutoSaved() )
		{
			SetAutoSaved( true );
			time(&oldTime);
			SetOldTime( static_cast<UI32>(oldTime) );
		}
		time_t newTime = GetNewTime();
		time(&newTime);
		SetNewTime( static_cast<UI32>(newTime) );

		if( difftime( GetNewTime(), GetOldTime() ) >= saveinterval )
		{
			// Added Dec 20, 1999
			// After an automatic world save occurs, lets check to see if
			// anyone is online (clients connected).  If nobody is connected
			// Lets do some maintenance on the bulletin boards.
			if( !GetPlayersOnline() && GetWorldSaveProgress() != SS_SAVING )
			{
				Console << "No players currently online. Starting bulletin board maintenance" << myendl;
				Console.log( "Bulletin Board Maintenance routine running (AUTO)", "server.log" );
				MsgBoardMaintenance();
			}

			SetAutoSaved( false );

#if PLATFORM == WINDOWS
			SetConsoleCtrlHandler( exit_handler, TRUE );
#endif
			isWorldSaving = true;
			SaveNewWorld( false );
			isWorldSaving = false;
#if PLATFORM == WINDOWS
			SetConsoleCtrlHandler( exit_handler, false );
#endif
		}
	}

	/*time_t oldIPTime = GetOldIPTime();
	if( !GetIPUpdated() )
	{
		SetIPUpdated( true );
		time(&oldIPTime);
		SetOldIPTime( static_cast<UI32>(oldIPTime) );
	}
	time_t newIPTime = GetNewIPTime();
	time(&newIPTime);
	SetNewIPTime( static_cast<UI32>(newIPTime) );

	if( difftime( GetNewIPTime(), GetOldIPTime() ) >= 120 )
	{
		ServerData()->RefreshIPs();
		SetIPUpdated( false );
	}*/

	//Time functions
	if( GetUOTickCount() <= GetUICurrentTime() || ( GetOverflow() ) )
	{
		UI08 oldHour = serverData->ServerTimeHours();
		if( serverData->incMinute() )
			Weather->NewDay();
		if( oldHour != serverData->ServerTimeHours() )
			Weather->NewHour();

		SetUOTickCount( BuildTimeValue( serverData->ServerSecondsPerUOMinute() ) );
	}

	if( GetTimer( tWORLD_LIGHTTIME ) <= GetUICurrentTime() || GetOverflow() )
	{
		doWorldLight();  //Changes lighting, if it is currently time to.
		Weather->DoStuff();	// updates the weather types
		SetTimer( tWORLD_LIGHTTIME, serverData->BuildSystemTimeValue( tSERVER_WEATHER ) );
		doWeather = true;
	}

	if( GetTimer( tWORLD_PETOFFLINECHECK ) <= GetUICurrentTime() || GetOverflow() )
	{
		SetTimer( tWORLD_PETOFFLINECHECK, serverData->BuildSystemTimeValue( tSERVER_PETOFFLINECHECK ) );
		doPetOfflineCheck = true;
	}

	bool checkFieldEffects = false;
	if( GetTimer( tWORLD_NEXTFIELDEFFECT ) <= GetUICurrentTime() || GetOverflow() )
	{
		checkFieldEffects = true;
		SetTimer( tWORLD_NEXTFIELDEFFECT, BuildTimeValue( 0.5f ) );
	}
	std::set< CMapRegion * > regionList;
	{
		//std::scoped_lock lock(Network->internallock);

		Network->pushConn();
		for( CSocket *iSock = Network->FirstSocket(); !Network->FinishedSockets(); iSock = Network->NextSocket() )
		{
			if( iSock == nullptr )
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

				// Restrict the amount of active regions based on how far player is from the border
				// to the next one. This reduces active regions around a player from always 9 to
				// varying between 3 to 6. Only regions on yOffset are considered, because the xOffset
				// ones are too narrow
				auto yOffsetUnrounded = static_cast<R32>(mChar->GetY()) / static_cast<R32>(MapRowSize);
				SI08 counter2Start = 0, counter2End = 0;
				if( yOffsetUnrounded < yOffset + 0.25 )
				{
					counter2Start = -1;
					counter2End = 0;
				}
				else if( yOffsetUnrounded > yOffset + 0.75 )
				{
					counter2Start = 0;
					counter2End = 1;
				}

				for( SI08 counter = -1; counter <= 1; ++counter ) // Check 3 x colums
				{
					for( SI08 ctr2 = counter2Start; ctr2 <= counter2End; ++ctr2 ) // Check variable y colums
					{
						CMapRegion *tC = MapRegion->GetMapRegion( xOffset + counter, yOffset + ctr2, worldNumber );
						if( tC == nullptr )
							continue;
						regionList.insert( tC );
					}
				}
			}
		}
		Network->popConn();
	}

	// Reduce some lag checking these timers constantly in the loop
	bool setNPCFlags = false, checkItems = false, checkAI = false, doRestock = false;
	if( nextSetNPCFlagTime <= GetUICurrentTime() || GetOverflow() )
	{
		nextSetNPCFlagTime = serverData->BuildSystemTimeValue( tSERVER_NPCFLAGUPDATETIMER );	// Slow down lag "needed" for setting flags, they are set often enough ;-)
		setNPCFlags = true;
	}
	if( nextCheckItems <= GetUICurrentTime() || GetOverflow() )
	{
		nextCheckItems = BuildTimeValue( static_cast<R32>(serverData->CheckItemsSpeed()) );
		nextDecayItems = serverData->BuildSystemTimeValue( tSERVER_DECAY );
		nextDecayItemsInHouses = serverData->BuildSystemTimeValue( tSERVER_DECAYINHOUSE );
		checkItems = true;
	}
	if( GetTimer( tWORLD_NEXTNPCAI ) <= GetUICurrentTime() || GetOverflow() )
	{
		SetTimer( tWORLD_NEXTNPCAI, BuildTimeValue( (R32)serverData->CheckNpcAISpeed() ) );
		checkAI = true;
	}
	if( GetTimer( tWORLD_SHOPRESTOCK ) <= GetUICurrentTime() || GetOverflow() )
	{
		SetTimer( tWORLD_SHOPRESTOCK, serverData->BuildSystemTimeValue( tSERVER_SHOPSPAWN ) );
		doRestock = true;
	}

	bool allowAwakeNPCs = cwmWorldState->ServerData()->AllowAwakeNPCs();
	std::set< CMapRegion * >::const_iterator tcCheck = regionList.begin();
	while( tcCheck != regionList.end() )
	{
		CMapRegion *toCheck = (*tcCheck);
		GenericList< CChar * > *regChars = toCheck->GetCharList();
		regChars->Push();
		for( CChar *charCheck = regChars->First(); !regChars->Finished(); charCheck = regChars->Next() )
		{
			if( !ValidateObject( charCheck ) || charCheck->isFree() )
				continue;
			if( charCheck->IsNpc() )
			{
				if( charCheck->IsAwake() && allowAwakeNPCs )
					continue;

				// Only perform these checks on NPCs that are not permanently awake
				if( !genericCheck( nullptr, (*charCheck), checkFieldEffects, doWeather ) )
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
						charCheck->Teleport();
					}
				}
			}
		}
		regChars->Pop();

		checkItem( toCheck, checkItems, nextDecayItems, nextDecayItemsInHouses, doWeather );
		++tcCheck;
	}

	// Check NPCs marked as always active, regardless of whether their region is "awake"
	if( allowAwakeNPCs )
	{
		GenericList< CChar * > *alwaysAwakeChars = Npcs->GetAlwaysAwakeNPCs();
		alwaysAwakeChars->Push();
		for( CChar *charCheck = alwaysAwakeChars->First(); !alwaysAwakeChars->Finished(); charCheck = alwaysAwakeChars->Next() )
		{
			if( !ValidateObject( charCheck ) || charCheck->isFree() || !charCheck->IsNpc() )
			{
				alwaysAwakeChars->Remove( charCheck );
				continue;
			}

			if( !genericCheck( nullptr, (*charCheck), checkFieldEffects, doWeather ) )
			{
				if( setNPCFlags )
					UpdateFlag( charCheck );	 // only set flag on npcs every 60 seconds (save a little extra lag)
				checkNPC( (*charCheck), checkAI, doRestock, doPetOfflineCheck );
			}
		}
		alwaysAwakeChars->Pop();
	}

	Effects->checktempeffects();
	SpeechSys->Poll();

	// Implement RefreshItem() / statwindow() queue here
	for (auto &entry : cwmWorldState->refreshQueue){
		CBaseObject *mObj = entry.first;
		if( ValidateObject( mObj ) )
		{
			if( mObj->CanBeObjType( OT_CHAR ) )
			{
				CChar *uChar = static_cast<CChar *>(mObj);

				if( uChar->GetUpdate( UT_HITPOINTS ) )
					updateStats( mObj, 0 );
				if( uChar->GetUpdate( UT_STAMINA ) )
					updateStats( mObj, 1 );
				if( uChar->GetUpdate( UT_MANA ) )
					updateStats( mObj, 2 );

				if( uChar->GetUpdate( UT_LOCATION ) )
					uChar->Teleport();
				else if( uChar->GetUpdate( UT_HIDE ) )
				{
					uChar->ClearUpdate();
					if( uChar->GetVisible() != VT_VISIBLE )
						uChar->RemoveFromSight();
					uChar->Update( nullptr, false );
				}
				else if( uChar->GetUpdate( UT_UPDATE ) )
					uChar->Update();
				else if( uChar->GetUpdate( UT_STATWINDOW ) )
				{
					CSocket *uSock = uChar->GetSocket();
					if( uSock != nullptr )
						uSock->statwindow( uChar );
				}

				uChar->ClearUpdate();
			}
			else
				mObj->Update();
		}
	}
	cwmWorldState->refreshQueue.clear();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void InitClasses( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Initialize UOX classes
//o-----------------------------------------------------------------------------------------------o
void InitClasses( void )
{
	cwmWorldState->ClassesInitialized( true );

	JSMapping		= nullptr;	Effects		= nullptr;
	Commands		= nullptr;	Combat		= nullptr;
	Items			= nullptr;	Map			= nullptr;
	Npcs			= nullptr;	Skills		= nullptr;
	Weight			= nullptr;	JailSys		= nullptr;
	Network			= nullptr;	Magic		= nullptr;
	Races			= nullptr;	Weather		= nullptr;
	Movement		= nullptr;	GuildSys	= nullptr;
	WhoList			= nullptr;	OffList		= nullptr;
	Books			= nullptr;	GMQueue		= nullptr;
	Dictionary		= nullptr;	Accounts	= nullptr;
	MapRegion		= nullptr;	SpeechSys	= nullptr;
	CounselorQueue	= nullptr;
	HTMLTemplates	= nullptr;
	FileLookup		= nullptr;

	JSEngine		= new CJSEngine;
	// MAKE SURE IF YOU ADD A NEW ALLOCATION HERE THAT YOU FREE IT UP IN Shutdown(...)
	if(( FileLookup		= new CServerDefinitions() )			== nullptr ) Shutdown( FATAL_UOX3_ALLOC_SCRIPTS );
	if(( Dictionary		= new CDictionaryContainer )			== nullptr ) Shutdown( FATAL_UOX3_ALLOC_DICTIONARY );
	if(( Combat			= new CHandleCombat )					== nullptr ) Shutdown( FATAL_UOX3_ALLOC_COMBAT );
	if(( Commands		= new cCommands )						== nullptr ) Shutdown( FATAL_UOX3_ALLOC_COMMANDS );
	if(( Items			= new cItem )							== nullptr ) Shutdown( FATAL_UOX3_ALLOC_ITEMS );
	if(( Map			= new CMulHandler )						== nullptr ) Shutdown( FATAL_UOX3_ALLOC_MAP );
	if(( Npcs			= new cCharStuff )						== nullptr ) Shutdown( FATAL_UOX3_ALLOC_NPCS );
	if(( Skills			= new cSkills )							== nullptr ) Shutdown( FATAL_UOX3_ALLOC_SKILLS );
	if(( Weight			= new CWeight )							== nullptr ) Shutdown( FATAL_UOX3_ALLOC_WEIGHT );
	if(( Network		= new cNetworkStuff )					== nullptr ) Shutdown( FATAL_UOX3_ALLOC_NETWORK );
	if(( Magic			= new cMagic )							== nullptr ) Shutdown( FATAL_UOX3_ALLOC_MAGIC );
	if(( Races			= new cRaces )							== nullptr ) Shutdown( FATAL_UOX3_ALLOC_RACES );
	if(( Weather		= new cWeatherAb )						== nullptr ) Shutdown( FATAL_UOX3_ALLOC_WEATHER );
	if(( Movement		= new cMovement )						== nullptr ) Shutdown( FATAL_UOX3_ALLOC_MOVE );
	if(( WhoList		= new cWhoList )						== nullptr ) Shutdown( FATAL_UOX3_ALLOC_WHOLIST );	// wholist
	if(( OffList		= new cWhoList( false ) )				== nullptr ) Shutdown( FATAL_UOX3_ALLOC_WHOLIST );	// offlist
	if(( Books			= new cBooks )							== nullptr ) Shutdown( FATAL_UOX3_ALLOC_BOOKS );
	if(( GMQueue		= new PageVector( "GM Queue" ) )		== nullptr ) Shutdown( FATAL_UOX3_ALLOC_PAGEVECTOR );
	if(( CounselorQueue	= new PageVector( "Counselor Queue" ) )	== nullptr ) Shutdown( FATAL_UOX3_ALLOC_PAGEVECTOR );
	if(( JSMapping		= new CJSMapping )						== nullptr ) Shutdown( FATAL_UOX3_ALLOC_TRIGGERS );
	JSMapping->ResetDefaults();
	JSMapping->GetEnvokeByID()->Parse();
	JSMapping->GetEnvokeByType()->Parse();
	if(( MapRegion		= new CMapHandler )						== nullptr ) Shutdown( FATAL_UOX3_ALLOC_MAPREGION );
	if(( Effects		= new cEffects )						== nullptr ) Shutdown( FATAL_UOX3_ALLOC_EFFECTS );
	if(( HTMLTemplates	= new cHTMLTemplates )					== nullptr ) Shutdown( FATAL_UOX3_ALLOC_HTMLTEMPLATES );
	if(( Accounts		= new cAccountClass( cwmWorldState->ServerData()->Directory( CSDDP_ACCOUNTS ) ) ) == nullptr ) Shutdown( FATAL_UOX3_ALLOC_ACCOUNTS );
	if(( SpeechSys		= new CSpeechQueue()	)				== nullptr ) Shutdown( FATAL_UOX3_ALLOC_SPEECHSYS );
	if(( GuildSys		= new CGuildCollection() )				== nullptr ) Shutdown( FATAL_UOX3_ALLOC_GUILDS );
	if(( JailSys		= new JailSystem() )					== nullptr ) Shutdown( FATAL_UOX3_ALLOC_JAILSYS );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void ParseArgs( SI32 argc, char *argv[] )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle command-line arguments on startup
//o-----------------------------------------------------------------------------------------------o
void ParseArgs( SI32 argc, char *argv[] )
{
	for( SI32 i = 1; i < argc; ++i )
	{
		// FIXME why is this for loop remaining?
	}
}

auto findNearbyObjects( SI16 x, SI16 y, UI08 worldNumber, UI16 instanceID, UI16 distance )->std::vector< CBaseObject* >	;
bool inMulti( SI16 x, SI16 y, SI08 z, CMultiObj *m );
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool FindMultiFunctor( CBaseObject *a, UI32 &b, void *extraData )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Looks for a multi at object's location and assigns any multi found to object
//o-----------------------------------------------------------------------------------------------o
bool FindMultiFunctor( CBaseObject *a, UI32 &b, void *extraData )
{
	if( ValidateObject( a ) )
	{
		if( a->CanBeObjType( OT_MULTI ))
		{
			CMultiObj *aMulti = static_cast<CMultiObj *>(a);
			auto objectList = findNearbyObjects( aMulti->GetX(), aMulti->GetY(), aMulti->WorldNumber(), aMulti->GetInstanceID(), 20 );
			for( auto &objToCheck:objectList){

				if( inMulti( objToCheck->GetX(), objToCheck->GetY(), objToCheck->GetZ(), aMulti ))
					objToCheck->SetMulti( aMulti );
				else if( objToCheck->GetObjType() == OT_ITEM
					&& ( ( objToCheck->GetID() >= 0x0b95 && objToCheck->GetID() <= 0x0c0e ) || objToCheck->GetID() == 0x1f28 || objToCheck->GetID() == 0x1f29 ) )
				{
					// Reunite house signs with their multis
					SERIAL houseSerial = static_cast<CItem *>( objToCheck )->GetTempVar( CITV_MORE );
					CMultiObj *multi = calcMultiFromSer( houseSerial );
					if( ValidateObject( multi ) )
					{
						objToCheck->SetMulti( multi );
					}
				}
				else
				{
					// No other multi found where item is, safe to set item's multi to INVALIDSERIAL
					if( findMulti( objToCheck ) == nullptr )
						objToCheck->SetMulti( INVALIDSERIAL );
				}
			}
		}
	}
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void InitMultis( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Initialize Multis
//o-----------------------------------------------------------------------------------------------o
void InitMultis( void )
{
	Console << "Initializing multis            ";

	UI32 b		= 0;
	ObjectFactory::getSingleton().IterateOver( OT_MULTI, b, nullptr, &FindMultiFunctor );

	Console.PrintDone();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void DisplayBanner( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Display some information at the end of UOX startup
//o-----------------------------------------------------------------------------------------------o
void DisplayBanner( void )
{
	Console.PrintSectionBegin();


	//auto idName = oldstrutil::format( "%s v%s(%s) [%s]\n| Compiled by %s\n| Programmed by: %s", CVersionClass::GetProductName().c_str(), CVersionClass::GetVersion().c_str(), CVersionClass::GetBuild().c_str(), OS_STR, CVersionClass::GetName().c_str(), CVersionClass::GetProgrammers().c_str() );

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


//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void Shutdown( SI32 retCode )
//|	Date		-	Oct. 09, 1999
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handled deleting / free() ing of pointers as neccessary
//|					as well as closing open file handles to avoid file file corruption.
//|					Exits with proper error code.
//o-----------------------------------------------------------------------------------------------o
void Shutdown( SI32 retCode )
{
	Console.PrintSectionBegin();
	Console << "Beginning UOX final shut down sequence..." << myendl;

	if( retCode && cwmWorldState && cwmWorldState->GetLoaded() && cwmWorldState->GetWorldSaveProgress() != SS_SAVING )
	{//they want us to save, there has been an error, we have loaded the world, and WorldState is a valid pointer.
#if PLATFORM == WINDOWS
	SetConsoleCtrlHandler( exit_handler, TRUE );
#endif
		isWorldSaving = true;
		do
		{
			cwmWorldState->SaveNewWorld( true );
		} while( cwmWorldState->GetWorldSaveProgress() == SS_SAVING );
		isWorldSaving = false;
#if PLATFORM == WINDOWS
	SetConsoleCtrlHandler( exit_handler, FALSE );
#endif
	}

	if( cwmWorldState && cwmWorldState->ClassesInitialized() )
	{
		if( HTMLTemplates )
		{
			Console << "HTMLTemplates object detected. Writing Offline HTML Now...";
			HTMLTemplates->Poll( ETT_OFFLINE );
			Console.PrintDone();
		}
		else
			Console << "HTMLTemplates object not found." << myendl;

		Console << "Cleaning up item and character memory... ";
		ObjectFactory::getSingleton().GarbageCollect();
		Console.PrintDone();

		Console << "Destroying class objects and pointers... ";
		// delete any objects that were created (delete takes care of nullptr check =)
		UnloadSpawnRegions();

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

		UnloadRegions();
		Console.PrintDone();

		delete JSEngine;
	}

	//Lets wait for console thread to quit here
	if( !retCode )
		cons.join();

	// don't leave file pointers open, could lead to file corruption

	Console.PrintSectionBegin();
	delete cwmWorldState;

	Console.TurnGreen();
	Console << "Server shutdown complete!" << myendl;
	Console << "Thank you for supporting " << CVersionClass::GetName() << myendl;
	Console.TurnNormal();
	Console.PrintSectionBegin();

	// dispay what error code we had
	// don't report errorlevel for no errors, this is confusing ppl
	if( retCode )
	{
		Console.TurnRed();
		Console << "Exiting UOX with errorlevel " << retCode << myendl;
		Console.TurnNormal();
#if PLATFORM == WINDOWS
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void advanceObj( CChar *applyTo, UI16 advObj, bool multiUse )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle advancement objects (stat / skill gates)
//o-----------------------------------------------------------------------------------------------o
void advanceObj( CChar *applyTo, UI16 advObj, bool multiUse )
{
	if( applyTo->GetAdvObj() == 0 || multiUse )
	{
		Effects->PlayStaticAnimation( applyTo, 0x373A, 0, 15);
		Effects->PlaySound( applyTo, 0x01E9 );
		applyTo->SetAdvObj( advObj );
		std::string sect			= std::string("ADVANCEMENT ") + oldstrutil::number( advObj );
		sect						= oldstrutil::trim( oldstrutil::removeTrailing( sect, "//" ));
		ScriptSection *Advancement	= FileLookup->FindEntry( sect, advance_def );
		if( Advancement == nullptr )
		{
			Console << "ADVANCEMENT OBJECT: Script section not found, Aborting" << myendl;
			applyTo->SetAdvObj( 0 );
			return;
		}
		CItem *retItem		= nullptr;
		CItem *hairobject	= applyTo->GetItemAtLayer( IL_HAIR );
		CItem *beardobject	= applyTo->GetItemAtLayer( IL_FACIALHAIR );
		DFNTAGS tag			= DFNTAG_COUNTOFTAGS;
		std::string cdata;
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
					retItem = Items->CreateBaseScriptItem( nullptr, cdata, applyTo->WorldNumber(), 1 );
					if( retItem != nullptr )
					{
						if( !retItem->SetCont( applyTo ) )
						{
							retItem->SetCont( applyTo->GetPackItem() );
							retItem->PlaceInPack();
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
				case DFNTAG_IMBUING:			skillToSet = IMBUING;							break;
				case DFNTAG_INTELLIGENCE:		applyTo->SetIntelligence( static_cast<SI16>(RandomNum( ndata, odata )) );	break;
				case DFNTAG_ITEMID:				skillToSet = ITEMID;							break;
				case DFNTAG_INSCRIPTION:		skillToSet = INSCRIPTION;						break;
				case DFNTAG_KARMA:				applyTo->SetKarma( static_cast<SI16>(ndata) );	break;
				case DFNTAG_KILLHAIR:
					retItem = applyTo->GetItemAtLayer( IL_HAIR );
					if( ValidateObject( retItem ) )
						retItem->Delete();
					break;
				case DFNTAG_KILLBEARD:
					retItem = applyTo->GetItemAtLayer( IL_FACIALHAIR );
					if( ValidateObject( retItem ) )
						retItem->Delete();
					break;
				case DFNTAG_KILLPACK:
					retItem = applyTo->GetItemAtLayer( IL_PACKITEM );
					if( ValidateObject( retItem ) )
						retItem->Delete();
					break;
				case DFNTAG_LOCKPICKING:		skillToSet = LOCKPICKING;					break;
				case DFNTAG_LUMBERJACKING:		skillToSet = LUMBERJACKING;					break;
				case DFNTAG_MAGERY:				skillToSet = MAGERY;						break;
				case DFNTAG_MAGICRESISTANCE:	skillToSet = MAGICRESISTANCE;				break;
				case DFNTAG_MACEFIGHTING:		skillToSet = MACEFIGHTING;					break;
				case DFNTAG_MEDITATION:			skillToSet = MEDITATION;					break;
				case DFNTAG_MINING:				skillToSet = MINING;						break;
				case DFNTAG_MUSICIANSHIP:		skillToSet = MUSICIANSHIP;					break;
				case DFNTAG_MYSTICISM:			skillToSet = MYSTICISM;						break;
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
						auto csecs = oldstrutil::sections( cdata, "," );
						if( !cdata.empty() )
						{
							if( csecs.size() > 1 )
							{
								retItem = Items->CreateScriptItem( nullptr, applyTo, oldstrutil::trim(oldstrutil::removeTrailing( csecs[0],"//") ), oldstrutil::value<std::uint16_t>( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" ))), OT_ITEM, true );
							}
							else
							{
								retItem = Items->CreateScriptItem( nullptr, applyTo, cdata, 1, OT_ITEM, true );
							}
						}
					}
					else
						Console << "Warning: Bad NPC Script with problem no backpack for packitem" << myendl;
					break;
				case DFNTAG_REMOVETRAP:			skillToSet = REMOVETRAP;					break;
				case DFNTAG_STRENGTH:			applyTo->SetStrength( static_cast<SI16>(RandomNum( ndata, odata )) );			break;
				case DFNTAG_SKILL:				applyTo->SetBaseSkill( static_cast<UI16>(odata), static_cast<UI08>(ndata) );	break;
				case DFNTAG_SKIN:				applyTo->SetSkin( static_cast<UI16>(std::stoul(cdata, nullptr, 0) ));			break;
				case DFNTAG_SNOOPING:			skillToSet = SNOOPING;						break;
				case DFNTAG_SPELLWEAVING:		skillToSet = SPELLWEAVING;					break;
				case DFNTAG_SPIRITSPEAK:		skillToSet = SPIRITSPEAK;					break;
				case DFNTAG_STEALING:			skillToSet = STEALING;						break;
				case DFNTAG_STEALTH:			skillToSet = STEALTH;						break;
				case DFNTAG_SWORDSMANSHIP:		skillToSet = SWORDSMANSHIP;					break;
				case DFNTAG_TACTICS:			skillToSet = TACTICS;						break;
				case DFNTAG_TAILORING:			skillToSet = TAILORING;						break;
				case DFNTAG_TAMING:				skillToSet = TAMING;						break;
				case DFNTAG_TASTEID:			skillToSet = TASTEID;						break;
				case DFNTAG_THROWING:			skillToSet = THROWING;						break;
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
		if( sock != nullptr )
			sock->sysmessage( 1366 );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 getclock( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Return CPU time used, Emulates clock()
//o-----------------------------------------------------------------------------------------------o
UI32 getclock( void )
{
	auto now = std::chrono::system_clock::now();
	return static_cast<std::uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(now-current).count());
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 roundNumber( R32 toRound )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	rounds a number up or down depending on it's value
//o-----------------------------------------------------------------------------------------------o
R32 roundNumber( R32 toRound)
{
	R32 flVal = floor( toRound );
	if( flVal < floor( toRound + 0.5 ) )
		return ceil( toRound );
	return flVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool isNumber( const std::string& str )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if string is a number, false if not
//o-----------------------------------------------------------------------------------------------o
bool isNumber( const std::string& str )
{
	return str.find_first_not_of( "0123456789" ) == std::string::npos;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void doLight( CSocket *s, UI08 level )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets light level for player and applies relevant effects
//o-----------------------------------------------------------------------------------------------o
void doLight( CSocket *s, UI08 level )
{
	if( s == nullptr )
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
	CWeather *wSys = Weather->Weather( curRegion->GetWeather() );
	LIGHTLEVEL toShow = cwmWorldState->ServerData()->WorldLightCurrentLevel();

	LIGHTLEVEL dunLevel = cwmWorldState->ServerData()->DungeonLightLevel();
	// we have a valid weather system
	if( wSys != nullptr )
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

	bool eventFound = false;
	std::vector<UI16> scriptTriggers = mChar->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			if( toExecute->OnLightChange( mChar, toShow ) == 1 )
			{
				// A script with the event returned true; prevent other scripts from running
				eventFound = true;
				break;
			}
		}
	}

	if( !eventFound )
	{
		// Check global script! Maybe there's another event there
		cScript *toExecute = JSMapping->GetScript( static_cast<UI16>(0) );
		if( toExecute != nullptr )
			toExecute->OnLightChange( mChar, toShow );
	}

	Weather->DoPlayerStuff( s, mChar );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void doLight( CChar *mChar, UI08 level )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets light level for character and applies relevant effects
//o-----------------------------------------------------------------------------------------------o
void doLight( CChar *mChar, UI08 level )
{
	if( (Races->Affect( mChar->GetRace(), LIGHT )) && mChar->GetWeathDamage( LIGHT ) == 0 )
		mChar->SetWeathDamage( static_cast<UI32>(BuildTimeValue( static_cast<R32>(Races->Secs( mChar->GetRace(), LIGHT )) )), LIGHT );

	CTownRegion *curRegion	= mChar->GetRegion();
	CWeather *wSys			= Weather->Weather( curRegion->GetWeather() );

	LIGHTLEVEL toShow = level;

	LIGHTLEVEL dunLevel = cwmWorldState->ServerData()->DungeonLightLevel();

	// we have a valid weather system
	if( wSys != nullptr )
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

	bool eventFound = false;
	std::vector<UI16> scriptTriggers = mChar->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			if( toExecute->OnLightChange( mChar, toShow ) == 1 )
			{
				// A script with the event returned true; prevent other scripts from running
				eventFound = true;
				break;
			}
		}
	}

	if( !eventFound )
	{
		// Check global script! Maybe there's another event there
		cScript *toExecute = JSMapping->GetScript( static_cast<UI16>(0) );
		if( toExecute != nullptr )
			toExecute->OnLightChange( mChar, toShow );
	}

	Weather->DoNPCStuff( mChar );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void doLight( CItem *mItem, UI08 level )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets light level for items and applies relevant effects
//o-----------------------------------------------------------------------------------------------o
void doLight( CItem *mItem, UI08 level )
{
	CTownRegion *curRegion	= mItem->GetRegion();
	CWeather *wSys			= Weather->Weather( curRegion->GetWeather() );

	LIGHTLEVEL toShow = level;

	LIGHTLEVEL dunLevel = cwmWorldState->ServerData()->DungeonLightLevel();

	// we have a valid weather system
	if( wSys != nullptr )
	{
		const R32 lightMin = wSys->LightMin();
		const R32 lightMax = wSys->LightMax();
		if( lightMin < 300 && lightMax < 300 )
		{
			toShow = static_cast<LIGHTLEVEL>(wSys->CurrentLight());
		}
	}
	else
	{
		if( mItem->inDungeon() )
		{
			toShow = dunLevel;
		}
	}

	bool eventFound = false;
	std::vector<UI16> scriptTriggers = mItem->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			if( toExecute->OnLightChange( mItem, toShow ) == 1 )
			{
				// A script with the event returned true; prevent other scripts from running
				eventFound = true;
				break;
			}
		}
	}

	if( !eventFound )
	{
		// Check global script! Maybe there's another event there
		cScript *toExecute = JSMapping->GetScript( static_cast<UI16>(0) );
		if( toExecute != nullptr )
			toExecute->OnLightChange( mItem, toShow );
	}

	Weather->DoItemStuff( mItem );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	TIMERVAL getPoisonDuration( UI08 poisonStrength )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculates the duration of poison based on its strength
//o-----------------------------------------------------------------------------------------------o
TIMERVAL getPoisonDuration( UI08 poisonStrength )
{
	// Calculate duration of poison, based on the strength of the poison
	TIMERVAL poisonDuration = 0;
	switch( poisonStrength )
	{
		case 1: // Lesser poison - 9 to 13 pulses, 2 second frequency
			poisonDuration = RandomNum( 9, 13 ) * 2;
			break;
		case 2: // Normal poison - 10 to 14 pulses, 3 second frequency
			poisonDuration = RandomNum( 10, 14 ) * 3;
			break;
		case 3: // Greater poison - 11 to 15 pulses, 4 second frequency
			poisonDuration = RandomNum( 11, 15 ) * 4;
			break;
		case 4: // Deadly poison - 12 to 16 pulses, 5 second frequency
			poisonDuration = RandomNum( 12, 16 ) * 5;
			break;
		case 5: // Lethal poison - 13 to 17 pulses, 5 second frequency
			poisonDuration = RandomNum( 13, 17 ) * 5;
			break;
	}
	return poisonDuration;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	TIMERVAL getPoisonTickTime( UI08 poisonStrength )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculates the time between each tick of a poison, based on its strength
//o-----------------------------------------------------------------------------------------------o
TIMERVAL getPoisonTickTime( UI08 poisonStrength )
{
	// Calculate duration of poison, based on the strength of the poison
	TIMERVAL poisonTickTime = 0;
	switch( poisonStrength )
	{
		case 1: // Lesser poison - 2 second frequency
			poisonTickTime = 2;
			break;
		case 2: // Normal poison - 3 second frequency
			poisonTickTime = 3;
			break;
		case 3: // Greater poison - 4 second frequency
			poisonTickTime = 4;
			break;
		case 4: // Deadly poison - 5 second frequency
			poisonTickTime = 5;
			break;
		case 5: // Lethal poison - 5 second frequency
			poisonTickTime = 5;
			break;
	}
	return poisonTickTime;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	size_t getTileName( CItem& mItem, std::string& itemname )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the length of an items name from tiledata.mul and
//|					sets itemname to the name.
//|					The format it accepts is same as UO style - %plural/single% or %plural%
//|						arrow%s%
//|						loa%ves/f% of bread
//o-----------------------------------------------------------------------------------------------o
size_t getTileName( CItem& mItem, std::string& itemname )
{
	std::string temp	= mItem.GetName();
	temp				= oldstrutil::trim( oldstrutil::removeTrailing( temp, "//" ));
	const UI16 getAmount = mItem.GetAmount();
	CTile& tile = Map->SeekTile( mItem.GetID() );
	if( temp.substr( 0, 1 ) == "#" )
	{
		temp = tile.Name();
	}
	
	if( getAmount == 1 )
	{
		if( tile.CheckFlag( TF_DISPLAYAN ) )
			temp = "an " + temp;
		else if( tile.CheckFlag( TF_DISPLAYA ) )
			temp = "a " + temp;
	}

	auto psecs = oldstrutil::sections( temp, "%" );
	// Find out if the name has a % in it
	if( psecs.size() > 2 )
	{
		std::string single;
		const std::string first	= psecs[0];
		std::string plural		= psecs[1];
		const std::string rest	= psecs[2];
		auto fssecs = oldstrutil::sections( plural, "/" );
		if( fssecs.size() > 1 )
		{
			single = fssecs[1];
			plural = fssecs[0];
		}
		if( getAmount < 2 )
			temp = first + single + rest;
		else
			temp = first + plural + rest;
	}
	itemname = oldstrutil::simplify( temp );
	return itemname.size() + 1;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	std::string getNpcDictName( CChar *mChar, CSocket *tSock )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the dictionary name for a given NPC, if their name equals # or a dictionary ID
//o-----------------------------------------------------------------------------------------------o
std::string getNpcDictName( CChar *mChar, CSocket *tSock )
{
	CChar *tChar = nullptr;
	if( tSock != nullptr )
		tChar = tSock->CurrcharObj();

	std::string dictName = mChar->GetNameRequest( tChar );
	SI32 dictEntryID = 0;

	if( dictName == "#" )
	{
		// If character name is #, get dictionary entry based on base dictionary entry for creature names (3000) plus character's ID
		dictEntryID = static_cast<SI32>( 3000 + mChar->GetID() );
		if( tSock != nullptr )
			dictName = Dictionary->GetEntry( dictEntryID, tSock->Language() );
		else
			dictName = Dictionary->GetEntry( dictEntryID );
	}
	else if( isNumber( dictName ))
	{
		// If name is a number, assume it's a direct dictionary entry reference, and use that
		dictEntryID = static_cast<SI32>( oldstrutil::value<SI32>( dictName ));
		if( tSock != nullptr )
			dictName = Dictionary->GetEntry( dictEntryID, tSock->Language() );
		else
			dictName = Dictionary->GetEntry( dictEntryID );
	}

	return dictName;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	std::string getNpcDictTitle( CChar *mChar, CSocket *tSock )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the dictionary string for the title of a given NPC, if their title 
//|					equals a dictionary ID
//o-----------------------------------------------------------------------------------------------o
std::string getNpcDictTitle( CChar *mChar, CSocket *tSock )
{
	std::string dictTitle = mChar->GetTitle();
	SI32 dictEntryID = 0;

	if( isNumber( dictTitle ) )
	{
		// If title is a number, assume it's a direct dictionary entry reference, and use that
		dictEntryID = static_cast<SI32>( oldstrutil::value<SI32>( dictTitle ) );
		if( tSock != nullptr )
			dictTitle = Dictionary->GetEntry( dictEntryID, tSock->Language() );
		else
			dictTitle = Dictionary->GetEntry( dictEntryID );
	}

	return dictTitle;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void checkRegion( CSocket *mSock, CChar& mChar, bool forceUpdateLight)
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Check what region a character is in, updating it if necesarry.
//o-----------------------------------------------------------------------------------------------o
void checkRegion( CSocket *mSock, CChar& mChar, bool forceUpdateLight)
{
	// Get character's old/previous region
	CTownRegion *iRegion = mChar.GetRegion();
	UI16 oldSubRegionNum = mChar.GetSubRegion();

	// Calculate character's current region
	CTownRegion *calcReg = calcRegionFromXY( mChar.GetX(), mChar.GetY(), mChar.WorldNumber(), mChar.GetInstanceID(), &mChar );

	if( iRegion == nullptr && calcReg != nullptr )
		mChar.SetRegion( calcReg->GetRegionNum() );
	else if( calcReg != iRegion )
	{
		if( mSock != nullptr )
		{
			if( iRegion != nullptr && calcReg != nullptr )
			{
				// Don't display left/entered region messages if name of region is identical
				if( iRegion->GetName() != calcReg->GetName() )
				{
					if( !iRegion->GetName().empty() )
						mSock->sysmessage( 1358, iRegion->GetName().c_str() ); // You have left %s.

					if( !calcReg->GetName().empty() )
						mSock->sysmessage( 1359, calcReg->GetName().c_str() ); // You have entered %s.
				}
				if( calcReg->IsGuarded() || iRegion->IsGuarded() )
				{
					if( calcReg->IsGuarded() )
					{
						// Don't display change of guard message if guardowner is identical
						if( !iRegion->IsGuarded() || ( iRegion->IsGuarded() && calcReg->GetOwner() != iRegion->GetOwner() ) )
						{
							if( calcReg->GetOwner().empty() )
								mSock->sysmessage( 1360 ); // You are now under the protection of the guards.
							else
								mSock->sysmessage( 1361, calcReg->GetOwner().c_str() ); // You are now under the protection of %s guards.
						}
					}
					else
					{
						if( iRegion->GetOwner().empty() )
							mSock->sysmessage( 1362 ); // You are no longer under the protection of the guards.
						else
							mSock->sysmessage( 1363, iRegion->GetOwner().c_str() ); // You are no longer under the protection of %s guards.
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
					mSock->sysmessage( 1364 ); // You feel loved and cherished under the protection of your home town.
					CItem *packItem = mChar.GetPackItem();
					if( ValidateObject( packItem ) )
					{
						GenericList< CItem * > *piCont = packItem->GetContainsList();
						for( CItem *toScan = piCont->First(); !piCont->Finished(); toScan = piCont->Next() )
						{
							if( ValidateObject( toScan ) )
							{
								if( toScan->GetType() == IT_TOWNSTONE )
								{
									CTownRegion *targRegion = cwmWorldState->townRegions[static_cast<UI16>(toScan->GetTempVar( CITV_MOREX ))];
									mSock->sysmessage( 1365, targRegion->GetName().c_str() ); // You have successfully returned the townstone of %s to your home town.
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
		if( iRegion != nullptr && calcReg != nullptr )
		{
			// Run onLeaveRegion/onEnterRegion for character
			std::vector<UI16> scriptTriggers = mChar.GetScriptTriggers();
			for( auto scriptTrig : scriptTriggers )
			{
				cScript *toExecute = JSMapping->GetScript( scriptTrig );
				if( toExecute != nullptr )
				{
					toExecute->OnLeaveRegion( &mChar, iRegion->GetRegionNum() );
					toExecute->OnEnterRegion( &mChar, calcReg->GetRegionNum() );
				}
			}

			// Run onLeaveRegion event for region being left
			scriptTriggers.clear();
			scriptTriggers.shrink_to_fit();
			scriptTriggers = iRegion->GetScriptTriggers();
			for( auto scriptTrig : scriptTriggers )
			{
				cScript *toExecute = JSMapping->GetScript( scriptTrig );
				if( toExecute != nullptr )
				{
					toExecute->OnLeaveRegion( &mChar, iRegion->GetRegionNum() );
				}
			}

			// Run onEnterRegion event for region being entered
			scriptTriggers.clear();
			scriptTriggers.shrink_to_fit();
			scriptTriggers = calcReg->GetScriptTriggers();
			for( auto scriptTrig : scriptTriggers )
			{
				cScript *toExecute = JSMapping->GetScript( scriptTrig );
				if( toExecute != nullptr )
				{
					toExecute->OnEnterRegion( &mChar, calcReg->GetRegionNum() );
				}
			}
		}
		if( calcReg != nullptr )
			mChar.SetRegion( calcReg->GetRegionNum() );
		if( mSock != nullptr )
		{
			Effects->doSocketMusic( mSock );
			doLight( mSock, cwmWorldState->ServerData()->WorldLightCurrentLevel() );
		}
	}
	else
	{
		// Main region didn't change, but subregion did! Update music
		if( oldSubRegionNum != mChar.GetSubRegion() )
		{
			Effects->doSocketMusic( mSock );
		}

		// Update lighting
		if( forceUpdateLight && mSock != nullptr )
		{
			doLight( mSock, cwmWorldState->ServerData()->WorldLightCurrentLevel() );
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CheckCharInsideBuilding( CChar *c, CSocket *mSock, bool doWeatherStuff )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if a character is inside a building before applying weather effects
//o-----------------------------------------------------------------------------------------------o
void CheckCharInsideBuilding( CChar *c, CSocket *mSock, bool doWeatherStuff )
{
	if( c->GetMounted() || c->GetStabled() )
		return;

	bool wasInBuilding = c->inBuilding();
	bool isInBuilding = Map->inBuilding( c->GetX(), c->GetY(), c->GetZ(), c->WorldNumber(), c->GetInstanceID() );
	if( wasInBuilding != isInBuilding )
	{
		c->SetInBuilding( isInBuilding );
		if( doWeatherStuff ){
			if( c->IsNpc() ) {
				Weather->DoNPCStuff( c );
			}
			else {
				Weather->DoPlayerStuff( mSock, c );
			}
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool WillResultInCriminal( CChar *mChar, CChar *targ )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Check flagging, race, and guild info to find if character
//|					should be flagged criminal (returns true if so)
//o-----------------------------------------------------------------------------------------------o
bool WillResultInCriminal( CChar *mChar, CChar *targ )
{
	CChar *tOwner = targ->GetOwnerObj();
	CChar *mOwner = mChar->GetOwnerObj();
	Party *mCharParty = PartyFactory::getSingleton().Get( mChar );
	if( !ValidateObject( mChar ) || !ValidateObject( targ ) || mChar == targ )
		return false;
	else if( !GuildSys->ResultInCriminal( mChar, targ ) || Races->Compare( mChar, targ ) <= RACE_ENEMY )
		return false;
	else if( mCharParty && mCharParty->HasMember( targ ) )
		return false;
	else if( targ->DidAttackFirst() && targ->GetTarg() == mChar )
		return false;
	else if( ValidateObject( tOwner ) )
	{
		if( tOwner == mChar || tOwner == mChar->GetOwnerObj() )
			return false;
	}
	else if( ValidateObject( mOwner ) )
	{
		if( mOwner == targ || mOwner == targ->GetOwnerObj() )
			return false;
	}
	else if( targ->IsInnocent() )
		return true;
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void criminal( CChar *c )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Make character a criminal
//o-----------------------------------------------------------------------------------------------o
void criminal( CChar *c )
{
	c->SetTimer( tCHAR_CRIMFLAG, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_CRIMINAL ) );
	if( !c->IsCriminal() && !c->IsMurderer() )
	{
		CSocket *cSock = c->GetSocket();
		if( cSock != nullptr )
			cSock->sysmessage( 1379 );
		UpdateFlag( c );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void UpdateFlag( CChar *mChar )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Updates character flags
//o-----------------------------------------------------------------------------------------------o
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
			Console.warning( oldstrutil::format("Tamed Creature has an invalid owner, Serial: 0x%X", mChar->GetSerial()) );
		}
	}
	else
	{
		if( mChar->GetKills() > cwmWorldState->ServerData()->RepMaxKills() )
			mChar->SetFlagRed();
		else if( mChar->GetTimer( tCHAR_CRIMFLAG ) != 0 && mChar->GetNPCFlag() != fNPC_EVIL )
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
		std::vector<UI16> scriptTriggers = mChar->GetScriptTriggers();
		for( auto scriptTrig : scriptTriggers )
		{
			cScript *toExecute = JSMapping->GetScript( scriptTrig );
			if( toExecute != nullptr )
			{
				if( toExecute->OnFlagChange( mChar, newFlag, oldFlag ) == 1 )
				{
					break;
				}
			}
		}

		mChar->Dirty( UT_UPDATE );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SendMapChange( UI08 worldNumber, CSocket *sock, bool initialLogin )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Send mapchange packet to client to teleport player to new world/map
//o-----------------------------------------------------------------------------------------------o
void SendMapChange( UI08 worldNumber, CSocket *sock, bool initialLogin )
{
	if( sock == nullptr )
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
	//CChar *mChar = sock->CurrcharObj();
	/*if( !initialLogin )
		mChar->Teleport();*/
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SocketMapChange( CSocket *sock, CChar *charMoving, CItem *gate )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if conditions are right to send a map change packet to the client
//o-----------------------------------------------------------------------------------------------o
void SocketMapChange( CSocket *sock, CChar *charMoving, CItem *gate )
{
	if( sock == nullptr )
		return;
	if( !ValidateObject( gate ) || !ValidateObject( charMoving ) )
		return;
	UI08 tWorldNum = (UI08)gate->GetTempVar( CITV_MORE );
	UI16 tInstanceID = gate->GetInstanceID();
	if( !Map->MapExists( tWorldNum ) )
		return;
	CChar *toMove = nullptr;
	if( ValidateObject( charMoving ) )
		toMove = charMoving;
	else
		toMove = sock->CurrcharObj();
	if( !ValidateObject( toMove ) )
		return;

	// Teleport pets to new location too!
	GenericList< CChar * > *myPets = toMove->GetPetList();
	for( CChar *myPet = myPets->First(); !myPets->Finished(); myPet = myPets->Next() )
	{
		if( !ValidateObject( myPet ) )
			continue;
		if( !myPet->GetMounted() && myPet->IsNpc() && myPet->GetOwnerObj() == toMove )
		{
			if( objInOldRange( toMove, myPet, DIST_CMDRANGE ) )
				myPet->SetLocation( (SI16)gate->GetTempVar( CITV_MOREX ), (SI16)gate->GetTempVar( CITV_MOREY ), (SI08)gate->GetTempVar( CITV_MOREZ ), tWorldNum, tInstanceID );
		}
	}

	switch( sock->ClientType() )
	{
		case CV_UO3D:
		case CV_KRRIOS:
			toMove->SetLocation( (SI16)gate->GetTempVar( CITV_MOREX ), (SI16)gate->GetTempVar( CITV_MOREY ), (SI08)gate->GetTempVar( CITV_MOREZ ), tWorldNum, tInstanceID );
			break;
		default:
			toMove->SetLocation( (SI16)gate->GetTempVar( CITV_MOREX ), (SI16)gate->GetTempVar( CITV_MOREY ), (SI08)gate->GetTempVar( CITV_MOREZ ), tWorldNum, tInstanceID );
			break;
	}
	SendMapChange( tWorldNum, sock );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void DoorMacro( CSocket *s )
//|	Date		-	11th October, 1999
//|	Changes		-	(support CSocket *s and door blocking)
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Door use macro support.
//o-----------------------------------------------------------------------------------------------o
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

	auto nearbyRegions = MapRegion->PopulateList( mChar );
	for( auto &toCheck : nearbyRegions ){
	
		if( toCheck != nullptr ){	// no valid region
			
			GenericList< CItem * > *regItems = toCheck->GetItemList();
			regItems->Push();
			for( CItem *itemCheck = regItems->First(); !regItems->Finished(); itemCheck = regItems->Next() )
			{
				if( !ValidateObject( itemCheck ) || itemCheck->GetInstanceID() != mChar->GetInstanceID() )
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
							if( envExecute != nullptr )
								[[maybe_unused]] SI08 retVal = envExecute->OnUseChecked( mChar, itemCheck );
							
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	int main( SI32 argc, char *argv[] )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Main UOX startup
//o-----------------------------------------------------------------------------------------------o
int main( SI32 argc, char *argv[] )
{
	UI32 tempSecs, tempMilli, tempTime;
	UI32 loopSecs, loopMilli;

#if PLATFORM != WINDOWS
	// Protection from server-shutdown during mid-worldsave
	signal(SIGINT, app_stopped);
#endif

	// Let's measure startup time
	auto startupStartTime = std::chrono::high_resolution_clock::now();

	// 042102: I moved this here where it basically should be for any windows application or dll that uses WindowsSockets.
#if PLATFORM == WINDOWS
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD( 2, 2 );
	SI32 err = WSAStartup( wVersionRequested, &wsaData );
	if( err )
	{
		Console.error( "Winsock 2.2 not found on your system..." );
		return 1;
	}
#endif

#ifdef _CRASH_PROTECT_
	try
	{// Error trapping....
#endif
		current = std::chrono::system_clock::now();

		Console.Start( oldstrutil::format("%s v%s.%s (%s)", CVersionClass::GetProductName().c_str(), CVersionClass::GetVersion().c_str(), CVersionClass::GetBuild().c_str(), OS_STR ) );

#if PLATFORM != WINDOWS
		signal( SIGPIPE, SIG_IGN ); // This appears when we try to write to a broken network connection
//		signal( SIGTERM, &endmessage );
//		signal( SIGQUIT, &endmessage );
//		signal( SIGINT, &endmessage );
//		signal( SIGILL, &illinst );
//		signal( SIGFPE, &aus );
#endif
		Console.PrintSectionBegin();
		Console << "UOX Server start up!" << myendl << "Welcome to " << CVersionClass::GetProductName() << " v" << CVersionClass::GetVersion() << "." << CVersionClass::GetBuild() << " (" << OS_STR << ")" << myendl;
		Console.PrintSectionBegin();

		if(( cwmWorldState = new CWorldMain ) == nullptr )
			Shutdown( FATAL_UOX3_ALLOC_WORLDSTATE );
		cwmWorldState->ServerData()->Load();

		Console << "Initializing and creating class pointers... " << myendl;
		InitClasses();
		cwmWorldState->SetUICurrentTime( getclock() );

		ParseArgs( argc, argv );
		Console.PrintSectionBegin();

		cwmWorldState->ServerData()->LoadTime();

		Console << "Loading skill advancement      ";
		LoadSkills();
		Console.PrintDone();

		// Moved BulkStartup here, dunno why that function was there...
		Console << "Loading dictionaries...        " << myendl;
		Console.PrintBasedOnVal( Dictionary->LoadDictionary() >= 0 );

		Console << "Loading teleport               ";
		LoadTeleportLocations();
		Console.PrintDone();

		Console << "Loading GoPlaces               ";
		LoadPlaces();
		Console.PrintDone();
		std::random_device rd;  //Will be used to obtain a seed for the random number engine
		generator = std::mt19937(rd()); //Standard mersenne_twister_engine seeded with rd()

		//srand( current.tv_sec ); // initial randomization call

		CJSMappingSection *packetSection = JSMapping->GetSection( SCPT_PACKET );
		for( cScript *ourScript = packetSection->First(); !packetSection->Finished(); ourScript = packetSection->Next() )
		{
			if( ourScript != nullptr )
				ourScript->ScriptRegistration( "Packet" );
		}

		// moved all the map loading into CMulHandler
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
		Console.PrintDone();

		Console.log( "-=Server Startup=-\n=======================================================================", "server.log" );

		Console << "Creating and Initializing Console Thread      ";

		cons = std::thread(&CheckConsoleKeyThread);
#ifdef __LOGIN_THREAD__
		Console << myendl << "Creating and Initializing xLOGINd Thread      ";
		netw = std::thread(&NetworkPollConnectionThread);

#else
		TIMERVAL uiNextCheckConn = 0;
#endif

		Console.PrintDone();

		Console.PrintSectionBegin();

		// Shows information about IPs and ports being listened on
		Console.TurnYellow();

		auto externalIP = cwmWorldState->ServerData()->ExternalIP();
		if( externalIP != "" && externalIP != "localhost" && externalIP != "127.0.0.1" )
		{
			Console << "UOX: listening for incoming connections on External/WAN IP: " << externalIP.c_str() << myendl;
		}

		auto deviceIPs = IP4Address::deviceIPs();
		for( auto &entry : deviceIPs )
		{
			switch (entry.type())
			{
				case IP4Address::lan:
					Console << "UOX: listening for incoming connections on LAN IP: " << entry.string() << myendl;
					break;
				case IP4Address::mine:
				case IP4Address::local:
					Console << "UOX: listening for incoming connections on Local IP: " << entry.string() << myendl;
					break;
				case IP4Address::wan:
					Console << "UOX: listening for incoming connections on WAN IP: " << entry.string() << myendl;
					break;
				default:
					Console << "UOX: listening for incoming connections on IP: " << entry.string() << myendl;
					break;
			}
		}
		Console.TurnNormal();

		// we've really finished loading here
		cwmWorldState->SetLoaded( true );

		// Get a second timestamp for startup time
		auto startupEndTime = std::chrono::high_resolution_clock::now();

		// Calculate startup time in milliseconds
		auto startupDuration = std::chrono::duration_cast<std::chrono::milliseconds>( startupEndTime - startupStartTime ).count();
		Console.TurnGreen();
		Console << "UOX: Startup Completed in " << (R32)startupDuration/1000 << " seconds." << myendl;
		Console.TurnNormal();
		Console.PrintSectionBegin();
		EVENT_TIMER(stopwatch,EVENT_TIMER_OFF);
		// MAIN SYSTEM LOOP
		while( cwmWorldState->GetKeepRun() )
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(( cwmWorldState->GetPlayersOnline() ? 5 : 90 )));
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
			EVENT_TIMER_RESET(stopwatch);

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
			EVENT_TIMER_NOW(stopwatch, Complete net checkmessages,EVENT_TIMER_KEEP);
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
			//stopwatch.output("Delta for CheckTimers");
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

			if( !cwmWorldState->GetReloadingScripts() ){
				//auto stopauto = EventTimer() ;
				EVENT_TIMER(stopauto,EVENT_TIMER_OFF);
				cwmWorldState->CheckAutoTimers();
				EVENT_TIMER_NOW(stopauto,CheckAutoTimers only,EVENT_TIMER_CLEAR);
			}

			tempTime = CheckMilliTimer( tempSecs, tempMilli );
			cwmWorldState->ServerProfile()->IncAutoTime( tempTime );
			cwmWorldState->ServerProfile()->IncAutoTimeCount();
			StartMilliTimer( tempSecs, tempMilli );
			EVENT_TIMER_RESET(stopwatch);
 			Network->ClearBuffers();
			EVENT_TIMER_NOW(stopwatch,Delta for ClearBuffers,EVENT_TIMER_CLEAR);
			tempTime = CheckMilliTimer( tempSecs, tempMilli );
			cwmWorldState->ServerProfile()->IncNetworkTime( tempTime );
			tempTime = CheckMilliTimer( loopSecs, loopMilli );
			cwmWorldState->ServerProfile()->IncLoopTime( tempTime );
			EVENT_TIMER_RESET(stopwatch);
			DoMessageLoop();
			EVENT_TIMER_NOW(stopwatch,Delta for DoMessageLoop,EVENT_TIMER_CLEAR);

		}

		sysBroadcast( "The server is shutting down." );
		Console << "Closing sockets...";
		netpollthreadclose = true;
		///HERE
#ifdef __LOGIN_THREAD__
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		netw.join();
#endif
		Network->SockClose();
		Console.PrintDone();

#if PLATFORM == WINDOWS
		SetConsoleCtrlHandler( exit_handler, TRUE );
#endif
		if( cwmWorldState->GetWorldSaveProgress() != SS_SAVING )
		{
			isWorldSaving = true;
			do
			{
				cwmWorldState->SaveNewWorld( true );
			} while( cwmWorldState->GetWorldSaveProgress() == SS_SAVING );
			isWorldSaving = false;
		}
		cwmWorldState->ServerData()->save();
#if PLATFORM == WINDOWS
		SetConsoleCtrlHandler( exit_handler, false );
#endif

		Console.log( "Server Shutdown!\n=======================================================================\n" , "server.log" );

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

