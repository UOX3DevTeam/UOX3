#ifndef __PACKETS_H__
#define __PACKETS_H__

class CSpeechEntry;		// It's around somewhere

class cPCharLocBody : public cPBuffer
{
protected:
	virtual void	CopyData( CChar &toCopy );
	virtual void	InternalReset( void );
public:
					cPCharLocBody();
					cPCharLocBody( CChar &toCopy );
	virtual			~cPCharLocBody();
	virtual void	PlayerID( SERIAL toPut );
	virtual void	BodyType( UI16 toPut );
	virtual void	X( UI16 toPut );
	virtual void	Y( UI16 toPut );
	virtual void	Z( UI16 toPut );
	virtual void	Direction( UI08 toPut );
	virtual void	Flag( UI08 toPut );
	virtual void	HighlightColour( UI08 color );
	cPCharLocBody &	operator=( CChar &toCopy );
};

class CPacketSpeech : public cPBuffer
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
	virtual void	Speech( const char *toPut );
	virtual void	SpeakerName( const char *toPut );
	virtual			~CPacketSpeech();
	CPacketSpeech	&operator=( CSpeechEntry &toCopy );
	CPacketSpeech	&operator=( CPacketSpeech &toCopy );
};

class CPWalkDeny : public cPBuffer
{
public:
					CPWalkDeny();
	virtual void	SequenceNumber( char newValue );
	virtual void	X( SI16 newValue );
	virtual void	Y( SI16 newValue );
	virtual void	Z( SI08 newValue );
	virtual void	Direction( char newValue );
};

class CPWalkOK : public cPBuffer
{
public:
					CPWalkOK();
	virtual void	SequenceNumber( char newValue );
	virtual void	OtherByte( char newValue );
};

class CPExtMove : public cPBuffer
{
protected:
	void			CopyData( CChar &toCopy );
public:
					CPExtMove();
					CPExtMove( CChar &toCopy );
	virtual void	Serial( SERIAL newSerial );
	virtual void	ID( UI16 bodyID );
	virtual void	X( SI16 newValue );
	virtual void	Y( SI16 newValue );
	virtual void	Z( SI08 newValue );
	virtual void	Direction( char newValue );
	virtual void	Colour( UI16 colourID );
	virtual void	FlagColour( char newValue );
	virtual void	Flag( char newValue );
	CPExtMove		&operator=( CChar &toCopy );
};

class CPAttackOK : public cPBuffer
{
protected:
	void			CopyData( CChar &toCopy );
public:
					CPAttackOK();
					CPAttackOK( CChar &toCopy );
	virtual void	Serial( SERIAL newSerial );
	CPAttackOK		&operator=( CChar &toCopy );
};

class CPRemoveItem : public cPBuffer
{
protected:
	void			CopyData( cBaseObject &toCopy );
public:
					CPRemoveItem();
					CPRemoveItem( cBaseObject &toCopy );
	virtual void	Serial( SERIAL newSerial );
	CPRemoveItem	&operator=( cBaseObject &toCopy );
};

class CPWorldChange : public cPBuffer
{
public:
					CPWorldChange();
					CPWorldChange( WorldType newSeason, UI08 newCursor );
	virtual void	Season( WorldType newSeason );
	virtual void	Cursor( UI08 newCursor );
};

class CPLightLevel : public cPBuffer
{
public:
					CPLightLevel();
					CPLightLevel( UI08 level );
	virtual void	Level( UI08 level );
};

class CPUpdIndSkill : public cPBuffer
{
protected:
	virtual void	CopyData( CChar& i, UI08 sNum );
	virtual void	InternalReset( void );
public:
					CPUpdIndSkill();
					CPUpdIndSkill( CChar& i, UI08 sNum );
	virtual void	Character( CChar& i, UI08 sNum );
	virtual void	SkillNum( UI08 sNum );
	virtual void	Skill( SI16 skillval );
	virtual void	BaseSkill( SI16 skillval );
	virtual void	Lock( UI08 lockVal );
};

class CPClearMsg : public cPBuffer
{
protected:
	virtual void	InternalReset( void );
	virtual void	CopyData( cBaseObject &i );
public:
					CPClearMsg();
					CPClearMsg( cBaseObject &i );
	CPClearMsg		&operator=( cBaseObject &toCopy );
	void			Serial( SERIAL toSet );
};

class CPRelay : public cPBuffer
{
protected:
	virtual void	InternalReset( void );
public:
					CPRelay();
					CPRelay( long newIP );
					CPRelay( long newIP, SI16 newPort );
	virtual void	ServerIP( long newIP );
	virtual void	Port( SI16 newPort );
	virtual void	SeedIP( long newIP );
};

