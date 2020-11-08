function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
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
			targZ = socket.GetSByte( 16 ) + GetTileHeight( socket.GetWord( 17 ) );
		}
		mChar.SoundEffect( 0x01FE, true );
		mChar.SetLocation( targX, targY, targZ );
		mChar.StaticEffect( 0x372A, 0x09, 0x06 );
	}
}