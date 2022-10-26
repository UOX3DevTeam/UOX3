function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && iUsed && iUsed.isItem )
	{
		socket.tempObj = iUsed;
		var targMsg = GetDictionaryEntry( 445, socket.language ); // Select material to use.
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
			socket.SysMessage( GetDictionaryEntry( 783, socket.language )); // You cannot use that material for carpentry.
			return;
		}

		var resID = ourObj.id;
		if( resID >= 0x1BD7 && resID <= 0x1BE2 ) 	// Is it a valid carpentry resource?
		{
			var ownerObj = GetPackOwner( ourObj, 0 );
			if( ownerObj && mChar.serial == ownerObj.serial )
			{
				socket.MakeMenu( 19, 11 );
			}
			else
			{
				socket.SysMessage( GetDictionaryEntry( 781, socket.language )); // You can't use lumber outside your backpack.
			}
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 6006, socket.language )); // You can't make anything from that material.
		}
	}
}

