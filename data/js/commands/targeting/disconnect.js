function CommandRegistration()
{
	RegisterCommand( "kick", 2, true );
	RegisterCommand( "disconnect", 2, true );
}

function command_KICK( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 196, socket.language );
	socket.CustomTarget( 0, targMsg );
}

function command_DISCONNECT( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 196, socket.language );
	socket.CustomTarget( 0, targMsg );
}

function onCallback0( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar && ourObj.online )
	{
		var targSock = ourObj.socket;
		if( targSock && targSock != socket )
		{
			socket.SysMessage( GetDictionaryEntry( 1029, socket.language ) );
			targSock.SysMessage( GetDictionaryEntry( 1030, targSock.language ) );
			targSock.Disconnect();
		}
	}
}