class CPWornItem : public cPBuffer
{
protected:
	virtual void	CopyData( CItem &toCopy );
public:
					CPWornItem();
					CPWornItem( CItem &toCopy );
	virtual void	ItemSerial( SERIAL itemSer );
	virtual void	Model( SI16 newModel );
	virtual void	Layer( UI08 layer );
	virtual void	CharSerial( SERIAL chSer );
	virtual void	Colour( SI16 newColour );
	CPWornItem		&operator=( CItem &toCopy );
};

class CPCharacterAnimation : public cPBuffer
{
protected:
	virtual void	CopyData( CChar &toCopy );
	virtual void	InternalReset( void );
public:
					CPCharacterAnimation();
					CPCharacterAnimation( CChar &toCopy );
	virtual void	Serial( SERIAL toSet );
	virtual void	Action( SI16 model );
	virtual void	Direction( UI08 dir );
	virtual void	Repeat( SI16 repeatValue );
	virtual void	DoBackwards( bool newValue );
	virtual void	RepeatFlag( bool newValue );
	virtual void	FrameDelay( UI08 delay );
	CPCharacterAnimation &operator=( CChar &toCopy );
};

class CPDrawGamePlayer : public cPBuffer
{
protected:
	virtual void	CopyData( CChar &toCopy );
	virtual void	InternalReset( void );
public:
					CPDrawGamePlayer();
					CPDrawGamePlayer( CChar &toCopy );
	virtual void	Serial( SERIAL toSet );
	virtual void	Model( SI16 toSet );
	virtual void	Colour( SI16 toSet );
	virtual void	Flag( UI08 toSet );
	virtual void	X( SI16 toSet );
	virtual void	Y( SI16 toSet );
	virtual void	Direction( UI08 toSet );
	virtual void	Z( SI08 toSet );
	CPDrawGamePlayer &operator=( CChar &toCopy );
};

class CPPersonalLightLevel : public cPBuffer
{
protected:
	virtual void	CopyData( CChar &toCopy );
	virtual void	InternalReset( void );
public:
					CPPersonalLightLevel();
					CPPersonalLightLevel( CChar &toCopy );
	virtual void	Serial( SERIAL toSet );
	virtual void	Level( UI08 lightLevel );
	CPPersonalLightLevel &operator=( CChar &toCopy );
};

class CPPlaySoundEffect : public cPBuffer
{
protected:
	virtual void	CopyData( cBaseObject &toCopy );
	virtual void	InternalReset( void );
public:
					CPPlaySoundEffect();
					CPPlaySoundEffect( cBaseObject &toCopy );
	virtual void	Mode( UI08 mode );
	virtual void	Model( SI16 newModel );
	virtual void	X( SI16 xLoc );
	virtual void	Y( SI16 yLoc );
	virtual void	Z( SI16 zLoc );
	CPPlaySoundEffect &operator=( cBaseObject &toCopy );
};

class CPPaperdoll : public cPBuffer
{
protected:
	virtual void	CopyData( CChar &toCopy );
	virtual void	InternalReset( void );
public:
					CPPaperdoll();
					CPPaperdoll( CChar &toCopy );
	virtual void	Serial( SERIAL tSerial );
	virtual void	FlagByte( UI08 fVal );
	virtual void	Text( const char *toPut );
	CPPaperdoll &	operator=( CChar &toCopy );
};

class CPWeather : public cPBuffer
{
protected:
	virtual void	InternalReset( void );
public:
					CPWeather();
					CPWeather( UI08 nType );
					CPWeather( UI08 nType, UI08 nParts );
					CPWeather( UI08 nType, UI08 nParts, UI08 nTemp );
	virtual void	Type( UI08 nType );
	virtual void	Particles( UI08 nParts );
	virtual void	Temperature( UI08 nTemp );
};

class CPGraphicalEffect : public cPBuffer
{
protected:
	virtual void	InternalReset( void );
public:
					CPGraphicalEffect( UI08 effectType );
					CPGraphicalEffect( UI08 effectType, cBaseObject &src, cBaseObject &trg );
					CPGraphicalEffect( UI08 effectType, cBaseObject &src );
	virtual void	Effect( UI08 effectType );
	virtual void	SourceSerial( cBaseObject &toSet );
	virtual void	SourceSerial( SERIAL toSet );
	virtual void	TargetSerial( cBaseObject &toSet );
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
	virtual void	SourceLocation( cBaseObject &toSet );
	virtual void	SourceLocation( SI16 x, SI16 y, SI08 z );
	virtual void	TargetLocation( cBaseObject &toSet );
	virtual void	TargetLocation( SI16 x, SI16 y, SI08 z );
};

