function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && iUsed && iUsed.isItem )
	{
		//Check to see if it's locked down
		if( iUsed.movable == 2 || iUsed.movable == 3 )
		{
			socket.SysMessage( GetDictionaryEntry( 774, socket.language )); //That is locked down and you cannot use it
			return false;
		}

		var pHunger = pUser.hunger;
		if( pHunger < 6 )
		{
			pUser.SoundEffect(( 0x003A + RandomNumber( 0, 2 )), true );
			if( pHunger >= 0 )
			{
				socket.SysMessage( GetDictionaryEntry(( 408 + pHunger ), socket.language )); //You eat the food, but are still extremely hungry.
			}
			else
			{
				socket.SysMessage( GetDictionaryEntry( 415, socket.language )); //You are simply too full to eat any more!
			}

			var iPoison = iUsed.poison;
			if( iPoison && pUser.poison < iPoison )
			{
				pUser.SetPoisoned( iPoison, GetServerSetting( "POISONTIMER" ) * 1000 );
				socket.SysMessage( GetDictionaryEntry(( 416 + RandomNumber( 0, 2 )), socket.language )); //You feel disoriented and nauseous.
				pUser.SoundEffect( 0x0246, true );
			}

			// Don't eat the crockery!
			if( iUsed.id == 0x15F9 || iUsed.id ==  0x15FA || iUsed.id ==  0x15FB || iUsed.id ==  0x15FC ) // bowls of food
			{
				var emptyObject = CreateDFNItem( socket, pUser, "0x15F8", 1, "ITEM", true ); //empty wooden bowl
				pUser.hunger = ( pHunger + 2 );
				iUsed.Delete();
			}
			else if( iUsed.id == 0x15FE || iUsed.id == 0x15FF || iUsed.id ==  0x1600 || iUsed.id ==  0x1601 || iUsed.id ==  0x1602 ) // bowls of food
			{
				var emptyObject = CreateDFNItem( socket, pUser, "0x15FD", 1, "ITEM", true ); //empty pewter bowl
				pUser.hunger = ( pHunger + 2 );
				iUsed.Delete();
			}
			else if( iUsed.id == 0x1604 || iUsed.id == 0x1606 ) // large wooden bowls of food
			{
				var emptyObject = CreateDFNItem( socket, pUser, "0x1605", 1, "ITEM", true ); //empty wooden bowl
				pUser.hunger = ( pHunger + 2 );
				iUsed.Delete();
			}
			else if( iUsed.id == 0x0993 ) // fruit basket
			{
				var emptyObject = CreateDFNItem( socket, pUser, "0x09AC", 1, "ITEM", true ); //empty bushel
				pUser.hunger = ( pHunger + 2 );
				iUsed.Delete();
			}
			else if( iUsed.id == 0x09AF || iUsed.id == 0x09D8 || iUsed.id == 0x09D9 || iUsed.id == 0x09DB ) // plates of food
			{
				var emptyObject = CreateDFNItem( socket, pUser, "0x09DA", 1, "ITEM", true ); //dirty plate
				pUser.hunger = ( pHunger + 2 );
				iUsed.Delete();
			}
			else
			{
				if( iUsed.amount > 1 )
				{
					iUsed.amount = ( iUsed.amount - 1 );
				}
				else
				{
					iUsed.Delete();
				}
				pUser.hunger = ( pHunger + 1 );
			}
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 407, socket.language )); //You are simply too full to eat any more!
		}
	}
	return false;
}
