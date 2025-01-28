// houseCommands.js - v1.0.1
// Last Updated 2022-09-09
//
// The functions in this script are shared between spoken house commands (triggered in
// js/server/house/house.js attached to multi) and the buttons in the house menu gump triggered in
// js/server/house/housesign.js attached to sign
//
// By default, this script uses script ID 15002, setup in js/jse_fileassociations.scp, and is not
// directly attached to any in-game object, but used as a resource for other scripts.

// Check if tracking of house ownership is done on a per-account (instead of per-char) basis (from uox.ini)
const trackHousesPerAccount = GetServerSetting( "TrackHousesPerAccount" );

// Check if players can own and co-own houses at the same time (from uox.ini)
const canOwnAndCoOwnHouses = GetServerSetting( "CanOwnAndCoOwnHouses" );

// Fetch uox.ini setting for whether characters on same account as house owner will be
// treated as if they were co-owners of the house
const coOwnHousesOnSameAccount = GetServerSetting( "CoOwnHousesOnSameAccount" );

// Fetch max amount of houses someone can own/co-own (from uox.ini)
const maxHousesOwnable = GetServerSetting( "MaxHousesOwnable" );
const maxHousesCoOwnable = GetServerSetting( "MaxHousesCoOwnable" );

// Transfer house ownership - Trigger
function TransferOwnership( pSocket, iMulti )
{
	if( pSocket != null )
	{
		if( !ValidateObject( iMulti ))
		{
			pSocket.SysMessage( GetDictionaryEntry( 1820, pSocket.language )); // Unable to detect house! Try again, or contact a GM if problem persists.
			return;
		}

		var pUser = pSocket.currentChar;
		if( iMulti.IsOwner( pUser ) || pUser.isGM )
		{
			// Store iMulti in tempObj2 for later, when player has selected something with
			// targeting cursor - verify at the other end that tempObj2 still contains a multi,
			// and that it's the one the player owns!
			pSocket.tempObj = iMulti;

			// Find the sign belonging to the multi based on the serial of the sign stored in
			// the MORE property
			var iSign;
			if( iMulti.more == 0 )
			{
				// No serial stored in house's MORE value; we need to find the sign in some other way!
				AreaItemFunction( "FindHouseSign", pUser, 30, pSocket );
				iSign = pSocket.tempObj2;
			}
			else
			{
				var iSignSerial = iMulti.more;
				var serialPart1 = ( iSignSerial >> 24 );
				var serialPart2 = ( iSignSerial >> 16 );
				var serialPart3 = ( iSignSerial >> 8 );
				var serialPart4 = ( iSignSerial % 256 );
				iSign = CalcItemFromSer( serialPart1, serialPart2, serialPart3, serialPart4 );

				if( ValidateObject( iSign ))
				{
					pSocket.tempObj2 = iSign;
				}
			}

			if( ValidateObject( iSign ))
			{
				pUser.CustomTarget( 0, GetDictionaryEntry( 557, pSocket.language )); // Select person to transfer ownership to.
			}
			else
			{
				pSocket.SysMessage( GetDictionaryEntry( 1821, pSocket.language )); // Unable to detect house sign! Try again, or contact a GM if problem persists.
			}
		}
		else
		{
			pSocket.SysMessage( GetDictionaryEntry( 1822, pSocket.language )); // Only the primary house owner can transfer ownership of the house!
		}
	}
	else
	{
		Console.Error( "Invalid socket provided in TransferOwnership() function in houseCommands.js!" );
	}
}

// For houses that don't have a reference to the house sign stored in their MORE property,
// this will find any nearby house signs (within 30 tiles of multi center) and see if any of them
// has a reference to the multi itself
function FindHouseSign( pChar, itemToCheck, pSocket )
{
	var iMulti = pSocket.tempObj;
	if( ValidateObject( iMulti ) && ValidateObject( itemToCheck ) && itemToCheck.type == 203 )
	{
		if( itemToCheck.multi == iMulti )
		{
			// Found sign belonging to house! Let's fix the multi's reference to the sign while at it...
			iMulti.more = itemToCheck.serial;
			pSocket.tempObj2 = itemToCheck;
		}
	}
}

// Transfer house ownership - Callback
function onCallback0( pSocket, myTarget )
{
	var pUser = pSocket.currentChar;
	var iMulti = pSocket.tempObj;
	var iSign = pSocket.tempObj2;
	pSocket.tempObj = null;
	pSocket.tempObj2 = null;

	if( !ValidateObject( iMulti ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 1823, pSocket.language )); // You need to be inside a house to use house commands!
		return;
	}

	if(( !ValidateObject( iMulti.owner ) || iMulti.owner != pUser ) && !pUser.isGM )
	{
		pSocket.SysMessage( GetDictionaryEntry( 1822, pSocket.language )); // Only the primary house owner can transfer ownership of the house!
		return;
	}

	if(( !ValidateObject( iSign ) || ( !ValidateObject( iSign.owner ) && iSign.owner != pUser )) && !pUser.isGM )
	{
		pSocket.SysMessage( GetDictionaryEntry( 1821, pSocket.language )); // Unable to detect house sign! Try again, or contact a GM if problem persists.
		return;
	}

	var StrangeByte = pSocket.GetWord( 1 );
	if( StrangeByte === 0 && ValidateObject( myTarget ) && myTarget.isChar && !myTarget.npc && myTarget.online )
	{
		if( myTarget == pUser )
		{
			pSocket.SysMessage( GetDictionaryEntry( 1824, pSocket.language )); // You cannot transfer ownership of the house to yourself - you already own it!
		}
		else if( iMulti.IsOnBanList( myTarget ))
		{
			pSocket.SysMessage( GetDictionaryEntry( 1825, pSocket.language )); // You cannot transfer ownership of the house to someone on the banlist of the house!
		}
		else
		{
			// First, check if new owner is already at the cap for how many houses they can own!
			var housesOwned = myTarget.housesOwned;
			if( housesOwned >= maxHousesOwnable )
			{
				pSocket.SysMessage( GetDictionaryEntry( 1826, pSocket.language )); // That player cannot own any more houses!
				myTarget.SysMessage( GetDictionaryEntry( 1827, myTarget.socket.language ), housesOwned, maxHousesOwnable, (trackHousesPerAccount ? "account" : "character") ); // You already own %i houses, you may not place another (Max %i per %s)!
				return false;
			}

			// Next, check if they are allowed to own and co-own houses at the same time (and whether they actually co-own any houses)
			if( !canOwnAndCoOwnHouses )
			{
				var housesCoOwned = myTarget.housesCoOwned;
				if( housesCoOwned > 0 )
				{
					pSocket.SysMessage( GetDictionaryEntry( 1826, pSocket.language )); // That player cannot own any more houses!
					myTarget.SysMessage( GetDictionaryEntry( 1828, myTarget.socket.language ), housesCoOwned ); // You are already a co-owner of %i houses, and you cannot own and co-own houses at the same time!
					return false;
				}
			}

			myTarget.SetTag( "iMultiSerial", ( iMulti.serial ).toString() );
			if( ValidateObject( iMulti.owner ))
			{
				myTarget.SetTag( "oldMultiOwner", ( iMulti.owner.serial ).toString() );
			}
			else
			{
				myTarget.SetTag( "oldMultiOwner", null );
			}
			myTarget.SetTag( "iSignSerial", ( iSign.serial ).toString() );

			var confirmButtonID = 1;
			var confirmString = pUser.name + " " + GetDictionaryEntry( 2853, pSocket.language ) + "<BR><B>" + iMulti.name + "</B>"; // wants to transfer ownership of this house to you:
			ConfirmActionGump( pSocket, myTarget, confirmString, confirmButtonID )

			pSocket.SysMessage( GetDictionaryEntry( 1829, pSocket.language ), myTarget.name ); // House ownership transfer initiated, waiting for response from %s...
		}
	}
}

// Add co-owner - Trigger
function AddOwner( pSocket, iMulti )
{
	if( pSocket != null )
	{
		if( !ValidateObject( iMulti ))
		{
			pSocket.SysMessage( GetDictionaryEntry( 1820, pSocket.language )); // Unable to detect house! Try again, or contact a GM if problem persists.
			return;
		}

		if( iMulti.IsOwner( pSocket.currentChar ))
		{
			if( iMulti.owners < iMulti.maxOwners )
			{
				pSocket.tempObj = iMulti;
				pSocket.CustomTarget( 1, GetDictionaryEntry( 1830, pSocket.language )); // Select player to add as co-owner:
			}
			else
			{
				pSocket.SysMessage( GetDictionaryEntry( 1831, pSocket.language ));; // The co-owner list is full!
			}
		}
		else
		{
			pSocket.SysMessage( GetDictionaryEntry( 1832, pSocket.language )); // Only the primary house owner may add co-owners!
		}
	}
}

