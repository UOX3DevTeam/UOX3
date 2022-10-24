function CommandRegistration()
{
	RegisterCommand( "cnsyell", 1, true );
	RegisterCommand( "gmyell", 2, true );
	RegisterCommand( "adminyell", 3, true );
	RegisterCommand( "yell", 2, true );
}

// Yell so all Counselors can hear it
function command_CNSYELL( socket, cmdString )
{
	if( cmdString )
	{
		Yell( socket, cmdString, 1 );
	}
}

// Yell so all GMs can hear it
function command_GMYELL( socket, cmdString )
{
	if( cmdString )
	{
		Yell( socket, cmdString, 2 );
	}
}

// Yell so all Admins can hear it
function command_ADMINYELL( socket, cmdString )
{
	if( cmdString )
	{
		Yell( socket, cmdString, 3 );
	}
}

// Yell so _everyone_ can hear it
function command_YELL( socket, cmdString )
{
	if( cmdString )
	{
		Yell( socket, cmdString, 0 );
	}
}
