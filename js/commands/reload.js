function CommandRegistration()
{
	RegisterCommand( "reloadaccounts", 3, true );	// reload accounts
	RegisterCommand( "reloaddefs", 3, true )	// reload all dfns
	RegisterCommand( "reloadini", 3, true );	// reload uox.ini
	RegisterCommand( "reloadjsfile", 3, true );	// reload JavaScript file
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

function command_RELOADJSFILE( socket, cmdString )
{
	var scriptID = StringToNum( cmdString );
	socket.SysMessage( "Attempting Reload of JavaScript (ScriptID " + cmdString + ")" );
	ReloadJSFile( scriptID );
}