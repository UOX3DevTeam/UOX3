#include "uox3.h"

// Unknown bytes
// 5->8
// 18->27
// 30->36

void cPCharLocBody::InternalReset( void )
{
	internalBuffer.resize( 37 );
	internalBuffer[0] = 0x1B;
	for( UI08 k = 5; k < 9; k++ )
		internalBuffer[k] = 0;
	for( UI08 i = 18; i < 28; i++ )
		internalBuffer[i] = 0;
	for( UI08 j = 30; j < 37; j++ )
		internalBuffer[j] = 0;
	HighlightColour( 0 );
}
cPCharLocBody::cPCharLocBody()
{
	InternalReset();
}

cPCharLocBody::cPCharLocBody( CChar &toCopy )
{
	InternalReset();
	CopyData( toCopy );
}

void cPCharLocBody::CopyData( CChar &toCopy )
{
	PlayerID( toCopy.GetSerial() );
	BodyType( toCopy.GetID() );
	X( toCopy.GetX() );
	Y( toCopy.GetY() );
	Z( toCopy.GetZ() );
	Direction( toCopy.GetDir() );
}

cPCharLocBody::~cPCharLocBody()
{
}

cPCharLocBody &cPCharLocBody::operator=( CChar &toCopy )
{
	CopyData( toCopy );
	return (*this);
}
void cPCharLocBody::PlayerID( SERIAL toPut )
{
	internalBuffer[1] = (UI08)(toPut>>24);
	internalBuffer[2] = (UI08)(toPut>>16);
	internalBuffer[3] = (UI08)(toPut>>8);
	internalBuffer[4] = (UI08)(toPut%256);
}
void cPCharLocBody::BodyType( UI16 toPut )
{
	internalBuffer[9] = (UI08)(toPut>>8);
	internalBuffer[10] = (UI08)(toPut%256);
}
void cPCharLocBody::X( UI16 toPut )
{
	internalBuffer[11] = (UI08)(toPut>>8);
	internalBuffer[12] = (UI08)(toPut%256);
}
void cPCharLocBody::Y( UI16 toPut )
{
	internalBuffer[13] = (UI08)(toPut>>8);
	internalBuffer[14] = (UI08)(toPut%256);
}
void cPCharLocBody::Z( UI16 toPut )
{
	internalBuffer[15] = (UI08)(toPut>>8);
	internalBuffer[16] = (UI08)(toPut%256);
}
void cPCharLocBody::Direction( UI08 toPut )
{
	internalBuffer[17] = toPut;
}
void cPCharLocBody::Flag( UI08 toPut )
{
	internalBuffer[28] = toPut;
}
void cPCharLocBody::HighlightColour( UI08 color )
{
	internalBuffer[29] = color;
}

CPacketSpeech::CPacketSpeech( CSpeechEntry &toCopy )
{
	internalBuffer.resize( 44 );
	internalBuffer[0] = 0x1C;
	CopyData( toCopy );
}

void CPacketSpeech::CopyData( CSpeechEntry &toCopy )
{
	Speech( toCopy.Speech() );
	SpeakerSerial( toCopy.Speaker() );
	Colour( toCopy.Colour() );
	Font( toCopy.Font() );
	Language( toCopy.Language() );
	Unicode( toCopy.Unicode() );
	Type( toCopy.Type() );
	if( strlen( toCopy.SpeakerName() ) != 0 )
		SpeakerName( toCopy.SpeakerName() );
	switch( toCopy.SpkrType() )
	{
	case SPK_UNKNOWN:
	case SPK_SYSTEM:
		SpeakerName( "System" );
		SpeakerModel( INVALIDID );
		if( toCopy.Colour() == 0 )
			Colour( 0x0040 );
		break;
	case SPK_CHARACTER:
		CChar *ourChar;
		ourChar = calcCharObjFromSer( toCopy.Speaker() );
		if( ourChar != NULL )
		{
			SpeakerName( ourChar->GetName() );
			SpeakerModel( ourChar->GetID() );
		}
		else
			SpeakerModel( INVALIDID );
		break;
	case SPK_ITEM:
		CItem *ourItem;
		ourItem = calcItemObjFromSer( toCopy.Speaker() );
		if( ourItem != NULL )
		{
			SpeakerName( ourItem->GetName() );
			SpeakerModel( ourItem->GetID() );
		}
		else
			SpeakerModel( INVALIDID );
		break;
	}
}

CPacketSpeech &CPacketSpeech::operator=( CSpeechEntry &toCopy )
{
	CopyData( toCopy );
	return (*this);
}

CPacketSpeech &CPacketSpeech::operator=( CPacketSpeech &toCopy )
{
	internalBuffer.resize( toCopy.Length() );
	for( UI32 i = 0; i < internalBuffer.size(); i++ )
		internalBuffer[i] = toCopy[i];
	return (*this);
}

CPacketSpeech::~CPacketSpeech()
{
}

CPacketSpeech::CPacketSpeech() : isUnicode( false )
{
	internalBuffer.resize( 44 );	// Minimum packet size
	internalBuffer[0] = 0x1C;
	internalBuffer[1] = 0;
	internalBuffer[2] = 44;
	internalBuffer[3] = 0xFF;
	internalBuffer[4] = 0xFF;
	internalBuffer[5] = 0xFF;
	internalBuffer[6] = 0xFF;
	internalBuffer[7] = 0xFF;
	internalBuffer[8] = 0xFF;

	internalBuffer[14] = 'S';
	internalBuffer[15] = 'y';
	internalBuffer[16] = 's';
	internalBuffer[17] = 't';
	internalBuffer[18] = 'e';
	internalBuffer[19] = 'm';
	internalBuffer[20] = 0;
}

void CPacketSpeech::SpeakerName( const char *toPut )
{
	long len = strlen( toPut );
	if( len >= 30 )
	{
		strncpy( (char *)&internalBuffer[14], toPut, 29 );
		internalBuffer[43] = 0;
	}
	else
	{
		strcpy( (char *)&internalBuffer[14], toPut );
	}
}
void CPacketSpeech::SpeakerSerial( SERIAL toPut )
{
	internalBuffer[3] = (UI08)(toPut>>24);
	internalBuffer[4] = (UI08)(toPut>>16);
	internalBuffer[5] = (UI08)(toPut>>8);
	internalBuffer[6] = (UI08)(toPut%256);
}
void CPacketSpeech::SpeakerModel( UI16 toPut )
{
	internalBuffer[7] = (UI08)(toPut>>8);
	internalBuffer[8] = (UI08)(toPut%256);
}
void CPacketSpeech::Colour( COLOUR toPut )
{
	internalBuffer[10] = (UI08)(toPut>>8);
	internalBuffer[11] = (UI08)(toPut%256);
}
void CPacketSpeech::Font( FontType toPut )
{
	internalBuffer[12] = 0;
	internalBuffer[13] = (UI08)toPut;
}
void CPacketSpeech::Language( UnicodeTypes toPut )
{

}
void CPacketSpeech::Unicode( bool toPut )
{
	isUnicode = toPut;
}
void CPacketSpeech::Type( SpeechType toPut )
{
	internalBuffer[9] = (UI08)toPut;
}
void CPacketSpeech::Speech( const char *toPut )
{
	long len = strlen( toPut );
	const long newLen = 44 + len + 1;
	internalBuffer.resize( 44 + len + 1 );
	strcpy( (char *)&internalBuffer[44], toPut );
	internalBuffer[1] = (UI08)(newLen>>8);
	internalBuffer[2] = (UI08)(newLen%256);
}

CPWalkDeny::CPWalkDeny()
{
	internalBuffer.resize( 8 );
	internalBuffer[0] = 0x21;
}
void CPWalkDeny::SequenceNumber( char newValue )
{
	internalBuffer[1] = newValue;
}
void CPWalkDeny::X( SI16 newValue )
{
	internalBuffer[2] = (UI08)(newValue>>8);
	internalBuffer[3] = (UI08)(newValue%256);
}
void CPWalkDeny::Y( SI16 newValue )
{
	internalBuffer[4] = (UI08)(newValue>>8);
	internalBuffer[5] = (UI08)(newValue%256);
}
void CPWalkDeny::Z( SI08 newValue )
{
	internalBuffer[7] = newValue;
}
void CPWalkDeny::Direction( char newValue )
{
	internalBuffer[6] = newValue;
}

CPWalkOK::CPWalkOK()
{
	internalBuffer.resize( 3 );
	internalBuffer[0] = 0x22;
}
void CPWalkOK::SequenceNumber( char newValue )
{
	internalBuffer[1] = newValue;
}
void CPWalkOK::OtherByte( char newValue )
{
	internalBuffer[2] = newValue;
}

CPExtMove::CPExtMove()
{
	internalBuffer.resize( 17 );
	internalBuffer[0] = 0x77;
}

CPExtMove::CPExtMove( CChar &toCopy )
{
	internalBuffer.resize( 17 );
	internalBuffer[0] = 0x77;
	CopyData( toCopy );
}

CPExtMove &CPExtMove::operator=( CChar &toCopy )
{
	CopyData( toCopy );
	return (*this);
}

void CPExtMove::Serial( SERIAL newSerial )
{
	internalBuffer[1] = (UI08)(newSerial>>24);
	internalBuffer[2] = (UI08)(newSerial>>16);
	internalBuffer[3] = (UI08)(newSerial>>8);
	internalBuffer[4] = (UI08)(newSerial%256);
}
void CPExtMove::ID( UI16 bodyID )
{
	internalBuffer[5] = (UI08)(bodyID>>8);
	internalBuffer[6] = (UI08)(bodyID%256);
}
void CPExtMove::X( SI16 newValue )
{
	internalBuffer[7] = (UI08)(newValue>>8);
	internalBuffer[8] = (UI08)(newValue%256);
}
void CPExtMove::Y( SI16 newValue )
{
	internalBuffer[9] = (UI08)(newValue>>8);
	internalBuffer[10] = (UI08)(newValue%256);
}
void CPExtMove::Z( SI08 newValue )
{
	internalBuffer[11] = newValue;
}
void CPExtMove::Direction( char newValue )
{
	internalBuffer[12] = newValue;
}
void CPExtMove::Colour( UI16 colourID )
{
	internalBuffer[13] = (UI08)(colourID>>8);
	internalBuffer[14] = (UI08)(colourID%256);
}
void CPExtMove::FlagColour( char newValue )
{
	internalBuffer[16] = newValue;
}
void CPExtMove::Flag( char newValue )
{
	internalBuffer[15] = newValue;
}

void CPExtMove::CopyData( CChar &toCopy )
{
	Serial( toCopy.GetSerial() );
	ID( toCopy.GetID() );
	X( toCopy.GetX() );
	Y( toCopy.GetY() );
	Z( toCopy.GetDispZ() );
	Direction( toCopy.GetDir() );
	Colour( toCopy.GetSkin() );
}

