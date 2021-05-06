#ifndef __CRESPONSE_H__
#define __CRESPONSE_H__

enum TargetIDs;

enum TriggerWords
{
	TW_BALANCE			= 0x0001,		// balance/statement
	TW_BANK				= 0x0002,		// bank
	TW_GUARDS			= 0x0007,		// guard/guards
	TW_QUESTDEST		= 0x001D,		// destination
	TW_QUESTTAKE		= 0x001E,		// I will take thee
	TW_HOUSELOCKDOWN	= 0x0023,		// I wish to lock this down
	TW_HOUSERELEASE		= 0x0024,		// I wish to release this
	TW_HOUSESECURE		= 0x0025,		// I wish to secure this
	TW_HOUSESTRONGBOX	= 0x0027,		// I wish to place a strongbox
	TW_RESIGN			= 0x002A,		// I resign from my guild
	TW_KILLS			= 0x0032,		// I must consider my sins
	TW_HOUSEEJECT		= 0x0033,		// Remove Thyself
	TW_HOUSEBAN			= 0x0034,		// I Ban Thee
	TW_STOP2			= 0x0036,
	TW_VENDORBUY		= 0x003C,		// Vendor Buy/Purchase
	TW_VENDORVIEW		= 0x003D,		// Vendor View/Browse/Look
	TW_VENDORGOLD		= 0x003E,		// Vendor Gold/Collect/Get
	TW_VENDORSTATUS		= 0x003F,		// Vendor Status/Info
	TW_VENDORDISMISS	= 0x0040,		// Vendor Dismiss/Replace
	TW_SETNAME			= 0x0042,		// Set Name
	TW_BOATFORWARD		= 0x0045,		// Forward
	TW_BOATBACKWARD		= 0x0046,		// Backward/Backwards/Back
	TW_BOATLEFT			= 0x0047,		// Left/Drift Left
	TW_BOATRIGHT		= 0x0048,		// Right/Drift Right
	TW_BOATSTARBOARD	= 0x0049,		// Starboard
	TW_BOATPORT			= 0x004A,		// Port
	TW_BOATFORWARDLEFT	= 0x004B,		// Forward Left
	TW_BOATFORWARDRIGHT	= 0x004C,		// Forward Right
	TW_BOATBACKLEFT		= 0x004D,		// Back Left/Backward Left/Backwards Left
	TW_BOATBACKRIGHT	= 0x004E,		// Back Right/Backward Right/Backwards Right
	TW_BOATSTOP			= 0x004F,		// Stop
	TW_BOATSLOWLEFT		= 0x0050,		// Slow Left
	TW_BOATSLOWRIGHT	= 0x0051,		// Slow Right
	TW_BOATSLOWFORWARD	= 0x0052,		// Slow Forward
	TW_BOATSLOWBACK		= 0x0053,		// Slow Back/Slow Backward/Slow Backwards
	TW_BOATSLOWFORWARDLEFT	= 0x0054,	// Slow Forward Left
	TW_BOATSLOWFORWARDRIGHT	= 0x0055,	// Slow Forward Right
	TW_BOATSLOWBACKRIGHT	= 0x0056,	// Slow Back Right/Slow Backward Right/Slow Backwards Right
	TW_BOATSLOWBACKLEFT		= 0x0057,	// Slow Back Left/Slow Backward Left/Slow Backwards Left
	TW_BOATONELEFT		= 0x0058,		// One Left/Left One
	TW_BOATONERIGHT		= 0x0059,		// One Right/Right One
	TW_BOATONEFORWARD	= 0x005A,		// One Forward/Forward One
	TW_BOATONEBACK		= 0x005B,		// One Back/One Backward/One Backwards/Back One/Backward One/Backwards One
	TW_BOATONEFORWARDLEFT	= 0x005C,	// One Forward Left/Forward Left One
	TW_BOATONEFORWARDRIGHT	= 0x005D,	// One Forward Right/Forward Right One
	TW_BOATONEBACKRIGHT	= 0x005E,		// One Back Right/One Backward Right/One Backwards Right/Back One Right/Backward One Right/Backwards One Right
	TW_BOATONEBACKLEFT	= 0x005F,		// One Back Left/One Backward Left/One Backwards Left/Back One Left/Backward One Left/Backwards One Left
	TW_BOATNAV			= 0x0060,		// Nav
	TW_BOATNAVSTART		= 0x0061,		// Start
	TW_BOATNAVCONTINUE	= 0x0062,		// Continue
	TW_BOATNAVGOTO		= 0x0063,		// Goto*
	TW_BOATNAVSINGLE	= 0x0064,		// Single*
	TW_BOATTURNRIGHT	= 0x0065,		// Turn Right
	TW_BOATTURNLEFT		= 0x0066,		// Turn Left
	TW_BOATTURNAROUND	= 0x0067,		// Turn Around/Come About
	TW_BOATUNFURL		= 0x0068,		// Unfurl Sail
	TW_BOATFURL			= 0x0069,		// Furl Sail
	TW_BOATANCHORDROP	= 0x006A,		// Drop Anchor/Lower Anchor
	TW_BOATANCHORRAISE	= 0x006B,		// Raise Anchor/Lift Anchor/Hoist Anchor
	TW_TRAIN			= 0x006C,		// Train, Teach
	TW_TRAINPARRY		= 0x006D,		// Train Parrying/Parry/Battle/Defense
	TW_TRAINHEAL		= 0x006E,		// Train First/Aid/Heal/Healing/Medicine
	TW_TRAINHIDE		= 0x006F,		// Train Hide/Hiding
	TW_TRAINSTEAL		= 0x0070,		// Train Steal/Stealing
	TW_TRAINALCHEMY		= 0x0071,		// Train Alchemy
	TW_TRAINANIMLORE	= 0x0072,		// Train Animal/Train Lore
	TW_TRAINIDENTIFY	= 0x0073,		// Train Appraising/Identifying/Appraise/Item/Identification/Identify
	TW_TRAINARMSLORE	= 0x0074,		// Train Armslore/Arms
	TW_TRAINBEG			= 0x0075,		// Train Beg/Begging
	TW_TRAINSMITH		= 0x0076,		// Train Blacksmith/Smith/Blacksmithy/Smithing/Blacksmithing

