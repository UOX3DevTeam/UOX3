function CommandRegistration(contextID)
{
	RegisterCommand( "jail", 1, true,contextID );
	RegisterCommand( "release", 1, true,contextID );
}

function command_JAIL( socket, cmdString )
{
	socket.tempint = 100000;
	if( cmdString )
		socket.tempint = StringToNum( cmdString );

	var targMsg = GetDictionaryEntry( 180, socket.Language );
	socket.CustomTarget( 0, targMsg );
}

function onCallback0( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar )
	{
		if( ourObj.isJailed )
			socket.SysMessage( GetDictionaryEntry( 1070, socket.Language ) );
		else
			ourObj.Jail( socket.tempint );
	}
	socket.tempint = 0;
}

function command_RELEASE( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 181, socket.Language );
	socket.CustomTarget( 1, targMsg );
}

function onCallback1( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar )
	{
		if( ourObj.isJailed )
			socket.SysMessage( GetDictionaryEntry( 1064, socket.Language ) );
		else
		{
			ourObj.Release();
			SysMessage( "Released " + ourObj.name + " from jail" );
		}
	}
}

