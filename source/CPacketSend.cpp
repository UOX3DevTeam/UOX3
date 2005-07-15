#include "uox3.h"
#include "CPacketSend.h"
#include "speech.h"
#include "combat.h"
#include "cMagic.h"
#include "power.h"

namespace UOX
{

void PackString( UI08 *toPack, int offset, std::string value, UI32 maxLen )
{
	int toWrite	= UOX_MIN( maxLen, static_cast<UI32>(value.length()) );
	for( int i = 0; i < toWrite; ++i )
		toPack[offset+i] = (UI08)value[i];
}
std::string UnpackString( UI08 *toPack, int offset, std::string &value, UI32 maxLen )
{
	value = "";
	for( UI32 i = 0; i < maxLen; ++i )
	{
		if( toPack[offset + i] != 0 )
			value += (char)(toPack[offset + i]);
	}
	return value;
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
	toPack[offset+0] = (UI08)((value&0xFF000000)>>24);
	toPack[offset+1] = (UI08)((value&0x00FF0000)>>16);
	toPack[offset+2] = (UI08)((value&0x0000FF00)>>8);
	toPack[offset+3] = (UI08)((value&0x000000FF)%256);
}

void PackShort( UI08 *toPack, int offset, UI16 value )
{
	toPack[offset+0] = (UI08)((value&0xFF00)>>8);
	toPack[offset+1] = (UI08)((value&0x00FF)%256);
}

void pSplit( const std::string pass0, std::string &pass1, std::string &pass2 ) // Split login password into UOX password and UO password
{
	int i = 0;
	pass1 = "";
	int pass0Len = pass0.length();
	while( pass0[i] != '/' && i < pass0Len ) 
		++i;
	pass1 = pass0.substr( 0, i );
	if( i < pass0Len ) 
		pass2 = pass0.substr( i );
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

void CPCharLocBody::Log( std::ofstream &outStream, bool fullHeader )
{
	if( fullHeader )
		outStream << "[SEND]Packet     : CPCharLocBody 0x1B --> Length: " << internalBuffer.size() << std::endl;
	outStream << "PlayerID         : " << std::hex << UnpackULong(  &internalBuffer[0], 1  ) << std::endl;
	outStream << "Unknown1         : " << std::dec << UnpackULong(  &internalBuffer[0], 5  ) << std::endl;
	outStream << "BodyType         : " << std::hex << UnpackUShort( &internalBuffer[0], 9  ) << std::endl;
	outStream << "X Loc            : " << std::dec << UnpackUShort( &internalBuffer[0], 11 ) << std::endl;
	outStream << "Y Loc            : " << UnpackUShort( &internalBuffer[0], 13 ) << std::endl;
	outStream << "Z Loc            : " << UnpackUShort( &internalBuffer[0], 15 ) << std::endl;
	outStream << "Direction        : " << internalBuffer[17] << std::endl;
	outStream << "Unknown2         : " << UnpackUShort( &internalBuffer[0], 18 ) << std::endl;
	outStream << "Unknown3         : " << UnpackULong(  &internalBuffer[0], 20 ) << std::endl;
	outStream << "Unknown4         : " << UnpackULong(  &internalBuffer[0], 24 ) << std::endl;
	outStream << "Flag Byte        : " << internalBuffer[28] << std::endl;
	outStream << "Highlight Colour : " << internalBuffer[29] << std::endl;
	outStream << "  Raw dump     :" << std::endl;
	cPUOXBuffer::Log( outStream, false );
}

void CPCharLocBody::InternalReset( void )
{
	internalBuffer.resize( 37 );
	internalBuffer[0] = 0x1B;
	for( UI08 k = 5; k < 9; ++k )
		internalBuffer[k] = 0;
	for( UI08 i = 18; i < 28; ++i )
		internalBuffer[i] = 0;
	for( UI08 j = 30; j < 37; ++j )
		internalBuffer[j] = 0;
	HighlightColour( 0 );
}
CPCharLocBody::CPCharLocBody()
{
	InternalReset();
}

CPCharLocBody::CPCharLocBody( CChar &toCopy )
{
	InternalReset();
	CopyData( toCopy );
}

void CPCharLocBody::CopyData( CChar &toCopy )
{
	PackLong( &internalBuffer[0], 1, toCopy.GetSerial() );
	PackShort( &internalBuffer[0], 9, toCopy.GetID() );
	PackShort( &internalBuffer[0], 11, toCopy.GetX() );
	PackShort( &internalBuffer[0], 13, toCopy.GetY() );
	internalBuffer[16] = toCopy.GetZ();
	internalBuffer[17] = toCopy.GetDir();
}

CPCharLocBody &CPCharLocBody::operator=( CChar &toCopy )
{
	CopyData( toCopy );
	return (*this);
}

void CPCharLocBody::Flag( UI08 toPut )
{
	internalBuffer[28] = toPut;
}
void CPCharLocBody::HighlightColour( UI08 color )
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
	if( toCopy.SpeakerName().length() != 0 )
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
			if( ValidateObject( ourChar ) )
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
			if( ValidateObject( ourItem ) )
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
	for( size_t i = 0; i < internalBuffer.size(); ++i )
		internalBuffer[i] = toCopy[i];
	return (*this);
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

void CPacketSpeech::SpeakerName( const std::string toPut )
{
	size_t len = toPut.length();
	if( len >= 30 )
	{
		strncpy( (char *)&internalBuffer[14], toPut.c_str(), 29 );
		internalBuffer[43] = 0;
	}
	else
	{
		strcpy( (char *)&internalBuffer[14], toPut.c_str() );
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
#if defined( _MSC_VER )
#pragma note( "Function Warning: CPacketSpeech::Language(), does nothing" )
#endif
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
void CPacketSpeech::Speech( const std::string toPut )
{
	size_t len			= toPut.length();
	const size_t newLen	= 44 + len + 1;
	internalBuffer.resize( newLen );
	strcpy( (char *)&internalBuffer[44], toPut.c_str() );
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

void CPExtMove::FlagColour( UI08 newValue )
{
	internalBuffer[16] = newValue;
}

void CPExtMove::CopyData( CChar &toCopy )
{
	PackLong( &internalBuffer[0], 1, toCopy.GetSerial() );
	PackShort( &internalBuffer[0], 5, toCopy.GetID() );
	PackShort( &internalBuffer[0], 7, toCopy.GetX() );
	PackShort( &internalBuffer[0], 9, toCopy.GetY() );
	internalBuffer[11] = toCopy.GetZ();

	UI08 dir = toCopy.GetDir();
	if( toCopy.IsNpc() && toCopy.CanRun() && toCopy.IsAtWar() )
		dir |= 0x80;
	internalBuffer[12] = dir;
	PackShort( &internalBuffer[0], 13, toCopy.GetSkin() );

	UI08 flag = 0;
	if( toCopy.IsAtWar() ) 
		flag |= 0x40;
	else if( toCopy.IsDead() )
		flag |= 0x80;
	if( toCopy.GetVisible() != VT_VISIBLE ) 
		flag |= 0x80;
	if( toCopy.GetPoisoned() ) 
		flag |= 0x04;
	if( toCopy.GetID() == 0x0191 )
		flag |= 0x02;
#if defined( _MSC_VER )
#pragma note( "Get confirmation here" )
#endif
	// turn it yellow on full health?
	if( toCopy.GetHP() == toCopy.GetMaxHP() )
		flag |= 0x08;
	internalBuffer[15] = flag;
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
void CPRemoveItem::CopyData( CBaseObject &toCopy )
{
	Serial( toCopy.GetSerial() );
}
CPRemoveItem::CPRemoveItem()
{
	internalBuffer.resize( 5 );
	internalBuffer[0] = 0x1D;
}
CPRemoveItem::CPRemoveItem( CBaseObject &toCopy )
{
	internalBuffer.resize( 5 );
	internalBuffer[0] = 0x1D;
	CopyData( toCopy );
}
void CPRemoveItem::Serial( SERIAL newSerial )
{
	PackLong( &internalBuffer[0], 1, newSerial );
}
CPRemoveItem &CPRemoveItem::operator=( CBaseObject &toCopy )
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
CPLightLevel::CPLightLevel( LIGHTLEVEL level )
{
	internalBuffer.resize( 2 );
	internalBuffer[0] = 0x4F;
	Level( level );
}

void CPLightLevel::Level( LIGHTLEVEL level )
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

void CPBuyItem::CopyData( CBaseObject &i )
{
	Serial( i.GetSerial() );
}
CPBuyItem::CPBuyItem()
{
	InternalReset();
}
CPBuyItem::CPBuyItem( CBaseObject &i )
{
	InternalReset();
	CopyData( i );
}
CPBuyItem &CPBuyItem::operator=( CBaseObject &toCopy )
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
CPRelay::CPRelay( long newIP, UI16 newPort )
{
	InternalReset();
	ServerIP( newIP );
	Port( newPort );
}
void CPRelay::ServerIP( long newIP )
{
	PackLong( &internalBuffer[0], 1, newIP );
}
void CPRelay::Port( UI16 newPort )
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
void CPCharacterAnimation::Action( UI16 model )
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
CPDrawGamePlayer::CPDrawGamePlayer( CChar &toCopy )
{
	InternalReset();
	CopyData( toCopy );
}

void CPDrawGamePlayer::CopyData( CChar &toCopy )
{
	PackLong( &internalBuffer[0], 1, toCopy.GetSerial() );
	PackShort( &internalBuffer[0], 5, toCopy.GetID() );
	PackShort( &internalBuffer[0], 8, toCopy.GetColour() );
	PackShort( &internalBuffer[0], 11, toCopy.GetX() );
	PackShort( &internalBuffer[0], 13, toCopy.GetY() );
	internalBuffer[17] = toCopy.GetDir();
	internalBuffer[18] = toCopy.GetZ();
	UI08 flag = 0;
	if( toCopy.IsInvulnerable() )
		flag |= 0x01;
	if( toCopy.IsDead() )
		flag |= 0x02;
	if( toCopy.GetPoisoned() )
		flag |= 0x04;
	if( toCopy.IsAtWar() )
		flag |= 0x40;
	if( toCopy.GetVisible() != VT_VISIBLE )
		flag |= 0x80;
	internalBuffer[10] = flag;
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
void CPPersonalLightLevel::Level( LIGHTLEVEL lightLevel )
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
void CPPlaySoundEffect::Model( UI16 newModel )
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
CPPlaySoundEffect &CPPlaySoundEffect::operator=( CBaseObject &toCopy )
{
	CopyData( toCopy );
	return (*this);
}

void CPPlaySoundEffect::CopyData( CBaseObject &toCopy )
{
	X( toCopy.GetX() );
	Y( toCopy.GetY() );
	Z( toCopy.GetZ() );
}

CPPlaySoundEffect::CPPlaySoundEffect( CBaseObject &toCopy )
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
void CPPaperdoll::Text( const std::string toPut )
{
	if( toPut.length() > 60 )
	{
		strncpy( (char *)&internalBuffer[5], toPut.c_str(), 59 );
		internalBuffer[64] = 0;
	}
	else
		strcpy( (char *)&internalBuffer[5], toPut.c_str() );
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
void CPGraphicalEffect::SourceSerial( CBaseObject &toSet )
{
	SourceSerial( toSet.GetSerial() );
}
void CPGraphicalEffect::SourceSerial( SERIAL toSet )
{
	PackLong( &internalBuffer[0], 2, toSet );
}
void CPGraphicalEffect::TargetSerial( CBaseObject &toSet )
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

CPGraphicalEffect::CPGraphicalEffect( UI08 effectType, CBaseObject &src, CBaseObject &trg )
{
	InternalReset();
	Effect( effectType );
	SourceSerial( src );
	TargetSerial( trg );
}
CPGraphicalEffect::CPGraphicalEffect( UI08 effectType, CBaseObject &src )
{
	InternalReset();
	Effect( effectType );
	SourceSerial( src );
}

void CPGraphicalEffect::SourceLocation( CBaseObject& toSet )
{
	SourceLocation( toSet.GetX(), toSet.GetY(), toSet.GetZ() );
}
void CPGraphicalEffect::TargetLocation( CBaseObject& toSet )
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
void CPDrawContainer::Model( UI16 newModel )
{
	PackShort( &internalBuffer[0], 5, newModel );
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
	PackLong( &internalBuffer[0], 1, toSet );
}

//	0x7C Packet
//	Last Modified on Thursday, 03-Sep-2002
//	Open Dialog Box (Variable # of bytes) 
//	BYTE cmd													0 
//	BYTE[2] blockSize											1
//	BYTE[4] dialogID (echo'd back to the server in 7d)			3
//	BYTE[2] menuid (echo'd back to server in 7d)				7
//	BYTE length of question										9
//	BYTE[length of question] question text						10
//	BYTE # of responses 
//	Then for each response: 
//		BYTE[2] model id # of shown item (if grey menu -- then always 0x00 as msb) 
//		BYTE[2] color of shown item
//		BYTE response text length 
//		BYTE[response text length] response text 

void CPOpenGump::Log( std::ofstream &outStream, bool fullHeader )
{
	if( fullHeader )
		outStream << "[SEND]Packet     : CPOpenGump 0x7C --> Length: " << internalBuffer.size() << std::endl;
	outStream << "DialogID         : " << std::hex << UnpackULong(  &internalBuffer[0], 3  ) << std::endl;
	outStream << "MenuID           : " << UnpackUShort( &internalBuffer[0], 7  ) << std::endl;
	outStream << "Question Length  : " << std::dec << internalBuffer[9] << std::endl;
	outStream << "Question         : ";

	for( UI32 i = 0; i < internalBuffer[9]; ++i )
		outStream << internalBuffer[10+i];
	outStream << std::endl;

	outStream << "# Responses      : " << internalBuffer[responseBaseOffset] << std::endl;
	size_t offsetCount = responseBaseOffset + 1;
	for( UI32 j = 0; j < internalBuffer[responseBaseOffset]; ++j )
	{
		outStream << "    Response " << j << std::endl;
		outStream << "      Model  : " << std::hex << UnpackUShort( &internalBuffer[0], static_cast< UI32 >(offsetCount + 0) ) << std::endl;
		outStream << "      Colour : " << UnpackUShort( &internalBuffer[0], static_cast< UI32 >(offsetCount + 2) ) << std::endl;
		outStream << "      Length : " << std::dec << internalBuffer[offsetCount + 4] << std::endl;
		outStream << "      Text   : ";

		for( UI32 k = 0; k < internalBuffer[offsetCount + 4]; ++k )
			outStream << internalBuffer[offsetCount + 5 + k];
		outStream << std::endl;
		offsetCount += ( 5 + internalBuffer[offsetCount + 4] );
	}
	outStream << "  Raw dump     :" << std::endl;
	cPUOXBuffer::Log( outStream, false );
}
void CPOpenGump::Question( std::string toAdd )
{
	internalBuffer.resize( 10 + toAdd.length() + 2 );	// 10 for start of string, length of string + NULL, plus spot for # responses
	for( UI32 i = 0; i < toAdd.length(); ++i )
		internalBuffer[10 + i] = toAdd[i];
#if defined( UOX_DEBUG_MODE )
	if( toAdd.length() >= 255 )
		Console.Error( 1, "CPOpenGump::Question toAdd.length() is too long (%i)", toAdd.length() );
#endif
	internalBuffer[9]	= static_cast< UI08 >(toAdd.length() + 1);
	responseBaseOffset	= (internalBuffer.size() - 1);
	responseOffset		= responseBaseOffset + 1;
}
void CPOpenGump::AddResponse( UI16 modelNum, UI16 colour, std::string responseText )
{
	++internalBuffer[responseBaseOffset]; // increment number of responses
#if defined( UOX_DEBUG_MODE )
	if( responseText.length() >= 255 )
		Console.Error( 1, "CPOpenGump::AddResponse responseText is too long (%i)", responseText.length() );
#endif
	UI16 toAdd = static_cast< UI16 >(5 + responseText.length());
	internalBuffer.resize( internalBuffer.size() + toAdd );
	PackShort(  &internalBuffer[0], static_cast< UI32 >(responseOffset + 0), modelNum );
	PackShort(  &internalBuffer[0], static_cast< UI32 >(responseOffset + 2), colour   );
	internalBuffer[responseOffset + 4] = static_cast< UI08 >(responseText.length());
	PackString( &internalBuffer[0], static_cast< UI32 >(responseOffset + 5), responseText, static_cast< UI32 >(responseText.length()) );
	responseOffset += toAdd;
}
void CPOpenGump::Finalize( void )
{
	PackShort( &internalBuffer[0], 1, static_cast< UI16 >(internalBuffer.size()) );
}
void CPOpenGump::InternalReset( void )
{
	responseOffset		= 0xFFFFFFFF;
	responseBaseOffset	= 0xFFFFFFFF;
	internalBuffer.resize( 9 );
	internalBuffer[0] = 0x7C;
	internalBuffer[1] = 0x00;
	internalBuffer[2] = 0x00;
	internalBuffer[3] = 0x01;
	internalBuffer[4] = 0x02;
	internalBuffer[5] = 0x03;
	internalBuffer[6] = 0x04;
	internalBuffer[7] = 0x00;
	internalBuffer[8] = 0x64;
}
CPOpenGump::CPOpenGump()
{
	InternalReset();
}
CPOpenGump::CPOpenGump( CChar &toCopy )
{
	InternalReset();
	CopyData( toCopy );
}
void CPOpenGump::Length( int totalLines )
{
	PackShort( &internalBuffer[0], 1, totalLines );
}
void CPOpenGump::GumpIndex( int index )
{
	PackShort( &internalBuffer[0], 7, index );
}
CPOpenGump &CPOpenGump::operator=( CChar &toCopy )
{
	CopyData( toCopy );
	return (*this);
}
void CPOpenGump::CopyData( CChar &toCopy )
{
	Serial( toCopy.GetSerial() );
}
void CPOpenGump::Serial( SERIAL toSet )
{
	PackLong( &internalBuffer[0], 3, toSet );
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
// 	BYTE[2]  statcap
//	BYTE  pets current
//	BYTE pets max
//	If (flag == 4) 
//	BYTE[2]	fireresist
//	BYTE[2]	coldresist
//	BYTE[2]	poisonresist
//	BYTE[2]	energyresist
//	BYTE[2]	luck
//	BYTE[2]	damage max 
//	BYTE[2]	damage min
//	BYTE[4]	unknown

//Note: Server Message
//Note: For characters other than the player, currentHitpoints and maxHitpoints are not the actual values.  MaxHitpoints is a fixed value, and currentHitpoints works like a percentage.
void CPStatWindow::SetCharacter( CChar &toCopy, CSocket &target )
{
	if( target.ReceivedVersion() )
	{
		if( target.ClientVersionMajor() >= 4 )
		{
			extended3 = true;
			extended4 = true;
			internalBuffer.resize( 88 );
			internalBuffer[2] = 88;
			Flag( 4 );
		}
		else if( target.ClientVersionMajor() >= 3 )
		{
			extended3 = true;
			internalBuffer.resize( 70 );
			internalBuffer[2] = 70;
			Flag( 3 );
		}
	}
	else
		Flag( 1 );
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
	if( extended3 )
	{
		StatCap( cwmWorldState->ServerData()->ServerStatCapStatus() );
		CurrentPets( toCopy.GetPetList()->Num() );
		MaxPets( 0xFF );
	}
	if( extended4 )
	{
		FireResist( 0 );
		ColdResist( 0 );
		EnergyResist( 0 );
		PoisonResist( 0 );
		Luck( 0 );
		Unknown( 0 );
		DamageMin( 0 );
		DamageMax( Combat->calcAtt( &toCopy ) );
	}
}
void CPStatWindow::InternalReset( void )
{
	internalBuffer.resize( 66 );
	internalBuffer[0] = 0x11;
	internalBuffer[1] = 0;
	internalBuffer[2] = 66;
	extended3 = false;
	extended4 = false;
	Flag( 0 );
}
CPStatWindow::CPStatWindow()
{
	InternalReset();
}
CPStatWindow::CPStatWindow( CChar &toCopy, CSocket &target )
{
	InternalReset();
	SetCharacter( toCopy, target );
}
void CPStatWindow::Serial( SERIAL toSet )
{
	internalBuffer[3] = (UI08)(toSet>>24);
	internalBuffer[4] = (UI08)(toSet>>16);
	internalBuffer[5] = (UI08)(toSet>>8);
	internalBuffer[6] = (UI08)(toSet%256);
}
void CPStatWindow::Name( const std::string nName )
{
	if( nName.length() >= 30 )
	{
		strncpy( (char *)&internalBuffer[7], nName.c_str(), 29 );
		internalBuffer[36] = 0;
	}
	else
		strcpy( (char*)&internalBuffer[7], nName.c_str() );
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
void CPStatWindow::AC( UI16 nValue )
{
	internalBuffer[62] = (UI08)(nValue>>8);
	internalBuffer[63] = (UI08)(nValue%256);
}
void CPStatWindow::Weight( UI16 nValue )
{
	internalBuffer[64] = (UI08)(nValue>>8);
	internalBuffer[65] = (UI08)(nValue%256);
}

void CPStatWindow::StatCap( UI16 value )
{
	if( extended3 )
	{
		internalBuffer[66] = (UI08)(value>>8);
		internalBuffer[67] = (UI08)(value%256);
	}
}
void CPStatWindow::CurrentPets( UI08 value )
{
	if( extended3 )
	{
		internalBuffer[68] = value;
	}
}
void CPStatWindow::MaxPets( UI08 value )
{
	if( extended3 )
	{
		internalBuffer[69] = value;
	}
}
void CPStatWindow::FireResist( UI16 value )
{
	if( extended4 )
	{
		internalBuffer[70] = (UI08)(value>>8);
		internalBuffer[71] = (UI08)(value%256);
	}
}
void CPStatWindow::ColdResist( UI16 value )
{
	if( extended4 )
	{
		internalBuffer[72] = (UI08)(value>>8);
		internalBuffer[73] = (UI08)(value%256);
	}
}
void CPStatWindow::PoisonResist( UI16 value )
{
	if( extended4 )
	{
		internalBuffer[74] = (UI08)(value>>8);
		internalBuffer[75] = (UI08)(value%256);
	}
}
void CPStatWindow::EnergyResist( UI16 value )
{
	if( extended4 )
	{
		internalBuffer[76] = (UI08)(value>>8);
		internalBuffer[77] = (UI08)(value%256);
	}
}
void CPStatWindow::Luck( UI16 value )
{
	if( extended4 )
	{
		internalBuffer[78] = (UI08)(value>>8);
		internalBuffer[79] = (UI08)(value%256);
	}
}
void CPStatWindow::DamageMax( UI16 value )
{
	if( extended4 )
	{
		internalBuffer[80] = (UI08)(value>>8);
		internalBuffer[81] = (UI08)(value%256);
	}
}
void CPStatWindow::DamageMin( UI16 value )
{
	if( extended4 )
	{
		internalBuffer[82] = (UI08)(value>>8);
		internalBuffer[83] = (UI08)(value%256);
	}
}
void CPStatWindow::Unknown( UI32 value )
{
	if( extended4 )
	{
		internalBuffer[84] = (UI08)(value>>24);
		internalBuffer[85] = (UI08)(value>>16);
		internalBuffer[86] = (UI08)(value>>8);
		internalBuffer[87] = (UI08)(value%256);
	}
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

bool CPPauseResume::ClientCanReceive( CSocket *mSock )
{
	if( mSock == NULL )
		return false;
	switch( mSock->ClientType() )
	{
	case CV_NORMAL:
	case CV_T2A:
	case CV_UO3D:
		if( mSock->ClientVersionMajor() >= 4 )
			return false;
		break;
	default:
		break;
	}
	return true;
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
CPWebLaunch::CPWebLaunch( const std::string txt )
{
	InternalReset();
	Text( txt );
}
void CPWebLaunch::Text( const std::string txt )
{
	const SI16 tLen = (SI16)txt.length() + 4;
	SetSize( tLen );
	strcpy( (char *)&internalBuffer[3], txt.c_str() );
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
CPTrackingArrow &CPTrackingArrow::operator=( CBaseObject &toCopy )
{
	Location( toCopy.GetX(), toCopy.GetY() );
	return (*this);
}
void CPTrackingArrow::Active( UI08 value )
{
	internalBuffer[1] = value;
}

CPTrackingArrow::CPTrackingArrow( CBaseObject &toCopy )
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
void CPDyeVat::CopyData( CBaseObject &target )
{
	Serial( target.GetSerial() );
	Model( target.GetID() );
}
CPDyeVat::CPDyeVat()
{
	InternalReset();
}
CPDyeVat::CPDyeVat( CBaseObject &target )
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
CPDyeVat &CPDyeVat::operator=( CBaseObject &target )
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
	for( UI08 i = 6; i < 18; ++i )
		internalBuffer[i] = 0;
	for( UI08 k = 20; k < 26; ++k )
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

CPEnableClientFeatures::CPEnableClientFeatures()
{
//Enable locked client features (3 bytes) 
//·        BYTE cmd 
//·        BYTE[2] feature#
//Bit# 1 T2A upgrade, enables chatbutton, 
//Bit# 2 enables LBR update.  (of course LBR installation is required)
//(plays MP3 instead of midis, 2D LBR client shows new LBR monsters,…)
//Bit# 3: unknown, never seen it set
//Bit# 4: unknown, set on OSI servers that have AOS code - no matter of account status (doesn’t seem to “unlock/lock” anything on client side)
//Bit# 5: enables AOS update (necro/paladin skills for all clients, malas map/AOS monsters if AOS installation present)
//Bit# 15: since client 4.0 this bit has to be set, otherwise bits 3..14 are ignored.
//Thus 0: neither T2A NOR LBR, equal to not sending it at all, 
//1 is T2A, chatbutton, 
//2 is LBR without chatbutton, 
//3 is LBR with chatbutton…
//8013, LBR + chatbutton + AOS enabled
//Note1: this message is send immediately after login.
//Note2: on OSI  servers this controls features OSI enables/disables via “upgrade codes.”
//Note3: a 3 doesn’t seem to “hurt” older (NON LBR) clients.

	internalBuffer.resize( 3 );
	internalBuffer[0] = 0xB9;
#if defined( _MSC_VER )
#pragma todo( "Currently all client support is hardcoded. Move this into the ini when possible." )
#endif
	internalBuffer[1] = 0x80;		// 0x00
	internalBuffer[2] = 0x3F;		// New chars enabled(shh they prolly wont work) and Enable 6th slot
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
	NumSkills( ALLSKILLS );
	SetCharacter( toCopy );
}

void CPSkillsValues::SetCharacter( CChar &toCopy )
{
	for( SI08 i = 0; i < NumSkills(); ++i )
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
UI08 CPSkillsValues::NumSkills( void )
{
	int size = ( (internalBuffer[1]) << 8 ) + internalBuffer[2];
	size -= 6;
	size /= 7;
	return (UI08)size;
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
void CPBookTitlePage::Title( const std::string txt )
{
	if( txt.length() >= 60 )
	{
		strncpy( (char *)&internalBuffer[9], txt.c_str(), 59 );
		internalBuffer[68] = 0;
	}
	else
		strcpy( (char *)&internalBuffer[9], txt.c_str() );
}
void CPBookTitlePage::Author( const std::string txt )
{
	if( txt.length() >= 30 )
	{
		strncpy( (char *)&internalBuffer[69], txt.c_str(), 29 );
		internalBuffer[98] = 0;
	}
	else
		strcpy( (char *)&internalBuffer[69], txt.c_str() );
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

void CPGumpTextEntry::InternalReset( void )
{
	BlockSize( 3 );
	internalBuffer[0] = 0xAB;
}
CPGumpTextEntry::CPGumpTextEntry()
{
	InternalReset();
}
CPGumpTextEntry::CPGumpTextEntry( const std::string text )
{
	InternalReset();
	Text1( text );
}
CPGumpTextEntry::CPGumpTextEntry( const std::string text1, const std::string text2 )
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
void CPGumpTextEntry::Text1( const std::string txt )
{
	size_t sLen = txt.length();
	BlockSize( 20 + sLen );	// 11 + 1 + 8
	Text1Len( sLen + 1 );
	strcpy( (char *)&internalBuffer[11], txt.c_str() );
}
void CPGumpTextEntry::Text2( const std::string txt )
{
	size_t sLen			= txt.length();
	SI16 currentSize	= CurrentSize();
	BlockSize( static_cast< UI16 >(currentSize + sLen + 1) );
	Text2Len( static_cast< SI16 >(sLen + 1) );
	strcpy( (char *)&internalBuffer[currentSize], txt.c_str() );
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

void CPGodModeToggle::CopyData( CSocket *s )
{
	internalBuffer[1] = s->GetByte( 1 );
}

CPGodModeToggle::CPGodModeToggle( CSocket *s )
{
	InternalReset();
	CopyData( s );
}

CPGodModeToggle& CPGodModeToggle::operator=( CSocket *s )
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

//	Subcommand 8: Set cursor hue / Set MAP 
//	BYTE hue  (0 = Felucca, unhued / BRITANNIA map.  1 = Trammel, hued gold / BRITANNIA map, 2 = (switch to) ILSHENAR map)
//	Note: Server Message

void CPMapChange::Log( std::ofstream &outStream, bool fullHeader )
{
	if( fullHeader )
		outStream << "[SEND]Packet   : CPMapChange 0xBF Subcommand 8 --> Length: " << internalBuffer.size() << std::endl;
	outStream << "Hue            : " << internalBuffer[5] << std::endl;
	switch( internalBuffer[5] )
	{
		case 0:		outStream << " (Felucca)" << std::endl;		break;
		case 1:		outStream << " (Trammel)" << std::endl;		break;
		case 2:		outStream << " (Ilshenar)" << std::endl;	break;
		case 3:		outStream << " (Malas)" << std::endl;		break;
		default:	outStream << " (Unknown)" << std::endl;		break;
	}
	outStream << "  Raw dump     :" << std::endl;
	cPUOXBuffer::Log( outStream, false );
}
void CPMapChange::InternalReset( void )
{
	internalBuffer.resize( 6 );
	internalBuffer[0] = 0xBF;
	internalBuffer[1] = 0x00;
	internalBuffer[2] = 0x06;
	internalBuffer[3] = 0x00;
	internalBuffer[4] = 0x08;	// set client hue
}
CPMapChange::CPMapChange()
{
	InternalReset();
}
CPMapChange::CPMapChange( UI08 newMap )
{
	InternalReset();
	SetMap( newMap );
}
CPMapChange::CPMapChange( CBaseObject *moving )
{
	InternalReset();
	if( ValidateObject( moving ) )
		SetMap( moving->WorldNumber() );
}

void CPMapChange::SetMap( UI08 newMap )
{
	internalBuffer[5] = newMap;
}
CPMapChange& CPMapChange::operator=( CBaseObject& moving )
{
	SetMap( moving.WorldNumber() );
	return (*this);
}

//0x3C Packet
//Items in Container (Variable # of bytes)
//  BYTE cmd
//  BYTE[2] blockSize
//  BYTE[2] # of Item segments
//  Item Segments:
//    BYTE[4] itemID
//    BYTE[2] model
//    BYTE unknown1 (0x00)
//    BYTE[2] # of items in stack
//    BYTE[2] xLoc
//    BYTE[2] yLoc
//    BYTE[4] Container ItemID
//    BYTE[2] color
void CPItemsInContainer::InternalReset( void )
{
	internalBuffer.resize( 5 );
	internalBuffer[0]	= 0x3C;
	isVendor			= false;
	vendorSerial		= INVALIDSERIAL;
	isCorpse			= false;
}
CPItemsInContainer::CPItemsInContainer()
{
	InternalReset();
}

CPItemsInContainer::CPItemsInContainer( CSocket *mSock, CItem *container, UI08 contType )
{
	if( ValidateObject( container ) )
	{
		InternalReset();
		if( contType == 0x01 ) // Corpse
			isCorpse = true;
		else if( contType == 0x02 ) // Vendor
		{
			isVendor = true;
			vendorSerial = container->GetSerial();
		}
		CopyData( mSock, (*container) );
	}
}

UI16 CPItemsInContainer::NumberOfItems( void ) const
{
	return static_cast<UI16>( (internalBuffer[3]<<8) + internalBuffer[4] );
}

void CPItemsInContainer::NumberOfItems( UI16 numItems )
{
	UI16 packetSize		= (UI16)((numItems * 19) + 5);
	internalBuffer.resize( packetSize );
	PackShort( &internalBuffer[0], 1, packetSize );
	PackShort( &internalBuffer[0], 3, numItems );
}
void CPItemsInContainer::AddItem( CItem *toAdd, UI16 itemNum )
{
	UI16 baseOffset = (UI16)(5 + itemNum * 19);
	PackLong(  &internalBuffer[0], baseOffset +  0, toAdd->GetSerial() );
	PackShort( &internalBuffer[0], baseOffset +  4, toAdd->GetID() );
	PackShort( &internalBuffer[0], baseOffset +  7, toAdd->GetAmount() );
	PackShort( &internalBuffer[0], baseOffset +  9, toAdd->GetX() );
	PackShort( &internalBuffer[0], baseOffset + 11, toAdd->GetY() );

	if( isVendor )
		PackLong( &internalBuffer[0], baseOffset + 13, vendorSerial );
	else
		PackLong( &internalBuffer[0], baseOffset + 13, toAdd->GetContSerial() );

	PackShort( &internalBuffer[0], baseOffset + 17, toAdd->GetColour() );

	toAdd->SetDecayTime( 0 );
}

void CPItemsInContainer::Add( UI16 itemNum, SERIAL toAdd, SERIAL cont, UI08 amount )
{
	UI16 baseOffset = (UI16)(5 + itemNum * 19);

	PackLong(  &internalBuffer[0], baseOffset,		toAdd );
	PackShort( &internalBuffer[0], baseOffset + 7,	amount );
	PackLong(  &internalBuffer[0], baseOffset + 13, cont );
}

void CPItemsInContainer::CopyData( CSocket *mSock, CItem& toCopy )
{
	UI16 itemCount		= 0;
	bool itemIsCorpse	= toCopy.isCorpse();

	CDataList< CItem * > *tcCont = toCopy.GetContainsList();
	for( CItem *ctr = tcCont->First(); !tcCont->Finished(); ctr = tcCont->Next() )
	{
		if( ValidateObject( ctr ) && ( !isCorpse || !itemIsCorpse || ( itemIsCorpse && ctr->GetLayer() ) ) )
		{
			if( !ctr->isFree() )
			{
				internalBuffer.resize( internalBuffer.size() + 19 );
				AddItem( ctr, itemCount );
				if( !isVendor && mSock != NULL )
				{
					CPQueryToolTip pSend( (*ctr) );
					mSock->Send( &pSend );
				}
				++itemCount;
			}
		}
	}

	NumberOfItems( itemCount );
}

void CPItemsInContainer::Log( std::ofstream &outStream, bool fullHeader )
{
	size_t numItems = UnpackUShort( &internalBuffer[0], 3 );
	if( fullHeader )
		outStream << "[SEND]Packet   : CPItemsInContainer 0x3c --> Length: " << internalBuffer.size() << std::endl;
	outStream << "Block size     : " << UnpackUShort( &internalBuffer[0], 1 ) << std::endl;
	outStream << "Number of Items: " << std::dec << numItems << std::endl;
	int baseOffset = 5;
	for( size_t x = 0; x < numItems; ++x )
	{
		outStream << "  ITEM " << x << "      ID: " << "0x" << std::hex << UnpackULong( &internalBuffer[0], baseOffset ) << std::endl;
		outStream << "      Model     : " << "0x" << UnpackUShort( &internalBuffer[0], baseOffset+=4 ) << std::endl;
		outStream << "      Amount    : " << std::dec << UnpackUShort( &internalBuffer[0], baseOffset+=2 ) << std::endl;
		outStream << "      XYZ       : " << UnpackUShort( &internalBuffer[0], baseOffset+=2 ) << "," <<
			UnpackUShort( &internalBuffer[0], baseOffset+=2 ) << "," << static_cast<UI16>(internalBuffer[baseOffset+=2] ) << std::endl;
		outStream << "      Container : " << "0x" << std::hex << UnpackULong( &internalBuffer[0], ++baseOffset) << std::endl;
		outStream << "      Color     : " << std::dec << UnpackUShort( &internalBuffer[0], baseOffset+=2) << std::endl;
	}
	outStream << "  Raw dump      :" << std::endl;

	cPUOXBuffer::Log( outStream, false );
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
	internalBuffer.resize( 8 );	// start big, and work back down
	internalBuffer[0] = 0x74;
}
CPOpenBuyWindow::CPOpenBuyWindow()
{
	InternalReset();
}
CPOpenBuyWindow::CPOpenBuyWindow( CItem *container, CChar *vendorID )
{
	if( ValidateObject( container ) )
	{
		InternalReset();
		PackLong( &internalBuffer[0], 3, container->GetSerial() );
		CopyData( (*container), vendorID );
	}
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

UI32 calcGoodValue( CTownRegion *tReg, CItem *i, UI32 value, bool isSelling );
UI32 calcValue( CItem *i, UI32 value );
void CPOpenBuyWindow::AddItem( CItem *toAdd, CTownRegion *tReg, UI16 &baseOffset )
{
	UI32 value = calcValue( toAdd, toAdd->GetBuyValue() );
	if( cwmWorldState->ServerData()->TradeSystemStatus() )
		value = calcGoodValue( tReg, toAdd, value, false );

	std::string itemname;
	itemname.reserve( MAX_NAME );
	UI08 sLen = 0;
	UString temp	= toAdd->GetName() ;
	temp			= temp.simplifyWhiteSpace();
	if( temp.substr( 0, 1 ) == "#" )
	{
		itemname = UString::number( 1020000 + toAdd->GetID() );
		sLen = static_cast<UI08>(itemname.size() + 1);
	}
	else
		sLen = static_cast<UI08>(getTileName( (*toAdd), itemname )); // Item name length, don't strip the NULL (3D client doesn't like it)

	internalBuffer.resize( baseOffset + 5 + sLen );
	PackLong( &internalBuffer[0], baseOffset, value );
	internalBuffer[baseOffset += 4] = sLen;

	for( UI08 k = 0; k < sLen; ++k )
		internalBuffer[++baseOffset] = itemname[k];
	++baseOffset;
}

void CPOpenBuyWindow::CopyData( CItem& toCopy, CChar *vendorID )
{
	UI08 itemCount	= 0;
	UI16 length		= 8;
	CTownRegion *tReg = NULL;
	if( cwmWorldState->ServerData()->TradeSystemStatus() && ValidateObject( vendorID ) )
		tReg = calcRegionFromXY( vendorID->GetX(), vendorID->GetY(), vendorID->WorldNumber() );
	CDataList< CItem * > *tcCont = toCopy.GetContainsList();
	for( CItem *ctr = tcCont->First(); !tcCont->Finished(); ctr = tcCont->Next() )
	{
		if( ValidateObject( ctr ) )
		{
			if( !ctr->isFree() )
			{
				AddItem( ctr, tReg, length );
				++itemCount;
			}
		}
	}
	NumberOfItems( itemCount );
	internalBuffer.resize( length );
	PackShort( &internalBuffer[0], 1, length );
}

void CPOpenBuyWindow::Log( std::ofstream &outStream, bool fullHeader )
{
	if( fullHeader )
		outStream << "[SEND]Packet   : CPOpenBuyWindow 0x74 --> Length: " << internalBuffer.size() << std::endl;
	outStream << "Block size     : " << UnpackUShort( &internalBuffer[0], 1 ) << std::endl;
	outStream << "Vendor ID      : " << std::hex << UnpackULong( &internalBuffer[0], 3 ) << std::endl;
	outStream << "Number of Items: " << std::dec << static_cast<UI16>(internalBuffer[7]) << std::endl;
	int baseOffset = 8;
	for( UI32 x = 0; x < internalBuffer[7]; ++x )
	{
		outStream << "  ITEM " << x << "      Price: " << UnpackULong( &internalBuffer[0], baseOffset ) << std::endl;
		baseOffset += 4;
		outStream << "      Len  : " << static_cast<UI16>(internalBuffer[baseOffset]) << std::endl;
		outStream << "      Name : ";
		for( UI08 y = 0; y < internalBuffer[baseOffset]; ++y )
			outStream << internalBuffer[baseOffset + 1 + y];
		baseOffset += internalBuffer[baseOffset] + 1;
		outStream << std::endl;
	}

	outStream << "  Raw dump       :" << std::endl;

	cPUOXBuffer::Log( outStream, false );
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
//	BYTE[4] Flags 
//		0x02 = send config/req logout (IGR?)
//		0x04 = single character (siege)
//		0x08 = enable npcpopup menus
//		0x10 = unknown, 
//		0x20 = enable common AOS features (tooltip thing/fight system book, but not AOS monsters/map/skills)
void CPCharAndStartLoc::Log( std::ofstream &outStream, bool fullHeader )
{
	if( fullHeader )
		outStream << "[SEND]Packet   : CPCharAndStartLoc 0xA9 --> Length: " << internalBuffer.size() << std::endl;
	outStream << "# Chars		 : " << (UI16)internalBuffer[3] << std::endl;
	outStream << "Characters --" << std::endl;
	for( UI08 i = 0; i < 6; ++i )
	{
		UI32 baseOffset = 4 + i * 60;
		outStream << "    Character " << (UI16)i << std::endl;
		outStream << "      Name: ";
		for( UI08 j = 0; j < 30; ++j )
		{
			if( internalBuffer[baseOffset+j] != 0 )
				outStream << internalBuffer[baseOffset+j];
			else
				break;
		}
		outStream << std::endl << "      Pass: ";
		for( UI08 k = 0; k < 30; ++k )
		{
			if( internalBuffer[baseOffset+k+30] != 0 )
				outStream << internalBuffer[baseOffset+k+30];
			else
				break;
		}
		outStream << std::endl;
	}
	outStream << "# Starts       : " << (UI16)internalBuffer[364] << std::endl;
	outStream << "Starting locations --" << std::endl;
	for( UI32 l = 0; l < internalBuffer[364]; ++l )
	{
		UI32 baseOffset = 366 + l * 63;
		outStream << "    Start " << l << std::endl;
		outStream << "      Index : " << (int)internalBuffer[baseOffset] << std::endl;
		outStream << "      General Name: ";
		++baseOffset;
		for( UI08 m = 0; m < 31; ++m )
		{
			if( internalBuffer[baseOffset+m] != 0 )
				outStream << internalBuffer[baseOffset+m];
			else
				break;
		}
		outStream << std::endl << "      Exact Name  : ";
		baseOffset += 31;
		for( UI08 n = 0; n < 31; ++n )
		{
			if( internalBuffer[baseOffset+n] != 0 )
				outStream << internalBuffer[baseOffset+n];
			else
				break;
		}
		outStream << std::endl;
	}
	UI32 lastByte = internalBuffer[internalBuffer.size() - 1];
	outStream << "Flags  		 : " << lastByte << std::endl;
	if( (lastByte&0x02) == 0x02 )
		outStream << "       		 : Send config/request logout" << std::endl;
	if( (lastByte&0x04) == 0x04 )
		outStream << "       		 : Single character" << std::endl;
	if( (lastByte&0x08) == 0x08 )
		outStream << "       		 : Enable NPC Popup menus" << std::endl;
	if( (lastByte&0x10) == 0x10 )
		outStream << "       		 : Unknown" << std::endl;
	if( (lastByte&0x20) == 0x20 )
		outStream << "       		 : Enable Common AoS features" << std::endl;
	outStream << "  Raw dump     :" << std::endl;
	cPUOXBuffer::Log( outStream, false );
}

void CPCharAndStartLoc::InternalReset( void )
{
	internalBuffer.resize( 4 );
	internalBuffer[0] = 0xA9;
}

void CPCharAndStartLoc::CopyData(ACCOUNTSBLOCK& toCopy )
{
	for( UI08 i = 0; i < 6; ++i )
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
	// was 305 +, now 309 +
	UI16 packetSize;
	if( internalBuffer[3] > 5 )
		packetSize = (UI16)(369 + 63 * numLocations);
	else
		packetSize = (UI16)(309 + 63 * numLocations);
	internalBuffer.resize( packetSize );
	internalBuffer[1] = (UI08)(packetSize>>8);
	internalBuffer[2] = (UI08)(packetSize%256);
	// If we are going to support the 6char client flag then we need to make sure we push this offset furtner down.
	if( internalBuffer[3] > 5 )
		internalBuffer[364] = numLocations;
	else
		internalBuffer[304] = numLocations;
	// turn on /*send config,*/ npcpopup menus and common AOS features
	internalBuffer[packetSize - 1] = ( 0x08 | 0x20 | 0x40 );
}
void CPCharAndStartLoc::NumberOfCharacters( UI08 numCharacters )
{
	internalBuffer[3] = numCharacters;
}

void CPCharAndStartLoc::AddCharacter( CChar *toAdd, UI08 charOffset )
{
	if( !ValidateObject( toAdd ) )
		return;
	UI16 baseOffset = (UI16)(4 + charOffset * 60);
	strncpy( (char *)&internalBuffer[baseOffset], toAdd->GetName().c_str(), 59 );
	// extra 30 bytes unused, as we don't use a character password
}

void CPCharAndStartLoc::AddStartLocation( LPSTARTLOCATION sLoc, UI08 locOffset )
{
	if( sLoc == NULL )
		return;
	UI16 baseOffset;
	if( internalBuffer[3] > 5 )
			baseOffset = (UI16)(365 + locOffset * 63);
	else
			baseOffset = (UI16)(305 + locOffset * 63);
	internalBuffer[baseOffset]	= locOffset;
	size_t townLen				= strlen( sLoc->town );
	size_t descLen				= strlen( sLoc->description );
	for( size_t i = 0; i < townLen; ++i )
		internalBuffer[baseOffset+i+1] = sLoc->town[i];
	for( size_t j = 0; j < descLen; ++j )
		internalBuffer[baseOffset+j+32] = sLoc->description[j];
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
	size_t y = strlen( tipData ) + 10;
	SetLength( static_cast< UI16 >(y) );
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
CPGraphicalEffect2::CPGraphicalEffect2( UI08 effectType, CBaseObject &src, CBaseObject &trg ) : CPGraphicalEffect( effectType, src, trg )
{
	InternalReset();
	Effect( effectType );
	SourceSerial( src );
	TargetSerial( trg );
}
CPGraphicalEffect2::CPGraphicalEffect2( UI08 effectType, CBaseObject &src ) : CPGraphicalEffect( effectType, src )
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

//0x78 Packet
//Draw object (Variable # of bytes) 
//	BYTE cmd 
//	BYTE[2] blockSize 
//	BYTE[4] itemID/playerID 
//	BYTE[2] model (item hex #) 
//	if (itemID & 0x80000000) 
//		BYTE[2] amount/Corpse Model Num 
//	BYTE[2] xLoc (only 15 lsb) 
//	BYTE[2] yLoc 
//	if (xLoc & 0x8000) 
//		BYTE direction 
//	BYTE zLoc 
//	BYTE direction 
//	BYTE[2] dye/skin color 
//	BYTE flag 
//	BYTE notoriety (2's complement signed) 
//	if (BYTE[4] == 0x00 0x00 0x00 0x00) 
//		DONE 
//	else loop this until above if statement is satisified
//		BYTE[4] itemID 
//		BYTE[2] model (item hex # - only 15 lsb) 
//		BYTE layer 
//		if (model & 0x8000) 
//			BYTE[2] hue 
//
CPDrawObject::CPDrawObject()
{
	InternalReset();
}

CPDrawObject::CPDrawObject( CChar &mChar )
{
	InternalReset();
	CopyData( mChar );
}

void CPDrawObject::InternalReset( void )
{
	SetLength( 19 );
	internalBuffer[0] = 0x78;
}

void CPDrawObject::SetLength( UI16 len )
{
	internalBuffer.resize( len );
	PackShort( &internalBuffer[0], 1, len );
	curLen = len;
}

void CPDrawObject::Finalize( void )
{
	UI16 cPos = curLen;
	SetLength( curLen + 4 );
	PackLong( &internalBuffer[0], cPos, static_cast<UI32>(0) );
}

void CPDrawObject::AddItem( CItem *toAdd )
{
	UI16 cPos = curLen;
	bool bColour = ( toAdd->GetColour() != 0 );
	if( bColour )
		SetLength( curLen + 9 );
	else
		SetLength( curLen + 7 );

	PackLong(  &internalBuffer[0], cPos, toAdd->GetSerial() );
	PackShort( &internalBuffer[0], cPos+=4, toAdd->GetID() );
	internalBuffer[cPos+=2] = toAdd->GetLayer();

	if( bColour )
	{
		internalBuffer[cPos-2] |= 0x80;
		PackShort( &internalBuffer[0], ++cPos, toAdd->GetColour() );
	}
}

void CPDrawObject::SetRepFlag( UI08 value )
{
	internalBuffer[18] = value;
}

void CPDrawObject::CopyData( CChar& mChar )
{
	PackLong(  &internalBuffer[0],  3, mChar.GetSerial() );
	PackShort( &internalBuffer[0],  7, mChar.GetID() );
	PackShort( &internalBuffer[0],  9, mChar.GetX() );
	PackShort( &internalBuffer[0], 11, mChar.GetY() );
	internalBuffer[13] = mChar.GetZ();
	internalBuffer[14] = mChar.GetDir();
	PackShort( &internalBuffer[0], 15, mChar.GetSkin() );

	UI08 cFlag = 0;
	if( mChar.GetPoisoned() )
		cFlag |= 0x04;
	if( ( !mChar.IsNpc() && !isOnline( mChar ) ) || ( mChar.GetVisible() != VT_VISIBLE )  || ( mChar.IsDead() && !mChar.IsAtWar() ) )
		cFlag |= 0x80;
	if( mChar.IsAtWar() )
		cFlag |= 0x40;
	internalBuffer[17] = cFlag;
}

//0x89 Packet
//Corpse Clothing (Variable # of bytes) 
//	BYTE cmd 
//	BYTE[2] blockSize
//	BYTE[4] corpseID
//		BYTE itemLayer 
//		BYTE[4] itemID
//	BYTE terminator (0x00)
//Followed by a 0x3C message with the contents.

void CPCorpseClothing::InternalReset( void )
{
	internalBuffer.resize( 7 );
	internalBuffer[0] = 0x89;
}
CPCorpseClothing::CPCorpseClothing()
{
	InternalReset();
}
CPCorpseClothing::CPCorpseClothing( CItem *corpse )
{
	InternalReset();
	if( ValidateObject( corpse ) )
		CopyData( (*corpse) );
}

void CPCorpseClothing::NumberOfItems( UI16 numItems )
{
	// knowing the number of items, set the packet size
	UI16 packetSize = (UI16)((numItems * 5) + 8);		// 7 for lead in, 1 for lead out
	internalBuffer.resize( packetSize );
	internalBuffer[1] = (UI08)(packetSize>>8);
	internalBuffer[2] = (UI08)(packetSize%256);
	internalBuffer[packetSize - 1] = 0;
}
void CPCorpseClothing::AddItem( CItem *toAdd, UI16 itemNum )
{
	UI16 baseOffset = (UI16)(7 + itemNum * 5);
	internalBuffer[baseOffset]	= toAdd->GetLayer();
	PackLong( &internalBuffer[0], baseOffset + 1, toAdd->GetSerial() );
}
CPCorpseClothing& CPCorpseClothing::operator=( CItem& corpse )
{
	CopyData( corpse );
	return (*this);
}

void CPCorpseClothing::CopyData( CItem& toCopy )
{
	PackLong( &internalBuffer[0], 3, toCopy.GetSerial() );
	UI16 itemCount = 0;
	CDataList< CItem * > *tcCont = toCopy.GetContainsList();
	for( CItem *ctr = tcCont->First(); !tcCont->Finished(); ctr = tcCont->Next() )
	{
		if( ValidateObject( ctr ) )
		{
			if( !ctr->isFree() && ctr->GetLayer() )
			{
				internalBuffer.resize( internalBuffer.size() + 5 );
				AddItem( ctr, itemCount );
				++itemCount;
			}
		}
	}
	NumberOfItems( itemCount );
}

//0x1A Packet
//Last Modified on Saturday, 13-Apr-1999 
//Object Information (Variable # of bytes) 
//	BYTE cmd 
//	BYTE[2] blockSize 
//	BYTE[4] itemID 
//	BYTE[2] model # 
//	if (itemID & 0x80000000) 
//		BYTE[2] item count (or model # for corpses) 
//	if (model & 0x8000) 
//		BYTE Incr Counter (increment model by this #) 
//	BYTE[2] xLoc (only use lowest significant 15 bits) 
//	BYTE[2] yLoc 
//	if (xLoc & 0x8000) 
//		BYTE direction 
//	BYTE zLoc 
//	if (yLoc & 0x8000) 
//		BYTE[2] dye 
//	if (yLoc & 0x4000) 
//		BYTE flag byte (See top)
void CPObjectInfo::InternalReset( void )
{
	internalBuffer.resize( 16 );
	internalBuffer[0] = 0x1A;
	internalBuffer[2] = 16;
}
void CPObjectInfo::CopyData( CItem& mItem, CChar& mChar )
{
	PackLong( &internalBuffer[0], 3, mItem.GetSerial() );

	if( mItem.CanBeObjType( OT_MULTI ) )
		CopyMultiData( static_cast<CMultiObj&>(mItem), mChar );
	else if( mItem.CanBeObjType( OT_ITEM ) )
		CopyItemData( mItem, mChar );
}

void CPObjectInfo::CopyItemData( CItem &mItem, CChar &mChar )
{
	if( mItem.isPileable() || mItem.isCorpse() )
	{
		internalBuffer.resize( 18 );
		internalBuffer[2] = 18;
		internalBuffer[3] |= 0x80;	// Enable piles
	}

	bool isInvisible	= (mItem.GetVisible() != VT_VISIBLE);
	bool isMovable		= (mItem.GetMovable() == 1 || mChar.AllMove() || ( mItem.IsLockedDown() && &mChar == mItem.GetOwnerObj() ));
	// if player is a gm, this item
	// is shown like a candle (so that he can move it),
	// ....if not, the item is a normal
	// invisible light source!
	if( mChar.IsGM() && mItem.GetID() == 0x1647 )
		PackShort( &internalBuffer[0], 7, 0x0A0F );
	else
		PackShort( &internalBuffer[0], 7, mItem.GetID() );

	UI08 byteNum = 7;
	if( mItem.isPileable() || mItem.isCorpse() )
		PackShort( &internalBuffer[0],  byteNum+=2, mItem.GetAmount() );

	PackShort( &internalBuffer[0], byteNum+=2, mItem.GetX() );
	if( isInvisible || isMovable )
	{
		PackShort( &internalBuffer[0], byteNum+=2, mItem.GetY() + 0xC000 );
		internalBuffer.resize( internalBuffer.size()+1 );
		++internalBuffer[2];
	}
	else
		PackShort( &internalBuffer[0], byteNum+=2, mItem.GetY() + 0x4000 );
	if( mItem.GetDir() )
	{
		internalBuffer.resize( internalBuffer.size()+1 );
		internalBuffer[byteNum-2]	|= 0x80;	// Enable direction
		internalBuffer[byteNum+=2]	= mItem.GetDir();
		internalBuffer[++byteNum]	= mItem.GetZ();
		++internalBuffer[2];
	}
	else
		internalBuffer[byteNum+=2] = mItem.GetZ();

	if( mChar.IsGM() && mItem.GetID() == 0x1647 )
		PackShort( &internalBuffer[0], ++byteNum, 0x00C6 );
	else
		PackShort( &internalBuffer[0], ++byteNum, mItem.GetColour() );

	if( isInvisible || isMovable )
	{
		internalBuffer[byteNum+=2] = 0;
		if( isInvisible )
			internalBuffer[byteNum] |= 0x80;

		if( isMovable ) 
			internalBuffer[byteNum] |= 0x20;
	}
}

void CPObjectInfo::CopyMultiData( CMultiObj& mMulti, CChar &mChar )
{
	bool isInvisible	= (mMulti.GetVisible() != VT_VISIBLE);
	bool isMovable		= (mChar.AllMove());

	if( mChar.ViewHouseAsIcon() && mMulti.GetID() >= 0x4000 )
		PackShort( &internalBuffer[0], 7, 0x14F0 );
	else
		PackShort( &internalBuffer[0], 7, mMulti.GetID() );

	PackShort( &internalBuffer[0], 9, mMulti.GetX() );
	if( isInvisible || isMovable )
	{
		PackShort( &internalBuffer[0], 11, mMulti.GetY() + 0xC000 );
		internalBuffer.resize( internalBuffer.size()+1 );
		++internalBuffer[2];
	}
	else
		PackShort( &internalBuffer[0], 11, mMulti.GetY() + 0x4000 );
	UI08 byteNum = 12;
	if( mMulti.GetDir() )
	{
		internalBuffer.resize( internalBuffer.size()+1 );
		internalBuffer[9]			|= 0x80;	// Enable direction
		internalBuffer[++byteNum]	= mMulti.GetDir();
		++internalBuffer[2];
	}
	internalBuffer[++byteNum] = mMulti.GetZ();

	PackShort( &internalBuffer[0], ++byteNum, mMulti.GetColour() );
	if( isInvisible || isMovable )
	{
		internalBuffer[++byteNum] = 0;
		if( isInvisible )
			internalBuffer[byteNum] |= 0x80;

		if( isMovable ) 
			internalBuffer[byteNum] |= 0x20;
	}
}

CPObjectInfo::CPObjectInfo()
{
	InternalReset();
}
CPObjectInfo::CPObjectInfo( CItem& mItem, CChar& mChar )
{
	InternalReset();
	CopyData( mItem, mChar );
}

void CPObjectInfo::Objects( CItem& mItem, CChar& mChar )
{
	CopyData( mItem, mChar );
}


//0x1C Packet
//Last Modified on Friday, 20-Apr-1998 
//Send Speech (Variable # of bytes) 
//	BYTE cmd										0
//	BYTE[2] blockSize								1
//	BYTE[4] itemID (FF FF FF FF = system)			3
//	BYTE[2] model (item hex # - FF FF = system)		7
//	BYTE Type										9
//	BYTE[2] Text Color								10
//	BYTE[2] Font									12
//	BYTE[30] Name									14
//	BYTE[?] Null-Terminated Message (? = blockSize - 44) 
void CPSpeech::Object( CBaseObject &talking )
{
	CopyData( talking );
}
void CPSpeech::Object( CPITalkRequest &tSaid )
{
	CopyData( tSaid );
}
void CPSpeech::Type( UI08 value )
{
	internalBuffer[9] = value;
}
void CPSpeech::Colour( COLOUR value )
{
	// Ideally one would track down where the value is getting set to 0, 
	// a color of 0 will crash 3d client
	if( value == 0 )
	{
		value = 0x3B2;
	}
	PackShort( &internalBuffer[0], 10, value );
}
void CPSpeech::Font( UI16 value )
{
	PackShort( &internalBuffer[0], 12, value );
}
void CPSpeech::Name( std::string value )
{
	strncpy( (char *)&internalBuffer[14], value.c_str(), 30 );
}
void CPSpeech::Message( char *value )
{
	size_t length = strlen( value );
	SetLength( static_cast< UI16 >(44 + length + 1) );
	strncpy( (char *)&internalBuffer[44], value, length );
}
void CPSpeech::SetLength( UI16 value )
{
	internalBuffer.resize( value );
	PackShort( &internalBuffer[0], 1, value );
}

void CPSpeech::InternalReset( void )
{
	SetLength( 44 );
	internalBuffer[0] = 0x1C;
	Colour( 0x3B2 );
}

CPSpeech::CPSpeech()
{
	InternalReset();
}
CPSpeech::CPSpeech( CBaseObject &toCopy )
{
	InternalReset();
	CopyData( toCopy );
}
CPSpeech::CPSpeech( CPITalkRequest &toCopy )
{
	InternalReset();
	CopyData( toCopy );
}
CPSpeech &CPSpeech::operator=( CBaseObject &toCopy )
{
	CopyData( toCopy );
	return (*this);
}
void CPSpeech::CopyData( CBaseObject &toCopy )
{
	Serial( toCopy.GetSerial() );
	ID( toCopy.GetID() );
	Name( toCopy.GetName() );
//	internalBuffer[9] = 1;
}
void CPSpeech::CopyData( CPITalkRequest &talking )
{
	Colour( talking.TextColour() );
	Font( talking.Font() );
	Type( talking.Type() );
	Message( talking.Text() );
}
void CPSpeech::Serial( SERIAL toSet )
{
	PackLong( &internalBuffer[0], 3, toSet );
}
void CPSpeech::ID( UI16 toSet )
{
	PackShort( &internalBuffer[0], 7, toSet );
}
void CPSpeech::GrabSpeech( CSocket *mSock, CChar *mChar )
{
	internalBuffer[10] = mSock->GetByte( 4 );
	internalBuffer[11] = mSock->GetByte( 5 );
	internalBuffer[12] = mSock->GetByte( 6 );
	internalBuffer[13] = mChar->GetFontType();
}

void CPSpeech::GhostIt( UI08 method )
{
	// Method ignored currently
	// Designed with the idea that you can garble text in more than one way
	// eg 0 == ghost, 1 == racial, 2 == magical, etc etc
	for( UI16 j = 44; j < internalBuffer.size() - 1; ++j )
	{
		if( internalBuffer[j] != 32 )
			internalBuffer[j] = ( RandomNum( 0, 1 ) == 0 ? 'O' : 'o' );
	}
}
CPSpeech &CPSpeech::operator=( CPSpeech &copyFrom )
{
	internalBuffer.resize( copyFrom.internalBuffer.size() );
	for( size_t i = 0; i < copyFrom.internalBuffer.size(); ++i )
		internalBuffer[i] = copyFrom.internalBuffer[i];
	return (*this);
}

//	0xAE Packet
//	Last Modified on Wednesday, 11-Nov-1998 
//	Unicode Speech message(Variable # of bytes) 
//	
//	BYTE cmd 
//	BYTE[2] blockSize
//	BYTE[4] ID
//	BYTE[2] Model
//	BYTE Type
//	BYTE[2] Color
//	BYTE[2] Font
//	BYTE[4] Language
//	BYTE[30] Name
//	BYTE[?][2] Msg – Null Terminated (blockSize - 48)
void CPUnicodeSpeech::Object( CBaseObject &talking )
{
	CopyData( talking );
}
void CPUnicodeSpeech::Object( CPITalkRequestAscii &tSaid )
{
	CopyData( tSaid );
}
void CPUnicodeSpeech::Object( CPITalkRequestUnicode &tSaid )
{
	CopyData( tSaid );
}
void CPUnicodeSpeech::Type( UI08 value )
{
	internalBuffer[9] = ( value & 0x0F );
}
void CPUnicodeSpeech::Colour( COLOUR value )
{
	PackShort( &internalBuffer[0], 10, value );
}
void CPUnicodeSpeech::Font( UI16 value )
{
	PackShort( &internalBuffer[0], 12, value );
}
void CPUnicodeSpeech::Language( char *value )
{
	strncpy( (char *)&internalBuffer[14], value, 4 );
}
void CPUnicodeSpeech::Name( std::string value )
{
	strncpy( (char *)&internalBuffer[18], value.c_str(), 30 );
}
void CPUnicodeSpeech::Message( char *value )
{
	size_t length = strlen( value );
	SetLength( static_cast< UI16 >(48 + 2 * length + 2) );
	for( UI16 i = 0; i < length; ++i )
		internalBuffer[49 + i * 2] = value[i];
}
void CPUnicodeSpeech::SetLength( UI16 value )
{
	internalBuffer.resize( value );
	PackShort( &internalBuffer[0], 1, value );
}

void CPUnicodeSpeech::InternalReset( void )
{
	SetLength( 48 );
	internalBuffer[0] = 0xAE;
	Language( "ENU" );
}

CPUnicodeSpeech::CPUnicodeSpeech()
{
	InternalReset();
}
CPUnicodeSpeech::CPUnicodeSpeech( CBaseObject &toCopy )
{
	InternalReset();
	CopyData( toCopy );
}
CPUnicodeSpeech::CPUnicodeSpeech( CPITalkRequestAscii &toCopy )
{
	InternalReset();
	CopyData( toCopy );
}
CPUnicodeSpeech::CPUnicodeSpeech( CPITalkRequestUnicode &toCopy )
{
	InternalReset();
	CopyData( toCopy );
}
CPUnicodeSpeech &CPUnicodeSpeech::operator=( CBaseObject &toCopy )
{
	CopyData( toCopy );
	return (*this);
}
void CPUnicodeSpeech::CopyData( CBaseObject &toCopy )
{
	Serial( toCopy.GetSerial() );
	ID( toCopy.GetID() );
	Name( toCopy.GetName() );
//	internalBuffer[9] = 1;
}
void CPUnicodeSpeech::CopyData( CPITalkRequestAscii &talking )
{
	Colour( talking.TextColour() );
	Font( talking.Font() );
	Type( talking.Type() );
	Language( "ENU" );
	Message( talking.Text() );
}
void CPUnicodeSpeech::CopyData( CPITalkRequestUnicode &talking )
{
	Colour( talking.TextColour() );
	Font( talking.Font() );
	Type( talking.Type() );
	Language( talking.Language() );
//	Message( talking.Text() );

	UI16 length = talking.Length();
	char *uniTxt = talking.UnicodeText();
	
	SetLength( 48 + 2 * length + 2 );
	for( UI16 i = 0; i < (2 * length); ++i )
		internalBuffer[48 + i] = uniTxt[i];
}
void CPUnicodeSpeech::Serial( SERIAL toSet )
{
	PackLong( &internalBuffer[0], 3, toSet );
}
void CPUnicodeSpeech::ID( UI16 toSet )
{
	PackShort( &internalBuffer[0], 7, toSet );
}
void CPUnicodeSpeech::GrabSpeech( CSocket *mSock, CChar *mChar )
{
	internalBuffer[10] = mSock->GetByte( 4 );
	internalBuffer[11] = mSock->GetByte( 5 );
	internalBuffer[12] = mSock->GetByte( 6 );
	internalBuffer[13] = mChar->GetFontType();
}

void CPUnicodeSpeech::GhostIt( UI08 method )
{
	// Method ignored currently
	// Designed with the idea that you can garble text in more than one way
	// eg 0 == ghost, 1 == racial, 2 == magical, etc etc
	for( UI16 j = 49; j < internalBuffer.size() - 1; j += 2 )
	{
		if( internalBuffer[j] != 32 )
			internalBuffer[j] = ( RandomNum( 0, 1 ) == 0 ? 'O' : 'o' );
	}
}
CPUnicodeSpeech &CPUnicodeSpeech::operator=( CPUnicodeSpeech &copyFrom )
{
	internalBuffer.resize( copyFrom.internalBuffer.size() );
	for( size_t i = 0; i < copyFrom.internalBuffer.size(); ++i )
		internalBuffer[i] = copyFrom.internalBuffer[i];
	return (*this);
}

//	0xA8 Packet
//	Last Modified on Monday, 13-Apr-1998 17:06:50 EDT 
//	Game Server List (Variable # of bytes) 
//	BYTE cmd 
//	BYTE[2] blockSize 
//	BYTE System Info Flag 
//		0xCC - Don't send 
//		0x64 - Send Video card 
//		?? - 
//	BYTE[2] # of servers 
//	Then each server -- 
//		BYTE[2] serverIndex (0-based) 
//		BYTE[32] serverName 
//		BYTE percentFull 
//		BYTE timezone 
//		BYTE[4] pingIP 
void CPGameServerList::InternalReset( void )
{
	internalBuffer.resize( 6 );
	internalBuffer[0] = 0xA8;
	internalBuffer[2] = 6;
	internalBuffer[3] = 0xFF;
}
CPGameServerList::CPGameServerList()
{
	InternalReset();
}
CPGameServerList::CPGameServerList( UI16 numServers )
{
	InternalReset();
	NumberOfServers( numServers );
}
void CPGameServerList::NumberOfServers( UI16 numItems )
{
	UI32 tlen = 6 + ( numItems * 40 );
	internalBuffer.resize( tlen );
	PackShort( &internalBuffer[0], 1, static_cast< UI16 >(tlen) );
	PackShort( &internalBuffer[0], 4, numItems );
}
void CPGameServerList::AddServer( UI16 servNum, physicalServer *data )
{
	UI32 baseOffset = 6 + servNum * 40;
	PackShort( &internalBuffer[0], baseOffset, servNum + 1 );
	strcpy( (char *)&internalBuffer[baseOffset + 2], data->getName().c_str() );
	UI32 ip = htonl( inet_addr( data->getIP().c_str() ) );
	PackLong( &internalBuffer[0], baseOffset + 36, ip );
}

//	0x6F Packet
//	Last Modified on Friday, 20-Nov-1998 
//	Secure Trading (Variable # of bytes) 
//	BYTE cmd 
//	BYTE[2] blockSize 
//	BYTE action
//	BYTE[4] id1
//	BYTE[4] id2
//	BYTE[4] id3
//	BYTE nameFollowing (0 or 1)
//	If (nameFollowing = 1) 
//	BYTE[?] charName 
void CPSecureTrading::InternalReset( void )
{
	internalBuffer.resize( 47 );
	internalBuffer[0] = 0x6F;
	internalBuffer[2] = 47;
}
void CPSecureTrading::CopyData( CBaseObject& mItem, CBaseObject& mItem2, CBaseObject& mItem3 )
{
	PackLong( &internalBuffer[0], 4,  mItem.GetSerial()  );
	PackLong( &internalBuffer[0], 8,  mItem2.GetSerial() );
	PackLong( &internalBuffer[0], 12, mItem3.GetSerial() );
}
void CPSecureTrading::CopyData( CBaseObject& mItem, SERIAL mItem2, SERIAL mItem3 )
{
	PackLong( &internalBuffer[0], 4,  mItem.GetSerial() );
	PackLong( &internalBuffer[0], 8,  mItem2			);
	PackLong( &internalBuffer[0], 12, mItem3			);
}
CPSecureTrading::CPSecureTrading()
{
	InternalReset();
}
CPSecureTrading::CPSecureTrading( CBaseObject& mItem, CBaseObject& mItem2, CBaseObject& mItem3 )
{
	InternalReset();
	CopyData( mItem, mItem2, mItem3 );
}
CPSecureTrading::CPSecureTrading( CBaseObject& mItem, SERIAL mItem2, SERIAL mItem3 )
{
	InternalReset();
	CopyData( mItem, mItem2, mItem3 );
}

void CPSecureTrading::Objects( CBaseObject& mItem, CBaseObject& mItem2, CBaseObject& mItem3 )
{
	CopyData( mItem, mItem2, mItem3 );
}
void CPSecureTrading::Objects( CBaseObject& mItem, SERIAL mItem2, SERIAL mItem3 )
{
	CopyData( mItem, mItem2, mItem3 );
}
void CPSecureTrading::Action( UI08 value )
{
	internalBuffer[3] = value;
}
void CPSecureTrading::Name( const std::string nameFollowing )
{
	internalBuffer.resize( 47 );
	internalBuffer[16] = 1;
	strcpy( (char *)&internalBuffer[17], nameFollowing.c_str() );
}

//	0x98 Packet
//	Last Modified on Monday, 04-Aug-2002 18:34:08 EDT 
//	All-names “3D” (Variable # of Bytes)
//	BYTE cmd 
//	BYTE[2] blocksize
//	BYTE[4] ID
//	If (server-reply) BYTE[30] name (0 terminated)

//	NOTE: Only 3D clients send this packet 
//	Server and client packet.

//	Client asks for name of object with ID x.
//	Server has to reply with ID + name
//	Client automatically knows names of items.
//	Hence it only asks only for NPC/Player names nearby, but shows bars of items plus NPC’s.

//	Client request has 7 bytes, server-reply 37
//	Triggered by Crtl + Shift. 
void CPAllNames3D::InternalReset( void )
{
	internalBuffer.resize( 37 );
	internalBuffer[0] = 0x98;
	internalBuffer[2] = 37;
}
void CPAllNames3D::CopyData( CBaseObject& obj )
{
	PackLong( &internalBuffer[0], 3, obj.GetSerial() );
	strcpy( (char *)&internalBuffer[7], obj.GetName().c_str() );
}
CPAllNames3D::CPAllNames3D()
{
	InternalReset();
}
CPAllNames3D::CPAllNames3D( CBaseObject& obj )
{
	InternalReset();
	CopyData( obj );
}
void CPAllNames3D::Object( CBaseObject& obj )
{
	CopyData( obj );
}

//	0x66 Packet
//	Last Modified on Monday, 19’th-Feb-2002 
//	Books - Page (Variable # of bytes) 
//	BYTE cmd 
//	BYTE[2] blockSize 
//	BYTE[4] bookID 
//	BYTE[2] # of pages in this packet 
//	For each page: 
//		BYTE[2] page # 
//		BYTE[2] # of lines on page 
//		Repeated for each line: 
//			BYTE[var] null terminated line 
//	Note: 
//	server side: # of pages equals value given in 0x93/0xd4
//	EACH page # given. If empty: # lines: 0 + terminator (=3 0’s)
//	client side:  # of pages always 1. if 2 pages changed, client generates 2 packets. 

void CPBookPage::IncLength( UI08 amount )
{
	bookLength += amount;
	internalBuffer.resize( bookLength );
}
void CPBookPage::InternalReset( void )
{
	internalBuffer.resize( 9 );
	bookLength			= 9;
	pageCount			= 0;
	internalBuffer[0]	= 0x66;
}
void CPBookPage::CopyData( CItem& obj )
{
	Serial( obj.GetSerial() );
}
CPBookPage::CPBookPage()
{
	InternalReset();
}
CPBookPage::CPBookPage( CItem& obj )
{
	InternalReset();
	CopyData( obj );
}
void CPBookPage::Object( CItem& obj )
{
	CopyData( obj );
}
void CPBookPage::NewPage( SI16 pNum )
{
	++pageCount;	// 1 based counter
	UI16 baseOffset = bookLength;
	IncLength( 4 );
	if( pNum == -1 )
		PackShort( &internalBuffer[0], baseOffset, pageCount );
	else
		PackShort( &internalBuffer[0], baseOffset, pNum );
	internalBuffer[baseOffset + 3] = 8;	// 8 lines per page
}
void CPBookPage::AddLine( const std::string line )
{
	UI16 baseOffset = bookLength;
	size_t strLen	= line.length() + 1;
	IncLength( static_cast< UI08 >(strLen) );
	strcpy( (char *)&internalBuffer[baseOffset], line.c_str() );
}
void CPBookPage::Finalize( void )
{
	PackShort( &internalBuffer[0], 1, bookLength );
	PackShort( &internalBuffer[0], 7, pageCount );
}
void CPBookPage::Serial( SERIAL value )
{
	PackLong( &internalBuffer[0], 3, value );
}

//	0xB0 Packet
//	Last Modified on Tuesday, 20-Apr-1999 
//	Send Gump Menu Dialog (Variable # of bytes) 
//	BYTE cmd											0
//	BYTE[2] blockSize									1
//	BYTE[4] id											3
//	BYTE[4] gumpid										7
//	BYTE[4] x											11
//	BYTE[4] y											15
//	BYTE[2] command section length						19
//	BYTE[?] commands (zero terminated)					21 + ??
//	BYTE[2] numTextLines
//	BYTE[2] text length (in unicode (2 byte) characters.)
//	BYTE[?] text (in unicode)

//	STRINGLIST		commands, text;
CPSendGumpMenu::CPSendGumpMenu()
{
	internalBuffer.resize( 21 );
	internalBuffer[0] = 0xB0;		// command byte
	internalBuffer[14] = 0x6E;		// default x
	internalBuffer[18] = 0x46;		// default y
}
void CPSendGumpMenu::UserID( SERIAL value )
{
	PackLong( &internalBuffer[0], 3, value );
}
void CPSendGumpMenu::GumpID( SERIAL value )
{
	PackLong( &internalBuffer[0], 7, value );
}
void CPSendGumpMenu::X( UI32 value )
{
	PackLong( &internalBuffer[0], 11, value );
}
void CPSendGumpMenu::Y( UI32 value )
{
	PackLong( &internalBuffer[0], 15, value );
}
void CPSendGumpMenu::AddCommand( const char *actualCommand, ... )
{
	va_list argptr;
	char msg[512];
#ifdef __NONANSI_VASTART__
	va_start( argptr );
#else
	va_start( argptr, actualCommand );
#endif
	vsprintf( msg, actualCommand, argptr );
	va_end( argptr );

	if( strlen( msg ) == 0 )
		throw new std::runtime_error( "Blank command field added!" );

#if defined( UOX_DEBUG_MODE )
	Console << msg << myendl;
#endif

	commands.push_back( msg );
}

void CPSendGumpMenu::AddCommand( const std::string actualCommand, ... )
{
	va_list argptr;
	char msg[512];
#ifdef __NONANSI_VASTART__
	va_start( argptr, actualCommand.c_str() );
#else
	va_start( argptr, actualCommand );
	
#endif
	vsprintf( msg, actualCommand.c_str(), argptr );
	va_end( argptr );

	if( strlen( msg ) == 0 )
		throw new std::runtime_error( "Blank command field added!" );

#if defined( UOX_DEBUG_MODE )
	Console << msg << myendl;
#endif

	commands.push_back( msg );
}

void CPSendGumpMenu::AddText( const char *actualText, ... )
{
	va_list argptr;
	char msg[512];
#ifdef __NONANSI_VASTART__
	va_start( argptr );
#else
	va_start( argptr, actualText );
#endif
	vsprintf( msg, actualText, argptr );
	va_end( argptr );

	if( strlen( msg ) == 0 )
		throw new std::runtime_error( "Blank text field added!" );

#if defined( UOX_DEBUG_MODE )
	Console << msg << myendl;
#endif

	text.push_back( msg );
}

void CPSendGumpMenu::AddText( const std::string actualText, ... )
{
	va_list argptr;
	char msg[512];
#ifdef __NONANSI_VASTART__
	va_start( argptr, actualText.c_str() );
#else
	va_start( argptr, actualText );
	
#endif
	vsprintf( msg, actualText.c_str(), argptr );
	va_end( argptr );

	if( strlen( msg ) == 0 )
		throw new std::runtime_error( "Blank text field added!" );

#if defined( UOX_DEBUG_MODE )
	Console << msg << myendl;
#endif

	text.push_back( msg );
}

void CPSendGumpMenu::Finalize( void )
{
	UI32 length		= 21;
	UI32 length2	= 1;
	UI16 increment	= 0;
	size_t line		= 0;

	std::string cmdString;

	for( line = 0; line < commands.size(); ++line )
	{
		if( commands[line].length() == 0 )
			break;
		increment = static_cast< UI16 >(commands[line].length() + 4);
		internalBuffer.resize( length + increment );
		cmdString = "{ " + commands[line] + " }";
		PackString( &internalBuffer[0], length, cmdString, increment );
		length	+= increment;
		length2	+= increment;
	}

	if( length2 > 65536 )
		throw std::runtime_error( "Packet 0xB0 is far too large" );

	PackShort( &internalBuffer[0], 19, static_cast<UI16>(length2) );

	UI32 tlOff	= length + 1;
	length		+= 3;
	UI32 tlines = 0;

	internalBuffer.resize( length );	// match the 3 byte increase
	UI16 lineLen = 0;

	for( line = 0; line < text.size(); ++line )
	{
		if( text[line].length() == 0 )
			break;
		// Unfortunately, unicode strings are... different
		// so we can't use PackString
		lineLen		= static_cast< UI16 >(text[line].length());
		increment	= lineLen * 2 + 2;
		internalBuffer.resize( length + increment );
		PackShort( &internalBuffer[0], length, lineLen );
		for( UI32 i = 0; i < lineLen; ++i )
			internalBuffer[length + 3 + i*2] = (text[line])[i];
		length += increment;
		++tlines;
	}
	
	if( length > 65536 )
		throw std::runtime_error( "Packet 0xB0 is far too large" );

	PackShort( &internalBuffer[0], 1, static_cast< UI16 >(length) );
	PackShort( &internalBuffer[0], tlOff, static_cast< UI16 >(tlines) );
}

//	0xB0 Packet
//	Last Modified on Tuesday, 20-Apr-1999 
//	Send Gump Menu Dialog (Variable # of bytes) 
//	BYTE cmd											0
//	BYTE[2] blockSize									1
//	BYTE[4] id											3
//	BYTE[4] gumpid										7
//	BYTE[4] x											11
//	BYTE[4] y											15
//	BYTE[2] command section length						19
//	BYTE[?] commands (zero terminated)					21 + ??
//	BYTE[2] numTextLines
//	BYTE[2] text length (in unicode (2 byte) characters.)
//	BYTE[?] text (in unicode)

void CPSendGumpMenu::Log( std::ofstream &outStream, bool fullHeader )
{
	if( fullHeader )
		outStream << "[SEND]Packet     : CPSendGumpMenu 0xB0 --> Length: " << internalBuffer.size() << std::endl;
	outStream << "ID               : " << std::hex << UnpackULong(  &internalBuffer[0], 3  ) << std::endl;
	outStream << "GumpID           : " << std::hex << UnpackULong(  &internalBuffer[0], 7  ) << std::endl;
	outStream << "X                : " << std::hex << UnpackUShort( &internalBuffer[0], 11  ) << std::endl;
	outStream << "Y                : " << std::hex << UnpackUShort( &internalBuffer[0], 15 ) << std::endl;
	outStream << "Command Sec Len  : " << std::dec << UnpackUShort( &internalBuffer[0], 19 ) << std::endl;
//	outStream << "Num text lines   : " << UnpackUShort( &internalBuffer[0], 15 ) << std::endl;
//	outStream << "Text Sec Len     : " << internalBuffer[17] << std::endl;
	outStream << "Commands         : " << std::endl;
	for( size_t x = 0; x < commands.size(); ++x )
		outStream << "     " << commands[x] << std::endl;
	outStream << "Text             : " << std::endl;
	for( size_t y = 0; y < text.size(); ++y )
		outStream << "     " << text[y] << std::endl;

	outStream << "  Raw dump     :" << std::endl;
	cPUOXBuffer::Log( outStream, false );
}

//Subcommand 0x1b: New Spellbook
//
// BYTE[2]   unknown, always 1 
// BYTE[4]   Spellbook serial 
// BYTE[2]   Item Id       
// BYTE[2]   scroll offset           // 1==regular, 101=necro, 201=paladin 
// BYTE[8]   spellbook content // first bit of first byte = spell #1, second bit of first byte = spell #2, first bit of second byte = spell #8, etc

void CPNewSpellBook::InternalReset( void )
{
	internalBuffer.resize( 23 );
	internalBuffer[0] = 0xBF;
	PackShort( &internalBuffer[0], 1, 23 );
	PackShort( &internalBuffer[0], 3, 0x1B );
	PackShort( &internalBuffer[0], 5, 0x01 );
	internalBuffer[11] = 0x0E;
	internalBuffer[12] = 0xFA;
	PackShort( &internalBuffer[0], 13, 1 );
}
void CPNewSpellBook::CopyData( CItem& obj )
{
	PackLong( &internalBuffer[0], 7, obj.GetSerial() );
	for( UI08 i = 0 ; i < 64 ; ++i )
	{
		int y = (i % 8);
		int x = 15 + (int)(i / 8);
		if( Magic->HasSpell( &obj, i+1 ) )
			internalBuffer[x] |= static_cast<UI08>(power( 2, y ));
	}
}
CPNewSpellBook::CPNewSpellBook()
{
	InternalReset();
}
CPNewSpellBook::CPNewSpellBook( CItem& obj )
{
	InternalReset();
	CopyData( obj );
}

bool CPNewSpellBook::ClientCanReceive( CSocket *mSock )
{
	if( mSock == NULL )
		return false;
	switch( mSock->ClientType() )
	{
	case CV_NORMAL:
	case CV_T2A:
	case CV_UO3D:
		if( mSock->ClientVersionMajor() < 4 )
			return false;
		break;
	default:
		break;
	}
	return true;
}

//Subcommand  0x22: Damage
//
// BYTE[2]   unknown, always 1 
// BYTE[4]   Serial 
// BYTE        Damage // how much damage was done ? 
//
//Note: displays damage above the npc/player’s head.

void CPDisplayDamage::InternalReset( void )
{
	internalBuffer.resize( 11 );
	internalBuffer[0] = 0xBF;
	internalBuffer[1] = 0x00;
	internalBuffer[2] = 0x0B;
	internalBuffer[3] = 0x00;
	internalBuffer[4] = 0x22;
	internalBuffer[5] = 0x01;
}
void CPDisplayDamage::CopyData( CChar& ourTarg, UI16 ourDamage )
{
	PackLong( &internalBuffer[0], 6, ourTarg.GetSerial() );
	internalBuffer[10] = (ourDamage>>8) + (ourDamage%256);
}
CPDisplayDamage::CPDisplayDamage()
{
	InternalReset();
}
CPDisplayDamage::CPDisplayDamage( CChar& ourTarg, UI16 ourDamage )
{
	InternalReset();
	CopyData( ourTarg, ourDamage );
}
bool CPDisplayDamage::ClientCanReceive( CSocket *mSock )
{
	if( mSock == NULL )
		return false;
	switch( mSock->ClientType() )
	{
	case CV_NORMAL:
	case CV_T2A:
	case CV_UO3D:
		if( mSock->ClientVersionMajor() < 4 )
			return false;
		break;
	default:
		break;
	}
	return true;
}

//Subcommand  0x10:
//
// BYTE[4]   Serial 
// BYTE[4]   Unknown (List?) 
//
//Note: Queries client (Do you want us to send a ToolTip?)

void CPQueryToolTip::InternalReset( void )
{
	internalBuffer.resize( 13 );
	internalBuffer[0] = 0xBF;
	internalBuffer[1] = 0x00;
	internalBuffer[2] = 0x0D;
	internalBuffer[3] = 0x00;
	internalBuffer[4] = 0x10;
	internalBuffer[9] = 0x78;
	internalBuffer[10] = 0xA1;
	internalBuffer[11] = 0xBA;
	internalBuffer[12] = 0x2B;
}
void CPQueryToolTip::CopyData( CBaseObject& mObj )
{
	PackLong( &internalBuffer[0], 5, mObj.GetSerial() );
}
CPQueryToolTip::CPQueryToolTip()
{
	InternalReset();
}
CPQueryToolTip::CPQueryToolTip( CBaseObject& mObj )
{
	InternalReset();
	CopyData( mObj );
}

bool CPQueryToolTip::ClientCanReceive( CSocket *mSock )
{
	if( mSock == NULL )
		return false;
	switch( mSock->ClientType() )
	{
	case CV_NORMAL:
	case CV_T2A:
	case CV_UO3D:
		if( mSock->ClientVersionMajor() < 4 )
			return false;
		break;
	default:
		break;
	}
	return true;
}

//0xD6 Packet
//
//Last Modified on Wednesday, 12’th-Feb-2003  
//
//AOS tooltip/Object property list (Variable# Bytes)
//
//  BYTE cmd 
//  BYTE[2] length 
//  BYTE[2] unknown1, always 1 
//  BYTE[4] serial 
//  BYTE unknown2, always 0 
//  BYTE unknown3, always 0 
//  BYTE[4]  list ID (see notes) 
//  Loop
//    BYTE[4]  Localization#
//    if (Localization#==0) break loop
//    BYTE[2] text length
//    BYTE[text length]  little endian Unicode text, not 0 terminated 
void CPToolTip::InternalReset( void )
{
	internalBuffer.resize( 15 );
	internalBuffer[0] = 0xD6;
	internalBuffer[3] = 0x00;
	internalBuffer[4] = 0x01;
	internalBuffer[9] = 0x00;
	internalBuffer[10] = 0x00;
	internalBuffer[11] = 0x78;
	internalBuffer[12] = 0xA1;
	internalBuffer[13] = 0xBA;
	internalBuffer[14] = 0x2B;
}

void CPToolTip::FinalizeData( toolTipEntry tempEntry, size_t &totalStringLen )
{
	tempEntry.stringLen = (tempEntry.ourText.size()*2);
	totalStringLen += (tempEntry.stringLen+6);
	ourEntries.push_back( tempEntry );
}

void CPToolTip::CopyItemData( CItem& cItem, size_t &totalStringLen )
{
	toolTipEntry tempEntry;
	if( cItem.GetType() == IT_HOUSESIGN )
		tempEntry.ourText = " \tA House Sign\t ";
	else if( cItem.GetName()[0] == '#' )
	{
		std::string temp;
		getTileName( cItem, temp );
		if( cItem.GetAmount() > 1 )
			tempEntry.ourText = UString::sprintf( " \t%s : %i\t ", temp.c_str(), cItem.GetAmount() );
		else
			tempEntry.ourText = UString::sprintf( " \t%s\t ",temp.c_str() );
	}
	else
	{
		if( cItem.GetAmount() > 1 && !cItem.isCorpse() )
	    	tempEntry.ourText = UString::sprintf( " \t%s : %i\t ", cItem.GetName().c_str(), cItem.GetAmount() );
		else
			tempEntry.ourText = UString::sprintf( " \t%s\t ",cItem.GetName().c_str() );
	}
	tempEntry.stringNum = 1050045;
	FinalizeData( tempEntry, totalStringLen );

	if( cItem.GetType() == IT_CONTAINER || cItem.GetType() == IT_LOCKEDCONTAINER )
	{
		tempEntry.stringNum = 1050044;
		tempEntry.ourText = UString::sprintf( "%u\t%i",cItem.GetContainsList()->Num(), (cItem.GetWeight()/100) );
		FinalizeData( tempEntry, totalStringLen );
	}
	else if( cItem.GetType() == IT_HOUSESIGN )
	{
		tempEntry.stringNum = 1061112;
		tempEntry.ourText = cItem.GetName();
		FinalizeData( tempEntry, totalStringLen );

		if( cItem.GetOwnerObj() != NULL )
		{
			tempEntry.stringNum = 1061113;
			tempEntry.ourText = cItem.GetOwnerObj()->GetName();
			FinalizeData( tempEntry, totalStringLen );
		}
	}
	else if( cItem.GetType() == IT_MAGICWAND && cItem.GetTempVar( CITV_MOREZ ) )
	{
		tempEntry.stringNum = 1060584;
		tempEntry.ourText = UString::number( cItem.GetHP() );
		FinalizeData( tempEntry, totalStringLen );
	}

	if( cItem.GetLayer() > 0 && cItem.GetLayer() != IL_NONE )
	{
		if( cItem.GetHiDamage() > 0 )
		{	
			tempEntry.stringNum = 1060403;
			tempEntry.ourText = UString::number( cItem.GetHiDamage() );
			FinalizeData( tempEntry, totalStringLen );
		}

		if( cItem.GetDef() > 0 )
		{	
			tempEntry.stringNum = 1060448;
			tempEntry.ourText = UString::number( cItem.GetDef() );
			FinalizeData( tempEntry, totalStringLen );
		}

		if( cItem.GetMaxHP() > 1 )
		{	
			tempEntry.stringNum = 1060639;
			tempEntry.ourText = UString::sprintf( "%i\t%i", cItem.GetHP(), cItem.GetMaxHP() );
			FinalizeData( tempEntry, totalStringLen );
		}

		if( cItem.GetStrength2() > 0 )
		{
			tempEntry.stringNum = 1060485;
			tempEntry.ourText = UString::number( cItem.GetStrength2() );
			FinalizeData( tempEntry, totalStringLen );
		}
		if( cItem.GetDexterity2() > 0 )
		{
			tempEntry.stringNum = 1060409;
			tempEntry.ourText = UString::number( cItem.GetDexterity2() );
			FinalizeData( tempEntry, totalStringLen );
		}
		if( cItem.GetIntelligence2() > 0 )
		{
			tempEntry.stringNum = 1060432;
			tempEntry.ourText = UString::number( cItem.GetIntelligence2() );
			FinalizeData( tempEntry, totalStringLen );
		}

		if( cItem.GetStrength() > 1 )
		{	
			tempEntry.stringNum = 1061170;
			tempEntry.ourText = UString::number( cItem.GetStrength() );
			FinalizeData( tempEntry, totalStringLen );
		}
	}
}

void CPToolTip::CopyCharData( CChar& mChar, size_t &totalStringLen )
{
	toolTipEntry tempEntry;
	tempEntry.stringNum = 1050045;
	tempEntry.ourText = UString::sprintf( " \t%s\t ",mChar.GetName().c_str() );
	FinalizeData( tempEntry, totalStringLen );
}

void CPToolTip::CopyData( SERIAL objSer )
{
	size_t totalStringLen = 0; //total string length

	if( objSer < BASEITEMSERIAL )
	{
		CChar *mChar = calcCharObjFromSer( objSer );
		if( mChar != NULL )
			CopyCharData( (*mChar), totalStringLen );
	}
	else
	{
		CItem *cItem = calcItemObjFromSer( objSer );
		if( cItem != NULL )
			CopyItemData( (*cItem), totalStringLen );
	}

	size_t packetLen = 14 + totalStringLen + 5;
	internalBuffer.resize( packetLen );
	internalBuffer[1] = static_cast<UI08>(packetLen>>8);
	internalBuffer[2] = static_cast<UI08>(packetLen%256);
	PackLong( &internalBuffer[0], 5, objSer );

	size_t modifier = 14;
	//loop through all lines
	for( size_t i = 0; i < ourEntries.size(); ++i )
	{
		size_t stringLen = ourEntries[i].stringLen;
		internalBuffer[++modifier] = static_cast<UI08>(ourEntries[i].stringNum>>24);
		internalBuffer[++modifier] = static_cast<UI08>(ourEntries[i].stringNum>>16);
		internalBuffer[++modifier] = static_cast<UI08>(ourEntries[i].stringNum>>8);
		internalBuffer[++modifier] = static_cast<UI08>(ourEntries[i].stringNum%256);
		internalBuffer[++modifier] = static_cast<UI08>(stringLen>>8);
		internalBuffer[++modifier] = static_cast<UI08>(stringLen%256);

		//convert to uni character
		for( size_t j = 0; j < stringLen; j += 2 )
		{	
			internalBuffer[++modifier] = ourEntries[i].ourText[j/2];
			internalBuffer[++modifier] = 0x00;
		}
	}

	internalBuffer[packetLen-4] = 0x00;
	internalBuffer[packetLen-3] = 0x00;
	internalBuffer[packetLen-2] = 0x00;
	internalBuffer[packetLen-1] = 0x00;
}


CPToolTip::CPToolTip()
{
	InternalReset();
}
CPToolTip::CPToolTip( SERIAL objSer )
{
	InternalReset();
	CopyData( objSer );
}

//0x9E Packet
//
//Last Modified on Sunday, 15-May-1998
//
//Sell List (Variable # of bytes) 
//  BYTE cmd 
//  BYTE[2] blockSize
//  BYTE[4] shopkeeperID
//  BYTE[2] numItems
//For each item, a structure containing:
//     BYTE[4] itemID
//     BYTE[2] itemModel
//     BYTE[2] itemHue/Color
//     BYTE[2] itemAmount
//     BYTE[2] value
//     BYTE[2] nameLength
//     BYTE[?] name


void CPSellList::InternalReset( void )
{
	internalBuffer.resize( 9 );
	internalBuffer[0] = 0x9E;
	numItems = 0;
}
void CPSellList::CopyData( CChar& mChar, CChar& vendorID )
{
	CItem *sellPack = vendorID.GetItemAtLayer( IL_SELLCONTAINER );
	CItem *ourPack	= mChar.GetPackItem();

	numItems			= 0;
	size_t packetLen	= 9;

	if( ValidateObject( sellPack ) && ValidateObject( ourPack ) )
	{
		CTownRegion *tReg = NULL;
		if( cwmWorldState->ServerData()->TradeSystemStatus() )
			tReg = calcRegionFromXY( vendorID.GetX(), vendorID.GetY(), vendorID.WorldNumber() );
		CDataList< CItem * > *spCont = sellPack->GetContainsList();
		for( CItem *spItem = spCont->First(); !spCont->Finished(); spItem = spCont->Next() )
		{
			if( ValidateObject( spItem ) )
				AddContainer( tReg, spItem, ourPack, packetLen );
		}
	}

	PackShort( &internalBuffer[0], 1, (UI16)packetLen );
	PackLong( &internalBuffer[0], 3, vendorID.GetSerial() );
	PackShort( &internalBuffer[0], 7, numItems );
}

void CPSellList::AddContainer( CTownRegion *tReg, CItem *spItem, CItem *ourPack, size_t &packetLen )
{
	CDataList< CItem * > *opCont = ourPack->GetContainsList();
	for( CItem *opItem = opCont->First(); !opCont->Finished(); opItem = opCont->Next() )
	{
		if( ValidateObject( opItem ) )
		{
			if( opItem->GetType() == IT_CONTAINER )
				AddContainer( tReg, spItem, opItem, packetLen );
			else if( opItem->GetID() == spItem->GetID() && opItem->GetType() == spItem->GetType() &&
				( spItem->GetName() == opItem->GetName() || !cwmWorldState->ServerData()->SellByNameStatus() ) )
			{
				AddItem( tReg, spItem, opItem, packetLen );
				++numItems;
			}
			if( numItems >= 60 )
				return;
		}
	}
}

void CPSellList::AddItem( CTownRegion *tReg, CItem *spItem, CItem *opItem, size_t &packetLen )
{
	std::string itemname;
	size_t stringLen	= getTileName( (*opItem), itemname );
	size_t newLen		= (packetLen + 14 + stringLen);
	internalBuffer.resize( newLen );
	PackLong( &internalBuffer[0], packetLen, opItem->GetSerial() );
	PackShort( &internalBuffer[0], packetLen+4, opItem->GetID() );
	PackShort( &internalBuffer[0], packetLen+6, opItem->GetColour() );
	PackShort( &internalBuffer[0], packetLen+8, opItem->GetAmount() );
	UI32 value = calcValue( opItem, spItem->GetSellValue() );
	if( cwmWorldState->ServerData()->TradeSystemStatus() )
		value = calcGoodValue( tReg, spItem, value, true );
	PackShort( &internalBuffer[0], packetLen+10, value );
	PackShort( &internalBuffer[0], packetLen+12, stringLen );
	for( size_t i = 0; i < stringLen; ++i )
	{
		internalBuffer[packetLen+14+i] = itemname[i];
	}
	packetLen = newLen;
}

CPSellList::CPSellList()
{
	InternalReset();
}

bool CPSellList::CanSellItems( CChar &mChar, CChar &vendor )
{
	CopyData( mChar, vendor );
	return (numItems != 0);
}

//0x71 Packet
//
//Last Modified on Wednesday, 24-May-2000
//
//Bulletin Board Message (Variable # of bytes)
//
//  BYTE cmd
//  BYTE[2] len
//  BYTE subcmd
//  BYTE submessage
//		Submessage 0 – Display Bulletin Board
//		BYTE[4] BoardID
//		BYTE[22] board name (default is “bulletin board”, the rest nulls)
//		BYTE[4] ID (0x402000FF)
//		BYTE[4] zero (0)

void CPOpenMessageBoard::InternalReset( void )
{
	internalBuffer.resize( 37 );
	internalBuffer[0] = 0x71;
	PackShort( &internalBuffer[0], 1, 37 );
	internalBuffer[3] = 0;
	std::string boardName = "Bulletin Board";
	strncpy( (char *)&internalBuffer[8], boardName.c_str(), 15 );
	PackLong( &internalBuffer[0], 29, (SERIAL)0x4020000FF );
}

void CPOpenMessageBoard::CopyData( CSocket *mSock )
{
	CItem *msgBoard = calcItemObjFromSer( mSock->GetDWord( 1 ) );

	if( ValidateObject( msgBoard ) )
	{
		PackLong( &internalBuffer[0], 4, msgBoard->GetSerial() );
		// If the name the item (Bulletin Board) has been defined, display it
		// instead of the default "Bulletin Board" title.
		if( msgBoard->GetName() != "#" )
			strncpy( (char *)&internalBuffer[8], msgBoard->GetName().c_str(), 21 );
	}
}

CPOpenMessageBoard::CPOpenMessageBoard( CSocket *mSock )
{
	InternalReset();
	CopyData( mSock );
}

}
