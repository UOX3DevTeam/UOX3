function CommandRegistration(contextID)
{
	RegisterCommand( "poly", 2, true,contextID );
}

function command_POLY( socket, cmdString )
{
	if( cmdString )
	{
		var pUser 	= socket.currentChar;
		var newBody 	= StringToNum( cmdString );
		var skinVal 	= pUser.colour;
	
		if( newBody <= 0x7CF )
		{
			pUser.id = newBody;
			if( ( skinVal&0x4000 == 0x4000 ) && ( newBody >= 0x0190 && newBody <= 0x03E1 ) )
			{
				if( skinVal != 0x8000 )
					pUser.colour = 0xF000;
			}
		}
	}
}