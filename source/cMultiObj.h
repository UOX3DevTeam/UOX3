#ifndef __CMULTIOBJ_H__
#define __CMULTIOBJ_H__


class CMultiObj : public CItem
{
protected:
	std::string				deed;

	// Temporary serial vectors for deferred loading
	std::vector<SERIAL>		pendingBans;
	std::vector<SERIAL>		pendingOwners;
	std::vector<SERIAL>		pendingFriends;
	std::vector<SERIAL>		pendingGuests;
	std::vector<SERIAL>		pendingLockedItems;
	std::vector<SERIAL>		pendingSecureContainers;
	std::vector<SERIAL>		pendingVendors;

	std::map<CChar *, UI08>	housePrivList;

	std::vector<CItem *>	lockedList;
	UI16					maxLockdowns;

	std::vector<CItem *>	secureContainerList;
	UI16					maxSecureContainers;

	UI16				maxFriends;
	UI16				maxGuests;
	UI16				maxOwners;
	UI16				maxBans;

	UI16				maxVendors;

	std::vector<CItem *>	trashContainerList;
	UI16					maxTrashContainers;

	std::vector<CChar *>	vendorList;

	SI16				banX;
	SI16				banY;

	bool				isPublic;
	UI16				numberOfVisits;

	time_t				buildTimestamp;
	time_t				tradeTimestamp;

	virtual bool		LoadRemnants( void ) override;

	GenericList<CItem *>	itemInMulti;
	GenericList<CChar *>	charInMulti;
	GenericList<CChar *>	ownersOfMulti;
	GenericList<CChar *>	friendsOfMulti;
	GenericList<CChar *>	bannedFromMulti;
	GenericList<CChar *>	guestsOfMulti;

public:
	CMultiObj();
	virtual				~CMultiObj();

	// Banlist
	void				AddToBanList( CChar *toBan );
	void				RemoveFromBanList( CChar *toRemove );
	void				ClearBanList( void );
	UI16				GetBanCount( void );
	UI16				GetMaxBans( void ) const;
	void				SetMaxBans( UI16 newValue );

	// Owner/CoOwnerlist
	void				AddAsOwner( CChar *newOwner );
	void				RemoveAsOwner( CChar *toRemove );
	void				ClearOwnerList( void );
	UI16				GetOwnerCount( void );
	UI16				GetMaxOwners( void ) const;
	void				SetMaxOwners( UI16 newValue );

	// Friendlist
	void				AddAsFriend( CChar *newFriend );
	void				RemoveAsFriend( CChar *toRemove );
	void				ClearFriendList( void );
	UI16				GetFriendCount( void );
	UI16				GetMaxFriends( void ) const;
	void				SetMaxFriends( UI16 newValue );

	// Guestlist (not saved)
	void				AddAsGuest( CChar *newGuest );
	void				RemoveAsGuest( CChar *toRemove );
	void				ClearGuestList( void );
	UI16				GetGuestCount( void );
	UI16				GetMaxGuests( void ) const;
	void				SetMaxGuests( UI16 newValue );

	// Lockdowns
	size_t				GetLockdownCount( void ) const;
	UI16				GetMaxLockdowns( void ) const;
	void				SetMaxLockdowns( UI16 newValue );
	void				LockDownItem( CItem *toLock );
	void				ReleaseItem( CItem *toRemove );

	// Secure Containers
	size_t				GetSecureContainerCount( void ) const;
	UI16				GetMaxSecureContainers( void ) const;
	void				SetMaxSecureContainers( UI16 newValue );
	bool				IsSecureContainer( CItem *toCheck );
	void				SecureContainer( CItem *toSecure );
	void				UnsecureContainer( CItem *toUnsecure );

	// Trash Containers
	size_t				GetTrashContainerCount( void ) const;
	UI16				GetMaxTrashContainers( void ) const;
	void				SetMaxTrashContainers( UI16 newValue );
	void				AddTrashContainer( CItem *toAdd );
	void				RemoveTrashContainer( CItem *toRemove );

