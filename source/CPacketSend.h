#ifndef __CPACKETSEND_H__
#define __CPACKETSEND_H__

#include "network.h"
#include "CPacketReceive.h"

namespace UOX
{

class CPCharLocBody : public cPUOXBuffer
{
protected:
	virtual void	CopyData( CChar &toCopy );
	virtual void	InternalReset( void );
public:
					CPCharLocBody();
					CPCharLocBody( CChar &toCopy );
	virtual			~CPCharLocBody();
	virtual void	Flag( UI08 toPut );
	virtual void	HighlightColour( UI08 color );
	CPCharLocBody &	operator=( CChar &toCopy );
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true );
};

class CPacketSpeech : public cPUOXBuffer
{
protected:
	bool			isUnicode;

	virtual void	CopyData( CSpeechEntry &toCopy );
public:
					CPacketSpeech();
					CPacketSpeech( CSpeechEntry &toCopy );
	virtual void	SpeakerSerial( SERIAL toPut );
	virtual void	SpeakerModel( UI16 toPut );
	virtual void	Colour( COLOUR toPut );
	virtual void	Font( FontType toPut );
	virtual void	Language( UnicodeTypes toPut );
	virtual void	Unicode( bool toPut );
	virtual void	Type( SpeechType toPut );
	virtual void	Speech( const std::string toPut );
	virtual void	SpeakerName( const std::string toPut );
	virtual			~CPacketSpeech();
	CPacketSpeech	&operator=( CSpeechEntry &toCopy );
	CPacketSpeech	&operator=( CPacketSpeech &toCopy );
};

class CPWalkDeny : public cPUOXBuffer
{
public:
					CPWalkDeny();
					virtual ~CPWalkDeny()
					{
					}
	virtual void	SequenceNumber( char newValue );
	virtual void	X( SI16 newValue );
	virtual void	Y( SI16 newValue );
	virtual void	Z( SI08 newValue );
	virtual void	Direction( char newValue );
};

class CPWalkOK : public cPUOXBuffer
{
public:
					CPWalkOK();
					virtual ~CPWalkOK()
					{
					}
	virtual void	SequenceNumber( char newValue );
	virtual void	OtherByte( char newValue );
};

class CPExtMove : public cPUOXBuffer
{
protected:
	void			CopyData( CChar &toCopy );
public:
					CPExtMove();
					virtual	~CPExtMove()
					{
					}
					CPExtMove( CChar &toCopy );
	virtual void	FlagColour( UI08 newValue );
	CPExtMove		&operator=( CChar &toCopy );
};

class CPAttackOK : public cPUOXBuffer
{
protected:
	void			CopyData( CChar &toCopy );
public:
					CPAttackOK();
		virtual			~CPAttackOK()
		{
		}
					CPAttackOK( CChar &toCopy );
	virtual void	Serial( SERIAL newSerial );
	CPAttackOK		&operator=( CChar &toCopy );
};

class CPRemoveItem : public cPUOXBuffer
{
protected:
	void			CopyData( CBaseObject &toCopy );
public:
					CPRemoveItem();
		virtual ~	CPRemoveItem()
		{
		}
					CPRemoveItem( CBaseObject &toCopy );
	virtual void	Serial( SERIAL newSerial );
	CPRemoveItem	&operator=( CBaseObject &toCopy );
};

class CPWorldChange : public cPUOXBuffer
{
public:
					CPWorldChange();
		virtual		~CPWorldChange()
		{
		}
					CPWorldChange( WorldType newSeason, UI08 newCursor );
	virtual void	Season( WorldType newSeason );
	virtual void	Cursor( UI08 newCursor );
};

class CPLightLevel : public cPUOXBuffer
{
public:
					CPLightLevel();
	virtual			~CPLightLevel()
	{
	}
					CPLightLevel( LIGHTLEVEL level );
	virtual void	Level( LIGHTLEVEL level );
};

class CPUpdIndSkill : public cPUOXBuffer
{
protected:
	virtual void	CopyData( CChar& i, UI08 sNum );
	virtual void	InternalReset( void );
public:
					CPUpdIndSkill();
	virtual			~CPUpdIndSkill()
	{
	}
					CPUpdIndSkill( CChar& i, UI08 sNum );
	virtual void	Character( CChar& i, UI08 sNum );
	virtual void	SkillNum( UI08 sNum );
	virtual void	Skill( SI16 skillval );
	virtual void	BaseSkill( SI16 skillval );
	virtual void	Lock( UI08 lockVal );
};

class CPBuyItem : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
	virtual void	CopyData( CBaseObject &i );
