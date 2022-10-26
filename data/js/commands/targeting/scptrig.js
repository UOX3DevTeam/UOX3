// Commands dealing with getting/setting/adding/removing JS scripts to/from objects

function CommandRegistration()
{
	RegisterCommand( "getscptrig", 2, true );
	RegisterCommand( "setscptrig", 2, true );
	RegisterCommand( "addscptrig", 2, true );
	RegisterCommand( "removescptrig", 2, true );
}

function command_GETSCPTRIG( socket, cmdString )
{
	socket.CustomTarget( 0, GetDictionaryEntry( 2041, socket.language )); // Select target to get script triggers for:
}

function onCallback0( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ( ourObj.isChar || ourObj.isItem ))
	{
		// Get list of script triggers from object
		var scriptTriggers = ourObj.scriptTriggers;
		var size = scriptTriggers.length;
		if( size > 0 )
		{
			for( var i = 0; i < size; i++ )
			{
				socket.SysMessage( scriptTriggers[i] );
			}
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 1752, socket.language )); // No such script is known to the server
		}
	}
}

function command_SETSCPTRIG( socket, cmdString )
{
	if( cmdString )
	{
		if( parseInt( cmdString ) == 0 )
		{
			socket.tempint = 0;
			socket.CustomTarget( 3, GetDictionaryEntry( 2043, socket.language )); // Select target to remove ALL script triggers from:
			return;
		}

		var tempDictString = GetDictionaryEntry( 2038, socket.language ); // Select target to set script trigger (%i) for:
		tempDictString = ( tempDictString.replace( /%i/gi, cmdString ));
		socket.tempint = parseInt( cmdString );
		socket.CustomTarget( 1, tempDictString );
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 1755, socket.language )); // This command requires additional arguments
	}
}

function onCallback1( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ( ourObj.isChar || ourObj.isItem ))
	{
		var scriptID = socket.tempint;
		if( scriptID == 0 )
		{
			// 0 removes all script triggers
			socket.SysMessage( GetDictionaryEntry( 2044, socket.language )); // All script triggers have been cleared from target!
			ourObj.RemoveScriptTrigger( 0 );
		}
		else
		{
			ourObj.scripttrigger = scriptID;
			var dictID = ( ourObj.isChar ? 1653 : 1652 );
			socket.SysMessage( GetDictionaryEntry( dictID, socket.language )); // Script assigned to item/char
		}
	}
	socket.tempint = 0;
}

// Add a new script trigger to an object's list of triggers
function command_ADDSCPTRIG( socket, cmdString )
{
	if( cmdString )
	{
		if( parseInt( cmdString ) == 0 )
		{
			return;
		}

		var tempDictString = GetDictionaryEntry( 2039, socket.language ); // Select target to add script trigger (%i) for:
		tempDictString = ( tempDictString.replace( /%i/gi, cmdString ));
		socket.tempint = parseInt( cmdString );
		socket.CustomTarget( 2, tempDictString );
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 1755, socket.language ));
	}
}

function onCallback2( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ( ourObj.isChar || ourObj.isItem ))
	{
		var scriptID = socket.tempint;
		ourObj.AddScriptTrigger( scriptID );
		var dictID = ( ourObj.isChar ? 1653 : 1652 );
		socket.SysMessage( GetDictionaryEntry( dictID, socket.language )); // Script assigned to item/char
	}
	socket.tempint = 0;
}

// Remove specified script trigger from object
function command_REMOVESCPTRIG( socket, cmdString )
{
	if( cmdString )
	{
		if( parseInt( cmdString ) == 0 )
		{
			socket.tempint = 0;
			socket.CustomTarget( 3, GetDictionaryEntry( 2043, socket.language )); // Select target to remove ALL script triggers from:
			return;
		}

		var tempDictString = GetDictionaryEntry( 2040, socket.language ); // Select target to remove script trigger (%i) from:
		tempDictString = ( tempDictString.replace(/%i/gi, cmdString ));
		socket.tempint = parseInt( cmdString );
		socket.CustomTarget( 3, tempDictString );
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 1755, socket.language ));
	}
}

function onCallback3( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ( ourObj.isChar || ourObj.isItem ))
	{
		var scriptID = socket.tempint;
		if( scriptID == 0 )
		{
			// 0 removes all script triggers
			socket.SysMessage( GetDictionaryEntry( 2044, socket.language )); // All script triggers have been cleared from target!
		}
		else
		{
			okMsg( socket );
		}

		ourObj.RemoveScriptTrigger( scriptID );
	}
	socket.tempint = 0;
}

function okMsg( socket )
{
	//Sends verification to the player that the specified value was successfully set.
	socket.SysMessage( GetDictionaryEntry( 84, socket.language ));
}