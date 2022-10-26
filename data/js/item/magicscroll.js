function onUseChecked( pUser, iUsed ) // Handling for using a Magic Scroll
{
	var pSock = pUser.socket;
	if ( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}
	if( iUsed.movable == 3 )
	{
		// Key is locked down, make sure only owners, co-owners or friends can use it
		var iMulti = iUsed.multi;
		if( !ValidateObject( iMulti ) || ( !iMulti.IsOnOwnerList( pUser ) && !iMulti.IsOnFriendList( pUser )))
		{
			pSock.SysMessage( GetDictionaryEntry( 1032, pSock.language )); // That is not yours!
			return false;
		}
	}
	if( pSock && iUsed && iUsed.isItem )
	{
		var usedID = iUsed.id;
		if( usedID > 0x1F2C && usedID  < 0x1F6D )
		{
			var success = false;
			pSock.currentSpellType = 1; // spell from scroll
			if( usedID == 0x1F2D )	// Reactive Armor spell scrolls
			{
				success = pUser.CastSpell( 7 );
			}
			else if( usedID >= 0x1F2E && usedID <= 0x1F34 )  // first circle spell scrolls
			{
				success = pUser.CastSpell( usedID  - 0x1F2D );
			}
			else if( usedID >= 0x1F35 && usedID <= 0x1F6C )  // 2 to 8 circle spell scrolls
			{
				success = pUser.CastSpell( usedID - 0x1F2D + 1 );
			}

			if( success )
			{
				var iAmount = iUsed.amount;
				if( iAmount > 1 )
				{
					iUsed.amount = iAmount - 1;
				}
				else
				{
					iUsed.Delete();
				}
			}
		}
	}
	return false;
}
