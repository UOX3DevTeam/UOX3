#ifndef __CPACKETSEND_H__
#define __CPACKETSEND_H__

#include "CPacketReceive.h"


// Forward declare
struct __STARTLOCATIONDATA__;

class CPCharLocBody : public CPUOXBuffer
{
protected:
	virtual void	CopyData( CChar &toCopy );
	virtual void	InternalReset( void ) override;
public:
	CPCharLocBody();
	CPCharLocBody( CChar &toCopy );
	virtual			~CPCharLocBody()
	{
	}
	virtual void	Flag( UI08 toPut );
	virtual void	HighlightColour( UI08 color );
	CPCharLocBody &	operator = ( CChar &toCopy );
	virtual void	Log( std::ostream &outStream, bool fullHeader = true ) override;
};

class CPacketSpeech : public CPUOXBuffer
{
protected:
	bool			isUnicode;

	virtual void	InternalReset( void ) override;
	void			CopyData( CSpeechEntry &toCopy );
	void			CopyData( CPITalkRequest &toCopy );
public:
	CPacketSpeech();
	CPacketSpeech( CSpeechEntry &toCopy );
	CPacketSpeech( CPITalkRequest &toCopy );
	virtual void	SpeakerSerial( SERIAL toPut );
	virtual void	SpeakerModel( UI16 toPut );
	virtual void	Colour( COLOUR toPut );
	virtual void	Font( FontType toPut );
	virtual void	Language( UnicodeTypes toPut );
	virtual void	Unicode( bool toPut );
	virtual void	Type( SpeechType toPut );
	virtual void	Speech( const std::string& toPut );
	virtual void	SpeakerName( const std::string& toPut );
	void	GhostIt( UI08 method );
	virtual			~CPacketSpeech()
	{
	}
	CPacketSpeech	&operator = ( CSpeechEntry &toCopy );
};

class CPWalkDeny : public CPUOXBuffer
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

class CPWalkOK : public CPUOXBuffer
{
public:
	CPWalkOK();
	virtual ~CPWalkOK()
	{
	}
	virtual void	SequenceNumber( char newValue );
	virtual void	FlagColour( UI08 newValue );
};

class CPExtMove : public CPUOXBuffer
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
	virtual void	SetFlags( CChar &totoCopy );
	CPExtMove		&operator = ( CChar &toCopy );
};

class CPAttackOK : public CPUOXBuffer
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
	CPAttackOK		&operator = ( CChar &toCopy );
};

class CPRemoveItem : public CPUOXBuffer
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
	CPRemoveItem	&operator = ( CBaseObject &toCopy );
};

class CPWorldChange : public CPUOXBuffer
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

class CPLightLevel : public CPUOXBuffer
{
public:
	CPLightLevel();
	virtual			~CPLightLevel()
	{
	}
	CPLightLevel( LIGHTLEVEL level );
	virtual void	Level( LIGHTLEVEL level );
};

class CPUpdIndSkill : public CPUOXBuffer
{
protected:
	virtual void	CopyData( CChar& i, UI08 sNum );
	virtual void	InternalReset( void ) override;
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
	virtual void	Lock( SkillLock lockVal );
	virtual void	Cap( SI16 capVal );
};

class CPBuyItem : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
	virtual void	CopyData( CBaseObject &i );
public:
	CPBuyItem();
	virtual			~CPBuyItem()
	{
	}
	CPBuyItem( CBaseObject &i );
	CPBuyItem		&operator = ( CBaseObject &toCopy );
	void			Serial( SERIAL toSet );
};

class CPRelay : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
public:
	CPRelay();
	virtual			~CPRelay()
	{
	}
	CPRelay( UI32 newIP );
	CPRelay( UI32 newIP, UI16 newPort );
	virtual void	ServerIP( UI32 newIP );
	virtual void	Port( UI16 newPort );
	virtual void	SeedIP( UI32 newIP );
};

class CPWornItem : public CPUOXBuffer
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
	CPWornItem		&operator = ( CItem &toCopy );
};

class CPCharacterAnimation : public CPUOXBuffer
{
protected:
	virtual void	CopyData( CChar &toCopy );
	virtual void	InternalReset( void ) override;
public:
	CPCharacterAnimation();
	virtual			~CPCharacterAnimation()
	{
	}
	CPCharacterAnimation( CChar &toCopy );
	virtual void	Serial( SERIAL toSet );
	virtual void	Action( UI16 model );
	virtual void	FrameCount( UI08 frameCount );
	virtual void	Repeat( SI16 repeatValue );
	virtual void	DoBackwards( bool newValue );
	virtual void	RepeatFlag( bool newValue );
	virtual void	FrameDelay( UI08 delay );
	CPCharacterAnimation &operator = ( CChar &toCopy );
};

