function onUse( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && iUsed && iUsed.isItem )
	{
		if( pUser.isUsingPotion )
		{
			socket.SysMessage( GetDictionaryEntry( 430, socket.Language ) );
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
						socket.SysMessage( GetDictionaryEntry( 1608, socket.Language ) );
						break;
					case 2:
						DoTempEffect( 0, pUser, pUser, 6, RandomNumber( 11, 30 ), 0, 0 );
						socket.SysMessage( GetDictionaryEntry( 1609, socket.Language ) );
						break;
					default:
						break;
				}
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
								pUser.poison = 0;
							break;
						case 2:
							if( pPoison == 1 || ( pPoison == 2 && chanceToCure < 81 ) ||
							  ( pPoison == 3 && chanceToCure < 41 ) || ( pPoison == 4 && chanceToCure < 21 ) )
								pUser.poison = 0;
							break;
						case 3:
							if( pPoison == 1 || pPoison == 2  || ( pPoison == 3 && chanceToCure < 81 ) ||
							  ( pPoison == 4 && chanceToCure < 61 ) )
								pUser.poison = 0;
							break;
						default:
							break;
					}

					if( pUser.poison )
						socket.SysMessage( GetDictionaryEntry( 1345, socket.Language ) );
					else
					{
						pUser.StaticEffect( 0x373A, 0, 15 );
						pUser.SoundEffect( 0x01E0, true );
						socket.SysMessage( GetDictionaryEntry( 1346, socket.Language ) );
					}
				}
				else
					socket.SysMessage( GetDictionaryEntry( 1344, socket.Language ) );
				break;
			case 3:		// Explosion Potion
				var pRegion = pUser.region;
				if( pRegion.isGuarded )
				{
					socket.SysMessage( GetDictionaryEntry( 1347, socket.Language ) );
					return false;
				}
				else
				{
					socket.tempObj = iUsed;
					DoTempEffect( 0, pUser, pUser, 16, 0, 1, 3 );
					DoTempEffect( 0, pUser, pUser, 16, 0, 2, 2 );
					DoTempEffect( 0, pUser, pUser, 16, 0, 3, 1 );
					DoTempEffect( 0, pUser, iUsed, 17, 0, 4, 0 );
					socket.CustomTarget( 0, GetDictionaryEntry( 1348, socket.Language ) );
				}
				break;
			case 4:		// Heal Potion
				switch( iUsed.morez )
				{
					case 1:
						pUser.hitpoints = (pUser.hitpoints + 5 + RandomNumber( 1, 5 ));
						pUser.SysMessage( GetDictionaryEntry( 1349, socket.Language ) );
						break;
					case 2:
						pUser.hitpoints = (pUser.hitpoints + 15 + RandomNumber( 1, 10 ));
						pUser.SysMessage( GetDictionaryEntry( 1350, socket.Language ) );
						break;
					case 3:
						pUser.hitpoints = (pUser.hitpoints + 20 + RandomNumber( 1, 20 ));
						pUser.SysMessage( GetDictionaryEntry( 1351, socket.Language ) );
						break;
					default:
						break;
				}
				pUser.StaticEffect( 0x376A, 0x09, 0x06 );
				pUser.SoundEffect( 0x01E3, true );
				DoTempEffect( 0, pUser, pUser, 2, 0, 0, 0 );
				break;
			case 5:		// Night Sight Potion
				pUser.StaticEffect( 0x376A, 0x09, 0x06 );
				pUser.SoundEffect( 0x01E3, true );
				DoTempEffect( 0, pUser, pUser, 2, 0, 0, 0 );
				break;
			case 6:		// Poison Potion
				if( pUser.poison < iUsed.morez )
					pUser.poison = iUsed.morez;
				
				pUser.SetTimer( 5, (180*1000) );	// Poison Wearoff Timer
				pUser.SoundEffect( 0x0246, true );
				socket.SysMessage( GetDictionaryEntry( 1352, socket.Language ) );
				break;
			case 7:		// Refresh Potion
				switch( iUsed.morez )
				{
					case 1:
						pUser.stamina = (pUser.stamina + 20 + RandomNumber( 1, 10 ));
						socket.SysMessage( GetDictionaryEntry( 1353, socket.Language ) );
						break;
					case 2:
						pUser.stamina = (pUser.stamina + 40 + RandomNumber( 1, 30 ));
						socket.SysMessage( GetDictionaryEntry( 1354, socket.Language ) );
						break;
					default:
						break;
				}
				pUser.StaticEffect( 0x376A, 0x09, 0x06 );
				pUser.SoundEffect( 0x01F2, true );
				break;
			case 8:		// Strength Potion
				switch( iUsed.morez )
				{
					case 1:
						DoTempEffect( 0, pUser, pUser, 8, (5 + RandomNumber( 1, 10 )), 0, 0 );
						socket.SysMessage( GetDictionaryEntry( 1355, socket.Language ) );
						break;
					case 2:
						DoTempEffect( 0, pUser, pUser, 8, (10 + RandomNumber( 1, 20 )), 0, 0 );
						socket.SysMessage( GetDictionaryEntry( 1356, socket.Language ) );
						break;
					default:
						break;
				}
				pUser.StaticEffect( 0x0373A, 0, 15 );
				pUser.SoundEffect( 0x01EE, true );
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
				break;
			default:
				break;
		}

		pUser.SoundEffect( 0x0030, true );
		if( pUser.id > 0x0189 && !pUser.isonhorse )
			pUser.DoAction( 0x22 );

		iUsed.Delete();

		var eBottle = CreateBlankItem( socket, pUser, 1, "#", 0x0F0E, 0, "ITEM", true );
		if( eBottle && eBottle.isItem )
			eBottle.decay = true;
	}
	return false;
}

function onCallback0( socket, ourObj )
{
	var mChar = socket.currentChar;
	var iUsed = socket.tempObj;
	if( mChar && mChar.isChar && iUsed && iUsed.isItem )
	{
		// We need a LineOfSight check
		var StrangeByte = socket.GetWord( 1 );
		if( StrangeByte == 0 && ourObj )
			iUsed.Teleport( ourObj )
		else
		{
			var x = socket.GetWord( 11 );
			var y = socket.GetWord( 13 );
			var z = socket.GetByte( 16 ) + GetTileHeight( socket.GetWord( 17 ) );
			iUsed.Teleport( x, y, z );
		}

		iUsed.movable = 2;
		DoMovingEffect( mChar, iUsed, 0x0F0D, 0x11, 0, false, 0, 0 );
	}
}
