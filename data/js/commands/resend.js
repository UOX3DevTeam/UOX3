function CommandRegistration(contextID)
{
	RegisterCommand( "resend", 1, true, contextID );
}

function command_RESEND( socket, cmdString )
{
	var pUser = socket.currentChar;
	pUser.Teleport();
}
