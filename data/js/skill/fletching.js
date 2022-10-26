function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && iUsed && iUsed.isItem )
	{
		if( iUsed.id == 0x1022 || iUsed.id == 0x1023 )
		{
			socket.MakeMenu( 51, 8 );
			return false;
		}
		socket.tempObj = iUsed;
		var targMsg = GetDictionaryEntry( 454, socket.language ); // What would you like to use this with?
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
			socket.SysMessage( GetDictionaryEntry( 779, socket.language ) ); // You cannot use that for fletching.
			return;
		}

		var useID = bItem.id;
		var resID = ourObj.id;
		if( ( useID >= 0x1BD1 && useID <= 0x1BD3 && resID >= 0x1BD4 && resID <= 0x1BD6 ) ||
		    ( useID >= 0x1BD4 && useID <= 0x1BD6 && resID >= 0x1BD1 && resID <= 0x1BD3 ) ) 	// Feathers on Shaft or Shaft on Feathers
		{
			var ownerObj = GetPackOwner( ourObj, 0 );
			if( ownerObj && mChar.serial == ownerObj.serial )
			{
				socket.MakeMenu( 51, 8 );
			}
			else
			{
				socket.SysMessage( GetDictionaryEntry( 778, socket.language ) ); // You can't use items outside your backpack.
			}
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 6006, socket.language ) ); // You can't make anything from that material.
		}
	}
}

