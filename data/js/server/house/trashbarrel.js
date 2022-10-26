// Trash Barrel, adapted from Xuri's Device-For-Disposal-Of-Unwanted-Items (aka Trash-can) script
// Last Updated: 11. October 2020

// Primarily used for trash barrels placed in player houses, but script can be assigned to
// any item to make said item act as a trash container

// As an added - optional - bonus, the script can be set to give out gold for every item disposed
// If enabled, the amount of gold defaults to the stack-size of the item, but can also be based on
// the weight of the item, or the sell-value of the item divided by two. Only one of the three options
// should be enabled at a time. This has the potential to bring down the amount of items players hoard.

var enableGoldReward = false; 	// Enable or disable gold rewards for disposed items. If enabled, item is instantly deleted
var baseGoldOnStackSize = true;	// Base gold reward on item stack size
var baseGoldOnWeight = false; 	// Base gold reward on item weight
var maxGoldForWeight = 100; 	// Max cap for gold reward based on item weight
var baseGoldOnSellValue = false;// Base gold reward on sell value of item
var sellValueMultipier = 0.5;	// If based on sell value of item, apply this multiplier to gold given out
var trashDisposeTimer = 180000; // 180000 ms = 180.000 seconds = 3 minutes
var maxContainerWeight = 40000; // Max weight trash container can hold

function onDropItemOnItem( iDropped, pDropper, iDroppedOn )
{
	// Safe-guard to stop trashcans from getting deleted if they're dropped into another container
	if( iDropped.type == 87 ) // Trash container
		return 1; // Use hard code, don't bounce item

	var pSocket = pDropper.socket;

	// Grab some vital data before dropped item is deleted
	iPickupSpot = pSocket.pickupSpot;
	iWeight = iDropped.weight;
	iStackSize = iDropped.amount;

	// Give feedback (and potentially gold) to player
	if( enableGoldReward == true )
	{
		var goldAmount = 0;
      	if( baseGoldOnStackSize == true ) // If gold-reward is based on stack-size
      	{
         	goldAmount = iStackSize;
      	}
		else if( baseGoldOnWeight == true ) // If gold-reward is based on item-weight
		{
			if( iDropped.weight != 0 )
			{
	            goldAmount = ( Math.ceil( iDropped.weight / 100 )); //rounds up to nearest whole number
			}
	        else if( Math.ceil( iDropped.weight / 100 ) > 100 )
	        {
	        	goldAmount = maxGoldForWeight; // Cap Gold-for-weight so players cannot abuse weight bugs to gain absurd amount of gold
	        }
			else
			{
	            goldAmount = 1; // Default to 1 gold for items that have no weight defined
			}
      	}
      	else if( baseGoldOnSellValue == true ) // If gold-reward is based on item sell-value
      	{
			if( iDropped.sellvalue != 0 )
			{
				goldAmount = Math.ceil( iDropped.sellvalue / 2 );
			}
      	}

      	// Now, spit out some gold for the player!
      	if( goldAmount > 0 )
      	{
			var newGold = CreateDFNItem( pSocket, pDropper, "0x0EED", goldAmount, "ITEM", true );
			var outMessage = GetDictionaryEntry( 1938, pSocket.language ); // The trash container disposes of unwanted junk and spits out %i gold coins in return!
			iDroppedOn.TextMessage( outMessage.replace( /%i/gi, goldAmount ), false, 0x7ef, 0, pDropper.serial );
      	}
	    else
	    {
      		iDroppedOn.TextMessage( GetDictionaryEntry( 1934, pSocket.language), false, 0x7ef, 0, pDropper.serial ); // The trash container disposes of your worthless junk.
	    }

      	// Finally, delete the item that was dropped, since we already handed out the gold for it!
      	iDropped.Delete();
   	}
   	else
   	{
   		// Gold rewards are not enabled, so let's put the item in the trash container and start
   		// a 3 minute timer before it gets deleted, in case the player changes their mind (or someone
   		// else wants their junk)

   		// But first, let's make sure there's space in the trash container for more items!
   		var iDroppedOnCount = iDroppedOn.totalItemCount;
   		var iDroppedOnMax = iDroppedOn.maxItems;
   		if( iPickupSpot != 2 && ( iDroppedOnCount >= iDroppedOnMax || iDroppedOnCount + Math.max( 1, 1 + iDropped.totalItemCount ) > iDroppedOnMax ))
   		{
   			pDropper.SysMessage( GetDictionaryEntry( 1935, pSocket.language)); // That container cannot hold any more items!
   			return 0;
   		}
   		if( iDropped.multi && !iDroppedOn.multi.IsSecureContainer( iDroppedOn ))
   		{
	   		if( iDroppedOn.weight >= maxContainerWeight || iDroppedOn.weight + iDropped.weight > maxContainerWeight )
	   		{
	   			pDropper.SysMessage( GetDictionaryEntry( 1936, pSocket.language)); // That container cannot hold any more weight!
	   			return 0;
	   		}
   		}

   		// Move item into container
     	iDropped.container = iDroppedOn;

		// First kill any existing timer
		iDroppedOn.KillTimers();

		// Start timer for deleting items in trash container
		iDroppedOn.StartTimer( trashDisposeTimer, 1, true );
		var outMessage = GetDictionaryEntry( 1939, pSocket.language ); // The item will be deleted in %i minutes.
		var timeToDisposal = (trashDisposeTimer / 1000 / 60);
		iDroppedOn.TextMessage( outMessage.replace( /%i/gi, timeToDisposal ), false, 0x7ef, 0, pDropper.serial );
   	}

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

	return 2; // Don't use hard code, don't bounce item
}

function onTimer( iDroppedOn, timerID )
{
	if( timerID == 1 )
	{
		var itemInTrash;
		for( itemInTrash = iDroppedOn.FirstItem(); !iDroppedOn.FinishedItems(); itemInTrash = iDroppedOn.NextItem() )
		{
			if( !ValidateObject( itemInTrash ))
				continue;

			itemInTrash.Delete();
		}

      	iDroppedOn.TextMessage( GetDictionaryEntry( 1937 ), false, 0x7ef ); // The trash container disposes of unwanted junk.
	}
}