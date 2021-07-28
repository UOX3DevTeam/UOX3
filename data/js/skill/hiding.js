function SkillRegistration()
{
	RegisterSkill( 21, true );	// Hiding
}

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
	{
		if( pUser.isonhorse && !GetServerSetting( "HideWhileMounted" ))
		{
			pSock.SysMessage( GetDictionaryEntry( 831, pSock.language ) ); // You cannot hide while mounted!
		}
		else if( pUser.visible != 0 )
			pSock.SysMessage( GetDictionaryEntry( 833, pSock.language ) ); // You are already hidden.
		else if( pUser.attacker && pUser.InRange( pUser.attacker, 15 ) )
			pSock.SysMessage( GetDictionaryEntry( 832, pSock.language ) ); // You cannot hide while fighting.
		else if( !pUser.CheckSkill( 21, 0, 1000 ) )
			pSock.SysMessage( GetDictionaryEntry( 834, pSock.language ) ); // You are unable to hide here.
		else
		{
			pSock.SysMessage( GetDictionaryEntry( 835, pSock.language ) ); // You have hidden yourself well.
			pUser.visible = 1;
			pUser.stealth = -1;
		}
	}
	return true;
}


