function SkillRegistration()
{
	RegisterSkill( 22, true );	// Provocation
}

// Override to 0/1 as desired, by default only enabled for AoS and beyond
const isProvokeDifficultyBased = ( GetServerSetting( "CoreShardEra" ) == EraStringToNum ( "aos" ));
const useLoSCheckForProvocation = true;

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
	{
		var pRegion = pUser.region;
		if( pRegion )
		{
			if( pRegion.isSafeZone )
			{
				// Player is in a safe zone where no aggressive actions can be taken, disallow
				pSock.SysMessage( GetDictionaryEntry( 1799, pSock.language )); // Hostile actions are not permitted in this safe area.
			}
			else
			{
				pSock.CustomTarget( 0, GetDictionaryEntry( 861, pSock.language ), 1 ); // Whom do you wish to incite?
			}
		}
	}

	return true;
}

function onCallback0( pSock, ourObj )
{
	if( ValidateObject( ourObj ) && ourObj.isChar )
	{
		if( !ourObj.npc )
		{
			pSock.SysMessage( GetDictionaryEntry( 1442, pSock.language )); // You cannot provoke other players.
		}
		else if( ourObj.aitype == 17 || ourObj.aitype == 4 || !ourObj.vulnerable )
		{
			pSock.SysMessage( GetDictionaryEntry( 830, pSock.language )); // You cannot provoke such a person!
		}
		else
		{
			var ourObjRegion = ourObj.region;
			if( ourObjRegion )
			{
				if( ourObjRegion.isSafeZone )
				{
					// Target is in a safe zone where no aggressive actions can be taken, disallow
					pSock.SysMessage( GetDictionaryEntry( 1799, pSock.language )); // Hostile actions are not permitted in this safe area.
					return;
				}
			}

			// Can target even be provoked?
			if( ourObj.skillToProv >= 2000 )
			{
				pSock.SysMessage( GetDictionaryEntry( 9128, pSock.language )); // You have no chance of provoking that creature.
				return;
			}

			var pUser = pSock.currentChar;

			// Can player see the target?
			if( useLoSCheckForProvocation && !pUser.CanSee( ourObj ))
			{
				pSock.SysMessage( GetDictionaryEntry( 1646, pSock.language )); // You cannot see that
				return;
			}

			if( ValidateObject( pUser ))
			{
				// TODO: At some point, base range of all bard abilities was 8, with increase of 1 tile per 15 points of skill in the ability
				if( !pUser.InRange( ourObj, 10 ))
				{
					pSock.SysMessage( GetDictionaryEntry( 393, pSock.language )); // That is too far away.
					return;
				}
				var myInstrument = GetInstrument( pUser );
				if( ValidateObject( myInstrument ))
				{
					pSock.tempObj = ourObj;
					pSock.CustomTarget( 1, GetDictionaryEntry( 1443, pSock.language ), 1 ); // You play your music, inciting anger, and your target begins to look furious.  Whom do you wish it to attack?
					PlayInstrument( pSock, myInstrument, true );
				}
				else
				{
					pSock.SysMessage( "lastInstrument: " + pUser.GetTempTag( "lastInstrument" ));
					pSock.SysMessage( GetDictionaryEntry( 1438, pSock.language )); // You do not have an instrument to play on!
				}
			}
		}
	}
}