// Add co-owner - Callback
function onCallback1( pSocket, myTarget )
{
	var pUser = pSocket.currentChar;
	var iMulti = pSocket.tempObj;
	pSocket.tempObj = null;

	if( !ValidateObject( iMulti ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 1823, pSocket.language )); // You need to be inside a house to use house commands!
		return;
	}

	if( !iMulti.IsOwner( pUser ) && !pUser.isGM )
	{
		pSocket.SysMessage( GetDictionaryEntry( 1832, pSocket.language )); // Only the primary house owner may add co-owners!
		return;
	}

	var StrangeByte = pSocket.GetWord( 1 );
	if( StrangeByte === 0 && ValidateObject( myTarget ) && myTarget.isChar && !myTarget.npc && myTarget.online )
	{
		if( !iMulti.IsOnOwnerList( myTarget ))
		{
			if( !iMulti.IsOnBanList( myTarget ))
			{
				// Check that targeted player doesn't already exceed the allowed number of houses owned or co-owned
				if( canOwnAndCoOwnHouses )
				{
					var housesCoOwned = myTarget.housesCoOwned;
					if( housesCoOwned >= maxHousesCoOwnable )
					{
						pSocket.SysMessage( GetDictionaryEntry( 1826, pSocket.language )); // That player cannot own any more houses!
						myTarget.SysMessage( GetDictionaryEntry( 1833, myTarget.socket.language ), housesCoOwned, maxHousesCoOwnable ); // You are already a co-owner of %i (out of max %i) houses, and you cannot co-own any more houses!
						return false;
					}
				}
				else
				{
					// Check if player already owns another house before adding them to the co-owner list
					var housesOwned = myTarget.housesOwned;
					if( housesOwned > 0 )
					{
						pSocket.SysMessage( GetDictionaryEntry( 1826, pSocket.language )); // That player cannot own any more houses!
						myTarget.SysMessage( GetDictionaryEntry( 1834, myTarget.socket.language )); // You are already the owner of at least one house, and cannot co-own another house at the same time!
						return false;
					}
				}

				myTarget.SetTag( "iMultiSerial", (iMulti.serial).toString() );
				myTarget.SetTag( "oldMultiOwner", (iMulti.owner.serial).toString() );

				var confirmButtonID = 2;
				var confirmString = pUser.name + " " + GetDictionaryEntry( 2856, pSocket.language ) + "<BR><B>" + iMulti.name + "</B>"; // wants to add you as co-owner of this house:
				ConfirmActionGump( pSocket, myTarget, confirmString, confirmButtonID )
				pSocket.SysMessage( GetDictionaryEntry( 1835, pSocket.language ), myTarget.name ); // Adding %s as co-owner, waiting for response...
			}
			else
			{
				pSocket.SysMessage( GetDictionaryEntry( 1836, pSocket.language )); // You cannot add a player on the ban list as a co-owner!
			}
		}
		else
		{
			pSocket.SysMessage( GetDictionaryEntry( 1837, pSocket.language )); // That player is already a co-owner of your house!
		}
	}
	else
	{
		pSocket.SysMessage( GetDictionaryEntry( 1103, pSocket.language )); // Not a valid target!
	}
}

// Remove co-owner - Trigger
function RemoveOwnerTrigger( pSocket, iMulti )
{
	if( pSocket != null )
	{
		if( !ValidateObject( iMulti ))
		{
			pSocket.SysMessage( GetDictionaryEntry( 1820, pSocket.language )); // Unable to detect house! Try again, or contact a GM if problem persists.
			return;
		}

		if( iMulti.IsOwner( pSocket.currentChar ) || iMulti.IsOnOwnerList( pSocket.currentChar ))
		{
			pSocket.tempObj = iMulti;
			pSocket.CustomTarget( 2, GetDictionaryEntry( 1838, pSocket.language )); // Select player to remove as co-owner:
		}
		else
			pSocket.SysMessage( GetDictionaryEntry( 1839, pSocket.language )); // Only the primary house owner can remove co-owners!
	}
}

// Remove co-owner - Callback
function onCallback2( pSocket, myTarget )
{
	var pUser = pSocket.currentChar;
	var iMulti = pSocket.tempObj;
	pSocket.tempObj = null;

	if( !ValidateObject( iMulti ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 1823, pSocket.language )); // You need to be inside a house to use house commands!
		return;
	}

	if( !iMulti.IsOwner( pUser ) && ( !iMulti.IsOnOwnerList( pUser ) || myTarget != pUser ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 1840, pSocket.language )); // Only the primary house owner can remove other co-owners! Co-owners can only remove themselves.
		return;
	}

	var StrangeByte = pSocket.GetWord( 1 );
	if( StrangeByte === 0 && ValidateObject( myTarget ) && myTarget.isChar && !myTarget.npc && myTarget.online )
	{
		RemoveOwner( pSocket, myTarget, iMulti );
	}
	else
	{
		pSocket.SysMessage( GetDictionaryEntry( 1103, pSocket.language )); // Not a valid target!
	}
}

function RemoveOwner( pSocket, charToRemove, iMulti )
{
	if( !ValidateObject( iMulti ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 1820, pSocket.language )); // Unable to detect house! Try again, or contact a GM if problem persists.
		return;
	}

	if( !ValidateObject( charToRemove ) || !charToRemove.isChar || charToRemove.npc )
	{
		pSocket.SysMessage( GetDictionaryEntry( 1103, pSocket.language )); // Not a valid target!
		return;
	}

	if( iMulti.IsOnOwnerList( charToRemove )) // Is character to be removed from owner list actually on owner list?
	{
		if( !iMulti.IsOwner( charToRemove )) // Only proceed if character to be removed is not the primary house owner
		{
			var pUser = pSocket.currentChar;
			// Allow going forward with removal process if player is the house owner, OR if player is co-owner trying to remove themselves from
			// co-owner list!
			if( iMulti.IsOwner( pUser ) || ( iMulti.IsOnOwnerList( pUser ) && pUser == charToRemove ))
			{
				iMulti.RemoveFromOwnerList( charToRemove );

				if( !iMulti.isPublic && iMulti.IsInMulti( charToRemove ))
				{
					// Eject character from house
					EjectPlayerActual( iMulti, charToRemove );
				}

				// Remove specific player's keys!
				iMulti.KillKeys( charToRemove );
				pSocket.SysMessage( GetDictionaryEntry( 582, pSocket.language )); // Removing excess keys from player's pack.

				// Remove ex-co-owner's keys to the house. How to delete, specifically the keys owned by this guy and not by everyone else, though?
				if( iMulti.IsOwner( pUser ))
				{
					pSocket.SysMessage( GetDictionaryEntry( 1842, pSocket.language ), charToRemove.name ); // You have removed %s as a co-owner of your house! You might want to change the locks on your doors!
				}
				else
				{
					pSocket.SysMessage( GetDictionaryEntry( 1843, pSocket.language )); // You have removed yourself as co-owner of this house!
				}

				if( charToRemove.online && charToRemove != pUser )
				{
					charToRemove.socket.SysMessage( GetDictionaryEntry( 1844, charToRemove.socket.language ), iMulti.name ); // You have been removed as a co-owner of %s!
				}
			}
		}
		else
		{
			pSocket.SysMessage( GetDictionaryEntry( 1845, pSocket.language )); // You cannot remove the primary owner of the house as owner!
		}
	}
	else
	{
		pSocket.SysMessage( GetDictionaryEntry( 1846, pSocket.language )); // That player is not a co-owner of your house!
	}
}

// Add friend - Trigger
function AddFriend( pSocket, iMulti )
{
	if( pSocket != null )
	{
		if( !ValidateObject( iMulti ))
		{
			pSocket.SysMessage( GetDictionaryEntry( 1820, pSocket.language )); // Unable to detect house! Try again, or contact a GM if problem persists.
			return;
		}

		var pUser = pSocket.currentChar;
		if( pUser.isGM || ( iMulti.IsOnOwnerList( pUser ) || ( coOwnHousesOnSameAccount && ValidateObject( iMulti.owner ) && iMulti.owner.accountNum == pUser.accountNum )))
		{
			if( iMulti.friends < iMulti.maxFriends )
			{
				pSocket.tempObj = iMulti;
				pSocket.CustomTarget( 3, GetDictionaryEntry( 561, pSocket.language )); // Select person to make a friend of the house.
			}
			else
			{
				pSocket.SysMessage( GetDictionaryEntry( 1847, pSocket.language )); // The friends list is full!
			}
		}
		else
		{
			pSocket.SysMessage( GetDictionaryEntry( 1848, pSocket.language )); // Only house owners and co-owners can add someone to the friend list!
		}
	}
}

// Add friend - Callback
function onCallback3( pSocket, myTarget )
{
	var pUser = pSocket.currentChar;
	var iMulti = pSocket.tempObj;
	pSocket.tempObj = null;

	if( !ValidateObject( iMulti ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 1823, pSocket.language )); // You need to be inside a house to use house commands!
		return;
	}

	if( !iMulti.IsOnOwnerList( pUser ))
	{
		if( !pUser.isGM && ( !coOwnHousesOnSameAccount || ( coOwnHousesOnSameAccount && ( !ValidateObject( iMulti.owner ) || iMulti.owner.accountNum != pUser.accountNum ))))
		{
			pSocket.SysMessage( GetDictionaryEntry( 1848, pSocket.language )); // Only house owners and co-owners can add someone to the friend list!
			return;
		}
	}

	var StrangeByte = pSocket.GetWord( 1 );
	if( StrangeByte === 0 && ValidateObject( myTarget ) && myTarget.isChar && !myTarget.npc && myTarget.online )
	{
		if( !iMulti.IsOnFriendList( myTarget ))
		{
			if( !iMulti.IsOnBanList( myTarget ))
			{
				if( !iMulti.IsOnOwnerList( myTarget ))
				{
					 iMulti.AddToFriendList( myTarget );

					 // 1088=%s has been made a friend of the house.
					 pUser.SysMessage( GetDictionaryEntry( 1088, pSocket.language ), myTarget.name ); //%s has been made a friend of the house.
					 if( myTarget.online )
					 {
					 	myTarget.SysMessage( GetDictionaryEntry( 1089, myTarget.socket.language )); // You have been made a friend of the house.
					 }
				}
				else
				{
					pSocket.SysMessage( GetDictionaryEntry( 1849, pSocket.language )); // You cannot add a owner or co-owner of the house to the friend list!
				}
			}
			else
			{
				pSocket.SysMessage( GetDictionaryEntry( 1850, pSocket.language )); // You cannot add a player on the ban list as a friend of the house!
			}
		}
		else
		{
			pSocket.SysMessage( GetDictionaryEntry( 1851, pSocket.language )); // That player is already on the friend list!
		}
	}
	else
	{
		pSocket.SysMessage( GetDictionaryEntry( 1103, pSocket.language )); // Not a valid target!
	}
}

