//o--------------------------------------------------------------------------o
//|	File			-	CResponse.cpp
//|	Date			-	11/15/2003
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Version History
//|									
//|							1.0		giwo		15th November, 2003
//|							Initial implementation.  Ripped out 99% of cSpeech::response
//|							and relocated contents here in individual handler classes
//|
//|							1.1		giwo		16th November, 2003
//|							Changed CBaseResponse::Handle() overload to WhichResponse(), and
//|								locally declared it wherever it was used, also handles different
//|								languages in WhichResponse() now.
//|							Added an inline findString() helper function to clean up WhichResponse().
//|							Now houses our vendor responses (enabling us to completely remove
//|								checkForVendorResponse() and responsevendor().
//|							Also handles all currently supported house speech using a single
//|								class, enabling the removal of HouseSpeech().
//|
//|							1.2		giwo		17th November, 2003
//|							We now make use of OSI's trigger words (the few that we actually handle,
//|								currently), to remove the need to search through the text string
//o--------------------------------------------------------------------------o
#include "uox3.h"
#include "CResponse.h"
#include "regions.h"
#include "msgboard.h"
#include "townregion.h"
#include "classes.h"
#include "cEffects.h"
#include "Dictionary.h"
#include "CPacketSend.h"

namespace UOX
{

bool BuyShop( CSocket *s, CChar *c );
void callGuards( CChar *mChar );

inline bool findString( std::string toCheck, std::string toFind )
{
	return ( toCheck.find( toFind ) != std::string::npos );
}

void WhichResponse( CSocket *mSock, CChar *mChar, std::string text )
{
	CBaseResponse *tResp	= NULL;

	switch( mSock->TriggerWord() )
	{
	case TW_COUNT:																						break;
	case TW_GUARDS:				callGuards( mChar );													break;
	case TW_KILLS:				tResp = new CKillsResponse();											break;
	case TW_BANK:				tResp = new CBankResponse();											break;
	case TW_BALANCE:			tResp = new CBankResponse( true );										break;
	case TW_QUESTTAKE:			tResp = new CEscortResponse();											break;
	case TW_QUESTDEST:			tResp = new CEscortResponse( true );									break;
	case TW_TRAIN:				tResp = new CTrainingResponse( text );									break;
	case TW_FOLLOW:
	case TW_FOLLOW2:			tResp = new CPetMultiResponse( text, false, TARGET_FOLLOW, 1310 );		break;
	case TW_COME:
	case TW_FOLLOWME:			tResp = new CPetComeResponse( false, text );							break;
	case TW_ALLCOME:
	case TW_ALLFOLLOW:
	case TW_ALLFOLLOWME:		tResp = new CPetComeResponse( true, text );								break;
	case TW_KILL:
	case TW_ATTACK:				tResp = new CPetAttackResponse( false, text );							break;
	case TW_ALLKILL:
	case TW_ALLATTACK:			tResp = new CPetAttackResponse( true, text );							break;
	case TW_FETCH:
	case TW_GET:
	case TW_BRING:				tResp = new CPetMultiResponse( text, false, TARGET_GUARD, 1316 );		break;
	case TW_FRIEND:				tResp = new CPetMultiResponse( text, true, TARGET_FRIEND, 1620 );		break;
	case TW_GUARD:				tResp = new CPetMultiResponse( text, true, TARGET_GUARD, 1104 );		break;
	case TW_ALLGUARD:
	case TW_ALLGUARDME:			tResp = new CPetGuardResponse( true, text );							break;
	case TW_STOP:
	case TW_STAY:				tResp = new CPetStayResponse( false, text );							break;
	case TW_ALLSTOP:
	case TW_ALLSTAY:			tResp = new CPetStayResponse( true, text );								break;
	case TW_TRANSFER:			tResp = new CPetMultiResponse( text, true, TARGET_TRANSFER, 1323 );		break;
	case TW_RELEASE:			tResp = new CPetReleaseResponse( text );								break;
	case TW_VENDORBUY:			tResp = new CVendorBuyResponse( true, text );							break;
	case TW_BUY:				tResp = new CVendorBuyResponse( false, text );							break;
	case TW_VENDORSELL:			tResp = new CVendorSellResponse( true, text );							break;
	case TW_SELL:				tResp = new CVendorSellResponse( false, text );							break;
	case TW_VENDORGOLD:			tResp = new CVendorGoldResponse( true, text );							break;
	case TW_HOUSEBAN:			tResp = new CHouseMultiResponse( TARGET_HOUSEBAN, 585 );				break;
	case TW_HOUSEEJECT:			tResp = new CHouseMultiResponse( TARGET_HOUSEEJECT, 587 );				break;
	case TW_HOUSELOCKDOWN:		tResp = new CHouseMultiResponse( TARGET_HOUSELOCKDOWN, 589 );			break;
	case TW_HOUSERELEASE:		tResp = new CHouseMultiResponse( TARGET_HOUSERELEASE, 591 );			break;
	case TW_BOATFORWARD:
	case TW_BOATUNFURL:			tResp = new CBoatMultiResponse( 1 );									break;
	case TW_BOATBACKWARD:		tResp = new CBoatMultiResponse( 2 );									break;
	case TW_BOATSTOP:
	case TW_STOP2:
	case TW_BOATFURL:			tResp = new CBoatMultiResponse( 0 );									break;
	case TW_BOATTURNRIGHT:
	case TW_BOATSTARBOARD:
	case TW_BOATTURNLEFT:
	case TW_BOATPORT:
	case TW_BOATTURNAROUND:
	case TW_BOATLEFT:
	case TW_BOATRIGHT:
	case TW_SETNAME:			tResp = new CBoatResponse( text );										break;
	default:
#if defined( UOX_DEBUG_MODE )
		Console.Print( "Unhandled TriggerWord sent by the client 0x%X\n",mSock->TriggerWord() );
#endif
		break;
	}

	if( tResp != NULL )
	{
		tResp->Handle( mSock, mChar );
		delete tResp;
	}
}

CHARLIST findNearbyNPCs( CChar *mChar, distLocs distance )
{
	CHARLIST ourNpcs;
	REGIONLIST nearbyRegions = MapRegion->PopulateList( mChar );
	for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
	{
		SubRegion *CellResponse = (*rIter);
		if( CellResponse == NULL )
			continue;

		CDataList< CChar * > *regChars = CellResponse->GetCharList();
		regChars->Push();
		for( CChar *Npc = regChars->First(); !regChars->Finished(); Npc = regChars->Next() )
		{
			if( !ValidateObject( Npc ) || Npc == mChar || !Npc->IsNpc() )
				continue;
			if( objInRange( mChar, Npc, distance ) )
				ourNpcs.push_back( Npc );
		}
		regChars->Pop();
	}
	return ourNpcs;
}

CEscortResponse::CEscortResponse( bool newVal )
{
	findDest = newVal;
}
void CEscortResponse::Handle( CSocket *mSock, CChar *mChar )
{
	// If the PC is dead then break out, The dead cannot accept quests
	if( mChar->IsDead() ) 
		return;
	CHARLIST npcList = findNearbyNPCs( mChar, DIST_INRANGE );
	for( CHARLIST_CITERATOR npcCtr = npcList.begin(); npcCtr != npcList.end(); ++npcCtr )
	{
		CChar *Npc = (*npcCtr);
		if( Npc->GetQuestType() == ESCORTQUEST )
		{
			// I WILL TAKE THEE
			// If this is a request for hire and the PC is within range of the NPC and the NPC is waiting for an ESCORT
			if( !findDest )
			{
				if( !ValidateObject( Npc->GetFTarg() ) )
				{
					Npc->SetFTarg( mChar );			// Set the NPC to follow the PC
					Npc->SetNpcWander( 1 );			// Set the NPC to wander freely
					Npc->SetTimer( tNPC_SUMMONTIME, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_ESCORTACTIVE ) );			// Set the expire time if nobody excepts the quest
		
					// Send out the rant about accepting the escort
					Npc->talkAll( 1294, false, regions[Npc->GetQuestDestRegion()]->GetName().c_str() );

					// Remove post from message board (Mark for deletion only - will be cleaned during cleanup)
					MsgBoardQuestEscortRemovePost( Npc );
					// Return 1 so that we indicate that we handled the message
					return;
				}
				else
					findDest = true;	// If the current NPC already has an ftarg then respond to query for quest
			}

			// DESTINATION
			// If this is a request to find out where a NPC wants to go and the PC is within range of the NPC and the NPC is waiting for an ESCORT
			if( findDest )
			{
				if( Npc->GetFTarg() == mChar )
					Npc->talkAll( 1295, false, regions[Npc->GetQuestDestRegion()]->GetName().c_str() );	// Send out the rant about accepting the escort
				else if( !ValidateObject( Npc->GetFTarg() ) )  // If nobody has been accepted for the quest yet
					Npc->talkAll( 1296, false, regions[Npc->GetQuestDestRegion()]->GetName().c_str() );	// Send out the rant about accepting the escort
				else // The must be enroute
					Npc->talkAll( 1297, false, regions[Npc->GetQuestDestRegion()]->GetName().c_str(), Npc->GetFTarg()->GetName().c_str() );	// Send out a message saying we are already being escorted
				// Return success ( we handled the message )
				return;
			}		
		}
	}
}

