// These commands are used for teleporting targeted objects to user's location, or teleport them
// to a specified location

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
			var a1 = parseInt( splitString[0] );
			var a2 = parseInt( splitString[1] );
			var a3 = parseInt( splitString[2] );
			var a4 = parseInt( splitString[3] );
			toTele = CalcCharFromSer( a1, a2, a3, a4 );
			if( toTele )
			{
				toTele.SetLocation( socket );
			}
			return;
		}
		else if( splitString[0] )
		{
			var ourNum = parseInt( splitString[0] );
			if( splitString[0].match( /0x/i ) )
			{
				toTele = CalcCharFromSer( ourNum );
				if( toTele )
				{
					toTele.SetLocation( socket );
				}
			}
			else
			{
				var tSock = GetSocketFromIndex( ourNum );
				if( tSock )
				{
					toTele = tSock.currentChar;
					if( toTele )
					{
						toTele.SetLocation( socket );
					}
				}
			}
			return;
		}
	}

	var targMsg = GetDictionaryEntry( 21, socket.language ); // Select char to teleport to your position.
	socket.CustomTarget( 0, targMsg );
}

function onCallback0( socket, ourObj )
{
	if( !socket.GetWord( 1 ))
	{
		ourObj.SetLocation( socket );
	}
}

function command_XGO( socket, cmdString )
{
	if( cmdString )
	{
		var targMsg;
		var splitString = cmdString.split( " " );
		if( splitString[2] )
		{
			socket.clickX = parseInt( splitString[0] );
			socket.clickY = parseInt( splitString[1] );
			socket.clickZ = parseInt( splitString[2] );

			if( splitString[3] ) // world
			{
				socket.xText = splitString[3];
			}
			if( splitString[4] ) // instance
			{
				socket.xText += " " + splitString[4];
			}

			targMsg = GetDictionaryEntry( 198, socket.language ); // Select char to teleport.
			socket.CustomTarget( 1, targMsg );
		}
		else if( splitString[0] )
		{
			socket.clickX = parseInt( cmdString );

			targMsg = GetDictionaryEntry( 20, socket.language ); // Select char to teleport
			socket.CustomTarget( 2, targMsg );
		}
	}
}

function onCallback1( socket, ourObj )
{
	if( !socket.GetWord( 1 ))
	{
		// Defaults to same world/instance as GM unless anything else is specified
		var worldNum = socket.currentChar.worldnumber;
		var instanceID = socket.currentChar.instanceid;

		var splitString = socket.xText.split( " " );
		if( splitString[0] )
		{
			worldNum = parseInt( splitString[0] );
		}
		if( splitString[1] )
		{
			instanceID = parseInt( splitString[1] );
		}

		ourObj.SetLocation( socket.clickX, socket.clickY, socket.clickZ, worldNum, instanceID );
	}

	socket.clickX = -1;
	socket.clickY = -1;
	socket.clickZ = -1;
	socket.xText = null;
}

function onCallback2( socket, ourObj )
{
	if( !socket.GetWord( 1 ))
	{
		ourObj.SetLocation( socket.clickX );
	}

	socket.clickX = -1;
}
