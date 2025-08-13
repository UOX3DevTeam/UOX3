/// <reference path="../definitions.d.ts" />
// @ts-check
// This command teleports user to their current location to resend their character
// to everyone around them and also get updates from all objects nearby

function CommandRegistration()
{
	RegisterCommand( "resend", 4, true );
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_RESEND( socket, cmdString )
{
	var pUser = socket.currentChar;
	pUser.Teleport();
}
