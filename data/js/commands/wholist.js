function CommandRegistration(contextID)
{
	RegisterCommand( "wholist", 1, true,contextID );
}

function command_WHOLIST( socket, cmdString )
{
	socket.OpenGump( 8 );
	var sendOnList = true;
	if( cmdString )
	{
		if( cmdString.toUpperCase() == "OFF" )
			sendOnList = false;
	}
	socket.WhoList( sendOnList );
}