	// Player Vendor list
	size_t				GetVendorCount( void ) const;
	UI16				GetMaxVendors( void ) const;
	void				SetMaxVendors( UI16 newValue );
	void				AddVendor( CChar *toAdd );
	void				RemoveVendor( CChar *toRemove );

	bool				IsOnBanList( CChar *toBan ) const;
	bool				IsOnFriendList( CChar *toCheck ) const;
	bool				IsOnGuestList( CChar *toCheck ) const;
	bool				IsOnOwnerList( CChar *toCheck ) const;
	bool				CheckForAccountCoOwnership( CChar *toCheck ) const;
	bool				IsOwner( CChar *toFind ) const;

	void				AddToMulti( CBaseObject *toAdd );
	void				RemoveFromMulti( CBaseObject *toRemove );

	bool				GetPublicStatus( void ) const;
	void				SetPublicStatus( const bool newBool );

	SI16				GetBanX( void ) const;
	void				SetBanX( const SI16 newVal );
	SI16				GetBanY( void ) const;
	void				SetBanY( const SI16 newVal );

	std::string			GetBuildTimestamp( void ) const;
	void				SetBuildTimestamp( time_t newTime );
	std::string			GetTradeTimestamp( void ) const;
	void				SetTradeTimestamp( time_t newTime );

	virtual bool		Save( std::ostream &outStream ) override;
	virtual bool		DumpHeader( std::ostream &outStream ) const override;
	virtual bool		DumpBody( std::ostream &outStream ) const override;
	virtual bool		HandleLine( std::string &UTag, std::string &data ) override;

	virtual void		SetOwner( CChar *newOwner ) override;

	std::string			GetDeed( void ) const;
	void				SetDeed( const std::string &newDeed );

	virtual void		Cleanup( void ) override;

	virtual bool		CanBeObjType( ObjectType toCompare ) const override;

	virtual void		PostLoadProcessing( void ) override;

	GenericList<CChar *> *	GetOwnersOfMultiList( bool clearList = false );
	GenericList<CChar *> *	GetFriendsOfMultiList( bool clearList = false );
	GenericList<CChar *> *	GetGuestsOfMultiList( bool clearList = false );
	GenericList<CChar *> *	GetBannedFromMultiList( bool clearList = false );
	GenericList<CChar *> *	GetCharsInMultiList( void );
	GenericList<CItem *> *	GetItemsInMultiList( void );

};

class CBoatObj : public CMultiObj
{
protected:
	SERIAL				tiller;
	SERIAL				planks[2];
	SERIAL				hold;
	SI08				moveType;

	TIMERVAL			nextMoveTime;

private:
	virtual bool		DumpHeader( std::ostream &outStream ) const override;
	virtual bool		DumpBody( std::ostream &outStream ) const override;

	virtual bool		HandleLine( std::string &UTag, std::string &data ) override;

public:
	CBoatObj();
	virtual				~CBoatObj();

	SERIAL				GetTiller( void ) const;
	SERIAL				GetPlank( UI08 plankNum ) const;
	SERIAL				GetHold( void ) const;
	SI08				GetMoveType( void ) const;

	void				SetPlank( UI08 plankNum, SERIAL newVal );
	void				SetTiller( SERIAL newVal );
	void				SetHold( SERIAL newVal );
	void				SetMoveType( SI08 newVal );

	TIMERVAL			GetMoveTime( void ) const;
	void				SetMoveTime( TIMERVAL newVal );

	virtual bool		CanBeObjType( ObjectType toCompare ) const override;
};

struct HouseTileEntry
{
    UI16 id;
    SI08 x;
    SI08 y;
    SI08 z;
};

struct HouseCustomSession
{
    SERIAL houseSerial;
    UI32 revision;
	UI08 clientLevel; // raw from client (1..3 typically)
	UI08 floor; // current floor selected by client (0 = ground)
	SI16 minX;
	SI16 maxX;
	SI16 minY;
	SI16 maxY;

    std::vector<HouseTileEntry> baseTiles;     // foundation border, always visible in design
    std::vector<HouseTileEntry> tiles;         // actual placed design tiles
    std::vector<HouseTileEntry> originalTiles; // snapshot from start of session
    std::vector<HouseTileEntry> backupTiles;   // set by Backup button
};

