function CommandRegistration()
{
	RegisterCommand( "setpoisoned", 2, true );
}

function command_SETPOISONED( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 240 );
	socket.tempint = StringToNum( cmdString );
	socket.CustomTarget( 0, targMsg );
}

function onCallback0( socket, ourObj )
{
	if( ourObj.isChar )
	{
		var poisonStrength = socket.tempint;
		ourObj.SetPoisoned( poisonStrength, 180000 );
	}
}