void CPAttackOK::CopyData( CChar &toCopy )
{
	Serial( toCopy.GetSerial() );
}
CPAttackOK::CPAttackOK()
{
	internalBuffer.resize( 5 );
	internalBuffer[0] = 0xAA;
}
CPAttackOK::CPAttackOK( CChar &toCopy )
{
	internalBuffer.resize( 5 );
	internalBuffer[0] = 0xAA;
	CopyData( toCopy );
}
void CPAttackOK::Serial( SERIAL newSerial )
{
	internalBuffer[1] = (UI08)(newSerial>>24);
	internalBuffer[2] = (UI08)(newSerial>>16);
	internalBuffer[3] = (UI08)(newSerial>>8);
	internalBuffer[4] = (UI08)(newSerial%256);
}
CPAttackOK &CPAttackOK::operator=( CChar &toCopy )
{
	CopyData( toCopy );
	return (*this);
}


void CPRemoveItem::CopyData( cBaseObject &toCopy )
{
	Serial( toCopy.GetSerial() );
}
CPRemoveItem::CPRemoveItem()
{
	internalBuffer.resize( 5 );
	internalBuffer[0] = 0x1D;
}
CPRemoveItem::CPRemoveItem( cBaseObject &toCopy )
{
	internalBuffer.resize( 5 );
	internalBuffer[0] = 0x1D;
	CopyData( toCopy );
}
void CPRemoveItem::Serial( SERIAL newSerial )
{
	internalBuffer[1] = (UI08)(newSerial>>24);
	internalBuffer[2] = (UI08)(newSerial>>16);
	internalBuffer[3] = (UI08)(newSerial>>8);
	internalBuffer[4] = (UI08)(newSerial%256);
}
CPRemoveItem &CPRemoveItem::operator=( cBaseObject &toCopy )
{
	CopyData( toCopy );
	return (*this);
}


CPWorldChange::CPWorldChange()
{
	internalBuffer.resize( 3 );
	internalBuffer[0] = 0xBC;
}
CPWorldChange::CPWorldChange( WorldType newSeason, UI08 newCursor )
{
	internalBuffer.resize( 3 );
	internalBuffer[0] = 0xBC;
	Season( newSeason );
	Cursor( newCursor );
}
void CPWorldChange::Season( WorldType newSeason )
{
	internalBuffer[1] = (UI08)newSeason;
}
void CPWorldChange::Cursor( UI08 newCursor )
{
	internalBuffer[2] = newCursor;
}


CPLightLevel::CPLightLevel()
{
	internalBuffer.resize( 2 );
	internalBuffer[0] = 0x4F;
}
CPLightLevel::CPLightLevel( UI08 level )
{
	internalBuffer.resize( 2 );
	internalBuffer[0] = 0x4F;
	Level( level );
}

void CPLightLevel::Level( UI08 level )
{
	internalBuffer[1] = level;
}

void CPUpdIndSkill::InternalReset( void )
{
	internalBuffer.resize( 11 );
	internalBuffer[0] = 0x3A;
	internalBuffer[1] = 0x00; // Length of message
	internalBuffer[2] = 0x0B; // Length of message
	internalBuffer[3] = 0xFF; // single entry
	internalBuffer[4] = 0x00;
}
void CPUpdIndSkill::CopyData( CChar& i, UI08 sNum )
{
	SkillNum( sNum );
	Skill( i.GetSkill( sNum ) );
	BaseSkill( i.GetBaseSkill( sNum ) );
	Lock( i.GetSkillLock( sNum ) );
}
CPUpdIndSkill::CPUpdIndSkill()
{
	InternalReset();
}
CPUpdIndSkill::CPUpdIndSkill( CChar& i, UI08 sNum )
{
	InternalReset();
	CopyData( i, sNum );
}
void CPUpdIndSkill::Character( CChar& i, UI08 sNum )
{
	CopyData( i, sNum );
}
void CPUpdIndSkill::SkillNum( UI08 sNum )
{
	internalBuffer[5] = sNum;
}
void CPUpdIndSkill::Skill( SI16 skillval )
{
	internalBuffer[6] = (UI08)(skillval>>8);
	internalBuffer[7] = (UI08)(skillval%256);
}
void CPUpdIndSkill::BaseSkill( SI16 skillval )
{
	internalBuffer[8] = (UI08)(skillval>>8);
	internalBuffer[9] = (UI08)(skillval%256);
}
void CPUpdIndSkill::Lock( UI08 lockVal )
{
	internalBuffer[10] = lockVal;
}

void CPClearMsg::CopyData( cBaseObject &i )
{
	Serial( i.GetSerial() );
}
CPClearMsg::CPClearMsg()
{
	InternalReset();
}
CPClearMsg::CPClearMsg( cBaseObject &i )
{
	InternalReset();
	CopyData( i );
}
CPClearMsg &CPClearMsg::operator=( cBaseObject &toCopy )
{
	CopyData( toCopy );
	return (*this);
}
void CPClearMsg::Serial( SERIAL toSet )
{
	internalBuffer[3] = (UI08)(toSet>>24);
	internalBuffer[4] = (UI08)(toSet>>16);
	internalBuffer[5] = (UI08)(toSet>>8);
	internalBuffer[6] = (UI08)(toSet%256);
}

void CPClearMsg::InternalReset( void )
{
	internalBuffer.resize( 8 );
	internalBuffer[0] = 0x3B;
	internalBuffer[1] = 0x00;
	internalBuffer[2] = 0x08;
	internalBuffer[7] = 0x00;
}
const long loopbackIP = (127<<24) + 1;
CPRelay::CPRelay()
{
	InternalReset();
}
CPRelay::CPRelay( long newIP )
{
	InternalReset();
	ServerIP( newIP );
}
CPRelay::CPRelay( long newIP, SI16 newPort )
{
	InternalReset();
	ServerIP( newIP );
	Port( newPort );
}
void CPRelay::ServerIP( long newIP )
{
	internalBuffer[1] = (UI08)(newIP>>24);
	internalBuffer[2] = (UI08)(newIP>>16);
	internalBuffer[3] = (UI08)(newIP>>8);
	internalBuffer[4] = (UI08)(newIP%256);
}
void CPRelay::Port( SI16 newPort )
{
	internalBuffer[5] = (UI08)(newPort>>8);
	internalBuffer[6] = (UI08)(newPort%256);
}
void CPRelay::SeedIP( long newIP )
{
	internalBuffer[7] = (UI08)(newIP>>24);
	internalBuffer[8] = (UI08)(newIP>>16);
	internalBuffer[9] = (UI08)(newIP>>8);
	internalBuffer[10] = (UI08)(newIP%256);
}
void CPRelay::InternalReset( void )
{
	internalBuffer.resize( 11 );
	internalBuffer[0] = 0x8C;
	SeedIP( loopbackIP );
}


CPWornItem::CPWornItem()
{
	internalBuffer.resize( 15 );
	internalBuffer[0] = 0x2E;
	internalBuffer[7] = 0x00;
}
void CPWornItem::ItemSerial( SERIAL itemSer )
{
	internalBuffer[1] = (UI08)(itemSer>>24);
	internalBuffer[2] = (UI08)(itemSer>>16);
	internalBuffer[3] = (UI08)(itemSer>>8);
	internalBuffer[4] = (UI08)(itemSer%256);
}
void CPWornItem::Model( SI16 newModel )
{
	internalBuffer[5] = (UI08)(newModel>>8);
	internalBuffer[6] = (UI08)(newModel%256);
}
void CPWornItem::Layer( UI08 layer )
{
	internalBuffer[8] = layer;
}
void CPWornItem::CharSerial( SERIAL chSer )
{
	internalBuffer[9] = (UI08)(chSer>>24);
	internalBuffer[10] = (UI08)(chSer>>16);
	internalBuffer[11] = (UI08)(chSer>>8);
	internalBuffer[12] = (UI08)(chSer%256);
}
void CPWornItem::Colour( SI16 newColour )
{
	internalBuffer[13] = (UI08)(newColour>>8);
	internalBuffer[14] = (UI08)(newColour%256);
}

CPWornItem::CPWornItem( CItem &toCopy )
{
	internalBuffer.resize( 15 );
	internalBuffer[0] = 0x2E;
	internalBuffer[7] = 0x00;
	CopyData( toCopy );
}

void CPWornItem::CopyData( CItem &toCopy )
{
	ItemSerial( toCopy.GetSerial() );
	Colour( toCopy.GetColour() );
	Model( toCopy.GetID() );
	Layer( toCopy.GetLayer() );
	CharSerial( toCopy.GetCont() );
}
CPWornItem &CPWornItem::operator=( CItem &toCopy )
{
	CopyData( toCopy );
	return (*this);
}

void CPCharacterAnimation::CopyData( CChar &toCopy )
{
	Serial( toCopy.GetSerial() );
}
CPCharacterAnimation::CPCharacterAnimation()
{
	InternalReset();
}
CPCharacterAnimation::CPCharacterAnimation( CChar &toCopy )
{
	InternalReset();
	CopyData( toCopy );
}
void CPCharacterAnimation::Serial( SERIAL toSet )
{
	internalBuffer[1] = (UI08)(toSet>>24);
	internalBuffer[2] = (UI08)(toSet>>16);
	internalBuffer[3] = (UI08)(toSet>>8);
	internalBuffer[4] = (UI08)(toSet%256);
}
void CPCharacterAnimation::Action( SI16 model )
{
	internalBuffer[5] = (UI08)(model>>8);
	internalBuffer[6] = (UI08)(model%256);
}
void CPCharacterAnimation::Direction( UI08 dir )
{
	internalBuffer[8] = dir;
}
void CPCharacterAnimation::Repeat( SI16 repeatValue )
{
	internalBuffer[9] = (UI08)(repeatValue>>8);
	internalBuffer[10] = (UI08)(repeatValue%256);
}
void CPCharacterAnimation::DoBackwards( bool newValue )
{
	internalBuffer[11] = (UI08)((newValue?1:0));
}
void CPCharacterAnimation::RepeatFlag( bool newValue )
{
	internalBuffer[12] = (UI08)((newValue?1:0));
}
void CPCharacterAnimation::FrameDelay( UI08 delay )
{
	internalBuffer[13] = delay;
}
CPCharacterAnimation &CPCharacterAnimation::operator=( CChar &toCopy )
{
	CopyData( toCopy );
	return (*this);
}
void CPCharacterAnimation::InternalReset( void )
{
	internalBuffer.resize( 14 );
	internalBuffer[0] = 0x6E;
	internalBuffer[7] = 0;
	Repeat( 1 );
	DoBackwards( false );
	RepeatFlag( false );
	FrameDelay( 0 );
}

