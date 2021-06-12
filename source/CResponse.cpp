//o-----------------------------------------------------------------------------------------------o
//|	File		-	CResponse.cpp
//|	Date		-	11/15/2003
//o-----------------------------------------------------------------------------------------------o
//|	Changes		-	Version History
//|
//|							1.0		 		15th November, 2003
//|							Initial implementation.  Ripped out 99% of cSpeech::response
//|							and relocated contents here in individual handler classes
//|
//|							1.1		 		16th November, 2003
//|							Changed CBaseResponse::Handle() overload to WhichResponse(), and
//|								locally declared it wherever it was used, also handles different
//|								languages in WhichResponse() now.
//|							Added an inline findString() helper function to clean up WhichResponse().
//|							Now houses our vendor responses (enabling us to completely remove
//|								checkForVendorResponse() and responsevendor().
//|							Also handles all currently supported house speech using a single
//|								class, enabling the removal of HouseSpeech().
//|
//|							1.2		 		17th November, 2003
//|							We now make use of OSI's trigger words (the few that we actually handle,
//|								currently), to remove the need to search through the text string
//o-----------------------------------------------------------------------------------------------o
#include "uox3.h"
#include "CResponse.h"
#include "regions.h"
#include "msgboard.h"
#include "townregion.h"
#include "classes.h"
#include "cEffects.h"
#include "Dictionary.h"
#include "CPacketSend.h"
#include "CJSMapping.h"
#include "cScript.h"
#include "regions.h"
#include "cGuild.h"
#include "StringUtility.hpp"


bool BuyShop( CSocket *s, CChar *c );
void callGuards( CChar *mChar );

