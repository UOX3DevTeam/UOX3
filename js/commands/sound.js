function CommandRegistration()
{
	RegisterCommand( "sfx", 2, true );
	RegisterCommand( "midi", 2, true );
}

function command_SFX( socket, cmdString )
{
	var value = StringToNum( cmdString );
	socket.SoundEffect( value, false );
}

function command_MIDI( socket, cmdString )
{
	var value = StringToNum( cmdString );
	socket.Midi( value );
}
