// This command can be used to toggle the the GM moderation mode for bulletin boards

function CommandRegistration()
{
	RegisterCommand( "msgmod", 2, true );
}

function command_MSGMOD( socket, cmdString )
{
	var pUser = socket.currentChar;
	var value = ( cmdString.toUpperCase() == "ON" );
	var sysMessage;

	pUser.SetTag( "modboards", value );
	if( value )
	{
		sysMessage = GetDictionaryEntry( 2021, socket.language ); // MSGMOD enabled - bulletin boards can now be moderated.
	}
	else
	{
		sysMessage = GetDictionaryEntry( 2022, socket.language ); // MSGMOD disabled - bulletin boards can no longer be moderated.
	}
	socket.SysMessage( sysMessage );
}