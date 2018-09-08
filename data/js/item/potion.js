function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && iUsed && iUsed.isItem )
	{
		if( pUser.isUsingPotion )
		{
			socket.SysMessage( GetDictionaryEntry( 430, socket.Language ) ); //You must wait a while before using another potion.
			return false;
		}

		//Check to see if it's locked down
		if( iUsed.movable == 2 || iUsed.movable == 3 )
		{
			socket.SysMessage( GetDictionaryEntry( 774, socket.Language ) ); //That is locked down and you cannot use it
			return false;
		}
		switch( iUsed.morey )
		{
			case 1:		// Agility Potion
				pUser.StaticEffect( 0x373A, 0, 15 );
				pUser.SoundEffect( 0x01E7, true );
				switch( iUsed.morez )
				{
					case 1:
						DoTempEffect( 0, pUser, pUser, 6, RandomNumber( 6, 15 ), 0, 0 );
						socket.SysMessage( GetDictionaryEntry( 1608, socket.Language ) ); //You feel more agile!
						break;
					case 2:
						DoTempEffect( 0, pUser, pUser, 6, RandomNumber( 11, 30 ), 0, 0 );
						socket.SysMessage( GetDictionaryEntry( 1609, socket.Language ) ); //You feel much more agile!
						break;
					default:
						break;
				}
				pUser.isUsingPotion = true;
				DoTempEffect( 0, pUser, pUser, 26, 0, 0, 0 ); //Disallow immediately using another potion				
				break;
			case 2:		// Cure Potion
				var pPoison = pUser.poison;
				if( pPoison > 0 )
				{
					var chanceToCure = RandomNumber( 1, 100 );
					switch( iUsed.morez )
					{
						case 1:
							if( ( pPoison == 1 && chanceToCure < 81 ) || ( pPoison == 2 && chanceToCure < 41 ) ||
							    ( pPoison == 3 && chanceToCure < 21 ) || ( pPoison == 4 && chanceToCure < 6 ) )
								pUser.SetPoisoned(0,0);
							break;
						case 2:
							if( pPoison == 1 || ( pPoison == 2 && chanceToCure < 81 ) ||
							  ( pPoison == 3 && chanceToCure < 41 ) || ( pPoison == 4 && chanceToCure < 21 ) )
								pUser.SetPoisoned(0,0);
							break;
						case 3:
							if( pPoison == 1 || pPoison == 2  || ( pPoison == 3 && chanceToCure < 81 ) ||
							  ( pPoison == 4 && chanceToCure < 61 ) )
								pUser.SetPoisoned(0,0);
							break;
						default:
							break;
					}

					if( pUser.poison )
						socket.SysMessage( GetDictionaryEntry( 1345, socket.Language ) ); //The potion was not able to cure this poison.
					else
					{
						pUser.StaticEffect( 0x373A, 0, 15 );
						pUser.SoundEffect( 0x01E0, true );
						socket.SysMessage( GetDictionaryEntry( 1346, socket.Language ) ); //The poison was cured!
					}
				}
				else
					socket.SysMessage( GetDictionaryEntry( 1344, socket.Language ) ); //The potion had no effect.
				pUser.isUsingPotion = true;
				DoTempEffect( 0, pUser, pUser, 26, 0, 0, 0 ); //Disallow immediately using another potion
				break;
			case 3:		// Explosion Potion
				var pRegion = pUser.region;
				if( pRegion.isGuarded )
				{
					socket.SysMessage( GetDictionaryEntry( 1347, socket.Language ) ); //You can't use that in town!
					return false;
				}
				else
				{
					socket.tempObj = iUsed;
					DoTempEffect( 0, pUser, pUser, 16, 0, 1, 3 );
					DoTempEffect( 0, pUser, pUser, 16, 0, 2, 2 );
					DoTempEffect( 0, pUser, pUser, 16, 0, 3, 1 );
					DoTempEffect( 1, pUser, iUsed, 17, 0, 4, 0 );
					socket.CustomTarget( 0, GetDictionaryEntry( 1348, socket.Language ) ); //Now would be a good time to throw it!
				}
				pUser.isUsingPotion = true;
				DoTempEffect( 0, pUser, pUser, 26, 0, 0, 0 ); //Disallow immediately using another potion
				break;
			case 4:		// Heal Potion
				switch( iUsed.morez )
				{
					case 1:
						pUser.health = (pUser.health + 5 + RandomNumber( 1, 5 ));
						pUser.SysMessage( GetDictionaryEntry( 1349, socket.Language ) ); //You feel a little better!
						break;
					case 2:
						pUser.health = (pUser.health + 15 + RandomNumber( 1, 10 ));
						pUser.SysMessage( GetDictionaryEntry( 1350, socket.Language ) ); //You feel better!
						break;
					case 3:
						pUser.health = (pUser.health + 20 + RandomNumber( 1, 20 ));
						pUser.SysMessage( GetDictionaryEntry( 1351, socket.Language ) ); //You feel much better!
						break;
					default:
						break;
				}
				pUser.StaticEffect( 0x376A, 0x09, 0x06 );
				pUser.SoundEffect( 0x01E3, true );
				pUser.isUsingPotion = true;
				DoTempEffect( 0, pUser, pUser, 26, 0, 0, 0 ); //Disallow immediately using another potion
				break;
			case 5:		// Night Sight Potion
				pUser.StaticEffect( 0x376A, 0x09, 0x06 );
				pUser.SoundEffect( 0x01E3, true );
				DoTempEffect( 0, pUser, pUser, 2, 0, 0, 0 );
				pUser.isUsingPotion = true;
				DoTempEffect( 0, pUser, pUser, 26, 0, 0, 0 ); //Disallow immediately using another potion				
				break;
			case 6:		// Poison Potion
				if( pUser.poison < iUsed.morez )
					pUser.SetPoisoned( iUsed.morez, 180*1000 );
				
				pUser.SoundEffect( 0x0246, true );
				socket.SysMessage( GetDictionaryEntry( 1352, socket.Language ) ); //You poisoned yourself! *sigh*
				pUser.isUsingPotion = true;
				DoTempEffect( 0, pUser, pUser, 26, 0, 0, 0 ); //Disallow immediately using another potion				
				break;
			case 7:		// Refresh Potion
				switch( iUsed.morez )
				{
					case 1:
						pUser.stamina = (pUser.stamina + 20 + RandomNumber( 1, 10 ));
						socket.SysMessage( GetDictionaryEntry( 1353, socket.Language ) ); //You feel more energetic!
						break;
					case 2:
						pUser.stamina = (pUser.stamina + 40 + RandomNumber( 1, 30 ));
						socket.SysMessage( GetDictionaryEntry( 1354, socket.Language ) ); //You feel much more energetic!
						break;
					default:
						break;
				}
				pUser.StaticEffect( 0x376A, 0x09, 0x06 );
				pUser.SoundEffect( 0x01F2, true );
				pUser.isUsingPotion = true;
				DoTempEffect( 0, pUser, pUser, 26, 0, 0, 0 ); //Disallow immediately using another potion			
				break;
			case 8:		// Strength Potion
				switch( iUsed.morez )
				{
					case 1:
						DoTempEffect( 0, pUser, pUser, 8, (5 + RandomNumber( 1, 10 )), 0, 0 );
						socket.SysMessage( GetDictionaryEntry( 1355, socket.Language ) ); //You feel stronger!
						break;
					case 2:
						DoTempEffect( 0, pUser, pUser, 8, (10 + RandomNumber( 1, 20 )), 0, 0 );
						socket.SysMessage( GetDictionaryEntry( 1356, socket.Language ) ); //You feel much stronger!
						break;
					default:
						break;
				}
				pUser.StaticEffect( 0x0373A, 0, 15 );
				pUser.SoundEffect( 0x01EE, true );
				pUser.isUsingPotion = true;
				DoTempEffect( 0, pUser, pUser, 26, 0, 0, 0 ); //Disallow immediately using another potion
				break;
			case 9:		// Mana Potion
				switch( iUsed.morez )
				{
					case 1:
						pUser.mana = (pUser.mana + 10 + (iUsed.morex / 100));
						break;
					case 2:
						pUser.mana = (pUser.mana + 20 + (iUsed.morex / 50));
						break;
					default:
						break;
				}
				pUser.StaticEffect( 0x376A, 0x09, 0x06 );
				pUser.SoundEffect( 0x01E7, true );
				pUser.isUsingPotion = true;
				DoTempEffect( 0, pUser, pUser, 26, 0, 0, 0 ); //Disallow immediately using another potion
				break;
			default:
				break;
		}

		if( iUsed.morey != 3 )
		{
			pUser.SoundEffect( 0x0030, true );
			if( pUser.id > 0x0189 && !pUser.isonhorse )
				pUser.DoAction( 0x22 );

			iUsed.Delete();

			var eBottle = CreateDFNItem( pUser.socket, pUser, "0x0F0E", 1, "ITEM", true );
			if( eBottle && eBottle.isItem )
				eBottle.decay = true;
		}
	}
	return false;
}

function onCallback0( socket, ourObj )
{
	var mChar = socket.currentChar;
	var iUsed = socket.tempObj;
	if( mChar && mChar.isChar && iUsed && iUsed.isItem )
	{
		iUsed.container = null;
		// We need a LineOfSight check
		var StrangeByte = socket.GetWord( 1 );
		if( StrangeByte == 0 && ourObj )
			iUsed.Teleport( ourObj );
		else
		{
			var x = socket.GetWord( 11 );
			var y = socket.GetWord( 13 );
			var z = socket.GetSByte( 16 ) + GetTileHeight( socket.GetWord( 17 ) );
			iUsed.Teleport( x, y, z );
		}

		iUsed.movable = 2;
		DoMovingEffect( mChar, iUsed, 0x0F0D, 0x11, 0, false, 0, 0 );
	}
}
