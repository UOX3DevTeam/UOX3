function CommandRegistration(contextID)
{
	RegisterCommand( "action", 2, true,contextID );
	RegisterCommand( "bolt", 2, true,contextID );
}

function command_ACTION( socket, cmdString )
{
	if( cmdString )
	{
		var targMsg = GetDictionaryEntry( 213, socket.Language );
		socket.tempint = StringToNum( cmdString );
		socket.CustomTarget( 0, targMsg );
	}
}

function onCallback0( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar )
		ourObj.DoAction( socket.tempint );
	socket.tempint = 0;
}

function command_BOLT( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 195, socket.Language );
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
