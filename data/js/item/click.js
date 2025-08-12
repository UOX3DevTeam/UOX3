/// <reference path="../definitions.d.ts" />
// @ts-check
/** @type { ( sockPlayer: Socket, objClicked: BaseObject ) => void } */
function onClick( pUser, iUsed )
{
   	pUser.SysMessage( "You have clicked on: " + iUsed.name );
}
