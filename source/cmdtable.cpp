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
#include "targeting.h"
#include "commands.h"
#include "msgboard.h"
#include "townregion.h"
#include "cWeather.hpp"
#include "cRaces.h"
#include "cServerDefinitions.h"
#include "wholist.h"
#include "cSpawnRegion.h"
#include "PageVector.h"
#include "speech.h"
#include "cHTMLSystem.h"
#include "gump.h"
#include "cEffects.h"
#include "network.h"
#include "classes.h"
#include "regions.h"
#include "CPacketSend.h"
#include "teffect.h"
#include "magic.h"
#include "ObjectFactory.h"

#include "Dictionary.h"

namespace UOX
{

void BuildAddMenuGump( cSocket *s, UI16 m );	// Menus for item creation
void CollectGarbage( void );
void endmessage( int x );
void HandleGumpCommand( cSocket *s, UString cmd, UString data );
void restock( bool stockAll );
void SpawnGate( CChar *caster, SI16 srcX, SI16 srcY, SI08 srcZ, SI16 trgX, SI16 trgY, SI08 trgZ );
void sysBroadcast( std::string txt );
void HandleHowTo( cSocket *sock, int cmdNumber );
void BuildGumpFromScripts( cSocket *s, UI16 m );
void telltime( cSocket *s );
void MakePlace( cSocket *s, SI32 i );

#define command_time telltime
#define command_gcollect CollectGarbage

//o---------------------------------------------------------------------------o
//|	Function	-	void cCommands::showQue( cSocket *s, bool isGM )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Shows next unhandled call in the queue
//o---------------------------------------------------------------------------o
void showQue( cSocket *s, bool isGM )
{
	if( isGM )
		GMQueue->SendAsGump( s );
	else
		CounselorQueue->SendAsGump( s );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cCommands::closeCall( cSocket *s, bool isGM )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Closes an open call in the Que
//o---------------------------------------------------------------------------o
void closeCall( cSocket *s, bool isGM )
{
	CChar *mChar = s->CurrcharObj();
	if( mChar->GetCallNum() != 0 )
	{
		if( isGM )
		{
			if( GMQueue->GotoPos( GMQueue->FindCallNum( mChar->GetCallNum() ) ) )
			{
				GMQueue->Remove();
				mChar->SetCallNum( 0 );
				s->sysmessage( 1285 );
			}
		}
		else
		{
			if( CounselorQueue->GotoPos( CounselorQueue->FindCallNum( mChar->GetCallNum() ) ) )
			{
				CounselorQueue->Remove();
				mChar->SetCallNum( 0 );
				s->sysmessage( 1286 );
			}
		}
	}
	else
		s->sysmessage( 1287 );
}

void currentCall( cSocket *s, bool isGM )
{
	CChar *mChar = s->CurrcharObj();
	if( mChar->GetCallNum() != 0 )
	{
		CChar *i = NULL;
		HelpRequest *currentPage = NULL;
		if( isGM )
		{
			if( GMQueue->GotoPos( GMQueue->FindCallNum( mChar->GetCallNum() ) ) )
			{
				currentPage = GMQueue->Current();
				i			= calcCharObjFromSer( currentPage->WhoPaging() );
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
		{
			if( CounselorQueue->GotoPos( CounselorQueue->FindCallNum( mChar->GetCallNum() ) ) )
			{
				currentPage		= CounselorQueue->Current();
				i				= calcCharObjFromSer( currentPage->WhoPaging() );
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
	}
	else
		s->sysmessage( 72 );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cCommands::nextCall( cSocket *s, bool isGM )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Send GM/Counsellor to next call in the que
//o---------------------------------------------------------------------------o
void nextCall( cSocket *s, bool isGM )
{
	CChar *j = NULL;
	int x = 0;
	CChar *mChar = s->CurrcharObj();
	if( mChar->GetCallNum() != 0 )
		closeCall( s, isGM );
	HelpRequest *tempPage = NULL;
	if( isGM )
	{
		for( tempPage = GMQueue->First(); !GMQueue->AtEnd(); tempPage = GMQueue->Next() )
		{
			if( !tempPage->IsHandled() )
			{
				j = calcCharObjFromSer( tempPage->WhoPaging() );
				if( ValidateObject( j ) )
				{
					GumpDisplay GMNext( s, 300, 200 );
					GMNext.AddData( "Pager: ", j->GetName() );
					GMNext.AddData( "Problem: ", tempPage->Reason() );
					GMNext.AddData( "Serial number ", tempPage->WhoPaging(), 3 );
					GMNext.AddData( "Paged at: ", tempPage->TimeOfPage() );
					GMNext.Send( 4, false, INVALIDSERIAL );
					tempPage->IsHandled( true );
					mChar->SetLocation( j );
					mChar->SetCallNum( static_cast<SI16>(tempPage->RequestID() ));
					++x;
				}
				if( x > 0 )
					break;
			}
		}
		if( x == 0 )
			s->sysmessage( 347 );
	}
	else //Player is a counselor
	{
		x = 0;
		for( tempPage = CounselorQueue->First(); !CounselorQueue->AtEnd(); tempPage = CounselorQueue->Next() )
		{
			if( !tempPage->IsHandled() )
			{
				j = calcCharObjFromSer( tempPage->WhoPaging() );
				if( ValidateObject( j ) )
				{
					GumpDisplay CNext( s, 300, 200 );
					CNext.AddData( "Pager: ", j->GetName() );
					CNext.AddData( "Problem: ", tempPage->Reason() );
					CNext.AddData( "Serial number ", tempPage->WhoPaging(), 3 );
					CNext.AddData( "Paged at: ", tempPage->TimeOfPage() );
					CNext.Send( 4, false, INVALIDSERIAL );
					tempPage->IsHandled( true );
					mChar->SetCallNum( static_cast<SI16>(tempPage->RequestID()) );
					mChar->SetLocation( j );
					++x;
					break;
				}
			}
			if( x > 0 )
				break;
		}
		if( x == 0 )
			s->sysmessage( 348 );
	}
}

bool FixSpawnFunctor( cBaseObject *a, UI32 &b, void *extraData )
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
					spawnItem->SetInterval( 0, i->GetTempVar( CITV_MOREY ) );
					spawnItem->SetInterval( 1, i->GetTempVar( CITV_MOREX ) );
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
//|	Function/Class-	void command_addaccount( cSocket *s)
//|	Date					-	10/17/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//|	Returns				-
//o--------------------------------------------------------------------------o
//|	Notes					-	
//o--------------------------------------------------------------------------o	
void command_addaccount( cSocket *s)
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
		if( Accounts->GetAccountByName(szUsername,actbTemp) )
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

void command_setrace( cSocket *s )
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() == 2 )
	{
		RACEID race = (RACEID)Commands->Argument( 1 );
		if( race < Races->Count() || race == 65535 )
			Races->gate( s->CurrcharObj(), race, 1 );
		else
			s->sysmessage( 13 );
	}
}
void command_getlight( cSocket *s )
// needs redoing to support new lighting system
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return;
	cTownRegion *tRegion	= mChar->GetRegion();
	UI08 wID				= tRegion->GetWeather();
	CWeather *sys			= Weather->Weather( wID );
	if( sys != NULL )
		s->sysmessage( 1632, sys->CurrentLight() );
	else
		s->sysmessage( 1632, cwmWorldState->ServerData()->WorldLightCurrentLevel() );
}

void command_resend( cSocket *s )
// Resends server data to client.
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return;
	mChar->Teleport();
}
// Returns the current bulletin board posting mode for the player
void command_post( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return;

	UnicodeTypes sLang	= s->Language();
	std::string temp	= Dictionary->GetEntry( 269, sLang );

	switch( mChar->GetPostType() )
	{
		case LOCALPOST:		temp += Dictionary->GetEntry( 270, sLang );		break;
		case REGIONALPOST:	temp += Dictionary->GetEntry( 271, sLang );		break;
		case GLOBALPOST:	temp += Dictionary->GetEntry( 272, sLang );		break;
		default:			temp += Dictionary->GetEntry( 273, sLang );
							mChar->SetPostType( LOCALPOST );
							break;
	}
	s->sysmessage( temp.c_str() );
}

void command_setpost( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return;
	UString upperCommand = Commands->CommandString( 2, 2 ).upper();
	if( upperCommand == "GLOBAL" )
	{
		// Sets the current bulletin board posting mode for the player to GLOBAL
		// ALL bulletin boards will see the next message posted to any bulletin board
		mChar->SetPostType( GLOBALPOST );
		s->sysmessage( 16 );
	}
	else if( upperCommand == "REGIONAL" )
	{
		// Sets the current bulletin board posting mode for the player to REGIONAL
		// All bulletin boards in the same region as the board posted to will show the
		// next message posted
		mChar->SetPostType( REGIONALPOST );
		s->sysmessage( 17 );
	}
	else if( upperCommand == "LOCAL" )
	{
		// Sets the current bulletin board posting mode for the player to LOCAL
		// Only this bulletin board will have this post
		mChar->SetPostType( LOCALPOST );
		s->sysmessage( 18 );
	}
}

void command_where( cSocket *s )
// Prints your current coordinates + region.
{
	VALIDATESOCKET( s );
	CChar *mChar			= s->CurrcharObj();
	cTownRegion *plRegion	= mChar->GetRegion();
	if( !plRegion->GetName().size() )
		s->sysmessage( 274, plRegion->GetName().c_str() );
	else
		s->sysmessage( 275 );

	s->sysmessage( "%i %i (%i/%i) %u", mChar->GetX(), mChar->GetY(), mChar->GetZ(), mChar->GetDispZ(), mChar->WorldNumber() ); 
}

//o---------------------------------------------------------------------------o
//|	Function	-	SI08 validTelePos( cSocket *s )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Checks if location is valid to teleport to
//o---------------------------------------------------------------------------o
SI08 validTelePos( cSocket *s )
{
	CChar *mChar = s->CurrcharObj();
	SI08 z = -1;
	SI16 tX = mChar->GetX();
	SI16 tY = mChar->GetY();
	if( tX >= 1397 && tX <= 1400 && tY >= 1622 && tY <= 1630 )
		z = 28;
	if( tX >= 1510 && tX <= 1537 && tY >= 1455 && tY <= 1456 )
		z = 15;
	return z;
}

void command_fix( cSocket *s )
// Try to compensate for messed up Z coordinates. Use this if you find yourself half-embedded in the ground.
{ // Rewrite to do it properly
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( Commands->NumArguments() == 2 )
	{
		if( validTelePos( s ) == -1 )
		{
			mChar->SetDispZ( static_cast<SI08>(Commands->Argument( 1 )) );
			mChar->SetZ( static_cast<SI08>(Commands->Argument( 1 )) );
		}
		else
		{
			mChar->SetDispZ( validTelePos( s ) );
			mChar->SetZ( validTelePos( s ) );
		}
	}
}

void command_xgoplace( cSocket *s )
// (d) Send another character to a location in your LOCATIONS.SCP file.
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() == 2 )
	{
		MakePlace( s, Commands->Argument( 1 ) );
		if( s->AddX( 0 ) != 0 )
			s->target( 0, TARGET_XGO, 20 );
	}
}

void command_showids( cSocket *s )
// Display the serial number of every item on your screen.
{
	VALIDATESOCKET( s );
	CChar *mChar	= s->CurrcharObj();
	SubRegion *Cell = MapRegion->GetCell( mChar->GetX(), mChar->GetY(), mChar->WorldNumber() );
	if( Cell == NULL )	// nothing to show
		return;
	Cell->PushChar();
	for( CChar *toShow = Cell->FirstChar(); !Cell->FinishedChars(); toShow = Cell->GetNextChar() )
	{
		if( !ValidateObject( toShow ) )
			continue;
		if( charInRange( mChar, toShow ) )
			s->ShowCharName( toShow, true );
	}
	Cell->PopChar();
}

void command_poly( cSocket *s )
// (h h) Polymorph yourself into any other creature.
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( Commands->NumArguments() < 2 || Commands->NumArguments() > 3 || !mChar )
		return;
	UI16 targBody = INVALIDID;
	if( Commands->NumArguments() == 2 )
		targBody = static_cast<UI16>(Commands->Argument( 1 ));
	else if( Commands->NumArguments() == 3 )
		targBody = static_cast<UI16>((Commands->Argument( 1 ) << 8 ) + Commands->Argument( 2 ));

