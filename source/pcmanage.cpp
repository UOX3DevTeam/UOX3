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

template< class T >
T Capped( const T value, const T minimum, const T maximum )
{
	return UOX_MAX( UOX_MIN( value, maximum ), minimum );
}

//o---------------------------------------------------------------------------o
//|   Function    :  bool validHairStyle( UI16 id )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Checks if selected hair is a valid hair type
//o---------------------------------------------------------------------------o
bool validHairStyle( UI16 id, UI16 bodyID )
{
	bool rvalue = false;
	switch( bodyID )
	{
	case 0x025D:	// elven male
		switch( id )
		{
		case 0x2FBF:	// mid long (male)
		case 0x2FC0:	// long feather (both)
		case 0x2FC1:	// short (both)
		case 0x2FC2:	// mullet (both)
		case 0x2FCD:	// long (male)
		case 0x2FCE:	// topknot (both)
		case 0x2FCF:	// long braid (both)
		case 0x2FD1:	// spiked (male)
			rvalue = true;
			break;
		default:
			break;
		}
		break;
	case 0x025E:	// elven female
		switch( id )
		{
		case 0x2FC0:	// long feather (both)
		case 0x2FC1:	// short (both)
		case 0x2FC2:	// mullet (both)
		case 0x2FCC:	// flower (female)
		case 0x2FCE:	// topknot (both)
		case 0x2FCF:	// long braid (both)
		case 0x2FD0:	// buns (female)
		case 0x2FD1:	// spiked (male)
			rvalue = true;
			break;
		default:
			break;
		}
		break;
	case 0x029A:	// gargoyle male
		switch( id )
		{
		case 0x4258: //
		case 0x4259: //
		case 0x425A: //
		case 0x425B: //
		case 0x425C: //
		case 0x425D: //
		case 0x425E: //
		case 0x425F: //
			rvalue = true;
			break;
		default:
			break;
		}
		break;
	case 0x029B:	// gargoyle female
		switch( id )
		{
		case 0x4261: //
		case 0x4262: //
		case 0x4273: //
		case 0x4274: //
		case 0x4275: //
		case 0x4276: //
		case 0x42AA: //
		case 0x42AB: //
			rvalue = true;
			break;
		default:
			break;
		}
		break;
	default:		// human otherwise
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
			break;
		}
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
bool validBeard( UI16 id, UI16 bodyID )
{
	bool rvalue = false;
	switch( bodyID )
	{
	case 0x029A:	// gargoyle male
		switch( id )
		{
		case 0x42AD:
		case 0x42AE:
		case 0x42AF: //1701?
		case 0x42B0:
			rvalue = true;
			break;
		}
		break;
	default: // human male
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
		}
	}
	return rvalue;
}

//o---------------------------------------------------------------------------o
//|   Function    :  UI16 validSkinColour( UI16 id, UI16 bodyID )
//|   Date        :  22nd January, 2006
//|   Programmer  :  Maarc
//|   Return      :  A validated skin colour (capped into range)
//o---------------------------------------------------------------------------o
//|   Purpose     :  Check if selected skin colour is a valid colour, based on
//|					 the body types involved (elves and humans not the same)
//o---------------------------------------------------------------------------o
COLOUR validSkinColour( UI16 id, UI16 bodyID )
{
	COLOUR rvalue;
	switch( bodyID )
	{
	case 0x025D:	// elven male
	case 0x025E:	// elven female
		switch( id )	// let's see if what is passed in is valid
		{
		case 191:
		case 589:
		case 590:
		case 591:
		case 851:
		case 865:
		case 871:
		case 884:
		case 885:
		case 886:
		case 897:
		case 898:
		case 899:
		case 900:
		case 901:
		case 905:
		case 990:
		case 997:
		case 998:
		case 1000:
		case 1001:
		case 1072:
		case 1191:
		case 1246:
		case 1309:
		case 1343:
		case 1401:
		case 1899:
		case 1900:
		case 1901:
		case 2101:
		case 2307:	rvalue = id;	break;
		default:	rvalue = 191;	break;	// nope, it's not, let's default
		}
		break;
	case 0x029A:
	case 0x029B:	//gargoyle male/female
		rvalue = Capped( id, static_cast<UI16>(0x06DB), static_cast<UI16>(0x06F3) );
		break;
	default:	// human male/female
		rvalue = Capped( id, static_cast<UI16>(0x03EA), static_cast<UI16>(0x0422) );
		break;
	}
	return rvalue;
}

