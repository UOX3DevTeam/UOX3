#include "uox3.h"

void PackString( UI08 *toPack, int offset, std::string value, int maxLen )
{
	int toWrite = min( maxLen, static_cast<int>(value.length()) );
	for( int i = 0; i < toWrite; i++ )
		toPack[offset+i] = (UI08)value[i];
}
void UnpackString( UI08 *toPack, int offset, std::string &value, int maxLen )
{
	value = "";
	for( int i = 0; i < maxLen; i++ )
	{
		if( toPack[offset + i] != 0 )
			value += (char)(toPack[offset + i]);
	}
}
void PackLong( UI08 *toPack, int offset, SI32 value )
{
	toPack[offset+0] = (UI08)(value>>24);
	toPack[offset+1] = (UI08)(value>>16);
	toPack[offset+2] = (UI08)(value>>8);
	toPack[offset+3] = (UI08)(value%256);
}
SI32 UnpackSLong( UI08 *toPack, int offset )
{
	return (toPack[offset+0]<<24) + (toPack[offset+1]<<16) + (toPack[offset+2]<<8) + toPack[offset+3];
}
UI32 UnpackULong( UI08 *toPack, int offset )
{
	return (UI32)((toPack[offset+0]<<24) + (toPack[offset+1]<<16) + (toPack[offset+2]<<8) + toPack[offset+3]);
}
UI16 UnpackUShort( UI08 *toPack, int offset )
{
	return (UI16)((toPack[offset+0]<<8) + toPack[offset+1]);
}
SI16 UnpackSShort( UI08 *toPack, int offset )
{
	return (SI16)((toPack[offset+0]<<8) + toPack[offset+1]);
}
void PackLong( UI08 *toPack, int offset, UI32 value )
{
	toPack[offset+0] = (UI08)(value>>24);
	toPack[offset+1] = (UI08)(value>>16);
	toPack[offset+2] = (UI08)(value>>8);
	toPack[offset+3] = (UI08)(value%256);
}

void PackShort( UI08 *toPack, int offset, UI16 value )
{
	toPack[offset+0] = (UI08)(value>>8);
	toPack[offset+1] = (UI08)(value%256);
}

void pSplit( char *pass0, char *pass1, char *pass2 ) // Split login password into UOX password and UO password
{
	int i = 0;
	pass1[0] = 0;
	while( pass0[i] != '/' && pass0[i] != 0 ) 
		i++;
	strncpy( pass1, pass0, i );
	pass1[i] = 0;
	if( pass0[i] != 0 ) 
		strcpy( pass2, pass0 + i + 1 );
}

// Unknown bytes
// 5->8
// 18->27
// 30->36

//0x1B Packet
//Last Modified on Friday, 20-Nov-1998
//Char Location and body type (37 bytes) 
//	BYTE cmd 
//	BYTE[4] player id 
//	BYTE[4] unknown1 
//	BYTE[2] bodyType 
//	BYTE[2] xLoc 
//	BYTE[2] yLoc 
//	BYTE[2] zLoc 
//	BYTE direction 
//	BYTE[2] unknown2 
//	BYTE[4] unknown3 (usually has FF somewhere in it) 
//	BYTE[4] unknown4 
//	BYTE flag byte 
//	BYTE highlight color 
//	BYTE[7] unknown5 
//Note: Only send once after login. It’s mandatory to send it once.
//Note2: OSI calls this packet Login confirm

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
	PackLong( &internalBuffer[0], 1, toPut );
}
void cPCharLocBody::BodyType( UI16 toPut )
{
	PackShort( &internalBuffer[0], 9, toPut );
}
void cPCharLocBody::X( UI16 toPut )
{
	PackShort( &internalBuffer[0], 11, toPut );
}
void cPCharLocBody::Y( UI16 toPut )
{
	PackShort( &internalBuffer[0], 13, toPut );
}
void cPCharLocBody::Z( UI16 toPut )
{
	PackShort( &internalBuffer[0], 15, toPut );
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

//0x1C Packet
//Last Modified on Friday, 20-Apr-1998 
//Send Speech (Variable # of bytes) 
//	BYTE cmd 
//	BYTE[2] blockSize 
//	BYTE[4] itemID (FF FF FF FF = system) 
//	BYTE[2] model (item hex # - FF FF = system) 
//	BYTE Type 
//	BYTE[2] Text Color 
//	BYTE[2] Font 
//	BYTE[30] Name 
//	BYTE[?] Null-Terminated Message (? = blockSize - 44) 

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
	PackLong( &internalBuffer[0], 3, toPut );
}
void CPacketSpeech::SpeakerModel( UI16 toPut )
{
	PackShort( &internalBuffer[0], 7, toPut );
}
void CPacketSpeech::Colour( COLOUR toPut )
{
	PackShort( &internalBuffer[0], 10, toPut );
}
void CPacketSpeech::Font( FontType toPut )
{
	internalBuffer[12] = 0;
	internalBuffer[13] = (UI08)toPut;
}
#pragma note( "Function Warning: CPacketSpeech::Language(), does nothing" )
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
	PackShort( &internalBuffer[0], 1, static_cast<UI16>(newLen) );
}

//0x21 Packet
//Last Modified on Wednesday, 06-May-1998 23:30:37 EDT 
//Character Move Reject (8 bytes) 
//	BYTE cmd 
//	BYTE sequence # 
//	BYTE[2] xLoc 
//	BYTE[2] yLoc 
//	BYTE direction 
//	BYTE zLoc 

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
	PackShort( &internalBuffer[0], 2, newValue );
}
void CPWalkDeny::Y( SI16 newValue )
{
	PackShort( &internalBuffer[0], 4, newValue );
}
void CPWalkDeny::Z( SI08 newValue )
{
	internalBuffer[7] = newValue;
}
void CPWalkDeny::Direction( char newValue )
{
	internalBuffer[6] = newValue;
}

//0x22 Packet
//Last Modified on Wednesday, 11-Nov-1998 
//Character Move ACK/ Resync Request(3 bytes) 
//	BYTE cmd 
//	BYTE sequence (matches sent sequence) 
//	BYTE (0x00) 

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

//0x77 Packet
//Last Modified on Sunday, 13-Feb-2000 
//Update Player (17 bytes) 
//	BYTE cmd 
//	BYTE[4] player id 
//	BYTE[2] model
//	BYTE[2] xLoc 
//	BYTE[2] yLoc 
//	BYTE zLoc 
//	BYTE direction 
//	BYTE[2] hue/skin color 
//	BYTE flag (bit field) 
//	BYTE highlight color

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
	PackLong( &internalBuffer[0], 1, newSerial );
}
void CPExtMove::ID( UI16 bodyID )
{
	PackShort( &internalBuffer[0], 5, bodyID );
}
void CPExtMove::X( SI16 newValue )
{
	PackShort( &internalBuffer[0], 7, newValue );
}
void CPExtMove::Y( SI16 newValue )
{
	PackShort( &internalBuffer[0], 9, newValue );
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
	PackShort( &internalBuffer[0], 13, colourID );
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

//0xAA Packet
//Last Modified on Thursday, 16-Apr-1998 19:52:14 EDT 
//OK / Not OK To Attack (5 bytes) 
//	BYTE cmd 
//	BYTE[4] CharID being attacked 
//ID is set to 00 00 00 00 when attack is refused.

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
	PackLong( &internalBuffer[0], 1, newSerial );
}
CPAttackOK &CPAttackOK::operator=( CChar &toCopy )
{
	CopyData( toCopy );
	return (*this);
}

//0x1D Packet
//Last Modified on Monday, 13-Apr-1998 17:06:02 EDT 
//Delete object (5 bytes) 
//	BYTE cmd 
//	BYTE[4] item/char id 
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
	PackLong( &internalBuffer[0], 1, newSerial );
}
CPRemoveItem &CPRemoveItem::operator=( cBaseObject &toCopy )
{
	CopyData( toCopy );
	return (*this);
}


//0xBC Packet
//Last Modified on Friday, 19-May-2000  
//Seasonal Information(3 bytes) 
//	BYTE cmd 
//	BYTE id1
//	BYTE id2
//Note: Server message
//Note: if id2 = 1, then this is a season change.
//Note: if season change, then id1 = (0=spring, 1=summer, 2=fall, 3=winter, 4 = desolation)
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


//0x4F Packet
//Last Modified on Saturday, 14-Nov-1998 
//Overall Light Level (2 bytes) 
//	BYTE cmd 
//	BYTE level 
//		0x00 - day 
//		0x09 - OSI night
//		0x1F - Black 
//		Max normal val = 0x1F 
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
	PackShort( &internalBuffer[0], 6, skillval );
}
void CPUpdIndSkill::BaseSkill( SI16 skillval )
{
	PackShort( &internalBuffer[0], 8, skillval );
}
void CPUpdIndSkill::Lock( UI08 lockVal )
{
	internalBuffer[10] = lockVal;
}

