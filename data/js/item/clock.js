/// <reference path="../definitions.d.ts" />
// @ts-check
/** @type { ( user: Character, iUsing: Item ) => boolean } */
function onUseChecked( pUser, iUsed )
{
	var pSock = pUser.socket;
	TriggerEvent( 1052, "TellTime", pSock );
	return false;
}

