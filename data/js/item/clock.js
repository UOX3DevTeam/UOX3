/// <reference path="../definitions.d.ts" />
// @ts-check
function onUseChecked( pUser, iUsed )
{
	var pSock = pUser.socket;
	TriggerEvent( 1052, "TellTime", pSock );
	return false;
}

