// These commands are alternate ways (to UOX3 console commands) for
// reloading INI, DFNs, JS files, etc.

function CommandRegistration()
{
	RegisterCommand( "reloadaccounts", 10, true );	// reload accounts
	RegisterCommand( "reloaddefs", 10, true );	// reload all dfns
	RegisterCommand( "reloadini", 10, true );	// reload uox.ini
	RegisterCommand( "reloadjsfile", 10, true );	// reload JavaScript file
	RegisterCommand( "reloadalljs", 10, true ); // reload all JavaScript files
	RegisterCommand( "reloadregions", 10, true ); // reload regions
	RegisterCommand( "reloadspawnregions", 10, true ); // reload spawnregions
	RegisterCommand( "reloadspells", 10, true ); //reload spells
	RegisterCommand( "reloadcommands", 10, true ); //reload commands
	RegisterCommand( "reloadhtml", 10, true ); //reload HTML templates
	RegisterCommand( "reloaddictionaries", 10, true ); //reload dictionaries
	RegisterCommand( "reloadall", 10, true ); //reload everything
}

function command_RELOADACCOUNTS( socket, cmdString )
{
	var dictMessage = GetDictionaryEntry( 86, socket.language );
	socket.SysMessage( dictMessage );
	Reload( 9 );
}

function command_RELOADDEFS( socket, cmdString )
{
	var dictMessage = GetDictionaryEntry( 1748, socket.language ); // Reloading new scripts...
	socket.SysMessage( dictMessage );
	Reload( 4 );
}

function command_RELOADINI( socket, cmdString )
{
	var dictMessage = GetDictionaryEntry( 45, socket.language ); // INI file reloaded.
	socket.SysMessage( dictMessage );
	Reload( 7 );
}

function command_RELOADJSFILE( socket, cmdString )
{
	var scriptID = parseInt( cmdString );
	var tempMsg = GetDictionaryEntry( 8003, socket.language ); // Attempting Reload of JavaScript (ScriptID %i)
	socket.SysMessage( tempMsg.replace( /%i/gi, cmdString ));
	ReloadJSFile( scriptID );
}

function command_RELOADALLJS( socket, cmdString )
{
	socket.SysMessage( GetDictionaryEntry( 8004, socket.language )); // Attempting Reload of all JavaScripts.
	Reload( 5 );
}

function command_RELOADREGIONS( socket, cmdString )
{
	socket.SysMessage( GetDictionaryEntry( 8005, socket.language )); // Attempting Reload of Regions.
	Reload( 0 );
}

function command_RELOADSPAWNREGIONS( socket, cmdString )
{
	socket.SysMessage( GetDictionaryEntry( 68, socket.language )); // Spawnregions reloaded.
	Reload( 1 );
}

function command_RELOADALL( socket, cmdString )
{
	BroadcastMessage( GetDictionaryEntry( 1746, socket.language )); // Possible pause... reloading server scripts
	Reload( 8 );
}

function command_RELOADSPELLS( socket, cmdString )
{
	socket.SysMessage( GetDictionaryEntry( 8006, socket.language )); // Reloading spells...
	Reload( 2 );
}

function command_RELOADCOMMANDS( socket, cmdString )
{
	socket.SysMessage( GetDictionaryEntry( 8007, socket.language )); // Reloading commands..
	Reload( 3 );
}

function command_RELOADHTML( socket, cmdString )
{
	socket.SysMessage( GetDictionaryEntry( 8008, socket.language )); // Reloading HTML Templates..
	Reload( 6 );
}

function command_RELOADDICTIONARIES( socket, cmdString )
{
	socket.SysMessage( GetDictionaryEntry( 8009, socket.language )); // Reloading Dictionaries..
	Reload( 10 );
}