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
			pSock.SysMessage( GetDictionaryEntry( 837, pSock.Language ) );
		else if( pUser.visible != 1 )
			pSock.SysMessage( GetDictionaryEntry( 836, pSock.Language ) );
		else if( pUser.skills.hiding < 700 )
		{
			pSock.SysMessage( GetDictionaryEntry( 838, pSock.Language ) );
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
			pSock.SysMessage( "You can move 10 steps unseen" );
			pUser.visible = 1;
			pUser.stealth = 0;
		}
	}
	return true;
}