CBankResponse::CBankResponse( bool newVal )
{
	checkBalance = newVal;
}
void CBankResponse::Handle( CSocket *mSock, CChar *mChar )
{
	if( !mChar->IsDead() )
	{
		CHARLIST npcList = findNearbyNPCs( mChar, DIST_INRANGE );
		for( CHARLIST_CITERATOR npcCtr = npcList.begin(); npcCtr != npcList.end(); ++npcCtr )
		{
			CChar *Npc = (*npcCtr);
			if( Npc->GetNPCAiType() == aiBANKER )// if he's a banker and we're close!
			{
				if( !checkBalance )
					mSock->openBank( mChar );
				else
				{
					UI32 goldCount = GetBankCount( mChar, 0x0EED );
					Npc->talk( mSock, 1298, true, mChar->GetName().c_str(), goldCount );
				}
				break;
			}
		}
	}
}

CKillsResponse::CKillsResponse( void )
{
}
void CKillsResponse::Handle( CSocket *mSock, CChar *mChar )
{
	if( !mChar->IsDead() && mSock != NULL )
	{
		SI16 i = mChar->GetKills();
		if( i == 0 )
			mSock->sysmessage( 1288 );
		else if( i > cwmWorldState->ServerData()->RepMaxKills() )
			mSock->sysmessage( 1289, i );
		else
			mSock->sysmessage( 1290, i );
	}
}