	if( targBody <= 0x7CF )	// body-values > 0x3E crash the client
	{
		mChar->SetID( targBody );
		mChar->SetOrgID( targBody );

		// if polymorphing from monster to player we have to switch from transparent to semi-transparent
		// or we have that sit-down-client crash
		UI16 c1 = mChar->GetSkin();
		UI16 b	= c1&0x4000;
		if( b == 16384 && targBody >= 0x0190 && targBody <= 0x03E1 )
		{
			if( c1 != 0x8000 )
			{
				mChar->SetSkin( 0xF000 );
				mChar->SetOrgSkin( 0xF000 );
			}
		}
	}
}

void command_skin( cSocket *s )
// (h h) Change the hue of your skin.
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( Commands->NumArguments() == 2 && ValidateObject( mChar ) )
	{
		int body = mChar->GetID();
		COLOUR k = (COLOUR)Commands->Argument( 1 );
		int b = k&0x4000;
		if( b == 16384 && ( body >= 0x0190 && body <= 0x03E1 ) )
			k = 0xF000;
		
		if( k != 0x8000 )
		{
			mChar->SetSkin( k );
			mChar->SetOrgSkin( k );
		}
	}
}

void command_xtele( cSocket *s )
// (d / h h h h / nothing) Teleport a player to your position.
// <UL><LI>If you specify nothing (/XTELE), you click on the player to teleport in.</LI>
// <LI>If you specify a serial number (/XTELE .. .. .. ..), you teleport that player to you.</LI>
// <LI>If you specify a single number (/XTELE ..), you teleport the player logged in
// under that slot to you.</LI>
// </UL>
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() == 5 || Commands->NumArguments() == 2 )
		Targ->XTeleport( s, static_cast<UI08>(Commands->NumArguments() ));
	else
		s->target( 0, TARGET_XTELEPORT, 21 );
}

void command_go( cSocket *s )
// (d d d) Go to the specified X/Y/Z coordinates
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( Commands->NumArguments() > 2 )
	{
		UString upperCommand = Commands->CommandString( 2, 2 ).upper();
		if( upperCommand == "PLACE" )
		{
			if( Commands->NumArguments() == 3 )
			{
				MakePlace( s, Commands->Argument( 2 ) );
				if( s->AddX( 0 ) != 0 )
					mChar->SetLocation( s->AddX( 0 ), s->AddY( 0 ), s->AddZ(), s->AddID4() );
			}
		}
		else if( upperCommand == "CHAR" )
		{
			if( Commands->NumArguments() == 6 )
			{
				SERIAL serial = calcserial( static_cast<UI08>(Commands->Argument( 2 )), static_cast<UI08>(Commands->Argument( 3 )), static_cast<UI08>(Commands->Argument( 4 )), static_cast<UI08>(Commands->Argument( 5 )) );
				CChar *iTo = calcCharObjFromSer( serial );
				if( ValidateObject( iTo ) )
				{
					if( iTo->GetCommandLevel() > mChar->GetCommandLevel() )
					{
						s->sysmessage( 19 );
						return;
					}
					if( iTo->WorldNumber() != mChar->WorldNumber() )
					{
						mChar->SetLocation( iTo );
						SendMapChange( iTo->WorldNumber(), s );
					}
					else
						mChar->SetLocation( iTo );
				}
				else
					s->sysmessage( 1110 );
			}
			else if( Commands->NumArguments() == 3 )
			{
				cSocket *tSock = Network->GetSockPtr( Commands->Argument( 2 ) );
				if( tSock != NULL )
				{
					CChar *tChar = tSock->CurrcharObj();
					if( ValidateObject( tChar ) )
					{
						if( tChar->GetCommandLevel() > mChar->GetCommandLevel() )
						{
							s->sysmessage( 19 );
							return;
						}
						if( tChar->WorldNumber() != mChar->WorldNumber() )
						{
							mChar->SetLocation( tChar );
							SendMapChange( tChar->WorldNumber(), s );
						}
						else
							mChar->SetLocation( tChar );
					}
					else
						s->sysmessage( 1110 );
				}
				else
					s->sysmessage( 1745 );
			}
		}
		else if( Commands->NumArguments() == 4 )
		{
			SI16 x = static_cast< SI16 >(Commands->Argument( 1 ));
			SI16 y = static_cast< SI16 >(Commands->Argument( 2 ));
			SI08 z = static_cast< SI08 >(Commands->Argument( 3 ));
			if( x < 6144 && y < 4096 )
				mChar->SetLocation( x, y, z );
		}
	}
}

