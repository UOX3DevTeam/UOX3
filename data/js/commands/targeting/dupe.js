function CommandRegistration(contextID)
{
	RegisterCommand( "dupe", 2, true,contextID );
}

function command_DUPE( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 38, socket.Language );
	if( cmdString )
		socket.tempint = StringToNum( cmdString );
	else
		socket.tempint = 1;
	socket.CustomTarget( 0, targMsg );
}

function onCallback0( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj && ourObj.isItem )
	{
		var numToDupe = socket.tempint;
		for( var i = 0; i < numToDupe; ++i )
		{
			ourObj.Dupe( socket );
		}
	}
	socket.tempint = 0;
}