public:
					CPBuyItem();
	virtual			~CPBuyItem()
	{
	}
					CPBuyItem( CBaseObject &i );
	CPBuyItem		&operator=( CBaseObject &toCopy );
	void			Serial( SERIAL toSet );
};

class CPRelay : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
public:
					CPRelay();
	virtual			~CPRelay()
	{
	}
					CPRelay( long newIP );
					CPRelay( long newIP, UI16 newPort );
	virtual void	ServerIP( long newIP );
	virtual void	Port( UI16 newPort );
	virtual void	SeedIP( long newIP );
};

class CPWornItem : public cPUOXBuffer
{
protected:
	virtual void	CopyData( CItem &toCopy );
public:
					CPWornItem();
	virtual			~CPWornItem()
	{
	}
					CPWornItem( CItem &toCopy );
	virtual void	ItemSerial( SERIAL itemSer );
	virtual void	Model( SI16 newModel );
	virtual void	Layer( UI08 layer );
	virtual void	CharSerial( SERIAL chSer );
	virtual void	Colour( SI16 newColour );
	CPWornItem		&operator=( CItem &toCopy );
};

class CPCharacterAnimation : public cPUOXBuffer
{
protected:
	virtual void	CopyData( CChar &toCopy );
	virtual void	InternalReset( void );
public:
					CPCharacterAnimation();
	virtual			~CPCharacterAnimation()
	{
	}
					CPCharacterAnimation( CChar &toCopy );
	virtual void	Serial( SERIAL toSet );
	virtual void	Action( UI16 model );
	virtual void	Direction( UI08 dir );
	virtual void	Repeat( SI16 repeatValue );
	virtual void	DoBackwards( bool newValue );
	virtual void	RepeatFlag( bool newValue );
	virtual void	FrameDelay( UI08 delay );
	CPCharacterAnimation &operator=( CChar &toCopy );
};

class CPDrawGamePlayer : public cPUOXBuffer
{
protected:
	virtual void	CopyData( CChar &toCopy );
	virtual void	InternalReset( void );
public:
					CPDrawGamePlayer();
		virtual		~CPDrawGamePlayer()
		{
		}
					CPDrawGamePlayer( CChar &toCopy );
	CPDrawGamePlayer &operator=( CChar &toCopy );
};

class CPPersonalLightLevel : public cPUOXBuffer
{
protected:
	virtual void	CopyData( CChar &toCopy );
	virtual void	InternalReset( void );
public:
					CPPersonalLightLevel();
		virtual		~CPPersonalLightLevel()
		{
		}
					CPPersonalLightLevel( CChar &toCopy );
	virtual void	Serial( SERIAL toSet );
	virtual void	Level( UI08 lightLevel );
	CPPersonalLightLevel &operator=( CChar &toCopy );
};

class CPPlaySoundEffect : public cPUOXBuffer
{
protected:
	virtual void	CopyData( CBaseObject &toCopy );
	virtual void	InternalReset( void );
public:
					CPPlaySoundEffect();
		virtual		~CPPlaySoundEffect()
		{
		}
					CPPlaySoundEffect( CBaseObject &toCopy );
	virtual void	Mode( UI08 mode );
	virtual void	Model( UI16 newModel );
	virtual void	X( SI16 xLoc );
	virtual void	Y( SI16 yLoc );
	virtual void	Z( SI16 zLoc );
	CPPlaySoundEffect &operator=( CBaseObject &toCopy );
};

class CPPaperdoll : public cPUOXBuffer
{
protected:
	virtual void	CopyData( CChar &toCopy );
	virtual void	InternalReset( void );
public:
					CPPaperdoll();
	virtual			~CPPaperdoll()
	{
	}
					CPPaperdoll( CChar &toCopy );
	virtual void	Serial( SERIAL tSerial );
	virtual void	FlagByte( UI08 fVal );
	virtual void	Text( const std::string toPut );
	CPPaperdoll &	operator=( CChar &toCopy );
};

class CPWeather : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
public:
					CPWeather();
	virtual			~CPWeather()
	{
	}
					CPWeather( UI08 nType );
					CPWeather( UI08 nType, UI08 nParts );
					CPWeather( UI08 nType, UI08 nParts, UI08 nTemp );
	virtual void	Type( UI08 nType );
	virtual void	Particles( UI08 nParts );
	virtual void	Temperature( UI08 nTemp );
};

