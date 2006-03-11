#include "uox3.h"
#include "CPacketSend.h"
#include "speech.h"
#include "combat.h"
#include "cMagic.h"
#include "power.h"
#include "msgboard.h"
#include "mapstuff.h"

namespace UOX
{


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
		outStream << "[SEND]Packet     : CPCharLocBody 0x1B --> Length: " << pStream.GetSize() << TimeStamp() << std::endl;
	outStream << "PlayerID         : " << std::hex << "0x" << pStream.GetULong( 1 ) << std::endl;
	outStream << "Unknown1         : " << std::dec << pStream.GetULong( 5 ) << std::endl;
	outStream << "BodyType         : " << std::hex << "0x" << pStream.GetUShort( 9 ) << std::endl;
	outStream << "X Loc            : " << std::dec << pStream.GetUShort( 11 ) << std::endl;
	outStream << "Y Loc            : " << pStream.GetUShort( 13 ) << std::endl;
	outStream << "Z Loc            : " << pStream.GetUShort( 15 ) << std::endl;
	outStream << "Direction        : " << (UI16)pStream.GetByte( 17 ) << std::endl;
	outStream << "Unknown2         : " << pStream.GetUShort( 18 ) << std::endl;
	outStream << "Unknown3         : " << pStream.GetULong( 20 ) << std::endl;
	outStream << "Unknown4         : " << pStream.GetULong( 24 ) << std::endl;
	outStream << "Flag Byte        : " << (UI16)pStream.GetByte( 28 ) << std::endl;
	outStream << "Highlight Colour : " << (UI16)pStream.GetByte( 29 ) << std::endl;
	outStream << "  Raw dump     :" << std::endl;
	CPUOXBuffer::Log( outStream, false );
}

void CPCharLocBody::InternalReset( void )
{
	pStream.ReserveSize( 37 );
	pStream.WriteByte( 0, 0x1B );

	for( UI08 k = 5; k < 9; ++k )
		pStream.WriteByte( k, 0x00 );
	for( UI08 i = 18; i < 28; ++i )
		pStream.WriteByte( i, 0x00 );
	for( UI08 j = 30; j < 37; ++j )
		pStream.WriteByte( j, 0x00 );
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
	pStream.WriteLong(  1, toCopy.GetSerial() );
	pStream.WriteShort( 9, toCopy.GetID() );
	pStream.WriteShort( 11, toCopy.GetX() );
	pStream.WriteShort( 13, toCopy.GetY() );
	pStream.WriteByte(  16, toCopy.GetZ() );
	pStream.WriteByte(  17, toCopy.GetDir() );
}

CPCharLocBody &CPCharLocBody::operator=( CChar &toCopy )
{
	CopyData( toCopy );
	return (*this);
}