inline bool findString( std::string toCheck, std::string toFind )
{
	return ( toCheck.find( toFind ) != std::string::npos );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CHARLIST findNearbyNPCs( CChar *mChar, distLocs distance )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a list of NPCs that are within a certain distance
//o-----------------------------------------------------------------------------------------------o
CHARLIST findNearbyNPCs( CChar *mChar, distLocs distance )
{
	CHARLIST ourNpcs;
	REGIONLIST nearbyRegions = MapRegion->PopulateList( mChar );
	for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
	{
		CMapRegion *CellResponse = (*rIter);
		if( CellResponse == nullptr )
			continue;

		GenericList< CChar * > *regChars = CellResponse->GetCharList();
		regChars->Push();
		for( CChar *Npc = regChars->First(); !regChars->Finished(); Npc = regChars->Next() )
		{
			if( !ValidateObject( Npc ) || Npc == mChar || !Npc->IsNpc() || Npc->GetInstanceID() != mChar->GetInstanceID() )
				continue;
			if( objInRange( mChar, Npc, distance ) )
				ourNpcs.push_back( Npc );
		}
		regChars->Pop();
	}
	return ourNpcs;
}

ITEMLIST findNearbyItems( CBaseObject *mObj, distLocs distance );
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool DoJSResponse( CSocket *mSock, CChar *mChar, const std::string& text )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if nearby NPCs run scripts that react to speech
//o-----------------------------------------------------------------------------------------------o
bool DoJSResponse( CSocket *mSock, CChar *mChar, const std::string& text )
{
	CChar *Npc			= nullptr;
	CHARLIST nearbyNPCs = findNearbyNPCs( mChar, DIST_INRANGE );
	for( CHARLIST_ITERATOR nIter = nearbyNPCs.begin(); nIter != nearbyNPCs.end(); ++nIter )
	{
		Npc = (*nIter);
		if( !ValidateObject( Npc ) )
			continue;

		if( abs( mChar->GetZ() - Npc->GetZ() ) <= 9 )
		{
			std::vector<UI16> scriptTriggers = Npc->GetScriptTriggers();
			for( auto i : scriptTriggers )
			{
				cScript *toExecute	= JSMapping->GetScript( i );
				if( toExecute != nullptr )
				{
					//|				-1	=> No such function or bad call
					//|				0	=> Let other NPCs and PCs see it
					//|				1	=> Let other PCs see it
					//|				2	=> Let no one else see it
					SI08 rVal = -1;
					if( Npc->isDisabled() )
						Npc->TextMessage( nullptr, 1291, TALK, false );
					else
						rVal = toExecute->OnSpeech( text.c_str(), mChar, Npc );
					switch( rVal )
					{
						case 1:		// No other NPCs to see it, but PCs should
							return true;
						case 2:		// no one else to see it
							return false;
						case 0:		// Other NPCs and PCs to see it
						case -1:	// no function, so do nothing... NOT handled!
						default:
							break;
					}
				}
			}
		}
	}

	// How about items?
	if( cwmWorldState->ServerData()->ItemsDetectSpeech() )
	{
		CItem *Item			= nullptr;
		ITEMLIST nearbyItems = findNearbyItems( mChar, DIST_INRANGE );
		for( ITEMLIST_ITERATOR nIter = nearbyItems.begin(); nIter != nearbyItems.end(); ++nIter )
		{
			Item = (*nIter);
			if( !ValidateObject( Item ) )
				continue;

			if( abs( mChar->GetZ() - Item->GetZ() ) <= 9 )
			{
				std::vector<UI16> scriptTriggers = Item->GetScriptTriggers();
				for( auto i : scriptTriggers )
				{
					cScript *toExecute	= JSMapping->GetScript( i );
					if( toExecute != nullptr )
					{
						SI08 rVal = -1;
						if( Item->isDisabled() )
							Item->TextMessage( nullptr, 1291, TALK, false );
						else
							rVal = toExecute->OnSpeech( text.c_str(), mChar, Item );
						switch( rVal )
						{
							case 1:		// No other NPCs to see it, but PCs should
								return true;
							case 2:		// no one else to see it
								return false;
							case 0:		// Other NPCs and PCs to see it
							case -1:	// no function, so do nothing... NOT handled!
							default:
								break;
						}
					}
				}
			}
		}
	}
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool WhichResponse( CSocket *mSock, CChar *mChar, std::string text )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle responses to keywords in player speech
//o-----------------------------------------------------------------------------------------------o
bool WhichResponse( CSocket *mSock, CChar *mChar, std::string text )
{
	if( !DoJSResponse( mSock, mChar, text ) )
		return false;

	CBaseResponse *tResp	= nullptr;

	for( UI16 trigWord = mSock->FirstTrigWord(); !mSock->FinishedTrigWords(); trigWord = mSock->NextTrigWord() )
	{
		switch( trigWord )
		{
			case TW_COUNT:																						break;
			case TW_GUARDS:				callGuards( mChar );													break;
			case TW_KILLS:				tResp = new CKillsResponse();											break;
			case TW_BANK:				tResp = new CBankResponse();											break;
			case TW_BALANCE:			tResp = new CBankResponse( true );										break;
			case TW_QUESTTAKE:			tResp = new CEscortResponse();											break;
			case TW_QUESTDEST:			tResp = new CEscortResponse( true );									break;
			case TW_TRAIN:				tResp = new CTrainingResponse( strutil::toupper( text ));					break;
			case TW_FOLLOW:
			case TW_FOLLOW2:			tResp = new CPetMultiResponse( strutil::toupper( text ), false, TARGET_FOLLOW, 1310 );	break;
			case TW_COME:
			case TW_FOLLOWME:			tResp = new CPetComeResponse( false, strutil::toupper( text ) );							break;
			case TW_ALLCOME:
			case TW_ALLFOLLOW:
			case TW_ALLFOLLOWME:		tResp = new CPetComeResponse( true, strutil::toupper( text ) );							break;
			case TW_KILL:
			case TW_ATTACK:				tResp = new CPetAttackResponse( false, strutil::toupper( text ) );						break;
			case TW_ALLKILL:
			case TW_ALLATTACK:			tResp = new CPetAttackResponse( true, strutil::toupper( text ) );						break;
			case TW_FETCH:
			case TW_GET:
			case TW_BRING:				tResp = new CPetMultiResponse( strutil::toupper( text ), false, TARGET_GUARD, 1316 );	break;
			case TW_FRIEND:				tResp = new CPetMultiResponse( strutil::toupper( text ), true, TARGET_FRIEND, 1620 );	break;
			case TW_GUARD:				tResp = new CPetMultiResponse( strutil::toupper( text ), true, TARGET_GUARD, 1104 );		break;
			case TW_ALLGUARD:
			case TW_ALLGUARDME:			tResp = new CPetGuardResponse( true, strutil::toupper( text ) );							break;
			case TW_STOP:
			case TW_STAY:				tResp = new CPetStayResponse( false, strutil::toupper( text ) );							break;
			case TW_ALLSTOP:
			case TW_ALLSTAY:			tResp = new CPetStayResponse( true, strutil::toupper( text ) );							break;
			case TW_TRANSFER:			tResp = new CPetMultiResponse( strutil::toupper( text ), true, TARGET_TRANSFER, 1323 );		break;
			case TW_RELEASE:			tResp = new CPetReleaseResponse( strutil::toupper( text ) );								break;
			case TW_VENDORBUY:			tResp = new CVendorBuyResponse( true, strutil::toupper( text ) );						break;
			case TW_BUY:				tResp = new CVendorBuyResponse( false, strutil::toupper( text ) );						break;
			case TW_VENDORSELL:			tResp = new CVendorSellResponse( true, strutil::toupper( text ) );						break;
			case TW_SELL:				tResp = new CVendorSellResponse( false, strutil::toupper( text ) );						break;
			case TW_VENDORVIEW:			tResp = new CVendorViewResponse( true, strutil::toupper( text ) );						break;
			case TW_VIEW:				tResp = new CVendorViewResponse( false, strutil::toupper( text ) );						break;
			case TW_VENDORGOLD:			tResp = new CVendorGoldResponse( true, strutil::toupper( text ) );						break;
			case TW_COLLECT:
			case TW_GOLD:				tResp = new CVendorGoldResponse( false, strutil::toupper( text ) );						break;
			case TW_VENDORSTATUS:		tResp = new CVendorStatusResponse( true, strutil::toupper( text ) );						break;
			case TW_STATUS:				tResp = new CVendorStatusResponse( false, strutil::toupper( text ) );					break;
			case TW_VENDORDISMISS:		tResp = new CVendorDismissResponse( true, strutil::toupper( text ) );					break;
			case TW_DISMISS:			tResp = new CVendorDismissResponse( false, strutil::toupper( text ));					break;
			case TW_HOUSEBAN:			tResp = new CHouseMultiResponse( TARGET_HOUSEBAN, 585 );				break;
			case TW_HOUSEEJECT:			tResp = new CHouseMultiResponse( TARGET_HOUSEEJECT, 587 );				break;
			case TW_HOUSELOCKDOWN:		tResp = new CHouseMultiResponse( TARGET_HOUSELOCKDOWN, 589 );			break;
			case TW_HOUSERELEASE:		tResp = new CHouseMultiResponse( TARGET_HOUSERELEASE, 591 );			break;
			case TW_HOUSESECURE:		tResp = new CHouseMultiResponse( TARGET_HOUSESECURE, 1816 );			break;
			case TW_HOUSESTRONGBOX:
			{
				// Special case because the phrase "I wish to place a strongbox" will also trigger
				// the triggerword for placing a trash barrel!
				tResp = new CHouseMultiResponse( TARGET_HOUSESTRONGBOX, -1 );
				tResp->Handle( mSock, mChar );
				delete tResp;
				tResp = nullptr;
				goto endResponseCheck; // :
			}
			case TW_TRASHBARREL:		tResp = new CHouseMultiResponse( TARGET_HOUSETRASHBARREL, -1 );			break;
			case TW_BOATFORWARD:
			case TW_BOATUNFURL:			tResp = new CBoatMultiResponse( BOAT_FORWARD );							break;
			case TW_BOATBACKWARD:		tResp = new CBoatMultiResponse( BOAT_BACKWARD );						break;
			case TW_BOATLEFT:			tResp = new CBoatMultiResponse( BOAT_LEFT );							break;
			case TW_BOATRIGHT:			tResp = new CBoatMultiResponse( BOAT_RIGHT );							break;
			case TW_BOATFORWARDLEFT:	tResp = new CBoatMultiResponse( BOAT_FORWARDLEFT );						break;
			case TW_BOATFORWARDRIGHT:	tResp = new CBoatMultiResponse( BOAT_FORWARDRIGHT );					break;
			case TW_BOATBACKLEFT:		tResp = new CBoatMultiResponse( BOAT_BACKWARDLEFT );					break;
			case TW_BOATBACKRIGHT:		tResp = new CBoatMultiResponse( BOAT_BACKWARDRIGHT );					break;
			case TW_BOATONEFORWARD:		tResp = new CBoatMultiResponse( BOAT_ONEFORWARD );						break;
			case TW_BOATONEBACK:		tResp = new CBoatMultiResponse( BOAT_ONEBACKWARD );						break;
			case TW_BOATONELEFT:		tResp = new CBoatMultiResponse( BOAT_ONELEFT );							break;
			case TW_BOATONERIGHT:		tResp = new CBoatMultiResponse( BOAT_ONERIGHT );						break;
			case TW_BOATONEFORWARDLEFT:	tResp = new CBoatMultiResponse( BOAT_ONEFORWARDLEFT );					break;
			case TW_BOATONEFORWARDRIGHT:tResp = new CBoatMultiResponse( BOAT_ONEFORWARDRIGHT );					break;
			case TW_BOATONEBACKLEFT:	tResp = new CBoatMultiResponse( BOAT_ONEBACKWARDLEFT );					break;
			case TW_BOATONEBACKRIGHT:	tResp = new CBoatMultiResponse( BOAT_ONEBACKWARDRIGHT );				break;
			case TW_BOATSLOWFORWARD:	tResp = new CBoatMultiResponse( BOAT_SLOWFORWARD );						break;
			case TW_BOATSLOWBACK:		tResp = new CBoatMultiResponse( BOAT_SLOWBACKWARD );					break;
			case TW_BOATSLOWLEFT:		tResp = new CBoatMultiResponse( BOAT_SLOWLEFT );						break;
			case TW_BOATSLOWRIGHT:		tResp = new CBoatMultiResponse( BOAT_SLOWRIGHT );						break;
			case TW_BOATSLOWFORWARDLEFT:	tResp = new CBoatMultiResponse( BOAT_SLOWFORWARDLEFT );				break;
			case TW_BOATSLOWFORWARDRIGHT:	tResp = new CBoatMultiResponse( BOAT_SLOWFORWARDRIGHT );			break;
			case TW_BOATSLOWBACKLEFT:		tResp = new CBoatMultiResponse( BOAT_SLOWBACKWARDLEFT );			break;
			case TW_BOATSLOWBACKRIGHT:		tResp = new CBoatMultiResponse( BOAT_SLOWBACKWARDRIGHT );			break;
			case TW_BOATSTOP:
			case TW_STOP2:
			case TW_BOATFURL:			//tResp = new CBoatMultiResponse( BOAT_STOP );							break;
			{
				tResp = new CBoatMultiResponse( BOAT_STOP );
				tResp->Handle( mSock, mChar );
				delete tResp;
				tResp = nullptr;
				goto endResponseCheck; // :
			}
			case TW_BOATTURNRIGHT:
			case TW_BOATSTARBOARD:
			case TW_BOATTURNLEFT:
			case TW_BOATPORT:
			case TW_BOATTURNAROUND:
			case TW_BOATANCHORDROP:
			case TW_BOATANCHORRAISE:
			case TW_SETNAME:			tResp = new CBoatResponse( text, trigWord );						break;
			case TW_RESIGN:				GuildSys->Resign( mSock );											break;
			default:
				//This is to handle the "train <skill>" keywords
				if(( trigWord >= 0x006D && trigWord <= 0x009C ) || trigWord == 0x154 || trigWord == 0x115 ||
				   trigWord == 0x17C || trigWord == 0x17D || trigWord == 0x17E )
				{
					tResp = new CTrainingResponse( strutil::toupper( text ));									break;
				}
#if defined( UOX_DEBUG_MODE )
				Console.print( strutil::format("Unhandled TriggerWord sent by the client 0x%X\n", trigWord) );
#endif
				break;
		}

		if( tResp != nullptr )
		{
			tResp->Handle( mSock, mChar );
			delete tResp;
			tResp = nullptr;
		}
	}
	endResponseCheck:
	return true;
}

CEscortResponse::CEscortResponse( bool newVal )
{
	findDest = newVal;
}
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CEscortResponse::Handle( CSocket *mSock, CChar *mChar )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles NPC escort response to player wanting to take on the escorting job
//o-----------------------------------------------------------------------------------------------o
void CEscortResponse::Handle( CSocket *mSock, CChar *mChar )
{
	// If the PC is dead then break out, The dead cannot accept quests
	if( mChar->IsDead() )
		return;
	CHARLIST npcList = findNearbyNPCs( mChar, DIST_NEARBY );
	for( CHARLIST_CITERATOR npcCtr = npcList.begin(); npcCtr != npcList.end(); ++npcCtr )
	{
		CChar *Npc = (*npcCtr);
		if( Npc->GetQuestType() == QT_ESCORTQUEST )
		{
			// I WILL TAKE THEE
			// If this is a request for hire and the PC is within range of the NPC and the NPC is waiting for an ESCORT
			if( !findDest )
			{
				if( !ValidateObject( Npc->GetFTarg() ) )
				{
					Npc->SetOwner( mChar );
					Npc->SetFTarg( mChar );			// Set the NPC to follow the PC
					Npc->SetNpcWander( WT_FOLLOW );
					Npc->SetTimer( tNPC_SUMMONTIME, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_ESCORTACTIVE ) );			// Set the expire time if nobody excepts the quest

					// Send out the rant about accepting the escort
					Npc->TextMessage( nullptr, 1294, TALK, 0, cwmWorldState->townRegions[Npc->GetQuestDestRegion()]->GetName().c_str() );

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
					Npc->TextMessage( nullptr, 1295, TALK, 0, cwmWorldState->townRegions[Npc->GetQuestDestRegion()]->GetName().c_str() );	// Send out the rant about accepting the escort
				else if( !ValidateObject( Npc->GetFTarg() ) )  // If nobody has been accepted for the quest yet
					Npc->TextMessage( nullptr, 1296, TALK, 0, cwmWorldState->townRegions[Npc->GetQuestDestRegion()]->GetName().c_str() );	// Send out the rant about accepting the escort
				else // The must be enroute
					Npc->TextMessage( nullptr, 1297, TALK, 0, cwmWorldState->townRegions[Npc->GetQuestDestRegion()]->GetName().c_str(), Npc->GetFTarg()->GetName().c_str() );	// Send out a message saying we are already being escorted
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
void ClilocMessage( CSocket *mSock, CBaseObject *srcObj, UI08 type, UI16 hue, UI16 font, UI32 messageNum, bool sendAll, const char *types = "", ... );
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CBankResponse::Handle( CSocket *mSock, CChar *mChar )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles NPC bankers response to players wanting to use bank commands
//o-----------------------------------------------------------------------------------------------o
void CBankResponse::Handle( CSocket *mSock, CChar *mChar )
{
	if( !mChar->IsDead() )
	{
		CHARLIST npcList = findNearbyNPCs( mChar, DIST_INRANGE );
		for( CHARLIST_CITERATOR npcCtr = npcList.begin(); npcCtr != npcList.end(); ++npcCtr )
		{
			CChar *Npc = (*npcCtr);
			if( Npc->GetNPCAiType() == AI_BANKER )// if he's a banker and we're close!
			{
				if( !checkBalance )
					mSock->openBank( mChar );
				else
				{
					UI32 goldCount = GetBankCount( mChar, 0x0EED );
					ClilocMessage( mSock, Npc, 0, 0x0040, FNT_NORMAL, 1042759, false, "i", goldCount );
					//Npc->TextMessage( mSock, 1298, TALK, 1, mChar->GetName().c_str(), goldCount );
				}
				break;
			}
		}
	}
}

CKillsResponse::CKillsResponse( void )
{
}
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CKillsResponse::Handle( CSocket *mSock, CChar *mChar )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response to players wanting to see the status of their kills
//o-----------------------------------------------------------------------------------------------o
void CKillsResponse::Handle( CSocket *mSock, CChar *mChar )
{
	if( !mChar->IsDead() && mSock != nullptr )
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

CTrainingResponse::CTrainingResponse( const std::string &text )
{
	ourText = text;
}
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CTrainingResponse::Handle( CSocket *mSock, CChar *mChar )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response to players wanting to train skills from NPCs
//o-----------------------------------------------------------------------------------------------o
void CTrainingResponse::Handle( CSocket *mSock, CChar *mChar )
{
	if( cwmWorldState->ServerData()->NPCTrainingStatus() ) //if the player wants to train
	{
		constexpr auto maxsize = 512 ;
		std::string temp ;
		std::string temp2 ;
		CHARLIST npcList = findNearbyNPCs( mChar, DIST_INRANGE );
		auto UText = strutil::toupper( ourText );
		auto skillName = std::string("");
		bool foundString = false;
		for( CHARLIST_CITERATOR npcCtr = npcList.begin(); npcCtr != npcList.end(); ++npcCtr )
		{
			CChar *Npc = (*npcCtr);
			if( cwmWorldState->creatures[Npc->GetID()].IsHuman() )
			{
				// Stop the NPC from moving for a minute while talking with the player
				Npc->SetTimer( tNPC_MOVETIME, BuildTimeValue( 60 ) );
				mSock->TempObj( nullptr ); //this is to prevent errors when a player says "train <skill>" then doesn't pay the npc
				SI16 skill = -1;
				auto ssecs = strutil::sections( UText, " " );
				if( ssecs.size() > 1 ) //Only if string has more than one word
				{
					//Search for perfect match first
					for( UI08 i = 0; i < ALLSKILLS; ++i )
					{
						if( findString( UText, cwmWorldState->skill[i].name ))
						{
							skill = i;
							break;
						}
					}
					//Then combine first 4 letters in each
					if( skill == -1 )
					{
						UText = (ssecs[1]).substr(0,4); //search using first three letters only
						for( UI08 i = 0; i < ALLSKILLS; ++i )
						{
							skillName = (cwmWorldState->skill[i].name).substr(0,4); //search using first three letters only
							foundString = findString( UText, skillName );
							if( foundString )
							{
								skill = i;
								break;
							}
						}
					}
					//If we STILL didn't find a match, try a reverse lookup!
					if( skill == -1 )
					{
						for( UI08 i = 0; i < ALLSKILLS; ++i )
						{
							skillName = cwmWorldState->skill[i].name;
							foundString = findString( cwmWorldState->skill[i].name, UText );
							if( foundString )
							{
								skill = i;
								break;
							}
						}
					}
				}
				if( skill == -1 ) // Didn't ask to be trained in a specific skill - Leviathan fix
				{
					if( !Npc->CanTrain() )
					{
						Npc->TextMessage( mSock, 1302, TALK, false ); // I am sorry, but I have nothing to teach thee.
						continue;
					}
					Npc->SetTrainingPlayerIn( 255 ); // Like above, this is to prevent  errors when a player says "train <skill>" then doesn't pay the npc
					temp =  Dictionary->GetEntry( 1303 ); // I can teach thee the following skills:
					UI08 skillsToTrainIn = 0;
					UI08 lastCommaPos = 0;
					for( UI08 j = 0; j < ALLSKILLS; ++j )
					{
						if( Npc->GetBaseSkill( j ) > 10 )
						{
							temp2= strutil::tolower(cwmWorldState->skill[j].name);
							if( !skillsToTrainIn ) {
								temp2[0] = toupper( temp2[0] ); // If it's the first skill,  capitalize it, and add a space in front.
								temp += (" " + temp2);
							}
							else if( skillsToTrainIn <= 10 )
							{
								// Additional skills
								lastCommaPos = static_cast<UI08>(temp.size()) + 1;
								temp += (", " + temp2);
							}
							else if( skillsToTrainIn > 10 ) // to stop UOX3 from crashing/sentence being cut off if NPC is too knowledgable!
							{
								temp += std::string(" and possibly more");
								break;
							}
							++skillsToTrainIn;
						}
					}
					if( skillsToTrainIn <= 10 )
					{
						// Replace last comma with " and "
						temp.replace( lastCommaPos, 1, " and " );
					}
					if( skillsToTrainIn )
					{
						temp += '.'; // Make last character a . just to look nicer
						Npc->TextMessage( mSock, temp, TALK, false );
					}
					else
						Npc->TextMessage( mSock, 1302, TALK, false );
				}
				else // They do want to learn a specific skill
				{
					if( !Npc->CanTrain() )
					{
						Npc->TextMessage( mSock, 1302, TALK, false );
						continue;
					}
					if( Npc->GetBaseSkill( (UI08)skill ) > 10 )
					{
						temp = strutil::format(maxsize, Dictionary->GetEntry( 1304 ), strutil::tolower(cwmWorldState->skill[skill].name ).c_str() );
						if( mChar->GetBaseSkill( (UI08)skill ) >= 250 ){
							temp += Dictionary->GetEntry( 1305 );
						}
						else
						{
							if( Npc->GetBaseSkill( (UI08)skill ) <= 250){
								temp2=strutil::format(maxsize, Dictionary->GetEntry( 1306 ),(SI32)( Npc->GetBaseSkill( (UI08)skill ) / 2 / 10 ),(SI32)( Npc->GetBaseSkill( (UI08)skill ) / 2 ) - mChar->GetBaseSkill( (UI08)skill ) );
							}
							else {
								temp2 = strutil::format(maxsize, Dictionary->GetEntry( 1306 ), 25, 250 - mChar->GetBaseSkill( (UI08)skill ) );
							}
							temp += temp2 ;
							mSock->TempObj( Npc );
							Npc->SetTrainingPlayerIn( (UI08)skill );
						}
						Npc->TextMessage( mSock, temp, TALK, false );
					}
					else
						Npc->TextMessage( mSock, 1307, TALK, false );
				}
				break;
			}
		}
	}
}

CBasePetResponse::CBasePetResponse( const std::string &text )
{
	ourText = text;
}
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CBasePetResponse::Handle( CSocket *mSock, CChar *mChar )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response to players wanting to execute pet commands
//o-----------------------------------------------------------------------------------------------o
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
	if( ValidateObject( Npc->GetOwnerObj() ) && Npc->GetNPCAiType() != AI_PLAYERVENDOR && Npc->GetQuestType() == 0 )
	{
		if( Npc->GetOwnerObj() == mChar )
			retVal = true;
		else if( !isRestricted && Npcs->checkPetFriend( mChar, Npc ) )
			retVal = true;
	}
	return retVal;
}

CPetMultiResponse::CPetMultiResponse( const std::string &text, bool restrictVal, TargetIDs targVal, SI32 dictVal ) : CBasePetResponse( text )
{
	isRestricted	= restrictVal;
	targID			= targVal;
	dictEntry		= dictVal;
}
bool CPetMultiResponse::Handle( CSocket *mSock, CChar *mChar, CChar *Npc )
{
	if( canControlPet( mChar, Npc, isRestricted ))
	{
		if( findString( ourText, strutil::toupper( Npc->GetName() )))
		{
			mSock->TempObj( Npc );
			mSock->target( 0, targID, dictEntry );
			return false;
		}
	}
	return true;
}

CPetReleaseResponse::CPetReleaseResponse( const std::string &text ) : CBasePetResponse( text )
{
}
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CPetReleaseResponse::Handle( CSocket *mSock, CChar *mChar, CChar *Npc )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response to players wanting to release pets
//o-----------------------------------------------------------------------------------------------o
bool CPetReleaseResponse::Handle( CSocket *mSock, CChar *mChar, CChar *Npc )
{
	if( canControlPet( mChar, Npc, true ))
	{
		if( findString( ourText, strutil::toupper( Npc->GetName() )))
		{
			Npcs->stopPetGuarding( Npc );
			Npc->SetFTarg( nullptr );
			Npc->SetNpcWander( WT_FREE );
			Npc->SetOwner( nullptr );
			Npc->TextMessage( nullptr, 1325, TALK, 0, Npc->GetName().c_str() );
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

CPetAllResponse::CPetAllResponse( bool allVal, const std::string &text ) : CBasePetResponse( text )
{
	saidAll = allVal;
}

CPetGuardResponse::CPetGuardResponse( bool allVal, const std::string &text ) : CPetAllResponse( allVal, text )
{
}
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CPetGuardResponse::Handle( CSocket *mSock, CChar *mChar, CChar *Npc )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response to players wanting their pets to guard an object
//o-----------------------------------------------------------------------------------------------o
bool CPetGuardResponse::Handle( CSocket *mSock, CChar *mChar, CChar *Npc )
{
	if( canControlPet( mChar, Npc, true ))
	{
		if( saidAll || findString( ourText, strutil::toupper( Npc->GetName() )))
		{
			Npcs->stopPetGuarding( Npc );
			mSock->sysmessage( 1321 );
			Npc->SetNPCAiType( AI_PET_GUARD );
			Npc->SetGuarding( mChar );
			mChar->SetGuarded( true );
			Npc->SetFTarg( mChar );
			Npc->SetNpcWander( WT_FOLLOW );
			if( !saidAll )
				return false;
		}
	}
	return true;
}

CPetAttackResponse::CPetAttackResponse( bool allVal, const std::string &text ) : CPetAllResponse( allVal, text )
{
}
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CPetAttackResponse::Handle( CSocket *mSock, CChar *mChar, CChar *Npc )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response to players wanting their pets to attack something
//o-----------------------------------------------------------------------------------------------o
bool CPetAttackResponse::Handle( CSocket *mSock, CChar *mChar, CChar *Npc )
{
	if( canControlPet( mChar, Npc, true ))
	{
		if( saidAll || findString( ourText, strutil::toupper( Npc->GetName() )))
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

CPetComeResponse::CPetComeResponse( bool allVal, const std::string &text ) : CPetAllResponse( allVal, text )
{
}
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CPetComeResponse::Handle( CSocket *mSock, CChar *mChar, CChar *Npc )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response to players wanting their pets to follow
//o-----------------------------------------------------------------------------------------------o
bool CPetComeResponse::Handle( CSocket *mSock, CChar *mChar, CChar *Npc )
{
	if( canControlPet( mChar, Npc ))
	{
		if( saidAll || findString( ourText, strutil::toupper( Npc->GetName() )))
		{
			Npcs->stopPetGuarding( Npc );
			Npc->SetFTarg( mChar );
			Npc->SetNpcWander( WT_FOLLOW );
			mSock->sysmessage( 1318 );
			if( !saidAll )
				return false;
		}
	}
	return true;
}

CPetStayResponse::CPetStayResponse( bool allVal, const std::string &text ) : CPetAllResponse( allVal, text )
{
}
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CPetStayResponse::Handle( CSocket *mSock, CChar *mChar, CChar *Npc )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response to players wanting their pets to stay
//o-----------------------------------------------------------------------------------------------o
bool CPetStayResponse::Handle( CSocket *mSock, CChar *mChar, CChar *Npc )
{
	if( canControlPet( mChar, Npc ))
	{
		if( saidAll || findString( ourText, strutil::toupper( Npc->GetName() )))
		{
			Npcs->stopPetGuarding( Npc );
			Npc->SetFTarg( nullptr );
			Npc->SetTarg( nullptr );
			if( Npc->IsAtWar() )
				Npc->ToggleCombat();
			Npc->SetNpcWander( WT_NONE );
			if( !saidAll )
				return false;
		}
	}
	return true;
}

CBaseVendorResponse::CBaseVendorResponse( bool vendVal, const std::string &text )
{
	saidVendor	= vendVal;
	ourText		= text;
}
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CBaseVendorResponse::Handle( CSocket *mSock, CChar *mChar )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response to players wanting to interact with a vendor
//o-----------------------------------------------------------------------------------------------o
void CBaseVendorResponse::Handle( CSocket *mSock, CChar *mChar )
{
	CHARLIST npcList = findNearbyNPCs( mChar, DIST_INRANGE );
	for( CHARLIST_CITERATOR npcCtr = npcList.begin(); npcCtr != npcList.end(); ++npcCtr )
	{
		CChar *Npc = (*npcCtr);
		if( Npc->IsShop() || Npc->GetNPCAiType() == AI_PLAYERVENDOR )
		{
			if( !LineOfSight( mSock, mChar, Npc->GetX(), Npc->GetY(), ( Npc->GetZ() + 15 ), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false ) )
				continue;

			if( saidVendor || findString( ourText,strutil::toupper( Npc->GetName() )))
			{
				if( !Handle( mSock, mChar, Npc ) )
					break;
			}
		}
	}
}

CVendorBuyResponse::CVendorBuyResponse( bool vendVal, const std::string &text ) : CBaseVendorResponse( vendVal, text )
{
}
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CVendorBuyResponse::Handle( CSocket *mSock, CChar *mChar, CChar *Npc )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response to players wanting to buy something from a vendor
//o-----------------------------------------------------------------------------------------------o
bool CVendorBuyResponse::Handle( CSocket *mSock, CChar *mChar, CChar *Npc )
{
	Npc->SetTimer( tNPC_MOVETIME, BuildTimeValue( 60 ) );
	if( Npc->GetNPCAiType() == AI_PLAYERVENDOR )
	{
		mSock->TempObj( Npc );
		Npc->TextMessage( mSock, 1333, TALK, false );
		mSock->target( 0, TARGET_PLVBUY, " " );
		return false;
	}
	else if( BuyShop( mSock, Npc ) )
		return false;
	return true;
}

CVendorSellResponse::CVendorSellResponse( bool vendVal, const std::string &text ) : CBaseVendorResponse( vendVal, text )
{
}
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CVendorSellResponse::Handle( CSocket *mSock, CChar *mChar, CChar *Npc )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response to players wanting to sell something to a vendor
//o-----------------------------------------------------------------------------------------------o
bool CVendorSellResponse::Handle( CSocket *mSock, CChar *mChar, CChar *Npc )
{
	//Check if vendor has onSell script running
	std::vector<UI16> scriptTriggers = Npc->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			// -1 == script doesn't exist, or returned -1
			// 0 == script returned false, 0, or nothing - don't execute hard code
			// 1 == script returned true or 1
			if( toExecute->OnSell( mSock, Npc ) == 0 ) //if script returns false, don't continue
			{
				return false;
			}
		}
	}

	Npc->SetTimer( tNPC_MOVETIME, BuildTimeValue( 60 ) );
	CPSellList toSend;
	if( toSend.CanSellItems( (*mChar), (*Npc) ) )
		mSock->Send( &toSend );
	else
		Npc->TextMessage( mSock, 1341, TALK, false );

	return false;
}

CVendorViewResponse::CVendorViewResponse( bool vendVal, const std::string &text ) : CBaseVendorResponse( vendVal, text )
{
}
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CVendorViewResponse::Handle( CSocket *mSock, CChar *mChar, CChar *Npc )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response to players wanting to view the items a vendor has for sale
//o-----------------------------------------------------------------------------------------------o
bool CVendorViewResponse::Handle( CSocket *mSock, CChar *mChar, CChar *Npc )
{
	if( Npc->GetNPCAiType() == AI_PLAYERVENDOR )
	{
		CItem *pack		= nullptr;
		Npc->TextMessage( mSock, 385, TALK, false );
		pack = Npc->GetPackItem();
		if( ValidateObject( pack ) )
			mSock->openPack( pack, true );
	}
	if( !saidVendor )
		return false;
	return true;
}

CVendorGoldResponse::CVendorGoldResponse( bool vendVal, const std::string &text ) : CBaseVendorResponse( vendVal, text )
{
}
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CVendorGoldResponse::Handle( CSocket *mSock, CChar *mChar, CChar *Npc )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response to players wanting to withdraw their earnings from their vendor
//o-----------------------------------------------------------------------------------------------o
bool CVendorGoldResponse::Handle( CSocket *mSock, CChar *mChar, CChar *Npc )
{
	if( Npc->GetNPCAiType() == AI_PLAYERVENDOR )
	{
		CChar *mChar = mSock->CurrcharObj();
		if( mChar == Npc->GetOwnerObj() )
		{
			UI32 pay = 0, give = Npc->GetHoldG(), earned = Npc->GetHoldG();
			if( Npc->GetHoldG() <= 0 )
			{
				Npc->TextMessage( mSock, 1326, TALK, false );
				Npc->SetHoldG( 0 );
			}
			else if( Npc->GetHoldG() > 0 && Npc->GetHoldG() <= MAX_STACK )
			{
				if( Npc->GetHoldG() > 9 )
				{
					pay = (SI32)( Npc->GetHoldG() / 10 );
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
				UI32 t = Npc->GetHoldG() - MAX_STACK;	// yank of 65 grand, then do calculations
				Npc->SetHoldG( t );
				pay = 6554;
				give = 58981;
				if( t > 0 )
					Npc->TextMessage( mSock, 1327, TALK, false );
			}
			if( give )
				Items->CreateScriptItem( mSock, mChar, "0x0EED", give, OT_ITEM, true );

			Npc->TextMessage( mSock, 1328, TALK, 0, earned, pay, give );
		}
		else
			Npc->TextMessage( mSock, 1329, TALK, false );
	}
	if( !saidVendor )
		return false;
	return true;
}

CVendorStatusResponse::CVendorStatusResponse( bool vendVal, const std::string &text ) : CBaseVendorResponse( vendVal, text )
{
}
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CVendorStatusResponse::Handle( CSocket *mSock, CChar *mChar, CChar *Npc )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response to players wanting to see the status of their vendor
//o-----------------------------------------------------------------------------------------------o
bool CVendorStatusResponse::Handle( CSocket *mSock, CChar *mChar, CChar *Npc )
{
	if( Npc->GetNPCAiType() == AI_PLAYERVENDOR )
	{
		CChar *mChar = mSock->CurrcharObj();
		if( mChar == Npc->GetOwnerObj() )
		{
			if( Npc->GetHoldG() <= 0 )
			{
				Npc->TextMessage( mSock, 1326, TALK, false );
			}
			else
			{
				UI32 pay = 0;
				if( Npc->GetHoldG() > 9 )
				{
					pay = (SI32)( Npc->GetHoldG() / 10 );
				}
				else
				{
					pay = Npc->GetHoldG();
				}
				Npc->TextMessage( mSock, 1782, TALK, 0, Npc->GetHoldG(), pay );
			}
		}
		else
			Npc->TextMessage( mSock, 1329, TALK, false );
	}
	if( !saidVendor )
		return false;
	return true;
}

CVendorDismissResponse::CVendorDismissResponse( bool vendVal, const std::string &text ) : CBaseVendorResponse( vendVal, text )
{
}
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CVendorDismissResponse::Handle( CSocket *mSock, CChar *mChar, CChar *Npc )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response to players wanting to dismiss their vendor
//o-----------------------------------------------------------------------------------------------o
bool CVendorDismissResponse::Handle( CSocket *mSock, CChar *mChar, CChar *Npc )
{
	if( Npc->GetNPCAiType() == AI_PLAYERVENDOR )
	{
		CChar *mChar = mSock->CurrcharObj();
		if( mChar == Npc->GetOwnerObj() )
		{
			Npc->Delete();
		}
		else
			Npc->TextMessage( mSock, 1329, TALK, false );
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
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CHouseMultiResponse::Handle( CSocket *mSock, CChar *mChar )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response to players wanting to use house commands
//o-----------------------------------------------------------------------------------------------o
void CHouseMultiResponse::Handle( CSocket *mSock, CChar *mChar )
{
	CMultiObj *realHouse = findMulti( mChar );
	if( ValidateObject( realHouse ) && !realHouse->CanBeObjType( OT_BOAT ))
	{
		if( realHouse->CanBeObjType( OT_MULTI ))
		{
			std::vector<UI16> scriptTriggers = realHouse->GetScriptTriggers();
			for( auto scriptTrig : scriptTriggers )
			{
				cScript *toExecute = JSMapping->GetScript( scriptTrig );
				if( toExecute != nullptr )
				{
					//-1 == event not found
					// 0 == script returned false/0
					// 1 == script returned true/1
					if( toExecute->OnHouseCommand( mSock, realHouse, targID ) == 1 ) //if script returns true, don't continue
					{
						return;
					}
				}
			}

			if( realHouse->IsOwner( mChar ) )
			{
				mSock->TempObj( realHouse );
				mSock->target( 0, targID, dictEntry );
			}
		}
	}
}

CBoatResponse::CBoatResponse( const std::string &text, UI16 tW )
{
	ourText		= text;
	trigWord	= tW;
}
// void CBoatResponse::Handle( CSocket *mSock, CChar *mChar ) in Boats.cpp

CBoatMultiResponse::CBoatMultiResponse( BoatMoveType mType )
{
	moveType = mType;
}
CBoatObj * GetBoat( CSocket *s );
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CBoatMultiResponse::Handle( CSocket *mSock, CChar *mChar )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response to players wanting to use boat commands
//o-----------------------------------------------------------------------------------------------o
void CBoatMultiResponse::Handle( CSocket *mSock, CChar *mChar )
{
	// Let's do some spam-prevention here
	if( mChar->GetTimer( tCHAR_ANTISPAM ) > cwmWorldState->GetUICurrentTime() )
	{
		return;
	}
	else
	{
		mChar->SetTimer( tCHAR_ANTISPAM, BuildTimeValue( (R32)cwmWorldState->ServerData()->CheckBoatSpeed() ) );
	}

	CBoatObj *boat = GetBoat( mSock );
	if( !ValidateObject( boat ) )
	{
		return;
	}

	CItem *tiller = calcItemObjFromSer( boat->GetTiller() );
	if( boat->GetMoveType() == BOAT_ANCHORED )
	{
		if( ValidateObject( tiller ) )
		{
			tiller->TextMessage( mSock, 2024 ); // Ar, the anchor is down sir!
			mSock->sysmessage( 2023 ); // You must raise the anchor to pilot the ship.
		}
		return;
	}

	boat->SetMoveType( moveType );

	if( ValidateObject( tiller ) )
	{
		tiller->TextMessage( mSock, 10 ); // Aye, sir.
	}
}
