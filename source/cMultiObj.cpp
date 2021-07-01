//o-----------------------------------------------------------------------------------------------o
//|	File		-	cMultiObj.cpp
//|	Date		-	28th July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Version History
//|
//|						1.0		28th July, 2000
//|						Very basically fleshed out
//|						Owner, ban list and object inside support included
//|						Note that lockdown stuff is NOT persistent yet, needs support later on
//|						Best place would be world loading, any locked down objects flagged at time added to multi
//|
//|						1.1		28th August, 2000
//|						Function declaration headers
//|						Addition of SetMaxLockDowns
//|
//|						1.2		30th November, 2000
//|						Changed lock downs to a vector, and full vector support added
//|						Save needs modification
//|
//| 					1.3		25th September 2003
//|						Added CBoatObj as a derived class of CMultiObj to simplify some processes in the cBoat class
//o-----------------------------------------------------------------------------------------------o
#include "uox3.h"
#include "mapstuff.h"

const UI16	DEFMULTI_MAXLOCKDOWNS	= 256;
const UI16	DEFMULTI_MAXSECURECONTAINERS = 4;
const UI16	DEFMULTI_MAXFRIENDS = 50;
const UI16	DEFMULTI_MAXGUESTS = 50;
const UI16	DEFMULTI_MAXOWNERS = 8;
const UI16	DEFMULTI_MAXBANS = 50;
const UI16	DEFMULTI_MAXVENDORS = 10;
const UI16	DEFMULTI_MAXTRASHCONTAINERS = 1;
const UI16	DEFMULTI_BANX = 0;
const UI16	DEFMULTI_BANY = 0;
const time_t DEFMULTI_TRADETIMESTAMP = (time_t)(-1);

const UI08 HOUSEPRIV_OWNER	= 0;
const UI08 HOUSEPRIV_BANNED = 1;
const UI08 HOUSEPRIV_FRIEND = 2;
const UI08 HOUSEPRIV_GUEST = 3;

CMultiObj::CMultiObj() : CItem(), deed( "" ), maxLockdowns( DEFMULTI_MAXLOCKDOWNS ), maxSecureContainers( DEFMULTI_MAXSECURECONTAINERS ),
			maxFriends( DEFMULTI_MAXFRIENDS ), maxGuests( DEFMULTI_MAXGUESTS ), maxOwners( DEFMULTI_MAXOWNERS ), maxBans( DEFMULTI_MAXBANS ), maxVendors( DEFMULTI_MAXVENDORS ), 
			maxTrashContainers( DEFMULTI_MAXTRASHCONTAINERS ), banX( DEFMULTI_BANX ), banY( DEFMULTI_BANY ), isPublic( false ), tradeTimestamp( DEFMULTI_TRADETIMESTAMP )
{
	objType = OT_MULTI;
	housePrivList.clear();
}

