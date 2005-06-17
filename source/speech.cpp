#include "uox3.h"
#include "speech.h"
#include "cVersionClass.h"
#include "cGuild.h"
#include "cRaces.h"
#include "commands.h"
#include "skills.h"
#include "CJSMapping.h"
#include "cScript.h"
#include "cEffects.h"
#include "CPacketSend.h"
#include "regions.h"
#include "CResponse.h"

namespace UOX
{

CSpeechQueue *SpeechSys;

std::map< std::string, UnicodeTypes > codeLookup;

void InitializeLookup( void )
{
	for( int i = (int)ZERO; i < (int)TOTAL_LANGUAGES; ++i )
		codeLookup[LanguageCodes[(UnicodeTypes)i]] = (UnicodeTypes)i;
}

UnicodeTypes FindLanguage( const char *lang )
{
	std::map< std::string, UnicodeTypes >::const_iterator p = codeLookup.find( lang );
	if( p != codeLookup.end() )
		return p->second;
	else
		return TOTAL_LANGUAGES;
}
void UpdateLanguage( CSocket *s )
{
	if( s == NULL )
		return;
	char langCode[4];

	langCode[0] = s->GetByte( 8 );
	langCode[1] = s->GetByte( 9 );
	langCode[2] = s->GetByte( 10 );
	langCode[3] = 0;

	UString ulangCode = langCode ;
	ulangCode = ulangCode.upper();
	
	UnicodeTypes cLang = s->Language();
	if( LanguageCodes[cLang] != ulangCode.c_str() )
	{
		UnicodeTypes newLang = FindLanguage( ulangCode.c_str() );
		if( newLang == TOTAL_LANGUAGES )
			Console.Error( 0, "Unknown language type \"%s\".  PLEASE report this on www.sourceforge.net/projects/uox3 in the bugtracker!", ulangCode.c_str() );
		else
			s->Language( newLang );
	} 
}

//o-------------------------------------------------------------------------
//|	Function	-	sysBroadcast( std::string txt )
//|	Date		-	Unknown
//|	Programmer	-	UOX DevTeam
//|	Modification-	EviLDeD(February 27, 2000)
//o-------------------------------------------------------------------------
//|	Purpose			-	This function was adapted to be used with the new code
//|						in the console thread that allows text to be entered on
//|						the console and it be shipped out to all logged in players.
//o-------------------------------------------------------------------------
void sysBroadcast( const std::string txt )
{
	if( !txt.empty() )
	{
		CSpeechEntry *toAdd = SpeechSys->Add();
		toAdd->Speech( txt );
		toAdd->Font( FNT_NORMAL );
		toAdd->Speaker( INVALIDSERIAL );
		toAdd->SpokenTo( INVALIDSERIAL );
		toAdd->Type( SYSTEM );
		toAdd->At( cwmWorldState->GetUICurrentTime() );
		toAdd->TargType( SPTRG_BROADCASTPC );
		toAdd->Colour( 0x084D );
		toAdd->Font( FNT_BOLD );
		toAdd->SpeakerName( "System: " );
	}
}

void broadcast( CSocket *mSock ) // GM Broadcast (Done if a GM yells something)
{
	CChar *mChar = mSock->CurrcharObj();
	CPSpeech toSend( *mChar );
	toSend.Type( 1 );
	toSend.GrabSpeech( mSock, mChar );

	if( !mChar->isUnicode() )
		toSend.Message( (char *)&(mSock->Buffer()[8]) );
	else
	{
		char nonuni[512]; 
		for( UI16 i = 13; i < mSock->GetWord( 1 ); i += 2 )
			nonuni[(i-13)/2] = mSock->GetByte( i );
		toSend.Message( &nonuni[0] );
	}

	Network->PushConn();
	for( CSocket *bSock = Network->FirstSocket(); !Network->FinishedSockets(); bSock = Network->NextSocket() )
	{
		bSock->Send( &toSend );
	}
	Network->PopConn();
}

void WhichResponse( CSocket *mSock, CChar *mChar, std::string text );
bool response( CSocket *mSock, CChar *mChar, std::string text )
{
	bool retVal					= false;
	REGIONLIST nearbyRegions	= MapRegion->PopulateList( mChar );
	for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
	{
		SubRegion *CellResponse = (*rIter);
		if( CellResponse == NULL )
			return false;

		CDataList< CChar * > *regChars = CellResponse->GetCharList();
		regChars->Push();
		for( CChar *Npc = regChars->First(); !regChars->Finished(); Npc = regChars->Next() )
		{
			if( !ValidateObject( Npc ) || Npc == mChar )
				continue;
			UI16 ourDist	= getDist( mChar, Npc );
			SI08 diffZ		= mChar->GetZ() - Npc->GetZ();
			if( ourDist > 30 || abs( diffZ ) > 30 )
				continue;
			if( ourDist <= 5 && abs( diffZ ) <= 9 )
			{
				UI16 speechTrig = Npc->GetScriptTrigger();
				cScript *toExecute = JSMapping->GetScript( speechTrig );
				if( toExecute != NULL )
				{
//|					-1	=> No such function or bad call
//|					0	=> Let other NPCs and PCs see it
//|					1	=> Let other PCs see it
//|					2	=> Let no one else see it
					SI08 rVal = -1;
					if( Npc->isDisabled() )
						Npc->talkAll( 1291, false );
					else if( !mChar->IsDead() )
						rVal = toExecute->OnSpeech( text.c_str(), mChar, Npc );
					switch( rVal )
					{
						case 1:		// No other NPCs to see it, but PCs should
							regChars->Pop();
							return false;
						case 2:		// no one else to see it
							regChars->Pop();
							return true;
						case 0:		// Other NPCs and PCs to see it
						case -1:	// no function, so do nothing... NOT handled!
						default:
							break;
					}
				}
			}
		}
		regChars->Pop();
	}
	WhichResponse( mSock, mChar, text );
	return retVal;
}
/*
Unicode speech format
byte=char, short=char[2], int=char[4], wchar=char[2]=unicode character

  Message Sent By Client:
  0xAD - Unicode Speech Request
  UI08 cmd (0xAD)
  short msgsize 1,2
  byte type (0=say, 2=emote, 8=whisper, 9=yell) 3
  short color 4,5
  short font 6,7
  UI08[4] lang (null terminated, "enu " for US english.) 8,9,10,11
  wchar[?] text (null terminated, ?=(msgsize-12)/2) 13
  
	Message Sent By Server:
	Unicode Speech message(Variable # of bytes) 
·	BYTE cmd											0
·	BYTE[2] blockSize									1-2
·	BYTE[4] ID											3-6
·	BYTE[2] Model										7-8
·	BYTE Type											9
·	BYTE[2] Color										10-11
·	BYTE[2] Font										12-13
·	BYTE[4] Language									14-17 (3 byte unicode language, mirrored from client)
·	BYTE[30] Name										18-47
·	BYTE[?][2] Msg - Null Terminated (blockSize - 48)	48+?
*/

bool CPITalkRequest::Handle( void )
{
	if( HandleCommon() )
		return true;
	CChar *mChar	= tSock->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return true;

	char *asciiText		= Text();
	cScript *myScript	= JSMapping->GetScript( mChar->GetScriptTrigger() );
	if( myScript != NULL )
	{
		if( !myScript->OnTalk( mChar, asciiText ) )
			return true;
	}
	
	if( ( asciiText[0] == cwmWorldState->ServerData()->ServerCommandPrefix() ) || ( ( asciiText[0] == '.' ) && ( asciiText[1] != '.' ) ) ) 
		Commands->Command( tSock, mChar, &asciiText[1] );
	else
	{
		if( mChar->GetVisible() == VT_TEMPHIDDEN || mChar->GetVisible() == VT_INVISIBLE )
			mChar->ExposeToView();
		
		if( Type() == 0x09 && mChar->CanBroadcast() )
			broadcast( tSock );
		else
		{
			UString text( asciiText );
			text = text.upper();

			if( Type() == 0 )
				mChar->SetSayColour( TextColour() );
			if( Type() == 2 )
				mChar->SetEmoteColour( TextColour() );
			if( cwmWorldState->ServerData()->ServerConsoleLogStatus() == 2 ) //Logging -- Zippy
			{
				char temp[1024], temp2[512];
				sprintf( temp2, "%s.log", mChar->GetName().c_str() );
				sprintf( temp, "%s [%x %x %x %x] [%i]: %s\n", mChar->GetName().c_str(), mChar->GetSerial( 1 ), mChar->GetSerial( 2 ), mChar->GetSerial( 3 ), mChar->GetSerial( 4 ), mChar->GetAccount().wAccountIndex, asciiText );
				Console.Log( temp, temp2 );
			}

			for( UI16 trigWord = tSock->FirstTrigWord(); !tSock->FinishedTrigWords(); trigWord = tSock->NextTrigWord() )
			{
				if( trigWord == TW_RESIGN )
				{
					GuildSys->Resign( tSock );
					break;
				}
			}
			if( text.find( "DEVTEAM033070" ) != std::string::npos )
			{
				std::string temp3 = "RBuild: " + CVersionClass::GetRealBuild() + " PBuild: " + CVersionClass::GetBuild() + " --> Version: " + CVersionClass::GetVersion();
				tSock->sysmessage( temp3.c_str() );
			}
			else if( !mChar->IsDead() )	// this makes it so npcs do not respond to dead people
			{
				if( response( tSock, mChar, text ) )
					return true;
			}

			cPUOXBuffer *txtToSend			= NULL;
			cPUOXBuffer *ghostedText		= NULL;
			CPUnicodeSpeech *uniTxtToSend	= NULL;
			CPUnicodeSpeech *uniGhostedText = NULL;
			CPSpeech *asciiTxtToSend		= NULL;
			CPSpeech *asciiGhostedText		= NULL;

			if( IsUnicode() )
			{
				uniTxtToSend = new CPUnicodeSpeech();
				uniTxtToSend->Object( *((CPITalkRequestUnicode *)this) );
				uniTxtToSend->Object( *mChar );
				txtToSend = uniTxtToSend;

				uniGhostedText = new CPUnicodeSpeech();
				(*uniGhostedText) = (*uniTxtToSend);
				uniGhostedText->GhostIt( 0 );

				ghostedText = uniGhostedText;
			}
			else
			{
				asciiTxtToSend = new CPSpeech();
				asciiTxtToSend->Object( *((CPITalkRequestAscii *)this) );
				asciiTxtToSend->Object( *mChar );
				txtToSend = asciiTxtToSend;

				asciiGhostedText = new CPSpeech();
				(*asciiGhostedText) = (*asciiTxtToSend);
				asciiGhostedText->GhostIt( 0 );

				ghostedText = asciiGhostedText;
			}
			tSock->Send( txtToSend );

			SOCKLIST nearbyChars = FindNearbyPlayers( mChar );
			for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
			{
				CSocket *tSock	= (*cIter);
				CChar *tChar	= tSock->CurrcharObj();
				if( mChar != tChar )
				{
					if( !tChar->IsGM() && !tChar->IsCounselor() && tSock->GetTimer( tPC_SPIRITSPEAK ) == 0 )// GM/Counselors can see ghosts talking always Seers?
					{
						if( mChar->IsDead() && !tChar->IsDead() )  // Ghost can talk normally to other ghosts
							tSock->Send( ghostedText );
						else
							tSock->Send( txtToSend );
					}
					else if( tChar->GetRace() != mChar->GetRace() && !tChar->IsGM() )
					{
						if( Skills->CheckSkill( tChar, SPIRITSPEAK, Races->LanguageMin( mChar->GetRace() ), 1000 ) != 1 )
							tSock->Send( ghostedText );
						else
							tSock->Send( txtToSend );
					}
					else
						tSock->Send( txtToSend );
				}
			}

			delete uniTxtToSend;
			delete asciiTxtToSend;
			delete uniGhostedText;
			delete asciiGhostedText;
		}
	}
	return true;
}


CSpeechQueue::CSpeechQueue( void ) : pollTime( 100 ), runAsThread( false )
{
	speechList.resize( 0 );
	InitializeLookup();
}
CSpeechQueue::~CSpeechQueue()
{
	speechList.clear();
}

void CSpeechQueue::SayIt( SPEECHLIST_ITERATOR toSay )
{
	CPacketSpeech toSend	= (*toSay);
	CChar *thisChar			= NULL;
	CItem *thisItem			= NULL;
	CBaseObject *thisObj	= NULL;
	if( toSay->Speaker() > BASEITEMSERIAL )
	{
		thisItem	= calcItemObjFromSer( toSay->Speaker() );
		thisObj		= thisItem;
	}
	else
	{
		thisChar	= calcCharObjFromSer( toSay->Speaker() );
		thisObj		= thisChar;
	}
	CChar *sChar	= NULL;
	CSocket *mSock	= NULL;
	switch( toSay->TargType() )
	{
		case SPTRG_INDIVIDUAL:		// aimed at individual person
			sChar = calcCharObjFromSer( toSay->SpokenTo() );
			if( ValidateObject( sChar ) )
			{
				mSock = calcSocketObjFromChar( sChar );
				if( mSock != NULL )
					mSock->Send( &toSend );
			}
			break;
		case SPTRG_PCNPC:				// all NPCs and PCs in range
		case SPTRG_PCS:					// all PCs in range
		{
			if( !ValidateObject( thisObj ) )
				break;
			if( ValidateObject( thisItem ) && thisItem->GetCont() != NULL )	// not on ground, can't guarantee speech
				break;
			SOCKLIST nearbyChars = FindPlayersInVisrange( thisObj );
			for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
			{
				mSock = (*cIter);
				CChar *mChar = mSock->CurrcharObj();
				if( ValidateObject( mChar ) )
				{
					if( mChar->GetCommandLevel() >= toSay->CmdLevel() )
						mSock->Send( &toSend );
				}
			}
			break;
		}
		case SPTRG_BROADCASTPC:			// ALL PCs everywhere + NPCs in range
		case SPTRG_BROADCASTALL:
			Network->PushConn();
			for( mSock = Network->FirstSocket(); !Network->FinishedSockets(); mSock = Network->NextSocket() )
			{
				if( mSock != NULL )
				{
					CChar *mChar = mSock->CurrcharObj();
					if( ValidateObject( mChar ) )
					{
						if( mChar->GetCommandLevel() >= toSay->CmdLevel() )
							mSock->Send( &toSend );
					}
				}
			}
			Network->PopConn();
			break;
	};
}

//okay.. we cannot use iterators for this, because any time something is erased from the vector, the iterator will no onger be valid... which could cause crashes and will cause weird things to happen.
bool CSpeechQueue::InternalPoll( void )		// Send out any pending speech, returning true if entries were sent
{
	bool retVal = false;
	SPEECHLIST toKeep;

	// Iterators work just great, just make sure to test them for valididty, and stop using []access vectors wont sort.
	SPEECHLIST_ITERATOR I;
	for( I = speechList.begin(); I != speechList.end(); ++I )
	{
		CSpeechEntry tempSpeechList = (*I);
		if( tempSpeechList.At() == -1 || static_cast<UI32>(tempSpeechList.At()) <= cwmWorldState->GetUICurrentTime() )
		{
			retVal = true;
			// 1/13/2003 - Maarc - Quick fix for more strict gcc 3.2 compliance.
			SayIt( (SPEECHLIST_ITERATOR) (&tempSpeechList) );
		}
		else	// We want to keep the entries that have been unsaid
			toKeep.push_back( tempSpeechList );
	}
	speechList.assign( toKeep.begin(), toKeep.end() );
	toKeep.clear();
	return retVal;
}

bool CSpeechQueue::Poll( void )
{
	if( RunAsThread() )
	{
		while( RunAsThread() )
		{
			InternalPoll();
			UOXSleep( pollTime );	// so that it's never 0 (and number of milliseconds)
		}
		return true;
	}
	else
		return InternalPoll();
}

CSpeechEntry * CSpeechQueue::Add( void )		// Make space in queue, and return pointer to new entry
{
	size_t iSize = speechList.size();
	speechList.resize( iSize + 1 );
	return &speechList[iSize];
}

int CSpeechQueue::PollTime( void ) const
{
	return pollTime;
}

void CSpeechQueue::PollTime( int value )
{
	pollTime = value;
}

bool CSpeechQueue::RunAsThread( void ) const
{
	return runAsThread;
}

void CSpeechQueue::RunAsThread( bool value )
{
	runAsThread = value;
}

void CSpeechQueue::DumpInFile( void )
{
	std::string speechFile	= cwmWorldState->ServerData()->Directory( CSDDP_LOGS ) + "speechdump.txt";
	std::ofstream speechDestination( speechFile.c_str() );
	if( !speechDestination ) 
	{
		Console.Error( 1, "Failed to open %s for writing", speechFile.c_str() );
		return;
	}
	SPEECHLIST_ITERATOR toWrite;
	for( toWrite = speechList.begin(); toWrite != speechList.end(); ++toWrite )
	{
		speechDestination << "Time: " << toWrite->At() << std::endl;
		speechDestination << "nColour: " << toWrite->Colour() << std::endl;
		speechDestination << "nFont: " << toWrite->Font()<< std::endl;
		speechDestination << "nLanguage: " << toWrite->Language() << std::endl;
		speechDestination << "nSpeech: " << toWrite->Speech() << std::endl;
		speechDestination << "nSpeaker: " << toWrite->SpeakerName() << std::endl << std::endl;
	}
	speechDestination.close();
}


}