void CPDrawGamePlayer::CopyData( CChar &toCopy )
{
	Serial( toCopy.GetSerial() );
	Model( toCopy.GetID() );
	Colour( toCopy.GetColour() );
	X( toCopy.GetX() );
	Y( toCopy.GetY() );
	Z( toCopy.GetZ() );
	Direction( toCopy.GetDir() );
	UI08 flag = 0;
	if( toCopy.GetPoisoned() )
		flag |= 0x04;
	if( toCopy.GetHidden() )
		flag |= 0x80;
	Flag( flag );
}
void CPDrawGamePlayer::InternalReset( void )
{
	internalBuffer.resize( 19 );
	internalBuffer[0] = 0x20;
	internalBuffer[7] = 0;
	internalBuffer[15] = 0;
	internalBuffer[16] = 0;
}
CPDrawGamePlayer::CPDrawGamePlayer()
{
	InternalReset();
}
CPDrawGamePlayer::CPDrawGamePlayer( CChar &toCopy )
{
	InternalReset();
	CopyData( toCopy );
}
void CPDrawGamePlayer::Serial( SERIAL toSet )
{
	internalBuffer[1] = (UI08)(toSet>>24);
	internalBuffer[2] = (UI08)(toSet>>16);
	internalBuffer[3] = (UI08)(toSet>>8);
	internalBuffer[4] = (UI08)(toSet%256);
}
void CPDrawGamePlayer::Model( SI16 toSet )
{
	internalBuffer[5] = (UI08)(toSet>>8);
	internalBuffer[6] = (UI08)(toSet%256);
}
void CPDrawGamePlayer::Colour( SI16 toSet )
{
	internalBuffer[8] = (UI08)(toSet>>8);
	internalBuffer[9] = (UI08)(toSet%256);
}
void CPDrawGamePlayer::Flag( UI08 toSet )
{
	internalBuffer[10] = toSet;
}
void CPDrawGamePlayer::X( SI16 toSet )
{
	internalBuffer[11] = (UI08)(toSet>>8);
	internalBuffer[12] = (UI08)(toSet%256);
}
void CPDrawGamePlayer::Y( SI16 toSet )
{
	internalBuffer[13] = (UI08)(toSet>>8);
	internalBuffer[14] = (UI08)(toSet%256);
}
void CPDrawGamePlayer::Direction( UI08 toSet )
{
	internalBuffer[17] = toSet;
}
void CPDrawGamePlayer::Z( SI08 toSet )
{
	internalBuffer[18] = toSet;
}
CPDrawGamePlayer &CPDrawGamePlayer::operator=( CChar &toCopy )
{
	CopyData( toCopy );
	return (*this);
}

void CPPersonalLightLevel::CopyData( CChar &toCopy )
{
	Serial( toCopy.GetSerial() );
}
void CPPersonalLightLevel::InternalReset( void )
{
	internalBuffer.resize( 6 );
	internalBuffer[0] = 0x4E;
}
CPPersonalLightLevel::CPPersonalLightLevel()
{
	InternalReset();
}
CPPersonalLightLevel::CPPersonalLightLevel( CChar &toCopy )
{
	InternalReset();
	CopyData( toCopy );
}
void CPPersonalLightLevel::Serial( SERIAL toSet )
{
	internalBuffer[1] = (UI08)(toSet>>24);
	internalBuffer[2] = (UI08)(toSet>>16);
	internalBuffer[3] = (UI08)(toSet>>8);
	internalBuffer[4] = (UI08)(toSet%256);
}
void CPPersonalLightLevel::Level( UI08 lightLevel )
{
	internalBuffer[5] = lightLevel;
}

CPPersonalLightLevel &CPPersonalLightLevel::operator=( CChar &toCopy )
{
	CopyData( toCopy );
	return (*this);
}

CPPlaySoundEffect::CPPlaySoundEffect()
{
	InternalReset();
}

void CPPlaySoundEffect::Mode( UI08 mode )
{
	internalBuffer[1] = mode;
}
void CPPlaySoundEffect::Model( SI16 newModel )
{
	internalBuffer[2] = (UI08)(newModel>>8);
	internalBuffer[3] = (UI08)(newModel%256);
}
void CPPlaySoundEffect::X( SI16 xLoc )
{
	internalBuffer[6] = (UI08)(xLoc>>8);
	internalBuffer[7] = (UI08)(xLoc%256);
}
void CPPlaySoundEffect::Y( SI16 yLoc )
{
	internalBuffer[8] = (UI08)(yLoc>>8);
	internalBuffer[9] = (UI08)(yLoc%256);
}
void CPPlaySoundEffect::Z( SI16 zLoc )
{
	internalBuffer[10] = (UI08)(zLoc>>8);
	internalBuffer[11] = (UI08)(zLoc%256);
}
void CPPlaySoundEffect::InternalReset( void )
{
	internalBuffer.resize( 12 );
	internalBuffer[0] = 0x54;
	internalBuffer[1] = 1;
	internalBuffer[4] = 0;
	internalBuffer[5] = 0;
}
CPPlaySoundEffect &CPPlaySoundEffect::operator=( cBaseObject &toCopy )
{
	CopyData( toCopy );
	return (*this);
}

void CPPlaySoundEffect::CopyData( cBaseObject &toCopy )
{
	X( toCopy.GetX() );
	Y( toCopy.GetY() );
	Z( toCopy.GetZ() );
}

CPPlaySoundEffect::CPPlaySoundEffect( cBaseObject &toCopy )
{
	InternalReset();
	CopyData( toCopy );
}

void CPPaperdoll::CopyData( CChar &toCopy )
{
	Serial( toCopy.GetSerial() );
}
void CPPaperdoll::InternalReset( void )
{
	internalBuffer.resize( 66 );
	internalBuffer[0] = 0x88;
}
CPPaperdoll::CPPaperdoll()
{
	InternalReset();
}
CPPaperdoll::CPPaperdoll( CChar &toCopy )
{
	InternalReset();
	CopyData( toCopy );
}
void CPPaperdoll::Serial( SERIAL tSerial )
{
	internalBuffer[1] = (UI08)(tSerial>>24);
	internalBuffer[2] = (UI08)(tSerial>>16);
	internalBuffer[3] = (UI08)(tSerial>>8);
	internalBuffer[4] = (UI08)(tSerial%256);
}
void CPPaperdoll::FlagByte( UI08 fVal )
{
	internalBuffer[65] = fVal;
}
void CPPaperdoll::Text( const char *toPut )
{
	if( strlen( toPut ) > 60 )
	{
		strncpy( (char *)&internalBuffer[5], toPut, 59 );
		internalBuffer[64] = 0;
	}
	else
		strcpy( (char *)&internalBuffer[5], toPut );
}
CPPaperdoll &CPPaperdoll::operator=( CChar &toCopy )
{
	CopyData( toCopy );
	return (*this);
}

void CPWeather::InternalReset( void )
{
	internalBuffer.resize( 4 );
	internalBuffer[0] = 0x65;
	internalBuffer[1] = 0xFF;
	internalBuffer[2] = 0x40;
	internalBuffer[3] = 0x10;
}
CPWeather::CPWeather()
{
	InternalReset();
}
CPWeather::CPWeather( UI08 nType )
{
	InternalReset();
	Type( nType );
}
CPWeather::CPWeather( UI08 nType, UI08 nParts )
{
	InternalReset();
	Type( nType );
	Particles( nParts );
}
CPWeather::CPWeather( UI08 nType, UI08 nParts, UI08 nTemp )
{
	InternalReset();
	Type( nType );
	Particles( nParts );
	Temperature( nTemp );
}
void CPWeather::Type( UI08 nType )
{
	internalBuffer[1] = nType;
}
void CPWeather::Particles( UI08 nParts )
{
	internalBuffer[2] = nParts;
}
void CPWeather::Temperature( UI08 nTemp )
{
	internalBuffer[3] = nTemp;
}

void CPGraphicalEffect::InternalReset( void )
{
	internalBuffer.resize( 28 );
	internalBuffer[0] = 0x70;
	internalBuffer[24] = 0;
	internalBuffer[25] = 0;
}
CPGraphicalEffect::CPGraphicalEffect( UI08 effectType )
{
	InternalReset();
	Effect( effectType );
}
void CPGraphicalEffect::Effect( UI08 effectType )
{
	internalBuffer[1] = effectType;
}
void CPGraphicalEffect::SourceSerial( cBaseObject &toSet )
{
	SourceSerial( toSet.GetSerial() );
}
void CPGraphicalEffect::SourceSerial( SERIAL toSet )
{
	internalBuffer[2] = (UI08)(toSet>>24);
	internalBuffer[3] = (UI08)(toSet>>16);
	internalBuffer[4] = (UI08)(toSet>>8);
	internalBuffer[5] = (UI08)(toSet%256);
}
void CPGraphicalEffect::TargetSerial( cBaseObject &toSet )
{
	TargetSerial( toSet.GetSerial() );
}
void CPGraphicalEffect::TargetSerial( SERIAL toSet )
{
	internalBuffer[6] = (UI08)(toSet>>24);
	internalBuffer[7] = (UI08)(toSet>>16);
	internalBuffer[8] = (UI08)(toSet>>8);
	internalBuffer[9] = (UI08)(toSet%256);
}
void CPGraphicalEffect::Model( SI16 nModel )
{
	internalBuffer[10] = (UI08)(nModel>>8);
	internalBuffer[11] = (UI08)(nModel%256);
}
void CPGraphicalEffect::X( SI16 nX )
{
	internalBuffer[12] = (UI08)(nX>>8);
	internalBuffer[13] = (UI08)(nX%256);
}
void CPGraphicalEffect::Y( SI16 nY )
{
	internalBuffer[14] = (UI08)(nY>>8);
	internalBuffer[15] = (UI08)(nY%256);
}
void CPGraphicalEffect::Z( SI08 nZ )
{
	internalBuffer[16] = nZ;
}
void CPGraphicalEffect::XTrg( SI16 nX )
{
	internalBuffer[17] = (UI08)(nX>>8);
	internalBuffer[18] = (UI08)(nX%256);
}
void CPGraphicalEffect::YTrg( SI16 nY )
{
	internalBuffer[19] = (UI08)(nY>>8);
	internalBuffer[20] = (UI08)(nY%256);
}
void CPGraphicalEffect::ZTrg( SI08 nZ )
{
	internalBuffer[21] = nZ;
}
void CPGraphicalEffect::Speed( UI08 nSpeed )
{
	internalBuffer[22] = nSpeed;
}
void CPGraphicalEffect::Duration( UI08 nDuration )
{
	internalBuffer[23] = nDuration;
}
void CPGraphicalEffect::AdjustDir( bool nValue )
{
	internalBuffer[26] = (UI08)((nValue?0:1));
}
void CPGraphicalEffect::ExplodeOnImpact( bool nValue )
{
	internalBuffer[27] = (UI08)((nValue?1:0));
}

CPGraphicalEffect::CPGraphicalEffect( UI08 effectType, cBaseObject &src, cBaseObject &trg )
{
	InternalReset();
	Effect( effectType );
	SourceSerial( src );
	TargetSerial( trg );
}
CPGraphicalEffect::CPGraphicalEffect( UI08 effectType, cBaseObject &src )
{
	InternalReset();
	Effect( effectType );
	SourceSerial( src );
}

