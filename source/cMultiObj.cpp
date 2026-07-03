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
#include "regions.h"
#include <classes.h>
#include <CPacketSend.h>
#include <array>
#include <fstream>
#include <sstream>
#include <unordered_set>
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
					// Store serial in a temporary vector until all objects have been loaded
					pendingBans.push_back( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
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
					// Store serial in a temporary vector until all objects have been loaded
					pendingOwners.push_back( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
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
					// Store serial in a temporary vector until all objects have been loaded
					pendingFriends.push_back( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				break;
			case 'G':
				if( UTag == "GUEST" )
				{
					// Store serial in a temporary vector until all objects have been loaded
					pendingGuests.push_back( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
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
					// Store serial in a temporary vector until all objects have been loaded
					pendingLockedItems.push_back( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
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
					// Store serial in a temporary vector until all objects have been loaded
					pendingOwners.push_back( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				break;
			case 'S':
				if( UTag == "SECURECONTAINER" )
				{
					// Store serial in a temporary vector until all objects have been loaded
					pendingSecureContainers.push_back( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
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
					// Store serial in a temporary vector until all objects have been loaded
					pendingVendors.push_back( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				break;
		}
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj::PostLoadProcessing()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Used to setup any pointers that may need adjustment following the loading of the world
//o------------------------------------------------------------------------------------------------o
void CMultiObj::PostLoadProcessing( void )
{
	// Ensure base class processing fires first
	CItem::PostLoadProcessing();

	// Helper lambda for resolving deferred Character/Item-lists
	auto ResolveObjects = [this]( std::vector<SERIAL>& pendingList, auto lookupFunc, auto action ) 
	{
		for( SERIAL ser : pendingList ) 
		{
			auto ptr = lookupFunc( ser );
			if( ValidateObject( ptr )) 
			{
				( this->*action )( ptr );
			}
		}
		pendingList.clear();
	};

	// Process deferred character-based lists
	ResolveObjects( pendingBans, CalcCharObjFromSer, &CMultiObj::AddToBanList );
	ResolveObjects( pendingOwners, CalcCharObjFromSer, &CMultiObj::AddAsOwner );
	ResolveObjects( pendingFriends, CalcCharObjFromSer, &CMultiObj::AddAsFriend );
	ResolveObjects( pendingGuests, CalcCharObjFromSer, &CMultiObj::AddAsGuest );
	ResolveObjects( pendingVendors, CalcCharObjFromSer, &CMultiObj::AddVendor );

	// Process deferred item-based lists
	ResolveObjects( pendingLockedItems, CalcItemObjFromSer, &CMultiObj::LockDownItem );
	ResolveObjects( pendingSecureContainers, CalcItemObjFromSer, &CMultiObj::SecureContainer );
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
static const SI08 DESIGN_FOUNDATION_Z = 0;
static const UI16 CUSTOM_FOUNDATION_STEP_ID = 0x0751;
static const UI16 CUSTOM_FOUNDATION_STEP_COLOUR = 0x0455;
static const UI16 CUSTOM_SIGNPOST_ID = 0x0009;
static const UI16 CUSTOM_SIGNHANGER_ID = 0x0B98;
static const char *CUSTOM_HOUSE_DESIGN_TAG = "customHouseDesign";
static const char *CUSTOM_HOUSE_REVISION_TAG = "customHouseRevision";

struct HouseStairComponent
{
    UI16 blockId;
    UI16 stairId;
    SI08 xStep;
    SI08 yStep;
};

static bool HC_LoadFoundationMultiTiles( UI16 multiNum, std::vector<HouseTileEntry> &baseTiles );
static void HC_AddFoundationStepTiles( HouseCustomSession &s );
static void HC_EnsureFoundationStepItems( CChar *chr, CItem *houseItem, CMultiObj *mMulti );
static void HC_EnsureFoundationSignFixtures( CChar *chr, CItem *houseItem, CMultiObj *mMulti );
static void HC_RemoveCustomizerFootStairs( HouseCustomSession &s, CChar *chr, CItem *houseItem, CMultiObj *mMulti );
static CItem *HC_FindHouseSign( CItem *houseItem, CMultiObj *mMulti );
static bool HC_AddComponentStairs( HouseCustomSession &s, UI16 multiId, SI08 x, SI08 y, SI08 z );
static bool HC_FindStairComponent( UI16 multiId, HouseStairComponent &component );
static bool HC_IsStairComponentTile( UI16 id );
static bool HC_IsPlaceableComponentTile( UI16 id );
static bool HC_CanPlaceDesignTile( const HouseCustomSession &s, UI16 id, SI08 x, SI08 y, SI08 z );
static void HC_ApplyFoundationBaseTiles( FoundationType fType, SI16 width, SI16 height, SI16 xCenter, SI16 yCenter, SI08 designZ, std::vector<HouseTileEntry> &baseTiles );
static void HC_RefreshHouseToClient( CSocket *sock, CItem *houseItem, CMultiObj *mMulti );
static bool HC_ConvertToCustomFoundation( CSocket *sock, CItem *houseItem, CMultiObj *mMulti );
static std::string HC_SerializeDesignTiles( const std::vector<HouseTileEntry> &tiles );
static bool HC_LoadSerializedDesignTiles( CMultiObj *mMulti, std::vector<HouseTileEntry> &tiles );
static void HC_SaveSerializedDesignTiles( CMultiObj *mMulti, const std::vector<HouseTileEntry> &tiles );
static UI32 HC_GetCommittedDesignRevision( CMultiObj *mMulti );
static UI32 HC_BumpCommittedDesignRevision( CMultiObj *mMulti );
static void HC_DeleteLegacyCustomHouseItems( CMultiObj *mMulti );
static bool IsCustomHouseItem( CItem *i );
static bool IsCustomFoundationStepItem( CItem *i );
static bool IsCustomFoundationSignFixture( CItem *i );

bool HC_StartSession( CSocket *sock, SERIAL houseSerial )
{
    if( sock == nullptr )
        return false;

    CChar *chr = sock->CurrcharObj();
    if( chr == nullptr )
        return false;

	CItem *houseItem = CalcItemObjFromSer( houseSerial );
	if( !ValidateObject( houseItem ))
		return false;

	CMultiObj *mMulti = FindMulti( houseItem );
	if( !ValidateObject( mMulti ))
		return false;

	HC_ConvertToCustomFoundation( sock, houseItem, mMulti );
	HC_EnsureFoundationStepItems( chr, houseItem, mMulti );
	HC_EnsureFoundationSignFixtures( chr, houseItem, mMulti );

	HouseCustomSession s;
	s.houseSerial = houseSerial;
	s.revision = 1;
	s.clientLevel = 1;
	s.floor = 0;
	s.minX = 0;
	s.maxX = 0;
	s.minY = 0;
	s.maxY = 0;
	s.tiles.clear();
	s.baseTiles.clear();
	s.originalTiles.clear();
	s.backupTiles.clear();

    HC_LoadFoundationTiles( sock, s );


	// Load already committed custom components into design tiles
	HC_LoadExistingCustomTiles( s, houseItem, mMulti );
	HC_RemoveCustomizerFootStairs( s, chr, houseItem, mMulti );

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

    const UI16 multiNum = static_cast<UI16>( mMulti->GetId() - 0x4000 );

    if( !Map->MultiExists( multiNum ) )
        return false;

    const auto& structure = Map->SeekMulti( multiNum );

	SI16 width  = static_cast<SI16>( structure.maxX - structure.minX + 1 );
	SI16 height = static_cast<SI16>( structure.maxY - structure.minY + 1 );
	SI16 xCenter = static_cast<SI16>( -structure.minX );
	SI16 yCenter = static_cast<SI16>( -structure.minY );

	s.minX = static_cast<SI16>( structure.minX );
	s.maxX = static_cast<SI16>( structure.maxX );
	s.minY = static_cast<SI16>( structure.minY );
	s.maxY = static_cast<SI16>( structure.maxY );

	if( width < 2 || height < 3 )
		return false;

    if( !HC_LoadFoundationMultiTiles( multiNum, s.baseTiles ))
    {
        UI08 fTypeVal = (UI08)FT_Stone;
        TAGMAPOBJECT t = houseItem->GetTag( "foundationType" );
        if( t.m_ObjectType == TAGMAP_TYPE_INT )
            fTypeVal = (UI08)t.m_IntValue;

        if( fTypeVal > static_cast<UI08>( FT_Stone ))
            fTypeVal = static_cast<UI08>( FT_Stone );

        HC_ApplyFoundationBaseTiles( static_cast<FoundationType>( fTypeVal ), width, height, xCenter, yCenter, DESIGN_FOUNDATION_Z, s.baseTiles );
    }

    HC_AddFoundationStepTiles( s );

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

static bool HC_CanPlaceDesignTile( const HouseCustomSession &s, UI16 id, SI08 x, SI08 y, SI08 z )
{
    if( !Map->IsValidTile( id ))
        return false;

    if( x < s.minX || x > s.maxX || y < s.minY || y > s.maxY )
        return false;

    if( z < FloorToDesignZ( 0 ) || z > FloorToDesignZ( 3 ))
        return false;

    for( const auto &tile : s.tiles )
    {
        if( tile.id == id && tile.x == x && tile.y == y && tile.z == z )
            return false;
    }

    return true;
}

bool HC_CanPlaceTile( const HouseCustomSession &s, UI16 id, SI08 x, SI08 y, SI08 z )
{
    if( !HC_CanPlaceDesignTile( s, id, x, y, z ))
        return false;

    return HC_IsPlaceableComponentTile( id );
}

bool HC_AddTile( HouseCustomSession &s, UI16 id, SI08 x, SI08 y, SI08 z )
{
    if( !HC_CanPlaceTile( s, id, x, y, z ))
        return false;

    HouseTileEntry e;
    e.id = id;
    e.x = x;
    e.y = y;
    e.z = z;
    s.tiles.push_back( e );
    return true;
}

bool HC_AddStairs( HouseCustomSession &s, UI16 multiId, SI08 x, SI08 y, SI08 z )
{
    if( multiId >= 0x4000 )
        multiId = static_cast<UI16>( multiId - 0x4000 );

    if( HC_AddComponentStairs( s, multiId, x, y, z ))
        return true;

    if( !Map->MultiExists( multiId ))
        return false;

    const auto& structure = Map->SeekMulti( multiId );
    std::vector<HouseTileEntry> pending;
    pending.reserve( structure.items.size() );

    for( const auto &multiItem : structure.items )
    {
        SI16 rx = static_cast<SI16>( x + multiItem.offsetX );
        SI16 ry = static_cast<SI16>( y + multiItem.offsetY );
        SI16 rz = static_cast<SI16>( z + multiItem.altitude );

        if( rx < -128 || rx > 127 || ry < -128 || ry > 127 || rz < -128 || rz > 127 )
            return false;

        HouseTileEntry e;
        e.id = multiItem.tileId;
        e.x = static_cast<SI08>( rx );
        e.y = static_cast<SI08>( ry );
        e.z = static_cast<SI08>( rz );

        if( !HC_CanPlaceDesignTile( s, e.id, e.x, e.y, e.z ))
            return false;

        pending.push_back( e );
    }

    for( const auto &tile : pending )
    {
        HC_RemoveAtXYZ( s, tile.x, tile.y, tile.z );
        s.tiles.push_back( tile );
    }

    return !pending.empty();
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

bool HC_RemoveTileAtXYZ( HouseCustomSession &s, SI08 x, SI08 y, SI08 z )
{
    for( auto it = s.tiles.begin(); it != s.tiles.end(); ++it )
    {
        if( it->x == x && it->y == y && it->z == z )
        {
            s.tiles.erase( it );
            return true;
        }
    }

    return false;
}

static bool IsCustomFoundationId( UI16 id )
{
    return ( id >= 0x53EC && id <= 0x547B );
}

static UI16 HC_GetFoundationIdForSize( SI16 width, SI16 height )
{
    if( width < 7 ) width = 7;
    if( height < 7 ) height = 7;
    if( width > 18 ) width = 18;
    if( height > 18 ) height = 18;

    const UI16 multiId = static_cast<UI16>( 0x13EC + (( height - 7 ) * 12 ) + ( width - 7 ));
    return static_cast<UI16>( 0x4000 + multiId );
}

static bool HC_ConvertToCustomFoundation( CSocket *sock, CItem *houseItem, CMultiObj *mMulti )
{
    if( !ValidateObject( houseItem ) || !ValidateObject( mMulti ))
        return false;

    if( IsCustomFoundationId( mMulti->GetId() ))
        return true;

    const UI16 oldMultiNum = static_cast<UI16>( mMulti->GetId() - 0x4000 );
    if( !Map->MultiExists( oldMultiNum ))
        return false;

    const auto& structure = Map->SeekMulti( oldMultiNum );
    const SI16 width = static_cast<SI16>( structure.maxX - structure.minX + 1 );
    const SI16 height = static_cast<SI16>( structure.maxY - structure.minY + 1 );
    const UI16 foundationId = HC_GetFoundationIdForSize( width, height );
    const UI16 foundationMultiNum = static_cast<UI16>( foundationId - 0x4000 );

    if( !Map->MultiExists( foundationMultiNum ))
        return false;

    auto itemList = mMulti->GetItemsInMultiList();
    if( itemList != nullptr )
    {
        std::vector<CItem*> toDelete;
        for( const auto &obj : itemList->collection() )
        {
            CItem *it = static_cast<CItem*>( obj );
            if( !ValidateObject( it ))
                continue;

            const ItemTypes type = it->GetType();
            if( type == IT_DOOR || type == IT_LOCKEDDOOR || IsCustomHouseItem( it ) || IsCustomFoundationStepItem( it ) || IsCustomFoundationSignFixture( it ))
                toDelete.push_back( it );
        }

        for( auto it : toDelete )
        {
            if( ValidateObject( it ))
                it->Delete();
        }
    }

    TAGMAPOBJECT convertedTag;
    convertedTag.m_Destroy = false;
    convertedTag.m_IntValue = mMulti->GetId();
    convertedTag.m_ObjectType = TAGMAP_TYPE_INT;
    convertedTag.m_StringValue = "";
    mMulti->SetTag( "customFoundationOriginalId", convertedTag );

    mMulti->SetId( foundationId );
    mMulti->ShouldSave( true );
    mMulti->Update( sock );
    return true;
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

static std::string HC_SerializeDesignTiles( const std::vector<HouseTileEntry> &tiles )
{
    std::ostringstream stream;

    bool first = true;
    for( const auto &tile : tiles )
    {
        if( !first )
            stream << ';';

        stream << tile.id << ','
               << static_cast<SI16>( tile.x ) << ','
               << static_cast<SI16>( tile.y ) << ','
               << static_cast<SI16>( tile.z );
        first = false;
    }

    return stream.str();
}

static bool HC_LoadSerializedDesignTiles( CMultiObj *mMulti, std::vector<HouseTileEntry> &tiles )
{
    tiles.clear();

    if( !ValidateObject( mMulti ))
        return false;

    TAGMAPOBJECT designTag = mMulti->GetTag( CUSTOM_HOUSE_DESIGN_TAG );
    if( designTag.m_ObjectType != TAGMAP_TYPE_STRING || designTag.m_StringValue.empty() )
        return false;

    std::stringstream stream( designTag.m_StringValue );
    std::string part;
    while( std::getline( stream, part, ';' ))
    {
        if( part.empty() )
            continue;

        std::stringstream entryStream( part );
        std::string field;
        SI32 values[4] = { 0, 0, 0, 0 };
        UI08 index = 0;

        while( index < 4 && std::getline( entryStream, field, ',' ))
        {
            try
            {
                values[index] = std::stoi( field );
            }
            catch( ... )
            {
                index = 0;
                break;
            }

            ++index;
        }

        if( index != 4 )
            continue;

        if( values[0] < 0 || values[0] > 0xFFFF ||
            values[1] < -128 || values[1] > 127 ||
            values[2] < -128 || values[2] > 127 ||
            values[3] < -128 || values[3] > 127 )
        {
            continue;
        }

        HouseTileEntry tile;
        tile.id = static_cast<UI16>( values[0] );
        tile.x = static_cast<SI08>( values[1] );
        tile.y = static_cast<SI08>( values[2] );
        tile.z = static_cast<SI08>( values[3] );
        tiles.push_back( tile );
    }

    return true;
}

static void HC_SaveSerializedDesignTiles( CMultiObj *mMulti, const std::vector<HouseTileEntry> &tiles )
{
    if( !ValidateObject( mMulti ))
        return;

    TAGMAPOBJECT designTag;
    designTag.m_ObjectType = TAGMAP_TYPE_STRING;
    designTag.m_IntValue = 0;
    designTag.m_Destroy = tiles.empty();
    designTag.m_StringValue = HC_SerializeDesignTiles( tiles );

    mMulti->SetTag( CUSTOM_HOUSE_DESIGN_TAG, designTag );
    mMulti->ShouldSave( true );
}

static UI32 HC_GetCommittedDesignRevision( CMultiObj *mMulti )
{
    if( !ValidateObject( mMulti ))
        return 1;

    TAGMAPOBJECT revisionTag = mMulti->GetTag( CUSTOM_HOUSE_REVISION_TAG );
    if( revisionTag.m_ObjectType == TAGMAP_TYPE_INT && revisionTag.m_IntValue > 0 )
        return static_cast<UI32>( revisionTag.m_IntValue );

    return 1;
}

static UI32 HC_BumpCommittedDesignRevision( CMultiObj *mMulti )
{
    if( !ValidateObject( mMulti ))
        return 1;

    UI32 revision = HC_GetCommittedDesignRevision( mMulti ) + 1;
    if( revision == 0 )
        revision = 1;

    TAGMAPOBJECT revisionTag;
    revisionTag.m_ObjectType = TAGMAP_TYPE_INT;
    revisionTag.m_IntValue = static_cast<SI32>( revision );
    revisionTag.m_Destroy = false;
    revisionTag.m_StringValue = "";
    mMulti->SetTag( CUSTOM_HOUSE_REVISION_TAG, revisionTag );
    mMulti->ShouldSave( true );

    return revision;
}

static void HC_DeleteLegacyCustomHouseItems( CMultiObj *mMulti )
{
    if( !ValidateObject( mMulti ))
        return;

    auto itemList = mMulti->GetItemsInMultiList();
    if( itemList == nullptr )
        return;

    std::vector<CItem*> toDelete;
    for( const auto &obj : itemList->collection() )
    {
        CItem *it = static_cast<CItem*>( obj );
        if( !ValidateObject( it ) || !IsCustomHouseItem( it ))
            continue;

        toDelete.push_back( it );
    }

    for( auto it : toDelete )
    {
        if( ValidateObject( it ))
            it->Delete();
    }
}

bool HC_LoadCommittedDesignTiles( CMultiObj *mMulti, std::vector<HouseTileEntry> &tiles )
{
    return HC_LoadSerializedDesignTiles( mMulti, tiles );
}

bool HC_SendCommittedDesignState( CSocket *sock, CMultiObj *mMulti, bool enableResponse )
{
    if( sock == nullptr || !ValidateObject( mMulti ) || !sock->LoginComplete() )
        return false;

    CChar *viewer = sock->CurrcharObj();
    if( !ValidateObject( viewer ))
        return false;

    if( HC_IsSessionForHouse( sock, mMulti->GetSerial() ))
        return false;

    std::vector<HouseTileEntry> customTiles;
    if( !HC_LoadCommittedDesignTiles( mMulti, customTiles ) || customTiles.empty() )
        return false;

    HouseCustomSession state;
    state.houseSerial = mMulti->GetSerial();
    state.revision = HC_GetCommittedDesignRevision( mMulti );
    state.clientLevel = 1;
    state.floor = 0;
    state.minX = 0;
    state.maxX = 0;
    state.minY = 0;
    state.maxY = 0;
    state.tiles = customTiles;
    state.baseTiles.clear();
    state.originalTiles.clear();
    state.backupTiles.clear();

    HC_LoadFoundationTiles( sock, state );

    sock->Send( &CPHouseDesignStateGeneral( state.houseSerial, state.revision ) );

    std::vector<HouseTileEntry> sendTiles;
    HC_BuildCombinedTiles( state, sendTiles );
    sock->Send( &CPHouseDesignStateDetailed( state.houseSerial, state.revision, sendTiles, enableResponse ) );
    return true;
}

static bool IsCustomFoundationStepItem( CItem *i )
{
    if( !ValidateObject( i ))
        return false;

    TAGMAPOBJECT t = i->GetTag( "customfoundationstep" );
    return ( t.m_ObjectType == TAGMAP_TYPE_INT && t.m_IntValue == 1 );
}

static bool IsCustomFoundationSignFixture( CItem *i )
{
    if( !ValidateObject( i ))
        return false;

    TAGMAPOBJECT t = i->GetTag( "customfoundationsignfixture" );
    return ( t.m_ObjectType == TAGMAP_TYPE_INT && t.m_IntValue == 1 );
}

static void SetCustomFoundationFixtureTag( CItem *i, const char *tagName )
{
    if( !ValidateObject( i ) || tagName == nullptr )
        return;

    TAGMAPOBJECT tagObject;
    tagObject.m_ObjectType = TAGMAP_TYPE_INT;
    tagObject.m_IntValue = 1;
    tagObject.m_Destroy = false;
    tagObject.m_StringValue = "";
    i->SetTag( tagName, tagObject );
}

static void SetCustomFoundationStepTag( CItem *i )
{
    SetCustomFoundationFixtureTag( i, "customfoundationstep" );
}

static void SetCustomFoundationSignFixtureTag( CItem *i )
{
    SetCustomFoundationFixtureTag( i, "customfoundationsignfixture" );
}

static void HC_AddFoundationStepTiles( HouseCustomSession &s )
{
    if( !Map->IsValidTile( CUSTOM_FOUNDATION_STEP_ID ))
        return;

    const SI16 y = static_cast<SI16>( s.maxY + 1 );
    s.maxY = y;

    for( SI16 x = static_cast<SI16>( s.minX + 1 ); x <= s.maxX; ++x )
    {
        if( x < -128 || x > 127 || y < -128 || y > 127 )
            continue;

        bool exists = false;
        for( const auto &tile : s.baseTiles )
        {
            if( tile.id == CUSTOM_FOUNDATION_STEP_ID && tile.x == x && tile.y == y && tile.z == DESIGN_FOUNDATION_Z )
            {
                exists = true;
                break;
            }
        }

        if( exists )
            continue;

        HouseTileEntry e;
        e.id = CUSTOM_FOUNDATION_STEP_ID;
        e.x = static_cast<SI08>( x );
        e.y = static_cast<SI08>( y );
        e.z = DESIGN_FOUNDATION_Z;
        s.baseTiles.push_back( e );
    }
}

static void HC_EnsureFoundationStepItems( CChar *chr, CItem *houseItem, CMultiObj *mMulti )
{
    if( !ValidateObject( chr ) || !ValidateObject( houseItem ) || !ValidateObject( mMulti ))
        return;

    if( !IsCustomFoundationId( mMulti->GetId() ))
        return;

    const UI16 multiNum = static_cast<UI16>( mMulti->GetId() - 0x4000 );
    if( !Map->MultiExists( multiNum ))
        return;

    auto itemList = mMulti->GetItemsInMultiList();
    if( itemList != nullptr )
    {
        std::vector<CItem*> toDelete;
        for( const auto &obj : itemList->collection() )
        {
            CItem *it = static_cast<CItem*>( obj );
            if( IsCustomFoundationStepItem( it ))
                toDelete.push_back( it );
        }

        for( auto it : toDelete )
        {
            if( ValidateObject( it ))
                it->Delete();
        }
    }
}

static CItem *HC_FindHouseSign( CItem *houseItem, CMultiObj *mMulti )
{
    if( !ValidateObject( houseItem ))
        return nullptr;

    CItem *sign = nullptr;
    if( ValidateObject( mMulti ))
        sign = CalcItemObjFromSer( mMulti->GetTempVar( CITV_MORE ));

    if( ValidateObject( sign ) && sign->GetType() == IT_HOUSESIGN )
        return sign;

    sign = CalcItemObjFromSer( houseItem->GetTempVar( CITV_MORE ));
    if( ValidateObject( sign ) && sign->GetType() == IT_HOUSESIGN )
        return sign;

    CItem *nearestSign = nullptr;
    SI32 nearestDistance = INT_MAX;
    for( auto &MapArea : MapRegion->PopulateList( houseItem ))
    {
        if( MapArea == nullptr )
            continue;

        auto regItems = MapArea->GetItemList();
        for( const auto &obj : regItems->collection() )
        {
            CItem *item = static_cast<CItem*>( obj );
            if( !ValidateObject( item ) || item->GetType() != IT_HOUSESIGN )
                continue;

            if( item->GetTempVar( CITV_MORE ) != houseItem->GetSerial() )
                continue;

            const SI32 distance = std::abs( item->GetX() - houseItem->GetX() ) + std::abs( item->GetY() - houseItem->GetY() );
            if( distance < nearestDistance )
            {
                nearestSign = item;
                nearestDistance = distance;
            }
        }
    }

    return nearestSign;
}

static void HC_EnsureFoundationSignFixtures( CChar *chr, CItem *houseItem, CMultiObj *mMulti )
{
    if( !ValidateObject( chr ) || !ValidateObject( houseItem ) || !ValidateObject( mMulti ))
        return;

    if( !IsCustomFoundationId( mMulti->GetId() ))
        return;

    const UI16 multiNum = static_cast<UI16>( mMulti->GetId() - 0x4000 );
    if( !Map->MultiExists( multiNum ))
        return;

    auto itemList = mMulti->GetItemsInMultiList();
    if( itemList != nullptr )
    {
        std::vector<CItem*> toDelete;
        for( const auto &obj : itemList->collection() )
        {
            CItem *it = static_cast<CItem*>( obj );
            if( IsCustomFoundationSignFixture( it ))
                toDelete.push_back( it );
        }

        for( auto it : toDelete )
        {
            if( ValidateObject( it ))
                it->Delete();
        }
    }

    const auto& structure = Map->SeekMulti( multiNum );
    const SI16 baseX = houseItem->GetX();
    const SI16 baseY = houseItem->GetY();
    const SI08 baseZ = houseItem->GetZ();
    SI16 signX = static_cast<SI16>( baseX + structure.minX );
    SI16 signY = static_cast<SI16>( baseY + structure.maxY + 1 );
    SI08 signZ = static_cast<SI08>( baseZ + 7 );

    CItem *sign = HC_FindHouseSign( houseItem, mMulti );
    if( ValidateObject( sign ) && sign->GetType() == IT_HOUSESIGN )
    {
        sign->SetLocation( signX, signY, signZ, houseItem->WorldNumber(), houseItem->GetInstanceId() );
        sign->SetTempVar( CITV_MORE, houseItem->GetSerial() );
        houseItem->SetTempVar( CITV_MORE, sign->GetSerial() );
        mMulti->SetTempVar( CITV_MORE, sign->GetSerial() );
        sign->SetMovable( 2 );
        sign->SetDecayable( false );
        sign->SetMulti( mMulti );
        sign->Update();
    }

    const SI16 postY = static_cast<SI16>( signY - 1 );

    CItem *post = Items->CreateItem( nullptr, chr, CUSTOM_SIGNPOST_ID, 1, 0, OT_ITEM, false, true,
        houseItem->WorldNumber(), houseItem->GetInstanceId(), signX, postY, signZ );
    if( ValidateObject( post ))
    {
        post->SetLocation( signX, postY, signZ, houseItem->WorldNumber(), houseItem->GetInstanceId() );
        post->SetMovable( 2 );
        post->SetDecayable( false );
        SetCustomFoundationSignFixtureTag( post );
        post->SetMulti( mMulti );
    }

    CItem *hanger = Items->CreateItem( nullptr, chr, CUSTOM_SIGNHANGER_ID, 1, 0, OT_ITEM, false, true,
        houseItem->WorldNumber(), houseItem->GetInstanceId(), signX, signY, signZ );
    if( ValidateObject( hanger ))
    {
        hanger->SetLocation( signX, signY, signZ, houseItem->WorldNumber(), houseItem->GetInstanceId() );
        hanger->SetMovable( 2 );
        hanger->SetDecayable( false );
        SetCustomFoundationSignFixtureTag( hanger );
        hanger->SetMulti( mMulti );
    }
}

static std::vector<std::string> HC_SplitTabLine( const std::string &line )
{
    std::vector<std::string> tokens;
    std::stringstream ss( line );
    std::string token;
    while( std::getline( ss, token, '\t' ))
    {
        tokens.push_back( token );
    }
    return tokens;
}

static bool HC_ParseComponentId( const std::vector<std::string> &tokens, size_t index, UI16 &value )
{
    if( index >= tokens.size() || tokens[index].empty() )
        return false;

    SI32 parsed = 0;
    try
    {
        parsed = static_cast<SI32>( std::stol( tokens[index], nullptr, 0 ));
    }
    catch( ... )
    {
        return false;
    }

    if( parsed <= 0 || parsed > 0xFFFF )
        return false;

    value = static_cast<UI16>( parsed );
    return true;
}

static std::unordered_map<UI16, HouseStairComponent> HC_LoadStairComponents()
{
    std::unordered_map<UI16, HouseStairComponent> components;
    const std::string fileName = cwmWorldState->ServerData()->Directory( CSDDP_ROOT ) + "data/components/stairs.txt";
    std::ifstream input( fileName.c_str() );
    if( !input.is_open() )
    {
        Console.Warning( oldstrutil::format( "Unable to load custom housing stair components from %s", fileName.c_str() ));
        return components;
    }

    std::string line;
    UI32 lineNo = 0;
    while( std::getline( input, line ))
    {
        ++lineNo;
        if( line.empty() || lineNo <= 2 )
            continue;

        const auto tokens = HC_SplitTabLine( line );
        if( tokens.size() < 14 )
            continue;

        UI16 blockId = 0;
        UI16 northId = 0;
        UI16 eastId = 0;
        UI16 southId = 0;
        UI16 westId = 0;
        if( !HC_ParseComponentId( tokens, 1, blockId ))
            continue;

        HC_ParseComponentId( tokens, 2, northId );
        HC_ParseComponentId( tokens, 3, eastId );
        HC_ParseComponentId( tokens, 4, southId );
        HC_ParseComponentId( tokens, 5, westId );

        const std::array<UI16, 4> stairIds = { northId, eastId, southId, westId };
        const std::array<SI08, 4> xSteps = { 0, 1, 0, -1 };
        const std::array<SI08, 4> ySteps = { -1, 0, 1, 0 };

        for( size_t dir = 0; dir < stairIds.size(); ++dir )
        {
            UI16 multiId = 0;
            if( !HC_ParseComponentId( tokens, 10 + dir, multiId ) || stairIds[dir] == 0 )
                continue;

            HouseStairComponent component;
            component.blockId = blockId;
            component.stairId = stairIds[dir];
            component.xStep = xSteps[dir];
            component.yStep = ySteps[dir];
            components[multiId] = component;
        }
    }

    return components;
}

static bool HC_FindStairComponent( UI16 multiId, HouseStairComponent &component )
{
    static const std::unordered_map<UI16, HouseStairComponent> components = HC_LoadStairComponents();
    auto it = components.find( multiId );
    if( it == components.end() )
        return false;

    component = it->second;
    return true;
}

static bool HC_IsStairComponentTile( UI16 id )
{
    static const std::unordered_map<UI16, HouseStairComponent> components = HC_LoadStairComponents();
    for( const auto &entry : components )
    {
        if( entry.second.stairId == id || entry.second.blockId == id )
            return true;
    }

    return false;
}

static void HC_LoadComponentIdsFromFile( std::unordered_set<UI16> &components, const std::string &fileName, size_t firstIndex, size_t lastIndex )
{
    std::ifstream input( fileName.c_str() );
    if( !input.is_open() )
    {
        Console.Warning( oldstrutil::format( "Unable to load custom housing components from %s", fileName.c_str() ));
        return;
    }

    std::string line;
    UI32 lineNo = 0;
    while( std::getline( input, line ))
    {
        ++lineNo;
        if( line.empty() || lineNo <= 2 )
            continue;

        const auto tokens = HC_SplitTabLine( line );
        if( tokens.size() <= lastIndex )
            continue;

        for( size_t index = firstIndex; index <= lastIndex; ++index )
        {
            UI16 componentId = 0;
            if( HC_ParseComponentId( tokens, index, componentId ) && Map->IsValidTile( componentId ))
                components.insert( componentId );
        }
    }
}

static std::unordered_set<UI16> HC_LoadPlaceableComponents()
{
    std::unordered_set<UI16> components;
    const std::string componentDir = cwmWorldState->ServerData()->Directory( CSDDP_ROOT ) + "data/components/";

    HC_LoadComponentIdsFromFile( components, componentDir + "walls.txt", 3, 16 );
    HC_LoadComponentIdsFromFile( components, componentDir + "floors.txt", 1, 16 );
    HC_LoadComponentIdsFromFile( components, componentDir + "roof.txt", 3, 18 );
    HC_LoadComponentIdsFromFile( components, componentDir + "misc.txt", 3, 10 );
    HC_LoadComponentIdsFromFile( components, componentDir + "teleprts.txt", 1, 16 );
    HC_LoadComponentIdsFromFile( components, componentDir + "doors.txt", 1, 8 );

    return components;
}

static bool HC_IsPlaceableComponentTile( UI16 id )
{
    static const std::unordered_set<UI16> components = HC_LoadPlaceableComponents();
    return ( components.find( id ) != components.end() );
}

static bool HC_AddComponentStairs( HouseCustomSession &s, UI16 multiId, SI08 x, SI08 y, SI08 z )
{
    HouseStairComponent component;
    if( !HC_FindStairComponent( multiId, component ))
        return false;

    if( !Map->IsValidTile( component.stairId ))
        return false;

    std::vector<HouseTileEntry> pending;
    pending.reserve( 4 );

    for( SI16 step = 0; step < 4; ++step )
    {
        const SI16 rx = static_cast<SI16>( x + ( component.xStep * step ));
        const SI16 ry = static_cast<SI16>( y + ( component.yStep * step ));
        const SI16 rz = static_cast<SI16>( z + ( step * 5 ));

        if( rx < -128 || rx > 127 || ry < -128 || ry > 127 || rz < -128 || rz > 127 )
            return false;

        HouseTileEntry e;
        e.id = component.stairId;
        e.x = static_cast<SI08>( rx );
        e.y = static_cast<SI08>( ry );
        e.z = static_cast<SI08>( rz );

        if( !HC_CanPlaceDesignTile( s, e.id, e.x, e.y, e.z ))
            return false;

        pending.push_back( e );
    }

    for( const auto &tile : pending )
    {
        HC_RemoveAtXYZ( s, tile.x, tile.y, tile.z );
        s.tiles.push_back( tile );
    }

    return !pending.empty();
}

bool HC_CommitSession( CSocket *sock )
{
    if( sock == nullptr )
        return false;

    CChar *chr = sock->CurrcharObj();
    if( chr == nullptr )
        return false;

    HouseCustomSession *s = HC_GetSession( sock );
    if( s == nullptr )
        return false;

    CItem *houseItem = CalcItemObjFromSer( s->houseSerial );
    if( !ValidateObject( houseItem ))
        return false;

    CMultiObj *mMulti = FindMulti( houseItem );
    if( !ValidateObject( mMulti ))
        return false;

    if( !mMulti->IsOwner( chr ))
    {
        return false;
    }

    HC_DeleteLegacyCustomHouseItems( mMulti );
    HC_SaveSerializedDesignTiles( mMulti, s->tiles );
    s->revision = HC_BumpCommittedDesignRevision( mMulti );
	HC_RefreshHouseToClient( sock, houseItem, mMulti );

    return true;
}

void HC_LoadExistingCustomTiles( HouseCustomSession &s, CItem *houseItem, CMultiObj *mMulti )
{
    s.tiles.clear();

    if( !ValidateObject( houseItem ) || !ValidateObject( mMulti ))
        return;

    if( HC_LoadSerializedDesignTiles( mMulti, s.tiles ))
    {
        HC_DeleteLegacyCustomHouseItems( mMulti );
        return;
    }

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

    if( !s.tiles.empty() )
        HC_SaveSerializedDesignTiles( mMulti, s.tiles );

    HC_DeleteLegacyCustomHouseItems( mMulti );
}

static void HC_RemoveCustomizerFootStairs( HouseCustomSession &s, CChar *chr, CItem *houseItem, CMultiObj *mMulti )
{
    if( !ValidateObject( chr ) || !ValidateObject( houseItem ) || !ValidateObject( mMulti ))
        return;

    const SI16 relX = static_cast<SI16>( chr->GetX() - houseItem->GetX() );
    const SI16 relY = static_cast<SI16>( chr->GetY() - houseItem->GetY() );
    if( relX < -128 || relX > 127 || relY < -128 || relY > 127 )
        return;

    const SI08 x = static_cast<SI08>( relX );
    const SI08 y = static_cast<SI08>( relY );
    bool removed = false;

    for( auto it = s.tiles.begin(); it != s.tiles.end(); )
    {
        if( it->x == x && it->y == y && HC_IsStairComponentTile( it->id ))
        {
            it = s.tiles.erase( it );
            removed = true;
        }
        else
        {
            ++it;
        }
    }

    if( !removed )
        return;

    auto itemList = mMulti->GetItemsInMultiList();
    if( itemList == nullptr )
        return;

    std::vector<CItem*> toDelete;
    for( const auto &obj : itemList->collection() )
    {
        CItem *it = static_cast<CItem*>( obj );
        if( !ValidateObject( it ) || !IsCustomHouseItem( it ))
            continue;

        if( it->GetX() == chr->GetX() && it->GetY() == chr->GetY() && HC_IsStairComponentTile( it->GetId() ))
            toDelete.push_back( it );
    }

    for( auto it : toDelete )
    {
        if( ValidateObject( it ))
            it->Delete();
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

void HC_SendDesignState( CSocket *sock, const HouseCustomSession &s, bool enableResponse )
{
    if( sock == nullptr )
        return;

    sock->Send( &CPHouseDesignStateGeneral( s.houseSerial, s.revision ) );

    std::vector<HouseTileEntry> sendTiles;
    HC_BuildCombinedTiles( s, sendTiles );
    sock->Send( &CPHouseDesignStateDetailed( s.houseSerial, s.revision, sendTiles, enableResponse ) );
}

static bool HC_LoadFoundationMultiTiles( UI16 multiNum, std::vector<HouseTileEntry> &baseTiles )
{
    baseTiles.clear();

    if( !Map->MultiExists( multiNum ))
        return false;

    const auto& structure = Map->SeekMulti( multiNum );
    baseTiles.reserve( structure.items.size() );

    for( const auto &multiItem : structure.items )
    {
        if( multiItem.tileId == INVALIDID || !Map->IsValidTile( multiItem.tileId ))
            continue;

        if( multiItem.offsetX < -128 || multiItem.offsetX > 127 ||
            multiItem.offsetY < -128 || multiItem.offsetY > 127 ||
            multiItem.altitude < -128 || multiItem.altitude > 127 )
        {
            continue;
        }

        HouseTileEntry e;
        e.id = multiItem.tileId;
        e.x = static_cast<SI08>( multiItem.offsetX );
        e.y = static_cast<SI08>( multiItem.offsetY );
        e.z = static_cast<SI08>( multiItem.altitude );
        baseTiles.push_back( e );
    }

    return !baseTiles.empty();
}

static void GetFoundationGraphics( FoundationType type, UI16 &east, UI16 &south, UI16 &post, UI16 &corner )
{
    switch( type )
    {
        case FT_DarkWood:  corner=0x0014; east=0x0015; south=0x0016; post=0x0017; break;
        case FT_LightWood: corner=0x00BD; east=0x00BE; south=0x00BF; post=0x00C0; break;
        case FT_Dungeon:   corner=0x02FD; east=0x02FF; south=0x02FE; post=0x0300; break;
        case FT_Brick:     corner=0x0041; east=0x0043; south=0x0042; post=0x0044; break;
        case FT_Stone:     corner=0x0065; east=0x0064; south=0x0063; post=0x0066; break;
		default:           corner=0x0065; east=0x0064; south=0x0063; post=0x0066; break;
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

}

static void HC_RefreshHouseToClient( CSocket *sock, CItem *houseItem, CMultiObj *mMulti )
{
    if( sock == nullptr || !ValidateObject( houseItem ) || !ValidateObject( mMulti ))
        return;

    auto itemList = mMulti->GetItemsInMultiList();
    if( itemList != nullptr )
    {
        for( const auto &obj : itemList->collection() )
        {
            CItem *it = static_cast<CItem*>( obj );
            if( !ValidateObject( it ))
                continue;

            if( IsCustomHouseItem( it ))
                continue;

            // Ensure the client receives the item state again.
            // Many UOX3 objects have Update( sock ) or SendToSocket methods.
            // Use whichever exists in your codebase.
            it->Update( sock );
        }
    }

    houseItem->Update( sock );
}

void HC_RemoveAtXYZ( HouseCustomSession& s, SI08 x, SI08 y, SI08 z )
{
	for( auto it = s.tiles.begin(); it != s.tiles.end(); )
	{
		if( it->x == x && it->y == y && it->z == z )
        {
			it = s.tiles.erase( it );
        }
		else
			++it;
	}
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
