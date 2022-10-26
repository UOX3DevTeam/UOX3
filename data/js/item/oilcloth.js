function onUseChecked( pUser, iUsed )
{
	var itemOwner = GetPackOwner( iUsed, 0 );
	if( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}
	if( itemOwner == null || itemOwner != pUser )
	{
		pUser.SysMessage( GetDictionaryEntry( 1763, pUser.socket.language )); // That item must be in your backpack before it can be used.
	}
	else
	{
		pUser.socket.tempObj = iUsed;
		pUser.CustomTarget( 0, GetDictionaryEntry( 2751, pUser.socket.language )); // Select the weapon or armour you wish to use the cloth on.
	}
	return false;
}

function onCallback0( pSock, myTarget )
{
	var iUsed = pSock.tempObj;
	var pUser = pSock.currentChar;
	if( !pSock.GetWord( 1 ) && myTarget.isItem )
	{
		// That must be in your pack for you to use it.
		var itemOwner = GetPackOwner( myTarget, 0 );
		if( itemOwner == null || itemOwner != pUser )
		{
			pUser.SysMessage( GetDictionaryEntry( 1763, pSock.language )); //That item must be in your backpack before it can be used.
		}
		else
		{
			if( myTarget.poison > 0 )
			{
				if( myTarget.poison > 2 )
				{
					myTarget.poison -= 2;
				}
				else
				{
					myTarget.poison = 0;
				}
				if( myTarget.poison == 0 )
				{
					pSock.SysMessage( GetDictionaryEntry( 2752, pSock.language )); // You carefully wipe the item clean of poison.
				}
				else
				{
					pSock.SysMessage( GetDictionaryEntry( 2753, pSock.language )); // You carefully wipe the item, but some poison still lingers.
				}
				pUser.UseResource( 1, 0x175d, 0x07d1 );
			}
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 2754, pSock.language )); // That item is not poisoned.
			}
		}
	}
	else
	{
		if( myTarget.isChar && myTarget == pUser && ( myTarget.id == 0x00b7 || myTarget.id == 0x00b8 ))
		{
			pSock.SysMessage( GetDictionaryEntry( 1681, pSock.language )); // You wipe away all of your body paint.
			pUser.id = pUser.orgID;
			pUser.colour = pUser.orgSkin;
			return;
		}

		pSock.SysMessage( GetDictionaryEntry( 2755, pSock.language )); // That's not an item!
	}
}