	TW_FOLLOW2			= 0x00E8,		// Follow
	TW_GOLD				= 0x0134,		// Gold
	TW_TRASHBARREL		= 0x013E,		// I wish to place a trash barrel
	TW_VENDORSELL		= 0x014D,		// Vendor Sell
	TW_COME				= 0x0155,		// Come
	TW_FETCH			= 0x0157,		// Fetch
	TW_GET				= 0x0158,		// Get
	TW_BRING			= 0x0159,		// Bring
	TW_FOLLOW			= 0x015A,		// Follow
	TW_FRIEND			= 0x015B,		// Friend
	TW_GUARD			= 0x015C,		// Guard (Pet)
	TW_KILL				= 0x015D,		// Kill
	TW_ATTACK			= 0x015E,		// Attack
	TW_STOP				= 0x0161,		// Stop
	TW_FOLLOWME			= 0x0163,		// Follow Me
	TW_ALLCOME			= 0x0164,		// All Come
	TW_ALLFOLLOW		= 0x0165,		// All Follow
	TW_ALLGUARD			= 0x0166,		// All Guard
	TW_ALLSTOP			= 0x0167,		// All Stop
	TW_ALLKILL			= 0x0168,		// All Kill
	TW_ALLATTACK		= 0x0169,		// All Attack
	TW_ALLGUARDME		= 0x016B,		// All Guard Me
	TW_ALLFOLLOWME		= 0x016C,		// All Follow Me
	TW_RELEASE			= 0x016D,		// Release
	TW_TRANSFER			= 0x016E,		// Transfer
	TW_STAY				= 0x016F,		// Stay
	TW_ALLSTAY			= 0x0170,		// All Stay
	TW_BUY				= 0x0171,		// Buy
	TW_VIEW				= 0x0172,		// View
	TW_COLLECT			= 0x0173,		// Collect
	TW_STATUS			= 0x0174,		// Status
	TW_DISMISS			= 0x0175,		// Dismiss
	TW_SELL				= 0x0177,		// Sell
	TW_COUNT			= 0xFFFF
};