void CPCharLocBody::Flag( UI08 toPut )
{
	pStream.WriteByte( 28, toPut );
}
void CPCharLocBody::HighlightColour( UI08 color )
{
	pStream.WriteByte( 29, color );
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
	pStream.ReserveSize( 44 );
	pStream.WriteByte( 0, 0x1C );
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

CPacketSpeech::CPacketSpeech() : isUnicode( false )
{
	pStream.ReserveSize( 44 );
	pStream.WriteByte(   0, 0x1C );
	pStream.WriteShort(  1, 44 );
	pStream.WriteLong(   3, 0xFFFFFFFF );
	pStream.WriteShort(  7, 0xFFFF );
	pStream.WriteString( 14, "System", 6 );
	pStream.WriteByte(   20, 0 );
}

void CPacketSpeech::SpeakerName( const std::string toPut )
{
	size_t len = toPut.length();
	if( len >= 30 )
	{
		pStream.WriteString( 14, toPut, 29 );
		pStream.WriteByte(   43, 0 );
	}
	else
		pStream.WriteString( 14, toPut, toPut.length() );
}
void CPacketSpeech::SpeakerSerial( SERIAL toPut )
{
	pStream.WriteLong( 3, toPut );
}
void CPacketSpeech::SpeakerModel( UI16 toPut )
{
	pStream.WriteShort( 7, toPut );
}
void CPacketSpeech::Colour( COLOUR toPut )
{
	pStream.WriteShort( 10, toPut );
}
void CPacketSpeech::Font( FontType toPut )
{
	pStream.WriteShort( 12, toPut );
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
	pStream.WriteByte( 9, static_cast<UI08>(toPut) );
}
void CPacketSpeech::Speech( const std::string toPut )
{
	size_t len			= toPut.length();
	const size_t newLen	= 44 + len + 1;

	pStream.ReserveSize( newLen );
	pStream.WriteString( 44, toPut, toPut.length() );
	pStream.WriteShort( 1, static_cast<UI16>(newLen) );
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
	pStream.ReserveSize( 8 );
	pStream.WriteByte( 0, 0x21 );
}
void CPWalkDeny::SequenceNumber( char newValue )
{
	pStream.WriteByte( 1, newValue );
}
void CPWalkDeny::X( SI16 newValue )
{
	pStream.WriteShort( 2, newValue );
}
void CPWalkDeny::Y( SI16 newValue )
{
	pStream.WriteShort( 4, newValue );
}
void CPWalkDeny::Z( SI08 newValue )
{
	pStream.WriteByte( 7, newValue );
}
void CPWalkDeny::Direction( char newValue )
{
	pStream.WriteByte( 6, newValue );
}

//0x22 Packet
//Last Modified on Wednesday, 11-Nov-1998 
//Character Move ACK/ Resync Request(3 bytes) 
//	BYTE cmd 
//	BYTE sequence (matches sent sequence) 
//	BYTE (0x00) 

CPWalkOK::CPWalkOK()
{
	pStream.ReserveSize( 3 );
	pStream.WriteByte( 0, 0x22 );
}
void CPWalkOK::SequenceNumber( char newValue )
{
	pStream.WriteByte( 1, newValue );
}
void CPWalkOK::OtherByte( char newValue )
{
	pStream.WriteByte( 2, newValue );
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
	pStream.ReserveSize( 17 );
	pStream.WriteByte( 0, 0x77 );
}

CPExtMove::CPExtMove( CChar &toCopy )
{
	pStream.ReserveSize( 17 );
	pStream.WriteByte( 0, 0x77 );
	CopyData( toCopy );
}

CPExtMove &CPExtMove::operator=( CChar &toCopy )
{
	CopyData( toCopy );
	return (*this);
}

void CPExtMove::FlagColour( UI08 newValue )
{
	pStream.WriteByte( 16, newValue );
}

void CPExtMove::CopyData( CChar &toCopy )
{
	const UI08 BIT_ATWAR	= 0x40;
	const UI08 BIT_DEAD		= 0x80;
	const UI08 BIT_POISONED	= 0x04;
	const UI08 BIT_FEMALE	= 0x02;
	const UI08 BIT_GOLDEN	= 0x08;

	pStream.WriteLong(  1, toCopy.GetSerial() );
	pStream.WriteShort( 5, toCopy.GetID() );
	pStream.WriteShort( 7, toCopy.GetX() );
	pStream.WriteShort( 9, toCopy.GetY() );
	pStream.WriteByte( 11, toCopy.GetZ() );

	UI08 dir = toCopy.GetDir();
	if( toCopy.IsNpc() && toCopy.CanRun() && ( toCopy.IsAtWar() || toCopy.GetRunning() ) )
		dir |= 0x80;
	else if( !toCopy.IsNpc() && toCopy.GetRunning() )
		dir |= 0x80;
	pStream.WriteByte( 12, dir );
	pStream.WriteShort( 13, toCopy.GetSkin() );

	UI08 flag = 0;
	MFLAGSET( flag, toCopy.IsAtWar(), BIT_ATWAR );
	MFLAGSET( flag, ( toCopy.IsDead() || toCopy.GetVisible() != VT_VISIBLE ), BIT_DEAD );
	MFLAGSET( flag, (toCopy.GetPoisoned() != 0), BIT_POISONED );
#pragma note( "we need to update this here to determine what goes on with elves too!" )
	MFLAGSET( flag, (toCopy.GetID() == 0x0191 || toCopy.GetID() == 0x025E), BIT_FEMALE );
//	MFLAGSET( flag, (toCopy.GetHP() == toCopy.GetMaxHP()), BIT_GOLDEN );
	pStream.WriteByte( 15, flag );
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
	pStream.ReserveSize( 5 );
	pStream.WriteByte( 0, 0xAA );
}
CPAttackOK::CPAttackOK( CChar &toCopy )
{
	pStream.ReserveSize( 5 );
	pStream.WriteByte( 0, 0xAA );
	CopyData( toCopy );
}
void CPAttackOK::Serial( SERIAL newSerial )
{
	pStream.WriteLong( 1, newSerial );
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
	pStream.ReserveSize( 5 );
	pStream.WriteByte( 0, 0x1D );
}
CPRemoveItem::CPRemoveItem( CBaseObject &toCopy )
{
	pStream.ReserveSize( 5 );
	pStream.WriteByte( 0, 0x1D );
	CopyData( toCopy );
}
void CPRemoveItem::Serial( SERIAL newSerial )
{
	pStream.WriteLong( 1, newSerial );
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
	pStream.ReserveSize( 3 );
	pStream.WriteByte( 0, 0xBC );
}
CPWorldChange::CPWorldChange( WorldType newSeason, UI08 newCursor )
{
	pStream.ReserveSize( 3 );
	pStream.WriteByte( 0, 0xBC );
	Season( newSeason );
	Cursor( newCursor );
}
void CPWorldChange::Season( WorldType newSeason )
{
	pStream.WriteByte( 1, (UI08)newSeason );
}
void CPWorldChange::Cursor( UI08 newCursor )
{
	pStream.WriteByte( 2, newCursor );
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
	pStream.ReserveSize( 2 );
	pStream.WriteByte( 0, 0x4F );
}
CPLightLevel::CPLightLevel( LIGHTLEVEL level )
{
	pStream.ReserveSize( 2 );
	pStream.WriteByte( 0, 0x4F );
	Level( level );
}

void CPLightLevel::Level( LIGHTLEVEL level )
{
	pStream.WriteByte( 1, level );
}

void CPUpdIndSkill::InternalReset( void )
{
	pStream.ReserveSize( 11 );
	pStream.WriteByte(  0, 0x3A );
	pStream.WriteShort( 1, 0x000B ); // Length of message
	pStream.WriteByte(  3, 0xFF); // single entry
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
	pStream.WriteByte( 5, sNum );
}
void CPUpdIndSkill::Skill( SI16 skillval )
{
	pStream.WriteShort( 6, skillval );
}
void CPUpdIndSkill::BaseSkill( SI16 skillval )
{
	pStream.WriteShort( 8, skillval );
}
void CPUpdIndSkill::Lock( UI08 lockVal )
{
	pStream.WriteByte( 10, lockVal );
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
	pStream.WriteLong( 3, toSet );
}

void CPBuyItem::InternalReset( void )
{
	pStream.ReserveSize( 8 );
	pStream.WriteByte(  0, 0x3B );
	pStream.WriteShort( 1, 0x0008 );
	pStream.WriteByte(  7, 0x00 );
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
	pStream.WriteLong( 1, newIP );
}
void CPRelay::Port( UI16 newPort )
{
	pStream.WriteShort( 5, newPort );
}
void CPRelay::SeedIP( long newIP )
{
	pStream.WriteLong( 7, newIP );
}
void CPRelay::InternalReset( void )
{
	pStream.ReserveSize( 11 );
	pStream.WriteByte( 0, 0x8C );
	SeedIP( 0xFFFFFFFF );
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
	pStream.ReserveSize( 15 );
	pStream.WriteByte( 0, 0x2E );
	pStream.WriteByte( 7, 0x00 );
}
void CPWornItem::ItemSerial( SERIAL itemSer )
{
	pStream.WriteLong( 1, itemSer );
}
void CPWornItem::Model( SI16 newModel )
{
	pStream.WriteShort( 5, newModel );
}
void CPWornItem::Layer( UI08 layer )
{
	pStream.WriteByte( 8, layer );
}
void CPWornItem::CharSerial( SERIAL chSer )
{
	pStream.WriteLong( 9, chSer );
}
void CPWornItem::Colour( SI16 newColour )
{
	pStream.WriteShort( 13, newColour );
}

CPWornItem::CPWornItem( CItem &toCopy )
{
	pStream.ReserveSize( 15 );
	pStream.WriteByte( 0, 0x2E );
	pStream.WriteByte( 7, 0x00 );
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
	Direction( toCopy.GetDir() );
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
	pStream.WriteLong( 1, toSet );
}
void CPCharacterAnimation::Action( UI16 model )
{
	pStream.WriteShort( 5, model );
}
void CPCharacterAnimation::Direction( UI08 dir )
{
	pStream.WriteByte( 8, dir );
}
void CPCharacterAnimation::Repeat( SI16 repeatValue )
{
	pStream.WriteShort( 9, repeatValue );
}
void CPCharacterAnimation::DoBackwards( bool newValue )
{
	pStream.WriteByte( 11, (UI08)((newValue?1:0)) );
}
void CPCharacterAnimation::RepeatFlag( bool newValue )
{
	pStream.WriteByte( 12, (UI08)((newValue?1:0)) );
}
void CPCharacterAnimation::FrameDelay( UI08 delay )
{
	pStream.WriteByte( 13, delay );
}
CPCharacterAnimation &CPCharacterAnimation::operator=( CChar &toCopy )
{
	CopyData( toCopy );
	return (*this);
}
void CPCharacterAnimation::InternalReset( void )
{
	pStream.ReserveSize( 14 );
	pStream.WriteByte( 0, 0x6E );
	pStream.WriteByte( 7, 0x00 );
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
	pStream.WriteLong( 1, toCopy.GetSerial() );
	pStream.WriteShort( 5, toCopy.GetID() );
	pStream.WriteShort( 8, toCopy.GetColour() );
	pStream.WriteShort( 11, toCopy.GetX() );
	pStream.WriteShort( 13, toCopy.GetY() );
	pStream.WriteByte(  17, toCopy.GetDir() );
	pStream.WriteByte(  18, toCopy.GetZ() );
	UI08 flag				= 0;
	const UI08 BIT_INVUL	= 0x01;
	const UI08 BIT_DEAD		= 0x02;
	const UI08 BIT_POISON	= 0x04;
	const UI08 BIT_ATWAR	= 0x40;
	const UI08 BIT_INVIS	= 0x80;

	MFLAGSET( flag, toCopy.IsInvulnerable(), BIT_INVUL );
	MFLAGSET( flag, toCopy.IsDead(), BIT_DEAD );
	MFLAGSET( flag, toCopy.GetPoisoned(), BIT_POISON );
	MFLAGSET( flag, toCopy.IsAtWar(), BIT_ATWAR );
	MFLAGSET( flag, (toCopy.GetVisible() != VT_VISIBLE), BIT_INVIS );
	pStream.WriteByte( 10, flag );
}
void CPDrawGamePlayer::InternalReset( void )
{
	pStream.ReserveSize( 19 );
	pStream.WriteByte( 0, 0x20 );
	pStream.WriteByte( 7, 0x00 );
	pStream.WriteShort( 15, 0x0000 );
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
	pStream.ReserveSize( 6 );
	pStream.WriteByte( 0, 0x4E );
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
	pStream.WriteLong( 1, toSet );
}
void CPPersonalLightLevel::Level( LIGHTLEVEL lightLevel )
{
	pStream.WriteByte( 5, lightLevel );
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
	pStream.WriteByte( 1, mode );
}
void CPPlaySoundEffect::Model( UI16 newModel )
{
	pStream.WriteShort( 2, newModel );
}
void CPPlaySoundEffect::X( SI16 xLoc )
{
	pStream.WriteShort( 6, xLoc );
}
void CPPlaySoundEffect::Y( SI16 yLoc )
{
	pStream.WriteShort( 8, yLoc );
}
void CPPlaySoundEffect::Z( SI16 zLoc )
{
	pStream.WriteShort( 10, zLoc );
}
void CPPlaySoundEffect::InternalReset( void )
{
	pStream.ReserveSize( 12 );
	pStream.WriteByte( 0, 0x54 );
	pStream.WriteByte(  1, 1 );
	pStream.WriteShort( 4, 0x00 );
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
	pStream.ReserveSize( 66 );
	pStream.WriteByte( 0, 0x88 );
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
	pStream.WriteLong( 1, tSerial );
}
void CPPaperdoll::FlagByte( UI08 fVal )
{
	pStream.WriteByte( 65, fVal );
}
void CPPaperdoll::Text( const std::string toPut )
{
	if( toPut.length() > 60 )
	{
		pStream.WriteString( 5, toPut, 59 );
		pStream.WriteByte(  64, 0x00 );
	}
	else
		pStream.WriteString( 5, toPut, toPut.size() );
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
	pStream.ReserveSize( 4 );
	pStream.WriteByte( 0, 0x65 );
	pStream.WriteByte( 1, 0xFF );
	pStream.WriteByte( 2, 0x40 );
	pStream.WriteByte( 3, 0x10 );
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
	pStream.WriteByte( 1, nType );
}
void CPWeather::Particles( UI08 nParts )
{
	pStream.WriteByte( 2, nParts );
}
void CPWeather::Temperature( UI08 nTemp )
{
	pStream.WriteByte( 3, nTemp );
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
	pStream.ReserveSize( 28 );
	pStream.WriteByte(  0, 0x70 );
	pStream.WriteShort( 24, 0x0000 );
}
CPGraphicalEffect::CPGraphicalEffect( UI08 effectType )
{
	InternalReset();
	Effect( effectType );
}
void CPGraphicalEffect::Effect( UI08 effectType )
{
	pStream.WriteByte( 1, effectType );
}
void CPGraphicalEffect::SourceSerial( CBaseObject &toSet )
{
	SourceSerial( toSet.GetSerial() );
}
void CPGraphicalEffect::SourceSerial( SERIAL toSet )
{
	pStream.WriteLong( 2, toSet );
}
void CPGraphicalEffect::TargetSerial( CBaseObject &toSet )
{
	TargetSerial( toSet.GetSerial() );
}
void CPGraphicalEffect::TargetSerial( SERIAL toSet )
{
	pStream.WriteLong( 6, toSet );
}
void CPGraphicalEffect::Model( SI16 nModel )
{
	pStream.WriteShort( 10, nModel );
}
void CPGraphicalEffect::X( SI16 nX )
{
	pStream.WriteShort( 12, nX );
}
void CPGraphicalEffect::Y( SI16 nY )
{
	pStream.WriteShort( 14, nY );
}
void CPGraphicalEffect::Z( SI08 nZ )
{
	pStream.WriteByte( 16, nZ );
}
void CPGraphicalEffect::XTrg( SI16 nX )
{
	pStream.WriteShort( 17, nX );
}
void CPGraphicalEffect::YTrg( SI16 nY )
{
	pStream.WriteShort( 19, nY );
}
void CPGraphicalEffect::ZTrg( SI08 nZ )
{
	pStream.WriteByte( 21, nZ );
}
void CPGraphicalEffect::Speed( UI08 nSpeed )
{
	pStream.WriteByte( 22, nSpeed );
}
void CPGraphicalEffect::Duration( UI08 nDuration )
{
	pStream.WriteByte( 23, nDuration );
}
void CPGraphicalEffect::AdjustDir( bool nValue )
{
	pStream.WriteByte( 26, (UI08)((nValue?0:1)) );
}
void CPGraphicalEffect::ExplodeOnImpact( bool nValue )
{
	pStream.WriteByte( 27, (UI08)((nValue?1:0)) );
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
	pStream.ReserveSize( 9 );
	pStream.WriteByte( 0, 0xA1 );
}
CPUpdateStat::CPUpdateStat( CChar &toUpdate, UI08 statNum )
{
	InternalReset();
	Serial( toUpdate.GetSerial() );
	switch( statNum )
	{
		case 0:	MaxVal( toUpdate.GetMaxHP() );
				CurVal( toUpdate.GetHP() );
				break;
		case 2:	MaxVal( toUpdate.GetMaxStam() );
				CurVal( toUpdate.GetStamina() );
				break;
		case 1:	MaxVal( toUpdate.GetMaxMana() );
				CurVal( toUpdate.GetMana() );
				break;
	}
	pStream.WriteByte( 0, (pStream.GetByte( 0 ) + statNum) );
}
void CPUpdateStat::Serial( SERIAL toSet )
{
	pStream.WriteLong( 1, toSet );
}
void CPUpdateStat::MaxVal( SI16 maxVal )
{
	pStream.WriteShort( 5, maxVal );
}
void CPUpdateStat::CurVal( SI16 curVal )
{
	pStream.WriteShort( 7, curVal );
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
	pStream.ReserveSize( 13 );
	pStream.WriteByte( 0, 0xAF );
	pStream.WriteByte( 9, 0x00 );
	pStream.WriteByte( 10, 0x00 );
	pStream.WriteByte( 11, 0x00 );
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
	pStream.WriteLong( 1, toSet );
}
void CPDeathAction::Corpse( SERIAL toSet )
{
	pStream.WriteLong( 5, toSet );
}
void CPDeathAction::FallDirection( UI08 toFall )
{
	pStream.WriteByte( 12, toFall );
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
	pStream.ReserveSize( 3 );
	pStream.WriteByte( 0, 0x6D );
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
	pStream.WriteShort( 1, musicID );
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
	pStream.ReserveSize( 7 );
	pStream.WriteByte( 0, 0x24 );
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
	pStream.WriteShort( 5, newModel );
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
	pStream.WriteLong( 1, toSet );
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
		outStream << "[SEND]Packet     : CPOpenGump 0x7C --> Length: " << pStream.GetSize() << TimeStamp() << std::endl;
	outStream << "DialogID         : " << std::hex << pStream.GetULong( 3 ) << std::endl;
	outStream << "MenuID           : " << pStream.GetUShort( 7 ) << std::endl;
	outStream << "Question Length  : " << std::dec << (SI16)pStream.GetByte( 9 ) << std::endl;
	outStream << "Question         : ";

	for( UI32 i = 0; i < pStream.GetByte( 9 ); ++i )
		outStream << pStream.GetByte( 10+i );
	outStream << std::endl;

	outStream << "# Responses      : " << pStream.GetByte( responseBaseOffset ) << std::endl;
	size_t offsetCount = responseBaseOffset + 1;
	for( UI32 j = 0; j < pStream.GetByte( responseBaseOffset ); ++j )
	{
		outStream << "    Response " << j << std::endl;
		outStream << "      Model  : " << std::hex << pStream.GetUShort( offsetCount + 0 ) << std::endl;
		outStream << "      Colour : " << pStream.GetUShort( offsetCount + 2 ) << std::endl;
		outStream << "      Length : " << std::dec << pStream.GetByte( offsetCount + 4 ) << std::endl;
		outStream << "      Text   : ";

		for( UI32 k = 0; k < pStream.GetByte( offsetCount + 4 ); ++k )
			outStream << pStream.GetByte( offsetCount + 5 + k );
		outStream << std::endl;
		offsetCount += ( 5 + pStream.GetByte( offsetCount + 4 ) );
	}
	outStream << "  Raw dump     :" << std::endl;
	CPUOXBuffer::Log( outStream, false );
}
void CPOpenGump::Question( std::string toAdd )
{
	pStream.ReserveSize( 10 + toAdd.length() + 2 );	// 10 for start of string, length of string + NULL, plus spot for # responses
	pStream.WriteString( 10, toAdd, toAdd.length() );
#if defined( UOX_DEBUG_MODE )
	if( toAdd.length() >= 255 )
		Console.Error( 1, "CPOpenGump::Question toAdd.length() is too long (%i)", toAdd.length() );
#endif
	pStream.WriteByte( 9, static_cast< UI08 >(toAdd.length() + 1) );
	responseBaseOffset	= (pStream.GetSize() - 1);
	responseOffset		= responseBaseOffset + 1;
}
void CPOpenGump::AddResponse( UI16 modelNum, UI16 colour, std::string responseText )
{
	pStream.WriteByte( responseBaseOffset, pStream.GetByte( responseBaseOffset ) + 1 ); // increment number of responses
#if defined( UOX_DEBUG_MODE )
	if( responseText.length() >= 255 )
		Console.Error( 1, "CPOpenGump::AddResponse responseText is too long (%i)", responseText.length() );
#endif
	UI16 toAdd = static_cast< UI16 >(5 + responseText.length());
	pStream.ReserveSize( pStream.GetSize() + toAdd );
	pStream.WriteShort(  (responseOffset + 0), modelNum );
	pStream.WriteShort(  (responseOffset + 2), colour   );
	pStream.WriteByte(   (responseOffset + 4), static_cast< UI08 >(responseText.length()) );
	pStream.WriteString( (responseOffset + 5), responseText, responseText.length() );
	responseOffset += toAdd;
}
void CPOpenGump::Finalize( void )
{
	pStream.WriteShort( 1, static_cast< UI16 >(pStream.GetSize()) );
}
void CPOpenGump::InternalReset( void )
{
	responseOffset		= 0xFFFFFFFF;
	responseBaseOffset	= 0xFFFFFFFF;
	pStream.ReserveSize( 9 );
	pStream.WriteByte(  0, 0x7C );
	pStream.WriteShort( 1, 0x0000 );
	pStream.WriteByte(  3, 0x01 );
	pStream.WriteByte(  4, 0x02 );
	pStream.WriteByte(  5, 0x03 );
	pStream.WriteByte(  6, 0x04 );
	pStream.WriteShort( 7, 0x0064 );
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
	pStream.WriteShort( 1, totalLines );
}
void CPOpenGump::GumpIndex( int index )
{
	pStream.WriteShort( 7, index );
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
	pStream.WriteLong( 3, toSet );
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
	pStream.ReserveSize( 19 );
	pStream.WriteByte( 0, 0x6C );
	CursorType( 0 );
}
void CPTargetCursor::Type( UI08 nType )
{
	pStream.WriteByte( 1, nType );
}
void CPTargetCursor::ID( SERIAL toSet )
{
	pStream.WriteLong( 2, toSet );
}
void CPTargetCursor::CursorType( UI08 nType )
{
	pStream.WriteByte( 6, nType );
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
			pStream.ReserveSize( 88 );
			pStream.WriteByte( 2, 88 );
			Flag( 4 );
		}
		else if( target.ClientVersionMajor() >= 3 )
		{
			extended3 = true;
			pStream.ReserveSize( 70 );
			pStream.WriteByte( 2, 70 );
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
		MaxHP( toCopy.GetMaxHP() );
	}
	NameChange( false );
#pragma note( "We need to check what the values are, OSI wise, for this value.  Are elfs 2/3?" )
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
		MaxStamina( toCopy.GetMaxStam() );
		Mana( toCopy.GetMana() );
		MaxMana( toCopy.GetMaxMana() );
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
		DamageMin( Combat->calcLowDamage( &toCopy ) );
		DamageMax( Combat->calcHighDamage( &toCopy ) );
	}
}
void CPStatWindow::InternalReset( void )
{
	pStream.ReserveSize( 66 );
	pStream.WriteByte( 0, 0x11 );
	pStream.WriteByte( 1, 0x00 );
	pStream.WriteByte( 2, 66 );
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
	pStream.WriteLong( 3, toSet );
}
void CPStatWindow::Name( const std::string nName )
{
	if( nName.length() >= 30 )
	{
		pStream.WriteString( 7, nName, 29 );
		pStream.WriteByte(   36, 0x00 );
	}
	else
		pStream.WriteString( 7, nName, nName.size() );
}
void CPStatWindow::CurrentHP( SI16 nValue )
{
	pStream.WriteShort( 37, nValue );
}
void CPStatWindow::MaxHP( SI16 nValue )
{
	pStream.WriteShort( 39, nValue );
}
void CPStatWindow::NameChange( bool nValue )
{
	pStream.WriteByte( 41, (UI08)((nValue?0xFF:0)) );
}
void CPStatWindow::Flag( UI08 nValue )
{
	pStream.WriteByte( 42, nValue );
}
void CPStatWindow::Sex( UI08 nValue )
{
	pStream.WriteByte( 43, nValue );
}
void CPStatWindow::Strength( SI16 nValue )
{
	pStream.WriteShort( 44, nValue );
}
void CPStatWindow::Dexterity( SI16 nValue )
{
	pStream.WriteShort( 46, nValue );
}
void CPStatWindow::Intelligence( SI16 nValue )
{
	pStream.WriteShort( 48, nValue );
}
void CPStatWindow::Stamina( SI16 nValue )
{
	pStream.WriteShort( 50, nValue );
}
void CPStatWindow::MaxStamina( SI16 nValue )
{
	pStream.WriteShort( 52, nValue );
}
void CPStatWindow::Mana( SI16 nValue )
{
	pStream.WriteShort( 54, nValue );
}
void CPStatWindow::MaxMana( SI16 nValue )
{
	pStream.WriteShort( 56, nValue );
}
void CPStatWindow::Gold( UI32 gValue )
{
	pStream.WriteLong( 58, gValue );
}
void CPStatWindow::AC( UI16 nValue )
{
	pStream.WriteShort( 62, nValue );
}
void CPStatWindow::Weight( UI16 nValue )
{
	pStream.WriteShort( 64, nValue );
}