void CPGraphicalEffect::SourceLocation( cBaseObject& toSet )
{
	SourceLocation( toSet.GetX(), toSet.GetY(), toSet.GetZ() );
}
void CPGraphicalEffect::TargetLocation( cBaseObject& toSet )
{
	TargetLocation( toSet.GetX(), toSet.GetY(), toSet.GetZ() );
}

void CPGraphicalEffect::SourceLocation( SI16 x, SI16 y, SI08 z )
{
	X( x );
	Y( y );
	Z( z );
}
void CPGraphicalEffect::TargetLocation( SI16 x, SI16 y, SI08 z )
{
	XTrg( x );
	YTrg( y );
	ZTrg( z );
}


void CPUpdateStat::InternalReset( void )
{
	internalBuffer.resize( 9 );
	internalBuffer[0] = 0xA1;
}
CPUpdateStat::CPUpdateStat( CChar &toUpdate, UI08 statNum )
{
	InternalReset();
	Serial( toUpdate.GetSerial() );
	switch( statNum )
	{
	case 0:	MaxVal( toUpdate.GetStrength() );
			CurVal( toUpdate.GetHP() );
			break;
	case 2:	MaxVal( toUpdate.GetDexterity() );
			CurVal( toUpdate.GetStamina() );
			break;
	case 1:	MaxVal( toUpdate.GetIntelligence() );
			CurVal( toUpdate.GetMana() );
			break;
	}
	internalBuffer[0] += (UI08)(statNum);
}
void CPUpdateStat::Serial( SERIAL toSet )
{
	internalBuffer[1] = (UI08)(toSet>>24);
	internalBuffer[2] = (UI08)(toSet>>16);
	internalBuffer[3] = (UI08)(toSet>>8);
	internalBuffer[4] = (UI08)(toSet%256);
}
void CPUpdateStat::MaxVal( SI16 maxVal )
{
	internalBuffer[5] = (UI08)(maxVal>>8);
	internalBuffer[6] = (UI08)(maxVal%256);
}
void CPUpdateStat::CurVal( SI16 curVal )
{
	internalBuffer[7] = (UI08)(curVal>>8);
	internalBuffer[8] = (UI08)(curVal%256);
}

void CPDeathAction::InternalReset( void )
{
	internalBuffer.resize( 13 );
	internalBuffer[0] = 0xAF;
	internalBuffer[9] = 0;
	internalBuffer[10] = 0;
	internalBuffer[11] = 0;
}
CPDeathAction::CPDeathAction( CChar &dying, CItem &corpse )
{
	InternalReset();
	Player( dying.GetSerial() );
	Corpse( corpse.GetSerial() );
}
CPDeathAction::CPDeathAction()
{
	InternalReset();
}
void CPDeathAction::Player( SERIAL toSet )
{
	internalBuffer[1] = (UI08)(toSet>>24);
	internalBuffer[2] = (UI08)(toSet>>16);
	internalBuffer[3] = (UI08)(toSet>>8);
	internalBuffer[4] = (UI08)(toSet%256);
}
void CPDeathAction::Corpse( SERIAL toSet )
{
	internalBuffer[5] = (UI08)(toSet>>24);
	internalBuffer[6] = (UI08)(toSet>>16);
	internalBuffer[7] = (UI08)(toSet>>8);
	internalBuffer[8] = (UI08)(toSet%256);
}
void CPDeathAction::FallDirection( UI08 toFall )
{
	internalBuffer[12] = toFall;
}
CPDeathAction &CPDeathAction::operator=( CChar &dying )
{
	Player( dying.GetSerial() );
	return (*this);
}
CPDeathAction &CPDeathAction::operator=( CItem &corpse )
{
	Corpse( corpse.GetSerial() );
	return (*this);
}

void CPPlayMusic::InternalReset( void )
{
	internalBuffer.resize( 3 );
	internalBuffer[0] = 0x6D;
}
CPPlayMusic::CPPlayMusic( SI16 musicID )
{
	InternalReset();
	MusicID( musicID );
}
CPPlayMusic::CPPlayMusic()
{
	InternalReset();
}
void CPPlayMusic::MusicID( SI16 musicID )
{
	internalBuffer[1] = (UI08)(musicID>>8);
	internalBuffer[2] = (UI08)(musicID%256);
}

void CPDrawContainer::InternalReset( void )
{
	internalBuffer.resize( 7 );
	internalBuffer[0] = 0x24;
}
CPDrawContainer::CPDrawContainer()
{
	InternalReset();
}
CPDrawContainer::CPDrawContainer( CItem &toCopy )
{
	InternalReset();
	CopyData( toCopy );
}
void CPDrawContainer::Model( SI16 newModel )
{
	internalBuffer[5] = (UI08)(newModel>>8);
	internalBuffer[6] = (UI08)(newModel%256);
}
CPDrawContainer &CPDrawContainer::operator=( CItem &toCopy )
{
	CopyData( toCopy );
	return (*this);
}
void CPDrawContainer::CopyData( CItem &toCopy )
{
	Serial( toCopy.GetSerial() );
}
void CPDrawContainer::Serial( SERIAL toSet )
{
	internalBuffer[1] = (UI08)(toSet>>24);
	internalBuffer[2] = (UI08)(toSet>>16);
	internalBuffer[3] = (UI08)(toSet>>8);
	internalBuffer[4] = (UI08)(toSet%256);
}

CPTargetCursor::CPTargetCursor()
{
	internalBuffer.resize( 19 );
	internalBuffer[0] = 0x6C;
	CursorType( 0 );
}
void CPTargetCursor::Type( UI08 nType )
{
	internalBuffer[1] = nType;
}
void CPTargetCursor::ID( SERIAL toSet )
{
	internalBuffer[2] = (UI08)(toSet>>24);
	internalBuffer[3] = (UI08)(toSet>>16);
	internalBuffer[4] = (UI08)(toSet>>8);
	internalBuffer[5] = (UI08)(toSet%256);
}
void CPTargetCursor::CursorType( UI08 nType )
{
	internalBuffer[6] = nType;
}

void CPStatWindow::CopyData( CChar &toCopy )
{
	Serial( toCopy.GetSerial() );
	Name( toCopy.GetName() );
	bool isDead = toCopy.IsDead();
	if( isDead )
	{
		CurrentHP( 0 );
		MaxHP( 0 );
	}
	else
	{
		CurrentHP( toCopy.GetHP() );
		MaxHP( toCopy.GetStrength() );
	}
	NameChange( false );
	Flag( 1 );
	if( toCopy.GetID() == 0x0190 || toCopy.GetID() == 0x0192 )
		Sex( 0 );
	else
		Sex( 1 );
	if( isDead )
	{
		Strength( 0 );
		Dexterity( 0 );
		Intelligence( 0 );
		Stamina( 0 );
		MaxStamina( 0 );
		Mana( 0 );
		MaxMana( 0 );
	}
	else
	{
		Strength( toCopy.GetStrength() );
		Dexterity( toCopy.GetDexterity() );
		Intelligence( toCopy.GetIntelligence() );
		Stamina( toCopy.GetStamina() );
		MaxStamina( toCopy.GetDexterity() );
		Mana( toCopy.GetMana() );
		MaxMana( toCopy.GetIntelligence() );
	}
}
void CPStatWindow::InternalReset( void )
{
	internalBuffer.resize( 66 );
	internalBuffer[0] = 0x11;
	internalBuffer[1] = 0;
	internalBuffer[2] = 0x42;
	Flag( 0 );
}
CPStatWindow::CPStatWindow()
{
	InternalReset();
}
CPStatWindow::CPStatWindow( CChar &toCopy )
{
	InternalReset();
	CopyData( toCopy );
}
void CPStatWindow::Serial( SERIAL toSet )
{
	internalBuffer[3] = (UI08)(toSet>>24);
	internalBuffer[4] = (UI08)(toSet>>16);
	internalBuffer[5] = (UI08)(toSet>>8);
	internalBuffer[6] = (UI08)(toSet%256);
}
void CPStatWindow::Name( const char *nName )
{
	if( strlen( nName ) >= 30 )
	{
		strncpy( (char *)&internalBuffer[7], nName, 29 );
		internalBuffer[36] = 0;
	}
	else
		strcpy( (char*)&internalBuffer[7], nName );
}
void CPStatWindow::CurrentHP( SI16 nValue )
{
	internalBuffer[37] = (UI08)(nValue>>8);
	internalBuffer[38] = (UI08)(nValue%256);
}
void CPStatWindow::MaxHP( SI16 nValue )
{
	internalBuffer[39] = (UI08)(nValue>>8);
	internalBuffer[40] = (UI08)(nValue%256);
}
void CPStatWindow::NameChange( bool nValue )
{
	internalBuffer[41] = (UI08)((nValue?0xFF:0));
}
void CPStatWindow::Flag( UI08 nValue )
{
	internalBuffer[42] = nValue;
}
void CPStatWindow::Sex( UI08 nValue )
{
	internalBuffer[43] = nValue;
}
void CPStatWindow::Strength( SI16 nValue )
{
	internalBuffer[44] = (UI08)(nValue>>8);
	internalBuffer[45] = (UI08)(nValue%256);
}
void CPStatWindow::Dexterity( SI16 nValue )
{
	internalBuffer[46] = (UI08)(nValue>>8);
	internalBuffer[47] = (UI08)(nValue%256);
}
void CPStatWindow::Intelligence( SI16 nValue )
{
	internalBuffer[48] = (UI08)(nValue>>8);
	internalBuffer[49] = (UI08)(nValue%256);
}
void CPStatWindow::Stamina( SI16 nValue )
{
	internalBuffer[50] = (UI08)(nValue>>8);
	internalBuffer[51] = (UI08)(nValue%256);
}
void CPStatWindow::MaxStamina( SI16 nValue )
{
	internalBuffer[52] = (UI08)(nValue>>8);
	internalBuffer[53] = (UI08)(nValue%256);
}
void CPStatWindow::Mana( SI16 nValue )
{
	internalBuffer[54] = (UI08)(nValue>>8);
	internalBuffer[55] = (UI08)(nValue%256);
}
void CPStatWindow::MaxMana( SI16 nValue )
{
	internalBuffer[56] = (UI08)(nValue>>8);
	internalBuffer[57] = (UI08)(nValue%256);
}
void CPStatWindow::Gold( long gValue )
{
	internalBuffer[58] = (UI08)(gValue>>24);
	internalBuffer[59] = (UI08)(gValue>>16);
	internalBuffer[60] = (UI08)(gValue>>8);
	internalBuffer[61] = (UI08)(gValue%256);
}
void CPStatWindow::AC( SI16 nValue )
{
	internalBuffer[62] = (UI08)(nValue>>8);
	internalBuffer[63] = (UI08)(nValue%256);
}
void CPStatWindow::Weight( SI16 nValue )
{
	internalBuffer[64] = (UI08)(nValue>>8);
	internalBuffer[65] = (UI08)(nValue%256);
}
CPStatWindow & CPStatWindow::operator=( CChar &toCopy )
{
	CopyData( toCopy );
	return (*this);
}

