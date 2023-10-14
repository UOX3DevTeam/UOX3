function SkillRegistration()
{
	RegisterSkill( 48, true );	// Remove Trap
}

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
	{
		if( pUser.skills.lockpicking < 500 )
		{
			// You do not know enough about locks.  Become better at picking locks.
			pSock.SysMessage( GetDictionaryEntry( 2091, pSock.language ));
		}
		else if( pUser.skills.detectinghidden < 500 )
		{
			// You are not perceptive enough.  Become better at detect hidden.
			pSock.SysMessage( GetDictionaryEntry( 2092, pSock.language ));
		}
		else
		{
			// Which trap will you attempt to disarm?
			pSock.CustomTarget( 0, GetDictionaryEntry( 2093, pSock.language ));
		}
	}
	return true;
}

function onCallback0( pSock, ourObj )
{
	if( !ValidateObject( ourObj ) || !ourObj.isItem )
	{
		// You feel that such an action would be inappropriate
		pSock.SysMessage( GetDictionaryEntry( 2094, pSock.language ));
		return;
	}

	var pUser = pSock.currentChar;
	var iMoreZPart1 = ourObj.GetMoreVar( "morez", 1 ); // Trap set
	var iMoreZPart2 = ourObj.GetMoreVar( "morez", 2 ); // Trap Damage
	var iMoreZPart3 = ourObj.GetMoreVar( "morez", 3 ); // Minskill
	var iMoreZPart4 = ourObj.GetMoreVar( "morez", 4 ); // Maxskill
	var isInRange = pUser.InRange( ourObj, 3 );

	pSock.tempObj = null;
	if( !isInRange )
	{
		pSock.SysMessage( GetDictionaryEntry( 461, pSock.language )); // You are too far away.
		return;
	}

	if( !pUser.CanSee( ourObj ))
	{
		// You cannot see that
		pSock.SysMessage( GetDictionaryEntry( 1646, pSock.language) );
	}
	else if( iMoreZPart1 == 0 )
	{
		// That doesn't appear to be trapped
		pSock.SysMessage( GetDictionaryEntry( 2095, pSock.language ));
	}
	else
	{
		// Is player attempting to disarm a dungeon-trap (ex: spike trap)
		var isDungeonTrap = false;
		var scriptTriggers = ourObj.scriptTriggers;
		for( var i = 0; i < scriptTriggers.length; i++ )
		{
			if( scriptTriggers[i] == 2504 ) // dungeon_traps.js
			{
				isDungeonTrap = true;
				break;
			}
		}

		var skillCheckModifier = 0;
		var iGloves = pUser.FindItemLayer( 7 ); // hands layer
		if( ValidateObject( iGloves ))
		{
			skillCheckModifier = iGloves.weight;
		}

		if( pUser.skills.removetrap >= iMoreZPart4 || pUser.CheckSkill( 48, ( iMoreZPart3 + skillCheckModifier ), 1000 ))
		{
			// You successfully render the trap harmless
			pSock.SysMessage( GetDictionaryEntry( 2096, pSock.language ));
			ourObj.SetMoreVar( "morez", 1, 0 );
			pSock.SoundEffect( 0x241, false );// lockpick sounds

			// Start timer to re-activate dungeon trap, if it is one
			if( isDungeonTrap )
			{
				TriggerEvent( 2504, "DeactivateTrap", ourObj );
			}
		}
		else
		{
			if( RandomNumber( 0, 1 ))
			{
				// Oops.
				pSock.SysMessage( GetDictionaryEntry( 2097, pSock.language ));
				if( isDungeonTrap )
				{
					// sets dungeon trap off
					switch( ourObj.id )
					{
						case 0x11a0: // spike trap
							pUser.Teleport( ourObj );
							TriggerEvent( 2504, "onCollide", pSock, pUser, ourObj );
							break;
						default:
							break;
					}
				}
				else
				{
					// sets trap off
					TriggerTrap( pSock, ourObj );
				}
			}
			else
			{
				// You breathe a sigh of relief, as you fail to disarm the trap, but don't set it off.
				pSock.SysMessage( GetDictionaryEntry( 2098, pSock.language ));
			}
		}
	}
}
