function CommandRegistration(contextID)
{
	RegisterCommand( "secondsperuominute", 2, true ,contextID);
}

function command_SECONDSPERUOMINUTE( socket, cmdString )
{
	if( cmdString )
	{
		SecondsPerUOMinute( StringToNum( cmdString ) );
		var sysMessage = GetDictionaryEntry( 49, socket.Language );
		socket.SysMessage( sysMessage );
	}
}