bool ZeroKillsFunctor( cBaseObject *a, UI32 &b, void *extraData )
{
	if( ValidateObject( a ) )
	{
		CChar *i = static_cast< CChar * >(a);
		i->SetKills( 0 );
		setcharflag( i );
	}
	return true;
}

void command_zerokills( cSocket *s )
// Sets all PK counters to 0.
{
	VALIDATESOCKET( s );
	s->sysmessage( 22 );
	UI32 b		= 0;
	ObjectFactory::getSingleton().IterateOver( OT_CHAR, b, NULL, &ZeroKillsFunctor );
	s->sysmessage( 23 );
}

void command_tile( cSocket *s )
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
				{
					a->SetPriv( 0 ); //Make them not decay
					a->SetLocation( x, y, z );
				}
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

bool WipeObjFunctor( cBaseObject *a, UI32 &b, void *extraData )
{
	bool retVal = true;
	if( ValidateObject( a ) )
	{
		if( (*((bool *)extraData)) )
		{
			CItem *i = static_cast< CItem * >(a);
			if( i->GetCont() == NULL && i->isWipeable() && i->GetType() != IT_GUILDSTONE )
			{
				i->Delete();
				++b;
			}
		}
		else
		{
			CChar *j = static_cast< CChar * >(a);
			if( j->IsNpc() && j->GetNPCAiType() != aiPLAYERVENDOR && !j->IsTamed() ) // PV and pets don't wipe
			{
				j->Delete();
				++b;
			}
		}
	}
	return retVal;
}

void command_wipe( cSocket *s )
// (d d d d / nothing) Deletes ALL NPC's and items inside a specified square.
// <UL><LI>With no arguments, /WIPE will ask you to click in opposing corners of
// the square.</LI>
// <LI>You may also specify coordinates - X1, Y1, X2, Y2.</LI>
// </UL>
{
	VALIDATESOCKET( s );
	s->AddID1( 0 );
	if( Commands->NumArguments() == 1 )
	{
		s->ClickX( -1 );
		s->ClickY( -1 );
		s->target( 0, TARGET_WIPING, 25 );	// 199 didn't seem taken...
	}
	else if( Commands->NumArguments() == 2 )
	{
		UString upperCommand	= Commands->CommandString( 2, 2 ).upper();
		bool saidAll			= ( upperCommand == "ALL" );
        CChar *mChar			= s->CurrcharObj();
		UI32 b					= 0;
		bool isItem				= true;
		if( saidAll || upperCommand == "ITEMS" ) // Really should warn that this will wipe ALL objects...
		{
			Console << mChar->GetName() << " has initiated an item wipe" << myendl;
			isItem = true;
			ObjectFactory::getSingleton().IterateOver( OT_ITEM, b, &isItem, &WipeObjFunctor );
			sysBroadcast( Dictionary->GetEntry( 365 ) );
		}
		if( saidAll || upperCommand == "NPCS" )
		{
			int deleted = 0;
			Console.Print( Dictionary->GetEntry( 81 ).c_str(), mChar->GetName().c_str() );
			isItem = false;
			ObjectFactory::getSingleton().IterateOver( OT_CHAR, b, &isItem, &WipeObjFunctor );
			Console.Print( "'WIPE' Deleted: %i npcs\n", b );
			sysBroadcast( Dictionary->GetEntry( 82 ) );
		}
	}
	else if( Commands->NumArguments() == 5 )
	{ // Wipe according to world coordinates
		s->ClickX( (SI16)Commands->Argument( 1 ) );
		s->ClickY( (SI16)Commands->Argument( 2 ) );
		s->SetWord( 11, static_cast<UI16>(Commands->Argument( 3 )) );
		s->SetWord( 13, static_cast<UI16>(Commands->Argument( 4 )) );
		Targ->Wiping( s );
	}
}

void command_areaCommand( cSocket *s )
{
	VALIDATESOCKET( s );
	if( !Commands->CommandString( 2 ).empty() )
	{
		s->XText( Commands->CommandString( 2 ).upper() );
		s->ClickX( -1 );
		s->ClickY( -1 );
		s->target( 0, TARGET_AREACOMMAND, 25 );
	}
	else
	{
		s->sysmessage( "Area command requires a subcommand!" );
	}
}


void command_iwipe( cSocket *s )
// (d d d d / nothing) Deletes ALL NPC's and items NOT inside a specified square.
// <UL><LI>With no arguments, /IWIPE will ask you to click in opposing corners of
// the square.</LI>
// <LI>You may also specify coordinates - X1, Y1, X2, Y2.</LI>
// </UL>
{
	VALIDATESOCKET( s );
	s->AddID1( 1 );
	if( Commands->NumArguments() == 1 )
	{
		s->ClickX( -1 );
		s->ClickY( -1 );
		s->target( 0, TARGET_WIPING, 26 );
	}
	else if( Commands->NumArguments() == 5 )
	{ // Wipe according to world coordinates
		s->ClickX( (SI16)Commands->Argument( 1 ) );
		s->ClickY( (SI16)Commands->Argument( 2 ) );
		s->SetWord( 11, static_cast<UI16>(Commands->Argument( 3 )));
		s->SetWord( 13, static_cast<UI16>(Commands->Argument( 4 )));
		Targ->Wiping( s );
	}
}

void command_add( cSocket *s )
{
	VALIDATESOCKET( s );

	UString UTag = Commands->CommandString( 2, 2 ).upper();
	if( Commands->NumArguments() == 1 )
		BuildAddMenuGump( s, 1 );
	else if( UTag == "NPC" )
	{
		if( Commands->NumArguments() == 4 )
		{
			s->AddID1( (UI08)Commands->Argument( 1 ) );
			s->AddID2( (UI08)Commands->Argument( 2 ) );
			s->target( 0, TARGET_ADDNPC, 1212, UString::number((s->AddID1()<<8) + s->AddID2()).c_str() );
		}
		else
		{
			s->XText( Commands->CommandString( 3 ).upper() );
			s->target( 0, TARGET_ADDSCRIPTNPC, 1212, s->XText().c_str() );
		}
	}
	else if( UTag == "ITEM" )
	{
		if( Commands->NumArguments() == 4 )
		{
			s->AddID1( (UI08)Commands->Argument( 2 ) );
			s->AddID2( (UI08)Commands->Argument( 3 ) );
			if( s->AddID1() < 0x40 )
				s->target( 0, TARGET_ADDITEM, 37, UString::number((s->AddID1()<<8) + s->AddID2()).c_str() );
		}
		else
		{
			s->XText( Commands->CommandString( 3 ).upper() );
			s->target( 0, TARGET_ADDSCRIPTITEM, 37, s->XText().c_str() );
		}
	}
	else if( UTag == "SPAWNER" )
	{
		if( Commands->NumArguments() == 4 )
		{
			s->AddID1( (UI08)Commands->Argument( 2 ) );
			s->AddID2( (UI08)Commands->Argument( 3 ) );
			if( s->AddID1() < 0x40 )
				s->target( 0, TARGET_ADDSPAWNER, 37, UString::number((s->AddID1()<<8) + s->AddID2()).c_str() );
		}
		else
		{
			s->XText( Commands->CommandString( 3 ).upper() );
			s->target( 0, TARGET_ADDSCRIPTSPAWNER, 37, s->XText().c_str() );
		}
	}
	else if( Commands->NumArguments() == 2 && Commands->Argument( 1 ) < 0x4000 )
	{
		s->AddID1( (UI08)(Commands->Argument( 1 )>>8) );
		s->AddID2( (UI08)(Commands->Argument( 1 )%256) );
		s->target( 0, TARGET_ADDITEM, 37, UString::number( Commands->Argument( 1 ) ) );
	}
}

