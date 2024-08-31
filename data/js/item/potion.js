// If enabled in INI setting, explosion potions get bonus damage from alchemy skill
// using this formula: ( attacker's alchemy skill / 10 ) / alchemyBonusModifier
// Example: (1000 skillpoints / 10 ) / 5 = 20 bonus damage
const alchemyBonusEnabled = GetServerSetting( "AlchemyBonusEnabled" );
const alchemyBonusModifier = parseInt( GetServerSetting( "AlchemyBonusModifier" ));

// Other settings
const randomizePotionCountdown = false; // If true, add/remove +1/-1 seconds to explosion potion countdowns

function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;

	if( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}

	if( socket && iUsed && iUsed.isItem )
	{
		if( pUser.isUsingPotion )
		{
			socket.SysMessage( GetDictionaryEntry( 430, socket.language )); //You must wait a while before using another potion.
			return false;
		}

		//Check to see if it's locked down
		if( iUsed.movable == 3 )
		{
			socket.SysMessage( GetDictionaryEntry( 774, socket.language )); //That is locked down and you cannot use it
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
						socket.SysMessage( GetDictionaryEntry( 1608, socket.language )); //You feel more agile!
						break;
					case 2:
						DoTempEffect( 0, pUser, pUser, 6, RandomNumber( 11, 30 ), 0, 0 );
						socket.SysMessage( GetDictionaryEntry( 1609, socket.language )); //You feel much more agile!
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
							    ( pPoison == 3 && chanceToCure < 21 ) || ( pPoison == 4 && chanceToCure < 6 ))
							{
								pUser.SetPoisoned( 0, 0 );
							}
							break;
						case 2:
							if( pPoison == 1 || ( pPoison == 2 && chanceToCure < 81 ) ||
							  ( pPoison == 3 && chanceToCure < 41 ) || ( pPoison == 4 && chanceToCure < 21 ))
							{
								pUser.SetPoisoned( 0, 0 );
							}
							break;
						case 3:
							if( pPoison == 1 || pPoison == 2  || ( pPoison == 3 && chanceToCure < 81 ) ||
							  ( pPoison == 4 && chanceToCure < 61 ))
							{
								pUser.SetPoisoned( 0, 0 );
							}
							break;
						default:
							break;
					}

					if( pUser.poison )
					{
						socket.SysMessage( GetDictionaryEntry( 1345, socket.language )); //The potion was not able to cure this poison.
					}
					else
					{
						pUser.StaticEffect( 0x373A, 0, 15 );
						pUser.SoundEffect( 0x01E0, true );
						socket.SysMessage( GetDictionaryEntry( 1346, socket.language )); //The poison was cured!
					}
				}
				else
				{
					socket.SysMessage( GetDictionaryEntry( 1344, socket.language )); //The potion had no effect.
				}
				pUser.isUsingPotion = true;
				DoTempEffect( 0, pUser, pUser, 26, 0, 0, 0 ); //Disallow immediately using another potion
				break;
			case 3:		// Explosion Potion
				var pRegion = pUser.region;
				if( pRegion.isSafeZone )
				{
					socket.SysMessage( GetDictionaryEntry( 1799, socket.language )); // Hostile actions are not permitted in this safe area.
					return false;
				}
				else if( pRegion.isGuarded )
				{
					socket.SysMessage( GetDictionaryEntry( 1347, socket.language )); //You can't use that in town!
					return false;
				}
				else
				{
					if( iUsed.amount > 1 )
					{
						// Stack of potions
						var explosionPotion = iUsed.Dupe( socket );
						explosionPotion.amount = 1;
						if( ValidateObject( explosionPotion ))
						{
							iUsed.amount--;
							iUsed = explosionPotion;
						}
					}

					// Store potion on socket and player serial on potion, for later!
					socket.tempObj = iUsed;
					iUsed.more = pUser.serial;

					// Set radius of explosion
					iUsed.morex = pUser.skills.alchemy / 250;

					// Randomize countdown length, if enabled
					if( randomizePotionCountdown )
					{
						iUsed.speed = RandomNumber( iUsed.speed - 1, iUsed.speed + 1 );
					}

					// Item's speed forms the basis of the countdownTime
					var countdownTime = iUsed.speed * 1000;

					// Start the initial timer that shows the first number over the character/object's head
		  			iUsed.StartTimer( 200, 1, true );

		  			// Start timers with IDs from 2, and count until we reach item's speed + 1
					var iCount = 2;
					for( iCount = 2; iCount < ( iUsed.speed + 2 ); iCount++ )
					{
						iUsed.StartTimer(( iCount - 1 ) * 1000, iCount, true );
					}

					// Disallow immediately using other potions
					pUser.isUsingPotion = true;
					DoTempEffect( 0, pUser, pUser, 26, 0, 0, 0 );
					socket.CustomTarget( 0, GetDictionaryEntry( 1348, socket.language )); //Now would be a good time to throw it!
				}
				break;
			case 4:		// Heal Potion
				if( pUser.health < pUser.maxhp )
				{
					if( pUser.poison > 0 )
					{
						pUser.SysMessage( GetDictionaryEntry( 9058, socket.language )); // You can not heal yourself in your current state.
						return;
					}

					switch( iUsed.morez )
					{
						case 1:
							pUser.health = ( pUser.health + RandomNumber( 3, 10 ));
							pUser.SysMessage( GetDictionaryEntry( 1349, socket.language )); // You feel a little better!
							break;
						case 2:
							pUser.health = ( pUser.health + RandomNumber( 6, 20 ));
							pUser.SysMessage( GetDictionaryEntry( 1350, socket.language )); // You feel better!
							break;
						case 3:
							pUser.health = ( pUser.health + RandomNumber( 9, 30 ));
							pUser.SysMessage( GetDictionaryEntry( 1351, socket.language )); // You feel much better!
							break;
						default:
							break;
					}
					pUser.StaticEffect( 0x376A, 0x09, 0x06 );
					pUser.SoundEffect( 0x01E3, true );
					pUser.isUsingPotion = true;
					DoTempEffect( 0, pUser, pUser, 26, 0, 0, 0 ); // Disallow immediately using another potion
				}
				else
				{
					pUser.SysMessage( GetDictionaryEntry( 9059, socket.language )); // You decide against drinking this potion, as you are already at full health.
				}
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
				{
					pUser.SetPoisoned( iUsed.morez, 180 * 1000 );
				}

				pUser.SoundEffect( 0x0246, true );
				socket.SysMessage( GetDictionaryEntry( 1352, socket.language )); //You poisoned yourself! *sigh*
				pUser.isUsingPotion = true;
				DoTempEffect( 0, pUser, pUser, 26, 0, 0, 0 ); //Disallow immediately using another potion
				break;
			case 7:		// Refresh Potion
				switch( iUsed.morez )
				{
					case 1:
						pUser.stamina = (pUser.stamina + 20 + RandomNumber( 1, 10 ));
						socket.SysMessage( GetDictionaryEntry( 1353, socket.language )); //You feel more energetic!
						break;
					case 2:
						pUser.stamina = (pUser.stamina + 40 + RandomNumber( 1, 30 ));
						socket.SysMessage( GetDictionaryEntry( 1354, socket.language )); //You feel much more energetic!
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
						DoTempEffect( 0, pUser, pUser, 8, ( 5 + RandomNumber( 1, 10 )), 0, 0 );
						socket.SysMessage( GetDictionaryEntry( 1355, socket.language )); //You feel stronger!
						break;
					case 2:
						DoTempEffect( 0, pUser, pUser, 8, ( 10 + RandomNumber( 1, 20 )), 0, 0 );
						socket.SysMessage( GetDictionaryEntry( 1356, socket.language )); //You feel much stronger!
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
						pUser.mana = ( pUser.mana + 10 + ( iUsed.morex / 100 ));
						break;
					case 2:
						pUser.mana = ( pUser.mana + 20 + ( iUsed.morex / 50 ));
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

		// For potions other than explosion potions, consume potion upon use
		if( iUsed.morey != 3 )
		{
			pUser.SoundEffect( 0x0030, true );
			if( pUser.id > 0x0189 && !pUser.isonhorse )
			{
				pUser.DoAction( 0x22 );
			}

			if( iUsed.amount > 1 )
			{
				iUsed.amount--;
			}
			else
			{
				iUsed.Delete();
			}

			// Create empty bottle
			var eBottle = CreateDFNItem( socket, pUser, "0x0F0E", 1, "ITEM", true );
			if( eBottle && eBottle.isItem )
			{
				eBottle.decay = true;
			}
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
		var StrangeByte = socket.GetWord( 1 );
		if( StrangeByte == 0 && ourObj )
		{
			// We need a LineOfSight check
			if( mChar.CanSee( ourObj.x, ourObj.y, ourObj.z ))
			{
				iUsed.container = null;
				iUsed.Teleport( ourObj );
			}
			else
			{
				socket.SysMessage( GetDictionaryEntry( 1646, socket.language )); // You cannot see that

				// Give player another chance to throw the potion before it blows up in their face
				socket.CustomTarget( 0, GetDictionaryEntry( 1348, socket.language )); //Now would be a good time to throw it!
				return;
			}
		}
		else
		{
			var x = socket.GetWord( 11 );
			var y = socket.GetWord( 13 );
			var z = socket.GetSByte( 16 );
			var StrangeByte = socket.GetWord(1);

			// If connected with a client lower than v7.0.9, manually add height of targeted tile
			if ((StrangeByte == 0 && ourObj.isItem) || (socket.clientMajorVer <= 7 && socket.clientSubVer < 9))
			{
				z += GetTileHeight( socket.GetWord( 17 ));
			}

			// We need a LineOfSight check
			if( mChar.CanSee( x, y, z ))
			{
				iUsed.container = null;
				iUsed.Teleport( x, y, z );
			}
			else
			{
				socket.SysMessage( GetDictionaryEntry( 1646, socket.language )); // You cannot see that

				// Give player another chance to throw the potion before it blows up in their face
				socket.CustomTarget( 0, GetDictionaryEntry( 1348, socket.language )); //Now would be a good time to throw it!
				return;
			}
		}

		iUsed.movable = 2;

		// Play moving effect of potion being thrown to potion's target location
		DoMovingEffect( mChar, iUsed, 0x0F0D, 5, 0, false, 0, 0 );
	}
}

