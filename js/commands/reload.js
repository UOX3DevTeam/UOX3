function CommandRegistration()
{
	RegisterCommand( "reloadaccounts", 3, true );	// reload accounts
	RegisterCommand( "reloaddefs", 3, true )	// reload all dfns
	RegisterCommand( "reloadini", 3, true );	// reload uox.ini
}

function command_RELOADACCOUNTS( socket, cmdString )
{
	var dictMessage = GetDictionaryEntry( 86, socket.Language );
	socket.SysMessage( dictMessage );
	Reload( 9 );
}

function command_RELOADDEFS( socket, cmdString )
{
	var dictMessage = GetDictionaryEntry( 1748, socket.Language );
	socket.SysMessage( dictMessage );
	Reload( 4 );
}

function command_RELOADINI( socket, cmdString )
{
	var dictMessage = GetDictionaryEntry( 45, socket.Language );
	socket.SysMessage( dictMessage );
	Reload( 7 );
}