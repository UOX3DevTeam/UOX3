function CommandRegistration()
{
	RegisterCommand( "zerokills", 2, true );
}

function command_ZEROKILLS( socket, cmdString )
{
	socket.SysMessage( GetDictionaryEntry( 22, socket.language )); // Zeroing all player kills...
	var count = IterateOver( "CHARACTER" );
	socket.SysMessage( GetDictionaryEntry( 23, socket.language )); // All player kills are now 0.
}

function onIterate( toCheck )
{
	if( toCheck && toCheck.isChar )
	{
		toCheck.murdercount = 0;
		return true;
	}
	return false;
}