class CPNewCharacterAnimation : public CPUOXBuffer
{
protected:
	virtual void	CopyData( CChar &toCopy );
	virtual void	InternalReset( void ) override;
public:
	CPNewCharacterAnimation();
	virtual			~CPNewCharacterAnimation()
	{
	}
	CPNewCharacterAnimation( CChar &toCopy );
	virtual void	Serial( SERIAL toSet );
	virtual void	Action( UI16 action );
	virtual void	SubAction( UI16 subAction );
	virtual void	SubSubAction( UI08 subSubAction );
	CPNewCharacterAnimation &operator = ( CChar &toCopy );
};

class CPDrawGamePlayer : public CPUOXBuffer
{
protected:
	virtual void	CopyData( CChar &toCopy );
	virtual void	InternalReset( void ) override;
public:
	CPDrawGamePlayer();
	virtual		~CPDrawGamePlayer()
	{
	}
	CPDrawGamePlayer( CChar &toCopy );
};

class CPPersonalLightLevel : public CPUOXBuffer
{
protected:
	virtual void	CopyData( CChar &toCopy );
	virtual void	InternalReset( void ) override;
public:
	CPPersonalLightLevel();
	virtual		~CPPersonalLightLevel()
	{
	}
	CPPersonalLightLevel( CChar &toCopy );
	virtual void	Serial( SERIAL toSet );
	virtual void	Level( UI08 lightLevel );
	CPPersonalLightLevel &operator = ( CChar &toCopy );
};

class CPPlaySoundEffect : public CPUOXBuffer
{
protected:
	virtual void	CopyData( CBaseObject &toCopy );
	virtual void	InternalReset( void ) override;
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
	CPPlaySoundEffect &operator = ( CBaseObject &toCopy );
};

class CPPaperdoll : public CPUOXBuffer
{
protected:
	virtual void	CopyData( CChar &toCopy );
	virtual void	InternalReset( void ) override;
public:
	CPPaperdoll();
	virtual			~CPPaperdoll()
	{
	}
	CPPaperdoll( CChar &toCopy );
	virtual void	Serial( SERIAL tSerial );
	virtual void	FlagByte( UI08 fVal );
	virtual void	Text( const std::string& toPut );
	CPPaperdoll &	operator = ( CChar &toCopy );
};

class CPWeather : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
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

class CPGraphicalEffect : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
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

class CPUpdateStat : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
public:
	virtual ~CPUpdateStat()
	{
	}
	CPUpdateStat( CBaseObject &toUpdate, UI08 statNum, bool normalizeStats );
	virtual void	Serial( SERIAL toSet );
	virtual void	MaxVal( SI16 maxVal );
	virtual void	CurVal( SI16 curVal );
};

class CPDeathAction : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
public:
	CPDeathAction( CChar &dying, CItem &corpse );
	CPDeathAction();
	virtual			~CPDeathAction()
	{
	}
	virtual void	Player( SERIAL toSet );
	virtual void	Corpse( SERIAL toSet );
	virtual void	FallDirection( UI08 toFall );
	CPDeathAction &	operator = ( CChar &dying );
	CPDeathAction & operator = ( CItem &corpse );
};

class CPPlayMusic : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
public:
	CPPlayMusic( SI16 musicId );
	CPPlayMusic();
	virtual			~CPPlayMusic()
	{
	}
	virtual void	MusicId( SI16 musicId );
};

class CPDrawContainer : public CPUOXBuffer
{
protected:
	virtual void	CopyData( CItem &toCopy );
	virtual void	InternalReset( void ) override;
public:
	CPDrawContainer();
	virtual			~CPDrawContainer()
	{
	}
	CPDrawContainer( CItem &toCopy );
	virtual void	Model( UI16 newModel );
	virtual void	ContType( UI16 contType );
	virtual void	Serial( SERIAL toSet );
	CPDrawContainer &operator = ( CItem &toCopy );
};

//	0x7C Packet
//	Last Modified on Thursday, 03-Sep-2002
//	Open Dialog Box (Variable # of bytes)
//	BYTE cmd
//	BYTE[2] blockSize
//	BYTE[4] dialogId (echo'd back to the server in 7d)
//	BYTE[2] menuid (echo'd back to server in 7d)
//	BYTE length of question
//	BYTE[length of question] question text
//	BYTE # of responses
//	Then for each response:
//		BYTE[2] model id # of shown item (if grey menu -- then always 0x00 as msb)
//		BYTE[2] color of shown item
//		BYTE response text length
//		BYTE[response text length] response text