class CPGraphicalEffect : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
public:
	virtual			~CPGraphicalEffect()
	{
	}
					CPGraphicalEffect( UI08 effectType );
					CPGraphicalEffect( UI08 effectType, CBaseObject &src, CBaseObject &trg );
					CPGraphicalEffect( UI08 effectType, CBaseObject &src );
	virtual void	Effect( UI08 effectType );
	virtual void	SourceSerial( CBaseObject &toSet );
	virtual void	SourceSerial( SERIAL toSet );
	virtual void	TargetSerial( CBaseObject &toSet );
	virtual void	TargetSerial( SERIAL toSet );
	virtual void	Model( SI16 nModel );
	virtual void	X( SI16 nX );
	virtual void	Y( SI16 nY );
	virtual void	Z( SI08 nZ );
	virtual void	XTrg( SI16 nX );
	virtual void	YTrg( SI16 nY );
	virtual void	ZTrg( SI08 nZ );
	virtual void	Speed( UI08 nSpeed );
	virtual void	Duration( UI08 nDuration );
	virtual void	AdjustDir( bool nValue );
	virtual void	ExplodeOnImpact( bool nValue );
	virtual void	SourceLocation( CBaseObject &toSet );
	virtual void	SourceLocation( SI16 x, SI16 y, SI08 z );
	virtual void	TargetLocation( CBaseObject &toSet );
	virtual void	TargetLocation( SI16 x, SI16 y, SI08 z );
};

class CPUpdateStat : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
public:
	virtual ~CPUpdateStat()
	{
	}
					CPUpdateStat( CChar &toUpdate, UI08 statNum );
	virtual void	Serial( SERIAL toSet );
	virtual void	MaxVal( SI16 maxVal );
	virtual void	CurVal( SI16 curVal );
};

class CPDeathAction : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
public:
					CPDeathAction( CChar &dying, CItem &corpse );
					CPDeathAction();
	virtual			~CPDeathAction()
	{
	}
	virtual void	Player( SERIAL toSet );
	virtual void	Corpse( SERIAL toSet );
	virtual void	FallDirection( UI08 toFall );
	CPDeathAction &	operator=( CChar &dying );
	CPDeathAction & operator=( CItem &corpse );
};

class CPPlayMusic : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
public:
					CPPlayMusic( SI16 musicID );
					CPPlayMusic();
	virtual			~CPPlayMusic()
	{
	}
	virtual void	MusicID( SI16 musicID );
};

class CPDrawContainer : public cPUOXBuffer
{
protected:
	virtual void	CopyData( CItem &toCopy );
	virtual void	InternalReset( void );
public:
					CPDrawContainer();
	virtual			~CPDrawContainer()
	{
	}
					CPDrawContainer( CItem &toCopy );
	virtual void	Model( UI16 newModel );
	virtual void	Serial( SERIAL toSet );
	CPDrawContainer &operator=( CItem &toCopy );
};

//	0x7C Packet
//	Last Modified on Thursday, 03-Sep-2002
//	Open Dialog Box (Variable # of bytes) 
//	BYTE cmd 
//	BYTE[2] blockSize 
//	BYTE[4] dialogID (echo'd back to the server in 7d) 
//	BYTE[2] menuid (echo'd back to server in 7d) 
//	BYTE length of question 
//	BYTE[length of question] question text 
//	BYTE # of responses 
//	Then for each response: 
//		BYTE[2] model id # of shown item (if grey menu -- then always 0x00 as msb) 
//		BYTE[2] color of shown item
//		BYTE response text length 
//		BYTE[response text length] response text 

class CPOpenGump : public cPUOXBuffer
{
protected:
	virtual void	CopyData( CChar &toCopy );
	virtual void	InternalReset( void );
	size_t			responseOffset;
	size_t			responseBaseOffset;
public:
					CPOpenGump();
	virtual			~CPOpenGump()
	{
	}
					CPOpenGump( CChar &toCopy );
	virtual void	Length( int TotalLines );
	virtual void	GumpIndex( int index );
	virtual void	Serial( SERIAL toSet );
	virtual void	Question( std::string toAdd );
	virtual void	AddResponse( UI16 modelNum, UI16 colour, std::string responseText );
	virtual void	Finalize( void );
	CPOpenGump &operator=( CChar &toCopy );
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true );
};

class CPTargetCursor : public cPUOXBuffer
{
public:
					CPTargetCursor();
	virtual			~CPTargetCursor()
	{
	}
	virtual void	Type( UI08 nType );
	virtual void	ID( SERIAL toSet );
	virtual void	CursorType( UI08 nType );	// There is more to this packet, but it's only valid when sent by the CLIENT
};

