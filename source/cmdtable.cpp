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
#include "cRaces.h"
#include "StringUtility.hpp"

#include "Dictionary.h"

void CollectGarbage( void );
void EndMessage( SI32 x );
void HandleGumpCommand( CSocket *s, std::string cmd, std::string data );
void Restock( bool stockAll );
void SysBroadcast( const std::string& txt );
void HandleHowTo( CSocket *sock, SI32 cmdNumber );

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CloseCall()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Closes an open call in the Que
//o------------------------------------------------------------------------------------------------o
void CloseCall( CSocket *s, bool isGM )
{
	CChar *mChar = s->CurrcharObj();
	if( mChar->GetCallNum() != 0 )
	{
		UI16 sysMessage = 1285; // Call removed from the GM queue.
		PageVector *tmpVector = nullptr;
		if( isGM )
		{
			tmpVector = GMQueue;
		}
		else
		{
			tmpVector = CounselorQueue;
			++sysMessage;
		}

		if( tmpVector->GotoPos( tmpVector->FindCallNum( mChar->GetCallNum() )))
		{
			tmpVector->Remove();
			mChar->SetCallNum( 0 );
			s->SysMessage( sysMessage );
		}
	}
	else
	{
		s->SysMessage( 1287 ); // You are currently not on a call.
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CurrentCall()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Send GM/Counselor to the current call in the queue
//o------------------------------------------------------------------------------------------------o
void CurrentCall( CSocket *s, bool isGM )
{
	CChar *mChar = s->CurrcharObj();
	if( mChar->GetCallNum() != 0 )
	{
		PageVector *tmpVector = nullptr;
		if( isGM )
		{
			tmpVector = GMQueue;
		}
		else
		{
			tmpVector = CounselorQueue;
		}

		if( tmpVector->GotoPos( tmpVector->FindCallNum( mChar->GetCallNum() )))
		{
			CHelpRequest *currentPage	= tmpVector->Current();
			CChar *i					= CalcCharObjFromSer( currentPage->WhoPaging() );
			if( ValidateObject( i ))
			{
				s->SysMessage( 73 ); // Transporting to your current call.
				mChar->SetLocation( i );
			}
			currentPage->IsHandled( true );
		}
		else
		{
			s->SysMessage( 75 ); // No such call exists
		}
	}
	else
	{
		s->SysMessage( 72 ); // You are not currently on a call.
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	NextCall()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Send GM/Counsellor to next call in the que
//o------------------------------------------------------------------------------------------------o
void NextCall( CSocket *s, bool isGM )
{
	CChar *mChar = s->CurrcharObj();
	if( mChar->GetCallNum() != 0 )
	{
		CloseCall( s, isGM );
	}
	if( isGM )
	{
		if( !GMQueue->AnswerNextCall( s, mChar ))
		{
			s->SysMessage( 347 ); // The GM queue is currently empty.
		}
	}
	else //Player is a counselor
	{
		if( !CounselorQueue->AnswerNextCall( s, mChar ))
		{
			s->SysMessage( 348 ); // The Counselor queue is currently empty.
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	FixSpawnFunctor()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Replaces legacy spawner objects that have incorrect item type
//o------------------------------------------------------------------------------------------------o
bool FixSpawnFunctor( CBaseObject *a, [[maybe_unused]] UI32 &b, [[maybe_unused]] void *extraData )
{
	bool retVal = true;
	if( ValidateObject( a ))
	{
		CItem *i = static_cast<CItem *>( a );
		ItemTypes iType = i->GetType();
		if( iType == IT_ITEMSPAWNER		|| iType == IT_NPCSPAWNER			|| iType == IT_SPAWNCONT ||
		   iType == IT_LOCKEDSPAWNCONT	|| iType == IT_UNLOCKABLESPAWNCONT	||
		   iType == IT_AREASPAWNER		|| iType == IT_ESCORTNPCSPAWNER )
		{
			if( i->GetObjType() != OT_SPAWNER )
			{
				CSpawnItem *spawnItem = static_cast<CSpawnItem *>(i->Dupe( OT_SPAWNER ));
				if( ValidateObject( spawnItem ))
				{
					spawnItem->SetInterval( 0, static_cast<UI08>( i->GetTempVar( CITV_MOREY )));
					spawnItem->SetInterval( 1, static_cast<UI08>( i->GetTempVar( CITV_MOREX )));
					spawnItem->SetSpawnSection( i->GetDesc() );
					spawnItem->IsSectionAList( i->IsSpawnerList() );
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
	ObjectFactory::GetSingleton().IterateOver( OT_ITEM, b, nullptr, &FixSpawnFunctor );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_AddAccount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	GM command for adding new user accounts while in-game
//o------------------------------------------------------------------------------------------------o
void Command_AddAccount( CSocket *s)
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() > 1 )
	{
		std::string newUsername = Commands->CommandString( 2, 2 );
		std::string newPassword = Commands->CommandString( 3, 3 );
		UI16 newFlags = 0x0000;

		if( Commands->NumArguments() > 2 )
		{
			newFlags = oldstrutil::value<UI16>( Commands->CommandString( 4, 4 ));
		}

		if( newUsername == "" || newPassword == "" )
		{
			s->SysMessage( 9018 ); // Unable to add account, insufficient data provided (syntax: [username] [password] [flags])
			return;
		}

		// ok we need to add the account now. We will rely in the internalaccountscreation system for this
		CAccountBlock_st &actbTemp = Accounts->GetAccountByName( newUsername );
		if( actbTemp.wAccountIndex == AB_INVALID_ID )
		{
			Accounts->AddAccount( newUsername, newPassword, "NA", newFlags );
			Console << "o Account added ingame: " << newUsername << ":" << newPassword << ":" << newFlags << myendl;
			s->SysMessage( 9019, newUsername.c_str(), newPassword.c_str(), newFlags ); // Account Added: %s:%s:%i
		}
		else
		{
			Console << "o Account was not added, an account with that username already exists!" << myendl;
			s->SysMessage( 9020 ); // Account not added, an account with that username already exists!
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_GetLight()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Displays current in-game light level as a system message
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Needs redoing to support new (weather-based) lighting system
//o------------------------------------------------------------------------------------------------o
void Command_GetLight( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ))
		return;

	CTownRegion *tRegion	= mChar->GetRegion();
	UI16 weatherId			= tRegion->GetWeather();
	CWeather *sys			= Weather->Weather( weatherId );
	if( sys != nullptr )
	{
		const R32 lightMin = sys->LightMin();
		const R32 lightMax = sys->LightMax();
		if( lightMin < 300 && lightMax < 300 )
		{
			R32 i = sys->CurrentLight();
			if( Races->VisLevel( mChar->GetRace() ) > i )
			{
				s->SysMessage( 1632, 0 ); // Current light level is %i
			}
			else
			{
				s->SysMessage( 1632, static_cast<LIGHTLEVEL>(RoundNumber( i - Races->VisLevel( mChar->GetRace() )))); // Current light level is %i
			}
		}
		else
		{
			s->SysMessage( 1632, cwmWorldState->ServerData()->WorldLightCurrentLevel() ); // Current light level is %i
		}
	}
	else
	{
		s->SysMessage( 1632, cwmWorldState->ServerData()->WorldLightCurrentLevel() ); // Current light level is %i
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_SetPost()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets bulletin board posting mode for user
//o------------------------------------------------------------------------------------------------o
void Command_SetPost( CSocket *s )
{
	VALIDATESOCKET( s );

	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ))
		return;

	PostTypes type = PT_LOCAL;
	std::string upperCommand = oldstrutil::upper( Commands->CommandString( 2, 2 ));
	if( upperCommand == "GLOBAL" ) // user's next post appears in ALL bulletin boards
	{
		type = PT_GLOBAL;
	}
	else if( upperCommand == "REGIONAL" ) // user's next post appears in all bulletin boards in current region
	{
		type = PT_REGIONAL;
	}
	else if( upperCommand == "LOCAL" ) // user's next post appears only locally in the next bulletin board used
	{
		type = PT_LOCAL;
	}

	mChar->SetPostType( static_cast<UI08>( type ));

	switch ( type )
	{
		case PT_LOCAL:				s->SysMessage( 726 );			break; // Post type set to LOCAL.
		case PT_REGIONAL:			s->SysMessage( 727 );			break; // Post type set to REGION
		case PT_GLOBAL:				s->SysMessage( 728 );			break; // Post type set to GLOBAL.
		default:					s->SysMessage( 725 );			break; // Invalid post type.
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_GetPost()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns current bulletin board posting mode for user
//o------------------------------------------------------------------------------------------------o
void Command_GetPost( CSocket *s )
{
	VALIDATESOCKET( s );

	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ))
		return;

	switch( mChar->GetPostType() )
	{
		case PT_LOCAL:				s->SysMessage( 722 );			break; // Currently posting LOCAL messages.
		case PT_REGIONAL:			s->SysMessage( 723 );			break; // Currently posting REGIONAL messages.
		case PT_GLOBAL:				s->SysMessage( 724 );			break; // Currently posting GLOBAL messages.
		default:					s->SysMessage( 725 );			break; // Invalid post type.
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_ShowIds()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Display the serial number of every item on user's screen
//o------------------------------------------------------------------------------------------------o
auto Command_ShowIds( CSocket *s ) -> void
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	CMapRegion *Cell = MapRegion->GetMapRegion( mChar );

	if( Cell )
	{
		auto regChars = Cell->GetCharList();
		for( auto &toShow : regChars->collection() )
		{
			if( ValidateObject( toShow ))
			{
				if( CharInRange( mChar, toShow ))
				{
					s->ShowCharName( toShow, true );
				}
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_Tile()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	(h) Tiles the item specified over a square area.
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	To find the hexidecimal ID code for an item to tile,
//|					either create the item with /add or find it in the
//|					world, and get /ISTATS on the object to get its ID code.
//o------------------------------------------------------------------------------------------------o
void Command_Tile( CSocket *s )
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() == 0 )
		return;

	UI16 targId = 0;
	if( oldstrutil::upper( Commands->CommandString( 2, 2 )) == "STATIC" )
	{
		targId = static_cast<UI16>( Commands->Argument( 2 ));
	}
	else
	{
		targId = static_cast<UI16>( Commands->Argument( 1 ));
	}

	// Reset tempint2 on socket
	s->TempInt2( 0 );

	if( Commands->NumArguments() == 7 || Commands->NumArguments() == 8 )
	{
		// tile itemId x1 y1 x2 y2 z rndVal
		SI32 rndVal = 0;
		UI16 rndId = 0;

		if( Commands->NumArguments() == 8 )
		{
			rndVal = static_cast<SI32>( Commands->Argument( 7 ));
		}

		// tile itemId x1 y1 x2 y2 z
		SI16 x1 = static_cast<SI16>( Commands->Argument( 2 ));
		SI16 x2 = static_cast<SI16>( Commands->Argument( 3 ));
		SI16 y1 = static_cast<SI16>( Commands->Argument( 4 ));
		SI16 y2 = static_cast<SI16>( Commands->Argument( 5 ));
		SI08 z  = static_cast<SI08>( Commands->Argument( 6 ));

		for( SI16 x = x1; x <= x2; ++x )
		{
			for( SI16 y = y1; y <= y2; ++y )
			{
				rndId = targId + RandomNum( static_cast<UI16>( 0 ), static_cast<UI16>( rndVal ));
				CItem *a = Items->CreateItem( nullptr, s->CurrcharObj(), rndId, 1, 0, OT_ITEM );
				if( ValidateObject( a ))	// crash prevention
				{
					a->SetLocation( x, y, z );
					a->SetDecayable( false );
				}
			}
		}
	}
	else if( Commands->NumArguments() == 4 )
	{
		// tile static itemId rndVal
		s->ClickX( -1 );
		s->ClickY( -1 );
		s->TempInt2( static_cast<SI32>( Commands->Argument( 3 )));

		s->AddId1( static_cast<UI08>( targId >> 8 ));
		s->AddId2( static_cast<UI08>( targId % 256 ));
		s->SendTargetCursor( 0, TARGET_TILING, 0, 24 );
	}
	else if( Commands->NumArguments() == 3 )
	{
		// tile itemId rndVal
		// tile static itemId
		s->ClickX( -1 );
		s->ClickY( -1 );
		if( oldstrutil::upper( Commands->CommandString( 2, 2 )) != "STATIC" )
		{
			s->TempInt2( static_cast<SI32>( Commands->Argument( 2 )));
		}

		s->AddId1( static_cast<UI08>( targId >> 8 ));
		s->AddId2( static_cast<UI08>( targId % 256 ));
		s->SendTargetCursor( 0, TARGET_TILING, 0, 24 );
	}
	else if( Commands->NumArguments() == 2 )
	{
		// tile itemId
		s->ClickX( -1 );
		s->ClickY( -1 );

		s->AddId1( static_cast<UI08>( targId >> 8 ));
		s->AddId2( static_cast<UI08>( targId % 256 ));
		s->SendTargetCursor( 0, TARGET_TILING, 0, 24 );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_Save()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Saves the current world data into .WSC files
//o------------------------------------------------------------------------------------------------o
void Command_Save( void )
{
	if( cwmWorldState->GetWorldSaveProgress() != SS_SAVING )
	{
		cwmWorldState->SaveNewWorld( true );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_Dye()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	(h h/nothing) Dyes an item a specific color, or brings up
//|					a dyeing menu if no color is specified.
//o------------------------------------------------------------------------------------------------o
void Command_Dye( CSocket *s )
{
	VALIDATESOCKET( s );
	s->DyeAll( 1 );
	if( Commands->NumArguments() == 2 )
	{
		UI16 tNum = static_cast<SI16>( Commands->Argument( 1 ));
		s->AddId1( static_cast<UI08>( tNum >> 8 ));
		s->AddId2( static_cast<UI08>( tNum % 256 ));
	}
	else if( Commands->NumArguments() == 3 )
	{
		s->AddId1( static_cast<UI08>( Commands->Argument( 1 )));
		s->AddId2( static_cast<UI08>( Commands->Argument( 2 )));
	}
	else
	{
		s->AddId1( 0xFF );
		s->AddId2( 0xFF );
	}
	s->SendTargetCursor( 0, TARGET_DYE, 0, 31 );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_SetTime()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	(d d) Sets the current UO time in hours and minutes.
//o------------------------------------------------------------------------------------------------o
void Command_SetTime( void )
{
	if( Commands->NumArguments() == 3 )
	{
		UI08 newhours	= static_cast<UI08>( Commands->Argument( 1 ));
		UI08 newminutes	= static_cast<UI08>( Commands->Argument( 2 ));
		if(( newhours < 25 ) && ( newhours > 0 ) && ( newminutes < 60 ))
		{
			cwmWorldState->ServerData()->ServerTimeAMPM(( newhours > 12 ));
			if( newhours > 12 )
			{
				cwmWorldState->ServerData()->ServerTimeHours( static_cast<UI08>( newhours - 12 ));
			}
			else
			{
				cwmWorldState->ServerData()->ServerTimeHours( newhours );
			}
			cwmWorldState->ServerData()->ServerTimeMinutes( newminutes );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_Shutdown()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	(d) Shuts down the server. Argument is how many minutes
//|					until shutdown.
//o------------------------------------------------------------------------------------------------o
void Command_Shutdown( void )
{
	if( Commands->NumArguments() == 2 )
	{
		cwmWorldState->SetEndTime( BuildTimeValue( static_cast<R32>( Commands->Argument( 1 ))));
		if( Commands->Argument( 1 ) == 0 )
		{
			cwmWorldState->SetEndTime( 0 );
			SysBroadcast( Dictionary->GetEntry( 36 ));
		}
		else
		{
			EndMessage( 0 );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_Tell()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	(d text) Sends an anonymous message to the user logged in under the specified slot.
//o------------------------------------------------------------------------------------------------o
void Command_Tell( CSocket *s )
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() > 2 )
	{
		CSocket *i = Network->GetSockPtr( Commands->Argument( 1 ));
		std::string txt = Commands->CommandString( 3 );
		if( i == nullptr || txt.empty() )
			return;

		CChar *mChar		= i->CurrcharObj();
		CChar *tChar		= s->CurrcharObj();
		std::string temp	= mChar->GetNameRequest( tChar ) + " tells " + tChar->GetNameRequest( mChar ) + ": " + txt;

		if( cwmWorldState->ServerData()->UseUnicodeMessages() )
		{
			CPUnicodeMessage unicodeMessage;
			unicodeMessage.Message( temp );
			unicodeMessage.Font( static_cast<FontType>( mChar->GetFontType() ));
			if( mChar->GetSayColour() == 0x1700 )
			{
				unicodeMessage.Colour( 0x5A );
			}
			else if( mChar->GetSayColour() == 0x0 )
			{
				unicodeMessage.Colour( 0x5A );
			}
			else
			{
				unicodeMessage.Colour( mChar->GetSayColour() );
			}
			unicodeMessage.Type( TALK );
			unicodeMessage.Language( "ENG" );
			unicodeMessage.Name( mChar->GetNameRequest( tChar ));
			unicodeMessage.ID( INVALIDID );
			unicodeMessage.Serial( mChar->GetSerial() );
			s->Send( &unicodeMessage );
		}
		else
		{
			CSpeechEntry& toAdd	= SpeechSys->Add();
			toAdd.Font( static_cast<FontType>( mChar->GetFontType() ));
			toAdd.Speech( temp );
			toAdd.Speaker( mChar->GetSerial() );
			toAdd.SpokenTo( tChar->GetSerial() );
			toAdd.Type( TALK );
			toAdd.At( cwmWorldState->GetUICurrentTime() );
			toAdd.TargType( SPTRG_INDIVIDUAL );
			if( mChar->GetSayColour() == 0x1700 )
			{
				toAdd.Colour( 0x5A );
			}
			else if( mChar->GetSayColour() == 0x0 )
			{
				toAdd.Colour( 0x5A );
			}
			else
			{
				toAdd.Colour( mChar->GetSayColour() );
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_GmMenu()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	(d) Opens the specified GM Menu from dfndata/menus.dfn
//o------------------------------------------------------------------------------------------------o
void Command_GmMenu( CSocket *s )
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() == 2 )
	{
		CPIHelpRequest toHandle( s, static_cast<UI16>( Commands->Argument( 1 )));
		toHandle.Handle();
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_Command()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Executes a trigger scripting command.
//o------------------------------------------------------------------------------------------------o
void Command_Command( CSocket *s )
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() > 1 )
	{
		HandleGumpCommand( s, oldstrutil::upper( Commands->CommandString( 2, 2 )), oldstrutil::upper( Commands->CommandString( 3 )));
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_MemStats()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Display some information about cache and memory usage.
//o------------------------------------------------------------------------------------------------o
void Command_MemStats( CSocket *s )
{
	VALIDATESOCKET( s );
	size_t charsSize		= ObjectFactory::GetSingleton().SizeOfObjects( OT_CHAR );
	size_t itemsSize		= ObjectFactory::GetSingleton().SizeOfObjects( OT_ITEM );
	size_t spellsSize		= 69 * sizeof( CSpellInfo );
	size_t teffectsSize		= sizeof( CTEffect ) * cwmWorldState->tempEffects.Num();
	size_t regionsSize		= sizeof( CTownRegion ) * cwmWorldState->townRegions.size();
	size_t spawnregionsSize = sizeof( CSpawnRegion ) * cwmWorldState->spawnRegions.size();
	size_t total			= charsSize + itemsSize + spellsSize + regionsSize + spawnregionsSize + teffectsSize;
	CGumpDisplay cacheStats( s, 350, 345 );
	cacheStats.SetTitle( "UOX Memory Information (sizes in bytes)" );
	cacheStats.AddData( "Total Memory Usage: ", static_cast<UI32>( total ));
	cacheStats.AddData( " Characters: ", ObjectFactory::GetSingleton().CountOfObjects( OT_CHAR ));
	cacheStats.AddData( "  Allocated Memory: ", static_cast<UI32>( charsSize ));
	cacheStats.AddData( "  CChar: ", sizeof( CChar ));
	cacheStats.AddData( " Items: ", ObjectFactory::GetSingleton().CountOfObjects( OT_ITEM ));
	cacheStats.AddData( "  Allocated Memory: ", static_cast<UI32>( itemsSize ));
	cacheStats.AddData( "  CItem: ", sizeof( CItem ));
	cacheStats.AddData( " Spells Size: ", static_cast<UI32>( spellsSize ));
	cacheStats.AddData( " TEffects: ", static_cast<UI32>( cwmWorldState->tempEffects.Num() ));
	cacheStats.AddData( "  Allocated Memory: ", static_cast<UI32>( teffectsSize ));
	cacheStats.AddData( "  TEffect: ", sizeof( CTEffect ));
	cacheStats.AddData( " Regions Size: ", static_cast<UI32>( cwmWorldState->townRegions.size() ));
	cacheStats.AddData( "  Allocated Memory: ", static_cast<UI32>( regionsSize ));
	cacheStats.AddData( "  CTownRegion: ", sizeof( CTownRegion ));
	cacheStats.AddData( " SpawnRegions ", static_cast<UI32>( cwmWorldState->spawnRegions.size() ));
	cacheStats.AddData( "  Allocated Memory: ", static_cast<UI32>( spawnregionsSize ));
	cacheStats.AddData( "  CSpawnRegion: ", sizeof( CSpawnRegion ));
	cacheStats.Send( 0, false, INVALIDSERIAL );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_Restock()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	(nothing / s) Forces a manual vendor restock
//o------------------------------------------------------------------------------------------------o
void Command_Restock( CSocket *s )
{
	VALIDATESOCKET( s );
	if( oldstrutil::upper( Commands->CommandString( 2, 2 )) == "ALL" )
	{
		Restock( true );
		s->SysMessage( 55 ); // Restocking all shops to their maximums
	}
	else
	{
		Restock( false );
		s->SysMessage( 54 ); // Manual shop restock has occurred.
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_SetShopRestockRate()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	(d) Sets the universe's shop restock rate
//o------------------------------------------------------------------------------------------------o
void Command_SetShopRestockRate( CSocket *s )
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() == 2 )
	{
		cwmWorldState->ServerData()->SystemTimer( tSERVER_SHOPSPAWN, static_cast<UI16>( Commands->Argument( 1 )));
		s->SysMessage( 56 ); // NPC shop restock rate changed.
	}
	else
		s->SysMessage( 57 ); // Invalid number of parameters.
}

bool RespawnFunctor( CBaseObject *a, [[maybe_unused]] UI32 &b, [[maybe_unused]] void *extraData )
{
	bool retVal = true;
	if( ValidateObject( a ))
	{
		CItem *i = static_cast<CItem *>( a );
		ItemTypes iType = i->GetType();
		if( iType == IT_ITEMSPAWNER		|| iType == IT_NPCSPAWNER			|| iType == IT_SPAWNCONT ||
		   iType == IT_LOCKEDSPAWNCONT	|| iType == IT_UNLOCKABLESPAWNCONT	||
		   iType == IT_AREASPAWNER		|| iType == IT_ESCORTNPCSPAWNER )
		{
			if( i->GetObjType() == OT_SPAWNER )
			{
				CSpawnItem *spawnItem = static_cast<CSpawnItem *>( i );
				if( !spawnItem->DoRespawn() )
				{
					spawnItem->SetTempTimer( BuildTimeValue( static_cast<R32>( RandomNum( spawnItem->GetInterval( 0 ) * 60, spawnItem->GetInterval( 1 ) * 60 ))));
				}
			}
			else
			{
				i->SetType( IT_NOTYPE );
			}
		}
	}
	return retVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_Respawn()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Forces a respawn of all spawn regions
//o------------------------------------------------------------------------------------------------o
void Command_Respawn( void )
{
	UI32 spawnedItems		= 0;
	UI32 spawnedNpcs		= 0;
	std::for_each( cwmWorldState->spawnRegions.begin(), cwmWorldState->spawnRegions.end(), [&spawnedItems, &spawnedNpcs]( std::pair<UI16, CSpawnRegion *> entry )
	{
		if( entry.second )
		{
			entry.second->DoRegionSpawn( spawnedItems, spawnedNpcs );
		}
	});

	UI32 b		= 0;
	ObjectFactory::GetSingleton().IterateOver( OT_ITEM, b, nullptr, &RespawnFunctor );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_RegSpawn()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	(s/d) Forces a region spawn, First argument is region number or "ALL"
//o------------------------------------------------------------------------------------------------o
void Command_RegSpawn( CSocket *s )
{
	VALIDATESOCKET( s );

	if( Commands->NumArguments() == 2 )
	{
		UI32 itemsSpawned	= 0;
		UI32 npcsSpawned	= 0;

		if( oldstrutil::upper( Commands->CommandString( 2, 2 )) == "ALL" )
		{
			std::for_each( cwmWorldState->spawnRegions.begin(), cwmWorldState->spawnRegions.end(), [&itemsSpawned, &npcsSpawned]( std::pair<UI16, CSpawnRegion *> entry )
			{
				if( entry.second )
				{
					entry.second->DoRegionSpawn( itemsSpawned, npcsSpawned );
				}
			});
			if( itemsSpawned || npcsSpawned )
			{
				s->SysMessage( 9021, itemsSpawned, npcsSpawned, cwmWorldState->spawnRegions.size() ); // Spawned %u items and %u npcs in %u SpawnRegions
			}
			else
			{
				s->SysMessage( 9022, cwmWorldState->spawnRegions.size() ); // Failed to spawn any new items or npcs in %u SpawnRegions
			}
		}
		else
		{
			UI16 spawnRegNum = static_cast<UI16>( Commands->Argument( 1 ));
			if( cwmWorldState->spawnRegions.find( spawnRegNum ) != cwmWorldState->spawnRegions.end() )
			{
				CSpawnRegion *spawnReg = cwmWorldState->spawnRegions[spawnRegNum];
				if( spawnReg != nullptr )
				{
					spawnReg->DoRegionSpawn( itemsSpawned, npcsSpawned );
					if( itemsSpawned || npcsSpawned )
					{
						s->SysMessage( 9023, spawnReg->GetName().c_str(), spawnRegNum, itemsSpawned, npcsSpawned ); // Region: '%s'(%u) spawned %u items and %u npcs
					}
					else
					{
						s->SysMessage( 9024, spawnReg->GetName().c_str(), spawnRegNum ); // Region: '%s'(%u) failed to spawn any new items or npcs
					}
					return;
				}
			}
			s->SysMessage( 9025, spawnRegNum ); // Invalid SpawnRegion %u
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_LoadDefaults()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads the hardcoded server defaults for various settings in uox.ini
//o------------------------------------------------------------------------------------------------o
void Command_LoadDefaults( void )
{
	cwmWorldState->ServerData()->ResetDefaults();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_CQ()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Display the Counselor queue when used with no arguments
//|					Can trigger other queue-related commands when arguments are provided
//o------------------------------------------------------------------------------------------------o
void Command_CQ( CSocket *s )
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() == 2 )
	{
		std::string upperCommand = oldstrutil::upper( Commands->CommandString( 2, 2 ));
		if( upperCommand == "NEXT" ) // Go to next call in Counselor queue
		{
			NextCall( s, false );
		}
		else if( upperCommand == "CLEAR" ) // Close and clear current call as resolved
		{
			CloseCall( s, false );
		}
		else if( upperCommand == "CURR" ) // Take Counselor to current call they are on
		{
			CurrentCall( s, false );
		}
		else if( upperCommand == "TRANSFER" ) // Transfer call from Counselor queue to GM queue
		{
			CChar *mChar = s->CurrcharObj();
			if( mChar->GetCallNum() != 0 )
			{
				if( CounselorQueue->GotoPos( CounselorQueue->FindCallNum( mChar->GetCallNum() )))
				{
					CHelpRequest pageToAdd;
					CHelpRequest *currentPage = nullptr;
					currentPage = CounselorQueue->Current();
					pageToAdd.Reason( currentPage->Reason() );
					pageToAdd.WhoPaging( currentPage->WhoPaging() );
					pageToAdd.IsHandled( false );
					pageToAdd.TimeOfPage( time( nullptr ));
					GMQueue->Add( &pageToAdd );
					s->SysMessage( 74 ); // Call successfully transferred to the GM queue.
					CloseCall( s, false );
				}
				else
				{
					s->SysMessage( 75 ); // No such call exists
				}
			}
			else
			{
				s->SysMessage( 72 ); // You are not currently on a call.
			}
		}
	}
	else
	{
		CounselorQueue->SendAsGump( s );	// Show the Counselor queue, not GM queue
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_GQ()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Display the GM queue when used with no arguments
//|					Can trigger other queue-related commands when arguments are provided
//o------------------------------------------------------------------------------------------------o
void Command_GQ( CSocket *s )
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() == 2 )
	{
		std::string upperCommand = oldstrutil::upper( Commands->CommandString( 2, 2 ));
		if( upperCommand == "NEXT" ) // Go to next call in GM queue
		{
			NextCall( s, true );
		}
		else if( upperCommand == "CLEAR" ) // Close and clear current call as resolved
		{
			CloseCall( s, true );
		}
		else if( upperCommand == "CURR" ) // Take GM to current call they are on
		{
			CurrentCall( s, true );
		}
	}
	else
	{
		GMQueue->SendAsGump( s );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_MineCheck()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	(d) Set the server mine check status to determine where mining will work
//|						0 - allow mining everywhere
//|						1 - mountainsides and cave floors
//|						2 - mining regions only
//o------------------------------------------------------------------------------------------------o
void Command_MineCheck( void )
{
	if( Commands->NumArguments() == 2 )
	{
		cwmWorldState->ServerData()->MineCheck( static_cast<UI08>( Commands->Argument( 1 )));
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_Guards()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Enables (ON) or disables (OFF) town guards globally
//o------------------------------------------------------------------------------------------------o
void Command_Guards( void )
{
	if( oldstrutil::upper( Commands->CommandString( 2, 2 )) == "ON" )
	{
		cwmWorldState->ServerData()->GuardStatus( true );
		SysBroadcast( Dictionary->GetEntry( 61 )); // Guards have been reactivated.
	}
	else if( oldstrutil::upper( Commands->CommandString( 2, 2 )) == "OFF" )
	{
		cwmWorldState->ServerData()->GuardStatus( false );
		SysBroadcast( Dictionary->GetEntry( 62 )); // Warning: Guards have been deactivated globally.
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_Announce()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Enables (ON) or disables (OFF) announcement of world saves
//o------------------------------------------------------------------------------------------------o
void Command_Announce( void )
{
	if( oldstrutil::upper( Commands->CommandString( 2, 2 )) == "ON" )
	{
		cwmWorldState->ServerData()->ServerAnnounceSaves( true );
		SysBroadcast( Dictionary->GetEntry( 63 )); // WorldStat Saves will be displayed.
	}
	else if( oldstrutil::upper( Commands->CommandString( 2, 2 )) == "OFF" )
	{
		cwmWorldState->ServerData()->ServerAnnounceSaves( false );
		SysBroadcast( Dictionary->GetEntry( 64 )); // WorldStat Saves will not be displayed.
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_PDump()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Display some performance information
//o------------------------------------------------------------------------------------------------o
void Command_PDump( CSocket *s )
{
	VALIDATESOCKET( s );
	UI32 networkTimeCount	= cwmWorldState->ServerProfile()->NetworkTimeCount();
	UI32 timerTimeCount		= cwmWorldState->ServerProfile()->TimerTimeCount();
	UI32 autoTimeCount		= cwmWorldState->ServerProfile()->AutoTimeCount();
	UI32 loopTimeCount		= cwmWorldState->ServerProfile()->LoopTimeCount();

	s->SysMessage( "Performance Dump:" );
	s->SysMessage( "Network code: %fmsec [%i]", static_cast<R32>( static_cast<R32>( cwmWorldState->ServerProfile()->NetworkTime() ) / static_cast<R32>( networkTimeCount )), networkTimeCount );
	s->SysMessage( "Timer code: %fmsec [%i]", static_cast<R32>( static_cast<R32>( cwmWorldState->ServerProfile()->TimerTime() ) / static_cast<R32>( timerTimeCount )), timerTimeCount );
	s->SysMessage( "Auto code: %fmsec [%i]", static_cast<R32>( static_cast<R32>( cwmWorldState->ServerProfile()->AutoTime() ) / static_cast<R32>( autoTimeCount )), autoTimeCount );
	s->SysMessage( "Loop Time: %fmsec [%i]", static_cast<R32>( static_cast<R32>( cwmWorldState->ServerProfile()->LoopTime() ) / static_cast<R32>( loopTimeCount )), loopTimeCount );
	s->SysMessage( "Simulation Cycles/Sec: %f", ( 1000.0 * ( 1.0 / static_cast<R32>( static_cast<R32>( cwmWorldState->ServerProfile()->LoopTime() ) / static_cast<R32>( loopTimeCount )))));
}
//o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_SpawnKill()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	(d) Kills spawns from the specified spawn region in SPAWN.DFN
//o------------------------------------------------------------------------------------------------o
auto Command_SpawnKill( CSocket *s ) -> void
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() == 2 )
	{
		UI16 regNum = static_cast<UI16>( Commands->Argument( 1 ));
		if( cwmWorldState->spawnRegions.find( regNum ) != cwmWorldState->spawnRegions.end() )
		{
			auto spawnReg = cwmWorldState->spawnRegions[regNum];
			if( spawnReg )
			{
				SI32 killed	= 0;
				
				s->SysMessage( 349 ); // Killing spawn, this may cause lag...
				std::vector<CChar *> spCharsToDelete;
				auto spCharList = spawnReg->GetSpawnedCharsList();
				for( auto &spChar : spCharList->collection() )
				{
					if( ValidateObject( spChar ))
					{
						if( spChar->IsSpawned() )
						{
							// Store reference to character we want to delete
							spCharsToDelete.push_back( spChar );
						}
					}
				}

				// Loop through the characters we want to delete from spawn region
				std::for_each( spCharsToDelete.begin(), spCharsToDelete.end(), [&killed]( CChar *charToDelete )
				{
					Effects->Bolteffect( charToDelete );
					Effects->PlaySound( charToDelete, 0x0029 );
					charToDelete->Delete();
					++killed;
				});

				s->SysMessage( 84 ); // Done.
				s->SysMessage( 350, killed, regNum ); // %i of Spawn %i have been killed.
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	BuildWhoGump()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Build and send a gump with a list of players based on provided criteria
//o------------------------------------------------------------------------------------------------o
void BuildWhoGump( CSocket *s, UI08 commandLevel, std::string title )
{
	UI16 j = 0;

	CGumpDisplay Who( s, 400, 300 );
	Who.SetTitle( title );
	{
		for( auto &iSock : Network->connClients )
		{
			CChar *iChar = iSock->CurrcharObj();
			if( iChar->GetCommandLevel() >= commandLevel )
			{
				auto temp = oldstrutil::format( "%i) %s", j, iChar->GetName().c_str() );
				Who.AddData( temp, iChar->GetSerial(), 3 );
			}
			++j;
		}
	}
	Who.Send( 4, false, INVALIDSERIAL );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_Who()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Displays a list of users currently online
//o------------------------------------------------------------------------------------------------o
void Command_Who( CSocket *s )
{
	VALIDATESOCKET( s );
	BuildWhoGump( s, 0, "Who's Online" );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	void Command_GMs( CSocket *s )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Displays a list of server staff currently online
//o------------------------------------------------------------------------------------------------o
void Command_GMs( CSocket *s )
{
	VALIDATESOCKET( s );
	BuildWhoGump( s, CL_CNS, Dictionary->GetEntry( 77, s->Language() )); // Current Staff online
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_ReportBug()
//| Date		-	9th February, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Writes out a bug reported by player to the bug file
//o------------------------------------------------------------------------------------------------o
void Command_ReportBug( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ))
		return;

	std::string logName = cwmWorldState->ServerData()->Directory( CSDDP_LOGS ) + "bugs.log";
	std::ofstream logDestination;
	logDestination.open( logName.c_str(), std::ios::out | std::ios::app );
	if( !logDestination.is_open() )
	{
		Console.Error( oldstrutil::format( "Unable to open bugs log file %s!", logName.c_str() ));
		return;
	}
	char dateTime[1024];
	RealTime( dateTime );

	logDestination << "[" << dateTime << "] ";
	logDestination << "<" << mChar->GetName() << "> ";
	logDestination << "Reports: " << Commands->CommandString( 2 ) << std::endl;
	logDestination.close();

	s->SysMessage( 87 ); // Thank you for your continuing support, your feedback is important to us
	bool x = false;
	{
		for( auto &iSock : Network->connClients )
		{
			CChar *iChar = iSock->CurrcharObj();
			if( ValidateObject( iChar ))
			{
				if( iChar->IsGMPageable() )
				{
					x = true;
					iSock->SysMessage( 277, mChar->GetName().c_str(), Commands->CommandString( 2 ).c_str() ); // User %s reported a bug (%s)
				}
			}
		}
	}
	if( x )
	{
		s->SysMessage( 88 ); // Available Game Masters have been notified of your bug submission.
	}
	else
	{
		s->SysMessage( 89 ); // There was no Game Master available to note your bug report.
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_ForceWho()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Brings up an interactive listing of online users.
//o------------------------------------------------------------------------------------------------o
void Command_ForceWho( CSocket *s )
{
	VALIDATESOCKET( s );
	WhoList->ZeroWho();
	WhoList->SendSocket( s );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_ValidCmd()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Displays all valid commands for the user's character
//o------------------------------------------------------------------------------------------------o
void Command_ValidCmd( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	UI08 targetCommand = mChar->GetCommandLevel();
	CGumpDisplay targetCmds( s, 300, 300 );
	targetCmds.SetTitle( "Valid Commands" );

	for( COMMANDMAP_ITERATOR myCounter = CommandMap.begin(); myCounter != CommandMap.end(); ++myCounter )
	{
		if( myCounter->second.cmdLevelReq <= targetCommand )
		{
			targetCmds.AddData( myCounter->first, myCounter->second.cmdLevelReq );
		}
	}
	for( TARGETMAP_ITERATOR targCounter = TargetMap.begin(); targCounter != TargetMap.end(); ++targCounter )
	{
		if( targCounter->second.cmdLevelReq <= targetCommand )
		{
			targetCmds.AddData( targCounter->first, targCounter->second.cmdLevelReq );
		}
	}
	for( JSCOMMANDMAP_ITERATOR jsCounter = JSCommandMap.begin(); jsCounter != JSCommandMap.end(); ++jsCounter )
	{
		if( jsCounter->second.cmdLevelReq <= targetCommand )
		{
			targetCmds.AddData( jsCounter->first, jsCounter->second.cmdLevelReq );
		}
	}

	targetCmds.Send( 4, false, INVALIDSERIAL );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_HowTo()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens a list of commands, with explanations of how to use each specific command
//o------------------------------------------------------------------------------------------------o
void Command_HowTo( CSocket *s )
{
	VALIDATESOCKET( s );
	std::string commandStart = oldstrutil::upper( Commands->CommandString( 2 ));
	if( commandStart.empty() )
	{
		CChar *mChar = s->CurrcharObj();
		if( !ValidateObject( mChar ))
			return;

		SI32 iCmd = 2;
		SI32 numAdded = 0;
		UI08 pagenum = 1;
		UI16 position = 40;
		UI16 linenum = 1;

		CPSendGumpMenu toSend;
		toSend.UserId( INVALIDSERIAL );
		toSend.GumpId( 13 );

		toSend.addCommand( oldstrutil::format( "resizepic 0 0 %u 480 320", cwmWorldState->ServerData()->BackgroundPic() ));
		toSend.addCommand( "page 0" );
		toSend.addCommand( "text 200 20 0 0" );
		toSend.addText( "HOWTO" );
		toSend.addCommand( oldstrutil::format( "button 440 20 %u %i 1 0 1", cwmWorldState->ServerData()->ButtonCancel(), cwmWorldState->ServerData()->ButtonCancel() + 1 ));

		UI08 currentLevel			= mChar->GetCommandLevel();
		COMMANDMAP_ITERATOR gAdd	= CommandMap.begin();
		TARGETMAP_ITERATOR tAdd		= TargetMap.begin();
		JSCOMMANDMAP_ITERATOR jAdd	= JSCommandMap.begin();

		toSend.addCommand( "page 1" );

		bool justDonePageAdd = false;
		while( gAdd != CommandMap.end() )
		{
			if( numAdded > 0 && !(numAdded%10) && !justDonePageAdd )
			{
				position = 40;
				++pagenum;
				toSend.addCommand( oldstrutil::format( "page %u", pagenum ));
				justDonePageAdd = true;
			}
			if( gAdd->second.cmdLevelReq <= currentLevel )
			{
				justDonePageAdd = false;
				toSend.addCommand( oldstrutil::format( "text 50 %u %u %u", position, cwmWorldState->ServerData()->LeftTextColour(), linenum ));
				toSend.addCommand( oldstrutil::format("button 20 %u %u %i %u 0 %i", position, cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1, pagenum, iCmd ));
				toSend.addText( gAdd->first );
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
				toSend.addCommand( oldstrutil::format( "page %u", pagenum ));
				justDonePageAdd = true;
			}
			if( tAdd->second.cmdLevelReq <= currentLevel )
			{
				justDonePageAdd = false;
				toSend.addCommand( oldstrutil::format( "text 50 %u %u %u", position, cwmWorldState->ServerData()->LeftTextColour(), linenum ));
				toSend.addCommand( oldstrutil::format( "button 20 %u %u %i %u 0 %i", position, cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1, pagenum, iCmd ));
				toSend.addText( tAdd->first );
				++numAdded;
				++linenum;
				position += 20;
			}
			++iCmd;
			++tAdd;
		}
		while( jAdd != JSCommandMap.end() )
		{
			if( numAdded > 0 && !(numAdded%10) && !justDonePageAdd )
			{
				position = 40;
				++pagenum;
				toSend.addCommand( oldstrutil::format( "page %u", pagenum ));
				justDonePageAdd = true;
			}
			if( jAdd->second.cmdLevelReq <= currentLevel )
			{
				justDonePageAdd = false;
				toSend.addCommand( oldstrutil::format( "text 50 %u %u %u", position, cwmWorldState->ServerData()->LeftTextColour(), linenum ));
				toSend.addCommand( oldstrutil::format( "button 20 %u %u %i %u 0 %i", position, cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1, pagenum, iCmd ));
				toSend.addText( jAdd->first );
				++numAdded;
				++linenum;
				position += 20;
			}
			++iCmd;
			++jAdd;
		}
		pagenum = 1;
		for( SI32 i = 0; i < numAdded; i += 10 )
		{
			toSend.addCommand( oldstrutil::format( "page %u", pagenum ));
			if( i >= 10 )
			{
				toSend.addCommand( oldstrutil::format( "button 30 290 %u %i 0 %i", cwmWorldState->ServerData()->ButtonLeft(), cwmWorldState->ServerData()->ButtonLeft() + 1, pagenum - 1 )); //back button
			}
			if(( numAdded > 10 ) && (( i + 10 ) < numAdded ))
			{
				toSend.addCommand( oldstrutil::format("button 440 290 %u %i 0 %i", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1, pagenum + 1 ));
			}
			++pagenum;
		}
		toSend.Finalize();
		s->Send( &toSend );
	}
	else
	{
		SI32 i = 0;
		COMMANDMAP_ITERATOR toFind;
		for( toFind = CommandMap.begin(); toFind != CommandMap.end(); ++toFind )
		{
			if( commandStart == toFind->first )
			{
				break;
			}
			++i;
		}
		if( toFind == CommandMap.end() )
		{
			TARGETMAP_ITERATOR findTarg;
			for( findTarg = TargetMap.begin(); findTarg != TargetMap.end(); ++findTarg )
			{
				if( commandStart == findTarg->first )
				{
					break;
				}
				++i;
			}
			if( findTarg == TargetMap.end() )
			{
				JSCOMMANDMAP_ITERATOR findJS = JSCommandMap.begin();
				for( findJS = JSCommandMap.begin(); findJS != JSCommandMap.end(); ++findJS )
				{
					if( commandStart == findJS->first )
					{
						break;
					}
					++i;
				}
				if( findJS == JSCommandMap.end() )
				{
					s->SysMessage( 280 ); // Error finding command
					return;
				}
			}
		}
		HandleHowTo( s, i );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_Temp()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Displays the current temperature in user's current townregion
//o------------------------------------------------------------------------------------------------o
void Command_Temp( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ))
		return;

	CTownRegion *reg	= mChar->GetRegion();
	WEATHID toGrab		= reg->GetWeather();
	if( toGrab != 0xFF )
	{
		R32 curTemp = Weather->Temp( toGrab );
		s->SysMessage( 1751, curTemp ); // It is currently %f degrees
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Command_Status()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens the HTML status information gump
//o------------------------------------------------------------------------------------------------o
void Command_Status( CSocket *s )
{
	VALIDATESOCKET( s );
	HTMLTemplates->TemplateInfoGump( s );
}

COMMANDMAP		CommandMap;
TARGETMAP		TargetMap;
JSCOMMANDMAP	JSCommandMap;

void CCommands::CommandReset()
{
	// TargetMap[Command Name] = TargetMapEntry_st(Required Command Level, Command Type, Target ID, Dictionary Entry);
	// A
	// B
	// C
	// D
	// E
	// F
	// G
	// H
	// I
	TargetMap["INFO"]			= TargetMapEntry_st( CL_GM,			CMD_TARGET,		TARGET_INFO,			261);
	// J
	// K
	// L
	// M
	TargetMap["MAKE"]			= TargetMapEntry_st( CL_ADMIN,			CMD_TARGETTXT,	TARGET_MAKESTATUS,		279);
	// N
	// O
	// P
	// Q
	// R
	// S
	TargetMap["SHOWSKILLS"]		= TargetMapEntry_st( CL_GM,			CMD_TARGETINT,	TARGET_SHOWSKILLS,		260);
	// T
	//TargetMap["TWEAK"]			= TargetMapEntry_st( CL_GM,			CMD_TARGET,		TARGET_TWEAK,			229);
	// U
	// V
	// W
	TargetMap["WSTATS"]			= TargetMapEntry_st( CL_CNS,			CMD_TARGET,		TARGET_WSTATS,			183);
	// X
	// Y
	// Z

	// CommandMap[Command Name] = CommandMapEntry_st(Required Command Level, Command Type, Command Function);
	//A
	CommandMap["ADDACCOUNT"]		= CommandMapEntry_st( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&Command_AddAccount);
	CommandMap["ANNOUNCE"]			= CommandMapEntry_st( CL_GM,		CMD_FUNC,		(CMD_DEFINE)&Command_Announce);
	//B
	//C
	CommandMap["CQ"]				= CommandMapEntry_st( CL_CNS,		CMD_SOCKFUNC,	(CMD_DEFINE)&Command_CQ);
	CommandMap["COMMAND"]			= CommandMapEntry_st( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&Command_Command);
	//D
	CommandMap["DYE"]				= CommandMapEntry_st( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&Command_Dye);
	//E
	//F
	CommandMap["FORCEWHO"]			= CommandMapEntry_st( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&Command_ForceWho);
	CommandMap["FIXSPAWN"]			= CommandMapEntry_st( CL_GM,		CMD_FUNC,		(CMD_DEFINE)&command_fixspawn);
	//G,
	CommandMap["GETLIGHT"]			= CommandMapEntry_st( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&Command_GetLight);
	CommandMap["GUARDS"]			= CommandMapEntry_st( CL_GM,		CMD_FUNC,		(CMD_DEFINE)&Command_Guards);
	CommandMap["GMS"]				= CommandMapEntry_st( CL_CNS,		CMD_SOCKFUNC,	(CMD_DEFINE)&Command_GMs);
	CommandMap["GMMENU"]			= CommandMapEntry_st( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&Command_GmMenu);
	CommandMap["GCOLLECT"]			= CommandMapEntry_st( CL_GM,		CMD_FUNC,		(CMD_DEFINE)&CollectGarbage);
	CommandMap["GQ"]				= CommandMapEntry_st( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&Command_GQ);
	//H
	CommandMap["HOWTO"]				= CommandMapEntry_st( CL_PLAYER,	CMD_SOCKFUNC,	(CMD_DEFINE)&Command_HowTo );
	//I
	//J
	//K
	//L
	CommandMap["LOADDEFAULTS"]		= CommandMapEntry_st( CL_ADMIN,	CMD_FUNC,		(CMD_DEFINE)&Command_LoadDefaults);
	//M
	CommandMap["MEMSTATS"]			= CommandMapEntry_st( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&Command_MemStats);
	CommandMap["MINECHECK"]			= CommandMapEntry_st( CL_GM,		CMD_FUNC,		(CMD_DEFINE)&Command_MineCheck);
	//N
	//O
	//P
	CommandMap["PDUMP"]				= CommandMapEntry_st( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&Command_PDump);
	CommandMap["POST"]    			= CommandMapEntry_st( CL_CNS,		CMD_SOCKFUNC,	(CMD_DEFINE)&Command_GetPost);
	//Q
	//R
	CommandMap["RESTOCK"]			= CommandMapEntry_st( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&Command_Restock);
	CommandMap["RESPAWN"]			= CommandMapEntry_st( CL_GM,		CMD_FUNC,		(CMD_DEFINE)&Command_Respawn);
	CommandMap["REGSPAWN"]			= CommandMapEntry_st( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&Command_RegSpawn);
	CommandMap["REPORTBUG"]			= CommandMapEntry_st( CL_PLAYER,	CMD_SOCKFUNC,	(CMD_DEFINE)&Command_ReportBug);
	//S
	CommandMap["SETPOST"]			= CommandMapEntry_st( CL_CNS,		CMD_SOCKFUNC,	(CMD_DEFINE)&Command_SetPost);
	CommandMap["SPAWNKILL"]			= CommandMapEntry_st( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&Command_SpawnKill);
	CommandMap["SETSHOPRESTOCKRATE"]= CommandMapEntry_st( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&Command_SetShopRestockRate);
	CommandMap["SETTIME"]			= CommandMapEntry_st( CL_GM,		CMD_FUNC,		(CMD_DEFINE)&Command_SetTime);
	CommandMap["SHUTDOWN"]			= CommandMapEntry_st( CL_GM,		CMD_FUNC,		(CMD_DEFINE)&Command_Shutdown);
	CommandMap["SAVE"]				= CommandMapEntry_st( CL_GM,		CMD_FUNC,		(CMD_DEFINE)&Command_Save);
	CommandMap["STATUS"]			= CommandMapEntry_st( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&Command_Status);
	CommandMap["SHOWIDS"]			= CommandMapEntry_st( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&Command_ShowIds);
	//T
	CommandMap["TEMP"]				= CommandMapEntry_st( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&Command_Temp );
	CommandMap["TELL"]				= CommandMapEntry_st( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&Command_Tell);
	CommandMap["TILE"]				= CommandMapEntry_st( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&Command_Tile);
	//U
	//V
	CommandMap["VALIDCMD"]			= CommandMapEntry_st( CL_PLAYER,	CMD_SOCKFUNC,	(CMD_DEFINE)&Command_ValidCmd );
	//W
	CommandMap["WHO"]				= CommandMapEntry_st( CL_CNS,		CMD_SOCKFUNC,	(CMD_DEFINE)&Command_Who);
	//X
	//Y
	//Z
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CCommands::UnRegister()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Unregisters a command from JS command table
//o------------------------------------------------------------------------------------------------o
void CCommands::UnRegister( const std::string &cmdName, [[maybe_unused]] cScript *toRegister )
{
#if defined( UOX_DEBUG_MODE )
	Console.Print( oldstrutil::format( "   UnRegistering command %s\n", cmdName.c_str() ));
#endif
	std::string upper	= cmdName;
	upper				= oldstrutil::upper( upper );
	JSCOMMANDMAP_ITERATOR p = JSCommandMap.find( upper );
	if( p != JSCommandMap.end() )
	{
		JSCommandMap.erase( p );
	}
#if defined( UOX_DEBUG_MODE )
	else
	{
		Console.Print( oldstrutil::format( "         Command \"%s\" was not found.\n", cmdName.c_str() ));
	}
#endif
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CCommands::Register()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Resgisters a new command in JS command table
//o------------------------------------------------------------------------------------------------o
void CCommands::Register( const std::string &cmdName, UI16 scriptId, UI08 cmdLevel, bool isEnabled )
{
#if defined( UOX_DEBUG_MODE )
	Console.Print( " " );
	Console.MoveTo( 15 );
	Console.Print( "Registering " );
	Console.TurnYellow();
	Console.Print( cmdName );
	Console.TurnNormal();
	Console.MoveTo( 50 );
	Console.Print( "@ command level " );
	Console.TurnYellow();
	Console.Print( oldstrutil::format( "%i\n", cmdLevel ));
	Console.TurnNormal();
#endif
	std::string upper	= cmdName;
	upper				= oldstrutil::upper( upper );
	JSCommandMap[upper]	= JSCommandEntry_st( cmdLevel, scriptId, isEnabled );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CCommands::SetCommandStatus()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Enables or disables a command in JS command table
//o------------------------------------------------------------------------------------------------o
void CCommands::SetCommandStatus( const std::string &cmdName, bool isEnabled )
{
	std::string upper				= cmdName;
	upper							= oldstrutil::upper( upper );
	JSCOMMANDMAP_ITERATOR	toFind	= JSCommandMap.find( upper );
	if( toFind != JSCommandMap.end() )
	{
		toFind->second.isEnabled = isEnabled;
	}
}
