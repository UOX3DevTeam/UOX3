function CommandRegistration()
{
	RegisterCommand( "incx", 2, true );
	RegisterCommand( "incy", 2, true );
	RegisterCommand( "incz", 2, true );
}

function command_INCX( socket, cmdString )
{
	if( cmdString )
		socket.tempint = StringToNum( cmdString );
	else
		socket.tempint = 1;
	var targMsg = GetDictionaryEntry( 254, socket.Language );
	socket.CustomTarget( 0, targMsg );
}

function onCallback0( socket, ourObj )
{
	ourObj.x = (ourObj.x + socket.tempint);
}

function command_INCY( socket, cmdString )
{
	if( cmdString )
		socket.tempint = StringToNum( cmdString );
	else
		socket.tempint = 1;
	var targMsg = GetDictionaryEntry( 254, socket.Language );
	socket.CustomTarget( 1, targMsg );
}

function onCallback1( socket, ourObj )
{
	ourObj.y = (ourObj.y + socket.tempint);
}

function command_INCZ( socket, cmdString )
{
	if( cmdString )
		socket.tempint = StringToNum( cmdString );
	else
		socket.tempint = 1;
	var targMsg = GetDictionaryEntry( 268, socket.Language );
	socket.CustomTarget( 2, targMsg );
}

function onCallback2( socket, ourObj )
{
	ourObj.z = (ourObj.z + socket.tempint);
}

