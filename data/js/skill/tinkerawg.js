function onUseChecked( pUser, iUsed )
{
	var pSock = pUser.socket;
	if( pSock && iUsed && iUsed.isItem )
	{
		var pOwner = GetPackOwner( iUsed, 0 );
		if( pOwner && pOwner.serial == pUser.serial )
		{
			pSock.tempObj = iUsed;
			pSock.CustomTarget( 0, GetDictionaryEntry( 478, pSock.language )); // Select part to combine it with.
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
			var idToMake;
			var useID = bItem.id;
			var resID = ourObj.id;
			if((( useID == 0x1051 || useID == 0x1052 ) && ( resID == 0x105D || resID == 0x105E )) ||
			    (( useID == 0x105D || useID == 0x105E ) && ( resID == 0x1051 || resID == 0x1052 )))
			{
				idToMake = 0x104F;
			}
			else if((( useID == 0x1051 || useID == 0x1052 ) && ( resID == 0x1055 || resID == 0x1056 )) ||
				 (( useID == 0x1055 || useID == 0x1056 ) && ( resID == 0x1051 || resID == 0x1052 )))
			{
				idToMake = 0x1059;
			}
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 962, pSock.language )); // You can't combine these.
				return;
			}

			if( !pUser.CheckSkill( 37, 0, 1000 ))
			{
				pSock.SysMessage( GetDictionaryEntry( 964, pSock.language )); // Your fingers slip and you break one of the parts.
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

				CreateBlankItem( pSock, pUser, 1, "#", idToMake, 0, "ITEM", true );
			}

		}
		else
		{
			pSock.SysMessage( GetDictionaryEntry( 963, pSock.language )); // You can't combine items outside your backpack.
		}
	}
}