CTrainingResponse::CTrainingResponse( std::string text )
{
	ourText = text;
}
void CTrainingResponse::Handle( CSocket *mSock, CChar *mChar )
{
	if( cwmWorldState->ServerData()->NPCTrainingStatus() ) //if the player wants to train
	{
		char temp[512];
		char temp2[512];
		CHARLIST npcList = findNearbyNPCs( mChar, DIST_INRANGE );
		for( CHARLIST_CITERATOR npcCtr = npcList.begin(); npcCtr != npcList.end(); ++npcCtr )
		{
			CChar *Npc = (*npcCtr);
			if( Npc->isHuman() )
			{
				// Stop the NPC from moving for a minute while talking with the player
				Npc->SetTimer( tNPC_MOVETIME, BuildTimeValue( 60 ) );
				mSock->TempObj( NULL ); //this is to prevent errors when a player says "train <skill>" then doesn't pay the npc
				SI16 skill = -1;
				for( UI08 i = 0; i < ALLSKILLS; ++i )
				{
					if( findString( ourText, skillname[i] ) )
					{
						skill = i;  //Leviathan fix
						break;
					}
				}
				if( skill == -1 ) // Didn't ask to be trained in a specific skill - Leviathan fix
				{
					if( !Npc->CanTrain() )
					{
						Npc->talk( mSock, 1302, false );
						continue;
					}
					Npc->SetTrainingPlayerIn( 255 ); // Like above, this is to prevent  errors when a player says "train <skill>" then doesn't pay the npc
					strcpy( temp, Dictionary->GetEntry( 1303 ).c_str() );
					UI08 skillsToTrainIn = 0;
					for( UI08 j = 0; j < ALLSKILLS; ++j )
					{
						if( Npc->GetBaseSkill( j ) > 10 )
						{
							sprintf( temp2, "%s, ", UString( skillname[j] ).lower().c_str() );
							if( !skillsToTrainIn ) 
								temp2[0] = toupper( temp2[0] ); // If it's the first skill,  capitalize it.
							strcat( temp, temp2 );
							++skillsToTrainIn;
						}
					}
					if( skillsToTrainIn )
					{
						temp[strlen( temp ) - 2] = '.'; // Make last character a . not a ,  just to look nicer
						Npc->talk( mSock, temp, false );
					}
					else
						Npc->talk( mSock, 1302, false );
				}
				else // They do want to learn a specific skill
				{
					if( !Npc->CanTrain() )
					{
						Npc->talk( mSock, 1302, false );
						continue;
					}
					if( Npc->GetBaseSkill( (UI08)skill ) > 10 )
					{
						sprintf( temp, Dictionary->GetEntry( 1304 ).c_str(), UString( skillname[skill] ).lower().c_str() );
						if( mChar->GetBaseSkill( (UI08)skill ) >= 250 )
							strcat( temp, Dictionary->GetEntry( 1305 ).c_str() );
						else
						{
							if( Npc->GetBaseSkill( (UI08)skill ) <= 250)
								sprintf( temp2, Dictionary->GetEntry( 1306 ).c_str(),(int)( Npc->GetBaseSkill( (UI08)skill ) / 2 / 10 ),(int)( Npc->GetBaseSkill( (UI08)skill ) / 2 ) - mChar->GetBaseSkill( (UI08)skill ) );
							else
								sprintf( temp2, Dictionary->GetEntry( 1306 ).c_str(), 25, 250 - mChar->GetBaseSkill( (UI08)skill ) );
							strcat( temp, temp2 );
							mSock->TempObj( Npc );
							Npc->SetTrainingPlayerIn( (UI08)skill );
						}
						Npc->talk( mSock, temp, false );
					}
					else
						Npc->talk( mSock, 1307, false ); 
				}
				break;
			}
		}
	}
}