class CBaseResponse
{
public:
	CBaseResponse( void )
	{
	}
	virtual			~CBaseResponse()
	{
	}
	virtual void	Handle( CSocket *mSock, CChar *mChar ) = 0;
};

class CKillsResponse : public CBaseResponse
{
public:
	CKillsResponse( void );
	virtual			~CKillsResponse()
	{
	}
	virtual void	Handle( CSocket *mSock, CChar *mChar ) override;
};

class CEscortResponse : public CBaseResponse
{
protected:
	bool			findDest;
public:
	CEscortResponse( bool newVal = false );
	virtual			~CEscortResponse()
	{
	}
	virtual void	Handle( CSocket *mSock, CChar *mChar ) override;
};

class CBankResponse : public CBaseResponse
{
protected:
	bool			checkBalance;
public:
	CBankResponse( bool newVal = false );
	virtual			~CBankResponse()
	{
	}
	virtual void	Handle( CSocket *mSock, CChar *mChar ) override;
};

class CTrainingResponse : public CBaseResponse
{
protected:
	std::string		ourText;
public:
	CTrainingResponse( const std::string &text );
	virtual			~CTrainingResponse()
	{
	}
	virtual void	Handle( CSocket *mSock, CChar *mChar ) override;
};

class CBasePetResponse : public CBaseResponse
{
protected:
	std::string		ourText;
public:
	CBasePetResponse( const std::string &text );
	virtual			~CBasePetResponse()
	{
	}
	virtual void	Handle( CSocket *mSock, CChar *mChar ) override;
	virtual bool	Handle( CSocket *mSock, CChar *mChar, CChar *Npc ) = 0;
	bool			canControlPet( CChar *mChar, CChar *Npc, bool isRestricted = false );
};

class CPetMultiResponse : public CBasePetResponse
{
protected:
	SI32			dictEntry;
	TargetIDs		targID;
	bool			isRestricted;
public:
	CPetMultiResponse( const std::string &text, bool isRestricted, TargetIDs targVal, SI32 dictVal );
	virtual			~CPetMultiResponse()
	{
	}
	virtual bool	Handle( CSocket *mSock, CChar *mChar, CChar *Npc ) override;
};

class CPetReleaseResponse : public CBasePetResponse
{
public:
	CPetReleaseResponse( const std::string &text );
	virtual			~CPetReleaseResponse()
	{
	}
	virtual bool	Handle( CSocket *mSock, CChar *mChar, CChar *Npc ) override;
};

class CPetAllResponse : public CBasePetResponse
{
protected:
	bool			saidAll;
public:
	CPetAllResponse( bool saidAll, const std::string &text );
	virtual			~CPetAllResponse()
	{
	}
};

class CPetGuardResponse : public CPetAllResponse
{
public:
	CPetGuardResponse( bool allVal, const std::string &text );
	virtual			~CPetGuardResponse()
	{
	}
	virtual bool	Handle( CSocket *mSock, CChar *mChar, CChar *Npc ) override;
};

class CPetAttackResponse : public CPetAllResponse
{
public:
	CPetAttackResponse( bool allVal, const std::string &text );
	virtual			~CPetAttackResponse()
	{
	}
	virtual bool	Handle( CSocket *mSock, CChar *mChar, CChar *Npc ) override;
};

