// These house commands are triggered by client sending some specific keywords that are
// detected as such by UOX3, and then forwarded to this script.
//
// Uses script trigger 15000, applied to any new houses built via SCRIPT tag in houses.dfn,
// or first time house sign is used if the house existed before this change was made

const coOwnHousesOnSameAccount = GetServerSetting( "COOWNHOUSESONSAMEACCOUNT" );
const protectPrivateHouses = GetServerSetting( "PROTECTPRIVATEHOUSES" );
const visitorPurgeTimer = 86400; // visitor list for house purged every 24 hours

// Fetch INI settings for access to safe logout in house, which if enabled does two things:
// 1. Allows player to safely and instantly log out inside the house
// 2. Prevents player from being automatically booted from the house when they logout
const safeCoOwnerLogout = GetServerSetting( "SafeCoOwnerLogout" );
const safeFriendLogout = GetServerSetting( "SafeFriendLogout" );
const safeGuestLogout = GetServerSetting( "SafeGuestLogout" );

// Also fetch INI settings for keyless access to house, to help determine who needs to be automatically
// booted from house after safely logging out (because they might not be able to open the door!)
const keylessGuestAccess = GetServerSetting( "KeylessGuestAccess" );
const keylessFriendAccess = GetServerSetting( "KeylessFriendAccess" );
const keylessCoOwnerAccess = GetServerSetting( "KeylessCoOwnerAccess" );

function onHouseCommand( pSocket, iMulti, cmdID )
{
	if( ValidateObject( iMulti ) && !iMulti.IsBoat() && iMulti.IsInMulti( pSocket.currentChar ))
	{
		switch( cmdID )
		{
			case 33: // New house owner - NOT a spoken command?
				TriggerEvent( 15002, "TransferOwnership", pSocket, iMulti );
				break;
			case 34: // Eject Target
				TriggerEvent( 15002, "EjectPlayer", pSocket, iMulti );
				break;
			case 35: // Ban Target
				TriggerEvent( 15002, "BanPlayer", pSocket, iMulti );
				break;
			case 36: // Add Friend - NOT a spoken command?
				TriggerEvent( 15002, "AddFriend", pSocket, iMulti );
				break;
			case 37: // Remove Friend - NOT a spoken command?
				TriggerEvent( 15002, "RemoveFriend", pSocket, iMulti );
				break;
			case 38: // Lockdown
				TriggerEvent( 15002, "LockdownItem", pSocket, iMulti );
				break;
			case 39: // Release
				TriggerEvent( 15002, "ReleaseItem", pSocket, iMulti );
				break;
			case 40: // Secure
				TriggerEvent( 15002, "SecureContainer", pSocket, iMulti );
				break;
			case 46: // Place Trash Barrel
				TriggerEvent( 15002, "PlaceTrashBarrel", pSocket, iMulti );
				break;
			case 47: // Place strongbox - co-owner only
				TriggerEvent( 15002, "PlaceStrongbox", pSocket, iMulti );
				break;
			default:
				Console.Log( "Unhandled house command detected with cmdID: " + cmdID );
				return false;
		}
	}

	return true;
}

