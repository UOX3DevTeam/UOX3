function CommandRegistration(contextID)
{
	RegisterCommand( "xgate", 1, true, contextID );
}

function command_XGATE( socket, cmdString )
{
	var mChar = socket.currentChar;
	if( mChar && cmdString )
		mChar.Gate( StringToNum( cmdString ) );
}