class CPStatWindow : public cPUOXBuffer
{
protected:
	bool			extended3;
	bool			extended4;
	virtual void	InternalReset( void );
public:
					CPStatWindow();
	virtual			~CPStatWindow()
	{
	}
					CPStatWindow( CChar &toCopy, CSocket &target );
	virtual void	Serial( SERIAL toSet );
	virtual void	Name( const std::string nName );
	virtual void	CurrentHP( SI16 nValue );
	virtual void	MaxHP( SI16 nValue );
	virtual void	NameChange( bool nValue );
	virtual void	Flag( UI08 nValue );
	virtual void	Sex( UI08 nValue );
	virtual void	Strength( SI16 nValue );
	virtual void	Dexterity( SI16 nValue );
	virtual void	Intelligence( SI16 nValue );
	virtual void	Stamina( SI16 nValue );
	virtual void	MaxStamina( SI16 nValue );
	virtual void	Mana( SI16 nValue );
	virtual void	MaxMana( SI16 nValue );
	virtual void	Gold( UI32 gValue );
	virtual void	AC( UI16 nValue );
	virtual void	Weight( UI16 nValue );
	virtual void	SetCharacter( CChar &toCopy, CSocket &target );
	virtual void	StatCap( UI16 value );
	virtual void	CurrentPets( UI08 value );
	virtual void	MaxPets( UI08 value );
	virtual void	FireResist( UI16 value );
	virtual void	ColdResist( UI16 value );
	virtual void	PoisonResist( UI16 value );
	virtual void	EnergyResist( UI16 value );
	virtual void	Luck( UI16 value );
	virtual void	DamageMax( UI16 value );
	virtual void	DamageMin( UI16 value );
	virtual void	Unknown( UI32 value );
};

class CPIdleWarning : public cPUOXBuffer
{
/*
0x53 Client Message:
				
	0x01=Char doesn't exist
	0x02=Char Already Exists 
	0x03,0x04=Cannot Attach to server 
	0x05=Char in world Msg
	0x06=Login server Syc Error
	0x07=Idle msg
	0x08 (and up?)=Can't Attach
*/
protected:
	virtual void	InternalReset( void );
public:
					CPIdleWarning();
	virtual			~CPIdleWarning()
	{
	}
					CPIdleWarning( UI08 errorNum );
	virtual void	Error( UI08 errorNum );
};

class CPTime : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
public:
					CPTime();
	virtual			~CPTime()
	{
	}
					CPTime( UI08 hour, UI08 minute, UI08 second );
	virtual void	Hour( UI08 hour );
	virtual void	Minute( UI08 minute );
	virtual void	Second( UI08 second );
};

class CPLoginComplete : public cPUOXBuffer
{
public:
					CPLoginComplete();
};

class CPTextEmoteColour : public cPUOXBuffer
{
public:
	virtual			~CPTextEmoteColour()
	{
	}
					CPTextEmoteColour();
	virtual void	BlockSize( SI16 newValue );
	virtual void	Unknown( SI16 newValue );
};

class CPWarMode : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
public:
	virtual			~CPWarMode()
	{
	}
					CPWarMode();
					CPWarMode( UI08 nFlag );
	virtual void	Flag( UI08 nFlag );
};

class CPPauseResume : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
public:
	virtual			~CPPauseResume()
	{
	}
					CPPauseResume();
					CPPauseResume( UI08 mode );
	virtual void	Mode( UI08 mode );
	virtual bool	ClientCanReceive( CSocket *mSock );
};

class CPWebLaunch : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
	virtual void	SetSize( SI16 newSize );
public:
	virtual			~CPWebLaunch()
	{
	}
					CPWebLaunch();
					CPWebLaunch( const std::string txt );
	virtual void	Text( const std::string txt );
};

class CPTrackingArrow : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
public:
	virtual			~CPTrackingArrow()
	{
	}
					CPTrackingArrow();
					CPTrackingArrow( SI16 x, SI16 y );
					CPTrackingArrow( CBaseObject &toCopy );
	virtual void	Location( SI16 x, SI16 y );
	virtual void	Active( UI08 value );
	CPTrackingArrow &operator=( CBaseObject &toCopy );
};

class CPBounce : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
public:
	virtual			~CPBounce()
	{
	}
					CPBounce();
					CPBounce( UI08 mode );
	virtual void	Mode( UI08 mode );
};

class CPDyeVat : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
	virtual void	CopyData( CBaseObject &target );
public:
	virtual			~CPDyeVat()
	{
	}
					CPDyeVat();
					CPDyeVat( CBaseObject &target );
	virtual void	Serial( SERIAL toSet );
	virtual void	Model( SI16 toSet );
	CPDyeVat &		operator=( CBaseObject &target );
};

class CPMultiPlacementView : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
	virtual void	CopyData( CItem &target );
