function CommandRegistration()
{
	RegisterCommand( "setrace", 2, true );
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
