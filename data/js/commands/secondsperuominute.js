// This command displays the value of the "secondsperuominute" setting from UOX.INI

function CommandRegistration()
{
	RegisterCommand( "secondsperuominute", 2, true );
}

function command_SECONDSPERUOMINUTE( socket, cmdString )
{
	if( cmdString )
	{
		SecondsPerUOMinute( parseInt( cmdString ) );
		var sysMessage = GetDictionaryEntry( 49, socket.language ); // Seconds per UO minute set.
		socket.SysMessage( sysMessage );
	}
}
