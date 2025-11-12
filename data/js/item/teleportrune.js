/// <reference path="../definitions.d.ts" />
// @ts-check
/** @type { ( user: Character, iUsing: Item ) => boolean } */
function onUseChecked( pUser, iUsed )
{
	var pSocket = pUser.socket;
	if ( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}
	if( pSocket && iUsed && iUsed.isItem )
	{
		pSocket.tempObj = iUsed;
		var targMsg = GetDictionaryEntry( 401, pSocket.language ); //Select teleport target.
		pSocket.CustomTarget( 1, targMsg );
	}
	return false;
}

/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
function onCallback1( socket, ourObj )
{
	var pUser = socket.currentChar;
	if( ValidateObject( pUser ))
	{
		if( pUser.visible == 1 || pUser.visible == 2 )
		{
			pUser.visible = 0;
		}
		var targX;
		var targY;
		var targZ;
		if( !socket.GetWord( 1 ) && ourObj )
		{
			targX = ourObj.x;
			targY = ourObj.y;
			targZ = ourObj.z;
		}
		else
		{
			targX = socket.GetWord( 11 );
			targY = socket.GetWord( 13 );
			targZ = socket.GetSByte( 16 );
			var StrangeByte = socket.GetWord(1);

			// If connected with a client lower than v7.0.9, manually add height of targeted tile
			if ((StrangeByte == 0 && ourObj.isItem) || (socket.clientMajorVer <= 7 && socket.clientSubVer < 9))
			{
				targZ += GetTileHeight( socket.GetWord( 17 ));
			}
		}
		pUser.SoundEffect( 0x01FE, true );
		pUser.SetLocation( targX, targY, targZ );
		pUser.StaticEffect( 0x372A, 0x09, 0x06 );
	}
}
