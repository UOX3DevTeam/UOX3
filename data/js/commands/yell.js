function CommandRegistration(contextID)
{
	RegisterCommand( "cnsyell", 1, true, contextID );
	RegisterCommand( "gmyell", 2, true, contextID );
	RegisterCommand( "adminyell", 3, true, contextID );
	RegisterCommand( "yellall", 2, true, contextID );
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

function command_YELLALL( socket, cmdString )
{
	if( cmdString )
		Yell( socket, cmdString, 0 );
}
