/// <reference path="../definitions.d.ts" />
// @ts-check
// This command triggers the event manager command handling in js/server/event_manager.js
function CommandRegistration()
{
	RegisterCommand( "em", 8, true );
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_EM( socket, cmdString )
{
	TriggerEvent( 5, "EventManagerCommands", socket, cmdString );
}
