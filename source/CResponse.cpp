//o------------------------------------------------------------------------------------------------o
//|	File		-	CResponse.cpp
//|	Date		-	11/15/2003
//o------------------------------------------------------------------------------------------------o
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
//|							Added an inline FindString() helper function to clean up WhichResponse().
//|							Now houses our vendor responses (enabling us to completely remove
//|								checkForVendorResponse() and responsevendor().
//|							Also handles all currently supported house speech using a single
//|								class, enabling the removal of HouseSpeech().
//|
//|							1.2		 		17th November, 2003
//|							We now make use of OSI's trigger words (the few that we actually handle,
//|								currently), to remove the need to search through the text string
//o------------------------------------------------------------------------------------------------o
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
#include "skills.h"
#include "StringUtility.hpp"
#include <algorithm>
#include <cctype>

bool BuyShop( CSocket *s, CChar *c );
void CallGuards( CChar *mChar );

inline bool FindString( std::string toCheck, std::string toFind )
{
	return ( toCheck.find( toFind ) != std::string::npos );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	FindNearbyNPCs()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a list of NPCs that are within a certain distance
//o------------------------------------------------------------------------------------------------o
auto FindNearbyNPCs( CChar *mChar, distLocs distance ) -> std::vector<CChar *>
{
	std::vector<CChar*> ourNpcs;

	for( auto &CellResponse : MapRegion->PopulateList( mChar ))
	{
		if( CellResponse )
		{
			auto regChars = CellResponse->GetCharList();
			for( auto &nearbyNpc : regChars->collection() )
			{
				if( ValidateObject( nearbyNpc ) && ( nearbyNpc != mChar ) && nearbyNpc->IsNpc() && ( nearbyNpc->GetInstanceId() == mChar->GetInstanceId() ))
				{
					if( ObjInRange( mChar, nearbyNpc, distance ))
					{
						ourNpcs.push_back( nearbyNpc );
					}
				}
			}
		}
	}
	return ourNpcs;
}

auto FindNearbyItems( CBaseObject *mObj, distLocs distance ) -> std::vector<CItem *>;
//o------------------------------------------------------------------------------------------------o
//|	Function	-	DoJSResponse()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if nearby NPCs run scripts that react to speech
//o------------------------------------------------------------------------------------------------o
UI08 DoJSResponse( [[maybe_unused]] CSocket *mSock, CChar *mChar, const std::string& text )
{
	for( auto &nearbyNpc : FindNearbyNPCs( mChar, DIST_CMDRANGE ))
	{
		if( !ValidateObject( nearbyNpc ))
			continue;

		// If player and NPC is too far apart on Z level, they're likely on different floors. Ignore!
		if( std::abs( mChar->GetZ() - nearbyNpc->GetZ() ) > 9 )
			continue;

		std::vector<UI16> scriptTriggers = nearbyNpc->GetScriptTriggers();
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
				if( nearbyNpc->IsDisabled() )
				{
					nearbyNpc->TextMessage( nullptr, 1291, TALK, false );
				}
				else
				{
					rVal = toExecute->OnSpeech( text.c_str(), mChar, nearbyNpc );
				}
				switch( rVal )
				{
					case 1:		// No other NPCs to see it, but PCs should
						return 1;
					case 2:		// no one else to see it
						return 2;
					case 0:		// Other NPCs and PCs to see it
					case -1:	// no function, so do nothing... NOT handled!
					default:
						break;
				}
			}
		}
	}

	// How about items? Could be more expensive, so leave that as an optional INI setting
	if( cwmWorldState->ServerData()->ItemsDetectSpeech() )
	{
		for( auto &nearbyItem : FindNearbyItems( mChar, DIST_CMDRANGE ))
		{
			if( !ValidateObject( nearbyItem ))
				continue;

			// If player and NPC is too far apart on Z level, they're likely on different floors. Ignore!
			if( std::abs( mChar->GetZ() - nearbyItem->GetZ() ) > 9 )
				continue;

			std::vector<UI16> scriptTriggers = nearbyItem->GetScriptTriggers();
			for( auto i : scriptTriggers )
			{
				cScript *toExecute	= JSMapping->GetScript( i );
				if( toExecute != nullptr )
				{
					SI08 rVal = -1;
					if( nearbyItem->IsDisabled() )
					{
						nearbyItem->TextMessage( nullptr, 1291, TALK, false );
					}
					else
					{
						rVal = toExecute->OnSpeech( text.c_str(), mChar, nearbyItem );
					}
					switch( rVal )
					{
						case 1:		// No other NPCs to see it, but PCs should
							return 1;
						case 2:		// no one else to see it
							return 2;
						case 0:		// Other NPCs and PCs to see it
						case -1:	// no function, so do nothing... NOT handled!
						default:
							break;
					}
				}
			}
		}
	}
	return 0;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	WhichResponse()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle responses to keywords in player speech
