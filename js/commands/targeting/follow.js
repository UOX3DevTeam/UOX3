function CommandRegistration()
{
	RegisterCommand( "follow", 1, true );
}

function command_FOLLOW( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 228 );
	socket.CustomTarget( 0, targMsg );
}

function onCallback0( socket, ourObj )
{
	if( ourObj.isChar )
	{
		var targMsg = GetDictionaryEntry( 1742 );
		socket.tempObj = ourObj;
		socket.CustomTarget( 1, targMsg );
	}
}

function onCallback1( socket, ourObj )
{
	if( ourObj.isChar )
	{
		var toFollow = socket.tempObj;
		if( ourObj.npc && toFollow )
			ourObj.Follow( toFollow );
	}
	socket.tempObj = null;
}
