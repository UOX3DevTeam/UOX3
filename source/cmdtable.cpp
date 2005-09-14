//
//	This code is an attempt to clean up the messy "if/then/else" routines
//	currently in use for GM commands, as well as adding more functionality
//	and more potential for functionality.
//	
//	Current features:
//	- Actual table of commands to execute, what perms are required, dialog
//	messages for target commands, etc handled by a central system
//	
//	
#include "uox3.h"
#include "commands.h"
#include "msgboard.h"
#include "townregion.h"
#include "cWeather.hpp"
#include "cServerDefinitions.h"
#include "wholist.h"
#include "cSpawnRegion.h"
#include "PageVector.h"
#include "speech.h"
#include "cHTMLSystem.h"
#include "gump.h"
#include "cEffects.h"
#include "classes.h"
#include "regions.h"
#include "CPacketSend.h"
#include "teffect.h"
#include "magic.h"
#include "ObjectFactory.h"

#include "Dictionary.h"

namespace UOX
{

void CollectGarbage( void );
void endmessage( int x );
void HandleGumpCommand( CSocket *s, UString cmd, UString data );
void restock( bool stockAll );
void sysBroadcast( const std::string txt );
void HandleHowTo( CSocket *sock, int cmdNumber );
void telltime( CSocket *s );
void Wiping( CSocket *s );

//o---------------------------------------------------------------------------o
//|	Function	-	void cCommands::closeCall( CSocket *s, bool isGM )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Closes an open call in the Que
//o---------------------------------------------------------------------------o
void closeCall( CSocket *s, bool isGM )
{
	CChar *mChar = s->CurrcharObj();
	if( mChar->GetCallNum() != 0 )
	{
		UI16 sysMessage = 1285;
		PageVector *tmpVector = NULL;
		if( isGM )
			tmpVector = GMQueue;
		else
		{
			tmpVector = CounselorQueue;
			++sysMessage;
		}

		if( tmpVector->GotoPos( tmpVector->FindCallNum( mChar->GetCallNum() ) ) )
		{
			tmpVector->Remove();
			mChar->SetCallNum( 0 );
			s->sysmessage( sysMessage );
		}
	}
	else
		s->sysmessage( 1287 );
}

void currentCall( CSocket *s, bool isGM )
{
	CChar *mChar = s->CurrcharObj();
	if( mChar->GetCallNum() != 0 )
	{
		PageVector *tmpVector = NULL;
		if( isGM )
			tmpVector = GMQueue;

		else
			tmpVector = CounselorQueue;

		if( tmpVector->GotoPos( tmpVector->FindCallNum( mChar->GetCallNum() ) ) )
		{
			HelpRequest *currentPage	= tmpVector->Current();
			CChar *i					= calcCharObjFromSer( currentPage->WhoPaging() );
			if( ValidateObject( i ) )
			{
				s->sysmessage( 73 );
				mChar->SetLocation( i );
			}
			currentPage->IsHandled( true );
		}
		else
			s->sysmessage( 75 );
	}
	else
		s->sysmessage( 72 );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cCommands::nextCall( CSocket *s, bool isGM )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Send GM/Counsellor to next call in the que
//o---------------------------------------------------------------------------o
void nextCall( CSocket *s, bool isGM )
{
	CChar *mChar = s->CurrcharObj();
	if( mChar->GetCallNum() != 0 )
		closeCall( s, isGM );
	if( isGM )
	{
		if( !GMQueue->AnswerNextCall( s, mChar ) )
			s->sysmessage( 347 );	
	}
	else //Player is a counselor
	{
		if( !CounselorQueue->AnswerNextCall( s, mChar ) )
			s->sysmessage( 348 );		
	}
}

bool FixSpawnFunctor( CBaseObject *a, UI32 &b, void *extraData )
{
	bool retVal = true;
	if( ValidateObject( a ) )
	{
		CItem *i = static_cast< CItem * >(a);
		ItemTypes iType = i->GetType();
		if( iType == IT_ITEMSPAWNER		|| iType == IT_NPCSPAWNER			|| iType == IT_SPAWNCONT ||
			iType == IT_LOCKEDSPAWNCONT	|| iType == IT_UNLOCKABLESPAWNCONT	||
			iType == IT_AREASPAWNER		|| iType == IT_ESCORTNPCSPAWNER )
		{
			if( i->GetObjType() != OT_SPAWNER )
			{
				CSpawnItem *spawnItem = static_cast<CSpawnItem *>(i->Dupe( OT_SPAWNER ));
				if( ValidateObject( spawnItem ) )
				{
					spawnItem->SetInterval( 0, static_cast<UI08>(i->GetTempVar( CITV_MOREY )) );
					spawnItem->SetInterval( 1, static_cast<UI08>(i->GetTempVar( CITV_MOREX )) );
					spawnItem->SetSpawnSection( i->GetDesc() );
					spawnItem->IsSectionAList( i->isSpawnerList() );
				}
				i->Delete();
			}
		}
	}
	return retVal;
}

void command_fixspawn( void )
{
	UI32 b		= 0;
	ObjectFactory::getSingleton().IterateOver( OT_ITEM, b, NULL, &FixSpawnFunctor );
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	void command_addaccount( CSocket *s)
//|	Date			-	10/17/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//|	Returns			-
//o--------------------------------------------------------------------------o
//|	Notes			-	
//o--------------------------------------------------------------------------o	
void command_addaccount( CSocket *s)
{
	VALIDATESOCKET( s );
	char szBuffer[128];
	char *szCommandLine, *szCommand, *szUsername, *szPassword, *szPrivs;
	ACCOUNTSBLOCK actbTemp;
	UI16 nFlags=0x0000;
	szPassword=szUsername=szCommand=szCommandLine=NULL;
	if( Commands->NumArguments() > 1 )
	{
		szCommandLine = (char *)Commands->CommandString( 2 ).c_str();
		szCommand =strtok(szCommandLine," ");
		szUsername=strtok(NULL," ");
		szPassword=strtok(NULL," ");
		szPrivs=strtok(NULL,"\0");
		if( szPassword==NULL||szUsername==NULL )
			return;
		if( szPrivs!=NULL )
			nFlags=atoi(szPrivs);
		// ok we need to add the account now. We will rely in the internalaccountscreation system for this
		ACCOUNTSBLOCK &actbTemp = Accounts->GetAccountByName( szUsername );
		if( actbTemp.wAccountIndex != AB_INVALID_ID )
		{
			Accounts->AddAccount(szUsername,szPassword,"NA",nFlags);
			Console << "o Account added ingame: " << szUsername << ":" << szPassword << ":" << nFlags << myendl;
			sprintf( szBuffer, "Account Added: %s:%s:%i", szUsername, szPassword, nFlags );
			s->sysmessage( szBuffer );
		}
		else
		{
			Console << "o Account was not added" << myendl;
			s->sysmessage( "Account not added" );
		}
	}
}

void command_getlight( CSocket *s )
// needs redoing to support new lighting system
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return;
	CTownRegion *tRegion	= mChar->GetRegion();
	UI08 wID				= tRegion->GetWeather();
	CWeather *sys			= Weather->Weather( wID );
	if( sys != NULL )
		s->sysmessage( 1632, sys->CurrentLight() );
	else
		s->sysmessage( 1632, cwmWorldState->ServerData()->WorldLightCurrentLevel() );
}

void command_setpost( CSocket *s )
{
	VALIDATESOCKET( s );

	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return;

	PostTypes type = PT_LOCAL;
	UString upperCommand = Commands->CommandString( 2, 2 ).upper();
	if( upperCommand == "GLOBAL" )
		type = PT_GLOBAL;
	else if( upperCommand == "REGIONAL" )
		type = PT_REGIONAL;
	else if( upperCommand == "LOCAL" )
		type = PT_LOCAL;

	mChar->SetPostType( static_cast<UI08>(type) );
	
	switch ( type )
	{
	case PT_LOCAL:				s->sysmessage( 726 );			break;
	case PT_REGIONAL:			s->sysmessage( 727 );			break;
	case PT_GLOBAL:				s->sysmessage( 728 );			break;
	default:					s->sysmessage( 725 );			break;
	}
}

void command_getpost( CSocket *s )
{
	VALIDATESOCKET( s );

	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return;

	switch( mChar->GetPostType() )
	{
	case PT_LOCAL:				s->sysmessage( 722 );			break;
	case PT_REGIONAL:			s->sysmessage( 723 );			break;
	case PT_GLOBAL:				s->sysmessage( 724 );			break;
	default:					s->sysmessage( 725 );			break;
	}
}

void command_showids( CSocket *s )
// Display the serial number of every item on your screen.
{
	VALIDATESOCKET( s );
	CChar *mChar	= s->CurrcharObj();
	CMapRegion *Cell = MapRegion->GetMapRegion( mChar );
	if( Cell == NULL )	// nothing to show
		return;
	CDataList< CChar * > *regChars = Cell->GetCharList();
	regChars->Push();
	for( CChar *toShow = regChars->First(); !regChars->Finished(); toShow = regChars->Next() )
	{
		if( !ValidateObject( toShow ) )
			continue;
		if( charInRange( mChar, toShow ) )
			s->ShowCharName( toShow, true );
	}
	regChars->Pop();
}

void command_tile( CSocket *s )
// (h) Tiles the item specified over a square area.
// To find the hexidecimal ID code for an item to tile,
// either create the item with /add or find it in the
// world, and get /ISTATS on the object to get it's ID
// code.
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() == 0 )
		return;

