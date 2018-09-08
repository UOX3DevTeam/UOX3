function onUseChecked( pUser, iUsed )
{
	var pSock = pUser.socket;
	if( pSock && iUsed && iUsed.isItem )
	{
		pSock.tempObj = iUsed;
		pSock.CustomTarget( 0, GetDictionaryEntry( 475, pSock.Language ) );
	}
	return false;
}

function onCallback0( pSock, ourObj )
{
	var bItem = pSock.tempObj;
	var pUser = pSock.currentChar;

	pSock.tempObj = null;

	if( ValidateObject( pUser ) && pUser.isChar && ValidateObject( bItem ) && bItem.isItem )
	{
		if( !ourObj || !ourObj.isItem )
		{
			pSock.SysMessage( "That does not have a lock." );
			return;
		}
		var iType = ourObj.type;
		switch( iType )
		{
		case 1:		// Container
		case 12:	// Door
		case 63:	// Spawn Container
			pSock.SysMessage( GetDictionaryEntry( 932, pSock.Language ) );
			break;
		case 8:		// Locked Container
		case 13:	// Locked Door
		case 64:	// Locked Spawn Container
			if( ourObj.more == 0 )
			{
				if( pUser.CheckSkill( 24, 0, 1000 ) )
				{
					if( iType == 8 )
						ourObj.type = 1;
					else if( iType == 13 )
						ourObj.type = 12;
					else if( iType == 64 )
						ourObj.type = 63;
					pSock.SoundEffect( 0x01FF, false );
					pSock.SysMessage( GetDictionaryEntry( 935, pSock.Language ) );
				}
				else
				{
					if( RandomNumber( 0, 1 ) )
					{
						pSock.SysMessage( GetDictionaryEntry( 933, pSock.Language ) );
						if( bItem.amount > 1 )
							bItem.amount = (bItem.amount-1);
						else
							bItem.Delete();
					}
					else
						pSock.SysMessage( GetDictionaryEntry( 936, pSock.Language ) );
				}
			}
			else
				pSock.SysMessage( GetDictionaryEntry( 937, pSock.Language ) );
			break;
		default:
			pSock.SysMessage( "That does not have a lock" );
			break;
		}
	}
}