class CPOpenGump : public CPUOXBuffer
{
protected:
	virtual void	CopyData( CChar &toCopy );
	virtual void	InternalReset( void ) override;
	size_t			responseOffset;
	size_t			responseBaseOffset;
public:
	CPOpenGump();
	virtual			~CPOpenGump()
	{
	}
	CPOpenGump( CChar &toCopy );
	virtual void	Length( SI32 TotalLines );
	virtual void	GumpIndex( SI32 index );
	virtual void	Serial( SERIAL toSet );
	virtual void	Question( std::string toAdd );
	virtual void	AddResponse( UI16 modelNum, UI16 colour, std::string responseText );
	virtual void	Finalize( void );
	CPOpenGump &operator = ( CChar &toCopy );
	virtual void	Log( std::ostream &outStream, bool fullHeader = true ) override;
};

class CPTargetCursor : public CPUOXBuffer
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

class CPStatWindow : public CPUOXBuffer
{
protected:
	bool			extended3;
	bool			extended4;
	bool			extended5;
	bool			extended6;
	size_t			byteOffset;
	virtual void	InternalReset( void ) override;
public:
	CPStatWindow();
	virtual			~CPStatWindow()
	{
	}
	CPStatWindow( CBaseObject &toCopy, CSocket &target );
	virtual void	Serial( SERIAL toSet );
	virtual void	Name( const std::string& nName );
	virtual void	CurrentHP( SI16 nValue );
	virtual void	MaxHP( SI16 nValue );
	virtual void	MaxWeight( UI16 value );
	virtual void	Race( UI08 value );
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
	virtual void	TithingPoints( UI32 value );
    // Extended-6 (UOKR+) Stat Methods
	virtual void	PhysicalResistCap( UI16 value );
	virtual void	FireResistCap( UI16 value );
	virtual void	ColdResistCap( UI16 value );
	virtual void	PoisonResistCap( UI16 value );
	virtual void	EnergyResistCap( UI16 value );
    virtual void	HitChanceIncrease( UI16 value );
    virtual void	SwingSpeedIncrease( UI16 value );
    virtual void	DamageChanceIncrease( UI16 value );
    virtual void	LowerReagentCost( UI16 value );
    virtual void    DefenseChanceIncrease( UI16 value );
    virtual void    DefenseChanceIncreaseCap( UI16 value );
    virtual void    SpellDamageIncrease( UI16 value );
    virtual void    FasterCastRecovery( UI16 value );
    virtual void    FasterCasting( UI16 value );
    virtual void    LowerManaCost( UI16 value );

};

class CPIdleWarning : public CPUOXBuffer
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
	virtual void	InternalReset( void ) override;
public:
	CPIdleWarning();
	virtual			~CPIdleWarning()
	{
	}
	CPIdleWarning( UI08 errorNum );
	virtual void	Error( UI08 errorNum );
};

class CPTime : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
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

class CPLoginComplete : public CPUOXBuffer
{
public:
	CPLoginComplete();
};

class CPTextEmoteColour : public CPUOXBuffer
{
public:
	virtual			~CPTextEmoteColour()
	{
	}
	CPTextEmoteColour();
	virtual void	BlockSize( SI16 newValue );
	virtual void	Unknown( SI16 newValue );
};

class CPWarMode : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
public:
	virtual			~CPWarMode()
	{
	}
	CPWarMode();
	CPWarMode( UI08 nFlag );
	virtual void	Flag( UI08 nFlag );
};

class CPPauseResume : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
public:
	virtual			~CPPauseResume()
	{
	}
	CPPauseResume();
	CPPauseResume( UI08 mode );
	virtual void	Mode( UI08 mode );
	virtual bool	ClientCanReceive( CSocket *mSock ) override;
};

class CPWebLaunch : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
	virtual void	SetSize( SI16 newSize );
public:
	virtual			~CPWebLaunch()
	{
	}
	CPWebLaunch();
	CPWebLaunch( const std::string& txt );
	virtual void	Text( const std::string& txt );
};

class CPTrackingArrow : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
public:
	virtual			~CPTrackingArrow()
	{
	}
	CPTrackingArrow();
	CPTrackingArrow( SI16 x, SI16 y );
	CPTrackingArrow( CBaseObject &toCopy );
	virtual void	Location( SI16 x, SI16 y );
	virtual void	Active( UI08 value );
	virtual void	AddSerial( SERIAL targetSerial );
	CPTrackingArrow &operator = ( CBaseObject &toCopy );
};

class CPBounce : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
public:
	virtual			~CPBounce()
	{
	}
	CPBounce();
	CPBounce( UI08 mode );
	virtual void	Mode( UI08 mode );
};

class CPDyeVat : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
	virtual void	CopyData( CBaseObject &target );
public:
	virtual			~CPDyeVat()
	{
	}
	CPDyeVat();
	CPDyeVat( CBaseObject &target );
	virtual void	Serial( SERIAL toSet );
	virtual void	Model( SI16 toSet );
	CPDyeVat &		operator = ( CBaseObject &target );
};

