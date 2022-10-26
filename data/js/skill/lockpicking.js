function onUseChecked( pUser, iUsed )
{
	var pSock = pUser.socket;
	if( pSock && iUsed && iUsed.isItem )
	{
		if( iUsed.movable == 2 || iUsed.movable == 3 )
		{
			pSock.SysMessage( GetDictionaryEntry( 774, pSock.language )); // That is locked down and you cannot use it.
			return;
		}

		pSock.tempObj = iUsed;
		pSock.CustomTarget( 0, GetDictionaryEntry( 475, pSock.language )); // What lock would you like to pick?
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
			pSock.SysMessage( GetDictionaryEntry( 678, pSock.language )); // That does not have a lock.
			return;
		}
		var iType = ourObj.type;
		switch( iType )
		{
			case 1:		// Container
			case 12:	// Door
			case 63:	// Spawn Container
				pSock.SysMessage( GetDictionaryEntry( 932, pSock.language )); // That is not locked.
				break;
			case 8:		// Locked Container
			case 13:	// Locked Door
			case 64:	// Locked Spawn Container
				if( ourObj.more == 0 )
				{
					var minSkill = 0;
					var maxSkill = 1000;
					var isTrapped = ( ourObj.morez >> 24 );
					if( isTrapped == 1 )
					{
						minSkill = 500;
					}

					// Check if the lock has been setup with a difficulty in morey property
					if( ourObj.morey != 0 )
					{
						// morey part 2 is a flag that says whether lock can be magically unlocked or not
						minSkill = ourObj.GetMoreVar( "morey", 3 ); // Min skill requirement to unlock
						maxSkill = ourObj.GetMoreVar( "morey", 4 ); // Max skill possible to gain from this lock
					}

					if( pUser.CheckSkill( 24, minSkill, maxSkill ))
					{
						if( iType == 8 )
						{
							ourObj.type = 1;
						}
						else if( iType == 13 )
						{
							ourObj.type = 12;
						}
						else if( iType == 64 )
						{
							ourObj.type = 63;
						}
						ourObj.Refresh();
						pSock.SoundEffect( 0x01FF, false );
						pSock.SysMessage( GetDictionaryEntry( 935, pSock.language )); // You manage to pick the lock.
					}
					else
					{
						if( RandomNumber( 0, 1 ))
						{
							pSock.SoundEffect( 0x013A, false ); // or 0x004b ?
							pSock.SysMessage( GetDictionaryEntry( 933, pSock.language )); // You broke your lockpick!
							if( bItem.amount > 1 )
							{
								bItem.amount = ( bItem.amount - 1 );
							}
							else
							{
								bItem.Delete();
							}
						}
						else
						{
							pSock.SysMessage( GetDictionaryEntry( 936, pSock.language )); // You fail to open the lock!
						}
					}
				}
				else
				{
					pSock.SysMessage( GetDictionaryEntry( 937, pSock.language )); // That cannot be unlocked without a key!
				}
				break;
			default:
				pSock.SysMessage( GetDictionaryEntry( 678, pSock.language )); // That does not have a lock
				break;
		}
	}
}

