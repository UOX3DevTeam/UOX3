/// <reference path="../definitions.d.ts" />
// @ts-check
/** @type { ( user: Character, iUsing: Item ) => boolean } */
function onUseChecked( pUser, iUsed )
{
	var pSock = pUser.socket;
	TriggerEvent( 1052, "TellTime", pSock );
	pUser.RemoveLiveStatic( pUser.x, pUser.y, pUser.z, pUser.worldnumber, 0x0007, 0 );
    pUser.SysMessage( "Added live static." );
	return false;
}

