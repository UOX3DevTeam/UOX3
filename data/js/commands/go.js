// This command is used to quickly teleport to a specific place, character, or set of coordinates

function CommandRegistration()
{
	RegisterCommand( "go", 1, true );
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
				{
					mChar.SetLocation( parseInt( splitString[1] ));
				}
				break;
			case "CHAR":
				var teleTo;
				if( splitString[4] )	// 4-byte Serial
				{
					var a1 = parseInt( splitString[1] );
					var a2 = parseInt( splitString[2] );
					var a3 = parseInt( splitString[3] );
					var a4 = parseInt( splitString[4] );
					teleTo = CalcCharFromSer( a1, a2, a3, a4 );
				}
				else if( splitString[1] )
				{
					var ourNum = parseInt( splitString[1] );
					if( splitString[1].match( /0x/i ))	// Whole Serial
					{
						teleTo = CalcCharFromSer( ourNum );
					}
					else	// Socket index
					{
						var tSock = GetSocketFromIndex( ourNum );
						if( tSock )
						{
							teleTo = tSock.currentChar;
						}
					}
				}

				if( teleTo )
				{
					if( teleTo.commandLevel > mChar.commandLevel )
					{
						socket.SysMessage( GetDictionaryEntry( 19, socket.language )); // You cannot move to your betters!
					}
					else
					{
						mChar.SetLocation( teleTo );
					}
				}
				else
				{
					socket.SysMessage( GetDictionaryEntry( 1110, socket.language )); // No such character exists!
				}
				break;
			default:
				var x = 0, y = 0, z = 0;
				var worldNumber = mChar.worldnumber;
				var instanceID = mChar.instanceID;
				if( splitString[4] )
				{
					instanceID = parseInt( splitString[4] );
				}

				if( splitString[3] )
				{
					worldNumber = parseInt( splitString[3] );
				}

				if( splitString[2] )
				{
					x = parseInt( splitString[0] );
					y = parseInt( splitString[1] );
					z = parseInt( splitString[2] );
				}
				else
				{
					return;
				}
				mChar.SetLocation( x, y, z, worldNumber, instanceID );
				break;
		}
	}
	else if( mChar )
	{
		mChar.ExecuteCommand( "itemmenu 6000" );
	}
}
