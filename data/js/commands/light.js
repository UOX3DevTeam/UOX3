function CommandRegistration(contextID)
{
	RegisterCommand( "brightlight", 2, true,contextID );
	RegisterCommand( "darklight", 2, true,contextID );
	RegisterCommand( "dungeonlight", 2, true,contextID );
}

function command_BRIGHTLIGHT( socket, cmdString )
{
	if( cmdString )
	{
		var newVal 	= StringToNum( cmdString );
		var txtMessage 	= GetDictionaryEntry( 50, socket.Language );
		WorldBrightLevel( newVal );
		socket.SysMessage( txtMessage );
	}
}

function command_DARKLIGHT( socket, cmdString )
{
	if( cmdString )
	{
		var newVal 	= StringToNum( cmdString );
		var txtMessage 	= GetDictionaryEntry( 51, socket.Language );
		WorldDarkLevel( newVal );
		socket.SysMessage( txtMessage );
	}
}

function command_DUNGEONLIGHT( socket, cmdString )
{
	if( cmdString )
	{
		var newVal 	= StringToNum( cmdString );
		var txtMessage 	= GetDictionaryEntry( 52, socket.Language );
		WorldDungeonLevel( newVal );
		socket.SysMessage( txtMessage );
	}
}