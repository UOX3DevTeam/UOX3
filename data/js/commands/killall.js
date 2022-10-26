// This command can be used to kill all characters (or a percentage of all characters)

function CommandRegistration()
{
	RegisterCommand( "killall", 2, true );
}

function command_KILLALL( socket, cmdString )
{
	percentToKill = 100;
	if( cmdString )
	{
		percentToKill = parseInt( cmdString );
	}

	var count = IterateOver( "CHARACTER" );
	var tempMsg = GetDictionaryEntry( 8010, socket.language ); // Killed %i characters
	socket.SysMessage( tempMsg.replace( /%i/gi, count.toString() ));
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