	UI16 targID = static_cast<UI16>(Commands->Argument( 1 ));

	if( Commands->NumArguments() == 7 )
	{
		SI16 x1 = static_cast<SI16>(Commands->Argument( 2 ));
		SI16 x2 = static_cast<SI16>(Commands->Argument( 3 ));
		SI16 y1 = static_cast<SI16>(Commands->Argument( 4 ));
		SI16 y2 = static_cast<SI16>(Commands->Argument( 5 ));
		SI08 z  = static_cast<SI08>(Commands->Argument( 6 ));

		for( SI16 x = x1; x <= x2; ++x )
		{
			for( SI16 y = y1; y <= y2; ++y )
			{
				CItem *a = Items->CreateItem( NULL, s->CurrcharObj(), targID, 1, 0, OT_ITEM );
				if( a != NULL )	// Antichrist crash prevention
					a->SetLocation( x, y, z );
			}
		}
	}
	else if( Commands->NumArguments() == 2 )
	{
		s->ClickX( -1 );
		s->ClickY( -1 );

		s->AddID1( static_cast<UI08>(targID>>8) );
		s->AddID2( static_cast<UI08>(targID%256) );
		s->target( 0, TARGET_TILING, 24 );
	}
}

void command_save( void )
// Saves the current world data into ITEMS.WSC and CHARS.WSC.
{
	if( cwmWorldState->GetWorldSaveProgress() != SS_SAVING )
		cwmWorldState->SaveNewWorld( true );
}

