function CommandRegistration()
{
	RegisterCommand( "makeshop", 2, true );
}

function command_MAKESHOP( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 232, socket.language );
	socket.CustomTarget( 0, targMsg );
}

function onCallback0( socket, ourObj )
{
	if( ValidateObject( ourObj ) )
	{
		if( !ourObj.isChar )
			socket.SysMessage( GetDictionaryEntry( 1069, socket.language ) );
		else
		{
			socket.SysMessage( GetDictionaryEntry( 1068, socket.language ) );
			ourObj.isShop = true;
		}
	}
}