void command_addx( cSocket *s )
// (h h) Adds a new item to your current location.
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return;

	UI16 addID = INVALIDID;
	SI08 targZ = mChar->GetZ();
	switch( Commands->NumArguments() )
	{
		case 2:	addID = static_cast<UI16>(Commands->Argument( 1 ));	break;
		case 4:	targZ = static_cast<SI08>(Commands->Argument( 3 ));
		case 3: addID = static_cast<UI16>( ( Commands->Argument( 1 ) << 8 ) + Commands->Argument( 2 ) );	break;
	}
	CTile tile;
	Map->SeekTile( addID, &tile );
	CItem *c = Items->CreateItem( NULL, mChar, addID, 1, 0, OT_ITEM );
	if( c != NULL )
	{
		c->SetLocation( mChar->GetX(), mChar->GetY(), targZ );
		c->SetPriv( 0 );
	}
}

void command_save( void )
// Saves the current world data into ITEMS.WSC and CHARS.WSC.
{
	if( cwmWorldState->GetWorldSaveProgress() != SS_SAVING )
		cwmWorldState->SaveNewWorld( true );
}

void command_dye( cSocket *s )
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

void command_showtime( cSocket *s )
// Displays the current UO time.
{
	VALIDATESOCKET( s );
	bool ampm	= cwmWorldState->ServerData()->ServerTimeAMPM();
	UI08 hour	= cwmWorldState->ServerData()->ServerTimeHours();
	UI08 minute	= cwmWorldState->ServerData()->ServerTimeMinutes();
	if( ampm || ( !ampm && hour == 12 ) )
		s->sysmessage( "%s %2.2d %s %2.2d %s", "Time: ", hour, ":", minute, "PM" );
	else
		s->sysmessage( "%s %2.2d %s %2.2d %s", "Time: ", hour, ":", minute, "AM" );
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
				cwmWorldState->ServerData()->ServerTimeHours( newhours - 12 );
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

void command_wholist( cSocket *s )
// Brings up an interactive listing of online users.
{
	VALIDATESOCKET( s );
	if( Commands->CommandString( 2, 2 ).upper() == "OFF" )
		OffList->SendSocket( s );
	else
		WhoList->SendSocket( s );
}

void command_sfx( cSocket *s )
// (h h) Plays the specified sound effect.
{
	VALIDATESOCKET( s );
	switch( Commands->NumArguments() )
	{
		case 2:	Effects->PlaySound( s, static_cast<UI16>(Commands->Argument( 1 )), true );								break;
		case 3:	Effects->PlaySound( s, static_cast<UI16>((Commands->Argument( 1 ) << 8 ) + Commands->Argument( 2 )), true );	break;
	}
}

void command_disconnect( void )
// (d) Disconnects the user logged in under the specified slot.
{
	if( Commands->NumArguments() == 2 )
		Network->Disconnect( Commands->Argument( 1 ) );
}

void command_tell( cSocket *s )
// (d text) Sends an anonymous message to the user logged in under the specified slot.
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() > 2 )
	{
		cSocket *i = Network->GetSockPtr( Commands->Argument( 1 ) );
		std::string txt = Commands->CommandString( 3 );
		if( i == NULL || txt.empty() )
			return;

		CChar *mChar		= i->CurrcharObj();
		CChar *tChar		= s->CurrcharObj();
		std::string temp	= mChar->GetName() + " tells " + tChar->GetName() + ": " + txt;
		CSpeechEntry *toAdd	= SpeechSys->Add();
		toAdd->Font( (FontType)mChar->GetFontType() );
		toAdd->Speech( temp );
		toAdd->Speaker( mChar->GetSerial() );
		toAdd->SpokenTo( tChar->GetSerial() );
		toAdd->Type( TALK );
		toAdd->At( cwmWorldState->GetUICurrentTime() );
		toAdd->TargType( SPTRG_INDIVIDUAL );
		toAdd->Colour( mChar->GetSayColour() );
	}
}

void command_gmmenu( cSocket *s )
// (d) Opens the specified GM Menu.
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() == 2 )
	{
		CPIHelpRequest toHandle( s, static_cast<UI16>(Commands->Argument( 1 ) ) );
		toHandle.Handle();
	}
}

void command_itemmenu( cSocket *s )
// (d) Opens the specified Item Menu from ITEMS.SCP.
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() == 2 )
		BuildAddMenuGump( s, static_cast<UI16>(Commands->Argument( 1 )) );
}

void command_dupe( cSocket *s )
// (d / nothing) Duplicates an item. If a parameter is specified, it's how many copies to make.
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() == 2 )
		s->AddID1( (UI08)Commands->Argument( 1 ) );
	else
		s->AddID1( 1 );
	s->target( 0, TARGET_DUPE, 38 );
}


void command_command( cSocket *s )
// Executes a trigger scripting command.
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() > 1 )
		HandleGumpCommand( s, Commands->CommandString( 2, 2 ).upper(), Commands->CommandString( 3 ).upper() );
}

void command_allmove( cSocket *s )
// Enables GM ability to pick up all objects.
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( Commands->CommandString( 2, 2 ).upper() == "ON" )
	{
		mChar->SetAllMove( true );
		mChar->Teleport();
		s->sysmessage( 39 );
	}
	else if( Commands->CommandString( 2, 2 ).upper() == "OFF" )
	{
		mChar->SetAllMove( false );
		mChar->Teleport();
		s->sysmessage( 40 );
	}
}

void command_houseicons( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( Commands->CommandString( 2, 2 ).upper() == "ON" )
	{
		// Makes houses appear as deeds. (The walls disappear and there's a deed on the ground in their place.)
		mChar->SetViewHouseAsIcon( true );
		mChar->Teleport();
		s->sysmessage( 41 );
	}
	else if( Commands->CommandString( 2, 2 ).upper() == "OFF" )
	{
		mChar->SetViewHouseAsIcon( false );
		mChar->Teleport();
		s->sysmessage( 42 );
	}
	mChar->Teleport();
}

//o--------------------------------------------------------------------------
//|	Function		-	void command_set( cSocket *s )
//|	Date			-	February 22, 2002
//|	Programmer		-	DarkStorm
//|	Modified		-	4/5/2003 - giwo
//o--------------------------------------------------------------------------
//|	Purpose			-	This Command is used for ingame manipulation of 
//|						items and characters, it'll set different kinds
//|						of properties for both chars and items. 
//o--------------------------------------------------------------------------
void command_set( cSocket *s )
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() != 3 )
	{
		s->sysmessage( "Usage: set <field> <value>" );
		return;
	}

	UString temp = Commands->CommandString( 2, 3 );
	Commands->Log( "/set", s->CurrcharObj(), NULL, temp );
	s->XText( temp );

	s->target( 0, TARGET_ALLSET, 1741, temp.c_str() );
}

void command_readini( cSocket *s )
// Re-loads the UOX3.INI file.
{
	VALIDATESOCKET( s );
	cwmWorldState->ServerData()->load();
	s->sysmessage( 45 );
}

void command_gumpmenu( cSocket *s )
// (d) Opens the specified GUMP menu.
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() == 2 )
		BuildGumpFromScripts( s, (SI16)Commands->Argument( 1 ) );
}