void command_dye( CSocket *s )
// (h h/nothing) Dyes an item a specific color, or brings up a dyeing menu if no color is specified.
{
	VALIDATESOCKET( s );
	s->DyeAll( 1 );
	if( Commands->NumArguments() == 2 )
	{
		UI16 tNum = (SI16)Commands->Argument( 1 );
		s->AddID1( static_cast<UI08>(tNum>>8) );
		s->AddID2( static_cast<UI08>(tNum%256) );
	}
	else if( Commands->NumArguments() == 3 )
	{
		s->AddID1( static_cast<UI08>(Commands->Argument( 1 ) ));
		s->AddID2( static_cast<UI08>(Commands->Argument( 2 ) ));
	}
	else
	{
		s->AddID1( 0xFF );
		s->AddID2( 0xFF );
	}
	s->target( 0, TARGET_DYE, 31 );
}

void command_settime( void )
// (d d) Sets the current UO time in hours and minutes.
{
	if( Commands->NumArguments() == 3 )
	{
		UI08 newhours	= static_cast<UI08>(Commands->Argument( 1 ));
		UI08 newminutes	= static_cast<UI08>(Commands->Argument( 2 ));
		if( ( newhours < 25 ) && ( newhours > 0 ) && ( newminutes < 60 ) )
		{
			cwmWorldState->ServerData()->ServerTimeAMPM( (newhours > 12) );
			if( newhours > 12 )
				cwmWorldState->ServerData()->ServerTimeHours( static_cast<UI08>(newhours - 12) );
			else
				cwmWorldState->ServerData()->ServerTimeHours( newhours );
			cwmWorldState->ServerData()->ServerTimeMinutes( newminutes );
		}
	}
}

void command_shutdown( void )
// (d) Shuts down the server. Argument is how many minutes until shutdown.
{
	if( Commands->NumArguments() == 2 )
	{
		cwmWorldState->SetEndTime( BuildTimeValue( static_cast<R32>(Commands->Argument( 1 )) ) );
		if( Commands->Argument( 1 ) == 0 )
		{
			cwmWorldState->SetEndTime( 0 );
			sysBroadcast( Dictionary->GetEntry( 36 ) );
		}
		else
			endmessage( 0 );
	}
}

void command_tell( CSocket *s )
// (d text) Sends an anonymous message to the user logged in under the specified slot.
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() > 2 )
	{
		CSocket *i = Network->GetSockPtr( Commands->Argument( 1 ) );
		std::string txt = Commands->CommandString( 3 );
		if( i == NULL || txt.empty() )
			return;

		CChar *mChar		= i->CurrcharObj();
		CChar *tChar		= s->CurrcharObj();
		std::string temp	= mChar->GetName() + " tells " + tChar->GetName() + ": " + txt;
		CSpeechEntry& toAdd	= SpeechSys->Add();
		toAdd.Font( (FontType)mChar->GetFontType() );
		toAdd.Speech( temp );
		toAdd.Speaker( mChar->GetSerial() );
		toAdd.SpokenTo( tChar->GetSerial() );
		toAdd.Type( TALK );
		toAdd.At( cwmWorldState->GetUICurrentTime() );
		toAdd.TargType( SPTRG_INDIVIDUAL );
		toAdd.Colour( mChar->GetSayColour() );
	}
}

void command_gmmenu( CSocket *s )
// (d) Opens the specified GM Menu.
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() == 2 )
	{
		CPIHelpRequest toHandle( s, static_cast<UI16>(Commands->Argument( 1 ) ) );
		toHandle.Handle();
	}
}

void command_command( CSocket *s )
// Executes a trigger scripting command.
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() > 1 )
		HandleGumpCommand( s, Commands->CommandString( 2, 2 ).upper(), Commands->CommandString( 3 ).upper() );
}

void command_memstats( CSocket *s )
// Display some information about the cache.
{
	VALIDATESOCKET( s );
	size_t cacheSize		= Map->TileMem + Map->MultisMem;
	size_t charsSize		= ObjectFactory::getSingleton().CountOfObjects( OT_CHAR ) * 4;
	size_t itemsSize		= ObjectFactory::getSingleton().CountOfObjects( OT_ITEM ) * 4;
	size_t spellsSize		= 69 * sizeof( SpellInfo );
	size_t teffectsSize		= sizeof( CTEffect ) * TEffects->Count();
	size_t regionsSize		= sizeof( CTownRegion ) * regions.size();
	size_t spawnregionsSize = sizeof( CSpawnRegion ) * spawnregions.size();
	size_t total			= charsSize + itemsSize + spellsSize + cacheSize + regionsSize + spawnregionsSize + teffectsSize;
	GumpDisplay cacheStats( s, 350, 345 );
	cacheStats.SetTitle( "UOX Memory Information" );
	cacheStats.AddData( "Total Memory Usage: ", total );
	cacheStats.AddData( " Cache Size: ", cacheSize );
	cacheStats.AddData( "  Tiles: ", Map->TileMem );
	cacheStats.AddData( "  Multis: ", Map->MultisMem );
	cacheStats.AddData( " Characters Size: ", charsSize );
	cacheStats.AddData( "  Actual Characters: ", ObjectFactory::getSingleton().CountOfObjects( OT_CHAR ) );
	cacheStats.AddData( "  Allocated Memory: ", ObjectFactory::getSingleton().SizeOfObjects( OT_CHAR ) );
	cacheStats.AddData( "  CChar: ", sizeof( CChar ) );
	cacheStats.AddData( " Items Size: ", itemsSize );
	cacheStats.AddData( "  Actual Items: ", ObjectFactory::getSingleton().CountOfObjects( OT_ITEM ) );
	cacheStats.AddData( "  Allocated Memory: ", ObjectFactory::getSingleton().SizeOfObjects( OT_ITEM ) );
	cacheStats.AddData( "  CItem: ", sizeof( CItem ) );
	cacheStats.AddData( " Spells Size: ", spellsSize );
	cacheStats.AddData( " TEffects Size: ", teffectsSize );
	cacheStats.AddData( "  TEffects: ", TEffects->Count() );
	cacheStats.AddData( "  TEffect: ", sizeof( CTEffect ) );
	cacheStats.AddData( " Regions Size: ", regionsSize );
	cacheStats.AddData( "  Regions: ", regions.size() );
	cacheStats.AddData( "  CTownRegion: ", sizeof( CTownRegion ) );
	cacheStats.AddData( " SpawnRegions Size ", spawnregionsSize );
	cacheStats.AddData( "  SpawnRegions: ", spawnregions.size() );
	cacheStats.AddData( "  CSpawnRegion: ", sizeof( CSpawnRegion ) );
	cacheStats.Send( 0, false, INVALIDSERIAL );

}

