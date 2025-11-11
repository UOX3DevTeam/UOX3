/// <reference path="../definitions.d.ts" />
// @ts-check
/** @type { ( user: Character, iUsing: Item ) => boolean } */
function onUseChecked( pUser, iUsed )
{
	var pSocket = pUser.socket;
	if( socket && ValidateObject( iUsed ) && iUsed.isItem )
	{
		var itemOwner = GetPackOwner( iUsed, 0 );
		if( itemOwner == null || itemOwner.serial != pUser.serial )
		{
			pSocket.SysMessage( GetDictionaryEntry( 1763, pSocket.language )); // That item must be in your backpack before it can be used.
			return false;
		}

		pSocket.SysMessage( GetDictionaryEntry( 9320, pSocket.language )); // You extract the message from the bottle.
		var itemToCreate = "waterstainedsos";
		/*if( GetServerSetting( "CLIENTSUPPORT70851" )) // TODO: Add this
		{
			itemToCreate = "waterstatinedsos_tol"; // 0xa30c is a dedicated message in a bottle item, but only available in clients ~7.0.85.1 and later
		}*/

		var waterStainedSOS = CreateDFNItem( pSocket, pUser, itemToCreate, 1, "ITEM", true );
		if( ValidateObject( waterStainedSOS ))
		{
			iUsed.Delete();
		}
	}
	return false;
}
