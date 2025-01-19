#include "uox3.h"
#include "CPacketSend.h"
#include "speech.h"
#include "combat.h"
#include "cMagic.h"
#include "power.h"
#include "msgboard.h"
#include "mapstuff.h"
#include "PartySystem.h"
#include "cGuild.h"
#include "townregion.h"
#include "classes.h"
#include "Dictionary.h"
#include "cScript.h"
#include "CJSMapping.h"
#include "cRaces.h"
#include <string>
#include <locale>
#include <codecvt>
#include "osunique.hpp"
#if defined(_WIN32)
#include <ws2tcpip.h>
#endif

using namespace std::string_literals;
// Unknown bytes
// 5->8
// 18->27
// 30->36

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPCharLocBody()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet that confirms client login
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x1B (Login Confirm)
//|					Size: 37 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[4] player id
//|						BYTE[4] unknown1
//|						BYTE[2] bodyType
//|						BYTE[2] xLoc
//|						BYTE[2] yLoc
//|						BYTE[2] zLoc
//|						BYTE direction
//|						BYTE[2] unknown2
//|						BYTE[4] unknown3 (usually has FF somewhere in it)
//|						BYTE[4] unknown4
//|						BYTE flag byte
//|						BYTE highlight color
//|						BYTE[7] unknown5
//|
//|					Newer, more up to date version of the packet. When did it change?
//|						BYTE cmd
//|						BYTE[4] player id
//|						BYTE[4] unknown1
//|						BYTE[2] bodyType
//|						BYTE[2] xLoc
//|						BYTE[2] yLoc
//|						BYTE Unknown (0x0)
//|						BYTE zLoc
//|						BYTE direction
//|						BYTE[4] unknown2 (0x0, Described as X in Jerrith's old docs)
//|						BYTE[4] unknown3 (0x0, Described as Y in Jerrith's old docs)
//|						BYTE unknown4 (0x0)
//|						BYTE[2] Map Width/Server boundary width (minus eight according to POL)
//|						BYTE[2] Map Height/Server boundary height
//|						BYTE[2] unknown5 (0x0)
//|						BYTE[4] unknown6 (0x0)
//|
//|						Note: Only send once after login. It’s mandatory to send it once.
//o------------------------------------------------------------------------------------------------o
void CPCharLocBody::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[SEND]Packet     : CPCharLocBody 0x1B --> Length: " << pStream.GetSize() << TimeStamp() << std::endl;
	}
	outStream << "PlayerID         : " << std::hex << "0x" << pStream.GetULong( 1 ) << std::endl;
	outStream << "Unknown1         : " << std::dec << pStream.GetULong( 5 ) << std::endl;
	outStream << "BodyType         : " << std::hex << "0x" << pStream.GetUShort( 9 ) << std::endl;
	outStream << "X Loc            : " << std::dec << pStream.GetUShort( 11 ) << std::endl;
	outStream << "Y Loc            : " << pStream.GetUShort( 13 ) << std::endl;
	outStream << "Z Loc            : " << static_cast<UI16>( pStream.GetByte( 16 )) << std::endl;
	outStream << "Direction        : " << static_cast<UI16>( pStream.GetByte( 17 )) << std::endl;
	outStream << "Unknown2         : " << pStream.GetUShort( 18 ) << std::endl;
	outStream << "Unknown3         : " << pStream.GetULong( 20 ) << std::endl;
	outStream << "Unknown4         : " << pStream.GetULong( 24 ) << std::endl;
	outStream << "Map Width        : " << pStream.GetUShort( 27 ) << std::endl;
	outStream << "Map Height       : " << pStream.GetUShort( 29 ) << std::endl;
	outStream << "Flag Byte        : " << static_cast<UI16>( pStream.GetByte( 31 )) << std::endl;
	outStream << "Highlight Colour : " << static_cast<UI16>( pStream.GetByte( 32 )) << std::endl;
	outStream << "  Raw dump     :" << std::endl;
	CPUOXBuffer::Log( outStream, false );
}

void CPCharLocBody::InternalReset( void )
{
	pStream.ReserveSize( 37 );
	pStream.WriteByte( 0, 0x1B );

	for( UI08 k = 5; k < 9; ++k )
	{
		pStream.WriteByte( k, 0x00 );
	}
	for( UI08 j = 33; j < 37; ++j )
	{
		pStream.WriteByte( j, 0x00 );
	}

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
	pStream.WriteShort( 9, toCopy.GetId() );
	pStream.WriteShort( 11, toCopy.GetX() );
	pStream.WriteShort( 13, toCopy.GetY() );
	pStream.WriteShort( 15, toCopy.GetZ() );
	pStream.WriteByte(  17, toCopy.GetDir() );
	pStream.WriteByte( 18, 0x0 );
	pStream.WriteShort( 19, 0xFFFF );
	pStream.WriteShort( 21, 0xFFFF );
	pStream.WriteLong( 23, 0x0000 );
	auto [width, height] = Map->SizeOfMap( toCopy.WorldNumber() );
	auto mapWidth = static_cast<UI16>( width );
	auto mapHeight = static_cast<UI16>( height );
	pStream.WriteShort( 27, mapWidth );
	pStream.WriteShort( 29, mapHeight );
}

CPCharLocBody &CPCharLocBody::operator = ( CChar &toCopy )
{
	CopyData( toCopy );
	return ( *this );
}

void CPCharLocBody::Flag( UI08 toPut )
{
	pStream.WriteByte( 31, toPut );
}
void CPCharLocBody::HighlightColour( UI08 color )
{
	pStream.WriteByte( 32, color );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPacketSpeech()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet containing speech
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x1C (Send Speech)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] blockSize
//|						BYTE[4] itemId (FF FF FF FF = system)
//|						BYTE[2] model (item hex # - FF FF = system)
//|						BYTE Type
//|							0x00 - Normal
//|							0x01 - Broadcast/System
//|							0x02 - Emote
//|							0x06 - System/Lower Corner
//|							0x07 - Message/Corner With Name
//|							0x08 - Whisper
//|							0x09 - Yell
//|							0x0A - Spell
//|							0x0D - Guild Chat
//|							0x0E - Alliance Chat
//|							0x0F - Command Prompts
//|						BYTE[2] Text Color
//|						BYTE[2] Font
//|						BYTE[30] Name
//|						BYTE[?] Null-Terminated Message (? = blockSize - 44)
//o------------------------------------------------------------------------------------------------o
void CPacketSpeech::InternalReset( void )
{
	pStream.ReserveSize( 44 );
	pStream.WriteByte( 0, 0x1C );
}
CPacketSpeech::CPacketSpeech( CSpeechEntry &toCopy )
{
	InternalReset();
	CopyData( toCopy );
}

CPacketSpeech::CPacketSpeech( CPITalkRequest &toCopy )
{
	InternalReset();
	CopyData( toCopy );
}

void CPacketSpeech::CopyData( CPITalkRequest &toCopy )
{
	Colour( toCopy.TextColour() );
	Font( static_cast<FontType>( toCopy.Font() ));
	Type( toCopy.Type() );
	Speech( toCopy.Text() );
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
	switch( toCopy.SpkrType() )
	{
		case SPK_UNKNOWN:
		case SPK_SYSTEM:
			SpeakerName( "System" );
			SpeakerModel( INVALIDID );
			if( toCopy.Colour() == 0 )
			{
				Colour( cwmWorldState->ServerData()->SysMsgColour() ); // 0x0048 by default
			}
			break;
		case SPK_CHARACTER:
			CChar *ourChar;
			ourChar = CalcCharObjFromSer( toCopy.Speaker() );
			if( ValidateObject( ourChar ))
			{
				std::string speakerName = GetNpcDictName( ourChar, nullptr, NRS_SPEECH );
				SpeakerName( speakerName );
				SpeakerModel( ourChar->GetId() );
			}
			else
			{
				SpeakerModel( INVALIDID );
			}
			break;
		case SPK_ITEM:
			CItem *ourItem;
			ourItem = CalcItemObjFromSer( toCopy.Speaker() );
			if( ValidateObject( ourItem ))
			{
				SpeakerName( ourItem->GetName() );
				SpeakerModel( ourItem->GetId() );
			}
			else
			{
				SpeakerModel( INVALIDID );
			}
			break;
		case SPK_NULL:

			break;
	}
	if( toCopy.SpeakerName().length() != 0 )
	{
		SpeakerName( toCopy.SpeakerName() );
	}
}

void CPacketSpeech::GhostIt( [[maybe_unused]] UI08 method )
{
	// Method ignored currently
	// Designed with the idea that you can garble text in more than one way
	// eg 0 == ghost, 1 == racial, 2 == magical, etc etc
	for( UI16 j = 44; j < pStream.GetSize() - 1; ++j )
	{
		if( pStream.GetByte( j ) != 32 )
		{
			pStream.WriteByte( j, ( RandomNum( 0, 1 ) == 0 ? 'O' : 'o' ));
		}
	}
}

