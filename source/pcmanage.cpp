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
#include "PartySystem.h"
#include "classes.h"
#include "townregion.h"
#include "Dictionary.h"
#include "cEffects.h"
#include "cRaces.h"
#include "StringUtility.hpp"
#include "cServerData.h"
#include "ObjectFactory.h"
#include <algorithm>

using namespace std::string_literals;

template< class T >
T Capped( const T value, const T minimum, const T maximum )
{
	return std::max( std::min( value, maximum ), minimum );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ValidHairStyle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if selected hair is a valid hair type
//o------------------------------------------------------------------------------------------------o
bool ValidHairStyle( UI16 id, UI16 bodyId )
{
	bool rValue = false;
	switch( bodyId )
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
					rValue = true;
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
					rValue = true;
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
					rValue = true;
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
					rValue = true;
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
					rValue = true;
					break;
				default:
					break;
			}
			break;
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ValidBeard()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if selected beard is a valid beard type
//|					 in his pack, on his paperdoll or in his hands
//o------------------------------------------------------------------------------------------------o
bool ValidBeard( UI16 id, UI16 bodyId )
{
	bool rValue = false;
	switch( bodyId )
	{
		case 0x029A:	// gargoyle male
			switch( id )
			{
				case 0x42AD:
				case 0x42AE:
				case 0x42AF: //1701?
				case 0x42B0:
					rValue = true;
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
					rValue = true;
					break;
			}
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ValidSkinColour()
//|	Date		-	22nd January, 2006
//|	Return		-	A validated skin colour (capped into range)
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if selected skin colour is a valid colour, based on
//|					 the body types involved (elves and humans not the same)
//o------------------------------------------------------------------------------------------------o
COLOUR ValidSkinColour( UI16 id, UI16 bodyId )
{
	COLOUR rValue;
	switch( bodyId )
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
				case 2307:	rValue = id;	break;
				default:	rValue = 191;	break;	// nope, it's not, let's default
			}
			break;
		case 0x029A:
		case 0x029B:	//gargoyle male/female
			rValue = Capped( id, static_cast<UI16>( 0x06DB ), static_cast<UI16>( 0x06F3 ));
			break;
		default:	// human male/female
			rValue = Capped( id, static_cast<UI16>( 0x03EA ), static_cast<UI16>( 0x0422 ));
			break;
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ValidHairColour()
//|	Date		-	22nd January, 2006
//|	Return		-	A validated hair colour (capped into range)
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if selected hair colour is a valid colour, based on
//|					 the body types involved (elves and humans not the same)
//o------------------------------------------------------------------------------------------------o
COLOUR ValidHairColour( UI16 id, UI16 bodyId )
{
	COLOUR rValue;
	switch( bodyId )
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
				case 2131:	rValue = id;	break;
				default:	rValue = 52;	break;	// nope, it's not, let's default
			}
			break;
		case 0x29A:	// gargoyle male
		case 0x29B: // gargoyle female
			// not all colors used, but easier than specialcasing each one
			rValue = Capped( id, static_cast<UI16>( 0x06E0 ), static_cast<UI16>( 0x076B ));
			break;
		default:	// human male/female
			rValue = Capped( id, static_cast<UI16>( 0x044E ), static_cast<UI16>( 0x04AD ));
			break;
	}
	return rValue;
}

void StartChar( CSocket *mSock, bool onCreate = false );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CPIPlayCharacter::Handle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Called when "Play Character" button is hit
//o------------------------------------------------------------------------------------------------o
bool CPIPlayCharacter::Handle( void )
{
	if( tSock != nullptr )
	{
		if( tSock->AcctNo() != AB_INVALID_ID )
		{
			bool disconnect = false;
			CAccountBlock_st&  actbRec	= tSock->GetAccount();
			CChar *kChar			= nullptr;
			CChar *ourChar			= nullptr;
			if( actbRec.wAccountIndex == AB_INVALID_ID )
			{
				Network->Disconnect( tSock );
				disconnect = true;
			}
			else
			{
				ourChar = actbRec.lpCharacters[slotChosen];
				if( ourChar != nullptr )
				{
					if( !ourChar->IsNpc() && !ourChar->IsFree() )
					{
						kChar = ourChar;
					}
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
					{
						actbRec.dwInGame = kChar->GetSerial();
					}
					else
					{
						actbRec.dwInGame = INVALIDSERIAL;
					}

					if( actbRec.dwInGame == INVALIDSERIAL || actbRec.dwInGame == kChar->GetSerial() )
					{
						actbRec.dwInGame = kChar->GetSerial();
						kChar->SetTimer( tPC_LOGOUT, 0 );
						kChar->SetAccount( actbRec );
						tSock->CurrcharObj( kChar );
						StartChar( tSock );
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
		{
			Network->Disconnect( tSock );
		}
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CPIDeleteCharacter::Handle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Remove a character from the accounts system, due to an account gump button press
//o------------------------------------------------------------------------------------------------o
bool CPIDeleteCharacter::Handle( void )
{
	if( tSock != nullptr )
	{
		SI08 deleteResult = -1;
		CAccountBlock_st * actbTemp = &tSock->GetAccount();
		UI08 slot = tSock->GetByte( 0x22 );
		if( actbTemp->wAccountIndex != AB_INVALID_ID )
		{
			CChar *ourObj = actbTemp->lpCharacters[slot];
			if( ValidateObject( ourObj ))	// we have a char
			{
				deleteResult = Accounts->DelCharacter( actbTemp->wAccountIndex, slot );
				if( deleteResult == CDR_SUCCESS )
				{
					ourObj->Delete();
				}
			}
		}
		else
		{
			// Support for accounts. The current copy of the account isn't correct. So get a new copy to work with.
			Accounts->Load();
			actbTemp = &Accounts->GetAccountById( actbTemp->wAccountIndex );
		}

		// For modern clients (?) we need to send packets 0x85 and 0x86 here!
		if( deleteResult != CDR_SUCCESS )
		{
			// 0x85 - DeleteResult - only send if deletion failed!
			CPCharDeleteResult pckDelResult( static_cast<CharacterDeletionResult>( deleteResult ));
			tSock->Send( &pckDelResult );
		}
		else
		{
			UI08 charCount = 0;
			for( UI08 i = 0; i < 7; ++i )
			{
				if( ValidateObject( actbTemp->lpCharacters[i] ))
				{
					++charCount;
				}
			}

			// 0x86 - Resend Characters after Delete
			CharacterListUpdate pckCharList( charCount );
			for( UI08 i = 0; i < charCount; ++i )
			{
				if( ValidateObject( actbTemp->lpCharacters[i] ))
				{
					pckCharList.AddCharName( i, actbTemp->lpCharacters[i]->GetName() );
				}
				else
				{
					pckCharList.AddCharName( i, "" );
				}
			}

			// If no characters exist, send an empty character list
			if( charCount == 0 )
			{
				pckCharList.AddCharName( 0, "" );
			}
			tSock->Send( &pckCharList );
		}
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	AddNewbieItem()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a newbie item defined in newbie.dfn
//o------------------------------------------------------------------------------------------------o
//| Changes		-	PACKITEM now supports item,amount
//o------------------------------------------------------------------------------------------------o
auto AddNewbieItem( CSocket *socket, CChar *c, const char* str, COLOUR pantsColour, COLOUR shirtColour ) -> void
{
	auto newbieData = FileLookup->FindEntry( str, newbie_def );
	if( newbieData == nullptr )
		return;

	CItem *n = nullptr;
	for( const auto &sec : newbieData->collection() )
	{
		auto tag = sec->tag;
		auto data = sec->data;
		data = oldstrutil::trim( oldstrutil::removeTrailing( data, "//" ));
		if( !data.empty() )
		{
			auto UTag = oldstrutil::upper( tag );
			if( UTag == "PACKITEM" )
			{
				auto csecs = oldstrutil::sections( data, "," );
				if( csecs.size() > 1 )
				{
					UI16 nAmount = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 ));
					n = Items->CreateScriptItem( socket, c, oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nAmount, OT_ITEM, true );
				}
				else
				{
					n = Items->CreateScriptItem( socket, c, data.c_str(), 1, OT_ITEM, true );
				}
			}
			else if( UTag == "EQUIPITEM" )
			{
				UI16 itemHue = 0;
				std::string itemSection;
				auto csecs = oldstrutil::sections( data, "," );
				if( csecs.size() > 1 )
				{
					itemSection = oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" ));
					itemHue = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 ));
				}
				else
				{
					itemSection = data;
				}

				n = Items->CreateScriptItem( socket, c, itemSection.c_str(), 1, OT_ITEM, true, itemHue );
				if( n != nullptr && n->GetLayer() != IL_NONE )
				{
					bool conflictItem = true;
					CItem *j = c->GetItemAtLayer( n->GetLayer() );
					if( !ValidateObject( j ))
					{
						if( n->GetLayer() == IL_RIGHTHAND )
						{
							j = c->GetItemAtLayer( IL_LEFTHAND );
						}
						else if( n->GetLayer() == IL_LEFTHAND )
						{
							j = c->GetItemAtLayer( IL_RIGHTHAND );
						}

						// GetDir-check is to allow for torches and lanterns,
						// which use left-hand layer but are not 2-handers or shields
						if( ValidateObject( j ) && !n->IsShieldType() && ( n->GetDir() == 0 ))
						{
							if( j->IsShieldType() || j->GetDir() != 0 )
							{
								conflictItem = false;
							}
						}
						else
						{
							conflictItem = false;
						}
					}
					if( conflictItem )
					{
						n->SetCont( c->GetPackItem() );
					}
					else
					{
						n->SetCont( c );
					}

					//Apply the choosen colour
					if(( n->GetLayer() == IL_PANTS || n->GetLayer() == IL_OUTERLEGGINGS ) && pantsColour != 0 )
					{
						n->SetColour( pantsColour );
						n->SetDye( true );
					}

					if(( n->GetLayer() == IL_INNERSHIRT || n->GetLayer() == IL_ROBE ) && shirtColour != 0 )
					{
						n->SetColour( shirtColour );
						n->SetDye( true );
					}
				}
			}
			if( n != nullptr && !n->IsPileable() )
			{
				// Set item as newbiefied/blessed by default - as long as it's not pileable!
				n->SetNewbie( true );
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	NewbieItems()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds newbie items based on characters chosen skills
//o------------------------------------------------------------------------------------------------o
void CPICreateCharacter::NewbieItems( CChar *mChar )
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

	CItem *CreatedItems[ITOTAL] = { nullptr, nullptr, nullptr, nullptr, nullptr };
	UI16 ItemId, ItemColour;
	if( ValidHairStyle( hairStyle, mChar->GetId() ))
	{
		ItemId				= hairStyle;
		ItemColour			= ValidHairColour( hairColour, mChar->GetId() );
		CreatedItems[HAIR]	= Items->CreateItem( tSock, mChar, ItemId, 1, ItemColour, OT_ITEM );
		if( CreatedItems[HAIR] != nullptr )
		{
			CreatedItems[HAIR]->SetDecayable( false );
			CreatedItems[HAIR]->SetLayer( IL_HAIR );
			CreatedItems[HAIR]->SetCont( mChar );
			mChar->SetHairStyle( ItemId );
			mChar->SetHairColour( ItemColour );
		}
	}
	if( ValidBeard( facialHair, mChar->GetId() ) && ( mChar->GetId() == 0x0190 || mChar->GetId() == 0x029A )) //Male human or male gargoyle
	{
		ItemId				= facialHair;
		if( mChar->GetId() == 0x029A ) // gargoyle male
		{
			ItemColour			= Capped( facialHairColour, static_cast<UI16>( 0x06E0 ), static_cast<UI16>( 0x076B ));
		}
		else // human male
		{
			ItemColour			= Capped( facialHairColour, static_cast<UI16>( 0x044E ), static_cast<UI16>( 0x04AD ));
		}
		CreatedItems[BEARD] = Items->CreateItem( tSock, mChar, ItemId, 1, ItemColour, OT_ITEM );
		if( CreatedItems[BEARD] != nullptr )
		{
			CreatedItems[BEARD]->SetDecayable( false );
			CreatedItems[BEARD]->SetLayer( IL_FACIALHAIR );
			CreatedItems[BEARD]->SetCont( mChar );
			mChar->SetBeardStyle( ItemId );
			mChar->SetBeardColour( ItemColour );
		}
	}
	CreatedItems[PACK] = Items->CreateItem( tSock, mChar, 0x0E75, 1, 0, OT_ITEM );
	if( CreatedItems[PACK] != nullptr )
	{
		mChar->SetPackItem( CreatedItems[PACK] );
		CreatedItems[PACK]->SetWeightMax( cwmWorldState->ServerData()->MaxPlayerPackWeight() );
		CreatedItems[PACK]->SetMaxItems( cwmWorldState->ServerData()->MaxPlayerPackItems() );
		CreatedItems[PACK]->SetDecayable( false );
		CreatedItems[PACK]->SetLayer( IL_PACKITEM );
		CreatedItems[PACK]->SetCont( mChar );
		CreatedItems[PACK]->SetType( IT_CONTAINER );
		CreatedItems[PACK]->SetDye( true );
	}
	CreatedItems[BANK] = Items->CreateItem( tSock, mChar, 0x09AB, 1, 0, OT_ITEM );
	if( CreatedItems[BANK] != nullptr )
	{
		CreatedItems[BANK]->SetName( oldstrutil::format(1024,Dictionary->GetEntry( 1283 ), mChar->GetName().c_str() ));
		CreatedItems[BANK]->SetDecayable( false );
		CreatedItems[BANK]->SetLayer( IL_BANKBOX );
		CreatedItems[BANK]->SetType( IT_CONTAINER );
		CreatedItems[BANK]->SetTempVar( CITV_MOREX, 1 );
		CreatedItems[BANK]->SetWeightMax( cwmWorldState->ServerData()->MaxPlayerPackWeight() );
		CreatedItems[BANK]->SetMaxItems( cwmWorldState->ServerData()->MaxPlayerBankItems() );
		CreatedItems[BANK]->SetOwner( mChar );
		CreatedItems[BANK]->SetCont( mChar );
	}

	std::vector<cSkillClass> vecSkills;
	std::string whichsect;
	for( UI08 sCtr = 0; sCtr < ALLSKILLS; ++sCtr )
	{
		vecSkills.push_back( cSkillClass( sCtr, mChar->GetBaseSkill( sCtr )));
	}

	// Give scripted newbie items precedence over default clothing
	std::sort( vecSkills.rbegin(), vecSkills.rend() );

	UI08 numStartSkills = 3; // 0 included
	if( cwmWorldState->ServerData()->ExtendedStartingSkills() )
	{
		numStartSkills = 4;
	}

	for( UI08 i = 0; i < numStartSkills; ++i )
	{
		if( vecSkills[i].value > 0 )
		{
			whichsect = oldstrutil::format( "BESTSKILL %i", vecSkills[i].skill );
			AddNewbieItem( tSock, mChar, whichsect.c_str(), 0, 0 );
		}
	}

	// Add default for all characters
	AddNewbieItem( tSock, mChar, "DEFAULT ALL", pantsColour, shirtColour );

	// Add gender/race-specific items
	if( mChar->GetId() == 0x0190 )
	{
		AddNewbieItem( tSock, mChar, "DEFAULT MALE", pantsColour, shirtColour );
	}
	else if( mChar->GetId() == 0x0191 )
	{
		AddNewbieItem( tSock, mChar, "DEFAULT FEMALE", pantsColour, shirtColour );
	}
	else if( mChar->GetId() == 0x025D )
	{
		AddNewbieItem( tSock, mChar, "DEFAULT ELF MALE", pantsColour, shirtColour );
	}
	else if( mChar->GetId() == 0x025E )
	{
		AddNewbieItem( tSock, mChar, "DEFAULT ELF FEMALE", pantsColour, shirtColour );
	}
	else if( mChar->GetId() == 0x029A )
	{
		AddNewbieItem( tSock, mChar, "DEFAULT GARG FEMALE", pantsColour, shirtColour );
	}
	else if( mChar->GetId() == 0x029B )
	{
		AddNewbieItem( tSock, mChar, "DEFAULT GARG FEMALE", pantsColour, shirtColour );
	}

	// Give the character some gold
	if( cwmWorldState->ServerData()->ServerStartGold() > 0 )
	{
		CreatedItems[GOLD] = Items->CreateScriptItem( tSock, mChar, "0x0EED", cwmWorldState->ServerData()->ServerStartGold(), OT_ITEM, true );
	}
	else
	{
		CreatedItems[GOLD] = 0;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	bool CPICreateCharacter::Handle( void )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Character creation stuff
//o------------------------------------------------------------------------------------------------o
bool CPICreateCharacter::Handle( void )
{
	// This function needs to be decomposed
	// Way too large from a maintenance perspective
	// --> I split certain things out into a few new functions to make this a bit more manageable

	if( tSock != nullptr )
	{
		CChar *mChar = static_cast<CChar *>( ObjectFactory::GetSingleton().CreateObject( OT_CHAR ));
		if( mChar != nullptr )
		{
			CPClientVersion verCheck;
			tSock->Send( &verCheck );

			tSock->CurrcharObj( mChar );
			mChar->SetName( charName );
			Accounts->AddCharacter( tSock->AcctNo(), mChar );
			CAccountBlock_st &actbRec	= tSock->GetAccount();
			if( actbRec.dwInGame != INVALIDSERIAL )
			{
				actbRec.dwInGame = mChar->GetSerial();
			}

			if( actbRec.dwInGame == INVALIDSERIAL || actbRec.dwInGame == mChar->GetSerial() )
			{
				actbRec.dwInGame = mChar->GetSerial();
				mChar->SetAccount( actbRec );
			}

			SetNewCharGenderAndRace( mChar );

			mChar->SetPriv( cwmWorldState->ServerData()->ServerStartPrivs() );

			CAccountBlock_st& actbTemp2 = mChar->GetAccount();
			if( actbTemp2.wAccountIndex != AB_INVALID_ID && actbTemp2.wFlags.test( AB_FLAGS_GM ))
			{
				mChar->SetPriv( 0xFF );
				mChar->SetCommandLevel( CL_GM );
			}

			if( tSock->ClientType() == CV_SA3D || tSock->ClientType() == CV_HS3D )
			{
				locationNumber = clientFlags;
			}

			// Fetch player's chosen start location
			auto toGo = cwmWorldState->ServerData()->ServerLocation( locationNumber );

			CServerData *sd = cwmWorldState->ServerData();
			size_t serverCount = sd->NumServerLocations();

			// Use random start location if enabled in uox.ini
			if( cwmWorldState->ServerData()->ServerRandomStartingLocation() )
			{
				if( serverCount > 0 )
				{
					size_t rndStartingLocation = RandomNum( static_cast<size_t>( 0 ), serverCount - 1 );
					toGo = cwmWorldState->ServerData()->ServerLocation( rndStartingLocation );
				}
			}

			if( toGo == nullptr )
			{
				// Invalid locationNumber; check if there are ANY start locations loaded
				if( serverCount == 0 )
				{
					// No start locations found, use a default hardcoded one
					Console.Error( "No starting locations found in ini file; sending new character to Sweet Dreams Inn (1495, 1629, 10)." );
					SI16 startX;
					SI16 startY;
					SI08 startZ;
					UI08 startWorld;
					UI16 startInstanceId;
					startX = 1495;
					startY = 1629;
					startZ = 10;
					startWorld = 0;
					startInstanceId = 0;
					mChar->SetLocation( startX, startY, startZ, startWorld, startInstanceId );
				}
				else
				{
					// Use first start location, which we know exists
					toGo = cwmWorldState->ServerData()->ServerLocation( 0 );
					mChar->SetLocation( toGo->x, toGo->y, static_cast<SI08>( toGo->z ), static_cast<UI08>( toGo->worldNum ), static_cast<UI16>( toGo->instanceId ));
				}
			}
			else
			{
				mChar->SetLocation( toGo->x, toGo->y, static_cast<SI08>( toGo->z ), static_cast<UI08>( toGo->worldNum ), static_cast<UI16>( toGo->instanceId ));
			}
			mChar->SetDir( SOUTH );

			SetNewCharSkillsStats( mChar );

			NewbieItems( mChar );

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

			// Set character creation timestamp
			mChar->SetCreatedOn( GetMinutesSinceEpoch() );

			StartChar( tSock, true );
		}
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CPICreateCharacter::SetNewCharSkillsStats()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets skills and stats for newly created characters
//o------------------------------------------------------------------------------------------------o
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
				skill[0] = 25; // Magery
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
				skill[0] = 7;	// Blacksmithing
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
				skill[0] = 49;	// Necromancy
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
				skill[0] = 51;	// Chivalry
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
				skill[0] = 52;	// Bushido
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
				skill[0] = 53; // Ninjitsu
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
				Console.Error( oldstrutil::format( "Character created with invalid profession - no skills or stats assigned! (0x%X, %s)", mChar->GetSerial(), mChar->GetName().c_str() ));
				break;
		}
	}
	else // Player did not pick a profession during character creation, but chose to customize his character
	{
		//	Date Unknown - Modified to fit in with new client, and 80 total starting stats. The highest any one stat can be is 60, and the lowest is 10.
		mChar->SetStrength( Capped( str, static_cast<UI08>( 10 ), static_cast<UI08>( 60 )));
		mChar->SetDexterity( Capped( dex, static_cast<UI08>( 10 ), static_cast<UI08>( 60 )));
		mChar->SetIntelligence( Capped( intel, static_cast<UI08>( 10 ), static_cast<UI08>( 60 )));

		totalstats = mChar->GetStrength() + mChar->GetDexterity() + mChar->GetIntelligence();
		if( totalstats != 80 && !cwmWorldState->ServerData()->ExtendedStartingStats() )
		{
			if( totalstats > 80 )
			{
				// If ExtendedStartingStats() is false, allow a total of 80 starting statpoints
				Console.Error( oldstrutil::format( "Character created with invalid stats (over 80 total): 0x%X, (%s)", mChar->GetSerial(), mChar->GetName().c_str() ));
				percheck = ( mChar->GetStrength() / static_cast<R32>( totalstats ));
				mChar->SetStrength( static_cast<UI08>(Capped( percheck * 80, 10.0f, 60.0f )));
				percheck = ( mChar->GetDexterity() / static_cast<R32>( totalstats ));
				mChar->SetDexterity( static_cast<UI08>(Capped( percheck * 80, 10.0f, 60.0f )));
				percheck = ( mChar->GetIntelligence() / static_cast<R32>( totalstats ));
				mChar->SetIntelligence( static_cast<UI08>(Capped( percheck * 80, 10.0f, 60.0f )));
			}
			else
			{
				Console.Error( oldstrutil::format( "Character created with invalid stats (under 80 total): 0x%X, (%s)", mChar->GetSerial(), mChar->GetName().c_str() ));
			}
		}
		else if( totalstats != 90 && cwmWorldState->ServerData()->ExtendedStartingStats() )
		{
			if( totalstats > 90 )
			{
				// If ExtendedStartingStats() is true, allow a total of 90 starting statpoints
				Console.Error( oldstrutil::format( "Character created with invalid stats (over 90 total): 0x%X, (%s)", mChar->GetSerial(), mChar->GetName().c_str() ));
				percheck = ( mChar->GetStrength() / static_cast<R32>( totalstats ));
				mChar->SetStrength( static_cast<UI08>(Capped( percheck * 90, 10.0f, 60.0f )));
				percheck = ( mChar->GetDexterity() / static_cast<R32>( totalstats ));
				mChar->SetDexterity( static_cast<UI08>(Capped( percheck * 90, 10.0f, 60.0f )));
				percheck = ( mChar->GetIntelligence() / static_cast<R32>( totalstats ));
				mChar->SetIntelligence( static_cast<UI08>(Capped( percheck * 90, 10.0f, 60.0f )));
			}
			else
			{
				Console.Error( oldstrutil::format( "Character created with invalid stats (under 90 total): 0x%X, (%s)", mChar->GetSerial(), mChar->GetName().c_str() ));
			}
		}

		if( skillValue[0] > 50 )
		{
			skillValue[0] = 50;
		}
		totalskills = skillValue[0];
		if( skillValue[1] > 50 )
		{
			skillValue[1] = 50;
		}
		totalskills += skillValue[1];
		if( skillValue[2] > 50 )
		{
			skillValue[2] = 50;
		}
		if( !cwmWorldState->ServerData()->ExtendedStartingSkills() )
		{
			if( skillValue[2] + totalskills > 100 )
			{
				skillValue[2] = static_cast<UI08>( 100 - totalskills );
			}
			totalskills += skillValue[2];
			if( totalskills < 100 )
			{
				Console.Error( oldstrutil::format( "Character created with invalid skills (under 100 total): 0x%X, (%s)", mChar->GetSerial(), mChar->GetName().c_str() ));
			}
		}
		else // If ExtendedStartingSkills is enabled, allow for the fourth starting skill
		{
			if( skillValue[2] + totalskills > 120 )
			{
				skillValue[2] = static_cast<UI08>( 120 - totalskills );
			}
			totalskills += skillValue[2];
			if( skillValue[3] > 50 )
			{
				skillValue[3] = 50;
			}
			if( skillValue[3] + totalskills > 120 )
			{
				skillValue[3] = static_cast<UI08>( 120 - totalskills );
			}
			totalskills += skillValue[3];
			if( totalskills < 120 )
			{
				Console.Error( oldstrutil::format( "Character created with invalid skills (under 120 total): 0x%X, (%s)", mChar->GetSerial(), mChar->GetName().c_str() ));
			}
		}
	}

	mChar->SetHP( mChar->GetMaxHP() );
	mChar->SetStamina( mChar->GetMaxStam() );
	mChar->SetMana( mChar->GetMaxMana() );

	mChar->SetRaceGate( 65535 );
	UI08 firstSkill		= skill[0];
	UI08 secondSkill	= skill[1];
	UI08 thirdSkill		= skill[2];
	UI08 fourthSkill	= skill[3];

	for( i = 0; i < ALLSKILLS; ++i )
	{
		mChar->SetBaseSkill( 0, i );
		if( i == firstSkill )
		{
			mChar->SetBaseSkill( static_cast<UI16>( skillValue[0] * 10 ), i );
		}
		else if( i == secondSkill )
		{
			mChar->SetBaseSkill( static_cast<UI16>( skillValue[1] * 10 ), i );
		}
		else if( i == thirdSkill )
		{
			mChar->SetBaseSkill( static_cast<UI16>( skillValue[2] * 10 ), i );
		}
		else if( i == fourthSkill )
		{
			if( cwmWorldState->ServerData()->ExtendedStartingSkills() )
			{
				mChar->SetBaseSkill( static_cast<UI16>( skillValue[3] * 10 ), i );
			}
		}
		Skills->UpdateSkillLevel( mChar, i );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CPICreateCharacter::SetNewCharGenderAndRace()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets gender and race for newly created characters
//o------------------------------------------------------------------------------------------------o
void CPICreateCharacter::SetNewCharGenderAndRace( CChar *mChar )
{
	UI16 pGenderId = 0x0190;
	bool gargCreation = false;
	bool elfCreation = false;
	if( cwmWorldState->ServerData()->GetClientFeature( CF_BIT_SA ))
	{
		gargCreation = true;
	}
	if( cwmWorldState->ServerData()->GetClientFeature( CF_BIT_ML ))
	{
		elfCreation = true;
	}

	if( tSock->ClientType() == CV_SA3D || tSock->ClientType() == CV_HS3D )
	{
		switch( sex )
		{
			case 0:
				switch( race )
				{
					case 0: pGenderId = 0x0190; break;	// human male
					case 1:	pGenderId = ( elfCreation ? 0x025D : 0x0190 ); break; // elf male
					case 2: pGenderId = ( gargCreation ? 0x029A : 0x0190 );	break;	// Gargoyle male
				}
				break;
			case 1:
				switch( race )
				{
					case 0: pGenderId = 0x0191;	break;	// human female
					case 1: pGenderId = ( elfCreation ? 0x025E : 0x0191 ); break; // elf female
					case 2: pGenderId = ( gargCreation ? 0x029B : 0x0191 );	break;	// Gargoyle female
				}
				break;
			default:
				break;
		}

		// Set race to match data sent from client. This should correspond with race setup in races.dfn
		// 0 = human, 1 = elf, 2 = gargoyle
		mChar->SetRace( race );
	}
	else if( tSock->ClientType() >= CV_SA2D )
	{
		switch( sex )
		{
			case 0:
				// human male
				pGenderId = 0x0190;
				mChar->SetRace( 0 ); // Human
				break;
			case 1:	// human female
				pGenderId = 0x0191;
				mChar->SetRace( 0 ); // Human
				break;
			case 2:	// human male
				pGenderId = 0x0190;
				mChar->SetRace( 0 ); // Human
				break;
			case 3:	// human female
				pGenderId = 0x0191;
				mChar->SetRace( 0 ); // Human
				break;
			case 4: // elf male
				pGenderId = ( elfCreation ? 0x025D : 0x0190 );
				mChar->SetRace( 1 ); // Elf
				break;
			case 5: // elf female
				pGenderId = ( elfCreation ? 0x025E : 0x0191 );
				mChar->SetRace( 1 ); // Elf
				break;
			case 6: // Gargoyle male
				pGenderId = ( gargCreation ? 0x029A : 0x0190 );
				mChar->SetLevitate( true ); // Enable potential for flying
				mChar->SetRace( 2 ); // Gargoyle
				break;
			case 7: // Gargoyle female
				pGenderId = ( gargCreation ? 0x029B : 0x0191 );
				mChar->SetLevitate( true ); // Enable potential for flying
				mChar->SetRace( 2 ); // Gargoyle
				break;
			default:
				break;
		}
	}
	else
	{
		switch( sex )
		{
			case 0:		pGenderId = 0x0190; break;	// human male
			case 1:		pGenderId = 0x0191;	break;	// human female
			case 2:		pGenderId = ( elfCreation ? 0x025D : 0x0190 ); break; // elf male (human if server doesn't allow elf but client tried anyway)
			case 3:		pGenderId = ( elfCreation ? 0x025E : 0x0191 ); break; // elf female (human if server doesn't allow elf but client tried anyway)
			default:						break;
		}
	}

	mChar->SetId( pGenderId );
	mChar->SetOrgId( pGenderId );

	mChar->SetSkin( ValidSkinColour( skinColour, pGenderId ) | 0x8000 );
	mChar->SetOrgSkin( mChar->GetSkin() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ShowMessageOfTheDay()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens the Message of the Day window
//o------------------------------------------------------------------------------------------------o
auto ShowMessageOfTheDay( CSocket *s ) -> void
{
	if( s )
	{
		auto Updates = FileLookup->FindEntry( "MOTD", misc_def );
		if( Updates )
		{
			std::string updateData = "";
			for( const auto &sec : Updates->collection() )
			{
				updateData += sec->data + " "s;
			}
			CPUpdScroll toSend( 2 );
			toSend.AddString( updateData.c_str() );
			toSend.Finalize();
			s->Send( &toSend );
		}
	}
}

void SysBroadcast( const std::string& txt );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	StartChar()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends character startup stuff to player
//o------------------------------------------------------------------------------------------------o
void StartChar( CSocket *mSock, bool onCreate )
{
	if( mSock != nullptr )
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
		if( ValidateObject( mChar ))
		{
			CPClientVersion verCheck;
			mSock->Send( &verCheck );

			CAccountBlock_st& actbTemp = mSock->GetAccount();
			if( actbTemp.wAccountIndex != AB_INVALID_ID )
			{
				actbTemp.wFlags.set( AB_FLAGS_ONLINE, true );
			}

			mSock->TargetOK( false );

			CPCharLocBody startup = (*mChar);
			if( mChar->GetPoisoned() )
			{
				startup.Flag( 0x04 );
			}
			else
			{
				startup.Flag( 0x00 );
			}
			mSock->Send( &startup );

			mSock->SetTimer( tPC_SPIRITSPEAK, 0 );
			mChar->SetStealth( -1 );
			if( mChar->GetVisible() != VT_PERMHIDDEN )
			{
				mChar->SetVisible( VT_VISIBLE );
			}
			mChar->SetWar( false );

			SendMapChange( mChar->WorldNumber(), mSock, true );

			if( cwmWorldState->ServerData()->MapDiffsEnabled() )
			{
				CPEnableMapDiffs mDiff;
				mSock->Send( &mDiff );
			}

			CPWorldChange wrldChange( mChar->GetRegion()->GetAppearance(), 1 );
			mSock->Send( &wrldChange );	// need to add this?

			CPPersonalLightLevel pllToSend = ( *mChar );		// need to add this?
			pllToSend.Level( 0 );
			mSock->Send( &pllToSend );

			CPWarMode wMode( 0 );
			mSock->Send( &wMode );

			CItem *nItem = mChar->GetItemAtLayer( IL_PACKITEM );
			mChar->SetPackItem( nItem );

			if( mChar->IsDead() )
			{
				CPResurrectMenu toSend( 0 );
				mSock->Send( &toSend );
			}

			mChar->Dirty( UT_LOCATION );
			mChar->SetStep( 1 );

			CPLoginComplete lc;	
			mSock->Send( &lc );
			Network->SetLastOn( mSock );

			UI08 currentHour = cwmWorldState->ServerData()->ServerTimeHours();
			UI08 currentMins = cwmWorldState->ServerData()->ServerTimeMinutes();
			UI08 currentSecs = cwmWorldState->ServerData()->ServerTimeSeconds();

			CPTime tmPckt( currentHour, currentMins, currentSecs );	mSock->Send( &tmPckt );

			mSock->SysMessage( "%s v%s.%s [%s] ", CVersionClass::GetProductName().c_str(), CVersionClass::GetVersion().c_str(), CVersionClass::GetBuild().c_str(), OS_STR );

			if( cwmWorldState->ServerData()->ServerJoinPartAnnouncementsStatus() )
			{
				//message upon entering a server
				SysBroadcast( oldstrutil::format(1024, Dictionary->GetEntry( 1208 ), mChar->GetName().c_str() )); // message upon entering a server
			}
			ShowMessageOfTheDay( mSock );
#if defined( _MSC_VER )
#pragma note( "HTML Status for Players" )
#endif
			if( onCreate )
			{
				cScript *onCreateScp = JSMapping->GetScript( static_cast<UI16>( 0 ));	// 0 == global script
				if( onCreateScp != nullptr )
				{
					onCreateScp->OnCreate( mChar, true );
				}
			}

			bool loginEventHandled = false;
			std::vector<UI16> scriptTriggers = mChar->GetScriptTriggers();
			for( auto scriptTrig : scriptTriggers )
			{
				cScript *toExecute = JSMapping->GetScript( scriptTrig );
				if( toExecute != nullptr )
				{
					// 0 == no such event was found
					// 1 == event was found, and executed
					if( toExecute->OnLogin( mSock, mChar ) == 1 )
					{
						loginEventHandled = true;
					}
				}
			}

			if( !loginEventHandled )
			{
				// No script attached to character handled onLoginevent. Let's check global script!
				cScript *toExecute = JSMapping->GetScript( static_cast<UI16>( 0 ));
				if( toExecute != nullptr )
				{
					toExecute->OnLogin( mSock, mChar );
				}
			}

			// Store hair and beard (if they have any) properly for characters created pre-0.99.2j
			CItem *hairObject = mChar->GetItemAtLayer( IL_HAIR );
			CItem *beardObject = mChar->GetItemAtLayer( IL_FACIALHAIR );
			if( ValidateObject( hairObject ))
			{
				if( mChar->GetHairStyle() == 0xFFFF )
				{
					mChar->SetHairStyle( hairObject->GetId() );
					mChar->SetHairColour( hairObject->GetColour() );
				}
			}

			if( ValidateObject( beardObject ))
			{
				if( mChar->GetBeardStyle() == 0xFFFF )
				{
					mChar->SetBeardStyle( beardObject->GetId() );
					mChar->SetBeardColour( beardObject->GetColour() );
				}
			}

			mSock->LoginComplete( true );

			// Negotiate features with assistant tools like Razor and AssistUO if ASSISTANTNEGOTIATION is enabled in uox.ini
			if( cwmWorldState->ServerData()->GetAssistantNegotiation() )
			{
				CPNegotiateAssistantFeatures ii( mSock );
				mSock->Send( &ii );
				mSock->SysMessage( 9012 ); // Attempting to negotiate features with assistant tool...

				// Set 30s negotiation timer if KICKONASSISTANTSILENCE setting is enabled in uox.ini
				if( cwmWorldState->ServerData()->KickOnAssistantSilence() )
				{
					// Start timer to kick player if assistant tool hasn't responded in 30 seconds
					mSock->SysMessage( 9013 ); // This server requires use of an assistant tool that supports feature negotiation. Enable the tool's option for negotiating features with server, or get kicked in 30 seconds.
					mSock->NegotiateTimeout( cwmWorldState->GetUICurrentTime() + ( 30 * 1000 ));
				}
			}

			mChar->Dirty( UT_LOCATION );

			if( mChar->WorldNumber() > 0 )
			{
				// Without this, world will not be properly updated in regular UO clients until they take the first step
				mChar->Update();
			}

			// Re-add player to party, if they are in one!
			Party *mCharParty = PartyFactory::GetSingleton().Get( mChar );
			if( mCharParty != nullptr )
			{
				// Player's in a party! Send the details
				mCharParty->SendList( nullptr );
				mSock->SysMessage( 9073 ); // You have rejoined the party.
			}

			// Play music associated with region player is logging into!
			Effects->DoSocketMusic( mSock );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CreateCorpseItem()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Generates a corpse or backpack based on the character killed.
//o------------------------------------------------------------------------------------------------o
CItem *CreateCorpseItem( CChar& mChar, CChar *killer, UI08 fallDirection )
{
	std::string corpseName = GetNpcDictName( &mChar );

	CItem *iCorpse = nullptr;
	bool shouldSave = mChar.ShouldSave();

	iCorpse = Items->CreateItem( nullptr, &mChar, 0x2006, 1, mChar.GetSkin(), OT_ITEM, false, shouldSave );
	if( !ValidateObject( iCorpse ))
		return nullptr;

	iCorpse->SetName( oldstrutil::format( 512, Dictionary->GetEntry( 1612 ), corpseName.c_str() )); // corpse of %s
	iCorpse->SetCarve( mChar.GetCarve() );
	iCorpse->SetMovable( 2 );//non-movable
	if( fallDirection )
	{
		iCorpse->SetDir( mChar.GetDir() | 0x80 );
	}
	else
	{
		iCorpse->SetDir( mChar.GetDir() );
	}

	iCorpse->SetAmount( mChar.GetId() );
	iCorpse->SetWeightMax( 50000 ); // 500 stones
	iCorpse->SetMaxItems( cwmWorldState->ServerData()->MaxPlayerPackItems() + 25 );
	iCorpse->SetCorpse( true );

	UI08 canCarve = 0;
	if( mChar.GetId( 1 ) == 0x00 && ( mChar.GetId( 2 ) == 0x0C || ( mChar.GetId( 2 ) >= 0x3B && mChar.GetId( 2 ) <= 0x3D ))) // If it's a dragon, 50/50 chance you can carve it
	{
		canCarve = static_cast<UI08>( RandomNum( 0, 1 ));
	}

	iCorpse->SetDecayable( true );
	iCorpse->SetName2( corpseName );
	iCorpse->SetType( IT_CONTAINER );
	iCorpse->SetTempVar( CITV_MOREY, 1, canCarve );
	iCorpse->SetTempVar( CITV_MOREY, 2, cwmWorldState->creatures[mChar.GetId()].IsHuman() );
	iCorpse->SetTempVar( CITV_MOREZ, mChar.GetFlag() );
	iCorpse->SetTempTimer( cwmWorldState->GetUICurrentTime() );
	if( !mChar.IsNpc() )
	{
		iCorpse->SetOwner( &mChar );
		iCorpse->SetDecayTime( BuildTimeValue( cwmWorldState->ServerData()->SystemTimer( tSERVER_CORPSEDECAY )));
	}
	else
	{
		iCorpse->SetSectionId( mChar.GetSectionId() ); // Store NPC section ID on corpse
		iCorpse->SetDecayTime( BuildTimeValue( cwmWorldState->ServerData()->SystemTimer( tSERVER_NPCCORPSEDECAY )));
	}

	if( !ValidateObject( killer ))
	{
		iCorpse->SetTempVar( CITV_MOREX, INVALIDSERIAL );
	}
	else
	{
		iCorpse->SetTempVar( CITV_MOREX, killer->GetSerial() );
	}

	return iCorpse;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	MoveItemsToCorpse()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Moves Items from Character to Corpse
//o------------------------------------------------------------------------------------------------o
auto MoveItemsToCorpse( CChar &mChar, CItem *iCorpse ) -> void
{
	CItem *packItem		= mChar.GetPackItem();
	CItem *dupeItem		= nullptr;
	bool packIsValid	= ValidateObject( packItem );
	for( CItem *j = mChar.FirstItem(); !mChar.FinishedItems(); j = mChar.NextItem() )
	{
		if( !ValidateObject( j ))
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
				if( mChar.GetBodyType() != BT_GARGOYLE ) // Ignore if gargoyle - doesn't seem to display properly on corpses
				{
					dupeItem = j->Dupe();
					dupeItem->SetCont( iCorpse );
					dupeItem->SetName( "Hair/Beard" );
					dupeItem->SetX( 0x47 );
					dupeItem->SetY( 0x93 );
					dupeItem->SetZ( 0 );
					dupeItem->SetMovable( 2 );
				}
				break;
			case IL_PACKITEM:
			{
				std::vector<CItem *> moveItems;
				auto jCont = j->GetContainsList();
				for( const auto &k : jCont->collection() )
				{
					if( ValidateObject( k ))
					{
						// If the character dying is a pack animal, drop everything they're carrying - including newbie items and spellbooks
						if(( mChar.GetId() == 0x0123 || mChar.GetId() == 0x0124 || mChar.GetId() == 0x0317 ) || ( !k->IsNewbie() && k->GetType() != IT_SPELLBOOK ))
						{
							// Store a reference to the item we want to move...
							moveItems.push_back( k );
						}
					}
				}

				// Loop through the items we want to move - and move them!
				std::for_each( moveItems.begin(), moveItems.end(), [iCorpse]( CItem *item )
				{
					item->SetCont( iCorpse );
					item->SetX( static_cast<SI16>( 20 + ( RandomNum( 0, 49 ))));
					item->SetY( static_cast<SI16>( 85 + ( RandomNum( 0, 75 ))));
					item->SetZ( 9 );
				});

				if( !mChar.IsShop() )
				{
					j->SetLayer( IL_BUYCONTAINER );
				}
				break;
			}
			default:
				if( packIsValid && j->IsNewbie() )
				{
					j->SetCont( packItem );
				}
				else
				{
					j->SetCont( iCorpse );
					j->SetX( static_cast<SI16>( 20 + ( RandomNum( 0, 49 ))));
					j->SetY( static_cast<SI16>( 85 + ( RandomNum( 0, 74 ))));
					j->SetZ( 9 );
				}
				break;
		}
	}
}

void KillTrades( CChar *i );
//o------------------------------------------------------------------------------------------------o
//|	Function    -	HandleDeath()
//o------------------------------------------------------------------------------------------------o
//|	Purpose     -	Performs death stuff. I.E.- creates a corpse, moves items
//|                  to it, take out of war mode, does animation and sound, etc.
//o------------------------------------------------------------------------------------------------o
void HandleDeath( CChar *mChar, CChar *attacker )
{
	if( !ValidateObject( mChar ) || mChar->IsDead() || mChar->IsInvulnerable() )	// don't kill them if they are dead or invulnerable!
		return;

	CSocket *pSock = nullptr;
	if( !mChar->IsNpc() )
	{
		pSock = mChar->GetSocket();
	}

	DismountCreature( mChar );

	if( pSock != nullptr )
	{
		KillTrades( mChar );
	}

	if( mChar->GetId() != mChar->GetOrgId() )
	{
		mChar->SetId( mChar->GetOrgId() );
	}

	UI08 fallDirection = static_cast<UI08>( RandomNum( 1, 100 ) % 2 );
	mChar->SetDead( true );

	Effects->PlayDeathSound( mChar );
	CItem *iCorpse = CreateCorpseItem(( *mChar ), attacker, fallDirection );
	if( iCorpse != nullptr )
	{
		MoveItemsToCorpse(( *mChar ), iCorpse );
		if( cwmWorldState->ServerData()->DeathAnimationStatus() )
		{
			Effects->DeathAction( mChar, iCorpse, fallDirection );
		}

		// Prevent pets from following ghost of dead player
		auto mPetList = mChar->GetPetList();
		for( const auto &tempChar : mPetList->collection() )
		{
			if( ValidateObject( tempChar ))
			{
				if( !tempChar->GetStabled() && tempChar->GetNpcWander() == WT_FOLLOW && tempChar->GetFTarg() == mChar )
				{
					tempChar->SetFTarg( nullptr );
					tempChar->SetOldNpcWander( WT_NONE );
					tempChar->SetNpcWander( WT_NONE );
					tempChar->SetGuarding( iCorpse );
					iCorpse->SetGuarded( true );
				}
			}
		}

		// Spawn blood effect below corpse
		UI16 bloodColour = Races->BloodColour( mChar->GetRace() ); // Fetch blood color from race property
		if( bloodColour == 0xffff )
		{
			// If blood colour is 0xffff in the race setup, inherit color of NPC instead!
			bloodColour = mChar->GetSkin();
		}
		CItem * bloodEffect = Effects->SpawnBloodEffect( iCorpse->WorldNumber(), iCorpse->GetInstanceId(), bloodColour, BLOOD_DEATH );
		if( ValidateObject( bloodEffect ))
		{
			// Set a timestamp for when the bloodeffect was created, and match it to the corpse!
			bloodEffect->SetTempTimer( iCorpse->GetTempTimer() );

			if( !mChar->IsNpc() )
			{
				// Set character as owner of blood-effects, if not an NPC
				bloodEffect->SetOwner( mChar );
			}

			// If there's a valid attacker still, store the serial on the blood item (just like the corpse)
			// so it could be accessed by something like forensics skill
			if( ValidateObject( attacker ))
			{
				bloodEffect->SetTempVar( CITV_MOREX, attacker->GetSerial() );
			}
			else
			{
				bloodEffect->SetTempVar( CITV_MOREX, INVALIDSERIAL );
			}

			// Finally, set blood's location to match that of the corpse
			bloodEffect->SetLocation( iCorpse );
		}
	}

	mChar->SetWar( false );
	mChar->StopSpell();
	mChar->SetFrozen( false );
	mChar->SetHP( 0 );
	mChar->SetPoisoned( 0 );
	mChar->SetPoisonStrength( 0 );

	if( !mChar->IsNpc() )
	{
		switch( mChar->GetOrgId() )
		{
			case 0x0190:	mChar->SetId( 0x0192 );	break;	// human male
			default:
			case 0x0191:	mChar->SetId( 0x0193 );	break;	// human female, or others
			case 0x025D:	mChar->SetId( 0x025F );	break;	// elf male
			case 0x025E:	mChar->SetId( 0x0260 );	break;	// elf female
			case 0x029A:	mChar->SetId( 0x02B6 ); break;	// gargoyle male
			case 0x029B:	mChar->SetId( 0x02B7 ); break;	// gargoyle female
			case 0x00B7:
			case 0x00B9:
			case 0x02EE:	mChar->SetId( 0x0192 );	break;	// savage male
			case 0x00B8:
			case 0x00BA:
			case 0x02EF:	mChar->SetId( 0x0192 );	break;	// savage female
		}

		CItem *c = Items->CreateItem( nullptr, mChar, 0x204E, 1, 0, OT_ITEM );
		if( c == nullptr )
			return;

		c->SetName( Dictionary->GetEntry( 1610 )); // A Death Shroud
		mChar->SetRobe( c->GetSerial() );
		c->SetLayer( IL_ROBE );
		if( c->SetCont( mChar ))
		{
			c->SetResist( 1, PHYSICAL );
		}

		if( mChar->GetAccount().wAccountIndex != AB_INVALID_ID )
		{
			if( pSock != nullptr )
			{
				CPResurrectMenu toSend( 0 );
				pSock->Send( &toSend );
			}
			// The fade to gray is done by the CPResurrectMenu packet
		}
	}

	std::vector<UI16> scriptTriggers = mChar->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			// If script returns true/1, prevent other scripts with event from running
			if( toExecute->OnDeath( mChar, iCorpse ) == 1 )
			{
				break;
			}
		}
	}

	if( mChar->IsNpc() )
	{
		mChar->Delete();
	}
	else
	{
		mChar->Dirty( UT_LOCATION );
	}

	// Play death music
	if( pSock != nullptr )
	{
		Effects->DoSocketMusic( pSock );
	}
}
