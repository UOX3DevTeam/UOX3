function CommandRegistration(contextID)
{
	RegisterCommand( "houseicons", 2, true,contextID );
}

function command_HOUSEICONS( socket, cmdString )
{
	var pUser = socket.currentChar;
	var value = (cmdString.toUpperCase() == "ON");
	var sysMessage;

	pUser.houseicons = value;
	pUser.Teleport();
	if( value )
		sysMessage = GetDictionaryEntry( 41, socket.Language );
	else
		sysMessage = GetDictionaryEntry( 42, socket.Language );
	socket.SysMessage( sysMessage );
}
