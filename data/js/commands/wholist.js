// This command opens a list of all players on the shard
// Without arguments it lists all online players
// With an argument of "off" it lists all offline players instead

function CommandRegistration()
{
	RegisterCommand( "wholist", 1, true );
}

function command_WHOLIST( socket, cmdString )
{
	var sendOnList = true;
	if( cmdString )
	{
		if( cmdString.toUpperCase() == "OFF" )
		{
			sendOnList = false;
		}
	}
	
	socket.WhoList( sendOnList );
}
