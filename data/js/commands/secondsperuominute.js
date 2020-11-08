function CommandRegistration()
{
	RegisterCommand( "secondsperuominute", 2, true );
}

function command_SECONDSPERUOMINUTE( socket, cmdString )
{
	if( cmdString )
	{
		SecondsPerUOMinute( parseInt( cmdString ) );
		var sysMessage = GetDictionaryEntry( 49, socket.language );
		socket.SysMessage( sysMessage );
	}
}
