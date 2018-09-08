function CommandRegistration()
{
	RegisterCommand( "killall", 2, true );
}

function command_KILLALL( socket, cmdString )
{
	percentToKill = 100;
	if( cmdString )
		percentToKill = parseInt( cmdString );

	var count = IterateOver( "CHARACTER" );
	socket.SysMessage( "Killed " + count.toString() + " characters" );
}

function onIterate( toCheck )
{
	if( toCheck && toCheck.isChar )
	{
		if( RandomNumber( 0, 99 ) + 1 <= percentToKill )
		{
			toCheck.BoltEffect();
			toCheck.SoundEffect( 0x0029, true );
			toCheck.Kill();
			return true;
		}
	}
	return false;
}
