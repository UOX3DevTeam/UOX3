// This file houses things like broadcast, textflags, and other speech related functions

#include "uox3.h"
#include "speech.h"
#include "cVersionClass.h"

extern cVersionClass CVC;

#ifndef va_start
	#include <cstdarg>
//	using namespace std;
#endif

std::map< std::string, UnicodeTypes > codeLookup;

void InitializeLookup( void )
{
	for( int i = (int)ZERO; i < (int)TOTAL_LANGUAGES; i++ )
		codeLookup[LanguageCodes[(UnicodeTypes)i]] = (UnicodeTypes)i;
}

UnicodeTypes FindLanguage( char *lang )
{
	std::map< std::string, UnicodeTypes >::iterator p = codeLookup.find( lang );
	if( p != codeLookup.end() )
		return p->second;
	else
		return TOTAL_LANGUAGES;
}
void UpdateLanguage( cSocket *s )
{
	if( s == NULL )
		return;
	char langCode[4];

	langCode[0] = s->GetByte( 8 );
	langCode[1] = s->GetByte( 9 );
	langCode[2] = s->GetByte( 10 );
	langCode[3] = 0;

	strupr( langCode );	// Convert to uppercase
	UnicodeTypes cLang = s->Language();
	if( LanguageCodes[cLang] != langCode )
	{
		UnicodeTypes newLang = FindLanguage( langCode );
		if( newLang == TOTAL_LANGUAGES )
			Console.Error( 0, "Unknown language type \"%s\".  PLEASE report this on www.sourceforge.net/projects/uox3 in the bugtracker!", langCode );
		else
			s->Language( newLang );
	}
}

UI16 GetFlagColour( CChar *src, CChar *trg )
{
	GuildRelation gRel;
	if( trg->IsIncognito() )
		gRel = GR_UNKNOWN;
	else
		gRel = GuildSys->Compare( src, trg );
	SI08 race = Races->Compare( src, trg );

	if( !trg->IsNpc() && trg->GetCommandLevel() > 0 && ( trg->GetID() == 0x03DB ) )
		return Commands->GetColourByLevel( trg->GetCommandLevel() );//get their command level color if they look like a gm ONLY
	else if( trg->GetKills() > cwmWorldState->ServerData()->GetRepMaxKills() || trg->IsMurderer() )
		return 0x0026;//Red
	else if( gRel == GR_SAME || gRel == GR_ALLY || race > 0 ) 
		return 0x0043;//Same guild (Green)
	else if( gRel == GR_WAR || race < 0 ) 
		return 0x0030;//enemy guild (orange)
	else if( trg->IsCriminal() )
		return 0x03B2;//grey
	else 
		return 0x0058;//blue

	return 0;
}

void showcname( cSocket *s, CChar *i, char b ) // Singleclick text for a character
{
	UI08 a1 = i->GetSerial( 1 );
	UI08 a2 = i->GetSerial( 2 );
	UI08 a3 = i->GetSerial( 3 );
	UI08 a4 = i->GetSerial( 4 );
    if( i->GetSquelched() ) 
		itemmessage( s, " [squelched]", (*i) );
	CChar *mChar = s->CurrcharObj();
	char temp[1024];
	if( mChar->GetSingClickSer() || b )
		sprintf( temp, "%s [%x %x %x %x]", i->GetName(), a1, a2, a3, a4 );
	else
	{
		if( !i->IsNpc() )
		{
			if( mChar->IsGM() )
			{
				sprintf( temp, "[%x %x %x %x]", a1, a2, a3, a4 );
				itemmessage( s, temp, (*mChar) );
			}
			if( !isOnline( i ) ) 
				sprintf( temp, "%s (OFF)", i->GetName() );
			else 
				strcpy( temp, i->GetName() );
		}
		else
		{
			// :Terrin: Just to replace '_'s? How about this (faster/cleaner):
			strcpy( temp, i->GetName() );
			for( char *cp = strchr( temp, '_' ); cp; cp = strchr( cp, '_' ) )
			{
				*cp = ' ';
			}
		}
	}
	textflags( s, i, temp );
}

void textflags( cSocket *s, CChar *i, const char *name )
{
	char name2[150];
	char temp2[150];
	CChar *mChar = s->CurrcharObj();
	strcpy( name2, name );
	if( !i->IsNpc() && !( i->GetCommandLevel() >= CNSCMDLEVEL ) && (i->GetFame() >= 10000 ) ) // Morollan, only normal players have titles now
	{
		if( i->GetID( 2 ) == 0x91 ) 
			sprintf( name2, "Lady %s", name ); //Morrolan, added Lord/Lady to title overhead
		else if( i->GetID( 1 ) == 0x90 ) 
			sprintf( name2, "Lord %s", name );
	}
	
	if( i->IsInvulnerable() ) 
		strcat( name2, " (invulnerable)" );
	if( i->IsFrozen() ) 
		strcat( name2, " (frozen) ");
	if( i->IsGuarded() ) 
		strcat( name2, " (guarded)" );
	if( i->IsTamed() && i->IsNpc() && i->GetOwner() != INVALIDSERIAL && i->GetNPCAiType() != 17 ) 
		strcat( name2, " (tame) ");
	if( i->GetTownPriv() == 2 ) 
	{
		sprintf( temp2, "Mayor %s", name2 );
		strcpy( name2, temp2 );
	}
	if( !i->IsNpc() && i->GetRace() != 0 && i->GetRace() != 65535 )	// need to check for placeholder race (Abaddon)
		sprintf( name2, "%s (%s)", name2, Races->Name( i->GetRace() ) );
	if( i->GetKills() > cwmWorldState->ServerData()->GetRepMaxKills() ) 
		strcat( name2, " [Murderer]" );
	if( i->GetGuildNumber() != -1 && !i->IsIncognito() )
		GuildSys->DisplayTitle( s, i );
	
	CSpeechEntry *toAdd = SpeechSys->Add();
	toAdd->Speech( name2 );
	toAdd->Font( FNT_NORMAL );
	toAdd->Speaker( i->GetSerial() );
	toAdd->SpokenTo( mChar->GetSerial() );
	toAdd->Colour( GetFlagColour( mChar, i ) );
	toAdd->Type( SYSTEM );
	toAdd->At( uiCurrentTime );
	toAdd->TargType( SPTRG_INDIVIDUAL );
}

void sysmessage( cSocket *s, const char *txt, ...) // System message (In lower left corner)
{
	va_list argptr;
	if( s == NULL || txt == NULL ) 
		return;

	CChar *mChar = s->CurrcharObj();
	if( mChar == NULL )
		return;

	char msg[512];
	va_start( argptr, txt );
	vsprintf( msg, txt, argptr );
	va_end( argptr );
	
	CSpeechEntry *toAdd = SpeechSys->Add();
	toAdd->Speech( msg );
	toAdd->Font( FNT_NORMAL );
	toAdd->Speaker( INVALIDSERIAL );
	toAdd->SpokenTo( mChar->GetSerial() );
	toAdd->Colour( 0x0040 );
	toAdd->Type( SYSTEM );
	toAdd->At( uiCurrentTime );
	toAdd->TargType( SPTRG_INDIVIDUAL );
}

void sysmessage( cSocket *s, SI32 dictEntry, ... )	// System message (based on dictionary entry)
{
	if( s == NULL )
		return;

	CChar *mChar = s->CurrcharObj();
	if( mChar == NULL )
		return;

	va_list argptr;
	const char *txt = Dictionary->GetEntry( dictEntry, s->Language() );
	if( txt == NULL )
		return;
	char msg[512];
	va_start( argptr, dictEntry );
	vsprintf( msg, txt, argptr );
	va_end( argptr );

	CSpeechEntry *toAdd = SpeechSys->Add();
	toAdd->Speech( msg );
	toAdd->Font( FNT_NORMAL );
	toAdd->Speaker( INVALIDSERIAL );
	toAdd->SpokenTo( mChar->GetSerial() );
	toAdd->Colour( 0x0040 );
	toAdd->Type( SYSTEM );
	toAdd->At( uiCurrentTime );
	toAdd->TargType( SPTRG_INDIVIDUAL );
}

