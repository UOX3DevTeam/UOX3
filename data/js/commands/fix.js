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
			mChar.z = GetMapElevation( mChar.x, mChar.y, mChar.worldnumber );
		}
	}
}