// Handle ejecting players to SE corner of multi when attempting to enter a building they cannot enter
// Also handle updating visitor counter for public houses
function onEntrance( iMulti, charEntering, objType )
{
	// First do some standard validation checking to make sure both house
	// and visiting character are actually valid objects
	if( !ValidateObject( iMulti ))
		return false;

	if( !ValidateObject( charEntering ))
		return false;

	if( objType != 0 ) // We only care about characters entering, not items
		return false;

	// Only run the following checks for non-GM/non-Counselor characters
	if( !charEntering.isGM && !charEntering.isCounselor )
	{
		// Prevent banned players from entering a building
		if( iMulti.IsOnBanList( charEntering ))
		{
			// Prevent banned players from entering. Eject them right back out again!
			PreventMultiAccess( iMulti, charEntering, 0, 1639 ); // You are banned from that location
			return false;
		}

		// Prevent unauthorized visitors from entering private buildings
		var charToCheck = charEntering;
		if( ValidateObject( charEntering.owner ))
		{
			// If the character has an owner, check the owner's access instead
			charToCheck = charEntering.owner;
		}
		if( !ValidateObject( iMulti.owner ) || ( charToCheck != iMulti.owner ))
		{
			if( !iMulti.isPublic && protectPrivateHouses
				&& ( !iMulti.IsOnOwnerList( charToCheck ) && !iMulti.IsOnFriendList( charToCheck ) && !iMulti.IsOnGuestList( charToCheck )
					&& ( !coOwnHousesOnSameAccount || !ValidateObject( iMulti.owner ) || ( coOwnHousesOnSameAccount && iMulti.owner.accountNum != charToCheck.accountNum ))))
			{
				// Prevent unauthorized visitors from entering private buildings
				PreventMultiAccess( iMulti, charEntering, 1, 1817 ); // This is a private home
				return false;
			}
		}

		// Update visitor count for players entering a public building (don't count owners, or friends of owners)
		if( iMulti.isPublic && !charEntering.npc && !iMulti.IsOnOwnerList( charEntering ) && !iMulti.IsOnFriendList( charEntering ))
		{
			// Has more than 24 hours passed since the visitorTracker was last cleared? If so, clear it now
			var lastPurgeTime = parseInt( iMulti.GetTag( "lastPurge" ));
			if( lastPurgeTime != 0 )
			{
				if(( Date.now() - lastPurgeTime ) / 1000 > visitorPurgeTimer )
				{
					PurgeVisitTracker( iMulti );
					iMulti.SetTag( "lastPurge", Date.now().toString() );
				}

				// Count visitor if they haven't entered the building for the past 24 hours
				if( CheckVisitTracker( iMulti, charEntering ) )
				{
					// Increase visitor count on multi
					var visitCount = iMulti.GetTag( "visitCount" );
					iMulti.SetTag( "visitCount", ++visitCount );
				}
			}
			else
			{
				// Tag didn't exist! This is the first visit, ever
				iMulti.SetTag( "lastPurge", Date.now().toString() );

				// Assume no file exists already, and just add visitor directly!
				if( AddVisitor( iMulti, charEntering ))
				{
					// Increase visitor count on multi
					var visitCount = iMulti.GetTag( "visitCount" );
					iMulti.SetTag( "visitCount", ++visitCount );
				}
				else
				{
					Console.Warning( "Unable to update visitor tracker for house with serial " + ( iMulti.serial ).toString() + " !" );
				}
			}
		}
	}

	return true;
}

function onMultiLogout( iMulti, cPlayer )
{
	// Always validate owner logging out of their own house
	// Returning true makes core UOX3 go through with the "safe/instant logout" code portion
	if( ValidateObject( iMulti.owner ))
	{
		if( iMulti.owner == cPlayer || ( coOwnHousesOnSameAccount && iMulti.owner.accountNum == cPlayer.accountNum ))
		{
			return true;
		}
	}

	// Allow co-owners, friends and/or guests to safely logout in the house if enabled
	if(( safeCoOwnerLogout && iMulti.IsOnOwnerList( cPlayer ))
		|| ( safeFriendLogout && iMulti.IsOnFriendList( cPlayer ))
		|| ( safeGuestLogout && iMulti.IsOnGuestList( cPlayer ) ))
	{
		if(( !keylessCoOwnerAccess && iMulti.IsOnOwnerList( cPlayer ))
			|| ( !keylessFriendAccess && iMulti.IsOnFriendList( cPlayer ))
			|| ( !keylessGuestAccess && iMulti.IsOnGuestList( cPlayer )))
		{
			// Look for key in their backpack. If they don't have one, boot them out
			var foundKey = false;
			var iPack = cPlayer.pack;
			if( iPack != null )
			{
				foundKey = TriggerEvent( 4500, "FindKeyInPack", cPlayer, iPack, iMulti );
			}

			if( !foundKey )
			{
				PreventMultiAccess( iMulti, cPlayer, 2, 0 );
			}
		}
		return true;
	}

	// Default to booting player from house on logout, and disallowing safe logout
	PreventMultiAccess( iMulti, cPlayer, 2, 0 );
	return false;
}