class CPetComeResponse : public CPetAllResponse
{
public:
	CPetComeResponse( bool allVal, const std::string &text );
	virtual			~CPetComeResponse()
	{
	}
	virtual bool	Handle( CSocket *mSock, CChar *mChar, CChar *Npc ) override;
};

class CPetStayResponse : public CPetAllResponse
{
public:
	CPetStayResponse( bool allVal, const std::string &text );
	virtual			~CPetStayResponse()
	{
	}
	virtual bool	Handle( CSocket *mSock, CChar *mChar, CChar *Npc ) override;
};

class CBaseVendorResponse : public CBaseResponse
{
protected:
	bool			saidVendor;
	std::string		ourText;
public:
	CBaseVendorResponse( bool vendVal, const std::string &text );
	virtual			~CBaseVendorResponse()
	{
	}
	virtual void	Handle( CSocket *mSock, CChar *mChar ) override;
	virtual bool	Handle( CSocket *mSock, CChar *mChar, CChar *Npc ) = 0;
};

class CVendorBuyResponse : public CBaseVendorResponse
{
public:
	CVendorBuyResponse( bool vendVal, const std::string &text );
	virtual			~CVendorBuyResponse()
	{
	}
	virtual bool	Handle( CSocket *mSock, CChar *mChar, CChar *Npc ) override;
};

class CVendorSellResponse : public CBaseVendorResponse
{
public:
	CVendorSellResponse( bool vendVal, const std::string &text );
	virtual			~CVendorSellResponse()
	{
	}
	virtual bool	Handle( CSocket *mSock, CChar *mChar, CChar *Npc ) override;
};

class CVendorViewResponse : public CBaseVendorResponse
{
public:
	CVendorViewResponse( bool vendVal, const std::string &text );
	virtual			~CVendorViewResponse()
	{
	}
	virtual bool	Handle( CSocket *mSock, CChar *mChar, CChar *Npc ) override;
};

class CVendorGoldResponse : public CBaseVendorResponse
{
public:
	CVendorGoldResponse( bool vendVal, const std::string &text );
	virtual			~CVendorGoldResponse()
	{
	}
	virtual bool	Handle( CSocket *mSock, CChar *mChar, CChar *Npc ) override;
};

class CVendorStatusResponse : public CBaseVendorResponse
{
public:
	CVendorStatusResponse( bool vendVal, const std::string &text );
	virtual			~CVendorStatusResponse()
	{
	}
	virtual bool	Handle( CSocket *mSock, CChar *mChar, CChar *Npc ) override;
};

class CVendorDismissResponse : public CBaseVendorResponse
{
public:
	CVendorDismissResponse( bool vendVal, const std::string &text );
	virtual			~CVendorDismissResponse()
	{
	}
	virtual bool	Handle( CSocket *mSock, CChar *mChar, CChar *Npc ) override;
};

class CHouseMultiResponse : public CBaseResponse
{
protected:
	SI32			dictEntry;
	TargetIDs		targID;
public:
	CHouseMultiResponse( TargetIDs newTarg, SI32 newDict );
	virtual			~CHouseMultiResponse()
	{
	}
	virtual void	Handle( CSocket *mSock, CChar *mChar ) override;
};

class CBoatResponse : public CBaseResponse
{
protected:
	std::string		ourText;
	UI16			trigWord;
public:
	CBoatResponse( const std::string &text, UI16 tW );
	virtual			~CBoatResponse()
	{
	}
	virtual void	Handle( CSocket *mSock, CChar *mChar ) override;
};

class CBoatMultiResponse : public CBaseResponse
{
protected:
	BoatMoveType		moveType;
public:
	CBoatMultiResponse( BoatMoveType mType );
	virtual			~CBoatMultiResponse()
	{
	}
	virtual void	Handle( CSocket *mSock, CChar *mChar ) override;
};

#endif