void command_restock( CSocket *s )
// Forces a manual vendor restock.
{
	VALIDATESOCKET( s );
	if( Commands->CommandString( 2, 2 ).upper() == "ALL" )
	{
		restock( true );
		s->sysmessage( 55 );
	}
	else
	{
		restock( false );
		s->sysmessage( 54 );
	}
}

void command_setshoprestockrate( CSocket *s )
// (d) Sets the universe's shop restock rate.
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() == 2 )
	{
		cwmWorldState->ServerData()->SystemTimer( tSERVER_SHOPSPAWN, static_cast<UI16>(Commands->Argument( 1 )) );
		s->sysmessage( 56 );
	}
	else
		s->sysmessage( 57 );
}

bool RespawnFunctor( CBaseObject *a, UI32 &b, void *extraData )
{
	bool retVal = true;
	if( ValidateObject( a ) )
	{
		CItem *i = static_cast< CItem * >(a);
		ItemTypes iType = i->GetType();
		if( iType == IT_ITEMSPAWNER		|| iType == IT_NPCSPAWNER			|| iType == IT_SPAWNCONT ||
			iType == IT_LOCKEDSPAWNCONT	|| iType == IT_UNLOCKABLESPAWNCONT	||
			iType == IT_AREASPAWNER		|| iType == IT_ESCORTNPCSPAWNER )
		{
			if( i->GetObjType() == OT_SPAWNER )
			{
				CSpawnItem *spawnItem = static_cast<CSpawnItem *>(i);
				if( !spawnItem->DoRespawn() )
					spawnItem->SetGateTime( BuildTimeValue( static_cast<R32>(RandomNum( spawnItem->GetInterval( 0 ) * 60, spawnItem->GetInterval( 1 ) * 60 ) )) );
			}
			else
				i->SetType( IT_NOTYPE );
		}
	}
	return retVal;
}

void command_respawn( void )
// Forces a respawn.
{
	UI16 spawnedItems	= 0;
	UI16 spawnedNpcs	= 0;
	std::vector< CSpawnRegion * >::const_iterator spawnCounter;
	for( spawnCounter = spawnregions.begin(); spawnCounter != spawnregions.end(); ++spawnCounter )
	{
		CSpawnRegion *spawnReg = (*spawnCounter);
		if( spawnReg != NULL )
			spawnReg->doRegionSpawn( spawnedItems, spawnedNpcs );
	}

	UI32 b		= 0;
	ObjectFactory::getSingleton().IterateOver( OT_ITEM, b, NULL, &RespawnFunctor );
}

// (s/d) Forces a region spawn, First argument is region number or "ALL"
void command_regspawn( CSocket *s )
{
	VALIDATESOCKET( s );

	if( Commands->NumArguments() == 2 )
	{
		UI16 itemsSpawned	= 0;
		UI16 npcsSpawned	= 0;

		if( Commands->CommandString( 2, 2 ).upper() == "ALL" )
		{
			std::vector< CSpawnRegion * >::const_iterator spawnCounter;
			for( spawnCounter = spawnregions.begin(); spawnCounter != spawnregions.end(); ++ spawnCounter )
			{
				CSpawnRegion *spawnReg = (*spawnCounter);
				if( spawnReg != NULL )
					spawnReg->doRegionSpawn( itemsSpawned, npcsSpawned );
			}
			if( itemsSpawned || npcsSpawned )
				s->sysmessage( "Spawned %u items and %u npcs in %u SpawnRegions", itemsSpawned, npcsSpawned, spawnregions.size() );
			else
				s->sysmessage( "Failed to spawn any new items or npcs in %u SpawnRegions", spawnregions.size() );
		}
		else
		{
			UI16 spawnRegNum = static_cast<UI16>(Commands->Argument( 1 ));
			if( spawnRegNum < spawnregions.size() )
			{
				CSpawnRegion *spawnReg = spawnregions[spawnRegNum];
				if( spawnReg != NULL )
				{
					spawnReg->doRegionSpawn( itemsSpawned, npcsSpawned );
					if( itemsSpawned || npcsSpawned )
						s->sysmessage( "Region: '%s'(%u) spawned %u items and %u npcs", spawnReg->GetName().c_str(), spawnRegNum, itemsSpawned, npcsSpawned );
					else
						s->sysmessage( "Region: '%s'(%u) failed to spawn any new items or npcs", spawnReg->GetName().c_str(), spawnRegNum );
					return;
				}
			}
			s->sysmessage( "Invalid SpawnRegion %u", spawnRegNum );
		}
	}
}

