function CommandRegistration()
{
	RegisterCommand( "movetobag", 2, true );
}

function command_MOVETOBAG( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 197, socket.Language );
	socket.CustomTarget( 0, targMsg );
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