class CPMultiPlacementView : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
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
	virtual void	SetHue( UI16 hueValue );
	CPMultiPlacementView &operator = ( CItem &target );
};

class CPEnableClientFeatures : public CPUOXBuffer
{
public:
	CPEnableClientFeatures( CSocket *mSock );
	virtual void	Log( std::ostream &outStream, bool fullHeader = true ) override;
};

class CPNegotiateAssistantFeatures : public CPUOXBuffer
{
public:
	CPNegotiateAssistantFeatures( CSocket *mSock );
	virtual void	Log( std::ostream &outStream, bool fullHeader = true ) override;
};

class CPKrriosClientSpecial : public CPUOXBuffer
{
public:
	CPKrriosClientSpecial( CSocket *mSock, CChar * mChar, UI08 type, bool locations );
	virtual void	Log( std::ostream &outStream, bool fullHeader = true ) override;
};

class CPLogoutResponse : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
public:
	CPLogoutResponse();
	CPLogoutResponse( UI08 extraByte );
};

class CPAddItemToCont : public CPUOXBuffer
{
protected:
	bool			uokrFlag;
	virtual void	InternalReset( void ) override;
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
	virtual void	GridLocation( SI08 gridLoc );
	virtual void	Container( SERIAL toAdd );
	virtual void	Colour( SI16 toSet );
	void			UOKRFlag( bool newVal );
	void			Object( CItem &toAdd );
};

class CPKickPlayer : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
	virtual void	CopyData( CChar &toCopy );
public:
	virtual			~CPKickPlayer()
	{
	}
	CPKickPlayer();
	CPKickPlayer( CChar &toCopy );
	virtual void	Serial( SERIAL toSet );
	CPKickPlayer &	operator = ( CChar &toCopy );
};

class CPResurrectMenu : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
public:
	virtual			~CPResurrectMenu()
	{
	}
	CPResurrectMenu();
	CPResurrectMenu( UI08 action );
	virtual void	Action( UI08 action );
};

class CPFightOccurring : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
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

class CPSkillsValues : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
	virtual void	CopyData( CChar &toCopy );
	virtual void	BlockSize( SI16 newValue );
	virtual UI08	NumSkills( void );
public:
	virtual			~CPSkillsValues()
	{
	}
	CPSkillsValues();
	CPSkillsValues( CChar &toCopy );
	virtual void	NumSkills( UI08 numSkills );
	virtual void	SetCharacter( CChar &toCopy );
	virtual void	SkillEntry( SI16 skillId, SI16 skillVal, SI16 baseSkillVal, SkillLock skillLock );
	CPSkillsValues &operator = ( CChar &toCopy );
};

class CPMapMessage : public CPUOXBuffer
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
	virtual void	KeyUsed( UI32 key );
};

class CPMapRelated : public CPUOXBuffer
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

class CPBookTitlePage : public CPUOXBuffer
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
	virtual void	Title( const std::string& txt );
	virtual void	Author( const std::string& txt );
};

class CPUltimaMessenger : public CPUOXBuffer
{
public:
	virtual			~CPUltimaMessenger()
	{
	}
	CPUltimaMessenger();
	virtual void	ID1( SERIAL toSet );
	virtual void	ID2( SERIAL toSet );
};

class CPGumpTextEntry : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
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
	CPGumpTextEntry( const std::string& text );
	CPGumpTextEntry( const std::string& text1, const std::string& text2 );
	virtual void	Serial( SERIAL id );
	virtual void	ParentId( UI08 newVal );
	virtual void	ButtonId( UI08 newVal );
	virtual void	Cancel( UI08 newVal );	// 0 = disable, 1 = enable
	virtual void	Style( UI08 newVal );	// 0 = disable, 1 = normal, 2 = numerical
	virtual void	Format( SERIAL id );	// if style 1, max text len, if style 2, max num len
	virtual void	Text1( const std::string& txt );
	virtual void	Text2( const std::string& txt );
};

class CPMapChange : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
public:
	CPMapChange();
	CPMapChange( UI08 newMap );
	CPMapChange( CBaseObject *moving );
	virtual			~CPMapChange()
	{
	}
	virtual	void	SetMap( UI08 newMap );
	CPMapChange&	operator = ( CBaseObject& moving );
	virtual void	Log( std::ostream &outStream, bool fullHeader = true ) override;
};

class CPCloseGump : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
	UI32			_gumpId;
	UI32			_buttonId;
public:
	CPCloseGump( UI32 dialogId, UI32 buttonId );
	virtual			~CPCloseGump()
	{
	}
	virtual void	Log( std::ostream &outStream, bool fullHeader = true ) override;
};

