function SkillRegistration()
{
	RegisterSkill( 21, true );	// Hiding
}

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
	{
//		if( pUser.isonhorse )
//			pSock.SysMessage( GetDictionaryEntry( 831, pSock.Language ) );
//		else if( pUser.visible != 0 )
		if( pUser.visible != 0 )
			pSock.SysMessage( GetDictionaryEntry( 833, pSock.Language ) );
		else if( pUser.attacker && pUser.InRange( pUser.attacker, 15 ) )
			pSock.SysMessage( GetDictionaryEntry( 832, pSock.Language ) );
		else if( !pUser.CheckSkill( 21, 0, 1000 ) )
			pSock.SysMessage( GetDictionaryEntry( 834, pSock.Language ) );
		else
		{
			pSock.SysMessage( GetDictionaryEntry( 835, pSock.Language ) );
			pUser.visible = 1;
			pUser.stealth = -1;
		}
	}
	return true;
}


