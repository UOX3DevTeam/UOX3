/// <reference path="../definitions.d.ts" />
// @ts-check
// Attempts to "fix" the Z position of a character stuck in the ground
function CommandRegistration()
{
	RegisterCommand( "fix", 4, true );
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
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
