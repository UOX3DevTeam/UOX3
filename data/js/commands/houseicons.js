function CommandRegistration()
{
	RegisterCommand( "houseicons", 2, true );
}

function command_HOUSEICONS( socket, cmdString )
{
	var pUser = socket.currentChar;
	var value = (cmdString.toUpperCase() == "ON");
	var sysMessage;

	pUser.houseicons = value;
	pUser.Teleport();
	if( value )
		sysMessage = GetDictionaryEntry( 41, socket.language );
	else
		sysMessage = GetDictionaryEntry( 42, socket.language );
	socket.SysMessage( sysMessage );
}
