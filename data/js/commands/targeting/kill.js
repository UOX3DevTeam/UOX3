// These commands handle killing of characters, or their hairs/beards, or resurrecting dead chars

function CommandRegistration()
{
	RegisterCommand( "kill", 2, true );
	RegisterCommand( "resurrect", 2, true );
	RegisterCommand( "res", 2, true );
	RegisterCommand( "shavehair", 2, true );
	RegisterCommand( "shavebeard", 2, true );
}

function command_KILL( socket, cmdString )
{
	if( cmdString )
	{
		socket.xText = cmdString;
		socket.CustomTarget( 0, GetDictionaryEntry( 8089, socket.language ) + " " + cmdString ); // Select target to kill:
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 8090, socket.language )); // This command takes at least 1 argument.
	}
}

// Alias of 'kill hair
function command_SHAVEHAIR( socket, cmdString )
{
	command_KILL( socket, cmdString + "hair" );
}

// Alias of 'kill beard
function command_SHAVEBEARD( socket, cmdString )
{
	command_KILL( socket, cmdString + "beard" );
}

function onCallback0( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar )
	{
		var splitString = socket.xText.split( " " );
		var uKey = splitString[0].toUpperCase();
		var layer = 0;
		switch( uKey )
		{
		case "HAIR":	layer = 0x0B;			break;
		case "BEARD":	layer = 0x10;			break;
		case "PACK":	layer = 0x15;			break;
		case "LAYER":
				if( splitString[1] )
				{
					layer = parseInt( splitString[1] );
				}
				break;
		case "CHAR":
				if( !ourObj.dead )
				{
					ourObj.BoltEffect();
					ourObj.SoundEffect( 0x0029, true );
					ourObj.Kill();
				}
				else
				{
					socket.SysMessage( GetDictionaryEntry( 8091, socket.language )); // That character is already dead!
				}
				break;
		default:
				socket.SysMessage( GetDictionaryEntry( 8092, socket.language )); // Invalid argument passed for KILL command
				break;
		}
		if( layer > 0 )
		{
			var ourLayer = ourObj.FindItemLayer( layer );
			if( ourLayer )
			{
				ourLayer.Delete();
			}
		}
	}
}

function command_RESURRECT( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 194, socket.language ); // Select character to resurrect.
	socket.CustomTarget( 1, targMsg );
}

function command_RES( socket, cmdString )
{
	command_RESURRECT( socket, cmdString );
}

function onCallback1( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar )
	{
		ourObj.Resurrect();
	}
}