void CPIdleWarning::InternalReset( void )
{
	internalBuffer.resize( 2 );
	internalBuffer[0] = 0x53;
}
CPIdleWarning::CPIdleWarning()
{
	InternalReset();
}
CPIdleWarning::CPIdleWarning( UI08 errorNum )
{
	InternalReset();
	Error( errorNum );
}
void CPIdleWarning::Error( UI08 errorNum )
{
	internalBuffer[1] = errorNum;
}


CPTime::CPTime()
{
	InternalReset();
}
void CPTime::Hour( UI08 hour )
{
	internalBuffer[1] = hour;
}
void CPTime::Minute( UI08 minute )
{
	internalBuffer[2] = minute;
}
void CPTime::Second( UI08 second )
{
	internalBuffer[3] = second;
}
void CPTime::InternalReset( void )
{
	internalBuffer.resize( 4 );
	internalBuffer[0] = 0x5B;
}
CPTime::CPTime( UI08 hour, UI08 minute, UI08 second )
{
	InternalReset();
	Hour( hour );
	Minute( minute );
	Second( second );
}

CPLoginComplete::CPLoginComplete()
{
	internalBuffer.resize( 1 );
	internalBuffer[0] = 0x55;
}


CPTextEmoteColour::CPTextEmoteColour()
{
	BlockSize( 5 );
}
void CPTextEmoteColour::BlockSize( SI16 newValue )
{
	internalBuffer.resize( newValue );
	internalBuffer[0] = 0x69;
	internalBuffer[1] = (UI08)(newValue>>8);
	internalBuffer[2] = (UI08)(newValue%256);
}
void CPTextEmoteColour::Unknown( SI16 newValue )
{
	internalBuffer[3] = (UI08)(newValue>>8);
	internalBuffer[4] = (UI08)(newValue%256);
}


CPWarMode::CPWarMode()
{
	InternalReset();
}

void CPWarMode::Flag( UI08 nFlag )
{
	internalBuffer[1] = nFlag;
}
void CPWarMode::InternalReset( void )
{
	internalBuffer.resize( 5 );
	internalBuffer[0] = 0x72;
	internalBuffer[2] = 0x00;
	internalBuffer[3] = 0x32;
	internalBuffer[4] = 0x00;
}
CPWarMode::CPWarMode( UI08 nFlag )
{
	InternalReset();
	Flag( nFlag );
}


void CPPauseResume::InternalReset( void )
{
	internalBuffer.resize( 2 );
	internalBuffer[0] = 0x33;
}
CPPauseResume::CPPauseResume()
{
	InternalReset();
}
CPPauseResume::CPPauseResume( UI08 mode )
{
	InternalReset();
	Mode( mode );
}
void CPPauseResume::Mode( UI08 mode )
{
	internalBuffer[1] = mode;
}

void CPWebLaunch::InternalReset( void )
{
	internalBuffer.resize( 4 );
	internalBuffer[0] = 0xA5;
}
CPWebLaunch::CPWebLaunch()
{
	InternalReset();
}
CPWebLaunch::CPWebLaunch( const char *txt )
{
	InternalReset();
	Text( txt );
}
void CPWebLaunch::Text( const char *txt )
{
	const SI16 tLen = (SI16)strlen( txt )+4;
	SetSize( tLen );
	strcpy( (char *)&internalBuffer[3], txt );
}

void CPWebLaunch::SetSize( SI16 newSize )
{
	internalBuffer.resize( newSize );
	internalBuffer[1] = (UI08)(newSize>>8);
	internalBuffer[2] = (UI08)(newSize%256);
}

void CPTrackingArrow::InternalReset( void )
{
	internalBuffer.resize( 6 );
	internalBuffer[0] = 0xBA;
}
CPTrackingArrow::CPTrackingArrow()
{
	InternalReset();
}
CPTrackingArrow::CPTrackingArrow( SI16 x, SI16 y )
{
	InternalReset();
	Location( x, y );
}
void CPTrackingArrow::Location( SI16 x, SI16 y )
{
	internalBuffer[2] = (UI08)(x>>8);
	internalBuffer[3] = (UI08)(x%256);
	internalBuffer[4] = (UI08)(y>>8);
	internalBuffer[5] = (UI08)(y%256);
}
CPTrackingArrow &CPTrackingArrow::operator=( cBaseObject &toCopy )
{
	Location( toCopy.GetX(), toCopy.GetY() );
	return (*this);
}
void CPTrackingArrow::Active( UI08 value )
{
	internalBuffer[1] = value;
}

CPTrackingArrow::CPTrackingArrow( cBaseObject &toCopy )
{
	InternalReset();
	Location( toCopy.GetX(), toCopy.GetY() );
}

void CPBounce::InternalReset( void )
{
	internalBuffer.resize( 2 );
	internalBuffer[0] = 0x27;
}
CPBounce::CPBounce()
{
	InternalReset();
}
CPBounce::CPBounce( UI08 mode )
{
	InternalReset();
	Mode( mode );
}
void CPBounce::Mode( UI08 mode )
{
	internalBuffer[1] = mode;
}

void CPDyeVat::InternalReset( void )
{
	internalBuffer.resize( 9 );
	internalBuffer[0] = 0x95;
	internalBuffer[5] = 0;
	internalBuffer[6] = 0;
}
void CPDyeVat::CopyData( cBaseObject &target )
{
	Serial( target.GetSerial() );
	Model( target.GetID() );
}
CPDyeVat::CPDyeVat()
{
	InternalReset();
}
CPDyeVat::CPDyeVat( cBaseObject &target )
{
	InternalReset();
	CopyData( target );
}
void CPDyeVat::Serial( SERIAL toSet )
{
	internalBuffer[1] = (UI08)(toSet>>24);
	internalBuffer[2] = (UI08)(toSet>>16);
	internalBuffer[3] = (UI08)(toSet>>8);
	internalBuffer[4] = (UI08)(toSet%256);
}
void CPDyeVat::Model( SI16 toSet )
{
	internalBuffer[7] = (UI08)(toSet>>8);
	internalBuffer[8] = (UI08)(toSet%256);
}
CPDyeVat &CPDyeVat::operator=( cBaseObject &target )
{
	CopyData( target );
	return (*this);
}

void CPMultiPlacementView::InternalReset( void )
{
	internalBuffer.resize( 26 );
	internalBuffer[0] = 0x99;
	for( UI08 i = 6; i < 18; i++ )
		internalBuffer[i] = 0;
	for( UI08 k = 20; k < 26; k++ )
		internalBuffer[k] = 0;
}
void CPMultiPlacementView::CopyData( CItem &target )
{
	DeedSerial( target.GetSerial() );
}
CPMultiPlacementView::CPMultiPlacementView()
{
	InternalReset();
}
CPMultiPlacementView::CPMultiPlacementView( CItem &target )
{
	InternalReset();
	CopyData( target );
}
void CPMultiPlacementView::RequestType( UI08 rType )
{
	internalBuffer[1] = rType;
}
void CPMultiPlacementView::DeedSerial( SERIAL toSet )
{
	internalBuffer[2] = (UI08)(toSet>>24);
	internalBuffer[3] = (UI08)(toSet>>16);
	internalBuffer[4] = (UI08)(toSet>>8);
	internalBuffer[5] = (UI08)(toSet%256);
}
void CPMultiPlacementView::MultiModel( SI16 toSet )
{
	internalBuffer[18] = (UI08)(toSet>>8);
	internalBuffer[19] = (UI08)(toSet%256);
}
CPMultiPlacementView &CPMultiPlacementView::operator=( CItem &target )
{
	CopyData( target );
	return (*this);
}

CPMultiPlacementView::CPMultiPlacementView( SERIAL toSet )
{
	InternalReset();
	DeedSerial( toSet );
}

CPEnableChat::CPEnableChat()
{
	internalBuffer.resize( 3 );
	internalBuffer[0] = 0xB9;
	internalBuffer[1] = 0x00;
	internalBuffer[2] = 0x01;
}

void CPAddItemToCont::InternalReset( void )
{
	internalBuffer.resize( 20 );
	internalBuffer[0] = 0x25;
	internalBuffer[7] = 0;
}
void CPAddItemToCont::CopyData( CItem &toCopy )
{
	Serial( toCopy.GetSerial() );
	Model( toCopy.GetID() );
	NumItems( toCopy.GetAmount() );
	X( toCopy.GetX() );
	Y( toCopy.GetY() );
	Container( toCopy.GetCont() );
	Colour( toCopy.GetColour() );
}
CPAddItemToCont::CPAddItemToCont()
{
	InternalReset();
}
CPAddItemToCont::CPAddItemToCont( CItem &toAdd )
{
	InternalReset();
	CopyData( toAdd );
}
void CPAddItemToCont::Serial( SERIAL toSet )
{
	internalBuffer[1] = (UI08)(toSet>>24);
	internalBuffer[2] = (UI08)(toSet>>16);
	internalBuffer[3] = (UI08)(toSet>>8);
	internalBuffer[4] = (UI08)(toSet%256);
}
void CPAddItemToCont::Model( SI16 toSet )
{
	internalBuffer[5] = (UI08)(toSet>>8);
	internalBuffer[6] = (UI08)(toSet%256);
}
void CPAddItemToCont::NumItems( SI16 toSet )
{
	internalBuffer[8] = (UI08)(toSet>>8);
	internalBuffer[9] = (UI08)(toSet%256);
}
void CPAddItemToCont::X( SI16 x )
{
	internalBuffer[10] = (UI08)(x>>8);
	internalBuffer[11] = (UI08)(x%256);
}
void CPAddItemToCont::Y( SI16 y )
{
	internalBuffer[12] = (UI08)(y>>8);
	internalBuffer[13] = (UI08)(y%256);
}
void CPAddItemToCont::Container( SERIAL toAdd )
{
	internalBuffer[14] = (UI08)(toAdd>>24);
	internalBuffer[15] = (UI08)(toAdd>>16);
	internalBuffer[16] = (UI08)(toAdd>>8);
	internalBuffer[17] = (UI08)(toAdd%256);
}
void CPAddItemToCont::Colour( SI16 toSet )
{
	internalBuffer[18] = (UI08)(toSet>>8);
	internalBuffer[19] = (UI08)(toSet%256);
}
CPAddItemToCont &CPAddItemToCont::operator=( CItem &toAdd )
{
	CopyData( toAdd );
	return (*this);
}

