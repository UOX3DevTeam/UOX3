function CommandRegistration()
{
	RegisterCommand( "remove", 2, true );
}

function command_REMOVE( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 188 );
	socket.CustomTarget( 0, targMsg );
}

function onCallback0( socket, ourObj )
{
	if( ourObj.isChar )
	{
		if( !ourObj.npc )
			return;

		socket.SysMessage( GetDictionaryEntry( 1015 ) );
		ourObj.Delete();
		return;
	}
	if( ourObj.isItem )
	{
		socket.SysMessage( GetDictionaryEntry( 1013 ) );
		ourObj.Delete();
	}
}