void command_loaddefaults( void )
// Loads the server defaults.
{
	cwmWorldState->ServerData()->ResetDefaults();
}

void command_cq( CSocket *s )
// Display the counselor queue.
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() == 2 )
	{
		UString upperCommand = Commands->CommandString( 2, 2 ).upper();
		if( upperCommand == "NEXT" )
			nextCall( s, false );
		else if( upperCommand == "CLEAR" )
			closeCall( s, false );
		else if( upperCommand == "CURR" )
			currentCall( s, false );
		else if( upperCommand == "TRANSFER" )
		{
			CChar *mChar = s->CurrcharObj();
			if( mChar->GetCallNum() != 0 )
			{
				if( CounselorQueue->GotoPos( CounselorQueue->FindCallNum( mChar->GetCallNum() ) ) )
				{
					HelpRequest pageToAdd;
					HelpRequest *currentPage = NULL;
					currentPage = CounselorQueue->Current();
					pageToAdd.Reason( currentPage->Reason() );
					pageToAdd.WhoPaging( currentPage->WhoPaging() );
					pageToAdd.IsHandled( false );
					pageToAdd.TimeOfPage( time( NULL ) );
					GMQueue->Add( &pageToAdd );
					s->sysmessage( 74 );
					closeCall( s, false );
				}
				else
					s->sysmessage( 75 );
			}
			else
				s->sysmessage( 72 );
		}
	}
	else
		CounselorQueue->SendAsGump( s );	// Show the Counselor queue, not GM queue
}

void command_gq( CSocket *s )
// Display the GM queue.
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() == 2 )
	{
		UString upperCommand = Commands->CommandString( 2, 2 ).upper();
		if( upperCommand == "NEXT" )
			nextCall( s, true );
		else if( upperCommand == "CLEAR" )
			closeCall( s, true );
		else if( upperCommand == "CURR" )
			currentCall( s, true );
	}
	else
		GMQueue->SendAsGump( s );
}

void command_minecheck( void )
// (d) Set the server mine check interval in minutes.
{
	if( Commands->NumArguments() == 2 )
		cwmWorldState->ServerData()->MineCheck( static_cast<UI08>(Commands->Argument( 1 )) );
}

void command_guards( void )
// Activates town guards.
{
	if( Commands->CommandString( 2, 2 ).upper() == "ON" )
	{
		cwmWorldState->ServerData()->GuardStatus( true );
		sysBroadcast( Dictionary->GetEntry( 61 ) );
	}
	else if( Commands->CommandString( 2, 2 ).upper() == "OFF" )
	{
		cwmWorldState->ServerData()->GuardStatus( false );
		sysBroadcast( Dictionary->GetEntry( 62 ) );
	}
}

void command_announce( void )
// Enable announcement of world saves.
{
	if( Commands->CommandString( 2, 2 ).upper() == "ON" )
	{
		cwmWorldState->ServerData()->ServerAnnounceSaves( true );
		sysBroadcast( Dictionary->GetEntry( 63 ) );
	}
	else if( Commands->CommandString( 2, 2 ).upper() == "OFF" )
	{
		cwmWorldState->ServerData()->ServerAnnounceSaves( false );
		sysBroadcast( Dictionary->GetEntry( 64 ) );
	}
}

void command_pdump( CSocket *s )
// Display some performance information.
{
	VALIDATESOCKET( s );
	UI32 networkTimeCount	= cwmWorldState->ServerProfile()->NetworkTimeCount();
	UI32 timerTimeCount		= cwmWorldState->ServerProfile()->TimerTimeCount();
	UI32 autoTimeCount		= cwmWorldState->ServerProfile()->AutoTimeCount();
	UI32 loopTimeCount		= cwmWorldState->ServerProfile()->LoopTimeCount();

	s->sysmessage( "Performace Dump:" );
	s->sysmessage( "Network code: %fmsec [%i]", (R32)((R32)cwmWorldState->ServerProfile()->NetworkTime()/(R32)networkTimeCount), networkTimeCount );
	s->sysmessage( "Timer code: %fmsec [%i]", (R32)((R32)cwmWorldState->ServerProfile()->TimerTime()/(R32)timerTimeCount), timerTimeCount );
	s->sysmessage( "Auto code: %fmsec [%i]", (R32)((R32)cwmWorldState->ServerProfile()->AutoTime()/(R32)autoTimeCount), autoTimeCount );
	s->sysmessage( "Loop Time: %fmsec [%i]", (R32)((R32)cwmWorldState->ServerProfile()->LoopTime()/(R32)loopTimeCount), loopTimeCount );
	s->sysmessage( "Simulation Cycles/Sec: %f", (1000.0*(1.0/(R32)((R32)cwmWorldState->ServerProfile()->LoopTime()/(R32)loopTimeCount ) ) ) );
}

