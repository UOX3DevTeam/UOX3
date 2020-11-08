function CommandRegistration()
{
	RegisterCommand( "action", 2, true );
	RegisterCommand( "bolt", 2, true );
	RegisterCommand( "npcaction", 2, true );
}

function command_ACTION( socket, cmdString )
{
	if( cmdString )
			socket.currentChar.DoAction( parseInt( cmdString ));
}


function command_BOLT( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 195, socket.language );
	socket.CustomTarget( 1, targMsg );
}

function onCallback1( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar )
	{
		ourObj.BoltEffect();
		ourObj.SoundEffect( 0x0029, true );
	}
}

function command_NPCACTION( socket, cmdString )
{
	if( cmdString )
	{
		var targMsg = GetDictionaryEntry( 213, socket.language );
		socket.tempint = parseInt( cmdString );
		socket.CustomTarget( 0, targMsg );
	}
}

function onCallback0( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar )
		ourObj.DoAction( socket.tempint );
	socket.tempint = 0;
}