class CPUpdateStat : public cPBuffer
{
protected:
	virtual void	InternalReset( void );
public:
					CPUpdateStat( CChar &toUpdate, UI08 statNum );
	virtual void	Serial( SERIAL toSet );
	virtual void	MaxVal( SI16 maxVal );
	virtual void	CurVal( SI16 curVal );
};

class CPDeathAction : public cPBuffer
{
protected:
	virtual void	InternalReset( void );
public:
					CPDeathAction( CChar &dying, CItem &corpse );
					CPDeathAction();
	virtual void	Player( SERIAL toSet );
	virtual void	Corpse( SERIAL toSet );
	virtual void	FallDirection( UI08 toFall );
	CPDeathAction &	operator=( CChar &dying );
	CPDeathAction & operator=( CItem &corpse );
};

class CPPlayMusic : public cPBuffer
{
protected:
	virtual void	InternalReset( void );
public:
					CPPlayMusic( SI16 musicID );
					CPPlayMusic();
	virtual void	MusicID( SI16 musicID );
};

class CPDrawContainer : public cPBuffer
{
protected:
	virtual void	CopyData( CItem &toCopy );
	virtual void	InternalReset( void );
public:
					CPDrawContainer();
					CPDrawContainer( CItem &toCopy );
	virtual void	Model( SI16 newModel );
	virtual void	Serial( SERIAL toSet );
	CPDrawContainer &operator=( CItem &toCopy );
};

class CPTargetCursor : public cPBuffer
{
public:
					CPTargetCursor();
	virtual void	Type( UI08 nType );
	virtual void	ID( SERIAL toSet );
	virtual void	CursorType( UI08 nType );	// There is more to this packet, but it's only valid when sent by the CLIENT
};

class CPStatWindow : public cPBuffer
{
protected:
	virtual void	CopyData( CChar &toCopy );
	virtual void	InternalReset( void );
public:
					CPStatWindow();
					CPStatWindow( CChar &toCopy );
	virtual void	Serial( SERIAL toSet );
	virtual void	Name( const char *nName );
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
	virtual void	Gold( long gValue );
	virtual void	AC( SI16 nValue );
	virtual void	Weight( SI16 nValue );
	CPStatWindow &	operator=( CChar &toCopy );
};

class CPIdleWarning : public cPBuffer
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
					CPIdleWarning( UI08 errorNum );
	virtual void	Error( UI08 errorNum );
};

class CPTime : public cPBuffer
{
protected:
	virtual void	InternalReset( void );
public:
					CPTime();
					CPTime( UI08 hour, UI08 minute, UI08 second );
	virtual void	Hour( UI08 hour );
	virtual void	Minute( UI08 minute );
	virtual void	Second( UI08 second );
};

class CPLoginComplete : public cPBuffer
{
public:
					CPLoginComplete();
};

class CPTextEmoteColour : public cPBuffer
{
public:
					CPTextEmoteColour();
	virtual void	BlockSize( SI16 newValue );
	virtual void	Unknown( SI16 newValue );
};

class CPWarMode : public cPBuffer
{
protected:
	virtual void	InternalReset( void );
public:
					CPWarMode();
					CPWarMode( UI08 nFlag );
	virtual void	Flag( UI08 nFlag );
};

class CPPauseResume : public cPBuffer
{
protected:
	virtual void	InternalReset( void );
public:
					CPPauseResume();
					CPPauseResume( UI08 mode );
	virtual void	Mode( UI08 mode );
};

class CPWebLaunch : public cPBuffer
{
protected:
	virtual void	InternalReset( void );
	virtual void	SetSize( SI16 newSize );
public:
					CPWebLaunch();
					CPWebLaunch( const char *txt );
	virtual void	Text( const char *txt );
};

class CPTrackingArrow : public cPBuffer
{
protected:
	virtual void	InternalReset( void );
public:
					CPTrackingArrow();
					CPTrackingArrow( SI16 x, SI16 y );
					CPTrackingArrow( cBaseObject &toCopy );
	virtual void	Location( SI16 x, SI16 y );
	virtual void	Active( UI08 value );
	CPTrackingArrow &operator=( cBaseObject &toCopy );
};

class CPBounce : public cPBuffer
{
protected:
	virtual void	InternalReset( void );
public:
					CPBounce();
					CPBounce( UI08 mode );
	virtual void	Mode( UI08 mode );
};