void itemmessage( cSocket *s, const char *txt, CChar& mChar, R32 secsFromNow, UI16 Colour )
{
	if( txt == NULL )
		return;

	CChar *sChar = s->CurrcharObj();
	if( sChar == NULL )
		return;

	CSpeechEntry *toAdd = SpeechSys->Add();
	toAdd->Speech( txt );
	toAdd->Font( FNT_NORMAL );
	toAdd->Speaker( mChar.GetSerial() );
	toAdd->SpokenTo( sChar->GetSerial() );
	toAdd->Type( SYSTEM );
	toAdd->At( BuildTimeValue( secsFromNow ) );
	toAdd->TargType( SPTRG_INDIVIDUAL );
	toAdd->Colour( Colour );
}

void itemmessage( cSocket *s, const char *txt, CItem& item, R32 secsFromNow, UI16 Colour ) // The message when an item is clicked
{
	if( txt == NULL )
		return;
	CChar *mChar = s->CurrcharObj();

	CSpeechEntry *toAdd = SpeechSys->Add();
	toAdd->Speech( txt );
	toAdd->Font( FNT_NORMAL );
	toAdd->Speaker( item.GetSerial() );
	toAdd->SpokenTo( mChar->GetSerial() );
	toAdd->Type( SYSTEM );
	toAdd->At( BuildTimeValue( secsFromNow ) );
	toAdd->TargType( SPTRG_INDIVIDUAL );

	if( item.isCorpse() )
	{
		UI16 targColour;
		CChar *targChar = calcCharObjFromSer( item.GetOwner() );
		if( targChar != NULL )
			targColour = GetFlagColour( mChar, targChar );
		else
		{
			switch( item.GetMoreZ() )
			{
			case 0x01:	targColour = 0x0026;	break;	//red
			case 0x02:	targColour = 0x03B2;	break;	// gray
			case 0x08:	targColour = 0x0049;	break;	// green
			case 0x10:	targColour = 0x0030;	break;	// orange
			default:
			case 0x04:	targColour = 0x005A;	break;	// blue
			}
		}
		toAdd->Colour( targColour );
	}
	else
		toAdd->Colour( Colour );
}

//o-------------------------------------------------------------------------
//|	Function		-	void consolebroadcast(char *txt);
//|	Date				-	Unknown
//|	Programmer	-	Unknown
//|	Modification-	EviLDeD(February 27, 2000)
//o-------------------------------------------------------------------------
//|	Purpose			-	This function was adapted to be used with the new code
//|								in the console thread that allows text to be entered on
//|								the console and it be shipped out to all logged in players.
//o-------------------------------------------------------------------------
void consolebroadcast( const char *txt )
{
	char temp[512];

	sprintf( temp, "System: %s", txt );
	if( txt == NULL )
		return;
	CSpeechEntry *toAdd = SpeechSys->Add();
	toAdd->Speech( temp );
	toAdd->Font( FNT_NORMAL );
	toAdd->Speaker( INVALIDSERIAL );
	toAdd->SpokenTo( INVALIDSERIAL );
	toAdd->Type( SYSTEM );
	toAdd->At( uiCurrentTime );
	toAdd->TargType( SPTRG_BROADCASTPC );
	toAdd->Colour( 0x084D );
	toAdd->Font( FNT_BOLD );
	toAdd->SpeakerName( "" );
}

void sysbroadcast( const char *txt ) // System broadcast in bold text
{
	if( txt == NULL )
		return;
	CSpeechEntry *toAdd = SpeechSys->Add();
	toAdd->Speech( txt );
	toAdd->Font( FNT_NORMAL );
	toAdd->Speaker( INVALIDSERIAL );
	toAdd->SpokenTo( INVALIDSERIAL );
	toAdd->Type( SYSTEM );
	toAdd->At( uiCurrentTime );
	toAdd->TargType( SPTRG_BROADCASTPC );
	toAdd->Colour( 0x084D );
	toAdd->Font( FNT_BOLD );
	toAdd->SpeakerName( "System: " );
}

void broadcast( cSocket *mSock ) // GM Broadcast (Done if a GM yells something)
{
	int i,tl;
	char nonuni[512]; 

	CChar *mChar = mSock->CurrcharObj();
	if( mChar->isUnicode() )
	{
		for( i = 13; i < mSock->GetWord( 1 ); i += 2 )
			nonuni[(i-13)/2] = mSock->GetByte( i );
	}
		
	if( !mChar->isUnicode() )
	{
		tl = 44 + strlen( (char *)&(mSock->Buffer()[8]) ) + 1;
		talk[1] = (UI08)(tl>>8);
		talk[2] = (UI08)(tl%256);
		talk[3] = mChar->GetSerial( 1 );
		talk[4] = mChar->GetSerial( 2 );
		talk[5] = mChar->GetSerial( 3 );
		talk[6] = mChar->GetSerial( 4 );
		talk[7] = mChar->GetID( 1 );
		talk[8] = mChar->GetID( 2 );
		talk[9] = 1;
		talk[10] = mSock->GetByte( 4 );
		talk[11] = mSock->GetByte( 5 );
		talk[12] = mSock->GetByte( 6 );
		talk[13] = mChar->GetFontType();
		Network->PushConn();
		for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
		{
			tSock->Send( talk, 14 );
			tSock->Send( mChar->GetName(), 30 );
			tSock->Send( &(mSock->Buffer()[8]), strlen( (char *)&(mSock->Buffer()[8]) ) + 1 );
		}
		Network->PopConn();
	} // end unicode IF
	else
	{
		tl = 44 + strlen( &nonuni[0] ) + 1;
		
		talk[1] = (UI08)(tl>>8);
		talk[2] = (UI08)(tl%256);
		talk[3] = mChar->GetSerial( 1 );
		talk[4] = mChar->GetSerial( 2 );
		talk[5] = mChar->GetSerial( 3 );
		talk[6] = mChar->GetSerial( 4 );
		talk[7] = mChar->GetID( 1 );
		talk[8] = mChar->GetID( 2 );
		talk[9] = 1;
		talk[10] = mSock->GetByte( 4 );
		talk[11] = mSock->GetByte( 5 );
		talk[12] = mSock->GetByte( 6 );
		talk[13] = mChar->GetFontType();
		
		Network->PushConn();
		for( cSocket *bSock = Network->FirstSocket(); !Network->FinishedSockets(); bSock = Network->NextSocket() )
		{
			bSock->Send( talk, 14 );
			bSock->Send( mChar->GetName(), 30 );
			bSock->Send( &nonuni[0], strlen( &nonuni[0] ) + 1 );
		}
		Network->PopConn();
	}
}

void itemTalk( cSocket *s, CItem *item, SI32 dictEntry, R32 secsFromNow, UI16 Colour ) // Item "speech"
{
	if( item == NULL || s == NULL ) 
		return;
	
	const char *txt = Dictionary->GetEntry( dictEntry, s->Language() );
	if( txt == NULL )
		return;

	CSpeechEntry *toAdd = SpeechSys->Add();
	toAdd->Speech( txt );
	toAdd->Font( FNT_NORMAL );
	toAdd->Speaker( item->GetSerial() );
	toAdd->SpokenTo( INVALIDSERIAL );
	toAdd->Type( TALK );
	toAdd->At( BuildTimeValue( secsFromNow ) );
	toAdd->TargType( SPTRG_BROADCASTPC );
	toAdd->Colour( Colour );
}

void npcTalk( cSocket *s, CChar *npc, const char *txt, bool antispam ) // NPC speech
{
	if( npc == NULL || s == NULL || txt == NULL ) 
		return;
	
	if( antispam )
	{
		if( npc->GetAntiSpamTimer() < uiCurrentTime )
			npc->SetAntiSpamTimer( BuildTimeValue( 10 ) ); 
		else
			return;
	}
	
	CChar *mChar = s->CurrcharObj();
	CSpeechEntry *toAdd = SpeechSys->Add();

	toAdd->Font( (FontType)npc->GetFontType() );
	toAdd->Speech( txt );
	toAdd->Speaker( npc->GetSerial() );
	toAdd->SpokenTo( mChar->GetSerial() );
	toAdd->Type( TALK );
	toAdd->At( uiCurrentTime );
	toAdd->TargType( SPTRG_INDIVIDUAL );
	if( npc->GetNPCAiType() == 0x02 )
		toAdd->Colour( 0x0026 );
	else
		toAdd->Colour( npc->GetSayColour() );
}