CMultiObj::~CMultiObj()
{
	housePrivList.clear();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void AddToBanList( CChar *toBan )
//|	Date		-	28th July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a character to the ban list
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::AddToBanList( CChar *toBan )
{
	// Restrictions handled through JS
	housePrivList[toBan] = HOUSEPRIV_BANNED;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void RemoveFromBanList( CChar *toRemove )
//|	Date		-	28th July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes a character from a ban list
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::RemoveFromBanList( CChar *toRemove )
{
	std::map< CChar *, UI08 >::iterator rIter = housePrivList.find( toRemove );
	if( rIter != housePrivList.end() )
	{
		if( rIter->second == HOUSEPRIV_BANNED )
			housePrivList.erase( rIter );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void ClearBanList( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Clears ban list
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::ClearBanList( void )
{
	for (auto it = housePrivList.cbegin(), next_it = it; it != housePrivList.cend(); it = next_it)
	{
		++next_it;
		if( it->second == HOUSEPRIV_BANNED )
		{
			housePrivList.erase( it );
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void AddAsOwner( CChar *newOwner )
//|	Date		-	28th July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a player as an owner
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::AddAsOwner( CChar *newOwner )
{
	housePrivList[newOwner] = HOUSEPRIV_OWNER;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void RemoveAsOwner( CChar *toRemove )
//|	Date		-	28th July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes a character from the owner's list
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::RemoveAsOwner( CChar *toRemove )
{
	std::map< CChar *, UI08 >::iterator rIter = housePrivList.find( toRemove );
	if( rIter != housePrivList.end() )
	{
		if( rIter->second == HOUSEPRIV_OWNER )
			housePrivList.erase( rIter );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void ClearOwnerList( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Clears co-owner list
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::ClearOwnerList( void )
{
	for (auto it = housePrivList.cbegin(), next_it = it; it != housePrivList.cend(); it = next_it)
	{
		++next_it;
		if( it->second == HOUSEPRIV_OWNER )
		{
			if( owner != it->first->GetSerial() ) // Don't clear primary house owner from list!
				housePrivList.erase( it );
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void AddAsFriend( CChar *newFriend )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a player as a friend
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::AddAsFriend( CChar *newFriend )
{
	housePrivList[newFriend] = HOUSEPRIV_FRIEND;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void RemoveAsFriend( CChar *toRemove )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes a character from the friend list
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::RemoveAsFriend( CChar *toRemove )
{
	std::map< CChar *, UI08 >::iterator rIter = housePrivList.find( toRemove );
	if( rIter != housePrivList.end() )
	{
		if( rIter->second == HOUSEPRIV_FRIEND )
			housePrivList.erase( rIter );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void ClearFriendList( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Clears friend list
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::ClearFriendList( void )
{
	for (auto it = housePrivList.cbegin(), next_it = it; it != housePrivList.cend(); it = next_it)
	{
		++next_it;
		if( it->second == HOUSEPRIV_FRIEND )
		{
			housePrivList.erase( it );
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void AddAsGuest( CChar *newGuest )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a player as a guest
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::AddAsGuest( CChar *newGuest )
{
	housePrivList[newGuest] = HOUSEPRIV_GUEST;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void RemoveAsGuest( CChar *toRemove )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes a character from the guest list
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::RemoveAsGuest( CChar *toRemove )
{
	std::map< CChar *, UI08 >::iterator rIter = housePrivList.find( toRemove );
	if( rIter != housePrivList.end() )
	{
		if( rIter->second == HOUSEPRIV_GUEST )
			housePrivList.erase( rIter );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void ClearGuestList( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Clears guest list
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::ClearGuestList( void )
{
	for (auto it = housePrivList.cbegin(), next_it = it; it != housePrivList.cend(); it = next_it)
	{
		++next_it;
		if( it->second == HOUSEPRIV_GUEST )
		{
			housePrivList.erase( it );
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool IsOnBanList( CChar *toBan ) const
//|	Date		-	28th July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if a character is on the ban list
//o-----------------------------------------------------------------------------------------------o
bool CMultiObj::IsOnBanList( CChar *toBan ) const
{
	std::map< CChar *, UI08 >::const_iterator bIter = housePrivList.find( toBan );
	if( bIter != housePrivList.end() )
	{
		if( bIter->second == HOUSEPRIV_BANNED )
			return true;
	}
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool IsOnFriendList( CChar *toCheck ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if a character is on the friend list
//o-----------------------------------------------------------------------------------------------o
bool CMultiObj::IsOnFriendList( CChar *toCheck ) const
{
	std::map< CChar *, UI08 >::const_iterator bIter = housePrivList.find( toCheck );
	if( bIter != housePrivList.end() )
	{
		if( bIter->second == HOUSEPRIV_FRIEND )
			return true;
	}
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool IsOnGuestList( CChar *toCheck ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if a character is on the guest list
//o-----------------------------------------------------------------------------------------------o
bool CMultiObj::IsOnGuestList( CChar *toCheck ) const
{
	std::map< CChar *, UI08 >::const_iterator bIter = housePrivList.find( toCheck );
	if( bIter != housePrivList.end() )
	{
		if( bIter->second == HOUSEPRIV_GUEST )
			return true;
	}
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool IsOnOwnerList( CChar *toFind ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if the character is the owner or a co-owner
//o-----------------------------------------------------------------------------------------------o
bool CMultiObj::IsOnOwnerList( CChar *toFind ) const
{
	std::map< CChar *, UI08 >::const_iterator oIter = housePrivList.find( toFind );
	if( oIter != housePrivList.end() )
	{
		if( oIter->second == HOUSEPRIV_OWNER )
			return true;
	}
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CheckForAccountCoOwnership( CChar *toFind ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if the player has another character on same account that co-owns house
//o-----------------------------------------------------------------------------------------------o
bool CMultiObj::CheckForAccountCoOwnership( CChar *mChar ) const
{
	UI16 accountNum = mChar->GetAccountNum();
	std::map< CChar *, UI08 >::const_iterator oIter;
	for( oIter = housePrivList.begin(); oIter != housePrivList.end(); ++oIter )
	{
		if( ValidateObject( oIter->first ) )
		{
			// Check if account number of character in list matches our player's account number, and
			// that the character is stored in the list with owner privilege
			if( oIter->first->GetAccountNum() == accountNum && oIter->second == HOUSEPRIV_OWNER )
			{
				// Make sure it's not the actual owner of the house, though
				if( !IsOwner( oIter->first ))
					return true;
			}
		}
	}
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool IsOwner( CChar *toFind ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if the character is the actual owner of the house, and not a co-owner
//o-----------------------------------------------------------------------------------------------o
bool CMultiObj::IsOwner( CChar *toFind ) const
{
	return ( owner == toFind->GetSerial() );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void AddToMulti( CBaseObject *toAdd )
//|	Date		-	28th July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds an item or character to the multi
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::AddToMulti( CBaseObject *toAdd )
{
	if( toAdd->GetObjType() == OT_CHAR )
		charInMulti.Add( static_cast< CChar * >(toAdd) );
	else
		itemInMulti.Add( static_cast< CItem * >(toAdd) );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void RemoveFromMulti( CBaseObject *toRemove )
//|	Date		-	28th July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes an item or character from the multi, if possible
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::RemoveFromMulti( CBaseObject *toRemove )
{
	if( toRemove->GetObjType() == OT_CHAR )
	{
		charInMulti.Remove( static_cast< CChar * >(toRemove) );
		if( CanBeObjType( OT_BOAT ) && charInMulti.Num() == 0 )
		{
			if( (static_cast<CBoatObj *>(this))->GetMoveType() != -1 )
			{
				(static_cast<CBoatObj *>(this))->SetMoveType( 0 );
			}
		}
	}
	else
	{
		if( static_cast< CItem * >(toRemove)->GetTempVar( CITV_MORE ) != serial ) // Prevent doors and signs from being accidentally removed from house!
		{
			itemInMulti.Remove( static_cast< CItem * >(toRemove) );
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	size_t GetLockdownCount( void ) const
//|	Date		-	28th July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a count of the locked down items
//o-----------------------------------------------------------------------------------------------o
size_t CMultiObj::GetLockdownCount( void ) const
{
	return lockedList.size();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetMaxLockdowns( void ) const
//|	Date		-	28th July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the maximum number of possible lockdowns
//o-----------------------------------------------------------------------------------------------o
UI16 CMultiObj::GetMaxLockdowns( void ) const
{
	return maxLockdowns;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	size_t GetTrashContainerCount( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a count of the locked down items
//o-----------------------------------------------------------------------------------------------o
size_t CMultiObj::GetTrashContainerCount( void ) const
{
	return trashContainerList.size();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetMaxTrashContainers( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the maximum number of possible trash containers
//o-----------------------------------------------------------------------------------------------o
UI16 CMultiObj::GetMaxTrashContainers( void ) const
{
	return maxTrashContainers;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	size_t GetVendorCount( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a count of the player vendors in the multi
//o-----------------------------------------------------------------------------------------------o
size_t CMultiObj::GetVendorCount( void ) const
{
	return vendorList.size();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetMaxVendors( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes a player vendor from the multi
//o-----------------------------------------------------------------------------------------------o
UI16 CMultiObj::GetMaxVendors( void ) const
{
	return maxVendors;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetFriendCount( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a count of the amount of friends of multi
//o-----------------------------------------------------------------------------------------------o
UI16 CMultiObj::GetFriendCount( void )
{
	UI16 friendCount = 0;
	for( std::map< CChar *, UI08 >::iterator rIter = housePrivList.begin(); rIter != housePrivList.end(); ++rIter )
	{
		if( rIter->second == HOUSEPRIV_FRIEND )
			friendCount++;
	}
	return friendCount;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetMaxFriends( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the maximum number of friends multi can have
//o-----------------------------------------------------------------------------------------------o
UI16 CMultiObj::GetMaxFriends( void ) const
{
	return maxFriends;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetGuestCount( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a count of the amount of guests of multi
//o-----------------------------------------------------------------------------------------------o
UI16 CMultiObj::GetGuestCount( void )
{
	UI16 guestCount = 0;
	for( std::map< CChar *, UI08 >::iterator rIter = housePrivList.begin(); rIter != housePrivList.end(); ++rIter )
	{
		if( rIter->second == HOUSEPRIV_GUEST )
			guestCount++;
	}
	return guestCount;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetMaxGuests( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the maximum number of guests multi can have
//o-----------------------------------------------------------------------------------------------o
UI16 CMultiObj::GetMaxGuests( void ) const
{
	return maxGuests;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetBanCount( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a count of the amount of banned players of multi
//o-----------------------------------------------------------------------------------------------o
UI16 CMultiObj::GetBanCount( void )
{
	UI16 banCount = 0;
	for( std::map< CChar *, UI08 >::iterator rIter = housePrivList.begin(); rIter != housePrivList.end(); ++rIter )
	{
		if( rIter->second == HOUSEPRIV_BANNED )
			banCount++;
	}
	return banCount;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetMaxBans( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the maximum number of banned players multi can have
//o-----------------------------------------------------------------------------------------------o
UI16 CMultiObj::GetMaxBans( void ) const
{
	return maxBans;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetOwnerCount( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a count of the amount of co-owners of multi
//o-----------------------------------------------------------------------------------------------o
UI16 CMultiObj::GetOwnerCount( void )
{
	UI16 ownerCount = 0;
	for( std::map< CChar *, UI08 >::iterator rIter = housePrivList.begin(); rIter != housePrivList.end(); ++rIter )
	{
		if( rIter->second == HOUSEPRIV_OWNER )
		{
			// Don't count primary house owner as co-owner
			if( rIter->first->GetSerial() != owner )
				ownerCount++;
		}
	}
	return ownerCount;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetMaxOwners( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the maximum number of co-owners multi can have
//o-----------------------------------------------------------------------------------------------o
UI16 CMultiObj::GetMaxOwners( void ) const
{
	return maxOwners;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void LockDownItem( CItem *toLock )
//|	Date		-	28th July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Locks down an item toLock
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::LockDownItem( CItem *toLock )
{
	if( lockedList.size() < maxLockdowns )
	{
		for( ITEMLIST_CITERATOR lIter = lockedList.begin(); lIter != lockedList.end(); ++lIter )
		{
			if( (*lIter) == toLock )
				return;
		}
		toLock->LockDown();
		toLock->Dirty( UT_UPDATE );
		lockedList.push_back( toLock );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void ReleaseItem( CItem *toRemove )
//|	Date		-	28th July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Unlocks a locked down item
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::ReleaseItem( CItem *toRemove )
{
	for( ITEMLIST_ITERATOR rIter = lockedList.begin(); rIter != lockedList.end(); ++rIter )
	{
		if( (*rIter) == toRemove )
		{
			toRemove->Dirty( UT_UPDATE );
			lockedList.erase( rIter );
			toRemove->SetMovable( 0 );
			toRemove->SetDecayTime( cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_DECAYINHOUSE ));
			return;
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void AddTrashContainer( CItem *toAdd )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a trash container to the multi's trash container list
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::AddTrashContainer( CItem *toAdd )
{
	if( trashContainerList.size() < maxTrashContainers )
	{
		for( ITEMLIST_CITERATOR lIter = trashContainerList.begin(); lIter != trashContainerList.end(); ++lIter )
		{
			if( (*lIter) == toAdd )
				return;
		}
		trashContainerList.push_back( toAdd );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void RemoveTrashContainer( CItem *toRemove )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Unlocks a locked down item
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::RemoveTrashContainer( CItem *toRemove )
{
	for( ITEMLIST_ITERATOR rIter = trashContainerList.begin(); rIter != trashContainerList.end(); ++rIter )
	{
		if( (*rIter) == toRemove )
		{
			trashContainerList.erase( rIter );
			return;
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void AddVendor( CChar *toAdd )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a player vendor to the multi
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::AddVendor( CChar *toAdd )
{
	if( vendorList.size() < maxVendors )
	{
		for( CHARLIST_CITERATOR lIter = vendorList.begin(); lIter != vendorList.end(); ++lIter )
		{
			if( (*lIter) == toAdd )
				return;
		}
		toAdd->Dirty( UT_UPDATE );
		vendorList.push_back( toAdd );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void RemoveVendor( CChar *toRemove )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes a player vendor from the multi
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::RemoveVendor( CChar *toRemove )
{
	for( CHARLIST_ITERATOR rIter = vendorList.begin(); rIter != vendorList.end(); ++rIter )
	{
		if( (*rIter) == toRemove )
		{
			toRemove->Dirty( UT_UPDATE );
			vendorList.erase( rIter );
			return;
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void GetPublicStatus( void ) const
//|	Function	-	void SetPublicStatus( bool newBool )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the public status of the multi - either true (public) or false (private)
//o-----------------------------------------------------------------------------------------------o
bool CMultiObj::GetPublicStatus( void ) const
{
	return isPublic;
}
void CMultiObj::SetPublicStatus( bool newBool )
{
	isPublic = newBool;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void GetBuildTimestamp( void ) const
//|	Function	-	void SetBuildTimestamp( bool newBool )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the timestamp for when the multi was built
//o-----------------------------------------------------------------------------------------------o
std::string CMultiObj::GetBuildTimestamp( void ) const
{
	if( buildTimestamp > 0 )
	{
		char tempTimestamp[100];
		struct tm * curtime = std::localtime( &buildTimestamp );
		strftime( tempTimestamp, 50, "%F at %T", curtime );
		return tempTimestamp;
	}
	else
		return "Unknown";
}
void CMultiObj::SetBuildTimestamp( time_t newTime )
{
	if( newTime && newTime > 0 )
		buildTimestamp = newTime;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void GetTradeTimestamp( void ) const
//|	Function	-	void SetTradeTimestamp( bool newBool )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the timestamp for when the multi was last traded between players
//o-----------------------------------------------------------------------------------------------o
std::string CMultiObj::GetTradeTimestamp( void ) const
{
	if( tradeTimestamp > 0 )
	{
		char tempTimestamp[100];
		struct tm * curtime = std::localtime( &tradeTimestamp );
		strftime( tempTimestamp, 50, "%F at %T", curtime );
		return tempTimestamp;
	}
	else
		return "Never";
}
void CMultiObj::SetTradeTimestamp( time_t newTime )
{
	tradeTimestamp = newTime;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	size_t GetSecureContainerCount( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a count of the amount of secure containers in the multi
//o-----------------------------------------------------------------------------------------------o
size_t CMultiObj::GetSecureContainerCount( void ) const
{
	return secureContainerList.size();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetMaxSecureContainers( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the maximum number of possible secure containers for a house
//o-----------------------------------------------------------------------------------------------o
UI16 CMultiObj::GetMaxSecureContainers( void ) const
{
	return maxSecureContainers;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void IsSecureContainer( CItem *toSecure )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if item is a secure container
//o-----------------------------------------------------------------------------------------------o
bool CMultiObj::IsSecureContainer( CItem *toCheck )
{
	for( ITEMLIST_ITERATOR rIter = secureContainerList.begin(); rIter != secureContainerList.end(); ++rIter )
	{
		if( (*rIter) == toCheck )
		{
			return true;
		}
	}
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SecureContainer( CItem *toSecure )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Secures specified container
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::SecureContainer( CItem *toSecure )
{
	if( secureContainerList.size() < maxSecureContainers )
	{
		for( ITEMLIST_CITERATOR lIter = secureContainerList.begin(); lIter != secureContainerList.end(); ++lIter )
		{
			if( (*lIter) == toSecure )
				return;
		}

		if( toSecure->GetType() != 87 ) // Don't lock down trash containers
		{
			toSecure->LockDown();
		}
		toSecure->Dirty( UT_UPDATE );
		secureContainerList.push_back( toSecure );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void UnsecureContainer( CItem *toUnsecure )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Unsecures a secured container
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::UnsecureContainer( CItem *toUnsecure )
{
	for( ITEMLIST_ITERATOR rIter = secureContainerList.begin(); rIter != secureContainerList.end(); ++rIter )
	{
		if( (*rIter) == toUnsecure )
		{
			toUnsecure->Dirty( UT_UPDATE );
			secureContainerList.erase( rIter );
			if( toUnsecure->GetType() != 87 ) // Trash container
			{
				toUnsecure->SetMovable( 1 );
				toUnsecure->SetDecayTime( cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_DECAYINHOUSE ));
			}
			return;
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool Save( std::ofstream &outStream )
//|	Date		-	28th July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Saves a multi out to disk. outStream is the file to write to
//o-----------------------------------------------------------------------------------------------o
bool CMultiObj::Save( std::ofstream &outStream )
{
	bool rvalue = false;
	if( !isFree() )
	{
		rvalue = true;
		MapData_st& mMap = Map->GetMapData( worldNumber );
		if( GetCont() != nullptr || ( GetX() > 0 && GetX() < mMap.xBlock && GetY() < mMap.yBlock ) )
		{
			DumpHeader( outStream );
			DumpBody( outStream );
			DumpFooter( outStream );
		}
	}
	return rvalue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool LoadRemnants( void )
//|	Date		-	20th January, 2002
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	After handling data specific load, other parts go here
//o-----------------------------------------------------------------------------------------------o
bool CMultiObj::LoadRemnants( void )
{
	bool rValue = CItem::LoadRemnants();

	// Add item weight if item doesn't have it yet
	if( GetWeight() <= 0 )
		SetWeight( 0 );
	return rValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetMaxLockdowns( UI16 newValue )
//|	Date		-	28th August, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the maximum number of lockdowns possible
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::SetMaxLockdowns( UI16 newValue )
{
	maxLockdowns = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetMaxTrashContainers( UI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the maximum number of trash containers possible
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::SetMaxTrashContainers( UI16 newValue )
{
	maxTrashContainers = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetMaxVendors( UI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the maximum number of player vendors possible
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::SetMaxVendors( UI16 newValue )
{
	maxVendors = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetMaxSecureContainers( UI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the maximum number of secure containers possible
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::SetMaxSecureContainers( UI16 newValue )
{
	maxSecureContainers = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetMaxBans( UI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the maximum number of banned players possible
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::SetMaxBans( UI16 newValue )
{
	maxBans = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetMaxFriends( UI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the maximum number of friends possible
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::SetMaxFriends( UI16 newValue )
{
	maxFriends = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetMaxGuests( UI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the maximum number of guests possible
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::SetMaxGuests( UI16 newValue )
{
	maxGuests = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetMaxOwners( UI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the maximum number of co-owners possible
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::SetMaxOwners( UI16 newValue )
{
	maxOwners = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool DumpHeader( std::ofstream &outStream ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Dumps out the header for the CMultiObj
//o-----------------------------------------------------------------------------------------------o
bool CMultiObj::DumpHeader( std::ofstream &outStream ) const
{
	outStream << "[HOUSE]" << '\n';
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool DumpBody( std::ofstream &outStream ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Writes out all CMultiObj specific information to a world
//|					file.  Also prints out the Item and BaseObject portions
//o-----------------------------------------------------------------------------------------------o
bool CMultiObj::DumpBody( std::ofstream &outStream ) const
{
	CItem::DumpBody( outStream );

	// Hexadecimal Values
	outStream << std::hex;

	// Decimal / String Values
	outStream << std::dec;
	std::map< CChar *, UI08 >::const_iterator oIter;
	for( oIter = housePrivList.begin(); oIter != housePrivList.end(); ++oIter )
	{
		if( ValidateObject( oIter->first ) )
		{
			if( oIter->second == HOUSEPRIV_OWNER )
				outStream << "Owner=";
			else if( oIter->second == HOUSEPRIV_BANNED )
				outStream << "Banned=";
			else if( oIter->second == HOUSEPRIV_FRIEND )
				outStream << "Friend=";
			else if( oIter->second == HOUSEPRIV_GUEST )
				outStream << "Guest=";
			else
				continue;
			outStream << oIter->first->GetSerial() << '\n';
		}
	}
	outStream << "IsPublic=" << ( isPublic ? "1" : "0" ) << '\n';
	outStream << "MaxBans=" << maxBans << '\n';
	outStream << "MaxFriends=" << maxFriends << '\n';
	outStream << "MaxGuests=" << maxGuests << '\n';

	ITEMLIST_CITERATOR lIter;
	for( lIter = lockedList.begin(); lIter != lockedList.end(); ++lIter )
	{
		if( ValidateObject( (*lIter) ) )
			outStream << "LockedItem=" << (*lIter)->GetSerial() << '\n';
	}

	ITEMLIST_CITERATOR lIterSecure;
	for( lIterSecure = secureContainerList.begin(); lIterSecure != secureContainerList.end(); ++lIterSecure )
	{
		if( ValidateObject( (*lIterSecure) ) )
			outStream << "SecureContainer=" << (*lIterSecure)->GetSerial() << '\n';
	}

	outStream << "MaxLockdowns=" << maxLockdowns << '\n';
	outStream << "MaxSecureContainers=" << maxSecureContainers << '\n';

	CHARLIST_CITERATOR lIterVendor;
	for( lIterVendor = vendorList.begin(); lIterVendor != vendorList.end(); ++lIterVendor )
	{
		if( ValidateObject( (*lIterVendor) ) )
			outStream << "Vendor=" << (*lIterVendor)->GetSerial() << '\n';
	}

	outStream << "MaxVendors=" << maxVendors << '\n';
	// Add deedname= to the save :) We need a way to preserve the name of the item
	outStream << "DeedName=" << deed << '\n';
	outStream << "BuildTime=" << buildTimestamp << '\n';
	outStream << "TradeTime=" << tradeTimestamp << '\n';
	return true;
}


//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool HandleLine( std::string &UTag, std::string &data )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Processes a tag/data pair if it can.  If it can, returns
//|					true.  Otherwise, returns false.
//o-----------------------------------------------------------------------------------------------o
bool CMultiObj::HandleLine( std::string &UTag, std::string &data )
{
	bool rvalue = CItem::HandleLine( UTag, data );
	auto csecs = strutil::sections( data, "," );
	
	if( !rvalue )
	{
		switch( (UTag.data()[0]) )
		{
			case 'B':
				if( UTag == "BANNED" )
				{
					CChar *bList = calcCharObjFromSer( static_cast<UI32>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					if( ValidateObject( bList ))
					{
						AddToBanList( bList );
					}
					rvalue = true;
				}
				else if( UTag == "BUILDTIME" )
				{
					time_t buildTime = static_cast<UI64>(std::stoll(strutil::stripTrim( data ), nullptr, 0));
					if( buildTime > 0 )
					{
						SetBuildTimestamp( buildTime );
					}
					rvalue = true;
				}
				break;
			case 'C':
				if( UTag == "COOWNER" ) // Legacy tag for loading older data
				{
					CChar *cList = calcCharObjFromSer( static_cast<UI32>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					if( ValidateObject( cList ))
					{
						AddAsOwner( cList );
					}
					rvalue = true;
				}
				break;
			case 'D':
				if( UTag == "DEEDNAME" )
				{
					SetDeed( data );
					rvalue = true;
				}
				break;
			case 'F':
				if( UTag == "FRIEND" )
				{
					CChar *cList = calcCharObjFromSer( static_cast<UI32>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					if( ValidateObject( cList ))
					{
						AddAsFriend( cList );
					}
					rvalue = true;
				}
				break;
			case 'G':
				if( UTag == "GUEST" )
				{
					CChar *cList = calcCharObjFromSer( static_cast<UI32>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					if( ValidateObject( cList ))
					{
						AddAsGuest( cList );
					}
					rvalue = true;
				}
				break;
			case 'I':
				if( UTag == "ISPUBLIC" )
				{
					SetPublicStatus( static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) == 1 );
					rvalue = true;
				}
				break;
			case 'L':
				if( UTag == "LOCKEDITEM" )
				{
					CItem *iList = calcItemObjFromSer( static_cast<UI32>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					if( ValidateObject( iList ) )
						LockDownItem( iList );
					rvalue = true;
				}
				break;
			case 'M':
				if( UTag == "MAXBANS" )
				{
					maxBans = static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0));
					rvalue = true;
				}
				else if( UTag == "MAXFRIENDS" )
				{
					maxFriends = static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0));
					rvalue = true;
				}
				else if( UTag == "MAXGUESTS" )
				{
					maxGuests = static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0));
					rvalue = true;
				}
				else if( UTag == "MAXLOCKDOWNS" || UTag == "MAXLOCKEDDOWN" )
				{
					maxLockdowns = static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0));
					rvalue = true;
				}
				else if( UTag == "MAXOWNERS" )
				{
					maxOwners = static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0));
					rvalue = true;
				}
				else if( UTag == "MAXSECURECONTAINERS" )
				{
					maxSecureContainers = static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0));
					rvalue = true;
				}
				else if( UTag == "MAXVENDORS" )
				{
					maxVendors = static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0));
					rvalue = true;
				}
				break;
			case 'O':
				if( UTag == "OWNER" )
				{
					CChar *cList = calcCharObjFromSer( static_cast<UI32>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					if( ValidateObject( cList ))
					{
						AddAsOwner( cList );
					}
					rvalue = true;
				}
				break;
			case 'S':
				if( UTag == "SECURECONTAINER" )
				{
					CItem *iList = calcItemObjFromSer( static_cast<UI32>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					if( ValidateObject( iList ))
					{
						SecureContainer( iList );
					}
					rvalue = true;
				}
				break;
			case 'T':
				if( UTag == "TRADETIME" )
				{
					time_t tradeTime = static_cast<UI64>(std::stoll(strutil::stripTrim( data ), nullptr, 0));
					if( tradeTime > 0 )
					{
						SetTradeTimestamp( tradeTime );
					}
					rvalue = true;
				}
				break;
			case 'V':
				if( UTag == "VENDOR" )
				{
					CChar *cList = calcCharObjFromSer( static_cast<UI32>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					if( ValidateObject( cList ) )
						AddVendor( cList );
					rvalue = true;
				}
				break;
		}
	}
	return rvalue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetOwner( CChar *newOwner )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Ensures that newOwner is also on the house ownership list.
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::SetOwner( CChar *newOwner )
{
	RemoveSelfFromOwner(); // Remove multi from previous owner's list of owned items
	if( ValidateObject( newOwner ) )
	{
		owner = newOwner->GetSerial();
		AddAsOwner( newOwner );
		AddSelfToOwner(); // Add multi to owner's list of owned items
	}
	else
		owner = INVALIDSERIAL;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void Cleanup( void )
//|	Date		-	26th September, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Makes sure that any items and chars inside the multi are removed
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::Cleanup( void )
{
	for( CItem *iRemove = itemInMulti.First(); !itemInMulti.Finished(); iRemove = itemInMulti.Next() )
	{
		if( ValidateObject( iRemove ) )
		{
			ItemTypes iType = iRemove->GetType();
			if( iType == IT_DOOR || iType == IT_LOCKEDDOOR || iType == IT_HOUSESIGN )
				iRemove->Delete();
			else
			{
				if( iRemove->IsLockedDown() )
					iRemove->SetMovable( 1 );
				iRemove->SetMulti( INVALIDSERIAL, false );
				iRemove->SetZ( GetZ() );
			}
		}
	}
	for( CChar *cRemove = charInMulti.First(); !charInMulti.Finished(); cRemove = charInMulti.Next() )
	{
		if( ValidateObject( cRemove ) )
		{
			cRemove->SetMulti( INVALIDSERIAL, false );
			cRemove->SetZ( GetZ() );
		}
	}
	CItem::Cleanup();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	std::string GetDeed( void ) const
//|					void SetDeed( const std::string &newDeed )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item ID of multi's deed item, for converting multi to/from deed
//o-----------------------------------------------------------------------------------------------o
std::string CMultiObj::GetDeed( void ) const
{
	return deed;
}
void CMultiObj::SetDeed( const std::string &newDeed )
{
	deed = newDeed;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CanBeObjType( ObjectType toCompare ) const
//|	Date		-	24 June, 2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Indicates whether an object can behave as a particular type
//o-----------------------------------------------------------------------------------------------o
bool CMultiObj::CanBeObjType( ObjectType toCompare ) const
{
	bool rvalue = CItem::CanBeObjType( toCompare );
	if( !rvalue )
	{
		if( toCompare == OT_MULTI )
			rvalue = true;
	}
	return rvalue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 GetBanX( void ) const
//|					void SetBanX( SI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the X offset for multi's ban location
//o-----------------------------------------------------------------------------------------------o
SI16 CMultiObj::GetBanX( void ) const
{
	return banX;
}
void CMultiObj::SetBanX( SI16 newValue )
{
	banX		= newValue;
}
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 GetBanY( void ) const
//|					void SetBanY( SI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the Y offset for multi's ban location
//o-----------------------------------------------------------------------------------------------o
SI16 CMultiObj::GetBanY( void ) const
{
	return banY;
}
void CMultiObj::SetBanY( SI16 newValue )
{
	banY		= newValue;
}

GenericList< CChar * > * CMultiObj::GetOwnersOfMultiList( bool clearList )
{
	if( clearList )
	{
		ownersOfMulti.Clear();

		for (auto it = housePrivList.cbegin(), next_it = it; it != housePrivList.cend(); it = next_it)
		{
			++next_it;
			if( it->second == HOUSEPRIV_OWNER )
			{
				ownersOfMulti.Add( static_cast< CChar * >(it->first) );
			}
		}
	}

	return &ownersOfMulti;
}

GenericList< CChar * > * CMultiObj::GetFriendsOfMultiList( bool clearList )
{
	if( clearList )
		friendsOfMulti.Clear();

	for (auto it = housePrivList.cbegin(), next_it = it; it != housePrivList.cend(); it = next_it)
	{
		++next_it;
		if( it->second == HOUSEPRIV_FRIEND )
		{
			friendsOfMulti.Add( static_cast< CChar * >(it->first) );
		}
	}

	return &friendsOfMulti;
}

GenericList< CChar * > * CMultiObj::GetGuestsOfMultiList( bool clearList )
{
	if( clearList )
		guestsOfMulti.Clear();

	for (auto it = housePrivList.cbegin(), next_it = it; it != housePrivList.cend(); it = next_it)
	{
		++next_it;
		if( it->second == HOUSEPRIV_GUEST )
		{
			guestsOfMulti.Add( static_cast< CChar * >(it->first) );
		}
	}

	return &guestsOfMulti;
}

GenericList< CChar * > * CMultiObj::GetBannedFromMultiList( bool clearList )
{
	if( clearList )
		bannedFromMulti.Clear();

	for (auto it = housePrivList.cbegin(), next_it = it; it != housePrivList.cend(); it = next_it)
	{
		++next_it;
		if( it->second == HOUSEPRIV_BANNED )
		{
			bannedFromMulti.Add( static_cast< CChar * >(it->first) );
		}
	}

	return &bannedFromMulti;
}

GenericList< CChar * > * CMultiObj::GetCharsInMultiList( void )
{
	return &charInMulti;
}

GenericList< CItem * > * CMultiObj::GetItemsInMultiList( void )
{
	return &itemInMulti;
}

//o-----------------------------------------------------------------------------------------------o
//|	Class		-	CBoatObj
//|	Date		-	25th September, 2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Version History
//|
//|					1.0		 		25th September, 2003
//|					Very basic, currently
//|					Simplifies some processes in cBoat class, eventually should allow
//|					easy removal of some generic containers in CItem class
//|
//|					1.1		 		28th October, 2003
//|					Added MoveType() to remove the need for CItem::Type2()
//|					Added MoveTime() instead of using CItem::GateTime()
//|					Changed Plank1() and Plank2() to Plank() using an array
//o-----------------------------------------------------------------------------------------------o

const SERIAL		DEFBOAT_TILLER		= INVALIDSERIAL;
const SERIAL		DEFBOAT_HOLD		= INVALIDSERIAL;
const SI08			DEFBOAT_MOVETYPE	= 0;
const TIMERVAL		DEFBOAT_MOVETIME	= 0;

CBoatObj::CBoatObj() : CMultiObj(), tiller( DEFBOAT_TILLER ), hold( DEFBOAT_HOLD ),
moveType( DEFBOAT_MOVETYPE ), nextMoveTime( DEFBOAT_MOVETIME )
{
	planks[0] = planks[1] = INVALIDSERIAL;
	objType = OT_BOAT;
}

CBoatObj::~CBoatObj()
{
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool DumpHeader( std::ofstream &outStream ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Dumps out the header for the CMultiObj
//o-----------------------------------------------------------------------------------------------o
bool CBoatObj::DumpHeader( std::ofstream &outStream ) const
{
	outStream << "[BOAT]" << '\n';
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool DumpBody( std::ofstream &outStream ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Writes out all CMultiObj specific information to a world
//|					file.  Also prints out the Item and BaseObject portions
//o-----------------------------------------------------------------------------------------------o
bool CBoatObj::DumpBody( std::ofstream &outStream ) const
{
	CMultiObj::DumpBody( outStream );

	// Hexadecimal Values
	outStream << std::hex;
	outStream << "Hold=" << "0x" << hold << '\n';
	outStream << "Planks=" << "0x" << planks[0] << ",0x" << planks[1] << '\n';
	outStream << "Tiller=" << "0x" << tiller << std::dec << '\n';

	// Decimal / String Values
	outStream << std::dec;
	return true;
}


//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool HandleLine( std::string &UTag,std::string &data )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Processes a tag/data pair if it can.  If it can, returns
//|					true.  Otherwise, returns false.
//o-----------------------------------------------------------------------------------------------o
bool CBoatObj::HandleLine( std::string &UTag,std::string &data )
{
	bool rvalue = CMultiObj::HandleLine( UTag, data );
	if( !rvalue )
	{
		auto csecs = strutil::sections( data, "," );
		switch( (UTag.data()[0]) )
		{
			case 'M':
				if( UTag == "MOVETYPE" )
				{
					rvalue = true;
				}
				break;
			case 'H':
				if( UTag == "HOLD" )
				{
					SetHold( static_cast<UI32>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				break;
			case 'P':
				if( UTag == "PLANKS" )
				{
					SetPlank( 0, static_cast<UI32>(std::stoul(strutil::stripTrim( csecs[0] ), nullptr, 0)) );
					SetPlank( 1, static_cast<UI32>(std::stoul(strutil::stripTrim( csecs[1] ), nullptr, 0)) );
					rvalue = true;
				}
				break;
			case 'T':
				if( UTag == "TILLER" )
				{
					SetTiller( strutil::value<std::uint32_t>(data) );
					rvalue = true;
				}
				break;
		}
	}
	return rvalue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SERIAL GetTiller( void ) const
//|					void SetTiller( SERIAL newVal )
//|	Date		-	September 25, 2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets boat's Tiller item reference
//o-----------------------------------------------------------------------------------------------o
SERIAL CBoatObj::GetTiller( void ) const
{
	return tiller;
}
void CBoatObj::SetTiller( SERIAL newVal )
{
	tiller = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SERIAL GetPlank( UI08 plankNum ) const
//|					void SetPlank( UI08 plankNum, SERIAL newVal )
//|	Date		-	September 25, 2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets boat's Plank item reference
//o-----------------------------------------------------------------------------------------------o
SERIAL CBoatObj::GetPlank( UI08 plankNum ) const
{
	SERIAL rvalue = INVALIDSERIAL;
	if( plankNum < 2 )
		rvalue = planks[plankNum];
	return rvalue;
}
void CBoatObj::SetPlank( UI08 plankNum, SERIAL newVal )
{
	if( plankNum < 2 )
		planks[plankNum] = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SERIAL GetHold( void ) const
//|					void SetHold( SERIAL newVal )
//|	Date		-	September 25, 2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets boat's Hold item reference
//o-----------------------------------------------------------------------------------------------o
SERIAL CBoatObj::GetHold( void ) const
{
	return hold;
}
void CBoatObj::SetHold( SERIAL newVal )
{
	hold = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 GetMoveType( void ) const
//|					void SetMoveType( SI08 newVal )
//|	Date		-	October 28, 2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets boat's movement type. See BoatMoveType enum for full list
//o-----------------------------------------------------------------------------------------------o
SI08 CBoatObj::GetMoveType( void ) const
{
	return moveType;
}
void CBoatObj::SetMoveType( SI08 newVal )
{
	moveType = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	TIMERVAL GetMoveTime( void ) const
//|					void SetMoveTime( TIMERVAL newVal )
//|	Date		-	October 28, 2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets time for when boat will move next
//o-----------------------------------------------------------------------------------------------o
TIMERVAL CBoatObj::GetMoveTime( void ) const
{
	return nextMoveTime;
}
void CBoatObj::SetMoveTime( TIMERVAL newVal )
{
	nextMoveTime = newVal;
}
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CanBeObjType( ObjectType toCompare ) const
//|	Date		-	24 June, 2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Indicates whether an object can behave as a particular type
//o-----------------------------------------------------------------------------------------------o
bool CBoatObj::CanBeObjType( ObjectType toCompare ) const
{
	bool rvalue = CMultiObj::CanBeObjType( toCompare );
	if( !rvalue )
	{
		if( toCompare == OT_BOAT )
			rvalue = true;
	}
	return rvalue;
}
