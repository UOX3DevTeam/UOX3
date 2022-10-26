// This command is a shortcut for opening up itemmenu 6000, which has teleport locations

function CommandRegistration()
{
	RegisterCommand( "travel", 2, true );
}

function command_TRAVEL( socket, cmdString )
{
	var pUser = socket.currentChar;
	pUser.ExecuteCommand( "itemmenu 6000" );
}