// Remove friend - Trigger
function RemoveFriendTrigger( pSocket, iMulti )
{
	if( pSocket != null )
	{
		if( !ValidateObject( iMulti ))
		{
			pSocket.SysMessage( GetDictionaryEntry( 1820, pSocket.language )); // Unable to detect house! Try again, or contact a GM if problem persists.
			return;
		}

		var pUser = pSocket.currentChar;
		if( pUser.isGM || ( iMulti.IsOnOwnerList( pUser ) || ( coOwnHousesOnSameAccount && ValidateObject( iMulti.owner ) && iMulti.owner.accountNum == pUser.accountNum )))
		{
			pSocket.tempObj = iMulti;
			pSocket.CustomTarget( 4, GetDictionaryEntry( 1852, pSocket.language )); // Select player to remove as friend:
		}
		else
		{
			pSocket.SysMessage( GetDictionaryEntry( 1853, pSocket.language )); // Only house owners and co-owners can add remove someone from the friend list!
		}
	}
}

// Remove friend - Callback
function onCallback4( pSocket, myTarget )
{
	var pUser = pSocket.currentChar;
	var iMulti = pSocket.tempObj;
	pSocket.tempObj = null;

	if( !ValidateObject( iMulti ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 1823, pSocket.language )); // You need to be inside a house to use house commands!
		return;
	}

	if( !iMulti.IsOnOwnerList( pUser ))
	{
		if( !pUser.isGM && ( !coOwnHousesOnSameAccount || ( coOwnHousesOnSameAccount && ( !ValidateObject( iMulti.owner ) || iMulti.owner.accountNum != pUser.accountNum ))))
		{
			pSocket.SysMessage( GetDictionaryEntry( 1853, pSocket.language )); // You need to be inside a house to use house commands!
			return;
		}
	}

	var StrangeByte = pSocket.GetWord( 1 );
	if( StrangeByte === 0 && ValidateObject( myTarget ) && myTarget.isChar && !myTarget.npc && myTarget.online )
	{
		RemoveFriend( pSocket, myTarget, iMulti );
	}
	else
	{
		pSocket.SysMessage( GetDictionaryEntry( 1103, pSocket.language )); // Not a valid target!
	}
}

function RemoveFriend( pSocket, charToRemove, iMulti )
{
	if( !ValidateObject( iMulti ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 1820, pSocket.language )); // Unable to detect house! Try again, or contact a GM if problem persists.
		return;
	}

	if( !ValidateObject( charToRemove ) || !charToRemove.isChar || charToRemove.npc )
	{
		pSocket.SysMessage( GetDictionaryEntry( 1103, pSocket.language )); // Not a valid target!
		return;
	}

	if( iMulti.IsOnFriendList( charToRemove ))
	{
		iMulti.RemoveFromFriendList( charToRemove );

		if( !iMulti.isPublic && iMulti.IsInMulti( charToRemove ))
		{
			// Eject character from house
			EjectPlayerActual( iMulti, charToRemove );
		}

		pSocket.SysMessage( GetDictionaryEntry( 1854, pSocket.language ), charToRemove.name ); // You have removed %s from the friend list of this house!

		if( charToRemove.online )
		{
			charToRemove.socket.SysMessage( GetDictionaryEntry( 1855, charToRemove.socket.language ), iMulti.name ); // You have been removed as a friend of %s!
		}
	}
	else
	{
		pSocket.SysMessage( GetDictionaryEntry( 1856, pSocket.language )); // That player is not on the friend list!
	}
}

// Add guest - Trigger
function AddGuest( pSocket, iMulti )
{
	if( pSocket != null )
	{
		if( !ValidateObject( iMulti ))
		{
			pSocket.SysMessage( GetDictionaryEntry( 1820, pSocket.language )); // Unable to detect house! Try again, or contact a GM if problem persists.
			return;
		}

		var pUser = pSocket.currentChar;
		if( pUser.isGM || ( iMulti.IsOnOwnerList( pUser ) || ( coOwnHousesOnSameAccount && ValidateObject( iMulti.owner ) && iMulti.owner.accountNum == pUser.accountNum )))
		{
			if( iMulti.guests < iMulti.maxGuests )
			{
				pSocket.tempObj = iMulti;
				pSocket.CustomTarget( 11, GetDictionaryEntry( 1857, pSocket.language )); // Select player to add as guest:
			}
			else
			{
				pSocket.SysMessage( GetDictionaryEntry( 1858, pSocket.language )); // The guest list is full!
			}
		}
		else
		{
			pSocket.SysMessage( GetDictionaryEntry( 1859, pSocket.language )); // Only house owners and co-owners can add someone to the guest list!
		}
	}
}

// Add guest - Callback
function onCallback11( pSocket, myTarget )
{
	var pUser = pSocket.currentChar;
	var iMulti = pSocket.tempObj;
	pSocket.tempObj = null;

	if( !ValidateObject( iMulti ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 1823, pSocket.language )); // You need to be inside a house to use house commands!
		return;
	}

	if( !iMulti.IsOnOwnerList( pUser ))
	{
		if( !pUser.isGM && ( !coOwnHousesOnSameAccount || ( coOwnHousesOnSameAccount && ( !ValidateObject( iMulti.owner ) || iMulti.owner.accountNum != pUser.accountNum ))))
		{
			pSocket.SysMessage( GetDictionaryEntry( 1859, pSocket.language )); // Only house owners and co-owners can add someone to the guest list!
			return;
		}
	}

	var StrangeByte = pSocket.GetWord( 1 );
	if( StrangeByte === 0 && ValidateObject( myTarget ) && myTarget.isChar && !myTarget.npc && myTarget.online )
	{
		if( !iMulti.IsOnGuestList( myTarget ))
		{
			if( !iMulti.IsOnFriendList( myTarget ))
			{
				if( !iMulti.IsOnBanList( myTarget ))
				{
					if( !iMulti.IsOnOwnerList( myTarget ))
					{
						iMulti.AddToGuestList( myTarget );
						pUser.SysMessage( GetDictionaryEntry( 1860, pSocket.language ), myTarget.name ); // You have added %s to the guest list for this house.
						if( myTarget.online )
						{
							myTarget.SysMessage( GetDictionaryEntry( 1861, myTarget.socket.language ), pUser.name, iMulti.name ); // You have been added by %s as a guest of %s
						}
					}
					else
					{
						pSocket.SysMessage( GetDictionaryEntry( 1862, pSocket.language )); // You cannot add a owner or co-owner of the house to the guest list!
					}
				}
				else
				{
					pSocket.SysMessage( GetDictionaryEntry( 1863, pSocket.language )); // You cannot add a player on the ban list as a guest of the house!
				}
			}
			else
			{
				pSocket.SysMessage( GetDictionaryEntry( 1864, pSocket.language )); // You cannot add a player on the friend list as a guest of the house!
			}
		}
		else
		{
			pSocket.SysMessage( GetDictionaryEntry( 1865, pSocket.language )); // That player is already on the guest list!
		}
	}
	else
	{
		pSocket.SysMessage( GetDictionaryEntry( 1103, pSocket.language )); // Not a valid target!
	}
}

// Remove guest - Trigger
function RemoveGuestTrigger( pSocket, iMulti )
{
	if( pSocket != null )
	{
		if( !ValidateObject( iMulti ))
		{
			pSocket.SysMessage( GetDictionaryEntry( 1820, pSocket.language )); // Unable to detect house! Try again, or contact a GM if problem persists.
			return;
		}

		var pUser = pSocket.currentChar;
		if( pUser.isGM || ( iMulti.IsOnOwnerList( pUser ) || ( coOwnHousesOnSameAccount && ValidateObject( iMulti.owner ) && iMulti.owner.accountNum == pUser.accountNum )))
		{
			pSocket.tempObj = iMulti;
			pSocket.CustomTarget( 12, GetDictionaryEntry( 1866, pSocket.language )); // Select player to remove as guest:
		}
		else
		{
			pSocket.SysMessage( GetDictionaryEntry( 1867, pSocket.language )); // Only house owners and co-owners can remove someone from the guest list!
		}
	}
}

// Remove guest - Callback
function onCallback12( pSocket, myTarget )
{
	var pUser = pSocket.currentChar;
	var iMulti = pSocket.tempObj;
	pSocket.tempObj = null;

	if( !ValidateObject( iMulti ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 1823, pSocket.language )); // You need to be inside a house to use house commands!
		return;
	}

	if( !iMulti.IsOnOwnerList( pUser ))
	{
		if( !pUser.isGM && ( !coOwnHousesOnSameAccount || ( coOwnHousesOnSameAccount && ( !ValidateObject( iMulti.owner ) || iMulti.owner.accountNum != pUser.accountNum ))))
		{
			pSocket.SysMessage( GetDictionaryEntry( 1867, pSocket.language )); // Only house owners and co-owners can remove someone from the guest list!
			return;
		}
	}

	var StrangeByte = pSocket.GetWord( 1 );
	if( StrangeByte === 0 && ValidateObject( myTarget ) && myTarget.isChar && !myTarget.npc && myTarget.online )
	{
		RemoveGuest( pSocket, myTarget, iMulti );
	}
	else
	{
		pSocket.SysMessage( GetDictionaryEntry( 1103, pSocket.language )); // Not a valid target!
	}
}

function RemoveGuest( pSocket, charToRemove, iMulti )
{
	if( !ValidateObject( iMulti ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 1820, pSocket.language )); // Unable to detect house! Try again, or contact a GM if problem persists.
		return;
	}

	if( !ValidateObject( charToRemove ) || !charToRemove.isChar || charToRemove.npc )
	{
		pSocket.SysMessage( GetDictionaryEntry( 1103, pSocket.language )); // Not a valid target!
		return;
	}

	if( iMulti.IsOnGuestList( charToRemove ))
	{
		iMulti.RemoveFromGuestList( charToRemove );

		if( !iMulti.isPublic && iMulti.IsInMulti( charToRemove ))
		{
			// Eject character from house
			EjectPlayerActual( iMulti, charToRemove );
		}

		pSocket.SysMessage( GetDictionaryEntry( 1868, pSocket.language ), charToRemove.name ); // You have removed %s from the guest list of this house!

		if( charToRemove.online )
		{
			charToRemove.socket.SysMessage( GetDictionaryEntry( 1869, charToRemove.socket.language ), iMulti.nam ); // You have been removed as a guest of %s!
		}
	}
	else
	{
		pSocket.SysMessage( GetDictionaryEntry( 1870, pSocket.language )); // That player is not on the guest list!
	}
}

