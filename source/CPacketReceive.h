#ifndef __CPACKETRECEIVE_H__
#define __CPACKETRECEIVE_H__

#include "network.h"

class CPIFirstLogin : public CPInputBuffer
{
protected:
	virtual void		InternalReset( void );
	std::string			userID;
	std::string			password;
	UI08				unknown;
public:
	virtual				~CPIFirstLogin()
	{
	}
	CPIFirstLogin();
	CPIFirstLogin( CSocket *s );
	virtual void		Receive( void ) override;
	virtual const std::string Name( void );
	virtual const std::string Pass( void );
	virtual UI08		Unknown( void );
	virtual bool		Handle( void ) override;
	virtual void		Log( std::ofstream &outStream, bool fullHeader = true ) override;
};

class CPIServerSelect : public CPInputBuffer
{
protected:
	virtual void	InternalReset( void );
public:
	virtual			~CPIServerSelect()
	{
	}
	CPIServerSelect();
	CPIServerSelect( CSocket *s );
	virtual void	Receive( void ) override;
	virtual SI16	ServerNum( void );
	virtual bool	Handle( void ) override;
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true ) override;
};

class CPISecondLogin : public CPInputBuffer
{
protected:
	virtual void		InternalReset( void );
	UI32				keyUsed;
	std::string			sid;
	std::string			password;
public:
	virtual				~CPISecondLogin()
	{
	}
	CPISecondLogin();
	CPISecondLogin( CSocket *s );
	virtual void		Receive( void ) override;
	virtual UI32		Account( void );
	virtual const std::string Name( void );
	virtual const std::string Pass( void );
	virtual bool		Handle( void ) override;
	virtual void		Log( std::ofstream &outStream, bool fullHeader = true ) override;
};

class CPIClientVersion : public CPInputBuffer
{
protected:
	virtual void	InternalReset( void );
	char *			Offset( void );
	SI16			len;
public:
	virtual			~CPIClientVersion()
	{
	}
	CPIClientVersion();
	CPIClientVersion( CSocket *s );
	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true ) override;
	virtual void	SetClientVersionShortAndType( CSocket *s, char *verString );
};

class CPINewClientVersion : public CPInputBuffer
{
protected:
	virtual void	InternalReset( void );
	//char *			Offset( void );
	//SI16			len;
	UI32			seed;
	UI32			majorVersion;
	UI32			minorVersion;
	UI32			clientRevision;
	UI32			clientPrototype;

public:
	virtual			~CPINewClientVersion()
	{
	}
	CPINewClientVersion();
	CPINewClientVersion( CSocket *s );
	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true ) override;
};

class CPIUpdateRangeChange : public CPInputBuffer
{
protected:
	virtual void	InternalReset( void );
public:
	virtual			~CPIUpdateRangeChange()
	{
	}
	CPIUpdateRangeChange();
	CPIUpdateRangeChange( CSocket *s );
	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true ) override;
};

class CPIKrriosClientSpecial : public CPInputBuffer
{
	UI08			type;
	UI16			blockLen;
	bool			locations;
protected:
	virtual void	InternalReset( void );
public:
	virtual			~CPIKrriosClientSpecial()
	{
	}
	CPIKrriosClientSpecial();
	CPIKrriosClientSpecial( CSocket *s );
	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true ) override;
};

class CPITips : public CPInputBuffer
{
protected:
	virtual void	InternalReset( void );
public:
	virtual			~CPITips()
	{
	}
	CPITips();
	CPITips( CSocket *s );
	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
};

class CPIRename : public CPInputBuffer
{
protected:
	virtual void	InternalReset( void );
public:
	virtual			~CPIRename()
	{
	}
	CPIRename();
	CPIRename( CSocket *s );
	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
};

class CPIKeepAlive : public CPInputBuffer
{
public:
	virtual			~CPIKeepAlive()
	{
	}
	CPIKeepAlive();
	CPIKeepAlive( CSocket *s );
	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
};

