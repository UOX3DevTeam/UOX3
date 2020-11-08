function CommandRegistration()
{
	RegisterCommand( "brightlight", 2, true );
	RegisterCommand( "darklight", 2, true );
	RegisterCommand( "dungeonlight", 2, true );
}

function command_BRIGHTLIGHT( socket, cmdString )
{
	if( cmdString )
	{
		var newVal 	= parseInt( cmdString );
		var txtMessage 	= GetDictionaryEntry( 50, socket.language );
		WorldBrightLevel( newVal );
		socket.SysMessage( txtMessage );
	}
}

function command_DARKLIGHT( socket, cmdString )
{
	if( cmdString )
	{
		var newVal 	= parseInt( cmdString );
		var txtMessage 	= GetDictionaryEntry( 51, socket.language );
		WorldDarkLevel( newVal );
		socket.SysMessage( txtMessage );
	}
}

function command_DUNGEONLIGHT( socket, cmdString )
{
	if( cmdString )
	{
		var newVal 	= parseInt( cmdString );
		var txtMessage 	= GetDictionaryEntry( 52, socket.language );
		WorldDungeonLevel( newVal );
		socket.SysMessage( txtMessage );
	}
}