function BanPlayer( pSocket, iMulti )
{
	if( pSocket != null )
	{
		if( !ValidateObject( iMulti ))
		{
			pSocket.SysMessage( GetDictionaryEntry( 1820, pSocket.language )); // Unable to detect house! Try again, or contact a GM if problem persists.
			return;
		}

		var pUser = pSocket.currentChar;
		if( pUser.isGM || ( iMulti.IsOnFriendList( pUser ) || iMulti.IsOnOwnerList( pUser ) || ( coOwnHousesOnSameAccount && ValidateObject( iMulti.owner ) && iMulti.owner.accountNum == pUser.accountNum )))
		{
			pSocket.tempObj = iMulti;
			pSocket.CustomTarget( 5, GetDictionaryEntry( 585, pSocket.language ), 1 ); // Select person to ban from the house.
		}
		else
		{
			pSocket.SysMessage( GetDictionaryEntry( 1871, pSocket.language )); // Only house owners, co-owners and friends can ban someone from a house!
		}
	}
}

// Add player to ban list
function onCallback5( pSocket, myTarget )
{
	var pUser = pSocket.currentChar;
	var iMulti = pSocket.tempObj;
	pSocket.tempObj = null;

	if( !ValidateObject( iMulti ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 1823, pSocket.language )); // You need to be inside a house to use house commands!
		return;
	}

	if( !iMulti.IsOnFriendList( pUser ) && !iMulti.IsOnOwnerList( pUser ))
	{
		if( !pUser.isGM && ( !coOwnHousesOnSameAccount || ( coOwnHousesOnSameAccount && ( !ValidateObject( iMulti.owner ) || iMulti.owner.accountNum != pUser.accountNum ))))
		{
			pSocket.SysMessage( GetDictionaryEntry( 1871, pSocket.language )); // Only house owners, co-owners and friends can ban someone from a house!
			return;
		}
	}

	var StrangeByte = pSocket.GetWord( 1 );
	if( StrangeByte === 0 && ValidateObject( myTarget ) && myTarget.isChar && !myTarget.npc && myTarget.online )
	{
		if( !iMulti.IsOnBanList( myTarget ))
		{
			if( !iMulti.IsOnOwnerList( myTarget ))
			{
				// Check if ban list can hold more people, but either way, continue to the
				// ejection from house part!
				if( iMulti.bans < iMulti.maxBans )
				{
					iMulti.AddToBanList( myTarget );
				}
				else
				{
					pSocket.SysMessage( GetDictionaryEntry( 1872, pSocket.language )); // The ban-list is full!
				}

				if( iMulti.IsInMulti( myTarget ))
				{
					// Eject character from house
					EjectPlayerActual( iMulti, myTarget );
				}

				// 1085=%s has been banned from this house.
				pSocket.SysMessage( GetDictionaryEntry( 1085, pSocket.language ), myTarget.name ); // %s has been banned from this house.
				if( myTarget.online )
				{
					myTarget.SysMessage( GetDictionaryEntry( 1873, myTarget.socket.language ), iMulti.name ); // You have been banned from %s
				}
			}
			else
			{
				pSocket.SysMessage( GetDictionaryEntry( 1874, pSocket.language )); // You cannot add a house owner to the ban list!
			}
		}
		else
		{
			pSocket.SysMessage( GetDictionaryEntry( 1875, pSocket.language )); // That player is already on the ban list!
		}
	}
	else
	{
		pSocket.SysMessage( GetDictionaryEntry( 1103, pSocket.language )); // Not a valid target!
	}
}

function UnbanPlayerTrigger( pSocket, iMulti )
{
	if( pSocket != null )
	{
		if( !ValidateObject( iMulti ))
		{
			pSocket.SysMessage( GetDictionaryEntry( 1820, pSocket.language )); // Unable to detect house! Try again, or contact a GM if problem persists.
			return;
		}

		var pUser = pSocket.currentChar;
		if( pUser.isGM || ( iMulti.IsOnFriendList( pUser ) || iMulti.IsOnOwnerList( pUser ) || ( coOwnHousesOnSameAccount && ValidateObject( iMulti.owner ) && iMulti.owner.accountNum == pUser.accountNum )))
		{
			pSocket.tempObj = iMulti;
			pSocket.CustomTarget( 6, GetDictionaryEntry( 1877, pSocket.language )); // Select player to lift ban for:
		}
		else
		{
			pSocket.SysMessage( GetDictionaryEntry( 1878, pSocket.language )); // Only house owners, co-owners and friends can lift a ban on someone in a house!
		}
	}
}

// Unban player
function onCallback6( pSocket, myTarget )
{
	var pUser = pSocket.currentChar;
	var iMulti = pSocket.tempObj;
	pSocket.tempObj = null;

	if( !ValidateObject( iMulti ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 1823, pSocket.language )); // You need to be inside a house to use house commands!
		return;
	}

	if( !iMulti.IsOnFriendList( pUser ) && !iMulti.IsOnOwnerList( pUser ))
	{
		if( !pUser.isGM && ( !coOwnHousesOnSameAccount || ( coOwnHousesOnSameAccount && ( !ValidateObject( iMulti.owner ) || iMulti.owner.accountNum != pUser.accountNum ))))
		{
			pSocket.SysMessage( GetDictionaryEntry( 1878, pSocket.language )); // Only house owners, co-owners and friends can lift a ban on someone in a house!
			return;
		}
	}

	var StrangeByte = pSocket.GetWord( 1 );
	if( StrangeByte === 0 && ValidateObject( myTarget ) && myTarget.isChar && !myTarget.npc && myTarget.online )
	{
		UnbanPlayer( pSocket, myTarget, iMulti );
	}
	else
	{
		pSocket.SysMessage( GetDictionaryEntry( 1103, pSocket.language )); // Not a valid target!
	}
}

function UnbanPlayer( pSocket, charToRemove, iMulti )
{
	if( !ValidateObject( iMulti ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 1820, pSocket.language )); // Unable to detect house! Try again, or contact a GM if problem persists.
		return;
	}

	if( !ValidateObject( charToRemove ) || !charToRemove.isChar || charToRemove.npc )
	{
		pSocket.SysMessage( GetDictionaryEntry( 1103, pSocket.language )); // Not a valid target!
		return;
	}

	if( iMulti.IsOnBanList( charToRemove ))
	{
		iMulti.RemoveFromBanList( charToRemove );
		pSocket.SysMessage( GetDictionaryEntry( 1879, pSocket.language ), charToRemove.name ); // You have lifted the ban on %s for your house.
	}
	else
	{
		pSocket.SysMessage( GetDictionaryEntry( 1880, pSocket.language )); // That player is not on the ban list!
	}
}

function EjectPlayer( pSocket, iMulti )
{
	if( pSocket != null )
	{
		if( !ValidateObject( iMulti ))
		{
			pSocket.SysMessage( GetDictionaryEntry( 1820, pSocket.language )); // Unable to detect house! Try again, or contact a GM if problem persists.
			return;
		}

		var pUser = pSocket.currentChar;
		if( pUser.isGM || ( iMulti.IsOnOwnerList( pUser ) || iMulti.IsOnFriendList( pUser ) || ( coOwnHousesOnSameAccount && ValidateObject( iMulti.owner ) && iMulti.owner.accountNum == pUser.accountNum )))
		{
			pSocket.tempObj = iMulti;
			pSocket.CustomTarget( 7, GetDictionaryEntry( 587, pSocket.language ), 1 ); // Select person to eject from the house.
		}
		else
		{
			pSocket.SysMessage( GetDictionaryEntry( 1881, pSocket.language )); // Only house owners, co-owners and friends can eject someone from a house!
		}
	}
}

// Eject player from house
function onCallback7( pSocket, myTarget )
{
	var pUser = pSocket.currentChar;
	var iMulti = pSocket.tempObj;
	pSocket.tempObj = null;

	if( !ValidateObject( iMulti ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 1823, pSocket.language )); // You need to be inside a house to use house commands!
		return;
	}

	if( !iMulti.IsOnFriendList( pUser ) && !iMulti.IsOnOwnerList( pUser ))
	{
		if( !pUser.isGM && ( !coOwnHousesOnSameAccount || ( coOwnHousesOnSameAccount && ( !ValidateObject( iMulti.owner ) || iMulti.owner.accountNum != pUser.accountNum ))))
		{
			pSocket.SysMessage( GetDictionaryEntry( 1881, pSocket.language )); // Only house owners, co-owners and friends can eject someone from a house!
			return;
		}
	}

	var StrangeByte = pSocket.GetWord( 1 );
	if( StrangeByte === 0 && ValidateObject( myTarget ) && myTarget.isChar && !myTarget.npc && myTarget.online )
	{
		if( myTarget == pUser )
		{
			pSocket.SysMessage( GetDictionaryEntry( 1882, pSocket.language )); // You cannot eject yourself from your own house!
			return;
		}

		// Eject player to SE corner of house
		if( iMulti.IsInMulti( myTarget ))
		{
			if( !iMulti.IsOnOwnerList( myTarget ) || myTarget == pUser )
			{
				// Eject character from house
				EjectPlayerActual( iMulti, myTarget );
				pSocket.SysMessage( GetDictionaryEntry( 1883, pSocket.language ), myTarget.name ); // You have ejected %s from your house!
			}
			else
			{
				pSocket.SysMessage( GetDictionaryEntry( 1884, pSocket.language )); // You cannot eject a house owner from the house!
			}
		}
		else
		{
			pSocket.SysMessage( GetDictionaryEntry( 1885, pSocket.language )); // You can only eject someone who's in your house!
		}
	}
	else
	{
		pSocket.SysMessage( GetDictionaryEntry( 1886, pSocket.language )); // You cannot eject that from your house!
	}
}

