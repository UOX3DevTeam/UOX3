function CommandRegistration()
{
	RegisterCommand( "xgate", 1, true );
}

function command_XGATE( socket, cmdString )
{
	var mChar = socket.currentChar;
	if( mChar && cmdString )
		mChar.Gate( StringToNum( cmdString ) );
}



