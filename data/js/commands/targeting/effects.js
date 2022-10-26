function CommandRegistration()
{
	RegisterCommand( "action", 2, true ); // Play an action on GM's character
	RegisterCommand( "bolt", 2, true ); // Play a bolt effect on target character
	RegisterCommand( "npcaction", 2, true ); // Play an action on targeted NPC (or player) character
}

function command_ACTION( socket, cmdString )
{
	if( cmdString )
	{
		var splitString = cmdString.split( " " );
		if( splitString[3] )
		{
			socket.currentChar.DoAction( parseInt( splitString[0] ), null, parseInt( splitString[2] ), ( parseInt( splitString[3] ) == 1 ? true : false ));
		}
		else if( splitString[2] )
		{
			socket.currentChar.DoAction( parseInt( splitString[0] ), null, parseInt( splitString[2] ));
		}
		else if( splitString[1] )
		{
			socket.currentChar.DoAction( parseInt( splitString[0] ), parseInt( splitString[1] ));
		}
		else
		{
			socket.currentChar.DoAction( parseInt( cmdString ));
		}
	}
}

function command_BOLT( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 195, socket.language ); // Select character to bolt.
	socket.CustomTarget( 1, targMsg );
}

function onCallback1( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar )
	{
		ourObj.BoltEffect();
		ourObj.SoundEffect( 0x0029, true );
	}
}

function command_NPCACTION( socket, cmdString )
{
	if( cmdString )
	{
		var targMsg = GetDictionaryEntry( 213, socket.language ); // Select NPC to make act.
		socket.xtext = cmdString;
		socket.CustomTarget( 0, targMsg );
	}
}

function onCallback0( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar )
	{
		var cmdString = socket.xtext;
		var splitString = cmdString.split( " " );
		if( splitString[3] )
		{
			ourObj.DoAction( parseInt( splitString[0] ), null, parseInt( splitString[2] ), ( parseInt( splitString[3] ) == 1 ? true : false ));
		}
		else if( splitString[2] )
		{
			ourObj.DoAction( parseInt( splitString[0] ), null, parseInt( splitString[2] ));
		}
		else if( splitString[1] )
		{
			ourObj.DoAction( parseInt( splitString[0] ), parseInt( splitString[1] ));
		}
		else
		{
			ourObj.DoAction( parseInt( cmdString ));
		}
	}
	socket.xtext = "";
}