function CommandRegistration()
{
	RegisterCommand( "setrace", 2, true );
}

function command_SETRACE( socket, cmdString )
{
	if( cmdString )
	{
		var pUser = socket.currentChar;
		var race = parseInt( cmdString );
		pUser.raceID = race;
	}
}
