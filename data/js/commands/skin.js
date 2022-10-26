// This command is a shortcut for 'set color #, and updates the color of the command user's char

function CommandRegistration()
{
	RegisterCommand( "skin", 2, true );
}

function command_SKIN( socket, cmdString )
{
	if( cmdString )
	{
		var pUser = socket.currentChar;
		var value = parseInt( cmdString );
		var body = pUser.id;

		if((( value & 0x4000 ) == 0x4000 ) && ( body >= 0x0190 && body <= 0x03E1 ))
		{
			value = 0xF000;
		}

		if( value != 0x8000 )
		{
			pUser.colour = value;
			pUser.orgSkin = value;
		}
	}
}