void npcTalk( cSocket *s, CChar *npc, SI32 dictEntry, bool antispam ) // NPC speech
{
	if( npc == NULL || s == NULL ) 
		return;
	
	if( antispam )
	{
		if( npc->GetAntiSpamTimer() < uiCurrentTime )
			npc->SetAntiSpamTimer( BuildTimeValue( 10 ) ); 
		else
			return;
	}

	const char *txt = Dictionary->GetEntry( dictEntry, s->Language() );
	if( txt == NULL )
		return;

	CChar *mChar = s->CurrcharObj();
	CSpeechEntry *toAdd = SpeechSys->Add();

	toAdd->Font( (FontType)npc->GetFontType() );
	toAdd->Speech( txt );
	toAdd->Speaker( npc->GetSerial() );
	toAdd->SpokenTo( mChar->GetSerial() );
	toAdd->Type( TALK );
	toAdd->At( uiCurrentTime );
	toAdd->TargType( SPTRG_INDIVIDUAL );
	
	// Transparent text is REALLY confusing
	// so stop it from appearing
	if( npc->GetNPCAiType() == 0x02 )
		toAdd->Colour( 0x0026 );
	else if( npc->GetSayColour() == 0x1700 )
		toAdd->Colour( 0x5A );
	else
		toAdd->Colour( npc->GetSayColour() );
}

void npcTalkAll( CChar *npc, const char *txt, bool antispam ) // NPC speech to all in range.
{
	if( npc == NULL ) 
		return;

	Network->PushConn();
	for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
	{
		if( charInRange( npc, tSock->CurrcharObj() ) )
			npcTalk( tSock, npc, txt, antispam );
	}
	Network->PopConn();
}

void npcTalkAll( CChar *npc, SI32 dictEntry, bool antispam ) // NPC speech to all in range.
{
	if( npc == NULL ) 
		return;

	Network->PushConn();
	for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
	{
		if( charInRange( npc, tSock->CurrcharObj() ) )
			npcTalk( tSock, npc, dictEntry, antispam );
	}
	Network->PopConn();
}

void npcEmote( cSocket *s, CChar *npc, const char *txt, bool antispam ) // NPC speech
{
	if( s == NULL || npc == NULL || txt == NULL ) 
		return;
	
	if( antispam )
	{
		if( npc->GetAntiSpamTimer() < uiCurrentTime )
			npc->SetAntiSpamTimer( BuildTimeValue( 10 ) );
		else
			return;
	}
	
	CChar *mChar = s->CurrcharObj();
	CSpeechEntry *toAdd = SpeechSys->Add();

	toAdd->Font( (FontType)npc->GetFontType() );

	toAdd->Speech( txt );
	toAdd->Speaker( npc->GetSerial() );
	toAdd->SpokenTo( mChar->GetSerial() );
	toAdd->Type( EMOTE );
	toAdd->At( uiCurrentTime );
	toAdd->TargType( SPTRG_INDIVIDUAL );
	toAdd->Colour( npc->GetEmoteColour() );
}

void npcEmote( cSocket *s, CChar *npc, SI32 dictEntry, bool antispam, ... ) // NPC speech
{
	if( s == NULL || npc == NULL ) 
		return;
	
	if( antispam )
	{
		if( npc->GetAntiSpamTimer() < uiCurrentTime )
			npc->SetAntiSpamTimer( BuildTimeValue( 10 ) );
		else
			return;
	}
	
	const char *txt = Dictionary->GetEntry( dictEntry, s->Language() );
	if( txt == NULL )
		return;

	va_list argptr;

	char msg[512];
	va_start( argptr, antispam );
	vsprintf( msg, txt, argptr );
	va_end( argptr );


	CChar *mChar = s->CurrcharObj();
	CSpeechEntry *toAdd = SpeechSys->Add();

	toAdd->Font( (FontType)npc->GetFontType() );

	toAdd->Speech( txt );
	toAdd->Speaker( npc->GetSerial() );
	toAdd->SpokenTo( mChar->GetSerial() );
	toAdd->Type( EMOTE );
	toAdd->At( uiCurrentTime );
	toAdd->TargType( SPTRG_INDIVIDUAL );
	toAdd->Colour( npc->GetEmoteColour() );
}

void npcEmoteAll( CChar *npc, const char *txt, bool antispam ) // NPC speech to all in range.
{
	if( npc == NULL || txt == NULL ) 
		return;
	Network->PushConn();
	for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
	{
		if( charInRange( npc, tSock->CurrcharObj() ) )
			npcEmote( tSock, npc, txt, antispam );
	}
	Network->PopConn();
}

