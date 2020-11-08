function onUseChecked( pUser, iUsed )
{
	var itemOwner = GetPackOwner( iUsed, 0 );
	if( itemOwner == null || itemOwner != pUser )
	{
		pUser.SysMessage( GetDictionaryEntry( 1763, pUser.socket.language )); //That item must be in your backpack before it can be used.
	}
	else
	{
		pUser.socket.tempObj = iUsed;
		pUser.CustomTarget( 0, "Select the weapon or armor you wish to use the cloth on." );
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
					myTarget.poison -= 2;
				else
					myTarget.poison = 0;
				if( myTarget.poison == 0 )
					pSock.SysMessage( "You carefully wipe the item clean of poison." );
				else
					pSock.SysMessage( "You carefully wipe the item, but some poison still lingers." );
				pUser.UseResource( 1, 0x175d, 0x07d1 );//iUsed.Delete();
			}
			else
				pSock.SysMessage( "That item is not poisoned." );
		}
	}
	else
		pSock.SysMessage( "That's not an item!" );
}