class CPIStatusRequest : public CPInputBuffer
{
	UI32			pattern;
	UI08			getType;
	SERIAL			playerID;
public:
	virtual			~CPIStatusRequest()
	{
	}
	CPIStatusRequest();
	CPIStatusRequest( CSocket *s );
	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true ) override;
};

class CPISpy : public CPInputBuffer
{
public:
	virtual			~CPISpy()
	{
	}
	CPISpy();
	CPISpy( CSocket *s );
	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
};

class CPIKRSeed : public CPInputBuffer
{
public:
	virtual			~CPIKRSeed()
	{
	}
	CPIKRSeed();
	CPIKRSeed( CSocket *s );
	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
};

class CPIKREncryptionVerification : public CPInputBuffer
{
public:
	virtual			~CPIKREncryptionVerification()
	{
	}
	CPIKREncryptionVerification();
	CPIKREncryptionVerification( CSocket *s );
	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
};

class CPIGodModeToggle : public CPInputBuffer
{
public:
	virtual			~CPIGodModeToggle()
	{
	}
	CPIGodModeToggle();
	CPIGodModeToggle( CSocket *s );
	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
};

class CPIDblClick : public CPInputBuffer
{
protected:
	SERIAL			objectID;
public:
	virtual			~CPIDblClick()
	{
	}
	CPIDblClick();
	CPIDblClick( CSocket *s );
	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true ) override;
};

class CPISingleClick : public CPInputBuffer
{
protected:
	SERIAL			objectID;
public:
	virtual			~CPISingleClick()
	{
	}
	CPISingleClick();
	CPISingleClick( CSocket *s );
	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true ) override;
};

class CPIMoveRequest : public CPInputBuffer
{
public:
	virtual			~CPIMoveRequest()
	{
	}
	CPIMoveRequest();
	CPIMoveRequest( CSocket *s );
	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true ) override;
};

class CPIResyncReq : public CPInputBuffer
{
public:
	virtual			~CPIResyncReq()
	{
	}
	CPIResyncReq();
	CPIResyncReq( CSocket *s );
	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
};

class CPIResMenuChoice : public CPInputBuffer
{
public:
	virtual			~CPIResMenuChoice()
	{
	}
	CPIResMenuChoice();
	CPIResMenuChoice( CSocket *s );
	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
};

class CPIAttack : public CPInputBuffer
{
public:
	virtual			~CPIAttack()
	{
	}
	CPIAttack();
	CPIAttack( CSocket *s );
	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
};

class CPITargetCursor : public CPInputBuffer
{
public:
	virtual			~CPITargetCursor()
	{
	}
	CPITargetCursor();
	CPITargetCursor( CSocket *s );
	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
	virtual void	Log( std::ofstream &outStream, bool fullHeader ) override;
};

class CPIEquipItem : public CPInputBuffer
{
public:
	virtual			~CPIEquipItem()
	{
	}
	CPIEquipItem();
	CPIEquipItem( CSocket *s );
	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
};

class CPIGetItem : public CPInputBuffer
{
public:
	virtual			~CPIGetItem()
	{
	}
	CPIGetItem();
	CPIGetItem( CSocket *s );
	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
};

class CPIDropItem : public CPInputBuffer
{
protected:
	bool			uokrFlag;
	SI16			x, y;
	SI08			z, gridLoc;
	SERIAL			item, dest;
public:
	virtual			~CPIDropItem()
	{
	}
	CPIDropItem();
	CPIDropItem( CSocket *s );
	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
};

class CPIGumpMenuSelect : public CPInputBuffer
{
public:
	CPIGumpMenuSelect();
	CPIGumpMenuSelect( CSocket *s );
	virtual			~CPIGumpMenuSelect()
	{
	}
	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;