function EjectPlayerActual( iMulti, charToEject )
{
	if( ValidateObject( iMulti ) && ValidateObject( charToEject ))
	{
		// Get ban location for multi, as well as map elevation at corner of house
		// to ensure ejected object is placed at correct height
		var multiCornerZ = 0;
		var banX = iMulti.banX;
		var banY = iMulti.banY;
		if( banX == 0 && banY == 0 )
		{
			// Fetch coordinates for corner 3 (SE) of multi and use that as ban location
			var multiCornerCoords = (iMulti.GetMultiCorner( 3 )).split( "," );
			banX = parseInt(multiCornerCoords[0]);
			banY = parseInt(multiCornerCoords[1]);
		}

		multiCornerZ = GetMapElevation( banX, banY, iMulti.worldnumber );
		if( multiCornerZ < iMulti.z )
		{
			multiCornerZ = iMulti.z;
		}

		charToEject.Teleport( banX, banY, multiCornerZ );
		if( charToEject.online )
		{
			charToEject.socket.walkSequence = -1;
		}
	}
}

// Cannot lock down:
// - Doors, and locked doors
// - Temporary objects like field spell effects
// - House signs
// - Something already locked down or secure
function LockdownItem( pSocket, iMulti )
{
	if( pSocket != null )
	{
		if( !ValidateObject( iMulti ))
		{
			pSocket.SysMessage( GetDictionaryEntry( 1820, pSocket.language )); // Unable to detect house! Try again, or contact a GM if problem persists.
			return;
		}

		var pUser = pSocket.currentChar;
		if( pUser.isGM || ( iMulti.IsOnOwnerList( pUser ) || iMulti.IsOnFriendList( pUser ) || ( coOwnHousesOnSameAccount && ValidateObject( iMulti.owner ) && iMulti.owner.accountNum == pUser.accountNum )))
		{
			pSocket.tempObj = iMulti;
			pSocket.CustomTarget( 8, GetDictionaryEntry( 589, pSocket.language )); // Select item to lock down
		}
		else
		{
			pSocket.SysMessage( GetDictionaryEntry( 1887, pSocket.language )); // Only house owners, co-owners and friends can lock down items in a house!
		}
	}
}

// Lockdown Item
function onCallback8( pSocket, myTarget )
{
	var pUser = pSocket.currentChar;
	var iMulti = pSocket.tempObj;
	pSocket.tempObj = null;

	if( !ValidateObject( iMulti ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 1823, pSocket.language )); // You need to be inside a house to use house commands!
		return;
	}

	if( !iMulti.IsOnFriendList( pUser ) && !iMulti.IsOnOwnerList( pUser ))
	{
		if( !pUser.isGM && ( !coOwnHousesOnSameAccount || ( coOwnHousesOnSameAccount && ( !ValidateObject( iMulti.owner ) || iMulti.owner.accountNum != pUser.accountNum ))))
		{
			pSocket.SysMessage( GetDictionaryEntry( 1887, pSocket.language )); // Only house owners, co-owners and friends can lock down items in a house!
			return;
		}
	}

	var StrangeByte = pSocket.GetWord( 1 );
	if( StrangeByte === 0 && ValidateObject( myTarget ) && myTarget.isItem )
	{
		if( myTarget.type == 87 )
		{
			pSocket.SysMessage( GetDictionaryEntry( 1888, pSocket.language )); // You cannot lock down trash barrels.
		}
		else if( myTarget.movable == 3 )
		{
			pSocket.SysMessage( GetDictionaryEntry( 1889, pSocket.language )); // That is already locked down!
		}
		else if( myTarget.type == 12 || myTarget.type == 13 || myTarget.type == 203 || myTarget.id == 0x0BD2 ||
			myTarget.movable == 2 || myTarget.movable == 3 || myTarget.id == 0x3996 || myTarget.id == 0x398C ||
			myTarget.id == 0x3915 || myTarget.id == 0x3920 || myTarget.id == 0x3979 || myTarget.id == 0x3967 ||
			myTarget.id == 0x3956 || myTarget.id == 0x3946 )
		{
			// Don't allow locking down field spells, doors, signs, or items that for some reason are not movable by the player
			// 1106=You can't lock that down!
			pSocket.SysMessage( GetDictionaryEntry( 1106, pSocket.language )); // You can't lock that down!
		}
		else if( iMulti.lockdowns < iMulti.maxLockdowns )
		{
			if( myTarget.container != null )
			{
				var targetContainer = myTarget.container;
				if( !ValidateObject( targetContainer ))
				{
					pSocket.SysMessage( GetDictionaryEntry( 1891, pSocket.language )); // Something is wrong - container of item seems to be no longer valid!
					return;
				}

				var targetRootContainer = FindRootContainer( targetContainer, 0 );
				if( targetContainer.movable != 3 || ( ValidateObject( targetRootContainer) && !iMulti.IsInMulti( targetRootContainer )))
				{
					pSocket.SysMessage( GetDictionaryEntry( 1892, pSocket.language )); // You cannot lock down items in a container that's not locked down in your house!
					return;
				}
				else if( iMulti.IsSecureContainer( targetContainer ) || ( ValidateObject( targetRootContainer ) && iMulti.IsSecureContainer( targetRootContainer )))
				{
					pSocket.SysMessage( GetDictionaryEntry( 1893, pSocket.language )); // You don't need to lock down items inside secure containers!
					return;
				}
			}
			else if( !iMulti.IsInMulti( myTarget ))
			{
				pSocket.SysMessage( GetDictionaryEntry( 1105, pSocket.language )); // You can't lock something down that's not in your house!
				return;
			}

			// Check if item blocks movement and is too close to a door
			// 6 = TF_BLOCKING
			if( CheckTileFlag( myTarget.id, 6 ))
			{
				// Check for nearby doors
				var foundDoor = AreaItemFunction( "CheckForNearbyDoors", myTarget, 3, pSocket );
				if( foundDoor )
				{
					pSocket.SysMessage( GetDictionaryEntry( 1890, pSocket.language )); // You cannot lock down blocking items adjacent to a door.
					return;
				}
			}

			// Lock down item
			iMulti.LockDownItem( myTarget );

			// Set player who locked down the item as that item's owner, so we know who will be able
			// to release it (other than primary house owner) later!
			myTarget.owner = pUser;

			var lockdownsLeft = iMulti.maxLockdowns - iMulti.lockdowns;
			pSocket.SysMessage( GetDictionaryEntry( 1894, pSocket.language ), lockdownsLeft ); // You lockdown the item (%i lockdown(s) remaining).
		}
		else
		{
			pSocket.SysMessage( GetDictionaryEntry( 1895, pSocket.language )); // This house cannot have any additional lockdowns!
		}
	}
	else
	{
		pSocket.SysMessage( GetDictionaryEntry( 1108, pSocket.language )); // Invalid item!
	}
}

function CheckForNearbyDoors( myTarget, itemToCheck, pSocket )
{
	if( ValidateObject( itemToCheck ))
	{
		if(( itemToCheck.type == 12 || itemToCheck.type == 13 ))
		{
			//if( itemToCheck.z > myTarget.z && itemToCheck.z - myTarget.z > GetTileHeight( myTarget ))
			if( itemToCheck.z > myTarget.z && itemToCheck.z - myTarget.z >= 20 )
			{
				// Ignore doors on floors above
				return false;
			}
			else if( itemToCheck.z < myTarget.z && myTarget.z - itemToCheck.z >= 20 )
			{
				// Ignore doors on floors below, too!
				return false;
			}

			if( itemToCheck.isDoorOpen )
			{
				// Make sure to check against the distance from the door in it's closed state, rather than it's open state!
				var origX = itemToCheck.x  - itemToCheck.GetTag( "DOOR_X" );
				var origY = itemToCheck.y  - itemToCheck.GetTag( "DOOR_Y" );
				if( myTarget.x - origX < 2 || origX - myTarget.x < 2 || myTarget.y - origY < 2 || origY - myTarget.y < 2 )
				{
					return true;
				}
			}

			if( myTarget.DistanceTo( itemToCheck ) <= 2 )
			{
				return true;
			}
		}
	}
	return false;
}

function ReleaseItem( pSocket, iMulti )
{
	if( pSocket != null )
	{
		if( !ValidateObject( iMulti ))
		{
			pSocket.SysMessage( GetDictionaryEntry( 1820, pSocket.language )); // Unable to detect house! Try again, or contact a GM if problem persists.
			return;
		}

		var pUser = pSocket.currentChar;
		if( pUser.isGM || ( iMulti.IsOnOwnerList( pUser ) || iMulti.IsOnFriendList( pUser ) || ( coOwnHousesOnSameAccount && ValidateObject( iMulti.owner ) && iMulti.owner.accountNum == pUser.accountNum )))
		{
			pSocket.tempObj = iMulti;
			pSocket.CustomTarget( 9, GetDictionaryEntry( 591, pSocket.language )); // Select item to release.
		}
		else
		{
			pSocket.SysMessage( GetDictionaryEntry( 1896, pSocket.language )); // Only house owners, co-owners and friends can release locked down items in a house!
		}
	}
}

