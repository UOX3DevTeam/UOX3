function SkillRegistration()
{
	RegisterSkill( 21, true );	// Hiding
}

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
	{
		// Fetch skill-specific skill delay to use on failure
		var pSkillDelay = Skills[21].skillDelay;

		if( pUser.isonhorse && !GetServerSetting( "HideWhileMounted" ))
		{
			pSock.SysMessage( GetDictionaryEntry( 831, pSock.language )); // You cannot hide while mounted!
			pSock.SetTimer( Timer.SOCK_SKILLDELAY, pSkillDelay * 1000 );
		}
		else if( pUser.visible != 0 )
		{
			pSock.SysMessage( GetDictionaryEntry( 833, pSock.language )); // You are already hidden.
			pSock.SetTimer( Timer.SOCK_SKILLDELAY, pSkillDelay * 1000 );
		}
		else if( pUser.attacker && pUser.InRange( pUser.attacker, 15 ))
		{
			pSock.SysMessage( GetDictionaryEntry( 832, pSock.language )); // You cannot hide while fighting.
			pSock.SetTimer( Timer.SOCK_SKILLDELAY, 0.25 * 1000 ); // Put very small skilldelay for failing to hide because of being in combat
		}
		else if( !pUser.CheckSkill( 21, 0, 1000 ) )
		{
			pSock.SysMessage( GetDictionaryEntry( 834, pSock.language )); // You are unable to hide here.
			pSock.SetTimer( Timer.SOCK_SKILLDELAY, pSkillDelay * 1000 );
		}
		else
		{
			pSock.SysMessage( GetDictionaryEntry( 835, pSock.language )); // You have hidden yourself well.
			pUser.visible = 1;
			pUser.stealth = -1;

			// Use global skill delay on success
			var globalSkillDelay = GetServerSetting( "SKILLDELAY" );
			pSock.SetTimer( Timer.SOCK_SKILLDELAY, globalSkillDelay * 1000 );
		}
	}
	return true;
}


