#include "uox3.h"
#include "cVersionClass.h"
#include "cSkillClass.h"
#include "cServerDefinitions.h"
#include "wholist.h"
#include "skills.h"
#include "ssection.h"
#include "CJSMapping.h"
#include "cScript.h"
#include "CPacketSend.h"
#include "classes.h"
#include "townregion.h"
#include "Dictionary.h"
#include "cEffects.h"

#include "ObjectFactory.h"

namespace UOX
{

//o---------------------------------------------------------------------------o
//|   Function    :  bool validHair( UI16 id )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Checks if selected hair is a valid hair type
//o---------------------------------------------------------------------------o
bool validHair( UI16 id )
{
	bool rvalue;
	switch( id )
	{
		case 0x203B:
		case 0x203C:
		case 0x203D:
		case 0x2044:
		case 0x2045:
		case 0x2046:
		case 0x2047:
		case 0x2048:
		case 0x2049:
		case 0x204A:
			rvalue = true;
			break;
		default:
			rvalue = false;
			break;
	}
	return rvalue;
}

//o---------------------------------------------------------------------------o
//|   Function    :  bool validBeard( UI16 id )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Check if selected beard is a valid beard type
//|					 in his pack, on his paperdoll or in his hands
//o---------------------------------------------------------------------------o
bool validBeard( UI16 id )
{
	bool rvalue;
	switch( id )
	{
		case 0x203E:
		case 0x203F:
		case 0x2040:
		case 0x2041:
		case 0x204B:
		case 0x204C:
		case 0x204D:
			rvalue = true;
			break;
		default:
			rvalue = false;
	}
	return rvalue;
}

void startChar( CSocket *mSock, bool onCreate = false );
//o---------------------------------------------------------------------------o
//|	Function	-	void playChar( CSocket *mSock )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Called when "Play Character" button is hit
//o---------------------------------------------------------------------------o
bool CPIPlayCharacter::Handle( void )
{
	bool disconnect = false;
	if( tSock != NULL )
	{
		if( tSock->AcctNo() != AB_INVALID_ID )
		{
			ACCOUNTSBLOCK&  actbRec	= tSock->GetAccount();
			CChar *kChar			= NULL;
			CChar *ourChar			= NULL;
			if( actbRec.wAccountIndex == AB_INVALID_ID )
			{
				Network->Disconnect( tSock );
				disconnect = true;
			}
			else
			{
				ourChar = actbRec.lpCharacters[slotChosen];
				if( ourChar != NULL )
				{
					if( !ourChar->IsNpc() && !ourChar->isFree() )
						kChar = ourChar;
				}
				else
				{
					Network->Disconnect( tSock );
					disconnect = true;
				}
			}
			if( ValidateObject( kChar ) && !disconnect )
			{
				// Added for Krrios client...
				if( tSock->GetByte( 7 ) == 0xFF )
				{
					// If we dont accept Krrios client we should
					// disconnect it here now
//					if( false )
//					{
//						CPKAccept Disconnected( 0x00 );
//						tSock->Send( &Disconnected );
//						Network->Disconnect( tSock );
//						Console.Warning( 1, "Disconnected a Krrios client" );
//						disconnect = true;
//					} 
					if( kChar->IsGM() ) 
					{					
						CPKAccept AckGM( 0x02 );
						tSock->Send( &AckGM );
						Console.Print( "Accepted a Krrios client with GM Privs\n" );
					} 
					else 
					{
						CPKAccept AckNoGM( 0x01 );
						tSock->Send( &AckNoGM );
						Console.Print( "Accepted a Krrios client without GM Privs\n" );
					}
				}
				if( !disconnect )
				{
					WhoList->FlagUpdate();
					OffList->FlagUpdate();	// offline list changes too
					if( actbRec.dwInGame != INVALIDSERIAL )
						actbRec.dwInGame = kChar->GetSerial();
					else
						actbRec.dwInGame = INVALIDSERIAL;

					if( actbRec.dwInGame == INVALIDSERIAL || actbRec.dwInGame == kChar->GetSerial() )
					{
						actbRec.dwInGame = kChar->GetSerial();
						kChar->SetTimer( tPC_LOGOUT, 0 );
						kChar->SetAccount( actbRec );
						tSock->CurrcharObj( kChar );
						startChar( tSock );
					} 
					else 
					{
						CPIdleWarning rejSend( 0x05 );
						/*
						0x53 Client Message:
						
						0x01=Char doesn't exist
						0x02=Char Already Exists 
						0x03,0x04=Cannot Attach to server 
						0x05=Char in world Msg
						0x06=Login server Syc Error
						0x07=Idle msg
						0x08 (and up?)=Can't Attach
						*/
						tSock->Send( &rejSend );
					}
				}
			}
		}
		else
			Network->Disconnect( tSock );
	}
	return true;
}

//o--------------------------------------------------------------------------o
//|	Function		-	void deleteChar( CSocket *s )
//|	Date			-	
//|	Developers		-	Unknown / EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Remove a character from the accounts system, due to 
//|							an account gump button press
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
bool CPIDeleteCharacter::Handle( void )
{
	if( tSock != NULL )
	{
		ACCOUNTSBLOCK * actbTemp = &tSock->GetAccount();
		UI08 slot = tSock->GetByte( 0x22 );
		if( actbTemp->wAccountIndex != AB_INVALID_ID )
		{
			CChar *ourObj = actbTemp->lpCharacters[slot];
			if( ValidateObject( ourObj ) )	// we have a char
			{
				Accounts->DelCharacter( actbTemp->wAccountIndex , slot );
				ourObj->Delete();
			}

		}
		else
		{
			// Support for accounts. The current copy of the account isn't correct. So get a new copy to work with.
			Accounts->Load();
			actbTemp = &Accounts->GetAccountByID( actbTemp->wAccountIndex );
		}

		UI08 charCount = 0;
		for( UI08 i = 0; i < 6; ++i )
		{
			if( ValidateObject( actbTemp->lpCharacters[i] ) )
				++charCount;
		}
		CServerData *sd		= cwmWorldState->ServerData();
		UI08 serverCount	= static_cast<UI08>(sd->NumServerLocations());
		CPCharAndStartLoc toSend( (*actbTemp), charCount, serverCount );
		for( UI08 j = 0; j < serverCount; ++j )
		{
			toSend.AddStartLocation( sd->ServerLocation( j ), j );
		}
		tSock->Send( &toSend );
	}
	return true;
}

template< class T >
T Capped( T value, T min, T max )
{
	return UOX_MAX( UOX_MIN( value, max ), min );
}

//o--------------------------------------------------------------------------o
//|	Function		-	void addNewbieItem( CSocket *socket, CChar *c, char* str)
//|	Date			-	
//|	Developers		-	Thyme
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Adds a newbie item defined in newbie.dfn
//o--------------------------------------------------------------------------o
//| Modifications	-	PACKITEM now supports item,amount - Zane
//o--------------------------------------------------------------------------o
void addNewbieItem( CSocket *socket, CChar *c, char* str)
{
	ScriptSection *newbieData = FileLookup->FindEntry( str, newbie_def );
	if( newbieData != NULL )
	{
		CItem *n = NULL;
		for( UString tag = newbieData->First(); !newbieData->AtEnd(); tag = newbieData->Next() )
		{
			UString data = newbieData->GrabData();
			if( !data.empty() )
			{
				UString UTag = tag.upper();
				if( UTag == "PACKITEM" )
				{
					if( data.sectionCount( "," ) != 0 )
					{
						n = Items->CreateScriptItem( socket, c, data.section( ",", 0, 0 ).stripWhiteSpace(), 1, OT_ITEM, true );
						if( n != NULL )
							n->SetAmount( data.section( ",", 1, 1 ).stripWhiteSpace().toUShort() );
					}
					else
						n = Items->CreateScriptItem( socket, c, data.c_str(), 1, OT_ITEM, true );
				}
				else if( UTag == "EQUIPITEM" )
				{
					n = Items->CreateScriptItem( socket, c, data.c_str(), 1, OT_ITEM, true );
					if( n != NULL && n->GetLayer() != IL_NONE )
					{
						if( c->GetItemAtLayer( n->GetLayer() ) != NULL )
							n->SetCont( c->GetPackItem() );
						else
							n->SetCont( c );
					}
				}
				if( n != NULL )
					n->SetNewbie( true );
			}
		}
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	void CPICreateCharacter::newbieItems( CChar *mChar )
//|	Date			-	
//|	Developers		-	thyme
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Adds newbie items based on characters chosen skills
//o--------------------------------------------------------------------------o
void CPICreateCharacter::newbieItems( CChar *mChar )
{
	enum NewbieItems
	{
		HAIR = 0,
		BEARD,
		PACK,
		LOWERGARMENT,
		UPPERGARMENT,
		SHOES,
		GOLD,
		ITOTAL
	};

	CItem *CreatedItems[ITOTAL] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL };

	UI16 ItemID, ItemColour;
	if( validHair( hairStyle ) )
	{
		ItemID = hairStyle;
		ItemColour = Capped( hairColour, static_cast< UI16 >(0x044E), static_cast< UI16 >(0x04AD) );
		CreatedItems[HAIR] = Items->CreateItem( tSock, mChar, ItemID, 1, ItemColour, OT_ITEM );
		if( CreatedItems[HAIR] != NULL )
			CreatedItems[HAIR]->SetLayer( IL_HAIR );
	}
	if( validBeard( facialHair ) && sex == 0 )
	{
		ItemID = facialHair;
		ItemColour = Capped( facialHairColour, static_cast< UI16 >(0x044E), static_cast< UI16 >(0x04AD) );
		CreatedItems[BEARD] = Items->CreateItem( tSock, mChar, ItemID, 1, ItemColour, OT_ITEM );
		if( CreatedItems[BEARD] != NULL )
			CreatedItems[BEARD]->SetLayer( IL_FACIALHAIR );
	}
	CreatedItems[PACK] = Items->CreateItem( tSock, mChar, 0x0E75, 1, 0, OT_ITEM );
	if( CreatedItems[PACK] != NULL )
	{
		mChar->SetPackItem( CreatedItems[PACK] );
		CreatedItems[PACK]->SetLayer( IL_PACKITEM );
		CreatedItems[PACK]->SetCont( mChar );
		CreatedItems[PACK]->SetType( IT_CONTAINER );
		CreatedItems[PACK]->SetDye( true );
	}
	CreatedItems[LOWERGARMENT] = Items->CreateItem( tSock, mChar, 0x0915, 1, 0, OT_ITEM );
	if( CreatedItems[LOWERGARMENT] != NULL )
	{
		UI16 newID = INVALIDID;
		ItemLayers newLayer = IL_NONE;
		if( mChar->GetID() == 0x0190 )
		{
			newLayer = IL_PANTS;
			switch( RandomNum( 0, 1 ) )
			{
				case 0:	newID = 0x152E;	break;
				case 1:	newID = 0x1539;	break;
			}
		}
		else
		{
			newLayer = IL_OUTERLEGGINGS;
			switch( RandomNum( 0, 1 ) )
			{
				case 0:	newID = 0x1537;	break;
				case 1:	newID = 0x1516;	break;
			}
		}
		CreatedItems[LOWERGARMENT]->SetLayer( newLayer );
		CreatedItems[LOWERGARMENT]->SetID( newID );
		CreatedItems[LOWERGARMENT]->SetColour( pantsColour );
		CreatedItems[LOWERGARMENT]->SetDye( true );
	}	
	CreatedItems[UPPERGARMENT] = Items->CreateItem( tSock, mChar, 0x0915, 1, 0, OT_ITEM );
	if( CreatedItems[UPPERGARMENT] != NULL )
	{
		if( RandomNum( 0, 1 ) )
			CreatedItems[UPPERGARMENT]->SetID( 0x1EFD );
		else
			CreatedItems[UPPERGARMENT]->SetID( 0x1517 );
		CreatedItems[UPPERGARMENT]->SetColour( shirtColour );
		CreatedItems[UPPERGARMENT]->SetLayer( IL_INNERSHIRT );
		CreatedItems[UPPERGARMENT]->SetDye( true );
	}	
	CreatedItems[SHOES] = Items->CreateItem( tSock, mChar, 0x170F, 1, 0x0287, OT_ITEM );
	if( CreatedItems[SHOES] != NULL )
	{
		CreatedItems[SHOES]->SetLayer( IL_FOOTWEAR );
		CreatedItems[SHOES]->SetDye( true );
	}	

	for( UI08 ctr = 0; ctr < GOLD; ++ctr )
	{
		if( CreatedItems[ctr] != NULL )
			CreatedItems[ctr]->SetCont( mChar );
	}
	
	// Give the character some gold
	CreatedItems[GOLD] = Items->CreateItem( tSock, mChar, 0x0EED, cwmWorldState->ServerData()->ServerStartGold(), 0, OT_ITEM, true );
	if( CreatedItems[GOLD] != NULL )
		CreatedItems[GOLD]->SetLayer( IL_RIGHTHAND );

	std::vector< cSkillClass > vecSkills;
	char whichsect[15];
	for( UI08 sCtr = 0; sCtr < ALLSKILLS; ++sCtr )
		vecSkills.push_back( cSkillClass( sCtr, mChar->GetBaseSkill( sCtr ) ) );

	std::sort( vecSkills.rbegin(), vecSkills.rend() );
	for( UI08 i = 0; i < 3 ; ++i )
	{
		if( vecSkills[i].value > 0 )
		{
			sprintf( whichsect, "BESTSKILL %i", vecSkills[i].skill );
			addNewbieItem( tSock, mChar, whichsect );
		}
	}
	addNewbieItem( tSock, mChar, "DEFAULT" );
}
//o---------------------------------------------------------------------------o
//|	Function	-	void createChar( CSocket *mSock )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Character creation stuff
//o---------------------------------------------------------------------------o
bool CPICreateCharacter::Handle( void )
{
	// This function needs to be decomposed
	// Way too large from a maintenance perspective
	if( tSock != NULL )
	{
		CChar *mChar = static_cast< CChar * >(ObjectFactory::getSingleton().CreateObject( OT_CHAR ));
		if( mChar != NULL )
		{
			SI32 totalstats, totalskills;
			UI08 i;
			R32 percheck;

			tSock->CurrcharObj( mChar );
			mChar->SetName( charName );
			Accounts->AddCharacter( tSock->AcctNo(), mChar );
			ACCOUNTSBLOCK &actbRec	= tSock->GetAccount();
			if( actbRec.dwInGame != INVALIDSERIAL )
				actbRec.dwInGame = mChar->GetSerial();

			if( actbRec.dwInGame == INVALIDSERIAL || actbRec.dwInGame == mChar->GetSerial() )
			{
				actbRec.dwInGame = mChar->GetSerial();
				mChar->SetAccount( actbRec );
			}
			UI16 pGenderID = 0x190;
			if( sex != 0 )
				pGenderID = 0x191;

			mChar->SetID( pGenderID );
			mChar->SetOrgID( pGenderID );

			mChar->SetSkin( Capped( skinColour, static_cast<UI16>(0x03ea), static_cast<UI16>(0x0422) ) );
//			if( mChar->GetSkin() < 0x83EA || mChar->GetSkin() > 0x8422 )
//				mChar->SetSkin( 0x83EA );
			mChar->SetOrgSkin( mChar->GetSkin() );

			mChar->SetPriv( cwmWorldState->ServerData()->ServerStartPrivs() );
			
			ACCOUNTSBLOCK& actbTemp2 = mChar->GetAccount();
			if( actbTemp2.wAccountIndex != AB_INVALID_ID && ( (actbTemp2.wFlags&AB_FLAGS_GM) == AB_FLAGS_GM ) )
			{ 
				mChar->SetPriv( 0xFF );
				mChar->SetCommandLevel( GM_CMDLEVEL );
			}
			LPSTARTLOCATION toGo = cwmWorldState->ServerData()->ServerLocation( locationNumber );
			if( toGo == NULL )
				mChar->SetLocation( 1495, 1629, 10 ); //Send character to the sweet dreams inn in Britain!
			else
				mChar->SetLocation( toGo->x, toGo->y, static_cast<SI08>(toGo->z) );
			mChar->SetDir( 4 );

			//	Date Unknown - Thyme - Modified to fit in with new client, and 80 total starting stats. The highest any one stat can be is 60, and the lowest is 10.
			mChar->SetStrength( Capped( str, static_cast< UI08 >(10), static_cast< UI08 >(60) ) );
			mChar->SetDexterity( Capped( dex, static_cast< UI08 >(10), static_cast< UI08 >(60) ) );
			mChar->SetIntelligence( Capped( intel, static_cast< UI08 >(10), static_cast< UI08 >(60) ) );

			totalstats = mChar->GetStrength() + mChar->GetDexterity() + mChar->GetIntelligence();

			// This should never happen, unless someone is trying to "hack" it.
			if( totalstats != 80 )
			{
				percheck = ( mChar->GetStrength() / (R32)totalstats );
				mChar->SetStrength( static_cast< UI08 >(Capped( percheck * 80, 10.0f, 60.0f ) ) );
				percheck = ( mChar->GetDexterity() / (R32)totalstats );
				mChar->SetDexterity( static_cast<UI08>(Capped( percheck * 80, 10.0f, 60.0f ) ) );
				percheck = ( mChar->GetIntelligence() / (R32)totalstats );
				mChar->SetIntelligence( static_cast<UI08>(Capped( percheck * 80, 10.0f, 60.0f ) ) );
			}

			mChar->SetHP( mChar->GetMaxHP() );
			mChar->SetStamina( mChar->GetMaxStam() );
			mChar->SetMana( mChar->GetMaxMana() );
			if( skillValue[0] > 50 )
				skillValue[0] = 50;
			totalskills = skillValue[0];
			if( skillValue[1] > 50 )
				skillValue[1] = 50;
			totalskills += skillValue[1];
			if( skillValue[2] > 50 )
				skillValue[2] = 50;
			if( skillValue[2] + totalskills > 100 )
				skillValue[2] = static_cast< UI08 >(100 - totalskills);
			
			mChar->SetRace( 0 );
			mChar->SetRaceGate( 65535 );
			UI08 priSkill	= skill[0];
			UI08 scdSkill	= skill[1];
			UI08 thrdSkill	= skill[2];

			for( i = 0; i < ALLSKILLS; ++i )
			{
				mChar->SetBaseSkill( 0, i );
				if( i == priSkill )
					mChar->SetBaseSkill( (UI16)(skillValue[0] * 10), i );
				else if( i == scdSkill )
					mChar->SetBaseSkill( (UI16)(skillValue[1] * 10), i );
				else if( i == thrdSkill )
					mChar->SetBaseSkill( (UI16)(skillValue[2] * 10), i );
				Skills->updateSkillLevel( mChar, i );
			}

			newbieItems( mChar );

			// Added for my client - Krrios
			if( pattern3 == 0xFF ) // Signal that this is not the standard client
			{
				if( mChar->IsGM() )
				{
					CPKAccept AckGM( 0x02 );
					tSock->Send( &AckGM );
					Console.Print( "Accepted a Krrios client with GM Privs\n" );
				}
				else
				{
					CPKAccept AckNoGM( 0x01 );
					tSock->Send( &AckNoGM );
					Console.Print( "Accepted a Krrios client without GM Privs\n" );
				}
			}
			startChar( tSock, true );
		}
	}
	return true;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void updates( CSocket *s )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Opens the Updates window
//o---------------------------------------------------------------------------o
void updates( CSocket *s )
{
	if( s == NULL )
		return;
	ScriptSection *Updates = FileLookup->FindEntry( "MOTD", misc_def );
	if( Updates == NULL )
		return;

	char updateData[2048];
	UString tag;
	UString data;
	updateData[0] = 0;
	for( tag = Updates->First(); !Updates->AtEnd(); tag = Updates->Next() )
	{
		data = Updates->GrabData();
		sprintf( updateData, "%s%s %s ", updateData, tag.c_str(), data.c_str() );
	}
	CPUpdScroll toSend( 2 );
	toSend.AddString( updateData );
	toSend.Finalize();
	s->Send( &toSend );
}

void sysBroadcast( const std::string txt );
//o---------------------------------------------------------------------------o
//|   Function    :  void startChar( CSocket *mSock, bool onCreate )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Sends character startup stuff to player
//o---------------------------------------------------------------------------o
void startChar( CSocket *mSock, bool onCreate )
{
	if( mSock != NULL )
	{
		/*
		//Login Packet Sequence per Krrios
		Login Confirm (0x1B)					tick
		Map Change (0xBFx08)					tick		SendMapChange
		Map Patches (0xBFx18)					don't send
		Supported Features (0xB9)				// 0x801F
		Season Change (0xBC)					added		SendWorldChange
		Mobile Update (0x20)					added		CPDrawGamePlayer
		Personal Light Level (0x4E)				added		CPPersonalLightLevel
		Mobile Incoming (0x78) 
		Mobile Attributes (0x2D) 
		Set War Mode (0x72)						tick
		[everything on screen] (0x1A/0x78) 
		Login Complete (0x55) 
		*/

		CChar *mChar = mSock->CurrcharObj();
		if( ValidateObject( mChar ) )
		{
			ACCOUNTSBLOCK& actbTemp = mSock->GetAccount();
			if( actbTemp.wAccountIndex != AB_INVALID_ID )
				actbTemp.wFlags |= AB_FLAGS_ONLINE;

			mSock->TargetOK( false );

			CPCharLocBody startup = (*mChar);
			if( mChar->GetPoisoned() ) 
				startup.Flag( 0x04 ); 
			else 
				startup.Flag( 0x00 );
			mSock->Send( &startup );

			mSock->SetTimer( tPC_SPIRITSPEAK, 0 );
			mChar->SetStealth( -1 );
			if( mChar->GetVisible() != VT_PERMHIDDEN )
				mChar->SetVisible( VT_VISIBLE );
			mChar->SetWar( false );

			SendMapChange( mChar->WorldNumber(), mSock, true );
			CPWorldChange wrldChange( mChar->GetRegion()->GetAppearance(), 1 );
			mSock->Send( &wrldChange );	// need to add this?

			CPPersonalLightLevel pllToSend = (*mChar);		// need to add this?
			pllToSend.Level( 0 );
			mSock->Send( &pllToSend );

			CPWarMode wMode( 0 );				mSock->Send( &wMode );

			CItem *nItem = mChar->GetItemAtLayer( IL_PACKITEM );
			mChar->SetPackItem( nItem );

			if( mChar->IsDead() )
			{
				CPResurrectMenu toSend( 0 );
				mSock->Send( &toSend );
			}

			mChar->Dirty( UT_LOCATION );
			mChar->SetStep( 1 );

			CPLoginComplete lc;					mSock->Send( &lc );
			Network->setLastOn( mSock );

			UI08 currentHour = cwmWorldState->ServerData()->ServerTimeHours();
			UI08 currentMins = cwmWorldState->ServerData()->ServerTimeMinutes();
			UI08 currentSecs = cwmWorldState->ServerData()->ServerTimeSeconds();

			CPTime tmPckt( currentHour, currentMins, currentSecs );	mSock->Send( &tmPckt );

			char idname[256];
			sprintf( idname, "%s v%s.%s [%s] Compiled by %s ", CVersionClass::GetProductName().c_str(), CVersionClass::GetVersion().c_str(), CVersionClass::GetBuild().c_str(), OS_STR, CVersionClass::GetName().c_str() );
			mSock->sysmessage( idname );
			sprintf( idname, "Programmed by: %s", CVersionClass::GetProgrammers().c_str() );
			mSock->sysmessage( idname );

			if( cwmWorldState->ServerData()->ServerJoinPartAnnouncementsStatus() )
			{
				char temp[1024];
				sprintf( temp, Dictionary->GetEntry( 1208 ).c_str(), mChar->GetName().c_str() );//message upon entering a server
				sysBroadcast( temp );//message upon entering a server
			}
			updates( mSock );
//			if( cwmWorldState->ServerData()->HtmlStatsStatus() > 0 )
#if defined( _MSC_VER )
#pragma note( "HTML Status for Players" )
#endif
				//DumpPlayerHTML( mChar );
			if( onCreate )
			{
				cScript *onCreateScp = JSMapping->GetScript( (UI16)0 );	// 0 == global script
				if( onCreateScp != NULL )
					onCreateScp->OnCreate( mChar, true );
			}

			cScript *onLoginScp = JSMapping->GetScript( mChar->GetScriptTrigger() );
			if( onLoginScp != NULL )
				onLoginScp->OnLogin( mSock, mChar );
			else 
			{
				onLoginScp = JSMapping->GetScript( (UI16)0 );
				
				if( onLoginScp != NULL )
					onLoginScp->OnLogin( mSock, mChar );
			}
			mSock->LoginComplete( true );
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	CItem *CreateCorpseItem( CChar &mChar )
//|	Programmer	-	giwo
//o---------------------------------------------------------------------------o
//|	Purpose		-	Generates a corpse or backpack based on the character killed.
//o---------------------------------------------------------------------------o
CItem *CreateCorpseItem( CChar& mChar, bool createPack )
{
	CItem *iCorpse = NULL;
	if( !createPack )
	{
		iCorpse = Items->CreateItem( NULL, &mChar, 0x2006, 1, mChar.GetSkin(), OT_ITEM );
		if( iCorpse == NULL )
			return NULL;

		char temp[512];
		sprintf( temp, Dictionary->GetEntry( 1612 ).c_str(), mChar.GetName().c_str() );
		iCorpse->SetName( temp );
		iCorpse->SetCarve( mChar.GetCarve() );
		iCorpse->SetMovable( 2 );//non-movable
		iCorpse->SetDir( mChar.GetDir() );
		iCorpse->SetAmount( mChar.GetID() );
		iCorpse->SetCorpse( true );
	}
	else
	{
		iCorpse = Items->CreateItem( NULL, &mChar, 0x09B2, 1, 0x0000, OT_ITEM );
		if( iCorpse== NULL )
			return NULL;

		iCorpse->SetName( Dictionary->GetEntry( 1611 ) );
	}

	UI08 canCarve = 0;
	if( mChar.GetID( 1 ) == 0x00 && ( mChar.GetID( 2 ) == 0x0C || ( mChar.GetID( 2 ) >= 0x3B && mChar.GetID( 2 ) <= 0x3D ) ) ) // If it's a dragon, 50/50 chance you can carve it
		canCarve = static_cast<UI08>(RandomNum( 0, 1 ));

	iCorpse->SetDecayable( true );
	iCorpse->SetName2( mChar.GetName().c_str() );
	iCorpse->SetType( IT_CONTAINER );
	iCorpse->SetTempVar( CITV_MOREY, 1, canCarve );
	iCorpse->SetTempVar( CITV_MOREY, 2, mChar.isHuman() );
	iCorpse->SetTempVar( CITV_MOREZ, mChar.GetFlag() );
	iCorpse->SetMurderTime( cwmWorldState->GetUICurrentTime() );
	R32 decayTime = static_cast<R32>(cwmWorldState->ServerData()->SystemTimer( tSERVER_DECAY ));
	if( !mChar.IsNpc() )
	{
		UI08 decayMultiplier = static_cast<UI08>(cwmWorldState->ServerData()->PlayerCorpseDecayMultiplier()&0xff);
		iCorpse->SetOwner( &mChar );
		iCorpse->SetDecayTime( BuildTimeValue( static_cast<R32>(decayTime*decayMultiplier) ) );
	}
	else
		iCorpse->SetDecayTime( BuildTimeValue( decayTime ) );

	if( !ValidateObject( mChar.GetAttacker() ) )
		iCorpse->SetTempVar( CITV_MOREX, INVALIDSERIAL );
	else
		iCorpse->SetTempVar( CITV_MOREX, mChar.GetAttacker()->GetSerial() );

	return iCorpse;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void MoveItemsToCorpse( CChar *mChar, CItem *iCorpse )
//|	Programmer	-	giwo
//o---------------------------------------------------------------------------o
//|	Purpose		-	Moves Items from Character to Corpse
//o---------------------------------------------------------------------------o
void MoveItemsToCorpse( CChar &mChar, CItem *iCorpse, bool createPack )
{
	CItem *k			= NULL;
	CItem *packItem		= mChar.GetPackItem();
	bool packIsValid	= ValidateObject( packItem );
	for( CItem *j = mChar.FirstItem(); !mChar.FinishedItems(); j = mChar.NextItem() )
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
			if( !mChar.IsShop() && !createPack )
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

void killTrades( CChar& i );
//o---------------------------------------------------------------------------o
//|   Function    -  void HandleDeath( CChar *mChar )
//|   Date        -  UnKnown
//|   Programmer  -  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     -  Performs death stuff. I.E.- creates a corpse, moves items
//|                  to it, take out of war mode, does animation and sound, etc.
//o---------------------------------------------------------------------------o
void HandleDeath( CChar *mChar )
{
	if( !ValidateObject( mChar ) || mChar->IsDead() || mChar->IsInvulnerable() )	// don't kill them if they are dead or invulnerable!
		return;

	CSocket *pSock = NULL;
	if( !mChar->IsNpc() )
		pSock = calcSocketObjFromChar( mChar );

	DismountCreature( mChar );

	if( pSock != NULL )
		killTrades( (*mChar) );

	if( mChar->GetID() != mChar->GetOrgID() )
		mChar->SetID( mChar->GetOrgID() );

	bool createPack = ( mChar->GetID( 2 ) == 0x0D || mChar->GetID( 2 ) == 0x0F || mChar->GetID( 2 ) == 0x10 || mChar->GetID() == 0x023E );

	CItem *iCorpse = CreateCorpseItem( (*mChar), createPack );
	if( iCorpse != NULL )
	{
		MoveItemsToCorpse( (*mChar), iCorpse, createPack );
		if( cwmWorldState->ServerData()->DeathAnimationStatus() )
			Effects->deathAction( mChar, iCorpse );
	}
	Effects->playDeathSound( mChar );

	mChar->SetDead( true );
	mChar->SetWar( false );
	mChar->StopSpell();
	mChar->SetHP( 0 );
	mChar->SetPoisoned( 0 );
	mChar->SetPoisonStrength( 0 );

	if( !mChar->IsNpc() )
	{
		if( mChar->GetOrgID() == 0x0190 )
			mChar->SetID( 0x0192 );  // Male or Female
		else
			mChar->SetID( 0x0193 );

		CItem *c = Items->CreateItem( NULL, mChar, 0x204E, 1, 0, OT_ITEM );
		if( c == NULL )
			return;
		c->SetName( Dictionary->GetEntry( 1610 ) );
		mChar->SetRobe( c->GetSerial() );
		c->SetLayer( IL_ROBE );
		if( c->SetCont( mChar ) )
			c->SetDef( 1 );

		if( mChar->GetAccount().wAccountIndex != AB_INVALID_ID )
		{
			if( pSock != NULL )
			{
				CPResurrectMenu toSend( 0 );
				pSock->Send( &toSend );
			}
			// The fade to gray is done by the CPResurrectMenu packet
			//mChar->Teleport();
		}
	}
	
	UI16 targTrig		= mChar->GetScriptTrigger();
	cScript *toExecute	= JSMapping->GetScript( targTrig );
	if( toExecute != NULL )
		toExecute->OnDeath( mChar );

	if( mChar->IsNpc() )
		mChar->Delete();
}

}
