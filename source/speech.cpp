#include "uox3.h"
#include "speech.h"
#include "cVersionClass.h"
#include "cRaces.h"
#include "commands.h"
#include "skills.h"
#include "CJSMapping.h"
#include "cScript.h"
#include "cEffects.h"
#include "CPacketSend.h"
#include "CResponse.h"
#include "movement.h"
#include "StringUtility.hpp"

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void ClilocMessage( CSocket *mSock, UI08 type, UI16 hue, UI16 font, UI32 messageNum, const char *types = "", ... )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends a cliloc message to the client, which is displayed as a system message
//o-----------------------------------------------------------------------------------------------o
void ClilocMessage( CSocket *mSock, UI08 type, UI16 hue, UI16 font, UI32 messageNum, const char *types = "", ... )
{
	bool multipleArgs		= false;
	std::string argList		= "";
	std::string stringVal	= "";
	const char *typesPtr	= types;

	va_list marker;
	va_start( marker, types );
	while( *typesPtr != '\0' )
	{
		if( *typesPtr == 'i' ){
			stringVal = strutil::number( va_arg( marker, SI32 ) );
		}
		else if( *typesPtr == 's' ){
			stringVal = va_arg( marker, char * );
		}

		if( !stringVal.empty() )
		{
			if( !multipleArgs )
			{
				multipleArgs = true;
				argList = stringVal;
			}
			else
				argList += strutil::format( "\t%s", stringVal.c_str() );
		}
		++typesPtr;
	}
	va_end( marker );

	CPClilocMessage toSend;
	toSend.Type( type );
	toSend.Hue( hue );
	toSend.Font( font );
	toSend.Message( messageNum );
	toSend.ArgumentString( argList );

	mSock->Send( &toSend );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void ClilocMessage( CSocket *mSock, CBaseObject *srcObj, UI08 type, UI16 hue, UI16 font, UI32 messageNum, bool sendAll, const char *types = "", ... )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends a clilocmessage to the client, which will be displayed as if said by srcObj
//o-----------------------------------------------------------------------------------------------o
void ClilocMessage( CSocket *mSock, CBaseObject *srcObj, UI08 type, UI16 hue, UI16 font, UI32 messageNum, bool sendAll, const char *types = "", ... )
{
	bool multipleArgs		= false;
	std::string argList		= "";
	std::string stringVal	= "";
	const char *typesPtr	= types;

	va_list marker;
	va_start( marker, types );
	while( *typesPtr != '\0' )
	{
		if( *typesPtr == 'i' )
			stringVal = strutil::number( va_arg( marker, SI32 ) );
		else if( *typesPtr == 's' )
			stringVal = va_arg( marker, char * );

		if( !stringVal.empty() )
		{
			if( !multipleArgs )
			{
				multipleArgs = true;
				argList = stringVal;
			}
			else
				argList += strutil::format( "\t%s", stringVal.c_str() );
		}
		++typesPtr;
	}
	va_end( marker );

	CPClilocMessage toSend( (*srcObj) );
	toSend.Type( type );
	toSend.Hue( hue );
	toSend.Font( font );
	toSend.Message( messageNum );
	toSend.ArgumentString( argList );

	bool sendSock = (mSock != nullptr);
	if( sendAll )
	{
		SOCKLIST nearbyChars = FindNearbyPlayers( srcObj, DIST_INRANGE );
		for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
		{
			if( sendSock && (*cIter) == mSock )
				sendSock = false;
			(*cIter)->Send( &toSend );
		}
	}

	if( sendSock )
		mSock->Send( &toSend );
}

CSpeechQueue *SpeechSys;

std::map< std::string, UnicodeTypes > codeLookup;

void InitializeLookup( void )
{
	for( SI32 i = (SI32)ZERO; i < (SI32)TOTAL_LANGUAGES; ++i )
		codeLookup[LanguageCodes[(UnicodeTypes)i]] = (UnicodeTypes)i;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UnicodeTypes FindLanguage( CSocket *s, UI16 offset )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Attempt to find language used by client
//o-----------------------------------------------------------------------------------------------o
UnicodeTypes FindLanguage( CSocket *s, UI16 offset )
{
	if( s == nullptr )
		return ZERO;

	char langCode[4];
	langCode[0] = s->GetByte( offset );
	langCode[1] = s->GetByte( ++offset );
	langCode[2] = s->GetByte( ++offset );
	langCode[3] = 0;

	std::string ulangCode = langCode;
	ulangCode = strutil::toupper( ulangCode );

	UnicodeTypes cLang = s->Language();
	if( LanguageCodes[cLang] != ulangCode.c_str() )
	{
		std::map< std::string, UnicodeTypes >::const_iterator p = codeLookup.find( ulangCode );
		if( p != codeLookup.end() )
			return p->second;
		else
			Console.error( strutil::format("Unknown language type \"%s\".  PLEASE report this in the Bugs section of the forums at https://www.uox3.org!", ulangCode.c_str()) );
	}
	return cLang;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void sysBroadcast( const std::string& txt )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	This function was adapted to be used with the new code
//|						in the console thread that allows text to be entered on
//|						the console and it be shipped out to all logged in players.
//|
//|	Changes		-	  (February 27, 2000)
//o-----------------------------------------------------------------------------------------------o
void sysBroadcast( const std::string& txt )
{
	if( !txt.empty() )
	{
		CSpeechEntry& toAdd = SpeechSys->Add();
		toAdd.Speech( txt );
		toAdd.Font( FNT_NORMAL );
		toAdd.Speaker( INVALIDSERIAL );
		toAdd.SpokenTo( INVALIDSERIAL );
		toAdd.Type( SYSTEM );
		toAdd.At( cwmWorldState->GetUICurrentTime() );
		toAdd.TargType( SPTRG_BROADCASTPC );
		toAdd.Colour( 0x084D );
		toAdd.Font( FNT_BOLD );
		toAdd.SpeakerName( "System: " );
	}
}

bool WhichResponse( CSocket *mSock, CChar *mChar, std::string text );
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CPITalkRequest::Handle( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles speech requests from client
//|
//|	Notes		-	Unicode speech format
//|					byte=char, short=char[2], int=char[4], wchar=char[2]=unicode character
//|
//|					Message Sent By Client:
//|					0xAD - Unicode Speech Request
//|					UI08 cmd (0xAD)
//|					short msgsize 1,2
//|					byte type (0=say, 2=emote, 8=whisper, 9=yell) 3
//|					short color 4,5
//|					short font 6,7
//|					UI08[4] lang (null terminated, "enu " for US english.) 8,9,10,11
//|					wchar[?] text (null terminated, ?=(msgsize-12)/2) 13
//|
//|					Message Sent By Server:
//|					Unicode Speech message(Variable # of bytes)
//|					·	BYTE cmd											0
//|					·	BYTE[2] blockSize									1-2
//|					·	BYTE[4] ID											3-6
//|					·	BYTE[2] Model										7-8
//|					·	BYTE Type											9
//|					·	BYTE[2] Color										10-11
//|					·	BYTE[2] Font										12-13
//|					·	BYTE[4] Language									14-17 (3 byte unicode language, mirrored from client)
//|					·	BYTE[30] Name										18-47
//|					·	BYTE[?][2] Msg - Null Terminated (blockSize - 48)	48+?
//o-----------------------------------------------------------------------------------------------o
bool CPITalkRequest::Handle( void )
{
	if( HandleCommon() )
		return true;
	CChar *mChar	= tSock->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return true;

	char *asciiText		= Text();

	std::vector<UI16> scriptTriggers = mChar->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			// If script returned false/0, prevent hard-code (and other scripts with event) from running
			if( toExecute->OnTalk( mChar, asciiText ) == 0 )
			{
				return true;
			}
		}
	}

	if( ( asciiText[0] == cwmWorldState->ServerData()->ServerCommandPrefix() ) || ( ( asciiText[0] == '.' ) && ( asciiText[1] != '.' ) ) )
		Commands->Command( tSock, mChar, &asciiText[1] );
	else
	{
		if( mChar->IsDead() )
		{
			if( !mChar->IsAtWar() )
			{
				mChar->SetWar( true );
				CPWarMode wMode( 1 );
				tSock->Send( &wMode );
			}
		}

		if( Type() != WHISPER && ( mChar->GetVisible() == VT_TEMPHIDDEN || mChar->GetVisible() == VT_INVISIBLE ))
			mChar->ExposeToView();

		if( ( Type() == YELL || Type() == ASCIIYELL ) && mChar->CanBroadcast() )
		{
			CSpeechEntry& toAdd = SpeechSys->Add();
			toAdd.Speech( asciiText );
			toAdd.Font( (FontType)mChar->GetFontType() );
			toAdd.Speaker( mChar->GetSerial() );
			toAdd.SpokenTo( INVALIDSERIAL );
			toAdd.Type( PROMPT );
			toAdd.At( cwmWorldState->GetUICurrentTime() );
			toAdd.TargType( SPTRG_BROADCASTPC );
			if( mChar->GetSayColour() == 0x1700 )
				toAdd.Colour( 0x5A );
			else if( mChar->GetSayColour() == 0x0 )
				toAdd.Colour( 0x5A );
			else
				toAdd.Colour( mChar->GetSayColour() );
			toAdd.Font( FNT_BOLD );
			toAdd.SpeakerName( mChar->GetName() );
		}
		else
		{
			std::string text( asciiText );

			if( Type() == 0 )
			{
				mChar->SetSayColour( TextColour() );
			}
			if( Type() == 2 )
			{
				mChar->SetEmoteColour( TextColour() );
			}
			if( cwmWorldState->ServerData()->ServerSpeechLog() ) //Logging
			{
				auto temp = strutil::format("%s.log", mChar->GetName().c_str() );
				auto temp2 = strutil::format("%s [%x %x %x %x] [%i]: %s\n", mChar->GetName().c_str(), mChar->GetSerial( 1 ), mChar->GetSerial( 2 ), mChar->GetSerial( 3 ), mChar->GetSerial( 4 ), mChar->GetAccount().wAccountIndex, asciiText );
				Console.log( temp, temp2 );
			}

			std::string upperText = strutil::toupper( text );
			if( upperText.find( "DEVTEAM033070" ) != std::string::npos )
			{
				std::string temp3 = "RBuild: " + CVersionClass::GetRealBuild() + " PBuild: " + CVersionClass::GetBuild() + " --> Version: " + CVersionClass::GetVersion();
				tSock->sysmessage( temp3.c_str() );
			}

			if( !WhichResponse( tSock, mChar, text ) )
				return true;

			CPUOXBuffer *txtToSend				= nullptr;
			CPUOXBuffer *ghostedText			= nullptr;
			CPUnicodeSpeech *uniTxtToSend		= nullptr;
			CPUnicodeSpeech *uniGhostedText		= nullptr;
			CPacketSpeech *asciiTxtToSend		= nullptr;
			CPacketSpeech *asciiGhostedText		= nullptr;

			if( IsUnicode() )
			{
				uniTxtToSend = new CPUnicodeSpeech();
				uniTxtToSend->Object( *(static_cast<CPITalkRequestUnicode *>(this)) );
				uniTxtToSend->Object( *mChar );
				txtToSend = uniTxtToSend;

				uniGhostedText = new CPUnicodeSpeech();
				(*uniGhostedText) = (*uniTxtToSend);
				uniGhostedText->GhostIt( 0 );

				ghostedText = uniGhostedText;
			}
			else
			{
				asciiTxtToSend = new CPacketSpeech( *(static_cast<CPITalkRequestAscii *>(this)) );
				asciiTxtToSend->SpeakerSerial( mChar->GetSerial() );
				asciiTxtToSend->SpeakerModel( mChar->GetID() );
				asciiTxtToSend->SpeakerName( mChar->GetName() );
				txtToSend = asciiTxtToSend;

				asciiGhostedText = new CPacketSpeech();
				(*asciiGhostedText) = (*asciiTxtToSend);
				asciiGhostedText->GhostIt( 0 );

				ghostedText = asciiGhostedText;
			}
			tSock->Send( txtToSend );

			SOCKLIST nearbyChars;
			// Distance at which other players can hear the speech depends on speech-type
			if( Type() == WHISPER && !mChar->IsGM() && !mChar->IsCounselor() )
				nearbyChars = FindNearbyPlayers( mChar, 1 );
			else if( Type() == WHISPER && ( mChar->IsGM() || mChar->IsCounselor() ))
				nearbyChars = FindNearbyPlayers( mChar, 3 );
			else if( Type() == YELL )
				nearbyChars = FindNearbyPlayers( mChar, ( DIST_SAMESCREEN * 1.5 ));
			else
				nearbyChars = FindNearbyPlayers( mChar );

			for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
			{
				CSocket *tSock	= (*cIter);
				CChar *tChar	= tSock->CurrcharObj();
				if( mChar != tChar )
				{
					// Line of Sight check!
					if( !tChar->IsGM() && !LineOfSight( mChar->GetSocket(), tChar, mChar->GetX(), mChar->GetY(), mChar->GetZ() + 15, WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false ) )
						continue;
					if( mChar->IsDead() && tChar->GetCommandLevel() < CL_CNS && tSock->GetTimer( tPC_SPIRITSPEAK ) == 0 )// GM/Counselors can see ghosts talking always Seers?
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
					{
						if( mChar->GetVisible() == VT_TEMPHIDDEN || mChar->GetVisible() == VT_INVISIBLE || mChar->GetVisible() == VT_PERMHIDDEN )
						{
							if(( tChar->IsGM() || tChar->IsCounselor() ) || Type() == WHISPER )
							{
								tSock->sysmessage( 1794, mChar->GetName().c_str() );
								tSock->Send( txtToSend );
							}
						}
						else
						{
							if( Type() == YELL && !objInRange( tChar, mChar, DIST_SAMESCREEN ) )
							{
								tSock->sysmessage( 1795, mChar->GetName().c_str() );
								tSock->Send( txtToSend );
							}
							else
								tSock->Send( txtToSend );
						}
					}
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

CSpeechQueue::CSpeechQueue() : pollTime( 100 ), runAsThread( false )
{
	speechList.resize( 0 );
	InitializeLookup();
}
CSpeechQueue::~CSpeechQueue()
{
	for( SPEECHLIST_ITERATOR slIter = speechList.begin(); slIter != speechList.end(); ++slIter )
	{
		delete (*slIter);
		(*slIter) = nullptr;
	}
	speechList.clear();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SayIt( CSpeechEntry& toSay )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends out specified speech entry to users
//o-----------------------------------------------------------------------------------------------o
void CSpeechQueue::SayIt( CSpeechEntry& toSay )
{
	CPacketSpeech toSend	= toSay;
	CChar *thisChar			= nullptr;
	CItem *thisItem			= nullptr;
	CBaseObject *thisObj	= nullptr;
	CSocket *thisSock		= nullptr;
	if( toSay.Speaker() > BASEITEMSERIAL )
	{
		thisItem	= calcItemObjFromSer( toSay.Speaker() );
		thisObj		= thisItem;
	}
	else
	{
		thisChar	= calcCharObjFromSer( toSay.Speaker() );
		thisObj		= thisChar;
	}
	CChar *sChar	= nullptr;
	CSocket *mSock	= nullptr;
	switch( toSay.TargType() )
	{
		case SPTRG_INDIVIDUAL:		// aimed at individual person
			if( ValidateObject( thisChar ) )
			{
				thisSock = thisChar->GetSocket();
				if( thisSock != nullptr )
					thisSock->Send( &toSend );
			}
			sChar = calcCharObjFromSer( toSay.SpokenTo() );
			if( ValidateObject( sChar ) && sChar != thisChar )
			{
				mSock = sChar->GetSocket();
				if( mSock != nullptr )
					mSock->Send( &toSend );
			}
			break;
		case SPTRG_PCNPC:				// all NPCs and PCs in range
		case SPTRG_PCS:					// all PCs in range
		{
			if( !ValidateObject( thisObj ) )
				break;
			if( ValidateObject( thisItem ) && thisItem->GetCont() != nullptr )	// not on ground, can't guarantee speech
				break;
			SOCKLIST nearbyChars = FindPlayersInVisrange( thisObj );
			for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
			{
				mSock = (*cIter);
				CChar *mChar = mSock->CurrcharObj();
				if( ValidateObject( mChar ) )
				{
					if( mChar->GetCommandLevel() >= toSay.CmdLevel() )
						mSock->Send( &toSend );
				}
			}
			break;
		}
		case SPTRG_BROADCASTPC:			// ALL PCs everywhere + NPCs in range
		case SPTRG_BROADCASTALL:
		{
			//std::scoped_lock lock(Network->internallock);
			Network->pushConn();
			for( mSock = Network->FirstSocket(); !Network->FinishedSockets(); mSock = Network->NextSocket() )
			{
				if( mSock != nullptr )
				{
					CChar *mChar = mSock->CurrcharObj();
					if( ValidateObject( mChar ) )
					{
						if( mChar->GetCommandLevel() >= toSay.CmdLevel() )
							mSock->Send( &toSend );
					}
				}
			}
			Network->popConn();
			break;
		}
		case SPTRG_ONLYRECEIVER:		// only the receiver gets the message
			sChar = calcCharObjFromSer( toSay.SpokenTo() );
			if( ValidateObject( sChar ) && sChar != thisChar )
			{
				mSock = sChar->GetSocket();
				if( mSock != nullptr )
					mSock->Send( &toSend );
			}
			break;
		case SPTRG_NULL:
			break;
	};
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool InternalPoll( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends out any pending speech, returning true if entries were sent
//o-----------------------------------------------------------------------------------------------o
bool CSpeechQueue::InternalPoll( void )
{
	bool retVal = false;
	CSpeechEntry *toCheck = nullptr;

	SPEECHLIST_ITERATOR slIter = speechList.begin();
	while( slIter != speechList.end() )
	{
		toCheck = (*slIter);

		if( toCheck->At() == -1 || static_cast<UI32>(toCheck->At()) <= cwmWorldState->GetUICurrentTime() )
		{
			retVal = true;
			SayIt( (*toCheck) );

			delete (*slIter);
			(*slIter) = nullptr;

			slIter = speechList.erase( slIter );
		}
		else
			++slIter;
	}
	return retVal;
}

bool CSpeechQueue::Poll( void )
{
	if( RunAsThread() )
	{
		while( RunAsThread() )
		{
			InternalPoll();
			std::this_thread::sleep_for(std::chrono::milliseconds( pollTime ));	// so that it's never 0 (and number of milliseconds)
		}
		return true;
	}
	else
		return InternalPoll();
}

CSpeechEntry& CSpeechQueue::Add( void )		// Make space in queue, and return pointer to new entry
{
	size_t iSize = speechList.size();
	speechList.push_back( new CSpeechEntry );
	return (*speechList[iSize]);
}

SI32 CSpeechQueue::PollTime( void ) const
{
	return pollTime;
}

void CSpeechQueue::PollTime( SI32 value )
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
		Console.error( strutil::format("Failed to open %s for writing", speechFile.c_str()) );
		return;
	}
	SPEECHLIST_ITERATOR toWrite;
	for( toWrite = speechList.begin(); toWrite != speechList.end(); ++toWrite )
	{
		speechDestination << "Time: " << (*toWrite)->At() << std::endl;
		speechDestination << "nColour: " << (*toWrite)->Colour() << std::endl;
		speechDestination << "nFont: " << (*toWrite)->Font()<< std::endl;
		speechDestination << "nLanguage: " << (*toWrite)->Language() << std::endl;
		speechDestination << "nSpeech: " << (*toWrite)->Speech() << std::endl;
		speechDestination << "nSpeaker: " << (*toWrite)->SpeakerName() << std::endl << std::endl;
	}
	speechDestination.close();
}
