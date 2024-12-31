function onQuestToggle( pUser, iUsed )
{
	var socket = pUser.socket;

	if( !ValidateObject( pUser ))
	{
		pUser.SysMessage( "Invalid pUser." );
		return false;
	}

	socket.CustomTarget( 1, "Target the item you wish to toggle Quest Item status on <ESC> to cancel" );
	return true;
}

function onCallback1( socket, iTarget )
{
	var pUser = socket.currentChar;

	if( !iTarget || !iTarget.isItem )
	{
		pUser.SysMessage( "That is not a valid item." );
		return;
	}

	var itemOwner = GetPackOwner( iTarget, 0 );
	if( !itemOwner || itemOwner.serial !== pUser.serial )
	{
		pUser.SysMessage( "The item must be in your backpack to toggle its quest status." );
		return;
	}

	// Check if the item is already marked as a quest item
	if( !iTarget.GetTag( "QuestItem" ))
	{
		// Notify collection
		TriggerEvent( 5800, "onItemCollected", pUser, iTarget, false ); // Toggle on
	}
	else
	{
		// Notify uncollection
		TriggerEvent( 5800, "onItemCollected", pUser, iTarget, true ); // Toggle off
	}

	//repeat the targetting
	socket.CustomTarget( 1, "Target the item you wish to toggle Quest Item status on <ESC> to cancel" );
}