public:
	virtual			~CPMultiPlacementView()
	{
	}
					CPMultiPlacementView();
					CPMultiPlacementView( CItem &target );
					CPMultiPlacementView( SERIAL toSet );
	virtual void	RequestType( UI08 rType );
	virtual void	DeedSerial( SERIAL toSet );
	virtual void	MultiModel( SI16 toSet );
	CPMultiPlacementView &operator=( CItem &target );
};

class CPEnableClientFeatures : public cPUOXBuffer
{
public:
					CPEnableClientFeatures();
};

class CPAddItemToCont : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
	virtual void	CopyData( CItem &toCopy );
public:
	virtual			~CPAddItemToCont()
	{
	}
					CPAddItemToCont();
					CPAddItemToCont( CItem &toAdd );
	virtual void	Serial( SERIAL toSet );
	virtual void	Model( SI16 toSet );
	virtual void	NumItems( SI16 toSet );
	virtual void	X( SI16 x );
	virtual void	Y( SI16 y );
	virtual void	Container( SERIAL toAdd );
	virtual void	Colour( SI16 toSet );
	CPAddItemToCont &operator=( CItem &toAdd );
};

class CPKickPlayer : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
	virtual void	CopyData( CChar &toCopy );
public:
	virtual			~CPKickPlayer()
	{
	}
					CPKickPlayer();
					CPKickPlayer( CChar &toCopy );
	virtual void	Serial( SERIAL toSet );
	CPKickPlayer &	operator=( CChar &toCopy );
};

class CPResurrectMenu : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
public:
	virtual			~CPResurrectMenu()
	{
	}
					CPResurrectMenu();
					CPResurrectMenu( UI08 action );
	virtual void	Action( UI08 action );
};

class CPFightOccurring : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
public:
	virtual			~CPFightOccurring()
	{
	}
					CPFightOccurring();
					CPFightOccurring( CChar &attacker, CChar &defender );
	virtual void	Attacker( SERIAL toSet );
	virtual void	Attacker( CChar &attacker );
	virtual void	Defender( SERIAL toSet );
	virtual void	Defender( CChar &defender );
};

class CPSkillsValues : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
	virtual void	CopyData( CChar &toCopy );
	virtual void	BlockSize( SI16 newValue );
	virtual void	WriteShort( SI16 offset, SI16 value );
	virtual UI08	NumSkills( void );
public:
	virtual			~CPSkillsValues()
	{
	}
					CPSkillsValues();
					CPSkillsValues( CChar &toCopy );
	virtual void	NumSkills( UI08 numSkills );
	virtual void	SetCharacter( CChar &toCopy );
	virtual void	SkillEntry( SI16 skillID, SI16 skillVal, SI16 baseSkillVal, UI08 skillLock );
	CPSkillsValues &operator=( CChar &toCopy );
};

class CPMapMessage : public cPUOXBuffer
{
public:
	virtual			~CPMapMessage()
	{
	}
					CPMapMessage();
	virtual void	UpperLeft( SI16 x, SI16 y );
	virtual void	LowerRight( SI16 x, SI16 y );
	virtual void	Dimensions( SI16 width, SI16 height );
	virtual void	GumpArt( SI16 newArt );
	virtual void	KeyUsed( long key );
};

class CPMapRelated : public cPUOXBuffer
{
public:
	virtual			~CPMapRelated()
	{
	}
					CPMapRelated();
	virtual void	PlotState( UI08 pState );
	virtual void	Location( SI16 x, SI16 y );
	virtual void	Command( UI08 cmd );
	virtual void	ID( SERIAL key );
};

class CPBookTitlePage : public cPUOXBuffer
{
public:
	virtual			~CPBookTitlePage()
	{
	}
					CPBookTitlePage();
	virtual void	Serial( SERIAL toSet );
	virtual void	WriteFlag( UI08 flag );
	virtual void	NewFlag( UI08 flag );
	virtual void	Pages( SI16 pages );
	virtual void	Title( const std::string txt );
	virtual void	Author( const std::string txt );
};

class CPUltimaMessenger : public cPUOXBuffer
{
public:
	virtual			~CPUltimaMessenger()
	{
	}
					CPUltimaMessenger();
	virtual void	ID1( SERIAL toSet );
	virtual void	ID2( SERIAL toSet );
};

class CPGumpTextEntry : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
	virtual void	BlockSize( SI16 newVal );
	virtual SI16	CurrentSize( void );
	virtual SI16	Text1Len( void );
	virtual void	Text1Len( SI16 newVal );
	virtual SI16	Text2Len( void );
	virtual void	Text2Len( SI16 newVal );
