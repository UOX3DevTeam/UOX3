function CommandRegistration(contextID)
{
	RegisterCommand( "setrace", 2, true,contextID );
}

function command_SETRACE( socket, cmdString )
{
	if( cmdString )
	{
		var pUser = socket.currentChar;
		var race = StringToNum( cmdString );
		RaceGate( socket, pUser, race );
	}
}
