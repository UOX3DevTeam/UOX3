#ifndef __CPACKETRECEIVE_H__
#define __CPACKETRECEIVE_H__

#include "network.h"

namespace UOX
{

class CPIFirstLogin : public cPInputBuffer
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
						CPIFirstLogin( cSocket *s );
	virtual void		Receive( void );
	virtual const std::string Name( void );
	virtual const std::string Pass( void );
	virtual UI08		Unknown( void );
	virtual bool		Handle( void );
	virtual void		Log( std::ofstream &outStream, bool fullHeader = true );
};

class CPIServerSelect : public cPInputBuffer
{
protected:
	virtual void	InternalReset( void );
public:
	virtual			~CPIServerSelect()
	{
	}
					CPIServerSelect();
					CPIServerSelect( cSocket *s );
	virtual void	Receive( void );
	virtual SI16	ServerNum( void );
	virtual bool	Handle( void );
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true );
};

class CPISecondLogin : public cPInputBuffer
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
						CPISecondLogin( cSocket *s );
	virtual void		Receive( void );
	virtual UI32		Account( void );
	virtual const std::string Name( void );
	virtual const std::string Pass( void );
	virtual bool		Handle( void );
	virtual void		Log( std::ofstream &outStream, bool fullHeader = true );
};

class CPIClientVersion : public cPInputBuffer
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
					CPIClientVersion( cSocket *s );
	virtual void	Receive( void );
	virtual bool	Handle( void );
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true );
};

class CPIUpdateRangeChange : public cPInputBuffer
{
protected:
	virtual void	InternalReset( void );
public:
	virtual			~CPIUpdateRangeChange()
	{
	}
					CPIUpdateRangeChange();
					CPIUpdateRangeChange( cSocket *s );
	virtual void	Receive( void );
	virtual bool	Handle( void );
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true );
};

class CPITips : public cPInputBuffer
{
protected:
	virtual void	InternalReset( void );
public:
	virtual			~CPITips()
	{
	}
					CPITips();
					CPITips( cSocket *s );
	virtual void	Receive( void );
	virtual bool	Handle( void );
};

class CPIRename : public cPInputBuffer
{
protected:
	virtual void	InternalReset( void );
public:
	virtual			~CPIRename()
	{
	}
					CPIRename();
					CPIRename( cSocket *s );
	virtual void	Receive( void );
	virtual bool	Handle( void );
};

class CPIKeepAlive : public cPInputBuffer
{
public:
	virtual			~CPIKeepAlive()
	{
	}
					CPIKeepAlive();
					CPIKeepAlive( cSocket *s );
	virtual void	Receive( void );
	virtual bool	Handle( void );
};

class CPIStatusRequest : public cPInputBuffer
{
	UI32			pattern;
	UI08			getType;
	SERIAL			playerID;
public:
	virtual			~CPIStatusRequest()
	{
	}
					CPIStatusRequest();
					CPIStatusRequest( cSocket *s );
	virtual void	Receive( void );
	virtual bool	Handle( void );
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true );
};

class CPISpy : public cPInputBuffer
{
public:
	virtual			~CPISpy()
	{
	}
					CPISpy();
					CPISpy( cSocket *s );
	virtual void	Receive( void );
	virtual bool	Handle( void );
};

class CPIGodModeToggle : public cPInputBuffer
{
public:
	virtual			~CPIGodModeToggle()
	{
	}
					CPIGodModeToggle();
					CPIGodModeToggle( cSocket *s );
	virtual void	Receive( void );
	virtual bool	Handle( void );
};

class CPIDblClick : public cPInputBuffer
{
protected:
	SERIAL			objectID;
public:
	virtual			~CPIDblClick()
	{
	}
					CPIDblClick();
					CPIDblClick( cSocket *s );
	virtual void	Receive( void );
	virtual bool	Handle( void );
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true );
};