class CPItemsInContainer : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
	virtual void	CopyData( CSocket *mSock, CItem& toCopy );
	bool			isVendor;
	bool			isPlayerVendor;
	bool			isCorpse;
	SERIAL			vendorSerial;
	bool			uokrFlag;
public:
	virtual			~CPItemsInContainer()
	{
	}
	CPItemsInContainer();
	CPItemsInContainer( CSocket *mSock, CItem *container, UI08 contType = 0x00, bool isPVendor = false );
	virtual void	NumberOfItems( UI16 numItems );
	virtual UI16	NumberOfItems( void ) const;
	void			Type( UI08 contType );
	void			UOKRFlag( bool value );
	void			PlayerVendor( bool value );
	void			VendorSerial( SERIAL toSet );
	virtual void	AddItem( CItem *toAdd, UI16 itemNum, CSocket *mSock );
	void			Add( UI16 itemNum, SERIAL toAdd, SERIAL cont, UI08 amount );
	virtual void	Log( std::ostream &outStream, bool fullHeader = true ) override;
};

class CPCorpseClothing : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
	virtual void	CopyData( CItem& toCopy );
public:
	virtual			~CPCorpseClothing()
	{
	}
	CPCorpseClothing();
	CPCorpseClothing( CItem *corpse );
	virtual void	NumberOfItems( UI16 numItems );
	virtual void	AddItem( CItem *toAdd, UI16 itemNum );
	CPCorpseClothing& operator = ( CItem& corpse );
};

class CPOpenBuyWindow : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
	virtual void	CopyData( CItem& toCopy, CChar *vendorId, CPItemsInContainer& iic, CSocket *mSock );
public:
	virtual			~CPOpenBuyWindow()
	{
	}
	CPOpenBuyWindow();
	CPOpenBuyWindow( CItem *container, CChar *vendorId, CPItemsInContainer& iic, CSocket *mSock );
	virtual void	NumberOfItems( UI08 numItems );
	virtual UI08	NumberOfItems( void ) const;
	virtual void	AddItem( CItem *toAdd, CTownRegion *tReg, UI16 &baseOffset );
	virtual void	Log( std::ostream &outStream, bool fullHeader = true ) override;
};

class CPCharAndStartLoc : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
	virtual void	CopyData( CAccountBlock_st& toCopy );
	UI16			packetSize;
	UI08			numCharacters;
public:
	virtual			~CPCharAndStartLoc()
	{
	}
	CPCharAndStartLoc();
	CPCharAndStartLoc( CAccountBlock_st& account, UI08 numCharacters, UI08 numLocations, CSocket *mSock );
	virtual void	NumberOfLocations( UI08 numLocations, CSocket *mSock );
	virtual void	AddCharacter( CChar *toAdd, UI08 charOffset );
	virtual auto 	AddStartLocation( __STARTLOCATIONDATA__ *sLoc, UI08 locOffset  ) -> void;
	virtual auto	NewAddStartLocation( __STARTLOCATIONDATA__ *sLoc, UI08 locOffset ) -> void;
	CPCharAndStartLoc& operator = ( CAccountBlock_st& actbBlock );
	virtual void	Log( std::ostream &outStream, bool fullHeader = true ) override;
};

class CPUpdScroll : public CPUOXBuffer
{
protected:
	char			tipData[2048];
	virtual void	InternalReset( void ) override;
	void			SetLength( UI16 len );
public:
	CPUpdScroll();
	CPUpdScroll( UI08 tType );
	CPUpdScroll( UI08 tType, UI08 tNum );
	virtual			~CPUpdScroll()
	{
	}
	void			AddString( const char *toAdd );
	void			Finalize( void );
	void			TipNumber( UI08 tipNum );
	void			TipType( UI08 tType );
};

