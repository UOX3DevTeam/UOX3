/// <reference path="../../../definitions.d.ts" />
// @ts-check

// Script attached to quest items
/** @type { ( equipper: Character, equipping: Item ) => boolean } */
function onEquipAttempt( pEquipper, iEquipped )
{
	TriggerEvent( 5800, "EquipAttempt", pEquipper, iEquipped );
}

/** @type { ( item: Item, dropper: Character ) => number } */
function onDrop( iDropped, pDropper )
{
	var pSocket = pDropper.socket;
	if( pSocket == null )
	{
		return 0;
	}

	// Fetch the serial of the drop target
	var temp1 = pSocket.GetByte( 10 );
	var temp2 = pSocket.GetByte( 11 );
	var temp3 = pSocket.GetByte( 12 );
	var temp4 = pSocket.GetByte( 13 );

	if( temp2 == 0xFF )
	{ // do not allow Drop on the ground
		pSocket.SysMessage( GetDictionaryEntry( 19631, pSocket.language ));//Quest items cannot be dropped on the ground.
		return 0; // Prevent ground drop
	}

	return 1; // Default allow for other cases
}

/** @type { ( item: Item, dropper: Character, dest: Item ) => number } */
function onDropItemOnItem( iDropped, cDropper, iDroppedOn )
{
	var pSocket = cDropper.socket;
	if( pSocket == null )
	{
		return 0;
	}

	// Check if the dropped item is marked as a quest item
	if( !iDropped.GetTag( "QuestItem" ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 19632, pSocket.language )); // This item is not marked as a quest item.
		return 1;
	}

	// Check if the item is being dropped on another quest item
	if( iDroppedOn.GetTag( "QuestItem" ))
	{
		// Allow stacking if both items are pileable and of the same type
		if( iDropped.isPileable && iDroppedOn.isPileable && iDropped.sectionID == iDroppedOn.sectionID )
		{
			return 1;
		}
		else
		{
			pSocket.SysMessage( GetDictionaryEntry( 19633, pSocket.language )); // You can only drop quest items into the top-most level of your backpack while you still need them for your quest.
			return 0;
		}
	}

	if( iDroppedOn.layer != 21 ) 
	{
		pSocket.SysMessage( GetDictionaryEntry( 19633, pSocket.language )); // You can only drop quest items into the top-most level of your backpack while you still need them for your quest.
		return 0;
	}

	// Find owner of root container iDropped is contained in, if any
	var packOwner = GetPackOwner( iDropped, 0 );
	if( packOwner != null && packOwner.serial != cDropper.serial )
	{
		pSocket.SysMessage( GetDictionaryEntry( 19633, pSocket.language )); // You can only drop quest items into the top-most level of your backpack while you still need them for your quest.
		return 0;
	}

	return 1;
}

/** @type { ( item: Item, pickerUpper: Character, objCont: BaseObject ) => boolean } */
function onPickup( iPickedUp, pGrabber, containerObj )
{
	var pSocket = pGrabber.socket;
	if( pSocket == null )
	{
		return false;
	}

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
		var pickupAmount = pSocket.GetWord( 5 ); // Assuming this retrieves the amount being picked up

		if( pickupAmount !== totalAmount )
		{
			pSocket.SysMessage( GetDictionaryEntry( 19634, pSocket.language )); // Stacks of quest items cannot be unstacked.
			return false; // Prevent partial stack pickup
		}
	}

	return true; // Allow the pickup
}

/** @type { ( user: Character, iUsing: Item ) => boolean } */
function onUseChecked( pUser, iUsed ) 
{
	var pSocket = pUser.socket;
	if( pSocket == null )
	{
		return false;
	}

	if( iUsed.GetTag( "QuestItem" ))
	{
		pUser.TextMessage( GetDictionaryEntry( 19635, pSocket.language )); // Quest items cannot be used in this way.
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
