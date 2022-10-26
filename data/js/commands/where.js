// Reports the command user's current in-game coordinates and region name

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
		{
			var tempMsg = GetDictionaryEntry( 8000, socket.language ); // You are at: %s
			socket.SysMessage( tempMsg.replace( /%s/gi, mRegion.name ));
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 8001, socket.language )); // You are nowhere
		}

		var x 		= mChar.x.toString();
		var y 		= mChar.y.toString();
		var z 		= mChar.z.toString();
		var worldNum 	= mChar.worldnumber.toString();
		var instanceID 	= mChar.instanceID;
		socket.SysMessage( x + " " + y + " " + z + " " + worldNum + " " + instanceID );
	}
}
