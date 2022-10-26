// This command lets GMs move a targeted object to their own backpack

function CommandRegistration()
{
	RegisterCommand( "movetobag", 2, true );
	RegisterCommand( "bringtopack", 2, true );
}

function command_MOVETOBAG( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 197, socket.language ); // Select an item to move into your bag.
	socket.CustomTarget( 0, targMsg );
}

// Alias of MOVETOBAG
function command_BRINGTOPACK( socket, cmdString )
{
	command_MOVETOBAG( socket, cmdString );
}

function onCallback0( socket, ourObj )
{
	var ourChar = socket.currentChar;
	if( !socket.GetWord( 1 ) && ourChar )
	{
		var ourPack = ourChar.pack;
		if( ourObj.isItem && ourPack )
		{
			ourObj.container = ourPack;
			ourObj.PlaceInPack();
			ourObj.decaytime = 0;
		}
	}
}