CBasePetResponse::CBasePetResponse( std::string text )
{
	ourText = text;
}
void CBasePetResponse::Handle( CSocket *mSock, CChar *mChar )
{
	CHARLIST npcList = findNearbyNPCs( mChar, DIST_INRANGE );
	for( CHARLIST_CITERATOR npcCtr = npcList.begin(); npcCtr != npcList.end(); ++npcCtr )
	{
		CChar *Npc = (*npcCtr);
		if( !Handle( mSock, mChar, Npc ) )
			break;
	}
}
bool CBasePetResponse::canControlPet( CChar *mChar, CChar *Npc, bool isRestricted )
{
	bool retVal = false;
	if( ValidateObject( Npc->GetOwnerObj() ) && Npc->GetNPCAiType() != aiPLAYERVENDOR )
	{
		if( Npc->GetOwnerObj() == mChar )
			retVal = true;
		else if( !isRestricted && Npcs->checkPetFriend( mChar, Npc ) )
			retVal = true;
	}
	return retVal;
}

CPetMultiResponse::CPetMultiResponse( std::string text, bool restrictVal, TargetIDs targVal, SI32 dictVal ) : CBasePetResponse( text )
{
	isRestricted	= restrictVal;
	targID			= targVal;
	dictEntry		= dictVal;
}
bool CPetMultiResponse::Handle( CSocket *mSock, CChar *mChar, CChar *Npc )
{
	if( canControlPet( mChar, Npc, isRestricted ) )
	{
		if( findString( ourText, UString(Npc->GetName()).upper() ) )
		{
			mSock->TempObj( Npc );
			mSock->target( 0, targID, dictEntry );
			return false;
		}
	}
	return true;
}

CPetReleaseResponse::CPetReleaseResponse( std::string text ) : CBasePetResponse( text )
{
}
bool CPetReleaseResponse::Handle( CSocket *mSock, CChar *mChar, CChar *Npc )
{
	if( canControlPet( mChar, Npc, true ) )
	{
		if( findString( ourText, UString(Npc->GetName()).upper() ) )
		{
			Npcs->stopPetGuarding( Npc );
			Npc->SetFTarg( NULL );
			Npc->SetNpcWander( 2 );
			Npc->SetOwner( NULL );
			Npc->talkAll( 1325, false, Npc->GetName().c_str() );
			if( Npc->GetTimer( tNPC_SUMMONTIME ) )
			{
				Effects->PlaySound( Npc, 0x01FE );
				Npc->Delete();
			}
			return false;
		}
	}
	return true;
}

