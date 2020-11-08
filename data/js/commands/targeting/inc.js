function CommandRegistration()
{
	RegisterCommand( "incx", 2, true );
	RegisterCommand( "incy", 2, true );
	RegisterCommand( "incz", 2, true );
}

function command_INCX( socket, cmdString )
{
	if( cmdString )
		socket.tempint = parseInt( cmdString );
	else
		socket.tempint = 1;
	var targMsg = GetDictionaryEntry( 254, socket.language );
	socket.CustomTarget( 0, targMsg );
}

function onCallback0( socket, ourObj )
{
	if( !socket.GetWord( 1 ) )
		ourObj.x = (ourObj.x + socket.tempint);
	socket.tempint = 0;
}

function command_INCY( socket, cmdString )
{
	if( cmdString )
		socket.tempint = parseInt( cmdString );
	else
		socket.tempint = 1;
	var targMsg = GetDictionaryEntry( 254, socket.language );
	socket.CustomTarget( 1, targMsg );
}

function onCallback1( socket, ourObj )
{
	if( !socket.GetWord( 1 ) )
		ourObj.y = (ourObj.y + socket.tempint);
	socket.tempint = 0;
}

function command_INCZ( socket, cmdString )
{
	if( cmdString )
		socket.tempint = parseInt( cmdString );
	else
		socket.tempint = 1;
	var targMsg = GetDictionaryEntry( 268, socket.language );
	socket.CustomTarget( 2, targMsg );
}

function onCallback2( socket, ourObj )
{
	if( !socket.GetWord( 1 ) )
		ourObj.z = (ourObj.z + socket.tempint);
	socket.tempint = 0;
}

