function CommandRegistration()
{
	RegisterCommand( "telestuff", 1, true );
}

function command_TELESTUFF( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 250, socket.Language );
	socket.CustomTarget( 0, targMsg );
}

function onCallback0( socket, ourObj )
{
	var targMsg;
	if( ourObj.isChar )
		targMsg = GetDictionaryEntry( 1045, socket.Language );
	else
		targMsg = GetDictionaryEntry( 1046, socket.Language );

	socket.tempObj = ourObj;
	socket.CustomTarget( 1, targMsg );
}

function onCallback1( socket, ourObj )
{
	var dictMsg;
	var toTele = socket.tempObj;
	if( toTele )
	{
		if( toTele.isChar )
			dictMsg = GetDictionaryEntry( 1047, socket.Language );
		else
			dictMsg = GetDictionaryEntry( 1048, socket.Language );

		var x = socket.GetWord( 11 );
		var y = socket.GetWord( 13 );
		var z = socket.GetByte( 16 ) + GetTileHeight( socket.GetWord( 17 ) );
		toTele.SetLocation( x, y, z );
		socket.SysMessage( dictMsg );
	}
	socket.tempObj = null;
}
