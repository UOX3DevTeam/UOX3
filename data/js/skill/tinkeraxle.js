function onUseChecked( pUser, iUsed )
{
	var pSock = pUser.socket;
	if( pSock && iUsed && iUsed.isItem )
	{
		var pOwner = GetPackOwner( iUsed, 0 );
		if( pOwner && pOwner.serial == pUser.serial )
		{
			pSock.tempObj = iUsed;
			pSock.CustomTarget( 0, GetDictionaryEntry( 477, pSock.language ) );
		}
		else
			pSock.SysMessage( GetDictionaryEntry( 775, pSock.language ) );
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
			pSock.SysMessage( GetDictionaryEntry( 962, pSock.language ) );
			return;
		}
		if( ourObj.serial == bItem.serial )
		{
			pSock.SysMessage( GetDictionaryEntry( 958, pSock.language ) );
			return;
		}

		var pOwner = GetPackOwner( ourObj, 0 );
		if( pOwner && pOwner.serial == pUser.serial )
		{
			var useID = bItem.id;
			var resID = ourObj.id;
			switch( useID )
			{
			case 0x1053:		// Gears
			case 0x1054:
				if( resID == 0x105B || resID == 0x105C )
					canCombine = true;
				break;
			case 0x105B:		// Axles
			case 0x105C:
				if( resID == 0x1053 || resID == 0x1054 )
					canCombine = true;
				break;
			default:
				break;
			}
		}
		else
		{
			pSock.SysMessage( GetDictionaryEntry( 963, pSock.language ) );
			return;
		}

		if( canCombine )
		{
			if( !pUser.CheckSkill( 37, 0, 1000 ) )
				pSock.SysMessage( GetDictionaryEntry( 959, pSock.language ) );
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 960, pSock.language ) );
				if( ourObj.amount > 1 )
					ourObj.amount = ourObj.amount - 1;
				else
					ourObj.Delete();
				if( bItem.amount > 1 )
					bItem.amount = bItem.amount - 1;
				else
					bItem.Delete();

				CreateBlankItem( pSock, pUser, 1, GetDictionaryEntry( 961 ), 0x1051, 0, "ITEM", true );
			}
		}
	}
	if( !canCombine )
		pSock.SysMessage( "You cannot combine those to make anything useful." );
}

