function CommandRegistration()
{
	RegisterCommand( "reload", 3, true );
	RegisterCommand( "reloadjsfile", 3, true );	// reload JavaScript file
}

function command_RELOAD( socket, cmdString )
{
	var pUser = socket.currentChar;
	if ( cmdString && pUser )
	{
		var splitString = cmdString.split(" ");
		switch ( splitString[0] )
		{
			case "regions":
				socket.SysMessage( GetDictionaryEntry( 8005, socket.language ) );
				Reload( 0 ); break;
			case "spawnregions":
				socket.SysMessage( GetDictionaryEntry( 68, socket.language ) );
				Reload( 1 ); break;
			case "spells":
				socket.SysMessage( GetDictionaryEntry( 8006, socket.language ) );
				Reload( 2 ); break;
			case "commands":
				socket.SysMessage( GetDictionaryEntry( 8007, socket.language ) );
				Reload( 3 ); break;
			case "defs":
				socket.SysMessage( GetDictionaryEntry( 1748, socket.language ) );
				Reload( 4 ); break;
			case "alljs":
				socket.SysMessage( GetDictionaryEntry(8004, socket.language ) );
				Reload( 5 ); break;
			case "html":
				socket.SysMessage(GetDictionaryEntry( 1748, socket.language ) );
				Reload( 6 ); break;
			case "ini":
				socket.SysMessage( GetDictionaryEntry(45, socket.language ) );
				Reload( 7 ); break;
			case "all":
				socket.SysMessage(GetDictionaryEntry( 8008, socket.language ) );
				Reload( 8 ); break;
			case "accounts":
				socket.SysMessage( GetDictionaryEntry( 86, socket.language ) );
				Reload( 9 ); break;
			case "Dictionaries":
				socket.SysMessage( GetDictionaryEntry( 8009, socket.language ) );
				Reload( 10 ); break;
		}
	}
}

function command_RELOADJSFILE( socket, cmdString )
{
	var scriptID = parseInt( cmdString );
	var tempMsg = GetDictionaryEntry( 8003, socket.language ); // Attempting Reload of JavaScript (ScriptID %i)
	socket.SysMessage( tempMsg.replace(/%i/gi, cmdString ) );
	ReloadJSFile( scriptID );
}