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

		var x 		= mChar.x.toString();
		var y 		= mChar.y.toString();
		var z 		= mChar.z.toString();
		var worldNum 	= mChar.worldnumber.toString();
		var instanceID 	= mChar.instanceID;
		socket.SysMessage( x + " " + y + " " + z + " " + worldNum + " " + instanceID );
	}
}