function onTimer( timerObj, timerID )
{
	var countdown = 0;
	countdown = ( timerObj.speed + 1 ) - timerID;
	var packOwner = GetPackOwner( timerObj, 0 );

	// If timerID equals object speed + 1, time to explode!
	if( timerID == timerObj.speed + 1 )
	{
		if( timerObj.container == null )
		{
		  	DoStaticEffect( timerObj.x, timerObj.y, timerObj.z, 0x36B0, 0x09, 0x0d, false );
			timerObj.SoundEffect( 0x0207, true );

		  	var explosionCounter = AreaCharacterFunction( "ApplyExplosionDamage", timerObj, 4 );
		}
		else
		{
			// Oops! Player is still holding the potion... explode player for damage!
			if( packOwner != null && packOwner.isChar )
			{
				packOwner.StaticEffect( 0x36b0, 0x09, 0x0d );
				packOwner.SoundEffect( 0x0207, true );

				// Apply alchemy bonus to explosion damage, if it's enabled and higher than 0 (both set in INI)
				var alchemyBonus = 0;
				if( alchemyBonusEnabled && alchemyBonusModifier > 0 )
				{
					alchemyBonus = Math.round(( packOwner.skills.alchemy / 10 ) / alchemyBonusModifier );
				}

				// Deal damage to player holding the potion
				packOwner.Damage( RandomNumber( timerObj.lodamage, timerObj.hidamage ) + alchemyBonus, 5 );
			}
		}
	  	timerObj.Delete();
		return;
	}

	// As long as timerID is lower than object speed, display a countdown message
	if( timerID <= timerObj.speed )
	{
		if( timerObj.container == null )
		{
			// Player threw potion, let's show countdown above potion - to everyone nearby
			timerObj.TextMessage( countdown.toString(), true, 0x0026, -1, -1, -1, 6 );
		}
		else
		{
			// Player has not thrown potion yet - show timer above character's head, but only to player
			if( packOwner != null && packOwner.isChar )
			{
				packOwner.TextMessage( countdown.toString(), false, 0x0026, -1, -1, -1, 6 );
			}
		}
	}
}

