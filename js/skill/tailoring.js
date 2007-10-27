function onUseChecked( pUser, iUsed )
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
		if( resID >= 0x0F95 && resID <= 0x0F9C )
		{
			CreateBlankItem( pSock, pUser, 50, "#", 0x1766, 0, "ITEM", true );
			if( ourObj.amount > 1 )
				ourObj.amount = ourObj.amount - 1;
			else
				ourObj.Delete();
		}
		else if( ( resID >= 0x175D && resID <= 0x1768 ) || resID == 0x1078 || resID == 0x1079 )	// A valid tailoring resource
		{
			var ownerObj = GetPackOwner( ourObj, 0 );
			if( ownerObj && mChar.serial == ownerObj.serial )
				socket.MakeMenu( 39, 34 );
			else
				socket.SysMessage( GetDictionaryEntry( 775, socket.Language ) );
		}
		else
			socket.SysMessage( "You can't make anything from that material." );
	}
}