CPetAllResponse::CPetAllResponse( bool allVal, std::string text ) : CBasePetResponse( text )
{
	saidAll = allVal;
}

CPetGuardResponse::CPetGuardResponse( bool allVal, std::string text ) : CPetAllResponse( allVal, text )
{
}
bool CPetGuardResponse::Handle( CSocket *mSock, CChar *mChar, CChar *Npc )
{
	if( canControlPet( mChar, Npc, true ) )
	{
		if( saidAll || findString( ourText, UString(Npc->GetName()).upper() ) )
		{
			Npcs->stopPetGuarding( Npc );
			mSock->sysmessage( 1321 );
			Npc->SetNPCAiType( aiPET_GUARD );
			Npc->SetGuarding( mChar );
			mChar->SetGuarded( true );
			Npc->SetFTarg( mChar );
			Npc->SetNpcWander( 1 );
			if( !saidAll )
				return false;
		}
	}
	return true;
}

CPetAttackResponse::CPetAttackResponse( bool allVal, std::string text ) : CPetAllResponse( allVal, text )
{
}
bool CPetAttackResponse::Handle( CSocket *mSock, CChar *mChar, CChar *Npc )
{
	if( canControlPet( mChar, Npc, true ) )
	{
		if( saidAll || findString( ourText, UString(Npc->GetName()).upper() ) )
		{
			Npcs->stopPetGuarding( Npc );
			mSock->TempObj( Npc );
			mSock->target( 0, TARGET_ATTACK, 1313 );
			if( !saidAll )
				return false;
		}
	}
	return true;
}

CPetComeResponse::CPetComeResponse( bool allVal, std::string text ) : CPetAllResponse( allVal, text )
{
}
bool CPetComeResponse::Handle( CSocket *mSock, CChar *mChar, CChar *Npc )
{
	if( canControlPet( mChar, Npc ) )
	{
		if( saidAll || findString( ourText, UString(Npc->GetName()).upper() ) )
		{
			Npcs->stopPetGuarding( Npc );
			Npc->SetFTarg( mChar );
			Npc->SetNpcWander( 1 );
			mSock->sysmessage( 1318 );
			if( !saidAll )
				return false;
		}
	}
	return true;
}

CPetStayResponse::CPetStayResponse( bool allVal, std::string text ) : CPetAllResponse( allVal, text )
{
}
bool CPetStayResponse::Handle( CSocket *mSock, CChar *mChar, CChar *Npc )
{
	if( canControlPet( mChar, Npc ) )
	{
		if( saidAll || findString( ourText, UString(Npc->GetName()).upper() ) )
		{
			Npcs->stopPetGuarding( Npc );
			Npc->SetFTarg( NULL );
			Npc->SetTarg( NULL );
			if( Npc->IsAtWar() ) 
				Npc->ToggleCombat();
			Npc->SetNpcWander( 0 );
			if( !saidAll )
				return false;
		}
	}
	return true;
}

CBaseVendorResponse::CBaseVendorResponse( bool vendVal, std::string text )
{
	saidVendor	= vendVal;
	ourText		= text;
}
void CBaseVendorResponse::Handle( CSocket *mSock, CChar *mChar )
{
	CHARLIST npcList = findNearbyNPCs( mChar, DIST_INRANGE );
	for( CHARLIST_CITERATOR npcCtr = npcList.begin(); npcCtr != npcList.end(); ++npcCtr )
	{
		CChar *Npc = (*npcCtr);
		if( Npc->IsShop() || Npc->GetNPCAiType() == aiPLAYERVENDOR )
		{
			if( saidVendor || findString( ourText, UString(Npc->GetName()).upper() ) )
			{
				if( !Handle( mSock, mChar, Npc ) )
					break;
			}
		}
	}
}

