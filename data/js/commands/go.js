function CommandRegistration(contextID)
{
	RegisterCommand( "go", 1, true ,contextID);
}

function command_GO( socket, cmdString )
{
	var mChar = socket.currentChar;
	if( cmdString && mChar )
	{
		var splitString = cmdString.split( " " );
		switch( splitString[0].toUpperCase() )
		{
		case "PLACE":
			if( splitString[1] )
				mChar.SetLocation( StringToNum( splitString[1] ) );
			break;
		case "CHAR":
			var teleTo;
			if( splitString[4] )	// 4-byte Serial
			{
				var a1 = StringToNum( splitString[1] );
				var a2 = StringToNum( splitString[2] );
				var a3 = StringToNum( splitString[3] );
				var a4 = StringToNum( splitString[4] );
				teleTo = CalcCharFromSer( a1, a2, a3, a4 );
			}
			else if( splitString[1] )
			{
				var ourNum = StringToNum( splitString[1] );
				if( splitString[1].match( /0x/i ) )	// Whole Serial
					teleTo = CalcCharFromSer( ourNum );
				else	// Socket index
				{
					var tSock = GetSocketFromIndex( ourNum );
					if( tSock )
						teleTo = tSock.currentChar;
				}
			}

			if( teleTo )
			{
				if( teleTo.commandLevel > mChar.commandLevel )
					socket.SysMessage( GetDictionaryEntry( 19, socket.Language ) );
				else
					mChar.SetLocation( teleTo );
			}
			else
				socket.SysMessage( GetDictionaryEntry( 1110, socket.Language ) );
			break;
		default:
			if( splitString[2] )
			{
				var x = StringToNum( splitString[0] );
				var y = StringToNum( splitString[1] );
				var z = StringToNum( splitString[2] );
				mChar.SetLocation( x, y, z );
			}
			break;
		}
	}
}
