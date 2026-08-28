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
		if( itemOwner == null || itemOwner.serial != pUser.serial )
		{
			pUser.SysMessage( GetDictionaryEntry( 1763, pSocket.language )); // That item must be in your backpack before it can be used.
			return false;
		}

		// Check if player is actually in a house
		var iMulti = pUser.multi;
		if( !ValidateObject( iMulti ) || !iMulti.IsInMulti( pUser ))
		{
			// Player vendors can only be placed in houses!
			pUser.SysMessage( "Mannequins can only be placed in houses!"); // Mannequins can only be placed in houses!
			return false;
		}

		// Make sure player has access to actually placing a player vendor in the house
		if( !iMulti.IsOwner( pUser ))
		{
			// Only the house owner can place player vendors in a house!
			pUser.SysMessage( "Only the house owner can place Mannequins in a house!"); // Only the house owner can place Mannequins in a house!
			return false;
		}

		// Make sure player is not trying to place the Mannequins too close to a door!
		// Check for nearby doors
		var foundDoor = AreaItemFunction( "CheckForNearbyDoors", pUser, 3, pSocket );
		if( foundDoor )
		{
			pSocket.SysMessage( "You cannot place a Mannequins adjacent to a door!" ); // You cannot place a Mannequins adjacent to a door!
			return false;
		}

		// Create player vendor at player's current location
		var npcMannequin = SpawnNPC( "mannequin", pUser.x, pUser.y, pUser.z, pUser.worldnumber, pUser.instanceID );
		if( ValidateObject( npcMannequin ))
		{
			npcMannequin.vulnerable = false;

			// Set direction of mannequin to match the direction the player is facing
			npcMannequin.direction = 4;

			// Set player as owner of the mannequin
			npcMannequin.owner = pUser;
			npcMannequin.wandertype = 5;
			npcMannequin.squelch = true;
			npcMannequin.willhunger = false;
			npcMannequin.willthirst = false;
			npcMannequin.aitype = 0;
			npcMannequin.tamed = false;
			npcMannequin.Teleport(pUser);
			npcMannequin.SetTag("Mannequin", 1);
			npcMannequin.SetTag("MannequinOwner", pUser.serial);

			// Delete the player vendor deed!
			iUsed.Delete();
		}
	}

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
				if( pUser.x - origX < 2 || origX - pUser.x < 2 || pUser.y - origY < 2 || origY - pUser.y < 2 )
					return true;
			}

			if( pUser.DistanceTo( itemToCheck ) <= 2 )
				return true;
		}
	}
	return false;
}