public:
	virtual			~CPGumpTextEntry()
	{
	}
					CPGumpTextEntry();
					CPGumpTextEntry( const std::string text );
					CPGumpTextEntry( const std::string text1, const std::string text2 );
	virtual void	Serial( SERIAL id );
	virtual void	ParentID( UI08 newVal );
	virtual void	ButtonID( UI08 newVal );
	virtual void	Cancel( UI08 newVal );	// 0 = disable, 1 = enable
	virtual void	Style( UI08 newVal );	// 0 = disable, 1 = normal, 2 = numerical
	virtual void	Format( SERIAL id );	// if style 1, max text len, if style 2, max num len
	virtual void	Text1( const std::string txt );
	virtual void	Text2( const std::string txt );
};

class CPMapChange : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
public:
					CPMapChange();
					CPMapChange( UI08 newMap );
					CPMapChange( CBaseObject *moving );
	virtual			~CPMapChange();
	virtual	void	SetMap( UI08 newMap );
	CPMapChange&	operator=( CBaseObject& moving );
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true );
};

class CPItemsInContainer : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
	virtual void	CopyData( CSocket *mSock, CItem& toCopy );
	bool			isVendor;
	bool			isCorpse;
	SERIAL			vendorSerial;
public:
	virtual			~CPItemsInContainer()
	{
	}
					CPItemsInContainer();
					CPItemsInContainer( CSocket *mSock, CItem *container, UI08 contType = 0x00 );
	virtual void	NumberOfItems( UI16 numItems );
	virtual UI16	NumberOfItems( void ) const;
	virtual void	AddItem( CItem *toAdd, UI16 itemNum );
	void			Add( UI16 itemNum, SERIAL toAdd, SERIAL cont, UI08 amount );
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true );
};

class CPCorpseClothing : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
	virtual void	CopyData( CItem& toCopy );
public:
	virtual			~CPCorpseClothing()
	{
	}
					CPCorpseClothing();
					CPCorpseClothing( CItem *corpse );
	virtual void	NumberOfItems( UI16 numItems );
	virtual void	AddItem( CItem *toAdd, UI16 itemNum );
	CPCorpseClothing& operator=( CItem& corpse );
};

class CPOpenBuyWindow : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
	virtual void	CopyData( CItem& toCopy, CChar *vendorID );
public:
	virtual			~CPOpenBuyWindow()
	{
	}
					CPOpenBuyWindow();
					CPOpenBuyWindow( CItem *container, CChar *vendorID );
	virtual void	NumberOfItems( UI08 numItems );
	virtual UI08	NumberOfItems( void ) const;
	virtual void	AddItem( CItem *toAdd, CTownRegion *tReg, UI16 &baseOffset );
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true );
};

class CPCharAndStartLoc : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
	virtual void	CopyData( ACCOUNTSBLOCK& toCopy );
public:
	virtual			~CPCharAndStartLoc()
	{
	}
					CPCharAndStartLoc();
					CPCharAndStartLoc(ACCOUNTSBLOCK& account, UI08 numCharacters, UI08 numLocations );
	virtual void	NumberOfLocations( UI08 numLocations );
	virtual void	NumberOfCharacters( UI08 numCharacters );
	virtual void	AddCharacter( CChar *toAdd, UI08 charOffset );
	virtual void	AddStartLocation( LPSTARTLOCATION sLoc, UI08 locOffset );
	CPCharAndStartLoc& operator=(ACCOUNTSBLOCK& actbBlock);
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true );
};

class CPUpdScroll : public cPUOXBuffer
{
protected:
	char			tipData[2048];
	virtual void	InternalReset( void );
	void			SetLength( UI16 len );
public:
					CPUpdScroll();
					CPUpdScroll( UI08 tType );
					CPUpdScroll( UI08 tType, UI08 tNum );
	virtual			~CPUpdScroll();
	void			AddString( const char *toAdd );
	void			AddStrings( const char *tag, const char *data );
	void			Finalize( void );
	void			TipNumber( UI08 tipNum );
	void			TipType( UI08 tType );
};

class CPGraphicalEffect2 : public CPGraphicalEffect
{
protected:
	virtual void	InternalReset( void );
public:
	virtual			~CPGraphicalEffect2()
	{
	}
					CPGraphicalEffect2( UI08 effectType );
					CPGraphicalEffect2( UI08 effectType, CBaseObject &src, CBaseObject &trg );
					CPGraphicalEffect2( UI08 effectType, CBaseObject &src );
	virtual void	Hue( UI32 hue );
	virtual void	RenderMode( UI32 mode );
};

class CPDrawObject : public cPUOXBuffer
{
protected:
	UI16			curLen;
	virtual void	InternalReset( void );
	void			SetLength( UI16 len );
	void			CopyData( CChar& mChar );
public:
					CPDrawObject();
					CPDrawObject( CChar &mChar );
	virtual			~CPDrawObject();
	void			Finalize( void );
	void			AddItem( CItem *toAdd );
	void			SetRepFlag( UI08 value );
	void			SetCharFlag( UI08 value );
	CPDrawObject&	operator=( CChar& mChar );
};

