// Strongbox
function onUseChecked( pUser, iUsed )
{
	var pSocket = pUser.socket;

	// Get multi
	var iMulti = iUsed.multi;
	if( ValidateObject( iMulti ))
	{
		if(( ValidateObject( iUsed.owner ) && iUsed.owner == pUser ) || ( ValidateObject( iMulti.owner ) && iMulti.owner == pUser ))
		{
			// Allow access, execute regular hardcoded container code
			return true;
		}
		else
		{
			pSocket.SysMessage( GetDictionaryEntry( 1933, pSocket.language)); // You have no access to this strongbox!
			return false; // don't handle doubleclick in code
		}
	}

	// container is not in a multi, so let players open it like any other container
	return true;
}

function onDropItemOnItem( iDropped, pDropper, iDroppedOn )
{
	if( !ValidateObject( iDropped )	|| !ValidateObject( pDropper ) || !ValidateObject( iDroppedOn ))
		return 0;

	var iMulti = iDroppedOn.multi;
	if( ValidateObject( iMulti ))
	{
		if(( !ValidateObject( iDroppedOn.owner ) || iDroppedOn.owner != pDropper ) && ( !ValidateObject( iMulti.owner ) || iMulti.owner != pDropper ))
		{
			// Don't allow anyone other than owner of strongbox or owner of multi to drop items in container
			return 0;
		}
	}

	var iDroppedOnItemCount = iDroppedOn.totalItemCount;
	var iDroppedOnMaxItems = iDroppedOn.maxItems;
	if( iDroppedOnItemCount >= iDroppedOnMaxItems || iDroppedOnItemCount + Math.max( 1, 1 + iDropped.totalItemCount ) > iDroppedOnMaxItems )
	{
		pDropper.SysMessage( GetDictionaryEntry( 1818, pDropper.socket.language )); // The container is already at capacity.
		return 0;
	}

	iPickupSpot = pDropper.socket.pickupSpot;
	iWeight = iDropped.weight;

	// Move item into container
 	iDropped.container = iDroppedOn;

    // Subtract weight if item dropped was picked up from one of these spots:
    // pickupspot 1 = ground, 3 = otherpack, 5 = bank
    // In other cases it's handled by code when item gets deleted
    if( iPickupSpot == 1 || iPickupSpot == 3 || iPickupSpot == 5 )
    {
    	if( iWeight == 0 || iWeight == 25500 )
    	{
			pDropper.weight = pDropper.weight - 25500;
    	}
		else
		{
			pDropper.weight = pDropper.weight - iWeight;
		}
	}

	//return 0; // bounce
	//return 1; // Use hard code, don't bounce item
	return 2; // Don't use hard code, don't bounce item
}