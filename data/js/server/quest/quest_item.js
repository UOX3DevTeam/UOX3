/// <reference path="../../../definitions.d.ts" />
// @ts-check

// Script attached to quest items
function onEquipAttempt( pEquipper, iEquipped )
{
	TriggerEvent( 5800, "EquipAttempt", pEquipper, iEquipped );
}

function onDrop( iDropped, pDropper )
{
	var pSock = pDropper.socket;

	// Fetch the serial of the drop target
	var temp1 = pSock.GetByte( 10 );
	var temp2 = pSock.GetByte( 11 );
	var temp3 = pSock.GetByte( 12 );
	var temp4 = pSock.GetByte( 13 );

	if( temp2 == 0xFF )
	{ // do not allow Drop on the ground
		pSock.SysMessage( GetDictionaryEntry( 19631, pSock.language ));//Quest items cannot be dropped on the ground.
		return 0; // Prevent ground drop
	}

	return 1; // Default allow for other cases
}

/** @type { ( item: Item, dropper: Character, dest: Item ) => number } */
function onDropItemOnItem( iDropped, cDropper, iDroppedOn )
{
	var pSock = cDropper.socket;
	// Check if the dropped item is marked as a quest item
	if( !iDropped.GetTag( "QuestItem" ))
	{
		pSock.SysMessage( GetDictionaryEntry( 19632, pSock.language )); // This item is not marked as a quest item.
		return true;
	}

	// Check if the item is being dropped on another quest item
	if( iDroppedOn.GetTag( "QuestItem" ))
	{
		// Allow stacking if both items are pileable and of the same type
		if( iDropped.isPileable && iDroppedOn.isPileable && iDropped.sectionID == iDroppedOn.sectionID )
		{
			return true;
		}
		else
		{
			pSock.SysMessage( GetDictionaryEntry( 19633, pSock.language )); // You can only drop quest items into the top-most level of your backpack while you still need them for your quest.
			return false;
		}
	}

	if( iDroppedOn.layer != 21 ) 
	{
		pSock.SysMessage( GetDictionaryEntry( 19633, pSock.language )); // You can only drop quest items into the top-most level of your backpack while you still need them for your quest.

		return false;
	}

	// Find owner of root container iDropped is contained in, if any
	var packOwner = GetPackOwner( iDropped, 0 );
	if( packOwner != null && packOwner.serial != cDropper.serial )
	{
		pSock.SysMessage( GetDictionaryEntry( 19633, pSock.language )); // You can only drop quest items into the top-most level of your backpack while you still need them for your quest.
		return false;
	}

	return true;
}

function onPickup( iPickedUp, pGrabber, containerObj )
{
	var pSock = pGrabber.socket;

	// Check if the item is marked as a quest item
	if( !iPickedUp.GetTag( "QuestItem" ))
	{
		return true; // Allow normal pickup for non-quest items
	}

	// Check if the item is pileable ( stackable )
	if( iPickedUp.isPileable )
	{
		// Handle stackable quest items
		var totalAmount = iPickedUp.amount; // Default to 1 if amount is not set
		var pickupAmount = pSock.GetWord( 5 ); // Assuming this retrieves the amount being picked up

		if( pickupAmount !== totalAmount )
		{
			pSock.SysMessage( GetDictionaryEntry( 19634, pSock.language )); // Stacks of quest items cannot be unstacked.
			return false; // Prevent partial stack pickup
		}
	}

	return true; // Allow the pickup
}

function onUseChecked( pUser, iUsed ) 
{
	var pSock = pUser.socket;
	if( iUsed.GetTag( "QuestItem" ))
	{
		pUser.TextMessage( GetDictionaryEntry( 19635, pSock.language )); // Quest items cannot be used in this way.
		return false; // Prevent usage
	}

	return true; // Allow usage for non-quest items
}

/** @type { ( myObj: BaseObject, pSocket: Socket ) => string } */
function onTooltip( questitem, pSocket ) 
{
	var tooltipText = "[" + GetDictionaryEntry( 19636, pSocket.language ) + "]"; // Quest Item
	questitem.SetTempTag( "tooltipSortOrder", 1 );
	return tooltipText;
}