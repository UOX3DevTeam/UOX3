function CommandRegistration()
{
	RegisterCommand( "wholist", 1, true );
}

function command_WHOLIST( socket, cmdString )
{
	var sendOnList = true;
	if( cmdString )
	{
		if( SubStringSearch( cmdString, "OFF" ) )
			sendOnList = false;
	}
	
	socket.WhoList( sendOnList );
}
