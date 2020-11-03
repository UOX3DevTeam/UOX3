function SkillRegistration()
{
	RegisterSkill( 47, true );	// Stealth
}

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
	{
		if( pUser.isonhorse )
			pSock.SysMessage( GetDictionaryEntry( 837, pSock.language ) );
		else if( pUser.visible != 1 )
			pSock.SysMessage( GetDictionaryEntry( 836, pSock.language ) );
		else if( pUser.skills.hiding < 700 )
		{
			pSock.SysMessage( GetDictionaryEntry( 838, pSock.language ) );
			pUser.visible = 0;
			pUser.stealth = -1;
		}
		else if( !pUser.CheckSkill( 47, 0, 1000 ) )
		{
			pUser.visible = 0;
			pUser.stealth = -1;
		}
		else
		{
			pSock.SysMessage( GetDictionaryEntry( 839, pSock.language ) );
			pUser.visible = 1;
			pUser.stealth = 0;
		}
	}
	return true;
}
