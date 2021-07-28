// Guildstone Deed/Stone
function onUseChecked( pUser, iUsed )
{
	// Check if object is in player's backpack
	var pSocket = pUser.socket;
	if ( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}
	if( pSocket != null && ValidateObject( iUsed ) && iUsed.isItem )
	{
		if( iUsed.id == 0x14F0 || iUsed.id == 0x1869 )
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
				pUser.SysMessage( GetDictionaryEntry( 2723, pSocket.language )); // Guildstones can only be placed in houses!
				return false;
			}

			// Make sure player has access to actually placing a guildstone in the house
			if( !iMulti.IsOwner( pUser ))
			{
				// Only the house owner can guildstones in a house!
				pUser.SysMessage( GetDictionaryEntry( 2724, pSocket.language )); // Only the house owner can place a guildstone in a house!
				return false;
			}

			// Check that the player is not already in a guild
			if( iUsed.id == 0x14F0 && pUser.guild != null )
			{
				pSocket.SysMessage( GetDictionaryEntry( 173, pSocket.language )); // You are already in a guild!
				return false;
			}

			// Check that the house doesn't already have a guildstone in it
			var tempItem;
			for( tempItem = iMulti.FirstItem(); !iMulti.FinishedItems(); tempItem = iMulti.NextItem() )
			{
				if( !ValidateObject( tempItem ))
					continue;

				if( tempItem.type == 202 && tempItem.id == 0x0ED5 )
				{
					pSocket.SysMessage( GetDictionaryEntry( 2725, pSocket.language )); // You cannot place any additional guildstones in this house!
					return false;
				}
			}

			// Make sure player is not trying to place the guildstone too close to a door!
			// Check for nearby doors
			var foundDoor = AreaItemFunction( "checkForNearbyDoors", pUser, 3, pSocket );
			if( foundDoor )
			{
				pSocket.SysMessage( GetDictionaryEntry( 2726, pSocket.language )); // You cannot place a guildstone adjacent to a door!
				return false;
			}

			// TODO - Move actual creation of guild and guildstone to this script as well.
		}

		// All good. Handle guild creation/guildstone menu in code for now
		return true;
	}

	return false;
}

function checkForNearbyDoors( pUser, itemToCheck, pSocket )
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