void command_spawnkill( CSocket *s )
// (d) Kills spawns from the specified spawn region in SPAWN.SCP.
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() == 2 )
	{
		UI16 regNum = static_cast<UI16>(Commands->Argument( 1 ));
		if( regNum >= spawnregions.size() )
			return;
		CSpawnRegion *spawnReg = spawnregions[regNum];
		if( spawnReg == NULL )
			return;
		int killed	= 0;

		s->sysmessage( 349 );
		CDataList< CChar * > *spCharList = spawnReg->GetSpawnedCharsList();
		for( CChar *i = spCharList->First(); !spCharList->Finished(); i = spCharList->Next() )
		{
			if( !ValidateObject( i ) )
				continue;
			if( i->isSpawned() )
			{
				Effects->bolteffect( i );
				Effects->PlaySound( i, 0x0029 );
				i->Delete();
				++killed;
			}
		}
		s->sysmessage( "Done." );
		s->sysmessage( 350, killed, regNum );
	}
}

void BuildWhoGump( CSocket *s, UI08 commandLevel, std::string title )
{
	size_t j = 0;
	char temp[512];

	GumpDisplay Who( s, 400, 300 );
	Who.SetTitle( title );

	Network->PushConn();
	for( CSocket *iSock = Network->FirstSocket(); !Network->FinishedSockets(); iSock = Network->NextSocket() )
	{
		CChar *iChar = iSock->CurrcharObj();
		if( iChar->GetCommandLevel() >= commandLevel )
		{
			sprintf( temp, "%i) %s", j, iChar->GetName().c_str() );
			Who.AddData( temp, iChar->GetSerial(), 3 );
		}
		++j;
	}
	Network->PopConn();
	Who.Send( 4, false, INVALIDSERIAL );
}
void command_who( CSocket *s )
// Displays a list of users currently online.
{
	VALIDATESOCKET( s );
	BuildWhoGump( s, 0, "Who's Online" );
}

void command_gms( CSocket *s )
{
	VALIDATESOCKET( s );
	BuildWhoGump( s, CNS_CMDLEVEL, Dictionary->GetEntry( 77, s->Language() ) );
}

void command_reportbug( CSocket *s )
// DESC:	Writes out a bug to the bug file
// DATE:	9th February, 2000
// CODER:	Abaddon
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return;
	std::string logName = cwmWorldState->ServerData()->Directory( CSDDP_LOGS ) + "bugs.log";
	std::ofstream logDestination;
	logDestination.open( logName.c_str(), std::ios::out | std::ios::app );
	if( !logDestination.is_open() )
	{
		Console.Error( 1, "Unable to open bugs log file %s!", logName.c_str() );
		return;
	}
	char dateTime[1024];
	RealTime( dateTime );

	logDestination << "[" << dateTime << "] ";
	logDestination << "<" << mChar->GetName() << "> ";
	logDestination << "Reports: " << Commands->CommandString( 2 ) << std::endl;
	logDestination.close();

	s->sysmessage( 87 );
	bool x = false;
	Network->PushConn();
	for( CSocket *iSock = Network->FirstSocket(); !Network->FinishedSockets(); iSock = Network->NextSocket() )
	{
		CChar *iChar = iSock->CurrcharObj();
		if( !ValidateObject( iChar ) )
			continue;
		if( iChar->IsGMPageable() )
		{
			x = true;
			iSock->sysmessage( 277, mChar->GetName().c_str(), Commands->CommandString( 2 ).c_str() );
		}
	}
	Network->PopConn();
	if( x )
		s->sysmessage( 88 );
	else
		s->sysmessage( 89 );
}

void command_forcewho( CSocket *s )
// Brings up an interactive listing of online users.
{
	VALIDATESOCKET( s );
	WhoList->ZeroWho();
	WhoList->SendSocket( s );
}

void command_validcmd( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	UI08 targetCommand = mChar->GetCommandLevel();
	GumpDisplay targetCmds( s, 300, 300 );
	targetCmds.SetTitle( "Valid Commands" );

	for( COMMANDMAP_ITERATOR myCounter = CommandMap.begin(); myCounter != CommandMap.end(); ++myCounter )
	{
		if( myCounter->second.cmdLevelReq <= targetCommand )
			targetCmds.AddData( myCounter->first, myCounter->second.cmdLevelReq );
	}
	for( TARGETMAP_ITERATOR targCounter = TargetMap.begin(); targCounter != TargetMap.end(); ++targCounter )
	{
		if( targCounter->second.cmdLevelReq <= targetCommand )
			targetCmds.AddData( targCounter->first, targCounter->second.cmdLevelReq );
	}
	for( JSCOMMANDMAP_ITERATOR jsCounter = JSCommandMap.begin(); jsCounter != JSCommandMap.end(); ++jsCounter )
	{
		if( jsCounter->second.cmdLevelReq <= targetCommand )
			targetCmds.AddData( jsCounter->first, jsCounter->second.cmdLevelReq );
	}

	targetCmds.Send( 4, false, INVALIDSERIAL );
}

