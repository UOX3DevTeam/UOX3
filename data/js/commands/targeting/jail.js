function CommandRegistration()
{
	RegisterCommand( "jail", 1, true );
	RegisterCommand( "release", 1, true );
}

function command_JAIL( socket, cmdString )
{
	socket.tempint = 86400;
	if( cmdString )
		socket.tempint = parseInt( cmdString );

	var targMsg = GetDictionaryEntry( 180, socket.language );
	socket.CustomTarget( 0, targMsg );
}

function onCallback0( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar )
	{
		if( ourObj.isJailed )
			socket.SysMessage( GetDictionaryEntry( 1070, socket.language ) );
		else
		{
			ourObj.Jail( socket.tempint );
			socket.SysMessage( ourObj.name + " has been jailed for " + (socket.tempint / 60 / 60) + " hours." );
		}
	}
	socket.tempint = 0;
}

function command_RELEASE( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 181, socket.language );
	socket.CustomTarget( 1, targMsg );
}

function onCallback1( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar )
	{
		if( ourObj.isJailed == false )
			socket.SysMessage( GetDictionaryEntry( 1064, socket.language ) );
		else
		{
			ourObj.Release();
			socket.SysMessage( "Released " + ourObj.name + " from jail." );
		}
	}
}