void CPStatWindow::StatCap( UI16 value )
{
	if( extended3 )
		pStream.WriteShort( 66, value );
}
void CPStatWindow::CurrentPets( UI08 value )
{
	if( extended3 )
		pStream.WriteByte( 68, value );
}
void CPStatWindow::MaxPets( UI08 value )
{
	if( extended3 )
		pStream.WriteByte( 69, value );
}
void CPStatWindow::FireResist( UI16 value )
{
	if( extended4 )
		pStream.WriteShort( 70, value );
}
void CPStatWindow::ColdResist( UI16 value )
{
	if( extended4 )
		pStream.WriteShort( 72, value );
}
void CPStatWindow::PoisonResist( UI16 value )
{
	if( extended4 )
		pStream.WriteShort( 74, value );
}
void CPStatWindow::EnergyResist( UI16 value )
{
	if( extended4 )
		pStream.WriteShort( 76, value );
}
void CPStatWindow::Luck( UI16 value )
{
	if( extended4 )
		pStream.WriteShort( 78, value );
}
void CPStatWindow::DamageMin( UI16 value )
{
	if( extended4 )
		pStream.WriteShort( 80, value );
}
void CPStatWindow::DamageMax( UI16 value )
{
	if( extended4 )
		pStream.WriteShort( 82, value );
}
void CPStatWindow::Unknown( UI32 value )
{
	if( extended4 )
		pStream.WriteLong( 84, value );
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
	pStream.ReserveSize( 2 );
	pStream.WriteByte( 0, 0x53 );
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
	pStream.WriteByte( 1, errorNum );
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
	pStream.WriteByte( 1, hour );
}
void CPTime::Minute( UI08 minute )
{
	pStream.WriteByte( 2, minute );
}
void CPTime::Second( UI08 second )
{
	pStream.WriteByte( 3, second );
}
void CPTime::InternalReset( void )
{
	pStream.ReserveSize( 4 );
	pStream.WriteByte( 0, 0x5B );
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
	pStream.ReserveSize( 1 );
	pStream.WriteByte( 0, 0x55 );
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
	pStream.ReserveSize( newValue );
	pStream.WriteByte(  0, 0x69 );
	pStream.WriteShort( 1, newValue );
}
void CPTextEmoteColour::Unknown( SI16 newValue )
{
	pStream.WriteShort( 3, newValue );
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
	pStream.WriteByte( 1, nFlag );
}
void CPWarMode::InternalReset( void )
{
	pStream.ReserveSize( 5 );
	pStream.WriteByte( 0, 0x72 );
	pStream.WriteByte( 2, 0x00 );
	pStream.WriteByte( 3, 0x32 );
	pStream.WriteByte( 4, 0x00 );
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
	pStream.ReserveSize( 2 );
	pStream.WriteByte( 0, 0x33 );
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
	pStream.WriteByte( 1, mode );
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
	pStream.ReserveSize( 4 );
	pStream.WriteByte( 0, 0xA5 );
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
	pStream.WriteString( 3, txt, txt.size() );
}

void CPWebLaunch::SetSize( SI16 newSize )
{
	pStream.ReserveSize( newSize );
	pStream.WriteShort( 1, newSize );
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
	pStream.ReserveSize( 6 );
	pStream.WriteByte( 0, 0xBA );
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
	pStream.WriteShort( 2, x );
	pStream.WriteShort( 4, y );
}
CPTrackingArrow &CPTrackingArrow::operator=( CBaseObject &toCopy )
{
	Location( toCopy.GetX(), toCopy.GetY() );
	return (*this);
}
void CPTrackingArrow::Active( UI08 value )
{
	pStream.WriteByte( 1, value );
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
	pStream.ReserveSize( 2 );
	pStream.WriteByte( 0, 0x27 );
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
	pStream.WriteByte( 1, mode );
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
	pStream.ReserveSize( 9 );
	pStream.WriteByte( 0, 0x95 );
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
	pStream.WriteLong( 1, toSet );
}
void CPDyeVat::Model( SI16 toSet )
{
	pStream.WriteShort( 7, toSet );
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
	pStream.ReserveSize( 26 );
	pStream.WriteByte( 0, 0x99 );
	for( UI08 i = 6; i < 18; ++i )
		pStream.WriteByte( i, 0 );
	for( UI08 k = 20; k < 26; ++k )
		pStream.WriteByte( k, 0 );
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
	pStream.WriteByte( 1, rType );
}
void CPMultiPlacementView::DeedSerial( SERIAL toSet )
{
	pStream.WriteLong( 2, toSet );
}
void CPMultiPlacementView::MultiModel( SI16 toSet )
{
	pStream.WriteShort( 18, toSet );
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
//	0x01	T2A upgrade, enables chatbutton
//	0x02	Enables LBR update.  (of course LBR installation is required)
//			(plays MP3 instead of midis, 2D LBR client shows new LBR monsters, ... )
//	0x04	Unknown, never seen it set	(single char?)
//	0x08	Unknown, set on OSI servers that have AOS code - no matter of account status (doesn’t seem to “unlock/lock” anything on client side)
//	0x10	Enables AOS update (necro/paladin skills for all clients, malas map/AOS monsters if AOS installation present)
//	0x20	Sixth Character Slot
//	0x40	Samurai Empire?
//	0x80	Elves?
//	0x100
//	0x200
//	0x400
//	0x800
//	0x1000
//	0x2000
//	0x4000	
//	0x8000	Since client 4.0 this bit has to be set, otherwise bits 3..14 are ignored.
// Thus	0		neither T2A NOR LBR, equal to not sending it at all, 
//		1		is T2A, chatbutton, 
//		2		is LBR without chatbutton, 
//		3		is LBR with chatbutton…
//		8013	LBR + chatbutton + AOS enabled
// Note1: this message is send immediately after login.
// Note2: on OSI  servers this controls features OSI enables/disables via “upgrade codes.”
// Note3: a 3 doesn’t seem to “hurt” older (NON LBR) clients.

	pStream.ReserveSize( 3 );
	pStream.WriteByte( 0, 0xB9 );
#if defined( _MSC_VER )
#pragma todo( "Currently all client support is hardcoded. Move this into the ini when possible." )
#endif
	pStream.WriteShort( 1, 0x80FB );	// all features + sixth char
}

void CPEnableClientFeatures::Log( std::ofstream &outStream, bool fullHeader )
{
	if( fullHeader )
		outStream << "[SEND]Packet     : CPEnableClientFeatures 0xB9 --> Length: 3" << TimeStamp() << std::endl;

	UI16 lastByte = pStream.GetUShort( 1 );
	outStream << "Flags          : " << std::hex << (UI32)lastByte << std::dec << std::endl;
	if( (lastByte&0x01) == 0x01 )
		outStream << "               : T2A features (including chat)" << std::endl;
	if( (lastByte&0x02) == 0x02 )
		outStream << "               : Enabled LBR support" << std::endl;
	if( (lastByte&0x04) == 0x04 )
		outStream << "               : Unknown 1 (single char?)" << std::endl;
	if( (lastByte&0x08) == 0x08 )
		outStream << "               : Unknown 2 (doesn't matter?)" << std::endl;
	if( (lastByte&0x10) == 0x10 )
		outStream << "               : Enabled AOS support" << std::endl;
	if( (lastByte&0x20) == 0x20 )
		outStream << "               : Enabled Sixth Character Slot?" << std::endl;
	if( (lastByte&0x40) == 0x40 )
		outStream << "               : Enable Samurai Empire?" << std::endl;
	if( (lastByte&0x80) == 0x80 )
		outStream << "               : Enable Elves and ML?" << std::endl;

	outStream << "  Raw dump     :" << std::endl;
	CPUOXBuffer::Log( outStream, false );
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
	pStream.ReserveSize( 20 );
	pStream.WriteByte( 0, 0x25 );
	pStream.WriteByte( 7, 0 );
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
	pStream.WriteLong( 1, toSet );
}
void CPAddItemToCont::Model( SI16 toSet )
{
	pStream.WriteShort( 5, toSet );
}
void CPAddItemToCont::NumItems( SI16 toSet )
{
	pStream.WriteShort( 8, toSet );
}
void CPAddItemToCont::X( SI16 x )
{
	pStream.WriteShort( 10, x );
}
void CPAddItemToCont::Y( SI16 y )
{
	pStream.WriteShort( 12, y );
}
void CPAddItemToCont::Container( SERIAL toAdd )
{
	pStream.WriteLong( 14, toAdd );
}
void CPAddItemToCont::Colour( SI16 toSet )
{
	pStream.WriteShort( 18, toSet );
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
	pStream.ReserveSize( 5 );
	pStream.WriteByte( 0, 0x26 );
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
	pStream.WriteLong( 1, toSet );
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
	pStream.ReserveSize( 2 );
	pStream.WriteByte( 0, 0x2C );
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
	pStream.WriteByte( 1, action );
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
	pStream.ReserveSize( 10 );
	pStream.WriteByte( 0, 0x2F );
	pStream.WriteByte( 1, 0 );
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
	pStream.WriteLong( 2, toSet );
}
void CPFightOccurring::Attacker( CChar &attacker )
{
	Attacker( attacker.GetSerial() );
}
void CPFightOccurring::Defender( SERIAL toSet )
{
	pStream.WriteLong( 6, toSet );
}
void CPFightOccurring::Defender( CChar &defender )
{
	Defender( defender.GetSerial() );
}

void CPSkillsValues::InternalReset( void )
{
	pStream.ReserveSize( 1 );
	pStream.WriteByte( 0, 0x3A );
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
	pStream.ReserveSize( newValue );
	pStream.WriteShort( 1, newValue );
	pStream.WriteByte(  3, 0x00 );	// full list
	pStream.WriteByte(  4, 0x00 );
	pStream.WriteByte(  newValue-1, 0x00 );	// finish off with a double NULL
	pStream.WriteByte(  newValue-2, 0x00 );
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
	int size = pStream.GetShort( 1 );
	size -= 6;
	size /= 7;
	return (UI08)size;
}

