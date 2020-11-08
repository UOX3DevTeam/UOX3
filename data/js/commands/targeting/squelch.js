function CommandRegistration()
{
	RegisterCommand( "mute", 2, true );
	RegisterCommand( "squelch", 2, true );
}

function command_MUTE( socket, cmdString )
{
	if( cmdString )
		socket.tempint = parseInt( cmdString );
	else
		socket.tempint = 0;

	var targMsg = GetDictionaryEntry( 71, socket.language );
	socket.tempint = parseInt( cmdString );
	socket.CustomTarget( 0, targMsg );
}

function command_SQUELCH( socket, cmdString )
{
	if( cmdString )
		socket.tempint = parseInt( cmdString );
	else
		socket.tempint = 0;

	var targMsg = GetDictionaryEntry( 71, socket.language );
	socket.tempint = parseInt( cmdString );
	socket.CustomTarget( 0, targMsg );
}

function onCallback0( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar && ourObj.online )
	{
		var ourSock = ourObj.socket;
		if( ourObj.commandlevel > 0 )
			socket.SysMessage( GetDictionaryEntry( 1042, socket.language ) );
		else
		{
			if( ourObj.squelch )
			{
				ourObj.squelch = 0;
				socket.SysMessage( GetDictionaryEntry( 1655 ) );
				ourSock.SysMessage( GetDictionaryEntry( 1043 ) );
				ourSock.SetTimer( 5, 0 );
			}
			else
			{
				if( socket.tempint )
				{
					ourSock.squelch = 2;
					ourSock.SetTimer( 5, socket.tempint );
				}
				else
				{
					ourObj.squelch = 1;
					ourSock.SetTimer( 5, 0 );
				}
				socket.SysMessage( GetDictionaryEntry( 761, socket.language ) );
				ourSock.SysMessage( GetDictionaryEntry( 1044, ourSock.language ) );
			}
		}
	}
	socket.tempint = 0;
}