class CPGraphicalEffect2 : public CPGraphicalEffect
{
protected:
	virtual void	InternalReset( void ) override;
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

class CP3DGraphicalEffect : public CPGraphicalEffect2
{
protected:
	virtual void	InternalReset( void ) override;
public:
	virtual			~CP3DGraphicalEffect()
	{
	}
	CP3DGraphicalEffect( UI08 effectType );
	CP3DGraphicalEffect( UI08 effectType, CBaseObject &src, CBaseObject &trg );
	CP3DGraphicalEffect( UI08 effectType, CBaseObject &src );
	virtual void	EffectId3D( UI16 effectId3D );
	virtual void	ExplodeEffectId( UI16 explodeEffectId );
	virtual void	MovingEffectId( UI16 movingEffectId );
	virtual void	TargetObjSerial( SERIAL targetObjSerial );
	virtual void	LayerId( UI08 layerId );
	virtual void	Unknown( UI16 unknown );
};

class CPDrawObject : public CPUOXBuffer
{
protected:
	UI16			curLen;
	virtual void	InternalReset( void ) override;
	void			SetLength( UI16 len );
	void			CopyData( CChar& mChar );
public:
	CPDrawObject();
	CPDrawObject( CChar &mChar );
	virtual			~CPDrawObject()
	{
	}
	void			Finalize( void );
	void			AddItem( CItem *toAdd, bool alwaysSendItemHue );
	void			SetRepFlag( UI08 value );
};

class CPObjectInfo : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
	void			CopyData( CItem& mItem, CChar& mChar );
	void			CopyItemData( CItem& mItem, CChar& mChar );
	void			CopyMultiData( CMultiObj& mObj, CChar& mChar );
public:
	CPObjectInfo();
	CPObjectInfo( CItem& mItem, CChar& mChar );
	virtual			~CPObjectInfo()
	{
	}
	void			Objects( CItem& mItem, CChar& mChar );
};

class CPNewObjectInfo : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
	void			CopyData( CItem& mItem, CChar& mChar );
	void			CopyItemData( CItem& mItem, CChar& mChar );
	void			CopyMultiData( CMultiObj& mObj, CChar& mChar );
public:
	CPNewObjectInfo();
	CPNewObjectInfo( CItem& mItem, CChar& mChar );
	virtual			~CPNewObjectInfo()
	{
	}
	void			Objects( CItem& mItem, CChar& mChar );
};

class CPGameServerList : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
public:
	virtual			~CPGameServerList()
	{
	}
	CPGameServerList();
	CPGameServerList( UI16 numServers );
	virtual void	NumberOfServers( UI16 numItems );
	virtual void	AddServer( UI16 servNum, PhysicalServer *data );
	virtual void 	addEntry( const std::string & name, UI32 addressBig );
};

class CPSecureTrading : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
	void			CopyData( CBaseObject& mItem, SERIAL mItem2, SERIAL mItem3 );
public:
	CPSecureTrading();
	CPSecureTrading( CBaseObject& mItem );
	CPSecureTrading( CBaseObject& mItem, SERIAL mItem2, SERIAL mItem3 );
	virtual			~CPSecureTrading()
	{
	}
	void			Action( UI08 value );
	void			Name( const std::string& nameFollowing );
};

class CPGodModeToggle : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
	virtual void	CopyData( CSocket *s );
public:
	virtual			~CPGodModeToggle()
	{
	}
	CPGodModeToggle();
	CPGodModeToggle( CSocket *s );
	CPGodModeToggle& operator = ( CSocket *s );
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

class CPLoginDeny : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
public:
	virtual			~CPLoginDeny()
	{
	}
	CPLoginDeny();
	CPLoginDeny( LoginDenyReason reason );
	virtual void	DenyReason( LoginDenyReason reason );
};

class CPCharDeleteResult : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
public:
	virtual			~CPCharDeleteResult()
	{
	}
	CPCharDeleteResult();
	CPCharDeleteResult( SI08 result );
	virtual void	DeleteResult( SI08 result );
};

class CharacterListUpdate : public CPUOXBuffer
{
protected:
	UI08			numChars;
	virtual void	InternalReset( void ) override;
public:
	virtual			~CharacterListUpdate()
	{
	}
	CharacterListUpdate();
	CharacterListUpdate( UI08 charCount );
	virtual void	AddCharName( UI08 charCount, std::string charName );
};

class CPKREncryptionRequest : public CPUOXBuffer
{
public:
	CPKREncryptionRequest( CSocket *mSock );
};

class CPClientVersion : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
public:
	virtual			~CPClientVersion()
	{
	}
	CPClientVersion();
};

// This packet is specific for Krrios' client
class CPKAccept : public CPUOXBuffer
{
public:
	CPKAccept( UI08 Response );
};

class CPUnicodeSpeech : public CPUOXBuffer
{
protected:
	void			CopyData( CBaseObject &toCopy );
	void			CopyData( CPITalkRequestAscii &talking );
	void			CopyData( CPITalkRequestUnicode &talking );
	void			InternalReset( void ) override;
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
	void			Serial( SERIAL toSet );
	void			Object( CPITalkRequestAscii &tSaid );
	void			Object( CPITalkRequestUnicode &tSaid );
	void			Object( CBaseObject &toCopy );
	void			Language( char *value );
	void            Language( const char *value );
	void            Lanaguge( const std::string& value );
	void			Type( UI08 value );
	void			Colour( COLOUR value );
	void			Font( UI16 value );
	void			Name( std::string value );
	void			Message( const char *value );
	void			GhostIt( UI08 method );
	CPUnicodeSpeech &operator = ( CBaseObject &toCopy );
	CPUnicodeSpeech &operator = ( CPITalkRequestAscii &talking );
	CPUnicodeSpeech &operator = ( CPITalkRequestUnicode &talking );
};

