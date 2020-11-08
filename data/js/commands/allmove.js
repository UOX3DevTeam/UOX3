function CommandRegistration()
{
	RegisterCommand( "allmove", 2, true );
}

function command_ALLMOVE( socket, cmdString )
{
	var pUser = socket.currentChar;
	var value = (cmdString.toUpperCase() == "ON");
	var sysMessage;

	pUser.allmove = value;
	pUser.Teleport();
	if( value )
		sysMessage = GetDictionaryEntry( 39, socket.language );
	else
		sysMessage = GetDictionaryEntry( 40, socket.language );
	socket.SysMessage( sysMessage );
}
