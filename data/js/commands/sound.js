function CommandRegistration()
{
	RegisterCommand( "sfx", 2, true );
	RegisterCommand( "midi", 2, true );
}

function command_SFX( socket, cmdString )
{
	if( cmdString )
	{
		var value = StringToNum( cmdString );
		socket.SoundEffect( value, false );
	}
}

function command_MIDI( socket, cmdString )
{
	if( cmdString )
	{
		var value = StringToNum( cmdString );
		socket.Midi( value );
	}
}