void CPKickPlayer::InternalReset( void )
{
	internalBuffer.resize( 5 );
	// NOTE: The 4 bytes following are NOT known, they are ONLY a guess
	internalBuffer[0] = 0x26;
}
void CPKickPlayer::CopyData( CChar &toCopy )
{
	Serial( toCopy.GetSerial() );
}
CPKickPlayer::CPKickPlayer()
{
	InternalReset();
}
CPKickPlayer::CPKickPlayer( CChar &toCopy )
{
	InternalReset();
	CopyData( toCopy );
}
void CPKickPlayer::Serial( SERIAL toSet )
{
	internalBuffer[1] = (UI08)(toSet>>24);
	internalBuffer[2] = (UI08)(toSet>>16);
	internalBuffer[3] = (UI08)(toSet>>8);
	internalBuffer[4] = (UI08)(toSet%256);
}
CPKickPlayer &CPKickPlayer::operator=( CChar &toCopy )
{
	CopyData( toCopy );
	return (*this);
}


void CPResurrectMenu::InternalReset( void )
{
	internalBuffer.resize( 2 );
	internalBuffer[0] = 0x2C;
}
CPResurrectMenu::CPResurrectMenu()
{
	InternalReset();
}
CPResurrectMenu::CPResurrectMenu( UI08 action )
{
	InternalReset();
	Action( action );
}
void CPResurrectMenu::Action( UI08 action )
{	// valid values 0 == from server, 1 == resurrect, 2 == ghost (server/client job)
	internalBuffer[1] = action;
}

void CPFightOccurring::InternalReset( void )
{
	internalBuffer.resize( 10 );
	internalBuffer[0] = 0x2F;
	internalBuffer[1] = 0;
}
// Sent when fight occuring somewhere on the screen
CPFightOccurring::CPFightOccurring()
{
}
CPFightOccurring::CPFightOccurring( CChar &attacker, CChar &defender )
{
	Attacker( attacker );
	Defender( defender );
}
void CPFightOccurring::Attacker( SERIAL toSet )
{
	internalBuffer[2] = (UI08)(toSet>>24);
	internalBuffer[3] = (UI08)(toSet>>16);
	internalBuffer[4] = (UI08)(toSet>>8);
	internalBuffer[5] = (UI08)(toSet%256);
}
void CPFightOccurring::Attacker( CChar &attacker )
{
	Attacker( attacker.GetSerial() );
}
void CPFightOccurring::Defender( SERIAL toSet )
{
	internalBuffer[6] = (UI08)(toSet>>24);
	internalBuffer[7] = (UI08)(toSet>>16);
	internalBuffer[8] = (UI08)(toSet>>8);
	internalBuffer[9] = (UI08)(toSet%256);
}
void CPFightOccurring::Defender( CChar &defender )
{
	Defender( defender.GetSerial() );
}

void CPSkillsValues::InternalReset( void )
{
	internalBuffer.resize( 1 );
	internalBuffer[0] = 0x3A;
}
void CPSkillsValues::CopyData( CChar &toCopy )
{
	NumSkills( TRUESKILLS );
	for( int i = 0; i < TRUESKILLS; i++ )
		SkillEntry( i, toCopy.GetSkill( i ), toCopy.GetBaseSkill( i ), toCopy.GetSkillLock( i ) );
}
void CPSkillsValues::BlockSize( SI16 newValue )
{
	internalBuffer.resize( newValue );
	internalBuffer[1] = (UI08)(newValue>>8);
	internalBuffer[2] = (UI08)(newValue%256);
	internalBuffer[3] = 0;	// full list
	internalBuffer[4] = 0x00;
	internalBuffer[newValue-1] = 0;	// finish off with a double NULL
	internalBuffer[newValue-2] = 0;
}
CPSkillsValues::CPSkillsValues()
{
	InternalReset();
}
CPSkillsValues::CPSkillsValues( CChar &toCopy )
{
	InternalReset();
	CopyData( toCopy );
}
void CPSkillsValues::NumSkills( UI08 numSkills )
{
	BlockSize( numSkills * 7 + 6 );
}
void CPSkillsValues::SkillEntry( SI16 skillID, SI16 skillVal, SI16 baseSkillVal, UI08 skillLock )
{
	int offset = skillID * 7 + 5;
	internalBuffer[offset] = skillID + 1;
	WriteShort( offset + 1, skillVal );
	WriteShort( offset + 3, baseSkillVal );
	internalBuffer[offset + 5] = skillLock;
}
CPSkillsValues &CPSkillsValues::operator=( CChar &toCopy )
{
	CopyData( toCopy );
	return (*this);
}

void CPSkillsValues::WriteShort( SI16 offset, SI16 value )
{
	internalBuffer[offset] = (UI08)(value>>8);
	internalBuffer[offset+1] = (UI08)(value%256);
}

CPMapMessage::CPMapMessage()
{
	internalBuffer.resize( 19 );
	internalBuffer[0] = 0x90;
	GumpArt( 0x139D );
}
void CPMapMessage::UpperLeft( SI16 x, SI16 y )
{
	internalBuffer[7] = (UI08)(x>>8);
	internalBuffer[8] = (UI08)(x%256);
	internalBuffer[9] = (UI08)(y>>8);
	internalBuffer[10] = (UI08)(y%256);
}
void CPMapMessage::LowerRight( SI16 x, SI16 y )
{
	internalBuffer[11] = (UI08)(x>>8);
	internalBuffer[12] = (UI08)(x%256);
	internalBuffer[13] = (UI08)(y>>8);
	internalBuffer[14] = (UI08)(y%256);
}
void CPMapMessage::Dimensions( SI16 width, SI16 height )
{
	internalBuffer[15] = (UI08)(width>>8);
	internalBuffer[16] = (UI08)(width%256);
	internalBuffer[17] = (UI08)(height>>8);
	internalBuffer[18] = (UI08)(height%256);
}
void CPMapMessage::GumpArt( SI16 newArt )
{
	internalBuffer[5] = (UI08)(newArt>>8);
	internalBuffer[6] = (UI08)(newArt%256);
}
void CPMapMessage::KeyUsed( long key )
{
	internalBuffer[1] = (UI08)(key>>24);
	internalBuffer[2] = (UI08)(key>>16);
	internalBuffer[3] = (UI08)(key>>8);
	internalBuffer[4] = (UI08)(key%256);
}


CPBookTitlePage::CPBookTitlePage()
{
	internalBuffer.resize( 99 );
	internalBuffer[0] = 0x93;
}
void CPBookTitlePage::Serial( SERIAL toSet )
{
	internalBuffer[1] = (UI08)(toSet>>24);
	internalBuffer[2] = (UI08)(toSet>>16);
	internalBuffer[3] = (UI08)(toSet>>8);
	internalBuffer[4] = (UI08)(toSet%256);
}
void CPBookTitlePage::WriteFlag( UI08 flag )
{
	internalBuffer[5] = flag;
}
void CPBookTitlePage::NewFlag( UI08 flag )
{
	internalBuffer[6] = flag;
}
void CPBookTitlePage::Pages( SI16 pages )
{
	internalBuffer[7] = (UI08)(pages>>8);
	internalBuffer[8] = (UI08)(pages%256);
}
void CPBookTitlePage::Title( const char *txt )
{
	if( strlen( txt ) >= 60 )
	{
		strncpy( (char *)&internalBuffer[9], txt, 59 );
		internalBuffer[68] = 0;
	}
	else
		strcpy( (char *)&internalBuffer[9], txt );
}
void CPBookTitlePage::Author( const char *txt )
{
	if( strlen( txt ) >= 30 )
	{
		strncpy( (char *)&internalBuffer[69], txt, 29 );
		internalBuffer[98] = 0;
	}
	else
		strcpy( (char *)&internalBuffer[69], txt );
}


CPUltimaMessenger::CPUltimaMessenger()
{
	internalBuffer.resize( 9 );
	internalBuffer[0] = 0xBB;
}
void CPUltimaMessenger::ID1( SERIAL toSet )
{
	internalBuffer[1] = (UI08)(toSet>>24);
	internalBuffer[2] = (UI08)(toSet>>16);
	internalBuffer[3] = (UI08)(toSet>>8);
	internalBuffer[4] = (UI08)(toSet%256);
}
void CPUltimaMessenger::ID2( SERIAL toSet )
{
	internalBuffer[5] = (UI08)(toSet>>24);
	internalBuffer[6] = (UI08)(toSet>>16);
	internalBuffer[7] = (UI08)(toSet>>8);
	internalBuffer[8] = (UI08)(toSet%256);
}

cPInputBuffer *WhichPacket( UI08 packetID )
{
	switch( packetID )
	{
	case 0x80:	return ( new CPIFirstLogin() );
	case 0xA0:	return ( new CPIServerSelect() );
	case 0x91:	return ( new CPISecondLogin() );
	case 0xBD:	return ( new CPIClientVersion() );
	case 0xC8:	return ( new CPIUpdateRangeChange() );
	default:	return NULL;
	}
	return NULL;
}

bool CPIFirstLogin::Handle( void )
{
	Network->Login1( tSock );
	return true;
}

CPIFirstLogin::CPIFirstLogin()
{
	InternalReset();
}
CPIFirstLogin::CPIFirstLogin( cSocket *s )
{
	InternalReset();
	Receive( s );
}
void CPIFirstLogin::Receive( cSocket *s )
{
	s->FirstPacket( false );
	s->Receive( 62 );
	memcpy( &internalBuffer[0], s->Buffer(), 62 );
}
CPIFirstLogin &CPIFirstLogin::operator=( cSocket &s )
{
	Receive( &s );
	return (*this);
}
void CPIFirstLogin::InternalReset( void )
{
	internalBuffer.resize( 62 );
	internalBuffer[0] = 0x80;
}

const char *CPIFirstLogin::Name( void )
{
	return (const char *)&internalBuffer[1];
}
const char *CPIFirstLogin::Pass( void )
{
	return (const char *)&internalBuffer[31];
}
UI08 CPIFirstLogin::Unknown( void )
{
	return internalBuffer[61];
}

void CPIServerSelect::InternalReset( void )
{
	internalBuffer.resize( 3 );
	internalBuffer[0] = 0xA0;
}
CPIServerSelect::CPIServerSelect()
{
	InternalReset();
}
CPIServerSelect::CPIServerSelect( cSocket *s )
{
	Receive( s );
}
void CPIServerSelect::Receive( cSocket *s )
{
	s->Receive( 3 );
	memcpy( &internalBuffer[0], s->Buffer(), 3 );
}
CPIServerSelect &CPIServerSelect::operator=( cSocket &s )
{
	Receive( &s );
	return (*this);
}
SI16 CPIServerSelect::ServerNum( void )
{
	// EviLDeD - Sept 19, 2002
	// Someone said that there was an issue with False logins that request server 0. Default to server 1.
	SI16 temp = ( (internalBuffer[1]<<8) + internalBuffer[2] );
	if(temp==0)
		return 1;
	else
		return temp;
}

