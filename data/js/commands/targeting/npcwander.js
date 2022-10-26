// These commands can be used to handle changes to NPC wandermodes

function CommandRegistration()
{
	RegisterCommand( "follow", 1, true );
	RegisterCommand( "npcrect", 2, true );
	RegisterCommand( "npccircle", 2, true );
}

function command_FOLLOW( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 228, socket.language ); // Select player for the NPC to follow.
	socket.CustomTarget( 0, targMsg );
}

function onCallback0( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar )
	{
		var targMsg = GetDictionaryEntry( 1742, socket.language ); // Select NPC to follow this player.
		socket.tempObj = ourObj;
		socket.CustomTarget( 1, targMsg );
	}
}

function onCallback1( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar )
	{
		var toFollow = socket.tempObj;
		if( ourObj.npc && toFollow )
		{
			ourObj.Follow( toFollow );
		}
	}
	socket.tempObj = null;
}

function command_NPCRECT( socket, cmdString )
{
	if( cmdString )
	{
		var splitSection = cmdString.split( " " );
		if( splitSection[3] )
		{
			x1 = parseInt( splitSection[0] );
			y1 = parseInt( splitSection[1] );
			x2 = parseInt( splitSection[2] );
			y2 = parseInt( splitSection[3] );
			var targMsg = GetDictionaryEntry( 46 ); // Select the NPC to set the bounding rectangle for.
			socket.CustomTarget( 2, targMsg );
		}
	}
}

function onCallback2( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar && ourObj.npc )
	{
		ourObj.Wander( x1, y1, x2, y2 );
	}
}

function command_NPCCIRCLE( socket, cmdString )
{
	if( cmdString )
	{
		var splitSection = cmdString.split( " " );
		if( splitSection[2] )
		{
			x1 = parseInt( splitSection[0] );
			y1 = parseInt( splitSection[1] );
			x2 = parseInt( splitSection[2] );
			var targMsg = GetDictionaryEntry( 47 ); // Select the NPC to set the bounding circle for.
			socket.CustomTarget( 3, targMsg );
		}
	}
}

function onCallback3( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar && ourObj.npc )
	{
		ourObj.Wander( x1, y1, x2 );
	}
}