	SERIAL			ButtonID( void ) const;
	SERIAL			GumpID( void ) const;
	SERIAL			ID( void ) const;
	UI32			SwitchCount( void ) const;
	UI32			TextCount( void ) const;
	UI32			SwitchValue( UI32 index ) const;

	UI16			GetTextID( UI08 number ) const;
	UI16			GetTextLength( UI08 number ) const;
	std::string		GetTextString( UI08 number ) const;

protected:
	SERIAL				id, buttonID, gumpID;
	UI32				switchCount, textCount;
	UI32				textOffset;

	std::vector< UI16 >	textLocationOffsets;

	void				BuildTextLocations( void );
};


class CPITalkRequest : public CPInputBuffer
{
protected:
	COLOUR				textColour;
	UI16				strLen;
	UI16				fontUsed;
	char				txtSaid[4096];
	SpeechType			typeUsed;
	bool				isUnicode;
	virtual void		InternalReset( void );
	bool				HandleCommon( void );
public:
	CPITalkRequest();
	CPITalkRequest( CSocket *s );
	virtual				~CPITalkRequest()
	{
	}
	virtual void		Receive( void ) override = 0;
	virtual bool		Handle( void ) override;

	COLOUR				TextColour( void )	const;
	UI16				Length( void )		const;
	UI16				Font( void )		const;
	SpeechType			Type( void )		const;
	bool				IsUnicode( void )	const;
	virtual std::string	TextString( void )	const;
	virtual char *		Text( void )		const;

};

class CPITalkRequestAscii : public CPITalkRequest
{
public:
	CPITalkRequestAscii();
	CPITalkRequestAscii( CSocket *s );
	virtual				~CPITalkRequestAscii()
	{
	}
	virtual void		Receive( void ) override;
};

class CPITalkRequestUnicode : public CPITalkRequest
{
protected:
	char unicodeTxt[8192];
	char langCode[4];
public:
	CPITalkRequestUnicode();
	CPITalkRequestUnicode( CSocket *s );
	virtual				~CPITalkRequestUnicode()
	{
	}
	virtual void		Receive( void ) override;

	virtual std::string	UnicodeTextString( void )	const;
	virtual char *		UnicodeText( void )			const;
	char *				Language( void )			const;

};

class CPIAllNames3D : public CPInputBuffer
{
public:
	virtual			~CPIAllNames3D()
	{
	}
	CPIAllNames3D();
	CPIAllNames3D( CSocket *s );
	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
};

class CPIGumpChoice : public CPInputBuffer
{
public:
	virtual			~CPIGumpChoice()
	{
	}
	CPIGumpChoice();
	CPIGumpChoice( CSocket *s );
	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
};

class CPIBuyItem : public CPInputBuffer
{
public:
	virtual			~CPIBuyItem()
	{
	}
	CPIBuyItem();
	CPIBuyItem( CSocket *s );
	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
};

class CPISellItem : public CPInputBuffer
{
public:
	virtual			~CPISellItem()
	{
	}
	CPISellItem();
	CPISellItem( CSocket *s );
	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
};

class CPIDeleteCharacter : public CPInputBuffer
{
public:
	virtual			~CPIDeleteCharacter()
	{
	}
	CPIDeleteCharacter();
	CPIDeleteCharacter( CSocket *s );
	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
};

class CPICreateCharacter : public CPInputBuffer
{
protected:
	UI32	pattern1;
	UI32	pattern2;
	UI08	pattern3;
	char	charName[30];
	char	password[30];
	UI08	sex;
	UI08	str;
	UI08	dex;
	UI08	intel;
	UI08	skill[4];
	UI08	skillValue[4];
	COLOUR	skinColour;
	UI16	hairStyle;
	COLOUR	hairColour;
	UI16	facialHair;
	COLOUR	facialHairColour;
	UI16	locationNumber;
	UI16	unknown;
	UI16	slot;
	UI32	ipAddress;
	COLOUR	shirtColour;
	COLOUR	pantsColour;

