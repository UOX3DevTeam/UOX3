#include "uox3.h"
#include "cVersionClass.h"

extern cVersionClass CVC;

//o---------------------------------------------------------------------------o
//|   Function    :  bool validHair( UI16 id )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Checks if selected hair is a valid hair type
//o---------------------------------------------------------------------------o
bool validHair( UI16 id )
{
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
		break;
	default:
		return false;
	}
	return true;
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
	switch( id )
	{
	case 0x203E:
	case 0x203F:
	case 0x2040:
	case 0x2041:
	case 0x204B:
	case 0x204C:
	case 0x204D:
		break;
	default:
		return false;
	}
	return true;
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

//o---------------------------------------------------------------------------o
//|	Function	-	void playChar( cSocket *mSock )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Called when "Play Character" button is hit
//o---------------------------------------------------------------------------o
void playChar( cSocket *mSock )
{
	if( mSock == NULL )
		return;

	UOXSOCKET sock = calcSocketFromSockObj( mSock );

	if( mSock->AcctNo() != -1 )
	{
		ACTREC *ourRec = NULL;
		ourRec = mSock->AcctObj();
		CChar *kChar = NULL, *ourChar = NULL;
		if( ourRec == NULL )
		{
			Network->Disconnect( sock );
			return;
		}
		else
		{
			ourChar = ourRec->characters[mSock->GetByte( 0x44 )];
			if( ourChar != NULL )
			{
				if( !ourChar->IsNpc() && !ourChar->isFree() )
					kChar = ourChar;
			}
			else
			{
				Network->Disconnect( sock );
				return;
			}
		}
		if( kChar != NULL )
		{
			// Added for Krrios client...
			if( mSock->GetByte( 7 ) == 0xFF )
			{
				// If we dont accept Krrios client we should
				// disconnect it here now
				if( false )
				{
					CPKAccept Disconnected( 0x00 );
					mSock->Send( &Disconnected );
					Network->Disconnect( sock );
					Console.Warning( 1, "Disconnected a Krrios client\n" );
					return;
				} else if( kChar->IsGM() ) {					
					CPKAccept AckGM( 0x02 );
					mSock->Send( &AckGM );
					Console.Print( "Accepted a Krrios client with GM Privs\n" );
				} else {
					CPKAccept AckNoGM( 0x01 );
					mSock->Send( &AckNoGM );
					Console.Print( "Accepted a Krrios client without GM Privs\n" );
				}
			}

			WhoList->FlagUpdate();
			OffList->FlagUpdate();	// offline list changes too
			if( ourRec->inworld != INVALIDSERIAL )
				ourRec->inworld = ourChar->GetSerial();
			else
				ourRec->inworld = INVALIDSERIAL;

			if( ourRec->inworld == INVALIDSERIAL || ourRec->inworld == ourChar->GetSerial() )
			{
				ourRec->inworld = ourChar->GetSerial();
				ourChar->SetLogout( -1 );
				mSock->CurrcharObj( kChar );
				startChar( mSock );
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
				mSock->Send( &rejSend );
			}
		}
	}
	else
		Network->Disconnect( sock );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void deleteChar( cSocket *s )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Delete character
//o---------------------------------------------------------------------------o
void deleteChar( cSocket *s )
{
	if( s == NULL )
		return;
	if( s->AcctNo() != -1 )
	{
		UI08 slot = s->GetByte( 0x22 );
		ACTREC *ourAccount = s->AcctObj();
		if( ourAccount != NULL )
		{
			CChar *ourObj = ourAccount->characters[slot];
			if( ourObj != NULL )	// we have a char
			{
				Accounts->RemoveCharacterFromAccount( ourAccount, slot );
				Npcs->DeleteChar( ourObj );
			}

		}

		UI08 charCount = 0;
		for( UI08 i = 0; i < 5; i++ )
		{
			if( ourAccount->characters[i] != NULL )
				charCount++;
		}
		cServerData *sd = cwmWorldState->ServerData();
		UI08 serverCount = sd->GetNumServerLocations();
		CPCharAndStartLoc toSend( ourAccount, charCount, serverCount );
		for( UI08 j = 0; j < serverCount; j++ )
		{
			toSend.AddStartLocation( sd->GetServerLocation( j ), j );
		}
		s->Send( &toSend );
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	UI08 CapIt( UI08 value )
//|	Programmer	-	Abaddon
//|	Date		-	6th November, 2001
//|	Modified	-	Thyme	23rd November, 2001
//|					Changed high value from 45 to 60
//o---------------------------------------------------------------------------o
//|	Purpose		-	Returns an upper and lower bound capped stat value
//o---------------------------------------------------------------------------o
UI08 CapIt( UI08 value )
{
	if( value > 60 )
		return 60;
	else if( value < 10 )
		return 10;
	else
		return value;
}

//o---------------------------------------------------------------------------o
//|	Function	-	UI16 CapColour( UI16 value )
//|	Programmer	-	Abaddon
//|	Date		-	6th November, 2001
//o---------------------------------------------------------------------------o
//|	Purpose		-	Returns an upper and lower bound capped stat value
//o---------------------------------------------------------------------------o
UI16 CapColour( UI16 value )
{
	if( value > 0x04AD )
		return 0x04AD;
	else if( value < 0x044E )
		return 0x044E;
	else
		return value;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void createChar( cSocket *mSock )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Character creation stuff
//o---------------------------------------------------------------------------o
void createChar( cSocket *mSock )
{
	if( mSock == NULL )
		return;

	CHARACTER mCharOff;
	CChar *mChar = Npcs->MemCharFree( mCharOff );
	if( mChar == NULL )
		return;

	UOXSOCKET sock = calcSocketFromSockObj( mSock );

	int totalstats, totalskills;
	UI08 i;
	R32 percheck;

	mSock->CurrcharObj( mChar );
	char tempName[MAX_NAME];
	UI08 *Buffer = mSock->Buffer();
	for( i = 0; i <= strlen( (char *)&(Buffer[10]) ); i++ )
	{
		if( i >= MAX_NAME )
			break;
		tempName[i] = Buffer[10 + i];
	}
	mChar->SetName( tempName );
	mChar->SetAccount( mSock->AcctNo() );
	Accounts->AddCharacterToAccount( mSock->AcctNo(), mChar );
	if( Buffer[0x46] != 0 )
	{
		mChar->SetID( 0x191 );
		mChar->SetxID( 0x191 );
		mChar->SetOrgID( 0x191 );
	}
	else
	{
		mChar->SetID( 0x190 );
		mChar->SetxID( 0x190 );
		mChar->SetOrgID( 0x190 );
	}
	mChar->SetSkin( mSock->GetWord( 0x50 ) );
	if( mChar->GetSkin() < 0x83EA || mChar->GetSkin() > 0x8422 )
		mChar->SetSkin( 0x83EA );
	mChar->SetxSkin( mChar->GetSkin() );
	mChar->SetPriv( cwmWorldState->ServerData()->GetServerStartPrivs( 0 ) );
	mChar->SetPriv2( cwmWorldState->ServerData()->GetServerStartPrivs( 1 ) );
	
	if( mSock->AcctNo() == 0 )
	{ 
		mChar->SetPriv( 0xFF );
		mChar->SetCommandLevel( GMCMDLEVEL );
	}
	LPSTARTLOCATION toGo = cwmWorldState->ServerData()->GetServerLocation( Buffer[0x5B] );
	if( toGo == NULL )
		mChar->SetLocation( 1000, 1000, 0 );
	else
		mChar->SetLocation( toGo->x, toGo->y, toGo->z );
	mChar->SetDir( 4 );

//	Thyme
//	Modified to fit in with new client, and 80 total starting stats.
//	The highest any one stat can be is 60, and the lowest is 10.
	mChar->SetStrength( CapIt( Buffer[0x47] ) );
	mChar->SetDexterity( CapIt( Buffer[0x48] ) );
	mChar->SetIntelligence( CapIt( Buffer[0x49] ) );

	totalstats = mChar->GetStrength() + mChar->GetDexterity() + mChar->GetIntelligence();

	// This should never happen, unless someone is trying to "hack" it.
	if( totalstats != 80 )
	{
		percheck = ( mChar->GetStrength() / (R32)totalstats );
		mChar->SetStrength( CapIt( (SI32)( percheck * 80 ) ) );
		percheck = ( mChar->GetDexterity() / (R32)totalstats );
		mChar->SetDexterity( CapIt( (SI32)( percheck * 80 ) ) );
		percheck = ( mChar->GetIntelligence() / (R32)totalstats );
		mChar->SetIntelligence( CapIt( (SI32)( percheck * 80 ) ) );
	}

	mChar->SetHP( mChar->GetMaxHP() );
	mChar->SetStamina( mChar->GetMaxStam() );
	mChar->SetMana( mChar->GetMaxMana() );
	if( Buffer[0x4B] > 50 )
		Buffer[0x4B] = 50;
	totalskills = Buffer[0x4B];
	if( Buffer[0x4D] > 50 )
		Buffer[0x4D] = 50;
	totalskills += Buffer[0x4D];
	if( Buffer[0x4F] > 50 )
		Buffer[0x4F] = 50;
	if( Buffer[0x4F] + totalskills > 100 )
		Buffer[0x4F] = 100 - totalskills;
	
	mChar->SetRace( 0 );
	mChar->SetRaceGate( 65535 );
	UI08 priSkill = Buffer[0x4A];
	UI08 scdSkill = Buffer[0x4C];
	UI08 thrdSkill = Buffer[0x4E];

	const UI08 HAIR = 0;
	const UI08 BEARD = 1;
	const UI08 PACK = 2;
	const UI08 LOWERGARMENT = 3;
	const UI08 EXTRA1 = 4;
	const UI08 EXTRA2 = 5;
	const UI08 EXTRA3 = 6;
	const UI08 GOLD =	7;
	const UI08 ITOTAL = 8;

	CItem *CreatedItems[ITOTAL] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

	for( i = 0; i < TRUESKILLS; i++ )
	{
		mChar->SetBaseSkill( 0, i );
		if( i == priSkill )
			mChar->SetBaseSkill( (UI16)(Buffer[0x4B] * 10), i );
		else if( i == scdSkill )
			mChar->SetBaseSkill( (UI16)(Buffer[0x4D] * 10), i );
		else if( i == thrdSkill )
			mChar->SetBaseSkill( (UI16)(Buffer[0x4F] * 10), i );
		Skills->updateSkillLevel( mChar, i );
	}

	UI16 ItemID, ItemColour;
	if( validHair( mSock->GetWord( 0x52 ) ) )
	{
		ItemID = mSock->GetWord( 0x52 );
		ItemColour = CapColour( mSock->GetWord( 0x54 ) );
		CreatedItems[HAIR] = Items->SpawnItem( mSock, mChar, 1, "#", false, ItemID, ItemColour, false, false );
		if( CreatedItems[HAIR] != NULL )
			CreatedItems[HAIR]->SetLayer( 0x0B );
	}
	if( validBeard( mSock->GetWord( 0x56 ) ) && mChar->GetID( 2 ) == 0x90 )
	{
		ItemID = mSock->GetWord( 0x56 );
		ItemColour = CapColour( mSock->GetWord( 0x58 ) );
		CreatedItems[BEARD] = Items->SpawnItem( mSock, mChar, 1, "#", false, ItemID, ItemColour, false, false );
		if( CreatedItems[BEARD] != NULL )
			CreatedItems[BEARD]->SetLayer( 0x10 );
	}
	CreatedItems[PACK] = Items->SpawnItem( mSock, mChar, 1, "#", false, 0x0E75, 0, false, false );
	if( CreatedItems[PACK] != NULL )
	{
		mChar->SetPackItem( CreatedItems[PACK] );
		CreatedItems[PACK]->SetLayer( 0x15 );
		CreatedItems[PACK]->SetCont( mChar->GetSerial() );
		CreatedItems[PACK]->SetType( 1 );
		CreatedItems[PACK]->SetDye( true );
	}
	CreatedItems[LOWERGARMENT] = Items->SpawnItem( mSock, mChar, 1, "#", false, 0x0915, 0, false, false );
	if( CreatedItems[LOWERGARMENT] != NULL )
	{
		UI16 newID;
		UI08 newLayer;
		if( mChar->GetID() == 0x0190 )
		{
			newLayer = 4;
			switch( RandomNum( 0, 1 ) )
			{
			case 0:	newID = 0x152E;	break;
			case 1:	newID = 0x1539;	break;
			}
		}
		else
		{
			newLayer = 23;
			switch( RandomNum( 0, 1 ) )
			{
			case 0:	newID = 0x1537;	break;
			case 1:	newID = 0x1516;	break;
			}
		}
		CreatedItems[LOWERGARMENT]->SetLayer( newLayer );
		CreatedItems[LOWERGARMENT]->SetID( newID );
		CreatedItems[LOWERGARMENT]->SetColour( mSock->GetWord( 102 ) );
		CreatedItems[LOWERGARMENT]->SetType( 0 );
		CreatedItems[LOWERGARMENT]->SetDye( true );
	}	
	CreatedItems[EXTRA1] = Items->SpawnItem( mSock, mChar, 1, "#", false, 0x0915, 0, false, false ); // spawn pants
	if( CreatedItems[EXTRA1] != NULL )
	{
		if( RandomNum( 0, 1 ) )
			CreatedItems[EXTRA1]->SetID( 0x1EFD );
		else
			CreatedItems[EXTRA1]->SetID( 0x1517 );
		CreatedItems[EXTRA1]->SetColour( mSock->GetWord( 100 ) );
		CreatedItems[EXTRA1]->SetLayer( 0x05 );
		CreatedItems[EXTRA1]->SetDye( true );
		CreatedItems[EXTRA1]->SetDef( 1 );
	}	
	CreatedItems[EXTRA2] = Items->SpawnItem( mSock, mChar, 1, "#", false, 0x170F, 0x0287, false, false);
	if( CreatedItems[EXTRA2] != NULL )
	{
		CreatedItems[EXTRA2]->SetLayer( 0x03 );
		CreatedItems[EXTRA2]->SetDye( true );
		CreatedItems[EXTRA2]->SetDef( 1 );
	}	
	CreatedItems[EXTRA3] = Items->SpawnItem( mSock, mChar, 1, "#", false, 0x0F51, 0, false, false );
	if( CreatedItems[EXTRA3] == NULL ) 
	{
		return;
	}
	CreatedItems[EXTRA3]->SetLayer( 0x01 );
	CreatedItems[EXTRA3]->SetLoDamage( 5 );
	CreatedItems[EXTRA3]->SetHiDamage( 5 );

	SERIAL mSerial = mChar->GetSerial();
	for( SI32 ctr = 0; ctr < GOLD; ctr++ )
	{
		if( CreatedItems[ctr] != NULL )
			CreatedItems[ctr]->SetCont( mSerial );
	}
	
	// Give the character some gold
	CreatedItems[GOLD] = Items->SpawnItem( mSock, mChar, cwmWorldState->ServerData()->GetServerStartGold() , "#", true, 0x0EED, 0, true, false );
	if( CreatedItems[GOLD] == NULL ) 
	{
		return;
	}
	CreatedItems[GOLD]->SetLayer( 0x01 );
	newbieItems( mChar );

	// Added for my client - Krrios
	if ( Buffer[9] == 0xFF ) // Signal that this is not the standard client
	{
		if ( false ) // TODO: Add a server option?
		{
			CPKAccept Disconnected( 0x00 );
			mSock->Send( &Disconnected );
			Network->Disconnect( sock );
			Console.Warning( 1, "Disconnected a Krrios client\n" );
			return;
		}
		else if ( mChar->IsGM() )
		{
			CPKAccept AckGM( 0x02 );
			mSock->Send( &AckGM );
			Console.Print( "Accepted a Krrios client with GM Privs\n" );
		}
		else
		{
			CPKAccept AckNoGM( 0x01 );
			mSock->Send( &AckNoGM );
			Console.Print( "Accepted a Krrios client without GM Privs\n" );
		}
	}

	startChar( mSock, true );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void startChar( cSocket *mSock, bool onCreate )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Sends character startup stuff to player
//o---------------------------------------------------------------------------o
void startChar( cSocket *mSock, bool onCreate )
{
	if( mSock == NULL )
		return;

	CChar *mChar = mSock->CurrcharObj();
	if( mChar == NULL )
		return;

	mSock->TargetOK( false );

	cPCharLocBody startup = (*mChar);
	if( mChar->GetPoisoned() ) 
		startup.Flag( 0x04 ); 
	else 
		startup.Flag( 0x00 );
	mSock->Send( &startup );

	mChar->SetSpiritSpeakTimer( 0 );
	mChar->SetStealth( -1 );
	if( !mChar->IsPermHidden() )
		mChar->SetHidden( 0 );
	mChar->SetWar( false );

	wornItems( mSock, mChar );
	
	CPWarMode wMode( 0 );				mSock->Send( &wMode );
	SendMapChange( mChar->WorldNumber(), mSock, true ); // Moved that here - darkstorm
	CPLoginComplete lc;					mSock->Send( &lc );

	UI08 currentHour = cwmWorldState->ServerData()->GetServerTimeHours();
	UI08 currentMins = cwmWorldState->ServerData()->GetServerTimeMinutes();
	UI08 currentSecs = cwmWorldState->ServerData()->GetServerTimeSeconds();

	CPTime tmPckt( currentHour, currentMins, currentSecs );	mSock->Send( &tmPckt );
	Weight->calcWeight( mChar );

	sprintf( idname, "%s v%s(%s) [%s] Compiled by %s ", CVC.GetProductName(), CVC.GetVersion(), CVC.GetBuild(), OS_STR, CVC.GetName() );
	sysmessage( mSock, idname );
	sprintf( idname, "Programmed by: %s", CVC.GetProgrammers() );
	sysmessage( mSock, idname );

	CItem *nItem = FindItemOnLayer( mChar, 0x15 );
	mChar->SetPackItem( nItem );

	mChar->SetRegion( 255 );
	checkRegion( mChar );
	Network->setLastOn( mSock );
	
	if( cwmWorldState->ServerData()->GetServerJoinPartAnnouncementsStatus() )
	{
		char temp[1024];
		sprintf( temp, Dictionary->GetEntry( 1208 ), mChar->GetName() );//message upon entering a server
		sysbroadcast( temp );//message upon entering a server
	}
	// This is already achieved by Login2
	// moved mapchange before login stuff
	mChar->Teleport();
	mChar->SetStep( 1 );
	updates( mSock );
	if( cwmWorldState->ServerData()->GetHtmlStatsStatus() > 0 )
#pragma note( "HTML Status for Players" )
		//DumpPlayerHTML( mChar );
	if( onCreate )
	{
		cScript *onCreateScp = Trigger->GetScript( 0 );	// 0 == global script
		if( onCreateScp != NULL )
			onCreateScp->OnCreate( mChar );
	}

	cScript *onLoginScp = Trigger->GetScript( mChar->GetScriptTrigger() );
	if( onLoginScp != NULL )
		onLoginScp->OnLogin( mSock, mChar );
	else 
	{
		onLoginScp = Trigger->GetScript( 0 );
		
		if( onLoginScp != NULL )
			onLoginScp->OnLogin( mSock, mChar );
	}
}