class CPISingleClick : public cPInputBuffer
{
protected:
	SERIAL			objectID;
public:
	virtual			~CPISingleClick()
	{
	}
					CPISingleClick();
					CPISingleClick( cSocket *s );
	virtual void	Receive( void );
	virtual bool	Handle( void );
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true );
};

class CPIMoveRequest : public cPInputBuffer
{
public:
	virtual			~CPIMoveRequest()
	{
	}
					CPIMoveRequest();
					CPIMoveRequest( cSocket *s );
	virtual void	Receive( void );
	virtual bool	Handle( void );
};

class CPIResyncReq : public cPInputBuffer
{
public:
	virtual			~CPIResyncReq()
	{
	}
					CPIResyncReq();
					CPIResyncReq( cSocket *s );
	virtual void	Receive( void );
	virtual bool	Handle( void );
};

class CPIResMenuChoice : public cPInputBuffer
{
public:
	virtual			~CPIResMenuChoice()
	{
	}
					CPIResMenuChoice();
					CPIResMenuChoice( cSocket *s );
	virtual void	Receive( void );
	virtual bool	Handle( void );
};

class CPIAttack : public cPInputBuffer
{
public:
	virtual			~CPIAttack()
	{
	}
					CPIAttack();
					CPIAttack( cSocket *s );
	virtual void	Receive( void );
	virtual bool	Handle( void );
};

class CPITargetCursor : public cPInputBuffer
{
public:
	virtual			~CPITargetCursor()
	{
	}
					CPITargetCursor();
					CPITargetCursor( cSocket *s );
	virtual void	Receive( void );
	virtual bool	Handle( void );
};

class CPIEquipItem : public cPInputBuffer
{
public:
	virtual			~CPIEquipItem()
	{
	}
					CPIEquipItem();
					CPIEquipItem( cSocket *s );
	virtual void	Receive( void );
	virtual bool	Handle( void );
};

class CPIGetItem : public cPInputBuffer
{
public:
	virtual			~CPIGetItem()
	{
	}
					CPIGetItem();
					CPIGetItem( cSocket *s );
	virtual void	Receive( void );
	virtual bool	Handle( void );
};

class CPIDropItem : public cPInputBuffer
{
public:
	virtual			~CPIDropItem()
	{
	}
					CPIDropItem();
					CPIDropItem( cSocket *s );
	virtual void	Receive( void );
	virtual bool	Handle( void );
};

class CPIGumpMenuSelect : public cPInputBuffer
{
public:
					CPIGumpMenuSelect();
					CPIGumpMenuSelect( cSocket *s );
	virtual			~CPIGumpMenuSelect()
	{
	}
	virtual void	Receive( void );
	virtual bool	Handle( void );

	SERIAL			ButtonID( void ) const;
	SERIAL			GumpID( void ) const;
	SERIAL			ID( void ) const;
	UI32			SwitchCount( void ) const;
	UI32			TextCount( void ) const;
	UI32			SwitchValue( UI32 index ) const;

	UI16			GetTextID( UI08 number ) const;
	UI16			GetTextLength( UI08 number ) const;
	std::string		GetTextString( UI08 number ) const;
	std::string		GetTextUString( UI08 number ) const;

protected:
	SERIAL				id, buttonID, gumpID;
	UI32				switchCount, textCount;
	size_t				textOffset;

	std::vector< UI16 >	textLocationOffsets;

	void				BuildTextLocations( void );
};


class CPITalkRequest : public cPInputBuffer
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
						CPITalkRequest( cSocket *s );
	virtual				~CPITalkRequest()
	{
	}
	virtual void		Receive( void ) = 0;
	virtual bool		Handle( void );

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
						CPITalkRequestAscii( cSocket *s );
	virtual				~CPITalkRequestAscii()
	{
	}
	virtual void		Receive( void );
};

class CPITalkRequestUnicode : public CPITalkRequest
{
protected:
	char unicodeTxt[8192];
	char langCode[4];
public:
						CPITalkRequestUnicode();
						CPITalkRequestUnicode( cSocket *s );
	virtual				~CPITalkRequestUnicode()
	{
	}
	virtual void		Receive( void );

