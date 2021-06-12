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
void endmessage( SI32 x );
void HandleGumpCommand( CSocket *s, std::string cmd, std::string data );
void restock( bool stockAll );
void sysBroadcast( const std::string& txt );
void HandleHowTo( CSocket *sock, SI32 cmdNumber );

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void closeCall( CSocket *s, bool isGM )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Closes an open call in the Que
//o-----------------------------------------------------------------------------------------------o
void closeCall( CSocket *s, bool isGM )
{
	CChar *mChar = s->CurrcharObj();
	if( mChar->GetCallNum() != 0 )
	{
		UI16 sysMessage = 1285;
		PageVector *tmpVector = nullptr;
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void currentCall( CSocket *s, bool isGM )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Send GM/Counselor to the current call in the queue
//o-----------------------------------------------------------------------------------------------o
void currentCall( CSocket *s, bool isGM )
{
	CChar *mChar = s->CurrcharObj();
	if( mChar->GetCallNum() != 0 )
	{
		PageVector *tmpVector = nullptr;
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void nextCall( CSocket *s, bool isGM )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Send GM/Counsellor to next call in the que
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool FixSpawnFunctor( CBaseObject *a, UI32 &b, void *extraData )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Replaces legacy spawner objects that have incorrect item type
//o-----------------------------------------------------------------------------------------------o
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
	ObjectFactory::getSingleton().IterateOver( OT_ITEM, b, nullptr, &FixSpawnFunctor );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void command_addaccount( CSocket *s)
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	GM command for adding new user accounts while in-game
//o-----------------------------------------------------------------------------------------------o
void command_addaccount( CSocket *s)
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() > 1 )
	{
		std::string newUsername = Commands->CommandString( 2, 2 );
		std::string newPassword = Commands->CommandString( 3, 3 );
		UI16 newFlags = 0x0000;

		if( Commands->NumArguments() > 2 )
			newFlags = strutil::value<UI16>( Commands->CommandString( 4, 4 ));

		if( newUsername == "" || newPassword == "" )
		{
			s->sysmessage( "Unable to add account, insufficient data provided (syntax: [username] [password] [flags])" );
			return;
		}

		// ok we need to add the account now. We will rely in the internalaccountscreation system for this
		CAccountBlock &actbTemp = Accounts->GetAccountByName( newUsername );
		if( actbTemp.wAccountIndex == AB_INVALID_ID )
		{
			Accounts->AddAccount( newUsername, newPassword, "NA", newFlags );
			Console << "o Account added ingame: " << newUsername << ":" << newPassword << ":" << newFlags << myendl;

			s->sysmessage( strutil::format( "Account Added: %s:%s:%i", newUsername.c_str(), newPassword.c_str(), newFlags ) );
		}
		else
		{
			Console << "o Account was not added, an account with that username already exists!" << myendl;
			s->sysmessage( "Account not added, an account with that username already exists!" );
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void command_getlight( CSocket *s )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Displays current in-game light level as a system message
//o-----------------------------------------------------------------------------------------------o
//|	Notes		-	Needs redoing to support new lighting system
//o-----------------------------------------------------------------------------------------------o
void command_getlight( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return;
	CTownRegion *tRegion	= mChar->GetRegion();
	UI16 wID				= tRegion->GetWeather();
	CWeather *sys			= Weather->Weather( wID );
	if( sys != nullptr )
	{
		const R32 lightMin = sys->LightMin();
		const R32 lightMax = sys->LightMax();
		if( lightMin < 300 && lightMax < 300 )
		{
			R32 i = sys->CurrentLight();
			if( Races->VisLevel( mChar->GetRace() ) > i )
				s->sysmessage( 1632, 0 );
			else
				s->sysmessage( 1632, static_cast<LIGHTLEVEL>(roundNumber( i - Races->VisLevel( mChar->GetRace() ))));
		}
		else
			s->sysmessage( 1632, cwmWorldState->ServerData()->WorldLightCurrentLevel() );
	}
	else
		s->sysmessage( 1632, cwmWorldState->ServerData()->WorldLightCurrentLevel() );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void command_setpost( CSocket *s )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets bulletin board posting mode for user
//o-----------------------------------------------------------------------------------------------o
void command_setpost( CSocket *s )
{
	VALIDATESOCKET( s );

	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return;

	PostTypes type = PT_LOCAL;
	std::string upperCommand = strutil::toupper( Commands->CommandString( 2, 2 ));
	if( upperCommand == "GLOBAL" ) // user's next post appears in ALL bulletin boards
		type = PT_GLOBAL;
	else if( upperCommand == "REGIONAL" ) // user's next post appears in all bulletin boards in current region
		type = PT_REGIONAL;
	else if( upperCommand == "LOCAL" ) // user's next post appears only locally in the next bulletin board used
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void command_getpost( CSocket *s )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns current bulletin board posting mode for user
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void command_showids( CSocket *s )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Display the serial number of every item on user's screen
//o-----------------------------------------------------------------------------------------------o
void command_showids( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar	= s->CurrcharObj();
	CMapRegion *Cell = MapRegion->GetMapRegion( mChar );
	if( Cell == nullptr )	// nothing to show
		return;
	GenericList< CChar * > *regChars = Cell->GetCharList();
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void command_tile( CSocket *s )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	(h) Tiles the item specified over a square area.
//o-----------------------------------------------------------------------------------------------o
//|	Notes		-	To find the hexidecimal ID code for an item to tile,
//|					either create the item with /add or find it in the
//|					world, and get /ISTATS on the object to get its ID code.
//o-----------------------------------------------------------------------------------------------o
void command_tile( CSocket *s )
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() == 0 )
		return;

	UI16 targID = 0;
	if( strutil::toupper( Commands->CommandString( 2, 2 )) == "STATIC" )
	{
		targID = static_cast<UI16>(Commands->Argument( 2 ));
	}
	else
	{
		targID = static_cast<UI16>(Commands->Argument( 1 ));
	}

	// Reset tempint2 on socket
	s->TempInt2( 0 );

	if( Commands->NumArguments() == 7 || Commands->NumArguments() == 8 )
	{
		// tile itemID x1 y1 x2 y2 z rndVal
		SI32 rndVal = 0;
		UI16 rndID = 0;

		if( Commands->NumArguments() == 8 )
			rndVal = static_cast<SI32>(Commands->Argument( 7 ));

		// tile itemID x1 y1 x2 y2 z
		SI16 x1 = static_cast<SI16>(Commands->Argument( 2 ));
		SI16 x2 = static_cast<SI16>(Commands->Argument( 3 ));
		SI16 y1 = static_cast<SI16>(Commands->Argument( 4 ));
		SI16 y2 = static_cast<SI16>(Commands->Argument( 5 ));
		SI08 z  = static_cast<SI08>(Commands->Argument( 6 ));

		for( SI16 x = x1; x <= x2; ++x )
		{
			for( SI16 y = y1; y <= y2; ++y )
			{
				rndID = targID + RandomNum( static_cast<UI16>(0), static_cast<UI16>(rndVal) );
				CItem *a = Items->CreateItem( nullptr, s->CurrcharObj(), rndID, 1, 0, OT_ITEM );
				if( ValidateObject( a ) )	// crash prevention
				{
					a->SetLocation( x, y, z );
					a->SetDecayable( false );
				}
			}
		}
	}
	else if( Commands->NumArguments() == 4 )
	{
		// tile static itemID rndVal
		s->ClickX( -1 );
		s->ClickY( -1 );
		s->TempInt2( static_cast<SI32>(Commands->Argument( 3 )));

		s->AddID1( static_cast<UI08>(targID>>8) );
		s->AddID2( static_cast<UI08>(targID%256) );
		s->target( 0, TARGET_TILING, 24 );
	}
	else if( Commands->NumArguments() == 3 )
	{
		// tile itemID rndVal
		// tile static itemID
		s->ClickX( -1 );
		s->ClickY( -1 );
		if( strutil::toupper( Commands->CommandString( 2, 2 )) != "STATIC" )
		{
			s->TempInt2( static_cast<SI32>(Commands->Argument( 2 )));
		}

		s->AddID1( static_cast<UI08>(targID>>8) );
		s->AddID2( static_cast<UI08>(targID%256) );
		s->target( 0, TARGET_TILING, 24 );
	}
	else if( Commands->NumArguments() == 2 )
	{
		// tile itemID
		s->ClickX( -1 );
		s->ClickY( -1 );

		s->AddID1( static_cast<UI08>(targID>>8) );
		s->AddID2( static_cast<UI08>(targID%256) );
		s->target( 0, TARGET_TILING, 24 );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void command_save( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Saves the current world data into .WSC files
//o-----------------------------------------------------------------------------------------------o
void command_save( void )
{
	if( cwmWorldState->GetWorldSaveProgress() != SS_SAVING )
		cwmWorldState->SaveNewWorld( true );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void command_dye( CSocket *s )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	(h h/nothing) Dyes an item a specific color, or brings up
//|					a dyeing menu if no color is specified.
//o-----------------------------------------------------------------------------------------------o
void command_dye( CSocket *s )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void command_settime( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	(d d) Sets the current UO time in hours and minutes.
//o-----------------------------------------------------------------------------------------------o
void command_settime( void )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void command_shutdown( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	(d) Shuts down the server. Argument is how many minutes
//|					until shutdown.
//o-----------------------------------------------------------------------------------------------o
void command_shutdown( void )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void command_tell( CSocket *s )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	(d text) Sends an anonymous message to the user logged in under the specified slot.
//o-----------------------------------------------------------------------------------------------o
void command_tell( CSocket *s )
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() > 2 )
	{
		CSocket *i = Network->GetSockPtr( Commands->Argument( 1 ) );
		std::string txt = Commands->CommandString( 3 );
		if( i == nullptr || txt.empty() )
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
		if( mChar->GetSayColour() == 0x1700 )
			toAdd.Colour( 0x5A );
		else if( mChar->GetSayColour() == 0x0 )
			toAdd.Colour( 0x5A );
		else
			toAdd.Colour( mChar->GetSayColour() );

	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void command_gmmenu( CSocket *s )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	(d) Opens the specified GM Menu from dfndata/menus.dfn
//o-----------------------------------------------------------------------------------------------o
void command_gmmenu( CSocket *s )
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() == 2 )
	{
		CPIHelpRequest toHandle( s, static_cast<UI16>(Commands->Argument( 1 ) ) );
		toHandle.Handle();
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void command_command( CSocket *s )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Executes a trigger scripting command.
//o-----------------------------------------------------------------------------------------------o
void command_command( CSocket *s )
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() > 1 )
	{
		HandleGumpCommand( s, strutil::toupper( Commands->CommandString( 2, 2 )), strutil::toupper( Commands->CommandString( 3 )) );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void command_memstats( CSocket *s )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Display some information about cache and memory usage.
//o-----------------------------------------------------------------------------------------------o
void command_memstats( CSocket *s )
{
	VALIDATESOCKET( s );
	size_t cacheSize		= Map->GetTileMem() + Map->GetMultisMem();
	size_t charsSize		= ObjectFactory::getSingleton().SizeOfObjects( OT_CHAR );
	size_t itemsSize		= ObjectFactory::getSingleton().SizeOfObjects( OT_ITEM );
	size_t spellsSize		= 69 * sizeof( SpellInfo );
	size_t teffectsSize		= sizeof( CTEffect ) * cwmWorldState->tempEffects.Num();
	size_t regionsSize		= sizeof( CTownRegion ) * cwmWorldState->townRegions.size();
	size_t spawnregionsSize = sizeof( CSpawnRegion ) * cwmWorldState->spawnRegions.size();
	size_t total			= charsSize + itemsSize + spellsSize + cacheSize + regionsSize + spawnregionsSize + teffectsSize;
	GumpDisplay cacheStats( s, 350, 345 );
	cacheStats.SetTitle( "UOX Memory Information (sizes in bytes)" );
	cacheStats.AddData( "Total Memory Usage: ", static_cast<UI32>(total) );
	cacheStats.AddData( " Cache Size: ", static_cast<UI32>(cacheSize ));
	cacheStats.AddData( "  Tiles: ", static_cast<UI32>(Map->GetTileMem()) );
	cacheStats.AddData( "  Multis: ", static_cast<UI32>(Map->GetMultisMem() ));
	cacheStats.AddData( " Characters: ", ObjectFactory::getSingleton().CountOfObjects( OT_CHAR ) );
	cacheStats.AddData( "  Allocated Memory: ", static_cast<UI32>(charsSize ));
	cacheStats.AddData( "  CChar: ", sizeof( CChar ) );
	cacheStats.AddData( " Items: ", ObjectFactory::getSingleton().CountOfObjects( OT_ITEM ) );
	cacheStats.AddData( "  Allocated Memory: ", static_cast<UI32>(itemsSize ));
	cacheStats.AddData( "  CItem: ", sizeof( CItem ) );
	cacheStats.AddData( " Spells Size: ", static_cast<UI32>(spellsSize ));
	cacheStats.AddData( " TEffects: ", static_cast<UI32>(cwmWorldState->tempEffects.Num() ));
	cacheStats.AddData( "  Allocated Memory: ", static_cast<UI32>(teffectsSize ));
	cacheStats.AddData( "  TEffect: ", sizeof( CTEffect ) );
	cacheStats.AddData( " Regions Size: ", static_cast<UI32>(cwmWorldState->townRegions.size()) );
	cacheStats.AddData( "  Allocated Memory: ", static_cast<UI32>( regionsSize) );
	cacheStats.AddData( "  CTownRegion: ", sizeof( CTownRegion ) );
	cacheStats.AddData( " SpawnRegions ", static_cast<UI32>(cwmWorldState->spawnRegions.size() ));
	cacheStats.AddData( "  Allocated Memory: ", static_cast<UI32>(spawnregionsSize ));
	cacheStats.AddData( "  CSpawnRegion: ", sizeof( CSpawnRegion ) );
	cacheStats.Send( 0, false, INVALIDSERIAL );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void command_restock( CSocket *s )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	(nothing / s) Forces a manual vendor restock
//o-----------------------------------------------------------------------------------------------o
void command_restock( CSocket *s )
{
	VALIDATESOCKET( s );
	if( strutil::toupper( Commands->CommandString( 2, 2 )) == "ALL" )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void command_restock( CSocket *s )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	(d) Sets the universe's shop restock rate
//o-----------------------------------------------------------------------------------------------o
void command_setshoprestockrate( CSocket *s )
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
					spawnItem->SetTempTimer( BuildTimeValue( static_cast<R32>(RandomNum( spawnItem->GetInterval( 0 ) * 60, spawnItem->GetInterval( 1 ) * 60 ) )) );
			}
			else
				i->SetType( IT_NOTYPE );
		}
	}
	return retVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void command_respawn( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Forces a respawn of all spawn regions
//o-----------------------------------------------------------------------------------------------o
void command_respawn( void )
{
	UI16 spawnedItems	= 0;
	UI16 spawnedNpcs	= 0;
	SPAWNMAP_CITERATOR spIter	= cwmWorldState->spawnRegions.begin();
	SPAWNMAP_CITERATOR spEnd	= cwmWorldState->spawnRegions.end();
	while( spIter != spEnd )
	{
		CSpawnRegion *spawnReg = spIter->second;
		if( spawnReg != nullptr )
			spawnReg->doRegionSpawn( spawnedItems, spawnedNpcs );
		++spIter;
	}

	UI32 b		= 0;
	ObjectFactory::getSingleton().IterateOver( OT_ITEM, b, nullptr, &RespawnFunctor );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void command_regspawn( CSocket *s )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	(s/d) Forces a region spawn, First argument is region number or "ALL"
//o-----------------------------------------------------------------------------------------------o
void command_regspawn( CSocket *s )
{
	VALIDATESOCKET( s );

	if( Commands->NumArguments() == 2 )
	{
		UI16 itemsSpawned	= 0;
		UI16 npcsSpawned	= 0;

		if( strutil::toupper( Commands->CommandString( 2, 2 )) == "ALL" )
		{
			SPAWNMAP_CITERATOR spIter	= cwmWorldState->spawnRegions.begin();
			SPAWNMAP_CITERATOR spEnd	= cwmWorldState->spawnRegions.end();
			while( spIter != spEnd )
			{
				CSpawnRegion *spawnReg = spIter->second;
				if( spawnReg != nullptr )
					spawnReg->doRegionSpawn( itemsSpawned, npcsSpawned );
				++spIter;
			}
			if( itemsSpawned || npcsSpawned )
				s->sysmessage( "Spawned %u items and %u npcs in %u SpawnRegions", itemsSpawned, npcsSpawned, cwmWorldState->spawnRegions.size() );
			else
				s->sysmessage( "Failed to spawn any new items or npcs in %u SpawnRegions", cwmWorldState->spawnRegions.size() );
		}
		else
		{
			UI16 spawnRegNum = static_cast<UI16>(Commands->Argument( 1 ));
			if( cwmWorldState->spawnRegions.find( spawnRegNum ) != cwmWorldState->spawnRegions.end() )
			{
				CSpawnRegion *spawnReg = cwmWorldState->spawnRegions[spawnRegNum];
				if( spawnReg != nullptr )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void command_loaddefaults( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads the hardcoded server defaults for various settings in uox.ini
//o-----------------------------------------------------------------------------------------------o
void command_loaddefaults( void )
{
	cwmWorldState->ServerData()->ResetDefaults();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void command_cq( CSocket *s )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Display the Counselor queue when used with no arguments
//|					Can trigger other queue-related commands when arguments are provided
//o-----------------------------------------------------------------------------------------------o
void command_cq( CSocket *s )
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() == 2 )
	{
		std::string upperCommand = strutil::toupper( Commands->CommandString( 2, 2 ));
		if( upperCommand == "NEXT" ) // Go to next call in Counselor queue
		{
			nextCall( s, false );
		}
		else if( upperCommand == "CLEAR" ) // Close and clear current call as resolved
		{
			closeCall( s, false );
		}
		else if( upperCommand == "CURR" ) // Take Counselor to current call they are on
		{
			currentCall( s, false );
		}
		else if( upperCommand == "TRANSFER" ) // Transfer call from Counselor queue to GM queue
		{
			CChar *mChar = s->CurrcharObj();
			if( mChar->GetCallNum() != 0 )
			{
				if( CounselorQueue->GotoPos( CounselorQueue->FindCallNum( mChar->GetCallNum() ) ) )
				{
					HelpRequest pageToAdd;
					HelpRequest *currentPage = nullptr;
					currentPage = CounselorQueue->Current();
					pageToAdd.Reason( currentPage->Reason() );
					pageToAdd.WhoPaging( currentPage->WhoPaging() );
					pageToAdd.IsHandled( false );
					pageToAdd.TimeOfPage( time( nullptr ) );
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void command_gq( CSocket *s )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Display the GM queue when used with no arguments
//|					Can trigger other queue-related commands when arguments are provided
//o-----------------------------------------------------------------------------------------------o
void command_gq( CSocket *s )
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() == 2 )
	{
		std::string upperCommand = strutil::toupper( Commands->CommandString( 2, 2 ));
		if( upperCommand == "NEXT" ) // Go to next call in GM queue
		{
			nextCall( s, true );
		}
		else if( upperCommand == "CLEAR" ) // Close and clear current call as resolved
		{
			closeCall( s, true );
		}
		else if( upperCommand == "CURR" ) // Take GM to current call they are on
		{
			currentCall( s, true );
		}
	}
	else
	{
		GMQueue->SendAsGump( s );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void command_minecheck( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	(d) Set the server mine check status to determine where mining will work
//|						0 - allow mining everywhere
//|						1 - mountainsides and cave floors
//|						2 - mining regions only
//o-----------------------------------------------------------------------------------------------o
void command_minecheck( void )
{
	if( Commands->NumArguments() == 2 )
		cwmWorldState->ServerData()->MineCheck( static_cast<UI08>(Commands->Argument( 1 )) );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void command_guards( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Enables (ON) or disables (OFF) town guards globally
//o-----------------------------------------------------------------------------------------------o
void command_guards( void )
{
	if( strutil::toupper( Commands->CommandString( 2, 2 )) == "ON" )
	{
		cwmWorldState->ServerData()->GuardStatus( true );
		sysBroadcast( Dictionary->GetEntry( 61 ) );
	}
	else if( strutil::toupper( Commands->CommandString( 2, 2 )) == "OFF" )
	{
		cwmWorldState->ServerData()->GuardStatus( false );
		sysBroadcast( Dictionary->GetEntry( 62 ) );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void command_announce( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Enables (ON) or disables (OFF) announcement of world saves
//o-----------------------------------------------------------------------------------------------o
void command_announce( void )
{
	if( strutil::toupper( Commands->CommandString( 2, 2 )) == "ON" )
	{
		cwmWorldState->ServerData()->ServerAnnounceSaves( true );
		sysBroadcast( Dictionary->GetEntry( 63 ) );
	}
	else if( strutil::toupper( Commands->CommandString( 2, 2 )) == "OFF" )
	{
		cwmWorldState->ServerData()->ServerAnnounceSaves( false );
		sysBroadcast( Dictionary->GetEntry( 64 ) );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void command_pdump( CSocket *s )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Display some performance information
//o-----------------------------------------------------------------------------------------------o
void command_pdump( CSocket *s )
{
	VALIDATESOCKET( s );
	UI32 networkTimeCount	= cwmWorldState->ServerProfile()->NetworkTimeCount();
	UI32 timerTimeCount		= cwmWorldState->ServerProfile()->TimerTimeCount();
	UI32 autoTimeCount		= cwmWorldState->ServerProfile()->AutoTimeCount();
	UI32 loopTimeCount		= cwmWorldState->ServerProfile()->LoopTimeCount();

	s->sysmessage( "Performance Dump:" );
	s->sysmessage( "Network code: %fmsec [%i]", (R32)((R32)cwmWorldState->ServerProfile()->NetworkTime()/(R32)networkTimeCount), networkTimeCount );
	s->sysmessage( "Timer code: %fmsec [%i]", (R32)((R32)cwmWorldState->ServerProfile()->TimerTime()/(R32)timerTimeCount), timerTimeCount );
	s->sysmessage( "Auto code: %fmsec [%i]", (R32)((R32)cwmWorldState->ServerProfile()->AutoTime()/(R32)autoTimeCount), autoTimeCount );
	s->sysmessage( "Loop Time: %fmsec [%i]", (R32)((R32)cwmWorldState->ServerProfile()->LoopTime()/(R32)loopTimeCount), loopTimeCount );
	s->sysmessage( "Simulation Cycles/Sec: %f", (1000.0*(1.0/(R32)((R32)cwmWorldState->ServerProfile()->LoopTime()/(R32)loopTimeCount ) ) ) );
}
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void command_spawnkill( CSocket *s )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	(d) Kills spawns from the specified spawn region in SPAWN.DFN
//o-----------------------------------------------------------------------------------------------o
void command_spawnkill( CSocket *s )
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() == 2 )
	{
		UI16 regNum = static_cast<UI16>(Commands->Argument( 1 ));
		if( cwmWorldState->spawnRegions.find( regNum ) == cwmWorldState->spawnRegions.end() )
			return;
		CSpawnRegion *spawnReg = cwmWorldState->spawnRegions[regNum];
		if( spawnReg == nullptr )
			return;
		SI32 killed	= 0;

		s->sysmessage( 349 );
		GenericList< CChar * > *spCharList = spawnReg->GetSpawnedCharsList();
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void BuildWhoGump( CSocket *s, UI08 commandLevel, std::string title )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Build and send a gump with a list of players based on provided criteria
//o-----------------------------------------------------------------------------------------------o
void BuildWhoGump( CSocket *s, UI08 commandLevel, std::string title )
{
	UI16 j = 0;

	GumpDisplay Who( s, 400, 300 );
	Who.SetTitle( title );
	{
		//std::scoped_lock lock(Network->internallock);
		Network->pushConn();
		for( CSocket *iSock = Network->FirstSocket(); !Network->FinishedSockets(); iSock = Network->NextSocket() )
		{
			CChar *iChar = iSock->CurrcharObj();
			if( iChar->GetCommandLevel() >= commandLevel )
			{
				auto temp = strutil::format("%i) %s", j, iChar->GetName().c_str() );
				Who.AddData( temp, iChar->GetSerial(), 3 );
			}
			++j;
		}
		Network->popConn();
	}
	Who.Send( 4, false, INVALIDSERIAL );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void command_who( CSocket *s )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Displays a list of users currently online
//o-----------------------------------------------------------------------------------------------o
void command_who( CSocket *s )
{
	VALIDATESOCKET( s );
	BuildWhoGump( s, 0, "Who's Online" );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void command_gms( CSocket *s )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Displays a list of server staff currently online
//o-----------------------------------------------------------------------------------------------o
void command_gms( CSocket *s )
{
	VALIDATESOCKET( s );
	BuildWhoGump( s, CL_CNS, Dictionary->GetEntry( 77, s->Language() ) );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void command_reportbug( CSocket *s )
//| Date		-	9th February, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Writes out a bug reported by player to the bug file
//o-----------------------------------------------------------------------------------------------o
void command_reportbug( CSocket *s )
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
		Console.error( strutil::format("Unable to open bugs log file %s!", logName.c_str()) );
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
	{
		//std::scoped_lock lock(Network->internallock);
		Network->pushConn();
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
		Network->popConn();
	}
	if( x )
		s->sysmessage( 88 );
	else
		s->sysmessage( 89 );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void command_forcewho( CSocket *s )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Brings up an interactive listing of online users.
//o-----------------------------------------------------------------------------------------------o
void command_forcewho( CSocket *s )
{
	VALIDATESOCKET( s );
	WhoList->ZeroWho();
	WhoList->SendSocket( s );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void command_validcmd( CSocket *s )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Displays all valid commands for the user's character
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void command_howto( CSocket *s )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens a list of commands, with explanations of how to use each specific command
//o-----------------------------------------------------------------------------------------------o
void command_howto( CSocket *s )
{
	VALIDATESOCKET( s );
	std::string commandStart = strutil::toupper( Commands->CommandString( 2 ));
	if( commandStart.empty() )
	{
		CChar *mChar = s->CurrcharObj();
		if( !ValidateObject( mChar ) )
			return;

		SI32 iCmd = 2;
		SI32 numAdded = 0;
		UI08 pagenum = 1;
		UI16 position = 40;
		UI16 linenum = 1;

		CPSendGumpMenu toSend;
		toSend.UserID( INVALIDSERIAL );
		toSend.GumpID( 13 );

		toSend.addCommand( strutil::format("resizepic 0 0 %u 480 320", cwmWorldState->ServerData()->BackgroundPic()) );
		toSend.addCommand( "page 0" );
		toSend.addCommand( "text 200 20 0 0" );
		toSend.addText( "HOWTO" );
		toSend.addCommand( strutil::format("button 440 20 %u %i 1 0 1", cwmWorldState->ServerData()->ButtonCancel(), cwmWorldState->ServerData()->ButtonCancel() + 1 ));

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
				toSend.addCommand( strutil::format("page %u", pagenum ));
				justDonePageAdd = true;
			}
			if( gAdd->second.cmdLevelReq <= currentLevel )
			{
				justDonePageAdd = false;
				toSend.addCommand( strutil::format("text 50 %u %u %u", position, cwmWorldState->ServerData()->LeftTextColour(), linenum ));
				toSend.addCommand( strutil::format("button 20 %u %u %i %u 0 %i", position, cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1, pagenum, iCmd ));
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
				toSend.addCommand( strutil::format("page %u", pagenum ));
				justDonePageAdd = true;
			}
			if( tAdd->second.cmdLevelReq <= currentLevel )
			{
				justDonePageAdd = false;
				toSend.addCommand( strutil::format("text 50 %u %u %u", position, cwmWorldState->ServerData()->LeftTextColour(), linenum ));
				toSend.addCommand( strutil::format("button 20 %u %u %i %u 0 %i", position, cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1, pagenum, iCmd ));
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
				toSend.addCommand( strutil::format("page %u", pagenum ));
				justDonePageAdd = true;
			}
			if( jAdd->second.cmdLevelReq <= currentLevel )
			{
				justDonePageAdd = false;
				toSend.addCommand( strutil::format("text 50 %u %u %u", position, cwmWorldState->ServerData()->LeftTextColour(), linenum ));
				toSend.addCommand( strutil::format("button 20 %u %u %i %u 0 %i", position, cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1, pagenum, iCmd ));
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
			toSend.addCommand( strutil::format("page %u", pagenum ));
			if( i >= 10 )
			{
				toSend.addCommand(strutil::format( "button 30 290 %u %i 0 %i", cwmWorldState->ServerData()->ButtonLeft(), cwmWorldState->ServerData()->ButtonLeft() + 1, pagenum - 1 )); //back button
			}
			if( ( numAdded > 10 ) && ( ( i + 10 ) < numAdded ) )
			{
				toSend.addCommand( strutil::format("button 440 290 %u %i 0 %i", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1, pagenum + 1 ));
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
			{
				JSCOMMANDMAP_ITERATOR findJS = JSCommandMap.begin();
				for( findJS = JSCommandMap.begin(); findJS != JSCommandMap.end(); ++findJS )
				{
					if( commandStart == findJS->first )
						break;
					++i;
				}
				if( findJS == JSCommandMap.end() )
				{
					s->sysmessage( 280 );
					return;
				}
			}
		}
		HandleHowTo( s, i );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void command_temp( CSocket *s )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Displays the current temperature in user's current townregion
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void command_status( CSocket *s )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens the HTML status information gump
//o-----------------------------------------------------------------------------------------------o
void command_status( CSocket *s )
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
	TargetMap["INFO"]			= TargetMapEntry( CL_GM,			CMD_TARGET,		TARGET_INFO,			261);
	// J
	// K
	// L
	// M
	TargetMap["MAKE"]			= TargetMapEntry( CL_ADMIN,			CMD_TARGETTXT,	TARGET_MAKESTATUS,		279);
	// N
	// O
	// P
	// Q
	// R
	// S
	TargetMap["SHOWSKILLS"]		= TargetMapEntry( CL_GM,			CMD_TARGETINT,	TARGET_SHOWSKILLS,		260);
	// T
	//TargetMap["TWEAK"]			= TargetMapEntry( CL_GM,			CMD_TARGET,		TARGET_TWEAK,			229);
	// U
	// V
	// W
	TargetMap["WSTATS"]			= TargetMapEntry( CL_CNS,			CMD_TARGET,		TARGET_WSTATS,			183);
	// X
	// Y
	// Z

	// CommandMap[Command Name] = CommandMapEntry(Required Command Level, Command Type, Command Function);
	//A
	CommandMap["ADDACCOUNT"]		= CommandMapEntry( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_addaccount);
	CommandMap["ANNOUNCE"]			= CommandMapEntry( CL_GM,		CMD_FUNC,		(CMD_DEFINE)&command_announce);
	//B
	//C
	CommandMap["CQ"]				= CommandMapEntry( CL_CNS,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_cq);
	CommandMap["COMMAND"]			= CommandMapEntry( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_command);
	//D
	CommandMap["DYE"]				= CommandMapEntry( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_dye);
	//E
	//F
	CommandMap["FORCEWHO"]			= CommandMapEntry( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_forcewho);
	CommandMap["FIXSPAWN"]			= CommandMapEntry( CL_GM,		CMD_FUNC,		(CMD_DEFINE)&command_fixspawn);
	//G,
	CommandMap["GETLIGHT"]			= CommandMapEntry( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_getlight);
	CommandMap["GUARDS"]			= CommandMapEntry( CL_GM,		CMD_FUNC,		(CMD_DEFINE)&command_guards);
	CommandMap["GMS"]				= CommandMapEntry( CL_CNS,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_gms);
	CommandMap["GMMENU"]			= CommandMapEntry( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_gmmenu);
	CommandMap["GCOLLECT"]			= CommandMapEntry( CL_GM,		CMD_FUNC,		(CMD_DEFINE)&CollectGarbage);
	CommandMap["GQ"]				= CommandMapEntry( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_gq);
	//H
	CommandMap["HOWTO"]				= CommandMapEntry( CL_PLAYER,	CMD_SOCKFUNC,	(CMD_DEFINE)&command_howto );
	//I
	//J
	//K
	//L
	CommandMap["LOADDEFAULTS"]		= CommandMapEntry( CL_ADMIN,	CMD_FUNC,		(CMD_DEFINE)&command_loaddefaults);
	//M
	CommandMap["MEMSTATS"]			= CommandMapEntry( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_memstats);
	CommandMap["MINECHECK"]			= CommandMapEntry( CL_GM,		CMD_FUNC,		(CMD_DEFINE)&command_minecheck);
	//N
	//O
	//P
	CommandMap["PDUMP"]				= CommandMapEntry( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_pdump);
	CommandMap["POST"]    			= CommandMapEntry( CL_CNS,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_getpost);
	//Q
	//R
	CommandMap["RESTOCK"]			= CommandMapEntry( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_restock);
	CommandMap["RESPAWN"]			= CommandMapEntry( CL_GM,		CMD_FUNC,		(CMD_DEFINE)&command_respawn);
	CommandMap["REGSPAWN"]			= CommandMapEntry( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_regspawn);
	CommandMap["REPORTBUG"]			= CommandMapEntry( CL_PLAYER,	CMD_SOCKFUNC,	(CMD_DEFINE)&command_reportbug);
	//S
	CommandMap["SETPOST"]			= CommandMapEntry( CL_CNS,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_setpost);
	CommandMap["SPAWNKILL"]			= CommandMapEntry( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_spawnkill);
	CommandMap["SETSHOPRESTOCKRATE"]= CommandMapEntry( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_setshoprestockrate);
	CommandMap["SETTIME"]			= CommandMapEntry( CL_GM,		CMD_FUNC,		(CMD_DEFINE)&command_settime);
	CommandMap["SHUTDOWN"]			= CommandMapEntry( CL_GM,		CMD_FUNC,		(CMD_DEFINE)&command_shutdown);
	CommandMap["SAVE"]				= CommandMapEntry( CL_GM,		CMD_FUNC,		(CMD_DEFINE)&command_save);
	CommandMap["STATUS"]			= CommandMapEntry( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_status);
	CommandMap["SHOWIDS"]			= CommandMapEntry( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_showids);
	//T
	CommandMap["TEMP"]				= CommandMapEntry( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_temp );
	CommandMap["TELL"]				= CommandMapEntry( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_tell);
	CommandMap["TILE"]				= CommandMapEntry( CL_GM,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_tile);
	//U
	//V
	CommandMap["VALIDCMD"]			= CommandMapEntry( CL_PLAYER,	CMD_SOCKFUNC,	(CMD_DEFINE)&command_validcmd );
	//W
	CommandMap["WHO"]				= CommandMapEntry( CL_CNS,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_who);
	//X
	//Y
	//Z
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void UnRegister( const std::string &cmdName, cScript *toRegister )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Unregisters a command from JS command table
//o-----------------------------------------------------------------------------------------------o
void cCommands::UnRegister( const std::string &cmdName, cScript *toRegister )
{
#if defined( UOX_DEBUG_MODE )
	Console.print(strutil::format( "   UnRegistering command %s\n", cmdName.c_str()));
#endif
	std::string upper	= cmdName;
	upper				= strutil::toupper( upper );
	JSCOMMANDMAP_ITERATOR p = JSCommandMap.find( upper );
	if( p != JSCommandMap.end() )
		JSCommandMap.erase( p );
#if defined( UOX_DEBUG_MODE )
	else
		Console.print( strutil::format("         Command \"%s\" was not found.\n", cmdName.c_str()));
#endif
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void Register( const std::string &cmdName, UI16 scriptID, UI08 cmdLevel, bool isEnabled )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Resgisters a new command in JS command table
//o-----------------------------------------------------------------------------------------------o
void cCommands::Register( const std::string &cmdName, UI16 scriptID, UI08 cmdLevel, bool isEnabled )
{
#if defined( UOX_DEBUG_MODE )
	Console.print( " " );
	Console.MoveTo( 15 );
	Console.print( "Registering " );
	Console.TurnYellow();
	Console.print( cmdName );
	Console.TurnNormal();
	Console.MoveTo( 50 );
	Console.print( "@ command level " );
	Console.TurnYellow();
	Console.print( strutil::format("%i\n", cmdLevel) );
	Console.TurnNormal();
#endif
	std::string upper	= cmdName;
	upper				= strutil::toupper( upper );
	JSCommandMap[upper]	= JSCommandEntry( cmdLevel, scriptID, isEnabled );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetCommandStatus( const std::string &cmdName, bool isEnabled )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Enables or disables a command in JS command table
//o-----------------------------------------------------------------------------------------------o
void cCommands::SetCommandStatus( const std::string &cmdName, bool isEnabled )
{
	std::string upper				= cmdName;
	upper							= strutil::toupper( upper );
	JSCOMMANDMAP_ITERATOR	toFind	= JSCommandMap.find( upper );
	if( toFind != JSCommandMap.end() )
	{
		toFind->second.isEnabled = isEnabled;
	}
}