void command_howto( CSocket *s )
{
	VALIDATESOCKET( s );
	UString commandStart = Commands->CommandString( 2 ).upper();
	if( commandStart.empty() )
	{
		CChar *mChar = s->CurrcharObj();
		if( !ValidateObject( mChar ) )
			return;

		int iCmd = 2;
		int numAdded = 0;
		UI08 pagenum = 1;
		UI16 position = 40;
		UI16 linenum = 1;

		CPSendGumpMenu toSend;
		toSend.UserID( INVALIDSERIAL );
		toSend.GumpID( 13 );

		toSend.AddCommand( "noclose" );
		toSend.AddCommand( "resizepic 0 0 %u 480 320", cwmWorldState->ServerData()->BackgroundPic() );
		toSend.AddCommand( "page 0" );
		toSend.AddCommand( "text 200 20 0 0" );
		toSend.AddText( "HOWTO" );
		toSend.AddCommand( "button 440 20 %u %i 1 0 1", cwmWorldState->ServerData()->ButtonCancel(), cwmWorldState->ServerData()->ButtonCancel() + 1 );

		UI08 currentLevel			= mChar->GetCommandLevel();
		COMMANDMAP_ITERATOR gAdd	= CommandMap.begin();
		TARGETMAP_ITERATOR tAdd		= TargetMap.begin();

		toSend.AddCommand( "page 1" );

		bool justDonePageAdd = false;
		while( gAdd != CommandMap.end() )
		{
			if( numAdded > 0 && !(numAdded%10) && !justDonePageAdd )
			{
				position = 40;
				++pagenum;
				toSend.AddCommand( "page %u", pagenum );
				justDonePageAdd = true;
			}
			if( gAdd->second.cmdLevelReq <= currentLevel )
			{
				justDonePageAdd = false;
				toSend.AddCommand( "text 50 %u %u %u", position, cwmWorldState->ServerData()->LeftTextColour(), linenum ); 
				toSend.AddCommand( "button 20 %u %u %i %u 0 %i", position, cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1, pagenum, iCmd );
				toSend.AddText( gAdd->first );
				++numAdded;
				++linenum;
				position += 20;
			}
			++iCmd;
			++gAdd;
		}
		while( tAdd != TargetMap.end() )
		{
			if( numAdded > 0 && !(numAdded%10) && !justDonePageAdd )
			{
				position = 40;
				++pagenum;
				toSend.AddCommand( "page %u", pagenum );
				justDonePageAdd = true;
			}
			if( tAdd->second.cmdLevelReq <= currentLevel )
			{
				justDonePageAdd = false;
				toSend.AddCommand( "text 50 %u %u %u", position, cwmWorldState->ServerData()->LeftTextColour(), linenum ); 
				toSend.AddCommand( "button 20 %u %u %i %u 0 %i", position, cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1, pagenum, iCmd );
				toSend.AddText( tAdd->first );
				++numAdded;
				++linenum;
				position += 20;
			}
			++iCmd;
			++tAdd;
		}
		pagenum = 1; 
		for( int i = 0; i < numAdded; i += 10 )
		{
			toSend.AddCommand( "page %u", pagenum );
			if( i >= 10 )
			{
				toSend.AddCommand( "button 30 290 %u %i 0 %i", cwmWorldState->ServerData()->ButtonLeft(), cwmWorldState->ServerData()->ButtonLeft() + 1, pagenum - 1 ); //back button
			}
			if( ( numAdded > 10 ) && ( ( i + 10 ) < numAdded ) )
			{
				toSend.AddCommand( "button 440 290 %u %i 0 %i", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1, pagenum + 1 );
			}
			++pagenum;
		}
		toSend.Finalize();
		s->Send( &toSend );
	}
	else
	{
		int i = 0;
		COMMANDMAP_ITERATOR toFind;
		for( toFind = CommandMap.begin(); toFind != CommandMap.end(); ++toFind )
		{
			if( commandStart == toFind->first )
				break;
			++i;
		}
		if( toFind == CommandMap.end() )
		{
			TARGETMAP_ITERATOR findTarg;
			for( findTarg = TargetMap.begin(); findTarg != TargetMap.end(); ++findTarg )
			{
				if( commandStart == findTarg->first )
					break;
				++i;
			}
			if( findTarg == TargetMap.end() )
				s->sysmessage( 280 );
		}
		else
			HandleHowTo( s, i );
	}
}


void command_temp( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return;

	CTownRegion *reg	= mChar->GetRegion();
	weathID toGrab		= reg->GetWeather();
	if( toGrab != 0xFF )
	{
		R32 curTemp = Weather->Temp( toGrab );
		s->sysmessage( 1751, curTemp );
	}
}

void command_status( CSocket *s )
// Opens the HTML status information gump
{
	VALIDATESOCKET( s );
	HTMLTemplates->TemplateInfoGump( s );
}

COMMANDMAP		CommandMap;
TARGETMAP		TargetMap;
JSCOMMANDMAP	JSCommandMap;

