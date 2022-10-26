function SkillRegistration()
{
	RegisterSkill( 22, true );	// Provocation
}

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
			if( ValidateObject( pUser ))
			{
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

			// Are the two NPCs involved too far away from one another?
			if( !pAttacker.InRange( toAttack, 10 ))
			{
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
				var myInstrument = GetInstrument( pUser );
				if( ValidateObject( myInstrument ))
				{
					if( pUser.CheckSkill( 29, 0, 1000 ))	// Musicianship
					{
						PlayInstrument( pSock, myInstrument, true );
						if( pUser.CheckSkill( 22, pAttacker.skillToProv, 1200 ))	// Provocation
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
	if( ValidateObject( pUser.pack ))
	{
		for( var toCheck = pUser.pack.FirstItem(); !pUser.pack.FinishedItems(); toCheck = pUser.pack.NextItem())
		{
			if( ValidateObject( toCheck ))
			{
				switch( toCheck.id )
				{
					case 0x0E9C:
					case 0x0E9D:
					case 0x0E9E:
					case 0x0EB1:
					case 0x0EB2:
					case 0x0EB3:
					case 0x0EB4:
						return toCheck;
					default:
						break;
				}
			}
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
	charOne.attackFirst = attackFirst;
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
