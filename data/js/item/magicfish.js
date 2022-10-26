function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if ( socket && iUsed && iUsed.isItem )
	{
		if ( pUser.isUsingPotion )
		{
			socket.SysMessage( GetDictionaryEntry( 9301, socket.language ) ); // You are already under a similar effect.
			return false;
		}

		//Check to see if it's locked down
		if ( iUsed.movable == 2 || iUsed.movable == 3 )
		{
			socket.SysMessage( GetDictionaryEntry( 774, socket.language ) ); // That is locked down and you cannot use it
			return false;
		}
		switch ( iUsed.morey )
		{
			case 1:	// Prized Fish
				pUser.StaticEffect( 0x375A, 0, 15 );
				pUser.SoundEffect( 0x1E7, true );
				DoTempEffect( 0, pUser, pUser, 6, RandomNumber( 6, 15 ), 0, 0 );
				DoTempEffect( 0, pUser, pUser, 8, ( 10 + RandomNumber( 1, 10 )), 0, 0 )
				pUser.tempint = ( pUser.tempint + 5 );
				pUser.isUsingPotion = true;
				pUser.EmoteMessage( GetDictionaryEntry( 9302, socket.language ) ); // You swallow the fish whole!
				DoTempEffect( 0, pUser, pUser, 26, 0, 0, 0 ); // Disallow immediately using another potion
				break;
			case 2:	// wondrous fish
				pUser.StaticEffect( 0x375A, 0, 15 );
				pUser.SoundEffect( 0x1E7, true );
				DoTempEffect( 0, pUser, pUser, 6, RandomNumber( 20, 30 ), 0, 0 );
				pUser.EmoteMessage( GetDictionaryEntry( 9302, socket.language ) ); // You swallow the fish whole!
				pUser.tempdex = ( pUser.tempdex + 5 );
				pUser.isUsingPotion = true;
				DoTempEffect( 0, pUser, pUser, 26, 0, 0, 0 ); // Disallow immediately using another potion
				break;
			case 3:	// truly rare fish
				pUser.StaticEffect( 0x375A, 0, 15 );
				pUser.SoundEffect( 0x1E7, true );
				DoTempEffect( 0, pUser, pUser, 6, RandomNumber( 20, 30 ), 0, 0 );
				pUser.EmoteMessage( GetDictionaryEntry( 9302, socket.language ) ); // You swallow the fish whole!
				pUser.tempstr = ( pUser.tempstr + 5 );
				pUser.isUsingPotion = true;
				DoTempEffect( 0, pUser, pUser, 26, 0, 0, 0 ); // Disallow immediately using another potion
				break;
			case 4:	// highly peculiar fish
				pUser.StaticEffect( 0x375A, 0, 15 );
				pUser.SoundEffect( 0x1E7, true );
				DoTempEffect( 0, pUser, pUser, 6, RandomNumber( 20, 30 ), 0, 0 );
				pUser.EmoteMessage( GetDictionaryEntry( 9302, socket.language ) ); // You swallow the fish whole!
				pUser.stamina = ( pUser.stamina + 10 );
				pUser.isUsingPotion = true;
				DoTempEffect( 0, pUser, pUser, 26, 0, 0, 0 ); // Disallow immediately using another potion
				break;
			default:
				break;
		}

		if ( iUsed.morey != 3 )
		{
			pUser.SoundEffect( 0x0030, true );
			if ( pUser.id > 0x0189 && !pUser.isonhorse )
			{
				pUser.DoAction( 0x22 );
			}

			iUsed.Delete();
		}
	}
	return false;
}
