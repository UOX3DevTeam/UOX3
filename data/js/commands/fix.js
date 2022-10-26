// Attempts to "fix" the Z position of a character stuck in the ground
function CommandRegistration()
{
	RegisterCommand( "fix", 1, true );
}

function command_FIX( socket, cmdString )
{
	var mChar = socket.currentChar;
	if( mChar )
	{
		if( cmdString )
		{
			mChar.z = parseInt( cmdString );
		}
		else
		{
			mChar.z = ValidTelePos( mChar );
		}
	}
}

function ValidTelePos( mChar )
{
	var z = -1;
	var tX = mChar.x;
	var tY = mChar.y;
	if( tX >= 1397 && tX <= 1400 && tY >= 1622 && tY <= 1630 )
	{
		z = 28;
	}
	if( tX >= 1510 && tX <= 1537 && tY >= 1455 && tY <= 1456 )
	{
		z = 15;
	}
	return z;
}