// Release Item
function onCallback9( pSocket, myTarget )
{
	var pUser = pSocket.currentChar;
	var iMulti = pSocket.tempObj;
	pSocket.tempObj = null;

	if( !ValidateObject( iMulti ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 1823, pSocket.language )); // You need to be inside a house to use house commands!
		return;
	}

	if( !iMulti.IsOnFriendList( pUser ) && !iMulti.IsOnOwnerList( pUser ))
	{
		if( !pUser.isGM && ( !coOwnHousesOnSameAccount || ( coOwnHousesOnSameAccount && ( !ValidateObject( iMulti.owner ) || iMulti.owner.accountNum != pUser.accountNum ))))
		{
			pSocket.SysMessage( GetDictionaryEntry( 1896, pSocket.language )); // Only house owners, co-owners and friends can release locked down items in a house!
			return;
		}
	}

	var StrangeByte = pSocket.GetWord( 1 );
	if( StrangeByte === 0 && ValidateObject( myTarget ) && myTarget.isItem )
	{
		if( myTarget.movable != 3 )
		{
			// 1785=That item is not currently locked down.
			pSocket.SysMessage( GetDictionaryEntry( 1897, pSocket.language )); // That item is not locked down!
		}
		else // if( myTarget.movable == 3 && myTarget.type != 87 && myTarget.multi == iMulti )
		{
			// Item doesn't appear to be in the multi.. maybe it's inside a container in the multi?
			if( myTarget.multi != iMulti && !iMulti.IsInMulti( myTarget ))
			{
				var targetContainer = myTarget.container;
				if( !ValidateObject( targetContainer ))
					return;

				var targetRootContainer = FindRootContainer( targetContainer, 0 );
				if( targetContainer.movable != 3 || ( ValidateObject( targetRootContainer ) && !iMulti.IsInMulti( targetRootContainer )))
				{
					pSocket.SysMessage( GetDictionaryEntry( 1109, pSocket.language )); // You can't release something that's not in your house!
					return;
				}
			}

			// Target item is a container that contains items
			if( myTarget.itemsinside > 0 )
			{
				var tempItem;
				for( tempItem = myTarget.FirstItem(); !myTarget.FinishedItems(); tempItem = myTarget.NextItem() )
				{
					if( !ValidateObject( tempItem ))
						continue;

					if( tempItem.movable == 3 )
					{
						pSocket.SysMessage( GetDictionaryEntry( 1898, pSocket.language )); // You can only release locked down containers that don't contain any locked down items!
						return;
					}
				}
			}

			// Non-house owners have some limitations for which items they can release
			// 				Owner 		- Can release all items
			// 				Co-Owner 	- Can release items locked down by self, other co-owners or friends
			// 				Friends 	- Can release items locked down by self only
			if( !iMulti.IsOwner( pUser ))
			{
				if( iMulti.IsOnOwnerList( pUser ))
				{
					// Co-owners can only release items locked down by themselves or by friends
					if( !ValidateObject( myTarget.owner ) || ( myTarget.owner.serial != pUser.serial && !iMulti.IsOnFriendList( myTarget.owner ) && ( iMulti.IsOnOwnerList( myTarget.owner ) || iMulti.IsOwner( myTarget.owner ))))
					{
						pSocket.SysMessage( GetDictionaryEntry( 1899, pSocket.language )); // You cannot release an item locked down by someone with a higher access level to the house than yourself!
						return;
					}
				}
				else
				{
					// Friends can only release items they locked down by themselves
					if( !pUser.isGM && ( !ValidateObject( myTarget.owner ) || myTarget.owner.serial != pUser.serial ))
					{
						pSocket.SysMessage( GetDictionaryEntry( 1899, pSocket.language )); // You cannot release an item locked down by someone with a higher access level to the house than yourself!
						return;
					}
				}
			}

			if( iMulti.IsSecureContainer( myTarget ))
			{
				// Secure Container
				iMulti.UnsecureContainer( myTarget );
				var secureContainersLeft = iMulti.maxSecureContainers - iMulti.secureContainers;
				pSocket.SysMessage( GetDictionaryEntry( 1900, pSocket.language ), secureContainersLeft ); // You unsecure the container (%i secure container(s) remaining).
			}
			else if( myTarget.type == 87 ) // Unsecured trash barrel
			{
				pSocket.SysMessage( GetDictionaryEntry( 1901, pSocket.language )); // To remove a trash barrel, chop it with an axe!
			}
			else
			{
				// Release the locked down item
				iMulti.ReleaseItem( myTarget );

				var lockdownsLeft = iMulti.maxLockdowns - iMulti.lockdowns;
				pSocket.SysMessage( GetDictionaryEntry( 1902, pSocket.language ), lockdownsLeft ); // You release the item (%i lockdown(s) remaining).
			}
		}
	}
	else
	{
		pSocket.SysMessage( GetDictionaryEntry( 1108, pSocket.language )); // Invalid item!
	}
}

function SecureContainer( pSocket, iMulti )
{
	if( pSocket != null )
	{
		if( !ValidateObject( iMulti ))
		{
			pSocket.SysMessage( GetDictionaryEntry( 1820, pSocket.language )); // Unable to detect house! Try again, or contact a GM if problem persists.
			return;
		}

		var pUser = pSocket.currentChar;
		if( pUser.isGM || ( iMulti.IsOnOwnerList( pUser ) || ( coOwnHousesOnSameAccount && ValidateObject( iMulti.owner ) && iMulti.owner.accountNum == pUser.accountNum )))
		{
			pSocket.tempObj = iMulti;
			pSocket.CustomTarget( 10, GetDictionaryEntry( 1903, pSocket.language )); // Select container to secure:
		}
		else
		{
			pSocket.SysMessage( GetDictionaryEntry( 1904, pSocket.language )); // Only house owners and co-owners can secure containers in a house!
		}
	}
}

// Secure Container
function onCallback10( pSocket, myTarget )
{
	var pUser = pSocket.currentChar;
	var iMulti = pSocket.tempObj;
	pSocket.tempObj = null;

	if( !ValidateObject( iMulti ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 1823, pSocket.language )); // You need to be inside a house to use house commands!
		return;
	}

	if( !iMulti.IsOnOwnerList( pUser ))
	{
		if( !pUser.isGM && ( !coOwnHousesOnSameAccount || ( coOwnHousesOnSameAccount && ( !ValidateObject( iMulti.owner ) || iMulti.owner.accountNum != pUser.accountNum ))))
		{
			pSocket.SysMessage( GetDictionaryEntry( 1904, pSocket.language )); // Only house owners and co-owners can secure containers in a house!
			return;
		}
	}

	var StrangeByte = pSocket.GetWord( 1 );
	if( StrangeByte === 0 && ValidateObject( myTarget ) && myTarget.isItem )
	{
		if( iMulti.secureContainers < iMulti.maxSecureContainers )
		{
			if( myTarget.type != 1 && myTarget.type != 87 ) // Unlocked containers and trash barrels
			{
				pSocket.SysMessage( GetDictionaryEntry( 1905, pSocket.language )); // You can only secure unlocked containers!
			}
			else if( myTarget.container != null )
			{
				pSocket.SysMessage( GetDictionaryEntry( 1906, pSocket.language )); // You can only secure containers that are not inside other containers!
			}
			else if( !iMulti.IsInMulti( myTarget ))
			{
				pSocket.SysMessage( GetDictionaryEntry( 1907, pSocket.language )); // You can only secure containers that are in your house!
			}
			else if( myTarget.type == 1 && myTarget.movable == 2 && ( myTarget.id == 0x09A8 || myTarget.id == 0x0E80 ))
			{
				pSocket.SysMessage( GetDictionaryEntry( 1908, pSocket.language )); // You cannot secure strongboxes; they're already secure!
			}
			else
			{
				iMulti.SecureContainer( myTarget );
				var secureContainersLeft = iMulti.maxSecureContainers - iMulti.secureContainers;
				pSocket.SysMessage(  GetDictionaryEntry( 1909, pSocket.language ), secureContainersLeft ); // You secure the container (%i secure container(s) remaining).
			}
		}
		else
		{
			pSocket.SysMessage(  GetDictionaryEntry( 1910, pSocket.language )); // This house cannot have any additional secure containers!
		}
	}
	else
	{
		pSocket.SysMessage( GetDictionaryEntry( 1108, pSocket.language )); // Invalid item!
	}
}

// Demolish house - Trigger
function DemolishHouse( pSocket, iMulti )
{
	if( pSocket != null )
	{
		if( !ValidateObject( iMulti ))
		{
			pSocket.SysMessage( GetDictionaryEntry( 1820, pSocket.language )); // Unable to detect house! Try again, or contact a GM if problem persists.
			return;
		}

		if( iMulti.IsOwner( pSocket.currentChar ) || pSocket.currentChar.isGM )
		{
			// Pack up any player vendors in the house and convert them to deeds, too
			var charInHouse;
			for( charInHouse = iMulti.FirstChar(); !iMulti.FinishedChars(); charInHouse = iMulti.NextChar() )
			{
				if( !ValidateObject( charInHouse ))
					continue;

				if( !ValidateObject( charInHouse.multi ))
					continue;

				if( charInHouse.aitype == 17 ) // player vendor AI
				{
					var tempName = GetDictionaryEntry( 580, pSocket.language ); // A vendor deed for %s.
					tempName = ( tempName.replace( /%s/gi, charInHouse.name ));

					var pvDeed = CreateBlankItem( pSocket, pSocket.currentChar, 1, tempName, 0x14F0, 0x0, "ITEM", true );
					if( ValidateObject( pvDeed ))
					{
						pvDeed.type = 217; // Player Vendor Deed
						pvDeed.buyValue = 2000;
						pSocket.SysMessage( GetDictionaryEntry( 581, pSocket.language ), pvDeed.name ); // Packed up vendor %s.
						charInHouse.Delete();
					}
				}
				else
				{
					// Eject character from house
					EjectPlayerActual( iMulti, charInHouse );
				}
			}

			// Release lockdown on any items left in the house, move them to ground level
			// Also remove any trash barrels
			var itemInHouse;
			for( itemInHouse = iMulti.FirstItem(); !iMulti.FinishedItems(); itemInHouse = iMulti.NextItem() )
			{
				if( !ValidateObject( itemInHouse ))
					continue;

				if( !ValidateObject( itemInHouse.multi ))
					continue;

				// Don't touch doors, or signs
				if( itemInHouse.type == 203 || itemInHouse.type == 13 || itemInHouse.type == 12 )
					continue;

				if( itemInHouse.type == 87 ) // trash container
				{
					if( iMulti.IsSecureContainer( itemInHouse ))
					{
						iMulti.UnsecureContainer( itemInHouse );
					}
					iMulti.RemoveTrashCont( itemInHouse );
					itemInHouse.Delete();
				}
				else if( itemInHouse.movable == 2 || itemInHouse.GetTag( "deed" )) // items placed as part of the house itself like forge/anvil in smithy or the addon deed
				{
					var addonDeed = itemInHouse.GetTag( "deed" );
					if( addonDeed )
					{
						var newDeed = CreateDFNItem( pSocket, pSocket.currentChar, addonDeed, 1, "ITEM", true );
						if( newDeed )
						{
							pSocket.SysMessage( GetDictionaryEntry( 1970, pSocket.language )); // A deed for the house add-on has been placed in your backpack.
						}
					}
					itemInHouse.Delete();
				}
				else if( itemInHouse.isLockedDown )
				{
					if( iMulti.IsSecureContainer( itemInHouse ))
					{
						iMulti.UnsecureContainer( itemInHouse );
					}
					iMulti.ReleaseItem( itemInHouse );

					// Drop all items contained in house to ground level so they're not stuck in the middle of the air!
					var groundZ = GetMapElevation( itemInHouse.x, itemInHouse.y, pSocket.currentChar.worldnumber );
					itemInHouse.Teleport( itemInHouse.x, itemInHouse.y, groundZ );
				}
			}

			// Kill all the keys related to the house - they are of no use any more
			iMulti.KillKeys();

			// Remove file that keeps track of visitors to house, if any exists
			//TriggerEvent( 15000, "RemoveTrackingFile", iMulti );
			var fileName = "house" + ( iMulti.serial ).toString() + ".jsdata";
			var folderName = "houseVisits";
			DeleteFile( fileName, folderName );

			// Create new deed for the house
			var newDeed = CreateDFNItem( pSocket, pSocket.currentChar, iMulti.deed, 1, "ITEM", true );
			if( newDeed )
			{
				pSocket.SysMessage( GetDictionaryEntry( 578, pSocket.language ), iMulti.name ); // Demolishing House %s.
				pSocket.SysMessage( GetDictionaryEntry( 579, pSocket.language ), newDeed.name ); // Converted into a %s.
			}

			// Finally, delete the house!
			iMulti.Delete();
		}
		else
		{
			pSocket.SysMessage( GetDictionaryEntry( 1911, pSocket.language )); // Only the primary house owner can demolish the house!
		}
	}
}