//o------------------------------------------------------------------------------------------------o
bool WhichResponse( CSocket *mSock, CChar *mChar, std::string text, CChar *tChar = nullptr )
{
	UI08 jsResponse = DoJSResponse( mSock, mChar, text );
	if( jsResponse == 1 )
		return true;

	if( jsResponse == 2 )
		return false;

	CBaseResponse *tResp	= nullptr;

	for( UI16 trigWord = mSock->FirstTrigWord(); !mSock->FinishedTrigWords(); trigWord = mSock->NextTrigWord() )
	{
		switch( trigWord )
		{
			case TW_COUNT:																						break;
			case TW_GUARDS:				CallGuards( mChar );													break;
			case TW_KILLS:				tResp = new CKillsResponse();											break;
			case TW_QUESTTAKE:			tResp = new CEscortResponse();											break;
			case TW_QUESTDEST:			tResp = new CEscortResponse( true );									break;
			case TW_TRAIN:				tResp = new CTrainingResponse( trigWord, tChar );						break;
			case TW_FOLLOW:			[[fallthrough]];
			case TW_FOLLOW2:			tResp = new CPetMultiResponse( oldstrutil::upper( text ), false, TARGET_FOLLOW, 1310, false, true ); break;
			case TW_COME:			[[fallthrough]];
			case TW_FOLLOWME:			tResp = new CPetComeResponse( false, oldstrutil::upper( text ));		break;
			case TW_ALLFOLLOWME:	[[fallthrough]];
			case TW_ALLCOME:			tResp = new CPetComeResponse( true, oldstrutil::upper( text ));			break;
			case TW_ALLFOLLOW:			tResp = new CPetMultiResponse( oldstrutil::upper( text ), false, TARGET_FOLLOW, 1310, true, true );	break;
			case TW_KILL:			[[fallthrough]];
			case TW_ATTACK:				tResp = new CPetAttackResponse( false, oldstrutil::upper( text ));		break;
			case TW_ALLKILL:		[[fallthrough]];
			case TW_ALLATTACK:
			/*{
				tResp = new CPetAttackResponse( true, oldstrutil::upper( text ));
				tResp->Handle( mSock, mChar );
				delete tResp;
				tResp = nullptr;
				goto endResponseCheck;
			}*/
				tResp = new CPetAttackResponse( true, oldstrutil::upper( text ));
				break;
			case TW_FETCH:
			case TW_GET:
			case TW_BRING:					tResp = new CPetMultiResponse( oldstrutil::upper( text ), false, TARGET_GUARD, 1316, false, true );	break;
			case TW_FRIEND:					tResp = new CPetMultiResponse( oldstrutil::upper( text ), true, TARGET_FRIEND, 1620, false, true );	break;
			case TW_GUARD:					tResp = new CPetMultiResponse( oldstrutil::upper( text ), false, TARGET_GUARD, 1104, false, true );	break;
			case TW_ALLGUARD:	[[fallthrough]];
			case TW_ALLGUARDME:				tResp = new CPetGuardResponse( true, oldstrutil::upper( text ));		break;
			case TW_STOP:		[[fallthrough]];
			case TW_STAY:					tResp = new CPetStayResponse( false, oldstrutil::upper( text ));		break;
			case TW_ALLSTOP:	[[fallthrough]];
			case TW_ALLSTAY:				tResp = new CPetStayResponse( true, oldstrutil::upper( text ));			break;
			case TW_TRANSFER:				tResp = new CPetMultiResponse( oldstrutil::upper( text ), true, TARGET_TRANSFER, 1323, false, false ); break;
			case TW_RELEASE:				tResp = new CPetReleaseResponse( oldstrutil::upper( text ));			break;
			case TW_VENDORBUY:				tResp = new CVendorBuyResponse( true, oldstrutil::upper( text ));		break;
			case TW_BUY:					tResp = new CVendorBuyResponse( false, oldstrutil::upper( text ));		break;
			case TW_VENDORSELL:				tResp = new CVendorSellResponse( true, oldstrutil::upper( text ));		break;
			case TW_SELL:					tResp = new CVendorSellResponse( false, oldstrutil::upper( text ));		break;
			case TW_VENDORVIEW:				tResp = new CVendorViewResponse( true, oldstrutil::upper( text ));		break;
			case TW_VIEW:					tResp = new CVendorViewResponse( false, oldstrutil::upper( text ));		break;
			case TW_VENDORGOLD:				tResp = new CVendorGoldResponse( true, oldstrutil::upper( text ));		break;
			case TW_COLLECT:	[[fallthrough]];
			case TW_GOLD:					tResp = new CVendorGoldResponse( false, oldstrutil::upper( text ));		break;
			case TW_VENDORSTATUS:			tResp = new CVendorStatusResponse( true, oldstrutil::upper( text ));	break;
			case TW_STATUS:					tResp = new CVendorStatusResponse( false, oldstrutil::upper( text ));	break;
			case TW_VENDORDISMISS:			tResp = new CVendorDismissResponse( true, oldstrutil::upper( text ));	break;
			case TW_DISMISS:				tResp = new CVendorDismissResponse( false, oldstrutil::upper( text ));	break;
			case TW_HOUSEBAN:				tResp = new CHouseMultiResponse( TARGET_HOUSEBAN, 585 );			break;
			case TW_HOUSEEJECT:				tResp = new CHouseMultiResponse( TARGET_HOUSEEJECT, 587 );			break;
			case TW_HOUSELOCKDOWN:			tResp = new CHouseMultiResponse( TARGET_HOUSELOCKDOWN, 589 );		break;
			case TW_HOUSERELEASE:			tResp = new CHouseMultiResponse( TARGET_HOUSERELEASE, 591 );		break;
			case TW_HOUSESECURE:			tResp = new CHouseMultiResponse( TARGET_HOUSESECURE, 1816 );		break;
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
			case TW_TRASHBARREL:			tResp = new CHouseMultiResponse( TARGET_HOUSETRASHBARREL, -1 );		break;
			case TW_BOATFORWARD:	[[fallthrough]];
			case TW_BOATUNFURL:				tResp = new CBoatMultiResponse( BOAT_FORWARD );						break;
			case TW_BOATBACKWARD:			tResp = new CBoatMultiResponse( BOAT_BACKWARD );					break;
			case TW_BOATLEFT:				tResp = new CBoatMultiResponse( BOAT_LEFT );						break;
			case TW_BOATRIGHT:				tResp = new CBoatMultiResponse( BOAT_RIGHT );						break;
			case TW_BOATFORWARDLEFT:		tResp = new CBoatMultiResponse( BOAT_FORWARDLEFT );					break;
			case TW_BOATFORWARDRIGHT:		tResp = new CBoatMultiResponse( BOAT_FORWARDRIGHT );				break;
			case TW_BOATBACKLEFT:			tResp = new CBoatMultiResponse( BOAT_BACKWARDLEFT );				break;
			case TW_BOATBACKRIGHT:			tResp = new CBoatMultiResponse( BOAT_BACKWARDRIGHT );				break;
			case TW_BOATONEFORWARD:			tResp = new CBoatMultiResponse( BOAT_ONEFORWARD );					break;
			case TW_BOATONEBACK:			tResp = new CBoatMultiResponse( BOAT_ONEBACKWARD );					break;
			case TW_BOATONELEFT:			tResp = new CBoatMultiResponse( BOAT_ONELEFT );						break;
			case TW_BOATONERIGHT:			tResp = new CBoatMultiResponse( BOAT_ONERIGHT );					break;
			case TW_BOATONEFORWARDLEFT:		tResp = new CBoatMultiResponse( BOAT_ONEFORWARDLEFT );				break;
			case TW_BOATONEFORWARDRIGHT:	tResp = new CBoatMultiResponse( BOAT_ONEFORWARDRIGHT );				break;
			case TW_BOATONEBACKLEFT:		tResp = new CBoatMultiResponse( BOAT_ONEBACKWARDLEFT );				break;
			case TW_BOATONEBACKRIGHT:		tResp = new CBoatMultiResponse( BOAT_ONEBACKWARDRIGHT );			break;
			case TW_BOATSLOWFORWARD:		tResp = new CBoatMultiResponse( BOAT_SLOWFORWARD );					break;
			case TW_BOATSLOWBACK:			tResp = new CBoatMultiResponse( BOAT_SLOWBACKWARD );				break;
			case TW_BOATSLOWLEFT:			tResp = new CBoatMultiResponse( BOAT_SLOWLEFT );					break;
			case TW_BOATSLOWRIGHT:			tResp = new CBoatMultiResponse( BOAT_SLOWRIGHT );					break;
			case TW_BOATSLOWFORWARDLEFT:	tResp = new CBoatMultiResponse( BOAT_SLOWFORWARDLEFT );				break;
			case TW_BOATSLOWFORWARDRIGHT:	tResp = new CBoatMultiResponse( BOAT_SLOWFORWARDRIGHT );			break;
			case TW_BOATSLOWBACKLEFT:		tResp = new CBoatMultiResponse( BOAT_SLOWBACKWARDLEFT );			break;
			case TW_BOATSLOWBACKRIGHT:		tResp = new CBoatMultiResponse( BOAT_SLOWBACKWARDRIGHT );			break;
			case TW_BOATSTOP:		[[fallthrough]];
			case TW_STOP2:			[[fallthrough]];
			case TW_BOATFURL:			//tResp = new CBoatMultiResponse( BOAT_STOP );							break;
			{
				tResp = new CBoatMultiResponse( BOAT_STOP );
				tResp->Handle( mSock, mChar );
				delete tResp;
				tResp = nullptr;
				goto endResponseCheck; // :
			}
			case TW_BOATTURNRIGHT:		[[fallthrough]];
			case TW_BOATSTARBOARD:		[[fallthrough]];
			case TW_BOATTURNLEFT:		[[fallthrough]];
			case TW_BOATPORT:			[[fallthrough]];
			case TW_BOATTURNAROUND:		[[fallthrough]];
			case TW_BOATANCHORDROP:		[[fallthrough]];
			case TW_BOATANCHORRAISE:	[[fallthrough]];
			case TW_SETNAME:			tResp = new CBoatResponse( text, trigWord );						break;
			case TW_RESIGN:				GuildSys->Resign( mSock );											break;
			default:
				//This is to handle the "train <skill>" keywords
				if(( trigWord >= 0x006D && trigWord <= 0x009C ) || trigWord == 0x154 || trigWord == 0x115 ||
				   trigWord == 0x17C || trigWord == 0x17D || trigWord == 0x17E )
				{
					tResp = new CTrainingResponse( trigWord, tChar );
					tResp->Handle( mSock, mChar );
					delete tResp;
					tResp = nullptr;
					goto endResponseCheck; // :
					break;
				}
#if defined( UOX_DEBUG_MODE )
				Console.Print( oldstrutil::format( "Unhandled trigger [%s] sent by the client 0x%X\n", text.c_str(), trigWord ));
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
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CEscortResponse::Handle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles NPC escort response to player wanting to take on the escorting job
//o------------------------------------------------------------------------------------------------o
void CEscortResponse::Handle( [[maybe_unused]] CSocket *mSock, CChar *mChar )
{
	// If the PC is dead then break out, The dead cannot accept quests
	if( mChar->IsDead() )
		return;

	for( auto &nearbyNpc : FindNearbyNPCs( mChar, DIST_NEARBY ))
	{
		if( nearbyNpc->GetQuestType() == QT_ESCORTQUEST )
		{
			// I WILL TAKE THEE
			// If this is a request for hire and the PC is within range of the NPC and the NPC is waiting for an ESCORT
			if( !findDest )
			{
				if( !ValidateObject( nearbyNpc->GetFTarg() ))
				{
					nearbyNpc->SetOwner( mChar );
					nearbyNpc->SetFTarg( mChar ); // Set the NPC to follow the PC
					nearbyNpc->SetNpcWander( WT_FOLLOW );
					nearbyNpc->SetTimer( tNPC_SUMMONTIME, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_ESCORTACTIVE )); // Set the expire time if nobody excepts the quest

					// Send out the rant about accepting the escort
					nearbyNpc->TextMessage( nullptr, 1294, TALK, 0, cwmWorldState->townRegions[nearbyNpc->GetQuestDestRegion()]->GetName().c_str() );

					// Remove post from message board (Mark for deletion only - will be cleaned during cleanup)
					MsgBoardQuestEscortRemovePost( nearbyNpc );
					return;
				}
				else
				{
					findDest = true; // If the current NPC already has an ftarg then respond to query for quest
				}
			}

			// DESTINATION
			// If this is a request to find out where a NPC wants to go and the PC is within range of the NPC and the NPC is waiting for an ESCORT
			if( findDest )
			{
				if( nearbyNpc->GetFTarg() == mChar )
				{
					// Send out the rant about accepting the escort
					nearbyNpc->TextMessage( nullptr, 1295, TALK, 0, cwmWorldState->townRegions[nearbyNpc->GetQuestDestRegion()]->GetName().c_str() );
				}
				else if( !ValidateObject( nearbyNpc->GetFTarg() ))  // If nobody has been accepted for the quest yet
				{
					// Send out the rant about accepting the escort
					nearbyNpc->TextMessage( nullptr, 1296, TALK, 0, cwmWorldState->townRegions[nearbyNpc->GetQuestDestRegion()]->GetName().c_str() );
				}
				else // The must be enroute
				{
					// Send out a message saying we are already being escorted
					nearbyNpc->TextMessage( nullptr, 1297, TALK, 0, cwmWorldState->townRegions[nearbyNpc->GetQuestDestRegion()]->GetName().c_str(), nearbyNpc->GetFTarg()->GetNameRequest( mChar ).c_str() );
				}
				return;
			}
		}
	}
}

CKillsResponse::CKillsResponse( void )
{
}
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CKillsResponse::Handle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response to players wanting to see the status of their kills
//o------------------------------------------------------------------------------------------------o
void CKillsResponse::Handle( CSocket *mSock, CChar *mChar )
{
	if( !mChar->IsDead() && mSock != nullptr )
	{
		SI16 i = mChar->GetKills();
		if( i == 0 )
		{
			mSock->SysMessage( 1288 ); // You are an upstanding Citizen... no kills.
		}
		else if( i > cwmWorldState->ServerData()->RepMaxKills() )
		{
			mSock->SysMessage( 1289, i ); // You are a very evil person... %i kills.
		}
		else
		{
			mSock->SysMessage( 1290, i ); // You have %i kills.
		}
	}
}

CTrainingResponse::CTrainingResponse( UI16 trigWord, CChar *tChar )
{
	trigChar = tChar;
	ourTrigWord = trigWord;
}
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CTrainingResponse::Handle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response to players wanting to train skills from NPCs
//o------------------------------------------------------------------------------------------------o
void CTrainingResponse::Handle( CSocket *mSock, CChar *mChar )
{
	if( cwmWorldState->ServerData()->NPCTrainingStatus() ) //if the player wants to train
	{
		constexpr auto maxsize = 512;
		std::string temp;
		std::string temp2;

		// Shuffle the npcList so it doesn't always trigger for the first NPC in the list
		std::random_device rd;
		std::mt19937 g( rd() );
		auto npcList = FindNearbyNPCs( mChar, DIST_INRANGE );
		std::shuffle( npcList.begin(), npcList.end(), g );

		auto skillName = std::string("");
		for( auto nearbyNpc : npcList )
		{
			if( ValidateObject( trigChar ))
			{
				nearbyNpc = trigChar;
			}
			if( cwmWorldState->creatures[nearbyNpc->GetId()].IsHuman() )
			{
				// Stop the NPC from moving for a minute while talking with the player
				nearbyNpc->SetTimer( tNPC_MOVETIME, BuildTimeValue( 60 ));
				mSock->TempObj( nullptr ); //this is to prevent errors when a player says "train <skill>" then doesn't pay the npc
				SI16 skill = -1;

				// Language independent implementation of training keywords
				switch( ourTrigWord )
				{
					case 340: // anatomy
						skill = ANATOMY; break;
					case 109: // parrying, parry, battle, defense
						skill = PARRYING; break;
					case 110: // first, aid, heal, healing, medicine
						skill = HEALING; break;
					case 111: // hide, hiding
						skill = HIDING; break;
					case 112: // steal, stealing
						skill = STEALING; break;
					case 113: // alchemy
						skill = ALCHEMY; break;
					case 114: // animal, animal lore
						skill = ANIMALLORE; break;
					case 115: // appraising, identifying, appraise, item, identification, identify
						skill = ITEMID; break;
					case 116: // armslore, arms
						skill = ARMSLORE; break;
					case 117: // beg, begging
						skill = BEGGING; break;
					case 118: // blacksmith, smith, blacksmithy, smithing, blacksmithing
						skill = BLACKSMITHING; break;
					case 119: // bower, bow, arrow, fletcher, bowcraft, fletching
						skill = BOWCRAFT; break;
					case 120: // calm, peace, peacemaking
						skill = PEACEMAKING; break;
					case 121: // camp, camping
						skill = CAMPING; break;
					case 122: // carpentry, woodwork, woodworking
						skill = CARPENTRY; break;
					case 123: // cartography, map, mapmaking
						skill = CARTOGRAPHY; break;
					case 124: // cooking, cook
						skill = COOKING; break;
					case 125: // detect, detect hidden, hidden
						skill = DETECTINGHIDDEN; break;
					case 126: // entice, enticement, enticing
						skill = ENTICEMENT; break;
					case 127: // evaluate, intelligence, evaluating
						skill = EVALUATINGINTEL; break;
					case 128: // fish, fishing
						skill = FISHING; break;
					case 129: // incite, provoke, provoking, provocation
						skill = PROVOCATION; break;
					case 130: // lockpicking, lock, pick, picking, locks
						skill = LOCKPICKING; break;
					case 131: // magic, magery, sorcery, wizardry, mage, wizard
						skill = MAGERY; break;
					case 132: // resist, resisting, spells
						skill = MAGICRESISTANCE; break;
					case 133: // battle, tactic, tactics, fight, fighting
						skill = TACTICS; break;
					case 134: // peek, peeking, snoop, snooping
						skill = SNOOPING; break;
					case 135: // disarm, disarming, remove, removing
						skill = REMOVETRAP; break;
					case 136: // play, instrument, playing, music, musician, musicianship
						skill = MUSICIANSHIP; break;
					case 137: // poisoning, poison
						skill = POISONING; break;
					case 138: // ranged, missile, missiles, shoot, shooting, archery, archer
						skill = ARCHERY; break;
					case 139: // spirit, ghost, seance, spiritualism
						skill = SPIRITSPEAK; break;
					case 140: // tailoring, tailor, clothier
						skill = TAILORING; break;
					case 141: // tame, taming
						skill = TAMING; break;
					case 142: // taste, tasting
						skill = TASTEID; break;
					case 143: // tinker, tinkering
						skill = TINKERING; break;
					case 144: // vet, veterinarian, veterinary
						skill = VETERINARY; break;
					case 145: // forensic, forensics
						skill = FORENSICS; break;
					case 146: // herd, herding
						skill = HERDING; break;
					case 147: // tracking, track, hunt, hunting
						skill = TRACKING; break;
					case 148: // stealth
						skill = STEALTH; break;
					case 149: // inscribe, scroll, inscribing, inscription
						skill = INSCRIPTION; break;
					case 150: // sword, swords, blade, blades, swordsman, swordsmanship
						skill = SWORDSMANSHIP; break;
					case 151: // club, clubs, mace, maces
						skill = MACEFIGHTING; break;
					case 152: // dagger, daggers, fence, fencing, spear
						skill = FENCING; break;
					case 153: // hand, wrestle, wrestling
						skill = WRESTLING; break;
					case 154: // lumberjack, lumberjacking, woodcuttingl
						skill = LUMBERJACKING; break;
					case 155: // mining, mine, smelt
						skill = MINING; break;
					case 156: // meditate
						skill = MEDITATION; break;
					case 380: // necromancy
						skill = NECROMANCY; break;
					case 381: // chivalry
						skill = CHIVALRY; break;
					case 382: // focus
						skill = FOCUS; break;
					default:
						break;
				}

				if( skill == -1 ) // Didn't ask to be trained in a specific skill - Leviathan fix
				{
					if( !nearbyNpc->CanTrain() )
					{
						nearbyNpc->TextMessage( mSock, 1302, TALK, false ); // I am sorry, but I have nothing to teach thee.
						continue;
					}
					nearbyNpc->SetTrainingPlayerIn( 255 ); // Like above, this is to prevent  errors when a player says "train <skill>" then doesn't pay the npc
					temp = Dictionary->GetEntry( 1303 ); // I can teach thee the following skills:
					UI08 skillsToTrainIn = 0;
					UI08 lastCommaPos = 0;
					for( UI08 j = 0; j < ALLSKILLS; ++j )
					{
						if( nearbyNpc->GetBaseSkill( j ) > 10 )
						{
							temp2 = oldstrutil::lower( cwmWorldState->skill[j].name );
							if( !skillsToTrainIn )
							{
								temp2[0] = std::toupper( temp2[0] ); // If it's the first skill,  capitalize it, and add a space in front.
								temp += ( " " + temp2 );
							}
							else if( skillsToTrainIn <= 10 )
							{
								// Additional skills
								lastCommaPos = static_cast<UI08>( temp.size() ) + 1;
								temp += ( ", " + temp2 );
							}
							else if( skillsToTrainIn > 10 ) // to stop UOX3 from crashing/sentence being cut off if NPC is too knowledgable!
							{
								temp += std::string( " and possibly more" );
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
						nearbyNpc->TextMessage( mSock, temp, TALK, false );
					}
					else
					{
						nearbyNpc->TextMessage( mSock, 1302, TALK, false ); // I am sorry, but I have nothing to teach thee.
					}
				}
				else // They do want to learn a specific skill
				{
					if( !nearbyNpc->CanTrain() )
					{
						nearbyNpc->TextMessage( mSock, 1302, TALK, false ); // I am sorry, but I have nothing to teach thee.
						continue;
					}
					if( nearbyNpc->GetBaseSkill( static_cast<UI08>( skill )) > 10 )
					{
						temp = oldstrutil::format( maxsize, Dictionary->GetEntry( 1304 ), oldstrutil::lower( cwmWorldState->skill[skill].name ).c_str() ); // Thou wishest to learn of  %s?
						if( mChar->GetBaseSkill( static_cast<UI08>( skill )) >= 250 )
						{
							temp += Dictionary->GetEntry( 1305 ); // I can teach thee no more than thou already knowest!
						}
						else
						{
							if( nearbyNpc->GetBaseSkill( static_cast<UI08>( skill )) <= 250 )
							{
								// Very well I, can train thee up to the level of %i percent for %i gold. Pay for less and I shall teach thee less.
								temp2 = oldstrutil::format( maxsize, Dictionary->GetEntry( 1306 ), static_cast<SI32>( nearbyNpc->GetBaseSkill( static_cast<UI08>( skill )) / 2 / 10 ), static_cast<SI32>( nearbyNpc->GetBaseSkill( static_cast<UI08>( skill )) / 2 ) - mChar->GetBaseSkill( static_cast<UI08>( skill )));
							}
							else
							{
								// Very well I, can train thee up to the level of %i percent for %i gold. Pay for less and I shall teach thee less.
								temp2 = oldstrutil::format( maxsize, Dictionary->GetEntry( 1306 ), 25, 250 - mChar->GetBaseSkill( static_cast<UI08>( skill )));
							}
							temp += " " + temp2;
							mSock->TempObj( nearbyNpc );
							nearbyNpc->SetTrainingPlayerIn( static_cast<UI08>( skill ));
						}
						nearbyNpc->TextMessage( mSock, temp, TALK, false );
					}
					else
					{
						nearbyNpc->TextMessage( mSock, 1307, TALK, false ); // I am sorry but I cannot train thee in that skill.
					}
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
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBasePetResponse::Handle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response to players wanting to execute pet commands
//o------------------------------------------------------------------------------------------------o
void CBasePetResponse::Handle( CSocket *mSock, CChar *mChar )
{
	TAGMAPOBJECT petTagObj = mChar->GetTag( "petTagObjSerial" );
	CChar *petCommandObj = CalcCharObjFromSer( petTagObj.m_IntValue );
	if( ValidateObject( petCommandObj ))
	{
		petTagObj.m_Destroy = true;
		petTagObj.m_IntValue = 0;

		mChar->SetTag( "petCommandObj", petTagObj );
		Handle( mSock, mChar, petCommandObj );
	}
	else
	{
		for( auto &nearbyNpc :FindNearbyNPCs( mChar, DIST_CMDRANGE ))
		{
			if( !Handle( mSock, mChar, nearbyNpc ))
				break;
		}
	}
}

CPetMultiResponse::CPetMultiResponse( const std::string &text, bool restrictVal, TargetIds targVal,
	SI32 dictVal, bool saidAll, bool checkControlDifficulty ) : CBasePetResponse( text )
{
	isRestricted	= restrictVal;
	targId			= targVal;
	dictEntry		= dictVal;
	allSaid			= saidAll;
	checkDifficulty = checkControlDifficulty;
}
bool CPetMultiResponse::Handle( CSocket *mSock, CChar *mChar, CChar *petNpc )
{
	if( allSaid )
	{
		for( auto &nearbyNpc: FindNearbyNPCs( mChar, DIST_CMDRANGE ))
		{
			if( ValidateObject( nearbyNpc ) && nearbyNpc->GetOwnerObj() == mChar )
			{
				if( Npcs->CanControlPet( mChar, nearbyNpc, isRestricted, checkDifficulty ))
				{
					UI08 cursorType = 0;
					if( targId == 25 ) // Guard
					{
						cursorType = 2; // helpful
					}
					mSock->TempObj( nearbyNpc );
					mSock->TempInt( 1 );
					mSock->SendTargetCursor( 0, targId, cursorType, dictEntry );
					return false;
				}
			}
		}
	}
	else
	{
		std::string npcName = petNpc->GetNameRequest( mChar );
		if( npcName == "#" )
		{
			// If character name is #, use default name from dictionary files instead - using base entry 3000 + character's ID
			npcName = Dictionary->GetEntry( 3000 + petNpc->GetId() );
		}

		if( FindString( ourText, oldstrutil::upper( npcName )))
		{
			if( Npcs->CanControlPet( mChar, petNpc, isRestricted, checkDifficulty ))
			{
				mSock->TempObj( petNpc );
				UI08 cursorType = 0;
				if( targId == 25 ) // Guard
				{
					cursorType = 2; // helpful
				}
				mSock->SendTargetCursor( 0, targId, cursorType, dictEntry );
				return false;
			}
		}
	}
	return true;
}

CPetReleaseResponse::CPetReleaseResponse( const std::string &text ) : CBasePetResponse( text )
{
}
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CPetReleaseResponse::Handle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response to players wanting to release pets
//o------------------------------------------------------------------------------------------------o
bool CPetReleaseResponse::Handle( CSocket *mSock, CChar *mChar, CChar *petNpc )
{
	std::string npcName = GetNpcDictName( petNpc, mSock );
	if( FindString( ourText, oldstrutil::upper( npcName )))
	{
		if( Npcs->CanControlPet( mChar, petNpc, true, false ))
		{
			// Reduce player's control slot usage by the amount of control slots taken up by the pet
			mChar->SetControlSlotsUsed( std::max( 0, mChar->GetControlSlotsUsed() - petNpc->GetControlSlots() ));

			// Release the pet
			Npcs->ReleasePet( petNpc );

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
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CPetGuardResponse::Handle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response to players wanting their pets to guard an object
//o------------------------------------------------------------------------------------------------o
bool CPetGuardResponse::Handle( CSocket *mSock, CChar *mChar, CChar *petNpc )
{
	std::string npcName = GetNpcDictName( petNpc, mSock );
	if( saidAll || FindString( ourText, oldstrutil::upper( npcName )))
	{
		if( Npcs->CanControlPet( mChar, petNpc, false, true ))
		{
			Npcs->StopPetGuarding( petNpc );
			mSock->SysMessage( 1321 ); // Your pet is now guarding you.
			petNpc->SetNPCAiType( AI_PET_GUARD );
			petNpc->SetGuarding( mChar );
			mChar->SetGuarded( true );
			petNpc->SetFTarg( mChar );
			petNpc->SetNpcWander( WT_FOLLOW );
			if( !saidAll )
				return false;
		}
	}
	return true;
}

CPetAttackResponse::CPetAttackResponse( bool allVal, const std::string &text ) : CPetAllResponse( allVal, text )
{
}
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CPetAttackResponse::Handle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response to players wanting their pets to attack something
//o------------------------------------------------------------------------------------------------o
bool CPetAttackResponse::Handle( CSocket *mSock, CChar *mChar, CChar *petNpc )
{
	std::string npcName = GetNpcDictName( petNpc, mSock );
	if( saidAll || FindString( ourText, oldstrutil::upper( npcName )))
	{
		if( Npcs->CanControlPet( mChar, petNpc, false, true ))
		{
			Npcs->StopPetGuarding( petNpc );
			mSock->TempObj( petNpc );
			if( saidAll )
			{
				mSock->TempInt( 1 );
			}
			mSock->SendTargetCursor( 0, TARGET_ATTACK, 1, 1313 ); // Select the target to attack.
			//if( !saidAll )
				return false;
		}
	}
	return true;
}

CPetComeResponse::CPetComeResponse( bool allVal, const std::string &text ) : CPetAllResponse( allVal, text )
{
}
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CPetComeResponse::Handle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response to players wanting their pets to follow
//o------------------------------------------------------------------------------------------------o
bool CPetComeResponse::Handle( CSocket *mSock, CChar *mChar, CChar *petNpc )
{
	std::string npcName = GetNpcDictName( petNpc, mSock );
	if( saidAll || FindString( ourText, oldstrutil::upper( npcName )))
	{
		if( Npcs->CanControlPet( mChar, petNpc, false, true ))
		{
			Npcs->StopPetGuarding( petNpc );
			petNpc->SetFTarg( mChar );
			petNpc->SetNpcWander( WT_FOLLOW );
			mSock->SysMessage( 1318 ); // Your pet begins following you.
			if( !saidAll )
				return false;
		}
	}
	return true;
}

CPetStayResponse::CPetStayResponse( bool allVal, const std::string &text ) : CPetAllResponse( allVal, text )
{
}
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CPetStayResponse::Handle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response to players wanting their pets to stay
//o------------------------------------------------------------------------------------------------o
bool CPetStayResponse::Handle( CSocket *mSock, CChar *mChar, CChar *petNpc )
{
	std::string npcName = GetNpcDictName( petNpc, mSock );
	if( saidAll || FindString( ourText, oldstrutil::upper( npcName )))
	{
		if( Npcs->CanControlPet( mChar, petNpc, false, true ))
		{
			Npcs->StopPetGuarding( petNpc );
			petNpc->SetFTarg( nullptr );
			petNpc->SetTarg( nullptr );
			petNpc->SetAttacker( nullptr );
			if( petNpc->IsAtWar() )
			{
				petNpc->ToggleCombat();
			}
			petNpc->SetOldNpcWander( WT_NONE );
			petNpc->SetNpcWander( WT_NONE );
			if( mSock != nullptr )
			{
				mSock->SysMessage( 2413, npcName.c_str() ); // %s stays put.
			}
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
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseVendorResponse::Handle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response to players wanting to interact with a vendor
//o------------------------------------------------------------------------------------------------o
void CBaseVendorResponse::Handle( CSocket *mSock, CChar *mChar )
{
	for( auto &nearbyNpc: FindNearbyNPCs( mChar, DIST_INRANGE ))
	{
		if( nearbyNpc->IsShop() || nearbyNpc->GetNpcAiType() == AI_PLAYERVENDOR )
		{
			if( !LineOfSight( mSock, mChar, nearbyNpc->GetX(), nearbyNpc->GetY(), ( nearbyNpc->GetZ() + 15 ), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false ))
				continue;

			std::string npcName = GetNpcDictName( nearbyNpc, mSock );
			if( saidVendor || FindString( ourText, oldstrutil::upper( npcName )))
			{
				if( !Handle( mSock, mChar, nearbyNpc ))
					break;
			}
		}
	}
}

CVendorBuyResponse::CVendorBuyResponse( bool vendVal, const std::string &text ) : CBaseVendorResponse( vendVal, text )
{
}
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CVendorBuyResponse::Handle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response to players wanting to buy something from a vendor
//o------------------------------------------------------------------------------------------------o
bool CVendorBuyResponse::Handle( CSocket *mSock, [[maybe_unused]] CChar *mChar, CChar *vendorNpc )
{
	vendorNpc->SetTimer( tNPC_MOVETIME, BuildTimeValue( 60 ));
	if( vendorNpc->GetNpcAiType() == AI_PLAYERVENDOR )
	{
		mSock->TempObj( vendorNpc );
		vendorNpc->TextMessage( mSock, 1333, TALK, false ); // What would you like to buy?
		mSock->SendTargetCursor( 0, TARGET_PLVBUY, " " );
		return false;
	}
	else if( BuyShop( mSock, vendorNpc ))
		return false;

	return true;
}

CVendorSellResponse::CVendorSellResponse( bool vendVal, const std::string &text ) : CBaseVendorResponse( vendVal, text )
{
}
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CVendorSellResponse::Handle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response to players wanting to sell something to a vendor
//o------------------------------------------------------------------------------------------------o
bool CVendorSellResponse::Handle( CSocket *mSock, CChar *mChar, CChar *vendorNpc )
{
	//Check if vendor has onSell script running
	std::vector<UI16> scriptTriggers = vendorNpc->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			// -1 == script doesn't exist, or returned -1
			// 0 == script returned false, 0, or nothing - don't execute hard code
			// 1 == script returned true or 1
			if( toExecute->OnSell( mSock, vendorNpc ) == 0 ) //if script returns false, don't continue
			{
				return false;
			}
		}
	}

	vendorNpc->SetTimer( tNPC_MOVETIME, BuildTimeValue( 60 ));
	CPSellList toSend;
	if( toSend.CanSellItems(( *mChar ), ( *vendorNpc )))
	{
		mSock->Send( &toSend );
	}
	else
	{
		vendorNpc->TextMessage( mSock, 1341, TALK, false ); // Thou doth posses nothing of interest to me.
	}

	return false;
}

CVendorViewResponse::CVendorViewResponse( bool vendVal, const std::string &text ) : CBaseVendorResponse( vendVal, text )
{
}
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CVendorViewResponse::Handle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response to players wanting to view the items a vendor has for sale
//o------------------------------------------------------------------------------------------------o
bool CVendorViewResponse::Handle( CSocket *mSock, [[maybe_unused]] CChar *mChar, CChar *vendorNpc )
{
	if( vendorNpc->GetNpcAiType() == AI_PLAYERVENDOR )
	{
		CItem *pack	= nullptr;
		vendorNpc->TextMessage( mSock, 385, TALK, false ); // Take a look at my goods.
		pack = vendorNpc->GetPackItem();
		if( ValidateObject( pack ))
		{
			mSock->OpenPack( pack, true );
		}
	}
	if( !saidVendor )
		return false;

	return true;
}

CVendorGoldResponse::CVendorGoldResponse( bool vendVal, const std::string &text ) : CBaseVendorResponse( vendVal, text )
{
}
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CVendorGoldResponse::Handle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response to players wanting to withdraw their earnings from their vendor
//o------------------------------------------------------------------------------------------------o
bool CVendorGoldResponse::Handle( CSocket *mSock, [[maybe_unused]] CChar *mChar, CChar *vendorNpc )
{
	if( vendorNpc->GetNpcAiType() == AI_PLAYERVENDOR )
	{
		CChar *mChar = mSock->CurrcharObj();
		if( mChar == vendorNpc->GetOwnerObj() )
		{
			UI32 pay = 0;
			auto give = vendorNpc->GetHoldG();
			auto earned = vendorNpc->GetHoldG();

			if( vendorNpc->GetHoldG() <= 0 )
			{
				vendorNpc->TextMessage( mSock, 1326, TALK, false ); // I have no gold waiting for you.
				vendorNpc->SetHoldG( 0 );
			}
			else if( vendorNpc->GetHoldG() > 0 && vendorNpc->GetHoldG() <= MAX_STACK )
			{
				if( vendorNpc->GetHoldG() > 9 )
				{
					pay = static_cast<SI32>( vendorNpc->GetHoldG() / 10 );
					give = vendorNpc->GetHoldG() - pay;
				}
				else
				{
					pay = vendorNpc->GetHoldG();
					give = 0;
				}
				vendorNpc->SetHoldG( 0 );
			}
			else
			{
				UI32 t = vendorNpc->GetHoldG() - MAX_STACK;	// yank of 65 grand, then do calculations
				vendorNpc->SetHoldG( t );
				pay = 6554;
				give = 58981;
				if( t > 0 )
				{
					vendorNpc->TextMessage( mSock, 1327, TALK, false ); // You still have money left to claim.
				}
			}
			if( give )
			{
				Items->CreateScriptItem( mSock, mChar, "0x0EED", give, OT_ITEM, true );
			}

			// Today's purchases total %i gold. I am keeping %i gold for my self. Here is the remaining %i gold. Have a nice day.
			vendorNpc->TextMessage( mSock, 1328, TALK, 0, earned, pay, give );
		}
		else
		{
			vendorNpc->TextMessage( mSock, 1329, TALK, false ); // I don't work for you!
		}
	}
	if( !saidVendor )
		return false;

	return true;
}

CVendorStatusResponse::CVendorStatusResponse( bool vendVal, const std::string &text ) : CBaseVendorResponse( vendVal, text )
{
}
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CVendorStatusResponse::Handle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response to players wanting to see the status of their vendor
//o------------------------------------------------------------------------------------------------o
bool CVendorStatusResponse::Handle( CSocket *mSock, [[maybe_unused]] CChar *mChar, CChar *vendorNpc )
{
	if( vendorNpc->GetNpcAiType() == AI_PLAYERVENDOR )
	{
		CChar *mChar = mSock->CurrcharObj();
		if( mChar == vendorNpc->GetOwnerObj() )
		{
			if( vendorNpc->GetHoldG() <= 0 )
			{
				vendorNpc->TextMessage( mSock, 1326, TALK, false ); // I have no gold waiting for you.
			}
			else
			{
				UI32 pay = 0;
				if( vendorNpc->GetHoldG() > 9 )
				{
					pay = static_cast<SI32>( vendorNpc->GetHoldG() / 10 );
				}
				else
				{
					pay = vendorNpc->GetHoldG();
				}
				vendorNpc->TextMessage( mSock, 1782, TALK, 0, vendorNpc->GetHoldG(), pay ); // Today's purchases total %i gold. I am keeping %i gold for my self.
			}
		}
		else
		{
			vendorNpc->TextMessage( mSock, 1329, TALK, false ); // I don't work for you!
		}
	}
	if( !saidVendor )
		return false;

	return true;
}

CVendorDismissResponse::CVendorDismissResponse( bool vendVal, const std::string &text ) : CBaseVendorResponse( vendVal, text )
{
}
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CVendorDismissResponse::Handle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response to players wanting to dismiss their vendor
//o------------------------------------------------------------------------------------------------o
bool CVendorDismissResponse::Handle( CSocket *mSock, [[maybe_unused]] CChar *mChar, CChar *vendorNpc )
{
	if( vendorNpc->GetNpcAiType() == AI_PLAYERVENDOR )
	{
		CChar *mChar = mSock->CurrcharObj();
		if( mChar == vendorNpc->GetOwnerObj() )
		{
			vendorNpc->Delete();
		}
		else
		{
			vendorNpc->TextMessage( mSock, 1329, TALK, false ); // I don't work for you!
		}
	}
	if( !saidVendor )
		return false;

	return true;
}

CHouseMultiResponse::CHouseMultiResponse( TargetIds targVal, SI32 dictVal )
{
	targId			= targVal;
	dictEntry		= dictVal;
}
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CHouseMultiResponse::Handle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response to players wanting to use house commands
//o------------------------------------------------------------------------------------------------o
void CHouseMultiResponse::Handle( CSocket *mSock, CChar *mChar )
{
	CMultiObj *realHouse = FindMulti( mChar );
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
					if( toExecute->OnHouseCommand( mSock, realHouse, targId ) == 1 ) //if script returns true, don't continue
					{
						return;
					}
				}
			}

			if( realHouse->IsOwner( mChar ))
			{
				mSock->TempObj( realHouse );
				mSock->SendTargetCursor( 0, targId, 0, dictEntry );
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
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBoatMultiResponse::Handle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response to players wanting to use boat commands
//o------------------------------------------------------------------------------------------------o
void CBoatMultiResponse::Handle( CSocket *mSock, CChar *mChar )
{
	// Let's do some spam-prevention here
	if( mChar->GetTimer( tCHAR_ANTISPAM ) > cwmWorldState->GetUICurrentTime() )
	{
		return;
	}
	else
	{
		mChar->SetTimer( tCHAR_ANTISPAM, BuildTimeValue( static_cast<R32>( cwmWorldState->ServerData()->CheckBoatSpeed() )));
	}

	CBoatObj *boat = GetBoat( mSock );
	if( !ValidateObject( boat ))
	{
		return;
	}

	CItem *tiller = CalcItemObjFromSer( boat->GetTiller() );
	if( boat->GetMoveType() == BOAT_ANCHORED )
	{
		if( ValidateObject( tiller ))
		{
			tiller->TextMessage( mSock, 2024 ); // Ar, the anchor is down sir!
			mSock->SysMessage( 2023 ); // You must raise the anchor to pilot the ship.
		}
		return;
	}

	boat->SetMoveType( moveType );

	if( ValidateObject( tiller ))
	{
		tiller->TextMessage( mSock, 10 ); // Aye, sir.
	}
}