function PreventMultiAccess( iMulti, charEntering, ejectReason, dictEntry )
{
	if( iMulti.IsInMulti( charEntering ))
	{
		// Get ban location for multi, as well as map elevation at corner of house
		// to ensure ejected object is placed at correct height
		var multiCornerZ = 0;
		var banX = iMulti.banX;
		var banY = iMulti.banY;
		if( banX == 0 && banY == 0 )
		{
			// Fetch coordinates for corner 3 (SE) of multi and use that as ban location
			var multiCornerCoords = ( iMulti.GetMultiCorner( 3 )).split( "," );
			banX = parseInt( multiCornerCoords[0] );
			banY = parseInt( multiCornerCoords[1] );
		}

		multiCornerZ = GetMapElevation( banX, banY, iMulti.worldnumber );
		if( multiCornerZ < iMulti.z )
		{
			multiCornerZ = iMulti.z;
		}

		charEntering.Teleport( banX, banY, multiCornerZ );
		if( charEntering.online )
		{
			charEntering.socket.walkSequence = -1;
		}
	}

	// Inform player why they were ejected from the house
	if( charEntering.online )
	{
		if( ejectReason == 0 )
		{
			charEntering.SysMessage( GetDictionaryEntry( dictEntry, charEntering.socket.language )); // You are banned from that location
		}
		else if( ejectReason == 1 )
		{
			charEntering.SysMessage( GetDictionaryEntry( dictEntry, charEntering.socket.language )); // This is a private home
		}
		else if( ejectReason == 2 )
		{
			// Character logged out and got ejected, no message needed
		}
	}
}

// Purge all visits to house by opening the file in write mode and close it again
function PurgeVisitTracker( iMulti )
{
	// Create a new file object
	var mFile = new UOXCFile();
	var fileName = "house" + ( iMulti.serial ).toString() + ".jsdata";

	// Open file for writing (which will overwrite any existing file with a new file) then close it again
	mFile.Open( fileName, "w", "houseVisits" );
	mFile.Close()
	mFile.Free();
}

function CheckVisitTracker( iMulti, charEntering )
{
	var rVal = false;
	var visitFound = false;
	var updateVisit = false;

	// Create a new file object
	var mFile = new UOXCFile();
	var fileName = "house" + ( iMulti.serial ).toString() + ".jsdata";

	// Open file for reading in "/shared/houseVisits/" subfolder
	mFile.Open( fileName, "r", "houseVisits" );
	if( mFile && mFile.Length() >= 0 )
	{
		// Read until we reach the end of the file, or until we find a match for the visitor
		while( !mFile.EOF() )
		{
			// Read a line of text from the file
			var line = mFile.ReadUntil( "\n" );
			if( line.length <= 1 || line == "" )
				continue;

			// Compare serial in line with character entering multi
			var visitorString = line.split( "," );
			if( visitorString[0].length > 0 && visitorString[0] == charEntering.serial )
			{
				// Found a repeat visitor! No need to continue searching
				visitFound = true;
				break;
			}
		}
		mFile.Close();

		// Frees memory allocated by file object
		mFile.Free();
	}

	if( !visitFound )
	{
		// Visitor not found in existing list! Add new visitor to list!
		rVal = AddVisitor( iMulti, charEntering );
	}

	return rVal;
}

// Append new visitor to the list!
function AddVisitor( iMulti, charEntering )
{
	// Create a new file object
	var mFile = new UOXCFile();
	var fileName = "house" + ( iMulti.serial ).toString() + ".jsdata";

	mFile.Open( fileName, "a", "houseVisits" ); // Open file for Appending
	if( mFile != null )
	{
		// Append a new line to the file with the visitor's serial and timestamp
		var visitTime = Date.now();
		var newLine = ( charEntering.serial ).toString() + "," + visitTime.toString();
		mFile.Write( newLine + "\n" );
		mFile.Close()
		mFile.Free();
		return true;
	}

	return false;
}

// Remove the tracking file for house - primarily used when demolishing houses!
function RemoveTrackingFile( iMulti )
{
	var fileName = "house" + ( iMulti.serial ).toString() + ".jsdata";
	var folderName = "houseVisits";
	DeleteFile( fileName, folderName );
}