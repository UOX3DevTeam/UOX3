function CommandRegistration(contextID)
{
	RegisterCommand( "allmove", 2, true,contextID );
}

function command_ALLMOVE( socket, cmdString )
{
	var pUser = socket.currentChar;
	var value = (cmdString.toUpperCase() == "ON");
	var sysMessage;

	pUser.allmove = value;
	pUser.Teleport();
	if( value )
		sysMessage = GetDictionaryEntry( 39, socket.Language );
	else
		sysMessage = GetDictionaryEntry( 40, socket.Language );
	socket.SysMessage( sysMessage );
}