function onCallback1( pSock, toAttack )
{
	if( ValidateObject( toAttack ))
	{
		if( toAttack.aitype == 17 || toAttack.aitype == 4 || !toAttack.vulnerable )
		{
			pSock.SysMessage( GetDictionaryEntry( 1730, pSock.language )); // You cannot provoke such a person!
		}
		else
		{
			var pUser = pSock.currentChar;
			if( !ValidateObject( pUser ))
				return;

			var pAttacker = pSock.tempObj;
			if( !ValidateObject( pAttacker ))
				return;

			var initiateAttack = false;
			var willAttack;

			// Is player trying to provoke a creature to attack itself?
			if( pAttacker == toAttack )
			{
				pSock.SysMessage( GetDictionaryEntry( 1449, pSock.language )); // Silly bard! You can't get something to attack itself.
				return;
			}

			// TODO: At some point, base range of all bard abilities was 8, with increase of 1 tile per 15 points of skill in the ability
			if( !pAttacker.InRange( toAttack, 10 ))
			{
				// The two NPCs involved are too far away from one another
				pSock.SysMessage( GetDictionaryEntry( 393, pSock.language )); // That is too far away.
				return;
			}

			// Does the target character have a immuneToProvocation tag?
			if( toAttack.GetTag( "provImmune" ))
			{
				pSock.SysMessage( GetDictionaryEntry( 9128, pSock.language )); // You have no chance of provoking that creature.

				// 50% chance of nothing happening, or creature attacking player instead
				if( RandomNumber( 1, 100 ) >= 50 )
					return;

				willAttack = pUser;
				initiateAttack = true;
			}
			else
			{
				// If provoker is a Young player, and target is a player, or a player's pet - disallow
				if( GetServerSetting( "YoungPlayerSystem" ))
				{
					if( pUser.account.isYoung && ( !toAttack.npc || ( ValidateObject( toAttack.owner ) && !toAttack.owner.npc )))
					{
						pSock.SysMessage( GetDictionaryEntry( 18736, pSock.language )); // Because of your young status in Britannia you cannot provoke the beast onto another player yet.
						return;
					}
					if(( !toAttack.npc && toAttack.account.isYoung ) || ( toAttack.npc && ValidateObject( toAttack.owner ) && !toAttack.owner.npc && toAttack.owner.account.isYoung ))
					{
						// Disallow provoking creatures onto Young players, or their pets
						pSock.SysMessage( GetDictionaryEntry( 18737, pSock.language )); // You cannot provoke the beast onto this player.
						return;
					}
				}

				// Check for Facet Ruleset
				if( DoesEventExist( 2507, "FacetRuleBardProvoke" ))
				{
					if( !TriggerEvent( 2507, "FacetRuleBardProvoke", pUser, toAttack ))
					{
						return;
					}
				}

				// Ensure target can see secondary target
				if( useLoSCheckForProvocation && !pAttacker.CanSee( toAttack ))
				{
					// No line of sight
					pAttacker.TextMessage( GetDictionaryEntry( 1669, pSock.language ), false, 0x3b2, 0, pUser.serial ); // Cannot see target!
					return;
				}

				var myInstrument = GetInstrument( pUser );
				if( ValidateObject( myInstrument ))
				{
					if( pUser.CheckSkill( 29, 0, pUser.skillCaps.musicianship ))	// Musicianship
					{
						PlayInstrument( pSock, myInstrument, true );

						var minDifficulty = pAttacker.skillToProv;
						var maxDifficulty = pUser.skillCaps.provocation;
						if( isProvokeDifficultyBased )
						{
							// (See setting at top of file)
							// In Publish 16 (considered AoS in UOX3) and later, difficulty to provoke is based on average of attacker & target
							// Additionally, there's a window of -25.0 to +25.0 where player has chance of success/can still gain skill
							minDifficulty = (( pAttacker.skillToProv + toAttack.skillToProv ) / 2 ) - 250;
							maxDifficulty = minDifficulty + 250;
						}

						// TODO: At some point, musicianship skill started giving 1% success bonus per point of musicianship over 100.0
						// for attempts at Discordance, Targeted Peacemaking (?) and Provocation

						if( pUser.CheckSkill( 22, minDifficulty, maxDifficulty ))	// Provocation
						{
							willAttack = toAttack;
							if( toAttack.innocent )
							{
								pUser.criminal = true;
							}
							pSock.SysMessage( GetDictionaryEntry( 1450, pSock.language )); // Your music succeeds as you start a fight.
						}
						else
						{
							pSock.SysMessage( GetDictionaryEntry( 1451, pSock.language )); // Your music fails to incite enough anger.
							willAttack = pUser;
						}
						initiateAttack = true;
					}
					else
					{
						PlayInstrument( pSock, myInstrument, false );
						pSock.SysMessage( GetDictionaryEntry( 1452, pSock.language )); // You play rather poorly and to no effect.
					}
				}
				else
				{
					pSock.SysMessage( "lastInstrument: " + pUser.GetTempTag( "lastInstrument" ));
					pSock.SysMessage( GetDictionaryEntry( 1438, pSock.language )); // You do not have an instrument to play on!
				}
			}

			if( initiateAttack )
			{
				// Provoke action succeeded - or failed spectacularly. Make combat happen!
				BeginAttack( pAttacker, willAttack, true );
				BeginAttack( willAttack, pAttacker, false );
				pAttacker.EmoteMessage( "You see " + pAttacker.name + " attacking " + willAttack.name + "!" );
			}
		}
	}
	pSock.tempObj = null;
}

function GetInstrument( pUser )
{
	// Fetch last instrument player played (set in musicianship.js)
	var lastInstrument = CalcItemFromSer( parseInt( pUser.GetTempTag( "lastInstrument" )));
	if( ValidateObject( lastInstrument ))
	{
		if( ValidateObject( lastInstrument.container ))
		{
			// Instrument is in a container. This should only work if item is inside player pack somewhere
			var rootCont = FindRootContainer( lastInstrument, 0 );
			if( ValidateObject( rootCont ) && rootCont == pUser.pack )
			{
				// Found instrument in player's pack somewhere!
				return lastInstrument;
			}
		}
		else if( pUser.InRange( lastInstrument, 3 ))
		{
			// Player is within range of the instrument!
			return lastInstrument;
		}
	}

	return null;
}

function PlayInstrument( pSock, myInstrument, wellPlayed )
{
	var soundID;
	switch( myInstrument.id )
	{
		case 0x0E9C:	// Drum
			if( wellPlayed )
			{
				soundID = 0x0038;
			}
			else
			{
				soundID = 0x0039;
			}
			break;
		case 0x0E9D:	// Tambourine
		case 0x0E9E:
			if( wellPlayed )
			{
				soundID = 0x0052;
			}
			else
			{
				soundID = 0x0053;
			}
			break;
		case 0x0EB1:	// Standing Harp
			if( wellPlayed )
			{
				soundID = 0x0043;
			}
			else
			{
				soundID = 0x0044;
			}
			break;
		case 0x0EB2:	// Harp
			if( wellPlayed )
			{
				soundID = 0x0045;
			}
			else
			{
				soundID = 0x0046;
			}
			break;
		case 0x0EB3:	// Lute
		case 0x0EB4:
			if( wellPlayed )
			{
				soundID = 0x004C;
			}
			else
			{
				soundID = 0x004D;
			}
			break;
		default:
			return;
	}
	pSock.SoundEffect( soundID, true );
}

function BeginAttack( charOne, charTwo, attackFirst )
{
	if( attackFirst && !charTwo.CheckAggressorFlag( charOne ))
	{
		charOne.AddAggressorFlag( charTwo );
	}
	charOne.attacker = charTwo;
	charOne.target = charTwo;

	if( charOne.visible == 1 || charOne.visible == 2 )
	{
		charOne.visible = 0;
		charOne.stealth = -1;
	}

	if( charOne.isMeditating )
	{
		charOne.isMeditating = false;
	}

	if( charOne.npc )
	{
		if( !charOne.atWar )
		{
			charOne.atWar = true;
		}
	}
}
