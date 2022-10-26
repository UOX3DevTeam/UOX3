// These commands lets GM quickly increase or decrease an object's x, y or z coordinate, or ID

function CommandRegistration()
{
	RegisterCommand( "incx", 2, true );
	RegisterCommand( "incy", 2, true );
	RegisterCommand( "incz", 2, true );
	RegisterCommand( "incid", 2, true );
}

function command_INCX( socket, cmdString )
{
	if( cmdString )
	{
		socket.tempint = parseInt( cmdString );
	}
	else
	{
		socket.tempint = 1;
	}
	var targMsg = GetDictionaryEntry( 254, socket.language ); // Select item to reposition.
	socket.CustomTarget( 0, targMsg );
}

function onCallback0( socket, ourObj )
{
	if( !socket.GetWord( 1 ))
	{
		ourObj.x = ( ourObj.x + socket.tempint );
	}
	socket.tempint = 0;
}

function command_INCY( socket, cmdString )
{
	if( cmdString )
	{
		socket.tempint = parseInt( cmdString );
	}
	else
	{
		socket.tempint = 1;
	}
	var targMsg = GetDictionaryEntry( 254, socket.language ); // Select item to reposition.
	socket.CustomTarget( 1, targMsg );
}

function onCallback1( socket, ourObj )
{
	if( !socket.GetWord( 1 ))
	{
		ourObj.y = ( ourObj.y + socket.tempint );
	}
	socket.tempint = 0;
}

function command_INCZ( socket, cmdString )
{
	if( cmdString )
		socket.tempint = parseInt( cmdString );
	else
		socket.tempint = 1;
	var targMsg = GetDictionaryEntry( 268, socket.language ); // Select item to reposition.
	socket.CustomTarget( 2, targMsg );
}

function onCallback2( socket, ourObj )
{
	if( !socket.GetWord( 1 ) )
	{
		ourObj.z = ( ourObj.z + socket.tempint );
	}
	socket.tempint = 0;
}

function command_INCID( socket, cmdString )
{
	if( cmdString )
	{
		socket.tempint = parseInt( cmdString );
	}
	else
	{
		socket.tempint = 1;
	}
	var targMsg = GetDictionaryEntry( 9101, socket.language ); // Select target to modify ID by %i:
	socket.CustomTarget( 3, targMsg.replace( /%i/gi, socket.tempint ));
}

function onCallback3( socket, ourObj )
{
	if( !socket.GetWord( 1 ))
	{
		ourObj.id = ( ourObj.id + socket.tempint );
	}
	socket.tempint = 0;
}
