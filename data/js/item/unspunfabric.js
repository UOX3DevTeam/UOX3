function onUse( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && iUsed && iUsed.isItem )
	{
		var ownerObj = GetPackOwner( iUsed, 0 );
		if( ownerObj && pUser.serial == ownerObj.serial )
		{
			socket.tempObj = iUsed;
			var targMsg = GetDictionaryEntry( 449, socket.Language );
			socket.CustomTarget( 0, targMsg );
		}
		else
			socket.SysMessage( GetDictionaryEntry( 775, socket.Language ) );
	}
	return false;
}

function onCallback0( socket, ourObj )
{
	var bItem = socket.tempObj;
	var mChar = socket.currentChar;

	socket.tempObj = null;

	if( mChar && mChar.isChar && bItem && bItem.isItem )
	{
		if( !ourObj || !ourObj.isItem )
		{
			socket.SysMessage( GetDictionaryEntry( 823, socket.Language ) );
			return;
		}

		var useID = ourObj.id;
		if( useID >= 0x10A4 && useID <= 0x10A6 )		// Is it a Wheel
		{
			if( mChar.InRange( ourObj, 3 ) )
			{
				if( mChar.CheckSkill( 34, 0, 1000 ) )
				{
					socket.SysMessage( GetDictionaryEntry( 822, socket.Language ) );
					var itemID = 0x0FA0;
					if( bItem.id == 0x0DF8 )	// Wool
						itemID = 0x0E1D;
					else if( bItem.id == 0x0DF9 )	// Cotton
						itemID = 0x0FA0;
	
					var newItem = CreateBlankItem( socket, mChar, 4, "#", itemID, 0, "ITEM", true );
					if( newItem && newItem.isItem )
						newItem.decay = true;
	
					if( bItem.amount > 1 )
						bItem.amount = (bItem.amount-1);
					else
						bItem.Delete();
				}
				else
					socket.SysMessage( GetDictionaryEntry( 821, socket.Language ) );
			}
			else
				socket.SysMessage( "That is too far away." );
		}
		else
			socket.SysMessage( "You can't make anything with that." );
	}
}

