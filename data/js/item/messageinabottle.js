function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && ValidateObject( iUsed ) && iUsed.isItem )
	{
		var itemOwner = GetPackOwner( iUsed, 0 );
		if( itemOwner == null || itemOwner.serial != pUser.serial )
		{
			pUser.SysMessage( GetDictionaryEntry( 1763, socket.language )); // That item must be in your backpack before it can be used.
			return false;
		}

		socket.SysMessage( GetDictionaryEntry( 9320, socket.language )); // You extract the message from the bottle.
		var itemToCreate = "waterstainedsos";
		/*if( GetServerSetting( "CLIENTSUPPORT70851" )) // TODO: Add this
		{
			itemToCreate = "waterstatinedsos_tol"; // 0xa30c is a dedicated message in a bottle item, but only available in clients ~7.0.85.1 and later
		}*/

		var waterStainedSOS = CreateDFNItem( socket, pUser, itemToCreate, 1, "ITEM", true );
		if( ValidateObject( waterStainedSOS ))
		{
			iUsed.Delete();
		}
	}
	return false;
}