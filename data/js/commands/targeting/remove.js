function CommandRegistration()
{
	RegisterCommand( "remove", 2, true );
	RegisterCommand( "deletechar", 3, true );
}

function command_REMOVE( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 188, socket.Language );
	socket.CustomTarget( 0, targMsg );
}

function onCallback0( socket, ourObj )
{
	if( !socket.GetWord( 1 ) )
	{
		if( ourObj.isChar )
		{
			if( ourObj.npc )
			{
				socket.SysMessage( GetDictionaryEntry( 1015 ) );
				ourObj.Delete();
			}
		}
		else if( ourObj.isItem )
		{
			socket.SysMessage( GetDictionaryEntry( 1013 ) );
			ourObj.Delete();
		}
	}
}

function command_DELETECHAR( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 1618, socket.Language );
	socket.CustomTarget( 1, targMsg );
}

function onCallback1( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar && ourObj != socket.currentChar )
	{
		if( !ourObj.npc && ourObj.online )
		{
			var targSock = ourObj.socket;
			if( targSock )
			{
				targSock.SysMessage( GetDictionaryEntry( 1659 ) );
				targSock.Disconnect();
			}
		}
		ourObj.Delete();
	}
}
