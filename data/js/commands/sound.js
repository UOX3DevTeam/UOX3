// These commands are used to play specific SFX and/or Music for the command user

function CommandRegistration()
{
	RegisterCommand( "sfx", 8, true );
	RegisterCommand( "music", 8, true );
	RegisterCommand( "midi", 8, true ); // Alias for music command
}

function command_SFX( socket, cmdString )
{
	if( cmdString )
	{
		var value = parseInt( cmdString );
		socket.SoundEffect( value, false );
	}
}

function command_MUSIC( socket, cmdString )
{
	if( cmdString )
	{
		var value = parseInt( cmdString );
		socket.Music( value );
	}
}

function command_MIDI( socket, cmdString )
{
	command_MUSIC( socket, cmdString )
}