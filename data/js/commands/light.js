/// <reference path="../definitions.d.ts" />
// @ts-check
// These commands can be used to update the light levels in the game

function CommandRegistration()
{
	RegisterCommand( "brightlight", 8, true );
	RegisterCommand( "darklight", 8, true );
	RegisterCommand( "dungeonlight", 8, true );
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
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

/** @type { ( socket: Socket, cmdString: string ) => void } */
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

/** @type { ( socket: Socket, cmdString: string ) => void } */
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
