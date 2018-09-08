function CommandRegistration()
{
	RegisterCommand( "sfx", 2, true );
	RegisterCommand( "midi", 2, true );
}

function command_SFX( socket, cmdString )
{
	if( cmdString )
	{
		var value = parseInt( cmdString );
		socket.SoundEffect( value, false );
	}
}

function command_MIDI( socket, cmdString )
{
	if( cmdString )
	{
		var value = parseInt( cmdString );
		socket.Midi( value );
	}
}
