function CommandRegistration()
{
	RegisterCommand( "openbank", 2, true );
	RegisterCommand( "openlayer", 2, true );
}

function command_OPENBANK( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 186, socket.Language );
	socket.CustomTarget( 0, targMsg );
}

function onCallback0( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar )
		ourObj.OpenBank( socket );
}

function command_OPENLAYER( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 53, socket.Language );
	if( !cmdString )
		socket.tempint = 0x15;
	else
		socket.tempint = parseInt( cmdString );
	socket.CustomTarget( 1, targMsg );
}

function onCallback1( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar )
	{
		ourObj.OpenLayer( socket, socket.tempint );
	}
}