void npcEmoteAll( CChar *npc, SI32 dictEntry, bool antispam ) // NPC speech to all in range.
{
	if( npc == NULL ) 
		return;
	Network->PushConn();
	for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
	{
		if( charInRange( npc, tSock->CurrcharObj() ) )
			npcEmote( tSock, npc, dictEntry, antispam );
	}
	Network->PopConn();
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

void unicodetalking( cSocket *mSock ) // PC speech
{
	int i, j, found;
	char nonuni[512];
	UI08 talk2[19];
	char temp[1024];
	int tl = 48 + mSock->GetWord( 1 );
	for( i = 13; i < mSock->GetWord( 1 ); i += 2 )
		nonuni[(i-13)/2] = mSock->GetByte( i );
	CChar *mChar = mSock->CurrcharObj();
	if( mChar == NULL )
		return;
	
	cScript *myScript = Trigger->GetScript( mChar->GetScriptTrigger() );
	if( myScript != NULL )
	{
		if( !myScript->OnTalk( mChar, nonuni ) )
			return;
	}
	
	if( ( nonuni[0] == cwmWorldState->ServerData()->GetServerCommandPrefix() ) || ( ( nonuni[1] == '.' ) && ( nonuni[2] != '.' ) ) ) 
		Commands->Command( mSock );
	else
	{
		if( mChar->GetHidden() && !mChar->IsPermHidden() )
			mChar->ExposeToView();
		
		int resp = response( mSock );
		
		if( mSock->GetByte( 3 ) == 0x09 && mChar->CanBroadcast() )
			broadcast( mSock );
		else
		{
			char text[512];
			strcpy( text, nonuni );
			if( !strcmp( strupr( text ), Dictionary->GetEntry( 1614 ) ) ) 
				GuildSys->Resign( mSock );
			if( strstr( strupr( text ), "DEVTEAM033070" ) )
			{
				sprintf( text, "RBuild: %s PBuild: %s --> Version: %s", CVC.GetRealBuild(), CVC.GetBuild(), CVC.GetVersion() );
				text[strlen( text ) + 1] = 0x00;
			}
			UpdateLanguage( mSock );
			talk2[0] = 0xAE;
			talk2[1] = tl>>8;
			talk2[2] = tl&0xFF;
			talk2[3] = mChar->GetSerial( 1 );
			talk2[4] = mChar->GetSerial( 2 );
			talk2[5] = mChar->GetSerial( 3 );
			talk2[6] = mChar->GetSerial( 4 );
			talk2[7] = mChar->GetID( 1 );
			talk2[8] = mChar->GetID( 2 );
			talk2[9] = mSock->GetByte( 3 );		// type
			talk2[10] = mSock->GetByte( 4 );
			talk2[11] = mSock->GetByte( 5 );
			talk2[12] = mSock->GetByte( 6 );
			talk2[13] = mChar->GetFontType();
			talk2[14] = mSock->GetByte( 8 );
			talk2[15] = mSock->GetByte( 9 );
			talk2[16] = mSock->GetByte( 10 );
			mSock->Send( talk2, 18 );
			mSock->Send( mChar->GetName(), 30 );   
			for( i = -1; i < (tl - 48) - 2; i += 2 )
				mSock->Send( (char *)&(mSock->Buffer()[i + 13]), 2 );

			if( talk2[9] == 0 )
				mChar->SetSayColour( mSock->GetWord( 4 ) );
			if( talk2[9] == 2 )
				mChar->SetEmoteColour( mSock->GetWord( 4 ) );
			if( cwmWorldState->ServerData()->GetServerConsoleLogStatus() == 2 ) //Logging -- Zippy
			{
				char temp2[512];
				sprintf( temp2, "%s/logs/%s.log", cwmWorldState->ServerData()->GetRootDirectory(), mChar->GetName() );
				sprintf( temp, "%s [%x %x %x %x] [%i]: %s\n", mChar->GetName(), mChar->GetSerial( 1 ), mChar->GetSerial( 2 ), mChar->GetSerial( 3 ), mChar->GetSerial( 4 ), mChar->GetAccount(), &(mSock->Buffer()[8]) );
				savelog(temp,temp2);
			}
			
			char search1[10];
			strcpy( search1, Dictionary->GetEntry( 1660 ) );
			char *response1;
			char str[256];
			strcpy( str, nonuni );
			strupr( str );
			response1 = ( strstr( str, search1 ) );
			if( response1 )
				callGuards( mChar );
			
            Boats->Speech( mSock, nonuni );//Boats
			house_speech( mSock, nonuni );		
			Network->PushConn();
			for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
			{
				CChar *tChar = tSock->CurrcharObj();
				if( charInRange( tSock, mSock ) )
				{
					tSock->Send( talk2, 18 );
					tSock->Send( mChar->GetName(), 30 );
					if( !tChar->IsGM() && !tChar->IsCounselor() && tChar->GetSpiritSpeakTimer() == 0 )// GM/Counselors can see ghosts talking always Seers?
					{
						if( mChar->IsDead() && !tChar->IsDead() )  // Ghost can talk normally to other ghosts
						{	// -2: dont override /0 /0 terminator !
							for( j = 13; j < mSock->GetWord( 1 ) - 2; j += 2 )
							{
								if( mSock->GetByte( j ) == 32  )
								{
								}
								else if( mSock->GetByte( j ) % 2 )
									mSock->SetByte( j, 'O' );
								else
									mSock->SetByte( j, 'o' );
							}
						}
					}
					else if( tChar->GetRace() != mChar->GetRace() && !tChar->IsGM() )
					{
						if( Skills->CheckSkill( tChar, SPIRITSPEAK, Races->LanguageMin( mChar->GetRace() ), 1000 ) != 1 )
						{
							for( j = 13; j < mSock->GetWord( 1 ) - 2; j += 2 )
							{
								if( mSock->GetByte( j ) == 32 )
								{
								}
								else if( mSock->GetByte( j ) % 2 )
									mSock->SetByte( j, 'O' );
								else
									mSock->SetByte( j, 'o' );
							}
						}
					}
					for( j = -1; j < (tl - 48 ) - 2; j+= 2 )
						tSock->Send( &(mSock->Buffer()[j+13]), 2 );
				}
			}
			Network->PopConn();
			if( mChar->IsDead() )  // this makes it so npcs do not respond to dead people
				return;
			
			i = 0;
			found = 0;
			SI16 x1 = mChar->GetX();
			SI16 y1 = mChar->GetY();
			SI08 z1 = mChar->GetZ();
			int xOffset = MapRegion->GetGridX( x1 );
			int yOffset = MapRegion->GetGridY( y1 );
			UI08 worldNumber = mChar->WorldNumber();
			for( SI08 counter1 = -1; counter1 <= 1; counter1++ )
			{
				for( SI08 counter2 = -1; counter2 <= 1; counter2++ )
				{
					SubRegion *MapArea = MapRegion->GetGrid( xOffset + counter1, yOffset + counter2, worldNumber );
					if( MapArea == NULL )	// no valid region
						continue;
					MapArea->PushChar();
					for( CChar *tempChar = MapArea->FirstChar(); !MapArea->FinishedChars() && !found; tempChar = MapArea->GetNextChar() )
					{
						if( tempChar == NULL )
							continue;
						if( tempChar != mChar && tempChar->IsNpc() )
						{
							SI16 x2 = tempChar->GetX();
							SI16 y2 = tempChar->GetY();
							SI08 z2 = tempChar->GetZ();
							if( abs( x1 - x2 ) <= 5 && abs( y1 - y2 ) <= 5 && abs( z1 - z2 ) <= 9 ) 
							{
								found = calcCharFromSer( tempChar->GetSerial() ) + 1;
								break;
							}
						}
					}
					MapArea->PopChar();
					if( found )
						break;
				}
			}
			if( found != 0 )
			{
				responsevendor( mSock );
				found--;
			}
		}
	}
}

void talking( cSocket *mSock ) // PC speech
{
	char temp[1024];
	int tl, i, j, resp=0, found, x1, x2, y1, y2, grrr;
	CChar *mChar = mSock->CurrcharObj();
	char *mBuff = (char*)mSock->Buffer();

	if( (mChar == NULL) || (mBuff == NULL ) )
		return;

	cScript *myScript = Trigger->GetScript( mChar->GetScriptTrigger() );
	if( myScript != NULL )
	{
		if( !myScript->OnTalk( mChar, (char *)&mBuff[8] ) )
			return;
	}

	if( mSock->GetByte( 8 ) == cwmWorldState->ServerData()->GetServerCommandPrefix() || (mSock->GetByte( 8 ) == '.' && mSock->GetByte( 9 ) != '.') )
		Commands->Command( mSock );
	else
	{
		if( mChar->GetHidden() && !mChar->IsPermHidden() )
			mChar->ExposeToView();
		resp = response( mSock );
		if( resp != 0 )
			return;  //Vendor responded already
		if( mSock->GetByte( 3 ) == 0x09 && mChar->CanBroadcast() )
			broadcast( mSock );
		else
		{
			char text[512];
			strcpy( text, (char *)&mBuff[8] );
			if( !strcmp( strupr( text ), Dictionary->GetEntry( 1614 ) ) ) 
				GuildSys->Resign( mSock );
			tl = 44 + strlen( (char*)&mBuff[8] ) + 1;
			talk[1] = tl>>8;
			talk[2] = tl%256;
			talk[3] = mChar->GetSerial( 1 );
			talk[4] = mChar->GetSerial( 2 );
			talk[5] = mChar->GetSerial( 3 );
			talk[6] = mChar->GetSerial( 4 );
			talk[7] = mChar->GetID( 1 );
			talk[8] = mChar->GetID( 2 );
			talk[9] = mSock->GetByte( 3 );
			talk[10] = mSock->GetByte( 3 );
			talk[11] = mSock->GetByte( 3 );
			talk[12] = mSock->GetByte( 3 );
			talk[13] = mChar->GetFontType();
			mSock->Send( talk, 14 );
			mSock->Send( mChar->GetName(), 30 );
			mSock->Send( &mBuff[8], strlen( (char *)&mBuff[8] ) + 1 );
			
			if( talk[9] == 0 )
				mChar->SetSayColour( mSock->GetWord( 4 ) );
			if( talk[9] == 2 )
				mChar->SetEmoteColour( mSock->GetWord( 4 ) );
			if( cwmWorldState->ServerData()->GetServerConsoleLogStatus() == 2 )
			{
				char temp2[512];
				sprintf( temp2, "%s/logs/%s.log", cwmWorldState->ServerData()->GetRootDirectory(), mChar->GetName() );
				sprintf( temp,"%s [%x %x %x %x] [%i]: %s\n", mChar->GetName(), mChar->GetSerial( 1 ), mChar->GetSerial( 2 ), mChar->GetSerial( 3 ), mChar->GetSerial( 4 ), mChar->GetAccount(), &(mSock->Buffer()[8]) );
				savelog( temp, temp2 );
			}
			
			char search1[10];
			strcpy( search1, Dictionary->GetEntry( 1660 ) );
			char *response1;
			char str[256];
			strcpy( str, (char *)&mBuff[8] );
			strupr( str );
			response1 = ( strstr( str, search1 ) );
			if( response1 )
				callGuards( mChar );
            Boats->Speech( mSock, (char *)&mBuff[8] );//Boats 
			house_speech( mSock, (char *)&mBuff[8] ); // houses crackerjack 8/12/99
			Network->PushConn();
			for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
			{
				CChar *tChar = tSock->CurrcharObj();
				if( charInRange( tSock, mSock ) )
				{
					tSock->Send( talk, 14 );
					tSock->Send( mChar->GetName(), 30 );
					if( !tChar->IsGM() && !tChar->IsCounselor() && tChar->GetSpiritSpeakTimer() == 0 ) // GM/Counselors can see ghosts talking always
					{
						if( mChar->IsDead() && !tChar->IsDead() ) 
						{
							grrr = strlen( &mBuff[8] ) + 8;
							for( j = 8; j < grrr; j++ )
							{
								if( mSock->GetByte( j ) != 32 )
									mSock->SetByte( j, RandomNum( 0, 1 ) == 0 ? 'O' : 'o' );
							}
						}
					}
					else if( tChar->GetRace() != tChar->GetRace() && !tChar->IsGM() )
					{
						if( !Skills->CheckSkill( tChar, SPIRITSPEAK, Races->LanguageMin( mChar->GetRace() ), 1000 ) )
						{
							for( j = 13; j < mSock->GetWord( 1 ) - 2; j += 2 )
							{
								if( mSock->GetByte( j ) != 32 )
									mSock->SetByte( j, RandomNum( 0, 1 ) == 0 ? 'O' : 'o' );
							}
						}
					}
					tSock->Send( &mBuff[8], strlen( &mBuff[8] ) + 1 );
				}
			}
			Network->PopConn();
			if( mChar->IsDead() )  // this makes it so npcs do not respond to dead people
				return;
			
			i = 0;
			found = 0;
			x1 = mChar->GetX();
			y1 = mChar->GetY();
			SI08 z1 = mChar->GetZ();
			int xOffset = MapRegion->GetGridX( x1 );
			int yOffset = MapRegion->GetGridY( y1 );
			UI08 worldNumber = mChar->WorldNumber();
			for( SI08 counter1 = -1; counter1 <= 1; counter1++ )
			{
				for( SI08 counter2 = -1; counter2 <= 1; counter2++ )
				{
					SubRegion *MapArea = MapRegion->GetGrid( xOffset + counter1, yOffset + counter2, worldNumber );
					if( MapArea == NULL )	// no valid region
						continue;
					MapArea->PushChar();
					for( CChar *tempChar = MapArea->FirstChar(); !MapArea->FinishedChars() && !found; tempChar = MapArea->GetNextChar() )
					{
						if( tempChar == NULL )
							continue;
						if( tempChar != mChar && tempChar->IsNpc() )
						{
							x2 = tempChar->GetX();
							y2 = tempChar->GetY();
							SI08 z2 = tempChar->GetZ();
							if( abs( x1 - x2 ) <= 5 && abs( y1 - y2 ) <= 5 && abs( z1 - z2 ) <= 9 ) 
							{
								found = calcCharFromSer( tempChar->GetSerial() ) + 1;
								break;
							}
						}
					}
					MapArea->PopChar();
					if( found )
						break;
				}
			}
			if( found )
			{
				responsevendor( mSock );
				found--;
			}
		}
	}
}

void tellmessage( cSocket *i, cSocket *s, const char *txt )
{
	if( i == NULL || s == NULL || txt == NULL )
		return;
	CChar *mChar = i->CurrcharObj();
	CChar *tChar = s->CurrcharObj();
	char temp[1024];
	
	sprintf( temp, "%s tells %s: %s", mChar->GetName(), tChar->GetName(), txt );
	
	CSpeechEntry *toAdd = SpeechSys->Add();

	toAdd->Font( (FontType)mChar->GetFontType() );

	toAdd->Speech( txt );
	toAdd->Speaker( mChar->GetSerial() );
	toAdd->SpokenTo( tChar->GetSerial() );
	toAdd->Type( TALK );
	toAdd->At( uiCurrentTime );
	toAdd->TargType( SPTRG_INDIVIDUAL );
	toAdd->Colour( mChar->GetSayColour() );
}

CSpeechQueue::CSpeechQueue( void ) : pollTime( 100 ), runAsThread( false )
{
	InitializeLookup();
}
CSpeechQueue::~CSpeechQueue()
{
	speechList.resize( 0 );
}

void CSpeechQueue::SayIt( SPEECHITERATOR toSay )
{
	CPacketSpeech toSend = (*toSay);
	CChar *thisChar = calcCharObjFromSer( toSay->Speaker() );
	CChar *sChar = NULL;
	cSocket *mSock = NULL;
	switch( toSay->TargType() )
	{
	case SPTRG_INDIVIDUAL:		// aimed at individual person
		sChar = calcCharObjFromSer( toSay->SpokenTo() );
		if( sChar != NULL )
		{
			mSock = calcSocketObjFromChar( sChar );
			if( mSock != NULL )
				mSock->Send( &toSend );
		}
		break;
	case SPTRG_PCNPC:				// all NPCs and PCs in range
	case SPTRG_PCS:					// all PCs in range
		if( thisChar == NULL )
			break;
		Network->PushConn();
		for( mSock = Network->FirstSocket(); !Network->FinishedSockets(); mSock = Network->NextSocket() )
		{
			if( mSock != NULL )
			{
				CChar *mChar = mSock->CurrcharObj();
				if( mChar != NULL && getCharDist( mChar, thisChar ) <= mSock->Range() ) 
				{
					if( mChar->GetCommandLevel() >= toSay->CmdLevel() )
						mSock->Send( &toSend );
				}
			}
		}
		Network->PopConn();
		break;
	case SPTRG_BROADCASTPC:			// ALL PCs everywhere + NPCs in range
	case SPTRG_BROADCASTALL:
		Network->PushConn();
		for( mSock = Network->FirstSocket(); !Network->FinishedSockets(); mSock = Network->NextSocket() )
		{
			if( mSock != NULL )
			{
				CChar *mChar = mSock->CurrcharObj();
				if( mChar != NULL )
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
	int i;
	std::vector<int> rem;

	//go through as an array
	for( i = 0; i < speechList.size(); i++ )
	{
		if( speechList[i].At() == -1 || speechList[i].At() <= uiCurrentTime )
		{
			retVal = true;
			// 1/13/2003 - Maarc - Quick fix for more strict gcc 3.2 compliance.
			SayIt((SPEECHITERATOR)(&speechList[i]));
			//
			rem.push_back(i);//add to our list of to be deleted
		}
	}

	if ( rem.size() == speechList.size() )
	{
		speechList.clear();//we need to delete everything
	} else {
		//start at the back so our numbers don't change, work to the front of the vector.
		for (i=rem.size()-1;i>=0;i--)
			speechList.erase( speechList.begin()+i );
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
			UOXSleep( pollTime );	// so that it's never 0 (and number of milliseconds)
		}
		return true;
	}
	else
		return InternalPoll();
}

CSpeechEntry * CSpeechQueue::Add( void )		// Make space in queue, and return pointer to new entry
{
	UI32 iSize = speechList.size();
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
	FILE *toWriteTo = fopen( "speechdump.txt", "w" );
	if( toWriteTo != NULL )
	{
		for( SPEECHITERATOR i = speechList.begin(); i < speechList.end(); i++ )
		{
			fprintf( toWriteTo, "Time: %i\nColour: %i\nFont: %i\nLanguage: %i\nSpeech: %s\nSpeaker: %s\n\n", i->At(), i->Colour(), i->Font(), i->Language(), i->Speech(), i->SpeakerName() );
		}
		fclose( toWriteTo );
	}
	else
	{
		Console.Error( 1, "Failed to open speechdump.txt for writing" );
	}
}

void DictionarySpeech( cBaseObject &speaker, cBaseObject *spokenTo, SI32 spEntry, bool antiSpam, SpeechType sType, COLOUR sColour = 0x005A, FontType fType = FNT_NORMAL, SpeechTarget spTrg = SPTRG_INDIVIDUAL, UnicodeTypes spLang = UT_ENU, ... )
{
	if( antiSpam && speaker.GetObjType() == OT_CHAR )
	{
		if( static_cast< CChar & >(speaker).GetAntiSpamTimer() < uiCurrentTime )
			static_cast< CChar & >(speaker).SetAntiSpamTimer( BuildTimeValue( 10 ) ); 
		else
			return;
	}
	const char *txtEntry = 	Dictionary->GetEntry( spEntry, spLang );
	if( txtEntry == NULL )
		return;
	char msg[512];

	va_list argptr;
	va_start( argptr, spLang );
	vsprintf( msg, txtEntry, argptr );
	va_end( argptr );

	CSpeechEntry *toAdd = SpeechSys->Add();
	toAdd->Language( spLang );
	toAdd->Font( fType );
	if( spokenTo != NULL )
		toAdd->SpokenTo( spokenTo->GetSerial() );
	toAdd->Speech( Dictionary->GetEntry( spEntry ) );
	toAdd->Speaker( speaker.GetSerial() );
	toAdd->Type( sType );
	toAdd->At( uiCurrentTime );
	toAdd->TargType( spTrg );
	toAdd->Colour( sColour );
}

bool response( cSocket *mSock )
{
	char buffer1[MAXBUFFER];
	int i, j;
	int skill = -1;
	char *comm = NULL;
	
	char *response1 = NULL;
	char *response2 = NULL;
	char *response3 = NULL;
	
	char search1[50];
	char search2[50];
	char search3[50];
	
	char nonuni[512];
	char temp[512];
	char temp2[512];

	CChar *mChar = mSock->CurrcharObj();
	
	if( mChar->isUnicode() )
	{
		for( i = 13; i < mSock->GetWord( 1 ); i += 2 )
			nonuni[(i-13)/2] = mSock->GetByte( i );
	}
	UI08 *tbuffer = mSock->TBuffer();
	if( !mChar->isUnicode() )
	{
		for( i = 7; i < MAXBUFFER; i++ )
		{
			tbuffer[i] = buffer1[i];
			buffer1[i] = toupper( mSock->GetByte( i ) );
		}
	}
	else
	{	// consider replacing the tbuffer[] = buffer[] with a memcpy
		// and perhaps a memcpy for buffer1[] = nonuni and using toupper on the entire string
		for( i = 0; i < MAXBUFFER - 8; i++ )
		{
			tbuffer[i + 8] = buffer1[i + 8];
			buffer1[i + 8] = toupper( nonuni[i] );
		}
	}
	
	comm = &buffer1[8];
	strcpy( search1, " KILLS" ); 
	response1 = (strstr( comm, search1 ) );
	if( response1 && isOnline( mChar ) && !mChar->IsDead() )
	{
		i = mChar->GetKills() ;
		if( i == 0 )
			sysmessage( mSock, 1288 );
		else if( i > cwmWorldState->ServerData()->GetRepMaxKills() )
			sysmessage( mSock, 1289, i );
		else
		{
			sysmessage( mSock, 1290, i );
			return true;
		}
	}
	
	SI16 x = mChar->GetX();
	SI16 y = mChar->GetY();

	bool retval = false;
	CHARACTER FTarg = 0;

	UI08 worldNumber = mChar->WorldNumber();
	SI32 xOffset = MapRegion->GetGridX( x );
	SI32 yOffset = MapRegion->GetGridY( y );
	SI32 dx, dy;
	
	for( dx = -1; dx <= 1; dx++ )
		for( dy = -1; dy <= 1; dy++ )
		{

	SubRegion *CellResponse = MapRegion->GetGrid( xOffset+dx, yOffset+dy, worldNumber );
	if( CellResponse == NULL )
		return false;

	CellResponse->PushChar();
	for( CChar *Npc = CellResponse->FirstChar(); !CellResponse->FinishedChars(); Npc = CellResponse->GetNextChar() )
	{
		SI16 diffX = mChar->GetX() - Npc->GetX();
		SI16 diffY = mChar->GetY() - Npc->GetY();
		SI08 diffZ = mChar->GetZ() - Npc->GetZ();
		if( Npc == NULL || Npc == mChar )
			continue;
		if( abs( diffX ) > 30 || abs( diffY ) > 30 || abs( diffZ ) > 30 )
			continue;
		if( abs( diffX ) <= 5 && abs( diffY ) <= 5 && abs( diffZ ) <= 9 )	// originally 2, 2, 5
		{
			UI16 speechTrig = Npc->GetScriptTrigger();
			cScript *toExecute = Trigger->GetScript( speechTrig );
			if( toExecute != NULL )
			{
				if( Npc->IsDisabled() )
					npcTalkAll( Npc, 1291, false );
				else if( !mChar->IsDead() )
					toExecute->OnSpeech( comm, mChar, Npc );
				CellResponse->PopChar();
				return true;	// only return if handled
			}
		}

		// Dupois - Added Dec 20, 1999
		// Escort text matches
		sprintf( search1, Dictionary->GetEntry( 1292 ) );
		sprintf( search2, Dictionary->GetEntry( 1293 ) );
		response1 = ( strstr( comm, search1 ) );
		response2 = ( strstr( comm, search2 ) );
		
		// If either of the above responses match, then find the NPC we are talking to
		if( response1 || response2 )
		{
			// If the PC is dead then break out, The dead cannot accept quests
			if( mChar->IsDead() ) 
			{
				CellResponse->PopChar();
				return false;
			}
			if( Npc->IsNpc() )	// If this is an NPC then check it 
			{
				// I WILL TAKE THEE
				// If this is a request for hire and the PC is within range of the NPC and the NPC is waiting for an ESCORT
				if( getCharDist( Npc, mChar ) <= 1 )
				{
					if( response1 && Npc->GetQuestType() == ESCORTQUEST  && Npc->GetFTarg() == INVALIDSERIAL )
					{
						Npc->SetFTarg( calcCharFromSer( mChar->GetSerial() ) );			// Set the NPC to follow the PC
						Npc->SetNpcWander( 1 );			// Set the NPC to wander freely
						Npc->SetSummonTimer( BuildTimeValue( cwmWorldState->ServerData()->GetEscortActiveExpire() ) );			// Set the expire time if nobody excepts the quest
						
						// Send out the rant about accepting the escort
						sprintf( temp, Dictionary->GetEntry( 1294 ), region[Npc->GetQuestDestRegion()]->GetName() );
						npcTalkAll( Npc, temp, false );
						
						// Remove post from message board (Mark for deletion only - will be cleaned during cleanup)
						MsgBoardQuestEscortRemovePost( Npc );
						// Return 1 so that we indicate that we handled the message
						CellResponse->PopChar();
						return true;
					}
					if( response1 && Npc->GetQuestType() == ESCORTQUEST )
						response2 = response1;	// If the current NPC already has an ftarg then respond to query for quest
					
					// DESTINATION
					// If this is a request to find out where a NPC wants to go and the PC is within range of the NPC and the NPC is waiting for an ESCORT
					if( response2 && Npc->GetQuestType() == ESCORTQUEST )
					{
						if( Npc->GetFTarg() == calcCharFromSer( mChar->GetSerial() ) )
						{	// Send out the rant about accepting the escort
							sprintf(temp, Dictionary->GetEntry( 1295 ), region[Npc->GetQuestDestRegion()]->GetName() );
							npcTalkAll( Npc, temp, false );
						}
						else if( Npc->GetFTarg() == INVALIDSERIAL )  // If nobody has been accepted for the quest yet
						{	// Send out the rant about accepting the escort
							sprintf(temp, Dictionary->GetEntry( 1296 ), region[Npc->GetQuestDestRegion()]->GetName() );
							npcTalkAll( Npc, temp, false );
						}
						else // The must be enroute
						{	// Send out a message saying we are already being escorted
							sprintf(temp, Dictionary->GetEntry( 1297 ), region[Npc->GetQuestDestRegion()]->GetName(), chars[Npc->GetFTarg()].GetName());
							npcTalkAll( Npc, temp, false );
						}
						// Return success ( we handled the message )
						CellResponse->PopChar();
						return true;
					}
				}
			}
		}
		
		if( ( Npc->GetNPCAiType()&0x08) == 0x08 && !mChar->IsDead() && getCharDist( Npc, mChar ) <= 6 )	// if he's a banker and we're close!
		{
			strcpy( search1, "BANK" );
			strcpy( search2, "BALANCE" );
			strcpy( search3, cwmWorldState->ServerData()->GetWildernessBankTriggerString() );
			response1 = strstr( comm, search1 );
			response2 = strstr( comm, search2 );
			if( response1 )
				openBank( mSock, mChar );
			// this only if special bank is enabled - AntiChrist
			if( cwmWorldState->ServerData()->GetWildernessBankStatus() )
			{
				response1 = strstr( comm, search3 );
				if( response1 )
					openSpecialBank( mSock, mChar );
			}
			if( response2 )	
			{
				long int goldCount = getBankCount( mChar, 0x0EED );
				sprintf( temp, Dictionary->GetEntry( 1298 ), mChar->GetName(), goldCount );
				npcTalk( mSock, Npc, temp, true );
			}
		}
		
		// This training code is by Anthracks (fred1117@tiac.net) and really psychotic
		// if it doesn't work or you can't decipher it, you know who to blame
		strcpy( search1, Dictionary->GetEntry( 1299 ) );
		strcpy( search2, Dictionary->GetEntry( 1300 ) );
		strcpy( search3, Dictionary->GetEntry( 1301 ) );
		response1 = strstr( comm, search1 );
		response2 = strstr( comm, search2 );
		response3 = strstr( comm, search3 );
		if( cwmWorldState->ServerData()->GetNPCTrainingStatus() && (response1 || response2 || response3) ) //if the player wants to train
		{
			// Stop the NPC from moving for a minute while talking with the player
			Npc->SetNpcMoveTime( BuildTimeValue( 60 ) );
			mChar->SetTrainer( INVALIDSERIAL ); //this is to prevent errors when a player says "train <skill>" then doesn't pay the npc
			skill = -1;
			for( i = 0; i < ALLSKILLS; i++ )
			{
				if( strstr( comm, skillname[i] ) )
				{
					skill = i;  //Leviathan fix
					break;
				}
			}
			if( skill == -1 ) // Didn't ask to be trained in a specific skill - Leviathan fix
			{
				if( mChar->GetTrainer() == INVALIDSERIAL ) //not being trained, asking what skills they can train in
				{
					if( Npc->IsNpc() )
					{
						if( getCharDist( Npc, mChar ) <= 3 && (Npc->GetID() == 0x0190 || Npc->GetID() == 0x0191) )
						{
							if( !Npc->CanTrain() )
							{
								npcTalk( mSock, Npc, 1302, false );
								CellResponse->PopChar();
								return true;
							}
							Npc->SetTrainingPlayerIn( 255 ); // Like above, this is to prevent  errors when a player says "train <skill>" then doesn't pay the npc
							strcpy( temp, Dictionary->GetEntry( 1303 ) );
							for( j = 0; j < ALLSKILLS; j++ )
							{
								if( Npc->GetBaseSkill( j ) > 10 )
								{
									sprintf( temp2, "%s, ", strlwr( skillname[j] ) );
									strupr( skillname[j] ); // I found out strlwr changes the actual  string permanently, so this undoes that
									if( !y ) 
										temp2[0] = toupper( temp2[0] ); // If it's the first skill,  capitalize it.
									strcat( temp, temp2 );
									y++;
								}
							}
							if( y )
							{
								temp[strlen( temp ) - 2] = '.'; // Make last character a . not a ,  just to look nicer
								npcTalk( mSock, Npc, temp, false );
							}
							else
								npcTalk( mSock, Npc, 1302, false );

							CellResponse->PopChar();
							return true;
						}
					}
				} 
			}
			else // They do want to learn a specific skill
			{
				if( Npc->IsNpc() )
				{
					if( getCharDist( Npc, mChar ) <= 3 && ( Npc->GetID() == 0x0190 || Npc->GetID() == 0x0191 ) )
					{
						if( !Npc->CanTrain() )
						{
							npcTalk( mSock, Npc, 1302, false );
							CellResponse->PopChar();
							return true;
						}
						if( Npc->GetBaseSkill( skill ) > 10 )
						{
							sprintf( temp, Dictionary->GetEntry( 1304 ), strlwr( skillname[skill] ) );
							strupr( skillname[skill] ); // I found out strlwr changes the actual string permanently, so this undoes that
							if( mChar->GetBaseSkill( skill ) >= 250 )
								strcat( temp, Dictionary->GetEntry( 1305 ) );
							else
							{
								if( Npc->GetBaseSkill( skill ) <= 250)
									sprintf( temp2, Dictionary->GetEntry( 1306 ),(int)( Npc->GetBaseSkill( skill ) / 2 / 10 ),(int)( Npc->GetBaseSkill( skill ) / 2 ) - mChar->GetBaseSkill( skill ) );
								else
									sprintf( temp2, Dictionary->GetEntry( 1306 ), 25, 250 - mChar->GetBaseSkill( skill ) );
								strcat( temp, temp2 );
								mChar->SetTrainer( Npc->GetSerial() );
								Npc->SetTrainingPlayerIn( skill );
							}
							npcTalk( mSock, Npc, temp, false );
							CellResponse->PopChar();
							return true;
						}
						else
						{
							npcTalk( mSock, Npc, 1307, false ); 
							CellResponse->PopChar();
							return true; 
						}
					}
				}
			}
		}

		if( ( Npc->GetOwnerObj() != NULL ) && ( Npc->GetNPCAiType() != 17 ) && 
			( ( abs( diffX ) <= 7 ) && ( abs( diffY ) <= 7 ) && ( abs( diffZ ) <= 20 ) ) )
		{
			bool isFriend = Npcs->checkPetFriend( mChar, Npc );
			if( Npc->GetOwner() == mChar->GetSerial() || isFriend || mChar->IsGM() )
			{
				char search4[50];
				char *response4;
				strcpy( search4, "ALL" );
				strcpy( search3, Npc->GetName() );
				strupr( search3 );
				response3 = strstr( comm, search3);
				response4 = strstr( comm, search4 );
				if( response3 || response4 ) // If petname or ALL is in the line
				{
					strcpy(search1, Dictionary->GetEntry( 1308 ) ); // Follow
					strcpy(search2, Dictionary->GetEntry( 1309 ) ); // Me
					response1 = strstr( comm, search1 );
					response2 = strstr( comm, search2 );
					if( response1 )
					{
						Npcs->stopPetGuarding( Npc );
						mChar->SetGuarded( false );
						if( response2 ) //if me is in
						{
							Npc->SetFTarg( calcCharFromSer( mChar->GetSerial() ) );
							Npc->SetNpcWander( 1 );
							playMonsterSound( Npc, Npc->GetID(), SND_STARTATTACK );
							retval = true;
						}
						else
						{	// add pet follow code here, look for "all" command
							if( ( FTarg == 0 ) || ( FTarg == INVALIDSERIAL ) )
							{
								mSock->AddID( Npc->GetSerial() );
								target( mSock, 0, 1, 0, 117, 1310 );
								FTarg = Npc->GetFTarg();
							}
							else 
							{
								Npc->SetFTarg( FTarg );
								Npc->SetNpcWander( 1 );
								playMonsterSound( Npc, Npc->GetID(), SND_STARTATTACK );
							}
							retval = true;
						}
						// check if "all" was said... if not => return
						if( !response4 )
						{
							CellResponse->PopChar();
							return true;
						}
					}

					strcpy( search1, Dictionary->GetEntry( 1311 ) ); // Kill
					strcpy( search2, Dictionary->GetEntry( 1312 ) ); // Attack
					response1 = strstr( comm, search1 );
					response2 = strstr( comm, search2 );
					if( response1 || response2 )
					{
						Npcs->stopPetGuarding( Npc );
						if( !isFriend )
						{
							mSock->AddID( Npc->GetSerial() );
							//pet kill code here
							target( mSock, 0, 1, 0, 118, 1313 );
							CellResponse->PopChar();
							return true;
						}
					}

					strcpy( search1, Dictionary->GetEntry( 1314 ) ); // Fetch
					strcpy( search2, Dictionary->GetEntry( 1315 ) ); // Get
					response1 = strstr( comm, search1 );
					response2 = strstr( comm, search2 );
					if( response1 || response2 )
					{
						Npcs->stopPetGuarding( Npc );
						mSock->AddID( Npc->GetSerial() );
						//pet fetch code here
						target( mSock, 0, 1, 0, 120, 1316 );
						CellResponse->PopChar();
						return true;
					}

					strcpy( search1, Dictionary->GetEntry( 1317 ) ); // Come
					response1 = strstr( comm, search1 );
					if( response1 )
					{
						Npcs->stopPetGuarding( Npc );
						Npc->SetFTarg( calcCharFromSer( mChar->GetSerial() ) );
						Npc->SetNpcWander( 1 );
						sysmessage( mSock, 1318 );
						retval = true;
						if( !response4 )
						{
							CellResponse->PopChar();
							return true;
						}
					}
				
					strcpy( search1, Dictionary->GetEntry( 1319 ) ); // Guard
					strcpy( search2, Dictionary->GetEntry( 1320 ) ); // Me
					response1 = strstr( comm, search1 );
					response2 = strstr( comm, search2 );
					if( response1 )
					{
						if( !isFriend )
						{
							Npcs->stopPetGuarding( Npc );
							if( response2 ) //if me is in
							{
								sysmessage( mSock, 1321 );
								Npc->SetNPCAiType( 32 );									// 32 is guard mode
								mChar->SetGuarded( true );
								Npc->SetFTarg( calcCharFromSer( mChar->GetSerial() ) );
								Npc->SetNpcWander( 1 );
								//add pet guard me code here
								CellResponse->PopChar();
								return true;
							}
							else
							{
								mSock->AddID( Npc->GetSerial() );
								// Guard target
								target( mSock, 0, 1, 0, 120, 1104 );
								CellResponse->PopChar();
								return true;
							}
						}
					}

					strcpy( search1, Dictionary->GetEntry( 1619 ) ); // Friend
					response1 = strstr( comm, search1 );
					if( response1 )
					{
						if( !isFriend )
						{
							mSock->AddID( Npc->GetSerial() );
							// Friend target
							target( mSock, 0, 1, 0, 124, 1620 );
							CellResponse->PopChar();
							return true;
						}
					}

					strcpy( search1, Dictionary->GetEntry( 1626 ) ); // Stop
					strcpy( search2, Dictionary->GetEntry( 1627 ) ); // Stay
					response1 = strstr( comm, search1 );
					response2 = strstr( comm, search2 );
					if( response1 || response2 )
					{
						Npcs->stopPetGuarding( Npc );
						//pet stop code here
						Npc->SetFTarg( INVALIDSERIAL );
						Npc->SetTarg( INVALIDSERIAL );
						if( Npc->IsAtWar() ) 
							npcToggleCombat( Npc );
						Npc->SetNpcWander( 0 );
						retval = true;
						if( !response4 )
						{
							CellResponse->PopChar();
							return true;
						}
					}

					strcpy( search1, Dictionary->GetEntry( 1322 ) ); // Transfer
					response1 = strstr( comm, search1 );
					if( response1 && !response4 )
					{
						Npcs->stopPetGuarding( Npc );
						//pet transfer code here
						mSock->AddID( Npc->GetSerial() );
						target( mSock, 0, 1, 0, 119, 1323 );
						CellResponse->PopChar();
						return true;
					}

					strcpy( search1, Dictionary->GetEntry( 1324 ) ); // Release
					response1 = strstr( comm, search1 );
					if( response1 && !response4 )
					{
						Npcs->stopPetGuarding( Npc );
						//pet release code here
						Npc->SetFTarg( INVALIDSERIAL );
						Npc->SetNpcWander( 2 );
						Npc->SetOwner( INVALIDSERIAL );
						sprintf(temp, Dictionary->GetEntry( 1325 ), Npc->GetName() );
						npcTalkAll( Npc, temp, false );
						if( Npc->GetSummonTimer() )
						{
							soundeffect( &chars[i], 0x01FE );
							Npcs->DeleteChar( Npc );
						}
						CellResponse->PopChar();
						return true;
					}
				}
			}
		}
	}
	CellResponse->PopChar();
	} // end of for loops
	return retval;
}

void responsevendor( cSocket *mSock )
{
	char buffer1[MAXBUFFER];
	int i;
	char *comm;
	
	char *response1 = NULL;
	char *response2 = NULL;
	char *response3 = NULL;
	char *response4 = NULL;
	
	char search1[50];
	char search2[50];
	char search3[50];
	char search4[50];
	
	char nonuni[512];
	CChar *mChar = mSock->CurrcharObj();
	UI08 *tbuffer = mSock->TBuffer();
	SI16 x = mChar->GetX(), y = mChar->GetY();
	
	if( mChar->isUnicode() )
		for( i = 13; i < mSock->GetWord( 1 ); i += 2 )
			nonuni[(i-13)/2] = mSock->GetByte( i );
		
	if( !mChar->isUnicode() )
	{
		for( i = 7; i < MAXBUFFER; i++ )
		{
			tbuffer[i] = buffer1[i];
			buffer1[i] = toupper( mSock->GetByte( i ) );
		}
	}
	else
	{
		for( i = 0; i < MAXBUFFER-8; i++ )
		{
			tbuffer[i+8] = buffer1[i+8];
			buffer1[i+8] = toupper( nonuni[i] );
		}
	}
	
	comm = &buffer1[8];

	SubRegion *toSearch = NULL;
	CChar *NpcSearch = NULL;
	
	strcpy( search1, Dictionary->GetEntry( 1330 ) );
	strcpy( search2, Dictionary->GetEntry( 1331 ) );
	strcpy( search4, Dictionary->GetEntry( 1332 ) );
	response1 = strstr( comm, search1 );
	response2 = strstr( comm, search2 );
	response4 = strstr( comm, search4 );
	
	UI08 worldNumber = mChar->WorldNumber();
	if( response4 )
	{
		toSearch = MapRegion->GetCell( x, y, worldNumber );
		if( !toSearch ) 
			return;
		toSearch->PushChar();
		for( NpcSearch = toSearch->FirstChar(); !toSearch->FinishedChars(); NpcSearch = toSearch->GetNextChar() )
		{
			if( mChar == NpcSearch )
				continue;
			if( !NpcSearch->IsNpc() && !isOnline( NpcSearch ) )
				continue;
			NpcSearch->SetNpcMoveTime( BuildTimeValue( 60 ) );
			strcpy( search3, NpcSearch->GetName() );
			strupr( search3 );
			response3 = strstr( comm, search3 );
			if( response3 || response2 || response1 )
			{
				if( abs( mChar->GetX() - NpcSearch->GetX() ) <= 3 &&
					abs( mChar->GetY() - NpcSearch->GetY() ) <= 3 &&
					abs( mChar->GetZ() - NpcSearch->GetZ() ) <= 5 )
				{ //PlayerVendors
					if( NpcSearch->GetNPCAiType() == 17 )
					{
						mSock->AddX( calcCharFromSer( NpcSearch->GetSerial() ) );
						npcTalk( mSock, NpcSearch, 1333, false );
						target( mSock, 0, 1, 0, 224, " ");
						break;
					} 
					else if( Targ->BuyShop( mSock, NpcSearch ) ) 
						break;
				}
			}
		}
		toSearch->PopChar();
	}
	
	
	//PlayerVendors
	response4 = 0;
	strcpy( search4, Dictionary->GetEntry( 1334 ) );
	response4 = strstr( comm, search4 );
	if( response4 )//AntiChrist
	{
		toSearch = MapRegion->GetCell( x, y, worldNumber );
		if( !toSearch ) 
			return;
		toSearch->PushChar();
		for( NpcSearch = toSearch->FirstChar(); !toSearch->FinishedChars(); NpcSearch = toSearch->GetNextChar() )
		{
			if( mChar == NpcSearch )
				continue;
			if( !NpcSearch->IsNpc() && !isOnline( NpcSearch ) )
				continue;
			NpcSearch->SetNpcMoveTime( BuildTimeValue( 60 ) );
			strcpy( search3, NpcSearch->GetName() );
			strupr( search3 );
			response3 = strstr( comm, search3 );
			if( response3 || response2 || response1 )
			{
				if( abs( mChar->GetX() - NpcSearch->GetX() ) <= 3 &&
					abs( mChar->GetY() - NpcSearch->GetY() ) <= 3 &&
					abs( mChar->GetZ() - NpcSearch->GetZ() ) <= 5 )
				{ //PlayerVendors
					if( NpcSearch->GetNPCAiType() == 17 )
					{
						PlVGetgold( mSock, NpcSearch );
						toSearch->PopChar();
						break;
					}
				}
			}
		}
		toSearch->PopChar();
	}
	//end PlayerVendors --^
	
	response4 = 0;
	strcpy( search4, Dictionary->GetEntry( 1335 ) );
	response4 = strstr( comm, search4 );
	if( response4 )//AntiChrist
	{
		toSearch = MapRegion->GetCell( x, y, worldNumber );
		if( !toSearch ) 
			return;
		toSearch->PushChar();
		for( NpcSearch = toSearch->FirstChar(); !toSearch->FinishedChars(); NpcSearch = toSearch->GetNextChar() )
		{
			if( mChar == NpcSearch )
				continue;
			if( !NpcSearch->IsNpc() && !isOnline( NpcSearch ) )
				continue;
			NpcSearch->SetNpcMoveTime( BuildTimeValue( 60 ) );
			strcpy( search3, NpcSearch->GetName() );
			strupr( search3 );
			response3 = strstr( comm, search3 );
			if( response3 || response2 || response1 )
			{
				if( abs( mChar->GetX() - NpcSearch->GetX() ) <= 3 &&
					abs( mChar->GetY() - NpcSearch->GetY() ) <= 3 &&
					abs( mChar->GetZ() - NpcSearch->GetZ() ) <= 5 )
				{ //PlayerVendors
					// Stop the NPC from moving for a minute while talking with the player
					NpcSearch->SetNpcMoveTime( BuildTimeValue( 60 ) );
					sendSellStuff( mSock, NpcSearch );
					break;
				}
			}
		}
		toSearch->PopChar();
	}
	return;
}


