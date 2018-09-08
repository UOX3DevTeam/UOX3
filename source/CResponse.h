#ifndef __CRESPONSE_H__
#define __CRESPONSE_H__

namespace UOX
{

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
	TW_BOATSTOP			= 0x004F,		// Stop
	TW_BOATTURNRIGHT	= 0x0065,		// Turn Right
	TW_BOATTURNLEFT		= 0x0066,		// Turn Left
	TW_BOATTURNAROUND	= 0x0067,		// Turn Around/Come About
	TW_BOATUNFURL		= 0x0068,		// Unfurl Sail
	TW_BOATFURL			= 0x0069,		// Furl Sail
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
	virtual void	Handle( CSocket *mSock, CChar *mChar );
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
	virtual void	Handle( CSocket *mSock, CChar *mChar );
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
	virtual void	Handle( CSocket *mSock, CChar *mChar );
};

class CTrainingResponse : public CBaseResponse
{
protected:
	std::string		ourText;
public:
					CTrainingResponse( std::string text );
	virtual			~CTrainingResponse()
					{
					}
	virtual void	Handle( CSocket *mSock, CChar *mChar );
};

class CBasePetResponse : public CBaseResponse
{
protected:
	std::string		ourText;
public:
					CBasePetResponse( std::string text );
	virtual			~CBasePetResponse()
					{
					}
	virtual void	Handle( CSocket *mSock, CChar *mChar );
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
					CPetMultiResponse( std::string text, bool isRestricted, TargetIDs targVal, SI32 dictVal );
	virtual			~CPetMultiResponse()
					{
					}
	virtual bool	Handle( CSocket *mSock, CChar *mChar, CChar *Npc );
};

class CPetReleaseResponse : public CBasePetResponse
{
public:
					CPetReleaseResponse( std::string text );
	virtual			~CPetReleaseResponse()
					{
					}
	virtual bool	Handle( CSocket *mSock, CChar *mChar, CChar *Npc );
};

class CPetAllResponse : public CBasePetResponse
{
protected:
	bool			saidAll;
public:
					CPetAllResponse( bool saidAll, std::string text );
	virtual			~CPetAllResponse()
					{
					}
};

class CPetGuardResponse : public CPetAllResponse
{
public:
					CPetGuardResponse( bool allVal, std::string text );
	virtual			~CPetGuardResponse()
					{
					}
	virtual bool	Handle( CSocket *mSock, CChar *mChar, CChar *Npc );
};

class CPetAttackResponse : public CPetAllResponse
{
public:
					CPetAttackResponse( bool allVal, std::string text );
	virtual			~CPetAttackResponse()
					{
					}
	virtual bool	Handle( CSocket *mSock, CChar *mChar, CChar *Npc );
};

class CPetComeResponse : public CPetAllResponse
{
public:
					CPetComeResponse( bool allVal, std::string text );
	virtual			~CPetComeResponse()
					{
					}
	virtual bool	Handle( CSocket *mSock, CChar *mChar, CChar *Npc );
};

class CPetStayResponse : public CPetAllResponse
{
public:
					CPetStayResponse( bool allVal, std::string text );
	virtual			~CPetStayResponse()
					{
					}
	virtual bool	Handle( CSocket *mSock, CChar *mChar, CChar *Npc );
};

class CBaseVendorResponse : public CBaseResponse
{
protected:
	bool			saidVendor;
	std::string		ourText;
public:
					CBaseVendorResponse( bool vendVal, std::string text );
	virtual			~CBaseVendorResponse()
					{
					}
	virtual void	Handle( CSocket *mSock, CChar *mChar );
	virtual bool	Handle( CSocket *mSock, CChar *mChar, CChar *Npc ) = 0;
};

class CVendorBuyResponse : public CBaseVendorResponse
{
public:
					CVendorBuyResponse( bool vendVal, std::string text );
	virtual			~CVendorBuyResponse()
					{
					}
	virtual bool	Handle( CSocket *mSock, CChar *mChar, CChar *Npc );
};

class CVendorSellResponse : public CBaseVendorResponse
{
public:
					CVendorSellResponse( bool vendVal, std::string text );
	virtual			~CVendorSellResponse()
					{
					}
	virtual bool	Handle( CSocket *mSock, CChar *mChar, CChar *Npc );
};

class CVendorViewResponse : public CBaseVendorResponse
{
public:
					CVendorViewResponse( bool vendVal, std::string text );
	virtual			~CVendorViewResponse()
					{
					}
	virtual bool	Handle( CSocket *mSock, CChar *mChar, CChar *Npc );
};

class CVendorGoldResponse : public CBaseVendorResponse
{
public:
					CVendorGoldResponse( bool vendVal, std::string text );
	virtual			~CVendorGoldResponse()
					{
					}
	virtual bool	Handle( CSocket *mSock, CChar *mChar, CChar *Npc );
};

class CVendorStatusResponse : public CBaseVendorResponse
{
public:
					CVendorStatusResponse( bool vendVal, std::string text );
	virtual			~CVendorStatusResponse()
					{
					}
	virtual bool	Handle( CSocket *mSock, CChar *mChar, CChar *Npc );
};

class CVendorDismissResponse : public CBaseVendorResponse
{
public:
					CVendorDismissResponse( bool vendVal, std::string text );
	virtual			~CVendorDismissResponse()
					{
					}
	virtual bool	Handle( CSocket *mSock, CChar *mChar, CChar *Npc );
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
	virtual void	Handle( CSocket *mSock, CChar *mChar );
};

class CBoatResponse : public CBaseResponse
{
protected:
	std::string		ourText;
	UI16			trigWord;
public:
					CBoatResponse( std::string text, UI16 tW );
	virtual			~CBoatResponse()
					{
					}
	virtual void	Handle( CSocket *mSock, CChar *mChar );
};

class CBoatMultiResponse : public CBaseResponse
{
protected:
	UI08			moveType;
public:
					CBoatMultiResponse( UI08 mType );
	virtual			~CBoatMultiResponse()
					{
					}
	virtual void	Handle( CSocket *mSock, CChar *mChar );
};

}

#endif