void command_memstats( cSocket *s )
// Display some information about the cache.
{
	VALIDATESOCKET( s );
	size_t cacheSize		= Map->TileMem + Map->StaMem + Map->versionMemory + 9*MAP0CACHE;
	size_t charsSize		= ObjectFactory::getSingleton().CountOfObjects( OT_CHAR ) * 4;
	size_t itemsSize		= ObjectFactory::getSingleton().CountOfObjects( OT_ITEM ) * 4;
	size_t spellsSize		= 69 * sizeof( SpellInfo );
	size_t teffectsSize		= sizeof( CTEffect ) * TEffects->Count();
	size_t regionsSize		= sizeof( cTownRegion ) * regions.size();
	size_t spawnregionsSize = sizeof( cSpawnRegion ) * spawnregions.size();
	size_t total			= charsSize + itemsSize + spellsSize + cacheSize + regionsSize + spawnregionsSize + teffectsSize;
	GumpDisplay cacheStats( s, 350, 345 );
	cacheStats.SetTitle( "UOX Memory Information" );
	cacheStats.AddData( "Total Memory Usage: ", total );
	cacheStats.AddData( " Cache Size: ", cacheSize );
	cacheStats.AddData( "  Tiles: ", Map->TileMem );
	cacheStats.AddData( "  Statics: ", Map->StaMem );
	cacheStats.AddData( "  Version: ", Map->versionMemory );
	cacheStats.AddData( "  Map0: ", 9*MAP0CACHE );
	cacheStats.AddData( "   Hits: ", Map->Map0CacheHit );
	cacheStats.AddData( "   Misses: ", Map->Map0CacheMiss );
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
	cacheStats.AddData( "  CTownRegion: ", sizeof( cTownRegion ) );
	cacheStats.AddData( " SpawnRegions Size ", spawnregionsSize );
	cacheStats.AddData( "  SpawnRegions: ", spawnregions.size() );
	cacheStats.AddData( "  CSpawnRegion: ", sizeof( cSpawnRegion ) );
	cacheStats.Send( 0, false, INVALIDSERIAL );

}

void command_npcbound( cSocket *s )
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() == 5 )	// Box
	{
		s->AddX( 0, static_cast<SI16>(Commands->Argument( 1 )) );
		s->AddY( 0, static_cast<SI16>(Commands->Argument( 2 )) );
		s->AddX( 1, static_cast<SI16>(Commands->Argument( 3 )) );
		s->AddY( 1, static_cast<SI16>(Commands->Argument( 4 )) );
		s->target( 0, TARGET_NPCRECT, 46 );
	}
	else if( Commands->NumArguments() == 4 )	// Circle
	{
		s->AddX( 0, static_cast<SI16>(Commands->Argument( 1 )) );
		s->AddY( 0, static_cast<SI16>(Commands->Argument( 2 )) );
		s->AddX( 1, static_cast<SI16>(Commands->Argument( 3 )) );
		s->target( 0, TARGET_NPCCIRCLE, 47 );
	}
}

void command_secondsperuominute( cSocket *s )
// (d) Sets the number of real-world seconds that pass for each UO minute.
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() == 2 )
	{
		cwmWorldState->ServerData()->ServerSecondsPerUOMinute( static_cast<UI16>(Commands->Argument( 1 )) );
		s->sysmessage( 49 );
	}
}

void command_brightlight( cSocket *s )
// (h) Sets default daylight level.
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() == 2 )
	{
		cwmWorldState->ServerData()->WorldLightBrightLevel( static_cast<UI08>(Commands->Argument( 1 )) );
		s->sysmessage( 50 );
	}
}

void command_darklight( cSocket *s )
// (h) Sets default nighttime light level.
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() == 2 )
	{
		cwmWorldState->ServerData()->WorldLightDarkLevel( static_cast<UI08>(Commands->Argument( 1 )) );
		s->sysmessage( 51 );
	}
}

void command_dungeonlight( cSocket *s )
// (h) Sets default dungeon light level.
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() == 2 )
	{
		cwmWorldState->ServerData()->DungeonLightLevel( static_cast<LIGHTLEVEL>(Commands->Argument( 1 )) );
		s->sysmessage( 52 );
	}
}

void command_gmopen( cSocket *s )
// (h / nothing) Opens specified layer on player/NPC, or player's pack if no layer specified.
// Useful hex codes for this command are:
// 0x15	Backpack
// 0x1A	NPC Buy Restock container
// 0x1B	NPC Buy no restock container
// 0x1C	NPC Sell container
// 0x1D	Bank Box
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() == 2 )
		s->TempInt( Commands->Argument( 1 ) );
	else
		s->TempInt( 0x15 );
	s->target( 0, TARGET_GMOPEN, 53 );
}

void command_restock( cSocket *s )
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

void command_setshoprestockrate( cSocket *s )
// (d) Sets the universe's shop restock rate.
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() == 2 )
	{
		cwmWorldState->ServerData()->SystemTimer( SHOP_SPAWN, static_cast<UI16>(Commands->Argument( 1 )) );
		s->sysmessage( 56 );
	}
	else
		s->sysmessage( 57 );
}

void command_midi( cSocket *s )
// (d d) Plays the specified MIDI file.
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() == 2 )
		Effects->playMidi( s, static_cast<UI16>(Commands->Argument( 1 )) );
	else if( Commands->NumArguments() == 3 )
		Effects->playMidi( s, static_cast<UI16>( ( Commands->Argument( 1 ) << 8 ) + Commands->Argument( 2 ) ) );
}

bool RespawnFunctor( cBaseObject *a, UI32 &b, void *extraData )
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
	std::vector< cSpawnRegion * >::iterator spawnCounter;
	for( spawnCounter = spawnregions.begin(); spawnCounter != spawnregions.end(); ++spawnCounter )
	{
		cSpawnRegion *spawnReg = (*spawnCounter);
		if( spawnReg != NULL )
			spawnReg->doRegionSpawn( spawnedItems, spawnedNpcs );
	}

	UI32 b		= 0;
	ObjectFactory::getSingleton().IterateOver( OT_ITEM, b, NULL, &RespawnFunctor );
}

// (s/d) Forces a region spawn, First argument is region number or "ALL"
void command_regspawn( cSocket *s )
{
	VALIDATESOCKET( s );

	if( Commands->NumArguments() == 2 )
	{
		UI16 itemsSpawned	= 0;
		UI16 npcsSpawned	= 0;

		if( Commands->CommandString( 2, 2 ).upper() == "ALL" )
		{
			std::vector< cSpawnRegion * >::iterator spawnCounter;
			for( spawnCounter = spawnregions.begin(); spawnCounter != spawnregions.end(); ++ spawnCounter )
			{
				cSpawnRegion *spawnReg = (*spawnCounter);
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
				cSpawnRegion *spawnReg = spawnregions[spawnRegNum];
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

void command_cq( cSocket *s )
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
		showQue( s, false );	// Show the Counselor queue, not GM queue
}

void command_gq( cSocket *s )
// Display the GM queue.
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() == 2 )
	{
		CChar *mChar = s->CurrcharObj();
		UString upperCommand = Commands->CommandString( 2, 2 ).upper();
		if( upperCommand == "NEXT" )
			nextCall( s, true );
		else if( upperCommand == "CLEAR" )
			closeCall( s, true );
		else if( upperCommand == "CURR" )
			currentCall( s, true );
	}
	else
		showQue( s, true );	// Show the Counselor queue, not GM queue
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

void command_wf( cSocket *s )
// Make the specified item worldforge compatible.
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() == 2 )
	{
		s->XText( "TYPE 0xFF" );
		s->target( 0, TARGET_ALLSET, 65 );
	}
}

bool KillAllFunctor( cBaseObject *a, UI32 &b, void *extraData )
{
	bool retVal = true;
	if( ValidateObject( a ) )
	{
		CChar *i = static_cast< CChar * >(a);
		if( i->IsNpc() )
		{
			if( RandomNum( 0, 99 ) + 1 <= (*((SI32 *)extraData)) )
			{
				Effects->bolteffect( i );
				Effects->PlaySound( i, 0x0029 );
				doDeathStuff( i );
			}
		}
	}
	return retVal;
}