//0x3B Packet
//Last Modified on Wednesday, 06-May-1998 23:30:41 EDT 
//Buy Item(s) (Variable # of bytes) 
//	BYTE cmd 
//	BYTE[2] blockSize 
//	BYTE[4] vendorID 
//	BYTE flag 
//		0x00 - no items following 
//		0x02 - items following 
//	For each item 
//		BYTE (0x1A) 
//		BYTE[4] itemID (from 3C packet) 
//		BYTE[2] # bought 

void CPBuyItem::CopyData( cBaseObject &i )
{
	Serial( i.GetSerial() );
}
CPBuyItem::CPBuyItem()
{
	InternalReset();
}
CPBuyItem::CPBuyItem( cBaseObject &i )
{
	InternalReset();
	CopyData( i );
}
CPBuyItem &CPBuyItem::operator=( cBaseObject &toCopy )
{
	CopyData( toCopy );
	return (*this);
}
void CPBuyItem::Serial( SERIAL toSet )
{
	PackLong( &internalBuffer[0], 3, toSet );
}

void CPBuyItem::InternalReset( void )
{
	internalBuffer.resize( 8 );
	internalBuffer[0] = 0x3B;
	internalBuffer[1] = 0x00;
	internalBuffer[2] = 0x08;
	internalBuffer[7] = 0x00;
}

const long loopbackIP = (127<<24) + 1;

//0x8C Packet
//Last Modified on Monday, 13-Apr-1998 17:06:42 EDT 
//Connect to Game Server (11 bytes) 
//	BYTE cmd 
//	BYTE[4] gameServer IP 
//	BYTE[2] gameServer port 
//	BYTE[4] new key 

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
	PackLong( &internalBuffer[0], 1, newIP );
}
void CPRelay::Port( SI16 newPort )
{
	PackShort( &internalBuffer[0], 5, newPort );
}
void CPRelay::SeedIP( long newIP )
{
	PackLong( &internalBuffer[0], 7, newIP );
}
void CPRelay::InternalReset( void )
{
	internalBuffer.resize( 11 );
	internalBuffer[0] = 0x8C;
	SeedIP( loopbackIP );
}


//0x2E Packet
//Last Modified on Thursday, 19-Nov-1998 
//Worn Item (15 bytes) 
//	BYTE cmd 
//	BYTE[4] itemid (always starts 0x40 in my data) 
//	BYTE[2] model (item hex #) 
//	BYTE (0x00) 
//	BYTE layer
//	BYTE[4] playerID 
//	BYTE[2] color/hue 

CPWornItem::CPWornItem()
{
	internalBuffer.resize( 15 );
	internalBuffer[0] = 0x2E;
	internalBuffer[7] = 0x00;
}
void CPWornItem::ItemSerial( SERIAL itemSer )
{
	PackLong( &internalBuffer[0], 1, itemSer );
}
void CPWornItem::Model( SI16 newModel )
{
	PackShort( &internalBuffer[0], 5, newModel );
}
void CPWornItem::Layer( UI08 layer )
{
	internalBuffer[8] = layer;
}
void CPWornItem::CharSerial( SERIAL chSer )
{
	PackLong( &internalBuffer[0], 9, chSer );
}
void CPWornItem::Colour( SI16 newColour )
{
	PackShort( &internalBuffer[0], 13, newColour );
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
	CharSerial( toCopy.GetContSerial() );
}
CPWornItem &CPWornItem::operator=( CItem &toCopy )
{
	CopyData( toCopy );
	return (*this);
}

//0x6E Packet
//Last Modified on Monday, 19-Apr-1999 
//Character Animation (14 bytes) 
//	BYTE cmd 
//	BYTE[4] item/char ID 
//	BYTE[2] movement model 
//		0x00 = walk 
//		0x01 = walk faster 
//		0x02 = run 
//		0x03 = run (faster?) 
//		0x04 = nothing 
//		0x05 = shift shoulders 
//		0x06 = hands on hips 
//		0x07 = attack stance (short) 
//		0x08 = attack stance (longer) 
//		0x09 = swing (attack with knife) 
//		0x0a = stab (underhanded) 
//		0x0b = swing (attack overhand with sword) 
//		0x0c = swing (attack with sword over and side) 
//		0x0d = swing (attack with sword side) 
//		0x0e = stab with point of sword 
//		0x0f = ready stance 
//		0x10 = magic (butter churn!) 
//		0x11 = hands over head (balerina) 
//		0x12 = bow shot 
//		0x13 = crossbow 
//		0x14 = get hit 
//		0x15 = fall down and die (backwards) 
//		0x16 = fall down and die (forwards) 
//		0x17 = ride horse (long) 
//		0x18 = ride horse (medium) 
//		0x19 = ride horse (short) 
//		0x1a = swing sword from horse 
//		0x1b = normal bow shot on horse 
//		0x1c = crossbow shot 
//		0x1d = block #2 on horse with shield 
//		0x1e = block on ground with shield 
//		0x1f = swing, and get hit in middle 
//		0x20 = bow (deep) 
//		0x21 = salute 
//		0x22 = scratch head 
//		0x23 = 1 foot forward for 2 secs 
//		0x24 = same 
//	BYTE unknown1 (0x00) 
//	BYTE direction 
//	BYTE[2] repeat (1 = once / 2 = twice / 0 = repeat forever) 
//	BYTE forward/backwards(0x00=forward, 0x01=backwards) 
//	BYTE repeat Flag (0 - Don't repeat / 1 repeat) 
//	BYTE frame Delay (0x00 - fastest / 0xFF - Too slow to watch) 

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
	PackLong( &internalBuffer[0], 1, toSet );
}
void CPCharacterAnimation::Action( SI16 model )
{
	PackShort( &internalBuffer[0], 5, model );
}
void CPCharacterAnimation::Direction( UI08 dir )
{
	internalBuffer[8] = dir;
}
void CPCharacterAnimation::Repeat( SI16 repeatValue )
{
	PackShort( &internalBuffer[0], 9, repeatValue );
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

//0x20 Packet
//Last Modified on Thursday, 19-Nov-1998 
//Draw Game Player (19 bytes) 
//	BYTE cmd 
//	BYTE[4] creature id 
//	BYTE[2] bodyType 
//	BYTE unknown1 (0) 
//	BYTE[2] skin color / hue 
//	BYTE flag byte 
//	BYTE[2] xLoc 
//	BYTE[2] yLoc 
//	BYTE[2] unknown2 (0)
//	BYTE direction 
//	BYTE zLoc 
//Note: Only used with the character being played by the client.

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
	PackLong( &internalBuffer[0], 1, toSet );
}
void CPDrawGamePlayer::Model( SI16 toSet )
{
	PackShort( &internalBuffer[0], 5, toSet );
}
void CPDrawGamePlayer::Colour( SI16 toSet )
{
	PackShort( &internalBuffer[0], 8, toSet );
}
void CPDrawGamePlayer::Flag( UI08 toSet )
{
	internalBuffer[10] = toSet;
}
void CPDrawGamePlayer::X( SI16 toSet )
{
	PackShort( &internalBuffer[0], 11, toSet );
}
void CPDrawGamePlayer::Y( SI16 toSet )
{
	PackShort( &internalBuffer[0], 13, toSet );
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

//0x4E Packet
//Last Modified on Sunday, 17-May-1998 13:33:55 EDT 
//Personal Light Level (6 bytes) 
//	BYTE cmd 
//	BYTE[4] creature id 
//	BYTE level 

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
	PackLong( &internalBuffer[0], 1, toSet );
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

//0x54 Packet
//Last Modified on Sunday, 13-Feb-2000 
//Play Sound Effect (12 bytes) 
//	BYTE cmd 
//	BYTE mode (0x00=quiet, repeating, 0x01=single normally played sound effect) 
//	BYTE[2] SoundModel 
//	BYTE[2] unknown3 (speed/volume modifier? Line of sight stuff?) 
//	BYTE[2] xLoc 
//	BYTE[2] yLoc 
//	BYTE[2] zLoc 
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
	PackShort( &internalBuffer[0], 2, newModel );
}
void CPPlaySoundEffect::X( SI16 xLoc )
{
	PackShort( &internalBuffer[0], 6, xLoc );
}
void CPPlaySoundEffect::Y( SI16 yLoc )
{
	PackShort( &internalBuffer[0], 8, yLoc );
}
void CPPlaySoundEffect::Z( SI16 zLoc )
{
	PackShort( &internalBuffer[0], 10, zLoc );
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

//0x88 Packet
//Last Modified on Saturday, 18-Apr-1998 17:49:39 EDT 
//Open Paperdoll (66 bytes) 
//	BYTE cmd 
//	BYTE[4] charid 
//	BYTE[60] text 
//	BYTE flag byte
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
	PackLong( &internalBuffer[0], 1, tSerial );
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

//0x65 Packet
//Last Modified on Wednesday, 24-May-2000 
//Set Weather (4 bytes) 
//	BYTE cmd 
//	BYTE type (0x00 – “It starts to rain”, 0x01 – “A fierce storm approaches.”, 0x02 – “It begins to snow”, 0x03 - “A storm is brewing.”, 0xFF – None (turns off sound effects), 0xFE (no effect?? Set temperature?) 
//	BYTE num (number of weather effects on screen)
//	BYTE temperature 
//Note: Server Message
//Note: Temperature has no effect at present.
//Note: maximum number of weather effects on screen is 70.
//Note: If it is raining, you can add snow by setting the num to the num of rain currently going, plus the number of snow you want. 
//Note: Weather messages are only displayed when weather starts.
//Note: Weather will end automatically after 6 minutes without any weather change packets.
//Note: You can totally end weather (to display a new message) by teleporting.  I think it’s either the 0x78 or 0x20 messages that reset it, though I haven’t checked to be sure (other possibilities, 0x4F or 0x4E)
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

//0x70 Packet
//Last Modified on Friday, 20-Nov-1998 
//Graphical Effect (28 bytes) 
//	BYTE cmd 
//	BYTE direction type 
//		00 = go from source to dest 
//		01 = lightning strike at source 
//		02 = stay at current x,y,z 
//		03 = stay with current source character id 
//	BYTE[4] character id 
//	BYTE[4] target id
//	BYTE[2] model of the first frame of the effect
//	BYTE[2] xLoc 
//	BYTE[2] yLoc 
//	BYTE zLoc 
//	BYTE[2] xLoc of target 
//	BYTE[2] yLoc of target 
//	BYTE zLoc of target 
//	BYTE speed of the animation
//	BYTE duration (0=really long, 1= shortest)
//	BYTE[2] unknown2 (0 works)
//	BYTE adjust direction during animation (1=no)
//	BYTE explode on impact  
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
	PackLong( &internalBuffer[0], 2, toSet );
}
void CPGraphicalEffect::TargetSerial( cBaseObject &toSet )
{
	TargetSerial( toSet.GetSerial() );
}
void CPGraphicalEffect::TargetSerial( SERIAL toSet )
{
	PackLong( &internalBuffer[0], 6, toSet );
}
void CPGraphicalEffect::Model( SI16 nModel )
{
	PackShort( &internalBuffer[0], 10, nModel );
}
void CPGraphicalEffect::X( SI16 nX )
{
	PackShort( &internalBuffer[0], 12, nX );
}
void CPGraphicalEffect::Y( SI16 nY )
{
	PackShort( &internalBuffer[0], 14, nY );
}
void CPGraphicalEffect::Z( SI08 nZ )
{
	internalBuffer[16] = nZ;
}
void CPGraphicalEffect::XTrg( SI16 nX )
{
	PackShort( &internalBuffer[0], 17, nX );
}
void CPGraphicalEffect::YTrg( SI16 nY )
{
	PackShort( &internalBuffer[0], 19, nY );
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


//0xA1 Packet
//Last Modified on Tuesday, 20-Apr-1999 
//Update Current Health (9 bytes) 
//	BYTE cmd 
//	BYTE[4] playerID 
//	BYTE[2] maxHealth 
//	BYTE[2] currentHealth
//0xA2 Packet
//Last Modified on Tuesday, 21-Apr-1998 20:45:30 EDT 
//Update Current Mana (9 bytes) 
//	BYTE cmd 
//	BYTE[4] playerID 
//	BYTE[2] maxMana 
//	BYTE[2] currentMana 
//0xA3 Packet
//Last Modified on Tuesday, 21-Apr-1998 20:45:32 EDT 
//Update Current Stamina (9 bytes) 
//	BYTE cmd 
//	BYTE[4] playerID 
//	BYTE[2] maxStamina 
//	BYTE[2] currentStamina 
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
	PackLong( &internalBuffer[0], 1, toSet );
}
void CPUpdateStat::MaxVal( SI16 maxVal )
{
	PackShort( &internalBuffer[0], 5, maxVal );
}
void CPUpdateStat::CurVal( SI16 curVal )
{
	PackShort( &internalBuffer[0], 7, curVal );
}