class CPDyeVat : public cPBuffer
{
protected:
	virtual void	InternalReset( void );
	virtual void	CopyData( cBaseObject &target );
public:
					CPDyeVat();
					CPDyeVat( cBaseObject &target );
	virtual void	Serial( SERIAL toSet );
	virtual void	Model( SI16 toSet );
	CPDyeVat &		operator=( cBaseObject &target );
};

class CPMultiPlacementView : public cPBuffer
{
protected:
	virtual void	InternalReset( void );
	virtual void	CopyData( CItem &target );
public:
					CPMultiPlacementView();
					CPMultiPlacementView( CItem &target );
					CPMultiPlacementView( SERIAL toSet );
	virtual void	RequestType( UI08 rType );
	virtual void	DeedSerial( SERIAL toSet );
	virtual void	MultiModel( SI16 toSet );
	CPMultiPlacementView &operator=( CItem &target );
};

class CPEnableChat : public cPBuffer
{
public:
					CPEnableChat();
};

class CPAddItemToCont : public cPBuffer
{
protected:
	virtual void	InternalReset( void );
	virtual void	CopyData( CItem &toCopy );
public:
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

class CPKickPlayer : public cPBuffer
{
protected:
	virtual void	InternalReset( void );
	virtual void	CopyData( CChar &toCopy );
public:
					CPKickPlayer();
					CPKickPlayer( CChar &toCopy );
	virtual void	Serial( SERIAL toSet );
	CPKickPlayer &	operator=( CChar &toCopy );
};

class CPResurrectMenu : public cPBuffer
{
protected:
	virtual void	InternalReset( void );
public:
					CPResurrectMenu();
					CPResurrectMenu( UI08 action );
	virtual void	Action( UI08 action );
};

class CPFightOccurring : public cPBuffer
{
protected:
	virtual void	InternalReset( void );
public:
					CPFightOccurring();
					CPFightOccurring( CChar &attacker, CChar &defender );
	virtual void	Attacker( SERIAL toSet );
	virtual void	Attacker( CChar &attacker );
	virtual void	Defender( SERIAL toSet );
	virtual void	Defender( CChar &defender );
};

class CPSkillsValues : public cPBuffer
{
protected:
	virtual void	InternalReset( void );
	virtual void	CopyData( CChar &toCopy );
	virtual void	BlockSize( SI16 newValue );
	virtual void	WriteShort( SI16 offset, SI16 value );
public:
					CPSkillsValues();
					CPSkillsValues( CChar &toCopy );
	virtual void	NumSkills( UI08 numSkills );
	virtual void	SkillEntry( SI16 skillID, SI16 skillVal, SI16 baseSkillVal, UI08 skillLock );
	CPSkillsValues &operator=( CChar &toCopy );
};

class CPMapMessage : public cPBuffer
{
public:
					CPMapMessage();
	virtual void	UpperLeft( SI16 x, SI16 y );
	virtual void	LowerRight( SI16 x, SI16 y );
	virtual void	Dimensions( SI16 width, SI16 height );
	virtual void	GumpArt( SI16 newArt );
	virtual void	KeyUsed( long key );
};

class CPBookTitlePage : public cPBuffer
{
public:
					CPBookTitlePage();
	virtual void	Serial( SERIAL toSet );
	virtual void	WriteFlag( UI08 flag );
	virtual void	NewFlag( UI08 flag );
	virtual void	Pages( SI16 pages );
	virtual void	Title( const char *txt );
	virtual void	Author( const char *txt );
};

class CPUltimaMessenger : public cPBuffer
{
public:
					CPUltimaMessenger();
	virtual void	ID1( SERIAL toSet );
	virtual void	ID2( SERIAL toSet );
};

class CPGumpTextEntry : public cPBuffer
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
					CPGumpTextEntry();
					CPGumpTextEntry( const char *text );
					CPGumpTextEntry( const char *text1, const char *text2 );
	virtual void	Serial( SERIAL id );
	virtual void	ParentID( UI08 newVal );
	virtual void	ButtonID( UI08 newVal );
	virtual void	Cancel( UI08 newVal );	// 0 = disable, 1 = enable
	virtual void	Style( UI08 newVal );	// 0 = disable, 1 = normal, 2 = numerical
	virtual void	Format( SERIAL id );	// if style 1, max text len, if style 2, max num len
	virtual void	Text1( const char *txt );
	virtual void	Text2( const char *txt );
};

class CMapChange : public cPBuffer
{
protected:
	virtual void	InternalReset( void );
public:
					CMapChange();
					CMapChange( UI08 newMap );
					CMapChange( cBaseObject *moving );
	virtual			~CMapChange();
	virtual	void	SetMap( UI08 newMap );
	CMapChange&		operator=( cBaseObject& moving );
};

