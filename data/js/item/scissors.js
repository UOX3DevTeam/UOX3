function onUse( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && iUsed && iUsed.isItem )
	{
		socket.tempObj = iUsed;
		var targMsg = GetDictionaryEntry( 471, socket.Language );
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
			socket.SysMessage( GetDictionaryEntry( 1491, socket.Language ) );
			return;
		}

		var ownerObj = GetPackOwner( ourObj, 0 );
		if( ownerObj && mChar.serial == ownerObj.serial )
		{

			var resID = ourObj.id;
			if( resID >= 0x0F95 && resID <= 0x0F9C )		// Bolt of Cloth
			{
				CreateBlankItem( socket, mChar, 50, GetDictionaryEntry( 1490 ), 0x175F, 0, "ITEM", true );
				mChar.SoundEffect( 0x0248, true );
				if( ourObj.amount > 1 )
					ourObj.amount = (ourObj.amount-1);
				else
					ourObj.Delete();
			}
			else if( ( resID >= 0x1515 && resID <= 0x1519 ) || ( resID >= 0x152E && resID <= 0x1531 ) ||
				 ( resID >= 0x1537 && resID <= 0x1544 ) || ( resID >= 0x1914 && resID <= 0x1915 ) ||
				 ( resID >= 0x1EFD && resID <= 0x1F04 ) )	// Cut Cloth
			{
				if( ourObj.container.serial != mChar.serial )
				{
					CreateBlankItem( socket, mChar, RandomNumber( 0, 4 ), GetDictionaryEntry( 1489 ), 0x0E21, 0, "ITEM", true );
					mChar.SoundEffect( 0x0248, true );
					if( ourObj.amount > 1 )
						ourObj.amount = (ourObj.amount-1);
					else
						ourObj.Delete();
				}
				else
					socket.SysMessage( GetDictionaryEntry( 1491, socket.Language ) );
			}
			else if( resID >= 0x175D && resID <= 0x1764 )		// Folded Cloth
			{
				socket.SysMessage( GetDictionaryEntry( 1488, socket.Language ) );
				CreateBlankItem( socket, mChar, 2, GetDictionaryEntr( 1489 ), 0x0E21, 0, "ITEM", true );
				mChar.SoundEffect( 0x0248, true );
				if( ourObj.amount > 1 )
					ourObj.amount = (ourObj.amount-1);
				else
					ourObj.Delete();
			}
			else
				socket.SysMessage( "You can't make anything from that material." );
		}
		else
			socket.SysMessage( GetDictionaryEntry( 775, socket.Language ) );
	}
}

