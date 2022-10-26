// This command is a shortcut for changing the body ID of user

function CommandRegistration()
{
	RegisterCommand( "poly", 2, true );
}

function command_POLY( socket, cmdString )
{
	if( cmdString )
	{
		var pUser = socket.currentChar;
		var newBody = parseInt( cmdString );
		var skinVal = pUser.colour;
	
		if( newBody <= 0x7CF && newBody != 0x20 )
		{
			pUser.id = newBody;
			pUser.orgID = newBody;
			if((( skinVal & 0x4000 ) == 0x4000 ) && ( newBody >= 0x0190 && newBody <= 0x03E1 ))
			{
				if( skinVal != 0x8000 )
				{
					pUser.colour = 0xF000;
					pUser.orgSkin = 0xF000;
				}
			}
		}
	}
}