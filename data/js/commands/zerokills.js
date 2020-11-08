function CommandRegistration()
{
	RegisterCommand( "zerokills", 2, true );
}

function command_ZEROKILLS( socket, cmdString )
{
	socket.SysMessage( GetDictionaryEntry( 22, socket.language ) );
	var count = IterateOver( "CHARACTER" );
	socket.SysMessage( GetDictionaryEntry( 23, socket.language ) );
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