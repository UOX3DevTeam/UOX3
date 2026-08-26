/// <reference path="../definitions.d.ts" />
// @ts-check

// Admin command for spawning a High Seas pirate ship encounter.

/** @type { (  ) => any } */
function CommandRegistration() { RegisterCommand( "addpirateship", 2, true ); }

/** @type { ( socket: Socket, cmdString: string ) => any } */
function command_ADDPIRATESHIP( socket, cmdString ) { socket.CustomTarget( 0, "Target open water for the pirate ship." ); }

/** @type { ( socket: Socket, target: Character | Item | null ) => any } */
function onCallback0( socket, target )
{
	if( parseInt( socket.GetByte( 11 ) ) == 255 )
	{
		return;
	}

	TriggerEvent( 5100, "AddPirateShip", socket, socket.GetWord( 11 ), socket.GetWord( 13 ), socket.GetSByte( 16 ) );
}
