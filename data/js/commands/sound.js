function CommandRegistration(contextID)
{
	RegisterCommand( "sfx", 2, true,contextID );
	RegisterCommand( "midi", 2, true,contextID );
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