	//extra for 3DCharCreate - 0x8D
	UI08	packetSize;
	UI08	profession;
	UI08	clientFlags;
	UI08	race;
	UI16	unknown1;
	UI16	unknown2;
	char	unknown3[25];
	UI16	unknown4;
	UI08	unknown5;
	UI16	unknown6;
	UI08	unknown7;
	UI08	unknown8;
	UI08	unknown9;
	UI16	shirtID;
	COLOUR	faceColour;
	UI16	faceID;

	void	newbieItems( CChar *mChar );
public:
	virtual			~CPICreateCharacter()
	{
	}
	CPICreateCharacter();
	CPICreateCharacter( CSocket *s );
	virtual void	Create2DCharacter( void );
	virtual void	Create3DCharacter( void );
	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
	virtual void	SetNewCharGenderAndRace( CChar *mChar );
	virtual void	SetNewCharSkillsStats( CChar *mChar );
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true ) override;
};

class CPIPlayCharacter : public CPInputBuffer
{
protected:
	UI32			pattern;
	char			charName[30];
	char			unknown[33];
	UI08			slotChosen;
	UI32			ipAddress;
public:
	virtual			~CPIPlayCharacter()
	{
	}
	CPIPlayCharacter();
	CPIPlayCharacter( CSocket *s );
	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true ) override;
};

class CPIGumpInput : public CPInputBuffer
{
protected:
	UI32			id;
	UI08			type;
	UI08			index;
	UI08			unk[3];
	std::string		reply;

	void			HandleTownstoneText( UI08 index );
public:
	virtual			~CPIGumpInput()
	{
	}
	CPIGumpInput();
	CPIGumpInput( CSocket *s );
	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;

	UI32				ID( void ) const;
	UI08				Type( void ) const;
	UI08				Index( void ) const;
	UI08				Unk( SI32 offset ) const;
	const std::string	Reply( void ) const;
	virtual void		Log( std::ofstream &outStream, bool fullHeader = true ) override;
};

class CPIHelpRequest : public CPInputBuffer
{
protected:
	UI16			menuNum;
public:
	virtual			~CPIHelpRequest()
	{
	}
	CPIHelpRequest( CSocket *s, UI16 menuVal );
	CPIHelpRequest( CSocket *s );
	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
};

class CPITradeMessage : public CPInputBuffer
{
public:
	virtual			~CPITradeMessage()
	{
	}
	CPITradeMessage();
	CPITradeMessage( CSocket *s );
	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
};

class CPIDyeWindow : public CPInputBuffer
{
protected:
	SERIAL			changing;
	COLOUR			newValue;
	UI16			modelID;
public:
	virtual			~CPIDyeWindow()
	{
	}
	CPIDyeWindow();
	CPIDyeWindow( CSocket *s );
	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true ) override;
};

class CPIMsgBoardEvent : public CPInputBuffer
{
public:
	virtual			~CPIMsgBoardEvent()
	{
	}
	CPIMsgBoardEvent();
	CPIMsgBoardEvent( CSocket *s );
	CPIMsgBoardEvent( CSocket *s, bool receive );

	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
};

class CPINewBookHeader : public CPInputBuffer
{
public:
	virtual			~CPINewBookHeader()
	{
	}
	CPINewBookHeader();
	CPINewBookHeader( CSocket *s );

	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
};

class CPIBookPage : public CPInputBuffer
{
public:
	virtual			~CPIBookPage()
	{
	}
	CPIBookPage();
	CPIBookPage( CSocket *s );

	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
};

class CPIMetrics : public CPInputBuffer
{
public:
	virtual			~CPIMetrics()
	{
	}
	CPIMetrics();
	CPIMetrics( CSocket *s );

	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true ) override;

};

