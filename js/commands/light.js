function CommandRegistration()
{
	RegisterCommand( "brightlight", 2, true );
	RegisterCommand( "darklight", 2, true );
	RegisterCommand( "dungeonlight", 2, true );
}

function command_BRIGHTLIGHT( socket, cmdString )
{
	var newVal 	= StringToNum( cmdString );
	var txtMessage 	= GetDictionaryEntry( 50 );
	WorldBrightLevel( newVal );
	socket.SysMessage( txtMessage );
}

function command_DARKLIGHT( socket, cmdString )
{
	var newVal 	= StringToNum( cmdString );
	var txtMessage 	= GetDictionaryEntry( 51 );
	WorldDarkLevel( newVal );
	socket.SysMessage( txtMessage );
}

function command_DUNGEONLIGHT( socket, cmdString )
{
	var newVal 	= StringToNum( cmdString );
	var txtMessage 	= GetDictionaryEntry( 52 );
	WorldDungeonLevel( newVal );
	socket.SysMessage( txtMessage );
}