class CPObjectInfo : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
	void			CopyData( CItem& mItem, CChar& mChar );
	void			CopyItemData( CItem& mItem, CChar& mChar );
	void			CopyMultiData( CMultiObj& mObj, CChar& mChar );
public:
					CPObjectInfo();
					CPObjectInfo( CItem& mItem, CChar& mChar );
	virtual			~CPObjectInfo();
	void			Objects( CItem& mItem, CChar& mChar );
};

class CPGameServerList : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
public:
	virtual			~CPGameServerList()
	{
	}
					CPGameServerList();
					CPGameServerList( UI16 numServers );
	virtual void	NumberOfServers( UI16 numItems );
	virtual void	AddServer( UI16 servNum, physicalServer *data );
};

class CPSecureTrading : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
	void			CopyData( CBaseObject& mItem, CBaseObject& mItem2, CBaseObject& mItem3 );
	void			CopyData( CBaseObject& mItem, SERIAL mItem2, SERIAL mItem3 );
public:
					CPSecureTrading();
					CPSecureTrading( CBaseObject& mItem, CBaseObject& mItem2, CBaseObject& mItem3 );
					CPSecureTrading( CBaseObject& mItem, SERIAL mItem2, SERIAL mItem3 );
	virtual			~CPSecureTrading();
	void			Objects( CBaseObject& mItem, CBaseObject& mItem2, CBaseObject& mItem3 );
	void			Objects( CBaseObject& mItem, SERIAL mItem2, SERIAL mItem3 );
	void			Action( UI08 value );
	void			Name( const std::string nameFollowing );
};

class CPGodModeToggle : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
	virtual void	CopyData( CSocket *s );
public:
	virtual			~CPGodModeToggle()
	{
	}
					CPGodModeToggle();
					CPGodModeToggle( CSocket *s );
	CPGodModeToggle& operator=( CSocket *s );
	virtual void	ToggleStatus( bool toSet );
};

enum LoginDenyReason
{
	LDR_UNKNOWNUSER = 0,
	LDR_ACCOUNTINUSE,
	LDR_ACCOUNTDISABLED,
	LDR_BADPASSWORD,
	LDR_COMMSFAILURE,
	LDR_NODENY
};

class CPLoginDeny : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
public:
	virtual			~CPLoginDeny()
	{
	}
					CPLoginDeny();
					CPLoginDeny( LoginDenyReason reason );
	virtual void	DenyReason( LoginDenyReason reason );
};

class CPClientVersion : public cPUOXBuffer
{
protected:
public:
					CPClientVersion();
};

// This packet is specific for Krrios' client
class CPKAccept : public cPUOXBuffer
{
public:
					CPKAccept( UI08 Response );
};

class CPSpeech : public cPUOXBuffer
{
protected:
	void			CopyData( CBaseObject &toCopy );
	void			CopyData( CPITalkRequest &talking );
	void			InternalReset( void );
	void			SetLength( UI16 value );
public:
					CPSpeech();
	virtual			~CPSpeech()
	{
	}
					CPSpeech( CBaseObject &toCopy );
					CPSpeech( CPITalkRequest &talking );
	void			ID( UI16 toSet );
	void			GrabSpeech( CSocket *mSock, CChar *mChar );
	void			Serial( SERIAL toSet );
	void			Object( CPITalkRequest &tSaid );
	void			Object( CBaseObject &toCopy );
	void			Type( UI08 value );
	void			Colour( COLOUR value );
	void			Font( UI16 value );
	void			Name( std::string value );
	void			Message( char *value );
	void			GhostIt( UI08 method );
	CPSpeech &operator=( CPSpeech &copyFrom );
	CPSpeech &operator=( CBaseObject &toCopy );
	CPSpeech &operator=( CPITalkRequest &talking );
};