CPacketSpeech &CPacketSpeech::operator = ( CSpeechEntry &toCopy )
{
	CopyData( toCopy );
	return ( *this );
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

void CPacketSpeech::SpeakerName( const std::string& toPut )
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
void CPacketSpeech::Language( [[maybe_unused]] UnicodeTypes toPut )
{

}
void CPacketSpeech::Unicode( bool toPut )
{
	isUnicode = toPut;
}
void CPacketSpeech::Type( SpeechType toPut )
{
	pStream.WriteByte( 9, static_cast<UI08>( toPut ));
}
void CPacketSpeech::Speech( const std::string& toPut )
{
	size_t len			= toPut.length();
	const size_t newLen	= 44 + len + 1;

	pStream.ReserveSize( newLen );
	pStream.WriteString( 44, toPut, toPut.length() );
	pStream.WriteShort( 1, static_cast<UI16>( newLen ));
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPWalkDeny()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet with movement rejection details
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x21 (Character Move Reject)
//|					Size: 8 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE sequence #
//|						BYTE[2] xLoc
//|						BYTE[2] yLoc
//|						BYTE direction
//|						BYTE zLoc
//o------------------------------------------------------------------------------------------------o
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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPWalkOK()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet with movement acknowledgement
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x22 (Character Move ACK/ Resync Request)
//|					Size: 3 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE sequence (matches sent sequence)
//|						BYTE notoriety (updates the notoriety of the char)
//o------------------------------------------------------------------------------------------------o
CPWalkOK::CPWalkOK()
{
	pStream.ReserveSize( 3 );
	pStream.WriteByte( 0, 0x22 );
}
void CPWalkOK::SequenceNumber( char newValue )
{
	pStream.WriteByte( 1, newValue );
}
void CPWalkOK::FlagColour( UI08 newValue )
{
	pStream.WriteByte( 2, newValue );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPExtMove()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet containing updated player details
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x77 (Update Player)
//|					Size: 17 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[4] player id
//|						BYTE[2] model
//|						BYTE[2] xLoc
//|						BYTE[2] yLoc
//|						BYTE zLoc
//|						BYTE direction
//|						BYTE[2] hue/skin color
//|						BYTE flag (bit field)
//|						BYTE highlight color
//o------------------------------------------------------------------------------------------------o
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
	SetFlags( toCopy );
}

CPExtMove &CPExtMove::operator = ( CChar &toCopy )
{
	CopyData( toCopy );
	return ( *this );
}

void CPExtMove::FlagColour( UI08 newValue )
{
	pStream.WriteByte( 16, newValue );
}

void CPExtMove::SetFlags( CChar &toCopy )
{
	std::bitset< 8 > flag( 0 );

	if( cwmWorldState->ServerData()->ClientSupport7000() || cwmWorldState->ServerData()->ClientSupport7090() ||
	   cwmWorldState->ServerData()->ClientSupport70160() || cwmWorldState->ServerData()->ClientSupport70240() ||
	   cwmWorldState->ServerData()->ClientSupport70300() || cwmWorldState->ServerData()->ClientSupport70331() ||
	   cwmWorldState->ServerData()->ClientSupport704565() || cwmWorldState->ServerData()->ClientSupport70610() )
	{
		// Clients 7.0.0.0 and later
		const UI08 BIT__FROZEN = 0;	//	0x01, frozen/paralyzed
		const UI08 BIT__FEMALE = 1;	//	0x02, female flag
		const UI08 BIT__FLYING = 2;	//	0x04, flying (post 7.0.0.0)
		const UI08 BIT__GOLDEN = 3;	//	0x08, yellow healthbar
		//const UI08 BIT__IGNOREMOBILES = 4;	// 0x10, ignore other mobiles?

		flag.set( BIT__FROZEN, toCopy.IsFrozen() );
		flag.set( BIT__FEMALE, ( toCopy.GetId() == 0x0191 || toCopy.GetId() == 0x025E || toCopy.GetId() == 0x029B || toCopy.GetId() == 0xb8 || toCopy.GetId() == 0xba ));
		flag.set( BIT__FLYING, ( toCopy.IsFlying() ));
		flag.set( BIT__GOLDEN, ( toCopy.IsInvulnerable() ));
	}
	else
	{
		// Clients earlier than 7.0.0.0
		const UI08 BIT__INVUL = 0;	//	0x01
		const UI08 BIT__DEAD = 1;	//	0x02
		const UI08 BIT__POISON = 2;	//	0x04, poison
		const UI08 BIT__GOLDEN	= 3;	//	0x08, yellow healthbar
		//const UI08 BIT__IGNOREMOBILES = 4;	// 0x10, ignore other mobiles?

		flag.set( BIT__INVUL, toCopy.IsInvulnerable() );
		flag.set( BIT__DEAD, toCopy.IsDead() );
		flag.set( BIT__POISON, ( toCopy.GetPoisoned() != 0 ));
		flag.set( BIT__GOLDEN, ( toCopy.IsInvulnerable() ));
	}

	const UI08 BIT__ATWAR = 6;	// 0x40
	const UI08 BIT__DEAD = 7;	// 0x80, dead or hidden
	flag.set( BIT__ATWAR, toCopy.IsAtWar() );
	flag.set( BIT__DEAD, ( toCopy.IsDead() || toCopy.GetVisible() != VT_VISIBLE ));

	pStream.WriteByte( 15, static_cast<UI08>( flag.to_ulong() ));
}

void CPExtMove::CopyData( CChar &toCopy )
{
	pStream.WriteLong(  1, toCopy.GetSerial() );
	pStream.WriteShort( 5, toCopy.GetId() );
	pStream.WriteShort( 7, toCopy.GetX() );
	pStream.WriteShort( 9, toCopy.GetY() );
	pStream.WriteByte( 11, toCopy.GetZ() );

	UI08 dir = toCopy.GetDir();
	if( toCopy.GetRunning() )
	{
		dir |= 0x80;
	}
	pStream.WriteByte( 12, dir );
	pStream.WriteShort( 13, toCopy.GetSkin() );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPAttackOK()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet informing client if it's ok or not to attack target
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xAA (OK / Not OK To Attack)
//|					Size: 5 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[4] CharID being attacked
//|							ID is set to 00 00 00 00 when attack is refused.
//o------------------------------------------------------------------------------------------------o
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
CPAttackOK &CPAttackOK::operator = ( CChar &toCopy )
{
	CopyData( toCopy );
	return ( *this );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPRemoveItem()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet informing client of the removal of an object
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x1D (Delete object)
//|					Size: 5 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[4] item/char id
//o------------------------------------------------------------------------------------------------o
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
CPRemoveItem &CPRemoveItem::operator = ( CBaseObject &toCopy )
{
	CopyData( toCopy );
	return ( *this );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPWorldChange()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet telling client to update season visuals of world
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xBC (Seasonal Information)
//|					Size: 3 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE id1
//|						BYTE id2
//|
//|					Note: Server message
//|					Note: if id2 = 1, then this is a season change. Although, this could also just
//|							be a flag to play a sound FX on season change or not...
//|					Note: if season change, then id1 = (0=spring, 1=summer, 2=fall, 3=winter, 4 = desolation)
//o------------------------------------------------------------------------------------------------o
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
	pStream.WriteByte( 1, static_cast<UI08>( newSeason ));
}
void CPWorldChange::Cursor( UI08 newCursor )
{
	pStream.WriteByte( 2, newCursor );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPLightLevel()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet updating client with current overall light level
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x4F (Overall Light Level)
//|					Size: 2 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE level
//|							0x00 - day
//|							0x09 - OSI night
//|							0x1F - Black
//|							Max normal val = 0x1F
//o------------------------------------------------------------------------------------------------o
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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPUpdIndSkill()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet updating client with skill values and lock states
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x3A (Send Skills)
//|					Size: 13 bytes
//|
//|					Packet Build
//|						BYTE Command
//|						BYTE[2] Length
//|						BYTE Type
//|						Repeat next for each skill
//|							BYTE[2] Skill ID Number
//|							BYTE[2] Skill Value * 10
//|							BYTE[2] Unmodified Value * 10
//|							BYTE Skill Lock
//|						If (Type==2 || Type==0xDF)
//|							BYTE[2] Skill Cap
//|						BYTE[2] Null Terminated(0000) (ONLY IF TYPE == 0x00)
//o------------------------------------------------------------------------------------------------o
void CPUpdIndSkill::InternalReset( void )
{
	pStream.ReserveSize( 13 );
	pStream.WriteByte(  0, 0x3A );
	pStream.WriteShort( 1, 0x000D ); // Length of message
	pStream.WriteByte(  3, 0xDF); // delta, capped
}
void CPUpdIndSkill::CopyData( CChar& i, UI08 sNum )
{
	SkillNum( sNum );
	Skill( i.GetSkill( sNum ));
	BaseSkill( i.GetBaseSkill( sNum ));
	Lock( static_cast<SkillLock>( i.GetSkillLock( sNum )));
	Cap( static_cast<UI16>( cwmWorldState->ServerData()->ServerSkillCapStatus() ));
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
	pStream.WriteShort( 4, sNum );
}
void CPUpdIndSkill::Skill( SI16 skillval )
{
	pStream.WriteShort( 6, skillval );
}
void CPUpdIndSkill::BaseSkill( SI16 skillval )
{
	pStream.WriteShort( 8, skillval );
}
void CPUpdIndSkill::Lock( SkillLock lockVal )
{
	pStream.WriteByte( 10, lockVal );
}
void CPUpdIndSkill::Cap( SI16 capVal )
{
	pStream.WriteShort( 11, capVal );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPBuyItem()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet with list of items bought from vendor
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x3B (Buy Item(s))
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] blockSize
//|						BYTE[4] vendorId
//|						BYTE flag
//|							0x00 - no items following
//|							0x02 - items following
//|						For each item
//|							BYTE (0x1A)
//|							BYTE[4] itemId (from 3C packet)
//|							BYTE[2] # bought
//o------------------------------------------------------------------------------------------------o
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
CPBuyItem &CPBuyItem::operator = ( CBaseObject &toCopy )
{
	CopyData( toCopy );
	return ( *this );
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

//const UI32 loopbackIP = (127<<24) + 1;
//o------------------------------------------------------------------------------------------------o
//| Function	-	CPRelay()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet relaying details about game server to client
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x8C (Connect to Game Server)
//|					Size: 11 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[4] gameServer IP
//|						BYTE[2] gameServer port
//|						BYTE[4] new key
//o------------------------------------------------------------------------------------------------o
CPRelay::CPRelay()
{
	InternalReset();
}
CPRelay::CPRelay( UI32 newIP )
{
	InternalReset();
	ServerIP( newIP );
}
CPRelay::CPRelay( UI32 newIP, UI16 newPort )
{
	InternalReset();
	ServerIP( newIP );
	Port( newPort );
}
void CPRelay::ServerIP( UI32 newIP )
{
	pStream.WriteLong( 1, newIP );
}
void CPRelay::Port( UI16 newPort )
{
	pStream.WriteShort( 5, newPort );
}
void CPRelay::SeedIP( UI32 newIP )
{
	pStream.WriteLong( 7, newIP );
}
void CPRelay::InternalReset( void )
{
	pStream.ReserveSize( 11 );
	pStream.WriteByte( 0, 0x8C );
	SeedIP( 0xFFFFFFFF );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPLogoutResponse()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet with response to client's logout request
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xD1 (Logout Status)
//|					Size: 2 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE 0x01
//|
//|					Notes
//|						Client + Server packet
//|						Client will send this packet without 0x01 Byte when the server sends FLAG & 0x02 in the 0xA9 Packet during logon.
//|						Server responds with same packet, plus the 0x01 Byte, allowing client to finish logging out.
//o------------------------------------------------------------------------------------------------o
CPLogoutResponse::CPLogoutResponse( [[maybe_unused]] UI08 extraByte )
{
	InternalReset();
}
void CPLogoutResponse::InternalReset( void )
{
	pStream.ReserveSize( 2 );
	pStream.WriteByte( 0, 0xD1 );
	pStream.WriteByte( 1, 0x01 );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPWornItem()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to equip a single item for player and update paperdoll
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x2E (Worn Item)
//|					Size: 15 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[4] item serial
//|						BYTE[2] model (item hex #)
//|						BYTE (0x00)
//|						BYTE layer
//|						BYTE[4] playerId
//|						BYTE[2] color/hue
//o------------------------------------------------------------------------------------------------o
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
	Model( toCopy.GetId() );
	Layer( toCopy.GetLayer() );
	CharSerial( toCopy.GetContSerial() );
}
CPWornItem &CPWornItem::operator = ( CItem &toCopy )
{
	CopyData( toCopy );
	return ( *this );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPCharacterAnimation()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to play specific animation for character
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x6E (Character Animation)
//|					Size: 14 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[4] item/char ID
//|						BYTE[2] movement model
//|							0x00 = walk
//|							0x01 = walk faster
//|							0x02 = run
//|							0x03 = run (faster?)
//|							0x04 = nothing
//|							0x05 = shift shoulders
//|							0x06 = hands on hips
//|							0x07 = attack stance (short)
//|							0x08 = attack stance (longer)
//|							0x09 = swing (attack with knife)
//|							0x0a = stab (underhanded)
//|							0x0b = swing (attack overhand with sword)
//|							0x0c = swing (attack with sword over and side)
//|							0x0d = swing (attack with sword side)
//|							0x0e = stab with point of sword
//|							0x0f = ready stance
//|							0x10 = magic (butter churn!)
//|							0x11 = hands over head (balerina)
//|							0x12 = bow shot
//|							0x13 = crossbow
//|							0x14 = get hit
//|							0x15 = fall down and die (backwards)
//|							0x16 = fall down and die (forwards)
//|							0x17 = ride horse (long)
//|							0x18 = ride horse (medium)
//|							0x19 = ride horse (short)
//|							0x1a = swing sword from horse
//|							0x1b = normal bow shot on horse
//|							0x1c = crossbow shot
//|							0x1d = block #2 on horse with shield
//|							0x1e = block on ground with shield
//|							0x1f = swing, and get hit in middle
//|							0x20 = bow (deep)
//|							0x21 = salute
//|							0x22 = scratch head
//|							0x23 = 1 foot forward for 2 secs
//|							0x24 = same
//|						BYTE[1] unknown1 (0x00)
//|						BYTE[1] Frame Count
//|						BYTE[2] repeat (1 = once / 2 = twice / 0 = repeat forever)
//|						BYTE forward/backwards(0x00=forward, 0x01=backwards)
//|						BYTE repeat Flag (0 - Don't repeat / 1 repeat)
//|						BYTE frame Delay (0x00 - fastest / 0xFF - Too slow to watch)
//o------------------------------------------------------------------------------------------------o
void CPCharacterAnimation::CopyData( CChar &toCopy )
{
	Serial( toCopy.GetSerial() );
	//Direction( toCopy.GetDir() );
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
void CPCharacterAnimation::FrameCount( UI08 frameCount )
{
	pStream.WriteByte( 8, frameCount );
}
void CPCharacterAnimation::Repeat( SI16 repeatValue )
{
	pStream.WriteShort( 9, repeatValue );
}
void CPCharacterAnimation::DoBackwards( bool newValue )
{
	pStream.WriteByte( 11, static_cast<UI08>(( newValue ? 1 : 0 )));
}
void CPCharacterAnimation::RepeatFlag( bool newValue )
{
	pStream.WriteByte( 12, static_cast<UI08>(( newValue ? 1 : 0 )));
}
void CPCharacterAnimation::FrameDelay( UI08 delay )
{
	pStream.WriteByte( 13, delay );
}
CPCharacterAnimation &CPCharacterAnimation::operator = ( CChar &toCopy )
{
	CopyData( toCopy );
	return ( *this );
}
void CPCharacterAnimation::InternalReset( void )
{
	pStream.ReserveSize( 14 );
	pStream.WriteByte( 0, 0x6E );
	pStream.WriteByte( 7, 0x00 );
	pStream.WriteByte( 8, 0x07 ); // Animation Length/Frame Count, default to 7
	Repeat( 1 );
	DoBackwards( false );
	RepeatFlag( false );
	FrameDelay( 0 );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPNewCharacterAnimation()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to play specific animation for character in clients 7.0.0.0+
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xE2 (New Character Animation)
//|					Size: 10 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[4] item/char serial
//|						BYTE[2] action type
//|						BYTE[2] sub action
//|						BYTE sub sub action/variation/delay?
//o------------------------------------------------------------------------------------------------o
void CPNewCharacterAnimation::CopyData( CChar &toCopy )
{
	Serial( toCopy.GetSerial() );
}
CPNewCharacterAnimation::CPNewCharacterAnimation()
{
	InternalReset();
}
CPNewCharacterAnimation::CPNewCharacterAnimation( CChar &toCopy )
{
	InternalReset();
	CopyData( toCopy );
}
void CPNewCharacterAnimation::Serial( SERIAL toSet )
{
	pStream.WriteLong( 1, toSet );
}
void CPNewCharacterAnimation::Action( UI16 action )
{
	pStream.WriteShort( 5, action );
}
void CPNewCharacterAnimation::SubAction( UI16 subAction )
{
	pStream.WriteShort( 7, subAction );
}
void CPNewCharacterAnimation::SubSubAction( UI08 subSubAction )
{
	pStream.WriteByte( 9, subSubAction );
}
CPNewCharacterAnimation &CPNewCharacterAnimation::operator = ( CChar &toCopy )
{
	CopyData( toCopy );
	return ( *this );
}
void CPNewCharacterAnimation::InternalReset( void )
{
	pStream.ReserveSize( 10 );
	pStream.WriteByte( 0, 0xE2 );
	pStream.WriteByte( 7, 0 );
	pStream.WriteByte( 9, 0 );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPDrawGamePlayer()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to update player's looks, flags and location
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x20 (Draw Game Player/Mobile Update/Teleport)
//|					Size: 19 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[4] creature id
//|						BYTE[2] bodyType
//|						BYTE unknown1 (0)
//|						BYTE[2] skin color / hue
//|						BYTE flag byte
//|						BYTE[2] xLoc
//|						BYTE[2] yLoc
//|						BYTE[2] unknown2 (0)
//|						BYTE direction
//|						BYTE zLoc
//|
//|						Note: Only used with the character being played by the client.
//|						Ancient client version Flags:
//|							0x01 = Unknown
//|							0x02 = CanAlterPaperdoll
//|							0x04 = Posioned
//|							0x08 = Yellow HealthBar
//|							0x10 = Unknown
//|							0x20 = Unknown
//|							0x40 = War Mode
//|							0x80 = Hidden
//|						Pre 7.0.0.0 Flags:
//|							0x01 = Invulnerable
//|							0x02 = Dead
//|							0x04 = Posioned
//|							0x08 = Yellow HealthBar
//|							0x10 = Ignore Mobiles
//|							0x40 = War Mode
//|							0x80 = Hidden )
//|						Post 7.0.0.0 Flags:
//|							0x01 = Frozen
//|							0x02 = Female
//|							0x04 = Flying
//|							0x08 = Yellow HealthBar
//|							0x10 = Ignore Mobiles
//|							0x20 = Movable if normally not
//|							0x40 = War Mode
//|							0x80 = Hidden )
//o------------------------------------------------------------------------------------------------o
CPDrawGamePlayer::CPDrawGamePlayer( CChar &toCopy )
{
	InternalReset();
	CopyData( toCopy );
}

void CPDrawGamePlayer::CopyData( CChar &toCopy )
{
	pStream.WriteLong(	1, toCopy.GetSerial() );
	pStream.WriteShort( 5, toCopy.GetId() );
	pStream.WriteShort( 8, toCopy.GetColour() );
	pStream.WriteShort( 11, toCopy.GetX() );
	pStream.WriteShort( 13, toCopy.GetY() );
	pStream.WriteByte(  17, toCopy.GetDir() );
	pStream.WriteByte(  18, toCopy.GetZ() );

	std::bitset<8> flag( 0 );

	if( cwmWorldState->ServerData()->ClientSupport7000() || cwmWorldState->ServerData()->ClientSupport7090() ||
	   cwmWorldState->ServerData()->ClientSupport70160() || cwmWorldState->ServerData()->ClientSupport70240() ||
	   cwmWorldState->ServerData()->ClientSupport70300() || cwmWorldState->ServerData()->ClientSupport70331() ||
	   cwmWorldState->ServerData()->ClientSupport704565() || cwmWorldState->ServerData()->ClientSupport70610() )
	{
		// Clients 7.0.0.0 and later
		const UI08 BIT__FROZEN	= 0;	//	0x01, frozen/paralyzed
		const UI08 BIT__FEMALE	= 1;	//	0x02, should be female flag
		const UI08 BIT__FLYING	= 2;	//	0x04, flying (post 7.0.0.0)
		const UI08 BIT__GOLDEN	= 3;	//	0x08, yellow healthbar
		//const UI08 BIT__IGNOREMOBILES = 5;	// 0x10, ignore other mobiles?

		flag.set( BIT__FROZEN, toCopy.IsFrozen() );
		flag.set( BIT__FEMALE, ( toCopy.GetId() == 0x0191 || toCopy.GetId() == 0x025E || toCopy.GetId() == 0x029B || toCopy.GetId() == 0xb8 || toCopy.GetId() == 0xba ));
		flag.set( BIT__FLYING, toCopy.IsFlying() );
		flag.set( BIT__GOLDEN, toCopy.IsInvulnerable() );
	}
	else
	{
		// Clients below 7.0.0.0
		const UI08 BIT__INVUL	= 0;	//	0x01
		const UI08 BIT__DEAD	= 1;	//	0x02
		const UI08 BIT__POISON	= 2;	//	0x04, poison
		const UI08 BIT__GOLDEN	= 3;	//	0x08, yellow healthbar
		//const UI08 BIT__IGNOREMOBILES = 5;	// 0x10, ignore other mobiles?

		flag.set( BIT__INVUL, toCopy.IsInvulnerable() );
		flag.set( BIT__DEAD, toCopy.IsDead() );
		flag.set( BIT__POISON, ( toCopy.GetPoisoned() != 0 ));
		flag.set( BIT__GOLDEN, toCopy.IsInvulnerable() );
	}

	const UI08 BIT__ATWAR	= 6;	//	0x40
	const UI08 BIT__INVIS	= 7;	//	0x80

	flag.set( BIT__ATWAR, toCopy.IsAtWar() );
	flag.set( BIT__INVIS, (toCopy.GetVisible() != VT_VISIBLE) || toCopy.IsDead() );

	pStream.WriteByte( 10, static_cast<UI08>( flag.to_ulong() ));
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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPPersonalLightLevel()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to update players personal light level
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x4E (Personal Light Level)
//|					Size: 6 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[4] creature id
//|						BYTE level
//o------------------------------------------------------------------------------------------------o
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

CPPersonalLightLevel &CPPersonalLightLevel::operator = ( CChar &toCopy )
{
	CopyData( toCopy );
	return ( *this );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPPlaySoundEffect()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to play sounds effect in player's client
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x54 (Play Sound Effect)
//|					Size: 12 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE mode (0x00=quiet, repeating, 0x01=single normally played sound effect)
//|						BYTE[2] SoundModel
//|						BYTE[2] unknown3 (speed/volume modifier? Line of sight stuff?)
//|						BYTE[2] xLoc
//|						BYTE[2] yLoc
//|						BYTE[2] zLoc / Or not z at all? Could be related to sequences when sent after packet 0x7B
//o------------------------------------------------------------------------------------------------o
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
CPPlaySoundEffect &CPPlaySoundEffect::operator = ( CBaseObject &toCopy )
{
	CopyData( toCopy );
	return ( *this );
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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPPaperdoll()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to open player's paperdoll
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x88 (Open Paperdoll)
//|					Size: 66 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[4] charid
//|						BYTE[60] text
//|						BYTE flag byte
//o------------------------------------------------------------------------------------------------o
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
void CPPaperdoll::Text( const std::string& toPut )
{
	if( toPut.length() > 60 )
	{
		pStream.WriteString( 5, toPut, 59 );
		pStream.WriteByte(  64, 0x00 );
	}
	else
	{
		pStream.WriteString( 5, toPut, toPut.size() );
	}
}
CPPaperdoll &CPPaperdoll::operator = ( CChar &toCopy )
{
	CopyData( toCopy );
	return ( *this );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPWeather()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to update weather in player's client
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x65 (Set Weather)
//|					Size: 4 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE type (0x00 – “It starts to rain”, 0x01 – “A fierce storm approaches.”, 0x02 – “It begins to snow”, 0x03 - “A storm is brewing.”, 0xFF – None (turns off sound effects), 0xFE (no effect?? Set temperature?)
//|						BYTE num (number of weather effects on screen)
//|						BYTE temperature
//|
//|					Note: Temperature has no effect at present.
//|					Note: Maximum number of weather effects on screen is 70.
//|					Note: If it is raining, you can add snow by setting the num to the num of rain
//|						currently going, plus the number of snow you want.
//|					Note: Weather messages are only displayed when weather starts.
//|					Note: Weather will end automatically after 6 minutes without any weather change packets.
//|					Note: You can totally end weather (to display a new message) by teleporting.
//|						I think it’s either the 0x78 or 0x20 messages that reset it, though I
//|						haven’t checked to be sure (other possibilities, 0x4F or 0x4E)
//o------------------------------------------------------------------------------------------------o
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
	if( nParts > 70 )
	{
		nParts = 70;
	}

	pStream.WriteByte( 2, nParts );
}
void CPWeather::Temperature( UI08 nTemp )
{
	pStream.WriteByte( 3, nTemp );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPGraphicalEffect()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to play a visual effect in player's client
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x70 (Graphical Effect)
//|					Size: 28 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE direction type
//|							00 = go from source to dest
//|							01 = lightning strike at source
//|							02 = stay at current x,y,z
//|							03 = stay with current source character id
//|						BYTE[4] character id
//|						BYTE[4] target id
//|						BYTE[2] model of the first frame of the effect
//|						BYTE[2] xLoc
//|						BYTE[2] yLoc
//|						BYTE zLoc
//|						BYTE[2] xLoc of target
//|						BYTE[2] yLoc of target
//|						BYTE zLoc of target
//|						BYTE speed of the animation
//|						BYTE duration (0=really long, 1= shortest)
//|						BYTE[2] unknown2 (0 works)
//|						BYTE adjust direction during animation (1=no)
//|						BYTE explode on impact
//o------------------------------------------------------------------------------------------------o
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
	pStream.WriteByte( 26, static_cast<UI08>(( nValue ? 0 : 1 )));
}
void CPGraphicalEffect::ExplodeOnImpact( bool nValue )
{
	pStream.WriteByte( 27, static_cast<UI08>(( nValue ? 1 : 0 )));
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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPUpdateStat()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet(s) to update player's health, mana and stamina
//|					Also used to update the health of items/multis with damageable flag set
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xA1 (Update Current Health)
//|					Packet: 0xA2 (Update Current Mana)
//|					Packet: 0xA3 (Update Current Stamina)
//|					Size: 9 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[4] playerId
//|						BYTE[2] maxHealth/maxMana/maxStamina
//|						BYTE[2] currentHealth/currentMana/currentStamina
//o------------------------------------------------------------------------------------------------o
void CPUpdateStat::InternalReset( void )
{
	pStream.ReserveSize( 9 );
	pStream.WriteByte( 0, 0xA1 );
}
CPUpdateStat::CPUpdateStat( CBaseObject &toUpdate, UI08 statNum, bool normalizeStats )
{
	InternalReset();
	Serial( toUpdate.GetSerial() );
	auto maxHP = 0;
	auto maxStam = 0;
	auto maxMana = 0;
	if( toUpdate.CanBeObjType( OT_CHAR ))
	{
		// For characters, 
		auto objChar = CalcCharObjFromSer( toUpdate.GetSerial() );
		maxHP = objChar->GetMaxHP();
		maxStam = objChar->GetMaxStam();
		maxMana = objChar->GetMaxMana();
	}
	else
	{
		// For items and multis, only health is relevant, as not only does
		// client not display anything relevant for stamina/mana for items,
		// but items also have no mana/stamina values to send!
		if( toUpdate.CanBeObjType(OT_MULTI) )
		{
			auto objMulti = CalcMultiFromSer( toUpdate.GetSerial() );
			maxHP = objMulti->GetMaxHP();
		}
		else
		{
			auto objItem = CalcItemObjFromSer( toUpdate.GetSerial() );
			maxHP = objItem->GetMaxHP();
		}
	}

	switch( statNum )
	{
		case 0:
			if( normalizeStats )
			{
				MaxVal( 100 );
				CurVal( static_cast<SI16>( ceil( 100 * ( static_cast<R32>( toUpdate.GetHP() ) / static_cast<R32>( maxHP )))));
			}
			else
			{
				MaxVal( maxHP );
				CurVal( toUpdate.GetHP() );
			}
			break;
		case 2:
			if( normalizeStats )
			{
				if( maxStam > 0 )
				{
					MaxVal( 100 );
					CurVal( static_cast<SI16>( ceil( 100 * ( static_cast<R32>( toUpdate.GetStamina() ) / static_cast<R32>( maxStam )))));
				}
				else
				{
					MaxVal( 0 );
					CurVal( 0 );
				}
			}
			else
			{
				MaxVal( maxStam );
				CurVal( toUpdate.GetStamina() );
			}
			break;
		case 1:
			if( normalizeStats )
			{
				if( maxMana > 0 )
				{
					MaxVal( 100 );
					CurVal( static_cast<SI16>( ceil( 100 * ( static_cast<R32>( toUpdate.GetMana() ) / static_cast<R32>( maxMana )))));
				}
				else
				{
					MaxVal( 0 );
					CurVal( 0 );
				}
			}
			else
			{
				MaxVal( maxMana );
				CurVal( toUpdate.GetMana() );
			}
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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPDeathAction()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to have client display player's death action
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xAF (Display Death Action)
//|					Size: 13 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[4] player id
//|						BYTE[4] corpse id
//|						BYTE[4] unknown (all 0)
//o------------------------------------------------------------------------------------------------o
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
CPDeathAction &CPDeathAction::operator = ( CChar &dying )
{
	Player( dying.GetSerial() );
	return ( *this );
}
CPDeathAction &CPDeathAction::operator = ( CItem &corpse )
{
	Corpse( corpse.GetSerial() );
	return ( *this );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPPlayMusic()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to have client play specified music
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x6D (Play Midi Music)
//|					Size: 3 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] musicID
//o------------------------------------------------------------------------------------------------o
void CPPlayMusic::InternalReset( void )
{
	pStream.ReserveSize( 3 );
	pStream.WriteByte( 0, 0x6D );
}
CPPlayMusic::CPPlayMusic( SI16 musicId )
{
	InternalReset();
	MusicId( musicId );
}
CPPlayMusic::CPPlayMusic()
{
	InternalReset();
}
void CPPlayMusic::MusicId( SI16 musicId )
{
	pStream.WriteShort( 1, musicId );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPDrawContainer()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to display a particular container gump
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x24 (Draw Container)
//|					Size: 3 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[4] item id
//|						BYTE[2] model-Gump
//|							0x003c = backpack
//|						BYTE[2] container type // added in UO:HS clients, 0x00 for vendors, 0x7D
//|							for spellbooks and containers
//o------------------------------------------------------------------------------------------------o
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
	pStream.WriteShort( 5, newModel ); //-1?
}
void CPDrawContainer::ContType( UI16 contType )
{
	pStream.ReserveSize( 9 );
	pStream.WriteShort( 7, contType );
}
CPDrawContainer &CPDrawContainer::operator = ( CItem &toCopy )
{
	CopyData( toCopy );
	return ( *this );
}
void CPDrawContainer::CopyData( CItem &toCopy )
{
	Serial( toCopy.GetSerial() );
}
void CPDrawContainer::Serial( SERIAL toSet )
{
	pStream.WriteLong( 1, toSet );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPOpenGump()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to open a dialog box/item list menu
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x7C (Open Dialog Box/Item List)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] blockSize
//|						BYTE[4] dialogID (echo'd back to the server in 7d)
//|						BYTE[2] menuid (echo'd back to server in 7d)
//|						BYTE length of question
//|						BYTE[length of question] question text
//|						BYTE # of responses
//|						Then for each response:
//|							BYTE[2] model id # of shown item (if grey menu -- then always 0x00 as msb)
//|							BYTE[2] color of shown item
//|							BYTE response text length
//|							BYTE[response text length] response text
//o------------------------------------------------------------------------------------------------o
void CPOpenGump::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[SEND]Packet     : CPOpenGump 0x7C --> Length: " << pStream.GetSize() << TimeStamp() << std::endl;
	}
	outStream << "DialogID         : " << std::hex << pStream.GetULong( 3 ) << std::endl;
	outStream << "MenuID           : " << pStream.GetUShort( 7 ) << std::endl;
	outStream << "Question Length  : " << std::dec << static_cast<SI16>( pStream.GetByte( 9 )) << std::endl;
	outStream << "Question         : ";

	for( UI32 i = 0; i < pStream.GetByte( 9 ); ++i )
	{
		outStream << pStream.GetByte( 10 + static_cast<size_t>( i ));
	}
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
		{
			outStream << pStream.GetByte( offsetCount + 5 + k );
		}
		outStream << std::endl;
		offsetCount += ( 5 + static_cast<size_t>( pStream.GetByte( static_cast<size_t>( offsetCount ) + 4 )));
	}
	outStream << "  Raw dump     :" << std::endl;
	CPUOXBuffer::Log( outStream, false );
}
void CPOpenGump::Question( std::string toAdd )
{
	pStream.ReserveSize( 10 + toAdd.length() + 2 );	// 10 for start of string, length of string + nullptr, plus spot for # responses
	pStream.WriteString( 10, toAdd, toAdd.length() );
#if defined( UOX_DEBUG_MODE )
	if( toAdd.length() >= 255 )
	{
		Console.Error( oldstrutil::format( "CPOpenGump::Question toAdd.length() is too long (%i)", toAdd.length() ));
	}
#endif
	pStream.WriteByte( 9, static_cast<UI08>( toAdd.length() + 1 ));
	responseBaseOffset	= ( pStream.GetSize() - 1 );
	responseOffset		= responseBaseOffset + 1;
}
void CPOpenGump::AddResponse( UI16 modelNum, UI16 colour, std::string responseText )
{
	pStream.WriteByte( responseBaseOffset, pStream.GetByte( responseBaseOffset ) + 1 ); // increment number of responses
#if defined( UOX_DEBUG_MODE )
	if( responseText.length() >= 255 )
	{
		Console.Error( oldstrutil::format( "CPOpenGump::AddResponse responseText is too long (%i)", responseText.length() ));
	}
#endif
	UI16 toAdd = static_cast<UI16>( 5 + responseText.length() );
	pStream.ReserveSize( pStream.GetSize() + toAdd );
	pStream.WriteShort(( responseOffset + 0 ), modelNum );
	pStream.WriteShort(( responseOffset + 2 ), colour   );
	pStream.WriteByte((  responseOffset + 4 ), static_cast<UI08>( responseText.length() ));
	pStream.WriteString(( responseOffset + 5 ), responseText, responseText.length() );
	responseOffset += toAdd;
}
void CPOpenGump::Finalize( void )
{
	pStream.WriteShort( 1, static_cast<UI16>( pStream.GetSize() ));
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
void CPOpenGump::Length( SI32 totalLines )
{
	pStream.WriteShort( 1, totalLines );
}
void CPOpenGump::GumpIndex( SI32 index )
{
	pStream.WriteShort( 7, index );
}
CPOpenGump &CPOpenGump::operator = ( CChar &toCopy )
{
	CopyData( toCopy );
	return ( *this );
}
void CPOpenGump::CopyData( CChar &toCopy )
{
	Serial( toCopy.GetSerial() );
}
void CPOpenGump::Serial( SERIAL toSet )
{
	pStream.WriteLong( 3, toSet );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPTargetCursor()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to activate targeting cursor in client
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x6C (Targeting Cursor Commands)
//|					Size: 19 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE type
//|							0x00 = Select Object
//|							0x01 = Select X, Y, Z
//|						BYTE[4] cursorID
//|						BYTE Cursor Type
//|							0: Neutral
//|							1: Harmful
//|							2: Helpful
//|							3: Cancel current targetting (server sent)
//|						The following are always sent but are only valid if sent by client
//|						BYTE[4] Clicked On ID
//|						BYTE[2] click xLoc
//|						BYTE[2] click yLoc
//|						BYTE unknown (0x00)
//|						BYTE click zLoc
//|						BYTE[2] model # (if a static tile, 0 if a map/landscape tile)
//|							Note: the model # shouldn’t be trusted.
//o------------------------------------------------------------------------------------------------o
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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPStatWindow()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to update player's stat window
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x11 (Stat Window Info )
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] packet length
//|						BYTE[4] player id
//|						BYTE[30] playerName
//|						BYTE[2] currentHitpoints
//|						BYTE[2] maxHitpoints
//|						BYTE name change flag (0x1 = allowed, 0 = not allowed)
//|						BYTE Status Flag/Supported Features
//|							Status Flag
//|							0x00: no more data following (end of packet here).
//|							0x01: T2A Extended Info
//|							0x03: UOR Extended Info
//|							0x04: AOS Extended Info (4.0+)
//|							0x05: UOML Extended Info (5.0+)
//|							0x06: UOKR Extended Info (UOKR+)
//|						BYTE sex + race
//|							0: Male Human
//|							1: Female Human
//|							2: Male Elf
//|							3: Female Elf
//|						BYTE[2] str
//|						BYTE[2] dex
//|						BYTE[2] int
//|						BYTE[2] currentStamina
//|						BYTE[2] maxStamina
//|						BYTE[2] currentMana
//|						BYTE[2] maxMana
//|						BYTE[4] gold
//|						BYTE[2] armor class
//|						BYTE[2] weight
//|						If (flag 5 or higher - ML attributes 5.0+)
//|							BYTE[2] Max Weight
//|							BYTE Race (see notes)
//|							UOML+ Race Flag
//|								1: Human
//|								2: Elf
//|								3: Gargoyle
//|						If (flag == 3) (UO:R attributes)
//|							BYTE[2]  statCap
//|							BYTE  pets current
//|							BYTE pets max
//|						If (flag == 4 - AoS attributes 4.0+)
//|							BYTE[2]	fireresist
//|							BYTE[2]	coldresist
//|							BYTE[2]	poisonresist
//|							BYTE[2]	energyresist
//|							BYTE[2]	luck
//|							BYTE[2]	damage max
//|							BYTE[2]	damage min
//|							BYTE[4]	Tithing points (Paladin Books)
//|						If (flag 6 or higher - KR attributes)
//|							BYTE[2] Hit Chance Increase
//|							BYTE[2] Swing Speed Increase
//|							BYTE[2] Damage Chance Increase
//|							BYTE[2] Lower Reagent Cost
//|							BYTE[2] Hit Points Regeneration
//|							BYTE[2] Stamina Regeneration
//|							BYTE[2] Mana Regeneration
//|							BYTE[2] Reflect Physical Damage
//|							BYTE[2] Enhance Potions
//|							BYTE[2] Defense Chance Increase
//|							BYTE[2] Spell Damage Increase
//|							BYTE[2] Faster Cast Recovery
//|							BYTE[2] Faster Casting
//|							BYTE[2] Lower Mana Cost
//|							BYTE[2] Strength Increase
//|							BYTE[2] Dexterity Increase
//|							BYTE[2] Intelligence Increase
//|							BYTE[2] Hit Points Increase
//|							BYTE[2] Stamina Increase
//|							BYTE[2] Mana Increase
//|							BYTE[2] Maximum Hit Points Increase
//|							BYTE[2] Maximum Stamina Increase
//|							BYTE[2] Maximum Mana Increase
//|
//|					Note: For characters other than the player, currentHitpoints and maxHitpoints
//|					are not the actual values.  MaxHitpoints is a fixed value, and currentHitpoints
//|					works like a percentage.
//o------------------------------------------------------------------------------------------------o
void CPStatWindow::SetCharacter( CChar &toCopy, CSocket &target )
{
	CChar *mChar = target.CurrcharObj();
	if( toCopy.GetSerial() == mChar->GetSerial() )
	{
		if( target.ReceivedVersion() )
		{
			if( target.ClientVersionMajor() >= 6 )
			{
				extended3 = true;
				extended4 = true;
				extended5 = true;
			    extended6 = true;
			    pStream.ReserveSize( 121 );
			    pStream.WriteByte( 2, 121 );
			    Flag( 6 );
			}
			else if( target.ClientVersionMajor() >= 5 )
			{
				extended3 = true;
				extended4 = true;
				extended5 = true;
				pStream.ReserveSize( 91 );
				pStream.WriteByte( 2, 91 );
				Flag( 5 );
			}
			else if( target.ClientVersionMajor() >= 4 )
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
		{
			// We haven't received any client details yet.. let's use default server settings

			if( cwmWorldState->ServerData()->GetClientFeature( CF_BIT_HS ))
			{
				extended3 = true;
				extended4 = true;
				extended5 = true;
				extended6 = true;
				pStream.ReserveSize( 121 );
				pStream.WriteByte( 2, 121 );
				Flag( 6 );
			}
			else if( cwmWorldState->ServerData()->GetClientFeature( CF_BIT_ML ))
			{
				extended3 = true;
				extended4 = true;
				extended5 = true;
				pStream.ReserveSize( 91 );
				pStream.WriteByte( 2, 91 );
				Flag( 5 );
			}
			else if( cwmWorldState->ServerData()->GetClientFeature( CF_BIT_AOS ))
			{
				extended3 = true;
				extended4 = true;
				pStream.ReserveSize( 88 );
				pStream.WriteByte( 2, 88 );
				Flag( 4 );
			}
			else if( cwmWorldState->ServerData()->GetClientFeature( CF_BIT_UOR ))
			{
				extended3 = true;
				pStream.ReserveSize( 70 );
				pStream.WriteByte( 2, 70 );
				Flag( 3 );
			}
			else
			{
				pStream.ReserveSize( 66 );
				pStream.WriteByte( 2, 66 );
				Flag( 1 );
			}
		}
	}

	// If toCopy is an NPC or another player, only show basic stats
	if( toCopy.GetSerial() != mChar->GetSerial() )
	{
		pStream.ReserveSize( 43 );
		pStream.WriteByte( 2, 43 );
		Flag( 0 );
		Serial( toCopy.GetSerial() );

		std::string charName = GetNpcDictName( &toCopy, &target, NRS_STATWINDOW_OTHER );
		Name( charName );
		SI16 currentHP = toCopy.GetHP();
		UI16 maxHP = toCopy.GetMaxHP();
		SI16 percentHP = 0;
		if( currentHP > 0 )
		{
			percentHP = static_cast<SI16>( ceil( 100 * ( static_cast<R32>( currentHP ) / static_cast<R32>( maxHP ))));
		}
		CurrentHP( percentHP );
		MaxHP( 100 );

		if( toCopy.IsTamed() && ValidateObject( toCopy.GetOwnerObj() ) && toCopy.GetOwner() == mChar->GetSerial() )
		{
			NameChange( true );
		}
		else
		{
			NameChange( false );
		}
	}
	else
	{
		// Send player their own full stats
		Serial( toCopy.GetSerial() );
		Name( toCopy.GetNameRequest( &toCopy, NRS_STATWINDOW_SELF ));

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
		if( toCopy.GetId() == 0x0190 || toCopy.GetOrgId() == 0x0190 ) // Male huamn
		{
			Sex( 0 );
		}
		else if( toCopy.GetId() == 0x0191 || toCopy.GetOrgId() == 0x0191 ) // Female human
		{
			Sex( 1 );
		}
		else if( toCopy.GetId() == 0x025D || toCopy.GetOrgId() == 0x025D ) // Male elf
		{
			Sex( 2 );
		}
		else if( toCopy.GetId() == 0x025E || toCopy.GetOrgId() == 0x025E ) // Female elf
		{
			Sex( 3 );
		}
		else if( toCopy.GetId() == 0x029A || toCopy.GetOrgId() == 0x029A ) // Male Garg
		{
			Sex( 4 );
		}
		else if( toCopy.GetId() == 0x029B || toCopy.GetOrgId() == 0x029B ) // Female Garg
		{
			Sex( 5 );
		}

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

		Weight( static_cast<UI16>( toCopy.GetWeight() / 100 ));
		if( extended5 )
		{
			MaxWeight( toCopy.GetStrength() * cwmWorldState->ServerData()->WeightPerStr() + 40 );
			UI16 bodyId = toCopy.GetId();
			switch( bodyId )
			{
				case 0x0190:
				case 0x0191:
				case 0x0192:
				case 0x0193:
					Race( 0x01 ); break; //human
				case 0x025D:
				case 0x025E:
				case 0x025F:
				case 0x0260:
					Race( 0x02 ); break; //elf
				case 0x029A:
				case 0x029B:
				case 0x02B6:
				case 0x02B7:
					Race( 0x03 ); break; //gargoyle
				default:
					Race( 0x01 ); break;
			}
		}
		if( extended3 )
		{
			StatCap( cwmWorldState->ServerData()->ServerStatCapStatus() );
			if( cwmWorldState->ServerData()->MaxControlSlots() > 0 )
			{
				// If pet control slots are enabled, send amount of pet slots used, and max value specified in ini (higher than 0 is enabled)
				CurrentPets( static_cast<UI08>( toCopy.GetControlSlotsUsed() ));
				MaxPets( static_cast<UI08>( cwmWorldState->ServerData()->MaxControlSlots() ));
			}
			else
			{
				// If pet control slots are disabled, send count of followers and maxFollowers value specified in ini instead
				CurrentPets( static_cast<UI08>( toCopy.GetFollowerList()->Num() ));
				MaxPets( static_cast<UI08>( cwmWorldState->ServerData()->MaxFollowers() ));
			}
		}
		if( extended4 )
		{
			FireResist( Combat->CalcDef( &toCopy, 0, false, HEAT ));
			ColdResist( Combat->CalcDef( &toCopy, 0, false, COLD ));
			PoisonResist( Combat->CalcDef( &toCopy, 0, false, POISON ));
			EnergyResist( Combat->CalcDef( &toCopy, 0, false, LIGHTNING ));
			Luck( 0 );
			DamageMin( Combat->CalcLowDamage( &toCopy ));
			DamageMax( Combat->CalcHighDamage( &toCopy ));
			TithingPoints( toCopy.GetTithing() );
		}
        if( extended6 )
        {
			PhysicalResistCap( cwmWorldState->ServerData()->PhysicalResistCap() );
			FireResistCap( cwmWorldState->ServerData()->FireResistCap() );
			ColdResistCap( cwmWorldState->ServerData()->ColdResistCap());
			PoisonResistCap( cwmWorldState->ServerData()->PoisonResistCap() );
			EnergyResistCap( cwmWorldState->ServerData()->EnergyResistCap() );

			DefenseChanceIncrease( 0 );
			DefenseChanceIncreaseCap( cwmWorldState->ServerData()->DefenseChanceIncreaseCap() );
			HitChanceIncrease( 0 );
			SwingSpeedIncrease( 0 );
			DamageChanceIncrease( 0 );
			LowerReagentCost( 0 );
			SpellDamageIncrease( 0 );
			FasterCastRecovery( 0 );
			FasterCasting( 0 );
			LowerManaCost( 0 );
        }
	}
}
void CPStatWindow::InternalReset( void )
{
	pStream.ReserveSize( 43 );
	pStream.WriteByte( 0, 0x11 );
	pStream.WriteByte( 1, 0x00 );
	pStream.WriteByte( 2, 43 );
	extended3 = false;
	extended4 = false;
	extended5 = false;
	extended6 = false;
	byteOffset = 0;
	Flag( 0 );
}
CPStatWindow::CPStatWindow()
{
	InternalReset();
}
CPStatWindow::CPStatWindow( CBaseObject &toCopy, CSocket &target )
{
	InternalReset();
	if( toCopy.CanBeObjType(OT_CHAR) )
	{
		auto charObj = CalcCharObjFromSer( toCopy.GetSerial() );
		SetCharacter( *charObj, target );
	}
	else
	{
		// Item! Let's send the compact version
		auto itemObj = CalcItemObjFromSer( toCopy.GetSerial() );
		pStream.ReserveSize( 43 );
		pStream.WriteByte( 2, 43 );
		Serial( itemObj->GetSerial() );
		Name( itemObj->GetNameRequest( target.CurrcharObj(), NRS_STATWINDOW_OTHER ));
		SI16 currentHP = itemObj->GetHP();
		UI16 maxHP = itemObj->GetMaxHP();
		SI16 percentHP = 0;
		if( currentHP > 0 )
		{
			percentHP = static_cast<SI16>( ceil( 100 * ( static_cast<R32>( currentHP ) / static_cast<R32>( maxHP ))));
		}
		CurrentHP( percentHP );
		MaxHP( 100 );
		NameChange( false );
		Flag( 0 );
	}
}
void CPStatWindow::Serial( SERIAL toSet )
{
	pStream.WriteLong( 3, toSet );
}
void CPStatWindow::Name( const std::string& nName )
{
	if( nName.length() >= 30 )
	{
		pStream.WriteString( 7, nName, 29 );
		pStream.WriteByte(   36, 0x00 );
	}
	else
	{
		pStream.WriteString( 7, nName, nName.size() );
	}
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
	pStream.WriteByte( 41, static_cast<UI08>(( nValue ? 0xFF : 0 )));
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
	byteOffset = 66;
}
//extended5
void CPStatWindow::MaxWeight( UI16 value )
{
	pStream.WriteShort( byteOffset, value );
	byteOffset += 2;
}
void CPStatWindow::Race( UI08 value )
{
	pStream.WriteByte( byteOffset, value );
	byteOffset += 1;
}
//extended5 end
//extended3 start
void CPStatWindow::StatCap( UI16 value )
{
	pStream.WriteShort( byteOffset, value );
	byteOffset += 2;
}
void CPStatWindow::CurrentPets( UI08 value )
{
	pStream.WriteByte( byteOffset, value );
	byteOffset += 1;
}
void CPStatWindow::MaxPets( UI08 value )
{
	pStream.WriteByte( byteOffset, value );
	byteOffset += 1;
}
//extended3 end
//extended4 start
void CPStatWindow::FireResist( UI16 value )
{
	pStream.WriteShort( byteOffset, value );
	byteOffset += 2;
}
void CPStatWindow::ColdResist( UI16 value )
{
	pStream.WriteShort( byteOffset, value );
	byteOffset += 2;
}
void CPStatWindow::PoisonResist( UI16 value )
{
	pStream.WriteShort( byteOffset, value );
	byteOffset += 2;
}
void CPStatWindow::EnergyResist( UI16 value )
{
	pStream.WriteShort( byteOffset, value );
	byteOffset += 2;
}
void CPStatWindow::Luck( UI16 value )
{
	pStream.WriteShort( byteOffset, value );
	byteOffset += 2;
}
void CPStatWindow::DamageMin( UI16 value )
{
	pStream.WriteShort( byteOffset, value );
	byteOffset += 2;
}
void CPStatWindow::DamageMax( UI16 value )
{
	pStream.WriteShort( byteOffset, value );
	byteOffset += 2;
}
void CPStatWindow::TithingPoints( UI32 value )
{
	pStream.WriteLong( byteOffset, value );
	byteOffset += 4;
}
//extended4 end
// extended6 start
void CPStatWindow::PhysicalResistCap( UI16 value )
{
	pStream.WriteShort( byteOffset, value);
	byteOffset += 2;
}
void CPStatWindow::FireResistCap( UI16 value )
{
	pStream.WriteShort( byteOffset, value);
	byteOffset += 2;
}
void CPStatWindow::ColdResistCap( UI16 value )
{
	pStream.WriteShort( byteOffset, value );
	byteOffset += 2;
}
void CPStatWindow::PoisonResistCap( UI16 value )
{
	pStream.WriteShort( byteOffset, value );
	byteOffset += 2;
}
void CPStatWindow::EnergyResistCap( UI16 value )
{
	pStream.WriteShort( byteOffset, value );
	byteOffset += 2;
}

void CPStatWindow::DefenseChanceIncrease(UI16 value)
{
    pStream.WriteShort(byteOffset, value);
    byteOffset += 2;
}

void CPStatWindow::DefenseChanceIncreaseCap(UI16 value)
{
    pStream.WriteShort(byteOffset, value);
    byteOffset += 2;
}

void CPStatWindow::HitChanceIncrease(UI16 value)
{
    pStream.WriteShort(byteOffset, value);
    byteOffset += 2;
}

void CPStatWindow::SwingSpeedIncrease(UI16 value)
{
    pStream.WriteShort(byteOffset, value);
    byteOffset += 2;
}

void CPStatWindow::DamageChanceIncrease(UI16 value)
{
    pStream.WriteShort(byteOffset, value);
    byteOffset += 2;
}

void CPStatWindow::LowerReagentCost(UI16 value)
{
    pStream.WriteShort(byteOffset, value);
    byteOffset += 2;
}

void CPStatWindow::SpellDamageIncrease(UI16 value)
{
    pStream.WriteShort(byteOffset, value);
    byteOffset += 2;
}

void CPStatWindow::FasterCastRecovery(UI16 value)
{
    pStream.WriteShort(byteOffset, value);
    byteOffset += 2;
}

void CPStatWindow::FasterCasting(UI16 value)
{
    pStream.WriteShort(byteOffset, value);
    byteOffset += 2;
}

void CPStatWindow::LowerManaCost(UI16 value)
{
    pStream.WriteShort(byteOffset, value);
    byteOffset += 2;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPIdleWarning()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to show idle warning or login rejection message
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x53 (Idle Warning/Reject Character Logon)
//|					Size: 2 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE value
//|							0x07 idle
//|							0x05 another character is online
//|								"Another character from this account is currently online in this world.
//|								You must either log in as that character or wait for it to time out.”
//o------------------------------------------------------------------------------------------------o
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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPTime()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to update time in client
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x5B (Time)
//|					Size: 4 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE hour
//|						BYTE minute
//|						BYTE second
//o------------------------------------------------------------------------------------------------o
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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPLoginComplete()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to verify login completion
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x55 (Login Complete, Start Game)
//|					Size: 1 bytes
//|
//|					Packet Build
//|						BYTE cmd
//o------------------------------------------------------------------------------------------------o
CPLoginComplete::CPLoginComplete()
{
	pStream.ReserveSize( 1 );
	pStream.WriteByte( 0, 0x55 );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPTextEmoteColour()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to set text/emote color
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x69 (Change Text/Emote Color)
//|					Size: 5 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] blockSize
//|						BYTE[2] unknown1
//|					The client sends two of these independent of the color chosen. It sends two of
//|						them in quick succession as part of the "same" packet. The unknown1 is
//|						0x00 0x01 in the first and 0x00 0x02 in the second.
//|					Note, this message has been removed.  It is no longer used.
//o------------------------------------------------------------------------------------------------o
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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPWarMode()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to respond to request for war mode/set war mode status
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x72 (Request War Mode Change/Send War Mode status)
//|					Size: 5 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE flag
//|							0x00 - Normal
//|							0x01 - Fighting
//|						BYTE[3] unknown1 (always 00 32 00 in testing)
//|
//|					Server also replies with 0x77 packet to update player after changing war mode
//o------------------------------------------------------------------------------------------------o
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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPPauseResume()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to pause/resume client
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x33 (Pause/Resume Client)
//|					Size: 2 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE pause/resume (0=pause, 1=resume)
//o------------------------------------------------------------------------------------------------o
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
	if( mSock == nullptr )
		return false;

	switch( mSock->ClientType() )
	{
		case CV_DEFAULT:
		case CV_T2A:
		case CV_UO3D:
		case CV_ML:
		case CV_KR2D:
		case CV_KR3D:
		case CV_SA2D:
		case CV_SA3D:
		case CV_HS2D:
		case CV_HS3D:
			if( mSock->ClientVersionMajor() >= 4 )
			{
				return false;
			}
			break;
		default:
			break;
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPWebLaunch()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to open a specific URL in player's default browser
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xA5 (Open Web Browser)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] blockSize
//|						BYTE[blockSize-3] null terminated full web address
//o------------------------------------------------------------------------------------------------o
void CPWebLaunch::InternalReset( void )
{
	pStream.ReserveSize( 4 );
	pStream.WriteByte( 0, 0xA5 );
}
CPWebLaunch::CPWebLaunch()
{
	InternalReset();
}
CPWebLaunch::CPWebLaunch( const std::string& txt )
{
	InternalReset();
	Text( txt );
}
void CPWebLaunch::Text( const std::string& txt )
{
	const SI16 tLen = static_cast<SI16>( txt.length() ) + 4;
	SetSize( tLen );
	pStream.WriteString( 3, txt, txt.size() );
}

void CPWebLaunch::SetSize( SI16 newSize )
{
	pStream.ReserveSize( newSize );
	pStream.WriteShort( 1, newSize );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPTrackingArrow()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to show quest tracking arrow in client
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xBA (Quest Arrow)
//|					Size: 6 or 10 bytes (HSA expansion)
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE active (1=on, 0=off)
//|						BYTE[2] xLoc
//|						BYTE[2] yLoc
//|						if HSA client
//|							BYTE[4] serial
//o------------------------------------------------------------------------------------------------o
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
CPTrackingArrow &CPTrackingArrow::operator = ( CBaseObject &toCopy )
{
	Location( toCopy.GetX(), toCopy.GetY() );
	return ( *this );
}
void CPTrackingArrow::Active( UI08 value )
{
	pStream.WriteByte( 1, value );
}
void CPTrackingArrow::AddSerial( SERIAL targetSerial )
{
	pStream.ReserveSize( 10 );
	pStream.WriteLong( 6, targetSerial );
}
CPTrackingArrow::CPTrackingArrow( CBaseObject &toCopy )
{
	InternalReset();
	Location( toCopy.GetX(), toCopy.GetY() );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPBounce()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to reject client request to move item
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x27 (Reject Request to Move Items)
//|					Size: 2 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE unknown1 (0x00)
//o------------------------------------------------------------------------------------------------o
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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPDyeVat()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to resond to client request to show dye window
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x95 (Dye Window)
//|					Size: 2 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[4] itemId of dye target
//|						BYTE[2] ignored on send, model on return
//|						BYTE[2] model on send, color on return  (default on server send is 0x0FAB)
//|
//|					NOTE: This packet is sent by both the server and client
//o------------------------------------------------------------------------------------------------o
void CPDyeVat::InternalReset( void )
{
	pStream.ReserveSize( 9 );
	pStream.WriteByte( 0, 0x95 );
}
void CPDyeVat::CopyData( CBaseObject &target )
{
	Serial( target.GetSerial() );
	Model( target.GetId() );
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
CPDyeVat &CPDyeVat::operator = ( CBaseObject &target )
{
	CopyData( target );
	return ( *this );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPMultiPlacementView()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to bring up house/boat placement preview
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x99 (Bring Up House/Boat Placement View)
//|					Size: 26/30 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE request (0x01 from server, 0x00 from client)
//|						BYTE[4] SERIAL of deed
//|						BYTE cursorFlags (0?)
//|						BYTE[11] unknown (all 0)
//|						BYTE[2] multi model (item model - 0x4000)
//|						BYTE[2] xOffset
//|						BYTE[2] yOffset
//|						BYTE[2] zOffset
//|						clientVer > 7.0.9.0, 30 byte size packet
//|							BYTE[4] Hue
//o------------------------------------------------------------------------------------------------o
void CPMultiPlacementView::InternalReset( void )
{
	pStream.ReserveSize( 26 );
	pStream.WriteByte( 0, 0x99 );
	for( UI08 i = 6; i < 18; ++i )
	{
		pStream.WriteByte( i, 0 );
	}
	for( UI08 k = 20; k < 26; ++k )
	{
		pStream.WriteByte( k, 0 );
	}
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
void CPMultiPlacementView::SetHue( UI16 hueValue )
{
	pStream.ReserveSize( 30 );
	pStream.WriteLong( 26, hueValue );
}
CPMultiPlacementView &CPMultiPlacementView::operator = ( CItem &target )
{
	CopyData( target );
	return ( *this );
}

CPMultiPlacementView::CPMultiPlacementView( SERIAL toSet )
{
	InternalReset();
	DeedSerial( toSet );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPEnableClientFeatures()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to enable client features
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xB9 (Enable locked client features)
//|					Size: 3 bytes
//|
//|					Packet Build
//|						Original Legacy Client Version
//|							BYTE cmd
//|							BYTE[2] Feature Bitflag
//|						From Legacy Client 6.0.14.2+
//|							BYTE cmd
//|							BYTE[4] Feature Bitflag
//|
//|						Bitflags:
//|						0x00	None
//|						0x01	Enable T2A features: chat-button, regions
//|						0x02	Enable UOR features
//|						0x04	Enable TD features
//|						0x08	Enable LBR features: skills, maps, mp3s, LBR monsters in 2D client
//|						0x10	Enable AOS update (Necromancer/Paladin skills, malas map/AOS monsters if AOS installation present)
//|						0x20	Enable Sixth Character Slot
//|						0x40	Enable SE features: Ninja/Samurai, spells, skills, map, housing tiles
//|						0x80	Enable ML features: elven race, spells, skills, housing tiles
//|						0x100	Enable the Eight Age splash screen
//|						0x200	Enable the Ninth Age splash screen and crystal/shadow housing tiles
//|						0x400	Enable the Tenth Age
//|						0x800	Enable increased housing and bank-storage
//|						0x1000	Enable 7th character slot
//|						0x2000	Enable KR faces
//|						0x4000	Enable Trial Account
//|						0x8000	Live (non-trial) Account. Since client 4.0 this bit has to be set, otherwise bits 3..14 are ignored.
//|						0x10000	Enable SA features: gargoyle race, spells, skills, housing tiles
//|						0x20000	Enable HS features
//|						0x40000	Enable Gothing housing tiles
//|						0x80000	Enable Rustic housing tiles
//|
//|						Example
//|						0	neither T2A NOR LBR, equal to not sending it at all,
//|						1	is T2A, chatbutton,
//|						2	is LBR without chatbutton,
//|						3	is LBR with chatbutton…
//|						8013	LBR + chatbutton + AOS enabled
//|
//|						Note1: this message is send immediately after login.
//|						Note2: on OSI  servers this controls features OSI enables/disables via “upgrade codes.”
//|						Note3: a 3 doesn’t seem to “hurt” older (NON LBR) clients.
//o------------------------------------------------------------------------------------------------o
CPEnableClientFeatures::CPEnableClientFeatures( CSocket *mSock )
{
	if( mSock->ClientType() <= CV_KR3D )
	{
		//Clients 6.0.14.1 and lower
		pStream.ReserveSize( 3 );
		pStream.WriteByte( 0, 0xB9 );
		pStream.WriteShort( 1, cwmWorldState->ServerData()->GetClientFeatures() );
	}
	if( mSock->ClientType() >= CV_SA2D )
	{
		//Clients 6.0.14.2 and higher
		pStream.ReserveSize( 5 );
		pStream.WriteByte( 0, 0xB9 );
		pStream.WriteLong( 1, cwmWorldState->ServerData()->GetClientFeatures() );
	}
}

void CPEnableClientFeatures::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[SEND]Packet     : CPEnableClientFeatures 0xB9 --> Length: 3" << TimeStamp() << std::endl;
	}

	UI32 lastByte = pStream.GetUShort( 1 );
	outStream << "Flags          : " << std::hex << static_cast<UI32>( lastByte ) << std::dec << std::endl;
	if(( lastByte & 0x01 ) == 0x01 )
	{
		outStream << "               : Enabled T2A features (including chat)" << std::endl;
	}
	if(( lastByte & 0x02 ) == 0x02 )
	{
		outStream << "               : Enabled UO:R support" << std::endl;
	}
	if(( lastByte & 0x04 ) == 0x04 )
	{
		outStream << "               : Enabled UO:TD support" << std::endl;
	}
	if(( lastByte & 0x08 ) == 0x08 )
	{
		outStream << "               : Enabled LBR support" << std::endl;
	}
	if(( lastByte & 0x10 ) == 0x10 )
	{
		outStream << "               : Enabled AOS support" << std::endl;
	}
	if(( lastByte & 0x20 ) == 0x20 )
	{
		outStream << "               : Enabled Sixth Character Slot?" << std::endl;
	}
	if(( lastByte & 0x40 ) == 0x40 )
	{
		outStream << "               : Enabled Samurai Empire?" << std::endl;
	}
	if(( lastByte & 0x80 ) == 0x80 )
	{
		outStream << "               : Enabled Elves and ML" << std::endl;
	}
	if(( lastByte & 0x100 ) == 0x100 )
	{
		outStream << "               : Enabled Eight Age splash-screen" << std::endl;
	}
	if(( lastByte & 0x200 ) == 0x200 )
	{
		outStream << "               : Enabled Ninth Age splash-screen" << std::endl;
	}
	if(( lastByte & 0x400 ) == 0x400 )
	{
		outStream << "               : Enabled Tenth Age splash-screen" << std::endl;
	}
	if(( lastByte & 0x800 ) == 0x800 )
	{
		outStream << "               : Enabled increased housing and bank-storage" << std::endl;
	}
	if(( lastByte & 0x1000 ) == 0x1000 )
	{
		outStream << "               : Enabled 7th character slot" << std::endl;
	}
	if(( lastByte & 0x2000 ) == 0x2000 )
	{
		outStream << "               : Enabled KR faces" << std::endl;
	}
	if(( lastByte & 0x4000 ) == 0x4000 )
	{
		outStream << "               : Enabled Trial Account" << std::endl;
	}
	if(( lastByte & 0x8000 ) == 0x8000 )
	{
		outStream << "               : Enabled Live (non-trial) account" << std::endl;
	}
	if(( lastByte & 0x10000 ) == 0x10000 )
	{
		outStream << "               : Enable SA features" << std::endl;
	}
	if(( lastByte & 0x20000 ) == 0x20000 )
	{
		outStream << "               : Enabled HS features" << std::endl;
	}
	if(( lastByte & 0x40000 ) == 0x40000 )
	{
		outStream << "               : Enabled Gothing housing tiles" << std::endl;
	}
	if(( lastByte & 0x80000 ) == 0x80000 )
	{
		outStream << "               : Enabled Rustic housing tiles" << std::endl;
	}
	if(( lastByte & 0x100000 ) == 0x100000 )
	{
		outStream << "               : Enabled Jungle housing tiles" << std::endl;
	}
	if(( lastByte & 0x200000 ) == 0x200000 )
	{
		outStream << "               : Enabled Shadowguard housing tiles" << std::endl;
	}
	if(( lastByte & 0x400000 ) == 0x400000 )
	{
		outStream << "               : Enabled ToL features" << std::endl;
	}
	if(( lastByte & 0x800000 ) == 0x800000 )
	{
		outStream << "               : Enabled Endless Journey account" << std::endl;
	}

	outStream << "  Raw dump     :" << std::endl;
	CPUOXBuffer::Log( outStream, false );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPNegotiateAssistantFeatures()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet with feature negotiation request for assistant tools
//|					like Razor and AssistUO
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xF0 (Negotiate Assistant Features)
//|					Size: 12 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] length
//|						BYTE 0xFE
//|						BYTE[8] Feature bits from enum
//|
//|					At the same time as this is sent in pcmanage.cpp, a timer is started if
//|					KICKONASSISTANTSILENCE is enabled in uox.ini. If no valid response is received
//|					within 30 seconds, player is disconnected from the server.
//o------------------------------------------------------------------------------------------------o
CPNegotiateAssistantFeatures::CPNegotiateAssistantFeatures( [[maybe_unused]] CSocket *mSock )
{
	pStream.ReserveSize( 12 );
	pStream.WriteByte(  0, 0xF0 );
	pStream.WriteShort( 1, 0x000C );
	pStream.WriteByte( 3, 0xFE );
	pStream.WriteLong( 4, cwmWorldState->ServerData()->GetDisabledAssistantFeatures() >> 32 );
	pStream.WriteLong( 8, cwmWorldState->ServerData()->GetDisabledAssistantFeatures() & 0xFFFFFFFF );
}

void CPNegotiateAssistantFeatures::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[SEND]Packet   : CPNegotiateAssistantFeatures 0xF0 --> Length: " << pStream.GetSize() << TimeStamp() << std::endl;
	}
	outStream << "  Raw dump     :" << std::endl;
	CPUOXBuffer::Log( outStream, false );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPAddItemToCont()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to add/show item to container
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x25 (Add Item to Container)
//|					Size: 20/21 bytes
//|
//|					Packet Build (Before 6.0.1.7, 2D client)
//|						BYTE cmd
//|						BYTE[4] serial if item to add
//|						BYTE[2] model ID
//|						BYTE model ID offset (0)
//|						BYTE[2] # of items in stack
//|						BYTE[2] xLoc in container
//|						BYTE[2] yLoc in container
//|						BYTE[4] serial of container
//|						BYTE[2] color
//|
//|					Packet Build (After 6.0.1.7, UOKR+ and 2D client)
//|						BYTE cmd
//|						BYTE[4] Item Serial
//|						BYTE[2] Item ID (Graphic)
//|						BYTE Item ID Offset (wtf?)
//|						BYTE[2] Stack Amount
//|						BYTE[2] X
//|						BYTE[2] Y
//|						BYTE Slot Index (see notes)
//|						BYTE[4] Container Serial
//|						BYTE[2] Color
//|
//|					NOTE: The backpack grid index exists since 6.0.1.7 2D and 2.45.5.6 KR.
//o------------------------------------------------------------------------------------------------o
void CPAddItemToCont::InternalReset( void )
{
	uokrFlag = false;
	pStream.ReserveSize( 20 );
	pStream.WriteByte( 0, 0x25 );
	pStream.WriteByte( 7, 0 );
}
void CPAddItemToCont::CopyData( CItem &toCopy )
{
	Serial( toCopy.GetSerial() );
	Model( toCopy.GetId() );
	NumItems( toCopy.GetAmount() );
	X( toCopy.GetX() );
	Y( toCopy.GetY() );
	if( uokrFlag )
	{
		GridLocation( toCopy.GetGridLocation() );
	}
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
void CPAddItemToCont::UOKRFlag( bool newVal )
{
	uokrFlag = newVal;
	pStream.ReserveSize(( uokrFlag ? 21 : 20 ));
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
void CPAddItemToCont::GridLocation( SI08 gridLoc )
{
	pStream.WriteByte( 14, gridLoc );
}
void CPAddItemToCont::Container( SERIAL toAdd )
{
	pStream.WriteLong(( uokrFlag ? 15 : 14 ), toAdd );
}
void CPAddItemToCont::Colour( SI16 toSet )
{
	pStream.WriteShort(( uokrFlag ? 19 : 18 ), toSet );
}
void CPAddItemToCont::Object( CItem &toAdd )
{
	CopyData( toAdd );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPKickPlayer()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to kick player from server
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x26 (Kick Player)
//|					Size: 5 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[4] serial of GM who issued kick?
//o------------------------------------------------------------------------------------------------o
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
CPKickPlayer &CPKickPlayer::operator = ( CChar &toCopy )
{
	CopyData( toCopy );
	return ( *this );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPResurrectMenu()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to respond to resurrection menu choice
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x2C (Resurrection Menu)
//|					Size: 2 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE action (2=ghost, 1=resurrect, 0=from server)
//|
//|					Note: Resurrection menu has been removed from UO.
//o------------------------------------------------------------------------------------------------o
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
{
	pStream.WriteByte( 1, action );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPFightOccurring()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet update client about combat taking place nearby
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x2F (Fight Occurring)
//|					Size: 10 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE unknown1 (0)
//|						BYTE[4] ID of attacker
//|						BYTE[4] ID of attacked
//|
//|					This packet is sent when there is a fight going on somewhere on screen.
//o------------------------------------------------------------------------------------------------o
void CPFightOccurring::InternalReset( void )
{
	pStream.ReserveSize( 10 );
	pStream.WriteByte( 0, 0x2F );
	pStream.WriteByte( 1, 0 );
}
// Sent when fight occuring somewhere on the screen
CPFightOccurring::CPFightOccurring()
{
	InternalReset();
}
CPFightOccurring::CPFightOccurring( CChar &attacker, CChar &defender )
{
	InternalReset();
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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPSkillsValues()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet update client with skill values
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x3A (Send Skills)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] Length
//|						BYTE Type
//|							0x00 - Full List of skills,
//|							0xFF - Single skill update,
//|							0x02 - Full List of skills with skill cap for each skill
//|							0xDF - Single skill update with skill cap for skill
//|						Repeat for each skill:
//|							BYTE[2] Skill ID number
//|							BYTE[2] Skill Value * 10
//|							BYTE[2] Unmodified Value * 10
//|							BYTE Skill Lock
//|						If( Type == 2 || Type == 0xDF )
//|							BYTE[2] Skill Cap
//|						Byte[2] Null terminated(0000) - Only if Type == 0x00
//o------------------------------------------------------------------------------------------------o
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
	UI08 numSkills = NumSkills();
	for( SI08 i = 0; i < numSkills; ++i )
	{
		SkillEntry( i, toCopy.GetSkill( i ), toCopy.GetBaseSkill( i ), toCopy.GetSkillLock( i ));
	}
}

void CPSkillsValues::NumSkills( UI08 numSkills )
{
	// multiply numSkills with amount of bytes needed per skill
	// plus the additional bytes needed for packet details
	BlockSize( 6 + ( numSkills * 9 ));
}
UI08 CPSkillsValues::NumSkills( void )
{
	SI32 size = pStream.GetShort( 1 );
	size -= 6;
	size /= 9;
	return static_cast<UI08>( size );
}

void CPSkillsValues::BlockSize( SI16 newValue )
{
	pStream.ReserveSize( newValue );
	pStream.WriteShort( 1, newValue ); //packet size, variable based on amount of skills
	pStream.WriteByte(  3, 0x02 );	// full list, capped
	pStream.WriteByte(  static_cast<size_t>( newValue ) - 1, 0x00 );	// finish off with a double nullptr
	pStream.WriteByte(  static_cast<size_t>( newValue ) - 2, 0x00 );
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

void CPSkillsValues::SkillEntry( SI16 skillId, SI16 skillVal, SI16 baseSkillVal, SkillLock skillLock )
{
	SI32 offset = ( skillId * 9 ) + 4;
	pStream.WriteShort( offset, skillId + 1 );
	pStream.WriteShort( static_cast<size_t>( offset ) + 2, skillVal );
	pStream.WriteShort( static_cast<size_t>( offset ) + 4, baseSkillVal );
	pStream.WriteByte(  static_cast<size_t>( offset ) + 6, skillLock );
	pStream.WriteShort( static_cast<size_t>( offset ) + 7, static_cast<UI16>( cwmWorldState->ServerData()->ServerSkillCapStatus() ));
}
CPSkillsValues &CPSkillsValues::operator = ( CChar &toCopy )
{
	CopyData( toCopy );
	return ( *this );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPMapMessage()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet informing client to display a map
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x90 (Map Message/Details/Course)
//|					Size: 19 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[4] key used/map serial
//|						BYTE[2] gump art id (0x139D), corner image
//|						BYTE[2] upper left x location of map
//|						BYTE[2] upper left y location of map
//|						BYTE[2] lower right x location of map
//|						BYTE[2] lower right y location of map
//|						BYTE[2] gump width in pixels
//|						BYTE[2] gump height in pixels
//o------------------------------------------------------------------------------------------------o
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
void CPMapMessage::KeyUsed( UI32 key )
{
	pStream.WriteLong( 1, key );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPBookTitlePage()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet updating title page of a book
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x93 (Book Header (old))
//|					Size: 99 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[4] book serial
//|						BYTE write flag
//|							0x00 - non-writable
//|							0x01 - writable
//|						BYTE new flag/unknown (0x1)
//|						BYTE[2] # of pages
//|						BYTE[60] title
//|						BYTE[30] author
//|
//|					NOTE: Client sends a 0x93 message on book close. Update packet for the server
//|						to handle changes. Write Flag through Page Count are all 0's on client response
//|					NOTE: Replaced with 0xD4 in newer clients?
//o------------------------------------------------------------------------------------------------o
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
void CPBookTitlePage::Title( const std::string& txt )
{
	if( txt.length() >= 60 )
	{
		pStream.WriteString( 9, txt, 59 );
		pStream.WriteByte( 68, 0x00 );
	}
	else
	{
		pStream.WriteString( 9, txt, txt.size() );
	}
}
void CPBookTitlePage::Author( const std::string& txt )
{
	if( txt.length() >= 30 )
	{
		pStream.WriteString( 69, txt, 29 );
		pStream.WriteByte( 98, 0x00 );
	}
	else
	{
		pStream.WriteString( 69, txt, txt.size() );
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPUltimaMessenger()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet updating title page of a book
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xBB (Ultima Messenger)
//|					Size: 9 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[4] id1 (source serial)
//|						BYTE[4] id2 (target serial)
//o------------------------------------------------------------------------------------------------o
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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPGumpTextEntry()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to display text entry dialog in client
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xAB (Gump Text Entry Dialog)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] blockSize
//|						BYTE[4] id
//|						BYTE parentID
//|						BYTE buttonId
//|						BYTE[2] textlen
//|						BYTE[?] text (text appearing at top of gump)
//|						BYTE cancel (0=disable, 1=enable - if enabled, client is able to cancel query)
//|						BYTE style (0=disable, 1=normal, 2=numerical)
//|						BYTE[4] format (if style 1, max text len, if style2, max numeric value)
//|						BYTE[2] text2 length
//|						BYTE[?] text2 (label above text entry box)
//o------------------------------------------------------------------------------------------------o
void CPGumpTextEntry::InternalReset( void )
{
	BlockSize( 3 );
	pStream.WriteByte( 0, 0xAB );
}
CPGumpTextEntry::CPGumpTextEntry()
{
	InternalReset();
}
CPGumpTextEntry::CPGumpTextEntry( const std::string& text )
{
	InternalReset();
	Text1( text );
}
CPGumpTextEntry::CPGumpTextEntry( const std::string& text1, const std::string& text2 )
{
	InternalReset();
	Text1( text1 );
	Text2( text2 );
}
void CPGumpTextEntry::Serial( SERIAL id )
{
	pStream.WriteLong( 3, id );
}
void CPGumpTextEntry::ParentId( UI08 newVal )
{
	pStream.WriteByte( 7, newVal );
}
void CPGumpTextEntry::ButtonId( UI08 newVal )
{
	pStream.WriteByte( 8, newVal );
}
void CPGumpTextEntry::Cancel( UI08 newVal )
{
	SI16 t1Len = Text1Len();
	pStream.WriteByte( static_cast<size_t>( t1Len ) + 11, newVal );
}
void CPGumpTextEntry::Style( UI08 newVal )
{
	SI16 t1Len = Text1Len();
	pStream.WriteByte( static_cast<size_t>( t1Len ) + 12, newVal );
}
void CPGumpTextEntry::Format( SERIAL id )
{
	SI16 t1Len = Text1Len();
	pStream.WriteLong( static_cast<size_t>( t1Len ) + 13, id );
}
void CPGumpTextEntry::Text1( const std::string& txt )
{
	size_t sLen = txt.length();
	BlockSize( static_cast<SI16>( sLen + 20 ));	// 11 + 1 + 8
	Text1Len( static_cast<SI16>( sLen + 1 ));
	pStream.WriteString( 11, txt, sLen );
}
void CPGumpTextEntry::Text2( const std::string& txt )
{
	size_t sLen	= txt.length();
	SI16 currentSize = CurrentSize();
	BlockSize( static_cast<UI16>( currentSize + sLen + 1 ));
	Text2Len( static_cast<SI16>( sLen + 1 ));
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
	return pStream.GetShort( static_cast<size_t>( t1Len ) + 17 );
}

void CPGumpTextEntry::Text2Len( SI16 newVal )
{
	SI16 t1Len = Text1Len();
	pStream.WriteShort( static_cast<size_t>( t1Len ) + 17, newVal );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPGodModeToggle()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to enable/disable god mode in god client
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x2B (Enable/Disable God Mode for God Client)
//|					Size: 2 bytes
//|
//|					Packet Build
//|						BYTE Cmd
//|						BYTE 0/1 For Disable/Enable God Mode in client.
//o------------------------------------------------------------------------------------------------o
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
	pStream.WriteByte( 1, s->GetByte( 1 ));
}

CPGodModeToggle::CPGodModeToggle( CSocket *s )
{
	InternalReset();
	CopyData( s );
}

CPGodModeToggle& CPGodModeToggle::operator = ( CSocket *s )
{
	CopyData( s );
	return ( *this );
}

void CPGodModeToggle::ToggleStatus( bool toSet )
{
	if( toSet )
	{
		pStream.WriteByte( 1, 1 );
	}
	else
	{
		pStream.WriteByte( 1, 0 );
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPLoginDeny()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to deny client's login request
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x82 (Login Denied)
//|					Size: 2 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE reason
//|
//|					Login reasons handled by client:
//|						0x00 - Incorrect name/password.
//|						0x01 - Someone is already using this account.
//|						0x02 - Your account has been blocked.
//|						0x03 - Your account credentials are invalid.
//|						0x04 - Communication problem.
//|						0x05 - The IGR concurrency limit has been met.
//|						0x06 - The IGR time limit has been met.
//|						0x07 - General IGR authentication failure.
//o------------------------------------------------------------------------------------------------o
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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPCharDeleteResult()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet with result of characterdeletion request
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x85 (Delete Result)
//|					Size: 2 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE result
//|
//|					Deletion results handled by client:
//|						0x00 - Incorrect password
//|						0x01 - Character does not exist
//|						0x02 - Character is currently being played
//|						0x03 - Character is too young to delete
//|						0x04 - Character is queued for deletion
//|						0x05 - Bad request
//o------------------------------------------------------------------------------------------------o
void CPCharDeleteResult::InternalReset( void )
{
	pStream.ReserveSize( 2 );
	pStream.WriteByte( 0, 0x85 );
}
CPCharDeleteResult::CPCharDeleteResult()
{
	InternalReset();
}
CPCharDeleteResult::CPCharDeleteResult( SI08 result )
{
	InternalReset();
	DeleteResult( result );
}
void CPCharDeleteResult::DeleteResult( SI08 result )
{
	pStream.WriteByte( 1, result );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CharacterListUpdate()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet with updated character list
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x86 (Character Lits Update)
//|					Size: variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] len
//|						BYTE[1] number of characters( slots 5, 6 or 7 )
//|						loop characters( 5, 6 or 7 ) :
//|							BYTE[30] character name
//|							BYTE[30] character password
//|						endloop( characters )
//|
//|					Deletion results handled by client:
//|						0x00 - Incorrect password
//|						0x01 - Character does not exist
//|						0x02 - Character is currently being played
//|						0x03 - Character is too young to delete
//|						0x04 - Character is queued for deletion
//|						0x05 - Bad request
//o------------------------------------------------------------------------------------------------o
CharacterListUpdate::CharacterListUpdate( UI08 charCount)
{
	numChars = charCount;
	InternalReset();
}
void CharacterListUpdate::InternalReset( void )
{
	UI16 packetLen = 4 + ( numChars * 60 );
	pStream.ReserveSize( packetLen );
	pStream.WriteByte( 0, 0x86 );
	pStream.WriteShort( 1, packetLen );
	pStream.WriteByte( 3, numChars );
}
void CharacterListUpdate::AddCharName( UI08 charNum, std::string charName )
{
	UI16 byteOffset = 4 + ( charNum > 0 ? ( charNum * 60 ) : 0 );
	pStream.WriteString( byteOffset, charName, 60 );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPKREncryptionRequest()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet with encryption request for KR3D client
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xE3 (KR Encryption Request)
//|					Size: 2 bytes
//|
//|					Packet Build
//|						BYTE[1] 0xE3
//|						BYTE[2] Length
//|						BYTE[4] Key 1 Length
//|						BYTE[Key 1 Length] Key 1
//|						BYTE[4] Key 2 Length
//|						BYTE[Key 2 Length] Key 2
//|						BYTE[4] Key 3 Length
//|						BYTE[Key 3 Length] Key 3
//|						BYTE[4] Unknown
//|						BYTE[4] Key 4 Length
//|						BYTE[Key 4 Length] Key 4
//|
//|					Notes
//|						Unverified encryption method of keys is AES in CFB Mode
//|						As of 08/02/2009, this is the current encryption known for KR clients up to date:
//|							0xe3,
//|							0x4d00,
//|							0x03000000,
//|							0x02, 0x01, 0x03,
//|							0x13000000,
//|							0x02, 0x11, 0x00, 0xfc, 0x2f, 0xe3, 0x81, 0x93, 0xcb, 0xaf, 0x98, 0xdd, 0x83, 0x13, 0xd2, 0x9e, 0xea, 0xe4, 0x13,
//|							0x10000000,
//|							0x78, 0x13, 0xb7, 0x7b, 0xce, 0xA8, 0xd7, 0xbc, 0x52, 0xde, 0x38, 0x30, 0xea, 0xe9, 0x1e, 0xa3,
//|							0x20000000,
//|							0x10000000,
//|							0x5a, 0xce, 0x3e, 0xe3, 0x97, 0x92, 0xe4, 0x8a, 0xf1, 0x9a, 0xd3, 0x04, 0x41, 0x03, 0xcb, 0x53
//o------------------------------------------------------------------------------------------------o
CPKREncryptionRequest::CPKREncryptionRequest( [[maybe_unused]] CSocket *s )
{
	pStream.ReserveSize( 77 );
	pStream.WriteByte( 0, 0xE3 );

	// Size
	pStream.WriteByte( 1, 0x00 );
	pStream.WriteByte( 2, 0x4D );

	pStream.WriteByte( 3, 0x00 );
	pStream.WriteByte( 4, 0x00 );
	pStream.WriteByte( 5, 0x00 );
	pStream.WriteByte( 6, 0x03 );
	pStream.WriteByte( 7, 0x02 );
	pStream.WriteByte( 8, 0x01 );
	pStream.WriteByte( 9, 0x03 );
	pStream.WriteByte( 10, 0x00 );
	pStream.WriteByte( 11, 0x00 );
	pStream.WriteByte( 12, 0x00 );
	pStream.WriteByte( 13, 0x13 );
	pStream.WriteByte( 14, 0x02 );
	pStream.WriteByte( 15, 0x11 );

	pStream.WriteByte( 16, 0x00 );
	pStream.WriteByte( 17, 0xFC );
	pStream.WriteByte( 18, 0x2F );
	pStream.WriteByte( 19, 0xE3 );
	pStream.WriteByte( 20, 0x81 );
	pStream.WriteByte( 21, 0x93 );
	pStream.WriteByte( 22, 0xCB );
	pStream.WriteByte( 23, 0xAF );
	pStream.WriteByte( 24, 0x98 );
	pStream.WriteByte( 25, 0xDD );
	pStream.WriteByte( 26, 0x83 );
	pStream.WriteByte( 27, 0x13 );
	pStream.WriteByte( 28, 0xD2 );
	pStream.WriteByte( 29, 0x9E );
	pStream.WriteByte( 30, 0xEA );
	pStream.WriteByte( 31, 0xE4 );

	pStream.WriteByte( 32, 0x13 );
	pStream.WriteByte( 33, 0x00 );
	pStream.WriteByte( 34, 0x00 );
	pStream.WriteByte( 35, 0x00 );
	pStream.WriteByte( 36, 0x10 );
	pStream.WriteByte( 37, 0x78 );
	pStream.WriteByte( 38, 0x13 );
	pStream.WriteByte( 39, 0xB7 );
	pStream.WriteByte( 40, 0x7B );
	pStream.WriteByte( 41, 0xCE );
	pStream.WriteByte( 42, 0xA8 );
	pStream.WriteByte( 43, 0xD7 );
	pStream.WriteByte( 44, 0xBC );
	pStream.WriteByte( 45, 0x52 );
	pStream.WriteByte( 46, 0xDE );
	pStream.WriteByte( 47, 0x38 );

	pStream.WriteByte( 48, 0x30 );
	pStream.WriteByte( 49, 0xEA );
	pStream.WriteByte( 50, 0xE9 );
	pStream.WriteByte( 51, 0x1E );
	pStream.WriteByte( 52, 0xA3 );
	pStream.WriteByte( 53, 0x00 );
	pStream.WriteByte( 54, 0x00 );
	pStream.WriteByte( 55, 0x00 );
	pStream.WriteByte( 56, 0x20 );
	pStream.WriteByte( 57, 0x00 );
	pStream.WriteByte( 58, 0x00 );
	pStream.WriteByte( 59, 0x00 );
	pStream.WriteByte( 60, 0x10 );
	pStream.WriteByte( 61, 0x5A );
	pStream.WriteByte( 62, 0xCE );
	pStream.WriteByte( 63, 0x3E );

	pStream.WriteByte( 64, 0xE3 );
	pStream.WriteByte( 65, 0x97 );
	pStream.WriteByte( 66, 0x92 );
	pStream.WriteByte( 67, 0xE4 );
	pStream.WriteByte( 68, 0x8A );
	pStream.WriteByte( 69, 0xF1 );
	pStream.WriteByte( 70, 0x9A );
	pStream.WriteByte( 71, 0xD3 );
	pStream.WriteByte( 72, 0x04 );
	pStream.WriteByte( 73, 0x41 );
	pStream.WriteByte( 74, 0x03 );
	pStream.WriteByte( 75, 0xCB );
	pStream.WriteByte( 76, 0x53 );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPKrriosClientSpecial()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet for krrios client special packet, used by ClassicUO
//|					for things like updating party/guild member locations in WorldMap
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xF0 (KrriosClientSpecial)
//|					Size: Variable
//|
//|					Packet Build
//|						Byte cmd
//|						Byte[2] length
//|						Byte type
//|							0x00 // krrios special packet accepted, enable WorldMap updates
//|							0x01 // custom party info
//|								Loop for each party member online
//|									BYTE[4] serial
//|									BYTE[2] x
//|									BYTE[2] y
//|									BYTE world
//|								BYTE[4] 0x0000
//|							0x02 // guild track info
//|								BYTE locations // bool, whether to update locations on map
//|								Loop for each party/guild member online
//|									BYTE[4] serial
//|									BYTE[2] x
//|									BYTE[2] y
//|									BYTE world
//|									BYTE healthpoint percentage of max hp
//|								BYTE[5] 0x00000
//|							0x03 // runebook contents
//|							0x04 // guardline data
//|							0xf0 // ???
//|							0xfe // Razor ack sent
//o------------------------------------------------------------------------------------------------o
CPKrriosClientSpecial::CPKrriosClientSpecial( CSocket * mSock, CChar * mChar, UI08 type, bool locations )
{
	pStream.ReserveSize( 2 );
	pStream.WriteByte( 0, 0xF0 );
	UI08 byteOffset = 3;

	switch( type )
	{
		case 0x00: // Party
		{
			pStream.WriteByte( byteOffset, 0x01 );
			byteOffset += 1;
			Party * myParty = PartyFactory::GetSingleton().Get( mSock->CurrcharObj() );
			if( myParty != nullptr )
			{
				std::vector<CPartyEntry *> *mList = myParty->MemberList();
				if( mList != nullptr )
				{
					for( size_t j = 0; j < mList->size(); ++j )
					{
						CPartyEntry *mEntry = ( *mList )[j];
						CChar *partyMember = mEntry->Member();

						if( partyMember->GetSerial() == mSock->CurrcharObj()->GetSerial() )
							continue;

						if( ObjInRange( mSock->CurrcharObj(), partyMember, DIST_INRANGE ) && partyMember->GetVisible() == 0 )
							continue;

						pStream.WriteLong( byteOffset, partyMember->GetSerial() );
						byteOffset += 4;
						pStream.WriteShort( byteOffset, partyMember->GetX() );
						byteOffset += 2;
						pStream.WriteShort( byteOffset, partyMember->GetY() );
						byteOffset += 2;
						pStream.WriteByte( byteOffset, partyMember->WorldNumber() );
						byteOffset += 1;
					}
				}
			}

			// Finalize packet and ship it!
			pStream.WriteLong( byteOffset, 0x0000 );
			byteOffset += 4;
			pStream.WriteShort( 1, byteOffset ); // length
			pStream.ReserveSize( byteOffset );
			break;
		}
		case 0x01: // Guild
		{
			pStream.WriteByte( byteOffset, 0x02 );
			byteOffset += 2;
			CGuild * mGuild = GuildSys->Guild( mChar->GetGuildNumber() );
			if( mGuild != nullptr )
			{
				size_t numRecruits = mGuild->NumRecruits();
				size_t numMembers = mGuild->NumMembers();

				// First, look up the recruits to see who's online
				for( size_t i = 0; i < numRecruits; i++ )
				{
					SERIAL recruitSerial = mGuild->RecruitNumber( i );
					CChar *guildRecruit = CalcCharObjFromSer( recruitSerial );
					if( guildRecruit != nullptr && guildRecruit->GetSocket() != nullptr )
					{
						if( guildRecruit->GetSerial() == mChar->GetSerial() )
							continue;

						if( locations && ObjInRange( mChar, guildRecruit, DIST_SAMESCREEN ) && guildRecruit->GetVisible() == 0 )
							continue;

						// Guild recruit is online, and it's not the player!
						pStream.WriteLong( byteOffset, guildRecruit->GetSerial() );
						byteOffset += 4;
						if( locations )
						{
							pStream.WriteShort( byteOffset, guildRecruit->GetX() );
							byteOffset += 2;
							pStream.WriteShort( byteOffset, guildRecruit->GetY() );
							byteOffset += 2;
							pStream.WriteByte( byteOffset, guildRecruit->WorldNumber() );
							byteOffset += 1;
							SI16 currentHP = guildRecruit->GetHP();
							UI16 maxHP = guildRecruit->GetMaxHP();
							UI08 percentHP = static_cast<UI08>( 100 * ( currentHP / maxHP ));

							pStream.WriteByte( byteOffset, percentHP );
							byteOffset += 1;
						}
					}
				}

				// Then, look up the guild members to see who's online
				for( size_t i = 0; i < numMembers; i++ )
				{
					SERIAL memberSerial = mGuild->MemberNumber( i );
					CChar *guildMember = CalcCharObjFromSer( memberSerial );
					if( guildMember != nullptr && guildMember->GetSocket() != nullptr )
					{
						if( guildMember->GetSerial() == mChar->GetSerial() )
							continue;

						if( locations && ObjInRange( mChar, guildMember, DIST_SAMESCREEN ) && guildMember->GetVisible() == 0 )
							continue;

						// Guild member is online, and it's not the player!
						pStream.WriteLong( byteOffset, guildMember->GetSerial() );
						byteOffset += 4;
						if( locations )
						{
							pStream.WriteShort( byteOffset, guildMember->GetX() );
							byteOffset += 2;
							pStream.WriteShort( byteOffset, guildMember->GetY() );
							byteOffset += 2;
							pStream.WriteByte( byteOffset, guildMember->WorldNumber() );
							byteOffset += 1;

							SI16 currentHP = guildMember->GetHP();
							UI16 maxHP = guildMember->GetMaxHP();
							UI08 percentHP = static_cast<UI08>( 100 * ( currentHP / maxHP ));

							pStream.WriteByte( byteOffset, percentHP );
							byteOffset += 1;
						}
					}
				}
			}

			// Finalize packet and ship it!
			pStream.WriteByte( 4, locations );
			pStream.WriteLong( byteOffset, 0x0000 );
			byteOffset += 4;
			pStream.WriteByte( byteOffset, 0x00 );
			byteOffset += 1;
			pStream.WriteShort( 1, byteOffset ); // length
			pStream.ReserveSize( byteOffset );
			break;
		}
		default:
			break;
	}
}

void CPKrriosClientSpecial::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[SEND]Packet   : CPKrriosClientSpecial 0xF0 --> Length: " << pStream.GetSize() << TimeStamp() << std::endl;
	}
	outStream << "  Raw dump     :" << std::endl;
	CPUOXBuffer::Log( outStream, false );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPMapChange()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to change huge of mouse cursor for Felucca and Trammel
//|					facets, and/or to change the map in the client
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xBF (General Information Packet)
//|					Subcommand: 0x8 (Set Cursor Hue / Set MAP )
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd (0xBF)
//|						BYTE[2] Length
//|						BYTE[2] Subcommand (0x8)
//|						Subcommand details
//|							BYTE hue
//|								0x00 = Felucca, unhued / BRITANNIA map.
//|								0x01 = Trammel, hued gold / BRITANNIA map
//|								0x02 = Ilshenar
//|								0x03 = Malas
//|								0x04 = Tokuno
//|								0x05 = TerMur)
//o------------------------------------------------------------------------------------------------o
void CPMapChange::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[SEND]Packet   : CPMapChange 0xBF Subcommand 8 --> Length: " << pStream.GetSize() << TimeStamp() << std::endl;
	}
	outStream << "Hue            : " << static_cast<SI16>( pStream.GetByte( 5 )) << std::endl;
	switch( pStream.GetByte( 5 ))
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
	if( ValidateObject( moving ))
	{
		SetMap( moving->WorldNumber() );
	}
}

void CPMapChange::SetMap( UI08 newMap )
{
	pStream.WriteByte( 5, newMap );
}
CPMapChange& CPMapChange::operator = ( CBaseObject& moving )
{
	SetMap( moving.WorldNumber() );
	return ( *this );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPCloseGump( UI32 gumpId, UI32 buttonId )
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to close a generic/custom gump with a specified ID,
//|					while also allowing to specify the button ID response to send after
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xBF (General Information Packet)
//|					Subcommand: 0x4 (Close Generic Gump)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd (0xBF)
//|						BYTE[2] Length
//|						BYTE[2] Subcommand (0x4)
//|						Subcommand details
//|							BYTE[4] dialogID // which gump to destroy (second ID in 0xB0 packet)
//|							BYTE[4] buttonId // response buttonId for packet 0xB1
//o------------------------------------------------------------------------------------------------o
CPCloseGump::CPCloseGump( UI32 gumpId, UI32 buttonId )
{
	_gumpId = gumpId;
	_buttonId = buttonId;
	InternalReset();
}
void CPCloseGump::InternalReset( void )
{
	pStream.ReserveSize( 13 );
	pStream.WriteByte( 0, 0xBF );		// Command: Packet 0xBF - General Information Packet
	pStream.WriteShort( 1, 13 );		// Packet length
	pStream.WriteShort( 3, 0x04 );		// Subcommand 0x04 - Close Generic Gump
	pStream.WriteLong( 5, _gumpId );	// gumpId - which gump to destroy
	pStream.WriteLong( 9, _buttonId );	// buttonId - response buttonId for packet 0xB1
}
void CPCloseGump::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[SEND]Packet   : CPCloseGump 0xBF Subcommand 4 --> Length: " << pStream.GetSize() << TimeStamp() << std::endl;
	}
	outStream << "gumpId            : " << static_cast<UI32>( pStream.GetLong( 5 )) << std::endl;
	outStream << "buttonId            : " << static_cast<UI32>( pStream.GetLong( 9 )) << std::endl;
	outStream << "  Raw dump     :" << std::endl;
	CPUOXBuffer::Log( outStream, false );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPItemsInContainer()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to send list of items in container to client
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x3C (Items in Container)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] blockSize
//|						BYTE[2] # of Item segments
//|						Item Segments:
//|							BYTE[4] itemId
//|							BYTE[2] model
//|							BYTE unknown1 (0x00)
//|							BYTE[2] # of items in stack
//|							BYTE[2] xLoc
//|							BYTE[2] yLoc
//|							BYTE[4] Container ItemID
//|							BYTE[2] color
//o------------------------------------------------------------------------------------------------o
void CPItemsInContainer::InternalReset( void )
{
	pStream.ReserveSize( 5 );
	pStream.WriteByte( 0, 0x3C );
	isVendor			= false;
	isPlayerVendor		= false;
	vendorSerial		= INVALIDSERIAL;
	isCorpse			= false;
	uokrFlag			= false;
}
CPItemsInContainer::CPItemsInContainer()
{
	InternalReset();
}

CPItemsInContainer::CPItemsInContainer( CSocket *mSock, CItem *container, UI08 contType, bool isPVendor )
{
	if( ValidateObject( container ))
	{
		InternalReset();
		if( mSock->ClientVerShort() >= CVS_6017 )
		{
			UOKRFlag( true );
		}
		Type( contType );
		PlayerVendor( isPVendor );
		if( isVendor )
		{
			VendorSerial( container->GetSerial() );
		}

		CopyData( mSock, ( *container ));
	}
}

void CPItemsInContainer::Type( UI08 contType )
{
	if( contType == 0x01 ) // Corpse
	{
		isCorpse = true;
	}
	else if( contType == 0x02 ) // Vendor
	{
		isVendor = true;
	}
}

void CPItemsInContainer::UOKRFlag( bool value )
{
	uokrFlag = value;
}
void CPItemsInContainer::PlayerVendor( bool value )
{
	isPlayerVendor = value;
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
	UI16 packetSize = static_cast<UI16>(( numItems * ( uokrFlag ? 20 : 19 )) + 5 );
	pStream.ReserveSize( packetSize );
	pStream.WriteShort( 1, packetSize );
	pStream.WriteShort( 3, numItems );
}
void CPItemsInContainer::AddItem( CItem *toAdd, UI16 itemNum, CSocket *mSock )
{
	pStream.ReserveSize( pStream.GetSize() + ( uokrFlag ? 20 : 19 ));
	UI16 baseOffset = static_cast<UI16>( 5 + itemNum * ( uokrFlag ? 20 : 19 ));
	pStream.WriteLong(  baseOffset +  0, toAdd->GetSerial() );
	pStream.WriteShort( static_cast<size_t>( baseOffset) +  4, toAdd->GetId() );
	pStream.WriteShort( static_cast<size_t>( baseOffset) +  7, toAdd->GetAmount() );
	pStream.WriteShort( static_cast<size_t>( baseOffset) +  9, toAdd->GetX() );
	pStream.WriteShort( static_cast<size_t>( baseOffset) + 11, toAdd->GetY() );
	if( uokrFlag )
	{
		pStream.WriteByte( static_cast<size_t>( baseOffset) + 13, toAdd->GetGridLocation() );
		baseOffset+=1;
	}

	if( isVendor )
	{
		pStream.WriteLong( static_cast<size_t>( baseOffset ) + 13, vendorSerial );
	}
	else
	{
		pStream.WriteLong( static_cast<size_t>( baseOffset ) + 13, toAdd->GetContSerial() );
	}

	pStream.WriteShort( static_cast<size_t>( baseOffset ) + 17, toAdd->GetColour() );

	toAdd->SetDecayTime( 0 );

	// Only send tooltip if server feature for tooltips is enabled
	if( cwmWorldState->ServerData()->GetServerFeature( SF_BIT_AOS ))
	{
		CPToolTip pSend( toAdd->GetSerial(), mSock, !isVendor, isPlayerVendor );
		mSock->Send( &pSend );
	}
}

void CPItemsInContainer::Add( UI16 itemNum, SERIAL toAdd, SERIAL cont, UI08 amount )
{
	UI16 baseOffset = static_cast<UI16>(( itemNum * ( uokrFlag ? 20 : 19 )) + 5 );

	pStream.WriteLong(  baseOffset + 0, toAdd );
	pStream.WriteShort( static_cast<size_t>( baseOffset ) + 7, amount );
	pStream.WriteLong(  static_cast<size_t>( baseOffset ) + ( uokrFlag ? 14 : 13 ), cont );
}

void CPItemsInContainer::CopyData( CSocket *mSock, CItem& toCopy )
{
	UI16 itemCount		= 0;
	bool itemIsCorpse	= toCopy.IsCorpse();

	auto tcCont = toCopy.GetContainsList();

	//tcCont->Reverse(); // This will reverse the order in which items are displayed in the container
	for( const auto &ctr : tcCont->collection() )
	{
		if( ValidateObject( ctr ) && ( !isCorpse || !itemIsCorpse || ( itemIsCorpse && ctr->GetLayer() )))
		{
			if( !ctr->IsFree() )
			{
				// don't show GM hidden objects to non-GM players.
				// don't show hairs and beards to anyone
				if( ctr->GetVisible() != 3 || mSock->CurrcharObj()->IsGM() )
				{
					AddItem( ctr, itemCount, mSock );
					++itemCount;
				}
			}
		}
	}
	NumberOfItems( itemCount );
}

void CPItemsInContainer::Log( std::ostream &outStream, bool fullHeader )
{
	size_t numItems = pStream.GetUShort( 3 );
	if( fullHeader )
	{
		outStream << "[SEND]Packet   : CPItemsInContainer 0x3c --> Length: " << pStream.GetSize() << TimeStamp() << std::endl;
	}
	outStream << "Block size     : " << pStream.GetUShort( 1 ) << std::endl;
	outStream << "Number of Items: " << std::dec << numItems << std::endl;
	size_t baseOffset = 5;
	for( size_t x = 0; x < numItems; ++x )
	{
		outStream << "  ITEM " << x << "      ID: " << "0x" << std::hex << pStream.GetULong( baseOffset ) << std::endl;
		outStream << "      Model     : " << "0x" << pStream.GetUShort( baseOffset+=4 ) << std::endl;
		outStream << "      Amount    : " << std::dec << pStream.GetUShort( baseOffset+=3 ) << std::endl;
		outStream << "      XY        : " << pStream.GetUShort( baseOffset+=2 );
		outStream << "," <<
		pStream.GetUShort( baseOffset+=2 ) << std::endl;
		outStream << "      Container : " << "0x" << std::hex << pStream.GetULong( baseOffset+=2 ) << std::endl;
		outStream << "      Color     : " << "0x" << pStream.GetUShort( baseOffset += 4 ) << std::endl;
		baseOffset += 2;
	}
	outStream << "  Raw dump      :" << std::endl;

	CPUOXBuffer::Log( outStream, false );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPOpenBuyWindow()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to open vendor buy window
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x74 (Open Buy Window)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] blockSize
//|						BYTE[4] (vendorId | 0x40000000)
//|						BYTE # of items
//|						# of items worth of item segments
//|							BYTE[4] price
//|							BYTE length of text description
//|							BYTE[text length] item description
//|
//|						NOTE: This packet is always preceded by a describe contents packet (0x3c)
//|							with the container id as the (vendorId | 0x40000000) and then an open
//|							container packet (0x24?) with the vendorId only and a model number of
//|							0x0030 (probably the model # for the buy screen)
//|						NOTE: The client displays items in the buy window from top left to bottom
//|							right. This means we need to sort the items logically before sending packets.
//o------------------------------------------------------------------------------------------------o
void CPOpenBuyWindow::InternalReset( void )
{
	pStream.ReserveSize( 8 );	// start big, and work back down
	pStream.WriteByte( 0, 0x74 );
}
CPOpenBuyWindow::CPOpenBuyWindow()
{
	InternalReset();
}
CPOpenBuyWindow::CPOpenBuyWindow( CItem *container, CChar *vendorId, CPItemsInContainer& iic, CSocket *mSock )
{
	if( ValidateObject( container ))
	{
		InternalReset();
		pStream.WriteLong( 3, container->GetSerial() );
		CopyData(( *container ), vendorId, iic, mSock );
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

UI32 CalcGoodValue( CTownRegion *tReg, CItem *i, UI32 value, bool isSelling );
UI32 CalcValue( CItem *i, UI32 value );
void CPOpenBuyWindow::AddItem( CItem *toAdd, CTownRegion *tReg, UI16 &baseOffset )
{
	UI32 value = toAdd->GetBuyValue();
	if( cwmWorldState->ServerData()->RankSystemStatus() )
	{
		value = CalcValue( toAdd, value );
	}
	if( cwmWorldState->ServerData()->TradeSystemStatus() )
	{
		value = CalcGoodValue( tReg, toAdd, value, false );
	}

	std::string itemname;
	itemname.reserve( MAX_NAME );
	UI08 sLen = 0;
	std::string temp	= toAdd->GetName();
	temp				= oldstrutil::simplify(temp);
	if( temp.substr( 0, 1 ) == "#" )
	{
		itemname = oldstrutil::number( 1020000 + toAdd->GetId() );
		sLen = static_cast<UI08>( itemname.size() + 1 );
	}
	else
	{
		sLen = static_cast<UI08>( GetTileName(( *toAdd ), itemname )); // Item name length, don't strip the nullptr (3D client doesn't like it)
	}

	pStream.ReserveSize( static_cast<size_t>( baseOffset ) + 5 + static_cast<size_t>( sLen ));
	pStream.WriteLong(   baseOffset, value );
	pStream.WriteByte(   baseOffset += 4, sLen );
	pStream.WriteString( baseOffset += 1, itemname, sLen );
	baseOffset += sLen;
}

auto CPOpenBuyWindow::CopyData( CItem& toCopy, CChar *vendorId, CPItemsInContainer& iic, CSocket *mSock ) -> void
{
	UI08 itemCount	= 0;
	UI16 length		= 8;
	CTownRegion *tReg = nullptr;
	if( cwmWorldState->ServerData()->TradeSystemStatus() && ValidateObject( vendorId ))
	{
		tReg = CalcRegionFromXY( vendorId->GetX(), vendorId->GetY(), vendorId->WorldNumber(), vendorId->GetInstanceId() );
	}

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

	auto tcCont = toCopy.GetContainsList();
	for( const auto &ctr : tcCont->collection() )
	{
		if( ValidateObject( ctr ))
		{
			if( !ctr->IsFree() )
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

void CPOpenBuyWindow::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[SEND]Packet   : CPOpenBuyWindow 0x74 --> Length: " << pStream.GetSize() << TimeStamp() << std::endl;
	}
	outStream << "Block size     : " << pStream.GetUShort( 1 ) << std::endl;
	outStream << "Vendor ID      : " << std::hex << pStream.GetULong( 3 ) << std::endl;
	outStream << "Number of Items: " << std::dec << static_cast<SI16>( pStream.GetByte( 7 )) << std::endl;
	SI32 baseOffset = 8;
	for( UI32 x = 0; x < pStream.GetByte( 7 ); ++x )
	{
		outStream << "  ITEM " << x << std::endl;
		outStream << "      Price: " << pStream.GetULong( baseOffset ) << std::endl;
		baseOffset += 4;
		outStream << "      Len  : " << static_cast<SI16>( pStream.GetByte( baseOffset )) << std::endl;
		outStream << "      Name : ";
		for( UI08 y = 0; y < pStream.GetByte( baseOffset ); ++y )
		{
			outStream << pStream.GetByte( static_cast<size_t>( baseOffset ) + 1 + y );
		}
		baseOffset += pStream.GetByte( baseOffset ) + 1;
		outStream << std::endl;
	}

	outStream << "  Raw dump :" << std::endl;

	CPUOXBuffer::Log( outStream, false );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPCharAndStartLoc()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet with character list / starting locations
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xA9 (Character List / Starting Locations)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] length
//|						BYTE # of characters (5, 6 or 7 slots)
//|						Following repeated for each character slot
//|							BYTE[30] character name
//|							BYTE[30] character password
//|						BYTE number of starting locations
//|						Repeat the following for as many locations as you have
//|							if client version >= 7.0.13.0
//|								BYTE locationIndex (0-based)
//|								BYTE[32] city name(general name)
//|								BYTE[32] area of city or town
//|								BYTE[4] City X Coordinate
//|								BYTE[4] City Y Coordinate
//|								BYTE[4] City Z Coordinate
//|								BYTE[4] CIty Map ( Probably Map ID same as in mul files have to make sure )
//|								BYTE[4] Cliloc Description
//|								BYTE[4] Always 0
//|							if client version < 7.0.13.0
//|								BYTE locationIndex (0-based)
//|								BYTE[31] town (general name)
//|								BYTE[31] exact name
//|						BYTE[4] Flags
//|							0x01	= unknown
//|							0x02	= send config/req logout (IGR?)
//|							0x04	= single character (siege) (alternative seen, Limit Characters)
//|							0x08	= enable npcpopup/context menus
//|							0x10	= unknown, (alternative seen, single character)
//|							0x20	= enable common AOS features (tooltip thing/fight system book, but not AOS monsters/map/skills)
//|							0x40	= Sixth Character Slot?
//|							8x80	= Samurai Empire?
//|							0x100	= Elven race
//|							0x200	= KR support flag1
//|							0x400	= Send UO3D client type (client will send 0xE1 packet)
//|							0x800	= unknown
//|							0x1000	= 7th character slot, 2D client
//|							0x2000	= unknown (SA?)
//|							0x4000	= new movement packets 0xF0 -> 0xF2
//|							0x8000	= unlock new felucca areas (faction areas)
//o------------------------------------------------------------------------------------------------o
void CPCharAndStartLoc::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
		outStream << "[SEND]Packet   : CPCharAndStartLoc 0xA9 --> Length: " << pStream.GetSize() << TimeStamp() << std::endl;
	outStream << "# Chars        : " << static_cast<SI16>( pStream.GetByte( 3 )) << std::endl;
	outStream << "Characters --" << std::endl;

	UI32 startLocOffset, realChars;
	if( pStream.GetByte( 3 ) > 6 )
	{
		startLocOffset	= 424;
		realChars		= 7;
	}
	else if( pStream.GetByte( 3 ) > 5 )
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
		outStream << "    Character " << static_cast<UI16>( i ) << ":" << std::endl;
		outStream << "      Name: ";
		for( UI08 j = 0; j < 30; ++j )
		{
			if( pStream.GetByte( static_cast<size_t>( baseOffset ) + j ) != 0 )
			{
				outStream << static_cast<SI08>( pStream.GetByte( static_cast<size_t>( baseOffset ) + j ));
			}
			else
			{
				break;
			}
		}
		outStream << std::endl << "      Pass: ";
		for( UI08 k = 0; k < 30; ++k )
		{
			if( pStream.GetByte( static_cast<size_t>( baseOffset ) + k + 30 ) != 0 )
			{
				outStream << static_cast<SI08>( pStream.GetByte( static_cast<size_t>( baseOffset ) + k + 30 ));
			}
			else
			{
				break;
			}
		}
		outStream << std::endl;
	}

	outStream << "# Starts       : " << static_cast<SI16>( pStream.GetByte( startLocOffset )) << std::endl;
	outStream << "Starting locations --" << std::endl;
	for( UI08 l = 0; l < pStream.GetByte( startLocOffset ); ++l )
	{
		UI32 baseOffset = startLocOffset + 1 + l * 63;
		outStream << "    Start " << static_cast<SI16>( l ) << std::endl;
		outStream << "      Index       : " << static_cast<SI16>( pStream.GetByte( baseOffset )) << std::endl;
		outStream << "      General Name: ";
		++baseOffset;
		for( UI08 m = 0; m < 31; ++m )
		{
			if( pStream.GetByte( static_cast<size_t>( baseOffset ) + m ) != 0 )
			{
				outStream << pStream.GetByte( static_cast<size_t>( baseOffset ) + m );
			}
			else
			{
				break;
			}
		}
		outStream << std::endl << "      Exact Name  : ";
		baseOffset += 31;
		for( UI08 n = 0; n < 31; ++n )
		{
			if( pStream.GetByte( static_cast<size_t>( baseOffset ) + n ) != 0 )
			{
				outStream << pStream.GetByte( static_cast<size_t>( baseOffset ) + n );
			}
			else
			{
				break;
			}
		}
		outStream << std::endl;
	}
	UI16 lastByte = pStream.GetUShort( pStream.GetSize() - 2 );
	outStream << "Flags          : " << std::hex << static_cast<UI32>( lastByte ) << std::dec << std::endl;
	if(( lastByte & 0x02 ) == 0x02 )
	{
		outStream << "               : Send config/request logout" << std::endl;
	}
	if(( lastByte & 0x04 ) == 0x04 )
	{
		outStream << "               : Single character" << std::endl;
	}
	if(( lastByte & 0x08 ) == 0x08 )
	{
		outStream << "               : Enable NPC Popup menus" << std::endl;
	}
	if(( lastByte & 0x10 ) == 0x10 )
	{
		outStream << "               : Unknown" << std::endl;
	}
	if(( lastByte & 0x20 ) == 0x20 )
	{
		outStream << "               : Enable Common AoS features" << std::endl;
	}
	if(( lastByte & 0x40 ) == 0x40 )
	{
		outStream << "               : Enable Sixth Character Slot?" << std::endl;
	}
	if(( lastByte & 0x80 ) == 0x80 )
	{
		outStream << "               : Enable Samurai Empire?" << std::endl;
	}
	if(( lastByte & 0x100 ) == 0x100 )
	{
		outStream << "               : Enable Elves and ML?" << std::endl;
	}
	if(( lastByte & 0x200 ) == 0x200 )
	{
		outStream << "               : Enable KR support flag1" << std::endl;
	}
	if(( lastByte & 0x400 ) == 0x400 )
	{
		outStream << "               : Send UO3D client type" << std::endl;
	}
	if(( lastByte & 0x800 ) == 0x800 )
	{
		outStream << "               : Unknown" << std::endl;
	}
	if(( lastByte & 0x1000 ) == 0x1000 )
	{
		outStream << "               : Enable 7th character slot" << std::endl;
	}
	if(( lastByte & 0x2000 ) == 0x2000 )
	{
		outStream << "               : Unknown" << std::endl;
	}
	if(( lastByte & 0x4000 ) == 0x4000 )
	{
		outStream << "               : Enable new movement packets" << std::endl;
	}
	if(( lastByte & 0x8000 ) == 0x8000 )
	{
		outStream << "               : Enable new Felucca faction areas" << std::endl;
	}
	outStream << "  Raw dump     :" << std::endl;
	CPUOXBuffer::Log( outStream, false );
}

void CPCharAndStartLoc::InternalReset( void )
{
	packetSize = 0;
	numCharacters = 0;
	pStream.ReserveSize( 4 );
	pStream.WriteByte( 0, 0xA9 );
}

CPCharAndStartLoc::CPCharAndStartLoc()
{
	InternalReset();
}
CPCharAndStartLoc::CPCharAndStartLoc( CAccountBlock_st& actbBlock, [[maybe_unused]] UI08 numCharacters, UI08 numLocations, CSocket *mSock )
{
	InternalReset();

	UI08 noLoopBytes = 9;
	if( mSock->ClientType() == CV_SA3D || mSock->ClientType() == CV_HS3D )
	{
		noLoopBytes = 11;
	}

	UI08 charSlots = 5;
	if( cwmWorldState->ServerData()->GetServerFeature( SF_BIT_SIXCHARS ) &&
	   cwmWorldState->ServerData()->GetClientFeature( CF_BIT_SIXCHARS ) && mSock->ClientVersionMajor() >= 4 )
	{
		charSlots = 6;
	}
	if( cwmWorldState->ServerData()->GetServerFeature( SF_BIT_SEVENCHARS ) &&
	   cwmWorldState->ServerData()->GetClientFeature( CF_BIT_SEVENCHARS ) && mSock->ClientVersionMajor() >= 7 )
	{
		charSlots = 7;
	}

	if( mSock->ClientType() >= CV_HS2D && mSock->ClientVersionSub() >= 13 )
	{
		packetSize = static_cast<UI16>( noLoopBytes + ( charSlots * 60 ) + ( numLocations * 89 ));
		pStream.ReserveSize( packetSize );
		pStream.WriteShort( 1, packetSize );
		pStream.WriteLong( packetSize - 4, static_cast<UI32>( cwmWorldState->ServerData()->GetServerFeatures() ));
	}
	else
	{
		packetSize = static_cast<UI16>( noLoopBytes + ( charSlots * 60 ) + ( numLocations * 63 ));
		pStream.ReserveSize( packetSize );
		pStream.WriteShort( 1, packetSize );
		pStream.WriteLong( packetSize - 4, static_cast<UI32>( cwmWorldState->ServerData()->GetServerFeatures() ));
	}

	pStream.WriteByte( 3, charSlots );

	if( mSock->ClientType() == CV_SA3D || mSock->ClientType() == CV_HS3D ) //maybe 7.0.13+ too?
	{
		pStream.WriteShort( packetSize - 2, 0x00 ); // last used character slot?
	}

	CopyData( actbBlock );
	NumberOfLocations( numLocations, mSock );
}

void CPCharAndStartLoc::CopyData( CAccountBlock_st& toCopy )
{
	UI16 baseOffset = 0;
	for( UI08 i = 0; i < pStream.GetByte( 3 ); ++i )
	{
		baseOffset = static_cast<UI16>( 4 + ( i * 60 ));
		if( toCopy.lpCharacters[i] != nullptr )
		{
			AddCharacter( toCopy.lpCharacters[i], i );
		}
		else
		{
			pStream.WriteString( baseOffset, "", 60 );
		}
	}
}

void CPCharAndStartLoc::AddCharacter( CChar *toAdd, UI08 charOffset )
{
	UI16 baseOffset = 0;
	baseOffset = static_cast<UI16>( 4 + ( charOffset * 60 ));
	if( !ValidateObject( toAdd ))
	{
		pStream.WriteString( baseOffset, "", 60 );
		return;
	}
	pStream.WriteString( baseOffset, toAdd->GetName(), 60 );
}

void CPCharAndStartLoc::NumberOfLocations( UI08 numLocations, [[maybe_unused]] CSocket *mSock )
{
	UI16 byteOffset = 0;
	byteOffset = static_cast<UI16>( 4 + ( pStream.GetByte( 3 ) * 60 ));
	pStream.WriteByte( byteOffset, numLocations );
}

auto CPCharAndStartLoc::AddStartLocation( __STARTLOCATIONDATA__ *sLoc, UI08 locOffset ) -> void
{
	if( sLoc )
	{
		UI16 baseOffset = 0;
		baseOffset = static_cast<UI16>( 5 + ( pStream.GetByte( 3 ) * 60 ));
		baseOffset += ( locOffset * 63 );

		pStream.WriteByte( baseOffset, locOffset ); // StartLocation #
		pStream.WriteString( static_cast<size_t>( baseOffset ) + 1, sLoc->oldTown, 31 );
		pStream.WriteString( static_cast<size_t>( baseOffset ) + 33, sLoc->oldDescription, 31 );
	}
}

auto CPCharAndStartLoc::NewAddStartLocation( __STARTLOCATIONDATA__ *sLoc, UI08 locOffset ) -> void
{
	if( sLoc )
	{
		UI16 baseOffset = 0;
		baseOffset = static_cast<UI16>( 5 + ( pStream.GetByte( 3 ) * 60 ));
		baseOffset += ( locOffset * 89 );

		pStream.WriteByte( baseOffset, locOffset ); // StartLocation #
		pStream.WriteString( static_cast<size_t>( baseOffset ) + 1, sLoc->newTown, 32 );
		pStream.WriteString( static_cast<size_t>( baseOffset ) + 33, sLoc->newDescription, 32 );
		pStream.WriteLong( static_cast<size_t>( baseOffset ) + 65, sLoc->x );
		pStream.WriteLong( static_cast<size_t>( baseOffset ) + 69, sLoc->y );
		pStream.WriteLong( static_cast<size_t>( baseOffset ) + 73, sLoc->z );
		pStream.WriteLong( static_cast<size_t>( baseOffset ) + 77, sLoc->worldNum );
		pStream.WriteLong( static_cast<size_t>( baseOffset ) + 81, sLoc->clilocDesc );
		pStream.WriteLong( static_cast<size_t>( baseOffset ) + 85, 0x00 );
	}
}

CPCharAndStartLoc& CPCharAndStartLoc::operator = ( CAccountBlock_st& actbBlock )
{
	CopyData( actbBlock );
	return ( *this );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPKAccept()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to acknowledge login of custom (Krrios') client
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xF0 (Custom client packet)
//|					Size: 5
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] len
//|						BYTE subcmd
//|						BYTE[len - 4] data
//|
//|					Subcommand 0: Acknowledge login (1 byte (for 5 total))
//|						BYTE ack
//|							0 = Rejected, unauthorized
//|							1 = Accepted, GM priviledges
//|							2 = Accepted, player priviledges
//|
//|					Note: GM priviledges unlock movement rate configurations
//o------------------------------------------------------------------------------------------------o
CPKAccept::CPKAccept( UI08 Response )
{
	pStream.ReserveSize( 5 );
	pStream.WriteByte(  0, 0xF0 );
	pStream.WriteShort( 1, 0x0005 );
	pStream.WriteByte(  3, 0x00 );
	pStream.WriteByte(  4, Response );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPUpdScroll()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to display tips/notice window with message on login
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xA6 (Tips/Notice window)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] blockSize
//|						BYTE flag
//|							0x00 - tips window
//|							0x01 - notice window
//|							0x02 - update window
//|						BYTE[2] unknown1
//|						BYTE[2] tip #
//|						BYTE[2] msgSize
//|						BYTE[msgSize] message (ascii, not null terminated)
//|						OR BYTE[?] message (? = blockSize - 10)
//|
//|					*Null terminated I think (Gimli)
//o------------------------------------------------------------------------------------------------o
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
	mstrcat( tipData, 2048, toAdd );
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
	SetLength( static_cast<UI16>( y ));
	pStream.WriteString( 10, tipData, y - 10 );
}

void CPUpdScroll::SetLength( UI16 len )
{
	pStream.ReserveSize( len );
	pStream.WriteShort( 1, len );
	pStream.WriteShort( 8, ( len - 10 ));
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPGraphicalEffect2()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to display graphical effect in client
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xC0 (Graphical Effect 2)
//|					Size: 36 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE type
//|						BYTE[4] sourceSerial
//|						BYTE[4] targetSerial
//|						BYTE[2] itemId
//|						BYTE[2] xSource
//|						BYTES[2] ySource
//|						BYTE zSource
//|						BYTE[2] xTarget
//|						BYTE[2] yTarget
//|						BYTE zTarget
//|						BYTE speed
//|						BYTE duration
//|						BYTE[2] unk // On OSI, flamestrikes are 0x0100
//|						BYTE fixedDirection
//|						BYTE explodes
//|						BYTE[4] hue
//|						BYTE[4] renderMode
//|						Rendermode details:
//|							One 	 Darken
//|							Two 	 Lighten
//|							Three 	 Transparent to dark colors to emphasize the bright colors
//|							Four 	 Semi-transparent (high transparency)
//|							Five 	 Translucent (near the present)
//|							Six 	 "Negapoji" (???) reversal
//|							Seven 	 Invert background "Negapoji Keta"(???)
//o------------------------------------------------------------------------------------------------o
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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CP3DGraphicalEffect()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to display 3D graphical/particle effect in client
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xC7 (3D Particle Effect)
//|					Size: 49 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[35] ...Preamble .. exactly like 0xC0
//|						BYTE[2] effect # (tile ID)
//|						BYTE[2] explode effect # (0 if no explosion)
//|						BYTE[2] additional effect # that's only used for moving effects, 0 otherwise
//|						BYTE[4] if target is item (type 2) that's itemId, 0 otherwise (EDIT: Packet logs indiciate this is target serial?)
//|						BYTE[1] layer (of the character, e.g left hand, right hand, ... 0-4, 0xff: moving effect or target is no char)
//|						BYTE[2] yet another (unknown) additional effect that's only set for moving effect, 0 otherwise
//o------------------------------------------------------------------------------------------------o
void CP3DGraphicalEffect::InternalReset( void )
{
	pStream.ReserveSize( 49 );
	pStream.WriteByte(  0, 0xC7 );
	pStream.WriteShort( 24, 0x0000 );
}
CP3DGraphicalEffect::CP3DGraphicalEffect( UI08 effectType ) : CPGraphicalEffect2( effectType )
{
	InternalReset();
}
CP3DGraphicalEffect::CP3DGraphicalEffect( UI08 effectType, CBaseObject &src, CBaseObject &trg ) : CPGraphicalEffect2( effectType, src, trg )
{
	InternalReset();
	Effect( effectType );
	SourceSerial( src );
	TargetSerial( trg );
}
CP3DGraphicalEffect::CP3DGraphicalEffect( UI08 effectType, CBaseObject &src ) : CPGraphicalEffect2( effectType, src )
{
	InternalReset();
	Effect( effectType );
	SourceSerial( src );
}
void CP3DGraphicalEffect::EffectId3D( UI16 effectId3D )
{
	pStream.WriteShort( 36, effectId3D );
}
void CP3DGraphicalEffect::ExplodeEffectId( UI16 explodeEffectId )
{
	pStream.WriteShort( 38, explodeEffectId );
}
void CP3DGraphicalEffect::MovingEffectId( UI16 movingEffectId )
{
	pStream.WriteShort( 40, movingEffectId );
}
void CP3DGraphicalEffect::TargetObjSerial( SERIAL targetObjSerial )
{
	pStream.WriteLong( 42, targetObjSerial );
}
void CP3DGraphicalEffect::LayerId( UI08 layerId )
{
	pStream.WriteByte( 46, layerId );
}
void CP3DGraphicalEffect::Unknown( UI16 unknown )
{
	pStream.WriteShort( 47, unknown );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPMapRelated()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet responding to client requests for map actions
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x56 (Map Related Actions)
//|					Size: 11 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[4] map object serial
//|						BYTE map action
//|							1 = add map point,
//|							2 = add new pin with pin number. (insertion. other pins after the number are pushed back.)
//|							3 = change pin
//|							4 = remove pin
//|							5 = remove all pins on the map
//|							6 = toggle the 'editable' state of the map.
//|							7 = return msg from the server to the request 6 of the client.
//|						BYTE plotting state (1=on, 0=off, valid only if command 7)
//|						BYTE[2] x location (relative to upper left corner of the map, in pixels, for points)
//|						BYTE[2] y location (relative to upper left corner of the map, in pixels, for points)
//o------------------------------------------------------------------------------------------------o
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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPDrawObject()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet with instructions for client to draw an object
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x78 (Draw object)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] blockSize
//|						BYTE[4] item/character serial
//|						BYTE[2] model (item hex #)
//|						if (itemId & 0x80000000)
//|							BYTE[2] amount/Corpse Model Num
//|						BYTE[2] xLoc (only 15 lsb)
//|						BYTE[2] yLoc
//|						if (xLoc & 0x8000)
//|							BYTE direction
//|						BYTE zLoc
//|						BYTE direction
//|						BYTE[2] dye/skin color
//|						BYTE flag
//|						BYTE notoriety (2's complement signed)
//|						if (BYTE[4] == 0x00 0x00 0x00 0x00)
//|							DONE
//|						else loop this until above if statement is satisified
//|							BYTE[4] item serial
//|							BYTE[2] model (item hex # - only 15 lsb)
//|							BYTE layer
//|							if (model & 0x8000)
//|								BYTE[2] hue
//o------------------------------------------------------------------------------------------------o
//|					Notes
//|						Status Options
//|							0x00: Normal
//|							0x01: Unknown
//|							0x02: Can Alter Paperdoll
//|							0x04: Poisoned
//|							0x08: Golden Health
//|							0x10: Unknown
//|							0x20: Unknown
//|							0x40: War Mode
//|						Notoriety
//|							0x1: Innocent (Blue)
//|							0x2: Friend (Green)
//|							0x3: Grey (Grey - Animal)
//|							0x4: Criminal (Grey)
//|							0x5: Enemy (Orange)
//|							0x6: Murderer (Red)
//|							0x7: Invulnerable (Yellow)
//o------------------------------------------------------------------------------------------------o
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
	pStream.WriteLong( cPos, static_cast<UI32>( 0 ));
}

void CPDrawObject::AddItem( CItem *toAdd, bool alwaysAddItemHue )
{
	UI16 cPos = curLen;

	pStream.WriteLong(  cPos, toAdd->GetSerial() );
	pStream.WriteShort( cPos += 4, static_cast<SI32>( toAdd->GetId() ));
	pStream.WriteByte(  cPos += 2, toAdd->GetLayer() );

	if( alwaysAddItemHue )
	{
		// Always send color to clients 7.0.33.1 and above
		pStream.WriteShort( cPos += 1, static_cast<SI32>( toAdd->GetColour() ));
		SetLength( curLen + 9 );
	}
	else
	{
		// Send color to clients below 7.0.33.1 only if different than default
		bool bColour = ( toAdd->GetColour() != 0 );
		if( bColour )
		{
			SetLength( curLen + 9 );
			pStream.WriteByte( cPos - 2, pStream.GetByte( cPos-2 ) | 0x80 );
			pStream.WriteShort( ++cPos, static_cast<SI32>( toAdd->GetColour() ));
		}
		else
		{
			SetLength( curLen + 7 );
		}
	}
}

void CPDrawObject::SetRepFlag( UI08 value )
{
	pStream.WriteByte( 18, value );
}

void CPDrawObject::CopyData( CChar& mChar )
{
	pStream.WriteLong(   3, mChar.GetSerial() );
	pStream.WriteShort(  7, mChar.GetId() );
	pStream.WriteShort(  9, mChar.GetX() );
	pStream.WriteShort( 11, mChar.GetY() );
	pStream.WriteByte(  13, mChar.GetZ() );
	pStream.WriteByte(  14, mChar.GetDir() );
	pStream.WriteShort( 15, static_cast<SI32>( mChar.GetSkin() ));

	//	0	0x01 - Frozen/Invulnerable
	//	1	0x02 - Female/Dead
	//	2	0x04 . Flying/Poisoned
	//	3	0x08 - Yellow Healthbar
	//	4	0x10 - Ignore Mobiles
	//	5	0x20 - ???
	//	6	0x40 - War Mode
	//	7	0x80 - Hidden

	std::bitset<8> flag( 0 );

	if( cwmWorldState->ServerData()->ClientSupport7000() || cwmWorldState->ServerData()->ClientSupport7090() ||
	   cwmWorldState->ServerData()->ClientSupport70160() || cwmWorldState->ServerData()->ClientSupport70240() ||
	   cwmWorldState->ServerData()->ClientSupport70300() || cwmWorldState->ServerData()->ClientSupport70331() ||
	   cwmWorldState->ServerData()->ClientSupport704565() || cwmWorldState->ServerData()->ClientSupport70610() )
	{
		// Clients 7.0.0.0 and later
		const UI08 BIT__FROZEN = 0;	//	0x01, frozen/paralyzed
		const UI08 BIT__FEMALE = 1;	//	0x02, female
		const UI08 BIT__FLYING = 2;	//	0x04, flying (post 7.0.0.0)
		const UI08 BIT__GOLDEN = 3;	//	0x08, yellow healthbar
		//const UI08 BIT__IGNOREMOBILES = 4;	// 0x10, ignore other mobiles?

		flag.set( BIT__FROZEN, mChar.IsFrozen() );
		flag.set( BIT__FEMALE, ( mChar.GetId() == 0x0191 || mChar.GetId() == 0x025E ));
		flag.set( BIT__FLYING, mChar.IsFlying() );
		flag.set( BIT__GOLDEN, mChar.IsInvulnerable() );
	}
	else
	{
		// Clients below 7.0.0.0
		const UI08 BIT__INVUL = 0;	//	0x01, invulnerable
		const UI08 BIT__DEAD = 1;	//	0x02, dead
		const UI08 BIT__POISON = 2;	//	0x04, poison
		const UI08 BIT__GOLDEN = 3;	//	0x08, yellow healthbar
		//const UI08 BIT__IGNOREMOBILES = 4;	// 0x10, ignore other mobiles?

		flag.set( BIT__INVUL, mChar.IsInvulnerable() );
		flag.set( BIT__DEAD, mChar.IsDead() );
		flag.set( BIT__POISON, ( mChar.GetPoisoned() != 0 ));
		flag.set( BIT__GOLDEN, mChar.IsInvulnerable() );
	}

	const UI08 BIT__ATWAR = 6;	//	0x40
	const UI08 BIT__OTHER = 7;	//	0x80
	flag.set( BIT__ATWAR, mChar.IsAtWar() );
	flag.set( BIT__OTHER, (( !mChar.IsNpc() && !IsOnline( mChar )) || ( mChar.GetVisible() != VT_VISIBLE ) || ( mChar.IsDead() && !mChar.IsAtWar() )));

	pStream.WriteByte( 17, static_cast<UI08>( flag.to_ulong() ));
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPCorpseClothing()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet with items to equip on corpse
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x89 (Corpse Clothing)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] blockSize
//|						BYTE[4] corpse serial
//|						Then, for each item to equip on corpse, loop this:
//|							BYTE itemLayer
//|							BYTE[4] itemId
//|						BYTE terminator (0x00)
//|
//|					Followed by a 0x3C message with the contents.
//o------------------------------------------------------------------------------------------------o
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
	if( ValidateObject( corpse ))
	{
		CopyData(( *corpse ));
	}
}

void CPCorpseClothing::NumberOfItems( UI16 numItems )
{
	// knowing the number of items, set the packet size
	UI16 packetSize = static_cast<UI16>(( numItems * 5 ) + 8 );		// 7 for lead in, 1 for lead out
	pStream.ReserveSize( packetSize );
	pStream.WriteShort( 1, packetSize );
	pStream.WriteByte(  packetSize - 1, 0x00 );
}
void CPCorpseClothing::AddItem( CItem *toAdd, UI16 itemNum )
{
	UI16 baseOffset = static_cast<UI16>( 7 + itemNum * 5 );
	pStream.WriteByte( baseOffset, toAdd->GetLayer() + 1 );
	pStream.WriteLong( static_cast<size_t>( baseOffset ) + 1, toAdd->GetSerial() );
}
CPCorpseClothing& CPCorpseClothing::operator = ( CItem& corpse )
{
	CopyData( corpse );
	return ( *this );
}

auto CPCorpseClothing::CopyData( CItem& toCopy ) -> void
{
	pStream.WriteLong( 3, toCopy.GetSerial() );
	UI16 itemCount = 0;
	auto tcCont = toCopy.GetContainsList();
	for( const auto &ctr : tcCont->collection() )
	{
		if( ValidateObject( ctr ))
		{
			if( !ctr->IsFree() && ctr->GetLayer() )
			{
				pStream.ReserveSize( pStream.GetSize() + 5 );
				AddItem( ctr, itemCount );
				++itemCount;
			}
		}
	}
	NumberOfItems( itemCount );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPNewObjectInfo()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet with object information (new version)
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xF3 (New Object Information)
//|					Size: 24 bytes SA, 26 bytes HS
//|
//|					Packet Build
//|						Byte cmd
//|						Byte[2] 0x1 // always 0x1 on OSI
//|						Byte DataType // 0x00 = Item , 0x01 = Character, 0x02 = Multi
//|						Byte[4] Serial
//|						Byte[2] Object ID // for multi its same value as the multi has in multi.mul
//|						Byte Offset/Facing // 0x00 if Multi
//|						Byte[2] Amount // 0x1 if Multi
//|						Byte[2] Amount // 0x1 if Multi , no idea why Amount is sent 2 times
//|						Byte[2] X
//|						Byte[2] Y
//|						Byte Z
//|						Byte Layer // 0x00 if Multi / Light Level (TileData.Quality or 0 for Mobiles)
//|						Byte[2] Color // 0x00 if Multi
//|						Byte Flag // 0x20 = Movable if normally not , 0x80 = Hidden , 0x00 if Multi
//|						IF ITEM
//|							Byte[2] Access (for items only, 0x01 = Player Item, 0x00 = World Item)
//|
//|						Notes
//|						Replaces 0x1A packet for clients 7.0.0.0+
//|						Required to to display items with IDs over 0x3FFF (old 0x1A packet only works with items up to that ID)
//o------------------------------------------------------------------------------------------------o
void CPNewObjectInfo::InternalReset( void )
{
	pStream.ReserveSize( 24 );
	pStream.WriteByte( 0, 0xF3 );
	pStream.WriteShort( 1, 0x1 );
}
void CPNewObjectInfo::CopyData( CItem& mItem, CChar& mChar )
{
	if( mItem.CanBeObjType( OT_MULTI ))
	{
		CopyMultiData( static_cast<CMultiObj&>( mItem ), mChar );
	}
	else if( mItem.CanBeObjType( OT_ITEM ))
	{
		CopyItemData( mItem, mChar );
	}
}

void CPNewObjectInfo::CopyItemData( CItem &mItem, CChar &mChar )
{
	bool isInvisible	= ( mItem.GetVisible() != VT_VISIBLE || ( mItem.GetVisible() == VT_TEMPHIDDEN && &mChar != mItem.GetOwnerObj() ));
	bool isMovable		= ( mItem.GetMovable() == 1 || mChar.AllMove() || ( mItem.IsLockedDown() && &mChar == mItem.GetOwnerObj() ));

	if( mItem.IsDamageable() )
	{
		pStream.WriteByte( 3, 0x03 ); // Damageable Item DataType
	}
	else
	{
		pStream.WriteByte( 3, 0x00 ); // Item DataType
	}

	pStream.WriteLong( 4, mItem.GetSerial() ); //Serial

	// if player is a gm, this item is shown like a candle (so that he can move it),
	// ....if not, the item is a normal invisible light source!
	if( mChar.IsGM() && mItem.GetId() == 0x1647 )
	{
		pStream.WriteShort( 8, 0x0A0F );
	}
	else
	{
		pStream.WriteShort( 8, mItem.GetId() );
	}

	pStream.WriteByte( 10, 0x00 ); //Offset/Facing?

	//Amount
	pStream.WriteShort( 11, mItem.GetAmount() );
	pStream.WriteShort( 13, mItem.GetAmount() );

	//Location
	pStream.WriteShort( 15, mItem.GetX() );
	pStream.WriteShort( 17, mItem.GetY() );
	pStream.WriteByte( 19, mItem.GetZ() );

	//Direction/Light Level
	pStream.WriteByte( 20, 	mItem.GetDir() );

	if( mChar.IsGM() && mItem.GetId() == 0x1647 ) //Lightsource
	{
		pStream.WriteShort( 21, 0x00C6 );
	}
	else
	{
		pStream.WriteShort( 21, mItem.GetColour() );
	}

	//Flags
	if( isInvisible )
	{
		pStream.WriteByte( 23, ( pStream.GetByte( 23 ) | 0x80 ));
	}
	if( isMovable )
	{
		pStream.WriteByte( 23, ( pStream.GetByte( 23 ) | 0x20 ));
	}

	if( mChar.GetSocket()->ClientType() >= CV_HS2D )
	{
		pStream.ReserveSize( 26 );
		pStream.WriteShort( 24, 0x00 ); // HS requires 2 extra bytes
	}
}

void CPNewObjectInfo::CopyMultiData( CMultiObj& mMulti, CChar &mChar )
{
	pStream.WriteByte( 3, 0x02 ); //DataType
	pStream.WriteLong( 4, mMulti.GetSerial() ); //Serial

	SI32 itemId = mMulti.GetId();
	itemId &= 0x3FFF;
	//itemId += 0x4000; // Old POL server forum post mentioned newer clients changing how they want the multiID sent. Adding this to the ID (so a small house goes from 0x4064 to 0x8064) still works, but so does the old value. Why the change, and what does it meean? When is it needed?
	if( mChar.ViewHouseAsIcon() )
	{
		pStream.WriteShort( 8, 0x14F0 );
	}
	else
	{
		pStream.WriteShort( 8, itemId ); //Client wants real ID from multi
	}

	pStream.WriteByte( 10, 0 ); //Offset/Facing? 0 or 0x00 - does it matter?

	//Amount
	pStream.WriteShort( 11, 1 ); //1 or 0x1 - does it matter? can multis have amounts higher than 1?
	pStream.WriteShort( 13, 1 ); //1 or 0x1 - does it matter? can multis have amounts higher than 1?

	//Location
	pStream.WriteShort( 15, mMulti.GetX() );
	pStream.WriteShort( 17, mMulti.GetY() );
	pStream.WriteByte( 19, mMulti.GetZ() );

	//Light Level
	pStream.WriteByte( 20, 	0x00 );

	pStream.WriteShort( 21, mMulti.GetColour() ); //Central mast on classic boats can have a color, apparently!

	//Flags
	pStream.WriteByte( 23, 0x00 );

	if( mChar.GetSocket()->ClientType() >= CV_HS2D ) //required for boats to work properly?
	{
		pStream.ReserveSize( 26 );
		pStream.WriteShort( 24, 0x00 ); // HS requires 2 extra bytes
	}
}

CPNewObjectInfo::CPNewObjectInfo()
{
	InternalReset();
}
CPNewObjectInfo::CPNewObjectInfo( CItem& mItem, CChar& mChar )
{
	InternalReset();
	CopyData( mItem, mChar );
}

void CPNewObjectInfo::Objects( CItem& mItem, CChar& mChar )
{
	CopyData( mItem, mChar );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPObjectInfo()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet with object information (old version)
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x1A (Object Information)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] blockSize
//|						BYTE[4] itemId
//|						BYTE[2] model #
//|						if (itemId & 0x80000000)
//|							BYTE[2] item count (or model # for corpses)
//|						if (model & 0x8000)
//|							BYTE Incr Counter (increment model by this #)
//|						BYTE[2] xLoc (only use lowest significant 15 bits)
//|						BYTE[2] yLoc
//|						if (xLoc & 0x8000)
//|							BYTE direction
//|						BYTE zLoc
//|						if (yLoc & 0x8000)
//|							BYTE[2] dye
//|						if (yLoc & 0x4000)
//|							BYTE flag byte
//|
//|					Flag:
//|						0x00 - None
//|						0x02 - Female
//|						0x04 - Poisoned
//|						0x08 - YellowHits // healthbar gets yellow
//|						0x10 - FactionShip // unsure why client needs to know
//|						0x20 - Movable if normally not
//|						0x40 - War mode
//|						0x80 - Hidden
//o------------------------------------------------------------------------------------------------o
void CPObjectInfo::InternalReset( void )
{
	pStream.ReserveSize( 16 );
	pStream.WriteByte( 0, 0x1A );
	pStream.WriteShort( 1, 16 );
}
void CPObjectInfo::CopyData( CItem& mItem, CChar& mChar )
{
	pStream.WriteLong( 3, mItem.GetSerial() );

	if( mItem.CanBeObjType( OT_MULTI ))
	{
		CopyMultiData( static_cast<CMultiObj&>( mItem ), mChar );
	}
	else if( mItem.CanBeObjType( OT_ITEM ))
	{
		CopyItemData( mItem, mChar );
	}
}

void CPObjectInfo::CopyItemData( CItem &mItem, CChar &mChar )
{
	if( mItem.IsPileable() || mItem.IsCorpse() )
	{
		pStream.ReserveSize( 18 );
		pStream.WriteByte( 2, 18 );
		pStream.WriteByte( 3, ( pStream.GetByte( 3 ) | 0x80 ));	// Enable piles
	}

	bool isInvisible	= ( mItem.GetVisible() != VT_VISIBLE );
	bool isMovable		= ( mItem.GetMovable() == 1 || mChar.AllMove() || ( mItem.IsLockedDown() && &mChar == mItem.GetOwnerObj() ));
	// if player is a gm, this item
	// is shown like a candle (so that he can move it),
	// ....if not, the item is a normal
	// invisible light source!
	if( mChar.IsGM() && mItem.GetId() == 0x1647 )
	{
		pStream.WriteShort( 7, 0x0A0F );
	}
	else
	{
		pStream.WriteShort( 7, mItem.GetId() );
	}

	UI08 byteNum = 7;
	if( mItem.IsPileable() || mItem.IsCorpse() )
	{
		pStream.WriteShort(  byteNum += 2, mItem.GetAmount() );
	}

	pStream.WriteShort( byteNum += 2, mItem.GetX() );
	if( isInvisible || isMovable )
	{
		pStream.WriteShort( byteNum += 2, ( mItem.GetY() | 0xC000 ));
		pStream.ReserveSize( pStream.GetSize() + 1 );
		pStream.WriteByte( 2, pStream.GetByte( 2 ) + 1 );
	}
	else
	{
		pStream.WriteShort( byteNum += 2, ( mItem.GetY() | 0x8000 ));
	}
	if( mItem.GetDir() )
	{
		pStream.ReserveSize( pStream.GetSize() + 1 );
		pStream.WriteByte( byteNum - 2, ( pStream.GetByte( byteNum-2 ) | 0x80 ));	// Enable direction
		pStream.WriteByte( byteNum += 2, mItem.GetDir() );
		pStream.WriteByte( ++byteNum, mItem.GetZ() );
		pStream.WriteByte( 2, pStream.GetByte( 2 ) + 1 );
	}
	else
	{
		pStream.WriteByte( byteNum += 2, mItem.GetZ() );
	}

	if( mChar.IsGM() && mItem.GetId() == 0x1647 )
	{
		pStream.WriteShort( ++byteNum, 0x00C6 );
	}
	else
	{
		pStream.WriteShort( ++byteNum, mItem.GetColour() );
	}

	if( isInvisible || isMovable )
	{
		pStream.WriteByte( byteNum += 2, 0x00 );
		if( isInvisible )
		{
			pStream.WriteByte( byteNum, ( pStream.GetByte( byteNum ) | 0x80 ));
		}

		if( isMovable )
		{
			pStream.WriteByte( byteNum, ( pStream.GetByte( byteNum ) | 0x20 ));
		}
	}
}

void CPObjectInfo::CopyMultiData( CMultiObj& mMulti, CChar &mChar )
{
	bool isInvisible	= ( mMulti.GetVisible() != VT_VISIBLE );
	bool isMovable		= ( mChar.AllMove() );

	if( mChar.ViewHouseAsIcon() )
	{
		pStream.WriteShort( 7, 0x14F0 );
	}
	else
	{
		pStream.WriteShort( 7, mMulti.GetId() );
	}

	pStream.WriteShort( 9, mMulti.GetX() );
	if( isInvisible || isMovable )
	{
		pStream.WriteShort( 11, ( mMulti.GetY() | 0xC000 ));
		pStream.ReserveSize( pStream.GetSize() + 1 );
		pStream.WriteByte( 2, pStream.GetByte( 2 ) + 1 );
	}
	else
	{
		pStream.WriteShort( 11, ( mMulti.GetY() | 0x8000 ));
	}
	UI08 byteNum = 12;
	if( mMulti.GetDir() )
	{
		pStream.ReserveSize( pStream.GetSize() + 1 );
		pStream.WriteByte( 2, pStream.GetByte( 2 ) + 1 );
		pStream.WriteByte( 9, ( pStream.GetByte( 9 ) | 0x80 ));	// Enable direction
		pStream.WriteByte( ++byteNum, mMulti.GetDir() );
	}
	pStream.WriteByte( ++byteNum, mMulti.GetZ() );

	pStream.WriteShort( ++byteNum, mMulti.GetColour() );
	if( isInvisible || isMovable )
	{
		pStream.WriteByte( ++byteNum, 0x00 );
		if( isInvisible )
		{
			pStream.WriteByte( byteNum, ( pStream.GetByte( byteNum ) | 0x80 ));
		}

		if( isMovable )
		{
			pStream.WriteByte( byteNum, ( pStream.GetByte( byteNum ) | 0x20 ));
		}
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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPUnicodeSpeech()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet with unicode speech message
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xAE (Unicode Speech message)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] blockSize
//|						BYTE[4] ID
//|						BYTE[2] Model
//|						BYTE Type
//|						BYTE[2] Color
//|						BYTE[2] Font
//|						BYTE[4] Language
//|						BYTE[30] Name
//|						BYTE[?][2] Msg – Null Terminated (blockSize - 48)
//|
//|					The various types of text is as follows:
//|						0x00 - Normal
//|						0x01 - Broadcast/System
//|						0x02 - Emote
//|						0x06 - System/Lower Corner
//|						0x07 - Message/Corner With Name
//|						0x08 - Whisper
//|						0x09 - Yell
//|						0x0A - Spell
//|						0x0D - Guild Chat
//|						0x0E - Alliance Chat
//|						0x0F - Command Prompts
//o------------------------------------------------------------------------------------------------o
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
	pStream.WriteByte( 9, ( value & 0x0F ));
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
void CPUnicodeSpeech::Language( const char *value )
{
	pStream.WriteString( 14, value, 4 );
}
void CPUnicodeSpeech::Lanaguge( const std::string& value )
{
	pStream.WriteString( 14, value.c_str(), 4 );
}
void CPUnicodeSpeech::Name( std::string value )
{
	pStream.WriteString( 18, value, 30 );
}
void CPUnicodeSpeech::Message( const char *value )
{
	size_t length = strlen( value );
	SetLength( static_cast<UI16>( 48 + ( 2 * length ) + 2 ));
	for( size_t i = 0; i < length; ++i )
	{
		pStream.WriteByte( 48 + i * 2, value[i] );
	}
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
CPUnicodeSpeech &CPUnicodeSpeech::operator = ( CBaseObject &toCopy )
{
	CopyData( toCopy );
	return ( *this );
}
void CPUnicodeSpeech::CopyData( CBaseObject &toCopy )
{
	Serial( toCopy.GetSerial() );
	ID( toCopy.GetId() );

	std::string charName = toCopy.GetName();
	if( charName == "#" )
	{
		// If character name is #, display default name from dictionary files instead - using base entry 3000 + character's ID
		charName = Dictionary->GetEntry( 3000 + toCopy.GetId() );
	}
	Name( charName );
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

	SetLength( 48 + ( 2 * length ));
	pStream.WriteArray( 48, (UI08 *)uniTxt, ( 2 * static_cast<size_t>( length )));
}
void CPUnicodeSpeech::Serial( SERIAL toSet )
{
	pStream.WriteLong( 3, toSet );
}
void CPUnicodeSpeech::ID( UI16 toSet )
{
	pStream.WriteShort( 7, toSet );
}

void CPUnicodeSpeech::GhostIt( [[maybe_unused]] UI08 method )
{
	// Method ignored currently
	// Designed with the idea that you can garble text in more than one way
	// eg 0 == ghost, 1 == racial, 2 == magical, etc etc
	for( UI16 j = 49; j < pStream.GetSize() - 1; j += 2 )
	{
		if( pStream.GetByte( j ) != 32 )
		{
			pStream.WriteByte( j, ( RandomNum( 0, 1 ) == 0 ? 'O' : 'o' ));
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPUnicodeMessage()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet with unicode message from server
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xAE (Unicode Speech message)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] blockSize
//|						BYTE[4] ID
//|						BYTE[2] Model
//|						BYTE Type
//|						BYTE[2] Color
//|						BYTE[2] Font
//|						BYTE[4] Language
//|						BYTE[30] Name
//|						BYTE[?][2] Msg – Null Terminated (blockSize - 48)
//|
//|					The various types of text is as follows:
//|						0x00 - Normal
//|						0x01 - Broadcast/System
//|						0x02 - Emote
//|						0x06 - System/Lower Corner
//|						0x07 - Message/Corner With Name
//|						0x08 - Whisper
//|						0x09 - Yell
//|						0x0A - Spell
//|						0x0D - Guild Chat
//|						0x0E - Alliance Chat
//|						0x0F - Command Prompts
//o------------------------------------------------------------------------------------------------o
CPUnicodeMessage::CPUnicodeMessage()
{
	InternalReset();
}
CPUnicodeMessage::CPUnicodeMessage( CBaseObject &toCopy )
{
	InternalReset();
	CopyData( toCopy );
}
void CPUnicodeMessage::InternalReset( void )
{
	SetLength( 48 );
	pStream.WriteByte( 0, 0xAE );
	Language( "ENU" );
}
CPUnicodeMessage &CPUnicodeMessage::operator = ( CBaseObject &toCopy )
{
	CopyData( toCopy );
	return ( *this );
}
void CPUnicodeMessage::CopyData( CBaseObject &toCopy )
{
	Serial( toCopy.GetSerial() );
	ID( toCopy.GetId() );

	std::string charName = toCopy.GetName();
	if( charName == "#" )
	{
		// If character name is #, display default name from dictionary files instead - using base entry 3000 + character's ID
		charName = Dictionary->GetEntry( 3000 + toCopy.GetId() );
	}
	Name( charName );
}
void CPUnicodeMessage::Object( CBaseObject &talking )
{
	CopyData( talking );
}
void CPUnicodeMessage::Type( UI08 value )
{
	pStream.WriteByte( 9, (value & 0x0F) );
}
void CPUnicodeMessage::Colour( COLOUR value )
{
	pStream.WriteShort( 10, value );
}
void CPUnicodeMessage::Font( UI16 value )
{
	pStream.WriteShort( 12, value );
}
void CPUnicodeMessage::Language( char *value )
{
	pStream.WriteString( 14, value, 4 );
}
void CPUnicodeMessage::Language( const char *value )
{
	pStream.WriteString( 14, value, 4 );
}
void CPUnicodeMessage::Lanaguge( const std::string& value )
{
	pStream.WriteString( 14, value.c_str(), 4 );
}
void CPUnicodeMessage::Name( std::string value )
{
	pStream.WriteString( 18, value, 30 );
}
void CPUnicodeMessage::Message( const char *value )
{
	size_t length = strlen( value );
	SetLength( static_cast<UI16>( 48 + ( 2 * length ) + 2 ));
	for(size_t i = 0; i < length; ++i)
	{
		pStream.WriteByte( 49 + i * 2, value[i] );
	}
}
void CPUnicodeMessage::Message( const std::string msg )
{
	std::wstring wMsg = oldstrutil::stringToWstring( msg );
	if( wMsg != L"" )
	{
		const size_t stringLen = wMsg.size();

		const UI16 packetLen = static_cast<UI16>( 48 + ( stringLen * 2 ) + 2 );
		SetLength( packetLen );

		for(size_t i = 0; i < stringLen; ++i)
		{
			pStream.WriteByte( 49 + i * 2, wMsg[i] );
		}
	}
	else
	{
		const size_t stringLen = msg.size();

		const UI16 packetLen = static_cast<UI16>( 48 + ( stringLen * 2 ) + 2 );
		SetLength( packetLen );

		for(size_t i = 0; i < stringLen; ++i)
		{
			pStream.WriteByte( 49 + i * 2, msg[i] );
		}
	}
}
void CPUnicodeMessage::SetLength( UI16 value )
{
	pStream.ReserveSize( value );
	pStream.WriteShort( 1, value );
}
void CPUnicodeMessage::Serial( SERIAL toSet )
{
	pStream.WriteLong( 3, toSet );
}
void CPUnicodeMessage::ID( UI16 toSet )
{
	pStream.WriteShort( 7, toSet );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPGameServerList()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet with gameserver list to display in client during login
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xA8 (Game Server List)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] blockSize
//|						BYTE System Info Flag
//|							0xCC - Don't send
//|							0x5D - ???
//|							0x64 - Send Video card
//|							?? -
//|						BYTE[2] # of servers
//|						Then each server --
//|							BYTE[2] serverIndex (0-based)
//|							BYTE[32] serverName
//|							BYTE percentFull
//|							BYTE timezone
//|							BYTE[4] pingIP
//|
//|					Note: System Info Flags: 0xCC - Do not send video card info. 0x64 - Send Video card.
//|					Note: Server IP has to be sent in reverse order. For example, 192.168.0.1 is sent as 0100A8C0.
//o------------------------------------------------------------------------------------------------o
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
	pStream.WriteShort( 1, static_cast<UI16>( tlen ));
	pStream.WriteShort( 4, numItems );
}
void CPGameServerList::AddServer( UI16 servNum, PhysicalServer *data )
{
	UI32 baseOffset = 6 + servNum * 40;
	pStream.WriteShort(  baseOffset, servNum + 1 );
	pStream.WriteString( static_cast<size_t>( baseOffset ) + 2, data->GetName(), data->GetName().length() );
	auto ip = UI32( 0 );

	inet_pton( AF_INET, data->GetIP().c_str(), &ip );
	
	//UI32 ip = inet_addr( data->GetIP().c_str() );
	pStream.WriteLong( static_cast<size_t>( baseOffset ) + 36, ip );
}
void CPGameServerList::addEntry( const std::string & name, UI32 addressBig )
{
	pStream.WriteShort( 1, 46 );
	pStream.WriteShort( 4, 1 );
	pStream.WriteShort( 6, 0 );
	UI32 baseOffset = 6;
	pStream.WriteString( static_cast<size_t>( baseOffset ) + 2, name.c_str(), name.size() );
	pStream.WriteLong( static_cast<size_t>( baseOffset ) + 36, addressBig );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPSecureTrading()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet with server response to secure trading request
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x6F (Secure Trading)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] blockSize
//|						BYTE action
//|							0x00: Start
//|							0x01: Cancel
//|							0x02: Update
//|						BYTE[4] id1
//|						BYTE[4] id2
//|						BYTE[4] id3
//|						BYTE nameFollowing (0 or 1)
//|						If (nameFollowing = 1)
//|							BYTE[?] charName
//o------------------------------------------------------------------------------------------------o
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
	pStream.WriteLong( 8,  mItem2 );
	pStream.WriteLong( 12, mItem3 );
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
void CPSecureTrading::Name( const std::string& nameFollowing )
{
	pStream.ReserveSize( 47 );
	pStream.WriteByte(	 2, 47 );
	pStream.WriteByte(   16, 1 );
	if( nameFollowing.length() >= 30 )
	{
		pStream.WriteString( 17, nameFollowing, 30 );
	}
	else
	{
		pStream.WriteString( 17, nameFollowing, nameFollowing.length() );
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPAllNames3D()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet with server response to all names request
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x98 (All-names “3D”)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] blocksize
//|						BYTE[4] ID
//|						If (server-reply)
//|							BYTE[30] name (0 terminated)
//|
//|						NOTE: Only 3D clients (used to) send this packet. Now both 2D and 3D do!
//|						Server and client packet.
//|
//|						Client asks for name of object with ID x.
//|						Server has to reply with ID + name
//|						Client automatically knows names of items.
//|						Hence it only asks only for NPC/Player names nearby, but shows bars of items plus NPC’s.
//|
//|						Client request has 7 bytes, server-reply 37
//|						Triggered by Crtl + Shift.
//o------------------------------------------------------------------------------------------------o
void CPAllNames3D::InternalReset( void )
{
	pStream.ReserveSize( 37 );
	pStream.WriteByte( 0, 0x98 );
	pStream.WriteByte( 2, 37 );
}
void CPAllNames3D::CopyData( CBaseObject& obj )
{
	pStream.WriteLong(   3, obj.GetSerial() );

	CChar *objChar = CalcCharObjFromSer( obj.GetSerial() );
	std::string objName = GetNpcDictName( objChar, nullptr, NRS_SINGLECLICK );
	pStream.WriteString( 7, objName, objName.length() );
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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPBookPage()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet with server response to request for book pages
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x66 (Books - Page)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] blockSize
//|						BYTE[4] bookID
//|						BYTE[2] # of pages in this packet
//|						For each page:
//|							BYTE[2] page #
//|							BYTE[2] # of lines on page
//|							Repeated for each line:
//|								BYTE[var] null terminated line
//|						Note:
//|						server side: # of pages equals value given in 0x93/0xd4
//|						EACH page # given. If empty: # lines: 0 + terminator (=3 0’s)
//|						client side:  # of pages always 1. if 2 pages changed, client generates 2 packets.
//o------------------------------------------------------------------------------------------------o
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
	{
		pStream.WriteShort( baseOffset, pageCount );
	}
	else
	{
		pStream.WriteShort( baseOffset, pNum );
	}
	pStream.WriteByte( static_cast<size_t>( baseOffset ) + 3, 8 );	// 8 lines per page
}
void CPBookPage::AddLine( const std::string& line )
{
	UI16 baseOffset = bookLength;
	size_t strLen	= line.length() + 1;
	IncLength( static_cast<UI08>( strLen ));
	pStream.WriteString( baseOffset, line, line.length() );
}
// Why do we use pointersfor collections. References are wonder things
void CPBookPage::NewPage( SI16 pNum, const std::vector<std::string> *lines )
{
	++pageCount;	// 1 based counter
	UI16 baseOffset = bookLength;
	IncLength( 4 );
	if( pNum == -1 )
	{
		pStream.WriteShort( baseOffset, pageCount );
	}
	else
	{
		pStream.WriteShort( baseOffset, pNum );
	}
	pStream.WriteByte( static_cast<size_t>( baseOffset + 3 ), static_cast<UI08>( lines->size() ));	// 8 lines per page

	std::for_each( lines->begin(), lines->end(), [this]( const std::string &entry )
	{
		AddLine( entry );
	});
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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPSendGumpMenu()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet with gump menu details
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xB0 (Send Gump Menu Dialog)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] blockSize
//|						BYTE[4] id
//|						BYTE[4] gumpid
//|						BYTE[4] x
//|						BYTE[4] y
//|						BYTE[2] command section length
//|						BYTE[?] commands (zero terminated)
//|						BYTE[2] numTextLines
//|						BYTE[2] text length (in unicode (2 byte) characters.)
//|						BYTE[?] text (in unicode)
//o------------------------------------------------------------------------------------------------o
CPSendGumpMenu::CPSendGumpMenu()
{
	pStream.ReserveSize( 21 );
	pStream.WriteByte( 0, 0xB0 );		// command byte
	pStream.WriteByte( 14, 0x6E );		// default x
	pStream.WriteByte( 18, 0x46 );		// default y
}
void CPSendGumpMenu::UserId( SERIAL value )
{
	pStream.WriteLong( 3, value );
}
void CPSendGumpMenu::GumpId( SERIAL value )
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
void CPSendGumpMenu::addCommand( const std::string& msg )
{
	if( msg.empty() )
	{
		throw new std::runtime_error( "Blank command field added!" );
	}
	auto temp = msg;
	if( temp.size() > 1024 )
	{
		temp = temp.substr( 0, 1024 );
	}

#if defined( UOX_DEBUG_MODE )
	Console << temp << myendl;
#endif
	commands.push_back( temp );
}

void CPSendGumpMenu::addText( const std::string& msg )
{
	if( msg.empty() || msg.size() == 0 )
	{
		//throw new std::runtime_error( "Blank text field added!" );
		Console.Error( "Blank text field added!" );
		return;
	}
	auto temp = msg;
	if( temp.size() > 512 )
	{
		temp = temp.substr( 0, 512 );
	}
#if defined( UOX_DEBUG_MODE )
	Console << temp << myendl;
#endif

	text.push_back( temp );
}

void CPSendGumpMenu::Finalize( void )
{
	UI32 length		= 21;
	UI16 increment	= 0;
	UI16 lineLen	= 0;

	std::string cmdString;
	for( auto &entry : commands )
	{
		if( !entry.empty() )
		{
			lineLen = static_cast<UI16>( entry.length() );
			increment = static_cast<UI16>( lineLen + 4 );
			if(( length + increment ) >= 0xFFFF )
			{
				Console.Warning( "SendGump Packet (0xB0) attempted to send a packet that exceeds 65355 bytes!" );
				break;
			}

			pStream.ReserveSize( static_cast<size_t>( length ) + static_cast<size_t>( increment ));
			cmdString = "{ "s + entry + " }"s;
			pStream.WriteString( length, cmdString, increment );
			length	+= increment;
		}
	}

	if( length > 65536 )
	{
		throw std::runtime_error( "Packet 0xB0 is far too large" );
	}

	pStream.WriteShort( 19, static_cast<UI16>(length-20) );

	UI32 tlOff	= length + 1;
	length		+= 3;
	UI32 tlines = 0;

	pStream.ReserveSize( length );	// match the 3 byte increase
	for( auto &entry : text )
	{
		if( !entry.empty() )
		{
			lineLen = static_cast<UI16>( entry.length() );
			// Unfortunately, unicode strings are... different
			// so we can't use PackString
			increment	= lineLen * 2 + 2;
			if(( length + increment ) >= 0xFFFF )
			{
				Console.Warning( "SendGump Packet (0xB0) attempted to send a packet that exceeds 65355 bytes!" );
				break;
			}

			pStream.ReserveSize( static_cast<size_t>( length ) + static_cast<size_t>( increment ));
			pStream.WriteShort( length, lineLen );
			for( UI16 i = 0; i < lineLen; ++i )
			{
				pStream.WriteByte( static_cast<size_t>( length ) + 3 + static_cast<size_t>( i ) * 2, entry[i] );
			}
			length += increment;
			++tlines;
		}
	}

	if( length > 65536 )
	{
		throw std::runtime_error( "Packet 0xB0 is far too large" );
	}

	pStream.WriteShort( 1, static_cast<UI16>( length ));
	pStream.WriteShort( tlOff, static_cast<UI16>( tlines ));
}

void CPSendGumpMenu::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[SEND]Packet     : CPSendGumpMenu 0xB0 --> Length: " << pStream.GetSize() << TimeStamp() << std::endl;
	}
	outStream << "ID               : " << std::hex << pStream.GetULong( 3 ) << std::endl;
	outStream << "GumpId           : " << std::hex << pStream.GetULong( 7 ) << std::endl;
	outStream << "X                : " << std::hex << pStream.GetUShort( 11 ) << std::endl;
	outStream << "Y                : " << std::hex << pStream.GetUShort( 15 ) << std::endl;
	outStream << "Command Sec Len  : " << std::dec << pStream.GetUShort( 19 ) << std::endl;
	outStream << "Commands         : " << std::endl;
	for( size_t x = 0; x < commands.size(); ++x )
	{
		outStream << "     " << commands[x] << std::endl;
	}
	outStream << "Text             : " << std::endl;
	for( size_t y = 0; y < text.size(); ++y )
	{
		outStream << "     " << text[y] << std::endl;
	}

	outStream << "  Raw dump     :" << std::endl;
	CPUOXBuffer::Log( outStream, false );
}

//o-----------------------------------------------------------------------------------------------o
//o------------------------------------------------------------------------------------------------o
//| Function	-	CPNewSpellBook()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to send spellbook content
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xBF (General Information Packet)
//|					Subcommand: 0x1B (New Spellbook)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd (0xBF)
//|						BYTE[2] Length
//|						BYTE[2] Subcommand (0x1B)
//|						Subcommand details
//|							BYTE[2] unknown, always 1
//|							BYTE[4] Spellbook serial
//|							BYTE[2] Item Id
//|							BYTE[2] scroll offset
//|								1 = regular
//|								101 = necro
//|								201 = paladin
//|								401 = bushido
//|								501 = ninjitsu
//|								601 = spellweaving
//|							BYTE[8] spellbook content
//|								first bit of first byte = spell #1
//|								second bit of first byte = spell #2
//|								first bit of second byte = spell #8, etc
//o------------------------------------------------------------------------------------------------o
void CPNewSpellBook::InternalReset( CItem &obj )
{
    // Reset the base buffer
    CPUOXBuffer::InternalReset();

	// Determine the offset based on book type
    int offset = 1; // Default to regular spellbook
    if( obj.GetType() == IT_PALADINBOOK )
        offset = 201;
    else if( obj.GetType() == IT_NECROBOOK )
        offset = 101;

    pStream.ReserveSize( 23 );
    pStream.WriteByte( 0, 0xBF ); // Main packet
    pStream.WriteShort( 1, 23 );
    pStream.WriteShort( 3, 0x1B ); // Subcommand
    pStream.WriteShort( 5, 0x01 );
    pStream.WriteByte( 11, 0x0E ); // Graphic part I
    pStream.WriteByte( 12, 0xFA ); // Graphic part II
    pStream.WriteShort( 13, offset ); // Offset
}

void CPNewSpellBook::CopyData( CItem &obj )
{
	// Determine the spell range and offset based on book type
    int startSpell = 1, endSpell = 64;
    if( obj.GetType() == IT_PALADINBOOK )
    {
        startSpell = 201;
        endSpell = 210;
    }
    else if( obj.GetType() == IT_NECROBOOK )
    {
        startSpell = 101;
        endSpell = 117;
    }

    int spellCount = endSpell - startSpell + 1;

    // Calculate the total size dynamically based on spell count
    int spellBytes = ( spellCount + 7 ) / 8; // Each byte holds 8 spells
    int packetSize = 15 + spellBytes;

    // Reserve space and write header
    pStream.ReserveSize( packetSize );
    pStream.WriteByte( 0, 0xBF );          // Packet ID
    pStream.WriteShort( 1, packetSize );   // Packet size
    pStream.WriteShort( 3, 0x1B );         // Subcommand
    pStream.WriteShort( 5, 0x01 );         // Unknown, always 1
    pStream.WriteLong( 7, obj.GetSerial() ); // Spellbook serial
    pStream.WriteShort( 11, obj.GetId() );  // Graphic ID
    pStream.WriteShort( 13, startSpell );   // Scroll offset (start spell ID)

    // Initialize spell content bytes to 0
    for( int i = 0; i < spellBytes; ++i )
    {
        pStream.WriteByte( 15 + i, 0x00 );
    }

    // Populate the spell content bitfield
    for( int spellNum = startSpell; spellNum <= endSpell; ++spellNum )
    {        
        int spellIndex = spellNum - startSpell; // Adjust to 0-based index
        int byteIndex = 15 + ( spellIndex / 8 );  // Byte position in the stream
        int bitIndex = spellIndex % 8;         // Bit position within the byte

        if( Magic->HasSpell( &obj, spellNum ))
        {
            // Set the corresponding bit in the spell content
            pStream.WriteByte( byteIndex, pStream.GetByte( byteIndex ) | ( 1 << bitIndex ));
        }
    }
}

CPNewSpellBook::CPNewSpellBook( CItem& obj )
{
	InternalReset( obj );
	CopyData( obj );
}

bool CPNewSpellBook::ClientCanReceive( CSocket *mSock )
{
	if( mSock == nullptr )
		return false;

	switch( mSock->ClientType() )
	{
		case CV_DEFAULT:
		case CV_T2A:
		case CV_UO3D:
			if( mSock->ClientVersionMajor() < 4 )
			{
				return false;
			}
			break;
		default:
			break;
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPDisplayDamage()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to show damage numbers above head of characters
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xBF (General Information Packet)
//|					Subcommand: 0x22 (Damage)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd (0xBF)
//|						BYTE[2] Length
//|						BYTE[2] Subcommand (0x22)
//|						Subcommand details
//|							BYTE[2]	unknown, always 1
//|							BYTE[4]	Serial
//|							BYTE Damage // how much damage was done ?
//|
//|						Note: displays damage above the npc/player’s head.
//o------------------------------------------------------------------------------------------------o
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
	pStream.WriteByte( 10, ( ourDamage >> 8 ) + ( ourDamage % 256 ));
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
	if( mSock == nullptr )
		return false;

	switch( mSock->ClientType() )
	{
		case CV_DEFAULT:
		case CV_T2A:
		case CV_UO3D:
			if( mSock->ClientVersionMajor() < 4 )
			{
				return false;
			}
			break;
		default:
			break;
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPQueryToolTip()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet with response to client request for item tooltip
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xBF (General Information Packet)
//|					Subcommand: 0x10 (Display Equipment Info)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd (0xBF)
//|						BYTE[2] Length
//|						BYTE[2] Subcommand (0x10)
//|						Subcommand details
//|							BYTE[4] Serial
//|							BYTE[4] Info Number
//|							BYTE[4] 0xFFFFFFFD (If Owner != null )
//|							BYTE[2] Owner Name Length (If Owner != null )
//|							BYTE[30] Owner Name (If Owner != null )
//|							BYTE[4] 0xFFFFFFFC (If equipment unidentified )
//|							loop Attribute
//|								BYTE[4] Number
//|								BYTE[2] Charges
//|							endloop	Attribute
//|							BYTE[4] 0xFFFFFFFF
//o------------------------------------------------------------------------------------------------o
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
	if( mSock == nullptr )
		return false;

	switch( mSock->ClientType() )
	{
		case CV_DEFAULT:
		case CV_T2A:
		case CV_UO3D:
			if( mSock->ClientVersionMajor() < 4 )
			{
				return false;
			}
			break;
		default:
			break;
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPToolTip()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet with response to AoS client request for item tooltip
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xD6 (AOS tooltip/Object property list)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] length
//|						BYTE[2] unknown1, always 0x001
//|						BYTE[4] serial of item/creature
//|						BYTE unknown2, always 0
//|						BYTE unknown3, always 0
//|						BYTE[4] list ID (see notes)
//|						Loop
//|							BYTE[4] cliloc#
//|							if cliloc# is 0, break loop
//|							BYTE[2] text length
//|							BYTE[text length] little endian Unicode text, not 0 terminated
//|						End Loop
//|						BYTE[4] 00000000 - Sent as end of packet/loop
//o------------------------------------------------------------------------------------------------o
void CPToolTip::InternalReset( void )
{
	pStream.ReserveSize( 15 );
	pStream.WriteByte( 0, 0xD6 );
	pStream.WriteShort( 3, 0x0001 );
	pStream.WriteShort( 9, 0x0000 );
	pStream.WriteLong( 11, 0x00000000 );
}

void CPToolTip::FinalizeData( ToolTipEntry_st tempEntry, size_t &totalStringLen )
{
	tempEntry.stringLen = ( tempEntry.ourText.size() * 2 );
	totalStringLen += ( tempEntry.stringLen + 6 );
	ourEntries.push_back( tempEntry );
}

void CPToolTip::CopyItemData( CItem& cItem, size_t &totalStringLen, bool addAmount, bool playerVendor )
{
	std::string cItemName = cItem.GetNameRequest( tSock->CurrcharObj(), NRS_TOOLTIP );
	ToolTipEntry_st tempEntry = {};
	if( cItem.GetType() == IT_HOUSESIGN )
	{
		tempEntry.ourText = " \tA House Sign\t ";
	}
	else if( cItemName[0] == '#' )
	{
		std::string temp;
		GetTileName( cItem, temp );
		if( cItem.GetAmount() > 1 && addAmount )
		{
			tempEntry.ourText = oldstrutil::format( " \t%s : %i\t ", temp.c_str(), cItem.GetAmount() );
		}
		else
		{
			tempEntry.ourText = oldstrutil::format( " \t%s\t ", temp.c_str() );
		}
	}
	else
	{
		if( cItem.GetAmount() > 1 && !cItem.IsCorpse() && addAmount && cItem.GetType() != IT_SPAWNCONT && cItem.GetType() != IT_LOCKEDSPAWNCONT && cItem.GetType() != IT_UNLOCKABLESPAWNCONT )
		{
			tempEntry.ourText = oldstrutil::format( " \t%s : %i\t ", cItemName.c_str(), cItem.GetAmount() );
		}
		else
		{
			if( cwmWorldState->ServerData()->RankSystemStatus() && cItem.GetRank() == 10 )
			{
				tempEntry.ourText = oldstrutil::format(" \t%s %s\t ", cItemName.c_str(), Dictionary->GetEntry( 9140, tSock->Language() ).c_str() ); // %s of exceptional quality
			}
			else
			{
				if( cItem.IsCorpse() )
				{
					std::string iFlagColor;
					CChar *iOwner = cItem.GetOwnerObj();
					if( ValidateObject( iOwner ))
					{
						// Match up corpse's flag color to that of the owner
						iFlagColor = tSock->GetHtmlFlagColour( tSock->CurrcharObj(), iOwner );
					}
					else
					{
						// No corpse owner, corpse from NPC/monster
						UI08 flag = cItem.GetTempVar( CITV_MOREZ ); // Get flag from morez value on corpse, to help determine color of corpse
						switch( flag )
						{
							case 0x01:	iFlagColor = "#FF0000";	break;	// Murderer, red
							case 0x02:	// Criminal, gray
								[[fallthrough]];
							case 0x08:	iFlagColor = "#808080";	break;	// Neutral, gray
							default:
							case 0x04:	iFlagColor = "#00FFFF";	break;	// Innocent, blue
						}
					}

					tempEntry.ourText = oldstrutil::format( " \t<color=%s>%s</color>\t ", iFlagColor.c_str(), cItemName.c_str() );
				}
				else
				{
					tempEntry.ourText = oldstrutil::format( " \t%s\t ", cItemName.c_str() );
				}
			}
		}
	}

	tempEntry.stringNum = 1050045; // ~1_PREFIX~~2_NAME~~3_SUFFIX~
	FinalizeData( tempEntry, totalStringLen );

	// Maker's mark
	if( cwmWorldState->ServerData()->RankSystemStatus() && cwmWorldState->ServerData()->DisplayMakersMark() && cItem.GetRank() == 10 
		&& cItem.GetCreator() != INVALIDSERIAL && cItem.IsMarkedByMaker() )
	{
		CChar *cItemCreator = CalcCharObjFromSer( cItem.GetCreator() );
		if( ValidateObject( cItemCreator ))
		{
			tempEntry.stringNum = 1042971; // ~1_NOTHING~
			tempEntry.ourText = oldstrutil::format( "%s by %s", cwmWorldState->skill[cItem.GetMadeWith()-1].madeWord.c_str(), cItemCreator->GetName().c_str() ); // tailored/tinkered/forged by %s
																																								 //tempEntry.ourText = oldstrutil::format( "%s %s", Dictionary->GetEntry( 9141, tSock->Language() ).c_str(), cItemCreator->GetName().c_str() ); // Crafted by %s
			FinalizeData( tempEntry, totalStringLen );
		}
	}

	if( cItem.IsLockedDown() )
	{
		if( ValidateObject( cItem.GetMultiObj() ) && cItem.GetMultiObj()->IsSecureContainer( &cItem ))
		{
			// Locked down and secure
			tempEntry.stringNum = 501644; // locked down & secure
			FinalizeData( tempEntry, totalStringLen );
		}
		else
		{
			// Locked down
			tempEntry.stringNum = 501643; // locked down
			FinalizeData( tempEntry, totalStringLen );
		}
	}
	if( cItem.IsGuarded() )
	{
		CTownRegion *itemTownRegion = CalcRegionFromXY( cItem.GetX(), cItem.GetY(), cItem.WorldNumber(), cItem.GetInstanceId() );
		if( !itemTownRegion->IsGuarded() && !itemTownRegion->IsSafeZone() )
		{
			tempEntry.stringNum = 1042971; // ~1_NOTHING~
			tempEntry.ourText = oldstrutil::format( "%s", Dictionary->GetEntry( 9051, tSock->Language() ).c_str() ); // [Guarded]
			FinalizeData( tempEntry, totalStringLen );
		}
	}
	if( cItem.IsNewbie() )
	{
		tempEntry.stringNum = 1042971; // ~1_NOTHING~
		tempEntry.ourText = oldstrutil::format( "%s", Dictionary->GetEntry( 9055, tSock->Language() ).c_str() ); // [Blessed]
		FinalizeData( tempEntry, totalStringLen );
	}
	if( cItem.GetType() == IT_LOCKEDDOOR )
	{
		tempEntry.stringNum = 1042971; // ~1_NOTHING~
		tempEntry.ourText = oldstrutil::format( "%s", Dictionary->GetEntry( 9050, tSock->Language() ).c_str() ); // [Locked]
		FinalizeData( tempEntry, totalStringLen );
	}

	// Custom item tooltips
	std::vector<UI16> scriptTriggers = cItem.GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute1 = JSMapping->GetScript( scriptTrig );
		if( toExecute1 != nullptr )
		{
			// Custom tooltip - each word capitalized
			std::string textFromScript1 = toExecute1->OnTooltip( &cItem, tSock );
			if( !textFromScript1.empty() )
			{
				UI32 clilocNumFromScript = 0;
				TAGMAPOBJECT tempTagObj = cItem.GetTempTag( "tooltipCliloc" );
				if( tempTagObj.m_ObjectType == TAGMAP_TYPE_INT && tempTagObj.m_IntValue > 0 )
				{
					// Use cliloc set in tooltipCliloc temptag, if present
					clilocNumFromScript = tempTagObj.m_IntValue;
				}
				else
				{
					// Fallback to this cliloc if no other cliloc was set in temptag for object
					clilocNumFromScript = 1042971; // ~1_NOTHING~
				}
				tempEntry.stringNum = clilocNumFromScript;
				tempEntry.ourText = textFromScript1;
				FinalizeData( tempEntry, totalStringLen );
			}
		}
	}

	// Also check the global script!
	cScript *toExecuteGlobal = JSMapping->GetScript( static_cast<UI16>( 0 ));
	if( toExecuteGlobal != nullptr )
	{
		std::string textFromGlobalScript = toExecuteGlobal->OnTooltip( &cItem, tSock );
		if( !textFromGlobalScript.empty() )
		{
			UI32 clilocNumFromScript = 0;
			TAGMAPOBJECT tempTagObj = cItem.GetTempTag( "tooltipCliloc" );
			if( tempTagObj.m_ObjectType == TAGMAP_TYPE_INT && tempTagObj.m_IntValue > 0 )
			{
				// Use cliloc set in tooltipCliloc temptag, if present
				clilocNumFromScript = tempTagObj.m_IntValue;
			}
			else
			{
				// Fallback to this cliloc if no other cliloc was set in temptag for object
				clilocNumFromScript = 1042971; // ~1_NOTHING~
			}
			tempEntry.stringNum = clilocNumFromScript;
			tempEntry.ourText = textFromGlobalScript;
			FinalizeData( tempEntry, totalStringLen );
		}
	}

	CChar *mChar = tSock->CurrcharObj();
	if( cItem.GetType() == IT_CONTAINER || cItem.GetType() == IT_LOCKEDCONTAINER )
	{	
		if( cItem.GetType() == IT_LOCKEDCONTAINER  )
		{
			tempEntry.stringNum = 1042971; // ~1_NOTHING~
			tempEntry.ourText = oldstrutil::format( "%s", Dictionary->GetEntry( 9050 ).c_str(), tSock->Language() ); // [Locked]
			FinalizeData( tempEntry, totalStringLen );
		}

		tempEntry.stringNum = 1050044; // ~1_COUNT~ items, ~2_WEIGHT~ stones
		//tempEntry.ourText = oldstrutil::format( "%u\t%i", cItem.GetContainsList()->Num(), ( cItem.GetWeight() / 100 ));
		tempEntry.ourText = oldstrutil::format( "%u\t%i", GetTotalItemCount( &cItem ), ( cItem.GetWeight() / 100 ));
		FinalizeData( tempEntry, totalStringLen );

		if(( cItem.GetWeightMax() / 100 ) >= 1 )
		{
			tempEntry.stringNum = 1072226; // Capacity: ~1_COUNT~ items, ~2_WEIGHT~ stones
			tempEntry.ourText = oldstrutil::format( "%u\t%i", cItem.GetMaxItems(), ( cItem.GetWeightMax() / 100 ));
			//tempEntry.stringNum = 1060658;
			//tempEntry.ourText = oldstrutil::format( "Capacity\t%i Stones", ( cItem.GetWeightMax() / 100 ));
			FinalizeData( tempEntry, totalStringLen );
		}
	}
	else if( cItem.GetType() == IT_LOCKEDSPAWNCONT )
	{
			tempEntry.stringNum = 1050045; // ~1_PREFIX~~2_NAME~~3_SUFFIX~
			tempEntry.ourText = oldstrutil::format( " \t%s\t ", Dictionary->GetEntry( 9050 ).c_str(), tSock->Language() ); // [Locked]
			FinalizeData( tempEntry, totalStringLen );
	}
	else if( cItem.GetType() == IT_HOUSESIGN )
	{
		tempEntry.stringNum = 1061112; // House Name: ~1_val~
		tempEntry.ourText = cItemName;
		FinalizeData( tempEntry, totalStringLen );

		if( cItem.GetOwnerObj() != nullptr )
		{
			tempEntry.stringNum = 1061113; // Owner: ~1_val~
			tempEntry.ourText = cItem.GetOwnerObj()->GetNameRequest( mChar, NRS_TOOLTIP );
			FinalizeData( tempEntry, totalStringLen );
		}
	}
	else if( !cItem.IsCorpse() && cItem.GetType() != IT_POTION && cItem.GetSectionId() != "potionkeg" && cItem.GetName2() != "#" && cItem.GetName2() != "" )
	{
		tempEntry.stringNum = 1050045; // ~1_PREFIX~~2_NAME~~3_SUFFIX~
		tempEntry.ourText = oldstrutil::format( " \t%s\t ", Dictionary->GetEntry( 9402 ).c_str(), tSock->Language() ); // [Unidentified]
		FinalizeData( tempEntry, totalStringLen );
	}
	else if( cItem.GetType() == IT_RECALLRUNE && cItem.GetTempVar( CITV_MOREX ) != 0 && cItem.GetTempVar( CITV_MOREY ) != 0 )
	{
		// Add a tooltip that says something about where the recall rune is marked
		switch( cItem.GetTempVar( CITV_MORE ))
		{
			case 1: // Trammel
				tempEntry.stringNum = 1050045; // ~1_VAL~
				tempEntry.ourText = "(Trammel)";
				break;
			case 2: // Ilshenar
				tempEntry.stringNum = 1050045; // ~1_VAL~
				tempEntry.ourText = "(Ilshenar)";
				break;
			case 3: // Malas
				tempEntry.stringNum = 1050045; // ~1_VAL~
				tempEntry.ourText = "(Malas)";
				break;
			case 4: // Tokuno
				tempEntry.stringNum = 1050045; // ~1_VAL~
				tempEntry.ourText = "(Tokuno Islands)";
				break;
			case 5: // TerMur
				tempEntry.stringNum = 1050045; // ~1_VAL~
				tempEntry.ourText = "(Ter Mur)";
				break;
			default: // Felucca
				tempEntry.stringNum = 1050045; // ~1_VAL~
				tempEntry.ourText = "(Felucca)";
				break;
		}
		FinalizeData( tempEntry, totalStringLen );
	}
	else if(( cItem.GetWeight() / 100 ) >= 1 && cItem.GetType() != IT_SPAWNCONT && cItem.GetType() != IT_LOCKEDSPAWNCONT && cItem.GetType() != IT_UNLOCKABLESPAWNCONT )
	{
		if(( cItem.GetWeight() / 100 ) == 1 )
		{
			tempEntry.stringNum = 1072788; // Weight: ~1_WEIGHT~ stone
		}
		else
		{
			tempEntry.stringNum = 1072789; // Weight: ~1_WEIGHT~ stones
		}

		if( cItem.GetType() == IT_ITEMSPAWNER || cItem.GetType() == IT_NPCSPAWNER )
		{
			tempEntry.ourText = oldstrutil::number(( cItem.GetWeight() / 100 ));
		}
		else
		{
			CItem *cItemCont = static_cast<CItem *>( cItem.GetCont() );
			if( ValidateObject( cItemCont ) && cItemCont->GetLayer() == IL_SELLCONTAINER )
			{
				tempEntry.ourText = oldstrutil::number(( cItem.GetWeight() / 100 ));
			}
			else
			{
				tempEntry.ourText = oldstrutil::number(( cItem.GetWeight() / 100 ) * cItem.GetAmount() );
			}
		}
		FinalizeData( tempEntry, totalStringLen );
	}

	if( cItem.GetArtifactRarity() > 0)
	{
		tempEntry.stringNum = 1061078; // // artifact rarity ~1_val~
		tempEntry.ourText = oldstrutil::number( cItem.GetArtifactRarity() );
		FinalizeData( tempEntry, totalStringLen );
	}

	if( cItem.GetType() == IT_MAGICWAND && cItem.GetTempVar( CITV_MOREZ ))
	{
		tempEntry.stringNum = 1060584; // uses remaining: ~1_val~
		tempEntry.ourText = oldstrutil::number( cItem.GetTempVar( CITV_MOREZ ));
		FinalizeData( tempEntry, totalStringLen );
	}
	if( cItem.GetType() == IT_SPELLCHANNELING )
	{
		tempEntry.stringNum = 1060482; // spell channeling
		FinalizeData( tempEntry, totalStringLen );
	}

	bool hideMagicItemStats = cwmWorldState->ServerData()->HideStatsForUnknownMagicItems();
	if( !cwmWorldState->ServerData()->BasicTooltipsOnly() && ( !hideMagicItemStats || ( hideMagicItemStats && ( !cItem.GetName2().empty() && cItem.GetName2() == "#"))))
	{
		if( cItem.GetLayer() != IL_NONE )
		{
			if( cItem.GetHiDamage() > 0 )
			{
				if( cwmWorldState->ServerData()->ShowWeaponDamageTypes() )
				{
					if( cItem.GetWeatherDamage( PHYSICAL ))
					{
						tempEntry.stringNum = 1060403; // physical damage ~1_val~%
						tempEntry.ourText = oldstrutil::number( 100 );
						FinalizeData( tempEntry, totalStringLen );
					}
					else if( cItem.GetWeatherDamage( LIGHT ))
					{
						tempEntry.stringNum = 1042971; // ~1_NOTHING~
						tempEntry.ourText = oldstrutil::format( "light damage: 100%" );
						FinalizeData( tempEntry, totalStringLen );
					}
					else if( cItem.GetWeatherDamage( RAIN ))
					{
						tempEntry.stringNum = 1042971; // ~1_NOTHING~
						tempEntry.ourText = oldstrutil::format( "rain damage: 100%" );
						FinalizeData( tempEntry, totalStringLen );
					}
					else if( cItem.GetWeatherDamage( COLD ))
					{
						tempEntry.stringNum = 1060403; // cold damage ~1_val~%
						tempEntry.ourText = oldstrutil::number( 100 );
						FinalizeData( tempEntry, totalStringLen );
					}
					else if( cItem.GetWeatherDamage( HEAT ))
					{
						tempEntry.stringNum = 1042971; // ~1_NOTHING~
						tempEntry.ourText = oldstrutil::format( "fire damage: 100%" );
						FinalizeData( tempEntry, totalStringLen );
					}
					else if( cItem.GetWeatherDamage( LIGHTNING ))
					{
						tempEntry.stringNum = 1060407; // energy damage ~1_val~%
						tempEntry.ourText = oldstrutil::number( 100 );
						FinalizeData( tempEntry, totalStringLen );
					}
					else if( cItem.GetWeatherDamage( POISON ))
					{
						tempEntry.stringNum = 1060406; // energy damage ~1_val~%
						tempEntry.ourText = oldstrutil::number( 100 );
						FinalizeData( tempEntry, totalStringLen );
					}
					else if( cItem.GetWeatherDamage( SNOW ))
					{
						tempEntry.stringNum = 1042971; // ~1_NOTHING~
						tempEntry.ourText = oldstrutil::format( "snow damage: 100%" );
						FinalizeData( tempEntry, totalStringLen );
					}
					else if( cItem.GetWeatherDamage( STORM ))
					{
						tempEntry.stringNum = 1042971; // ~1_NOTHING~
						tempEntry.ourText = oldstrutil::format( "storm damage: 100%" );
						FinalizeData( tempEntry, totalStringLen );
					}
				}

				tempEntry.stringNum = 1061168; // weapon damage ~1_val~ - ~2_val~
				tempEntry.ourText = oldstrutil::format( "%i\t%i", cItem.GetLoDamage(), cItem.GetHiDamage() );
				FinalizeData( tempEntry, totalStringLen );
			}

			if( cItem.GetSpeed() > 0 )
			{
				tempEntry.stringNum = 1061167; // weapon speed ~1_val~
				tempEntry.ourText = oldstrutil::number( cItem.GetSpeed() );
				FinalizeData( tempEntry, totalStringLen );
			}

			if( cItem.GetHiDamage() > 0 )
			{
				if( cItem.GetLayer() == IL_RIGHTHAND )
				{
					tempEntry.stringNum = 1061824; // one-handed weapon
				}
				else
				{
					tempEntry.stringNum = 1061171; // two-handed weapon
				}
				FinalizeData( tempEntry, totalStringLen );
			}

			if( Combat->GetCombatSkill( &cItem ) != WRESTLING )
			{
				switch( Combat->GetCombatSkill( &cItem ))
				{
					case SWORDSMANSHIP:
						tempEntry.stringNum = 1061172; // skill required: swordsmanship
						break;
					case MACEFIGHTING:
						tempEntry.stringNum = 1061173; // skill required: mace fighting
						break;
					case FENCING:
						tempEntry.stringNum = 1061174; // skill required: fencing
						break;
					case ARCHERY:
						tempEntry.stringNum = 1061175; // skill required: archery
						break;
					case THROWING:
						tempEntry.stringNum = 1112075; // skill required: throwing
						break;
				}
				FinalizeData( tempEntry, totalStringLen );
			}

			if( cwmWorldState->ServerData()->ShowItemResistStats() )
			{
				if( cItem.GetResist( PHYSICAL ) > 0 )
				{
					tempEntry.stringNum = 1060448; // physical resist ~1_val~%
					tempEntry.ourText = oldstrutil::number( cItem.GetResist( PHYSICAL ));
					FinalizeData( tempEntry, totalStringLen );
				}

				if( cItem.GetResist( HEAT ) > 0 )
				{
					tempEntry.stringNum = 1060447; // fire resist ~1_val~%
					tempEntry.ourText = oldstrutil::number( cItem.GetResist( HEAT ));
					FinalizeData( tempEntry, totalStringLen );
				}

				if( cItem.GetResist( COLD ) > 0 )
				{
					tempEntry.stringNum = 1060445; // cold resist ~1_val~%
					tempEntry.ourText = oldstrutil::number( cItem.GetResist( COLD ));
					FinalizeData( tempEntry, totalStringLen );
				}

				if( cItem.GetResist( POISON ) > 0 )
				{
					tempEntry.stringNum = 1060449; // poison resist ~1_val~%
					tempEntry.ourText = oldstrutil::number( cItem.GetResist( POISON ));
					FinalizeData( tempEntry, totalStringLen );
				}

				if( cItem.GetResist( LIGHTNING ) > 0 )
				{
					tempEntry.stringNum = 1060446; // energy/electrical resist ~1_val~%
					tempEntry.ourText = oldstrutil::number( cItem.GetResist( LIGHTNING ));
					FinalizeData( tempEntry, totalStringLen );
				}
			}
			else
			{
				if( cItem.GetResist( PHYSICAL ) > 0 )
				{
					tempEntry.stringNum = 1042971; // ~1_NOTHING~
					tempEntry.ourText = oldstrutil::format( "Armor Rating: %s", oldstrutil::number( cItem.GetResist( PHYSICAL )).c_str() );
					FinalizeData( tempEntry, totalStringLen );
				}
			}

			if( cItem.GetMaxHP() > 1 )
			{
				tempEntry.stringNum = 1060639; // durability ~1_val~ / ~2_val~
				tempEntry.ourText = oldstrutil::format( "%i\t%i", cItem.GetHP(), cItem.GetMaxHP() );
				FinalizeData( tempEntry, totalStringLen );
			}

			if( cItem.GetStrength2() > 0 )
			{
				tempEntry.stringNum = 1060485; // strength bonus ~1_val~
				tempEntry.ourText = oldstrutil::number( cItem.GetStrength2() );
				FinalizeData( tempEntry, totalStringLen );
			}
			if( cItem.GetDexterity2() > 0 )
			{
				tempEntry.stringNum = 1060409; // dexterity bonus ~1_val~
				tempEntry.ourText = oldstrutil::number( cItem.GetDexterity2() );
				FinalizeData( tempEntry, totalStringLen );
			}
			if( cItem.GetIntelligence2() > 0 )
			{
				tempEntry.stringNum = 1060432; // intelligence bonus ~1_val~
				tempEntry.ourText = oldstrutil::number( cItem.GetIntelligence2() );
				FinalizeData( tempEntry, totalStringLen );
			}

			if( cItem.GetStrength() > 1 )
			{
				tempEntry.stringNum = 1061170; // strength requirement ~1_val~
				tempEntry.ourText = oldstrutil::number( cItem.GetStrength() );
				FinalizeData( tempEntry, totalStringLen );
			}
		}
	}

	// Address refresh issue when items in player vendor packs received updated pricing and name
	if( cItem.GetCont() != nullptr && cItem.GetCont()->CanBeObjType( OT_ITEM ))
	{
		ObjectType oType = OT_CBO;
		CBaseObject *iOwner	= FindItemOwner( &cItem, oType );
		if( ValidateObject( iOwner ) && iOwner->CanBeObjType( OT_CHAR ))
		{
			if( static_cast<CChar *>( iOwner )->GetNpcAiType() == AI_PLAYERVENDOR )
			{
				playerVendor = true;
			}
		}
	}

	if( playerVendor )
	{
		if( cItem.GetVendorPrice() > 0 )
		{
			// First the price
			tempEntry.stringNum = 1043304; // Price: ~1_COST~
			tempEntry.ourText = oldstrutil::number( cItem.GetVendorPrice() );
			FinalizeData( tempEntry, totalStringLen );
		}
		else
		{
			// Item is not for sale
			tempEntry.stringNum = 1043307; // Price: Not for sale.
			FinalizeData( tempEntry, totalStringLen );
		}

		// Then the description
		tempEntry.stringNum = 1043305; // <br>Seller's Description:<br>"~1_DESC~"
		if( !cItem.GetDesc().empty() )
		{
			// A description has been set for item, use it
			tempEntry.ourText = cItem.GetDesc();
		}
		else
		{
			// No description is set, use default item name
			tempEntry.ourText = cItem.GetName();
		}
		FinalizeData( tempEntry, totalStringLen );
	}
}

void GetFameTitle( CChar *p, std::string& FameTitle );
void CPToolTip::CopyCharData( CChar& mChar, size_t &totalStringLen )
{
	ToolTipEntry_st tempEntry = {};

	// Fame prefix
	std::string fameTitle = "";
	if( cwmWorldState->ServerData()->ShowReputationTitleInTooltip() )
	{
		if( cwmWorldState->creatures[mChar.GetId()].IsHuman() && !mChar.IsIncognito() && !mChar.IsDisguised() )
		{
			GetFameTitle( &mChar, fameTitle );
			fameTitle = oldstrutil::trim( fameTitle );
		}
	}

	// Character Name
	tempEntry.stringNum = 1050045; // ~1_PREFIX~~2_NAME~~3_SUFFIX~
	std::string mCharName = GetNpcDictName( &mChar, tSock, NRS_TOOLTIP );
	std::string convertedString = oldstrutil::stringToWstringToString( mCharName );
	tempEntry.ourText = oldstrutil::format( "%s \t%s\t ", fameTitle.c_str(), convertedString.c_str() );

	FinalizeData( tempEntry, totalStringLen );

	// Show guild title in character tooltip?
	if( cwmWorldState->ServerData()->ShowGuildInfoInTooltip() )
	{
		if( !mChar.IsIncognito() && !mChar.IsDisguised() )
		{
			CGuild *myGuild = GuildSys->Guild( mChar.GetGuildNumber() );
			if( myGuild != nullptr )
			{
				tempEntry.stringNum = 1042971; // ~1_NOTHING~
				auto guildTitle = mChar.GetGuildTitle();
				if( guildTitle != "" )
				{
					tempEntry.ourText = oldstrutil::format( "%s, %s", guildTitle.c_str(), myGuild->Name().c_str() );
				}
				else
				{
					tempEntry.ourText = oldstrutil::format( "%s", myGuild->Name().c_str() );
				}
				FinalizeData( tempEntry, totalStringLen );
			}
		}
	}

	// Show Race in character tooltip?
	if( cwmWorldState->ServerData()->ShowRaceWithName() )
	{
		if( mChar.GetRace() != 0 && mChar.GetRace() != 65535 && cwmWorldState->creatures[mChar.GetId()].IsHuman() ) // need to check for placeholder race
		{
			tempEntry.stringNum = 1042971; // ~1_NOTHING~
			auto raceName = Races->Name( mChar.GetRace() );
			tempEntry.ourText = oldstrutil::format( "%s", ( "("s + raceName + ")"s ).c_str() );
			FinalizeData( tempEntry, totalStringLen );
		}
	}

	// Is character Guarded?
	if( mChar.IsGuarded() )
	{
		CTownRegion *charTownRegion = CalcRegionFromXY( mChar.GetX(), mChar.GetY(), mChar.WorldNumber(), mChar.GetInstanceId() );
		if( !charTownRegion->IsGuarded() && !charTownRegion->IsSafeZone() )
		{
			tempEntry.stringNum = 1042971; // ~1_NOTHING~
			tempEntry.ourText = oldstrutil::format( "%s", Dictionary->GetEntry( 9051, tSock->Language() ).c_str() ); // [Guarded]
			FinalizeData( tempEntry, totalStringLen );
		}
	}

	// Does NPC have a title, and should we show it?
	if( cwmWorldState->ServerData()->ShowNpcTitlesInTooltips() && mChar.IsNpc() && mChar.GetTitle() != "" )
	{
		tempEntry.stringNum = 1042971; // ~1_NOTHING~

		std::string mCharTitle = GetNpcDictTitle( &mChar, tSock );
		convertedString = oldstrutil::stringToWstringToString( mCharTitle );
		tempEntry.ourText = oldstrutil::format( "%s", convertedString.c_str() );

		FinalizeData( tempEntry, totalStringLen );
	}

	// Custom character tooltips
	std::vector<UI16> scriptTriggers = mChar.GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			// Custom tooltip - each word capitalized
			std::string textFromScript = toExecute->OnTooltip( &mChar, tSock );
			if( !textFromScript.empty() )
			{
				UI32 clilocNumFromScript = 0;
				TAGMAPOBJECT tempTagObj = mChar.GetTempTag( "tooltipCliloc" );
				if( tempTagObj.m_ObjectType == TAGMAP_TYPE_INT && tempTagObj.m_IntValue > 0 )
				{
					// Use cliloc set in tooltipCliloc temptag, if present
					clilocNumFromScript = tempTagObj.m_IntValue;
				}
				else
				{
					// Fallback to this cliloc if no other cliloc was set in temptag for object
					clilocNumFromScript = 1042971; // ~1_NOTHING~
				}
				tempEntry.stringNum = clilocNumFromScript;
				tempEntry.ourText = textFromScript;
				FinalizeData( tempEntry, totalStringLen );
			}
		}
	}
}

void CPToolTip::CopyData( SERIAL objSer, bool addAmount, bool playerVendor )
{
	size_t totalStringLen = 0; //total string length

	if( objSer < BASEITEMSERIAL )
	{
		CChar *mChar = CalcCharObjFromSer( objSer );
		if( mChar != nullptr )
		{
			CopyCharData(( *mChar ), totalStringLen );
		}
	}
	else
	{
		CItem *cItem = CalcItemObjFromSer( objSer );
		if( cItem != nullptr )
		{
			CopyItemData(( *cItem ), totalStringLen, addAmount, playerVendor );
		}
	}

	size_t packetLen = 14 + totalStringLen + 5;
	//size_t packetLen = 15 + totalStringLen + 5;
	pStream.ReserveSize( packetLen );
	pStream.WriteShort( 1, static_cast<SI32>( packetLen ));
	pStream.WriteLong(  5, objSer );

	size_t modifier = 14;
	//size_t modifier = 15;
	//loop through all lines
	for( size_t i = 0; i < ourEntries.size(); ++i )
	{
		size_t stringLen = ourEntries[i].stringLen;
		pStream.WriteLong( ++modifier, ourEntries[i].stringNum );
		modifier += 4;
		pStream.WriteShort( modifier, static_cast<SI32>( stringLen ));
		modifier += 1;

		//convert to uni character
		for( size_t j = 0; j < stringLen; j += 2 )
		{
			pStream.WriteByte( ++modifier, ourEntries[i].ourText[j / 2] );
			pStream.WriteByte( ++modifier, 0x00 );
		}
	}

	pStream.WriteLong( packetLen-4, 0x00000000 );
}

CPToolTip::CPToolTip()
{
	InternalReset();
}
CPToolTip::CPToolTip( SERIAL objSer, CSocket *mSock, bool addAmount, bool playerVendor )
{
	tSock = mSock;
	InternalReset();
	CopyData( objSer, addAmount, playerVendor );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPSellList()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet with vendor's shop sell list
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x9E (Sell List)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] blockSize
//|						BYTE[4] shopkeeper serial
//|						BYTE[2] numItems
//|						For each item, a structure containing:
//|							BYTE[4] item serial
//|							BYTE[2] itemModel
//|							BYTE[2] itemHue/Color
//|							BYTE[2] itemAmount
//|							BYTE[2] value
//|							BYTE[2] nameLength
//|							BYTE[?] name
//o------------------------------------------------------------------------------------------------o
void CPSellList::InternalReset( void )
{
	pStream.ReserveSize( 9 );
	pStream.WriteByte( 0, 0x9E );
	numItems = 0;
}
auto CPSellList::CopyData( CChar& mChar, CChar& vendorId ) -> void
{
	auto buyPack = vendorId.GetItemAtLayer( IL_BUYCONTAINER );
	auto ourPack	= mChar.GetPackItem();

	numItems			= 0;
	size_t packetLen	= 9;

	if( ValidateObject( buyPack ) && ValidateObject( ourPack ))
	{
		CTownRegion *tReg = nullptr;
		if( cwmWorldState->ServerData()->TradeSystemStatus() )
		{
			tReg = CalcRegionFromXY( vendorId.GetX(), vendorId.GetY(), vendorId.WorldNumber(), vendorId.GetInstanceId() );
		}

		auto spCont = buyPack->GetContainsList();
		for( const auto &spItem : spCont->collection() )
		{
			if( ValidateObject( spItem ))
			{
				AddContainer( tReg, spItem, ourPack, packetLen );
			}
		}
	}

	pStream.WriteShort( 1, static_cast<UI16>( packetLen ));
	pStream.WriteLong( 3, vendorId.GetSerial() );
	pStream.WriteShort( 7, numItems );
}

auto CPSellList::AddContainer( CTownRegion *tReg, CItem *spItem, CItem *ourPack, size_t &packetLen ) -> void
{
	auto opCont = ourPack->GetContainsList();
	for( const auto &opItem : opCont->collection() )
	{
		if( ValidateObject( opItem ))
		{
			if( opItem->GetType() == IT_CONTAINER )
			{
				AddContainer( tReg, spItem, opItem, packetLen );
			}
			else if(( opItem->GetSectionId() == spItem->GetSectionId() && opItem->GetSectionId() != "UNKNOWN" )
				&& ( spItem->GetName() == opItem->GetName() || !cwmWorldState->ServerData()->SellByNameStatus() ))
			{
				// Basing it on GetSectionId() should replace all the other checks below...
				AddItem( tReg, spItem, opItem, packetLen );
				++numItems;
			}
			else if( opItem->GetSectionId() == "UNKNOWN" && opItem->GetId() == spItem->GetId() && opItem->GetType() == spItem->GetType() &&
					( spItem->GetName() == opItem->GetName() || !cwmWorldState->ServerData()->SellByNameStatus() ))
			{
				// If sectionID of an item is Unknown, it's a legacy item. Handle it the legacy way.
				// Also includes special case for deeds and dyes, which all use same ID
				if(( opItem->GetId() != 0x14f0 && opItem->GetId() != 0x0eff ) || ( opItem->GetTempVar( CITV_MOREX ) == spItem->GetTempVar( CITV_MOREX )))
				{
					AddItem( tReg, spItem, opItem, packetLen );
					++numItems;
				}
			}

			if( numItems >= 60 )
			{
				return;
			}
		}
	}
}

void CPSellList::AddItem( CTownRegion *tReg, CItem *spItem, CItem *opItem, size_t &packetLen )
{
	std::string itemname;
	size_t stringLen	= GetTileName(( *opItem ), itemname );
	size_t newLen		= ( packetLen + 14 + stringLen );
	pStream.ReserveSize( newLen );
	pStream.WriteLong( packetLen, opItem->GetSerial() );
	pStream.WriteShort(  packetLen + 4, opItem->GetId() );
	pStream.WriteShort(  packetLen + 6, opItem->GetColour() );
	pStream.WriteShort(  packetLen + 8, opItem->GetAmount() );
	UI32 value = CalcValue( opItem, spItem->GetSellValue() );
	if( cwmWorldState->ServerData()->TradeSystemStatus() )
	{
		value = CalcGoodValue( tReg, spItem, value, true );
	}
	pStream.WriteShort(  packetLen + 10, value );
	pStream.WriteShort(  packetLen + 12, static_cast<SI32>( stringLen ));
	pStream.WriteString( packetLen + 14, itemname, stringLen );
	packetLen = newLen;
}

CPSellList::CPSellList()
{
	InternalReset();
}

bool CPSellList::CanSellItems( CChar &mChar, CChar &vendor )
{
	CopyData( mChar, vendor );
	return ( numItems != 0 );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPOpenMessageBoard()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to display bulletin board
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x71 (Bulletin Board Message)
//|					Size: Variable
//|
//|					Packet Build
//|						 BYTE cmd
//|						 BYTE[2] len
//|						 BYTE subcmd
//|						 BYTE[ len - 4 ] submessage
//|							Submessage 0 – Display Bulletin Board
//|							BYTE[4] Board serial
//|							BYTE[22] board name (default is “bulletin board”, the rest nulls)
//|							BYTE[4] unknown/ID?
//|							BYTE[4] zero (0)
//o------------------------------------------------------------------------------------------------o
void CPOpenMessageBoard::InternalReset( void )
{
	pStream.ReserveSize( 38 );
	pStream.WriteByte(   0, 0x71 );
	pStream.WriteShort(	 1, 38 );
	pStream.WriteByte(   3, 0x00 ); //was 38
	//	pStream.WriteString( 8, "Bulletin Board", 22 ); //was 15
}

void CPOpenMessageBoard::CopyData( CSocket *mSock )
{
	CItem *msgBoard = CalcItemObjFromSer( mSock->GetDWord( 1 ));

	if( ValidateObject( msgBoard ))
	{
		pStream.WriteLong( 4, msgBoard->GetSerial() );
		// If the name the item (Bulletin Board) has been defined, display it
		// instead of the default "Bulletin Board" title.
		std::string msgBoardName = msgBoard->GetName();
		if( msgBoardName != "#" )
		{
			if( msgBoardName.length() >= 29 )
			{
				pStream.WriteString( 8, msgBoardName, 29 );
			}
			else
			{
				pStream.WriteString( 8, msgBoardName, msgBoardName.length() );
			}
		}
		else
		{
			pStream.WriteString( 8, "Bulletin Board", 29 ); //was 15
		}
		pStream.WriteByte( 37, 0x00 );
		/*	pStream.WriteLong( 4, msgBoard->GetSerial() );
		 // If the name the item (Bulletin Board) has been defined, display it
		 // instead of the default "Bulletin Board" title.
		 if( msgBoard->GetName() != "#" )
		 pStream.WriteString( 8, msgBoard->GetName(), 22 ); //was 21
		 else
		 pStream.WriteString( 8, "Bulletin Board", 22 ); //was 15
		 pStream.WriteLong( 30, 0x402000FF );
		 pStream.WriteLong( 34, 0x00 );*/
	}
}

CPOpenMessageBoard::CPOpenMessageBoard( CSocket *mSock )
{
	InternalReset();
	CopyData( mSock );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPOpenMsgBoardPost()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to open a bulletin board msg and/or msg summary
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x71 (Bulletin Board Message)
//|
//|					Subcommand: 0x1 (Message Summary)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd (0x71)
//|						BYTE[2] len
//|						BYTE subcmd (0x1)
//|						Subcommand details
//|							BYTE[4] BoardID
//|							BYTE[4] MessageID
//|							BYTE[4] ParentId (0 if top level)
//|							BYTE posterLen
//|							BYTE[posterLen] poster (null terminated string)
//|							BYTE subjectLen
//|							BYTE[subjectLen] subject (null terminated string)
//|							BYTE timeLen
//|							BYTE[timeLen] time (null terminated string with time of posting) (“Day 1 @ 11:28”)
//o------------------------------------------------------------------------------------------------o
//|					Subcommand: 0x2 (Message Summary)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd (0x71)
//|						BYTE[2] len
//|						BYTE subcmd (0x2)
//|						Subcommand details
//|							BYTE[4] BoardID
//|							BYTE[4] MessageID
//|							BYTE posterLen
//|							BYTE[posterLen] poster (null terminated string)
//|							BYTE subjectLen
//|							BYTE[subjectLen] subject (null terminated string)
//|							BYTE timeLen
//|							BYTE[timeLen] time (null terminated string with time of posting) (“Day 1 @ 11:28”)
//|							BYTE[5] Unknown (01 90 03 F7 00)
//|							BYTE numlines
//|							For each line:
//|								BYTE linelen
//|								BYTE[linelen] body (null terminated)
//o------------------------------------------------------------------------------------------------o
void CPOpenMsgBoardPost::InternalReset( void )
{
	pStream.ReserveSize( 4 );
	pStream.WriteByte( 0, 0x71 );
	if( bFullPost )
	{
		pStream.WriteByte( 3, 2 );
	}
	else
	{
		pStream.WriteByte( 3, 1 );
	}
}

void CPOpenMsgBoardPost::CopyData( CSocket *mSock, const MsgBoardPost_st& mbPost )
{
	size_t totSize = 19 + static_cast<size_t>( mbPost.dateLen ) + static_cast<size_t> (mbPost.posterLen ) + static_cast<size_t>( mbPost.subjectLen );
	std::vector<std::string>::const_iterator pIter;
	if( !bFullPost ) //index
	{
		pStream.ReserveSize( totSize );
		pStream.WriteShort( 1, static_cast<UI16>( totSize )); //packetSize
		pStream.WriteLong( 4, mSock->GetDWord( 4 )); // board serial
		pStream.WriteLong( 8, ( mbPost.serial + BASEITEMSERIAL )); //message serial
		SERIAL pSerial = mbPost.parentSerial; // thread serial
		if( pSerial )
		{
			pSerial += BASEITEMSERIAL;
		}
		else
		{
			pSerial += 0x80000000;
		}

		pStream.WriteLong( 12, pSerial );
		size_t byteOffset = 16;

		pStream.WriteByte( byteOffset, mbPost.posterLen );
		pStream.WriteString( ++byteOffset, ( char * )mbPost.poster, mbPost.posterLen );
		byteOffset += mbPost.posterLen;
		pStream.WriteByte( byteOffset - 1, 0x00 );

		pStream.WriteByte( byteOffset, mbPost.subjectLen );
		pStream.WriteString( ++byteOffset, ( char * )mbPost.subject, mbPost.subjectLen );
		byteOffset += mbPost.subjectLen;
		pStream.WriteByte( byteOffset - 1, 0x00 );

		pStream.WriteByte( byteOffset, mbPost.dateLen );
		pStream.WriteString( ++byteOffset, ( char * )mbPost.date, mbPost.dateLen );
		byteOffset += mbPost.dateLen;
		pStream.WriteByte( byteOffset - 1, 0x00 );
	}
	else if( bFullPost ) //full post
	{
		for( pIter = mbPost.msgBoardLine.begin(); pIter != mbPost.msgBoardLine.end(); ++pIter )
		{
			totSize += ( *pIter ).size() + 5;
		}
		totSize += 1;
		pStream.ReserveSize( totSize );
		pStream.WriteShort( 1, static_cast<UI16>( totSize )); //packet size
		pStream.WriteLong( 4, mSock->GetDWord( 4 )); //board serial
		pStream.WriteLong( 8, ( mbPost.serial + BASEITEMSERIAL )); //message serial
		size_t offset = 12;

		CChar *mChar = mSock->CurrcharObj();
		TAGMAPOBJECT modboards = mChar->GetTag( "modboards" );
		if( mChar->IsGM() && modboards.m_IntValue == 1 )
		{
			// If character is a GM, and has used the MSGMOD ON command, send their username instead of poster's,
			// to allow removing messages from the message board
			pStream.WriteByte( offset, static_cast<UI08>( mChar->GetName().length() + 1 ));
			pStream.WriteString( ++offset, mChar->GetName(), mChar->GetName().length() + 1 );
			offset += mChar->GetName().length() + 1;
		}
		else
		{
			pStream.WriteByte( offset, mbPost.posterLen + 1 );
			pStream.WriteString( ++offset, ( char * )mbPost.poster, mbPost.posterLen + 1 );
			offset += mbPost.posterLen + 1;
		}

		pStream.WriteByte( offset, mbPost.subjectLen );
		pStream.WriteString( ++offset, ( char * )mbPost.subject, mbPost.subjectLen );
		offset += mbPost.subjectLen;

		pStream.WriteByte( offset, mbPost.dateLen );
		pStream.WriteString( ++offset, ( char * )mbPost.date, mbPost.dateLen );
		offset += mbPost.dateLen;

		pStream.WriteShort( offset, 0x0190 ); //postedbody
		pStream.WriteShort( offset += 2, 0x03F7 ); //postedhue
		pStream.WriteByte(  offset += 2, 0x00 ); // postedequip-length?

		pStream.WriteByte( ++offset, mbPost.lines );

		for( pIter = mbPost.msgBoardLine.begin(); pIter != mbPost.msgBoardLine.end(); ++pIter )
		{
			pStream.WriteByte( ++offset, static_cast<UI08>(( *pIter ).size() + 2 ));
			pStream.WriteString( ++offset, ( *pIter ), ( *pIter ).size() );
			offset += ( *pIter ).size();
			pStream.WriteByte( offset, 0x00 );
			pStream.WriteByte( ++offset, 0x00 );
		}
	}
}

CPOpenMsgBoardPost::CPOpenMsgBoardPost( CSocket *mSock, const MsgBoardPost_st& mbPost, bool fullPost )
{
	bFullPost = fullPost;
	InternalReset();
	CopyData( mSock, mbPost );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPSendMsgBoardPosts()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to display index of bulletin board messages
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x3C (Bulletin Board Index/Container Content)
//|					Size: Variable
//|
//|					Packet Build
//|						 BYTE cmd
//|						 BYTE[2] packet length
//|						 BYTE[2] number of Items to add
//|						 loop items:
//|							BYTE[4] item serial
//|							BYTE[2] item ID (objtype)
//|							BYTE unknown (0x00)
//|							BYTE[2] item amount (stack)
//|							BYTE[2] xLoc
//|							BYTE[2] yLoc
//|							BYTE Backpack grid index
//|							BYTE[4] Container serial
//|							BYTE[2] item color
//|						 endloop
//|
//|					NOTE: Backpack grid index is only sent for client versions above 6.0.1.7 (2D)
//|						and 2.45.5.6 (KR)
//o------------------------------------------------------------------------------------------------o
void CPSendMsgBoardPosts::InternalReset( void )
{
	pStream.ReserveSize( 5 );
	pStream.WriteByte( 0, 0x3C );
	pStream.WriteShort( 1, 5 );
}

void CPSendMsgBoardPosts::CopyData( CSocket *mSock, SERIAL mSerial, [[maybe_unused]] UI08 pToggle, SERIAL oSerial )
{
	size_t byteOffset = pStream.GetSize();
	if( mSock->ClientVerShort() >= CVS_6017 )
	{
		pStream.ReserveSize( byteOffset + 20 );
	}
	else
	{
		pStream.ReserveSize( byteOffset + 19 );
	}

	pStream.WriteLong(  byteOffset, ( mSerial + BASEITEMSERIAL ));
	pStream.WriteShort( byteOffset + 4, 0x0EB0 ); // Item ID
	pStream.WriteByte(	byteOffset + 6, 0 ); // itemId offset
	pStream.WriteShort( byteOffset + 7, 0x0001 ); // item amount
	pStream.WriteShort( byteOffset + 9, 0x00 ); // xLoc
	pStream.WriteShort( byteOffset + 11, 0x00 ); // yLoc
	if( mSock->ClientVerShort() >= CVS_6017 )
	{
		pStream.WriteByte(	byteOffset + 13, 0 ); // grid location
		pStream.WriteLong(  byteOffset + 14, oSerial ); // container serial
		pStream.WriteShort( byteOffset + 18, 0x00 ); // item color
	}
	else
	{
		pStream.WriteLong(  byteOffset + 13, oSerial ); // container serial
		pStream.WriteShort( byteOffset + 17, 0x00 ); // item color
	}

	++postCount;
}

void CPSendMsgBoardPosts::Finalize( void )
{
	pStream.WriteShort( 1, static_cast<SI32>( pStream.GetSize() ));
	pStream.WriteShort( 3, postCount );
}

CPSendMsgBoardPosts::CPSendMsgBoardPosts()
{
	postCount = 0;
	InternalReset();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPExtendedStats()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet with extended stats
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xBF (General Information Packet)
//|					Subcommand: 0x19 (Extended Stats)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd (0xBF)
//|						BYTE[2] Length
//|						BYTE[2] Subcommand (0x19)
//|						Subcommand details
//|							BYTE subsubcommand (0x2 for 2D client, 0x5 for KR client)
//|							BYTE[4] serial
//|							BYTE unknown // always 0 ?
//|							BYTE lockBits
//|								Bits: XXSS DDII
//|									s=strength, d=dex, i=int
//|									0 = up, 1 = down, 2 = locked, FF = update mobile status animation (KR only)
//|							if subsubcommand is 0x5
//|								If Lock flags = 0xFF //Update mobile status animation
//|									BYTE[1] Status // Unveryfied if lock flags == FF the locks will be handled here
//|									BYTE[1] unknown (0x00)
//|									BYTE[1] Animation
//|									BYTE[1] unknown (0x00)
//|									BYTE[1] Frame
//|								else
//|									BYTE[1] unknown (0x00)
//|									BYTE[4] unknown (0x00000000)
//o------------------------------------------------------------------------------------------------o
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
	pStream.WriteByte(  5, 2 );
}

void CPExtendedStats::CopyData( CChar& mChar )
{
	pStream.WriteLong( 6, mChar.GetSerial() );

	const UI08 strength		= static_cast<UI08>(( mChar.GetSkillLock( STRENGTH ) & 0x3 ) << 4 );
	const UI08 dexterity	= static_cast<UI08>(( mChar.GetSkillLock( DEXTERITY ) & 0x3 ) << 2 );
	const UI08 intelligence	= static_cast<UI08>( mChar.GetSkillLock( INTELLECT ) & 0x3 );

	pStream.WriteByte( 11, ( strength | dexterity | intelligence ));
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPHealthBarStatus()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to update health bar status of character
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x16 (UO3D Mobile New Health Bar Status)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] Packet Size
//|						BYTE[4] Serial
//|						BYTE[2] Extended (1 if status bar is green or yellow, else 0)
//|						BYTE[2] Status Color (0x01 = Green, 0x02 = Yellow, sends only if extended == 1)
//|						BYTE Flag (0x00 = Remove Status Color, 0x01 = Enable Status Color, sends only if extended == 1)
//|						If mobile is poisoned, flag value > 0x00 - poison level.
//|						Since 4.0.7.0/7.0.7.0, sends to both 2d and 3d but works only in 3d. Server
//|							sends it as response for 0x34 Mobile Status Query.
//o------------------------------------------------------------------------------------------------o
//|					Packet: 0x17 (Mobile Health Bar Status Update)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] Packet Size
//|						BYTE[4] Mobile Serial
//|						BYTE[2] 0x01
//|						BYTE[2] Status Color (0x01 = Green, 0x02 = Yellow, 0x03 = Red)
//|						BYTE Flag (0x00 = Remove Status Color, 0x01 = Enable Status Color)
//|						If mobile is poisoned, flag value > 0x00 - poison level
//o------------------------------------------------------------------------------------------------o
CPHealthBarStatus::CPHealthBarStatus()
{
	InternalReset();
}

CPHealthBarStatus::CPHealthBarStatus( CChar &mChar, CSocket &tSock, UI08 healthBarColor )
{
	InternalReset();
	SetHBStatusData( mChar, tSock, healthBarColor );
	//CopyData( mChar );
}

void CPHealthBarStatus::InternalReset( void )
{
	pStream.ReserveSize( 9 );
}

void CPHealthBarStatus::SetHBStatusData( CChar &mChar, CSocket &tSock, UI08 healthBarColor )
{
	if( tSock.ClientType() == CV_UO3D || tSock.ClientType() == CV_KR3D || tSock.ClientType() == CV_SA3D || tSock.ClientType() == CV_HS3D )
	{
		pStream.WriteByte( 0, 0x16 );
		pStream.WriteLong( 3, mChar.GetSerial() );
		CChar *sockChar = tSock.CurrcharObj();
		if(( mChar.GetGuildNumber() == sockChar->GetGuildNumber() ) || mChar.IsInvulnerable() )
		{
			pStream.ReserveSize( 12 );
			pStream.WriteShort( 1, 12 );
			pStream.WriteShort( 7, 0x01 );
			if( mChar.GetGuildNumber() == sockChar->GetGuildNumber() )
			{
				pStream.WriteShort( 9, 1 );
			}
			else if( mChar.IsInvulnerable() )
			{
				pStream.WriteShort( 9, 2 );
			}
			else
			{
				pStream.WriteShort( 9, 0 );
			}

			if( mChar.GetPoisoned() > 0 )
			{
				pStream.WriteByte( 11, mChar.GetPoisoned() );
			}
			else
			{
				pStream.WriteByte( 11, 0 );
			}
		}
		else
		{
			pStream.WriteShort( 7, 0x0 ); //Packet ends
		}
	}
	else
	{
		pStream.ReserveSize( 12 );
		pStream.WriteByte(  0, 0x17 );
		pStream.WriteShort( 1, 12 );
		pStream.WriteLong( 3, mChar.GetSerial() );
		pStream.WriteShort( 7, 0x01 );

		if( healthBarColor == 1 ) // poisoned?
		{
			pStream.WriteShort( 9, 1 );
			if( mChar.GetPoisoned() > 0 )
			{
				pStream.WriteByte( 11, mChar.GetPoisoned() );
			}
			else
			{
				pStream.WriteByte( 11, 0 );
			}
		}
		else // invulnerable?
		{
			// healthBarColor == 2
			pStream.WriteShort( 9, 2 );
			pStream.WriteByte( 11, mChar.IsInvulnerable() );
		}
	}
}

void CPHealthBarStatus::CopyData( CChar& mChar )
{
	pStream.WriteLong( 6, mChar.GetSerial() );

	const UI08 strength		= static_cast<UI08>(( mChar.GetSkillLock( STRENGTH ) & 0x3 ) << 4 );
	const UI08 dexterity	= static_cast<UI08>(( mChar.GetSkillLock( DEXTERITY ) & 0x3 ) << 2 );
	const UI08 intelligence	= static_cast<UI08>( mChar.GetSkillLock( INTELLECT ) & 0x3 );

	pStream.WriteByte( 11, (strength | dexterity | intelligence) );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPEnableMapDiffs()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet enabling map diffs in client
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xBF (General Information Packet)
//|					Subcommand: 0x18 (Enable Map-Diff files)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd (0xBF)
//|						BYTE[2] Length
//|						BYTE[2] Subcommand (0x18)
//|						Subcommand details
//|							BYTE[4] Number of maps
//|							For each map
//|								BYTE[4] Number of map patches in this map (these two lines are in reverse order!)
//|								BYTE[4] Number of static patches in this map
//|
//|					Note: Only client versions lower than v7.0.8.2 will load diff files
//o------------------------------------------------------------------------------------------------o
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
	size_t pSize	= (( static_cast<size_t>( mapCount ) + 1 ) * 8 ) + 9;

	pStream.ReserveSize( pSize );
	pStream.WriteShort( 1, static_cast<SI32>( pSize ));
	pStream.WriteLong( 5, mapCount+1 );

	for( UI08 i = 0; i < mapCount; ++i )
	{
		auto [stadif, mapdif] = Map->DiffCountForMap( i );
		pStream.WriteLong( 9 + ( static_cast<size_t>( i ) * 8 ), static_cast<UI32>( stadif ));
		pStream.WriteLong( 13 + ( static_cast<size_t>( i ) * 8 ), static_cast<UI32>( mapdif ));
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPNewBookHeader()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to respond to client request to change book title
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xD4 (New Book Header)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] length
//|						BYTE[4] book ID
//|						BYTE flag1
//|						BYTE flag2
//|						BYTE[2] number of pages
//|						BYTE[2] length of author string (0 terminator included)
//|						BYTE[?] author (ASCII, 0 terminated)
//|						BYTE[2] length of title string (0 terminator included)
//|						BYTE[?] title string
//|
//|					Note1:
//|						server and client packet.
//|							server side: opening book writeable: flag1+flag2 both 1
//|								(opening readonly book: flag1+flag2 0, unverified though)
//|						client side: flag1+flag2 both 0, number of pages 0.
//|
//|					Note2:
//|						Opening books goes like this:
//|							open book ->
//|							server sends 0xd4(title + author)
//|							server sends 0x66 with all book data "beyond" title + author
//|							if title + author changed: client side 0xd4 send
//|							if other book pages changed: 0x66 client side send.
//o------------------------------------------------------------------------------------------------o
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

void CPNewBookHeader::Author( const std::string& newAuthor )
{
	author = newAuthor;
}

void CPNewBookHeader::Title( const std::string& newTitle )
{
	title = newTitle;
}

void CPNewBookHeader::Finalize( void )
{
	const UI16 authorLen	= static_cast<UI16>( author.size() );
	const UI16 titleLen		= static_cast<UI16>( title.size() );
	const UI16 totalLen		= static_cast<UI16>( 15 + authorLen + titleLen + 2 );
	size_t offset			= 11;

	pStream.ReserveSize( totalLen );
	pStream.WriteShort( 1, totalLen );

	pStream.WriteShort( offset, titleLen + 1 );
	pStream.WriteString( offset += 2, title, titleLen );
	pStream.WriteByte( offset += titleLen, 0x00 );

	pStream.WriteShort( offset += 1, authorLen + 1 );
	pStream.WriteString( offset += 2, author, authorLen );
	pStream.WriteByte( offset += authorLen, 0x00 );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPPopupMenu()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to display context menu
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xBF (General Information Packet)
//|					Subcommand: 0x14 (Display Popup menu, Old version)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd (0xBF)
//|						BYTE[2] Length
//|						BYTE[2] Subcommand (0x14)
//|						Subcommand details
//|							BYTE unknown (0x00)
//|							BYTE subsubcommand
//|								0x01 for 2D < 7.0.0.0
//|								0x02 for KR, SA3D and 2D > 7.0.0.0
//|							BYTE[4] Serial
//|							BYTE Number of entries in the popup
//|							if subsubcommand is 0x01:
//|								Loop for each entry:
//|									BYTE[2] Entry Tag (this will be returned by the client on selection)
//|									BYTE[2] Cliloc ID (file number for intloc#.language e.g intloc6.enu and the index into that)
//|									BYTE[2] Flags 0x00=enabled, 0x01=disabled, 0x02=arrow, 0x20 = color
//|									If (Flags &0x20)
//|										BYTE[2] color
//|											rgb 1555 color (ex, 0 = transparent, 0x8000 = solid black,
//|											0x1F = blue, 0x3E0 = green, 0x7C00 = red)
//|							if subsubcommand is 0x02:
//|								Loop for each entry:
//|									BYTE[4] Cliloc ID
//|									BYTE[2] Index of entry (entry tag)?
//|									BYTE[2] Flags 0x00=enabled, 0x01=disabled, 0x04 = highlighted, 0x20 = color
//|									If (flags &0x20 and 2D client)
//|										BYTE[2] color
//o------------------------------------------------------------------------------------------------o
CPPopupMenu::CPPopupMenu( void )
{
	InternalReset();
}

CPPopupMenu::CPPopupMenu( CBaseObject& toCopy, CSocket &tSock )
{
	InternalReset();
	CopyData( toCopy, tSock );
}

void CPPopupMenu::InternalReset( void )
{
	pStream.ReserveSize( 5 );
	pStream.WriteByte( 0, 0xBF );
	pStream.WriteShort( 1, 5 );
	pStream.WriteShort( 3, 0x14 );
	pStream.WriteShort( 5, 0x0001 );
}

void CPPopupMenu::CopyData( CBaseObject& toCopy, CSocket &tSock )
{
	UI16 packetLen = ( 12 + ( 4 * 8 ));
	pStream.ReserveSize( packetLen );
	pStream.WriteShort( 1, packetLen );

	pStream.WriteLong( 7, toCopy.GetSerial() );
	size_t offset = 12;
	UI08 numEntries = 0;

	CChar *mChar = nullptr;
	if( ValidateObject( tSock.CurrcharObj() ))
	{
		mChar = tSock.CurrcharObj();
	}

	auto toCopyChar = static_cast<CChar *>( &toCopy );

	// Stop moving for 2 seconds when someone opens a context menu, to give them 
	// the chance to select something before walking out of range
	if( toCopy.CanBeObjType( OT_CHAR ) )
	{
		if( toCopyChar->GetNpcWander() != WT_PATHFIND && toCopyChar->GetNpcWander() != WT_FOLLOW && toCopyChar->GetNpcWander() != WT_FLEE )
		{
			toCopyChar->SetTimer( tNPC_MOVETIME, BuildTimeValue( 3 ));
		}
	}
	else
	{
		return;
	}

	// Open Paperdoll
	if( cwmWorldState->creatures[toCopy.GetId()].IsHuman() )
	{
		numEntries++;
		pStream.WriteShort( offset, 0x000A );	// Unique ID
		pStream.WriteShort( offset += 2, 6123 );
		pStream.WriteShort( offset += 2, 0x0020 ); // Flag, color enabled
		pStream.WriteShort( offset += 2, 0x03E0 ); // Hue of text
	}
	/*else
	{
		// Left this here as an example of how to show a disabled context menu option
		pStream.WriteShort( offset+=2, 0x0021 ); // Flag, color enabled, entry disabled
		pStream.WriteShort( offset+=2, 0xFFFF ); // Hue of text
	}*/

	// Bulk Order Info - if enabled in ini
	if( cwmWorldState->ServerData()->OfferBODsFromContextMenu() && toCopyChar->IsShop() )
	{
		TAGMAPOBJECT isBodVendor = toCopy.GetTag( "bodType" );
		if( isBodVendor.m_IntValue > 0 )
		{
			if( numEntries > 0 )
			{
				offset += 2;
			}

			numEntries++;
			pStream.WriteShort( offset, 0x0100 );	// Unique ID
			pStream.WriteShort( offset += 2, 6152 ); // Bulk Order Info
			pStream.WriteShort( offset += 2, 0x0020 ); // Flag, color enabled
			pStream.WriteShort( offset += 2, 0x03E0 ); // Hue of text
		}
	}

	// Open Backpack
	//	|| ( toCopy.IsTamed() && ValidateObject( toCopy.GetOwnerObj() ) && toCopy.GetOwnerObj() == mChar && !toCopy.CanBeHired() )))
	if( toCopyChar->GetQuestType() != QT_ESCORTQUEST && (( toCopy.GetSerial() == tSock.CurrcharObj()->GetSerial() 
		|| toCopy.GetId() == 0x0123 || toCopy.GetId() == 0x0124 || toCopy.GetId() == 0x0317 ) && ValidateObject( toCopyChar->GetPackItem() )))
	{
		if( numEntries > 0 )
		{
			offset += 2;
		}

		numEntries++;
		pStream.WriteShort( offset, 0x000B );	// Unique ID
		pStream.WriteShort( offset += 2, 6145 );
		if( ObjInRange( mChar, &toCopy, 8 ))
		{
			pStream.WriteShort( offset += 2, 0x0020 ); // Flag, color enabled
			pStream.WriteShort( offset += 2, 0x03E0 ); // Hue of text
		}
		else
		{
			pStream.WriteShort( offset += 2, 0x0021 ); // Flag, color enabled, entry disabled
			pStream.WriteShort( offset += 2, 0xFFFF ); // Hue of text
		}
	}

	// Banker
	if( toCopyChar->IsNpc() && toCopyChar->GetNpcAiType() == AI_BANKER )
	{
		if( numEntries > 0 )
		{
			offset += 2;
		}

		// Open Bankbox
		numEntries++;
		pStream.WriteShort( offset, 0x000E );	// Unique ID
		pStream.WriteShort( offset += 2, 6105 );
		if( ObjInRange( mChar, &toCopy, 8 ))
		{
			pStream.WriteShort( offset += 2, 0x0020 );	// Flag, entry enabled
			pStream.WriteShort( offset += 2, 0x03E0 );	// Hue of text
		}
		else
		{
			pStream.WriteShort( offset += 2, 0x0021 ); // Flag, color enabled, entry disabled
			pStream.WriteShort( offset += 2, 0xFFFF ); // Hue of text
		}
	}

	// Stablemaster
	if( toCopyChar->GetNpcAiType() == AI_STABLEMASTER )
	{
		if( numEntries > 0 )
		{
			offset += 2;
		}

		// Claim All Pets
		numEntries++;
		pStream.WriteShort( offset, 0x0019 );	// Unique ID
		pStream.WriteShort( offset += 2, 6127 );
		if( ObjInRange( mChar, &toCopy, 8 ))
		{
			pStream.WriteShort( offset += 2, 0x0020 ); // Flag, color enabled
			pStream.WriteShort( offset += 2, 0x03E0 ); // Hue of text
		}
		else
		{
			pStream.WriteShort( offset += 2, 0x0021 ); // Flag, color enabled, entry disabled
			pStream.WriteShort( offset += 2, 0xFFFF ); // Hue of text
		}

		// Stable Pet
		numEntries++;
		pStream.WriteShort( offset += 2, 0x001a );	// Unique ID
		pStream.WriteShort( offset += 2, 6126 );
		if( ObjInRange( mChar, &toCopy, 8 ))
		{
			pStream.WriteShort( offset += 2, 0x0020 ); // Flag, color enabled
			pStream.WriteShort( offset += 2, 0x03E0 ); // Hue of text
		}
		else
		{
			pStream.WriteShort( offset += 2, 0x0021 ); // Flag, color enabled, entry disabled
			pStream.WriteShort( offset += 2, 0xFFFF ); // Hue of text
		}
	}

	// Hireling
	if( toCopyChar->IsNpc() && toCopyChar->CanBeHired() && !ValidateObject( toCopy.GetOwnerObj() ))
	{
		if( numEntries > 0 )
		{
			offset += 2;
		}

		// Hire (Hireling)
		numEntries++;
		pStream.WriteShort( offset, 0x001e );	// Unique ID
		pStream.WriteShort( offset += 2, 6120 );
		if( ObjInRange( mChar, &toCopy, 3 ))
		{
			pStream.WriteShort( offset += 2, 0x0020 ); // Flag, color enabled
			pStream.WriteShort( offset += 2, 0x03E0 ); // Hue of text
		}
		else
		{
			pStream.WriteShort( offset += 2, 0x0021 ); // Flag, color enabled, entry disabled
			pStream.WriteShort( offset += 2, 0xFFFF ); // Hue of text
		}
	}

	// NPC Escort
	if( toCopyChar->IsNpc() && toCopyChar->GetQuestType() == QT_ESCORTQUEST )
	{
		if( numEntries > 0 )
		{
			offset += 2;
		}

		// Ask Destination
		numEntries++;
		pStream.WriteShort( offset, 0x001b );	// Unique ID
		pStream.WriteShort( offset += 2, 6100 );
		if( ObjInRange( mChar, &toCopy, 3 ))
		{
			pStream.WriteShort( offset += 2, 0x0020 );	// Flag, entry enabled
			pStream.WriteShort( offset += 2, 0x03E0 );	// Hue of text
		}
		else
		{
			pStream.WriteShort( offset += 2, 0x0021 ); // Flag, color enabled, entry disabled
			pStream.WriteShort( offset += 2, 0xFFFF ); // Hue of text
		}

		// Accept Escort
		if( toCopy.GetOwnerObj() == nullptr )
		{
			numEntries++;
			pStream.WriteShort( offset += 2, 0x001c );	// Unique ID
			pStream.WriteShort( offset += 2, 6101 );
			if( ObjInRange( mChar, &toCopy, 3 ))
			{
				pStream.WriteShort( offset += 2, 0x0020 );	// Flag, entry enabled
				pStream.WriteShort( offset += 2, 0x03E0 );	// Hue of text
			}
			else
			{
				pStream.WriteShort( offset += 2, 0x0021 ); // Flag, color enabled, entry disabled
				pStream.WriteShort( offset += 2, 0xFFFF ); // Hue of text
			}
		}

		// Abandon Escort
		if( toCopy.GetOwnerObj() == mChar )
		{
			numEntries++;
			pStream.WriteShort( offset += 2, 0x001d );	// Unique ID
			pStream.WriteShort( offset += 2, 6102 );
			pStream.WriteShort( offset += 2, 0x0020 );	// Flag, entry enabled
			pStream.WriteShort( offset += 2, 0x03E0 );	// Hue of text
		}
	}

	// Shopkeeper
	if( toCopyChar->IsShop() )
	{
		// Shopkeeper - Buy
		CItem *sellContainer = toCopyChar->GetItemAtLayer( IL_SELLCONTAINER );
		if( ValidateObject( sellContainer ) && sellContainer->GetContainsList()->Num() > 0 )
		{
			if( numEntries > 0 )
			{
				offset += 2;
			}

			numEntries++;
			pStream.WriteShort( offset, 0x000C );	// Unique ID
			pStream.WriteShort( offset += 2, 6103 );

			if( ObjInRange( mChar, &toCopy, 8 ))
			{
				pStream.WriteShort( offset += 2, 0x0020 );	// Flag, entry enabled
				pStream.WriteShort( offset += 2, 0x03E0 );	// Hue of text
			}
			else
			{
				pStream.WriteShort( offset += 2, 0x0021 ); // Flag, color enabled, entry disabled
				pStream.WriteShort( offset += 2, 0xFFFF ); // Hue of text
			}
		}

		// Shopkeeper - Sell
		CItem *buyContainer = toCopyChar->GetItemAtLayer( IL_BUYCONTAINER );
		if( ValidateObject( buyContainer ) && buyContainer->GetContainsList()->Num() > 0 )
		{
			if( numEntries > 0 )
			{
				offset += 2;
			}

			numEntries++;
			pStream.WriteShort( offset, 0x000D );	// Unique ID
			pStream.WriteShort( offset += 2, 6104 );
			if( ObjInRange( mChar, &toCopy, 8 ))
			{
				pStream.WriteShort( offset += 2, 0x0020 );	// Flag, entry enabled
				pStream.WriteShort( offset += 2, 0x03E0 );	// Hue of text
			}
			else
			{
				pStream.WriteShort( offset += 2, 0x0021 ); // Flag, color enabled, entry disabled
				pStream.WriteShort( offset += 2, 0xFFFF ); // Hue of text
			}
		}
	}

	// Tame
	if( toCopyChar->IsNpc() && cwmWorldState->creatures[toCopy.GetId()].IsAnimal() && !ValidateObject( toCopy.GetOwnerObj() ))
	{
		auto skillToTame = toCopyChar->GetTaming();
		if( skillToTame > 0 )
		{
			if( numEntries > 0 )
			{
				offset += 2;
			}

			numEntries++;
			pStream.WriteShort( offset, 0x000F );	// Unique ID
			pStream.WriteShort( offset += 2, 6130 );
			if( skillToTame <= mChar->GetSkill( TAMING ) && ObjInRange( mChar, &toCopy, 8 ))
			{
				pStream.WriteShort( offset += 2, 0x0020 ); // Flag, color enabled
				pStream.WriteShort( offset += 2, 0x03E0 ); // Hue of text
			}
			else
			{
				pStream.WriteShort( offset+=2, 0x0021 ); // Flag, color enabled, entry disabled
				pStream.WriteShort( offset+=2, 0xFFFF ); // Hue of text
			}
		}
	}

	// Pet commands
	bool playerIsOwner = ( ValidateObject( toCopy.GetOwnerObj() ) && toCopy.GetOwnerObj() == mChar );
	bool playerIsFriend = Npcs->CheckPetFriend( mChar, toCopyChar );
	if( toCopyChar->IsNpc() && toCopyChar->GetNpcAiType() != AI_PLAYERVENDOR && toCopyChar->GetQuestType() != QT_ESCORTQUEST && ( toCopyChar->IsTamed() || toCopyChar->CanBeHired() )
		&& ( playerIsOwner || playerIsFriend ))
	{
		if( numEntries > 0 )
		{
			offset += 2;
		}

		// Command: Kill (Pet)
		numEntries++;
		pStream.WriteShort( offset, 0x0010 );	// Unique ID
		pStream.WriteShort( offset += 2, 6111 );
		if( ObjInRange( mChar, &toCopy, 12 ))
		{
			pStream.WriteShort( offset += 2, 0x0020 ); // Flag, color enabled
			pStream.WriteShort( offset += 2, 0x03E0 ); // Hue of text
		}
		else
		{
			pStream.WriteShort( offset += 2, 0x0021 ); // Flag, color enabled, entry disabled
			pStream.WriteShort( offset += 2, 0xFFFF ); // Hue of text
		}

		// Command: Stop (Pet)
		numEntries++;
		pStream.WriteShort( offset += 2, 0x0011 );	// Unique ID
		pStream.WriteShort( offset += 2, 6112 );
		if( ObjInRange( mChar, &toCopy, 12 ))
		{
			pStream.WriteShort( offset += 2, 0x0020 ); // Flag, color enabled
			pStream.WriteShort( offset += 2, 0x03E0 ); // Hue of text
		}
		else
		{
			pStream.WriteShort( offset += 2, 0x0021 ); // Flag, color enabled, entry disabled
			pStream.WriteShort( offset += 2, 0xFFFF ); // Hue of text
		}

		// Command: Follow (Pet)
		numEntries++;
		pStream.WriteShort( offset += 2, 0x0012 );	// Unique ID
		pStream.WriteShort( offset += 2, 6108 );
		if( ObjInRange( mChar, &toCopy, 12 ))
		{
			pStream.WriteShort( offset += 2, 0x0020 ); // Flag, color enabled
			pStream.WriteShort( offset += 2, 0x03E0 ); // Hue of text
		}
		else
		{
			pStream.WriteShort( offset += 2, 0x0021 ); // Flag, color enabled, entry disabled
			pStream.WriteShort( offset += 2, 0xFFFF ); // Hue of text
		}

		// Command: Stay (Pet)
		numEntries++;
		pStream.WriteShort( offset += 2, 0x0013 );	// Unique ID
		pStream.WriteShort( offset += 2, 6114 );
		if( ObjInRange( mChar, &toCopy, 12 ))
		{
			pStream.WriteShort( offset += 2, 0x0020 ); // Flag, color enabled
			pStream.WriteShort( offset += 2, 0x03E0 ); // Hue of text
		}
		else
		{
			pStream.WriteShort( offset += 2, 0x0021 ); // Flag, color enabled, entry disabled
			pStream.WriteShort( offset += 2, 0xFFFF ); // Hue of text
		}

		// Command: Guard (Pet)
		numEntries++;
		pStream.WriteShort( offset += 2, 0x0014 );	// Unique ID
		pStream.WriteShort( offset += 2, 6107 );
		if( ObjInRange( mChar, &toCopy, 12 ))
		{
			pStream.WriteShort( offset += 2, 0x0020 ); // Flag, color enabled
			pStream.WriteShort( offset += 2, 0x03E0 ); // Hue of text
		}
		else
		{
			pStream.WriteShort( offset += 2, 0x0021 ); // Flag, color enabled, entry disabled
			pStream.WriteShort( offset += 2, 0xFFFF ); // Hue of text
		}

		// Restrict to owner only
		if( playerIsOwner )
		{
			// Add Friend (Pet)
			numEntries++;
			pStream.WriteShort( offset += 2, 0x0015 );	// Unique ID
			pStream.WriteShort( offset += 2, 6110 );
			if( ObjInRange( mChar, &toCopy, 12 ))
			{
				pStream.WriteShort( offset += 2, 0x0020 ); // Flag, color enabled
				pStream.WriteShort( offset += 2, 0x03E0 ); // Hue of text
			}
			else
			{
				pStream.WriteShort( offset += 2, 0x0021 ); // Flag, color enabled, entry disabled
				pStream.WriteShort( offset += 2, 0xFFFF ); // Hue of text
			}

			// Remove Friend (Pet)
			numEntries++;
			pStream.WriteShort( offset += 2, 0x001f );	// Unique ID

			pStream.WriteShort( offset += 2, 6099 );
			if( ObjInRange( mChar, &toCopy, 12 ))
			{
				pStream.WriteShort( offset += 2, 0x0020 ); // Flag, color enabled
				pStream.WriteShort( offset += 2, 0x03E0 ); // Hue of text
			}
			else
			{
				pStream.WriteShort( offset += 2, 0x0021 ); // Flag, color enabled, entry disabled
				pStream.WriteShort( offset += 2, 0xFFFF ); // Hue of text
			}

			// Transfer (Pet)
			numEntries++;
			pStream.WriteShort( offset += 2, 0x0016 );	// Unique ID
			pStream.WriteShort( offset += 2, 6113 );
			if( ObjInRange( mChar, &toCopy, 12 ))
			{
				pStream.WriteShort( offset += 2, 0x0020 ); // Flag, color enabled
				pStream.WriteShort( offset += 2, 0x03E0 ); // Hue of text
			}
			else
			{
				pStream.WriteShort( offset += 2, 0x0021 ); // Flag, color enabled, entry disabled
				pStream.WriteShort( offset += 2, 0xFFFF ); // Hue of text
			}

			if( toCopyChar->IsTamed() )
			{
				// Release (Pet)
				numEntries++;
				pStream.WriteShort( offset += 2, 0x0017 );	// Unique ID
				pStream.WriteShort( offset += 2, 6118 );
				if( ObjInRange( mChar, &toCopy, 12 ))
				{
					pStream.WriteShort( offset += 2, 0x0020 ); // Flag, color enabled
					pStream.WriteShort( offset += 2, 0x03E0 ); // Hue of text
				}
				else
				{
					pStream.WriteShort( offset += 2, 0x0021 ); // Flag, color enabled, entry disabled
					pStream.WriteShort( offset += 2, 0xFFFF ); // Hue of text
				}
			}

			if( toCopyChar->CanBeHired() )
			{
				// Dismiss (Hireling)
				numEntries++;
				pStream.WriteShort( offset += 2, 0x0018 );	// Unique ID
				pStream.WriteShort( offset += 2, 6129 );
				if( ObjInRange( mChar, &toCopy, 12 ))
				{
					pStream.WriteShort( offset += 2, 0x0020 ); // Flag, color enabled
					pStream.WriteShort( offset += 2, 0x03E0 ); // Hue of text
				}
				else
				{
					pStream.WriteShort( offset += 2, 0x0021 ); // Flag, color enabled, entry disabled
					pStream.WriteShort( offset += 2, 0xFFFF ); // Hue of text
				}
			}
		}
	}

	// Skill training - IDs 0x006D to 0x009C
	if( toCopyChar->IsNpc() && !toCopyChar->IsTamed() && !cwmWorldState->creatures[toCopy.GetId()].IsAnimal() && toCopyChar->CanTrain() && cwmWorldState->creatures[toCopy.GetId()].IsHuman() )
	{
		auto idTracker = 0x0071;
		auto clilocTracker = 6000;
		auto skillEntries = 0;
		for( UI08 i = 0; i < ALLSKILLS; ++i )
		{
			// Limit it to 10 entries per NPC. Context menus are not scrollable
			if( skillEntries >= 10 )
				break;

			auto trainerSkillPoints = toCopyChar->GetBaseSkill( i );
			if( trainerSkillPoints > 600 && ( mChar->GetSkill( i ) < trainerSkillPoints / 3 ))
			{
				if( numEntries > 0 )
				{
					offset += 2;
				}
				numEntries++;
				skillEntries++;

				switch( i )
				{
					case ANATOMY:			pStream.WriteShort( offset, 0x0154 ); break;
					case PARRYING:			pStream.WriteShort( offset, 0x006d ); break;
					case HEALING:			pStream.WriteShort( offset, 0x006e ); break;
					case HIDING:			pStream.WriteShort( offset, 0x006f ); break;
					case STEALING:			pStream.WriteShort( offset, 0x0070 ); break;
					case ALCHEMY:			pStream.WriteShort( offset, 0x0071 ); break;
					case ANIMALLORE:		pStream.WriteShort( offset, 0x0072 ); break;
					case ITEMID:			pStream.WriteShort( offset, 0x0073 ); break;
					case ARMSLORE:			pStream.WriteShort( offset, 0x0074 ); break;
					case BEGGING:			pStream.WriteShort( offset, 0x0075 ); break;
					case BLACKSMITHING:		pStream.WriteShort( offset, 0x0076 ); break;
					case BOWCRAFT:			pStream.WriteShort( offset, 0x0077 ); break;
					case PEACEMAKING:		pStream.WriteShort( offset, 0x0078 ); break;
					case CAMPING:			pStream.WriteShort( offset, 0x0079 ); break;
					case CARPENTRY:			pStream.WriteShort( offset, 0x007a ); break;
					case CARTOGRAPHY:		pStream.WriteShort( offset, 0x007b ); break;
					case COOKING:			pStream.WriteShort( offset, 0x007c ); break;
					case DETECTINGHIDDEN:	pStream.WriteShort( offset, 0x007d ); break;
					case ENTICEMENT:		pStream.WriteShort( offset, 0x007e ); break;
					case EVALUATINGINTEL:	pStream.WriteShort( offset, 0x007f ); break;
					case FISHING:			pStream.WriteShort( offset, 0x0080 ); break;
					case PROVOCATION:		pStream.WriteShort( offset, 0x0081 ); break;
					case LOCKPICKING:		pStream.WriteShort( offset, 0x0082 ); break;
					case MAGERY:			pStream.WriteShort( offset, 0x0083 ); break;
					case MAGICRESISTANCE:	pStream.WriteShort( offset, 0x0084 ); break;
					case TACTICS:			pStream.WriteShort( offset, 0x0085 ); break;
					case SNOOPING:			pStream.WriteShort( offset, 0x0086 ); break;
					case REMOVETRAP:		pStream.WriteShort( offset, 0x0087 ); break;
					case MUSICIANSHIP:		pStream.WriteShort( offset, 0x0088 ); break;
					case POISONING:			pStream.WriteShort( offset, 0x0089 ); break;
					case ARCHERY:			pStream.WriteShort( offset, 0x008a ); break;
					case SPIRITSPEAK:		pStream.WriteShort( offset, 0x008b ); break;
					case TAILORING:			pStream.WriteShort( offset, 0x008c ); break;
					case TAMING:			pStream.WriteShort( offset, 0x008d ); break;
					case TASTEID:			pStream.WriteShort( offset, 0x008e ); break;
					case TINKERING:			pStream.WriteShort( offset, 0x008f ); break;
					case VETERINARY:		pStream.WriteShort( offset, 0x0090 ); break;
					case FORENSICS:			pStream.WriteShort( offset, 0x0091 ); break;
					case HERDING:			pStream.WriteShort( offset, 0x0092 ); break;
					case TRACKING:			pStream.WriteShort( offset, 0x0093 ); break;
					case STEALTH:			pStream.WriteShort( offset, 0x0094 ); break;
					case INSCRIPTION:		pStream.WriteShort( offset, 0x0095 ); break;
					case SWORDSMANSHIP:		pStream.WriteShort( offset, 0x0096 ); break;
					case MACEFIGHTING:		pStream.WriteShort( offset, 0x0097 ); break;
					case FENCING:			pStream.WriteShort( offset, 0x0098 ); break;
					case WRESTLING:			pStream.WriteShort( offset, 0x0099 ); break;
					case LUMBERJACKING:		pStream.WriteShort( offset, 0x009a ); break;
					case MINING:			pStream.WriteShort( offset, 0x009b ); break;
					case MEDITATION:		pStream.WriteShort( offset, 0x009c ); break;
					case NECROMANCY:		pStream.WriteShort( offset, 0x017c ); break;
					case CHIVALRY:			pStream.WriteShort( offset, 0x017d ); break;
					case FOCUS:				pStream.WriteShort( offset, 0x017e ); break;
					default:
						break;
				}
				pStream.WriteShort( offset += 2, clilocTracker );
				pStream.WriteShort( offset += 2, 0x0020 ); // Flag, color enabled
				pStream.WriteShort( offset += 2, 0x03E0 ); // Hue of text
			}
			idTracker++;
			clilocTracker++;
		}
	}

	// Write number of entries
	pStream.WriteByte( 11, numEntries );

	// Update size of packet
	packetLen = ( 12 + ( numEntries * 8 ));
	pStream.ReserveSize( packetLen );
	pStream.WriteShort( 1, packetLen );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPClilocMessage()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet with cliloc message
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xC1 (Cliloc Message)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] length
//|						BYTE[4] serial (0xffff for system message)
//|						BYTE[2] body/id (0xff for system message)
//|						BYTE type (6 - lower left, 7 on player)
//|							Message Type
//|								0x00 = Regular, 0x01 = System, 0x02 = Emote, 0x06 = Label,
//|								0x07 = Focus, 0x08 = Whisper, 0x09 = Yell, 0x0A = Spell,
//|								0x0D = Guild, 0x0E = Alliance, 0x0F = GM Request, 0x10 = GM Response,
//|								0x20 = Special, 0xC0 = Encoded
//|						BYTE[2] hue
//|						BYTE[2] font
//|						BYTE[4] Message number
//|						BYTE[30] - speaker's name
//|						BYTE[?*2] - arguments // _little-endian_ unicode string, tabs ('\t') seperate the arguments
//|							Argument example:
//|							take number 1042762:
//|							"Only ~1_AMOUNT~ gold could be deposited. A check for ~2_CHECK_AMOUNT~ gold was returned to you."
//|							the arguments string may have "100 thousand\t25 hundred", which in turn would modify the string:
//|							"Only 100 thousand gold could be deposited. A check for 25 hundred gold was returned to you."
//o------------------------------------------------------------------------------------------------o
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
	Body( toCopy.GetId() );

	std::string toCopyName = toCopy.GetName();

	if( toCopy.CanBeObjType( OT_CHAR ))
	{
		CChar *toCopyChar = CalcCharObjFromSer( toCopy.GetSerial() );
		toCopyName = GetNpcDictName( toCopyChar, nullptr, NRS_SPEECH );
	}
	Name( toCopyName );
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

void CPClilocMessage::Name( const std::string& name )
{
	if( name.size() > 29 )
	{
		pStream.WriteString( 18, name, 29 );
		pStream.WriteByte( 47, 0x00 );
	}
	else
		pStream.WriteString( 18, name, 30 );
}

void CPClilocMessage::ArgumentString( const std::string& arguments )
{
	std::wstring wArguments = oldstrutil::stringToWstring( arguments );
	const size_t stringLen = wArguments.size();

	const UI16 packetLen = static_cast<UI16>( pStream.GetShort( 1 ) + ( stringLen * 2 ) + 2 );
	pStream.ReserveSize( packetLen );
	pStream.WriteShort( 1, packetLen );

	for( size_t i = 0; i < stringLen; ++i )
	{
		pStream.WriteByte( 48 + i * 2, wArguments[i] );
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPPartyMemberList()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet with response to client request to add someone to party
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xBF (General Information Packet)
//|					Subcommand: 0x06 (Party Commands)
//|					Subsubcommand: 0x01 (Party Add Member)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd (0xBF)
//|						BYTE[2] Length
//|						BYTE[2] Subcommand (0x06)
//|						Subcommand details
//|							BYTE subsubcommand (0x01)
//|							BYTE number of Members (total number of members in the party)
//|							Then, loop for each member in numMembers:
//|								BYTE[4] serial
//o------------------------------------------------------------------------------------------------o
CPPartyMemberList::CPPartyMemberList( void )
{
	InternalReset();
}

void CPPartyMemberList::InternalReset( void )
{
	pStream.ReserveSize( 7 );
	pStream.WriteByte( 0, 0xBF );	// packet ID
	pStream.WriteShort( 1, 7 );		// packet length
	pStream.WriteShort( 3, 6 );		// party command
	pStream.WriteByte( 5, 1 );		// subcommand
	pStream.WriteByte( 6, 0 );		// member Count
}

void CPPartyMemberList::AddMember( CChar *member )
{
	size_t curPos = pStream.GetSize();
	pStream.ReserveSize( curPos + 4 );
	pStream.WriteLong( curPos, member->GetSerial() );
	pStream.WriteByte( 6, pStream.GetByte( 6 ) + 1 );
	pStream.WriteShort( 1, static_cast<SI32>( curPos ) + 4 );
}

void CPPartyMemberList::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[SEND]Packet     : CPPartyMemberList 0xBF Subcommand Party Command --> Length: " << pStream.GetSize() << TimeStamp() << std::endl;
	}
	outStream << "Packet ID        : " << std::hex << static_cast<UI16>( pStream.GetByte( 0 )) << std::dec << std::endl;
	outStream << "Subcommand       : " << pStream.GetShort( 3 ) << std::endl;
	outStream << "Party Sub        : " << static_cast<UI16>( pStream.GetByte( 5 )) << std::endl;
	outStream << "Member Count     : " << static_cast<UI16>( pStream.GetByte( 6 )) << std::endl;
	for( SI32 i = 0; i < pStream.GetByte( 6 ); ++i )
	{
		outStream << "    Member #" << i << "    : " << std::hex << "0x" << pStream.GetLong( 7 + 4 * static_cast<size_t>( i )) << std::dec << std::endl;
	}
	outStream << "  Raw dump     :" << std::endl;
	CPUOXBuffer::Log( outStream, false );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPPartyInvitation()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet with invitation for player to join a party
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xBF (General Information Packet)
//|					Subcommand: 0x06 (Party Commands)
//|					Subsubcommand: 0x07 (Party Invitation)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd (0xBF)
//|						BYTE[2] Length
//|						BYTE[2] Subcommand (0x06)
//|						Subcommand details
//|							BYTE subsubcommand (0x07)
//|							BYTE[4] serial (party leader)
//o------------------------------------------------------------------------------------------------o
CPPartyInvitation::CPPartyInvitation( void )
{
	InternalReset();
}

void CPPartyInvitation::InternalReset( void )
{
	pStream.ReserveSize( 7 );
	pStream.WriteByte( 0, 0xBF );	// packet ID
	pStream.WriteShort( 1, 10 );	// packet length
	pStream.WriteShort( 3, 6 );		// party command
	pStream.WriteByte( 5, 7 );		// subcommand
}

void CPPartyInvitation::Leader( CChar *leader )
{
	pStream.WriteLong( 6, leader->GetSerial() );
}

void CPPartyInvitation::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[SEND]Packet     : CPPartyInvitation 0xBF Subcommand Party Command --> Length: " << pStream.GetSize() << TimeStamp() << std::endl;
	}
	outStream << "Packet ID        : " << std::hex << static_cast<UI16>( pStream.GetByte( 0 )) << std::dec << std::endl;
	outStream << "Subcommand       : " << pStream.GetShort( 3 ) << std::endl;
	outStream << "Party Sub        : " << static_cast<UI16>( pStream.GetByte( 5 )) << std::endl;
	outStream << "Leader Serial    : 0x" << std::hex << pStream.GetLong( 6 ) << std::dec << std::endl;
	outStream << "  Raw dump     :" << std::endl;
	CPUOXBuffer::Log( outStream, false );
}


//o------------------------------------------------------------------------------------------------o
//| Function	-	CPPartyMemberRemove()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to remove member from party
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xBF (General Information Packet)
//|					Subcommand: 0x06 (Party Commands)
//|					Subsubcommand: 0x02 (Remove Party Member)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd (0xBF)
//|						BYTE[2] Length
//|						BYTE[2] Subcommand (0x06)
//|						Subcommand details
//|							BYTE subsubcommand (0x02)
//|							BYTE	membersInNewParty
//|							BYTE[4]	idOfRemovedPlayer
//|							For each member
//|								BYTE[4]	memberID
//o------------------------------------------------------------------------------------------------o
CPPartyMemberRemove::CPPartyMemberRemove( CChar *removed )
{
	InternalReset();
	pStream.WriteLong( 7, removed->GetSerial() );
}

void CPPartyMemberRemove::InternalReset( void )
{
	pStream.ReserveSize( 11 );
	pStream.WriteByte( 0, 0xBF );	// packet ID
	pStream.WriteShort( 1, 11 );		// packet length
	pStream.WriteShort( 3, 6 );		// party command
	pStream.WriteByte( 5, 2 );		// subcommand
	pStream.WriteByte( 6, 0 );		// member Count
}

void CPPartyMemberRemove::AddMember( CChar *member )
{
	size_t curPos = pStream.GetSize();
	pStream.ReserveSize( curPos + 4 );
	pStream.WriteLong( curPos, member->GetSerial() );
	pStream.WriteByte( 6, pStream.GetByte( 6 ) + 1 );
	pStream.WriteShort( 1, static_cast<SI32>( curPos ) + 4 );
}

void CPPartyMemberRemove::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[SEND]Packet     : CPPartyMemberRemove 0xBF Subcommand Party Command --> Length: " << pStream.GetSize() << TimeStamp() << std::endl;
	}
	outStream << "Packet ID        : " << std::hex << static_cast<UI16>( pStream.GetByte( 0 )) << std::dec << std::endl;
	outStream << "Subcommand       : " << pStream.GetShort( 3 ) << std::endl;
	outStream << "Party Sub        : " << static_cast<UI16>( pStream.GetByte( 5 )) << std::endl;
	outStream << "Member Count     : " << static_cast<UI16>( pStream.GetByte( 6 )) << std::endl;
	outStream << "Member Removed   : 0x" << std::hex << static_cast<UI16>( pStream.GetLong( 7 )) << std::dec << std::endl;
	outStream << "Members          : " << std::endl;
	for( SI32 i = 0; i < pStream.GetByte( 6 ); ++i )
	{
		outStream << "    Member #" << i << "    : " << std::hex << "0x" << pStream.GetLong( 11 + 4 * static_cast<size_t>( i )) << std::dec << std::endl;
	}
	outStream << "  Raw dump     :" << std::endl;
	CPUOXBuffer::Log( outStream, false );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPPartyTell()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to send a message to a specific party member
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xBF (General Information Packet)
//|					Subcommand: 0x06 (Party Commands)
//|					Subsubcommand: 0x03 (Tell party member a message)
//|					Size: Variable
//|
//|					Packet Build
//|						BYTE cmd (0xBF)
//|						BYTE[2] Length
//|						BYTE[2] Subcommand (0x06)
//|						Subcommand details
//|							BYTE subsubcommand (0x03)
//|							BYTE[4] id (of target, from client, of source, from server)
//|							BYTE[n][2] Null terminated Unicode message.
//o------------------------------------------------------------------------------------------------o
CPPartyTell::CPPartyTell( CPIPartyCommand *removed, CSocket *talker )
{
	InternalReset();
	CSocket *talkSocket	= removed->GetSocket();
	// let's mirror tell all/indiv
	UI08 tellMode		= talkSocket->GetByte( 5 );
	pStream.WriteByte( 5, tellMode );
	pStream.WriteLong( 6, talker->CurrcharObj()->GetSerial() );

	UI16 sizeModifier = 6;
	if( tellMode == 3 )
	{
		sizeModifier = 10;
	}

	UI16 messageLength = talkSocket->GetWord( 1 ) - sizeModifier;

	pStream.ReserveSize( 12 + static_cast<size_t>( messageLength ));
	pStream.WriteShort( 1, 12 + messageLength );
	for( size_t i = 0; i < messageLength; ++i )
	{
		pStream.WriteByte( i + 10, talkSocket->GetByte( sizeModifier + i ));
	}
	pStream.WriteShort( 10 + static_cast<size_t>( messageLength ), 0 );
}

void CPPartyTell::InternalReset( void )
{
	pStream.ReserveSize( 12 );
	pStream.WriteByte( 0, 0xBF );	// packet ID
	pStream.WriteShort( 1, 12 );		// packet length
	pStream.WriteShort( 3, 6 );		// party command
	pStream.WriteByte( 5, 3 );		// subcommand
}

void CPPartyTell::Log( std::ostream &outStream, bool fullHeader )
{
	if( fullHeader )
	{
		outStream << "[SEND]Packet     : CPPartyTell 0xBF Subcommand Party Command --> Length: " << pStream.GetSize() << TimeStamp() << std::endl;
	}
	outStream << "Packet ID        : " << std::hex << static_cast<UI16>( pStream.GetByte( 0 )) << std::dec << std::endl;
	outStream << "Subcommand       : " << pStream.GetShort( 3 ) << std::endl;
	outStream << "Party Sub        : " << static_cast<UI16>( pStream.GetByte( 5 )) << std::endl;
	outStream << "Talker           : 0x" << std::hex << pStream.GetLong( 6 ) << std::dec << std::endl;
	outStream << "Message          : ";
	for( size_t j = 10; j < pStream.GetSize(); ++j )
	{
		outStream << static_cast<UI08>( pStream.GetByte( j ));
	}
	outStream << std::endl;
	outStream << "  Raw dump     :" << std::endl;
	CPUOXBuffer::Log( outStream, false );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPClientVersion()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet to request client version
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0xBD (Client Version Request)
//|					Size: 3 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|						BYTE[2] length
//|
//|					Notes
//|						Server version : 0xbd 0x0 0x3 (client replies with client version of this packet)
//|						Clients sends a client version of this packet ONCE at login (without server request.)
//o------------------------------------------------------------------------------------------------o
void CPClientVersion::InternalReset( void )
{
	pStream.ReserveSize( 3 );
	pStream.WriteByte( 0, 0xBD );
	pStream.WriteByte( 2, 0x03 );
}
CPClientVersion::CPClientVersion()
{
	InternalReset();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CPDropItemApproved()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Handles outgoing packet
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Packet: 0x29 (Drop Item Approved)
//|					Size: 1 bytes
//|
//|					Packet Build
//|						BYTE cmd
//|
//|					Notes
//|						Older 2D Clients this was for Paperdoll ackknowledgement.
//|						Newest (KR era and 6.0.1.7 and higher) use this also for the regular drop item packet.
//|							> 6.0.1.7 = Client sends Drop On Paperdoll (0x13), Server Responds with This (0x29)
//|							< 6.0.1.7 = Client sends Drop On Paperdoll (0x13) or sends Drop Item (0x08), server responds with this (0x29)
//o------------------------------------------------------------------------------------------------o
CPDropItemApproved::CPDropItemApproved()
{
	pStream.ReserveSize( 1 );
	pStream.WriteByte( 0, 0x29 );
}