void CPSkillsValues::SkillEntry( SI16 skillID, SI16 skillVal, SI16 baseSkillVal, UI08 skillLock )
{
	int offset = skillID * 7 + 5;
	pStream.WriteByte(  offset, skillID + 1 );
	pStream.WriteShort( offset + 1, skillVal );
	pStream.WriteShort( offset + 3, baseSkillVal );
	pStream.WriteByte(  offset + 5, skillLock ); 
}
CPSkillsValues &CPSkillsValues::operator=( CChar &toCopy )
{
	CopyData( toCopy );
	return (*this);
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
	pStream.ReserveSize( 19 );
	pStream.WriteByte( 0, 0x90 );
	GumpArt( 0x139D );
}
void CPMapMessage::UpperLeft( SI16 x, SI16 y )
{
	pStream.WriteShort( 7, x );
	pStream.WriteShort( 9, y );
}
void CPMapMessage::LowerRight( SI16 x, SI16 y )
{
	pStream.WriteShort( 11, x );
	pStream.WriteShort( 13, y );
}
void CPMapMessage::Dimensions( SI16 width, SI16 height )
{
	pStream.WriteShort( 15, width );
	pStream.WriteShort( 17, height );
}
void CPMapMessage::GumpArt( SI16 newArt )
{
	pStream.WriteShort( 5, newArt );
}
void CPMapMessage::KeyUsed( long key )
{
	pStream.WriteLong( 1, key );
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
	pStream.ReserveSize( 99 );
	pStream.WriteByte( 0, 0x93 );
}
void CPBookTitlePage::Serial( SERIAL toSet )
{
	pStream.WriteLong( 1, toSet );
}
void CPBookTitlePage::WriteFlag( UI08 flag )
{
	pStream.WriteByte( 5, flag );
}
void CPBookTitlePage::NewFlag( UI08 flag )
{
	pStream.WriteByte( 6, flag );
}
void CPBookTitlePage::Pages( SI16 pages )
{
	pStream.WriteShort( 7, pages );
}
void CPBookTitlePage::Title( const std::string txt )
{
	if( txt.length() >= 60 )
	{
		pStream.WriteString( 9, txt, 59 );
		pStream.WriteByte( 68, 0x00 );
	}
	else
		pStream.WriteString( 9, txt, txt.size() );
}
void CPBookTitlePage::Author( const std::string txt )
{
	if( txt.length() >= 30 )
	{
		pStream.WriteString( 69, txt, 29 );
		pStream.WriteByte( 98, 0x00 );
	}
	else
		pStream.WriteString( 69, txt, txt.size() );
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
	pStream.ReserveSize( 9 );
	pStream.WriteByte( 0, 0xBB );
}
void CPUltimaMessenger::ID1( SERIAL toSet )
{
	pStream.WriteLong( 1, toSet );
}
void CPUltimaMessenger::ID2( SERIAL toSet )
{
	pStream.WriteLong( 5, toSet );
}

void CPGumpTextEntry::InternalReset( void )
{
	BlockSize( 3 );
	pStream.WriteByte( 0, 0xAB );
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
	pStream.WriteLong( 3, id );
}
void CPGumpTextEntry::ParentID( UI08 newVal )
{
	pStream.WriteByte( 7, newVal );
}
void CPGumpTextEntry::ButtonID( UI08 newVal )
{
	pStream.WriteByte( 8, newVal );
}
void CPGumpTextEntry::Cancel( UI08 newVal )
{
	SI16 t1Len = Text1Len();
	pStream.WriteByte( t1Len + 11, newVal );
}
void CPGumpTextEntry::Style( UI08 newVal )
{
	SI16 t1Len = Text1Len();
	pStream.WriteByte( t1Len + 12, newVal );
}
void CPGumpTextEntry::Format( SERIAL id )
{
	SI16 t1Len = Text1Len();
	pStream.WriteLong( t1Len + 13, id );
}
void CPGumpTextEntry::Text1( const std::string txt )
{
	size_t sLen = txt.length();
	BlockSize( 20 + sLen );	// 11 + 1 + 8
	Text1Len( sLen + 1 );
	pStream.WriteString( 11, txt, sLen );
}
void CPGumpTextEntry::Text2( const std::string txt )
{
	size_t sLen			= txt.length();
	SI16 currentSize	= CurrentSize();
	BlockSize( static_cast< UI16 >(currentSize + sLen + 1) );
	Text2Len( static_cast< SI16 >(sLen + 1) );
	pStream.WriteString( currentSize, txt, sLen );
}
void CPGumpTextEntry::BlockSize( SI16 newVal )
{
	pStream.ReserveSize( newVal );
	pStream.WriteByte(  0, 0xAB );
	pStream.WriteShort( 1, newVal );
}

SI16 CPGumpTextEntry::CurrentSize( void )
{
	return pStream.GetShort( 1 );
}

SI16 CPGumpTextEntry::Text1Len( void )
{
	return pStream.GetShort( 9 );
}

void CPGumpTextEntry::Text1Len( SI16 newVal )
{
	pStream.WriteShort( 9, newVal );
}

SI16 CPGumpTextEntry::Text2Len( void )
{
	SI16 t1Len = Text1Len();
	return pStream.GetShort( t1Len + 17 );
}

void CPGumpTextEntry::Text2Len( SI16 newVal )
{
	SI16 t1Len = Text1Len();
	pStream.WriteShort( t1Len + 17, newVal );
}

CPGodModeToggle::CPGodModeToggle()
{
	InternalReset();
}
void CPGodModeToggle::InternalReset( void )
{
	pStream.ReserveSize( 2 );
	pStream.WriteByte( 0, 0x2B );
}

void CPGodModeToggle::CopyData( CSocket *s )
{
	pStream.WriteByte( 1, s->GetByte( 1 ) );
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
		pStream.WriteByte( 1, 1 );
	else
		pStream.WriteByte( 1, 0 );
}

void CPLoginDeny::InternalReset( void )
{
	pStream.ReserveSize( 2 );
	pStream.WriteByte( 0, 0x82 );
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
	pStream.WriteByte( 1, reason );
}

//	Subcommand 8: Set cursor hue / Set MAP 
//	BYTE hue  (0 = Felucca, unhued / BRITANNIA map.  1 = Trammel, hued gold / BRITANNIA map, 2 = (switch to) ILSHENAR map)
//	Note: Server Message

void CPMapChange::Log( std::ofstream &outStream, bool fullHeader )
{
	if( fullHeader )
		outStream << "[SEND]Packet   : CPMapChange 0xBF Subcommand 8 --> Length: " << pStream.GetSize() << TimeStamp() << std::endl;
	outStream << "Hue            : " << (SI16)pStream.GetByte( 5 ) << std::endl;
	switch( pStream.GetByte( 5 ) )
	{
		case 0:		outStream << " (Felucca)" << std::endl;		break;
		case 1:		outStream << " (Trammel)" << std::endl;		break;
		case 2:		outStream << " (Ilshenar)" << std::endl;	break;
		case 3:		outStream << " (Malas)" << std::endl;		break;
		default:	outStream << " (Unknown)" << std::endl;		break;
	}
	outStream << "  Raw dump     :" << std::endl;
	CPUOXBuffer::Log( outStream, false );
}
void CPMapChange::InternalReset( void )
{
	pStream.ReserveSize( 6 );
	pStream.WriteByte( 0, 0xBF );
	pStream.WriteShort( 1, 0x0006 );
	pStream.WriteShort( 3, 0x0008 );	// set client hue
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
	pStream.WriteByte( 5, newMap );
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
	pStream.ReserveSize( 5 );
	pStream.WriteByte( 0, 0x3C );
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
		Type( contType );
		if( isVendor )
			VendorSerial( container->GetSerial() );
		CopyData( mSock, (*container) );
	}
}

void CPItemsInContainer::Type( UI08 contType )
{
	if( contType == 0x01 ) // Corpse
		isCorpse = true;
	else if( contType == 0x02 ) // Vendor
		isVendor = true;
}

void CPItemsInContainer::VendorSerial( SERIAL toSet )
{
	vendorSerial = toSet;
}

UI16 CPItemsInContainer::NumberOfItems( void ) const
{
	return pStream.GetUShort( 3 );
}

void CPItemsInContainer::NumberOfItems( UI16 numItems )
{
	UI16 packetSize		= (UI16)((numItems * 19) + 5);
	pStream.ReserveSize( packetSize );
	pStream.WriteShort( 1, packetSize );
	pStream.WriteShort( 3, numItems );
}
void CPItemsInContainer::AddItem( CItem *toAdd, UI16 itemNum, CSocket *mSock )
{
	pStream.ReserveSize( pStream.GetSize() + 19 );
	UI16 baseOffset = (UI16)(5 + itemNum * 19);
	pStream.WriteLong(  baseOffset +  0, toAdd->GetSerial() );
	pStream.WriteShort( baseOffset +  4, toAdd->GetID() );
	pStream.WriteShort( baseOffset +  7, toAdd->GetAmount() );
	pStream.WriteShort( baseOffset +  9, toAdd->GetX() );
	pStream.WriteShort( baseOffset + 11, toAdd->GetY() );

	if( isVendor )
		pStream.WriteLong( baseOffset + 13, vendorSerial );
	else
		pStream.WriteLong( baseOffset + 13, toAdd->GetContSerial() );

	pStream.WriteShort( baseOffset + 17, toAdd->GetColour() );

	toAdd->SetDecayTime( 0 );

	CPToolTip pSend( toAdd->GetSerial() );
	mSock->Send( &pSend );
}

void CPItemsInContainer::Add( UI16 itemNum, SERIAL toAdd, SERIAL cont, UI08 amount )
{
	UI16 baseOffset = (UI16)(5 + (itemNum * 19));

	pStream.WriteLong(  baseOffset + 0, toAdd );
	pStream.WriteShort( baseOffset + 7,	amount );
	pStream.WriteLong(  baseOffset + 13, cont );
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
				AddItem( ctr, itemCount, mSock );
				++itemCount;
			}
		}
	}
	NumberOfItems( itemCount );
}

