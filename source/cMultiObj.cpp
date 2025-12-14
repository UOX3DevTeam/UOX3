//o------------------------------------------------------------------------------------------------o
//|	File		-	cMultiObj.cpp
//|	Date		-	28th July, 2000
//o------------------------------------------------------------------------------------------------o
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
//o------------------------------------------------------------------------------------------------o
#include "uox3.h"
#include "mapstuff.h"
#include "osunique.hpp"
#include <classes.h>
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
const time_t DEFMULTI_TRADETIMESTAMP = static_cast<time_t>( -1 );

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

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::AddToBanList()
//|	Date		-	28th July, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a character to the ban list
//o------------------------------------------------------------------------------------------------o
void CMultiObj::AddToBanList( CChar *toBan )
{
	// Restrictions handled through JS
	housePrivList[toBan] = HOUSEPRIV_BANNED;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::RemoveFromBanList()
//|	Date		-	28th July, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes a character from a ban list
//o------------------------------------------------------------------------------------------------o
void CMultiObj::RemoveFromBanList( CChar *toRemove )
{
	std::map<CChar *, UI08>::iterator rIter = housePrivList.find( toRemove );
	if( rIter != housePrivList.end() )
	{
		if( rIter->second == HOUSEPRIV_BANNED )
		{
			housePrivList.erase( rIter );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::ClearBanList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Clears ban list
//o------------------------------------------------------------------------------------------------o
void CMultiObj::ClearBanList( void )
{
	for( auto it = housePrivList.cbegin(), next_it = it; it != housePrivList.cend(); it = next_it )
	{
		++next_it;
		if( it->second == HOUSEPRIV_BANNED )
		{
			housePrivList.erase( it );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::AddAsOwner()
//|	Date		-	28th July, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a player as an owner
//o------------------------------------------------------------------------------------------------o
void CMultiObj::AddAsOwner( CChar *newOwner )
{
	housePrivList[newOwner] = HOUSEPRIV_OWNER;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::RemoveAsOwner()
//|	Date		-	28th July, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes a character from the owner's list
//o------------------------------------------------------------------------------------------------o
void CMultiObj::RemoveAsOwner( CChar *toRemove )
{
	std::map<CChar *, UI08>::iterator rIter = housePrivList.find( toRemove );
	if( rIter != housePrivList.end() )
	{
		if( rIter->second == HOUSEPRIV_OWNER )
		{
			housePrivList.erase( rIter );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::ClearOwnerList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Clears co-owner list
//o------------------------------------------------------------------------------------------------o
void CMultiObj::ClearOwnerList( void )
{
	for( auto it = housePrivList.cbegin(), next_it = it; it != housePrivList.cend(); it = next_it )
	{
		++next_it;
		if( it->second == HOUSEPRIV_OWNER )
		{
			if( owner != it->first->GetSerial() ) // Don't clear primary house owner from list!
			{
				housePrivList.erase( it );
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::AddAsFriend()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a player as a friend
//o------------------------------------------------------------------------------------------------o
void CMultiObj::AddAsFriend( CChar *newFriend )
{
	housePrivList[newFriend] = HOUSEPRIV_FRIEND;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::RemoveAsFriend()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes a character from the friend list
//o------------------------------------------------------------------------------------------------o
void CMultiObj::RemoveAsFriend( CChar *toRemove )
{
	std::map<CChar *, UI08>::iterator rIter = housePrivList.find( toRemove );
	if( rIter != housePrivList.end() )
	{
		if( rIter->second == HOUSEPRIV_FRIEND )
		{
			housePrivList.erase( rIter );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::ClearFriendList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Clears friend list
//o------------------------------------------------------------------------------------------------o
void CMultiObj::ClearFriendList( void )
{
	for( auto it = housePrivList.cbegin(), next_it = it; it != housePrivList.cend(); it = next_it )
	{
		++next_it;
		if( it->second == HOUSEPRIV_FRIEND )
		{
			housePrivList.erase( it );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::AddAsGuest()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a player as a guest
//o------------------------------------------------------------------------------------------------o
void CMultiObj::AddAsGuest( CChar *newGuest )
{
	housePrivList[newGuest] = HOUSEPRIV_GUEST;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::RemoveAsGuest()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes a character from the guest list
//o------------------------------------------------------------------------------------------------o
void CMultiObj::RemoveAsGuest( CChar *toRemove )
{
	std::map<CChar *, UI08>::iterator rIter = housePrivList.find( toRemove );
	if( rIter != housePrivList.end() )
	{
		if( rIter->second == HOUSEPRIV_GUEST )
		{
			housePrivList.erase( rIter );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::ClearGuestList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Clears guest list
//o------------------------------------------------------------------------------------------------o
void CMultiObj::ClearGuestList( void )
{
	for( auto it = housePrivList.cbegin(), next_it = it; it != housePrivList.cend(); it = next_it )
	{
		++next_it;
		if( it->second == HOUSEPRIV_GUEST )
		{
			housePrivList.erase( it );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::IsOnBanList()
//|	Date		-	28th July, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if a character is on the ban list
//o------------------------------------------------------------------------------------------------o
bool CMultiObj::IsOnBanList( CChar *toBan ) const
{
	std::map<CChar *, UI08>::const_iterator bIter = housePrivList.find( toBan );
	if( bIter != housePrivList.end() )
	{
		if( bIter->second == HOUSEPRIV_BANNED )
			return true;
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::IsOnFriendList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if a character is on the friend list
//o------------------------------------------------------------------------------------------------o
bool CMultiObj::IsOnFriendList( CChar *toCheck ) const
{
	std::map<CChar *, UI08>::const_iterator bIter = housePrivList.find( toCheck );
	if( bIter != housePrivList.end() )
	{
		if( bIter->second == HOUSEPRIV_FRIEND )
			return true;
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::IsOnGuestList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if a character is on the guest list
//o------------------------------------------------------------------------------------------------o
bool CMultiObj::IsOnGuestList( CChar *toCheck ) const
{
	std::map<CChar *, UI08>::const_iterator bIter = housePrivList.find( toCheck );
	if( bIter != housePrivList.end() )
	{
		if( bIter->second == HOUSEPRIV_GUEST )
			return true;
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::IsOnOwnerList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if the character is the owner or a co-owner
//o------------------------------------------------------------------------------------------------o
bool CMultiObj::IsOnOwnerList( CChar *toFind ) const
{
	std::map<CChar *, UI08>::const_iterator oIter = housePrivList.find( toFind );
	if( oIter != housePrivList.end() )
	{
		if( oIter->second == HOUSEPRIV_OWNER )
			return true;
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::CheckForAccountCoOwnership()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if the player has another character on same account that co-owns house
//o------------------------------------------------------------------------------------------------o
bool CMultiObj::CheckForAccountCoOwnership( CChar *mChar ) const
{
	UI16 accountNum = mChar->GetAccountNum();
	std::map<CChar *, UI08>::const_iterator oIter;
	for( oIter = housePrivList.begin(); oIter != housePrivList.end(); ++oIter )
	{
		if( ValidateObject( oIter->first ))
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

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::IsOwner()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if the character is the actual owner of the house, and not a co-owner
//o------------------------------------------------------------------------------------------------o
bool CMultiObj::IsOwner( CChar *toFind ) const
{
	return ( owner == toFind->GetSerial() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::AddToMulti()
//|	Date		-	28th July, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds an item or character to the multi
//o------------------------------------------------------------------------------------------------o
void CMultiObj::AddToMulti( CBaseObject *toAdd )
{
	if( toAdd->GetObjType() == OT_CHAR )
	{
		charInMulti.Add( static_cast<CChar *>( toAdd ));
	}
	else
	{
		itemInMulti.Add( static_cast<CItem *>( toAdd ));
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::RemoveFromMulti()
//|	Date		-	28th July, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes an item or character from the multi, if possible
//o------------------------------------------------------------------------------------------------o
void CMultiObj::RemoveFromMulti( CBaseObject *toRemove )
{
	if( toRemove->GetObjType() == OT_CHAR )
	{
		charInMulti.Remove( static_cast<CChar *>( toRemove ));
		if( CanBeObjType( OT_BOAT ) && charInMulti.Num() == 0 )
		{
			if(( static_cast<CBoatObj *>( this ))->GetMoveType() != -1 )
			{
				( static_cast<CBoatObj *>( this ))->SetMoveType( 0 );
			}
		}
	}
	else
	{
		if( static_cast<CItem *>( toRemove )->GetTempVar( CITV_MORE ) != serial ) // Prevent doors and signs from being accidentally removed from house!
		{
			itemInMulti.Remove( static_cast<CItem *>( toRemove ));
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::GetLockdownCount()
//|	Date		-	28th July, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a count of the locked down items
//o------------------------------------------------------------------------------------------------o
size_t CMultiObj::GetLockdownCount( void ) const
{
	return lockedList.size();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::GetMaxLockdowns()
//|	Date		-	28th July, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the maximum number of possible lockdowns
//o------------------------------------------------------------------------------------------------o
UI16 CMultiObj::GetMaxLockdowns( void ) const
{
	return maxLockdowns;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::GetTrashContainerCount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a count of the locked down items
//o------------------------------------------------------------------------------------------------o
size_t CMultiObj::GetTrashContainerCount( void ) const
{
	return trashContainerList.size();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::GetMaxTrashContainers()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the maximum number of possible trash containers
//o------------------------------------------------------------------------------------------------o
UI16 CMultiObj::GetMaxTrashContainers( void ) const
{
	return maxTrashContainers;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::GetVendorCount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a count of the player vendors in the multi
//o------------------------------------------------------------------------------------------------o
size_t CMultiObj::GetVendorCount( void ) const
{
	return vendorList.size();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::GetMaxVendors()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes a player vendor from the multi
//o------------------------------------------------------------------------------------------------o
UI16 CMultiObj::GetMaxVendors( void ) const
{
	return maxVendors;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::GetFriendCount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a count of the amount of friends of multi
//o------------------------------------------------------------------------------------------------o
UI16 CMultiObj::GetFriendCount( void )
{
	UI16 friendCount = 0;
	for( std::map<CChar *, UI08>::iterator rIter = housePrivList.begin(); rIter != housePrivList.end(); ++rIter )
	{
		if( rIter->second == HOUSEPRIV_FRIEND )
		{
			friendCount++;
		}
	}
	return friendCount;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::GetMaxFriends()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the maximum number of friends multi can have
//o------------------------------------------------------------------------------------------------o
UI16 CMultiObj::GetMaxFriends( void ) const
{
	return maxFriends;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::GetGuestCount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a count of the amount of guests of multi
//o------------------------------------------------------------------------------------------------o
UI16 CMultiObj::GetGuestCount( void )
{
	UI16 guestCount = 0;
	for( std::map<CChar *, UI08>::iterator rIter = housePrivList.begin(); rIter != housePrivList.end(); ++rIter )
	{
		if( rIter->second == HOUSEPRIV_GUEST )
		{
			guestCount++;
		}
	}
	return guestCount;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::GetMaxGuests( void )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the maximum number of guests multi can have
//o------------------------------------------------------------------------------------------------o
UI16 CMultiObj::GetMaxGuests( void ) const
{
	return maxGuests;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::GetBanCount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a count of the amount of banned players of multi
//o------------------------------------------------------------------------------------------------o
UI16 CMultiObj::GetBanCount( void )
{
	UI16 banCount = 0;
	for( std::map<CChar *, UI08>::iterator rIter = housePrivList.begin(); rIter != housePrivList.end(); ++rIter )
	{
		if( rIter->second == HOUSEPRIV_BANNED )
		{
			banCount++;
		}
	}
	return banCount;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::GetMaxBans()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the maximum number of banned players multi can have
//o------------------------------------------------------------------------------------------------o
UI16 CMultiObj::GetMaxBans( void ) const
{
	return maxBans;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::GetOwnerCount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a count of the amount of co-owners of multi
//o------------------------------------------------------------------------------------------------o
UI16 CMultiObj::GetOwnerCount( void )
{
	UI16 ownerCount = 0;
	for( std::map<CChar *, UI08>::iterator rIter = housePrivList.begin(); rIter != housePrivList.end(); ++rIter )
	{
		if( rIter->second == HOUSEPRIV_OWNER )
		{
			// Don't count primary house owner as co-owner
			if( rIter->first->GetSerial() != owner )
			{
				ownerCount++;
			}
		}
	}
	return ownerCount;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::GetMaxOwners()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the maximum number of co-owners multi can have
//o------------------------------------------------------------------------------------------------o
UI16 CMultiObj::GetMaxOwners( void ) const
{
	return maxOwners;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::LockDownItem()
//|	Date		-	28th July, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Locks down an item toLock
//o------------------------------------------------------------------------------------------------o
auto CMultiObj::LockDownItem( CItem *toLock ) -> void
{
	if( lockedList.size() < maxLockdowns )
	{
		auto iter = std::find_if( lockedList.begin(), lockedList.end(), [toLock]( CItem *entry )
		{
			return entry == toLock;
		});
		if( iter == lockedList.end() )
		{
			toLock->LockDown( this );
			toLock->Dirty( UT_UPDATE );
			lockedList.push_back( toLock );

		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::ReleaseItem()
//|	Date		-	28th July, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Unlocks a locked down item
//o------------------------------------------------------------------------------------------------o
auto CMultiObj::ReleaseItem( CItem *toRemove ) -> void
{
	auto iter = std::find_if( lockedList.begin(), lockedList.end(), [toRemove]( const CItem *entry )
	{
		return toRemove == entry;
	});
	if( iter != lockedList.end() )
	{
		toRemove->Dirty( UT_UPDATE );
		lockedList.erase( iter );
		toRemove->SetMovable( 1 ); // Default to "always movable" for released items
		toRemove->SetDecayTime( cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_DECAYINHOUSE ));
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::AddTrashContainer()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a trash container to the multi's trash container list
//o------------------------------------------------------------------------------------------------o
void CMultiObj::AddTrashContainer( CItem *toAdd )
{
	if( trashContainerList.size() < maxTrashContainers )
	{
		auto iter = std::find_if( trashContainerList.begin(), trashContainerList.end(), [toAdd]( CItem *entry )
		{
			return entry == toAdd;
		});
		if( iter == trashContainerList.end() )
		{
			trashContainerList.push_back( toAdd );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::RemoveTrashContainer()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Unlocks a locked down item
//o------------------------------------------------------------------------------------------------o
auto CMultiObj::RemoveTrashContainer( CItem *toRemove ) -> void
{
	auto iter = std::find_if( trashContainerList.begin(), trashContainerList.end(), [toRemove]( CItem *entry )
	{
		return toRemove == entry;
	});
	if( iter != trashContainerList.end() )
	{
		trashContainerList.erase( iter );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::AddVendor()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a player vendor to the multi
//o------------------------------------------------------------------------------------------------o
void CMultiObj::AddVendor( CChar *toAdd )
{
	if( vendorList.size() < maxVendors )
	{
		auto iter = std::find_if( vendorList.begin(), vendorList.end(), [toAdd]( CChar *entry )
		{
			return entry == toAdd;
		});
		if( iter == vendorList.end() ) // Wasnt found, add it
		{
			toAdd->Dirty( UT_UPDATE );
			vendorList.push_back( toAdd );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::RemoveVendor()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes a player vendor from the multi
//o------------------------------------------------------------------------------------------------o
void CMultiObj::RemoveVendor( CChar *toRemove )
{
	auto iter = std::find_if( vendorList.begin(), vendorList.end(), [toRemove]( CChar *entry )
	{
		return toRemove == entry;
	});
	if( iter != vendorList.end() )
	{
		toRemove->Dirty( UT_UPDATE );
		vendorList.erase(iter);
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::GetPublicStatus()
//|	Function	-	CMultiObj::SetPublicStatus()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the public status of the multi - either true (public) or false (private)
//o------------------------------------------------------------------------------------------------o
bool CMultiObj::GetPublicStatus( void ) const
{
	return isPublic;
}
void CMultiObj::SetPublicStatus( bool newBool )
{
	isPublic = newBool;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::GetBuildTimestamp()
//|	Function	-	CMultiObj::SetBuildTimestamp()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the timestamp for when the multi was built
//o------------------------------------------------------------------------------------------------o
std::string CMultiObj::GetBuildTimestamp( void ) const
{
	if( buildTimestamp > 0 )
	{
		char tempTimestamp[100];
		struct tm curtime;
		lcltime( buildTimestamp, curtime );
		strftime( tempTimestamp, 50, "%F at %T", &curtime );
		return tempTimestamp;
	}
	else
		return "Unknown";
}
void CMultiObj::SetBuildTimestamp( time_t newTime )
{
	if( newTime && newTime > 0 )
	{
		buildTimestamp = newTime;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::GetTradeTimestamp()
//|	Function	-	CMultiObj::SetTradeTimestamp()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the timestamp for when the multi was last traded between players
//o------------------------------------------------------------------------------------------------o
std::string CMultiObj::GetTradeTimestamp( void ) const
{
	if( tradeTimestamp > 0 )
	{
		char tempTimestamp[100];
		struct tm curtime;
		lcltime( tradeTimestamp, curtime );
		strftime( tempTimestamp, 50, "%F at %T", &curtime );
		return tempTimestamp;
	}
	else
		return "Never";
}
void CMultiObj::SetTradeTimestamp( time_t newTime )
{
	tradeTimestamp = newTime;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::GetSecureContainerCount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a count of the amount of secure containers in the multi
//o------------------------------------------------------------------------------------------------o
size_t CMultiObj::GetSecureContainerCount( void ) const
{
	return secureContainerList.size();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::GetMaxSecureContainers()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the maximum number of possible secure containers for a house
//o------------------------------------------------------------------------------------------------o
UI16 CMultiObj::GetMaxSecureContainers( void ) const
{
	return maxSecureContainers;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::IsSecureContainer()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if item is a secure container
//o------------------------------------------------------------------------------------------------o
auto CMultiObj::IsSecureContainer( CItem *toCheck ) -> bool
{
	auto rValue = false;
	auto iter = std::find_if( secureContainerList.begin(), secureContainerList.end(), [toCheck]( CItem *entry )
	{
		return toCheck == entry;
	});
	if( iter != secureContainerList.end() )
	{
		rValue = true;
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::SecureContainer()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Secures specified container
//o------------------------------------------------------------------------------------------------o
void CMultiObj::SecureContainer( CItem *toSecure )
{
	if( secureContainerList.size() < maxSecureContainers )
	{
		auto iter = std::find_if( secureContainerList.begin(), secureContainerList.end(), [toSecure]( CItem *entry )
		{
			return entry == toSecure;
		});
		if( iter == secureContainerList.end() )
		{
			secureContainerList.push_back( toSecure );
			if( toSecure->GetType() != 87 ) // Don't lock down trash containers
			{
				toSecure->LockDown();
			}
			toSecure->Dirty( UT_UPDATE );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::UnsecureContainer()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Unsecures a secured container
//o------------------------------------------------------------------------------------------------o
auto CMultiObj::UnsecureContainer( CItem *toUnsecure ) -> void
{
	auto iter = std::find_if( secureContainerList.begin(), secureContainerList.end(), [toUnsecure]( CItem *entry )
	{
		return toUnsecure == entry;
	});
	if( iter != secureContainerList.end() )
	{
		toUnsecure->Dirty( UT_UPDATE );
		secureContainerList.erase( iter );
		if( toUnsecure->GetType() != 87 ) // Trash container
		{
			toUnsecure->SetMovable( 1 );
			toUnsecure->SetDecayTime( cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_DECAYINHOUSE ));
		}

	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::Save()
//|	Date		-	28th July, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Saves a multi out to disk. outStream is the file to write to
//o------------------------------------------------------------------------------------------------o
bool CMultiObj::Save( std::ostream &outStream )
{
	bool rValue = false;
	if( !IsFree() )
	{
		rValue = true;
		auto [mapWidth, mapHeight] = Map->SizeOfMap( worldNumber );
		if( GetCont() != nullptr || ( GetX() > 0 && GetX() < mapWidth && GetY() < mapHeight ))
		{
			DumpHeader( outStream );
			DumpBody( outStream );
			DumpFooter( outStream );
		}
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::LoadRemnants()
//|	Date		-	20th January, 2002
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	After handling data specific load, other parts go here
//o------------------------------------------------------------------------------------------------o
bool CMultiObj::LoadRemnants( void )
{
	bool rValue = CItem::LoadRemnants();

	// Add item weight if item doesn't have it yet
	if( GetWeight() <= 0 )
	{
		SetWeight( 0 );
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::SetMaxLockdowns()
//|	Date		-	28th August, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the maximum number of lockdowns possible
//o------------------------------------------------------------------------------------------------o
void CMultiObj::SetMaxLockdowns( UI16 newValue )
{
	maxLockdowns = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::SetMaxTrashContainers()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the maximum number of trash containers possible
//o------------------------------------------------------------------------------------------------o
void CMultiObj::SetMaxTrashContainers( UI16 newValue )
{
	maxTrashContainers = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::SetMaxVendors()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the maximum number of player vendors possible
//o------------------------------------------------------------------------------------------------o
void CMultiObj::SetMaxVendors( UI16 newValue )
{
	maxVendors = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::SetMaxSecureContainers()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the maximum number of secure containers possible
//o------------------------------------------------------------------------------------------------o
void CMultiObj::SetMaxSecureContainers( UI16 newValue )
{
	maxSecureContainers = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::SetMaxBans()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the maximum number of banned players possible
//o------------------------------------------------------------------------------------------------o
void CMultiObj::SetMaxBans( UI16 newValue )
{
	maxBans = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::SetMaxFriends()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the maximum number of friends possible
//o------------------------------------------------------------------------------------------------o
void CMultiObj::SetMaxFriends( UI16 newValue )
{
	maxFriends = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::SetMaxGuests()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the maximum number of guests possible
//o------------------------------------------------------------------------------------------------o
void CMultiObj::SetMaxGuests( UI16 newValue )
{
	maxGuests = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::SetMaxOwners()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the maximum number of co-owners possible
//o------------------------------------------------------------------------------------------------o
void CMultiObj::SetMaxOwners( UI16 newValue )
{
	maxOwners = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::DumpHeader()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Dumps out the header for the CMultiObj
//o------------------------------------------------------------------------------------------------o
bool CMultiObj::DumpHeader( std::ostream &outStream ) const
{
	outStream << "[HOUSE]" << '\n';
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::DumpBody()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Writes out all CMultiObj specific information to a world
//|					file.  Also prints out the Item and BaseObject portions
//o------------------------------------------------------------------------------------------------o
bool CMultiObj::DumpBody( std::ostream &outStream ) const
{
	CItem::DumpBody( outStream );

	const char newLine = '\n';

	// Hexadecimal Values
	outStream << std::hex;

	// Decimal / String Values
	outStream << std::dec;
	std::map<CChar *, UI08>::const_iterator oIter;
	for( oIter = housePrivList.begin(); oIter != housePrivList.end(); ++oIter )
	{
		if( ValidateObject( oIter->first ))
		{
			if( oIter->second == HOUSEPRIV_OWNER )
			{
				outStream << "Owner=";
			}
			else if( oIter->second == HOUSEPRIV_BANNED )
			{
				outStream << "Banned=";
			}
			else if( oIter->second == HOUSEPRIV_FRIEND )
			{
				outStream << "Friend=";
			}
			else if( oIter->second == HOUSEPRIV_GUEST )
			{
				outStream << "Guest=";
			}
			else
			{
				continue;
			}
			outStream << std::to_string( oIter->first->GetSerial() ) + newLine;
		}
	}
	outStream << "IsPublic=" + std::to_string(( isPublic ? 1 : 0 )) + newLine;
	outStream << "MaxBans=" + std::to_string( maxBans ) + newLine;
	outStream << "MaxFriends=" + std::to_string( maxFriends ) + newLine;
	outStream << "MaxGuests=" + std::to_string( maxGuests ) + newLine;

#if _MSC_VER
	std::for_each( lockedList.begin(), lockedList.end(), [&outStream, newLine]( CItem *entry )
#else
	std::for_each( lockedList.begin(), lockedList.end(), [&outStream]( CItem *entry )
#endif
	{
		if( ValidateObject( entry ))
		{
			outStream << "LockedItem=" + std::to_string( entry->GetSerial() ) + newLine;
		}
	});

#if _MSC_VER
	std::for_each( secureContainerList.begin(), secureContainerList.end(), [&outStream, newLine]( CItem *entry )
#else
	std::for_each( secureContainerList.begin(), secureContainerList.end(), [&outStream]( CItem *entry )
#endif
	{
		if( ValidateObject( entry ))
		{
			outStream << "SecureContainer=" + std::to_string( entry->GetSerial() ) + newLine;
		}

	});

	outStream << "MaxLockdowns=" + std::to_string( maxLockdowns ) + newLine;
	outStream << "MaxSecureContainers=" + std::to_string( maxSecureContainers ) + newLine;

	for( auto &vendor:vendorList )
	{
		if( ValidateObject( vendor ))
		{
			outStream << "Vendor=" + std::to_string( vendor->GetSerial() ) + newLine;
		}
	}

	outStream << "MaxVendors=" + std::to_string( maxVendors ) + newLine;

	// Add deedname= to the save :) We need a way to preserve the name of the item
	outStream << "DeedName=" + deed + newLine;
	outStream << "BuildTime=" + std::to_string( buildTimestamp ) + newLine;
	outStream << "TradeTime=" + std::to_string( tradeTimestamp ) + newLine;

	return true;
}


//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::HandleLine()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Processes a tag/data pair if it can.  If it can, returns
//|					true.  Otherwise, returns false.
//o------------------------------------------------------------------------------------------------o
bool CMultiObj::HandleLine( std::string &UTag, std::string &data )
{
	bool rValue = CItem::HandleLine( UTag, data );
	auto csecs = oldstrutil::sections( data, "," );
	
	if( !rValue )
	{
		switch(( UTag.data()[0] ))
		{
			case 'B':
				if( UTag == "BANNED" )
				{
					CChar *bList = CalcCharObjFromSer( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					if( ValidateObject( bList ))
					{
						AddToBanList( bList );
					}
					rValue = true;
				}
				else if( UTag == "BUILDTIME" )
				{
					time_t buildTime = static_cast<time_t>( std::stoll( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 ));
					if( buildTime > 0 )
					{
						SetBuildTimestamp( buildTime );
					}
					rValue = true;
				}
				break;
			case 'C':
				if( UTag == "COOWNER" ) // Legacy tag for loading older data
				{
					CChar *cList = CalcCharObjFromSer( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					if( ValidateObject( cList ))
					{
						AddAsOwner( cList );
					}
					rValue = true;
				}
				break;
			case 'D':
				if( UTag == "DEEDNAME" )
				{
					SetDeed( data );
					rValue = true;
				}
				break;
			case 'F':
				if( UTag == "FRIEND" )
				{
					CChar *cList = CalcCharObjFromSer( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					if( ValidateObject( cList ))
					{
						AddAsFriend( cList );
					}
					rValue = true;
				}
				break;
			case 'G':
				if( UTag == "GUEST" )
				{
					CChar *cList = CalcCharObjFromSer( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					if( ValidateObject( cList ))
					{
						AddAsGuest( cList );
					}
					rValue = true;
				}
				break;
			case 'I':
				if( UTag == "ISPUBLIC" )
				{
					SetPublicStatus( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )) == 1 );
					rValue = true;
				}
				break;
			case 'L':
				if( UTag == "LOCKEDITEM" )
				{
					CItem *iList = CalcItemObjFromSer( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					if( ValidateObject( iList ))
					{
						LockDownItem( iList );
					}
					rValue = true;
				}
				break;
			case 'M':
				if( UTag == "MAXBANS" )
				{
					maxBans = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 ));
					rValue = true;
				}
				else if( UTag == "MAXFRIENDS" )
				{
					maxFriends = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 ));
					rValue = true;
				}
				else if( UTag == "MAXGUESTS" )
				{
					maxGuests = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 ));
					rValue = true;
				}
				else if( UTag == "MAXLOCKDOWNS" || UTag == "MAXLOCKEDDOWN" )
				{
					maxLockdowns = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 ));
					rValue = true;
				}
				else if( UTag == "MAXOWNERS" )
				{
					maxOwners = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 ));
					rValue = true;
				}
				else if( UTag == "MAXSECURECONTAINERS" )
				{
					maxSecureContainers = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 ));
					rValue = true;
				}
				else if( UTag == "MAXVENDORS" )
				{
					maxVendors = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 ));
					rValue = true;
				}
				break;
			case 'O':
				if( UTag == "OWNER" )
				{
					CChar *cList = CalcCharObjFromSer( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					if( ValidateObject( cList ))
					{
						AddAsOwner( cList );
					}
					rValue = true;
				}
				break;
			case 'S':
				if( UTag == "SECURECONTAINER" )
				{
					CItem *iList = CalcItemObjFromSer( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					if( ValidateObject( iList ))
					{
						SecureContainer( iList );
					}
					rValue = true;
				}
				break;
			case 'T':
				if( UTag == "TRADETIME" )
				{
					time_t tradeTime = static_cast<time_t>( std::stoll( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 ));
					if( tradeTime > 0 )
					{
						SetTradeTimestamp( tradeTime );
					}
					rValue = true;
				}
				break;
			case 'V':
				if( UTag == "VENDOR" )
				{
					CChar *cList = CalcCharObjFromSer( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					if( ValidateObject( cList ))
					{
						AddVendor( cList );
					}
					rValue = true;
				}
				break;
		}
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::SetOwner()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Ensures that newOwner is also on the house ownership list.
//o------------------------------------------------------------------------------------------------o
void CMultiObj::SetOwner( CChar *newOwner )
{
	RemoveSelfFromOwner(); // Remove multi from previous owner's list of owned items
	if( ValidateObject( newOwner ))
	{
		owner = newOwner->GetSerial();
		AddAsOwner( newOwner );
		AddSelfToOwner(); // Add multi to owner's list of owned items
	}
	else
	{
		owner = INVALIDSERIAL;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::Cleanup()
//|	Date		-	26th September, 2001
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Makes sure that any items and chars inside the multi are removed
//o------------------------------------------------------------------------------------------------o
void CMultiObj::Cleanup( void )
{
	for( CItem *iRemove = itemInMulti.First(); !itemInMulti.Finished(); iRemove = itemInMulti.Next() )
	{
		if( ValidateObject( iRemove ))
		{
			ItemTypes iType = iRemove->GetType();
			if( iType == IT_DOOR || iType == IT_LOCKEDDOOR || iType == IT_HOUSESIGN )
			{
				iRemove->Delete();
			}
			else
			{
				if( iRemove->IsLockedDown() )
				{
					iRemove->SetMovable( 1 );
				}
				iRemove->SetMulti( INVALIDSERIAL, false );
				iRemove->SetZ( GetZ() );
			}
		}
	}
	for( CChar *cRemove = charInMulti.First(); !charInMulti.Finished(); cRemove = charInMulti.Next() )
	{
		if( ValidateObject( cRemove ))
		{
			cRemove->SetMulti( INVALIDSERIAL, false );
			cRemove->SetZ( GetZ() );
		}
	}
	CItem::Cleanup();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::GetDeed()
//|					void SetDeed( const std::string &newDeed )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item ID of multi's deed item, for converting multi to/from deed
//o------------------------------------------------------------------------------------------------o
std::string CMultiObj::GetDeed( void ) const
{
	return deed;
}
void CMultiObj::SetDeed( const std::string &newDeed )
{
	deed = newDeed;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::CanBeObjType()
//|	Date		-	24 June, 2004
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Indicates whether an object can behave as a particular type
//o------------------------------------------------------------------------------------------------o
bool CMultiObj::CanBeObjType( ObjectType toCompare ) const
{
	bool rValue = CItem::CanBeObjType( toCompare );
	if( !rValue )
	{
		if( toCompare == OT_MULTI )
		{
			rValue = true;
		}
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::GetBanX()
//|					CMultiObj::SetBanX()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the X offset for multi's ban location
//o------------------------------------------------------------------------------------------------o
SI16 CMultiObj::GetBanX( void ) const
{
	return banX;
}
void CMultiObj::SetBanX( SI16 newValue )
{
	banX = newValue;
}
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::GetBanY()
//|					CMultiObj::SetBanY()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the Y offset for multi's ban location
//o------------------------------------------------------------------------------------------------o
SI16 CMultiObj::GetBanY( void ) const
{
	return banY;
}
void CMultiObj::SetBanY( SI16 newValue )
{
	banY = newValue;
}

GenericList<CChar *> * CMultiObj::GetOwnersOfMultiList( bool clearList )
{
	if( clearList )
	{
		ownersOfMulti.Clear();

		for( auto it = housePrivList.cbegin(), next_it = it; it != housePrivList.cend(); it = next_it )
		{
			++next_it;
			if( it->second == HOUSEPRIV_OWNER )
			{
				ownersOfMulti.Add( static_cast<CChar *>( it->first ));
			}
		}
	}

	return &ownersOfMulti;
}

GenericList<CChar *> * CMultiObj::GetFriendsOfMultiList( bool clearList )
{
	if( clearList )
	{
		friendsOfMulti.Clear();
	}

	for( auto it = housePrivList.cbegin(), next_it = it; it != housePrivList.cend(); it = next_it )
	{
		++next_it;
		if( it->second == HOUSEPRIV_FRIEND )
		{
			friendsOfMulti.Add( static_cast<CChar *>( it->first ));
		}
	}

	return &friendsOfMulti;
}

GenericList<CChar *> * CMultiObj::GetGuestsOfMultiList( bool clearList )
{
	if( clearList )
	{
		guestsOfMulti.Clear();
	}

	for( auto it = housePrivList.cbegin(), next_it = it; it != housePrivList.cend(); it = next_it )
	{
		++next_it;
		if( it->second == HOUSEPRIV_GUEST )
		{
			guestsOfMulti.Add( static_cast<CChar *>( it->first ));
		}
	}

	return &guestsOfMulti;
}

GenericList<CChar *> * CMultiObj::GetBannedFromMultiList( bool clearList )
{
	if( clearList )
	{
		bannedFromMulti.Clear();
	}

	for( auto it = housePrivList.cbegin(), next_it = it; it != housePrivList.cend(); it = next_it )
	{
		++next_it;
		if( it->second == HOUSEPRIV_BANNED )
		{
			bannedFromMulti.Add( static_cast<CChar *>( it->first ));
		}
	}

	return &bannedFromMulti;
}

GenericList<CChar *> * CMultiObj::GetCharsInMultiList( void )
{
	return &charInMulti;
}

GenericList<CItem *> * CMultiObj::GetItemsInMultiList( void )
{
	return &itemInMulti;
}

//o------------------------------------------------------------------------------------------------o
//|	Class		-	CBoatObj
//|	Date		-	25th September, 2003
//o------------------------------------------------------------------------------------------------o
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
//o------------------------------------------------------------------------------------------------o

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

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBoatObj::DumpHeader()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Dumps out the header for the CMultiObj
//o------------------------------------------------------------------------------------------------o
bool CBoatObj::DumpHeader( std::ostream &outStream ) const
{
	outStream << "[BOAT]" << '\n';
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBoatObj::DumpBody()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Writes out all CMultiObj specific information to a world
//|					file.  Also prints out the Item and BaseObject portions
//o------------------------------------------------------------------------------------------------o
bool CBoatObj::DumpBody( std::ostream &outStream ) const
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


//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBoatObj::HandleLine()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Processes a tag/data pair if it can.  If it can, returns
//|					true.  Otherwise, returns false.
//o------------------------------------------------------------------------------------------------o
bool CBoatObj::HandleLine( std::string &UTag, std::string &data )
{
	bool rValue = CMultiObj::HandleLine( UTag, data );
	if( !rValue )
	{
		auto csecs = oldstrutil::sections( data, "," );
		switch(( UTag.data()[0] ))
		{
			case 'M':
				if( UTag == "MOVETYPE" )
				{
					rValue = true;
				}
				break;
			case 'H':
				if( UTag == "HOLD" )
				{
					SetHold( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				break;
			case 'P':
				if( UTag == "PLANKS" )
				{
					SetPlank( 0, static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 )));
					SetPlank( 1, static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 )));
					rValue = true;
				}
				break;
			case 'T':
				if( UTag == "TILLER" )
				{
					SetTiller( oldstrutil::value<UI32>( data ));
					rValue = true;
				}
				break;
		}
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBoatObj::GetTiller()
//|					CBoatObj::SetTiller()
//|	Date		-	September 25, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets boat's Tiller item reference
//o------------------------------------------------------------------------------------------------o
SERIAL CBoatObj::GetTiller( void ) const
{
	return tiller;
}
void CBoatObj::SetTiller( SERIAL newVal )
{
	tiller = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBoatObj::GetPlank()
//|					CBoatObj::SetPlank()
//|	Date		-	September 25, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets boat's Plank item reference
//o------------------------------------------------------------------------------------------------o
SERIAL CBoatObj::GetPlank( UI08 plankNum ) const
{
	SERIAL rValue = INVALIDSERIAL;
	if( plankNum < 2 )
	{
		rValue = planks[plankNum];
	}
	return rValue;
}
void CBoatObj::SetPlank( UI08 plankNum, SERIAL newVal )
{
	if( plankNum < 2 )
	{
		planks[plankNum] = newVal;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBoatObj::GetHold()
//|					CBoatObj::SetHold()
//|	Date		-	September 25, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets boat's Hold item reference
//o------------------------------------------------------------------------------------------------o
SERIAL CBoatObj::GetHold( void ) const
{
	return hold;
}
void CBoatObj::SetHold( SERIAL newVal )
{
	hold = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBoatObj::GetMoveType()
//|					CBoatObj::SetMoveType()
//|	Date		-	October 28, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets boat's movement type. See BoatMoveType enum for full list
//o------------------------------------------------------------------------------------------------o
SI08 CBoatObj::GetMoveType( void ) const
{
	return moveType;
}
void CBoatObj::SetMoveType( SI08 newVal )
{
	moveType = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBoatObj::GetMoveTime()
//|					CBoatObj::SetMoveTime()
//|	Date		-	October 28, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets time for when boat will move next
//o------------------------------------------------------------------------------------------------o
TIMERVAL CBoatObj::GetMoveTime( void ) const
{
	return nextMoveTime;
}
void CBoatObj::SetMoveTime( TIMERVAL newVal )
{
	nextMoveTime = newVal;
}
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBoatObj::CanBeObjType()
//|	Date		-	24 June, 2004
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Indicates whether an object can behave as a particular type
//o------------------------------------------------------------------------------------------------o
bool CBoatObj::CanBeObjType( ObjectType toCompare ) const
{
	bool rValue = CMultiObj::CanBeObjType( toCompare );
	if( !rValue )
	{
		if( toCompare == OT_BOAT )
		{
			rValue = true;
		}
	}
	return rValue;
}

static std::unordered_map<SERIAL, HouseCustomSession> g_houseCustomSessions;
static const SI08 DESIGN_BASE_Z = 7;

bool HC_StartSession( CSocket *sock, SERIAL houseSerial )
{
    if( sock == nullptr )
        return false;

    CChar *chr = sock->CurrcharObj();
    if( chr == nullptr )
        return false;

    CItem *houseItem = CalcItemObjFromSer( houseSerial );
    if( !ValidateObject( houseItem ) )
        return false;

    CMultiObj *mMulti = FindMulti( houseItem );
    if( !ValidateObject( mMulti ) )
        return false;

    HouseCustomSession s;
    s.houseSerial = houseSerial;
    s.revision    = 1;
	s.clientLevel = 1;
    s.floor       = 0;

    s.baseTiles.clear();
    s.tiles.clear();
    s.originalTiles.clear();
    s.backupTiles.clear();

    // 1) Seed foundation tiles into baseTiles
    HC_LoadFoundationTiles( sock, s );

    // 2) Load already committed custom items into tiles
    HC_LoadExistingCustomTiles( s, houseItem, mMulti );

    // 3) Snapshot for Revert / Backup (custom tiles only)
    s.originalTiles = s.tiles;
    s.backupTiles   = s.tiles;

    g_houseCustomSessions[ chr->GetSerial() ] = s;
    return true;
}

bool HC_LoadFoundationTiles( CSocket* sock, HouseCustomSession& s )
{
    CItem* houseItem = CalcItemObjFromSer( s.houseSerial );
    if( !ValidateObject( houseItem ))
        return false;

    CMultiObj* mMulti = FindMulti( houseItem );
    if( !ValidateObject( mMulti ))
        return false;

    // ----------------------------------------------------------------
    // Determine plot width/height and center offsets.
    // You said mapstuff shows how to get x/y of a multi - use that same
    // approach to compute bounds/size for the foundation.
    // ----------------------------------------------------------------

    // UOX3 multis are ITEMID_MULTI (0x4000) + multiIndex
    const UI16 multiNum = static_cast<UI16>( mMulti->GetId() - 0x4000 ); // same pattern as Map->MultiArea :contentReference[oaicite:2]{index=2}

    if( !Map->MultiExists( multiNum ) )
        return false;

    const auto& structure = Map->SeekMulti( multiNum ); // contains minX/maxX/minY/maxY :contentReference[oaicite:3]{index=3}

	SI16 width  = 7;
	SI16 height = 7;
	SI16 xCenter = width / 2;
	SI16 yCenter = (height - 1) / 2; // note: height-1 vs height

	if( width < 2 || height < 3 )
		return false;

    // ----------------------------------------------------------------
    // Determine foundation type (store on the house foundation item as a tag)
    // ----------------------------------------------------------------
    UI08 fTypeVal = (UI08)FT_Stone;
    TAGMAPOBJECT t = houseItem->GetTag( "foundationType" );
    if( t.m_ObjectType == TAGMAP_TYPE_INT )
        fTypeVal = (UI08)t.m_IntValue;

    FoundationType fType = FT_Stone;//(FoundationType)fTypeVal;

    // Design Z should be your “ground plane” in design space.
    // If your client expects ground plane == DESIGN_BASE_Z, use that.
    const SI08 designZ = 0;

    // ----------------------------------------------------------------
    // Apply foundation border tiles into baseTiles
    // ----------------------------------------------------------------
    HC_ApplyFoundationBaseTiles( fType, width, height, xCenter, yCenter, designZ, s.baseTiles );

	char msg[128];
	sprintf( msg, "Foundation: w=%d h=%d center=(%d,%d) baseTiles=%u",
			(int)width, (int)height, (int)xCenter, (int)yCenter, (UI32)s.baseTiles.size() );
	sock->SysMessage( msg );

    return ( !s.baseTiles.empty() );
}


void HC_EndSession( CSocket *sock )
{
    if( sock == nullptr )
        return;

    CChar *chr = sock->CurrcharObj();
    if( chr == nullptr )
        return;

    g_houseCustomSessions.erase( chr->GetSerial() );
}

HouseCustomSession *HC_GetSession( CSocket *sock )
{
    if( sock == nullptr )
        return nullptr;

    CChar *chr = sock->CurrcharObj();
    if( chr == nullptr )
        return nullptr;

    auto it = g_houseCustomSessions.find( chr->GetSerial() );
    if( it == g_houseCustomSessions.end() )
        return nullptr;

    return &(it->second);
}

bool HC_IsSessionForHouse( CSocket *sock, SERIAL houseSerial )
{
    HouseCustomSession *s = HC_GetSession( sock );
    return ( s != nullptr && s->houseSerial == houseSerial );
}

void HC_BumpRevision( HouseCustomSession &s )
{
    ++s.revision;
    if( s.revision == 0 )
        s.revision = 1;
}

bool HC_AddTile( HouseCustomSession &s, UI16 id, SI08 x, SI08 y, SI08 z )
{
    HouseTileEntry e;
    e.id = id;
    e.x = x;
    e.y = y;
    e.z = z;
    s.tiles.push_back( e );
    return true;
}

bool HC_RemoveTile( HouseCustomSession &s, UI16 id, SI08 x, SI08 y, SI08 z )
{
    for( auto it = s.tiles.begin(); it != s.tiles.end(); ++it )
    {
        if( it->id == id && it->x == x && it->y == y && it->z == z )
        {
            s.tiles.erase( it );
            return true;
        }
    }
    return false;
}

static void SetCustomHouseTag( CItem *i )
{
    TAGMAPOBJECT t;
    t.m_IntValue     = 1;
    t.m_ObjectType   = TAGMAP_TYPE_INT;
    t.m_StringValue  = "";
    i->SetTag( "customhouse", t );
}

static bool IsCustomHouseItem( CItem *i )
{
    if( !ValidateObject( i ))
        return false;

    TAGMAPOBJECT t = i->GetTag( "customhouse" );
    if( t.m_ObjectType == TAGMAP_TYPE_INT && t.m_IntValue == 1 )
        return true;

    return false;
}

bool HC_CommitSession( CSocket *sock )
{
    if( sock == nullptr )
        return false;

    CChar *chr = sock->CurrcharObj();
    if( chr == nullptr )
    {
        sock->SysMessage( "Commit: chr null" );
        return false;
    }

    HouseCustomSession *s = HC_GetSession( sock );
    if( s == nullptr )
    {
        sock->SysMessage( "Commit: no session" );
        return false;
    }

    CItem *houseItem = CalcItemObjFromSer( s->houseSerial );
    if( !ValidateObject( houseItem ))
    {
        sock->SysMessage( "Commit: houseItem invalid" );
        return false;
    }

    CMultiObj *mMulti = FindMulti( houseItem );
    if( !ValidateObject( mMulti ))
    {
        sock->SysMessage( "Commit: multi invalid" );
        return false;
    }

    // Ownership check - adjust if your shard uses co-owners
    if( !mMulti->IsOwner( chr ))
    {
        sock->SysMessage( "Commit: not owner" );
        return false;
    }

	sock->SysMessage( "Commit: starting apply" );

    // 1) Remove prior custom house components (ONLY those tagged customhouse=1)
    auto itemList = mMulti->GetItemsInMultiList();
    if( itemList != nullptr )
    {
        // Copy to a temp vector first so we do not mutate while iterating
        std::vector<CItem*> toDelete;
        for( const auto &obj : itemList->collection() )
        {
            CItem *it = static_cast<CItem*>( obj );
            if( !ValidateObject( it ))
                continue;

            if( IsCustomHouseItem( it ))
                toDelete.push_back( it );
        }

        for( auto it : toDelete )
        {
            if( ValidateObject( it ))
                it->Delete();
        }
    }

    // 2) Add new custom components from the session
    const SI16 baseX = houseItem->GetX();
    const SI16 baseY = houseItem->GetY();
    const SI08 baseZ = houseItem->GetZ();

    for( const auto &t : s->tiles )
    {
        // Create the component item (graphic = t.id)
        // Use the same factory you use elsewhere for generic items
		CItem *comp = Items->CreateItem( nullptr, chr, t.id, 1, 0, OT_ITEM );
		if( !ValidateObject( comp ))
		{
			sock->SysMessage( "Commit: CreateBlankItem failed" );
			continue;
		}

        comp->SetMovable( 2 ); // non-moveable like house addons
        SetCustomHouseTag( comp );

        // Convert relative coords (client) to world coords (server)
        // NOTE: This assumes x/y/z in your session are relative to the house center.
        const SI16 wx = static_cast<SI16>( baseX + t.x );
        const SI16 wy = static_cast<SI16>( baseY + t.y );
		const SI08 wz = (SI08)( baseZ + t.z );

		char msg[96];
		sprintf( msg, "Commit: baseZ=%d tileZ=%d -> wz=%d", (int)baseZ, (int)t.z, (int)wz );
		sock->SysMessage( msg );

        comp->SetLocation( wx, wy, wz, houseItem->WorldNumber(), houseItem->GetInstanceId() );

        // Attach to multi for saving
		comp->SetMulti( mMulti );
		mMulti->AddToMulti( comp );
    }
    // Force the committing player to see the final house immediately
	HC_RefreshHouseToClient( sock, houseItem, mMulti );

    return true;
}

void HC_LoadExistingCustomTiles( HouseCustomSession &s, CItem *houseItem, CMultiObj *mMulti )
{
    s.tiles.clear();

    if( !ValidateObject( houseItem ) || !ValidateObject( mMulti ))
        return;

    auto itemList = mMulti->GetItemsInMultiList();
    if( itemList == nullptr )
        return;

    const SI16 baseX = houseItem->GetX();
    const SI16 baseY = houseItem->GetY();
    const SI08 baseZ = houseItem->GetZ();

    for( const auto &obj : itemList->collection() )
    {
        CItem *it = static_cast<CItem*>( obj );
        if( !ValidateObject( it ))
            continue;

        if( !IsCustomHouseItem( it ))
            continue;

        HouseTileEntry e;
        e.id = it->GetId();

        // Convert world -> relative
        e.x = (SI08)( it->GetX() - baseX );
        e.y = (SI08)( it->GetY() - baseY );

        // Store design z as "relative to baseZ" (this matches your commit logic: baseZ + t.z)
        e.z = (SI08)( it->GetZ() - baseZ );

        s.tiles.push_back( e );
    }
}

void HC_Backup( HouseCustomSession &s )
{
    s.backupTiles = s.tiles;
}

void HC_Restore( HouseCustomSession &s )
{
    s.tiles = s.backupTiles;
}

void HC_Revert( HouseCustomSession &s )
{
    s.tiles = s.originalTiles;
}

void HC_ClearAll( HouseCustomSession &s )
{
    s.tiles.clear();
}

bool HC_RemoveTileAnyZ( HouseCustomSession &s, UI16 id, SI08 x, SI08 y )
{
    for( auto it = s.tiles.begin(); it != s.tiles.end(); ++it )
    {
        if( it->id == id && it->x == x && it->y == y )
        {
            s.tiles.erase( it );
            return true;
        }
    }
    return false;
}

void HC_BuildCombinedTiles( const HouseCustomSession &s, std::vector<HouseTileEntry> &out )
{
    out.clear();
    out.reserve( s.baseTiles.size() + s.tiles.size() );

    out.insert( out.end(), s.baseTiles.begin(), s.baseTiles.end() );
    out.insert( out.end(), s.tiles.begin(), s.tiles.end() );
}

static void GetFoundationGraphics( FoundationType type, UI16 &east, UI16 &south, UI16 &post, UI16 &corner )
{
    switch( type )
    {
        default:
        case FT_DarkWood:  corner=0x0014; east=0x0015; south=0x0016; post=0x0017; break;
        case FT_LightWood: corner=0x00BD; east=0x00BE; south=0x00BF; post=0x00C0; break;
        case FT_Dungeon:   corner=0x02FD; east=0x02FF; south=0x02FE; post=0x0300; break;
        case FT_Brick:     corner=0x0041; east=0x0043; south=0x0042; post=0x0044; break;
        case FT_Stone:     corner=0x0065; east=0x0064; south=0x0063; post=0x0066; break;
    }
}

static void HC_ApplyFoundationBaseTiles( FoundationType fType, SI16 width, SI16 height, SI16 xCenter, SI16 yCenter, SI08 designZ, std::vector<HouseTileEntry> &baseTiles )
{
    baseTiles.clear();

    UI16 east, south, post, corner;
    GetFoundationGraphics( fType, east, south, post, corner );

    auto Add = [&]( UI16 id, SI16 rx, SI16 ry )
    {
        HouseTileEntry e;
        e.id = id;
        e.x  = (SI08)rx;
        e.y  = (SI08)ry;
        e.z  = designZ;
        baseTiles.push_back( e );
    };

    // IMPORTANT: use height - 1 (not height - 2) for the far edge
    const SI16 westCol  = 0 - xCenter;
    const SI16 eastCol  = (width  - 1) - xCenter;
    const SI16 southRow = 0 - yCenter;
    const SI16 northRow = (height - 1) - yCenter;

    // Corner / post anchors
    Add( post,   westCol,  southRow );
    Add( corner, eastCol,  northRow );

    // South and North edges
    for( SI16 x = 1; x < width; ++x )
    {
        Add( south, x - xCenter, southRow );

        if( x < width - 1 )
            Add( south, x - xCenter, northRow );
    }

    // West and East edges
    for( SI16 y = 1; y < height; ++y )
    {
        Add( east, westCol, y - yCenter );

        if( y < height - 1 )
            Add( east, eastCol, y - yCenter );
    }

    // OPTIONAL: porch/front extension
    // Disable this while aligning your rectangle; re-enable once aligned.
    /*
    {
        const SI16 frontRow = (southRow - 1); // one tile outside the south edge
        for( SI16 x = 0; x < width; ++x )
            Add( south, x - xCenter, frontRow );

        Add( post, westCol, frontRow );
        Add( post, eastCol, frontRow );
    }
    */
}

static void HC_RefreshHouseToClient( CSocket *sock, CItem *houseItem, CMultiObj *mMulti )
{
    if( sock == nullptr || !ValidateObject( houseItem ) || !ValidateObject( mMulti ))
        return;

    // 1) If your core has a "send multi" or "send object" call, use it here.
    // Different UOX3 branches vary; the safe fallback is to resend items in multi.

    auto itemList = mMulti->GetItemsInMultiList();
    if( itemList != nullptr )
    {
        for( const auto &obj : itemList->collection() )
        {
            CItem *it = static_cast<CItem*>( obj );
            if( !ValidateObject( it ))
                continue;

            // Ensure the client receives the item state again.
            // Many UOX3 objects have Update( sock ) or SendToSocket methods.
            // Use whichever exists in your codebase.
            it->Update( sock );
        }
    }

    // 2) Resend the foundation/multi itself if you have a supported API.
    // Some codebases support houseItem->Update(sock) or mMulti->Update(sock).
    houseItem->Update( sock );

    // 3) If needed, force the character to refresh world view
    // (only if you have issues with stale multis)
    // sock->CurrcharObj()->Teleport();
}

namespace zlibhelper
{
    std::vector<UI08> Decompress( const std::vector<UI08> &source, size_t decompressedSize )
    {
        uLongf srcSize = static_cast<uLongf>( source.size() );
        uLongf dstSize = static_cast<uLongf>( decompressedSize );

        std::vector<UI08> dest( decompressedSize, 0 );
        int status = uncompress2( dest.data(), &dstSize, source.data(), &srcSize );
        if( status != Z_OK )
        {
            dest.clear();
            return dest;
        }

        dest.resize( dstSize );
        return dest;
    }

    std::vector<UI08> Compress( const std::vector<UI08> &source )
    {
        uLongf outSize = compressBound( static_cast<uLong>( source.size() ) );
        std::vector<UI08> out( outSize, 0 );

        int status = compress2(
            reinterpret_cast<Bytef*>( out.data() ), &outSize,
            reinterpret_cast<const Bytef*>( source.data() ), static_cast<uLongf>( source.size() ),
            Z_DEFAULT_COMPRESSION
        );

        if( status != Z_OK )
        {
            out.clear();
            return out;
        }

        out.resize( outSize );
        return out;
    }
}