void cCommands::CommandReset( void )
{
	// TargetMap[Command Name] = TargetMapEntry(Required Command Level, Command Type, Target ID, Dictionary Entry);
	// A
	// B
	// C
	// D
	// E
	// F
	// G
	// H
	// I
	TargetMap["INFO"]			= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGET,		TARGET_INFO,			261);
	// J
	// K
	// L
	// M
	TargetMap["MAKE"]			= TargetMapEntry( ADMIN_CMDLEVEL,	CMD_TARGETTXT,	TARGET_MAKESTATUS,		279);
	TargetMap["MAKESHOP"]		= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGET,		TARGET_MAKESHOP,		232);
	// N
	// O
	// P
	// Q
	// R
	// S
	TargetMap["SETSCPTRIG"]		= TargetMapEntry( ADMIN_CMDLEVEL,	CMD_TARGETINT,	TARGET_SETSCPTRIG,		267);
	TargetMap["SHOWSKILLS"]		= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGETINT,	TARGET_SHOWSKILLS,		260);
	// T
	TargetMap["TWEAK"]			= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGET,		TARGET_TWEAK,			229);
	// U
	// V
	// W
	TargetMap["WSTATS"]			= TargetMapEntry( CNS_CMDLEVEL,		CMD_TARGET,		TARGET_WSTATS,			183);
	// X
	// Y
	// Z

	// CommandMap[Command Name] = CommandMapEntry(Required Command Level, Command Type, Command Function);
	//A
	CommandMap["ADDACCOUNT"]		= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_addaccount);
	CommandMap["ANNOUNCE"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_FUNC,		(CMD_DEFINE)&command_announce);
	//B
	//C
	CommandMap["CQ"]				= CommandMapEntry( CNS_CMDLEVEL,	CMD_SOCKFUNC,	(CMD_DEFINE)&command_cq);
	CommandMap["COMMAND"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_command);
	//D
	CommandMap["DYE"]				= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_dye);
	//E
	//F
	CommandMap["FORCEWHO"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_forcewho);
	CommandMap["FIXSPAWN"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_FUNC,		(CMD_DEFINE)&command_fixspawn);
	//G,
	CommandMap["GETLIGHT"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_getlight);
	CommandMap["GUARDS"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_FUNC,		(CMD_DEFINE)&command_guards);
	CommandMap["GMS"]				= CommandMapEntry( CNS_CMDLEVEL,	CMD_SOCKFUNC,	(CMD_DEFINE)&command_gms);
	CommandMap["GMMENU"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_gmmenu);
	CommandMap["GCOLLECT"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_FUNC,		(CMD_DEFINE)&CollectGarbage);
	CommandMap["GQ"]				= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_gq);
	//H
	CommandMap["HOWTO"]				= CommandMapEntry( PLAYER_CMDLEVEL,	CMD_SOCKFUNC,	(CMD_DEFINE)&command_howto );
	//I
	//J
	//K
	//L
	CommandMap["LOADDEFAULTS"]		= CommandMapEntry( ADMIN_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_loaddefaults);
	//M
	CommandMap["MEMSTATS"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_memstats);
	CommandMap["MINECHECK"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_FUNC,		(CMD_DEFINE)&command_minecheck);
	//N
	//O
	//P
	CommandMap["PDUMP"]				= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_pdump);
	CommandMap["POST"]    			= CommandMapEntry( CNS_CMDLEVEL,	CMD_SOCKFUNC,	(CMD_DEFINE)&command_getpost);
	//Q
	//R
	CommandMap["RESTOCK"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_restock);
	CommandMap["RESPAWN"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_FUNC,		(CMD_DEFINE)&command_respawn);
	CommandMap["REGSPAWN"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_regspawn);
	CommandMap["REPORTBUG"]			= CommandMapEntry( PLAYER_CMDLEVEL,	CMD_SOCKFUNC,	(CMD_DEFINE)&command_reportbug);
	//S
	CommandMap["SETPOST"]			= CommandMapEntry( CNS_CMDLEVEL,	CMD_SOCKFUNC,	(CMD_DEFINE)&command_setpost);
	CommandMap["SPAWNKILL"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_spawnkill);
	CommandMap["SETSHOPRESTOCKRATE"]= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_setshoprestockrate);
	CommandMap["SETTIME"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_FUNC,		(CMD_DEFINE)&command_settime);
	CommandMap["SHUTDOWN"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_FUNC,		(CMD_DEFINE)&command_shutdown);
	CommandMap["SAVE"]				= CommandMapEntry( GM_CMDLEVEL,		CMD_FUNC,		(CMD_DEFINE)&command_save);
	CommandMap["STATUS"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_status);
	CommandMap["SHOWIDS"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_showids);
	//T
	CommandMap["TEMP"]				= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_temp );
	CommandMap["TIME"]				= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&telltime);
	CommandMap["TELL"]				= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_tell);
	CommandMap["TILE"]				= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_tile);
	//U
	//V
	CommandMap["VALIDCMD"]			= CommandMapEntry( PLAYER_CMDLEVEL,	CMD_SOCKFUNC,	(CMD_DEFINE)&command_validcmd );
	//W
	CommandMap["WHO"]				= CommandMapEntry( CNS_CMDLEVEL,	CMD_SOCKFUNC,	(CMD_DEFINE)&command_who);
	//X
	//Y
	//Z
}

void cCommands::UnRegister( std::string cmdName, cScript *toRegister )
{
#if defined( UOX_DEBUG_MODE )
	Console.Print( "   UnRegistering command %s\n", cmdName.c_str());
#endif
	UString upper		= cmdName;
	upper				= upper.upper();
	JSCOMMANDMAP_ITERATOR p = JSCommandMap.find( upper );
	if( p != JSCommandMap.end() )
		JSCommandMap.erase( p );
#if defined( UOX_DEBUG_MODE )
	else
		Console.Print( "         Command \"%s\" was not found.\n", cmdName.c_str());
#endif
}

void cCommands::Register( std::string cmdName, UI16 scriptID, UI08 cmdLevel, bool isEnabled )
{
#if defined( UOX_DEBUG_MODE )
	Console.Print( "         Registering \"%s\" @ command level %i\n", cmdName.c_str(), cmdLevel );
#endif
	UString upper		= cmdName;
	upper				= upper.upper();
	JSCommandMap[upper]	= JSCommandEntry( cmdLevel, scriptID, isEnabled );
}

void cCommands::SetCommandStatus( std::string cmdName, bool isEnabled )
{
	UString upper					= cmdName;
	upper							= upper.upper();
	JSCOMMANDMAP_ITERATOR	toFind	= JSCommandMap.find( upper );
	if( toFind != JSCommandMap.end() )
	{
		toFind->second.isEnabled = isEnabled;
	}
}

}