//0xAF Packet
//Last Modified on Tuesday, 20-Apr-1999 
//Display Death Action (13 bytes) 
//	BYTE cmd 
//	BYTE[4] player id
//	BYTE[4] corpse id
//	BYTE[4] unknown (all 0)
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
	PackLong( &internalBuffer[0], 1, toSet );
}
void CPDeathAction::Corpse( SERIAL toSet )
{
	PackLong( &internalBuffer[0], 5, toSet );
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

//0x6D Packet
//Last Modified on Sunday, 17-May-1998 13:33:59 EDT 
//Play Midi Music (3 bytes) 
//	BYTE cmd 
//	BYTE[2] musicID 
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

//0x24 Packet
//Last Modified on Tuesday, 14-Apr-1998 20:53:33 EDT 
//Draw Container (7 bytes) 
//	BYTE cmd 
//	BYTE[4] item id 
//	BYTE[2] model-Gump 
//		0x003c = backpack 
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

//0x6C Packet
//Last Modified on Sunday, 13-Feb-2000 
//Targeting Cursor Commands (19 bytes) 
//	BYTE cmd 
//	BYTE type 
//		0x00 = Select Object
//		0x01 = Select X, Y, Z 
//	BYTE[4] cursorID 
//	BYTE Cursor Type 
//		Always 0 now  
//	The following are always sent but are only valid if sent by client 
//	BYTE[4] Clicked On ID 
//	BYTE[2] click xLoc 
//	BYTE[2] click yLoc 
//	BYTE unknown (0x00) 
//	BYTE click zLoc 
//	BYTE[2] model # (if a static tile, 0 if a map/landscape tile)
//Note: the model # shouldn’t be trusted.
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

//0x11 Packet
//Last Modified on Sunday, 28-Jul-2002
//Stat window info 
//	BYTE cmd 
//	BYTE[2] packet length 
//	BYTE[4] player id 
//	BYTE[30] playerName 
//	BYTE[2] currentHitpoints 
//	BYTE[2] maxHitpoints 
//	BYTE[1] name change flag (0x1 = allowed, 0 = not allowed)
//	BYTE[1] flag 
//		(0x00 no more data following (end of packet here).  
//		0x01 more data after this flag following, 
//		0x03: like 1, with extended info)
//	BYTE sex (0=male, 1=female) 
//	BYTE[2] str 
//	BYTE[2] dex 
//	BYTE[2] int 
//	BYTE[2] currentStamina 
//	BYTE[2] maxStamina 
//	BYTE[2] currentMana 
//	BYTE[2] maxMana 
//	BYTE[4] gold 
//	BYTE[2] armor class 
//	BYTE[2] weight 
//	If (flag == 3) (fill new statusbar data)
//	BYTE[2]  statcap
//	BYTE  pets current
//	BYTE pets max
//Note: Server Message
//Note: For characters other than the player, currentHitpoints and maxHitpoints are not the actual values.  MaxHitpoints is a fixed value, and currentHitpoints works like a percentage.
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
void CPStatWindow::Gold( UI32 gValue )
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

//0x53 Packet
//Last Modified on Sunday, 13-Feb-2000 
//Idle Warning(2 bytes) 
//	BYTE cmd 
//	BYTE value 
//		(0x07 idle
//		0x05 another character is online
//Another character from this account is currently online in this world.  You must either log in as that character or wait for it to time out.”

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


//0x5B Packet
//Last Modified on Saturday, 18-Apr-1998 17:49:28 EDT 
//Time (4 bytes) 
//	BYTE cmd 
//	BYTE hour 
//	BYTE minute 
//	BYTE second 
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

//0x55 Packet
//Last Modified on Monday, 26-Oct-1998 
//Login Complete, Start Game (1 byte) 
//	BYTE cmd 
CPLoginComplete::CPLoginComplete()
{
	internalBuffer.resize( 1 );
	internalBuffer[0] = 0x55;
}


//0x69 Packet
//Last Modified on Thursday, 23-Apr-1998 19:26:05 EDT 
//Change Text/Emote Color (5 bytes) 
//	BYTE cmd 
//	BYTE[2] blockSize 
//	BYTE[2] unknown1 
//The client sends two of these independent of the color chosen. It sends two of them in quick succession as part of the "same" packet. The unknown1 is 0x00 0x01 in the first and 0x00 0x02 in the second.
//Note, this message has been removed.  It is no longer used.
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


//0x72 Packet
//Last Modified on Saturday, 14-Nov-1998 
//Request War Mode Change/Send War Mode status (5 bytes) 
//	BYTE cmd 
//	BYTE flag 
//		0x00 - Normal 
//		0x01 - Fighting 
//	BYTE[3] unknown1 (always 00 32 00 in testing) 
//Server replies with 0x77 packet
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

//0x33 Packet
//Last Modified on Thursday, 19-Nov-1998 
//Pause/Resume Client (2 bytes)
//	BYTE cmd 
//	BYTE pause/resume (0=pause, 1=resume)
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

//0xA5 Packet
//Last Modified on Wednesday, 06-May-1998 23:30:48 EDT 
//Open Web Browser (Variable # of bytes) 
//	BYTE cmd 
//	BYTE[2] blockSize 
//	BYTE[blockSize-3] null terminated full web address 
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

//0xBA Packet
//Last Modified on Sunday, 13-Feb-2000  
//Quest Arrow (6 bytes) 
//	BYTE cmd
//	BYTE active (1=on, 0=off)
//	BYTE[2] xLoc 
//	BYTE[2] yLoc
//Server Message
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

//0x27 Packet
//Last Modified on Friday, 19-May-2000
//Reject Request to Move Items (2 bytes) 
//	BYTE cmd 
//	BYTE unknown1 (0x00) 
//Note: Server Message
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

//0x95 Packet
//Last Modified on Thursday, 30-Apr-1998 18:34:08 EDT 
//Dye Window (9 bytes) 
//	BYTE cmd 
//	BYTE[4] itemID of dye target
//	BYTE[2] ignored on send, model on return
//	BYTE[2] model on send, color on return  (default on server send is 0x0FAB) 
//NOTE: This packet is sent by both the server and client
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

//0x99 Packet
//Last Modified on Friday, 20-Nov-1998 
//Bring Up House/Boat Placement View (26 bytes) 
//	BYTE cmd 
//	BYTE request (0x01 from server, 0x00 from client) 
//	BYTE[4] ID of deed 
//	BYTE[12] unknown (all 0)
//	BYTE[2] multi model (item model - 0x4000)
//	BYTE[6] unknown (all 0)
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

//0x25 Packet
//Last Modified on Saturday, 02-May-1998 16:05:35 EDT 
//Add Item to Container (20 bytes) 
//	BYTE cmd 
//	BYTE[4] item id to add 
//	BYTE[2] model 
//	BYTE unknown1 (0)
//	BYTE[2] # of items 
//	BYTE[2] xLoc in container 
//	BYTE[2] yLoc in container 
//	BYTE[4] itemID of container 
//	BYTE[2] color 
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
	Container( toCopy.GetContSerial() );
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

//0x26 Packet
//Last Modified on Friday, 19-May-2000
//Kick Player (5 bytes) 
//	BYTE cmd 
//	BYTE[4] ID of GM who issued kick? 
//Note: Server Message
void CPKickPlayer::InternalReset( void )
{
	internalBuffer.resize( 5 );
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

//0x2C Packet
//Last Modified on Friday, 19-May-2000
//Resurrection Menu Choice (2 bytes) 
//	BYTE cmd 
//	BYTE action (2=ghost, 1=resurrect, 0=from server)
//Note: Client and Server Message
//Note: Resurrection menu has been removed from UO.
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

//0x2F Packet
//Last Modified on Saturday, 1-May-1999
//Fight Occurring (10 bytes) 
//	BYTE cmd 
//	BYTE unknown1 (0)
//	BYTE[4] ID of attacker 
//	BYTE[4] ID of attacked 
//This packet is sent when there is a fight going on somewhere on screen.
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

//0x90 Packet
//Last Modified on Sunday, 2-May-1999 
//Map message(19 bytes) 
//	BYTE cmd 
//	BYTE[4] key used 
//	BYTE[2] gump art id (0x139D)
//	BYTE[2] upper left x location
//	BYTE[2] upper left y location
//	BYTE[2] lower right x location
//	BYTE[2] lower right y location
//	BYTE[2] gump width in pixels
//	BYTE[2] gump height in pixels 
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


//0x93 Packet
//Last Modified on Wednesday, 2-Feb-2000 
//Books - Title Page (99 bytes) 
//	BYTE cmd 
//	BYTE[4] bookID 
//	BYTE write flag 
//		0x00 - non-writable 
//		0x01 - writable 
//	BYTE new flag
//	BYTE[2] # of pages 
//	BYTE[60] title 
//	BYTE[30] author 
//Client sends a 0x93 message on book close Look into this. J
//Client Ver:
//Books - Update Title Page (99 bytes)
//	BYTE cmd
//	BYTE[4] bookID
//	BYTE[4] unknown (0)
//	BYTE[60] title (zero terminated, garbage after terminator)
//	BYTE[30] author (zero terminated, garbage after terminator)
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


//0xBB Packet
//Last Modified on Sunday, 30-Jan-2000  
//Ultima Messenger  (9 bytes) 
//	BYTE cmd 
//	BYTE[4] id1
//	BYTE[4] id2
//Note: This is both a client and server message.
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

cPInputBuffer *WhichPacket( UI08 packetID, cSocket *s )
{
	switch( packetID )
	{
	case 0x02:	return ( new CPIMoveRequest( s )		);
	case 0x04:	return ( new CPIGodModeToggle( s )		);
	case 0x05:	return ( new CPIAttack( s )				);
	case 0x06:	return ( new CPIDblClick( s )			);
	case 0x07:	return ( new CPIGetItem( s )			);
	case 0x08:	return ( new CPIDropItem( s )			);
	case 0x09:	return ( new CPISingleClick( s )		);
	case 0x13:	return ( new CPIEquipItem( s )			);
	case 0x22:	return ( new CPIResyncReq( s )			);
	case 0x2C:	return ( new CPIResMenuChoice( s )		);
	case 0x34:	return ( new CPIStatusRequest( s )		);
	case 0x6C:	return ( new CPITargetCursor( s )		);
	case 0x73:	return ( new CPIKeepAlive( s )			);
	case 0x75:	return ( new CPIRename( s )				);
	case 0x80:	return ( new CPIFirstLogin( s )			);
	case 0x91:	return ( new CPISecondLogin( s )		);
	case 0xA0:	return ( new CPIServerSelect( s )		);
	case 0xA4:	return ( new CPISpy( s )				);
	case 0xA7:	return ( new CPITips( s )				);
	case 0xB1:	return ( new CPIGumpMenuSelect( s )		);
	case 0xBD:	return ( new CPIClientVersion( s )		);
	case 0xC8:	return ( new CPIUpdateRangeChange( s )	);
	default:	return NULL;
	}
	return NULL;
}

bool CPIFirstLogin::Handle( void )
{
	tSock->AcctNo( AB_INVALID_ID );
	LoginDenyReason t = LDR_NODENY;
	ACCOUNTSBLOCK actbTemp;

	if( Accounts->GetAccountByName( (char *)&tSock->Buffer()[1], actbTemp ) )
		tSock->SetAccount( actbTemp );

	char *username = (char *)&tSock->Buffer()[1];
	char pass1[256], pass2[256];
	memset( pass1, 0, 256 );
	memset( pass2, 0, 256 );
	pSplit( (char *)&tSock->Buffer()[31], pass1, pass2 );
//	char *password = pass1;

	// Need auto account creation code here
	if( tSock->AcctNo() == AB_INVALID_ID )
	{
		if( cwmWorldState->ServerData()->GetInternalAccountStatus() )
		{
			Accounts->AddAccount( username, pass1, "" );
			if( Accounts->GetAccountByName( username, actbTemp ) )	// grab our account number
				tSock->SetAccount( actbTemp );
		}
	}
	if( tSock->AcctNo() != AB_INVALID_ID )
	{
		if( actbTemp.wFlags&AB_FLAGS_BANNED )
			t = LDR_ACCOUNTDISABLED;
		else if( strcmp( actbTemp.sPassword.c_str(), pass1 ) )
			t = LDR_BADPASSWORD;
	}
	else
		t = LDR_UNKNOWNUSER;
	if( t == LDR_NODENY && ( actbTemp.wFlags&AB_FLAGS_ONLINE ) )
		t = LDR_ACCOUNTINUSE;
	if( t != LDR_NODENY )
	{
		CPLoginDeny pckDeny( t );
		tSock->Send( &pckDeny );
		tSock->AcctNo( AB_INVALID_ID );
		Network->LoginDisconnect( tSock );
		return true;
	}
	if( tSock->AcctNo() != AB_INVALID_ID )
	{
		char temp[1024];
		char newlist1[7] = "\xA8\x01\x23\xFF\x00\x01";
		cPBuffer bufNewlist1( newlist1, 6 );
		char newlist2[41] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x12\x01\x7F\x00\x00\x01";
		actbTemp.dwLastIP = calcserial( tSock->ClientIP4(), tSock->ClientIP3(), tSock->ClientIP2(), tSock->ClientIP1() );
		sprintf( temp, "Client [%i.%i.%i.%i] connected using Account '%s'.", tSock->ClientIP4(), tSock->ClientIP3(), tSock->ClientIP2(), tSock->ClientIP1(), username );
		Console.Log( temp, "server.log" );
		messageLoop << temp;

		actbTemp.wFlags |= AB_FLAGS_ONLINE;

		UI32 servcount = cwmWorldState->ServerData()->GetServerCount();
		UI32 tlen = 6 + ( servcount * 40 );
		bufNewlist1[1] = (UI08)( tlen>>8 );
		bufNewlist1[2] = (UI08)( tlen%256 );
		bufNewlist1[4] = (UI08)( servcount>>8 );
		bufNewlist1[5] = (UI08)( servcount%256 );

		tSock->Send( &bufNewlist1 );
		for( UI16 i = 0; i < servcount; i++ )
		{
			physicalServer *sData = cwmWorldState->ServerData()->GetServerEntry( i );
			newlist2[0] = (UI08)((i+1)>>8);
			newlist2[1] = (UI08)((i+1)%256);
			strcpy( &newlist2[2], sData->getName().c_str() );
			UI32 ip = htonl( inet_addr( sData->getIP().c_str() ) );
			newlist2[36] = (UI08)( ip>>24 );
			newlist2[37] = (UI08)( ip>>16 );
			newlist2[38] = (UI08)( ip>>8 );
			newlist2[39] = (UI08)( ip%256 );
			tSock->Send( newlist2, 40 );
		}
	}

	return true;
}

CPIFirstLogin::CPIFirstLogin()
{
	InternalReset();
}
CPIFirstLogin::CPIFirstLogin( cSocket *s ) : cPInputBuffer( s )
{
	InternalReset();
	Receive();
}
void CPIFirstLogin::Receive( void )
{
	tSock->FirstPacket( false );
	tSock->Receive( 62 );
	memcpy( &internalBuffer[0], tSock->Buffer(), 62 );
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
CPIServerSelect::CPIServerSelect( cSocket *s ) : cPInputBuffer( s )
{
	InternalReset();
	Receive();
}
void CPIServerSelect::Receive( void )
{
	tSock->Receive( 3 );
	memcpy( &internalBuffer[0], tSock->Buffer(), 3 );
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
CPISecondLogin::CPISecondLogin( cSocket *s ) : cPInputBuffer( s )
{
	InternalReset();
	Receive();
}
void CPISecondLogin::Receive( void )
{
	tSock->FirstPacket( false );
	tSock->Receive( 65 );
	tSock->CryptClient( true );
	memcpy( &internalBuffer[0], tSock->Buffer(), 65 );
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
	tSock->AcctNo( AB_INVALID_ID );
	ACCOUNTSBLOCK actbTemp;
	if( Accounts->GetAccountByName( (char *)&tSock->Buffer()[5], actbTemp ) )
		tSock->SetAccount( actbTemp );

//	char *username = (char *)&tSock->Buffer()[5];
	char pass1[256], pass2[256];
	memset( pass1, 0, 256 );
	memset( pass2, 0, 256 );
	pSplit( (char *)&tSock->Buffer()[35], pass1, pass2 );
//	char *password = pass1;

	if( tSock->AcctNo() != AB_INVALID_ID )
	{
		if( actbTemp.wFlags&AB_FLAGS_BANNED )
			t = LDR_ACCOUNTDISABLED;
		else if( strcmp( pass1, actbTemp.sPassword.c_str() ) )
			t = LDR_UNKNOWNUSER;
	}
	else
		t = LDR_UNKNOWNUSER;
	if( t != LDR_NODENY )
	{
		tSock->AcctNo( AB_INVALID_ID );
		CPLoginDeny pckDeny( t );
		tSock->Send( &pckDeny );
		Network->LoginDisconnect( tSock );
		return true;
	}
	else
	{
		UI08 charCount = 0;
		for( UI32 i = 0; i < 5; i++ )
		{
			if( actbTemp.dwCharacters[i] != INVALIDSERIAL )
				charCount++;
		}
		cServerData *sd = cwmWorldState->ServerData();
		UI32 serverCount = sd->GetNumServerLocations();
		CPCharAndStartLoc toSend( actbTemp, charCount, static_cast<UI08>(serverCount) );
		for( UI32 j = 0; j < serverCount; j++ )
		{
			toSend.AddStartLocation( sd->GetServerLocation( j ), static_cast<UI08>(j) );
		}
		tSock->Send( &toSend );
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
CPIClientVersion::CPIClientVersion( cSocket *s ) : cPInputBuffer( s )
{
	InternalReset();
	Receive();
}
void CPIClientVersion::Receive( void )
{
	tSock->Receive( 3 );
	len = tSock->GetWord( 1 );
	tSock->Receive( len );
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

	std::string s( verString );
	std::istringstream ss( s );
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
	internalBuffer.resize( 2 );
}
CPIUpdateRangeChange::CPIUpdateRangeChange()
{
	InternalReset();
}
CPIUpdateRangeChange::CPIUpdateRangeChange( cSocket *s ) : cPInputBuffer( s )
{
	InternalReset();
	Receive();
}
void CPIUpdateRangeChange::Receive( void )
{
	tSock->Receive( 2 );
	internalBuffer[0] = tSock->GetByte( 0 );
	internalBuffer[1] = tSock->GetByte( 1 );
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
	tSock->Send( &(internalBuffer[0]), 2 );	// we want to echo it back to the client
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
	return (UI16)( (internalBuffer[3]<<8) + internalBuffer[4] );
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


//0x74 Packet
//Last Modified on Sunday, 03-May-1998 22:52:07 EDT 
//Open Buy Window (Variable # of bytes) 
//	BYTE cmd 
//	BYTE[2] blockSize 
//	BYTE[4] (vendorID | 0x40000000) 
//	BYTE # of items 
//	# of items worth of item segments 
//		BYTE[4] price 
//		BYTE length of text description 
//		BYTE[text length] item description 
//NOTE: This packet is always preceded by a describe contents packet (0x3c) with the container id as the (vendorID | 0x40000000) and then an open container packet (0x24?) with the vendorID only and a model number of 0x0030 (probably the model # for the buy screen)
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
	UI32 value = calcValue( toAdd, (UI32)toAdd->GetValue() );
	if( cwmWorldState->ServerData()->GetTradeSystemStatus() )
		value = calcGoodValue( p, toAdd, value, false );
	char itemname[MAX_NAME];
	UI08 sLen = getTileName( toAdd, itemname ); // Item name length

	internalBuffer[baseOffset +  0] = (UI08)((toAdd->GetValue())>>24);
	internalBuffer[baseOffset +  1] = (UI08)((toAdd->GetValue())>>16);
	internalBuffer[baseOffset +  2] = (UI08)((toAdd->GetValue())>>8);
	internalBuffer[baseOffset +  3] = (UI08)((toAdd->GetValue())%256);
	internalBuffer[baseOffset +  4] = sLen;

	for( UI08 k = 0; k < sLen; k++ )
		internalBuffer[baseOffset + 5 + k] = itemname[k];
	return (SI16)(5 + sLen);
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

//0xA9 Packet
//Last Modified on Sunday, 28-Jul-2002
//Characters / Starting Locations (Variable # of bytes) 
//	BYTE cmd 
//	BYTE[2] blockSize 
//	BYTE # of characters 
//	Following repeated 5 times 
//		BYTE[30] character name 
//		BYTE[30] character password 
//	BYTE number of starting locations 
//	Following for as many locations as you have 
//		BYTE locationIndex (0-based) 
//		BYTE[31] town (general name) 
//		BYTE[31] exact name 
//	BYTE Flags 
//		0x02 = send config/req logout (IGR?), 
//		0x04 = single character (siege), 
//		0x08 = enable npcpopup menus, 
//		0x10 = unknown
void CPCharAndStartLoc::InternalReset( void )
{
	internalBuffer.resize( 4 );
	internalBuffer[0] = 0xA9;
}
void CPCharAndStartLoc::CopyData(ACCOUNTSBLOCK& toCopy )
{
	for( UI08 i = 0; i < 5; i++ )
	{
		if( toCopy.lpCharacters[i] != NULL )
			AddCharacter( toCopy.lpCharacters[i], i );
	}
}

CPCharAndStartLoc::CPCharAndStartLoc()
{
	InternalReset();
}
CPCharAndStartLoc::CPCharAndStartLoc(ACCOUNTSBLOCK& actbBlock, UI08 numCharacters, UI08 numLocations )
{
	InternalReset();
	NumberOfCharacters( numCharacters );
	NumberOfLocations( numLocations );
	CopyData(actbBlock);
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

CPCharAndStartLoc& CPCharAndStartLoc::operator=(ACCOUNTSBLOCK& actbBlock )
{
	CopyData(actbBlock);
	return (*this);
}

//0xF0 Packet
//Last Modified on Tuesday, 26-Feb-2002  
//Custom client packet
//	BYTE cmd 
//	BYTE[2] len
//	BYTE subcmd 
//	BYTE[len - 4] data 

//Subcommand 0: Acknowledge login (1 byte (for 5 total))
//	BYTE ack  (0 = Rejected, unauthorized.  1 = Accepted, GM priviledges. 2 = Accepted, player priviledges)
//Note: Server Message
//Note: GM priviledges unlock movement rate configurations

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

//0xA7 Packet
//Last Modified on Wednesday, 06-May-1998 23:30:53 EDT 
//Request Tips/Notice (4 bytes) 
//	BYTE cmd 
//	BYTE[2] last tip # 
//	BYTE flag 
//		0x00 - tips window 
//		0x01 - notice window 
void CPITips::InternalReset( void )
{
}
CPITips::CPITips()
{
	InternalReset();
}
CPITips::CPITips( cSocket *s ) : cPInputBuffer( s )
{
	InternalReset();
	Receive();
}
void CPITips::Receive( void )
{
	tSock->Receive( 4 );
}
bool CPITips::Handle( void )
{
	ScriptSection *Tips = FileLookup->FindEntry( "TIPS", misc_def );
	if( Tips == NULL )
		return true;

	int i = tSock->GetWord( 1 ) + 1;
	if( i == 0 ) 
		i = 1;

	int x = i;
	const char *tag = NULL;
	const char *data = NULL;
	char temp[1024];
	for( tag = Tips->First(); !Tips->AtEnd(); tag = Tips->Next() )
	{
		if( tag != NULL && !strcmp( tag, "TIP" ) )
			x--;
		if( x <= 0 )
			break;
	}
	if( x > 0 )
		tag = Tips->Prev();
	data = Tips->GrabData();
	sprintf(temp, "TIP %i", makeNum( data ) );
	Tips = FileLookup->FindEntry( temp, misc_def );
	if( Tips == NULL )
		return true;
	x = -1;
	char tipData[2048];
	tipData[0] = 0;
	CPUpdScroll toSend( 0, (UI08)i );
	for( tag = Tips->First(); !Tips->AtEnd(); tag = Tips->Next() )
	{
		data = Tips->GrabData();
		sprintf( tipData, "%s%s %s ", tipData, tag, data );
	}

	std::string toAdd;
	toSend.AddString( tipData );
	toSend.Finalize();
	tSock->Send( &toSend );

	return true;
}

//0xA6 Packet
//Last Modified on Wednesday, 06-May-1998 23:30:49 EDT 
//Tips/Notice window (Variable # of bytes) 
//	BYTE cmd 
//	BYTE[2] blockSize 
//	BYTE flag 
//		0x00 - tips window 
//		0x01 - notice window 
//	BYTE[2] unknown1 
//	BYTE[2] tip # 
//	BYTE[2] msgSize 
//	BYTE[?] message (? = blockSize - 10) 
//*Null terminated I think (Gimli)
void CPUpdScroll::InternalReset( void )
{
	internalBuffer.resize( 10 );	// 10, not 11
	internalBuffer[0] = 0xA6;
	internalBuffer[1] = 0x01;
	internalBuffer[2] = 0x02;
	internalBuffer[3] = 0x02;
	internalBuffer[4] = 0x00;
	internalBuffer[5] = 0x00;
	internalBuffer[6] = 0x00;
	internalBuffer[7] = 0x00;
	internalBuffer[8] = 0x01;
	internalBuffer[9] = 0x02;
	memset( tipData, 0, 2048 );
}
CPUpdScroll::CPUpdScroll()
{
	InternalReset();
}
CPUpdScroll::CPUpdScroll( UI08 tType )
{
	InternalReset();
	TipType( tType );
}
CPUpdScroll::CPUpdScroll( UI08 tType, UI08 tNum )
{
	InternalReset();
	TipType( tType );
	TipNumber( tNum );
}
CPUpdScroll::~CPUpdScroll()
{
}
void CPUpdScroll::AddString( const char *toAdd )
{
	strcat( tipData, toAdd );
}
void CPUpdScroll::AddStrings( const char *tag, const char *data )
{
	char temp[1024];
	sprintf( temp, "%s %s", tag, data );
	AddString( temp );
}
void CPUpdScroll::TipType( UI08 tType )
{
	internalBuffer[3] = tType;
}
void CPUpdScroll::TipNumber( UI08 tipNum )
{
	internalBuffer[7] = tipNum;
}
void CPUpdScroll::Finalize( void )
{
	int y = strlen( tipData ) + 10;
	SetLength( y );
	memcpy( &internalBuffer[10], tipData, y - 10 );
}

void CPUpdScroll::SetLength( UI16 len )
{
	internalBuffer.resize( len );
	internalBuffer[1] = (UI08)(len>>8);
	internalBuffer[2] = (UI08)(len%256);
	internalBuffer[8] = (UI08)((len - 10)>>8);
	internalBuffer[9] = (UI08)((len - 10)%256);
}

//
//0x75 Packet
//Last Modified on Sunday, 13-Feb-2000 
//Rename Character (35 bytes) 
//	BYTE cmd 
//	BYTE[4] id 
//	BYTE[30] new name 

void CPIRename::InternalReset( void )
{
}
CPIRename::CPIRename()
{
	InternalReset();
}
CPIRename::CPIRename( cSocket *s ) : cPInputBuffer( s )
{
	InternalReset();
	Receive();
}
void CPIRename::Receive( void )
{
	tSock->Receive( 0x23 );
}
bool CPIRename::Handle( void )
{
	SERIAL serial = tSock->GetDWord( 1 );
	if( serial == INVALIDSERIAL ) 
		return true;
	CChar *c = calcCharObjFromSer( serial );
	if( c != NULL )
		c->SetName( (char *)&tSock->Buffer()[5] );
	return true;
}

//0xC0 Packet
//Last Modified on Friday, 26-Jul-2002  
//(yet another) Graphical Effect (36 Bytes)
//	BYTE cmd 
//	BYTE type 
//	BYTE[4] sourceSerial 
//	BYTE[4] targetSerial 
//	BYTE[2] itemID 
//	BYTE[2] xSource 
//	BYTES[2] ySource 
//	BYTE zSource 
//	BYTE[2] xTarget 
//	BYTE[2] yTarget 
//	BYTE zTarget 
//	BYTE speed 
//	BYTE duration 
//	BYTE[2] unk // On OSI, flamestrikes are 0x0100 
//	BYTE fixedDirection 
//	BYTE explodes 
//	BYTE[4] hue 
//	BYTE[4] renderMode 
//Server message
void CPGraphicalEffect2::InternalReset( void )
{
	internalBuffer.resize( 36 );
	internalBuffer[0] = 0xC0;
	internalBuffer[24] = 0;
	internalBuffer[25] = 0;
}
CPGraphicalEffect2::CPGraphicalEffect2( UI08 effectType ) : CPGraphicalEffect( effectType )
{
	InternalReset();
}
CPGraphicalEffect2::CPGraphicalEffect2( UI08 effectType, cBaseObject &src, cBaseObject &trg ) : CPGraphicalEffect( effectType, src, trg )
{
	InternalReset();
	Effect( effectType );
	SourceSerial( src );
	TargetSerial( trg );
}
CPGraphicalEffect2::CPGraphicalEffect2( UI08 effectType, cBaseObject &src ) : CPGraphicalEffect( effectType, src )
{
	InternalReset();
	Effect( effectType );
	SourceSerial( src );
}
void CPGraphicalEffect2::Hue( UI32 hue )
{
	PackLong( &internalBuffer[0], 28, hue );
}
void CPGraphicalEffect2::RenderMode( UI32 mode )
{
	PackLong( &internalBuffer[0], 32, mode );
}

//0x73 Packet
//Last Modified on Friday, 20-Nov-1998 
//Ping message (2 bytes) 
//	BYTE cmd 
//	BYTE seq 
CPIKeepAlive::CPIKeepAlive()
{
}
CPIKeepAlive::CPIKeepAlive( cSocket *s ) : cPInputBuffer( s )
{
	Receive();
}
void CPIKeepAlive::Receive( void )
{
	tSock->Receive( 2 );
}
bool CPIKeepAlive::Handle( void )
{
	tSock->Send( tSock->Buffer(), 2 );
	return true;
}

//0x34 Packet
//Last modified on Thursday, 19-Nov-1998 
//Get Player Status (10 bytes) 
//	BYTE cmd 
//	BYTE[4] pattern (0xedededed) 
//	BYTE getType 
//		0x04 - Basic Stats (Packet 0x11 Response) 
//		0x05 = Request Skills (Packet 0x3A Response) 
//	BYTE[4] playerID 
CPIStatusRequest::CPIStatusRequest()
{
}
CPIStatusRequest::CPIStatusRequest( cSocket *s ) : cPInputBuffer( s )
{
	Receive();
}
void CPIStatusRequest::Receive( void )
{
	tSock->Receive( 10 );
}
bool CPIStatusRequest::Handle( void )
{
	if( tSock->GetByte( 5 ) == 4 )
		statwindow( tSock, calcCharObjFromSer( tSock->GetDWord( 6 ) ) );
	if( tSock->GetByte( 5 ) == 5 )
	{
		CPSkillsValues toSend = (*(tSock->CurrcharObj()));
		tSock->Send( &toSend );
	}
	return true;
}

//0xA4 Packet
//Last Modified on Friday, 15-May-2000 
//Spy on Client (149 bytes) 
//	BYTE cmd 
//	BYTE[148] Unknown (previously, this has had info such as your graphics card name, free HD space, number of processors, etc.)
CPISpy::CPISpy()
{
}
CPISpy::CPISpy( cSocket *s ) : cPInputBuffer( s )
{
	Receive();
}
void CPISpy::Receive( void )
{
	tSock->Receive( 0x95 );
}
bool CPISpy::Handle( void )
{
	return true;
}

//0x04 Packet
//Last Modified on Friday, 19-May-2000 
//Request (2 bytes) 
//	BYTE cmd 
//	BYTE mode (0=off, 1=on)  
//Note: Client Message
CPIGodModeToggle::CPIGodModeToggle()
{
}
CPIGodModeToggle::CPIGodModeToggle( cSocket *s ) : cPInputBuffer( s )
{
	Receive();
}
void CPIGodModeToggle::Receive( void )
{
	tSock->Receive( 2 );
}
bool CPIGodModeToggle::Handle( void )
{
	CChar *ourChar = tSock->CurrcharObj();
	if( ourChar->IsGM() )
	{
		CPGodModeToggle cpgmt = tSock;
		tSock->Send( &cpgmt );
	}
	else
	{
		sysmessage( tSock, 1641 );
		Console << "Godclient detected - Account[" << ourChar->GetAccount().wAccountIndex << "] Username[" << ourChar->GetAccount().sUsername << ". Client disconnected!" << myendl;
		Network->Disconnect( tSock );
	}
	return true;
}

//0x56 Packet
//Last Modified on Friday, 26-Jul-2002
//Map Related(11 bytes) 
//	BYTE cmd																				0
//	BYTE[4] id																				1
//	BYTE command																			5
//		1 = add map point, 
//		2 = add new pin with pin number. (insertion. other pins after the number are pushed back.) 
//		3 = change pin
//		4 = remove pin
//		5 = remove all pins on the map 
//		6 = toggle the 'editable' state of the map. 
//		7 = return msg from the server to the request 6 of the client. 
//	BYTE plotting state (1=on, 0=off, valid only if command 7)								6
//	BYTE[2] x location (relative to upper left corner of the map, in pixels, for points)	7
//	BYTE[2] y location (relative to upper left corner of the map, in pixels, for points)	9
CPMapRelated::CPMapRelated()
{
	internalBuffer.resize( 11 );
	internalBuffer[0] = 0x56;
}
void CPMapRelated::PlotState( UI08 pState )
{
	internalBuffer[6] = pState;
}
void CPMapRelated::Location( SI16 x, SI16 y )
{
	PackShort( &internalBuffer[0], 7, x );
	PackShort( &internalBuffer[0], 9, y );
}
void CPMapRelated::Command( UI08 cmd )
{
	internalBuffer[5] = cmd;
}
void CPMapRelated::ID( SERIAL key )
{
	PackLong( &internalBuffer[0], 1, key );
}

//0x06 Packet
//Last Modified on Friday, 19-May-2000 
//Double click (5 bytes) 
//	BYTE cmd 
//	BYTE[4] ID of double clicked object 
//Note: Client Message
CPIDblClick::CPIDblClick()
{
}
CPIDblClick::CPIDblClick( cSocket *s ) : cPInputBuffer( s )
{
	Receive();
}
void CPIDblClick::Receive( void )
{
	tSock->Receive( 5 );
}
bool CPIDblClick::Handle( void )
{
	CChar *ourChar = tSock->CurrcharObj();
	doubleClick( tSock );
	ourChar->BreakConcentration( tSock );
	return true;
}

//0x09 Packet
//Last Modified on Friday, 19-May-2000 
//Single click (5 bytes) 
//	BYTE cmd 
//	BYTE[4] ID of single clicked object 
//Note: Client Message
CPISingleClick::CPISingleClick()
{
}
CPISingleClick::CPISingleClick( cSocket *s ) : cPInputBuffer( s )
{
	Receive();
}
void CPISingleClick::Receive( void )
{
	tSock->Receive( 5 );
}
bool CPISingleClick::Handle( void )
{
	singleClick( tSock );
	return true;
}

//0x02 Packet
//Last Modified on Friday, 19-May-2000
//Move Request (7 bytes) 
//	BYTE cmd 
//	BYTE direction 
//	BYTE sequence number 
//	BYTE[4] fastwalk prevention key
//Note: Client Message
//Note: sequence number starts at 0, after a reset.  However, if 255 is reached, the next seq # is 1, not 0.
//Fastwalk prevention notes: each 0x02 pops the top element from fastwalk key stack. (0xbf sub1 init. fastwalk stack, 0xbf sub2 pushes an element to stack)
//If stack is empty key value is 0. ( never set keys to 0 in 0xbf sub 1/2)
//Because client sometimes sends bursts of 0x02’s DON’T check for a certain top stack value.
//The only safe way to detect fastwalk: push a key after EACH x021, 0x22, (=send 0xbf sub 2) check in 0x02 for stack emptyness.
//If empty -> fastwalk alert.
//Note that actual key values are irrelevant. (just don’t use 0)
//Of course without perfect 0x02/0x21/0x22 synch (serverside) it’s useless to use fastwalk detection.
//Last but not least: fastwalk detection adds 9 bytes per step and player !
CPIMoveRequest::CPIMoveRequest()
{
}
CPIMoveRequest::CPIMoveRequest( cSocket *s ) : cPInputBuffer( s )
{
	Receive();
}
void CPIMoveRequest::Receive( void )
{
	tSock->Receive( 7 );
}
bool CPIMoveRequest::Handle( void )
{
	CChar *ourChar = tSock->CurrcharObj();
	Movement->Walking( ourChar, tSock->GetByte( 1 ), tSock->GetByte( 2 ) );
	ourChar->BreakConcentration( tSock );
	return true;
}

//0x22 Packet
//Last Modified on Wednesday, 11-Nov-1998 
//Character Move ACK/ Resync Request(3 bytes) 
//	BYTE cmd 
//	BYTE sequence (matches sent sequence) 
//	BYTE (0x00) 
CPIResyncReq::CPIResyncReq()
{
}
CPIResyncReq::CPIResyncReq( cSocket *s ) : cPInputBuffer( s )
{
	Receive();
}
void CPIResyncReq::Receive( void )
{
	tSock->Receive( 3 );
}
bool CPIResyncReq::Handle( void )
{
	return true;
}

//0x2C Packet
//Last Modified on Friday, 19-May-2000
//Resurrection Menu Choice (2 bytes) 
//	BYTE cmd 
//	BYTE action (2=ghost, 1=resurrect, 0=from server)
//Note: Client and Server Message
//Note: Resurrection menu has been removed from UO.
CPIResMenuChoice::CPIResMenuChoice()
{
}
CPIResMenuChoice::CPIResMenuChoice( cSocket *s ) : cPInputBuffer( s )
{
	Receive();
}
void CPIResMenuChoice::Receive( void )
{
	tSock->Receive( 2 );
}
bool CPIResMenuChoice::Handle( void )
{
	UI08 cmd = tSock->GetByte( 1 );
	if( cmd == 0x02 ) 
		sysmessage( tSock, 766 );
	if( cmd == 0x01 ) 
		sysmessage( tSock, 767 );
	return true;
}

//0x05 Packet
//Last Modified on Friday, 19-May-2000 
//Attack Request (5 bytes) 
//	BYTE cmd 
//	BYTE[4] ID to be attacked 
//Note: Client Message
CPIAttack::CPIAttack()
{
}
CPIAttack::CPIAttack( cSocket *s ) : cPInputBuffer( s )
{
	Receive();
}
void CPIAttack::Receive( void )
{
	tSock->Receive( 5 );
}
bool CPIAttack::Handle( void )
{
	CChar *ourChar = tSock->CurrcharObj();
	ourChar->BreakConcentration( tSock );
	PlayerAttack( tSock );
	return true;
}

//0x6C Packet
//Last Modified on Sunday, 13-Feb-2000 
//Targeting Cursor Commands (19 bytes) 
//	BYTE cmd 
//	BYTE type 
//		0x00 = Select Object
//		0x01 = Select X, Y, Z 
//	BYTE[4] cursorID 
//	BYTE Cursor Type 
//	Always 0 now  
//	The following are always sent but are only valid if sent by client 
//	BYTE[4] Clicked On ID 
//	BYTE[2] click xLoc 
//	BYTE[2] click yLoc 
//	BYTE unknown (0x00) 
//	BYTE click zLoc 
//	BYTE[2] model # (if a static tile, 0 if a map/landscape tile)
//Note: the model # shouldn’t be trusted.
CPITargetCursor::CPITargetCursor()
{
}
CPITargetCursor::CPITargetCursor( cSocket *s ) : cPInputBuffer( s )
{
	Receive();
}
void CPITargetCursor::Receive( void )
{
	tSock->Receive( 19 );
}
bool CPITargetCursor::Handle( void )
{
	CChar *ourChar = tSock->CurrcharObj();
	if( tSock->TargetOK() )
		Targ->MultiTarget( tSock );
	ourChar->BreakConcentration( tSock );
	return true;
}

//0x13 Packet
//Last Modified on Thursday, 19-Nov-1998 
//Drop->Wear Item (10 bytes) 
//	BYTE cmd 
//	BYTE[4] itemid 
//	BYTE layer (see layer list at top) 
//	BYTE[4] playerID 
//Note: The layer byte should not be trusted.  
CPIEquipItem::CPIEquipItem()
{
}
CPIEquipItem::CPIEquipItem( cSocket *s ) : cPInputBuffer( s )
{
	Receive();
}
void CPIEquipItem::Receive( void )
{
	tSock->Receive( 10 );
}
bool CPIEquipItem::Handle( void )
{
	CChar *ourChar = tSock->CurrcharObj();
	wearItem( tSock );
	ourChar->BreakConcentration( tSock );
	return true;
}

//0x07 Packet
//Last Modified on Friday, 19-May-2000
//Pick Up Item(s) (7 bytes) 
//	BYTE cmd 
//	BYTE[4] item id 
//	BYTE[2] # of items in stack 
//Note: Client Message
CPIGetItem::CPIGetItem()
{
}
CPIGetItem::CPIGetItem( cSocket *s ) : cPInputBuffer( s )
{
	Receive();
}
void CPIGetItem::Receive( void )
{
	tSock->Receive( 7 );
}
bool CPIGetItem::Handle( void )
{
	CChar *ourChar = tSock->CurrcharObj();
	grabItem( tSock );
	ourChar->BreakConcentration( tSock );
	return true;
}

//0x08 Packet
//Last Modified on Friday, 19-May-2000 
//Drop Item(s) (14 bytes) 
//	BYTE cmd 
//	BYTE[4] item id 
//	BYTE[2] X Location 
//	BYTE[2] Y Location
//	BYTE Z Location
//	BYTE[4] Move Into Container ID (FF FF FF FF if normal world) 
//Note: Client Message
//Note: 3D clients sometimes sends 2 of them (bursts) for ONE drop action. 
//The last one having –1’s in X/Y locs.
//Be very careful how to handle this odd “bursts” server side, neither always process, nor always skipping is correct.
CPIDropItem::CPIDropItem()
{
}
CPIDropItem::CPIDropItem( cSocket *s ) : cPInputBuffer( s )
{
	Receive();
}
void CPIDropItem::Receive( void )
{
	tSock->Receive( 14 );
}
bool CPIDropItem::Handle( void )
{
	CChar *ourChar = tSock->CurrcharObj();
	UI08 opt = tSock->GetByte( 10 );
	if( opt >= 0x40 && opt != 0xFF )
		packItem( tSock );
	else 
		dropItem( tSock );
	ourChar->BreakConcentration( tSock );
	return true;
}

//0xB1 Packet
//Last Modified on Sunday, 28-Jul-2002
//Gump Menu Selection (Variable # of bytes) 
//	BYTE cmd													0
//	BYTE[2] blockSize											1
//	BYTE[4] id (first Id in 0xb0)								3
//	BYTE[4] gumpId (second Id in 0xb0)							7
//	BYTE[4] buttonId (which button perssed ? 0 if closed)		11
//	BYTE[4] switchcount  (response info for radio buttons and checkboxes, any switches listed here are switched on)	15
//	For each switch
//		BYTE[4] SwitchId
//	BYTE[4] textcount  (response info for textentries)			19 + switchcount * 4
//	For each textentry
//		BYTE[2] textId
//		BYTE[2] textlength
//		BYTE[length*2] Unicode text (not nullterminated)
CPIGumpMenuSelect::CPIGumpMenuSelect()
{
}
CPIGumpMenuSelect::CPIGumpMenuSelect( cSocket *s ) : cPInputBuffer( s )
{
	Receive();
}
void CPIGumpMenuSelect::Receive( void )
{
	tSock->Receive( 3 );
	tSock->Receive( tSock->GetWord( 1 ) );
}
bool CPIGumpMenuSelect::Handle( void )
{
	id			= tSock->GetDWord( 3 );
	gumpID		= tSock->GetDWord( 7 );
	buttonID	= tSock->GetDWord( 11 );
	switchCount	= tSock->GetDWord( 15 );
	textOffset	= 19 + 4 * switchCount;
	textCount	= tSock->GetDWord( textOffset );
	
	BuildTextLocations();
	Gumps->Button( this );
	return true;
}

SERIAL CPIGumpMenuSelect::ButtonID( void ) const
{
	return buttonID;
}
SERIAL CPIGumpMenuSelect::GumpID( void ) const
{
	return gumpID;
}
SERIAL CPIGumpMenuSelect::ID( void ) const
{
	return id;
}
UI32 CPIGumpMenuSelect::SwitchCount( void ) const
{
	return switchCount;
}
UI32 CPIGumpMenuSelect::TextCount( void ) const
{
	return textCount;
}

UI32 CPIGumpMenuSelect::SwitchValue( UI32 index ) const
{
	if( index >= switchCount )
		return INVALIDSERIAL;
	return tSock->GetDWord( 15 + 4 * index );
}

UI16 CPIGumpMenuSelect::GetTextID( UI08 number ) const
{
	if( number >= textCount )
		return 0xFFFF;
	return tSock->GetWord( textLocationOffsets[number] );
}
UI16 CPIGumpMenuSelect::GetTextLength( UI08 number ) const
{
	if( number >= textCount )
		return 0xFFFF;
	return tSock->GetWord( textLocationOffsets[number] + 2 );
}
std::string CPIGumpMenuSelect::GetTextString( UI08 number ) const
{
	if( number >= textCount )
		return "";
	UI08 bufferLen = GetTextLength( number ) * 2 + 1;
	UI08 *buffer = new UI08[bufferLen];
	UI16 bufferOffset = textLocationOffsets[number] + 4;
	strncpy( (char *)buffer, (const char *)&(tSock->Buffer()[bufferOffset]), bufferLen );
	buffer[bufferLen-1] = 0;
	std::string toReturn = (char *)buffer;
	delete [] buffer;
	return toReturn;

}
std::string CPIGumpMenuSelect::GetTextUString( UI08 number ) const
{
	if( number >= textCount )
		return "";
	std::string toReturn = "";
	UI16 offset = textLocationOffsets[number] + 4;
	for( int counter = 0; counter < GetTextLength( number ); counter++ )
		toReturn += tSock->GetByte( offset + counter * 2 + 1 );
	return toReturn;
}
void CPIGumpMenuSelect::BuildTextLocations( void )
{
	UI32 i = 23 + switchCount * 4;	// first is 19 + number of switches * 4 + 4 for the textcount
	textLocationOffsets.resize( textCount );
	for( unsigned int j = 0; j < textCount; j++ )
	{
		textLocationOffsets[j] = static_cast<wchar_t>(i);
		i += 2;	// skip the text ID
		UI16 textLen = tSock->GetWord( i );
		i += ( 2 * textLen + 2 );	// we need to add the + 2 for the text len field
	}
}