function ChangeHouseLocks( pSocket, iMulti )
{
	if( pSocket != null )
	{
		if( !ValidateObject( iMulti ))
		{
			pSocket.SysMessage( GetDictionaryEntry( 1820, pSocket.language )); // Unable to detect house! Try again, or contact a GM if problem persists.
			return;
		}

		var pUser = pSocket.currentChar;
		if( pUser.isGM || iMulti.IsOwner( pUser ))
		{
			// Some doors might be outside the house because they are open......
			var foundOpenFrontDoor = AreaItemFunction( "CheckForOpenFrontDoors", iMulti, 40, pSocket );
			if( foundOpenFrontDoor )
			{
				pSocket.SysMessage( GetDictionaryEntry( 1912, pSocket.language )); // You cannot change the house locks while the front doors are open!
				return;
			}

			// First off, remove all old keys!
			iMulti.KillKeys();
			pSocket.SysMessage( GetDictionaryEntry( 1913, pSocket.language )) // All old house keys associated with house have been deleted.

			// Next up, create a new master key for the new owner, and
			// make sure all the doors are linked to this master key
			var newKey = CreateDFNItem( pSocket, pUser, "0x100F", 1, "ITEM", true );
			if( newKey )
			{
				newKey.name = "a house key";
				newKey.more = iMulti.serial;
				newKey.type = 7; // IT_KEY item type
				pSocket.SysMessage( GetDictionaryEntry( 1914, pSocket.language )) // A new master key for the house has been created.
			}
		}
		else
		{
			pSocket.SysMessage( GetDictionaryEntry( 1915, pSocket.language )); //Only the house owner can change the house locks!
		}
	}
}

function DeclareHousePublic( pSocket, iMulti )
{
	if( pSocket != null )
	{
		if( !ValidateObject( iMulti ))
		{
			pSocket.SysMessage( GetDictionaryEntry( 1820, pSocket.language )); // Unable to detect house! Try again, or contact a GM if problem persists.
			return;
		}

		var pUser = pSocket.currentChar;
		if( pUser.isGM || iMulti.IsOwner( pUser ))
		{
			// Some doors might be outside the house because they are open......
			var foundOpenFrontDoor = AreaItemFunction( "CheckForOpenFrontDoors", iMulti, 40, pSocket );
			if( foundOpenFrontDoor )
			{
				pSocket.SysMessage( GetDictionaryEntry( 1916, pSocket.language )); // You cannot change the public status of the house while the front doors are open!
				return;
			}

			iMulti.isPublic = true;

			// Clear guest list
			iMulti.ClearGuestList();

			// Automatically unlock front doors!
			var itemInHouse;
			for( itemInHouse = iMulti.FirstItem(); !iMulti.FinishedItems(); itemInHouse = iMulti.NextItem() )
			{
				if( !ValidateObject( itemInHouse ))
					continue;

				if( !ValidateObject( itemInHouse.multi ))
					continue;

				if( itemInHouse.type != 13 ) // Only look for locked doors
					continue;

				// Unlock the door if it's a front door!
				if( itemInHouse.GetTag( "DoorType" ) == "front" )
				{
					itemInHouse.type = 12;
				}
			}

			pSocket.SysMessage( GetDictionaryEntry( 1917, pSocket.language )); // House has been declared public; front doors cannot be locked, and have been automatically unlocked!
		}
		else
		{
			pSocket.SysMessage( GetDictionaryEntry( 1918, pSocket.language )); // Only the house owner can change the public status of the house!
		}
	}
}

function DeclareHousePrivate( pSocket, iMulti )
{
	if( pSocket != null )
	{
		if( !ValidateObject( iMulti ))
		{
			pSocket.SysMessage( GetDictionaryEntry( 1820, pSocket.language )); // Unable to detect house! Try again, or contact a GM if problem persists.
			return;
		}

		var pUser = pSocket.currentChar;
		if( pUser.isGM || iMulti.IsOwner( pUser ))
		{
			// Some doors might be outside the house because they are open......
			var foundOpenFrontDoor = AreaItemFunction( "CheckForOpenFrontDoors", iMulti, 40, pSocket );
			if( foundOpenFrontDoor )
			{
				pSocket.SysMessage( GetDictionaryEntry( 1916, pSocket.language )); // You cannot change the public status of the house while the front doors are open!
				return;
			}

			// Check if there are any player vendors in the house, disallow switching to private if so
			var charInHouse;
			for( charInHouse = iMulti.FirstChar(); !iMulti.FinishedChars(); charInHouse = iMulti.NextChar() )
			{
				if( !ValidateObject( charInHouse ))
					continue;

				if( !ValidateObject( charInHouse.multi ))
					continue;

				if( charInHouse.aitype == 17 ) // player vendor AI
				{
					pSocket.SysMessage( GetDictionaryEntry( 1919, pSocket.language )); // You have vendors working out of this building. It cannot be declared private until there are no vendors in place.
					return;
				}
				else
				{
					// Eject non-owners and friends from the multi
					if( !iMulti.IsOnOwnerList( charInHouse ) && !iMulti.IsOnFriendList( charInHouse ))
					{
						// Eject character from house
						EjectPlayerActual( iMulti, charInHouse );
						if( charInHouse.online )
						{
							charInHouse.SysMessage( GetDictionaryEntry( 1920, charInHouse.socket.language )); // 1920=You have been ejected due to a change in public status of the house.
						}
					}
				}
			}

			// Change sign to basic brass sign
			var iSignSerial = iMulti.more;
			var serialPart1 = ( iSignSerial >> 24 );
			var serialPart2 = ( iSignSerial >> 16 );
			var serialPart3 = ( iSignSerial >> 8 );
			var serialPart4 = ( iSignSerial % 256 );
			iSign = CalcItemFromSer( serialPart1, serialPart2, serialPart3, serialPart4 );

			if( ValidateObject( iSign ))
			{
				iSign.id = 0x0BD2;
			}

			iMulti.isPublic = false;

			// Automatically lock front doors!
			var itemInHouse;
			for( itemInHouse = iMulti.FirstItem(); !iMulti.FinishedItems(); itemInHouse = iMulti.NextItem() )
			{
				if( !ValidateObject( itemInHouse ))
					continue;

				if( !ValidateObject( itemInHouse.multi ))
					continue;

				if( itemInHouse.type != 12 ) // Only look for unlocked doors
					continue;

				// Lock the door if it's a front door!
				if( itemInHouse.GetTag( "DoorType" ) == "front" )
				{
					itemInHouse.type = 13;
				}
			}
			pSocket.SysMessage( GetDictionaryEntry( 1921, pSocket.language )); // 1921=This house is now private.
		}
		else
		{
			pSocket.SysMessage( GetDictionaryEntry( 1922, pSocket.language )); // 1922=Only the house owner can change the public status of the house!
		}
	}
}

function CheckForOpenFrontDoors( iMulti, itemToCheck, pSocket )
{
	if( ValidateObject( itemToCheck ))
	{
		if(( itemToCheck.type == 12 || itemToCheck.type == 13 ) && ( itemToCheck.more == iMulti.serial || itemToCheck.multi == iMulti ))
		{
			if( itemToCheck.GetTag( "DoorType" ) == "front" && itemToCheck.isDoorOpen )
			{
				return true;
			}
		}
	}
	return false;
}

function PlaceTrashBarrel( pSocket, iMulti )
{
	if( pSocket != null )
	{
		if( !ValidateObject( iMulti ))
		{
			pSocket.SysMessage( GetDictionaryEntry( 1820, pSocket.language )); // Unable to detect house! Try again, or contact a GM if problem persists.
			return;
		}

		var pUser = pSocket.currentChar;
		if( pUser.isGM || ( iMulti.IsOnOwnerList( pUser ) || ( coOwnHousesOnSameAccount && ValidateObject( iMulti.owner ) && iMulti.owner.accountNum == pUser.accountNum )))
		{
			// First check if there's a trash barrel in the house already
			var trashContainerCount = iMulti.trashContainers;
			var maxTrashContainers = iMulti.maxTrashContainers;
			if( trashContainerCount < maxTrashContainers )
			{
				// Still have room for another trash barrel, so let's create a new one, and add it
				// to the multi's list of trash containers
				var trashBarrel = CreateDFNItem( pSocket, pSocket.currentChar, "trash_barrel", 1, "ITEM", false );
				iMulti.AddTrashCont( trashBarrel );
				trashBarrel.movable = 3;
			}
			else
			{
				pSocket.SysMessage( GetDictionaryEntry( 1923, pSocket.language )); // You cannot add any more trash barrels to this house! (%i / %i)
			}
		}
		else
		{
			pSocket.SysMessage( GetDictionaryEntry( 1924, pSocket.language )); // Only house owners or co-owners can place a trash barrel in a house!
		}
	}
}