class CPUnicodeMessage : public CPUOXBuffer
{
protected:
	void			CopyData( CBaseObject &toCopy );
	void			InternalReset( void ) override;
	void			SetLength( UI16 value );
public:
	CPUnicodeMessage();
	virtual			~CPUnicodeMessage()
	{
	}
	CPUnicodeMessage( CBaseObject &toCopy );
	void			ID( UI16 toSet );
	void			Serial( SERIAL toSet );
	void			Object( CBaseObject &toCopy );
	void			Language( char *value );
	void            Language( const char *value );
	void            Lanaguge( const std::string& value );
	void			Type( UI08 value );
	void			Colour( COLOUR value );
	void			Font( UI16 value );
	void			Name( std::string value );
	void			Message( const char *value );
	void			Message( const std::string value );
	CPUnicodeMessage &operator = ( CBaseObject &toCopy );
};

class CPAllNames3D : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
	virtual void	CopyData( CBaseObject& obj );
public:
	virtual			~CPAllNames3D()
	{
	}
	CPAllNames3D();
	CPAllNames3D( CBaseObject& obj );
	void			Object( CBaseObject& obj );
};

class CPBookPage : public CPUOXBuffer
{
protected:
	UI16			bookLength;
	UI08			pageCount;
	virtual void	InternalReset( void ) override;
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
	void			NewPage( SI16 pNum, const std::vector<std::string> *lines );
	void			AddLine( const std::string& line );
	void			Finalize( void );
};

class CPSendGumpMenu : public CPUOXBuffer
{
protected:
	std::vector<std::string>		commands, text;
public:
	virtual			~CPSendGumpMenu()
	{
	}
	CPSendGumpMenu();
	void			UserId( SERIAL value );
	void			GumpId( SERIAL value );
	void			X( UI32 value );
	void			Y( UI32 value );

	void			addCommand( const std::string& msg );
	void			addText( const std::string& msg );

	void			Finalize( void );
	virtual void	Log( std::ostream &outStream, bool fullHeader ) override;
};

class CPNewSpellBook : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( CItem &obj );
	virtual void	CopyData( CItem& obj );
public:
	virtual			~CPNewSpellBook()
	{
	}
	CPNewSpellBook();
	CPNewSpellBook( CItem& obj );
	virtual bool	ClientCanReceive( CSocket *mSock ) override;
};

class CPDisplayDamage : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
	virtual void	CopyData( CChar& ourTarg, UI16 ourDamage );
public:
	virtual			~CPDisplayDamage()
	{
	}
	CPDisplayDamage();
	CPDisplayDamage( CChar& ourTarg, UI16 ourDamage );
	virtual bool	ClientCanReceive( CSocket *mSock ) override;
};

class CPQueryToolTip : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
	virtual void	CopyData( CBaseObject& mObj );
public:
	virtual			~CPQueryToolTip()
	{
	}
	CPQueryToolTip();
	CPQueryToolTip( CBaseObject& mObj );
	virtual bool	ClientCanReceive( CSocket *mSock ) override;
};

class CPToolTip : public CPUOXBuffer
{
protected:
	struct ToolTipEntry_st
	{
		std::string ourText;
		UI32 stringNum;
		size_t stringLen;
	};
	std::vector<ToolTipEntry_st> ourEntries;

	struct ToolTipEntryWide_st
	{
		std::wstring ourText;
		UI32 stringNum;
		size_t stringLen;
	};
	std::vector<ToolTipEntryWide_st> ourWideEntries;
	CSocket *tSock;
	virtual void	InternalReset( void ) override;
	virtual void	CopyData( SERIAL objSer, bool addAmount = true, bool playerVendor = false );
	void				CopyItemData( CItem& cItem, size_t &totalStringLen, bool addAmount = true, bool playerVendor = false );
	void				CopyCharData( CChar& mChar, size_t &totalStringLen );
	void				FinalizeData( ToolTipEntry_st tempEntry, size_t &totalStringLen );
	void				FinalizeWideData( ToolTipEntryWide_st tempEntry, size_t &totalStringLen );
public:
	virtual			~CPToolTip()
	{
	}
	CPToolTip();
	CPToolTip( SERIAL objSer, CSocket *mSock = nullptr, bool addAmount = true, bool playerVendor = false );
};

class CPSellList : public CPUOXBuffer
{
protected:
	UI16			numItems;