class CPISubcommands : public CPInputBuffer
{
protected:
	UI16			subCmd;
	UI08			subSubCmd;
	CPInputBuffer *	subPacket;
	bool			skipOver;
public:
	virtual			~CPISubcommands()
	{
	}
	CPISubcommands();
	CPISubcommands( CSocket *s );
	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true ) override;
};

class CPIPartyCommand : public CPInputBuffer
{
public:
	virtual			~CPIPartyCommand()
	{
	}
	CPIPartyCommand();
	CPIPartyCommand( CSocket *s );

	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true ) override;

};

class CPITrackingArrow : public CPInputBuffer
{
public:
	virtual			~CPITrackingArrow()
	{
	}
	CPITrackingArrow();
	CPITrackingArrow( CSocket *s );

	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true ) override;

};

class CPIClientLanguage : public CPInputBuffer
{
protected:
	UnicodeTypes	newLang;
public:
	virtual			~CPIClientLanguage()
	{
	}
	CPIClientLanguage();
	CPIClientLanguage( CSocket *s );

	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true ) override;

};

class CPIUOTDActions : public CPInputBuffer
{
protected:
	UI16			action;
public:
	virtual			~CPIUOTDActions()
	{
	}
	CPIUOTDActions();
	CPIUOTDActions( CSocket *s );

	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true ) override;

};

class CPIToolTipRequestAoS : public CPInputBuffer
{
protected:
	SERIAL			getSer;
public:
	virtual			~CPIToolTipRequestAoS()
	{
	}
	CPIToolTipRequestAoS();
	CPIToolTipRequestAoS( CSocket *s );

	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true ) override;

};

class CPIToolTipRequest : public CPInputBuffer
{
protected:
	SERIAL			getSer;
public:
	virtual			~CPIToolTipRequest()
	{
	}
	CPIToolTipRequest();
	CPIToolTipRequest( CSocket *s );

	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true ) override;

};

class CPIPopupMenuRequest : public CPInputBuffer
{
protected:
	SERIAL			mSer;
public:
	virtual			~CPIPopupMenuRequest()
	{
	}
	CPIPopupMenuRequest();
	CPIPopupMenuRequest( CSocket *s );

	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true ) override;

};

class CPIPopupMenuSelect : public CPInputBuffer
{
protected:
	UI16			popupEntry;
	CChar *			targChar;
public:
	virtual			~CPIPopupMenuSelect()
	{
	}
	CPIPopupMenuSelect();
	CPIPopupMenuSelect( CSocket *s );

	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true ) override;

};

class CPIExtendedStats : public CPInputBuffer
{
protected:
	UI08			statToSet;
	UI08			value;
public:
	virtual			~CPIExtendedStats()
	{
	}
	CPIExtendedStats();
	CPIExtendedStats( CSocket *s );

	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true ) override;

};

class CPIBandageMacro : public CPInputBuffer
{
public:
	virtual			~CPIBandageMacro()
	{
	}
	CPIBandageMacro();
	CPIBandageMacro(CSocket *s);

	virtual void	Receive(void) override;
	virtual bool	Handle(void) override;
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true ) override;

};

class CPIToggleFlying : public CPInputBuffer
{
public:
	virtual			~CPIToggleFlying()
	{
	}
	CPIToggleFlying();
	CPIToggleFlying(CSocket *s);

	virtual void	Receive(void) override;
	virtual bool	Handle(void) override;
	virtual void	Log(std::ofstream &outStream, bool fullHeader = true) override;

};

class CPISpellbookSelect : public CPInputBuffer
{
public:
	virtual			~CPISpellbookSelect()
	{
	}
	CPISpellbookSelect();
	CPISpellbookSelect( CSocket *s );

	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true ) override;

};

class CPIAOSCommand : public CPInputBuffer
{
public:
	virtual			~CPIAOSCommand()
	{
	}
	CPIAOSCommand();
	CPIAOSCommand( CSocket *s );

	virtual void	Receive( void ) override;
	virtual bool	Handle( void ) override;
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true ) override;

};

#endif

