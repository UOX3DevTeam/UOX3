function CommandRegistration()
{
	RegisterCommand( "msgmod", 2, true );
}

function command_MSGMOD( socket, cmdString )
{
	var pUser = socket.currentChar;
	var value = (cmdString.toUpperCase() == "ON");
	var sysMessage;

	pUser.SetTag( "modboards", value );
	if( value )
		sysMessage = GetDictionaryEntry( 2021, socket.language );
	else
		sysMessage = GetDictionaryEntry( 2022, socket.language );
	socket.SysMessage( sysMessage );
}