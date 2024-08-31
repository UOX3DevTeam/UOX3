function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if ( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}
	if( socket && iUsed && iUsed.isItem )
	{
		socket.tempObj = iUsed;
		var targMsg = GetDictionaryEntry( 401, socket.language ); //Select teleport target.
		socket.CustomTarget( 1, targMsg );
	}
	return false;
}

function onCallback1( socket, ourObj )
{
	var mChar = socket.currentChar;
	if ( mChar.visible == 1 || mChar.visible == 2 )
	{
		mChar.visible = 0;
	}
	if( mChar )
	{
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
		mChar.SoundEffect( 0x01FE, true );
		mChar.SetLocation( targX, targY, targZ );
		mChar.StaticEffect( 0x372A, 0x09, 0x06 );
	}
}