function ApplyExplosionDamage( timerObj, targetChar )
{
	var sourceChar = CalcCharFromSer( timerObj.more );
	if( ValidateObject( sourceChar ))
	{
		// Don't damage offline players
		if( !targetChar.npc && !targetChar.online )
			return;

		// Don't damage Young players
		if( GetServerSetting( "YoungPlayerSystem" ))
		{
			// Don't damage a Young player, or a Young player's pets
			if(( !targetChar.npc && targetChar.account.isYoung )
				|| ( targetChar.npc && ValidateObject( targetChar.owner ) && !targetChar.owner.npc && targetChar.owner.account.isYoung ))
			{
				return;
			}

			// Don't let Young players damage other players, or the pets of other players
			if(( !sourceChar.npc && sourceChar.account.isYoung && !targetChar.npc )
				|| ( targetChar.npc && ValidateObject( targetChar.owner ) && !targetChar.owner.npc ))
			{
				return;
			}
		}

		// Check for Facet Ruleset
		if( !TriggerEvent( 2507, "FacetRuleExplosionDamage", sourceChar, targetChar ))
		{
			return;
		}

		// Ignore targets that are in safe zones
		var targetRegion = targetChar.region;
		if( targetRegion.isSafeZone )
		{
			sourceChar.SysMessage( GetDictionaryEntry( 2756, sourceChar.socket.language )); // Your target is in a safe zone!
			return;
		}

		// Don't allow a Z difference greater than 5
		if( Math.abs( targetChar.z - timerObj.z) > 5 )
			return;

		// Ignore characters that are not in Line of Sight of the potion
		if( !targetChar.CanSee( timerObj ))
			return;

		// Apply alchemy bonus to explosion damage, if it's enabled and higher than 0 (both set in INI)
		var alchemyBonus = 0;
		if( alchemyBonusEnabled && alchemyBonusModifier > 0 )
		{
			alchemyBonus = Math.round(( sourceChar.skills.alchemy / 10 ) / alchemyBonusModifier );
		}

		// Deal damage, and do criminal check for source character!
		targetChar.Damage( RandomNumber( timerObj.lodamage, timerObj.hidamage ) + alchemyBonus, 5, sourceChar, true );

		// If target is an NPC, make them attack the person who threw the potion!
		if( targetChar.npc && targetChar.target == null && targetChar.atWar == false )
		{
			targetChar.target = sourceChar;
			targetChar.atWar = true;
			targetChar.attacker = sourceChar;
		}
	}
	else
	{
		// Source character not found - apply damage on general basis
		targetChar.Damage( RandomNumber( timerObj.lodamage, timerObj.hidamage ), 5 );
	}
}

