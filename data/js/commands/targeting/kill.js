function CommandRegistration(contextID)
{
	RegisterCommand( "kill", 2, true,contextID );
	RegisterCommand( "resurrect", 2, true,contextID );
}

function command_KILL( socket, cmdString )
{
	if( cmdString )
	{
		socket.xText = cmdString;
		socket.CustomTarget( 0, "Select target to kill: " + cmdString );
	}
}

function onCallback0( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar )
	{
		var splitString = socket.xText.split( " " );
		var uKey 	= splitString[0].toUpperCase();
		var layer 	= 0;
		switch( uKey )
		{
		case "HAIR":	layer = 0x0B;			break;
		case "BEARD":	layer = 0x10;			break;
		case "PACK":	layer = 0x15;			break;
		case "LAYER":
				if( splitString[1] )
					layer = StringToNum( splitString[1] ); 
				break;
		case "CHAR":
				if( !ourObj.dead )
				{
					ourObj.BoltEffect();
					ourObj.SoundEffect( 0x0029, true );
					ourObj.Kill();
				}
				else
					socket.SysMessage( "That character is already dead!" );
				break;
		default:					break;
		}
		if( layer > 0 )
		{
			var ourLayer = ourObj.FindItemLayer( layer );
			if( ourLayer )
				ourLayer.Delete();
		}
	}
}

function command_RESURRECT( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 194, socket.Language );
	socket.CustomTarget( 1, targMsg );
}

function onCallback1( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar )
		ourObj.Resurrect();
}