CVendorBuyResponse::CVendorBuyResponse( bool vendVal, std::string text ) : CBaseVendorResponse( vendVal, text )
{
}
bool CVendorBuyResponse::Handle( CSocket *mSock, CChar *mChar, CChar *Npc )
{
	Npc->SetTimer( tNPC_MOVETIME, BuildTimeValue( 60 ) );
	if( Npc->GetNPCAiType() == aiPLAYERVENDOR )
	{
		mSock->TempObj( Npc );
		Npc->talk( mSock, 1333, false );
		mSock->target( 0, TARGET_PLVBUY, " " );
		return false;
	} 
	else if( BuyShop( mSock, Npc ) ) 
		return false;
	return true;
}

CVendorSellResponse::CVendorSellResponse( bool vendVal, std::string text ) : CBaseVendorResponse( vendVal, text )
{
}
bool CVendorSellResponse::Handle( CSocket *mSock, CChar *mChar, CChar *Npc )
{
	Npc->SetTimer( tNPC_MOVETIME, BuildTimeValue( 60 ) );
	CPSellList toSend( (*mChar), (*Npc) );
	mSock->Send( &toSend );
	return false;
}

CVendorGoldResponse::CVendorGoldResponse( bool vendVal, std::string text ) : CBaseVendorResponse( vendVal, text )
{
}
bool CVendorGoldResponse::Handle( CSocket *mSock, CChar *mChar, CChar *Npc )
{
	if( Npc->GetNPCAiType() == aiPLAYERVENDOR )
	{
		CChar *mChar = mSock->CurrcharObj();
		UI32 pay = 0, give = Npc->GetHoldG(), t = 0;
		if( mChar == Npc->GetOwnerObj() )
		{
			if( Npc->GetHoldG() <= 0 )
			{
				Npc->talk( mSock, 1326, false );
				Npc->SetHoldG( 0 );
			} 
			else if( Npc->GetHoldG() > 0 && Npc->GetHoldG() <= MAX_STACK )
			{
				if( Npc->GetHoldG() > 9 )
				{
					pay = (int)( Npc->GetHoldG() / 10 );
					give = Npc->GetHoldG() - pay;
				} 
				else 
				{
					pay = Npc->GetHoldG();
					give = 0;
				}
				Npc->SetHoldG( 0 );
			}
			else 
			{
				t = Npc->GetHoldG() - MAX_STACK;	// yank of 65 grand, then do calculations
				Npc->SetHoldG( t );
				pay = 6554;
				give = 58981;
				if( t > 0 )
					Npc->talk( mSock, 1327, false );
			}
			if( give ) 
				Items->CreateItem( mSock, mChar, 0x0EED, give, 0, OT_ITEM, true );
			
			Npc->talk( mSock, 1328, false, Npc->GetHoldG(), pay, give );
		} 
		else 
			Npc->talk( mSock, 1329, false );
	}
	if( !saidVendor )
		return false;
	return true;
}

CHouseMultiResponse::CHouseMultiResponse( TargetIDs targVal, SI32 dictVal )
{
	targID			= targVal;
	dictEntry		= dictVal;
}
void CHouseMultiResponse::Handle( CSocket *mSock, CChar *mChar )
{
	CMultiObj *realHouse = findMulti( mChar );
	if( ValidateObject( realHouse ) ) 
	{
		if( realHouse->CanBeObjType( OT_MULTI ) && realHouse->IsOwner( mChar ) )
		{
			mSock->TempObj( realHouse );
			mSock->target( 0, targID, dictEntry );
		}
	}
}

CBoatResponse::CBoatResponse( std::string text )
{
	ourText = text;
}
// void CBoatResponse::Handle( CSocket *mSock, CChar *mChar ) in Boats.cpp

CBoatMultiResponse::CBoatMultiResponse( UI08 mType )
{
	moveType = mType;
}
CBoatObj * GetBoat( CSocket *s );
void CBoatMultiResponse::Handle( CSocket *mSock, CChar *mChar )
{
	CBoatObj *boat = GetBoat( mSock );
	if( !ValidateObject( boat ) )
		return;

	boat->SetMoveType( moveType );

	CItem *tiller = calcItemObjFromSer( boat->GetTiller() );
	if( ValidateObject( tiller ) )
		tiller->itemTalk( mSock, 10 );
}

}