bool CPIServerSelect::Handle( void )
{
	UI16 selectedServer = ServerNum() - 1;
	physicalServer *tServer = cwmWorldState->ServerData()->GetServerEntry( selectedServer );
	UI32 ip = htonl( inet_addr( tServer->getIP().c_str() ) );
	CPRelay toSend( ip, tServer->getPort() );
	tSock->Send( &toSend );
	return true;
}

void CPISecondLogin::InternalReset( void )
{
	internalBuffer.resize( 65 );
	internalBuffer[0] = 0x91;
}
CPISecondLogin::CPISecondLogin()
{
	InternalReset();
}
CPISecondLogin::CPISecondLogin( cSocket *s )
{
	InternalReset();
	Receive( s );
}
void CPISecondLogin::Receive( cSocket *s )
{
	s->FirstPacket( false );
	s->Receive( 65 );
	s->CryptClient( true );
	memcpy( &internalBuffer[0], s->Buffer(), 65 );
}
CPISecondLogin &CPISecondLogin::operator=( cSocket &s )
{
	Receive( &s );
	return (*this);
}
long CPISecondLogin::Account( void )
{
	return DWord( 1 );
}
const char *CPISecondLogin::Name( void )
{
	return (const char *)&internalBuffer[5];
}
const char *CPISecondLogin::Pass( void )
{
	return (const char *)&internalBuffer[35];
}

bool CPISecondLogin::Handle( void )
{
	LoginDenyReason t = LDR_NODENY;
	tSock->AcctNo( -1 );
	ACTREC *ourAccount = Accounts->GetAccount( (char *)&tSock->Buffer()[5] );	// oops, my fault, it SHOULD be 5
	if( ourAccount != NULL )
		tSock->AcctObj( ourAccount );
	if( tSock->AcctNo() != -1 )
	{
		Network->pSplit( (char *)&tSock->Buffer()[35] );
		if( ourAccount->lpaarHolding->bFlags&0x01 )	// no point verifying if we're banned
			t = LDR_ACCOUNTDISABLED;
		else if( strcmp( pass1, ourAccount->lpaarHolding->Info.password ) )	// non matching password
			t = LDR_UNKNOWNUSER;
		if( t == LDR_NODENY )
			Network->GoodAuth( tSock, ourAccount );
		else
		{
			tSock->AcctNo( -1 );
			CPLoginDeny toSend( t );
			tSock->Send( &toSend );
			Network->LoginDisconnect( tSock );
		}
	}
	else
	{
		CPLoginDeny noUser( LDR_UNKNOWNUSER );
		tSock->Send( &noUser );
		Network->LoginDisconnect( tSock );
	}
	return true;
}

void CPGumpTextEntry::InternalReset( void )
{
	BlockSize( 3 );
	internalBuffer[0] = 0xAB;
}
CPGumpTextEntry::CPGumpTextEntry()
{
	InternalReset();
}
CPGumpTextEntry::CPGumpTextEntry( const char *text )
{
	InternalReset();
	Text1( text );
}
CPGumpTextEntry::CPGumpTextEntry( const char *text1, const char *text2 )
{
	InternalReset();
	Text1( text1 );
	Text2( text2 );
}
void CPGumpTextEntry::Serial( SERIAL id )
{
	internalBuffer[3] = (UI08)(id>>24);
	internalBuffer[4] = (UI08)(id>>16);
	internalBuffer[5] = (UI08)(id>>8);
	internalBuffer[6] = (UI08)(id%256);
}
void CPGumpTextEntry::ParentID( UI08 newVal )
{
	internalBuffer[7] = newVal;
}
void CPGumpTextEntry::ButtonID( UI08 newVal )
{
	internalBuffer[8] = newVal;
}
void CPGumpTextEntry::Cancel( UI08 newVal )
{
	SI16 t1Len = Text1Len();
	internalBuffer[t1Len + 11] = newVal;
}
void CPGumpTextEntry::Style( UI08 newVal )
{
	SI16 t1Len = Text1Len();
	internalBuffer[t1Len + 12] = newVal;
}
void CPGumpTextEntry::Format( SERIAL id )
{
	SI16 t1Len = Text1Len();
	internalBuffer[t1Len + 13] = (UI08)(id>>24);
	internalBuffer[t1Len + 14] = (UI08)(id>>16);
	internalBuffer[t1Len + 15] = (UI08)(id>>8);
	internalBuffer[t1Len + 16] = (UI08)(id%256);
}
void CPGumpTextEntry::Text1( const char *txt )
{
	SI16 sLen = strlen( txt );
	BlockSize( 20 + sLen );	// 11 + 1 + 8
	Text1Len( sLen + 1 );
	strcpy( (char *)&internalBuffer[11], txt );
}
void CPGumpTextEntry::Text2( const char *txt )
{
	SI16 sLen = strlen( txt );
	SI16 currentSize = CurrentSize();
	BlockSize( currentSize + sLen + 1 );
	Text2Len( sLen + 1 );
	strcpy( (char *)&internalBuffer[currentSize], txt );
}
void CPGumpTextEntry::BlockSize( SI16 newVal )
{
	internalBuffer.resize( newVal );
	internalBuffer[0] = 0xAB;
	internalBuffer[1] = (UI08)(newVal>>8);
	internalBuffer[2] = (UI08)(newVal%256);
}

SI16 CPGumpTextEntry::CurrentSize( void )
{
	return (SI16)((internalBuffer[1]<<8) + internalBuffer[2]);
}

SI16 CPGumpTextEntry::Text1Len( void )
{
	return (SI16)((internalBuffer[9]<<8) + internalBuffer[10]);
}

void CPGumpTextEntry::Text1Len( SI16 newVal )
{
	internalBuffer[9] = (UI08)(newVal>>8);
	internalBuffer[10] = (UI08)(newVal%256);
}

SI16 CPGumpTextEntry::Text2Len( void )
{
	SI16 t1Len = Text1Len();
	return (SI16)(( internalBuffer[t1Len + 17]<<8 ) + internalBuffer[t1Len + 18]);
}

void CPGumpTextEntry::Text2Len( SI16 newVal )
{
	SI16 t1Len = Text1Len();
	internalBuffer[t1Len + 17] = (UI08)(newVal>>8);
	internalBuffer[t1Len + 18] = (UI08)(newVal%256);
}

CPGodModeToggle::CPGodModeToggle()
{
	InternalReset();
}
void CPGodModeToggle::InternalReset( void )
{
	internalBuffer.resize( 2 );
	internalBuffer[0] = 0x2B;
}

void CPGodModeToggle::CopyData( cSocket *s )
{
	internalBuffer[1] = s->GetByte( 1 );
}

CPGodModeToggle::CPGodModeToggle( cSocket *s )
{
	InternalReset();
	CopyData( s );
}

CPGodModeToggle& CPGodModeToggle::operator=( cSocket *s )
{
	CopyData( s );
	return (*this);
}

void CPGodModeToggle::ToggleStatus( bool toSet )
{
	if( toSet )
		internalBuffer[1] = 1;
	else
		internalBuffer[1] = 0;
}

void CPLoginDeny::InternalReset( void )
{
	internalBuffer.resize( 2 );
	internalBuffer[0] = 0x82;
}
CPLoginDeny::CPLoginDeny()
{
	InternalReset();
}
CPLoginDeny::CPLoginDeny( LoginDenyReason reason )
{
	InternalReset();
	DenyReason( reason );
}
void CPLoginDeny::DenyReason( LoginDenyReason reason )
{
	internalBuffer[1] = reason;
}

void CPIClientVersion::InternalReset( void )
{
	internalBuffer.resize( 3 );
	internalBuffer[0] = 0xBD;
	len = 0;
}
CPIClientVersion::CPIClientVersion()
{
	InternalReset();
}
CPIClientVersion::CPIClientVersion( cSocket *s )
{
	InternalReset();
	Receive( s );
}
void CPIClientVersion::Receive( cSocket *s )
{
	tSock->Receive( 3 );
	len = tSock->GetWord( 1 );
	tSock->Receive( len );
}
CPIClientVersion &CPIClientVersion::operator=( cSocket *s )
{
	Receive( s );
	return (*this);
}

//o---------------------------------------------------------------------------o
//|   Function    -	UI08 ShiftValue( UI08 toShift, UI08 base, UI08 upper, bool extra)
//|   Date        -	21st November, 2001
//|   Programmer  -	Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -	If the value is between base and upper (inclusive)
//|					it shifts the data down by base (if extra) or base - 1 (if not extra)
//o---------------------------------------------------------------------------o
UI08 ShiftValue( UI08 toShift, UI08 base, UI08 upper, bool extra )
{
	if( extra )
	{
		if( toShift >= base && toShift <= upper )
			toShift = toShift - base;
	}
	else
	{
		if( toShift >= base && toShift <= upper )
			toShift = toShift - base + 1;
	}
	return toShift;
}
bool CPIClientVersion::Handle( void )
{
	char *verString = (char *)&(tSock->Buffer()[3]);
	verString[len] = 0;
	UI08 major, minor, sub, letter;

	string s( verString );
	istringstream ss( s );
	char period;
	ss >> major >> period;
	ss >> minor >> period;
	ss >> sub >> letter;

	major	= ShiftValue( major,  '0', '9', true );
	minor	= ShiftValue( minor,  '0', '9', true );
	sub		= ShiftValue( sub,    '0', '9', true );
	letter	= ShiftValue( letter, 'a', 'z', false );
	letter	= ShiftValue( letter, 'A', 'Z', false );

	tSock->ClientVersion( major, minor, sub, letter );
	if( strstr( verString, "Dawn" ) )
		tSock->ClientType( CV_UO3D );
	else if( strstr( verString, "Krrios" ) )
		tSock->ClientType( CV_KRRIOS );
	return true;
}

void CPIUpdateRangeChange::InternalReset( void )
{
}
CPIUpdateRangeChange::CPIUpdateRangeChange()
{
	InternalReset();
}
CPIUpdateRangeChange::CPIUpdateRangeChange( cSocket *s )
{
	InternalReset();
	Receive( s );
}
void CPIUpdateRangeChange::Receive( cSocket *s )
{
	s->Receive( 2 );
}
CPIUpdateRangeChange &CPIUpdateRangeChange::operator=( cSocket *s )
{
	Receive( s );
	return (*this);
}
bool CPIUpdateRangeChange::Handle( void )
{
	// Byte 0 == 0xC8
	// Byte 1 == 0x0F (by default)  This is the distance, we believe

	// SURPRISE! This value, if used by the server, WILL NOT SHOW THE ITEMS IN A REAL CLIENT!
	// Krrios' client behaves (go Krrios!), but the UO ones don't
	switch( tSock->ClientType() )
	{
	case CV_NORMAL:
	case CV_T2A:
	case CV_UO3D:
		tSock->Range( tSock->GetByte( 1 ) - 3 );
		break;
	default:
		tSock->Range( tSock->GetByte( 1 ) );
		break;
	}
	return true;
}

