/// <reference path="../definitions.d.ts" />
// @ts-check
// This command is an alias for 'set race # that sets the race ID of target

function CommandRegistration()
{
	RegisterCommand( "setrace", 8, true );
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_SETRACE( socket, cmdString )
{
	if( cmdString )
	{
		var pUser = socket.currentChar;
		var race = parseInt( cmdString );
		pUser.raceID = race;
	}
}