void CPItemsInContainer::Log( std::ofstream &outStream, bool fullHeader )
{
	size_t numItems = pStream.GetUShort( 3 );
	if( fullHeader )
		outStream << "[SEND]Packet   : CPItemsInContainer 0x3c --> Length: " << pStream.GetSize() << TimeStamp() << std::endl;
	outStream << "Block size     : " << pStream.GetUShort( 1 ) << std::endl;
	outStream << "Number of Items: " << std::dec << numItems << std::endl;
	int baseOffset = 5;
	for( size_t x = 0; x < numItems; ++x )
	{
		outStream << "  ITEM " << x << "      ID: " << "0x" << std::hex << pStream.GetULong( baseOffset ) << std::endl;
		outStream << "      Model     : " << "0x" << pStream.GetUShort( baseOffset+=4 ) << std::endl;
		outStream << "      Amount    : " << std::dec << pStream.GetUShort( baseOffset+=3 ) << std::endl;
		outStream << "      XY        : " << pStream.GetUShort( baseOffset+=2 ) << "," <<
			pStream.GetUShort( baseOffset+=2 ) << std::endl;
		outStream << "      Container : " << "0x" << std::hex << pStream.GetULong( baseOffset+=2 ) << std::endl;
		outStream << "      Color     : " << "0x" << pStream.GetUShort( baseOffset+=4 ) << std::endl;
		baseOffset += 2;
	}
	outStream << "  Raw dump      :" << std::endl;

	CPUOXBuffer::Log( outStream, false );
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
//NOTE: The client displays items in the buy window from top left to bottom right. This means we need to sort the items logically before sending packets.
void CPOpenBuyWindow::InternalReset( void )
{
	pStream.ReserveSize( 8 );	// start big, and work back down
	pStream.WriteByte( 0, 0x74 );
}
CPOpenBuyWindow::CPOpenBuyWindow()
{
	InternalReset();
}
CPOpenBuyWindow::CPOpenBuyWindow( CItem *container, CChar *vendorID, CPItemsInContainer& iic, CSocket *mSock )
{
	if( ValidateObject( container ) )
	{
		InternalReset();
		pStream.WriteLong( 3, container->GetSerial() );
		CopyData( (*container), vendorID, iic, mSock );
	}
}

UI08 CPOpenBuyWindow::NumberOfItems( void ) const
{
	return pStream.GetByte( 7 );
}

void CPOpenBuyWindow::NumberOfItems( UI08 numItems )
{
	// set the number of items
	pStream.WriteByte( 7, numItems );
}

UI32 calcGoodValue( CTownRegion *tReg, CItem *i, UI32 value, bool isSelling );
UI32 calcValue( CItem *i, UI32 value );
void CPOpenBuyWindow::AddItem( CItem *toAdd, CTownRegion *tReg, UI16 &baseOffset )
{
	UI32 value = toAdd->GetBuyValue();
	if( cwmWorldState->ServerData()->RankSystemStatus() )
		value = calcValue( toAdd, value );
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

	pStream.ReserveSize( baseOffset + 5 + sLen );
	pStream.WriteLong(   baseOffset, value );
	pStream.WriteByte(   baseOffset += 4, sLen );
	pStream.WriteString( baseOffset += 1, itemname, sLen );
	baseOffset += sLen;
}

void CPOpenBuyWindow::CopyData( CItem& toCopy, CChar *vendorID, CPItemsInContainer& iic, CSocket *mSock )
{
	UI08 itemCount	= 0;
	UI16 length		= 8;
	CTownRegion *tReg = NULL;
	if( cwmWorldState->ServerData()->TradeSystemStatus() && ValidateObject( vendorID ) )
		tReg = calcRegionFromXY( vendorID->GetX(), vendorID->GetY(), vendorID->WorldNumber() );

	SI16 baseY = 0, baseX = 0;
	switch( toCopy.GetLayer() )
	{
		case IL_BUYCONTAINER:			// buy layer
			break;
		case IL_BOUGHTCONTAINER:		// bought layer
			baseY = 100;
			break;
		default:
			break;
	}

	CDataList< CItem * > *tcCont = toCopy.GetContainsList();
	for( CItem *ctr = tcCont->First(); !tcCont->Finished(); ctr = tcCont->Next() )
	{
		if( ValidateObject( ctr ) )
		{
			if( !ctr->isFree() )
			{
				ctr->WalkXY( ++baseX, baseY );
				if( baseX == 200 )
				{
					baseX = 0;
					++baseY;
				}

				iic.AddItem( ctr, itemCount, mSock );
				AddItem( ctr, tReg, length );
				++itemCount;
			}
		}
	}
	iic.NumberOfItems( itemCount );

	NumberOfItems( itemCount );
	pStream.ReserveSize( length );
	pStream.WriteShort( 1, length );
}

void CPOpenBuyWindow::Log( std::ofstream &outStream, bool fullHeader )
{
	if( fullHeader )
		outStream << "[SEND]Packet   : CPOpenBuyWindow 0x74 --> Length: " << pStream.GetSize() << TimeStamp() << std::endl;
	outStream << "Block size     : " << pStream.GetUShort( 1 ) << std::endl;
	outStream << "Vendor ID      : " << std::hex << pStream.GetULong( 3 ) << std::endl;
	outStream << "Number of Items: " << std::dec << (SI16)pStream.GetByte( 7 ) << std::endl;
	int baseOffset = 8;
	for( UI32 x = 0; x < pStream.GetByte( 7 ); ++x )
	{
		outStream << "  ITEM " << x << std::endl;
		outStream << "      Price: " << pStream.GetULong( baseOffset ) << std::endl;
		baseOffset += 4;
		outStream << "      Len  : " << (SI16)pStream.GetByte( baseOffset ) << std::endl;
		outStream << "      Name : ";
		for( UI08 y = 0; y < pStream.GetByte( baseOffset ); ++y )
			outStream << pStream.GetByte( baseOffset + 1 + y );
		baseOffset += pStream.GetByte( baseOffset ) + 1;
		outStream << std::endl;
	}

	outStream << "  Raw dump :" << std::endl;

	CPUOXBuffer::Log( outStream, false );
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
//		0x01	= unknown
//		0x02	= send config/req logout (IGR?)
//		0x04	= single character (siege) (alternative seen, Limit Characters)
//		0x08	= enable npcpopup menus
//		0x10	= unknown, (alternative seen, single character)
//		0x20	= enable common AOS features (tooltip thing/fight system book, but not AOS monsters/map/skills)
//		0x40	= Sixth Character Slot?
//		8x80	= Samurai Empire?
//		0x100	= Elf races?
void CPCharAndStartLoc::Log( std::ofstream &outStream, bool fullHeader )
{
	if( fullHeader )
		outStream << "[SEND]Packet   : CPCharAndStartLoc 0xA9 --> Length: " << pStream.GetSize() << TimeStamp() << std::endl;
	outStream << "# Chars        : " << (SI16)pStream.GetByte( 3 ) << std::endl;
	outStream << "Characters --" << std::endl;

	UI32 startLocOffset, realChars;
	if( pStream.GetByte( 3 ) > 5 )
	{
		startLocOffset	= 364;
		realChars		= 6;
	}
	else
	{
		startLocOffset	= 304;
		realChars		= 5;
	}

	for( UI08 i = 0; i < realChars; ++i )
	{
		UI32 baseOffset = 4 + i * 60;
		outStream << "    Character " << (UI16)i << ":" << std::endl;
		outStream << "      Name: ";
		for( UI08 j = 0; j < 30; ++j )
		{
			if( pStream.GetByte( baseOffset+j ) != 0 )
				outStream << (char)pStream.GetByte( baseOffset+j );
			else
				break;
		}
		outStream << std::endl << "      Pass: ";
		for( UI08 k = 0; k < 30; ++k )
		{
			if( pStream.GetByte( baseOffset+k+30 ) != 0 )
				outStream << (char)pStream.GetByte( baseOffset+k+30 );
			else
				break;
		}
		outStream << std::endl;
	}

	outStream << "# Starts       : " << (SI16)pStream.GetByte( startLocOffset ) << std::endl;
	outStream << "Starting locations --" << std::endl;
	for( UI08 l = 0; l < pStream.GetByte( startLocOffset ); ++l )
	{
		UI32 baseOffset = startLocOffset + 1 + l * 63;
		outStream << "    Start " << (SI16)l << std::endl;
		outStream << "      Index       : " << (SI16)pStream.GetByte( baseOffset ) << std::endl;
		outStream << "      General Name: ";
		++baseOffset;
		for( UI08 m = 0; m < 31; ++m )
		{
			if( pStream.GetByte( baseOffset+m ) != 0 )
				outStream << pStream.GetByte( baseOffset+m );
			else
				break;
		}
		outStream << std::endl << "      Exact Name  : ";
		baseOffset += 31;
		for( UI08 n = 0; n < 31; ++n )
		{
			if( pStream.GetByte( baseOffset+n ) != 0 )
				outStream << pStream.GetByte( baseOffset+n );
			else
				break;
		}
		outStream << std::endl;
	}
	UI16 lastByte = pStream.GetUShort( pStream.GetSize() - 2 );
	outStream << "Flags          : " << std::hex << (UI32)lastByte << std::dec << std::endl;
	if( (lastByte&0x02) == 0x02 )
		outStream << "               : Send config/request logout" << std::endl;
	if( (lastByte&0x04) == 0x04 )
		outStream << "               : Single character" << std::endl;
	if( (lastByte&0x08) == 0x08 )
		outStream << "               : Enable NPC Popup menus" << std::endl;
	if( (lastByte&0x10) == 0x10 )
		outStream << "               : Unknown" << std::endl;
	if( (lastByte&0x20) == 0x20 )
		outStream << "               : Enable Common AoS features" << std::endl;
	if( (lastByte&0x40) == 0x40 )
		outStream << "               : Enable Sixth Character Slot?" << std::endl;
	if( (lastByte&0x80) == 0x80 )
		outStream << "               : Enable Samurai Empire?" << std::endl;
	if( (lastByte&0x100) == 0x100 )
		outStream << "               : Enable Elves and ML?" << std::endl;
	outStream << "  Raw dump     :" << std::endl;
	CPUOXBuffer::Log( outStream, false );
}

void CPCharAndStartLoc::InternalReset( void )
{
	pStream.ReserveSize( 4 );
	pStream.WriteByte( 0, 0xA9 );
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
	if( pStream.GetByte( 3 ) > 5 )
		packetSize = (UI16)(369 + 63 * numLocations);
	else
		packetSize = (UI16)(309 + 63 * numLocations);
	pStream.ReserveSize( packetSize );
	pStream.WriteShort( 1, packetSize );
	// If we are going to support the 6char client flag then we need to make sure we push this offset furtner down.
	if( pStream.GetByte( 3 ) > 5 )
		pStream.WriteByte( 364, numLocations );
	else
		pStream.WriteByte( 304, numLocations );
	pStream.WriteShort( packetSize - 2, ( 0x08 | 0x20 | 0x40 | 0x80 | 0x100 ) );
}
void CPCharAndStartLoc::NumberOfCharacters( UI08 numCharacters )
{
	pStream.WriteByte( 3, numCharacters );
}

void CPCharAndStartLoc::AddCharacter( CChar *toAdd, UI08 charOffset )
{
	if( !ValidateObject( toAdd ) )
		return;
	UI16 baseOffset = (UI16)(4 + charOffset * 60);
	pStream.WriteString( baseOffset, toAdd->GetName(), 59 );
	// extra 30 bytes unused, as we don't use a character password
}

void CPCharAndStartLoc::AddStartLocation( LPSTARTLOCATION sLoc, UI08 locOffset )
{
	if( sLoc == NULL )
		return;
	UI16 baseOffset;
	if( pStream.GetByte( 3 ) > 5 )
			baseOffset = (UI16)(365 + locOffset * 63);
	else
			baseOffset = (UI16)(305 + locOffset * 63);
	pStream.WriteByte( baseOffset, locOffset );
	size_t townLen				= strlen( sLoc->town );
	size_t descLen				= strlen( sLoc->description );
	pStream.WriteString( baseOffset+1, sLoc->town, townLen );
	pStream.WriteString( baseOffset+32, sLoc->description, descLen );
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
	pStream.ReserveSize( 5 );
	pStream.WriteByte(  0, 0xF0 );
	pStream.WriteShort( 1, 0x0005 );
	pStream.WriteByte(  3, 0x00 );
	pStream.WriteByte(  4, Response );
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
	pStream.ReserveSize( 10 );	// 10, not 11
	pStream.WriteByte( 0, 0xA6 );
	pStream.WriteByte( 1, 0x01 );
	pStream.WriteByte( 2, 0x02 );
	pStream.WriteByte( 3, 0x02 );
	pStream.WriteByte( 4, 0x00 );
	pStream.WriteByte( 5, 0x00 );
	pStream.WriteByte( 6, 0x00 );
	pStream.WriteByte( 7, 0x00 );
	pStream.WriteByte( 8, 0x01 );
	pStream.WriteByte( 9, 0x02 );
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
	pStream.WriteByte( 3, tType );
}
void CPUpdScroll::TipNumber( UI08 tipNum )
{
	pStream.WriteByte( 7, tipNum );
}
void CPUpdScroll::Finalize( void )
{
	size_t y = strlen( tipData ) + 10;
	SetLength( static_cast< UI16 >(y) );
	pStream.WriteString( 10, tipData, y-10 );
}

void CPUpdScroll::SetLength( UI16 len )
{
	pStream.ReserveSize( len );
	pStream.WriteShort( 1, len );
	pStream.WriteShort( 8, (len - 10) );
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
	pStream.ReserveSize( 36 );
	pStream.WriteByte(  0, 0xC0 );
	pStream.WriteShort( 24, 0x0000 );
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
	pStream.WriteLong( 28, hue );
}
void CPGraphicalEffect2::RenderMode( UI32 mode )
{
	pStream.WriteLong( 32, mode );
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
	pStream.ReserveSize( 11 );
	pStream.WriteByte( 0, 0x56 );
}
void CPMapRelated::PlotState( UI08 pState )
{
	pStream.WriteByte( 6, pState );
}
void CPMapRelated::Location( SI16 x, SI16 y )
{
	pStream.WriteShort( 7, x );
	pStream.WriteShort( 9, y );
}
void CPMapRelated::Command( UI08 cmd )
{
	pStream.WriteByte( 5, cmd );
}
void CPMapRelated::ID( SERIAL key )
{
	pStream.WriteLong( 1, key );
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
	pStream.WriteByte( 0, 0x78 );
}

void CPDrawObject::SetLength( UI16 len )
{
	pStream.ReserveSize( len );
	pStream.WriteShort( 1, len );
	curLen = len;
}

void CPDrawObject::Finalize( void )
{
	UI16 cPos = curLen;
	SetLength( curLen + 4 );
	pStream.WriteLong( cPos, static_cast<UI32>(0) );
}

void CPDrawObject::AddItem( CItem *toAdd )
{
	UI16 cPos = curLen;
	bool bColour = ( toAdd->GetColour() != 0 );
	if( bColour )
		SetLength( curLen + 9 );
	else
		SetLength( curLen + 7 );

	pStream.WriteLong(  cPos, toAdd->GetSerial() );
	pStream.WriteShort( cPos+=4, toAdd->GetID() );
	pStream.WriteByte(  cPos+=2, toAdd->GetLayer() );

	if( bColour )
	{
		pStream.WriteByte( cPos-2, pStream.GetByte( cPos-2 ) | 0x80 );
		pStream.WriteShort( ++cPos, toAdd->GetColour() );
	}
}

void CPDrawObject::SetRepFlag( UI08 value )
{
	pStream.WriteByte( 18, value );
}

void CPDrawObject::CopyData( CChar& mChar )
{
	pStream.WriteLong(   3, mChar.GetSerial() );
	pStream.WriteShort(  7, mChar.GetID() );
	pStream.WriteShort(  9, mChar.GetX() );
	pStream.WriteShort( 11, mChar.GetY() );
	pStream.WriteByte(  13, mChar.GetZ() );
	pStream.WriteByte(  14, mChar.GetDir() );
	pStream.WriteShort( 15, mChar.GetSkin() );

	UI08 cFlag = 0;
	const UI08 BIT_POISON	= 0x04;
	const UI08 BIT_ATWAR	= 0x40;
	const UI08 BIT_OTHER	= 0x80;

	MFLAGSET( cFlag, mChar.GetPoisoned(), BIT_POISON );
	MFLAGSET( cFlag, ( ( !mChar.IsNpc() && !isOnline( mChar ) ) || ( mChar.GetVisible() != VT_VISIBLE )  || ( mChar.IsDead() && !mChar.IsAtWar() ) ), BIT_OTHER );
	MFLAGSET( cFlag, mChar.IsAtWar(), BIT_ATWAR );
	pStream.WriteByte( 17, cFlag );
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
	pStream.ReserveSize( 7 );
	pStream.WriteByte( 0, 0x89 );
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
	pStream.ReserveSize( packetSize );
	pStream.WriteShort( 1, packetSize );
	pStream.WriteByte(  packetSize - 1, 0x00 );
}
void CPCorpseClothing::AddItem( CItem *toAdd, UI16 itemNum )
{
	UI16 baseOffset = (UI16)(7 + itemNum * 5);
	pStream.WriteByte( baseOffset, toAdd->GetLayer() );
	pStream.WriteLong( baseOffset + 1, toAdd->GetSerial() );
}
CPCorpseClothing& CPCorpseClothing::operator=( CItem& corpse )
{
	CopyData( corpse );
	return (*this);
}

void CPCorpseClothing::CopyData( CItem& toCopy )
{
	pStream.WriteLong( 3, toCopy.GetSerial() );
	UI16 itemCount = 0;
	CDataList< CItem * > *tcCont = toCopy.GetContainsList();
	for( CItem *ctr = tcCont->First(); !tcCont->Finished(); ctr = tcCont->Next() )
	{
		if( ValidateObject( ctr ) )
		{
			if( !ctr->isFree() && ctr->GetLayer() )
			{
				pStream.ReserveSize( pStream.GetSize() + 5 );
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
	pStream.ReserveSize( 16 );
	pStream.WriteByte( 0, 0x1A );
	pStream.WriteShort( 1, 16 );
}
void CPObjectInfo::CopyData( CItem& mItem, CChar& mChar )
{
	pStream.WriteLong( 3, mItem.GetSerial() );

	if( mItem.CanBeObjType( OT_MULTI ) )
		CopyMultiData( static_cast<CMultiObj&>(mItem), mChar );
	else if( mItem.CanBeObjType( OT_ITEM ) )
		CopyItemData( mItem, mChar );
}

void CPObjectInfo::CopyItemData( CItem &mItem, CChar &mChar )
{
	if( mItem.isPileable() || mItem.isCorpse() )
	{
		pStream.ReserveSize( 18 );
		pStream.WriteByte( 2, 18 );
		pStream.WriteByte( 3, (pStream.GetByte( 3 ) | 0x80) );	// Enable piles
	}

	bool isInvisible	= (mItem.GetVisible() != VT_VISIBLE);
	bool isMovable		= (mItem.GetMovable() == 1 || mChar.AllMove() || ( mItem.IsLockedDown() && &mChar == mItem.GetOwnerObj() ));
	// if player is a gm, this item
	// is shown like a candle (so that he can move it),
	// ....if not, the item is a normal
	// invisible light source!
	if( mChar.IsGM() && mItem.GetID() == 0x1647 )
		pStream.WriteShort( 7, 0x0A0F );
	else
		pStream.WriteShort( 7, mItem.GetID() );

	UI08 byteNum = 7;
	if( mItem.isPileable() || mItem.isCorpse() )
		pStream.WriteShort(  byteNum+=2, mItem.GetAmount() );

	pStream.WriteShort( byteNum+=2, mItem.GetX() );
	if( isInvisible || isMovable )
	{
		pStream.WriteShort( byteNum+=2, (mItem.GetY() | 0xC000) );
		pStream.ReserveSize( pStream.GetSize()+1 );
		pStream.WriteByte( 2, pStream.GetByte( 2 ) + 1 );
	}
	else
		pStream.WriteShort( byteNum+=2, (mItem.GetY() | 0x8000) );
	if( mItem.GetDir() )
	{
		pStream.ReserveSize( pStream.GetSize()+1 );
		pStream.WriteByte( byteNum-2, (pStream.GetByte( byteNum-2 ) | 0x80) );	// Enable direction
		pStream.WriteByte( byteNum+=2, mItem.GetDir() );
		pStream.WriteByte( ++byteNum, mItem.GetZ() );
		pStream.WriteByte( 2, pStream.GetByte( 2 ) + 1 );
	}
	else
		pStream.WriteByte( byteNum+=2, mItem.GetZ() );

	if( mChar.IsGM() && mItem.GetID() == 0x1647 )
		pStream.WriteShort( ++byteNum, 0x00C6 );
	else
		pStream.WriteShort( ++byteNum, mItem.GetColour() );

	if( isInvisible || isMovable )
	{
		pStream.WriteByte( byteNum+=2, 0x00 );
		if( isInvisible )
			pStream.WriteByte( byteNum, (pStream.GetByte( byteNum ) | 0x80) );

		if( isMovable )
			pStream.WriteByte( byteNum, (pStream.GetByte( byteNum ) | 0x20) );
	}
}

void CPObjectInfo::CopyMultiData( CMultiObj& mMulti, CChar &mChar )
{
	bool isInvisible	= (mMulti.GetVisible() != VT_VISIBLE);
	bool isMovable		= (mChar.AllMove());

	if( mChar.ViewHouseAsIcon() && mMulti.GetID() >= 0x4000 )
		pStream.WriteShort( 7, 0x14F0 );
	else
		pStream.WriteShort( 7, mMulti.GetID() );

	pStream.WriteShort( 9, mMulti.GetX() );
	if( isInvisible || isMovable )
	{
		pStream.WriteShort( 11, (mMulti.GetY() | 0xC000) );
		pStream.ReserveSize( pStream.GetSize()+1 );
		pStream.WriteByte( 2, pStream.GetByte( 2 ) + 1 );
	}
	else
		pStream.WriteShort( 11, (mMulti.GetY() | 0x8000) );
	UI08 byteNum = 12;
	if( mMulti.GetDir() )
	{
		pStream.ReserveSize( pStream.GetSize()+1 );
		pStream.WriteByte( 2, pStream.GetByte( 2 ) + 1 );
		pStream.WriteByte( 9, (pStream.GetByte( 9 ) | 0x80) );	// Enable direction
		pStream.WriteByte( ++byteNum, mMulti.GetDir() );
	}
	pStream.WriteByte( ++byteNum, mMulti.GetZ() );

	pStream.WriteShort( ++byteNum, mMulti.GetColour() );
	if( isInvisible || isMovable )
	{
		pStream.WriteByte( ++byteNum, 0x00 );
		if( isInvisible )
			pStream.WriteByte( byteNum, (pStream.GetByte( byteNum ) | 0x80) );

		if( isMovable ) 
			pStream.WriteByte( byteNum, (pStream.GetByte( byteNum ) | 0x20) );
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
	pStream.WriteByte( 9, value );
}
void CPSpeech::Colour( COLOUR value )
{
	// Ideally one would track down where the value is getting set to 0, 
	// a color of 0 will crash 3d client
	if( value == 0 )
	{
		value = 0x3B2;
	}
	pStream.WriteShort( 10, value );
}
void CPSpeech::Font( UI16 value )
{
	pStream.WriteShort( 12, value );
}
void CPSpeech::Name( std::string value )
{
	pStream.WriteString( 14, value, 30 );
}
void CPSpeech::Message( char *value )
{
	size_t length = strlen( value );
	SetLength( static_cast< UI16 >(44 + length + 1) );
	pStream.WriteString( 44, value, length );
}
void CPSpeech::SetLength( UI16 value )
{
	pStream.ReserveSize( value );
	pStream.WriteShort( 1, value );
}

void CPSpeech::InternalReset( void )
{
	SetLength( 44 );
	pStream.WriteByte( 0, 0x1C );
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
	pStream.WriteLong( 3, toSet );
}
void CPSpeech::ID( UI16 toSet )
{
	pStream.WriteShort( 7, toSet );
}
void CPSpeech::GrabSpeech( CSocket *mSock, CChar *mChar )
{
	pStream.WriteByte( 10, mSock->GetByte( 4 ) );
	pStream.WriteByte( 11, mSock->GetByte( 5 ) );
	pStream.WriteByte( 12, mSock->GetByte( 6 ) );
	pStream.WriteByte( 13, mChar->GetFontType() );
}

void CPSpeech::GhostIt( UI08 method )
{
	// Method ignored currently
	// Designed with the idea that you can garble text in more than one way
	// eg 0 == ghost, 1 == racial, 2 == magical, etc etc
	for( UI16 j = 44; j < pStream.GetSize() - 1; ++j )
	{
		if( pStream.GetByte( j ) != 32 )
			pStream.WriteByte( j, ( RandomNum( 0, 1 ) == 0 ? 'O' : 'o' ) );
	}
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
	pStream.WriteByte( 9, ( value & 0x0F ) );
}
void CPUnicodeSpeech::Colour( COLOUR value )
{
	pStream.WriteShort( 10, value );
}
void CPUnicodeSpeech::Font( UI16 value )
{
	pStream.WriteShort( 12, value );
}
void CPUnicodeSpeech::Language( char *value )
{
	pStream.WriteString( 14, value, 4 );
}
void CPUnicodeSpeech::Name( std::string value )
{
	pStream.WriteString( 18, value, 30 );
}
void CPUnicodeSpeech::Message( const char *value )
{
	size_t length = strlen( value );
	SetLength( static_cast< UI16 >(48 + (2 * length) + 2) );
	for( size_t i = 0; i < length; ++i )
		pStream.WriteByte( 48 + i * 2, value[i] );
}
void CPUnicodeSpeech::SetLength( UI16 value )
{
	pStream.ReserveSize( value );
	pStream.WriteShort( 1, value );
}

void CPUnicodeSpeech::InternalReset( void )
{
	SetLength( 48 );
	pStream.WriteByte( 0, 0xAE );
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

	UI16 length = talking.Length();
	char *uniTxt = talking.UnicodeText();

	SetLength( 48 + (2 * length) );
	pStream.WriteArray( 48, (UI08 *)uniTxt, (2 * length) );
}
void CPUnicodeSpeech::Serial( SERIAL toSet )
{
	pStream.WriteLong( 3, toSet );
}
void CPUnicodeSpeech::ID( UI16 toSet )
{
	pStream.WriteShort( 7, toSet );
}
void CPUnicodeSpeech::GrabSpeech( CSocket *mSock, CChar *mChar )
{
	pStream.WriteByte( 10, mSock->GetByte( 4 ) );
	pStream.WriteByte( 11, mSock->GetByte( 5 ) );
	pStream.WriteByte( 12, mSock->GetByte( 6 ) );
	pStream.WriteByte( 13, mChar->GetFontType() );
}

void CPUnicodeSpeech::GhostIt( UI08 method )
{
	// Method ignored currently
	// Designed with the idea that you can garble text in more than one way
	// eg 0 == ghost, 1 == racial, 2 == magical, etc etc
	for( UI16 j = 49; j < pStream.GetSize() - 1; j += 2 )
	{
		if( pStream.GetByte( j ) != 32 )
			pStream.WriteByte( j, ( RandomNum( 0, 1 ) == 0 ? 'O' : 'o' ) );
	}
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
	pStream.ReserveSize( 6 );
	pStream.WriteByte( 0, 0xA8 );
	pStream.WriteByte( 2, 6 );
	pStream.WriteByte( 3, 0xFF );
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
	pStream.ReserveSize( tlen );
	pStream.WriteShort( 1, static_cast< UI16 >(tlen) );
	pStream.WriteShort( 4, numItems );
}
void CPGameServerList::AddServer( UI16 servNum, physicalServer *data )
{
	UI32 baseOffset = 6 + servNum * 40;
	pStream.WriteShort(  baseOffset, servNum + 1 );
	pStream.WriteString( baseOffset + 2, data->getName(), data->getName().length() );
	UI32 ip = htonl( inet_addr( data->getIP().c_str() ) );
	pStream.WriteLong(  baseOffset + 36, ip );
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
	pStream.ReserveSize( 8 );
	pStream.WriteByte( 0, 0x6F );
	pStream.WriteByte( 2, 8 );
}
void CPSecureTrading::CopyData( CBaseObject& mItem, SERIAL mItem2, SERIAL mItem3 )
{
	pStream.ReserveSize( 16 );
	pStream.WriteByte( 2, 16 );
	pStream.WriteLong( 4,  mItem.GetSerial() );
	pStream.WriteLong( 8,  mItem2			);
	pStream.WriteLong( 12, mItem3			);
}
CPSecureTrading::CPSecureTrading()
{
	InternalReset();
}
CPSecureTrading::CPSecureTrading( CBaseObject& mItem )
{
	InternalReset();
	pStream.WriteLong( 4, mItem.GetSerial() );
}
CPSecureTrading::CPSecureTrading( CBaseObject& mItem, SERIAL mItem2, SERIAL mItem3 )
{
	InternalReset();
	CopyData( mItem, mItem2, mItem3 );
}

void CPSecureTrading::Action( UI08 value )
{
	pStream.WriteByte( 3, value );
}
void CPSecureTrading::Name( const std::string nameFollowing )
{
	pStream.ReserveSize( 47 );
	pStream.WriteByte(	 2, 47 );
	pStream.WriteByte(   16, 1 );
	if( nameFollowing.length() >= 30 )
		pStream.WriteString( 17, nameFollowing, 30 );
	else
		pStream.WriteString( 17, nameFollowing, nameFollowing.length() );
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
	pStream.ReserveSize( 37 );
	pStream.WriteByte( 0, 0x98 );
	pStream.WriteByte( 2, 37 );
}
void CPAllNames3D::CopyData( CBaseObject& obj )
{
	pStream.WriteLong(   3, obj.GetSerial() );
	pStream.WriteString( 7, obj.GetName(), obj.GetName().length() );
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
	pStream.ReserveSize( bookLength );
}
void CPBookPage::InternalReset( void )
{
	pStream.ReserveSize( 9 );
	bookLength			= 9;
	pageCount			= 0;
	pStream.WriteByte( 0, 0x66 );
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
		pStream.WriteShort( baseOffset, pageCount );
	else
		pStream.WriteShort( baseOffset, pNum );
	pStream.WriteByte( baseOffset + 3, 8 );	// 8 lines per page
}
void CPBookPage::AddLine( const std::string line )
{
	UI16 baseOffset = bookLength;
	size_t strLen	= line.length() + 1;
	IncLength( static_cast< UI08 >(strLen) );
	pStream.WriteString( baseOffset, line, line.length() );
}
void CPBookPage::NewPage( SI16 pNum, const STRINGLIST *lines )
{
	++pageCount;	// 1 based counter
	UI16 baseOffset = bookLength;
	IncLength( 4 );
	if( pNum == -1 )
		pStream.WriteShort( baseOffset, pageCount );
	else
		pStream.WriteShort( baseOffset, pNum );
	pStream.WriteByte( baseOffset + 3, lines->size() );	// 8 lines per page

	for( STRINGLIST_CITERATOR lIter = lines->begin(); lIter != lines->end(); ++lIter )
	{
		AddLine( (*lIter) );
	}
}
void CPBookPage::Finalize( void )
{
	pStream.WriteShort( 1, bookLength );
	pStream.WriteShort( 7, pageCount );
}
void CPBookPage::Serial( SERIAL value )
{
	pStream.WriteLong( 3, value );
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

CPSendGumpMenu::CPSendGumpMenu()
{
	pStream.ReserveSize( 21 );
	pStream.WriteByte( 0, 0xB0 );		// command byte
	pStream.WriteByte( 14, 0x6E );		// default x
	pStream.WriteByte( 18, 0x46 );		// default y
}
void CPSendGumpMenu::UserID( SERIAL value )
{
	pStream.WriteLong( 3, value );
}
void CPSendGumpMenu::GumpID( SERIAL value )
{
	pStream.WriteLong( 7, value );
}
void CPSendGumpMenu::X( UI32 value )
{
	pStream.WriteLong( 11, value );
}
void CPSendGumpMenu::Y( UI32 value )
{
	pStream.WriteLong( 15, value );
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
	UI16 increment	= 0;
	UI16 lineLen	= 0;

	std::string cmdString;

	for( STRINGLIST_CITERATOR cIter = commands.begin(); cIter != commands.end(); ++cIter )
	{
		lineLen = static_cast<UI16>((*cIter).length());
		if( lineLen == 0 )
			break;
		increment = static_cast<UI16>(lineLen + 4);
		if( (length + increment) >= 0xFFFF )
		{
			Console.Warning( 2, "SendGump Packet (0xB0) attempted to send a packet that exceeds 65355 bytes!" );
			break;
		}

		pStream.ReserveSize( length + increment );
		cmdString = "{ " + (*cIter) + " }";
		pStream.WriteString( length, cmdString, increment );
		length	+= increment;
	}

	if( length > 65536 )
		throw std::runtime_error( "Packet 0xB0 is far too large" );

	pStream.WriteShort( 19, static_cast<UI16>(length-20) );

	UI32 tlOff	= length + 1;
	length		+= 3;
	UI32 tlines = 0;

	pStream.ReserveSize( length );	// match the 3 byte increase

	for( STRINGLIST_CITERATOR tIter = text.begin(); tIter != text.end(); ++tIter )
	{
		lineLen = static_cast<UI16>((*tIter).length());
		if( lineLen == 0 )
			break;
		// Unfortunately, unicode strings are... different
		// so we can't use PackString
		increment	= lineLen * 2 + 2;
		if( (length + increment) >= 0xFFFF )
		{
			Console.Warning( 2, "SendGump Packet (0xB0) attempted to send a packet that exceeds 65355 bytes!" );
			break;
		}

		pStream.ReserveSize( length + increment );
		pStream.WriteShort( length, lineLen );
		for( UI16 i = 0; i < lineLen; ++i )
			pStream.WriteByte( length + 3 + i*2, (*tIter)[i] );
		length += increment;
		++tlines;
	}
	
	if( length > 65536 )
		throw std::runtime_error( "Packet 0xB0 is far too large" );

	pStream.WriteShort( 1, static_cast< UI16 >(length) );
	pStream.WriteShort( tlOff, static_cast< UI16 >(tlines) );
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
		outStream << "[SEND]Packet     : CPSendGumpMenu 0xB0 --> Length: " << pStream.GetSize() << TimeStamp() << std::endl;
	outStream << "ID               : " << std::hex << pStream.GetULong( 3 ) << std::endl;
	outStream << "GumpID           : " << std::hex << pStream.GetULong( 7 ) << std::endl;
	outStream << "X                : " << std::hex << pStream.GetUShort( 11 ) << std::endl;
	outStream << "Y                : " << std::hex << pStream.GetUShort( 15 ) << std::endl;
	outStream << "Command Sec Len  : " << std::dec << pStream.GetUShort( 19 ) << std::endl;
	outStream << "Commands         : " << std::endl;
	for( size_t x = 0; x < commands.size(); ++x )
		outStream << "     " << commands[x] << std::endl;
	outStream << "Text             : " << std::endl;
	for( size_t y = 0; y < text.size(); ++y )
		outStream << "     " << text[y] << std::endl;

	outStream << "  Raw dump     :" << std::endl;
	CPUOXBuffer::Log( outStream, false );
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
	pStream.ReserveSize( 23 );
	pStream.WriteByte( 0, 0xBF );
	pStream.WriteShort( 1, 23 );
	pStream.WriteShort( 3, 0x1B );
	pStream.WriteShort( 5, 0x01 );
	pStream.WriteByte( 11, 0x0E );
	pStream.WriteByte( 12, 0xFA );
	pStream.WriteShort( 13, 1 );
}
void CPNewSpellBook::CopyData( CItem& obj )
{
	pStream.WriteLong( 7, obj.GetSerial() );
	for( UI08 i = 0 ; i < 64 ; ++i )
	{
		int y = (i % 8);
		int x = 15 + (int)(i / 8);
		if( Magic->HasSpell( &obj, i ) )
			pStream.WriteByte( x, (pStream.GetByte( x ) | static_cast<UI08>(power( 2, y ))) );
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
// BYTE[2]	unknown, always 1 
// BYTE[4]	Serial 
// BYTE		Damage // how much damage was done ? 
//
//Note: displays damage above the npc/player’s head.

void CPDisplayDamage::InternalReset( void )
{
	pStream.ReserveSize( 11 );
	pStream.WriteByte( 0, 0xBF );
	pStream.WriteByte( 1, 0x00 );
	pStream.WriteByte( 2, 0x0B );
	pStream.WriteByte( 3, 0x00 );
	pStream.WriteByte( 4, 0x22 );
	pStream.WriteByte( 5, 0x01 );
}
void CPDisplayDamage::CopyData( CChar& ourTarg, UI16 ourDamage )
{
	pStream.WriteLong( 6, ourTarg.GetSerial() );
	pStream.WriteByte( 10, (ourDamage>>8) + (ourDamage%256) );
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
	pStream.ReserveSize( 13 );
	pStream.WriteByte( 0, 0xBF );
	pStream.WriteByte( 1, 0x00 );
	pStream.WriteByte( 2, 0x0D );
	pStream.WriteByte( 3, 0x00 );
	pStream.WriteByte( 4, 0x10 );
	pStream.WriteByte( 9, 0x78 );
	pStream.WriteByte( 10, 0xA1 );
	pStream.WriteByte( 11, 0xBA );
	pStream.WriteByte( 12, 0x2B );
}
void CPQueryToolTip::CopyData( CBaseObject& mObj )
{
	pStream.WriteLong( 5, mObj.GetSerial() );
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
	pStream.ReserveSize( 15 );
	pStream.WriteByte( 0, 0xD6 );
	pStream.WriteShort( 3, 0x0001 );
	pStream.WriteShort( 9, 0x0000 );
	pStream.WriteLong( 11, 0x00000000 );
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

	if( cItem.GetLayer() != IL_NONE )
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
	pStream.ReserveSize( packetLen );
	pStream.WriteShort( 1, packetLen );
	pStream.WriteLong(  5, objSer );

	size_t modifier = 14;
	//loop through all lines
	for( size_t i = 0; i < ourEntries.size(); ++i )
	{
		size_t stringLen = ourEntries[i].stringLen;
		pStream.WriteLong( ++modifier, ourEntries[i].stringNum );
		modifier += 4;
		pStream.WriteShort( modifier, stringLen );
		modifier += 1;

		//convert to uni character
		for( size_t j = 0; j < stringLen; j += 2 )
		{
			pStream.WriteByte( ++modifier, ourEntries[i].ourText[j/2] );
			pStream.WriteByte( ++modifier, 0x00 );
		}
	}

	pStream.WriteLong( packetLen-4, 0x00000000 );
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
	pStream.ReserveSize( 9 );
	pStream.WriteByte( 0, 0x9E );
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

	pStream.WriteShort( 1, (UI16)packetLen );
	pStream.WriteLong( 3, vendorID.GetSerial() );
	pStream.WriteShort( 7, numItems );
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
	pStream.ReserveSize( newLen );
	pStream.WriteLong( packetLen, opItem->GetSerial() );
	pStream.WriteShort(  packetLen+4, opItem->GetID() );
	pStream.WriteShort(  packetLen+6, opItem->GetColour() );
	pStream.WriteShort(  packetLen+8, opItem->GetAmount() );
	UI32 value = calcValue( opItem, spItem->GetSellValue() );
	if( cwmWorldState->ServerData()->TradeSystemStatus() )
		value = calcGoodValue( tReg, spItem, value, true );
	pStream.WriteShort(  packetLen+10, value );
	pStream.WriteShort(  packetLen+12, stringLen );
	pStream.WriteString( packetLen+14, itemname, stringLen );
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
//Last Modified on Wednesday, 24-May-2000
//Bulletin Board Message (Variable # of bytes)
//  BYTE cmd
//  BYTE[2] len
//  BYTE subcmd
//  BYTE[ len - 4 ] submessage
//		Submessage 0 – Display Bulletin Board
//		BYTE[4] BoardID
//		BYTE[22] board name (default is “bulletin board”, the rest nulls)
//		BYTE[4] unknown
//		BYTE[4] zero (0)

void CPOpenMessageBoard::InternalReset( void )
{
	pStream.ReserveSize( 38 );
	pStream.WriteByte(   0, 0x71 );
	pStream.WriteByte(   2, 38 );
	pStream.WriteString( 8, "Bulletin Board", 15 );
}

void CPOpenMessageBoard::CopyData( CSocket *mSock )
{
	CItem *msgBoard = calcItemObjFromSer( mSock->GetDWord( 1 ) );

	if( ValidateObject( msgBoard ) )
	{
		pStream.WriteLong( 4, msgBoard->GetSerial() );
		// If the name the item (Bulletin Board) has been defined, display it
		// instead of the default "Bulletin Board" title.
		if( msgBoard->GetName() != "#" )
			pStream.WriteString( 8, msgBoard->GetName(), 21 );
	}
}

CPOpenMessageBoard::CPOpenMessageBoard( CSocket *mSock )
{
	InternalReset();
	CopyData( mSock );
}

//	Submessage 1 – Message Summary
//		BYTE[4] BoardID
//		BYTE[4] MessageID
//		BYTE[4] ParentID (0 if top level)
//		BYTE posterLen
//		BYTE[posterLen] poster (null terminated string)
//		BYTE subjectLen
//		BYTE[subjectLen] subject (null terminated string)
//		BYTE timeLen
//		BYTE[timeLen] time (null terminated string with time of posting) (“Day 1 @ 11:28”)

//	Submessage 2 – Message
//		BYTE[4] BoardID
//		BYTE[4] MessageID
//		BYTE posterLen
//		BYTE[posterLen] poster (null terminated string)
//		BYTE subjectLen
//		BYTE[subjectLen] subject (null terminated string)
//		BYTE timeLen
//		BYTE[timeLen] time (null terminated string with time of posting) (“Day 1 @ 11:28”)
//		BYTE[5] Unknown (01 90 03 F7 00)
//		BYTE numlines
//		For each line:
//			BYTE linelen
//			BYTE[linelen] body (null terminated)

void CPOpenMsgBoardPost::InternalReset( void )
{
	pStream.ReserveSize( 4 );
	pStream.WriteByte( 0, 0x71 );
	if( bFullPost )
		pStream.WriteByte( 3, 2 );
	else
		pStream.WriteByte( 3, 1 );
}

void CPOpenMsgBoardPost::CopyData( CSocket *mSock, const msgBoardPost_st& mbPost )
{
	size_t totSize = 7 + mbPost.DateLen + mbPost.PosterLen + mbPost.SubjectLen;

	std::vector< std::string >::const_iterator pIter;
	if( bFullPost )
	{
		totSize += 14;
		for( pIter = mbPost.msgBoardLine.begin(); pIter != mbPost.msgBoardLine.end(); ++pIter )
			totSize += (*pIter).size()+3;
	}
	else
		totSize += 12;

	pStream.ReserveSize( totSize );
	pStream.WriteShort( 1, static_cast<UI16>(totSize) );

	if( bFullPost )
		pStream.WriteLong( 4, mSock->GetDWord( 1 ) );
	else
		pStream.WriteLong( 4, mSock->GetDWord( 4 ) );

	pStream.WriteLong( 8, (mbPost.Serial + BASEITEMSERIAL) );

	size_t offset = 12;

	if( !bFullPost )
	{
		SERIAL pSerial = mbPost.ParentSerial;
		if( pSerial )
			pSerial += BASEITEMSERIAL;
		else
			pSerial += 0x80000000;
		pStream.WriteLong( offset, pSerial );
		offset += 4;
	}

	pStream.WriteByte( offset, mbPost.PosterLen );
	pStream.WriteString( ++offset, (char *)mbPost.Poster, mbPost.PosterLen );
	offset += mbPost.PosterLen;

	pStream.WriteByte( offset, mbPost.SubjectLen );
	pStream.WriteString( ++offset, (char *)mbPost.Subject, mbPost.SubjectLen );
	offset += mbPost.SubjectLen;

	pStream.WriteByte( offset, mbPost.DateLen );
	pStream.WriteString( ++offset, (char *)mbPost.Date, mbPost.DateLen );
	offset += mbPost.DateLen;

	if( bFullPost )
	{
		pStream.WriteShort( offset, 0x0190 );
		pStream.WriteShort( offset+=2, 0x03F7 );
		pStream.WriteByte(  offset+=2, 0x00 );

		pStream.WriteByte( ++offset, mbPost.Lines );
		for( pIter = mbPost.msgBoardLine.begin(); pIter != mbPost.msgBoardLine.end(); ++pIter )
		{
			pStream.WriteByte( ++offset, (*pIter).size()+2 );
			pStream.WriteString( ++offset, (*pIter), (*pIter).size() );
			offset += (*pIter).size();
			pStream.WriteByte( offset, 0x32 );
			pStream.WriteByte( ++offset, 0x00 );
		}
	}
}

CPOpenMsgBoardPost::CPOpenMsgBoardPost( CSocket *mSock, const msgBoardPost_st& mbPost, bool fullPost )
{
	bFullPost = fullPost;
	InternalReset();
	CopyData( mSock, mbPost );
}

void CPSendMsgBoardPosts::InternalReset( void )
{
	pStream.ReserveSize( 5 );
	pStream.WriteByte( 0, 0x3c );
	pStream.WriteShort( 1, 5 );
}

void CPSendMsgBoardPosts::CopyData( SERIAL mSerial, UI08 pToggle, SERIAL oSerial )
{
	size_t offset = pStream.GetSize();
	pStream.ReserveSize( offset+19 );

	pStream.WriteLong(  offset, (mSerial + BASEITEMSERIAL) );
	pStream.WriteShort( offset+4, 0x0EB0 );
	pStream.WriteByte(  offset+6, 0x00 );
	pStream.WriteShort( offset+7, 0x0001 );
	pStream.WriteShort( offset+9, 0x0000 );
	pStream.WriteShort( offset+11, 0x0000 );
	pStream.WriteLong(  offset+13, oSerial );
	pStream.WriteShort( offset+17, 0x0000 );

	++postCount;
}

void CPSendMsgBoardPosts::Finalize( void )
{
	pStream.WriteShort( 1, pStream.GetSize() );
	pStream.WriteShort( 3, postCount );
}

CPSendMsgBoardPosts::CPSendMsgBoardPosts()
{
	postCount = 0;
	InternalReset();
}

//0xBF Packet
//Subcommand: 0x19: Extended stats
//	BYTE[1] type // always 2 ? never seen other value
//	BYTE[4] serial
//	BYTE[1] unknown // always 0 ?
//	BYTE[1] lockBits // Bits: XXSS DDII (s=strength, d=dex, i=int), 0 = up, 1 = down, 2 = locked
CPExtendedStats::CPExtendedStats()
{
	InternalReset();
}

CPExtendedStats::CPExtendedStats( CChar& mChar )
{
	InternalReset();
	CopyData( mChar );
}

void CPExtendedStats::InternalReset( void )
{
	pStream.ReserveSize( 12 );
	pStream.WriteByte(  0, 0xBF );
	pStream.WriteShort( 1, 12 );
	pStream.WriteShort( 3, 0x19 );
	pStream.WriteByte(  6, 2 );
}

void CPExtendedStats::CopyData( CChar& mChar )
{
	pStream.WriteLong( 7, mChar.GetSerial() );

	UI08 strength		= mChar.GetSkillLock( STRENGTH );
	UI08 dexterity		= mChar.GetSkillLock( DEXTERITY );
	UI08 intelligence	= mChar.GetSkillLock( INTELLECT );

	pStream.WriteByte( 11, ( ( strength & 0x3 ) << 4 ) | ( ( dexterity & 0x3 ) << 2 ) | ( intelligence & 0x3 ) );
}

//0xBF Packet
//Subcommand 0x18: Enable map-diff (files)
//	BYTE[4] Number of maps
//	For each map
//		BYTE[4] Number of map patches in this map
//		BYTE[4] Number of static patches in this map 

void CPEnableMapDiffs::InternalReset( void )
{
	pStream.ReserveSize( 6 );
	pStream.WriteByte( 0, 0xBF );
	pStream.WriteShort( 1, 0x0006 );
	pStream.WriteShort( 3, 0x0018 );
}
CPEnableMapDiffs::CPEnableMapDiffs()
{
	InternalReset();
	CopyData();
}

void CPEnableMapDiffs::CopyData( void )
{
	UI08 mapCount	= Map->MapCount();
	size_t pSize	= ((mapCount+1)*8)+9;

	pStream.ReserveSize( pSize );
	pStream.WriteShort( 1, pSize );
	pStream.WriteLong( 5, mapCount+1 );

	for( UI08 i = 0; i < mapCount; ++i )
	{
		MapData_st &mMap = Map->GetMapData( i );
		pStream.WriteLong( 9+(i*8), mMap.mapDiffList.size() );
		pStream.WriteLong( 13+(i*8), mMap.staticsDiffIndex.size() );
	}
}

//0xD4 Packet
//Last Modified on Monday, 19'th-Aug-2002
//"new Book Header" (Variable# Bytes)
//    BYTE cmd
//    BYTE[2] length
//    BYTE[4] book ID
//    BYTE flag1
//    BYTE flag2
//    BYTE[2] number of pages
//    BYTE[2] length of author string (0 terminator included)
//    BYTE[?] author (ASCII, 0 terminated)
//    BYTE[2] length of title string (0 terminator included)
//    BYTE[?] title string
//
//Note1
//server and client packet.
//
//server side: opening book writeable: flag1+flag2 both 1
//(opening readonly book: flag1+flag2 0, unverified though)
//
//client side: flag1+flag2 both 0, number of pages 0.
//
//Note2
//
//Opening books goes like this:
//open book ->
//server sends 0xd4(title + author)
//server sends 0x66 with all book data "beyond" title + author
//
//if title + author changed: client side 0xd4 send
//
//if other book pages changed: 0x66 client side send. 

CPNewBookHeader::CPNewBookHeader()
{
	InternalReset();
}

void CPNewBookHeader::InternalReset( void )
{
	pStream.ReserveSize( 12 );
	pStream.WriteByte( 0, 0xD4 );
	pStream.WriteShort( 1, 12 );
}

void CPNewBookHeader::Serial( UI32 bookSer )
{
	pStream.WriteLong( 3, bookSer );
}

void CPNewBookHeader::Flag1( UI08 toSet )
{
	pStream.WriteByte( 7, toSet );
}

void CPNewBookHeader::Flag2( UI08 toSet )
{
	pStream.WriteByte( 8, toSet );
}

void CPNewBookHeader::Pages( UI16 numPages )
{
	pStream.WriteShort( 9, numPages );
}

void CPNewBookHeader::Author( const std::string newAuthor )
{
	author = newAuthor;
}

void CPNewBookHeader::Title( const std::string newTitle )
{
	title = newTitle;
}

void CPNewBookHeader::Finalize( void )
{
	const UI16 authorLen	= static_cast<UI16>(author.size());
	const UI16 titleLen		= static_cast<UI16>(title.size());
	const UI16 totalLen		= static_cast<UI16>(15+authorLen+titleLen+2);
	size_t offset			= 11;

	pStream.ReserveSize( totalLen );
	pStream.WriteShort( 1, totalLen );

	pStream.WriteShort( offset, titleLen+1 );
	pStream.WriteString( offset+=2, title, titleLen );
	pStream.WriteByte( offset+=titleLen, 0x00 );

	pStream.WriteShort( offset+=1, authorLen+1 );
	pStream.WriteString( offset+=2, author, authorLen );
	pStream.WriteByte( offset+=authorLen, 0x00 );
}

//0xBF Packet
//Subcommand 0x14: Display Popup menu
//	BYTE[2] Unknown(always 00 01)
//	BYTE[4] Serial
//	BYTE Number of entries in the popup
//	For each Entry
//		BYTE[2] Entry Tag (this will be returned by the client on selection)
//		BYTE[2] Text ID ID is the file number for intloc#.language e.g intloc6.enu and the index into that
//		BYTE[2] Flags 0x01 = locked, 0x02 = arrow, 0x20 = color
//		If (Flags &0x20)
//			BYTE[2] color	// rgb 1555 color (ex, 0 = transparent, 0x8000 = solid black, 0x1F = blue, 0x3E0 = green, 0x7C00 = red)
CPPopupMenu::CPPopupMenu( void )
{
	InternalReset();
}

CPPopupMenu::CPPopupMenu( CChar& toCopy )
{
	InternalReset();
	CopyData( toCopy );
}

void CPPopupMenu::InternalReset( void )
{
	pStream.ReserveSize( 5 );
	pStream.WriteByte( 0, 0xBF );
	pStream.WriteShort( 1, 5 );
	pStream.WriteShort( 3, 0x14 );
	pStream.WriteShort( 5, 0x0001 );
}

void CPPopupMenu::CopyData( CChar& toCopy )
{
	UI16 packetLen = (12 + (4 * 8));
	pStream.ReserveSize( packetLen );
	pStream.WriteShort( 1, packetLen );

	pStream.WriteLong( 7, toCopy.GetSerial() );
	pStream.WriteByte( 11, 4 );
	size_t offset = 12;

	pStream.WriteShort( offset, 0x000A );	// Open Paperdoll
	pStream.WriteShort( offset+=2, 6123 );
	if( toCopy.isHuman() )
	{
		pStream.WriteShort( offset+=2, 0x0020 );
		pStream.WriteShort( offset+=2, 0x03E0 );
	}
	else
	{
		pStream.WriteShort( offset+=2, 0x0021 );
		pStream.WriteShort( offset+=2, 0xFFFF );
	}

	pStream.WriteShort( offset+=2, 0x000B );	// Open Backpack
	pStream.WriteShort( offset+=2, 6145 );
	if( ValidateObject( toCopy.GetPackItem() ) )
	{
		pStream.WriteShort( offset+=2, 0x0020 );
		pStream.WriteShort( offset+=2, 0x03E0 );
	}
	else
	{
		pStream.WriteShort( offset+=2, 0x0021 );
		pStream.WriteShort( offset+=2, 0xFFFF );
	}

	pStream.WriteShort( offset+=2, 0x000C );	// Shopkeep
	pStream.WriteShort( offset+=2, 6103 );
	if( toCopy.IsShop() )
	{
		pStream.WriteShort( offset+=2, 0x0020 );
		pStream.WriteShort( offset+=2, 0x03E0 );
	}
	else
	{
		pStream.WriteShort( offset+=2, 0x0021 );
		pStream.WriteShort( offset+=2, 0xFFFF );
	}

	pStream.WriteShort( offset+=2, 0x000D );
	pStream.WriteShort( offset+=2, 6104 );
	if( toCopy.IsShop() )
	{
		pStream.WriteShort( offset+=2, 0x0020 );
		pStream.WriteShort( offset+=2, 0x03E0 );
	}
	else
	{
		pStream.WriteShort( offset+=2, 0x0021 );
		pStream.WriteShort( offset+=2, 0xFFFF );
	}
}

//0xC1 Packet
//Last Modified on Monday, 4'th-August-20002
//Predefined Message (localized Message) (Variable # of bytes )
//	BYTE cmd
//	BYTE[2] length
//	BYTE[4] id (0xffff for system message)
//	BYTE[2] body (0xff for system message)
//	BYTE type (6 - lower left, 7 on player)
//	BYTE[2] hue
//	BYTE[2] font
//	BYTE[4] Message number
//	BYTE[30] - speaker's name
//	BYTE[?*2] - arguments // _little-endian_ unicode string, tabs ('\t') seperate the arguments
//
//Argument example:
//take number 1042762:
//"Only ~1_AMOUNT~ gold could be deposited. A check for ~2_CHECK_AMOUNT~ gold was returned to you."
//the arguments string may have "100 thousand\t25 hundred", which in turn would modify the string:
//"Only 100 thousand gold could be deposited. A check for 25 hundred gold was returned to you."

CPClilocMessage::CPClilocMessage( void )
{
	InternalReset();
	Serial( INVALIDSERIAL );
	Body( INVALIDID );
	Name( "System" );
}

CPClilocMessage::CPClilocMessage( CBaseObject& toCopy )
{
	InternalReset();
	CopyData( toCopy );
}

void CPClilocMessage::InternalReset( void )
{
	pStream.ReserveSize( 48 );
	pStream.WriteByte( 0, 0xC1 );
	pStream.WriteShort( 1, 48 );
}

void CPClilocMessage::CopyData( CBaseObject& toCopy )
{
	Serial( toCopy.GetSerial() );
	Body( toCopy.GetID() );
	Name( toCopy.GetName() );
}

void CPClilocMessage::Serial( SERIAL toSet )
{
	pStream.WriteLong( 3, toSet );
}

void CPClilocMessage::Body( UI16 toSet )
{
	pStream.WriteShort( 7, toSet );
}

void CPClilocMessage::Type( UI08 toSet )
{
	pStream.WriteByte( 9, toSet );
}

void CPClilocMessage::Hue( UI16 hueColor )
{
	pStream.WriteShort( 10, hueColor );
}

void CPClilocMessage::Font( UI16 fontType )
{
	pStream.WriteShort( 12, fontType );
}

void CPClilocMessage::Message( UI32 messageNum )
{
	pStream.WriteLong( 14, messageNum );
}

void CPClilocMessage::Name( const std::string name )
{
	if( name.size() > 29 )
	{
		pStream.WriteString( 18, name, 29 );
		pStream.WriteByte( 47, 0x00 );
	}
	else
		pStream.WriteString( 18, name, 30 );
}

void CPClilocMessage::ArgumentString( const std::string arguments )
{
	const size_t stringLen = arguments.size();
	const UI16 packetLen = static_cast<UI16>(pStream.GetShort( 1 ) + (stringLen * 2) + 2);
	pStream.ReserveSize( packetLen );
	pStream.WriteShort( 1, packetLen );

	for( size_t i = 0; i < stringLen; ++i )
		pStream.WriteByte( 48 + i * 2, arguments[i] );
}

}