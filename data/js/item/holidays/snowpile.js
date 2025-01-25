function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}

	if( socket && iUsed && iUsed.isItem )
	{
		//Check to see if it's locked down
		if( iUsed.movable == 2 || iUsed.movable == 3 )
		{
			socket.SysMessage( GetDictionaryEntry( 774, socket.language )); // That is locked down and you cannot use it
			return false;
		}
		var itemOwner = GetPackOwner( iUsed, 0 );
		if( itemOwner == null || itemOwner.serial != pUser.serial ) 
		{
			pUser.SysMessage( GetDictionaryEntry( 1763, socket.language )); // // The item must be in your backpack to use it.
			return false;
		}
		else if( pUser.isonhorse )
		{
			pUser.SysMessage( GetDictionaryEntry(2713, socket.language )); // You cannot use this while riding a mount.
			return false;
		}


		if( pUser.GetTempTag( "snowpile" ) == null || pUser.GetTempTag( "snowpile" ) == 0 )
		{
			socket.tempObj = iUsed;
			pUser.SetTempTag( "snowpile", 1 );

			//the addition of the false-flag in the EmoteMessage below tells the server to only
			//send the message to this character
			pUser.SysMessage( GetDictionaryEntry( 5512, socket.language )); //You carefully pack the snow into a ball...
			pUser.CustomTarget(0);
			pUser.StartTimer( 5000, 0, true );
		}
		else if( pUser.GetTempTag( "snowpile" ) == 1 )
		{
			pUser.SysMessage( GetDictionaryEntry( 5511, socket.language )); // The snow is not ready to be packed yet.  Keep trying.
		}
		else
		{
			return true;
		}
	}
	return false;
}

function onCallback0( socket, myTarget )
{
	var pUser = socket.currentChar;
	var iUsed = socket.tempObj;
	if( pUser.visible == 1 || pUser.visible == 2 ) 
	{
		pUser.visible = 0;
	}

	if( !socket.GetWord( 1 ) && myTarget.isChar && myTarget.socket )
	{
		pUser.DoAction( 0x9 );
		pUser.SoundEffect( 0x145, true );
		DoMovingEffect( pUser.x, pUser.y, pUser.z + 15, myTarget.x, myTarget.y, myTarget.z + 3, 0x36E4, 0x7, 0x00, true, 0x480, 0 );
		myTarget.SysMessage( GetDictionaryEntry(5508, socket.language )); // You have just been hit by a snowball!
		pUser.SysMessage( GetDictionaryEntry (5509, socket.language )); // You throw the snowball and hit the target!
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 5510, socket.language )); // You can only throw a snowball at something that can throw one back.
	}
}

function onTimer( pUser, timerID )
{
	var socket = pUser.socket;
	if( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}

	if( timerID == 0 )
	{
		pUser.SetTempTag( "snowpile", null );
	}
}