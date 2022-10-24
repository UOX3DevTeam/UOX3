// This command opens a gate to a specific location
function CommandRegistration()
{
	RegisterCommand( "xgate", 1, true );
}

function command_XGATE( socket, cmdString )
{
	var mChar = socket.currentChar;
	if( mChar && cmdString )
	{
		var splitString = cmdString.split( " " );
		if( splitString[2] )
		{
			var destX = parseInt( splitString[0] );
			var destY = parseInt( splitString[1] );
			var destZ = parseInt( splitString[2] );
			var destWorld = mChar.worldnumber;
			if( splitString[3] )
			{
				destWorld = parseInt( splitString[3] );
			}

			mChar.Gate( destX, destY, destZ, destWorld );
		}
		else if( splitString[0] )
		{
			var ourNum = parseInt( splitString[0] );
			if( splitString[0].match( /0x/i ))
			{
				var toTele = CalcItemFromSer( ourNum );
				if( toTele )
				{
					mChar.Gate( toTele );
				}
			}
			else
			{
				mChar.Gate( ourNum );
			}
		}
	}
}