	virtual void	InternalReset( void ) override;
	virtual void	CopyData( CChar& mChar, CChar& vendor );
public:
	virtual			~CPSellList()
	{
	}
	CPSellList();
	void			AddContainer( CTownRegion *tReg, CItem *spItem, CItem *ourPack, size_t &packetLen );
	void			AddItem( CTownRegion *tReg, CItem *spItem, CItem *opItem, size_t &packetLen );
	bool			CanSellItems( CChar &mChar, CChar &vendor );
};

class CPOpenMessageBoard : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
	virtual void	CopyData( CSocket *mSock );
public:
	virtual			~CPOpenMessageBoard()
	{
	}
	CPOpenMessageBoard( CSocket *mSock );
};

struct MsgBoardPost_st;

class CPOpenMsgBoardPost : public CPUOXBuffer
{
protected:
	bool			bFullPost;

	virtual void	InternalReset( void ) override;
	virtual void	CopyData( CSocket *mSock, const MsgBoardPost_st &mbPost );
public:
	virtual			~CPOpenMsgBoardPost()
	{
	}
	CPOpenMsgBoardPost( CSocket *mSock, const MsgBoardPost_st& mbPost, bool fullPost );
};

class CPSendMsgBoardPosts : public CPUOXBuffer
{
protected:
	UI16			postCount;

	virtual void	InternalReset( void ) override;
public:
	virtual			~CPSendMsgBoardPosts()
	{
	}
	virtual void	CopyData( CSocket *msock, SERIAL mSerial, UI08 pToggle, SERIAL oSerial );
	void			Finalize( void );
	CPSendMsgBoardPosts();
};

class CPHealthBarStatus : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
public:
	CPHealthBarStatus();
	CPHealthBarStatus( CChar& mChar, CSocket &target, UI08 healthBarColor );
	virtual			~CPHealthBarStatus()
	{
	}

	virtual void	CopyData( CChar& mChar );
	virtual void	SetHBStatusData( CChar& mChar, CSocket &target, UI08 healthBarColor );
};

class CPExtendedStats : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
public:
	CPExtendedStats();
	CPExtendedStats( CChar& mChar );
	virtual			~CPExtendedStats()
	{
	}

	virtual void	CopyData( CChar& mChar );
};

class CPEnableMapDiffs : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
public:
	CPEnableMapDiffs();
	virtual			~CPEnableMapDiffs()
	{
	}
	virtual void	CopyData( void );
};

class CPNewBookHeader : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
	std::string		title;
	std::string		author;
public:
	CPNewBookHeader();
	virtual			~CPNewBookHeader()
	{
	}
	void			Serial( UI32 bookSer );
	void			Flag1( UI08 toSet );
	void			Flag2( UI08 toSet );
	void			Pages( UI16 numPages );
	void			Author( const std::string& author );
	void			Title( const std::string& title );
	void			Finalize( void );
};

class CPPopupMenu : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
public:
	CPPopupMenu();
	CPPopupMenu( CBaseObject&, CSocket& );
	virtual			~CPPopupMenu()
	{
	}
	virtual void	CopyData( CBaseObject&, CSocket& );
};

class CPClilocMessage : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
public:
	CPClilocMessage();
	CPClilocMessage( CBaseObject& toCopy );
	virtual			~CPClilocMessage()
	{
	}
	virtual void	CopyData( CBaseObject& toCopy );

	void			Serial( SERIAL sourceSer );
	void			Body( UI16 toSet );
	void			Type( UI08 toSet );
	void			Hue( UI16 hueColor );
	void			Font( UI16 fontType );
	void			Message( UI32 messageNum );
	void			Name( const std::string& name );
	void			ArgumentString( const std::string& arguments );
};

class CPPartyMemberList : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
public:
	CPPartyMemberList();
	virtual			~CPPartyMemberList()
	{
	}
	void			AddMember( CChar *member );
	virtual void	Log( std::ostream &outStream, bool fullHeader = true ) override;
};

class CPPartyInvitation : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
public:
	CPPartyInvitation();
	virtual			~CPPartyInvitation()
	{
	}
	virtual void	Log( std::ostream &outStream, bool fullHeader = true ) override;
	void			Leader( CChar *leader );
};

class CPPartyMemberRemove : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
public:
	CPPartyMemberRemove( CChar *removed );
	virtual			~CPPartyMemberRemove()
	{
	}
	void			AddMember( CChar *member );
	virtual void	Log( std::ostream &outStream, bool fullHeader = true ) override;
};

class CPPartyTell : public CPUOXBuffer
{
protected:
	virtual void	InternalReset( void ) override;
public:
	CPPartyTell( CPIPartyCommand *told, CSocket *talker );
	virtual			~CPPartyTell()
	{
	}
	virtual void	Log( std::ostream &outStream, bool fullHeader = true ) override;
};

class CPDropItemApproved : public CPUOXBuffer
{
public:
	CPDropItemApproved();
};

#endif