enum FoundationType : UI08
{
    FT_DarkWood = 0,
    FT_LightWood = 1,
    FT_Dungeon = 2,
    FT_Brick = 3,
    FT_Stone = 4,
    // add more as needed
};

bool HC_StartSession( CSocket *sock, SERIAL houseSerial );
void HC_EndSession( CSocket *sock );
void HC_CancelSession( CSocket *sock );
HouseCustomSession *HC_GetSession( CSocket *sock );
bool HC_IsSessionForHouse( CSocket *sock, SERIAL houseSerial );
bool HC_IsHiddenToCustomizer( CSocket *sock, CItem *item );
void HC_LoadExistingCustomTiles( HouseCustomSession &s, CItem *houseItem, CMultiObj *mMulti );
bool HC_LoadCommittedDesignTiles( CMultiObj *mMulti, std::vector<HouseTileEntry> &tiles );
bool HC_SendCommittedDesignState( CSocket *sock, CMultiObj *mMulti, bool enableResponse = false, bool allowActiveSession = false );
void HC_HideCustomHouseFixtures( CSocket *sock, CMultiObj *mMulti );
bool HC_SyncSessionFixtures( CSocket *sock, const HouseCustomSession &s );
void HC_BumpRevision( HouseCustomSession &s );
bool HC_CanPlaceTile( const HouseCustomSession &s, UI16 id, SI08 x, SI08 y, SI08 z );
bool HC_AddTile( HouseCustomSession &s, UI16 id, SI08 x, SI08 y, SI08 z );
bool HC_AddStairs( HouseCustomSession &s, UI16 multiId, SI08 x, SI08 y, SI08 z );
bool HC_AddRoofTile( HouseCustomSession &s, UI16 id, SI08 x, SI08 y, SI08 relativeZ );
bool HC_RemoveTile( HouseCustomSession &s, UI16 id, SI08 x, SI08 y, SI08 z );
bool HC_DeleteComponent( HouseCustomSession &s, UI16 id, SI08 x, SI08 y, SI08 z );
bool HC_DeleteRoofTile( HouseCustomSession &s, UI16 id, SI08 x, SI08 y, SI08 z );
bool HC_DeleteFixtureAt( CSocket *sock, const HouseCustomSession &s, UI16 id, SI08 x, SI08 y, SI08 z );
bool HC_CommitSession( CSocket *sock );
bool HC_RequestCommitConfirm( CSocket *sock );
bool HC_SetCustomizationFixture( CSocket *sock, CItem *houseItem, UI08 action, UI16 value );
SI32 HC_GetCommitCost( CItem *houseItem, const HouseCustomSession &s );
void HC_Backup( HouseCustomSession &s );
void HC_Restore( HouseCustomSession &s );
void HC_Revert( HouseCustomSession &s );
void HC_ClearAll( HouseCustomSession &s );
void HC_BuildCombinedTiles( const HouseCustomSession &s, std::vector<HouseTileEntry> &out );
void HC_SendDesignState( CSocket *sock, const HouseCustomSession &s, bool enableResponse = true );
bool HC_LoadFoundationTiles( CSocket* sock, HouseCustomSession& s );
void HC_RemoveAtXYZ( HouseCustomSession& s, SI08 x, SI08 y, SI08 z );

static SI08 FloorToDesignZ( UI08 floor )
{
    // Your expected planes:
    // floor 0 => 7
    // floor 1 => 27
    // floor 2 => 47
    // floor 3 => 67 (if you allow it)
    return (SI08)( 7 + (floor * 20) );
}

static SI08 SessionDesignZ( const HouseCustomSession *s )
{
    if( s == nullptr ) return 7;
    return FloorToDesignZ( s->floor );
}

namespace zlibhelper
{
    std::vector<UI08> Compress( const std::vector<UI08> &src );
    std::vector<UI08> Decompress( const std::vector<UI08> &src, size_t outSize );
}

#endif
