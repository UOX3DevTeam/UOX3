function onUseChecked( pUser, iUsed )
{
	var pSock = pUser.socket;
	if( pSock && iUsed && iUsed.isItem )
	{
		var pOwner = GetPackOwner( iUsed, 0 );
		if( pOwner && pOwner.serial == pUser.serial )
		{
			pSock.tempObj = iUsed;
			pSock.CustomTarget( 0, GetDictionaryEntry( 479, pSock.language )); // Select clock frame to combine with.
		}
		else
		{
			pSock.SysMessage( GetDictionaryEntry( 775, pSock.language )); // You can't use material outside your backpack.
		}
	}
	return false;
}

function onCallback0( pSock, ourObj )
{
	var bItem = pSock.tempObj;
	var pUser = pSock.currentChar;

	pSock.tempObj = null;
	var canCombine = false;

	if( pUser && pUser.isChar && bItem && bItem.isItem )
	{
		if( !ourObj || !ourObj.isItem )
		{
			pSock.SysMessage( GetDictionaryEntry( 962, pSock.language )); // You can't combine these.
			return;
		}
		if( ourObj.serial == bItem.serial )
		{
			pSock.SysMessage( GetDictionaryEntry( 958, pSock.language )); // You can't combine an item with itself!
			return;
		}

		var pOwner = GetPackOwner( ourObj, 0 );
		if( pOwner && pOwner.serial == pUser.serial )
		{
			var useID = bItem.id;
			var resID = ourObj.id;
			switch( useID )
			{
				case 0x104D: // Clock Frame
				case 0x104E:
					if( resID == 0x104F || resID == 0x1050 )
					{
						canCombine = true;
					}
					break;
				case 0x104F: // Clock Parts
				case 0x1050:
					if( resID == 0x104D || resID == 0x104E )
					{
						canCombine = true;
					}
					break;
				default:
					break;
			}
		}
		else
		{
			pSock.SysMessage( GetDictionaryEntry( 963, pSock.language )); // You can't combine items outside your backpack.
			return;
		}

		if( canCombine )
		{
			if( !pUser.CheckSkill( 37, 0, pUser.skillCaps.tinkering ))
			{
				pSock.SysMessage( GetDictionaryEntry( 959, pSock.language )); // You failed to combine the parts.
			}
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 960, pSock.language )); // You combined the parts.
				if( ourObj.amount > 1 )
				{
					ourObj.amount = ourObj.amount - 1;
				}
				else
				{
					ourObj.Delete();
				}
				if( bItem.amount > 1 )
				{
					bItem.amount = bItem.amount - 1;
				}
				else
				{
					bItem.Delete();
				}

				CreateBlankItem( pSock, pUser, 1, "#", 0x104B, 0, "ITEM", true );
			}
		}
	}
	if( !canCombine )
	{
		pSock.SysMessage( GetDictionaryEntry( 6023, pSock.language )); // You cannot combine those to make anything useful.
	}
}