// Show real name to those who have identified potion
function onNameRequest( iPotion, pUser )
{
	// Default name
	var nameString = iPotion.name;

	if( iPotion.name2 != "#" )
	{
		// Potion has a second name revealed only to people who've used taste identification on it
		var listOfTastersTemp = iPotion.GetTag( "listOfTasters" );
		if( listOfTastersTemp.length > 0 )
		{
			listOfTasters = listOfTastersTemp.split( "," ).map( Number );
			if( listOfTasters && listOfTasters.indexOf( pUser.serial ) != -1 )
			{
				// Show identified name of potion in tooltip
				nameString = iPotion.name2;
			}
		}
	}

	return nameString;
}

// Add tooltip details
function onTooltip( iPotion, pSocket )
{
	var pUser = null;
	if( pSocket != null )
	{
		pUser = pSocket.currentChar;
	}

	var tooltipText = "";
	var listOfTasters = new Array;
	if( iPotion.name2 != "#" )
	{
		// Check if player is on potion's list of players who have identified its taste
		var listOfTastersTemp = iPotion.GetTag( "listOfTasters" );
		if( listOfTastersTemp.length > 0 )
		{
			listOfTasters = listOfTastersTemp.split( "," ).map( Number );
			if( listOfTasters && listOfTasters.indexOf( pUser.serial ) == -1 )
			{
				// If potion has not been identified by player, add [Unidentified] tooltip entry
				tooltipText = GetDictionaryEntry( 9402, pSocket.language ); // [Unidentified]
			}
		}
	}

	return tooltipText;
}

function _restorecontext_() {}
