function SkillRegistration()
{
	RegisterSkill( 48, true );	// Remove Trap
}

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if ( pSock )
	{
		if ( pUser.skills[24] < 500 )
		{
			// You do not know enough about locks.  Become better at picking locks.
			pSock.SysMessage( GetDictionaryEntry( 2091, pSock.language ) );
		}
		else if ( pUser.skills[14] < 500 )
		{
			// You are not perceptive enough.  Become better at detect hidden.
			pSock.SysMessage( GetDictionaryEntry( 2092, pSock.language ) );
		}
		else
		{
			// Which trap will you attempt to disarm?
			pSock.CustomTarget( 0, GetDictionaryEntry( 2093, pSock.language ) );
		}
	}
	return true;
}

function onCallback0( pSock, ourObj )
{
	var pUser = pSock.currentChar;
	var iMoreZ = ourObj.morez;
	var iMoreZPart1 = ( iMoreZ >> 24 ); // Trap set
	var iMoreZPart2 = ( iMoreZ >> 16 ); // Trap Damage
	var iMoreZPart3 = ( iMoreZ >> 8 );  // Minskill
	var iMoreZPart4 = ( iMoreZ % 256 ); // Maxskill
	var isInRange = pUser.InRange( ourObj, 3 );

	pSock.tempObj = null;
	if ( !isInRange )
	{
		pSock.SysMessage(GetDictionaryEntry( 461, pSock.Language ) ); // You are too far away.
		return;
	}

	if ( !ValidateObject( ourObj ) || !ourObj.isItem )
	{
		// You feel that such an action would be inappropriate
		pSock.SysMessage( GetDictionaryEntry( 2094, pSock.language ) );
	}
	else if ( !pUser.CanSee( ourObj ) )
	{
		// You cannot see that
		pSock.SysMessage( GetDictionaryEntry( 1646, pSock.language) );
	}
	else if ( iMoreZPart1 == 0 )
	{
		// That doesn't appear to be trapped
		pSock.SysMessage( GetDictionaryEntry( 2095, pSock.language ) );
	}
	else
	{
		var skillCheckModifier = 0;
		var iGloves = pUser.FindItemLayer( 7 ); // hands layer
		if ( ValidateObject( iGloves ) )
		{
			skillCheckModifier = iGloves.weight;
		}
		if ( pUser.skills.removetrap >= iMoreZPart4 || pUser.CheckSkill( 48, ( iMoreZPart3 + skillCheckModifier ), 1000 ) )
		{
			// You successfully render the trap harmless
			pSock.SysMessage( GetDictionaryEntry( 2096, pSock.language ) );
			ourObj.morez = "0" + " " + iMoreZPart2.toString() + " " + iMoreZPart3.toString() + " " + iMoreZPart4.toString();
			pSock.SoundEffect( 0x241, false );// lockpick sounds
		}
		else
		{
			if ( RandomNumber( 0, 1 ) )
			{
				// Oops.
				pSock.SysMessage( GetDictionaryEntry( 2097, pSock.language ) );
				TriggerTrap( pSock, ourObj )// sets trap off
			}
			else
			{
				// You breathe a sigh of relief, as you fail to disarm the trap, but don't set it off.
				pSock.SysMessage( GetDictionaryEntry( 2098, pSock.language ) );
			}
		}
	}
}
