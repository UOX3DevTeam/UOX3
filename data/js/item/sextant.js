function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && ValidateObject( iUsed ) && iUsed.isItem )
	{
		var itemOwner = GetPackOwner( iUsed, 0 );
		if( itemOwner == null || itemOwner.serial != pUser.serial )
		{
			socket.SysMessage( GetDictionaryEntry( 1763, socket.language )); // That item must be in your backpack before it can be used.
			return false;
		}

		var mapCoords = TriggerEvent( 2503, "GetMapCoordinates", iUsed.morex, iUsed.morey, iUsed.morez );
		var mapCoordsString = mapCoords[3] + "o " + mapCoords[4] + "'" + ( mapCoords[5] ? "S" : "N" ) + " " + mapCoords[0] + "o " + mapCoords[1] + "'" + ( mapCoords[2] ? "E" : "W" );
		var finalString = GetDictionaryEntry( 274, socket.language ); // You are at %s
		finalString = ( finalString.replace( /%s/gi, mapCoordsString ));
		socket.SysMessage( finalString );
	}
	return false;
}