	virtual std::string	UnicodeTextString( void )	const;
	virtual char *		UnicodeText( void )			const;
	char *				Language( void )			const;

};

class CPIAllNames3D : public cPInputBuffer
{
public:
	virtual			~CPIAllNames3D()
	{
	}
					CPIAllNames3D();
					CPIAllNames3D( cSocket *s );
	virtual void	Receive( void );
	virtual bool	Handle( void );
};

class CPIGumpChoice : public cPInputBuffer
{
public:
	virtual			~CPIGumpChoice()
	{
	}
					CPIGumpChoice();
					CPIGumpChoice( cSocket *s );
	virtual void	Receive( void );
	virtual bool	Handle( void );
};

class CPIBuyItem : public cPInputBuffer
{
public:
	virtual			~CPIBuyItem()
	{
	}
					CPIBuyItem();
					CPIBuyItem( cSocket *s );
	virtual void	Receive( void );
	virtual bool	Handle( void );
};

class CPISellItem : public cPInputBuffer
{
public:
	virtual			~CPISellItem()
	{
	}
					CPISellItem();
					CPISellItem( cSocket *s );
	virtual void	Receive( void );
	virtual bool	Handle( void );
};

class CPIDeleteCharacter : public cPInputBuffer
{
public:
	virtual			~CPIDeleteCharacter()
	{
	}
					CPIDeleteCharacter();
					CPIDeleteCharacter( cSocket *s );
	virtual void	Receive( void );
	virtual bool	Handle( void );
};

class CPICreateCharacter : public cPInputBuffer
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
	UI08	skill[3];
	UI08	skillValue[3];
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

	void	newbieItems( CChar *mChar );
public:
	virtual			~CPICreateCharacter()
	{
	}
					CPICreateCharacter();
					CPICreateCharacter( cSocket *s );
	virtual void	Receive( void );
	virtual bool	Handle( void );
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true );
};

class CPIPlayCharacter : public cPInputBuffer
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
					CPIPlayCharacter( cSocket *s );
	virtual void	Receive( void );
	virtual bool	Handle( void );
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true );
};

class CPIGumpInput : public cPInputBuffer
{
protected:
	UI32			id;
	UI08			type;
	UI08			index;
	UI08			unk[3];
	UString			reply;

	void			HandleTweakItemText( UI08 index );
	void			HandleTweakCharText( UI08 index );
	void			HandleTownstoneText( UI08 index );
public:
	virtual			~CPIGumpInput()
	{
	}
					CPIGumpInput();
					CPIGumpInput( cSocket *s );
	virtual void	Receive( void );
	virtual bool	Handle( void );

	UI32				ID( void ) const;
	UI08				Type( void ) const;
	UI08				Index( void ) const;
	UI08				Unk( int offset ) const;
	const UString		Reply( void ) const;
	virtual void		Log( std::ofstream &outStream, bool fullHeader = true );
};

class CPIHelpRequest : public cPInputBuffer
{
protected:
	UI16			menuNum;
public:
	virtual			~CPIHelpRequest()
	{
	}
					CPIHelpRequest( cSocket *s, UI16 menuVal );
					CPIHelpRequest( cSocket *s );
	virtual void	Receive( void );
	virtual bool	Handle( void );
};

class CPITradeMessage : public cPInputBuffer
{
public:
	virtual			~CPITradeMessage()
	{
	}
					CPITradeMessage();
					CPITradeMessage( cSocket *s );
	virtual void	Receive( void );
	virtual bool	Handle( void );
};

class CPIDyeWindow : public cPInputBuffer
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
					CPIDyeWindow( cSocket *s );
	virtual void	Receive( void );
	virtual bool	Handle( void );
	virtual void	Log( std::ofstream &outStream, bool fullHeader = true );
};

}

#endif