void command_killall( cSocket *s )
// (d text) Kills all of a specified item.
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() > 2 )
	{
		SI32 percent		= Commands->Argument( 1 );
		std::string sysmsg	= Commands->CommandString( 3 );
		s->sysmessage( 358 );
		sysBroadcast( sysmsg );
		UI32 b		= 0;
		ObjectFactory::getSingleton().IterateOver( OT_CHAR, b, &percent, &KillAllFunctor );
		s->sysmessage( "Done." );
	}
}

void command_pdump( cSocket *s )
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

void command_gy( cSocket *s )
// (text) GM Yell - Announce a message to all online GMs.
{
	VALIDATESOCKET( s );

	if( cwmWorldState->GetPlayersOnline() == 1 )
	{
		s->sysmessage( 69 );
		return;
	}
	CChar *me = s->CurrcharObj();
	if( !ValidateObject( me ) )
		return;

	UString tmpString = UString::sprintf( "%s (GM ONLY): %s", me->GetName(), Commands->CommandString( 2 ).c_str() );

	CSpeechEntry *toAdd = SpeechSys->Add();
	toAdd->Speech( tmpString );
	toAdd->Font( (FontType)me->GetFontType() );
	toAdd->Speaker( INVALIDSERIAL );
	toAdd->Colour( s->GetWord( 4 ) );
	toAdd->Type( SYSTEM );
	toAdd->At( cwmWorldState->GetUICurrentTime() );
	toAdd->TargType( SPTRG_BROADCASTALL );
	toAdd->CmdLevel( 2 );
}

