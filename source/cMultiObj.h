#ifndef __CMULTIOBJ_H__
#define __CMULTIOBJ_H__

// UO High Seas vessel security levels.  The numeric order is
// significant: the highest applicable grant wins, while an explicit Denied
// manifest entry always takes precedence.
enum class BoatSecurityLevel : UI08
{
	NA = 0,
	Denied,
	Passenger,
	Crewman,
	Officer,
	Captain
};

enum class BoatPartyAccess : UI08
{
	Never = 0,
	LeaderOnly,
	MemberOnly
};


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
	SERIAL				pilot;
	SERIAL				pilotMount;
	UI08				pilotSpeed;
	SI32				hullHits;
	SI32				hullMaxHits;
	UI64				emergencyRepairUntil;
	UI64				boatDecayAt;
	UI64				nextSinkAt;
	UI08				sinkStep;
	SI08				moveType;
	// Temporary High Seas paint is persistent vessel state. Store one compact
	// native record instead of several world-file tag-map entries.
	UI16				paintBaseBoatHue;
	UI16				paintHue;
	UI08				paintCoats;
	UI64				paintDecayAt;
	bool				tillermanMoved;
	SI16				tillermanLocalX;
	SI16				tillermanLocalY;
	SI08				tillermanArtZ;
	// High Seas galleons are assembled from dynamic fixture items.  Keep their
	// identities on the boat, just as UO Galleon owns its Fixtures
	// collection, rather than rediscovering them from overlapping world tiles.
	std::vector<SERIAL>	fixtures;
	std::map<SERIAL, BoatSecurityLevel> securityManifest;
	BoatSecurityLevel	defaultPublicAccess;
	BoatSecurityLevel	defaultPartyAccess;
	BoatSecurityLevel	defaultGuildAccess;
	BoatPartyAccess		partyAccess;

	TIMERVAL			nextMoveTime;

private:
	virtual bool		DumpHeader( std::ostream &outStream ) const override;
	virtual bool		DumpBody( std::ostream &outStream ) const override;

	virtual bool		HandleLine( std::string &UTag, std::string &data ) override;

public:
	CBoatObj();
	virtual				~CBoatObj();
	virtual void		PostLoadProcessing( void ) override;

	SERIAL				GetTiller( void ) const;
	SERIAL				GetPlank( UI08 plankNum ) const;
	SERIAL				GetHold( void ) const;
	SERIAL				GetPilot( void ) const;
	SERIAL				GetPilotMount( void ) const;
	UI08				GetPilotSpeed( void ) const;
	SI32				GetHullHits( void ) const;
	SI32				GetHullMaxHits( void ) const;
	UI08				GetHullDamageLevel( void ) const;
	bool				IsScuttled( void ) const;
	bool				IsUnderEmergencyRepairs( void ) const;
	UI64				GetBoatDecayAt( void ) const;
	UI64				GetNextSinkAt( void ) const;
	UI08				GetSinkStep( void ) const;
	bool				IsSinking( void ) const;
	SI08				GetMoveType( void ) const;
	UI16				GetPaintBaseBoatHue( void ) const;
	UI16				GetPaintHue( void ) const;
	UI08				GetPaintCoats( void ) const;
	UI64				GetPaintDecayAt( void ) const;
	bool				IsTillermanMoved( void ) const;
	SI16				GetTillermanLocalX( void ) const;
	SI16				GetTillermanLocalY( void ) const;
	SI08				GetTillermanArtZ( void ) const;

	void				SetPlank( UI08 plankNum, SERIAL newVal );
	void				SetTiller( SERIAL newVal );
	void				SetHold( SERIAL newVal );
	void				SetPilot( SERIAL newVal );
	void				SetPilotMount( SERIAL newVal );
	void				SetPilotSpeed( UI08 newVal );
	void				SetHullHits( SI32 newVal );
	void				SetHullMaxHits( SI32 newVal );
	void				StartEmergencyRepairs( UI32 durationSeconds );
	void				RefreshBoatDecay( void );
	void				SetBoatDecayAt( UI64 newVal );
	void				SetNextSinkAt( UI64 newVal );
	void				SetSinkStep( UI08 newVal );
	void				SetMoveType( SI08 newVal );
	void				SetPaintState( UI16 baseBoatHue, UI16 basePaintHue, UI08 coats, UI64 decayAt );
	void				ClearPaintState( void );
	void				SetTillermanOffset( SI16 localX, SI16 localY );
	void				SetTillermanArtZ( SI08 artZ );
	void				RegisterFixture( SERIAL serial );
	void				UnregisterFixture( SERIAL serial );
	void				ClearFixtures( void );
	bool				IsFixture( SERIAL serial ) const;
	const std::vector<SERIAL>& GetFixtures( void ) const;

	BoatSecurityLevel	GetSecurityLevel( CChar *toCheck ) const;
	bool				HasAccess( CChar *toCheck ) const;
	bool				CanCommand( CChar *toCheck ) const;
	void				SetSecurityLevel( CChar *toSet, BoatSecurityLevel level );
	BoatSecurityLevel	GetDefaultPublicAccess( void ) const;
	BoatSecurityLevel	GetDefaultPartyAccess( void ) const;
	BoatSecurityLevel	GetDefaultGuildAccess( void ) const;
	BoatPartyAccess		GetPartyAccess( void ) const;
	void				SetDefaultPublicAccess( BoatSecurityLevel level );
	void				SetDefaultPartyAccess( BoatSecurityLevel level );
	void				SetDefaultGuildAccess( BoatSecurityLevel level );
	void				SetPartyAccess( BoatPartyAccess access );
	void				ResetSecurity( void );

	TIMERVAL			GetMoveTime( void ) const;
	void				SetMoveTime( TIMERVAL newVal );

	virtual bool		CanBeObjType( ObjectType toCompare ) const override;
};

#endif

