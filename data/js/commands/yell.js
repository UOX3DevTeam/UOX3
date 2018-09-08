function CommandRegistration()
{
	RegisterCommand( "cnsyell", 1, true );
	RegisterCommand( "gmyell", 2, true );
	RegisterCommand( "adminyell", 3, true );
	RegisterCommand( "yell", 2, true );
}

function command_CNSYELL( socket, cmdString )
{
	if( cmdString )
		Yell( socket, cmdString, 1 );
}

function command_GMYELL( socket, cmdString )
{
	if( cmdString )
		Yell( socket, cmdString, 2 );
}

function command_ADMINYELL( socket, cmdString )
{
	if( cmdString )
		Yell( socket, cmdString, 3 );
}

function command_YELL( socket, cmdString )
{
	if( cmdString )
		Yell( socket, cmdString, 0 );
}