void command_spawnkill( cSocket *s )
// (d) Kills spawns from the specified spawn region in SPAWN.SCP.
{
	VALIDATESOCKET( s );
	if( Commands->NumArguments() == 2 )
	{
		UI16 regNum = static_cast<UI16>(Commands->Argument( 1 ));
		if( regNum >= spawnregions.size() )
			return;
		cSpawnRegion *spawnReg = spawnregions[regNum];
		if( spawnReg == NULL )
			return;
		int killed	= 0;

		s->sysmessage( 349 );
		for( CChar *i = spawnReg->FirstChar(); !spawnReg->FinishedChars(); i = spawnReg->NextChar() )
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

void command_who( cSocket *s )
// Displays a list of users currently online.
{
	VALIDATESOCKET( s );
	if( cwmWorldState->GetPlayersOnline() == 1 )
	{
		s->sysmessage( 69 );
		return;
	}
	int j = 0;

	Network->PushConn();
	s->sysmessage( "Who's online:" );
	for( cSocket *iSock = Network->FirstSocket(); !Network->FinishedSockets(); iSock = Network->NextSocket() )
	{
		CChar *iChar = iSock->CurrcharObj();
		++j;
		s->sysmessage( "%i) %s", ( j - 1 ), iChar->GetName().c_str() );
	}
	Network->PopConn();
	s->sysmessage( "Total: %i users online.", j );
}

void command_gms( cSocket *s )
{
	VALIDATESOCKET( s );
	int j = 0;
	GumpDisplay Who( s, 400, 300 );
	char temp[512];
	Who.SetTitle( Dictionary->GetEntry( 77, s->Language() ) );
	Network->PushConn();
	for( cSocket *iSock = Network->FirstSocket(); !Network->FinishedSockets(); iSock = Network->NextSocket() )
	{
		CChar *iChar = iSock->CurrcharObj();
		++j;
		if( iChar->GetCommandLevel() >= CNS_CMDLEVEL )
		{
			sprintf( temp, "%i) %s", ( j - 1 ), iChar->GetName().c_str() );
			Who.AddData( temp, iChar->GetSerial(), 3 );
		}
	}
	Network->PopConn();
	Who.Send( 4, false, INVALIDSERIAL );
}

bool CleanUpFunctor( cBaseObject *a, UI32 &b, void *extraData )
{
	bool retVal = true;
	if( ValidateObject( a ) )
	{
		CItem *i = static_cast< CItem * >(a);
		if( i->isCorpse() || i->GetType() == IT_GATE || i->GetType() == IT_ENDGATE )
		{
			i->Delete();
			++b;
		}
	}
	return retVal;
}
void command_cleanup( cSocket *s )
{
	VALIDATESOCKET( s );
	int corpses = 0;
	s->sysmessage( 83 );
	UI32 b		= 0;
	ObjectFactory::getSingleton().IterateOver( OT_ITEM, b, NULL, &CleanUpFunctor );
	corpses		= b;
	s->sysmessage( 84 );
	s->sysmessage( 85, corpses );
}

//o--------------------------------------------------------------------------o
//|	Function			-	void command_reloadaccounts( cSocket *s )
//|	Date					-	
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Reload accounts from the console interface.
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
void command_reloadaccounts( cSocket *s )
{
	VALIDATESOCKET( s );
	if( Accounts->Load() )
		s->sysmessage( 86 );
}

void command_reportbug( cSocket *s )
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
	for( cSocket *iSock = Network->FirstSocket(); !Network->FinishedSockets(); iSock = Network->NextSocket() )
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

bool DelIDFunctor( cBaseObject *a, UI32 &b, void *extraData )
{
	if( ValidateObject( a ) )
	{
		if( a->GetID() == (*((UI16 *)extraData)) )
			a->Delete();
	}
	return true;
}

void command_delid( cSocket *s )
// (h h) Deletes all items in the world with a particular id
{
	VALIDATESOCKET( s );
	UI16 tID = INVALIDID;
	switch( Commands->NumArguments() )
	{
		case 3:		tID = static_cast<UI16>( (Commands->Argument( 1 )<<8) + Commands->Argument( 2 ) );	break;
		case 2:		tID = static_cast<UI16>( Commands->Argument( 1 ) );										break;
		default:	s->sysmessage( 1634 );															return;
	}

	UI32 b		= 0;
	ObjectFactory::getSingleton().IterateOver( OT_ITEM, b, NULL, &DelIDFunctor );
}

void command_forcewho( cSocket *s )
// Brings up an interactive listing of online users.
{
	VALIDATESOCKET( s );
	WhoList->ZeroWho();
	WhoList->SendSocket( s );
}

void command_cy( cSocket *s )
// (text) Counselor Yell - Announce a message to all online counselors and above.
{
	VALIDATESOCKET( s );
	if( cwmWorldState->GetPlayersOnline() == 1 )
	{
		s->sysmessage( 69 );
		return;
	}
	UString tmpString = UString::sprintf( "(CY): %s", Commands->CommandString( 2 ).c_str() );

	CChar *me			= s->CurrcharObj();
	CSpeechEntry *toAdd = SpeechSys->Add();
	toAdd->Speech( tmpString );
	toAdd->Font( (FontType)me->GetFontType() );
	toAdd->Speaker( me->GetSerial() );
	toAdd->Colour( s->GetWord( 4 ) );
	toAdd->Type( SYSTEM );
	toAdd->At( cwmWorldState->GetUICurrentTime() );
	toAdd->TargType( SPTRG_BROADCASTPC );
	toAdd->CmdLevel( 1 );
}
void command_sgy( cSocket *s )
// (text) GM Yell - Announce a message to all online GMs.
{
	VALIDATESOCKET( s );
	if( cwmWorldState->GetPlayersOnline() == 1 )
	{
		s->sysmessage( 69 );
		return;
	}
	UString tmpString = UString::sprintf( "(SuperGM): %s", Commands->CommandString( 2 ).c_str() );

	CChar *me			= s->CurrcharObj();
	CSpeechEntry *toAdd = SpeechSys->Add();
	toAdd->Speech( tmpString );
	toAdd->Font( (FontType)me->GetFontType() );
	toAdd->Speaker( me->GetSerial() );
	toAdd->Colour( s->GetWord( 4 ) );
	toAdd->Type( SYSTEM );
	toAdd->At( cwmWorldState->GetUICurrentTime() );
	toAdd->TargType( SPTRG_BROADCASTPC );
	toAdd->CmdLevel( 3 );
}

void command_validcmd( cSocket *s )
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

	targetCmds.Send( 4, false, INVALIDSERIAL );
}

void command_nacct( cSocket *s )
{
	VALIDATESOCKET( s );
	UString upperCommand = Commands->CommandString( 2, 2 ).upper();
	if( upperCommand == "ADD" )
		BuildGumpFromScripts( s, 8 );
	else if( upperCommand == "BAN" )
	{
	}
	else if( upperCommand == "EDIT" )
		BuildGumpFromScripts( s, 10 );
	else if( upperCommand == "VIEW" )
		BuildGumpFromScripts( s, 9 );
	else if( upperCommand == "LIST" )
	{
//		Accounts->SendAccountsAsGump( s );
	}
	else	// by default list them
		BuildGumpFromScripts( s, 7 );
}

void command_howto( cSocket *s )
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

// command_xgate - Gates from locations.scp - Retalin 9/17/2001
void command_xgate( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return;
	if( Commands->NumArguments() == 2 )
	{
		MakePlace( s, Commands->Argument( 1 ) );
		if( s->AddX( 0 ) != 0 )
			SpawnGate( mChar, mChar->GetX(), mChar->GetY(), mChar->GetZ(), s->AddX( 0 ), s->AddY( 0 ), s->AddZ() );
	}
}

void command_reloaddefs( cSocket *s )
{
	VALIDATESOCKET( s );
	sysBroadcast( Dictionary->GetEntry( 1746 ) );
	s->sysmessage( 1747 );
	SpeechSys->Poll();
	delete FileLookup;
	s->sysmessage( 1748 );
	SpeechSys->Poll();
	if( ( FileLookup = new cServerDefinitions() ) == NULL )
	{
		sysBroadcast( Dictionary->GetEntry( 1749 ) );
		Shutdown( FATAL_UOX3_ALLOC_SCRIPTS );
	}
	else
		sysBroadcast( Dictionary->GetEntry( 1750 ) );
	SpeechSys->Poll();
}

void command_temp( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return;
	cTownRegion *reg	= mChar->GetRegion();
	weathID toGrab		= reg->GetWeather();
	if( toGrab != 0xFF )
	{
		R32 curTemp = Weather->Temp( toGrab );
		s->sysmessage( 1751, curTemp );
	}
}

void command_status( cSocket *s )
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
	TargetMap["ACTION"]			= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGETID1,	TARGET_NPCACT,			213);
	// B
	TargetMap["BAN"]			= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGET,		TARGET_BAN,				253);
	TargetMap["BOLT"]			= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGET,		TARGET_BOLT,			195);
	TargetMap["BUY"]			= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGET,		TARGET_BUYSHOP,			233);
	// C
	TargetMap["CSTATS"]			= TargetMapEntry( CNS_CMDLEVEL,		CMD_TARGET,		TARGET_CSTATS,			183);
	// D
	TargetMap["DELETECHAR"]		= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGET,		TARGET_DELETECHAR,		1618);
	// E
	// F
	TargetMap["FULLSTATS"]		= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGET,		TARGET_FULLSTATS,		243);
	// G
	TargetMap["GLOW"]			= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGET,		TARGET_GLOW,			258);
	TargetMap["GATE"]			= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGET,		TARGET_GATE,			226);
	// H
	TargetMap["HEAL"]			= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGET,		TARGET_HEAL,			227);
	TargetMap["HIDE"]			= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGET,		TARGET_PERMHIDE,		244);
	// I
	TargetMap["INCX"]			= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGETX,	TARGET_INCX,			254);
	TargetMap["INCY"]			= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGETX,	TARGET_INCY,			254);
	TargetMap["INCZ"]			= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGETX,	TARGET_INCZ,			268);
	TargetMap["ISTATS"]			= TargetMapEntry( CNS_CMDLEVEL,		CMD_TARGET,		TARGET_ISTATS,			182);
	// J
	TargetMap["JAIL"]			= TargetMapEntry( CNS_CMDLEVEL,		CMD_TARGET,		TARGET_JAIL,			180);
	// K
	TargetMap["KICK"]			= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGET,		TARGET_KICK,			196);
	TargetMap["KILL"]			= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGETTXT,	TARGET_KILL,			193);
	// L
	// M
	TargetMap["MAKE"]			= TargetMapEntry( ADMIN_CMDLEVEL,	CMD_TARGETTXT,	TARGET_MAKESTATUS,		279);
	TargetMap["MAKESHOP"]		= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGET,		TARGET_MAKESHOP,		232);
	TargetMap["MANA"]			= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGET,		TARGET_MANA,			230);
	TargetMap["MARK"]			= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGET,		TARGET_MARK,			225);
	TargetMap["MOVETOBAG"]		= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGET,		TARGET_MOVETOBAG,		197);
	TargetMap["MUTE"]			= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGETID1,	TARGET_SQUELCH,			71 );
	// N
	TargetMap["NPCTARGET"]		= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGET,		TARGET_NPCFOLLOW,		228);
	TargetMap["NPCWANDER"]		= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGETINT,	TARGET_NPCWANDER,		48);
	// O
	// P
	TargetMap["POSSESS"]		= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGET,		TARGET_POSSESS,			249);
	// Q
	// R
	TargetMap["RECALL"]			= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGET,		TARGET_RECALL,			224);
	TargetMap["RELEASE"]		= TargetMapEntry( CNS_CMDLEVEL,		CMD_TARGET,		TARGET_RELEASE,			181);
	TargetMap["REMOVE"]			= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGET,		TARGET_REMOVE,			188);
	TargetMap["RESURRECT"]		= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGET,		TARGET_RESURRECT,		194);
	TargetMap["REMOVESHOP"]		= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGET,		TARGET_REMOVESHOP,		262);
	// S
	TargetMap["SETSCPTRIG"]		= TargetMapEntry( ADMIN_CMDLEVEL,	CMD_TARGETINT,	TARGET_SETSCPTRIG,		267);
	TargetMap["SHOWSKILLS"]		= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGETINT,	TARGET_SHOWSKILLS,		260);
	TargetMap["SHOWDETAIL"]		= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGET,		TARGET_INFO,			261);
	TargetMap["SETPOISONED"]	= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGETINT,	TARGET_SETPOISONED,		240);
	TargetMap["SELL"]			= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGET,		TARGET_SELL,			236);
	TargetMap["STAMINA"]		= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGET,		TARGET_STAMINA,			231);
	TargetMap["SQUELCH"]		= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGETID1,	TARGET_SQUELCH,			71 );
	// T
	TargetMap["TELESTUFF"]		= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGET,		TARGET_TELESTUFF,		250);
	TargetMap["TWEAK"]			= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGET,		TARGET_TWEAK,			229);
	TargetMap["TELE"]			= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGET,		TARGET_TELE,			185);
	// U
	TargetMap["UNHIDE"]			= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGET,		TARGET_UNHIDE,			245);
	TargetMap["UNGLOW"]			= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGET,		TARGET_UNGLOW,			259);
	// V
	// W
	TargetMap["WSTATS"]			= TargetMapEntry( CNS_CMDLEVEL,		CMD_TARGET,		TARGET_WSTATS,			183);
	// X
	TargetMap["XBANK"]			= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGET,		TARGET_XBANK,			186);
	TargetMap["XGO"]			= TargetMapEntry( GM_CMDLEVEL,		CMD_TARGETXYZ,	TARGET_XGO,				198);
	// Y
	// Z

	// CommandMap[Command Name] = CommandMapEntry(Required Command Level, Command Type, Command Function);
	//A
	CommandMap["ADD"]				= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_add);
	CommandMap["ADDACCOUNT"]		= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_addaccount);
	CommandMap["ADDX"]				= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_addx);
	CommandMap["ALLMOVE"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_allmove);
	CommandMap["ANNOUNCE"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_FUNC,		(CMD_DEFINE)&command_announce);
	CommandMap["AREACOMMAND"]		= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_areaCommand);
	//B
	CommandMap["BRIGHTLIGHT"]		= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_brightlight);
	//C
	CommandMap["CY"]				= CommandMapEntry( CNS_CMDLEVEL,	CMD_SOCKFUNC,	(CMD_DEFINE)&command_cy);
	CommandMap["CLEANUP"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_cleanup);
	CommandMap["CQ"]				= CommandMapEntry( CNS_CMDLEVEL,	CMD_SOCKFUNC,	(CMD_DEFINE)&command_cq);
	CommandMap["COMMAND"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_command);
	//D
	CommandMap["DARKLIGHT"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_darklight);
	CommandMap["DUNGEONLIGHT"]		= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_dungeonlight);
	CommandMap["DUPE"]				= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_dupe);
	CommandMap["DISCONNECT"]		= CommandMapEntry( GM_CMDLEVEL,		CMD_FUNC,		(CMD_DEFINE)&command_disconnect);
	CommandMap["DYE"]				= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_dye);
	CommandMap["DELID"]				= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_delid);
	//E
	//F
	CommandMap["FORCEWHO"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_forcewho);
	CommandMap["FIX"]				= CommandMapEntry( CNS_CMDLEVEL,	CMD_SOCKFUNC,	(CMD_DEFINE)&command_fix);
	CommandMap["FIXSPAWN"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_FUNC,		(CMD_DEFINE)&command_fixspawn);
	//G,
	CommandMap["GETLIGHT"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_getlight);
	CommandMap["GY"]				= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_gy);
	CommandMap["GUARDS"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_FUNC,		(CMD_DEFINE)&command_guards);
	CommandMap["GMS"]				= CommandMapEntry( CNS_CMDLEVEL,	CMD_SOCKFUNC,	(CMD_DEFINE)&command_gms);
	CommandMap["GUMPMENU"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_gumpmenu);
	CommandMap["GMMENU"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_gmmenu);
	CommandMap["GO"]				= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_go);
	CommandMap["GMOPEN"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_gmopen);
	CommandMap["GCOLLECT"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_FUNC,		(CMD_DEFINE)&command_gcollect);
	CommandMap["GQ"]				= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_gq);
	//H
	CommandMap["HOUSEICONS"]		= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_houseicons);
	CommandMap["HOWTO"]				= CommandMapEntry( PLAYER_CMDLEVEL,	CMD_SOCKFUNC,	(CMD_DEFINE)&command_howto );
	//I
	CommandMap["IWIPE"]				= CommandMapEntry( ADMIN_CMDLEVEL,	CMD_SOCKFUNC,	(CMD_DEFINE)&command_iwipe);
	CommandMap["ITEMMENU"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_itemmenu);
	//J
	//K
	CommandMap["KILLALL"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_killall);
	//L
	CommandMap["LOADDEFAULTS"]		= CommandMapEntry( ADMIN_CMDLEVEL,	CMD_FUNC,		(CMD_DEFINE)&command_loaddefaults);
	//M
	CommandMap["MEMSTATS"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_memstats);
	CommandMap["MINECHECK"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_FUNC,		(CMD_DEFINE)&command_minecheck);
	CommandMap["MIDI"]				= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_midi);
	//N
	CommandMap["NACCT"]				= CommandMapEntry( ADMIN_CMDLEVEL,	CMD_SOCKFUNC,	(CMD_DEFINE)&command_nacct );
	CommandMap["NPCBOUND"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_npcbound);
	//O
	//P
	CommandMap["PDUMP"]				= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_pdump);
	CommandMap["POLY"]				= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_poly);
	CommandMap["POST"]    			= CommandMapEntry( CNS_CMDLEVEL,	CMD_SOCKFUNC,	(CMD_DEFINE)&command_post);
	//Q
	//R
	CommandMap["RESEND"]			= CommandMapEntry( CNS_CMDLEVEL,	CMD_SOCKFUNC,	(CMD_DEFINE)&command_resend);
	CommandMap["READINI"]			= CommandMapEntry( ADMIN_CMDLEVEL,	CMD_SOCKFUNC,	(CMD_DEFINE)&command_readini);
	CommandMap["RESTOCK"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_restock);
	CommandMap["RESPAWN"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_FUNC,		(CMD_DEFINE)&command_respawn);
	CommandMap["REGSPAWN"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_regspawn);
	CommandMap["RELOADACCOUNTS"]	= CommandMapEntry( ADMIN_CMDLEVEL,	CMD_SOCKFUNC,	(CMD_DEFINE)&command_reloadaccounts);
	CommandMap["REPORTBUG"]			= CommandMapEntry( PLAYER_CMDLEVEL,	CMD_SOCKFUNC,	(CMD_DEFINE)&command_reportbug);
	CommandMap["RELOADDEFS"]		= CommandMapEntry( ADMIN_CMDLEVEL,	CMD_SOCKFUNC,	(CMD_DEFINE)&command_reloaddefs );
	//S
	CommandMap["SGY"]				= CommandMapEntry( ADMIN_CMDLEVEL,	CMD_SOCKFUNC,	(CMD_DEFINE)&command_sgy);
	CommandMap["SETPOST"]			= CommandMapEntry( CNS_CMDLEVEL,	CMD_SOCKFUNC,	(CMD_DEFINE)&command_setpost);
	CommandMap["SETRACE"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_setrace);
	CommandMap["SPAWNKILL"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_spawnkill);
	CommandMap["SETSHOPRESTOCKRATE"]= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_setshoprestockrate);
	CommandMap["SECONDSPERUOMINUTE"]= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_secondsperuominute);
	CommandMap["SET"]				= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_set);
	CommandMap["SFX"]				= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_sfx);
	CommandMap["SHOWTIME"]			= CommandMapEntry( CNS_CMDLEVEL,	CMD_SOCKFUNC,	(CMD_DEFINE)&command_showtime);
	CommandMap["SETTIME"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_FUNC,		(CMD_DEFINE)&command_settime);
	CommandMap["SHUTDOWN"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_FUNC,		(CMD_DEFINE)&command_shutdown);
	CommandMap["SAVE"]				= CommandMapEntry( GM_CMDLEVEL,		CMD_FUNC,		(CMD_DEFINE)&command_save);
	CommandMap["SKIN"]				= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_skin);
	CommandMap["STATUS"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_status);
	CommandMap["SHOWIDS"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_showids);
	//T
	CommandMap["TEMP"]				= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_temp );
	CommandMap["TIME"]				= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_time);
	CommandMap["TELL"]				= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_tell);
	CommandMap["TILE"]				= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_tile);
	//U
	//V
	CommandMap["VALIDCMD"]			= CommandMapEntry( PLAYER_CMDLEVEL,	CMD_SOCKFUNC,	(CMD_DEFINE)&command_validcmd );
	//W
	CommandMap["WF"]				= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_wf);
	CommandMap["WHO"]				= CommandMapEntry( CNS_CMDLEVEL,	CMD_SOCKFUNC,	(CMD_DEFINE)&command_who);
	CommandMap["WHOLIST"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_wholist);
	CommandMap["WIPE"]				= CommandMapEntry( ADMIN_CMDLEVEL,	CMD_SOCKFUNC,	(CMD_DEFINE)&command_wipe);
	CommandMap["WHERE"]				= CommandMapEntry( CNS_CMDLEVEL,	CMD_SOCKFUNC,	(CMD_DEFINE)&command_where);
	//X
	CommandMap["XTELE"]				= CommandMapEntry( CNS_CMDLEVEL,	CMD_SOCKFUNC,	(CMD_DEFINE)&command_xtele);
	CommandMap["XGATE"]				= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_xgate );
	CommandMap["XGOPLACE"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_xgoplace);
	//Y
	//Z
	CommandMap["ZEROKILLS"]			= CommandMapEntry( GM_CMDLEVEL,		CMD_SOCKFUNC,	(CMD_DEFINE)&command_zerokills);
}

void cCommands::Register( std::string cmdName, cScript *toRegister, UI08 cmdLevel, bool isEnabled )
{
#ifdef _DEBUG
	Console.Print( "Registering command %s at command level %i\n", cmdName.c_str(), cmdLevel );
#endif
	UString upper		= cmdName;
	upper				= upper.upper();
	JSCommandMap[upper]	= JSCommandEntry( cmdLevel, toRegister, isEnabled );
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