class CPUnicodeSpeech : public cPUOXBuffer
{
protected:
	void			CopyData( CBaseObject &toCopy );
	void			CopyData( CPITalkRequestAscii &talking );
	void			CopyData( CPITalkRequestUnicode &talking );
	void			InternalReset( void );
	void			SetLength( UI16 value );
public:
					CPUnicodeSpeech();
	virtual			~CPUnicodeSpeech()
	{
	}
					CPUnicodeSpeech( CBaseObject &toCopy );
					CPUnicodeSpeech( CPITalkRequestAscii &talking );
					CPUnicodeSpeech( CPITalkRequestUnicode &talking );
	void			ID( UI16 toSet );
	void			GrabSpeech( CSocket *mSock, CChar *mChar );
	void			Serial( SERIAL toSet );
	void			Object( CPITalkRequestAscii &tSaid );
	void			Object( CPITalkRequestUnicode &tSaid );
	void			Object( CBaseObject &toCopy );
	void			Language( char *value );
	void			Type( UI08 value );
	void			Colour( COLOUR value );
	void			Font( UI16 value );
	void			Name( std::string value );
	void			Message( char *value );
	void			GhostIt( UI08 method );
	CPUnicodeSpeech &operator=( CPUnicodeSpeech &copyFrom );
	CPUnicodeSpeech &operator=( CBaseObject &toCopy );
	CPUnicodeSpeech &operator=( CPITalkRequestAscii &talking );
	CPUnicodeSpeech &operator=( CPITalkRequestUnicode &talking );
};

class CPAllNames3D : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
	virtual void	CopyData( CBaseObject& obj );
public:
	virtual			~CPAllNames3D()
	{
	}
					CPAllNames3D();
					CPAllNames3D( CBaseObject& obj );
	void			Object( CBaseObject& obj );
};

class CPBookPage : public cPUOXBuffer
{
protected:
	UI16			bookLength;
	UI08			pageCount;
	virtual void	InternalReset( void );
	virtual void	CopyData( CItem& obj );
	void			IncLength( UI08 amount );
public:
	virtual			~CPBookPage()
	{
	}
					CPBookPage();
					CPBookPage( CItem& obj );
	void			Object( CItem& obj );
	void			Serial( SERIAL value );
	void			NewPage( SI16 pNum = -1 );
	void			AddLine( const std::string line );
	void			Finalize( void );
};

class CPSendGumpMenu : public cPUOXBuffer
{
protected:
	STRINGLIST		commands, text;
public:
	virtual			~CPSendGumpMenu()
	{
	}
					CPSendGumpMenu();
	void			UserID( SERIAL value );
	void			GumpID( SERIAL value );
	void			X( UI32 value );
	void			Y( UI32 value );
	void			AddCommand( const char *actualCommand, ... );
	void			AddCommand( const std::string actualCommand, ... );
	void			AddText( const char *actualText, ... );
	void			AddText( const std::string actualText, ... );
	void			Finalize( void );
	virtual void	Log( std::ofstream &outStream, bool fullHeader );
};

class CPNewSpellBook : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
	virtual void	CopyData( CItem& obj );
public:
	virtual			~CPNewSpellBook()
	{
	}
					CPNewSpellBook();
					CPNewSpellBook( CItem& obj );
	virtual bool	ClientCanReceive( CSocket *mSock );
};

class CPDisplayDamage : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
	virtual void	CopyData( CChar& ourTarg, UI16 ourDamage );
public:
	virtual			~CPDisplayDamage()
	{
	}
					CPDisplayDamage();
					CPDisplayDamage( CChar& ourTarg, UI16 ourDamage );
	virtual bool	ClientCanReceive( CSocket *mSock );
};

class CPQueryToolTip : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
	virtual void	CopyData( CBaseObject& mObj );
public:
	virtual			~CPQueryToolTip()
	{
	}
					CPQueryToolTip();
					CPQueryToolTip( CBaseObject& mObj );
	virtual bool	ClientCanReceive( CSocket *mSock );
};

class CPToolTip : public cPUOXBuffer
{
protected:
	struct toolTipEntry
	{
		std::string ourText;
		UI32 stringNum;
		size_t stringLen;
	};
	std::vector< toolTipEntry > ourEntries;
	virtual void	InternalReset( void );
	virtual void	CopyData( SERIAL objSer );
	void				CopyItemData( CItem& cItem, size_t &totalStringLen );
	void				CopyCharData( CChar& mChar, size_t &totalStringLen );
	void				FinalizeData( toolTipEntry tempEntry, size_t &totalStringLen );
public:
	virtual			~CPToolTip()
	{
	}
					CPToolTip();
					CPToolTip( SERIAL objSer );
};

class CPSellList : public cPUOXBuffer
{
protected:
	virtual void	InternalReset( void );
	virtual void	CopyData( CChar& mChar, CChar& vendor );
public:
	virtual			~CPSellList()
	{
	}
					CPSellList();
					CPSellList( CChar& mChar, CChar& vendor );
	void			AddContainer( CTownRegion *tReg, CItem *spItem, CItem *ourPack, UI16 &numItems, size_t &packetLen );
	void			AddItem( CTownRegion *tReg, CItem *spItem, CItem *opItem, size_t &packetLen );
};

}

#endif

