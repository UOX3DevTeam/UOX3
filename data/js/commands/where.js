function CommandRegistration()
{
	RegisterCommand( "where", 1, true );
}

function command_WHERE( socket, cmdString )
{
	var mChar = socket.currentChar;
	if( mChar )
	{
		var mRegion = mChar.region;
		if( mRegion && mRegion.name )
			socket.SysMessage( "You are at: " + mRegion.name );
		else
			socket.SysMessage( "You are nowhere" );

		var x 		= NumToString( mChar.x );
		var y 		= NumToString( mChar.y );
		var z 		= NumToString( mChar.z );
		var worldNum 	= NumToString( mChar.worldnumber );
		socket.SysMessage( x + " " + y + " " + z + " " + worldNum );
	}
}