function PlaceStrongbox( pSocket, iMulti )
{
	if( pSocket != null )
	{
		if( !ValidateObject( iMulti ))
		{
			pSocket.SysMessage( GetDictionaryEntry( 1820, pSocket.language )); // Unable to detect house! Try again, or contact a GM if problem persists.
			return;
		}

		var pUser = pSocket.currentChar;
		if( iMulti.IsOwner( pUser ))
		{
			pSocket.SysMessage( GetDictionaryEntry( 1925, pSocket.language )); // Owners do not get a strongbox of their own.
			return;
		}
		if( iMulti.IsOnOwnerList( pUser )) // Only co-owners can place one of these secure containers
		{
			// First check if there's already a strongbox in the house that belongs to this co-owner - each co-owner can only have 1 strongbox
			var tempItem;
			for( tempItem = iMulti.FirstItem(); !iMulti.FinishedItems(); tempItem = iMulti.NextItem() )
			{
				if( !ValidateObject( tempItem ))
					continue;

				if( tempItem.movable == 2 && ( tempItem.id == 0x09A8 || tempItem.id == 0x0E80 ) && tempItem.owner == pUser )
				{
					pSocket.SysMessage( GetDictionaryEntry( 1926, pSocket.language )); // You already have a strongbox!
					return;
				}
			}

			var strongbox = CreateDFNItem( pSocket, pSocket.currentChar, "strongbox", 1, "ITEM", false );
			strongbox.owner = pSocket.currentChar;
			strongbox.movable = 2;
			strongbox.name = pSocket.currentChar.name + "'s Strongbox";
			pSocket.SysMessage( GetDictionaryEntry( 1927, pSocket.language )); // You have a new strongbox. Only you and the house owner may access this box.
		}
		else
		{
			pSocket.SysMessage( GetDictionaryEntry( 1928, pSocket.language )); // You may not place a strongbox.
		}
	}
}

function ConfirmActionGump( pSocket, myTarget, confirmString, confirmButtonID )
{
	var confirmGump = new Gump;
	confirmGump.AddPage( 0 );
	confirmGump.AddBackground( 0, 0, 300, 160, 2620 ); // Tile Dark Gray Background
	confirmGump.AddBackground( 10, 10, 280, 140, 3000 ); // Tile White Background
	confirmGump.AddHTMLGump( 10, 20, 280, 60, 0, 0, "<CENTER><BIG>" + GetDictionaryEntry( 2854, pSocket.language ) + "</BIG></CENTER>" ); // Confirm?
	confirmGump.AddButton( 100, 120, 4023, 4024, 1, 0, confirmButtonID ); // Okay, confirm
	confirmGump.AddButton( 175, 120, 4020, 4021, 1, 0, 0 ); // Cancel
	confirmGump.AddText( 25, 120, 0, GetDictionaryEntry( 2849, pSocket.language )); // CONFIRM >
	confirmGump.AddText( 210, 120, 0, GetDictionaryEntry( 2850, pSocket.language )); // < CANCEL
	confirmGump.AddHTMLGump( 25, 50, 250, 80, 0, 0, confirmString );
	confirmGump.Send( myTarget.socket );
	confirmGump.Free();
}

// Handle button presses in the house menu
function onGumpPress( pSocket, pButton, gumpData )
{
	var pUser = pSocket.currentChar;

	// Calculate multi!
	var iMultiSerial = parseInt( pUser.GetTag( "iMultiSerial" ));
	var iMultiSerial1 = ( iMultiSerial >> 24 );
	var iMultiSerial2 = ( iMultiSerial >> 16 );
	var iMultiSerial3 = ( iMultiSerial >> 8 );
	var iMultiSerial4 = ( iMultiSerial % 256 );
	var iMulti = CalcMultiFromSer( iMultiSerial1, iMultiSerial2, iMultiSerial3, iMultiSerial4 );

	// Calculate sign on multi!
	var iSignSerial = parseInt( pUser.GetTag( "iSignSerial" ));
	var iSignSerial1 = ( iSignSerial >> 24 );
	var iSignSerial2 = ( iSignSerial >> 16 );
	var iSignSerial3 = ( iSignSerial >> 8 );
	var iSignSerial4 = ( iSignSerial % 256 );
	var iSign = CalcItemFromSer( iSignSerial1, iSignSerial2, iSignSerial3, iSignSerial4 );

	// Calculate multi owner!
	var oldMultiOwner = CalcCharFromSer( parseInt( pUser.GetTag( "oldMultiOwner" )));

	// Remove the custom tags
	pUser.SetTag( "iMultiSerial", null );
	pUser.SetTag( "oldMultiOwner", null );
	pUser.SetTag( "iSignSerial", null );

	if( !ValidateObject( iMulti ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 1820, pSocket.language )); // Unable to detect house! Try again, or contact a GM if problem persists.
		return;
	}

	if( !ValidateObject( oldMultiOwner ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 1929, pSocket.language )); // Cannot find previous owner! Try again or contact a GM for assistance.
		return;
	}

	switch( pButton )
	{
		case 0: // Cancel
			oldMultiOwner.SysMessage( GetDictionaryEntry( 2855, pSocket.language )); // Player rejected the offer.
			break;
		case 1: // Confirm transfer of house ownership
		{
			if( !ValidateObject( iSign ))
			{
				pSocket.SysMessage( GetDictionaryEntry( 1821, pSocket.language )); // Unable to detect house sign! Try again, or contact a GM if problem persists.
				return;
			}

			// Transfer ownership of both house and sign to targeted player
			iMulti.owner = pUser;
			iSign.owner = pUser;

			// Clear old co-owner, friend, guest and ban lists
			iMulti.ClearOwnerList();
			iMulti.ClearFriendList();
			iMulti.ClearGuestList();
			iMulti.ClearBanList();

			// Remove the old owner from the ownerList (not affected by clearing of list)
			if( iMulti.IsOnOwnerList( oldMultiOwner ))
			{
				iMulti.RemoveFromOwnerList( oldMultiOwner );
			}

			// Kill all old keys associated with house
			iMulti.KillKeys();
			oldMultiOwner.SysMessage( GetDictionaryEntry( 1913, oldMultiOwner.socket.language )) // All old house keys associated with house have been deleted.

			// Add the new owner to ownerList
			iMulti.AddToOwnerList( pUser );

			// Eject old owner - and all other characters who might no longer belong in the house - from the house
			var charInHouse;
			for( charInHouse = iMulti.FirstChar(); !iMulti.FinishedChars(); charInHouse = iMulti.NextChar() )
			{
				if( !ValidateObject( charInHouse ))
					continue;

				if( !ValidateObject( charInHouse.multi ))
					continue;

				if( charInHouse == pUser ) // Don't eject the new owner
					continue;

				if( charInHouse.aitype != 17 ) // player vendor AI
				{
					// Eject character from house
					EjectPlayerActual( iMulti, charInHouse );
				}
			}

			// Update last traded timestamp
			iMulti.tradeTimestamp = 1; // dummy, turn into method instead

			// // Next up, create a new master key for the new owner, and
			// // make sure all the doors are linked to this master key
			var newKey = CreateDFNItem( pSocket, pUser, "0x100F", 1, "ITEM", true );
			if( newKey )
			{
				newKey.name = "a house key for " + iSign.name;
				newKey.name = "a house key";
				newKey.more = iMulti.serial;
				newKey.type = 7; // IT_KEY item type

				// Make one copy for the player's bank box
				var newKeyCopy = newKey.Dupe( pSocket );
				if( ValidateObject( newKeyCopy ))
				{
					var bankBox = pUser.FindItemLayer( 29 );
					if( ValidateObject( bankBox ) && bankBox.isItem )
					{
						newKeyCopy.container = bankBox;
					}
					else
					{
						// Create a new bankbox for the player, since they do not automatically get created until first time the bank is used
						var newBankBox = CreateDFNItem( pSocket, pUser, "0x09ab", 1, "ITEM", false );
						newBankBox.name = pUser.name + "'s bank box";
						newBankBox.layer = 29;
						newBankBox.owner = pUser;
						newBankBox.container = pUser;
						newBankBox.maxItems = parseInt( GetServerSetting( "MAXPLAYERBANKITEMS" ));
						newBankBox.type = 1;
						newBankBox.morex = 1;

						if( ValidateObject( newBankBox ))
						{
							newKeyCopy.container = newBankBox;
						}
					}
				}
				else
				{
					pSocket.SysMessage( GetDictionaryEntry( 1930, pSocket.language )); // Something went wrong when copying house key - unable to deposit extra key in bankbox!
				}

				if( oldMultiOwner.online )
				{
					oldMultiOwner.socket.SysMessage( GetDictionaryEntry( 1081, oldMultiOwner.socket.language ), pUser.name ); // You have transferred your house to %s.
				}
				if( pUser.online )
				{
					pSocket.SysMessage( GetDictionaryEntry( 1082, pSocket.language ), pUser.name ); // %s has transferred a house to you.
				}
			}
			break;
		}
		case 2:
		{
			// Add target player to co-owner list
			iMulti.AddToOwnerList( pUser );

			var newKey = CreateDFNItem( pSocket, pUser, "0x100F", 1, "ITEM", true );
			if( newKey )
			{
				newKey.name = "a house key";
				newKey.more = iMulti.serial;
				newKey.type = 7; // IT_KEY item type
			}

			oldMultiOwner.socket.SysMessage( GetDictionaryEntry( 1931, oldMultiOwner.socket.language ), pUser.name ); // You have added %s as a co-owner of your house!
			pSocket.SysMessage( GetDictionaryEntry( 1932, pSocket.language ), oldMultiOwner.name, iMulti.name ); // You have been added by %s as a co-owner of %s!
			break;
		}
	}
}

function _restorecontext_() {}