void CMapChange::InternalReset( void )
{
	internalBuffer.resize( 6 );
	internalBuffer[0] = 0xBF;
	internalBuffer[1] = 0x00;
	internalBuffer[2] = 0x06;
	internalBuffer[3] = 0x00;
	internalBuffer[4] = 0x08;	// set client hue
}
CMapChange::CMapChange()
{
	InternalReset();
}
CMapChange::CMapChange( UI08 newMap )
{
	InternalReset();
	SetMap( newMap );
}
CMapChange::CMapChange( cBaseObject *moving )
{
	InternalReset();
	if( moving != NULL )
		SetMap( moving->WorldNumber() );
}
CMapChange::~CMapChange()
{
}
void CMapChange::SetMap( UI08 newMap )
{
	internalBuffer[5] = newMap;
}
CMapChange& CMapChange::operator=( cBaseObject& moving )
{
	SetMap( moving.WorldNumber() );
	return (*this);
}

void CPItemsInContainer::InternalReset( void )
{
	internalBuffer.resize( 5 );
	internalBuffer[0] = 0x3C;
}
CPItemsInContainer::CPItemsInContainer()
{
	InternalReset();
}
CPItemsInContainer::CPItemsInContainer( CItem *container )
{
	InternalReset();
	if( container != NULL )
		CopyData( (*container) );
}

UI16 CPItemsInContainer::NumberOfItems( void ) const
{
	return ( (internalBuffer[3]<<8) + internalBuffer[4] );
}

void CPItemsInContainer::NumberOfItems( UI16 numItems )
{
	// set the number of items
	internalBuffer[3] = (UI08)(numItems>>8);
	internalBuffer[4] = (UI08)(numItems%256);
	// knowing the number of items, set the packet size
	UI16 packetSize = (UI16)((numItems * 19) + 5);
	internalBuffer.resize( packetSize );
	internalBuffer[1] = (UI08)(packetSize>>8);
	internalBuffer[2] = (UI08)(packetSize%256);
}
void CPItemsInContainer::AddItem( CItem *toAdd, UI16 itemNum )
{
	UI16 baseOffset = (UI16)(5 + itemNum * 19);
	internalBuffer[baseOffset +  0] = toAdd->GetSerial( 1 );
	internalBuffer[baseOffset +  1] = toAdd->GetSerial( 2 );
	internalBuffer[baseOffset +  2] = toAdd->GetSerial( 3 );
	internalBuffer[baseOffset +  3] = toAdd->GetSerial( 4 );

	internalBuffer[baseOffset +  4] = toAdd->GetID( 1 );
	internalBuffer[baseOffset +  5] = toAdd->GetID( 2 );

	internalBuffer[baseOffset +  7] = (UI08)((toAdd->GetAmount())>>8);
	internalBuffer[baseOffset +  8] = (UI08)((toAdd->GetAmount())%256);

	internalBuffer[baseOffset +  9] = (UI08)((toAdd->GetX())>>8);
	internalBuffer[baseOffset + 10] = (UI08)((toAdd->GetX())%256);

	internalBuffer[baseOffset + 11] = (UI08)((toAdd->GetY())>>8);
	internalBuffer[baseOffset + 12] = (UI08)((toAdd->GetY())%256);

	internalBuffer[baseOffset + 13] = toAdd->GetCont( 1 );
	internalBuffer[baseOffset + 14] = toAdd->GetCont( 2 );
	internalBuffer[baseOffset + 15] = toAdd->GetCont( 3 );
	internalBuffer[baseOffset + 16] = toAdd->GetCont( 4 );

	internalBuffer[baseOffset + 17] = toAdd->GetColour( 1 );
	internalBuffer[baseOffset + 18] = toAdd->GetColour( 2 );

	toAdd->SetDecayTime( 0 );
}
CPItemsInContainer& CPItemsInContainer::operator=( CItem& container )
{
	CopyData( container );
	return (*this);
}

void CPItemsInContainer::CopyData( CItem& toCopy )
{
	UI16 itemCount = 0;
	for( CItem *ctr = toCopy.FirstItemObj(); !toCopy.FinishedItems(); ctr = toCopy.NextItemObj() )
	{
		if( ctr != NULL )
		{
			if( !ctr->isFree() )
				itemCount++;
		}
	}
	NumberOfItems( itemCount );
	UI16 iCount = 0;
	for( CItem *ci = toCopy.FirstItemObj(); !toCopy.FinishedItems(); ci = toCopy.NextItemObj() )
	{
		if( ci != NULL )
		{
			if( !ci->isFree() )
			{
				AddItem( ci, iCount );
				iCount++;
			}
		}
	}
}


void CPOpenBuyWindow::InternalReset( void )
{
	internalBuffer.resize( 16384 );	// start big, and work back down
	internalBuffer[0] = 0x74;
	internalBuffer[1] = 0;
	internalBuffer[2] = 0;
	internalBuffer[3] = 0;
	internalBuffer[4] = 0;
	internalBuffer[5] = 0;
	internalBuffer[6] = 0;
	internalBuffer[7] = 0;
}
CPOpenBuyWindow::CPOpenBuyWindow()
{
	InternalReset();
}
CPOpenBuyWindow::CPOpenBuyWindow( CItem *container, CChar *p )
{
	InternalReset();
	VendorID( container );
	if( container != NULL )
		CopyData( (*container), p );
}

UI08 CPOpenBuyWindow::NumberOfItems( void ) const
{
	return internalBuffer[7];
}

void CPOpenBuyWindow::NumberOfItems( UI08 numItems )
{
	// set the number of items
	internalBuffer[7] = numItems;
}
void CPOpenBuyWindow::VendorID( CItem *i )
{
	internalBuffer[3] = i->GetSerial( 1 );// Container object
	internalBuffer[4] = i->GetSerial( 2 );// Container object
	internalBuffer[5] = i->GetSerial( 3 );// Container object
	internalBuffer[6] = i->GetSerial( 4 );// Container object
}
SI16 CPOpenBuyWindow::AddItem( CItem *toAdd, CChar *p, UI16 baseOffset )
{
	UI32 value = calcValue( toAdd, toAdd->GetValue() );
	if( cwmWorldState->ServerData()->GetTradeSystemStatus() )
		value = calcGoodValue( p, toAdd, value, 0 );
	char itemname[MAX_NAME];
	UI08 sLen = getTileName( toAdd, itemname ); // Item name length

	internalBuffer[baseOffset +  0] = (UI08)((toAdd->GetValue())>>24);
	internalBuffer[baseOffset +  1] = (UI08)((toAdd->GetValue())>>16);
	internalBuffer[baseOffset +  2] = (UI08)((toAdd->GetValue())>>8);
	internalBuffer[baseOffset +  3] = (UI08)((toAdd->GetValue())%256);
	internalBuffer[baseOffset +  4] = sLen;

	for( int k = 0; k < sLen; k++ )
		internalBuffer[baseOffset + 5 + k] = itemname[k];
	return (5 + sLen);
}
void CPOpenBuyWindow::Container( CItem *toAdd, CChar *p )
{
	VendorID( toAdd );
	CopyData( (*toAdd), p );
}

void CPOpenBuyWindow::CopyData( CItem& toCopy, CChar *p )
{
	UI08 itemCount = 0;
	for( CItem *ctr = toCopy.FirstItemObj(); !toCopy.FinishedItems(); ctr = toCopy.NextItemObj() )
	{
		if( ctr != NULL )
		{
			if( !ctr->isFree() )
				itemCount++;
		}
	}
	NumberOfItems( itemCount );
	UI16 iCount = 0;
	UI16 length = 8;
	for( CItem *ci = toCopy.FirstItemObj(); !toCopy.FinishedItems(); ci = toCopy.NextItemObj() )
	{
		if( ci != NULL )
		{
			if( !ci->isFree() )
			{
				length += AddItem( ci, p, length );
				iCount++;
			}
		}
	}
	internalBuffer.resize( length );
	internalBuffer[1] = (UI08)( (length)>>8 );
	internalBuffer[2] = (UI08)( (length)%256 );
}

void CPCharAndStartLoc::InternalReset( void )
{
	internalBuffer.resize( 4 );
	internalBuffer[0] = 0xA9;
}
void CPCharAndStartLoc::CopyData( ACTREC& toCopy )
{
	for( UI08 i = 0; i < 5; i++ )
	{
		if( toCopy.characters[i] != NULL )
			AddCharacter( toCopy.characters[i], i );
	}
}
CPCharAndStartLoc::CPCharAndStartLoc()
{
	InternalReset();
}
CPCharAndStartLoc::CPCharAndStartLoc( ACTREC *account, UI08 numCharacters, UI08 numLocations )
{
	InternalReset();
	NumberOfCharacters( numCharacters );
	NumberOfLocations( numLocations );
	CopyData( *account );
}
void CPCharAndStartLoc::NumberOfLocations( UI08 numLocations )
{
	UI16 packetSize = (UI16)(305 + 63 * numLocations);
	internalBuffer.resize( packetSize );
	internalBuffer[1] = (UI08)(packetSize>>8);
	internalBuffer[2] = (UI08)(packetSize%256);
	internalBuffer[304] = numLocations;
}
void CPCharAndStartLoc::NumberOfCharacters( UI08 numCharacters )
{
	internalBuffer[3] = numCharacters;
}
void CPCharAndStartLoc::AddCharacter( CChar *toAdd, UI08 charOffset )
{
	if( toAdd == NULL )
		return;
	UI16 baseOffset = (UI16)(4 + charOffset * 60);
	strncpy( (char *)&internalBuffer[baseOffset], toAdd->GetName(), 29 );
	// extra 30 bytes unused, as we don't use a character password
}
void CPCharAndStartLoc::AddStartLocation( LPSTARTLOCATION sLoc, UI08 locOffset )
{
	if( sLoc == NULL )
		return;
	UI16 baseOffset = (UI16)(305 + locOffset * 63);
	internalBuffer[baseOffset] = locOffset;
	UI32 townLen = strlen( sLoc->town );
	UI32 descLen = strlen( sLoc->description );
	for( UI32 i = 0; i < townLen; i++ )
		internalBuffer[baseOffset+i+1] = sLoc->town[i];
	for( UI32 j = 0; j < descLen; j++ )
		internalBuffer[baseOffset+j+31] = sLoc->town[j];
}

CPCharAndStartLoc& CPCharAndStartLoc::operator=( ACTREC& account )
{
	CopyData( account );
	return (*this);
}

/*
byte Command = 0xF0; // Custom client packet
short Length;
byte Command = 0x00; // Client response
byte Ack;  0x00 : Connection rejected, disconnect
           0x01 : Connection ok, no privs
           0x02 : Connection ok, gm privs
           
*/
CPKAccept::CPKAccept( UI08 Response )
{
	internalBuffer.resize( 5 );
	internalBuffer[0] = 0xF0;
	internalBuffer[1] = 0x00;
	internalBuffer[2] = 0x05; // Length... 5 bytes
	internalBuffer[3] = 0x00; // Client response

	// Ack
	internalBuffer[4] = Response;
}
