function onUse( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && iUsed && iUsed.isItem )
	{
		var pHunger = pUser.hunger;
		if( pHunger < 6 )
		{
			pUser.SoundEffect( (0x003A + RandomNumber( 0, 2 )), true );
			if( pHunger >= 0 )
				socket.SysMessage( GetDictionaryEntry( (408 + pHunger), socket.Language ) );
			else
				socket.SysMessage( GetDictionaryEntry( 415, socket.Language ) );

			iPoison = iUsed.poison;
			if( iPoison && pUser.poison < iPoison )
			{
				socket.SysMessage( GetDictionaryEntry( (416 + RandomNumber( 0, 2 )), socket.Language ) );
				pUser.SoundEffect( 0x0246, true );
				pUser.poison = iPoison;
				pUser.SetTimer( 5, 180000 );	// Set the Poison Timer (Need to use uox.ini eventually)
			}

			if( iUsed.amount > 1 )
				iUsed.amount = (iUsed.amount-1);
			else
				iUsed.Delete();

			pUser.hunger = (pHunger+1);
		}
		else
			socket.SysMessage( GetDictionaryEntry( 407, socket.Language ) );
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
			targZ = socket.GetByte( 16 ) + GetTileHeight( socket.GetWord( 17 ) );
		}
		mChar.SoundEffect( 0x01FE, true );
		mChar.SetLocation( targX, targY, targZ );
		mChar.StaticEffect( 0x372A, 0x09, 0x06 );
	}
}