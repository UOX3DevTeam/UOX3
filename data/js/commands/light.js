// These commands can be used to update the light levels in the game

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
		var txtMessage 	= GetDictionaryEntry( 50, socket.language ); // World bright light level set.
		WorldBrightLevel( newVal );
		socket.SysMessage( txtMessage );
	}
}

function command_DARKLIGHT( socket, cmdString )
{
	if( cmdString )
	{
		var newVal 	= parseInt( cmdString );
		var txtMessage 	= GetDictionaryEntry( 51, socket.language ); // World dark light level set.
		WorldDarkLevel( newVal );
		socket.SysMessage( txtMessage );
	}
}

function command_DUNGEONLIGHT( socket, cmdString )
{
	if( cmdString )
	{
		var newVal 	= parseInt( cmdString );
		var txtMessage 	= GetDictionaryEntry( 52, socket.language ); // Dungeon light level set.
		WorldDungeonLevel( newVal );
		socket.SysMessage( txtMessage );
	}
}