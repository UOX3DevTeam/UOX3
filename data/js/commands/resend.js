function CommandRegistration()
{
	RegisterCommand( "resend", 1, true );
}

function command_RESEND( socket, cmdString )
{
	var pUser = socket.currentChar;
	pUser.Teleport();
}
