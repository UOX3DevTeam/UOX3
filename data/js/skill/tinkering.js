function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && iUsed && iUsed.isItem )
	{
		socket.tempObj = iUsed;
		var targMsg = GetDictionaryEntry( 484, socket.language );
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
			socket.SysMessage( GetDictionaryEntry( 942, socket.language ) );
			return;
		}

		if( ourObj.movable == 2 || ourObj.movable == 3 )
		{
			socket.SysMessage( "This has to be in your backpack before you can use it." );
			return;
		}

		var resID = ourObj.id;
		if( resID == 0x1BEF || resID == 0x1BF2 || resID == 0x1BDD || resID == 0x1BE0 )	// A valid tinker resource
		{
			var ownerObj = GetPackOwner( ourObj, 0 );
			if( ownerObj && mChar.serial == ownerObj.serial )
			{
				if( resID != 0x1BDD && resID != 0x1BE0 )
					socket.MakeMenu( 59, 37 );
				else
					socket.SysMessage( GetDictionaryEntry( 941, socket.language ) );
			}
			else
				socket.SysMessage( GetDictionaryEntry( 775, socket.language ) );
		}
		else
			socket.SysMessage( "You can't make anything from that material." );
	}
}

