/// <reference path="../../definitions.d.ts" />
// @ts-check
// Player Vendor Deeds
/** @type { ( user: Character, iUsing: Item ) => boolean } */
function onUseChecked( pUser, iUsed )
{
	// Check if object is in player's backpack
	var pSocket = pUser.socket;
	if( pSocket != null && ValidateObject( iUsed ) && iUsed.isItem )
	{
		var itemOwner = GetPackOwner( iUsed, 0 );
		if( itemOwner == null || itemOwner.serial != pUser.serial || !ValidateObject( iUsed.container) || iUsed.container.layer == 0x1D )		
		{
			pSocket.SysMessage( GetDictionaryEntry( 1763, pSocket.language )); // That item must be in your backpack before it can be used.
			return false;
		}

		// Check if player is actually in a house
		var iMulti = pUser.multi;
		if( !ValidateObject( iMulti ) || !iMulti.IsInMulti( pUser ))
		{
			// Player vendors can only be placed in houses!
			pSocket.SysMessage( GetDictionaryEntry( 2857, pSocket.language )); // Player vendors can only be placed in houses!
			return false;
		}

		// Make sure player has access to actually placing a player vendor in the house
		if( !iMulti.IsOwner( pUser ))
		{
			// Only the house owner can place player vendors in a house!
			pSocket.SysMessage( GetDictionaryEntry( 2858, pSocket.language )); // Only the house owner can place player vendors in a house!
			return false;
		}

		// Check that the house hasn't reached cap for number of player vendors
		if( iMulti.vendors >= iMulti.maxVendors )
		{
			// You cannot place any more player vendors in this house!
			pSocket.SysMessage( GetDictionaryEntry( 2859, pSocket.language )); // You cannot place any more player vendors in this house!
			return false;
		}

		// Make sure the house is not private - player vendors can only be placed in public houses!
		if( !iMulti.isPublic )
		{
			pSocket.SysMessage( GetDictionaryEntry( 2860, pSocket.language )); // Player vendors can only be placed in public houses!
			return false;
		}

		// Make sure player is not trying to place the player vendor too close to a door!
		// Check for nearby doors
		var foundDoor = AreaItemFunction( "CheckForNearbyDoors", pUser, 3, pSocket );
		if( foundDoor )
		{
			pSocket.SysMessage( GetDictionaryEntry( 2861, pSocket.language )); // You cannot place a player vendor adjacent to a door!
			return false;
		}

		// Make sure there is not already an NPC on the exact tile
		var foundNPC = AreaCharacterFunction( "CheckForNPCAtVendorSpot", pUser, 0, pSocket );
		if( foundNPC )
		{
			pSocket.SysMessage( "You cannot place a player vendor on top of another NPC." );
			return false;
		}

		// Create player vendor at player's current location
		var npcVendor = SpawnNPC( "playervendor", pUser.x, pUser.y, pUser.z, pUser.worldnumber, pUser.instanceID );
		if( ValidateObject( npcVendor ))
		{
			// Set player vendor's NPC AI type to AI_PLAYERVENDOR (17) and make it invulnerable
			npcVendor.aitype = 17;
			npcVendor.vulnerable = false;

			// Set direction of player vendor to match the direction the player is facing
			npcVendor.direction = pUser.direction;

			// Set player as owner of the player vendor
			npcVendor.owner = pUser;

			// Add the player vendor to the house's list of vendors
			iMulti.AddVendor( npcVendor );
			npcVendor.SetTag( "VendorBankAccount",0 );
			npcVendor.AddScriptTrigger( 3110 );
			npcVendor.StartTimer( 60000, 90, 3110 );
			var vendorName = iUsed.GetTag( "vendorName" );
			if( vendorName )
			{
				npcVendor.name = vendorName;
			}

			// Delete the player vendor deed!
			iUsed.Delete();

			// Inform the player that they have placed a new player vendor
			var npcMsg = GetDictionaryEntry( 388, pSocket.language ); // Hello sir! My name is %s and I will be working for you.
			npcVendor.TextMessage( npcMsg.replace(/%s/gi, npcVendor.name ));
		}
	}

	return false;
}

/** @type { ( srcChar: Character, trgChar: Character, pSocket: Socket ) => boolean } */
function CheckForNPCAtVendorSpot( srcChar, trgChar, pSocket )
{
	if( !ValidateObject( srcChar ) || !ValidateObject( trgChar ))
		return false;

	// Ignore the player placing the vendor
	if( trgChar.serial == srcChar.serial )
		return false;

	// Only block NPCs already standing on the exact placement tile
	if( trgChar.npc && trgChar.x == srcChar.x && trgChar.y == srcChar.y && trgChar.z == srcChar.z )
		return true;

	return false;
}

function CheckForNearbyDoors( pUser, itemToCheck, pSocket )
{
	if( ValidateObject( itemToCheck ))
	{
		if(( itemToCheck.type == 12 || itemToCheck.type == 13 ))
		{
			if( itemToCheck.z > pUser.z && itemToCheck.z - pUser.z >= 20 )
			{
				// Ignore doors on floors above
				return false;
			}
			else if( itemToCheck.z < pUser.z && pUser.z - itemToCheck.z >= 20 )
			{
				// Ignore doors on floors below, too!
				return false;
			}

			if( itemToCheck.isDoorOpen )
			{
				// Make sure to check against the distance from the door in it's closed state, rather than it's open state!
				var origX = itemToCheck.x  - itemToCheck.GetTag( "DOOR_X" );
				var origY = itemToCheck.y  - itemToCheck.GetTag( "DOOR_Y" );
				if( Math.abs( pUser.x - origX ) < 1 && Math.abs( pUser.y - origY ) < 1 )
				return true;
			}

			if( pUser.DistanceTo( itemToCheck ) <= 1 )
				return true;
		}
	}
	return false;
}