//o---------------------------------------------------------------------------o
//|   Function    :  UI16 validHairColour( UI16 id, UI16 bodyID )
//|   Date        :  22nd January, 2006
//|   Programmer  :  Maarc
//|   Return      :  A validated hair colour (capped into range)
//o---------------------------------------------------------------------------o
//|   Purpose     :  Check if selected hair colour is a valid colour, based on
//|					 the body types involved (elves and humans not the same)
//o---------------------------------------------------------------------------o
COLOUR validHairColour( UI16 id, UI16 bodyID )
{
	COLOUR rvalue;
	switch( bodyID )
	{
	case 0x025D:	// elven male
	case 0x025E:	// elven female
		switch( id )	// let's see if what is passed in is valid
		{
		case 52:
		case 53:
		case 54:
		case 55:
		case 56:
		case 57:
		case 88:
		case 142:
		case 143:
		case 144:
		case 145:
		case 146:
		case 257:
		case 296:
		case 303:
		case 345:
		case 346:
		case 347:
		case 348:
		case 349:
		case 350:
		case 445:
		case 484:
		case 499:
		case 519:
		case 529:
		case 569:
		case 593:
		case 620:
		case 707:
		case 713:
		case 797:
		case 798:
		case 799:
		case 800:
		case 801:
		case 802:
		case 803:
		case 804:
		case 805:
		case 806:
		case 873:
		case 902:
		case 903:
		case 904:
		case 905:
		case 906:
		case 1437:
		case 1720:
		case 1829:
		case 2131:	rvalue = id;	break;
		default:	rvalue = 52;	break;	// nope, it's not, let's default
		}
		break;
	case 0x29A:	// gargoyle male
	case 0x29B: // gargoyle female
		// not all colors used, but easier than specialcasing each one
		rvalue = Capped( id, static_cast< UI16 >(0x06E0), static_cast< UI16 >(0x076B) ); 
		break;
	default:	// human male/female
		rvalue = Capped( id, static_cast< UI16 >(0x044E), static_cast< UI16 >(0x04AD) );
		break;
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
	if( tSock != NULL )
	{
		if( tSock->AcctNo() != AB_INVALID_ID )
		{
			bool disconnect = false;
			CAccountBlock&  actbRec	= tSock->GetAccount();
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
		CAccountBlock * actbTemp = &tSock->GetAccount();
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
		for( UI08 i = 0; i < 7; ++i )
		{
			if( ValidateObject( actbTemp->lpCharacters[i] ) )
				++charCount;
		}
		CServerData *sd		= cwmWorldState->ServerData();
		UI08 serverCount	= static_cast<UI08>(sd->NumServerLocations());
		CPCharAndStartLoc toSend( (*actbTemp), charCount, serverCount, tSock );
		for( UI08 j = 0; j < serverCount; ++j )
		{
			if( tSock->ClientType() >= CV_HS2D && tSock->ClientVersionSub() >= 13 )
				toSend.NewAddStartLocation( sd->ServerLocation( j ), j );
			else
				toSend.AddStartLocation( sd->ServerLocation( j ), j );
		}
		tSock->Send( &toSend );
	}
	return true;
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
void addNewbieItem( CSocket *socket, CChar *c, const char* str, COLOUR pantsColour, COLOUR shirtColour )
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
						UI32 nAmount = data.section( ",", 1, 1 ).stripWhiteSpace().toUShort();
						n = Items->CreateScriptItem( socket, c, data.section( ",", 0, 0 ).stripWhiteSpace(), nAmount, OT_ITEM, true );
					}
					else
					{
						n = Items->CreateScriptItem( socket, c, data.c_str(), 1, OT_ITEM, true );
					}
				}
				else if( UTag == "EQUIPITEM" )
				{
					n = Items->CreateScriptItem( socket, c, data.c_str(), 1, OT_ITEM, true );
					if( n != NULL && n->GetLayer() != IL_NONE )
					{
						bool conflictItem = true;
						CItem *j = c->GetItemAtLayer( n->GetLayer() );
						if( !ValidateObject( j ))
						{
							if( n->GetLayer() == IL_RIGHTHAND )
								j = c->GetItemAtLayer( IL_LEFTHAND );
							else if( n->GetLayer() == IL_LEFTHAND )
								j = c->GetItemAtLayer( IL_RIGHTHAND );

							// GetDir-check is to allow for torches and lanterns, 
							// which use left-hand layer but are not 2-handers or shields
							if( ValidateObject( j ) && !n->IsShieldType() && n->GetDir() == 0 )
							{
								if( j->IsShieldType() || j->GetDir() != 0 )
									conflictItem = false;
							}
							else
							{
								conflictItem = false;
							}
						}
						if( conflictItem == true )
							n->SetCont( c->GetPackItem() );
						else
						{
							n->SetCont( c );
						}
						
						//Apply the choosen colour
						if( ( n->GetLayer() == IL_PANTS || n->GetLayer() == IL_OUTERLEGGINGS ) && pantsColour != 0)
						{
							n->SetColour( pantsColour );
							n->SetDye( true );
						}

						if(( n->GetLayer() == IL_INNERSHIRT || n->GetLayer() == IL_ROBE ) && shirtColour != 0)
						{
							n->SetColour( shirtColour );
							n->SetDye( true );
						}
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
		BANK,
		GOLD,
		ITOTAL
	};

	CItem *CreatedItems[ITOTAL] = { NULL, NULL, NULL, NULL, NULL };
	UI16 ItemID, ItemColour;
	if( validHairStyle( hairStyle, mChar->GetID() ) )
	{
		ItemID				= hairStyle;
		ItemColour			= validHairColour( hairColour, mChar->GetID() );
		CreatedItems[HAIR]	= Items->CreateItem( tSock, mChar, ItemID, 1, ItemColour, OT_ITEM );
		if( CreatedItems[HAIR] != NULL )
		{
			CreatedItems[HAIR]->SetDecayable( false );
			CreatedItems[HAIR]->SetLayer( IL_HAIR );
			CreatedItems[HAIR]->SetCont( mChar );
		}
	}
	if( validBeard( facialHair, mChar->GetID() ) && ( mChar->GetID() == 0x0190 || mChar->GetID() == 0x029A )) //Male human or male gargoyle
	{
		ItemID				= facialHair;
		if( mChar->GetID() == 0x029A ) // gargoyle male
			ItemColour			= Capped( facialHairColour, static_cast< UI16 >(0x06E0), static_cast< UI16 >(0x076B) );
		else // human male
			ItemColour			= Capped( facialHairColour, static_cast< UI16 >(0x044E), static_cast< UI16 >(0x04AD) );
		CreatedItems[BEARD] = Items->CreateItem( tSock, mChar, ItemID, 1, ItemColour, OT_ITEM );
		if( CreatedItems[BEARD] != NULL )
		{
			CreatedItems[BEARD]->SetDecayable( false );
			CreatedItems[BEARD]->SetLayer( IL_FACIALHAIR );
			CreatedItems[BEARD]->SetCont( mChar );
		}
	}
	CreatedItems[PACK] = Items->CreateItem( tSock, mChar, 0x0E75, 1, 0, OT_ITEM );
	if( CreatedItems[PACK] != NULL )
	{
		mChar->SetPackItem( CreatedItems[PACK] );
		CreatedItems[PACK]->SetDecayable( false );
		CreatedItems[PACK]->SetLayer( IL_PACKITEM );
		CreatedItems[PACK]->SetCont( mChar );
		CreatedItems[PACK]->SetType( IT_CONTAINER );
		CreatedItems[PACK]->SetDye( true );
	}
	CreatedItems[BANK] = Items->CreateItem( tSock, mChar, 0x09AB, 1, 0, OT_ITEM );
	if( CreatedItems[BANK] != NULL )
	{
		char temp[1024];
		sprintf( temp, Dictionary->GetEntry( 1283 ).c_str(), mChar->GetName().c_str() );
		CreatedItems[BANK]->SetName( temp );
		CreatedItems[BANK]->SetDecayable( false );
		CreatedItems[BANK]->SetLayer( IL_BANKBOX );
		CreatedItems[BANK]->SetType( IT_CONTAINER );
		CreatedItems[BANK]->SetTempVar( CITV_MOREX, 1 );
		CreatedItems[BANK]->SetOwner( mChar );
		CreatedItems[BANK]->SetCont( mChar );
	}

	std::vector< cSkillClass > vecSkills;
	char whichsect[15];
	for( UI08 sCtr = 0; sCtr < ALLSKILLS; ++sCtr )
		vecSkills.push_back( cSkillClass( sCtr, mChar->GetBaseSkill( sCtr ) ) );

	// Give scripted newbie items precedence over default clothing
	std::sort( vecSkills.rbegin(), vecSkills.rend() );

	UI08 numStartSkills = 3; // 0 included
	if( cwmWorldState->ServerData()->ExtendedStartingSkills() )
		numStartSkills = 4;

	for( UI08 i = 0; i < numStartSkills ; ++i )
	{
		if( vecSkills[i].value > 0 )
		{
			sprintf( whichsect, "BESTSKILL %i", vecSkills[i].skill );
			addNewbieItem( tSock, mChar, whichsect, 0, 0 );
		}
	}

	// Add default for all characters
	addNewbieItem( tSock, mChar, "DEFAULT ALL", pantsColour, shirtColour );

	// Add gender/race-specific items
	if( mChar->GetID() == 0x0190 )
		addNewbieItem( tSock, mChar, "DEFAULT MALE", pantsColour, shirtColour );
	else if( mChar->GetID() == 0x0191 )
		addNewbieItem( tSock, mChar, "DEFAULT FEMALE", pantsColour, shirtColour );
	else if( mChar->GetID() == 0x025D )
		addNewbieItem( tSock, mChar, "DEFAULT ELF MALE", pantsColour, shirtColour );
	else if( mChar->GetID() == 0x025E )
		addNewbieItem( tSock, mChar, "DEFAULT ELF FEMALE", pantsColour, shirtColour );
	else if( mChar->GetID() == 0x029A )
		addNewbieItem( tSock, mChar, "DEFAULT GARG FEMALE", pantsColour, shirtColour );
	else if( mChar->GetID() == 0x029B )
		addNewbieItem( tSock, mChar, "DEFAULT GARG FEMALE", pantsColour, shirtColour );
	
	// Give the character some gold
	CreatedItems[GOLD] = Items->CreateScriptItem( tSock, mChar, "0x0EED", cwmWorldState->ServerData()->ServerStartGold(), OT_ITEM, true );
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
	// --> I split certain things out into a few new functions to make this a bit more manageable

	if( tSock != NULL )
	{
		CChar *mChar = static_cast< CChar * >(ObjectFactory::getSingleton().CreateObject( OT_CHAR ));
		if( mChar != NULL )
		{
			CPClientVersion verCheck;
			tSock->Send( &verCheck );

			tSock->CurrcharObj( mChar );
			mChar->SetName( charName );
			Accounts->AddCharacter( tSock->AcctNo(), mChar );
			CAccountBlock &actbRec	= tSock->GetAccount();
			if( actbRec.dwInGame != INVALIDSERIAL )
				actbRec.dwInGame = mChar->GetSerial();

			if( actbRec.dwInGame == INVALIDSERIAL || actbRec.dwInGame == mChar->GetSerial() )
			{
				actbRec.dwInGame = mChar->GetSerial();
				mChar->SetAccount( actbRec );
			}

			SetNewCharGender( mChar );

			mChar->SetPriv( cwmWorldState->ServerData()->ServerStartPrivs() );
			
			CAccountBlock& actbTemp2 = mChar->GetAccount();
			if( actbTemp2.wAccountIndex != AB_INVALID_ID && actbTemp2.wFlags.test( AB_FLAGS_GM ) )
			{ 
				mChar->SetPriv( 0xFF );
				mChar->SetCommandLevel( CL_GM );
			}

			if( tSock->ClientType() == CV_SA3D || tSock->ClientType() == CV_HS3D )
				locationNumber = clientFlags;
			LPSTARTLOCATION toGo = cwmWorldState->ServerData()->ServerLocation( locationNumber );
			if( toGo == NULL )
				toGo = cwmWorldState->ServerData()->ServerLocation( 0 );
			mChar->SetLocation( toGo->x, toGo->y, static_cast<SI08>(toGo->z) );
			mChar->SetDir( SOUTH );

			SetNewCharSkillsStats( mChar );

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
//|	Function	-	void SetNewCharStatsSkills( CChar *mChar )
//|	Programmer	-	Xuri
//o---------------------------------------------------------------------------o
//|	Purpose		-	Sets skills and stats for newly created characters
//o---------------------------------------------------------------------------o
void CPICreateCharacter::SetNewCharSkillsStats( CChar *mChar )
{
	SI32 totalstats, totalskills;
	UI08 i;
	R32 percheck;

	if( profession != 0 ) //player picked a character profession during character creation
	{
		bool extStats = cwmWorldState->ServerData()->ExtendedStartingStats();
		bool extSkills = cwmWorldState->ServerData()->ExtendedStartingSkills();

		// Which skills and stats a character starts with depends on chosen profession,
		// plus ExtendedStartingStats/Skills-setting in UOX.INI. 
		// Matches old (4.x - 7.0.8.x) or new (7.0.9+) profession-templates in client, but we
		// shouldn't trust client-data!
		switch( profession )
		{
		case 1: // Warrior
			skill[0] = ( extSkills ? 40 : 27 ); // Swordsmanship or Tactics
			skill[1] = ( extSkills ? 27 : 17 );	// Tactics or Healing
			skill[2] = ( extSkills ? 17 : 40 );	// Healing or Swordsmanship
			skill[3] = ( extSkills ? 1 : 0xFF ); // Anatomy or nothing
			skillValue[0] = ( extSkills ? 30 : 50 ); // 30 or 50 skillpoints
			skillValue[1] = ( extSkills ? 30 : 45 ); // 30 or 45 skillpoints
			skillValue[2] = ( extSkills ? 30 : 5 ); // 30 or 5 skillpoints
			skillValue[3] = ( extSkills ? 30 : 0 ); // 30 or 0 skillpoints
			mChar->SetStrength( extStats ? 45 : 35 );
			mChar->SetDexterity( 35 );
			mChar->SetIntelligence( 10 );
			break;
		case 2: // Mage
			skill[0] = ( extSkills ? 25 : 25 ); // Magery or Magery
			skill[1] = ( extSkills ? 16 : 46 ); // Evaulate Intelligence or Meditation
			skill[2] = ( extSkills ? 46 : 43 ); // Meditation or Wrestling
			skill[3] = ( extSkills ? 43 : 0xFF ); // Wrestling or nothing
			skillValue[0] = ( extSkills ? 30 : 50 ); // 30 or 50 skillpoints
			skillValue[1] = ( extSkills ? 30 : 50 ); // 30 or 50 skillpoints
			skillValue[2] = ( extSkills ? 30 : 0 ); // 30 or 0 skillpoints
			skillValue[3] = ( extSkills ? 30 : 0 ); // 30 or 0 skillpoints
			mChar->SetStrength( 25 );
			mChar->SetDexterity( extStats ? 20 : 10 );
			mChar->SetIntelligence( 45 );
			break;
		case 3: // Blacksmith
			skill[0] = ( extSkills ? 7 : 7 );	// Blacksmithing
			skill[1] = ( extSkills ? 45 : 37 );	// Mining or Tinkering
			skill[2] = ( extSkills ? 37 : 45 );	// Tinkering or Mining
			skill[3] = ( extSkills ? 34 : 0xFF );	// Tailoring or nothing
			skillValue[0] = ( extSkills ? 30 : 50 ); // 30 or 50 skillpoints
			skillValue[1] = ( extSkills ? 30 : 45 ); // 30 or 45 skillpoints
			skillValue[2] = ( extSkills ? 30 : 5 ); // 30 or 5 skillpoints
			skillValue[3] = ( extSkills ? 30 : 0 ); // 30 or 0 skillpoints
			mChar->SetStrength( 60 );
			mChar->SetDexterity( extStats ? 15 : 10 );
			mChar->SetIntelligence( extStats ? 15 : 10);
			break;
		case 4: // Necromancer
			skill[0] = ( extSkills ? 49 : 49 );	// Necromancy
			skill[1] = ( extSkills ? 32 : 40 );	// Spirit Speak or Swordsmanship
			skill[2] = ( extSkills ? 40 : 27 );	// Swordsmanship or Tactics
			skill[3] = ( extSkills ? 46 : 0xFF );	// Meditation or nothing
			skillValue[0] = ( extSkills ? 30 : 50 ); // 30 or 50 skillpoints
			skillValue[1] = ( extSkills ? 30 : 30 ); // 30 or 30 skillpoints
			skillValue[2] = ( extSkills ? 30 : 20 ); // 30 or 20 skillpoints
			skillValue[3] = ( extSkills ? 30 : 0 ); // 30 or 0 skillpoints
			mChar->SetStrength( 25 );
			mChar->SetDexterity( 20 );
			mChar->SetIntelligence( extStats ? 45 : 35 );
			break;
		case 5: // Paladin
			skill[0] = ( extSkills ? 51 : 51 );	// Chivalry
			skill[1] = ( extSkills ? 40 : 27 );	// Swordsmanship or Tactics
			skill[2] = ( extSkills ? 27 : 43 );	// Tactics or Wrestling
			skill[3] = ( extSkills ? 50 : 0xFF );	// Focus or nothing
			skillValue[0] = ( extSkills ? 30 : 50 ); // 30 or 50 skillpoints
			skillValue[1] = ( extSkills ? 30 : 50 ); // 30 or 50 skillpoints
			skillValue[2] = ( extSkills ? 30 : 0 ); // 30 or 0 skillpoints
			skillValue[3] = ( extSkills ? 30 : 0 ); // 30 or 0 skillpoints
			mChar->SetStrength( 45 );
			mChar->SetDexterity( 20 );
			mChar->SetIntelligence( extStats ? 25 : 15 );
			break;
		case 6: // Samurai
			skill[0] = ( extSkills ? 52 : 52 );	// Bushido
			skill[1] = ( extSkills ? 40 : 40 );	// Swordsmanship
			skill[2] = ( extSkills ? 50 : 43 );	// Focus or Wrestling
			skill[3] = ( extSkills ? 5 : 0xFF );	// Parrying or nothing
			skillValue[0] = ( extSkills ? 30 : 50 ); // 30 or 50 skillpoints
			skillValue[1] = ( extSkills ? 30 : 50 ); // 30 or 50 skillpoints
			skillValue[2] = ( extSkills ? 30 : 0 ); // 30 or 0 skillpoints
			skillValue[3] = ( extSkills ? 30 : 0 ); // 30 or 0 skillpoints
			mChar->SetStrength( 40 );
			mChar->SetDexterity( 30 );
			mChar->SetIntelligence( extStats ? 20 : 10 );
			break;
		case 7: // Ninja
			skill[0] = ( extSkills ? 53 : 53 ); // Ninjitsu or Ninjitsu
			skill[1] = ( extSkills ? 42 : 21 ); // Fencing or Hiding
			skill[2] = ( extSkills ? 21 : 43 ); // Hiding or Wrestling
			skill[3] = ( extSkills ? 47 : 0xFF ); // Stealth or nothing
			skillValue[0] = ( extSkills ? 30 : 50 ); // 30 or 50 skillpoints
			skillValue[1] = ( extSkills ? 30 : 50 ); // 30 or 50 skillpoints
			skillValue[2] = ( extSkills ? 30 : 0 ); // 30 or 0 skillpoints
			skillValue[3] = ( extSkills ? 30 : 0 ); // 30 or 0 skillpoints
			mChar->SetStrength( 40 );
			mChar->SetDexterity( 30 );
			mChar->SetIntelligence( extStats ? 20 : 10 );
			break;
		default:
			Console.Error( "Character created with invalid profession - no skills or stats assigned! (0x%X, %s)", mChar->GetSerial(), mChar->GetName().c_str() );
			break;
		}
	}
	else // Player did not pick a profession during character creation, but chose to customize his character
	{
		//	Date Unknown - Thyme - Modified to fit in with new client, and 80 total starting stats. The highest any one stat can be is 60, and the lowest is 10.
		mChar->SetStrength( Capped( str, static_cast< UI08 >(10), static_cast< UI08 >(60) ) );
		mChar->SetDexterity( Capped( dex, static_cast< UI08 >(10), static_cast< UI08 >(60) ) );
		mChar->SetIntelligence( Capped( intel, static_cast< UI08 >(10), static_cast< UI08 >(60) ) );

		totalstats = mChar->GetStrength() + mChar->GetDexterity() + mChar->GetIntelligence();
		if( totalstats != 80 && !cwmWorldState->ServerData()->ExtendedStartingStats() )
		{
			if( totalstats > 80 )
			{
				// If ExtendedStartingStats() is false, allow a total of 80 starting statpoints
				Console.Error( "Character created with invalid stats (over 80 total): 0x%X, (%s)", mChar->GetSerial(), mChar->GetName().c_str() );
				percheck = ( mChar->GetStrength() / (R32)totalstats );
				mChar->SetStrength( static_cast< UI08 >(Capped( percheck * 80, 10.0f, 60.0f ) ) );
				percheck = ( mChar->GetDexterity() / (R32)totalstats );
				mChar->SetDexterity( static_cast<UI08>(Capped( percheck * 80, 10.0f, 60.0f ) ) );
				percheck = ( mChar->GetIntelligence() / (R32)totalstats );
				mChar->SetIntelligence( static_cast<UI08>(Capped( percheck * 80, 10.0f, 60.0f ) ) );
			}
			else
			{
				Console.Error( "Character created with invalid stats (under 80 total): 0x%X, (%s)", mChar->GetSerial(), mChar->GetName().c_str() );
			}
		}
		else if( totalstats != 90 && cwmWorldState->ServerData()->ExtendedStartingStats() )
		{
			if( totalstats > 90 )
			{
				// If ExtendedStartingStats() is true, allow a total of 90 starting statpoints
				Console.Error( "Character created with invalid stats (over 90 total): 0x%X, (%s)", mChar->GetSerial(), mChar->GetName().c_str() );
				percheck = ( mChar->GetStrength() / (R32)totalstats );
				mChar->SetStrength( static_cast< UI08 >(Capped( percheck * 90, 10.0f, 60.0f ) ) );
				percheck = ( mChar->GetDexterity() / (R32)totalstats );
				mChar->SetDexterity( static_cast<UI08>(Capped( percheck * 90, 10.0f, 60.0f ) ) );
				percheck = ( mChar->GetIntelligence() / (R32)totalstats );
				mChar->SetIntelligence( static_cast<UI08>(Capped( percheck * 90, 10.0f, 60.0f ) ) );
			}
			else
			{
				Console.Error( "Character created with invalid stats (under 90 total): 0x%X, (%s)", mChar->GetSerial(), mChar->GetName().c_str() );
			}
		}

		if( skillValue[0] > 50 )
			skillValue[0] = 50;
		totalskills = skillValue[0];
		if( skillValue[1] > 50 )
			skillValue[1] = 50;
		totalskills += skillValue[1];
		if( skillValue[2] > 50 )
			skillValue[2] = 50;
		if( !cwmWorldState->ServerData()->ExtendedStartingSkills() )
		{
			if( skillValue[2] + totalskills > 100 )
				skillValue[2] = static_cast< UI08 >(100 - totalskills);
			totalskills += skillValue[2];
			if( totalskills < 100 )
			{
				Console.Error( "Character created with invalid skills (under 100 total): 0x%X, (%s)", mChar->GetSerial(), mChar->GetName().c_str() );
			}
		}
		else // If ExtendedStartingSkills is enabled, allow for the fourth starting skill
		{
			if( skillValue[2] + totalskills > 120 )
				skillValue[2] = static_cast< UI08 >(120 - totalskills);
			totalskills += skillValue[2];
			if( skillValue[3] > 50 )
				skillValue[3] = 50;
			if( skillValue[3] + totalskills > 120 )
				skillValue[3] = static_cast< UI08 >(120 - totalskills);
			totalskills += skillValue[3];
			if( totalskills < 120 )
			{
				Console.Error( "Character created with invalid skills (under 120 total): 0x%X, (%s)", mChar->GetSerial(), mChar->GetName().c_str() );
			}
		}
	}

	mChar->SetHP( mChar->GetMaxHP() );
	mChar->SetStamina( mChar->GetMaxStam() );
	mChar->SetMana( mChar->GetMaxMana() );

	mChar->SetRace( 0 );
	mChar->SetRaceGate( 65535 );
	UI08 firstSkill		= skill[0];
	UI08 secondSkill	= skill[1];
	UI08 thirdSkill		= skill[2];
	UI08 fourthSkill	= skill[3];

	for( i = 0; i < ALLSKILLS; ++i )
	{
		mChar->SetBaseSkill( 0, i );
		if( i == firstSkill )
			mChar->SetBaseSkill( (UI16)(skillValue[0] * 10), i );
		else if( i == secondSkill )
			mChar->SetBaseSkill( (UI16)(skillValue[1] * 10), i );
		else if( i == thirdSkill )
			mChar->SetBaseSkill( (UI16)(skillValue[2] * 10), i );
		else if( i == fourthSkill )
		{
			if( cwmWorldState->ServerData()->ExtendedStartingSkills() )
				mChar->SetBaseSkill( (UI16)(skillValue[3] * 10), i );
		}
		Skills->updateSkillLevel( mChar, i );
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void SetNewCharGender( CChar *mChar )
//|	Programmer	-	Xuri
//o---------------------------------------------------------------------------o
//|	Purpose		-	Sets gender for newly created characters
//o---------------------------------------------------------------------------o
void CPICreateCharacter::SetNewCharGender( CChar *mChar )
{
	UI16 pGenderID = 0x0190;
	bool gargCreation = false;
	bool elfCreation = false;
	if( cwmWorldState->ServerData()->GetClientFeature( CF_BIT_SA ) )
		gargCreation = true;
	if( cwmWorldState->ServerData()->GetServerFeature( SF_BIT_ML ) )
		elfCreation = true;

	if( tSock->ClientType() == CV_SA3D || tSock->ClientType() == CV_HS3D )
	{
		switch( sex )
		{
		case 0:
			switch( race )
			{
			case 1: pGenderID = 0x0190; break;	// human male
			case 2:	pGenderID = ( elfCreation ? 0x025D : 0x0190 ); break; // elf male
			case 3: pGenderID = ( gargCreation ? 0x029A : 0x0190 );	break;	// Gargoyle male
			}
			break;
		case 1:
			switch( race )
			{
			case 1: pGenderID = 0x0191;	break;	// human female
			case 2: pGenderID = ( elfCreation ? 0x025E : 0x0191 ); break; // elf female
			case 3: pGenderID = ( gargCreation ? 0x029B : 0x0191 );	break;	// Gargoyle female
			}
			break;
		}
	}
	else if( tSock->ClientType() >= CV_SA2D )
	{
		switch( sex )
		{
		case 0:		pGenderID = 0x0190; break;	// human male
		case 1:		pGenderID = 0x0191;	break;	// human female
		case 2:		pGenderID = 0x0190; break;	// human male
		case 3:		pGenderID = 0x0191;	break;	// human female
		case 4:		pGenderID = ( elfCreation ? 0x025D : 0x0190 ); break; // elf male
		case 5:		pGenderID = ( elfCreation ? 0x025E : 0x0191 ); break; // elf female
		case 6:		pGenderID = ( gargCreation ? 0x029A : 0x0190 );	break;	// Gargoyle male
		case 7:		pGenderID = ( gargCreation ? 0x029B : 0x0191 );	break;	// Gargoyle female
		default:	break;
		}
	}
	else
	{
		switch( sex )
		{
		case 0:		pGenderID = 0x0190; break;	// human male
		case 1:		pGenderID = 0x0191;	break;	// human female
		case 2:		pGenderID = ( elfCreation ? 0x025D : 0x0190 ); break; // elf male (human if server doesn't allow elf but client tried anyway)
		case 3:		pGenderID = ( elfCreation ? 0x025E : 0x0191 ); break; // elf female (human if server doesn't allow elf but client tried anyway)
		default:						break;
		}
	}

	mChar->SetID( pGenderID );
	mChar->SetOrgID( pGenderID );

	mChar->SetSkin( validSkinColour( skinColour, pGenderID ) | 0x8000 );
	mChar->SetOrgSkin( mChar->GetSkin() );
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

	UString updateData = "";
	for( UString tag = Updates->First(); !Updates->AtEnd(); tag = Updates->Next() )
	{
		updateData += Updates->GrabData() + " ";
	}
	CPUpdScroll toSend( 2 );
	toSend.AddString( updateData.c_str() );
	toSend.Finalize();
	s->Send( &toSend );
}

void sysBroadcast( const std::string& txt );
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
			CPClientVersion verCheck;
			mSock->Send( &verCheck );

			CAccountBlock& actbTemp = mSock->GetAccount();
			if( actbTemp.wAccountIndex != AB_INVALID_ID )
			{
				actbTemp.wFlags.set( AB_FLAGS_ONLINE, true );
			}

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

			CPEnableMapDiffs mDiff;
			mSock->Send( &mDiff );

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

			mSock->sysmessage( "%s v%s.%s [%s] Compiled by %s ", CVersionClass::GetProductName().c_str(), CVersionClass::GetVersion().c_str(), CVersionClass::GetBuild().c_str(), OS_STR, CVersionClass::GetName().c_str() );
			mSock->sysmessage( "Programmed by: %s", CVersionClass::GetProgrammers().c_str() );

			if( cwmWorldState->ServerData()->ServerJoinPartAnnouncementsStatus() )
			{
				char temp[1024];
				sprintf( temp, Dictionary->GetEntry( 1208 ).c_str(), mChar->GetName().c_str() );//message upon entering a server
				sysBroadcast( temp );//message upon entering a server
			}
			updates( mSock );
#if defined( _MSC_VER )
#pragma note( "HTML Status for Players" )
#endif
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
    
			if( mChar->WorldNumber() > 0 )
			{
				mSock->sysmessage( "Adjusting character position..." );
				mChar->Teleport();
			}
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	CItem *CreateCorpseItem( CChar &mChar )
//|	Programmer	-	giwo
//o---------------------------------------------------------------------------o
//|	Purpose		-	Generates a corpse or backpack based on the character killed.
//o---------------------------------------------------------------------------o
CItem *CreateCorpseItem( CChar& mChar, bool createPack, UI08 fallDirection )
{
	CItem *iCorpse = NULL;
	if( !createPack )
	{
		iCorpse = Items->CreateItem( NULL, &mChar, 0x2006, 1, mChar.GetSkin(), OT_ITEM );
		if( !ValidateObject( iCorpse ) )
			return NULL;

		char temp[512];
		sprintf( temp, Dictionary->GetEntry( 1612 ).c_str(), mChar.GetName().c_str() );
		iCorpse->SetName( temp );
		iCorpse->SetCarve( mChar.GetCarve() );
		iCorpse->SetMovable( 2 );//non-movable
		if( fallDirection )
			iCorpse->SetDir( mChar.GetDir() | 0x80 );
		else
			iCorpse->SetDir( mChar.GetDir() );

		iCorpse->SetAmount( mChar.GetID() );
		iCorpse->SetWeightMax( 50000 ); // 500 stones
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
	iCorpse->SetTempVar( CITV_MOREY, 2, cwmWorldState->creatures[mChar.GetID()].IsHuman() );
	iCorpse->SetTempVar( CITV_MOREZ, mChar.GetFlag() );
	iCorpse->SetTempTimer( cwmWorldState->GetUICurrentTime() );
	if( !mChar.IsNpc() )
	{
		iCorpse->SetOwner( &mChar );
		iCorpse->SetDecayTime( BuildTimeValue( cwmWorldState->ServerData()->SystemTimer( tSERVER_CORPSEDECAY ) ) );
	}
	else
		iCorpse->SetDecayTime( BuildTimeValue( cwmWorldState->ServerData()->SystemTimer( tSERVER_DECAY ) ) );

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
	CItem *dupeItem		= NULL;
	bool packIsValid	= ValidateObject( packItem );
	for( CItem *j = mChar.FirstItem(); !mChar.FinishedItems(); j = mChar.NextItem() )
	{
		if( !ValidateObject( j ) )
			continue;

		ItemLayers iLayer = j->GetLayer();

		switch( iLayer )
		{
		case IL_NONE:
		case IL_SELLCONTAINER:
		case IL_BOUGHTCONTAINER:
		case IL_BUYCONTAINER:
		case IL_BANKBOX:
			continue;
		case IL_HAIR:
		case IL_FACIALHAIR:
			dupeItem = j->Dupe();
			dupeItem->SetCont( iCorpse );
			dupeItem->SetName( "Hair/Beard" );
			dupeItem->SetX( 0x47 );
			dupeItem->SetY( 0x93 );
			dupeItem->SetZ( 0 );
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

void killTrades( CChar *i );
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
		pSock = mChar->GetSocket();

	DismountCreature( mChar );

	if( pSock != NULL )
		killTrades( mChar );

	if( mChar->GetID() != mChar->GetOrgID() )
		mChar->SetID( mChar->GetOrgID() );

	bool createPack = ( mChar->GetID( 2 ) == 0x0D || mChar->GetID( 2 ) == 0x0F || mChar->GetID( 2 ) == 0x10 || mChar->GetID() == 0x023E );

	UI08 fallDirection = (UI08)(RandomNum( 0, 100 ) % 2);
	mChar->SetDead( true );

	CItem *iCorpse = CreateCorpseItem( (*mChar), createPack, fallDirection );
	if( iCorpse != NULL )
	{
		MoveItemsToCorpse( (*mChar), iCorpse, createPack );
		if( cwmWorldState->ServerData()->DeathAnimationStatus() )
			Effects->deathAction( mChar, iCorpse, fallDirection );
	}
	Effects->playDeathSound( mChar );

	mChar->SetWar( false );
	mChar->StopSpell();
	mChar->SetHP( 0 );
	mChar->SetPoisoned( 0 );
	mChar->SetPoisonStrength( 0 );

	if( !mChar->IsNpc() )
	{
		switch( mChar->GetOrgID() )
		{
		case 0x0190:	mChar->SetID( 0x0192 );	break;	// human male
		default:
		case 0x0191:	mChar->SetID( 0x0193 );	break;	// human female, or others
		case 0x025D:	mChar->SetID( 0x025F );	break;	// elf male
		case 0x025E:	mChar->SetID( 0x0260 );	break;	// elf female
		case 0x029A:	mChar->SetID( 0x02B6 ); break;	// gargoyle male
		case 0x029B:	mChar->SetID( 0x02B7 ); break;	// gargoyle female
		case 0x00B7:
		case 0x00B9:
		case 0x02EE:	mChar->SetID( 0x0192 );	break;	// savage male
		case 0x00B8:
		case 0x00BA:
		case 0x02EF:	mChar->SetID( 0x0192 );	break;	// savage female		
		}

		CItem *c = Items->CreateItem( NULL, mChar, 0x204E, 1, 0, OT_ITEM );
		if( c == NULL )
			return;
		c->SetName( Dictionary->GetEntry( 1610 ) );
		mChar->SetRobe( c->GetSerial() );
		c->SetLayer( IL_ROBE );
		if( c->SetCont( mChar ) )
			c->SetResist( 1, PHYSICAL );

		if( mChar->GetAccount().wAccountIndex != AB_INVALID_ID )
		{
			if( pSock != NULL )
			{
				CPResurrectMenu toSend( 0 );
				pSock->Send( &toSend );
			}
			// The fade to gray is done by the CPResurrectMenu packet
		}
	}
	
	UI16 targTrig		= mChar->GetScriptTrigger();
	cScript *toExecute	= JSMapping->GetScript( targTrig );
	if( toExecute != NULL )
		toExecute->OnDeath( mChar );

	if( mChar->IsNpc() )
		mChar->Delete();
	else
		mChar->Dirty( UT_LOCATION );
}

}
