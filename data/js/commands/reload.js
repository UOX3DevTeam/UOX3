function CommandRegistration()
{
	RegisterCommand( "reloadaccounts", 3, true );	// reload accounts
	RegisterCommand( "reloaddefs", 3, true );	// reload all dfns
	RegisterCommand( "reloadini", 3, true );	// reload uox.ini
	RegisterCommand( "reloadjsfile", 3, true );	// reload JavaScript file
	RegisterCommand( "reloadalljs", 3, true ); // reload all JavaScript files
	RegisterCommand( "reloadregions", 3, true ); // reload regions
	RegisterCommand( "reloadspawnregions", 3, true ); // reload spawnregions
	RegisterCommand( "reloadspells", 3, true ); //reload spells
	RegisterCommand( "reloadcommands", 3, true ); //reload commands
	RegisterCommand( "reloadhtml", 3, true ); //reload HTML templates
	RegisterCommand( "reloadall", 3, true ); //reload everything
}

function command_RELOADACCOUNTS( socket, cmdString )
{
	var dictMessage = GetDictionaryEntry( 86, socket.language );
	socket.SysMessage( dictMessage );
	Reload( 9 );
}

function command_RELOADDEFS( socket, cmdString )
{
	var dictMessage = GetDictionaryEntry( 1748, socket.language );
	socket.SysMessage( dictMessage );
	Reload( 4 );
}

function command_RELOADINI( socket, cmdString )
{
	var dictMessage = GetDictionaryEntry( 45, socket.language );
	socket.SysMessage( dictMessage );
	Reload( 7 );
}

function command_RELOADJSFILE( socket, cmdString )
{
	var scriptID = parseInt( cmdString );
	socket.SysMessage( "Attempting Reload of JavaScript (ScriptID " + cmdString + ")" );
	ReloadJSFile( scriptID );
}

function command_RELOADALLJS( socket, cmdString )
{
	socket.SysMessage( "Attempting Reload of all JavaScripts." );
	Reload( 5 );
}

function command_RELOADREGIONS( socket, cmdString )
{
	socket.SysMessage( "Attempting Reload of Regions." );
	Reload( 0 );
}

function command_RELOADSPAWNREGIONS( socket, cmdString )
{
	var dictMessage = GetDictionaryEntry( 68, socket.language );
	socket.SysMessage( dictMessage );
	Reload( 1 );
}

function command_RELOADALL( socket, cmdString )
{
	var dictMessage = GetDictionaryEntry( 1746, socket.language );
	BroadcastMessage( dictMessage );
	Reload( 8 );
}

function command_RELOADSPELLS( socket, cmdString )
{
	socket.SysMessage( "Reloading spells.." );
	Reload( 2 );
}

function command_RELOADCOMMANDS( socket, cmdString )
{
	socket.SysMessage( "Reloading commands.." );
	Reload( 3 );
}

function command_RELOADHTML( socket, cmdString )
{
	socket.SysMessage( "Reloading HTML Templates.." );
	Reload( 6 );
}