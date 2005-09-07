function onUse( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && iUsed && iUsed.isItem )
	{
		socket.tempObj = iUsed;
		var targMsg = GetDictionaryEntry( 459, socket.Language );
		socket.CustomTarget( 1, targMsg );
	}
	return false;
}

function onCallback1( socket, ourObj )
{
	var bItem = socket.tempObj;
	var mChar = socket.currentChar;

	socket.tempObj = null;

	if( mChar && mChar.isChar && bItem && bItem.isItem )
	{
		if( !ourObj || !ourObj.isItem )
		{
			socket.SysMessage( GetDictionaryEntry( 777, socket.Language ) );
			return;
		}

		var resID = ourObj.id;
		if( ( resID >= 0x0F95 && resID <= 0x0F9C ) || ( resID >= 0x175D && resID <= 0x1765 ) || resID == 0x1067 || resID == 0x1078 )	// A valid tailoring resource
		{
			var ownerObj = GetPackOwner( ourObj, 0 );
			if( ownerObj && mChar.serial == ownerObj.serial )
			{
				var countResource = mChar.ResourceCount( resID );
				if( countResource >= 1 )
					socket.MakeMenu( 39, 34 );
				else
					socket.SysMessage( GetDictionaryEntry( 776, socket.Language ) );
			}
			else
				socket.SysMessage( GetDictionaryEntry( 775, socket.Language ) );
		}
		else
			socket.SysMessage( "You can't make anything from that material." );
	}
}

