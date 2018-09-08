function SkillRegistration()
{
	RegisterSkill( 32, true );	// Spirit Speak
}

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
	{
		if( !pUser.CheckSkill( 32, 0, 1000 ) )
			pSock.SysMessage( GetDictionaryEntry( 1501, pSock.Language ) );
		else
		{
			pUser.SoundEffect( 0x024A, true );
			pSock.SysMessage( GetDictionaryEntry( 1502, pSock.Language ) );
			pSock.SetTimer( 2, (1000 * (30 + (pUser.skills.spiritspeak / 10) + pUser.intelligence)) );
		}
	}
	return true;
}


