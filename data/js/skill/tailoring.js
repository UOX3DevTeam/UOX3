function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && iUsed && iUsed.isItem )
	{
		socket.tempObj = iUsed;
		var targMsg = GetDictionaryEntry( 459, socket.language ); // Select material to use.
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
			socket.SysMessage( GetDictionaryEntry( 777, socket.language )); // You cannot use that material for tailoring.
			return;
		}

		if( ourObj.movable == 2 || ourObj.movable == 3 )
		{
			socket.SysMessage( GetDictionaryEntry( 6022, socket.language )); // This has to be in your backpack before you can use it.
			return;
		}

		var resID = ourObj.id;
		if( resID >= 0x0F95 && resID <= 0x0F9C )
		{
			CreateBlankItem( socket, mChar, 50, "#", 0x1766, 0, "ITEM", true );
			if( ourObj.amount > 1 )
			{
				ourObj.amount = ourObj.amount - 1;
			}
			else
			{
				ourObj.Delete();
			}
		}
		else if(( resID >= 0x175D && resID <= 0x1768 ) || resID == 0x1078 || resID == 0x1079 || resID == 0x1081 || resID == 0x1082 ) // A valid tailoring resource
		{
			var ownerObj = GetPackOwner( ourObj, 0 );
			if( ownerObj && mChar.serial == ownerObj.serial )
			{
				socket.MakeMenu( 39, 34 );
			}
			else
			{
				socket.SysMessage( GetDictionaryEntry( 775, socket.language )); // You can't use material outside your backpack.
			}
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 6006, socket.language )); // You can't make anything from that material.
		}
	}
}