class CPItemsInContainer : public cPBuffer
{
protected:
	virtual void	InternalReset( void );
	virtual void	CopyData( CItem& toCopy );
public:
					CPItemsInContainer();
					CPItemsInContainer( CItem *container );
	virtual void	NumberOfItems( UI16 numItems );
	virtual UI16	NumberOfItems( void ) const;
	virtual void	AddItem( CItem *toAdd, UI16 itemNum );
	CPItemsInContainer& operator=( CItem& container );
};

class CPOpenBuyWindow : public cPBuffer
{
protected:
	virtual void	InternalReset( void );
	virtual void	CopyData( CItem& toCopy, CChar *p );
public:
					CPOpenBuyWindow();
					CPOpenBuyWindow( CItem *container, CChar *p );
	virtual void	NumberOfItems( UI08 numItems );
	virtual UI08	NumberOfItems( void ) const;
	virtual SI16	AddItem( CItem *toAdd, CChar *p, UI16 baseOffset );
	virtual void	Container( CItem *toAdd, CChar *p );
	virtual void	VendorID( CItem *p );
};

class CPCharAndStartLoc : public cPBuffer
{
protected:
	virtual void	InternalReset( void );
	virtual void	CopyData( ACTREC& toCopy );
public:
					CPCharAndStartLoc();
					CPCharAndStartLoc( ACTREC *account, UI08 numCharacters, UI08 numLocations );
	virtual void	NumberOfLocations( UI08 numLocations );
	virtual void	NumberOfCharacters( UI08 numCharacters );
	virtual void	AddCharacter( CChar *toAdd, UI08 charOffset );
	virtual void	AddStartLocation( LPSTARTLOCATION sLoc, UI08 locOffset );
	CPCharAndStartLoc& operator=( ACTREC& account );
};

class CPIFirstLogin : public cPInputBuffer
{
protected:
	virtual void	InternalReset( void );
public:
					CPIFirstLogin();
					CPIFirstLogin( cSocket *s );
	virtual void	Receive( cSocket *s );
	CPIFirstLogin &operator=( cSocket &s );
	virtual const char *Name( void );
	virtual const char *Pass( void );
	virtual UI08	Unknown( void );
	virtual bool	Handle( void );
};

class CPIServerSelect : public cPInputBuffer
{
protected:
	virtual void	InternalReset( void );
public:
					CPIServerSelect();
					CPIServerSelect( cSocket *s );
	virtual void	Receive( cSocket *s );
	CPIServerSelect &operator=( cSocket &s );
	virtual SI16	ServerNum( void );
	virtual bool	Handle( void );
};

class CPISecondLogin : public cPInputBuffer
{
protected:
	virtual void	InternalReset( void );
public:
					CPISecondLogin();
					CPISecondLogin( cSocket *s );
	virtual void	Receive( cSocket *s );
	CPISecondLogin &operator=( cSocket &s );
	virtual long	Account( void );
	virtual const char *Name( void );
	virtual const char *Pass( void );
	virtual bool	Handle( void );
};

class CPGodModeToggle : public cPBuffer
{
protected:
	virtual void	InternalReset( void );
	virtual void	CopyData( cSocket *s );
public:
					CPGodModeToggle();
					CPGodModeToggle( cSocket *s );
	CPGodModeToggle& operator=( cSocket *s );
	virtual void	ToggleStatus( bool toSet );
};

class CPLoginDeny : public cPBuffer
{
protected:
	virtual void	InternalReset( void );
public:
					CPLoginDeny();
					CPLoginDeny( LoginDenyReason reason );
	virtual void	DenyReason( LoginDenyReason reason );
};

class CPIClientVersion : public cPInputBuffer
{
protected:
	virtual void	InternalReset( void );
	SI16 len;
public:
					CPIClientVersion();
					CPIClientVersion( cSocket *s );
	virtual void	Receive( cSocket *s );
	CPIClientVersion &operator=( cSocket *s );
	virtual bool	Handle( void );
};

class CPIUpdateRangeChange : public cPInputBuffer
{
protected:
	virtual void	InternalReset( void );
public:
					CPIUpdateRangeChange();
					CPIUpdateRangeChange( cSocket *s );
	virtual void	Receive( cSocket *s );
	CPIUpdateRangeChange &operator=( cSocket *s );
	virtual bool	Handle( void );
};

// This packet is specific for Krrios' client
class CPKAccept : public cPBuffer
{
public:
					CPKAccept( UI08 Response );
};

#endif

