function CommandRegistration()
{
	RegisterCommand( "xteleport", 1, true );
}

function command_XTELEPORT( socket, cmdString )
{
	if( cmdString )
	{
		var toTele;
		var splitString = cmdString.split( " " );
		if( splitString[3] )
		{
			var a1 = StringToNum( splitString[0] );
			var a2 = StringToNum( splitString[1] );
			var a3 = StringToNum( splitString[2] );
			var a4 = StringToNum( splitString[3] );
			toTele = CalcCharFromSer( a1, a2, a3, a4 );
			if( toTele )
				toTele.SetLocation( socket );
			return;
		}
		else if( splitString[0] )
		{
			var ourNum = StringToNum( splitString[0] );
			if( splitString[0].match( /0x/i ) )
			{
				toTele = CalcCharFromSer( ourNum );
				if( toTele )
					toTele.SetLocation( socket );
			}
			else
			{
				var tSock = GetSocketFromIndex( ourNum );
				if( tSock )
				{
					toTele = tSock.currentChar;
					if( toTele )
						toTele.SetLocation( socket );
				}
			}
			return;
		}
	}

	var targMsg = GetDictionaryEntry( 21, socket.Language );
	socket.CustomTarget( 0, targMsg );
}

function onCallback0( socket, ourObj )
{
	if( !socket.GetWord( 1 ) )
		ourObj.SetLocation( socket );
}
