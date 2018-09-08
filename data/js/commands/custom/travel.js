function CommandRegistration()
{
	RegisterCommand( "travel", 2, true );
}

function command_TRAVEL( socket, cmdString )
{
	var pUser = socket.currentChar;
	pUser.ExecuteCommand( "itemmenu 6000" );
}