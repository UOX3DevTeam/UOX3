function CommandRegistration()
{
	RegisterCommand( "xteleport", 1, true );
	RegisterCommand( "xgo", 1, true );
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

function command_XGO( socket, cmdString )
{
	if( cmdString )
	{
		var targMsg;
		var splitString = cmdString.split( " " );
		if( splitString[2] )
		{
			socket.clickX = StringToNum( splitString[0] );
			socket.clickY = StringToNum( splitString[1] );
			socket.clickZ = StringToNum( splitString[2] );

			targMsg = GetDictionaryEntry( 198, socket.Language );
			socket.CustomTarget( 1, targMsg );
		}
		else if( splitString[0] )
		{
			socket.tempint = StringToNum( cmdString );

			targMsg = GetDictionaryEntry( 20, socket.Language );
			socket.CustomTarget( 2, targMsg );
		}
	}
}

function onCallback1( socket, ourObj )
{
	if( !socket.GetWord( 1 ) )
		ourObj.SetLocation( socket.clickX, socket.clickY, socket.clickZ );

	socket.clickX = -1;
	socket.clickY = -1;
	socket.clickZ = -1;
}

function onCallback2( socket, ourObj )
{
	if( !socket.GetWord( 1 ) )
		ourObj.SetLocation( socket.tempint );

	socket.tempint = 0;
}
