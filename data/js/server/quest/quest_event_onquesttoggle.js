/// <reference path="../../../definitions.d.ts" />
// @ts-check

function onQuestToggle( pUser, iUsed )
{
	var socket = pUser.socket;

	if( !ValidateObject( pUser ))
	{
		return false;
	}

	socket.CustomTarget( 1, GetDictionaryEntry( 19600, socket.language ));//Target the item you wish to toggle Quest Item status on <ESC> to cancel
	return true;
}

/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
function onCallback1( socket, iTarget )
{
	var pUser = socket.currentChar;

	if( !ValidateObject(iTarget) || !iTarget.isItem )
	{
		socket.SysMessage( GetDictionaryEntry( 711, socket.language ));//"That is not a valid item."
		return;
	}

	var itemOwner = GetPackOwner( iTarget, 0 );
	if( !itemOwner || itemOwner.serial != pUser.serial )
	{
		socket.SysMessage( GetDictionaryEntry( 19601, socket.language ));//The item must be in your backpack to toggle its quest status.
		return;
	}

	// Check if the item is already marked as a quest item
	if( !iTarget.GetTag( "QuestItem" ))
	{
		// Notify collection
		TriggerEvent( 5800, "ItemCollected", pUser, iTarget, false ); // Toggle on
	}
	else
	{
		// Notify uncollection
		TriggerEvent( 5800, "ItemCollected", pUser, iTarget, true ); // Toggle off
	}

	//repeat the targetting
	socket.CustomTarget( 1, GetDictionaryEntry( 19600, socket.language ));//Target the item you wish to toggle Quest Item status on <ESC> to cancel
}